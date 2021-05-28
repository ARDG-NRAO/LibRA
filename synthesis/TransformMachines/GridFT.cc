//# GridFT.cc: Implementation of GridFT class
//# Copyright (C) 1997-2014
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

#include <msvis/MSVis/VisibilityIterator.h>
#include <casa/Quanta/UnitMap.h>
#include <casa/Quanta/UnitVal.h>
#include <measures/Measures/Stokes.h>
#include <coordinates/Coordinates/CoordinateSystem.h>
#include <coordinates/Coordinates/DirectionCoordinate.h>
#include <coordinates/Coordinates/SpectralCoordinate.h>
#include <coordinates/Coordinates/StokesCoordinate.h>
#include <coordinates/Coordinates/Projection.h>
#include <ms/MeasurementSets/MSColumns.h>
#include <casa/BasicSL/Constants.h>
#include <scimath/Mathematics/FFTServer.h>
#include <synthesis/TransformMachines/GridFT.h>
#include <scimath/Mathematics/RigidVector.h>
#include <msvis/MSVis/StokesVector.h>
#include <synthesis/TransformMachines/StokesImageUtil.h>
#include <synthesis/TransformMachines/Utils.h>
#include <msvis/MSVis/VisBuffer.h>
#include <msvis/MSVis/VisSet.h>
#include <images/Images/ImageInterface.h>
#include <images/Images/PagedImage.h>
#include <casa/Containers/Block.h>
#include <casa/Containers/Record.h>
#include <casa/Arrays/ArrayLogical.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/MaskedArray.h>
#include <casa/Arrays/Vector.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/Cube.h>
#include <casa/Arrays/MatrixIter.h>
#include <casa/BasicSL/String.h>
#include <casa/Utilities/Assert.h>
#include <casa/Exceptions/Error.h>
#include <lattices/Lattices/ArrayLattice.h>
#include <measures/Measures/UVWMachine.h>
#include <lattices/Lattices/SubLattice.h>
#include <lattices/LRegions/LCBox.h>
#include <lattices/Lattices/LatticeCache.h>
#include <lattices/LatticeMath/LatticeFFT.h>
#include <lattices/Lattices/LatticeIterator.h>
#include <lattices/Lattices/LatticeStepper.h>
#include <scimath/Mathematics/ConvolveGridder.h>
#include <casa/Utilities/CompositeNumber.h>
#include <casa/OS/Timer.h>
#include <casa/sstream.h>
#ifdef _OPENMP
#include <omp.h>
#endif

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN
using namespace casacore;
  //  using namespace casa::async;
  GridFT::GridFT() : FTMachine(), padding_p(1.0), imageCache(0), cachesize(1000000), tilesize(1000), gridder(0), isTiled(false), convType("SF"),
  maxAbsData(0.0), centerLoc(IPosition(4,0)), offsetLoc(IPosition(4,0)),
  usezero_p(false), noPadding_p(false), usePut2_p(false), 
		     machineName_p("GridFT"), timemass_p(0.0), timegrid_p(0.0),  convFunc_p(0), convSampling_p(1), convSupport_p(0) {

  }
GridFT::GridFT(Long icachesize, Int itilesize, String iconvType, Float padding,
	       Bool usezero, Bool useDoublePrec)
: FTMachine(), padding_p(padding), imageCache(0), cachesize(icachesize), tilesize(itilesize),
  gridder(0), isTiled(false), convType(iconvType),
  maxAbsData(0.0), centerLoc(IPosition(4,0)), offsetLoc(IPosition(4,0)),
  usezero_p(usezero), noPadding_p(false), usePut2_p(false), 
  machineName_p("GridFT"), timemass_p(0.0), timegrid_p(0.0),  convFunc_p(0), convSampling_p(1), convSupport_p(0) 
{
  useDoubleGrid_p=useDoublePrec;  
  //  peek=NULL;
}

GridFT::GridFT(Long icachesize, Int itilesize, String iconvType,
	       MPosition mLocation, Float padding, Bool usezero, Bool useDoublePrec)
: FTMachine(), padding_p(padding), imageCache(0), cachesize(icachesize),
  tilesize(itilesize), gridder(0), isTiled(false), convType(iconvType), maxAbsData(0.0), centerLoc(IPosition(4,0)),
  offsetLoc(IPosition(4,0)), usezero_p(usezero), noPadding_p(false), 
  usePut2_p(false), machineName_p("GridFT"), timemass_p(0.0), timegrid_p(0.0),  convFunc_p(0), convSampling_p(1), convSupport_p(0) 
{
  mLocation_p=mLocation;
  tangentSpecified_p=false;
  useDoubleGrid_p=useDoublePrec;
  //  peek=NULL;
}

GridFT::GridFT(Long icachesize, Int itilesize, String iconvType,
	       MDirection mTangent, Float padding, Bool usezero, Bool useDoublePrec)
: FTMachine(), padding_p(padding), imageCache(0), cachesize(icachesize),
  tilesize(itilesize), gridder(0), isTiled(false), convType(iconvType), maxAbsData(0.0), centerLoc(IPosition(4,0)),
  offsetLoc(IPosition(4,0)), usezero_p(usezero), noPadding_p(false), 
  usePut2_p(false), machineName_p("GridFT"), timemass_p(0.0), timegrid_p(0.0), convFunc_p(0), convSampling_p(1), convSupport_p(0)
{
  mTangent_p=mTangent;
  tangentSpecified_p=true;
  useDoubleGrid_p=useDoublePrec;
  //  peek=NULL;
}

