//# GridFT.cc: Implementation of GridFT class
//# Copyright (C) 1997-2012
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
#include <casacore/casa/Quanta/UnitMap.h>
#include <casacore/casa/Quanta/UnitVal.h>
#include <casacore/measures/Measures/Stokes.h>
#include <casacore/coordinates/Coordinates/CoordinateSystem.h>
#include <casacore/coordinates/Coordinates/DirectionCoordinate.h>
#include <casacore/coordinates/Coordinates/SpectralCoordinate.h>
#include <casacore/coordinates/Coordinates/StokesCoordinate.h>
#include <casacore/coordinates/Coordinates/Projection.h>
#include <casacore/ms/MeasurementSets/MSColumns.h>
#include <casacore/casa/BasicSL/Constants.h>
#include <casacore/scimath/Mathematics/FFTServer.h>
#include <synthesis/TransformMachines/rGridFT.h>
#include <casacore/scimath/Mathematics/RigidVector.h>
#include <msvis/MSVis/StokesVector.h>
#include <synthesis/TransformMachines/StokesImageUtil.h>
#include <msvis/MSVis/VisBuffer.h>
#include <msvis/MSVis/VisSet.h>
#include <casacore/images/Images/ImageInterface.h>
#include <casacore/images/Images/PagedImage.h>
#include <casacore/casa/Containers/Block.h>
#include <casacore/casa/Containers/Record.h>
#include <casacore/casa/Arrays/ArrayLogical.h>
#include <casacore/casa/Arrays/ArrayMath.h>
#include <casacore/casa/Arrays/Array.h>
#include <casacore/casa/Arrays/MaskedArray.h>
#include <casacore/casa/Arrays/Vector.h>
#include <casacore/casa/Arrays/Matrix.h>
#include <casacore/casa/Arrays/Cube.h>
#include <casacore/casa/Arrays/MatrixIter.h>
#include <casacore/casa/BasicSL/String.h>
#include <casacore/casa/Utilities/Assert.h>
#include <casacore/casa/Exceptions/Error.h>
#include <casacore/lattices/Lattices/ArrayLattice.h>
#include <casacore/measures/Measures/UVWMachine.h>
#include <casacore/lattices/Lattices/SubLattice.h>
#include <casacore/lattices/LRegions/LCBox.h>
#include <casacore/lattices/Lattices/LatticeCache.h>
#include <casacore/lattices/LatticeMath/LatticeFFT.h>
#include <casacore/lattices/Lattices/LatticeIterator.h>
#include <casacore/lattices/Lattices/LatticeStepper.h>
#include <casacore/scimath/Mathematics/ConvolveGridder.h>
#include <casacore/casa/Utilities/CompositeNumber.h>
#include <casacore/casa/OS/Timer.h>
#include <sstream>
#ifdef HAS_OMP
#include <omp.h>
#endif

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN
using namespace casacore;
  //  using namespace casa::async;
rGridFT::rGridFT(Long icachesize, Int itilesize, String iconvType, Float padding,
	       Bool usezero, Bool useDoublePrec)
: FTMachine(), padding_p(padding), imageCache(0), cachesize(icachesize), tilesize(itilesize),
  gridder(0), isTiled(false), convType(iconvType),
  maxAbsData(0.0), centerLoc(IPosition(4,0)), offsetLoc(IPosition(4,0)),
  usezero_p(usezero), noPadding_p(false), usePut2_p(false), 
  machineName_p("rGridFT"), timemass_p(0.0), timegrid_p(0.0)
{
  useDoubleGrid_p=useDoublePrec;  
  //  peek=NULL;
}

rGridFT::rGridFT(Long icachesize, Int itilesize, String iconvType,
	       MPosition mLocation, Float padding, Bool usezero, Bool useDoublePrec)
: FTMachine(), padding_p(padding), imageCache(0), cachesize(icachesize),
  tilesize(itilesize), gridder(0), isTiled(false), convType(iconvType), maxAbsData(0.0), centerLoc(IPosition(4,0)),
  offsetLoc(IPosition(4,0)), usezero_p(usezero), noPadding_p(false), 
  usePut2_p(false), machineName_p("rGridFT"), timemass_p(0.0), timegrid_p(0.0)
{
  mLocation_p=mLocation;
  tangentSpecified_p=false;
  useDoubleGrid_p=useDoublePrec;
  //  peek=NULL;
}

