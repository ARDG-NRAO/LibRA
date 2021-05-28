//# MosaicFT.cc: Implementation of MosaicFT class
//# Copyright (C) 2002-2016
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This library is free software; you can redistribute it and/or modify it
//# under the terms of the GNU General Public License as published by
//# the Free Software Foundation; either version 2 of the License, or (at your
//# option) any later version.
//#
//# This library is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
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

#include <casa/Quanta/UnitMap.h>
#include <casa/Quanta/MVTime.h>
#include <casa/Quanta/UnitVal.h>
#include <measures/Measures/Stokes.h>
#include <measures/Measures/UVWMachine.h>
#include <coordinates/Coordinates/CoordinateSystem.h>
#include <coordinates/Coordinates/DirectionCoordinate.h>
#include <coordinates/Coordinates/SpectralCoordinate.h>
#include <coordinates/Coordinates/StokesCoordinate.h>
#include <coordinates/Coordinates/Projection.h>
#include <ms/MeasurementSets/MSColumns.h>
#include <casa/BasicSL/Constants.h>
#include <scimath/Mathematics/FFTServer.h>
#include <synthesis/TransformMachines2/MosaicFT.h>
#include <synthesis/TransformMachines2/SimplePBConvFunc.h>
#include <synthesis/TransformMachines2/HetArrayConvFunc.h>
#include <synthesis/TransformMachines/PBMath.h>
#include <synthesis/TransformMachines2/VPSkyJones.h>
#include <scimath/Mathematics/RigidVector.h>
#include <msvis/MSVis/StokesVector.h>
#include <synthesis/TransformMachines/StokesImageUtil.h>
#include <msvis/MSVis/VisBuffer2.h>
#include <msvis/MSVis/VisibilityIterator2.h>
#include <images/Images/ImageInterface.h>
#include <images/Images/PagedImage.h>
#include <images/Images/SubImage.h>
#include <images/Regions/ImageRegion.h>
#include <images/Regions/WCBox.h>
#include <casa/Containers/Block.h>
#include <casa/Containers/Record.h>
#include <casa/Arrays/ArrayLogical.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/MaskedArray.h>
#include <casa/Arrays/Vector.h>
#include <casa/Arrays/Slice.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/Cube.h>
#include <casa/Arrays/MatrixIter.h>
#include <casa/BasicSL/String.h>
#include <casa/Utilities/Assert.h>
#include <casa/Exceptions/Error.h>
#include <lattices/Lattices/ArrayLattice.h>
#include <lattices/Lattices/SubLattice.h>
#include <lattices/LRegions/LCBox.h>
#include <lattices/LEL/LatticeExpr.h>
#include <lattices/Lattices/LatticeCache.h>
#include <lattices/LatticeMath/LatticeFFT.h>
#include <lattices/Lattices/LatticeIterator.h>
#include <lattices/Lattices/LatticeStepper.h>
#include <casa/Utilities/CompositeNumber.h>
#include <casa/OS/Timer.h>
#include <casa/OS/HostInfo.h>
#include <casa/sstream.h>
#ifdef _OPENMP
#include <omp.h>
#endif
using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN
namespace refim {//# namespace for imaging refactor
using namespace casacore;
using namespace casa;
using namespace casacore;
using namespace casa::refim;

  MosaicFT::MosaicFT(SkyJones* sj, MPosition mloc, String stokes,
		   Long icachesize, Int itilesize, 
		     Bool usezero, Bool useDoublePrec, Bool useConjConvFunc, Bool usePointing)
  : FTMachine(), sj_p(sj),
    imageCache(0),  cachesize(icachesize), tilesize(itilesize), gridder(nullptr),
    isTiled(false),
    maxAbsData(0.0), centerLoc(IPosition(4,0)), offsetLoc(IPosition(4,0)),
    mspc(0), msac(0), pointingToImage(0), usezero_p(usezero), convSampling(1),
    skyCoverage_p( ), machineName_p("MosaicFT"), stokes_p(stokes), useConjConvFunc_p(useConjConvFunc), usePointingTable_p(usePointing),timemass_p(0.0), timegrid_p(0.0), timedegrid_p(0.0)
{
  convSize=0;
  lastIndex_p=0;
  doneWeightImage_p=false;
  convWeightImage_p=0;
  pbConvFunc_p=new SimplePBConvFunc();
    
  mLocation_p=mloc;
  useDoubleGrid_p=useDoublePrec;  
  // We should get rid of the ms dependence in the constructor
  // not used
}

MosaicFT::MosaicFT(const RecordInterface& stateRec)
  : FTMachine()
{
  // Construct from the input state record
  String error;
  if (!fromRecord(error, stateRec)) {
    throw (AipsError("Failed to create MosaicFT: " + error));
  };
}

//---------------------------------------------------------------------- 
MosaicFT& MosaicFT::operator=(const MosaicFT& other)
{
  if(this!=&other) {

    //Do the base parameters
    FTMachine::operator=(other);
     
    convSampling=other.convSampling;
    sj_p=other.sj_p;
    imageCache=other.imageCache;
    cachesize=other.cachesize;
    tilesize=other.tilesize;
    isTiled=other.isTiled;
    //lattice=other.lattice;
    lattice=0;
    // arrayLattice=other.arrayLattice;
    // weightLattice=other.weightLattice;
    //if(arrayLattice) delete arrayLattice;
    arrayLattice=0;
    //if(weightLattice) delete weightLattice;
    weightLattice=0;
    maxAbsData=other.maxAbsData;
    centerLoc=other.centerLoc;
    offsetLoc=other.offsetLoc;
    pointingToImage=other.pointingToImage;
    usezero_p=other.usezero_p;
    doneWeightImage_p=other.doneWeightImage_p;
    pbConvFunc_p=other.pbConvFunc_p;
    stokes_p=other.stokes_p;
    if(!other.skyCoverage_p.null())
      skyCoverage_p=other.skyCoverage_p;
    else
      skyCoverage_p=0;
    if(other.convWeightImage_p !=0)
      convWeightImage_p=(TempImage<Complex> *)other.convWeightImage_p->cloneII();
    else
      convWeightImage_p=0;
    if(other.gridder==0)
      gridder.reset(nullptr);
    else{
      uvScale=other.uvScale;
      uvOffset=other.uvOffset;
      gridder.reset(new ConvolveGridder<Double, Complex>(IPosition(2, nx, ny),
                                                         uvScale, uvOffset,
                                                         "SF"));
	  
    }
    useConjConvFunc_p=other.useConjConvFunc_p;
    usePointingTable_p=other.usePointingTable_p;
    timemass_p=other.timemass_p;
    timegrid_p=other.timegrid_p;
    timedegrid_p=other.timedegrid_p;
  };
  return *this;
};

//----------------------------------------------------------------------
  MosaicFT::MosaicFT(const MosaicFT& other): FTMachine(),machineName_p("MosaicFT")
{
  operator=(other);
}

//----------------------------------------------------------------------
//void MosaicFT::setSharingFT(MosaicFT& otherFT){
//  otherFT_p=&otherFT;
//}
void MosaicFT::init() {
  
  /* if((image->shape().product())>cachesize) {
    isTiled=true;
  }
  else {
    isTiled=false;
  }
  */
  //For now only isTiled false works.
  isTiled=false;
  nx    = image->shape()(0);
  ny    = image->shape()(1);
  npol  = image->shape()(2);
  nchan = image->shape()(3);


  //  if(skyCoverage_p==0){
  //    Double memoryMB=HostInfo::memoryTotal()/1024.0/(20.0);
  //    skyCoverage_p=new TempImage<Float> (IPosition(4,nx,ny,1,1),
  //					image->coordinates(), memoryMB);
    //Setting it to zero
//   (*skyCoverage_p)-=(*skyCoverage_p);
//  }
  sumWeight.resize(npol, nchan);
  
  convSupport=0;

  uvScale.resize(2);
  uvScale=0.0;
  uvScale(0)=Float(nx)*image->coordinates().increment()(0); 
  uvScale(1)=Float(ny)*image->coordinates().increment()(1); 
    
  uvOffset.resize(2);
  uvOffset(0)=nx/2;
  uvOffset(1)=ny/2;
  
  gridder.reset(new ConvolveGridder<Double, Complex>(IPosition(2, nx, ny),
                                                     uvScale, uvOffset,
                                                     "SF"));

  // Set up image cache needed for gridding. 
  if(imageCache) delete imageCache; imageCache=0;
  /*
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
  */
}

// This is nasty, we should use CountedPointers here.
MosaicFT::~MosaicFT() {
  if(imageCache) delete imageCache; imageCache=0;
  //  if(arrayLattice) delete arrayLattice; arrayLattice=0;
}


void MosaicFT::setConvFunc(CountedPtr<SimplePBConvFunc>& pbconvFunc){


  pbConvFunc_p=pbconvFunc;
  

}

CountedPtr<SimplePBConvFunc>& MosaicFT::getConvFunc(){
  return pbConvFunc_p;
}

void MosaicFT::findConvFunction(const ImageInterface<Complex>& iimage,
				const vi::VisBuffer2& vb) {
  
  
  //oversample if image is small
  //But not more than 5000 pixels
  convSampling=(max(nx, ny) < 50) ? 100: Int(ceil(5000.0/max(nx, ny)));
  if(convSampling <1) 
    convSampling=1;
  if(pbConvFunc_p.null())
    pbConvFunc_p=new SimplePBConvFunc();
  if(sj_p)
    pbConvFunc_p->setSkyJones(sj_p.get());
  ////TEST for HetArray only for now
  if(pbConvFunc_p->name()=="HetArrayConvFunc"){
    if(convSampling <10) 
      convSampling=10;
    AipsrcValue<Int>::find (convSampling, "mosaic.oversampling", 10);
  }
  if((pbConvFunc_p->getVBUtil()).null()){
    if(vbutil_p.null()){
	vbutil_p=new VisBufferUtil(vb);
    }
    pbConvFunc_p->setVBUtil(vbutil_p);
  }
  pbConvFunc_p->findConvFunction(iimage, vb, convSampling, interpVisFreq_p, convFunc, weightConvFunc_p, convSizePlanes_p, convSupportPlanes_p,
				 convPolMap_p, convChanMap_p, convRowMap_p, (useConjConvFunc_p && !toVis_p), MVDirection(-(movingDirShift_p.getAngle())), fixMovingSource_p);

  // cerr << "MAX of convFunc " << max(abs(convFunc)) << endl;
  //For now only use one size and support
  if(convSizePlanes_p.nelements() ==0)
    convSize=0;
  else
    convSize=max(convSizePlanes_p);
  if(convSupportPlanes_p.nelements() ==0)
    convSupport=0;
  else
    convSupport=max(convSupportPlanes_p);
  				 
}

void MosaicFT::initializeToVis(ImageInterface<Complex>& iimage,
			       const vi::VisBuffer2& vb)
{
  image=&iimage;
  toVis_p=true;
  ok();
  
  //  if(convSize==0) {
    init();
    
    //  }
  
  // Initialize the maps for polarization and channel. These maps
  // translate visibility indices into image indices
  initMaps(vb);
  pbConvFunc_p->setVBUtil(vbutil_p);
  pbConvFunc_p->setUsePointing(usePointingTable_p);
 //make sure we rotate the first field too
  lastFieldId_p=-1;
  phaseShifter_p=new UVWMachine(*uvwMachine_p);
  //This is needed here as we need to know the grid correction before FFTing 
  findConvFunction(*image, vb);
  
  prepGridForDegrid();

}


void MosaicFT::prepGridForDegrid(){

  //For now isTiled=false
  isTiled=false;
  nx    = image->shape()(0);
  ny    = image->shape()(1);
  npol  = image->shape()(2);
  nchan = image->shape()(3);

  IPosition gridShape(4, nx, ny, npol, nchan);
  griddedData.resize(gridShape);
  griddedData=Complex(0.0);
  
  IPosition stride(4, 1);
  IPosition blc(4, (nx-image->shape()(0)+(nx%2==0))/2,
		(ny-image->shape()(1)+(ny%2==0))/2, 0, 0);
  IPosition trc(blc+image->shape()-stride);
    
  IPosition start(4, 0);
  griddedData(blc, trc) = image->getSlice(start, image->shape());
  
  image->clearCache();
    //if(arrayLattice) delete arrayLattice; arrayLattice=0;
  arrayLattice = new ArrayLattice<Complex>(griddedData);
  lattice=arrayLattice;
   {///UnDo the grid correction
      Int inx = lattice->shape()(0);
      //Int iny = lattice->shape()(1);
      Vector<Complex> correction(inx);
      correction=Complex(1.0, 0.0);

      // Int npixCorr= max(nx,ny);
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
	//Int pol=lix.position()(2);
	//Int chan=lix.position()(3);
	
	Int iy=lix.position()(1);
	//gridder->correctX1D(correction,iy);
	for (Int ix=0;ix<nx;ix++) {
	  correction(ix)=(sincConvX(ix)*sincConvY(iy));
	}
	lix.rwVectorCursor()*=correction;
	
      }
  }
    
  
  logIO() << LogIO::DEBUGGING << "Starting FFT of image" << LogIO::POST;
   // Now do the FFT2D in place
  ft_p.c2cFFT(*lattice);
  ///////////////////////
  /*{
    CoordinateSystem ftCoords(image->coordinates());
    Int directionIndex=ftCoords.findCoordinate(Coordinate::DIRECTION);
    DirectionCoordinate dc=ftCoords.directionCoordinate(directionIndex);
    Vector<Bool> axes(2); axes(0)=true;axes(1)=true;
    Vector<Int> shape(2); shape(0)=griddedData.shape()(0) ;shape(1)=griddedData.shape()(1);
    Coordinate* ftdc=dc.makeFourierCoordinate(axes,shape);
    ftCoords.replaceCoordinate(*ftdc, directionIndex);
    delete ftdc; ftdc=0;
    PagedImage<Float> thisScreen(griddedData.shape(), ftCoords, String("MODEL_GRID_VIS"));
    thisScreen.put(amplitude(griddedData));
    }*/
  ////////////////////////
  logIO() << LogIO::DEBUGGING << "Finished FFT" << LogIO::POST;



}


