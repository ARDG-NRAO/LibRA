// -*- C++ -*-
// # Coyote.cc: Definition of the Coyote functions
// # Copyright (C) 2021
// # Associated Universities, Inc. Washington DC, USA.
// #
// # This library is free software; you can redistribute it and/or modify it
// # under the terms of the GNU Library General Public License as published by
// # the Free Software Foundation; either version 2 of the License, or (at your
// # option) any later version.
// #
// # This library is distributed in the hope that it will be useful, but WITHOUT
// # ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// # FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
// # License for more details.is
// #
// # You should have received a copy of the GNU Library General Public License
// # along with this library; if not, write to the Free Software Foundation,
// # Inc., 675 Massachusetts Ave, Cambridge, MA 02139, USA.
// #
// # Correspondence concerning this should be addressed as follows:
// #        Postal address: National Radio Astronomy Observatory
// #                        1003 Lopezville Road,
// #                        Socorro, NM - 87801, USA
// #
// # $Id$


#include <libracore/rWeightor.h>
#include <libracore/DataBase.h>
#include <libracore/MakeComponents.h>

#include <coyote.h>
//
//--------------------------------------------------------------------------
//
CountedPtr<refim::PolOuterProduct> setPOP(vi::VisBuffer2 &vb2,
					  Vector<casacore::Stokes::StokesTypes> visPolMap,
					  Vector<int> polMap,
					  std::string &stokes, std::string &mType)
{
  CountedPtr<refim::PolOuterProduct> pop_l = new PolOuterProduct;
  
  //------------------------a mess----------------------------------------------------
  Vector<Int> intpolmap(visPolMap.nelements());
  for (uInt kk=0; kk < intpolmap.nelements(); ++kk){
    intpolmap[kk]=Int(visPolMap[kk]);
  }
  pop_l->initCFMaps(intpolmap, polMap);
  
  PolMapType polMat, polIndexMat, conjPolMat, conjPolIndexMat;
  Vector<Int> visPol(vb2.correlationTypes());
  polMat = pop_l->makePolMat(visPol,polMap);
  polIndexMat = pop_l->makePol2CFMat(visPol,polMap);
  
  conjPolMat = pop_l->makeConjPolMat(visPol,polMap);
  conjPolIndexMat = pop_l->makeConjPol2CFMat(visPol,polMap);
  
  return pop_l;
}
//
//--------------------------------------------------------------------------
//
std::vector<std::string> fileList(const std::string& cfCacheName,
				  const std::vector<std::string>& regexList)
{
  // Just easier to work with STL...
  std::vector<std::string> selectedCF;
  try
    {
      Directory dirObj(cfCacheName);

      casacore::String combinedRegex="("+Regex::fromPattern(regexList[0])+")";
      for(int i=1;i<regexList.size();i++)
	if (regexList[i].size() > 0) combinedRegex += "|("+Regex::fromPattern(regexList[i])+")";
      {
	//Regex regex(Regex::fromPattern(combinedRegex));
	Regex regex(combinedRegex);
	Vector<String> tmp = dirObj.find(regex);
	for (auto y : tmp) selectedCF.push_back(y);
      }
      if (selectedCF.size() == 0)
	throw(SynthesisFTMachineError(String("fileList: CF selection leads to a NULL set!")));
      // Guard against user error that includes WTCF* in the supplied list of CFs to fill.
      // The list of WTCFs is constructed internally to match the list of CFs.
      {
	Regex regex(Regex::fromPattern("WTCF*"));
	for(auto x:selectedCF)
	  {
	    if (regex.fullMatch(x.c_str(),x.size()))
	      throw(SynthesisFTMachineError(String("fileList: The list of CFs to fill contains WTCFs. Please supply list of only CFs")));
	  }
      }
    }
  catch(AipsError& x)
    {
      throw(SynthesisFTMachineError(String("fileList: Error in making a list of CFs to fill")
				    +x.getMesg()));
    }
  return selectedCF;
}
//
//--------------------------------------------------------------------------
//
void Coyote(//bool &restartUI, int &argc, char **argv,
	    string &MSNBuf, 
	    string &telescopeName,
	    int &NX, float &cellSize,
	    string &stokes, string &refFreqStr, int &nW,
	    string &cfCacheName,
	    bool &WBAwp, bool& aTerm, bool &psTerm, string &mType,
	    float& pa, float& dpa,
	    string &fieldStr, string &spwStr, string &phaseCenter,
	    bool &conjBeams,  
	    int &cfBufferSize, int &cfOversampling,
	    std::vector<std::string>& cfList,
	    string& mode)
{
  LogFilter filter(LogMessage::NORMAL);
  LogSink::globalSink().filter(filter);
  LogIO log_l(LogOrigin("coyote", "Coyote"));
  
  try
    {
      std::vector<std::string> wtCFList;
      if (mode=="fillcf")
	{
	  cfList = fileList(cfCacheName,cfList);

	  cerr << "Found " << cfList.size() << " elements to fill." << endl;

	  for(auto x : cfList) wtCFList.push_back("WT"+x);
	}
      
      // Make a name for the temp image that will be unique for multiple
      // instances on different computers but writing to the same
      // directory.
      char hostname[HOST_NAME_MAX];
      gethostname(hostname, HOST_NAME_MAX);
      string imageName=cfCacheName + "/uvgrid.im_" +
	to_string(getppid()) + "_"+string(hostname);
      bool wTerm = (nW > 1);

      //-------------------------------------------------------------------------------------------------
      // Instantiate AWCF object for making the CFs later.
      //
      CountedPtr<refim::ConvolutionFunction> awcf_l =
	AWProjectFT::makeCFObject(telescopeName,
				  aTerm, psTerm, wTerm, true,
				  WBAwp, conjBeams,
				  cfBufferSize,cfOversampling);
      //-------------------------------------------------------------------------------------------------

      //-------------------------------------------------------------------------------------------------
      // Instantiate the CFCache object, initialize it and extract the
      // CFStore objects from it (the CFC in-memory model).
      //
      CountedPtr<refim::CFCache> cfCacheObj_l = new refim::CFCache();
      try
	{
	  cfCacheObj_l->setCacheDir(cfCacheName.data());

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
	      cfCacheObj_l->setLazyFill(refim::SynthesisUtils::getenv("CFCache.LAZYFILL",1)==1);
	      try
		{
		  cfCacheObj_l->initCache2(false, dpa, -1.0,
					   casacore::String("CFS*")); // This would load CFs
		  cfCacheObj_l->initCache2(false, dpa, -1.0,
					   casacore::String("WTCFS*")); // This would load WTCFs
		}
	      catch (CFCIsEmpty& e)
		{
		  // Ignore the exception.  Empty CFs will be created in
		  // the section below after the CFStore objects (which
		  // encapsulate the in-memory model of the CFCache) are
		  // derived.
		  log_l << "The CFCache (\"" << cfCacheName << "\") is empty.  Building a new one." << LogIO::POST;
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
	      cfCacheObj_l->setLazyFill(refim::SynthesisUtils::getenv("CFCache.LAZYFILL",1)==1);
	      casacore::Vector<casacore::String> cfNames(cfList);
	      casacore::Vector<casacore::String> wtCFNames(wtCFList);

	      cfCacheObj_l->initCacheFromList2(cfCacheName,
					       casacore::Vector<casacore::String>(cfList), //cfNames,
					       casacore::Vector<casacore::String>(wtCFNames),
					       pa,dpa,
					       verbose);
	    }
	  else
	    {
	      throw(AipsError("Coyote: Don't know what to do with mode="+mode+"!"));
	    }
	}
      catch (CFSupportZero &e)
	{
	  // Ignore the "CFS is empty" exception.  This exception should
	  // reach here since it is resolved in AWCF.  But leaving the
	  // code here in case there is a bug in the resolution code.
	  cerr << e.what() << endl;
	}
      CountedPtr<casa::refim::CFStore2> cfs2_l, cfswt2_l;

      if (!cfCacheObj_l.null())
	{
	  cfs2_l = CountedPtr<CFStore2>(&(cfCacheObj_l->memCache2_p)[0],false);//new CFStore2;
	  cfswt2_l =  CountedPtr<CFStore2>(&cfCacheObj_l->memCacheWt2_p[0],false);//new CFStore2;
	}
      // cfs2_l->show("Coyote: CFS::show(CF): ",cerr,false);
      // cfswt2_l->show("Coyote: CFS::show(WTCF): ",cerr,false);
      //-------------------------------------------------------------------------------------------------

      //-------------------------------------------------------------------------------------------------
      // Setup and apply makeConvFunction() for mode=dryrun and
      // makeConvFunction2() for mode=fillcf on the CF store memory models
      // (cfs2_l and cfswt2_l)
      string uvDistStr;
      bool doSPWDataIter = false;
      Vector<Int> imSize(2,NX);
      const Vector<Double> uvScale(3,0);
      Bool fillCF = (mode == "fillcf");
      Vector<Double> uvOffset;

      uvOffset.resize(3);
      uvOffset(0)=imSize[0];
      uvOffset(1)=imSize[1];
      uvOffset(2)=0;

      if (mode=="dryrun")
	{
	  //
	  // mode="dryrun" case.  This makes "blank CFs" in the CFC from
	  // scratch.  So needs meta data from the sky image as well
	  // as the MS.
	  //
	  //
	  // Instantite the DataBase object, make a complex grid using meta
	  // from the DataBase and the sky-image provided.
	  //
	  DataBase db(MSNBuf, fieldStr, spwStr, uvDistStr, WBAwp, nW,
		      doSPWDataIter);
	  
	  //
	  // Creating a complex image from the user inputed image.
	  // To be turned into the most basic parameters needed to make CF
	  // To read an image on disk you need PagedImage. 
	  // This is inherited from ImageInterface so should be able to pass
	  // an image interface object from it.
	  //
	  // PagedImage<Complex> cgrid =
	  //   makeEmptySkyImage4CF(*(db.vi2_l), db.selectedMS, db.msSelection, 
	  // 			 imageName, imSize, cellSize, phaseCenter, 
	  // 			 stokes, refFreqStr);
	  TempImage<Complex> cgrid =
	    makeEmptySkyImage(*(db.vi2_l), db.selectedMS, db.msSelection, 
			      imageName, String(""),imSize, cellSize, phaseCenter, 
			      stokes, refFreqStr,String("mfs"));
	  //
	  // Save the coordinate system in a record and make it persistent in
	  // the CFCache.
	  //
	  //	  String targetName=String(cfCacheName);
	  ImageInformation<Complex> imInfo(cgrid,casacore::String(cfCacheName));
	  imInfo.save();
	  //	  cgrid.table().markForDelete();
	  
	  //-------------------------------------------------------------------------------------------------
	  
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
	  Matrix<Double> mssFreqSel;
	  mssFreqSel  = db.msSelection.getChanFreqList(NULL,true);
	  awcf_l->setPolMap(polMap);
	  //  awcf_l->setSpwSelection(spwChanSelFlag_p);
	  // Replace mssFreqSel with a filtered version of it, filtered
	  // for the SPW IDs in the stl::vector db.spwidList.tovector().
	  mssFreqSel.assign(filterByFirstColumn(mssFreqSel,db.spwidList.tovector()));
	  awcf_l->setSpwFreqSelection(mssFreqSel);

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
	  Matrix<Double> vbFreqSelection ;
	  awcf_l->makeConvFunction(cgrid , *(db.vb_l), nW,
				   pop_p, pa, dpa, uvScale, uvOffset,
				   vbFreqSelection,
				   *cfs2_l, *cfswt2_l,
				   fillCF);
	  //
	  // AWConvFunc::makeConvFunction() does not make the memory
	  // model (CFStore) persistent.  So save the contents of the
	  // CFStore on the disk.
	  //
	  // [07Jan2024] In the dryrun mode, only the meta info is
	  // written as casacore::Records converted to
	  // casacore::Tables.  Writing these with multi-threadings
	  // seems to be work.  The bool parameter is therefore set to
	  // true (it is false in the default interface).
	  // cfs2_l->makePersistent(cfCacheObj_l->getCacheDir().c_str(),"","", Quantity(pa,"rad"),Quantity(dpa,"rad"),0,0,true);
	  // cfswt2_l->makePersistent(cfCacheObj_l->getCacheDir().c_str(),"","WT",Quantity(pa,"rad"),Quantity(dpa,"rad"),0,0,true);
	  cfs2_l->makePersistent(cfCacheObj_l->getCacheDir().c_str(),"","", true);
	  cfswt2_l->makePersistent(cfCacheObj_l->getCacheDir().c_str(),"","WT",true);
	}
      else
	{
	  //
	  // mode="fillcf" case.  The list of CFs in the CFC are
	  // expected to be "blank CFs" with all the necessary meta
	  // information to fill them.  Nothing else other than the
	  // CFC and the list of CFs is necessary.  An already filled
	  // CF (the IsFilled=1 entry in CFS*/miscInfo.rec) will be
	  // left untouched.
	  //
	  Vector<Double> dummyUVScale;
	  Matrix<Double> dummyvbFreqSel;
	  AWConvFunc::makeConvFunction2(cfCacheName,
					dummyUVScale, uvOffset,	dummyvbFreqSel,
					*cfs2_l,*cfswt2_l,
					psTerm,	aTerm, conjBeams);
	}

      // Report some stats.
      Double memUsed=cfs2_l->memUsage();
      String unit(" KB");
      memUsed = (Int)(memUsed/1024.0+0.5);
      if (memUsed > 1024) {memUsed /=1024; unit=" MB";}

      LogIO log_l(LogOrigin("AWProjectFT2", "findConvFunction[R&D]"));
      log_l << "Convolution function memory footprint:" 
	    << (Int)(memUsed) << unit << " out of a maximum of "
	    << HostInfo::memoryTotal(true)/1024 << " MB" << LogIO::POST;
    }
  catch(AipsError &e)
    {
      log_l << e.what() << endl;
    }
}
//
//--------------------------------------------------------------------------
//

