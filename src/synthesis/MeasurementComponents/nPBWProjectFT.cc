// -*- C++ -*-
//# nPBWProjectFT.cc: Implementation of nPBWProjectFT class
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

#include <msvis/MSVis/VisibilityIterator.h>
#include <casacore/casa/Quanta/UnitMap.h>
#include <casacore/casa/Quanta/MVTime.h>
#include <casacore/casa/Quanta/UnitVal.h>
#include <casacore/measures/Measures/Stokes.h>
#include <casacore/coordinates/Coordinates/CoordinateSystem.h>
#include <casacore/coordinates/Coordinates/DirectionCoordinate.h>
#include <casacore/coordinates/Coordinates/SpectralCoordinate.h>
#include <casacore/coordinates/Coordinates/StokesCoordinate.h>
#include <casacore/coordinates/Coordinates/Projection.h>
#include <casacore/ms/MeasurementSets/MSColumns.h>
#include <casacore/ms/MeasurementSets/MSRange.h>
#include <casacore/casa/BasicSL/Constants.h>
#include <casacore/scimath/Mathematics/FFTServer.h>
#include <synthesis/MeasurementComponents/nPBWProjectFT.h>
#include <casacore/scimath/Mathematics/RigidVector.h>
#include <msvis/MSVis/StokesVector.h>
#include <synthesis/TransformMachines/StokesImageUtil.h>
#include <msvis/MSVis/VisBuffer.h>
#include <msvis/MSVis/VisSet.h>
#include <casacore/images/Images/ImageInterface.h>
#include <casacore/images/Images/ImageRegrid.h>
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

#include <synthesis/TransformMachines/VLACalcIlluminationConvFunc.h>
#include <synthesis/TransformMachines/IlluminationConvFunc.h>
#include <synthesis/MeasurementComponents/ExpCache.h>
#include <synthesis/MeasurementComponents/CExp.h>
#include <synthesis/TransformMachines/Utils.h>
#include <synthesis/TransformMachines/SynthesisError.h>
#include <casacore/measures/Measures/MEpoch.h>
#include <casacore/measures/Measures/MeasTable.h>
#include <casacore/scimath/Mathematics/MathFunc.h>

#include <casacore/casa/System/ProgressMeter.h>

#define CONVSIZE (1024*2)
#define CONVWTSIZEFACTOR 1.0
#define OVERSAMPLING 20
#define THRESHOLD 1E-4
#define USETABLES 0           // If equal to 1, use tabulated exp() and
			      // complex exp() functions.
#define MAXPOINTINGERROR 250.0 // Max. pointing error in arcsec used to
// determine the resolution of the
// tabulated exp() function.
using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN
  
#define NEED_UNDERSCORES
  extern "C" 
  {
    //
    // The Gridding Convolution Function (GCF) used by the underlying
    // gridder written in FORTRAN.
    //
    // The arguments must all be pointers and the value of the GCF at
    // the given (u,v) point is returned in the weight variable.  Making
    // this a function which returns a complex value (namely the weight)
    // has problems when called in FORTRAN - I (SB) don't understand
    // why.
    //
#if defined(NEED_UNDERSCORES)
#define nwcppeij nwcppeij_
#endif
    //
    //---------------------------------------------------------------
    //
    IlluminationConvFunc nwEij;
    void nwcppeij(Double *griduvw, Double *area,
		 Double *raoff1, Double *decoff1,
		 Double *raoff2, Double *decoff2, 
		 Int *doGrad,
		 Complex *weight,
		 Complex *dweight1,
		 Complex *dweight2,
		 Double *currentCFPA)
    {
      Complex w,d1,d2;
      nwEij.getValue(griduvw, raoff1, raoff2, decoff1, decoff2,
		    area,doGrad,w,d1,d2,*currentCFPA);
      *weight   = w;
      *dweight1 = d1;
      *dweight2 = d2;
    }
  }
  //
  //---------------------------------------------------------------
  //
