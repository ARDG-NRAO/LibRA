//# SDGrid.cc: Implementation of SDGrid class
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

#include <casa/Arrays/Array.h>
#include <casa/Arrays/ArrayLogical.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/Cube.h>
#include <casa/Arrays/MaskedArray.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/MatrixIter.h>
#include <casa/Arrays/Slice.h>
#include <casa/Arrays/Vector.h>
#include <casa/BasicSL/Constants.h>
#include <casa/BasicSL/String.h>
#include <casa/Containers/Block.h>
#include <casa/Exceptions/Error.h>
#include <casa/OS/Timer.h>
#include <casa/Quanta/MVAngle.h>
#include <casa/Quanta/MVTime.h>
#include <casa/Quanta/UnitMap.h>
#include <casa/Quanta/UnitVal.h>
#include <casa/sstream.h>
#include <casa/Utilities/Assert.h>

#include <components/ComponentModels/ConstantSpectrum.h>
#include <components/ComponentModels/Flux.h>
#include <components/ComponentModels/PointShape.h>

#include <coordinates/Coordinates/CoordinateSystem.h>
#include <coordinates/Coordinates/DirectionCoordinate.h>
#include <coordinates/Coordinates/Projection.h>
#include <coordinates/Coordinates/SpectralCoordinate.h>
#include <coordinates/Coordinates/StokesCoordinate.h>

#include <images/Images/ImageInterface.h>
#include <images/Images/PagedImage.h>
#include <images/Images/TempImage.h>

#include <lattices/Lattices/ArrayLattice.h>
#include <lattices/Lattices/LatticeCache.h>
#include <lattices/Lattices/LatticeIterator.h>
#include <lattices/Lattices/LatticeStepper.h>
#include <lattices/Lattices/SubLattice.h>
#include <lattices/LEL/LatticeExpr.h>
#include <lattices/LRegions/LCBox.h>

#include <measures/Measures/Stokes.h>
#include <ms/MeasurementSets/MSColumns.h>
#include <msvis/MSVis/StokesVector.h>
#include <msvis/MSVis/VisBuffer2.h>
#include <msvis/MSVis/VisibilityIterator2.h>
#include <scimath/Mathematics/RigidVector.h>
#include <synthesis/TransformMachines2/SDGrid.h>
#include <synthesis/TransformMachines2/SkyJones.h>
#include <synthesis/TransformMachines/StokesImageUtil.h>

