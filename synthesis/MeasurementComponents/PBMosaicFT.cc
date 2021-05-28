// -*- C++ -*-
//# PBMosaicFT.cc: Implementation of PBMosaicFT class
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
#include <casa/Quanta/UnitMap.h>
#include <casa/Quanta/MVTime.h>
#include <casa/Quanta/UnitVal.h>
#include <measures/Measures/Stokes.h>
#include <coordinates/Coordinates/CoordinateSystem.h>
#include <coordinates/Coordinates/DirectionCoordinate.h>
#include <coordinates/Coordinates/SpectralCoordinate.h>
#include <coordinates/Coordinates/StokesCoordinate.h>
#include <coordinates/Coordinates/Projection.h>
#include <ms/MeasurementSets/MSColumns.h>
#include <ms/MeasurementSets/MSRange.h>
#include <ms/MSSel/MSSelection.h>
#include <casa/BasicSL/Constants.h>
#include <scimath/Mathematics/FFTServer.h>
#include <synthesis/MeasurementComponents/PBMosaicFT.h>
#include <scimath/Mathematics/RigidVector.h>
#include <msvis/MSVis/StokesVector.h>
#include <synthesis/TransformMachines/StokesImageUtil.h>
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
#include <images/Regions/WCBox.h>
#include <images/Images/SubImage.h>
#include <images/Regions/ImageRegion.h>
#include <images/Images/ImageSummary.h>
#include <casa/BasicMath/Random.h>

#include <synthesis/MeasurementComponents/PBMosaicFT.h>
//#include <synthesis/MeasurementComponents/GlobalFTMachineCallbacks.h>
#include <casa/System/ProgressMeter.h>

#define DELTAPA 1.0
#define MAGICPAVALUE -999.0
#define CONVSIZE (1024*4)
#define OVERSAMPLING 20
#define USETABLES 0           // If equal to 1, use tabulated exp() and
			      // complex exp() functions.
#define MAXPOINTINGERROR 250.0 // Max. pointing error in arcsec used to
// determine the resolution of the
// tabulated exp() function.
using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN
  //
  //---------------------------------------------------------------
  //
#define FUNC(a)  (sqrt((a)))
  PBMosaicFT::PBMosaicFT(MeasurementSet& /*ms*/, 
			 Int nWPlanes, Long icachesize, 
			 String& cfCacheDirName,
			 Bool applyPointingOffset,
			 Bool doPBCorr,
			 Int itilesize, 
			 Float paSteps,
			 Float pbLimit,
			 Bool usezero)
    ////: nPBWProjectFT(ms,nWPlanes,icachesize,cfCacheDirName,applyPointingOffset,doPBCorr,itilesize,paSteps,pbLimit,usezero),
    : nPBWProjectFT(nWPlanes,icachesize,cfCacheDirName,applyPointingOffset,doPBCorr,itilesize,paSteps,pbLimit,usezero),
      fieldIds_p(0)
  {
    //
    // Set the function pointer for FORTRAN call for GCF services.  
    // This is a pure C function pointer which FORTRAN can call.  
    // The function itself uses GCF object services.
    //
    epJ=NULL;  // We do not yet support antenna pointing error
	       // handling in this FTMachine.
    convSize=0;
    tangentSpecified_p=false;
    lastIndex_p=0;
    paChangeDetector.reset();
    pbLimit_p=pbLimit;
    //
    // Get various parameters from the visibilities.  
    //
    //bandID_p = getVisParams();
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
    //use memory size defined in aipsrc if exists
    Long hostRAM = (HostInfo::memoryTotal(true)*1024); // In bytes
    hostRAM = hostRAM/(sizeof(Float)*2); // In complex pixels
    if (cachesize > hostRAM) cachesize=hostRAM;

    // MSFieldColumns msfc(ms.field());
    // MSRange msr(ms);
    // //
    // // An array of shape [2,1,1]!
    // //
    // fieldIds_p = msr.range(MSS::FIELD_ID).asArrayInt(RecordFieldId(0));
    nApertures = 0;
    lastPAUsedForWtImg = MAGICPAVALUE;
  }
  //
  //---------------------------------------------------------------
  //
  PBMosaicFT::PBMosaicFT(const RecordInterface& stateRec)
    : nPBWProjectFT(stateRec)
  {
    // Construct from the input state record
    String error;
    
    if (!fromRecord(error, stateRec)) {
      throw (AipsError("Failed to create PBMosaicFT: " + error));
    };
    //    bandID_p = getVisParams();
    PAIndex = -1;
    maxConvSupport=-1;
    convSampling=OVERSAMPLING;
    convSize=CONVSIZE;
    nApertures = 0;
  }
  //
  //---------------------------------------------------------------
  //
  PBMosaicFT& PBMosaicFT::operator=(const PBMosaicFT& other)
  {
    if(this!=&other) {
      nPBWProjectFT::operator=(other);
      padding_p=other.padding_p;
      //      ms_p=other.ms_p;
      nWPlanes_p=other.nWPlanes_p;
      imageCache=other.imageCache;
      cachesize=other.cachesize;
      tilesize=other.tilesize;
      gridder=other.gridder;
      isTiled=other.isTiled;
      lattice=other.lattice;
      arrayLattice=other.arrayLattice;
      maxAbsData=other.maxAbsData;
      centerLoc=other.centerLoc;
      offsetLoc=other.offsetLoc;
      mspc=other.mspc;
      msac=other.msac;
      pointingToImage=other.pointingToImage;
      usezero_p=other.usezero_p;
      doPBCorrection = other.doPBCorrection;
      maxConvSupport= other.maxConvSupport;
      nApertures = other.nApertures;
    };
    return *this;
  };
  //
  //----------------------------------------------------------------------
  //
