//# SkyEquation.cc: Implementation of Sky Equation classes
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
#include <casa/BasicSL/Complex.h>
#include <casa/Arrays/Matrix.h>
#include <measures/Measures/MeasConvert.h>
#include <synthesis/MeasurementEquations/SkyEquation.h>
#include <images/Images/ImageInterface.h>
#include <images/Images/SubImage.h>
#include <images/Regions/ImageRegion.h>
#include <synthesis/TransformMachines/SkyJones.h>
#include <synthesis/TransformMachines/FTMachine.h>

#include <coordinates/Coordinates/CoordinateSystem.h>
#include <coordinates/Coordinates/DirectionCoordinate.h>

#include <components/ComponentModels/Flux.h>
#include <components/ComponentModels/PointShape.h>
#include <components/ComponentModels/ConstantSpectrum.h>

#include <synthesis/TransformMachines/ComponentFTMachine.h>
#include <synthesis/MeasurementComponents/SkyModel.h>

#include <msvis/MSVis/VisSet.h>
#include <synthesis/TransformMachines/StokesImageUtil.h>
#include <msvis/MSVis/StokesVector.h>
#include <msvis/MSVis/VisBufferUtil.h>

#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/MatrixMath.h>
#include <casa/Utilities/Assert.h>
#include <casa/BasicSL/String.h>
#include <lattices/Lattices/Lattice.h>
#include <measures/Measures/UVWMachine.h>
#include <lattices/Lattices/ArrayLattice.h>
#include <lattices/LatticeMath/LatticeFFT.h>
#include <lattices/LEL/LatticeExpr.h>
#include <lattices/Lattices/TiledLineStepper.h>
#include <lattices/Lattices/LatticeIterator.h>
#include <lattices/Lattices/LatticeStepper.h>
#include <lattices/LRegions/LCRegion.h>
#include <casa/Containers/Block.h>

#include <casa/Exceptions/Error.h>
#include <msvis/MSVis/VisibilityIterator.h>
#include <msvis/MSVis/VisBuffer.h>
///////////////#include <msvis/MSVis/VisBufferAsync.h>
#include <casa/iostream.h>

#include <casa/System/ProgressMeter.h>

#include <memory>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

// ***************************************************************************
// ********************  Start of public member functions ********************
// ***************************************************************************


//----------------------------------------------------------------------
SkyEquation::SkyEquation(SkyModel& sm, VisSet& vs, FTMachine& ft,
			 FTMachine& ift)
  : sm_(&sm), vs_(&vs), ft_(&ft), ift_(&ift), cft_(0), ej_(0), dj_(0), 
    tj_(0), fj_(0), iDebug_p(0), isPSFWork_p(false), noModelCol_p(false), isBeginingOfSkyJonesCache_p(true), doflat_p(false)
{
  rvi_p=&(vs_->iter());
  wvi_p=&(vs_->iter());

  vb_p.set (rvi_p);
};

//----------------------------------------------------------------------
SkyEquation::SkyEquation(SkyModel& sm, VisSet& vs, FTMachine& ft)
  : sm_(&sm), vs_(&vs), ft_(&ft), ift_(&ft), cft_(0), ej_(0), dj_(0), 
    tj_(0), fj_(0), iDebug_p(0), isPSFWork_p(false),noModelCol_p(false), isBeginingOfSkyJonesCache_p(true), doflat_p(false)
{
  rvi_p=&(vs_->iter());
  wvi_p=&(vs_->iter());


  vb_p.set (rvi_p);
};

//----------------------------------------------------------------------
SkyEquation::SkyEquation(SkyModel& sm, VisSet& vs, FTMachine& ft,
			 FTMachine& ift, ComponentFTMachine& cft)
  : sm_(&sm), vs_(&vs), ft_(&ft), ift_(&ift), cft_(&cft), ej_(0),
    dj_(0), tj_(0), fj_(0), iDebug_p(0), isPSFWork_p(false),noModelCol_p(false),isBeginingOfSkyJonesCache_p(true), doflat_p(false)
{
  rvi_p=&(vs_->iter());
  wvi_p=&(vs_->iter());


  vb_p.set (rvi_p);
};

//----------------------------------------------------------------------
SkyEquation::SkyEquation(SkyModel& sm, VisSet& vs, FTMachine& ft,
			 ComponentFTMachine& cft, Bool noModelCol)
  : sm_(&sm), vs_(&vs), ft_(&ft), ift_(&ft), cft_(&cft), ej_(0),
    dj_(0), tj_(0), fj_(0), iDebug_p(0), isPSFWork_p(false), 
    noModelCol_p(noModelCol),isBeginingOfSkyJonesCache_p(true),doflat_p(false)
{

  rvi_p=&(vs_->iter());

  wvi_p=&(vs_->iter());
  vb_p.set (rvi_p);
};

SkyEquation::SkyEquation(SkyModel& sm, ROVisibilityIterator& vi, FTMachine& ft,
			 ComponentFTMachine& cft, Bool noModelCol)
  : sm_(&sm), ft_(&ft), ift_(&ft), cft_(&cft), ej_(0),
    dj_(0), tj_(0), fj_(0), iDebug_p(0), isPSFWork_p(false), 
    noModelCol_p(noModelCol),isBeginingOfSkyJonesCache_p(true), doflat_p(false){


  //visiter is read only
  rvi_p=&vi;
  if(rvi_p->ms().isWritable())
    wvi_p=static_cast<VisibilityIterator *>(&vi);
  else
    wvi_p=NULL;

  vb_p.set (rvi_p);
}

//----------------------------------------------------------------------
SkyEquation::~SkyEquation() {
}

//---------------------------------------------------------------------- 
SkyEquation& SkyEquation::operator=(const SkyEquation& other)
{
  if(this!=&other) {
    sm_=other.sm_;
    vs_=other.vs_;
    ft_=other.ft_;
    ift_=other.ift_;
    cft_=other.cft_;
    ej_=other.ej_;
    dj_=other.dj_;
    tj_=other.tj_;
    fj_=other.fj_;
    rvi_p=other.rvi_p;
    wvi_p=other.wvi_p;
    doflat_p=other.doflat_p;
    vb_p = other.vb_p;
  };
  return *this;
};

//----------------------------------------------------------------------
SkyEquation::SkyEquation(const SkyEquation& other)
{
  operator=(other);
}

//----------------------------------------------------------------------
void SkyEquation::setSkyJones(SkyJones& j) {
  SkyJones::Type t=j.type();
  switch(t) {
  case SkyJones::E: 
    ej_=&j;
    break;
  case SkyJones::D: 
    dj_=&j;
    break;
  case SkyJones::T: 
    tj_=&j;
    break;
  case SkyJones::F: 
    fj_=&j;
    break;
  }
};

//----------------------------------------------------------------------
// Predict the Sky coherence
//void SkyEquation::predictComponents(Bool& incremental, Bool& initialized){
//  predictComponents(incrementa,initialized,MS::MODEL_DATA);
//}

