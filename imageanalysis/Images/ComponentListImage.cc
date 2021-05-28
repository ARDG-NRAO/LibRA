//# ComponentListImage.cc: defines the PagedImage class
//# Copyright (C) 1994,1995,1996,1997,1998,1999,2000,2001,2003
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
//# $Id$

#include <imageanalysis/Images/ComponentListImage.h>

#include <casacore/casa/OS/Path.h>
#include <casacore/coordinates/Coordinates/DirectionCoordinate.h>
#include <casacore/coordinates/Coordinates/SpectralCoordinate.h>
#include <casacore/images/Images/ImageOpener.h>
#include <casacore/images/Regions/RegionHandlerTable.h>
#include <components/ComponentModels/ComponentShape.h>
#include <components/ComponentModels/SpectralModel.h>
#include <casa/Quanta/UnitMap.h>
#ifdef _OPENMP
#include <omp.h>
#endif
#include <set>

using namespace casacore;

namespace casa {

const String ComponentListImage::IMAGE_TYPE = "ComponentListImage";

ComponentListImage::ComponentListImage(
    const ComponentList& compList, const CoordinateSystem& csys,
    const IPosition& shape, const String& tableName, Bool doCache
) : ImageInterface<Float>(RegionHandlerTable(getTable, this)),
    _cl(compList.copy()), _modifiedCL(compList.copy()),
    _cache(doCache) {
    ThrowIf(tableName.empty(), "Table name cannot be empty");
    _cl.rename(Path(tableName));
    // resizing must precede setting of coordinate system
    _resize(shape);
    setCoordinateInfo(csys);
    setUnits("Jy/pixel");
}

ComponentListImage::ComponentListImage(
    const ComponentList& compList, const CoordinateSystem& csys,
    const IPosition& shape, Bool doCache
) : ImageInterface<Float>(), _cl(compList.copy()),
    _modifiedCL(compList.copy()), _cache(doCache) {
    // resizing must precede setting of coordinate system
    _resize(shape);
    setCoordinateInfo(csys);
    setUnits("Jy/pixel");
}

ComponentListImage::ComponentListImage(
    const String& filename, Bool doCache, MaskSpecifier maskSpec
) : ImageInterface<Float>(RegionHandlerTable(getTable, this)), _cl(filename, False, False),
    _modifiedCL(_cl.copy()),
    _cache(doCache) {
    _openLogTable();
    _restoreAll(_cl.getTable().keywordSet());
    _applyMaskSpecifier(maskSpec);
}

ComponentListImage::ComponentListImage(const ComponentListImage& image)
: ImageInterface<Float>(image), _cl(image._cl),
  _modifiedCL(image._modifiedCL), _shape(image._shape),
  _latAxis(image._latAxis),
  _longAxis(image._longAxis), _freqAxis(image._freqAxis),
  _stokesAxis(image._stokesAxis), _pixelLongSize(image._pixelLongSize),
  _pixelLatSize(image._pixelLatSize), _dirRef(image._dirRef),
  _dirVals(image._dirVals), _freqRef(image._freqRef),
  _freqVals(image._freqVals), _ptSourcePixelVals(image._ptSourcePixelVals),
  _pixelToIQUV(image._pixelToIQUV), _cache(image._cache),
  _hasFreq(image._hasFreq), _hasStokes(image._hasStokes),
  _computedPtSources(image._computedPtSources),
  _defaultFreq(image._defaultFreq) {
    if (image._mask) {
        _mask.reset(new LatticeRegion(*image._mask));
    }
    if (image._valueCache) {
        _valueCache.reset(
            dynamic_cast<TempImage<Float>*>(image._valueCache->cloneII())
        );
    }
}

ComponentListImage::~ComponentListImage() {}

ComponentListImage& ComponentListImage::operator=(const ComponentListImage& other) {
    if (this != &other) {
        ImageInterface<Float>::operator= (other);
        _cl = other._cl;
        _modifiedCL = other._modifiedCL;
        _shape = other._shape;
        if (other._mask) {
            _mask.reset(new LatticeRegion(*other._mask));
        }
        _latAxis = other._latAxis;
        _longAxis = other._longAxis;
        _freqAxis = other._freqAxis;
        _stokesAxis = other._stokesAxis;
        _dirRef = other._dirRef;
        _dirVals = other._dirVals;
        _freqRef = other._freqRef;;
        _freqVals = other._freqVals;
        _pixelLongSize = other._pixelLongSize;
        _pixelLatSize = other._pixelLatSize;
        _ptSourcePixelVals = other._ptSourcePixelVals;
        _pixelToIQUV = other._pixelToIQUV;
        _cache = other._cache;
        _hasFreq = other._hasFreq;
        _hasStokes = other._hasStokes;
        _computedPtSources = other._computedPtSources;
        _defaultFreq = other._defaultFreq;
        if (other._valueCache) {
            _valueCache.reset(
                dynamic_cast<TempImage<Float>*>(other._valueCache->cloneII())
            );
        }
    }
    return *this;
}
void ComponentListImage::apply(casacore::Float (*)(casacore::Float)) {
    ThrowCc("There is no general way to run " + String(__func__) + " on a ComponentList");
}

void ComponentListImage::apply(casacore::Float (*)(const casacore::Float&)) {
    ThrowCc("There is no general way to run " + String(__func__) + " on a ComponentList");
}

void ComponentListImage::apply(
    const casacore::Functional<casacore::Float, casacore::Float>&
) {
    ThrowCc("There is no general way to run " + String(__func__) + " on a ComponentList");
}

ImageInterface<Float>* ComponentListImage::cloneII() const {
    return new ComponentListImage(*this);
}

void ComponentListImage::copyData (const casacore::Lattice<casacore::Float>&) {
    ThrowCc("There is no general way to run " + String(__func__) + " on a ComponentList");
}

const ComponentList& ComponentListImage::componentList() const {
    return _cl;
}

Bool ComponentListImage::doGetMaskSlice(Array<Bool>& buffer, const Slicer& section) {
    if (_mask) {
        return _mask->doGetSlice(buffer, section);
    }
    else {
        // If no mask, base implementation returns a True mask.
        return MaskedLattice<Float>::doGetMaskSlice (buffer, section);
    }
}

Bool ComponentListImage::doGetSlice(
    Array<Float>& buffer, const Slicer& section
) {
    if (! *_computedPtSources) {
        _computePointSourcePixelValues();
    }
    const auto& chunkShape = section.length();
    if (_cache) {
        // if the values have already been computed and cached, just use them
        Array<Bool> mask(chunkShape);
        _valueCache->getMaskSlice(mask, section);
        if (allTrue(mask)) {
            _valueCache->getSlice(buffer, section);
            return True;
        }
    }
    auto secStart = section.start();
    const auto nDirs = chunkShape(_longAxis) * chunkShape(_latAxis);
    const auto nFreqs = _hasFreq ? chunkShape[_freqAxis] : 1;
    Cube<Double> pixelVals(4, nDirs, nFreqs);
    if (_modifiedCL.nelements() == 0) {
        pixelVals = 0;
    }
    else {
        Vector<MVDirection> dirVals(nDirs);
        auto secEnd = section.end();
        auto endLong = secEnd[_longAxis];
        auto endLat = secEnd[_latAxis];
        const auto& dirCoord = coordinates().directionCoordinate();
        if (_cache) {
            _getDirValsDoCache(dirVals, secStart, endLong, endLat, dirCoord);
        }
        else {
            _getDirValsNoCache(dirVals, secStart, endLong, endLat, dirCoord);
        }
        Vector<MVFrequency> freqVals(nFreqs);
        if (_hasFreq) {
            const auto& specCoord = coordinates().spectralCoordinate();
            if (_cache) {
                _getFreqValsDoCache(freqVals, secStart, nFreqs, specCoord);
            }
            else {
                _getFreqValsNoCache(freqVals, secStart, nFreqs, specCoord);
            }
        }
        else {
            freqVals[0] = _defaultFreq.getValue();
        }
        _modifiedCL.sample(
            pixelVals, Unit("Jy"), dirVals, _dirRef, _pixelLatSize,
            _pixelLongSize, freqVals, _freqRef
        );
    }
    _fillBuffer(
        buffer, chunkShape, secStart, nFreqs, pixelVals
    );
    if (_cache) {
        _valueCache->putSlice(buffer, secStart);
        Array<Bool> trueChunk(chunkShape, True);
        _valueCache->pixelMask().putSlice(trueChunk, secStart);
    }
    return True;
}

void ComponentListImage::doPutSlice (
    const Array<Float>&, const IPosition&, const IPosition&
) {
    ThrowCc(
        "ComponentListImage::" + String(__func__)
        + ": pixel values cannot be modified in a ComponentListImage"
    );
}

Table& ComponentListImage::getTable (void* imagePtr, Bool writable) {
    ComponentListImage* im = static_cast<ComponentListImage*>(imagePtr);
    if (writable) {
        im->_reopenRW();
    }
    return im->_cl._getTable();
}

const LatticeRegion* ComponentListImage::getRegionPtr() const {
    return _mask.get();
}

Bool ComponentListImage::hasPixelMask() const {
    return Bool(_mask);
}

String ComponentListImage::imageType() const {
    return (_cl.getTable().isNull() ? "Temporary " : "Persistent ") + IMAGE_TYPE;
}

Bool ComponentListImage::isMasked() const {
    return Bool(_mask);
}

Bool ComponentListImage::isPaged() const {
    return ! _cl.getTable().isNull();
}

Bool ComponentListImage::isPersistent() const {
    return ! _cl.getTable().isNull();
}

Bool ComponentListImage::isWritable() const {
    return False;
}

String ComponentListImage::name(bool stripPath) const {
    const static String tempIndicator = "Temporary ComponentListImage";
    const auto& table = _cl.getTable();
    if (table.isNull()) {
        return tempIndicator;
    }
    else {
        Path path(table.tableName());
        if (!stripPath) {
            return path.absoluteName();
        }
        return path.baseName();
    }
}

bool ComponentListImage::ok() const {
    auto n = _shape.size();
    auto x = coordinates().nPixelAxes();
    return n > 1 && n < 5 && x > 1 && x < 5;
}

LatticeBase* ComponentListImage::openCLImage(
    const String& name, const MaskSpecifier&
) {
    return new ComponentListImage(name);
}

const Lattice<Bool>& ComponentListImage::pixelMask() const {
    ThrowIf(
        ! _mask, "ComponentListImage::" + String(__func__)
        + " - no mask attached"
    );
    return *_mask;
}

void ComponentListImage::registerOpenFunction() {
    ImageOpener::registerOpenImageFunction(
        ImageOpener::COMPLISTIMAGE, &openCLImage
    );
}

void ComponentListImage::removeRegion(
    const String& name, RegionHandler::GroupType type, Bool throwIfUnknown
) {
     // Remove the default mask if it is the region to be removed.
     if (name == getDefaultMask()) {
         setDefaultMask("");
     }
     ImageInterface<Float>::removeRegion(name, type, throwIfUnknown);
}

void ComponentListImage::resize(const TiledShape& newShape) {
    _resize(newShape);
    _deleteCache();
    if (_cache) {
        _initCache();
    }
}

void ComponentListImage::set(const casacore::Float&) {
    ThrowCc(
        "There is no general way to run " + String(__func__)
        + " on a ComponentList"
    );
}

void ComponentListImage::setCache(casacore::Bool doCache) {
    if (doCache == _cache) {
        // already set to what is wanted
        return;
    }
    _cache = doCache;
    _initCache();
}

Bool ComponentListImage::setCoordinateInfo (const CoordinateSystem& csys) {
    auto x = csys.nPixelAxes();
    ThrowIf(
        x != _shape.size(),
        "Coordinate system must have the same dimensionality as the shape"
    );
    ThrowIf(
        ! csys.hasDirectionCoordinate(),
        "coordinate system must have a direction coordinate"
    );
    auto polAxisNum = csys.polarizationAxisNumber(False);
    _hasStokes = polAxisNum > 0;
    ThrowIf(
        _hasStokes && _shape[polAxisNum] > 4,
        "Polarization axis can have no more than four pixels"
    );
    if (_hasStokes) {
        const auto& stCoord = csys.stokesCoordinate();
        auto idx = stCoord.stokes();
        for (auto stokesIndex: idx) {
            Stokes::StokesTypes type = Stokes::type(stokesIndex);
            ThrowIf(
                type != Stokes::I && type != Stokes::Q
                && type != Stokes::U && type != Stokes::V,
                "Unsupported stokes type " + Stokes::name(type)
            );
        }
    }
    // implementation copied from PagedImage and tweaked.
    auto& table = _cl._getTable();
    ThrowIf(
        ! table.isNull() && ! table.isWritable(),
        "Image is not writable, cannot save coordinate system"
    );
    ThrowIf(
        ! ImageInterface<Float>::setCoordinateInfo(csys),
        "Could not set coordinate system"
    );
    if (! table.isNull()) {
        // we've tested for writability above, so if here it's writable
        _reopenRW();
        // Update the coordinates
        if (table.keywordSet().isDefined("coords")) {
            table.rwKeywordSet().removeField("coords");
        }
        ThrowIf(
            ! csys.save(table.rwKeywordSet(), "coords"),
            "Error writing coordinate system to image"
        );
    }
    _cacheCoordinateInfo(csys);
    return True;
}

void ComponentListImage::setDefaultMask(const String& regionName) {
    // Use the new region as the image's mask.
    _applyMask(regionName);
    // Store the new default name.
    ImageInterface<Float>::setDefaultMask(regionName);
}

Bool ComponentListImage::setImageInfo(const ImageInfo& info) {
    ThrowIf(info.hasBeam(), "A ComponentListImage may not have a beam(s)");
    // Set imageinfo in base class.
    Bool ok = ImageInterface<Float>::setImageInfo(info);
    auto& tab = _cl._getTable();
    if (ok && ! tab.isNull()) {
        // Make persistent in table keywords.
        _reopenRW();
        if (tab.isWritable()) {
            // Delete existing one if there.
            if (tab.keywordSet().isDefined("imageinfo")) {
                tab.rwKeywordSet().removeField("imageinfo");
            }
            // Convert info to a record and save as keyword.
            TableRecord rec;
            String error;
            if (imageInfo().toRecord(error, rec)) {
                tab.rwKeywordSet().defineRecord("imageinfo", rec);
            }
            else {
                // Could not convert to record.
                LogIO os;
                os << LogIO::SEVERE << "Error saving ImageInfo in image " << name()
                   << "; " << error << LogIO::POST;
                ok = False;
            }
        }
        else {
            // Table not writable.
            LogIO os;
            os << LogIO::SEVERE
                << "Image " << name() << " is not writable; not saving ImageInfo"
                << LogIO::POST;
        }
    }
    return ok;
}

Bool ComponentListImage::setMiscInfo(const RecordInterface& newInfo) {
    setMiscInfoMember(newInfo);
    auto& tab = _cl._getTable();
    if (! tab.isNull()) {
        _reopenRW();
        if (! tab.isWritable()) {
            return False;
        }
        if (tab.keywordSet().isDefined("miscinfo")) {
            tab.rwKeywordSet().removeField("miscinfo");
        }
        tab.rwKeywordSet().defineRecord("miscinfo", newInfo);
    }
    return True;
}

Bool ComponentListImage::setUnits(const Unit& newUnits) {
    ThrowIf(
        newUnits.getName() != "Jy/pixel", "units must be Jy/pixel"
    );
    setUnitMember (newUnits);
    if (! _cl.getTable().isNull()) {
        _reopenRW();
        auto& tab = _cl._getTable();
        if (! tab.isWritable()) {
            return False;
        }
        if (tab.keywordSet().isDefined("units")) {
            tab.rwKeywordSet().removeField("units");
        }
        tab.rwKeywordSet().define("units", newUnits.getName());
    }
    return True;
}

IPosition ComponentListImage::shape() const {
    return _shape;
}

void ComponentListImage::useMask(MaskSpecifier spec) {
    _applyMaskSpecifier(spec);
}

void ComponentListImage::handleMath(const Lattice<Float>&, int) {
    ThrowCc(
        "There is no general way to run " + String(__func__)
        + " on a ComponentList"
    );
}

void ComponentListImage::_applyMask(const String& maskName) {
    // No region if no mask name is given.
    if (maskName.empty()) {
        _mask.reset();
        return;
    }
    // Reconstruct the ImageRegion object.
    // Turn the region into lattice coordinates.
    std::unique_ptr<ImageRegion> regPtr(
        getImageRegionPtr(maskName, RegionHandler::Masks)
    );
    std::shared_ptr<LatticeRegion> latReg(
        new LatticeRegion(regPtr->toLatticeRegion(coordinates(), shape()))
    );
    // The mask has to cover the entire image.
    ThrowIf(
        latReg->shape() != shape(),
        "region " + maskName + " does not cover the full image"
    );
    _mask = latReg;
}

void ComponentListImage::_applyMaskSpecifier (const MaskSpecifier& spec) {
    // Use default mask if told to do so.
    // If it does not exist, use no mask.
    auto name = spec.name();
    if (spec.useDefault()) {
        name = getDefaultMask();
        if (! hasRegion(name, RegionHandler::Masks)) {
            name = "";
        }
    }
    _applyMask(name);
}

void ComponentListImage::_cacheCoordinateInfo(const CoordinateSystem& csys) {
    // cache often used info
    const auto dirAxes = csys.directionAxesNumbers();
    _longAxis = dirAxes[0];
    _latAxis = dirAxes[1];
    const auto& dirCoord = csys.directionCoordinate();
    // use the conversion frame, not the native one
    MDirection::Types dirFrame;
    dirCoord.getReferenceConversion(dirFrame);
    _dirRef = MeasRef<MDirection>(dirFrame);
    const auto inc = dirCoord.increment();
    const auto units = dirCoord.worldAxisUnits();
    _pixelLongSize = MVAngle(Quantity(abs(inc[_longAxis]), units[_longAxis]));
    _pixelLatSize = MVAngle(Quantity(abs(inc[_latAxis]), units[_latAxis]));
    _freqAxis = csys.spectralAxisNumber(false);
    _hasFreq = _freqAxis > 0;
    if (_hasFreq) {
        const auto& specCoord = csys.spectralCoordinate();

        // Create Frequency MeasFrame; this will enable conversions between
        // spectral frames (e.g. the CS frame might be TOPO and the CL
        // frame LSRK)

        MFrequency::Types specConv;
        MEpoch epochConv;
        MPosition posConv;
        MDirection dirConv;
        specCoord.getReferenceConversion(
            specConv,  epochConv, posConv, dirConv
        );
        MeasFrame measFrame(epochConv, posConv, dirConv);
        _freqRef = MeasRef<MFrequency>(specConv, measFrame);
    }
    else {
        _defaultFreq = _cl.component(0).spectrum().refFrequency();
        _freqRef = _defaultFreq.getRef();
    }
    _stokesAxis = csys.polarizationAxisNumber(False);
    _hasStokes = _stokesAxis > 0;
    if (_hasStokes) {
        auto nstokes = _shape[_stokesAxis];
        _pixelToIQUV.resize(nstokes);
        for (uInt s=0; s<nstokes; ++s) {
            auto mystokes = csys.stokesAtPixel(s);
            if (mystokes == "I") {
                _pixelToIQUV[s] = 0;
            }
            else if (mystokes == "Q") {
                _pixelToIQUV[s] = 1;
            }
            else if (mystokes == "U") {
                _pixelToIQUV[s] = 2;
            }
            else if (mystokes == "V") {
                _pixelToIQUV[s] = 3;
            }
            else {
                ThrowCc(
                    "Unsupported stokes value " + mystokes + " at pixel "
                    + String::toString(s)
                );
            }
        }
    }
    else {
        _pixelToIQUV.resize(1);
        _pixelToIQUV[0] = 0;
    }
    _deleteCache();
    if (_cache) {
        _initCache();
    }
}

void ComponentListImage::_computePointSourcePixelValues() {
    if (*_computedPtSources) {
        return;
    }
    _ptSourcePixelVals->clear();
    auto n = _cl.nelements();
    std::set<uInt> pointSourceIdx;
    for (uInt i=0; i<n; ++i) {
        if (_cl.getShape(i)->type() == ComponentType::POINT) {
            pointSourceIdx.insert(i);
        }
    }
    if (pointSourceIdx.empty()) {
        *_computedPtSources = True;
        return;
    }
    std::vector<uInt> foundPtSourceIdx;
    uInt nInside = 0;
    uInt nOutside = 0;
    _findPointSoures(foundPtSourceIdx, nInside, nOutside, pointSourceIdx);
    LogIO log(LogOrigin(IMAGE_TYPE, __func__));
    auto npts = pointSourceIdx.size();
    if (nInside > 0) {
        log << LogIO::NORMAL << "Found " << nInside << " of " << npts
            << " point sources located within the image and cached their "
            << "pixel coordinates." << LogIO::POST;
    }
    if (nOutside > 0) {
        log << LogIO::NORMAL << "Found " << nOutside << " of " << npts
            << " point sources to be located outside the image, so will ignore "
            << "those." << LogIO::POST;
    }
    if (! foundPtSourceIdx.empty()) {
        _modifiedCL.remove(Vector<Int>(foundPtSourceIdx));
    }
    *_computedPtSources = True;
}

void ComponentListImage::_deleteCache() {
    _freqVals.resize();
    _dirVals.resize();
    if (_valueCache) {
        _valueCache->resize(TiledShape());
    }
    _ptSourcePixelVals->clear();
    *_computedPtSources = False;
}

void ComponentListImage::_fillBuffer(
    Array<Float>& buffer, const IPosition& chunkShape,
    const IPosition& secStart, uInt nFreqs,
    const Cube<Double>& pixelVals
) const {
    if (buffer.size() != chunkShape) {
        buffer.resize(chunkShape, False);
    }
    const auto lookForPtSources = ! _ptSourcePixelVals->empty();
    const auto nLong = chunkShape[_longAxis];
    const auto nLat = chunkShape[_latAxis];
    const auto nStokes = _hasStokes ? chunkShape[_stokesAxis] : 1;
    const uInt ilatStart = secStart[_latAxis];
    const uInt startLong = secStart[_longAxis];
    const uInt startFreq =  _hasFreq ? secStart[_freqAxis] : 0;
    const uInt startPol = _hasStokes ? secStart[_stokesAxis] : 0;
    const uInt ndim = chunkShape.size();
#ifdef _OPENMP
#pragma omp parallel for collapse(4)
#endif
    for(uInt blat=0; blat<nLat; ++blat) {
        for(uInt blong=0; blong<nLong; ++blong) {
            for (uInt bfreq=0; bfreq<nFreqs; ++bfreq) {
                for (uInt bpol=0; bpol<nStokes; ++bpol) {
                    uInt ilat = ilatStart + blat;
                    IPosition posInArray(ndim);
                    posInArray[_latAxis] = blat;
                    std::pair<uInt, uInt> dirPos;
                    uInt ilong = startLong + blong;
                    dirPos.first = ilong;
                    uInt d = blat * nLong + blong;
                    posInArray[_longAxis] = blong;
                    dirPos.second = ilat;
                    uInt ifreq = startFreq + bfreq;
                    if (_hasFreq) {
                        posInArray[_freqAxis] = bfreq;
                    }
                    uInt ipol = startPol + bpol;
                    if (_hasStokes) {
                        posInArray[_stokesAxis] = bpol;
                    }
                    buffer(posInArray) = pixelVals(_pixelToIQUV[ipol], d, bfreq);
                    if (lookForPtSources) {
                        auto ptSourceContrib = _ptSourcePixelVals->find(dirPos);
                        if (ptSourceContrib != _ptSourcePixelVals->end()) {
                            buffer(posInArray) += ptSourceContrib->second(ifreq, ipol);
                        }
                    }
                }
            }
        }
    }
}

ComponentListImage::PtFound ComponentListImage::_findPixel(
    Cube<Double>& values, const IPosition& pixelPosition,
    const DirectionCoordinate& dirCoord, const SkyComponent& point,
    const Vector<MVFrequency>& freqValues
) const {
    MVDirection imageWorld;
    static const Unit jy("Jy");
    Vector<Double> pixel(2);
    pixel[0] = pixelPosition[0];
    pixel[1] = pixelPosition[1];
    dirCoord.toWorld(imageWorld, pixel);
    point.sample(
        values, jy, Vector<MVDirection>(1, imageWorld),
        _dirRef, _pixelLatSize, _pixelLongSize, freqValues, _freqRef
    );
    if (anyNE(values, 0.0)) {
        if (
            pixelPosition[_longAxis] >= 0 && pixelPosition[_latAxis] >= 0
            && pixelPosition[_longAxis] < _shape[_longAxis]
            && pixelPosition[_latAxis] < _shape[_latAxis]
        ) {
            return FOUND_INSIDE;
        }
        else {
            return FOUND_OUTSIDE;
        }
    }
    return NOT_FOUND;
}

ComponentListImage::PtFound ComponentListImage::_findPixelIn3x3Box(
    IPosition& pixelPosition, Cube<Double>& values,
    const DirectionCoordinate& dirCoord, const SkyComponent& point,
    const Vector<MVFrequency>& freqValues
) const {
    // look for the pixel in a 3x3 square around the target pixel
    auto targetPixel = pixelPosition;
    for (
        pixelPosition[_longAxis]=targetPixel[_longAxis]-1;
        pixelPosition[_longAxis]<=targetPixel[_longAxis]+1; ++pixelPosition[_longAxis]
    ) {
        for (
            pixelPosition[_latAxis]=targetPixel[_latAxis]-1;
            pixelPosition[_latAxis]<=targetPixel[_latAxis]+1; ++pixelPosition[_latAxis]
        ) {
            // we've already looked at the target pixel position before this method
            // was called and didn't find the point source there, so don't waste
            // time doing it again
            if (
                (
                    pixelPosition[_longAxis] != targetPixel[_longAxis]
                    || pixelPosition[_latAxis] != targetPixel[_latAxis]
                )
            ) {
                auto res = _findPixel(values, pixelPosition, dirCoord, point, freqValues);
                if (res != NOT_FOUND) {
                    return res;
                }
            }
        }
    }
    return NOT_FOUND;
}

void ComponentListImage::_findPointSoures(
    std::vector<uInt>& foundPtSourceIdx, uInt& nInside, uInt& nOutside,
    const std::set<uInt>& pointSourceIdx
) {
    Vector<Double> pixel;
    auto nFreqs = _hasFreq ? _shape[_freqAxis] : 1;
    auto freqValues = _getAllFreqValues(nFreqs);
    Cube<Double> values(4, 1, nFreqs);
    IPosition pixelPosition(_shape.size(), 0);
    const auto& dirCoord = coordinates().directionCoordinate();
    uInt nStokes = _hasStokes ? _shape[_stokesAxis] : 1;
    std::pair<uInt, uInt> dirPos;
    for (const auto i: pointSourceIdx) {
        dirCoord.toPixel(pixel, _cl.getRefDirection(i));
        pixelPosition[_longAxis] = floor(pixel[0] + 0.5);
        pixelPosition[_latAxis] = floor(pixel[1] + 0.5);
        const auto& point = _cl.component(i);
        values = 0;
        // allow some slop at the image boundaries on the first pass
        if (
            pixelPosition[_longAxis] < -1
            || pixelPosition[_latAxis] < -1
            || pixelPosition[_longAxis] > _shape[_longAxis]
            || pixelPosition[_latAxis] > _shape[_latAxis]
        ) {
            ++nOutside;
            foundPtSourceIdx.push_back(i);
        }
        else {
            auto foundPixel = _findPixel(
                values, pixelPosition, dirCoord, point, freqValues
            );
            if (foundPixel == NOT_FOUND) {
                foundPixel = _findPixelIn3x3Box(
                    pixelPosition, values, dirCoord, point, freqValues
                );
            }
            if (foundPixel == FOUND_INSIDE) {
                ++nInside;
                foundPtSourceIdx.push_back(i);
                dirPos.first = pixelPosition[_longAxis];
                dirPos.second = pixelPosition[_latAxis];
                auto myiter = _ptSourcePixelVals->find(dirPos);
                if (myiter == _ptSourcePixelVals->end()) {
                    (*_ptSourcePixelVals)[dirPos]
                        = Matrix<Float>(nFreqs, nStokes, 0);
                }
                for (uInt f = 0; f < nFreqs; ++f) {
                    for (uInt s = 0; s < nStokes; ++s) {
                        auto v = values(_pixelToIQUV[s], 0, f);
                        (*_ptSourcePixelVals)[dirPos](f, s) += v;
                    }
                }
            }
            else if (foundPixel == FOUND_OUTSIDE) {
                ++nOutside;
                foundPtSourceIdx.push_back(i);
            }
        }
    }
}

Vector<MVFrequency> ComponentListImage::_getAllFreqValues(uInt nFreqs) {
    Vector<MVFrequency> freqValues(nFreqs);
    if (! _hasFreq) {
        freqValues[0] = _defaultFreq.get("Hz");
        if (_cache) {
            _freqVals[0].reset(new MVFrequency(freqValues[0]));
        }
        return freqValues;
    }
    Double thisFreq;
    const auto& specCoord = coordinates().spectralCoordinate();
    auto freqUnit = specCoord.worldAxisUnits()[0];
    Quantity freq(0, freqUnit);
    for (Double pixelFreq=0; pixelFreq<nFreqs; ++pixelFreq) {
        // Includes any frame conversion
        ThrowIf (
            ! specCoord.toWorld(thisFreq, pixelFreq),
            "cannot convert a frequency value"
        );
        freq.setValue(thisFreq);
        freqValues[pixelFreq] = MVFrequency(freq);
        if (_cache) {
            _freqVals[pixelFreq].reset(new MVFrequency(freq));
        }
    }
    return freqValues;
}

void ComponentListImage::_getDirValsDoCache(
    Vector<MVDirection>& dirVals, const IPosition& secStart, uInt endLong,
    uInt endLat, const DirectionCoordinate& dirCoord
) {
    Vector<Double> pixelDir(2);
    IPosition iPixelDir(2);
    uInt d = 0;
    for(
        pixelDir[1]=secStart[_latAxis], iPixelDir[1]=secStart[_latAxis];
        pixelDir[1]<=endLat; ++pixelDir[1], ++iPixelDir[1]
    ) {
        for (
            pixelDir[0]=secStart[_longAxis], iPixelDir[0]=secStart[_longAxis];
            pixelDir[0]<=endLong; ++pixelDir[0], ++iPixelDir[0], ++d
        ) {
            auto dirVal = _dirVals(iPixelDir);
            if (dirVal) {
                // cached value exists, use it
                dirVals[d] = *dirVal;
            }
            else {
                std::shared_ptr<MVDirection> newDirVal(new MVDirection);
                if (! dirCoord.toWorld(*newDirVal, pixelDir)) {
                    ostringstream oss;
                    oss << "Unable to convert to world direction at pixel "
                        << pixelDir;
                    ThrowCc(oss.str());
                }
                // cache it
                _dirVals(iPixelDir) = newDirVal;
                dirVals[d] = *newDirVal;
            }
        }
    }
}

void ComponentListImage::_getDirValsNoCache(
    Vector<MVDirection>& dirVals, const IPosition& secStart, uInt endLong,
    uInt endLat, const DirectionCoordinate& dirCoord
) const {
    Vector<Double> pixelDir(2);
    IPosition iPixelDir(2);
    uInt d = 0;
    for(
        pixelDir[1]=secStart[_latAxis], iPixelDir[1]=secStart[_latAxis];
        pixelDir[1]<=endLat; ++pixelDir[1], ++iPixelDir[1]
    ) {
        for (
            pixelDir[0]=secStart[_longAxis], iPixelDir[0]=secStart[_longAxis];
            pixelDir[0]<=endLong; ++pixelDir[0], ++iPixelDir[0], ++d
        ) {
            if (! dirCoord.toWorld(dirVals[d], pixelDir)) {
                ostringstream oss;
                oss << "Unable to convert to world direction at pixel "
                    << pixelDir;
                ThrowCc(oss.str());
            }
        }
    }
}

void ComponentListImage::_getFreqValsDoCache(
    Vector<MVFrequency>& freqVals, const IPosition& secStart,
    uInt nFreqs, const SpectralCoordinate& specCoord
) {
    uInt f = 0;
    Double thisFreq;
    auto freqUnit = specCoord.worldAxisUnits()[0];
    Quantity freq(0, freqUnit);
    for (Double pixelFreq=secStart[_freqAxis]; f<nFreqs; ++pixelFreq, ++f) {
        auto freqVal = _freqVals[pixelFreq];
        if (freqVal) {
            // already cached
            freqVals[f] = *freqVal;
        }
        else {
            // Includes any frame conversion
            ThrowIf (
                ! specCoord.toWorld(thisFreq, pixelFreq),
                "cannot convert a frequency value"
            );
            freq.setValue(thisFreq);
            freqVals[f] = freq;
            _freqVals[pixelFreq].reset(new MVFrequency(freq));
        }
    }
}

void ComponentListImage::_getFreqValsNoCache(
    Vector<MVFrequency>& freqVals, const IPosition& secStart,
    uInt nFreqs, const SpectralCoordinate& specCoord
) const {
    uInt f = 0;
    Double thisFreq;
    auto freqUnit = specCoord.worldAxisUnits()[0];
    Quantity freq(0, freqUnit);
    for (Double pixelFreq=secStart[_freqAxis]; f<nFreqs; ++pixelFreq, ++f) {
        // Includes any frame conversion
        ThrowIf (
            ! specCoord.toWorld(thisFreq, pixelFreq),
            "cannot convert a frequency value"
        );
        freq.setValue(thisFreq);
        freqVals[f] = freq;
    }
}

void ComponentListImage::_initCache() {
    if (_cache) {
        auto nLong = _shape[_longAxis];
        auto nLat = _shape[_latAxis];
        _dirVals = Matrix<std::shared_ptr<MVDirection>>(nLong, nLat);
        _dirVals.set(std::shared_ptr<MVDirection>(nullptr));
        auto nFreqs = _hasFreq ? _shape[_freqAxis] : 1;
        _freqVals.resize(nFreqs);
        _freqVals.set(std::shared_ptr<MVFrequency>(nullptr));
        _valueCache.reset(new TempImage<Float>(_shape, coordinates()));
        _valueCache->attachMask(TempLattice<Bool>(_shape, False));
        *_computedPtSources = False;
        _ptSourcePixelVals->clear();
    }
    else {
        _deleteCache();
    }
}

void ComponentListImage::_openLogTable() {
    // Open logtable as readonly if main table is not writable.
    auto& tab = _cl._getTable();
    setLogMember (LoggerHolder (name() + "/logtable", tab.isWritable()));
    // Insert the keyword if possible and if it does not exist yet.
    if (tab.isWritable()  &&  ! tab.keywordSet().isDefined ("logtable")) {
        tab.rwKeywordSet().defineTable("logtable", Table(name() + "/logtable"));
    }
}

void ComponentListImage::_reopenRW() {
    // implementation copied from PagedImage and tweaked
    auto& table = _cl._getTable();
    //# Open for write if not done yet and if writable.
    if (!table.isWritable()) {
        table.reopenRW();
    }
}

void ComponentListImage::_resize(const TiledShape& newShape) {
    auto shape = newShape.shape();
    ThrowIf(
        shape.size() < 2 || shape.size() > 4,
        "ComponentListImages must have 2, 3, or 4 dimensions"
    );
    ThrowIf(anyLE(shape.asVector(), 0), "All shape elements must be positive");
    if (! _shape.conform(shape)) {
        _shape.resize(shape.size(), False);
    }
    _shape = shape;
    auto& table = _cl._getTable();
    if (! table.isNull()) {
        _reopenRW();
        if (table.isWritable()) {
            // Update the shape
            if (table.keywordSet().isDefined("shape")) {
                table.rwKeywordSet().removeField("shape");
            }
            table.rwKeywordSet().define("shape", _shape.asVector());
        }
        else {
            LogIO os;
            os << LogIO::SEVERE << "Image " << name()
               << " is not writable; not saving shape"
               << LogIO::POST;
        }
    }
}

void ComponentListImage::_restoreAll(const TableRecord& rec) {
    // must do shape before coordinates
    ThrowIf(! rec.isDefined("shape"), "shape is not present in " + name());
    ThrowIf(
        rec.type(rec.fieldNumber("shape")) != TpArrayInt,
        "shape found in " + name() + " is not stored as an integer array"
    );
    _resize(IPosition(rec.asArrayInt("shape")));
    // Restore the coordinates.
    std::unique_ptr<CoordinateSystem> restoredCoords(
        CoordinateSystem::restore(rec, "coords")
    );
    ThrowIf(! restoredCoords, "Error restoring coordinate system");
    setCoordinateInfo(*restoredCoords);
    // Restore the image info.
    _restoreImageInfo(rec);
    // Restore the units.
    _restoreUnits(rec);
    // Restore the miscinfo.
    _restoreMiscInfo(rec);
}

void ComponentListImage::_restoreImageInfo (const TableRecord& rec) {
    if (rec.isDefined("imageinfo")) {
        String error;
        ImageInfo info;
        Bool ok = info.fromRecord (error, rec.asRecord("imageinfo"));
        if (ok) {
            setImageInfoMember(info);
        }
        else {
            LogIO os;
            os << LogIO::WARN << "Failed to restore the ImageInfo in image " << name()
                 << "; " << error << LogIO::POST;
        }
    }
}

void ComponentListImage::_restoreMiscInfo (const TableRecord& rec) {
    if (rec.isDefined("miscinfo") && rec.dataType("miscinfo") == TpRecord) {
        setMiscInfoMember (rec.asRecord ("miscinfo"));
    }
}

void ComponentListImage::_restoreUnits (const TableRecord& rec) {
    Unit retval;
    String unitName;
    if (rec.isDefined("units")) {
        if (rec.dataType("units") != TpString) {
            LogIO os;
            os << LogOrigin("PagedImage<T>", "units()", WHERE)
                << "'units' keyword in image table is not a string! Units not restored."
                << LogIO::SEVERE << LogIO::POST;
        }
        else {
            rec.get("units", unitName);
        }
    }
    if (! unitName.empty()) {
        // OK, non-empty unit, see if it's valid, if not try some known things to
        // make a valid unit out of it.
        if (! UnitVal::check(unitName)) {
            // Beam and Pixel are the most common undefined units
            UnitMap::putUser("Pixel",UnitVal(1.0),"Pixel unit");
            UnitMap::putUser("Beam",UnitVal(1.0),"Beam area");
        }
        if (! UnitVal::check(unitName)) {
            // OK, maybe we need FITS
            UnitMap::addFITS();
        }
        if (!UnitVal::check(unitName)) {
            // I give up!
            LogIO os;
            UnitMap::putUser(unitName, UnitVal(1.0, UnitDim::Dnon), unitName);
            os << LogIO::WARN << "FITS unit \"" << unitName
                << "\" unknown to CASA - will treat it as non-dimensional."
                << LogIO::POST;
            retval.setName(unitName);
            retval.setValue(UnitVal(1.0, UnitDim::Dnon));
        }
        else {
            retval = Unit(unitName);
        }
    }
    setUnitMember(retval);
}

}
