//# SynthesisDeconvolver.cc: Implementation of Imager.h
//# Copyright (C) 1997-2020
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

#include <casa/Exceptions/Error.h>
#include <casa/iostream.h>
#include <casa/sstream.h>

#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/ArrayLogical.h>

#include <casa/Logging.h>
#include <casa/Logging/LogIO.h>
#include <casa/Logging/LogMessage.h>
#include <casa/Logging/LogSink.h>
#include <casa/Logging/LogMessage.h>

#include <casa/OS/DirectoryIterator.h>
#include <casa/OS/File.h>
#include <casa/OS/Path.h>

#include <casa/OS/HostInfo.h>

#include <images/Images/TempImage.h>
#include <images/Images/SubImage.h>
#include <images/Regions/ImageRegion.h>
#include <lattices/Lattices/LatticeLocker.h>
#include <synthesis/ImagerObjects/CubeMinorCycleAlgorithm.h>
#include <imageanalysis/ImageAnalysis/CasaImageBeamSet.h>
#include <synthesis/ImagerObjects/SynthesisDeconvolver.h>
#include <synthesis/ImagerObjects/SIMinorCycleController.h>

#include <sys/types.h>
#include <unistd.h>
using namespace std;

using namespace casacore;
extern casa::Applicator casa::applicator;
namespace casa { //# NAMESPACE CASA - BEGIN

  
  SynthesisDeconvolver::SynthesisDeconvolver() : 
				       itsDeconvolver(nullptr), 
				       itsMaskHandler(nullptr ),
                                       itsImages(nullptr),
                                       itsImageName(""),
				       //                                       itsPartImageNames(Vector<String>(0)),
				       itsBeam(0.0),
				       itsDeconvolverId(0),
				       itsScales(Vector<Float>()),
                                       itsMaskType(""),
                                       itsPBMask(0.0),
				       //itsMaskString(String("")),
                                       itsIterDone(0.0),
                                       itsChanFlag(Vector<Bool>(0)),
                                       itsRobustStats(Record()),
                                       initializeChanMaskFlag(false),
                                       itsPosMask(nullptr),
				       itsIsMaskLoaded(false),
				       itsMaskSum(-1e+9),
				       itsPreviousFutureRes(0.0),
				       itsPreviousIterBotRec_p(Record())
  {
  }
  
  SynthesisDeconvolver::~SynthesisDeconvolver() 
  {
        LogIO os( LogOrigin("SynthesisDeconvolver","descructor",WHERE) );
	os << LogIO::DEBUG1 << "SynthesisDeconvolver destroyed" << LogIO::POST;
	SynthesisUtilMethods::getResource("End SynthesisDeconvolver");

  }

  void SynthesisDeconvolver::setupDeconvolution(const SynthesisParamsDeconv& decpars)
  {
    LogIO os( LogOrigin("SynthesisDeconvolver","setupDeconvolution",WHERE) );

    //Copy this decpars into a private variable that can be used elsewhere
    //there is no proper copy operator (as public casa::Arrays members = operator fails) 
    itsDecPars.fromRecord(decpars.toRecord());
    itsImageName = decpars.imageName;
    itsStartingModelNames = decpars.startModel;
    itsDeconvolverId = decpars.deconvolverId;
    
    os << "Set Deconvolution Options for [" << itsImageName << "] : " << decpars.algorithm ;
      if( itsStartingModelNames.nelements()>0 && itsStartingModelNames[0].length() > 0 ) 
      os << " , starting from model : " << itsStartingModelNames;
    os << LogIO::POST;
    
    try
      {
	if(decpars.algorithm==String("hogbom"))
	  {
	    itsDeconvolver.reset(new SDAlgorithmHogbomClean()); 
	  }
	else if(decpars.algorithm==String("mtmfs"))
	  {
	    itsDeconvolver.reset(new SDAlgorithmMSMFS( decpars.nTaylorTerms, decpars.scales, decpars.scalebias )); 
	  } 
	else if(decpars.algorithm==String("clark_exp"))
	  {
	    itsDeconvolver.reset(new SDAlgorithmClarkClean("clark")); 
	  } 
	else if(decpars.algorithm==String("clarkstokes_exp"))
	  {
	    itsDeconvolver.reset(new SDAlgorithmClarkClean("clarkstokes")); 
	  } 
	else if(decpars.algorithm==String("clark"))
	  {
	    itsDeconvolver.reset(new SDAlgorithmClarkClean2("clark")); 
	  } 
	else if(decpars.algorithm==String("clarkstokes"))
	  {
	    itsDeconvolver.reset(new SDAlgorithmClarkClean2("clarkstokes")); 
	  } 
	else if(decpars.algorithm==String("multiscale"))
	  {
	    itsDeconvolver.reset(new SDAlgorithmMSClean( decpars.scales, decpars.scalebias )); 
	  } 
	else if(decpars.algorithm==String("mem"))
	  {
	    itsDeconvolver.reset(new SDAlgorithmMEM( "entropy" )); 
	  } 
	else if (decpars.algorithm==String("aasp"))
	  {
	    itsDeconvolver.reset(new SDAlgorithmAAspClean());
	  }
	else
	  {
	    throw( AipsError("Un-known algorithm : "+decpars.algorithm) );
	  }

	// Set restoring beam options
	itsDeconvolver->setRestoringBeam( decpars.restoringbeam, decpars.usebeam );
        itsUseBeam = decpars.usebeam;// store this info also here.

	// Set Masking options
	//	itsDeconvolver->setMaskOptions( decpars.maskType );
	itsMaskHandler.reset(new SDMaskHandler());
	//itsMaskString = decpars.maskString;
	itsMaskType = decpars.maskType;
        if(itsMaskType=="auto-thresh") 
          {
            itsAutoMaskAlgorithm="thresh";
          }
        else if(itsMaskType=="auto-thresh2")
          {
            itsAutoMaskAlgorithm="thresh2";
          }
        else if(itsMaskType=="auto-multithresh")
          { 
            itsAutoMaskAlgorithm="multithresh";
          }
        else if(itsMaskType=="auto-onebox")
          {
            itsAutoMaskAlgorithm="onebox";
          }
        else {
            itsAutoMaskAlgorithm="";
        }
        itsPBMask = decpars.pbMask;
        itsMaskString = decpars.maskString;
        if(decpars.maskList.nelements()==0 || 
            (decpars.maskList.nelements()==1 && decpars.maskList[0]==""))
          {
            itsMaskList.resize(1);
            itsMaskList[0] = itsMaskString;
          }
        else 
          {
            itsMaskList = decpars.maskList;
          }
        itsMaskThreshold = decpars.maskThreshold;
        itsFracOfPeak = decpars.fracOfPeak;
        itsMaskResolution = decpars.maskResolution;
        itsMaskResByBeam = decpars.maskResByBeam;
        itsNMask = decpars.nMask;
        //itsAutoAdjust = decpars.autoAdjust;
        //desable autoadjust 
        itsAutoAdjust = false;
        itsSidelobeThreshold = decpars.sidelobeThreshold;
        itsNoiseThreshold = decpars.noiseThreshold;
        itsLowNoiseThreshold = decpars.lowNoiseThreshold;
        itsNegativeThreshold = decpars.negativeThreshold;
        itsSmoothFactor = decpars.smoothFactor;
        itsMinBeamFrac = decpars.minBeamFrac;
        itsCutThreshold = decpars.cutThreshold;
        itsGrowIterations = decpars.growIterations;
        itsDoGrowPrune = decpars.doGrowPrune;
        itsMinPercentChange = decpars.minPercentChange;
        itsVerbose = decpars.verbose;
        itsFastNoise = decpars.fastnoise;
	itsIsInteractive = decpars.interactive;
        itsNsigma = decpars.nsigma;
      }
    catch(AipsError &x)
      {
	throw( AipsError("Error in constructing a Deconvolver : "+x.getMesg()) );
      }
    
    itsAddedModel=false;
  }