void SkyEquation::predictComponents(Bool& incremental, Bool& initialized,  MS::PredefinedColumns Type){
  // Initialize 
  
   // Do the component model only if this is not an incremental update;
  if(sm_->hasComponentList() &&  !incremental ) {
    //if(noModelCol_p)
    //    throw(AipsError("Cannot deal with componentlists without using scratch columns yet"));
    if(wvi_p==NULL)
      throw(AipsError("Cannot save model in non-writable ms"));
    VisIter& vi=*wvi_p;
    checkVisIterNumRows(vi);
    VisBufferAutoPtr vb(vi);

    // Reset the various SkyJones
    resetSkyJones();

    // Loop over all visibilities

//    This code is currently commented out because numberCoh
//    called on the VI is weird relative to the VI Async semantics,
//    at least at first glance
//
//    Int cohDone=0;
//    ProgressMeter pm(1.0, Double(vi.numberCoh()),
//		     "Predicting component coherences",
//		     "", "", "", true);
    Int oldmsid=-1;

    for (vi.originChunks();vi.moreChunks();vi.nextChunk()) {
      for (vi.origin(); vi.more(); vi++) {
	if(!noModelCol_p){
	  if(!incremental&&!initialized) {
	    vb->setModelVisCube(Complex(0.0,0.0));
	    //	  vi.setVis(vb->modelVisCube(),VisibilityIterator::Model);
	  }
	  

	  // get always fills Model
	  get(* vb, sm_->componentList() );
	  
	  // and write it to the model MS
	  switch(Type) {
	  case MS::MODEL_DATA:	  
	    vi.setVis(vb->modelVisCube(),VisibilityIterator::Model);
	    break;
	  case MS::DATA:	  
	    vi.setVis(vb->modelVisCube(),VisibilityIterator::Observed);
	    break;
	  case MS::CORRECTED_DATA:	  
	    vi.setVis(vb->modelVisCube(),VisibilityIterator::Corrected);
	    break;
	  default:
	    throw (AipsError("Programmer made a wrong call"));
	  }
	}
	else{
	  if(vb->msId() != oldmsid){
	    oldmsid=vb->msId();
	    String err;
	    Record clrec;
	    //cerr << "in componentlist saving" << endl;
	    if(anyGT(Int(Stokes::RR), vb->corrType())){
	      throw(AipsError("Invalid MS: correlation type cannot be decomposed into feed product; DataDescId="+String::toString(vb->dataDescriptionId()) ));
	    } 
	    if(!sm_->componentList().toRecord(err, clrec))
	      throw(AipsError("Error saving componentlist: "+err));
	    vi.putModel(clrec, true, true);
	  }

	}
//  See above commented-out code
//	cohDone+=vb->nRow();
//	pm.update(Double(cohDone));
      }
    }
    if(!incremental&&!initialized) initialized=true;
  }


}

void SkyEquation::predict(Bool incremental,  MS::PredefinedColumns Type) {



  //  if(noModelCol_p)
  //        throw(AipsError("Cannot predict model vis without using scratch columns yet"));
  AlwaysAssert(cft_, AipsError);
  AlwaysAssert(sm_, AipsError);
  //AlwaysAssert(vs_, AipsError);
  if(sm_->numberOfModels()!= 0)  AlwaysAssert(ok(),AipsError);
  // Initialize 
  VisIter& vi= *wvi_p;
  checkVisIterNumRows(vi);

  VisBufferAutoPtr vb(vi);

  // Reset the visibilities only if this is not an incremental
  // change to the model
  Bool initialized=false;
  // **** predictcomponents doesn't do anything if incremental!
  // it gets components into vb->model, and writes to vi::(Type)
  predictComponents(incremental, initialized, Type);

  // Now do the images
  for (Int model=0;model<sm_->numberOfModels();model++) {      
    
    if( (sm_->isEmpty(model)) && (model==0) && !initialized && !incremental){ 
      // We are at the begining with an empty model start
      for (vi.originChunks();vi.moreChunks();vi.nextChunk()) {
	for (vi.origin(); vi.more(); vi++) {
	  switch(Type) {
	  case MS::MODEL_DATA:
	    vb->setModelVisCube(Complex(0.0,0.0));
	    vi.setVis(vb->modelVisCube(),VisibilityIterator::Model);
	    break;
	  case MS::DATA:
	    vb->setModelVisCube(Complex(0.0,0.0));
	    vi.setVis(vb->modelVisCube(),VisibilityIterator::Observed);
	    break;
	  case MS::CORRECTED_DATA:
	    vb->setModelVisCube(Complex(0.0,0.0));
	    vi.setVis(vb->modelVisCube(),VisibilityIterator::Corrected);
	    break;
	  default:
	    throw (AipsError("Programmer made a wrong call"));
	  }
	}
      }
      //if (!incremental&&!initialized) initialized=true;


    }
    // Don't bother with empty images
    if(!sm_->isEmpty(model)) {
      
      // Change the model polarization frame
      if(vb->polFrame()==MSIter::Linear) {
	StokesImageUtil::changeCStokesRep(sm_->cImage(model),
					  StokesImageUtil::LINEAR);
      }
      else {
	StokesImageUtil::changeCStokesRep(sm_->cImage(model),
					  StokesImageUtil::CIRCULAR);
      }
      
      scaleImage(model, incremental);

      // Reset the various SkyJones
      resetSkyJones();
      

      // Initialize get (i.e. Transform from Sky)
      vi.originChunks();
      vi.origin();      
      initializeGet(* vb, 0, model, incremental);
      Int cohDone=0;
      
      ostringstream modelName;modelName<<"Model "<<model+1
				    <<" : predicting coherences";
      ProgressMeter pm(1.0, Double(vi.numberCoh()),
		       modelName, "", "", "", true);
      // Loop over all visibilities
      for (vi.originChunks();vi.moreChunks();vi.nextChunk()) {
	for (vi.origin(); vi.more(); vi++) {
	  if(!incremental&&!initialized) {
	    vb->setModelVisCube(Complex(0.0,0.0));
	    //	    vi.setVis(vb->modelVisCube(),VisibilityIterator::Model);
	    switch(Type) {
	    case MS::MODEL_DATA:
	      vi.setVis(vb->modelVisCube(),VisibilityIterator::Model);
	      break;
	    case MS::DATA:
	      vi.setVis(vb->modelVisCube(),VisibilityIterator::Observed);
	      break;
	    case MS::CORRECTED_DATA:
	      vi.setVis(vb->modelVisCube(),VisibilityIterator::Corrected);
	      break;
	    default:
	      throw (AipsError("Programmer made a wrong call"));
	    }
	  }
	  // get the model visibility (adds to vb->model)
	  //get(vb,model,incremental);
	  // this version takes Type and reads existing Type col instead 
	  // of hardcoded existing MODEL column
	  get(* vb,model,incremental,Type);
	  // and write it to VI	  
	  switch(Type) {
	  case MS::MODEL_DATA:
	    vi.setVis(vb->modelVisCube(),VisibilityIterator::Model);
	    break;
	  case MS::DATA:
	    vi.setVis(vb->modelVisCube(),VisibilityIterator::Observed);
	    break;
	  case MS::CORRECTED_DATA:
	    vi.setVis(vb->modelVisCube(),VisibilityIterator::Corrected);
	    break;
	  default:
	    throw (AipsError("Programmer made a wrong call"));
	  }
	  cohDone+=vb->nRow();
	  pm.update(Double(cohDone));
	}
      }
      finalizeGet();
      unScaleImage(model, incremental);
      if(!incremental&&!initialized) initialized=true;
    
    } else {
      cout << "WARN: model is empty." << endl;
    }
  }
}

