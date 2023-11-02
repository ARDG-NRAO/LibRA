//# HogbomCleanImageSkyModel.h: Definition for HogbomCleanImageSkyModel
//# Copyright (C) 1996,1997,1998,1999,2003
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

#ifndef SYNTHESIS_HOGBOMCLEANIMAGESKYMODEL_H
#define SYNTHESIS_HOGBOMCLEANIMAGESKYMODEL_H

#include <synthesis/MeasurementComponents/CleanImageSkyModel.h>
namespace casacore{

template <class T> class RO_LatticeIterator;
}

namespace casa { //# NAMESPACE CASA - BEGIN

//forward
class SkyEquation;
// <summary> 
// Hogbom Clean Image Sky Model: Image Sky Model implementing the Hogbom Clean algorithm
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//   <li> <linkto class=ImageSkyModel>ImageSkyModel</linkto> module
// </prerequisite>
//
// <etymology>
// HogbomCleanImageSkyModel implements the Hogbom Clean algorithm.
// It is derived from <linkto class=SkyModel>SkyModel</linkto> and uses the
// fortran code in hclean.f
// </etymology>
//
// <synopsis> 
// The Hogbom Clean is the standard classic clean.
//
// The Hogbom Clean is implemented using the 
// hclean.f code.
//
// Masking is optionally performed using a mask image: only points
// where the mask is non-zero are cleaned. If no mask is specified
// all points in the inner quarter of the image are cleaned.
// </synopsis> 
//
// <example>
// See the example for <linkto class=SkyModel>SkyModel</linkto>.
// </example>
//
// <motivation>
// </motivation>
//
// <todo asof="97/10/01">
// <ul> Allow specification of more control parameters
// </todo>

class HogbomCleanImageSkyModel : public CleanImageSkyModel {
public:

  // Solve for this SkyModel
  virtual casacore::Bool solve (SkyEquation& me);
  casacore::Matrix<casacore::Float>* makeMaskMatrix(const casacore::Int& nx, 
				const casacore::Int& ny,
				casacore::Int& newNx,
				casacore::Int& newNy,
				casacore::RO_LatticeIterator<casacore::Float>& maskIter,
				casacore::Int& xbeg,
				casacore::Int& xend,
				casacore::Int& ybeg,
				casacore::Int& yend);

private:
};


} //# NAMESPACE CASA - END

#endif


