// -*- C++ -*-
//# AWProjectWBFT.cc: Implementation of AWProjectWBFT class
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

#include <synthesis/TransformMachines/AWProjectWBFT.h>
#include <synthesis/TransformMachines/AWVisResampler.h>
#include <synthesis/TransformMachines/StokesImageUtil.h>
#include <casacore/coordinates/Coordinates/CoordinateSystem.h>
#include <casacore/scimath/Mathematics/FFTServer.h>
#include <casacore/scimath/Mathematics/Convolver.h>
#include <casacore/lattices/LatticeMath/LatticeFFT.h>
#include <casacore/images/Images/ImageInterface.h>
#include <casacore/images/Images/PagedImage.h>
#include <msvis/MSVis/VisBuffer.h>
#include <casacore/casa/Arrays/Vector.h>
#include <casacore/casa/Arrays/Slice.h>
#include <casacore/casa/Arrays/ArrayMath.h>
#include <casacore/casa/Arrays/Array.h>
#include <casacore/casa/OS/HostInfo.h>
#include <sstream>
#include <casacore/casa/Utilities/CompositeNumber.h>

//#define CONVSIZE (1024*4)
//#define OVERSAMPLING 10
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

  AWProjectWBFT::AWProjectWBFT(Int nWPlanes, Long icachesize, 
			       CountedPtr<CFCache>& cfcache,
			       CountedPtr<ConvolutionFunction>& cf,
			       CountedPtr<VisibilityResamplerBase>& visResampler,
			       Bool applyPointingOffset,
			       Bool doPBCorr,
			       Int itilesize, 
			       Float paSteps,
			       Float pbLimit,
			       Bool usezero,
			       Bool conjBeams,
			       Bool doublePrecGrid)
    : AWProjectFT(nWPlanes,icachesize,cfcache,cf,visResampler,applyPointingOffset,doPBCorr, itilesize,pbLimit,usezero,conjBeams,doublePrecGrid),
      avgPBReady_p(false),resetPBs_p(true),wtImageFTDone_p(false),fieldIds_p(0),rotatedCFWts_p(),visResamplerWt_p(),oneTimeMessage_p(false)
  {
    (void)paSteps;
    //
    // Set the function pointer for FORTRAN call for GCF services.  
    // This is a pure C function pointer which FORTRAN can call.  
    // The function itself uses GCF object services.
    //
    //    convSize=0;
    paChangeDetector.reset();
    pbLimit_p=pbLimit;
    if (applyPointingOffset) doPointing=1; else doPointing=0;
    maxConvSupport=-1;  
    //
    // Set up the Conv. Func. disk cache manager object.
    //
    //convSampling=OVERSAMPLING;
    //    convSize=CONVSIZE;
    //use memory size defined in aipsrc if exists
    Long hostRAM = (HostInfo::memoryTotal(true)*1024); // In bytes
    hostRAM = hostRAM/(sizeof(Float)*2); // In complex pixels
    if (cachesize > hostRAM) cachesize=hostRAM;

    //    visResampler_p->init(useDoubleGrid_p);
    lastPAUsedForWtImg = MAGICPAVALUE;
  }
  //
  //---------------------------------------------------------------
  //
  AWProjectWBFT::AWProjectWBFT(const RecordInterface& stateRec)
    : AWProjectFT(stateRec), oneTimeMessage_p(false)
  {
    LogIO log_l(LogOrigin("AWProjectWBFT", "AWProjectWBFT[R&D]"));
    // Construct from the input state record
    
    //    if (!fromRecord(error, stateRec)) 
    if (!fromRecord(stateRec)) 
      log_l << "Failed to create " << name() << " object." << LogIO::EXCEPTION;

    maxConvSupport=-1;
    //convSampling=OVERSAMPLING;
    //convSize=CONVSIZE;
    visResampler_p->init(useDoubleGrid_p);
  }
  //
  //---------------------------------------------------------------
  //
  AWProjectWBFT& AWProjectWBFT::operator=(const AWProjectWBFT& other)
  {
    if(this!=&other) 
      {
	AWProjectFT::operator=(other);
	padding_p       =   other.padding_p;
	nWPlanes_p      =   other.nWPlanes_p;
	imageCache      =   other.imageCache;
	cachesize       =   other.cachesize;
	tilesize        =   other.tilesize;
	gridder         =   other.gridder;
	isTiled         =   other.isTiled;
	lattice         =   other.lattice;
	arrayLattice    =   other.arrayLattice;
	maxAbsData      =   other.maxAbsData;
	centerLoc       =   other.centerLoc;
	offsetLoc       =   other.offsetLoc;
	pointingToImage =   other.pointingToImage;
	usezero_p       =   other.usezero_p;
	doPBCorrection  =   other.doPBCorrection;
	maxConvSupport  =   other.maxConvSupport;
	avgPBReady_p    =   other.avgPBReady_p;
	resetPBs_p      =   other.resetPBs_p;
	wtImageFTDone_p =   other.wtImageFTDone_p;
	rotatedCFWts_p  =   other.rotatedCFWts_p;
	visResamplerWt_p=other.visResamplerWt_p; // Copy the counted pointer
	oneTimeMessage_p = other.oneTimeMessage_p;
	//	*visResamplerWt_p = *other.visResamplerWt_p; // Call the appropriate operator=()

	//	visResampler_p=other.visResampler_p->clone();
    };
    return *this;
  };
  //
  //----------------------------------------------------------------------
  //
  Int AWProjectWBFT::findPointingOffsets(const VisBuffer& vb, 
					 Array<Float> &l_off,
					 Array<Float> &m_off,
					 Bool Evaluate)
  {
    LogIO log_l(LogOrigin("AWProjectWBFT","findPointingOffsets[R&D]"));
    Int NAnt=0;
    //
    // This will return 0 if EPJ Table is not given.  Otherwise will
    // return the number of antennas it detected (from the EPJ table)
    // and the offsets in l_off and m_off.
    //
    NAnt = AWProjectFT::findPointingOffsets(vb,l_off,m_off,Evaluate);
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
    CoordinateSystem coords(image->coordinates());
    Int directionIndex=coords.findCoordinate(Coordinate::DIRECTION);
    AlwaysAssert(directionIndex>=0, AipsError);
    DirectionCoordinate directionCoord=coords.directionCoordinate(directionIndex);
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
  void AWProjectWBFT::normalizeAvgPB()
  {
    LogIO log_l(LogOrigin("AWProjectWBFT","normalizeAvgPB[R&D]"));
    // We accumulated normalized PBs.  So don't normalize the average
    // PB.
    pbNormalized_p = false;
    
  }
  //
  //---------------------------------------------------------------
  // For wide-band case, this just tells the user that the sensitivity
  // image will be computed during the first gridding cycle.
  //
  // Weights image (sum of convolution functions) is accumulated
  // during gridding.  At the end of the gridding cycle, the weight
  // image is FT'ed (using ftWeightImage()) and properly normalized
  // and converted to sensitivity image (using
  // makeSensitivityImage()).  These methods are triggred in the first
  // call to getImage().  In subsequent calls to getImage() these
  // calls are NoOps.
  //
  void AWProjectWBFT::makeSensitivityImage(const VisBuffer&,
					   const ImageInterface<Complex>& /*imageTemplate*/,
					   ImageInterface<Float>& /*sensitivityImage*/)
  {
    if (oneTimeMessage_p == false)
      {
	LogIO log_l(LogOrigin("AWProjectWBFT", "makeSensitivityImage[R&D]"));
	log_l << "Setting up for weights accumulation ";
	if (sensitivityPatternQualifierStr_p != "") log_l << "for term " << sensitivityPatternQualifier_p << " ";
	log_l << "during gridding to compute sensitivity pattern." 
	      << endl
	      << "Consequently, the first gridding cycle will be slower than the subsequent ones." 
	      << LogIO::WARN;
	oneTimeMessage_p=true;
      }
  }
  //
  //---------------------------------------------------------------
  // 
  void AWProjectWBFT::ftWeightImage(Lattice<Complex>& wtImage, 
				    const Matrix<Float>& sumWt,
				    const Bool& doFFTNorm)
  {
    LogIO log_l(LogOrigin("AWProjectWBFT", "ftWeightImage[R&D]"));
    if (wtImageFTDone_p) return;

    //cerr << "From ftWeightImage" << endl;
    // not used
    // Bool doSumWtNorm=true;
    // if (sumWt.shape().nelements()==0) doSumWtNorm=false;
    if ((sumWt.shape().nelements() < 2) || 
	(sumWt.shape()(0) != wtImage.shape()(2)) || 
	(sumWt.shape()(1) != wtImage.shape()(3)))
      log_l << "Sum of weights per poln and chan is required" << LogIO::EXCEPTION;
    //
    // Use only the amplitude of the gridded weights.  LatticeExpr
    // classes while useful, appear to be too strict about types (the
    // code below will not compile if the abs(wtImage) is not
    // converted back to a complex type).

    // LatticeExpr<Complex> le(abs(wtImage)*Complex(1,0));
    // wtImage.copyData(le);

    // {
    //   String name("wtimg.im");
    //   storeArrayAsImage(name,griddedWeights.coordinates(),wtImage.get());
    // }
    LatticeFFT::cfft2d(wtImage,false);
    // {
    //   String name("ftwtimg.im");
    //   storeArrayAsImage(name,griddedWeights.coordinates(),wtImage.get());
    // }
    wtImageFTDone_p=true;

    Int sizeX=wtImage.shape()(0), sizeY=wtImage.shape()(1);

    Array<Complex> wtBuf; wtImage.get(wtBuf,false);
    ArrayLattice<Complex> wtLat(wtBuf,true);
    //
    // Copy one 2D plane at a time, normalizing by the sum of weights
    // and possibly 2D FFT.
    //
    // Set up Lattice iterators on wtImage and sensitivityImage
    //
    IPosition axisPath(4, 0, 1, 2, 3);
    IPosition cursorShape(4, sizeX, sizeY, 1, 1);

    LatticeStepper wtImStepper(wtImage.shape(), cursorShape, axisPath);
    LatticeIterator<Complex> wtImIter(wtImage, wtImStepper);
    //
    // Iterate over channel and polarization axis
    //
    if (!doFFTNorm) sizeX=sizeY=1;
    //
    // Normalize each frequency and polarization plane of the complex
    // sensitivity pattern
    //
    // getSumOfCFWeights() returns the sum-of-weight appropriate for
    // computing sensitivity patterns (which could be different from
    // the data-sum-of-weights.
    //

    // USEFUL DEBUG MESSAGE
    //cerr << "SumCFWt: " << getSumOfCFWeights() << " " << max(wtBuf) << " " << sensitivityPatternQualifier_p << endl;
    for(wtImIter.reset(); !wtImIter.atEnd(); wtImIter++)
      {
	Int pol_l=wtImIter.position()(2), chan_l=wtImIter.position()(3);
	Double sumwt_l=1.0;;
	// Lets write some mildly obfuscated code ~[8-)
	//if ((sensitivityPatternQualifier_p == -1) && (doSumWtNorm))
	//  sumwt_l = ((sumwt_l = getSumOfCFWeights()(pol_l,chan_l))==0)?1.0:sumwt_l;

	sumwt_l = getSumOfCFWeights()(pol_l,chan_l);

	wtImIter.rwCursor() = (wtImIter.rwCursor()
			       *Float(sizeX)*Float(sizeY)
			       /sumwt_l
			       );

	////////////////////	wtImIter.rwCursor() = sqrt( fabs(wtImIter.rwCursor()) );

	//	Double maxval = fabs( max( wtImIter.rwCursor() ) );
	//	sumwt_l = getSumOfCFWeights()(pol_l,chan_l);
	//	weightRatio_p = maxval * Float(sizeX)*Float(sizeY) / sumwt_l;
      }

  }
  //
  //---------------------------------------------------------------
  // 
  void AWProjectWBFT::makeSensitivitySqImage(Lattice<Complex>& wtImage,
					     ImageInterface<Complex>& sensitivitySqImage,
					     const Matrix<Float>& sumWt,
					     const Bool& doFFTNorm)
  {
    //    if (avgPBReady_p) return;
    LogIO log_l(LogOrigin("AWProjectWBFT", "makeSensitivitySqImage[R&D]"));

    //    avgPBReady_p=true;

    ftWeightImage(wtImage, sumWt, doFFTNorm);

    sensitivitySqImage.resize(griddedWeights.shape()); 
    sensitivitySqImage.setCoordinateInfo(griddedWeights.coordinates());

    Int sizeX=wtImage.shape()(0), sizeY=wtImage.shape()(1);
    Array<Complex> senSqBuf; sensitivitySqImage.get(senSqBuf,false); 
    ArrayLattice<Complex> senSqLat(senSqBuf, true);

    Array<Complex> wtBuf; wtImage.get(wtBuf,false);
    ArrayLattice<Complex> wtLat(wtBuf,true);
    //
    // Copy one 2D plane at a time, normalizing by the sum of weights
    // and possibly 2D FFT.
    //
    // Set up Lattice iterators on wtImage and sensitivityImage
    //
    IPosition axisPath(4, 0, 1, 2, 3);
    IPosition cursorShape(4, sizeX, sizeY, 1, 1);

    LatticeStepper wtImStepper(wtImage.shape(), cursorShape, axisPath);
    LatticeIterator<Complex> wtImIter(wtImage, wtImStepper);

    LatticeStepper senSqImStepper(senSqLat.shape(), cursorShape, axisPath);
    LatticeIterator<Complex> senSqImIter(senSqLat, senSqImStepper);
    //
    // Iterate over channel and polarization axis
    //
    //
    // The following code is averaging RR and LL planes and writing
    // the result to back to both planes.  This needs to be
    // generalized for full-pol case.
    //
    IPosition start0(4,0,0,0,0), start1(4,0,0,1,0), length(4,sizeX,sizeY,1,1);
    Slicer slicePol0(start0,length), slicePol1(start1,length);
    Array<Complex> polPlane0C, polPlane1C, polPlaneSq0C, polPlaneSq1C;

    senSqLat.getSlice(polPlaneSq0C, slicePol0);
    senSqLat.getSlice(polPlaneSq1C,slicePol1);
    wtLat.getSlice(polPlane0C, slicePol0);
    wtLat.getSlice(polPlane1C, slicePol1);

    polPlaneSq0C = polPlane0C*polPlane1C;
    polPlaneSq1C = polPlaneSq0C;

    pbNormalized_p=false;

    resetPBs_p=false;
    // String name("avgPBSq.im");
    // storeImg(name,sensitivitySqImage);
  }
  //
  //---------------------------------------------------------------
  // 
  void AWProjectWBFT::makeSensitivityImage(Lattice<Complex>& wtImage,
					   ImageInterface<Float>& sensitivityImage,
					   const Matrix<Float>& sumWt,
					   const Bool& doFFTNorm)
  {
    if (avgPBReady_p) return;
    LogIO log_l(LogOrigin("AWProjectWBFT", "makeSensitivityImage[R&D]"));


    // Matrix<Float> cfWts(sumWt.shape());
    // convertArray(cfWts,sumCFWeight);
    // ftWeightImage(wtImage, cfWts, doFFTNorm);
    // {
    //   String name("wtimg.im");
    //   storeArrayAsImage(name,griddedWeights.coordinates(),wtImage.get());
    // }
    ftWeightImage(wtImage, sumWt, doFFTNorm);
    // {
    //   String name("ftwtimg.im");
    //   storeArrayAsImage(name,griddedWeights.coordinates(),wtImage.get());
    // }
    // if (tt_pp != "")
    //   { // uncommented by UUU
    // 	String name("ftwtimg"+sensitivityPatternQualifierStr_p+".im.conj");
    // 	storeArrayAsImage(name,griddedWeights.coordinates(),wtImage.get());
    //   }
    // else
    //   { // uncommented by UUU
    // 	String name("ftwtimg"+sensitivityPatternQualifierStr_p+".im");
    // 	storeArrayAsImage(name,griddedWeights.coordinates(),wtImage.get());
    //   }

    sensitivityImage.resize(griddedWeights.shape()); 
    sensitivityImage.setCoordinateInfo(griddedWeights.coordinates());

    Int sizeX=wtImage.shape()(0), sizeY=wtImage.shape()(1);
    Array<Float> senBuf; sensitivityImage.get(senBuf,false); 
    ArrayLattice<Float> senLat(senBuf, true);

    Array<Complex> wtBuf; wtImage.get(wtBuf,false);
    ArrayLattice<Complex> wtLat(wtBuf,true);
    //
    // Set up Lattice iteratos on wtImage and sensitivityImage
    //
    IPosition axisPath(4, 0, 1, 2, 3);
    IPosition cursorShape(4, sizeX, sizeY, 1, 1);

    LatticeStepper senImStepper(senLat.shape(), cursorShape, axisPath);
    LatticeIterator<Float> senImIter(senLat, senImStepper);
    //
    // The following code is averaging RR and LL planes and writing
    // the result to back to both planes.  This needs to be
    // generalized for full-pol case.
    //
    IPosition start0(4,0,0,0,0), start1(4,0,0,1,0), length(4,sizeX,sizeY,1,1);
    Slicer slicePol0(start0,length), slicePol1(start1,length);
    Array<Float> polPlane0F, polPlane1F;
    Array<Complex> polPlane0C, polPlane1C;
    
    // Use StokesImageUtil to convert from Complex sensitivity pattern
    // to real value image planes.
    //    StokesImageUtil::To(sensitivityImage, griddedWeights);

    //
    // Copy the real part of the average of all planes of the wtImage to
    // all the planes of the sensitivity image (senImage).
    //
    LatticeStepper wtImStepper(wtImage.shape(), cursorShape, axisPath);
    LatticeIterator<Complex> wtImIter(wtImage, wtImStepper);

    //    Matrix<Complex> tmp(senImIter.rwMatrixCursor().shape());
    Matrix<Complex> tmp(senImIter.rwMatrixCursor().shape());
    tmp = 1.0;
    Int n=0;
    for(wtImIter.reset(); !wtImIter.atEnd(); wtImIter++)
      {
	//	tmp += real(wtImIter.rwMatrixCursor().nonDegenerate());
	tmp = (wtImIter.rwMatrixCursor().nonDegenerate());
	n++;
      }
    //    tmp = tmp/n;
    //UUU// tmp = fabs(tmp);  // fabs(Array<Complex>&) returns a complex array
    //    tmp = (sqrt(fabs(tmp)));  // fabs(Array<Complex>&) returns a complex array
    for(senImIter.reset(); !senImIter.atEnd(); senImIter++)
      senImIter.rwMatrixCursor() = real(tmp);

    if (tt_pp == "")
      cfCache_p->flush(sensitivityImage,sensitivityPatternQualifierStr_p);

    pbNormalized_p=false;
    resetPBs_p=false;

    avgPBReady_p=true;
  }
  //
  //---------------------------------------------------------------
  //
  // Convert the gridded visibilities to image. This implements the
  // basic 2D transform (vC-Z Theorem) using the FFT.
  //
  // This specialization of getImage() exists since the sensitivity
  // pattern in this FTMachine is computed as the FT of the gridded
  // weights (convolution functions).  The gridded weights are
  // available along with the gridded data at the end of the gridding
  // cycle.  This method first converts the gridded weights to
  // sensitivity image and then calls AWProjectFT::getImage(), which
  // in-turn calls normalizeImage() with the sensitivityImage and the
  // sum of weights.
  //
  ImageInterface<Complex>& AWProjectWBFT::getImage(Matrix<Float>& weights,
						   Bool fftNormalization) 
  {
    AlwaysAssert(image, AipsError);
    LogIO log_l(LogOrigin("AWProjectWBFT", "getImage[R&D]"));

    weights.resize(sumWeight.shape());
    convertArray(weights, sumWeight);//I suppose this converts a
				     //Matrix<Double> (sumWeights) to
				     //Matrix<Float> (weights).  Why
				     //is this conversion required?
				     //--SB (Dec. 2010)
    //
    // Compute the sensitivity image as FT of the griddedWegiths.
    // Return the result in avgPB_p.  Cache it in the cfCache_p
    // object.  And raise the avgPBReady_p flag so that this becomes a
    // null call the next time around.
    // // REMOVE THIS CODE
    // {
    //   tt_pp="_conj_";
    //   makeSensitivityImage(griddedConjWeights, *avgPB_p, weights, true);
    //   tt_pp="";
    //   wtImageFTDone_p = avgPBReady_p=false;
    // }
    // // REMOVE THIS CODE
    makeSensitivityImage(griddedWeights, *avgPB_p, weights, true);

    //        if (avgPBSq_p.null()) avgPBSq_p = new TempImage<Complex>();
    //    makeSensitivitySqImage(griddedWeights, *avgPBSq_p, weights, true);

    //
    // This calls the overloadable method normalizeImage() which
    // normalizes the raw image by the sensitivty pattern (avgPB_p).
    //
    AWProjectFT::getImage(weights,fftNormalization);
    // if (makingPSF)
    //   {
    //     String name("psf.im");
    //     image->put(griddedData);
    //     storeImg(name,*image);
    //   }
	
    return *image;
  }


// #define NEED_UNDERSCORES
// #if defined(NEED_UNDERSCORES)
// #define gpbmos gpbmos_
// #define dpbmos dpbmos_
// #define dpbmosgrad dpbmosgrad_
// #define dpbwgrad dpbwgrad_
// #endif
  
//   extern "C" { 
//     void gpbmos(Double *uvw,
// 		Double *dphase,
// 		const Complex *values,
// 		Int *nvispol,
// 		Int *nvischan,
// 		Int *dopsf,
// 		const Int *flag,
// 		const Int *rflag,
// 		const Float *weight,
// 		Int *nrow,
// 		Int *rownum,
// 		Double *scale,
// 		Double *offset,
// 		Complex *grid,
// 		Int *nx,
// 		Int *ny,
// 		Int *npol,
// 		Int *nchan,
// 		const Double *freq,
// 		const Double *c,
// 		Int *support,
// 		Int *convsize,
// 		Int *convwtsize,
// 		Int *sampling,
// 		Int *wconvsize,
// 		Complex *convfunc,
// 		Int *chanmap,
// 		Int *polmap,
// 		Int *polused,
// 		Double *sumwt,
// 		Int *ant1,
// 		Int *ant2,
// 		Int *nant,
// 		Int *scanno,
// 		Double *sigma,
// 		Float *raoff,
// 		Float *decoff,
// 		Double *area,
// 		Int *doGrad,
// 		Int *doPointingCorrection,
// 		Int *nPA,
// 		Int *paIndex,
// 		Int *CFMap,
// 		Int *ConjCFMap,
// 		Double *currentCFPA, Double *actualPA,
// 		Int *avgPBReady_p,
// 		Complex *avgPB, Double *cfRefFreq_p,
// 		Complex *convWeights,
// 		Int *convWtSupport);
//     void dpbmos(Double *uvw,
// 		Double *dphase,
// 		Complex *values,
// 		Int *nvispol,
// 		Int *nvischan,
// 		const Int *flag,
// 		const Int *rflag,
// 		Int *nrow,
// 		Int *rownum,
// 		Double *scale,
// 		Double *offset,
// 		const Complex *grid,
// 		Int *nx,
// 		Int *ny,
// 		Int *npol,
// 		Int *nchan,
// 		const Double *freq,
// 		const Double *c,
// 		Int *support,
// 		Int *convsize,
// 		Int *sampling,
// 		Int *wconvsize,
// 		Complex *convfunc,
// 		Int *chanmap,
// 		Int *polmap,
// 		Int *polused,
// 		Int *ant1, 
// 		Int *ant2, 
// 		Int *nant, 
// 		Int *scanno,
// 		Double *sigma, 
// 		Float *raoff, Float *decoff,
// 		Double *area, 
// 		Int *dograd,
// 		Int *doPointingCorrection,
// 		Int *nPA,
// 		Int *paIndex,
// 		Int *CFMap,
// 		Int *ConjCFMap,
// 		Double *currentCFPA, Double *actualPA, Double *cfRefFreq_p);
//     void dpbmosgrad(Double *uvw,
// 		  Double *dphase,
// 		  Complex *values,
// 		  Int *nvispol,
// 		  Int *nvischan,
// 		  Complex *gazvalues,
// 		  Complex *gelvalues,
// 		  Int *doconj,
// 		  const Int *flag,
// 		  const Int *rflag,
// 		  Int *nrow,
// 		  Int *rownum,
// 		  Double *scale,
// 		  Double *offset,
// 		  const Complex *grid,
// 		  Int *nx,
// 		  Int *ny,
// 		  Int *npol,
// 		  Int *nchan,
// 		  const Double *freq,
// 		  const Double *c,
// 		  Int *support,
// 		  Int *convsize,
// 		  Int *sampling,
// 		  Int *wconvsize,
// 		  Complex *convfunc,
// 		  Int *chanmap,
// 		  Int *polmap,
// 		  Int *polused,
// 		  Int *ant1, 
// 		  Int *ant2, 
// 		  Int *nant, 
// 		  Int *scanno,
// 		  Double *sigma, 
// 		  Float *raoff, Float *decoff,
// 		  Double *area, 
// 		  Int *dograd,
// 		  Int *doPointingCorrection,
// 		  Int *nPA,
// 		  Int *paIndex,
// 		  Int *CFMap,
// 		  Int *ConjCFMap,
// 		  Double *currentCFPA, Double *actualPA,Double *cfRefFreq_p);
//   }
//   //
//   //----------------------------------------------------------------------
//   //
//   void AWProjectWBFT::runFortranGet(Matrix<Double>& uvw,Vector<Double>& dphase,
// 				 Cube<Complex>& visdata,
// 				 IPosition& s,
// 				 Int& Conj,
// 				 Cube<Int>& flags,Vector<Int>& rowFlags,
// 				 Int& rownr,Vector<Double>& actualOffset,
// 				 Array<Complex>* dataPtr,
// 				 Int& aNx, Int& aNy, Int& npol, Int& nchan,
// 				 VisBuffer& vb,Int& Nant_p, Int& scanNo,
// 				 Double& sigma,
// 				 Array<Float>& l_off,
// 				 Array<Float>& m_off,
// 				 Double area,
// 				 Int& doGrad,
// 				 Int paIndex)
//   {
//     (void)Conj;
//     LogIO log_l(LogOrigin("AWProjectWBFT","runFortranGet[R&D]"));
//     enum whichGetStorage {RAOFF,DECOFF,UVW,DPHASE,VISDATA,GRADVISAZ,GRADVISEL,
// 			  FLAGS,ROWFLAGS,UVSCALE,ACTUALOFFSET,DATAPTR,VBFREQ,
// 			  CONVSUPPORT,CONVFUNC,CHANMAP,POLMAP,VBANT1,VBANT2,CONJCFMAP,CFMAP};
//     Vector<Bool> deleteThem(21);
    
//     Double *uvw_p, *dphase_p, *actualOffset_p, *vb_freq_p, *uvScale_p;
//     Complex *visdata_p, *dataPtr_p, *f_convFunc_p;
//     Int *flags_p, *rowFlags_p, *chanMap_p, *polMap_p, *convSupport_p, *vb_ant1_p, *vb_ant2_p,
//       *ConjCFMap_p, *CFMap_p;
//     Float *l_off_p, *m_off_p;
//     Double actualPA;
    
//     Vector<Int> ConjCFMap, CFMap;
//     Int N;
//     actualPA = getVBPA(vb);

//     N=polMap.nelements();
//     CFMap = polMap; ConjCFMap = polMap;
//     for(Int i=0;i<N;i++) CFMap[i] = polMap[N-i-1];
    
//     Array<Complex> rotatedConvFunc;

//     // Rotate the convolution function using Image rotation and
//     // disable rotation in the gridder
//     SynthesisUtils::rotateComplexArray(log_l, *cfs_p.data,/*convFunc_p*/ cfs_p.coordSys,
// 				       rotatedConvFunc,(currentCFPA-actualPA),"LINEAR");
//     actualPA = currentCFPA; 

//     // SynthesisUtils::rotateComplexArray(log_l, convFunc_p, cfs_p.coordSys,
//     // 				       rotatedConvFunc,0.0);

//     ConjCFMap = polMap;
//     makeCFPolMap(vb,cfStokes_p,CFMap);
//     makeConjPolMap(vb,CFMap,ConjCFMap);

    
//     ConjCFMap_p     = ConjCFMap.getStorage(deleteThem(CONJCFMAP));
//     CFMap_p         = CFMap.getStorage(deleteThem(CFMAP));
    
//     uvw_p           = uvw.getStorage(deleteThem(UVW));
//     dphase_p        = dphase.getStorage(deleteThem(DPHASE));
//     visdata_p       = visdata.getStorage(deleteThem(VISDATA));
//     flags_p         = flags.getStorage(deleteThem(FLAGS));
//     rowFlags_p      = rowFlags.getStorage(deleteThem(ROWFLAGS));
//     uvScale_p       = uvScale.getStorage(deleteThem(UVSCALE));
//     actualOffset_p  = actualOffset.getStorage(deleteThem(ACTUALOFFSET));
//     dataPtr_p       = dataPtr->getStorage(deleteThem(DATAPTR));
//     vb_freq_p       = vb.frequency().getStorage(deleteThem(VBFREQ));
//     convSupport_p   = cfs_p.xSupport.getStorage(deleteThem(CONVSUPPORT));
//     //    f_convFunc_p      = convFunc_p.getStorage(deleteThem(CONVFUNC));
//     f_convFunc_p      = rotatedConvFunc.getStorage(deleteThem(CONVFUNC));
//     chanMap_p       = chanMap.getStorage(deleteThem(CHANMAP));
//     polMap_p        = polMap.getStorage(deleteThem(POLMAP));
//     vb_ant1_p       = vb.antenna1().getStorage(deleteThem(VBANT1));
//     vb_ant2_p       = vb.antenna2().getStorage(deleteThem(VBANT2));
//     l_off_p     = l_off.getStorage(deleteThem(RAOFF));
//     m_off_p    = m_off.getStorage(deleteThem(DECOFF));

// //     static int ttt=0;
// //     if (ttt==0) cout << l_off(IPosition(3,0,0,0)) << " " << m_off(IPosition(3,0,0,0)) << endl;
// //     ttt++;
    
//     Int npa=1,actualConvSize;
//     Int paIndex_Fortran = paIndex;
//     //    actualConvSize = convFunc_p.shape()(0);
//     actualConvSize = cfs_p.data->shape()(0);
    
//     //    IPosition shp=convSupport.shape();
//     Int alwaysDoPointing=1;
//     alwaysDoPointing=doPointing;
//     dpbmos(uvw_p,
// 	   dphase_p,
// 	   visdata_p,
// 	   &s.asVector()(0),
// 	   &s.asVector()(1),
// 	   flags_p,
// 	   rowFlags_p,
// 	   &s.asVector()(2),
// 	   &rownr,
// 	   uvScale_p,
// 	   actualOffset_p,
// 	   dataPtr_p,
// 	   &aNx,
// 	   &aNy,
// 	   &npol,
// 	   &nchan,
// 	   vb_freq_p,
// 	   &C::c,
// 	   convSupport_p,
// 	   &actualConvSize,
// 	   &convSampling,
// 	   &wConvSize,
// 	   f_convFunc_p,
// 	   chanMap_p,
// 	   polMap_p,
// 	   &polInUse_p,
// 	   vb_ant1_p,
// 	   vb_ant2_p,
// 	   &Nant_p,
// 	   &scanNo,
// 	   &sigma,
// 	   l_off_p, m_off_p,
// 	   &area,
// 	   &doGrad,
// 	   &alwaysDoPointing,
// 	   &npa,
// 	   &paIndex_Fortran,
// 	   CFMap_p,
// 	   ConjCFMap_p,
// 	   &currentCFPA
// 	   ,&actualPA, &cfRefFreq_p
// 	   );
    
//     ConjCFMap.freeStorage((const Int *&)ConjCFMap_p,deleteThem(CONJCFMAP));
//     CFMap.freeStorage((const Int *&)CFMap_p,deleteThem(CFMAP));
    
//     l_off.freeStorage((const Float*&)l_off_p,deleteThem(RAOFF));
//     m_off.freeStorage((const Float*&)m_off_p,deleteThem(DECOFF));
//     uvw.freeStorage((const Double*&)uvw_p,deleteThem(UVW));
//     dphase.freeStorage((const Double*&)dphase_p,deleteThem(DPHASE));
//     visdata.putStorage(visdata_p,deleteThem(VISDATA));
//     flags.freeStorage((const Int*&) flags_p,deleteThem(FLAGS));
//     rowFlags.freeStorage((const Int *&)rowFlags_p,deleteThem(ROWFLAGS));
//     actualOffset.freeStorage((const Double*&)actualOffset_p,deleteThem(ACTUALOFFSET));
//     dataPtr->freeStorage((const Complex *&)dataPtr_p,deleteThem(DATAPTR));
//     uvScale.freeStorage((const Double*&) uvScale_p,deleteThem(UVSCALE));
//     vb.frequency().freeStorage((const Double*&)vb_freq_p,deleteThem(VBFREQ));
//     cfs_p.xSupport.freeStorage((const Int*&)convSupport_p,deleteThem(CONVSUPPORT));
//     //    convFunc_p.freeStorage((const Complex *&)f_convFunc_p,deleteThem(CONVFUNC));
//     chanMap.freeStorage((const Int*&)chanMap_p,deleteThem(CHANMAP));
//     polMap.freeStorage((const Int*&) polMap_p,deleteThem(POLMAP));
//     vb.antenna1().freeStorage((const Int*&) vb_ant1_p,deleteThem(VBANT1));
//     vb.antenna2().freeStorage((const Int*&) vb_ant2_p,deleteThem(VBANT2));
//   }
//   //
//   //----------------------------------------------------------------------
//   //
//   void AWProjectWBFT::runFortranGetGrad(Matrix<Double>& uvw,Vector<Double>& dphase,
// 				     Cube<Complex>& visdata,
// 				     IPosition& s,
// 				     Cube<Complex>& gradVisAzData,
// 				     Cube<Complex>& gradVisElData,
// 				     Int& Conj,
// 				     Cube<Int>& flags,Vector<Int>& rowFlags,
// 				     Int& rownr,Vector<Double>& actualOffset,
// 				     Array<Complex>* dataPtr,
// 				     Int& aNx, Int& aNy, Int& npol, Int& nchan,
// 				     VisBuffer& vb,Int& Nant_p, Int& scanNo,
// 				     Double& sigma,
// 				     Array<Float>& l_off,
// 				     Array<Float>& m_off,
// 				     Double area,
// 				     Int& doGrad,
// 				     Int paIndex)
//   {
//     LogIO log_l(LogOrigin("AWProjectWBFT","runFortranGetGrad[R&D]"));
//     enum whichGetStorage {RAOFF,DECOFF,UVW,DPHASE,VISDATA,GRADVISAZ,GRADVISEL,
// 			  FLAGS,ROWFLAGS,UVSCALE,ACTUALOFFSET,DATAPTR,VBFREQ,
// 			  CONVSUPPORT,CONVFUNC,CHANMAP,POLMAP,VBANT1,VBANT2,CONJCFMAP,CFMAP};
//     Vector<Bool> deleteThem(21);
    
//     Double *uvw_p, *dphase_p, *actualOffset_p, *vb_freq_p, *uvScale_p;
//     Complex *visdata_p, *dataPtr_p, *f_convFunc_p;
//     Complex *gradVisAzData_p, *gradVisElData_p;
//     Int *flags_p, *rowFlags_p, *chanMap_p, *polMap_p, *convSupport_p, *vb_ant1_p, *vb_ant2_p,
//       *ConjCFMap_p, *CFMap_p;
//     Float *l_off_p, *m_off_p;
//     Double actualPA;

//     Vector<Int> ConjCFMap, CFMap;
//     actualPA = getVBPA(vb);
//     ConjCFMap = polMap;
//     makeCFPolMap(vb,cfStokes_p,CFMap);
//     makeConjPolMap(vb,CFMap,ConjCFMap);

//     Array<Complex> rotatedConvFunc;
//     // Rotate the convolution function using Image rotation and
//     // disable rotation in the gridder
//     SynthesisUtils::rotateComplexArray(log_l, *(cfs_p.data) /*convFunc_p*/, cfs_p.coordSys,
// 				       rotatedConvFunc,(currentCFPA-actualPA));
//     actualPA = currentCFPA; 

//     // SynthesisUtils::rotateComplexArray(log_l, convFunc_p, cfs_p.coordSys,
//     // 				       rotatedConvFunc,0.0);

//     ConjCFMap_p     = ConjCFMap.getStorage(deleteThem(CONJCFMAP));
//     CFMap_p         = CFMap.getStorage(deleteThem(CFMAP));
    
//     uvw_p           = uvw.getStorage(deleteThem(UVW));
//     dphase_p        = dphase.getStorage(deleteThem(DPHASE));
//     visdata_p       = visdata.getStorage(deleteThem(VISDATA));
//     gradVisAzData_p = gradVisAzData.getStorage(deleteThem(GRADVISAZ));
//     gradVisElData_p = gradVisElData.getStorage(deleteThem(GRADVISEL));
//     flags_p         = flags.getStorage(deleteThem(FLAGS));
//     rowFlags_p      = rowFlags.getStorage(deleteThem(ROWFLAGS));
//     uvScale_p       = uvScale.getStorage(deleteThem(UVSCALE));
//     actualOffset_p  = actualOffset.getStorage(deleteThem(ACTUALOFFSET));
//     dataPtr_p       = dataPtr->getStorage(deleteThem(DATAPTR));
//     vb_freq_p       = vb.frequency().getStorage(deleteThem(VBFREQ));
//     convSupport_p   = cfs_p.xSupport.getStorage(deleteThem(CONVSUPPORT));
//     //    f_convFunc_p      = convFunc_p.getStorage(deleteThem(CONVFUNC));
//     f_convFunc_p      = rotatedConvFunc.getStorage(deleteThem(CONVFUNC));
//     chanMap_p       = chanMap.getStorage(deleteThem(CHANMAP));
//     polMap_p        = polMap.getStorage(deleteThem(POLMAP));
//     vb_ant1_p       = vb.antenna1().getStorage(deleteThem(VBANT1));
//     vb_ant2_p       = vb.antenna2().getStorage(deleteThem(VBANT2));
//     l_off_p     = l_off.getStorage(deleteThem(RAOFF));
//     m_off_p    = m_off.getStorage(deleteThem(DECOFF));
    
//     Int npa=1,actualConvSize;
//     Int paIndex_Fortran = paIndex;
//     //    actualConvSize = convFunc_p.shape()(0);
//     actualConvSize = cfs_p.data->shape()(0);
    
//     //    IPosition shp=convSupport.shape();
//     Int alwaysDoPointing=1;
//     alwaysDoPointing = doPointing;
//     dpbmosgrad(uvw_p,
// 	     dphase_p,
// 	     visdata_p,
// 	     &s.asVector()(0),
// 	     &s.asVector()(1),
// 	     gradVisAzData_p,
// 	     gradVisElData_p,
// 	     &Conj,
// 	     flags_p,
// 	     rowFlags_p,
// 	     &s.asVector()(2),
// 	     &rownr,
// 	     uvScale_p,
// 	     actualOffset_p,
// 	     dataPtr_p,
// 	     &aNx,
// 	     &aNy,
// 	     &npol,
// 	     &nchan,
// 	     vb_freq_p,
// 	     &C::c,
// 	     convSupport_p,
// 	     &actualConvSize,
// 	     &convSampling,
// 	     &wConvSize,
// 	     f_convFunc_p,
// 	     chanMap_p,
// 	     polMap_p,
// 	     &polInUse_p,
// 	     vb_ant1_p,
// 	     vb_ant2_p,
// 	     &Nant_p,
// 	     &scanNo,
// 	     &sigma,
// 	     l_off_p, m_off_p,
// 	     &area,
// 	     &doGrad,
// 	     &alwaysDoPointing,
// 	     &npa,
// 	     &paIndex_Fortran,
// 	     CFMap_p,
// 	     ConjCFMap_p,
// 	     &currentCFPA
// 	     ,&actualPA,&cfRefFreq_p
// 	     );
//     ConjCFMap.freeStorage((const Int *&)ConjCFMap_p,deleteThem(CONJCFMAP));
//     CFMap.freeStorage((const Int *&)CFMap_p,deleteThem(CFMAP));
    
//     l_off.freeStorage((const Float*&)l_off_p,deleteThem(RAOFF));
//     m_off.freeStorage((const Float*&)m_off_p,deleteThem(DECOFF));
//     uvw.freeStorage((const Double*&)uvw_p,deleteThem(UVW));
//     dphase.freeStorage((const Double*&)dphase_p,deleteThem(DPHASE));
//     visdata.putStorage(visdata_p,deleteThem(VISDATA));
//     gradVisAzData.putStorage(gradVisAzData_p,deleteThem(GRADVISAZ));
//     gradVisElData.putStorage(gradVisElData_p,deleteThem(GRADVISEL));
//     flags.freeStorage((const Int*&) flags_p,deleteThem(FLAGS));
//     rowFlags.freeStorage((const Int *&)rowFlags_p,deleteThem(ROWFLAGS));
//     actualOffset.freeStorage((const Double*&)actualOffset_p,deleteThem(ACTUALOFFSET));
//     dataPtr->freeStorage((const Complex *&)dataPtr_p,deleteThem(DATAPTR));
//     uvScale.freeStorage((const Double*&) uvScale_p,deleteThem(UVSCALE));
//     vb.frequency().freeStorage((const Double*&)vb_freq_p,deleteThem(VBFREQ));
//     cfs_p.xSupport.freeStorage((const Int*&)convSupport_p,deleteThem(CONVSUPPORT));
//     //    convFunc_p.freeStorage((const Complex *&)f_convFunc_p,deleteThem(CONVFUNC));
//     chanMap.freeStorage((const Int*&)chanMap_p,deleteThem(CHANMAP));
//     polMap.freeStorage((const Int*&) polMap_p,deleteThem(POLMAP));
//     vb.antenna1().freeStorage((const Int*&) vb_ant1_p,deleteThem(VBANT1));
//     vb.antenna2().freeStorage((const Int*&) vb_ant2_p,deleteThem(VBANT2));
//   }
//   //
//   //----------------------------------------------------------------------
//   //
//   void AWProjectWBFT::runFortranPut(Matrix<Double>& uvw,Vector<Double>& dphase,
// 				 const Complex& visdata,
// 				 IPosition& s,
// 				 Int& Conj,
// 				 Cube<Int>& flags,Vector<Int>& rowFlags,
// 				 const Matrix<Float>& weight,
// 				 Int& rownr,Vector<Double>& actualOffset,
// 				 Array<Complex>& dataPtr,
// 				 Int& aNx, Int& aNy, Int& npol, Int& nchan,
// 				 const VisBuffer& vb,Int& Nant_p, Int& scanNo,
// 				 Double& sigma,
// 				 Array<Float>& l_off,
// 				 Array<Float>& m_off,
// 				 Matrix<Double>& sumWeight,
// 				 Double& area,
// 				 Int& doGrad,
// 				 Int& doPSF,
// 				 Int paIndex)
//   {
//     (void)Conj;
//     LogIO log_l(LogOrigin("AWProjectWBFT","runFortranPut[R&D]"));
//     enum whichGetStorage {RAOFF,DECOFF,UVW,DPHASE,VISDATA,GRADVISAZ,GRADVISEL,
// 			  FLAGS,ROWFLAGS,UVSCALE,ACTUALOFFSET,DATAPTR,VBFREQ,
// 			  CONVSUPPORT,CONVWTSUPPORT,CONVFUNC,CHANMAP,POLMAP,VBANT1,VBANT2,WEIGHT,
// 			  SUMWEIGHT,CONJCFMAP,CFMAP,AVGPBPTR,CONVWTS};
//     Vector<Bool> deleteThem(25);
    
//     Double *uvw_p, *dphase_p, *actualOffset_p, *vb_freq_p, *uvScale_p;
//     Complex *dataPtr_p, *f_convFunc_p, *f_convWts_p,*avgPBPtr;
//     Int *flags_p, *rowFlags_p, *chanMap_p, *polMap_p, *convSupport_p, *convWtSupport_p,
//       *vb_ant1_p, *vb_ant2_p,
//       *ConjCFMap_p, *CFMap_p;
//     Float *l_off_p, *m_off_p;
//     Float *weight_p;Double *sumwt_p,*isumwt_p;
//     Double actualPA;
//     const Complex *visdata_p=&visdata;
    
//     Matrix<Double> iSumWt(sumWeight.shape());
//     iSumWt=0.0;
//     Vector<Int> ConjCFMap, CFMap;
//     actualPA = getVBPA(vb);

//     ConjCFMap = polMap;
//     makeCFPolMap(vb,cfStokes_p,CFMap);
//     makeConjPolMap(vb,CFMap,ConjCFMap);

//     // {
//     //   IPosition tt;
//     //   cfs_p.coordSys.list(log_l,MDoppler::RADIO,tt,tt);
//     //   cfwts_p.coordSys.list(log_l,MDoppler::RADIO,tt,tt);
//     // }
//     Array<Complex> rotatedConvFunc_l, rotatedConvWeights_l;

//     // Rotate the convolution function using Image rotation and
//     // disable rotation in the gridder
//     SynthesisUtils::rotateComplexArray(log_l, *(cfs_p.data)/*convFunc_p*/, cfs_p.coordSys,
//         			       rotatedConvFunc_l,(currentCFPA-actualPA));
//     if (!avgPBReady_p)
//       SynthesisUtils::rotateComplexArray(log_l, *(cfwts_p.data), /*convWeights_p,*/ cfwts_p.coordSys,
//     					 rotatedConvWeights_l,(currentCFPA-actualPA));
//     // Disable rotation in the gridder
//     actualPA = currentCFPA; 

//     // SynthesisUtils::rotateComplexArray(log_l, convFunc_p, cfs_p.coordSys,
//     //     			       rotatedConvFunc_l,0.0);
//     // if (!avgPBReady_p)
//     //   SynthesisUtils::rotateComplexArray(log_l, convWeights_p, cfwts_p.coordSys,
//     // 					 rotatedConvWeights_l,0.0);


//     ConjCFMap_p     = ConjCFMap.getStorage(deleteThem(CONJCFMAP));
//     CFMap_p         = CFMap.getStorage(deleteThem(CFMAP));
    
//     uvw_p           = uvw.getStorage(deleteThem(UVW));
//     dphase_p        = dphase.getStorage(deleteThem(DPHASE));
//     flags_p         = flags.getStorage(deleteThem(FLAGS));
//     rowFlags_p      = rowFlags.getStorage(deleteThem(ROWFLAGS));
//     uvScale_p       = uvScale.getStorage(deleteThem(UVSCALE));
//     actualOffset_p  = actualOffset.getStorage(deleteThem(ACTUALOFFSET));
//     dataPtr_p       = dataPtr.getStorage(deleteThem(DATAPTR));
//     vb_freq_p       = (Double *)(vb.frequency().getStorage(deleteThem(VBFREQ)));
//     convSupport_p   = cfs_p.xSupport.getStorage(deleteThem(CONVSUPPORT));
//     convWtSupport_p = cfwts_p.xSupport.getStorage(deleteThem(CONVWTSUPPORT));
//     f_convFunc_p    = rotatedConvFunc_l.getStorage(deleteThem(CONVFUNC));
//     f_convWts_p     = rotatedConvWeights_l.getStorage(deleteThem(CONVWTS));
//     chanMap_p       = chanMap.getStorage(deleteThem(CHANMAP));
//     polMap_p        = polMap.getStorage(deleteThem(POLMAP));
//     vb_ant1_p       = (Int *)(vb.antenna1().getStorage(deleteThem(VBANT1)));
//     vb_ant2_p       = (Int *)(vb.antenna2().getStorage(deleteThem(VBANT2)));
//     l_off_p         = l_off.getStorage(deleteThem(RAOFF));
//     m_off_p         = m_off.getStorage(deleteThem(DECOFF));
//     weight_p        = (Float *)(weight.getStorage(deleteThem(WEIGHT)));
//     sumwt_p         = sumWeight.getStorage(deleteThem(SUMWEIGHT));
//     Bool tmp;
//     isumwt_p        = iSumWt.getStorage(tmp);

//     //    Array<Complex> avgPB_p(griddedWeights.get());
//     Array<Complex> avgAperture;
//     if (!avgPBReady_p)
//       {
// 	avgAperture.resize(griddedWeights.shape());
// 	avgAperture.set(Complex(0,0));
// 	avgPBPtr        = avgAperture.getStorage(deleteThem(AVGPBPTR));
//       }
//     else
//       avgPBPtr=NULL;
    
//     Int npa=1,actualConvSize, actualConvWtSize;
//     Int paIndex_Fortran = paIndex; 
//     // Int doAvgPB=((avgPBReady_p==false) && 
//     //     	 ((fabs(lastPAUsedForWtImg-actualPA)*57.2956 >= DELTAPA) || 
//     //     	  (lastPAUsedForWtImg == MAGICPAVALUE)));

//     Int doAvgPB=computeAvgPB(actualPA, lastPAUsedForWtImg);//(avgPBReady_p==false);
//     //    actualConvSize = convFunc_p.shape()(0);
//     actualConvSize = cfs_p.data->shape()(0);
//     //    actualConvWtSize = convWeights_p.shape()(0);
//     actualConvWtSize = cfwts_p.data->shape()(0);

//     if (fabs(lastPAUsedForWtImg-actualPA)*57.2956 >= DELTAPA) lastPAUsedForWtImg = actualPA;

//     //    IPosition shp=convSupport.shape();
//     Int alwaysDoPointing=1;
//     alwaysDoPointing = doPointing;

//     gpbmos(uvw_p,
// 	   dphase_p,
// 	   visdata_p,
// 	   &s.asVector()(0),
// 	   &s.asVector()(1),
// 	   &doPSF,
// 	   flags_p,
// 	   rowFlags_p,
// 	   weight_p,
// 	   &s.asVector()(2),
// 	   &rownr,
// 	   uvScale_p,
// 	   actualOffset_p,
// 	   dataPtr_p,
// 	   &aNx,
// 	   &aNy,
// 	   &npol,
// 	   &nchan,
// 	   vb_freq_p,
// 	   &C::c,
// 	   convSupport_p,
// 	   &actualConvSize,
// 	   &actualConvWtSize,
// 	   &convSampling,
// 	   &wConvSize,
// 	   f_convFunc_p,
// 	   chanMap_p,
// 	   polMap_p,
// 	   &polInUse_p,
// 	   //	   sumwt_p,
// 	   isumwt_p,
// 	   vb_ant1_p,
// 	   vb_ant2_p,
// 	   &Nant_p,
// 	   &scanNo,
// 	   &sigma,
// 	   l_off_p, m_off_p,
// 	   &area,
// 	   &doGrad,
// 	   &alwaysDoPointing,
// 	   &npa,
// 	   &paIndex_Fortran,
// 	   CFMap_p,
// 	   ConjCFMap_p,
// 	   &currentCFPA,&actualPA,
// 	   &doAvgPB,
// 	   avgPBPtr,&cfRefFreq_p,
// 	   f_convWts_p,convWtSupport_p
// 	   );

//     ConjCFMap.freeStorage((const Int *&)ConjCFMap_p,deleteThem(CONJCFMAP));
//     CFMap.freeStorage((const Int *&)CFMap_p,deleteThem(CFMAP));
    
//     l_off.freeStorage((const Float*&)l_off_p,deleteThem(RAOFF));
//     m_off.freeStorage((const Float*&)m_off_p,deleteThem(DECOFF));
//     uvw.freeStorage((const Double*&)uvw_p,deleteThem(UVW));
//     dphase.freeStorage((const Double*&)dphase_p,deleteThem(DPHASE));
//     flags.freeStorage((const Int*&) flags_p,deleteThem(FLAGS));
//     rowFlags.freeStorage((const Int *&)rowFlags_p,deleteThem(ROWFLAGS));
//     actualOffset.freeStorage((const Double*&)actualOffset_p,deleteThem(ACTUALOFFSET));
//     dataPtr.freeStorage((const Complex *&)dataPtr_p,deleteThem(DATAPTR));
//     uvScale.freeStorage((const Double*&) uvScale_p,deleteThem(UVSCALE));
//     vb.frequency().freeStorage((const Double*&)vb_freq_p,deleteThem(VBFREQ));
//     cfs_p.xSupport.freeStorage((const Int*&)convSupport_p,deleteThem(CONVSUPPORT));
//     //    convFunc_p.freeStorage((const Complex *&)f_convFunc_p,deleteThem(CONVFUNC));
//     //    convWeights_p.freeStorage((const Complex *&)f_convFunc_p,deleteThem(CONVWTS));
//     chanMap.freeStorage((const Int*&)chanMap_p,deleteThem(CHANMAP));
//     polMap.freeStorage((const Int*&) polMap_p,deleteThem(POLMAP));
//     vb.antenna1().freeStorage((const Int*&) vb_ant1_p,deleteThem(VBANT1));
//     vb.antenna2().freeStorage((const Int*&) vb_ant2_p,deleteThem(VBANT2));
//     weight.freeStorage((const Float*&)weight_p,deleteThem(WEIGHT));
//     sumWeight.putStorage(sumwt_p,deleteThem(SUMWEIGHT));
//     iSumWt.putStorage(isumwt_p,tmp);
//     sumWeight += iSumWt;

//     if (!avgPBReady_p)
//       {
// 	// Get the griddedWeigths as a referenced array
// 	Array<Complex> gwts; Bool removeDegenerateAxis=false;
// 	griddedWeights.get(gwts, removeDegenerateAxis);
// 	//	griddedWeights.put(griddedWeights.get()+avgPB_p);
// 	gwts = gwts + avgAperture;
// 	// if (!reference)
// 	//   griddedWeights.put(gwts);
//       }
//   }
  //
  //---------------------------------------------------------------
  //
  // Initialize the FFT to the Sky. Here we have to setup and
  // initialize the grid.
  //
  void AWProjectWBFT::initializeToSky(ImageInterface<Complex>& iimage,
				   Matrix<Float>& weight,
				   const VisBuffer& vb)
  {
    LogIO log_l(LogOrigin("AWProjectWBFT","initializeToSky[R&D]"));
    AWProjectFT::initializeToSky(iimage,weight,vb);
    // The following code is same as that in the parent class call above. 
//     image=&iimage;
    
//     init();
//     initMaps(vb);
//     nx    = image->shape()(0);
//     ny    = image->shape()(1);
//     npol  = image->shape()(2);
//     nchan = image->shape()(3);

//     isTiled = false;

//     sumWeight=0.0;
//     weight.resize(sumWeight.shape());
//     weight=0.0;

//     if(isTiled) 
//       {
// 	imageCache->flush();
// 	image->set(Complex(0.0));
// 	//lattice=image;
// 	lattice=CountedPtr<Lattice<Complex> > (image, false);
//       }
//     else 
//       {
// 	IPosition gridShape(4, nx, ny, npol, nchan);
// 	griddedData.resize(gridShape);
// 	griddedData=Complex(0.0);
// //	if(arrayLattice) delete arrayLattice; arrayLattice=0;
// 	arrayLattice = new ArrayLattice<Complex>(griddedData);
// 	lattice=arrayLattice;
// 	visResampler_p->initializeToSky(griddedData, sumWeight);
//       }

    //AlwaysAssert(lattice, AipsError);
    if (resetPBs_p)
      {
	griddedWeights.resize(iimage.shape()); 
	griddedWeights.setCoordinateInfo(iimage.coordinates());
	griddedWeights.set(0.0);
	pbPeaks.resize(griddedWeights.shape()(2));
	pbPeaks.set(0.0);

	resetPBs_p=false;
      }
    avgPBReady_p = (cfCache_p->loadAvgPB(avgPB_p,sensitivityPatternQualifierStr_p) != CFDefs::NOTCACHED);
    //    avgPBReady_p = cfCache_p->avgPBReady(sensitivityPatternQualifierStr_p);
    // Need to grid the weighted Convolution Functions to make the sensitivity pattern.
    if (!avgPBReady_p)
      {
	// Make a copy of the re-sampler and set it up.
	if (visResamplerWt_p.null()) visResamplerWt_p = visResampler_p->clone();
	visResamplerWt_p = visResampler_p;
	visResamplerWt_p->setMaps(chanMap, polMap);
	Array<Complex> gwts; Bool removeDegenerateAxis=false;
	griddedWeights.get(gwts, removeDegenerateAxis);
	//	cerr << "initializeToSky for gwts" << endl;
	visResamplerWt_p->initializeToSky(gwts, sumCFWeight);
      }
  }
  //
  //---------------------------------------------------------------
  //
  void AWProjectWBFT::finalizeToSky()
  {
    LogIO log_l(LogOrigin("AWProjectWBFT", "finalizeToSky[R&D]"));
    AWProjectFT::finalizeToSky();
    // The following commented code is the same as in the parent class
    // call above.

    // if(isTiled) 
    //   {
    // 	AlwaysAssert(image, AipsError);
    // 	AlwaysAssert(imageCache, AipsError);
    // 	imageCache->flush();
    // 	ostringstream o;
    // 	imageCache->showCacheStatistics(o);
    // 	log_l << o.str() << LogIO::POST;
    //   }
    
    // if(pointingToImage) delete pointingToImage; pointingToImage=0;
    
    // paChangeDetector.reset();
    // cfCache_p->flush();
    // visResampler_p->finalizeToSky(griddedData, sumWeight);

    if (!avgPBReady_p) 
      {
	Array<Complex> gwts; Bool removeDegenerateAxis=false;
	griddedWeights.get(gwts, removeDegenerateAxis);
	visResamplerWt_p->finalizeToSky(gwts, sumCFWeight);
	visResamplerWt_p->releaseBuffers();
      }
    /*
    cerr << "Gridding run time = " 
    	 << " " << visResampler_p->runTimeG_p 
    	 << " " << visResampler_p->runTimeG1_p 
    	 << " " << visResampler_p->runTimeG2_p 
    	 << " " << visResampler_p->runTimeG3_p 
    	 << " " << visResampler_p->runTimeG4_p 
    	 << " " << visResampler_p->runTimeG5_p 
    	 << " " << visResampler_p->runTimeG6_p 
    	 << " " << visResampler_p->runTimeG7_p 
	 << " C " << runTime1_p 
    	 << endl;
    */
    visResampler_p->runTimeG_p=visResampler_p->runTimeG1_p=visResampler_p->runTimeG2_p=visResampler_p->runTimeG3_p=visResampler_p->runTimeG4_p=visResampler_p->runTimeG5_p=visResampler_p->runTimeG6_p=visResampler_p->runTimeG7_p=0.0;
    runTime1_p=0;
  }
  //
  //---------------------------------------------------------------
  //
  void AWProjectWBFT::resampleCFToGrid(Array<Complex>& gwts, 
				       VBStore& vbs, const VisBuffer& vb)
  {
    //
    // Grid the weighted convolution function as well
    //
    LogIO log_l(LogOrigin("AWProjectFT", "resampleCFToGrid[R&D]"));
    //
    // Now rotate and put the rotated convolution weight function
    // in rotatedCFWts_l object.
    //
    
    //	makeWBCFWt(*cfwts2_p, imRefFreq_p);
    
    timer_p.mark();
    visResamplerWt_p->copy(*visResampler_p);
    
    Vector<Double> pointingOffset(convFuncCtor_p->findPointingOffset(*image, vb));
    //cerr << "AWPWB: " << pointingOffset << endl;
    visResamplerWt_p->makeVBRow2CFMap(*cfwts2_p,*convFuncCtor_p, vb,
				      paChangeDetector.getParAngleTolerance(),
				      chanMap,polMap,pointingOffset);
    VBRow2CFBMapType& theMap=visResamplerWt_p->getVBRow2CFBMap();
    convFuncCtor_p->prepareConvFunction(vb,theMap);
    runTime1_p += timer_p.real();
    //
    // Set the uvw array to zero-sized array and dopsf=true.
    // uvw.nelements()==0 is a hint to the re-sampler to put the
    // gridded weights at the origin of the uv-grid. dopsf=true so
    // that CF*Wts are accumulated (as against CF*Wts*Vis).
    //
    // Receive the sum-of-weights in a dummy array.
    Matrix<Double> uvwOrigin;
    vbs.uvw_p.reference(uvwOrigin); 
    Bool dopsf_l=true;
    vbs.accumCFs_p=((vbs.uvw_p.nelements() == 0) && dopsf_l);
    
    // Array<Complex> gwts; Bool removeDegenerateAxis=false;
    // wtsGrid.get(gwts, removeDegenerateAxis);
    Int nDataChan = vbs.flagCube_p.shape()[1];
    
    vbs.startChan_p = 0; vbs.endChan_p = nDataChan;
    visResamplerWt_p->DataToGrid(gwts, vbs, sumCFWeight, dopsf_l); 
  }
  //
  //---------------------------------------------------------------
  //
  void AWProjectWBFT::resampleDataToGrid(Array<Complex>& griddedData_l,
					 VBStore& vbs, const VisBuffer& vb, 
					 Bool& dopsf) 
  {
    AWProjectFT::resampleDataToGrid(griddedData_l,vbs,vb,dopsf);
    if (!avgPBReady_p)
      {
	//
	// Get a reference to the pixels of griddedWeights (a
	// TempImage!)
	//
	Array<Complex> gwts; Bool removeDegenerateAxis=false;
	griddedWeights.get(gwts, removeDegenerateAxis);
	resampleCFToGrid(gwts, vbs, vb);
      }
  };
  //
  //---------------------------------------------------------------
  //
  void AWProjectWBFT::resampleDataToGrid(Array<DComplex>& griddedData_l,
					 VBStore& vbs, const VisBuffer& vb, 
					 Bool& dopsf) 
  {
    AWProjectFT::resampleDataToGrid(griddedData_l,vbs,vb,dopsf);
    if (!avgPBReady_p)
      {
	//
	// Get a reference to the pixels of griddedWeights (a
	// TempImage!)
	//
	Array<Complex> gwts; Bool removeDegenerateAxis=false;
	griddedWeights.get(gwts, removeDegenerateAxis);
	resampleCFToGrid(gwts, vbs, vb);
      }
  };
  //  void AWProjectWBFT::resampleGridToData(VBStore& vbs, const VisBuffer& vb) {};
  
  void AWProjectWBFT::setCFCache(CountedPtr<CFCache>& cfc, const Bool resetCFC) 
  {
    if (resetCFC) cfCache_p = cfc;
    if (!cfCache_p.null())
      {
	cfs2_p = CountedPtr<CFStore2>(&cfCache_p->memCache2_p[0],false);//new CFStore2;
	cfwts2_p =  CountedPtr<CFStore2>(&cfCache_p->memCacheWt2_p[0],false);//new CFStore2;
	
	// cfCache_p->summarize(cfCache_p->memCache2_p,String("New CFC"));
	// cfCache_p->summarize(cfCache_p->memCacheWt2_p,String(""));
	avgPBReady_p=false;
      }
  }

} //# NAMESPACE CASA - END
