//# ImageMetaData.h: Meta information for Images
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

#ifndef IMAGEANALYSIS_IMAGEMETADATA_H
#define IMAGEANALYSIS_IMAGEMETADATA_H

#include <imageanalysis/ImageAnalysis/ImageMetaDataBase.h>

#include <casacore/images/Images/ImageInterface.h>
#include <casacore/casa/aips.h>

#include <memory>

namespace casa {

// <summary>
// A class in which to store and allow read-only access to image metadata.
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">
// </reviewed>

// <prerequisite>
//   <li> <linkto class=casacore::ImageInterface>ImageInterface</linkto>
// </prerequisite>

// <etymology>
// The ImageMetaData class name is derived from its role as holding image metadata.
// </etymology>

// <synopsis> 
// The ImageMetaData object is meant to allow access to image metadata (eg, shape,
// coordinate system info such as spectral and polarization axes numbers, etc).
// </synopsis>

// <example>
// Construct an object of this class by passing the associated image to the constructor.
// <srcblock>
// casacore::PagedImage<casacore::Float> myImage("myImage");
// ImageMetaData<casacore::Float> myImageMetaData(myImage);
// </srcblock>
// </example>

// <motivation> 
// This class is meant to provide an object-oriented interface for accessing
// image metadata without polluting the casacore::ImageInterface and CoordinateSystem
// classes with these methods.
// </motivation>
// <todo>
// Merge casacore::ImageInfo class into this class.
// </todo>

template <class T> class ImageMetaData : public ImageMetaDataBase<T> {

public:

    ImageMetaData() = delete;

    ImageMetaData(SPCIIT image);

    ~ImageMetaData() {}

    casacore::Record toRecord(casacore::Bool verbose) const;

    // For ia.summary() moved from ImageAnalysis
    casacore::Record summary(
        const casacore::String& doppler, const casacore::Bool list,
        const casacore::Bool pixelorder, const casacore::Bool verbose
    );

protected:

    const casacore::ImageInfo& _getInfo() const { return _info; }

    const casacore::CoordinateSystem& _getCoords() const { return _csys; }

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

    casacore::Record _getStatistics() const;

    casacore::String _getTelescope() const;

    casacore::Vector<casacore::String> _getStokes() const;

private:
    SPCIIT _image;

    const casacore::ImageInfo _info;
    const casacore::CoordinateSystem _csys;

    // These are mutable because they are only to be set once and
    // then cached. If this contract is broken, and they are set elsewhere
    // defects will likely occur.
    mutable casacore::Record _header;
    mutable casacore::String _bunit, _imtype, _object, _equinox,
        _projection, _observer, _telescope, _reffreqtype;
    mutable casacore::MEpoch _obsdate;
    mutable casacore::Quantity _restFreq;
    mutable casacore::GaussianBeam _beam;
    mutable casacore::Vector<casacore::String> _masks, _stokes;
    mutable casacore::Vector<casacore::String> _axisNames, _axisUnits;
    mutable casacore::Vector<casacore::Double> _refPixel;
    mutable std::vector<casacore::Quantity> _refVal, _increment;
    mutable casacore::Record _stats;
};

}

#ifndef AIPS_NO_TEMPLATE_SRC
#include <imageanalysis/ImageAnalysis/ImageMetaData.tcc>
#endif //# AIPS_NO_TEMPLATE_SRC

#endif
