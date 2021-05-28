//# SDAlgorithmHogbomClean.cc: Implementation of SDAlgorithmHogbomClean classes
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
#include <synthesis/ImagerObjects/SDAlgorithmHogbomClean.h>
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

#define NEED_UNDERSCORES
#if defined(NEED_UNDERSCORES)
#define hclean hclean_
#endif
extern "C" {
  void hclean(Float*, Float*, Float*, int*, Float*, int*, int*, int*,
              int*, int*, int*, int*, int*, int*, int*, Float*, Float*,
              Float*, void *, void *);
   };



  ////////////////// Global functions ? 
void REFHogbomCleanImageSkyModelstopnow (Int *yes) {

  *yes=0;
  return;

  /*
  Vector<String> choices(2);
  choices(0)="Continue";
  choices(1)="Stop Now";
  LogMessage message(LogOrigin("REFHogbomCleanImageSkyModel","solve"));
  LogSink logSink;
  *yes=0;
  return;
  String choice=Choice::choice("Clean iteration: do you want to continue or stop?", choices);
  if (choice==choices(0)) {
    *yes=0;
  }
  else {
    message.message("Stopping");
    logSink.post(message);
    *yes=1;
  }
  */

}

void REFHogbomCleanImageSkyModelmsgput(Int *npol, Int* /*pol*/, Int* iter, Int* px, Int* py,
				    Float* fMaxVal) {
  LogIO os(LogOrigin("REFHogbomCleanImageSkyModel","solve"));
   ostringstream o; 
//  LogSink logSink(LogMessage::NORMAL2);
  
  if(*npol<0) {
    StokesVector maxVal(fMaxVal[0], fMaxVal[1], fMaxVal[2], fMaxVal[3]);
    if(*iter==0) {
      //      o<<stokes<<": Before iteration, peak is "<<maxVal<<" at "<<*px-1<<","<<*py-1;
      o<<"Before iteration, peak is "<<maxVal<<" at "<<*px-1<<","<<*py-1;
      os << LogIO::NORMAL1 << o.str() << LogIO::POST;
      //      message.message(o);
      //      logSink.post(message);
    }
    else if(*iter>-1) {
      //      o<<stokes<<": Iteration "<<*iter<<" peak is "<<maxVal<<" at "<<*px-1<<","<<*py-1;
      o<<"Iteration "<<*iter<<" peak is "<<maxVal<<" at "<<*px-1<<","<<*py-1;
      os << LogIO::NORMAL1 << o.str() << LogIO::POST;
      //      message.message(o);
      //      logSink.post(message);
    }
    else {
      //      o<<stokes<<": Final iteration "<<abs(*iter)<<" peak is "<<maxVal<<" at "<<*px-1<<","<<*py-1;
      o<<"Final iteration "<<abs(*iter)<<" peak is "<<maxVal<<" at "<<*px-1<<","<<*py-1;
      os << LogIO::NORMAL1 << o.str() << LogIO::POST;
      //      message.message(o);
      //      logSink.post(message);
    }
  }
  else {
    Float maxVal(fMaxVal[0]);
    if(*iter==0) {
      //      o<<stokes<<": Before iteration, peak is "<<maxVal<<" at "<<*px-1<<","<<*py-1;
      o<<"Before iteration, peak is "<<maxVal<<" at "<<*px-1<<","<<*py-1;
      os << LogIO::NORMAL1 << o.str() << LogIO::POST;
      //      message.message(o);
      //      logSink.post(message);
    }
    else if(*iter>-1) {
      //      o<<stokes<<": Iteration "<<*iter<<" peak is "<<maxVal<<" at "<<*px-1<<","<<*py-1;
      o<<"Iteration "<<*iter<<" peak is "<<maxVal<<" at "<<*px-1<<","<<*py-1;
      os << LogIO::NORMAL1 << o.str() << LogIO::POST;
      //      message.message(o);
      //      logSink.post(message);
    }
    else {
      //      o<<stokes<<": Final iteration "<<abs(*iter)<<" peak is "<<maxVal<<" at "<<*px-1<<","<<*py-1;
      o<<"Final iteration "<<abs(*iter)<<" peak is "<<maxVal<<" at "<<*px-1<<","<<*py-1;
      os << LogIO::NORMAL1 << o.str() << LogIO::POST;
      //      message.message(o);
      //      logSink.post(message);
    }
  }
  
}



  SDAlgorithmHogbomClean::SDAlgorithmHogbomClean():
    SDAlgorithmBase()
    //    itsMatResidual(), itsMatModel(), itsMatPsf(),
    //    itsMaxPos( IPosition() ),
    //    itsPeakResidual(0.0),
    //    itsModelFlux(0.0),
    //    itsMatMask()
 {
   itsAlgorithmName=String("Hogbom");
 }

  SDAlgorithmHogbomClean::~SDAlgorithmHogbomClean()
 {
   
 }

  //  void SDAlgorithmHogbomClean::initializeDeconvolver( Float &peakresidual, Float &modelflux )
  void SDAlgorithmHogbomClean::initializeDeconvolver()
  {
    LogIO os( LogOrigin("SDAlgorithmHogbomClean","initializeDeconvolver",WHERE) );

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
  }


  void SDAlgorithmHogbomClean::takeOneStep( Float loopgain, 
					    Int cycleNiter, 
					    Float cycleThreshold, 
					    Float &peakresidual, 
					    Float &modelflux, 
					    Int &iterdone)
  {

    Bool delete_iti, delete_its, delete_itp, delete_itm;
    const Float *lpsf_data, *lmask_data;
    Float *limage_data, *limageStep_data;

    limage_data = itsMatModel.getStorage( delete_iti );
    limageStep_data = itsMatResidual.getStorage( delete_its );
    lpsf_data = itsMatPsf.getStorage( delete_itp );
    lmask_data = itsMatMask.getStorage( delete_itm );

    Int niter= cycleNiter;
    Float g = loopgain;
    Float thres = cycleThreshold;

    IPosition shp = itsMatPsf.shape();
    /*
    Int xbeg = shp[0]/4;
    Int xend = 3*shp[0]/4;
    Int ybeg = shp[1]/4;
    Int yend = 3*shp[1]/4;
    */
    
    Int xbeg = 0;
    Int xend = shp[0]-1;
    Int ybeg = 0;
    Int yend = shp[1]-1;
    

    Int newNx = shp[0];
    Int newNy = shp[1];
    Int npol = 1;
    

    Int fxbeg=xbeg+1;
    Int fxend=xend;
    Int fybeg=ybeg+1;
    Int fyend=yend;
    
    Int domaskI = 1;
    
    Int starting_iteration = 0;  
    Int ending_iteration=0;         
    Float cycleSpeedup = -1; // ie, ignore it
    
    hclean(limage_data, limageStep_data,
	   (Float*)lpsf_data, &domaskI, (Float*)lmask_data,
	   &newNx, &newNy, &npol,
	   &fxbeg, &fxend, &fybeg, &fyend, &niter,
	   &starting_iteration, &ending_iteration,
	   &g, &thres, &cycleSpeedup,
	   (void*) &REFHogbomCleanImageSkyModelmsgput,
	   (void*) &REFHogbomCleanImageSkyModelstopnow);
    
    iterdone=ending_iteration;
    
    itsMatModel.putStorage( limage_data, delete_iti );
    itsMatResidual.putStorage( limageStep_data, delete_its );
    itsMatPsf.freeStorage( lpsf_data, delete_itp );
    itsMatMask.freeStorage( lmask_data, delete_itm );
    
    
    /////////////////
    findMaxAbsMask( itsMatResidual, itsMatMask, itsPeakResidual, itsMaxPos );
    peakresidual = itsPeakResidual;

    modelflux = sum( itsMatModel ); // Performance hog ?
  }	    

  void SDAlgorithmHogbomClean::finalizeDeconvolver()
  {
    (itsImages->residual())->put( itsMatResidual );
    (itsImages->model())->put( itsMatModel );
  }


} //# NAMESPACE CASA - END