  Long SynthesisDeconvolver::estimateRAM(const vector<int>& imsize){

    Long mem=0;
    /* This does not work
    if( ! itsImages )
      {
	itsImages = makeImageStore( itsImageName );
      }
    */
    if(itsDeconvolver)
      mem=itsDeconvolver->estimateRAM(imsize);
    return mem;
  }
  Record SynthesisDeconvolver::initMinorCycle(){
    /////IMPORTANT initMinorCycle has to be called before setupMask...that order has to be kept !

    
    if(!itsImages)
      itsImages=makeImageStore(itsImageName);
    //For cubes as we are not doing a post major cycle residual automasking
    //Force recalculation of robust stats to update nsigmathreshold with
    //most recent residual
   
    if(itsAutoMaskAlgorithm=="multithresh" && itsImages->residual()->shape()[3] >1 && itsNsigma > 0.0){
      Record retval;
      Record backupRobustStats=itsRobustStats;
      itsRobustStats=Record();
      retval=initMinorCycle(itsImages);
      itsRobustStats=backupRobustStats;
      return retval;
    }
    
    /* else if (itsAutoMaskAlgorithm=="multithresh" && itsImages->residual()->shape()[3]){
      ///As the automask for cubes pre-CAS-9386...
      /// was tuned to look for threshold in future mask
      ///It is as good as somewhere in between no mask and mask
      //      Record backupRobustStats=itsRobustStats;
      Record retval=initMinorCycle(itsImages);
      //cerr << "INITMINOR " << itsRobustStats << endl;
      //itsRobustStats=backupRobustStats;
      if(retval.isDefined("peakresidualnomask")){
	Float futureRes=Float(retval.asFloat("peakresidualnomask")-(retval.asFloat("peakresidualnomask")-retval.asFloat("peakresidual"))/1000.0);
	if(futureRes != itsPreviousFutureRes){
	  //itsLoopController.setPeakResidual(retval.asFloat("peakresidualnomask"));
	  retval.define("peakresidual", futureRes);
	  itsPreviousFutureRes=futureRes;
	}
      }
      return retval;
      }
    */
    Record retval= initMinorCycle(itsImages);
    //    cerr << "INITMINOR retval" << retval << endl;

    return retval;
  }
  Record SynthesisDeconvolver::initMinorCycle(std::shared_ptr<SIImageStore> imstor )
  { 
    LogIO os( LogOrigin("SynthesisDeconvolver","initMinorCycle",WHERE) );
    Record returnRecord;
    Timer timer;
    Timer tim;
    tim.mark();
    try {

      //os << "---------------------------------------------------- Init (?) Minor Cycles ---------------------------------------------" << LogIO::POST;

      itsImages = imstor;

      // If a starting model exists, this will initialize the ImageStore with it. Will do this only once.
      setStartingModel();

      //itsIterDone is currently only used by automask code so move this to inside setAutomask
      //itsIterDone += itsLoopController.getIterDone();

      //      setupMask();

      Float masksum;
      if( ! itsImages->hasMask() ) // i.e. if there is no existing mask to re-use...
	{ masksum = -1.0;}
      else 
	{
	  masksum = itsImages->getMaskSum();
	  itsImages->mask()->unlock();
	}
      Bool validMask = ( masksum > 0 );
      //    os << LogIO::NORMAL3 << "****INITMINOR Masksum stuff "<< tim.real() << LogIO::POST;
      // tim.mark();

      // Calculate Peak Residual and Max Psf Sidelobe, and fill into SubIterBot.
      Float peakresnomask = itsImages->getPeakResidual();
      Float peakresinmask= validMask ? itsImages->getPeakResidualWithinMask() : peakresnomask;
      //os << LogIO::NORMAL3 << "****INITMINOR residual peak "<< tim.real() << LogIO::POST;
      //tim.mark();
      itsLoopController.setPeakResidual( validMask ? peakresinmask : peakresnomask );
      //os << LogIO::NORMAL3 << "****INITMINOR OTHER residual peak "<< tim.real() << LogIO::POST;
      //tim.mark();
      itsLoopController.setPeakResidualNoMask( peakresnomask );
      itsLoopController.setMaxPsfSidelobe( itsImages->getPSFSidelobeLevel() );
      
      //re-calculate current nsigma threhold
      //os<<"Calling calcRobustRMS ....syndeconv."<<LogIO::POST;
      Float nsigmathresh = 0.0;
      Bool useautomask = ( itsAutoMaskAlgorithm=="multithresh" ? true : false);
      Int iterdone = itsLoopController.getIterDone();

      //cerr << "INIT automask " << useautomask << " alg " << itsAutoMaskAlgorithm << " sigma " << itsNsigma  << endl;
      if ( itsNsigma >0.0) { 
        itsMaskHandler->setPBMaskLevel(itsPBMask);
        Array<Double> medians, robustrms;
        // 2 cases to use existing stats.
        // 1. automask has run and so the image statistics record has filled
        // or
        // 2. no automask but for the first cycle but already initial calcRMS has ran to avoid duplicate
        //
        //cerr << "useauto " << useautomask << " nfields " << itsRobustStats.nfields() << " iterdone " << iterdone << endl;
        if ((useautomask && itsRobustStats.nfields()) || 
            (!useautomask && iterdone==0 && itsRobustStats.nfields()) ) {
           os <<LogIO::DEBUG1<<"automask on: check the current stats"<<LogIO::POST;
           //os<< "itsRobustStats nfield="<< itsRobustStats.nfields() << LogIO::POST;;
           if (itsRobustStats.isDefined("medabsdevmed")) {
             Array<Double> mads;
             itsRobustStats.get(RecordFieldId("medabsdevmed"), mads);
             os<<LogIO::DEBUG1<<"Using robust rms from automask ="<< mads*1.4826 <<LogIO::POST;
             robustrms = mads*1.4826;
           }
           else if(itsRobustStats.isDefined("robustrms")) {
             itsRobustStats.get(RecordFieldId("robustrms"), robustrms);
           }
           itsRobustStats.get(RecordFieldId("median"), medians);
              
        }
       else { // do own stats calculation
          timer.mark();
	  
          os<<LogIO::DEBUG1<<"Calling calcRobustRMS .. "<<LogIO::POST;
          robustrms = itsImages->calcRobustRMS(medians, itsPBMask, itsFastNoise);
          os<< LogIO::NORMAL << "time for calcRobustRMS:  real "<< timer.real() << "s ( user " << timer.user() 
             <<"s, system "<< timer.system() << "s)" << LogIO::POST;
          //reset itsRobustStats
          //cerr << "medians " << medians << " pbmask " << itsPBMask << endl;
          try {
            //os<<"current content of itsRobustStats nfields=="<<itsRobustStats.nfields()<<LogIO::POST;
            itsRobustStats.define(RecordFieldId("robustrms"), robustrms);
            itsRobustStats.define(RecordFieldId("median"), medians);
          }   
          catch(AipsError &x) { 
            throw( AipsError("Error in storing the robust image statistics") );
          }

	  //cerr << this << " DOING robust " << itsRobustStats << endl;
	  
       }
 
        /***
        Array<Double> robustrms =kitsImages->calcRobustRMS(medians, itsPBMask, itsFastNoise);
        // Before the first iteration the iteration parameters have not been
        // set in SIMinorCycleController
        // Use nsigma pass to SynthesisDeconvolver directly for now...
        //Float nsigma = itsLoopController.getNsigma();
        ***/
        Double minval, maxval;
        IPosition minpos, maxpos;
        //Double maxrobustrms = max(robustrms);
        minMax(minval, maxval, minpos, maxpos, robustrms);
        
        //Float nsigmathresh = nsigma * (Float)robustrms(IPosition(1,0));
        //nsigmathresh = itsNsigma * (Float)maxrobustrms;
        String msg="";
        if (useautomask) {
          nsigmathresh = (Float)(medians(maxpos)) + itsNsigma * (Float)maxval;
          msg+=" (nsigma*rms + median)";
        }
        else {
          nsigmathresh = itsNsigma * (Float)maxval;
        }
        os << "Current nsigma threshold (maximum along spectral channels ) ="<<nsigmathresh<< msg <<LogIO::POST;
      }
   
      itsLoopController.setNsigmaThreshold(nsigmathresh);
      itsLoopController.setPBMask(itsPBMask);


      if ( itsAutoMaskAlgorithm=="multithresh" && !initializeChanMaskFlag ) {
        IPosition maskshp = itsImages->mask()->shape();
        Int nchan = maskshp(3);
        itsChanFlag=Vector<Bool>(nchan,False);
        initializeChanMaskFlag=True;
        // also initialize posmask, which tracks only positive (emission)
        if(!itsPosMask){
          //itsPosMask = TempImage<Float> (maskshp, itsImages->mask()->coordinates(),SDMaskHandler::memoryToUse());
          itsPosMask=itsImages->tempworkimage();
	  //you don't want to modify this here...
	  //It is set to 0.0 in SIImageStore first time it is created.
          //itsPosMask->set(0);
          itsPosMask->unlock();
        }
      }
      os<<LogIO::DEBUG1<<"itsChanFlag.shape="<<itsChanFlag.shape()<<LogIO::POST;

      /*
      Array<Double> rmss = itsImages->calcRobustRMS();
      AlwaysAssert( rmss.shape()[0]>0 , AipsError);
      cout  << "madRMS : " << rmss << "  with shape : " << rmss.shape() << endl;
      //itsLoopController.setMadRMS( rmss[0] );
      */

      if( itsMaskSum != masksum || masksum == 0.0 ) // i.e. mask has changed. 
	{ 
	  itsMaskSum = masksum; 
	  itsLoopController.setMaskSum( masksum );
	}
      else // mask has not changed...
	{
	  itsLoopController.setMaskSum( -1.0 );
	}
      
      returnRecord = itsLoopController.getCycleInitializationRecord();
      //cerr << "INIT record " << returnRecord << endl;

      //      itsImages->printImageStats();
      os << " Absolute Peak residual within mask : " << peakresinmask << ", over full image : " << peakresnomask  << LogIO::POST;
      itsImages->releaseLocks();

      os << LogIO::DEBUG2 << "Initialized minor cycle. Returning returnRec" << LogIO::POST;

    } catch(AipsError &x) {
      throw( AipsError("Error initializing the Minor Cycle for "  + itsImageName + " : "+x.getMesg()) );
    }
    return returnRecord;
  }
  void SynthesisDeconvolver::setChanFlag(const Vector<Bool>& chanflag){
    //ignore if it has not been given a size yet in initminorcycle
    if(itsChanFlag.nelements()==0)
      return;
    if(itsChanFlag.nelements() != chanflag.nelements())
      throw(AipsError("cannot set chan flags for different number of channels"));
    itsChanFlag =chanflag;

  }
  Vector<Bool> SynthesisDeconvolver::getChanFlag(){
    return itsChanFlag;
  }
  void SynthesisDeconvolver::setRobustStats(const Record& rec){
    itsRobustStats=Record();
    itsRobustStats=rec;
    
  }
  Record SynthesisDeconvolver::getRobustStats(){
    return itsRobustStats;
  }

