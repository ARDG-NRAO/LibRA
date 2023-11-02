//# PWFCleanImageSkyModel.cc: Implementation of WFCleanImageSkyModel class
//# Copyright (C) 1996,1997,1998,1999,2000,2002,2003
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
//# $Id$

#include <casacore/casa/Arrays/ArrayMath.h>
#include <synthesis/MeasurementComponents/PWFCleanImageSkyModel.h>
#include <casacore/images/Images/PagedImage.h>
#include <casacore/images/Images/ImageInterface.h>
#include <casacore/casa/OS/File.h>
#include <casacore/images/Images/SubImage.h>
#include <casacore/lattices/Lattices/LatticeStepper.h>
#include <casacore/lattices/Lattices/LatticeIterator.h>
#include <casacore/lattices/LEL/LatticeExpr.h>
#include <casacore/lattices/LRegions/LCBox.h>
#include <synthesis/MeasurementEquations/SkyEquation.h>
//#include <synthesis/MeasurementEquations/PSkyEquation.h>
#include <casacore/casa/Exceptions/Error.h>
#include <casacore/casa/BasicSL/String.h>
#include <casacore/casa/Utilities/Assert.h>
#include <casacore/coordinates/Coordinates/DirectionCoordinate.h>
#include <casacore/coordinates/Coordinates/CoordinateSystem.h>
#include <casacore/casa/Logging/LogMessage.h>
#include <casacore/casa/Logging/LogSink.h>
#include <casacore/casa/Logging/LogIO.h>
#include <sstream>
#include <synthesis/MeasurementComponents/MakeApproxPSFAlgorithm.h>
#include <synthesis/Parallel/Applicator.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

extern Applicator applicator;

  // This is kind of deprecated...good as an example but should be redone
PWFCleanImageSkyModel::PWFCleanImageSkyModel() : WFCleanImageSkyModel() {
 };

PWFCleanImageSkyModel::PWFCleanImageSkyModel(const Int facets): WFCleanImageSkyModel(facets)  {
 };

Bool PWFCleanImageSkyModel::solve(SkyEquation& se) {
  // Solve for the sky model

  LogIO os(LogOrigin("PWFCleanImageSkyModel","solve"));
  os << "Starting parallel wide-field clean with " << nfacets_p << " facets"
     << " and " << this->WFCleanImageSkyModel::numberOfModels()-nfacets_p 
     << " outliers" << LogIO::POST;
 
  // MFCleanImageSkyModel::solve() will call the parallel
  // PSkyEquation::solveSkyModel() method, so no explicit 
  // parallelization of MFCleanImageSkyModel is required 
  // for the wide-field case. This would be required for 
  // mosaicing however.
    if(!MFCleanImageSkyModel::solve(se)) {
      os << "Wide-field clean failed" << LogIO::POST;
      return false;
    }
    return(true);
}; 

void PWFCleanImageSkyModel::makeApproxPSFs(SkyEquation& se){
  // This function runs on the controller process. It distributes
  // the MakeApproxPSFAlgorithm task to worker processes, and
  // receives back the PSF for each facet and the associated
  // beam fit values.

  // Unlock the MS for the worker as the controller process in 
  // imager will have locked it
  se.unlock();

  LogIO os(LogOrigin("PWFCleanImageSkyModel", "makeApproxPSFs"));
  MakeApproxPSFAlgorithm makepsf ;
  Int rank;
  Bool allDone, assigned; 
  std::map<Int, Int> psfNo;
  Int gotten=0;

  try{

    for (Int thismodel=0; thismodel<nmodels_p; thismodel++) {
      assigned=applicator.nextAvailProcess(makepsf, rank);
      while(!assigned){
	rank= applicator.nextProcessDone(makepsf, allDone);
	// Receive PSF and fit to PSF info
       	//applicator.get(beam(psfNo(rank)));
	Array<Float> psfArray;
	applicator.get(psfArray);
	PSF(psfNo.at(rank)).putSlice(psfArray, IPosition(4, 0, 0, 0, 0));	 
	++gotten;
	/*if((*beam_p[psfNo(rank)])(0) == Float(-1.0)){
	  os << "Model " << thismodel << "PSF formation failed "
	     <<LogIO::POST ;
	     }*/
	// Assign next avail. child processor.
	assigned = applicator.nextAvailProcess(makepsf, rank);
     
      }
      // Send the worker the data it needs to build an FTMachine,
      // make a PSF and fit the beam

      Record container_ft;
      String errorString;
      se.fTMachine().toRecord(errorString, container_ft);
      applicator.put(container_ft);
      applicator.put(se.associatedMSName());
      // The above is for the creation of FTMachine  below should be for the 
      // cImage
      applicator.put(this->weight(thismodel));
      Record image_container;
      cImage(thismodel).toRecord(errorString, image_container);
      applicator.put(image_container);
      psfNo.insert( std::pair<Int,Int>(rank, thismodel) );
   
      //Serial transport execution
      applicator.apply(makepsf);
    }

    // Wait for the outstanding processes
    rank= applicator.nextProcessDone(makepsf,allDone);
 
    while (!allDone) {
      //applicator.get(beam(psfNo(rank)));
      Array<Float> psfArray;
      applicator.get(psfArray);
      PSF(psfNo.at(rank)).putSlice(psfArray, IPosition(4, 0, 0, 0, 0));   
      /*if((*beam_p[psfNo(rank)])(0)== Float(-1.0)){
	os << "Model "<< psfNo(rank) <<"Beam forming failed "<< LogIO::POST ;
	}*/
      rank=applicator.nextProcessDone(makepsf,allDone);
    }

  }catch (AipsError x){

    cout << x.getMesg() << endl;
  }  ;

  se.lock();
}


} //# NAMESPACE CASA - END

