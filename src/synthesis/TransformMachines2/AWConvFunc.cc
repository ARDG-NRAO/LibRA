// -*- C++ -*-
//# AWConvFunc.cc: Implementation of the AWConvFunc class
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
//
#include <synthesis/TransformMachines2/Utils.h>

#include <synthesis/TransformMachines2/AWConvFunc.h>
#include <synthesis/TransformMachines2/AWProjectFT.h>
#include <synthesis/TransformMachines/SynthesisError.h>
#include <casacore/images/Images/ImageInterface.h>
#include <synthesis/TransformMachines2/Utils.h>
#include <synthesis/TransformMachines/BeamCalc.h>
#include <synthesis/TransformMachines2/CFStore.h>
#include <synthesis/TransformMachines2/CFStore2.h>
#include <synthesis/TransformMachines2/VB2CFBMap.h>
#include <synthesis/TransformMachines2/PSTerm.h>
#include <synthesis/TransformMachines2/WTerm.h>
#include <synthesis/TransformMachines2/ATerm.h>
#include <synthesis/TransformMachines2/VLACalcIlluminationConvFunc.h>
#include <synthesis/TransformMachines2/ConvolutionFunction.h>
#include <synthesis/TransformMachines2/PolOuterProduct.h>
#include <synthesis/TransformMachines2/ImageInformation.h>
#include <casacore/coordinates/Coordinates/DirectionCoordinate.h>
#include <casacore/coordinates/Coordinates/LinearCoordinate.h>
#include <casacore/coordinates/Coordinates/SpectralCoordinate.h>
#include <casacore/coordinates/Coordinates/StokesCoordinate.h>
#include <casacore/casa/System/ProgressMeter.h>
#include <casacore/lattices/LatticeMath/LatticeFFT.h>
#include <casacore/casa/Utilities/CompositeNumber.h>
#include <casacore/casa/OS/Directory.h>
#include <casacore/casa/OS/Timer.h>
#include <ostream>
#ifdef _OPENMP
#include <omp.h>
#endif

#define MAX_FREQ 1e30

using namespace casacore;
namespace casa{
  namespace refim{
using namespace casacore;
using namespace casa::vi;

AWConvFunc::AWConvFunc(const casacore::CountedPtr<ATerm> aTerm,
			 const casacore::CountedPtr<PSTerm> psTerm,
			 const casacore::CountedPtr<WTerm> wTerm,
			 const casacore::Bool wbAWP,
			 const casacore::Bool conjPB):
    ConvolutionFunction(),aTerm_p(aTerm),psTerm_p(psTerm), wTerm_p(wTerm), pixFieldGrad_p(),
    wbAWP_p(wbAWP), conjPB_p(conjPB), baseCFB_p()
  {
    if (psTerm->isNoOp() && aTerm->isNoOp())
      {
	LogIO log_l(LogOrigin("AWConvFunc", "AWConvFunc"));
	log_l << "Both, psterm and aterm cannot be set to NoOp. " << LogIO::EXCEPTION;
      }
    if (wbAWP && aTerm->isNoOp()) wbAWP_p=false;

    pixFieldGrad_p.resize(2);pixFieldGrad_p(0)=0.0; pixFieldGrad_p(1)=0.0;
  }