  Record SynthesisDeconvolver::interactiveGUI(Record& iterRec)
  {
    LogIO os( LogOrigin("SynthesisDeconvolver","interactiveGUI",WHERE) );
    Record returnRecord;
    
    try {
      
      // Check that required parameters are present in the iterRec.
      Int niter=0,cycleniter=0,iterdone;
      Float threshold=0.0, cyclethreshold=0.0;
      if( iterRec.isDefined("niter") &&  
	  iterRec.isDefined("cycleniter") &&  
	  iterRec.isDefined("threshold") && 
	  iterRec.isDefined("cyclethreshold") &&
	  iterRec.isDefined("iterdone") )
	{
	  iterRec.get("niter", niter);
	  iterRec.get("cycleniter", cycleniter);
	  iterRec.get("threshold", threshold);
	  iterRec.get("cyclethreshold", cyclethreshold);
	  iterRec.get("iterdone",iterdone);
	}
      else throw(AipsError("SD::interactiveGui() needs valid niter, cycleniter, threshold to start up."));
      
      if( ! itsImages ) itsImages = makeImageStore( itsImageName );
      
      //      SDMaskHandler masker;
      String strthresh = String::toString(threshold)+"Jy";
      String strcycthresh = String::toString(cyclethreshold)+"Jy";

      if( itsMaskString.length()>0 ) {
	  itsMaskHandler->fillMask( itsImages, itsMaskString );
      }
      
      Int iterleft = niter - iterdone;
      if( iterleft<0 ) iterleft=0;
      
      Int stopcode = itsMaskHandler->makeInteractiveMask( itsImages, iterleft, cycleniter, strthresh, strcycthresh );
      
      Quantity qa;
      casacore::Quantity::read(qa,strthresh);
      threshold = qa.getValue(Unit("Jy"));
      casacore::Quantity::read(qa,strcycthresh);
      cyclethreshold = qa.getValue(Unit("Jy"));
      
      itsIsMaskLoaded=true;

      returnRecord.define( RecordFieldId("actioncode"), stopcode );
      returnRecord.define( RecordFieldId("niter"), iterdone + iterleft );
      returnRecord.define( RecordFieldId("cycleniter"), cycleniter );
      returnRecord.define( RecordFieldId("threshold"), threshold );
      returnRecord.define( RecordFieldId("cyclethreshold"), cyclethreshold );

    } catch(AipsError &x) {
      throw( AipsError("Error in Interactive GUI : "+x.getMesg()) );
    }
    return returnRecord;
  }

