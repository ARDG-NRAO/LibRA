//# ClarkCleanImageSkyModel.cc: Implementation of ClarkCleanImageSkyModel class
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
#include <synthesis/MeasurementComponents/ClarkCleanImageSkyModel.h>
#include <images/Images/PagedImage.h>
#include <casa/OS/File.h>
#include <lattices/Lattices/LatticeStepper.h>
#include <lattices/Lattices/LatticeIterator.h>
#include <lattices/LEL/LatticeExpr.h>
#include <lattices/LEL/LatticeExprNode.h>
#include <lattices/LRegions/LCBox.h>
#include <lattices/Lattices/SubLattice.h>
#include <synthesis/MeasurementEquations/SkyEquation.h>
#include <casa/Exceptions/Error.h>
#include <casa/BasicSL/String.h>
#include <casa/Utilities/Assert.h>

#include <casa/sstream.h>

#include <casa/Logging/LogMessage.h>
#include <casa/Logging/LogIO.h>
#include <casa/Logging/LogSink.h>

#include <synthesis/MeasurementEquations/LatConvEquation.h>
#include <synthesis/MeasurementEquations/ClarkCleanLatModel.h>
#include <synthesis/MeasurementEquations/ClarkCleanProgress.h>


using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

  ClarkCleanImageSkyModel::ClarkCleanImageSkyModel() : CleanImageSkyModel(), itsProgress(0) {



}


ClarkCleanImageSkyModel::~ClarkCleanImageSkyModel()
{ 
  if (itsProgress) delete itsProgress; 
};

