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

#include <synthesis/TransformMachines2/VB2CFBMap.h>
#include <synthesis/TransformMachines2/AWProjectWBFT.h>
#include <synthesis/TransformMachines2/AWVisResampler.h>
#include <synthesis/TransformMachines/StokesImageUtil.h>
#include <casacore/coordinates/Coordinates/CoordinateSystem.h>
#include <casacore/scimath/Mathematics/FFTServer.h>
#include <casacore/scimath/Mathematics/Convolver.h>
#include <casacore/lattices/LatticeMath/LatticeFFT.h>
#include <casacore/images/Images/ImageInterface.h>
#include <casacore/images/Images/PagedImage.h>
#include <msvis/MSVis/VisBuffer2.h>
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
  namespace refim{
    //==================================================================================================
    // Various template instantiations
    //
    template
    void AWProjectWBFT::ftWeightImage(Lattice<casacore::Complex>& wtImage, 
				      const Matrix<Float>& sumWt,
				      const Bool& doFFTNorm);
    template
    void AWProjectWBFT::makeWBSensitivityImage(Lattice<casacore::Complex>& wtImage,
					     ImageInterface<Float>& sensitivityImage,
					     const Matrix<Float>& sumWt,
					     const Bool& doFFTNorm);
    //==================================================================================================
    template
    void AWProjectWBFT::ftWeightImage(Lattice<casacore::DComplex>& wtImage, 
				      const Matrix<Float>& sumWt,
				      const Bool& doFFTNorm);
    template
    void AWProjectWBFT::makeWBSensitivityImage(Lattice<casacore::DComplex>& wtImage,
					     ImageInterface<Float>& sensitivityImage,
					     const Matrix<Float>& sumWt,
					     const Bool& doFFTNorm);
    //==================================================================================================
    template 
    void AWProjectWBFT::resampleCFToGrid(Array<casacore::Complex>& gwts, 
					 VBStore& vbs, const VisBuffer2& vb);
    template 
    void AWProjectWBFT::resampleCFToGrid(Array<casacore::DComplex>& gwts, 
					 VBStore& vbs, const VisBuffer2& vb);
    //==================================================================================================

  //
  //---------------------------------------------------------------
  //
  AWProjectWBFT::AWProjectWBFT(Int nWPlanes, Long icachesize, 
			       CountedPtr<CFCache>& cfcache,
			       CountedPtr<ConvolutionFunction>& cf,
			       CountedPtr<VisibilityResamplerBase>& visResampler,
			       Bool applyPointingOffset,
			       vector<float> pointingOffsetSigDev,
			       Bool doPBCorr,
			       Int itilesize, 
			       Float paSteps,
			       Float pbLimit,
			       Bool usezero,
			       Bool conjBeams,
			       Bool doublePrecGrid)
    : AWProjectFT(nWPlanes,icachesize,cfcache,cf,visResampler,applyPointingOffset,pointingOffsetSigDev,doPBCorr, itilesize,pbLimit,usezero,conjBeams,doublePrecGrid),
      //avgPBReady_p(false),
      resetPBs_p(true),wtImageFTDone_p(false),fieldIds_p(0),rotatedCFWts_p(),visResamplerWt_p(),oneTimeMessage_p(false)
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
    LogIO log_l(LogOrigin("AWProjectWBFT2", "AWProjectWBFT[R&D]"));
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
	//	avgPBReady_p    =   other.avgPBReady_p;
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
  Int AWProjectWBFT::findPointingOffsets(const VisBuffer2& vb, 
					 Array<Float> &l_off,
					 Array<Float> &m_off,
					 Bool Evaluate)
  {
    LogIO log_l(LogOrigin("AWProjectWBFT2","findPointingOffsets[R&D]"));
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
	NAnt=vb.subtableColumns().antenna().nrow();
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
    LogIO log_l(LogOrigin("AWProjectWBFT2","normalizeAvgPB[R&D]"));
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
  void AWProjectWBFT::makeSensitivityImage(const VisBuffer2&,
					   const ImageInterface<Complex>& /*imageTemplate*/,
					   ImageInterface<Float>& /*sensitivityImage*/)
  {
    if (avgPBReady_p == false)
      {
	if (oneTimeMessage_p == false)
	  {
	    LogIO log_l(LogOrigin("AWProjectWBFT2", "makeSensitivityImage(Complex)[R&D]"));
	    log_l << "Setting up for weights accumulation ";
	    if (sensitivityPatternQualifierStr_p != "") log_l << "for term " << sensitivityPatternQualifier_p << " ";
	    log_l << "during gridding to compute sensitivity pattern." 
		  << endl
		  << "Consequently, the first gridding cycle will be slower than the subsequent ones." 
		  << LogIO::WARN;
	    oneTimeMessage_p=true;
	  }
      }
  }
  void AWProjectWBFT::makeSensitivityImage(const VisBuffer2&,
					   const ImageInterface<DComplex>& /*imageTemplate*/,
					   ImageInterface<Float>& /*sensitivityImage*/)
  {
    if (avgPBReady_p == false)
      {
	if (oneTimeMessage_p == false)
	  {
	    LogIO log_l(LogOrigin("AWProjectWBFT2", "makeSensitivityImage(DComplex)[R&D]"));
	    log_l << "Setting up for weights accumulation ";
	    if (sensitivityPatternQualifierStr_p != "") log_l << "for term " << sensitivityPatternQualifier_p << " ";
	    log_l << "during gridding to compute sensitivity pattern." 
		  << endl
		  << "Consequently, the first gridding cycle will be slower than the subsequent ones." 
		  << LogIO::WARN;
	    oneTimeMessage_p=true;
	  }
      }
  }
  //
  //---------------------------------------------------------------
  // 
    template <class T>
  void AWProjectWBFT::ftWeightImage(Lattice<T>& wtImage, 
				    const Matrix<Float>& sumWt,
				    const Bool& doFFTNorm)
  {
    LogIO log_l(LogOrigin("AWProjectWBFT2", "ftWeightImage[R&D]"));
    if (wtImageFTDone_p) return;

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
    wtImageFTDone_p=true;

    Int sizeX=wtImage.shape()(0), sizeY=wtImage.shape()(1);

    Array<T> wtBuf; wtImage.get(wtBuf,false);
    ArrayLattice<T> wtLat(wtBuf,true);
    //
    // Copy one 2D plane at a time, normalizing by the sum of weights
    // and possibly 2D FFT.
    //
    // Set up Lattice iterators on wtImage and sensitivityImage
    //
    IPosition axisPath(4, 0, 1, 2, 3);
    IPosition cursorShape(4, sizeX, sizeY, 1, 1);

    LatticeStepper wtImStepper(wtImage.shape(), cursorShape, axisPath);
    LatticeIterator<T> wtImIter(wtImage, wtImStepper);
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
    //    log_l << "SumCFWt: " << getSumOfCFWeights() << " " << max(wtBuf) << " " << sensitivityPatternQualifier_p <<LogIO::WARN << LogIO::POST;
    for(wtImIter.reset(); !wtImIter.atEnd(); wtImIter++)
      {
	wtImIter.rwCursor() = (wtImIter.rwCursor()
			       *Float(sizeX)*Float(sizeY)
			       );
      }
  }
  //
  //---------------------------------------------------------------
  // 
    template <class T>
  void AWProjectWBFT::makeWBSensitivityImage(Lattice<T>& wtImage,
					   ImageInterface<Float>& sensitivityImage,
					   const Matrix<Float>& sumWt,
					   const Bool& doFFTNorm)
  {
    if (avgPBReady_p) return;
    LogIO log_l(LogOrigin("AWProjectWBFT2", "makeSensitivityImage[R&D]"));

    ftWeightImage(wtImage, sumWt, doFFTNorm);

    if (useDoubleGrid_p)
      {
	sensitivityImage.resize(griddedWeights_D.shape()); 
	sensitivityImage.setCoordinateInfo(griddedWeights_D.coordinates());
      }
    else
      {
	sensitivityImage.resize(griddedWeights.shape()); 
	sensitivityImage.setCoordinateInfo(griddedWeights.coordinates());
      }

    //
    // Rest of the code below is to (1) average the poln planes of the
    // weight image for each freq. channel, and (2) copy the averaged
    // value to the all the poln planes for each freq. channels of the
    // sensitivity image.  
    //
    // Set up Lattice iterator on wtImage for averaging all poln
    // planes and writing the result the only the first poln plane.
    //
    IPosition axisPath(2, 2, 3); // Step through the Poln (2) and Freq (3) axis.
    IPosition cursorShapeWt(4, wtImage.shape()(0), wtImage.shape()(1), 1, 1);

    LatticeStepper wtImStepper(wtImage.shape(), cursorShapeWt, axisPath);
    LatticeIterator<T> wtImIter(wtImage, wtImStepper);
    int nPolPlanes=wtImage.shape()(2);
    // First average all poln planes and write the result back to the first poln plane.
    for(wtImIter.reset(); !wtImIter.atEnd(); /*increment is inside the for-loop below*/)
    {
      Matrix<T> tmpWt(wtImIter.rwMatrixCursor().shape());
      ArrayLattice<T> tt(tmpWt,true);
      for(int i=0;i<nPolPlanes;i++)
	{
	  //	  cerr << "WT0: " << i << " " << wtImIter.position() << endl;
	  Matrix<T> tmp0_ref;tmp0_ref.reference(wtImIter.rwMatrixCursor().nonDegenerate());
	  ArrayLattice<T> p0(tmp0_ref,true); //Make ArrayLattice from Array by refence
	  LatticeExpr<T> le(tt+p0);
	  tt.copyData(le); 
	  wtImIter++;
	}
      LatticeExpr<T> le(tt/nPolPlanes);
      tt.copyData(le); 
    }
    
    IPosition cursorShape(4, sensitivityImage.shape()(0), sensitivityImage.shape()(1), 1, 1);
    LatticeStepper senImStepper(sensitivityImage.shape(), cursorShape, axisPath);
    LatticeIterator<Float> senImIter(sensitivityImage, senImStepper);
    //
    // Copy the real part of the average of all planes of the wtImage
    // to all the planes of the sensitivity image (senImage).
    //
    // IPosition cShape(4,wtImage.shape()(0), wtImage.shape()(1),1,1);
    // LatticeStepper YAwtImStepper(wtImage.shape(), cursorShapeWt, axisPath);
    // LatticeIterator<T> YAwtImIter(wtImage, YAwtImStepper);
    //
    for(wtImIter.reset(),senImIter.reset(); !wtImIter.atEnd();/*increment is inside the first for-loop below*/)
      {
	// Extract the first polarization plane from the weight image. 
	Matrix<T> wt_ref;wt_ref.reference(wtImIter.rwMatrixCursor().nonDegenerate());
	ArrayLattice<T> p0(wt_ref,true);
	for (int i=0;i<nPolPlanes;i++) wtImIter++; // Skip the rest of the poln planes

	for(int i=0;i<sensitivityImage.shape()(2);i++)
	  {
	    // // Now replace the polarization planes with their average.  REVIEW THIS FOR FM CASE.
	    Matrix<Float> tmp;tmp.reference(senImIter.rwMatrixCursor().nonDegenerate());
	    ArrayLattice<Float> tt(tmp,true); //Make ArrayLattice from Array by refence
	    LatticeExpr<Float> le(abs(p0));
	    tt.copyData(le); 

	    // Copy values to the sensitivty image, do type conversion if necessary
	    // convertArray(senImIter.rwMatrixCursor(),real(tmp));
	    //	    cerr << "SEN: " << senImIter.position() << " " << tmp.shape() << endl;
	    senImIter++;
	  }
      }
    // {
    //   String name("ftwtimg.im");
    //   storeArrayAsImage(name,griddedWeights_D.coordinates(),sensitivityImage.get());
    // }

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
    LogIO log_l(LogOrigin("AWProjectWBFT2", "getImage[R&D]"));

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
    if (!avgPB_p.null())
      {
	if (useDoubleGrid_p)
	  makeWBSensitivityImage(griddedWeights_D, *avgPB_p, weights, true);
	else
	  makeWBSensitivityImage(griddedWeights, *avgPB_p, weights, true);
      }
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
  //
  //---------------------------------------------------------------
  //
  // Initialize the FFT to the Sky. Here we have to setup and
  // initialize the grid.
  //
  void AWProjectWBFT::initializeToSky(ImageInterface<Complex>& iimage,
				   Matrix<Float>& weight,
				   const VisBuffer2& vb)
  {
    LogIO log_l(LogOrigin("AWProjectWBFT2","initializeToSky[R&D]"));
    AWProjectFT::initializeToSky(iimage,weight,vb);

    if (resetPBs_p)
      {
	if (useDoubleGrid_p)
	  {
	    griddedWeights_D.resize(iimage.shape()); 
	    griddedWeights_D.setCoordinateInfo(iimage.coordinates());
	    griddedWeights_D.set(0.0);
	    pbPeaks.resize(griddedWeights_D.shape()(2));
	    pbPeaks.set(0.0);
	  }
	else
	  {
	    griddedWeights.resize(iimage.shape()); 
	    griddedWeights.setCoordinateInfo(iimage.coordinates());
	    griddedWeights.set(0.0);
	    pbPeaks.resize(griddedWeights.shape()(2));
	    pbPeaks.set(0.0);
	  }

	resetPBs_p=false;
      }

    std::tuple<int, double>cubeinfo(1,-1.0);    
    double freqofBegChan;
    spectralCoord_p.toWorld(freqofBegChan, 0.0);
        
    cubeinfo=std::make_tuple(iimage.shape()(3),freqofBegChan);
    if (!avgPBReady_p)
      avgPBReady_p = (cfCache_p->loadAvgPB(avgPB_p,sensitivityPatternQualifierStr_p, cubeinfo) != CFDefs::NOTCACHED);
    
    // Need to grid the weighted Convolution Functions to make the sensitivity pattern.
    if (!avgPBReady_p)
      {
	// Make a copy of the re-sampler and set it up.
	if (visResamplerWt_p.null()) visResamplerWt_p = visResampler_p->clone();
	visResamplerWt_p = visResampler_p;
	visResamplerWt_p->setMaps(chanMap, polMap);
	if (useDoubleGrid_p)
	  {
	    Array<DComplex> gwts; Bool removeDegenerateAxis=false;
	    griddedWeights_D.get(gwts, removeDegenerateAxis);
	    visResamplerWt_p->initializeToSky(gwts, sumCFWeight); //A NoOp right now.
	  }
	else
	  {
	    Array<Complex> gwts; Bool removeDegenerateAxis=false;
	    griddedWeights.get(gwts, removeDegenerateAxis);
	    visResamplerWt_p->initializeToSky(gwts, sumCFWeight); //A NoOp right now.
	  }
      }
  }
  //
  //---------------------------------------------------------------
  //
  void AWProjectWBFT::finalizeToSky()
  {
    LogIO log_l(LogOrigin("AWProjectWBFT2", "finalizeToSky[R&D]"));
    AWProjectFT::finalizeToSky();
    // The following commented code is the same as in the parent class
    // call above.

    if (!avgPBReady_p) 
      {
	if (useDoubleGrid_p)
	  {
	    Array<DComplex> gwts; Bool removeDegenerateAxis=false;
	    griddedWeights_D.get(gwts, removeDegenerateAxis);
	    visResamplerWt_p->finalizeToSky(gwts, sumCFWeight);
	    visResamplerWt_p->releaseBuffers();
	  }
	else
	  {
	    Array<Complex> gwts; Bool removeDegenerateAxis=false;
	    griddedWeights.get(gwts, removeDegenerateAxis);
	    visResamplerWt_p->finalizeToSky(gwts, sumCFWeight);
	    visResamplerWt_p->releaseBuffers();
	  }
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
    template <class T>
  void AWProjectWBFT::resampleCFToGrid(Array<T>& gwts, 
				       VBStore& vbs, const VisBuffer2& vb)
  {
    //
    // Grid the weighted convolution function as well
    //
    //LogIO log_l(LogOrigin("AWProjectFT2", "resampleCFToGrid[R&D]"));
    //
    // Now rotate and put the rotated convolution weight function
    // in rotatedCFWts_l object.
    //
    
    //	makeWBCFWt(*cfwts2_p, imRefFreq_p);
    
    //timer_p.mark();
    visResamplerWt_p->copy(*visResampler_p);
    
    po_p->fetchPointingOffset(*image, vb, doPointing);
    //cerr << "AWPWB: " << pointingOffset << endl;
    // visResamplerWt_p->makeVBRow2CFBMap(*cfwts2_p,*convFuncCtor_p, vb,
    // 				      paChangeDetector.getParAngleTolerance(),
    // 				      chanMap,polMap,pointingOffset);
    //    VB2CFBMap& theMap=visResamplerWt_p->getVBRow2CFBMap();
    vb2CFBMap_p->makeVBRow2CFBMap(*cfwts2_p,vb,
				      paChangeDetector.getParAngleTolerance(),
				      chanMap,polMap,po_p);
    convFuncCtor_p->prepareConvFunction(vb,*vb2CFBMap_p);
    //runTime1_p += timer_p.real();
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
    //    cerr << "uvw_p.nelements " << vbs.uvw_p.nelements() << endl;
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
					 VBStore& vbs, const VisBuffer2& vb, 
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
					 VBStore& vbs, const VisBuffer2& vb, 
					 Bool& dopsf) 
  {
    AWProjectFT::resampleDataToGrid(griddedData_l,vbs,vb,dopsf);
    if (!avgPBReady_p)
      {
	//
	// Get a reference to the pixels of griddedWeights (a
	// TempImage!)
	//
	Array<DComplex> gwts; Bool removeDegenerateAxis=false;
	griddedWeights_D.get(gwts, removeDegenerateAxis);
	resampleCFToGrid(gwts, vbs, vb);
      }
  };
    
    ////-----------------------------------------------------------------

 void  AWProjectWBFT::gridImgWeights(const VisBuffer2& vb)
 {
   if(avgPBReady_p)
     return;
   try
     {
       findConvFunction(*image, vb);
     }
   catch(AipsError& x)
     {
       LogIO log_l(LogOrigin("AWProjectFT2", "put[R&D]"));
       log_l << x.getMesg() << LogIO::WARN;
       return;
     }
   Matrix<Float> imagingweight;
   getImagingWeight(imagingweight, vb);
   matchChannel(vb);
   Cube<Complex> data;
   //Fortran gridder need the flag as ints 
   Cube<Int> flags;
   Matrix<Float> elWeight;
   
   interpolateFrequencyTogrid(vb, imagingweight,data, flags , elWeight, FTMachine::PSF);
   Matrix<Double> uvw(negateUV(vb));
   Vector<Double> dphase(vb.nRows());
   dphase=0.0;
   doUVWRotation_p=true;
   //rotateUVW(uvw, dphase, vb);
   //girarUVW(uvw, dphase, vb);
   //refocus(uvw, vb.antenna1(), vb.antenna2(), dphase, vb);
   VBStore vbs;
   Vector<Int> gridShape = griddedData2.shape().asVector();
   setupVBStore(vbs,vb, elWeight,data,uvw,flags, dphase, True /*dopsf*/,gridShape);
   if (useDoubleGrid_p){
     	Array<DComplex> gwts; Bool removeDegenerateAxis=false;
	griddedWeights_D.get(gwts, removeDegenerateAxis);
	resampleCFToGrid(gwts, vbs, vb);
        
    }
   else{
     Array<Complex> gwts; Bool removeDegenerateAxis=false;
     griddedWeights.get(gwts, removeDegenerateAxis);
     resampleCFToGrid(gwts, vbs, vb);
     
   }
 }
    
  
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

} //# NAMESPACE refim - END
} //# NAMESPACE CASA - END

  //
  //---------------------------------------------------------------
  // R&D code, most should ignore code below this line.
  //
  // void AWProjectWBFT::makeSensitivitySqImage(Lattice<Complex>& wtImage,
  // 					     ImageInterface<Complex>& sensitivitySqImage,
  // 					     const Matrix<Float>& sumWt,
  // 					     const Bool& doFFTNorm)
  // {
  //   //    if (avgPBReady_p) return;
  //   LogIO log_l(LogOrigin("AWProjectWBFT2", "makeSensitivitySqImage[R&D]"));

  //   //    avgPBReady_p=true;

  //   ftWeightImage(wtImage, sumWt, doFFTNorm);

  //   if (useDoubleGrid_p)
  //     {
  // 	sensitivitySqImage.resize(griddedWeights_D.shape()); 
  // 	sensitivitySqImage.setCoordinateInfo(griddedWeights_D.coordinates());
  //     }
  //   else
  //     {
  // 	sensitivitySqImage.resize(griddedWeights.shape()); 
  // 	sensitivitySqImage.setCoordinateInfo(griddedWeights.coordinates());
  //     }

  //   Int sizeX=wtImage.shape()(0), sizeY=wtImage.shape()(1);
  //   Array<Complex> senSqBuf; sensitivitySqImage.get(senSqBuf,false); 
  //   ArrayLattice<Complex> senSqLat(senSqBuf, true);

  //   Array<Complex> wtBuf; wtImage.get(wtBuf,false);
  //   ArrayLattice<Complex> wtLat(wtBuf,true);
  //   //
  //   // Copy one 2D plane at a time, normalizing by the sum of weights
  //   // and possibly 2D FFT.
  //   //
  //   // Set up Lattice iterators on wtImage and sensitivityImage
  //   //
  //   IPosition axisPath(4, 0, 1, 2, 3);
  //   IPosition cursorShape(4, sizeX, sizeY, 1, 1);

  //   LatticeStepper wtImStepper(wtImage.shape(), cursorShape, axisPath);
  //   LatticeIterator<Complex> wtImIter(wtImage, wtImStepper);

  //   LatticeStepper senSqImStepper(senSqLat.shape(), cursorShape, axisPath);
  //   LatticeIterator<Complex> senSqImIter(senSqLat, senSqImStepper);
  //   //
  //   // Iterate over channel and polarization axis
  //   //
  //   //
  //   // The following code is averaging RR and LL planes and writing
  //   // the result to back to both planes.  This needs to be
  //   // generalized for full-pol case.
  //   //
  //   IPosition start0(4,0,0,0,0), start1(4,0,0,1,0), length(4,sizeX,sizeY,1,1);
  //   Slicer slicePol0(start0,length), slicePol1(start1,length);
  //   Array<Complex> polPlane0C, polPlane1C, polPlaneSq0C, polPlaneSq1C;

  //   senSqLat.getSlice(polPlaneSq0C, slicePol0);
  //   senSqLat.getSlice(polPlaneSq1C,slicePol1);
  //   wtLat.getSlice(polPlane0C, slicePol0);
  //   wtLat.getSlice(polPlane1C, slicePol1);

  //   polPlaneSq0C = polPlane0C*polPlane1C;
  //   polPlaneSq1C = polPlaneSq0C;

  //   pbNormalized_p=false;

  //   resetPBs_p=false;
  //   // String name("avgPBSq.im");
  //   // storeImg(name,sensitivitySqImage);
  // }

