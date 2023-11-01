/*
 * LinearMosaic.h  Linear mosaicing code
//# Copyright (C) 2015
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

 *
 *
 *  Created on: Feb 24, 2015
 *      Author: kgolap
 */

#ifndef SYNTHESIS_LINEARMOSAIC_H
#define SYNTHESIS_LINEARMOSAIC_H

#include <casacore/casa/Arrays/ArrayFwd.h>

namespace casacore{

  template<class T> class ImageInterface;
  class CoordinateSystem;
}

namespace casa { //# NAMESPACE CASA - BEGIN
  //Forward declaration
  // <summary> Class that contains functions needed for feathering</summary>

  class LinearMosaic{

  public:
	  LinearMosaic();
	  LinearMosaic(const casacore::String outim, const casacore::String outwgt, const casacore::MDirection& imcen, const casacore::Int nx, const casacore::Int ny,
			  const casacore::Quantity cellx=casacore::Quantity(0.0, "arcsec"), const casacore::Quantity celly=casacore::Quantity(0.0, "arcsec"), const casacore::Int linmostype=2);
	  //casacore::Coordinate System of output image along with the different pointing images and weight images
	  // linmostype 1 or 2.. ie. mosaic in PB or PB^2
	  LinearMosaic(const casacore::String outim, const casacore::String outwgt, const casacore::MDirection& imcen, const casacore::Int nx, const casacore::Int ny,
			  	  casacore::Vector<casacore::CountedPtr<casacore::ImageInterface<casacore::Float> > >& ims,
			  	  casacore::Vector<casacore::CountedPtr<casacore::ImageInterface<casacore::Float> > >& wgtims, const casacore::Int linmostype=2);
	  casacore::Bool makeMosaic(casacore::ImageInterface<casacore::Float>& outim, casacore::ImageInterface<casacore::Float>& outwgt,
			  casacore::Vector<casacore::CountedPtr<casacore::ImageInterface<casacore::Float> > >& ims,
			  casacore::Vector<casacore::CountedPtr<casacore::ImageInterface<casacore::Float> > >& wgtims);
	  casacore::Bool makeMosaic(casacore::Vector<casacore::CountedPtr<casacore::ImageInterface<casacore::Float> > >& ims,
		  	   casacore::Vector<casacore::CountedPtr<casacore::ImageInterface<casacore::Float> > >& wgtims);

	  // outim is weighted 0: flux correct, 1: flat noise, 2: noise optimal
	  void setOutImages(casacore::ImageInterface<casacore::Float>& outim, casacore::ImageInterface<casacore::Float>& outwgt, const casacore::Int imageWeightType=1, const casacore::Int weightType=2);
	  // Disk based image of the above
	  void setlinmostype(const casacore::Int linmostype);
	  void saultWeightImage(const casacore::String& outimname, const casacore::Float& fracPeakWgt);
	  void setOutImages(const casacore::String& outim, const casacore::String& outwgt, const casacore::Int imageWeightType=1, const casacore::Int weightType=2);
  private:
	  casacore::Bool addOnToImage(casacore::ImageInterface<casacore::Float>& outim, casacore::ImageInterface<casacore::Float>& outwgt, const casacore::ImageInterface<casacore::Float>& inIm,
			  const casacore::ImageInterface<casacore::Float>& inWgt, casacore::Bool unWeightOutImage=true);
	  void makeEmptyImage(const casacore::String imagename, const casacore::CoordinateSystem& cs, const casacore::MDirection& imcen, const casacore::Int nx, const casacore::Int ny, const casacore::Int npol, const casacore::Int nchan);
	  void createOutImages(const casacore::CoordinateSystem& cs, const casacore::Int npol, const casacore::Int nchan );
	  casacore::CountedPtr<casacore::ImageInterface<casacore::Float> > outImage_p, outWgt_p;
	  casacore::String outImName_p, outWgtName_p;
	  casacore::Int nx_p, ny_p;
	  casacore::MDirection imcen_p;
	  casacore::Quantity cellx_p, celly_p;

	  casacore::Int imageWeightType_p, weightType_p;
	  casacore::Int linmosType_p;
};



} //# NAMESPACE CASA - END



#endif /* SYNTHESISLINEARMOSAIC_H */
