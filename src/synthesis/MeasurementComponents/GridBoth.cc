//# GridBoth.cc: Implementation of GridBoth class
//# Copyright (C) 1997,1998,1999,2000,2001,2002,2003
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

#include <synthesis/MeasurementComponents/GridBoth.h>
#include <synthesis/TransformMachines/SimpCompGridMachine.h>
#include <msvis/MSVis/VisBuffer.h>
#include <casacore/casa/Arrays/ArrayMath.h>
#include <casacore/images/Images/ImageInterface.h>
#include <casacore/images/Images/TempImage.h>

#include <synthesis/TransformMachines/StokesImageUtil.h>

#include <components/ComponentModels/Flux.h>
#include <components/ComponentModels/PointShape.h>
#include <components/ComponentModels/ConstantSpectrum.h>

#include <casacore/lattices/LRegions/LCBox.h>
#include <casacore/lattices/LEL/LatticeExpr.h>
#include <casacore/lattices/Lattices/SubLattice.h>
#include <casacore/lattices/Lattices/LatticeIterator.h>
#include <casacore/lattices/Lattices/LatticeStepper.h>
#include <casacore/lattices/LEL/LatticeExpr.h>
#include <casacore/casa/Containers/Record.h>
#include <casacore/casa/BasicSL/String.h>
#include <casacore/casa/Utilities/Assert.h>
#include <casacore/casa/Exceptions/Error.h>
#include <casacore/casa/OS/Timer.h>
#include <sstream>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

GridBoth::GridBoth(SkyJones& sj, Long icachesize,
		   Int itilesize, 
		   String sdConvType,
		   String synConvType,
		   Float padding,
		   Float sdScale,
		   Float sdWeight)
  : FTMachine(), synMachine_p(0), sdMachine_p(0), lastMachine_p(0),
    sdImage_p(0), synImage_p(0), sdScale_p(sdScale), sdWeight_p(sdWeight)
{
  synMachine_p = new GridFT(icachesize, itilesize, synConvType,
			    padding, false);
  sdMachine_p  = new SDGrid(sj, icachesize, itilesize, sdConvType, -1);
  ok();
}

GridBoth::GridBoth(SkyJones& sj, Long icachesize,
		   Int itilesize, 
		   MPosition mLocation,
		   String sdConvType,
		   String synConvType,
		   Float padding,
		   Float sdScale,
		   Float sdWeight)
  : FTMachine(), synMachine_p(0), sdMachine_p(0), lastMachine_p(0),
    sdImage_p(0), synImage_p(0), sdScale_p(sdScale), sdWeight_p(sdWeight)
{
  synMachine_p = new GridFT(icachesize, itilesize, synConvType, mLocation,
			    padding, false);
  sdMachine_p  = new SDGrid(mLocation, sj, icachesize, itilesize, sdConvType, -1);
  ok();
}

GridBoth::GridBoth(SkyJones& sj, Long icachesize,
		   Int itilesize, 
		   MPosition mLocation,
		   MDirection mDirection,
		   String sdConvType,
		   String synConvType,
		   Float padding,
		   Float sdScale,
		   Float sdWeight)
  
  : FTMachine(), synMachine_p(0), sdMachine_p(0), lastMachine_p(0),
    sdImage_p(0), synImage_p(0), sdScale_p(sdScale), sdWeight_p(sdWeight)
{
  synMachine_p = new GridFT(icachesize, itilesize, synConvType, mLocation,
			    mDirection, padding, false);
  sdMachine_p  = new SDGrid(mLocation, sj, icachesize, itilesize, 
			    sdConvType, -1);
  ok();
}

GridBoth::GridBoth(const RecordInterface& stateRec)
  : FTMachine()
{
  // Construct from the input state record
  String error;
  if (!fromRecord(error, stateRec)) {
    throw (AipsError("Failed to create gridder: " + error));
  };
}