  void SynthesisDeconvolver::setMinorCycleControl(const Record& minorCycleControlRec){
    //Don't know what itsloopcontroller does not need a const record;
    Record lala=minorCycleControlRec;
    itsLoopController.setCycleControls(lala);

  }
  

  Record SynthesisDeconvolver::executeMinorCycle(Record& minorCycleControlRec)
  {
    // LogIO os( LogOrigin("SynthesisDeconvolver","executeMinorCycle",WHERE) );
    

    //    itsImages->printImageStats();
    SynthesisUtilMethods::getResource("Start Deconvolver");
    ///if cube execute cube deconvolution...check on residual shape as itsimagestore return 0 shape sometimes
    if(!itsImages)
      throw(AipsError("Initminor Cycle has not been called yet"));
    if(itsImages->residual()->shape()[3]> 1){
     return  executeCubeMinorCycle(minorCycleControlRec);
    }
     
    //  os << "---------------------------------------------------- Run Minor Cycle Iterations  ---------------------------------------------" << LogIO::POST;
    return executeCoreMinorCycle(minorCycleControlRec);
    SynthesisUtilMethods::getResource("End Deconvolver");
  }
  Record SynthesisDeconvolver::executeCoreMinorCycle(Record& minorCycleControlRec)
  {
   
    Record returnRecord;
    try {
      //      if ( !itsIsInteractive ) setAutoMask();
      //cerr << "MINORCYCLE control Rec " << minorCycleControlRec << endl;
      itsLoopController.setCycleControls(minorCycleControlRec);
      bool automaskon (false);
      if (itsAutoMaskAlgorithm=="multithresh") {
        automaskon=true;
      }
      //itsDeconvolver->deconvolve( itsLoopController, itsImages, itsDeconvolverId, automaskon, itsFastNoise );
      // include robust stats rec
      
      itsDeconvolver->deconvolve( itsLoopController, itsImages, itsDeconvolverId, automaskon, itsFastNoise, itsRobustStats );
      
      returnRecord = itsLoopController.getCycleExecutionRecord();

      //scatterModel(); // This is a no-op for the single-node case.

      itsImages->releaseLocks();

    } catch(AipsError &x) {
      throw( AipsError("Error in running Minor Cycle : "+x.getMesg()) );
    }

   

    return returnRecord;
  }
  Record SynthesisDeconvolver::executeCubeMinorCycle(Record& minorCycleControlRec, const Int AutoMaskFlag)
  {
        LogIO os( LogOrigin("SynthesisDeconvolver","executeCubeMinorCycle",WHERE) );
    Record returnRecord;
    Int doAutoMask=AutoMaskFlag;

    SynthesisUtilMethods::getResource("Start Deconvolver");

    try {
      //      if ( !itsIsInteractive ) setAutoMask();
      if(doAutoMask < 1){
	itsLoopController.setCycleControls(minorCycleControlRec);
      }
      else if(doAutoMask==1){
	minorCycleControlRec=itsPreviousIterBotRec_p;
      }
      CubeMinorCycleAlgorithm cmc;
      //casa::applicator.defineAlgorithm(cmc);
      ///argv and argc are needed just to callthe right overloaded init
      Int argc=1;
      char **argv=nullptr;
      applicator.init(argc, argv);
      if(applicator.isController()){
        os << ((AutoMaskFlag != 1) ? "---------------------------------------------------- Run Minor Cycle Iterations  ---------------------------------------------" : "---------------------------------------------------- Run Automask  ---------------------------------------------" )<< LogIO::POST;
        /*{///TO BE REMOVED
          LatticeExprNode le( sum( *(itsImages->mask()) ) );
          os << LogIO::WARN << "#####Sum of mask BEFORE minor cycle " << le.getFloat() << endl;
            }
        */
        Timer tim;
        tim.mark();
        //itsImages->printImageStats();
        // Add itsIterdone to be sent to child processes ...needed for automask
        //cerr << "before record " << itsIterDone << " loopcontroller " << itsLoopController.getIterDone() << endl;
        minorCycleControlRec.define("iterdone", itsIterDone);
	if(doAutoMask < 0) // && itsPreviousIterBotRec_p.nfields() >0)
	  doAutoMask=0;
	minorCycleControlRec.define("onlyautomask",doAutoMask);
        if(itsPosMask){
          minorCycleControlRec.define("posmaskname", itsPosMask->name());
        }
	//Int numprocs = applicator.numProcs(); 
        //cerr << "Number of procs: " << numprocs << endl;
          
          Int numchan=itsImages->residual()->shape()[3];
          Vector<Int> startchans;
          Vector<Int> endchans;
          Int numblocks=numblockchans(startchans, endchans); 
          String psfname=itsImages->psf()->name();
          
          Float psfsidelobelevel=itsImages->getPSFSidelobeLevel();
          String residualname=itsImages->residual()->name();
          String maskname=itsImages->mask()->name();
          String modelname=itsImages->model()->name();
          ////Need the pb too as calcrobustrms in SynthesisDeconvolver uses it
          String pbname="";
          if(itsImages->hasPB())
            pbname=itsImages->pb()->name();
          itsImages->releaseLocks();
          ///in lieu of = operator go via record
          // need to create a proper = operator for SynthesisParamsDeconv
          SynthesisParamsDeconv decpars;
          ///Will have to create a = operator...right now kludging
          ///from record has a check that has to be bypassed for just the
          /// usage as a = operator
          {
            String tempMaskString= itsDecPars.maskString;
            itsDecPars.maskString="";
            decpars.fromRecord(itsDecPars.toRecord());
            //return itsDecPars back to its original state
            itsDecPars.maskString=tempMaskString;
          }
          ///remove starting model as already dealt with in this deconvolver
          decpars.startModel="";
          ///masking is dealt already by this deconvolver so mask image
          //is all that is needed which is sent as maskname to subdeconvolver
          decpars.maskString="";
          (decpars.maskList).resize();
          Record decParsRec = decpars.toRecord();

          /////Now we loop over channels and deconvolve each
          ///If we do want to do block of channels rather than 1 channel
          ///at a time chanRange can take that and the trigger to call this
          ///function in executeMinorCycle has to change.
          Int rank(0);
          Bool assigned; 
          Bool allDone(false);
          Vector<Int> chanRange(2);
          //Record beamsetRec;
          Vector<Bool> retvals(numblocks, False);
          Vector<Bool> chanFlag(0);
          if(itsChanFlag.nelements()==0){
            itsChanFlag.resize(numchan);
            itsChanFlag.set(False);
          }
	  Record chanflagRec;
          Int indexofretval=0;
          for (Int k=0; k < numblocks; ++k) {
            //os << LogIO::DEBUG1 << "deconvolving channel "<< k << LogIO::POST;
            assigned=casa::applicator.nextAvailProcess(cmc, rank);
            //cerr << "assigned "<< assigned << endl;
            while(!assigned) {
              //cerr << "SErial ? " << casa::applicator.isSerial() << endl;
              rank = casa::applicator.nextProcessDone(cmc, allDone);
              //cerr << "while rank " << rank << endl;
              //receiving output of CubeMinorCycleAlgorithm::put
              //#1
              Vector<Int> chanRangeProcessed;
              casa::applicator.get(chanRangeProcessed);
              //#2
              
              Record chanflagRec;
	      casa::applicator.get(chanflagRec);
               
              //#3
              Record retval;
              casa::applicator.get(retval);
	      
	      Vector<Bool> retchanflag;
	      chanflagRec.get("chanflag", retchanflag);
	      if(retchanflag.nelements() >0)
		itsChanFlag(Slice(chanRangeProcessed[0], chanRangeProcessed[1]-chanRangeProcessed[0]+1))=retchanflag;
	      Record substats=chanflagRec.asRecord("statsrec");
	      setSubsetRobustStats(substats, chanRangeProcessed[0], chanRangeProcessed[1], numchan);
	      
              retvals(indexofretval)=(retval.nfields() > 0);
              ++indexofretval;
              ///might need to merge these retval
	      if(doAutoMask <1)
		mergeReturnRecord(retval, returnRecord, chanRangeProcessed[0]);
              /*if(retval.nfields())
                cerr << k << "deconv rank " << rank << " successful " << endl;
               else
                cerr << k << "deconv rank " << rank << " failed " << endl;
              */
              //cerr <<"rank " << rank << " return rec "<< retval << endl;
              assigned = casa::applicator.nextAvailProcess(cmc, rank);
	  
            }

            ///send process info
            // put dec sel params #1
            applicator.put(decParsRec);
            // put itercontrol  params #2
            applicator.put(minorCycleControlRec);
            // put which channel to process #3
            chanRange[0]=startchans[k];  chanRange[1]=endchans[k];
            applicator.put(chanRange);
            // psf  #4
            applicator.put(psfname);
            // residual #5
            applicator.put(residualname);
            // model #6
            applicator.put(modelname);
            // mask #7
            applicator.put(maskname);
            //pb #8
            applicator.put(pbname);
            //#9 psf side lobe
            applicator.put(psfsidelobelevel);
            //# put chanflag
            chanFlag.resize();
            chanFlag=itsChanFlag(IPosition(1, chanRange[0]), IPosition(1, chanRange[1]));
           
            chanflagRec.define("chanflag", chanFlag);
	    Record statrec=getSubsetRobustStats(chanRange[0], chanRange[1]);
	    chanflagRec.defineRecord("statsrec", statrec);
            applicator.put(chanflagRec);
            /// Tell worker to process it
            applicator.apply(cmc);
            
          }
          // Wait for all outstanding processes to return
          rank = casa::applicator.nextProcessDone(cmc, allDone);
          while (!allDone) {

            Vector<Int> chanRangeProcessed;
            casa::applicator.get(chanRangeProcessed);
            Record chanflagRec;
            casa::applicator.get(chanflagRec);       	    
            Record retval;
            casa::applicator.get(retval);
	    Vector<Bool> retchanflag;
	    chanflagRec.get("chanflag", retchanflag);
	    if(retchanflag.nelements() >0)
              itsChanFlag(Slice(chanRangeProcessed[0], chanRangeProcessed[1]-chanRangeProcessed[0]+1))=retchanflag;
	    Record substats=chanflagRec.asRecord("statsrec");
	    setSubsetRobustStats(substats, chanRangeProcessed[0], chanRangeProcessed[1], numchan);
            retvals(indexofretval)=(retval.nfields() > 0);
            ++indexofretval;
	    if(doAutoMask < 1)
	      mergeReturnRecord(retval, returnRecord, chanRangeProcessed[0]);
            if(retval.nfields() >0)
              //cerr << "deconv remainder rank " << rank << " successful " << endl;
              cerr << "";
            else
              cerr << "deconv remainder rank " << rank << " failed " << endl;
            
            rank = casa::applicator.nextProcessDone(cmc, allDone);
            if(casa::applicator.isSerial())
              allDone=true;

          }

          if(anyEQ(retvals, False))
            throw(AipsError("one of more section of the cube failed in deconvolution"));
	  if(doAutoMask < 1){
	    itsLoopController.incrementMinorCycleCount(returnRecord.asInt("iterdone"));
	    itsIterDone+=returnRecord.asInt("iterdone");
	  }
	  itsPreviousIterBotRec_p=Record();
	  itsPreviousIterBotRec_p=minorCycleControlRec;
          /*{///TO BE REMOVED
          LatticeExprNode le( sum( *(itsImages->mask()) ) );
          os << LogIO::WARN << "#####Sum of mask AFTER minor cycle " << le.getFloat()  << "loopcontroller iterdeconv " << itsLoopController.getIterDone() << endl;
          }*/ 

      }///end of if controller
      /////////////////////////////////////////////////
  
      //scatterModel(); // This is a no-op for the single-node case.

      itsImages->releaseLocks();

    } catch(AipsError &x) {
      //MPI_Abort(MPI_COMM_WORLD, 6);
      throw( AipsError("Error in running Minor Cycle : "+x.getMesg()) );
    }

    SynthesisUtilMethods::getResource("End Deconvolver");

    return returnRecord;
  }