//   PBMosaicFT::PBMosaicFT(const PBMosaicFT& other)
//   {
//     operator=(other);
//   }
  //
  //---------------------------------------------------------------
  //
  Int PBMosaicFT::findPointingOffsets(const VisBuffer& vb, 
					Array<Float> &l_off,
					Array<Float> &m_off,
					Bool Evaluate)
  {
    //
    // Get the antenna pointing errors, if any.
    Int NAnt=0;
    //
    // This will return 0 if EPJ Table is not given.  Otherwise will
    // return the number of antennas it detected (from the EPJ table)
    // and the offsets in l_off and m_off.
    //
    NAnt = nPBWProjectFT::findPointingOffsets(vb,l_off,m_off,Evaluate);
    //    NAnt = l_off.shape()(2);
    
    // Resize the offset arrays if no pointing table was given.
    //
    if (NAnt <=0 )
      {
	NAnt=vb.msColumns().antenna().nrow();
	l_off.resize(IPosition(3,1,1,NAnt)); // Poln x NChan x NAnt 
	m_off.resize(IPosition(3,1,1,NAnt)); // Poln x NChan x NAnt 
	l_off = m_off = 0.0; 
      }
    //
    // Add field offsets to the pointing errors.
    //

//     Float dayHr=2*3.141592653589793116;
//     MVDirection ref(directionCoord.referenceValue()(0),
// 		    directionCoord.referenceValue()(1)),
//       vbDir(vb.direction1()(0).getAngle().getValue()(0),
// 	    vb.direction1()(0).getAngle().getValue()(1));
    
//     if (0)
//       {
// 	l_off = l_off - (Float)(directionCoord.referenceValue()(0) -
// 				dayHr-vb.direction1()(0).getAngle().getValue()(0));
// 	m_off = m_off - (Float)(directionCoord.referenceValue()(1) -
// 				vb.direction1()(0).getAngle().getValue()(1));
//       }

    //
    // Convert the direction from image co-ordinate system and the VB
    // to MDirection.  Then convert the MDirection to Quantity so that
    // arithematic operation (subtraction) can be done.  Then use the
    // subtracted Quantity to construct another MDirection and use
    // *it's* getAngle().getValue() to extract the difference in the
    // sky direction (from image co-ordinate system) and the phase
    // center direction of the VB in radians!  
    //
    // If only VisBuffer, and DirectionCoordinate class could return
    // MDirection, and MDirection class had operator-(), the code
    // below could look more readable as:
    //  MDirection diff=vb.mDirection()-directionCoord.mDirection();
    // 
    MDirection vbMDir(vb.direction1()(0)),skyMDir, diff;
    directionCoord.toWorld(skyMDir, directionCoord.referencePixel());
    diff = MDirection(skyMDir.getAngle()-vbMDir.getAngle());
    l_off = l_off - (Float)diff.getAngle().getValue()(0);
    m_off = m_off - (Float)diff.getAngle().getValue()(1);

    static int firstPass=0;
    //    static int fieldID=-1;
    static Vector<Float> offsets0,offsets1;
    if (firstPass==0)
      {
	offsets0.resize(NAnt);
	offsets1.resize(NAnt);
// 	MLCG mlcg((Int)(vb.time()(0)));
// 	Normal nrand(&mlcg,0.0,10.0);
// 	for(Int i=0;i<NAnt;i++) offsets0(i) = (Float)(nrand());
// 	for(Int i=0;i<NAnt;i++) offsets1(i) = (Float)(nrand());
	offsets0 = offsets1 = 0.0;
      }
    for(Int i=0;i<NAnt;i++)
      {
	l_off(IPosition(3,0,0,i)) = l_off(IPosition(3,0,0,i)) + offsets0(i)/2.062642e+05;
	m_off(IPosition(3,0,0,i)) = m_off(IPosition(3,0,0,i)) + offsets1(i)/2.062642e+05;
      }

    //m_off=l_off=0.0;
//     if (fieldID != vb.fieldId())
//       {
// 	fieldID = vb.fieldId();
// 	cout << l_off*2.062642e5 << endl;
// 	cout << m_off*2.062642e5 << endl;
//       }
    if (firstPass==0) 
      {
// 	 cout << (Float)(directionCoord.referenceValue()(0)) << " "
// 	      << (Float)(directionCoord.referenceValue()(1)) << endl;
// 	 cout << vb.direction1()(0).getAngle().getValue()(0) << " "
// 	      << vb.direction1()(0).getAngle().getValue()(1) << endl;
//  	cout << l_off << endl;
//  	cout << m_off << endl;
       }
     firstPass++;
    return NAnt;
  }
  //
  //---------------------------------------------------------------
  //
  void PBMosaicFT::normalizeAvgPB()
  {
    // We accumulated normalized PBs.  So don't normalize the average
    // PB.
    pbNormalized = false;
    
  }
  //
  //---------------------------------------------------------------
  //
  Bool PBMosaicFT::makeAveragePB0(const VisBuffer& /*vb*/, 
				  const ImageInterface<Complex>& image,
				  Int& /*polInUse*/,
				  TempImage<Float>& theavgPB)
  {
    Bool pbMade=false;
    if (!resetPBs) return pbMade;
    //
    // If this is the first time, resize the average PB
    //
    if (resetPBs)
      {
	theavgPB.resize(image.shape()); 
	theavgPB.setCoordinateInfo(image.coordinates());
	theavgPB.set(0.0);
	noOfPASteps = 1;
	pbPeaks.resize(theavgPB.shape()(2));
	pbPeaks.set(0.0);
	resetPBs=false;
	pbNormalized=false;
      }
    return pbMade;
  }
  //
  //--------------------------------------------------------------------------------
  //
  void PBMosaicFT::normalizePB(ImageInterface<Float>& /*pb*/, const Float& /*peakValue*/)
  {
  }
  //
  //---------------------------------------------------------------
  //
  // Finalize the FFT to the Sky. Here we actually do the FFT and
  // return the resulting image
  //
  // A specialization exists here only to not normalize by the avgPB.
  //
  ImageInterface<Complex>& PBMosaicFT::getImage(Matrix<Float>& weights,
						  Bool normalize) 
  {
    //AlwaysAssert(lattice, AipsError);
    AlwaysAssert(image, AipsError);
    
    logIO() << "#########getimage########" << LogIO::DEBUGGING << LogIO::POST;
    
    logIO() << LogOrigin("PBMosaicFT", "getImage") << LogIO::NORMAL;
    
    weights.resize(sumWeight.shape());
    
    convertArray(weights, sumWeight);
    //    cerr << "Sum Wt = " << sumWeight << endl;
    //  
    // If the weights are all zero then we cannot normalize otherwise
    // we don't care.
    //
    if(max(weights)==0.0) 
      {
	if(normalize) logIO() << LogIO::SEVERE
			      << "No useful data in PBMosaicFT: weights all zero"
			      << LogIO::POST;
	else logIO() << LogIO::WARN << "No useful data in PBMosaicFT: weights all zero"
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
	// if (!makingPSF)
	//   {
	//     String name("griddedVis.im");
	//     image->put(griddedData);
	//     storeImg(name,*image);
	//   }
	LatticeFFT::cfft2d(*lattice,false);
	if (!avgPBReady)
	  {
	    avgPBReady=true;
	    avgPB.resize(griddedWeights.shape()); 
	    avgPB.setCoordinateInfo(griddedWeights.coordinates());
	    //	    pbNormalized=true;
	    // {
	    //   String name("cpb.im");
	    //   storeImg(name,griddedWeights);
	    // }
	    makeSensitivityImage(griddedWeights, avgPB, weights, true);
	    //
	    // For effeciency reasons, weight functions are
	    // accumulated only once per channel and polarization per
	    // VB (i.e. for a given VB, if the weight functions are
	    // accumulated for every channel and polarization, they
	    // are not accumulated for the rest of VB).  This makes
	    // the sum of weights for wegith functions different from
	    // sum of weights for the visibility data and the
	    // normalzation in makeSensitivityImage() will be
	    // incorrect.  For now, normalize the peak of the average
	    // weight function (wavgPB) to 1.0.
	    //
	    pbNormalized=false;
	    nPBWProjectFT::normalizeAvgPB();
	    resetPBs=false;
	    cfCache.finalize(avgPB);
	  }
	
	//
	// Apply the gridding correction
	//    
	{
	  //	  normalizeAvgPB();
	  Int inx = lattice->shape()(0);
	  Int iny = lattice->shape()(1);
	  Vector<Complex> correction(inx);
	  
	  Vector<Float> sincConv(nx);
	  Float centerX=nx/2+1;
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
	  Float peakAvgPB = max(avgPB.get());
	  for(lix.reset(),liavgpb.reset();
	      !lix.atEnd();
	      lix++,liavgpb++) 
	    {
	      Int pol=lix.position()(2);
	      Int chan=lix.position()(3);
	      //	      if(weights(pol, chan)>0.0) 
	      {
		Int iy=lix.position()(1);
		gridder->correctX1D(correction,iy);
		  
		Vector<Float> PBCorrection(liavgpb.rwVectorCursor().shape()),
		  avgPBVec(liavgpb.rwVectorCursor().shape());
		  
		PBCorrection = liavgpb.rwVectorCursor();
		avgPBVec = liavgpb.rwVectorCursor();
		  
		sincConv=1.0;
		if (!doPBCorrection) 		  avgPBVec=1.0;
		//		avgPBVec=1.0;
		for(int i=0;i<PBCorrection.shape();i++)
		  {
		    //
		    // This with PS functions
		    //
		    // 		      PBCorrection(i)=pbFunc(avgPBVec(i))*sincConv(i)*sincConv(iy);
		    // 		      //		      PBCorrection(i)=(avgPBVec(i))*sincConv(i)*sincConv(iy);
		    // 		      if ((abs(PBCorrection(i)*correction(i))) >= pbLimit_p)
		    // 			lix.rwVectorCursor()(i) /= PBCorrection(i)*correction(i); //*pbNorm
		    // 		      else 
		    // 			lix.rwVectorCursor()(i) /= correction(i)*sincConv(i)*sincConv(iy);//pbNorm;
		    //
		    // This without the PS functions
		    //
		    PBCorrection(i)=pbFunc(avgPBVec(i))*sincConv(i)*sincConv(iy);
		    if ((abs(PBCorrection(i))) >= pbLimit_p*peakAvgPB)
		      lix.rwVectorCursor()(i) /= PBCorrection(i);
		    else if (!makingPSF)
		      lix.rwVectorCursor()(i) /= sincConv(i)*sincConv(iy);
		  }

		if(normalize) 
		  {
		    if(weights(pol, chan)>0.0) 
		      {
			Complex rnorm(Float(inx)*Float(iny)/(weights(pol,chan)));
			lix.rwCursor()*=rnorm;
		      }
		    else 
		      lix.woCursor()=0.0;
		  }
		else 
		  {
		    Complex rnorm(Float(inx)*Float(iny));
		    lix.rwCursor()*=rnorm;
		  }
	      }
	    }
	}

	if(!isTiled) 
	  {
	    //
	    // Check the section from the image BEFORE converting to a lattice 
	    //
	    IPosition blc(4, (nx-image->shape()(0))/2,
			  (ny-image->shape()(1))/2, 0, 0);
	    IPosition stride(4, 1);
	    IPosition trc(blc+image->shape()-stride);
	    //
	    // Do the copy
	    //
	    image->put(griddedData(blc, trc));
	    //if(arrayLattice) delete arrayLattice; arrayLattice=0;
	    griddedData.resize(IPosition(1,0));
	  }
      }
	// if (!makingPSF)
	//   {
	//     String name("cdirty.im");
	//     image->put(griddedData);
	//     storeImg(name,*image);
	//   }

	
    return *image;
  }


