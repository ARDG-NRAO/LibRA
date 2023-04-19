// -*- C++ -*-
//# Roadrunner_func.h: Definition of the Roadrunner function
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

#ifndef ROADRUNNER_ROADRUNNER_FUNC_H
#define ROADRUNNER_ROADRUNNER_FUNC_H
#include <casacore/casa/namespace.h>
#include <casacore/casa/OS/Directory.h>
#include <casacore/casa/Utilities/Regex.h>
#include <casacore/casa/Logging/LogFilter.h>
#include <casacore/casa/System/ProgressMeter.h>

#include <casacore/measures/Measures/Stokes.h>
#include <casacore/coordinates/Coordinates/CoordinateSystem.h>
#include <casacore/images/Images/ImageInterface.h>
#include <casacore/images/Images/PagedImage.h>
#include <casacore/tables/TaQL/ExprNode.h>

#include <casacore/ms/MSSel/MSSelection.h>
#include <msvis/MSVis/VisibilityIterator2.h>
#include <msvis/MSVis/VisBuffer2.h>
#include <msvis/MSVis/ViFrequencySelection.h>

#include <synthesis/TransformMachines2/FTMachine.h>
#include <synthesis/TransformMachines2/AWVisResampler.h>
//#include <synthesis/TransformMachines2/PointingOffsets.h>
#include <synthesis/TransformMachines2/CFStore2.h>
#include <synthesis/TransformMachines2/AWVisResamplerHPG.h>
#include <synthesis/TransformMachines2/AWProjectWBFTHPG.h>
#include <synthesis/TransformMachines2/MakeCFArray.h>
#include <synthesis/TransformMachines2/ThreadCoordinator.h>
#include <hpg/hpg.hpp>

using namespace casa;
using namespace casa::refim;
using namespace casacore;

//vi::VisBuffer2 *vb_cfsrvr;
//vi::VisibilityIterator2 *vi2_cfsrvr;
CountedPtr<refim::FTMachine> ftm_g;
hpg::CFSimpleIndexer cfsi_g({1,false},{1,false},{1,true},{1,true}, 1);
std::shared_ptr<hpg::RWDeviceCFArray> dcfa_sptr_g;
bool isRoot=true;
//
//-------------------------------------------------------------------------
//
std::string remove_extension(const std::string& path) {
    if (path == "." || path == "..")
        return path;

    size_t pos = path.find_last_of("\\/.");
    if (pos != std::string::npos && path[pos] == '.')
        return path.substr(0, pos);

    return path;
}
//
//-------------------------------------------------------------------------
//
// Moving the embedded MPI interfaces out of the (visual) way.  If
// necessary in the future, MPI code should be implemented in a
// separate class and this code appropriate re-factored to be used as
// an API for MPI class.  Ideally, MPI code should not be part of the
// roadrunner (application layer) code.
//#include <synthesis/TransformMachines2/test/RR_MPI.h>
//
//-------------------------------------------------------------------------
// Class to manages the HPG initialize/finalize scope.
// hpg::finalize() in the destructor.  This also reports, via the
// destructor, the time elapsed between contruction and destruction.
//
class LibHPG
{
public:
  LibHPG(const bool usingHPG=true): init_hpg(usingHPG)
  {
    initialize();
  }

  bool initialize()
  {
    bool ret = true;
    startTime = std::chrono::steady_clock::now();
#ifdef ROADRUNNER_USE_HPG
    if (init_hpg) ret = hpg::initialize();
#endif
    if (!ret)
      throw(AipsError("LibHPG::initialize() failed"));
    return ret;
  };

  void finalize()
  {
#ifdef ROADRUNNER_USE_HPG
    if (init_hpg)
      {
	cerr << endl << "CALLING hpg::finalize()" << endl;
	hpg::finalize();
      }	
#endif
  };

  ~LibHPG()
  {
    finalize();

    auto endTime = std::chrono::steady_clock::now();
    std::chrono::duration<double> runtime = endTime - startTime;
    std::cerr << "roadrunner runtime: " << runtime.count()
	      << " sec" << std::endl;
  };

