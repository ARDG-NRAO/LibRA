//# SDAlgorithmMSClean.cc: Implementation of SDAlgorithmMSClean classes
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

#include <casa/Arrays/ArrayMath.h>
#include <casa/OS/HostInfo.h>
#include <synthesis/ImagerObjects/SDAlgorithmMSClean.h>

#include <components/ComponentModels/SkyComponent.h>
#include <components/ComponentModels/ComponentList.h>
#include <images/Images/TempImage.h>
#include <images/Images/SubImage.h>
#include <images/Regions/ImageRegion.h>
#include <casa/OS/File.h>
#include <lattices/LEL/LatticeExpr.h>
#include <lattices/Lattices/TiledLineStepper.h>
#include <lattices/Lattices/LatticeStepper.h>
#include <lattices/Lattices/LatticeIterator.h>
#include <lattices/Lattices/LatticeLocker.h>
#include <synthesis/TransformMachines/StokesImageUtil.h>
#include <coordinates/Coordinates/StokesCoordinate.h>
#include <casa/Exceptions/Error.h>
#include <casa/BasicSL/String.h>
#include <casa/Utilities/Assert.h>
#include <casa/OS/Directory.h>
#include <tables/Tables/TableLock.h>

#include<synthesis/ImagerObjects/SIMinorCycleController.h>

#include <casa/sstream.h>

#include <casa/Logging/LogMessage.h>
#include <casa/Logging/LogIO.h>
#include <casa/Logging/LogSink.h>

#include <casa/System/Choice.h>
#include <msvis/MSVis/StokesVector.h>