  //
  //----------------------------------------------------------------------
  //
  AWConvFunc& AWConvFunc::operator=(const AWConvFunc& other)
  {
    if(this!=&other)
      {
	aTerm_p = other.aTerm_p;
	psTerm_p = other.psTerm_p;
	wTerm_p = other.wTerm_p;
      }
    return *this;

  }
  //
  //----------------------------------------------------------------------
  //
  void AWConvFunc::makePBSq(ImageInterface<Complex>& PB)
  {
    IPosition pbShape=PB.shape();
    IPosition cursorShape(4, pbShape(0), pbShape(1), 1, 1), axisPath(4,0,1,2,3);
    Array<Complex> buf; PB.get(buf,false);
    ArrayLattice<Complex> lat(buf, true);
    LatticeStepper latStepper(lat.shape(), cursorShape,axisPath);
    LatticeIterator<Complex> latIter(lat, latStepper);

    IPosition start0(4,0,0,0,0), start1(4,0,0,1,0), length(4, pbShape(0), pbShape(1),1,1);
    Slicer slicePol0(start0, length), slicePol1(start1, length);
    if (pbShape(2) > 1)
      {
	Array<Complex> pol0, pol1,tmp;

	lat.getSlice(pol0, slicePol0);
	lat.getSlice(pol1, slicePol1);
	tmp = pol0;
	pol0 = pol0*conj(pol1);
	pol1 = tmp*conj(pol1);
	lat.putSlice(pol0,start0);
	lat.putSlice(pol1,start1);
      }
    else
      {
	buf = buf * conj(buf);
      }
  }
  //
  //----------------------------------------------------------------------
  //
  void AWConvFunc::makeConjPolAxis(CoordinateSystem& cs,
				   Int conjStokes_in)
  {
    //LogIO log_l(LogOrigin("AWConvFunc2", "makeConjPolAxis[R&D]"));
    IPosition dummy;
    Vector<String> csList;
    Vector<Int> stokes, conjStokes;

    Int stokesIndex=cs.findCoordinate(Coordinate::STOKES);
    StokesCoordinate sc=cs.stokesCoordinate(stokesIndex);

    if (conjStokes_in == -1)
      {
	stokes=sc.stokes();
	conjStokes.resize(stokes.shape());
	for (uInt i=0; i<stokes.nelements(); i++)
	  {
	    if (stokes(i) == Stokes::RR) conjStokes(i) = Stokes::LL;
	    if (stokes(i) == Stokes::LL) conjStokes(i) = Stokes::RR;
	    if (stokes(i) == Stokes::LR) conjStokes(i) = Stokes::RL;
	    if (stokes(i) == Stokes::RL) conjStokes(i) = Stokes::LR;

	    if (stokes(i) == Stokes::XX) conjStokes(i) = Stokes::YY;
	    if (stokes(i) == Stokes::YY) conjStokes(i) = Stokes::XX;
	    if (stokes(i) == Stokes::YX) conjStokes(i) = Stokes::XY;
	    if (stokes(i) == Stokes::XY) conjStokes(i) = Stokes::YX;
	  }
      }
    else
      {
	conjStokes.resize(1);
	conjStokes[0]=conjStokes_in;
      }
    sc.setStokes(conjStokes);
    cs.replaceCoordinate(sc,stokesIndex);
  }
  //
  //----------------------------------------------------------------------
  //
  void AWConvFunc::fillConvFuncBuffer(CFBuffer& cfb, CFBuffer& cfWtb,
				      const Int&,// skyNX,
				      const Int&,// skyNY,
				      const Vector<Double>&,// skyIncr,
				      const Int& nx, const Int& ny,
				      const Vector<Double>& freqValues,
				      const Vector<Double>& wValues,
				      const Double& wScale,
				      const Double& vbPA, const Double& freqHi,
				      const PolMapType& muellerElements,
				      const PolMapType& muellerElementsIndex,
				      const VisBuffer2& vb,
				      const Float& psScale,
				      PSTerm& psTerm, WTerm& wTerm, ATerm& aTerm,
				      Bool isDryRun)
  {
    // Unused variable from the dark-ages era interface that should ultimately go.
    (void)psScale;
    (void)muellerElementsIndex;
    (void)freqHi;
    //    Int ttt=0;
    Complex cfNorm, cfWtNorm;
    //Double vbPA = getPA(vb);
    Complex cpeak,wtcpeak;
    aTerm.cacheVBInfo(vb);
    Int totalCFs=muellerElements.shape().product()*freqValues.shape().product()*wValues.shape().product()*2,
      cfsDone=0;

    ProgressMeter pm(1.0, Double(totalCFs), "fillCF", "","","",true);

    for (uInt imx=0;imx<muellerElements.nelements();imx++) // Loop over all MuellerElements
      for (uInt imy=0;imy<muellerElements(imx).nelements();imy++)
    	{
	  {
	    for (uInt inu=0;inu<freqValues.nelements();inu++) // All freq. channels
	      {
		Float sampling, samplingWt;
		Int xSupport, ySupport, xSupportWt, ySupportWt;
		CoordinateSystem cs_l;
		String bandName;
		// Extract the parameters index by (MuellerElement, Freq, W)
		cfWtb.getParams(cs_l, samplingWt, xSupportWt, ySupportWt, bandName,
				freqValues(inu),
				//				wValues(iw),
				wValues(0),
				muellerElements(imx)(imy));
		cfb.getParams(cs_l, sampling, xSupport, ySupport, bandName,
			      freqValues(inu),
			      wValues(0),
			      muellerElements(imx)(imy));
		aTerm.setBandName(bandName);

		IPosition pbshp(4,nx, ny,1,1);
		// Set the shape to 2x2 pixel images for dry gridding
		if (isDryRun) pbshp[0]=pbshp[1]=2;

		//
		// Cache the A-Term for this polarization and frequency
		//
		Double conjFreq=SynthesisUtils::conjFreq(freqValues(inu),imRefFreq_p);
		Int conjFreqIndex;
		conjFreq=SynthesisUtils::nearestValue(freqValues, conjFreq, conjFreqIndex);

		// USEFUL DEBUG MESSAGE
		// cerr<<"Muller Array = "<<muellerElements(imx)(imy)<<"\n" ;
		// cerr << "Freq. values: "
		//      << freqValues(inu) << " "
		//      << imRefFreq_p << " "
		//      << conjFreq << " "
		//      << endl;

		CoordinateSystem conjPolCS_l=cs_l;  AWConvFunc::makeConjPolAxis(conjPolCS_l);
		TempImage<Complex> ftATerm_l(pbshp, cs_l), ftATermSq_l(pbshp,conjPolCS_l);
		Int index;
		Vector<Int> conjPol;
		index = conjPolCS_l.findCoordinate(Coordinate::STOKES);
		conjPol = conjPolCS_l.stokesCoordinate(index).stokes();

		Bool doSquint=true;
		ftATerm_l.set(Complex(1.0,0.0));   ftATermSq_l.set(Complex(1.0,0.0));

		Int me=muellerElements(imx)(imy);
		if (!isDryRun)
		  {
		    aTerm.applySky(ftATerm_l, vb, doSquint, 0, me, freqValues(inu));//freqHi);
		    if (conjPB_p) aTerm.applySky(ftATermSq_l, vb, doSquint, 0,me,conjFreq);
		    else aTerm.applySky(ftATermSq_l, vb, doSquint, 0,me,freqValues(inu));
		  }


		Int directionIndex=cs_l.findCoordinate(Coordinate::DIRECTION);
		DirectionCoordinate dc=cs_l.directionCoordinate(directionIndex);
		Vector<Double> cellSize;
		cellSize = dc.increment();

		//
		// Now compute the PS x W-Term and apply the cached
		// A-Term to build the full CF.
		//
    		for (uInt iw=0;iw<wValues.nelements();iw++)     // All w-planes
    		  {
		    if (!isDryRun)
		      {
			LogIO log_l(LogOrigin("AWConvFunc2", "fillConvFuncBuffer[R&D]"));

			log_l << " CF("
			      << "M:"<<muellerElements(imx)(imy)
			      << ",C:" << inu
			      << ",W:" << iw << "): ";
		      }

    		    Array<Complex> &cfWtBuf=(*(cfWtb.getCFCellPtr(freqValues(inu), wValues(iw),
								  muellerElements(imx)(imy))->storage_p));
		    Array<Complex> &cfBuf=(*(cfb.getCFCellPtr(freqValues(inu), wValues(iw),
							      muellerElements(imx)(imy))->storage_p));
    		    // IPosition cfWtBufShape= cfWtb.getCFCellPtr(freqValues(inu), wValues(iw),
		    // 					       muellerElements(imx)(imy))->shape_p;
		    // IPosition cfBufShape=cfb.getCFCellPtr(freqValues(inu), wValues(iw),
		    // 					  muellerElements(imx)(imy))->shape_p;

		    cfWtBuf.resize(pbshp);
		    cfBuf.resize(pbshp);

		    const Vector<Double> sampling_l(2,sampling);
		    //		    Double wval = wValues[iw];
		    Matrix<Complex> cfBufMat(cfBuf.nonDegenerate()),
		      cfWtBufMat(cfWtBuf.nonDegenerate());
		    //
		    // Apply the Prolate Spheroidal and W-Term kernels
		    //

		    Vector<Double> s(2); s=sampling;

		    if (psTerm.isNoOp() || isDryRun)
		      cfBufMat = cfWtBufMat = 1.0;
		    else
		      {
			//psTerm.applySky(cfBufMat, false);   // Assign (psScale set in psTerm.init()
			//psTerm.applySky(cfWtBufMat, false); // Assign
			psTerm.applySky(cfBufMat, s, cfBufMat.shape()(0)/s(0));   // Assign (psScale set in psTerm.init()
			psTerm.applySky(cfWtBufMat, s, cfWtBufMat.shape()(0)/s(0)); // Assign

			cfWtBuf *= cfWtBuf;
		      }

		    // WBAWP CODE BEGIN  -- make PS*PS for Weights
		    // psTerm.applySky(cfWtBufMat, true);  // Multiply
		    // WBAWP CODE END

		    // psTerm.applySky(cfBufMat, s, inner/2.0);//pbshp(0)/(os));
		    // psTerm.applySky(cfWtBufMat, s, inner/2.0);//pbshp(0)/(os));

		    // W-term is a unit-amplitude term in the image
		    // doimain.  No need to apply it to the
		    // wt-functions.

		    if (!isDryRun)
		      {
			wTerm.applySky(cfBufMat, iw, cellSize, wScale, cfBuf.shape()(0));///4);
		      }

    		    IPosition PolnPlane(4,0,0,0,0),
		      pbShape(4, cfBuf.shape()(0), cfBuf.shape()(1), 1, 1);
		    //
		    // Make TempImages and copy the buffers with PS *
		    // WKernel applied (too bad that TempImages can't be
		    // made with existing buffers)
		    //
		    //-------------------------------------------------------------
		    TempImage<Complex> twoDPB_l(pbShape, cs_l);
		    TempImage<Complex> twoDPBSq_l(pbShape,cs_l);
		    //-------------------------------------------------------------
		    cfWtBuf *= ftATerm_l.get()*conj(ftATermSq_l.get());

		    cfBuf *= ftATerm_l.get();

		    twoDPB_l.putSlice(cfBuf, PolnPlane);
		    twoDPBSq_l.putSlice(cfWtBuf, PolnPlane);
		    //tim.show("putSlice:");
		    // WBAWP CODE BEGIN
		    //		    twoDPB_l *= ftATerm_l;
		    // WBAWP CODE END

		    //		    twoDPBSq_l *= ftATermSq_l;//*conj(ftATerm_l);

		    // To accumulate avgPB2, call this function.
		    // PBSQWeight
		    Bool PBSQ = false;
		    if(PBSQ) makePBSq(twoDPBSq_l);
		    //
		    // Set the ref. freq. of the co-ordinate system to
		    // that set by ATerm::applySky().
		    //
    		    CoordinateSystem cs=twoDPB_l.coordinates();
    		    Int index= twoDPB_l.coordinates().findCoordinate(Coordinate::SPECTRAL);
    		    SpectralCoordinate SpCS = twoDPB_l.coordinates().spectralCoordinate(index);

    		    Double cfRefFreq=SpCS.referenceValue()(0);
    		    Vector<Double> refValue; refValue.resize(1); refValue(0)=cfRefFreq;
    		    SpCS.setReferenceValue(refValue);
    		    cs.replaceCoordinate(SpCS,index);
		    //
		    // Now FT the function and copy the data from
		    // TempImages back to the CFBuffer buffers
		    //
		    if (!isDryRun)
		      {
			LatticeFFT::cfft2d(twoDPB_l);
			LatticeFFT::cfft2d(twoDPBSq_l);
		      }

		    IPosition shp(twoDPB_l.shape());
		    IPosition start(4, 0, 0, 0, 0), pbSlice(4, shp[0]-1, shp[1]-1,1/*polInUse*/, 1),
		      sliceLength(4,cfBuf.shape()[0]-1,cfBuf.shape()[1]-1,1,1);

		    cfBuf(Slicer(start,sliceLength)).nonDegenerate()
		      =(twoDPB_l.getSlice(start, pbSlice, true));

		    shp = twoDPBSq_l.shape();
		    IPosition pbSqSlice(4, shp[0]-1, shp[1]-1, 1, 1),
		      sqSliceLength(4,cfWtBuf.shape()(0)-1,cfWtBuf.shape()[1]-1,1,1);

		    cfWtBuf(Slicer(start,sqSliceLength)).nonDegenerate()
		      =(twoDPBSq_l.getSlice(start, pbSqSlice, true));
		    //
		    // Finally, resize the buffers, limited to the
		    // support size determined by the threshold
		    // suppled by the ATerm (done internally in
		    // resizeCF()).  Transform the co-ord. system to
		    // the FT domain set the co-ord. sys. and modified
		    // support sizes.
		    //
		    Int supportBuffer = (Int)(getOversampling(psTerm, wTerm, aTerm)*2.0);
		    if (!isDryRun)
		      {
			wtcpeak = max(cfWtBuf);
			cfWtBuf /= wtcpeak;
		      }

		    if (!isDryRun)
		      AWConvFunc::resizeCF(cfWtBuf, xSupportWt, ySupportWt, supportBuffer, samplingWt,0.0);

		    Vector<Double> ftRef(2);

		    ftRef(0)=cfWtBuf.shape()(0)/2.0;
		    ftRef(1)=cfWtBuf.shape()(1)/2.0;
		    CoordinateSystem ftCoords=cs_l;
		    if (isDryRun)
		      {
			ftRef(0)=nx/2.0;
			ftRef(1)=ny/2.0;
			SynthesisUtils::makeFTCoordSys(cs_l, nx,ftRef , ftCoords);
		      }
		    else
		      SynthesisUtils::makeFTCoordSys(cs_l, cfWtBuf.shape()(0), ftRef, ftCoords);

		    CountedPtr<CFCell> cfCellPtr;
		    cfWtb.setParams(inu,iw,imx,imy,//muellerElements(imx)(imy),
				    freqValues(inu), String(""), wValues(iw), muellerElements(imx)(imy),
				    ftCoords, samplingWt, xSupportWt, ySupportWt,
				    String(""), // Default ==> don't set it in the CFCell
				    conjFreq, conjPol[0]);

		    cfCellPtr = cfWtb.getCFCellPtr(freqValues(inu), wValues(iw),
						   muellerElements(imx)(imy));
		    cfCellPtr->pa_p=Quantity(vbPA,"rad");
		    cfCellPtr->telescopeName_p = aTerm.getTelescopeName();
		    cfCellPtr->isRotationallySymmetric_p = aTerm.isNoOp();

		    if (!isDryRun)
		      {
			cpeak = max(cfBuf);
			cfBuf /= cpeak;
		      }

		    if (!isDryRun)
		      AWConvFunc::resizeCF(cfBuf, xSupport, ySupport, supportBuffer, sampling,0.0);

		    if (!isDryRun)
		      {
			LogIO log_l(LogOrigin("AWConvFunc2", "fillConvFuncBuffer[R&D]"));
			log_l << "CF Support: " << xSupport << " (" << xSupportWt << ") " << "pixels" <<  LogIO::POST;
		      }

		    ftRef(0)=cfBuf.shape()(0)/2.0;
		    ftRef(1)=cfBuf.shape()(1)/2.0;

		    cfNorm=cfWtNorm=1.0;

		    if (cfNorm != Complex(0.0)) cfBuf /= cfNorm;
		    if (cfWtNorm != Complex(0.0)) cfWtBuf /= cfWtNorm;

		    ftCoords=cs_l;
		    if (isDryRun)
		      {
			ftRef(0) = nx/2.0;
			ftRef(1) = ny/2.0;
			SynthesisUtils::makeFTCoordSys(cs_l, nx, ftRef, ftCoords);
		      }
		    else
		      SynthesisUtils::makeFTCoordSys(cs_l, cfBuf.shape()(0), ftRef, ftCoords);

		    cfb.setParams(inu,iw,imx,imy,//muellerElements(imx)(imy),
				  freqValues(inu), String(""), wValues(iw), muellerElements(imx)(imy),
				  ftCoords, sampling, xSupport, ySupport,
				  String(""), // Default ==> Don't set in the CFCell
				  conjFreq, conjPol[0]);

		    // Setting CFCell internal parameters works if set
		    // here, not earlier than this!  Not really sure
		    // why (SB).
		    cfCellPtr=cfb.getCFCellPtr(freqValues(inu), wValues(iw),
					       muellerElements(imx)(imy));
		    cfCellPtr->pa_p=Quantity(vbPA,"rad");
		    cfCellPtr->telescopeName_p = aTerm.getTelescopeName();
		    cfCellPtr->isRotationallySymmetric_p = aTerm.isNoOp();

		    cfCellPtr->aTermOn_p = !aTerm.isNoOp();
		    cfCellPtr->psTermOn_p = !psTerm.isNoOp();
		    cfCellPtr->wTermOn_p = !wTerm.isNoOp();
		    cfCellPtr->conjBeams_p = conjPB_p;
		    //
		    // Now tha the CFs have been computed, cache its
		    // paramters in CFCell for quick access in tight
		    // loops (in the re-sampler, e.g.).
		    //

		    (cfWtb.getCFCellPtr(freqValues(inu), wValues(iw),
					muellerElements(imx)(imy)))->initCache(isDryRun);
		    (cfb.getCFCellPtr(freqValues(inu), wValues(iw),
				      muellerElements(imx)(imy)))->initCache(isDryRun);

		    pm.update((Double)cfsDone++);
    		  }
	      }
	  }
    	}
  }
  //
  //----------------------------------------------------------------------
  //
  Complex AWConvFunc::cfArea(Matrix<Complex>& cf,
			     const Int& xSupport, const Int& ySupport,
			     const Float& sampling)
  {
    Complex cfNorm=0;
    Int origin=cf.shape()(0)/2;
    Float peak=0;
    IPosition ndx(4,0,0,0,0);
    IPosition peakPix(ndx);
    peakPix = 0;
    for(ndx(1)=0;ndx(1)<cf.shape()(1);ndx(1)++)
      for(ndx(0)=0;ndx(0)<cf.shape()(0);ndx(0)++)
	if (abs(cf(ndx)) > peak) {peakPix = ndx;peak=abs(cf(ndx));}

   if (origin != peakPix(0))
      {
	LogIO log_l(LogOrigin("AWConvFunc2","cfArea"));
	log_l << "Peak not at the center " << origin << " " << cf(IPosition(4,origin,origin,0,0)) << " " << peakPix << " " << peak << LogIO::NORMAL1;
      }
    for (Int ix=-xSupport;ix<xSupport;ix++)
      for (int iy=-ySupport;iy<ySupport;iy++)
	{
	  //cfNorm += Complex(real(cf(ix*(Int)sampling+origin, iy*(Int)sampling+origin)),0.0);
	  cfNorm += (cf(ix*(Int)sampling+origin, iy*(Int)sampling+origin));
	}
    //    cf /= cfNorm;
    return cfNorm;
  }
  //
  //----------------------------------------------------------------------
  //
  Vector<Double> AWConvFunc::makeWValList(const Double &dW, const Int &nW)
  {
    Vector<Double> wValues(nW);
    //    for (Int iw=0;iw<nW;iw++) wValues[iw]=iw*dW;
    wValues = 0.0;
    if (dW > 0.0)
      for (Int iw=0;iw<nW;iw++) wValues[iw]=iw*iw/dW;
    return wValues;
  }

