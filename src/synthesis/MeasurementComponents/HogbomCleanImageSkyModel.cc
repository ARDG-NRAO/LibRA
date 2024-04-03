//# HogbomCleanImageSkyModel.cc: Implementation of HogbomCleanImageSkyModel class
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

#include <casacore/casa/Arrays/Cube.h>
#include <casacore/casa/Arrays/Matrix.h>
#include <casacore/casa/Arrays/ArrayMath.h>
#include <casacore/casa/Arrays/ArrayMath.h>
#include <casacore/casa/Arrays/ArrayPosIter.h>
#include <msvis/MSVis/StokesVector.h>
#include <synthesis/MeasurementComponents/HogbomCleanImageSkyModel.h>
#include <casacore/images/Images/PagedImage.h>
#include <casacore/casa/OS/File.h>
#include <casacore/lattices/Lattices/LatticeStepper.h>
#include <casacore/lattices/Lattices/LatticeIterator.h>
#include <casacore/lattices/LEL/LatticeExpr.h>
#include <casacore/lattices/LEL/LatticeExprNode.h>
#include <synthesis/MeasurementEquations/SkyEquation.h>
#include <casacore/casa/Exceptions/Error.h>
#include <casacore/casa/BasicSL/String.h>
#include <casacore/casa/Utilities/Assert.h>

#include <sstream>

#include <casacore/casa/Logging/LogMessage.h>
#include <casacore/casa/Logging/LogSink.h>

#include <casacore/casa/System/Choice.h>


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

void HogbomCleanImageSkyModelstopnow (Int *yes) {
  Vector<String> choices(2);
  choices(0)="Continue";
  choices(1)="Stop Now";
  LogMessage message(LogOrigin("HogbomCleanImageSkyModel","solve"));
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
}

void HogbomCleanImageSkyModelmsgput(Int *npol, Int* /*pol*/, Int* iter, Int* px, Int* py,
				    Float* fMaxVal) {
  LogMessage message(LogOrigin("HogbomCleanImageSkyModel","solve"));
  ostringstream o; 
  LogSink logSink;
  /*
  String stokes("Unknown");
  ///UUU
  // This code allows only  I, IV, IQU, IQUV !!!!!
  if(*npol==1) {
    stokes="I";
  }
  else if(*npol==2) {
    if(*pol==1) {
      stokes="I";
    }
    else if(*pol==2) {
      stokes="V";
    }
  }
  else if(*npol==3) {
    if(*pol==1) {
      stokes="I";
    }
    else if(*pol==2) {
      stokes="Q";
    }
    else if(*pol==3) {
      stokes="U";
    }
  }
  else if(*npol==4) {
    if(*pol==1) {
      stokes="I";
    }
    else if(*pol==2) {
      stokes="Q";
    }
    else if(*pol==3) {
      stokes="U";
    }
    else if(*pol==4) {
      stokes="V";
    }
  }
  if(*pol==-1) {
    stokes="I";
  }
  else if(*pol==-2) {
    stokes="I,V";
  }
  else if(*pol==-3) {
    stokes="I,Q,U";
  }
  else if(*pol==-4) {
    stokes="I,Q,U,V";
  }
  */  
  if(*npol<0) {
    StokesVector maxVal(fMaxVal[0], fMaxVal[1], fMaxVal[2], fMaxVal[3]);
    if(*iter==0) {
      //      o<<stokes<<": Before iteration, peak is "<<maxVal<<" at "<<*px-1<<","<<*py-1;
      o<<"Before iteration, peak is "<<maxVal<<" at "<<*px-1<<","<<*py-1;
      message.message(o);
      logSink.post(message);
    }
    else if(*iter>-1) {
      //      o<<stokes<<": Iteration "<<*iter<<" peak is "<<maxVal<<" at "<<*px-1<<","<<*py-1;
      o<<"Iteration "<<*iter<<" peak is "<<maxVal<<" at "<<*px-1<<","<<*py-1;
      message.message(o);
      logSink.post(message);
    }
    else {
      //      o<<stokes<<": Final iteration "<<abs(*iter)<<" peak is "<<maxVal<<" at "<<*px-1<<","<<*py-1;
      o<<"Final iteration "<<abs(*iter)<<" peak is "<<maxVal<<" at "<<*px-1<<","<<*py-1;
      message.message(o);
      logSink.post(message);
    }
  }
  else {
    Float maxVal(fMaxVal[0]);
    if(*iter==0) {
      //      o<<stokes<<": Before iteration, peak is "<<maxVal<<" at "<<*px-1<<","<<*py-1;
      o<<"Before iteration, peak is "<<maxVal<<" at "<<*px-1<<","<<*py-1;
      message.message(o);
      logSink.post(message);
    }
    else if(*iter>-1) {
      //      o<<stokes<<": Iteration "<<*iter<<" peak is "<<maxVal<<" at "<<*px-1<<","<<*py-1;
      o<<"Iteration "<<*iter<<" peak is "<<maxVal<<" at "<<*px-1<<","<<*py-1;
      message.message(o);
      logSink.post(message);
    }
    else {
      //      o<<stokes<<": Final iteration "<<abs(*iter)<<" peak is "<<maxVal<<" at "<<*px-1<<","<<*py-1;
      o<<"Final iteration "<<abs(*iter)<<" peak is "<<maxVal<<" at "<<*px-1<<","<<*py-1;
      message.message(o);
      logSink.post(message);
    }
  }
  
}

