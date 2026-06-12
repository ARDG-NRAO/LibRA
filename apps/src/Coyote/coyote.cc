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
#include <synthesis/TransformMachines2/CFCacheHelper.h>
#include <coyote.h>
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
	  log_l << "Found " << cfList.size() << " elements to fill." << LogIO::POST;
	  for(auto x : cfList) wtCFList.push_back("WT"+x);
	}

      //-------------------------------------------------------------------------------------------------
      // Instantiate the CFCache object, initialize it and extract the
      // CFStore objects from it (the CFC in-memory model).
      //
      CountedPtr<refim::CFCache> cfCacheObj_l = new refim::CFCache();
      CountedPtr<casa::refim::CFStore2> cfs2_l=nullptr, cfswt2_l=nullptr;
      try
	{
	  std::tie(cfs2_l, cfswt2_l) =
	    casa::refim::SynthesisUtils::constructCFS(cfCacheObj_l.get(), cfCacheName,
				    cfList, wtCFList, mode, pa, dpa);
	}
      catch (CFSupportZero &e)
      	{
      	  // Ignore the "CFS is empty" exception.  This exception
      	  // should not reach here since it is resolved in AWCF.  But
      	  // leaving the code here in case there is a bug in the
      	  // resolution code.
      	  log_l << e.what() << LogIO::POST;
      	}
      //
      //-------------------------------------------------------------------------------------------------
      //
      Vector<int> imSize(2,NX);
      Vector<double> uvOffset={(double)imSize[0],(double)imSize[1],0.0};

      if (mode=="dryrun")
	{
	  // Setup and apply makeConvFunction() for mode=dryrun and
	  // makeConvFunction2() for mode=fillcf on the CF store memory models
	  // (cfs2_l and cfswt2_l)
	  string uvDistStr;
	  bool doSPWDataIter = false;
	  const Vector<double> uvScale(3,0);
	  //
	  // mode="dryrun" case.  This makes "blank CFs" in the CFC from
	  // scratch.  So needs meta data from the sky image as well
	  // as the MS.
	  //
	  // Instantiate the DataBase object
	  //
	  DataBase db(MSNBuf, fieldStr, spwStr, uvDistStr, WBAwp, nW, doSPWDataIter);

	  //-------------------------------------------------------------------------------------------------
	  // Create a complex image from the user-defined image.
	  // To be turned into the most basic parameters needed to make CF
	  TempImage<Complex> cgrid =
	    makeEmptySkyImage(*(db.vi2_l), db.selectedMS, db.msSelection, imSize,
			      cellSize, phaseCenter, stokes, refFreqStr,String("mfs"));
	  //-------------------------------------------------------------------------------------------------
	  // Instantiate the AWCF object for making the CFs.
	  //
	  bool wTerm = (nW > 1);
	  CountedPtr<refim::ConvolutionFunction> awcf_l =
	    AWProjectFT::makeCFObject(telescopeName, aTerm, psTerm, wTerm, true,
				      WBAwp, conjBeams, cfBufferSize,cfOversampling);
	  //-------------------------------------------------------------------------------------------------
	  // Extract complex grid info from cgrid (csys, imsize,
	  // etc.), save in a record and make it persistent in the
	  // CFCache.
	  //
	  ImageInformation<Complex> imInfo(cgrid,casacore::String(cfCacheName));
	  imInfo.save();

	  //-------------------------------------------------------------------------------------------------
	  casa::refim::SynthesisUtils::makeCFS_inmemory(db, cfs2_l, cfswt2_l, *awcf_l,
				      cgrid, nW, pa, dpa, uvScale,
				      uvOffset, cfCacheName);

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
	  casa::refim::SynthesisUtils::fillCFS_inmemory(cfCacheName,
				      cfs2_l, cfswt2_l, uvOffset,
				      psTerm, aTerm, conjBeams);
	}
      //
      // Save the contents of the in-memory CFStore on the disk.
      // Until this point, CFSes, in both settings of the "mode"
      // keyword, are in-memory only.
      //
      // [07Jan2024] In the dryrun mode, only the meta info is
      // written (as casacore::Records converted to
      // casacore::Tables).  Writing these with multi-threading
      // seems to work.  The bool parameter below is therefore set
      // to true (it is false in the default interface).
      //
      if (!cfCacheName.empty())
	{
	  cfs2_l->makePersistent(cfCacheName.c_str(),"","", true);
	  cfswt2_l->makePersistent(cfCacheName.c_str(),"","WT",true);
	}
      //
      //------------------------------------------------------------------------------------------------------
      //
      // Report some stats.
      double memUsed=cfs2_l->memUsage();
      String unit(" KB");  memUsed = (int)(memUsed/1024.0+0.5);
      if (memUsed > 1024) {memUsed /=1024; unit=" MB";}
      log_l << "Convolution function memory footprint:"
	    << (int)(memUsed) << unit << " out of a maximum of "
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

