//# SDAlgorithmMEM.cc: Implementation of SDAlgorithmMEM classes
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
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//# $Id$

#include <casacore/casa/Arrays/ArrayMath.h>
#include <casacore/casa/OS/HostInfo.h>

#include <components/ComponentModels/SkyComponent.h>
#include <components/ComponentModels/ComponentList.h>
#include <casacore/images/Images/TempImage.h>
#include <casacore/images/Images/SubImage.h>
#include <casacore/images/Regions/ImageRegion.h>
#include <casacore/casa/OS/File.h>
#include <casacore/lattices/LEL/LatticeExpr.h>
#include <casacore/lattices/Lattices/TiledLineStepper.h>
#include <casacore/lattices/Lattices/LatticeStepper.h>
#include <casacore/lattices/Lattices/LatticeIterator.h>
#include <synthesis/TransformMachines/StokesImageUtil.h>
#include <casacore/coordinates/Coordinates/StokesCoordinate.h>
#include <casacore/casa/Exceptions/Error.h>
#include <casacore/casa/BasicSL/String.h>
#include <casacore/casa/Utilities/Assert.h>
#include <casacore/casa/OS/Directory.h>
#include <casacore/tables/Tables/TableLock.h>

#include<synthesis/ImagerObjects/SIMinorCycleController.h>

#include <sstream>

#include <casacore/casa/Logging/LogMessage.h>
#include <casacore/casa/Logging/LogIO.h>
#include <casacore/casa/Logging/LogSink.h>

#include <casacore/casa/System/Choice.h>
#include <msvis/MSVis/StokesVector.h>

#include <synthesis/ImagerObjects/SDAlgorithmMEM.h>
#include <synthesis/MeasurementEquations/CEMemModel.h>
#include <synthesis/MeasurementEquations/LatConvEquation.h>
#include <synthesis/MeasurementEquations/IPLatConvEquation.h>