//----------------------------------------------------------------------
void SkyEquation::gradientsChiSquared(const Matrix<Bool>&,
				      SkyJones&) {
  throw(AipsError("SkyEquation:: solution for SkyJones not yet implemented"));
}

//----------------------------------------------------------------------
void SkyEquation::gradientsChiSquared(Bool incremental, Bool commitModel) {
  AlwaysAssert(ok(),AipsError);

  if ((ft_->name() == "PBWProjectFT"))
    {
      ft_->setNoPadding(false);
      fullGradientsChiSquared(incremental);
    }
  else
    {
  Bool forceFull=true;
  // for these 2 gridders force incremental
  if((ft_->name() == "MosaicFT") || (ft_->name() == "WProjectFT"))
    forceFull=true;

  if( (sm_->numberOfModels() != 1) || !ft_->isFourier() || !incremental 
      || forceFull){
    if(commitModel || !noModelCol_p){
      ft_->setNoPadding(false);
      fullGradientsChiSquared(incremental);
    }
    else{
      // For now use corrected_data...
      ft_->setNoPadding(true);
      cout << "This mode is gone...we should not be coming here" << endl;
    }
  }
  else {
    incrementGradientsChiSquared();
  }
    }
}
//----------------------------------------------------------------------
void SkyEquation::fullGradientsChiSquared(Bool incremental) {

  AlwaysAssert(ok(),AipsError);

  // Predict the visibilities
  predict(incremental);

  sumwt = 0.0;
  chisq = 0.0;

  // Initialize the gradients
  sm_->initializeGradients();

  ROVisIter& vi(*rvi_p);

  // Loop over all models in SkyModel
  for (Int model=0;model<sm_->numberOfModels();model++) {

    // Reset the various SkyJones
    resetSkyJones();

    // Loop over all visibilities and pixels
    checkVisIterNumRows(vi);
    VisBuffer vb(vi);
      
    if(sm_->isSolveable(model)) {

      // Initialize 
      scaleImage(model, incremental);

      vi.originChunks();
      vi.origin();

      // Change the model polarization frame
      if(vb.polFrame()==MSIter::Linear) {
	StokesImageUtil::changeCStokesRep(sm_->cImage(model),
					  StokesImageUtil::LINEAR);
      }
      else {
	StokesImageUtil::changeCStokesRep(sm_->cImage(model),
					  StokesImageUtil::CIRCULAR);
      }

      initializePut(vb, model);
      Int cohDone=0;
      
      ostringstream modelName;modelName<<"Model "<<model+1
				    <<" : transforming residuals";
      ProgressMeter pm(1.0, Double(vi.numberCoh()),
		       modelName, "", "", "", true);
      // Loop over the visibilities, putting VisBuffers
      
      for (vi.originChunks();vi.moreChunks();vi.nextChunk()) {
	for (vi.origin(); vi.more(); vi++) {
	  vb.modelVisCube()-=vb.correctedVisCube();
	  //	  vb.setVisCube(vb.modelVisCube());
	  put(vb, model, false, FTMachine::MODEL);
	  cohDone+=vb.nRow();
	  pm.update(Double(cohDone));
	}
      }
      // Do the transform, apply the SkyJones transformation
      // and sum the statistics for this model
      finalizePut(* vb_p, model);
      unScaleImage(model, incremental);
    }
  }

  fixImageScale();

  // Finish off any calculations needed internal to SkyModel
  sm_->finalizeGradients();

}

//----------------------------------------------------------------------


//----------------------------------------------------------------------


void SkyEquation::incrementGradientsChiSquared() {

  AlwaysAssert(ok(),AipsError);

  // Check to see if we need to make the XFRs
  if(!sm_->hasXFR(0)) makeComplexXFRs();

  ROVisIter& vi(*rvi_p);

  // Reset the various SkyJones
  resetSkyJones();

  // Loop over all models in SkyModel
  for (Int model=0;model<sm_->numberOfModels();model++) {

    if(sm_->isSolveable(model)) {

      iDebug_p++;

      scaleDeltaImage(model);
      VisBuffer vb(vi);
      vi.origin();

      // Change the model polarization frame
      if(vb.polFrame()==MSIter::Linear) {
	StokesImageUtil::changeCStokesRep(sm_->cImage(model),
					  StokesImageUtil::LINEAR);
      }
      else {
	StokesImageUtil::changeCStokesRep(sm_->cImage(model),
					  StokesImageUtil::CIRCULAR);
      }

      Int numXFR=0;
      vi.originChunks();
      vi.origin();
      initializePutConvolve(vb, model, numXFR);
      // Iterate
      for (vi.originChunks();vi.moreChunks();vi.nextChunk()) {
	for (vi.origin(); vi.more(); vi++) {
	  putConvolve(vb, model, numXFR);
	}
      }
      // Finish off
      finalizePutConvolve(* vb_p, model, numXFR);
      unScaleDeltaImage(model);    
    }
  }


  // Finish off any calculations needed internal to SkyModel
  sm_->finalizeGradients();
};

//----------------------------------------------------------------------
void SkyEquation::makeComplexXFRs() 
{

  AlwaysAssert(ok(),AipsError);

  ROVisIter& vi(*rvi_p);

  // Loop over all models in SkyModel
  for (Int model=0;model<sm_->numberOfModels();model++) {

    if(sm_->isSolveable(model)) {

      // Loop over all visibilities and pixels
      VisBuffer vb(vi);
      
      // Change the model polarization frame
      if(vb.polFrame()==MSIter::Linear) {
	StokesImageUtil::changeCStokesRep(sm_->cImage(model),
					  StokesImageUtil::LINEAR);
      }
      else {
	StokesImageUtil::changeCStokesRep(sm_->cImage(model),
					  StokesImageUtil::CIRCULAR);
      }

      // Initialize put (i.e. transform to Sky) for this model
      // and XFR
      vi.origin();
      Int numXFR=0;
      vi.originChunks();
      vi.origin();
      initializePutXFR(vb, model, numXFR);
      Int cohDone=0;
      
      ostringstream modelName;modelName<<"Model "<<model+1
				    <<" : transforming to PSF";
      ProgressMeter pm(1.0, Double(vi.numberCoh()),
		       modelName, "", "", "", true);
      // Loop over the visibilities, putting VisBuffers
      
      for (vi.originChunks();vi.moreChunks();vi.nextChunk()) {
	for (vi.origin(); vi.more(); vi++) {
	  
	  vb.setVisCube(Complex(1.0,0.0));
	  putXFR(vb, model, numXFR);
	  cohDone+=vb.nRow();
	  pm.update(Double(cohDone));
	}
      }
      // Do the transform, apply the SkyJones transformation
      // and sum the statistics for this model
      finalizePutXFR(* vb_p, model, numXFR);
    }
  }
}

//----------------------------------------------------------------------
// Solve for a SkyJones
Bool SkyEquation::solveSkyJones(SkyJones& sj) {
  setSkyJones(sj);
  return sj.solve(*this);
}