  // Restore Image.
  void SynthesisDeconvolver::restore()
  {
    LogIO os( LogOrigin("SynthesisDeconvolver","restoreImage",WHERE) );

    if( ! itsImages )
      {
	itsImages = makeImageStore( itsImageName );
      }

    SynthesisUtilMethods::getResource("Restoration");

    itsDeconvolver->restore(itsImages);
    itsImages->releaseLocks();

  }

  void SynthesisDeconvolver::mergeReturnRecord(const Record& inRec, Record& outRec, const Int chan){

    ///Something has to be done about what is done in SIIterBot_state::mergeMinorCycleSummary if it is needed
    Matrix<Double> summaryminor(SIMinorCycleController::nSummaryFields,0);
    if(outRec.isDefined("summaryminor"))
      summaryminor=Matrix<Double>(outRec.asArrayDouble("summaryminor"));
    Matrix<Double> subsummaryminor;
    if(inRec.isDefined("summaryminor"))
      subsummaryminor=Matrix<Double>(inRec.asArrayDouble("summaryminor"));
    if(subsummaryminor.nelements() !=0){
      ///The 6th element is supposed to be the subimage id
      subsummaryminor.row(5)= subsummaryminor.row(5)+(Double(chan));
      Matrix<Double> newsummary(SIMinorCycleController::nSummaryFields, summaryminor.shape()[1]+subsummaryminor.shape()[1]);
      Int ocol=0;
      for (Int col=0; col< summaryminor.shape()[1]; ++col, ++ocol)
        newsummary.column(ocol)=summaryminor.column(col);
      for (Int col=0; col< subsummaryminor.shape()[1]; ++col, ++ocol)
        newsummary.column(ocol)=subsummaryminor.column(col);
      summaryminor.resize(newsummary.shape());
      summaryminor=newsummary;
    }
    outRec.define("summaryminor", summaryminor);
    //cerr << "inRec summ minor " << inRec.asArrayDouble("summaryminor") << endl;
    //cerr << "outRec summ minor " << summaryminor << endl;
    outRec.define("iterdone", Int(inRec.asInt("iterdone")+ (outRec.isDefined("iterdone") ? outRec.asInt("iterdone"): Int(0))));
    outRec.define("maxcycleiterdone", outRec.isDefined("maxcycleiterdone") ? max(inRec.asInt("maxcycleiterdone"), outRec.asInt("maxcycleiterdone")) :inRec.asInt("maxcycleiterdone")) ;
    
    outRec.define("peakresidual", outRec.isDefined("peakresidual") ? max(inRec.asFloat("peakresidual"), outRec.asFloat("peakresidual")) :inRec.asFloat("peakresidual")) ;

    ///is not necessarily defined it seems
    Bool updatedmodelflag=False;
    if(inRec.isDefined("updatedmodelflag"))
      inRec.get("updatedmodelflag", updatedmodelflag);
    outRec.define("updatedmodelflag", outRec.isDefined("updatedmodelflag") ? updatedmodelflag || outRec.asBool("updatedmodelflag") : updatedmodelflag) ;



  }
  // get channel blocks
  Int SynthesisDeconvolver::numblockchans(Vector<Int>& startchans, Vector<Int>& endchans){
    Int nchan=itsImages->residual()->shape()[3];
    //roughly 8e6 pixel to deconvolve per lock/process is a  minimum
    Int optchan= 8e6/(itsImages->residual()->shape()[0])/(itsImages->residual()->shape()[1]);
    // cerr << "OPTCHAN" << optchan  << endl;
    if(optchan < 10) optchan=10;
    Int nproc= applicator.numProcs() < 2 ? 1 : applicator.numProcs()-1;
    /*if(nproc==1){
      startchans.resize(1);
      endchans.resize(1);
      startchans[0]=0;
      endchans[0]=nchan-1;
      return 1;
      }
    */
    Int blksize= nchan/nproc > optchan ? optchan : Int( std::floor(Float(nchan)/Float(nproc)));
    if(blksize< 1) blksize=1;
    Int nblk=Int(nchan/blksize);
    startchans.resize(nblk);
    endchans.resize(nblk);
    for (Int k=0; k < nblk; ++k){
      startchans[k]= k*blksize;
      endchans[k]=(k+1)*blksize-1;
    }
    if(endchans[nblk-1] < (nchan-1)){
      startchans.resize(nblk+1,True);
      startchans[nblk]=endchans[nblk-1]+1;
      endchans.resize(nblk+1,True);
      endchans[nblk]=nchan-1;
      ++nblk;
    }
    //cerr << "nblk " << nblk << " beg " << startchans << " end " << endchans << endl;
    return nblk;
  }
  