using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

  SDAlgorithmMEM::SDAlgorithmMEM(String entropy):
    SDAlgorithmBase()
 {
   LogIO os( LogOrigin("SDAlgorithmMEM","Constructor",WHERE) );
   itsAlgorithmName=String("Mem");
   itsMatDeltaModel.resize();

    if(entropy=="entropy") {
      //      os << "Deconvolving image using maximum entropy algorithm" << LogIO::POST;
      itsEnt = new EntropyI;
    }
    else if (entropy=="emptiness") {
      itsEnt = new EntropyEmptiness;
    }
    else {
      // Put check in Deconvolver parameter object.
      os << " Known MEM entropies: entropy | emptiness " << LogIO::POST;
      os << LogIO::SEVERE << "Unknown MEM entropy: " << entropy << LogIO::POST;
      //      return false;
    }
   
 }

  SDAlgorithmMEM::~SDAlgorithmMEM()
 {
   
 }

  //  void SDAlgorithmMEM::initializeDeconvolver( Float &peakresidual, Float &modelflux )
  void SDAlgorithmMEM::initializeDeconvolver()
  {
    LogIO os( LogOrigin("SDAlgorithmMEM","initializeDeconvolver",WHERE) );

    itsImages->residual()->get( itsMatResidual, true );
    itsImages->model()->get( itsMatModel, true );
    itsImages->psf()->get( itsMatPsf, true );
    itsImages->mask()->get( itsMatMask, true );


    //    cout << "initDecon : " << itsImages->residual()->shape() << " : " << itsMatResidual.shape() 
    //	 << itsImages->model()->shape() << " : " << itsMatModel.shape() 
    //	 << itsImages->psf()->shape() << " : " << itsMatPsf.shape() 
    //	 << endl;

    /*
    findMaxAbs( itsMatResidual, itsPeakResidual, itsMaxPos );
    itsModelFlux = sum( itsMatModel );

    peakresidual = itsPeakResidual;
    modelflux = itsModelFlux;
    */

    // Initialize the Delta Image model. Resize if needed.
    if ( itsMatDeltaModel.shape().nelements() != itsMatModel.shape().nelements() )
      { itsMatDeltaModel.resize ( itsMatModel.shape() ); }


  }

  // Code obtained from Deconvolver.cc
  void SDAlgorithmMEM::takeOneStep( Float /*loopgain*/, 
					    Int cycleNiter, 
					    Float cycleThreshold, 
					    Float &peakresidual, 
					    Float &modelflux, 
					    Int &iterdone)
  {
    LogIO os( LogOrigin("SDAlgorithmMEM","takeOneStep",WHERE) );
    // tmp
    itsImages->residual()->get( itsMatResidual, true );
    itsImages->mask()->get( itsMatMask, true );
    findMaxAbsMask( itsMatResidual, itsMatMask, itsPeakResidual, itsMaxPos );
    cout << "Peak Res at start of step : " << itsPeakResidual << endl;
    // tmp


    // Store current model in this matrix.
    itsImages->model()->get( itsMatDeltaModel, true );
    itsMatModel.assign( itsMatDeltaModel ); // This should make an explicit copy
    cout << "Flux at start of step : " << sum(itsMatModel) << endl;

    // Set model to zero
    
    LatticeLocker lockmod(*(itsImages->model()), FileLocker::Write);
    itsImages->model()->set( 0.0 );
    
    // Add to construction params
    Float targetFlux=1.0;
    Bool constrainTargetFlux=false;
    Bool initializeModel=true; // Use incremental model ?
    Bool imagePlane=true; // Use full image plane. Otherwise, use inner quarter.
    
    CEMemModel memer( *itsEnt, *(itsImages->model()), cycleNiter, cycleThreshold,
		      targetFlux, constrainTargetFlux, 
		      initializeModel, imagePlane);
    
    if (!initializeModel) {
      Record info=itsImages->model()->miscInfo();
      try {
	Float alpha = 0.0;
	Float beta = 0.0;
	info.get("ALPHA", alpha);
	memer.setAlpha(alpha);
	info.get("BETA", beta);
	memer.setBeta(beta); 
      } catch  (AipsError x) {
	// could not get Alpha and Beta for initialization
	// continue
	os << "Could not retrieve Alpha and Beta from previously initialized model" 
	   << LogIO::POST;
      } 
    } 

    /// Set the Prior    
    ///    if(prior != 0){
    ///      memer.setPrior(priorSub);
    ///    }

    memer.setMask(*(itsImages->mask()));

    CountedPtr<ResidualEquation<Lattice<Float> > > residEqn;

    if (imagePlane) {
      residEqn = new IPLatConvEquation (*(itsImages->psf()), *(itsImages->residual()));
    } else {
      residEqn = new LatConvEquation (*(itsImages->psf()), *(itsImages->residual()));
    }    
    
    //Bool result=
    memer.solve(*residEqn);

    Record info=itsImages->model()->miscInfo();
    info.define("ALPHA", memer.getAlpha());
    info.define("BETA",  memer.getBeta());
    itsImages->model()->setMiscInfo(info);

    iterdone = memer.numberIterations();

    LatticeExprNode maxres( max( memer.getResidual() ) );
    cout << "MAX RES at end : " << maxres.getFloat() << endl;

    // Retrieve residual before major cycle
    itsImages->residual()->copyData( memer.getResidual() );

    // Add delta model to old model
    //Bool ret2 = 
    itsImages->model()->get( itsMatDeltaModel, true );
    itsMatModel += itsMatDeltaModel;

    //---------------------------------

    //  Find Peak Residual
    itsImages->residual()->get( itsMatResidual, true );
    itsImages->mask()->get( itsMatMask, true );
    findMaxAbsMask( itsMatResidual, itsMatMask, itsPeakResidual, itsMaxPos );
    peakresidual = itsPeakResidual;

    // Find Total Model flux
    modelflux = sum( itsMatModel ); // Performance hog ?
    (itsImages->model())->put( itsMatModel );

    cout << "peakres : " << peakresidual << "    model : " << modelflux << endl;

  }	    

  void SDAlgorithmMEM::finalizeDeconvolver()
  {
    LatticeLocker lock1(*(itsImages->residual()), FileLocker::Write);
    LatticeLocker lock2(*(itsImages->model()), FileLocker::Write);
    (itsImages->residual())->put( itsMatResidual );
    (itsImages->model())->put( itsMatModel );
  }


} //# NAMESPACE CASA - END

