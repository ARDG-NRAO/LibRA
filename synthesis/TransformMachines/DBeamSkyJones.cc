//# DBeamSkyJones.cc: Implementation for DBeamSkyJones
//# Copyright (C) 1996,1997,1998,2000,2001
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

#include <casacore/casa/aips.h>
#include <casacore/casa/BasicSL/Complex.h>
#include <synthesis/TransformMachines/VPSkyJones.h>

#include <casacore/ms/MeasurementSets/MeasurementSet.h>
#include <casacore/ms/MeasurementSets/MSColumns.h>

#include <casacore/measures/Measures/Stokes.h>

#include <casacore/casa/BasicSL/Constants.h>

#include <components/ComponentModels/Flux.h>
#include <components/ComponentModels/ComponentShape.h>

#include <msvis/MSVis/VisBuffer.h>
#include <casacore/images/Images/ImageInterface.h>
#include <casacore/lattices/Lattices/LatticeStepper.h>
#include <casacore/lattices/Lattices/LatticeIterator.h>

#include <casacore/coordinates/Coordinates/CoordinateSystem.h>
#include <casacore/coordinates/Coordinates/DirectionCoordinate.h>
#include <casacore/coordinates/Coordinates/SpectralCoordinate.h>
#include <casacore/coordinates/Coordinates/StokesCoordinate.h>
#include <casacore/coordinates/Coordinates/Projection.h>

#include <synthesis/TransformMachines/DBeamSkyJones.h>
#include <casacore/casa/Utilities/Assert.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

DBeamSkyJones::DBeamSkyJones(MeasurementSet& ms,
			     Bool makePBs,
			     const Quantity &parAngleInc,
			     BeamSquint::SquintType doSquint) 
  : BeamSkyJones(parAngleInc, doSquint)
{
  LogIO os(LogOrigin("DBeamSkyJones", "DBeamSkyJones"));

  if (makePBs) {
    MSColumns msc(ms);
    ScalarColumn<String> telescopesCol(msc.observation().telescopeName());
        
    for (uInt i=0; i < telescopesCol.nrow(); i++) {
      String telescope_p = telescopesCol(i); // access to protected member of
                          // the base class (not a good style, but left as is)
      // if operator condition checks that the names are not redundant
      if (indexTelescope(telescope_p)<0) {
	// Ultimately, we need to get the Beam parameters from the MS.beamSubTable,
	// but until then we will use this: figure it out from Telescope and Freq
	String band;
	PBMath::CommonPB whichPB;
	String commonPBName;
	// This frequency is ONLY required to determine which PB model to use:
	// The VLA, the ATNF, and WSRT have frequency - dependent PB models
	Quantity freq( msc.spectralWindow().refFrequency()(0), "Hz");
	

	// We need different code here for the DBeam: this is waiting for PBMath2D
	// to be fully functional, including an HGEOM-like routine to scale and
	// rotate images
	PBMath::whichCommonPBtoUse( telescope_p, freq, band, whichPB, commonPBName );
      
	os << LogIO::DEBUGGING << "Telescope name is " << telescope_p 
	   << LogIO::POST;
	os << LogIO::DEBUGGING << "Telescope band is " << band 
	   << LogIO::POST;
	os << LogIO::DEBUGGING << "Common PB type is " << commonPBName
	   << LogIO::POST;
     
	PBMath  myPBMath(whichPB);
	setPBMath (telescope_p, myPBMath);
      }
    }
  }
};

 

} //# NAMESPACE CASA - END

