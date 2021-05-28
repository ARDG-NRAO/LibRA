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
#include <synthesis/TransformMachines/AWConvFunc.h>
#include <synthesis/TransformMachines/AWProjectFT.h>
#include <synthesis/TransformMachines/SynthesisError.h>
#include <images/Images/ImageInterface.h>
#include <synthesis/TransformMachines/Utils.h>
#include <synthesis/TransformMachines/BeamCalc.h>
#include <synthesis/TransformMachines/CFStore.h>
#include <synthesis/TransformMachines/CFStore2.h>
#include <synthesis/TransformMachines/PSTerm.h>
#include <synthesis/TransformMachines/WTerm.h>
#include <synthesis/TransformMachines/ATerm.h>
#include <synthesis/TransformMachines/VLACalcIlluminationConvFunc.h>
#include <synthesis/TransformMachines/ConvolutionFunction.h>
#include <synthesis/TransformMachines/PolOuterProduct.h>
#include <coordinates/Coordinates/DirectionCoordinate.h>
#include <coordinates/Coordinates/LinearCoordinate.h>
#include <coordinates/Coordinates/SpectralCoordinate.h>
#include <coordinates/Coordinates/StokesCoordinate.h>
#include <casa/System/ProgressMeter.h>
#include <lattices/LatticeMath/LatticeFFT.h>
#include <casa/Utilities/CompositeNumber.h>
#include <casa/OS/Directory.h>
#include <casa/OS/Timer.h>
#include <ostream>
#ifdef _OPENMP
#include <omp.h>
#endif

#define MAX_FREQ 1e30

