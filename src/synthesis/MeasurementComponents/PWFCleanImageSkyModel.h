//# PWFCleanImageSkyModel.h: Parallelized version of WFCleanImageSkyModel
//# Copyright (C) 1996,1997,1998,1999,2002
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
//# $Id$

#ifndef SYNTHESIS_PWFCLEANIMAGESKYMODEL_H
#define SYNTHESIS_PWFCLEANIMAGESKYMODEL_H

#include <synthesis/MeasurementComponents/WFCleanImageSkyModel.h>

namespace casa { //# NAMESPACE CASA - BEGIN

//forward
class SkyEquation;

// <summary> 
// Parallelized version of class WFCleanImageSkyModel
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//   <li> <linkto class=ImageSkyModel>ImageSkyModel</linkto> module
//   <li> <linkto class=WFCleanImageSkyModel>WFCleanImageSkyModel</linkto> module
// </prerequisite>
//
// <etymology>
// From "parallel", "wide-field" and "image sky model"
// </etymology>
//
// <synopsis> 
// This class is a parallelized version of class WFCleanImageSkyModel,
// implemented as an inherited specialization. The parallel class
// uses the same algorithms as the serial parent class, except it
// is modified to support parallel processing.
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

class PWFCleanImageSkyModel : public WFCleanImageSkyModel {

public:
  // Constructors
  PWFCleanImageSkyModel();
  PWFCleanImageSkyModel(const casacore::Int nfacets);

  // Null destructor
  virtual ~PWFCleanImageSkyModel(){};

  // Parallelized solve for the sky model
  virtual casacore::Bool solve(SkyEquation& se);

  // Parallelized formation of the approximate PSF's
  virtual void makeApproxPSFs(SkyEquation& se);
};


} //# NAMESPACE CASA - END

#endif