//----------------------------------------------------------------------
// We make an approximate PSF for each plane.We only do this per model
// since we may not need all PSFs.
// ************* Note that this overwrites the model! ******************
void SkyEquation::makeApproxPSF(Int model, ImageInterface<Float>& psf) {

  LogIO os(LogOrigin("SkyEquation", "makeApproxPSF"));

  AlwaysAssert(ok(), AipsError);
  AlwaysAssert(cft_, AipsError);
  AlwaysAssert(sm_, AipsError);
  //AlwaysAssert(vs_, AipsError);
  
  ft_->setNoPadding(noModelCol_p);

  isPSFWork_p= true; // avoid PB correction etc for PSF estimation
  Bool doPSF=true;
  if(ft_->name() == "MosaicFT") {
    // Reset the various SkyJones
    doPSF=false;
    resetSkyJones();
    
    VisIter& vi(*wvi_p);
    checkVisIterNumRows(vi);
    // Loop over all visibilities and pixels
    VisBuffer vb(vi);
    
    vi.originChunks();
    vi.origin();
    
    // Change the model polarization frame
    if(vb.polFrame()==MSIter::Linear) {
      StokesImageUtil::changeCStokesRep(sm_->cImage(model),
					StokesImageUtil::LINEAR);
    }
    else {
      StokesImageUtil::changeCStokesRep(sm_->cImage(model),
					StokesImageUtil::CIRCULAR);
    }
    
    IPosition start(4, sm_->image(model).shape()(0)/2,
		    sm_->image(model).shape()(1)/2, 0, 0);
    IPosition shape(4, 1, 1, sm_->image(model).shape()(2), sm_->image(model).shape()(3));
    Array<Float> line(shape);
    TempImage<Float> savedModel(sm_->image(model).shape(),
				sm_->image(model).coordinates());
    savedModel.copyData(sm_->image(model));
    sm_->image(model).set(0.0);
    line=1.0;
    sm_->image(model).putSlice(line, start);
    //initializeGet(vb, -1, model, false);
    StokesImageUtil::From(sm_->cImage(model), static_cast <const ImageInterface<Float>& >(sm_->image(model)));
    ft_->initializeToVis(sm_->cImage(model),vb);
    // Loop over all visibilities
    for (vi.originChunks();vi.moreChunks();vi.nextChunk()) {
      for (vi.origin(); vi.more(); vi++) {
       	vb.setModelVisCube(Complex(0.0,0.0));
	//	get(vb, model, false);
	ft_->get(vb);
	vi.setVis(vb.modelVisCube(),VisibilityIterator::Model);
      }
    }
    finalizeGet();
    
    sm_->image(model).copyData(savedModel);
  }
  
  // Initialize the gradients
  sm_->initializeGradients();
  

  ROVisIter& vi(*rvi_p);
  
  // Reset the various SkyJones
  resetSkyJones();
  
  checkVisIterNumRows(vi);
  // Loop over all visibilities and pixels
  VisBuffer vb(vi);


  vi.originChunks();
  vi.origin();
  
  // Change the model polarization frame
  if(vb.polFrame()==MSIter::Linear) {
    StokesImageUtil::changeCStokesRep(sm_->cImage(model),
				      StokesImageUtil::LINEAR);
  }
  else {
    StokesImageUtil::changeCStokesRep(sm_->cImage(model),
				      StokesImageUtil::CIRCULAR);
  }


  initializePut(vb, model);
  
  // Loop over the visibilities, putting VisBuffers
  for (vi.originChunks();vi.moreChunks();vi.nextChunk()) {
    for (vi.origin(); vi.more(); vi++) {
      // vb.setVisCube(vb.modelVisCube());
      put(vb, model, doPSF, FTMachine::MODEL);
    }
  }
  
  // Do the transform, apply the SkyJones transformation
  finalizePut(* vb_p, model);
  sm_->finalizeGradients();
  fixImageScale();
  LatticeExpr<Float> le(iif(sm_->ggS(model)>(0.0),
  			    (sm_->gS(model)/sm_->ggS(model)), 0.0));
  psf.copyData(le);
  LatticeExprNode maxPSF=max(psf);
  Float maxpsf=maxPSF.getFloat();
  if(abs(maxpsf-1.0) > 1e-3) {
    os << "Maximum of approximate PSF for field " << model << " = "
       << maxpsf << " : renormalizing to unity" <<  LogIO::POST;
  }
  if(maxpsf > 0.0 ){
    LatticeExpr<Float> len(psf/maxpsf);
    psf.copyData(len);
 
  }
  else{
     throw(AipsError("SkyEquation:: PSF calculation resulted in a PSF lesser than 0 !"));

  }
 


  isPSFWork_p=false; // resseting this flag so that subsequent calculation uses
  // the right SkyJones correction;
  
  
}

void SkyEquation::makeApproxPSF(PtrBlock<ImageInterface<Float> *>& PSFs) {

  Int nmodels=PSFs.nelements();
  for (Int model=0; model < nmodels; ++model){
    makeApproxPSF(model, *(PSFs[model]));
    
  }


}
//----------------------------------------------------------------------
// Solve for a SkyModel
Bool SkyEquation::solveSkyModel() {
  return sm_->solve(*this);
}

// ***************************************************************************
// ********************  Start of private member functions *******************
// ***************************************************************************

// Initialize
void SkyEquation::initializeGet(const VisBuffer& vb, Int row, Int model,
				Bool incremental) {

  AlwaysAssert(ok(),AipsError);
  if(incremental) {
    applySkyJones(vb, row, sm_->deltaImage(model), sm_->cImage(model));
  }
  else {
    applySkyJones(vb, row, sm_->image(model), sm_->cImage(model));
  }
  ft_->initializeToVis(sm_->cImage(model),vb);
}
//

// Add the sky visibility for this coherence sample
VisBuffer& SkyEquation::get(VisBuffer& result, Int model,
			    Bool incremental,
			    MS::PredefinedColumns Type) {

  AlwaysAssert(ok(),AipsError);
  Int nRow=result.nRow();

  // we'll always return MODEL, but maybe expect existing data in another col 
  // yes this is a bit convoluted - probably should change whatever calls 
  // get to expect different columns.
  switch(Type) {
  case MS::MODEL_DATA:	  
    result.modelVisCube(); 
    break;
  case MS::DATA:	  
    result.visCube(); 
    result.setModelVisCube(result.visCube()); 
    break;
  case MS::CORRECTED_DATA:	  
    result.correctedVisCube(); 
    result.setModelVisCube(result.correctedVisCube()); 
    break;
  default:
    throw (AipsError("Programmer made a wrong call"));
  }

  //result.modelVisCube(); // get the visibility so vb will have it
  //VisBufferAutoPtr vb (result);
  // We need msId and mscolumns from result in the vb passed to FT below.
  // I can't find a public method to copy that ms info over, so instead, 
  // save the datacube and add it back at the end of this method.
  VisBuffer *oldvb,*vb;
  vb = &result; // this will get replaced by FT->get
  oldvb = new VisBuffer(result);
  oldvb->setModelVisCube(result.modelVisCube());

  Bool FTChanged=changedFTMachine(* vb);

  // we might need to recompute the "sky" for every single row, but we
  // avoid this if possible.
  Bool internalChanges=false;  // Does this VB change inside itself?
  Bool firstOneChanges=false;  // Has this VB changed from the previous one?
  if(ft_->name()!="MosaicFT"){
    changedSkyJonesLogic(result, firstOneChanges, internalChanges);
  } 
  if(internalChanges) {
    // Yes there are changes within this buffer: go row by row.
    // This will automatically catch a change in the FTMachine so
    // we don't have to check for that.
    for (Int row=0; row<nRow; row++) {
      finalizeGet();
      initializeGet(result, row, model, incremental);
      ft_->get(* vb,row);
    }
  }
  else if (FTChanged||firstOneChanges) {
    // This buffer has changed wrt the previous buffer, but
    // this buffer has no changes within it. Again we don't need to
    // check for the FTMachine changing.
    finalizeGet();
    initializeGet(result, 0, model, incremental);
    ft_->get(* vb);
  }
  else {
    ft_->get(* vb);
  }
  result.modelVisCube()+=oldvb->modelVisCube();
  return result;
}