#define FUNC(a)  (((a)))
  nPBWProjectFT::nPBWProjectFT(Int nWPlanes, Long icachesize, 
			       String& cfCacheDirName,
			       Bool applyPointingOffset,
			       Bool doPBCorr,
			       Int itilesize, 
			       Float /*paSteps*/,
			       Float pbLimit,
			       Bool usezero)
    : FTMachine(), padding_p(1.0), nWPlanes_p(nWPlanes),
      imageCache(0), cachesize(icachesize), tilesize(itilesize),
      gridder(0), isTiled(false), arrayLattice( ), lattice( ), 
      maxAbsData(0.0), centerLoc(IPosition(4,0)), offsetLoc(IPosition(4,0)),
      mspc(0), msac(0), pointingToImage(0), usezero_p(usezero),
      doPBCorrection(doPBCorr),
      Second("s"),Radian("rad"),Day("d"), noOfPASteps(0),
      pbNormalized(false),resetPBs(true), rotateAperture_p(true),
      cfCache(), paChangeDetector(), cfStokes(),Area(), 
      avgPBSaved(false),avgPBReady(false)
  {
    epJ=NULL;
    convSize=0;
    tangentSpecified_p=false;
    lastIndex_p=0;
    paChangeDetector.reset();
    pbLimit_p=pbLimit;
    //
    // Get various parameters from the visibilities.  
    //
    bandID_p=-1;
    if (applyPointingOffset) doPointing=1; else doPointing=0;

    convFuncCacheReady=false;
    PAIndex = -1;
    maxConvSupport=-1;  
    //
    // Set up the Conv. Func. disk cache manager object.
    //
    cfCache.setCacheDir(cfCacheDirName.data());
    cfCache.initCache();
    convSampling=OVERSAMPLING;
    convSize=CONVSIZE;
    Long hostRAM = (HostInfo::memoryTotal(true)*1024); // In bytes
    hostRAM = hostRAM/(sizeof(Float)*2); // In complex pixels
    if (cachesize > hostRAM) cachesize=hostRAM;
  }
  //
  //---------------------------------------------------------------
  //
  nPBWProjectFT::nPBWProjectFT(const RecordInterface& stateRec)
    : FTMachine(),Second("s"),Radian("rad"),Day("d")
  {
    //
    // Construct from the input state record
    //
    String error;
    
    if (!fromRecord(error, stateRec)) {
      throw (AipsError("Failed to create nPBWProjectFT: " + error));
    };
    bandID_p = -1;
    PAIndex = -1;
    maxConvSupport=-1;
    convSampling=OVERSAMPLING;
    convSize=CONVSIZE;
  }
  //
  //----------------------------------------------------------------------
  //
  nPBWProjectFT::nPBWProjectFT(const nPBWProjectFT& other):FTMachine()
  {
    operator=(other);
  }
  //
  //---------------------------------------------------------------
  //
  nPBWProjectFT& nPBWProjectFT::operator=(const nPBWProjectFT& other)
  {
    if(this!=&other) 
      {
	//Do the base parameters
	FTMachine::operator=(other);

	
	padding_p=other.padding_p;
	
	nWPlanes_p=other.nWPlanes_p;
	imageCache=other.imageCache;
	cachesize=other.cachesize;
	tilesize=other.tilesize;
	cfRefFreq_p = other.cfRefFreq_p;
	if(other.gridder==0) gridder=0;
	else
	  {
	    uvScale.resize();
	    uvOffset.resize();
	    uvScale=other.uvScale;
	    uvOffset=other.uvOffset;
	    gridder = new ConvolveGridder<Double, Complex>(IPosition(2, nx, ny),
							   uvScale, uvOffset,
							   "SF");
	  }

	isTiled=other.isTiled;
	lattice=0;
	arrayLattice=0;

	maxAbsData=other.maxAbsData;
	centerLoc=other.centerLoc;
	offsetLoc=other.offsetLoc;
	pointingToImage=other.pointingToImage;
	usezero_p=other.usezero_p;

	
	padding_p=other.padding_p;
	nWPlanes_p=other.nWPlanes_p;
	imageCache=other.imageCache;
	cachesize=other.cachesize;
	tilesize=other.tilesize;
	isTiled=other.isTiled;
	maxAbsData=other.maxAbsData;
	centerLoc=other.centerLoc;
	offsetLoc=other.offsetLoc;
	mspc=other.mspc;
	msac=other.msac;
	pointingToImage=other.pointingToImage;
	usezero_p=other.usezero_p;
	doPBCorrection = other.doPBCorrection;
	maxConvSupport= other.maxConvSupport;

	epJ=other.epJ;
	convSize=other.convSize;
	lastIndex_p=other.lastIndex_p;
	paChangeDetector=other.paChangeDetector;
	pbLimit_p=other.pbLimit_p;
	//
	// Get various parameters from the visibilities.  
	//
	bandID_p = other.bandID_p;
	doPointing=other.doPointing;

	convFuncCacheReady=other.convFuncCacheReady;
	PAIndex = other.PAIndex;
	maxConvSupport=other.maxConvSupport;
	//
	// Set up the Conv. Func. disk cache manager object.
	//
	cfCache=other.cfCache;
	convSampling=other.convSampling;
	convSize=other.convSize;
	cachesize=other.cachesize;
    
	resetPBs=other.resetPBs;
	pbNormalized=other.pbNormalized;
	currentCFPA=other.currentCFPA;
	lastPAUsedForWtImg = other.lastPAUsedForWtImg;
	cfStokes=other.cfStokes;
	Area=other.Area;
	avgPB = other.avgPB;
	avgPBReady = other.avgPBReady;
      };
    return *this;
  };
  //
  //---------------------------------------------------------------------
  //
  int nPBWProjectFT::getVisParams(const VisBuffer& vb)
  {
    Double Freq;
    Vector<String> telescopeNames=vb.msColumns().observation().telescopeName().getColumn();
    for(uInt nt=0;nt<telescopeNames.nelements();nt++)
      {
	if ((telescopeNames(nt) != "VLA") && (telescopeNames(nt) != "EVLA"))
	  {
	    String mesg="pbwproject algorithm can handle only (E)VLA antennas for now.\n";
	    mesg += "Erroneous telescope name = " + telescopeNames(nt) + ".";
	    SynthesisError err(mesg);
	    throw(err);
	  }
	if (telescopeNames(nt) != telescopeNames(0))
	  {
	    String mesg="pbwproject algorithm does not (yet) handle inhomogeneous arrays!\n";
	    mesg += "Not yet a \"priority\"!!";
	    SynthesisError err(mesg);
	    throw(err);
	  }
      }
    //    MSSpWindowColumns mssp(vb.msColumns().spectralWindow());
    Freq = vb.msColumns().spectralWindow().refFrequency()(0);
    Diameter_p=0;
    Nant_p     = vb.msColumns().antenna().nrow();
    for (Int i=0; i < Nant_p; i++)
      if (!vb.msColumns().antenna().flagRow()(i))
	{
	  Diameter_p = vb.msColumns().antenna().dishDiameter()(i);
	  break;
	}
    if (Diameter_p == 0)
      {
	logIO() << LogOrigin("nPBWProjectFT", "getVisParams")
		<< "No valid or finite sized antenna found in the antenna table. "
		<< "Assuming diameter = 25m."
		<< LogIO::WARN
		<< LogIO::POST;
	Diameter_p=25.0;
      }
    
    Double Lambda=C::c/Freq;
    HPBW = Lambda/(Diameter_p*sqrt(log(2.0)));
    sigma = 1.0/(HPBW*HPBW);
    nwEij.setSigma(sigma);
    Int bandID = BeamCalc::Instance()->getBandID(Freq,telescopeNames(0),"");
    //    Int bandID = getVLABandID(Freq,telescopeNames(0));
    return bandID;
  }
  //
  //----------------------------------------------------------------------
  //
  void nPBWProjectFT::init() 
  {
    nx    = image->shape()(0);
    ny    = image->shape()(1);
    npol  = image->shape()(2);
    nchan = image->shape()(3);
    
    if(image->shape().product()>cachesize) 
      isTiled=true;
    else 
      isTiled=false;
    
    sumWeight.resize(npol, nchan);
    
    wConvSize=max(1, nWPlanes_p);
    if (!convFuncCacheReady) 
      {
	if (convSupport.shape()(0) != wConvSize)
	  {
	    convSupport.resize(wConvSize,1,1,true);
	    convSupport=0;
	  }
      }
    
    uvScale.resize(3);
    uvScale=0.0;
    uvScale(0)=Float(nx)*image->coordinates().increment()(0); 
    uvScale(1)=Float(ny)*image->coordinates().increment()(1); 
    uvScale(2)=Float(wConvSize)*abs(image->coordinates().increment()(0));
    
    uvOffset.resize(3);
    uvOffset(0)=nx/2;
    uvOffset(1)=ny/2;
    uvOffset(2)=0;
    
    if(gridder) delete gridder; gridder=0;
    gridder = new ConvolveGridder<Double, Complex>(IPosition(2, nx, ny),
						   uvScale, uvOffset,
						   "SF");
    
    // Set up image cache needed for gridding. 
    if(imageCache) delete imageCache;   imageCache=0;
    
    // The tile size should be large enough that the
    // extended convolution function can fit easily
    if(isTiled) 
      {
	Float tileOverlap=0.5;
	tilesize=min(256,tilesize);
	IPosition tileShape=IPosition(4,tilesize,tilesize,npol,nchan);
	Vector<Float> tileOverlapVec(4);
	tileOverlapVec=0.0;
	tileOverlapVec(0)=tileOverlap;
	tileOverlapVec(1)=tileOverlap;
	if (sizeof(long) < 4)  // 32-bit machine
	  {
	    Int tmpCacheVal=static_cast<Int>(cachesize);
	    imageCache=new LatticeCache <Complex> (*image, tmpCacheVal, tileShape, 
						   tileOverlapVec,
						   (tileOverlap>0.0));
	  }
	else  // 64-bit machine
	  {
	    Long tmpCacheVal=cachesize;
	    imageCache=new LatticeCache <Complex> (*image, tmpCacheVal, tileShape, 
						   tileOverlapVec,
						   (tileOverlap>0.0));
	  }
      }
    
#if(USETABLES)
    Double StepSize;
    Int N=500000;
    StepSize = abs((((2*nx)/uvScale(0))/(sigma) + 
		    MAXPOINTINGERROR*1.745329E-02*(sigma)/3600.0))/N;
    if (!nwEij.isReady())
      {
	logIO() << LogOrigin("nPBWProjectFT","init")
		<< "Making lookup table for exp function with a resolution of " 
		<< StepSize << " radians.  "
		<< "Memory used: " << sizeof(Float)*N/(1024.0*1024.0)<< " MB." 
		<< LogIO::NORMAL 
		<<LogIO::POST;
	
	nwEij.setSigma(sigma);
	nwEij.initExpTable(N,StepSize);
	//    ExpTab.build(N,StepSize);
	
	logIO() << LogOrigin("nPBWProjectFT","init")
		<< "Making lookup table for complex exp function with a resolution of " 
		<< 2*M_PI/N << " radians.  "
		<< "Memory used: " << 2*sizeof(Float)*N/(1024.0*1024.0) << " MB." 
		<< LogIO::NORMAL
		<< LogIO::POST;
	nwEij.initCExpTable(N);
	//    CExpTab.build(N);
      }
#endif
    //    vpSJ->reset();
    paChangeDetector.reset();
    makingPSF = false;
  }
  //
  //---------------------------------------------------------------
  //
  // This is nasty, we should use CountedPointers here.
  nPBWProjectFT::~nPBWProjectFT() 
  {
      if(imageCache) delete imageCache; imageCache=0;
      if(gridder) delete gridder; gridder=0;
      
      Int NCF=convFuncCache.nelements();
      for(Int i=0;i<NCF;i++) delete convFuncCache[i];
      NCF=convWeightsCache.nelements();
      for(Int i=0;i<NCF;i++) delete convWeightsCache[i];
  }
  //
  //---------------------------------------------------------------
  //
  Int nPBWProjectFT::makePBPolnCoords(CoordinateSystem& squintCoord,const VisBuffer&vb)
  {
    //
    // Make an image with circular polarization axis.
    //
    Int NPol=0,M,N=0;
    M=polMap.nelements();
    for(Int i=0;i<M;i++) if (polMap(i) > -1) NPol++;
    Vector<Int> poln(NPol);
    
    Int index;
    Vector<Int> inStokes;
    index = squintCoord.findCoordinate(Coordinate::STOKES);
    inStokes = squintCoord.stokesCoordinate(index).stokes();
    N = 0;
    try
      {
	for(Int i=0;i<M;i++) if (polMap(i) > -1) {poln(N) = vb.corrType()(i);N++;}
	StokesCoordinate polnCoord(poln);
	Int StokesIndex = squintCoord.findCoordinate(Coordinate::STOKES);
	squintCoord.replaceCoordinate(polnCoord,StokesIndex);
	cfStokes = poln;
      }
    catch(AipsError& x)
      {
	throw(SynthesisFTMachineError("Likely cause: Discrepancy between the poln. "
				      "axis of the data and the image specifications."));
      }
    
    return NPol;
  }
  //
  //---------------------------------------------------------------
  //
  MDirection::Convert nPBWProjectFT::makeCoordinateMachine(const VisBuffer& vb,
							   const MDirection::Types& From,
							   const MDirection::Types& To,
							   MEpoch& last)
  {
    Double time = getCurrentTimeStamp(vb);
    
    MEpoch epoch(Quantity(time,Second),MEpoch::TAI);
    //  epoch = MEpoch(Quantity(time,Second),MEpoch::TAI);
    //
    // ...now make an object to hold the observatory position info...
    //
    MPosition pos;
    String ObsName=vb.msColumns().observation().telescopeName()(vb.arrayId());
    
    if (!MeasTable::Observatory(pos,ObsName))
      throw(AipsError("Observatory position for "+ ObsName + " not found"));
    //
    // ...now make a Frame object out of the observatory position and
    // time objects...
    //
    MeasFrame frame(epoch,pos);
    //
    // ...finally make the convert machine.
    //
    MDirection::Convert mac(MDirection::Ref(From,frame),
			    MDirection::Ref(To,frame));
    
    MEpoch::Convert toLAST = MEpoch::Convert(MEpoch::Ref(MEpoch::TAI,frame),
					     MEpoch::Ref(MEpoch::LAST,frame));
    last = toLAST(epoch);
    
    return mac;
  }
  //
  //---------------------------------------------------------------
  //
  int nPBWProjectFT::findPointingOffsets(const VisBuffer& vb, 
					Array<Float> &l_off,
					Array<Float> &m_off,
					Bool Evaluate)
  {

    //    throw(AipsError("PBWProject::findPointingOffsets temporarily disabled. (gmoellen 06Nov10)"));

    Int NAnt = 0;
    MEpoch LAST;
    Double thisTime = getCurrentTimeStamp(vb);
    //    Array<Float> pointingOffsets = epJ->nearest(thisTime);
    if (epJ==NULL) return 0;
    Array<Float> pointingOffsets; epJ->nearest(thisTime,pointingOffsets);
    NAnt=pointingOffsets.shape()(2);
    l_off.resize(IPosition(3,1,1,NAnt)); // Poln x NChan x NAnt 
    m_off.resize(IPosition(3,1,1,NAnt)); // Poln x NChan x NAnt 
    // Can't figure out how to do the damn slicing of [Pol,NChan,NAnt,1] array
    // into [Pol,NChan,NAnt] array
    //
    //    l_off = pointingOffsets(Slicer(IPosition(4,0,0,0,0),IPosition(4,1,1,NAnt+1,0)));
    //    m_off = pointingOffsets(Slicer(IPosition(4,1,0,0,0),IPosition(4,1,1,NAnt+1,0)));
    IPosition tndx(3,0,0,0), sndx(4,0,0,0,0);
    for(tndx(2)=0;tndx(2)<NAnt; tndx(2)++,sndx(2)++)
      //    for(Int j=0;j<NAnt;j++)
      {
	// l_off(IPosition(3,0,0,j)) = pointingOffsets(IPosition(4,0,0,j,0));
	// m_off(IPosition(3,0,0,j)) = pointingOffsets(IPosition(4,1,0,j,0));

	sndx(0)=0; l_off(tndx) = pointingOffsets(sndx);
	sndx(0)=2; m_off(tndx) = pointingOffsets(sndx);
      }
    return NAnt;
    if (!Evaluate) return NAnt;
    
    //  cout << "AzEl Offsets: " << pointingOffsets << endl;
    //
    // Make a Coordinate Conversion Machine to go from (Az,El) to
    // (HA,Dec).
    //
    MDirection::Convert toAzEl = makeCoordinateMachine(vb,MDirection::HADEC,
						       MDirection::AZEL,
						       LAST);
    MDirection::Convert toHADec = makeCoordinateMachine(vb,MDirection::AZEL,
							MDirection::HADEC,
							LAST);
    //
    // ...and now hope that it all works and works correctly!!!
    //
    Quantity dAz(0,Radian),dEl(0,Radian);
    //
    // An array of shape [2,1,1]!
    //
    Array<Double> phaseDir = vb.msColumns().field().phaseDir().getColumn();
    Double RA0   = phaseDir(IPosition(3,0,0,0));
    Double Dec0  = phaseDir(IPosition(3,1,0,0));
    //  
    // Compute reference (HA,Dec)
    //
    Double LST   = LAST.get(Day).getValue();
    Double SDec0 = sin(Dec0), CDec0=cos(Dec0);
    LST -= floor(LST); // Extract the fractional day
    LST *= 2*C::pi;// Convert to Raidan
    
    Double HA0;
    HA0 = LST - RA0;
    Quantity QHA0(HA0,Radian), QDEC0(Dec0,Radian);
    //
    // Convert reference (HA,Dec) to reference (Az,El)
    //
    MDirection PhaseCenter(QHA0, QDEC0,MDirection::Ref(MDirection::HADEC));
    MDirection AzEl0 = toAzEl(PhaseCenter);
    
    MDirection tmpHADec = toHADec(AzEl0);
    
    Double Az0_Rad = AzEl0.getAngle(Radian).getValue()(0);
    Double El0_Rad = AzEl0.getAngle(Radian).getValue()(1);
    
    //
    // Convert the antenna pointing offsets from (Az,El)-->(RA,Dec)-->(l,m) 
    //
    
    for(IPosition n(3,0,0,0);n(2)<=NAnt;n(2)++)
      {
	//
	// From (Az,El) -> (HA,Dec)
	//
	// Add (Az,El) offsets to the reference (Az,El)
	//
	dAz.setValue(l_off(n)+Az0_Rad);  dEl.setValue(m_off(n)+El0_Rad);
	//      dAz.setValue(0.0+Az0_Rad);  dEl.setValue(0.0+El0_Rad);
	MDirection AzEl(dAz,dEl,MDirection::Ref(MDirection::AZEL));
	//
	// Convert offsetted (Az,El) to (HA,Dec) and then to (RA,Dec)
	//
	MDirection HADec = toHADec(AzEl);
	Double HA,Dec,RA, dRA;
	HA  = HADec.getAngle(Radian).getValue()(0);
	Dec = HADec.getAngle(Radian).getValue()(1);
	RA  = LST - HA;
	dRA = RA - RA0;
	//
	// Convert offsetted (RA,Dec) -> (l,m)
	//
	l_off(n)  = sin(dRA)*cos(Dec);
	m_off(n) = sin(Dec)*CDec0-cos(Dec)*SDec0*cos(dRA);
	//      cout << "FindOff: " << n(2) << " " << l_offsets(n) << " " << m_offsets(n) << endl;
	
	//       cout << l_off(n) << " " << m_off(n) << " "
	// 	   << " " << HA << " " << Dec
	// 	   << " " << LST << " " << RA0 << " " << Dec0 
	// 	   << " " << RA << " " << Dec
	// 	   << endl;
      }
    
    return NAnt+1;
  }
  //
  //---------------------------------------------------------------
  //
  int nPBWProjectFT::findPointingOffsets(const VisBuffer& vb, 
					 Cube<Float>& pointingOffsets,
					Array<Float> &l_off,
					Array<Float> &m_off,
					Bool Evaluate)
  {
    Int NAnt = 0;
    // TBD: adapt the following to VisCal mechanism:
    MEpoch LAST;
    
    NAnt=pointingOffsets.shape()(2);
    l_off.resize(IPosition(3,2,1,NAnt));
    m_off.resize(IPosition(3,2,1,NAnt));
    IPosition ndx(3,0,0,0),ndx1(3,0,0,0);
    for(ndx(2)=0;ndx(2)<NAnt;ndx(2)++)
      {
	ndx1=ndx;
	ndx(0)=0;ndx1(0)=0;	l_off(ndx)  = pointingOffsets(ndx1);//Axis_0,Pol_0,Ant_i
	ndx(0)=1;ndx1(0)=1;	l_off(ndx)  = pointingOffsets(ndx1);//Axis_0,Pol_1,Ant_i
	ndx(0)=0;ndx1(0)=2;	m_off(ndx)  = pointingOffsets(ndx1);//Axis_1,Pol_0,Ant_i
	ndx(0)=1;ndx1(0)=3;	m_off(ndx)  = pointingOffsets(ndx1);//Axis_1,Pol_1,Ant_i
      }

//     l_off  = pointingOffsets(IPosition(3,0,0,0),IPosition(3,0,0,NAnt));
//     m_off = pointingOffsets(IPosition(3,1,0,0),IPosition(3,1,0,NAnt));
    /*
    IPosition shp(pointingOffsets.shape());
    IPosition shp1(l_off.shape()),shp2(m_off.shape());
    for(Int ii=0;ii<NAnt;ii++)
      {
	IPosition ndx(3,0,0,0);
	ndx(2)=ii;
	cout << "Pointing Offsets: " << ii << " " 
	     << l_off(ndx)*57.295*60.0 << " " 
	     << m_off(ndx)*57.295*60.0 << endl;
      }
    */
    return NAnt;
    if (!Evaluate) return NAnt;
    
    //
    // Make a Coordinate Conversion Machine to go from (Az,El) to
    // (HA,Dec).
    //
    MDirection::Convert toAzEl = makeCoordinateMachine(vb,MDirection::HADEC,
						       MDirection::AZEL,
						       LAST);
    MDirection::Convert toHADec = makeCoordinateMachine(vb,MDirection::AZEL,
							MDirection::HADEC,
							LAST);
    //
    // ...and now hope that it all works and works correctly!!!
    //
    Quantity dAz(0,Radian),dEl(0,Radian);
    //
    // An array of shape [2,1,1]!
    //
    Array<Double> phaseDir = vb.msColumns().field().phaseDir().getColumn();
    Double RA0   = phaseDir(IPosition(3,0,0,0));
    Double Dec0  = phaseDir(IPosition(3,1,0,0));
    //  
    // Compute reference (HA,Dec)
    //
    Double LST   = LAST.get(Day).getValue();
    Double SDec0 = sin(Dec0), CDec0=cos(Dec0);
    LST -= floor(LST); // Extract the fractional day
    LST *= 2*C::pi;// Convert to Raidan
    
    Double HA0;
    HA0 = LST - RA0;
    Quantity QHA0(HA0,Radian), QDEC0(Dec0,Radian);
    //
    // Convert reference (HA,Dec) to reference (Az,El)
    //
    MDirection PhaseCenter(QHA0, QDEC0,MDirection::Ref(MDirection::HADEC));
    MDirection AzEl0 = toAzEl(PhaseCenter);
    
    MDirection tmpHADec = toHADec(AzEl0);
    
    Double Az0_Rad = AzEl0.getAngle(Radian).getValue()(0);
    Double El0_Rad = AzEl0.getAngle(Radian).getValue()(1);
    
    //
    // Convert the antenna pointing offsets from (Az,El)-->(RA,Dec)-->(l,m) 
    //
    
    for(IPosition n(3,0,0,0);n(2)<=NAnt;n(2)++)
      {
	//
	// From (Az,El) -> (HA,Dec)
	//
	// Add (Az,El) offsets to the reference (Az,El)
	//
	dAz.setValue(l_off(n)+Az0_Rad);  dEl.setValue(m_off(n)+El0_Rad);
	//      dAz.setValue(0.0+Az0_Rad);  dEl.setValue(0.0+El0_Rad);
	MDirection AzEl(dAz,dEl,MDirection::Ref(MDirection::AZEL));
	//
	// Convert offsetted (Az,El) to (HA,Dec) and then to (RA,Dec)
	//
	MDirection HADec = toHADec(AzEl);
	Double HA,Dec,RA, dRA;
	HA  = HADec.getAngle(Radian).getValue()(0);
	Dec = HADec.getAngle(Radian).getValue()(1);
	RA  = LST - HA;
	dRA = RA - RA0;
	//
	// Convert offsetted (RA,Dec) -> (l,m)
	//
	l_off(n)  = sin(dRA)*cos(Dec);
	m_off(n) = sin(Dec)*CDec0-cos(Dec)*SDec0*cos(dRA);
	//      cout << "FindOff: " << n(2) << " " << l_offsets(n) << " " << m_offsets(n) << endl;
	
	//       cout << l_off(n) << " " << m_off(n) << " "
	// 	   << " " << HA << " " << Dec
	// 	   << " " << LST << " " << RA0 << " " << Dec0 
	// 	   << " " << RA << " " << Dec
	// 	   << endl;
      }
    
    return NAnt+1;
  }
  //
  //---------------------------------------------------------------
  //
  void nPBWProjectFT::makeSensitivityImage(Lattice<Complex>& wtImage,
					   ImageInterface<Float>& sensitivityImage,
					   const Matrix<Float>& sumWt,
					   const Bool& doFFTNorm)
  {
    Bool doSumWtNorm=true;
    if (sumWt.shape().nelements()==0) doSumWtNorm=false;

    if ((sumWt.shape().nelements() < 2) || 
	(sumWt.shape()(0) != wtImage.shape()(2)) || 
	(sumWt.shape()(1) != wtImage.shape()(3)))
      throw(AipsError("makeSensitivityImage(): "
		      "Sum of weights per poln and chan required"));
    Float sumWtVal=1.0;

    LatticeFFT::cfft2d(wtImage,false);
    Int sizeX=wtImage.shape()(0), sizeY=wtImage.shape()(1);
    sensitivityImage.resize(wtImage.shape());
    Array<Float> senBuf;
    sensitivityImage.get(senBuf,false);
    ArrayLattice<Float> senLat(senBuf, true);

    //
    // Copy one 2D plane at a time, normalizing by the sum of weights
    // and possibly 2D FFT.
    //
    // Set up Lattice iteratos on wtImage and sensitivityImage
    //
    IPosition axisPath(4, 0, 1, 2, 3);
    IPosition cursorShape(4, sizeX, sizeY, 1, 1);
    LatticeStepper wtImStepper(wtImage.shape(), cursorShape, axisPath);
    LatticeIterator<Complex> wtImIter(wtImage, wtImStepper);
    LatticeStepper senImStepper(senLat.shape(), cursorShape, axisPath);
    LatticeIterator<Float> senImIter(senLat, senImStepper);
    //
    // Iterate over channel and polarization axis
    //
    if (!doFFTNorm) sizeX=sizeY=1;
    for(wtImIter.reset(),senImIter.reset();  !wtImIter.atEnd(); wtImIter++,senImIter++) 
      {
	Int pol=wtImIter.position()(2), chan=wtImIter.position()(3);
	if (doSumWtNorm) sumWtVal=sumWt(pol,chan);
	senImIter.rwCursor() = (real(wtImIter.rwCursor())
				*Float(sizeX)*Float(sizeY)/sumWtVal);
      }
    //
    // The following code is averaging RR and LL planes and writing
    // the result to back to both planes.  This needs to be
    // generalized for full-pol case.
    //
    IPosition start0(4,0,0,0,0), start1(4,0,0,1,0), length(4,sizeX,sizeY,1,1);
    Slicer slicePol0(start0,length), slicePol1(start1,length);
    Array<Float> polPlane0, polPlane1;
    senLat.getSlice(polPlane0,slicePol0);
    senLat.getSlice(polPlane1,slicePol1);
    polPlane0=(polPlane0+polPlane1)/2.0;
    polPlane1=polPlane0;
    // senLat.putSlice(polPlane0,IPosition(4,0,0,0,0));
    // senLat.putSlice(polPlane1,IPosition(4,0,0,1,0));
    // cerr << "Pol0: " << polPlane0.shape() << " " << max(polPlane0) << endl;
    // cerr << "Pol1: " << polPlane1.shape() << " " << max(polPlane1) << endl;
  }
  //
  //---------------------------------------------------------------
  //
  void nPBWProjectFT::normalizeAvgPB()
  {
    if (!pbNormalized)
      {
	pbPeaks.resize(avgPB.shape()(2),true);
	if (makingPSF) pbPeaks = 1.0;
	else pbPeaks /= (Float)noOfPASteps;
	pbPeaks = 1.0;
	logIO() << LogOrigin("nPBWProjectFT", "normalizeAvgPB")  
		<< "Normalizing the average PBs to " << 1.0
		<< LogIO::NORMAL
		<< LogIO::POST;
	
	IPosition avgPBShape(avgPB.shape()),ndx(4,0,0,0,0);
	Vector<Float> peak(avgPBShape(2));
	
	Bool isRefF;
	Array<Float> avgPBBuf;
	isRefF=avgPB.get(avgPBBuf);
	
	Float pbMax = max(avgPBBuf);

	ndx=0;
	for(ndx(1)=0;ndx(1)<avgPBShape(1);ndx(1)++)
	  for(ndx(0)=0;ndx(0)<avgPBShape(0);ndx(0)++)
	    {
	      IPosition plane1(ndx);
	      plane1=ndx;
	      plane1(2)=1; // The other poln. plane
	      avgPBBuf(ndx) = (avgPBBuf(ndx) + avgPBBuf(plane1))/2.0;
	    }
	for(ndx(1)=0;ndx(1)<avgPBShape(1);ndx(1)++)
	  for(ndx(0)=0;ndx(0)<avgPBShape(0);ndx(0)++)
	    {
	      IPosition plane1(ndx);
	      plane1=ndx;
	      plane1(2)=1; // The other poln. plane
	      avgPBBuf(plane1) = avgPBBuf(ndx);
	    }
	if (fabs(pbMax-1.0) > 1E-3)
	  {
	    //	    avgPBBuf = avgPBBuf/noOfPASteps;
	    for(ndx(3)=0;ndx(3)<avgPBShape(3);ndx(3)++)
	      for(ndx(2)=0;ndx(2)<avgPBShape(2);ndx(2)++)
		{
		  peak(ndx(2)) = 0;
		  for(ndx(1)=0;ndx(1)<avgPBShape(1);ndx(1)++)
		    for(ndx(0)=0;ndx(0)<avgPBShape(0);ndx(0)++)
		      if (abs(avgPBBuf(ndx)) > peak(ndx(2)))
			peak(ndx(2)) = avgPBBuf(ndx);
	      
		  for(ndx(1)=0;ndx(1)<avgPBShape(1);ndx(1)++)
		    for(ndx(0)=0;ndx(0)<avgPBShape(0);ndx(0)++)
		      avgPBBuf(ndx) *= (pbPeaks(ndx(2))/peak(ndx(2)));
		}
	    if (isRefF) avgPB.put(avgPBBuf);
	  }
      }
    pbNormalized = true;
  }
  //
  //---------------------------------------------------------------
  //
  Bool nPBWProjectFT::makeAveragePB0(const VisBuffer& vb, 
				     const ImageInterface<Complex>& image,
				     Int& /*polInUse*/,
				     //TempImage<Float>& thesquintPB,
				     TempImage<Float>& theavgPB)
  {
    TempImage<Float> localPB;
    
    logIO() << LogOrigin("nPBWProjecFT","makeAveragePB")
	    << LogIO::NORMAL;
    
    localPB.resize(image.shape()); localPB.setCoordinateInfo(image.coordinates());
    localPB.setMaximumCacheSize(cachesize);
    // cerr << "Max. cache size = " << localPB.maximumCacheSize() << " " << cachesize << endl;
    //
    // If this is the first time, resize the average PB
    //
    if (resetPBs)
      {
	logIO() << "Initializing the average PBs"
		<< LogIO::NORMAL
		<< LogIO::POST;
	theavgPB.resize(localPB.shape()); 
	theavgPB.setCoordinateInfo(localPB.coordinates());
	theavgPB.set(0.0);
	noOfPASteps = 0;
	pbPeaks.resize(theavgPB.shape()(2));
	pbPeaks.set(0.0);
	resetPBs=false;
      }
    //
    // Make the Stokes PB
    //
    localPB.set(1.0);

    // The following replaces the simple vlaPB.applyPB() call.
    // The functional interface of VLACalcIllumination was
    // modified to apply one polarization PB at a time.  As a
    // result, the co-ord. sys. of the image going to applyPB()
    // should have only one Poln. plane.
    //
    // Before this change, the localPB image was directly sent to
    // applyPB(). Now, in the code below, we go over the poln. planes
    // of this image, make a temp. image with one poln. planes of
    // localPB at a time, applyPB() on the temp. image, and copy the
    // result in the appropriate planes of the localPB image.  The rest
    // of the code therefore does not see a difference.
    {
      VLACalcIlluminationConvFunc vlaPB;
      if (bandID_p == -1) bandID_p=getVisParams(vb);
      CoordinateSystem coords=localPB.coordinates();
      //----------------------------------------------------------------------
      IPosition PolnPlane(4,0,0,0,0);
      CoordinateSystem singlePolCoords=coords;
      Int index, outNdx, whichPolPlane=0;
      Vector<Int> inStokes, outStokes(1);
      index = coords.findCoordinate(Coordinate::STOKES);
      inStokes = coords.stokesCoordinate(index).stokes();
      for (uInt i=0; i<inStokes.nelements(); i++)
	{
	  outStokes(0)=inStokes(i);
	  // Make a temp. image with a single Stokes along Pol. axis.
	  StokesCoordinate polnCoord(outStokes);
	  outNdx = singlePolCoords.findCoordinate(Coordinate::STOKES);
	  singlePolCoords.replaceCoordinate(polnCoord, outNdx);
	  IPosition singlePolShape = localPB.shape();
	  singlePolShape(2)=1;
	  Bool doSquint=false;
	  {
	    TempImage<Float> singlePolImg(singlePolShape, singlePolCoords);
	    // Copy screen to the single pol. image.  Apply PB to it.  Copy 
	    // the single pol. image plane to the twoDPB image.
	    singlePolImg.set(1.0);
	    Double pa=getPA(vb);
	    Vector<Double> chanFreq = vb.frequency();
	    //freqHi = max(chanFreq);
	    Double Freq = max(chanFreq);

	    vlaPB.applyPB(singlePolImg, pa, bandID_p, doSquint, Freq);
	    PolnPlane(2)=whichPolPlane;   localPB.putSlice(singlePolImg.get(), PolnPlane);
	  }
	  whichPolPlane++;
	}
      //----------------------------------------------------------------------

      //      vlaPB.applyPB(localPB, vb, bandID_p);
    }
    
    IPosition twoDPBShape(localPB.shape());
    TempImage<Complex> localTwoDPB(twoDPBShape,localPB.coordinates());
    localTwoDPB.setMaximumCacheSize(cachesize);
    Float peak=0;
    Int NAnt;
    noOfPASteps++;
    NAnt=1;
   
//     logIO() << " Shape of localPB Cube : " << twoDPBShape << LogIO::POST;
//     logIO() << " Shape of avgPB Cube : " << theavgPB.shape() << LogIO::POST;

    for(Int ant=0;ant<NAnt;ant++)
      { //Ant loop
	{
	  IPosition ndx(4,0,0,0,0);
	  for(ndx(0)=0; ndx(0)<twoDPBShape(0); ndx(0)++)
	    for(ndx(1)=0; ndx(1)<twoDPBShape(1); ndx(1)++)
	      for(ndx(2)=0; ndx(2)<twoDPBShape(2); ndx(2)++)
	       for(ndx(3)=0; ndx(3)<twoDPBShape(3); ndx(3)++)
		  localTwoDPB.putAt(Complex((localPB(ndx)),0.0),ndx);
	}
	//
	// If antenna pointing errors are not applied, no shifting
	// (which can be expensive) is required.
	//
	//
	// Accumulate the shifted PBs
	//
	{
	  Bool isRefF;
	  Array<Float> fbuf;
	  Array<Complex> cbuf;
	  isRefF=theavgPB.get(fbuf);
	  //isRefC=
	  localTwoDPB.get(cbuf);
	  
	  IPosition fs(fbuf.shape());
	  {
	    IPosition ndx(4,0,0,0,0),avgNDX(4,0,0,0,0);
	    for(ndx(3)=0,avgNDX(3)=0;ndx(3)<fs(3);ndx(3)++,avgNDX(3)++)
	    {
	    for(ndx(2)=0,avgNDX(2)=0;ndx(2)<twoDPBShape(2);ndx(2)++,avgNDX(2)++)
	      {
		for(ndx(0)=0,avgNDX(0)=0;ndx(0)<fs(0);ndx(0)++,avgNDX(0)++)
		  for(ndx(1)=0,avgNDX(1)=0;ndx(1)<fs(1);ndx(1)++,avgNDX(1)++)
		    {
		      Float val;
		      val = real(cbuf(ndx));
		      fbuf(avgNDX) += val;
		      if (fbuf(avgNDX) > peak) peak=fbuf(avgNDX);
		    }
	      }
	    }
	  }
	  if (!isRefF) theavgPB.put(fbuf);
	  pbPeaks += peak;
	}
      }
    theavgPB.setCoordinateInfo(localPB.coordinates());
    return true; // i.e., an average PB was made and is in the mem. cache
  }
  //
  //---------------------------------------------------------------
  //
  //
  //---------------------------------------------------------------
  //
  // Locate a convlution function in either mem. or disk cache.  
  // Return 1 if found in the disk cache.
  //        2 if found in the mem. cache.
  //       <0 if not found in either cache.  In this case, absolute of
  //          the return value corresponds to the index in the list of
  //          conv. funcs. where this conv. func. should be filled
  //
  Int nPBWProjectFT::locateConvFunction(Int Nw, Int /*polInUse*/,  
                                        const VisBuffer& vb, Float &pa)
  {
    Int i; Bool found;
    // Commented out to prevent compiler warning
    // Float dPA = paChangeDetector.getParAngleTolerance().getValue("rad");
    found = cfCache.searchConvFunction(vb,paChangeDetector,i,pa);
    if (found)
      {
	Vector<Float> sampling;
	PAIndex=i;
	//	CoordinateSystem csys;
	if (cfCache.loadConvFunction(i,Nw,convFuncCache,convSupport,sampling,
				     cfRefFreq_p,convFuncCS_p))
	  {
	    convSampling = (Int)sampling[0];
	    convFunc.reference(*convFuncCache[PAIndex]);
	    cfCache.loadConvFunction(i,Nw,convWeightsCache,convSupport,sampling,cfRefFreq_p,
				     convFuncCS_p, "/CFWT");
	    convWeights.reference(*convWeightsCache[PAIndex]);
	    if (PAIndex < (Int)convFuncCache.nelements())
	      logIO() << "Loaded from disk cache: Conv. func. # "
		      << PAIndex << LogIO::POST;
	    return 1;
	  }
	convFunc.reference(*convFuncCache[PAIndex]);
	convWeights.reference(*convWeightsCache[PAIndex]);
	return 2;
      }
    return i;
  }
  void nPBWProjectFT::makeCFPolMap(const VisBuffer& vb, Vector<Int>& polM)
  {
    Vector<Int> msStokes = vb.corrType();
    Int nPol = msStokes.nelements();
    polM.resize(polMap.shape());
    polM = -1;

    for(Int i=0;i<nPol;i++)
      for(uInt j=0;j<cfStokes.nelements();j++)
	if (cfStokes(j) == msStokes(i))
	    {polM(i) = j;break;}
  }
  //
  //---------------------------------------------------------------
  //
  // Given a polMap (mapping of which Visibility polarization is
  // gridded onto which grid plane), make a map of the conjugate
  // planes of the grid E.g, for Stokes-I and -V imaging, the two
  // planes of the uv-grid are [LL,RR].  For input VisBuffer
  // visibilites in order [RR,RL,LR,LL], polMap = [1,-1,-1,0].  The
  // conjugate map will be [0,-1,-1,1].
  //
  void nPBWProjectFT::makeConjPolMap(const VisBuffer& vb, 
				     const Vector<Int> cfPolMap, 
				     Vector<Int>& conjPolMap)
  {
    //
    // All the Natak (Drama) below with slicers etc. is to extract the
    // Poln. info. for the first IF only (not much "information
    // hiding" for the code to slice arrays in a general fashion).
    //
    // Extract the shape of the array to be sliced.
    //
    Array<Int> stokesForAllIFs = vb.msColumns().polarization().corrType().getColumn();
    IPosition stokesShape(stokesForAllIFs.shape());
    IPosition firstIFStart(stokesShape),firstIFLength(stokesShape);
    //
    // Set up the start and length IPositions to extract only the
    // first column of the array.  The following is required since the
    // array could have only one column as well.
    //
    firstIFStart(0)=0;firstIFLength(0)=stokesShape(0);
    for(uInt i=1;i<stokesShape.nelements();i++) {firstIFStart(i)=0;firstIFLength(i)=1;}
    //
    // Construct the slicer and produce the slice.  .nonDegenerate
    // required to ensure the result of slice is a pure vector.
    //
    Vector<Int> visStokes = stokesForAllIFs(Slicer(firstIFStart,firstIFLength)).nonDegenerate();

    conjPolMap = cfPolMap;
    
    Int i,j,N = cfPolMap.nelements();
    for(i=0;i<N;i++)
      if (cfPolMap[i] > -1)
	{
	if      (visStokes[i] == Stokes::RR) 
	  {
	    conjPolMap[i]=-1;
	    for(j=0;j<N;j++) if (visStokes[j] == Stokes::LL) break; 
	    conjPolMap[i]=cfPolMap[j];
	  }
	else if (visStokes[i] == Stokes::LL) 
	  {
	    conjPolMap[i]=-1;
	    for(j=0;j<N;j++) if (visStokes[j] == Stokes::RR) break; 
	    conjPolMap[i]=cfPolMap[j];
	  }
	else if (visStokes[i] == Stokes::LR) 
	  {
	    conjPolMap[i]=-1;
	    for(j=0;j<N;j++) if (visStokes[j] == Stokes::RL) break; 
	    conjPolMap[i]=cfPolMap[j];
	  }
	else if (visStokes[i] == Stokes::RL) 
	  {
	    conjPolMap[i]=-1;
	    for(j=0;j<N;j++) if (visStokes[j] == Stokes::LR) break; 
	    conjPolMap[i]=cfPolMap[j];
	  }
	}
  }
  //
  //---------------------------------------------------------------
  //
  void nPBWProjectFT::findConvFunction(const ImageInterface<Complex>& image,
				       const VisBuffer& vb)
  {
    if (!paChangeDetector.changed(vb,0)) return;

    logIO() << LogOrigin("nPBWProjectFT", "findConvFunction")  << LogIO::NORMAL;
    
    ok();
    
    
    CoordinateSystem coords(image.coordinates());
    
    //
    // Make a two dimensional image to calculate auto-correlation of
    // the ideal illumination pattern. We want this on a fine grid in
    // the UV plane
    //
    Int directionIndex=coords.findCoordinate(Coordinate::DIRECTION);
    AlwaysAssert(directionIndex>=0, AipsError);
    DirectionCoordinate dc=coords.directionCoordinate(directionIndex);
    directionCoord=coords.directionCoordinate(directionIndex);
    Vector<Double> sampling;
    sampling = dc.increment();
    sampling*=Double(convSampling);
    sampling*=Double(nx)/Double(convSize);
    dc.setIncrement(sampling);
    
    
    Vector<Double> unitVec(2);
    unitVec=convSize/2;
    dc.setReferencePixel(unitVec);
    
    // Set the reference value to that of the image
    coords.replaceCoordinate(dc, directionIndex);
    
    //
    // Make an image with circular polarization axis.  Return the
    // no. of vis. poln. planes that will be used in making the user
    // defined Stokes image.
    //
    polInUse=makePBPolnCoords(coords,vb);
    
    Float pa;
    Int cfSource=locateConvFunction(wConvSize, polInUse, vb, pa);
    // cerr << "CFS: " << cfSource << " " << PAIndex << " " 
    // 	 << convFuncCache.nelements() << " " 
    // 	 << convWeightsCache.nelements() << " " 
    // 	 << endl;
    lastPAUsedForWtImg = currentCFPA = pa;
    
    Bool pbMade=false;
    if (cfSource==1) // CF found and loaded from the disk cache
      {
	//	cout << "### New CFPA = " << currentCFPA << endl;
	polInUse = convFunc.shape()(3);
	wConvSize = convFunc.shape()(2);
	try
	  {
	    cfCache.loadAvgPB(avgPB);
	    avgPBReady=true;
	  }
	catch (AipsError& err)
	  {
	    logIO() << "Average PB does not exist in the cache.  A fresh one will be made."
		    << LogIO::NORMAL << LogIO::POST;
	    pbMade=makeAveragePB0(vb, image, polInUse, avgPB);
	    pbNormalized=false; normalizeAvgPB(); pbNormalized=true;
	  }
      }
    else if (cfSource==2)  // CF found in the mem. cache
      {
      }
    else                     // CF not found in either cache
      {
	//
	// Make the CF, update the average PB and update the CF and
	// the avgPB disk cache
	//
	PAIndex = abs(cfSource);
        //
        // Load the average PB from the disk since it's going to be
        // updated in memory and on the disk.  Without loading it from
        // the disk (from a potentially more complete existing cache),
        // the average PB can get inconsistant with the rest of the
        // cache.
        //
// 	logIO() << LogOrigin("nPBWProjectFT::findConvFunction()","") 
// 		<< "Making the convolution function for PA=" << pa << "deg."
// 		<< LogIO::NORMAL 
// 		<< LogIO::POST;
	makeConvFunction(image,vb,pa);
	try
	  {
	    cfCache.loadAvgPB(avgPB);
	    resetPBs = false;
	    avgPBReady=true;
	  }
	catch(SynthesisFTMachineError &err)
	  {
	    logIO() << LogOrigin("nPBWProjectFT::findConvFunction()","") 
		    << "Average PB does not exist in the cache.  A fresh one will be made." 
		    << LogIO::NORMAL 
		    << LogIO::POST;
	    pbMade=makeAveragePB0(vb, image, polInUse,avgPB);
	  }

	//	makeAveragePB(vb, image, polInUse,avgPB);
	pbNormalized=false; 
	normalizeAvgPB();
	pbNormalized=true;
	Int index=coords.findCoordinate(Coordinate::SPECTRAL);
	SpectralCoordinate spCS = coords.spectralCoordinate(index);
	Vector<Double> refValue; refValue.resize(1);refValue(0)=cfRefFreq_p;
	spCS.setReferenceValue(refValue);
	coords.replaceCoordinate(spCS,index);
	cfCache.cacheConvFunction(PAIndex, pa, convFunc, coords, convFuncCS_p, 
				  convSize, convSupport,convSampling);
	Cube<Int> convWtSize=convSupport*CONVWTSIZEFACTOR;
	cfCache.cacheConvFunction(PAIndex, pa, convWeights, coords, convFuncCS_p,
				  convSize, convWtSize,convSampling,"WT",false);
	cfCache.finalize(); // Write the aux info file
	if (pbMade) cfCache.finalize(avgPB); // Save the AVG PB and write the aux info.
      }

    verifyShapes(avgPB.shape(), image.shape());

    Int lastPASlot = PAIndex;

    if (paChangeDetector.changed(vb,0)) paChangeDetector.update(vb,0);
    //
    // If mem. cache not yet ready and the latest CF was loaded from
    // the disk cache, compute and give some user useful info.
    //
    if ((!convFuncCacheReady) && (cfSource != 2))
      {
	//
	// Compute the aggregate memory used by the cached convolution
	// functions.
	//
	Int maxMemoryMB=HostInfo::memoryTotal(true)/1024;
	Float memoryKB=0;
	String unit(" KB");
	for(uInt iPA=0;iPA<convFuncCache.nelements();iPA++)
	  {
	    Int volume=1;
	    if (convFuncCache[iPA] != NULL)
	      {
		IPosition shape=(*convFuncCache[iPA]).shape();
		volume=1;
		for(uInt i=0;i<shape.nelements();i++)
		  volume*=shape(i);
		memoryKB += volume;
	      }
	  }

	
	memoryKB = Int(memoryKB*sizeof(Complex)*2/1024.0+0.5);
	if (memoryKB > 1024) {memoryKB /=1024; unit=" MB";}
	
	logIO() << "Memory used in gridding functions = "
		<< (Int)(memoryKB+0.5) << unit << " out of a maximum of "
		<< maxMemoryMB << " MB" << LogIO::POST;
	
	//
	// Show the list of support sizes along the w-axis for the current PA.
	//
	IPosition sliceStart(3,0,0,lastPASlot),
	  sliceLength(3,wConvSize,1,1);
	logIO() << "Convolution support [CF#= " << lastPASlot 
		<< ", PA=" << pa*180.0/M_PI << "deg"
		<<"] = "
		<< convSupport(Slicer(sliceStart,sliceLength)).nonDegenerate()
		<< " pixels in Fourier plane"
		<< LogIO::POST;
      }

    IPosition shp(convFunc.shape());
    IPosition ndx(shp);
    ndx =0;
    Area.resize(Area.nelements()+1,true);
    Area(lastPASlot)=0;

    for(ndx(0)=0;ndx(0)<shp(0);ndx(0)++)
      for(ndx(1)=0;ndx(1)<shp(1);ndx(1)++)
	Area(lastPASlot)+=convFunc(ndx);
  }
  //
  //---------------------------------------------------------------
  //
  void nPBWProjectFT::makeConvFunction(const ImageInterface<Complex>& image,
				       const VisBuffer& vb,Float pa)
  {
    if (bandID_p == -1) bandID_p=getVisParams(vb);
    Int NNN=0;
    logIO() << LogOrigin("nPBWProjectFT", "makeConvFunction") 
	    << "Making a new convolution function for PA="
	    << pa*(180/C::pi) << "deg"
	    << LogIO::NORMAL 
	    << LogIO::POST;

    if(wConvSize>NNN) {
      logIO() << "Using " << wConvSize << " planes for W-projection" << LogIO::POST;
      Double maxUVW;
      maxUVW=0.25/abs(image.coordinates().increment()(0));
      logIO() << "Estimating maximum possible W = " << maxUVW
	      << " (wavelengths)" << LogIO::POST;
      
      Double invLambdaC=vb.frequency()(0)/C::c;
//       logIO() << "Typical wavelength = " << 1.0/invLambdaC
// 	      << " (m)" << LogIO::POST;
      Double invMinL = vb.frequency()((vb.frequency().nelements())-1)/C::c;
      logIO() << "wavelength range = " << 1.0/invLambdaC << " (m) to " 
	      << 1.0/invMinL << " (m)" << LogIO::POST;
      if (wConvSize > 1)
	{
	  uvScale(2)=Float((wConvSize-1)*(wConvSize-1))/maxUVW;
	  logIO() << "Scaling in W (at maximum W) = " << 1.0/uvScale(2)
		  << " wavelengths per pixel" << LogIO::POST;
	}
    }
    //  
    // Get the coordinate system
    //
    CoordinateSystem coords(image.coordinates());
    
    //
    // Set up the convolution function. 
    //
    if(wConvSize>NNN) 
      {
	if(wConvSize>256) 
	  {
	    convSampling=4;
	    convSize=min(nx,512);
	  }
	else 
	  {
	    convSampling=4;
	    convSize=min(nx,2048);
	  }
      }
    else 
      {
	convSampling=4;
	convSize=nx;
      }
    convSampling=OVERSAMPLING;
    convSize=CONVSIZE;
    //
    // Make a two dimensional image to calculate auto-correlation of
    // the ideal illumination pattern. We want this on a fine grid in
    // the UV plane
    //
    Int directionIndex=coords.findCoordinate(Coordinate::DIRECTION);
    AlwaysAssert(directionIndex>=0, AipsError);
    DirectionCoordinate dc=coords.directionCoordinate(directionIndex);
    directionCoord=coords.directionCoordinate(directionIndex);
    Vector<Double> sampling;
    sampling = dc.increment();
    //    sampling /= Double(2.0);

    sampling*=Double(convSampling);
    sampling*=Double(nx)/Double(convSize);

    // cerr << "Sampling on the sky = " << dc.increment() << " " << nx << "x" << ny << endl;
    // cerr << "Sampling on the PB  = " << sampling << " " << convSize << "x" << convSize << endl;
    dc.setIncrement(sampling);
    
    Vector<Double> unitVec(2);
    unitVec=convSize/2;
    dc.setReferencePixel(unitVec);
    
    // Set the reference value to that of the image
    coords.replaceCoordinate(dc, directionIndex);
    
    //
    // Make an image with circular polarization axis.  Return the
    // no. of vis. poln. planes that will be used in making the user
    // defined Stokes image.
    //
    polInUse=makePBPolnCoords(coords,vb);
    convSupport.resize(wConvSize,polInUse,PAIndex+1,true);
    Int N=convFuncCache.nelements();
    convFuncCache.resize(PAIndex+1,true);
    for(Int i=N;i<PAIndex;i++) convFuncCache[i]=NULL;
    convWeightsCache.resize(PAIndex+1,true);
    for(Int i=N;i<PAIndex;i++) convWeightsCache[i]=NULL;
    //------------------------------------------------------------------
    //
    // Make the sky Stokes PB.  This will be used in the gridding
    // correction
    //
    //------------------------------------------------------------------
    IPosition pbShape(4, convSize, convSize, polInUse, 1);
    TempImage<Complex> twoDPB(pbShape, coords);

    IPosition pbSqShp(pbShape);
    //    pbSqShp[0] *= 2;    pbSqShp[1] *= 2;

    unitVec=pbSqShp[0]/2;
    dc.setReferencePixel(unitVec);
    // sampling *= Double(2.0);
    // dc.setIncrement(sampling);
    coords.replaceCoordinate(dc, directionIndex);

    TempImage<Complex> twoDPBSq(pbSqShp,coords);
    twoDPB.setMaximumCacheSize(cachesize);
    twoDPB.set(Complex(1.0,0.0));
    twoDPBSq.setMaximumCacheSize(cachesize);
    twoDPBSq.set(Complex(1.0,0.0));
    //
    // Accumulate the various terms that constitute the gridding
    // convolution function.
    //
    //------------------------------------------------------------------
    
    Int inner=convSize/convSampling;
    //    inner = convSize/2;

    Vector<Double> cfUVScale(3,0),cfUVOffset(3,0);

    cfUVScale(0)=Float(twoDPB.shape()(0))*sampling(0);
    cfUVScale(1)=Float(twoDPB.shape()(1))*sampling(1);
    cfUVOffset(0)=Float(twoDPB.shape()(0))/2;
    cfUVOffset(1)=Float(twoDPB.shape()(1))/2;
    // cerr << uvScale << " " << cfUVScale << endl;
    // cerr << uvOffset << " " << cfUVOffset << endl;
    ConvolveGridder<Double, Complex>
      //      ggridder(IPosition(2, inner, inner), cfUVScale, cfUVOffset, "SF");
      ggridder(IPosition(2, inner, inner), uvScale, uvOffset, "SF");
    
    // convFuncCache[PAIndex] = new Array<Complex>(IPosition(4,convSize/2,convSize/2,
    // 							  wConvSize,polInUse));
    // convWeightsCache[PAIndex] = new Array<Complex>(IPosition(4,convSize/2,convSize/2,
    // 							     wConvSize,polInUse));
    convFuncCache[PAIndex] = new Array<Complex>(IPosition(4,convSize,convSize,
							  wConvSize,polInUse));
    convWeightsCache[PAIndex] = new Array<Complex>(IPosition(4,convSize,convSize,
    							     wConvSize,polInUse));
    convFunc.reference(*convFuncCache[PAIndex]);
    convWeights.reference(*convWeightsCache[PAIndex]);
    convFunc=0.0;
    convWeights=0.0;
    
    IPosition start(4, 0, 0, 0, 0);
    IPosition pbSlice(4, convSize, convSize, 1, 1);
    
    Matrix<Complex> screen(convSize, convSize);
    if (paChangeDetector.changed(vb,0)) paChangeDetector.update(vb,0);
    VLACalcIlluminationConvFunc vlaPB;
    vlaPB.setMaximumCacheSize(cachesize);

    for (Int iw=0;iw<wConvSize;iw++) 
      {
	screen = 1.0;

	/*
	screen=0.0;
	// First the spheroidal function
	//
	//      inner=convSize/2;
	//	screen = 0.0;
	Vector<Complex> correction(inner);
	for (Int iy=-inner/2;iy<inner/2;iy++) 
	  {
	    ggridder.correctX1D(correction, iy+inner/2);
	    for (Int ix=-inner/2;ix<inner/2;ix++) 
	      screen(ix+convSize/2,iy+convSize/2)=correction(ix+inner/2);
	    // if (iy==0)
	    //   for (Int ii=0;ii<inner;ii++)
	    // 	cout << ii << " " << correction(ii) << endl;
	  }
	*/
	//
	// Now the w term
	//
	if(wConvSize>1) 
	  {
	    logIO() << LogOrigin("nPBWProjectFT", "")
		    << "Computing WPlane " << iw  << LogIO::POST;
	    
	    Double twoPiW=2.0*C::pi*Double(iw*iw)/uvScale(2);
	    
	    for (Int iy=-inner/2;iy<inner/2;iy++) 
	      {
		Double m=sampling(1)*Double(iy);
		Double msq=m*m;
		for (Int ix=-inner/2;ix<inner/2;ix++) 
		  {
		    Double l=sampling(0)*Double(ix);
		    Double rsq=l*l+msq;
		    if(rsq<1.0) 
		      {
			Double phase=twoPiW*(sqrt(1.0-rsq)-1.0);
			screen(ix+convSize/2,iy+convSize/2)*=Complex(cos(phase),sin(phase));
		      }
		  }
	      }
	  }
	
	//
	// Fill the complex image with the w-term...
	//
	IPosition PolnPlane(4,0,0,0,0);
	IPosition ndx(4,0,0,0,0);
	
	for(Int i=0;i<polInUse;i++)
	  {
	    PolnPlane(2)=i;
	    twoDPB.putSlice(screen, PolnPlane);
	    twoDPBSq.putSlice(screen, PolnPlane);
	  }
	// {
	//   Vector<IPosition> posMax(twoDPB.shape()(2));
	//   posMax(0)(0)=pbShape(0)/2;
	//   posMax(0)(1)=pbShape(1)/2;
	//   posMax(1)(0)=pbShape(0)/2;
	//   posMax(1)(1)=pbShape(1)/2;
	//   getVisParams(vb);
	//   applyAntiAliasingOp(twoDPB,posMax,0);
	// }
	//
	// Apply the PB...
	//

	// The following replaces the simple vlaPB.applyPB() call.
	// The functional interface of VLACalcIllumination was
	// modified to apply one polarization PB at a time.  As a
	// result, the co-ord. sys. of the image going to applyPB()
	// should have only one Poln. plane.
	//
	// Before this change, the twoDPB and twoDPBSq images were
	// directly sent to applyPB(). Now, in the code below, we go
	// over the poln. planes of these images, make a temp. image
	// with the poln. planes of these images, applyPB() on the
	// temp. image, and copy the result in the appropriate planes
	// of the twoD* images.  The rest of the code therefore does
	// not see a difference.
	{
	  CoordinateSystem singlePolCoords=coords;
	  Int index, outNdx, whichPolPlane=0;
	  Vector<Int> inStokes, outStokes(1);
	  index = coords.findCoordinate(Coordinate::STOKES);
	  inStokes = coords.stokesCoordinate(index).stokes();
	  for (uInt i=0; i<inStokes.nelements(); i++)
	    {
	      outStokes(0)=inStokes(i);
	      // Make a temp. image with a single Stokes along Pol. axis.
	      StokesCoordinate polnCoord(outStokes);
	      outNdx = singlePolCoords.findCoordinate(Coordinate::STOKES);
	      singlePolCoords.replaceCoordinate(polnCoord, outNdx);
	      IPosition singlePolShape = pbSqShp;
	      singlePolShape(2)=1;
	      Bool doSquint=true;
	      Double pa=getPA(vb);
	      Vector<Double> chanFreq = vb.frequency();
	      //freqHi = max(chanFreq);
	      Double Freq = max(chanFreq);
	      {
		TempImage<Complex> singlePolImg(singlePolShape, singlePolCoords);
		// Copy screen to the single pol. image.  Apply PB to it.  Copy 
		// the single pol. image plane to the twoDPB image.
		doSquint=true;
		PolnPlane(2)=0;               singlePolImg.putSlice(screen, PolnPlane);
		vlaPB.applyPB(singlePolImg, pa, doSquint,bandID_p, 0, Freq);
		PolnPlane(2)=whichPolPlane;   twoDPB.putSlice(singlePolImg.get(), PolnPlane);
	      }
	      {
		TempImage<Complex> singlePolImg(singlePolShape, singlePolCoords);
		// Copy screen to the single pol. image.  Apply PB to it.  Copy 
		// the single pol. image plane to the twoDPBSq image.
		doSquint = false;
		PolnPlane(2)=0;               singlePolImg.putSlice(screen, PolnPlane);
		//vlaPB.applyPBSq(twoDPBSq, vb, bandID_p, doSquint);
		vlaPB.applyPB(singlePolImg, pa, doSquint, bandID_p, 0, Freq);
		PolnPlane(2)=whichPolPlane;   twoDPBSq.putSlice(singlePolImg.get(), PolnPlane);
	      }

	      whichPolPlane++;
	    }
	}
	/*
// 	twoDPB.put(abs(twoDPB.get()));
// 	twoDPBSq.put(abs(twoDPBSq.get()));
        */

	// {
	//   String name("twoDPB.before.im");
	//   storeImg(name,twoDPB);
	// }
	// {
	//   //
	//   // Apply (multiply) by the Spheroidal functions
	//   //
	//   Vector<Float> maxVal(twoDPB.shape()(2));
	//   Vector<IPosition> posMax(twoDPB.shape()(2));
	  
	//   SynthesisUtils::findLatticeMax(twoDPB,maxVal,posMax); 
	//   posMax(0)(0)+=1;
	//   posMax(0)(1)+=1;
	//   posMax(1)(0)+=1;
	//   posMax(1)(1)+=1;
	//   //	  applyAntiAliasingOp(twoDPB,posMax,1);

	//   SynthesisUtils::findLatticeMax(twoDPBSq,maxVal,posMax); 
	//   posMax(0)(0)+=1;
	//   posMax(0)(1)+=1;
	//   posMax(1)(0)+=1;
	//   posMax(1)(1)+=1;
	//   //	  applyAntiAliasingOp(twoDPBSq,posMax,1,true);
	// }

	Complex cpeak=max(twoDPB.get());
	twoDPB.put(twoDPB.get()/cpeak);
	cpeak=max(twoDPBSq.get());
	twoDPBSq.put(twoDPBSq.get()/cpeak);
	//	twoDPBSq.set(1.0);
	// {
	//   String name("twoDPB.im");
	//   storeImg(name,twoDPB);
	// }

	CoordinateSystem cs=twoDPB.coordinates();
	Int index= twoDPB.coordinates().findCoordinate(Coordinate::SPECTRAL);
	SpectralCoordinate SpCS = twoDPB.coordinates().spectralCoordinate(index);

	cfRefFreq_p=SpCS.referenceValue()(0);
	//
	// Now FFT and get the result back
	//
	// {
	//   String name("twoDPB.im");
	//   storeImg(name,twoDPB);
	// }
	LatticeFFT::cfft2d(twoDPB);
	LatticeFFT::cfft2d(twoDPBSq);
	// {
	//   String name("twoDPBFT.im");
	//   storeImg(name,twoDPB);
	// }
	//
	// Fill the convolution function planes with the result.
	//
	{
	  // IPosition start(4, convSize/4, convSize/4, 0, 0),
	  //   pbSlice(4, convSize/2-1, convSize/2-1, polInUse, 1);
	  // IPosition sliceStart(4,0,0,iw,0), 
	  //   sliceLength(4,convSize/2-1,convSize/2-1,1,polInUse);
	  
	  IPosition start(4, 0, 0, 0, 0),
	    pbSlice(4, twoDPB.shape()[0]-1, twoDPB.shape()[1]-1, polInUse, 1);
	  IPosition sliceStart(4,0,0,iw,0), 
	    sliceLength(4,convFunc.shape()[0]-1,convFunc.shape()[1]-1,1,polInUse);
	  
	  convFunc(Slicer(sliceStart,sliceLength)).nonDegenerate()
	    =(twoDPB.getSlice(start, pbSlice, true));

	  IPosition shp(twoDPBSq.shape());
	  
          //UNUSED: Int bufSize=convWeights.shape()[0], Org=shp[0]/2;
	  // IPosition sqStart(4, Org-bufSize/2, Org-bufSize/2, 0, 0),
	  //   pbSqSlice(4, bufSize-1, bufSize-1, polInUse, 1);
	  // IPosition sqSliceStart(4,0,0,iw,0), 
	  //   sqSliceLength(4,bufSize-1,bufSize-1,1,polInUse);

	  IPosition sqStart(4, 0, 0, 0, 0),
	    pbSqSlice(4, shp[0]-1, shp[1]-1, polInUse, 1);
	  IPosition sqSliceStart(4,0,0,iw,0), 
	    sqSliceLength(4,shp[0]-1,shp[1]-1,1,polInUse);
	  
	  convWeights(Slicer(sqSliceStart,sqSliceLength)).nonDegenerate()
	    =(twoDPBSq.getSlice(sqStart, pbSqSlice, true));

	}
      }
    
    {
      Complex cpeak = max(convFunc);
      convFunc/=cpeak;
      //      cout << "#### max(convFunc) = " << cpeak << endl;
      cpeak=max(convWeights);
      //      cout << "#### max(convWeights) = " << cpeak << endl;
      convWeights/=cpeak;
      //      cout << "#### max(convWeights) = " << max(convWeights) << endl;
    }
    //
    // Find the convolution function support size.  No assumption
    // about the symmetry of the conv. func. can be made (except that
    // they are same for all poln. planes).
    //
    Int lastPASlot=PAIndex;
    for(Int iw=0;iw<wConvSize;iw++)
      for(Int ipol=0;ipol<polInUse;ipol++)
	convSupport(iw,ipol,lastPASlot)=-1;
    //
    // !!!Caution: This assumes that the support size at each Poln. is
    // the same, starting from the center pixel (in pixel
    // co-ordinates).  For large pointing offsets, this might not be
    // true.
    //
    //    Int ConvFuncOrigin=convSize/4;  // Conv. Func. is half that size of convSize
    Int ConvFuncOrigin=convFunc.shape()[0]/2;  // Conv. Func. is half that size of convSize
    IPosition ndx(4,ConvFuncOrigin,0,0,0);
    //    Cube<Int> convWtSupport(convSupport.shape());
    convWtSupport.resize(convSupport.shape(),true);
    Int maxConvWtSupport=0, supportBuffer;
    for (Int iw=0;iw<wConvSize;iw++)
      {
	Bool found=false;
	Float threshold;
	Int R;
	ndx(2) = iw;
	
	ndx(0)=ndx(1)=ConvFuncOrigin;
	ndx(2) = iw;
	//	Complex maxVal = max(convFunc);
	threshold = abs(convFunc(ndx))*THRESHOLD;
	//
	// Find the support size of the conv. function in pixels
	//
	Int wtR;
	found =findSupport(convWeights,threshold,ConvFuncOrigin,wtR);
	found = findSupport(convFunc,threshold,ConvFuncOrigin,R);
	
	//	R *=2.5;
	//
	// Set the support size for each W-plane and for all
	// Pol-planes.  Assuming that support size for all Pol-planes
	// is same.
	//
	if(found) 
	  {
	    //	    Int maxR=R;//max(ndx(0),ndx(1));
	    for(Int ipol=0;ipol<polInUse;ipol++)
	      {
		convSupport(iw,ipol,lastPASlot)=Int(R/Float(convSampling));
		convSupport(iw,ipol,lastPASlot)=Int(0.5+Float(R)/Float(convSampling))+1;
		//		convSupport(iw,ipol,lastPASlot) += (convSupport(iw,ipol,lastPASlot)+1)%2;
		convWtSupport(iw,ipol,lastPASlot)=Int(R*CONVWTSIZEFACTOR/Float(convSampling));
		convWtSupport(iw,ipol,lastPASlot)=Int(0.5+Float(R)*CONVWTSIZEFACTOR/Float(convSampling))+1;
		//		convWtSupport(iw,ipol,lastPASlot)=Int(wtR/Float(convSampling));
		//		convWtSupport(iw,ipol,lastPASlot) += (convWtSupport(iw,ipol,lastPASlot)+1)%2;
		if ((lastPASlot == 0) || (maxConvSupport == -1))
		  if (convSupport(iw,ipol,lastPASlot) > maxConvSupport)
		    maxConvSupport = convSupport(iw,ipol,lastPASlot);
		maxConvWtSupport=convWtSupport(iw,ipol,lastPASlot);
	      }
	  }
      }

    if(convSupport(0,0,lastPASlot)<1) 
      logIO() << "Convolution function is misbehaved - support seems to be zero"
	      << LogIO::EXCEPTION;
    
    logIO() << LogOrigin("nPBWProjectFT", "makeConvFunction")
	    << "Re-sizing the convolution functions"
	    << LogIO::POST;
    
    {
      supportBuffer = OVERSAMPLING;
      Int bot=ConvFuncOrigin-convSampling*maxConvSupport-supportBuffer,//-convSampling/2, 
      top=ConvFuncOrigin+convSampling*maxConvSupport+supportBuffer;//+convSampling/2;
      bot = max(0,bot);
      top = min(top, convFunc.shape()(0)-1);
      {
	Array<Complex> tmp;
	IPosition blc(4,bot,bot,0,0), trc(4,top,top,wConvSize-1,polInUse-1);
      
	tmp = convFunc(blc,trc);
	(*convFuncCache[lastPASlot]).resize(tmp.shape());
	(*convFuncCache[lastPASlot]) = tmp; 
	convFunc.reference(*convFuncCache[lastPASlot]);
      }
      
      supportBuffer = (Int)(OVERSAMPLING*CONVWTSIZEFACTOR);
      bot=ConvFuncOrigin-convSampling*maxConvWtSupport-supportBuffer;
      top=ConvFuncOrigin+convSampling*maxConvWtSupport+supportBuffer;
      bot=max(0,bot);
      top=min(top,convWeights.shape()(0)-1);
      {
	Array<Complex> tmp;
	IPosition blc(4,bot,bot,0,0), trc(4,top,top,wConvSize-1,polInUse-1);

	tmp = convWeights(blc,trc);
	(*convWeightsCache[lastPASlot]).resize(tmp.shape());
	(*convWeightsCache[lastPASlot]) = tmp; 
	convWeights.reference(*convWeightsCache[lastPASlot]);
      }
    }    
    
    //
    // Normalize such that plane 0 sums to 1 (when jumping in steps of
    // convSampling).  This is really not necessary here since we do
    // the normalizing by the area more accurately in the gridder
    // (fpbwproj.f).
    //
    ndx(2)=ndx(3)=0;
    
    
    Complex pbSum=0.0;
    IPosition peakPix(ndx);
    
    Int Nx = convFunc.shape()(0), Ny=convFunc.shape()(1);
	      
    for(Int nw=0;nw<wConvSize;nw++)
      for(Int np=0;np<polInUse;np++)
	{
	  ndx(2) = nw; ndx(3)=np;
	  {
	    //
	    // Locate the pixel with the peak value.  That's the
	    // origin in pixel co-ordinates.
	    //
	    Float peak=0;
	    peakPix = 0;
	    for(ndx(1)=0;ndx(1)<convFunc.shape()(1);ndx(1)++)
	      for(ndx(0)=0;ndx(0)<convFunc.shape()(0);ndx(0)++)
		if (abs(convFunc(ndx)) > peak) {peakPix = ndx;peak=abs(convFunc(ndx));}
	  }
	   
	  ConvFuncOrigin = peakPix(0);
	 //	  ConvFuncOrigin = convFunc.shape()(0)/2+1;
	  //	  Int thisConvSupport=convSampling*convSupport(nw,np,lastPASlot);
	  Int thisConvSupport=convSupport(nw,np,lastPASlot);
	  pbSum=0.0;

	  for(Int iy=-thisConvSupport;iy<thisConvSupport;iy++)
	    for(Int ix=-thisConvSupport;ix<thisConvSupport;ix++)
	      {
		ndx(0)=ix*convSampling+ConvFuncOrigin;
		ndx(1)=iy*convSampling+ConvFuncOrigin;
		pbSum += real(convFunc(ndx));
	      }
	  /*
	  for(Int iy=0;iy<Ny;iy++)
	    for(Int ix=0;ix<Nx;ix++)
	      {
		ndx(0)=ix;ndx(1)=iy;
		pbSum += convFunc(ndx);
	      }
	  */
	  if(pbSum>0.0)  
	    {
	      //
	      // Normalize each Poln. plane by the area under its convfunc.
	      //
	      Nx = convFunc.shape()(0), Ny = convFunc.shape()(1);
	      for (ndx(1)=0;ndx(1)<Ny;ndx(1)++) 
	      	for (ndx(0)=0;ndx(0)<Nx;ndx(0)++) 
	      	  {
	      	    convFunc(ndx) /= pbSum;
	      	  }

	      Nx = convWeights.shape()(0); Ny = convWeights.shape()(1);
	      for (ndx(1)=0;  ndx(1)<Ny;  ndx(1)++) 
		for (ndx(0)=0;  ndx(0)<Nx;  ndx(0)++) 
		  {
		    convWeights(ndx) /= pbSum*pbSum;
		    // if ((ndx(0)==Nx/2+1) && (ndx(1)==Ny/2+1))
		    //   {
		    // 	convWeights(ndx)=1.0;	
		    // 	cout << ndx << " " << convWeights(ndx) << endl;
		    //   }
		    // else
		    //   convWeights(ndx)=0.0;
		  }
	    }
	  else 
	    throw(SynthesisFTMachineError("Convolution function integral is not positive"));

	  Vector<Float> maxVal(convWeights.shape()(2));
	  Vector<IPosition> posMax(convWeights.shape()(2));
	  SynthesisUtils::findLatticeMax(convWeights,maxVal,posMax); 
	  //	  cout << "convWeights: " << maxVal << " " << posMax << endl;
	}

  }
  //
  //------------------------------------------------------------------------------
  //
  void nPBWProjectFT::initializeToVis(ImageInterface<Complex>& iimage,
				     const VisBuffer& vb)
  {
    image=&iimage;
    
    ok();
    
    init();
    makingPSF = false;
    initMaps(vb);
    
    findConvFunction(*image, vb);
    //  
    // Initialize the maps for polarization and channel. These maps
    // translate visibility indices into image indices
    //

    nx    = image->shape()(0);
    ny    = image->shape()(1);
    npol  = image->shape()(2);
    nchan = image->shape()(3);
    
    if(image->shape().product()>cachesize) isTiled=true;
    else isTiled=false;
    //
    // If we are memory-based then read the image in and create an
    // ArrayLattice otherwise just use the PagedImage
    //

    isTiled=false;

    if(isTiled){
    	lattice=CountedPtr<Lattice<Complex> > (image, false);
    }
    else 
      {
	IPosition gridShape(4, nx, ny, npol, nchan);
	griddedData.resize(gridShape);
	griddedData=Complex(0.0);
	
	IPosition stride(4, 1);
	IPosition blc(4, (nx-image->shape()(0)+(nx%2==0))/2,
		      (ny-image->shape()(1)+(ny%2==0))/2, 0, 0);
	IPosition trc(blc+image->shape()-stride);
	
	IPosition start(4, 0);
	griddedData(blc, trc) = image->getSlice(start, image->shape());
	
	arrayLattice = new ArrayLattice<Complex>(griddedData);
	lattice=arrayLattice;
      }

    //AlwaysAssert(lattice, AipsError);
    
    logIO() << LogIO::DEBUGGING << "Starting FFT of image" << LogIO::POST;
    
    Vector<Float> sincConv(nx);
    Float centerX=nx/2;
    for (Int ix=0;ix<nx;ix++) 
      {
	Float x=C::pi*Float(ix-centerX)/(Float(nx)*Float(convSampling));
	if(ix==centerX) sincConv(ix)=1.0;
	else            sincConv(ix)=sin(x)/x;
      }
    
    Vector<Complex> correction(nx);
    //
    // Do the Grid-correction
    //
    {
      normalizeAvgPB();
      
      IPosition cursorShape(4, nx, 1, 1, 1);
      IPosition axisPath(4, 0, 1, 2, 3);
      LatticeStepper lsx(lattice->shape(), cursorShape, axisPath);
      LatticeIterator<Complex> lix(*lattice, lsx);
	  
      verifyShapes(avgPB.shape(), image->shape());
      Array<Float> avgBuf; avgPB.get(avgBuf);
      if (max(avgBuf) < 1e-04)
	throw(AipsError("Normalization by PB requested but either PB not found in the cache "
			"or is ill-formed."));

      LatticeStepper lpb(avgPB.shape(),cursorShape,axisPath);
      LatticeIterator<Float> lipb(avgPB, lpb);

      Vector<Complex> griddedVis;
      //
      // Grid correct in anticipation of the convolution by the
      // convFunc.  Each polarization plane is corrected by the
      // appropraite primary beam.
      //
      for(lix.reset(),lipb.reset();!lix.atEnd();lix++,lipb++) 
	{
	  Int iy=lix.position()(1);
	  gridder->correctX1D(correction,iy);
	  griddedVis = lix.rwVectorCursor();
	  
	  Vector<Float> PBCorrection(lipb.rwVectorCursor().shape());
	  PBCorrection = lipb.rwVectorCursor();
	  for(int ix=0;ix<nx;ix++) 
	    {
	      // PBCorrection(ix) = (FUNC(PBCorrection(ix)))/(sincConv(ix)*sincConv(iy));
	      
	      //
	      // This is with PS functions included
	      //
	      // if (doPBCorrection)
	      // 	{
	      // 	  PBCorrection(ix) = FUNC(PBCorrection(ix))/(sincConv(ix)*sincConv(iy));
	      // 	  //PBCorrection(ix) = FUNC(PBCorrection(ix))*(sincConv(ix)*sincConv(iy));
 	      // 	  if ((abs(PBCorrection(ix)*correction(ix))) >= pbLimit_p)
	      // 	    {lix.rwVectorCursor()(ix) /= (PBCorrection(ix))*correction(ix);}
 	      // 	  else
	      // 	    {lix.rwVectorCursor()(ix) *= (sincConv(ix)*sincConv(iy));}
	      // 	}
	      // else 
	      // 	lix.rwVectorCursor()(ix) /= (correction(ix)/(sincConv(ix)*sincConv(iy)));
	      //
	      // This without the PS functions
	      //
	      if (doPBCorrection)
		{
		  // PBCorrection(ix) = FUNC(PBCorrection(ix))/(sincConv(ix)*sincConv(iy));
		  PBCorrection(ix) = FUNC(PBCorrection(ix))*(sincConv(ix)*sincConv(iy));
//		  PBCorrection(ix) = (PBCorrection(ix))*(sincConv(ix)*sincConv(iy));
 		  if ((abs(PBCorrection(ix))) >= pbLimit_p)
		    {lix.rwVectorCursor()(ix) /= (PBCorrection(ix));}
 		  else
		    {lix.rwVectorCursor()(ix) *= (sincConv(ix)*sincConv(iy));}
		}
	      else 
		lix.rwVectorCursor()(ix) /= (1.0/(sincConv(ix)*sincConv(iy)));
	    }
	}
    }
    // {
    //   ostringstream name;
    //   cout << image->shape() << endl;
    //   name << "theModel.im";
    //   PagedImage<Float> tmp(image->shape(), image->coordinates(), name);
    //   Array<Complex> buf;
    //   Bool isRef = lattice->get(buf);
    //   cout << "The model max. = " << max(buf) << endl;
    //   LatticeExpr<Float> le(abs((*lattice)));
    //   tmp.copyData(le);
    // }
    //
    // Now do the FFT2D in place
    //
//     {
//       Array<Complex> buf;
//       Bool isRef = lattice->get(buf);
//     }
    LatticeFFT::cfft2d(*lattice);

    logIO() << LogIO::DEBUGGING << "Finished FFT" << LogIO::POST;
  }
  //
  //---------------------------------------------------------------
  //
  void nPBWProjectFT::initializeToVis(ImageInterface<Complex>& iimage,
				     const VisBuffer& vb,
				     Array<Complex>& griddedVis,
				     Vector<Double>& uvscale)
  {
    initializeToVis(iimage, vb);
    griddedVis.assign(griddedData); //using the copy for storage
    uvscale.assign(uvScale);
  }
  //
  //---------------------------------------------------------------
  //
  void nPBWProjectFT::finalizeToVis()
  {
    logIO() << "##########finalizeToVis()###########" << LogIO::DEBUGGING << LogIO::POST;
    if(isTiled) 
      {
	logIO() << LogOrigin("nPBWProjectFT", "finalizeToVis")  << LogIO::NORMAL;
	
	AlwaysAssert(imageCache, AipsError);
	AlwaysAssert(image, AipsError);
	ostringstream o;
	imageCache->flush();
	imageCache->showCacheStatistics(o);
	logIO() << o.str() << LogIO::POST;
      }
    if(pointingToImage) delete pointingToImage; pointingToImage=0;
  }
  //
  //---------------------------------------------------------------
  //
  // Initialize the FFT to the Sky. Here we have to setup and
  // initialize the grid.
  //
  void nPBWProjectFT::initializeToSky(ImageInterface<Complex>& iimage,
				     Matrix<Float>& weight,
				     const VisBuffer& vb)
  {
    logIO() << "#########initializeToSky()##########" << LogIO::DEBUGGING << LogIO::POST;
    
    // image always points to the image
    image=&iimage;
    
    init();
    initMaps(vb);
    
    // Initialize the maps for polarization and channel. These maps
    // translate visibility indices into image indices
    
    nx    = image->shape()(0);
    ny    = image->shape()(1);
    npol  = image->shape()(2);
    nchan = image->shape()(3);
    
    if(image->shape().product()>cachesize) isTiled=true;
    else                                   isTiled=false;
    
    
    sumWeight=0.0;
    weight.resize(sumWeight.shape());
    weight=0.0;
    //
    // Initialize for in memory or to disk gridding. lattice will
    // point to the appropriate Lattice, either the ArrayLattice for
    // in memory gridding or to the image for to disk gridding.
    //
    if(isTiled) 
      {
	imageCache->flush();
	image->set(Complex(0.0));
	lattice=CountedPtr<Lattice<Complex> > (image, false);
      }
    else 
      {
	IPosition gridShape(4, nx, ny, npol, nchan);
	griddedData.resize(gridShape);
	griddedData=Complex(0.0);
	arrayLattice = new ArrayLattice<Complex>(griddedData);
	lattice=arrayLattice;
      }
    PAIndex = -1;
  }
  //
  //---------------------------------------------------------------
  //
  void nPBWProjectFT::finalizeToSky()
  {
    //
    // Now we flush the cache and report statistics For memory based,
    // we don't write anything out yet.
    //
    logIO() << "#########finalizeToSky()#########" << LogIO::DEBUGGING << LogIO::POST;
    if(isTiled) 
      {
	logIO() << LogOrigin("nPBWProjectFT", "finalizeToSky")  << LogIO::NORMAL;
	
	AlwaysAssert(image, AipsError);
	AlwaysAssert(imageCache, AipsError);
	imageCache->flush();
	ostringstream o;
	imageCache->showCacheStatistics(o);
	logIO() << o.str() << LogIO::POST;
      }
    if(pointingToImage) delete pointingToImage; pointingToImage=0;
    PAIndex = -1;

    paChangeDetector.reset();
    cfCache.finalize();
    convFuncCacheReady=true;
  }
  //
  //---------------------------------------------------------------
  //
  Array<Complex>* nPBWProjectFT::getDataPointer(const IPosition& centerLoc2D,
					       Bool readonly) 
  {
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
#define gpbwproj gpbwproj_
#define dpbwproj dpbwproj_
#define dpbwgrad dpbwgrad_
#endif
  
  extern "C" { 
    void gpbwproj(Double *uvw,
		  Double *dphase,
		  const Complex *values,
		  Int *nvispol,
		  Int *nvischan,
		  Int *dopsf,
		  const Int *flag,
		  const Int *rflag,
		  const Float *weight,
		  Int *nrow,
		  Int *rownum,
		  Double *scale,
		  Double *offset,
		  Complex *grid,
		  Int *nx,
		  Int *ny,
		  Int *npol,
		  Int *nchan,
		  const Double *freq,
		  const Double *c,
		  Int *support,
		  Int *convsize,
		  Int *sampling,
		  Int *wconvsize,
		  Complex *convfunc,
		  Int *chanmap,
		  Int *polmap,
		  Int *polused,
		  Double *sumwt,
		  Int *ant1,
		  Int *ant2,
		  Int *nant,
		  Int *scanno,
		  Double *sigma,
		  Float *raoff,
		  Float *decoff,
		  Double *area,
		  Int *doGrad,
		  Int *doPointingCorrection,
		  Int *nPA,
		  Int *paIndex,
		  Int *CFMap,
		  Int *ConjCFMap,
		  Double *currentCFPA, Double *actualPA,Double *cfRefFreq_p);
    void dpbwproj(Double *uvw,
		  Double *dphase,
		  Complex *values,
		  Int *nvispol,
		  Int *nvischan,
		  const Int *flag,
		  const Int *rflag,
		  Int *nrow,
		  Int *rownum,
		  Double *scale,
		  Double *offset,
		  const Complex *grid,
		  Int *nx,
		  Int *ny,
		  Int *npol,
		  Int *nchan,
		  const Double *freq,
		  const Double *c,
		  Int *support,
		  Int *convsize,
		  Int *sampling,
		  Int *wconvsize,
		  Complex *convfunc,
		  Int *chanmap,
		  Int *polmap,
		  Int *polused,
		  Int *ant1, 
		  Int *ant2, 
		  Int *nant, 
		  Int *scanno,
		  Double *sigma, 
		  Float *raoff, Float *decoff,
		  Double *area, 
		  Int *dograd,
		  Int *doPointingCorrection,
		  Int *nPA,
		  Int *paIndex,
		  Int *CFMap,
		  Int *ConjCFMap,
		  Double *currentCFPA, Double *actualPA, Double *cfRefFreq_p);
    void dpbwgrad(Double *uvw,
		  Double *dphase,
		  Complex *values,
		  Int *nvispol,
		  Int *nvischan,
		  Complex *gazvalues,
		  Complex *gelvalues,
		  Int *doconj,
		  const Int *flag,
		  const Int *rflag,
		  Int *nrow,
		  Int *rownum,
		  Double *scale,
		  Double *offset,
		  const Complex *grid,
		  Int *nx,
		  Int *ny,
		  Int *npol,
		  Int *nchan,
		  const Double *freq,
		  const Double *c,
		  Int *support,
		  Int *convsize,
		  Int *sampling,
		  Int *wconvsize,
		  Complex *convfunc,
		  Int *chanmap,
		  Int *polmap,
		  Int *polused,
		  Int *ant1, 
		  Int *ant2, 
		  Int *nant, 
		  Int *scanno,
		  Double *sigma, 
		  Float *raoff, Float *decoff,
		  Double *area, 
		  Int *dograd,
		  Int *doPointingCorrection,
		  Int *nPA,
		  Int *paIndex,
		  Int *CFMap,
		  Int *ConjCFMap,
		  Double *currentCFPA, Double *actualPA, Double *cfRefFreq_p);
  }
  //
  //----------------------------------------------------------------------
  //
  void nPBWProjectFT::runFortranGet(Matrix<Double>& uvw,Vector<Double>& dphase,
				   Cube<Complex>& visdata,
				   IPosition& s,
				   //				Cube<Complex>& gradVisAzData,
				   //				Cube<Complex>& gradVisElData,
				   //				IPosition& gradS,
                                    Int& /*Conj*/,
				   Cube<Int>& flags,Vector<Int>& rowFlags,
				   Int& rownr,Vector<Double>& actualOffset,
				   Array<Complex>* dataPtr,
				   Int& aNx, Int& aNy, Int& npol, Int& nchan,
				   VisBuffer& vb,Int& Nant_p, Int& scanNo,
				   Double& sigma,
				   Array<Float>& l_off,
				   Array<Float>& m_off,
                                   Double area,
				   Int& doGrad,
				   Int paIndex)
  {
    enum whichGetStorage {RAOFF,DECOFF,UVW,DPHASE,VISDATA,GRADVISAZ,GRADVISEL,
			  FLAGS,ROWFLAGS,UVSCALE,ACTUALOFFSET,DATAPTR,VBFREQ,
			  CONVSUPPORT,CONVFUNC,CHANMAP,POLMAP,VBANT1,VBANT2,CONJCFMAP,CFMAP};
    Vector<Bool> deleteThem(21);
    
    Double *uvw_p, *dphase_p, *actualOffset_p, *vb_freq_p, *uvScale_p;
    Complex *visdata_p, *dataPtr_p, *f_convFunc_p;
    Int *flags_p, *rowFlags_p, *chanMap_p, *polMap_p, *convSupport_p, *vb_ant1_p, *vb_ant2_p,
      *ConjCFMap_p, *CFMap_p;
    Float *l_off_p, *m_off_p;
    Double actualPA;
    
    Vector<Int> ConjCFMap, CFMap;
    /*
      ConjCFMap = CFMap = polMap;
      CFMap = makeConjPolMap(vb);
    */
    Int N;
    actualPA = getVBPA(vb);

    N=polMap.nelements();
    CFMap = polMap; ConjCFMap = polMap;
    for(Int i=0;i<N;i++) CFMap[i] = polMap[N-i-1];
    
    Array<Complex> rotatedConvFunc;
//     SynthesisUtils::rotateComplexArray(logIO(), convFunc, convFuncCS_p, 
// 				       rotatedConvFunc,(currentCFPA-actualPA),"CUBIC");
    SynthesisUtils::rotateComplexArray(logIO(), convFunc, convFuncCS_p, 
    				       rotatedConvFunc,0.0,"LINEAR");
    // SynthesisUtils::rotateComplexArray(logIO(), convFunc, convFuncCS_p, 
    // 				       rotatedConvFunc,(currentCFPA-actualPA),"LINEAR");

    ConjCFMap = polMap;
    makeCFPolMap(vb,CFMap);
    makeConjPolMap(vb,CFMap,ConjCFMap);

    
    ConjCFMap_p     = ConjCFMap.getStorage(deleteThem(CONJCFMAP));
    CFMap_p         = CFMap.getStorage(deleteThem(CFMAP));
    
    uvw_p           = uvw.getStorage(deleteThem(UVW));
    dphase_p        = dphase.getStorage(deleteThem(DPHASE));
    visdata_p       = visdata.getStorage(deleteThem(VISDATA));
    //  gradVisAzData_p = gradVisAzData.getStorage(deleteThem(GRADVISAZ));
    //  gradVisElData_p = gradVisElData.getStorage(deleteThem(GRADVISEL));
    flags_p         = flags.getStorage(deleteThem(FLAGS));
    rowFlags_p      = rowFlags.getStorage(deleteThem(ROWFLAGS));
    uvScale_p       = uvScale.getStorage(deleteThem(UVSCALE));
    actualOffset_p  = actualOffset.getStorage(deleteThem(ACTUALOFFSET));
    dataPtr_p       = dataPtr->getStorage(deleteThem(DATAPTR));
    vb_freq_p       = vb.frequency().getStorage(deleteThem(VBFREQ));
    convSupport_p   = convSupport.getStorage(deleteThem(CONVSUPPORT));
    //    f_convFunc_p      = convFunc.getStorage(deleteThem(CONVFUNC));
    f_convFunc_p      = rotatedConvFunc.getStorage(deleteThem(CONVFUNC));
    chanMap_p       = chanMap.getStorage(deleteThem(CHANMAP));
    polMap_p        = polMap.getStorage(deleteThem(POLMAP));
    vb_ant1_p       = vb.antenna1().getStorage(deleteThem(VBANT1));
    vb_ant2_p       = vb.antenna2().getStorage(deleteThem(VBANT2));
    l_off_p     = l_off.getStorage(deleteThem(RAOFF));
    m_off_p    = m_off.getStorage(deleteThem(DECOFF));
    
    Int npa=convSupport.shape()(2),actualConvSize;
    Int paIndex_Fortran = paIndex;
    actualConvSize = convFunc.shape()(0);
    
    IPosition shp=convSupport.shape();
    
    dpbwproj(uvw_p,
	     dphase_p,
	     //		  vb.modelVisCube().getStorage(del),
	     visdata_p,
	     &s.asVector()(0),
	     &s.asVector()(1),
	     //	   gradVisAzData_p,
	     //	   gradVisElData_p,
	     //	    &gradS(0),
	     //	    &gradS(1),
	     //	   &Conj,
	     flags_p,
	     rowFlags_p,
	     &s.asVector()(2),
	     &rownr,
	     uvScale_p,
	     actualOffset_p,
	     dataPtr_p,
	     &aNx,
	     &aNy,
	     &npol,
	     &nchan,
	     vb_freq_p,
	     &C::c,
	     convSupport_p,
	     &actualConvSize,
	     &convSampling,
	     &wConvSize,
	     f_convFunc_p,
	     chanMap_p,
	     polMap_p,
	     &polInUse,
	     vb_ant1_p,
	     vb_ant2_p,
	     &Nant_p,
	     &scanNo,
	     &sigma,
	     l_off_p, m_off_p,
	     &area,
	     &doGrad,
	     &doPointing,
	     &npa,
	     &paIndex_Fortran,
	     CFMap_p,
	     ConjCFMap_p,
	     &currentCFPA
	     ,&actualPA,&cfRefFreq_p
	     );
    
    ConjCFMap.freeStorage((const Int *&)ConjCFMap_p,deleteThem(CONJCFMAP));
    CFMap.freeStorage((const Int *&)CFMap_p,deleteThem(CFMAP));
    
    l_off.freeStorage((const Float*&)l_off_p,deleteThem(RAOFF));
    m_off.freeStorage((const Float*&)m_off_p,deleteThem(DECOFF));
    uvw.freeStorage((const Double*&)uvw_p,deleteThem(UVW));
    dphase.freeStorage((const Double*&)dphase_p,deleteThem(DPHASE));
    visdata.putStorage(visdata_p,deleteThem(VISDATA));
    flags.freeStorage((const Int*&) flags_p,deleteThem(FLAGS));
    rowFlags.freeStorage((const Int *&)rowFlags_p,deleteThem(ROWFLAGS));
    actualOffset.freeStorage((const Double*&)actualOffset_p,deleteThem(ACTUALOFFSET));
    dataPtr->freeStorage((const Complex *&)dataPtr_p,deleteThem(DATAPTR));
    uvScale.freeStorage((const Double*&) uvScale_p,deleteThem(UVSCALE));
    vb.frequency().freeStorage((const Double*&)vb_freq_p,deleteThem(VBFREQ));
    convSupport.freeStorage((const Int*&)convSupport_p,deleteThem(CONVSUPPORT));
    convFunc.freeStorage((const Complex *&)f_convFunc_p,deleteThem(CONVFUNC));
    chanMap.freeStorage((const Int*&)chanMap_p,deleteThem(CHANMAP));
    polMap.freeStorage((const Int*&) polMap_p,deleteThem(POLMAP));
    vb.antenna1().freeStorage((const Int*&) vb_ant1_p,deleteThem(VBANT1));
    vb.antenna2().freeStorage((const Int*&) vb_ant2_p,deleteThem(VBANT2));
  }
  //
  //----------------------------------------------------------------------
  //
  void nPBWProjectFT::runFortranGetGrad(Matrix<Double>& uvw,Vector<Double>& dphase,
				       Cube<Complex>& visdata,
				       IPosition& s,
				       Cube<Complex>& gradVisAzData,
				       Cube<Complex>& gradVisElData,
				       //				     IPosition& gradS,
				       Int& Conj,
				       Cube<Int>& flags,Vector<Int>& rowFlags,
				       Int& rownr,Vector<Double>& actualOffset,
				       Array<Complex>* dataPtr,
				       Int& aNx, Int& aNy, Int& npol, Int& nchan,
				       VisBuffer& vb,Int& Nant_p, Int& scanNo,
				       Double& sigma,
				       Array<Float>& l_off,
				       Array<Float>& m_off,
				       Double area,
				       Int& doGrad,
				       Int paIndex)
  {
    enum whichGetStorage {RAOFF,DECOFF,UVW,DPHASE,VISDATA,GRADVISAZ,GRADVISEL,
			  FLAGS,ROWFLAGS,UVSCALE,ACTUALOFFSET,DATAPTR,VBFREQ,
			  CONVSUPPORT,CONVFUNC,CHANMAP,POLMAP,VBANT1,VBANT2,CONJCFMAP,CFMAP};
    Vector<Bool> deleteThem(21);
    
    Double *uvw_p, *dphase_p, *actualOffset_p, *vb_freq_p, *uvScale_p;
    Complex *visdata_p, *dataPtr_p, *f_convFunc_p;
    Complex *gradVisAzData_p, *gradVisElData_p;
    Int *flags_p, *rowFlags_p, *chanMap_p, *polMap_p, *convSupport_p, *vb_ant1_p, *vb_ant2_p,
      *ConjCFMap_p, *CFMap_p;
    Float *l_off_p, *m_off_p;
    Double actualPA;

    Vector<Int> ConjCFMap, CFMap;
    actualPA = getVBPA(vb);
    ConjCFMap = polMap;
    makeCFPolMap(vb,CFMap);
    makeConjPolMap(vb,CFMap,ConjCFMap);

    Array<Complex> rotatedConvFunc;
//     SynthesisUtils::rotateComplexArray(logIO(), convFunc, convFuncCS_p, 
//  				       rotatedConvFunc,(currentCFPA-actualPA),"LINEAR");
    SynthesisUtils::rotateComplexArray(logIO(), convFunc, convFuncCS_p, 
    				       rotatedConvFunc,0.0);
    // SynthesisUtils::rotateComplexArray(logIO(), convFunc, convFuncCS_p, 
    // 				       rotatedConvFunc,(currentCFPA-actualPA),"LINEAR");

    ConjCFMap_p     = ConjCFMap.getStorage(deleteThem(CONJCFMAP));
    CFMap_p         = CFMap.getStorage(deleteThem(CFMAP));
    
    uvw_p           = uvw.getStorage(deleteThem(UVW));
    dphase_p        = dphase.getStorage(deleteThem(DPHASE));
    visdata_p       = visdata.getStorage(deleteThem(VISDATA));
    gradVisAzData_p = gradVisAzData.getStorage(deleteThem(GRADVISAZ));
    gradVisElData_p = gradVisElData.getStorage(deleteThem(GRADVISEL));
    flags_p         = flags.getStorage(deleteThem(FLAGS));
    rowFlags_p      = rowFlags.getStorage(deleteThem(ROWFLAGS));
    uvScale_p       = uvScale.getStorage(deleteThem(UVSCALE));
    actualOffset_p  = actualOffset.getStorage(deleteThem(ACTUALOFFSET));
    dataPtr_p       = dataPtr->getStorage(deleteThem(DATAPTR));
    vb_freq_p       = vb.frequency().getStorage(deleteThem(VBFREQ));
    convSupport_p   = convSupport.getStorage(deleteThem(CONVSUPPORT));
    //    f_convFunc_p      = convFunc.getStorage(deleteThem(CONVFUNC));
    f_convFunc_p      = rotatedConvFunc.getStorage(deleteThem(CONVFUNC));
    chanMap_p       = chanMap.getStorage(deleteThem(CHANMAP));
    polMap_p        = polMap.getStorage(deleteThem(POLMAP));
    vb_ant1_p       = vb.antenna1().getStorage(deleteThem(VBANT1));
    vb_ant2_p       = vb.antenna2().getStorage(deleteThem(VBANT2));
    l_off_p     = l_off.getStorage(deleteThem(RAOFF));
    m_off_p    = m_off.getStorage(deleteThem(DECOFF));
    
    Int npa=convSupport.shape()(2),actualConvSize;
    Int paIndex_Fortran = paIndex;
    actualConvSize = convFunc.shape()(0);
    
    IPosition shp=convSupport.shape();

    dpbwgrad(uvw_p,
	     dphase_p,
	     //		  vb.modelVisCube().getStorage(del),
	     visdata_p,
	     &s.asVector()(0),
	     &s.asVector()(1),
	     gradVisAzData_p,
	     gradVisElData_p,
	     //	    &gradS(0),
	     //	    &gradS(1),
	     &Conj,
	     flags_p,
	     rowFlags_p,
	     &s.asVector()(2),
	     &rownr,
	     uvScale_p,
	     actualOffset_p,
	     dataPtr_p,
	     &aNx,
	     &aNy,
	     &npol,
	     &nchan,
	     vb_freq_p,
	     &C::c,
	     convSupport_p,
	     &actualConvSize,
	     &convSampling,
	     &wConvSize,
	     f_convFunc_p,
	     chanMap_p,
	     polMap_p,
	     &polInUse,
	     vb_ant1_p,
	     vb_ant2_p,
	     &Nant_p,
	     &scanNo,
	     &sigma,
	     l_off_p, m_off_p,
	     &area,
	     &doGrad,
	     &doPointing,
	     &npa,
	     &paIndex_Fortran,
	     CFMap_p,
	     ConjCFMap_p,
	     &currentCFPA
	     ,&actualPA,&cfRefFreq_p
	     );

    ConjCFMap.freeStorage((const Int *&)ConjCFMap_p,deleteThem(CONJCFMAP));
    CFMap.freeStorage((const Int *&)CFMap_p,deleteThem(CFMAP));
    
    l_off.freeStorage((const Float*&)l_off_p,deleteThem(RAOFF));
    m_off.freeStorage((const Float*&)m_off_p,deleteThem(DECOFF));
    uvw.freeStorage((const Double*&)uvw_p,deleteThem(UVW));
    dphase.freeStorage((const Double*&)dphase_p,deleteThem(DPHASE));
    visdata.putStorage(visdata_p,deleteThem(VISDATA));
    gradVisAzData.putStorage(gradVisAzData_p,deleteThem(GRADVISAZ));
    gradVisElData.putStorage(gradVisElData_p,deleteThem(GRADVISEL));
    flags.freeStorage((const Int*&) flags_p,deleteThem(FLAGS));
    rowFlags.freeStorage((const Int *&)rowFlags_p,deleteThem(ROWFLAGS));
    actualOffset.freeStorage((const Double*&)actualOffset_p,deleteThem(ACTUALOFFSET));
    dataPtr->freeStorage((const Complex *&)dataPtr_p,deleteThem(DATAPTR));
    uvScale.freeStorage((const Double*&) uvScale_p,deleteThem(UVSCALE));
    vb.frequency().freeStorage((const Double*&)vb_freq_p,deleteThem(VBFREQ));
    convSupport.freeStorage((const Int*&)convSupport_p,deleteThem(CONVSUPPORT));
    convFunc.freeStorage((const Complex *&)f_convFunc_p,deleteThem(CONVFUNC));
    chanMap.freeStorage((const Int*&)chanMap_p,deleteThem(CHANMAP));
    polMap.freeStorage((const Int*&) polMap_p,deleteThem(POLMAP));
    vb.antenna1().freeStorage((const Int*&) vb_ant1_p,deleteThem(VBANT1));
    vb.antenna2().freeStorage((const Int*&) vb_ant2_p,deleteThem(VBANT2));
  }
  //
  //----------------------------------------------------------------------
  //
  void nPBWProjectFT::runFortranPut(Matrix<Double>& uvw,Vector<Double>& dphase,
				   const Complex& visdata,
				   IPosition& s,
				   //				Cube<Complex>& gradVisAzData,
				   //				Cube<Complex>& gradVisElData,
				   //				IPosition& gradS,
                                    Int& /*Conj*/,
				   Cube<Int>& flags,Vector<Int>& rowFlags,
				   const Matrix<Float>& weight,
				   Int& rownr,Vector<Double>& actualOffset,
				   Array<Complex>& dataPtr,
				   Int& aNx, Int& aNy, Int& npol, Int& nchan,
				   const VisBuffer& vb,Int& Nant_p, Int& scanNo,
				   Double& sigma,
				   Array<Float>& l_off,
				   Array<Float>& m_off,
				   Matrix<Double>& sumWeight,
				   Double& area,
                                   Int& doGrad,
				   Int& doPSF,
				   Int paIndex)
  {
    enum whichGetStorage {RAOFF,DECOFF,UVW,DPHASE,VISDATA,GRADVISAZ,GRADVISEL,
			  FLAGS,ROWFLAGS,UVSCALE,ACTUALOFFSET,DATAPTR,VBFREQ,
			  CONVSUPPORT,CONVFUNC,CHANMAP,POLMAP,VBANT1,VBANT2,WEIGHT,
			  SUMWEIGHT,CONJCFMAP,CFMAP};
    Vector<Bool> deleteThem(23);
    
    Double *uvw_p, *dphase_p, *actualOffset_p, *vb_freq_p, *uvScale_p;
    Complex *dataPtr_p, *f_convFunc_p;
    //  Complex *gradVisAzData_p, *gradVisElData_p;
    Int *flags_p, *rowFlags_p, *chanMap_p, *polMap_p, *convSupport_p, *vb_ant1_p, *vb_ant2_p,
      *ConjCFMap_p, *CFMap_p;
    Float *l_off_p, *m_off_p;
    Float *weight_p;Double *sumwt_p;
    Double actualPA;
    const Complex *visdata_p=&visdata;
    
    Vector<Int> ConjCFMap, CFMap;
    actualPA = getVBPA(vb);
    ConjCFMap = polMap;

    Array<Complex> rotatedConvFunc;
//    SynthesisUtils::rotateComplexArray(logIO(), convFunc, convFuncCS_p, 
//				       rotatedConvFunc,(currentCFPA-actualPA),"LINEAR");
     SynthesisUtils::rotateComplexArray(logIO(), convFunc, convFuncCS_p, 
 				       rotatedConvFunc,0.0,"LINEAR");

    /*
    CFMap = polMap; ConjCFMap = polMap;
    CFMap = makeConjPolMap(vb);
    */
     makeCFPolMap(vb,CFMap);
    makeConjPolMap(vb,CFMap,ConjCFMap);

    ConjCFMap_p     = ConjCFMap.getStorage(deleteThem(CONJCFMAP));
    CFMap_p         = CFMap.getStorage(deleteThem(CFMAP));
    
    uvw_p           = uvw.getStorage(deleteThem(UVW));
    dphase_p        = dphase.getStorage(deleteThem(DPHASE));
    //  visdata_p       = visdata.getStorage(deleteThem(VISDATA));
    //  gradVisAzData_p = gradVisAzData.getStorage(deleteThem(GRADVISAZ));
    //  gradVisElData_p = gradVisElData.getStorage(deleteThem(GRADVISEL));
    flags_p         = flags.getStorage(deleteThem(FLAGS));
    rowFlags_p      = rowFlags.getStorage(deleteThem(ROWFLAGS));
    uvScale_p       = uvScale.getStorage(deleteThem(UVSCALE));
    actualOffset_p  = actualOffset.getStorage(deleteThem(ACTUALOFFSET));
    dataPtr_p       = dataPtr.getStorage(deleteThem(DATAPTR));
    vb_freq_p       = (Double *)(vb.frequency().getStorage(deleteThem(VBFREQ)));
    convSupport_p   = convSupport.getStorage(deleteThem(CONVSUPPORT));
    //    f_convFunc_p      = convFunc.getStorage(deleteThem(CONVFUNC));
    f_convFunc_p      = rotatedConvFunc.getStorage(deleteThem(CONVFUNC));
    chanMap_p       = chanMap.getStorage(deleteThem(CHANMAP));
    polMap_p        = polMap.getStorage(deleteThem(POLMAP));
    vb_ant1_p       = (Int *)(vb.antenna1().getStorage(deleteThem(VBANT1)));
    vb_ant2_p       = (Int *)(vb.antenna2().getStorage(deleteThem(VBANT2)));
    l_off_p     = l_off.getStorage(deleteThem(RAOFF));
    m_off_p    = m_off.getStorage(deleteThem(DECOFF));
    weight_p        = (Float *)(weight.getStorage(deleteThem(WEIGHT)));
    sumwt_p         = sumWeight.getStorage(deleteThem(SUMWEIGHT));
    
    
    Int npa=convSupport.shape()(2),actualConvSize;
    Int paIndex_Fortran = paIndex; 
    actualConvSize = convFunc.shape()(0);
    
    IPosition shp=convSupport.shape();
    
    gpbwproj(uvw_p,
	     dphase_p,
	     //		  vb.modelVisCube().getStorage(del),
	     visdata_p,
	     &s.asVector()(0),
	     &s.asVector()(1),
	     //	   gradVisAzData_p,
	     //	   gradVisElData_p,
	     //	    &gradS(0),
	     //	    &gradS(1),
	     //	   &Conj,
	     &doPSF,
	     flags_p,
	     rowFlags_p,
	     weight_p,
	     &s.asVector()(2),
	     &rownr,
	     uvScale_p,
	     actualOffset_p,
	     dataPtr_p,
	     &aNx,
	     &aNy,
	     &npol,
	     &nchan,
	     vb_freq_p,
	     &C::c,
	     convSupport_p,
	     &actualConvSize,
	     &convSampling,
	     &wConvSize,
	     f_convFunc_p,
	     chanMap_p,
	     polMap_p,
	     &polInUse,
	     sumwt_p,
	     vb_ant1_p,
	     vb_ant2_p,
	     &Nant_p,
	     &scanNo,
	     &sigma,
	     l_off_p, m_off_p,
	     &area,
	     &doGrad,
	     &doPointing,
	     &npa,
	     &paIndex_Fortran,
	     CFMap_p,
	     ConjCFMap_p,
	     &currentCFPA
	     ,&actualPA,&cfRefFreq_p
	     );
    
    ConjCFMap.freeStorage((const Int *&)ConjCFMap_p,deleteThem(CONJCFMAP));
    CFMap.freeStorage((const Int *&)CFMap_p,deleteThem(CFMAP));
    
    l_off.freeStorage((const Float*&)l_off_p,deleteThem(RAOFF));
    m_off.freeStorage((const Float*&)m_off_p,deleteThem(DECOFF));
    uvw.freeStorage((const Double*&)uvw_p,deleteThem(UVW));
    dphase.freeStorage((const Double*&)dphase_p,deleteThem(DPHASE));
    //  visdata.putStorage(visdata_p,deleteThem(VISDATA));
    //  gradVisAzData.putStorage(gradVisAzData_p,deleteThem(GRADVISAZ));
    //  gradVisElData.putStorage(gradVisElData_p,deleteThem(GRADVISEL));
    flags.freeStorage((const Int*&) flags_p,deleteThem(FLAGS));
    rowFlags.freeStorage((const Int *&)rowFlags_p,deleteThem(ROWFLAGS));
    actualOffset.freeStorage((const Double*&)actualOffset_p,deleteThem(ACTUALOFFSET));
    dataPtr.freeStorage((const Complex *&)dataPtr_p,deleteThem(DATAPTR));
    uvScale.freeStorage((const Double*&) uvScale_p,deleteThem(UVSCALE));
    vb.frequency().freeStorage((const Double*&)vb_freq_p,deleteThem(VBFREQ));
    convSupport.freeStorage((const Int*&)convSupport_p,deleteThem(CONVSUPPORT));
    convFunc.freeStorage((const Complex *&)f_convFunc_p,deleteThem(CONVFUNC));
    chanMap.freeStorage((const Int*&)chanMap_p,deleteThem(CHANMAP));
    polMap.freeStorage((const Int*&) polMap_p,deleteThem(POLMAP));
    vb.antenna1().freeStorage((const Int*&) vb_ant1_p,deleteThem(VBANT1));
    vb.antenna2().freeStorage((const Int*&) vb_ant2_p,deleteThem(VBANT2));
    weight.freeStorage((const Float*&)weight_p,deleteThem(WEIGHT));
    sumWeight.putStorage(sumwt_p,deleteThem(SUMWEIGHT));
  }
  //
  //---------------------------------------------------------------
  //
  void nPBWProjectFT::put(const VisBuffer& vb, Int row, Bool dopsf,
			  FTMachine::Type type)
  {
    // Take care of translation of Bools to Integer
    makingPSF=dopsf;
    
    findConvFunction(*image, vb);
    

    const Matrix<Float> *imagingweight;
    imagingweight=&(vb.imagingWeight());

    const Cube<Complex> *data;
    if(type==FTMachine::MODEL)
      data=&(vb.modelVisCube());
    else if(type==FTMachine::CORRECTED)
      data=&(vb.correctedVisCube());
    else
      data=&(vb.visCube());
    
    Bool isCopy;
    const casacore::Complex *datStorage=data->getStorage(isCopy);
    Int NAnt = 0;

    if (doPointing) NAnt = findPointingOffsets(vb,l_offsets,m_offsets,true);
    
    //
    // If row is -1 then we pass through all rows
    //
    Int startRow, endRow, nRow;
    if (row==-1) 
      {
	nRow=vb.nRow();
	startRow=0;
	endRow=nRow-1;
      } 
    else 
      {
	nRow=1;
	startRow=row;
	endRow=row;
      }
    //    
    // Get the uvws in a form that Fortran can use and do that
    // necessary phase rotation. On a Pentium Pro 200 MHz when null,
    // this step takes about 50us per uvw point. This is just barely
    // noticeable for Stokes I continuum and irrelevant for other
    // cases.
    //
    Matrix<Double> uvw(3, vb.uvw().nelements());
    uvw=0.0;
    Vector<Double> dphase(vb.uvw().nelements());
    dphase=0.0;
    //NEGATING to correct for an image inversion problem
    for (Int i=startRow;i<=endRow;i++) 
      {
	for (Int idim=0;idim<2;idim++) uvw(idim,i)=-vb.uvw()(i)(idim);
	uvw(2,i)=vb.uvw()(i)(2);
      }
    
    rotateUVW(uvw, dphase, vb);
    refocus(uvw, vb.antenna1(), vb.antenna2(), dphase, vb);
    
    // This is the convention for dphase
    dphase*=-1.0;
    
    Cube<Int> flags(vb.flagCube().shape());
    flags=0;
    flags(vb.flagCube())=true;
    
    Vector<Int> rowFlags(vb.nRow());
    rowFlags=0;
    rowFlags(vb.flagRow())=true;
    if(!usezero_p) 
      for (Int rownr=startRow; rownr<=endRow; rownr++) 
	if(vb.antenna1()(rownr)==vb.antenna2()(rownr)) rowFlags(rownr)=1;
    //Check if ms has changed then cache new spw and chan selection
    if(vb.newMS())
      matchAllSpwChans(vb);  
    
    //Here we redo the match or use previous match
    
    //Channel matching for the actual spectral window of buffer
    if(doConversion_p[vb.spectralWindow()])
      matchChannel(vb.spectralWindow(), vb);
    else
      {
	chanMap.resize();
	chanMap=multiChanMap_p[vb.spectralWindow()];
      }
    
    if(isTiled) 
      {// Tiled Version
	Double invLambdaC=vb.frequency()(0)/C::c;
	Vector<Double> uvLambda(2);
	Vector<Int> centerLoc2D(2);
	centerLoc2D=0;
	//
	// Loop over all rows
	//
	for (Int rownr=startRow; rownr<=endRow; rownr++) 
	  {
	    // Calculate uvw for this row at the center frequency
	    uvLambda(0)=uvw(0,rownr)*invLambdaC;
	    uvLambda(1)=uvw(1,rownr)*invLambdaC;
	    centerLoc2D=gridder->location(centerLoc2D, uvLambda);
	    //
	    // Is this point on the grid?
	    //
	    if(gridder->onGrid(centerLoc2D)) 
	      {
		// Get the tile
		Array<Complex>* dataPtr=getDataPointer(centerLoc2D, false);
		Int aNx=dataPtr->shape()(0);
		Int aNy=dataPtr->shape()(1);
		//
		// Now use FORTRAN to do the gridding. Remember to 
		// ensure that the shape and offsets of the tile are 
		// accounted for.
		//
		Vector<Double> actualOffset(3);
		for (Int i=0;i<2;i++) actualOffset(i)=uvOffset(i)-Double(offsetLoc(i));
		
		actualOffset(2)=uvOffset(2);
		IPosition s(flags.shape());
		//
		// Now pass all the information down to a FORTRAN routine to
		// do the work
		//
		Int Conj=0,doPSF;
		Int ScanNo=0,doGrad=0;
		Double area=1.0;
		
		Int tmpPAI=PAIndex+1;
		if (dopsf) doPSF=1; else doPSF=0;
		runFortranPut(uvw,dphase,*datStorage,s,Conj,flags,rowFlags,
			      *imagingweight,rownr,actualOffset,
			      *dataPtr,aNx,aNy,npol,nchan,vb,NAnt,ScanNo,sigma,
			      l_offsets,m_offsets,sumWeight,area,doGrad,doPSF,tmpPAI);
	      }
	  }
      }
    else 
      {//Non-tiled version
	IPosition s(flags.shape());
	
	Int Conj=0,doPSF=0;
	Int ScanNo=0,doGrad=0;Double area=1.0;
	
	if (dopsf) doPSF=1;
	
	Int tmpPAI=PAIndex+1;
	runFortranPut(uvw,dphase,*datStorage,s,Conj,flags,rowFlags,
		      *imagingweight,
		      row,uvOffset,
		      griddedData,nx,ny,npol,nchan,vb,NAnt,ScanNo,sigma,
		      l_offsets,m_offsets,sumWeight,area,doGrad,doPSF,tmpPAI);
      }
    
    data->freeStorage(datStorage, isCopy);
  }
  //
  //----------------------------------------------------------------------
  //
  void nPBWProjectFT::initVisBuffer(VisBuffer& vb, Type whichVBColumn)
  {
    if (whichVBColumn      == FTMachine::MODEL)    vb.modelVisCube()=Complex(0.0,0.0);
    else if (whichVBColumn == FTMachine::OBSERVED) vb.visCube()=Complex(0.0,0.0);
  }
  //
  //----------------------------------------------------------------------
  //
  void nPBWProjectFT::initVisBuffer(VisBuffer& vb, Type whichVBColumn, Int row)
  {
    if (whichVBColumn == FTMachine::MODEL)
      vb.modelVisCube().xyPlane(row)=Complex(0.0,0.0);
    else if (whichVBColumn == FTMachine::OBSERVED)
      vb.visCube().xyPlane(row)=Complex(0.0,0.0);
  }
  //
  //---------------------------------------------------------------
  //
  // Predict the coherences as well as their derivatives w.r.t. the
  // pointing offsets.
  //
  void nPBWProjectFT::nget(VisBuffer& vb,
			  // These offsets should be appropriate for the VB
			  Array<Float>& l_off, Array<Float>& m_off,
			  Cube<Complex>& Mout,
			  Cube<Complex>& dMout1,
			  Cube<Complex>& dMout2,
			  Int Conj, Int doGrad)
  {
    Int startRow, endRow, nRow;
    nRow=vb.nRow();
    startRow=0;
    endRow=nRow-1;

    Mout = dMout1 = dMout2 = Complex(0,0);

    findConvFunction(*image, vb);
    Int NAnt=0;
    if (bandID_p == -1) bandID_p=getVisParams(vb);
    if (doPointing)   
      NAnt = findPointingOffsets(vb,l_offsets,m_offsets,false);

    l_offsets=l_off;
    m_offsets=m_off;
    Matrix<Double> uvw(3, vb.uvw().nelements());
    uvw=0.0;
    Vector<Double> dphase(vb.uvw().nelements());
    dphase=0.0;
    //NEGATING to correct for an image inversion problem
    for (Int i=startRow;i<=endRow;i++) 
      {
	for (Int idim=0;idim<2;idim++) uvw(idim,i)=-vb.uvw()(i)(idim);
	uvw(2,i)=vb.uvw()(i)(2);
      }
    
    rotateUVW(uvw, dphase, vb);
    refocus(uvw, vb.antenna1(), vb.antenna2(), dphase, vb);
    
    // This is the convention for dphase
    dphase*=-1.0;

    Cube<Int> flags(vb.flagCube().shape());
    flags=0;
    flags(vb.flagCube())=true;
    
    //Check if ms has changed then cache new spw and chan selection
    if(vb.newMS())
      matchAllSpwChans(vb);
    
    //Here we redo the match or use previous match
    //
    //Channel matching for the actual spectral window of buffer
    //
    if(doConversion_p[vb.spectralWindow()])
      matchChannel(vb.spectralWindow(), vb);
    else
      {
	chanMap.resize();
	chanMap=multiChanMap_p[vb.spectralWindow()];
      }
    
    Vector<Int> rowFlags(vb.nRow());
    rowFlags=0;
    rowFlags(vb.flagRow())=true;
    if(!usezero_p) 
      for (Int rownr=startRow; rownr<=endRow; rownr++) 
	if(vb.antenna1()(rownr)==vb.antenna2()(rownr)) rowFlags(rownr)=1;
    
    IPosition s,gradS;
    Cube<Complex> visdata,gradVisAzData,gradVisElData;
    //
    // visdata now references the Mout data structure rather than to the internal VB storeage.
    //
    visdata.reference(Mout);

    if (doGrad)
      {
	// The following should reference some slice of dMout?
	gradVisAzData.reference(dMout1);
	gradVisElData.reference(dMout2);
      }
    //
    // Begin the actual de-gridding.
    //
    if(isTiled) 
      {
	logIO() << "nPBWProjectFT::nget(): The sky model is tiled" << LogIO::NORMAL << LogIO::POST;
	Double invLambdaC=vb.frequency()(0)/C::c;
	Vector<Double> uvLambda(2);
	Vector<Int> centerLoc2D(2);
	centerLoc2D=0;
	
	// Loop over all rows
	for (Int rownr=startRow; rownr<=endRow; rownr++) 
	  {
	    
	    // Calculate uvw for this row at the center frequency
	    uvLambda(0)=uvw(0, rownr)*invLambdaC;
	    uvLambda(1)=uvw(1, rownr)*invLambdaC;
	    centerLoc2D=gridder->location(centerLoc2D, uvLambda);
	    
	    // Is this point on the grid?
	    if(gridder->onGrid(centerLoc2D)) 
	      {
		
		// Get the tile
		Array<Complex>* dataPtr=getDataPointer(centerLoc2D, true);
		gridder->setOffset(IPosition(2, offsetLoc(0), offsetLoc(1)));
		Int aNx=dataPtr->shape()(0);
		Int aNy=dataPtr->shape()(1);
		
		// Now use FORTRAN to do the gridding. Remember to 
		// ensure that the shape and offsets of the tile are 
		// accounted for.
		
		Vector<Double> actualOffset(3);
		for (Int i=0;i<2;i++) 
		  actualOffset(i)=uvOffset(i)-Double(offsetLoc(i));
		
		actualOffset(2)=uvOffset(2);
		IPosition s(vb.modelVisCube().shape());
		
		Int ScanNo=0, tmpPAI;
		Double area=1.0;
		tmpPAI = PAIndex+1;
		runFortranGetGrad(uvw,dphase,visdata,s,
				  gradVisAzData,gradVisElData,
				  Conj,flags,rowFlags,rownr,
				  actualOffset,dataPtr,aNx,aNy,npol,nchan,vb,NAnt,ScanNo,sigma,
				  l_offsets,m_offsets,area,doGrad,tmpPAI);
	      }
	  }
      }
    else 
      {
	IPosition s(vb.modelVisCube().shape());
	Int ScanNo=0, tmpPAI, trow=-1;
	Double area=1.0;
	tmpPAI = PAIndex+1;
	runFortranGetGrad(uvw,dphase,visdata/*vb.modelVisCube()*/,s,
			  gradVisAzData, gradVisElData,
			  Conj,flags,rowFlags,trow,
			  uvOffset,&griddedData,nx,ny,npol,nchan,vb,NAnt,ScanNo,sigma,
			  l_offsets,m_offsets,area,doGrad,tmpPAI);
      }
    
  }
  void nPBWProjectFT::get(VisBuffer& vb,       
			 VisBuffer& gradVBAz,
			 VisBuffer& gradVBEl,
			 Cube<Float>& pointingOffsets,
			 Int row,  // default row=-1 
			 Type whichVBColumn, // default whichVBColumn = FTMachine::MODEL
			 Type whichGradVBColumn,// default whichGradVBColumn = FTMachine::MODEL
			 Int Conj, Int doGrad) // default Conj=0, doGrad=1
  {
    // If row is -1 then we pass through all rows
    Int startRow, endRow, nRow;
    if (row==-1) 
      {
	nRow=vb.nRow();
	startRow=0;
	endRow=nRow-1;
	initVisBuffer(vb,whichVBColumn);
	if (doGrad)
	  {
	    initVisBuffer(gradVBAz, whichGradVBColumn);
	    initVisBuffer(gradVBEl, whichGradVBColumn);
	  }
      }
    else 
      {
	nRow=1;
	startRow=row;
	endRow=row;
	initVisBuffer(vb,whichVBColumn,row);
	if (doGrad)
	  {
	    initVisBuffer(gradVBAz, whichGradVBColumn,row);
	    initVisBuffer(gradVBEl, whichGradVBColumn,row);
	  }
      }
    
    findConvFunction(*image, vb);

    if (bandID_p == -1) bandID_p=getVisParams(vb);
    Int NAnt=0;
    if (doPointing)   
      NAnt = findPointingOffsets(vb,pointingOffsets,l_offsets,m_offsets,false);

    Matrix<Double> uvw(3, vb.uvw().nelements());
    uvw=0.0;
    Vector<Double> dphase(vb.uvw().nelements());
    dphase=0.0;
    //NEGATING to correct for an image inversion problem
    for (Int i=startRow;i<=endRow;i++) 
      {
	for (Int idim=0;idim<2;idim++) uvw(idim,i)=-vb.uvw()(i)(idim);
	uvw(2,i)=vb.uvw()(i)(2);
      }
    
    rotateUVW(uvw, dphase, vb);
    refocus(uvw, vb.antenna1(), vb.antenna2(), dphase, vb);
    
    // This is the convention for dphase
    dphase*=-1.0;
    
    
    Cube<Int> flags(vb.flagCube().shape());
    flags=0;
    flags(vb.flagCube())=true;
    //    
    //Check if ms has changed then cache new spw and chan selection
    //
    if(vb.newMS()) matchAllSpwChans(vb);
    
    //Here we redo the match or use previous match
    //
    //Channel matching for the actual spectral window of buffer
    //
    if(doConversion_p[vb.spectralWindow()])
      matchChannel(vb.spectralWindow(), vb);
    else
      {
	chanMap.resize();
	chanMap=multiChanMap_p[vb.spectralWindow()];
      }
    
    Vector<Int> rowFlags(vb.nRow());
    rowFlags=0;
    rowFlags(vb.flagRow())=true;
    if(!usezero_p) 
      for (Int rownr=startRow; rownr<=endRow; rownr++) 
	if(vb.antenna1()(rownr)==vb.antenna2()(rownr)) rowFlags(rownr)=1;
	
    for (Int rownr=startRow; rownr<=endRow; rownr++) 
      if (vb.antenna1()(rownr) != vb.antenna2()(rownr)) 
	rowFlags(rownr) = (vb.flagRow()(rownr)==true);
    
    IPosition s,gradS;
    Cube<Complex> visdata,gradVisAzData,gradVisElData;
    if (whichVBColumn == FTMachine::MODEL) 
      {
	s = vb.modelVisCube().shape();
	visdata.reference(vb.modelVisCube());
      }
    else if (whichVBColumn == FTMachine::OBSERVED)  
      {
	s = vb.visCube().shape();
	visdata.reference(vb.visCube());
      }
    
    if (doGrad)
      {
	if (whichGradVBColumn == FTMachine::MODEL) 
	  {
	    //	    gradS = gradVBAz.modelVisCube().shape();
	    gradVisAzData.reference(gradVBAz.modelVisCube());
	    gradVisElData.reference(gradVBEl.modelVisCube());
	  }
	else if (whichGradVBColumn == FTMachine::OBSERVED)  
	  {
	    //	    gradS = gradVBAz.visCube().shape();
	    gradVisAzData.reference(gradVBAz.visCube());
	    gradVisElData.reference(gradVBEl.visCube());
	  }
      }
    
    if(isTiled) 
      {
	Double invLambdaC=vb.frequency()(0)/C::c;
	Vector<Double> uvLambda(2);
	Vector<Int> centerLoc2D(2);
	centerLoc2D=0;
	
	// Loop over all rows
	for (Int rownr=startRow; rownr<=endRow; rownr++) 
	  {
	    
	    // Calculate uvw for this row at the center frequency
	    uvLambda(0)=uvw(0, rownr)*invLambdaC;
	    uvLambda(1)=uvw(1, rownr)*invLambdaC;
	    centerLoc2D=gridder->location(centerLoc2D, uvLambda);
	    
	    // Is this point on the grid?
	    if(gridder->onGrid(centerLoc2D)) 
	      {
		
		// Get the tile
		Array<Complex>* dataPtr=getDataPointer(centerLoc2D, true);
		gridder->setOffset(IPosition(2, offsetLoc(0), offsetLoc(1)));
		Int aNx=dataPtr->shape()(0);
		Int aNy=dataPtr->shape()(1);
		
		// Now use FORTRAN to do the gridding. Remember to 
		// ensure that the shape and offsets of the tile are 
		// accounted for.
		
		Vector<Double> actualOffset(3);
		for (Int i=0;i<2;i++) 
		  actualOffset(i)=uvOffset(i)-Double(offsetLoc(i));
		
		actualOffset(2)=uvOffset(2);
		IPosition s(vb.modelVisCube().shape());
		
		Int ScanNo=0, tmpPAI;
		Double area=1.0;
		tmpPAI = PAIndex+1;
		runFortranGetGrad(uvw,dphase,visdata,s,
				  gradVisAzData,gradVisElData,
				  Conj,flags,rowFlags,rownr,
				  actualOffset,dataPtr,aNx,aNy,npol,nchan,vb,NAnt,ScanNo,sigma,
				  l_offsets,m_offsets,area,doGrad,tmpPAI);
	      }
	  }
      }
    else 
      {
	
	IPosition s(vb.modelVisCube().shape());
	Int ScanNo=0, tmpPAI;
	Double area=1.0;

	tmpPAI = PAIndex+1;

	runFortranGetGrad(uvw,dphase,visdata/*vb.modelVisCube()*/,s,
			  gradVisAzData, gradVisElData,
			  Conj,flags,rowFlags,row,
			  uvOffset,&griddedData,nx,ny,npol,nchan,vb,NAnt,ScanNo,sigma,
			  l_offsets,m_offsets,area,doGrad,tmpPAI);
// 	runFortranGet(uvw,dphase,vb.modelVisCube(),s,Conj,flags,rowFlags,row,
// 		      uvOffset,&griddedData,nx,ny,npol,nchan,vb,NAnt,ScanNo,sigma,
// 		      l_offsets,m_offsets,area,doGrad,tmpPAI);
      }
  }
  //
  //---------------------------------------------------------------
  //
  void nPBWProjectFT::get(VisBuffer& vb, Int row)
  {
    // If row is -1 then we pass through all rows
    Int startRow, endRow, nRow;
    if (row==-1) 
      {
	nRow=vb.nRow();
	startRow=0;
	endRow=nRow-1;
	vb.modelVisCube()=Complex(0.0,0.0);
      }
    else 
      {
	nRow=1;
	startRow=row;
	endRow=row;
	vb.modelVisCube().xyPlane(row)=Complex(0.0,0.0);
      }
    
    findConvFunction(*image, vb);
    
    if (bandID_p == -1) bandID_p=getVisParams(vb);
    Int NAnt=0;
    if (doPointing)   NAnt = findPointingOffsets(vb,l_offsets,m_offsets,true);
    
    // Get the uvws in a form that Fortran can use
    Matrix<Double> uvw(3, vb.uvw().nelements());
    uvw=0.0;
    Vector<Double> dphase(vb.uvw().nelements());
    dphase=0.0;
    //NEGATING to correct for an image inversion problem
    for (Int i=startRow;i<=endRow;i++) 
      {
	for (Int idim=0;idim<2;idim++) uvw(idim,i)=-vb.uvw()(i)(idim);
	uvw(2,i)=vb.uvw()(i)(2);
      }
    
    rotateUVW(uvw, dphase, vb);
    refocus(uvw, vb.antenna1(), vb.antenna2(), dphase, vb);
    
    // This is the convention for dphase
    dphase*=-1.0;
    
    
    Cube<Int> flags(vb.flagCube().shape());
    flags=0;
    flags(vb.flagCube())=true;
    
    //Check if ms has changed then cache new spw and chan selection
    if(vb.newMS())
      matchAllSpwChans(vb);
    
    //Here we redo the match or use previous match
    //
    //Channel matching for the actual spectral window of buffer
    //
    if(doConversion_p[vb.spectralWindow()])
      matchChannel(vb.spectralWindow(), vb);
    else
      {
	chanMap.resize();
	chanMap=multiChanMap_p[vb.spectralWindow()];
      }
    
    Vector<Int> rowFlags(vb.nRow());
    rowFlags=0;
    rowFlags(vb.flagRow())=true;
    
    if(!usezero_p) 
      for (Int rownr=startRow; rownr<=endRow; rownr++) 
	if(vb.antenna1()(rownr)==vb.antenna2()(rownr)) rowFlags(rownr)=1;
    
    if(isTiled) 
      {
	
	Double invLambdaC=vb.frequency()(0)/C::c;
	Vector<Double> uvLambda(2);
	Vector<Int> centerLoc2D(2);
	centerLoc2D=0;
	
	// Loop over all rows
	for (Int rownr=startRow; rownr<=endRow; rownr++) 
	  {
	    
	    // Calculate uvw for this row at the center frequency
	    uvLambda(0)=uvw(0, rownr)*invLambdaC;
	    uvLambda(1)=uvw(1, rownr)*invLambdaC;
	    centerLoc2D=gridder->location(centerLoc2D, uvLambda);
	    
	    // Is this point on the grid?
	    if(gridder->onGrid(centerLoc2D)) 
	      {
		
		// Get the tile
		Array<Complex>* dataPtr=getDataPointer(centerLoc2D, true);
		gridder->setOffset(IPosition(2, offsetLoc(0), offsetLoc(1)));
		Int aNx=dataPtr->shape()(0);
		Int aNy=dataPtr->shape()(1);
		
		// Now use FORTRAN to do the gridding. Remember to 
		// ensure that the shape and offsets of the tile are 
		// accounted for.
		
		Vector<Double> actualOffset(3);
		for (Int i=0;i<2;i++) 
		  actualOffset(i)=uvOffset(i)-Double(offsetLoc(i));
		
		actualOffset(2)=uvOffset(2);
		IPosition s(vb.modelVisCube().shape());
		
		Int Conj=0,doGrad=0,ScanNo=0;
		Double area=1.0;
		
		runFortranGet(uvw,dphase,vb.modelVisCube(),s,Conj,flags,rowFlags,rownr,
			      actualOffset,dataPtr,aNx,aNy,npol,nchan,vb,NAnt,ScanNo,sigma,
			      l_offsets,m_offsets,area,doGrad,PAIndex+1);
	      }
	  }
      }
    else 
      {
	
	IPosition s(vb.modelVisCube().shape());
	Int Conj=0,doGrad=0,ScanNo=0;
	Double area=1.0;
	
	runFortranGet(uvw,dphase,vb.modelVisCube(),s,Conj,flags,rowFlags,row,
		      uvOffset,&griddedData,nx,ny,npol,nchan,vb,NAnt,ScanNo,sigma,
		      l_offsets,m_offsets,area,doGrad,PAIndex+1);
	/*
	static int junk=0;
	if (junk==4)
	  {
	    cout << "Time = " << vb.time()/1e9 << endl;
	  for(Int i=0;i<vb.modelVisCube().shape()(2);i++)
	    cout << "PBWP: Residual: " << i 
		 << " " << vb.modelVisCube()(0,0,i) 
		 << " " << vb.modelVisCube()(3,0,i)
		 << " " << vb.visCube()(0,0,i) 
		 << " " << vb.visCube()(3,0,i)
		 << " " << vb.flag()(0,i) 
		 << " " << vb.antenna1()(i)<< "-" << vb.antenna2()(i) 
		 << " " << vb.flagRow()(i) 
		 << " " << vb.flagCube()(0,0,i) 
		 << " " << vb.flagCube()(3,0,i) 
		 << endl;
	  }
	junk++;
	*/
      }
  }
  //
  //---------------------------------------------------------------
  //
  void nPBWProjectFT::get(VisBuffer& vb, Cube<Complex>& modelVis, 
                          Array<Complex>& griddedVis, Vector<Double>& /*scale*/,
			 Int row)
  {
    Int nX=griddedVis.shape()(0);
    Int nY=griddedVis.shape()(1);
    Vector<Double> offset(2);
    offset(0)=Double(nX)/2.0;
    offset(1)=Double(nY)/2.0;
    // If row is -1 then we pass through all rows
    Int startRow, endRow, nRow;
    if (row==-1) 
      {
	nRow=vb.nRow();
	startRow=0;
	endRow=nRow-1;
	modelVis.set(Complex(0.0,0.0));
      } 
    else 
      {
	nRow=1;
	startRow=row;
	endRow=row;
	modelVis.xyPlane(row)=Complex(0.0,0.0);
      }
    
    Int NAnt=0;
    
    if (doPointing) 
      NAnt = findPointingOffsets(vb,l_offsets, m_offsets,true);
    
    
    //  
    // Get the uvws in a form that Fortran can use
    //
    Matrix<Double> uvw(3, vb.uvw().nelements());
    uvw=0.0;
    Vector<Double> dphase(vb.uvw().nelements());
    dphase=0.0;
    //
    //NEGATING to correct for an image inversion problem
    //
    for (Int i=startRow;i<=endRow;i++) 
      {
	for (Int idim=0;idim<2;idim++) uvw(idim,i)=-vb.uvw()(i)(idim);
	uvw(2,i)=vb.uvw()(i)(2);
      }
    
    rotateUVW(uvw, dphase, vb);
    refocus(uvw, vb.antenna1(), vb.antenna2(), dphase, vb);
    
    // This is the convention for dphase
    dphase*=-1.0;
    
    Cube<Int> flags(vb.flagCube().shape());
    flags=0;
    flags(vb.flagCube())=true;
    
    //Check if ms has changed then cache new spw and chan selection
    if(vb.newMS())
      matchAllSpwChans(vb);
    
    //Channel matching for the actual spectral window of buffer
    if(doConversion_p[vb.spectralWindow()])
      matchChannel(vb.spectralWindow(), vb);
    else
      {
	chanMap.resize();
	chanMap=multiChanMap_p[vb.spectralWindow()];
      }
    
    Vector<Int> rowFlags(vb.nRow());
    rowFlags=0;
    rowFlags(vb.flagRow())=true;
    if(!usezero_p) 
      for (Int rownr=startRow; rownr<=endRow; rownr++) 
	if(vb.antenna1()(rownr)==vb.antenna2()(rownr)) rowFlags(rownr)=1;
    
    IPosition s(modelVis.shape());
    Int Conj=0,doGrad=0,ScanNo=0;
    Double area=1.0;
    
    runFortranGet(uvw,dphase,vb.modelVisCube(),s,Conj,flags,rowFlags,row,
		  offset,&griddedVis,nx,ny,npol,nchan,vb,NAnt,ScanNo,sigma,
		  l_offsets,m_offsets,area,doGrad,PAIndex+1);
  }
  //
  //---------------------------------------------------------------
  //
  // Finalize the FFT to the Sky. Here we actually do the FFT and
  // return the resulting image
  ImageInterface<Complex>& nPBWProjectFT::getImage(Matrix<Float>& weights,
						  Bool normalize) 
  {
    //AlwaysAssert(lattice, AipsError);
    AlwaysAssert(image, AipsError);
    
    logIO() << "#########getimage########" << LogIO::DEBUGGING << LogIO::POST;
    
    logIO() << LogOrigin("nPBWProjectFT", "getImage") << LogIO::NORMAL;
    
    weights.resize(sumWeight.shape());
    
    convertArray(weights, sumWeight);
    //  
    // If the weights are all zero then we cannot normalize otherwise
    // we don't care.
    //
    if(max(weights)==0.0) 
      {
	if(normalize) logIO() << LogIO::SEVERE
			      << "No useful data in nPBWProjectFT: weights all zero"
			      << LogIO::POST;
	else logIO() << LogIO::WARN << "No useful data in nPBWProjectFT: weights all zero"
		     << LogIO::POST;
      }
    else
      {
	const IPosition latticeShape = lattice->shape();
	
	logIO() << LogIO::DEBUGGING
		<< "Starting FFT and scaling of image" << LogIO::POST;
	//    
	// x and y transforms (lattice has the gridded vis.  Make the
	// dirty images)
	//
	LatticeFFT::cfft2d(*lattice,false);
	
	//
	// Apply the gridding correction
	//    
	{
	  normalizeAvgPB();
	  Int inx = lattice->shape()(0);
	  Int iny = lattice->shape()(1);
	  Vector<Complex> correction(inx);
	  
	  Vector<Float> sincConv(nx);
	  Float centerX=nx/2;
	  for (Int ix=0;ix<nx;ix++) 
	    {
	      Float x=C::pi*Float(ix-centerX)/(Float(nx)*Float(convSampling));
	      if(ix==centerX) sincConv(ix)=1.0;
	      else 	    sincConv(ix)=sin(x)/x;
	    }
	  
	  IPosition cursorShape(4, inx, 1, 1, 1);
	  IPosition axisPath(4, 0, 1, 2, 3);
	  LatticeStepper lsx(lattice->shape(), cursorShape, axisPath);
	  LatticeIterator<Complex> lix(*lattice, lsx);
	  
	  LatticeStepper lavgpb(avgPB.shape(),cursorShape,axisPath);
	  LatticeIterator<Float> liavgpb(avgPB, lavgpb);
	  
	  for(lix.reset(),liavgpb.reset();
	      !lix.atEnd();
	      lix++,liavgpb++) 
	    {
	      Int pol=lix.position()(2);
	      Int chan=lix.position()(3);
	      
	      if(weights(pol, chan)>0.0) 
		{
		  /*
		  Int iy=lix.position()(1);
		  gridder->correctX1D(correction,iy);
		  
		  Vector<Float> PBCorrection(liavgpb.rwVectorCursor().shape()),
		    avgPBVec(liavgpb.rwVectorCursor().shape());
		  
		  PBCorrection = liavgpb.rwVectorCursor();
		  avgPBVec = liavgpb.rwVectorCursor();

		  for(int i=0;i<PBCorrection.shape();i++)
		    {
		      //
		      // This with the PS functions
		      //
		      // PBCorrection(i)=FUNC(avgPBVec(i))*sincConv(i)*sincConv(iy);
		      // if ((abs(PBCorrection(i)*correction(i))) >= pbLimit_p)
		      // 	lix.rwVectorCursor()(i) /= PBCorrection(i)*correction(i);
 		      // else if (!makingPSF)
 		      // 	lix.rwVectorCursor()(i) /= correction(i)*sincConv(i)*sincConv(iy);
		      //
		      // This without the PS functions
		      //
 		      PBCorrection(i)=FUNC(avgPBVec(i))*sincConv(i)*sincConv(iy);
 		      if ((abs(PBCorrection(i))) >= pbLimit_p)
		      	lix.rwVectorCursor()(i) /= PBCorrection(i);
	 	      else if (!makingPSF)
 		      	lix.rwVectorCursor()(i) /= sincConv(i)*sincConv(iy);
		    }
		  */
		  if(normalize) 
		    {
		      Complex rnorm(Float(inx)*Float(iny)/weights(pol,chan));
		      lix.rwCursor()*=rnorm;
		    }
		  else 
		    {
		      Complex rnorm(Float(inx)*Float(iny));
		      lix.rwCursor()*=rnorm;
		    }
		}
	      else 
		lix.woCursor()=0.0;
	    }
	}


	if(!isTiled) 
	  {
	    //
	    // Check the section from the image BEFORE converting to a lattice 
	    //
	    IPosition blc(4, (nx-image->shape()(0)+(nx%2==0))/2,
			  (ny-image->shape()(1)+(ny%2==0))/2, 0, 0);
	    IPosition stride(4, 1);
	    IPosition trc(blc+image->shape()-stride);
	    //
	    // Do the copy
	    //
	    image->put(griddedData(blc, trc));
	    griddedData.resize(IPosition(1,0));
	  }
      }
    
    return *image;
  }
  //
  //---------------------------------------------------------------
  //
  // Get weight image
  void nPBWProjectFT::getWeightImage(ImageInterface<Float>& weightImage,
				    Matrix<Float>& weights) 
  {
    
    logIO() << LogOrigin("nPBWProjectFT", "getWeightImage") << LogIO::NORMAL;
    
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
    LatticeIterator<Float> liy(avgPB,lsx);
    for(lix.reset();!lix.atEnd();lix++) 
      {
	// UNUSED  Int pol=lix.position()(2);
	// UNUSED  Int chan=lix.position()(3);
	//	lix.rwCursor()=weights(pol,chan);
	lix.rwCursor()=liy.rwCursor();
      }
  }
  //
  //---------------------------------------------------------------
  //
Bool nPBWProjectFT::toRecord(String&, //error,
			     RecordInterface& outRec, Bool withImage, const String diskimage) 
{    
    // Save the current nPBWProjectFT object to an output state record
    Bool retval = true;
    Double cacheVal=(Double) cachesize;
    outRec.define("cache", cacheVal);
    outRec.define("tile", tilesize);
    
    Vector<Double> phaseValue(2);
    String phaseUnit;
    phaseValue=mTangent_p.getAngle().getValue();
    phaseUnit= mTangent_p.getAngle().getUnit();
    outRec.define("phasevalue", phaseValue);
    outRec.define("phaseunit", phaseUnit);
    
    Vector<Double> dirValue(3);
    String dirUnit;
    dirValue=mLocation_p.get("m").getValue();
    dirUnit=mLocation_p.get("m").getUnit();
    outRec.define("dirvalue", dirValue);
    outRec.define("dirunit", dirUnit);
    
    outRec.define("padding", padding_p);
    outRec.define("maxdataval", maxAbsData);
    
    Vector<Int> center_loc(4), offset_loc(4);
    for (Int k=0; k<4 ; k++)
      {
	center_loc(k)=centerLoc(k);
	offset_loc(k)=offsetLoc(k);
      }
    outRec.define("centerloc", center_loc);
    outRec.define("offsetloc", offset_loc);
    outRec.define("sumofweights", sumWeight);
    if(withImage && image ){
      if(diskimage==""){ 
	ImageInterface<Complex>& tempimage(*image);
	Record imageContainer;
	String error;
	retval = (retval || tempimage.toRecord(error, imageContainer));
	outRec.defineRecord("image", imageContainer);
      }
      else{
	PagedImage<Complex> tempImage(image->shape(), image->coordinates(), diskimage);
	tempImage.copyData(*image);
	outRec.define("diskimage", diskimage);
      }
    }
    
    return retval;
    
  //
}
  //---------------------------------------------------------------
  //
Bool nPBWProjectFT::fromRecord(String&, //error,
			       const RecordInterface& inRec)
{
    Bool retval = true;
    imageCache=0; lattice=0; arrayLattice=0;
    Double cacheVal;
    inRec.get("cache", cacheVal);
    cachesize=(Long) cacheVal;
    inRec.get("tile", tilesize);
    
    Vector<Double> phaseValue(2);
    inRec.get("phasevalue",phaseValue);
    String phaseUnit;
    inRec.get("phaseunit",phaseUnit);
    Quantity val1(phaseValue(0), phaseUnit);
    Quantity val2(phaseValue(1), phaseUnit); 
    MDirection phasecenter(val1, val2);
    
    mTangent_p=phasecenter;
    // This should be passed down too but the tangent plane is 
    // expected to be specified in all meaningful cases.
    tangentSpecified_p=true;  
    Vector<Double> dirValue(3);
    String dirUnit;
    inRec.get("dirvalue", dirValue);
    inRec.get("dirunit", dirUnit);
    MVPosition dummyMVPos(dirValue(0), dirValue(1), dirValue(2));
    MPosition mLocation(dummyMVPos, MPosition::ITRF);
    mLocation_p=mLocation;
    
    inRec.get("padding", padding_p);
    inRec.get("maxdataval", maxAbsData);
    
    Vector<Int> center_loc(4), offset_loc(4);
    inRec.get("centerloc", center_loc);
    inRec.get("offsetloc", offset_loc);
    uInt ndim4 = 4;
    centerLoc=IPosition(ndim4, center_loc(0), center_loc(1), center_loc(2), 
			center_loc(3));
    offsetLoc=IPosition(ndim4, offset_loc(0), offset_loc(1), offset_loc(2), 
			offset_loc(3));
    inRec.get("sumofweights", sumWeight);
    if(inRec.isDefined("image"))
      {
	Record imageAsRec=inRec.asRecord("image");
	if(!image) image= new TempImage<Complex>(); 
	
	String error;
	retval = (retval || image->fromRecord(error, imageAsRec));    
	
	// Might be changing the shape of sumWeight
	init(); 
	
	if(isTiled) 
    	  lattice=CountedPtr<Lattice<Complex> > (image, false);
	else 
	  {
	    //
	    // Make the grid the correct shape and turn it into an
	    // array lattice Check the section from the image BEFORE
	    // converting to a lattice
	    //
	    IPosition gridShape(4, nx, ny, npol, nchan);
	    griddedData.resize(gridShape);
	    griddedData=Complex(0.0);
	    IPosition blc(4, (nx-image->shape()(0)+(nx%2==0))/2,
			  (ny-image->shape()(1)+(ny%2==0))/2, 0, 0);
	    IPosition start(4, 0);
	    IPosition stride(4, 1);
	    IPosition trc(blc+image->shape()-stride);
	    griddedData(blc, trc) = image->getSlice(start, image->shape());
	    
	    arrayLattice = new ArrayLattice<Complex>(griddedData);
	    lattice=arrayLattice;
	  }
	
	AlwaysAssert(image, AipsError);
      }
    else if(inRec.isDefined("diskimage")){
	String diskim;
	inRec.get("diskimage", diskim);
	image=new PagedImage<Complex>(diskim);
    }
    
    
    return retval;
  }
  //
  //---------------------------------------------------------------
  //
  void nPBWProjectFT::ok() 
  {
    AlwaysAssert(image, AipsError);
  }
  //----------------------------------------------------------------------
  //
  // Make a plain straightforward honest-to-god image. This returns a
  // complex image, without conversion to Stokes. The polarization
  // representation is that required for the visibilities.
  //
  void nPBWProjectFT::makeImage(FTMachine::Type type, 
			       VisSet& vs,
			       ImageInterface<Complex>& theImage,
			       Matrix<Float>& weight) 
  {
    logIO() << LogOrigin("nPBWProjectFT", "makeImage") << LogIO::NORMAL;
    
    if(type==FTMachine::COVERAGE) 
      logIO() << "Type COVERAGE not defined for Fourier transforms"
	      << LogIO::EXCEPTION;
    
    
    // Initialize the gradients
    ROVisIter& vi(vs.iter());
    
    // Loop over all visibilities and pixels
    VisBuffer vb(vi);
    
    // Initialize put (i.e. transform to Sky) for this model
    vi.origin();
    
    if(vb.polFrame()==MSIter::Linear) 
      StokesImageUtil::changeCStokesRep(theImage, StokesImageUtil::LINEAR);
    else 
      StokesImageUtil::changeCStokesRep(theImage, StokesImageUtil::CIRCULAR);
    
    initializeToSky(theImage,weight,vb);

    //    
    // Loop over the visibilities, putting VisBuffers
    //
    paChangeDetector.reset();

    for (vi.originChunks();vi.moreChunks();vi.nextChunk()) 
      {
	for (vi.origin(); vi.more(); vi++) 
	  {
	    if (type==FTMachine::PSF) makingPSF=true;
	    findConvFunction(theImage,vb);
	    
	    switch(type) 
	      {
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
		makingPSF = true;
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
  
  void nPBWProjectFT::setPAIncrement(const Quantity& paIncrement)
  {
    paChangeDetector.setTolerance(paIncrement);
    rotateAperture_p = true;
    if (paIncrement.getValue("rad") < 0)
      rotateAperture_p = false;
    logIO() << "Setting PA increment to " << paIncrement.getValue("deg") << " deg" << endl;
  }

  Bool nPBWProjectFT::verifyShapes(IPosition pbShape, IPosition skyShape)
  {
    if ((pbShape(0) != skyShape(0)) && // X-axis
	(pbShape(1) != skyShape(1)) && // Y-axis
	(pbShape(2) != skyShape(2)))   // Poln-axis
      {
	throw(AipsError("Sky and/or polarization shape of the avgPB "
			"and the sky model do not match."));
	return false;
      }
    return true;
    
  }
  //
  // The functions are not azimuthally symmetric - so compute the
  // support size carefully.  Collect every PixInc pixel along a
  // quarter circle of radius R (four-fold azimuthal symmetry is
  // assumed), and check if any pixel in this list is above the
  // threshold. If TRUE, then R is the support radius.  Else decrease
  // R and check again.
  //
  //
  Bool nPBWProjectFT::findSupport(Array<Complex>& func, Float& threshold,Int& origin, Int& R)
  {
    Double NSteps;
    Int PixInc=1;
    Vector<Complex> vals;
    IPosition ndx(4,origin,0,0,0);
    Bool found=false;
    IPosition cfShape=func.shape();
    for(R=convSize/4;R>1;R--)
      {
	NSteps = 90*R/PixInc; //Check every PixInc pixel along a
	                      //circle of radious R
	vals.resize((Int)(NSteps+0.5));
	vals=0;
	for(Int th=0;th<NSteps;th++)
	  {
	    ndx(0)=(int)(origin + R*sin(2.0*M_PI*th*PixInc/R));
	    ndx(1)=(int)(origin + R*cos(2.0*M_PI*th*PixInc/R));
	    
	    if ((ndx(0) < cfShape(0)) && (ndx(1) < cfShape(1)))
	      vals(th)=convFunc(ndx);
	  }
	if (max(abs(vals)) > threshold)
	  {found=true;break;}
      }
    return found;
  }
  /*
  void nPBWProjectFT::makeAveragePB(const VisBuffer& vb, 
				   const ImageInterface<Complex>& image,
				   Int& polInUse,
				   //TempImage<Float>& thesquintPB,
				   TempImage<Float>& theavgPB)
  {
    if (!resetPBs) return;

    Vector<Float> paList;
    //
    // Compute a list of PA present in the data with an increment of 10deg
    // for PA-averaging of the average PB
    //
    {
      Record time;
      MSRange msr(*ms_p);
      time = msr.range(MSS::TIMES);
      Vector<Double> times=time.asArrayDouble("times");

      Float pa0=vb.feed_pa(times[0])[0],pa1, dpa;
      paList.resize(1);

      paList[0]=pa0;

      for(uInt ipa=0;ipa<times.nelements();ipa++) 
	{
	  pa1=vb.feed_pa(times[ipa])[0];
	  dpa = abs(pa1-pa0);
	  if (dpa >= 10.0/57.2956)
	    {
	      pa0=pa1;
	      paList.resize(paList.nelements()+1,true);
	      paList[paList.nelements()-1] = pa0;
	    }
	}
//        paList.resize(1);
//        paList[0]=pa0;
    }
    TempImage<Float> localPB;
    localPB.setMaximumCacheSize(cachesize);
    logIO() << LogOrigin("nPBWProjecFT","makeAveragePB")
	    << LogIO::NORMAL;
    
    localPB.resize(image.shape()); localPB.setCoordinateInfo(image.coordinates());
    //
    // If this is the first time, resize the average PB
    //
    if (resetPBs)
      {
	logIO() << LogOrigin("nPBWProjectFT", "makeAveragePB")  
		<< "Computing the PA-averaged PBs.  Averaging over "
		<< paList.nelements() << " PA values."
		<< LogIO::NORMAL
		<< LogIO::POST;
	theavgPB.resize(localPB.shape()); 
	theavgPB.setCoordinateInfo(localPB.coordinates());
	theavgPB.set(0.0);
	noOfPASteps = 0;
	pbPeaks.resize(theavgPB.shape()(2));
	pbPeaks.set(0.0);
	resetPBs=false;
      }
    ProgressMeter pm(1.0, Double(paList.nelements()),
		     "Making average PB",
		     "", "", "", true);
    //
    // Make the Stokes PB
    //
    for(Int iPA=0;iPA<paList.nelements();iPA++)
      {
	localPB.set(1.0);
	//   vpSJ->applySquare(localPB,localPB,vb,1);  // This is Stokes PB (not squinted)
	{
	  VLACalcIlluminationConvFunc vlaPB;
	  vlaPB.setMaximumCacheSize(cachesize);
	  Vector<Float> pa(1);
	  pa[0] = paList(iPA);
	  //	  vlaPB.applyPB(localPB, vb, paList, bandID_p);
	  if (bandID_p == -1) bandID_p=getVisParams(vb);
	  vlaPB.applyPB(localPB, vb, pa, bandID_p);
	}
	
// 	IPosition twoDPBShape(localPB.shape());
 	IPosition twoDPBShape(localPB.shape());
// 	TempImage<Complex> localTwoDPB(twoDPBShape,localPB.coordinates());
// 	localTwoDPB.setMaximumCacheSize(cachesize);
    
	//   Array<Float> raoff,decoff;
	Float peak=0;
	Int NAnt;
	//    NAnt=findPointingOffsets(vb,l_offsets,m_offsets,true);
	//     if (doPointing)
	//       NAnt=findPointingOffsets(vb,l_offsets,m_offsets,false);
	//     else
	//       NAnt = 1;
    
// 	logIO() << LogOrigin("nPBWProjectFT", "makeAveragePB")  
// 		<< "Updating average PB @ PA = " << paList(iPA)*57.2956 << "deg"
// 		<< LogIO::NORMAL
// 		<< LogIO::POST;
	
	noOfPASteps++;
	if (!doPointing) NAnt=1;
	//
	// For now, disable the "jittering" of the avg. PB by antenna pointing offsets.
	// May not be required even for the very deep imaging - but thinking is a bit 
	// too clouded right now to delete the code below (SB)
	//
	NAnt=1; 
	
	for(Int ant=0;ant<NAnt;ant++)
	  { //Ant loop
// 	    {
// 	      IPosition ndx(4,0,0,0,0);
// 	      for(ndx(0)=0; ndx(0)<twoDPBShape(0); ndx(0)++)
// 		for(ndx(1)=0; ndx(1)<twoDPBShape(1); ndx(1)++)
// 		  //	     for(ndx(2)=0; ndx(2)<polInUse; ndx(2)++)
// 		  for(ndx(2)=0; ndx(2)<twoDPBShape(2); ndx(2)++)
// 		    localTwoDPB.putAt(Complex((localPB(ndx)),0.0),ndx);
// 	    }
	    //
	    // If antenna pointing errors are not applied, no shifting
	    // (which can be expensive) is required.
	    //
	    //
	    // Accumulate the shifted PBs
	    //
	    {
	      Bool isRefF,isRefC;
	      Array<Float> fbuf,cbuf;
	      //	      Array<Complex> cbuf;
	      isRefF=theavgPB.get(fbuf);
	      //	      isRefC=localTwoDPB.get(cbuf);
	      isRefC=localPB.get(cbuf);
	      
	      //	      Vector<Float> tmpPeaks(pbPeaks.nelements());
	      IPosition cs(cbuf.shape()),fs(fbuf.shape()),peakLoc(4,0,0,0,0);;
	      //	 if (makingPSF)
	      {
		IPosition ndx(4,0,0,0,0),avgNDX(4,0,0,0,0);
		for(ndx(2)=0,avgNDX(2)=0;ndx(2)<twoDPBShape(2);ndx(2)++,avgNDX(2)++)
		  {
		    for(ndx(0)=0,avgNDX(0)=0;ndx(0)<fs(0);ndx(0)++,avgNDX(0)++)
		      for(ndx(1)=0,avgNDX(1)=0;ndx(1)<fs(1);ndx(1)++,avgNDX(1)++)
			{
			  Float val;
			  val = real(cbuf(ndx));
			  fbuf(avgNDX) += val;
			  if (fbuf(avgNDX) > peak) peak=fbuf(avgNDX);
			}
		  }
	      }
	      if (!isRefF) theavgPB.put(fbuf);
	      //	      pbPeaks += tmpPeaks;
	      //	      cout << "Current PB peak = " << peak << endl;
	    }
	  }
	pm.update(Double(iPA+1));
      }
  }
  */

  void nPBWProjectFT::makeAntiAliasingOp(Vector<Complex>& op, const Int nx_l)
  {
    MathFunc<Float> sf(SPHEROIDAL);
    if (op.nelements() != (uInt)nx_l)
      {
	op.resize(nx_l);
	Int inner=nx_l/2, center=nx_l/2;
	//
	// The "complicated" equation below is worthy of a comment (as
	// notes are called in program text).
        //
	// uvScale/nx == Size of a pixel size in the image in radians.
	// Lets call it dx.  HPBW is the HPBW for the antenna at the
	// centre freq. in use.  HPBW/dx == Pixel where the PB will be
	// ~0.5x its peak value.  ((2*N*HPBW)/dx) == the pixel where
	// the N th. PB sidelobe will be (rougly speaking).  When this
	// value is equal to 3.0, the Spheroidal implemtation goes to
	// zero!
	//
	Float dx=uvScale(0)*convSampling/nx;
	Float MaxSideLobeNum = 3.0;
	Float S=1.0*dx/(MaxSideLobeNum*2*HPBW),cfScale;
	
	cout << "UVSCALE = " << uvScale(0) << " " << convSampling << endl;
	cout << "HPBW = " << HPBW 
	     << " " << Diameter_p 
	     << " " << uvScale(0)/nx 
	     << " " << S 
	     << " " << dx 
	     << endl;
	

	cfScale=S=6.0/inner;
	for(Int ix=-inner;ix<inner;ix++)	    op(ix+center)=sf.value(abs((ix)*cfScale));
	// for(Int ix=-inner;ix<inner;ix++)
	//   if (abs(op(ix+center)) > 1e-8) 
	// 	    cout << "SF: " << ix 
	// 		 << " " << (ix)*cfScale 
	// 		 << " " << real(op(ix+center)) 
	// 		 << " " << imag(op(ix+center))
	// 		 << endl;
      }
  }

  void nPBWProjectFT::makeAntiAliasingCorrection(Vector<Complex>& correction, 
						 const Vector<Complex>& op,
						 const Int nx_l)
  {
    if (correction.nelements() != (uInt)nx_l)
      {
	correction.resize(nx_l);
	correction=0.0;
	Int opLen=op.nelements(), orig=nx_l/2;
	for(Int i=-opLen;i<opLen;i++)
	  {
	    correction(i+orig) += op(abs(i));
	  }
	ArrayLattice<Complex> tmp(correction);
	LatticeFFT::cfft(tmp,false);
	correction=tmp.get();
      }
//     for(uInt i=0;i<correction.nelements();i++)
//       cout << "FTSF: " << real(correction(i)) << " " << imag(correction(i)) << endl;
  }

  void nPBWProjectFT::correctAntiAliasing(Lattice<Complex>& image)
  {
    //  applyAntiAliasingOp(cf,2);
    IPosition shape(image.shape());
    IPosition ndx(shape);
    ndx=0;
    makeAntiAliasingCorrection(antiAliasingCorrection, 
			       antiAliasingOp,shape(0));

    Complex tmp,val;
    for(Int i=0;i<polInUse;i++)
      {
	ndx(2)=i;
	for (Int iy=0;iy<shape(1);iy++) 
	  {
	    for (Int ix=0;ix<shape(0);ix++) 
	      {
		ndx(0)=ix;
		ndx(1)=iy;
		tmp = image.getAt(ndx);
		val=(antiAliasingCorrection(ix)*antiAliasingCorrection(iy));
		if (abs(val) > 1e-5) tmp = tmp/val; else tmp=0.0;
		image.putAt(tmp,ndx);
	      }
	  }
      }
  }

  void nPBWProjectFT::applyAntiAliasingOp(ImageInterface<Complex>& cf, 
					  Vector<IPosition>& maxPos,
					  Int op, 
					  Bool Square)
  {
    //
    // First the spheroidal function
    //
    IPosition shape(cf.shape());
    IPosition ndx(shape);
    Vector<Double> refPixel=cf.coordinates().referencePixel();
    ndx=0;
    Int nx_l=shape(0),posX,posY;
    makeAntiAliasingOp(antiAliasingOp, nx_l);

    Complex tmp,gain;

    for(Int i=0;i<polInUse;i++)
      {
	ndx(2)=i;
	for (Int iy=-nx_l/2;iy<nx_l/2;iy++) 
	  {
	    for (Int ix=-nx_l/2;ix<nx_l/2;ix++) 
	      {
		ndx(0)=ix+nx_l/2;
		ndx(1)=iy+nx_l/2;
		tmp = cf.getAt(ndx);
		posX=ndx(0)+(Int)(refPixel(0)-maxPos(i)(0))+1;
		posY=ndx(1)+(Int)(refPixel(1)-maxPos(i)(1))+1;
		if ((posX > 0) && (posX < nx_l) &&
		    (posY > 0) && (posY < nx_l))
		  gain = antiAliasingOp(posX)*antiAliasingOp(posY);
		else
		  if (op==2) gain = 1.0; else gain=0.0;
		if (Square) gain *= gain;
		switch (op)
		  {
		  case 0: tmp = tmp+gain;break;
		  case 1: 
		    {
		      tmp = tmp*gain;
		      break;
		    }
		  case 2: tmp = tmp/gain;break;
		  }
		cf.putAt(tmp,ndx);
	      }
	  }
      }
  };
  void nPBWProjectFT::applyAntiAliasingOp(ImageInterface<Float>& cf, 
					  Vector<IPosition>& maxPos,
					  Int op, 
					  Bool Square)
  {
    //
    // First the spheroidal function
    //
    IPosition shape(cf.shape());
    IPosition ndx(shape);
    Vector<Double> refPixel=cf.coordinates().referencePixel();
    ndx=0;
    Int nx_l=shape(0),posX,posY;
    makeAntiAliasingOp(antiAliasingOp, nx_l);

    Float tmp,gain;

    for(Int i=0;i<polInUse;i++)
      {
	ndx(2)=i;
	for (Int iy=-nx_l/2;iy<nx_l/2;iy++) 
	  {
	    for (Int ix=-nx_l/2;ix<nx_l/2;ix++) 
	      {
		ndx(0)=ix+nx_l/2;
		ndx(1)=iy+nx_l/2;
		tmp = cf.getAt(ndx);
		posX=ndx(0)+(Int)(refPixel(0)-maxPos(i)(0))+1;
		posY=ndx(1)+(Int)(refPixel(1)-maxPos(i)(1))+1;
		if ((posX > 0) && (posX < nx_l) &&
		    (posY > 0) && (posY < nx_l))
		  gain = real(antiAliasingOp(posX)*antiAliasingOp(posY));
		else
		  if (op==2) gain = 1.0; else gain=0.0;
		if (Square) gain *= gain;
		switch (op)
		  {
		  case 0: tmp = tmp+gain;break;
		  case 1: 
		    {
		      tmp = tmp*gain;
		      break;
		    }
		  case 2: tmp = tmp/gain;break;
		  }
		cf.putAt(tmp,ndx);
	      }
	  }
      }
  };
} //# NAMESPACE CASA - END