using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN


  SDAlgorithmMSClean::SDAlgorithmMSClean( Vector<Float> scalesizes, 
					  Float smallscalebias, 
					  // Int stoplargenegatives, 
					  Int stoppointmode ):
    SDAlgorithmBase(),
    itsMatPsf(), itsMatResidual(), itsMatModel(),
    itsCleaner(),
    itsScaleSizes(scalesizes),
    itsSmallScaleBias(smallscalebias),
    //    itsStopLargeNegatives(stoplargenegatives),
    itsStopPointMode(stoppointmode)
 {
   itsAlgorithmName=String("multiscale");
   if( itsScaleSizes.nelements()==0 ){ itsScaleSizes.resize(1); itsScaleSizes[0]=0.0; }
 }

  SDAlgorithmMSClean::~SDAlgorithmMSClean()
 {
   
 }

  Long SDAlgorithmMSClean::estimateRAM(const vector<int>& imsize){
    Long mem=0;
    IPosition shp;
    if(itsImages)
      shp=itsImages->getShape();
    else if(imsize.size() >1)
      shp=IPosition(imsize);
    else
      return 0;
      //throw(AipsError("Deconvolver cannot estimate the memory usage at this point"));
    
    //Number of planes in memory
    //npsf=nscales+1
    //nresidual=4+nscales
    //nmodel=1
    //nmasks=2+nscales
    //transfer functions=nscales*2
      Long nplanes=5*itsScaleSizes.nelements()+7;
    //in kB
      mem=sizeof(Float)*(shp(0))*(shp(1))*nplanes/1024;
    
    return mem;
  }
 
  //  void SDAlgorithmMSClean::initializeDeconvolver( Float &peakresidual, Float &modelflux )
  void SDAlgorithmMSClean::initializeDeconvolver()
  {
    LogIO os( LogOrigin("SDAlgorithmMSClean","initializeDeconvolver",WHERE) );

    AlwaysAssert( (bool) itsImages, AipsError );
    {
      LatticeLocker lock1 (*(itsImages->residual()), FileLocker::Read);
      LatticeLocker lock2 (*(itsImages->model()), FileLocker::Read);
      LatticeLocker lock3 (*(itsImages->psf()), FileLocker::Read);
      LatticeLocker lock4 (*(itsImages->mask()), FileLocker::Read);
      (itsImages->residual())->get( itsMatResidual , true );
      (itsImages->model())->get( itsMatModel , true );
      (itsImages->psf())->get( itsMatPsf , true );
      itsImages->mask()->get( itsMatMask, true );
      
    }
    //// Initialize the MatrixCleaner.
    ///  ----------- do once ----------
      {
	itsCleaner.defineScales( itsScaleSizes );
	
	if(itsSmallScaleBias > 1)
	{
	  os << LogIO::WARN << "Acceptable smallscalebias values are [-1,1].Changing smallscalebias from " << itsSmallScaleBias <<" to 1." << LogIO::POST; 
	  itsSmallScaleBias = 1;
	}
	
	if(itsSmallScaleBias < -1)
	{
	  os << LogIO::WARN << "Acceptable smallscalebias values are [-1,1].Changing smallscalebias from " << itsSmallScaleBias <<" to -1." << LogIO::POST; 
	  itsSmallScaleBias = -1;
	}
	
	
	itsCleaner.setSmallScaleBias( itsSmallScaleBias );
	//itsCleaner.stopAtLargeScaleNegative( itsStopLargeNegatives );// In MFMSCleanImageSkyModel.cc, this is only for the first two major cycles...
	itsCleaner.stopPointMode( itsStopPointMode );
	itsCleaner.ignoreCenterBox( true ); // Clean full image

	Matrix<Float> tempMat;
	tempMat.reference( itsMatPsf );
	itsCleaner.setPsf(  tempMat );
	itsCleaner.makePsfScales();

      }
    /// -----------------------------------------

    /*
    /// Find initial max vals..
    findMaxAbsMask( itsMatResidual, itsMatMask, itsPeakResidual, itsMaxPos );
    itsModelFlux = sum( itsMatModel );

    peakresidual = itsPeakResidual;
    modelflux = itsModelFlux;
    */

    // Parts to be repeated at each minor cycle start....
    
    itsCleaner.setcontrol(CleanEnums::MULTISCALE,0,0,0);/// Needs to come before makeDirtyScales

    Matrix<Float> tempmask(itsMatMask);
    itsCleaner.setMask( tempmask );

    Matrix<Float> tempMat1;
    tempMat1.reference( itsMatResidual );
    itsCleaner.setDirty( tempMat1 );
    itsCleaner.makeDirtyScales();
    
  }

  void SDAlgorithmMSClean::takeOneStep( Float loopgain, Int cycleNiter, Float cycleThreshold, Float &peakresidual, Float &modelflux, Int &iterdone)
  {
    LogIO os( LogOrigin("SDAlgorithmMSClean","takeOneStep",WHERE) );

    Quantity thresh( cycleThreshold, "Jy" );
    //    Quantity ftthresh( 100.0, "Jy" ); /// Look at MFMSCleanImageSkyModel.cc for more.
    itsCleaner.setcontrol(CleanEnums::MULTISCALE, cycleNiter, loopgain, thresh); //, ftthresh);

    Matrix<Float> tempModel;
    tempModel.reference( itsMatModel );
    //save the previous model
    Matrix<Float> prevModel;
    prevModel=itsMatModel;

    //cout << "SDALMS,  matrix shape : " << tempModel.shape() << " array shape : " << itsMatModel.shape() << endl;

    // retval
    //  1 = converged
    //  0 = not converged but behaving normally
    // -1 = not converged and stopped on cleaning consecutive smallest scale
    // -2 = not converged and either large scale hit negative or diverging 
    // -3 = clean is diverging rather than converging
    itsCleaner.startingIteration( 0 );
    Int retval = itsCleaner.clean( tempModel );
    iterdone = itsCleaner.numberIterations();

    if( retval==-1 ) {os << LogIO::WARN << "MSClean minor cycle stopped on cleaning consecutive smallest scale" << LogIO::POST; }
    if( retval==-2 ) {os << LogIO::WARN << "MSClean minor cycle stopped at large scale negative or diverging" << LogIO::POST;}
    if( retval==-3 ) {os << LogIO::WARN << "MSClean minor cycle stopped because it is diverging" << LogIO::POST; }

    // Retrieve residual to be saved to the .residual file in finalizeDeconvolver.
    ////This is going to be wrong if there is no 0 scale;
    ///Matrix<Float> residual(itsCleaner.residual());
    //Matrix<Float> residual(itsCleaner.residual(tempModel-prevModel));
    //    cout << "Max tempModel : " << max(abs(tempModel)) << "  Max prevModel  : " << max(abs(prevModel)) << endl;
    itsMatResidual = itsCleaner.residual(tempModel-prevModel);

    // account for mask as well
    //peakresidual = max(abs(residual*itsMatMask));
    peakresidual = max(abs(itsMatResidual*itsMatMask));
    modelflux = sum( itsMatModel ); // Performance hog ?
  }	    

  void SDAlgorithmMSClean::finalizeDeconvolver()
  {
    ///MatrixCleaner does not modify the original residual image matrix
    ///so the first line is a dummy.
    LatticeLocker lock1 (*(itsImages->residual()), FileLocker::Write);
    LatticeLocker lock2 (*(itsImages->model()), FileLocker::Write);
    (itsImages->residual())->put( itsMatResidual );
    (itsImages->model())->put( itsMatModel );
  }


} //# NAMESPACE CASA - END