rGridFT::rGridFT(Long icachesize, Int itilesize, String iconvType,
	       MDirection mTangent, Float padding, Bool usezero, Bool useDoublePrec)
: FTMachine(), padding_p(padding), imageCache(0), cachesize(icachesize),
  tilesize(itilesize), gridder(0), isTiled(false), convType(iconvType), maxAbsData(0.0), centerLoc(IPosition(4,0)),
  offsetLoc(IPosition(4,0)), usezero_p(usezero), noPadding_p(false), 
  usePut2_p(false), machineName_p("rGridFT"), timemass_p(0.0), timegrid_p(0.0)
{
  mTangent_p=mTangent;
  tangentSpecified_p=true;
  useDoubleGrid_p=useDoublePrec;
  //  peek=NULL;
}

rGridFT::rGridFT(Long icachesize, Int itilesize, String iconvType,
	       MPosition mLocation, MDirection mTangent, Float padding,
	       Bool usezero, Bool useDoublePrec)
: FTMachine(), padding_p(padding), imageCache(0), cachesize(icachesize),
  tilesize(itilesize), gridder(0), isTiled(false), convType(iconvType), maxAbsData(0.0), centerLoc(IPosition(4,0)),
  offsetLoc(IPosition(4,0)), usezero_p(usezero), noPadding_p(false), 
  usePut2_p(false),machineName_p("rGridFT"), timemass_p(0.0), timegrid_p(0.0)
{
  mLocation_p=mLocation;
  mTangent_p=mTangent;
  tangentSpecified_p=true;
  useDoubleGrid_p=useDoublePrec;
  //  peek=NULL;
}

rGridFT::rGridFT(const RecordInterface& stateRec)
: FTMachine()
{
  // Construct from the input state record
  String error;
  if (!fromRecord(error, stateRec)) {
    throw (AipsError("Failed to create gridder: " + error));
  };
  timemass_p=0.0;
  timegrid_p=0.0;
  //  peek=NULL;
}

//---------------------------------------------------------------------- 
rGridFT& rGridFT::operator=(const rGridFT& other)
{
  if(this!=&other) {
    //Do the base parameters
    FTMachine::operator=(other);
    
    //private params
    imageCache=other.imageCache;
    cachesize=other.cachesize;
    tilesize=other.tilesize;
    convType=other.convType;
    uvScale.resize();
    uvOffset.resize();
    uvScale=other.uvScale;
    uvOffset=other.uvOffset;
    if(other.gridder==0)
      gridder=0;
    else{  
      gridder = new ConvolveGridder<Double, Complex>(IPosition(2, nx, ny),
						     uvScale, uvOffset,
						     convType);
    }
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
    machineName_p="rGridFT";
    timemass_p=0.0;
    timegrid_p=0.0;
    //    peek = other.peek;
  };
  return *this;
};

//----------------------------------------------------------------------
rGridFT::rGridFT(const rGridFT& other) : FTMachine(), machineName_p("rGridFT")
{
  operator=(other);
}