void MosaicFT::finalizeToVis()
{
  logIO() << LogOrigin("MosaicFT", "finalizeToVis")  << LogIO::NORMAL;
  logIO()<< LogIO::NORMAL2 << "Time degrid " << timedegrid_p << LogIO::POST;
  timedegrid_p=0.0;
  
  if(!arrayLattice.null()) arrayLattice=0;
  if(!lattice.null()) lattice=0;
  griddedData.resize();

  /*
  if(isTiled) {
    
    logIO() << LogOrigin("MosaicFT", "finalizeToVis")  << LogIO::NORMAL;
    
    AlwaysAssert(imageCache, AipsError);
    AlwaysAssert(image, AipsError);
    ostringstream o;
    imageCache->flush();
    imageCache->showCacheStatistics(o);
    logIO() << o.str() << LogIO::POST;
  }
  */
  if(pointingToImage) delete pointingToImage; pointingToImage=0;
}


// Initialize the FFT to the Sky. Here we have to setup and initialize the
// grid. 





void MosaicFT::initializeToSky(ImageInterface<Complex>& iimage,
			       Matrix<Float>& weight,
			       const vi::VisBuffer2& vb)
{
  // image always points to the image
  image=&iimage;
  toVis_p=False;
  //  if(convSize==0) {
    init();
    
    //  }
  
  // Initialize the maps for polarization and channel. These maps
  // translate visibility indices into image indices
  initMaps(vb);
  pbConvFunc_p->setVBUtil(vbutil_p);
  pbConvFunc_p->setUsePointing(usePointingTable_p);
  //make sure we rotate the first field too
  lastFieldId_p=-1;
  phaseShifter_p=new UVWMachine(*uvwMachine_p);
  //findConvFunction(*image, vb);
  /*if((image->shape().product())>cachesize) {
    isTiled=true;
  }
  else {
    isTiled=false;
  }
  */
  //For now isTiled has to be false
  isTiled=false;
  nx    = image->shape()(0);
  ny    = image->shape()(1);
  npol  = image->shape()(2);
  nchan = image->shape()(3);

  sumWeight=0.0;
  weight.resize(sumWeight.shape());
  weight=0.0;
  
  image->clearCache();
  // Initialize for in memory or to disk gridding. lattice will
  // point to the appropriate Lattice, either the ArrayLattice for
  // in memory gridding or to the image for to disk gridding.
  /*if(isTiled) {
    imageCache->flush();
    image->set(Complex(0.0));
    lattice=CountedPtr<Lattice<Complex> >(image, false);
    if( !doneWeightImage_p && (convWeightImage_p==0)){
      
      convWeightImage_p=new  TempImage<Complex> (iimage.shape(), 
						 iimage.coordinates());




      convWeightImage_p->set(Complex(0.0));
      weightLattice=convWeightImage_p;

    }
    }
    else*/ 
  {
    IPosition gridShape(4, nx, ny, npol, nchan);
    if(!useDoubleGrid_p) {
	griddedData.resize(gridShape);
	griddedData=Complex(0.0);
      }
    else {
      griddedData2.resize(gridShape);
      griddedData2=DComplex(0.0);
    }
    //if(arrayLattice) delete arrayLattice; arrayLattice=0;
    //arrayLattice = new ArrayLattice<Complex>(griddedData);
    //lattice=arrayLattice;
      
    if( !doneWeightImage_p && (convWeightImage_p==0)){
     
      
 
      convWeightImage_p=new  TempImage<Complex> (iimage.shape(), 
						 iimage.coordinates());
      griddedWeight.resize(gridShape);
      /*IPosition stride(4, 1);
      IPosition blc(4, (nx-image->shape()(0)+(nx%2==0))/2,
		    (ny-image->shape()(1)+(ny%2==0))/2, 0, 0);
      IPosition trc(blc+image->shape()-stride);
      
      griddedWeight(blc, trc).set(Complex(0.0));
      */
      if(useDoubleGrid_p){
	griddedWeight2.resize(gridShape);
	griddedWeight2=DComplex(0.0);
      }
      else{
	griddedWeight=Complex(0.0);
      }
      //if(weightLattice) delete weightLattice; weightLattice=0;
      weightLattice = new ArrayLattice<Complex>(griddedWeight);

    }

  }

  //cerr << "initializetosky lastfield " << lastFieldId_p << endl;
  // AlwaysAssert(lattice, AipsError);
  
}