void SkyEquation::finalizeGet()
{
  // Do nothing
  // MFSkyEquation has something to do, we just
  // need to have "get()" calling finalizeGet()
};


// Add the sky visibility for this component
VisBuffer& SkyEquation::get(VisBuffer& result,
                            const SkyComponent& component)
{

  AlwaysAssert(sm_,AipsError);
  AlwaysAssert(cft_,AipsError);

  Int nRow=result.nRow();

  result.modelVisCube(); // get the visibility so vb will have it
  VisBufferAutoPtr vb (result);
  SkyComponent corruptedComponent = component.copy();
  if(vb->polFrame()==MSIter::Linear) {
    if(corruptedComponent.flux().pol()==ComponentType::STOKES) {
      corruptedComponent.flux().convertPol(ComponentType::LINEAR);
    }
  }
  else {
    if(corruptedComponent.flux().pol()==ComponentType::STOKES) {
      corruptedComponent.flux().convertPol(ComponentType::CIRCULAR);
    }
  }
  // we might need to recompute the "sky" for every single row, but
  // we avoid this, if possible
  Bool internalChanges=false; // Does this VB change inside itself
  Bool firstOneChanges=false; // Has this VB changed from the previous one?
  changedSkyJonesLogic(result,firstOneChanges,internalChanges);
  if (internalChanges) // yes, we have to go row by row
      for (Int row=0;row<nRow;++row) {
	   SkyComponent tempComponent=corruptedComponent.copy();
	   applySkyJones(tempComponent,result,row);
	   cft_->get(* vb,tempComponent,row);
      }
  else { // we don't have a cache of corruptedComponent, therefore
	 // firstOneChanges is equivalent to the default case
      applySkyJones(corruptedComponent, result, 0);
      cft_->get(* vb, corruptedComponent);
  }
  result.modelVisCube()+=vb->modelVisCube();
  return result;
}



// Add the sky visibility for this component
VisBuffer& SkyEquation::get(VisBuffer& result,
			    const ComponentList& compList)
//			    MS::PredefinedColumns Type) {
  {

  AlwaysAssert(sm_,AipsError);
  AlwaysAssert(cft_,AipsError);

  Int nRow=result.nRow();

  result.modelVisCube(); // get the visibility so vb will have it
  //VisBufferAutoPtr vb(result);
  VisBuffer vb(result); // method only called using writable VI so no ROVIA
  //Above state copy  is incomplete like msId is not copied...make sure to use
  //result when metadata is important.
  result.setModelVisCube(Complex(0.0,0.0));
    
  // CURRENTLY we do not have the applySkyJones code which
  // works on ComponentLists;  so we need to get out the
  // individual SkyComponents here
  
  // we might need to recompute the "sky" for every single row, but
  // we avoid this, if possible
  Bool internalChanges=false; // Does this VB change inside itself
  Bool firstOneChanges=false; // Has this VB changed from the previous one?
  changedSkyJonesLogic(result,firstOneChanges,internalChanges);

  uInt ncomponents=compList.nelements();
  for (uInt icomp=0;icomp<ncomponents;icomp++) {
    SkyComponent component=compList.component(icomp).copy();
    if(vb.polFrame()==MSIter::Linear) {
      if(component.flux().pol()==ComponentType::STOKES) {
	component.flux().convertPol(ComponentType::LINEAR);
      }
    }
    else {
      if(component.flux().pol()==ComponentType::STOKES) {
	component.flux().convertPol(ComponentType::CIRCULAR);
      }
    }
    if (internalChanges) // yes, we have to go row by row
        for (Int row=0;row<nRow;++row) {
	     SkyComponent tempComponent=component.copy();
	     applySkyJones(tempComponent,result,row);
	     cft_->get(vb,tempComponent,row);
        }
    else { // we don't have a cache for component, therefore
	   // firstOneChanges is equivalent to the default case
         applySkyJones(component, result, 0);
         cft_->get(vb, component);
    }
    result.modelVisCube()+=vb.modelVisCube();
  }
  // Now add into the existing model visibility cube
  return result;
}


// Corrupt a SkyComponent
SkyComponent& SkyEquation::applySkyJones(SkyComponent& corruptedComponent,
					 const VisBuffer& vb,
					 Int row)
{
  if(!isPSFWork_p){
    //The last of the bolean in the following line forces a full spectral corruption
    // May be should do it on detection of fractional bandwidth 
    if(ej_) ej_->apply(corruptedComponent,corruptedComponent,vb,row, True, True);
    if(dj_) dj_->apply(corruptedComponent,corruptedComponent,vb,row);
    if(tj_) tj_->apply(corruptedComponent,corruptedComponent,vb,row);
    if(fj_) fj_->apply(corruptedComponent,corruptedComponent,vb,row);
  }
  return corruptedComponent;
}

void SkyEquation::initializePut(const VisBuffer& vb, Int model) {
  AlwaysAssert(ok(),AipsError);
  ift_->initializeToSky(sm_->cImage(model),sm_->weight(model),vb);
  assertSkyJones(vb, -1);
  vb_p->assign(vb, false);
  vb_p->updateCoordInfo();
}


void SkyEquation::put(const VisBuffer & vb, Int model, Bool dopsf, FTMachine::Type col) {

  AlwaysAssert(ok(),AipsError);

  Bool IFTChanged=changedIFTMachine(vb);

  // we might need to recompute the "sky" for every single row, but we
  // avoid this if possible.

  
  Int nRow=vb.nRow();
  Bool internalChanges=false;  // Does this VB change inside itself?
  Bool firstOneChanges=false;  // Has this VB changed from the previous one?
  if(ft_->name() != "MosaicFT"){
    changedSkyJonesLogic(vb, firstOneChanges, internalChanges);
  }
  if(internalChanges) {
    // Yes there are changes: go row by row. 
    for (Int row=0; row<nRow; row++) {
      if(IFTChanged||changedSkyJones(vb,row)) {
	// Need to apply the SkyJones from the previous row
	// and finish off before starting with this row
	finalizePut(* vb_p, model);
	initializePut(vb, model);
      }
      ift_->put(vb, row, dopsf, col);
    }
  }
  else if (IFTChanged||firstOneChanges) {

    if(!isBeginingOfSkyJonesCache_p){
      finalizePut(* vb_p, model);
    }
    initializePut(vb, model);
    isBeginingOfSkyJonesCache_p=false;
    ift_->put(vb, -1, dopsf, col);
  }
  else {
    ift_->put(vb, -1, dopsf, col);
  }

  isBeginingOfSkyJonesCache_p=false;
}