  std::chrono::time_point<std::chrono::steady_clock> startTime;
  bool init_hpg;
};
//
//-------------------------------------------------------------------------
//
//
//-------------------------------------------------------------------------
// The engine that loads the required CFs from the cache and copies
// them to the hpg::CFArray.
std::tuple<bool, std::shared_ptr<hpg::RWDeviceCFArray>>
prepCFEngine(casa::refim::MakeCFArray& mkCF,
	     bool WBAwp, int nW,
	     int ispw, double spwRefFreq,
	     int nDataPol,
	     casacore::ImageInterface<casacore::Float>& skyImage,
	     casacore::Vector<Int>& polMap,
	     casacore::CountedPtr<casa::refim::CFStore2>& cfs2_l
	     )
{
  LogIO log_l(LogOrigin("roadrunner","prepCFEngine"));

  //casa::refim::MakeCFArray mkCF(mndx,conj_mndx);

  casacore::Vector<Int> wNdxList;
  casacore::Vector<Int> spwNdxList;
  std::chrono::time_point<std::chrono::steady_clock>
    startMkCF=std::chrono::steady_clock::now();
  auto ret=
    mkCF.makeRWDArray(WBAwp,  nW,
		     skyImage,
		     polMap,
		     cfs2_l,
		     ispw, spwRefFreq,
		     nDataPol,
		     wNdxList, spwNdxList);
  auto endMkCF = std::chrono::steady_clock::now();
  std::chrono::duration<double> runtimeMkCF = endMkCF - startMkCF;
  bool newCF=std::get<0>(ret);
  std::shared_ptr<hpg::RWDeviceCFArray> dcf_sptr;
  if (newCF)
    {
      casa::refim::MyCFArray cfArray;
      cerr << "Make CF Array run time: " << runtimeMkCF.count() << " sec" << endl;
      cerr << "CF W list: " << wNdxList << endl << "CF SPW List: " << spwNdxList << endl;
      cfsi_g = get<2>(ret);

      dcf_sptr = std::get<3>(ret);

      log_l << "done." << LogIO::POST;
    }
  return std::make_tuple(newCF,dcf_sptr);
}
//
//-------------------------------------------------------------------------
// Server function that triggers the prepCFEngine() fro each SPW in
// the spwidList.  This is run in parallel with gridding in a separate
// thread than the gridder-thread (the main thread).  Coordination
// between the thread is managed by the ThreadCoordinator object.
//
void CFServer(ThreadCoordinator& thcoord,
	      casa::refim::MakeCFArray& mkCF,
	      bool& WBAwp, int& nW,
	      casacore::ImageInterface<casacore::Float>& skyImage,
	      casacore::Vector<Int>& polMap,
	      casacore::CountedPtr<casa::refim::CFStore2>& cfs2_l,
	      casacore::Vector<int>& spwidList,
	      casacore::Vector<double>& spwRefFreqList,
	      int& nDataPol)
{
  //for(auto i : spwidList ) cerr << i << " ";
  //cerr << endl;
  for(auto ispw : spwidList)
    {
      cerr << ".................CFServer................" << endl;

      //Declare the in-memory CFCache dirty.  Specifically, the memory
      //accessed by the dcfa_sptr_g point should be considered dirty and
      //remain so till prepCFEngine returns.
      thcoord.setCFReady(false);
      // Using the global VB to get SPWID.  While most reliable, vb_g
      // is used in the main thread while CFServer runs in another
      // thread.  This can lead to runtime issues. If this list can be
      // determined up front before starting the data iteration (which
      // uses two threads), this can be made reliable.
      double spwRefFreq = spwRefFreqList[ispw];//spwRefFreqList[ispw];
      // int vbSPWID=(vb_g)->spectralWindows()(0);
      // cerr << "iSPW: " << ispw << ", VB SPWID: " << vbSPWID  << ", SPW Ref. Freq. (Hz): " << spwRefFreq << endl;
      cerr << "iSPW: " << ispw << ", SPW Ref. Freq. (Hz): " << spwRefFreq << endl;
      auto ret = prepCFEngine(mkCF,
			      WBAwp, nW,
			      //vbSPWID,
			      ispw,
			      spwRefFreq,nDataPol,
			      skyImage,
			      polMap,
			      cfs2_l);
      thcoord.newCF=std::get<0>(ret);
      dcfa_sptr_g = std::get<1>(ret);

      thcoord.setCFReady(true);
      thcoord.Notify_CFReady();
      cerr << ".................CFServer waitForCFSent................" << endl;
      if (thcoord.isEoD())
	{
	  cerr << ".................EoD detected (CFServer exiting)................" << endl;
	  return;
	}
      thcoord.waitForCFSent();

      if (
	  ((nW == 1) && (WBAwp == true)) ||  // A-only projection
	  ((nW > 1) && (WBAwp == false)) // W-only projection
	  )
	{
	  thcoord.setEoD(true);
	  return;
	}
      // if (vi2_cfsrvr->moreChunks())
      // 	{
      // 	  vi2_cfsrvr->nextChunk();
      // 	  vi2_cfsrvr->origin(); // So that the associated vb is valid
      // 	  cerr << "Next SPWID from vi2_cfsrvr: " << vb_cfsrvr->spectralWindows()(0) << endl;
      // 	}
    }
}
//
//--------------------------------------------------------------------------------------------
// Place-holder functions to build the appropriate Mueller index
// matrix.  For production this should be generalized and perhaps
// internally build using the user stokes setup and visibility vector
// from the MS.
std::vector<std::vector<int>>
readMNdxText(const string& fileName)
{
  std::string line;
  std::vector<std::vector<int>> mat;
  std::ifstream ifs(fileName);

  if (!ifs)
    throw(AipsError(String("Error in reading file "+fileName)));

  auto split = [] (const std::string &s) -> std::vector<int> 
	  {
	    std::vector<int> elems;
	    std::istringstream iss(s);
	    while(!iss.eof())
	      {
		int v;
		iss >> v;
		elems.push_back(v);
	      }
	    return elems;
	  };

  while(std::getline(ifs,line))
    {
     std::vector<int> tok = split(line);
     mat.push_back(tok);
    }
  
  return mat;
}
//
//--------------------------------------------------------------------------------------------
// Function to build the vis-to-imageplane map (and it's conjugate).
//
// If the named file exist, it is expect to have the the map.
// Validation of the contents of this file is not yet complete.  If
// the file reading fails, a map is built based on the number grid
// planes and length of the vis vector. A warning message is given
// with the generated map and that the named file could not be read.
//
// The maps constructed here are translated in HPGVisBuffer to be
// consistent with the minimum-set visibility data that is transported
// to the GPU.
std::tuple<PolMapType, PolMapType>
makeMNdx(const string& fileName,
	 const Vector<int>& polMap,
	 const int& nGridPlanes)
{
  PolMapType mndx,conj_mndx;
  uint nDataPlanes=polMap.nelements();
  {
    std::vector<std::vector<int>> tt;
    try
      {
	tt = readMNdxText(fileName);
      }
    catch (AipsError &e)
      {
	// If fileName was not found or was unreadable, built a
	// vis-to-image map based on number of image plane and length
	// of vis vector.
	LogIO log_l(LogOrigin("roadrunner","makeMNdx"));
	log_l << e.what() << LogIO::WARN << LogIO::POST;

	// Resize and initialize the vis-to-image-planes map.
	// tt.shape = nGridPlanes rows x nDataPlanes columns
	tt.resize(nGridPlanes);
	{
	  for(uint i=0; i<tt.size(); i++)
	    {
	      tt[i].resize(nDataPlanes,-1);
	      // Special case where all visibilities map to a single
	      // image plane.
	      if (nGridPlanes==1)
		{
		  uint k=0;
		  for(uint j=0; j < tt[i].size(); j++)
		    {
		      if (polMap[j] >= 0) tt[i][j] = k++;
		      else tt[i][j]=polMap[j];
		    }
		}
	      else
		// Diagonal Mueller map.
		tt[i][i] = i;
	    }
	}
	// Some user support...
	{
	  log_l << "Setting Mueller map to ";

	  for( auto row : tt)
	    {
	      log_l << "| ";
	      for (auto col : row)
		log_l << col << " ";
	      log_l << "|";
	    }
	  log_l<<LogIO::POST;

	  log_l << "If this is not correct, to override it please provide a file named stokesI.mndx "
                << "with the mapping between image planes and visibility polarization vector."
		<< LogIO::POST;
	}
    }
    
    // Convert from STL to CASACore vector
    mndx.resize(tt.size());
    int i=0;
    for(auto r : tt)
      {
	mndx[i].resize(r.size());
	int j=0;
	for(auto c : r) mndx[i][j++]=c;
	i++;
      }
  }

  // Do some elementary error checks
  if ((mndx.nelements() < (uint)nGridPlanes) || (mndx[0].nelements() != nDataPlanes))
    {
      LogIO log_l(LogOrigin("roadrunner","makeMNdx"));
      log_l << "Expected Mueller index of shape "
	    << nGridPlanes << "x" << nDataPlanes
	    << ". Got of shape "
	    << mndx.nelements() << "x" << mndx[0].nelements()
	    << LogIO::EXCEPTION;
    }
  conj_mndx.assign(mndx);
  
  // Make the equivalent conj MNdx.  Is this general enough to make conj_mndx from mndx?
  for(int i=nGridPlanes-1,  ii=0;
      (i>=0) && (ii<(int)nGridPlanes);
      i--,ii++)
    for (int j=nDataPlanes-1, jj=0;
	 (j>=0) && (jj<(int)nDataPlanes);
	 j--,jj++)
      conj_mndx[ii][jj] = mndx[i][j];

  return make_tuple(mndx,conj_mndx);
}