// Clean solver
Bool ClarkCleanImageSkyModel::solve(SkyEquation& se) {

  LogIO os(LogOrigin("ClarkCleanImageSkyModel","solve",WHERE));
  
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

  Float maxRes=0.0;
  Int iterused=0;
  
  if(hasMask(0))
    converged = clean(image(0), residual(0), PSF(0), mask(0), maxRes, iterused, gain(), numberIterations(),  
	  threshold(), cycleFactor_p, true, doPolJoint_p);
  else{
    ImageInterface<Float> *tmpMask=0;
    converged = clean(image(0), residual(0), PSF(0), *tmpMask, maxRes, iterused, gain(), numberIterations(),  
	  threshold(), cycleFactor_p, false, doPolJoint_p);
  }
  
  setThreshold(maxRes);
  setNumberIterations(iterused);
  modified_p=true;
  return(converged);
};


  Bool ClarkCleanImageSkyModel::clean(ImageInterface<Float>& image, ImageInterface<Float> & residual, 
				      ImageInterface<Float>& psf, ImageInterface<Float>& mask, Float& maxresidual, Int& iterused, Float gain, Int numIter,  
				      Float thresh, Float cycleFactor, Bool useMask, Bool doPolJoint){

    Bool converged=false;
    LogIO os(LogOrigin("ClarkCleanImageSkyModel","clean",WHERE)); 
    Int nx=image.shape()(0);
    Int ny=image.shape()(1);
    Int npol=image.shape()(2);
    Int nchan=image.shape()(3);
    Int polloop=1;
    Vector<String> stokesID(npol, "");
    if (!doPolJoint) {
      polloop=npol;
      CoordinateSystem cs=image.coordinates();
      Int stokesindex=cs.findCoordinate(Coordinate::STOKES);
      StokesCoordinate stcoord=cs.stokesCoordinate(stokesindex);
      for (Int jj=0; jj < npol ; ++jj){
	stokesID(jj)=Stokes::name(Stokes::type(stcoord.stokes()(jj)));      
      }
    }
  
  
    Int newNx=nx;
    Int newNy=ny;

    Int xbeg, xend, ybeg, yend;
    //default clean box
    xbeg=nx/4; 
    xend=3*nx/4-1;
    ybeg=ny/4; 
    yend=3*ny/4-1;

    Bool isCubeMask=false; 
    //AlwaysAssert((npol==1)||(npol==2)||(npol==4), AipsError);
  
    Lattice<Float>* mask_sl = 0;
    RO_LatticeIterator<Float>* maskli = 0;
  
    if(useMask) {
      // AlwaysAssert(mask(0).shape()(0)==nx, AipsError);
      // AlwaysAssert(mask(0).shape()(1)==ny, AipsError);
      if((mask.shape()(0)!=nx) || (mask.shape()(1)!=ny)){
	throw(AipsError("Mask image shape is different from dirty image"));
      }
      if(nchan >1){
	if(mask.shape()(3)==nchan){
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
      LatticeStepper mls(mask.shape(),
			 IPosition(4, nx, ny, 1, 1),
			 IPosition(4, 0, 1, 3, 2));
      maskli= new RO_LatticeIterator<Float>(mask, mls);
      maskli->reset();
      mask_sl=makeMaskSubLat(nx, ny, newNx, newNy, *maskli, xbeg, xend, 
			     ybeg, yend);
    }


    maxresidual=0.0;

    for (Int ichan=0; ichan < nchan; ichan++) {
      if(useMask && isCubeMask && ichan >0) {
	(*maskli)++;
	if(mask_sl) delete mask_sl;
	mask_sl=0;
	mask_sl=makeMaskSubLat(nx, ny, newNx, newNy, *maskli, xbeg, 
			       xend, ybeg, yend);       
      }

      if(nchan>1) {
	os<<"Processing channel "<<ichan+1<<" of "<<nchan<<LogIO::POST;
      }
      for (Int ipol=0; ipol < polloop; ++ipol){
	Int polbeg=0;
	Int polend=npol-1;
	if(!doPolJoint){
	  polbeg=ipol;
	  polend=ipol;
	  os << "Doing stokes "<< stokesID(ipol) << " image" <<LogIO::POST;
	}
	LCBox imagebox(IPosition(4, xbeg, ybeg, polbeg, ichan), 
		       IPosition(4, xend, yend, polend, ichan),
		       image.shape());
	LCBox psfbox(IPosition(4, 0, 0, 0, ichan), 
		     IPosition(4, nx-1, ny-1, 0, ichan),
		     psf.shape());

	SubLattice<Float> psf_sl(psf, psfbox, false);
	SubLattice<Float>  residual_sl(residual, imagebox, true);
	SubLattice<Float>  model_sl=SubLattice<Float>   (image, imagebox, true);
    
    
	ArrayLattice<Float> psftmp;
	
	if(nx != newNx){
	  
	  psftmp=ArrayLattice<Float> (IPosition(4, newNx, newNy, 1, 1));
	  psftmp.set(0.0);
	  Array<Float> tmparr=psf_sl.get();
	  psftmp.putSlice(tmparr, IPosition(4, (newNx-nx)/2, (newNy-ny)/2, 0,0));
	  psf_sl=SubLattice<Float>(psftmp, true);
 
	}
      
	TempLattice<Float> dirty_sl( residual_sl.shape());
	dirty_sl.copyData(residual_sl);
	TempLattice<Float> localmodel(model_sl.shape());
	localmodel.set(0.0);
      
	Float psfmax;
	{
	  LatticeExprNode node = max(psf_sl);
	  psfmax = node.getFloat();
	}
      
	if((psfmax==0.0) ||(useMask && (mask_sl == 0)) ) {
	  maxresidual=0.0;
	  iterused=0;
	  os << LogIO::NORMAL // Loglevel INFO
	     << "No data or blank mask for this channel: skipping" << LogIO::POST;
	} else {
	  LatConvEquation eqn(psf_sl, residual_sl);
	  ClarkCleanLatModel cleaner( localmodel );
	  cleaner.setResidual(dirty_sl);
	  if (mask_sl != 0 ) cleaner.setMask( *mask_sl );
	
	  /*
	  ClarkCleanProgress *cpp  = 0;
	  if (displayProgress_p) {
	    cpp = new ClarkCleanProgress( pgplotter_p );
	    cleaner.setProgress(*cpp);
	  }
	  */
	
	  cleaner.setGain(gain);
	  cleaner.setNumberIterations(numIter);
	  cleaner.setThreshold(thresh); 
	  cleaner.setPsfPatchSize(IPosition(2,51,51)); 
	  cleaner.setHistLength(1024);
	  cleaner.setMaxNumPix(32*1024);
	  cleaner.setCycleFactor(cycleFactor);
	  // clean if there is no mask or if it has mask AND mask is not empty 
	  cleaner.solve(eqn);
	  cleaner.setChoose(false);
          if (cleaner.numberIterations()>0) {
	    os << LogIO::NORMAL // Loglevel INFO
	       << "Clean used " << cleaner.numberIterations() << " iterations" 
	       << " in this round to get to a max residual of " << cleaner.threshold()
	       << LogIO::POST;
	  } 
	  LatticeExpr<Float> expr= model_sl + localmodel; 
	  model_sl.copyData(expr);
	  maxresidual=max(maxresidual, cleaner.getMaxResidual());
	  iterused=cleaner.numberIterations();
	  converged =  (cleaner.getMaxResidual() < thresh) 
	    || (cleaner.numberIterations()==0);
	  //      if (cpp != 0 ) delete cpp; cpp=0;
	  //      if (pgp != 0 ) delete pgp; pgp=0;
	}
      }
    }
    if (mask_sl != 0)  {
      delete mask_sl;
      mask_sl=0;
    }
    if (maskli !=0) {
      delete maskli;
      maskli=0;
    }
    os << LogIO::NORMAL // Loglevel INFO
       << LatticeExprNode(sum(image)).getFloat()
       << " Jy <- The sum of the clean components"
       << LogIO::POST;
    return converged;


  }

Lattice<Float>* ClarkCleanImageSkyModel::makeMaskSubLat(const Int& nx, 
							    const Int& ny, 
							Int& newNx, Int& newNy,
							    RO_LatticeIterator<Float>& maskIter,
							   Int& xbeg,
							   Int& xend,
							   Int& ybeg,
							   Int& yend) {

  LogIO os(LogOrigin("ClarkCleanImageSkyModel","makeMaskSubLat",WHERE)); 


  newNx=nx;
  newNy=ny;
  SubLattice<Float>* mask_sl = 0;
  xbeg=nx/4;
  ybeg=ny/4;
  
  xend=xbeg+nx/2-1;
  yend=ybeg+ny/2-1;  
  Matrix<Float> mask= maskIter.matrixCursor();
  if(max(mask) < 0.000001) {
    //zero mask ....
    return mask_sl;
  }
  // Now read the mask and determine the bounding box

  xbeg=nx-1;
  ybeg=ny-1;
  xend=0;
  yend=0;

  
  for (Int iy=0;iy<ny;iy++) {
    for (Int ix=0;ix<nx;ix++) {
      if(mask(ix,iy)>0.000001) {
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
    // xbeg=nx/4-1; //if larger than quarter take inner of mask
    //os << LogIO::WARN << "Mask span over more than half the x-axis: Considering inner half of the x-axis"  << LogIO::POST;
    larger_quarter=true;
  } 
  if((yend - ybeg)>ny/2) { 
    //ybeg=ny/4-1;
    //os << LogIO::WARN << "Mask span over more than half the y-axis: Considering inner half of the y-axis" << LogIO::POST;
    larger_quarter=true;
  }  

  // make sure xend, yend does not go out of bounds
  if(larger_quarter){
    xend=min(xend, nx-1);
    yend=min(yend, ny-1);

  }
  else{
    xend=min(xend,xbeg+nx/2-1);
    yend=min(yend,ybeg+ny/2-1); 
  }

  
  if ((xend > xbeg) && (yend > ybeg) ) {

    IPosition latshape(4, mask.shape()(0), mask.shape()(1), 1,1);
    ArrayLattice<Float> arrayLat(latshape);
    LCBox maskbox (IPosition(4, xbeg, ybeg, 0, 0), 
		   IPosition(4, xend, yend, 0, 0), 
    		   latshape);
    
    arrayLat.putSlice(mask, IPosition(4,0,0,0,0));
    mask_sl=new SubLattice<Float> (arrayLat, maskbox, false);
    


  }
  
  if(larger_quarter){
    newNx=2*nx;
    newNy=2*ny;
    /*xbeg=xbeg+(newNx-nx)/2;
    ybeg=ybeg+(newNy-ny)/2;
    xend=xend+(newNx-nx)/2;
    yend=yend+(newNy-ny)/2;
    */
  }
  return mask_sl;
}





} //# NAMESPACE CASA - END