void SkyEquation::finalizePut(const VisBuffer& vb, Int model) {

  // Actually do the transform. Update weights as we do so.
  ift_->finalizeToSky();
  // 1. Now get the (unnormalized) image and add the 
  // weight to the summed weights
  Matrix<Float> delta;
  sm_->cImage(model).copyData(ift_->getImage(delta, false));
  sm_->weight(model)+=delta;
  // 2. Apply the SkyJones and add to grad chisquared
  applySkyJonesInv(vb, -1, sm_->cImage(model), sm_->work(model),
		   sm_->gS(model));

  // 3. Apply the square of the SkyJones and add this to gradgrad chisquared
  applySkyJonesSquare(vb, -1, sm_->weight(model), sm_->work(model),
		      sm_->ggS(model));

  
  // 4. Finally, we add the statistics
  sm_->addStatistics(sumwt, chisq);
}




void SkyEquation::initializePutXFR(const VisBuffer& vb, Int model,
				   Int numXFR) {
  AlwaysAssert(ok(),AipsError);
  Matrix<Float> weight;
  ift_->initializeToSky(sm_->XFR(model, numXFR), weight, vb);
  assertSkyJones(vb, -1);
  vb_p->assign(vb, false);
  vb_p->updateCoordInfo();
}

void SkyEquation::putXFR(const VisBuffer & vb, Int model, Int& numXFR) {

  AlwaysAssert(ok(),AipsError);

  Bool IFTChanged=changedIFTMachine(vb);

  Bool internalChanges=false;  // Does this VB change inside itself?
  Bool firstOneChanges=false;  // Has this VB changed from the previous one?
  changedSkyJonesLogic(vb, firstOneChanges, internalChanges);
  if(internalChanges) {
    // Yes there are changes within this buffer: go row by row. 
    Int nRow=vb.nRow();
    for (Int row=0; row<nRow; row++) {
      if(IFTChanged||changedSkyJones(vb,row)) {
	// Need to apply the SkyJones from the previous row
	// and finish off before starting with this row
	finalizePutXFR(* vb_p, model, numXFR);  //  also, this needs to know about
	                                      //  the vb row number 
	numXFR++; 
	initializePutXFR(vb, model, numXFR);
      }
      ift_->put(vb, row, true);
    }
  }
  else if (IFTChanged||firstOneChanges) {
    finalizePutXFR(* vb_p, model, numXFR);
    numXFR++; 
    initializePutXFR(vb, model, numXFR);
    ift_->put(vb, -1, true);
  } else {
    ift_->put(vb, -1, true);
  }
}

void SkyEquation::finalizePutXFR(const VisBuffer& vb, Int model, Int numXFR) 
{
  // Actually do the transform. FFT back to the visibility plane
  ift_->finalizeToSky();
  Matrix<Float> weights;
  sm_->XFR(model, numXFR).copyData(ift_->getImage(weights, false));
  LatticeFFT::cfft2d(sm_->XFR(model, numXFR));
  assertSkyJones(vb, -1);
  //  TempImage<Complex> *tip = ( TempImage<Complex> *) ( &(sm_->XFR(model, numXFR)) );
  //  tip->tempClose();
}

// Here we do the whole thing: apply SkyJones, FFT, cross-multiply
// by the XFR, inverseFFT and then apply SkyJones again
void SkyEquation::initializePutConvolve(const VisBuffer& vb, Int model,
					Int numXFR) 
{
  LogIO os(LogOrigin("SkyEquation", "initializePutConvolve"));
  AlwaysAssert(ok(),AipsError);
  AlwaysAssert(model>-1, AipsError);
  AlwaysAssert(numXFR>-1, AipsError);
  assertSkyJones(vb, -1);
  vb_p->assign(vb, false);
  vb_p->updateCoordInfo();
}

// Here we step through the visbuffer and do the convolution
// if something changes
void SkyEquation::putConvolve(const VisBuffer & vb, Int model, Int& numXFR) {

  AlwaysAssert(ok(),AipsError);

  Int nRow=vb.nRow();
  Bool internalChanges=false;  // Does this VB change inside itself?
  Bool firstOneChanges=false;  // Has this VB changed from the previous one?
  changedSkyJonesLogic(vb, firstOneChanges, internalChanges);
  if(internalChanges) {
    // Yes there are changes within this buffer: go row by row. 
    for (Int row=0; row<nRow; row++) {
      if(changedSkyJones(vb,row)) {
	// Need to apply the SkyJones from the previous row
	// and finish off before starting with this row
	finalizePutConvolve(* vb_p, model, numXFR);
	numXFR++;
	initializePutConvolve(vb, model, numXFR);
      }
    }
  }
  else if (firstOneChanges) {
    finalizePutConvolve(* vb_p, model, numXFR);
    numXFR++;
    initializePutConvolve(vb, model, numXFR);
  }
};

// Here we do the convolution and transform back
void SkyEquation::finalizePutConvolve(const VisBuffer& vb, Int model,
				      Int numXFR) 
{
  LogIO os(LogOrigin("SkyEquation", "finalizePutConvolve"));

  applySkyJones(vb, -1, sm_->deltaImage(model), sm_->cImage(model));
  LatticeFFT::cfft2d(sm_->cImage(model));
  LatticeExpr<Complex> latticeExpr(conj(sm_->XFR(model, numXFR))*sm_->cImage(model));
  sm_->cImage(model).copyData(latticeExpr);
  LatticeFFT::cfft2d(sm_->cImage(model), false);
  applySkyJonesInv(vb, -1, sm_->cImage(model), sm_->work(model),
		   sm_->gS(model));
}

void SkyEquation::changedSkyJonesLogic(const VisBuffer& vb, 
				       Bool& firstOneChanges,
				       Bool& internalChanges)
{
  if(changedSkyJones(vb,0)) {
    firstOneChanges=true;
  }
  Int lastrow = -1;
  if(changedSkyJonesBuffer(vb, 0, lastrow)) {
    internalChanges=true;
  } 
  return;
};


// Has the total SkyJones changed since the last application of the SkyJones?
Bool SkyEquation::changedSkyJones(const VisBuffer& vb, Int row) {
  if(ej_) if(ej_->changed(vb,row)) return true; // Electric field pattern
  if(dj_) if(dj_->changed(vb,row)) return true; // Polarization field pattern
  if(tj_) if(tj_->changed(vb,row)) return true; // Atmospheric gain
  if(fj_) if(fj_->changed(vb,row)) return true; // Faraday rotation
  return false;
};


// Has the FT Machine changed since the last application ?
Bool SkyEquation::changedFTMachine(const VisBuffer& vb) {
  return ft_->changed(vb);
};

// Has the Inverse FT Machine changed since the last application ?
Bool SkyEquation::changedIFTMachine(const VisBuffer& vb) {
  return ift_->changed(vb);
};


// Does the SkyJones change in this buffer starting at row1?
Bool SkyEquation::changedSkyJonesBuffer
(const VisBuffer& vb, Int row1, Int& row2) {
  Int row2temp = 0;
  row2 = vb.nRow() - 1;
  Bool didChange = false;
  if(ej_) {  // Electric field pattern
    if(ej_->changedBuffer(vb,row1, row2temp)) {
      didChange = true;
      row2 = min (row2, row2temp);
    }
  }
  if(dj_) {  // Polarization field pattern
    if(dj_->changedBuffer(vb,row1, row2temp)) {
      didChange = true;
      row2 = min (row2, row2temp);
    }
  }
  if(tj_) {  // Atmospheric gain
    if(tj_->changedBuffer(vb,row1, row2temp)) {
      didChange = true;
      row2 = min (row2, row2temp);
    }
  }
  if(fj_) {  // Faraday rotation
    if(fj_->changedBuffer(vb,row1, row2temp)) {
      didChange = true;
      row2 = min (row2, row2temp);
    }
  }
  return didChange;
};