void MosaicFT::reset(){
  //call the base class reset
  FTMachine::reset();
  doneWeightImage_p=false;
  convWeightImage_p=nullptr;
  pbConvFunc_p->reset();
}

void MosaicFT::finalizeToSky()
{
  logIO() << LogOrigin("MosaicFT", "finalizeToSky")  << LogIO::NORMAL;
  logIO() << LogIO::NORMAL2 << "time to massage data " << timemass_p << LogIO::POST;
  logIO() << LogIO::NORMAL2<< "time gridding " << timegrid_p << LogIO::POST;
   timemass_p=0.0;
   timegrid_p=0.0;
  // Now we flush the cache and report statistics
  // For memory based, we don't write anything out yet.
  /*if(isTiled) {
    logIO() << LogOrigin("MosaicFT", "finalizeToSky")  << LogIO::NORMAL;
    
    AlwaysAssert(image, AipsError);
    AlwaysAssert(imageCache, AipsError);
    imageCache->flush();
    ostringstream o;
    imageCache->showCacheStatistics(o);
    logIO() << o.str() << LogIO::POST;
  }
  */
  
  
  if(!doneWeightImage_p){
    if(useDoubleGrid_p){
      convertArray(griddedWeight, griddedWeight2);
      //Don't need the double-prec grid anymore...
      griddedWeight2.resize();
    }
    ft_p.c2cFFT(*weightLattice, false);
    //Get the stokes right
    CoordinateSystem coords=convWeightImage_p->coordinates();
    Int stokesIndex=coords.findCoordinate(Coordinate::STOKES);
    Int npol=1;
    Vector<Int> whichStokes(npol);
    if(stokes_p=="I" || stokes_p=="RR" || stokes_p=="LL" ||stokes_p=="XX" 
       || stokes_p=="YY" || stokes_p=="Q" || stokes_p=="U" || stokes_p=="V"){
      npol=1;
      whichStokes(0)=Stokes::type(stokes_p);
       // if single plane Q U or V are used...the weight should be the I weight
      //if(stokes_p=="Q" || stokes_p=="U" || stokes_p=="V")
      //whichStokes(0)=Stokes::type("I");
    }
    else if(stokes_p=="IV"){
      npol=2;
      whichStokes.resize(2);
      whichStokes(0)=Stokes::I;
      whichStokes(1)=Stokes::V;
    }
    else if(stokes_p=="QU"){
      npol=2;
      whichStokes.resize(2);
      whichStokes(0)=Stokes::Q;
      whichStokes(1)=Stokes::U;
    }
    else if(stokes_p=="RRLL"){
      npol=2;
      whichStokes.resize(2);
      whichStokes(0)=Stokes::RR;
      whichStokes(1)=Stokes::LL;
    }   
    else if(stokes_p=="XXYY"){
      npol=2;
      whichStokes.resize(2);
      whichStokes(0)=Stokes::XX;
      whichStokes(1)=Stokes::YY;
    }  
    else if(stokes_p=="IQU"){
      npol=3;
      whichStokes.resize(3);
      whichStokes(0)=Stokes::I;
      whichStokes(1)=Stokes::Q;
      whichStokes(2)=Stokes::U;
    }
    else if(stokes_p=="IQUV"){
      npol=4;
      whichStokes.resize(4);
      whichStokes(0)=Stokes::I;
      whichStokes(1)=Stokes::Q;
      whichStokes(2)=Stokes::U;
      whichStokes(3)=Stokes::V;
    } 
    
    StokesCoordinate newStokesCoord(whichStokes);
    coords.replaceCoordinate(newStokesCoord, stokesIndex);
    IPosition imshp=convWeightImage_p->shape();
    imshp(2)=npol;


    skyCoverage_p=new TempImage<Float> (imshp, coords,1.0);
    IPosition blc(4, (nx-image->shape()(0)+(nx%2==0))/2,
		    (ny-image->shape()(1)+(ny%2==0))/2, 0, 0);
    IPosition stride(4, 1);
    IPosition trc(blc+image->shape()-stride);
    
    // Do the copy
    IPosition start(4, 0);
    convWeightImage_p->put(griddedWeight(blc, trc));
    StokesImageUtil::ToStokesPSF(*skyCoverage_p, *convWeightImage_p);
    if(npol>1){
      // only the I get it right Q and U or V may end up with zero depending 
      // if RR or XX
      blc(0)=0; blc(1)=0; blc(3)=0;blc(2)=0;
      trc=skyCoverage_p->shape()-stride;
      trc(2)=0;
      SubImage<Float> isubim(*skyCoverage_p, Slicer(blc, trc, Slicer::endIsLast));
      for (Int k=1; k < npol; ++k){
	blc(2)=k; trc(2)=k;
	SubImage<Float> quvsubim(*skyCoverage_p, Slicer(blc, trc, Slicer::endIsLast), true);
	quvsubim.copyData(isubim);
      }

    }
    //Store this image in the pbconvfunc object as
    //it can be used for rescaling or shared by other ftmachines that use
    //this pbconvfunc
    pbConvFunc_p->setWeightImage(skyCoverage_p);
    if(convWeightImage_p) delete convWeightImage_p;
    convWeightImage_p=0;
    doneWeightImage_p=true;

    /*
    if(0){
      PagedImage<Float> thisScreen(skyCoverage_p->shape(), 
				   skyCoverage_p->coordinates(), "Screen");
      thisScreen.copyData(*skyCoverage_p);
    }
    */

  }

  if(!weightLattice.null()) weightLattice=0;
  griddedWeight.resize();
  if(pointingToImage) delete pointingToImage; pointingToImage=0;
}

void MosaicFT::setWeightImage(CountedPtr<TempImage<Float> >& wgtimage){
  skyCoverage_p=wgtimage;
  Record rec=skyCoverage_p->miscInfo();
  //For mosaicFTNew it has the nx*ny factor already in
  rec.define("isscaled", True);
  skyCoverage_p->setMiscInfo(rec);
  //cerr << "IN SET " << max(wgtimage->get()) << endl;
  pbConvFunc_p->setWeightImage(skyCoverage_p);
  doneWeightImage_p=true;
}

