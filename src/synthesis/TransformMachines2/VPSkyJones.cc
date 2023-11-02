//# VPSkyJones.cc: Implementation for VPSkyJones
//# Copyright (C) 1996,1997,1998,1999,2000,2001,2002,2003
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
#include <synthesis/TransformMachines/PBMath.h>
#include <synthesis/TransformMachines2/VPSkyJones.h>

#include <casacore/ms/MeasurementSets/MeasurementSet.h>
#include <casacore/tables/Tables.h>
#include <casacore/tables/Tables/TableRecord.h>
#include <casacore/ms/MeasurementSets/MSColumns.h>

#include <casacore/measures/Measures/Stokes.h>

#include <casacore/casa/BasicSL/Constants.h>

#include <components/ComponentModels/Flux.h>
#include <components/ComponentModels/ComponentShape.h>

#include <casacore/images/Images/ImageInterface.h>
#include <casacore/lattices/Lattices/LatticeStepper.h>
#include <casacore/lattices/Lattices/LatticeIterator.h>

#include <casacore/coordinates/Coordinates/CoordinateSystem.h>
#include <casacore/coordinates/Coordinates/DirectionCoordinate.h>
#include <casacore/coordinates/Coordinates/SpectralCoordinate.h>
#include <casacore/coordinates/Coordinates/StokesCoordinate.h>
#include <casacore/coordinates/Coordinates/Projection.h>

#include <casacore/casa/Utilities/Assert.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

namespace refim {// REFIM namespace

using namespace casacore;
 using namespace casa;
using namespace casacore;
 using namespace casa::refim;
VPSkyJones::VPSkyJones(const MSColumns& msc, Table& tab,
		       const Quantity &parAngleInc,
		       BeamSquint::SquintType doSquint,
		       const Quantity &skyPositionThreshold) 
  : BeamSkyJones(parAngleInc, doSquint,skyPositionThreshold)
{ 
  LogIO os(LogOrigin("VPSkyJones", "VPSkyJones"));

  const uInt nrow = tab.nrow();

  ScalarColumn<String> telCol(tab, "telescope");
  ScalarColumn<Int> antCol(tab, "antenna");
  ScalarColumn<TableRecord> recCol(tab, "pbdescription");

    
  for (uInt i=0; i < nrow; ++i) {
    // if-operator condition checks that the names are not redundant
    if (indexTelescope(telCol(i))<0) {
   
    
	 
  
      String name;
      String commonpb;
      if(recCol(i).isDefined("name") && (recCol(i).asString("name") != "REFERENCE")){

	const Int nameFieldNumber=recCol(i).fieldNumber("name");
	if (nameFieldNumber!=-1)
          recCol(i).get(nameFieldNumber,name);      
	
	if (name == "COMMONPB") {
	  const Int commonPBFieldNumber=recCol(i).fieldNumber("commonpb");
	  AlwaysAssert(commonPBFieldNumber!=-1,AipsError);
	  recCol(i).get(commonPBFieldNumber, commonpb );
	}      
	
	if (commonpb == "DEFAULT") {
	  String band;
	  PBMath::CommonPB whichPB;
	  String commonPBName;
	  ScalarColumn<String> telescopesCol(msc.observation().telescopeName());
	  Quantity freq( msc.spectralWindow().refFrequency()(0), "Hz");	
	  String tele =  telCol(i);
	  if(tele=="") {
	    os  << "Telescope name for row " << i << " of " << tab.tableName()
		<< " is blank : cannot find correct primary beam model" << LogIO::EXCEPTION;
	  }
	  else {
	    PBMath::whichCommonPBtoUse( tele, freq, band, whichPB, commonPBName );
	  }
	  
	  PBMath  myPBMath(tele, false, freq );
	  setPBMath (telCol(i), myPBMath);	
	  
	} else {        
	  PBMath  myPBMath( recCol(i));
	setPBMath (telCol(i), myPBMath);
	}
      }

    }

  }
};


VPSkyJones::VPSkyJones(const MSColumns& msc,
		       Bool makePBs,
		       const Quantity &parAngleInc,
		       BeamSquint::SquintType doSquint,
		       const Quantity &skyPositionThreshold) 
  : BeamSkyJones(parAngleInc, doSquint, skyPositionThreshold)
{ 
  LogIO os(LogOrigin("VPSkyJones", "VPSkyJones"));

  if (makePBs) {
    ScalarColumn<String> telescopesCol(msc.observation().telescopeName());
    

    for (uInt i=0; i < telescopesCol.nrow(); ++i) {
      String telescope_p = telescopesCol(i); 
      // if-operator condition checks that the names are not redundant
      if (indexTelescope(telescope_p)<0) {
	// Ultimately, we need to get the Beam parameters from the MS.beamSubTable,
	// but until then we will use this: figure it out from Telescope and Freq
	String band;
	PBMath::CommonPB whichPB;
	String commonPBName;
	// This frequency is ONLY required to determine which PB model to use:
	// The VLA, the ATNF, and WSRT have frequency - dependent PB models
	Quantity freq( msc.spectralWindow().refFrequency()(0), "Hz");
      
	if((telescope_p==" ") || (telescope_p=="")) {
	  whichPB=PBMath::UNKNOWN;
	}
	else {
	  PBMath::whichCommonPBtoUse( telescope_p, freq, band, whichPB, commonPBName );
	}
	
	if(whichPB != PBMath::UNKNOWN){
	  os << "PB used " << commonPBName << LogIO::POST;
	    PBMath  myPBMath(telescope_p, false, freq );
	    setPBMath (telescope_p, myPBMath);
	}
	else{
	  //lets do it by diameter
	  os << "PB used determined from dish-diameter" << LogIO::POST;
	  Double diam=msc.antenna().dishDiameter()(0);
	  PBMath myPBMath(diam, false, freq);
	  setPBMath(telescope_p, myPBMath);

	}



      }
    }
  }
};


VPSkyJones::VPSkyJones(const String& telescope,
		       PBMath::CommonPB commonPBType,
		       const Quantity &parAngleInc,
		       BeamSquint::SquintType doSquint,
		       const Quantity &skyPositionThreshold) 
  : BeamSkyJones(parAngleInc, doSquint,skyPositionThreshold)
{
  LogIO os(LogOrigin("VPSkyJones", "VPSkyJones"));
   

  // we need a way to do this for multiple telescope cases
  String telescope_p = telescope;
  PBMath  myPBMath(commonPBType);
  setPBMath (telescope_p, myPBMath);

};

 


VPSkyJones::VPSkyJones(const String& telescope,
		       PBMath& myPBMath,
		       const Quantity &parAngleInc,
		       BeamSquint::SquintType doSquint,
		       const Quantity &skyPositionThreshold) 
  : BeamSkyJones(parAngleInc, doSquint,skyPositionThreshold)
{ 
  LogIO os(LogOrigin("VPSkyJones", "VPSkyJones"));
   

  // we need a way to do this for multiple telescope cases
  String telescope_p = telescope;
  setPBMath (telescope_p, myPBMath);

};

 
} //END of REFIM
} //# NAMESPACE CASA - END

