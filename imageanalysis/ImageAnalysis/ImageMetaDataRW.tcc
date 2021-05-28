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


#ifndef IMAGES_IMAGEMETADATARW_TCC
#define IMAGES_IMAGEMETADATARW_TCC

#include <imageanalysis/ImageAnalysis/ImageMetaDataRW.h>

#include <casa/Containers/ValueHolder.h>
#include <casa/Quanta/QuantumHolder.h>
#include <coordinates/Coordinates/CoordinateUtil.h>
#include <coordinates/Coordinates/DirectionCoordinate.h>
#include <coordinates/Coordinates/SpectralCoordinate.h>
#include <tables/Tables/TableRecord.h>

#include <imageanalysis/ImageAnalysis/ImageHistory.h>

#include <casa/aips.h>

#include <iomanip>

#define _LOCATEA "ImageMetaDataRW" << __func__ << " "
#define _ORIGINA LogOrigin("ImageMetaDataRW", __func__)

using namespace casacore;

namespace casa {

template <class T> ImageMetaDataRW<T>::ImageMetaDataRW(SPIIT image)
    : ImageMetaDataBase<T>(image), _image(image) {}

template <class T> Record ImageMetaDataRW<T>::toRecord(Bool verbose) const {
    if (_header.empty()) {
        _header = this->_makeHeader();
    }
    if (verbose) {
        this->_toLog(_header);
    }
    return _header;
}

template <class T> Bool ImageMetaDataRW<T>::_isWritable() const {
    return _image->isWritable();
}

template <class T> Bool ImageMetaDataRW<T>::_setUnit(const String& unit) {
    return _image->setUnits(unit);
}

template <class T> const TableRecord ImageMetaDataRW<T>::_miscInfo() const {
    return _image->miscInfo();
}

template <class T> void ImageMetaDataRW<T>::_setMiscInfo(const TableRecord& rec) {
    auto res = _image->setMiscInfo(rec);
    ThrowIf(
        ! res,
        "Error setting misc info"
    );
}

template <class T> Bool ImageMetaDataRW<T>::add(const String& key, const ValueHolder& value) {
    ThrowIf(
        ! _isWritable(),
        "This image is not writable; metadata may not be added to it"
    );
    String c = key;
    c.downcase();
    ThrowIf(
        c.startsWith(ImageMetaDataConstants::_CDELT)
        || c.startsWith(ImageMetaDataConstants::_CRPIX)
        || c.startsWith(ImageMetaDataConstants::_CRVAL)
        || c.startsWith(ImageMetaDataConstants::_CTYPE)
        || c.startsWith(ImageMetaDataConstants::_CUNIT),
        key + " pertains to a "
        + "coordinate system axis attribute. It may be "
        + "modified if it exists, but it may not be added."
    );
    ThrowIf(
        c == ImageMetaDataConstants::_EQUINOX,
        "The direction reference frame ("
        + key + "=" + _getEquinox()
        +") already exists. It may be modified but not added."
    );
    ThrowIf(
        c == ImageMetaDataConstants::MASKS,
        "This application does not support adding masks."
    );
    ThrowIf(
        c == ImageMetaDataConstants::_OBSDATE || c == ImageMetaDataConstants::_EPOCH,
        "The epoch (" + key + "=" + this->_getEpochString()
        + ") already exists. It may be modified but not added."
    );
    ThrowIf(
        c == ImageMetaDataConstants::_PROJECTION,
        "The projection ("
        + key + "=" + _getProjection()
        +") already exists. It may be modified but not added."
    );
    ThrowIf(
        c == ImageMetaDataConstants::_REFFREQTYPE,
        "The velocity reference frame ("
        + key + "=" + _getProjection()
        +") already exists. It may be modified but not added."
    );
    ThrowIf(
        c == ImageMetaDataConstants::_SHAPE,
        "The shape is intrinsic to the image and may "
        "not be added."
    );
    ThrowIf(
        c == ImageMetaDataConstants::_BEAMPA || c == ImageMetaDataConstants::_BPA,
        "Cannot add a beam position "
        "angle. Add the major or minor axis and then "
        "modify the other and the position angle."
    );
    ThrowIf(
        c == ImageMetaDataConstants::_DATAMIN
        || c == ImageMetaDataConstants::_DATAMAX
        || c == ImageMetaDataConstants::_MINPIXPOS
        || c == ImageMetaDataConstants::_MAXPIXPOS
        || c == ImageMetaDataConstants::_MINPOS
        || c == ImageMetaDataConstants::_MAXPOS,
        key + " is is a statistic of the image and may "
        "not be added."
    );
    if (c == ImageMetaDataConstants::_BUNIT) {
        ThrowIf(
            ! _getBrightnessUnit().empty(),
            key + " is already present and has value "
            + _getBrightnessUnit() + ". It may be modified but not added."
        );
        String v = value.asString();
        ThrowIf(
            ! _setUnit(v),
            "Unable to set " + key
        );
        _bunit = v;
        _modHistory(__func__, c, String(""), v);
    }
    else if (c == ImageMetaDataConstants::_IMTYPE) {
        String imtype = _getImType();
        ThrowIf(
            ! imtype.empty(),
            "The image type ("
            + key + "=" + ImageInfo::imageType(imtype)
            + ") already exists. It may be modified but be added."
        );
        set(c, value);
    }
    else if (c == ImageMetaDataConstants::_OBJECT) {
        String object = _getObject();
        ThrowIf(
            ! object.empty(),
            key + " is already present and has value "
            + object + ". It may be modified but not added."
        );
        set(c, value);
    }
    else if (c == ImageMetaDataConstants::_OBSERVER) {
        String observer = _getObserver();
        ThrowIf(
            ! observer.empty(),
            key + " is already present and has value "
            + observer + ".  It may be modified but not added."
        );
        set(c, value);
    }
    else if (c == ImageMetaDataConstants::_RESTFREQ) {
        ThrowIf(
            _getRestFrequency().getValue() > 0,
            "The rest frequency ("
            + key + "=" + String::toString(_getRestFrequency().getValue())
            + _getRestFrequency().getUnit()
            + ") already exists. It may be modified but not added "
            + "by this application. If you wish to append a rest frequency "
            + "to an already existing list, use cs.setrestfrequency()."
        );
        set(c, value);
    }
    else if (c == ImageMetaDataConstants::_TELESCOPE) {
        String telescope = _getTelescope();
        ThrowIf(
            ! telescope.empty(),
            key + " is already present and has value "
            + telescope + ". It may be modified but not added."
        );
        set(c, value);
    }
    else if (
        c == ImageMetaDataConstants::_BEAMMAJOR
        || c == ImageMetaDataConstants::_BEAMMINOR
        || c == ImageMetaDataConstants::_BMAJ
        || c == ImageMetaDataConstants::_BMIN
    ) {
        ImageInfo info = _getInfo();
        ThrowIf(
            info.hasBeam(),
            "This image already has a beam(s). Cannot add one."
        );
        set(c, value);
    }
    else if (
        _miscInfo().isDefined(key)
        || _miscInfo().isDefined(c)
    ) {
        ThrowCc(
            "Keyword " + key
            + " already exists so cannot be added."
        );
    }
    else {
        _setUserDefined(key, value);
    }
    // clear cached header
    _header.assign(Record());
    return true;
}


template <class T> Bool ImageMetaDataRW<T>::remove(const String& key) {
    ThrowIf(
        ! _isWritable(),
        "This image is not writable; metadata may not be removed from it"
    );
    String c = key;
    auto log = this->_getLog();
    log << _ORIGINA;
    c.downcase();
    ThrowIf(
        c.startsWith(ImageMetaDataConstants::_CDELT)
        || c.startsWith(ImageMetaDataConstants::_CRPIX)
        || c.startsWith(ImageMetaDataConstants::_CRVAL)
        || c.startsWith(ImageMetaDataConstants::_CTYPE)
        || c.startsWith(ImageMetaDataConstants::_CUNIT),
        key + " pertains to a "
        "coordinate system axis attribute. It may be "
        "modified, but it may not be removed."
    );
    ThrowIf(
        c == ImageMetaDataConstants::_EQUINOX,
        "Although the direction reference frame ("
        + key + ") may be modified, it may not be removed."
    );
    ThrowIf(
        c == ImageMetaDataConstants::_IMTYPE,
        "Although the image type ("
        + key + ") may be modified, it may not be removed."
    );
    ThrowIf(
        c == ImageMetaDataConstants::MASKS,
        "Logic Error: removeMask() should be called instead"
    );
    ThrowIf(
        c == ImageMetaDataConstants::_OBSDATE || c == ImageMetaDataConstants::_EPOCH,
        "Although the epoch (" + key
        + ") may be modified, it cannot be removed."
    );
    ThrowIf(
        c == ImageMetaDataConstants::_PROJECTION,
        "Although the projection (" + key
        + ") may be modified, it cannot be removed."
    );
    ThrowIf(
        c == ImageMetaDataConstants::_REFFREQTYPE,
        "Although the velocity reference frame (" + key
        + ") may be modified, it cannot be removed."
    );
    ThrowIf(
        c == ImageMetaDataConstants::_RESTFREQ,
        "Although the rest frequency (" + key
        + ") may be modified, it cannot be removed."
    );
    ThrowIf(
        c == ImageMetaDataConstants::_SHAPE,
        "The shape is intrinsic to the image and may "
        "not be modified nor removed."
    );
    ThrowIf(
        c == ImageMetaDataConstants::_DATAMIN
        || c == ImageMetaDataConstants::_DATAMAX
        || c == ImageMetaDataConstants::_MINPIXPOS
        || c == ImageMetaDataConstants::_MAXPIXPOS
        || c == ImageMetaDataConstants::_MINPOS
        || c == ImageMetaDataConstants::_MAXPOS,
        key + " is is a statistic of the image and may "
        + "not be modified nor removed by this application."
    );
    if (c == ImageMetaDataConstants::_BUNIT) {
        auto oldUnit = _getBrightnessUnit();
        ThrowIf(
            ! _setUnit(""),
            "Unable to clear " + key
        );
        _bunit = "";
        _modHistory(__func__, c, oldUnit, String(""));
        log << LogIO::NORMAL << "Setting " << key << " to empty string" << LogIO::POST;
    }
    else if (c == ImageMetaDataConstants::_OBJECT) {
        ImageInfo info = _getInfo();
        auto old = info.objectName();
        info.setObjectName("");
        _setImageInfo(info);
        log << LogIO::NORMAL << "Setting " << key << " to empty string" << LogIO::POST;
        _object = "";
        _modHistory(__func__, c, old, String(""));
    }
    else if (c == ImageMetaDataConstants::_OBSERVER) {
        CoordinateSystem csys = _getCoords();
        ObsInfo info = csys.obsInfo();
        auto old = info.observer();
        info.setObserver("");
        csys.setObsInfo(info);
        _setCsys(csys);
        log << LogIO::NORMAL << "Setting " << key << " to empty string" << LogIO::POST;
        _observer = "";
        _modHistory(__func__, c, old, String(""));
    }
    else if (c == ImageMetaDataConstants::_TELESCOPE) {
        CoordinateSystem csys = _getCoords();
        ObsInfo info = csys.obsInfo();
        auto old = info.telescope();
        info.setTelescope("");
        csys.setObsInfo(info);
        _setCsys(csys);
        log << LogIO::NORMAL << "Setting " << key << " to empty string" << LogIO::POST;
        _telescope = "";
        _modHistory(__func__, c, old, String(""));
    }
    else if (
        c == ImageMetaDataConstants::_BEAMMAJOR
        || c == ImageMetaDataConstants::_BEAMMINOR
        || c == ImageMetaDataConstants::_BEAMPA
        || c == ImageMetaDataConstants::_BMAJ
        || c == ImageMetaDataConstants::_BMIN
        || c == ImageMetaDataConstants::_BPA
    ) {
        ImageInfo info = _getInfo();
        if (info.hasBeam()) {
            String history;
            if (info.hasSingleBeam()) {
                log << LogIO::NORMAL << "Removing this image's single beam"
                    << LogIO::POST;
                history = "Removed image's single beam";
            }
            else {
                log << LogIO::NORMAL << "Removing all of this image's multiple beams"
                    << LogIO::POST;
                history = "Removed image's multiple beams";
            }
            info.removeRestoringBeam();
            _setImageInfo(info);
            _beam = GaussianBeam::NULL_BEAM;
            _toHistory(__func__, history);
        }
        else {
            log << LogIO::WARN << "This image has no beam(s) to remove." << LogIO::POST;
            return false;
        }
    }
    else if (_miscInfo().isDefined(key)) {
        TableRecord info = _miscInfo();
        info.removeField(key);
        _setMiscInfo(info);
        auto history = "Removed user-defined keyword " + key;
        log << LogIO::NORMAL << history << LogIO::POST;
        _toHistory(__func__, history);
    }
    else if (_miscInfo().isDefined(c)) {
        TableRecord info = _miscInfo();
        info.removeField(c);
        _setMiscInfo(info);
        auto history = "Removed user-defined keyword " + c;
        log << LogIO::NORMAL << history << LogIO::POST;
        _toHistory(__func__, history);
    }
    else {
        ThrowCc("Unknown keyword " + c);
    }
    _header.assign(Record());
    return true;
}

template <class T> Bool ImageMetaDataRW<T>::_hasRegion(const String& maskName) const {
    return _image->hasRegion(maskName, RegionHandler::Masks);
}

template <class T> Bool ImageMetaDataRW<T>::removeMask(const String& maskName) {
    ThrowIf(
        ! _isWritable(),
        "This image is not writable; a mask(s) may not be removed from it"
    );
    auto log = this->_getLog();
    log << _ORIGINA;
    if (maskName.empty()) {
        Vector<String> masks = _getMasks().copy();
        if (masks.size() == 0) {
            log << LogIO::WARN << "This image has no masks, so nothing to do."
                << LogIO::POST;
            return true;
        }
        else {
            Vector<String>::const_iterator end = masks.end();
            for (
                Vector<String>::const_iterator iter=masks.begin();
                iter!=end; iter++
            ) {
                removeMask(*iter);
            }
            _masks.resize(0);
            return _getMasks().size() == 0;
        }
    }
    else {
        ThrowIf(
            ! _hasRegion(maskName),
            "No mask named " + maskName + " found"
        );
        _image->removeRegion(maskName, RegionHandler::Masks);
        ThrowIf(
            _hasRegion(maskName),
            "Unable to remove mask " + maskName
        );
        _masks.resize(0);
        log << LogIO::NORMAL << "Removed mask named " << maskName << endl;
        _header.assign(Record());
        return true;
    }
}

template <class T> void ImageMetaDataRW<T>::setCsys(const Record& coordinates) {
    ThrowIf(
        coordinates.nfields() == 0,
        "Record is empty"
    );
    ThrowIf(
        ! _setCsys(
            *_makeCoordinateSystem(
                coordinates, this->_getShape()
            )
        ), "Unable to set coordinate system"
    );
}

template <class T> std::unique_ptr<CoordinateSystem> ImageMetaDataRW<T>::_makeCoordinateSystem(
    const Record& coordinates, const IPosition& shape
) {
    std::unique_ptr<CoordinateSystem> pCS;
    if (coordinates.nfields() == 1) {
        // must be a record as an element
        Record tmp(coordinates.asRecord(RecordFieldId(0)));
        pCS.reset(CoordinateSystem::restore(tmp, ""));
    }
    else {
        pCS.reset(CoordinateSystem::restore(coordinates, ""));
    }

    // Fix up any body longitude ranges...
    String errMsg;
    if (! CoordinateUtil::cylindricalFix(*pCS, errMsg, shape)) {
       this->_getLog() << LogOrigin("ImageMetaDataRW", __func__)
           << LogIO::WARN << errMsg << LogIO::POST;
    }
    return pCS;
}

template <class T> Bool ImageMetaDataRW<T>::_setCsys(const CoordinateSystem& csys) {
    return _image->setCoordinateInfo(csys);
}

template <class T> Bool ImageMetaDataRW<T>::_setImageInfo(const ImageInfo& info) {
    return _image->setImageInfo(info);
}

template <class T> Quantity ImageMetaDataRW<T>::_getQuantity(const ValueHolder& v) {
    QuantumHolder qh;
    String error;
    DataType type = v.dataType();
    if (type == TpRecord) {
        ThrowIf(
            ! qh.fromRecord(error, v.asRecord()),
            "Error converting to Quantity. " + error
        );
        return qh.asQuantity();
    }
    else if (type == TpString) {
        Quantity q;
        ThrowIf(
            ! readQuantity(q, v.asString()),
            "Error converting " + v.asString() + " to Quantity"
        );
        return q;
    }
    else {
        ostringstream os;
        os << "Input ValueHolder is of type "
            << v.dataType() << ", but it must be either "
            << " a Record or a String";
        ThrowCc(os.str());
    }
}

template <class T> Bool ImageMetaDataRW<T>::set(
    const String& key, const ValueHolder& value
) {
    ThrowIf(
        ! _isWritable(),
        "This image is not writable; metadata may not be modified in it"
    );
    String c = key;
    c.downcase();
    ValueHolder old;
    if (c == ImageMetaDataConstants::_BUNIT) {
        auto v = _getString(key, value);
        old = ValueHolder(_getBrightnessUnit());
        if (_setUnit(v)) {
            _bunit = v;
        }
        else {
            ThrowCc("Unable to set " + key);
        }
    }
    else if (
        c.startsWith(ImageMetaDataConstants::_CDELT)
        || c.startsWith(ImageMetaDataConstants::_CRPIX)
        || c.startsWith(ImageMetaDataConstants::_CRVAL)
        || c.startsWith(ImageMetaDataConstants::_CTYPE)
        || c.startsWith(ImageMetaDataConstants::_CUNIT)
    ) {
        _setCoordinateValue(c, value);
    }
    else if (c == ImageMetaDataConstants::_EQUINOX) {
        ThrowIf(
            ! _getCoords().hasDirectionCoordinate(),
            "This image does not have a direction "
            "coordinate and so a direction projection cannot be added."
        );
        String v = _getString(key, value);
        v.upcase();
        MDirection::Types type;
        ThrowIf(
            !MDirection::getType(type, v),
            "Unknown direction reference frame specification"
        );
        CoordinateSystem csys = _getCoords();
        DirectionCoordinate dircoord = csys.directionCoordinate();
        if (dircoord.directionType(false) == type) {
            // nothing to do
            return true;
        }
        old = ValueHolder(
            MDirection::showType(dircoord.directionType(false))
        );
        dircoord.setReferenceFrame(type);
        csys.replaceCoordinate(dircoord, csys.directionCoordinateNumber());
        _setCsys(csys);
        _equinox = v;
    }
    else if (c == ImageMetaDataConstants::_IMTYPE) {
        String v = _getString(key, value);
        String imtype = _getImType();
        /*
        ImageInfo info = _floatImage
            ? _floatImage->imageInfo()
            : _complexImage->imageInfo();
            */
        auto info = _image->imageInfo();
        old = ValueHolder(ImageInfo::imageType(info.imageType()));
        info.setImageType(ImageInfo::imageType(v));
        _setImageInfo(info);
        String newType = ImageInfo::imageType(info.imageType());
        _imtype = newType;
    }
    else if (c == ImageMetaDataConstants::MASKS) {
        ThrowCc("This application does not support modifying masks.");
    }
    else if (c == ImageMetaDataConstants::_OBJECT) {
        String v = _getString(key, value);
        String object = _getObject();
        ImageInfo info = _getInfo();
        old = ValueHolder(info.objectName());
        info.setObjectName(v);
        _setImageInfo(info);
        _object = v;
    }
    else if (c == ImageMetaDataConstants::_OBSDATE || c == ImageMetaDataConstants::_EPOCH) {
        ThrowIf(
            value.dataType() == TpString
            && value.asString().empty(),
            key + " value not specified"
        );
        Quantity qval = _getQuantity(value);
        ThrowIf(
            ! qval.isConform("s"),
            key + " value must have units of time or be in a supported time format"
        );
        MEpoch epoch(qval);
        CoordinateSystem csys = _getCoords();
        ObsInfo info = csys.obsInfo();
        auto oldEpoch = info.obsDate();
        ostringstream oss;
        oss << oldEpoch << endl;
        old = ValueHolder(oss.str());
        info.setObsDate(epoch);
        csys.setObsInfo(info);
        _setCsys(csys);
        _obsdate = epoch;
    }
    else if (c == ImageMetaDataConstants::_OBSERVER) {
        String v = _getString(key, value);
        CoordinateSystem csys = _getCoords();
        ObsInfo info = csys.obsInfo();
        old = ValueHolder(info.observer());
        info.setObserver(v);
        csys.setObsInfo(info);
        _setCsys(csys);
        _observer = v;
    }
    else if (c == ImageMetaDataConstants::_PROJECTION) {
        String v = _getString(key, value);
        ThrowIf(
            ! _getCoords().hasDirectionCoordinate(),
            "This image does not have a direction "
            "coordinate and so a direction projection cannot be added."
        );
        v.upcase();
        Projection::Type ptype = Projection::type(v);
        ThrowIf(
            ptype == Projection::N_PROJ,
            "Unknown projection specification " + v
        );
        CoordinateSystem csys = _getCoords();
        DirectionCoordinate dircoord = csys.directionCoordinate();
        Projection curProj = dircoord.projection();
        if (curProj.type() == ptype) {
            // nothing to do
            return true;
        }
        Vector<Double> curParms = curProj.parameters();
        Projection projection(ptype, curParms);
        old = ValueHolder(dircoord.projection().name());
        dircoord.setProjection(projection);
        csys.replaceCoordinate(dircoord, csys.directionCoordinateNumber());
        _setCsys(csys);
        _projection = _getProjection();
    }
    else if (c == ImageMetaDataConstants::_REFFREQTYPE) {
        String v = _getString(key, value);
        ThrowIf(
            ! _getCoords().hasSpectralAxis(),
            "This image does not have a spectral coordinate"
            "and so a velocity reference frame cannot be added."
        );
        v.upcase();
        MFrequency::Types type;
        ThrowIf(
            ! MFrequency::getType(type, v),
            "Unknown velocity reference frame specification " + v
        );
        CoordinateSystem csys = _getCoords();
        SpectralCoordinate spcoord = csys.spectralCoordinate();
        if (spcoord.frequencySystem(false) == type) {
            return true;
        }
        old = ValueHolder(MFrequency::showType(spcoord.frequencySystem(false)));
        spcoord.setFrequencySystem(type);
        csys.replaceCoordinate(spcoord, csys.spectralCoordinateNumber());
        _setCsys(csys);
        _reffreqtype = v;
    }
    else if (c == ImageMetaDataConstants::_RESTFREQ) {
        ThrowIf(
            ! _getCoords().hasSpectralAxis(),
            "This image does not have a spectral coordinate"
            "and so a velocity reference frame cannot be added."
        );
        Quantity rf = _getQuantity(value);
        ThrowIf(
            rf.getValue() <= 0,
            "Unable to set rest frequency to "
            + value.asString()
        );
        ThrowIf(
            ! rf.getUnit().empty() && ! rf.isConform("Hz"),
            "Unable to set rest frequency to "
            + value.asString() + " because units do not conform to Hz"
        );
        CoordinateSystem csys = _getCoords();
        SpectralCoordinate sp = csys.spectralCoordinate();
        DataType type = value.dataType();
        Double v = (
            type == TpShort || type == TpUShort
            || type == TpInt || type == TpUInt
            || type == TpFloat || type == TpDouble
        )
            ? value.asDouble()
            : rf.getValue(sp.worldAxisUnits()[0]);
        old = ValueHolder(sp.restFrequency());
        sp.setRestFrequency(v);
        csys.replaceCoordinate(sp, csys.spectralCoordinateNumber());
        _setCsys(csys);
        _restFreq = rf;
    }
    else if (c == ImageMetaDataConstants::_SHAPE) {
        ThrowCc(
            "The shape is intrinsic to the image and may "
            "not be modified."
        );
    }

    else if (c == ImageMetaDataConstants::_TELESCOPE) {
        String v = _getString(key, value);
        CoordinateSystem csys = _getCoords();
        ObsInfo info = csys.obsInfo();
        old = ValueHolder(info.telescope());
        info.setTelescope(v);
        csys.setObsInfo(info);
        _setCsys(csys);
        _telescope = v;
    }
    else if (
        c == ImageMetaDataConstants::_BEAMMAJOR
        || c == ImageMetaDataConstants::_BEAMMINOR
        || c == ImageMetaDataConstants::_BEAMPA
        || c == ImageMetaDataConstants::_BMAJ
        || c == ImageMetaDataConstants::_BMIN
        || c == ImageMetaDataConstants::_BPA
    ) {
        ImageInfo info = _getInfo();
        ThrowIf(
            info.hasMultipleBeams(),
            "This image has multiple beams. "
            "This application cannot modify beams in such an image."
        )
        Quantity v = _getQuantity(value);
        GaussianBeam beam;
        ostringstream oss;
        if (c == ImageMetaDataConstants::_BEAMPA || c == ImageMetaDataConstants::_BPA) {
            ThrowIf(
                ! info.hasBeam(),
                "This image has no beam. This application cannot add a beam position "
                "angle to an image with no beam. Add the major or minor axis and then "
                "modify the other and the position angle with put."
            );
            beam = info.getBeamSet()(0, 0);
            oss << beam.getPA(false);
            old = ValueHolder(oss.str());
            beam.setPA(v);
        }
        else if (info.hasBeam()) {
            beam = info.getBeamSet()(0, 0);
            if (
                c == ImageMetaDataConstants::_BEAMMAJOR
                || c == ImageMetaDataConstants::_BMAJ
            ) {
                oss << beam.getMajor() << endl;
                beam.setMajorMinor(v, beam.getMinor());
            }
            else {
                oss << beam.getMinor() << endl;
                beam.setMajorMinor(beam.getMajor(), v);
            }
            old = ValueHolder(oss.str());
        }
        else {
            beam = GaussianBeam(v, v, Quantity(0, "deg"));
            oss << "Added beam " << beam << endl;
            _toHistory(__func__, oss.str());
        }
        info.setRestoringBeam(beam);
        _setImageInfo(info);
        _beam = beam;
        this->_getLog() << LogIO::NORMAL << "Updated single beam " << beam
            << " in image." << LogIO::POST;
    }
    else if (
        c == ImageMetaDataConstants::_DATAMIN
        || c == ImageMetaDataConstants::_DATAMAX
        || c == ImageMetaDataConstants::_MINPIXPOS
        || c == ImageMetaDataConstants::_MAXPIXPOS
        || c == ImageMetaDataConstants::_MINPOS
        || c == ImageMetaDataConstants::_MAXPOS
    ) {
        ThrowCc(
            key + " is is a statistic of the image and may "
            "not be added or modified."
        );
    }
    else {
        _setUserDefined(key, value);
    }
    if (! old.isNull()) {
        _modHistory(__func__, key, old, value);
    }
    // clear the cached header values
    _header.assign(Record());
    return true;
}

template <class T> void ImageMetaDataRW<T>::_setUserDefined(
    const String& key, const ValueHolder& value
) {
    TableRecord info = _miscInfo();
    DataType type = value.dataType();
    switch(type) {
    case TpBool:
        info.define(key, value.asBool());
        break;
    case TpArrayBool:
        info.define(key, value.asArrayBool());
        break;
    case TpComplex:
        info.define(key, value.asComplex());
        break;
    case TpDouble:
        info.define(key, value.asDouble());
        break;
    case TpArrayDouble:
        info.define(key, value.asArrayDouble());
        break;
    case TpInt:
        info.define(key, value.asInt());
        break;
    case TpArrayInt:
        info.define(key, value.asArrayInt());
        break;
    case TpInt64:
        info.define(key, value.asInt64());
        break;
    case TpArrayInt64:
        info.define(key, value.asArrayInt64());
        break;
    case TpRecord: {
        info.defineRecord(key, value.asRecord());
        break;
    }
    case TpString:
        info.define(key, value.asString());
        break;
    case TpArrayString: {
        info.define(key, value.asArrayString());
        break;
    }
    default:
        ostringstream os;
        os << "Unhandled value type " << type
            << " for " << key;
        ThrowCc(os.str());
        break;
    }
    _setMiscInfo(info);
    _addHistory(__func__, key, value);
}

template <class T> void ImageMetaDataRW<T>::_setCoordinateValue(
    const String& key, const ValueHolder& value
) {
    LogIO log = this->_getLog();
    String prefix = key.substr(0, 5);
    CoordinateSystem csys = _getCoords();
    uInt n = this->_getAxisNumber(key);
    ValueHolder old;
    Bool isStokes = csys.hasPolarizationCoordinate()
        && (Int)n == csys.polarizationAxisNumber(false) + 1;
    if (prefix == ImageMetaDataConstants::_CDELT) {
        ThrowIf(
            isStokes,
            "A polarization axis cannot have an increment"
        );
        Quantity qinc = _getQuantity(value);
        Vector<Double> increments = csys.increment();
        auto oldUnit = _getAxisUnits()[n-1];
        if (qinc.getFullUnit().empty()) {
            qinc.setUnit(oldUnit);
        }
        old = ValueHolder(
            String::toString(increments[n-1]) + oldUnit
        );
        increments[n-1] = qinc.getValue(_getAxisUnits()[n-1]);
        csys.setIncrement(increments);
        if (! _increment.empty()) {
            _increment[n-1] = qinc;
        }
    }
    else if (prefix == ImageMetaDataConstants::_CRPIX) {
        ThrowIf(
            isStokes,
            "A polarization axis cannot have a reference pixel"
        );
        DataType t = value.dataType();
        Double x = 0;
        if (t == TpString) {
            x = String::toDouble(value.asString(), true);
        }
        else if (
            t == TpInt || t == TpInt64
            || t == TpDouble || t == TpFloat
        ) {
            x = value.asDouble();
        }
        else {
            ostringstream os;
            os << t;
            ThrowCc("For crpix, value must be numeric, not " + os.str());
        }
        auto refpix = _getRefPixel();
        old = ValueHolder(refpix[n-1]);
        refpix[n-1] = x;
        csys.setReferencePixel(refpix);
        if (! _refPixel.empty()) {
            _refPixel[n-1] = refpix[n-1];
        }
    }
    else if (prefix == ImageMetaDataConstants::_CRVAL) {
        DataType dType = value.dataType();
        ThrowIf(
            dType == TpString
            && value.asString().empty(),
            key + " value not specified"
        );
        Vector<Double> refval = csys.referenceValue();
        if (isStokes) {
            ThrowIf(
                dType != TpString && dType != TpArrayString,
                "Data type to put must be either a string array or string"
            );
            uInt nStokes = csys.stokesCoordinate().stokes().size();
            Vector<String> stokesTypes;
            if (dType == TpString) {
                ThrowIf(
                    nStokes > 1,
                    "There are " + String::toString(nStokes)
                    + " polarization values, "
                    "so a string array of that length is needed"
                );
                stokesTypes = Vector<String>(1, value.asString());
            }
            else if (dType == TpArrayString) {
                ThrowIf(
                    value.asArrayString().size() != nStokes,
                    "There are " + String::toString(nStokes)
                    + " polarization values, "
                    "so a string array of that length is needed"
                );
                stokesTypes = value.asArrayString();
            }
            Vector<Int> stokesNumbers(nStokes);
            Vector<String>::const_iterator begin = stokesTypes.begin();
            Vector<String>::const_iterator typeIter = stokesTypes.begin();
            Vector<String>::const_iterator end = stokesTypes.end();
            Vector<Int>::iterator numberIter = stokesNumbers.begin();
            while (typeIter != end) {
                *numberIter = (Int)Stokes::type(*typeIter);
                typeIter++;
                numberIter++;
            }
            StokesCoordinate coord(stokesNumbers);
            old = ValueHolder(csys.stokesCoordinate().stokesStrings());
            ThrowIf(
                ! csys.replaceCoordinate(coord, csys.polarizationCoordinateNumber()),
                "Failed to replace stokes coordinate"
            );
            _stokes = stokesTypes;
        }
        else {
            auto qval = _getQuantity(value);
            auto oldUnit = _getAxisUnits()[n-1];
            if (qval.getUnit().empty()) {
                qval.setUnit(oldUnit);
            }
            old = ValueHolder(String::toString(refval[n-1]) + oldUnit);
            refval[n-1] = qval.getValue(_getAxisUnits()[n-1]);
            csys.setReferenceValue(refval);
            if (! _refVal.empty()) {
                _refVal[n-1] = qval;
            }
        }
    }
    else if (prefix == ImageMetaDataConstants::_CTYPE) {
        auto names = _getAxisNames();
        old = ValueHolder(names[n-1]);
        names[n-1] = _getString(key, value);
        csys.setWorldAxisNames(names);
        if (! _axisNames.empty()) {
            _axisNames[n-1] = names[n-1];
        }
    }
    else if (prefix == ImageMetaDataConstants::_CUNIT) {
        ThrowIf(
            isStokes,
            "A polarization axis cannot have a unit"
        );
        auto u = _getString(key, value);
        // Test to see if CASA supports this string as a Unit
        Unit x = Unit(u);
        Vector<String> units = _getAxisUnits();
        old = ValueHolder(units[n-1]);
        units[n-1] = u;
        csys.setWorldAxisUnits(units, true);
        if (! _axisUnits.empty()) {
            _axisUnits[n-1] = units[n-1];
        }
    }
    _setCsys(csys);
    _modHistory(__func__, key, old, value);
    // clear stats because modifying the coordinate system may invalidate
    // min and max value world coordinates
    _stats = Record();
}


template <class T> String ImageMetaDataRW<T>::_getString(
    const String& key, const ValueHolder& v
) const {
    ThrowIf(
        v.dataType() != TpString,
        key + "value must be a string"
    );
    return v.asString();
}

template <class T> Vector<String> ImageMetaDataRW<T>::_getAxisNames() const {
    if (_axisNames.size() == 0) {
        _axisNames = _getCoords().worldAxisNames();
    }
    return _axisNames;
}

template <class T> Vector<String> ImageMetaDataRW<T>::_getAxisUnits() const {
    if (_axisUnits.size() == 0) {
        _axisUnits = _getCoords().worldAxisUnits();
    }
    return _axisUnits;
}

template <class T> GaussianBeam ImageMetaDataRW<T>::_getBeam() const {
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

template <class T> String ImageMetaDataRW<T>::_getBrightnessUnit() const {
    if (_bunit.empty()) {
        _bunit = _image->units().getName();
    }
    return _bunit;
}

template <class T> String ImageMetaDataRW<T>::_getEquinox() const {
    const CoordinateSystem& csys = _getCoords();
    if (_equinox.empty()) {
        if (csys.hasDirectionCoordinate()) {
            _equinox = MDirection::showType(
                csys.directionCoordinate().directionType()
            );
        }
    }
    return _equinox;
}

template <class T> String ImageMetaDataRW<T>::_getImType() const {
    if (_imtype.empty()) {
        _imtype = ImageInfo::imageType(_getInfo().imageType());
    }
    return _imtype;
}

template <class T> vector<Quantity> ImageMetaDataRW<T>::_getIncrements() const {
    if (_increment.size() == 0) {
        Vector<Double> incs = _getCoords().increment();
        Vector<String> units = _getAxisUnits();
        for (uInt i=0; i<incs.size(); i++) {
            _increment.push_back(Quantity(incs[i], units[i]));
        }
    }
    return _increment;
}

template <class T> String ImageMetaDataRW<T>::_getObject() const {
    if (_object.empty()) {
        _object = _getInfo().objectName();
    }
    return _object;
}

template <class T> const ImageInfo& ImageMetaDataRW<T>::_getInfo() const {
    return _image->imageInfo();
}

template <class T> const CoordinateSystem& ImageMetaDataRW<T>::_getCoords() const {
    return _image->coordinates();
}

template <class T> Vector<String> ImageMetaDataRW<T>::_getMasks() const {
    if (_masks.empty()) {
        _masks = _image->regionNames(RegionHandler::Masks);
    }
    return _masks;
}

template <class T> MEpoch ImageMetaDataRW<T>::_getObsDate() const {
    if (_obsdate.get("s").getValue() == 0) {
        _obsdate = _getCoords().obsInfo().obsDate();
    }
    return _obsdate;
}

template <class T> String ImageMetaDataRW<T>::_getObserver() const {
    if (_observer.empty()) {
        _observer = _getCoords().obsInfo().observer();
    }
    return _observer;
}

template <class T> String ImageMetaDataRW<T>::_getProjection() const {
    if (_projection.empty()) {
        _projection = ImageMetaDataBase<T>::_getProjection();
    }
    return _projection;
}

template <class T> Vector<Double> ImageMetaDataRW<T>::_getRefPixel() const {
    if (_refPixel.size() == 0) {
        _refPixel = _getCoords().referencePixel();
    }
    return _refPixel;
}

template <class T> Vector<String> ImageMetaDataRW<T>::_getStokes() const {
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

template <class T> Vector<Quantity> ImageMetaDataRW<T>::_getRefValue() const {
    if (_refVal.size() == 0) {
        Vector<Double> vals = _getCoords().referenceValue();
        Vector<String> units = _getAxisUnits();
        for (uInt i=0; i<vals.size(); i++) {
            _refVal.push_back(Quantity(vals[i], units[i]));
        }
    }
    return _refVal;
}

template <class T> String ImageMetaDataRW<T>::_getRefFreqType() const {
    const CoordinateSystem& csys = _getCoords();
    if (_reffreqtype.empty() && csys.hasSpectralAxis()) {
        _reffreqtype = MFrequency::showType(csys.spectralCoordinate().frequencySystem(false));
    }
    return _reffreqtype;
}

template <class T> Quantity ImageMetaDataRW<T>::_getRestFrequency() const {
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

template <class T> String ImageMetaDataRW<T>::_getTelescope() const {
    if (_telescope.empty()) {
        _telescope = _getCoords().obsInfo().telescope();
    }
    return _telescope;
}

template <class T> Record ImageMetaDataRW<T>::_getStatistics() const {
    if (_stats.nfields() == 0 && isReal(_image->dataType())) {
        _stats = this->_calcStats();
    }
    return _stats;
}

template <class T> template <class U> void ImageMetaDataRW<T>::_addHistory(
    const String& func, const String& keyword, const U& newVal
) {
    ostringstream oss;
    oss << "Added " << keyword << " = "
        << _quotify(newVal);
    _toHistory(func, oss.str());
}

template <class T> template <class U, class V> void ImageMetaDataRW<T>::_modHistory(
    const String& func, const String& keyword,
    const U& oldVal, const V& newVal
) {
    ostringstream oss;
    oss << "Modified " << keyword << " from "
        << _quotify(oldVal) << " to " << _quotify(newVal) << endl;
    _toHistory(func, oss.str());
}

template <class T> void ImageMetaDataRW<T>::_toHistory(
    const String& origin, const String& record
) {
    ImageHistory<T> ih(_image);
    ih.addHistory("ImageMetaDataRW::" + origin, record);
}

template <class T> template <class U> String ImageMetaDataRW<T>::_quotify(const U& val) {
    ostringstream oss;
    DataType x = whatType(&val);
    if (x == TpOther && typeid(ValueHolder) == typeid(val)) {
        x = ((ValueHolder)val).dataType();
    }
    if(x == TpString) {
        oss << "\"" << val << "\"";
    }
    else if (x == TpChar || x == TpUChar) {
        oss << "'" << val << "'";
    }
    else if (x == TpFloat || x == TpComplex) {
        oss << std::setprecision(6) << val;
    }
    else if (x == TpDouble || x == TpDComplex) {
        oss << std::setprecision(12) << val;
    }
    else {
        oss << val;
    }
    return oss.str();
}

}

#endif