using namespace casacore;
namespace casa{
  AWConvFunc::AWConvFunc(const casacore::CountedPtr<ATerm> aTerm,
			 const casacore::CountedPtr<PSTerm> psTerm,
			 const casacore::CountedPtr<WTerm> wTerm,
			 const casacore::Bool wbAWP,
			 const casacore::Bool conjPB):
    ConvolutionFunction(),aTerm_p(aTerm),psTerm_p(psTerm), wTerm_p(wTerm), pixFieldGrad_p(), 
    wbAWP_p(wbAWP), conjPB_p(conjPB), baseCFB_p()
  {
    LogIO log_l(LogOrigin("AWConvFunc", "AWConvFunc"));
    if (psTerm->isNoOp() && aTerm->isNoOp())
      log_l << "Both, psterm and aterm cannot be set to NoOp. " << LogIO::EXCEPTION;
    
    if (wbAWP && aTerm->isNoOp())
      {
	//log_l << "wbawp=True is ineffective when aterm is OFF.  Setting wbawp to False." << LogIO::NORMAL1;
	wbAWP_p=false;
      }
    
    pixFieldGrad_p.resize(2);pixFieldGrad_p=0.0;
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
	// Array<Complex> pol0;
	// lat.getSlice(pol0,slicePol0);
	// pol0 = pol0*conj(pol0);
	buf = buf * conj(buf);
      }
  }
  //
  //----------------------------------------------------------------------
  //
  void AWConvFunc::makeConjPolAxis(CoordinateSystem& cs,
				   Int conjStokes_in)
  {
    LogIO log_l(LogOrigin("AWConvFunc", "makeConjPolAxis[R&D]"));
    IPosition dummy;
    Vector<String> csList;
    Vector<Int> stokes, conjStokes;

    // cout << "CoordSys: ";
    // csList = cs.list(log_l,MDoppler::RADIO,dummy,dummy);
    // cout << csList << endl;
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
				      const Int& nx, const Int& ny, 
				      const Vector<Double>& freqValues,
				      const Vector<Double>& wValues,
				      const Double& wScale,
				      const Double& vbPA, const Double& freqHi,
				      const PolMapType& muellerElements,
				      const PolMapType& muellerElementsIndex,
				      const VisBuffer& vb, 
				      const Float& psScale,
				      PSTerm& psTerm, WTerm& wTerm, ATerm& aTerm,
				      Bool isDryRun)
  {
    // Unused variable from the dark-ages era interface that should ultimately go.
    (void)psScale;
    (void)muellerElementsIndex;
    (void)freqHi;
    LogIO log_l(LogOrigin("AWConvFunc", "fillConvFuncBuffer[R&D]"));
    //    Int ttt=0;
    Complex cfNorm, cfWtNorm;
    //Double vbPA = getPA(vb);
    Complex cpeak,wtcpeak;
    aTerm.cacheVBInfo(vb);

    for (uInt imx=0;imx<muellerElements.nelements();imx++) // Loop over all MuellerElements
      for (uInt imy=0;imy<muellerElements(imx).nelements();imy++)
    	{
	  {
	    for (uInt inu=0;inu<freqValues.nelements();inu++) // All freq. channels
	      {
		Float sampling, samplingWt;
		Int xSupport, ySupport, xSupportWt, ySupportWt;
		CoordinateSystem cs_l;
		// Extract the parameters index by (MuellerElement, Freq, W)
		cfWtb.getParams(cs_l, samplingWt, xSupportWt, ySupportWt, 
				freqValues(inu), 
				//				wValues(iw), 
				wValues(0), 
				muellerElements(imx)(imy));
		cfb.getParams(cs_l, sampling, xSupport, ySupport, 
			      freqValues(inu), 
			      wValues(0), 
			      muellerElements(imx)(imy));
		IPosition pbshp(4,nx,ny,1,1);
		//
		// Cache the A-Term for this polarization and frequency
		//
		Double conjFreq=SynthesisUtils::conjFreq(freqValues(inu),imRefFreq_p);
		Int conjFreqIndex;
		conjFreq=SynthesisUtils::nearestValue(freqValues, conjFreq, conjFreqIndex);

//		cout<<"Muller Array = "<<muellerElements(imx)(imy)<<"\n" ;
		// USEFUL DEBUG MESSAGE
//		 cerr << "Freq. values: " 
//		      << freqValues(inu) << " " 
//		      << imRefFreq_p << " " 
//		      << conjFreq << " " 
//		      << endl;

		CoordinateSystem conjPolCS_l=cs_l;  AWConvFunc::makeConjPolAxis(conjPolCS_l);
		TempImage<Complex> ftATerm_l(pbshp, cs_l), ftATermSq_l(pbshp,conjPolCS_l);
		Int index;
		Vector<Int> conjPol;
		index = conjPolCS_l.findCoordinate(Coordinate::STOKES);
		conjPol = conjPolCS_l.stokesCoordinate(index).stokes();
		//cerr << "ConjPol = " << conjPol << endl;

		// {
		//   // Vector<Double> chanFreq = vb.frequency();
		//   CoordinateSystem skyCS(ftATerm_l.coordinates());
		//   Int index = skyCS.findCoordinate(Coordinate::SPECTRAL);
		//   SpectralCoordinate SpC = skyCS.spectralCoordinate(index);
		//   Vector<Double> refVal = SpC.referenceValue();
		  
		//   Double ff = refVal[0];
		//   cerr << "Freq, ConjFreq: " << freqValues(inu) << " " << conjFreq << " " << ff << endl;
		// }


		Bool doSquint=true;
		//		Bool doSquint=false; Complex tt;
		ftATerm_l.set(Complex(1.0,0.0));   ftATermSq_l.set(Complex(1.0,0.0));

		Int me=muellerElements(imx)(imy);
		if (!isDryRun)
		  {
		    aTerm.applySky(ftATerm_l, vb, doSquint, 0, me, freqValues(inu));//freqHi);
		    // {
		    //   ostringstream name;
		    //   name << "ftATerm" << "_" << inu << "_" << muellerElements(imx)(imy) <<".im";
		    //   storeImg(name,ftATerm_l);
		    // }
		    //tt=max(ftATerm_l.get()); ftATerm_l.put(ftATerm_l.get()/tt);
		    if (conjPB_p) aTerm.applySky(ftATermSq_l, vb, doSquint, 0,me,conjFreq);
		    else aTerm.applySky(ftATermSq_l, vb, doSquint, 0,me,freqValues(inu));
		  }

		//tt=max(ftATermSq_l.get()); ftATermSq_l.put(abs(ftATermSq_l.get()/tt));

		//{
		//   ostringstream name;
		//   name << "ftTermSq" << "_" << muellerElements(imx)(imy) <<".im";
		//   storeImg(name,ftATermSq_l);
		//}
		// TempImage<Complex> ftATermSq_l(pbshp,cs_l);
		// ftATermSq_l.set(Complex(1.0,0.0));
		// aTerm.applySky(ftATermSq_l, vb, false, 0);
		// tt=max(ftATermSq_l.get());
		// ftATermSq_l.put(ftATermSq_l.get()/tt);

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
		      log_l << " CF("
			    << "M:"<<muellerElements(imx)(imy) 
			    << ",C:" << inu 
			    << ",W:" << iw << "): ";
		    // {
		    //   CountedPtr<CFCell> thisCell=cfb.getCFCellPtr(freqValues(inu), wValues(iw), muellerElements(imx)(imy));
		    //   thisCell->conjFreq_p = conjFreq;
		    //   cerr << "ConjFreq: " << thisCell->conjFreq_p << " " << inu << " " << iw << " " << muellerElements(imx)(imy) << endl;
		    // }

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
		    //		    Int inner = cfBufMat.shape()(0)/aTerm.getOversampling();
		    //		    Float inner = 2.0*aTerm.getOversampling()/cfBufMat.shape()(0);

		    //Timer tim;
		    //tim.mark();
		    if (psTerm.isNoOp() || isDryRun)
		      cfBufMat = cfWtBufMat = 1.0;
		    else
		      {
			// psTerm.applySky(cfBufMat, False);   // Assign (psScale set in psTerm.init()
			// psTerm.applySky(cfWtBufMat, False); // Assign
            psTerm.applySky(cfBufMat, s, cfBufMat.shape()(0)/s(0));   // Assign (psScale set in psTerm.init()
	    psTerm.applySky(cfWtBufMat, s, cfWtBufMat.shape()(0)/s(0)); // Assign
			cfWtBuf *= cfWtBuf;
		      }
		    //tim.show("PSTerm*2: ");

		    // WBAWP CODE BEGIN  -- make PS*PS for Weights
		    // psTerm.applySky(cfWtBufMat, true);  // Multiply
		    // WBAWP CODE END

		    // psTerm.applySky(cfBufMat, s, inner/2.0);//pbshp(0)/(os));
		    // psTerm.applySky(cfWtBufMat, s, inner/2.0);//pbshp(0)/(os));

		    // W-term is a unit-amplitude term in the image
		    // doimain.  No need to apply it to the
		    // wt-functions.

		    //tim.mark();
		    if (!isDryRun)
		      {
			wTerm.applySky(cfBufMat, iw, cellSize, wScale, cfBuf.shape()(0));///4);
		      }
		    //tim.show("WTerm: ");
		    // wTerm.applySky(cfWtBufMat, iw, cellSize, wScale, cfWtBuf.shape()(0)/4);

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
		    // WBAWP CODE BEGIN -- ftATermSq_l has conj. PolCS
		    cfWtBuf *= ftATerm_l.get()*conj(ftATermSq_l.get());
		      
		    //tim.mark();
		    //UUU cfWtBuf *= ftATerm_l.get();
		    cfBuf *= ftATerm_l.get();
		    //tim.show("W*A*2: ");
		    // WBAWP CODE END

		    

		    // cfWtBuf = sqrt(cfWtBuf);
		    // psTerm.applySky(cfWtBufMat,true);

		    //tim.mark();
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
		    //tim.mark();
    		    CoordinateSystem cs=twoDPB_l.coordinates();
    		    Int index= twoDPB_l.coordinates().findCoordinate(Coordinate::SPECTRAL);
    		    SpectralCoordinate SpCS = twoDPB_l.coordinates().spectralCoordinate(index);
		    
    		    Double cfRefFreq=SpCS.referenceValue()(0);
    		    Vector<Double> refValue; refValue.resize(1); refValue(0)=cfRefFreq;
    		    SpCS.setReferenceValue(refValue);
    		    cs.replaceCoordinate(SpCS,index);
		    //tim.show("CSStuff:");
		    //
		    // Now FT the function and copy the data from
		    // TempImages back to the CFBuffer buffers
		    //
		    //tim.mark();
		    if (!isDryRun)
		      {
			LatticeFFT::cfft2d(twoDPB_l);
			LatticeFFT::cfft2d(twoDPBSq_l);
		      }
		    //tim.show("FFT*2:");
		    // Array<Complex> t0;
		    // twoDPBSq_l.get(t0); t0 = abs(t0);
		    // twoDPBSq_l.put(t0);

    		    // {
		    //   ostringstream name;
		    //   name << "twoDPB.before" << iw << "_" << inu << "_" << muellerElements(imx)(imy) <<".im";
    		    //   storeImg(name,twoDPB_l);
		    // }
		    // {
		    //   ostringstream name;
		    //   name << "twoDPBSq.before" << iw << "_" << inu << "_" << muellerElements(imx)(imy) <<".im";
    		    //   storeImg(name,twoDPBSq_l);
    		    // }
		    // exit(0);

		    //tim.mark();

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
		    //tim.show("Slicer*2:");
		    //
		    // Finally, resize the buffers, limited to the
		    // support size determined by the threshold
		    // suppled by the ATerm (done internally in
		    // resizeCF()).  Transform the co-ord. system to
		    // the FT domain set the co-ord. sys. and modified
		    // support sizes.
		    //
		    //tim.mark();
		    Int supportBuffer = (Int)(getOversampling(psTerm, wTerm, aTerm)*2.0);
		    if (!isDryRun)
		      {
			if (iw==0) wtcpeak = max(cfWtBuf);
			cfWtBuf /= wtcpeak;
		      }
		    //tim.show("Norm");

		    //tim.mark();
		    if (!isDryRun)
		      AWConvFunc::resizeCF(cfWtBuf, xSupportWt, ySupportWt, supportBuffer, samplingWt,0.0);
		    //log_l << "CF WT Support: " << xSupport << " (" << xSupportWt << ") " << "pixels" <<  LogIO::POST;
		    //tim.show("Resize:");

		    //tim.mark();
		    Vector<Double> ftRef(2);
		    ftRef(0)=cfWtBuf.shape()(0)/2.0;
		    ftRef(1)=cfWtBuf.shape()(1)/2.0;
		    CoordinateSystem ftCoords=cs_l;
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

		    //cerr << "AWConvFunc: Telescope name = " << cfCellPtr->telescopeName_p << " " << aTerm.getTelescopeName() << endl;
		    //tim.show("CSStuff:");
		    // setUpCFSupport(cfBuf, xSupport, ySupport, sampling);
		    //		    if (iw==0) 
		    //tim.mark();
		    //Int supportBuffer = (Int)(aTerm->getOversampling()*1.5);

		    if (!isDryRun)
		      {
			cpeak = max(cfBuf);
			cfBuf /= cpeak;
		      }
		    //tim.show("Peaknorm:");
    		    // {
    		    //   ostringstream name;
    		    //   name << "twoDPB.after" << iw << "_" << inu << "_" << muellerElements(imx)(imy) << ".im";
    		    //   storeImg(name,twoDPB_l);
    		    //   // name << "twoDPBSq.after" << iw << "_" << inu << "_" << muellerElements(imx)(imy) << ".im";
    		    //   // storeImg(name,twoDPBSq_l);
    		    // }

		    if (!isDryRun)
		      AWConvFunc::resizeCF(cfBuf, xSupport, ySupport, supportBuffer, sampling,0.0);

		    if (!isDryRun)
		      log_l << "CF Support: " << xSupport << " (" << xSupportWt << ") " << "pixels" <<  LogIO::POST;

		    // cfb.getCFCellPtr(freqValues(inu), wValues(iw), muellerElement)->storage_p->assign(cfBuf);
		    // ftRef(0)=cfBuf.shape()(0)/2-1;
		    // ftRef(1)=cfBuf.shape()(1)/2-1;
		    ftRef(0)=cfBuf.shape()(0)/2.0;
		    ftRef(1)=cfBuf.shape()(1)/2.0;

		    //tim.mark();
		    cfNorm=cfWtNorm=1.0;
		    //if ((iw == 0) && (!isDryRun))
		    if (!isDryRun)
		      {
			cfNorm=0; cfWtNorm=0;
			cfNorm = AWConvFunc::cfArea(cfBufMat, xSupport, ySupport, sampling);
			cfWtNorm = AWConvFunc::cfArea(cfWtBufMat, xSupportWt, ySupportWt, sampling);
		      }
		    //tim.show("Area*2:");

		    //tim.mark();
		    cfBuf /= cfNorm;
		    cfWtBuf /= cfWtNorm;
		    //tim.show("cfNorm*2:");

		    //tim.mark();
		    ftCoords=cs_l;
		    SynthesisUtils::makeFTCoordSys(cs_l, cfBuf.shape()(0), ftRef, ftCoords);

		    cfb.setParams(inu,iw,imx,imy,//muellerElements(imx)(imy),
				  freqValues(inu), String(""), wValues(iw), muellerElements(imx)(imy),
				  ftCoords, sampling, xSupport, ySupport,
				  String(""), // Default ==> Don't set in the CFCell
				  conjFreq, conjPol[0]);
		    cfCellPtr=cfb.getCFCellPtr(freqValues(inu), wValues(iw), 
					       muellerElements(imx)(imy));
		    cfCellPtr->pa_p=Quantity(vbPA,"rad");
		    cfCellPtr->telescopeName_p = aTerm.getTelescopeName();
		    cfCellPtr->isRotationallySymmetric_p = aTerm.isNoOp();
		    //
		    // Now tha the CFs have been computed, cache its
		    // paramters in CFCell for quick access in tight
		    // loops (in the re-sampler, e.g.).
		    //

		    (cfWtb.getCFCellPtr(freqValues(inu), wValues(iw), 
					muellerElements(imx)(imy)))->initCache(isDryRun);
		    (cfb.getCFCellPtr(freqValues(inu), wValues(iw), 
				      muellerElements(imx)(imy)))->initCache(isDryRun);

		    //tim.show("End*2:");
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
    LogIO log_l(LogOrigin("AWConvFunc","cfArea"));
    Complex cfNorm=0;
    Int origin=cf.shape()(0)/2;
    Float peak=0;
    IPosition ndx(4,0,0,0,0);
    IPosition peakPix(ndx);
    peakPix = 0;
    for(ndx(1)=0;ndx(1)<cf.shape()(1);ndx(1)++)
      for(ndx(0)=0;ndx(0)<cf.shape()(0);ndx(0)++)
	if (abs(cf(ndx)) > peak) {peakPix = ndx;peak=abs(cf(ndx));}
    //    origin = peakPix(0);

    //    int peakNIC=0;

    if (origin != peakPix(0))
      {
	log_l << "Peak not at the center " << origin << " " << cf(IPosition(4,origin,origin,0,0)) << " " << peakPix << " " << peak << LogIO::NORMAL1;
	//	peakNIC=1e7;
      }
    for (Int ix=-xSupport;ix<xSupport;ix++)
      for (int iy=-ySupport;iy<ySupport;iy++)
	{
	  //cfNorm += Complex(real(cf(ix*(Int)sampling+origin, iy*(Int)sampling+origin)),0.0);
	  cfNorm += (cf(ix*(Int)sampling+origin, iy*(Int)sampling+origin));
	  // cerr << cfNorm << " " << ix << " " << iy << " " << ix*(Int)sampling+origin << " " << iy*(Int)sampling+origin
	  //      << real(cf(ix*(Int)sampling+origin, iy*(Int)sampling+origin)) << endl;
	}
    //    cf /= cfNorm;

    return cfNorm;//+Complex(peakNIC,0.0);
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
  Matrix<Double> AWConvFunc::getFreqRangePerSpw(const VisBuffer& vb)
  {
    //
    // Find the total effective bandwidth
    //
    Cube<Double> fminmax;
    Double fMax=0, fMin=MAX_FREQ;
    ArrayColumn<Double> spwCol=vb.msColumns().spectralWindow().chanFreq();
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
					     const VisBuffer& vb, 
					     const ImageInterface<Complex>& uvGrid,
					     Vector<String>& bandNames)
  {
    (void)uvGrid; (void)dNU; (void)vb;
    Vector<Double> fValues;
    if (wbAWP_p==false)
      {
	// Return the sky-image ref. freq.
	fValues.resize(1);
	fValues[0]=imRefFreq_p;

	// // Return the max. freq. from the list of selected SPWs
	// fValues.resize(1);
	// Double maxFreq=0.0;
	// for (Int i=0;i<spwFreqSelection_p.shape()(0);i++)
	//   if (spwFreqSelection_p(i,2) > maxFreq) maxFreq=spwFreqSelection_p(i,2);
	// fValues[0]=maxFreq;
      }
    else
      {
	Int nSpw;

	// USEFUL DEBUG MESSAGE
	//cerr << "##### Min. Max. Freq. per Spw: " << spwFreqSelection_p << " " << spwFreqSelection_p.shape() <<endl;

	nSpw = spwFreqSelection_p.shape()(0);
	fValues.resize(nSpw);
	bandNames.resize(nSpw);
	//	dNU = (spwFreqSelection_p(0,1) - spwFreqSelection_p(0,2));
	ScalarColumn<String> spwNames=vb.msColumns().spectralWindow().name();
	for(Int i=0;i<nSpw;i++) 
	  {
	    int s=spwFreqSelection_p(i,0);
	    cerr << "Spw"<<s<<": " << spwNames.getColumn()[s] << endl;
	    
	    bandNames(i)=spwNames.getColumn()[s];
	    fValues(i)=spwFreqSelection_p(i,2);
	    // Int j=0;
	    // while (j*dNU+spwFreqSelection_p(i,1) <= spwFreqSelection_p(i,2))
	    //   {
	    //     fValues.resize(j+1,true); 
	    //     //	fValues(j)=spwFreqSelection_p(i,2); // Pick up the max. freq. for each selected SPW
	    //     fValues(j)=j*dNU+spwFreqSelection_p(i,1);
	    //     j=fValues.nelements();
	    //   }
	  }
	//    cerr << "Max. freq. per SPW = " << fValues << endl;
      }
    return fValues;
  }
  //
  //----------------------------------------------------------------------
  //
  void AWConvFunc::makeConvFunction(const ImageInterface<Complex>& image,
				    const VisBuffer& vb,
				    const Int wConvSize,
				    const CountedPtr<PolOuterProduct>& pop,
				    const Float pa,
				    const Float dpa,
				    const Vector<Double>& uvScale, const Vector<Double>& /*uvOffset*/,
				    const Matrix<Double>& ,//vbFreqSelection,
				    CFStore2& cfs2,
				    CFStore2& cfwts2,
				    Bool fillCF)
  {
    LogIO log_l(LogOrigin("AWConvFunc", "makeConvFunction[R&D]"));
    Int convSize, convSampling, polInUse;
    Double wScale=0.0; Int bandID_l=-1;
    Array<Complex> convFunc_l, convWeights_l;
    Double cfRefFreq=-1, freqScale=1e8;
    Quantity paQuant(pa,"rad");

    
    Int nx=image.shape()(0);//, ny=image.shape()(1);
    if (bandID_l == -1) bandID_l=getVisParams(vb,image.coordinates());
    
    log_l << "Making a new convolution function for PA="
	  << pa*(180/C::pi) << "deg"
	  << " for field ID " << vb.fieldId();
    // log_l << "TimeStamps(0-10) ";
    // for (Int i=0;i<10;i++) 
    //   //log_l << MVTime(vb.time()(i)).string(MVTime::TIME) << " ";
    //   log_l << vb.time()(i)/1e8 << " ";
    // log_l << LogIO::NORMAL << LogIO::POST;
    
    if(wConvSize>0) 
      {
	log_l << "Using " << wConvSize << " planes for W-projection" << LogIO::POST;
	Double maxUVW;
	maxUVW=0.25/abs(image.coordinates().increment()(0));
	log_l << "Estimating maximum possible W = " << maxUVW
	      << " (wavelengths)" << LogIO::POST;
	
	Double invLambdaC=vb.frequency()(0)/C::c;
	Double invMinL = vb.frequency()((vb.frequency().nelements())-1)/C::c;
	log_l << "wavelength range = " << 1.0/invLambdaC << " (m) to " 
	      << 1.0/invMinL << " (m)" << LogIO::POST;
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
    //convSampling=aTerm_p->getOversampling();
    convSampling=getOversampling(*psTerm_p, *wTerm_p, *aTerm_p);
    convSize=aTerm_p->getConvSize();
//    cout<<"Conv Sampling listed in aipsrc is : "<<convSampling<<endl;
//    cout<<"Conv Size is : "<<convSize<<endl;
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
    sampling = dc.increment();
//    cout<<"The image sampling is set to :"<<sampling<<endl; 
    sampling*=Double(convSampling);
    sampling*=Double(nx)/Double(convSize);
//    cout<<"The resampled increment is :"<<sampling<<endl;
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
    //    Int inner=convSize/convSampling;
    //    CFStore2 cfs2_p, cfwts2_p;
    CountedPtr<CFBuffer> cfb_p, cfwtb_p;
    // cfs2.rememberATerm(aTerm_p);
    // cfwts2.rememberATerm(aTerm_p);
    
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
    // outer-product.  This probably OK for antena arrays. After the
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

    //cerr << "AWCF: " << polMap << endl << polIndexMap << endl << conjPolMap << endl << conjPolIndexMap << endl;
    
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
    //Quantity dPA(360.0,"deg");
    Quantity dPA(dpa,"rad");
    Int totalCFs=uniqueBaselineTypeList.shape().product()*wConvSize*freqValues.nelements()*polMap.shape().product();
    ProgressMeter pm(1.0, Double(totalCFs), "makeCF", "","","",true);
    int cfDone=0;
    for(Int ib=0;ib<uniqueBaselineTypeList.shape()(0);ib++)
      {
	Vector<Int> pos;
	pos=cfs2.resize(paQuant, dPA, uniqueBaselineTypeList(ib,0), uniqueBaselineTypeList(ib,1)); 
	pos=cfwts2.resize(paQuant, dPA, uniqueBaselineTypeList(ib,0), uniqueBaselineTypeList(ib,1)); 
	//
	// Re-size the CFBuffer object.  It holds the 2D convolution
	// functions index by (FreqValue, WValue, MuellerElement).
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
	
	Matrix<Complex> screen(convSize, convSize);
	// WTerm wterm_l;
	// PSTerm psTerm_l;

	//Initiate construction of the "ConvolveGridder" object inside
	//PSTerm.  This is, for historical reasons, used to access the
	//"standard" Prolate Spheroidal function implementaion.  This
	//however should be replaced with a simpler access, direct
	//access the PS function implementation (in Utils.h
	//SynthesisUtils::libreSpheroidal() - but this needs more
	//testing).
	Int inner=convSize/(convSampling);
	// Float psScale= (image.coordinates().increment()(0)*nx) /
	//   (coords.increment()(0)*screen.shape()(0));

	Float psScale = (2*coords.increment()(0))/(nx*image.coordinates().increment()(0)),
	  innerQuaterFraction=1.0;
	// psScale when using SynthesisUtils::libreSpheroidal() is
	// 2.0/nSupport.  nSupport is in pixels and the 2.0 is due to
	// the center being at Nx/2.  Here the nSupport is determined

	// by the sky-image and is equal to convSize/convSampling.
	psScale = 2.0/(innerQuaterFraction*convSize/convSampling);// nx*image.coordinates().increment()(0)*convSampling/2;
	Vector<Double> uvOffset_cf(3,0); uvOffset_cf(0)=uvOffset_cf(2)=convSize/2; 
	psTerm_p->init(IPosition(2,inner,inner), uvScale, uvOffset_cf,psScale);

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
	  Double vbPA = getPA(vb), freqHi;

	  
	  Vector<Double> chanFreq = vb.frequency();
	  index = image.coordinates().findCoordinate(Coordinate::SPECTRAL);
	  SpectralCoordinate SpC = cfb_cs.spectralCoordinate(index);
	  Vector<Double> refVal = SpC.referenceValue();
	
	  freqHi = refVal[0];
	  fillConvFuncBuffer(*cfb_p, *cfwtb_p, convSize, convSize, freqValues, wValues, wScale,
			     vbPA, freqHi,
			     polMap, polIndexMap, vb, psScale,
			     *psTerm_p, *wTerm_p, *aTerm_p, !fillCF);
	}
	// cfb_p->show(NULL,cerr);
	//cfb_p->makePersistent("test.cf");
	// cfwtb_p->makePersistent("test.wtcf");
	
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
  Bool AWConvFunc::setUpCFSupport(Array<Complex>& cfVals, Int& xSupport, Int& ySupport,
				  const Float& sampling, const Complex& peak)
  {
    LogIO log_l(LogOrigin("AWConvFunc", "setUpCFSupport[R&D]"));
    //
    // Find the convolution function support size.  No assumption
    // about the symmetry of the conv. func. can be made (except that
    // they are same for all poln. planes).
    //
    xSupport = ySupport = -1;
    Int convFuncOrigin=cfVals.shape()[0]/2, R; 
    Bool found=false;
    Float threshold;
    // Threshold as a fraction of the peak (presumed to be the center pixel).
    if (abs(peak) != 0) threshold = real(abs(peak));
    else 
      threshold   = real(abs(cfVals(IPosition(4,convFuncOrigin,convFuncOrigin,0,0))));

    //threshold *= aTerm_p->getSupportThreshold();
    threshold *= 1e-3;
    //    threshold *=  0.1;
    // if (aTerm_p->isNoOp()) 
    //   threshold *= 1e-3; // This is the threshold used in "standard" FTMchines
    // else

    //
    // Find the support size of the conv. function in pixels
    //
    // Timer tim;
    // tim.mark();
    if ((found = AWConvFunc::awFindSupport(cfVals,threshold,convFuncOrigin,R)))
      xSupport=ySupport=Int(0.5+Float(R)/sampling)+1;
    // tim.show("findSupport:");

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
	log_l << "Convolution function support size > N/2.  Limiting it to N/2 "
	      << "(threshold = " << threshold << ")."
	      << LogIO::WARN;
	xSupport = ySupport = (Int)(convFuncOrigin/sampling-1);
      }

    if(xSupport<1) 
      log_l << "Convolution function is misbehaved - support seems to be zero"
	    << LogIO::EXCEPTION;
    return found;
  }
  //
  //----------------------------------------------------------------------
  //
  Bool AWConvFunc::resizeCF(Array<Complex>& cfVals, Int& xSupport, Int& ySupport,
			    const Int& supportBuffer, const Float& sampling, const Complex& peak)
  {
    LogIO log_l(LogOrigin("AWConvFunc", "resizeCF[R&D]"));
    Int ConvFuncOrigin=cfVals.shape()[0]/2;  // Conv. Func. is half that size of convSize
    
    Bool found = setUpCFSupport(cfVals, xSupport, ySupport, sampling,peak);

    //Int supportBuffer = (Int)(aTerm_p->getOversampling()*1.5);
    Int bot=(Int)(ConvFuncOrigin-sampling*xSupport-supportBuffer),//-convSampling/2, 
      top=(Int)(ConvFuncOrigin+sampling*xSupport+supportBuffer);//+convSampling/2;
    //    bot *= 2; top *= 2;
    bot = max(0,bot);
    top = min(top, cfVals.shape()(0)-1);
    
    Array<Complex> tmp;
    IPosition blc(4,bot,bot,0,0), trc(4,top,top,0,0);
    //
    // Cut out the conv. func., copy in a temp. array, resize the
    // CFStore.data, and copy the cutout version to CFStore.data.
    //
    tmp = cfVals(blc,trc);
    cfVals.resize(tmp.shape());
    cfVals = tmp; 
    return found;
  }
  //
  //----------------------------------------------------------------------
  // A global method for use in OMP'ed findSupport() below
  //
  void archPeak(const Float& threshold, const Int& origin, const Block<Int>& cfShape, const Complex* cfValsPtr, 
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
	  //vals[pix]=cfVals(ndx);
	  vals[pix]=cfValsPtr[ndx[0]+ndx[1]*cfShape[1]+ndx[2]*cfShape[2]+ndx[3]*cfShape[3]];
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
  Bool AWConvFunc::findSupport(Array<Complex>& cfVals, Float& threshold, 
			       Int& origin, Int& radius)
  {
    return awFindSupport(cfVals, threshold, origin, radius);
  }
  Bool AWConvFunc::awFindSupport(Array<Complex>& cfVals, Float& threshold, 
			       Int& origin, Int& radius)
  {
    LogIO log_l(LogOrigin("AWConvFunc", "findSupport[R&D]"));

    Int nCFS=cfVals.shape().nelements(),
      PixInc=1, R0, R1, R, convSize;
    Block<Int> cfShape(nCFS);
    Bool found=false;
    Complex *cfValsPtr;
    Bool dummy;
    uInt Nth=1, threadID=0;

    for (Int i=0;i<nCFS;i++)
    	cfShape[i]=cfVals.shape()[i];
    convSize = cfShape[0];

#ifdef _OPENMP
    Nth = max(omp_get_max_threads()-2,1);
#endif
    
    Block<Int> maxR(Nth);

    cfValsPtr = cfVals.getStorage(dummy);

    R1 = convSize/2-2;

    while (R1 > 1)
      {
	    R0 = R1; R1 -= Nth;

//#pragma omp parallel default(none) firstprivate(R0,R1)  private(R,threadID) shared(origin,threshold,PixInc,maxR,cfShape,nCFS,cfValsPtr) num_threads(Nth)
#pragma omp parallel firstprivate(R0,R1)  private(R,threadID) shared(PixInc,maxR,cfShape,nCFS,cfValsPtr) num_threads(Nth)
	    { 
#pragma omp for
	      for(R=R0;R>R1;R--)
		{
#ifdef _OPENMP
		  threadID=omp_get_thread_num();
#endif
		  archPeak(threshold, origin, cfShape, cfValsPtr, nCFS, PixInc, threadID, R, maxR);
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
  // Bool AWConvFunc::findSupport(Array<Complex>& func, Float& threshold,
  // 			       Int& origin, Int& R)
  // {
  //   LogIO log_l(LogOrigin("AWConvFunc", "findSupport[R&D]"));
  //   Double NSteps;
  //   Int PixInc=1;
  //   Vector<Complex> vals;
  //   IPosition ndx(4,origin,0,0,0);
  //   Bool found=false;
  //   IPosition cfShape=func.shape();
  //   Int convSize = cfShape(0);

  //   for(R=convSize/2-2;R>1;R--)
  //     {
  // 	//Check every PixInc pixel along a circle of radius R
  // 	NSteps = 90*R/PixInc; 
  // 	vals.resize((Int)(NSteps+0.5));
  // 	vals=0;
  // 	for(Int th=0;th<NSteps;th++)
  // 	  {
  // 	    ndx(0)=(int)(origin + R*sin(2.0*M_PI*th*PixInc/R));
  // 	    ndx(1)=(int)(origin + R*cos(2.0*M_PI*th*PixInc/R));
	    
  // 	    if ((ndx(0) < cfShape(0)) && (ndx(1) < cfShape(1)))
  // 	      vals(th)=func(ndx);
  // 	  }

  // 	if (max(abs(vals)) > threshold)
  // 	  {found=true;break;}
  //     }
  //   return found;
  // }
  //
  //----------------------------------------------------------------------
  //
  Bool AWConvFunc::makeAverageResponse(const VisBuffer& vb, 
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
  Bool AWConvFunc::makeAverageResponse(const VisBuffer& vb, 
				       const ImageInterface<Complex>& image,
				       ImageInterface<Complex>& theavgPB,
				       Bool reset)
  {
    LogIO log_l(LogOrigin("AWConvFunc","makeAverageResponse(Complex)[R&D]"));
    
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
    LogIO log_l(LogOrigin("AWConvFunc", "normalizeAvgPB[R&D]"));
    
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
  //-------------------------------------------------------------------------
  // Legacy code.  Should ultimately be deleteted after re-facatoring
  // is finished.
  //
  Bool AWConvFunc::makeAverageResponse_org(const VisBuffer& vb, 
					   const ImageInterface<Complex>& image,
					   ImageInterface<Float>& theavgPB,
					   Bool reset)
  {
    LogIO log_l(LogOrigin("AWConvFunc", "makeAverageResponse_org[R&D]"));
    TempImage<Float> localPB;
    
    log_l << "Making the average response for " 
	  << aTerm_p->name() 
	  << LogIO::NORMAL << LogIO::POST;
    
    localPB.resize(image.shape()); localPB.setCoordinateInfo(image.coordinates());
    if (reset)
      {
	log_l << "Initializing the average PBs" << LogIO::NORMAL << LogIO::POST;
	theavgPB.resize(localPB.shape()); 
	theavgPB.setCoordinateInfo(localPB.coordinates());
	theavgPB.set(0.0);
      }
    //
    // Make the Stokes PB
    //
    localPB.set(1.0);
    
    // Block<CountedPtr<ImageInterface<Float > > > tmpBlock(1);
    // tmpBlock[0]=CountedPtr<ImageInterface<Float> >(&localPB, false);
    // aTerm_p->applySky(tmpBlock, vb, 0, false);
    aTerm_p->applySky(localPB, vb, false, 0);
    
    IPosition twoDPBShape(localPB.shape());
    TempImage<Complex> localTwoDPB(twoDPBShape,localPB.coordinates());
    //    localTwoDPB.setMaximumCacheSize(cachesize);
    Int NAnt;
    NAnt=1;
    
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
	// Accumulate the shifted PBs
	//
	{
	  Bool isRefF;
	  Array<Float> fbuf;
	  Array<Complex> cbuf;
	  isRefF=theavgPB.get(fbuf);
	  //isRefC=localTwoDPB.get(cbuf);
	  
	  IPosition fs(fbuf.shape());
	  IPosition ndx(4,0,0,0,0),avgNDX(4,0,0,0,0);
	  for(ndx(3)=0,avgNDX(3)=0;ndx(3)<fs(3);ndx(3)++,avgNDX(3)++)
	    for(ndx(2)=0,avgNDX(2)=0;ndx(2)<twoDPBShape(2);ndx(2)++,avgNDX(2)++)
	      for(ndx(0)=0,avgNDX(0)=0;ndx(0)<fs(0);ndx(0)++,avgNDX(0)++)
		for(ndx(1)=0,avgNDX(1)=0;ndx(1)<fs(1);ndx(1)++,avgNDX(1)++)
		  {
		    Float val;
		    val = real(cbuf(ndx));
		    fbuf(avgNDX) += val;
		  }
	  if (!isRefF) theavgPB.put(fbuf);
	}
      }
    theavgPB.setCoordinateInfo(localPB.coordinates());
    return true; // i.e., an average PB was made
  }
  //
  //----------------------------------------------------------------------
  //
  void AWConvFunc::prepareConvFunction(const VisBuffer& vb, VBRow2CFBMapType& theMap)
  {
    if (aTerm_p->rotationallySymmetric() == false) return;
    Int nRow=theMap.nelements();
    // CountedPtr<CFBuffer> cfb, cbPtr;
    // CountedPtr<CFCell>  cfc;
    // CountedPtr<ATerm> aTerm_l=aTerm_p;
    CFBuffer *cfb, *cbPtr=0;
    CFCell  *cfc, *baseCFC=NULL;
    ATerm *aTerm_l=&*aTerm_p;
    
    cfb=&*(theMap(0));
    cfc = &*(cfb->getCFCellPtr(0,0,0));
    Double actualPA = getPA(vb), currentCFPA = cfc->pa_p.getValue("rad");
    Double dPA = currentCFPA-actualPA;

    if (fabs(dPA) <= fabs(rotateCFOTFAngleRad_p)) return;

    LogIO log_l(LogOrigin("AWConvFunc", "prepareConvFunction"));

//     Int Nth=1;
// #ifdef _OPENMP
//     Nth=max(omp_get_max_threads()-2,1);
// #endif
    for (Int irow=0;irow<nRow;irow++)
      {
	cfb=&*(theMap(irow));
	//	if ((!cfb.null()) && (cfb != cbPtr))
	if ((cfb!=NULL) && (cfb != cbPtr))
	  {
	    // baseCFB_p = cfb->clone();
	    // cerr << "NRef = " << baseCFB_p.nrefs() << endl;
	    //
	    // If the following messsage is emitted more than once, we
	    // are in a heterogeneous-array case
	    //
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
  // REFACTORED CODE
  //

  //
  //----------------------------------------------------------------------
  //
  void AWConvFunc::fillConvFuncBuffer2(CFBuffer& cfb, CFBuffer& cfWtb,
				       const Int& nx, const Int& ny, 
				       const ImageInterface<Complex>& skyImage,
				       //const CoordinateSystem& skyCoords,
				       const CFCStruct& miscInfo,
				       PSTerm& psTerm, WTerm& wTerm, ATerm& aTerm,
				       Bool conjPB)

  {
    LogIO log_l(LogOrigin("AWConvFunc", "fillConvFuncBuffer2[R&D]"));
    Complex cfNorm, cfWtNorm;
    Complex cpeak;
    {
      Float sampling, samplingWt;
      Int xSupport, ySupport, xSupportWt, ySupportWt;
      CoordinateSystem cs_l;
      // Extract the parameters index by (MuellerElement, Freq, W)
      cfWtb.getParams(cs_l, samplingWt, xSupportWt, ySupportWt, 
		      miscInfo.freqValue,
		      //				wValues(iw), 
		      miscInfo.wValue, 
		      miscInfo.muellerElement);
      cfb.getParams(cs_l, sampling, xSupport, ySupport, 
		    miscInfo.freqValue,
		    miscInfo.wValue, 
		    miscInfo.muellerElement);
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

	if (conjPB) aTerm.applySky(ftATermSq_l, vbPA, doSquint, 0,miscInfo.muellerElement,conjFreq);
	else aTerm.applySky(ftATermSq_l, vbPA, doSquint, 0,miscInfo.muellerElement,freq_l);
      }

      Vector<Double> cellSize;
      {
	CoordinateSystem skyCoords(skyImage.coordinates());
	Int directionIndex=skyCoords.findCoordinate(Coordinate::DIRECTION);
	DirectionCoordinate dc=skyCoords.directionCoordinate(directionIndex);
	cellSize = dc.increment()*(Double)(miscInfo.sampling*skyImage.shape()(0)/nx); // nx is the size of the CF buffer
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
	//Timer tim;
	//tim.mark();
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

	//tim.mark();
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
	// WBAWP CODE BEGIN -- ftATermSq_l has conj. PolCS
	cfWtBuf *= ftATerm_l.get()*conj(ftATermSq_l.get());

	//tim.mark();
	cfBuf *= ftATerm_l.get();
	//tim.show("W*A*2: ");
	// WBAWP CODE END
	//tim.mark();
	twoDPB_l.putSlice(cfBuf, PolnPlane);
	twoDPBSq_l.putSlice(cfWtBuf, PolnPlane);
	//tim.show("putSlice:");

	// To accumulate avgPB2, call this function. 
	// PBSQWeight
	// Bool PBSQ = false;
	// if(PBSQ) makePBSq(twoDPBSq_l); 
		    
	//
	// Set the ref. freq. of the co-ordinate system to
	// that set by ATerm::applySky().
	//
	//tim.mark();
	CoordinateSystem cs=twoDPB_l.coordinates();
	Int index= twoDPB_l.coordinates().findCoordinate(Coordinate::SPECTRAL);
	SpectralCoordinate SpCS = twoDPB_l.coordinates().spectralCoordinate(index);
		    
	Double cfRefFreq=SpCS.referenceValue()(0);
	Vector<Double> refValue; refValue.resize(1); refValue(0)=cfRefFreq;
	SpCS.setReferenceValue(refValue);
	cs.replaceCoordinate(SpCS,index);
	
	//tim.mark();
	{
	  LatticeFFT::cfft2d(twoDPB_l);
	  LatticeFFT::cfft2d(twoDPBSq_l);
	}
	//tim.show("FFT*2:");

	//tim.mark();
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
	//tim.show("Slicer*2:");

	//tim.mark();
	Int supportBuffer = (Int)(AWConvFunc::getOversampling(psTerm, wTerm, aTerm)*2.0);

	AWConvFunc::resizeCF(cfWtBuf, xSupportWt, ySupportWt, supportBuffer, samplingWt,0.0);
	//tim.show("Resize:");

	//tim.mark();
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
	//tim.show("CSStuff:");

	//tim.mark();
	{
	  cpeak = max(cfBuf);
	  cfBuf /= cpeak;
	}
	//tim.show("Peaknorm:");

	AWConvFunc::resizeCF(cfBuf, xSupport, ySupport, supportBuffer, sampling,0.0);

	log_l << "CF Support: " << xSupport << " (" << xSupportWt << ") " << "pixels" <<  LogIO::POST;
	
	ftRef(0)=cfBuf.shape()(0)/2.0;
	ftRef(1)=cfBuf.shape()(1)/2.0;

	//tim.mark();
	//	if (miscInfo.wValue == 0)
	{
	  cfNorm=0; cfWtNorm=0;
	  cfNorm = AWConvFunc::cfArea(cfBufMat, xSupport, ySupport, sampling);
	  cfWtNorm = AWConvFunc::cfArea(cfWtBufMat, xSupportWt, ySupportWt, sampling);	    
	}
	//tim.show("Area*2:");

	//==============================================================
	  // thisCell=cfb.getCFCellPtr(miscInfo.freqValue, miscInfo.wValue, miscInfo.muellerElement);
	  // if (real(cfNorm) > 1e7)
	  //   {
	  //     cfNorm -= Complex(1e7,0.0);
	  //     ostringstream name;
	  //     name << "WT_" << miscInfo.wValue;
	  //     thisCell->makePersistent("./",name.str().c_str());
	  //   }
	  // if (real(cfWtNorm) > 1e7)
	  //   {
	  //     cfWtNorm -= Complex(1e7,0.0);
	  //     // ostringstream name;
	  //     // name << "CFWT_" << miscInfo.wValue;
	  //     // storeArrayAsImage(name,ftCoords, cfWtBufMat);
	  //   }
	//==============================================================

	
	//tim.mark();
	cfBuf /= cfNorm;
	cfWtBuf /= cfWtNorm;
	//tim.show("cfNorm*2:");

	//tim.mark();
	ftCoords=cs_l;
	SynthesisUtils::makeFTCoordSys(cs_l, cfBuf.shape()(0), ftRef, ftCoords);
	//CountedPtr<CFCell>

	thisCell=cfb.getCFCellPtr(miscInfo.freqValue, miscInfo.wValue, miscInfo.muellerElement);
	thisCell->pa_p=Quantity(vbPA,"rad");
	thisCell->coordSys_p = ftCoords;
	thisCell->xSupport_p = xSupport;
	thisCell->ySupport_p = ySupport;
	thisCell->isRotationallySymmetric_p = aTerm.isNoOp();

	(cfWtb.getCFCellPtr(miscInfo.freqValue, miscInfo.wValue, miscInfo.muellerElement))->initCache();
	(cfb.getCFCellPtr(miscInfo.freqValue, miscInfo.wValue, miscInfo.muellerElement))->initCache();
	//tim.show("End*2:");
      }
    }
  }


  //
  //----------------------------------------------------------------------
  //
  void AWConvFunc::makeConvFunction2(const String& cfCachePath,
				     const Vector<Double>& uvScale, const Vector<Double>& uvOffset,
				     const Matrix<Double>& ,//vbFreqSelection,
				     CFStore2& cfs2,
				     CFStore2& cfwts2,
				     const Bool psTermOn,
				     const Bool aTermOn,
				     const Bool conjBeams)
  {
    LogIO log_l(LogOrigin("AWConvFunc", "makeConvFunction2[R&D]"));
    Int convSize, convSampling;//, polInUse;
    Array<Complex> convFunc_l, convWeights_l;
    //  
    // Get the coordinate system
    //
    const String uvGridDiskImage=cfCachePath+"/"+"uvgrid.im";
    PagedImage<Complex> image_l(uvGridDiskImage);//cfs2.getCacheDir()+"/uvgrid.im");
    CoordinateSystem coords(image_l.coordinates());
    
    //Int nx=image_l.shape()(0);//, ny=image.shape()(1);
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

	    IPosition cfbShape = cfb_p->shape();

	    for (int iNu=0; iNu<cfbShape(0); iNu++)       // Frequency axis
	      for (int iPol=0; iPol<cfbShape(2); iPol++)     // Polarization axis
		for (int iW=0; iW<cfbShape(1); iW++)   // W axis
		  {
		    CFCStruct miscInfo;
		    CoordinateSystem cs_l;
		    Int xSupport, ySupport;
		    Float sampling;

		    CountedPtr<CFCell>& tt=(*cfb_p).getCFCellPtr(iNu, iW, iPol);
		    //cerr << "####@#$#@$@ " << iNu << " " << iW << " " << iPol << endl;
		    //tt->show("test",cout);
		    if (tt->cfShape_p.nelements() != 0)
		       {
			 (*cfb_p)(iNu,iW,iPol).getAsStruct(miscInfo); // Get misc. info. for this CFCell

			 wbAWP=True; // Always true since the Freq. value is got from the coord. sys.
			 wTermOn=(miscInfo.wValue > 0.0);

			 CountedPtr<ConvolutionFunction> awCF = AWProjectFT::makeCFObject(miscInfo.telescopeName,
											  aTermOn, psTermOn, wTermOn, True, wbAWP, conjBeams);
			 (static_cast<AWConvFunc &>(*awCF)).aTerm_p->cacheVBInfo(miscInfo.telescopeName, miscInfo.diameter);
			 //aTerm_p->cacheVBInfo(miscInfo.telescopeName, miscInfo.diameter);

			 cfb_p->getParams(cs_l, sampling, xSupport, ySupport,iNu,iW,iPol);
			 convSampling=miscInfo.sampling;

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
			 
			 Matrix<Complex> screen(convSize, convSize);
			 
			 Int inner=convSize/(convSampling);
			 //Float psScale = (2*coords.increment()(0))/(nx*image.coordinates().increment()(0));
			 Float innerQuaterFraction=1.0;
			 
			 Float psScale = 2.0/(innerQuaterFraction*convSize/convSampling);// nx*image.coordinates().increment()(0)*convSampling/2;
			 ((static_cast<AWConvFunc &>(*awCF)).psTerm_p)->init(IPosition(2,inner,inner), uvScale, uvOffset,psScale);
			 
			 //
			 // By this point,  all the 4 axis (Time/PA, Freq, Pol, Baseline)
			 // of the CFBuffer objects have been setup.  The CFs will now
			 // be filled using the supplied PS-, W- and the A-term objects.
			 //
			 
			 AWConvFunc::fillConvFuncBuffer2(*cfb_p, *cfwtb_p, convSize, convSize, 
							 image_l,//coords,
							 miscInfo,
							 *((static_cast<AWConvFunc &>(*awCF)).psTerm_p),
							 *((static_cast<AWConvFunc &>(*awCF)).wTerm_p),
							 *((static_cast<AWConvFunc &>(*awCF)).aTerm_p),
							 conjBeams);
					     
			 //				     *psTerm_p, *wTerm_p, *aTerm_p);
			 //cfb_p->show(NULL,cerr);
			 //
			 // Make the CFStores persistent.
			 //
			 // cfs2.makePersistent(cfCachePath.c_str());
			 // cfwts2.makePersistent(cfCachePath.c_str(),"WT");
		       }
		  }
	  } // End of loop over baselines

    cfs2.makePersistent(cfCachePath.c_str());
    cfwts2.makePersistent(cfCachePath.c_str(),"","WT");
    // Directory dir(uvGridDiskImage);
    // dir.removeRecursive(false);
    // dir.remove();
  }
  Int AWConvFunc::getOversampling(PSTerm& psTerm, WTerm& wTerm, ATerm& aTerm)
  {
    Int os;
    if (!aTerm.isNoOp()) os=aTerm.getOversampling();
    else if (!wTerm.isNoOp()) os=wTerm.getOversampling();
    else os=psTerm.getOversampling();
    return os;
  }
};
