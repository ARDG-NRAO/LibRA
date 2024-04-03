//# WProjectFT.cc: Implementation of WProjectFT class
//# Copyright (C) 2001-2016
//# Associated Universities, Inc. Washington DC, USA
//#
//# This library is free software; you can redistribute it and/or modify it
//# under the terms of the GNU General Public License as published by
//# the Free Software Foundation; either version 2 of the License, or (at your
//# option) any later version.
//#
//# This library is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
//# License for more details.
//#
//# You should have received a copy of the GNU General Public License
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


#include <casacore/casa/Quanta/UnitMap.h>
#include <casacore/casa/Quanta/MVTime.h>
#include <casacore/casa/Quanta/UnitVal.h>
#include <casacore/casa/Utilities/CountedPtr.h>
#include <casacore/measures/Measures/Stokes.h>
#include <casacore/coordinates/Coordinates/CoordinateSystem.h>
#include <casacore/coordinates/Coordinates/DirectionCoordinate.h>
#include <casacore/coordinates/Coordinates/SpectralCoordinate.h>
#include <casacore/coordinates/Coordinates/StokesCoordinate.h>
#include <casacore/coordinates/Coordinates/Projection.h>
#include <casacore/ms/MeasurementSets/MSColumns.h>
#include <casacore/casa/BasicSL/Constants.h>
#include <casacore/scimath/Mathematics/FFTServer.h>
#include <synthesis/TransformMachines/WProjectFT.h>
#include <synthesis/TransformMachines/WPConvFunc.h>
#include <casacore/scimath/Mathematics/RigidVector.h>
#include <msvis/MSVis/StokesVector.h>
#include <synthesis/TransformMachines/StokesImageUtil.h>
#include <msvis/MSVis/VisBuffer.h>
#include <msvis/MSVis/VisSet.h>
#include <msvis/MSVis/VisibilityIterator.h>
#include <casacore/images/Images/ImageInterface.h>
#include <casacore/images/Images/PagedImage.h>
#include <casacore/casa/Containers/Block.h>
#include <casacore/casa/Containers/Record.h>
#include <casacore/casa/Arrays/ArrayLogical.h>
#include <casacore/casa/Arrays/ArrayMath.h>
#include <casacore/casa/Arrays/Array.h>
#include <casacore/casa/Arrays/MaskedArray.h>
#include <casacore/casa/Arrays/Vector.h>
#include <casacore/casa/Arrays/Slice.h>
#include <casacore/casa/Arrays/Matrix.h>
#include <casacore/casa/Arrays/Cube.h>
#include <casacore/casa/Arrays/MatrixIter.h>
#include <casacore/casa/BasicSL/String.h>
#include <casacore/casa/Utilities/Assert.h>
#include <casacore/casa/Exceptions/Error.h>
#include <iostream>
#include <iomanip>
#include <casacore/lattices/Lattices/ArrayLattice.h>
#include <casacore/lattices/Lattices/SubLattice.h>
#include <casacore/lattices/LRegions/LCBox.h>
#include <casacore/lattices/LEL/LatticeExpr.h>
#include <casacore/lattices/Lattices/LatticeCache.h>
#include <casacore/lattices/LatticeMath/LatticeFFT.h>
#include <casacore/lattices/Lattices/LatticeIterator.h>
#include <casacore/lattices/Lattices/LatticeStepper.h>
#include <casacore/casa/Utilities/CompositeNumber.h>
#include <casacore/casa/OS/Timer.h>
#include <casacore/casa/OS/HostInfo.h>
#include <sstream>
#ifdef _OPENMP
#include <omp.h>
#endif
using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

WProjectFT::WProjectFT( Int nWPlanes, Long icachesize, Int itilesize, 
			Bool usezero, Bool useDoublePrec, const Double minW, const Double maxW, const Double rmsW)
  : FTMachine(), padding_p(1.0), nWPlanes_p(nWPlanes),
    imageCache(0), cachesize(icachesize), tilesize(itilesize),
    gridder(0), isTiled(false), 
    maxAbsData(0.0), centerLoc(IPosition(4,0)), offsetLoc(IPosition(4,0)), usezero_p(usezero), 
    machineName_p("WProjectFT"), timemass_p(0.0), timegrid_p(0.0), timedegrid_p(0.0), minW_p(minW), maxW_p(maxW), rmsW_p(rmsW)
{
  convSize=0;
  tangentSpecified_p=false;
  lastIndex_p=0;
  useDoubleGrid_p=useDoublePrec;
  wpConvFunc_p=new WPConvFunc(minW, maxW, rmsW);

}
WProjectFT::WProjectFT(Int nWPlanes, 
		       MPosition mLocation, 
		       Long icachesize, Int itilesize, 
		       Bool usezero, Float padding, Bool useDoublePrec, const Double minW, const Double maxW, const Double rmsW)
  : FTMachine(), padding_p(padding), nWPlanes_p(nWPlanes),
    imageCache(0), cachesize(icachesize), tilesize(itilesize),
    gridder(0), isTiled(false),  
    maxAbsData(0.0), centerLoc(IPosition(4,0)), offsetLoc(IPosition(4,0)),
    usezero_p(usezero),  
    machineName_p("WProjectFT"), timemass_p(0.0), timegrid_p(0.0), timedegrid_p(0.0), minW_p(minW), maxW_p(maxW), rmsW_p(rmsW)
{
  convSize=0;
  savedWScale_p=0.0;
  tangentSpecified_p=false;
  mLocation_p=mLocation;
  lastIndex_p=0;
  wpConvFunc_p=new WPConvFunc(minW, maxW, rmsW);
  useDoubleGrid_p=useDoublePrec;
}
WProjectFT::WProjectFT(
		       Int nWPlanes, MDirection mTangent, 
		       MPosition mLocation, 
		       Long icachesize, Int itilesize, 
		       Bool usezero, Float padding, Bool useDoublePrec, const Double minW, const Double maxW, const Double rmsW)
  : FTMachine(), padding_p(padding), nWPlanes_p(nWPlanes),
    imageCache(0), cachesize(icachesize), tilesize(itilesize),
    gridder(0), isTiled(false),  
    maxAbsData(0.0), centerLoc(IPosition(4,0)), offsetLoc(IPosition(4,0)),
    usezero_p(usezero), 
    machineName_p("WProjectFT"), timemass_p(0.0), timegrid_p(0.0), timedegrid_p(0.0), minW_p(minW), maxW_p(maxW), rmsW_p(rmsW)
{
  convSize=0;
  savedWScale_p=0.0;
  mTangent_p=mTangent;
  tangentSpecified_p=true;
  mLocation_p=mLocation;
  lastIndex_p=0;
  wpConvFunc_p=new WPConvFunc(minW, maxW, rmsW);
  useDoubleGrid_p=useDoublePrec;
}

WProjectFT::WProjectFT(const RecordInterface& stateRec)
  : FTMachine(),machineName_p("WProjectFT"), timemass_p(0.0), timegrid_p(0.0), timedegrid_p(0.0)
{
  // Construct from the input state record
  String error;
  if (!fromRecord(error, stateRec)) {
    throw (AipsError("Failed to create WProjectFT: " + error));
  };
}
//---------------------------------------------------------------------- 
WProjectFT& WProjectFT::operator=(const WProjectFT& other)
{
  if(this!=&other) {
    //Do the base parameters
    FTMachine::operator=(other);
    
    
    padding_p=other.padding_p;
    nWPlanes_p=other.nWPlanes_p;
    imageCache=other.imageCache;
    cachesize=other.cachesize;
    tilesize=other.tilesize;
    if(other.gridder==0)
      gridder=0;
    else{
      uvScale.resize();
      uvOffset.resize();
      uvScale=other.uvScale;
      uvOffset=other.uvOffset;
      gridder = new ConvolveGridder<Double, Complex>(IPosition(2, nx, ny),
						     uvScale, uvOffset,
						     "SF");
    }

    isTiled=other.isTiled;
    //lattice=other.lattice;
    //arrayLattice=other.arrayLattice;
    lattice=0;
    arrayLattice=0;

    maxAbsData=other.maxAbsData;
    centerLoc=other.centerLoc;
    offsetLoc=other.offsetLoc;
    usezero_p=other.usezero_p;
    machineName_p=other.machineName_p;
    wpConvFunc_p=other.wpConvFunc_p;
    timemass_p=0.0;
    timegrid_p=0.0;
    timedegrid_p=0.0;
    minW_p=other.minW_p;
    maxW_p=other.maxW_p;
    rmsW_p=other.rmsW_p;
  };
  return *this;
};

//----------------------------------------------------------------------
  WProjectFT::WProjectFT(const WProjectFT& other) :FTMachine(), machineName_p("WProjectFT"),
