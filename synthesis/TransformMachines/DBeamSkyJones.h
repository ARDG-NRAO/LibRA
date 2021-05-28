//# DBeamSkyJones.h: Definitions of interface for DBeamSkyJones 
//# Copyright (C) 1996,1997,1998,2000
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

#ifndef SYNTHESIS_DBEAMSKYJONES_H
#define SYNTHESIS_DBEAMSKYJONES_H

#include <casa/aips.h>
#include <measures/Measures/Stokes.h>
#include <synthesis/TransformMachines/BeamSkyJones.h>

namespace casa { //# NAMESPACE CASA - BEGIN

//#forward
class SkyModel;

//# Need forward declaration for Solve in the Jones Matrices
class SkyEquation;

// <summary> Model the polarization leakage in the Sky Jones Matrices </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
// <li> <linkto class="SkyEquation">SkyEquation</linkto> class
// <li> <linkto class="BeamSkyJones">BeamSkyJones</linkto> class
// </prerequisite>
//
// <etymology>
// DBeam = sky-position-dependent polarization leakage, 
// SkyJones = Sky-based Jones matrices.
// This class only deals with the off-diagonal elements of the
// primary beam jones matrices.
// </etymology>
//
// <synopsis> 
// </synopsis> 
//
// <example>
// <srcblock>
// </srcblock>
// </example>
//
//
// <motivation>
// To deal with the polarization leakage beam (mainly casacore::Stokes I into
// polarization visibilities).  The main primary beam response, ie,
// the diagonal Jones matrix elements, are handled by VPSkyJones.
// The motivation for this split is differing storage requirements
// for the unerlying PBMath types, and different methods available
// to VPSkyJones and DBeamSkyJones.
// </motivation>
//
// <todo asof="98/09/01">
// <li> Waiting for PBMath2D, which requires an HGEOM-like utility.
//      We can do nothing here until this is in place.
// <li> Need to check the history that PB has been applied before applying DB
//      (or at least provide for some sort of checking.)
// <li> Details of APPLY procedure may need to be modified
// </todo>

class DBeamSkyJones : public BeamSkyJones {

public:

  DBeamSkyJones(casacore::MeasurementSet& ms, 	     
		casacore::Bool makeDefaultPBsFromMS = true,
		const casacore::Quantity &parallacticAngleIncrement = casacore::Quantity(5.0, "deg"),
		BeamSquint::SquintType doSquint = BeamSquint::NONE);

  // Note that DBeamSkyJones uses BeamSkyJones' "apply" methods

  // return SkyJones type
  Type type() {return SkyJones::D;};

  // Is this solveable?
  virtual casacore::Bool isSolveable() {return false;};

protected:

private:    

};
 

} //# NAMESPACE CASA - END

#endif

