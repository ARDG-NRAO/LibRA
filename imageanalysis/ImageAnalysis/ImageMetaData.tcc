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

#include <imageanalysis/ImageAnalysis/ImageMetaData.h>

#include <casacore/casa/Logging/LogFilter.h>

#include <casa/aips.h>

#include <images/Images/ImageSummary.h>


#ifndef IMAGEANALYSIS_IMAGEMETADATA_TCC
#define IMAGEANALYSIS_IMAGEMETADATA_TCC

using namespace casacore;

namespace casa {

template <class T> ImageMetaData<T>::ImageMetaData(
    SPCIIT image
) : ImageMetaDataBase<T>(image), _image(image), _info(image->imageInfo()),
    _csys(image->coordinates()) {}

template<class T> String ImageMetaData<T>::_getBrightnessUnit() const {
    return _image->units().getName();
}

template<class T> casacore::Record ImageMetaData<T>::summary(
    const casacore::String& doppler, const casacore::Bool list,
    const casacore::Bool pixelorder, const casacore::Bool verbose
) {
    auto log = this->_getLog();
    log << casacore::LogOrigin("ImageMetaData", __func__);
    casacore::Vector<casacore::String> messages;
    casacore::Record retval;
    casacore::ImageSummary<T> s(*_image);
    casacore::MDoppler::Types velType;
    if (!casacore::MDoppler::getType(velType, doppler)) {
        log << casacore::LogIO::WARN << "Illegal velocity type, using RADIO"
                << casacore::LogIO::POST;
        velType = casacore::MDoppler::RADIO;
    }

    if (list) {
        messages = s.list(log, velType, false, verbose);
    }
    else {
        // Write messages to local sink only so we can fish them out again
        casacore::LogFilter filter;
        casacore::LogSink sink(filter, false);
        casacore::LogIO osl(sink);
        messages = s.list(osl, velType, true);
    }
    retval.define("messages", messages);
    auto axes = s.axisNames(pixelorder);
    auto crpix = s.referencePixels(false); // 0-rel
    auto crval = s.referenceValues(pixelorder);
    auto cdelt = s.axisIncrements(pixelorder);
    auto axisunits = s.axisUnits(pixelorder);

    auto shape = this->_getShape();
    retval.define("ndim", (casacore::Int)shape.size());
    retval.define("shape", shape.asVector());
    retval.define("tileshape", s.tileShape().asVector());
    retval.define("axisnames", axes);
    retval.define("refpix", crpix);
    retval.define("refval", crval);
    retval.define("incr", cdelt);
    retval.define("axisunits", axisunits);
    retval.define("unit", _getBrightnessUnit());
    retval.define("hasmask", s.hasAMask());
    retval.define("defaultmask", s.defaultMaskName());
    retval.define("masks", s.maskNames());
    retval.define("imagetype", _getImType());

    const auto& info = _image->imageInfo();
    casacore::Record iRec;
    casacore::String error;
    using casacore::AipsError;
    ThrowIf(
        ! info.toRecord(error, iRec),
        "Failed to convert ImageInfo to a record because "
    );
    if (iRec.isDefined("restoringbeam")) {
        retval.defineRecord("restoringbeam", iRec.asRecord("restoringbeam"));
    }
    else if (iRec.isDefined("perplanebeams")) {
        retval.defineRecord("perplanebeams", info.beamToRecord(-1, -1));
    }
    return retval;
}

template <class T> Record ImageMetaData<T>::toRecord(Bool verbose) const {
    if (_header.empty()) {
        _header = this->_makeHeader();
    }
    if (verbose) {
        this->_toLog(_header);
    }
    return _header;
}

template <class T> Vector<String> ImageMetaData<T>::_getAxisNames() const {
    if (_axisNames.empty()) {
        _axisNames = _getCoords().worldAxisNames();
    }
    return _axisNames;
}

template <class T> Vector<String> ImageMetaData<T>::_getAxisUnits() const {
    if (_axisUnits.empty()) {
        _axisUnits = _getCoords().worldAxisUnits();
    }
    return _axisUnits;
}

template <class T> GaussianBeam ImageMetaData<T>::_getBeam() const {
    const ImageInfo& info = _getInfo();
    if (info.hasSingleBeam()) {
        if (_beam == GaussianBeam::NULL_BEAM) {
            _beam = info.restoringBeam(-1, -1);
        }
        return _beam;
    }
    else if (info.hasMultipleBeams()) {
        throw AipsError("This image has multiple beams.");
    }
    else {
        throw AipsError("This image has no beam(s).");
    }
}

template <class T> String ImageMetaData<T>::_getEquinox() const {
    if (_equinox.empty()) {
        if (_getCoords().hasDirectionCoordinate()) {
            _equinox = MDirection::showType(
                _getCoords().directionCoordinate().directionType()
            );
        }
    }
    return _equinox;
}

template <class T> String ImageMetaData<T>::_getImType() const {
    if (_imtype.empty()) {
        _imtype = ImageInfo::imageType(_getInfo().imageType());
    }
    return _imtype;
}

template <class T> vector<Quantity> ImageMetaData<T>::_getIncrements() const {
    if (_increment.size() == 0) {
        Vector<Double> incs = _getCoords().increment();
        Vector<String> units = _getAxisUnits();
        for (uInt i=0; i<incs.size(); i++) {
            _increment.push_back(Quantity(incs[i], units[i]));
        }
    }
    return _increment;
}

template <class T> String ImageMetaData<T>::_getObject() const {
    if (_object.empty()) {
        _object = _getInfo().objectName();
    }
    return _object;
}

template <class T> Vector<String> ImageMetaData<T>::_getMasks() const {
    if (_masks.empty()) {
        _masks = _image->regionNames(RegionHandler::Masks);
    }
    return _masks;
}

template <class T> MEpoch ImageMetaData<T>::_getObsDate() const {
    if (_obsdate.get("s") == 0) {
        _obsdate = _getCoords().obsInfo().obsDate();
    }
    return _obsdate;
}

template <class T> String ImageMetaData<T>::_getObserver() const {
    if (_observer.empty()) {
        _observer = _getCoords().obsInfo().observer();
    }
    return _observer;
}

template <class T> String ImageMetaData<T>::_getProjection() const {
    if (_projection.empty()) {
        _projection = ImageMetaDataBase<T>::_getProjection();
    }
    return _projection;
}

template <class T> Vector<Double> ImageMetaData<T>::_getRefPixel() const {
    if (_refPixel.size() == 0) {
        _refPixel = _getCoords().referencePixel();
    }
    return _refPixel;
}

template <class T> Vector<String> ImageMetaData<T>::_getStokes() const {
    const CoordinateSystem csys = _getCoords();
    ThrowIf(
        ! csys.hasPolarizationCoordinate(),
        "Logic Error: coordinate system does not have a polarization coordinate"
    );
    if (_stokes.empty()) {
        _stokes = csys.stokesCoordinate().stokesStrings();
    }
    return _stokes;
}

template <class T> Vector<Quantity> ImageMetaData<T>::_getRefValue() const {
    if (_refVal.size() == 0) {
        Vector<Double> vals = _getCoords().referenceValue();
        Vector<String> units = _getAxisUnits();
        for (uInt i=0; i<vals.size(); i++) {
            _refVal.push_back(Quantity(vals[i], units[i]));
        }
    }
    return _refVal;
}

template <class T> String ImageMetaData<T>::_getRefFreqType() const {
    if (_reffreqtype.empty() && _getCoords().hasSpectralAxis()) {
        _reffreqtype = MFrequency::showType(
            _getCoords().spectralCoordinate().frequencySystem(false)
        );
    }
    return _reffreqtype;
}

template <class T> Quantity ImageMetaData<T>::_getRestFrequency() const {
    const CoordinateSystem& csys = _getCoords();
    ThrowIf(
        ! csys.hasSpectralAxis(),
        "Image has no spectral axis so there is no rest frequency"
    );
    if (_restFreq.getValue() == 0) {
        _restFreq = Quantity(
            csys.spectralCoordinate().restFrequency(),
            csys.spectralCoordinate().worldAxisUnits()[0]
        );
    }
    return _restFreq;
}

template <class T> Record ImageMetaData<T>::_getStatistics() const {
    if (_stats.empty() && isReal(_image->dataType())) {
        _stats = this->_calcStats();
    }
    return _stats;
}

template <class T> String ImageMetaData<T>::_getTelescope() const {
    if (_telescope.empty()) {
        _telescope = _getCoords().obsInfo().telescope();
    }
    return _telescope;
}

}

#endif