timemass_p(0.0), timegrid_p(0.0), timedegrid_p(0.0)
{
  operator=(other);
}

FTMachine* WProjectFT::cloneFTM(){
  return new WProjectFT(*this);
}

//----------------------------------------------------------------------
void WProjectFT::init() {
  /*  if((padding_p*padding_p*image->shape().product())>cachesize) {
    isTiled=true;
    nx    = image->shape()(0);
    ny    = image->shape()(1);
    npol  = image->shape()(2);
    nchan = image->shape()(3);
  }
  else {*/
    // We are padding.
    isTiled=false;
    CompositeNumber cn(uInt(image->shape()(0)*2));    
    nx    = cn.nextLargerEven(Int(padding_p*Float(image->shape()(0))-0.5));
    ny    = cn.nextLargerEven(Int(padding_p*Float(image->shape()(1))-0.5));
    //cerr << "INIT shape " <<  image->shape()(0) << " " << image->shape()(1) << " new " << nx << " "  << ny << endl;
    npol  = image->shape()(2);
    nchan = image->shape()(3);
    //}
  
  //  if(image->shape().product()>cachesize) {
  //   isTiled=true;
  // }
  // else {
  // isTiled=false;
  // }
  //The Tiled version need some fixing: sof or now
  isTiled=false;

 
  sumWeight.resize(npol, nchan);
  
  wConvSize=max(0, nWPlanes_p);
  convSupport.resize(wConvSize);
  convSupport=0;

  uvScale.resize(3);
  uvScale=0.0;
  uvScale(0)=Float(nx)*image->coordinates().increment()(0); 
  uvScale(1)=Float(ny)*image->coordinates().increment()(1); 
  if(savedWScale_p==0.0){
    uvScale(2)=Float(wConvSize)*abs(image->coordinates().increment()(0));
  }
  else{
    uvScale(2)=savedWScale_p;
  }
  uvOffset.resize(3);
  uvOffset(0)=nx/2;
  uvOffset(1)=ny/2;
  uvOffset(2)=0;
  
  

  if(gridder) delete gridder; gridder=0;
  gridder = new ConvolveGridder<Double, Complex>(IPosition(2, nx, ny),
						 uvScale, uvOffset,
						 "SF");

  // Set up image cache needed for gridding. 
  if(imageCache) delete imageCache; imageCache=0;
  
  // The tile size should be large enough that the
  // extended convolution function can fit easily
  if(isTiled) {
    Float tileOverlap=0.5;
    tilesize=min(256,tilesize);
    IPosition tileShape=IPosition(4,tilesize,tilesize,npol,nchan);
    Vector<Float> tileOverlapVec(4);
    tileOverlapVec=0.0;
    tileOverlapVec(0)=tileOverlap;
    tileOverlapVec(1)=tileOverlap;
    Int tmpCacheVal=static_cast<Int>(cachesize);
    imageCache=new LatticeCache <Complex> (*image, tmpCacheVal, tileShape, 
					   tileOverlapVec,
					   (tileOverlap>0.0));
    
  }
}

// This is nasty, we should use CountedPointers here.
WProjectFT::~WProjectFT() {
  if(imageCache) delete imageCache; imageCache=0;
  if(gridder) delete gridder; gridder=0;
  /*
  if(arrayLattice) delete arrayLattice; arrayLattice=0;
  
  Int numofmodels=convFunctions_p.nelements();
  for (Int k=0; k< numofmodels; ++k){
    delete convFunctions_p[k];
    delete convSupportBlock_p[k];

  }
  */
  // convFuctions_p.resize();
  //  convSupportBlock_p.resize();

}


void WProjectFT::setConvFunc(CountedPtr<WPConvFunc>& pbconvFunc){

  wpConvFunc_p=pbconvFunc;
}
CountedPtr<WPConvFunc>& WProjectFT::getConvFunc(){

  return wpConvFunc_p;
}

void WProjectFT::findConvFunction(const ImageInterface<Complex>& image,
				const VisBuffer& vb) {
  




  wpConvFunc_p->findConvFunction(image, vb, wConvSize, uvScale, uvOffset,
				 padding_p,
				 convSampling, 
				 convFunc, convSize, convSupport, 
				 savedWScale_p); 

  nWPlanes_p=convSupport.nelements();
  wConvSize=max(1,nWPlanes_p);
  uvScale(2)=savedWScale_p;

  
  
}

void WProjectFT::initializeToVis(ImageInterface<Complex>& iimage,
			       const VisBuffer& vb)
{
  image=&iimage;
  toVis_p=true;
  ok();
  
  //   if(convSize==0) {
  init();
  // }
  findConvFunction(*image, vb);

  
  // Initialize the maps for polarization and channel. These maps
  // translate visibility indices into image indices
  initMaps(vb);
  
  //  nx    = image->shape()(0);
  //  ny    = image->shape()(1);
  //  npol  = image->shape()(2);
  //  nchan = image->shape()(3);


  isTiled=false;
  // If we are memory-based then read the image in and create an
  // ArrayLattice otherwise just use the PagedImage
  /*if(isTiled) {
    lattice=CountedPtr<Lattice<Complex> > (image, false);
  }
  else {
   }
  */
  //AlwaysAssert(lattice, AipsError);
  
  prepGridForDegrid();
}

void WProjectFT::prepGridForDegrid(){
  IPosition gridShape(4, nx, ny, npol, nchan);
  griddedData.resize(gridShape);
  griddedData=Complex(0.0);
  

  IPosition stride(4, 1);
  IPosition blc(4, (nx-image->shape()(0)+(nx%2==0))/2,
		(ny-image->shape()(1)+(ny%2==0))/2, 0, 0);
  IPosition trc(blc+image->shape()-stride);
  
  IPosition start(4, 0);
  griddedData(blc, trc) = image->getSlice(start, image->shape());
  
  //if(arrayLattice) delete arrayLattice; arrayLattice=0;
  arrayLattice = new ArrayLattice<Complex>(griddedData);
  lattice=arrayLattice;

  logIO() << LogIO::DEBUGGING << "Starting FFT of image" << LogIO::POST;
  
  
  Vector<Float> sincConvX(nx);
  for (Int ix=0;ix<nx;ix++) {
    Float x=C::pi*Float(ix-nx/2)/(Float(nx)*Float(convSampling));
    if(ix==nx/2) {
      sincConvX(ix)=1.0;
    }
    else {
      sincConvX(ix)=sin(x)/x;
    }
  }
  Vector<Float> sincConvY(ny);
  for (Int ix=0;ix<ny;ix++) {
    Float x=C::pi*Float(ix-ny/2)/(Float(ny)*Float(convSampling));
    if(ix==ny/2) {
      sincConvY(ix)=1.0;
    }
    else {
      sincConvY(ix)=sin(x)/x;
    }
  }
    

  Vector<Complex> correction(nx);
  correction=Complex(1.0, 0.0);
  // Do the Grid-correction
  IPosition cursorShape(4, nx, 1, 1, 1);
  IPosition axisPath(4, 0, 1, 2, 3);
  LatticeStepper lsx(lattice->shape(), cursorShape, axisPath);
  LatticeIterator<Complex> lix(*lattice, lsx);
  for(lix.reset();!lix.atEnd();lix++) {
    Int iy=lix.position()(1);
    gridder->correctX1D(correction,iy);
    for (Int ix=0;ix<nx;ix++) {
      correction(ix)/=(sincConvX(ix)*sincConvY(iy));
    }
    lix.rwVectorCursor()/=correction;
  }
  
  // Now do the FFT2D in place
  LatticeFFT::cfft2d(*lattice);
  
  logIO() << LogIO::DEBUGGING << "Finished FFT" << LogIO::POST;



}


void WProjectFT::finalizeToVis()
{
  
  //cerr <<"Time to degrid " << timedegrid_p << endl;
  timedegrid_p=0.0;
  if(!arrayLattice.null()) arrayLattice=0;
  if(!lattice.null()) lattice=0;
  griddedData.resize();
  if(isTiled) {
    
    logIO() << LogOrigin("WProjectFT", "finalizeToVis")  << LogIO::NORMAL;
    
    AlwaysAssert(imageCache, AipsError);
    AlwaysAssert(image, AipsError);
    ostringstream o;
    imageCache->flush();
    imageCache->showCacheStatistics(o);
    logIO() << o.str() << LogIO::POST;
  }
}