  // pbcor Image.
  void SynthesisDeconvolver::pbcor()
  {
    LogIO os( LogOrigin("SynthesisDeconvolver","pbcor",WHERE) );

    if( ! itsImages )
      {
	itsImages = makeImageStore( itsImageName );
      }

    itsDeconvolver->pbcor(itsImages);
    itsImages->releaseLocks();

  }



  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ////    Internal Functions start here.  These are not visible to the tool layer.
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  std::shared_ptr<SIImageStore> SynthesisDeconvolver::makeImageStore( String imagename )
  {
    std::shared_ptr<SIImageStore> imstore;
    if( itsDeconvolver->getAlgorithmName() == "mtmfs" )
      {  imstore.reset( new SIImageStoreMultiTerm( imagename, itsDeconvolver->getNTaylorTerms(), true ) ); }
    else
      {  imstore.reset( new SIImageStore( imagename, true ) ); }

    return imstore;

  }


  // #############################################
  // #############################################
  // #############################################
  // #############################################

  // Set a starting model.
  void SynthesisDeconvolver::setStartingModel()
  {
    LogIO os( LogOrigin("SynthesisDeconvolver","setStartingModel",WHERE) );

    if(itsAddedModel==true) {return;}
    
    try
      {
	
	if( itsStartingModelNames.nelements()>0 && itsImages )
	  {
	    //	    os << "Setting " << itsStartingModelNames << " as starting model for deconvolution " << LogIO::POST;
	    itsImages->setModelImage( itsStartingModelNames );
	  }

	itsAddedModel=true;
	
      }
    catch(AipsError &x)
      {
	throw( AipsError("Error in setting  starting model for deconvolution: "+x.getMesg()) );
      }

  }

