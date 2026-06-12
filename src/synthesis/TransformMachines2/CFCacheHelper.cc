// -*- C++ -*-
//# CFCacheHeler.cc: Helper functions for making and managing CFCache
//# Copyright (C) 2021
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
//# Correspondence concerning this should be addressed as follows:
//#        Postal address: National Radio Astronomy Observatory
//#                        1003 Lopezville Road,
//#                        Socorro, NM - 87801, USA
//#
//# $Id$

#include <casacore/coordinates/Coordinates/CoordinateSystem.h>
#include <synthesis/TransformMachines2/CFCacheHelper.h>
//
//--------------------------------------------------------------------------
//
using namespace casa;
using namespace casa::refim;
using namespace casacore;
using namespace std;
namespace casa
{
  namespace refim
  {
    namespace SynthesisUtils
    {
      //
      //--------------------------------------------------------------------------
      //
      CountedPtr<refim::PolOuterProduct>
      setPOP(vi::VisBuffer2 &vb2,
	     Vector<casacore::Stokes::StokesTypes> visPolMap,
	     Vector<int> polMap,
	     std::string &stokes, std::string &mType)
      {
	CountedPtr<refim::PolOuterProduct> pop_l = new PolOuterProduct;

	//------------------------a mess----------------------------------------------------
	Vector<int> intpolmap(visPolMap.nelements());
	for (uint kk=0; kk < intpolmap.nelements(); ++kk){
	  intpolmap[kk]=int(visPolMap[kk]);
	}
	pop_l->initCFMaps(intpolmap, polMap);

	PolMapType polMat, polIndexMat, conjPolMat, conjPolIndexMat;
	Vector<int> visPol(vb2.correlationTypes());
	polMat = pop_l->makePolMat(visPol,polMap);
	polIndexMat = pop_l->makePol2CFMat(visPol,polMap);

	conjPolMat = pop_l->makeConjPolMat(visPol,polMap);
	conjPolIndexMat = pop_l->makeConjPol2CFMat(visPol,polMap);

	return pop_l;
      }
      //
      //--------------------------------------------------------------------------
      //
      void fillCFS_inmemory(const string& cfCacheName,
			    CountedPtr<casa::refim::CFStore2> cfs2_l,
			    CountedPtr<casa::refim::CFStore2> cfswt2_l,
			    Vector<double>& uvOffset,
			    const bool& psTerm,
			    const bool& aTerm,
			    const bool& conjBeams)
      {
	//
	// mode="fillcf" case.  The list of CFs in the CFC are
	// expected to be "blank CFs" with all the necessary meta
	// information to fill them.  Nothing else other than the
	// CFC and the list of CFs is necessary.  An already filled
	// CF (the IsFilled=1 entry in CFS*/miscInfo.rec) will be
	// left untouched.
	//
	Vector<double> dummyUVScale;
	Matrix<double> dummyvbFreqSel;

	bool makePersistent=false; // Keep it in-memory
	AWConvFunc::makeConvFunction2(cfCacheName,
				      dummyUVScale, uvOffset,dummyvbFreqSel,
				      *cfs2_l,*cfswt2_l,
				      psTerm,aTerm, conjBeams,
				      makePersistent);

	// Report some stats.
	double memUsed=cfs2_l->memUsage();
	String unit(" KB");
	memUsed = (int)(memUsed/1024.0+0.5);
	if (memUsed > 1024) {memUsed /=1024; unit=" MB";}
      }
      //
      //--------------------------------------------------------------------------
      //
      void makeCFS_inmemory(DataBase& db,
			    //CountedPtr<refim::ConvolutionFunction>& awcf_l
			    CountedPtr<casa::refim::CFStore2> cfs2_l,
			    CountedPtr<casa::refim::CFStore2> cfswt2_l,
			    refim::ConvolutionFunction& awcf_l,
			    const TempImage<Complex>& cgrid,
			    int& nW, float& pa, float& dpa,
			    const Vector<double>& uvScale,
			    const Vector<double>& uvOffset,
			    const string cfCacheName,
			    std::string stokes,
			    std::string mType)
      {
	//-------------------------------------------------------------------------------------------------
	// Instantiate the PolOuterProduce object which encapsulates the
	// polarization maps from VB to Muller (which is CFs) to the
	// image-plane polarizations requested.  Set the necessary maps in
	// the ConvolutionFunction object (awcf_l variable).
	//
	//Replace this with the actual polarization parser (this commented
	//existed earlier and I (SB) don't know what it may mean).
	Vector<int> polMap;
	Vector<casacore::Stokes::StokesTypes> visPolMap;//{0,1,2,3};

	refim::SynthesisUtils::matchPol(*(db.vb_l),cgrid.coordinates(),polMap,visPolMap);
	// Initialize pop to have the right values

	CountedPtr<refim::PolOuterProduct> pop_p = setPOP(*(db.vb_l), visPolMap, polMap, stokes, mType);

	// Vector<int> spwidList, fieldidList;
	// Vector<double> spwRefFreqList;
	// spwidList      = db.spwidList;
	// fieldidList    = db.fieldidList;
	// spwRefFreqList = db.spwRefFreqList;
	Matrix<double> mssFreqSel;
	mssFreqSel  = db.msSelection.getChanFreqList(NULL,true);
	awcf_l.setPolMap(polMap);
	//  awcf_l->setSpwSelection(spwChanSelFlag_p);
	// Replace mssFreqSel with a filtered version of it, filtered
	// for the SPW IDs in the stl::vector db.spwidList.tovector().
	mssFreqSel.assign(filterByFirstColumn(mssFreqSel,db.spwidList));
	awcf_l.setSpwFreqSelection(mssFreqSel);

	// Get the PA from the MS/VB if UI setting is outside the valid
	// range for PA [-180, +180].
	if (abs(pa) > 180.0) pa=getPA(*(db.vb_l));
	constexpr double D2R=M_PI/180.0;
	pa *= D2R; dpa *= D2R;
	//
	// This currently makes both CF and WTCF.  It will also fill the
	// CFs (serially) for fillCF=true.  For fillCF=false (default
	// value here), it will produce an "empty CFC" which has
	// single-pixel CFs with the necessary information in the
	// header/MiscInfo to fill them later. This allows
	// parallelization of the compute intensive step (filling the
	// CFs) which is highly parallelizable and scales well.
	//
	Matrix<double> vbFreqSelection ;
	bool fillCF=false;
	awcf_l.makeConvFunction(cgrid , *(db.vb_l), nW,
				pop_p, pa, dpa, uvScale, uvOffset,
				vbFreqSelection,
				*cfs2_l, *cfswt2_l,
				fillCF);
	//
	// AWConvFunc::makeConvFunction() does not make the memory model
	// (CFStore) persistent. CFSes can be made persistent via
	// CFStore2::makePersistent() in the client code.
	//
      }
      //
      //--------------------------------------------------------------------------
      // This function configures the CFCache and returns the requirested CFSes.
      // Options for whichCFC are MAKE_CFCFS, MAKE_WTCFS and MAKE_BOTHCFS
      //
      std::tuple<CountedPtr<casa::refim::CFStore2>,
		 CountedPtr<casa::refim::CFStore2>>
      //  constructCFS(CountedPtr<refim::CFCache> cfCacheObj,
      constructCFS(refim::CFCache* cfCacheObj,
		   const std::string& cfCacheName,
		   const std::vector<std::string>& cfList,
		   const std::vector<std::string>& wtCFList,
		   const std::string& mode,
		   const double& pa,
		   const double& dpa,
		   const CFCHelperCodes whichCFS)
      {
	//-------------------------------------------------------------------------------------------------
	// Instantiate the CFCache object, initialize it and extract the
	// CFStore objects from it (the CFC in-memory model).
	//
	//      CountedPtr<refim::CFCache> cfCacheObj_l = new refim::CFCache();
	// try
	//   {
	if (cfCacheObj == nullptr)
	  throw(AipsError("CFCacheHelper::constructCFS(): cfCacheObj is a null pointer"));

	cfCacheObj->setCacheDir(cfCacheName.data());

	if (mode == "dryrun")
	  {
	    // In LazyFill model, the CFs are loaded in memory when
	    // accessed (e.g. in the gridder loops).  None of their meta
	    // data is available till then, which is required for
	    // filling.  Hence, setup the CFCacheObj for LAZYFILL only
	    // for the dryRun=True case.  In case there are some CFs in
	    // the CFC already, their lazy loading is OK since all that
	    // is required to know is if the requested CFs already exist
	    // or not.
	    //
	    cfCacheObj->setLazyFill(refim::SynthesisUtils::getenv("CFCache.LAZYFILL",1)==1);
	    try
	      {
		if (whichCFS == MAKE_BOTHCFS)
		  cfCacheObj->initCache2(false, dpa, -1.0,
					 casacore::String("")); // This would load CFs and WTCFs
		else if (whichCFS == MAKE_CFCFS)
		  cfCacheObj->initCache2(false, dpa, -1.0,
					 casacore::String("CFS*")); // This would load CFs
		else if (whichCFS == MAKE_WTCFS)
		  cfCacheObj->initCache2(false, dpa, -1.0,
					      casacore::String("WTCFS*")); // This would load WTCFs
	      }
	    catch (CFCIsEmpty& e)
	      {
		// Ignore the exception.  Empty CFs will be created in
		// the section below after the CFStore objects (which
		// encapsulate the in-memory model of the CFCache) are
		// derived.
		//cerr << "The CFCache (\"" << cfCacheName << "\") is empty.  Building a new one." << LogIO::POST;
	      }
	  }
	else if (mode == "fillcf")
	  {
	    // Do not set CFCacheObj for LAZYFILL for dryRun=False case.
	    // For this case, the CFs listed in the cfList are expected
	    // to be empty and needs filling.  The CF meta data is
	    // therefore required in the memory for configuring the code
	    // for filling the CF.
	    //
	    // This should be made more robust polymorphically in the
	    // CFCache/CFBufer/CFCell tree.  Otherwise logic that is
	    // really internal detals of how these objects work together
	    // leaks all the way to the client layers.
	    int verbose=0;
	    cfCacheObj->setLazyFill(refim::SynthesisUtils::getenv("CFCache.LAZYFILL",1)==1);

	    cfCacheObj->initCacheFromList2(cfCacheName,
					   casacore::Vector<casacore::String>(cfList), //cfNames,
					   casacore::Vector<casacore::String>(wtCFList),
					   pa,dpa,
					   verbose);
	  }
	else
	  {
	    throw(AipsError("CFCacheHelper::constructCFS(): Don't know what to do with mode="+mode+"!"));
	  }
	//  } //try
	// catch (CFSupportZero &e)
	// 	{
	// 	  // Ignore the "CFS is empty" exception.  This exception
	// 	  // should not reach here since it is resolved in AWCF.  But
	// 	  // leaving the code here in case there is a bug in the
	// 	  // resolution code.
	// 	  cerr << e.what() << endl;
	// 	}
	casacore::CountedPtr<casa::refim::CFStore2> cfs2_l, cfswt2_l;

	if ((whichCFS == MAKE_CFCFS) || (whichCFS == MAKE_BOTHCFS))
	  {
	    //cfs2_l = casacore::CountedPtr<CFStore2>(&(cfCacheObj->memCache2_p)[0],false);
	    cfs2_l = cfCacheObj->getCFS();
	    if (cfs2_l == nullptr)
	      throw(AipsError("CFCacheHelper::constructCFS(): CFStore pointer for CFs is a null pointer"));
	  }
	if ((whichCFS == MAKE_WTCFS) || (whichCFS == MAKE_BOTHCFS))
	  {
	    cfswt2_l =  cfCacheObj->getWTCFS();
	    if (cfswt2_l == nullptr)
	      throw(AipsError("CFCacheHelper::constructCFS(): CFStore pointer for WTCFs is a null pointer"));
	  }

	return std::make_tuple(cfs2_l, cfswt2_l);
      }

    }; // End SynthesisUtils namespace
  }; // End refim namespace
}; // End casa namespace
