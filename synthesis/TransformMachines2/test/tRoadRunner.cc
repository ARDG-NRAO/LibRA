//# roadrunner.cc: Driver for the AWProject class of FTMachines
//# Copyright (C) 2021
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This program is free software; you can redistribute it and/or modify it
//# under the terms of the GNU General Public License as published by the Free
//# Software Foundation; either version 2 of the License, or (at your option)
//# any later version.
//#
//# This program is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
//# more details.
//#
//# You should have received a copy of the GNU General Public License along
//# with this program; if not, write to the Free Software Foundation, Inc.,
//# 675 Massachusetts Ave, Cambridge, MA 02139, USA.
//#
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//# $Id$
#define ROADRUNNER_USE_HPG 1
//#undef ROADRUNNER_USE_MPI 

#include <casacore/casa/Arrays/ArrayMath.h>
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

//
// Following are from the parafeed project (the UI library)
//
// #include <cl.h> // C++ized version
// #include <clinteract.h>

 #include </home/pjaganna/Software/casa_build_native/linux_64b/include/parafeed/cl.h> // C++ized version
 #include </home/pjaganna/Software/casa_build_native/linux_64b/include/parafeed/clinteract.h>


#ifdef ROADRUNNER_USE_MPI
# include <mpi.h>
#endif
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <chrono>
#include <future>
#include <fstream>
#include <strstream>
//
//================================================================================
//
using namespace casa;
using namespace casa::refim;
using namespace casacore;

#include <synthesis/TransformMachines2/test/rWeightor.inc>
#include <synthesis/TransformMachines2/test/DataIterations.h>

std::tuple<CountedPtr<refim::CFCache>,CountedPtr<refim::VisibilityResamplerBase>>
createAWPFTMachine(const String ftmName,
		   const String modelImageName,
		   CountedPtr<refim::FTMachine>& theFT, 
		   const String& telescopeName,
		   MPosition& observatoryLocation,
		   const String cfCache= "testCF.cf",
		   const Bool wbAWP= true,
		   //------------------------------
		   const Int wprojPlane=1,
		   //const Float=1.0,
		   const Bool useDoublePrec=true,
		   //------------------------------
		   const Bool aTermOn= true,
		   const Bool psTermOn= false,
		   const Bool mTermOn= false,
		   const Bool doPointing= true,
		   const Bool doPBCorr= true,
		   const Bool conjBeams= true,
		   Float pbLimit_l=1e-3,
		   vector<float> posigdev = {300.0,300.0},
		   const String imageNamePrefix=String(""),
		   const String imagingMode="residual",
		   const Float computePAStep=360.0,
		   const Float rotatePAStep=360.0,
		   const Int cache=1000000000,
		   const Int tile=16)
  