//---------------------------------------------------------------------- 
GridBoth& GridBoth::operator=(const GridBoth& other)
{
  if(this!=&other) {
    synMachine_p=other.synMachine_p;
    sdMachine_p=other.sdMachine_p;
    lastMachine_p=other.lastMachine_p;
    sdImage_p=other.sdImage_p;
    synImage_p=other.synImage_p;
    sdScale_p=other.sdScale_p;
    sdWeight_p=other.sdWeight_p;
  };
  return *this;
};

//----------------------------------------------------------------------
  GridBoth::GridBoth(const GridBoth& other):FTMachine()
{
  operator=(other);
}

GridBoth::~GridBoth() {
  if(synMachine_p) delete synMachine_p; synMachine_p=0;
  if(sdMachine_p) delete sdMachine_p; sdMachine_p=0;
  if(sdImage_p) delete sdImage_p; sdImage_p=0;
  if(synImage_p) delete synImage_p; synImage_p=0;
}

//----------------------------------------------------------------------
Bool GridBoth::changed(const VisBuffer& vb) {
  return synMachine_p->changed(vb)||sdMachine_p->changed(vb);
}

// Initialize for a transform from the Sky domain. 
void GridBoth::initializeToVis(ImageInterface<Complex>& iimage,
			       const VisBuffer& vb)
{
  ok();

  lastMachine_p=0;
  
  image=&iimage;
  
  if(sdImage_p) delete sdImage_p;
  sdImage_p=new TempImage<Complex>(iimage.shape(), iimage.coordinates());
  AlwaysAssert(sdImage_p, AipsError);
  sdImage_p->copyData(iimage);
  sdMachine_p->initializeToVis(*sdImage_p, vb);
  
  if(synImage_p) delete synImage_p;
  synImage_p=new TempImage<Complex>(iimage.shape(), iimage.coordinates());
  AlwaysAssert(synImage_p, AipsError);
  synImage_p->copyData(iimage);
  synMachine_p->initializeToVis(*synImage_p, vb);

  AlwaysAssert(sdImage_p->shape()==synImage_p->shape(), AipsError);
  
}

void GridBoth::finalizeToVis()
{
  ok();
  synMachine_p->finalizeToVis();
  sdMachine_p->finalizeToVis();
}


// Initialize the transform to the Sky. Here we have to setup and initialize the
// grid. 
void GridBoth::initializeToSky(ImageInterface<Complex>& iimage,
			       Matrix<Float>& weight, const VisBuffer& vb)
{
  
  ok();
  
  lastMachine_p=0;

  image=&iimage;
  
  Matrix<Float> sdWeights, synWeights;

  if(sdImage_p) delete sdImage_p;
 
  
  sdImage_p=new TempImage<Complex>(iimage.shape(), iimage.coordinates());
  AlwaysAssert(sdImage_p, AipsError);
  sdMachine_p->initializeToSky(*sdImage_p, sdWeights, vb);
  
  if(synImage_p) delete synImage_p;
  synImage_p=new TempImage<Complex>(iimage.shape(), iimage.coordinates());
  AlwaysAssert(synImage_p, AipsError);
  synMachine_p->initializeToSky(*synImage_p, synWeights, vb);

  sumWeight=0.0;
  weight.resize(synWeights.shape());
  weight=0.0;
 


  AlwaysAssert(sdImage_p->shape()==synImage_p->shape(), AipsError);
}

void GridBoth::finalizeToSky()
{
  ok();
  synMachine_p->finalizeToSky();
  sdMachine_p->finalizeToSky();
}

void GridBoth::put(const VisBuffer& vb, Int row, Bool dopsf, 
		   FTMachine::Type type)
{
  synMachine_p->put(vb, row, dopsf, type);
  sdMachine_p->put(vb, row, dopsf, type);
}