Array<Complex>* MosaicFT::getDataPointer(const IPosition& centerLoc2D,
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
#define sectgmoss2 sectgmoss2_
#define gmoss2 gmoss2_
#define sectgmosd2 sectgmosd2_
#define gmosd2 gmosd2_
#define sectdmos2 sectdmos2_
#define dmos2 dmos2_
#define gmoswgtd gmoswgtd_
#define gmoswgts gmoswgts_
#define locuvw locuvw_
#endif

extern "C" { 
  void locuvw(const Double*, const Double*, const Double*, const Int*, const Double*, const Double*, const Int*, 
	      Int*, Int*, Complex*, const Int*, const Int*, const Double*);
  void gmoswgtd(const Int*/*nvispol*/, const Int*/*nvischan*/,
		const Int*/*flag*/, const Int*/*rflag*/, const Float*/*weight*/, const Int*/*nrow*/, 
		const Int*/*nx*/, const Int*/*ny*/, const Int*/*npol*/, const Int*/*nchan*/, 
		const Int*/*support*/, const Int*/*convsize*/, const Int*/*sampling*/, 
		const Int*/*chanmap*/, const Int*/*polmap*/,
		DComplex* /*weightgrid*/, const Complex*/*convweight*/, const Int*/*convplanemap*/, 
		const Int*/*convchanmap*/,  const Int*/*convpolmap*/, 
		const Int*/*nconvplane*/, const Int*/*nconvchan*/, const Int*/*nconvpol*/, const Int*/*rbeg*/, 
		const Int*/*rend*/, const Int*/*loc*/, const Int*/*off*/, const Complex*/*phasor*/);
 void gmoswgts(const Int*/*nvispol*/, const Int*/*nvischan*/,
		const Int*/*flag*/, const Int*/*rflag*/, const Float*/*weight*/, const Int*/*nrow*/, 
		const Int*/*nx*/, const Int*/*ny*/, const Int*/*npol*/, const Int*/*nchan*/, 
		const Int*/*support*/, const Int*/*convsize*/, const Int*/*sampling*/, 
		const Int*/*chanmap*/, const Int*/*polmap*/,
		Complex* /*weightgrid*/, const Complex*/*convweight*/, const Int*/*convplanemap*/, 
		const Int*/*convchanmap*/,  const Int*/*convpolmap*/, 
		const Int*/*nconvplane*/, const Int*/*nconvchan*/, const Int*/*nconvpol*/, const Int*/*rbeg*/, 
		const Int*/*rend*/, const Int*/*loc*/, const Int*/*off*/, const Complex*/*phasor*/);
  void sectgmosd2(const Complex* /*values*/,
		  Int* /*nvispol*/, Int* /*nvischan*/,
		  Int* /*dopsf*/, const Int* /*flag*/, const Int* /*rflag*/, const Float* /*weight*/,
		  Int* /* nrow*/, DComplex* /*grid*/, Int* /*nx*/, Int* /*ny*/, Int * /*npol*/, Int * /*nchan  */,
		  Int*/*support*/, Int*/*convsize*/, Int*/*sampling*/, const Complex*/*convfunc*/,
		  const Int*/*chanmap*/, const Int*/*polmap*/,
		  Double*/*sumwgt*/, const Int*/*convplanemap*/,
		  const Int*/*convchanmap*/, const Int*/*convpolmap*/, 
		  Int*/*nconvplane*/, Int*/*nconvchan*/, Int* /*nconvpol*/,
		  const Int*/*x0*/,const Int*/*y0*/, const Int*/*nxsub*/, const Int*/*nysub*/, const Int*/*rbeg*/, 
		  const Int* /*rend*/, const Int*/*loc*/, const Int* /*off*/, const Complex*/*phasor*/);     

 void sectgmoss2(const Complex* /*values*/,
		  Int* /*nvispol*/, Int* /*nvischan*/,
		  Int* /*dopsf*/, const Int* /*flag*/, const Int* /*rflag*/, const Float* /*weight*/,
		  Int* /* nrow*/, Complex* /*grid*/, Int* /*nx*/, Int* /*ny*/, Int * /*npol*/, Int * /*nchan  */,
		  Int*/*support*/, Int*/*convsize*/, Int*/*sampling*/, const Complex*/*convfunc*/,
		  const Int*/*chanmap*/, const Int*/*polmap*/,
		  Double*/*sumwgt*/, const Int*/*convplanemap*/,
		  const Int*/*convchanmap*/, const Int*/*convpolmap*/, 
		  Int*/*nconvplane*/, Int*/*nconvchan*/, Int* /*nconvpol*/,
		  const Int*/*x0*/,const Int*/*y0*/, const Int*/*nxsub*/, const Int*/*nysub*/, const Int*/*rbeg*/, 
		  const Int* /*rend*/, const Int*/*loc*/, const Int* /*off*/, const Complex*/*phasor*/);     


  void gmosd2(const Double*,
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
  	      const Complex*,
  	      Int*,
  	      Int*,
  	      Double*,
  	      DComplex*,
  	      Complex*,
  	      Int*,
  	      Int*,
  	      Int*,Int*, Int*, Int*, Int*);
  /*  void gmoss(const Double*,
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
    	      const Complex*,
    	      Int*,
    	      Int*,
    	      Double*,
    	      Complex*,
    	      Complex*,
    	      Int*,
    	      Int*,
    	      Int*);
  */
    void gmoss2(const Double*,
  	      Double*,
  	      const Complex*,
  	      Int*,
  	      Int*,
  	      Int*,
  	      const Int*,
  	      const Int*,
  	      const Float*,
  	      Int*, //10
  	      Int*,
  	      Double*,
  	      Double*,
  	      Complex*,
  	      Int*,
  	      Int*,
  	      Int *,
  	      Int *,
  	      const Double*,
  	      const Double*, //20
  	      Int*,
  	      Int*,
  	      Int*,
  	      const Complex*,
  	      Int*,
  	      Int*,
  	      Double*,
  	      Complex*,
  	      Complex*,
  	      Int*, //30
  	      Int*,
  	      Int*, Int*, Int*, Int*, Int*);

  /* void dmos(const Double*,
	      Double*,
	      Complex*,
	      Int*,
	      Int*,
	      const Int*,
	      const Int*,
	      Int*,
	      Int*,
	    Double*, //10
	      Double*,
	      const Complex*,
	      Int*,
	      Int*,
	      Int *,
	      Int *,
	      const Double*,
	      const Double*,
	      Int*,
	    Int*,//20
	      Int*,
	      const Complex*,
	      Int*,
	      Int*,
              Int*,
              Int*);
  */

  void dmos2(const Double*,
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
  	      const Complex*,
  	      Int*,
  	      Int*,
  	      Int*,
  	      Int*, Int*, Int*, Int*, Int*);
  void sectdmos2(Complex*,
  	      Int*,
  	      Int*,
  	      const Int*,
  	      const Int*,
		 Int*,
  	      const Complex*,
  	      Int*,
  	      Int*,
  	      Int *,
		 Int *,
  	      Int*,
  	      Int*,
  	      Int*,
  	      const Complex*,
  	      const Int*,
  	      const Int*,
  	      const Int*,
	      const  Int*, 
	      const Int*, 
	      Int*, Int*, Int*,
		 //rbeg
		 const Int*,
		 const Int*,
		 const Int*,
		 const Int*,
		 const Complex*);

	     

}
void MosaicFT::put(const vi::VisBuffer2& vb, Int row, Bool dopsf,
		   FTMachine::Type type)
{


  
  
  Timer tim;
  tim.mark();
 
  matchChannel(vb);
 

  //cerr << "CHANMAP " << chanMap << endl;
  //No point in reading data if its not matching in frequency
  if(max(chanMap)==-1)
    return;

  //const Matrix<Float> *imagingweight;
  //imagingweight=&(vb.imagingWeight());
  Matrix<Float> imagingweight;
  getImagingWeight(imagingweight, vb);

  if(dopsf) type=FTMachine::PSF;

  Cube<Complex> data;
  //Fortran gridder need the flag as ints 
  Cube<Int> flags;
  Matrix<Float> elWeight;
  interpolateFrequencyTogrid(vb, imagingweight,data, flags, elWeight, type);
  
 

  Bool iswgtCopy;
  const Float *wgtStorage;
  wgtStorage=elWeight.getStorage(iswgtCopy);


  

  Bool isCopy;
  const Complex *datStorage=0;

  // cerr << "dopsf " << dopsf << " isWeightCopy " << iswgtCopy << "  " << wgtStorage<< endl;
  if(!dopsf)
    datStorage=data.getStorage(isCopy);
    
  
  // If row is -1 then we pass through all rows
  Int startRow, endRow, nRow;
  if (row==-1) {
    nRow=vb.nRows();
    startRow=0;
    endRow=nRow-1;
  } else {
    nRow=1;
    startRow=row;
    endRow=row;
  }
  
  // Get the uvws in a form that Fortran can use and do that
  // necessary phase rotation. 
  Matrix<Double> uvw(negateUV(vb));
  Vector<Double> dphase(vb.nRows());
  dphase=0.0;
 
  doUVWRotation_p=true;
  girarUVW(uvw, dphase, vb);
  refocus(uvw, vb.antenna1(), vb.antenna2(), dphase, vb);
  // This needs to be after the interp to get the interpolated channels
  //Also has to be after rotateuvw in case tracking is on
  findConvFunction(*image, vb);
  //nothing to grid here as the pointing resulted in a zero support convfunc
  if(convSupport <= 0)
    return;
  
  // Get the pointing positions. This can easily consume a lot 
  // of time thus we are for now assuming a field per 
  // vb chunk...need to change that accordingly if we start using
  // multiple pointings per vb.
  //Warning 

  // Take care of translation of Bools to Integer
  Int idopsf=0;
  if(dopsf) idopsf=1;
  
  
  Vector<Int> rowFlags(vb.nRows());
  rowFlags=0;
  rowFlags(vb.flagRow())=true;
  if(!usezero_p) {
    for (Int rownr=startRow; rownr<=endRow; rownr++) {
      if(vb.antenna1()(rownr)==vb.antenna2()(rownr)) rowFlags(rownr)=1;
    }
  }
  
  


  //Tell the gridder to grid the weights too ...need to do that once only
  //Int doWeightGridding=1;
  //if(doneWeightImage_p)
  //  doWeightGridding=-1;
  Bool del;
  //    IPosition s(flags.shape());
  const IPosition& fs=flags.shape();
  //cerr << "flags shape " << fs << endl;
  std::vector<Int>s(fs.begin(), fs.end());
  Int nvp=s[0];
  Int nvc=s[1];
  Int nvisrow=s[2];
  Int csamp=convSampling;
  Bool uvwcopy; 
  const Double *uvwstor=uvw.getStorage(uvwcopy);
  Bool gridcopy;
  Bool convcopy;
  Bool wconvcopy;
  const Complex *convstor=convFunc.getStorage(convcopy);
  const Complex *wconvstor=weightConvFunc_p.getStorage(wconvcopy);
  Int nPolConv=convFunc.shape()[2];
  Int nChanConv=convFunc.shape()[3];
  Int nConvFunc=convFunc.shape()(4);
  Bool weightcopy;
  ////////**************************
  Cube<Int> loc(2, nvc, nRow);
  Cube<Int> off(2, nvc, nRow);
  Matrix<Complex> phasor(nvc, nRow);
  Bool delphase;
  Complex * phasorstor=phasor.getStorage(delphase);
  const Double * visfreqstor=interpVisFreq_p.getStorage(del);
  const Double * scalestor=uvScale.getStorage(del);
  const Double * offsetstor=uvOffset.getStorage(del);
  Int * locstor=loc.getStorage(del);
  Int * offstor=off.getStorage(del);
  const Double *dpstor=dphase.getStorage(del);
  Int irow;
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
  Double cinv=Double(1.0)/C::c;
 
  Int dow=0;
#pragma omp parallel default(none) private(irow) firstprivate(visfreqstor, nvc, scalestor, offsetstor, csamp, phasorstor, uvwstor, locstor, offstor, dpstor, dow, cinv) shared(startRow, endRow) num_threads(nth)  
{
#pragma omp for
  for (irow=startRow; irow<=endRow;irow++){
    /*locateuvw(uvwstor,dpstor, visfreqstor, nvc, scalestor, offsetstor, csamp, 
	      locstor, 
	      offstor, phasorstor, irow, false);*/
    locuvw(uvwstor, dpstor, visfreqstor, &nvc, scalestor, offsetstor, &csamp, locstor, offstor, phasorstor, &irow, &dow, &cinv);
  }  

 }//end pragma parallel


 
timemass_p +=tim.real();
Int  ixsub, iysub, icounter;
  ixsub=1;
  iysub=1;
  //////***********************DEBUGGING
  //nth=1;
  ////////***************
  if (nth >3){
    ixsub=8;
    iysub=8; 
  }
  else if(nth >1){
     ixsub=2;
     iysub=2; 
  }
  Int rbeg=startRow+1;
  Int rend=endRow+1;
  Block<Matrix<Double> > sumwgt(ixsub*iysub);
  Vector<Double *> swgtptr(ixsub*iysub);
  Vector<Bool> swgtdel(ixsub*iysub);
  for (icounter=0; icounter < ixsub*iysub; ++icounter){
    sumwgt[icounter].resize(sumWeight.shape());
    sumwgt[icounter].set(0.0);
    swgtptr[icounter]=sumwgt[icounter].getStorage(swgtdel(icounter));
  }
  //cerr << "done thread " << doneThreadPartition_p << "  " << ixsub*iysub << endl;
   if(doneThreadPartition_p < 0){
    xsect_p.resize(ixsub*iysub);
    ysect_p.resize(ixsub*iysub);
    nxsect_p.resize(ixsub*iysub);
    nysect_p.resize(ixsub*iysub);
    for (icounter=0; icounter < ixsub*iysub; ++icounter){
      findGridSector(nx, ny, ixsub, iysub, 0, 0, icounter, xsect_p(icounter), ysect_p(icounter), nxsect_p(icounter), nysect_p(icounter), true);
    }
  }
   Vector<Int> xsect, ysect, nxsect, nysect;
   xsect=xsect_p; ysect=ysect_p; nxsect=nxsect_p; nysect=nysect_p;
   //cerr << xsect.shape() << "  " << xsect << endl;
  const Int* pmapstor=polMap.getStorage(del);
  const Int* cmapstor=chanMap.getStorage(del);
  Int nc=nchan;
  Int np=npol;
  Int nxp=nx;
  Int nyp=ny;
  Int csize=convSize;
  const Int * flagstor=flags.getStorage(del);
  const Int * rowflagstor=rowFlags.getStorage(del);
  Int csupp=convSupport;
  const Int *convrowmapstor=convRowMap_p.getStorage(del);
  const Int *convchanmapstor=convChanMap_p.getStorage(del);
  const Int *convpolmapstor=convPolMap_p.getStorage(del);
  ////////***************************
  tim.mark(); 

  if(useDoubleGrid_p) {
    DComplex *gridstor=griddedData2.getStorage(gridcopy);
    
#pragma omp parallel default(none) private(icounter, del) firstprivate(idopsf, /*doWeightGridding,*/ datStorage, wgtStorage, flagstor, rowflagstor, convstor, wconvstor, pmapstor, cmapstor, gridstor,  csupp, nxp, nyp, np, nc,ixsub, iysub, rend, rbeg, csamp, csize, nvp, nvc, nvisrow, phasorstor, locstor, offstor, convrowmapstor, convchanmapstor, convpolmapstor, nPolConv, nChanConv, nConvFunc,xsect, ysect, nxsect, nysect) shared(swgtptr) 
    {   
#pragma omp for schedule(dynamic)      
    for(icounter=0; icounter < ixsub*iysub; ++icounter){
      Int x0=xsect(icounter);
      Int y0=ysect(icounter);
      Int nxsub=nxsect(icounter);
      Int nysub=nysect(icounter);
      
      /*
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
      */

    sectgmosd2(datStorage,
	   &nvp,
	   &nvc,
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
	   &csize,
	   &csamp,
	   convstor,
	   cmapstor,
	   pmapstor,
	   swgtptr[icounter],
	   convrowmapstor,
	   convchanmapstor,
	   convpolmapstor,
	       &nConvFunc, &nChanConv, &nPolConv,
	       &x0, &y0, &nxsub, &nysub, &rbeg, &rend, locstor, offstor,
		 phasorstor
	       );
    }
    }//end pragma parallel
    for (icounter=0; icounter < ixsub*iysub; ++icounter){
      sumwgt[icounter].putStorage(swgtptr[icounter],swgtdel[icounter]);
      sumWeight=sumWeight+sumwgt[icounter];
    }    
    
    griddedData2.putStorage(gridstor, gridcopy);
    if(dopsf && (nth >4))
      tweakGridSector(nx, ny, ixsub, iysub);
    timegrid_p+=tim.real();
    tim.mark();
    if(!doneWeightImage_p){
      //This can be parallelized by making copy of the central part of the griddedWeight
      //and adding it after dooing the gridding
      DComplex *gridwgtstor=griddedWeight2.getStorage(weightcopy);
      gmoswgtd(&nvp, &nvc,flagstor, rowflagstor, wgtStorage, &nvisrow, 
	       &nxp, &nyp, &np, &nc, &csupp, &csize, &csamp, 
	       cmapstor, pmapstor,
	       gridwgtstor, wconvstor, convrowmapstor, 
	       convchanmapstor,  convpolmapstor, 
	       &nConvFunc, &nChanConv, &nPolConv, &rbeg, 
	       &rend, locstor, offstor, phasorstor);
      griddedWeight2.putStorage(gridwgtstor, weightcopy);
    
    }
    timemass_p+=tim.real();
  }
  else {
    //cerr << "maps "  << convChanMap_p << "   " << chanMap  << endl;
    //cerr << "nchan " << nchan << "  nchanconv " << nChanConv << endl;
    Complex *gridstor=griddedData.getStorage(gridcopy);
#pragma omp parallel default(none) private(icounter, del) firstprivate(idopsf, /*doWeightGridding,*/ datStorage, wgtStorage, flagstor, rowflagstor, convstor, wconvstor, pmapstor, cmapstor, gridstor, csupp, nxp, nyp, np, nc,ixsub, iysub, rend, rbeg, csamp, csize, nvp, nvc, nvisrow, phasorstor, locstor, offstor, convrowmapstor, convchanmapstor, convpolmapstor, nPolConv, nChanConv, nConvFunc, xsect, ysect, nxsect, nysect)  shared(swgtptr) 
    {   
#pragma omp for schedule(dynamic)      
      for(icounter=0; icounter < ixsub*iysub; ++icounter){
	/*ix= (icounter+1)-((icounter)/ixsub)*ixsub;
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
	*/
	Int x0=xsect(icounter);
	Int y0=ysect(icounter);
	Int nxsub=nxsect(icounter);
	Int nysub=nysect(icounter);

	   sectgmoss2(datStorage,
	   &nvp,
	   &nvc,
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
	   &csize,
	   &csamp,
	   convstor,
	   cmapstor,
	   pmapstor,
	   swgtptr[icounter],
	   convrowmapstor,
	   convchanmapstor,
	   convpolmapstor,
	       &nConvFunc, &nChanConv, &nPolConv,
	       &x0, &y0, &nxsub, &nysub, &rbeg, &rend, locstor, offstor,
		 phasorstor
	       );


    }
    } //end pragma   
     for (icounter=0; icounter < ixsub*iysub; ++icounter){
       sumwgt[icounter].putStorage(swgtptr[icounter],swgtdel[icounter]);
       sumWeight=sumWeight+sumwgt[icounter];
    }
    griddedData.putStorage(gridstor, gridcopy);
    if(dopsf && (nth > 4))
      tweakGridSector(nx, ny, ixsub, iysub);
    timegrid_p+=tim.real();
    tim.mark();
    if(!doneWeightImage_p){
      Complex *gridwgtstor=griddedWeight.getStorage(weightcopy);
      gmoswgts(&nvp, &nvc,flagstor, rowflagstor, wgtStorage, &nvisrow, 
	       &nxp, &nyp, &np, &nc, &csupp, &csize, &csamp, 
	       cmapstor, pmapstor,
	       gridwgtstor, wconvstor, convrowmapstor, 
	       convchanmapstor,  convpolmapstor, 
	       &nConvFunc, &nChanConv, &nPolConv, &rbeg, 
	       &rend, locstor, offstor, phasorstor);
      griddedWeight.putStorage(gridwgtstor, weightcopy);
    
    }
    timemass_p+=tim.real();
  }
  convFunc.freeStorage(convstor, convcopy);
  weightConvFunc_p.freeStorage(wconvstor, wconvcopy);
  uvw.freeStorage(uvwstor, uvwcopy);
  if(!dopsf)
    data.freeStorage(datStorage, isCopy);

  elWeight.freeStorage(wgtStorage,iswgtCopy);
  



}


void MosaicFT::get(vi::VisBuffer2& vb, Int row)
{
  

  
  // If row is -1 then we pass through all rows
  Int startRow, endRow, nRow;
  if (row==-1) {
    nRow=vb.nRows();
    startRow=0;
    endRow=nRow-1;
    //  vb.modelVisCube()=Complex(0.0,0.0);
  } else {
    nRow=1;
    startRow=row;
    endRow=row;
    //  vb.modelVisCube().xyPlane(row)=Complex(0.0,0.0);
  }
  

 

  matchChannel(vb);
 
  //No point in reading data if its not matching in frequency
  if(max(chanMap)==-1)
    return;

  // Get the uvws in a form that Fortran can use
  Matrix<Double> uvw(negateUV(vb));
  Vector<Double> dphase(vb.nRows());
  dphase=0.0;
 
  doUVWRotation_p=true;
  girarUVW(uvw, dphase, vb);
  refocus(uvw, vb.antenna1(), vb.antenna2(), dphase, vb);
  
  
  
  
  Cube<Complex> data;
  Cube<Int> flags;
  getInterpolateArrays(vb, data, flags);
  //Need to get interpolated freqs
  findConvFunction(*image, vb);
  // no valid pointing in this buffer
  if(convSupport <= 0)
    return;
  Complex *datStorage;
  Bool isCopy;
  datStorage=data.getStorage(isCopy);
  

  Vector<Int> rowFlags(vb.nRows());
  rowFlags=0;
  rowFlags(vb.flagRow())=true;
  if(!usezero_p) {
    for (Int rownr=startRow; rownr<=endRow; rownr++) {
      if(vb.antenna1()(rownr)==vb.antenna2()(rownr)) rowFlags(rownr)=1;
    }
  }
  Int nvp=data.shape()[0];
  Int nvc=data.shape()[1];
  Int nvisrow=data.shape()[2];
  Int csamp=convSampling;
  Int csize=convSize;
  Int csupp=convSupport;
  Int nc=nchan;
  Int np=npol;
  Int nxp=nx;
  Int nyp=ny;
  Bool uvwcopy; 
  const Double *uvwstor=uvw.getStorage(uvwcopy);
  Int nPolConv=convFunc.shape()[2];
  Int nChanConv=convFunc.shape()[3];
  Int nConvFunc=convFunc.shape()(4);
  ////////**************************
  Cube<Int> loc(2, nvc, nRow);
  Cube<Int> off(2, nvc, nRow);
  Matrix<Complex> phasor(nvc, nRow);
  Bool delphase;
  Bool del;
  const Int* pmapstor=polMap.getStorage(del);
  const Int* cmapstor=chanMap.getStorage(del);
  Complex * phasorstor=phasor.getStorage(delphase);
  const Double * visfreqstor=interpVisFreq_p.getStorage(del);
  const Double * scalestor=uvScale.getStorage(del);
  const Double * offsetstor=uvOffset.getStorage(del);
  const Int * flagstor=flags.getStorage(del);
  const Int * rowflagstor=rowFlags.getStorage(del);
  Int * locstor=loc.getStorage(del);
  Int * offstor=off.getStorage(del);
  const Double *dpstor=dphase.getStorage(del);
  const Int *convrowmapstor=convRowMap_p.getStorage(del);
  const Int *convchanmapstor=convChanMap_p.getStorage(del);
  const Int *convpolmapstor=convPolMap_p.getStorage(del);
  ////////***************************

  Int irow;
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

  Timer tim;
  tim.mark();

   Int dow=0;
   Double cinv=Double(1.0)/C::c;
#pragma omp parallel default(none) private(irow) firstprivate(visfreqstor, nvc, scalestor, offsetstor, csamp, phasorstor, uvwstor, locstor, offstor, dpstor, dow, cinv) shared(startRow, endRow) num_threads(nth)  
{
#pragma omp for
  for (irow=startRow; irow<=endRow;irow++){
    /////////////////*locateuvw(uvwstor,dpstor, visfreqstor, nvc, scalestor, offsetstor, csamp, 
    //    locstor, 
		///////////	      offstor, phasorstor, irow, false);
    //using the fortran version which is significantly faster ...this can account for 10% less overall degridding time
    locuvw(uvwstor, dpstor, visfreqstor, &nvc, scalestor, offsetstor, &csamp, locstor, 
	   offstor, phasorstor, &irow, &dow, &cinv);
  }  

 }//end pragma parallel
 Int rbeg=startRow+1;
 Int rend=endRow+1;
 Int npart=nth;
 
 Bool gridcopy;
 const Complex *gridstor=griddedData.getStorage(gridcopy);
 Bool convcopy;
 ////Degridding needs the conjugate ...doing it here
 Array<Complex> conjConvFunc=conj(convFunc);
 const Complex *convstor=conjConvFunc.getStorage(convcopy);
  Int ix=0;
#pragma omp parallel default(none) private(ix, rbeg, rend) firstprivate(uvwstor, datStorage, flagstor, rowflagstor, convstor, pmapstor, cmapstor, gridstor, nxp, nyp, np, nc, csamp, csize, csupp, nvp, nvc, nvisrow, phasorstor, locstor, offstor, nPolConv, nChanConv, nConvFunc, convrowmapstor, convpolmapstor, convchanmapstor, npart)  num_threads(npart)
  {
    #pragma omp for schedule(dynamic) 
    for (ix=0; ix< npart; ++ix){
      rbeg=ix*(nvisrow/npart)+1;
      rend=(ix != (npart-1)) ? (rbeg+(nvisrow/npart)-1) : (rbeg+(nvisrow/npart)-1+nvisrow%npart) ;
      //cerr << "maps "  << convChanMap_p << "   " << chanMap  << endl;
      //cerr << "nchan " << nchan << "  nchanconv " << nChanConv << " npolconv " << nPolConv << " nRowConv " << nConvFunc << endl;
     sectdmos2(
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
	       &csupp,
	       &csize,   
	       &csamp,
	       convstor,
	       cmapstor,
	       pmapstor,
	       convrowmapstor, convchanmapstor,
	       convpolmapstor,
	       &nConvFunc, &nChanConv, &nPolConv,
	       &rbeg, &rend, locstor, offstor, phasorstor
	       );


    }
  }//end pragma omp


  data.putStorage(datStorage, isCopy);
  griddedData.freeStorage(gridstor, gridcopy);
  convFunc.freeStorage(convstor, convcopy);
  
   timedegrid_p+=tim.real();

  interpolateFrequencyFromgrid(vb, data, FTMachine::MODEL);
}


/*
void MosaicFT::get(VisBuffer& vb, Int row)
{
  

  
  // If row is -1 then we pass through all rows
  Int startRow, endRow, nRow;
  if (row==-1) {
    nRow=vb.nRow();
    startRow=0;
    endRow=nRow-1;
    //  vb.modelVisCube()=Complex(0.0,0.0);
  } else {
    nRow=1;
    startRow=row;
    endRow=row;
    //  vb.modelVisCube().xyPlane(row)=Complex(0.0,0.0);
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
  
  doUVWRotation_p=true;
  girarUVW(uvw, dphase, vb);
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
  //No point in reading data if its not matching in frequency
  if(max(chanMap)==-1)
    return;

  Cube<Complex> data;
  Cube<Int> flags;
  getInterpolateArrays(vb, data, flags);
  //Need to get interpolated freqs
  findConvFunction(*image, vb);
  // no valid pointing in this buffer
  if(convSupport <= 0)
    return;
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
  Int nPolConv=convFunc.shape()[2];
  Int nChanConv=convFunc.shape()[3];
  Int nConvFunc=convFunc.shape()(4);
 
  {
    Bool del;
     Bool uvwcopy; 
     const Double *uvwstor=uvw.getStorage(uvwcopy);
     Bool gridcopy;
     const Complex *gridstor=griddedData.getStorage(gridcopy);
     Bool convcopy;
     ////Degridding needs the conjugate ...doing it here
     Array<Complex> conjConvFunc=conj(convFunc);
     const Complex *convstor=conjConvFunc.getStorage(convcopy);
     //     IPosition s(data.shape());
     const IPosition& fs=data.shape();
     std::vector<Int> s(fs.begin(), fs.end());
     //cerr << "maps "  << convChanMap_p << "   " << chanMap  << endl;
     //cerr << "nchan " << nchan << "  nchanconv " << nChanConv << " npolconv " << nPolConv << " nRowConv " << nConvFunc << endl;
     dmos2(uvwstor,
	    dphase.getStorage(del),
	    datStorage,
	    &s[0],
	    &s[1],
	    flags.getStorage(del),
	    rowFlags.getStorage(del),
	    &s[2],
	    &row,
	   uvScale.getStorage(del), //10
	    uvOffset.getStorage(del),
	    gridstor,
	    &nx,
	    &ny,
	    &npol,
	    &nchan,
	    interpVisFreq_p.getStorage(del),
	    &C::c,
	    &convSupport,
	   &convSize,   //20
	    &convSampling,
	    convstor,
	    chanMap.getStorage(del),
	    polMap.getStorage(del),
	    convRowMap_p.getStorage(del), convChanMap_p.getStorage(del),
	    convPolMap_p.getStorage(del),
	    &nConvFunc, &nChanConv, &nPolConv);
      data.putStorage(datStorage, isCopy);
     uvw.freeStorage(uvwstor, uvwcopy);
     griddedData.freeStorage(gridstor, gridcopy);
     convFunc.freeStorage(convstor, convcopy);
  }
  interpolateFrequencyFromgrid(vb, data, FTMachine::MODEL);
}

*/


// Finalize the FFT to the Sky. Here we actually do the FFT and
// return the resulting image
ImageInterface<Complex>& MosaicFT::getImage(Matrix<Float>& weights,
					    Bool normalize) 
{
  //AlwaysAssert(lattice, AipsError);
  AlwaysAssert(image, AipsError);

   if((griddedData.nelements() ==0) && (griddedData2.nelements()==0)){
    throw(AipsError("Programmer error ...request for image without right order of calls"));
  }

  logIO() << LogOrigin("MosaicFT", "getImage") << LogIO::NORMAL;
  
  weights.resize(sumWeight.shape());
  convertArray(weights, sumWeight);
  SynthesisUtilMethods::getResource("mem peak in getImage");
  
  // If the weights are all zero then we cannot normalize
  // otherwise we don't care.
  if(max(weights)==0.0) {
    if(normalize) {
      logIO() << LogIO::SEVERE << "No useful data in MosaicFT: weights all zero"
	      << LogIO::POST;
    }
    else {
      logIO() << LogIO::WARN << "No useful data in MosaicFT: weights all zero"
	      << LogIO::POST;
    }
  }
  else {
    
    //const IPosition latticeShape = lattice->shape();
    
    logIO() << LogIO::DEBUGGING
	    << "Starting FFT and scaling of image" << LogIO::POST;
    if(useDoubleGrid_p){
      ArrayLattice<DComplex> darrayLattice(griddedData2);
      ft_p.c2cFFT(darrayLattice,false);
      griddedData.resize(griddedData2.shape());
      convertArray(griddedData, griddedData2);
      
      //Don't need the double-prec grid anymore...
      griddedData2.resize();
      arrayLattice = new ArrayLattice<Complex>(griddedData);
      lattice=arrayLattice;

    }
    else{
      arrayLattice = new ArrayLattice<Complex>(griddedData);
      lattice=arrayLattice;
      ft_p.c2cFFT(*lattice,false);
    }
   {////Do the grid correction
      Int inx = lattice->shape()(0);
      //Int iny = lattice->shape()(1);
      Vector<Complex> correction(inx);
      correction=Complex(1.0, 0.0);

      // Int npixCorr= max(nx,ny);
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
	  //gridder->correctX1D(correction,iy);
	  for (Int ix=0;ix<nx;ix++) {
	    correction(ix)=1.0/(sincConvX(ix)*sincConvY(iy));
	  }
	  lix.rwVectorCursor()*=correction;
	  if(normalize) {
	    Complex rnorm(1.0/weights(pol,chan));
	    lix.rwCursor()*=rnorm;
	  }
	}
	else {
	  lix.woCursor()=0.0;
	}
      }
    }
     
    

    //if(!isTiled) 
    {
      LatticeLocker lock1 (*(image), FileLocker::Write);
      // Check the section from the image BEFORE converting to a lattice 
      IPosition blc(4, (nx-image->shape()(0)+(nx%2==0))/2,
		    (ny-image->shape()(1)+(ny%2==0))/2, 0, 0);
      IPosition stride(4, 1);
      IPosition trc(blc+image->shape()-stride);
      
      // Do the copy
      IPosition start(4, 0);
      image->put(griddedData(blc, trc));
    }
  }
  if(!arrayLattice.null()) arrayLattice=0;
  if(!lattice.null()) lattice=0;
   griddedData.resize();
  image->clearCache();
  return *image;
}