{
  LogIO os( LogOrigin("roadrunner","createAWPFTMachine",WHERE));
  
  if (wprojPlane<=1)
    {
      os << LogIO::NORMAL
	 << "You are using wprojplanes=1. Doing co-planar imaging (no w-projection needed)" 
	 << LogIO::POST;
      os << LogIO::NORMAL << "Performing WBA-Projection" << LogIO::POST; // Loglevel PROGRESS
    }
  else
    os << LogIO::NORMAL << "Performing WBAW-Projection" << LogIO::POST; // Loglevel PROGRESS
  // if((wprojPlane>1)&&(wprojPlane<64)) 
  //   {
  //     os << LogIO::WARN
  // 	 << "No. of w-planes set too low for W projection - recommend at least 128"
  // 	 << LogIO::POST;
  //   }
  
  // MSObservationColumns msoc(mss4vi_p[0].observation());
  // String telescopeName=msoc.telescopeName()(0);
  CountedPtr<refim::ConvolutionFunction> awConvFunc = AWProjectFT::makeCFObject(telescopeName, 
									 aTermOn,
									 psTermOn, true, mTermOn, wbAWP,
									 false);
  //
  // PointingOffsets is now a private object in AWP.
  //
  // CountedPtr<refim::PointingOffsets> po = new refim::PointingOffsets();
  // po->setOverSampling(awConvFunc->getOversampling());
  // po->setDoPointing(doPointing);
  // awConvFunc->setPointingOffsets(po);

  //
  // Construct the appropriate re-sampler.
  //
  CountedPtr<refim::VisibilityResamplerBase> visResampler;
  if (ftmName == "awphpg") visResampler = new refim::AWVisResamplerHPG(false);
  else visResampler = new refim::AWVisResampler();
  visResampler->setModelImage(modelImageName);
  //
  // Construct and initialize the CF cache object.
  //
  CountedPtr<refim::CFCache> cfCacheObj;
  
  //
  // Finally construct the FTMachine with the CFCache, ConvFunc and
  // Re-sampler objects.  
  //
  if (ftmName == "awphpg")
    theFT = new refim::AWProjectWBFTHPG(wprojPlane, cache/2,
					   cfCacheObj, awConvFunc,
					   visResampler,
					   /*true */doPointing, posigdev, doPBCorr,
					   tile, computePAStep, pbLimit_l, true,conjBeams,
					   useDoublePrec);
  else
    theFT = new refim::AWProjectWBFT(wprojPlane, cache/2,
					   cfCacheObj, awConvFunc,
					   visResampler,
					   /*true */doPointing, posigdev, doPBCorr,
					   tile, computePAStep, pbLimit_l, true,conjBeams,
					   useDoublePrec);
  
  if (imagingMode=="weight")    {theFT->setFTMType(casa::refim::FTMachine::WEIGHT);}
  else if (imagingMode=="psf")  {theFT->setFTMType(casa::refim::FTMachine::PSF);}
  else if (imagingMode=="snrpsf")  {theFT->setFTMType(casa::refim::FTMachine::SNRPSF);}

  cfCacheObj = new refim::CFCache();
  cfCacheObj->setCacheDir(cfCache.data());
  cfCacheObj->setLazyFill(refim::SynthesisUtils::getenv("CFCache.LAZYFILL",1)==1);
  cfCacheObj->setWtImagePrefix(imageNamePrefix.c_str());
  //cfCacheObj->initCache2(); // This loads, both CFS* and WTCFS* CFs.  In roadrunner, only one of them required (depending on the "imagingmode" parameter)
  if (theFT->ftmType()   == casa::refim::FTMachine::PSF 
      || theFT->ftmType()== casa::refim::FTMachine::WEIGHT)
    cfCacheObj->initCache2(false, 400.0, -1.0,casacore::String("WTCFS*")); // This would load only WTCFS* CFs
  else
    cfCacheObj->initCache2(false, 400.0, -1.0,casacore::String("CFS*")); // This would load only CFS* CFs
  // {
  //   CountedPtr<casa::refim::CFBuffer> cfb_l = cfCacheObj->memCache2_p[0].getCFBuffer(0,0);
  //   cfb_l->show("cfb: ");
  // }
  
  theFT->setCFCache(cfCacheObj);
  
  
  Quantity rotateOTF(rotatePAStep,"deg");
  static_cast<refim::AWProjectFT &>(*theFT).setObservatoryLocation(observatoryLocation);
  static_cast<refim::AWProjectFT &>(*theFT).setPAIncrement(Quantity(computePAStep,"deg"),rotateOTF);

  // Send in Freq info. Reference code for enabling arbitrary
  // freq. resolution for wbawp imaging (currently it is set to SPW
  // resolution).
  //
  // os << "Sending frequency selection information " <<  mssFreqSel_p  <<  " to AWP FTM." << LogIO::POST;
  // theFT->setSpwFreqSelection( mssFreqSel_p );
  // theIFT->setSpwFreqSelection( mssFreqSel_p );

  std::tuple<CountedPtr<refim::CFCache>,CountedPtr<refim::VisibilityResamplerBase>> retup = std::make_tuple(cfCacheObj,visResampler); 

  return retup;
}
//
//-------------------------------------------------------------------------
//
#define RestartUI(Label)  {if(clIsInteractive()) {goto Label;}}
//#define RestartUI(Label)  {if(clIsInteractive()) {clRetry();goto Label;}}
//
void UI(Bool restart, int argc, char **argv, string& MSNBuf,
	string& imageName, string& modelImageName,string& sowImageExt,
	string& cmplxGridName, int& ImSize, int& nW,
	float& cellSize, string& stokes, string& refFreqStr,
	string& phaseCenter, string& weighting,
	string& rmode, float& robust,
	string& FTMName, string& CFCache, string& imagingMode,
	Bool& WBAwp,string& fieldStr, string& spwStr,
	string& uvDistStr,
	Bool& doPointing, Bool& normalize,
	Bool& doPBCorr,
	Bool& conjBeams,
	Float& pbLimit,
	vector<float> &posigdev,
	Bool& doSPWDataIter)
{
  if (!restart)
    {
	BeginCL(argc,argv);
	clInteractive(0);
      //clCmdLineFirst(0);//In an interactive session, this is supposed
			//to keep the order of the keywords presented
			//the same as in the code.  But it does not!
			//The library always behaves like
			//clCmdLineFirst(1).
      }
  // else
  //   clRetry();
 REENTER:
      try
    {
      SMap watchPoints; VString exposedKeys;
      int i;
      MSNBuf="";
      i=1;clgetSValp("ms", MSNBuf,i);  
      i=1;clgetSValp("imagename", imageName,i);  
      i=1;clgetSValp("modelimagename", modelImageName,i);  
      i=1;clgetSValp("sowimageext", sowImageExt,i);  
      i=1;clgetSValp("complexgrid", cmplxGridName,i);  

      i=1;clgetIValp("imsize", ImSize,i);  
      i=1;clgetFValp("cellsize", cellSize,i);  
      i=1;clgetSValp("stokes", stokes,i);  clSetOptions("stokes",{"I","IV"});
      i=1;clgetSValp("reffreq", refFreqStr,i);  
      i=1;clgetSValp("phasecenter", phaseCenter,i);  

      InitMap(watchPoints,exposedKeys);
      exposedKeys.push_back("rmode");
      exposedKeys.push_back("robust");
      watchPoints["briggs"]=exposedKeys;

      //Add watchpoints for exposing rmode and robust parameters when weight=briggs
      i=1;clgetSValp("weighting", weighting, i ,watchPoints);
      clSetOptions("weighting",{"natural","uniform","briggs"});

      i=1;clgetSValp("rmode", rmode,i);  
      i=1;clgetFValp("robust", robust,i);  
      

      i=1;clgetIValp("wplanes", nW,i);  
      i=1;clgetSValp("ftm", FTMName,i); clSetOptions("ftm",{"awphpg","awproject"});
      i=1;clgetSValp("cfcache", CFCache,i);
      i=1;clgetSValp("mode", imagingMode,i); clSetOptions("mode",{"weight","psf","snrpsf","residual","predict"});
      i=1;clgetBValp("wbawp", WBAwp,i); 
      i=1;clgetSValp("field", fieldStr,i);
      i=1;clgetSValp("spw", spwStr,i);
      i=1;clgetSValp("uvdist", uvDistStr,i);
      i=1;clgetBValp("pbcor", doPBCorr,i);
      i=1;clgetBValp("conjbeams", conjBeams,i);
      i=1;clgetFValp("pblimit", pbLimit,i);
      InitMap(watchPoints,exposedKeys);
      exposedKeys.push_back("pointingoffsetsigdev");
      watchPoints["1"]=exposedKeys;


      i=1;clgetBValp("dopointing", doPointing,i,watchPoints);
      i=2;clgetNFValp("pointingoffsetsigdev", posigdev,i);

      i=1;dbgclgetBValp("normalize",normalize,i);
      i=1;dbgclgetBValp("spwdataiter",doSPWDataIter,i);
     EndCL();
    }
  catch (clError x)
    {
      x << x << endl;
      if (x.Severity() == CL_FATAL) exit(1);
      //clRetry();
      RestartUI(REENTER);
    }
}
//
//-------------------------------------------------------------------------
//
bool mdFromString(casacore::MDirection &theDir, const casacore::String &in)
{
   bool rstat(false);
   String tmpA, tmpB, tmpC;
   std::istringstream iss(in);
   iss >> tmpA >> tmpB >> tmpC;
   ushort ntrials=0;

   while((rstat==false) && (ntrials < 2))
     {
       try
	 {
	   casacore::Quantity tmpQA;
	   casacore::Quantity tmpQB;
	   casacore::Quantity::read(tmpQA, tmpA);
	   casacore::Quantity::read(tmpQB, tmpB);
	   if(tmpC.length() > 0)
	     {
	       MDirection::Types theRF;
	       MDirection::getType(theRF, tmpC);
	       theDir = MDirection (tmpQA, tmpQB, theRF);
	       rstat = true;
	     }
	   else
	     {
	       theDir = MDirection (tmpQA, tmpQB);
	       rstat = true;
	     }
	 }
       catch (AipsError& e)
	 {
	   cerr << e.what() << endl;
	   if (ntrials > 1) throw(e);
	   // Assume that the string is in the EPOCH RA DEC format
	   // (and therefore failed).  So try RA DEC EPOCH format.
	   String tt=tmpA;
	   tmpA=tmpB;
	   tmpB=tmpC;
	   tmpC=tt;
	   cerr << "###Informational: Re-trying phasecenter specification as " << tmpA << " " << tmpB << " " << tmpC << "...";
	   ntrials++;
	 }
     }
   if (ntrials > 0) {rstat ? cerr << "success!" << endl : cerr << "failed." << endl;}

   return rstat;
}
//
//-------------------------------------------------------------------------
//
std::map<casacore::Int, std::map<casacore::Int, casacore::Vector<casacore::Int> > >
makeTheChanSelMap(MSSelection& mss)
{
  Matrix<Int> mssChanSel = mss.getChanList();
  IPosition mssChanSelShape=mssChanSel.shape();

  std::map<casacore::Int, std::map<casacore::Int, casacore::Vector<casacore::Int> > > chanselKurfuffel;
  std::map<casacore::Int, casacore::Vector<casacore::Int> > channelsPerSpw;
  for(int i=0;i<mssChanSelShape[0];i++)
    {
      Vector<Int> channels(2,-1);
      int spw=mssChanSel(i,0);
      channels[0]=mssChanSel(i,2)-mssChanSel(i,1)+1;//mssChanSel(i,1);
      channels[1]=mssChanSel(i,1);//mssChanSel(i,2);
      channelsPerSpw[spw]=channels;
      //      cerr << "SPW:CHAN " << spw << " " << channels << endl;
    }
  chanselKurfuffel[0]=channelsPerSpw;
  return chanselKurfuffel;
}
//
//-------------------------------------------------------------------------
//
PagedImage<Complex> makeEmptySkyImage(VisibilityIterator2& vi2,
				      const MeasurementSet& selectedMS,
				      MSSelection& msSelection,
				      const String& imageName, const String& startModelImageName,
				      const Vector<Int>& imSize, const float& cellSize,
				      const String& phaseCenter, const String& stokes,
				      const String& refFreq,
				      const String& mode)
{
  Vector<Quantity> qCellSize(2);
  for (int i=0;i<2;i++)
    {
      String qUnit("arcsec");
      qCellSize[i]=Quantity(cellSize, qUnit);
    }

  int imStokes=1;
  if (stokes=="I") imStokes=1;
  else if (stokes=="IV") imStokes=2;
  else if (stokes=="IQUV") imStokes=4;

  int imNChan=1;
  if (mode=="mfs") imNChan=1;
  // else if (mode=="pseudo") {}
  // else if (mode=="spectral") {imnchan=datanchan[0];imstart=datastart[0];imstep=datastep[0];}
  
  MDirection mphaseCenter;
  mdFromString(mphaseCenter, phaseCenter);

  SynthesisParamsGrid gridParams;
  SynthesisParamsImage imageParams;
  imageParams.imageName=imageName;
  imageParams.startModel=startModelImageName;
  imageParams.imsize=imSize;
  imageParams.cellsize=qCellSize;
  imageParams.phaseCenter = mphaseCenter;
  imageParams.stokes=stokes;
  imageParams.mode=mode;
  imageParams.frame=String("LSRK");
  imageParams.veltype=String("radio");

  //
  // There are two items related to ref. freq. "reffreq" (a String) and "refFreq" (a Quantity).
  // Set them both.
  //
  if (refFreq != "")
    {
      imageParams.reffreq=refFreq;
     
      std::istringstream iss(refFreq);
      Double ff;
      iss >> ff;
      //      cerr << "Ref. freq. = " << ff << endl;
      imageParams.refFreq=Quantity(ff,"GHz");
    }
  
  casacore::Block<const casacore::MeasurementSet *> msList(1); msList[0]=&selectedMS;
  CoordinateSystem csys = imageParams.buildCoordinateSystem(vi2,makeTheChanSelMap(msSelection),msList);
  IPosition imshape(4,imSize(0),imSize(1),imStokes,imNChan);
  return PagedImage<Complex>(imshape, csys, imageParams.imageName+".tmp");
}
//
//-------------------------------------------------------------------------
//
std::tuple<Vector<Int>, Vector<Int> > loadMS(const String& msname,
					     const String& spwStr,
					     const String& fieldStr,
					     const String& uvDistStr,
					     MeasurementSet& thems,
					     MeasurementSet& selectedMS,
					     MSSelection& msSelection)
{
  //MeasurementSet thems;
  if (Table::isReadable(msname))
    thems=MeasurementSet(msname, Table::Update);
  else
    throw(AipsError(msname+" does exist or not readable"));
  //
  //-------------------------------------------------------------------
  //
  msSelection.setSpwExpr(spwStr);
  //msSelection.setAntennaExpr(antStr);
  msSelection.setFieldExpr(fieldStr);
  msSelection.setUvDistExpr(uvDistStr);
  selectedMS = MeasurementSet(thems);
  Vector<int> spwid, fieldid;
  TableExprNode exprNode=msSelection.toTableExprNode(&thems);
  if (!exprNode.isNull())
    {
      selectedMS = MS(thems(exprNode));
      // TODO: should the following statements be moved outside this
      // block?
      spwid=msSelection.getSpwList();
      fieldid=msSelection.getFieldList();
    }
  return std::tuple<Vector<Int>, Vector<Int> >{spwid, fieldid};
}
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
#include <synthesis/TransformMachines2/test/RR_MPI.h>
//
//-------------------------------------------------------------------------
//
class LibHPG
{
public:
  LibHPG(const bool usingHPG=true): init_p(usingHPG) {};

