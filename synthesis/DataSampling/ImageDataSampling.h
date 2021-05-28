//# ImageDataSampling.h: Definition for ImageDataSampling
//# Copyright (C) 1996,1997,1998,1999,2000,2001
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
//# Correspondence concerning AIPS++ should be adressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//#
//# $Id$

#ifndef SYNTHESIS_IMAGEDATASAMPLING_H
#define SYNTHESIS_IMAGEDATASAMPLING_H

#include <synthesis/DataSampling/DataSampling.h>
#include <synthesis/TransformMachines/SkyJones.h>
#include <images/Images/ImageInterface.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/Vector.h>
#include <casa/Arrays/Matrix.h>
#include <coordinates/Coordinates/DirectionCoordinate.h>

namespace casa { //# NAMESPACE CASA - BEGIN

// <summary> Provides sampling of single dish data for esimation algorithms
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
// </prerequisite>
//
// <etymology>
// Samples single dish casacore::Data as needed for various estimation algorithms
// </etymology>
//
// <synopsis> 
// Esimation algorithms such as the Pixon method need sampled
// and unitless versions of the data. This class is derived from
// DataSampling. It adds methods specific to single dish data.
// </synopsis> 
//
// <example>
// </example>
//
// <motivation>
// Define an interface to allow efficient processing of chunks of 
// data
// </motivation>
//
// <todo asof="01/03/03">
// <ul> Derive more examples
// </todo>


class ImageDataSampling : public DataSampling {
public:

  ImageDataSampling(const casacore::ImageInterface<casacore::Float>& dirty,
		    const casacore::ImageInterface<casacore::Float>& psf,
		    const casacore::ImageInterface<casacore::Float>& sigma);

  ImageDataSampling(const casacore::ImageInterface<casacore::Float>& dirty,
		    const casacore::ImageInterface<casacore::Float>& psf,
		    const casacore::Float sigma);

  // Copy constructor
  ImageDataSampling(const ImageDataSampling &other);

  // Assignment operator
  ImageDataSampling &operator=(const ImageDataSampling &other);

  ~ImageDataSampling();

private:

  void init(const casacore::ImageInterface<casacore::Float>& dirty,
	    const casacore::ImageInterface<casacore::Float>& psf,
	    const casacore::ImageInterface<casacore::Float>* sigmaImagePtr,
	    const casacore::Float sigma);

  void ok();

};

} //# NAMESPACE CASA - END

#endif
