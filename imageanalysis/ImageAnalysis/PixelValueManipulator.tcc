//# Copyright (C) 1998,1999,2000,2001,2003
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This program is free software; you can redistribute it and/or modify it
//# under the terms of the GNU General Public License as published by the Free
//# Software Foundation; either version 2 of the License, or (at your option)
//# any later version.
//#
//# This program is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
//# more details.
//#
//# You should have received a copy of the GNU General Public License along
//# with this program; if not, write to the Free Software Foundation, Inc.,
//# 675 Massachusetts Ave, Cambridge, MA 02139, USA.
//#
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//# $Id: $

#include <imageanalysis/ImageAnalysis/PixelValueManipulator.h>

#include <casa/Quanta/QuantumHolder.h>
#include <images/Images/ImageRegrid.h>
#include <lattices/LatticeMath/LatticeAddNoise.h>
#include <lattices/LatticeMath/LatticeSlice1D.h>
#include <lattices/Lattices/PixelCurve1D.h>

#include <imageanalysis/ImageAnalysis/ImageCollapser.h>

namespace casa {

template<class T> const casacore::String PixelValueManipulator<T>::_className = "PixelValueManipulator";

template<class T> PixelValueManipulator<T>::PixelValueManipulator(
    const SPCIIT image,
    const casacore::Record *const regionRec,
    const casacore::String& mask, casacore::Bool verboseDuringConstruction
) : ImageTask<T>(
    image, "", regionRec, "", "", "",
    mask, "", false
), _axes() {
    this->_construct(verboseDuringConstruction);
}

template<class T> void PixelValueManipulator<T>::addNoise(
    SPIIT image, const String& type, const Record& region,
    const Vector<Double>& pars, Bool zeroIt,
    const std::pair<Int, Int> *const &seeds
) {
    String mask;
    auto subImage = SubImageFactory<T>::createSubImageRW(
        *image, region, mask, nullptr
    );
    if (zeroIt) {
        subImage->set(0.0);
    }
    Random::Types typeNoise = Random::asType(type);
    std::shared_ptr<casacore::LatticeAddNoise> lan(
        seeds
        ? new LatticeAddNoise(typeNoise, pars, seeds->first, seeds->second)
        : new LatticeAddNoise(typeNoise, pars)
    );
    lan->add(*subImage);
}

template<class T> casacore::Record* PixelValueManipulator<T>::coordMeasures(
    casacore::Quantum<T>& intensity, casacore::Record& direction,
    casacore::Record& frequency, casacore::Record& velocity,
    SPCIIT image, const casacore::Vector<casacore::Double>& pixel,
    const casacore::String& dirFrame, const casacore::String& freqFrame
) {
    casacore::Record *r = nullptr;

    const auto& cSys = image->coordinates();

    casacore::Vector<casacore::Double> vpixel = pixel.empty() ? cSys.referencePixel() : pixel;

    casacore::String format("m");
    ImageMetaData<T> imd(image);
    r = new casacore::Record(imd.toWorld(vpixel, format, true, dirFrame, freqFrame));

    casacore::Vector<casacore::Int> ipixel(vpixel.size());
    convertArray(ipixel, vpixel);

    casacore::Bool offImage;
    casacore::Quantum<casacore::Double> value;
    casacore::Bool mask = false;
    PixelValueManipulator<T> pvm(image, nullptr, "");
    pvm.pixelValue(offImage, intensity, mask, ipixel);
    if (offImage) {
        return r;
    }

    r->define(casacore::RecordFieldId("mask"), mask);

    if (r->isDefined("direction")) {
        direction = r->asRecord("direction");
    }
    if (r->isDefined("spectral")) {
        casacore::Record specRec = r->asRecord("spectral");
        if (specRec.isDefined("frequency")) {
            frequency = specRec.asRecord("frequency");
        }
        if (specRec.isDefined("radiovelocity")) {
            velocity = specRec.asRecord("radiovelocity");
        }
    }
    return r;
}

template<class T> void PixelValueManipulator<T>::setAxes(
    const casacore::IPosition& axes, casacore::Bool invert
) {
    casacore::uInt ndim = this->_getImage()->ndim();
    ThrowIf(
        axes.nelements() > ndim,
        "Too many axes, image only has "
        + casacore::String::toString(ndim)
        + " dimensions"
    );
    if (! axes.empty()) {
        casacore::Vector<casacore::Int> t = axes.asVector();
        ThrowIf(
            max(t) >= (casacore::Int)ndim,
            "image does not have axis " + casacore::String::toString(max(t))
        );
        ThrowIf(
            min(t) < 0, "Axis cannot be negative"
        );
    }
    _axes = invert
        ? casacore::IPosition::otherAxes(ndim, axes)
        : axes;
}

template<class T> casacore::Record PixelValueManipulator<T>::get() const {
    SPCIIT subImage = SubImageFactory<T>::createSubImageRO(
        *this->_getImage(), *this->_getRegion(), this->_getMask(),
        (this->_getVerbosity() > ImageTask<T>::QUIET ? this->_getLog().get() : 0),
        casacore::AxesSpecifier(), this->_getStretch()
    );
    if (! _axes.empty()) {
        ImageCollapser<T> collapser(
            subImage, _axes, false, ImageCollapserData::MEAN,
            "", false
        );
        subImage = collapser.collapse();
    }
    casacore::Array<T> values = subImage->get(this->_getDropDegen());
    casacore::Array<casacore::Bool> mask = casacore::Array<casacore::Bool>(values.shape(), true);
    if (subImage->isMasked()) {
        mask = mask && subImage->getMask(this->_getDropDegen());
    }
    casacore::Record ret;
    ret.define("values", values);
    ret.define("mask", mask);
    return ret;
}

template<class T> Record PixelValueManipulator<T>::getProfile(
    uInt axis, ImageCollapserData::AggregateType function,
    const casacore::String& unit,
    PixelValueManipulatorData::SpectralType specType,
    const casacore::Quantity *const restFreq,
    const casacore::String& frame
) {
    return getProfile(
        axis, ImageCollapserData::minMatchMap()->at((uInt)function),
        unit,  specType, restFreq,  frame
    );
}

template<class T> Record PixelValueManipulator<T>::getProfile(
        casacore::uInt axis, const casacore::String& function,
    const casacore::String& unit,
    PixelValueManipulatorData::SpectralType specType,
    const casacore::Quantity *const restFreq, const casacore::String& frame
) {
    SPIIT collapsed;
    Operator op = NONE;
    if (function.contains("+")) {
        op = ADDITION;
    }
    else if (function.contains("-")) {
        op = SUBTRACTION;
    }
    else if (function.contains("*")) {
        op = MULTIPLICATION;
    }
    else if (function.contains("/")) {
        op = DIVISION;
    }
    if (op == NONE) {
        collapsed = _doSingle(axis, function);
    }
    else {
        collapsed = _doComposite(axis, function, op);
    }
    Record ret;
    auto values = collapsed->get(true);
    ret.define("values", values);
    Array<Bool> mask(values.shape(), true);
    if (collapsed->isMasked()) {
        mask = mask && collapsed->getMask(true);
    }
    if (collapsed->hasPixelMask()) {
        mask = mask && collapsed->pixelMask().get(true);
    }
    ret.define("mask", mask);
    ret.define("yUnit", collapsed->units().getName());
    ret.define("npix", _npts(axis));
    auto tunit = unit;
    tunit.downcase();
    Vector<Double> pix(collapsed->ndim(), 0);
    auto outputRef = collapsed->coordinates().toWorld(pix);
    auto inputRef = this->_getImage()->coordinates().referenceValue();
    inputRef[axis] = outputRef[axis];
    auto inputPixel = this->_getImage()->coordinates().toPixel(inputRef);
    Double count = floor(inputPixel[axis] + 0.5);
    auto length = values.shape()[0];
    Vector<Double> coords = indgen(length, count, 1.0);
    ret.define("planes", coords);
    if (tunit.startsWith("pix")) {
        ret.define("coords", coords);
        ret.define("xUnit", "pixel");
    }
    else {
        ret.merge(
            _doWorld(collapsed, unit, specType, restFreq, frame, axis)
        );
    }
    if (this->_getLogFile()) {
        auto axisName = this->_getImage()->coordinates().worldAxisNames()[axis];
        auto cAxis = axisName;
        cAxis.downcase();
        Quantity xunit(1, ret.asString("xUnit"));
        if (
            cAxis.startsWith("freq") && xunit.isConform(Unit("m/s"))
        ) {
            axisName = "Velocity";
        }
        auto imageName = this->_getImage()->name();
        std::ostringstream oss;
        oss << "#title: " << axisName
            << " profile - " << imageName << endl;
        if (! _regionName.empty()) {
            oss << "#region : " << _regionName << endl;
        }
        oss << "#xUnit " << xunit.getUnit() << endl;
        oss << "#yUnit " << ret.asString("yUnit") << endl;
        oss << "# " << imageName << endl << endl;
        casacore::Vector<T> data;
        ret.get("values", data);
        casacore::Vector<casacore::Bool> mask;
        ret.get("mask", mask);
        casacore::Vector<casacore::Double> xvals = ret.asArrayDouble("coords");
        auto citer = xvals.begin();
        auto miter = mask.begin();
        for (const auto& d : data) {
            if (*miter) {
                oss << fixed << setprecision(7) << *citer
                    << " " << setw(10) << d << endl;
            }
            ++miter;
            ++citer;
        }
        this->_writeLogfile(oss.str());
    }
    return ret;
}

template<class T> SPIIT PixelValueManipulator<T>::_doComposite(
        casacore::uInt axis, const casacore::String& function, Operator op
) const {
    auto x = op == ADDITION ? "+"
        : op == SUBTRACTION ? "-"
            : op == MULTIPLICATION ? "*"
                : "/";
    // have to make a copy, because after not const :(
    auto f = function;
    casacore::String func0 = f.before(x);
    func0.trim();
    casacore::String func1 = f.after(x);
    func1.trim();
    auto im0 = _doSingle(axis, func0);
    auto im1 = func0 == func1 ? im0 : _doSingle(axis, func1);
    SPIIT ret(new TempImage<T>(im0->shape(), im0->coordinates()));
    LatticeExpr<T> data = op == ADDITION ? *im0 + *im1
        : op == SUBTRACTION ? *im0 - *im1
            : op == MULTIPLICATION ? (*im0)*(*im1)
                : (*im0)/(*im1);
    ret->copyData(data);
    auto unit0 = im0->units();
    auto unit1 = im1->units();
    casacore::Unit outUnit;
    if (op == ADDITION || op == SUBTRACTION) {
        if (unit1 == unit0) {
            outUnit = unit0;
        }
        else {
            *this->_getLog() << LogIO::WARN
                << "Units incompatible for this operation, setting output "
                << "image brightness unit to empty" << LogIO::POST;
            outUnit = Unit();
        }
    }
    else {
        casacore::Quantity q0(1, unit0);
        casacore::Quantity q1(1, unit1);
        outUnit = op == MULTIPLICATION
            ? (q0*q1).getFullUnit() : (q0/q1).getFullUnit();
    }
    ret->setUnits(outUnit);
    return ret;
}

template<class T> SPIIT PixelValueManipulator<T>::_doSingle(
    casacore::uInt axis, const casacore::String& function
) const {
    ImageCollapser<T> collapser(
        function, this->_getImage(), this->_getRegion(),
        this->_getMask(), IPosition(1, axis), true, "", ""
    );
    collapser.setStretch(this->_getStretch());
    return collapser.collapse();
}

template<class T>
casacore::Vector<casacore::uInt> PixelValueManipulator<T>::_npts(
    casacore::uInt axis
) const {
    auto subim = SubImageFactory<T>::createSubImageRO(
        *this->_getImage(), *this->_getRegion(), this->_getMask(),
        this->_getLog().get(), AxesSpecifier(), this->_getStretch()
    );
    auto shape = subim->shape();
    casacore::uInt nvals = shape[axis];
    casacore::Vector<casacore::uInt> counts(nvals);
    if (subim->hasPixelMask() || subim->isMasked()) {
        casacore::IPosition begin(shape.size(), 0);
        auto sliceShape = shape;
        sliceShape[axis] = 1;
        for (casacore::uInt i=0; i<nvals; ++i, ++begin[axis]) {
            counts[i] = ntrue(subim->getMaskSlice(begin, sliceShape));
        }
    }
    else {
        // no mask, just return number of pixels in each plane
        counts = shape.removeAxes(casacore::IPosition(1, axis)).product();
    }
    return counts;
}


template<class T> casacore::Record* PixelValueManipulator<T>::getSlice(
    SPCIIT image, const casacore::Vector<casacore::Double>& x,
    const casacore::Vector<casacore::Double>& y,
    const casacore::Vector<casacore::Int>& axes,
    const casacore::Vector<casacore::Int>& coord, Int npts,
    const casacore::String& method
) {
    casacore::Vector<casacore::Float> xPos, yPos, distance;
    casacore::Vector<T> pixels;
    casacore::Vector<casacore::Bool> pixelMask;
    casacore::PixelCurve1D curve(x, y, npts);
    casacore::IPosition iCoord(coord);
    casacore::IPosition iAxes(axes);
    // Get the Slice
    auto method2 = casacore::LatticeSlice1D<T>::stringToMethod(method);
    casacore::LatticeSlice1D<T> slicer(*image, method2);
    slicer.getSlice(pixels, pixelMask, curve, iAxes(0), iAxes(1), iCoord);
    // Get slice locations
    casacore::uInt axis0, axis1;
    slicer.getPosition(axis0, axis1, xPos, yPos, distance);
    casacore::RecordDesc outRecDesc;
    outRecDesc.addField("pixel", asArray(image->dataType()));
    outRecDesc.addField("mask", TpArrayBool);
    outRecDesc.addField("xpos", TpArrayFloat);
    outRecDesc.addField("ypos", TpArrayFloat);
    outRecDesc.addField("distance", TpArrayFloat);
    outRecDesc.addField("axes", TpArrayInt);
    auto *outRec = new casacore::Record(outRecDesc);
    outRec->define("pixel", pixels);
    outRec->define("mask", pixelMask);
    outRec->define("xpos", xPos);
    outRec->define("ypos", yPos);
    outRec->define("distance", distance);
    outRec->define("axes", Vector<Int>(vector<uInt> {axis0, axis1}));
    return outRec;
}

template<class T> void PixelValueManipulator<T>::insert(
    casacore::ImageInterface<T>& target,
    const casacore::ImageInterface<T>& infile, const casacore::Record& region,
    const casacore::Vector<double>& locatePixel, casacore::Bool verbose
) {
    auto doRef = locatePixel.empty();
    casacore::Int dbg = 0;
    auto inSub = SubImageFactory<T>::createSubImageRO(
        infile, region, "",
        verbose
            ? std::unique_ptr<casacore::LogIO>(new casacore::LogIO()).get()
            : nullptr
    );

    // Generate output pixel location
    const auto inShape = inSub->shape();
    const auto outShape = target.shape();
    const auto nDim = target.ndim();
    casacore::Vector<casacore::Double> outPix(doRef ? 0 : nDim);
    const auto nDim2 = locatePixel.size();
    if (! doRef) {
        for (casacore::uInt i = 0; i < nDim; ++i) {
            outPix[i] = i < nDim2 ? locatePixel[i]
                : (outShape(i) - inShape(i)) / 2.0; // Centrally located
        }
    }
    casacore::ImageRegrid<T> ir;
    ir.showDebugInfo(dbg);
    ir.insert(target, outPix, *inSub);
}

template<class T> casacore::Record PixelValueManipulator<T>::_doWorld(
    SPIIT collapsed, const casacore::String& unit,
    PixelValueManipulatorData::SpectralType specType,
    const casacore::Quantity *const restFreq, const casacore::String& frame,
    casacore::uInt axis
) const {
    // drop degenerate axes
    SPIIT tmp = SubImageFactory<T>::createImage(
        *collapsed, "", casacore::Record(), "",
        casacore::AxesSpecifier(casacore::IPosition(1, axis)),
        false, false, false
    );
    const casacore::CoordinateSystem csys = tmp->coordinates();
    casacore::Quantity t(0, unit);
    casacore::String axisUnit = csys.worldAxisUnits()[0];
    if (! unit.empty()) {
        _checkUnit(unit, csys, specType);
    }
    casacore::uInt length = tmp->shape()[0];
    casacore::Vector<casacore::Double> coords(length);
    casacore::Matrix<casacore::Double> pixel;
    casacore::Matrix<casacore::Double> world;
    casacore::Vector<casacore::Bool> failures;
    if (
        ! frame.empty() && csys.hasSpectralAxis()
        && casacore::MFrequency::typeFromString(frame)
        != csys.spectralCoordinate().frequencySystem(true)
    ) {
        // We need to use the original coordinate system because we need
        // the direction coordinate to be able to set the spectral
        // conversion frame
        casacore::CoordinateSystem mycsys(this->_getImage()->coordinates());
        pixel.resize(casacore::IPosition(2, mycsys.nPixelAxes(), length));
        pixel.set(0);
        pixel.row(axis) = indgen(length, 0.0, 1.0);
        casacore::SpectralCoordinate spCoord = mycsys.spectralCoordinate();
        mycsys.setSpectralConversion(frame);
        ThrowIf (!
            mycsys.toWorldMany(world, pixel, failures),
            "Unable to convert spectral coordinates to " + frame
        );
        coords = world.row(axis);
    }
    else {
        pixel.resize(casacore::IPosition(2, 1, length));
        pixel.set(0);
        pixel.row(0) = indgen(length, 0.0, 1.0);
        ThrowIf(
            ! csys.toWorldMany(world, pixel, failures),
            "Unable to convert to world coordinates"
        );
        coords = world.row(0);
    }
    if (! unit.empty() && unit != axisUnit) {
        if (t.isConform(axisUnit)) {
            casacore::Quantum<casacore::Vector<casacore::Double>> q(
                coords, axisUnit
            );
            coords = q.getValue(unit);
        }
        else {
            _doNoncomformantUnit(
                coords, csys, unit, specType, restFreq, axisUnit
            );
        }
    }
    casacore::Record ret;
    ret.define("coords", coords);
    ret.define("xUnit", unit.empty() ? axisUnit : unit);
    return ret;
}

template<class T> void PixelValueManipulator<T>::_doNoncomformantUnit(
    casacore::Vector<casacore::Double>& coords,
    const casacore::CoordinateSystem& csys, const casacore::String& unit,
    PixelValueManipulatorData::SpectralType specType,
    const casacore::Quantity *const restFreq, const casacore::String& axisUnit
) const {
    ThrowIf(
        ! csys.hasSpectralAxis(),
        "Units must be conformant with" + axisUnit
    );
    casacore::SpectralCoordinate sp = csys.spectralCoordinate();
    if (restFreq) {
        casacore::Double value = restFreq->getValue(axisUnit);
        sp.setRestFrequency(value, false);
        sp.selectRestFrequency(value);
    }
    casacore::Quantity t(0, unit);
    if (t.isConform("m/s")) {
        casacore::MDoppler::Types doppler;
        if (
            specType == PixelValueManipulatorData::DEFAULT
            || specType == PixelValueManipulatorData::RELATIVISTIC
        ) {
            doppler = casacore::MDoppler::RELATIVISTIC;
        }
        else if (specType == PixelValueManipulatorData::RADIO_VELOCITY) {
            doppler = casacore::MDoppler::RADIO;
        }
        else if (specType == PixelValueManipulatorData::OPTICAL_VELOCITY) {
            doppler = casacore::MDoppler::OPTICAL;
        }
        else {
            ThrowCc("Spectral type not compatible with velocity units");
        }
        sp.setVelocity(unit ,doppler);
        sp.frequencyToVelocity(coords, coords);
    }
    else {
        // unit must be conformant with meters
        sp.setWavelengthUnit(unit);
        if (
            specType == PixelValueManipulatorData::DEFAULT
            || specType == PixelValueManipulatorData::WAVELENGTH
        ) {
            sp.frequencyToWavelength(coords, coords);
        }
        else if (specType == PixelValueManipulatorData::AIR_WAVELENGTH) {
            sp.frequencyToAirWavelength(coords, coords);
        }
    }
}

template<class T> void PixelValueManipulator<T>::_checkUnit(
    const casacore::String& unit, const casacore::CoordinateSystem& csys,
    PixelValueManipulatorData::SpectralType specType
) const {
    casacore::Quantity t(0, unit);
    casacore::String axisUnit = csys.worldAxisUnits()[0];
    if (! t.isConform(axisUnit)) {
        if (csys.hasSpectralAxis()) {
            ThrowIf(
                ! (t.isConform("m/s") || t.isConform("m")),
                "Invalid spectral conversion unit " + unit
            );
            ThrowIf(
                t.isConform("m/s")
                && (
                    specType == PixelValueManipulatorData::WAVELENGTH
                    || specType == PixelValueManipulatorData::AIR_WAVELENGTH
                ),
                "Inconsistent spectral type used for velocity units"
            );
            ThrowIf(
                t.isConform("m")
                && (
                    specType == PixelValueManipulatorData::OPTICAL_VELOCITY
                    || specType == PixelValueManipulatorData::RADIO_VELOCITY
                ),
                "Inconsistent spectral type used for wavelength units"
            );
        }
        else {
            ThrowCc(
                "Unit " + unit
                + " does not conform to corresponding axis unit "
                + axisUnit
            );
        }
    }
}

template<class T> void PixelValueManipulator<T>::put(
    SPIIT image, const casacore::Array<T>& pixelsArray,
    const casacore::Vector<casacore::Int>& blc,
    const casacore::Vector<casacore::Int>& inc,
    casacore::Bool list, casacore::Bool locking, casacore::Bool replicate
) {
    casacore::IPosition imageShape = image->shape();
    casacore::uInt ndim = imageShape.size();
    ThrowIf(
        pixelsArray.ndim() > ndim,
        "Pixel array cannot have more dimensions than the image!"
    );

    // Verify blc value. Fill in values for blc and inc.  trc set to shape-1
    casacore::IPosition iblc = casacore::IPosition(blc);
    casacore::IPosition itrc = imageShape - 1;
    casacore::IPosition iinc(inc.size());
    for (casacore::uInt i = 0; i < inc.size(); i++) {
        iinc(i) = inc[i];
    }
    casacore::LCBox::verify(iblc, itrc, iinc, imageShape);

    // Create two slicers; one describing the region defined by blc + shape-1
    // with extra axes given length 1. The other we extend with the shape
    casacore::IPosition len = pixelsArray.shape();
    len.resize(ndim, true);
    for (casacore::uInt i = pixelsArray.shape().nelements(); i < ndim; i++) {
        len(i) = 1;
        itrc(i) = imageShape(i) - 1;
    }
    casacore::Slicer sl(iblc, len, iinc, casacore::Slicer::endIsLength);
    ThrowIf(
        sl.end() + 1 > imageShape,
        "Pixels array, including inc, extends beyond edge of image."
    );
    casacore::Slicer sl2(iblc, itrc, iinc, casacore::Slicer::endIsLast);

    if (list) {
        casacore::LogIO log;
        log << casacore::LogOrigin("PixelValueManipulator", __func__)
            << casacore::LogIO::NORMAL << "Selected bounding box " << sl.start()
            << " to " << sl.end() << casacore::LogIO::POST;
    }
    // Put the pixels
    if (pixelsArray.ndim() == ndim) {
        // _setCache(pixelsArray.shape());
        if (replicate) {
            casacore::LatticeUtilities::replicate(*image, sl2, pixelsArray);
        }
        else {
            image->putSlice(pixelsArray, iblc, iinc);
        }
    }
    else {
        // Pad with extra degenerate axes if necessary (since it is somewhat
        // costly).
        casacore::Array<T> pixelsref(
            pixelsArray.addDegenerate(
                ndim - pixelsArray.ndim()
            )
        );
        // _setCache(pixelsref.shape());
        if (replicate) {
            casacore::LatticeUtilities::replicate(*image, sl2, pixelsref);
        }
        else {
            image->putSlice(pixelsref, iblc, iinc);
        }
    }
    // Ensure that we reconstruct the  histograms objects
    // now that the data have changed
    if (locking) {
        image->unlock();
    }
}

template<class T> Bool PixelValueManipulator<T>::putRegion(
    SPIIT image, const casacore::Array<T>& pixels,
    const casacore::Array<casacore::Bool>& mask, casacore::Record& region,
    casacore::Bool list, casacore::Bool usemask, casacore::Bool replicateArray
) {
    // used to verify array dimension
    auto imageNDim = image->ndim();
    // Checks on pixels dimensions
    auto pixelShape = pixels.shape();
    auto pixelNDim = pixels.ndim();
    ThrowIf(
        pixelNDim > imageNDim,
        "Pixels array has more axes than the image"
    );
    for (const auto& length: pixelShape) {
        ThrowIf(
            length <= 0,
            "The shape of the pixels array is invalid"
        );
    }
    // Checks on pixelmask dimensions
    auto maskShape = mask.shape().asVector();
    auto maskNDim = mask.ndim();
    ThrowIf(
        maskNDim > imageNDim,
        "Mask array has more axes than the image"
    );
    for (const auto& length: maskShape) {
        ThrowIf(
            length <= 0,
            "The shape of the pixelmask array is invalid"
        );
    }
    // Warning, an empty casacore::Array comes through the Glish tasking system
    // as shape = [0], ndim = 1, nelements = 0
    casacore::IPosition dataShape;
    casacore::uInt dataDim = 0;
    auto pixelElements = pixels.size();
    auto maskElements = mask.size();
    if (pixelElements != 0 && maskElements != 0) {
        ThrowIf(
            ! pixels.shape().isEqual(mask.shape()),
            "Pixels and mask arrays have different shapes"
        );
        if (pixelElements != 0) {
            dataShape = pixelShape;
            dataDim = pixelNDim;
        }
        else {
            dataShape = mask.shape();
            dataDim = maskNDim;
        }
    }
    else if (pixelElements != 0) {
        dataShape = pixelShape;
        dataDim = pixelNDim;
    }
    else if (maskElements != 0) {
        dataShape = mask.shape();
        dataDim = maskNDim;
    }
    else {
        ThrowCc("Pixels and mask arrays are both zero length");
    }

    // Make region.  If the region extends beyond the image, it is
    // truncated here.
    casacore::LogIO mylog;
    const auto& csys = image->coordinates();
    const auto imShape = image->shape();
    std::unique_ptr<const casacore::ImageRegion> pRegion(
        casacore::ImageRegion::fromRecord(
            (list ? &mylog : nullptr), csys, imShape, region
        )
    );
    casacore::LatticeRegion latRegion = pRegion->toLatticeRegion(
        csys, imShape
    );
    // The pixels array must be same shape as the bounding box of the
    // region for as many axes as there are in the pixels array.  We
    // pad with degenerate axes for missing axes. If the region
    // dangled over the edge, it will have been truncated and the
    // array will no longer be the correct shape and we get an error.
    // We could go to the trouble of fishing out the bit that doesn't
    // fall off the edge.
    auto latRegionShape = latRegion.shape();
    for (casacore::uInt i = 0; i < dataDim; ++i) {
        if (dataShape[i] != latRegionShape[i]) {
            if (!(i == dataDim - 1 && dataShape[i] == 1)) {
                ostringstream oss;
                oss << "Data array shape (" << dataShape
                    << ") including inc, does not"
                    << " match the shape of the region bounding box ("
                    << latRegionShape << ")" << endl;
                ThrowCc(casacore::String(oss));
            }
        }
    }
    // If our image doesn't have a mask, try and make it one.
    if (maskElements > 0) {
        if (! image->hasPixelMask()) {
            casacore::String maskName("");
            ImageMaskAttacher::makeMask(
                *image, maskName, true, true, mylog, list
            );
        }
    }
    if (! image->isMasked()) {
        usemask = false;
    }
    // Put the mask first
    if (maskElements > 0 && image->hasPixelMask()) {
        casacore::Lattice<casacore::Bool>& maskOut = image->pixelMask();
        if (maskOut.isWritable()) {
            if (dataDim == imageNDim) {
                if (replicateArray) {
                    casacore::LatticeUtilities::replicate(
                        maskOut, latRegion.slicer(),
                        mask
                    );
                }
                else {
                    maskOut.putSlice(mask, latRegion.slicer().start());
                }
            }
            else {
                mylog << casacore::LogIO::NORMAL
                    << "Padding mask array with degenerate axes"
                    << casacore::LogIO::POST;
                casacore::Array<casacore::Bool> maskref(
                    mask.addDegenerate(imageNDim - mask.ndim())
                );
                if (replicateArray) {
                    casacore::LatticeUtilities::replicate(
                        maskOut, latRegion.slicer(),
                        maskref
                    );
                }
                else {
                    maskOut.putSlice(maskref, latRegion.slicer().start());
                }
            }
        }
        else {
            ThrowCc(
                "The mask is not writable. Probably an ImageExpr or SubImage"
            );
        }
    }
    // Get the mask and data from disk if we need it
    casacore::Array<casacore::Bool> oldMask;
    casacore::Array<T> oldData;
    casacore::Bool deleteOldMask, deleteOldData, deleteNewData;
    const casacore::Bool* pOldMask = nullptr;
    const T* pOldData = nullptr;
    const T* pNewData = nullptr;
    if (pixelElements > 0 && usemask) {
        if (pixelNDim != imageNDim) {
            pixelShape.append(casacore::IPosition(imageNDim - pixelNDim, 1));
        }
        oldData = image->getSlice(
            latRegion.slicer().start(), pixelShape, false
        );
        oldMask = image->getMaskSlice(
            latRegion.slicer().start(), pixelShape, false
        );
        pOldData = oldData.getStorage(deleteOldData); // From disk
        pOldMask = oldMask.getStorage(deleteOldMask); // From disk
        pNewData = pixels.getStorage(deleteNewData); // From user
    }
    // Put the pixels
    if (dataDim == imageNDim) {
        if (pixelElements > 0) {
            if (usemask) {
                Bool deleteNewData2;
                Array<T> pixels2(pixelShape);
                T* pNewData2 = pixels2.getStorage(deleteNewData2);
                for (casacore::uInt i = 0; i < pixels2.nelements(); i++) {
                    pNewData2[i] = pNewData[i]; // Value user gives
                    if (!pOldMask[i]) {
                        pNewData2[i] = pOldData[i]; // Value on disk
                    }
                }
                pixels2.putStorage(pNewData2, deleteNewData2);
                if (replicateArray) {
                    LatticeUtilities::replicate(
                        *image, latRegion.slicer(), pixels2
                    );
                }
                else {
                    image->putSlice(pixels2, latRegion.slicer().start());
                }
            }
            else {
                if (replicateArray) {
                    casacore::LatticeUtilities::replicate(
                        *image, latRegion.slicer(),
                        pixels
                    );
                }
                else {
                    image->putSlice(pixels, latRegion.slicer().start());
                }
            }
        }
    }
    else {
        if (pixelElements > 0) {
            mylog << casacore::LogIO::NORMAL
                << "Padding pixels array with degenerate axes"
                << casacore::LogIO::POST;
            //
            if (usemask) {
                Bool deleteNewData2;
                Array<T> pixels2(pixelShape);
                T* pNewData2 = pixels2.getStorage(deleteNewData2);
                for (uInt i = 0; i < pixels2.nelements(); i++) {
                    pNewData2[i] = pNewData[i]; // Value user gives
                    if (!pOldMask[i]) {
                        pNewData2[i] = pOldData[i]; // Value on disk
                    }
                }
                pixels2.putStorage(pNewData2, deleteNewData2);
                if (replicateArray) {
                    casacore::LatticeUtilities::replicate(
                        *image, latRegion.slicer(), pixels2
                    );
                }
                else {
                    image->putSlice(pixels2, latRegion.slicer().start());
                }
            }
            else {
                Array<T> pixelsref(
                    pixels.addDegenerate(imageNDim - pixels.ndim())
                );
                if (replicateArray) {
                    casacore::LatticeUtilities::replicate(
                        *image, latRegion.slicer(), pixelsref
                    );
                }
                else {
                    image->putSlice(pixelsref, latRegion.slicer().start());
                }
            }
        }
    }
    if (pOldMask != 0) {
        oldMask.freeStorage(pOldMask, deleteOldMask);
    }
    if (pOldData != 0) {
        oldData.freeStorage(pOldData, deleteOldData);
    }
    if (pNewData != 0) {
        pixels.freeStorage(pNewData, deleteNewData);
    }
    return true;
}

template<class T> casacore::Bool PixelValueManipulator<T>::set(
    SPIIF image, const casacore::String& lespixels,
    const casacore::Int pixelmask, casacore::Record& p_Region,
    const casacore::Bool list
) {
    casacore::LogIO mylog;
    mylog << casacore::LogOrigin(_className, __func__);
    auto setPixels = ! lespixels.empty();
    auto pixels = setPixels ? lespixels : "0.0";
    auto setMask = pixelmask != -1;
    auto mask = setMask ? pixelmask > 0 : true;
    if (!setPixels && !setMask) {
        mylog << casacore::LogIO::WARN << "Nothing to do"
            << casacore::LogIO::POST;
        return false;
    }
    casacore::Record tempRegions;
    // Try and make a mask if we need one.
    if (setMask && ! image->isMasked()) {
        casacore::String maskName("");
        ImageMaskAttacher::makeMask(*image, maskName, true, true, mylog, list);
    }
    // Make region and subimage
    std::unique_ptr<casacore::Record> tmpRegion(new casacore::Record(p_Region));
    std::unique_ptr<const casacore::ImageRegion> pRegion(
        casacore::ImageRegion::fromRecord(
            (list ? &mylog : 0), image->coordinates(), image->shape(),
            *tmpRegion
        )
    );
    casacore::SubImage<casacore::Float> subImage(*image, *pRegion, true);
    // Set the pixels
    if (setPixels) {
        // Get casacore::LatticeExprNode (tree) from parser
        // Convert the GlishRecord containing regions to a
        // casacore::PtrBlock<const casacore::ImageRegion*>.
        ThrowIf(
            pixels.empty(), "You must specify an expression"
        );
        casacore::Block<casacore::LatticeExprNode> temps;
        casacore::String exprName;
        casacore::PtrBlock<const casacore::ImageRegion*> tempRegs;
        makeRegionBlock(tempRegs, tempRegions);
        auto node = casacore::ImageExprParse::command(pixels, temps, tempRegs);
        // Delete the ImageRegions
        makeRegionBlock(tempRegs, casacore::Record());
        // We must have a scalar expression
        ThrowIf(
            ! node.isScalar(), "The pixels expression must be scalar"
        );
        ThrowIf(
            node.isInvalidScalar(),
            "The scalar pixels expression is invalid"
        );
        auto node2 = toFloat(node);
        // if region==T (good) set value given by pixel expression, else
        // leave the pixels as they are
        auto region = subImage.region();
        casacore::LatticeExprNode node3(
            iif(region, node2.getFloat(), subImage)
        );
        subImage.copyData(casacore::LatticeExpr<casacore::Float> (node3));
    }
    // Set the mask
    if (setMask) {
        auto& pixelMask = subImage.pixelMask();
        auto region = subImage.region();
        // if region==T (good) set value given by "mask", else
        // leave the pixelMask as it is
        casacore::LatticeExprNode node4(iif(region, mask, pixelMask));
        pixelMask.copyData(casacore::LatticeExpr<casacore::Bool> (node4));
    }
    return true;
}

template<class T> void PixelValueManipulator<T>::makeRegionBlock(
    casacore::PtrBlock<const casacore::ImageRegion*>& regions,
    const casacore::Record& Regions
) {
    auto n = regions.size();
    for (casacore::uInt j=0; j<n; ++j) {
        delete regions[j];
    }
    regions.resize(0, true, true);
    casacore::uInt nreg = Regions.nfields();
    if (nreg > 0) {
        regions.resize(nreg);
        regions.set(static_cast<casacore::ImageRegion*> (0));
        for (casacore::uInt i=0; i<nreg; ++i) {
            regions[i] = casacore::ImageRegion::fromRecord(
                Regions.asRecord(i), ""
            );
        }
    }
}

template<class T> casacore::Record PixelValueManipulator<T>::pixelValue(
    const casacore::Vector<casacore::Int>& pixel
) const {
    casacore::Bool offImage;
    casacore::Quantum<T> value;
    casacore::Bool mask;
    casacore::Vector<casacore::Int> pos(pixel);
    pixelValue(offImage, value, mask, pos);
    if (offImage) {
        return casacore::Record();
    }
    casacore::RecordDesc outRecDesc;
    outRecDesc.addField("mask", TpBool);
    outRecDesc.addField("value", TpRecord);
    outRecDesc.addField("pixel", TpArrayInt);
    casacore::Record outRec(outRecDesc);
    outRec.define("mask", mask);
    casacore::String error;
    casacore::QuantumHolder qh(value);
    casacore::Record qr;
    ThrowIf(
        ! qh.toRecord(error, qr),
        "Unable to convert QuantumHolder to Record " + error
    );
    outRec.defineRecord("value", qr);
    outRec.define("pixel", pos);
    return outRec;
}

template<class T> void PixelValueManipulator<T>::pixelValue (
    casacore::Bool& offImage, casacore::Quantum<T>& value, casacore::Bool& mask,
    casacore::Vector<casacore::Int>& pos
) const {
    const auto myim = this->_getImage();
    const auto imShape = myim->shape();
    const auto refPix = myim->coordinates().referencePixel();
    const auto nDim = myim->ndim();
    if (pos.size() == 1 && pos[0] == -1) { // check for default input parameter
        pos.resize(nDim);
        for (casacore::uInt i = 0; i < nDim; ++i) {
            pos[i] = casacore::Int(refPix[i] + 0.5);
        }
    }
    casacore::IPosition iPos = casacore::IPosition(pos);
    const casacore::uInt nPix = iPos.nelements();
    iPos.resize(nDim, true);

    // Discard extra pixels, add ref pixel for missing ones
    offImage = false;
    for (casacore::uInt i = 0; i < nDim; ++i) {
        if ((i + 1) > nPix) {
            iPos[i] = casacore::Int(refPix[i] + 0.5);
        }
        else {
            if (iPos(i) < 0 || iPos[i] > (imShape[i] - 1)) {
                offImage = true;
            }
        }
    }
    if (offImage) {
        return;
    }
    casacore::IPosition shape(myim->ndim(), 1);
    auto pixel = myim->getSlice(iPos, shape);
    auto maskPixel = myim->getMaskSlice(iPos, shape);
    auto units = myim->units();
    if (pos.size() != iPos.size()) {
        pos.resize(iPos.size());
    }
    auto n = pos.size();
    for (casacore::uInt i = 0; i < n; i++) {
        pos(i) = iPos(i);
    }
    value = casacore::Quantum<T> (pixel(shape - 1), units);
    mask = maskPixel(shape - 1);
}

}