  bool initialize()
  {
#ifdef ROADRUNNER_USE_HPG
    if (init_p)	return hpg::initialize();
#endif
    return true;
  };

  void finalize()
  {
#ifdef ROADRUNNER_USE_HPG
    if (init_p)
      {
	cerr << endl << "CALLING hpg::finalize()" << endl;
	hpg::finalize();
      }	
#endif
  };

  ~LibHPG(){finalize();};

  bool init_p;
};
//
//-------------------------------------------------------------------------
//

LibHPG* libhpg;

#ifdef ROADRUNNER_USE_HPG
bool hpg_initialize()
{
  return hpg::initialize();
}
//
//-------------------------------------------------------------------------
//
void hpg_finalize()
{
  hpg::finalize();
}
#else // !ROADRUNNER_USE_HPG
//
//-------------------------------------------------------------------------
//
bool hpg_initialize()
{
  return true;
}
//
//-------------------------------------------------------------------------
//
void hpg_finalize()
{
  // no-op
}
#endif // ROADRUNNER_USE_HPG

// we need a global variable to know whether the call to UI() on the
// root rank has returned normally or called exit(), because an exit()
// leaves a broadcast pending
bool bcastQuitOnExit;
std::chrono::time_point<std::chrono::steady_clock> startTime;
//
//-------------------------------------------------------------------------
//
void tpl_initialize(int* argc, char*** argv)
{
  mpi_init(argc, argv);
  mpi_barrier(MPI_COMM_WORLD);
  startTime = std::chrono::steady_clock::now();
  //hpg_initialize();
  libhpg->initialize();
  // UIParams::datatype = UIParams::compute_datatype();
  // mpi_type_commit(&UIParams::datatype);
}