// Initialize the FFT to the Sky. Here we have to setup and initialize the
// grid. 
void WProjectFT::initializeToSky(ImageInterface<Complex>& iimage,
			       Matrix<Float>& weight,
			       const VisBuffer& vb)
{
  // image always points to the image
  image=&iimage;
  toVis_p=false;
  
  //  if(convSize==0) {
  init();
  //  }
  findConvFunction(*image, vb);
  
  
  // Initialize the maps for polarization and channel. These maps
  // translate visibility indices into image indices
  initMaps(vb);
  
  //  nx    = image->shape()(0);
  //  ny    = image->shape()(1);
  //  npol  = image->shape()(2);
  //  nchan = image->shape()(3);

  //  if(image->shape().product()>cachesize) {
  //  isTiled=true;
  // }
  // else {
  //  isTiled=false;
  // }
  isTiled=false;
  sumWeight=0.0;
  weight.resize(sumWeight.shape());
  weight=0.0;
  
  // Initialize for in memory or to disk gridding. lattice will
  // point to the appropriate Lattice, either the ArrayLattice for
  // in memory gridding or to the image for to disk gridding.
  if(isTiled) {
    imageCache->flush();
    image->set(Complex(0.0));
    lattice=CountedPtr<Lattice<Complex> > (image, false);
  }
  else {
    IPosition gridShape(4, nx, ny, npol, nchan);
    if( !useDoubleGrid_p )
      {
	griddedData.resize(gridShape);
	griddedData=Complex(0.0);
	griddedData2.resize();
      }
    else
      {griddedData.resize();
	griddedData2.resize(gridShape);
	griddedData2=DComplex(0.0);
      }
    //if(arrayLattice) delete arrayLattice; arrayLattice=0;
   
  }
  //AlwaysAssert(lattice, AipsError);
  
}

void WProjectFT::finalizeToSky()
{
  

  //cerr <<"Time to massage data " << timemass_p << endl;
  //cerr <<"Time to grid data " << timegrid_p << endl;
  timemass_p=0.0;
  timegrid_p=0.0;
  // Now we flush the cache and report statistics
  // For memory based, we don't write anything out yet.
  if(isTiled) {
    logIO() << LogOrigin("WProjectFT", "finalizeToSky")  << LogIO::NORMAL;
    
    AlwaysAssert(image, AipsError);
    AlwaysAssert(imageCache, AipsError);
    imageCache->flush();
    ostringstream o;
    imageCache->showCacheStatistics(o);
    logIO() << o.str() << LogIO::POST;
  }
}

Array<Complex>* WProjectFT::getDataPointer(const IPosition& centerLoc2D,
					 Bool readonly) {
  Array<Complex>* result;
  // Is tiled: get tiles and set up offsets
  centerLoc(0)=centerLoc2D(0);
  centerLoc(1)=centerLoc2D(1);
  result=&imageCache->tile(offsetLoc, centerLoc, readonly);
  gridder->setOffset(IPosition(2, offsetLoc(0), offsetLoc(1)));
  return result;
}

#define NEED_UNDERSCORES
#if defined(NEED_UNDERSCORES)
#define gwgrid gwgrid_
#define gwproj gwproj_
#define dwproj dwproj_
#define sectgwgridd sectgwgridd_
#define sectgwgrids sectgwgrids_
#define sectdwgrid sectdwgrid_
#define locuvw locuvw_
#endif