// Get weight image
void MosaicFT::getWeightImage(ImageInterface<Float>& weightImage,
			      Matrix<Float>& weights) 
{
  
  logIO() << LogOrigin("MosaicFT", "getWeightImage") << LogIO::NORMAL;
  
  weights.resize(sumWeight.shape());
  convertArray(weights,sumWeight);
  /*
  weightImage.copyData((LatticeExpr<Float>) 
		       (iif((pbConvFunc_p->getFluxScaleImage()) > (0.0), 
			    (*skyCoverage_p),0.0)));
  */
  weightImage.copyData(*skyCoverage_p);
  //cerr << "getWeightIm " << max(sumWeight) << "    " << max(skyCoverage_p->get()) << endl;
  
   skyCoverage_p->tempClose();

}

void MosaicFT::getFluxImage(ImageInterface<Float>& fluxImage) {

  if (stokes_p=="QU" || stokes_p=="IV"){
    pbConvFunc_p->sliceFluxScale(2);
  }
  else if(stokes_p=="Q" || stokes_p=="U" ||  stokes_p=="V" || stokes_p=="I" ){
     pbConvFunc_p->sliceFluxScale(1);
  }
   else if(stokes_p=="IQU"){
       pbConvFunc_p->sliceFluxScale(3);
   }
  IPosition inShape=(pbConvFunc_p->getFluxScaleImage()).shape();
  IPosition outShape=fluxImage.shape();
  if(outShape==inShape){
    fluxImage.copyData(pbConvFunc_p->getFluxScaleImage());
  }
  else if((outShape(0)==inShape(0)) && (outShape(1)==inShape(1)) 
	  && (outShape(2)==inShape(2))){
    //case where CubeSkyEquation is chunking...copy the first pol-cube
    IPosition cursorShape(4, inShape(0), inShape(1), inShape(2), 1);
    IPosition axisPath(4, 0, 1, 2, 3);
    LatticeStepper lsout(outShape, cursorShape, axisPath);
    LatticeStepper lsin(inShape, cursorShape, axisPath);
    LatticeIterator<Float> liout(fluxImage, lsout);
    RO_LatticeIterator<Float> liin(pbConvFunc_p->getFluxScaleImage(), lsin);
    liin.reset();
    for(liout.reset();!liout.atEnd();liout++) {
      if(inShape(2)==1)
	liout.woMatrixCursor()=liin.matrixCursor();
      else
	liout.woCubeCursor()=liin.cubeCursor();
    }


  }
  else{
    //Should not reach here but the we're getting old
    cout << "Bad case of shape mismatch in flux image shape" << endl;
  }
}