  // Set mask
  Bool SynthesisDeconvolver::setupMask()
  {

    ////Remembet this has to be called only after initMinorCycle
    LogIO os( LogOrigin("SynthesisDeconvolver","setupMask",WHERE) );
    if(!itsImages)
      throw(AipsError("Initminor Cycle has not been called yet"));
    Bool maskchanged=False;
    //debug
    if( itsIsMaskLoaded==false ) {
      // use mask(s) 
      if(  itsMaskList[0] != "" || itsMaskType == "pb" || itsAutoMaskAlgorithm != "" ) {
        // Skip automask for non-interactive mode. 
        if ( itsAutoMaskAlgorithm != "") { // && itsIsInteractive) {
	  os << "[" << itsImages->getName() << "] Setting up an auto-mask"<<  ((itsPBMask>0.0)?" within PB mask limit ":"") << LogIO::POST;
          ////For Cubes this is done in CubeMinorCycle
	  //cerr << this << "SETUP mask " << itsRobustStats << endl;
          if(itsImages->residual()->shape()[3] ==1)
            setAutoMask();
	  else if((itsImages->residual()->shape()[3] >1)){
	    Record dummy;
	    executeCubeMinorCycle(dummy, 1);
	  }
          /***
          if ( itsPBMask > 0.0 ) {
            itsMaskHandler->autoMaskWithinPB( itsImages, itsAutoMaskAlgorithm, itsMaskThreshold, itsFracOfPeak, itsMaskResolution, itsMaskResByBeam, itsPBMask);
          }
          else { 
            cerr<<"automask by automask.."<<endl;
            itsMaskHandler->autoMask( itsImages, itsAutoMaskAlgorithm, itsMaskThreshold, itsFracOfPeak, itsMaskResolution, itsMaskResByBeam);
          }
          ***/
        }
        //else if( itsMaskType=="user" && itsMaskList[0] != "" ) {
        if( itsMaskType=="user" && itsMaskList[0] != "" ) {
	  os << "[" << itsImages->getName() << "] Setting up a mask from " << itsMaskList  <<  ((itsPBMask>0.0)?" within PB mask limit ":"") << LogIO::POST;

          itsMaskHandler->fillMask( itsImages, itsMaskList);
          if( itsPBMask > 0.0 ) {  
            itsMaskHandler->makePBMask(itsImages, itsPBMask, True);
          }
        }
        else if( itsMaskType=="pb") {
	  os << "[" << itsImages->getName() << "] Setting up a PB based mask" << LogIO::POST;
          itsMaskHandler->makePBMask(itsImages, itsPBMask);
        }

	os << "----------------------------------------------------------------------------------------------------------------------------------------" << LogIO::POST;

      } else {

        // new im statistics creates an empty mask and need to take care of that case 
        Bool emptyMask(False);
        if( itsImages->hasMask() ) 
          {
            if (itsImages->getMaskSum()==0.0) {
              emptyMask=True;
            }
          }
	if( ! itsImages->hasMask() || emptyMask ) // i.e. if there is no existing mask to re-use...
	  {
            LatticeLocker lock1 (*(itsImages->mask()), FileLocker::Write);
	    if( itsIsInteractive ) itsImages->mask()->set(0.0);
	    else itsImages->mask()->set(1.0);
	    os << "[" << itsImages->getName() << "] Initializing new mask to " << (itsIsInteractive?"0.0 for interactive drawing":"1.0 for the full image") << LogIO::POST;
	  }
	else {
	  os << "[" << itsImages->getName() << "] Initializing to existing mask" << LogIO::POST;
	}

      }
	
      // If anything other than automasking, don't re-make the mask here.
      if ( itsAutoMaskAlgorithm == "" )
        {	itsIsMaskLoaded=true; }
   

      // Get the number of mask pixels (sum) and send to the logger.
      Float masksum = itsImages->getMaskSum();
      Float npix = (itsImages->getShape()).product();

      //Int npix2 = 20000*20000*16000*4;
      //Float npix2f = 20000*20000*16000*4;

      //cout << " bigval : " << npix2 << " and " << npix2f << endl;

      os << "[" << itsImages->getName() << "] Number of pixels in the clean mask : " << masksum << " out of a total of " << npix << " pixels. [ " << 100.0 * masksum/npix << " % ]" << LogIO::POST;

      maskchanged=True;
    }
    else {
    }

    itsImages->mask()->unlock();
    return maskchanged;
}

  void SynthesisDeconvolver::setAutoMask()
  {
     //modify mask using automask otherwise no-op
     if ( itsAutoMaskAlgorithm != "" )  {
       itsIterDone += itsLoopController.getIterDone();
       
      
       
       Bool isThresholdReached = itsLoopController.isThresholdReached();
             //cerr << this << " setAuto " << itsRobustStats << endl;
       LogIO os( LogOrigin("SynthesisDeconvolver","setAutoMask",WHERE) );
       os << "Generating AutoMask" << LogIO::POST;
       //os << LogIO::WARN << "#####ItsIterDone value " << itsIterDone << endl;
       
       //os << "itsMinPercentChnage = " << itsMinPercentChange<< LogIO::POST;
       //cerr << "SUMa of chanFlag before " << ntrue(itsChanFlag) << endl;
       if ( itsPBMask > 0.0 ) {
         //itsMaskHandler->autoMaskWithinPB( itsImages, itsPosMask, itsIterDone, itsChanFlag, itsAutoMaskAlgorithm, itsMaskThreshold, itsFracOfPeak, itsMaskResolution, itsMaskResByBeam, itsNMask, itsAutoAdjust,  itsSidelobeThreshold, itsNoiseThreshold, itsLowNoiseThreshold, itsNegativeThreshold,itsCutThreshold, itsSmoothFactor, itsMinBeamFrac, itsGrowIterations, itsDoGrowPrune, itsMinPercentChange, itsVerbose, itsFastNoise, isThresholdReached, itsPBMask);
         //pass robust stats
         itsMaskHandler->autoMaskWithinPB( itsImages, *itsPosMask, itsIterDone, itsChanFlag, itsRobustStats, itsAutoMaskAlgorithm, itsMaskThreshold, itsFracOfPeak, itsMaskResolution, itsMaskResByBeam, itsNMask, itsAutoAdjust,  itsSidelobeThreshold, itsNoiseThreshold, itsLowNoiseThreshold, itsNegativeThreshold,itsCutThreshold, itsSmoothFactor, itsMinBeamFrac, itsGrowIterations, itsDoGrowPrune, itsMinPercentChange, itsVerbose, itsFastNoise, isThresholdReached, itsPBMask);
       }
       else {
         //itsMaskHandler->autoMask( itsImages, itsPosMask, itsIterDone, itsChanFlag,itsAutoMaskAlgorithm, itsMaskThreshold, itsFracOfPeak, itsMaskResolution, itsMaskResByBeam, itsNMask, itsAutoAdjust, itsSidelobeThreshold, itsNoiseThreshold, itsLowNoiseThreshold, itsNegativeThreshold, itsCutThreshold, itsSmoothFactor, itsMinBeamFrac, itsGrowIterations, itsDoGrowPrune, itsMinPercentChange, itsVerbose, itsFastNoise, isThresholdReached );
        // pass robust stats 
        itsMaskHandler->autoMask( itsImages, *itsPosMask, itsIterDone, itsChanFlag, itsRobustStats, itsAutoMaskAlgorithm, itsMaskThreshold, itsFracOfPeak, itsMaskResolution, itsMaskResByBeam, itsNMask, itsAutoAdjust, itsSidelobeThreshold, itsNoiseThreshold, itsLowNoiseThreshold, itsNegativeThreshold, itsCutThreshold, itsSmoothFactor, itsMinBeamFrac, itsGrowIterations, itsDoGrowPrune, itsMinPercentChange, itsVerbose, itsFastNoise, isThresholdReached );
       }
       //cerr <<this << " SETAutoMask " << itsRobustStats << endl;
       //cerr << "SUM of chanFlag AFTER " << ntrue(itsChanFlag) << endl;
     }
  }