using namespace casacore;
namespace casa {
namespace refim {//# namespace for imaging refactor

SDGrid::SDGrid(SkyJones& sj, Int icachesize, Int itilesize,
	       String iconvType, Int userSupport, Bool useImagingWeight)
  : FTMachine(), sj_p(&sj), imageCache(0), wImageCache(0),
  cachesize(icachesize), tilesize(itilesize),
  isTiled(false), wImage(0), arrayLattice(0),  wArrayLattice(0), lattice(0), wLattice(0), convType(iconvType),
    pointingToImage(0), userSetSupport_p(userSupport),
    truncate_p(-1.0), gwidth_p(0.0), jwidth_p(0.0),
    minWeight_p(0.), lastIndexPerAnt_p(), useImagingWeight_p(useImagingWeight), lastAntID_p(-1), msId_p(-1),
    isSplineInterpolationReady(false), interpolator(0), clipminmax_(false)
{
  lastIndex_p=0;
}

SDGrid::SDGrid(MPosition& mLocation, SkyJones& sj, Int icachesize, Int itilesize,
	       String iconvType, Int userSupport, Float minweight, Bool clipminmax, Bool useImagingWeight)
  : FTMachine(), sj_p(&sj), imageCache(0), wImageCache(0),
  cachesize(icachesize), tilesize(itilesize),
  isTiled(false), wImage(0), arrayLattice(0),  wArrayLattice(0), lattice(0), wLattice(0), convType(iconvType),
    pointingToImage(0), userSetSupport_p(userSupport),
    truncate_p(-1.0), gwidth_p(0.0),  jwidth_p(0.0),
    minWeight_p(minweight), lastIndexPerAnt_p(), useImagingWeight_p(useImagingWeight), lastAntID_p(-1), msId_p(-1),
    isSplineInterpolationReady(false), interpolator(0), clipminmax_(clipminmax)
{
  mLocation_p=mLocation;
  lastIndex_p=0;
}

SDGrid::SDGrid(Int icachesize, Int itilesize,
	       String iconvType, Int userSupport, Bool useImagingWeight)
  : FTMachine(), sj_p(0), imageCache(0), wImageCache(0),
  cachesize(icachesize), tilesize(itilesize),
  isTiled(false), wImage(0), arrayLattice(0),  wArrayLattice(0), lattice(0), wLattice(0), convType(iconvType),
    pointingToImage(0), userSetSupport_p(userSupport),
    truncate_p(-1.0), gwidth_p(0.0), jwidth_p(0.0),
    minWeight_p(0.), lastIndexPerAnt_p(), useImagingWeight_p(useImagingWeight), lastAntID_p(-1), msId_p(-1),
    isSplineInterpolationReady(false), interpolator(0), clipminmax_(false)
{
  lastIndex_p=0;
}

SDGrid::SDGrid(MPosition &mLocation, Int icachesize, Int itilesize,
	       String iconvType, Int userSupport, Float minweight, Bool clipminmax, Bool useImagingWeight)
  : FTMachine(), sj_p(0), imageCache(0), wImageCache(0),
  cachesize(icachesize), tilesize(itilesize),
  isTiled(false), wImage(0), arrayLattice(0),  wArrayLattice(0), lattice(0), wLattice(0), convType(iconvType),
    pointingToImage(0), userSetSupport_p(userSupport),
    truncate_p(-1.0), gwidth_p(0.0), jwidth_p(0.0),
    minWeight_p(minweight), lastIndexPerAnt_p(), useImagingWeight_p(useImagingWeight), lastAntID_p(-1),
    msId_p(-1),
    isSplineInterpolationReady(false), interpolator(0), clipminmax_(clipminmax)
{
  mLocation_p=mLocation;
  lastIndex_p=0;
}

SDGrid::SDGrid(MPosition &mLocation, Int icachesize, Int itilesize,
	       String iconvType, Float truncate, Float gwidth, Float jwidth,
	       Float minweight, Bool clipminmax, Bool useImagingWeight)
  : FTMachine(), sj_p(0), imageCache(0), wImageCache(0),
  cachesize(icachesize), tilesize(itilesize),
  isTiled(false), wImage(0), arrayLattice(0),  wArrayLattice(0), lattice(0), wLattice(0), convType(iconvType),
    pointingToImage(0), userSetSupport_p(-1),
    truncate_p(truncate), gwidth_p(gwidth), jwidth_p(jwidth),
    minWeight_p(minweight), lastIndexPerAnt_p(), useImagingWeight_p(useImagingWeight), lastAntID_p(-1), msId_p(-1),
    isSplineInterpolationReady(false), interpolator(0), clipminmax_(clipminmax)
{
  mLocation_p=mLocation;
  lastIndex_p=0;
}


//----------------------------------------------------------------------
SDGrid& SDGrid::operator=(const SDGrid& other)
{
  if(this!=&other) {
     //Do the base parameters
    FTMachine::operator=(other);
    sj_p=other.sj_p;
    imageCache=other.imageCache;
    wImage=other.wImage;
    wImageCache=other.wImageCache;
    cachesize=other.cachesize;
    tilesize=other.tilesize;
    isTiled=other.isTiled;
    lattice=other.lattice;
    arrayLattice=other.arrayLattice;
    wLattice=other.wLattice;
    wArrayLattice=other.wArrayLattice;
    convType=other.convType;
    if(other.wImage !=0)
      wImage=(TempImage<Float> *)other.wImage->cloneII();
    else
      wImage=0;
    pointingDirCol_p=other.pointingDirCol_p;
    pointingToImage=0;
    xyPos.resize();
    xyPos=other.xyPos;
    xyPosMovingOrig_p=other.xyPosMovingOrig_p;
    convFunc.resize();
    convFunc=other.convFunc;
    convSampling=other.convSampling;
    convSize=other.convSize;
    convSupport=other.convSupport;
    userSetSupport_p=other.userSetSupport_p;
    lastIndex_p=0;
    lastIndexPerAnt_p=0;
    lastAntID_p=-1;
    msId_p=-1;
    useImagingWeight_p=other.useImagingWeight_p;
    clipminmax_=other.clipminmax_;
  };
  return *this;
};

String SDGrid::name() const{
    return String("SDGrid");
}

//----------------------------------------------------------------------
// Odds are that it changed.....
Bool SDGrid::changed(const vi::VisBuffer2& /*vb*/) {
  return false;
}

//----------------------------------------------------------------------
SDGrid::SDGrid(const SDGrid& other):FTMachine()
{
  operator=(other);
}

#define NEED_UNDERSCORES
#if defined(NEED_UNDERSCORES)
#define grdsf grdsf_
#define grdgauss grdgauss_
#define grdjinc1 grdjinc1_
#endif

extern "C" {
   void grdsf(Double*, Double*);
   void grdgauss(Double*, Double*, Double*);
   void grdjinc1(Double*, Double*, Int*, Double*);
}

//----------------------------------------------------------------------
void SDGrid::init() {

  logIO() << LogOrigin("SDGrid", "init")  << LogIO::NORMAL;

  //pfile = fopen("ptdata.txt","w");

  ok();

  /*if((image->shape().product())>cachesize) {
    isTiled=true;
  }
  else {
    isTiled=false;
    }*/
  isTiled=false;
  nx    = image->shape()(0);
  ny    = image->shape()(1);
  npol  = image->shape()(2);
  nchan = image->shape()(3);

  sumWeight.resize(npol, nchan);

  // Set up image cache needed for gridding. For BOX-car convolution
  // we can use non-overlapped tiles. Otherwise we need to use
  // overlapped tiles and additive gridding so that only increments
  // to a tile are written.
  if(imageCache) delete imageCache; imageCache=0;

  convType=downcase(convType);
  logIO() << "Convolution function : " << convType << LogIO::DEBUG1 << LogIO::POST;
  if(convType=="pb") {
    //cerr << "CNVFunc " << convFunc << endl;

  }
  else if(convType=="box") {
    convSupport=(userSetSupport_p >= 0) ? userSetSupport_p : 0;
    logIO() << "Support : " << convSupport << " pixels" << LogIO::POST;
    convSampling=100;
    convSize=convSampling*(2*convSupport+2);
    convFunc.resize(convSize);
    convFunc=0.0;
    for (Int i=0;i<convSize/2;i++) {
      convFunc(i)=1.0;
    }
  }
  else if(convType=="sf") {
    // SF
    convSupport=(userSetSupport_p >= 0) ? userSetSupport_p : 3;
    logIO() << "Support : " << convSupport << " pixels" << LogIO::POST;
    convSampling=100;
    convSize=convSampling*(2*convSupport+2);
    convFunc.resize(convSize);
    convFunc=0.0;
    for (Int i=0;i<convSampling*convSupport;i++) {
      Double nu=Double(i)/Double(convSupport*convSampling);
      Double val;
      grdsf(&nu, &val);
      convFunc(i)=(1.0-nu*nu)*val;
    }
  }
  else if(convType=="gauss") {
    // default is b=1.0 (Mangum et al. 2007)
    Double hwhm=(gwidth_p > 0.0) ? Double(gwidth_p) : sqrt(log(2.0));
    Float truncate=(truncate_p >= 0.0) ? truncate_p : 3.0*hwhm;
    convSampling=100;
    Int itruncate=(Int)(truncate*Double(convSampling)+0.5);
    logIO() << LogIO::DEBUG1 << "hwhm=" << hwhm << LogIO::POST;
    logIO() << LogIO::DEBUG1 << "itruncate=" << itruncate << LogIO::POST;
    //convSupport=(Int)(truncate+0.5);
    convSupport = (Int)(truncate);
    convSupport += (((truncate-(Float)convSupport) > 0.0) ? 1 : 0);
    convSize=convSampling*(2*convSupport+2);
    convFunc.resize(convSize);
    convFunc=0.0;
    Double val, x;
    for (Int i = 0 ; i <= itruncate ; i++) {
      x = Double(i)/Double(convSampling);
      grdgauss(&hwhm, &x, &val);
      convFunc(i)=val;
    }

//     String outfile = convType + ".dat";
//     ofstream ofs(outfile.c_str());
//     for (Int i = 0 ; i < convSize ; i++) {
//       ofs << i << " " << convFunc[i] << endl;
//     }
//     ofs.close();
  }
  else if (convType=="gjinc") {
    // default is b=2.52, c=1.55 (Mangum et al. 2007)
    Double hwhm = (gwidth_p > 0.0) ? Double(gwidth_p) : sqrt(log(2.0))*2.52;
    Double c = (jwidth_p > 0.0) ? Double(jwidth_p) : 1.55;
    //Float truncate = truncate_p;
    convSampling = 100;
    Int itruncate=(Int)(truncate_p*Double(convSampling)+0.5);
    logIO() << LogIO::DEBUG1 << "hwhm=" << hwhm << LogIO::POST;
    logIO() << LogIO::DEBUG1 << "c=" << c << LogIO::POST;
    logIO() << LogIO::DEBUG1 << "itruncate=" << itruncate << LogIO::POST;
    //convSupport=(truncate_p >= 0.0) ? (Int)(truncate_p+0.5) : (Int)(2*c+0.5);
    Float convSupportF = (truncate_p >= 0.0) ? truncate_p : (2*c);
    convSupport = (Int)convSupportF;
    convSupport += (((convSupportF-(Float)convSupport) > 0.0) ? 1 : 0);
    convSize=convSampling*(2*convSupport+2);
    convFunc.resize(convSize);
    convFunc=0.0;
    //UNUSED: Double r;
    Double x, val1, val2;
    Int normalize = 1;
    if (itruncate >= 0) {
      for (Int i = 0 ; i < itruncate ; i++) {
        x = Double(i) / Double(convSampling);
        //r = Double(i) / (Double(hwhm)*Double(convSampling));
        grdgauss(&hwhm, &x, &val1);
        grdjinc1(&c, &x, &normalize, &val2);
        convFunc(i) = val1 * val2;
      }
    }
    else {
      // default is to truncate at first null
      for (Int i=0;i<convSize;i++) {
        x = Double(i) / Double(convSampling);
        //r = Double(i) / (Double(hwhm)*Double(convSampling));
        grdjinc1(&c, &x, &normalize, &val2);
        if (val2 <= 0.0) {
          logIO() << LogIO::DEBUG1 << "convFunc is automatically truncated at radius " << x << LogIO::POST;
          break;
        }
        grdgauss(&hwhm, &x, &val1);
        convFunc(i) = val1 * val2;
      }
    }

//    String outfile = convType + ".dat";
//    ofstream ofs(outfile.c_str());
//    for (Int i = 0 ; i < convSize ; i++) {
//      ofs << i << " " << convFunc[i] << endl;
//    }
//    ofs.close();
  }
  else {
    logIO_p << "Unknown convolution function " << convType << LogIO::EXCEPTION;
  }

  if(wImage) delete wImage; wImage=0;
  wImage = new TempImage<Float>(image->shape(), image->coordinates());

  /*if(isTiled) {
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
    imageCache=new LatticeCache <Complex> (*image, cachesize, tileShape,
					   tileOverlapVec,
					   (tileOverlap>0.0));

    wImageCache=new LatticeCache <Float> (*wImage, cachesize, tileShape,
					   tileOverlapVec,
					   (tileOverlap>0.0));

  }
  */
}

// This is nasty, we should use CountedPointers here.
SDGrid::~SDGrid() {
  //fclose(pfile);
  if (imageCache) delete imageCache; imageCache = 0;
  if (arrayLattice) delete arrayLattice; arrayLattice = 0;
  if (wImage) delete wImage; wImage = 0;
  if (wImageCache) delete wImageCache; wImageCache = 0;
  if (wArrayLattice) delete wArrayLattice; wArrayLattice = 0;
  if (interpolator) delete interpolator; interpolator = 0;
}

void SDGrid::findPBAsConvFunction(const ImageInterface<Complex>& image,
				  const vi::VisBuffer2& vb) {

  // Get the coordinate system and increase the sampling by
  // a factor of ~ 100.
  CoordinateSystem coords(image.coordinates());

  // Set up the convolution function: make the buffer plenty
  // big so that we can trim it back
  convSupport=max(128, sj_p->support(vb, coords));

  convSampling=100;
  convSize=convSampling*convSupport;

  // Make a one dimensional image to calculate the
  // primary beam. We oversample this by a factor of
  // convSampling.
  Int directionIndex=coords.findCoordinate(Coordinate::DIRECTION);
  AlwaysAssert(directionIndex>=0, AipsError);
  DirectionCoordinate dc=coords.directionCoordinate(directionIndex);
  Vector<Double> sampling;
  sampling = dc.increment();
  sampling*=1.0/Double(convSampling);
  dc.setIncrement(sampling);

  // Set the reference value to the first pointing in the coordinate
  // system used in the POINTING table.
  {
    uInt row = 0;

    // reset lastAntID_p to use correct antenna position
    lastAntID_p = -1;

    const MSPointingColumns& act_mspc = vb.subtableColumns().pointing();
    Bool nullPointingTable = (act_mspc.nrow() < 1);
    Int pointIndex = -1;
    if (!nullPointingTable){
      //if(vb.newMS()) This thing is buggy...using msId change
      if (vb.msId() != msId_p) {
	lastIndex_p=0;
	if (lastIndexPerAnt_p.nelements() < (size_t)vb.nAntennas()) {
	  lastIndexPerAnt_p.resize(vb.nAntennas());
	}
	lastIndexPerAnt_p = 0;
	msId_p = vb.msId();
      }
      pointIndex = getIndex(act_mspc, vb.time()(row), -1.0, vb.antenna1()(row));
      if (pointIndex < 0)
	pointIndex = getIndex(act_mspc, vb.time()(row), vb.timeInterval()(row), vb.antenna1()(row));
    }
    if (!nullPointingTable && ((pointIndex < 0) || (pointIndex >= Int(act_mspc.time().nrow())))) {
      ostringstream o;
      o << "Failed to find pointing information for time " <<
	MVTime(vb.time()(row)/86400.0);
      logIO_p << String(o) << LogIO::EXCEPTION;
    }

    MEpoch epoch(Quantity(vb.time()(row), "s"));
    if (!pointingToImage) {
      lastAntID_p = vb.antenna1()(row);
      MPosition pos = vb.subtableColumns().antenna().positionMeas()(lastAntID_p);
      //mFrame_p = MeasFrame(epoch, pos);
      (!mFrame_p.epoch()) ?  mFrame_p.set(epoch) : mFrame_p.resetEpoch(epoch);
      (!mFrame_p.position()) ? mFrame_p.set(pos) : mFrame_p.resetPosition(pos);
      if (!nullPointingTable) {
	worldPosMeas = directionMeas(act_mspc, pointIndex);
      } else {
	worldPosMeas = vb.direction1()(row);
      }

      // Make a machine to convert from the worldPosMeas to the output
      // Direction Measure type for the relevant frame
      MDirection::Ref outRef(dc.directionType(), mFrame_p);
      pointingToImage = new MDirection::Convert(worldPosMeas, outRef);
      if (!pointingToImage) {
	logIO_p << "Cannot make direction conversion machine" << LogIO::EXCEPTION;
      }

    } else {
      mFrame_p.resetEpoch(epoch);
      if (lastAntID_p != vb.antenna1()(row)) {
        logIO_p << LogIO::DEBUGGING
          << "updating antenna position. MS ID " << msId_p
          << ", last antenna ID " << lastAntID_p
          << " new antenna ID " << vb.antenna1()(row) << LogIO::POST;
	lastAntID_p = vb.antenna1()(row);
	MPosition pos = vb.subtableColumns().antenna().positionMeas()(lastAntID_p);
	mFrame_p.resetPosition(pos);
      }
    }

    if (!nullPointingTable) {
      worldPosMeas = (*pointingToImage)(directionMeas(act_mspc, pointIndex));
    } else {
      worldPosMeas = (*pointingToImage)(vb.direction1()(row));
    }
    delete pointingToImage;
    pointingToImage = 0;
  }

  directionCoord = coords.directionCoordinate(directionIndex);
  //make sure we use the same units
  worldPosMeas.set(dc.worldAxisUnits()(0));

  // Reference pixel may be modified in dc.setReferenceValue when
  // projection type is SFL. To take into account this effect,
  // keep original reference pixel here and subtract it from
  // the reference pixel after dc.setReferenceValue instead
  // of setting reference pixel to (0,0).
  Vector<Double> const originalReferencePixel = dc.referencePixel();
  dc.setReferenceValue(worldPosMeas.getAngle().getValue());
  //Vector<Double> unitVec(2);
  //unitVec=0.0;
  //dc.setReferencePixel(unitVec);
  Vector<Double> updatedReferencePixel = dc.referencePixel() - originalReferencePixel;
  dc.setReferencePixel(updatedReferencePixel);

  coords.replaceCoordinate(dc, directionIndex);

  IPosition pbShape(4, convSize, 2, 1, 1);
  IPosition start(4, 0, 0, 0, 0);

  TempImage<Complex> onedPB(pbShape, coords);

  onedPB.set(Complex(1.0, 0.0));

  AlwaysAssert(sj_p, AipsError);
  sj_p->apply(onedPB, onedPB, vb, 0);

  IPosition pbSlice(4, convSize, 1, 1, 1);
  Vector<Float> tempConvFunc=real(onedPB.getSlice(start, pbSlice, true));
  // Find number of significant points
  uInt cfLen=0;
  for(uInt i=0;i<tempConvFunc.nelements();++i) {
    if(tempConvFunc(i)<1e-3) break;
    ++cfLen;
  }
  if(cfLen<1) {
    logIO() << LogIO::SEVERE
	    << "Possible problem in primary beam calculation: no points in gridding function"
	    << " - no points to be gridded on this image?" << LogIO::POST;
    cfLen=1;
  }
  Vector<Float> trimConvFunc=tempConvFunc(Slice(0,cfLen-1,1));

  // Now fill in the convolution function vector
  convSupport=cfLen/convSampling;
  convSize=convSampling*(2*convSupport+2);
  convFunc.resize(convSize);
  convFunc=0.0;
  convFunc(Slice(0,cfLen-1,1))=trimConvFunc(Slice(0,cfLen-1,1));


}

// Initialize for a transform from the Sky domain. This means that
// we grid-correct, and FFT the image
void SDGrid::initializeToVis(ImageInterface<Complex>& iimage,
			     const vi::VisBuffer2& vb)
{
  image=&iimage;

  ok();

  init();

  if(convType=="pb") {
    findPBAsConvFunction(*image, vb);
  }

  // reset msId_p and lastAntID_p to -1
  // this is to ensure correct antenna position in getXYPos
  msId_p = -1;
  lastAntID_p = -1;

  // Initialize the maps for polarization and channel. These maps
  // translate visibility indices into image indices
  initMaps(vb);

  // First get the CoordinateSystem for the image and then find
  // the DirectionCoordinate
  CoordinateSystem coords=image->coordinates();
  Int directionIndex=coords.findCoordinate(Coordinate::DIRECTION);
  AlwaysAssert(directionIndex>=0, AipsError);
  directionCoord=coords.directionCoordinate(directionIndex);
  /*if((image->shape().product())>cachesize) {
    isTiled=true;
  }
  else {
    isTiled=false;
    }*/
  isTiled=false;
  nx    = image->shape()(0);
  ny    = image->shape()(1);
  npol  = image->shape()(2);
  nchan = image->shape()(3);

  // If we are memory-based then read the image in and create an
  // ArrayLattice otherwise just use the PagedImage
  /*if(isTiled) {
    lattice=image;
    wLattice=wImage;
  }
  else*/
{
    // Make the grid the correct shape and turn it into an array lattice
    IPosition gridShape(4, nx, ny, npol, nchan);
    griddedData.resize(gridShape);
    griddedData = Complex(0.0);

    wGriddedData.resize(gridShape);
    wGriddedData = 0.0;

    if(arrayLattice) delete arrayLattice; arrayLattice=0;
    arrayLattice = new ArrayLattice<Complex>(griddedData);

    if(wArrayLattice) delete wArrayLattice; wArrayLattice=0;
    wArrayLattice = new ArrayLattice<Float>(wGriddedData);
    wArrayLattice->set(0.0);
    wLattice=wArrayLattice;

    // Now find the SubLattice corresponding to the image
    IPosition blc(4, (nx-image->shape()(0)+(nx%2==0))/2, (ny-image->shape()(1)+(ny%2==0))/2, 0, 0);
    IPosition stride(4, 1);
    IPosition trc(blc+image->shape()-stride);
    LCBox gridBox(blc, trc, gridShape);
    SubLattice<Complex> gridSub(*arrayLattice, gridBox, true);

    // Do the copy
    gridSub.copyData(*image);

    lattice=arrayLattice;
  }
  AlwaysAssert(lattice, AipsError);
  AlwaysAssert(wLattice, AipsError);
}

void SDGrid::finalizeToVis()
{
  /*if(isTiled) {

    logIO() << LogOrigin("SDGrid", "finalizeToVis")  << LogIO::NORMAL;

    AlwaysAssert(imageCache, AipsError);
    AlwaysAssert(image, AipsError);
    ostringstream o;
    imageCache->flush();
    imageCache->showCacheStatistics(o);
    logIO() << o.str() << LogIO::POST;
    }*/
  if(pointingToImage) delete pointingToImage; pointingToImage=0;
}


// Initialize the FFT to the Sky. Here we have to setup and initialize the
// grid.
void SDGrid::initializeToSky(ImageInterface<Complex>& iimage,
			     Matrix<Float>& weight, const vi::VisBuffer2& vb)
{
  // image always points to the image
  image=&iimage;

  ok();

  init();

  if(convType=="pb") {
    findPBAsConvFunction(*image, vb);
  }

  // reset msId_p and lastAntID_p to -1
  // this is to ensure correct antenna position in getXYPos
  msId_p = -1;
  lastAntID_p = -1;

  // Initialize the maps for polarization and channel. These maps
  // translate visibility indices into image indices
  initMaps(vb);
  //cerr << "ToSky cachesize " << cachesize << " im shape " << (image->shape().product()) << endl;
  /*if((image->shape().product())>cachesize) {
    isTiled=true;
  }
  else {
    isTiled=false;
  }
  */
  //////////////No longer using isTiled
  isTiled=false;
  nx    = image->shape()(0);
  ny    = image->shape()(1);
  npol  = image->shape()(2);
  nchan = image->shape()(3);

  sumWeight=0.0;
  weight.resize(sumWeight.shape());
  weight=0.0;

  // First get the CoordinateSystem for the image and then find
  // the DirectionCoordinate
  CoordinateSystem coords=image->coordinates();
  Int directionIndex=coords.findCoordinate(Coordinate::DIRECTION);
  AlwaysAssert(directionIndex>=0, AipsError);
  directionCoord=coords.directionCoordinate(directionIndex);

  // Initialize for in memory or to disk gridding. lattice will
  // point to the appropriate Lattice, either the ArrayLattice for
  // in memory gridding or to the image for to disk gridding.
  /*if(isTiled) {
    imageCache->flush();
    image->set(Complex(0.0));
    lattice=image;
    wLattice=wImage;
  }
  else*/
  {
    IPosition gridShape(4, nx, ny, npol, nchan);
    logIO() << LogOrigin("SDGrid", "initializeToSky", WHERE) << LogIO::DEBUGGING
        << "gridShape = " << gridShape << LogIO::POST;
    griddedData.resize(gridShape);
    griddedData=Complex(0.0);
    if(arrayLattice) delete arrayLattice; arrayLattice=0;
    arrayLattice = new ArrayLattice<Complex>(griddedData);
    lattice=arrayLattice;
    wGriddedData.resize(gridShape);
    wGriddedData=0.0;
    if(wArrayLattice) delete wArrayLattice; wArrayLattice=0;
    wArrayLattice = new ArrayLattice<Float>(wGriddedData);
    wLattice=wArrayLattice;

    // clipping related stuff
    if (clipminmax_) {
      gmin_.resize(gridShape);
      gmin_ = Complex(FLT_MAX);
      gmax_.resize(gridShape);
      gmax_ = Complex(-FLT_MAX);
      wmin_.resize(gridShape);
      wmin_ = 0.0f;
      wmax_.resize(gridShape);
      wmax_ = 0.0f;
      npoints_.resize(gridShape);
      npoints_ = 0;
    }
  }
  AlwaysAssert(lattice, AipsError);
  AlwaysAssert(wLattice, AipsError);
}

void SDGrid::finalizeToSky()
{

  // Now we flush the cache and report statistics
  // For memory based, we don't write anything out yet.
  /*if(isTiled) {
    logIO() << LogOrigin("SDGrid", "finalizeToSky")  << LogIO::NORMAL;

    AlwaysAssert(image, AipsError);
    AlwaysAssert(imageCache, AipsError);
    imageCache->flush();
    ostringstream o;
    imageCache->showCacheStatistics(o);
    logIO() << o.str() << LogIO::POST;
  }
  */

  if(pointingToImage) delete pointingToImage; pointingToImage=0;
}

Array<Complex>* SDGrid::getDataPointer(const IPosition& centerLoc2D,
				       Bool readonly) {
  Array<Complex>* result;
  // Is tiled: get tiles and set up offsets
  centerLoc(0)=centerLoc2D(0);
  centerLoc(1)=centerLoc2D(1);
  result=&imageCache->tile(offsetLoc, centerLoc, readonly);
  return result;
}
Array<Float>* SDGrid::getWDataPointer(const IPosition& centerLoc2D,
				      Bool readonly) {
  Array<Float>* result;
  // Is tiled: get tiles and set up offsets
  centerLoc(0)=centerLoc2D(0);
  centerLoc(1)=centerLoc2D(1);
  result=&wImageCache->tile(offsetLoc, centerLoc, readonly);
  return result;
}

#define NEED_UNDERSCORES
#if defined(NEED_UNDERSCORES)
#define ggridsd ggridsd_
#define dgridsd dgridsd_
#define ggridsdclip ggridsdclip_
#endif

extern "C" {
   void ggridsd(Double*,
		const Complex*,
                Int*,
                Int*,
                Int*,
		const Int*,
		const Int*,
		const Float*,
		Int*,
		Int*,
		Complex*,
		Float*,
                Int*,
		Int*,
		Int *,
		Int *,
                Int*,
		Int*,
		Float*,
		Int*,
		Int*,
		Double*);
   void ggridsdclip(Double*,
                 const Complex*,
                 Int*,
                 Int*,
                 Int*,
                 const Int*,
                 const Int*,
                 const Float*,
                 Int*,
                 Int*,
                 Complex*,
                 Float*,
                 Int*,
                 Complex*,
                 Float*,
                 Complex*,
                 Float*,
                 Int*,
                 Int*,
                 Int *,
                 Int *,
                 Int*,
                 Int*,
                 Float*,
                 Int*,
                 Int*,
                 Double*);
   void dgridsd(Double*,
		Complex*,
                Int*,
                Int*,
		const Int*,
		const Int*,
		Int*,
		Int*,
		const Complex*,
                Int*,
		Int*,
		Int *,
		Int *,
                Int*,
		Int*,
		Float*,
		Int*,
		Int*);
}

void SDGrid::put(const vi::VisBuffer2& vb, Int row, Bool dopsf,
		 FTMachine::Type type)
{
  LogIO os(LogOrigin("SDGrid", "put"));

  gridOk(convSupport);

  // There is no channel mapping cache in VI/VB2 version of FTMachine
  // Perform matchChannel everytime
  matchChannel(vb);

  //No point in reading data if its not matching in frequency
  if(max(chanMap)==-1)
    return;

  Matrix<Float> imagingweight;
  //imagingweight=&(vb.imagingWeight());
  pickWeights(vb, imagingweight);

  if(type==FTMachine::PSF || type==FTMachine::COVERAGE)
    dopsf=true;
  if(dopsf) type=FTMachine::PSF;
  Cube<Complex> data;
  //Fortran gridder need the flag as ints
  Cube<Int> flags;
  Matrix<Float> elWeight;
  interpolateFrequencyTogrid(vb, imagingweight,data, flags, elWeight, type);
  //cerr << "number of rows " << vb.nRow() << " data shape " << data.shape() << endl;
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
    nRow=vb.nRows();
    startRow=0;
    endRow=nRow-1;
  } else {
    nRow=1;
    startRow=row;
    endRow=row;
  }