CountedPtr<TempImage<Float> >& MosaicFT::getConvWeightImage(){
  if(!doneWeightImage_p)
    finalizeToSky();
  return skyCoverage_p;
}

Bool MosaicFT::toRecord(String&  error,
			RecordInterface& outRec, Bool withImage, const String diskimage)
{  
  // Save the current MosaicFT object to an output state record
  Bool retval = true;
  if(!FTMachine::toRecord(error, outRec, withImage, diskimage))
    return false;
  
  if(sj_p){
    outRec.define("telescope", sj_p->telescope());
    //cerr <<" Telescope " << sj_p->telescope() << endl;
  }
  outRec.define("uvscale", uvScale);
  outRec.define("uvoffset", uvOffset);
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
  outRec.define("usezero", usezero_p);
  outRec.define("convfunc", convFunc);
  outRec.define("weightconvfunc", weightConvFunc_p);
  outRec.define("convsampling", convSampling);
  outRec.define("convsize", convSize);
  outRec.define("convsupport", convSupport);
  outRec.define("convsupportplanes", convSupportPlanes_p);
  outRec.define("convsizeplanes", convSizePlanes_p);
  outRec.define("convRowMap",  convRowMap_p);
  outRec.define("stokes", stokes_p);
  outRec.define("useconjconvfunc", useConjConvFunc_p);
  outRec.define("usepointingtable", usePointingTable_p);
  if(!pbConvFunc_p.null()){
    Record subRec;
    //cerr << "Doing pbconvrec " << endl;
    pbConvFunc_p->toRecord(subRec);
    outRec.defineRecord("pbconvfunc", subRec);	
  }
  

  return retval;
}