void GridBoth::get(VisBuffer& vb, Int row)
{
  synMachine_p->get(vb, row);
  Cube<Complex> synModelVis(vb.modelVisCube().copy());
  sdMachine_p->get(vb, row);
  Cube<Complex> sdModelVis(vb.modelVisCube().copy());
  vb.setModelVisCube(sdModelVis+synModelVis);
  Cube<Complex> totalModelVis(vb.modelVisCube().copy());
}

// Finalize the transform to the Sky. We take the final images and
// add them together, returning the resulting image
ImageInterface<Complex>& GridBoth::getImage(Matrix<Float>& weights,
					    Bool normalize) 
{
  ok();
  AlwaysAssert(image, AipsError);
  AlwaysAssert(synImage_p, AipsError);
  AlwaysAssert(sdImage_p, AipsError);
  AlwaysAssert(sdImage_p->shape()==synImage_p->shape(), AipsError);
  
  logIO() << LogOrigin("GridBoth", "getImage") << LogIO::NORMAL;
  
  Matrix<Float> synWeights, sdWeights;

  sdImage_p->copyData(sdMachine_p->getImage(sdWeights, false));
  synImage_p->copyData(synMachine_p->getImage(synWeights, false));
  Complex scale(sdScale_p*sdWeight_p);
  LatticeExpr<Complex> le(*synImage_p+scale*(*sdImage_p));
  image->copyData(le);

  if(normalize) {
    TempImage<Float> weightImage(image->shape(), image->coordinates());
    getWeightImage(weightImage, weights);
    if(max(weights)==0.0) {
      logIO() << LogIO::WARN << "No useful data in GridBoth: weights all zero"
	      << LogIO::POST;
    }
    image->copyData((LatticeExpr<Complex>)(iif((weightImage<=0.0), 0.0, (*image)/(weightImage))));
  }
  else {
    weights.resize(synWeights.shape());
    weights=synWeights+sdWeight_p*sdWeights;
  }
  
  return *image;
}

void GridBoth::getWeightImage(ImageInterface<Float>& weightImage, Matrix<Float>& weights)
{
  ok();

  logIO() << LogOrigin("GridBoth", "getWeightImage") << LogIO::NORMAL;
  
  AlwaysAssert(image, AipsError);
  AlwaysAssert(synImage_p, AipsError);
  AlwaysAssert(sdImage_p, AipsError);
  AlwaysAssert(sdImage_p->shape()==synImage_p->shape(), AipsError);
  
  logIO() << LogOrigin("GridBoth", "getWeightImage") << LogIO::NORMAL;
  
  TempImage<Float> sdWeightImage(sdImage_p->shape(), sdImage_p->coordinates());
  TempImage<Float> synWeightImage(synImage_p->shape(), synImage_p->coordinates());

  Matrix<Float> synWeights, sdWeights;

  sdMachine_p->getWeightImage(sdWeightImage, sdWeights);
  synMachine_p->getWeightImage(synWeightImage, synWeights);
 
  LatticeExpr<Float> le(synWeightImage+sdWeight_p*sdWeightImage);
  weightImage.copyData(le);
  
  weights.resize(synWeights.shape());
  weights=synWeights+sdWeight_p*sdWeights;
}

// Both these need filling out!
Bool GridBoth::toRecord(String& error, RecordInterface& outRec, 
			Bool withImage, const String diskimage ) {
  ok();
  String im1=""; 
  String im2="";
  if(diskimage != ""){
    im1=diskimage+"_synImage";
    im2=diskimage+"_sdImage";
  }
  return synMachine_p->toRecord(error, outRec, withImage, im1) && 
    sdMachine_p->toRecord(error, outRec, withImage, im2);
}

Bool GridBoth::fromRecord(String& error, const RecordInterface& inRec)
{
  ok();
  return synMachine_p->fromRecord(error, inRec) && 
    sdMachine_p->fromRecord(error, inRec);
}

void GridBoth::ok() {
  AlwaysAssert(synMachine_p, AipsError);
  AlwaysAssert(sdMachine_p, AipsError);
}


} //# NAMESPACE CASA - END

