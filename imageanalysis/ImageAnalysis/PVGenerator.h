//# tSubImage.cc: Test program for class SubImage
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
//# $Id: tSubImage.cc 20567 2009-04-09 23:12:39Z gervandiepen $

#ifndef IMAGES_PVGENERATOR_H
#define IMAGES_PVGENERATOR_H

#include <imageanalysis/ImageAnalysis/ImageTask.h>
#include <casa/namespace.h>


namespace casacore{

class MDirection;
}

namespace casa {

class PVGenerator : public ImageTask<casacore::Float> {
    // <summary>
    // Top level interface for generating position-velocity images
    // </summary>

    // <reviewed reviewer="" date="" tests="" demos="">
    // </reviewed>

    // <prerequisite>
    // </prerequisite>

    // <etymology>
    // Collapses image.
    // </etymology>

    // <synopsis>
    // High level interface for generating position-velocity images.
    // </synopsis>

    // <example>
    // <srcblock>
    // ImageCollapser collapser();
    // collapser.collapse();
    // </srcblock>
    // </example>

public:

    PVGenerator() = delete;

    // The region selection in the constructor only applies to the non-direction coordinates.
    // The direction coordinate limits are effectively set by calling setEndPoints()
    // after construction. The region selection in the constructor is only for things like
    // spectral selection and polarization selection. In addition at most one of <src>regionRec</src>
    // and <src>chanInp/stokes</src> should be supplied. If specifying <src>regionRec</src> that should
    // be a non-null pointer and chanInp and stokes should both be empty strings. If specifying either or
    // both of chanInp and/or stokes, the one(s) being specified should be non-empty strings corresponding
    // to correct syntax for that particular parameter, and regionRec should be null.
    // If you specify <src>regionRec</src>=0
    // and <src>stokes</src>="", and <src>chanInp</src>="", that implies you want to use all
    // spectral channels and all polarization planes in the input image.
    PVGenerator(
        const SPCIIF image,
        const casacore::Record *const &regionRec, const casacore::String& chanInp,
        const casacore::String& stokes, const casacore::String& maskInp,
        const casacore::String& outname, const casacore::Bool overwrite
    );

    PVGenerator(const PVGenerator&) = delete;

    // destructor
    ~PVGenerator();

    // perform the collapse. Returns a pointer to the
    // collapsed image.
    SPIIF generate() const;

    // set the end points of the slice in direction space. casacore::Input values represent pixel
    // coordinates in the input image.
    void setEndpoints(
        const std::pair<casacore::Double, casacore::Double>& start,
        const std::pair<casacore::Double, casacore::Double>& end
    );

    // set end points given center in pixels, length of segment in pixels, and position angle
    // taken in the normal astronomical sense, measured from north through east.
    void setEndpoints(
        const std::pair<casacore::Double, casacore::Double>& center, casacore::Double length,
        const casacore::Quantity& pa
    );

    void setEndpoints(
        const std::pair<casacore::Double, casacore::Double>& center, const casacore::Quantity& length,
        const casacore::Quantity& pa
    );

    void setEndpoints(
        const casacore::MDirection& center, const casacore::Quantity& length,
        const casacore::Quantity& pa
    );

    // <src>length in pixels</src>
    void setEndpoints(
        const casacore::MDirection& center, casacore::Double length,
        const casacore::Quantity& pa
    );

    void setEndpoints(
        const casacore::MDirection& start, const casacore::MDirection& end
    );

    // Set the number of pixels perpendicular to the slice for which averaging
    // should occur. Must be odd and >= 1. 1 => just use the pixels coincident with the slice
    // (no averaging). 3 => Average three pixels, one pixel on either side of the slice and the
    // pixel lying on the slice.
    // Note this average is done after the image has been rotated.
    void setWidth(casacore::uInt width);
    // This will set the width by rounding <src>q</src> up so that the width is an odd number of pixels.
    void setWidth(const casacore::Quantity& q);

    casacore::String getClass() const;

    // set the unit to be used for the offset axis in the resulting image (from calling
    // generate()). Must conform to angular units
    void setOffsetUnit(const casacore::String& s);


protected:
    inline  CasacRegionManager::StokesControl _getStokesControl() const {
        return CasacRegionManager::USE_ALL_STOKES;
    }

    inline std::vector<casacore::Coordinate::Type> _getNecessaryCoordinates() const {
        std::vector<casacore::Coordinate::Type> v;
        v.push_back(casacore::Coordinate::SPECTRAL);
        v.push_back(casacore::Coordinate::DIRECTION);
        return v;
     }

    virtual casacore::Bool _mustHaveSquareDirectionPixels() const {return true;}

private:
    std::unique_ptr<std::vector<casacore::Double> > _start, _end;
    casacore::uInt _width;
    casacore::String _unit;
    static const casacore::String _class;

    void _checkRotatedImageSanity(
        SPCIIF rotated, const Vector<Double>& rotPixStart,
        const Vector<Double>& rotPixEnd, Int xAxis, Int yAxis,
        Double xdiff, Double ydiff
    ) const;

    SPIIF _doCollapse(
        Int& collapsedAxis, SPCIIF rotated, Int xAxis, Int yAxis, const Vector<Double>& rotPixStart,
        const Vector<Double>& rotPixEnd, Double halfwidth
    ) const;

    SPCIIF _doRotate(
        SPIIF subImage, const std::vector<Double>& start, const std::vector<Double>& end,
        Int xAxis, Int yAxis, Double halfwidth, Double paInRad
    ) const;

    SPIIF _dropDegen(SPIIF collapsed, Int collapsedAxis) const;

    void _checkWidth(const casacore::Int64 xShape, const casacore::Int64 yShape) const;

    void _checkWidthSanity(
        Double paInRad, Double halfwidth, const std::vector<Double>& start,
        const std::vector<Double>& end, SPCIIF subImage, Int xAxis, Int yAxis
    ) const;

    casacore::Quantity _increment() const;

    void _moveRefPixel(
        SPIIF subImage, CoordinateSystem& subCoords, const std::vector<Double>& start,
        const std::vector<Double>& end, Double paInDeg, Int xAxis, Int yAxis
    ) const;
    
    static casacore::String _pairToString(const std::pair<casacore::Double, casacore::Double>& p);

};
}

#endif
