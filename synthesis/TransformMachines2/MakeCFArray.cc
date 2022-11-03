// -*- C++ -*-
//# MakeCFArray.cc: Implementation of the MakeCFArray class
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
#include <vector> // std::vector
#include <tuple>
#include <casacore/casa/Arrays/Vector.h> // casa::Vector
#include <msvis/MSVis/VisBuffer2.h>
#include <msvis/MSVis/VisibilityIterator2.h>
#include <synthesis/TransformMachines2/CFStore2.h>
#include <synthesis/TransformMachines2/CFBuffer.h>
#include <synthesis/TransformMachines2/CFDefs.h>
#include <synthesis/TransformMachines2/MakeCFArray.h>
#include <synthesis/TransformMachines2/MakeCFArray.h>
#include <synthesis/TransformMachines2/Utils.h>
#include <casacore/coordinates/Coordinates/SpectralCoordinate.h>

using namespace casacore;
using namespace hpg;

namespace casa{
  namespace refim{
    //
    //--------------------------------------------------------------------------------------------
    //
    Complex* getConvFuncArray(const double& vbPA, Vector<int>& cfShape,
			      Vector<int>& support,
			      int& muellerElement,
			      casa::refim::CFBuffer& cfb,
			      double& wVal, int& fndx, int& wndx,
			      const casa::refim::PolMapType& mNdx,
			      const casa::refim::PolMapType& conjMNdx,
			      const int& ipol, const uint& mCol,
			      const double& paTolerance)
    {
      Bool Dummy;
      Array<Complex> *convFuncV;
      CFCell *cfcell;
      //
      // Since we conjugate the CF depending on the sign of the w-value,
      // pick the appropriate element of the Mueller Matrix (see note on
      // this for details). Without reading the note/understanding,
      // fiddle with this logic at your own risk (can easily lead to a
      // lot of grief. --Sanjay).
      //
      //timer_p.mark();
      
      int pndx;
      
      if (wVal > 0.0) pndx=mNdx[ipol][mCol];
      else            pndx=conjMNdx[ipol][mCol];
      
      //pndx=1;
      //cerr << "CFC indexes(w,f,p),ipol: " << wndx << " " << fndx << " " << pndx << " " << ipol << endl;
      cfcell=&(*(cfb.getCFCellPtr(fndx,wndx,pndx)));
      
      convFuncV = &(*cfcell->getStorage());
      support(0)=support(1)=cfcell->xSupport_p;
      
      // Get the pointer to the CFCell storage (a single CF)
      //    if ((convFuncV = &(*cfcell->getStorage())) == NULL)
      if (convFuncV == NULL)
	throw(SynthesisFTMachineError("cfcell->getStorage() == null"));
      
      // Load the CF if it not already loaded.  If a new CF is loaded,
      // check if it needs to be rotated.
      if (convFuncV->shape().product() == 0)
	{
	  Array<Complex>  tt=SynthesisUtils::getCFPixels(cfb.getCFCacheDir(), cfcell->fileName_p);
	  cfcell->setStorage(tt);
	  
	  //cerr << (cfcell->isRotationallySymmetric_p?"o":"+");
	  
	  // No rotation necessary if the CF is rotationally symmetric
	  // SB: Disabled for HPG testing.
	  //
	  // if (!(cfcell->isRotationallySymmetric_p))
	  //   {
	  //     CFCell *baseCFC=NULL;
	  //     // Rotate only if the difference between CF PA and VB PA
	  //     // is greater than paTolerance.
	  //     SynthesisUtils::rotate2(vbPA, *baseCFC, *cfcell, paTolerance);
	  //   }
	  convFuncV = &(*cfcell->getStorage());
	}
      
      cfShape.assign(convFuncV->shape().asVector());
      
      // Always extract the Mueller element value from mNdx.  mNdx
      // carries the direct mapping between Mueller Matrix and
      // Visibility vector.
      //     muellerElement=cfb->getCFCellPtr(fndx,wndx,mNdx[ipol][mRow])->muellerElement_p;
      muellerElement=cfcell->muellerElement_p;

      return convFuncV->getStorage(Dummy);
    };
    //
    //-------------------------------------------------------------------------------------
    // Global function slated to be out of the class
    //
    double makeAWLists_p(casa::refim::CFBuffer& cfb,
			 const bool& wbAWP,
			 const uint& user_wprojplanes,
			 const ImageInterface<Float>& skyImage,
			 const double& spwRefFreq,
			 const int vbSPW,
			 Vector<Int>& wNdxList, Vector<Int>& spwNdxList)
    {
      CoordinateSystem cs = skyImage.coordinates();
      Int index= cs.findCoordinate(Coordinate::SPECTRAL);
      SpectralCoordinate spCS = cs.spectralCoordinate(index);
      double imRefFreq = spCS.referenceValue()(0);

      // Get the coordiantes along the Frequency- and W-axis of the
      // CFB.  These axis, in general, are irregularly sampled (hence
      // the list of coordinates, rather than X_0, dX and N_x kind of
      // coordinate system description).
      Vector<Double> wVals, fVals;
      fVals = cfb.getFreqList();
      wVals = cfb.getWList();
      SynthesisUtils::makeAWLists(wVals, fVals, wbAWP, user_wprojplanes, imRefFreq, spwRefFreq,
				  vbSPW, wNdxList, spwNdxList);
      return imRefFreq;
    }
    //
    //--------------------------------------------------------------------------------------------
    //
    std::tuple<bool,                   // If new CFs were loaded (reloadCFs)
	       int,                    // The SPW ID for which the CFs were loaded
	       hpg::CFSimpleIndexer,   // The corresponding CFSI.  This should be used in AWVRHPG to fill hpg::VisData
	       std::shared_ptr<hpg::RWDeviceCFArray>> // The RWDeviceCFArray.  Use it if reloadCFs==true
    MakeCFArray::makeRWDArray(const bool& wbAWP, const int& user_wprojplanes,
			     const ImageInterface<Float>& skyImage,
			     const Vector<int>& polMap,
			     CountedPtr<casa::refim::CFStore2> cfs2_l,
			     const int& vbSpw_l, const double& spwRefFreq,
			     const int& nDataPol,
			     Vector<Int>& wNdxList, Vector<Int>& spwNdxList)
    {
      bool reloadCFs = SynthesisUtils::needNewCF(cachedVBSpw_p, vbSpw_l, user_wprojplanes, wbAWP,initialized_p==false);
      hpg::CFSimpleIndexer cfsi({1,false},{1,false},{1,true},{1,true}, 1);
      std::shared_ptr<hpg::RWDeviceCFArray> rwdcfa_sptr;

      if (reloadCFs)
	{
	  initialized_p=true;
	  cachedVBSpw_p = vbSpw_l;

	  //cerr << "SPW ID: " << vbSpw_l << endl;
	  //	  cerr << "FIELD ID: " << vb_l->fieldId()[0] << endl;

	  Vector<Int> ant1List, ant2List;
	  Vector<Quantity> paList;
	  ant1List = cfs2_l->getAnt1List();
	  ant2List = cfs2_l->getAnt2List();
	  paList   = cfs2_l->getPAList();
	  Quantity dPA(360.0,"deg");
	  int a1=0, a2=0;

	  casa::refim::CFBuffer& cfb_l = *cfs2_l->getCFBuffer(paList[0], dPA, ant1List(a1), ant2List(a2));

	  //double spwRefFreq_l =spwRefFreq;//vb_l->subtableColumns().spectralWindow().refFrequency()(vbSpw_l);

	  double imRefFreq_l = makeAWLists_p(cfb_l, wbAWP,  user_wprojplanes,
					     skyImage, spwRefFreq,
					     vbSpw_l,
					     wNdxList, spwNdxList);

	  double vbPA=360.0;
	  int nGridPol = skyImage.shape()[2];
	  //	  int nDataPol  = vb_l->flagCube().shape()[0];
	  //std::tuple<hpg::CFSimpleIndexer, std::shared_ptr<hpg::RWDeviceCFArray>>
	  casa::refim::MyCFArrayShape cfArrayShape;
	  auto ret = makeRWDCFA_p(cfArrayShape,
				  vbPA, imRefFreq_l,
				  cfb_l,
				  nGridPol, nDataPol,
				  polMap,
				  wNdxList,
				  spwNdxList);

	  cfsi = std::get<0>(ret);
	  rwdcfa_sptr = std::move(std::get<1>(ret));
	}
      return std::make_tuple(reloadCFs, cachedVBSpw_p, cfsi, rwdcfa_sptr);
    }
    //
    //--------------------------------------------------------------------------------------------
    //
    std::tuple<hpg::CFSimpleIndexer, std::shared_ptr<hpg::RWDeviceCFArray>>
    MakeCFArray::makeRWDCFA_p(casa::refim::MyCFArrayShape& cfArrayShape,
			      const double& vbPA, const double& imRefFreq,
			      casa::refim::CFBuffer& cfb,
			      //VBStore& vbs,
			      const Int& nGridPol, const Int& nDataPol,
			      const Vector<Int>& polMap,
			      Vector<Int>& wNdxList,
			      Vector<Int>& spwNdxList)
    {
      LogIO log_l(LogOrigin("MakeCFArray","makeRWDCFA_p"));
      
      
      Int nWCF=1, nFreqCF, nPolCF;
      Vector<float> sampling(2);
      Vector<Int> support(2), cfShape;
      Vector<Double> wVals, fVals; PolMapType mVals, mNdx, conjMVals, conjMNdx;
      
      Double cfRefFreq,dataWVal=0,fIncr, wIncr;
      
      cfb.getCoordList(fVals,wVals,mNdx, mVals, conjMNdx, conjMVals, fIncr, wIncr);
      
      nWCF = wNdxList.nelements();
      nFreqCF = spwNdxList.nelements();
      
      // Get the oversampling parameter.
      {
    	Float s;
    	int dummyWNdx=0;
	
    	int refSpw;
    	SynthesisUtils::stdNearestValue(fVals.tovector(),
    					imRefFreq,//vbs.imRefFreq_p,
    					refSpw);
    	// cerr << "RefSpw, W, F, RefFreq: "
    	//      << refSpw << " " << wVals.nelements() << " " << fVals.nelements() << " " << imRefFreq
    	//      << endl;
	
    	cfb.getParams(cfRefFreq, s, support(0), support(1),refSpw,dummyWNdx,0);
    	sampling(0) = sampling(1) = SynthesisUtils::nint(s);
      }
      
      nPolCF=0;
      std::map<int,int> p2m;
      unsigned nCF;
      if (cfArrayShape.oversampling()==0)
    	{
    	  //unsigned nc=mNdx[0].shape()(0);
    	  uint nr=mNdx.shape()(0);
    	  for (uint r=0;r<nr;r++)
    	    {
    	      // MP: the use of "p2m" fixes the issue mentioned below
    	      for (uint c=0;c<mNdx[r].shape()[0];c++) {
    		auto p = mNdx[r][c];
    		if (p >= 0 && p2m.count(p) == 0) {
    		  auto m = p2m.size();
    		  p2m[p] = m;
    		}
    	      }
    	      // This assumes that all entries are unique elements of
    	      // the Mueller matrix.  In effect, this ignores symmetries
    	      // in the Mueller matrix elements, which can (should?) be
    	      // exploited to reduce memory footprint.
    	      // SB: nPolCF += conjMNdx[r].shape()(0); 
	      //    	      cerr  << mNdx[r] << endl;
    	    }
    	  nPolCF = p2m.size();
    	  log_l << "CFBuffer shape (nW x nF x nPol): " << nWCF << " x " << nFreqCF << " x " << nPolCF << LogIO::POST;
	  
    	  nCF = nWCF * nFreqCF;
    	  log_l << "Setting cfArray size: " << "nCF: " << nCF << " x " << nPolCF << " sampling: " << sampling(0) << LogIO::POST;
	  
    	  cfArrayShape.setSize((unsigned)(nCF),(unsigned)sampling(0));
    	}
      

      // Reference for call signature
      // CFSimpleIndexer(
      //   const axis_desc_t& baseline_class,
      //   const axis_desc_t& time,
      //   const axis_desc_t& w_plane,
      //   const axis_desc_t& frequency,
      //   unsigned mueller)
      //                               nBLType, nTime/PA, nW, nFreq, nPol
      hpg::CFSimpleIndexer cfsi({1,false},{1,false},{nWCF,true},{nFreqCF,true}, nPolCF);
      unsigned iGrp=0; // HPG Group index

      // Determine the size of the Mueller (poln) axis for CFArray.
      // In the code below, it is determined as the max. zero-based
      // index in the mndx_p matrix, plus 1.
      int nMuellerElements = -1;
      for (auto r : mndx_p) nMuellerElements = max(nMuellerElements,max(r));
      nMuellerElements++; // mndx_p has 0-based indices
      log_l << "No. of Mueller elements: " << nMuellerElements << LogIO::POST;
      
      //
      // Loops below just get the cfShapes and use them to set up cfArray
      //-------------------------------------------------------------------------------------------------
      for(int iFreq=0; iFreq < nFreqCF; iFreq++) // CASA CF Freq-index
    	{
    	  for(int iW=0; iW < nWCF; iW++)   // CASA CF W-index
    	    {
    	      for(int ipol=0; ipol < nDataPol; ipol++)  // CASA CF Pol-index
    		{  
    		  Int targetIMPol=polMap(ipol);
    		  if ((targetIMPol>=0) && (targetIMPol<nGridPol)) 
    		    {
    		      Vector<int> mRow = mndx_p[targetIMPol];
    		      for (uInt mCols=0;mCols<mRow.nelements(); mCols++) 
    			{
    			  if (mRow[mCols] < 0) break;
    			  {
    			    int fNdx=spwNdxList[iFreq],wNdx=wNdxList[iW],pndx;
			    // if (wVal > 0.0) pndx=mNdx[ipol][mCols];
			    // else            pndx=conjMNdx[ipol][mCols];
			    pndx=1;
			    IPosition cfShp=(cfb.getCFCellPtr(fNdx,wNdx,pndx))->shape_p;

    			    // hpg::CFCellIndex cfCellNdx(0,0,wNdx,fNdx,(mndx_p[targetIMPol][mCols]));
    			    // std::array<unsigned, 3> tt=cfsi.cf_index(cfCellNdx);
    			    cfArrayShape.resize(iGrp,cfShp(0), cfShp(1),nMuellerElements,1);//This only sets extents, but does not allocate memory
    			  }
    			}
    		    }
    		} // Pol loop
    	      iGrp++;
    	    } // W loop
    	  if (nWCF > 1) iGrp=0;
    	} // Freq loop
      //-------------------------------------------------------------------------------------------------
      auto err_or_val = hpg::RWDeviceCFArray::create(hpg::Device::Cuda, cfArrayShape);
      if (!hpg::is_value(err_or_val))
	throw(AipsError("Error while creating hpg::RWDeviceCFArray in MakeCFArray::makeRWDCFA_p()"));
      std::shared_ptr<hpg::RWDeviceCFArray> rwDCFArray = std::move(hpg::get_value(err_or_val));
      //-------------------------------------------------------------------------------------------------

      double paTolerance = 360.0;
      iGrp=0; // HPG Group index
      for(int iFreq=0; iFreq < nFreqCF; iFreq++) // CASA CF Freq-index
    	{
    	  for(int iW=0; iW < nWCF; iW++)   // CASA CF W-index
    	    {
    	      for(int ipol=0; ipol < nDataPol; ipol++)  // CASA CF Pol-index
    		{  
    		  Int targetIMPol=polMap(ipol);

    		  if ((targetIMPol >= 0) && (targetIMPol < nGridPol))
    		    {
    		      Vector<int> mRow = mndx_p[targetIMPol];

    		      // ipol determines the targetIMPol.  Each targetIMPol gets a row of CFs (mRow).
    		      // visVecElements is gridded using the convFuncV and added to the target grid.

    		      for (uInt mCols=0;mCols<mRow.nelements(); mCols++) 
    			{
    			  if (mRow[mCols] >= 0)
			    {
			      //int visVecElement=mCols;
			      int muellerElement;
			      Complex* convFuncV=NULL;

			      int fNdx=spwNdxList[iFreq];
			      int wNdx=wNdxList[iW];
			    
			      convFuncV=getConvFuncArray(vbPA,cfShape, support,muellerElement,
							 cfb, dataWVal,
							 fNdx, wNdx,
							 mndx_p, conj_mndx_p,
							 targetIMPol,//ipol,
							 mCols,
							 paTolerance);
			      //
			      // Ref. for meaning of the indices in CFCellInex: (BL, PA, W, Freq, Pol)
			      //hpg::CFCellIndex cfCellNdx(0,0,wNdx,fNdx,(mndx_p[targetIMPol][mCols]));
			      hpg::CFCellIndex cfCellNdx(0,0,wNdx,fNdx,mRow[mCols]);

			      //			hpg::CFCellIndex cfCellNdx(0,0,dummyNdx,iCF,(mNdx[ipol][mCols])); 
			      // SB: Check if p2m.at(mNdx[ipol][mCols] should go to mNdx[ipol][mCols]
			      // tt[0] ==> pol
			      // tt[1] ==> cube : should always be 0
			      // tt[2] ==> group : linearized index from (BLType, PA, W, SPW)
			      std::array<unsigned, 3> tt=cfsi.cf_index(cfCellNdx);
			    
			      uint k=0;
			      for(auto x=0; x<cfShape(0); x++)
				for(auto y=0; y<cfShape(1); y++)
				  (*rwDCFArray)(x, y, tt[0]/*Mueller*/, tt[1]/*Cube*/, iGrp)=convFuncV[k++];

			      //			      if (ipol==0) cerr << support[0] << " ";
			    }
    			}
    		    }
    		} // Pol loop
    	      iGrp++;
    	    } // W loop
    	  if (nWCF > 1) iGrp=0;
    	} // Freq loop
      //      cerr << endl;
      return std::make_tuple(cfsi,rwDCFArray);
    }
  } // NAMESPACE refin - END
} // NAMESPACE casa - END