void Roadrunner(bool& restartUI, int& argc, char** argv,
		string& MSNBuf, string& imageName, string& modelImageName,
		string& sowImageExt, string& cmplxGridName,
		int& NX, int& nW, float& cellSize,
		string& stokes, string& refFreqStr, string& phaseCenter,
		string& weighting, string& rmode,  float& robust,
		string& ftmName, string& cfCache, string& imagingMode,
		bool& WBAwp,
		string& fieldStr, string& spwStr, string& uvDistStr,
		bool& doPointing, bool& normalize, bool& doPBCorr,
		bool& conjBeams, float& pbLimit, vector<float>& posigdev,
		bool& doSPWDataIter)
{
  LogFilter filter(LogMessage::NORMAL);
  LogSink::globalSink().filter(filter);
  LogIO log_l(LogOrigin("roadrunner","Roadrunner_func"));

  try
    {
      // A RAII class instance that manages the HPG initialize/finalize scope.
      // And hpg::finalize() is called when this instance goes out of
      // scope.
      LibHPG libhpg(ftmName=="awphpg");
      // if (!libhpg.initialize())
      // 	{
      // 	  throw(AipsError("LibHPG::initialize() failed"));
      // 	  exit(-1);
      // 	}

      //  std::atexit(tpl_finalize);

      bool const doSow = sowImageExt != "";
      // to prevent using the same image name by all ranks, we insert
      // substrings into the given image name that encode the rank number
      // on all ranks but the root
      string imageNamePrefix =
	imageName.substr(0, imageName.find_last_of("."));

      //
      // -------------------------------------- End of UI -------------------------------------------------------------------
      //

      Timer timer;
      double griddingTime=0;

      // NB: No exception handling is intentional (for now). It's better
      // to have the program crash than hang due to uncompleted collective
      // MPI calls. Exception handling could be re-introduced, but only
      // with careful consideration of all MPI function call sequences.
      // unsigned const ranksPerMS = numRanks / msInflationFactor;

      {
	Directory dirObj(modelImageName);
	if ((modelImageName!="") && ((!dirObj.exists()) || (!dirObj.isReadable())))
	  {
	    std::string msg;
	    msg="Model image \""+modelImageName + "\" not found.";
	    throw(AipsError(msg));
	  }
      }
      // The scope below has the scientific code. That's where runtime
      // exceptions will happen.  So enclose it in a try-catch clause.
      //-------------------------------------------------------------------
      // Load the selected MS.  The original ms (thems), the selected
      // MS and the MSSelection objects are modified.  The selected
      // list of SPW and FIELD IDs are also generated.  All these are
      // currently internal but public members of the DataBase class.
      //
      Vector<int> spwidList, fieldidList;
      Vector<double> spwRefFreqList;


      DataBase db(MSNBuf, fieldStr, spwStr, uvDistStr, WBAwp, nW,
		  doSPWDataIter);

      // spwidList      = db.spwidList;
      // fieldidList    = db.fieldidList;
      // spwRefFreqList = db.spwRefFreqList;

      //
      //-------------------------------------------------------------------
      // Make the empty grid with the sky image coordinates
      //
      Vector<Int> imSize(2,NX);
      String mode="mfs", startModelImageName="";

      // casacore::MDirection pc;
      // Int pcField = casa::refim::getPhaseCenter(selectedMS,pc);
      // std::ostringstream oss;
      // pc.print(oss);
      //      cerr << "PC = " << oss << endl;

      PagedImage<Complex> cgrid=makeEmptySkyImage(*(db.vi2_l), db.selectedMS, db.msSelection,
						  cmplxGridName, startModelImageName,
						  imSize, cellSize, phaseCenter,
						  stokes, refFreqStr, mode);
      PagedImage<Float> skyImage(cgrid.shape(),cgrid.coordinates(), imageName);

      cgrid.table().markForDelete();

      // Setup the weighting scheme in the supplied VI2
      weightor(*(db.vi2_l),
	       cgrid.coordinates(), // CSys of the sky image
	       cgrid.shape(),       // X-Y shape of the sky image
	       weighting,
	       rmode,
	       casacore::Quantity(0.0, "Jy"),  // noise
	       robust);

      // remove all images except the reduced one on the root rank
      if (!isRoot)
	skyImage.table().markForDelete();

      StokesImageUtil::From(cgrid, skyImage);
      if(db.vb_l->polarizationFrame()==MSIter::Linear) StokesImageUtil::changeCStokesRep(cgrid,StokesImageUtil::LINEAR);
      else StokesImageUtil::changeCStokesRep(cgrid, StokesImageUtil::CIRCULAR);

      //-------------------------------------------------------------------
      // Create the AWP FTMachine.  The AWProjectionFT is construed
      // with the re-sampler depending on the ftmName (AWVisResampler
      // for ftmName='awproject' or AWVisResamplerHPG for
      // ftmName='awphpg')
      //
      MPosition loc;
      MeasTable::Observatory(loc, MSColumns(db.selectedMS).observation().telescopeName()(0));
      Bool useDoublePrec=true, aTermOn=true, psTermOn=false, mTermOn=false, doPSF=false;

      auto ret = 
	createAWPFTMachine(ftmName, modelImageName, ftm_g,
			   String("EVLA"),
			   loc, cfCache,
			   //cfBufferSize, cfOversampling,
			   WBAwp,nW,
			   useDoublePrec,
			   aTermOn,
			   psTermOn,
			   mTermOn,
			   doPointing,
			   doPBCorr,
			   conjBeams,
			   pbLimit,
			   posigdev,
			   imageNamePrefix,
			   imagingMode
			   );
      CountedPtr<refim::CFCache>  cfc = get<0>(ret);
      CountedPtr<refim::VisibilityResamplerBase> visResampler = get<1>(ret);
      {
	Matrix<Double> mssFreqSel;
	mssFreqSel  = db.msSelection.getChanFreqList(NULL,true);
	// Send in Freq info.
	ftm_g->setSpwFreqSelection( mssFreqSel );

	doPSF=(ftm_g->ftmType()==casa::refim::FTMachine::PSF);
      }

      //-------------------------------------------------------------------
      // Iterate over the MS, trigger the gridding.
      //
      cgrid.set(Complex(0.0));
      Matrix<Float> weight;
      db.vi2_l->originChunks();
      db.vi2_l->origin();

      // If no normalization is requested, AWProjectWBFT should not grid
      // for .weight image.
      //
      // This de-couples calculations for .weight, .psf, .residual, which
      // can now be done independently and in any order.
      //
      // The run for .residual image also save the sumwt image, since that
      // matches what production CASA does.  Which SoW out of .weight,
      // .psf, or .residual runs is the appropriate one is unresolved.
      // Probably all should be used, but this needs to be carefully
      // worked out.
      if (!normalize) ftm_g->setPBReady(true);

      if (imagingMode=="predict")
	ftm_g->initializeToVis(cgrid,*(db.vb_l));
      else
	ftm_g->initializeToSky(cgrid, weight, *(db.vb_l));

      // barrier needed for accurate elapsed time measurement
      // mpi_barrier(MPI_COMM_WORLD);
      timer.mark();

      CountedPtr<casa::refim::CFStore2> cfs2_l;
      if (!cfc.null())
	{
	  if (doPSF || (imagingMode=="weight"))
	    cfs2_l =  CountedPtr<casa::refim::CFStore2>(&cfc->memCacheWt2_p[0],false);//new CFStore2;
	  else
	    cfs2_l = CountedPtr<casa::refim::CFStore2>(&(cfc->memCache2_p)[0],false);//new CFStore2;
	}

      Vector<int> chanMap, polMap;
      visResampler->getMaps(chanMap, polMap);
      int nGridPlanes = skyImage.shape()[2]; 
      PolMapType mndx, conj_mndx;
      {
	auto ret = makeMNdx(std::string("stokesI.mndx"), polMap, nGridPlanes);
	mndx      = std::get<0>(ret);
	conj_mndx = std::get<1>(ret);

	auto prt = [](std::string tag, PolMapType& p) -> void
		   {
		     cerr << tag << endl;
		     for( auto row : p)
		       {
			 for (auto col : row) cerr << col << " ";
			 cerr << endl;
		       }
		   };
	prt(std::string("mndx:"),mndx);
	prt(std::string("cmndx:"),conj_mndx);
      }

      int nDataPol;
      // Extract SPW ref. frequency and the number of polarizations in the
      // data (nDataPol).
      {
	db.vi2_l->originChunks();
	db.vi2_l->origin();
	spwRefFreqList.assign(db.vb_l->subtableColumns().spectralWindow().refFrequency().getColumn());
	nDataPol  = db.vb_l->flagCube().shape()[0];
      }
      //
      // Finally, the data iteration loops.
      //-----------------------------------------------------------------------------------
      double griddingEngine_time=0;
      unsigned long vol=0;
      ProgressMeter pm(1.0, db.vi2_l->ms().nrow(),
		       "Gridding", "","","",true);
      DataIterator di(isRoot);
      
      if (ftm_g->name() != "AWProjectWBFTHPG")
	{
	  // auto waitForCFReady = [](int&, int&){}; // NoOp
	  // auto notifyCFSent   = [](const int&){}; //NoOp
	  auto ret = di.dataIter(db.vi2_l, db.vb_l, ftm_g,doPSF,imagingMode);
	  griddingEngine_time += std::get<2>(ret);
	  vol+= std::get<1>(ret);
	}
      else // if (ftm_g->name()=="AWProjectWBFTHPG")
	{
	  // Start the CFServer in a separate thread.  This thred does
	  // a look-ahead for the set of CFs required and loads them
	  // in the memory data structure needed for sending them to
	  // the device.  All this gets done in parallel with the
	  // gridding which is managed by the main thread (this
	  // thread).
	  //
	  // Gridding/De-gridding is triggered via di.dataIter() call
	  // below, which is modified by lambda functions supplied to
	  // cooridinate with the CFServer thread.
	  //
	  casa::refim::MakeCFArray mkCF(mndx,conj_mndx);
	  ThreadCoordinator thcoord;
	  thcoord.newCF=false;

	  // Start the CFServer thread.
	  auto cfPrep = std::async(&CFServer,
				   std::ref(thcoord),
				   std::ref(mkCF),
				   std::ref(WBAwp), std::ref(nW),
				   std::ref(skyImage),
				   std::ref(polMap),
				   std::ref(cfs2_l),
				   std::ref(db.spwidList),
				   std::ref(spwRefFreqList),
				   std::ref(nDataPol));

	  // First set of CFs have to be ready before proceeding. The
	  // ThreadCoordinator state after this remains CFReady=true,
	  // since the call to .waitForCFReady_or_EoD() in the
	  // DataIterator should immediately return.  The CFReady state is
	  // reset in the lambda function code passed to the DataIterator
	  // for thread coordination.
	  thcoord.waitForCFReady_or_EoD();

	  //-------------------------------------------------------------------------------------------
	  // Lambda functions with the code to coordinate the two threads
	  // via the data iteration loops in the DataIterator object.  The
	  // following functions assume that the MS iteraters over the SPW
	  // axis.
	  //
	  // If the VB has a new SPW ID, increment the index into the list
	  // of the selected SPW IDs, and wait for CFReady signal from the
	  // CFServer thread. Once that is received, send the current CFSI
	  // and DeviceCFArray to the visResampler. This function is used
	  // in the chunk iterations of VI2.  When this returns,
	  // VisResampler is ready for gridding, which is then triggered.
	  // That in-turn first sends the current DeviceCFArray to the
	  // device and issues the CFSent signal via a call to the
	  // notifyCFSent() function.  This unblocks the CFServe thread,
	  // which proceeds to load the next CF set.
	  //
	  auto waitForCFReady =
	    [&thcoord, &visResampler,&db](int& nVB, int& spwNdx)
	    {
	      if (db.vb_l->spectralWindows()(0) != db.spwidList[spwNdx])
		{
		  nVB=0;    // Reset the VB count for the new SPW
		  spwNdx++; // Advance the SPW ID counter
		  //
		  // At this point the internal state of the
		  // ThreadCoordinator object remains CFReady=True from an
		  // earlier call to thcoord.waitForCFReady_or_EoD(), soon
		  // after starting the CFServer thread
		  // (std::async(&CFServer,...).  This earlier call is to
		  // make sure that the code does not proceed to using the
		  // data iterators till the first CFs are loaded and
		  // ready for use in the memory.
		  //
		  // .setCFReady(false) explicitly sets the internal state
		  // of the ThreadCoordinator object to CFReady=false.
		  //
		  thcoord.waitForCFReady_or_EoD(); // Wait for notification from the CFServer
		  thcoord.setCFReady(false);
		}
	      if (thcoord.newCF)
		{
		  visResampler->setCFSI(cfsi_g);
		  
		  //if (visResampler->set_cf(dcfa_sptr_g)==false)
		  //		  cerr << "WFCFR: " << dcfa_sptr_g << endl;
		  if (visResampler->set_cf(std::move(dcfa_sptr_g))==false)
		    throw(AipsError("Device CFArray pointer is null in CFServer"));
		  thcoord.newCF=false;
		}
	    };
	  //-------------------------------------------------------------------------------------------
	  auto notifyCFSent =
	    [&thcoord](const int& nVB)
	    {
	      if ((nVB==0) && (!thcoord.isEoD()))
		{
		  cerr << "gridderEngine: CFSent notification" << endl;
		  thcoord.setCFSent(true);
		  thcoord.newCF=false;
		  thcoord.Notify_CFSent();
		  dcfa_sptr_g.reset();
		}
	    };
	  //-------------------------------------------------------------------------------------------

	  try
	    {
	      auto ret = di.dataIter(db.vi2_l, db.vb_l, ftm_g,doPSF,imagingMode,
				     waitForCFReady, notifyCFSent);
	      griddingEngine_time += std::get<2>(ret);
	      vol+= std::get<1>(ret);
	    }
	  catch (AipsError &er)
	    {
	      throw(er);
	    }
	  // Trying to ensure that the CFServer thread gets the signal
	  // that EoD has been reached in the main thread.
	  thcoord.setEoD(true);
	  thcoord.setCFSent(true);
	  thcoord.Notify_CFSent();
	  //cfPrep.wait(); // The main thread does not have to wait for the CFServer thread to exit.
	}
      // End of data iteration loops
      //-----------------------------------------------------------------------------------

      cerr << "Cumulative time in griddingEngine: " << griddingEngine_time << " sec" << endl;
      unsigned long allVol=vol;
      // #ifdef ROADRUNNER_USE_MPI 
      //   allVol=0;
      //   mpi_reduce(&vol, &allVol, 1, MPI_UNSIGNED_LONG, MPI_SUM, 0, MPI_COMM_WORLD);
      // #endif
      log_l << "Total rows processed: " << allVol << LogIO::POST;
      {
	// NB: to be safe, we ensure that value returned by getGridPtr()
	// (a shared_ptr) releases its managed object as soon as we're
	// done with it by assigning the value to a variable with block
	// scope
	size_t gridValuesCount;
	auto gridValuesPtr = ftm_g->getGridPtr(gridValuesCount);
	assert((size_t)((int)gridValuesCount) == gridValuesCount);
	// mpi_reduce(
	//   (isRoot ? MPI_IN_PLACE : gridValuesPtr.get()),
	//   gridValuesPtr.get(),
	//   static_cast<int>(gridValuesCount),
	//   gridValueDatatype,
	//   MPI_SUM,
	//   0,
	//   MPI_COMM_WORLD);
	if (doSow) {
	  // NB: similar caution (as getSumWeightsPtr() value) is
	  // warranted for value returned by getGridPtr()
	  size_t gridWeightsCount;
	  auto gridWeightsPtr = ftm_g->getSumWeightsPtr(gridWeightsCount);
	  // mpi_reduce(
	  //   (isRoot ? MPI_IN_PLACE : gridWeightsPtr.get()),
	  //   gridWeightsPtr.get(),
	  //   static_cast<int>(gridWeightsCount),
	  //   gridWeightDatatype,
	  //   MPI_SUM,
	  //   0,
	  //   MPI_COMM_WORLD);
	}
      }

      // complete the gridding only on the root, after reduction
      //if (isRoot && (imagingMode!="predict"))
      if (imagingMode!="predict")
	{
	  griddingTime += timer.real();
	  log_l << "Gridding time: " << griddingTime << ". No. of rows processed: " << allVol << ".  Data rate: " << allVol/griddingTime << " rows/s" << LogIO::POST;

	  if (cmplxGridName!="")
	    visResampler->saveGriddedData(cmplxGridName+".vis",cgrid.coordinates());

	  // Do the FFT of the grid on the device and get the grid and
	  // weights to the CPU memory.
	  //
	  // The call sequence is is
	  // AWProjectWBFT->finalizeToSky()-->AWVisResamplerHPG::finalizeToSky()-->[GPU
	  // FFT + gatherGrids()].  This gets the image to the FMT
	  // griddedData object, which is set to be the cgrid
	  // PagedImage<Complex> in roadrunner via FTM::initializeToSky().
	  ftm_g->finalizeToSky();

	  // Normalize the image.  normalize=True will divide the FFT'ed 
	  // image by the weights (which is SoW)
	  //
	  //Bool normalize=false;
	  ftm_g->getImage(weight, normalize);

	  casacore::Matrix<double> sow_dp=ftm_g->getSumWeights();
	  IPosition shp=sow_dp.shape();
	  // Save .sumwt in DP
	  // .sumwt is REQUIRED to be a float image somewhere in the CASA framework!
	  casacore::Array<float> sow(IPosition(4,shp(0),shp(1),1,1));
	  //casacore::Array<double> sow(IPosition(4,shp(0),shp(1),1,1));

	  for (auto i=0;i<shp(0);i++)
	    for (auto j=0;j<shp(1);j++)
	      sow(IPosition(4,i,j,0,0))=sow_dp(i,j);

	  log_l << "main: Sum of weights: " << sow << LogIO::POST; // casacore::LogIO is not inherited from std::streams!  Can't use std::setprecision(). 
	  cerr << "main: Sum of weights: " << std::setprecision((std::numeric_limits<long double>::digits10 + 1)) << sow(IPosition(4,0,0,0,0)) << endl;

	  // Save the SoW as an image.
	  if (doSow)
	    {
	      // Split any extension in imageName to construct a name with
	      // same base name and extension given by sowImageExt
	      std::string baseName=imageName;
	      baseName=remove_extension(imageName);

	      PagedImage<float> sowImage(sow.shape(),cgrid.coordinates(), baseName+"."+sowImageExt);

	      // Not sure what this info. is about, and if it is
	      // necessary.  It is present in the SoW image written to
	      // the disk from CASA framework.
	      Record miscinfo;
	      miscinfo.define("INSTRUME", "EVLA");
	      miscinfo.define("distance", 0.0);
	      miscinfo.define("useweightimage", true);
	      sowImage.setMiscInfo(miscinfo);
	      sowImage.put(sow);
	      //storeArrayAsImage(name+".sumwt", cgrid.coordinates(), sow);
	    }
	  // Convert the skyImage (retrieved in ftm->finalizeToSky()) and
	  // convert it from Feed basis to Stokes basis.
	  StokesImageUtil::To(skyImage, cgrid);
	}
      //MSes are detach for cleaning up when the DataBase object goes
      //out of scope here.
      log_l << "...done" << LogIO::POST;
    }
  catch(AipsError& er)
    {
      log_l << er.what() << LogIO::SEVERE;
    }
}
#endif