//----------------------------------------------------------------------
void rGridFT::init() {

  logIO() << LogOrigin("rGridFT", "init")  << LogIO::NORMAL;

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
  gridder = new ConvolveGridder<Double, Complex>(IPosition(2, nx, ny),
						 uvScale, uvOffset,
						 convType);

  // Set up image cache needed for gridding. For BOX-car convolution
  // we can use non-overlapped tiles. Otherwise we need to use
  // overlapped tiles and additive gridding so that only increments
  // to a tile are written.
  if(imageCache) delete imageCache; imageCache=0;

  if(isTiled) {
    Float tileOverlap=0.5;
    if(convType=="box") {
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
rGridFT::~rGridFT() {
  if(imageCache) delete imageCache; imageCache=0;
  //if(arrayLattice) delete arrayLattice; arrayLattice=0;
  if(gridder) delete gridder; gridder=0;
}

// Initialize for a transform from the Sky domain. This means that
// we grid-correct, and FFT the image

void rGridFT::initializeToVis(ImageInterface<Complex>& iimage,
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

void rGridFT::prepGridForDegrid(){
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
   logIO() << LogIO::DEBUGGING
	  << "Starting grid correction and FFT of image" << LogIO::POST;

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
   
   // Now do the FFT2D in place
   LatticeFFT::cfft2d(*lattice);
   
    logIO() << LogIO::DEBUGGING
	    << "Finished grid correction and FFT of image" << LogIO::POST;
    
}


void rGridFT::finalizeToVis()
{
  if(isTiled) {

    logIO() << LogOrigin("rGridFT", "finalizeToVis")  << LogIO::NORMAL;

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
void rGridFT::initializeToSky(ImageInterface<Complex>& iimage,
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
    griddedData.resize(gridShape);
    griddedData=Complex(0.0);
    if(useDoubleGrid_p){
      griddedData2.resize(gridShape);
      griddedData2=DComplex(0.0);
    }
    //iimage.get(griddedData, false);
    //if(arrayLattice) delete arrayLattice; arrayLattice=0;
    arrayLattice = new ArrayLattice<Complex>(griddedData);
    lattice=arrayLattice;
  }
  //AlwaysAssert(lattice, AipsError);
}



void rGridFT::finalizeToSky()
{  
  //AlwaysAssert(lattice, AipsError);
  // Now we flush the cache and report statistics
  // For memory based, we don't write anything out yet.
  cerr <<"Time to massage data " << timemass_p << endl;
  cerr <<"Time to grid data " << timegrid_p << endl;
  timemass_p=0.0;
  timegrid_p=0.0;
  if(isTiled) {
    logIO() << LogOrigin("rGridFT", "finalizeToSky")  << LogIO::NORMAL;

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



Array<Complex>* rGridFT::getDataPointer(const IPosition& centerLoc2D,
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
#endif

extern "C" { 
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
void rGridFT::put(const VisBuffer& vb, Int row, Bool dopsf, 
		 FTMachine::Type type)
{

  gridOk(gridder->cSupport()(0));
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
  Int csamp=gridder->cSampling();
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
#pragma omp parallel default(none) private(irow) firstprivate(visfreqstor, nvchan, scalestor, offsetstor, csamp, phasorstor, uvstor, locstor, offstor, dpstor) shared(startRow, endRow)
  {
#pragma omp for
  for (irow=startRow; irow<=endRow;irow++){
    locateuvw(uvstor,dpstor, visfreqstor, nvchan, scalestor, offsetstor, csamp, 
	      locstor, 
	      offstor, phasorstor, irow);
  }  

  }//end pragma parallel
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
  
  /////////////Some extra stuff for openmp

  Int x0, y0, nxsub, nysub, ixsub, iysub, icounter, ix, iy;
  Int csupp=gridder->cSupport()(0);
  
  const Double * convfuncstor=(gridder->cFunction()).getStorage(del);
  
  // cerr <<"Poffset " << min(off) << "  " << max(off) << " length " << gridder->cFunction().shape() << endl;
  
  ixsub=2;
  iysub=2; 
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
#pragma omp parallel default(none) private(icounter,ix,iy,x0,y0,nxsub,nysub, del) firstprivate(idopsf, datStorage, wgtStorage, flagstor, rowflagstor, convfuncstor, pmapstor, cmapstor, gridstor, nxp, nyp, np, nc,ixsub, iysub, rend, rbeg, csamp, csupp, nvispol, nvischan, nvisrow, phasorstor, locstor, offstor) shared(sumwgt) 
  
  {
    //cerr << "numthreads " << omp_get_num_threads() << endl;
#pragma omp for nowait
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
#pragma omp parallel default(none) private(icounter,ix,iy,x0,y0,nxsub,nysub, del) firstprivate(idopsf, datStorage, wgtStorage, flagstor, rowflagstor, convfuncstor, pmapstor, cmapstor, gridstor, nxp, nyp, np, nc,ixsub, iysub, rend, rbeg, csamp, csupp, nvispol, nvischan, nvisrow, phasorstor, locstor, offstor) shared(sumwgt) 
    {
      //cerr << "numthreads " << omp_get_num_threads() << endl;
#pragma omp for nowait
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


void rGridFT::get(VisBuffer& vb, Int row)
{

  gridOk(gridder->cSupport()(0));
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
  Int csamp=gridder->cSampling();
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
#pragma omp parallel default(none) private(irow) firstprivate(visfreqstor, nvchan, scalestor, offsetstor, csamp, phasorstor, uvstor, locstor, offstor, dpstor) shared(startRow, endRow)
  {
#pragma omp for
  for (irow=startRow; irow<=endRow; ++irow){
    locateuvw(uvstor,dpstor, visfreqstor, nvchan, scalestor, offsetstor, csamp, 
	      locstor, 
	      offstor, phasorstor, irow);
  }  

  }//end pragma parallel

  Int rbeg=startRow+1;
  Int rend=endRow+1;


  Vector<Int> rowFlags(vb.nRow());
  rowFlags=0;
  rowFlags(vb.flagRow())=true;
  if(!usezero_p) {
    for (Int rownr=startRow; rownr<=endRow; rownr++) {
      if(vb.antenna1()(rownr)==vb.antenna2()(rownr)) rowFlags(rownr)=1;
    }
  }


  //cerr <<"offset " << min(off) << "  " <<max(off) << " length " << gridder->cFunction().shape() << endl;

  {
    Bool delgrid;
    const Complex* gridstor=griddedData.getStorage(delgrid);
    const Double * convfuncstor=(gridder->cFunction()).getStorage(del);
    
    const Int* pmapstor=polMap.getStorage(del);
    const Int *cmapstor=chanMap.getStorage(del);
    Int nc=nchan;
    Int np=npol;
    Int nxp=nx;
    Int nyp=ny;
    Int csupp=gridder->cSupport()(0);
    const Int * flagstor=flags.getStorage(del);
    const Int * rowflagstor=rowFlags.getStorage(del);


    Int npart=4;
    Int ix=0;
#pragma omp parallel default(none) private(ix, rbeg, rend) firstprivate(datStorage, flagstor, rowflagstor, convfuncstor, pmapstor, cmapstor, gridstor, nxp, nyp, np, nc, csamp, csupp, nvp, nvc, nvisrow, phasorstor, locstor, offstor) shared(npart)
    { 
#pragma omp for nowait
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
ImageInterface<Complex>& rGridFT::getImage(Matrix<Float>& weights, Bool normalize) 
{
  //AlwaysAssert(lattice, AipsError);
  AlwaysAssert(gridder, AipsError);
  AlwaysAssert(image, AipsError);
  logIO() << LogOrigin("rGridFT", "getImage") << LogIO::NORMAL;

  weights.resize(sumWeight.shape());

  convertArray(weights, sumWeight);
  // If the weights are all zero then we cannot normalize
  // otherwise we don't care.
  if(normalize&&max(weights)==0.0) {
    logIO() << LogIO::SEVERE << "No useful data in rGridFT: weights all zero"
	    << LogIO::POST;
  }
  else {

    const IPosition latticeShape = lattice->shape();
    
    logIO() << LogIO::DEBUGGING
	    << "Starting FFT and scaling of image" << LogIO::POST;
    

  
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
	convertArray(griddedData, griddedData2);
	//Don't need the double-prec grid anymore...
	griddedData2.resize();
      }
    else
      LatticeFFT::cfft2d(*lattice,false);

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
    
  return *image;
}

// Get weight image
void rGridFT::getWeightImage(ImageInterface<Float>& weightImage, Matrix<Float>& weights) 
{

  logIO() << LogOrigin("rGridFT", "getWeightImage") << LogIO::NORMAL;

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

Bool rGridFT::toRecord(String& error,
		       RecordInterface& outRec, Bool withImage, const String diskimage)
{

  

  // Save the current rGridFT object to an output state record
  Bool retval = true;

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
  outRec.define("padding", padding_p);
  outRec.define("usezero", usezero_p);
  outRec.define("nopadding", noPadding_p);
  return retval;
}

Bool rGridFT::fromRecord(String& error,
			const RecordInterface& inRec)
{
  Bool retval = true;
  if(!FTMachine::fromRecord(error, inRec))
    return false;
  gridder=0; imageCache=0; lattice=0; arrayLattice=0;
  cachesize=inRec.asInt64("cachesize");
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

  machineName_p="rGridFT";
  ///setup some of the parameters
  init();
 
  return retval;
}

void rGridFT::ok() {
  AlwaysAssert(image, AipsError);
}

// Make a plain straightforward honest-to-God image. This returns
// a complex image, without conversion to Stokes. The representation
// is that required for the visibilities.
//----------------------------------------------------------------------
void rGridFT::makeImage(FTMachine::Type type, 
		       VisSet& vs,
		       ImageInterface<Complex>& theImage,
		       Matrix<Float>& weight) {


  logIO() << LogOrigin("rGridFT", "makeImage") << LogIO::NORMAL;

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

String rGridFT::name() const{

  return machineName_p;


}

} //# NAMESPACE CASA - END