  Vector<Int> rowFlags(vb.flagRow().nelements());
  rowFlags=0;
  for (Int rownr=startRow; rownr<=endRow; rownr++) {
    if(vb.flagRow()(rownr)) rowFlags(rownr)=1;
  }

  // Take care of translation of Bools to Integer
  Int idopsf=0;
  if(dopsf) idopsf=1;

  /*if(isTiled) {
    for (Int rownr=startRow; rownr<=endRow; rownr++) {

      if(getXYPos(vb, rownr)) {

	IPosition centerLoc2D(2, Int(xyPos(0)), Int(xyPos(1)));
	Array<Complex>* dataPtr=getDataPointer(centerLoc2D, false);
	Array<Float>*  wDataPtr=getWDataPointer(centerLoc2D, false);
	Int aNx=dataPtr->shape()(0);
	Int aNy=dataPtr->shape()(1);
	Vector<Double> actualPos(2);
	for (Int i=0;i<2;i++) {
	  actualPos(i)=xyPos(i)-Double(offsetLoc(i));
	}
	// Now use FORTRAN to do the gridding. Remember to
	// ensure that the shape and offsets of the tile are
	// accounted for.
	{
	  Bool del;
	  //	  IPosition s(data.shape());
	  const IPosition& fs=flags.shape();
	  std::vector<Int> s(fs.begin(), fs.end());

	  ggridsd(actualPos.getStorage(del),
		  datStorage,
		  &s[0],
		  &s[1],
		  &idopsf,
		  flags.getStorage(del),
		  rowFlags.getStorage(del),
		  wgtStorage,
		  &s[2],
		  &rownr,
		  dataPtr->getStorage(del),
		  wDataPtr->getStorage(del),
		  &aNx,
		  &aNy,
		  &npol,
		  &nchan,
		  &convSupport,
		  &convSampling,
		  convFunc.getStorage(del),
		  chanMap.getStorage(del),
		  polMap.getStorage(del),
		  sumWeight.getStorage(del));
	}
      }
    }
  }
  else*/
  {
    Matrix<Double> xyPositions(2, endRow-startRow+1);
    xyPositions=-1e9; // make sure failed getXYPos does not fall on grid
    for (Int rownr=startRow; rownr<=endRow; rownr++) {
      if(getXYPos(vb, rownr)) {
	xyPositions(0, rownr)=xyPos(0);
	xyPositions(1, rownr)=xyPos(1);
      }
    }
    {
      Bool del;
      //      IPosition s(data.shape());
      const IPosition& fs=flags.shape();
      std::vector<Int> s(fs.begin(), fs.end());
      Bool datCopy, wgtCopy;
      Complex * datStor=griddedData.getStorage(datCopy);
      Float * wgtStor=wGriddedData.getStorage(wgtCopy);

      //Bool call_ggridsd = !clipminmax_ || dopsf;
      Bool call_ggridsd = !clipminmax_;

      if (call_ggridsd) {

      ggridsd(xyPositions.getStorage(del),
	      datStorage,
	      &s[0],
	      &s[1],
	      &idopsf,
	      flags.getStorage(del),
	      rowFlags.getStorage(del),
	      wgtStorage,
	      &s[2],
	      &row,
	      datStor,
	      wgtStor,
	      &nx,
	      &ny,
	      &npol,
	      &nchan,
	      &convSupport,
	      &convSampling,
	      convFunc.getStorage(del),
	      chanMap.getStorage(del),
	      polMap.getStorage(del),
	      sumWeight.getStorage(del));

      } else {
        Bool gminCopy;
        Complex *gminStor = gmin_.getStorage(gminCopy);
        Bool gmaxCopy;
        Complex *gmaxStor = gmax_.getStorage(gmaxCopy);
        Bool wminCopy;
        Float *wminStor = wmin_.getStorage(wminCopy);
        Bool wmaxCopy;
        Float *wmaxStor = wmax_.getStorage(wmaxCopy);
        Bool npCopy;
        Int *npStor = npoints_.getStorage(npCopy);

        ggridsdclip(xyPositions.getStorage(del),
          datStorage,
          &s[0],
          &s[1],
          &idopsf,
          flags.getStorage(del),
          rowFlags.getStorage(del),
          wgtStorage,
          &s[2],
          &row,
          datStor,
          wgtStor,
          npStor,
          gminStor,
          wminStor,
          gmaxStor,
          wmaxStor,
          &nx,
          &ny,
          &npol,
          &nchan,
          &convSupport,
          &convSampling,
          convFunc.getStorage(del),
          chanMap.getStorage(del),
          polMap.getStorage(del),
          sumWeight.getStorage(del));

        gmin_.putStorage(gminStor, gminCopy);
        gmax_.putStorage(gmaxStor, gmaxCopy);
        wmin_.putStorage(wminStor, wminCopy);
        wmax_.putStorage(wmaxStor, wmaxCopy);
        npoints_.putStorage(npStor, npCopy);
      }
      griddedData.putStorage(datStor, datCopy);
      wGriddedData.putStorage(wgtStor, wgtCopy);
    }
  }
  if(!dopsf)
    data.freeStorage(datStorage, isCopy);
  elWeight.freeStorage(wgtStorage,iswgtCopy);

}

void SDGrid::get(vi::VisBuffer2& vb, Int row)
{
  LogIO os(LogOrigin("SDGrid", "get"));

  gridOk(convSupport);

  // If row is -1 then we pass through all rows
  Int startRow, endRow, nRow;
  if (row==-1) {
    nRow=vb.nRows();
    startRow=0;
    endRow=nRow-1;
    // TODO: ask imager guru if commenting out the following line
    //       is safe for SDGrid
    //unnecessary zeroing
    //vb.modelVisCube()=Complex(0.0,0.0);
  } else {
    nRow=1;
    startRow=row;
    endRow=row;
    // TODO: ask imager guru if commenting out the following line
    //       is safe for SDGrid
    //unnecessary zeroing
    //vb.modelVisCube().xyPlane(row)=Complex(0.0,0.0);
  }

  // There is no channel mapping cache in VI/VB2 version of FTMachine
  // Perform matchChannel everytime
  matchChannel(vb);

  //No point in reading data if its not matching in frequency
  if(max(chanMap)==-1)
    return;
  Cube<Complex> data;
  Cube<Int> flags;
  getInterpolateArrays(vb, data, flags);

  Complex *datStorage;
  Bool isCopy;
  datStorage=data.getStorage(isCopy);
  // NOTE: with MS V2.0 the pointing could change per antenna and timeslot
  //
  Vector<Int> rowFlags(vb.flagRow().nelements());
  rowFlags=0;
  for (Int rownr=startRow; rownr<=endRow; rownr++) {
    if(vb.flagRow()(rownr)) rowFlags(rownr)=1;
    //single dish yes ?
    if(max(vb.uvw().column(rownr)) > 0.0) rowFlags(rownr)=1;
  }


  /*if(isTiled) {

    for (Int rownr=startRow; rownr<=endRow; rownr++) {

      if(getXYPos(vb, rownr)) {

	  // Get the tile
	IPosition centerLoc2D(2, Int(xyPos(0)), Int(xyPos(1)));
	Array<Complex>* dataPtr=getDataPointer(centerLoc2D, true);
	Int aNx=dataPtr->shape()(0);
	Int aNy=dataPtr->shape()(1);

	// Now use FORTRAN to do the gridding. Remember to
	// ensure that the shape and offsets of the tile are
	// accounted for.
	Bool del;
	Vector<Double> actualPos(2);
	for (Int i=0;i<2;i++) {
	  actualPos(i)=xyPos(i)-Double(offsetLoc(i));
	}
	//	IPosition s(data.shape());
	const IPosition& fs=data.shape();
	std::vector<Int> s(fs.begin(), fs.end());

	dgridsd(actualPos.getStorage(del),
		datStorage,
		&s[0],
		&s[1],
		flags.getStorage(del),
		rowFlags.getStorage(del),
		&s[2],
		&rownr,
		dataPtr->getStorage(del),
		&aNx,
		&aNy,
		&npol,
		&nchan,
		&convSupport,
		&convSampling,
		convFunc.getStorage(del),
		chanMap.getStorage(del),
		polMap.getStorage(del));
      }
    }
  }
  else*/
  {
    Matrix<Double> xyPositions(2, endRow-startRow+1);
    xyPositions=-1e9;
    for (Int rownr=startRow; rownr<=endRow; rownr++) {
      if(getXYPos(vb, rownr)) {
	xyPositions(0, rownr)=xyPos(0);
	xyPositions(1, rownr)=xyPos(1);
      }
    }

    Bool del;
    //    IPosition s(data.shape());
    const IPosition& fs=data.shape();
    std::vector<Int> s(fs.begin(), fs.end());
    dgridsd(xyPositions.getStorage(del),
	    datStorage,
	    &s[0],
	    &s[1],
	    flags.getStorage(del),
	    rowFlags.getStorage(del),
	    &s[2],
	    &row,
	    griddedData.getStorage(del),
	    &nx,
	    &ny,
	    &npol,
	    &nchan,
	    &convSupport,
	    &convSampling,
	    convFunc.getStorage(del),
	    chanMap.getStorage(del),
	    polMap.getStorage(del));

    data.putStorage(datStorage, isCopy);
  }
  interpolateFrequencyFromgrid(vb, data, FTMachine::MODEL);
}



