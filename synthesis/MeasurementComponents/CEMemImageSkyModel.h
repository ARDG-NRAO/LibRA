//# CEMemImageSkyModel.h: Definition for CEMemImageSkyModel
//# Copyright (C) 1996,1997,1998,1999,2000
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

#ifndef SYNTHESIS_CEMEMIMAGESKYMODEL_H
#define SYNTHESIS_CEMEMIMAGESKYMODEL_H

#include <synthesis/MeasurementComponents/CleanImageSkyModel.h>
namespace casa { //# NAMESPACE CASA - BEGIN

//forward
class SkyEquation;
class CEMemProgress;

// <summary> Image Sky Model for the Cornwell-Evens maximum entropy algorithm </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//   <li> <linkto class=ImageSkyModel>ImageSkyModel</linkto> class
//   <li> <linkto class=CEMemLatModel>CEMemModel</linkto> class
// </prerequisite>
//
// <etymology>
// CEMemImageSkyModel implements the Cornwell-Evens MEM algorithm.
// It is derived from <linkto class=CleanImageSkyModel>CleanImageSkyModel</linkto>.
// </etymology>
//
// <synopsis> 
// We need to add more stuff here.
//
//
//
// Masking is optionally performed using a mask image: only points
// where the mask is non-zero are cleaned. If no mask is specified
// all points in the inner quarter of the image are cleaned.
// </synopsis> 
//
// <example>
// See the example for <linkto class=CleanImageSkyModel>CleanImageSkyModel</linkto>.
// </example>
//
// <motivation>
// </motivation>
//
// <todo asof="99/11/01">
// <ul> Some of the "CleanImageSkyModel" functionality should be moved to "ImageSkyModel"
// <ul> Create another virtual base class, "MemImageSkyModel", derived from "ImageSkyModel"
// <ul> THEN: derive CEMemImageSkyModel from "MemImageSkyModel"
// <ul> However, now we are just doing what works quickly: deriving from CleanImageSkyModel
// </todo>

class CEMemImageSkyModel : public CleanImageSkyModel {
public:

  // constructor
  CEMemImageSkyModel(casacore::Float sigma, 
		     casacore::Float targetFlux,
		     casacore::Bool constrainFlux,
		     const casacore::Vector<casacore::String>& priors,
		     const casacore::String& entropy);

  // destructor
  ~CEMemImageSkyModel();

  // Solve for this SkyModel
  virtual casacore::Bool solve (SkyEquation& me);

  // Set control parameters which are different from Clean:
  //<group>
  void setSigma(const casacore::Float targetSigma) {itsSigma = targetSigma; }
  void setTargetFlux(const casacore::Float targetFlux) {itsTargetFlux = targetFlux; }
  void setConstrainTargetFlux(const casacore::Bool constrainFlux) { itsConstrainFlux = constrainFlux; }
  void setPrior(const casacore::Vector<casacore::String>& prior) { itsPrior = prior; }
  void setEntropy(const casacore::String& ent) { itsEntropy = ent; }
  //</group>

  // Get the various control parameters which are different from Clean:
  //<group>
  casacore::Float sigma() { return itsSigma; }
  casacore::Float targetFlux() { return itsTargetFlux; }
  casacore::Bool constrainFlux() { return itsConstrainFlux; }
  casacore::Vector<casacore::String> prior() { return itsPrior; }
  casacore::String entropy() { return itsEntropy; }
  //</group>

protected:

  casacore::Bool initializeModel() { return itsInitializeModel; }
  
  casacore::Float itsSigma;
  casacore::Float itsTargetFlux;
  casacore::Bool itsConstrainFlux;
  casacore::Vector<casacore::String> itsPrior;
  casacore::String itsEntropy;
  casacore::Bool itsInitializeModel;

  CEMemProgress *itsProgress;

};


} //# NAMESPACE CASA - END

#endif


