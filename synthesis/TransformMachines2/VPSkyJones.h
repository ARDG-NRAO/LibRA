//# VPSkyJones.h: Definitions of interface for VPSkyJones 
//# Copyright (C) 1996-2016
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This library is free software; you can redistribute it and/or modify it
//# under the terms of the GNU  General Public License as published by
//# the Free Software Foundation; either version 2 of the License, or (at your
//# option) any later version.
//#
//# This library is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU  General Public
//# License for more details.
//#
//# You should have received a copy of the GNU  General Public License
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

#ifndef SYNTHESIS_TRANSFORM2_VPSKYJONES_H
#define SYNTHESIS_TRANSFORM2_VPSKYJONES_H

#include <casa/aips.h>
#include <measures/Measures/Stokes.h>
#include <synthesis/TransformMachines2/BeamSkyJones.h>


namespace casacore{

class Table;
}

namespace casa { //# NAMESPACE CASA - BEGIN


//# Someday, we'll need these forward declarations for Solve in the Jones Matrices
class SkyEquation;
class SkyModel;

namespace refim { //REFIM
// <summary> Model the diagonal elements of the Voltage Pattern Sky Jones Matrices </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
// <li> <linkto class="SkyEquation">SkyEquation</linkto> class
// <li> <linkto class="BeamSkyJones">BeamSkyJones</linkto> class
// </prerequisite>
//
// <etymology>
// VP = Voltage Pattern, SkyJones = Sky-based Jones matrices.
// This class only deals with the diagonal elements of the
// voltage pattern jones matrices.
// </etymology>
//
// <synopsis> 
//
// </synopsis> 
//
// <example>
// <srcblock>
// </srcblock>
// </example>
//
// <motivation>
// To deal with the non-leakage voltage pattern as applied to all casacore::Stokes,
// and beam squint (ie, errors in casacore::Stokes V caused by differing RR and
// LL beams).  Polarization leakage beams are in DBeamSkyJones.
// The motivation for this split is differing storage requirements
// for the unerlying PBMath types, and different methods available
// to VPSkyJones and DBeamSkyJones.
// </motivation>
//
// <todo asof="98/09/01">
// <li> Solvable part needs implementation: we need to derive an
// image of gradients of the elements of the Jones matrix. See VisJones
// for how to do this.
// </todo>

class VPSkyJones : public BeamSkyJones {

public:

  // constructor from a VP Table
  VPSkyJones(const casacore::MSColumns& msc, casacore::Table& table,
	     const casacore::Quantity &parAngleInc,
	     BeamSquint::SquintType doSquint,
	     const casacore::Quantity &skyPositionThreshold = casacore::Quantity(180.,"deg"));

  // constructor for default PB type associated with MS
  VPSkyJones(const casacore::MSColumns& msc, 
	     casacore::Bool makeDefaultPBsFromMS = true,
	     const casacore::Quantity &parallacticAngleIncrement = casacore::Quantity(720.0, "deg"),
	     BeamSquint::SquintType doSquint = BeamSquint::NONE,
	     const casacore::Quantity &skyPositionThreshold = casacore::Quantity(180.,"deg"));

  
  // constructor for common PB type
  VPSkyJones(const casacore::String& tel,
	     PBMath::CommonPB commonPBType,
	     const casacore::Quantity &parallacticAngleIncrement = casacore::Quantity(720.0, "deg"),
	     BeamSquint::SquintType doSquint = BeamSquint::NONE,
	     const casacore::Quantity &skyPositionThreshold = casacore::Quantity(180.,"deg"));

  // constructor for given PBMath type
  VPSkyJones(const casacore::String& tel,
	     PBMath& myPBMath,
	     const casacore::Quantity &parallacticAngleIncrement = casacore::Quantity(720.0, "deg"),
	     BeamSquint::SquintType doSquint = BeamSquint::NONE,
	     const casacore::Quantity &skyPositionThreshold = casacore::Quantity(180.,"deg"));

  
  // destructor needed so it's not an abstract baseclass
  ~VPSkyJones(){}

  // return SkyJones type
  ::casa::SkyJones::Type type() {return ::casa::SkyJones::E;};

  // Is this solveable?
  virtual casacore::Bool isSolveable() {return false;};
 


protected:

private:    

};
 
} // REFIM END
} //# NAMESPACE CASA - END

#endif

