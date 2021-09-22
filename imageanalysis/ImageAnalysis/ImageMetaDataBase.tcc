//# Copyright (C) 2009
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This library is free software; you can redistribute it and/or modify it
//# under the terms of the GNU Library General Public License as published by
//# the Free Software Foundation; either version 2 of the License, or (at your
//# option) any later version.
//#
//# This library is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
//# License for more details.
//#
//# You should have received a copy of the GNU Library General Public License
//# along with this library; if not, write to the Free Software Foundation,
//# Inc., 675 Massachusetts Ave, Cambridge, MA 02139, USA.
//#
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#

#ifndef IMAGEANALYSIS_IMAGEMETADATABASE_TCC
#define IMAGEANALYSIS_IMAGEMETADATABASE_TCC

#include <imageanalysis/ImageAnalysis/ImageMetaDataBase.h>

#include <casacore/casa/aips.h>

#include <casacore/casa/Arrays/ArrayLogical.h>
#include <casacore/casa/Containers/ValueHolder.h>
#include <casacore/casa/Quanta/QuantumHolder.h>
#include <casacore/casa/Utilities/DataType.h>
#include <casacore/images/Images/ImageSummary.h>
#include <casacore/images/Images/ImageStatistics.h>
#include <casacore/measures/Measures/MeasureHolder.h>
#include <casacore/casa/Utilities/Regex.h>

#include <iostream>
#include <iomanip>

#define _ORIGINB LogOrigin("ImageMetaDataBase", __func__, WHERE)

using namespace std;

using namespace casacore;