void tpl_finalize()
{
  // if (bcastQuitOnExit) {
  //   UIParams params;
  //   params.doQuit = true;
  //   mpi_bcast(&params, 1, UIParams::datatype, 0, MPI_COMM_WORLD);
  // }
  // mpi_type_free(&UIParams::datatype);
  //hpg_finalize();
  delete libhpg;
  
  // mpi_barrier(MPI_COMM_WORLD);
  auto endTime = std::chrono::steady_clock::now();
  std::chrono::duration<double> runtime = endTime - startTime;
  // int rank;
  // mpi_comm_rank(MPI_COMM_WORLD, &rank);
  // mpi_finalize();
  // if (rank == 0)
    std::cerr << "roadrunner runtime: " << runtime.count()
              << " sec" << std::endl;
}
//
//-------------------------------------------------------------------------
//
#ifdef ROADRUNNER_USE_HPG
static const string defaultFtmName = "awphpg";
#else // !ROADRUNNER_USE_HPG

static const string defaultFtmName = "awproject";
#endif // ROADRUNNER_USE_HPG
//
//-------------------------------------------------------------------------
//
vi::VisBuffer2 *vb_g;
//vi::VisBuffer2 *vb_cfsrvr;
vi::VisibilityIterator2 *vi2_g;
//vi::VisibilityIterator2 *vi2_cfsrvr;
CountedPtr<refim::FTMachine> ftm_g;
hpg::CFSimpleIndexer cfsi_g({1,false},{1,false},{1,true},{1,true}, 1);
std::shared_ptr<hpg::RWDeviceCFArray> dcfa_sptr_g;
//
//-------------------------------------------------------------------------
//
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
      int vbSPWID=(vb_g)->spectralWindows()(0);
      //double spwRefFreq = spwRefFreqList[vbSPWID];
      double spwRefFreq = spwRefFreqList[ispw];//spwRefFreqList[ispw];
      cerr << "iSPW: " << ispw << ", VB SPWID: " << vbSPWID  << ", SPW Ref. Freq. (Hz): " << spwRefFreq << endl;
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
//
//-------------------------------------------------------------------------
//
// #include <synthesis/TransformMachines2/test/ImagingEngine.h>
//   ImagingEngine imgEngine();
//
//-------------------------------------------------------------------------
//
int main(int argc, char **argv)
{
  //
  // -------------------------------------- Just the UI -------------------------------------------------------------------
  //
  // The Factory Settings.
  string MSNBuf,ftmName=defaultFtmName,
    cfCache, fieldStr="", spwStr="*", uvDistStr="",
    imageName, modelImageName,cmplxGridName="",phaseCenter, stokes="I",
    refFreqStr="3.0e9", weighting="natural", sowImageExt,
    imagingMode="residual",rmode="none";

  float cellSize;//refFreq=3e09, freqBW=3e9;
  float robust=0.0;
  int NX=0, nW=1;//cfBufferSize=512, cfOversampling=20, nW=1;
  Bool WBAwp=true;
  Bool restartUI=false;
  Bool doPointing=false;
  Bool normalize=false;
  Bool doPBCorr= true;
  Bool conjBeams= true;
  Float pbLimit=1e-3;
  Bool doSPWDataIter=false;
  vector<float> posigdev = {300.0,300.0};

  // int rank;
  // int numRanks;
  // mpi_comm_size(MPI_COMM_WORLD, &numRanks);
  // mpi_comm_rank(MPI_COMM_WORLD, &rank);
  bool const isRoot = true;

  // get UI parameter: load on root, then broadcast
  // UIParams params;
  //  if (isRoot)
    {
      // because UI may call std::exit on root, while other ranks go
      // straight to mpi_bcast, we track whether parameter processing
      // completed normally through a global variable, which is used
      // in tpl_finalize() (an exit handler) to complete the bcast if
      // needed
      // bcastQuitOnExit= true;
      {
	UI(restartUI, argc, argv, MSNBuf,imageName, modelImageName,
	   sowImageExt, cmplxGridName, NX, nW, cellSize,
	   stokes, refFreqStr, phaseCenter, weighting, rmode, robust,
	   ftmName,cfCache, imagingMode, WBAwp,fieldStr,spwStr,uvDistStr,
	   doPointing,normalize,doPBCorr, conjBeams, pbLimit, posigdev,
	   doSPWDataIter);
      }
      set_terminate(NULL);
      // do some input parameter checking now; return on error before
      // bcastQuitOnExit is set to false ensures bcast completion in
      // that cast
      // if (phaseCenter == "")
      //   {
      //     cerr << "The phasecenter parameter needs to be set" << endl;
      //     return 1;
      //   }
      // if (refFreqStr == "")
      //   {
      //     cerr << "The reffreq parameter needs to be set" << endl;
      //     return 1;
      //   }
      // if (NX <= 0)
      //   {
      //     cerr << "The imsize parameter needs to be set to a positive finite value" << endl;
      //     return 1;
      //   }
      // if (cellSize <= 0)
      //   {
      //     cerr << "The cellsize parameter needs to be set to a positive finite value" << endl;
      //     return 1;
      //   }

      // UIParams rootParams(
      // 			  MSNBuf, ftmName, cfCache, fieldStr, spwStr, uvDistStr,
      // 			  imageName, modelImageName, cmplxGridName, phaseCenter, stokes,
      // 			  refFreqStr, weighting, rmode, robust, sowImageExt, imagingMode,
      // 			  cellSize, NX, nW, nIntDummy, nchanDummy, WBAwp, restartUI, doPointing,
      // 			  normalize, doPBCorr, conjBeams, pbLimit, posigdev,
      // 			  doSPWDataIter);
      // params = rootParams;
    }

  libhpg  = (new LibHPG(ftmName=="awphpg"));
  tpl_initialize(&argc, &argv);
  //  std::atexit(tpl_finalize);
  
  bcastQuitOnExit = false;
  // broadcast params
  // mpi_bcast(&params, 1, UIParams::datatype, 0, MPI_COMM_WORLD);
  // if (params.doQuit) // quit when root has told us to quit

  // params.unpack(
  //   MSNBuf, ftmName, cfCache, fieldStr, spwStr, uvDistStr,
  //   imageName, modelImageName, cmplxGridName, phaseCenter, stokes,
  //   refFreqStr, weighting, rmode, robust, sowImageExt, imagingMode,
  //   cellSize, NX, nW, nIntDummy, nchanDummy, WBAwp, restartUI, doPointing,
  //   normalize, doPBCorr, conjBeams, pbLimit, posigdev,
  //   doSPWDataIter, msInflationFactor);
  bool const doSow = sowImageExt != "";
  // to prevent using the same image name by all ranks, we insert
  // substrings into the given image name that encode the rank number
  // on all ranks but the root
  string imageNamePrefix =
    imageName.substr(0, imageName.find_last_of("."));
  //  if (!isRoot)
    // {
    //   string imageNameSuffix =
    //     imageName.substr(imageName.find_last_of("."));
    //   imageName =
    //     imageNamePrefix + "." + std::to_string(rank) + imageNameSuffix;
    // }

  //
  // -------------------------------------- End of UI -------------------------------------------------------------------
  //

  int result = 0;
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

  //-------------------------------------------------------------------
  // Load the selected MS.  The original ms (thems), the selected
  // MS and the MSSelection objects are modified.  The selected
  // list of SPW and FIELD IDs are returned as a std::tuple.
  //
  MSSelection msSelection;
  MS theMS, selectedMS;
  //	String msname=MSNBuf;
  Vector<int> spwidList, fieldidList;
  Vector<double> spwRefFreqList;
  LogFilter filter(isRoot ? LogMessage::NORMAL : LogMessage::WARN);
  LogSink::globalSink().filter(filter);
  LogIO log_l(LogOrigin("roadrunner","main"));
  log_l << "Opening the MS (\"" << MSNBuf << "\"), applying data selection, "
        << "setting up data iterators...all that boring stuff."
        << LogIO::POST;
  auto lists = loadMS(MSNBuf,
                      spwStr,
                      fieldStr,
                      uvDistStr,
                      theMS,
                      selectedMS,
                      msSelection);
  spwidList=std::get<0>(lists);
  fieldidList=std::get<1>(lists);

  if (WBAwp && (spwidList.nelements() > 1) && (nW > 1)) // AW-Projection.
    doSPWDataIter=true;  // Allow user to override the decision

  //  log_l << "No. of rows selected: " << msInflationFactor * selectedMS.nrow() << LogIO::POST;
  log_l << "No. of rows selected: " << selectedMS.nrow() << LogIO::POST;
  MS subMS;
  // if (ranksPerMS > 1)
  //   {
  //     size_t const idx = rank % ranksPerMS;
  //     size_t const nRow = selectedMS.nrow();
  //     size_t const minRowPerRank = nRow / ranksPerMS;
  //     size_t const rem = nRow % ranksPerMS;
  //     size_t const offset = idx * minRowPerRank + std::min(idx, rem);
  //     size_t const maxRow = minRowPerRank + ((idx < rem) ? 1 : 0);
  //     subMS = selectedMS(TableExprNode(), maxRow, offset);
  //   }
  // else
    {
      subMS = selectedMS;
    }

  //
  //-------------------------------------------------------------------
  //

  //-------------------------------------------------------------------
  // Set up the data iterator
  //
  float timeSpan=10.0;
  Block<Int> sortCols;
  if (doSPWDataIter)
    {
      sortCols.resize(4);
      sortCols[0]=MS::ARRAY_ID;
      sortCols[1]=MS::DATA_DESC_ID; // Proxy for SPW (in most MEse anyway!)
      sortCols[2]=MS::FIELD_ID;
      sortCols[3]=MS::TIME;
      timeSpan=100000.0;
    }
  //
  // Construct the vis iterator Vi2.  Set up the frequency selection
  // in it.  Freq. selection is a two-part process.  The SPW level
  // selection is done at the MS level (i.e., the subMS has only the
  // selected SPWs in it).  The selection of frequency channels
  // *within* each selected SPW is done in the Vi2 below.
  //
  // The weight scheme is also set up in the Vi2 via the weightor()
  // call after making the empty sky images below.
  //
  vi2_g = new vi::VisibilityIterator2(subMS,vi::SortColumns(sortCols),true,0,timeSpan);

  // vi2_cfsrvr = new vi::VisibilityIterator2(subMS,vi::SortColumns(sortCols),true,0,timeSpan);
  // vb_cfsrvr=vi2_cfsrvr->getVisBuffer();
  // vi2_cfsrvr->originChunks();
  // vi2_cfsrvr->origin();
  // cerr << "First SPWID from vi2_cfsrvr: " << vb_cfsrvr->spectralWindows()(0) << endl;
  {
    Matrix<Double> freqSelection= msSelection.getChanFreqList(NULL,true);
    //      vi2_g.setInterval(50.0);

    FrequencySelectionUsingFrame fsel(MFrequency::Types::LSRK);
    for(unsigned i=0;i<freqSelection.shape()(0); i++)
      fsel.add(freqSelection(i,0), freqSelection(i,1), freqSelection(i,2));
    vi2_g->setFrequencySelection (fsel);
  }
  // 
  // Rebuild the spwList using the MS iterator.  This will get the actual SPW IDs in the MS,
  // rather than rely on getting this info. from the sub-tables (since the latter aren't always
  // made consistent with the main-table (this is not a deficiency of the MS structure)).  This
  // may be a tad slower in the absolute sense, specially for large monolith MSes.  In a relative
  // sense, its cost will remain insignificant for more imaging cases.
  //
  {
    log_l << "Getting SPW ID list using VI..." << LogIO::POST;
    std::vector<int> vb_SPWIDList;
    vb_g=vi2_g->getVisBuffer();
    vi2_g->originChunks();
    for (vi2_g->originChunks();vi2_g->moreChunks(); vi2_g->nextChunk())
      {
  	vi2_g->origin(); // So that the global vb is valid
  	vb_SPWIDList.push_back(vb_g->spectralWindows()(0));
      }

    log_l << "Done with SPWID list determination" << LogIO::POST;
    spwidList.resize(vb_SPWIDList.size());
    for(uint i=0;i<vb_SPWIDList.size();i++) spwidList[i] = vb_SPWIDList[i];
  }

  cerr << "Selected SPW ID list: " << spwidList << endl;
  // Global VB (seems to be needed for multi-threading)
  vb_g=vi2_g->getVisBuffer();

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

  PagedImage<Complex> cgrid=makeEmptySkyImage(*vi2_g, selectedMS, msSelection,
                                              cmplxGridName, startModelImageName,
                                              imSize, cellSize, phaseCenter,
                                              stokes, refFreqStr, mode);
  PagedImage<Float> skyImage(cgrid.shape(),cgrid.coordinates(), imageName);

  cgrid.table().markForDelete();

  // Setup the weighting scheme in the supplied VI2
  weightor(*vi2_g,
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
  if(vb_g->polarizationFrame()==MSIter::Linear) StokesImageUtil::changeCStokesRep(cgrid,StokesImageUtil::LINEAR);
  else StokesImageUtil::changeCStokesRep(cgrid, StokesImageUtil::CIRCULAR);

  //-------------------------------------------------------------------
  // Create the AWP FTMachine.  The AWProjectionFT is construed
  // with the re-sampler depending on the ftmName (AWVisResampler
  // for ftmName='awproject' or AWVisResamplerHPG for
  // ftmName='awphpg')
  //
  MPosition loc;
  MeasTable::Observatory(loc, MSColumns(subMS).observation().telescopeName()(0));
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
    mssFreqSel  = msSelection.getChanFreqList(NULL,true);
    // Send in Freq info.
    ftm_g->setSpwFreqSelection( mssFreqSel );

    doPSF=(ftm_g->ftmType()==casa::refim::FTMachine::PSF);
  }

  //-------------------------------------------------------------------
  // Iterate over the MS, trigger the gridding.
  //
  cgrid.set(Complex(0.0));
  Matrix<Float> weight;
  vi2_g->originChunks();
  vi2_g->origin();

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
    ftm_g->initializeToVis(cgrid,*vb_g);
  else
    ftm_g->initializeToSky(cgrid, weight, *vb_g);

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
    vi2_g->originChunks();
    vi2_g->origin();
    spwRefFreqList.assign(vb_g->subtableColumns().spectralWindow().refFrequency().getColumn());
    nDataPol  = vb_g->flagCube().shape()[0];
  }
  //
  // Finally, the data iteration loops.
  //-----------------------------------------------------------------------------------
  double griddingEngine_time=0;
  unsigned long vol=0;
  ProgressMeter pm(1.0, vi2_g->ms().nrow(),
		   "Gridding", "","","",true);
  DataIterator di(isRoot);
      
  if (ftm_g->name() != "AWProjectWBFTHPG")
    {
      // auto waitForCFReady = [](int&, int&){}; // NoOp
      // auto notifyCFSent   = [](const int&){}; //NoOp
      auto ret = di.dataIter(vi2_g, vb_g, ftm_g,doPSF,imagingMode);
      griddingEngine_time += std::get<2>(ret);
      vol+= std::get<1>(ret);
    }
  else // if (ftm_g->name()=="AWProjectWBFTHPG")
    {
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
			       std::ref(spwidList),
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
	[&thcoord, &spwidList, &visResampler](int& nVB, int& spwNdx)
	{
	  if (vb_g->spectralWindows()(0) != spwidList[spwNdx])
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

	      if (visResampler->set_cf(dcfa_sptr_g)==false)
		throw(AipsError("Device CFArray pointer is null in CFServer"));
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

      auto ret = di.dataIter(vi2_g, vb_g, ftm_g,doPSF,imagingMode,
			     waitForCFReady, notifyCFSent);

      griddingEngine_time += std::get<2>(ret);
      vol+= std::get<1>(ret);
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

  //detach the ms for cleaning up
  subMS = MeasurementSet();
  selectedMS = MeasurementSet();
  log_l << "...done" << LogIO::POST;
  tpl_finalize();
  return result;
}
