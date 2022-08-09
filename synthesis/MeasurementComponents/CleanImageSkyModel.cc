//# CleanImageSkyModel.cc: Implementation of CleanImageSkyModel classes
//# Copyright (C) 1996,1997,1998,1999,2000,2001,2003
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
#include <synthesis/MeasurementComponents/CleanImageSkyModel.h>
#include <components/ComponentModels/SkyComponent.h>
#include <components/ComponentModels/ComponentList.h>
#include <casacore/casa/OS/File.h>
#include <synthesis/MeasurementEquations/SkyEquation.h>
#include <synthesis/TransformMachines/StokesImageUtil.h>
#include <casacore/casa/Exceptions/Error.h>
#include <casacore/casa/BasicSL/String.h>
#include <casacore/casa/Utilities/Assert.h>

#include <casacore/tables/Tables/TableLock.h>

#include <sstream>

#include <casacore/casa/Logging/LogMessage.h>
#include <casacore/casa/Logging/LogIO.h>
#include <casacore/casa/Logging/LogSink.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN
#define NEED_UNDERSCORES
#if defined(NEED_UNDERSCORES)
#define maximg maximg_
#endif
extern "C" {
  void maximg(Float*, int*, Float*, int*, int*, int*, Float*, Float*);
};

  CleanImageSkyModel::CleanImageSkyModel() : ImageSkyModel(), doPolJoint_p(true)
{


}

Bool CleanImageSkyModel::addMask(Int thismodel, ImageInterface<Float>& mask)
{
  LogIO os(LogOrigin("CleanImageSkyModel", "addMask"));
  if(thismodel>=nmodels_p||thismodel<0) {
    os << LogIO::SEVERE << "Illegal model slot" << thismodel << LogIO::POST;
    return false;
  }
  if(Int(mask_p.nelements())<=thismodel) mask_p.resize(thismodel+1);
  mask_p[thismodel] = &mask;
  AlwaysAssert(mask_p[thismodel], AipsError);
  return true;
}
  
  CleanImageSkyModel::CleanImageSkyModel(const CleanImageSkyModel& other) : ImageSkyModel() {
  operator=(other);
};

CleanImageSkyModel::~CleanImageSkyModel() {
};

CleanImageSkyModel& CleanImageSkyModel::operator=(const CleanImageSkyModel& other) {
  if(this!=&other) {
    for (Int thismodel=0;thismodel<nmodels_p;thismodel++) {
      mask_p[thismodel]=other.mask_p[thismodel];
      fluxmask_p[thismodel]=other.fluxmask_p[thismodel];
    }
  };
  return *this;
}

Bool CleanImageSkyModel::add(ComponentList& compList)
{
  return ImageSkyModel::add(compList);
}
 
Int CleanImageSkyModel::add(ImageInterface<Float>& image, const Int maxNumXfr)
{
  Int index=ImageSkyModel::add(image, maxNumXfr);

  mask_p.resize(nmodels_p); 
  fluxmask_p.resize(nmodels_p);
  
  mask_p[index]=0;
  fluxmask_p[index]=0;

  return index;
}

Bool CleanImageSkyModel::hasMask(Int model) 
{
  if(mask_p.nelements()==0) return false;
  return (mask_p[model]);
}

ImageInterface<Float>& CleanImageSkyModel::mask(Int model) 
{
  AlwaysAssert(nmodels_p>0, AipsError);
  AlwaysAssert((model>-1)&&(model<nmodels_p), AipsError);
  AlwaysAssert(mask_p[model], AipsError);
  return *mask_p[model];
};

Bool CleanImageSkyModel::hasFluxMask(Int model) 
{
  if(fluxmask_p.nelements()==0) return false;
  return (fluxmask_p[model]);
}

ImageInterface<Float>& CleanImageSkyModel::fluxMask(Int model) 
{
  AlwaysAssert(nmodels_p>0, AipsError);
  AlwaysAssert((model>-1)&&(model<nmodels_p), AipsError);
  AlwaysAssert(fluxmask_p[model], AipsError);
  return *fluxmask_p[model];
};

Bool CleanImageSkyModel::addFluxMask(Int thismodel, ImageInterface<Float>& fluxMask)
{
  LogIO os(LogOrigin("CleanImageSkyModel", "add"));
  if(thismodel>=nmodels_p||thismodel<0) {
    os << LogIO::SEVERE << "Illegal model slot" << thismodel << LogIO::POST;
    return false;
  }
  if(Int(fluxmask_p.nelements())<=thismodel) fluxmask_p.resize(thismodel+1);
  fluxmask_p[thismodel] = &fluxMask;
  AlwaysAssert(fluxmask_p[thismodel], AipsError);
  return true;
}

void  CleanImageSkyModel::setJointStokesClean(Bool joint) {
  doPolJoint_p=joint;
}