namespace casa {

template <class T> ImageMetaDataBase<T>::ImageMetaDataBase(SPCIIT image)
    : _image(image), _log(), _shape() {
    ThrowIf(! _image, "image cannot be NULL");
    _shape = _image->shape();
}

template <class T> Record ImageMetaDataBase<T>::_makeHeader() const {
    Record header;
    header.define(ImageMetaDataConstants::_IMTYPE, _getImType());
    header.define(ImageMetaDataConstants::_OBJECT, _getObject());
    const auto& csys = _getCoords();

    if (csys.hasDirectionCoordinate()) {
        const DirectionCoordinate& dc = csys.directionCoordinate();
        String equinox = MDirection::showType(
                dc.directionType()
        );
        header.define(ImageMetaDataConstants::_EQUINOX, _getEquinox());
        header.define(ImageMetaDataConstants::_PROJECTION, _getProjection());
    }
    header.define(ImageMetaDataConstants::_OBSDATE, _getEpochString());
    header.define(ImageMetaDataConstants::MASKS, _getMasks());
    header.define(ImageMetaDataConstants::_OBSERVER, _getObserver());
    header.define(ImageMetaDataConstants::_SHAPE, _getShape().asVector());
    header.define(ImageMetaDataConstants::_TELESCOPE, _getTelescope());
    header.define(ImageMetaDataConstants::_BUNIT, _getBrightnessUnit());
    if (csys.hasSpectralAxis()) {
        const SpectralCoordinate& sc = csys.spectralCoordinate();
        header.define(ImageMetaDataConstants::_RESTFREQ, sc.restFrequencies());
        header.define(
            ImageMetaDataConstants::_REFFREQTYPE , _getRefFreqType()
        );
    }
    const auto& info = _getInfo();
    if (info.hasSingleBeam()) {
        GaussianBeam beam = _getBeam();
        header.defineRecord(
            ImageMetaDataConstants::_BEAMMAJOR,
            QuantumHolder(beam.getMajor()).toRecord()
        );
        header.defineRecord(
            ImageMetaDataConstants::_BEAMMINOR,
            QuantumHolder(beam.getMinor()).toRecord()
        );
        header.defineRecord(
            ImageMetaDataConstants::_BEAMPA,
            QuantumHolder(beam.getPA(true)).toRecord()
        );
    }
    else if (info.hasMultipleBeams()) {
        String error;
        Record rec;
        info.toRecord(error, rec);
        static const String recName = "perplanebeams";
        Record beamRec = rec.asRecord(recName);
        beamRec.defineRecord(
            "median area beam", info.getBeamSet().getMedianAreaBeam().toRecord()
        );
        header.defineRecord(recName, beamRec);
    }
    auto cdelt = _getIncrements();
    auto units = _getAxisUnits();
    auto crpix = _getRefPixel();
    auto crval = _getRefValue();
    auto types = _getAxisNames();
    header.merge(_getStatistics());
    for (uInt i=0; i<cdelt.size(); ++i) {
        auto iString = String::toString(i + 1);
        auto delt = ImageMetaDataConstants::_CDELT + iString;
        header.define(delt, cdelt[i].getValue());
        auto unit = ImageMetaDataConstants::_CUNIT + iString;
        header.define(unit, units[i]);
        auto pix = ImageMetaDataConstants::_CRPIX + iString;
        header.define(pix, crpix[i]);
        auto val = ImageMetaDataConstants::_CRVAL + iString;
        header.define(val, crval[i].getValue());
        auto type = ImageMetaDataConstants::_CTYPE + iString;
        header.define(type, types[i]);
    }
    return header;
}

template <class T> const TableRecord ImageMetaDataBase<T>::_miscInfo() const {
    return _image->miscInfo();
}

template <class T> CoordinateSystem ImageMetaDataBase<T>::coordsys(
    const std::vector<Int>& pixelAxes
) const {
    // Recover CoordinateSytem into a Record
    auto cSys = _getCoords();
    if (pixelAxes.empty()) {
        return cSys;
    }
    Record rec;
    CoordinateSystem cSys2;
    // Fish out the coordinate of the desired axes
    uInt j = 0;
    const Int nPixelAxes = cSys.nPixelAxes();
    Vector<uInt> coordinates(cSys.nCoordinates(), uInt(0));
    Int coord, axisInCoord;
    for (const auto& axis: pixelAxes) {
        ThrowIf (
            axis < 0 || axis >= nPixelAxes,
            "Specified zero-based pixel axis " + String::toString(axis)
            + " is not a valid pixel axis"
        );
        cSys.findPixelAxis(coord, axisInCoord, uInt(axis));
        ThrowIf(
            coord < 0,
            "Zero-based pixel axis " + String::toString(axis)
            + " has been removed"
        );
        coordinates(coord)++;
        // Copy desired coordinate (once)
        if (coordinates(coord) == 1) {
            cSys2.addCoordinate(cSys.coordinate(coord));
        }
    }
    // Find mapping.  Says where world axis i in cSys is in cSys2
    Vector<Int> worldAxisMap, worldAxisTranspose;
    Vector<Bool> refChange;
    ThrowIf(
        ! cSys2.worldMap(worldAxisMap, worldAxisTranspose, refChange, cSys),
        "Error finding world map because " + cSys2.errorMessage()
    );
    // Generate list of world axes to keep
    Vector<Int> keepList(cSys.nWorldAxes());
    Vector<Double> worldReplace;
    j = 0;
    for (const auto& axis: pixelAxes) {
        if (axis >= 0 && axis < nPixelAxes) {
            Int worldAxis = cSys.pixelAxisToWorldAxis(uInt(axis));
            ThrowIf(
                worldAxis < 0,
                "World axis corresponding to zero-based pixel axis "
                + String::toString(axis) + " has been removed"
            );
            keepList(j++) = worldAxisMap(worldAxis);
        }
    }
    // Remove unwanted world (and pixel) axes.  Better would be to just
    // remove the pixel axes and leave the world axes there...
    if (j > 0) {
        keepList.resize(j, true);
        CoordinateUtil::removeAxes(cSys2, worldReplace, keepList, false);
    }
    // Copy the ObsInfo
    cSys2.setObsInfo(cSys.obsInfo());
    return cSys2;
}

template <class T> DataType ImageMetaDataBase<T>::dataType() const {
    return _image->dataType();
}

template <class T> Record* ImageMetaDataBase<T>::getBoundingBox(
    const Record& region
) const {
    const auto& csys = _getCoords();
    const auto shape = _getShape();
    const unique_ptr<ImageRegion> pRegion(
        ImageRegion::fromRecord(
            nullptr, csys, shape, region
        )
    );
    LatticeRegion latRegion = pRegion->toLatticeRegion(
        csys, shape
    );
    Slicer sl = latRegion.slicer();
    IPosition blc(sl.start());
    IPosition trc(sl.end());
    IPosition inc(sl.stride());
    IPosition length(sl.length());
    std::unique_ptr<Record> outRec(new Record());
    outRec->define("blc", blc.asVector());
    outRec->define("trc", trc.asVector());
    outRec->define("inc", inc.asVector());
    outRec->define("bbShape", (trc - blc + 1).asVector());
    outRec->define("regionShape", length.asVector());
    outRec->define("imageShape", shape.asVector());
    outRec->define("blcf", CoordinateUtil::formatCoordinate(blc, csys)); // 0-rel for use in C++
    outRec->define("trcf", CoordinateUtil::formatCoordinate(trc, csys));
    return outRec.release();
}

template <class T> ValueHolder ImageMetaDataBase<T>::getFITSValue(const String& key) const {
    String c = key;
    c.downcase();
    const TableRecord info = _miscInfo();
    if (c == ImageMetaDataConstants::_BUNIT) {
        return ValueHolder(_getBrightnessUnit());
    }
    else if (
        c.startsWith(ImageMetaDataConstants::_CDELT) || c.startsWith(ImageMetaDataConstants::_CRPIX)
        || c.startsWith(ImageMetaDataConstants::_CRVAL) || c.startsWith(ImageMetaDataConstants::_CTYPE)
        || c.startsWith(ImageMetaDataConstants::_CUNIT)
    ) {
        String prefix = c.substr(0, 5);
        uInt n = _getAxisNumber(c);
        if (prefix == ImageMetaDataConstants::_CDELT) {
            return ValueHolder(
                QuantumHolder(
                    _getIncrements()[n-1]
                ).toRecord()
            );
        }
        else if (prefix == ImageMetaDataConstants::_CRPIX) {
            return ValueHolder(_getRefPixel()[n-1]);
        }
        else if (prefix == ImageMetaDataConstants::_CRVAL) {
            if (_getCoords().polarizationAxisNumber(false) == (Int)(n-1)) {
                return ValueHolder(
                    _getStokes()
                );
            }
            else {
                return ValueHolder(
                    QuantumHolder(_getRefValue()[n-1]).toRecord()
                );
            }
        }
        else if (prefix == ImageMetaDataConstants::_CTYPE) {
            return ValueHolder(_getAxisNames()[n-1]);
        }
        else if (prefix == ImageMetaDataConstants::_CUNIT) {
            return ValueHolder(_getAxisUnits()[n-1]);
        }
    }
    else if (c == ImageMetaDataConstants::_EQUINOX) {
        return ValueHolder(_getEquinox());
    }
    else if (c == ImageMetaDataConstants::_IMTYPE) {
        return ValueHolder(_getImType());
    }
    else if (c == ImageMetaDataConstants::MASKS) {
        return ValueHolder(_getMasks());
    }
    else if (c == ImageMetaDataConstants::_OBJECT) {
        return ValueHolder(_getObject());
    }
    else if (c == ImageMetaDataConstants::_OBSDATE || c == ImageMetaDataConstants::_EPOCH) {
        return ValueHolder(_getEpochString());
    }
    else if (c == ImageMetaDataConstants::_OBSERVER) {
        return ValueHolder(_getObserver());
    }
    else if (c == ImageMetaDataConstants::_PROJECTION) {
        return ValueHolder(_getProjection());
    }
    else if (c == ImageMetaDataConstants::_REFFREQTYPE) {
        return ValueHolder(_getRefFreqType());
    }
    else if (c == ImageMetaDataConstants::_RESTFREQ) {
        return ValueHolder(
            QuantumHolder(_getRestFrequency()).toRecord()
        );
    }
    else if (c == ImageMetaDataConstants::_SHAPE) {
        return ValueHolder(_getShape().asVector());
    }
    else if (c == ImageMetaDataConstants::_TELESCOPE) {
        return ValueHolder(_getTelescope());
    }
    else if (
        c == ImageMetaDataConstants::_BEAMMAJOR || c == ImageMetaDataConstants::_BEAMMINOR || c == ImageMetaDataConstants::_BEAMPA
        || c == ImageMetaDataConstants::_BMAJ || c == ImageMetaDataConstants::_BMIN || c == ImageMetaDataConstants::_BPA
    ) {
        GaussianBeam beam = _getBeam();
        if (c == ImageMetaDataConstants::_BEAMMAJOR || c == ImageMetaDataConstants::_BMAJ) {
            return ValueHolder(QuantumHolder(beam.getMajor()).toRecord());
        }
        else if (c == ImageMetaDataConstants::_BEAMMINOR || c == ImageMetaDataConstants::_BMIN) {
            return ValueHolder(QuantumHolder(beam.getMinor()).toRecord());
        }
        else {
            return ValueHolder(QuantumHolder(beam.getPA()).toRecord());
        }
    }
    else if (
        c == ImageMetaDataConstants::_DATAMIN || c == ImageMetaDataConstants::_DATAMAX || c == ImageMetaDataConstants::_MINPIXPOS
        || c == ImageMetaDataConstants::_MINPOS || c == ImageMetaDataConstants::_MAXPIXPOS || c == ImageMetaDataConstants::_MAXPOS
    ) {
        Record x = _getStatistics();
        if (c == ImageMetaDataConstants::_DATAMIN || c == ImageMetaDataConstants::_DATAMAX) {
            auto dt = dataType();
            if (dt == TpFloat) {
                Float val;
                x.get(c, val);
                return ValueHolder(val);
            }
            else if (dt == TpComplex) {
                Complex val;
                x.get(c, val);
                return ValueHolder(val);
            }
            else if (dt == TpDouble) {
                Double val;
                x.get(c, val);
                return ValueHolder(val);
            }
            else if (dt == TpDComplex) {
                DComplex val;
                x.get(c, val);
                return ValueHolder(val);
            }
            else {
                ThrowCc("Logic error");
            }
        }
        else if (c == ImageMetaDataConstants::_MINPOS || c == ImageMetaDataConstants::_MAXPOS) {
            return ValueHolder(x.asString(c));
        }
        else if (c == ImageMetaDataConstants::_MINPIXPOS || c == ImageMetaDataConstants::_MAXPIXPOS) {
            return ValueHolder(x.asArrayInt(c));
        }
    }
    else if (
        info.isDefined(key)    || info.isDefined(c)
    ) {
        String x = info.isDefined(key) ? key : c;
        switch (info.type(info.fieldNumber(x))) {
        case TpString:
            return ValueHolder(info.asString(x));
            break;
        case TpInt:
            return ValueHolder(info.asInt(x));
            break;
        case TpDouble:
            return ValueHolder(info.asDouble(x));
            break;
        case TpRecord:
            // allow fall through
        case TpQuantity: {
            return ValueHolder(info.asRecord(x));
            break;
        }
        default:
            ostringstream os;
            os << info.type(info.fieldNumber(x));
            ThrowCc(
                "Unhandled data type "
                + os.str()
                + " for user defined type. Send us a bug report"
            );
        }
    }
    ThrowCc(
        "Unknown keyword " + c
    );
    return ValueHolder();
}

template <class T> uInt ImageMetaDataBase<T>::_ndim() const {
    return _image->ndim();
}

template <class T> uInt ImageMetaDataBase<T>::_getAxisNumber(
    const String& key
) const {
    uInt n = 0;
    string sre = key.substr(0, 5) + "[0-9]+";
    Regex myre(Regex::makeCaseInsensitive(sre));
    if (key.find(myre) != String::npos) {
        n = String::toInt(key.substr(5));
        uInt ndim = _ndim();
        ThrowIf(
            n == 0,
            "The FITS convention is that axes "
            "are 1-based. Therefore, " + key + " is not a valid "
            "FITS keyword specification"
        );
        ThrowIf(
            n > ndim,
            "This image only has " + String::toString(ndim)
            + " axes."
        );
    }
    else {
        ThrowCc("Unsupported key " + key);
    }
    return n;
}

template <class T> String ImageMetaDataBase<T>::_getEpochString() const {
    return MVTime(_getObsDate().getValue()).string(MVTime::YMD, 12);
}

template <class T> IPosition ImageMetaDataBase<T>::_getShape() const {
    if (_shape.empty()) {
        _shape = _image->shape();
    }
    return _shape;
}

template <class T> void ImageMetaDataBase<T>::_fieldToLog(
    const Record& header,const String& field, Int precision
) const {
    _log << "        -- " << field << ": ";
    if (header.isDefined(field)) {
        DataType type = header.type(header.idToNumber(field));
        if (precision >= 0) {
            _log.output() << setprecision(precision);
        }
        switch (type) {
            case TpArrayDouble: {
                _log << header.asArrayDouble(field);
                break;
            }
            case TpArrayInt: {
                _log << header.asArrayInt(field);
                break;
            }
            case TpArrayString: {
                _log << header.asArrayString(field);
                break;
            }
            case TpDouble: {
                _log << header.asDouble(field);
                break;
            }
            case TpRecord: {
                Record r = header.asRecord(field);
                QuantumHolder qh;
                String error;
                if (qh.fromRecord(error, r) && qh.isQuantity()) {
                    Quantity q = qh.asQuantity();
                    _log << q.getValue() << q.getUnit();
                }
                else {
                    _log << "Logic Error: Don't know how to deal with records of this type "
                        << LogIO::EXCEPTION;
                }
                break;
            }
            case TpString: {
                _log << header.asString(field);
                break;
            }
            default: {
                _log << "Logic Error: Unsupported type "
                    << type << LogIO::EXCEPTION;
                break;
            }
        }
    }
    else {
        _log << "Not found";
    }
    _log << LogIO::POST;
}

template <class T> void ImageMetaDataBase<T>::_toLog(const Record& header) const {
    _log << _ORIGINB << "General --" << LogIO::POST;
    _fieldToLog(header, ImageMetaDataConstants::_IMTYPE);
    _fieldToLog(header, ImageMetaDataConstants::_OBJECT);
    _fieldToLog(header, ImageMetaDataConstants::_EQUINOX);
    _fieldToLog(header, ImageMetaDataConstants::_OBSDATE);
    _fieldToLog(header, ImageMetaDataConstants::_OBSERVER);
    _fieldToLog(header, ImageMetaDataConstants::_PROJECTION);
    if (header.isDefined(ImageMetaDataConstants::_RESTFREQ)) {
        _log << "        -- " << ImageMetaDataConstants::_RESTFREQ << ": ";
        _log.output() << std::fixed << std::setprecision(1);
        _log <<  header.asArrayDouble(ImageMetaDataConstants::_RESTFREQ) << LogIO::POST;
    }
    _fieldToLog(header, ImageMetaDataConstants::_REFFREQTYPE);
    _fieldToLog(header, ImageMetaDataConstants::_TELESCOPE);
    _fieldToLog(header, ImageMetaDataConstants::_BEAMMAJOR, 12);
    _fieldToLog(header, ImageMetaDataConstants::_BEAMMINOR, 12);
    _fieldToLog(header, ImageMetaDataConstants::_BEAMPA, 12);
    _fieldToLog(header, ImageMetaDataConstants::_BUNIT);
    _fieldToLog(header, ImageMetaDataConstants::MASKS);
    _fieldToLog(header, ImageMetaDataConstants::_SHAPE);
    _fieldToLog(header, ImageMetaDataConstants::_DATAMIN);
    _fieldToLog(header, ImageMetaDataConstants::_DATAMAX);
    _fieldToLog(header, ImageMetaDataConstants::_MINPOS);
    _fieldToLog(header, ImageMetaDataConstants::_MINPIXPOS);
    _fieldToLog(header, ImageMetaDataConstants::_MAXPOS);
    _fieldToLog(header, ImageMetaDataConstants::_MAXPIXPOS);

    uInt i = 1;
    _log << LogIO::NORMAL << "axes --" << LogIO::POST;
    while (true) {
        String iString = String::toString(i);
        String key = ImageMetaDataConstants::_CTYPE + iString;
        if (! header.isDefined(key)) {
            break;
        }
        _log << "        -- " << key << ": "
            << header.asString(key) << LogIO::POST;
        String unit = ImageMetaDataConstants::_CUNIT + iString;
        i++;
    }
    i = 1;
    _log << LogIO::NORMAL << ImageMetaDataConstants::_CRPIX << " --" << LogIO::POST;
    while (true) {
        String iString = String::toString(i);
        String key = ImageMetaDataConstants::_CRPIX + iString;
        if (! header.isDefined(key)) {
            break;
        }
        _log.output() << std::fixed << std::setprecision(1);
        _log << "        -- " << key << ": " << header.asDouble(key)
            << LogIO::POST;
        i++;
    }
    i = 1;
    _log << LogIO::NORMAL << ImageMetaDataConstants::_CRVAL << " --" << LogIO::POST;
    while (true) {
        String iString = String::toString(i);
        String key = ImageMetaDataConstants::_CRVAL + iString;
        if (! header.isDefined(key)) {
            break;
        }
        _log << "        -- " << key << ": ";
        ostringstream x;
        Double val = header.asDouble(key);
        x << val;
        String unit = ImageMetaDataConstants::_CUNIT + iString;
        if (header.isDefined(unit)) {
            x << header.asString(unit);
        }
        String valunit = x.str();
        if (header.isDefined(unit)) {
            String myunit = header.asString(unit);
            if (header.asString(unit).empty()) {
                String ctype = ImageMetaDataConstants::_CTYPE + iString;
                if (
                    header.isDefined(ctype)
                    && header.asString(ctype) == "Stokes"
                ) {
                    valunit = "['" + Stokes::name((Stokes::StokesTypes)((Int)val)) + "']";
                }
            }
            else {
                String tmp = _doStandardFormat(val, myunit);
                if (! tmp.empty()) {
                    valunit = tmp;
                }
            }
        }
        _log << valunit << LogIO::POST;
        i++;
    }
    i = 1;
    _log << LogIO::NORMAL << ImageMetaDataConstants::_CDELT << " --" << LogIO::POST;
    while (true) {
        String iString = String::toString(i);
        String key = ImageMetaDataConstants::_CDELT + iString;
        if (! header.isDefined(key)) {
            break;
        }
        _log << "        -- " << key << ": ";
        Double val = header.asDouble(key);
        String unit = ImageMetaDataConstants::_CUNIT + iString;
        String myunit;
        if (header.isDefined(unit)) {
            myunit = header.asString(unit);
        }
        ostringstream x;
        x << val << myunit;
        String valunit = x.str();
        if (header.isDefined(unit)) {
            String myunit = header.asString(unit);
            if (! header.asString(unit).empty()) {
                String tmp = _doStandardFormat(val, myunit);
                if (! tmp.empty()) {
                    valunit = tmp;
                }
            }
        }
        _log << valunit << LogIO::POST;
        i++;
    }
    i = 1;
    _log << LogIO::NORMAL << "units --" << LogIO::POST;
    while (true) {
        String iString = String::toString(i);
        String key = ImageMetaDataConstants::_CUNIT + iString;
        if (! header.isDefined(key)) {
            break;
        }
        _log << "        -- " << key << ": "
            << header.asString(key) << LogIO::POST;
        String unit = ImageMetaDataConstants::_CUNIT + iString;
        i++;
    }
}

template <class T> String ImageMetaDataBase<T>::_doStandardFormat(
    Double value, const String& unit
) const {
    String valunit;
    try {
        Quantity q(1, unit);
        if (q.isConform(Quantity(1, "rad"))) {
            // to dms
            valunit = MVAngle(Quantity(value, unit)).string(MVAngle::CLEAN, 9) + "deg.min.sec";
        }
        else if (unit == "Hz") {
            ostringstream x;
            x << std::fixed << std::setprecision(1);
            x << value << "Hz";
            valunit = x.str();
        }
    }
    catch (const AipsError& x) {}
    return valunit;
}

template <class T> uInt ImageMetaDataBase<T>::nChannels() const {
    const CoordinateSystem csys = _getCoords();
    if (! csys.hasSpectralAxis()) {
        return 0;
    }
    return _getShape()[csys.spectralAxisNumber()];
}

template <class T> Bool ImageMetaDataBase<T>::isChannelNumberValid(
    const uInt chan
) const {
    if (! _getCoords().hasSpectralAxis()) {
        return false;
    }
    return (chan < nChannels());
}

template <class T> uInt ImageMetaDataBase<T>::nStokes() const {
    const CoordinateSystem& csys = _getCoords();
    if (! csys.hasPolarizationCoordinate()) {
        return 0;
    }
    return _getShape()[csys.polarizationAxisNumber()];
}

template <class T> Int ImageMetaDataBase<T>::stokesPixelNumber(
    const String& stokesString) const {
    Int pixNum = _getCoords().stokesPixelNumber(stokesString);
    if (pixNum >= (Int)nStokes()) {
        pixNum = -1;
    }
    return pixNum;
}

template <class T> String ImageMetaDataBase<T>::_getProjection() const {
    const CoordinateSystem csys = _getCoords();
    if (! csys.hasDirectionCoordinate()) {
        return "";
    }
    const DirectionCoordinate dc = csys.directionCoordinate();
    Projection proj = dc.projection();
    if (proj.type() == Projection::SIN) {
        Vector<Double> pars =  proj.parameters();
        if (dc.isNCP()) {
            ostringstream os;
            os << "SIN (" << pars << "): NCP";
            return os.str();
        }
        else if(pars.size() == 2 && (anyNE(pars, 0.0))) {
            // modified SIN
            ostringstream os;
            os << "SIN (" << pars << ")";
            return os.str();
        }
    }
    return proj.name();
}

template <class T> String ImageMetaDataBase<T>::stokesAtPixel(
    const uInt pixel
) const {
    const CoordinateSystem& csys = _getCoords();
    if (! csys.hasPolarizationCoordinate() || pixel >= nStokes()) {
             return "";
        }
    return csys.stokesAtPixel(pixel);
}

template <class T> Bool ImageMetaDataBase<T>::isStokesValid(
    const String& stokesString
) const {
    if (! _getCoords().hasPolarizationCoordinate()) {
        return false;
    }
    Int stokesPixNum = stokesPixelNumber(stokesString);
    return stokesPixNum >= 0 && stokesPixNum < (Int)nStokes();
}

template <class T> Vector<Int> ImageMetaDataBase<T>::directionShape() const {
    Vector<Int> dirAxesNums = _getCoords().directionAxesNumbers();
    if (dirAxesNums.nelements() == 0) {
        return Vector<Int>();
    }
    Vector<Int> dirShape(2);
    IPosition shape = _getShape();
    dirShape[0] = shape[dirAxesNums[0]];
    dirShape[1] = shape[dirAxesNums[1]];
    return dirShape;
}

template <class T> Bool ImageMetaDataBase<T>::areChannelAndStokesValid(
    String& message, const uInt chan, const String& stokesString
) const {
    ostringstream os;
    Bool areValid = true;
    if (! isChannelNumberValid(chan)) {
        os << "Zero-based channel number " << chan << " is too large. There are only "
            << nChannels() << " spectral channels in this image.";
        areValid = false;
    }
    if (! isStokesValid(stokesString)) {
        if (! areValid) {
            os << " and ";
        }
        os << "Stokes parameter " << stokesString << " is not in image";
        areValid = false;
    }
    if (! areValid) {
        message = os.str();
    }
    return areValid;
}

template <class T> Record ImageMetaDataBase<T>::_calcStats() const {
    return _calcStatsT(_image);
}

template <class T> Record ImageMetaDataBase<T>::_calcStatsT(
        std::shared_ptr<const ImageInterface<T> > image
) const {
    Record x;
    if (! isReal(image->dataType())) {
        // the min and max and associated positions
        // cannot be calculated for complex images
        return x;
    }
    ImageStatistics<T> stats(*image);
    Array<typename NumericTraits<T>::PrecisionType> min;
    stats.getStatistic(min, LatticeStatsBase::MIN);
    if (min.size() == 0) {
        // image is completely masked
        return x;
    }
    x.define(ImageMetaDataConstants::_DATAMIN, min(IPosition(min.ndim(), 0)));
    Array<typename NumericTraits<T>::PrecisionType> max;
    stats.getStatistic(max, LatticeStatsBase::MAX);
    x.define(ImageMetaDataConstants::_DATAMAX, max(IPosition(max.ndim(), 0)));
    IPosition minPixPos, maxPixPos;
    stats.getMinMaxPos(minPixPos, maxPixPos);
    x.define(ImageMetaDataConstants::_MINPIXPOS, minPixPos.asVector());
    x.define(ImageMetaDataConstants::_MAXPIXPOS, maxPixPos.asVector());
    const auto& csys = _getCoords();
    Vector<Double> minPos = csys.toWorld(minPixPos);
    Vector<Double> maxPos = csys.toWorld(maxPixPos);

    String minFormat, maxFormat;
    uInt ndim = csys.nPixelAxes();
    Int spAxis = csys.spectralAxisNumber();

    for (uInt i=0; i<ndim; i++) {
        Int worldAxis = csys.pixelAxisToWorldAxis(i);
        String foundUnit;
        minFormat += csys.format(
            foundUnit, Coordinate::DEFAULT,
            minPos[i], worldAxis
        );
        maxFormat += csys.format(
            foundUnit, Coordinate::DEFAULT,
            maxPos[i], worldAxis
        );
        if ((Int)i == spAxis) {
            minFormat += foundUnit;
            maxFormat += foundUnit;
        }
        if (i != ndim-1) {
            minFormat += " ";
            maxFormat += " ";
        }
    }
    x.define(ImageMetaDataConstants::_MINPOS, minFormat);
    x.define(ImageMetaDataConstants::_MAXPOS, maxFormat);
    return x;
}

template <class T> Record ImageMetaDataBase<T>::toWorld(
    const Vector<Double>& pixel, const String& format, Bool doVelocity,
    const String& dirFrame, const String& freqFrame
) const {
    Vector<Double> pixel2 = pixel.copy();
    auto csys = _getCoords();
    {
        Vector<Double> replace = csys.referencePixel();
        const Int nIn = pixel2.size();
        const Int nOut = replace.size();
        Vector<Double> out(nOut);
        for (Int i = 0; i < nOut; ++i) {
            if (i > nIn - 1) {
                out(i) = replace(i);
            }
            else {
                out(i) = pixel2(i);
            }
        }
        pixel2.assign(out);
    }

    // Convert to world

    Vector<Double> world;
    Record rec;
    String dFrame = dirFrame;
    dFrame.upcase();
    String fFrame = freqFrame;
    fFrame.upcase();
    MDirection::Types dirType = csys.hasDirectionCoordinate()
        ? csys.directionCoordinate().directionType(dFrame == "CL")
        : MDirection::J2000;
    MFrequency::Types freqType = csys.hasSpectralAxis()
        ? csys.spectralCoordinate().frequencySystem(fFrame == "CL")
        : MFrequency::LSRK;
    if (
        (! csys.hasDirectionCoordinate() || dFrame == "CL")
        && (! csys.hasSpectralAxis() || fFrame == "CL")
    ) {
        ThrowIf(
            ! csys.toWorld(world, pixel2, true),
            "Error converting to world coordinates: " + csys.errorMessage()
        );
    }
    else if (
        (! csys.hasDirectionCoordinate() || dFrame == "NATIVE")
        && (! csys.hasSpectralAxis() || fFrame == "NATIVE")
    ) {
        ThrowIf(
            ! csys.toWorld(world, pixel2, false),
            "Error converting to world coordinates: " + csys.errorMessage()
        );
    }
    else {
        if (csys.hasDirectionCoordinate() && dFrame != "CL") {
            if (dFrame == "NATIVE") {
                dirType = csys.directionCoordinate().directionType(false);
            }
            else {
                ThrowIf(
                    ! MDirection::getType(dirType, dFrame),
                    "Unknown direction reference frame " + dirFrame
                );
            }
            auto dirCoord = csys.directionCoordinate();
            dirCoord.setReferenceConversion(dirType);
            csys.replaceCoordinate(dirCoord, csys.directionCoordinateNumber());
        }
        if (csys.hasSpectralAxis() && fFrame != "CL") {
            if (fFrame == "NATIVE") {
                freqType = csys.spectralCoordinate().frequencySystem(false);
            }
            else {
                ThrowIf(
                    ! MFrequency::getType(freqType, fFrame),
                    "Unknown frequency reference frame " + freqFrame
                );
            }
            auto specCoord = csys.spectralCoordinate();
            MFrequency::Types clFrame;
            MEpoch epoch;
            MPosition pos;
            MDirection dir;
            specCoord.getReferenceConversion(clFrame, epoch, pos, dir);
            specCoord.setReferenceConversion(freqType, epoch, pos, dir);
            csys.replaceCoordinate(specCoord, csys.spectralCoordinateNumber());
        }
        ThrowIf(
            ! csys.toWorld(world, pixel2, true),
            "Error converting to world coordinates: " + csys.errorMessage()
        );

    }
    return _worldVectorToRecord(
        csys, world, -1, format, true, true,
        doVelocity, dirType, freqType
    );
}

template <class T> Record ImageMetaDataBase<T>::_worldVectorToRecord(
    const CoordinateSystem& csys, const Vector<Double>& world, Int c,
    const String& format, Bool isAbsolute, Bool showAsAbsolute, Bool doVelocity,
    MDirection::Types dirFrame, MFrequency::Types freqFrame
) {
    // World vector must be in the native units of cSys
    // c = -1 means world must be length cSys.nWorldAxes
    // c > 0 means world must be length cSys.coordinate(c).nWorldAxes()
    // format from 'n,q,s,m'

    auto ct = upcase(format);
    Vector<String> units;
    if (c < 0) {
        units = csys.worldAxisUnits();
    }
    else {
        units = csys.coordinate(c).worldAxisUnits();
    }
    AlwaysAssert(world.size() == units.size(),AipsError);
    Record rec;
    if (ct.contains("N")) {
        rec.define("numeric", world);
    }
    if (ct.contains("Q")) {
        String error;
        Record recQ1, recQ2;
        for (uInt i = 0; i < world.size(); ++i) {
            Quantum<Double> worldQ(world(i), Unit(units(i)));
            QuantumHolder h(worldQ);
            ThrowIf(! h.toRecord(error, recQ1), error);
            recQ2.defineRecord(i, recQ1);
        }
        rec.defineRecord("quantity", recQ2);
    }
    if (ct.contains("S")) {
        Vector<Int> worldAxes;
        if (c < 0) {
            worldAxes.resize(world.size());
            indgen(worldAxes);
        }
        else {
            worldAxes = csys.worldAxes(c);
        }
        Coordinate::formatType fType = Coordinate::SCIENTIFIC;
        Int prec = 8;
        String u;
        Int coord, axisInCoord;
        Vector<String> fs(world.nelements());
        for (uInt i = 0; i < world.size(); ++i) {
            csys.findWorldAxis(coord, axisInCoord, i);
            if (
                csys.type(coord) == Coordinate::DIRECTION
                || csys.type(coord) == Coordinate::STOKES
            ) {
                fType = Coordinate::DEFAULT;
            }
            else {
                fType = Coordinate::SCIENTIFIC;
            }
            u = "";
            fs(i) = csys.format(
                u, fType, world(i), worldAxes(i),
                isAbsolute, showAsAbsolute, prec
            );
            if (! u.empty() && (u != " ")) {
                fs(i) += " " + u;
            }
        }
        rec.define("string", fs);
    }
    if (ct.contains(String("M"))) {
        Record recM = _worldVectorToMeasures(
            csys, world, c, isAbsolute, doVelocity,
            dirFrame, freqFrame
        );
        rec.defineRecord("measure", recM);
    }
    return rec;
}

template <class T> Record ImageMetaDataBase<T>::_worldVectorToMeasures(
    const CoordinateSystem& csys, const Vector<Double>& world,
    Int c, Bool abs, Bool doVelocity, MDirection::Types dirFrame,
    MFrequency::Types freqFrame
) {
    LogIO log;
    log << LogOrigin("ImageMetaDataBase", __func__);
    uInt directionCount, spectralCount, linearCount, stokesCount, tabularCount;
    directionCount = spectralCount = linearCount = stokesCount = tabularCount
            = 0;
    // Loop over desired Coordinates
    Record rec;
    String error;
    uInt s, e;
    if (c < 0) {
        AlwaysAssert(world.nelements()==csys.nWorldAxes(), AipsError);
        s = 0;
        e = csys.nCoordinates();
    }
    else {
        AlwaysAssert(world.nelements()==csys.coordinate(c).nWorldAxes(), AipsError);
        s = c;
        e = c + 1;
    }
    for (uInt i = s; i < e; ++i) {
        // Find the world axes in the CoordinateSystem that this coordinate belongs to

        const auto& worldAxes = csys.worldAxes(i);
        const auto nWorldAxes = worldAxes.size();
        Vector<Double> world2(nWorldAxes);
        const auto& coord = csys.coordinate(i);
        auto units = coord.worldAxisUnits();
        Bool none = true;

        // Fill in missing world axes if all coordinates specified

        if (c < 0) {
            for (uInt j = 0; j < nWorldAxes; ++j) {
                if (worldAxes(j) < 0) {
                    world2[j] = coord.referenceValue()[j];
                }
                else {
                    world2(j) = world(worldAxes[j]);
                    none = false;
                }
            }
        }
        else {
            world2 = world;
            none = false;
        }
        if (
            csys.type(i) == Coordinate::LINEAR
            || csys.type(i) == Coordinate::TABULAR
        ) {
            if (!none) {
                Record linRec1, linRec2;
                for (uInt k = 0; k < world2.size(); ++k) {
                    Quantum<Double> value(world2(k), units(k));
                    QuantumHolder h(value);
                    ThrowIf(
                        ! h.toRecord(error, linRec1), error
                    );
                    linRec2.defineRecord(k, linRec1);
                }
                if (csys.type(i) == Coordinate::LINEAR) {
                    rec.defineRecord("linear", linRec2);
                }
                else if (csys.type(i) == Coordinate::TABULAR) {
                    rec.defineRecord("tabular", linRec2);
                }
            }
            if (csys.type(i) == Coordinate::LINEAR) {
                ++linearCount;
            }
            if (csys.type(i) == Coordinate::TABULAR) {
                ++tabularCount;
            }
        }
        else if (csys.type(i) == Coordinate::DIRECTION) {
            ThrowIf(
                ! abs,
                "It is not possible to have a relative MDirection measure"
            );
            AlwaysAssert(worldAxes.nelements() == 2,AipsError);
            if (!none) {
                // Make an MDirection and stick in record

                Quantum<Double> t1(world2(0), units(0));
                Quantum<Double> t2(world2(1), units(1));
                MDirection direction(
                    t1, t2, dirFrame
                );
                MeasureHolder h(direction);
                Record dirRec;
                ThrowIf(
                    ! h.toRecord(error, dirRec), error
                );
                rec.defineRecord("direction", dirRec);
            }
            directionCount++;
        }
        else if (csys.type(i) == Coordinate::SPECTRAL) {
            ThrowIf(
                ! abs,
                "It is not possible to have a relative MFrequency measure"
            );
            AlwaysAssert(worldAxes.nelements()==1,AipsError);
            if (!none) {
                // Make an MFrequency and stick in record

                Record specRec, specRec1;
                Quantum<Double> t1(world2(0), units(0));
                const auto& sc0 = csys.spectralCoordinate(i);
                MFrequency frequency(t1, freqFrame);
                MeasureHolder h(frequency);
                ThrowIf(
                    ! h.toRecord(error, specRec1), error
                );
                specRec.defineRecord("frequency", specRec1);
                if (doVelocity) {
                    SpectralCoordinate sc(sc0);

                    // Do velocity conversions and stick in MDOppler
                    // Radio

                    sc.setVelocity(String("km/s"), MDoppler::RADIO);
                    Quantum<Double> velocity;
                    ThrowIf(
                        ! sc.frequencyToVelocity(velocity, frequency),
                        sc.errorMessage()
                    );
                    MDoppler v(velocity, MDoppler::RADIO);
                    MeasureHolder h(v);
                    ThrowIf(
                        ! h.toRecord(error, specRec1), error
                    );
                    specRec.defineRecord("radiovelocity", specRec1);
                    // Optical

                    sc.setVelocity(String("km/s"), MDoppler::OPTICAL);
                    ThrowIf(
                        ! sc.frequencyToVelocity(velocity, frequency),
                        sc.errorMessage()
                    );
                    v = MDoppler(velocity, MDoppler::OPTICAL);
                    h = MeasureHolder(v);
                    ThrowIf(
                        ! h.toRecord(error, specRec1), error
                    );
                    specRec.defineRecord("opticalvelocity", specRec1);

                    // beta (relativistic/true)

                    sc.setVelocity(String("km/s"), MDoppler::BETA);
                    ThrowIf(
                        ! sc.frequencyToVelocity(velocity, frequency),
                        sc.errorMessage()
                    );
                    v = MDoppler(velocity, MDoppler::BETA);
                    h = MeasureHolder(v);
                    ThrowIf(
                        ! h.toRecord(error, specRec1), error
                    );
                    specRec.defineRecord("betavelocity", specRec1);
                }
                rec.defineRecord("spectral", specRec);
            }
            ++spectralCount;
        }
        else if (csys.type(i) == Coordinate::STOKES) {
            ThrowIf (
                ! abs,
                "It makes no sense to have a relative Stokes measure"
            );
            AlwaysAssert(worldAxes.size() == 1, AipsError);
            if (!none) {
                const auto& coord0 = csys.stokesCoordinate(i);
                StokesCoordinate coord(coord0); // non-const
                String u;
                auto s = coord.format(
                    u, Coordinate::DEFAULT, world2(0),
                    0, true, true, -1
                );
                rec.define("stokes", s);
            }
            ++stokesCount;
        }
        else {
            ThrowCc("Cannot handle Coordinates of type " + csys.showType(i));
        }
    }
    if (directionCount > 1) {
        log << LogIO::WARN
                << "There was more than one DirectionCoordinate in the "
                << LogIO::POST;
        log << LogIO::WARN << "CoordinateSystem.  Only the last one is returned"
                << LogIO::POST;
    }
    if (spectralCount > 1) {
        log << LogIO::WARN
                << "There was more than one SpectralCoordinate in the "
                << LogIO::POST;
        log << LogIO::WARN << "CoordinateSystem.  Only the last one is returned"
                << LogIO::POST;
    }
    if (stokesCount > 1) {
        log << LogIO::WARN << "There was more than one StokesCoordinate in the "
                << LogIO::POST;
        log << LogIO::WARN << "CoordinateSystem.  Only the last one is returned"
                << LogIO::POST;
    }
    if (linearCount > 1) {
        log << LogIO::WARN << "There was more than one LinearCoordinate in the "
                << LogIO::POST;
        log << LogIO::WARN << "CoordinateSystem.  Only the last one is returned"
                << LogIO::POST;
    }
    if (tabularCount > 1) {
        log << LogIO::WARN
                << "There was more than one TabularCoordinate in the "
                << LogIO::POST;
        log << LogIO::WARN << "CoordinateSystem.  Only the last one is returned"
                << LogIO::POST;
    }
    return rec;
}

}

#endif