Bool MosaicFT::fromRecord(String& error,
			  const RecordInterface& inRec)
{
  Bool retval = true;
  pointingToImage=0;
  doneWeightImage_p=false;
  convWeightImage_p=nullptr;
  
  if(!FTMachine::fromRecord(error, inRec))
    return false;
  sj_p=0;
  if(inRec.isDefined("telescope")){
    String tel=inRec.asString("telescope");
    PBMath::CommonPB pbtype;
    Quantity freq(1e12, "Hz");// no useful band...just get default beam
    String band="";
    String pbname;
    PBMath::whichCommonPBtoUse(tel, freq, band, pbtype, pbname);
    if(pbtype != PBMath::UNKNOWN)
      sj_p.reset(new VPSkyJones(tel,pbtype));
  }

  inRec.get("name", machineName_p);
  inRec.get("uvscale", uvScale);
  inRec.get("uvoffset", uvOffset);
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
  imageCache=0; lattice=0; arrayLattice=0;
  inRec.get("usezero", usezero_p);
  inRec.get("convfunc", convFunc);
  inRec.get("weightconvfunc", weightConvFunc_p);
  inRec.get("convsampling", convSampling);
  inRec.get("convsize", convSize);
  inRec.get("convsupport", convSupport);
  inRec.get("convsupportplanes", convSupportPlanes_p);
  inRec.get("convsizeplanes", convSizePlanes_p);
  inRec.get("convRowMap",  convRowMap_p);
  inRec.get("stokes", stokes_p);
  inRec.get("useconjconvfunc", useConjConvFunc_p);
  inRec.get("usepointingtable", usePointingTable_p);
  if(inRec.isDefined("pbconvfunc")){
    Record subRec=inRec.asRecord("pbconvfunc");
    String elname=subRec.asString("name");
    // if we are predicting only ...no need to estimate fluxscale
    if(elname=="HetArrayConvFunc"){
    
      pbConvFunc_p=new HetArrayConvFunc(subRec, !toVis_p);
    }
    else{
      pbConvFunc_p=new SimplePBConvFunc(subRec, !toVis_p);
      if(!sj_p)
	throw(AipsError("Failed to recovermosaic FTmachine;\n If you are seeing this message when try to get model vis \n then either try to reset the model or use scratch column for now"));
    }
  }
  else{
    pbConvFunc_p=0;
  }
  gridder.reset(nullptr);
   return retval;
}