// Clean solver
Bool HogbomCleanImageSkyModel::solve(SkyEquation& se) {
  
  LogIO os(LogOrigin("HogbomCleanImageSkyModel","solve"));
  
  Bool converged=false;
  if(numberOfModels()>1) {
    os << "Cannot process more than one field" << LogIO::EXCEPTION;
  }
  
  // Make the residual image
  if(modified_p)
    makeNewtonRaphsonStep(se, false, (numberIterations()<1)?true:False);
  
  //Make the PSF
  if(!donePSF_p)
    makeApproxPSFs(se);

  if(numberIterations() < 1){
    return true;
  }
  
  if(!isSolveable(0)) {
    os << "Model 1 is not solveable!" << LogIO::EXCEPTION;
  }
  
  

  Int nx=image(0).shape()(0);
  Int ny=image(0).shape()(1);
  Int npol=image(0).shape()(2);
  Int nchan=image(0).shape()(3);
  Bool isCubeMask=false; 
  AlwaysAssert((npol==1)||(npol==2)||(npol==3)||(npol==4), AipsError);
  
  // Loop over all channels
  LatticeStepper psfls(PSF(0).shape(), IPosition(4,nx,ny,1,1),
		       IPosition(4,0,1,2,3));
  RO_LatticeIterator<Float> psfli(PSF(0),psfls);
  
  // Read the entire image for each spectral channel
  LatticeStepper ls(image(0).shape(),
		    IPosition(4, nx, ny, npol, 1),
		    IPosition(4, 0, 1, 2, 3));
  LatticeIterator<Float> imageStepli(residual(0), ls);
  LatticeIterator<Float> imageli(image(0), ls);
  
  // Now set up the mask.
  RO_LatticeIterator<Float>* maskli = 0;
  Matrix<Float>* lmask= new Matrix<Float>(1,1); 
  lmask->set(1.0);
  Int xbeg, xend;
  Int ybeg, yend;
  Int newNx=nx;
  Int newNy=ny;
  //default clean support
  xbeg=nx/4; 
  xend=3*nx/4-1;
  ybeg=ny/4; 
  yend=3*ny/4-1;
  Bool domask=true;
  if(hasMask(0)) {
    domask=true;
    AlwaysAssert(mask(0).shape()(0)==nx, AipsError);
    AlwaysAssert(mask(0).shape()(1)==ny, AipsError);
    if(nchan >1){
      if(mask(0).shape()(3)==nchan){
	isCubeMask=true;
	os << "Using multichannel mask" << LogIO::POST;
      }
      else{
	os << "Image cube and mask donot match in number of channels" 
	   << LogIO::WARN;
	os << "Will use first plane of the mask for all channels" 
	   << LogIO::WARN;
      }
    }
    LatticeStepper mls(mask(0).shape(),
		       IPosition(4, nx, ny, 1, 1),
		       IPosition(4, 0, 1, 2, 3));
    maskli= new RO_LatticeIterator<Float>(mask(0), mls);
    maskli->reset();
    lmask=makeMaskMatrix(nx, ny, newNx, newNy, *maskli, xbeg, 
			 xend, ybeg, yend); 
  }
  else {
    domask=false;
  }
  
  //Some variables if we change the image size
  IPosition newshp(4, newNx, newNy, npol,1);
  Array<Float> newData;
  Array<Float> newStep;
  Array<Float> newPsf;
  //------------------

  Float maxRes=0.0;
  Int iterUsed=0;
  Int chan=0;
  for (imageStepli.reset(),imageli.reset(),psfli.reset();
       !imageStepli.atEnd();
       imageStepli++,imageli++,psfli++,chan++) {
    //Deal with cube mask
    if(hasMask(0) && isCubeMask && chan >0) {
      (*maskli)++;
      if(lmask) delete lmask;
      lmask=0;
      lmask=makeMaskMatrix(nx, ny, newNx, newNy, *maskli, xbeg, 
			   xend, ybeg, yend);       
    }
    
    // Make IPositions and find position of peak of PSF
    IPosition psfposmax(psfli.cursor().ndim());
    IPosition psfposmin(psfli.cursor().ndim());
    Float psfmax;
    Float psfmin;
    minMax(psfmin, psfmax, psfposmin, psfposmax, psfli.cursor()); 
    
    if(nchan>1) {
      os<<"Processing channel "<<chan+1<<" of "<<nchan<<LogIO::POST;
    }
    if((psfmax==0.0) || (hasMask(0) && (lmask==0))) {
      os<<"No data or blank mask for this channel: skipping"<<LogIO::POST;
    }
    else {
      
      Bool delete_iti, delete_its, delete_itp, delete_itm;
      const Float *lpsf_data, *lmask_data;
      Float *limage_data, *limageStep_data;
      lmask_data=lmask->getStorage(delete_itm);
      if(newNx==nx){
	limage_data=imageli.rwCursor().getStorage(delete_iti);
	limageStep_data=imageStepli.rwCursor().getStorage(delete_its);
	lpsf_data=psfli.cursor().getStorage(delete_itp);
      }
      else{
	IPosition blc(4, (newNx-nx)/2, (newNy-ny)/2, 0,0);
	IPosition trc(4, (newNx+nx)/2-1, (newNy+ny)/2-1, npol-1,0);
	newshp(0)=newNx; 
	newshp(1)=newNy;
	newData.resize(newshp);
	newStep.resize(newshp);
	
	newData.set(0.0); newStep.set(0.0);
	newData(blc,trc).assign(imageli.rwCursor());
	newStep(blc,trc).assign(imageStepli.rwCursor());
	//psf we use first plane only of pol image
	newshp(2)=1;
	newPsf.resize(newshp);
	newPsf.set(0.0);
	trc(2)=0;
	newPsf(blc,trc).assign(psfli.cursor());
	limage_data=newData.getStorage(delete_iti);
	limageStep_data=newStep.getStorage(delete_its);
	lpsf_data=newPsf.getStorage(delete_itp);
      }
      Int niter=numberIterations();
      Float g=gain();
      Float thres=threshold();
      // Fortran indexes at 1
      Int fxbeg=xbeg+1;
      Int fxend=xend+1;
      Int fybeg=ybeg+1;
      Int fyend=yend+1;
      Int domaskI;
      if (domask) {
	domaskI = 1;
      } else {
	domaskI = 0;
      }
      
      Int starting_iteration = 0;  
      Int ending_iteration=0;         
      //# The const of lpsf and mask has to be cast away for the
      //# fortran function hclean.
      Float cycleSpeedup = -1; // ie, ignore it
      hclean(limage_data, limageStep_data,
	     (Float*)lpsf_data, &domaskI, (Float*)lmask_data,
	     &newNx, &newNy, &npol,
	     &fxbeg, &fxend, &fybeg, &fyend, &niter,
	     &starting_iteration, &ending_iteration,
	     &g, &thres, &cycleSpeedup,
	     (void*) &HogbomCleanImageSkyModelmsgput,
	     (void*) &HogbomCleanImageSkyModelstopnow);

      iterUsed=max(iterUsed, ending_iteration);
      if(nx==newNx){
	imageli.rwCursor().putStorage (limage_data, delete_iti);
	imageStepli.rwCursor().putStorage (limageStep_data, delete_its);
	psfli.cursor().freeStorage (lpsf_data, delete_itp);
      }
      else{
	IPosition blc(4, (newNx-nx)/2, (newNy-ny)/2, 0,0);
	IPosition trc(4, (newNx+nx)/2-1, (newNy+ny)/2-1, npol-1,0);
	newData.putStorage(limage_data, delete_iti);
	newStep.putStorage(limageStep_data, delete_its);
	newPsf.freeStorage(lpsf_data, delete_itp);
	imageli.rwCursor().assign(newData(blc,trc));
	imageStepli.rwCursor().assign(newStep(blc,trc));
      }
      lmask->freeStorage (lmask_data, delete_itm);
      Float residualmax, residualmin;
      minMax(residualmin, residualmax, imageStepli.cursor());
      residualmax=max(residualmax, abs(residualmin));
      maxRes=max(maxRes, residualmax);
      
      converged = (residualmax < threshold());
    }
    if (lmask != 0 && isCubeMask)  {
      lmask->resize(1,1);
    }

  }
   os << LatticeExprNode(sum(image(0))).getFloat() 
	       << " Jy is the sum of clean components " << LogIO::POST;
  modified_p=true;
  setThreshold(maxRes);
  setNumberIterations(iterUsed);
  if (maskli != 0) {
    delete maskli;
    maskli = 0;
  }
  return(converged);
};

