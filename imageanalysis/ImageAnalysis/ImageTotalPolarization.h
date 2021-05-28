//# Copyright (C) 1996,1997,1998,1999,2000,2001,2002
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

#ifndef IMAGEANALYSIS_IMAGETOTALPOLARIZATION_H
#define IMAGEANALYSIS_IMAGETOTALPOLARIZATION_H

#include <imageanalysis/ImageAnalysis/ImagePolTask.h>

namespace casa {

// <summary>
// Compute total polarization intensity
// </summary>

// <use visibility=export>

// <prerequisite>
//   <li> <linkto class=casacore::ImageExpr>ImageExpr</linkto>
//   <li> <linkto class=casacore::ImageInterface>ImageInterface</linkto>
// </prerequisite>

// <etymology>
//  Polarimetric analysis of Images
// </etymology>

// <synopsis>

// </synopsis>
//
// <motivation>
// Basic image analysis capability
// </motivation>

// <todo asof="1999/11/01">
// </todo>

class ImageTotalPolarization : public ImagePolTask {
public:

    ImageTotalPolarization() = delete;

    ImageTotalPolarization(
        const SPCIIF image, const casacore::String& outname,
        casacore::Bool overwrite
    );

    virtual ~ImageTotalPolarization();

    String getClass() const;

    void setClip(casacore::Float clip);

    void setDebias(casacore::Bool debias);

    void setSigma(casacore::Float sigma);

    SPIIF compute();

private:
    casacore::Bool _debias = False;
    casacore::Float _clip = 10.0;
    casacore::Float _sigma = -1;

    const static String CLASS_NAME;

    casacore::ImageExpr<Float> _totPolInt();

};

}

#endif
