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

#ifndef IMAGEANALYSIS_IMAGEMETADATABASE_H
#define IMAGEANALYSIS_IMAGEMETADATABASE_H

#include <images/Images/ImageInterface.h>
#include <imageanalysis/ImageTypedefs.h>
#include <casa/aips.h>

namespace casa {

// <summary>
// Base class for image metadata access.
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">
// </reviewed>

// <prerequisite>
//   <li> <linkto class=casacore::ImageInterface>ImageInterface</linkto>
// </prerequisite>

// <etymology>
// </etymology>

// <synopsis> 
// </synopsis>

// <example>
// </example>

// <motivation> 

// </motivation>

class ImageMetaDataConstants {
public:
    const static casacore::String MASKS;
    
    const static casacore::String _BEAMMAJOR, _BEAMMINOR, _BEAMPA, _BMAJ, _BMIN, _BPA,
        _BUNIT, _CDELT, _CRPIX, _CRVAL, _CTYPE, _CUNIT, _DATAMAX, _DATAMIN,
        _EPOCH, _EQUINOX, _IMTYPE, _MAXPIXPOS, _MAXPOS, _MINPIXPOS,
        _MINPOS, _OBJECT, _OBSDATE, _OBSERVER, _PROJECTION,
        _RESTFREQ, _REFFREQTYPE, _SHAPE, _TELESCOPE;

};

template <class T> class ImageMetaDataBase {

public:


    ImageMetaDataBase() = delete;
    
    virtual ~ImageMetaDataBase() {}

    casacore::CoordinateSystem coordsys(const std::vector<casacore::Int>& axes) const;

    casacore::DataType dataType() const;

    casacore::uInt nChannels() const;

    // Is the specified channel number valid for this image?
    casacore::Bool isChannelNumberValid(const casacore::uInt chan) const;

    // Get the pixel number on the polarization axis of the specified stokes parameter.
    // If the specified stokes parameter does not exist in the image, the value returned
    // is not gauranteed to be anything other than outside the range of 0 to nStokes-1
    // inclusive. Return -1 if the specified stokes parameter is not present or
    // if this image does not have a polarization axis.
 
    casacore::Int stokesPixelNumber(const casacore::String& stokesString) const;

    // get the stokes parameter at the specified pixel value on the polarization axis.
    // returns "" if the specified pixel is out of range or if no polarization axis.

    casacore::String stokesAtPixel(const casacore::uInt pixel) const;

    // Get the number of stokes parameters in this image.
    casacore::uInt nStokes() const;

    // is the specified stokes parameter present in the image?
    casacore::Bool isStokesValid(const casacore::String& stokesString) const;

    // Get the shape of the direction axes. Returns a two element
    // casacore::Vector if there is a direction coordinate, if not returns a zero element
    // vector.

    casacore::Vector<casacore::Int> directionShape() const;

    // if the specified stokes parameter is valid. A message suitable for
    // error notification is returned in the form of an in-out parameter
    //if one or both of these is invalid.
    casacore::Bool areChannelAndStokesValid(
        casacore::String& message, const casacore::uInt chan, const casacore::String& stokesString
    ) const;

    // get the bounding box in the image for the specified region.
    casacore::Record* getBoundingBox(const casacore::Record& region) const;

    // convert the header info to a casacore::Record and list to logger if verbose=true
    virtual casacore::Record toRecord(casacore::Bool verbose) const = 0;

    // get the value of the datum corresponding to the given casacore::FITS keyword.
    casacore::ValueHolder getFITSValue(const casacore::String& key) const;

    // if doVelocity=true, compute spectral velocities as well as frequencies, if false, only
    // compute frequencies.
    // <src>dirFrame</src> and <src>freqFrame</src> are the codes for the
    // frames for which it is desired that the returned measures should be specified.
    // In both cases, one can specify "native" for the native coordinate frame,
    // "cl" for the conversion layer frame, or any valid frame string from casacore::MDirection::showType()
    // or casacore::MFrequency::showType().
    casacore::Record toWorld(
        const casacore::Vector<double>& value, const casacore::String& format = "n",
        casacore::Bool doVelocity=true, const casacore::String& dirFrame="cl",
        const casacore::String& freqFrame="cl"
    ) const;

protected:
    ImageMetaDataBase(SPCIIT);
    
    casacore::LogIO& _getLog() { return _log; }

    virtual const casacore::ImageInfo& _getInfo() const = 0;

    virtual const casacore::CoordinateSystem& _getCoords() const = 0;

    virtual casacore::Vector<casacore::String> _getAxisNames() const = 0;

    virtual casacore::Vector<casacore::String> _getAxisUnits() const = 0;

    virtual casacore::GaussianBeam _getBeam() const = 0;

    virtual casacore::String _getBrightnessUnit() const = 0;

    virtual casacore::String _getImType() const = 0;

    virtual std::vector<casacore::Quantity> _getIncrements() const = 0;

    virtual casacore::Vector<casacore::String> _getMasks() const = 0;

    virtual casacore::String _getObject() const = 0;

    virtual casacore::String _getEquinox() const = 0;

    virtual casacore::MEpoch _getObsDate() const = 0;

    casacore::String _getEpochString() const;

    virtual casacore::String _getObserver() const = 0;

    virtual casacore::String _getProjection() const;

    virtual casacore::String _getRefFreqType() const = 0;

    virtual casacore::Vector<casacore::Double> _getRefPixel() const = 0;

    virtual casacore::Vector<casacore::Quantity> _getRefValue() const = 0;

    virtual casacore::Quantity _getRestFrequency() const = 0;

    casacore::IPosition _getShape() const;

    virtual casacore::Record _getStatistics() const = 0;

    virtual casacore::String _getTelescope() const = 0;

    casacore::Record _makeHeader() const;

    void _toLog(const casacore::Record& header) const;

    casacore::uInt _getAxisNumber(const casacore::String& key) const;

    casacore::Record _calcStats() const;

    const casacore::TableRecord _miscInfo() const;

    casacore::uInt _ndim() const;

    virtual casacore::Vector<casacore::String> _getStokes() const = 0;

private:
    SPCIIT _image;
    mutable casacore::LogIO _log;
    mutable casacore::IPosition _shape;

    // precision < 0 => use default precision when printing numbers
    void _fieldToLog(const casacore::Record& header, const casacore::String& field, casacore::Int precision=-1) const;

    casacore::String _doStandardFormat(casacore::Double value, const casacore::String& unit) const;

    casacore::Record _calcStatsT(
        std::shared_ptr<const casacore::ImageInterface<T> > image
    ) const;

    static casacore::Record _worldVectorToRecord(
        const casacore::CoordinateSystem& csys, const casacore::Vector<casacore::Double>& world, casacore::Int c,
        const casacore::String& format,
        casacore::Bool isAbsolute, casacore::Bool showAsAbsolute, casacore::Bool doVelocity,
        casacore::MDirection::Types dirFrame, casacore::MFrequency::Types freqFrame
    );

    static casacore::Record _worldVectorToMeasures(
        const casacore::CoordinateSystem& csys,
        const casacore::Vector<casacore::Double>& world, casacore::Int c, casacore::Bool abs, casacore::Bool doVelocity,
        casacore::MDirection::Types dirFrame, casacore::MFrequency::Types freqFrame
    );

};

}

#ifndef AIPS_NO_TEMPLATE_SRC
#include <imageanalysis/ImageAnalysis/ImageMetaDataBase.tcc>
#endif //# AIPS_NO_TEMPLATE_SRC

#endif
