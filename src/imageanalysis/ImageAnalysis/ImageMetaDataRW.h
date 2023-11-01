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
//# $Id$

#ifndef IMAGEANALYSIS_IMAGEMETADATARW_H
#define IMAGEANALYSIS_IMAGEMETADATARW_H

#include <imageanalysis/ImageAnalysis/ImageMetaDataBase.h>

#include <casacore/casa/aips.h>
#include <memory>

namespace casa {

// <summary>
// A class in which to store and allow read-write access to image metadata.
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">
// </reviewed>

// <prerequisite>
//   <li> <linkto class=casacore::ImageInterface>ImageInterface</linkto>
// </prerequisite>

// <etymology>
// The ImageMetaDataRW class name is derived from its role as holding image metadata
// and providing read-write access.
// </etymology>

// <synopsis> 
// The ImageMetaDataRW object is meant to allow access to image metadata (eg, shape,
// coordinate system info such as spectral and polarization axes numbers, etc). It allows
// write access to some values. This class contains the writable methods and extends
// ImageMetaData which contains the read-only methods.
// </synopsis>

// <example>
// Construct an object of this class by passing the associated image to the constructor.
// <srcblock>
// casacore::PagedImage<casacore::Float> myImage("myImage");
// ImageMetaDataRW<casacore::Float> myImageMetaData(myImage);
// </srcblock>
// </example>

// <motivation> 
// This class is meant to provide an object-oriented interface for accessing
// image metadata without polluting the casacore::ImageInterface and CoordinateSystem
// classes with these methods.
// </motivation>

template <class T> class ImageMetaDataRW : public ImageMetaDataBase<T> {

public:

    ImageMetaDataRW() = delete;

    ImageMetaDataRW(SPIIT image);

    // remove, if possible, the specified parameter. Returns true if removal
    // was successful.
    casacore::Bool remove(const casacore::String& key);

    // remove the specified mask. If the empty string is given, all masks will
    // be removed. Returns true if successful, false otherwise or if the specfied
    // mask does not exist.
    casacore::Bool removeMask(const casacore::String& maskName);

    casacore::Record toRecord(casacore::Bool verbose) const;

    // add a key-value pair
    casacore::Bool add(const casacore::String& key, const casacore::ValueHolder& value);

    // set (update) the value associated with the key.
    casacore::Bool set(const casacore::String& key, const casacore::ValueHolder& value);

    // set the coordinate system from a Record.
    void setCsys(const casacore::Record& coordinates);

protected:
    const casacore::ImageInfo& _getInfo() const;

    const casacore::CoordinateSystem& _getCoords() const;

    casacore::Vector<casacore::String> _getAxisNames() const;

    casacore::Vector<casacore::String> _getAxisUnits() const;

    casacore::GaussianBeam _getBeam() const;

    casacore::String _getBrightnessUnit() const;

    casacore::String _getImType() const;

    std::vector<casacore::Quantity> _getIncrements() const;

    casacore::Vector<casacore::String> _getMasks() const;

    casacore::String _getObject() const;

    casacore::String _getEquinox() const;

    casacore::MEpoch _getObsDate() const;

    casacore::String _getObserver() const;

    casacore::String _getProjection() const;

    casacore::String _getRefFreqType() const;

    casacore::Vector<casacore::Double> _getRefPixel() const;

    casacore::Vector<casacore::Quantity> _getRefValue() const;

    casacore::Quantity _getRestFrequency() const;

    casacore::String _getTelescope() const;

    casacore::Record _getStatistics() const;

    casacore::Vector<casacore::String> _getStokes() const;

private:
    SPIIT _image;

    // These are mutable because they are only to be set once and
    // then cached. If this contract is broken, and they are set elsewhere
    // defects will likely occur.
    mutable casacore::String _bunit, _imtype, _object, _equinox, _observer, _projection,
        _reffreqtype, _telescope;
    mutable casacore::MEpoch _obsdate;
    mutable casacore::Quantity _restFreq;
    mutable casacore::Vector<casacore::String> _masks, _stokes;
    mutable casacore::GaussianBeam _beam;
    mutable casacore::Vector<casacore::String> _axisNames, _axisUnits;
    mutable casacore::Vector<casacore::Double> _refPixel;
    mutable std::vector<casacore::Quantity> _refVal, _increment;
    mutable casacore::Record _header, _stats;

    std::unique_ptr<casacore::CoordinateSystem> _makeCoordinateSystem(
        const casacore::Record& coordinates, const casacore::IPosition& shape
    );

    void _setCoordinateValue(const casacore::String& key, const casacore::ValueHolder& value);

    casacore::String  _getString(const casacore::String& key, const casacore::ValueHolder& value) const;

    void _setUserDefined(const casacore::String& key, const casacore::ValueHolder& v);

    casacore::Bool _setUnit(const casacore::String& unit);

    casacore::Bool _setCsys(const casacore::CoordinateSystem& csys);

    casacore::Bool _setImageInfo(const casacore::ImageInfo& info);

    const casacore::TableRecord _miscInfo() const;

    void _setMiscInfo(const casacore::TableRecord& rec);

    casacore::Bool _hasRegion(const casacore::String& maskName) const;

    static casacore::Quantity _getQuantity(const casacore::ValueHolder& v);

    casacore::Bool _isWritable() const;

    template <class U, class V> void _modHistory(
        const casacore::String& func, const casacore::String& keyword,
        const U& oldVal, const V& newVal
    );

    template <class U> void _addHistory(
        const casacore::String& func, const casacore::String& keyword, const U& newVal
    );

    void _toHistory(const casacore::String& origin, const casacore::String& record);

    template <class U> static casacore::String _quotify(const U& val);
};

}

#ifndef AIPS_NO_TEMPLATE_SRC
#include <imageanalysis/ImageAnalysis/ImageMetaDataRW.tcc>
#endif //# AIPS_NO_TEMPLATE_SRC

#endif