  // This methods is depcricated.  Keeping it here since it *might*
  // have use sometime later and therefore want to push it on to SVN
  // before deleting it form the active version of this file.
  Matrix<Double> AWConvFunc::getFreqRangePerSpw(const VisBuffer2& vb)
  {
    //
    // Find the total effective bandwidth
    //
    Cube<Double> fminmax;
    Double fMax=0, fMin=MAX_FREQ;
    ArrayColumn<Double> spwCol=vb.subtableColumns().spectralWindow().chanFreq();
    fminmax.resize(spwChanSelFlag_p.shape()(0),spwChanSelFlag_p.shape()(1),2);
    fminmax=0;
    for (uInt ims=0; ims<spwChanSelFlag_p.shape()(0); ims++)
      for(uInt ispw=0; ispw<spwChanSelFlag_p.shape()(1); ispw++)
	{
	  fMax=0, fMin=MAX_FREQ;
	  for(uInt ichan=0; ichan<spwChanSelFlag_p.shape()(2); ichan++)
	    {
	      if (spwChanSelFlag_p(ims,ispw,ichan)==1)
		{
		  Slicer slicer(IPosition(1,ichan), IPosition(1,1));
		  Vector<Double> freq = spwCol(ispw)(slicer);
		  if (freq(0) < fMin) fMin = freq(0);
		  if (freq(0) > fMax) fMax = freq(0);
		}
	    }
	  fminmax(ims,ispw,0)=fMin;
	  fminmax(ims,ispw,1)=fMax;
	}

    Matrix<Double> freqRangePerSpw(fminmax.shape()(1),2);
    for (uInt j=0;j<fminmax.shape()(1);j++) // SPW
      {
	freqRangePerSpw(j,0)=0;
	freqRangePerSpw(j,1)=MAX_FREQ;
	for (uInt i=0;i<fminmax.shape()(0);i++) //MSes
	  {
	    if (freqRangePerSpw(j,0) < fminmax(i,j,0)) freqRangePerSpw(j,0)=fminmax(i,j,0);
	    if (freqRangePerSpw(j,1) > fminmax(i,j,1)) freqRangePerSpw(j,1)=fminmax(i,j,1);
	  }
      }
    for(uInt i=0;i<freqRangePerSpw.shape()(0);i++)
      {
	if (freqRangePerSpw(i,0) == MAX_FREQ) freqRangePerSpw(i,0)=-1;
	if (freqRangePerSpw(i,1) == 0) freqRangePerSpw(i,1)=-1;
      }

    return freqRangePerSpw;
  }
  //
  //----------------------------------------------------------------------
  // Given the VB and the uv-grid, make a list of frequency values to
  // sample the frequency axis of the CFBuffer.  Typically, this will
  // be determined by the bandwidth-smearning limit.
  //
  // This limit is (deltaNu/Nu) * sqrt(l^2 + m^2) < ResolutionElement.
  // Translating max. distance from the phase center to field-of-view
  // of the supplied image, and converting Resolution Element to
  // 1/Cellsize, this expression translates to deltaNU<FMin/Nx (!)
  Vector<Double> AWConvFunc::makeFreqValList(Double &dNU,
					     const VisBuffer2& vb,
					     const ImageInterface<Complex>& uvGrid,
					     Vector<String>& bandNames)
  {
    (void)uvGrid; (void)dNU; (void)vb;
    Vector<Double> fValues;
    Int nSpw = spwFreqSelection_p.shape()(0);
    if (wbAWP_p==false)
      {
	// Return the sky-image ref. freq.
	fValues.resize(1);
	fValues[0]=imRefFreq_p;
      }
    else
      {
	fValues.resize(nSpw);
	for(Int i=0;i<nSpw;i++)
	  fValues(i)=spwFreqSelection_p(i,2);
      }

    bandNames.resize(nSpw);
    ScalarColumn<String> spwNames=vb.subtableColumns().spectralWindow().name();
    for(Int i=0;i<nSpw;i++)
      {
	int s=spwFreqSelection_p(i,0);
	// LogIO os;
	// os << "Spw"<<s<<": " << spwNames.getColumn()[s]
	//    << " " << s << " " << nSpw
	//    << LogIO::WARN;

	bandNames(i)=spwNames.getColumn()[s];
      }
    return fValues;
  }
  //
  //----------------------------------------------------------------------
  //
  void AWConvFunc::makeConvFunction(const ImageInterface<Complex>& image,
				    const VisBuffer2& vb,
				    const Int wConvSize,
				    const CountedPtr<PolOuterProduct>& pop,
				    const Float pa,
				    const Float dpa,
				    const Vector<Double>& uvScale,
				    const Vector<Double>& ,//uvOffset,
				    const Matrix<Double>& ,//vbFreqSelection,
				    CFStore2& cfs2,
				    CFStore2& cfwts2,
				    Bool fillCF)
  {
    LogIO log_l(LogOrigin("AWConvFunc2", "makeConvFunction[R&D]"));
    Int convSize, convSampling, polInUse;
    Double wScale=0.0;
    Array<Complex> convFunc_l, convWeights_l;
    Double cfRefFreq=-1, freqScale=1e8;
    Quantity paQuant(pa,"rad");


    Int nx=image.shape()(0);//, ny=image.shape()(1);
    Vector<Double> skyIncr;

    log_l << "Making a new convolution function for PA="
	  << pa*(180/M_PI) << "deg"
	  << " for field ID " << vb.fieldId()(0);
    // log_l << "TimeStamps(0-10) ";
    // for (Int i=0;i<10;i++)
    //   //log_l << MVTime(vb.time()(i)).string(MVTime::TIME) << " ";
    //   log_l << vb.time()(i)/1e8 << " ";
    log_l << LogIO::NORMAL << LogIO::POST;

    if(wConvSize>0)
      {
	log_l << "Using " << wConvSize << " planes for W-projection" << LogIO::POST;
	Double maxUVW;
	float WFUDGE=4.0;
	WFUDGE=refim::SynthesisUtils::getenv("WTerm.WFUDGE",WFUDGE);

	//maxUVW=0.25/abs(image.coordinates().increment()(0));
	maxUVW=1.0/abs(image.coordinates().increment()(0)*WFUDGE);
	log_l << "Estimating maximum possible W = " << maxUVW
	      << " (wavelengths)" << LogIO::POST;

	// Double invLambdaC=vb.getFrequencies(0)(0)/C::c;
	// Int nFreq = (vb.getFrequencies(0).nelements())-1;
	// Double invMinL = vb.getFrequencies(0)(nFreq)/C::c;
	// log_l << "wavelength range = " << 1.0/invLambdaC << " (m) to "
	//       << 1.0/invMinL << " (m)" << LogIO::POST;
	if (wConvSize > 1)
	  {
	    wScale=Float((wConvSize-1)*(wConvSize-1))/maxUVW;
	    log_l << "Scaling in W (at maximum W) = " << 1.0/wScale
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
    convSampling=getOversampling(*psTerm_p, *wTerm_p, *aTerm_p);
    convSize=aTerm_p->getConvSize();
    //
    // Make a two dimensional image to calculate auto-correlation of
    // the ideal illumination pattern. We want this on a fine grid in
    // the UV plane
    //
    Int index= coords.findCoordinate(Coordinate::SPECTRAL);
    SpectralCoordinate spCS = coords.spectralCoordinate(index);
    imRefFreq_p=spCS.referenceValue()(0);

    index=coords.findCoordinate(Coordinate::DIRECTION);
    AlwaysAssert(index>=0, AipsError);
    DirectionCoordinate dc=coords.directionCoordinate(index);
    Vector<Double> sampling;
    skyIncr = sampling = dc.increment();
    sampling*=Double(convSampling);
    sampling*=Double(nx)/Double(convSize);
    dc.setIncrement(sampling);

    Vector<Double> unitVec(2);
    unitVec=convSize/2;
    dc.setReferencePixel(unitVec);

    // Set the reference value to that of the image
    coords.replaceCoordinate(dc, index);
    //
    // Make an image with circular polarization axis.  Return the
    // no. of vis. poln. planes that will be used in making the user
    // defined Stokes image.
    //
    polInUse=aTerm_p->makePBPolnCoords(vb, convSize, convSampling,
				       image.coordinates(),nx,nx,
				       coords);//,feedStokes_l);
    //------------------------------------------------------------------
    // Make the sky Stokes PB.  This will be used in the gridding
    // correction
    //------------------------------------------------------------------
    IPosition pbShape(4, convSize, convSize, polInUse, 1);
    TempImage<Complex> twoDPB(pbShape, coords);
    IPosition pbSqShp(pbShape);

    unitVec=pbSqShp[0]/2;
    dc.setReferencePixel(unitVec);
    coords.replaceCoordinate(dc, index);

    TempImage<Complex> twoDPBSq(pbSqShp,coords);
    twoDPB.set(Complex(1.0,0.0));
    twoDPBSq.set(Complex(1.0,0.0));
    //
    // Accumulate the various terms that constitute the gridding
    // convolution function.
    //
    //------------------------------------------------------------------
    CountedPtr<CFBuffer> cfb_p, cfwtb_p;
    Vector<Quantity> paList(1); paList[0]=paQuant;
    //
    // Determine the "Mueller Matrix" (called PolOuterProduct here for
    // a better name) elements to use based on the sky-Stokes planes
    // requested.  PolOuterProduct::makePolMap() makes a
    // Matrix<Int>.  The elements of this matrix has the index of the
    // convolution function for the pol. product.  Unused elements are
    // set to -1.  The physical definition of the PolOuterProduct
    // elements are as defined in Eq. 4 in A&A 487, 419-429 (2008)
    // (http://arxiv.org/abs/0805.0834).
    //
    // First detemine the list of Stokes requested.  Then convert the
    // requested Stokes to the appropriate Pol cross-product.  When
    // the off-diagonal elements of the outer-product are significant,
    // this will lead to more than one outer-product element per
    // Stokes.
    //
    // The code below still assume a diagonally dominant
    // outer-product.  This is probably OK for antenna arrays. After the
    // debugging phase is over, the
    // Vector<PolOuterProduct::CrossCircular> should become
    // Matrix<PolOuterProduct> and PolOuterProduct should be
    // "templated" to be of type Circular or Linear.
    //
    StokesCoordinate skyStokesCo=coords.stokesCoordinate(coords.findCoordinate(Coordinate::STOKES));
    Vector<Int> skyStokes=skyStokesCo.stokes();
    //Vector<PolOuterProduct::CrossPolCircular> pp(skyStokes.nelements());
    PolMapType polMap, polIndexMap, conjPolMap, conjPolIndexMap;
    polMap = pop->getPolMat();
    polIndexMap = pop->getPol2CFMat();
    conjPolMap = pop->getConjPolMat();
    conjPolIndexMap = pop->getConjPol2CFMat();

    // cerr << "AWCF: " << polMap << endl << polIndexMap << endl << conjPolMap << endl << conjPolIndexMap << endl;

    // for(uInt ip=0;ip<pp.nelements();ip++)
    // 	pp(ip)=translateStokesToCrossPol(skyStokes(ip));

    // PolOuterProduct pOP; pOP.makePolMap(pp);
    // const Matrix<Int> muellerMatrix=pOP.getPolMap();

    Vector<Double> wValues    = makeWValList(wScale, wConvSize);
    Vector<String> bandNames;
    Vector<Double> freqValues = makeFreqValList(freqScale,vb,image,bandNames);
    log_l << "Making " << wValues.nelements() << " w plane(s). " << LogIO::POST;
    log_l << "Making " << freqValues.nelements() << " frequency plane(s)." << LogIO::POST;
    //
    // If w-term is unity, we can scale the A-term with frequency.  So
    // compute it only for the highest frequency involved.
    //
    //log_l << "Disabled scaling of CFs" << LogIO::WARN << LogIO::POST;
    // if (wConvSize <= 1)
    //   {
    // 	Double rFreq = max(freqValues);
    // 	if (freqValues.nelements() > 1)
    // 	  freqScale=2*(rFreq-min(freqValues));
    // 	freqValues.resize(1);freqValues(0)=rFreq;
    //   }
    log_l << "CFB Freq. axis [N, Min, Max, Incr. (GHz)]: "
	  << freqValues.nelements()  << " "
	  << min(freqValues)/1e9 << " "
	  << max(freqValues)/1e9 << " "
	  << freqScale/1e9
	  << LogIO::POST;
    //
    // Re-size the CFStore object.  It holds CFBuffers index by PA and
    // list of unique baselines (all possible pairs of unique antenna
    // types).
    //
    Matrix<Int> uniqueBaselineTypeList=makeBaselineList(aTerm_p->getAntTypeList());
    Quantity dPA(dpa,"rad");
    Int totalCFs=uniqueBaselineTypeList.shape().product()*wConvSize*freqValues.nelements()*polMap.shape().product()*2;
    ProgressMeter pm(1.0, Double(totalCFs), "makeCF", "","","",true);
    int cfDone=0;
    for(Int ib=0;ib<uniqueBaselineTypeList.shape()(0);ib++)
      {
	//
	// Resize the CFStore if a new PA or unique baseline type is
	// detected.
	//
	Vector<Int> pos;
	pos=cfs2.resize(paQuant, dPA, uniqueBaselineTypeList(ib,0), uniqueBaselineTypeList(ib,1));
	pos=cfwts2.resize(paQuant, dPA, uniqueBaselineTypeList(ib,0), uniqueBaselineTypeList(ib,1));
	//
	// Get and re-size the CFBuffer object.  It holds the 2D
	// convolution functions index by (FreqValue, WValue,
	// MuellerElement).
	//
	cfb_p=cfs2.getCFBuffer(paQuant, dPA, uniqueBaselineTypeList(ib,0),uniqueBaselineTypeList(ib,1));
	cfwtb_p=cfwts2.getCFBuffer(paQuant, dPA, uniqueBaselineTypeList(ib,0),uniqueBaselineTypeList(ib,1));
	cfb_p->setPointingOffset(pixFieldGrad_p);

	// cfb_p->resize(wValues,freqValues,muellerMatrix);
	// cfwtb_p->resize(wValues,freqValues,muellerMatrix);
	cfb_p->resize(wScale, freqScale, wValues,freqValues,polMap, polIndexMap,conjPolMap, conjPolIndexMap);
	cfwtb_p->resize(wScale, freqScale, wValues,freqValues,polMap, polIndexMap,conjPolMap, conjPolIndexMap);

	IPosition start(4, 0, 0, 0, 0);
	IPosition pbSlice(4, convSize, convSize, 1, 1);

	//Initiate construction of the "ConvolveGridder" object inside
	//PSTerm.  This is, for historical reasons, used to access the
	//"standard" Prolate Spheroidal function implementaion.  This
	//however should be replaced with a simpler access, direct
	//access the PS function implementation (in Utils.h
	//SynthesisUtils::libreSpheroidal() - but this needs more
	//testing).
	Float psScale = (2.0*coords.increment()(0))/(nx*image.coordinates().increment()(0)),
	  innerQuaterFraction=1.0;
	{
	  Int inner=convSize/(convSampling);

	  // psScale when using SynthesisUtils::libreSpheroidal() is
	  // 2.0/nSupport.  nSupport is in pixels and the 2.0 is due to
	  // the center being at Nx/2.  Here the nSupport is determined
	  // by the sky-image and is equal to convSize/convSampling.
	  innerQuaterFraction=refim::SynthesisUtils::getenv("AWCF.FUDGE",innerQuaterFraction);

	  Double lambdaByD = innerQuaterFraction*1.22*C::c/min(freqValues)/25.0;
	  Double FoV_x = fabs(nx*skyIncr(0));
	  Double FoV_y = fabs(nx*skyIncr(1));
	  Vector<Double> uvScale_l(3);
	  uvScale_l(0) = (FoV_x < lambdaByD) ? FoV_x : lambdaByD;
	  uvScale_l(1) = (FoV_y < lambdaByD) ? FoV_y : lambdaByD;
	  uvScale_l(2) = uvScale(2);

	  psScale = 2.0/(innerQuaterFraction*convSize/convSampling);// nx*image.coordinates().increment()(0)*convSampling/2;
	  Vector<Double> uvOffset_cf(3,0); uvOffset_cf(0)=uvOffset_cf(2)=convSize/2;
	  psTerm_p->init(IPosition(2,inner,inner), uvScale_l, uvOffset_cf,psScale);
	}

	MuellerElementType muellerElement(0,0);
	CoordinateSystem cfb_cs=coords;
	//
	// Set up the Mueller matrix, the co-ordinate system, freq, and
	// wvalues in the CFBuffer for the currenct CFStore object.
	//
	//cerr<<"Mueller matrix of row length:"<<polMap.nelements()<<" at the start of the CFBuf Loop" <<endl;
	for (Int iw=0;iw<wConvSize;iw++)
	  {
	    for(uInt inu=0;inu<freqValues.nelements(); inu++)
	      {
		Int npol=0;
		for (uInt ipolx=0;ipolx<polMap.nelements();ipolx++)
		  {
		    npol=0;
		  for (uInt ipoly=0;ipoly<polMap(ipolx).nelements();ipoly++)
		    {
		      // Now make a CS with a single appropriate
		      // polarization axis per Mueller element
		      Vector<Int> whichStokes(1,skyStokes(npol++));
		      Int sIndex=cfb_cs.findCoordinate(Coordinate::STOKES);
		      StokesCoordinate stokesCS=cfb_cs.stokesCoordinate(sIndex);
		      Int fIndex=coords.findCoordinate(Coordinate::SPECTRAL);
		      SpectralCoordinate spCS = coords.spectralCoordinate(fIndex);
		      Vector<Double> refValue, incr;
		      refValue = spCS.referenceValue();
		      incr = spCS.increment();
		      cfRefFreq=freqValues(inu);
		      refValue=cfRefFreq;
		      spCS.setReferenceValue(refValue);

		      stokesCS.setStokes(whichStokes);
		      cfb_cs.replaceCoordinate(stokesCS,sIndex);
		      cfb_cs.replaceCoordinate(spCS,fIndex);
		      //
		      // Set the various axis-parameters for the CFBuffer.
		      //
		      Float s=convSampling;
		      // cfb_p->setParams(convSize,convSize,cfb_cs,s,
		      // 		       convSize, convSize,
		      // 		       freqValues(inu), wValues(iw), polMap(ipolx)(ipoly));
		      // cfwtb_p->setParams(convSize,convSize,cfb_cs,s,
		      // 			 convSize, convSize,
		      // 			 freqValues(inu), wValues(iw), polMap(ipolx)(ipoly));
		      cfb_p->setParams(inu, iw, ipolx,ipoly,//polMap(ipolx)(ipoly),
		      		       freqValues(inu), bandNames(inu), wValues(iw), polMap(ipolx)(ipoly),
				       cfb_cs, s, convSize, convSize);
		      cfwtb_p->setParams(inu, iw, ipolx,ipoly,//polMap(ipolx)(ipoly),
		      			 freqValues(inu), bandNames(inu), wValues(iw), polMap(ipolx)(ipoly),
					 cfb_cs, s, convSize, convSize);
		      pm.update((Double)cfDone++);
		    }
		  }
		} // End of loop over Mueller elements.
	  } // End of loop over w
	//
	// By this point, the all the 4 axis (Time/PA, Freq, Pol,
	// Baseline) of the CFBuffer objects have been setup.  The CFs
	// will now be filled using the supplied PS-, W- ad A-term objects.
	//
	if (fillCF) log_l << "Making CFs for baseline type " << ib << LogIO::POST;
	else        log_l << "Making empty CFs for baseline type " << ib << LogIO::POST;
	{
	  //
	  // Get VB PA isn't necessary here since the pa passed to
	  // this function is already derived from VB in AWP.  In
	  // general this function should use whatever is passed to
	  // it, and the caller should decide the source PA value
	  // (e.g. VB or the UI).
	  //
	  // Double vbPA = getPA(vb);
	  Double freqHi;


	  Vector<Double> chanFreq = vb.getFrequencies(0);
	  index = image.coordinates().findCoordinate(Coordinate::SPECTRAL);
	  SpectralCoordinate SpC = cfb_cs.spectralCoordinate(index);
	  Vector<Double> refVal = SpC.referenceValue();

	  freqHi = refVal[0];
	  fillConvFuncBuffer(*cfb_p, *cfwtb_p, nx, nx, skyIncr, convSize, convSize, freqValues, wValues, wScale,
			     paQuant.getValue(),//;vbPA,
			     freqHi,
			     polMap, polIndexMap, vb, psScale,
			     *psTerm_p, *wTerm_p, *aTerm_p, !fillCF);
	}
      } // End of loop over baselines

    index=coords.findCoordinate(Coordinate::SPECTRAL);
    spCS = coords.spectralCoordinate(index);
    Vector<Double> refValue; refValue.resize(1);refValue(0)=cfRefFreq;
    spCS.setReferenceValue(refValue);
    coords.replaceCoordinate(spCS,index);

    // cfs.coordSys=coords;         cfwts.coordSys=coords;
    // cfs.pa=paQuant;   cfwts.pa=paQuant;

    //    aTerm_p->makeConvFunction(image,vb,wConvSize,pa,cfs,cfwts);
  }
  //
  //----------------------------------------------------------------------
  //
  Bool AWConvFunc::setUpCFSupport(Array<Complex>& cffunc, Int& xSupport, Int& ySupport,
				  const Float& sampling, const Complex& peak)
  {
    //
    // Find the convolution function support size.  No assumption
    // about the symmetry of the conv. func. can be made (except that
    // they are same for all poln. planes).
    //
    xSupport = ySupport = -1;
    Int convFuncOrigin=cffunc.shape()[0]/2, R;
    Bool found=false;
    Float threshold;
    // Threshold as a fraction of the peak (presumed to be the center pixel).
    if (abs(peak) != 0) threshold = real(abs(peak));
    else
      threshold   = real(abs(cffunc(IPosition(4,convFuncOrigin,convFuncOrigin,0,0))));

    //threshold *= aTerm_p->getSupportThreshold();
    threshold *= 1e-3;
    //threshold *= 7.5e-2;

    //    threshold *=  0.1;
    // if (aTerm_p->isNoOp())
    //   threshold *= 1e-3; // This is the threshold used in "standard" FTMchines
    // else

    //
    // Find the support size of the conv. function in pixels
    //
    if ((found = AWConvFunc::awFindSupport(cffunc,threshold,convFuncOrigin,R)))
      xSupport=ySupport=Int(0.5+Float(R)/sampling)+1;

    // If the support size overflows, give a warning and set the
    // support size to be convFuncSize/2 + the max. possible offset in
    // the oversamplied grid.  The max. possible offset would 0.5
    // pixels on the sky, which would be sampling/2.0.
    //
    // If the extra buffer (max(offset)) is not included, the problem
    // will show up when gridding the data or weights.  It will not
    // show up when making the avgPB since the gridding for that is
    // always centered on the center of the image.
    if ((xSupport*sampling + int(sampling/2.0+0.5)) > convFuncOrigin)
      {
	LogIO log_l(LogOrigin("AWConvFunc2", "setUpCFSupport[R&D]"));

	log_l << "Convolution function support size > N/2.  Limiting it to N/2 "
	      << "(threshold = " << threshold << ")."
	      << LogIO::WARN;
	xSupport = ySupport = (Int)(convFuncOrigin/sampling-1);
      }

    if(xSupport<1)
      {
	throw(casa::CFSupportZero("AWConvFunc2::setupCFSupport(): Convolution function is misbehaved - support seems to be zero"));
      }
    return found;
  }
  //
  //----------------------------------------------------------------------
  //
  Bool AWConvFunc::resizeCF(Array<Complex>& func, Int& xSupport, Int& ySupport,
			    const Int& supportBuffer, const Float& sampling, const Complex& peak)
  {
    Int ConvFuncOrigin=func.shape()[0]/2;  // Conv. Func. is half that size of convSize

    Bool found = setUpCFSupport(func, xSupport, ySupport, sampling,peak);

    //Int supportBuffer = (Int)(aTerm_p->getOversampling()*1.5);
    Int bot=(Int)(ConvFuncOrigin-sampling*xSupport-supportBuffer),//-convSampling/2,
      top=(Int)(ConvFuncOrigin+sampling*xSupport+supportBuffer);//+convSampling/2;
    //    bot *= 2; top *= 2;
    bot = max(0,bot);
    top = min(top, func.shape()(0)-1);

    Array<Complex> tmp;
    IPosition blc(4,bot,bot,0,0), trc(4,top,top,0,0);
    //
    // Cut out the conv. func., copy in a temp. array, resize the
    // CFStore.data, and copy the cutout version to CFStore.data.
    //
    tmp = func(blc,trc);
    func.resize(tmp.shape());
    func = tmp;
    return found;
  }
  //
  //----------------------------------------------------------------------
  // A global method for use in OMP'ed findSupport() below
  //
  void archPeak(const Float& threshold, const Int& origin, const Block<Int>& cfShape, const Complex* funcPtr,
		const Int& nCFS, const Int& PixInc,const Int& th, const Int& R, Block<Int>& maxR)
  {
    Block<Complex> vals;
    Block<Int> ndx(nCFS);	ndx=0;
    Int NSteps;
    //Check every PixInc pixel along a circle of radius R
    NSteps = 90*R/PixInc;
    vals.resize((Int)(NSteps+0.5));
    uInt valsNelements=vals.nelements();
    vals=0;

    for(Int pix=0;pix<NSteps;pix++)
      {
	ndx[0]=(int)(origin + R*sin(2.0*M_PI*pix*PixInc/R));
	ndx[1]=(int)(origin + R*cos(2.0*M_PI*pix*PixInc/R));

	if ((ndx[0] < cfShape[0]) && (ndx[1] < cfShape[1]))
	  //vals[pix]=func(ndx);
	  vals[pix]=funcPtr[ndx[0]+ndx[1]*cfShape[1]+ndx[2]*cfShape[2]+ndx[3]*cfShape[3]];
      }

    maxR[th]=-R;
    for (uInt i=0;i<valsNelements;i++)
      if (fabs(vals[i]) > threshold)
	{
	  maxR[th]=R;
	  break;
	}
    //		th++;
  }
  //
  //----------------------------------------------------------------------
  //
  Bool AWConvFunc::findSupport(Array<Complex>& func, Float& threshold,
			       Int& origin, Int& radius)
  {
    return awFindSupport(func, threshold, origin, radius);
  }
  //
  //----------------------------------------------------------------------
  //
  Bool AWConvFunc::awFindSupport(Array<Complex>& func, Float& threshold,
			       Int& origin, Int& radius)
  {
    //LogIO log_l(LogOrigin("AWConvFunc2", "findSupport[R&D]"));

    Int nCFS=func.shape().nelements(),
      PixInc=1, R0, R1, R, convSize;
    Block<Int> cfShape(nCFS);
    Bool found=false;
    Complex *funcPtr;
    Bool dummy;
    uInt Nth=1, threadID=0;

    for (Int i=0;i<nCFS;i++)
    	cfShape[i]=func.shape()[i];
    convSize = cfShape[0];

#ifdef _OPENMP
    Nth = max(omp_get_max_threads()-2,1);
#endif

    Block<Int> maxR(Nth);

    funcPtr = func.getStorage(dummy);

    R1 = convSize/2-2;

    while (R1 > 1)
      {
	    R0 = R1; R1 -= Nth;

//#pragma omp parallel default(none) firstprivate(R0,R1)  private(R,threadID) shared(origin,threshold,PixInc,maxR,cfShape,nCFS,funcPtr) num_threads(Nth)
#pragma omp parallel firstprivate(R0,R1)  private(R,threadID) shared(PixInc,maxR,cfShape,nCFS,funcPtr) num_threads(Nth)
	    {
#pragma omp for
	      for(R=R0;R>R1;R--)
		{
#ifdef _OPENMP
		  threadID=omp_get_thread_num();
#endif
		  archPeak(threshold, origin, cfShape, funcPtr, nCFS, PixInc, threadID, R, maxR);
		}
	    }///omp

	    for (uInt th=0;th<Nth;th++)
	      if (maxR[th] > 0)
		{found=true; radius=maxR[th]; return found;}
      }
    return found;
  }
  //
  //----------------------------------------------------------------------
  //
  Bool AWConvFunc::makeAverageResponse(const VisBuffer2& vb,
				       const ImageInterface<Complex>& image,
				       ImageInterface<Float>& theavgPB,
				       Bool reset)
  {
    TempImage<Complex> complexPB;
    Bool pbMade;
    pbMade = makeAverageResponse(vb, image, complexPB,reset);
    normalizeAvgPB(complexPB, theavgPB);
    return pbMade;
  }
  //
  //----------------------------------------------------------------------
  //
  Bool AWConvFunc::makeAverageResponse(const VisBuffer2& vb,
				       const ImageInterface<Complex>& image,
				       ImageInterface<Complex>& theavgPB,
				       Bool reset)
  {
    LogIO log_l(LogOrigin("AWConvFunc2","makeAverageResponse(Complex)[R&D]"));

    log_l << "Making the average response for " << aTerm_p->name()
	  << LogIO::NORMAL  << LogIO::POST;

    if (reset)
      {
	log_l << "Initializing the average PBs"
	      << LogIO::NORMAL << LogIO::POST;
	theavgPB.resize(image.shape());
	theavgPB.setCoordinateInfo(image.coordinates());
	theavgPB.set(1.0);
      }

    aTerm_p->applySky(theavgPB, vb, true, 0);

    return true; // i.e., an average PB was made
  }
  //
  //----------------------------------------------------------------------
  //
  void AWConvFunc::normalizeAvgPB(ImageInterface<Complex>& inImage,
				  ImageInterface<Float>& outImage)
  {
    LogIO log_l(LogOrigin("AWConvFunc2", "normalizeAvgPB[R&D]"));

    String name("avgpb.im");
    storeImg(name,inImage);
    IPosition inShape(inImage.shape()),ndx(4,0,0,0,0);
    Vector<Complex> peak(inShape(2));

    outImage.resize(inShape);
    outImage.setCoordinateInfo(inImage.coordinates());

    Bool isRefIn;
    Array<Complex> inBuf;
    Array<Float> outBuf;

    isRefIn  = inImage.get(inBuf);
    //isRefOut = outImage.get(outBuf);
    log_l << "Normalizing the average PBs to unity"
	  << LogIO::NORMAL << LogIO::POST;
    //
    // Normalize each plane of the inImage separately to unity.
    //
    Complex inMax = max(inBuf);
    if (abs(inMax)-1.0 > 1E-3)
      {
	for(ndx(3)=0;ndx(3)<inShape(3);ndx(3)++)
	  for(ndx(2)=0;ndx(2)<inShape(2);ndx(2)++)
	    {
	      peak(ndx(2)) = 0;
	      for(ndx(1)=0;ndx(1)<inShape(1);ndx(1)++)
		for(ndx(0)=0;ndx(0)<inShape(0);ndx(0)++)
		  if (abs(inBuf(ndx)) > peak(ndx(2)))
		    peak(ndx(2)) = inBuf(ndx);

	      for(ndx(1)=0;ndx(1)<inShape(1);ndx(1)++)
		for(ndx(0)=0;ndx(0)<inShape(0);ndx(0)++)
		  //		      avgPBBuf(ndx) *= (pbPeaks(ndx(2))/peak(ndx(2)));
		  inBuf(ndx) /= peak(ndx(2));
	    }
	if (isRefIn) inImage.put(inBuf);
      }

    ndx=0;
    for(ndx(1)=0;ndx(1)<inShape(1);ndx(1)++)
      for(ndx(0)=0;ndx(0)<inShape(0);ndx(0)++)
	{
	  IPosition plane1(ndx);
	  plane1=ndx;
	  plane1(2)=1; // The other poln. plane
	  //	  avgPBBuf(ndx) = (avgPBBuf(ndx) + avgPBBuf(plane1))/2.0;
	  outBuf(ndx) = sqrt(real(inBuf(ndx) * inBuf(plane1)));
	}
    //
    // Rather convoluted way of copying Pol. plane-0 to Pol. plane-1!!!
    //
    for(ndx(1)=0;ndx(1)<inShape(1);ndx(1)++)
      for(ndx(0)=0;ndx(0)<inShape(0);ndx(0)++)
	{
	  IPosition plane1(ndx);
	  plane1=ndx;
	  plane1(2)=1; // The other poln. plane
	  outBuf(plane1) = real(outBuf(ndx));
	}
  }
  //
  //----------------------------------------------------------------------
  //
  void AWConvFunc::prepareConvFunction(const VisBuffer2& vb, VB2CFBMap& theMap)
  {
    if (aTerm_p->rotationallySymmetric() == false) return;
    Int nRow=theMap.nelements();
    // CountedPtr<CFBuffer> cfb, cbPtr;
    // CountedPtr<CFCell>  cfc;
    // CountedPtr<ATerm> aTerm_l=aTerm_p;
    CFBuffer *cfb, *cbPtr=0;
    CFCell  *cfc, *baseCFC=NULL;
    ATerm *aTerm_l=&*aTerm_p;

    cfb=&*(theMap[0]);
    cfc = &*(cfb->getCFCellPtr(0,0,0));
    Double actualPA = getPA(vb), currentCFPA = cfc->pa_p.getValue("rad");
    Double dPA = currentCFPA-actualPA;

    if (fabs(dPA) <= fabs(rotateCFOTFAngleRad_p)) return;


//     Int Nth=1;
// #ifdef _OPENMP
//     Nth=max(omp_get_max_threads()-2,1);
// #endif
    for (Int irow=0;irow<nRow;irow++)
      {
	cfb=&*(theMap[irow]);
	//	if ((!cfb.null()) && (cfb != cbPtr))
	if ((cfb!=NULL) && (cfb != cbPtr))
	  {
	    // baseCFB_p = cfb->clone();
	    // cerr << "NRef = " << baseCFB_p.nrefs() << endl;
	    //
	    // If the following messsage is emitted more than once, we
	    // are in a heterogeneous-array case
	    //
	    LogIO log_l(LogOrigin("AWConvFunc2", "prepareConvFunction"));
	    log_l << "Rotating the base CFB from PA=" << cfb->getCFCellPtr(0,0,0)->pa_p.getValue("deg")
		  << " to " << actualPA*57.2957795131
		  << " " << cfb->getCFCellPtr(0,0,0)->shape_p
		  << LogIO::DEBUG1 << LogIO::POST;

	    IPosition shp(cfb->shape());
	    cbPtr = cfb;
	    for(Int k=0;k<shp(2);k++)   // Mueller-loop
	      for(Int j=0;j<shp(1);j++)     // W-loop
// #pragma omp parallel default(none) firstprivate(j,k) shared(shp,cfb,aTerm_l) num_threads(Nth)
     {
// #pragma omp for
		for (Int i=0;i<shp(0);i++)      // Chan-loop
		  {
		    cfc = &*(cfb->getCFCellPtr(i,j,k));
		    //baseCFC = &*(baseCFB_p->getCFCellPtr(i,j,k));
		    // Call this for every VB.  Any optimization
		    // (e.g. rotating at some increment only) is
		    // implemented in the ATerm::rotate().
		    //		    if (rotateCF_p)
		    // Rotate the cell only if it has been loaded.
		    if (cfc->getShape().product() > 0)
		      aTerm_l->rotate2(vb,*baseCFC, *cfc,rotateCFOTFAngleRad_p);
		  }
    }
	  }
      }
  };
  //
  //----------------------------------------------------------------------
  //
  void AWConvFunc::setMiscInfo(const RecordInterface& params)
  {
    (void)params;
  }
    //
    //----------------------------------------------------------------------
    // This is a static function declared in this class. It is called
    // from makeConvFuncion2() which is another static function. I.e.,
    // this can be used without instantiating the AWConvFunc class.  It
    // is intented to work with only the supplied arugments (and not use
    // any of the internal objects of AWConvFunc class).  It can (and
    // should) be a global function outside this class definition.
    //
  void AWConvFunc::fillConvFuncBuffer2(CFBuffer& cfb, CFBuffer& cfWtb,
				       const Int& nx, const Int& ny,
				       const ImageInterface<Complex>* skyImage,
				       const CFCStruct& miscInfo,
				       PSTerm& psTerm, WTerm& wTerm, ATerm& aTerm,
				       Bool conjBeams)

  {
    LogIO log_l(LogOrigin("AWConvFunc2", "fillConvFuncBuffer2[R&D]"));
    Complex cfNorm, cfWtNorm;
    Complex cpeak;
    {
      Float sampling, samplingWt;
      Int xSupport, ySupport, xSupportWt, ySupportWt;
      CoordinateSystem cs_l;
      String bandName;
      // Extract the parameters index by (MuellerElement, Freq, W)
      cfWtb.getParams(cs_l, samplingWt, xSupportWt, ySupportWt, bandName,
		      miscInfo.freqValue, miscInfo.wValue, //The address of CFCell as physical co-ords
		      miscInfo.muellerElement);
      cfb.getParams(cs_l, sampling, xSupport, ySupport, bandName,
		    miscInfo.freqValue,miscInfo.wValue, //The address of CFCell as physical co-ords
		    miscInfo.muellerElement);
      aTerm.setBandName(bandName);
      //
      // Cache the A-Term for this polarization and frequency
      //
      Double conjFreq, vbPA;
      CountedPtr<CFCell> thisCell=cfb.getCFCellPtr(miscInfo.freqValue, miscInfo.wValue, miscInfo.muellerElement);
      vbPA = thisCell->pa_p.getValue("rad");
      conjFreq = thisCell->conjFreq_p;
      CoordinateSystem conjPolCS_l=cs_l;  AWConvFunc::makeConjPolAxis(conjPolCS_l, thisCell->conjPoln_p);
      IPosition pbshp(4,nx,ny,1,1);
      TempImage<Complex> ftATerm_l(pbshp, cs_l), ftATermSq_l(pbshp,conjPolCS_l);
      Bool doSquint=true;
      ftATerm_l.set(Complex(1.0,0.0));   ftATermSq_l.set(Complex(1.0,0.0));
      Double freq_l=miscInfo.freqValue;

      {
	aTerm.applySky(ftATerm_l, vbPA, doSquint, 0, miscInfo.muellerElement,freq_l);//freqHi);
	if (conjBeams) aTerm.applySky(ftATermSq_l, vbPA, doSquint, 0, miscInfo.muellerElement, conjFreq);//freqHi);
	else aTerm.applySky(ftATermSq_l, vbPA, doSquint, 0,miscInfo.muellerElement,freq_l);
      }

      Vector<Double> cellSize;
      // {
      // 	Int linIndex=cs_l.findCoordinate(Coordinate::LINEAR);
      // 	LinearCoordinate lc=cs_l.linearCoordinate(linIndex);
      // 	Vector<Bool> axes(2); axes=true;
      // 	Vector<Int> dirShape(2); dirShape(0)=nx;dirShape(1)=ny;
      // 	Coordinate* FTlc=lc.makeFourierCoordinate(axes,dirShape);
      // 	cellSize = lc.increment();
      // }

      //
      // By the time control gets here, ImageInformation inputs should
      // exist in the CFC one way or another.  If not, this is due to
      // a case that logic in makeConvFunction2() missed -- an
      // internal error.
      //
      try
	{
	  ImageInformation<Complex> imInfo(cfb.getCFCacheDir());
	  CoordinateSystem skyCoords(imInfo.getCoordinateSystem());
	  //	CoordinateSystem skyCoords(skyImage.coordinates());

	  Vector<int> skyImageShape = imInfo.getImShape();
	  Int directionIndex=skyCoords.findCoordinate(Coordinate::DIRECTION);
	  DirectionCoordinate dc=skyCoords.directionCoordinate(directionIndex);
	  //Vector<Double> cellSize;
	  //cellSize = dc.increment()*(Double)(miscInfo.sampling*skyImage.shape()(0)/nx); // nx is the size of the CF buffer
	  cellSize = dc.increment()*(Double)(miscInfo.sampling*skyImageShape(0)/nx); // nx is the size of the CF buffer
	}
      catch(casa::refim::ImageInformationError &e)
	{
	  log_l << e.what() << endl << "This is an internal error." << LogIO::EXCEPTION;
	}
      //
      // Now compute the PS x W-Term and apply the cached
      // A-Term to build the full CF.
      //
      {
	log_l << " CF("
	      << "M:"<< miscInfo.muellerElement
	      << ",C:" << miscInfo.freqValue/1e9
	      << ",W:" << miscInfo.wValue << "): ";
	Array<Complex> &cfWtBuf=(*(cfWtb.getCFCellPtr(miscInfo.freqValue, miscInfo.wValue, miscInfo.muellerElement))->storage_p);
	Array<Complex> &cfBuf=(*(cfb.getCFCellPtr(miscInfo.freqValue, miscInfo.wValue, miscInfo.muellerElement))->storage_p);

	cfWtBuf.resize(pbshp);
	cfBuf.resize(pbshp);

	const Vector<Double> sampling_l(2,sampling);
	Matrix<Complex> cfBufMat(cfBuf.nonDegenerate()),
	  cfWtBufMat(cfWtBuf.nonDegenerate());
	//
	// Apply the Prolate Spheroidal and W-Term kernels
	//
	Vector<Double> s(2); s=sampling;

	if (psTerm.isNoOp())
	  cfBufMat = cfWtBufMat = 1.0;
	else
	  {
	    // psTerm.applySky(cfBufMat, False);   // Assign (psScale set in psTerm.init()
	    // psTerm.applySky(cfWtBufMat, False); // Assign
	    psTerm.applySky(cfBufMat, s, cfBufMat.shape()(0)/s(0));   // Assign (psScale set in psTerm.init()
	    psTerm.applySky(cfWtBufMat, s, cfWtBufMat.shape()(0)/s(0)); // Assign
	    cfWtBuf *= cfWtBuf;
	  }

	if (miscInfo.wValue > 0)
	  wTerm.applySky(cfBufMat, cellSize, miscInfo.wValue, cfBuf.shape()(0));///4);

	IPosition PolnPlane(4,0,0,0,0),
	  pbShape(4, cfBuf.shape()(0), cfBuf.shape()(1), 1, 1);
	//
	// Make TempImages and copy the buffers with PS *
	// WKernel applied (too bad that TempImages can't be
	// made with existing buffers)
	//
	//-------------------------------------------------------------
	TempImage<Complex> twoDPB_l(pbShape, cs_l);
	TempImage<Complex> twoDPBSq_l(pbShape,cs_l);
	//-------------------------------------------------------------

	cfWtBuf *= ftATerm_l.get()*conj(ftATermSq_l.get());

	cfBuf *= ftATerm_l.get();
	twoDPB_l.putSlice(cfBuf, PolnPlane);
	twoDPBSq_l.putSlice(cfWtBuf, PolnPlane);

	// To accumulate avgPB2, call this function.
	// PBSQWeight
	// Bool PBSQ = false;
	// if(PBSQ) makePBSq(twoDPBSq_l);

	//
	// Set the ref. freq. of the co-ordinate system to
	// that set by ATerm::applySky().
	//
	CoordinateSystem cs=twoDPB_l.coordinates();
	Int index= twoDPB_l.coordinates().findCoordinate(Coordinate::SPECTRAL);
	SpectralCoordinate SpCS = twoDPB_l.coordinates().spectralCoordinate(index);

	Double cfRefFreq=SpCS.referenceValue()(0);
	Vector<Double> refValue; refValue.resize(1); refValue(0)=cfRefFreq;
	SpCS.setReferenceValue(refValue);
	cs.replaceCoordinate(SpCS,index);

	{
	  LatticeFFT::cfft2d(twoDPB_l);
	  LatticeFFT::cfft2d(twoDPBSq_l);
	}

	IPosition shp(twoDPB_l.shape());
	IPosition start(4, 0, 0, 0, 0), pbSlice(4, shp[0]-1, shp[1]-1,1/*polInUse*/, 1),
	  sliceLength(4,cfBuf.shape()[0]-1,cfBuf.shape()[1]-1,1,1);

	cfBuf(Slicer(start,sliceLength)).nonDegenerate()
	  =(twoDPB_l.getSlice(start, pbSlice, true));

	shp = twoDPBSq_l.shape();
	IPosition pbSqSlice(4, shp[0]-1, shp[1]-1, 1, 1),
	  sqSliceLength(4,cfWtBuf.shape()(0)-1,cfWtBuf.shape()[1]-1,1,1);

	cfWtBuf(Slicer(start,sqSliceLength)).nonDegenerate()
	  =(twoDPBSq_l.getSlice(start, pbSqSlice, true));
	Int supportBuffer = (Int)(AWConvFunc::getOversampling(psTerm, wTerm, aTerm)*2.0);
	AWConvFunc::resizeCF(cfWtBuf, xSupportWt, ySupportWt, supportBuffer, samplingWt,0.0);

	Vector<Double> ftRef(2);
	ftRef(0)=cfWtBuf.shape()(0)/2.0;
	ftRef(1)=cfWtBuf.shape()(1)/2.0;
	CoordinateSystem ftCoords=cs_l;
	SynthesisUtils::makeFTCoordSys(cs_l, cfWtBuf.shape()(0), ftRef, ftCoords);

	thisCell=cfWtb.getCFCellPtr(miscInfo.freqValue, miscInfo.wValue, miscInfo.muellerElement);
	thisCell->pa_p=Quantity(vbPA,"rad");
	thisCell->coordSys_p = ftCoords;
	thisCell->xSupport_p = xSupportWt;
	thisCell->ySupport_p = ySupportWt;
	thisCell->isRotationallySymmetric_p = aTerm.isNoOp();
	{
	  cpeak = max(cfBuf);
	  cfBuf /= cpeak;
	}

	AWConvFunc::resizeCF(cfBuf, xSupport, ySupport, supportBuffer, sampling,0.0);

	log_l << "CF Support: " << xSupport << " (" << xSupportWt << ") " << "pixels" <<  LogIO::POST;

	ftRef(0)=cfBuf.shape()(0)/2.0;
	ftRef(1)=cfBuf.shape()(1)/2.0;

	{
	  cfNorm=0; cfWtNorm=0;
	  cfNorm = AWConvFunc::cfArea(cfBufMat, xSupport, ySupport, sampling);
	  cfWtNorm = AWConvFunc::cfArea(cfWtBufMat, xSupportWt, ySupportWt, sampling);
	}

	if (cfNorm != Complex(0.0))    cfBuf /= cfNorm;
	if (cfWtNorm != Complex(0.0)) cfWtBuf /= cfWtNorm;

	ftCoords=cs_l;
	SynthesisUtils::makeFTCoordSys(cs_l, cfBuf.shape()(0), ftRef, ftCoords);

	thisCell=cfb.getCFCellPtr(miscInfo.freqValue, miscInfo.wValue, miscInfo.muellerElement);
	thisCell->pa_p=Quantity(vbPA,"rad");
	thisCell->coordSys_p = ftCoords;
	thisCell->xSupport_p = xSupport;
	thisCell->ySupport_p = ySupport;
	thisCell->isRotationallySymmetric_p = aTerm.isNoOp();
	thisCell->conjBeams_p = conjBeams;
	thisCell->aTermOn_p  = !aTerm.isNoOp();
	thisCell->psTermOn_p = !psTerm.isNoOp();
	thisCell->wTermOn_p  = !wTerm.isNoOp();

	(cfWtb.getCFCellPtr(miscInfo.freqValue, miscInfo.wValue, miscInfo.muellerElement))->initCache();
	(cfb.getCFCellPtr(miscInfo.freqValue, miscInfo.wValue, miscInfo.muellerElement))->initCache();
      }
    }
  }

  //
  //----------------------------------------------------------------------
  // This is a static function declared in this class.  I.e., this can
  // be used without instantiating the AWConvFunc class.  It is
  // intented to work with only the supplied arugments (and not use
  // any of the internal objects of AWConvFunc class).  It can (and
  // should) be a global function outside this class definition. It
  // calls fillConvFuncBuffer2(), another static function.
  //
  void AWConvFunc::makeConvFunction2(const String& cfCachePath,
				     const Vector<Double>&,// uvScale,
				     const Vector<Double>& uvOffset,
				     const Matrix<Double>& ,//vbFreqSelection,
				     CFStore2& cfs2,
				     CFStore2& cfwts2,
				     const Bool psTermOn,
				     const Bool aTermOn,
				     const Bool conjBeams)
  {
    LogIO log_l(LogOrigin("AWConvFunc2", "makeConvFunction2[R&D]"));
    Int convSize, convSampling;//, polInUse;
    Array<Complex> convFunc_l, convWeights_l;
    //
    // Get the coordinate system
    //
    Int skyNX,skyNY;
    Vector<int> imShape;
    CoordinateSystem skyCoords;
    const String uvGridDiskImage=cfCachePath+"/"+"uvgrid.im";
    Double skyMinFreq;
    Vector<Double> skyIncr;
    CountedPtr<PagedImage<Complex> > skyImage_l;
    ImageInformation<Complex> imInfo;
    //
    // Get the sky image coordinates and shape.
    //
    // The logic in the code below is that if the CFC is a new one, it
    // should have inputs for the ImageInformation object (the
    // try-block below) and not need the uvGridDiskImage image.
    //
    // If ImageInformation<T> object could not get this information, it is
    // assumed that the CFC is the older one which has the
    // uvGridDiskImage image.  So use it as input to construct the
    // ImageInformation<T>, save it, and retrieve the information from
    // it.  The try-block code should work in next application of this
    // code to the same CFC.
    //
    try
      {
	// Assume this is a new-format CFC and misc info can be read
	// from a saved Record using ImageInformation<T>(PATH).
	imInfo = ImageInformation<Complex> (cfCachePath);
	skyCoords = imInfo.getCoordinateSystem();
	imShape = imInfo.getImShape();
      }
    catch (casa::refim::ImageInformationError &e)
      {
	// Fallback: If ImageInformation<T>(CFCPath) did not succeed,
	// it indicates that this is an old CFC, never before touced by
	// this code.  So, fill from the "uvgrid.im" image that is
	// saved in the old CFC using
	// ImageInformation<T>(Image,CFCPath), and write back the
	// information to the CFC.  This would make the CFC into the
	// new format and on the next visit the try{...} clause above
	// should succeed.
	log_l << e.what() << LogIO::WARN;
	skyImage_l = new PagedImage<Complex> (uvGridDiskImage);//cfs2.getCacheDir()+"/uvgrid.im");
	imInfo = ImageInformation<Complex>(*skyImage_l, cfCachePath);
	imInfo.save();
	skyCoords = imInfo.getCoordinateSystem();
	imShape = imInfo.getImShape();
      }

    {
      skyNX = imShape[0];
      skyNY = imShape[1];
      Int directionIndex=skyCoords.findCoordinate(Coordinate::DIRECTION);
      DirectionCoordinate dc=skyCoords.directionCoordinate(directionIndex);
      skyIncr = dc.increment();
    }

    CountedPtr<CFBuffer> cfb_p, cfwtb_p;

    IPosition cfsShape = cfs2.getShape();
    IPosition wCFStShape = cfwts2.getShape();

    //Matrix<Int> uniqueBaselineTypeList=makeBaselineList(aTerm_p->getAntTypeList());
    Bool wbAWP, wTermOn;
    for (int iPA=0; iPA<cfsShape[0]; iPA++)
      for (int iB=0; iB<cfsShape[1]; iB++)
	  {
	    log_l << "Filling CFs for baseline type " << iB << ", PA slot " << iPA << LogIO::WARN << LogIO::POST;
	    cfb_p=cfs2.getCFBuffer(iPA,iB);
	    cfwtb_p=cfwts2.getCFBuffer(iPA,iB);

	    cfb_p->primeTheCache();
	    cfwtb_p->primeTheCache();

	    IPosition cfbShape = cfb_p->shape();
	    for (int iNu=0; iNu<cfbShape(0); iNu++)       // Frequency axis
	      {
		for (int iPol=0; iPol<cfbShape(2); iPol++)     // Polarization axis
		  for (int iW=0; iW<cfbShape(1); iW++)   // W axis
		    {
		      CFCStruct miscInfo;
		      CoordinateSystem cs_l;
		      Float sampling;

		      CountedPtr<CFCell>& tt=(*cfb_p).getCFCellPtr(iNu, iW, iPol);
		      // tt->show("",cout);

		      // Fill the CFCell if it isn't already filled.
		      if ((tt->isFilled_p==false) && (tt->shape_p.nelements() != 0))
			{
			  tt->getAsStruct(miscInfo); // Get misc. info. for this CFCell

			  int xSupport=miscInfo.xSupport;
			  int ySupport=miscInfo.ySupport;

			  if (miscInfo.shape[0] == miscInfo.xSupport*2*miscInfo.sampling + 4*miscInfo.sampling+1)
			    break;
			  wbAWP=True; // Always true since the Freq. value is got from the coord. sys.
			  wTermOn=(miscInfo.wValue > 0.0);

			  {
			    //This code uses the BeamCalc class to get
			    //the nominal min. freq. of the band in
			    //use.  While not accurate, may be
			    //sufficient for the purpose of the
			    //anti-aliasing operator.
			    try
			      {
				Int bandID = BeamCalc::Instance()->getBandID(miscInfo.freqValue,miscInfo.telescopeName,miscInfo.bandName);
				skyMinFreq = casa::EVLABandMinFreqDefaults[bandID];
			      }
			    catch(AipsError &e)
			      {
				log_l << "Determining the minimum frequency from sky image." << LogIO::POST;
				Int index= skyCoords.findCoordinate(Coordinate::SPECTRAL);
				SpectralCoordinate SpCS = skyCoords.spectralCoordinate(index);
				skyMinFreq=SpCS.referenceValue()(0);
			      }
			  }

			  bool aTermOn_l=aTermOn, psTermOn_l=psTermOn, wTermOn_l=wTermOn, conjBeams_l=conjBeams;
			  {
			    // Read the miscinfo for the currect CFCell.
			    ImageInformation<Complex> imInfo(cfCachePath+"/"+tt->fileName_p);
			    Record miscInfoRec = imInfo.getMiscInfo();
			    //
			    // Older CFCs which do not have miscInfo.rec will not have the following parameters defined.
			    //
			    // So, if the parameters are defined in the CFCell's miscInfo.rec, use them.
			    // Else use the values supplied as parameters to this method.
			    //
			    if (miscInfoRec.isDefined("aTermOn"))  miscInfoRec.get("aTermOn",  aTermOn_l);
			    if (miscInfoRec.isDefined("psTermOn")) miscInfoRec.get("psTermOn", psTermOn_l);
			    if (miscInfoRec.isDefined("wTermOn"))  miscInfoRec.get("wTermOn",  wTermOn_l);
			    if (miscInfoRec.isDefined("conjBeams"))  miscInfoRec.get("conjBeams",  conjBeams_l);
			    float s;
			    miscInfoRec.get("Sampling",s);
			    convSampling = s;
			  }
			  CountedPtr<ConvolutionFunction> awCF = AWProjectFT::makeCFObject(miscInfo.telescopeName,
											   aTermOn_l, psTermOn_l, wTermOn_l,true,
											   wbAWP, conjBeams_l, xSupport, convSampling);
			  if (aTermOn_l==false)
			    {
			      (static_cast<AWConvFunc &>(*awCF)).aTerm_p->setOpCode(CFTerms::NOOP);
			      (static_cast<AWConvFunc &>(*awCF)).aTerm_p->cacheVBInfo(miscInfo.telescopeName, miscInfo.diameter);
			    }
			  if (psTermOn_l==false) (static_cast<AWConvFunc &>(*awCF)).psTerm_p->setOpCode(CFTerms::NOOP);
			  if (wTermOn_l==false) (static_cast<AWConvFunc &>(*awCF)).wTerm_p->setOpCode(CFTerms::NOOP);

			  //aTerm_p->cacheVBInfo(miscInfo.telescopeName, miscInfo.diameter);

			  String bandName;
			  cfb_p->getParams(cs_l, sampling, xSupport, ySupport,bandName,iNu,iW,iPol);
			  //convSampling=miscInfo.sampling;

			  //convSize=miscInfo.shape[0];
			  // This method loads "empty CFs".  Those have
			  // support size equal to the CONVBUF size
			  // required.  So use that, instead of the
			  // "shape" information from CFs, since the
			  // latter for empty CFs can be small (to save
			  // disk space and i/o -- the CFs are supposed
			  // to be empty anyway at this stage!)
			  convSize=xSupport;

			  IPosition start(4, 0, 0, 0, 0);
			  IPosition pbSlice(4, convSize, convSize, 1, 1);
			  {
			    // Set up the anti-aliasing operator (psTerm_p) for this CF.
			    Int inner=convSize/(convSampling);

			    //Float psScale = (2*coords.increment()(0))/(nx*image.coordinates().increment()(0));
			    Float innerQuaterFraction=1.0;
			    innerQuaterFraction=refim::SynthesisUtils::getenv("AWCF.FUDGE",innerQuaterFraction);

			    Double lambdaByD = innerQuaterFraction*1.22*C::c/skyMinFreq/miscInfo.diameter;
			    Double FoV_x = fabs(skyNX*skyIncr(0));
			    Double FoV_y = fabs(skyNY*skyIncr(1));
			    Vector<Double> uvScale_l(3);
			    uvScale_l(0) = (FoV_x < lambdaByD) ? FoV_x : lambdaByD;
			    uvScale_l(1) = (FoV_y < lambdaByD) ? FoV_y : lambdaByD;
			    uvScale_l(2) = 0.0;

			    Float psScale = 2.0/(innerQuaterFraction*convSize/convSampling);// nx*image.coordinates().increment()(0)*convSampling/2;
			    ((static_cast<AWConvFunc &>(*awCF)).psTerm_p)->init(IPosition(2,inner,inner), uvScale_l, uvOffset,psScale);
			  }
			  //
			  // By this point, the all the 4 axis (Time/PA, Freq, Pol,
			  // Baseline) of the CFBuffer objects have been setup.  The CFs
			  // will now be filled using the supplied PS-, W- ad A-term objects.
			  //
			  try
			    {
			      // A note for future cleanup: The cfb_p,
			      // cfwtb_p and convSize information now
			      // should not be required since those are
			      // in miscInfo. Any information is
			      // currently derived from CFBs should be
			      // made available via miscInfo.  This
			      // will also make this call more CASACore
			      // agnostic (and some day exposed for
			      // direct use).
			      AWConvFunc::fillConvFuncBuffer2(*cfb_p, *cfwtb_p, convSize, convSize,
							      NULL,
							      miscInfo,
							      *((static_cast<AWConvFunc &>(*awCF)).psTerm_p),
							      *((static_cast<AWConvFunc &>(*awCF)).wTerm_p),
							      *((static_cast<AWConvFunc &>(*awCF)).aTerm_p),
							      conjBeams_l);
			    }
			  catch (CFSupportZero& e)
			    {
			      LogIO log_l(LogOrigin("AWConvFunc", "makeConvFunction2"));
			      log_l << e.what() << LogIO::POST
				    << "We are assuming that the CF (\"" << tt->fileName_p <<"\") is already filled"
				    << LogIO::POST;
			    }
			  // Mark this CFCell as filled.  The decision
			  // to trigger filling of the CF and WTCF
			  // earlier is based on checking isFilled_p
			  // only for CF, but since
			  // fillConvFuncBuffer2() fills both CF and
			  // WTCF, mark the latter as filled also.
			  tt->isFilled_p=true;
			  ((*cfwtb_p).getCFCellPtr(iNu, iW, iPol))->isFilled_p  = true;
			  ((*cfwtb_p).getCFCellPtr(iNu, iW, iPol))->conjBeams_p = conjBeams_l;
			  ((*cfwtb_p).getCFCellPtr(iNu, iW, iPol))->aTermOn_p   = aTermOn_l;
			  ((*cfwtb_p).getCFCellPtr(iNu, iW, iPol))->psTermOn_p  = psTermOn_l;
			  ((*cfwtb_p).getCFCellPtr(iNu, iW, iPol))->wTermOn_p   = wTermOn_l;

			  //cfb_p->show(NULL,cerr);
			}
		    } // End of loop over W terms
	      } // End of loop over frequencies
	  } // End of loop over baselines

    //
    // Make the CFStores persistent.
    //
    cfs2.makePersistent(cfCachePath.c_str());
    cfwts2.makePersistent(cfCachePath.c_str(),"","WT");
    // Directory dir(uvGridDiskImage);
    // dir.removeRecursive(false);
    // dir.remove();
  }
  //
  //----------------------------------------------------------------------
  //
  Int AWConvFunc::getOversampling(PSTerm& psTerm, WTerm& wTerm, ATerm& aTerm)
  {
    Int os;
    if (!aTerm.isNoOp())
      {
	os=aTerm.getOversampling();
      }
    else if (!wTerm.isNoOp())
      {
	os=wTerm.getOversampling();
      }
    else
      {
	os=psTerm.getOversampling();
      }
    return os;
  }
  //
  //----------------------------------------------------------------------
  //
  // Vector<Vector<Double> > AWConvFunc::findPointingOffset(const ImageInterface<Complex>& image,
  // 						const VisBuffer2& vb, const Bool& doPointing)
  // {
  //   Assert(po_p.null()==False && "Pointingoffset call has not been initialized in AWProjectFT call being made");
  //       return po_p->findPointingOffset(image,vb,doPointing);
  //   //    if (!doPointing)
  //   //      {cerr<<"AWCF: Using mosaic pointing \n";return po_p->findMosaicPointingOffset(image,vb);}
  //   //    else
  //   //      {cerr<<"AWCF: Using antenna pointing table \n";return po_p->findAntennaPointingOffset(image,vb);}
  // }



};
};