Matrix<Float>* HogbomCleanImageSkyModel::makeMaskMatrix(const Int& nx, 
						       const Int& ny,
							Int& newNx, Int& newNy,
							RO_LatticeIterator<Float>& maskIter,
							Int& xbeg,
							Int& xend,
							Int& ybeg,
							Int& yend) {

  LogIO os(LogOrigin("HogbomCleanImageSkyModel","makeMaskMatrix",WHERE)); 


  newNx=nx;
  newNy=ny;
  xbeg=nx/4;
  ybeg=ny/4;
  
  xend=xbeg+nx/2-1;
  yend=ybeg+ny/2-1;  
  Matrix<Float>* mask= new Matrix<Float>(maskIter.matrixCursor().shape());
  (*mask)=maskIter.matrixCursor();
  // ignore mask if none exists
  if(max(*mask) < 0.000001) {
    //os << "Mask seems to be empty; will CLEAN inner quarter" 
    //   << LogIO::WARN;
    //mask->resize(1,1);
    //mask->set(1.0);
    delete mask;
    mask=0;
    return mask;
  }
  // Now read the mask and determine the bounding box
  xbeg=nx-1;
  ybeg=ny-1;
  xend=0;
  yend=0;

  for (Int iy=0;iy<ny;iy++) {
    for (Int ix=0;ix<nx;ix++) {
      if((*mask)(ix,iy)>0.000001) {
	xbeg=min(xbeg,ix);
	ybeg=min(ybeg,iy);
	xend=max(xend,ix);
	yend=max(yend,iy);

      }
    }
  }
  // Now have possible BLC. Make sure that we don't go over the
  // edge later
  Bool larger_quarter=false;
  if((xend - xbeg)>nx/2) {
    //xbeg=nx/4-1; //if larger than quarter take inner of mask
    //os << LogIO::WARN << "Mask span over more than half the x-axis: Considering inner half of the x-axis"  << LogIO::POST;
    larger_quarter=true;
  } 
  if((yend - ybeg)>ny/2) { 
    //ybeg=ny/4-1;
    //os << LogIO::WARN << "Mask span over more than half the y-axis: Considering inner half of the y-axis" << LogIO::POST;
    larger_quarter=true;
  }  
  if(larger_quarter){
    newNx=2*nx;
    newNy=2*ny;
    Matrix<Float> * newMask= new Matrix<Float>(newNx, newNy);
    newMask->set(0.0);
    IPosition blc(2, (newNx-nx)/2, (newNy-ny)/2);
    IPosition trc(2, (newNx+nx)/2-1, (newNy+ny)/2-1);
    ((*newMask)(blc,trc)).assign(*mask);
    delete mask;
    mask=newMask;
    xbeg=xbeg+(newNx-nx)/2;
    ybeg=ybeg+(newNy-ny)/2;
    xend=xend+(newNx-nx)/2;
    yend=yend+(newNy-ny)/2;
  }
  
  xend=min(xend,xbeg+newNx/2-1);
  yend=min(yend,ybeg+newNy/2-1);   
  

  return mask;
}


} //# NAMESPACE CASA - END