GridFT::GridFT(Long icachesize, Int itilesize, String iconvType,
	       MPosition mLocation, MDirection mTangent, Float padding,
	       Bool usezero, Bool useDoublePrec)
: FTMachine(), padding_p(padding), imageCache(0), cachesize(icachesize),
  tilesize(itilesize), gridder(0), isTiled(false), convType(iconvType), maxAbsData(0.0), centerLoc(IPosition(4,0)),
  offsetLoc(IPosition(4,0)), usezero_p(usezero), noPadding_p(false), 
  usePut2_p(false),machineName_p("GridFT"), timemass_p(0.0), timegrid_p(0.0),  convFunc_p(0), convSampling_p(1), convSupport_p(0) 
{
  mLocation_p=mLocation;
  mTangent_p=mTangent;
  tangentSpecified_p=true;
  useDoubleGrid_p=useDoublePrec;
  //  peek=NULL;
}

GridFT::GridFT(const RecordInterface& stateRec)
: FTMachine()
{
  // Construct from the input state record
  String error;
  if (!fromRecord(error, stateRec)) {
    throw (AipsError("Failed to create gridder: " + error));
  };
  timemass_p=0.0;
  timegrid_p=0.0;
  timedegrid_p=0.0;
  //  peek=NULL;
}

//---------------------------------------------------------------------- 
GridFT& GridFT::operator=(const GridFT& other)
{
  if(this!=&other) {
    //Do the base parameters
    FTMachine::operator=(other);
    
    //private params
    imageCache=other.imageCache;
    cachesize=other.cachesize;
    tilesize=other.tilesize;
    convType=other.convType;
    convType.upcase();
    uvScale.resize();
    uvOffset.resize();
    uvScale.assign(other.uvScale);
    uvOffset.assign(other.uvOffset);
    if(other.gridder==0)
      gridder=0;
    else{  
      gridder = new ConvolveGridder<Double, Complex>(IPosition(2, nx, ny),
						     uvScale, uvOffset,
						     convType);
    }
    convFunc_p.resize();
    convSupport_p=other.convSupport_p;
    convSampling_p=other.convSampling_p;
    convFunc_p=other.convFunc_p;
    isTiled=other.isTiled;
    //lattice=other.lattice;
    lattice=0;
    tilesize=other.tilesize;
    arrayLattice=0;
    maxAbsData=other.maxAbsData;
    centerLoc=other.centerLoc;
    offsetLoc=other.offsetLoc;
    padding_p=other.padding_p;
    usezero_p=other.usezero_p;
    noPadding_p=other.noPadding_p;	
    machineName_p="GridFT";
    timemass_p=0.0;
    timegrid_p=0.0;
    //    peek = other.peek;
  };
  return *this;
};

//----------------------------------------------------------------------
GridFT::GridFT(const GridFT& other) : FTMachine(), machineName_p("GridFT")
{
  operator=(other);
}

//-----------------------------------------------------------------------
  FTMachine* GridFT::cloneFTM(){
    return new GridFT(*this);
  }