// Find maximum residual
Float CleanImageSkyModel::maxField(Vector<Float>& imagemax,
				     Vector<Float>& imagemin) {

  LogIO os(LogOrigin("ImageSkyModel","maxField"));
  
  Float absmax=0.0;
  imagemax.resize(numberOfModels());
  imagemin.resize(numberOfModels());
  imagemax=-1e20;
  imagemin=1e20;

 
 

  // Loop over all models
  for (Int model=0;model<numberOfModels();model++) {
    // Find maximum of ggS for scaling in maximg
    Float maxggS=0.0;  
    {
      LatticeExprNode LEN = max(LatticeExpr<Float>(ggS(model)));
      maxggS = LEN.getFloat();
    }
    // Remember that the residual image can be either as specified
    // or created specially.
    ImageInterface<Float>* imagePtr=0;
    if(residual_p[model]) {
      imagePtr=residual_p[model];
    }
    else {
      imagePtr=(ImageInterface<Float> *)residualImage_p[model];
    }
    AlwaysAssert(imagePtr, AipsError);
    AlwaysAssert(imagePtr->shape().nelements()==4, AipsError);
    Int nx=imagePtr->shape()(0);
    Int ny=imagePtr->shape()(1);
    Int npol=imagePtr->shape()(2);
    Int nchan=imagePtr->shape()(3);

    //    AlwaysAssert((npol==1)||(npol==2)||(npol==4), AipsError);
    
    // Loop over all channels
    IPosition oneSlab(4, nx, ny, npol, 1);
    LatticeStepper ls(imagePtr->shape(), oneSlab, IPosition(4, 0, 1, 2, 3));
    IPosition onePlane(4, nx, ny, 1, 1);
    LatticeStepper fsls(ggS(model).shape(), oneSlab,
			IPosition(4,0,1,2,3));
    RO_LatticeIterator<Float> ggSli(ggS(model),fsls);
    LatticeIterator<Float> imageli(*imagePtr, ls);
    
    // If we are using a mask then reset the region to be
    // cleaned
    Array<Float> maskArray;
    RO_LatticeIterator<Float> maskIter;
    Bool cubeMask=false;
    
    Int domask=0;
    if(hasMask(model)) {
      Int mx=mask(model).shape()(0);
      Int my=mask(model).shape()(1);
      Int mpol=mask(model).shape()(2);
      Int nMaskChan=0;
      if(mask(model).shape().nelements()==4){
	nMaskChan=mask(model).shape()(3);
      }
      if( (nchan >1) && nMaskChan==nchan)
	cubeMask=true;
      if((mx != nx) || (my != ny) || (mpol != npol)){
	throw(AipsError("Mask image shape is not the same as dirty image"));
      }
      LatticeStepper mls(mask(model).shape(), oneSlab,
			 IPosition(4, 0, 1, 2, 3));
      
      RO_LatticeIterator<Float> maskli(mask(model), mls);
      maskli.reset();
      maskIter=maskli;
      if (maskli.cursor().shape().nelements() > 1) {
	domask=1;
	maskArray=maskli.cursor();
      }
    }
    
    Int chan=0;
    Float imax, imin;
    imax=-1E20; imagemax(model)=imax;
    imin=+1E20; imagemin(model)=imin;

    for (imageli.reset(),ggSli.reset();!imageli.atEnd();imageli++,ggSli++,chan++) {
      Float fmax, fmin;
      Bool delete_its;
      Bool delete_its2;
      // Renormalize by the weights
      if(cubeMask){
	if(maskIter.cursor().shape().nelements() > 1){
	  domask=1;
	  maskArray=maskIter.cursor();
	}
	maskIter++;
	
      }
      
      Cube<Float> weight(sqrt(ggSli.cursor().nonDegenerate(3)/maxggS));
      // resid=(imageli.cursor().nonDegenerate(3)) does not make a
      // copy.  This is a bug in LatticeIterator.cursor().
      //
      // As a result resid gets modified for the rest of the code
      // (residual image is multipled by sqrt(ggS)!!
      //
      // For now, using Cube::assign() to force a copy.
      Cube<Float>resid;resid.assign(imageli.cursor().nonDegenerate(3));
      for (Int pol=0;pol<npol;pol++) {
      	for (Int iy=0;iy<ny;iy++) {
      	  for (Int ix=0;ix<nx;ix++) {
      	    resid(ix,iy,pol)*=weight(ix,iy,pol);
      	  }
      	}
      }
      const Float* limage_data=resid.getStorage(delete_its);
      const Float* lmask_data=maskArray.getStorage(delete_its2);
      maximg((Float*)limage_data, &domask, (Float*)lmask_data,
	     &nx, &ny, &npol, &fmin, &fmax);

      
      resid.freeStorage(limage_data, delete_its);
      maskArray.freeStorage(lmask_data, delete_its2);
      if(fmax<0.99*imax) fmax=0.0;
      if(fmin>0.99*imin) fmin=0.0;
      if(abs(fmax)>absmax) absmax=abs(fmax);
      if(abs(fmin)>absmax) absmax=abs(fmin);
      if(fmin<imagemin(model)) imagemin(model)=fmin;
      if(fmax>imagemax(model)) imagemax(model)=fmax;
    }
  }
  return absmax;
};


} //# NAMESPACE CASA - END