extern "C" { 
  void locuvw(const Double*, const Double*, const Double*, const Int*, const Double*, const Double*, const Int*, 
	      Int*, Int*, Complex*, const Int*, const Int*, const Double*);
  //Double precision gridding
  void gwgrid(const Double*,
	      Double*,
	      const Complex*,
	      Int*,
	      Int*,
	      Int*,
	      const Int*,
	      const Int*,
	      const Float*,
	      Int*,
	      Int*,
	      Double*,
	      Double*,
	      DComplex*,
	      Int*,
	      Int*,
	      Int *,
	      Int *,
	      const Double*,
	      const Double*,
	      Int*,
	      Int*,
	      Int*,
	      Int*,
	      const Complex*,
	      Int*,
	      Int*,
	      Double*);

  void sectgwgridd(const Double*,
		   const Complex*,
	      const Int*,
	      const Int*,
	      const Int*,
	      const Int*,
	      const Int*,
	      const Float*,
	      const Int*,
	      DComplex*,
	      const Int*,
	      const Int*,
	      const Int *,
	      const Int *,
		   //support
	      const Int*,
	      const Int*,
	      const Int*,
	      const Int*,
	      const Complex*,
	      const Int*,
	      const Int*,
		   Double*,
		   //x0
		   const Int*,
		   const Int*,
		   const Int*, 
		   const Int*, 
		   const Int*, 
		   const Int*,
		   const Int*,
		   const Int*,
		   const Complex*
		   );

  //Single precision gridding
    void sectgwgrids(const Double*,
		   const Complex*,
	      const Int*,
	      const Int*,
	      const Int*,
	      const Int*,
	      const Int*,
	      const Float*,
	      const Int*,
	      Complex*,
	      const Int*,
	      const Int*,
	      const Int *,
	      const Int *,
		   //support
	      const Int*,
	      const Int*,
	      const Int*,
	      const Int*,
	      const Complex*,
	      const Int*,
	      const Int*,
		   Double*,
		   //x0
		   const Int*,
		   const Int*,
		   const Int*, 
		   const Int*, 
		   const Int*, 
		   const Int*,
		   const Int*,
		   const Int*,
		   const Complex*
		   );


  void gwproj(const Double*,
	      Double*,
	      const Complex*,
	      Int*,
	      Int*,
	      Int*,
	      const Int*,
	      const Int*,
	      const Float*,
	      Int*,
	      Int*,
	      Double*,
	      Double*,
	      Complex*,
	      Int*,
	      Int*,
	      Int *,
	      Int *,
	      const Double*,
	      const Double*,
	      Int*,
	      Int*,
	      Int*,
	      Int*,
	      const Complex*,
	      Int*,
	      Int*,
	      Double*);

  void sectdwgrid(const Double*,
		  Complex*,
		  const Int*,
		  const Int*,
		  const Int*,
		  const Int*,
		  const Int*,
		  const Complex*,
		  const Int*,
		  const Int*,
		  const Int *,
		  const Int *,
		  //support
		  const Int*,
		  const Int*,
		  const Int*,
		  const Int*,
		  const Complex*,
		  const Int*,
		  const Int*,
		  //rbeg, rend, loc, off, phasor
		  const Int*,
		  const Int*,
		  const Int*,
		  const Int*,
		  const Complex*);
  void dwproj(const Double*,
	      Double*,
	      Complex*,
	      Int*,
	      Int*,
	      const Int*,
	      const Int*,
	      Int*,
	      Int*,
	      Double*,
	      Double*,
	      const Complex*,
	      Int*,
	      Int*,
	      Int *,
	      Int *,
	      const Double*,
	      const Double*,
	      Int*,
	      Int*,
	      Int*,
	      Int*,
	      const Complex*,
	      Int*,
	      Int*);
}
void WProjectFT::put(const VisBuffer& vb, Int row, Bool dopsf,
		     FTMachine::Type type)
{
  

   //Check if ms has changed then cache new spw and chan selection
  if(vb.newMS())
    matchAllSpwChans(vb);
  
  //Here we redo the match or use previous match
  
  //Channel matching for the actual spectral window of buffer
  if(doConversion_p[vb.spectralWindow()]){
    matchChannel(vb.spectralWindow(), vb);
  }
  else{
    chanMap.resize();
    chanMap=multiChanMap_p[vb.spectralWindow()];
  }
  
  //No point in reading data if its not matching in frequency
  if(max(chanMap)==-1)
    return;
   Timer tim;
   tim.mark();

  const Matrix<Float> *imagingweight;
  imagingweight=&(vb.imagingWeight());

  if(dopsf) type=FTMachine::PSF;

  Cube<Complex> data;
  //Fortran gridder need the flag as ints 
  Cube<Int> flags;
  Matrix<Float> elWeight;
  interpolateFrequencyTogrid(vb, *imagingweight,data, flags, elWeight, type);
  
  
  Bool iswgtCopy;
  const Float *wgtStorage;
  wgtStorage=elWeight.getStorage(iswgtCopy);


  Bool isCopy;
  const Complex *datStorage=0;

  if(!dopsf)
    datStorage=data.getStorage(isCopy);


  // If row is -1 then we pass through all rows
  Int startRow, endRow, nRow;
  if (row==-1) {
    nRow=vb.nRow();
    startRow=0;
    endRow=nRow-1;
  } else {
    nRow=1;
    startRow=row;
    endRow=row;
  }
  
  // Get the uvws in a form that Fortran can use and do that
  // necessary phase rotation. On a Pentium Pro 200 MHz
  // when null, this step takes about 50us per uvw point. This
  // is just barely noticeable for Stokes I continuum and
  // irrelevant for other cases.
  Matrix<Double> uvw(3, vb.uvw().nelements());
  uvw=0.0;
  Vector<Double> dphase(vb.uvw().nelements());
  dphase=0.0;
  //NEGATING to correct for an image inversion problem
  for (Int i=startRow;i<=endRow;i++) {
    for (Int idim=0;idim<2;idim++) uvw(idim,i)=-vb.uvw()(i)(idim);
    uvw(2,i)=vb.uvw()(i)(2);
  }
  
  rotateUVW(uvw, dphase, vb);
  refocus(uvw, vb.antenna1(), vb.antenna2(), dphase, vb);


  
  // Take care of translation of Bools to Integer
  Int idopsf=0;
  if(dopsf) idopsf=1;
  
  Vector<Int> rowFlags(vb.nRow());
  rowFlags=0;
  rowFlags(vb.flagRow())=true;
  if(!usezero_p) {
    for (Int rownr=startRow; rownr<=endRow; rownr++) {
      if(vb.antenna1()(rownr)==vb.antenna2()(rownr)) rowFlags(rownr)=1;
    }
  }
  
  
  Bool del;
  //    IPosition s(flags.shape());
  Vector<Int> s(flags.shape().nelements());
  convertArray(s, flags.shape().asVector());
  Int nvp=s[0];
  Int nvc=s[1];
  Int nvisrow=s[2];

  Int csamp=convSampling;
  
  Bool uvwcopy; 
  const Double *uvwstor=uvw.getStorage(uvwcopy);
  Bool gridcopy;
  Bool convcopy;
  const Complex *convstor=convFunc.getStorage(convcopy);
  Cube<Int> loc(3, nvc, nRow);
  Cube<Int> off(3, nvc, nRow);
  Matrix<Complex> phasor(nvc, nRow);
  Bool delphase;
  Complex * phasorstor=phasor.getStorage(delphase);
  const Double * visfreqstor=interpVisFreq_p.getStorage(del);
  const Double * scalestor=uvScale.getStorage(del);
  const Double * offsetstor=uvOffset.getStorage(del);
  Bool delloc, deloff;
  Int * locstor=loc.getStorage(delloc);
  Int * offstor=off.getStorage(deloff);
  const Double *dpstor=dphase.getStorage(del);
  Double cinv=Double(1.0)/C::c;
  Int irow;
  Int dow=1;
  Int nth=1;
#ifdef _OPENMP
  if(numthreads_p >0){
    nth=min(numthreads_p, omp_get_max_threads());
  }
  else{   
    nth= omp_get_max_threads();
  }
  //nth=min(4,nth);
#endif




   #pragma omp parallel default(none) private(irow) firstprivate(visfreqstor, nvc, scalestor, offsetstor, csamp, phasorstor, uvwstor, locstor, offstor, dpstor, cinv, dow) shared(startRow, endRow) num_threads(nth)  
{
#pragma omp for schedule(dynamic)
  for (irow=startRow; irow<=endRow;irow++){
    //locateuvw(uvwstor,dpstor, visfreqstor, nvc, scalestor, offsetstor, csamp, 
    //	      locstor, 
    //	      offstor, phasorstor, irow, true);
    locuvw(uvwstor, dpstor, visfreqstor, &nvc, scalestor, offsetstor, &csamp, locstor, offstor, phasorstor, &irow, &dow, &cinv);
  }  

  


 }//end pragma parallel
  //Int maxx=0; 
  Int minx=1000000;
  //Int maxy=0;
  Int miny=1000000;
  //Int maxsupp=max(convSupport);
  /*loc.putStorage(locstor, delloc);
  maxx=max(loc.yzPlane(0))+convSize;
  maxx=min(nx-1,  maxx-1);
  minx=min(loc.yzPlane(0))-convSize;
  minx=max(0, minx-1);
  maxy=max(loc.yzPlane(1))+convSize;
  maxy=min(ny-1, maxy-1);
  miny=min(loc.yzPlane(1))-convSize;
  miny=max(0,miny-1);
  locstor=loc.getStorage(delloc);
  */
  //cerr << "dels " << delloc << "  " << deloff << endl;
  //cerr << "LOC " << min(loc.xzPlane(0)) << "  max " << loc.shape() << endl;
  timemass_p +=tim.real();
  Int ixsub, iysub, icounter;
  ixsub=1;
  iysub=1;
  if(nth > 4){
    ixsub=8;
    iysub=8;
  }
  else{
    ixsub=2;
    iysub=2; 
  }
  /* else if(nth >1){
     ixsub=2;
     iysub=1; 
  }
  */

  //nxsub=nx;
  //nysub=ny;
  //nxsub=maxx-minx+1;
  //nysub=maxy-miny+1;
  Int rbeg=startRow+1;
  Int rend=endRow+1;
  const Int* pmapstor=polMap.getStorage(del);
  const Int *cmapstor=chanMap.getStorage(del);
  Int nc=nchan;
  Int np=npol;
  ////For now no limitation
  minx=0;
  miny=0; 
  Int nxp=nx;
  Int nyp=ny;
 
  
  Int nxcopy=nx;
  Int nycopy=ny;
  
  Int csize=convSize;
   Int wcsize=wConvSize;
  const Int * flagstor=flags.getStorage(del);
  const Int * rowflagstor=rowFlags.getStorage(del);
  const Int * suppstor=convSupport.getStorage(del);
  //for (Int biba=4; biba < 60; biba+=2){
  // cerr << "biba 0 " << biba << endl;
  //  ixsub=biba;
  //  iysub=biba;
 
  tim.mark(); 
  //clock_t  tick;
  //tick = clock();
  //time_t now;
  //time(&now);
  
  Vector<Int> x0p, y0p, nxpsub, nypsub;

  Block<Matrix<Double> > sumwgt(ixsub*iysub);
  //  if(timegrid_p==0.0 && dopsf){
  {
    xsect_p.resize(ixsub*iysub);
    ysect_p.resize(ixsub*iysub);
    nxsect_p.resize(ixsub*iysub);
    nysect_p.resize(ixsub*iysub);
    for (icounter=0; icounter < ixsub*iysub; ++icounter){
      ///testoo
      //minx=0;
      //miny=0;
      //nxp=nx;
      //nyp=ny;
      ////////
      findGridSector(nxp, nyp, ixsub, iysub, minx, miny, icounter, xsect_p(icounter), ysect_p(icounter), nxsect_p(icounter), nysect_p(icounter), true);
    }
  }
  /*if((Int(timegrid_p)%10000)==0){
    cerr << timegrid_p << endl;
    cerr << "xsect " << xsect_p << "\n ysect " << ysect_p << "\n nxsect "<< nxsect_p << "\n nysect " << nysect_p << endl;  

  }
  */
  for (icounter=0; icounter < ixsub*iysub; ++icounter){
    sumwgt[icounter].resize(sumWeight.shape());
    sumwgt[icounter].set(0.0);
  }
  Vector<Int> xsect, ysect, nxsect, nysect;
  xsect=xsect_p; ysect=ysect_p; nxsect=nxsect_p; nysect=nysect_p;
  //cerr << "useDoubleGrid " << useDoubleGrid_p << endl;
  if(!useDoubleGrid_p){
    Complex *gridstor=griddedData.getStorage(gridcopy);
#pragma omp parallel default(none) private(icounter, del) firstprivate(idopsf, uvwstor, datStorage, wgtStorage, flagstor, rowflagstor, convstor, pmapstor, cmapstor, gridstor, suppstor, nxp, nyp, nxcopy, nycopy, np, nc,ixsub, iysub, rend, rbeg, csamp, csize, wcsize, nvp, nvc, nvisrow, phasorstor, locstor, offstor, minx, miny, xsect, ysect, nxsect, nysect) shared(sumwgt) num_threads(nth)
    {
#pragma omp for schedule(dynamic)      
    for(icounter=0; icounter < ixsub*iysub; ++icounter){
      //Int realicounter=icounter%2==0 ? ixsub*iysub/2+icounter/2 :  ixsub*iysub/2-icounter/2-1;
      //findGridSector(nxp, nyp, ixsub, iysub, minx, miny, icounter, x0, y0, nxsub, nysub, true);
      Int x0=xsect(icounter);
      Int y0=ysect(icounter);
      Int nxsub=nxsect(icounter);
      Int nysub=nysect(icounter);

      sectgwgrids(uvwstor,
	   datStorage,
	   &nvp,
	   &nvc,
	   &idopsf,
	   flagstor,
	   rowflagstor,
	   wgtStorage,
	   &nvisrow,
	   gridstor,
	   &nxcopy,
	   &nycopy,
	   &np,
	   &nc,
	   suppstor,
	   &csize,
	   &csamp,
	   &wcsize,
	   convstor,
	   cmapstor,
	   pmapstor,
		  (sumwgt[icounter]).getStorage(del), 
		  &x0, &y0, &nxsub, &nysub, &rbeg, &rend, locstor, offstor,
		 phasorstor);
    }
    }//end pragma parallel

    timegrid_p+=tim.real();
    //if(dopsf)
    //  tweakGridSector(nx, ny, ixsub, iysub, xsect_p, ysect_p, nxsect_p, nysect_p);
    for (icounter=0; icounter < ixsub*iysub; ++icounter){
      //cerr << "ixsub, iysub " << icounter%ixsub << "  " << icounter/ixsub << " sumwgt " << sumwgt[icounter](0,0) << endl;
      sumWeight=sumWeight+sumwgt[icounter];
    }    
    griddedData.putStorage(gridstor, gridcopy);
    
  }
  else{
    DComplex *gridstor=griddedData2.getStorage(gridcopy);
#pragma omp parallel default(none) private(icounter, del) firstprivate(idopsf, uvwstor, datStorage, wgtStorage, flagstor, rowflagstor, convstor, pmapstor, cmapstor, gridstor, suppstor, nxp, nyp, nxcopy, nycopy, np, nc,ixsub, iysub, rend, rbeg, csamp, csize, wcsize, nvp, nvc, nvisrow, phasorstor, locstor, offstor,minx,miny, xsect, ysect, nxsect, nysect) shared(sumwgt) num_threads(nth)
    {
#pragma omp for schedule(dynamic)    
    for(icounter=0; icounter < ixsub*iysub; ++icounter){
      //Int realicounter=icounter%2==0 ? ixsub*iysub/2+icounter/2 :  ixsub*iysub/2-icounter/2-1;
      // findGridSector(nxp, nyp, ixsub, iysub, minx, miny, icounter, x0, y0, nxsub, nysub, true);
      //Double wt0=omp_get_wtime();
      Int x0=xsect(icounter);
      Int y0=ysect(icounter);
      Int nxsub=nxsect(icounter);
      Int nysub=nysect(icounter);
      
       sectgwgridd(uvwstor,
	   datStorage,
	   &nvp,
	   &nvc,
	   &idopsf,
	   flagstor,
	   rowflagstor,
	   wgtStorage,
	   &nvisrow,
	   gridstor,
	   &nxcopy,
	   &nycopy,
	   &np,
	   &nc,
	   suppstor,
	   &csize,
	   &csamp,
	   &wcsize,
	   convstor,
	   cmapstor,
	   pmapstor,
		  (sumwgt[icounter]).getStorage(del), 
		  &x0, &y0, &nxsub, &nysub, &rbeg, &rend, locstor, offstor,
		 phasorstor);
       //cerr << "icounter " << icounter << "  " << icounter%ixsub << "  " << icounter/ixsub << "    " << omp_get_wtime()-wt0 << endl;
    }
    }//end pragma parallel
    timegrid_p+=1.0;
     //timegrid_p+=tim.real();
     //if(dopsf)
     //  tweakGridSector(nx, ny, ixsub, iysub, xsect_p, ysect_p, nxsect_p, nysect_p);
    for (icounter=0; icounter < ixsub*iysub; ++icounter){
      //cerr << "ixsub, iysub " << icounter%ixsub << "  " << icounter/ixsub << " sumwgt " << sumwgt[icounter](0,0) << endl;
      sumWeight=sumWeight+sumwgt[icounter];
    }
    griddedData2.putStorage(gridstor, gridcopy);
  }
 
  
  
  //   std::cerr << std::setprecision(10) << "biba " << biba << " time " << tim.real() << " timall " << tim.all_usec() << endl;
  //  }
  uvw.freeStorage(uvwstor, uvwcopy);
  convFunc.freeStorage(convstor, convcopy);
  
  if(!dopsf)
    data.freeStorage(datStorage, isCopy);
  elWeight.freeStorage(wgtStorage,iswgtCopy);
  //timegrid_p+=tim.real();
  //timegrid_p+= Double(clock()-tick);
  // time_t now1;
  //time(&now1);
  //timegrid_p+=difftime(now1, now);
  }