  // Make a plain straightforward honest-to-FSM image. This returns
  // a complex image, without conversion to Stokes. The representation
  // is that required for the visibilities.
  //----------------------------------------------------------------------
  void SDGrid::makeImage(FTMachine::Type type,
			    vi::VisibilityIterator2& vi,
			    ImageInterface<Complex>& theImage,
			    Matrix<Float>& weight) {


    logIO() << LogOrigin("FTMachine", "makeImage0") << LogIO::NORMAL;

    // Loop over all visibilities and pixels
    vi::VisBuffer2 *vb = vi.getVisBuffer();

    // Initialize put (i.e. transform to Sky) for this model
    vi.origin();

    if(vb->polarizationFrame()==MSIter::Linear) {
      StokesImageUtil::changeCStokesRep(theImage, StokesImageUtil::LINEAR);
    }
    else {
      StokesImageUtil::changeCStokesRep(theImage, StokesImageUtil::CIRCULAR);
    }
    Bool useCorrected= !(MSMainColumns(vi.ms()).correctedData().isNull());
    if((type==FTMachine::CORRECTED) && (!useCorrected))
      type=FTMachine::OBSERVED;
    Bool normalize=true;
    if(type==FTMachine::COVERAGE)
      normalize=false;

    Int Nx=theImage.shape()(0);
    Int Ny=theImage.shape()(1);
    Int Npol=theImage.shape()(2);
    Int Nchan=theImage.shape()(3);
    Double memtot=Double(HostInfo::memoryTotal(true))*1024.0; // return in kB
    Int nchanInMem=Int(memtot/2.0/8.0/Double(Nx*Ny*Npol));
    Int nloop=nchanInMem >= Nchan ? 1 : Nchan/nchanInMem+1;
    ImageInterface<Complex> *imCopy=NULL;
    IPosition blc(theImage.shape());
    IPosition trc(theImage.shape());
    blc-=blc; //set all values to 0
    trc=theImage.shape();
    trc-=1; // set trc to image size -1
    if(nloop==1) {
      imCopy=& theImage;
      nchanInMem=Nchan;
    }
    else
      logIO()  << "Not enough memory to image in one go \n will process the image in   "
	       << nloop
	      << " sections  "
	      << LogIO::POST;

    weight.resize(Npol, Nchan);
    Matrix<Float> wgtcopy(Npol, Nchan);

    Bool isWgtZero=true;
    for (Int k=0; k < nloop; ++k){
      Int bchan=k*nchanInMem;
      Int echan=(k+1)*nchanInMem < Nchan ?  (k+1)*nchanInMem-1 : Nchan-1;

      if(nloop > 1) {
	 blc[3]=bchan;
	 trc[3]=echan;
	 Slicer sl(blc, trc, Slicer::endIsLast);
	 imCopy=new SubImage<Complex>(theImage, sl, true);
	 wgtcopy.resize(npol, echan-bchan+1);
      }
      vi.originChunks();
      vi.origin();
      initializeToSky(*imCopy,wgtcopy,*vb);


      // for minmax clipping
      logIO() << LogOrigin("SDGrid", "makeImage", WHERE) << LogIO::DEBUGGING
          << "doclip_ = " << (clipminmax_ ? "TRUE" : "FALSE") << " (" << clipminmax_ << ")" << LogIO::POST;
      if (clipminmax_) {
        logIO() << LogOrigin("SDGRID", "makeImage", WHERE)
             << LogIO::DEBUGGING << "use ggridsd2 for imaging" << LogIO::POST;
      }

      // Loop over the visibilities, putting VisBuffers
	  for (vi.originChunks();vi.moreChunks();vi.nextChunk()) {
	    for (vi.origin(); vi.more(); vi.next()) {

	  switch(type) {
	  case FTMachine::RESIDUAL:
	    vb->setVisCube(vb->visCubeCorrected() - vb->visCubeModel());
	    put(*vb, -1, false);
	    break;
	  case FTMachine::MODEL:
	    put(*vb, -1, false, FTMachine::MODEL);
	    break;
	  case FTMachine::CORRECTED:
	    put(*vb, -1, false, FTMachine::CORRECTED);
	    break;
	  case FTMachine::PSF:
	    vb->setVisCube(Complex(1.0,0.0));
	    put(*vb, -1, true, FTMachine::PSF);
	    break;
	  case FTMachine::COVERAGE:
	    vb->setVisCube(Complex(1.0));
	    put(*vb, -1, true, FTMachine::COVERAGE);
	    break;
	  case FTMachine::OBSERVED:
	  default:
	    put(*vb, -1, false, FTMachine::OBSERVED);
	    break;
	  }
	}
      }
      finalizeToSky();
      // Normalize by dividing out weights, etc.
      getImage(wgtcopy, normalize);
      if(max(wgtcopy)==0.0){
	if(nloop > 1)
	  logIO() << LogIO::WARN
		  << "No useful data in SDGrid: weights all zero for image slice  " << k
		  << LogIO::POST;
      }
      else
	isWgtZero=false;

      weight(Slice(0, Npol), Slice(bchan, echan-bchan+1))=wgtcopy;
      if(nloop >1) delete imCopy;
    }//loop k
    if(isWgtZero)
      logIO() << LogIO::SEVERE
	      << "No useful data in SDGrid: weights all zero"
	      << LogIO::POST;
  }



// Finalize : optionally normalize by weight image
ImageInterface<Complex>& SDGrid::getImage(Matrix<Float>& weights,
					  Bool normalize)
{
  AlwaysAssert(lattice, AipsError);
  AlwaysAssert(image, AipsError);

  logIO() << LogOrigin("SDGrid", "getImage") << LogIO::NORMAL;

  // execute minmax clipping
  clipMinMax();

  weights.resize(sumWeight.shape());

  convertArray(weights,sumWeight);

  // If the weights are all zero then we cannot normalize
  // otherwise we don't care.
  ///////////////////////
  /*{
  PagedImage<Float> thisScreen(lattice->shape(), image->coordinates(), "TheData");
  LatticeExpr<Float> le(abs(*lattice));
  thisScreen.copyData(le);
  PagedImage<Float> thisScreen2(lattice->shape(), image->coordinates(), "TheWeight");
  LatticeExpr<Float> le2(abs(*wLattice));
  thisScreen2.copyData(le2);
  }*/
  /////////////////////

  if(normalize) {
    if(max(weights)==0.0) {
      //logIO() << LogIO::WARN << "No useful data in SDGrid: weights all zero"
      //	      << LogIO::POST;
      //image->set(Complex(0.0));
      return *image;
    }
    //Timer tim;
    //tim.mark();
    //lattice->copyData((LatticeExpr<Complex>)(iif((*wLattice<=minWeight_p), 0.0,
    //						 (*lattice)/(*wLattice))));
    //As we are not using disk based lattices anymore the above uses too much memory for
    // ArrayLattice...it does not do a real  inplace math but rather mutiple copies
    // seem to be involved  thus the less elegant but faster and less memory hog loop
    // below.

    IPosition pos(4);
    for (Int chan=0; chan < nchan; ++chan){
      pos[3]=chan;
      for( Int pol=0; pol < npol; ++ pol){
	pos[2]=pol;
	for (Int y=0; y < ny ; ++y){
	  pos[1]=y;
	  for (Int x=0; x < nx; ++x){
	    pos[0]=x;
	    Float wgt=wGriddedData(pos);
	    if(wgt > minWeight_p)
	      griddedData(pos)=griddedData(pos)/wgt;
	    else
	      griddedData(pos)=0.0;
	  }
	}
      }
      }
    //tim.show("After normalizing");
  }

  //if(!isTiled)
  {
    // Now find the SubLattice corresponding to the image
    IPosition gridShape(4, nx, ny, npol, nchan);
    IPosition blc(4, (nx-image->shape()(0)+(nx%2==0))/2,
		  (ny-image->shape()(1)+(ny%2==0))/2, 0, 0);
    IPosition stride(4, 1);
    IPosition trc(blc+image->shape()-stride);
    LCBox gridBox(blc, trc, gridShape);
    SubLattice<Complex> gridSub(*arrayLattice, gridBox);

    // Do the copy
    image->copyData(gridSub);
  }

  // IMAGER MIGRATION
  // set sumWeight to 1.0
  weights = 1.0;

  return *image;
}

// Return weights image
void SDGrid::getWeightImage(ImageInterface<Float>& weightImage, Matrix<Float>& weights)
{
  AlwaysAssert(lattice, AipsError);
  AlwaysAssert(image, AipsError);

  logIO() << LogOrigin("SDGrid", "getWeightImage") << LogIO::NORMAL;

  weights.resize(sumWeight.shape());
  // IMAGER MIGRATION
  // set sumWeight to 1.0
  weights = 1.0;
//  convertArray(weights,sumWeight);

  weightImage.copyData(*wArrayLattice);
}

void SDGrid::ok() {
  AlwaysAssert(image, AipsError);
}

// Get the index into the pointing table for this time. Note that the
// in the pointing table, TIME specifies the beginning of the spanned
// time range, whereas for the main table, TIME is the centroid.
// Note that the behavior for multiple matches is not specified! i.e.
// if there are multiple matches, the index returned depends on the
// history of previous matches. It is deterministic but not obvious.
// One could cure this by searching but it would be considerably
// costlier.
Int SDGrid::getIndex(const MSPointingColumns& mspc, const Double& time,
		     const Double& interval, const Int& antid) {
  //Int start=lastIndex_p;
  Int start=lastIndexPerAnt_p[antid];
  Double tol=interval < 0.0 ? time*C::dbl_epsilon : interval/2.0;
  // Search forwards
  Int nrows=mspc.time().nrow();
  for (Int i=start;i<nrows;i++) {
    // Check for ANTENNA_ID. When antid<0 (default) ANTENNA_ID in POINTING table < 0 is ignored.
    // MS v2 definition indicates ANTENNA_ID in POINING >= 0.
    if (antid >= 0 && mspc.antennaId()(i) != antid) {
      continue;
    }

    Double midpoint = mspc.time()(i); // time in POINTING table is midpoint
    // If the interval in the pointing table is negative, use the last
    // entry. Note that this may be invalid (-1) but in that case
    // the calling routine will generate an error
    Double mspc_interval = mspc.interval()(i);

    if(mspc_interval<0.0) {
      //return lastIndex_p;
      return lastIndexPerAnt_p[antid];
    }
    // Pointing table interval is specified so we have to do a match
    else {
      // Is the midpoint of this pointing table entry within the specified
      // tolerance of the main table entry?
      if(abs(midpoint-time) <= (mspc_interval/2.0+tol)) {
      	//lastIndex_p=i;
      	lastIndexPerAnt_p[antid]=i;
	return i;
      }
    }
  }
  // Search backwards
  for (Int i=start;i>=0;i--) {
    if (antid >= 0 && mspc.antennaId()(i) != antid) {
      continue;
    }
    Double midpoint = mspc.time()(i); // time in POINTING table is midpoint
    Double mspc_interval = mspc.interval()(i);
    if(mspc_interval<0.0) {
      //return lastIndex_p;
      return lastIndexPerAnt_p[antid];
    }
    // Pointing table interval is specified so we have to do a match
    else {
      // Is the midpoint of this pointing table entry within the specified
      // tolerance of the main table entry?
      if (abs(midpoint-time) <= (mspc_interval/2.0+tol)) {
	//lastIndex_p=i;
  lastIndexPerAnt_p[antid]=i;
	return i;
      }
    }
  }
  // No match!
  return -1;
}

Bool SDGrid::getXYPos(const vi::VisBuffer2& vb, Int row) {

  Bool dointerp;
  const MSPointingColumns& act_mspc = vb.subtableColumns().pointing();
  Bool nullPointingTable = (act_mspc.nrow() < 1);
  Int pointIndex = -1;
  if (!nullPointingTable) {
    ///if(vb.newMS())  vb.newMS does not work well using msid
    if (vb.msId() != msId_p) {
      lastIndex_p = 0;
      if (lastIndexPerAnt_p.nelements() < (size_t)vb.nAntennas()) {
        lastIndexPerAnt_p.resize(vb.nAntennas());
      }
      lastIndexPerAnt_p = 0;
      msId_p = vb.msId();
      lastAntID_p = -1;
    }
    pointIndex = getIndex(act_mspc, vb.time()(row), -1.0, vb.antenna1()(row));
    //Try again to locate a pointing within the integration
    if (pointIndex < 0)
      pointIndex = getIndex(act_mspc, vb.time()(row), vb.timeInterval()(row), vb.antenna1()(row));
  }
  if (!nullPointingTable && ((pointIndex < 0) || (pointIndex >= Int(act_mspc.time().nrow())))) {
    ostringstream o;
    o << "Failed to find pointing information for time " <<
      MVTime(vb.time()(row)/86400.0) << ": Omitting this point";
    logIO_p << LogIO::DEBUGGING << String(o) << LogIO::POST;
    //    logIO_p << String(o) << LogIO::POST;
    return false;
  }

  dointerp = false;
  if (!nullPointingTable && (vb.timeInterval()(row) < act_mspc.interval()(pointIndex))) {
    dointerp = true;
    if (!isSplineInterpolationReady) {
      interpolator = new SDPosInterpolator(vb, pointingDirCol_p);
      isSplineInterpolationReady = true;
    } else {
      if (!interpolator->inTimeRange(vb.time()(row), vb.antenna1()(row))) {
	// setup spline interpolator for the current dataset (CAS-11261, 2018/6/13 WK)
	delete interpolator;
	interpolator = 0;
	interpolator = new SDPosInterpolator(vb, pointingDirCol_p);
      }
    }
  }

  MEpoch epoch(Quantity(vb.time()(row), "s"));
  if (!pointingToImage) {
    // Set the frame
    lastAntID_p = vb.antenna1()(row);
    MPosition pos = vb.subtableColumns().antenna().positionMeas()(lastAntID_p);
    (!mFrame_p.epoch()) ?  mFrame_p.set(epoch) : mFrame_p.resetEpoch(epoch);
    (!mFrame_p.position()) ? mFrame_p.set(pos) : mFrame_p.resetPosition(pos);
    if (!nullPointingTable) {
      if (dointerp) {
        worldPosMeas = directionMeas(act_mspc, pointIndex, vb.time()(row));
      } else {
        worldPosMeas = directionMeas(act_mspc, pointIndex);
      }
    } else {
      worldPosMeas = vb.direction1()(row);
    }

    // Make a machine to convert from the worldPosMeas to the output
    // Direction Measure type for the relevant frame
    MDirection::Ref outRef(directionCoord.directionType(), mFrame_p);
    pointingToImage = new MDirection::Convert(worldPosMeas, outRef);
    if (!pointingToImage) {
      logIO_p << "Cannot make direction conversion machine" << LogIO::EXCEPTION;
    }

    // perform direction conversion to clear cache
    // set timestamp 1 week earlier
    MEpoch epoch_tmp(Quantity(vb.time()(row) - 86400.0 * 7, "s"));
    mFrame_p.resetEpoch(epoch_tmp);
    // perform conversion
    MDirection _dir_tmp = (*pointingToImage)(vb.direction1()(row));

    // revert timestamp
    mFrame_p.resetEpoch(epoch);

  } else {
    mFrame_p.resetEpoch(epoch);
    if (lastAntID_p != vb.antenna1()(row)) {
      if (lastAntID_p == -1) {
        // antenna ID is unset
        logIO_p << LogIO::DEBUGGING
          << "update antenna position for conversion: new MS ID " << msId_p
          << ", antenna ID " << vb.antenna1()(row) << LogIO::POST;
      } else {
        logIO_p << LogIO::DEBUGGING
          << "update antenna position for conversion: MS ID " << msId_p
          << ", last antenna ID " << lastAntID_p
          << ", new antenna ID " << vb.antenna1()(row) << LogIO::POST;
      }
      MPosition pos;
      lastAntID_p = vb.antenna1()(row);
      pos = vb.subtableColumns().antenna().positionMeas()(lastAntID_p);
      mFrame_p.resetPosition(pos);
    }
  }

  if (!nullPointingTable) {
    if (dointerp) {
      MDirection newdir = directionMeas(act_mspc, pointIndex, vb.time()(row));
      //Vector<Double> newdirv = newdir.getAngle("rad").getValue();
      worldPosMeas = (*pointingToImage)(newdir);
      //cerr<<"dir0="<<newdirv(0)<<endl;

    //fprintf(pfile,"%.8f %.8f \n", newdirv(0), newdirv(1));
    //printf("%lf %lf \n", newdirv(0), newdirv(1));
    } else {
      worldPosMeas = (*pointingToImage)(directionMeas(act_mspc, pointIndex));
    }
  } else {
    worldPosMeas = (*pointingToImage)(vb.direction1()(row));
  }

  Bool result = directionCoord.toPixel(xyPos, worldPosMeas);
  if (!result) {
    logIO_p << "Failed to find a pixel for pointing direction of "
	    << MVTime(worldPosMeas.getValue().getLong("rad")).string(MVTime::TIME) << ", " << MVAngle(worldPosMeas.getValue().getLat("rad")).string(MVAngle::ANGLE) << LogIO::WARN << LogIO::POST;
    return false;
  }

  if ((pointingDirCol_p == "SOURCE_OFFSET") || (pointingDirCol_p == "POINTING_OFFSET")) {
    //there is no sense to track in offset coordinates...hopefully the
    //user set the image coords right
    fixMovingSource_p = false;
  }
  if (fixMovingSource_p) {
    if (xyPosMovingOrig_p.nelements() < 2) {
      directionCoord.toPixel(xyPosMovingOrig_p, firstMovingDir_p);
    }
    //via HADEC or AZEL for parallax of near sources
    MDirection::Ref outref1(MDirection::AZEL, mFrame_p);
    MDirection tmphadec;
    if (upcase(movingDir_p.getRefString()).contains("APP")) {
      tmphadec = MDirection::Convert((vbutil_p->getEphemDir(vb, phaseCenterTime_p)), outref1)();
    } else if (upcase(movingDir_p.getRefString()).contains("COMET")) {
      MeasComet mcomet(Path(ephemTableName_p).absoluteName());
      mFrame_p.set(mcomet);
      tmphadec = MDirection::Convert(movingDir_p, outref1)();
    } else {
      tmphadec = MDirection::Convert(movingDir_p, outref1)();
    }
    MDirection actSourceDir = (*pointingToImage)(tmphadec);
    Vector<Double> actPix;
    directionCoord.toPixel(actPix, actSourceDir);

    //cout << row << " scan " << vb.scan()(row) << "xyPos " << xyPos << " xyposmovorig " << xyPosMovingOrig_p << " actPix " << actPix << endl;

    xyPos = xyPos + xyPosMovingOrig_p - actPix;
  }

  return result;
  // Convert to pixel coordinates
}

MDirection SDGrid::directionMeas(const MSPointingColumns& mspc, const Int& index){
  if (pointingDirCol_p == "TARGET") {
    return mspc.targetMeas(index);
  } else if (pointingDirCol_p == "POINTING_OFFSET") {
    if (!mspc.pointingOffsetMeasCol().isNull()) {
      return mspc.pointingOffsetMeas(index);
    }
    cerr << "No PONTING_OFFSET column in POINTING table" << endl;
  } else if (pointingDirCol_p == "SOURCE_OFFSET") {
    if (!mspc.sourceOffsetMeasCol().isNull()) {
      return mspc.sourceOffsetMeas(index);
    }
    cerr << "No SOURCE_OFFSET column in POINTING table" << endl;
  } else if (pointingDirCol_p == "ENCODER") {
    if (!mspc.encoderMeas().isNull()) {
      return mspc.encoderMeas()(index);
    }
    cerr << "No ENCODER column in POINTING table" << endl;
  }

  //default  return this
  return mspc.directionMeas(index);
  }

// for the cases, interpolation of the pointing direction requires
// when data sampling rate higher than the pointing data recording
// (e.g. fast OTF)
MDirection SDGrid::directionMeas(const MSPointingColumns& mspc, const Int& index,
                                 const Double& time){
  //spline interpolation
  if (isSplineInterpolationReady) {
    Int antid = mspc.antennaId()(index);
    if (interpolator->doSplineInterpolation(antid)) {
      return interpolator->interpolateDirectionMeasSpline(mspc, time, index, antid);
    }
  }

  //linear interpolation (as done before CAS-7911)
  Int index1, index2;
  if (time < mspc.time()(index)) {
    if (index > 0) {
      index1 = index-1;
      index2 = index;
    } else if (index == 0) {
      index1 = index;
      index2 = index+1;
    }
  } else {
    if (index < Int(mspc.nrow()-1)) {
      index1 = index;
      index2 = index+1;
    } else if (index == Int(mspc.nrow()-1) || (mspc.time()(index)-mspc.time()(index+1))>2*mspc.interval()(index)) {
      index1 = index-1;
      index2 = index;
    }
  }
  return interpolateDirectionMeas(mspc, time, index, index1, index2);
}

MDirection SDGrid::interpolateDirectionMeas(const MSPointingColumns& mspc,
                                            const Double& time,
                                            const Int& index,
                                            const Int& indx1,
                                            const Int& indx2){
  Vector<Double> dir1,dir2;
  Vector<Double> newdir(2),scanRate(2);
  Double dLon, dLat;
  Double ftime,ftime2,ftime1,dtime;
  MDirection newDirMeas;
  MDirection::Ref rf;
  Bool isfirstRefPt;

  if (indx1 == index) {
    isfirstRefPt = true;
  } else {
    isfirstRefPt = false;
  }

  if (pointingDirCol_p == "TARGET") {
    dir1 = mspc.targetMeas(indx1).getAngle("rad").getValue();
    dir2 = mspc.targetMeas(indx2).getAngle("rad").getValue();
  } else if (pointingDirCol_p == "POINTING_OFFSET") {
    if (!mspc.pointingOffsetMeasCol().isNull()) {
      dir1 = mspc.pointingOffsetMeas(indx1).getAngle("rad").getValue();
      dir2 = mspc.pointingOffsetMeas(indx2).getAngle("rad").getValue();
    } else {
      cerr << "No PONTING_OFFSET column in POINTING table" << endl;
    }
  } else if (pointingDirCol_p == "SOURCE_OFFSET") {
    if (!mspc.sourceOffsetMeasCol().isNull()) {
      dir1 = mspc.sourceOffsetMeas(indx1).getAngle("rad").getValue();
      dir2 = mspc.sourceOffsetMeas(indx2).getAngle("rad").getValue();
    } else {
      cerr << "No SOURCE_OFFSET column in POINTING table" << endl;
    }
  } else if (pointingDirCol_p == "ENCODER") {
    if (!mspc.encoderMeas().isNull()) {
      dir1 = mspc.encoderMeas()(indx1).getAngle("rad").getValue();
      dir2 = mspc.encoderMeas()(indx2).getAngle("rad").getValue();
    } else {
      cerr << "No ENCODER column in POINTING table" << endl;
    }
  } else {
    dir1 = mspc.directionMeas(indx1).getAngle("rad").getValue();
    dir2 = mspc.directionMeas(indx2).getAngle("rad").getValue();
  }

  dLon = dir2(0) - dir1(0);
  dLat = dir2(1) - dir1(1);
  ftime = floor(mspc.time()(indx1));
  ftime2 = mspc.time()(indx2) - ftime;
  ftime1 = mspc.time()(indx1) - ftime;
  dtime = ftime2 - ftime1;
  scanRate(0) = dLon/dtime;
  scanRate(1) = dLat/dtime;
  //scanRate(0) = dir2(0)/dtime-dir1(0)/dtime;
  //scanRate(1) = dir2(1)/dtime-dir1(1)/dtime;
  //Double delT = mspc.time()(index)-time;
  //cerr<<"index="<<index<<" dLat="<<dLat<<" dtime="<<dtime<<" delT="<< delT<<endl;
  //cerr<<"deldirlat="<<scanRate(1)*fabs(delT)<<endl;
  if (isfirstRefPt) {
    newdir(0) = dir1(0)+scanRate(0)*fabs(mspc.time()(index)-time);
    newdir(1) = dir1(1)+scanRate(1)*fabs(mspc.time()(index)-time);
    rf = mspc.directionMeas(indx1).getRef();
  } else {
    newdir(0) = dir2(0)-scanRate(0)*fabs(mspc.time()(index)-time);
    newdir(1) = dir2(1)-scanRate(1)*fabs(mspc.time()(index)-time);
    rf = mspc.directionMeas(indx2).getRef();
  }
  //default  return this
  Quantity rDirLon(newdir(0), "rad");
  Quantity rDirLat(newdir(1), "rad");
  newDirMeas = MDirection(rDirLon, rDirLat, rf);
  //cerr<<"newDirMeas rf="<<newDirMeas.getRefString()<<endl;
  //return mspc.directionMeas(index);
  return newDirMeas;
}

void SDGrid::pickWeights(const vi::VisBuffer2& vb, Matrix<Float>& weight){
  //break reference
  weight.resize();

  if (useImagingWeight_p) {
    weight.reference(vb.imagingWeight());
  } else {
    const Cube<Float> weightspec(vb.weightSpectrum());
    weight.resize(vb.nChannels(), vb.nRows());

    if (weightspec.nelements() == 0) {
      for (rownr_t k = 0; k < vb.nRows(); ++k) {
        //cerr << "nrow " << vb.nRow() << " " << weight.shape() << "  "  << weight.column(k).shape() << endl;
        weight.column(k).set(mean(vb.weight().column(k)));
      }
    } else {
      Int npol = weightspec.shape()(0);
      if (npol == 1) {
        for (rownr_t k = 0; k < vb.nRows(); ++k) {
          for (int chan = 0; chan < vb.nChannels(); ++chan) {
            weight(chan, k)=weightspec(0, chan, k);
          }
        }
      } else {
        for (rownr_t k = 0; k < vb.nRows(); ++k) {
          for (int chan = 0; chan < vb.nChannels(); ++chan) {
            weight(chan, k) = (weightspec(0, chan, k) + weightspec((npol-1), chan, k))/2.0f;
          }
        }
      }
    }
  }
}

void SDGrid::clipMinMax() {
  if (clipminmax_) {
    Bool gmin_b, gmax_b, wmin_b, wmax_b, np_b;
    const auto *gmin_p = gmin_.getStorage(gmin_b);
    const auto *gmax_p = gmax_.getStorage(gmax_b);
    const auto *wmin_p = wmin_.getStorage(wmin_b);
    const auto *wmax_p = wmax_.getStorage(wmax_b);
    const auto *np_p = npoints_.getStorage(np_b);

    Bool data_b, weight_b, sumw_b;
    auto data_p = griddedData.getStorage(data_b);
    auto weight_p = wGriddedData.getStorage(weight_b);
    auto sumw_p = sumWeight.getStorage(sumw_b);

    auto arrayShape = griddedData.shape();
    size_t num_xy = arrayShape.getFirst(2).product();
    size_t num_polchan = arrayShape.getLast(2).product();
    for (size_t i = 0; i < num_xy; ++i) {
      for (size_t j = 0; j < num_polchan; ++j) {
        auto k = i * num_polchan + j;
        if (np_p[k] == 1) {
          auto wt = wmin_p[k];
          data_p[k] = wt * gmin_p[k];
          weight_p[k] = wt;
          sumw_p[j] += wt;
        } else if (np_p[k] == 2) {
          auto wt = wmin_p[k];
          data_p[k] = wt * gmin_p[k];
          weight_p[k] = wt;
          sumw_p[j] += wt;
          wt = wmax_p[k];
          data_p[k] += wt * gmax_p[k];
          weight_p[k] += wt;
          sumw_p[j] += wt;
        }
      }
    }

    wGriddedData.putStorage(weight_p, weight_b);
    griddedData.putStorage(data_p, data_b);
    sumWeight.putStorage(sumw_p, sumw_b);

    npoints_.freeStorage(np_p, np_b);
    wmax_.freeStorage(wmax_p, wmax_b);
    wmin_.freeStorage(wmin_p, wmin_b);
    gmax_.freeStorage(gmax_p, gmax_b);
    gmin_.freeStorage(gmin_p, gmin_b);
  }
}

} //End of namespace refim
} //#End casa namespace
