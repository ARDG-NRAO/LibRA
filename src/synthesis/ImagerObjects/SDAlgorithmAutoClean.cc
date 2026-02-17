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

#include <casacore/casa/Arrays/ArrayMath.h>
#include <casacore/casa/OS/HostInfo.h>
#include <synthesis/ImagerObjects/SDAlgorithmAutoClean.h>

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
#include <casacore/lattices/Lattices/LatticeLocker.h>
#include <synthesis/TransformMachines/StokesImageUtil.h>
#include <casacore/coordinates/Coordinates/StokesCoordinate.h>
#include <casacore/casa/Exceptions/Error.h>
#include <casacore/casa/BasicSL/String.h>
#include <casacore/casa/Utilities/Assert.h>
#include <casacore/casa/OS/Directory.h>
#include <casacore/tables/Tables/TableLock.h>

//new
#include <casacore/casa/Arrays/ArrayUtil.h>

#include<synthesis/ImagerObjects/SIMinorCycleController.h>

#include <sstream>

#include <casacore/casa/Logging/LogMessage.h>
#include <casacore/casa/Logging/LogIO.h>
#include <casacore/casa/Logging/LogSink.h>

#include <casacore/casa/System/Choice.h>
#include <msvis/MSVis/StokesVector.h>


using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN


  SDAlgorithmAutoClean::SDAlgorithmAutoClean(Float autoThreshold, Int autoMaxiter, Float autoGain, Float hogbomGain, Bool autoHogbom, Float autoTrigger, Float autoPower, Int autoXMask, Int autoYMask):
    SDAlgorithmBase(),
    itsMatPsf(), itsMatResidual(), itsMatModel(), itsMatTemp(),
    itsCleaner(),
    itsautoThreshold(autoThreshold), itsautoMaxiter(autoMaxiter), itsautoGain(autoGain), itshogbomGain(hogbomGain), itsautoHogbom(autoHogbom), itsautoTrigger(autoTrigger), itsautoPower(autoPower), itsautoXMask(autoXMask), itsautoYMask(autoYMask)
    {	
	itsAlgorithmName=String("autocorrelation");
    }


  SDAlgorithmAutoClean::~SDAlgorithmAutoClean()
  {

  }

  Long SDAlgorithmAutoClean::estimateRAM(const vector<int>& imsize){
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
      Long nplanes=5*1+7;
    //in kB
      mem=sizeof(Float)*(shp(0))*(shp(1))*nplanes/1024;

    return mem;
  }

  //  void SDAlgorithmMSClean::initializeDeconvolver( Float &peakresidual, Float &modelflux )
  void SDAlgorithmAutoClean::initializeDeconvolver()
  {
    LogIO os( LogOrigin("SDAlgorithmAutoClean","initializeDeconvolver",WHERE) );
    AlwaysAssert( (bool) itsImages, AipsError );
    {
      LatticeLocker lock1 (*(itsImages->residual()), FileLocker::Read);
      LatticeLocker lock2 (*(itsImages->model()), FileLocker::Read);
      LatticeLocker lock3 (*(itsImages->psf()), FileLocker::Read);
      LatticeLocker lock4 (*(itsImages->mask()), FileLocker::Read);
      //LatticeLocker lock5 (*(itsImages->tempworkimage()), FileLocker::Read);
      (itsImages->residual())->get( itsMatResidual , true );
      (itsImages->model())->get( itsMatModel , true );
      (itsImages->psf())->get( itsMatPsf , true );
      itsImages->mask()->get( itsMatMask, true );

    }
    //// Initialize the AutoCleaner.
    ///  ----------- do once ----------
    {
    	itsCleaner.ignoreCenterBox( true ); // Clean full image

    	Matrix<Float> tempMat;
    	tempMat.reference( itsMatPsf );
    	itsCleaner.setPsf(  tempMat );
        itsCleaner.setscales();

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

    //itsCleaner.initializeCorrProducts();
    //itsCleaner.approximateBasisFunction();

    if (itsautoThreshold < 0)
    {
       os << LogIO::WARN << "Acceptable autothreshld values are >= 0. Changing autothreshold from " << itsautoThreshold << " to 0." << LogIO::POST;
       itsautoThreshold = 0.0;
    }
    if (itsautoMaxiter < 0)
    {
       os << LogIO::WARN << "Acceptable automaxiter values are >= 0. Changing automaxiter from " << itsautoMaxiter << " to 0." << LogIO::POST;
       itsautoMaxiter = 0.0;
    }
    if (itsautoGain < 0)
    {
       os << LogIO::WARN << "Acceptable autogain values are >= 0. Changing autogain from " << itsautoGain << " to 0." << LogIO::POST;
       itsautoGain = 0.0;
    }
    if (itshogbomGain < 0)
    {
       os << LogIO::WARN << "Acceptable hogbomgain values are >= 0. Changing hogbomgain from " << itshogbomGain << " to 0." << LogIO::POST;
       itshogbomGain = 0.0;
    }
    if (itsautoPower < 0)
    {
       os << LogIO::WARN << "Acceptable autopower values are >= 0. Changing autopower from " << itsautoPower << " to 0." << LogIO::POST;
       itsautoPower = 1.0;
    }

    itsCleaner.setAutoControl(itsautoThreshold, itsautoMaxiter, itsautoGain, itshogbomGain, itsautoHogbom, itsautoTrigger, itsautoPower, itsautoXMask, itsautoYMask);

    if(itsautoHogbom==false){
	    if(itsImages->doesImageExist(itsImages->getName()+String(".tildeII")) &&
	       itsImages->doesImageExist(itsImages->getName()+String(".cmap")) &&
	       itsImages->doesImageExist(itsImages->getName()+String(".MtildeMB")) &&
	       itsImages->doesImageExist(itsImages->getName()+String(".mod")) &&
	       itsImages->doesImageExist(itsImages->getName()+String(".tildeMI")) &&
	       itsImages->doesImageExist(itsImages->getName()+String(".BI")) &&
	       itsImages->doesImageExist(itsImages->getName()+String(".tildeMB")) &&
	       itsImages->doesImageExist(itsImages->getName()+String(".tildeMBB")) &&
	       itsImages->doesImageExist(itsImages->getName()+String(".tildeMBI")) &&
	       itsImages->doesImageExist(itsImages->getName()+String(".MtildeMBB")) &&
	       itsImages->doesImageExist(itsImages->getName()+String(".BB")) &&
	       itsImages->doesImageExist(itsImages->getName()+String(".window_basis"))
	    ){
	       os << "Autocorrelation product exist, load autocorrelation products" << LogIO::POST;
	       itsImages->loadmatrix(itsCleaner.tildeII, itsImages->getName()+String(".tildeII"));
	       itsImages->loadmatrix(itsCleaner.cmap, itsImages->getName()+String(".cmap"));
	       itsImages->loadmatrix(itsCleaner.MtildeMB, itsImages->getName()+String(".MtildeMB"));
	       itsImages->loadmatrix(itsCleaner.mod, itsImages->getName()+String(".mod"));
	       itsImages->loadmatrix(itsCleaner.tildeMI, itsImages->getName()+String(".tildeMI"));
	       itsImages->loadmatrix(itsCleaner.BI, itsImages->getName()+String(".BI"));
	       itsImages->loadmatrix(itsCleaner.tildeMB, itsImages->getName()+String(".tildeMB"));
	       itsImages->loadmatrix(itsCleaner.tildeMBB, itsImages->getName()+String(".tildeMBB"));
	       itsImages->loadmatrix(itsCleaner.tildeMBI, itsImages->getName()+String(".tildeMBI"));
	       itsImages->loadmatrix(itsCleaner.MtildeMBB, itsImages->getName()+String(".MtildeMBB"));
	       itsImages->loadmatrix(itsCleaner.BB, itsImages->getName()+String(".BB"));
	       itsImages->loadmatrix(itsCleaner.window_basis, itsImages->getName()+String(".window_basis"));

	       itsCleaner.reinitializeCorrProducts();
	    }
	   else{
	      itsCleaner.initializeCorrProducts();
	      itsCleaner.approximateBasisFunction();
	   }
	}
  }

  void SDAlgorithmAutoClean::takeOneStep( Float loopgain, Int cycleNiter, Float cycleThreshold, Float &peakresidual, Float &modelflux, Int &iterdone)
  {
    LogIO os( LogOrigin("SDAlgorithmAutoClean","takeOneStep",WHERE) );

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

    // Retrieve residual to be saved to the .residual file in finalizeDeconvolver.
    ////This is going to be wrong if there is no 0 scale;
    ///Matrix<Float> residual(itsCleaner.residual());
    //Matrix<Float> residual(itsCleaner.residual(tempModel-prevModel));
    //    cout << "Max tempModel : " << max(abs(tempModel)) << "  Max prevModel  : " << max(abs(prevModel)) << endl;
    //itsMatResidual = itsCleaner.residual(tempModel-prevModel);
    itsMatResidual = itsCleaner.residual();
    //itsMatTemp = itsCleaner.cmap;

    // account for mask as well
    //peakresidual = max(abs(residual*itsMatMask));
    peakresidual = max(abs(itsMatResidual*itsMatMask));
    modelflux = sum( itsMatModel ); // Performance hog ?
  }

  void SDAlgorithmAutoClean::finalizeDeconvolver()
  {
    ///MatrixCleaner does not modify the original residual image matrix
    ///so the first line is a dummy.
    LatticeLocker lock1 (*(itsImages->residual()), FileLocker::Write);
    LatticeLocker lock2 (*(itsImages->model()), FileLocker::Write);
    //LatticeLocker lock5 (*(itsImages->tempworkimage()), FileLocker::Write);
    (itsImages->residual())->put( itsMatResidual );
    (itsImages->model())->put( itsMatModel );
    //(itsImages->tempworkimage())->put( itsMatTemp );
    itsImages->savematrix(itsCleaner.tildeII, itsImages->getName()+String(".tildeII"));
    itsImages->savematrix(itsCleaner.cmap, itsImages->getName()+String(".cmap"));
    itsImages->savematrix(itsCleaner.MtildeMB, itsImages->getName()+String(".MtildeMB"));
    itsImages->savematrix(itsCleaner.mod, itsImages->getName()+String(".mod"));
    itsImages->savematrix(itsCleaner.tildeMI, itsImages->getName()+String(".tildeMI"));
    itsImages->savematrix(itsCleaner.BI, itsImages->getName()+String(".BI"));
    itsImages->savematrix(itsCleaner.tildeMB, itsImages->getName()+String(".tildeMB"));
    itsImages->savematrix(itsCleaner.tildeMBB, itsImages->getName()+String(".tildeMBB"));
    itsImages->savematrix(itsCleaner.tildeMBI, itsImages->getName()+String(".tildeMBI"));
    itsImages->savematrix(itsCleaner.MtildeMBB, itsImages->getName()+String(".MtildeMBB"));
    itsImages->savematrix(itsCleaner.BB, itsImages->getName()+String(".BB"));    
    itsImages->savematrix(itsCleaner.window_basis, itsImages->getName()+String(".window_basis"));
  }

} //# NAMESPACE CASA - END