void SkyEquation::setPhaseCenterTime(const Double time){

  if(cft_)
    cft_->setPhaseCenterTime(time);
  if(ft_)
    ft_->setPhaseCenterTime(time);
  if(ift_)
    ift_->setPhaseCenterTime(time);
}
Double SkyEquation::getPhaseCenterTime(){
  if(cft_)
    return cft_->getPhaseCenterTime();
  if(ft_)
    return ft_->getPhaseCenterTime();
  return -1.0;
}

// Reset all of the SkyJones to initial state
void SkyEquation::resetSkyJones() 
{
  if(ej_) ej_->reset(); // Electric field pattern
  if(dj_) dj_->reset(); // Polarization field pattern
  if(tj_) tj_->reset(); // Atmospheric gain
  if(fj_) fj_->reset(); // Faraday rotation

  isBeginingOfSkyJonesCache_p=true;
  
};

// Assure that we've taken care of the SkyJones terms
// (needed for e.g. putXFR)
void SkyEquation::assertSkyJones(const VisBuffer& vb, Int row) {
  if(ej_) ej_->assure(vb,row); // Electric field pattern
  if(dj_) dj_->assure(vb,row); // Polarization field pattern
  if(tj_) tj_->assure(vb,row); // Atmospheric gain
  if(fj_) fj_->assure(vb,row); // Faraday rotation
};

ImageInterface<Complex>& SkyEquation::applySkyJones(const VisBuffer& vb,
						    Int row,
						    ImageInterface<Float>& in,
						    ImageInterface<Complex>& out) {

  //Pol axis need not be same
  AlwaysAssert(in.shape()[0]==out.shape()[0], AipsError);
  AlwaysAssert(in.shape()[1]==out.shape()[1], AipsError);
  AlwaysAssert(in.shape()[3]==out.shape()[3], AipsError);
  // Convert from Stokes to Complex
  StokesImageUtil::From(out, in);

  // Now apply the SkyJones as needed
  if(!isPSFWork_p  && (!(ift_->name().contains("MosaicFT")))){
    if(ej_) ej_->apply(out,out,vb,row,true);
    if(dj_) dj_->apply(out,out,vb,row,true);
    if(tj_) tj_->apply(out,out,vb,row,true);
    if(fj_) fj_->apply(out,out,vb,row,true);
  }
  return out;
};


// Calculate gradChisq. In the SkyModel, this is used to update
// the estimated image.
void SkyEquation::applySkyJonesInv(const VisBuffer& vb, Int row,
				   ImageInterface<Complex>& in,
                                   ImageInterface<Float>& work,
				   ImageInterface<Float>& gS) {

  AlwaysAssert(in.shape()[0]==work.shape()[0], AipsError);
  AlwaysAssert(in.shape()[1]==work.shape()[1], AipsError);
  AlwaysAssert(in.shape()[3]==work.shape()[3], AipsError);
  AlwaysAssert(gS.shape()==work.shape(), AipsError);

  // Apply the various SkyJones to the current image
  // remembering to apply the Jones in the backward
  // direction
  if(!isPSFWork_p && (ift_->name() != "MosaicFT")){

    if(ej_) ej_->apply(in,in,vb,row,false);
    if(dj_) dj_->apply(in,in,vb,row,false);
    if(tj_) tj_->apply(in,in,vb,row,false);
    if(fj_) fj_->apply(in,in,vb,row,false);
  }
  // Convert to IQUV format
  if(isPSFWork_p) 
    { 
       // For the PSF, choose only those stokes planes that have a valid PSF
       StokesImageUtil::ToStokesPSF(work,in);
    }
  else 
    {
       StokesImageUtil::To(work,in);
    }

  // Now add to the existing gradChisq image
  LatticeExpr<Float> le(gS+work);
  gS.copyData(le);

};



// Calculate gradgradChisq. In the SkyModel, this is
// used to update the estimated image.
void SkyEquation::applySkyJonesSquare(const VisBuffer& vb, Int row,
				      Matrix<Float>& weights,
				      ImageInterface<Float>& work,
				      ImageInterface<Float>& ggS) {

  AlwaysAssert(work.shape()==ggS.shape(), AipsError);

  // First fill the work image with the appropriate value
  // of the weight.
  ift_->getWeightImage(work, weights);
   // Apply SkyJones as needed
  if((ft_->name() != "MosaicFT") ){
    if(ej_) ej_->applySquare(work,work,vb,row);
    if(dj_) dj_->applySquare(work,work,vb,row);
    if(tj_) tj_->applySquare(work,work,vb,row);
    if(fj_) fj_->applySquare(work,work,vb,row);
  }
  // Now add to the existing gradgradChisq image
  if((ft_->name() != "MosaicFT")){
    LatticeExpr<Float> le(ggS+work);
    ggS.copyData(le);
  }
  else{
    ggS.copyData(work);
  }
 
};


Bool SkyEquation::ok() {

  AlwaysAssert(sm_,AipsError);
  //AlwaysAssert(vs_,AipsError);
  AlwaysAssert(ft_,AipsError);
  AlwaysAssert(ift_,AipsError);
  AlwaysAssert(rvi_p, AipsError);

  return(true);
}


void SkyEquation::scaleImage(Int model)
{
  
  if (sm_->doFluxScale(model)){  

    LatticeExpr<Float> latticeExpr( iif(sm_->fluxScale(model) <= (0.0), 0.0, (sm_->image(model))/(sm_->fluxScale(model))) );
    sm_->image(model).copyData(latticeExpr);
    sm_->fluxScale(model).clearCache();
    sm_->image(model).clearCache();
  }
};

void SkyEquation::unScaleImage(Int model)
{

  if ( sm_->doFluxScale(model)){

    LatticeExpr<Float> latticeExpr( sm_->image(model) * (sm_->fluxScale(model)) );
    sm_->image(model).copyData(latticeExpr);
    sm_->fluxScale(model).clearCache();
    sm_->image(model).clearCache();
  }
};

void SkyEquation::scaleImage(Int model, Bool incremental)
{
  if (incremental) {
    scaleDeltaImage(model);
  } else {
    scaleImage(model);
  }
};

void SkyEquation::unScaleImage(Int model, Bool incremental)
{
  if (incremental) {
    unScaleDeltaImage(model);
  } else {
    unScaleImage(model);
  }
};

void SkyEquation::scaleDeltaImage(Int model)
{
  if ((sm_->doFluxScale(model))){
    sm_->deltaImage(model).copyData( (LatticeExpr<Float>)
				     (iif(sm_->fluxScale(model) <= (0.0), 0.0,
					  ((sm_->deltaImage(model))/(sm_->fluxScale(model))) )) );
    sm_->fluxScale(model).clearCache();
    sm_->deltaImage(model).clearCache();
  }
  
};