#define NEED_UNDERSCORES
#if defined(NEED_UNDERSCORES)
#define gpbmos gpbmos_
#define dpbmos dpbmos_
#define dpbmosgrad dpbmosgrad_
#define dpbwgrad dpbwgrad_
#endif
  
  extern "C" { 
    void gpbmos(Double *uvw,
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
		Int *convwtsize,
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
		Double *currentCFPA, Double *actualPA,
		Int *avgPBReady,
		Complex *avgPB, Double *cfRefFreq_p,
		Complex *convWeights,
		Int *convWtSupport);
    void dpbmos(Double *uvw,
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
    void dpbmosgrad(Double *uvw,
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
		  Double *currentCFPA, Double *actualPA,Double *cfRefFreq_p);
  }
  //
  //----------------------------------------------------------------------
  //
  void PBMosaicFT::runFortranGet(Matrix<Double>& uvw,Vector<Double>& dphase,
				 Cube<Complex>& visdata,
				 IPosition& s,
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
    Int N;
    actualPA = getVBPA(vb);

    N=polMap.nelements();
    CFMap = polMap; ConjCFMap = polMap;
    for(Int i=0;i<N;i++) CFMap[i] = polMap[N-i-1];
    
    Array<Complex> rotatedConvFunc;
//     SynthesisUtils::rotateComplexArray(logIO(), convFunc, convFuncCS_p, 
// 				       rotatedConvFunc,(currentCFPA-actualPA),"LINEAR");
    SynthesisUtils::rotateComplexArray(logIO(), convFunc, convFuncCS_p, 
				       rotatedConvFunc,0.0);

    ConjCFMap = polMap;
    makeCFPolMap(vb,CFMap);
    makeConjPolMap(vb,CFMap,ConjCFMap);

    
    ConjCFMap_p     = ConjCFMap.getStorage(deleteThem(CONJCFMAP));
    CFMap_p         = CFMap.getStorage(deleteThem(CFMAP));
    
    uvw_p           = uvw.getStorage(deleteThem(UVW));
    dphase_p        = dphase.getStorage(deleteThem(DPHASE));
    visdata_p       = visdata.getStorage(deleteThem(VISDATA));
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

//     static int ttt=0;
//     if (ttt==0) cout << l_off(IPosition(3,0,0,0)) << " " << m_off(IPosition(3,0,0,0)) << endl;
//     ttt++;
    
    Int npa=convSupport.shape()(2),actualConvSize;
    Int paIndex_Fortran = paIndex;
    actualConvSize = convFunc.shape()(0);
    
    IPosition shp=convSupport.shape();
    Int alwaysDoPointing=1;
    alwaysDoPointing=doPointing;
    dpbmos(uvw_p,
	   dphase_p,
	   visdata_p,
	   &s.asVector()(0),
	   &s.asVector()(1),
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
	   &alwaysDoPointing,
	   &npa,
	   &paIndex_Fortran,
	   CFMap_p,
	   ConjCFMap_p,
	   &currentCFPA
	   ,&actualPA, &cfRefFreq_p
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
  void PBMosaicFT::runFortranGetGrad(Matrix<Double>& uvw,Vector<Double>& dphase,
				     Cube<Complex>& visdata,
				     IPosition& s,
				     Cube<Complex>& gradVisAzData,
				     Cube<Complex>& gradVisElData,
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
// 				       rotatedConvFunc,(currentCFPA-actualPA));
    SynthesisUtils::rotateComplexArray(logIO(), convFunc, convFuncCS_p, 
				       rotatedConvFunc,0.0);

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
    Int alwaysDoPointing=1;
    alwaysDoPointing = doPointing;
    dpbmosgrad(uvw_p,
	     dphase_p,
	     visdata_p,
	     &s.asVector()(0),
	     &s.asVector()(1),
	     gradVisAzData_p,
	     gradVisElData_p,
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
	     &alwaysDoPointing,
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
  void PBMosaicFT::runFortranPut(Matrix<Double>& uvw,Vector<Double>& dphase,
				 const Complex& visdata,
				 IPosition& s,
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
			  CONVSUPPORT,CONVWTSUPPORT,CONVFUNC,CHANMAP,POLMAP,VBANT1,VBANT2,WEIGHT,
			  SUMWEIGHT,CONJCFMAP,CFMAP,AVGPBPTR,CONVWTS};
    Vector<Bool> deleteThem(25);
    
    Double *uvw_p, *dphase_p, *actualOffset_p, *vb_freq_p, *uvScale_p;
    Complex *dataPtr_p, *f_convFunc_p, *f_convWts_p,*avgPBPtr;
    Int *flags_p, *rowFlags_p, *chanMap_p, *polMap_p, *convSupport_p, *convWtSupport_p,
      *vb_ant1_p, *vb_ant2_p,
      *ConjCFMap_p, *CFMap_p;
    Float *l_off_p, *m_off_p;
    Float *weight_p;Double *sumwt_p,*isumwt_p;
    Double actualPA;
    const Complex *visdata_p=&visdata;
    
    Matrix<Double> iSumWt(sumWeight.shape());
    iSumWt=0.0;
    Vector<Int> ConjCFMap, CFMap;
    actualPA = getVBPA(vb);

    ConjCFMap = polMap;
    makeCFPolMap(vb,CFMap);
    makeConjPolMap(vb,CFMap,ConjCFMap);

    Array<Complex> rotatedConvFunc;
//     SynthesisUtils::rotateComplexArray(logIO(), convFunc, convFuncCS_p, 
// 				       rotatedConvFunc,(currentCFPA-actualPA));
    SynthesisUtils::rotateComplexArray(logIO(), convFunc, convFuncCS_p, 
				       rotatedConvFunc,0.0);


    ConjCFMap_p     = ConjCFMap.getStorage(deleteThem(CONJCFMAP));
    CFMap_p         = CFMap.getStorage(deleteThem(CFMAP));
    
    uvw_p           = uvw.getStorage(deleteThem(UVW));
    dphase_p        = dphase.getStorage(deleteThem(DPHASE));
    flags_p         = flags.getStorage(deleteThem(FLAGS));
    rowFlags_p      = rowFlags.getStorage(deleteThem(ROWFLAGS));
    uvScale_p       = uvScale.getStorage(deleteThem(UVSCALE));
    actualOffset_p  = actualOffset.getStorage(deleteThem(ACTUALOFFSET));
    dataPtr_p       = dataPtr.getStorage(deleteThem(DATAPTR));
    vb_freq_p       = (Double *)(vb.frequency().getStorage(deleteThem(VBFREQ)));
    convSupport_p   = convSupport.getStorage(deleteThem(CONVSUPPORT));
    convWtSupport_p = convWtSupport.getStorage(deleteThem(CONVWTSUPPORT));
    //    f_convFunc_p      = convFunc.getStorage(deleteThem(CONVFUNC));
    f_convFunc_p      = rotatedConvFunc.getStorage(deleteThem(CONVFUNC));
    f_convWts_p     = convWeights.getStorage(deleteThem(CONVWTS));
    chanMap_p       = chanMap.getStorage(deleteThem(CHANMAP));
    polMap_p        = polMap.getStorage(deleteThem(POLMAP));
    vb_ant1_p       = (Int *)(vb.antenna1().getStorage(deleteThem(VBANT1)));
    vb_ant2_p       = (Int *)(vb.antenna2().getStorage(deleteThem(VBANT2)));
    l_off_p         = l_off.getStorage(deleteThem(RAOFF));
    m_off_p         = m_off.getStorage(deleteThem(DECOFF));
    weight_p        = (Float *)(weight.getStorage(deleteThem(WEIGHT)));
    sumwt_p         = sumWeight.getStorage(deleteThem(SUMWEIGHT));
    Bool tmp;
    isumwt_p        = iSumWt.getStorage(tmp);

    //    Array<Complex> avgPB_p(griddedWeights.get());
    Array<Complex> avgPB_p;
    if (!avgPBReady)
      {
	avgPB_p.resize(griddedWeights.shape());
	avgPB_p=Complex(0,0);
	avgPBPtr        = avgPB_p.getStorage(deleteThem(AVGPBPTR));
      }
    else
      avgPBPtr=NULL;
    
    Int npa=convSupport.shape()(2),actualConvSize, actualConvWtSize;
    Int paIndex_Fortran = paIndex; 
    Int doAvgPB=((avgPBReady==false) && 
		 ((fabs(lastPAUsedForWtImg-actualPA)*57.2956 >= DELTAPA) || 
		  (lastPAUsedForWtImg == MAGICPAVALUE)));
    doAvgPB=(avgPBReady==false);
    actualConvSize = convFunc.shape()(0);
    actualConvWtSize = convWeights.shape()(0);

    if (fabs(lastPAUsedForWtImg-actualPA)*57.2956 >= DELTAPA) lastPAUsedForWtImg = actualPA;

    IPosition shp=convSupport.shape();
    Int alwaysDoPointing=1;
    alwaysDoPointing = doPointing;
    gpbmos(uvw_p,
	   dphase_p,
	   visdata_p,
	   &s.asVector()(0),
	   &s.asVector()(1),
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
	   &actualConvWtSize,
	   &convSampling,
	   &wConvSize,
	   f_convFunc_p,
	   chanMap_p,
	   polMap_p,
	   &polInUse,
	   //	   sumwt_p,
	   isumwt_p,
	   vb_ant1_p,
	   vb_ant2_p,
	   &Nant_p,
	   &scanNo,
	   &sigma,
	   l_off_p, m_off_p,
	   &area,
	   &doGrad,
	   &alwaysDoPointing,
	   &npa,
	   &paIndex_Fortran,
	   CFMap_p,
	   ConjCFMap_p,
	   &currentCFPA,&actualPA,
	   &doAvgPB,
	   avgPBPtr,&cfRefFreq_p,
	   f_convWts_p,convWtSupport_p
	   );

    ConjCFMap.freeStorage((const Int *&)ConjCFMap_p,deleteThem(CONJCFMAP));
    CFMap.freeStorage((const Int *&)CFMap_p,deleteThem(CFMAP));
    
    l_off.freeStorage((const Float*&)l_off_p,deleteThem(RAOFF));
    m_off.freeStorage((const Float*&)m_off_p,deleteThem(DECOFF));
    uvw.freeStorage((const Double*&)uvw_p,deleteThem(UVW));
    dphase.freeStorage((const Double*&)dphase_p,deleteThem(DPHASE));
    flags.freeStorage((const Int*&) flags_p,deleteThem(FLAGS));
    rowFlags.freeStorage((const Int *&)rowFlags_p,deleteThem(ROWFLAGS));
    actualOffset.freeStorage((const Double*&)actualOffset_p,deleteThem(ACTUALOFFSET));
    dataPtr.freeStorage((const Complex *&)dataPtr_p,deleteThem(DATAPTR));
    uvScale.freeStorage((const Double*&) uvScale_p,deleteThem(UVSCALE));
    vb.frequency().freeStorage((const Double*&)vb_freq_p,deleteThem(VBFREQ));
    convSupport.freeStorage((const Int*&)convSupport_p,deleteThem(CONVSUPPORT));
    convFunc.freeStorage((const Complex *&)f_convFunc_p,deleteThem(CONVFUNC));
    convWeights.freeStorage((const Complex *&)f_convFunc_p,deleteThem(CONVWTS));
    chanMap.freeStorage((const Int*&)chanMap_p,deleteThem(CHANMAP));
    polMap.freeStorage((const Int*&) polMap_p,deleteThem(POLMAP));
    vb.antenna1().freeStorage((const Int*&) vb_ant1_p,deleteThem(VBANT1));
    vb.antenna2().freeStorage((const Int*&) vb_ant2_p,deleteThem(VBANT2));
    weight.freeStorage((const Float*&)weight_p,deleteThem(WEIGHT));
    sumWeight.putStorage(sumwt_p,deleteThem(SUMWEIGHT));
    iSumWt.putStorage(isumwt_p,tmp);
    sumWeight += iSumWt;

    if (!avgPBReady)
      {
	nApertures+=Complex(1.0,0.0);
	// Get the griddedWeigths as a referenced array
	Array<Complex> gwts; Bool removeDegenerateAxis=false;
	griddedWeights.get(gwts, removeDegenerateAxis);
	//	griddedWeights.put(griddedWeights.get()+avgPB_p);
	gwts = gwts + avgPB_p;
	// if (!reference)
	//   griddedWeights.put(gwts);
      }
  }
  //
  //---------------------------------------------------------------
  //
  // Initialize the FFT to the Sky. Here we have to setup and
  // initialize the grid.
  //
  void PBMosaicFT::initializeToSky(ImageInterface<Complex>& iimage,
				   Matrix<Float>& weight,
				   const VisBuffer& vb)
  {
    image=&iimage;
    
    init();
    initMaps(vb);
    nx    = image->shape()(0);
    ny    = image->shape()(1);
    npol  = image->shape()(2);
    nchan = image->shape()(3);
    
    if(image->shape().product()>cachesize) isTiled=true;
    else                                   isTiled=false;
    
    
    sumWeight=0.0;
    weight.resize(sumWeight.shape());
    weight=0.0;

    if(isTiled) 
      {
	imageCache->flush();
	image->set(Complex(0.0));
	//lattice=image;
	lattice=CountedPtr<Lattice<Complex> > (image, false);
      }
    else 
      {
	IPosition gridShape(4, nx, ny, npol, nchan);
	griddedData.resize(gridShape);
	griddedData=Complex(0.0);
//	if(arrayLattice) delete arrayLattice; arrayLattice=0;
	arrayLattice = new ArrayLattice<Complex>(griddedData);
	lattice=arrayLattice;
      }
    //AlwaysAssert(lattice, AipsError);
    PAIndex = -1;
    if (resetPBs)
      {
	griddedWeights.resize(iimage.shape()); 
	griddedWeights.setCoordinateInfo(iimage.coordinates());
	griddedWeights.set(0.0);
	noOfPASteps = 1;
	pbPeaks.resize(griddedWeights.shape()(2));
	pbPeaks.set(0.0);
	resetPBs=false;
      }
  }
  //
  //---------------------------------------------------------------
  //
  void PBMosaicFT::finalizeToSky()
  {
    if(isTiled) 
      {
	logIO() << LogOrigin("PBMosaicFT", "finalizeToSky")  << LogIO::NORMAL;
	
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

} //# NAMESPACE CASA - END