void MosaicFT::ok() {
  AlwaysAssert(image, AipsError);
}

// Make a plain straightforward honest-to-God image. This returns
// a complex image, without conversion to Stokes. The representation
// is that required for the visibilities.
//----------------------------------------------------------------------
void MosaicFT::makeImage(FTMachine::Type type, 
			 vi::VisibilityIterator2& vi,
			 ImageInterface<Complex>& theImage,
			 Matrix<Float>& weight) {
  
  
  logIO() << LogOrigin("MosaicFT", "makeImage") << LogIO::NORMAL;
  
  if(type==FTMachine::COVERAGE) {
    logIO() << "Type COVERAGE not defined for Fourier transforms"
	    << LogIO::EXCEPTION;
  }
  
  

  // Loop over all visibilities and pixels
  vi::VisBuffer2* vb=vi.getVisBuffer();
  
  // Initialize put (i.e. transform to Sky) for this model
  vi.origin();
  
  if(vb->polarizationFrame()==MSIter::Linear) {
    StokesImageUtil::changeCStokesRep(theImage, StokesImageUtil::LINEAR);
  }
  else {
    StokesImageUtil::changeCStokesRep(theImage, StokesImageUtil::CIRCULAR);
  }
  
  initializeToSky(theImage,weight,*vb);
  
  // Loop over the visibilities, putting VisBuffers
  for (vi.originChunks();vi.moreChunks();vi.nextChunk()) {
    for (vi.origin(); vi.more(); vi.next()) {
      
      switch(type) {
      case FTMachine::RESIDUAL:
	vb->setVisCube(vb->visCubeCorrected()-vb->visCubeModel());
        put(*vb, -1, false);
        break;
      case FTMachine::MODEL:
	vb->setVisCube(vb->visCubeModel());
        put(*vb, -1, false);
        break;
      case FTMachine::CORRECTED:
	vb->setVisCube(vb->visCubeCorrected());
        put(*vb, -1, false);
        break;
      case FTMachine::PSF:
	vb->setVisCube(Complex(1.0,0.0));
        put(*vb, -1, true);
        break;
      case FTMachine::OBSERVED:
      default:
        put(*vb, -1, false);
        break;
      }
    }
  }
  finalizeToSky();
  // Normalize by dividing out weights, etc.
  getImage(weight, true);
}

Bool MosaicFT::getXYPos(const vi::VisBuffer2& vb, Int row) {
  
  MSColumns mscol(vb.ms());
  const MSPointingColumns& act_mspc=mscol.pointing();
  Int pointIndex=getIndex(act_mspc, vb.time()(row), vb.timeInterval()(row));
  if((pointIndex<0)||pointIndex>=Int(act_mspc.time().nrow())) {
    //    ostringstream o;
    //    o << "Failed to find pointing information for time " <<
    //      MVTime(vb.time()(row)/86400.0);
    //    logIO_p << LogIO::DEBUGGING << String(o) << LogIO::POST;
    //    logIO_p << String(o) << LogIO::POST;
    
    worldPosMeas = mscol.field().phaseDirMeas(vb.fieldId()(0));
  }
  else {
   
      worldPosMeas=act_mspc.directionMeas(pointIndex);
      // Make a machine to convert from the worldPosMeas to the output
      // Direction Measure type for the relevant frame
 

 
  }

  if(!pointingToImage) {
    // Set the frame - choose the first antenna. For e.g. VLBI, we
    // will need to reset the frame per antenna
    FTMachine::mLocation_p=mscol.antenna().positionMeas()(0);
    mFrame_p=MeasFrame(MEpoch(Quantity(vb.time()(row), "s")),
		       FTMachine::mLocation_p);
    MDirection::Ref outRef(directionCoord.directionType(), mFrame_p);
    pointingToImage = new MDirection::Convert(worldPosMeas, outRef);
  
    if(!pointingToImage) {
      logIO_p << "Cannot make direction conversion machine" << LogIO::EXCEPTION;
    }
  }
  else {
    mFrame_p.resetEpoch(MEpoch(Quantity(vb.time()(row), "s")));
  }
  
  worldPosMeas=(*pointingToImage)(worldPosMeas);
 
  Bool result=directionCoord.toPixel(xyPos, worldPosMeas);
  if(!result) {
    logIO_p << "Failed to find pixel location for " 
	    << worldPosMeas.getAngle().getValue() << LogIO::EXCEPTION;
    return false;
  }
  return result;
  
}
// Get the index into the pointing table for this time. Note that the 
// in the pointing table, TIME specifies the beginning of the spanned
// time range, whereas for the main table, TIME is the centroid.
// Note that the behavior for multiple matches is not specified! i.e.
// if there are multiple matches, the index returned depends on the
// history of previous matches. It is deterministic but not obvious.
// One could cure this by searching but it would be considerably
// costlier.
Int MosaicFT::getIndex(const MSPointingColumns& mspc, const Double& time,
		       const Double& /*interval*/) {
  Int start=lastIndex_p;
  // Search forwards
  Int nrows=mspc.time().nrow();
  if(nrows<1) {
    //    logIO_p << "No rows in POINTING table - cannot proceed" << LogIO::EXCEPTION;
    return -1;
  }
  for (Int i=start;i<nrows;i++) {
    Double midpoint = mspc.time()(i); // time in POINTING table is midpoint
    // If the interval in the pointing table is negative, use the last
    // entry. Note that this may be invalid (-1) but in that case 
    // the calling routine will generate an error
    if(mspc.interval()(i)<0.0) {
      return lastIndex_p;
    }
    // Pointing table interval is specified so we have to do a match
    else {
      // Is the midpoint of this pointing table entry within the specified
      // tolerance of the main table entry?
      if(abs(midpoint-time) < (mspc.interval()(i)/2.0)) {
	lastIndex_p=i;
	return i;
      }
    }
  }
  // Search backwards
  for (Int i=start;i>=0;i--) {
    Double midpoint = mspc.time()(i); // time in POINTING table is midpoint
    if(mspc.interval()(i)<0.0) {
      return lastIndex_p;
    }
    // Pointing table interval is specified so we have to do a match
    else {
      // Is the midpoint of this pointing table entry within the specified
      // tolerance of the main table entry?
      if(abs(midpoint-time) < (mspc.interval()(i)/2.0)) {
	lastIndex_p=i;
	return i;
      }
    }
  }
  // No match!
  return -1;
}




void MosaicFT::addBeamCoverage(ImageInterface<Complex>& pbImage){

  CoordinateSystem cs(pbImage.coordinates());
  //  IPosition blc(4,0,0,0,0);
  //  IPosition trc(pbImage.shape());
  //  trc(0)=trc(0)-1;
  //  trc(1)=trc(1)-1;
  // trc(2)=0;
  //  trc(3)=0;
  WCBox *wbox= new WCBox(LCBox(pbImage.shape()), cs);
  SubImage<Float> toAddTo(*skyCoverage_p, ImageRegion(wbox), true);
  TempImage<Float> beamStokes(pbImage.shape(), cs);
  StokesImageUtil::To(beamStokes, pbImage);
  //  toAddTo.copyData((LatticeExpr<Float>)(toAddTo + beamStokes ));
  skyCoverage_p->copyData((LatticeExpr<Float>)(*skyCoverage_p + beamStokes ));


}





String MosaicFT::name() const {
  return machineName_p;
}

} // REFIM ends
} //# NAMESPACE CASA - END