void SkyEquation::getCoverageImage(Int model, ImageInterface<Float>& im){
  if ((sm_->doFluxScale(model))){
    ift_->getFluxImage(im);
  }

}


void SkyEquation::unScaleDeltaImage(Int model)
{
  if ( (sm_->doFluxScale(model))){
    LatticeExpr<Float> latticeExpr( sm_->deltaImage(model) * (sm_->fluxScale(model)) );
    sm_->deltaImage(model).copyData(latticeExpr);
    sm_->fluxScale(model).clearCache();
    sm_->deltaImage(model).clearCache();
  } 
};

void SkyEquation::fixImageScale()
{
  LogIO os(LogOrigin("SkyEquation", "fixImageScale"));

  // make a minimum value to ggS
  // This has the same effect as Sault Weighting, but 
  // is implemented somewhat differently.
  // We also keep the fluxScale(mod) images around to
  // undo the weighting.
  
  if(ej_ || (ft_->name() == "MosaicFT") ) {
    Float ggSMax=0.0;
    for (Int model=0;model<sm_->numberOfModels();model++) {
    
      LatticeExprNode LEN = max( sm_->ggS(model) );
      ggSMax =  max(ggSMax,LEN.getFloat());
    }

    ggSMax_p=ggSMax;
    Float ggSMin1;
    Float ggSMin2;
    
    ggSMin1 = ggSMax * constPB_p * constPB_p;
    ggSMin2 = ggSMax * minPB_p * minPB_p;
    

    /*Don't print this for now
    if (scaleType_p == "SAULT") {
	os << "Using SAULT image plane weighting" << LogIO::POST;
    }
    else {
	os << "Using No image plane weighting" << LogIO::POST;
    }
    */
	
    for (Int model=0;model<sm_->numberOfModels();model++) {
      sm_->fluxScale(model).removeRegion ("mask0", RegionHandler::Any, false);
      if ((ft_->name()!="MosaicFT")) {
	if(scaleType_p=="SAULT"){
	  
	  // Adjust flux scale to account for ggS being truncated at ggSMin1
	  // Below ggSMin2, set flux scale to 0.0
	  // FluxScale * image => true brightness distribution, but
	  // noise increases at edge.
	  // if ggS < ggSMin2, set to Zero;
	  // if ggS > ggSMin2 && < ggSMin1, set to ggSMin1/ggS
	  // if ggS > ggSMin1, set to 1.0
	  sm_->fluxScale(model).copyData( (LatticeExpr<Float>) 
					  (iif(sm_->ggS(model) < (ggSMin2), 0.0,
					       sqrt((sm_->ggS(model))/ggSMin1) )) );
	  sm_->fluxScale(model).copyData( (LatticeExpr<Float>) 
					  (iif(sm_->ggS(model) > (ggSMin1), 1.0,
					       (sm_->fluxScale(model)) )) );
	  // truncate ggS at ggSMin1
	  sm_->ggS(model).copyData( (LatticeExpr<Float>) 
				    (iif(sm_->ggS(model) < (ggSMin1), ggSMin1*(sm_->fluxScale(model)), 
					 sm_->ggS(model)) )
				    );
	}

	else{

	  sm_->fluxScale(model).copyData( (LatticeExpr<Float>) 
					  (iif(sm_->ggS(model) < (ggSMin2), 0.0,
					       sqrt((sm_->ggS(model))/ggSMax) )) );
	  sm_->ggS(model).copyData( (LatticeExpr<Float>) 
					  (iif(sm_->ggS(model) < (ggSMin2), 0.0,
					       sqrt((sm_->ggS(model))*ggSMax) )) );

	}

      } else {
	/*
	if(ft_->name() != "MosaicFT"){
	  sm_->fluxScale(model).copyData( (LatticeExpr<Float>) 1.0 );
	  sm_->ggS(model).copyData( (LatticeExpr<Float>) 
	  			    (iif(sm_->ggS(model) < (ggSMin2), 0.0, 
	  				 sm_->ggS(model)) ));
	 

	}
	else{

	*/
	 
	
	  Int nXX=sm_->ggS(model).shape()(0);
	  Int nYY=sm_->ggS(model).shape()(1);
	  Int npola= sm_->ggS(model).shape()(2);
	  Int nchana= sm_->ggS(model).shape()(3);
	  IPosition blc(4,nXX, nYY, npola, nchana);
	  IPosition trc(4, nXX, nYY, npola, nchana);
	  blc(0)=0; blc(1)=0; trc(0)=nXX-1; trc(1)=nYY-1; 


	  //Those damn weights per plane can be wildly different so 
	  //deal with it properly here
	  for (Int j=0; j < npola; ++j){
	    for (Int k=0; k < nchana ; ++k){
	      
	      blc(2)=j; trc(2)=j;
	      blc(3)=k; trc(3)=k;
	      Slicer sl(blc, trc, Slicer::endIsLast);
	      SubImage<Float> fscalesub(sm_->fluxScale(model), sl, true);
	      SubImage<Float> ggSSub(sm_->ggS(model), sl, true);
	      Float planeMax;
	      LatticeExprNode LEN = max( ggSSub );
	      planeMax =  LEN.getFloat();
	      if(planeMax !=0){
		fscalesub.copyData( (LatticeExpr<Float>) 
				    (iif(ggSSub < (ggSMin2), 
					 0.0, (ggSSub/planeMax))));
		ggSSub.copyData( (LatticeExpr<Float>) 
				 (iif(ggSSub < (ggSMin2), 0.0, 
				      planeMax) ));
	
	

	      }
	    }

	  }
	
	  /*
	    ft_->getFluxImage(sm_->fluxScale(model));
	
	    sm_->ggS(model).copyData( (LatticeExpr<Float>) 
	    (iif(sm_->ggS(model) < (ggSMin2), 0.0,
					       (sm_->ggS(model)) )) );
	  */
	  //}	
      }
    
      //because for usual ft machines a applySJoneInv is done on the gS
      //in the finalizeput stage...need to understand if its necessary
      /*need to understand that square business
      if( (ft_->name() != "MosaicFT") && (!isPSFWork_p)){
	sm_->gS(model).copyData( (LatticeExpr<Float>) 
				 (iif(sm_->fluxScale(model) > 0.0, 
				      ((sm_->gS(model))/(sm_->fluxScale(model))), 0.0 )) );


      }
      */
      ///
      sm_->ggS(model).clearCache();
      sm_->fluxScale(model).clearCache();
    }

  }
}

void SkyEquation::checkVisIterNumRows(ROVisibilityIterator& vi){

  VisBuffer * vb = vi.getVisBuffer();
  VisBufferAutoPtr vbap;
  if (vb == NULL){
      VisBufferAutoPtr tmp (vi);
      vbap = tmp;
      vb = vbap.get();
  }

  vi.originChunks();
  vi.origin();
  Int nAnt=vb->numberAnt();
  if(nAnt >1){
    if (vb->nRow() < (nAnt*(nAnt-1)/4)){
      vi.setRowBlocking( nAnt*(nAnt-1)/2+nAnt);
      vi.originChunks();
      vi.origin();
    }
  }
}

String SkyEquation::associatedMSName(){
  return String("");
};

void SkyEquation::lock(){

  //Do nothing for now

}

void SkyEquation::unlock(){
  // Do nothing for now

}

} //# NAMESPACE CASA - END