void WProjectFT::findGridSector(const Int& nxp, const Int& nyp, const Int& ixsub, const Int& iysub, const Int& minx, const Int& miny, const Int& icounter, Int& x0, Int& y0, Int&  nxsub, Int& nysub, const Bool linear){
  /* Vector<Int> ord(36);
       ord(0)=14; 
      ord(1)=15;
      ord(2)=20;
      ord(3)=21;ord(4)=13;
      ord(5)=16;ord(6)=19;ord(7)=22;ord(8)=8;ord(9)=9;
      ord(10)=26;ord(11)=27;ord(12)=25;ord(13)=28;ord(14)=7;
      ord(15)=10;ord(16)=32;ord(17)=33;ord(18)=2;ord(19)=3;
      ord(20)=18;ord(21)=23;ord(22)=12;ord(23)=17;ord(24)=1;
      ord(25)=4;ord(26)=6;ord(27)=11;ord(28)=24;ord(29)=29;
      ord(30)=31;ord(31)=34;ord(32)=0;ord(33)=5;ord(34)=30;
      ord(35)=35;
      */
  /*
      Int ix= (icounter+1)-((icounter)/ixsub)*ixsub;
      Int iy=(icounter)/ixsub+1;
      y0=(nyp/iysub)*(iy-1)+1+miny;
      nysub=nyp/iysub;
      if( iy == iysub) {
	nysub=nyp-(nyp/iysub)*(iy-1);
      }
      x0=(nxp/ixsub)*(ix-1)+1+minx;
      nxsub=nxp/ixsub;
      if( ix == ixsub){
	nxsub=nxp-(nxp/ixsub)*(ix-1);
      }
  */
         
         
      {
	Int elrow=icounter/ixsub;
	Int elcol=(icounter-elrow*ixsub);
	//cerr << "row "<< elrow << " col " << elcol; 
	//nxsub=Int(floor(((ceil(fabs(float(2*elcol+1-ixsub)/2.0))-1.0)*5 +1)*nxp/36.0 + 0.5));
	Float factor=0;
	for (Int k=0; k < ixsub/2; ++k)
	  factor= linear ? factor+(k+1): factor+sqrt(Float(k+1));
	  //factor= linear ? factor+(k+1): factor+(k+1)*(k+1)*(k+1);
	factor *= 2.0;
	if(linear)
	  nxsub=Int(floor((ceil(fabs(float(2*elcol+1-ixsub)/2.0))/factor)*nxp + 0.5));
	else
	  //nxsub=Int(floor((ceil(fabs(float(2*elcol+1-ixsub)/2.0))*ceil(fabs(float(2*elcol+1-ixsub)/2.0))*ceil(fabs(float(2*elcol+1-ixsub)/2.0))/factor)*nxp + 0.5));
	  nxsub=Int(floor((sqrt(ceil(fabs(float(2*elcol+1-ixsub)/2.0)))/factor)*nxp + 0.5));
        //cerr << nxp << " col " << elcol << " nxsub " << nxsub << endl;
	x0=minx;
	elcol-=1;
	while(elcol >= 0){
	  //x0+=Int(floor(((ceil(fabs(float(2*elcol+1-ixsub)/2.0))-1.0)*5 +1)*nxp/36.0+0.5));

	  if(linear)
	    x0+=Int(floor((ceil(fabs(float(2*elcol+1-ixsub)/2.0))/factor)*nxp + 0.5));
	  else
	    //x0+=Int(floor((ceil(fabs(float(2*elcol+1-ixsub)/2.0))*ceil(fabs(float(2*elcol+1-ixsub)/2.0))*ceil(fabs(float(2*elcol+1-ixsub)/2.0))/factor)*nxp + 0.5));
	    x0+=Int(floor((sqrt(ceil(fabs(float(2*elcol+1-ixsub)/2.0)))/factor)*nxp + 0.5));
	  elcol-=1;
	}
	factor=0;
	for (Int k=0; k < iysub/2; ++k)
	  //factor=linear ? factor+(k+1): factor+(k+1)*(k+1)*(k+1);
	  factor= linear ? factor+(k+1): factor+sqrt(Float(k+1));
	factor *= 2.0;
	//nysub=Int(floor(((ceil(fabs(float(2*elrow+1-iysub)/2.0))-1.0)*5 +1)*nyp/36.0+0.5));
	if(linear)
	  nysub=Int(floor((ceil(fabs(float(2*elrow+1-iysub)/2.0))/factor)*nyp + 0.5));
	else
	  nysub=Int(floor((sqrt(ceil(fabs(float(2*elrow+1-iysub)/2.0)))/factor)*nyp + 0.5));
	  //nysub=Int(floor((ceil(fabs(float(2*elrow+1-iysub)/2.0))*ceil(fabs(float(2*elrow+1-iysub)/2.0))*ceil(fabs(float(2*elrow+1-iysub)/2.0))/factor)*nyp + 0.5));
	y0=miny;
	elrow-=1;
	
	while(elrow >=0){
	  //y0+=Int(floor(((ceil(fabs(float(2*elrow+1-iysub)/2.0))-1.0)*5 +1)*nyp/36.0+0.5));
	  if(linear)
	    y0+=Int(floor((ceil(fabs(float(2*elrow+1-iysub)/2.0))/factor)*nyp + 0.5));
	  else
	    y0+=Int(floor((sqrt(ceil(fabs(float(2*elrow+1-iysub)/2.0)))/factor)*nyp + 0.5));
	    //y0+=Int(floor((ceil(fabs(float(2*elrow+1-iysub)/2.0))*ceil(fabs(float(2*elrow+1-iysub)/2.0))*ceil(fabs(float(2*elrow+1-iysub)/2.0))/factor)*nyp + 0.5));
	  elrow-=1;
	}
      }


      y0+=1;
      x0+=1;
      
   
}