//----------------------------------------------------------------------
void GridFT::init() {

  logIO() << LogOrigin("GridFT", "init")  << LogIO::NORMAL;

  ok();
  // if (peek == NULL) 
  //   {
  //     // peek = new SynthesisAsyncPeek();
  //     // peek->reset();
  //     // peek->startThread();
  //   }

  /* hardwiring isTiled is false
  // Padding is possible only for non-tiled processing
  if((padding_p*padding_p*image->shape().product())>cachesize) {
    isTiled=true;
    nx    = image->shape()(0);
    ny    = image->shape()(1);
    npol  = image->shape()(2);
    nchan = image->shape()(3);
  }
  else {
  */
    // We are padding.
    isTiled=false;
    if(!noPadding_p){
      CompositeNumber cn(uInt(image->shape()(0)*2));    
      nx    = cn.nextLargerEven(Int(padding_p*Float(image->shape()(0))-0.5));
      ny    = cn.nextLargerEven(Int(padding_p*Float(image->shape()(1))-0.5));
    }
    else{
      nx    = image->shape()(0);
      ny    = image->shape()(1);
    }
    npol  = image->shape()(2);
    nchan = image->shape()(3);
    // }

  sumWeight.resize(npol, nchan);

  uvScale.resize(2);
  uvScale(0)=(Float(nx)*image->coordinates().increment()(0)); 
  uvScale(1)=(Float(ny)*image->coordinates().increment()(1)); 
  uvOffset.resize(2);
  uvOffset(0)=nx/2;
  uvOffset(1)=ny/2;

  // Now set up the gridder. The possibilities are BOX and SF
  if(gridder) delete gridder; gridder=0;
  convType.upcase();
  gridder = new ConvolveGridder<Double, Complex>(IPosition(2, nx, ny),
						 uvScale, uvOffset,
						 convType);

  
  convFunc_p.resize();
  convSupport_p=gridder->cSupport()(0);
  convSampling_p=gridder->cSampling();
  convFunc_p=gridder->cFunction();
  // Set up image cache needed for gridding. For BOX-car convolution
  // we can use non-overlapped tiles. Otherwise we need to use
  // overlapped tiles and additive gridding so that only increments
  // to a tile are written.
  if(imageCache) delete imageCache; imageCache=0;

  if(isTiled) {
    Float tileOverlap=0.5;
    if(convType=="BOX") {
      tileOverlap=0.0;
    }
    else {
      tileOverlap=0.5;
      tilesize=max(12,tilesize);
    }
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
GridFT::~GridFT() {
  if(imageCache) delete imageCache; imageCache=0;
  //if(arrayLattice) delete arrayLattice; arrayLattice=0;
  if(gridder) delete gridder; gridder=0;
}

// Initialize for a transform from the Sky domain. This means that
// we grid-correct, and FFT the image

void GridFT::initializeToVis(ImageInterface<Complex>& iimage,
			     const VisBuffer& vb)
{
  image=&iimage;
  toVis_p=true;

  ok();

  init();
  //  peek->reset();
  // Initialize the maps for polarization and channel. These maps
  // translate visibility indices into image indices
  initMaps(vb);

  // Need to reset nx, ny for padding
  // Padding is possible only for non-tiled processing
  

  //cerr << "initialize to vis nx" << nx << "   " << ny <<  "  " << npol << " " << nchan << endl;

  //cerr << "image shape " << image->shape() << endl;

  // If we are memory-based then read the image in and create an
  // ArrayLattice otherwise just use the PagedImage
  /*if(isTiled) {
    lattice=CountedPtr<Lattice<Complex> >(image, false);
  }
  else {
     
  }*/
  prepGridForDegrid();

  //AlwaysAssert(lattice, AipsError);

 
    
}

void GridFT::prepGridForDegrid(){
  IPosition gridShape(4, nx, ny, npol, nchan);
  griddedData.resize(gridShape);
  //griddedData can be a reference of image data...if not using model col
  //hence using an undocumented feature of resize that if 
  //the size is the same as old data it is not changed.
  //if(!usePut2_p) griddedData.set(0);
  griddedData.set(Complex(0.0));

  IPosition stride(4, 1);
  IPosition blc(4, (nx-image->shape()(0)+(nx%2==0))/2, (ny-image->shape()(1)+(ny%2==0))/2, 0, 0);
  IPosition trc(blc+image->shape()-stride);
  
  IPosition start(4, 0);
  griddedData(blc, trc) = image->getSlice(start, image->shape());
  
  //if(arrayLattice) delete arrayLattice; arrayLattice=0;
  arrayLattice = new ArrayLattice<Complex>(griddedData);
  lattice=arrayLattice;
  //logIO() << LogIO::DEBUGGING
  //	  << "Starting grid correction and FFT of image" << LogIO::POST;

   // Do the Grid-correction. 
   {
     Vector<Complex> correction(nx);
     correction=Complex(1.0, 0.0);
     // Do the Grid-correction
     IPosition cursorShape(4, nx, 1, 1, 1);
     IPosition axisPath(4, 0, 1, 2, 3);
     LatticeStepper lsx(lattice->shape(), cursorShape, axisPath);
     LatticeIterator<Complex> lix(*lattice, lsx);
     for(lix.reset();!lix.atEnd();lix++) {
       gridder->correctX1D(correction, lix.position()(1));
       lix.rwVectorCursor()/=correction;
     }
   }
   image->clearCache();
   // Now do the FFT2D in place
   LatticeFFT::cfft2d(*lattice);
   
   //logIO() << LogIO::DEBUGGING
   //	    << "Finished grid correction and FFT of image" << LogIO::POST;
    
}


void GridFT::finalizeToVis()
{

  //cerr <<"Time to degrid " << timedegrid_p << endl;
  timedegrid_p=0.0;

  if(!arrayLattice.null()) arrayLattice=0;
  if(!lattice.null()) lattice=0;
  griddedData.resize();

  if(isTiled) {

    logIO() << LogOrigin("GridFT", "finalizeToVis")  << LogIO::NORMAL;

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
void GridFT::initializeToSky(ImageInterface<Complex>& iimage,
			     Matrix<Float>& weight, const VisBuffer& vb)
{
  // image always points to the image
  image=&iimage;
  toVis_p=false;

  init();

  // Initialize the maps for polarization and channel. These maps
  // translate visibility indices into image indices
  initMaps(vb);


  
  sumWeight=0.0;
  weight.resize(sumWeight.shape());
  weight=0.0;

  // Initialize for in memory or to disk gridding. lattice will
  // point to the appropriate Lattice, either the ArrayLattice for
  // in memory gridding or to the image for to disk gridding.
  if(isTiled) {
    imageCache->flush();
    image->set(Complex(0.0));
    lattice=CountedPtr<Lattice<Complex> >(image, false);
  }
  else {
    IPosition gridShape(4, nx, ny, npol, nchan);
    if( !useDoubleGrid_p )
      {
	griddedData.resize(gridShape);
	griddedData=Complex(0.0);
      }
    if(useDoubleGrid_p){
      //griddedData.resize();
      griddedData2.resize(gridShape);
      griddedData2=DComplex(0.0);
    }
    image->clearCache();
    //iimage.get(griddedData, false);
    //if(arrayLattice) delete arrayLattice; arrayLattice=0;
    
  }
  //AlwaysAssert(lattice, AipsError);
}



void GridFT::finalizeToSky()
{  
  //AlwaysAssert(lattice, AipsError);
  // Now we flush the cache and report statistics
  // For memory based, we don't write anything out yet.
  //cerr <<"Time to massage data " << timemass_p << endl;
  //cerr <<"Time to grid data " << timegrid_p << endl;
  timemass_p=0.0;
  timegrid_p=0.0;
  if(isTiled) {
    logIO() << LogOrigin("GridFT", "finalizeToSky")  << LogIO::NORMAL;

    AlwaysAssert(image, AipsError);
    AlwaysAssert(imageCache, AipsError);
    imageCache->flush();
    ostringstream o;
    imageCache->showCacheStatistics(o);
    logIO() << o.str() << LogIO::POST;
  }
  //  peek->terminate();
  //  peek->reset();
}



Array<Complex>* GridFT::getDataPointer(const IPosition& centerLoc2D,
				       Bool readonly) {
  Array<Complex>* result;
  // Is tiled: get tiles and set up offsets
  centerLoc(0)=centerLoc2D(0);
  centerLoc(1)=centerLoc2D(1);
  result=&imageCache->tile(offsetLoc,centerLoc, readonly);
  gridder->setOffset(IPosition(2, offsetLoc(0), offsetLoc(1)));
  return result;
}

#define NEED_UNDERSCORES
#if defined(NEED_UNDERSCORES)
#define ggrid ggrid_
#define dgrid dgrid_
#define ggrids ggrids_
#define sectggridd sectggridd_
#define sectggrids sectggrids_
#define sectdgrid sectdgrid_
#define locuvw locuvw_
#endif

extern "C" { 
  void locuvw(const Double*, const Double*, const Double*, const Int*, const Double*, const Double*, const Int*, 
	      Int*, Int*, Complex*, const Int*, const Int*, const Double*);
   void ggrid(Double*,
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
		Double*,
		Int*,
		Int*,
		Double*);
  void sectggridd(const Complex*,
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
		const Double*,
		const Int*,
		const Int*,
		  Double*,
		  const Int*,
		  const Int*, 
		  const Int*,
		  const Int*,
		  const Int*, 
		  const Int*,
		  //loc, off, phasor
		  const Int*,
		  const Int*,
		  const Complex*);
  //single precision gridder
  void sectggrids(const Complex*,
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
                const Int*,
		const Int*,
		const Double*,
		const Int*,
		const Int*,
		  Double*,
		  const Int*,
		  const Int*, 
		  const Int*,
		  const Int*,
		  const Int*, 
		  const Int*,
		  //loc, off, phasor
		  const Int*,
		  const Int*,
		  const Complex*);
  void ggrids(Double*,
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
		Double*,
		Int*,
		Int*,
		Double*);

   void dgrid(Double*,
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
		Double*,
		Int*,
		Int*);

  void sectdgrid(Complex*,
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
		const Double*,
		const Int*,
		 const Int*,
		 //rbeg, rend, loc, off, phasor
		 const Int*,
		 const Int*,
		 const Int*,
		 const Int*,
		 const Complex*);
}
void GridFT::put(const VisBuffer& vb, Int row, Bool dopsf, 
		 FTMachine::Type type)
{

  gridOk(convSupport_p);
  //  peek->setVBPtr(&vb);

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
    {
      //      peek->reset();
      return;
    }

  Timer tim;
  tim.mark();

  const Matrix<Float> *imagingweight;
  imagingweight=&(vb.imagingWeight());
  
  if(dopsf) {type=FTMachine::PSF;}

  Cube<Complex> data;
  //Fortran gridder need the flag as ints 
  Cube<Int> flags;
  Matrix<Float> elWeight;
  interpolateFrequencyTogrid(vb, *imagingweight,data, flags, elWeight, type);


  Bool iswgtCopy;
  const Float *wgtStorage;
  wgtStorage=elWeight.getStorage(iswgtCopy);
  
  Bool isCopy;
  const Complex *datStorage;
  if(!dopsf)
    datStorage=data.getStorage(isCopy);
  else
    datStorage=0;
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

  //cerr << "nRow " << nRow << endl;


  // Get the uvws in a form that Fortran can use and do that
  // necessary phase rotation. On a Pentium Pro 200 MHz
  // when null, this step takes about 50us per uvw point. This
  // is just barely noticeable for Stokes I continuum and
  // irrelevant for other cases.
 
  Bool del; 
  //    IPosition s(flags.shape());
  const IPosition &fs=flags.shape();
  std::vector<Int> s(fs.begin(),fs.end());
  Int nvispol=s[0];
  Int nvischan=s[1];
  Int nvisrow=s[2];
  Matrix<Double> uvw(3, vb.uvw().nelements());
  uvw=0.0;
  Vector<Double> dphase(vb.uvw().nelements());
  Cube<Int> loc(2, nvischan, nRow);
  Cube<Int> off(2, nvischan, nRow);
  Matrix<Complex> phasor(nvischan, nRow);
  Int csamp=convSampling_p;
  dphase=0.0;
  //NEGATING to correct for an image inversion problem
  for (Int i=startRow;i<=endRow;i++) {
    for (Int idim=0;idim<2;idim++) uvw(idim,i)=-vb.uvw()(i)(idim);
    uvw(2,i)=vb.uvw()(i)(2);
  }
  timemass_p +=tim.real();
  tim.mark(); 
  rotateUVW(uvw, dphase, vb);
  refocus(uvw, vb.antenna1(), vb.antenna2(), dphase, vb);
  Bool delphase;
  Complex * phasorstor=phasor.getStorage(delphase);
  const Double * visfreqstor=interpVisFreq_p.getStorage(del);
  const Double * scalestor=uvScale.getStorage(del);
  const Double * offsetstor=uvOffset.getStorage(del);
  const Double* uvstor= uvw.getStorage(del);
  Int * locstor=loc.getStorage(del);
  Int * offstor=off.getStorage(del);
  const Double *dpstor=dphase.getStorage(del);
  //Vector<Double> f1=interpVisFreq_p.copy();
  Int nvchan=nvischan;
  Int irow;
  Double cinv=Double(1.0)/C::c;
  Int dow=0;
  Int nth=1;
#ifdef _OPENMP
  if(numthreads_p >0){
    nth=min(numthreads_p, omp_get_max_threads());
  }
  else{   
    nth= omp_get_max_threads();
  }
  nth=min(4,nth);
#endif
  

#pragma omp parallel default(none) private(irow) firstprivate(visfreqstor, nvchan, scalestor, offsetstor, csamp, phasorstor, uvstor, locstor, offstor, dpstor, cinv, dow) shared(startRow, endRow) num_threads(nth)
  {
#pragma omp for
  for (irow=startRow; irow<=endRow; ++irow){
    //locateuvw(uvstor,dpstor, visfreqstor, nvchan, scalestor, offsetstor, csamp, 
    //	      locstor, 
    //	      offstor, phasorstor, irow);
    locuvw(uvstor, dpstor, visfreqstor, &nvchan, scalestor, offsetstor, &csamp, locstor, offstor, phasorstor, &irow, &dow, &cinv); 
  }  

  }//end pragma parallel
  // Take care of translation of Bools to Integer
  //////TESTOO
  //ofstream myfile;
  //myfile.open ("putLoc.txt", ios::out | ios::app | ios::ate );
  //myfile << vb.rowIds()(0) << " uv " << uvw.column(0) << " loc " << loc(0,0,0) << ", " << loc(1,0,0) << "\n" << endl;
  //myfile.close();
  ///////////////
  
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
  
  /////////////Some extra stuff for openmp

  Int x0, y0, nxsub, nysub, ixsub, iysub, icounter, ix, iy;
  Int csupp=convSupport_p;
  
  const Double * convfuncstor=(gridder->cFunction()).getStorage(del);
  
  // cerr <<"Poffset " << min(off) << "  " << max(off) << " length " << gridder->cFunction().shape() << endl;
  


  ixsub=1;
  iysub=1; 
  if (nth >3){
    ixsub=2;
    iysub=2; 
  }
  else if(nth >1){
     ixsub=2;
     iysub=1; 
  }

  x0=1;
  y0=1;
  nxsub=nx;
  nysub=ny;
  Int rbeg=startRow+1;
  Int rend=endRow+1;
  Block<Matrix<Double> > sumwgt(ixsub*iysub);
  for (icounter=0; icounter < ixsub*iysub; ++icounter){
    sumwgt[icounter].resize(sumWeight.shape());
    sumwgt[icounter].set(0.0);
  }
  const Int* pmapstor=polMap.getStorage(del);
  const Int *cmapstor=chanMap.getStorage(del);
  Int nc=nchan;
  Int np=npol;
  Int nxp=nx;
  Int nyp=ny;
  const Int * flagstor=flags.getStorage(del);
  const Int * rowflagstor=rowFlags.getStorage(del);
  ////////////////////////

  Bool gridcopy;
  if(useDoubleGrid_p){
    DComplex *gridstor=griddedData2.getStorage(gridcopy);
#pragma omp parallel default(none) private(icounter,ix,iy,x0,y0,nxsub,nysub, del) firstprivate(idopsf, datStorage, wgtStorage, flagstor, rowflagstor, convfuncstor, pmapstor, cmapstor, gridstor, nxp, nyp, np, nc,ixsub, iysub, rend, rbeg, csamp, csupp, nvispol, nvischan, nvisrow, phasorstor, locstor, offstor) shared(sumwgt) num_threads(ixsub*iysub)
  
  {
    //cerr << "numthreads " << omp_get_num_threads() << endl;
#pragma omp for 
    for(icounter=0; icounter < ixsub*iysub; ++icounter){
      //cerr << "thread id " << omp_get_thread_num() << endl;
      ix= (icounter+1)-((icounter)/ixsub)*ixsub;
      iy=(icounter)/ixsub+1;
      y0=(nyp/iysub)*(iy-1)+1;
      nysub=nyp/iysub;
      if( iy == iysub) {
	nysub=nyp-(nyp/iysub)*(iy-1);
      }
      x0=(nxp/ixsub)*(ix-1)+1;
      nxsub=nxp/ixsub;
      if( ix == ixsub){
	nxsub=nxp-(nxp/ixsub)*(ix-1);
      } 
      sectggridd(datStorage,
	  &nvispol,
	  &nvischan,
	  &idopsf,
	  flagstor,
	  rowflagstor,
	  wgtStorage,
	  &nvisrow,
	  gridstor,
	  &nxp,
	  &nyp,
	  &np,
	  &nc,
	  &csupp,
	   &csamp,
	  convfuncstor,
	  cmapstor,
	  pmapstor,
	 (sumwgt[icounter]).getStorage(del),
		 &x0, &y0, &nxsub, &nysub, &rbeg, &rend, locstor, offstor,
		 phasorstor);
    }//end for
  }// end pragma parallel
  for (icounter=0; icounter < ixsub*iysub; ++icounter){
      sumWeight=sumWeight+sumwgt[icounter];
  }
  //phasor.putStorage(phasorstor, delphase); 
  griddedData2.putStorage(gridstor, gridcopy);
  }
  else{
    Complex *gridstor=griddedData.getStorage(gridcopy);
#pragma omp parallel default(none) private(icounter,ix,iy,x0,y0,nxsub,nysub, del) firstprivate(idopsf, datStorage, wgtStorage, flagstor, rowflagstor, convfuncstor, pmapstor, cmapstor, gridstor, nxp, nyp, np, nc,ixsub, iysub, rend, rbeg, csamp, csupp, nvispol, nvischan, nvisrow, phasorstor, locstor, offstor) shared(sumwgt) num_threads(ixsub*iysub)
    {
      //cerr << "numthreads " << omp_get_num_threads() << endl;
#pragma omp for

      for(icounter=0; icounter < ixsub*iysub; ++icounter){
	//cerr << "thread id " << omp_get_thread_num() << endl;
	ix= (icounter+1)-((icounter)/ixsub)*ixsub;
	iy=(icounter)/ixsub+1;
	y0=(nyp/iysub)*(iy-1)+1;
	nysub=nyp/iysub;
	if( iy == iysub) {
	  nysub=nyp-(nyp/iysub)*(iy-1);
	}
	x0=(nxp/ixsub)*(ix-1)+1;
	nxsub=nxp/ixsub;
	if( ix == ixsub){
	  nxsub=nxp-(nxp/ixsub)*(ix-1);
	} 
	//cerr << "x0 " << x0 << " y0 " << y0 << " nxsub " << nxsub << " nysub " << nysub << endl;
	sectggrids(datStorage,
		   &nvispol,
		   &nvischan,
		   &idopsf,
		   flagstor,
		   rowflagstor,
		   wgtStorage,
		   &nvisrow,
		   gridstor,
		   &nxp,
		   &nyp,
		   &np,
		   &nc,
		   &csupp,
		   &csamp,
		   convfuncstor,
		   cmapstor,
		   pmapstor,
		   (sumwgt[icounter]).getStorage(del),
		   &x0, &y0, &nxsub, &nysub, &rbeg, &rend, locstor, offstor,
		   phasorstor);
      }//end for
  }// end pragma parallel
    for (icounter=0; icounter < ixsub*iysub; ++icounter){
      sumWeight=sumWeight+sumwgt[icounter];
    }
    
    griddedData.putStorage(gridstor, gridcopy);
  }
  // cerr << "sunweight " << sumWeight << endl;

  timegrid_p+=tim.real();
  
  if(!dopsf)
    data.freeStorage(datStorage, isCopy);
  elWeight.freeStorage(wgtStorage,iswgtCopy);
  
  //  peek->reset();
}

void GridFT::modifyConvFunc(const Vector<Double>& convFunc, Int convSupport, Int convSampling){
  convFunc_p.resize();
  convFunc_p=convFunc;
  convSupport_p=convSupport;
  convSampling_p=convSampling;

} 

void GridFT::get(VisBuffer& vb, Int row)
{

  gridOk(convSupport_p);
  // If row is -1 then we pass through all rows
  Int startRow, endRow, nRow;
  if (row < 0) {
    nRow=vb.nRow();
    startRow=0;
    endRow=nRow-1;
    //unnecessary zeroing
    //vb.modelVisCube()=Complex(0.0,0.0);
  } else {
    nRow=1;
    startRow=row;
    endRow=row;
    //unnecessary zeroing
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

  //cerr << "chanMap " << chanMap << endl;
  //No point in reading data if its not matching in frequency
  if(max(chanMap)==-1)
    return;

  Cube<Complex> data;
  Cube<Int> flags;
  getInterpolateArrays(vb, data, flags);

  //cerr << "max min data " << max(griddedData) << " " << min(griddedData) << "  flags  " << max(flags) << "    " << min(flags) << endl;  

  //    IPosition s(data.shape());
  Int nvp=data.shape()(0);
  Int nvc=data.shape()(1);
  Int nvisrow=data.shape()(2);
    

  //cerr << "get flags " << min(flags) << "  " << max(flags) << endl;
  Complex *datStorage;
  Bool isCopy;
  datStorage=data.getStorage(isCopy);
  
  ///
  Cube<Int> loc(2, nvc, nvisrow);
  Cube<Int> off(2, nvc, nRow);
  Matrix<Complex> phasor(nvc, nRow);
  Int csamp=convSampling_p;
  Bool delphase;
  Bool del;
  Complex * phasorstor=phasor.getStorage(delphase);
  const Double * visfreqstor=interpVisFreq_p.getStorage(del);
  const Double * scalestor=uvScale.getStorage(del);
  const Double * offsetstor=uvOffset.getStorage(del);
  const Double* uvstor= uvw.getStorage(del);
  Int * locstor=loc.getStorage(del);
  Int * offstor=off.getStorage(del);
  const Double *dpstor=dphase.getStorage(del);
  //Vector<Double> f1=interpVisFreq_p.copy();
  Int nvchan=nvc;
  Int irow;
  Double cinv=Double(1.0)/C::c;
  Int dow=0;
  Int nth=1;
#ifdef _OPENMP
  if(numthreads_p >0){
    nth=min(numthreads_p, omp_get_max_threads());
  }
  else{   
    nth= omp_get_max_threads();
  }
  nth=min(4,nth);
#endif


#pragma omp parallel default(none) private(irow) firstprivate(visfreqstor, nvchan, scalestor, offsetstor, csamp, phasorstor, uvstor, locstor, offstor, dpstor, cinv, dow) shared(startRow, endRow) num_threads(nth) 

  {
#pragma omp for
  for (irow=startRow; irow<=endRow; ++irow){
    //locateuvw(uvstor,dpstor, visfreqstor, nvchan, scalestor, offsetstor, csamp, 
    //	      locstor, 
    //	      offstor, phasorstor, irow);
    
    locuvw(uvstor, dpstor, visfreqstor, &nvchan, scalestor, offsetstor, &csamp, locstor, offstor, phasorstor, &irow, &dow, &cinv);
  }  

  }//end pragma parallel

  Int rbeg=startRow+1;
  Int rend=endRow+1;


  Vector<Int> rowFlags(vb.nRow());
  rowFlags=0;
  rowFlags(vb.flagRow())=true;
  //cerr << "rowFlags " << rowFlags << endl;
  if(!usezero_p) {
    for (Int rownr=startRow; rownr<=endRow; rownr++) {
      if(vb.antenna1()(rownr)==vb.antenna2()(rownr)) rowFlags(rownr)=1;
    }
  }


  //cerr <<"offset " << min(off) << "  " <<max(off) << " length " << gridder->cFunction().shape() << endl;

  {
    Bool delgrid;
    const Complex* gridstor=griddedData.getStorage(delgrid);
    const Double * convfuncstor=convFunc_p.getStorage(del);;
    
    const Int* pmapstor=polMap.getStorage(del);
    const Int *cmapstor=chanMap.getStorage(del);
    Int nc=nchan;
    Int np=npol;
    Int nxp=nx;
    Int nyp=ny;
    Int csupp=convSupport_p;
    const Int * flagstor=flags.getStorage(del);
    const Int * rowflagstor=rowFlags.getStorage(del);


    Int npart=1;
    if (nth >3){
      npart=4;
    }
    else if(nth >1){
     npart=2; 
    }

    Int ix=0;
#pragma omp parallel default(none) private(ix, rbeg, rend) firstprivate(datStorage, flagstor, rowflagstor, convfuncstor, pmapstor, cmapstor, gridstor, nxp, nyp, np, nc, csamp, csupp, nvp, nvc, nvisrow, phasorstor, locstor, offstor) shared(npart) num_threads(npart)
    { 
#pragma omp for 
      for (ix=0; ix< npart; ++ix){
	rbeg=ix*(nvisrow/npart)+1;
	rend=(ix != (npart-1)) ? (rbeg+(nvisrow/npart)-1) : (rbeg+(nvisrow/npart)+nvisrow%npart-1) ;
	//cerr << "rbeg " << rbeg << " rend " << rend << "  " << nvisrow << endl;
	
	sectdgrid(datStorage,
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
		  &csupp,
		  &csamp,
		  convfuncstor,
		  cmapstor,
		  pmapstor,
		  &rbeg, &rend, locstor, offstor, phasorstor);
      }//end pragma parallel
    }
    data.putStorage(datStorage, isCopy);
    griddedData.freeStorage(gridstor, delgrid);
    //cerr << "Get min max " << min(data) << "   " << max(data) << endl;
  }
  interpolateFrequencyFromgrid(vb, data, FTMachine::MODEL);

}



// Finalize the FFT to the Sky. Here we actually do the FFT and
// return the resulting image
ImageInterface<Complex>& GridFT::getImage(Matrix<Float>& weights, Bool normalize) 
{
  //AlwaysAssert(lattice, AipsError);
  AlwaysAssert(gridder, AipsError);
  AlwaysAssert(image, AipsError);
  logIO() << LogOrigin("GridFT", "getImage") << LogIO::NORMAL;

  if((griddedData.nelements() ==0) && (griddedData2.nelements()==0)){
    throw(AipsError("Programmer error ...request for image without right order of calls"));
  }
  weights.resize(sumWeight.shape());

  convertArray(weights, sumWeight);
  // If the weights are all zero then we cannot normalize
  // otherwise we don't care.
  if(normalize&&max(weights)==0.0) {
    logIO() << LogIO::SEVERE << "No useful data in GridFT: weights all zero"
	    << LogIO::POST;
  }
  else {

    //const IPosition latticeShape = lattice->shape();
    
    //logIO() << LogIO::DEBUGGING
    //	    << "Starting FFT and scaling of image" << LogIO::POST;
    

  
    // if(useDoubleGrid_p){
    //   convertArray(griddedData, griddedData2);
    //   //Don't need the double-prec grid anymore...
    //   griddedData2.resize();
    // }

    // x and y transforms
    //    LatticeFFT::cfft2d(*lattice,false);
    //
    // Retain the double precision grid for FFT as well.  Convert it
    // to single precision just after (since images are still single
    // precision).
    //
    if(useDoubleGrid_p)
      {
	ArrayLattice<DComplex> darrayLattice(griddedData2);
	
	LatticeFFT::cfft2d(darrayLattice,false);
	griddedData.resize(griddedData2.shape());
	convertArray(griddedData, griddedData2);

	SynthesisUtilMethods::getResource("mem peak in getImage");
	
	//Don't need the double-prec grid anymore...
	griddedData2.resize();
	arrayLattice = new ArrayLattice<Complex>(griddedData);
	lattice=arrayLattice;
      }
    else{
      arrayLattice = new ArrayLattice<Complex>(griddedData);
      //storeArrayAsImage(String("cgrid_ft.im"), image->coordinates(), griddedData);
      lattice=arrayLattice;
      LatticeFFT::cfft2d(*lattice,false);
    }

    
    
    {
      Int inx = lattice->shape()(0);
      Int iny = lattice->shape()(1);
      Vector<Complex> correction(inx);
      correction=Complex(1.0, 0.0);
      // Do the Grid-correction
      IPosition cursorShape(4, inx, 1, 1, 1);
      IPosition axisPath(4, 0, 1, 2, 3);
      LatticeStepper lsx(lattice->shape(), cursorShape, axisPath);
      LatticeIterator<Complex> lix(*lattice, lsx);
      for(lix.reset();!lix.atEnd();lix++) {
	Int pol=lix.position()(2);
	Int chan=lix.position()(3);
	if(weights(pol, chan)!=0.0) {
	  gridder->correctX1D(correction, lix.position()(1));
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
      IPosition blc(4, (nx-image->shape()(0)+(nx%2==0))/2, (ny-image->shape()(1)+(ny%2==0))/2, 0, 0);
      IPosition stride(4, 1);
      IPosition trc(blc+image->shape()-stride);
      // Do the copy
      IPosition start(4, 0);
      image->put(griddedData(blc, trc));
    }
  }

  image->flush();
  image->clearCache();

  if(!arrayLattice.null()) arrayLattice=0;
  if(!lattice.null()) lattice=0;
  griddedData.resize();

  return *image;
}

// Get weight image
void GridFT::getWeightImage(ImageInterface<Float>& weightImage, Matrix<Float>& weights) 
{

  logIO() << LogOrigin("GridFT", "getWeightImage") << LogIO::NORMAL;

  weights.resize(sumWeight.shape());
  convertArray(weights,sumWeight);

  const IPosition latticeShape = weightImage.shape();

  Int nx=latticeShape(0);
  Int ny=latticeShape(1);

  IPosition loc(2, 0);
  IPosition cursorShape(4, nx, ny, 1, 1);
  IPosition axisPath(4, 0, 1, 2, 3);
  LatticeStepper lsx(latticeShape, cursorShape, axisPath);
  LatticeIterator<Float> lix(weightImage, lsx);
  for(lix.reset();!lix.atEnd();lix++) {
    Int pol=lix.position()(2);
    Int chan=lix.position()(3);
    lix.rwCursor()=weights(pol,chan);
  }
}

Bool GridFT::toRecord(String& error,
		      RecordInterface& outRec, Bool withImage, const String diskimage)
{

  

  // Save the current GridFT object to an output state record
  Bool retval = true;
  Float elpadd=padding_p;
  if(toVis_p && withImage)
    elpadd=1.0;
  //save the base class variables
  if(!FTMachine::toRecord(error, outRec, withImage, diskimage))
    return false;

  //a call to init  will redo imagecache and gridder
  // so no need to save these

  outRec.define("cachesize", Int64(cachesize));
  outRec.define("tilesize", tilesize);
  outRec.define("convtype", convType);
  outRec.define("uvscale", uvScale);
  outRec.define("uvoffset", uvOffset);
  outRec.define("istiled", isTiled);
  outRec.define("maxabsdata", maxAbsData);
  Vector<Int> center_loc(4), offset_loc(4);
  for (Int k=0; k<4 ; k++){
    center_loc(k)=centerLoc(k);
    offset_loc(k)=offsetLoc(k);
  }
  outRec.define("centerloc", center_loc);
  outRec.define("offsetloc", offset_loc);
  outRec.define("padding", elpadd);
  outRec.define("usezero", usezero_p);
  outRec.define("nopadding", noPadding_p);
  return retval;
}

Bool GridFT::fromRecord(String& error,
			const RecordInterface& inRec)
{
  Bool retval = true;
  if(!FTMachine::fromRecord(error, inRec))
    return false;
  gridder=0; imageCache=0; lattice=0; arrayLattice=0;
  //For some reason int64 seems to be getting lost...
  //this is not an important parameter...so filing a jira 
  if(inRec.isDefined("cachesize"))
    cachesize=inRec.asInt64("cachesize");
  else
    cachesize=1000000;
  inRec.get("tilesize", tilesize);
  inRec.get("convtype", convType);
  inRec.get("uvscale", uvScale);
  inRec.get("uvoffset", uvOffset);
  inRec.get("istiled", isTiled);
  inRec.get("maxabsdata", maxAbsData);
  Vector<Int> center_loc(4), offset_loc(4);
  inRec.get("centerloc", center_loc);
  inRec.get("offsetloc", offset_loc);
  uInt ndim4 = 4;
  centerLoc=IPosition(ndim4, center_loc(0), center_loc(1), center_loc(2), 
		      center_loc(3));
  offsetLoc=IPosition(ndim4, offset_loc(0), offset_loc(1), offset_loc(2), 
		      offset_loc(3));
  inRec.get("padding", padding_p);
  inRec.get("usezero", usezero_p);
  inRec.get("nopadding", noPadding_p);

  machineName_p="GridFT";
  ///setup some of the parameters
  init();
  /*if(!cmplxImage_p.null()){
    //FTMachine::fromRecord would have recovered the image
    // Might be changing the shape of sumWeight

    if(isTiled) {
      lattice=CountedPtr<Lattice<Complex> >(image, false);
    }
    else {
      // Make the grid the correct shape and turn it into an array lattice
      // Check the section from the image BEFORE converting to a lattice 
      if(!toVis_p){
	IPosition gridShape(4, nx, ny, npol, nchan);
	griddedData.resize(gridShape);
	griddedData=Complex(0.0);
      }
      else{
	prepGridForDegrid();
      }
    }
    };*/
  return retval;
}

void GridFT::ok() {
  AlwaysAssert(image, AipsError);
}

// Make a plain straightforward honest-to-God image. This returns
// a complex image, without conversion to Stokes. The representation
// is that required for the visibilities.
//----------------------------------------------------------------------
void GridFT::makeImage(FTMachine::Type type, 
		       VisSet& vs,
		       ImageInterface<Complex>& theImage,
		       Matrix<Float>& weight) {


  logIO() << LogOrigin("GridFT", "makeImage") << LogIO::NORMAL;

  if(type==FTMachine::COVERAGE) {
    logIO() << "Type COVERAGE not defined for Fourier transforms" << LogIO::EXCEPTION;
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

String GridFT::name() const {

  return machineName_p;


}

} //# NAMESPACE CASA - END