  // check if restoring beam is reasonable 
  void SynthesisDeconvolver::checkRestoringBeam() 
  {
    LogIO os( LogOrigin("SynthesisDeconvolver","checkRestoringBeam",WHERE) );
    //check for a bad restoring beam
    GaussianBeam beam;
    
    if( ! itsImages ) itsImages = makeImageStore( itsImageName );
    ImageInfo psfInfo = itsImages->psf()->imageInfo();
    if (psfInfo.hasSingleBeam()) {
      beam = psfInfo.restoringBeam();  
    }
    else if (psfInfo.hasMultipleBeams() && itsUseBeam=="common") {
      beam = CasaImageBeamSet(psfInfo.getBeamSet()).getCommonBeam(); 
    }
    Double beammaj = beam.getMajor(Unit("arcsec"));
    Double beammin = beam.getMinor(Unit("arcsec"));
    if (std::isinf(beammaj) || std::isinf(beammin)) {
      String msg;
      if (itsUseBeam=="common") {
        msg+="Bad restoring beam using the common beam (at least one of the beam axes has an infinite size)  ";
        throw(AipsError(msg));
      }
    }
    itsImages->releaseLocks();
  }
    
  // This is for interactive-clean.
  void SynthesisDeconvolver::getCopyOfResidualAndMask( TempImage<Float> &/*residual*/,
                                           TempImage<Float> &/*mask*/ )
  {
    // Actually all I think we need here are filenames JSK 12/12/12
    // resize/shape and copy the residual image and mask image to these in/out variables.
    // Allocate Memory here.
  }
  void SynthesisDeconvolver::setMask( TempImage<Float> &/*mask*/ )
  {
    // Here we will just pass in the new names
    // Copy the input mask to the local main image mask
  }
  void SynthesisDeconvolver::setIterDone(const Int iterdone){
    //cerr << "SETITERDONE iterdone " << iterdone << endl;
    ///this get lost in initMinorCycle
    //itsLoopController.incrementMinorCycleCount(iterdone);
    itsIterDone=iterdone;
    
  }
  void SynthesisDeconvolver::setPosMask(std::shared_ptr<ImageInterface<Float> > posmask){
    itsPosMask=posmask;									      
									      
  }

  auto key2Mat = [](const Record& rec, const String& key, const Int npol) {
     Matrix<Double> tmp;
     //cerr << "KEY2mat " << key <<"  "<< rec.asArrayDouble(key).shape() << endl;
     if(rec.asArrayDouble(key).shape().nelements()==1){
       if(rec.asArrayDouble(key).shape()[0] != npol){
	 tmp.resize(1,rec.asArrayDouble(key).shape()[0]);
	 Vector<Double>tmpvec=rec.asArrayDouble(key);
	 tmp.row(0)=tmpvec;
       }
       else{
	 tmp.resize(rec.asArrayDouble(key).shape()[0],1);
	 Vector<Double>tmpvec=rec.asArrayDouble(key);
	 tmp.column(0)=tmpvec;
       }
	 
     }
     else{
       tmp=rec.asArrayDouble(key);
     }
     return tmp;
   };
  
  Record SynthesisDeconvolver::getSubsetRobustStats(const Int chanBeg, const Int chanEnd){
    Record outRec;
    //cerr << "getSUB " << itsRobustStats << endl;
    if(itsRobustStats.nfields()==0)
      return outRec;
    Matrix<Double> tmp;
    std::vector<String> keys={"min", "max", "rms", "medabsdevmed", "med", "robustrms", "median"};
    for (auto it = keys.begin() ; it != keys.end(); ++it){
      if(itsRobustStats.isDefined(*it)){
	tmp.resize();
	tmp=key2Mat(itsRobustStats, *it, itsImages->residual()->shape()[2]);
	/*
	cerr << "size of " << *it << "   " << itsRobustStats.asArrayDouble(*it).shape() << endl;
	if(itsRobustStats.asArrayDouble(*it).shape().nelements()==1){
	  tmp.resize(1, itsRobustStats.asArrayDouble(*it).shape()[0]);
	  Vector<Double>tmpvec=itsRobustStats.asArrayDouble(*it);
	  tmp.row(0)=tmpvec;

	}
	else
	  tmp=itsRobustStats.asArrayDouble(*it);
	*/
	//	cerr << std::setprecision(12) << tmp[chanBeg] << " bool " <<(tmp[chanBeg]> (C::dbl_max-(C::dbl_max*1e-15))) << endl;
	if(tmp(0,chanBeg)> (C::dbl_max-(C::dbl_max*1e-15)))
	  return Record();
	//cerr << "GETSUB blc "<< IPosition(2, 0, chanBeg)<<  " trc " << IPosition(2, tmp.shape()[0]-1, chanEnd) << " shape " << tmp.shape() << endl;
	outRec.define(*it, tmp(IPosition(2, 0, chanBeg), IPosition(2, tmp.shape()[0]-1, chanEnd)));     
      }
    }
    //cerr <<"chanbeg " << chanBeg << " chanend " << chanEnd << endl;
    //cerr << "GETSUB " << outRec << endl; 
    return outRec;
  }
  
  void SynthesisDeconvolver::setSubsetRobustStats(const Record& inrec, const Int chanBeg, const Int chanEnd, const Int numchan){
    if(inrec.nfields()==0)
      return ;
    Matrix<Double> tmp;
    std::vector<String> keys={"min", "max", "rms", "medabsdevmed", "med", "robustrms", "median"};
   
    for (auto it = keys.begin() ; it != keys.end(); ++it){
      if(inrec.isDefined(*it)){
	tmp.resize();
	tmp=key2Mat(inrec, *it,itsImages->residual()->shape()[2] );
	Matrix<Double> outvec;
	if(itsRobustStats.isDefined(*it)){
	  outvec=key2Mat(itsRobustStats, *it, itsImages->residual()->shape()[2]);	 
	}
	else{
	  outvec.resize(itsImages->residual()->shape()[2], numchan);
	  outvec.set(C::dbl_max);
	}
	
	
	outvec(IPosition(2, 0, chanBeg), IPosition(2,outvec.shape()[0]-1, chanEnd))=tmp;
	itsRobustStats.define(*it, outvec);     
      }
    }

    //cerr << "SETT " << itsRobustStats << endl;
    //cerr << "SETT::ItsRobustStats " << Vector<Double>(itsRobustStats.asArrayDouble("min")) << endl;
  }
} //# NAMESPACE CASA - END