void WProjectFT::tweakGridSector(const Int& nx, const Int& ny, const Int& ixsub, const Int& iysub, Vector<Int>& xsect, Vector<Int>& ysect, Vector<Int>&  nxsect, Vector<Int>& nysect){
  Vector<Int> x0, y0, nxsub, nysub;
  Vector<Float> xcut(nx/2);
  Vector<Float> ycut(ny/2);
  if(griddedData2.nelements() >0 ){
    //cerr << "shapes " << xcut.shape() << "   gd " << amplitude(griddedData2(IPosition(4, 0, ny/2-1, 0, 0), IPosition(4, nx/2-1, ny/2-1, 0,0))).shape() << endl;
    convertArray(xcut, amplitude(griddedData2(IPosition(4, 0, ny/2-1, 0, 0), IPosition(4, nx/2-1, ny/2-1, 0,0))).reform(xcut.shape()));
    convertArray(ycut, amplitude(griddedData2(IPosition(4, nx/2-1, 0, 0, 0), IPosition(4, nx/2-1, ny/2-1, 0,0))).reform(ycut.shape()));
  }
  else{
    xcut=amplitude(griddedData(IPosition(4, 0, ny/2-1, 0, 0), IPosition(4, nx/2-1, ny/2-1, 0,0)));
    ycut=amplitude(griddedData(IPosition(4, nx/2-1, 0, 0, 0), IPosition(4, nx/2-1, ny/2-1, 0,0)));
  }
  //cerr << griddedData2.shape() << "   " << griddedData.shape() << endl;
  Vector<Float> cumSumX(nx/2, 0);
  //Vector<Float> cumSumX2(nx/2,0);
  cumSumX(0)=xcut(0);
  //cumSumX2(0)=cumSumX(0)*cumSumX(0);
  Float sumX=sum(xcut);
  if(sumX==0.0)
    return;
  //cerr << "sumX " << sumX << endl;
  //sumX *= sumX;
  x0.resize(ixsub);
  x0=nx/2-1;
  nxsub.resize(ixsub);
  nxsub=0;
  x0(0)=0;
  Int counter=1;
  for (Int k=1; k < nx/2; ++k){
    cumSumX(k)=cumSumX(k-1)+xcut(k);
    //cumSumX2(k)=cumSumX(k)*cumSumX(k);
    Float nextEdge=sumX/(Float(ixsub/2)*Float(ixsub/2))*Float(counter)*Float(counter);
    if(cumSumX(k-1) < nextEdge && cumSumX(k) >= nextEdge){
      x0(counter)=k;
      //cerr << counter << "    "   << k << " diff " << x0(counter)-x0(counter-1) << endl;
      nxsub(counter-1)=x0(counter)-x0(counter-1);
      ++counter;
    }
  } 
  
  x0(ixsub/2)=nx/2-1;
  nxsub(ixsub/2)=nxsub(ixsub/2-1)=nx/2-1-x0(ixsub/2-1);
  for(Int k=ixsub/2+1; k < ixsub; ++k){
    x0(k)=x0(k-1)+ nxsub(ixsub-k);
    nxsub(k)=nxsub(ixsub-k-1);
  }
  nxsub(ixsub-1)+=1;
  
  Vector<Float> cumSumY(ny/2, 0);
  //Vector<Float> cumSumY2(ny/2,0);
  cumSumY(0)=ycut(0);
  //cumSumY2(0)=cumSumY(0)*cumSumY(0);
  Float sumY=sum(ycut);
  if(sumY==0.0)
    return;
  //sumY *=sumY;
  y0.resize(iysub);
  y0=ny/2-1;
  nysub.resize(iysub);
  nysub=0;
  y0(0)=0;
  counter=1;
  for (Int k=1; k < ny/2; ++k){
    cumSumY(k)=cumSumY(k-1)+ycut(k);
    //cumSumY2(k)=cumSumY(k)*cumSumY(k);
    Float nextEdge=sumY/(Float(iysub/2)*Float(iysub/2))*Float(counter)*Float(counter);
    if(cumSumY(k-1) < nextEdge && cumSumY(k) >= nextEdge){
      y0(counter)=k;
      nysub(counter-1)=y0(counter)-y0(counter-1);
      ++counter;
    }
  } 
 
  y0(ixsub/2)=ny/2-1;
  nysub(iysub/2)=nysub(iysub/2-1)=ny/2-1-y0(iysub/2-1);
  for(Int k=iysub/2+1; k < iysub; ++k){
    y0(k)=y0(k-1)+ nysub(iysub-k);
    nysub(k)=nysub(iysub-k-1);
  }
  nysub(iysub-1)+=1;


  //cerr << " x0 " << x0 << "  nxsub " << nxsub << endl;
  //cerr << " y0 " << y0 << "  nysub " << nysub << endl;
  x0+=1;
  y0+=1;
  xsect.resize(ixsub*iysub);
  ysect.resize(ixsub*iysub);
  nxsect.resize(ixsub*iysub);
  nysect.resize(ixsub*iysub);
  for (Int iy=0; iy < iysub; ++iy){
    for (Int ix=0; ix< ixsub; ++ix){
      
      xsect(iy*ixsub+ix)=x0[ix];
      ysect(iy*ixsub+ix)=y0[iy];
      nxsect(iy*ixsub+ix)=nxsub[ix];
      nysect(iy*ixsub+ix)=nysub[iy];
    }
  }



}


void WProjectFT::get(VisBuffer& vb, Int row)
{
  
  ///This is needed here as virtual model column may not call initializeToVis
  findConvFunction(*image, vb);
  // If row is -1 then we pass through all rows
  Int startRow, endRow, nRow;
  if (row==-1) {
    nRow=vb.nRow();
    startRow=0;
    endRow=nRow-1;
    //vb.modelVisCube()=Complex(0.0,0.0);
  } else {
    nRow=1;
    startRow=row;
    endRow=row;
    //vb.modelVisCube().xyPlane(row)=Complex(0.0,0.0);
  }
  
  // Get the uvws in a form that Fortran can use
  Matrix<Double> uvw(3, vb.uvw().nelements());
  uvw=0.0;
  Vector<Double> dphase(vb.uvw().nelements());
  dphase=0.0;
  //NEGATING to correct for an image inversion problem
  for (Int i=startRow;i<=endRow;i++) {
    for (Int idim=0;idim<2;idim++) uvw(idim,i)=-vb.uvw()(i)(idim);
    uvw(2,i)=vb.uvw()(i)(2);
  }
  
  rotateUVW(uvw, dphase, vb);
  refocus(uvw, vb.antenna1(), vb.antenna2(), dphase, vb);

  // This is the convention for dphase
  // dphase*=-1.0;
 
  
  //Check if ms has changed then cache new spw and chan selection
  if(vb.newMS())
    matchAllSpwChans(vb);
  //Here we redo the match or use previous match
  
  //Channel matching for the actual spectral window of buffer
  if(doConversion_p[vb.spectralWindow()]){
    matchChannel(vb.spectralWindow(), vb);
  }
  else{
    chanMap.resize();
    chanMap=multiChanMap_p[vb.spectralWindow()];
  }
  
  //No point in reading data if its not matching in frequency
  if(max(chanMap)==-1)
    return;

  Cube<Complex> data;
  Cube<Int> flags;
  getInterpolateArrays(vb, data, flags);


  
  Complex *datStorage;
  Bool isCopy;
  datStorage=data.getStorage(isCopy);

  Vector<Int> rowFlags(vb.nRow());
  rowFlags=0;
  rowFlags(vb.flagRow())=true;
  if(!usezero_p) {
    for (Int rownr=startRow; rownr<=endRow; rownr++) {
      if(vb.antenna1()(rownr)==vb.antenna2()(rownr)) rowFlags(rownr)=1;
    }
  }
  
  Int nvp=data.shape()(0);
  Int nvc=data.shape()(1);
  Int nvisrow=data.shape()(2);
  Int nc=nchan;
  Int np=npol;
  Int nxp=nx;
  Int nyp=ny;
  Cube<Int> loc(3, nvc, nvisrow);
  Cube<Int> off(3, nvc, nRow);
  Int csamp=convSampling;
  Int csize=convSize;
  Int wcsize=wConvSize;
  Matrix<Complex> phasor(nvc, nRow);
  Bool delphase;
  Bool del;
  Complex * phasorstor=phasor.getStorage(delphase);
  const Double * visfreqstor=interpVisFreq_p.getStorage(del);
  const Double * scalestor=uvScale.getStorage(del);
  const Double * offsetstor=uvOffset.getStorage(del);
  Int * locstor=loc.getStorage(del);
  Int * offstor=off.getStorage(del);
  const Int * flagstor=flags.getStorage(del);
  const Int * rowflagstor=rowFlags.getStorage(del);
  const Double *dpstor=dphase.getStorage(del);
  Bool uvwcopy; 
  const Double *uvwstor=uvw.getStorage(uvwcopy);
  Bool gridcopy;
  const Complex *gridstor=griddedData.getStorage(gridcopy);
  Bool convcopy;
  const Complex *convstor=convFunc.getStorage(convcopy);
  const Int* pmapstor=polMap.getStorage(del);
  const Int *cmapstor=chanMap.getStorage(del);
  const Int * suppstor=convSupport.getStorage(del);
  Int irow;
  Int nth=1;
#ifdef _OPENMP
  if(numthreads_p >0){
    nth=min(numthreads_p, omp_get_max_threads());
  }
  else{   
    nth= omp_get_max_threads();
  }
  // nth=min(4,nth);
#endif
  Int dow=1;
  Double cinv=Double(1.0)/C::c;

    #pragma omp parallel default(none) private(irow) firstprivate(visfreqstor, nvc, scalestor, offsetstor, csamp, phasorstor, uvwstor, locstor, offstor, dpstor, dow, cinv) shared(startRow, endRow) num_threads(nth) 
  {
#pragma omp for schedule(dynamic)
    for (irow=startRow; irow<=endRow; ++irow){
      /*locateuvw(uvwstor,dpstor, visfreqstor, nvc, scalestor, offsetstor, csamp, 
		locstor, 
		offstor, phasorstor, irow, true);*/
      locuvw(uvwstor, dpstor, visfreqstor, &nvc, scalestor, offsetstor, &csamp, locstor, offstor, phasorstor, &irow, &dow, &cinv);
  }  

  }//end pragma parallel
  Int rbeg=startRow+1;
  Int rend=endRow+1;
  Int npart=nth*2;
  
 Timer tim;
  tim.mark();
  
  Int ix=0;
  #pragma omp parallel default(none) private(ix, rbeg, rend) firstprivate(uvwstor, datStorage, flagstor, rowflagstor, convstor, pmapstor, cmapstor, gridstor, nxp, nyp, np, nc, suppstor, csamp, csize, wcsize, nvp, nvc, nvisrow, phasorstor, locstor, offstor) shared(npart) num_threads(nth)
  {
#pragma omp for schedule(dynamic,1) 
    for (ix=0; ix< npart; ++ix){
      rbeg=ix*(nvisrow/npart)+1;
      rend=(ix != (npart-1)) ? (rbeg+(nvisrow/npart)-1) : (rbeg+(nvisrow/npart)+nvisrow%npart-1) ;
      // cerr << "rbeg " << rbeg << " rend " << rend << " nRow " << nvisrow << endl;
      sectdwgrid(uvwstor,
		 datStorage,
		 &nvp,
		 &nvc,
		 flagstor,
		 rowflagstor,
		 &nvisrow,
		 gridstor,
		 &nxp,
		 &nyp,
		 &np,
		 &nc,
		 suppstor,
		 &csize,
		 &csamp,
		 &wcsize,
		 convstor,
		 cmapstor,
		 pmapstor,
		 &rbeg, &rend, locstor, offstor, phasorstor);
    }

  }//end pragma parallel
  data.putStorage(datStorage, isCopy);
  uvw.freeStorage(uvwstor, uvwcopy);
  griddedData.freeStorage(gridstor, gridcopy);
  convFunc.freeStorage(convstor, convcopy);
   timedegrid_p+=tim.real();

  interpolateFrequencyFromgrid(vb, data, FTMachine::MODEL);
}



// Finalize the FFT to the Sky. Here we actually do the FFT and
// return the resulting image
ImageInterface<Complex>& WProjectFT::getImage(Matrix<Float>& weights,
					      Bool normalize) 
{
  //AlwaysAssert(lattice, AipsError);
  AlwaysAssert(image, AipsError);
  
  if((griddedData.nelements() ==0) && (griddedData2.nelements()==0)){
     throw(AipsError("Programmer error ...request for image without right order of calls"));
  }
  logIO() << LogOrigin("WProjectFT", "getImage") << LogIO::NORMAL;
  
  weights.resize(sumWeight.shape());
  
  convertArray(weights, sumWeight);
  
  // If the weights are all zero then we cannot normalize
  // otherwise we don't care.
  if(max(weights)==0.0) {
    if(normalize) {
      logIO() << LogIO::SEVERE
	      << "No useful data in WProjectFT: weights all zero"
	      << LogIO::POST;
    }
    else {
      logIO() << LogIO::WARN
	      << "No useful data in WProjectFT: weights all zero"
	      << LogIO::POST;
    }
  }
  else {
    logIO() << LogIO::DEBUGGING
	    << "Starting FFT and scaling of image" << LogIO::POST;
    
    if(useDoubleGrid_p){
      ArrayLattice<DComplex> darrayLattice(griddedData2);
      LatticeFFT::cfft2d(darrayLattice,false);
      griddedData.resize(griddedData2.shape());
      convertArray(griddedData, griddedData2);
      SynthesisUtilMethods::getResource("mem peak in getImage");
      griddedData2.resize();
      arrayLattice = new ArrayLattice<Complex>(griddedData);
      lattice=arrayLattice;
    }else{
      arrayLattice = new ArrayLattice<Complex>(griddedData);
      lattice=arrayLattice;
      LatticeFFT::cfft2d(*lattice,false);

    }


    const IPosition latticeShape = lattice->shape();
    
    
    {
      Int inx = lattice->shape()(0);
      Int iny = lattice->shape()(1);
      Vector<Complex> correction(inx);
      correction=Complex(1.0, 0.0);
      Vector<Float> sincConvX(nx);
      for (Int ix=0;ix<nx;ix++) {
	Float x=C::pi*Float(ix-nx/2)/(Float(nx)*Float(convSampling));
	if(ix==nx/2) {
	  sincConvX(ix)=1.0;
	}
	else {
	  sincConvX(ix)=sin(x)/x;
	}
      }
      Vector<Float> sincConvY(ny);
      for (Int ix=0;ix<ny;ix++) {
	Float x=C::pi*Float(ix-ny/2)/(Float(ny)*Float(convSampling));
	if(ix==ny/2) {
	  sincConvY(ix)=1.0;
	}
	else {
	  sincConvY(ix)=sin(x)/x;
	}
      }
    
      
      
      IPosition cursorShape(4, inx, 1, 1, 1);
      IPosition axisPath(4, 0, 1, 2, 3);
      LatticeStepper lsx(lattice->shape(), cursorShape, axisPath);
      LatticeIterator<Complex> lix(*lattice, lsx);
      for(lix.reset();!lix.atEnd();lix++) {
	Int pol=lix.position()(2);
	Int chan=lix.position()(3);
	if(weights(pol, chan)!=0.0) {
	  Int iy=lix.position()(1);
	  gridder->correctX1D(correction,iy);
	  for (Int ix=0;ix<nx;ix++) {
	    correction(ix)*=sincConvX(ix)*sincConvY(iy);
	  }
	  lix.rwVectorCursor()/=correction;
	  if(normalize) {
	    Complex rnorm(Float(inx)*Float(iny)/weights(pol,chan));
	    lix.rwCursor()*=rnorm;
	  }
	  else {
	    Complex rnorm(Float(inx)*Float(iny));
	    lix.rwCursor()*=rnorm;
	  }
	}
	else {
	  lix.woCursor()=0.0;
	}
      }
    }

    if(!isTiled) {
      // Check the section from the image BEFORE converting to a lattice 
      IPosition blc(4, (nx-image->shape()(0)+(nx%2==0))/2,
		    (ny-image->shape()(1)+(ny%2==0))/2, 0, 0);
      IPosition stride(4, 1);
      IPosition trc(blc+image->shape()-stride);
      
      // Do the copy
      image->put(griddedData(blc, trc));
      //if(arrayLattice) delete arrayLattice; arrayLattice=0;
      griddedData.resize(IPosition(1,0));
    }
  }
  if(!arrayLattice.null()) arrayLattice=0;
  if(!lattice.null()) lattice=0;
  griddedData.resize();
  return *image;
}

// Get weight image
void WProjectFT::getWeightImage(ImageInterface<Float>& weightImage,
			      Matrix<Float>& weights) 
{
  
  logIO() << LogOrigin("WProjectFT", "getWeightImage") << LogIO::NORMAL;
  
  weights.resize(sumWeight.shape());
  convertArray(weights,sumWeight);
  
  const IPosition latticeShape = weightImage.shape();
  
  Int inx=latticeShape(0);
  Int iny=latticeShape(1);
  
  IPosition loc(2, 0);
  IPosition cursorShape(4, inx, iny, 1, 1);
  IPosition axisPath(4, 0, 1, 2, 3);
  LatticeStepper lsx(latticeShape, cursorShape, axisPath);
  LatticeIterator<Float> lix(weightImage, lsx);
  for(lix.reset();!lix.atEnd();lix++) {
    Int pol=lix.position()(2);
    Int chan=lix.position()(3);
    lix.rwCursor()=weights(pol,chan);
  }
}

Bool WProjectFT::toRecord(String& error,
			  RecordInterface& outRec, Bool withImage, const String diskimage)
{  

  /*

  CountedPtr<WPConvFunc> wpConvFunc_p;
  */

  // Save the current WProjectFT object to an output state record
  Bool retval = true;
  //save the base class variables
  //this is a memory hog and slow on saving and recovering...better to recompute convfunctions
  /* Record wpconvrec;
  if(wpConvFunc_p->toRecord(wpconvrec))
    outRec.defineRecord("wpconvfunc", wpconvrec);
  */
  Float elpadd=padding_p;
  if(toVis_p && withImage)
    elpadd=1.0;
  if(!FTMachine::toRecord(error, outRec, withImage, diskimage))
    return false;

  outRec.define("uvscale", uvScale);
  outRec.define("uvoffset", uvOffset);
  outRec.define("istiled", isTiled);
  outRec.define("cachesize", Int64(cachesize));
  outRec.define("tilesize", tilesize);
  outRec.define("maxabsdata", maxAbsData);
  Vector<Int> center_loc(4), offset_loc(4);
  for (Int k=0; k<4 ; k++){
    center_loc(k)=centerLoc(k);
    offset_loc(k)=offsetLoc(k);
  }
  outRec.define("centerloc", center_loc);
  outRec.define("offsetloc", offset_loc);
  outRec.define("padding", elpadd);
  outRec.define("nwplanes", nWPlanes_p);
  outRec.define("savedwscale", savedWScale_p);
  outRec.define("usezero", usezero_p);
  ///no need to save convfunc as it can be big and is recalculated anyways
  ///outRec.define("convfunc", convFunc);
  outRec.define("convsampling", convSampling);
  outRec.define("convsize", convSize);
  outRec.define("convsupport", convSupport);
  outRec.define("convsizes", convSizes_p);
  outRec.define("wconvsize", wConvSize);
  outRec.define("lastindex", lastIndex_p);
  outRec.define("minw", minW_p);
  outRec.define("maxw", maxW_p);
  outRec.define("rmsw", rmsW_p);


  return retval;
}

Bool WProjectFT::fromRecord(String& error,
			    const RecordInterface& inRec)
{
  if(!FTMachine::fromRecord(error, inRec))
    return false;
  machineName_p="WProjectFT";
  Bool retval = true;
  imageCache=0; lattice=0; arrayLattice=0;
  inRec.get("uvscale", uvScale);
  inRec.get("uvoffset", uvOffset);
  inRec.get("istiled", isTiled);
  cachesize=inRec.asInt64("cachesize");
  inRec.get("tilesize", tilesize);
  inRec.get("maxabsdata", maxAbsData);
  Vector<Int> center_loc(4), offset_loc(4);
  inRec.get("centerloc", center_loc);
  inRec.get("offsetloc", offset_loc);
  uInt ndim4 = 4;
  centerLoc=IPosition(ndim4, center_loc(0), center_loc(1), center_loc(2), 
		      center_loc(3));
  offsetLoc=IPosition(ndim4, offset_loc(0), offset_loc(1), offset_loc(2), 
		      offset_loc(3));
  inRec.get("minw", minW_p);
  inRec.get("maxw", maxW_p);
  inRec.get("rmsw", rmsW_p);
  if(inRec.isDefined("wpconvfunc")){
    wpConvFunc_p=new WPConvFunc(inRec.asRecord("wpconvfunc"));
  }
  else{
    wpConvFunc_p=new WPConvFunc(minW_p, maxW_p, rmsW_p);
  }
  inRec.get("padding", padding_p);
  inRec.get("nwplanes", nWPlanes_p);
  inRec.get("savedwscale", savedWScale_p);
  inRec.get("usezero", usezero_p);
  ///have stopped saving this parameter
  ////inRec.get("convfunc", convFunc);
  convFunc.resize();
  inRec.get("convsampling", convSampling);
  inRec.get("convsize", convSize);
  inRec.get("convsupport", convSupport);
  inRec.get("convsizes", convSizes_p);
  inRec.get("wconvsize", wConvSize);
  inRec.get("lastindex", lastIndex_p);
  
  gridder=0;
    ///setup some of the parameters
  init();
     

  
  return retval;
}

void WProjectFT::ok() {
  AlwaysAssert(image, AipsError);
}

// Make a plain straightforward honest-to-God image. This returns
// a complex image, without conversion to Stokes. The representation
// is that required for the visibilities.
//----------------------------------------------------------------------
void WProjectFT::makeImage(FTMachine::Type type, 
			 VisSet& vs,
			 ImageInterface<Complex>& theImage,
			 Matrix<Float>& weight) {
  
  
  logIO() << LogOrigin("WProjectFT", "makeImage") << LogIO::NORMAL;
  
  if(type==FTMachine::COVERAGE) {
    logIO() << "Type COVERAGE not defined for Fourier transforms"
	    << LogIO::EXCEPTION;
  }
  
  
  // Initialize the gradients
  ROVisIter& vi(vs.iter());
  
  // Loop over all visibilities and pixels
  VisBuffer vb(vi);
  
  // Initialize put (i.e. transform to Sky) for this model
  vi.origin();
  
  if(vb.polFrame()==MSIter::Linear) {
    StokesImageUtil::changeCStokesRep(theImage, StokesImageUtil::LINEAR);
  }
  else {
    StokesImageUtil::changeCStokesRep(theImage, StokesImageUtil::CIRCULAR);
  }
  
  initializeToSky(theImage,weight,vb);
  
  // Loop over the visibilities, putting VisBuffers
  for (vi.originChunks();vi.moreChunks();vi.nextChunk()) {
    for (vi.origin(); vi.more(); vi++) {
      
      switch(type) {
      case FTMachine::RESIDUAL:
	vb.visCube()=vb.correctedVisCube();
	vb.visCube()-=vb.modelVisCube();
        put(vb, -1, false);
        break;
      case FTMachine::MODEL:
	vb.visCube()=vb.modelVisCube();
        put(vb, -1, false);
        break;
      case FTMachine::CORRECTED:
	vb.visCube()=vb.correctedVisCube();
        put(vb, -1, false);
        break;
      case FTMachine::PSF:
	vb.visCube()=Complex(1.0,0.0);
        put(vb, -1, true);
        break;
      case FTMachine::OBSERVED:
      default:
        put(vb, -1, false);
        break;
      }
    }
  }
  finalizeToSky();
  // Normalize by dividing out weights, etc.
  getImage(weight, true);
}


String WProjectFT::name() const {

  return machineName_p;

}

  void WProjectFT::wStat(ROVisibilityIterator& vi, Double& minW, Double& maxW, Double& rmsW){
     VisBuffer vb(vi);
    maxW=0.0;
    minW=1e99;
    Double nval=0;
    rmsW=0.0;
    for (vi.originChunks(); vi.moreChunks(); vi.nextChunk()) {
      for (vi.origin();vi.more();vi++) {
	maxW=max(maxW, max(abs(vb.uvwMat().row(2)*max(vb.frequency())))/C::c);
	minW=min(minW, min(abs(vb.uvwMat().row(2)*min(vb.frequency())))/C::c);
	///////////some shenanigans as some compilers is confusing * operator for vector
	Vector<Double> elw;
	elw=vb.uvwMat().row(2);
	elw*=vb.uvwMat().row(2);
	//////////////////////////////////////////////////
	rmsW+=sum(elw);

	nval+=Double(vb.nRow());
      }
    }
    rmsW=sqrt(rmsW/Double(nval))*max(vb.frequency())/C::c;

  }


} //# NAMESPACE CASA - END

