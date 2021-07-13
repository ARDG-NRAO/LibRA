//# Calibrater.cc: Implementation of Calibrater.h
//# Copyright (C) 1996,1997,1998,1999,2000,2001,2002,2003
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
//# $Id: Calibrater.cc,v 19.37 2006/03/16 01:28:09 gmoellen Exp $

#include <tables/Tables/Table.h>
#include <tables/Tables/TableRecord.h>
#include <tables/Tables/TableDesc.h>
#include <tables/Tables/TableLock.h>
#include <tables/TaQL/TableParse.h>
#include <tables/Tables/ArrColDesc.h>
#include <tables/DataMan/TiledShapeStMan.h>

#include <casa/System/AipsrcValue.h>
#include <casa/Arrays/ArrayUtil.h>
#include <casa/Arrays/ArrayLogical.h>
#include <casa/Arrays/ArrayPartMath.h>
//#include <casa/Arrays/ArrayMath.h>
#include <ms/MeasurementSets/MSColumns.h>
#include <ms/MSSel/MSFieldIndex.h>
#include <ms/MSSel/MSSelection.h>
#include <ms/MSSel/MSSelectionTools.h>
#include <casa/BasicSL/Constants.h>
#include <casa/Exceptions/Error.h>
#include <casa/iostream.h>
#include <casa/sstream.h>
#include <casa/OS/File.h>
#include <synthesis/MeasurementComponents/Calibrater.h>
#include <synthesis/CalTables/CLPatchPanel.h>
#include <synthesis/MeasurementComponents/CalSolVi2Organizer.h>
#include <synthesis/MeasurementComponents/MSMetaInfoForCal.h>
#include <synthesis/MeasurementComponents/VisCalSolver.h>
#include <synthesis/MeasurementComponents/VisCalSolver2.h>
#include <synthesis/MeasurementComponents/UVMod.h>
#include <synthesis/MeasurementComponents/TsysGainCal.h>
#include <msvis/MSVis/VisSetUtil.h>
#include <msvis/MSVis/VisBuffAccumulator.h>
#include <msvis/MSVis/VisibilityIterator2.h>
#include <msvis/MSVis/VisBuffer2.h>
#include <msvis/MSVis/ViFrequencySelection.h>
#include <casa/Quanta/MVTime.h>

#include <casa/Logging/LogMessage.h>
#include <casa/Logging/LogIO.h>
#include <casa/Utilities/Assert.h>

#include <tables/Tables/SetupNewTab.h>
#include <vector>
using std::vector;
#include <msvis/MSVis/UtilJ.h>

#ifdef _OPENMP
 #include <omp.h>
#endif

//#define REPORT_CAL_TIMING

using namespace casacore;
using namespace casa::utilj;

using namespace casacore;
using namespace casa::vpf;

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

Calibrater::Calibrater(): 
  ms_p(0), 
  mssel_p(0), 
  mss_p(0),
  frequencySelections_p(nullptr),
  msmc_p(0),
  ve_p(0),
  vc_p(),
  svc_p(0),
  histLockCounter_p(), 
  hist_p(0),
  usingCalLibrary_(false),
  corrDepFlags_(false), // default (==traditional behavior)
  actRec_(),
  simdata_p(false),
  ssvp_p()
{
  //  cout << "This is the NEW VI2-aware Calibrater" << endl;
}

Calibrater::Calibrater(String msname): 
  msname_p(msname),
  ms_p(0), 
  mssel_p(0), 
  mss_p(0),
  frequencySelections_p(nullptr),
  msmc_p(0),
  ve_p(0),
  vc_p(),
  svc_p(0),
  histLockCounter_p(), 
  hist_p(0),
  usingCalLibrary_(false),
  corrDepFlags_(false), // default (==traditional behavior)
  actRec_(),
  simdata_p(false),
  ssvp_p()
{


  if (!Table::isReadable(msname))
    throw(AipsError("MS "+msname+" does not exist."));


  logSink() << LogOrigin("Calibrater","") << LogIO::NORMAL
	    << "Arranging to calibrate MS: "+msname
	    << LogIO::POST;
  
  // This is a bare Calibrater, intended to serve a VisEquation 

  // We need very little of the usual stuff

  // A VisEquation
  ve_p = new VisEquation();

  // Reset the apply/solve VisCals
  reset();

}

Calibrater::Calibrater(const vi::SimpleSimVi2Parameters& ssvp): 
  msname_p("<noms>"),
  ms_p(0), 
  mssel_p(0), 
  mss_p(0),
  frequencySelections_p(nullptr),
  msmc_p(0),
  ve_p(0),
  vc_p(),
  svc_p(0),
  histLockCounter_p(), 
  hist_p(0),
  usingCalLibrary_(false),
  corrDepFlags_(false), // default (==traditional behavior)
  actRec_(),
  simdata_p(true),
  ssvp_p(ssvp)
{

  logSink() << LogOrigin("Calibrater","") << LogIO::NORMAL
	    << "Arranging SIMULATED MS data for testing!!!!"
	    << LogIO::POST;
  
  // A VisEquation
  ve_p = new VisEquation();

  // Initialize the meta-info server that will be shared with VisCals
  if (msmc_p) delete msmc_p;
  msmc_p = new MSMetaInfoForCal(ssvp_p);

  // Reset the apply/solve VisCals
  reset();

}

/*
Calibrater::Calibrater(const Calibrater & other)
{
  operator=(other);
}

Calibrater& Calibrater::operator=(const Calibrater & other)
{
  ms_p=other.ms_p;
  mssel_p=other.mssel_p;
  ve_p=other.ve_p;
  histLockCounter_p=other.histLockCounter_p;
  hist_p=other.hist_p;
  historytab_p=other.historytab_p;
  
  return *this;
}
*/

Calibrater::~Calibrater()
{
  cleanup();
  if (msmc_p) delete msmc_p; msmc_p=0;
  if (ms_p)   delete ms_p;   ms_p=0;
  if (hist_p) delete hist_p; hist_p=0;

}

Calibrater* Calibrater::factory(Bool old) {
  
  Calibrater* cal(NULL);

  if (old)
    cal = new OldCalibrater();
  else 
    //throw(AipsError("VI2-aware Calibrater not yet available..."));
    cal = new Calibrater();

  return cal;

}

Calibrater* Calibrater::factory(String msname, Bool old) {
  
  Calibrater* cal(NULL);

  if (old)
    cal = new OldCalibrater(msname);
  else
    throw(AipsError("VI2-aware Calibrater not yet available..."));
  //cal = new Calibrater(msname);

  return cal;

}


LogIO& Calibrater::logSink() {return sink_p;};

String Calibrater::timerString() {
  ostringstream o;
  o <<" [user:   " << timer_p.user () << 
    "  system: " << timer_p.system () <<
    "  real:   " << timer_p.real () << "]"; 
  timer_p.mark();
  return o;
};

Bool Calibrater::initialize(MeasurementSet& inputMS, 
			    Bool compress,
			    Bool addScratch, Bool addModel)  {
  
  logSink() << LogOrigin("Calibrater","") << LogIO::NORMAL3;
  
  try {
    timer_p.mark();

    // Set pointer ms_p from input MeasurementSet
    if (ms_p) {
      *ms_p=inputMS;
    } else {
      ms_p = new MeasurementSet(inputMS);
      AlwaysAssert(ms_p,AipsError);
    };

    // Remember the ms's name
    msname_p=ms_p->tableName();

    // Initialize the meta-info server that will be shared with VisCals
    if (msmc_p) delete msmc_p;
    msmc_p = new MSMetaInfoForCal(*ms_p);

    // Add/init scr cols, if requested (init is hard-wired)
    if (addScratch || addModel) {
      Bool alsoinit=true;
      VisSetUtil::addScrCols(*ms_p,addModel,addScratch,alsoinit,compress);
    }

    // Set the selected MeasurementSet to be the same initially
    // as the input MeasurementSet
    logSink() << LogIO::NORMAL
	      << "Initializing nominal selection to the whole MS."
	      << LogIO::POST;

    if (mssel_p) delete mssel_p;
    mssel_p=new MeasurementSet(*ms_p);
    
    // Create the associated VisEquation
    //  TBD: move to ctor and make it non-pointer
    if (ve_p) {
      delete ve_p;
      ve_p=0;
    };
    ve_p=new VisEquation();

    // Reset the apply/solve VisCals
    reset(true,true);

    return true;

  } catch (AipsError x) {
    logSink() << LogOrigin("Calibrater","initialize",WHERE) 
	      << LogIO::SEVERE << "Caught exception: " << x.getMesg() 
	      << LogIO::POST;
    cleanup();
    if (msmc_p) delete msmc_p; msmc_p=NULL;
    if (ms_p)   delete ms_p;   ms_p=NULL;
    if (hist_p) delete hist_p; hist_p=NULL;

    throw(AipsError("Error in Calibrater::initialize()"));
    return false;
  } 
  return false;
}


// Select data (using MSSelection syntax)
void Calibrater::selectvis(const String& time,
			   const String& spw,
			   const String& scan,
			   const String& field,
			   const String& intent,
			   const String& obsIDs,
			   const String& baseline,
			   const String& uvrange,
			   const String& chanmode,
			   const Int&,
			   const Int&,
			   const Int&,
			   const MRadialVelocity&,
			   const MRadialVelocity&,
			   const String& msSelect)
{
// Define primary measurement set selection criteria
// Inputs:
//    time
//    spw
//    scan
//    field
//    intent
//    obsIDs
//    baseline
//    uvrange
//    chanmode     const String&            Frequency/velocity selection mode
//                                          ("channel", "velocity" or 
//                                           "opticalvelocity")
//    nchan        const Int&               No of channels to select
//    start        const Int&               Start channel to select
//    step         const Int&               Channel increment
//    mStart       const MRadialVelocity&   Start radial vel. to select
//    mStep        const MRadialVelocity&   Radial velocity increment
//    msSelect     const String&            MS selection string (TAQL)
// Output to private data:
//
  logSink() << LogOrigin("Calibrater","selectvis") << LogIO::NORMAL3;
  
  try {

 /*   
    cout << "time     = " << time << " " << time.length() <<endl;
    cout << "spw      = " << spw << " " << spw.length() <<endl;
    cout << "scan     = " << scan << " " << scan.length() <<endl;
    cout << "field    = " << field << " " << field.length() <<endl;
    cout << "baseline = " << baseline << " " << baseline.length() << endl;
    cout << "uvrange  = " << uvrange << " " << uvrange.length() << endl;
 */

    logSink() << "Selecting data" << LogIO::POST;
    
    // Apply selection to the original MeasurementSet
    logSink() << "Performing selection on MeasurementSet" << endl;
    
    if (mssel_p) {
      delete mssel_p;
      mssel_p=0;
    };

    // Report non-trivial user selections
    if (time!="")
      logSink() << " Selecting on time: '" << time << "'" << endl;
    if (spw!="")
      logSink() << " Selecting on spw: '" << spw << "'" << endl;
    if (scan!="")
      logSink() << " Selecting on scan: '" << scan << "'" << endl;
    if (field!="")
      logSink() << " Selecting on field: '" << field << "'" << endl;
    if (intent!="")
      logSink() << " Selecting on intent: '" << intent << "'" << endl;
    if(obsIDs != "")
      logSink() << " Selecting by observation IDs: '" << obsIDs << "'" << endl;
    if (baseline!="")
      logSink() << " Selecting on antenna/baseline: '" << baseline << "'" << endl;
    if (uvrange!="")
      logSink() << " Selecting on uvrange: '" << uvrange << "'" << endl;
    if (msSelect!="")
      logSink() << " Selecting with TaQL: '" << msSelect << "'" << endl;
    logSink() << LogIO::POST;


    // Assume no selection, for starters
    mssel_p = new MeasurementSet(*ms_p);

    // Apply user-supplied selection
    Bool nontrivsel=false;

    // Ensure use of a fresh MSSelection object
    if (mss_p) { delete mss_p; mss_p=NULL; }
    mss_p=new MSSelection();
    nontrivsel= mssSetData(*ms_p,
			   *mssel_p,"",
			   time,baseline,
			   field,spw,
			   uvrange,msSelect,
			   "",scan,"",intent, obsIDs,mss_p);

    // Keep any MR status for the MS
    mssel_p->setMemoryResidentSubtables(ms_p->getMrsEligibility());

    // If non-trivial MSSelection invoked and nrow reduced:
    if(nontrivsel && mssel_p->nrow()<ms_p->nrow()) {

      // Escape if no rows selected
      if (mssel_p->nrow()==0) 
	throw(AipsError("Specified selection selects zero rows!"));

      // ...otherwise report how many rows are selected
      logSink() << "By selection " << ms_p->nrow() 
		<< " rows are reduced to " << mssel_p->nrow() 
		<< LogIO::POST;
    }
    else {
      // Selection did nothing:
      logSink() << "Selection did not drop any rows" << LogIO::POST;
    }

    // Attempt to use MSSelection for channel selection
    //  if user not using the old way
    if (chanmode=="none") {
      selectChannel(spw);
    }
    else {
      // Old-fashioned way now deprecated
      logSink() << LogIO::WARN 
		<< "You have used the old-fashioned mode parameter" << endl
		<< "for channel selection." << endl
		<< "Please begin using the new channel selection" << endl
		<< "syntax in the spw parameter." << LogIO::POST;
      throw(AipsError("Old-fashioned chanmode selection is no longer supported!"));
    }

  }
  catch (MSSelectionError& x) {
    // Re-initialize with the existing MS
    logSink() << LogOrigin("Calibrater","selectvis",WHERE) 
	      << LogIO::SEVERE << "Caught exception: " << x.getMesg()
	      << LogIO::POST;
    // jagonzal (CAS-4110): I guess it is not necessary to create these columns when the selection is empty
    initialize(*ms_p,false,false,false);
    throw(AipsError("Error in data selection specification: " + x.getMesg()));
  } 
  catch (AipsError x) {
    // Re-initialize with the existing MS
    logSink() << LogOrigin("Calibrater","selectvis",WHERE) 
	      << LogIO::SEVERE << "Caught exception: " << x.getMesg()
	      << LogIO::POST;
    // jagonzal (CAS-4110): I guess it is not necessary to create these columns when the selection is empty.
    initialize(*ms_p,false,false,false);
    throw(AipsError("Error in Calibrater::selectvis(): " + x.getMesg()));
  } 
};


Bool Calibrater::setapply(const String& type, 
			  const Double& t,
			  const String& table,
			  const String& spw,
			  const String& field,
			  const String& interp,
			  const Bool& calwt,
			  const Vector<Int>& spwmap,
			  const Vector<Double>& opacity) 
{

  logSink() << LogOrigin("Calibrater",
                         "setapply(type, t, table, spw, field, interp, calwt, spwmap, opacity)")
            << LogIO::NORMAL;


  //  cout << "Calibrater::setapply: field="<< field << endl;
   
  // Set record format for calibration table application information
  RecordDesc applyparDesc;
  applyparDesc.addField ("t", TpDouble);
  applyparDesc.addField ("table", TpString);
  applyparDesc.addField ("interp", TpString);
  applyparDesc.addField ("spw", TpArrayInt);
  //  applyparDesc.addField ("field", TpArrayInt);
  applyparDesc.addField ("fieldstr", TpString);
  applyparDesc.addField ("calwt",TpBool);
  applyparDesc.addField ("spwmap",TpArrayInt);
  applyparDesc.addField ("opacity",TpArrayDouble);
  
  // Create record with the requisite field values
  Record applypar(applyparDesc);
  applypar.define ("t", t);
  applypar.define ("table", table);

  /*
  String cinterp=interp;
  //  cinterp.erase(remove_if(cinterp.begin(), cinterp.end(), isspace), cinterp.end());
  cinterp.erase( remove( cinterp.begin(), cinterp.end(), ' ' ), cinterp.end() );
  */
 
  applypar.define ("interp", interp);
  applypar.define ("spw",getSpwIdx(spw));
  //  applypar.define ("field",getFieldIdx(field));
  applypar.define ("fieldstr",field);
  applypar.define ("calwt",calwt);
  applypar.define ("spwmap",spwmap);
  applypar.define ("opacity", opacity);
  
  String upType=type;
  upType.upcase();
  if (upType=="")
    // Get type from table
    upType = calTableType(table);

  return setapply(upType,applypar);

}

Bool Calibrater::setapply (const String& type, 
			   const Record& applypar)
{
  logSink() << LogOrigin("Calibrater", "setapply(type, applypar)");

  // First try to create the requested VisCal object
  VisCal *vc(NULL);

  try {

    if(!ok()) 
      throw(AipsError("Calibrater not prepared for setapply."));

    String upType=type;
    upType.upcase();

    logSink() << LogIO::NORMAL 
	      << "Arranging to APPLY:"
	      << LogIO::POST;

    // Add a new VisCal to the apply list
    vc = createVisCal(upType,*msmc_p);  

    vc->setApply(applypar);       

    logSink() << LogIO::NORMAL << ".   "
	      << vc->applyinfo()
	      << LogIO::POST;

  } catch (AipsError x) {
    logSink() << LogIO::SEVERE << x.getMesg() 
	      << " Check inputs and try again."
	      << LogIO::POST;
    if (vc) delete vc;
    throw(AipsError("Error in Calibrater::setapply."));
    return false;
  }

  // Creation apparently successful, so add to the apply list
  // TBD: consolidate with above?
  try {

    uInt napp=vc_p.nelements();
    vc_p.resize(napp+1,false,true);      
    vc_p[napp] = vc;
    vc=NULL;
   
    // Maintain sort of apply list
    ve_p->setapply(vc_p);
    
    return true;

  } catch (AipsError x) {
    logSink() << LogIO::SEVERE << "Caught exception: " << x.getMesg() 
	      << LogIO::POST;
    if (vc) delete vc;
    throw(AipsError("Error in Calibrater::setapply."));
    return false;
  } 
  return false;
}


// Validate a Cal Library record
Bool Calibrater::validatecallib(Record callib) {

  uInt ntab=callib.nfields();
  for (uInt itab=0;itab<ntab;++itab) {

    String tabname=callib.name(itab);
    Record thistabrec=callib.asRecord(itab);
    uInt ncl=thistabrec.nfields();

    for (uInt icl=0;icl<ncl;++icl) {

      if (thistabrec.dataType(icl)!=TpRecord)
	continue;

      Record thisicl=thistabrec.asRecord(icl);
      try {
	CalLibSlice::validateCLS(thisicl);
      }
      catch ( AipsError x) {
	logSink() << LogIO::SEVERE
		  << "Caltable " << tabname 
		  << " is missing the following fields: " 
		  << x.getMesg() 
		  << LogIO::POST;
      }

    }
  }
  return true;
}


// Set up apply-able calibration via a Cal Library
Bool Calibrater::setcallib2(Record callib, const MeasurementSet* ms) {

  logSink() << LogOrigin("Calibrater", "setcallib2(callib)");

  //  cout << "Calibrater::setcallib2(callib) : " << boolalpha << callib << endl;

  uInt ntab=callib.nfields();

  //  cout << "callib.nfields() = " << ntab << endl;

  // Do some preliminary per-table verification
  for (uInt itab=0;itab<ntab;++itab) {

    String tabname=callib.name(itab);

    // Trap parang
    // TBD...
    //    if (tabname=="<parang>")
    //      continue;

    // Insist that the table exists on disk
    if (!Table::isReadable(tabname))
      throw(AipsError("Caltable "+tabname+" does not exist."));

  }

  // Tables exist, so deploy them...

  // Local MS object for callib parsing (only)
  //  MeasurementSet lms(msname_p,Table::Update);
  // TBD: Use selected MS instead (not yet available in OTF plotms context!)
  //const MeasurementSet lms(*mssel_p);
  //MeasurementSet lms(msname_p);

  // Local const MS object for callib parsing (only)
  const MeasurementSet *lmsp(0);
  if (ms) {
    // Use supplied MS (from outside), if specified...
    // TBD: should we verify same base MS as ms_p/mssel_p?
    //cout << "Using externally-specified MS!!" << endl;
    lmsp=ms;
  }
  else {
    // ...use internal one instead
    //cout << "Using internal MS (mssel_p)!!" << endl;
    lmsp=mssel_p;
  }
  // Reference for use below
  const MeasurementSet &lms(*lmsp);


  for (uInt itab=0;itab<ntab;++itab) {

    String tabname=callib.name(itab);

    // Get the type from the table
    String upType=calTableType(tabname);
    upType.upcase();

    // Add table name to the record
    Record thistabrec=callib.asrwRecord(itab);
    thistabrec.define("tablename",tabname);

    // First try to create the requested VisCal object
    VisCal *vc(NULL);

    try {

      //      if(!ok()) 
      //	throw(AipsError("Calibrater not prepared for setapply."));
      
      logSink() << LogIO::NORMAL 
		<< "Arranging to APPLY:"
		<< LogIO::POST;
      
      // Add a new VisCal to the apply list
      vc = createVisCal(upType,*msmc_p);

      // ingest this table according to its callib
      vc->setCallib(thistabrec,lms);
      
    } catch (AipsError x) {
      logSink() << LogIO::SEVERE << x.getMesg() 
		<< " Check inputs and try again."
		<< LogIO::POST;
      if (vc) delete vc;
      throw(AipsError("Error in Calibrater::callib2."));
      return false;
    }

    // Creation apparently successful, so add to the apply list
    // TBD: consolidate with above?
    try {
      
      uInt napp=vc_p.nelements();
      vc_p.resize(napp+1,false,true);      
      vc_p[napp] = vc;
      vc=NULL;
   
      // Maintain sort of apply list
      ve_p->setapply(vc_p);
      
    } catch (AipsError x) {
      logSink() << LogIO::SEVERE << "Caught exception: " << x.getMesg() 
		<< LogIO::POST;
      if (vc) delete vc;
      throw(AipsError("Error in Calibrater::setapply."));
      return false;
    } 
  }

  // Signal use of CalLibrary
  usingCalLibrary_=true;

  // All ok, if we get this far!
  return true;

}

Bool Calibrater::setmodel(const String& modelImage)
{
  if (!svc_p)
    throw(AipsError("Calibrater::setmodel() called before Calibrater::setsolve()"));
  svc_p->setModel(modelImage);
  return true;
}

Bool Calibrater::setModel(const Vector<Double>& stokes) {

  if (ve_p) {
    Vector<Float> fstokes(stokes.shape());
    convertArray(fstokes,stokes);
    ve_p->setModel(fstokes);
  }
  else
    throw(AipsError("Error in Calibrater::setModel: no VisEquation."));

  return true;

}

Bool Calibrater::setsolve (const String& type, 
			   const String& solint,
			   const String& table,
                           const Bool append,
                           const Double preavg, 
			   const String& apmode,
			   const Int minblperant,
                           const String& refant,
                           const String& refantmode,
			   const Bool solnorm,
			   const String& normtype,
			   const Float minsnr,
			   const String& combine,
			   const Int fillgaps,
			   const String& cfcache,
			   const Double painc,
                           const Int fitorder,
                           const Float fraction,
                           const Int numedge,
                           const String& radius,
                           const Bool smooth,
                           const Bool zerorates,
                           const Bool globalsolve,
                           const Int niter,
                           const Vector<Double>& delaywindow, 
                           const Vector<Double>& ratewindow,
                           const Vector<Bool>& paramactive,
			   const String& solmode,
			   const Vector<Double>& rmsthresh
    )
{
  
  logSink() << LogOrigin("Calibrater","setsolve") << LogIO::NORMAL3;
  
  // Create a record description containing the solver parameters
  RecordDesc solveparDesc;
  solveparDesc.addField ("solint", TpString);
  solveparDesc.addField ("preavg", TpDouble);
  solveparDesc.addField ("apmode", TpString);
  solveparDesc.addField ("refant", TpArrayInt);
  solveparDesc.addField ("refantmode", TpString);
  solveparDesc.addField ("minblperant", TpInt);
  solveparDesc.addField ("table", TpString);
  solveparDesc.addField ("append", TpBool);
  solveparDesc.addField ("solnorm", TpBool);
  solveparDesc.addField ("normtype", TpString);
  solveparDesc.addField ("type", TpString);
  solveparDesc.addField ("combine", TpString);
  solveparDesc.addField ("maxgap", TpInt);
  solveparDesc.addField ("cfcache", TpString);
  solveparDesc.addField ("painc", TpDouble);
  solveparDesc.addField ("fitorder", TpInt);
  solveparDesc.addField ("solmode", TpString);
  solveparDesc.addField ("rmsthresh", TpArrayDouble);

  // fringe-fit specific fields
  solveparDesc.addField ("zerorates", TpBool);
  solveparDesc.addField ("minsnr", TpFloat);
  solveparDesc.addField ("globalsolve", TpBool);
  solveparDesc.addField ("delaywindow", TpArrayDouble);
  solveparDesc.addField ("ratewindow", TpArrayDouble);
  solveparDesc.addField ("niter", TpInt);
  solveparDesc.addField ("paramactive", TpArrayBool);

  // single dish specific fields
  solveparDesc.addField ("fraction", TpFloat);
  solveparDesc.addField ("numedge", TpInt);
  solveparDesc.addField ("radius", TpString);
  solveparDesc.addField ("smooth", TpBool);


  // Create a solver record with the requisite field values
  Record solvepar(solveparDesc);
  solvepar.define ("solint", solint);
  solvepar.define ("preavg", preavg);
  String upmode=apmode;
  upmode.upcase();
  solvepar.define ("apmode", upmode);
  solvepar.define ("refant", getRefantIdxList(refant));
  solvepar.define ("refantmode", refantmode);
  solvepar.define ("minblperant", minblperant);
  solvepar.define ("table", table);
  solvepar.define ("append", append);
  solvepar.define ("solnorm", solnorm);
  solvepar.define ("normtype", normtype);
  // Fringe-fit specific
  solvepar.define ("minsnr", minsnr);
  solvepar.define ("zerorates", zerorates);
  solvepar.define ("globalsolve", globalsolve);
  solvepar.define ("niter", niter);
  solvepar.define ("delaywindow", delaywindow);
  solvepar.define ("ratewindow", ratewindow);
  solvepar.define ("solmode", solmode);
  solvepar.define ("rmsthresh", rmsthresh);
  solvepar.define ("paramactive", paramactive);
  
  String uptype=type;
  uptype.upcase();
  solvepar.define ("type", uptype);
  
  String upcomb=combine;
  upcomb.upcase();
  solvepar.define("combine",upcomb);
  solvepar.define("maxgap",fillgaps);
  solvepar.define ("cfcache", cfcache);
  solvepar.define ("painc", painc);
  solvepar.define("fitorder", fitorder);

  // single dish specific
  solvepar.define("fraction", fraction);
  solvepar.define("numedge", numedge);
  solvepar.define("radius", radius);
  solvepar.define("smooth", smooth);

  return setsolve(type,solvepar);
}

Bool Calibrater::setsolvebandpoly(const String& table,
				  const Bool& append,
				  const String& solint,
				  const String& combine,
				  const Vector<Int>& degree,
				  const Bool& visnorm,
				  const Bool& solnorm,
				  const Int& maskcenter,
				  const Float& maskedge,
				  const String& refant) {

  logSink() << LogOrigin("Calibrater","setsolvebandpoly") << LogIO::NORMAL3;

  // TBD: support solution interval!

    // Create a record description containing the solver parameters
    RecordDesc solveparDesc;
    solveparDesc.addField ("table", TpString);
    solveparDesc.addField ("append", TpBool);
    solveparDesc.addField ("solint", TpString);
    solveparDesc.addField ("combine", TpString);
    solveparDesc.addField ("degree", TpArrayInt);
    solveparDesc.addField ("visnorm", TpBool);
    solveparDesc.addField ("solnorm", TpBool);
    solveparDesc.addField ("maskcenter", TpInt);
    solveparDesc.addField ("maskedge", TpFloat);
    solveparDesc.addField ("refant", TpArrayInt);

    //    solveparDesc.addField ("preavg", TpDouble);
    //    solveparDesc.addField ("phaseonly", TpBool);
    
    // Create a solver record with the requisite field values
    Record solvepar(solveparDesc);
    solvepar.define ("table", table);
    solvepar.define ("append", append);
    solvepar.define ("solint",solint);
    String upcomb=combine;
    upcomb.upcase();
    solvepar.define ("combine",combine);
    solvepar.define ("degree", degree);
    solvepar.define ("visnorm", visnorm);
    solvepar.define ("solnorm", solnorm);
    solvepar.define ("maskcenter", maskcenter);
    solvepar.define ("maskedge", maskedge);
    solvepar.define ("refant", getRefantIdxList(refant));


    //    solvepar.define ("t", t);
    //    solvepar.define ("preavg", preavg);
    //    solvepar.define ("phaseonly", phaseonly);


    return setsolve("BPOLY",solvepar);

}

Bool Calibrater::setsolvegainspline(const String& table,
				    const Bool&   append,
				    const String& apmode,
				    const Double& splinetime,
				    const Double& preavg,
				    const Int&    numpoint,
				    const Double& phasewrap,
				    const String& refant) {
  
  logSink() << LogOrigin("Calibrater","setsolvegainspline") << LogIO::NORMAL3;

  // Create a record description containing the solver parameters
  RecordDesc solveparDesc;
  solveparDesc.addField ("table", TpString);
  solveparDesc.addField ("append", TpBool);
  solveparDesc.addField ("apmode", TpString);
  solveparDesc.addField ("splinetime", TpDouble);
  solveparDesc.addField ("preavg", TpDouble);
  solveparDesc.addField ("refant", TpArrayInt);
  solveparDesc.addField ("numpoint", TpInt);
  solveparDesc.addField ("phasewrap", TpDouble);
  
  // Create a solver record with the requisite field values
  Record solvepar(solveparDesc);
  solvepar.define ("table", table);
  solvepar.define ("append", append);
  String upMode=apmode;
  upMode.upcase();
  solvepar.define ("apmode", upMode);
  solvepar.define ("splinetime",splinetime);
  solvepar.define ("preavg", preavg);
  solvepar.define ("refant", getRefantIdxList(refant));
  solvepar.define ("numpoint",numpoint);
  solvepar.define ("phasewrap",phasewrap);
  
  return setsolve("GSPLINE",solvepar);
  
}

Bool Calibrater::setsolve (const String& type, 
			   const Record& solvepar) {

  // Attempt to create the solvable object
  SolvableVisCal *svc(NULL);
  try {

    if(!ok()) 
      throw(AipsError("Calibrater not prepared for setsolve."));

    String upType = type;
    upType.upcase();

    // Clean out any old solve that was lying around
    unsetsolve();

    logSink() << LogIO::NORMAL 
	      << "Arranging to SOLVE:"
	      << LogIO::POST;

    // Create the new SolvableVisCal
    svc = createSolvableVisCal(upType,*msmc_p);
    svc->setSolve(solvepar);
    
    logSink() << LogIO::NORMAL << ".   "
	      << svc->solveinfo()
	      << LogIO::POST;

    // Creation apparently successful, keep it
    svc_p=svc;
    svc=NULL;

    // if calibration specific data filter is necessary
    // keep configuration parameter as a record
    setCalFilterConfiguration(upType, solvepar);

    return true;

  } catch (AipsError x) {
    logSink() << LogIO::SEVERE << "Caught exception: " << x.getMesg() 
	      << LogIO::POST;
    unsetsolve();
    if (svc) delete svc;
    throw(AipsError("Error in Calibrater::setsolve."));
    return false;
  } 
  return false;
}

Bool Calibrater::state() {

  logSink() << LogOrigin("Calibrater","state") << LogIO::NORMAL3;

  applystate();
  solvestate();

  return true;

}

Bool Calibrater::applystate() {


  //  logSink() << LogOrigin("Calibrater","applystate") << LogIO::NORMAL;

  logSink() << LogIO::NORMAL 
	    << "The following calibration terms are arranged for apply:"
	    << LogIO::POST;

  Int napp(vc_p.nelements());
  if (napp>0)
    for (Int iapp=0;iapp<napp;++iapp)
      logSink() << LogIO::NORMAL << ".   "
		<< vc_p[iapp]->applyinfo()
		<< LogIO::POST;
  else
    logSink() << LogIO::NORMAL << ".   "
	      << "(None)"
	      << LogIO::POST;

  return true;

}


Bool Calibrater::solvestate() {

  //  logSink() << LogOrigin("Calibrater","solvestate") << LogIO::NORMAL;

  logSink() << LogIO::NORMAL 
	    << "The following calibration term is arranged for solve:"
	    << LogIO::POST;

  if (svc_p)
    logSink() << LogIO::NORMAL << ".   "
	      << svc_p->solveinfo()
	      << LogIO::POST;
  else
    logSink()  << LogIO::NORMAL << ".   "
	      << "(None)"
	      << LogIO::POST;

  return true;
}

Bool Calibrater::reset(const Bool& apply, const Bool& solve) {

  //  logSink() << LogOrigin("Calibrater","reset") << LogIO::NORMAL;

  // Delete the VisCal apply list
  if (apply)   
    unsetapply();

  // Delete the VisCal solve object
  if (solve)
    unsetsolve();
        
  return true;
}

// Delete all (default) or one VisCal in apply list
Bool Calibrater::unsetapply(const Int& which) {

  //  logSink() << LogOrigin("Calibrater","unsetapply") << LogIO::NORMAL;
  
  try {
    if (which<0) {
      for (uInt i=0;i<vc_p.nelements();i++)
	if (vc_p[i]) delete vc_p[i];
      vc_p.resize(0,true);
    } else {
      if (vc_p[which]) delete vc_p[which];
      vc_p.remove(which);
    }
    
    // Maintain size/sort of apply list
    if(ve_p) ve_p->setapply(vc_p);

    return true;
  } catch (AipsError x) {
    logSink() << LogIO::SEVERE << "Caught exception: " << x.getMesg() 
	      << LogIO::POST;
    throw(AipsError("Error in Calibrater::unsetapply."));

    return false;
  }
  return false;
}

  // Delete solve VisCal
Bool Calibrater::unsetsolve() {

  //  logSink() << LogOrigin("Calibrater","unsetsolve") << LogIO::NORMAL;

  try {
    if (svc_p) delete svc_p;
    svc_p=NULL;
    
    if(ve_p) ve_p->setsolve(*svc_p);

    return true;

  } catch (AipsError x) {
    logSink() << LogIO::SEVERE << "Caught exception: " << x.getMesg() 
	      << LogIO::POST;
    throw(AipsError("Error in Calibrater::unsetsolve."));
    return false;
  }
  return false;
}

Bool
Calibrater::setCorrDepFlags(const Bool& corrDepFlags) 
{

  logSink() << LogOrigin("Calibrater","setCorrDepFlags") << LogIO::NORMAL;

  // Set it
  corrDepFlags_=corrDepFlags;

  logSink() << "Setting correlation dependent flags = " << (corrDepFlags_ ? "True" : "False") << LogIO::POST;

  return true;

}


Bool
Calibrater::correct2(String mode)
{
    logSink() << LogOrigin("Calibrater","correct2 (VI2/VB2)") << LogIO::NORMAL;

    //cout << "Artificial STOP!" << endl;
    //return false;


    Bool retval = true;

    try {

      // Ensure apply list non-zero and properly sorted
      ve_p->setapply(vc_p);

      bool forceOldVIByEnv(false);
      forceOldVIByEnv = (getenv("VI1CAL")!=NULL);
      if (forceOldVIByEnv && anyEQ(ve_p->listTypes(),VisCal::A)) {
	logSink() << LogIO::WARN << "Using VI2 calibration apply.  AMueller (uvcontsub) no longer requires VI1 for apply." << LogIO::POST;
      }

      /*   CAS-12434 (2019Jun07, gmoellen): AMueller works with VB2 in _apply_ (only) context now
      // Trap uvcontsub case, since it does not yet handlg VB2
      if (anyEQ(ve_p->listTypes(),VisCal::A)) {

	// Only uvcontsub, nothing else
	if (ve_p->nTerms()==1) {
	  // Use old method (which doesn't need WEIGHT_SPECTRUM support in this context)
	  return this->correct(mode);
	}
	else
	  throw(AipsError("Cannot handle AMueller (uvcontsub) and other types simultaneously."));
      }
      */
      
      // Report the types that will be applied
      applystate();

      // make mode all-caps
      String upmode=mode;
      upmode.upcase();

      // If trialmode=T, only the flags will be set
      //   (and only written if not TRIAL)
      Bool trialmode=(upmode.contains("TRIAL") || 
		      upmode.contains("FLAGONLY"));


      // Arrange for iteration over data
      Block<Int> columns;
      // include scan iteration
      columns.resize(5);
      columns[0]=MS::ARRAY_ID;
      columns[1]=MS::SCAN_NUMBER;
      columns[2]=MS::FIELD_ID;
      columns[3]=MS::DATA_DESC_ID;
      columns[4]=MS::TIME;

      vi::SortColumns sc(columns);
      vi::VisibilityIterator2 vi(*mssel_p,sc,true);

      // Apply channel selection (see selectChannel(spw))
      if (frequencySelections_p)
	vi.setFrequencySelection(*frequencySelections_p);

      vi::VisBuffer2 *vb = vi.getVisBuffer();

      // Detect if we will be setting WEIGHT_SPECTRUM, and arrange for this
      vi.originChunks();    // required for wSExists() in next line to work
      vi.origin();
      Bool doWtSp=vi.weightSpectrumExists();  // Exists non-trivially

      if (doWtSp && calWt()) 
	logSink() << "Found valid WEIGHT_SPECTRUM, correcting it." << LogIO::POST;

      // Pass each timestamp (VisBuffer) to VisEquation for correction
      
      Vector<Bool> uncalspw(vi.nSpectralWindows());  // Used to accumulate error messages
      uncalspw.set(false);		             // instead of bombing the user

      uInt nvb(0);

        for (vi.originChunks(); vi.moreChunks(); vi.nextChunk()) {

	  for (vi.origin(); vi.more(); vi.next()) {

	    uInt spw = vb->spectralWindows()(0);
	    //if (ve_p->spwOK(spw)){
	    //if (    (usingCalLibrary_ && ve_p->VBOKforCalApply(*vb))  // CalLibrary case
	    //     || (!usingCalLibrary_ && ve_p->spwOK(spw))          // old-fashioned case
	    //	 ) {
	    if ( ve_p->VBOKforCalApply(*vb) ) {  // Handles old and new (CL) contexts

	      // Re-initialize weight info from sigma info
	      //   This is smart wrt spectral weights, etc.
	      //   (this makes W and WS, if present, "dirty" in the vb)
	      // TBD: only do this if !trial (else: avoid the I/O)
	      vb->resetWeightsUsingSigma();

	      // Arrange for _in-place_ apply on CORRECTED_DATA (init from DATA)
	      //   (this makes CD "dirty" in the vb)
	      // TBD: only do this if !trial (else: avoid the I/O)
	      vb->setVisCubeCorrected(vb->visCube());

	      // Make flagcube dirty in the vb
	      //  NB: we must _always_ do this I/O  (even trial mode)
	      vb->setFlagCube(vb->flagCube());

	      // Make all vb "not dirty"; we'll carefully arrange the writeback below
	      vb->dirtyComponentsClear();

	      // throws exception if nothing to apply
	      ve_p->correct2(*vb,trialmode,doWtSp);
		    
	      // Only if not a trial run, manage writes to disk
	      if (upmode!="TRIAL") {
		
		if (upmode.contains("CAL")) {
		  vb->setVisCubeCorrected(vb->visCubeCorrected());

		  if (calWt()) {
		    // Set weights dirty only if calwt=T
		    if (doWtSp) {
		      vb->setWeightSpectrum(vb->weightSpectrum());
		      // If WS was calibrated, set W to its channel-axis median
		      vb->setWeight(partialMedians(vb->weightSpectrum(),IPosition(1,1)));
		    }
		    else
		      vb->setWeight(vb->weight()); 
		  }
		}

		if (upmode.contains("FLAG"))
		  vb->setFlagCube(vb->flagCube());
		
		// Push the calibrated data etc. back to the MS
		vb->writeChangesBack();
		nvb+=1;  // increment vb counter
	      }

	    }
	    else{
	      uncalspw[spw] = true;

	      // set the flags, if we are being strict
	      // (don't touch the data/weights, which are initialized)
	      if (upmode.contains("STRICT")) {

		// reference (to avoid copy) and set the flags
		Cube<Bool> fC(vb->flagCube());   // reference
		fC.set(true);  

		// make dirty for writeChangesBack  (does this do an actual copy?)
		vb->setFlagCube(vb->flagCube());

		// write back to 
		vb->writeChangesBack();

	      }
	      else {

		// Break out of inner VI2 loop, if possible
		if (! vi.isAsynchronous()){
		  
		  // Asynchronous I/O doesn't have a way to skip
		  // VisBuffers, so only break out when not using
		  // async i/o.
		  break; 

		}

	      }
	    }
	  }
        }

        // Now that we're out of the loop, summarize any errors.

        retval = summarize_uncalspws(uncalspw, "correct",
 				     upmode.contains("STRICT"));
	
	actRec_=Record();
	actRec_.define("origin","Calibrater::correct");
	actRec_.defineRecord("VisEquation",ve_p->actionRec());
	

	//cout << "Number of VisBuffers corrected: " << nvb << endl;

    }
    catch (AipsError x) {
        logSink() << LogIO::SEVERE << "Caught exception: " << x.getMesg()
	              << LogIO::POST;

        logSink() << "Resetting all calibration application settings." << LogIO::POST;
        unsetapply();

        throw(AipsError("Error in Calibrater::correct."));
        retval = false;         // Not that it ever gets here...
    }
    return retval;
}

Bool Calibrater::corrupt2()
{
    logSink() << LogOrigin("Calibrater","corrupt2 (VI2/VB2)") << LogIO::NORMAL;
    Bool retval = true;

    try {

      if (!ok())
	throw(AipsError("Calibrater not prepared for corrupt2!"));
      
      // MODEL_DATA column must be physically present!
      if (!ms_p->tableDesc().isColumn("MODEL_DATA"))
	throw(AipsError("MODEL_DATA column unexpectedly absent. Cannot corrupt."));

      // Ensure apply list non-zero and properly sorted
      ve_p->setapply(vc_p);

      /*
      // Trap uvcontsub case, since it does not yet handlg VB2
      if (anyEQ(ve_p->listTypes(),VisCal::A)) {

	// Only uvcontsub, nothing else
	if (ve_p->nTerms()==1) {
	  // Use old method (which doesn't need WEIGHT_SPECTRUM support in this context)
	  return this->correct(mode);
	}
	else
	  throw(AipsError("Cannot handle AMueller (uvcontsub) and other types simultaneously."));
      }
      */
      
      // Report the types that will be applied
      applystate();

      // Arrange for iteration over data
      Block<Int> columns;
      // include scan iteration
      columns.resize(5);
      columns[0]=MS::ARRAY_ID;
      columns[1]=MS::SCAN_NUMBER;
      columns[2]=MS::FIELD_ID;
      columns[3]=MS::DATA_DESC_ID;
      columns[4]=MS::TIME;

      vi::SortColumns sc(columns);
      vi::VisibilityIterator2 vi(*mssel_p,sc,true);

      // Apply channel selection (see selectChannel(spw))
      if (frequencySelections_p)
	vi.setFrequencySelection(*frequencySelections_p);

      vi::VisBuffer2 *vb = vi.getVisBuffer();

      // Detect if we will be setting WEIGHT_SPECTRUM, and arrange for this
      vi.originChunks();    // required for wSExists() in next line to work
      vi.origin();
      
      Vector<Bool> uncalspw(vi.nSpectralWindows());  // Used to accumulate error messages
      uncalspw.set(false);		             // instead of bombing the user

      // Pass each timestamp (VisBuffer) to VisEquation for correction
      uInt nvb(0);
      for (vi.originChunks(); vi.moreChunks(); vi.nextChunk()) {
	for (vi.origin(); vi.more(); vi.next()) {
	  uInt spw = vb->spectralWindows()(0);
	  //if (ve_p->spwOK(spw)){
	  if (usingCalLibrary_ || ve_p->spwOK(spw)){

	    // Make all vb "not dirty", for safety
	    vb->dirtyComponentsClear();

	    // throws exception if nothing to apply
	    ve_p->corrupt2(*vb);

	    // make visCubeModel _ONLY_ dirty for writeChangesBack
	    vb->setVisCubeModel(vb->visCubeModel());
	    
	    // Push the corrupted model back to the MS
	    vb->writeChangesBack();
	    nvb+=1;  // increment vb counter
	  }
	  else{
	    uncalspw[spw] = true;
	  }
	}
      }

      // Now that we're out of the loop, summarize any errors.
      retval = summarize_uncalspws(uncalspw, "corrupt2",false);   // (didn't flag them)
      
      actRec_=Record();
      actRec_.define("origin","Calibrater::corrupt2");
      actRec_.defineRecord("VisEquation",ve_p->actionRec());

      //cout << "Number of VisBuffers corrupted: " << nvb << endl;
      
    }
    catch (AipsError x) {
        logSink() << LogIO::SEVERE << "Caught exception: " << x.getMesg()
	              << LogIO::POST;

        logSink() << "Resetting all calibration application settings." << LogIO::POST;
        unsetapply();

        throw(AipsError("Error in Calibrater::corrupt2."));
        retval = false;         // Not that it ever gets here...
    }
    return retval;
 }

Bool Calibrater::initWeightsWithTsys(String wtmode, Bool dowtsp,
				     String tsystable, String gainfield, String interp, Vector<Int> spwmap) {

	logSink() << LogOrigin("Calibrater", "initWeightsWithTsys")
			<< LogIO::NORMAL;
	Bool retval = true;

	try {

		if (!ok())
			throw(AipsError("Calibrater not prepared for initWeights!"));

		String uptype = calTableType(tsystable);
		if (!uptype.contains("TSYS")) {
			throw(AipsError(
					"Invalid calibration table type for Tsys weighting."));
		}
		// Set record format for calibration table application information
		RecordDesc applyparDesc;
		applyparDesc.addField("t", TpDouble);
		applyparDesc.addField("table", TpString);
		applyparDesc.addField("interp", TpString);
		applyparDesc.addField("spw", TpArrayInt);
		applyparDesc.addField("fieldstr", TpString);
		applyparDesc.addField("calwt", TpBool);
		applyparDesc.addField("spwmap", TpArrayInt);
		applyparDesc.addField("opacity", TpArrayDouble);

		// Create record with the requisite field values
		Record applypar(applyparDesc);
		applypar.define("t", 0.0);
		applypar.define("table", tsystable);
		applypar.define("interp", interp);
		applypar.define("spw", getSpwIdx(""));
		applypar.define("fieldstr", gainfield);
		applypar.define("calwt", true);
		applypar.define("spwmap", spwmap);
		applypar.define("opacity", Vector<Double>(1, 0.0));

		if (vc_p.nelements() > 0) {
			logSink() << LogIO::WARN << "Resetting all calibration application settings." << LogIO::POST;
			unsetapply();
		}
		logSink() << LogIO::NORMAL << "Weight initialization does not support selection. Resetting MS selection." << LogIO::POST;
		selectvis();
		StandardTsys vc = StandardTsys(*msmc_p);
		vc.setApply(applypar);

		logSink() << LogIO::NORMAL << ".   " << vc.applyinfo() << LogIO::POST;
		PtrBlock<VisCal*> vcb(1, &vc);
		// Maintain sort of apply list
		ve_p->setapply(vcb);

		// Detect WEIGHT_SPECTRUM and SIGMA_SPECTRUM
		TableDesc mstd = ms_p->actualTableDesc();
		String colWtSp = MS::columnName(MS::WEIGHT_SPECTRUM);
		Bool wtspexists = mstd.isColumn(colWtSp);
		String colSigSp = MS::columnName(MS::SIGMA_SPECTRUM);
		Bool sigspexists = mstd.isColumn(colSigSp);
		Bool addsigsp = (dowtsp && !sigspexists);

		// Some log info
		bool use_exposure = false;
		if (wtmode == "tsys") {
			logSink()
					<< "Initializing SIGMA and WEIGHT according to channel bandwidth and Tsys. NOTE this is an expert mode."
					<< LogIO::WARN << LogIO::POST;
		} else if (wtmode == "tinttsys") {
			logSink()
					<< "Initializing SIGMA and WEIGHT according to channel bandwidth, integration time, and Tsys. NOTE this is an expert mode."
					<< LogIO::WARN << LogIO::POST;
			use_exposure = true;
		} else {
			throw(AipsError("Unrecognized wtmode specified: " + wtmode));
		}

		// Force dowtsp if the column already exists
		if (wtspexists && !dowtsp) {
			logSink() << "Found WEIGHT_SPECTRUM; will force its initialization."
					<< LogIO::POST;
			dowtsp = true;
		}

		// Report that we are initializing the WEIGHT_SPECTRUM, and prepare to do so.
		if (dowtsp) {

			// Ensure WEIGHT_SPECTRUM really exists at all
			//   (often it exists but is empty)
			if (!wtspexists) {
				logSink() << "Creating WEIGHT_SPECTRUM." << LogIO::POST;

				// Nominal defaulttileshape
				IPosition dts(3, 4, 32, 1024);

				// Discern DATA's default tile shape and use it
				const Record dminfo = ms_p->dataManagerInfo();
				for (uInt i = 0; i < dminfo.nfields(); ++i) {
					Record col = dminfo.asRecord(i);
					//if (upcase(col.asString("NAME"))=="TILEDDATA") {
					if (anyEQ(col.asArrayString("COLUMNS"), String("DATA"))) {
						dts = IPosition(
								col.asRecord("SPEC").asArrayInt(
										"DEFAULTTILESHAPE"));
						//cout << "Found DATA's default tile: " << dts << endl;
						break;
					}
				}

				// Add the column
				String colWtSp = MS::columnName(MS::WEIGHT_SPECTRUM);
				TableDesc tdWtSp;
				tdWtSp.addColumn(
						ArrayColumnDesc<Float>(colWtSp, "weight spectrum", 2));
				TiledShapeStMan wtSpStMan("TiledWgtSpectrum", dts);
				ms_p->addColumn(tdWtSp, wtSpStMan);
			} else
				logSink() << "Found WEIGHT_SPECTRUM." << LogIO::POST;
			// Ensure WEIGHT_SPECTRUM really exists at all
			//   (often it exists but is empty)
		if (!sigspexists) {
				logSink() << "Creating SIGMA_SPECTRUM." << LogIO::POST;

				// Nominal defaulttileshape
				IPosition dts(3, 4, 32, 1024);

				// Discern DATA's default tile shape and use it
				const Record dminfo = ms_p->dataManagerInfo();
				for (uInt i = 0; i < dminfo.nfields(); ++i) {
					Record col = dminfo.asRecord(i);
					//if (upcase(col.asString("NAME"))=="TILEDDATA") {
					if (anyEQ(col.asArrayString("COLUMNS"), String("DATA"))) {
						dts = IPosition(
								col.asRecord("SPEC").asArrayInt(
										"DEFAULTTILESHAPE"));
						//cout << "Found DATA's default tile: " << dts << endl;
						break;
					}
				}

				// Add the column
				String colSigSp = MS::columnName(MS::SIGMA_SPECTRUM);
				TableDesc tdSigSp;
				tdSigSp.addColumn(
						ArrayColumnDesc<Float>(colSigSp, "sigma spectrum", 2));
				TiledShapeStMan sigSpStMan("TiledSigtSpectrum", dts);
				ms_p->addColumn(tdSigSp, sigSpStMan);
				{
				  TableDesc loctd = ms_p->actualTableDesc();
				  String loccolSigSp = MS::columnName(MS::SIGMA_SPECTRUM);
				  AlwaysAssert(loctd.isColumn(loccolSigSp),AipsError);
				}
		}
		}
		else {
    if (sigspexists) {
      logSink() << "Removing SIGMA_SPECTRUM for non-channelized weight." << LogIO::POST;
      if (true || ms_p->canRemoveColumn(colSigSp)) {
	ms_p->removeColumn(colSigSp);
      }
      else
	logSink() << LogIO::WARN << "Failed to remove SIGMA_SPECTRUM column. Values in SIGMA and SIGMA_SPECTRUM columns may be inconsistent after the operation." << LogIO::POST;
    }
		}

		// Arrange for iteration over data
		//  TBD: Be sure this sort is optimal for creating WS?
		Block<Int> columns;
		// include scan iteration
		columns.resize(5);
		columns[0] = MS::ARRAY_ID;
		columns[1] = MS::SCAN_NUMBER;
		columns[2] = MS::FIELD_ID;
		columns[3] = MS::DATA_DESC_ID;
		columns[4] = MS::TIME;

		vi::SortColumns sc(columns);
		vi::VisibilityIterator2 vi2(*ms_p, sc, true);
		vi::VisBuffer2 *vb = vi2.getVisBuffer();

		MSColumns mscol(*ms_p);
		const MSSpWindowColumns& msspw(mscol.spectralWindow());
		uInt nSpw = msspw.nrow();
		Vector<Double> effChBw(nSpw, 0.0);
		for (uInt ispw = 0; ispw < nSpw; ++ispw) {
			effChBw[ispw] = mean(msspw.effectiveBW()(ispw));
		}

		Int ivb(0);
		for (vi2.originChunks(); vi2.moreChunks(); vi2.nextChunk()) {

			for (vi2.origin(); vi2.more(); vi2.next(), ++ivb) {

				Int spw = vb->spectralWindows()(0);

				auto nrow = vb->nRows();
				Int nchan = vb->nChannels();
				Int ncor = vb->nCorrelations();

				// Prepare for WEIGHT_SPECTRUM and SIGMA_SPECTRUM, if nec.
				Cube<Float> newwtsp(0, 0, 0), newsigsp(0, 0, 0);
				if (dowtsp) {
				  newwtsp.resize(ncor, nchan, nrow);
				  newwtsp.set(1.0);
				  newsigsp.resize(ncor, nchan, nrow);
				  newsigsp.set(1.0);
				}

				if (ve_p->spwOK(spw)) {

					// Re-initialize weight info from sigma info
					//   This is smart wrt spectral weights, etc.
					//   (this makes W and WS, if present, "dirty" in the vb)
					// TBD: only do this if !trial (else: avoid the I/O)
					// vb->resetWeightsUsingSigma();
					// Handle non-trivial modes
					// Init WEIGHT, SIGMA  from bandwidth & time
					Matrix<Float> newwt(ncor, nrow), newsig(ncor, nrow);
					newwt.set(1.0);
					newsig.set(1.0);

					// Detect ACs
					const Vector<Int> a1(vb->antenna1());
					const Vector<Int> a2(vb->antenna2());
					Vector<Bool> ac(a1 == a2);

					// XCs need an extra factor of 2
					Vector<Float> xcfactor(nrow, 2.0);
					xcfactor(ac) = 1.0;				// (but not ACs)

					// The row-wise integration time
					Vector<Float> expo(nrow);
					convertArray(expo, vb->exposure());

					// Set weights to channel bandwidth first.
					newwt.set(Float(effChBw(spw)));

					// For each correlation, apply exposure and xcfactor
					for (Int icor = 0; icor < ncor; ++icor) {

						Vector<Float> wt(newwt.row(icor));
						if (use_exposure) {
							wt *= expo;
						}
						wt *= xcfactor;
						if (dowtsp) {
							for (Int ich = 0; ich < nchan; ++ich) {
								Vector<Float> wtspi(
										newwtsp(Slice(icor, 1, 1),
												Slice(ich, 1, 1), Slice()).nonDegenerate(
												IPosition(1, 2)));
								wtspi = wt;
							}
						}
					}
					// Handle SIGMA_SPECTRUM
					if (dowtsp) {
					  newsigsp = 1.0f / sqrt(newwtsp);
					}
					// sig from wt is inverse sqrt
					newsig = 1.0f / sqrt(newwt);

					// Arrange write-back of both SIGMA and WEIGHT
					vb->setSigma(newsig);
					vb->setWeight(newwt);
					if (dowtsp) {
					  vb->initWeightSpectrum(newwtsp);
					  vb->initSigmaSpectrum(newsigsp);
					}
					// Force writeback to disk (need to initialize weight/sigma before applying cal table)
					vb->writeChangesBack();

					// Arrange for _in-place_ apply on CORRECTED_DATA (init from DATA)
					//   (this makes CD "dirty" in the vb)
					// TBD: only do this if !trial (else: avoid the I/O)
					vb->setVisCubeCorrected(vb->visCube());

					// Make flagcube dirty in the vb
					//  NB: we must _always_ do this I/O  (even trial mode)
					vb->setFlagCube(vb->flagCube());

					// Make all vb "not dirty"; we'll carefully arrange the writeback below
					vb->dirtyComponentsClear();

					// throws exception if nothing to apply
					ve_p->correct2(*vb, false, dowtsp);

					if (dowtsp) {
						vb->setWeightSpectrum(vb->weightSpectrum());
						// If WS was calibrated, set W to its channel-axis median
						vb->setWeight( partialMedians(vb->weightSpectrum(), IPosition(1, 1)) );
						newsigsp = 1.0f / sqrt(vb->weightSpectrum());
						vb->initSigmaSpectrum(newsigsp);
						vb->setSigma( partialMedians(newsigsp, IPosition(1, 1)) );
					} else {
						vb->setWeight(vb->weight());
						newsig = 1.0f / sqrt(vb->weight());
						vb->setSigma(newsig);
					}
					// Force writeback to disk
					vb->writeChangesBack();

				} else {//Not calibrating the spw
				  if (dowtsp && !wtspexists) {
				    // newly created WS Need to initialize
				    vb->initWeightSpectrum(newwtsp);
				  }
				  if (addsigsp) {
				    // newly created SS Need to initialize
				    vb->initSigmaSpectrum(newsigsp);
				    vb->writeChangesBack();
				  }
				}
			}
		}
		// clear-up Tsys caltable from list of apply
		unsetapply();

	} catch (AipsError x) {
		logSink() << LogIO::SEVERE << "Caught exception: " << x.getMesg()
				<< LogIO::POST;

		logSink() << "Resetting all calibration application settings."
				<< LogIO::POST;
		unsetapply();

		throw(AipsError("Error in Calibrater::initWeights."));
		retval = false;  // Not that it ever gets here...
	}
	return retval;
}


Bool Calibrater::initWeights(String wtmode, Bool dowtsp) {

  logSink() << LogOrigin("Calibrater","initWeights") << LogIO::NORMAL;
  Bool retval = true;

  try {

    if (!ok())
      throw(AipsError("Calibrater not prepared for initWeights!"));

    // An enum for the requested wtmode
    enum wtInitModeEnum {
      NONE,    // ambiguous, will complain
      ONES,    // SIGMA=1.0, propagate to WEIGHT, WEIGHT_SEPCTRUM
      NYQ,      // SIGMA=f(df,dt), propagate to WEIGHT, WEIGHT_SEPCTRUM
      SIGMA,   // SIGMA as is, propagate to WEIGHT, WEIGHT_SEPCTRUM
      WEIGHT,  // SIGMA, WEIGHT as are, propagate to WEIGHT_SEPCTRUM (if requested)
      DELWTSP,  // just delete WEIGHT_SPECTRUM if it exists
      DELSIGSP,  // just delete SIGMA_SPECTRUM if it exists
    };
    
    // Translate mode String to enum value
    wtInitModeEnum initmode(NONE);
    if (wtmode=="ones") initmode=ONES;
    else if (wtmode=="nyq") initmode=NYQ;
    else if (wtmode=="sigma") initmode=SIGMA;
    else if (wtmode=="weight") initmode=WEIGHT;
    else if (wtmode=="delwtsp") initmode=DELWTSP;
    else if (wtmode=="delsigsp") initmode=DELSIGSP;

    // Detect WEIGHT_SPECTRUM
    TableDesc mstd = ms_p->actualTableDesc();
    String colWtSp=MS::columnName(MS::WEIGHT_SPECTRUM);
    Bool wtspexists=mstd.isColumn(colWtSp);
    String colSigSp=MS::columnName(MS::SIGMA_SPECTRUM);
    Bool sigspexists=mstd.isColumn(colSigSp);

    // Some log info
    switch (initmode) {
    case DELWTSP: {
      if (wtspexists) {
	if (true || ms_p->canRemoveColumn(colWtSp)) {
	  logSink() << "Removing WEIGHT_SPECTRUM." << LogIO::POST;
	  ms_p->removeColumn(colWtSp);
	}
	else
	  logSink() << "Sorry, WEIGHT_SPECTRUM is not removable." << LogIO::POST;
      }
      else
	logSink() << "WEIGHT_SPECTRUM already absent." << LogIO::POST;

      // Nothing more to do here
      return true;
      break;
    }
    case DELSIGSP: {
      if (sigspexists) {
	if (true || ms_p->canRemoveColumn(colSigSp)) {
	  logSink() << "Removing SIGMA_SPECTRUM." << LogIO::POST;
	  ms_p->removeColumn(colSigSp);
	}
	else
	  logSink() << "Sorry, SIGMA_SPECTRUM is not removable." << LogIO::POST;
      }
      else
	logSink() << "SIGMA_SPECTRUM already absent." << LogIO::POST;

      // Nothing more to do here
      return true;
      break;
    }
    case NONE: {
      throw(AipsError("Unrecognized wtmode specified: "+wtmode));
      break;
    }
    case ONES: {
      logSink() << "Initializing SIGMA and WEIGHT with 1.0" << LogIO::POST;
      break;
    }
    case NYQ: {
      logSink() << "Initializing SIGMA and WEIGHT according to channel bandwidth and integration time." 
		<< LogIO::POST;
      break;
    }
    case SIGMA: {
      logSink() << "Initializing WEIGHT according to existing SIGMA." << LogIO::POST;
      break;
    }
    case WEIGHT: {
      // Complain if dowtsp==F, because otherwise we have nothing to do
      if (!dowtsp)
	throw(AipsError("Specified wtmode requires dowtsp=T"));
      break;
    }
    }

    // Force dowtsp if the column already exists
    if (wtspexists && !dowtsp) {
      logSink() << "Found WEIGHT_SPECTRUM; will force its initialization." << LogIO::POST;
      dowtsp=true;
    }
    // remove SIGMA_SPECTRUM column for non-channelized weight
    if (sigspexists) {
      logSink() << "Removing SIGMA_SPECTRUM for non-channelized weight." << LogIO::POST;
      if (true || ms_p->canRemoveColumn(colSigSp)) {
	ms_p->removeColumn(colSigSp);
      }
      else
	logSink() << LogIO::WARN << "Failed to remove SIGMA_SPECTRUM column. Values in SIGMA and SIGMA_SPECTRUM columns may be inconsistent after the operation." << LogIO::POST;
    }

    // Report that we are initializing the WEIGHT_SPECTRUM, and prepare to do so.
    if (dowtsp) {

      // Ensure WEIGHT_SPECTRUM really exists at all 
      //   (often it exists but is empty)
      if (!wtspexists) {
	logSink() << "Creating WEIGHT_SPECTRUM." << LogIO::POST;

	// Nominal defaulttileshape
	IPosition dts(3,4,32,1024); 

	// Discern DATA's default tile shape and use it
	const Record dminfo=ms_p->dataManagerInfo();
	for (uInt i=0;i<dminfo.nfields();++i) {
	  Record col=dminfo.asRecord(i);
	  //if (upcase(col.asString("NAME"))=="TILEDDATA") {
	  if (anyEQ(col.asArrayString("COLUMNS"),String("DATA"))) {
	    dts=IPosition(col.asRecord("SPEC").asArrayInt("DEFAULTTILESHAPE"));
	    //cout << "Found DATA's default tile: " << dts << endl;
	    break;
	  }
	}

	// Add the column
	String colWtSp=MS::columnName(MS::WEIGHT_SPECTRUM);
	TableDesc tdWtSp;
	tdWtSp.addColumn(ArrayColumnDesc<Float>(colWtSp,"weight spectrum", 2));
	TiledShapeStMan wtSpStMan("TiledWgtSpectrum",dts);
	ms_p->addColumn(tdWtSp,wtSpStMan);
      }
      else
	logSink() << "Found WEIGHT_SPECTRUM." << LogIO::POST;


      logSink() << "Initializing WEIGHT_SPECTRUM uniformly in channel (==WEIGHT)." << LogIO::POST;

    }

    // Arrange for iteration over data
    //  TBD: Be sure this sort is optimal for creating WS?
    Block<Int> columns;
    // include scan iteration
    columns.resize(5);
    columns[0]=MS::ARRAY_ID;
    columns[1]=MS::SCAN_NUMBER;
    columns[2]=MS::FIELD_ID;
    columns[3]=MS::DATA_DESC_ID;
    columns[4]=MS::TIME;

    vi::SortColumns sc(columns);
    vi::VisibilityIterator2 vi2(*ms_p,sc,true);
    vi::VisBuffer2 *vb = vi2.getVisBuffer();

    MSColumns mscol(*ms_p);
    const MSSpWindowColumns& msspw(mscol.spectralWindow());
    uInt nSpw=msspw.nrow();
    Vector<Double> effChBw(nSpw,0.0);
    for (uInt ispw=0;ispw<nSpw;++ispw) {
      effChBw[ispw]=mean(msspw.effectiveBW()(ispw));
    }

    Int ivb(0);
    for (vi2.originChunks(); vi2.moreChunks(); vi2.nextChunk()) {

      for (vi2.origin(); vi2.more(); vi2.next(),++ivb) {

	Int spw = vb->spectralWindows()(0);

	auto nrow=vb->nRows();
	Int nchan=vb->nChannels();
	Int ncor=vb->nCorrelations();

	// Vars for new sigma/weight info

	// Prepare for WEIGHT_SPECTRUM, if nec.
	Cube<Float> newwtsp(0,0,0);
	if (dowtsp) {
	  newwtsp.resize(ncor,nchan,nrow);
	  newwtsp.set(1.0);
	}

	// Handle non-trivial modes
	switch (initmode) {
	// Init WEIGHT, SIGMA  with ones
	case ONES: {

	  Matrix<Float> newwt(ncor,nrow),newsig(ncor,nrow);
	  newwt.set(1.0);
	  newsig.set(1.0);

	  // Arrange write-back of both SIGMA and WEIGHT
	  vb->setSigma(newsig);
	  vb->setWeight(newwt);

	  // NB: newwtsp already ready

	  break;
	}	  

	// Init WEIGHT, SIGMA  from bandwidth & time
	case NYQ: {

	  Matrix<Float> newwt(ncor,nrow),newsig(ncor,nrow);
	  newwt.set(1.0);
	  newsig.set(1.0);

	  // Detect ACs
	  const Vector<Int> a1(vb->antenna1());
	  const Vector<Int> a2(vb->antenna2());
	  Vector<Bool> ac(a1==a2);
	  
	  // XCs need an extra factor of 2
	  Vector<Float> xcfactor(nrow,2.0);
	  xcfactor(ac)=1.0;   // (but not ACs)
	  
	  // The row-wise integration time
	  Vector<Float> expo(nrow);
	  convertArray(expo,vb->exposure());

	  // Set weights to channel bandwidth first.
	  newwt.set(Float(effChBw(spw)));
	  
	  // For each correlation, apply exposure and xcfactor
	  for (Int icor=0;icor<ncor;++icor) {

	    Vector<Float> wt(newwt.row(icor));
	    wt*=expo;
	    wt*=xcfactor;
	    if (dowtsp) {
	      for (Int ich=0;ich<nchan;++ich) {
		Vector<Float> wtspi(newwtsp(Slice(icor,1,1),Slice(ich,1,1),Slice()).nonDegenerate(IPosition(1,2)));
		wtspi=wt;
	      }
	    }
	  }

	  // sig from wt is inverse sqrt
	  newsig=1.0f/sqrt(newwt);

	  // Arrange write-back of both SIGMA and WEIGHT
	  vb->setSigma(newsig);
	  vb->setWeight(newwt);

	  break;
	}
	// Init WEIGHT from SIGMA 
	case SIGMA: {

	  Matrix<Float> newwt(ncor,nrow);
	  newwt.set(FLT_EPSILON);        // effectively zero, but strictly not zero

	  Matrix<Float> sig;
	  sig.assign(vb->sigma());       // access existing SIGMA

	  LogicalArray mask(sig==0.0f);  // mask out unphysical SIGMA
	  sig(mask)=1.0f;

	  newwt=1.0f/square(sig);
	  newwt(mask)=FLT_EPSILON;       // effectively zero

	  // Writeback WEIGHT
	  vb->setWeight(newwt);
	  
	  if (dowtsp) {
	    for (Int ich=0;ich<nchan;++ich) {
	      Matrix<Float> wtspi(newwtsp(Slice(),Slice(ich,1,1),Slice()).nonDegenerate(IPosition(2,0,2)));
	      wtspi=newwt;
	    }
	  }

	  break;
	}
	// Init WEIGHT_SPECTRUM from WEIGHT
	case WEIGHT: {
	  const Matrix<Float> wt(vb->weight()); // access existing WEIGHT
	  for (Int ich=0;ich<nchan;++ich) {
	    Matrix<Float> wtspi(newwtsp(Slice(),Slice(ich,1,1),Slice()).nonDegenerate(IPosition(2,0,2)));
	    wtspi=wt;
	  }
	  break;
	} 
	default: {
	  throw(AipsError("Problem in weight initialization loop."));
	}
	}

	// Force writeback to disk
	vb->writeChangesBack();
	
	// Handle WEIGHT_SPECTRUM
	if (dowtsp)
	  vb->initWeightSpectrum(newwtsp);

      }
    }

  }
  catch (AipsError x) {
    logSink() << LogIO::SEVERE << "Caught exception: " << x.getMesg()
             << LogIO::POST;

    logSink() << "Resetting all calibration application settings." << LogIO::POST;
    unsetapply();

    throw(AipsError("Error in Calibrater::initWeights."));
    retval = false;  // Not that it ever gets here...
  }
  return retval;
}


Bool Calibrater::initWeights(Bool doBT, Bool dowtsp) {

  logSink() << LogOrigin("Calibrater","initWeights") << LogIO::NORMAL;
  Bool retval = true;

  try {

    if (!ok())
      throw(AipsError("Calibrater not prepared for initWeights!"));

    // Log that we are beginning...
    if (doBT)
      logSink() << "Initializing SIGMA and WEIGHT according to channel bandwidth and integration time." << LogIO::POST;
    else
      logSink() << "Initializing SIGMA and WEIGHT to unity." << LogIO::POST;

    if (dowtsp) {
      logSink() << "Also initializing WEIGHT_SPECTRUM uniformly in channel (==WEIGHT)." << LogIO::POST;

      // Ensure WEIGHT_SPECTRUM really exists at all 
      //   (often it exists but is empty)
      TableDesc mstd = ms_p->actualTableDesc();
      if (!mstd.isColumn("WEIGHT_SPECTRUM")) {
	cout << "Creating WEIGHT_SPECTRUM!" << endl;

	// Nominal defaulttileshape
	IPosition dts(3,4,32,1024); 

	// Discern DATA's default tile shape and use it
	const Record dminfo=ms_p->dataManagerInfo();
	for (uInt i=0;i<dminfo.nfields();++i) {
	  Record col=dminfo.asRecord(i);
	  //if (upcase(col.asString("NAME"))=="TILEDDATA") {
	  if (anyEQ(col.asArrayString("COLUMNS"),String("DATA"))) {
	    dts=IPosition(col.asRecord("SPEC").asArrayInt("DEFAULTTILESHAPE"));
	    cout << "Found DATA's default tile: " << dts << endl;
	    break;
	  }
	}

	// Add the column
	String colWtSp=MS::columnName(MS::WEIGHT_SPECTRUM);
	TableDesc tdWtSp;
	tdWtSp.addColumn(ArrayColumnDesc<Float>(colWtSp,"weight spectrum", 2));
	TiledShapeStMan wtSpStMan("TiledWgtSpectrum",dts);
	ms_p->addColumn(tdWtSp,wtSpStMan);
      }
    }

    // Arrange for iteration over data
    Block<Int> columns;
    // include scan iteration
    columns.resize(5);
    columns[0]=MS::ARRAY_ID;
    columns[1]=MS::SCAN_NUMBER;
    columns[2]=MS::FIELD_ID;
    columns[3]=MS::DATA_DESC_ID;
    columns[4]=MS::TIME;

    vi::SortColumns sc(columns);
    vi::VisibilityIterator2 vi2(*ms_p,sc,true);
    vi::VisBuffer2 *vb = vi2.getVisBuffer();

    MSColumns mscol(*ms_p);
    const MSSpWindowColumns& msspw(mscol.spectralWindow());
    uInt nSpw=msspw.nrow();
    Vector<Double> effChBw(nSpw,0.0);
    for (uInt ispw=0;ispw<nSpw;++ispw) {
      effChBw[ispw]=mean(msspw.effectiveBW()(ispw));
    }

    Int ivb(0);
    for (vi2.originChunks(); vi2.moreChunks(); vi2.nextChunk()) {

      for (vi2.origin(); vi2.more(); vi2.next()) {

	Int spw = vb->spectralWindows()(0);

	auto nrow=vb->nRows();
	Int nchan=vb->nChannels();
	Int ncor=vb->nCorrelations();

	Matrix<Float> newwt(ncor,nrow),newsig(ncor,nrow);
	newwt.set(1.0);
	newsig.set(1.0);

	Cube<Float> newwtsp(0,0,0);
	if (dowtsp) {
	  newwtsp.resize(ncor,nchan,nrow);
	  newwtsp.set(1.0);
	}

	// If requested, set weights according to bandwidth and integration time
	if (doBT) {
	  
	  // Detect ACs
	  const Vector<Int> a1(vb->antenna1());
	  const Vector<Int> a2(vb->antenna2());
	  Vector<Bool> ac(a1==a2);
	  
	  // XCs need an extra factor of 2
	  Vector<Float> xcfactor(nrow,2.0);
	  xcfactor(ac)=1.0;   // (but not ACs)
	  
	  // The row-wise integration time
	  Vector<Float> expo(nrow);
	  convertArray(expo,vb->exposure());

	  // Set weights to channel bandwidth first.
	  newwt.set(Float(effChBw(spw)));
	  
	  // For each correlation, apply exposure and xcfactor
	  for (Int icor=0;icor<ncor;++icor) {

	    Vector<Float> wt(newwt.row(icor));
	    wt*=expo;
	    wt*=xcfactor;
	    if (dowtsp) {
	      for (Int ich=0;ich<nchan;++ich) {
		Vector<Float> wtsp(newwtsp(Slice(icor,1,1),Slice(ich,1,1),Slice()));
		wtsp=wt;
	      }
	    }
	    
	  }

	  // sig from wt is inverse sqrt
	  newsig=newsig/sqrt(newwt);

	}

	/*
	  cout << ivb << " "
	  << ncor << " " << nchan << " " << nrow << " "
	  << expo(0) << " "
	  << newwt(0,0) << " "
	  << newsig(0,0) << " "
	  << endl;
	*/
	++ivb;

	// Set in vb, and writeback
	vb->setWeight(newwt);
	vb->setSigma(newsig);
	vb->writeChangesBack();
	
	if (dowtsp)
	  vb->initWeightSpectrum(newwtsp);

      }
    }

  }
  catch (AipsError x) {
    logSink() << LogIO::SEVERE << "Caught exception: " << x.getMesg()
             << LogIO::POST;

    logSink() << "Resetting all calibration application settings." << LogIO::POST;
    unsetapply();

    throw(AipsError("Error in Calibrater::initWeightsOLD."));
    retval = false;  // Not that it ever gets here...
  }
  return retval;
}


Bool Calibrater::solve() {

  logSink() << LogOrigin("Calibrater","solve") << LogIO::NORMAL3;

  try {

    if (!ok()) 
      throw(AipsError("Calibrater not prepared for solve."));

    // Handle nothing-to-solve-for case
    if (!svc_p)
      throw(AipsError("Please run setsolve before attempting to solve."));

    // Handle specified caltable
    if (svc_p) {

      // If table exists (readable) and not deletable
      //   we have to abort (append=T requires deletable)
      if ( Table::isReadable(svc_p->calTableName()) &&
	   !Table::canDeleteTable(svc_p->calTableName()) ) {
	throw(AipsError("Specified caltable ("+svc_p->calTableName()+") exists and\n cannot be replaced (or appended to) because it appears to be open somewhere (Quit plotcal?)."));
      }
    }

    // Arrange VisEquation for solve
    ve_p->setsolve(*svc_p);

    // Ensure apply list properly sorted w.r.t. solvable term
    ve_p->setapply(vc_p);

    // Report what is being applied and solved-for
    applystate();
    solvestate();

    // Report correct/corrupt apply order
    //    ve_p->state();

    // Generally use standard solver
    if (svc_p->useGenericGatherForSolve())
      genericGatherAndSolve();  // VI2-driven, SDBs
    else {
      //cout << "Fully self-directed data gather and solve" << endl;
      // Fully self-directed data gather and solve
      throw(AipsError("Can't do selfGatherAndSolve for "+svc_p->typeName()));
      //svc_p->selfGatherAndSolve(*vs_p,*ve_p);
    }

  } catch (AipsError x) {
    logSink() << LogIO::SEVERE << "Caught exception: " << x.getMesg() << LogIO::POST;

    logSink() << "Reseting entire solve/apply state." << LogIO::POST;
    reset();

    throw(AipsError("Error in Calibrater::solve."));
    return false;
  } 

  return true;

}


Bool Calibrater::summarize_uncalspws(const Vector<Bool>& uncalspw,
				     const String& origin,
				     Bool strictflag)
{
  Bool hadprob = false;
  uInt totNspw = uncalspw.nelements();

  for(uInt i = 0; i < totNspw; ++i){
    if(uncalspw[i]){
      hadprob = true;
      break;
    }
  }
  if(hadprob){
    logSink() << LogIO::WARN
	      << "Spectral window(s) ";
    for(uInt i = 0; i < totNspw; ++i){
      if(uncalspw[i]){
	logSink() << i << ", ";
      }
    }
    logSink() << "\n  could not be " << origin << "ed due to missing (pre-)calibration\n"
	      << "    in one or more of the specified tables.\n";
    if (strictflag)
      logSink() << "    These spws have been flagged!";
    else
      logSink() << "    Please check your results carefully!";

    logSink() << LogIO::POST;
  }
  return !hadprob;
}

void Calibrater::fluxscale(const String& infile, 
			   const String& outfile,
			   const String& refFields, 
			   const Vector<Int>& refSpwMap, 
			   const String& tranFields,
			   const Bool& append,
                           const Float& inGainThres,
                           const String& antSel,
                           const String& timerangeSel,
                           const String& scanSel,
			   SolvableVisCal::fluxScaleStruct& oFluxScaleFactor,
			   Vector<Int>& tranidx,
			   const String& oListFile,
                           const Bool& incremental,
                           const Int& fitorder,
                           const Bool& display) {

  // TBD:  Permit more flexible matching on specified field names
  //  (Currently, exact matches are required.)

  logSink() << LogOrigin("Calibrater","fluxscale") << LogIO::NORMAL3;

  //outfile check
  if (outfile=="") 
    throw(AipsError("output fluxscaled caltable name must be specified!"));
  else {
    if (File(outfile).exists() && !append) 
      throw(AipsError("output caltable name, "+outfile+" exists. Please specify a different caltable name"));
  }

  // Convert refFields/transFields to index lists
  Vector<Int> refidx(0);

  if (refFields.length()>0)
    refidx=getFieldIdx(refFields);
  else
    throw(AipsError("A reference field must be specified!"));

  if (tranFields.length()>0)
    tranidx=getFieldIdx(tranFields);

  // Call Vector<Int> version:
  fluxscale(infile,outfile,refidx,refSpwMap,tranidx,append,inGainThres,antSel,timerangeSel,
      scanSel,oFluxScaleFactor,oListFile,incremental,fitorder,display);

}

void Calibrater::fluxscale(const String& infile, 
			   const String& outfile,
			   const Vector<Int>& refField, 
			   const Vector<Int>& refSpwMap, 
			   const Vector<Int>& tranField,
			   const Bool& append,
			   const Float& inGainThres,
			   const String& antSel,
			   const String& timerangeSel,
			   const String& scanSel,
			   SolvableVisCal::fluxScaleStruct& oFluxScaleFactor,
			   const String& oListFile,
			   const Bool& incremental,
			   const Int& fitorder,
			   const Bool& display) {
  
  //  throw(AipsError("Method 'fluxscale' is temporarily disabled."));

  // TBD: write inputs to MSHistory
  logSink() << LogOrigin("Calibrater","fluxscale") << LogIO::NORMAL3;

  SolvableVisCal *fsvj_(NULL);
  try {
    // If infile is Calibration table
    if (Table::isReadable(infile) && 
	Table::tableInfo(infile).type()=="Calibration") {

      // get calibration type
      String caltype;
      caltype = Table::tableInfo(infile).subType();
      logSink() << "Table " << infile 
		<< " is of type: "<< caltype 
		<< LogIO::POST;
      String message="Table "+infile+" is of type: "+caltype;
      MSHistoryHandler::addMessage(*ms_p, message, "calibrater", "", "calibrater::fluxscale()");
      
      // form selection
      String select="";
      // Selection is empty for case of no tran specification
      if (tranField.nelements()>0) {
	
	// All selected fields
	Vector<Int> allflds = concatenateArray(refField,tranField);
	
	// Assemble TaQL
	ostringstream selectstr;
	selectstr << "FIELD_ID IN [";
	for (Int iFld=0; iFld<allflds.shape(); iFld++) {
	  if (iFld>0) selectstr << ", ";
	  selectstr << allflds(iFld);
	}
	selectstr << "]";
	select=selectstr.str();
      }

      // Construct proper SVC object
      if (caltype == "G Jones") {
	fsvj_ = createSolvableVisCal("G",*msmc_p);
      } else if (caltype == "T Jones") {
	fsvj_ = createSolvableVisCal("T",*msmc_p);
      } else {
	// Can't process other than G and T (add B?)
	ostringstream typeErr;
	typeErr << "Type " << caltype 
	       << " not supported in fluxscale.";

	throw(AipsError(typeErr.str()));
      }

      // fill table with selection
      RecordDesc applyparDesc;
      applyparDesc.addField ("table", TpString);
      applyparDesc.addField ("select", TpString);
      Record applypar(applyparDesc);
      applypar.define ("table", infile);
      applypar.define ("select", select);
      fsvj_->setApply(applypar);

      //Bool incremental=false;
      // Make fluxscale calculation
      Vector<String> fldnames(MSFieldColumns(ms_p->field()).name().getColumn());
      //fsvj_->fluxscale(refField,tranField,refSpwMap,fldnames,oFluxScaleFactor,
      fsvj_->fluxscale(outfile,refField,tranField,refSpwMap,fldnames,inGainThres,antSel,
        timerangeSel,scanSel,oFluxScaleFactor, oListFile,incremental,fitorder,display);
//        oListFile);
     
      // If no outfile specified, use infile (overwrite!)
      String out(outfile);
      if (out.length()==0)
	out = infile;

      // Store result
      if (append) {
	logSink() << "Appending result to " << out << LogIO::POST;
	String message="Appending result to "+out;
	MSHistoryHandler::addMessage(*ms_p, message, "calibrater", "", "calibrater::fluxscale()");
      } else {
	logSink() << "Storing result in " << out << LogIO::POST;
	String message="Storing result in "+out;
	MSHistoryHandler::addMessage(*ms_p, message, "calibrater", "", "calibrater::fluxscale()");
      }
      fsvj_->storeNCT(out,append);
      
      // Clean up
      delete fsvj_;

    } else {
      // Table not found/unreadable, or not Calibration
      ostringstream tabErr;
      tabErr << "File " << infile
	     << " does not exist or is not a Calibration Table.";

      throw(AipsError(tabErr.str()));

    }
  } catch (AipsError x) {

    logSink() << LogIO::SEVERE
	      << "Caught Exception: "
	      << x.getMesg()
	      << LogIO::POST;
    
    // Clean up
    if (fsvj_) delete fsvj_;

    // Write to MS History table
    //    String message="Caught Exception: "+x.getMesg();
    //    MSHistoryHandler::addMessage(*ms_p, message, "calibrater", "", "calibrater::fluxscale()");

    throw(AipsError("Error in Calibrater::fluxscale."));

    return;

  }
  return;


}

void Calibrater::specifycal(const String& type,
			    const String& caltable,
			    const String& time,
			    const String& spw,
			    const String& antenna,
			    const String& pol,
			    const Vector<Double>& parameter,
			    const String& infile,
			    const Bool& uniform) {

  logSink() << LogOrigin("Calibrater","specifycal") << LogIO::NORMAL;

  // SVJ objects:
  SolvableVisCal *cal_(NULL);

  try {
 			    
    // Set record format for calibration table application information
    RecordDesc specifyDesc;
    specifyDesc.addField ("caltable", TpString);
    specifyDesc.addField ("time", TpString);
    specifyDesc.addField ("spw", TpArrayInt);
    specifyDesc.addField ("antenna", TpArrayInt);
    specifyDesc.addField ("pol", TpString);
    specifyDesc.addField ("parameter", TpArrayDouble);
    specifyDesc.addField ("caltype",TpString);
    specifyDesc.addField ("infile",TpString);
    specifyDesc.addField ("uniform",TpBool);

    // Create record with the requisite field values
    Record specify(specifyDesc);
    specify.define ("caltable", caltable);
    specify.define ("time", time);
    if (spw=="*")
      specify.define ("spw",Vector<Int>(1,-1));
    else
      specify.define ("spw",getSpwIdx(spw));
    if (antenna=="*")
      specify.define ("antenna",Vector<Int>(1,-1) );
    else
      specify.define ("antenna",getAntIdx(antenna));
    specify.define ("pol",pol);
    specify.define ("parameter",parameter);
    specify.define ("caltype",type);
    specify.define ("infile",infile);
    specify.define ("uniform",uniform);

    // Now do it
    String utype=upcase(type);
    if (utype=="G" || utype.contains("AMP") || utype.contains("PH"))
      cal_ = createSolvableVisCal("G",*msmc_p);
    else if (utype=='K' || utype.contains("SBD") || utype.contains("DELAY"))
      cal_ = createSolvableVisCal("K",*msmc_p);
    else if (utype.contains("MBD"))
      cal_ = createSolvableVisCal("K",*msmc_p);  // As of 5.3, MBD is ordinary K
    else if (utype.contains("ANTPOS"))
      cal_ = createSolvableVisCal("KANTPOS",*msmc_p);
    else if (utype.contains("TSYS"))
      cal_ = createSolvableVisCal("TSYS",*msmc_p);
    else if (utype.contains("EVLAGAIN") ||
	     utype.contains("SWP") ||
	     utype.contains("RQ"))
      cal_ = createSolvableVisCal("EVLASWP",*msmc_p);
    else if (utype.contains("OPAC"))
      cal_ = createSolvableVisCal("TOPAC",*msmc_p);
    else if (utype.contains("GC") && ms_p && ms_p->keywordSet().isDefined("GAIN_CURVE"))
      cal_ = createSolvableVisCal("POWERCURVE",*msmc_p);
    else if (utype.contains("GC") || utype.contains("EFF"))
      cal_ = createSolvableVisCal("GAINCURVE",*msmc_p);
    else if (utype.contains("TEC"))
      cal_ = createSolvableVisCal("TEC",*msmc_p);
    else if (utype.contains("SDSKY_PS"))
      cal_ = createSolvableVisCal("SDSKY_PS",*msmc_p);
    else if (utype.contains("SDSKY_RASTER"))
      cal_ = createSolvableVisCal("SDSKY_RASTER",*msmc_p);
    else if (utype.contains("SDSKY_OTF"))
      cal_ = createSolvableVisCal("SDSKY_OTF",*msmc_p);
    else
      throw(AipsError("Unrecognized caltype."));

    // set up for specification (set up the CalSet)
    cal_->setSpecify(specify);

    // fill with specified values
    cal_->specify(specify);

    // Store result
    cal_->storeNCT();

    delete cal_;

  } catch (AipsError x) {
    logSink() << LogIO::SEVERE
	      << "Caught Exception: "
	      << x.getMesg()
	      << LogIO::POST;

    if (cal_) delete cal_;
    
    throw(AipsError("Error in Calibrater::specifycal."));
    return;
  }
  return;

}


Bool Calibrater::smooth(const String& infile, 
			String& outfile,  // const Bool& append,
			const String& smoothtype,
			const Double& smoothtime,
			const String& fields)
{

  // TBD: support append?
  // TBD: spw selection?

  logSink() << LogOrigin("Calibrater","smooth") << LogIO::NORMAL;

  logSink() << "Beginning smoothing/interpolating method." << LogIO::POST;


  // A pointer to an SVC
  SolvableVisCal *svc(NULL);

  try {
    
    // Handle no in file 
    if (infile=="")
      throw(AipsError("Please specify an input calibration table."));

    // Handle bad smoothtype
    if (smoothtype!="mean" && smoothtype!="median")
      throw(AipsError("Unrecognized smooth type!"));

    // Handle bad smoothtime
    if (smoothtime<=0)
      throw(AipsError("Please specify a strictly positive smoothtime."));

    // Handle no outfile
    if (outfile=="") {
      outfile=infile;
      logSink() << "Will overwrite input file with smoothing result." 
		<< LogIO::POST;
    }


    svc = createSolvableVisCal(calTableType(infile),*msmc_p);
    
    if (svc->smoothable()) {
      
      // Fill calibration table using setApply
      RecordDesc applyparDesc;
      applyparDesc.addField ("table", TpString);
      Record applypar(applyparDesc);
      applypar.define ("table", infile);
      svc->setApply(applypar);

      // Convert refFields/transFields to index lists
      Vector<Int> fldidx(0);
      if (fields.length()>0)
	fldidx=getFieldIdx(fields);

      // Delegate to SVC
      svc->smooth(fldidx,smoothtype,smoothtime);
      
      // Store the result on disk
      //    if (append) logSink() << "Appending result to " << outfile << LogIO::POST;
      //else 
      logSink() << "Storing result in " << outfile << LogIO::POST;
      
      
      if (outfile != "") 
	svc->calTableName()=outfile;
      svc->storeNCT();

      // Clean up
      if (svc) delete svc; svc=NULL;
      
      // Apparently, it worked
      return true;

    }
    else
      throw(AipsError("This type ("+svc->typeName()+") does not support smoothing."));

  } catch (AipsError x) {
   
    logSink() << LogIO::SEVERE
	      << "Caught Exception: "
	      << x.getMesg()
	      << LogIO::POST;
    // Clean up
    if (svc) delete svc; svc=NULL;

    throw(AipsError("Error in Calibrater::smooth."));

    return false;
  }
  return false;
}

// Apply new reference antenna to calibration
Bool Calibrater::reRefant(const casacore::String& infile,
			  casacore::String& outfile, 
			  const casacore::String& refantmode, 
			  const casacore::String& refant)
{

  logSink() << LogOrigin("Calibrater","reRefant") << LogIO::NORMAL;

  //  logSink() << "Beginning smoothing/interpolating method." << LogIO::POST;


  // A pointer to an SVC
  SolvableVisJones *svj(NULL);

  try {
    
    // Handle no in file 
    if (infile=="")
      throw(AipsError("Please specify an input calibration table."));

    // Handle bad refantmode
    if (refantmode!="strict" && 
	refantmode!="flex")
      throw(AipsError("Unrecognized refantmode!"));

    // Handle no outfile
    if (outfile=="") {
      outfile=infile;
      logSink() << "Will overwrite input file with smoothing result." 
		<< LogIO::POST;
    }


    svj = (SolvableVisJones*) createSolvableVisCal(calTableType(infile),*msmc_p);
    
    // Fill calibration table using setApply
    RecordDesc applyparDesc;
    applyparDesc.addField ("table", TpString);
    Record applypar(applyparDesc);
    applypar.define ("table", infile);
    svj->setApply(applypar);

    // Do the work
    svj->refantmode() = refantmode;
    svj->refantlist().reference(getRefantIdxList(refant));   // replaces the default list
    svj->applyRefAnt();

    // Store the result on disk
    logSink() << "Storing result in " << outfile << LogIO::POST;
      
    if (outfile != "") 
      svj->calTableName()=outfile;
    svj->storeNCT();

    // Clean up
    if (svj) delete svj; svj=NULL;
      
    // Apparently, it worked
    return true;

  } catch (AipsError x) {
   
    logSink() << LogIO::SEVERE
	      << "Caught Exception: "
	      << x.getMesg()
	      << LogIO::POST;
    // Clean up
    if (svj) delete svj; svj=NULL;

    throw(AipsError("Error in Calibrater::reRefant."));
    
    return false;
  }
  return false;
}


// List a calibration table
Bool Calibrater::listCal(const String& infile,
			 const String& field,
			 const String& antenna,
			 const String& spw,
			 const String& listfile,
			 const Int& pagerows) {
    
    SolvableVisCal *svc(NULL);
    logSink() << LogOrigin("Calibrater","listCal");
    
    try {
        
        // Trap (currently) unsupported types
        if (upcase(calTableType(infile))=="GSPLINE" ||
            upcase(calTableType(infile))=="BPOLY")
            throw(AipsError("GSPLINE and BPOLY tables cannot currently be listed."));
        
        // Get user's selected fields, ants
        Vector<Int> ufldids=getFieldIdx(field);
        Vector<Int> uantids=getAntIdx(antenna);
        
        String newSpw = spw;
        Bool defaultSelect = false;
        if (spw.empty()) { // list all channels (default)
            defaultSelect = true;
            newSpw = "*"; 
            logSink() << LogIO::NORMAL1 << "Spws selected: ALL" << endl
                      << "Channels selected: ALL" << LogIO::POST;
        }
        // Get user's selected spw and channels
        Vector<Int> uspwids=getSpwIdx(newSpw);
        Matrix<Int> uchanids=getChanIdx(newSpw);
        if (!defaultSelect) {
            logSink() << LogIO::NORMAL1 << "Spw and Channel selection matrix: "
                      << endl << "Each rows shows: [ Spw , Start Chan , Stop Chan , Chan Step ]"
                      << endl << uchanids << LogIO::POST;
        }
        logSink() << LogIO::DEBUG2 
                  << "uspwids = "  << uspwids  << endl
                  << "uchanids = " << uchanids << LogIO::POST;
        
        // By default, do first spw, first chan
        if (uspwids.nelements()==0) {
            uchanids.resize(1,4);
            uchanids=0;
        } 
        
        // Set record format for calibration table application information
        RecordDesc applyparDesc;
        applyparDesc.addField ("table", TpString);
        
        // Create record with the requisite field values
        Record applypar(applyparDesc);
        applypar.define ("table", infile);
        
        // Generate the VisCal to be listed
        svc = createSolvableVisCal(calTableType(infile),*msmc_p);  
        svc->setApply(applypar);       
        
        // list it
        svc->listCal(ufldids,uantids,uchanids,  //uchanids(0,0),uchanids(0,1),
                     listfile,pagerows);
        
        if (svc) delete svc; svc=NULL;
        
        return true;
        
    } catch (AipsError x) {
        
        logSink() << LogIO::SEVERE
                  << "Caught Exception: "
                  << x.getMesg()
                  << LogIO::POST;
        // Clean up
        if (svc) delete svc; svc=NULL;
        
        throw(AipsError("Error in Calibrater::listCal."));
        
        return false;
    }
    return false;
    
}



Bool Calibrater::updateCalTable(const String& caltable) {

  // Call the SVC method that knows how
  return NewCalTable::CTBackCompat(caltable);

}

void Calibrater::selectChannel(const String& spw) {

  if (mss_p && mssel_p) {

    // Refresh the frequencySelections object to feed to VI2, if relevant
    frequencySelections_p.reset(new vi::FrequencySelections());

    vi::FrequencySelectionUsingChannels usingChannels;
    usingChannels.add(*mss_p,mssel_p);
    frequencySelections_p->add(usingChannels);

    //    cout << usingChannels.toString() << endl;
    //    cout << "FS.size() = " << frequencySelections_p->size() << endl;

  }

  // TBD:  Does frequencySelections_p support string info for reporting to logger?

  Matrix<Int> chansel = getChanIdx(spw);
  uInt nselspw=chansel.nrow();

  if (nselspw==0)
    logSink() << "Frequency selection: Selecting all channels in all spws." 
	      << LogIO::POST;
  else {

    logSink() << "Frequency selection: " << LogIO::POST;

    // Trap non-unit step (for now)
    if (ntrue(chansel.column(3)==1)!=nselspw) {
      logSink() << LogIO::WARN
		<< "Calibration does not support non-unit channel stepping; "
		<< "using step=1."
		<< LogIO::POST;
      chansel.column(3)=1;
    }

    Vector<Int> uspw(chansel.column(0));
    Vector<Int> ustart(chansel.column(1));
    Vector<Int> uend(chansel.column(2));
    Vector<Int> ustep(chansel.column(3));

    logSink() << LogIO::NORMAL;
    for (uInt i=0;i<nselspw;++i) {

      logSink() << ".  Spw " << uspw(i) << ":"
		<< ustart(i) << "~" << uend(i) 
		<< " (" << uend(i)-ustart(i)+1 << " channels,"
		<< " step by " << ustep(i) << ")"
		<< endl;
	
    } // i
    logSink() << LogIO::POST;

  } // non-triv spw selection

}


Bool Calibrater::cleanup() {

  //  logSink() << LogOrigin("Calibrater","cleanup") << LogIO::NORMAL;

  // Delete the VisCals
  reset();

  // Delete derived dataset stuff
  if(mssel_p) delete mssel_p; mssel_p=0;
  if(mss_p) delete mss_p; mss_p=0;
  frequencySelections_p.reset();

  // Delete the current VisEquation
  if(ve_p) delete ve_p; ve_p=0;

  return true;

}

// Parse refant specification
Vector<Int> Calibrater::getRefantIdxList(const String& refant) {

  Vector<Int> irefant;
  if (refant.length()==0) {
    // Nothing specified, return -1 in single-element vector
    irefant.resize(1);
    irefant(0)=-1;
  }
  else {
    // parse the specification
    MSSelection msselect;
    msselect.setAntennaExpr(refant);
    Vector<Int> iref=msselect.getAntenna1List(mssel_p);
    if (anyLT(iref,0))
      cout << "Negated selection (via '!') not yet implemented for refant," << endl << " and will be ignored." << endl;
    irefant=iref(iref>-1).getCompressedArray();
    if (irefant.nelements()==0) {
      irefant.resize(1);
      irefant(0)=-1;
    }
  }

  return irefant;
}


// Interpret refant *index*
Vector<Int> Calibrater::getAntIdx(const String& antenna) {

    MSSelection msselect;
    msselect.setAntennaExpr(antenna);
    return msselect.getAntenna1List(mssel_p);

}

// Interpret field indices (MSSelection)
Vector<Int> Calibrater::getFieldIdx(const String& fields) {

  MSSelection mssel;
  mssel.setFieldExpr(fields);
  return mssel.getFieldList(mssel_p);

}

// Interpret spw indices (MSSelection)
Vector<Int> Calibrater::getSpwIdx(const String& spws) {

  MSSelection mssel;
  mssel.setSpwExpr(spws);

  /*
  cout << "mssel.getSpwList(mssel_p) = " << mssel.getSpwList(mssel_p) << endl;
  cout << "mssel.getChanList(mssel_p) = " << mssel.getChanList(mssel_p) << endl;  cout << "Vector<Int>() = " << Vector<Int>() << endl;
  */

  // Use getChanList (column 0 for spw) because it is 
  //  more reliable about the number and order of specified spws.
  //  return mssel.getSpwList(mssel_p);
  Matrix<Int> chanmat=mssel.getChanList(mssel_p);
  if (chanmat.nelements()>0) 
    return chanmat.column(0);
  else
    return Vector<Int>();

}

Matrix<Int> Calibrater::getChanIdx(const String& spw) {

  MSSelection mssel;
  mssel.setSpwExpr(spw);

  return mssel.getChanList(mssel_p);

}


// Query apply types if we are calibrating the weights
Bool Calibrater::calWt() {

  Int napp(vc_p.nelements());
  // Return true as soon as we find a type which is cal'ing wts
  for (Int iapp=0;iapp<napp;++iapp)
    if (vc_p[iapp] && vc_p[iapp]->calWt())
      return true;

  // None cal'd weights, so return false
  return false;

}

Bool Calibrater::ok() {

  if ((simdata_p || 
       (ms_p && mssel_p))
      && ve_p) {
    return true;
  }
  else {
    logSink() << "Calibrater is not yet initialized" << LogIO::POST;
    return false;
  }
}
// The standard solving mechanism
casacore::Bool Calibrater::genericGatherAndSolve()  
{

#ifdef _OPENMP
  Double Tsetup(0.0),Tgather(0.0),Tsolve(0.0),Tadd(0.0);
  Double time0=omp_get_wtime();
#endif

  // Condition solint values 
  svc_p->reParseSolintForVI2();

  // Organize VI2 layers for solving:
  CalSolVi2Organizer vi2org;

  //-------------------------------------------------
  //  NB: Populating the vi2org should probably be delegated to the svc_p
  //      since that is where most of the required (non-data) info is,
  //      and then some calibration types may introduce layers that are
  //      very specific to them (e.g., SD data selection, etc.)
  //  e.g., replace the following with something like:
  //
  //  svc_p->formVi2LayerFactories(vi2org,mssel_p,ve_p);
  //                                      ssvp_p          // a simdata version
  //

  // Add the (bottom) data access layer
  if (simdata_p)
    // Simulated data (used for testing)
    vi2org.addSimIO(ssvp_p);
  else
  {
    // Real (selected) data in an MS (channel selection handled by addDiskIO method)
    //   The iteration time-interval is the solution interval
    vi2org.addDiskIO(mssel_p,svc_p->solTimeInterval(),
		     svc_p->combobs(),svc_p->combscan(),
		     svc_p->combfld(),svc_p->combspw(),
		     true,     // use MSIter2
                     frequencySelections_p);  // Tell VI2 factory about the freq selection!
  }

  // Add ad hoc SD section layer (e.g., OTF select of raster boundaries, etc.)
  // only double circle gain calibration is implemented
  bool SD = svc_p->longTypeName().startsWith("SDGAIN_OTFD");
  if (SD) {
    vi2org.addCalFilter(calFilterConfig_p);
  }

  // Add pre-cal layer, using the VisEquation
  //  (include control for corr-dep flags)
  vi2org.addCalForSolving(*ve_p,corrDepFlags_);


  // Add the freq-averaging layer, if needed
  //cout << "svc_p->fintervalChV() = " << svc_p->fintervalChV() << endl;                                                    
  //cout << "svc_p->fsolint() = " << svc_p->fsolint() << endl;
  // TBD: improve the following logic with new method in SVC...
  if (!svc_p->freqDepMat() ||       // entirely unchannelized cal  OR
      (svc_p->freqDepPar() &&            // channelized par and
       svc_p->fsolint()!="none" &&       // some partial channel averaging
       anyGT(svc_p->fintervalChV(),1.0)) // explicity specified (non-trivially)
      ) { 
    Vector<Int> chanbin(svc_p->fintervalChV().nelements());
    convertArray(chanbin,svc_p->fintervalChV());
    vi2org.addChanAve(chanbin);  
  }

  // Add the time-averaging layer, if needed
  //  NB: There is some evidence that doing time-averaging _after_ 
  //      channel averaging may be more expensive then doing it _before_, 
  //      but this ensures that the meta-info (e.g. time, timeCentroid, etc.)
  //      appear correctly in the VB2 accessed at this scope.
  //      The problem is that AveragingTvi2 has not explicitly
  //      implemented all of the relevant data accessors; it just
  //      assumes---incorrectly---that you are going to use its VB2.
  //      Making AveragingTvi2 the top layer ensures you do use it.
  //      (Hard-wired use of AveragingTvi2 in MSTransform set it up
  //       as the top [last] layer, and has been well-tested...)

  //cout << "svc_p->solint() = " << svc_p->solint() << endl;
  //cout << "svc_p->solTimeInterval() = " << svc_p->solTimeInterval() << endl;
  //cout << "svc_p->preavg() = " << svc_p->preavg() << endl;
  if (!svc_p->solint().contains("int") &&      // i.e., not "int")
      svc_p->preavg() != -DBL_MAX) {
    Float avetime(svc_p->solTimeInterval());   // avetime is solint, nominally
    // Use preavg instead, if...
    if (svc_p->preavg()>FLT_EPSILON &&             // ...set meaningfully
	svc_p->preavg()<svc_p->solTimeInterval())  // ...and less than solint
      avetime=svc_p->preavg();
    vi2org.addTimeAve(avetime);  // use min of solint and preavg here!
  }

  //  vi2org should be fully configured at this point
  //-------------------------------------------------


  // Form the VI2 to drive data iteration below
  vi::VisibilityIterator2& vi(vi2org.makeFullVI());
  //cout << "VI Layers: " << vi.ViiType() << endl;

  // Access to the net VB2 for forming SolveDataBuffers in the SDBList below
  //  NB: this assumes that the internal address of the VI2's VB2 will never change!
  vi::VisBuffer2 *vb = vi.getImpl()->getVisBuffer();

  // VI2 should now be ready....

  // Discern number of Solutions and how many VI2 chunks per each
  Int nSol(0);
  Vector<Int> nChPSol(1000,0);  // nominal size; will enlarge as needed, and trim at end

  {
    //cout << "Counting chunks and solutions..." << endl;
    uInt isol(0);
    for (vi.originChunks();vi.moreChunks();vi.nextChunk()) {
      // NB: this loop NEVER touches the the subChunks, since
      //     this would cause data I/O, cal, averaging, etc.

      // Enlarge nChPSol Vector, if necessary
      if (isol>nChPSol.nelements()-1) {
	//cout << "   ...At isol=" << isol << ", resizing nchPSol: " << nChPSol.nelements() << "-->";
	nChPSol.resize(isol*2,True);        // double length, copy existing elements
	nChPSol(Slice(isol,isol,1)).set(0); // init new elements
	//cout << nChPSol.nelements() << endl;
	//cout << "sol counting: " << isol << " " << nChPSol << " (" << ntrue(nChPSol>0) << "/" << nChPSol.nelements() << ")" << endl;
      }

      // incr chunk counter for current solution
      ++nChPSol(isol);  
      //cout << "sol counting: " << isol << " " << nChPSol(Slice(0,min(isol+2,nChPSol.nelements()),1))
      //     << " (" << ntrue(nChPSol>0) << "/" << nChPSol.nelements() << ")" 
      //     << "  keyCh=" << vi.keyChange()
      //     << endl;

      // next chunk is nominally next solution
      ++isol;           

      // If combining spw or field, and the next chunk changes by
      //   DATA_DESC_ID or FIELD_ID (and _not_ TIME, which is
      //   changing slower than these when combine is on),
      //   then the next chunk should be counted in the same soln
      if ( (svc_p->combspw() && vi.keyChange()=="DATA_DESC_ID") ||
	   (svc_p->combfld() && vi.keyChange()=="FIELD_ID") )
	--isol; // next one is the same solution interval

    }
    // Trim list to realized length, which is the number of solutions we'll 
    //   attempt below
    nSol=ntrue(nChPSol>0);
    nChPSol.resize(nSol,true);
    //cout << "nSol   =" << nSol << endl;
    //cout << "nChPSol=" << nChPSol << endl;

    /*  from SVC...
    if (svc_p->combobs())
      logSink() << "Combining observation Ids." << LogIO::POST;
    if (svc_p->combscan())
      logSink() << "Combining scans." << LogIO::POST;
    if (svc_p->combspw()) 
      logSink() << "Combining spws: " << spwlist << " -> " << spwlab << LogIO::POST;
    if (svc_p->combfld()) 
      logSink() << "Combining fields." << LogIO::POST;
    */

    logSink() << "For solint = " << svc_p->solint() << ", found "
	      <<  nSol << " solution intervals."
	      << LogIO::POST;
  }

  //  throw(AipsError("EARLY ESCAPE!!"));

  // Create the output caltable                                                                                             
  //  (this version doesn't revise frequencies)                                                                             
  svc_p->createMemCalTable2();

  Vector<Float> spwwts(msmc_p->nSpw(),-1.0);
  Vector<Int64> nexp(msmc_p->nSpw(),0), natt(msmc_p->nSpw(),0),nsuc(msmc_p->nSpw(),0);

#ifdef _OPENMP
  Tsetup+=(omp_get_wtime()-time0);
#endif

  Int nGood(0);
  vi.originChunks();
  Int nGlobalChunks=0;  // counts VI chunks globally
  for (Int isol=0;isol<nSol && vi.moreChunks();++isol) {

#ifdef _OPENMP
    time0=omp_get_wtime();
#endif

    // Data will accumulate here                                                                                            
    SDBList sdbs;

    // Gather the chunks/VBs for this solution                                                                              
    //   Solution boundaries will ALWAYS occur on chunk boundaries,
    //   though some chunk boundaries will be ignored in the 
    //   combine='spw' or 'field' context

    for (Int ichunk=0;                                // count chunks in this solution
	 ichunk<nChPSol(isol)&&vi.moreChunks();  // while more chunks needed _and_ available
	 ++ichunk,vi.nextChunk()) {                     // advance to next chunk

      // Global chunk counter
      ++nGlobalChunks;

      //  Loop over VB2s in this chunk
      //    (we get more then one when preavg<solint)
      Int ivb(0);
      for (vi.origin();
	   vi.more();
	   ++ivb,vi.next()) {

	// Add this VB to the SDBList                                                                                       
#ifdef _OPENMP
	Double Tadd0=omp_get_wtime();
#endif

	sdbs.add(*vb);

#ifdef _OPENMP
	Tadd+=(omp_get_wtime()-Tadd0);
#endif

	// Keep track of spws seen but not included in solving
	Int ispw=vb->spectralWindows()(0);
	if (spwwts(ispw)<0) spwwts(ispw)=0.0f;
	spwwts(ispw)+=sum(vb->weightSpectrum());

	/*
	Double modv(86400.0);
	cout.precision(7);
	cout << "isol=" << isol
             << " ichk="<<ichunk
             << " ivb="<<ivb
             << " sc="<<vb->scan()(0)
             << " fld="<<vb->fieldId()(0)
             << " spw="<<vb->spectralWindows()(0)
             << " nr="<<vb->nRows()
             << " nch="<<vb->nChannels() << flush;
	cout << " f="<<mean(vb->getFrequencies(0))/1e9
	     << "->"<< mean(sdbs.freqs())/1e9
             << " t="<<fmod(mean(vb->time()),modv)
	     << "->"<< fmod(sdbs.aggregateTime(),modv)
             << " tC="<<fmod(mean(vb->timeCentroid()),modv)
	     << "->"<< fmod(sdbs.aggregateTimeCentroid(),modv)
             << " wt="<<sum(vb->weightSpectrum())
	     << " nSDB=" << sdbs.nSDB() 
	     << " nGlChks=" << nGlobalChunks
	     << " (keych=" << vi.keyChange() << ")"
             << endl;
	//*/

      }  // VI2 subchunks (VB2s)
    } // VI2 chunks

    // Which spw is this?
    Int thisSpw(sdbs.aggregateSpw());

    // Expecting a solution                                                                                                 
    nexp(thisSpw)+=1;

#ifdef _OPENMP
    Tgather+=(omp_get_wtime()-time0);
    time0=omp_get_wtime();
#endif

    if (sdbs.Ok()) {

      // Some unflagged data, so Attempting a solution                                                                      
      natt(thisSpw)+=1;

      // make phase- or amp-only, if necessary                                                                              
      sdbs.enforceAPonData(svc_p->apmode());

      // zero cross-hand weights, if necessary                                                                              
      sdbs.enforceSolveWeights(svc_p->phandonly());

      // Synchronize meta-info in SVC                                                                                       
      svc_p->syncSolveMeta(sdbs);

      // Set or verify freqs in the caltable                                                                                
      svc_p->setOrVerifyCTFrequencies(thisSpw);                                                                     

      // Size the solvePar arrays inside SVC                                                                                
      //  (smart:  if freqDepPar()=F, uses 1)                                                                               
      //  returns the number of channel solutions to iterate over                                                           
      //Int nChanSol=svc_p->sizeSolveParCurrSpw(sdbs.nChannels());
      Int nChanSol=svc_p->sizeSolveParCurrSpw((svc_p->freqDepPar() ? sdbs.nChannels() : 1));

      if (svc_p->useGenericSolveOne()) {

        // We'll use the generic solver                                                                                     
        VisCalSolver2 vcs(svc_p->solmode(),svc_p->rmsthresh());

        // Guess from the data                                                                                              
        svc_p->guessPar(sdbs,corrDepFlags_);

        Bool totalGoodSol(False);  // Will be set True if any channel is good                                               
        //for (Int ich=0;ich<nChanSol;++ich) {
        for (Int ich=nChanSol-1;ich>-1;--ich) {
          svc_p->markTimer();
          svc_p->focusChan()=ich;

          // Execute the solve                                                                                              
          Bool goodsol=vcs.solve(*ve_p,*svc_p,sdbs);


          if (goodsol) {
            totalGoodSol=True;

            svc_p->formSolveSNR();
            svc_p->applySNRThreshold();
          }
          else
	    svc_p->currMetaNote();

          // Record solution in its channel, good or bad                                                                    
          if (svc_p->freqDepPar())
            svc_p->keep1(ich);

        } // ich                                                                                                            

        if (totalGoodSol) {
          // Keep this good solution, and count it                                                                          
          svc_p->keepNCT();
          ++nGood;
          nsuc(thisSpw)+=1;
        }

      } // useGenericSolveOne                                                                                               
      else {
        // Use self-directed individual solve                                                                               
        //   TBD: return T/F for goodness?                                                                                  
	//cout << "Calling selfSolveOne()!!!!!!" << endl;
        svc_p->selfSolveOne(sdbs);

        // Keep this good solution, and count it                                                                            
        svc_p->keepNCT();
        ++nGood;
        nsuc(thisSpw)+=1;
      }

    } // sdbs.Ok()
    else {
      // Synchronize meta-info in SVC
      svc_p->syncSolveMeta(sdbs);
      cout << "Found no unflagged data at:";
      svc_p->currMetaNote();
    }
    //cout << endl;

#ifdef _OPENMP
    Tsolve+=(omp_get_wtime()-time0);
#endif    
    
    //    throw(AipsError("EARLY ESCAPE!!"));


  } // isol                                                                                                                 

  // Report nGood to logger
  logSink() << "  Found good " 
	    << svc_p->typeName() << " solutions in "
	    << nGood << " solution intervals."
	    << LogIO::POST;

#ifdef _OPENMP
 #ifdef REPORT_CAL_TIMING
  cout << "Calibrater::genericGatherAndSolve Timing: " << endl;
  cout << " setup=" << Tsetup
       << " gather=" << Tgather 
       << " (SDBadd=" << Tadd << ")"
       << " solve=" << Tsolve 
       << " total=" << Tsetup+Tgather+Tsolve
    //       << " tick=" << omp_get_wtick()
       << endl;
 #endif
#endif

  // Report spws that were seen but not solved
  Vector<Bool> unsolspw=(spwwts==0.0f); 
  summarize_uncalspws(unsolspw, "solv");                                                                                  

  //  throw(AipsError("EARLY ESCAPE!!"));

  if (nGood>0) {
    if (svc_p->typeName()!="BPOLY") {  // needed?                                                                           
      // apply refant, etc.                                                                                                 
      svc_p->globalPostSolveTinker();

      // write to disk                                                                                                      
      svc_p->storeNCT();
    }
  }
  else {
    logSink() << "No output calibration table written."
	      << LogIO::POST;
  }

  // Fill activity record
  //  cout << "  Expected, Attempted, Succeeded (by spw) = " << nexp << ", " << natt << ", " << nsuc << endl;                 
  //  cout << " Expected, Attempted, Succeeded = " << sum(nexp) << ", " << sum(natt) << ", " << sum(nsuc) << endl;
  actRec_=Record();
  actRec_.define("origin","Calibrater::genericGatherAndSolve");
  actRec_.define("nExpected",nexp);
  actRec_.define("nAttempt",natt);
  actRec_.define("nSucceed",nsuc);

  { 
    Record solveRec=svc_p->solveActionRec();
    if (solveRec.nfields()>0)
      actRec_.merge(solveRec);
  }

  // Reach here, all is good
  return True;

}


void Calibrater::writeHistory(LogIO& /*os*/, Bool /*cliCommand*/)
{
  // Disabled 2016/04/19: avoid direct MS.HISTORY updates from
  //   below the python level, FOR NOW

  return;
  /*
  if (!historytab_p.isNull()) {
    if (histLockCounter_p == 0) {
      historytab_p.lock(false);
    }
    ++histLockCounter_p;

    os.postLocally();
    if (cliCommand) {
      hist_p->cliCommand(os);
    } else {
      hist_p->addMessage(os);
    }

    if (histLockCounter_p == 1) {
      historytab_p.unlock();
    }
    if (histLockCounter_p > 0) {
      --histLockCounter_p;
    }
  } else {
    os << LogIO::SEVERE << "calibrater is not yet initialized" << LogIO::POST;
  }
  */
}

void Calibrater::setCalFilterConfiguration(String const &type,
    Record const &config) {
  // currently only SDDoubleCircleGainCal requires data filtering
  if (type.startsWith("SDGAIN_OTFD")) {
    calFilterConfig_p.define("mode", "SDGAIN_OTFD");
    if (config.isDefined("smooth")) {
      calFilterConfig_p.define("smooth", config.asBool("smooth"));
    }
    if (config.isDefined("radius")) {
      calFilterConfig_p.define("radius", config.asString("radius"));
    }
  }
}

// *********************************************
//  OldCalibrater implementations that use vs_p

OldCalibrater::OldCalibrater(): 
  Calibrater(),
  vs_p(0), 
  rawvs_p(0)
{
  //  cout << "This is the OLD VI2-aware Calibrater" << endl;
}

OldCalibrater::OldCalibrater(String msname): 
  Calibrater(msname),
  vs_p(0), 
  rawvs_p(0)
{
}

/*
OldCalibrater::OldCalibrater(const OldCalibrater & other) :
  Calibrater(other)
{
  operator=(other);
}

OldCalibrater& OldCalibrater::operator=(const OldCalibrater & other)
{
  Calibrater::operator=(other); // copy parental units
  vs_p=other.vs_p;
  rawvs_p=other.rawvs_p;
  return *this;
}
*/

OldCalibrater::~OldCalibrater()
{
  OldCalibrater::cleanupVisSet();
}


// Select data (using MSSelection syntax)
void OldCalibrater::selectvis(const String& time,
			      const String& spw,
			      const String& scan,
			      const String& field,
			      const String& intent,
			      const String& obsIDs,
			      const String& baseline,
			      const String& uvrange,
			      const String& chanmode,
			      const Int& nchan,
			      const Int& start, 
			      const Int& step,
			      const MRadialVelocity& mStart,
			      const MRadialVelocity& mStep,
			      const String& msSelect)
{
// Define primary measurement set selection criteria
// Inputs:
//    time
//    spw
//    scan
//    field
//    intent
//    obsIDs
//    baseline
//    uvrange
//    chanmode     const String&            Frequency/velocity selection mode
//                                          ("channel", "velocity" or 
//                                           "opticalvelocity")
//    nchan        const Int&               No of channels to select
//    start        const Int&               Start channel to select
//    step         const Int&               Channel increment
//    mStart       const MRadialVelocity&   Start radial vel. to select
//    mStep        const MRadialVelocity&   Radial velocity increment
//    msSelect     const String&            MS selection string (TAQL)
// Output to private data:
//
  logSink() << LogOrigin("Calibrater","selectvis") << LogIO::NORMAL3;
  
  try {

 /*   
    cout << "time     = " << time << " " << time.length() <<endl;
    cout << "spw      = " << spw << " " << spw.length() <<endl;
    cout << "scan     = " << scan << " " << scan.length() <<endl;
    cout << "field    = " << field << " " << field.length() <<endl;
    cout << "baseline = " << baseline << " " << baseline.length() << endl;
    cout << "uvrange  = " << uvrange << " " << uvrange.length() << endl;
 */

    logSink() << "Selecting data" << LogIO::POST;
    
    // Apply selection to the original MeasurementSet
    logSink() << "Performing selection on MeasurementSet" << endl;
    
    if (mssel_p) {
      delete mssel_p;
      mssel_p=0;
    };

    // Report non-trivial user selections
    if (time!="")
      logSink() << " Selecting on time: '" << time << "'" << endl;
    if (spw!="")
      logSink() << " Selecting on spw: '" << spw << "'" << endl;
    if (scan!="")
      logSink() << " Selecting on scan: '" << scan << "'" << endl;
    if (field!="")
      logSink() << " Selecting on field: '" << field << "'" << endl;
    if (intent!="")
      logSink() << " Selecting on intent: '" << intent << "'" << endl;
    if(obsIDs != "")
      logSink() << " Selecting by observation IDs: '" << obsIDs << "'" << endl;
    if (baseline!="")
      logSink() << " Selecting on antenna/baseline: '" << baseline << "'" << endl;
    if (uvrange!="")
      logSink() << " Selecting on uvrange: '" << uvrange << "'" << endl;
    if (msSelect!="")
      logSink() << " Selecting with TaQL: '" << msSelect << "'" << endl;
    logSink() << LogIO::POST;


    // Assume no selection, for starters
    // gmoellen 2012/01/30    mssel_p = new MeasurementSet(sorted, ms_p);
    mssel_p = new MeasurementSet(*ms_p);

    // Apply user-supplied selection
    Bool nontrivsel=false;
    // gmoellen 2012/01/30    nontrivsel= mssSetData(MeasurementSet(sorted, ms_p),

    // Ensure use of a fresh MSSelection object
    if (mss_p) { delete mss_p; mss_p=NULL; }
    mss_p=new MSSelection();
    nontrivsel= mssSetData(*ms_p,
			   *mssel_p,"",
			   time,baseline,
			   field,spw,
			   uvrange,msSelect,
			   "",scan,"",intent, obsIDs,mss_p);

    // Keep any MR status for the MS
    mssel_p->setMemoryResidentSubtables(ms_p->getMrsEligibility());

    // If non-trivial MSSelection invoked and nrow reduced:
    if(nontrivsel && mssel_p->nrow()<ms_p->nrow()) {

      // Escape if no rows selected
      if (mssel_p->nrow()==0) 
	throw(AipsError("Specified selection selects zero rows!"));

      // ...otherwise report how many rows are selected
      logSink() << "By selection " << ms_p->nrow() 
		<< " rows are reduced to " << mssel_p->nrow() 
		<< LogIO::POST;
    }
    else {
      // Selection did nothing:
      logSink() << "Selection did not drop any rows" << LogIO::POST;
    }

    // Now, re-create the associated VisSet
    if(vs_p) delete vs_p; vs_p=0;
    Block<int> sort(0);
    Matrix<Int> noselection;
    // gmoellen 2012/01/30    vs_p = new VisSet(*mssel_p,sort,noselection);
    vs_p = new VisSet(*mssel_p,sort,noselection,false,0.0,false,false);
    AlwaysAssert(vs_p, AipsError);

    // Attempt to use MSSelection for channel selection
    //  if user not using the old way
    if (chanmode=="none") {
      selectChannel(spw);
    }
    else {
      // Reluctantly use the old-fashioned way
      logSink() << LogIO::WARN 
		<< "You have used the old-fashioned mode parameter" << endl
		<< "for channel selection.  It still works, for now," << endl
		<< "but this will be eliminated in the near future." << endl
		<< "Please begin using the new channel selection" << endl
		<< "syntax in the spw parameter." << LogIO::POST;
      selectChannel(chanmode,nchan,start,step,mStart,mStep);
    }

  }
  catch (MSSelectionError& x) {
    // Re-initialize with the existing MS
    logSink() << LogOrigin("Calibrater","selectvis",WHERE) 
	      << LogIO::SEVERE << "Caught exception: " << x.getMesg()
	      << LogIO::POST;
    // jagonzal (CAS-4110): I guess it is not necessary to create these columns when the selection is empty
    initialize(*ms_p,false,false,false);
    throw(AipsError("Error in data selection specification: " + x.getMesg()));
  } 
  catch (AipsError x) {
    // Re-initialize with the existing MS
    logSink() << LogOrigin("Calibrater","selectvis",WHERE) 
	      << LogIO::SEVERE << "Caught exception: " << x.getMesg()
	      << LogIO::POST;
    // jagonzal (CAS-4110): I guess it is not necessary to create these columns when the selection is empty.
    initialize(*ms_p,false,false,false);
    throw(AipsError("Error in Calibrater::selectvis(): " + x.getMesg()));
  } 
};


Bool OldCalibrater::setapply (const String& type, 
			      const Record& applypar)
{
  logSink() << LogOrigin("Calibrater", "setapply(type, applypar)");

  // First try to create the requested VisCal object
  VisCal *vc(NULL);

  try {

    if(!ok()) 
      throw(AipsError("Calibrater not prepared for setapply."));

    String upType=type;
    upType.upcase();

    logSink() << LogIO::NORMAL 
	      << "Arranging to APPLY:"
	      << LogIO::POST;

    // Add a new VisCal to the apply list
    vc = createVisCal(upType,*vs_p);  

    vc->setApply(applypar);       

    logSink() << LogIO::NORMAL << ".   "
	      << vc->applyinfo()
	      << LogIO::POST;

  } catch (AipsError x) {
    logSink() << LogIO::SEVERE << x.getMesg() 
	      << " Check inputs and try again."
	      << LogIO::POST;
    if (vc) delete vc;
    throw(AipsError("Error in Calibrater::setapply."));
    return false;
  }

  // Creation apparently successful, so add to the apply list
  // TBD: consolidate with above?
  try {

    uInt napp=vc_p.nelements();
    vc_p.resize(napp+1,false,true);      
    vc_p[napp] = vc;
    vc=NULL;
   
    // Maintain sort of apply list
    ve_p->setapply(vc_p);
    
    return true;

  } catch (AipsError x) {
    logSink() << LogIO::SEVERE << "Caught exception: " << x.getMesg() 
	      << LogIO::POST;
    if (vc) delete vc;
    throw(AipsError("Error in Calibrater::setapply."));
    return false;
  } 
  return false;
}

// Set up apply-able calibration via a Cal Library
Bool OldCalibrater::setcallib(Record callib) {

  logSink() << LogOrigin("Calibrater", "setcallib(callib)");

  //  cout << "Calibrater::setcallib: callib.isFixed() = " << boolalpha << callib.isFixed() << endl;

  uInt ntab=callib.nfields();

  //  cout << "callib.nfields() = " << ntab << endl;

  // Do some preliminary per-table verification
  for (uInt itab=0;itab<ntab;++itab) {

    String tabname=callib.name(itab);

    // Insist that the table exists on disk
    if (!Table::isReadable(tabname))
      throw(AipsError("Caltable "+tabname+" does not exist."));

  }

  // Tables exist, so deploy them...

  for (uInt itab=0;itab<ntab;++itab) {

    String tabname=callib.name(itab);

    // Get the type from the table
    String upType=calTableType(tabname);
    upType.upcase();

    // Add table name to the record
    Record thistabrec=callib.asrwRecord(itab);
    thistabrec.define("tablename",tabname);

    // First try to create the requested VisCal object
    VisCal *vc(NULL);

    try {

      if(!ok()) 
	throw(AipsError("Calibrater not prepared for setapply."));
      
      logSink() << LogIO::NORMAL 
		<< "Arranging to APPLY:"
		<< LogIO::POST;
      
      // Add a new VisCal to the apply list
      vc = createVisCal(upType,*vs_p);  


      // ingest this table according to its callib
      vc->setCallib(thistabrec,*mssel_p);

    } catch (AipsError x) {
      logSink() << LogIO::SEVERE << x.getMesg() 
		<< " Check inputs and try again."
		<< LogIO::POST;
      if (vc) delete vc;
      throw(AipsError("Error in Calibrater::setapply."));
      return false;
    }

    // Creation apparently successful, so add to the apply list
    // TBD: consolidate with above?
    try {
      
      uInt napp=vc_p.nelements();
      vc_p.resize(napp+1,false,true);      
      vc_p[napp] = vc;
      vc=NULL;
   
      // Maintain sort of apply list
      ve_p->setapply(vc_p);
      
    } catch (AipsError x) {
      logSink() << LogIO::SEVERE << "Caught exception: " << x.getMesg() 
		<< LogIO::POST;
      if (vc) delete vc;
      throw(AipsError("Error in Calibrater::setapply."));
      return false;
    } 
  }

  // All ok, if we get this far!
  return true;

}


// Set up apply-able calibration via a Cal Library
Bool OldCalibrater::setcallib2(Record callib, const casacore::MeasurementSet* ms) {

  logSink() << LogOrigin("OldCalibrater", "setcallib2(callib)");

  //  cout << "Calibrater::setcallib2(callib) : " << boolalpha << callib << endl;

  uInt ntab=callib.nfields();

  //  cout << "callib.nfields() = " << ntab << endl;

  // Do some preliminary per-table verification
  for (uInt itab=0;itab<ntab;++itab) {

    String tabname=callib.name(itab);

    // Trap parang
    // TBD...
    //    if (tabname=="<parang>")
    //      continue;

    // Insist that the table exists on disk
    if (!Table::isReadable(tabname))
      throw(AipsError("Caltable "+tabname+" does not exist."));

  }

  // Tables exist, so deploy them...

  // Local MS object for callib parsing (only)
  //  MeasurementSet lms(msname_p,Table::Update);
  //cout << "OLD lms" << endl;


  // Local const MS object for callib parsing (only)
  const MeasurementSet *lmsp(0);
  if (ms) {
    // Use supplied MS (from outside), if specified...
    // TBD: should we verify same base MS as ms_p/mssel_p?
    lmsp=ms;
  }
  else {
    // ...use internal one instead
    lmsp=mssel_p;
  }
  // Reference for use below
  const MeasurementSet &lms(*lmsp);

  // Get some global shape info:
  Int MSnAnt = lms.antenna().nrow();
  Int MSnSpw = lms.spectralWindow().nrow();

  for (uInt itab=0;itab<ntab;++itab) {

    String tabname=callib.name(itab);

    // Get the type from the table
    String upType=calTableType(tabname);
    upType.upcase();

    // Add table name to the record
    Record thistabrec=callib.asrwRecord(itab);
    thistabrec.define("tablename",tabname);

    // First try to create the requested VisCal object
    VisCal *vc(NULL);

    try {

      //      if(!ok()) 
      //	throw(AipsError("Calibrater not prepared for setapply."));
      
      logSink() << LogIO::NORMAL 
		<< "Arranging to APPLY:"
		<< LogIO::POST;
      
      // Add a new VisCal to the apply list
      vc = createVisCal(upType,msname_p,MSnAnt,MSnSpw);  

      // ingest this table according to its callib
      vc->setCallib(thistabrec,lms);

    } catch (AipsError x) {
      logSink() << LogIO::SEVERE << x.getMesg() 
		<< " Check inputs and try again."
		<< LogIO::POST;
      if (vc) delete vc;
      throw(AipsError("Error in Calibrater::callib2."));
      return false;
    }

    // Creation apparently successful, so add to the apply list
    // TBD: consolidate with above?
    try {
      
      uInt napp=vc_p.nelements();
      vc_p.resize(napp+1,false,true);      
      vc_p[napp] = vc;
      vc=NULL;
   
      // Maintain sort of apply list
      ve_p->setapply(vc_p);
      
    } catch (AipsError x) {
      logSink() << LogIO::SEVERE << "Caught exception: " << x.getMesg() 
		<< LogIO::POST;
      if (vc) delete vc;
      throw(AipsError("Error in Calibrater::setapply."));
      return false;
    } 
  }
  // All ok, if we get this far!
  return true;

}


Bool OldCalibrater::setsolve (const String& type, 
			      const Record& solvepar) {

  // Attempt to create the solvable object
  SolvableVisCal *svc(NULL);
  try {

    if(!ok()) 
      throw(AipsError("Calibrater not prepared for setsolve."));

    String upType = type;
    upType.upcase();

    // Clean out any old solve that was lying around
    unsetsolve();

    logSink() << LogIO::NORMAL 
	      << "Arranging to SOLVE:"
	      << LogIO::POST;

    // Create the new SolvableVisCal
    svc = createSolvableVisCal(upType,*vs_p);
    svc->setSolve(solvepar);
    
    logSink() << LogIO::NORMAL << ".   "
	      << svc->solveinfo()
	      << LogIO::POST;

    // Creation apparently successful, keep it
    svc_p=svc;
    svc=NULL;

    return true;

  } catch (AipsError x) {
    logSink() << LogIO::SEVERE << "Caught exception: " << x.getMesg() 
	      << LogIO::POST;
    unsetsolve();
    if (svc) delete svc;
    throw(AipsError("Error in Calibrater::setsolve."));
    return false;
  } 
  return false;
}

Bool OldCalibrater::correct(String mode)
{
    logSink() << LogOrigin("Calibrater","correct") << LogIO::NORMAL;

    Bool retval = true;

    try {

        // make mode all-caps
        String upmode=mode;
	upmode.upcase();

	// If trialmode=T, only the flags will be set
	//   (and only written if not TRIAL)
	Bool trialmode=(upmode.contains("TRIAL") || 
			upmode.contains("FLAGONLY"));

        // Set up VisSet and its VisibilityIterator.

        VisibilityIterator::DataColumn whichOutCol = configureForCorrection ();

        VisIter& vi(vs_p->iter());
        VisBufferAutoPtr vb (vi);
        vi.origin();

        // Pass each timestamp (VisBuffer) to VisEquation for correction

        Vector<Bool> uncalspw(vi.numberSpw());	// Used to accumulate error messages
        uncalspw.set(false);		        // instead of bombing the user
        uncalspw.set(False);		        // instead of bombing the user
                                        // in a loop.

        for (vi.originChunks(); vi.moreChunks(); vi.nextChunk()) {

            for (vi.origin(); vi.more(); vi++) {

                uInt spw = vb->spectralWindow();

		// Re-initialize weights from sigma column
		vb->resetWeightMat();

		// If we can calibrate this vb, do it...
                if (ve_p->spwOK(spw)){
		  
		  // throws exception if nothing to apply
		  ve_p->correct(*vb,trialmode);
		    
                }
		// ...else don't, prepare warning, and possibly set flags
                else{

		  // set uncalspw for warning message
		  uncalspw[spw] = true;
		  // set the flags, if we are being strict
		  if (upmode.contains("STRICT"))
		    // set the flags
		    // (don't touch the data/weights, which are initialized)
		    vb->flag().set(true);
                }

		// Only if not a trial run, trigger write to disk
		if (!upmode.contains("TRIAL")) {
		      
		  if (upmode.contains("CAL")) {
		    vi.setVis (vb->visCube(), whichOutCol);
		    vi.setWeightMat(vb->weightMat()); 
		  }
		  
		  if (upmode.contains("FLAG"))
		    vi.setFlag (vb->flag());
		  
		}
		
            }
        }

        vs_p->flush (); // Flush to disk

        // Now that we're out of the loop, summarize any errors.

        retval = summarize_uncalspws(uncalspw, "correct",
 				     upmode.contains("STRICT"));

	actRec_=Record();
	actRec_.define("origin","Calibrater::correct");
	actRec_.defineRecord("VisEquation",ve_p->actionRec());

    }
    catch (AipsError x) {
        logSink() << LogIO::SEVERE << "Caught exception: " << x.getMesg()
	              << LogIO::POST;

        logSink() << "Resetting all calibration application settings." << LogIO::POST;
        unsetapply();

        throw(AipsError("Error in Calibrater::correct."));
        retval = false;         // Not that it ever gets here...
    }
    return retval;
}

Bool OldCalibrater::corrupt() {
  
  logSink() << LogOrigin("Calibrater","corrupt") << LogIO::NORMAL;
  Bool retval = true;

  try {

    if (!ok())
      throw(AipsError("Calibrater not prepared for corrupt!"));

    // Nominally, we write out to the MODEL_DATA, unless absent
    VisibilityIterator::DataColumn whichOutCol(VisibilityIterator::Model);

    if (!ms_p->tableDesc().isColumn("MODEL_DATA"))
      throw(AipsError("MODEL_DATA column unexpectedly absent. Cannot corrupt."));

    // Ensure apply list non-zero and properly sorted
    ve_p->setapply(vc_p);

    // Report the types that will be applied
    applystate();

    // Arrange for iteration over data
    Block<Int> columns;
    // include scan iteration
    columns.resize(5);
    columns[0]=MS::ARRAY_ID;
    columns[1]=MS::SCAN_NUMBER;
    columns[2]=MS::FIELD_ID;
    columns[3]=MS::DATA_DESC_ID;
    columns[4]=MS::TIME;
    vs_p->resetVisIter(columns,0.0);
    VisIter& vi(vs_p->iter());
    VisBuffer vb(vi);
    
    // Pass each timestamp (VisBuffer) to VisEquation for corruption.
    Vector<Bool> uncalspw(vi.numberSpw());	// Used to accumulate error messages
    uncalspw.set(false);		        // instead of bombing the user
						// in a loop.
    for (vi.originChunks(); vi.moreChunks(); vi.nextChunk()) {
      Int spw = vi.spectralWindow();

      // Only proceed if spw can be calibrated
      if (ve_p->spwOK(spw)) {

	for (vi.origin(); vi.more(); vi++) {
	  
	  // Corrupt the MODEL_DATA
	  //  (note we are not treating weights and flags)
	  ve_p->corrupt(vb);    // throws exception if nothing to apply
	  vi.setVis(vb.modelVisCube(),whichOutCol);

	}
      }
      else 
	uncalspw[spw] = true;
    }
    // Flush to disk
    vs_p->flush();

    // Now that we're out of the loop, summarize any errors.
    retval = summarize_uncalspws(uncalspw, "corrupt");
  }
  catch (AipsError x) {
    logSink() << LogIO::SEVERE << "Caught exception: " << x.getMesg() 
	      << LogIO::POST;

    logSink() << "Resetting all calibration application settings." << LogIO::POST;
    unsetapply();

    throw(AipsError("Error in Calibrater::corrupt."));
    retval = false;  // Not that it ever gets here...
  } 
  return retval;
}

Bool OldCalibrater::initWeightsWithTsys(String wtmode, Bool dowtsp,
					String tsystable, String gainfield, String interp, Vector<Int> spwmap) {

	logSink() << LogOrigin("Calibrater", "initWeightsWithTsys")
			<< LogIO::NORMAL;
	Bool retval = true;

	try {

		if (!ok())
			throw(AipsError("Calibrater not prepared for initWeights!"));

		String uptype = calTableType(tsystable);
		if (!uptype.contains("TSYS")) {
			throw(AipsError(
					"Invalid calibration table type for Tsys weighting."));
		}
		// Set record format for calibration table application information
		RecordDesc applyparDesc;
		applyparDesc.addField("t", TpDouble);
		applyparDesc.addField("table", TpString);
		applyparDesc.addField("interp", TpString);
		applyparDesc.addField("spw", TpArrayInt);
		applyparDesc.addField("fieldstr", TpString);
		applyparDesc.addField("calwt", TpBool);
		applyparDesc.addField("spwmap", TpArrayInt);
		applyparDesc.addField("opacity", TpArrayDouble);

		// Create record with the requisite field values
		Record applypar(applyparDesc);
		applypar.define("t", 0.0);
		applypar.define("table", tsystable);
		applypar.define("interp", interp);
		applypar.define("spw", getSpwIdx(""));
		applypar.define("fieldstr", gainfield);
		applypar.define("calwt", true);
		applypar.define("spwmap", spwmap);
		applypar.define("opacity", Vector<Double>(1, 0.0));

		if (vc_p.nelements() > 0) {
			logSink() << LogIO::WARN << "Resetting all calibration application settings." << LogIO::POST;
			unsetapply();
		}
		logSink() << LogIO::NORMAL << "Weight initialization does not support selection. Resetting MS selection." << LogIO::POST;
		selectvis();
		StandardTsys vc = StandardTsys(*vs_p);
		vc.setApply(applypar);

		logSink() << LogIO::NORMAL << ".   " << vc.applyinfo() << LogIO::POST;
		PtrBlock<VisCal*> vcb(1, &vc);
		// Maintain sort of apply list
		ve_p->setapply(vcb);

		// Detect WEIGHT_SPECTRUM and SIGMA_SPECTRUM
		TableDesc mstd = ms_p->actualTableDesc();
		String colWtSp = MS::columnName(MS::WEIGHT_SPECTRUM);
		Bool wtspexists = mstd.isColumn(colWtSp);
		String colSigSp = MS::columnName(MS::SIGMA_SPECTRUM);
		Bool sigspexists = mstd.isColumn(colSigSp);
		Bool addsigsp = (dowtsp && !sigspexists);

		// Some log info
		bool use_exposure = false;
		if (wtmode == "tsys") {
			logSink()
					<< "Initializing SIGMA and WEIGHT according to channel bandwidth and Tsys. NOTE this is an expert mode."
					<< LogIO::WARN << LogIO::POST;
		} else if (wtmode == "tinttsys") {
			logSink()
					<< "Initializing SIGMA and WEIGHT according to channel bandwidth, integration time, and Tsys. NOTE this is an expert mode."
					<< LogIO::WARN << LogIO::POST;
			use_exposure = true;
		} else {
			throw(AipsError("Unrecognized wtmode specified: " + wtmode));
		}

		// Force dowtsp if the column already exists
		if (wtspexists && !dowtsp) {
			logSink() << "Found WEIGHT_SPECTRUM; will force its initialization."
					<< LogIO::POST;
			dowtsp = true;
		}

		// Report that we are initializing the WEIGHT_SPECTRUM, and prepare to do so.
		if (dowtsp) {

			// Ensure WEIGHT_SPECTRUM really exists at all
			//   (often it exists but is empty)
			if (!wtspexists) {
				logSink() << "Creating WEIGHT_SPECTRUM." << LogIO::POST;

				// Nominal defaulttileshape
				IPosition dts(3, 4, 32, 1024);

				// Discern DATA's default tile shape and use it
				const Record dminfo = ms_p->dataManagerInfo();
				for (uInt i = 0; i < dminfo.nfields(); ++i) {
					Record col = dminfo.asRecord(i);
					//if (upcase(col.asString("NAME"))=="TILEDDATA") {
					if (anyEQ(col.asArrayString("COLUMNS"), String("DATA"))) {
						dts = IPosition(
								col.asRecord("SPEC").asArrayInt(
										"DEFAULTTILESHAPE"));
						//cout << "Found DATA's default tile: " << dts << endl;
						break;
					}
				}

				// Add the column
				String colWtSp = MS::columnName(MS::WEIGHT_SPECTRUM);
				TableDesc tdWtSp;
				tdWtSp.addColumn(
						ArrayColumnDesc<Float>(colWtSp, "weight spectrum", 2));
				TiledShapeStMan wtSpStMan("TiledWgtSpectrum", dts);
				ms_p->addColumn(tdWtSp, wtSpStMan);
			} else
				logSink() << "Found WEIGHT_SPECTRUM." << LogIO::POST;
			// Ensure WEIGHT_SPECTRUM really exists at all
			//   (often it exists but is empty)
		if (!sigspexists) {
				logSink() << "Creating SIGMA_SPECTRUM." << LogIO::POST;

				// Nominal defaulttileshape
				IPosition dts(3, 4, 32, 1024);

				// Discern DATA's default tile shape and use it
				const Record dminfo = ms_p->dataManagerInfo();
				for (uInt i = 0; i < dminfo.nfields(); ++i) {
					Record col = dminfo.asRecord(i);
					//if (upcase(col.asString("NAME"))=="TILEDDATA") {
					if (anyEQ(col.asArrayString("COLUMNS"), String("DATA"))) {
						dts = IPosition(
								col.asRecord("SPEC").asArrayInt(
										"DEFAULTTILESHAPE"));
						//cout << "Found DATA's default tile: " << dts << endl;
						break;
					}
				}

				// Add the column
				String colSigSp = MS::columnName(MS::SIGMA_SPECTRUM);
				TableDesc tdSigSp;
				tdSigSp.addColumn(
						ArrayColumnDesc<Float>(colSigSp, "sigma spectrum", 2));
				TiledShapeStMan sigSpStMan("TiledSigtSpectrum", dts);
				ms_p->addColumn(tdSigSp, sigSpStMan);
				{
				  TableDesc loctd = ms_p->actualTableDesc();
				  String loccolSigSp = MS::columnName(MS::SIGMA_SPECTRUM);
				  AlwaysAssert(loctd.isColumn(loccolSigSp),AipsError);
				}
		}
		}
		else {
    if (sigspexists) {
      logSink() << "Removing SIGMA_SPECTRUM for non-channelized weight." << LogIO::POST;
      if (true || ms_p->canRemoveColumn(colSigSp)) {
	ms_p->removeColumn(colSigSp);
      }
      else
	logSink() << LogIO::WARN << "Failed to remove SIGMA_SPECTRUM column. Values in SIGMA and SIGMA_SPECTRUM columns may be inconsistent after the operation." << LogIO::POST;
    }
		}

		// Arrange for iteration over data
		//  TBD: Be sure this sort is optimal for creating WS?
		Block<Int> columns;
		// include scan iteration
		columns.resize(5);
		columns[0] = MS::ARRAY_ID;
		columns[1] = MS::SCAN_NUMBER;
		columns[2] = MS::FIELD_ID;
		columns[3] = MS::DATA_DESC_ID;
		columns[4] = MS::TIME;

		vi::SortColumns sc(columns);
		vi::VisibilityIterator2 vi2(*ms_p, sc, true);
		vi::VisBuffer2 *vb = vi2.getVisBuffer();

		MSColumns mscol(*ms_p);
		const MSSpWindowColumns& msspw(mscol.spectralWindow());
		uInt nSpw = msspw.nrow();
		Vector<Double> effChBw(nSpw, 0.0);
		for (uInt ispw = 0; ispw < nSpw; ++ispw) {
			effChBw[ispw] = mean(msspw.effectiveBW()(ispw));
		}

		Int ivb(0);
		for (vi2.originChunks(); vi2.moreChunks(); vi2.nextChunk()) {

			for (vi2.origin(); vi2.more(); vi2.next(), ++ivb) {

				Int spw = vb->spectralWindows()(0);

				auto nrow = vb->nRows();
				Int nchan = vb->nChannels();
				Int ncor = vb->nCorrelations();

				// Prepare for WEIGHT_SPECTRUM and SIGMA_SPECTRUM, if nec.
				Cube<Float> newwtsp(0, 0, 0), newsigsp(0, 0, 0);
				if (dowtsp) {
				  newwtsp.resize(ncor, nchan, nrow);
				  newwtsp.set(1.0);
				  newsigsp.resize(ncor, nchan, nrow);
				  newsigsp.set(1.0);
				}

				if (ve_p->spwOK(spw)) {

					// Re-initialize weight info from sigma info
					//   This is smart wrt spectral weights, etc.
					//   (this makes W and WS, if present, "dirty" in the vb)
					// TBD: only do this if !trial (else: avoid the I/O)
					// vb->resetWeightsUsingSigma();
					// Handle non-trivial modes
					// Init WEIGHT, SIGMA  from bandwidth & time
					Matrix<Float> newwt(ncor, nrow), newsig(ncor, nrow);
					newwt.set(1.0);
					newsig.set(1.0);

					// Detect ACs
					const Vector<Int> a1(vb->antenna1());
					const Vector<Int> a2(vb->antenna2());
					Vector<Bool> ac(a1 == a2);

					// XCs need an extra factor of 2
					Vector<Float> xcfactor(nrow, 2.0);
					xcfactor(ac) = 1.0;				// (but not ACs)

					// The row-wise integration time
					Vector<Float> expo(nrow);
					convertArray(expo, vb->exposure());

					// Set weights to channel bandwidth first.
					newwt.set(Float(effChBw(spw)));

					// For each correlation, apply exposure and xcfactor
					for (Int icor = 0; icor < ncor; ++icor) {

						Vector<Float> wt(newwt.row(icor));
						if (use_exposure) {
							wt *= expo;
						}
						wt *= xcfactor;
						if (dowtsp) {
							for (Int ich = 0; ich < nchan; ++ich) {
								Vector<Float> wtspi(
										newwtsp(Slice(icor, 1, 1),
												Slice(ich, 1, 1), Slice()).nonDegenerate(
												IPosition(1, 2)));
								wtspi = wt;
							}
						}
					}
					// Handle SIGMA_SPECTRUM
					if (dowtsp) {
					  newsigsp = 1.0f / sqrt(newwtsp);
					}
					// sig from wt is inverse sqrt
					newsig = 1.0f / sqrt(newwt);

					// Arrange write-back of both SIGMA and WEIGHT
					vb->setSigma(newsig);
					vb->setWeight(newwt);
					if (dowtsp) {
					  vb->initWeightSpectrum(newwtsp);
					  vb->initSigmaSpectrum(newsigsp);
					}
					// Force writeback to disk (need to initialize weight/sigma before applying cal table)
					vb->writeChangesBack();

					// Arrange for _in-place_ apply on CORRECTED_DATA (init from DATA)
					//   (this makes CD "dirty" in the vb)
					// TBD: only do this if !trial (else: avoid the I/O)
					vb->setVisCubeCorrected(vb->visCube());

					// Make flagcube dirty in the vb
					//  NB: we must _always_ do this I/O  (even trial mode)
					vb->setFlagCube(vb->flagCube());

					// Make all vb "not dirty"; we'll carefully arrange the writeback below
					vb->dirtyComponentsClear();

					// throws exception if nothing to apply
					ve_p->correct2(*vb, false, dowtsp);

					if (dowtsp) {
						vb->setWeightSpectrum(vb->weightSpectrum());
						// If WS was calibrated, set W to its channel-axis median
						vb->setWeight( partialMedians(vb->weightSpectrum(), IPosition(1, 1)) );
						newsigsp = 1.0f / sqrt(vb->weightSpectrum());
						vb->initSigmaSpectrum(newsigsp);
						vb->setSigma( partialMedians(newsigsp, IPosition(1, 1)) );
					} else {
						vb->setWeight(vb->weight());
						newsig = 1.0f / sqrt(vb->weight());
						vb->setSigma(newsig);
					}
					// Force writeback to disk
					vb->writeChangesBack();

				} else {//Not calibrating the spw
				  if (dowtsp && !wtspexists) {
				    // newly created WS Need to initialize
				    vb->initWeightSpectrum(newwtsp);
				  }
				  if (addsigsp) {
				    // newly created SS Need to initialize
				    vb->initSigmaSpectrum(newsigsp);
				    vb->writeChangesBack();
				  }
				}
			}
		}
		// clear-up Tsys caltable from list of apply
		unsetapply();

	} catch (AipsError x) {
		logSink() << LogIO::SEVERE << "Caught exception: " << x.getMesg()
				<< LogIO::POST;

		logSink() << "Resetting all calibration application settings."
				<< LogIO::POST;
		unsetapply();

		throw(AipsError("Error in Calibrater::initWeights."));
		retval = false;  // Not that it ever gets here...
	}
	return retval;
}

Bool OldCalibrater::solve() {

  logSink() << LogOrigin("Calibrater","solve") << LogIO::NORMAL3;

  try {

    if (!ok()) 
      throw(AipsError("Calibrater not prepared for solve."));

    // Handle nothing-to-solve-for case
    if (!svc_p)
      throw(AipsError("Please run setsolve before attempting to solve."));

    // Handle specified caltable
    if (true && svc_p) {

      /*      
      cout << "name: " << svc_p->calTableName() << endl;
      cout << boolalpha;
      cout << "append?   " << svc_p->append() << endl;
      cout << "opened?   " << Table::isOpened(svc_p->calTableName()) << endl;
      cout << "readable? " << Table::isReadable(svc_p->calTableName()) << endl;
      //      cout << "writable? " << Table::isWritable(svc_p->calTableName()) << endl;
      cout << "canDelete? " << Table::canDeleteTable(svc_p->calTableName(),true) << endl;
      */

      // If table exists (readable) and not deletable
      //   we have to abort (append=T requires deletable)
      if ( Table::isReadable(svc_p->calTableName()) &&
	   !Table::canDeleteTable(svc_p->calTableName()) ) {
	throw(AipsError("Specified caltable ("+svc_p->calTableName()+") exists and\n cannot be replaced (or appended to) because it appears to be open somewhere (Quit plotcal?)."));
      }
    }

    // Arrange VisEquation for solve
    ve_p->setsolve(*svc_p);

    // Ensure apply list properly sorted w.r.t. solvable term
    ve_p->setapply(vc_p);

    // Report what is being applied and solved-for
    applystate();
    solvestate();


    // Report correct/corrupt apply order
    //    ve_p->state();

    // Set the channel mask
    svc_p->setChanMask(chanmask_);

    // Generally use standard solver
    if (svc_p->useGenericGatherForSolve())
      genericGatherAndSolve();   // using VisBuffGroupAcc
    else {
      //cout << "Fully self-directed data gather and solve" << endl;
      // Fully self-directed data gather and solve
      svc_p->selfGatherAndSolve(*vs_p,*ve_p);
    }

    svc_p->clearChanMask();

  } catch (AipsError x) {
    logSink() << LogIO::SEVERE << "Caught exception: " << x.getMesg() << LogIO::POST;

    logSink() << "Reseting entire solve/apply state." << LogIO::POST;
    reset();

    throw(AipsError("Error in Calibrater::solve."));
    return false;
  } 

  return true;

}

Vector<Double> OldCalibrater::modelfit(const Int& niter,
				       const String& stype,
				       const Vector<Double>& par,
				       const Vector<Bool>& vary,
				       const String& file) {

 /*
  cout << "Calibrater::modelfit" << endl;
  cout << " niter = " << niter << endl;
  cout << " stype = " << stype << endl;
  cout << " par   = " << par << endl;
  cout << " vary  = " << vary << endl;
  cout << " file  = " << file << endl;
 */
  //  logSink() << LogOrigin("Calibrater","modelfit") << LogIO::NORMAL;
  
  try {
    if(!ok()) throw(AipsError("Calibrater not ok()"));

    // Construct UVMod with the VisSet
    UVMod uvmod(*vs_p);

    if (stype=="P")
      uvmod.setModel(ComponentType::POINT, par, vary);
    else if (stype=="G")
      uvmod.setModel(ComponentType::GAUSSIAN, par, vary);
    else if (stype=="D")
      uvmod.setModel(ComponentType::DISK, par, vary);
    else
      throw(AipsError("Unrecognized component type in Calibrater::modelfit."));
    
    // Run the fit
    uvmod.modelfit(niter,file);

    // Return the parameter vector
    return uvmod.par();

  } catch (AipsError x) {
    logSink() << LogIO::SEVERE << "Caught exception: " << x.getMesg() << LogIO::POST;
    throw(AipsError("Error in Calibrater::modelfit."));
    
    return Vector<Double>();
  } 

}

void OldCalibrater::fluxscale(const String& infile, 
			      const String& outfile,
			      const Vector<Int>& refField, 
			      const Vector<Int>& refSpwMap, 
			      const Vector<Int>& tranField,
			      const Bool& append,
			      const Float& inGainThres,
			      const String& antSel,
			      const String& timerangeSel,
			      const String& scanSel,
			      SolvableVisCal::fluxScaleStruct& oFluxScaleFactor,
			      const String& oListFile,
			      const Bool& incremental,
			      const Int& fitorder,
			      const Bool& display) {

  //  throw(AipsError("Method 'fluxscale' is temporarily disabled."));

  // TBD: write inputs to MSHistory
  logSink() << LogOrigin("Calibrater","fluxscale") << LogIO::NORMAL3;

  SolvableVisCal *fsvj_(NULL);
  try {
    // If infile is Calibration table
    if (Table::isReadable(infile) && 
	Table::tableInfo(infile).type()=="Calibration") {

      // get calibration type
      String caltype;
      caltype = Table::tableInfo(infile).subType();
      logSink() << "Table " << infile 
		<< " is of type: "<< caltype 
		<< LogIO::POST;
      String message="Table "+infile+" is of type: "+caltype;
      MSHistoryHandler::addMessage(*ms_p, message, "calibrater", "", "calibrater::fluxscale()");
      
      // form selection
      String select="";
      // Selection is empty for case of no tran specification
      if (tranField.nelements()>0) {
	
	// All selected fields
	Vector<Int> allflds = concatenateArray(refField,tranField);
	
	// Assemble TaQL
	ostringstream selectstr;
	selectstr << "FIELD_ID IN [";
	for (Int iFld=0; iFld<allflds.shape(); iFld++) {
	  if (iFld>0) selectstr << ", ";
	  selectstr << allflds(iFld);
	}
	selectstr << "]";
	select=selectstr.str();
      }

      // Construct proper SVC object
      if (caltype == "G Jones") {
	fsvj_ = createSolvableVisCal("G",*vs_p);
      } else if (caltype == "T Jones") {
	fsvj_ = createSolvableVisCal("T",*vs_p);
      } else {
	// Can't process other than G and T (add B?)
	ostringstream typeErr;
	typeErr << "Type " << caltype 
	       << " not supported in fluxscale.";

	throw(AipsError(typeErr.str()));
      }

      // fill table with selection
      RecordDesc applyparDesc;
      applyparDesc.addField ("table", TpString);
      applyparDesc.addField ("select", TpString);
      Record applypar(applyparDesc);
      applypar.define ("table", infile);
      applypar.define ("select", select);
      fsvj_->setApply(applypar);

      //Bool incremental=false;
      // Make fluxscale calculation
      Vector<String> fldnames(MSFieldColumns(ms_p->field()).name().getColumn());
      //fsvj_->fluxscale(refField,tranField,refSpwMap,fldnames,oFluxScaleFactor,
      fsvj_->fluxscale(outfile,refField,tranField,refSpwMap,fldnames,inGainThres,antSel,
        timerangeSel,scanSel,oFluxScaleFactor, oListFile,incremental,fitorder,display);
//        oListFile);
     
      // If no outfile specified, use infile (overwrite!)
      String out(outfile);
      if (out.length()==0)
	out = infile;

      // Store result
      if (append) {
	logSink() << "Appending result to " << out << LogIO::POST;
	String message="Appending result to "+out;
	MSHistoryHandler::addMessage(*ms_p, message, "calibrater", "", "calibrater::fluxscale()");
      } else {
	logSink() << "Storing result in " << out << LogIO::POST;
	String message="Storing result in "+out;
	MSHistoryHandler::addMessage(*ms_p, message, "calibrater", "", "calibrater::fluxscale()");
      }
      fsvj_->storeNCT(out,append);
      
      // Clean up
      delete fsvj_;

    } else {
      // Table not found/unreadable, or not Calibration
      ostringstream tabErr;
      tabErr << "File " << infile
	     << " does not exist or is not a Calibration Table.";

      throw(AipsError(tabErr.str()));

    }
  } catch (AipsError x) {

    logSink() << LogIO::SEVERE
	      << "Caught Exception: "
	      << x.getMesg()
	      << LogIO::POST;
    
    // Clean up
    if (fsvj_) delete fsvj_;

    // Write to MS History table
    //    String message="Caught Exception: "+x.getMesg();
    //    MSHistoryHandler::addMessage(*ms_p, message, "calibrater", "", "calibrater::fluxscale()");

    throw(AipsError("Error in Calibrater::fluxscale."));

    return;

  }
  return;


}

void OldCalibrater::accumulate(const String& intab,
			       const String& incrtab,
			       const String& outtab,
			       const String& fields,
			       const String& calFields,
			       const String& interp,
			       const Double& t,
			       const Vector<Int>& spwmap) {
  
  //  logSink() << LogOrigin("Calibrater","accumulate") << LogIO::NORMAL;

  logSink() << "Beginning accumulate." << LogIO::POST;

  // SVJ objects:
  SolvableVisCal *incal_(NULL), *incrcal_(NULL);

  try {
    
  /*
    cout << "intab     = " << intab << endl;
    cout << "incrtab   = " << incrtab << endl;
    cout << "outtab    = " << outtab << endl;
    cout << "fields    = " << fields << endl;
    cout << "calFields = " << calFields << endl;
    cout << "interp    = " << interp << endl;
    cout << "t         = " << t << endl;
  */

    // Incremental table's type sets the type we are dealing with
    String caltype=calTableType(incrtab);

    // If no input cumulative timescale specified, then
    //   a valid input cumulative table must be specified
    if (t < 0.0) {

      String intype=calTableType(intab);

      if (intype!=caltype) {
      
	ostringstream typeErr;
	typeErr << "Table " << intab
		<< " is not the same type as "
		<< incrtab << " (" << caltype << ")";
	throw(AipsError(typeErr.str()));
      }
    }

    // At this point all is ok; we will:
    //  o fill from intab and accumulate to it (t<0), OR
    //  o create a new cumulative table from scratch (t>0)

    // If creating a new cumulative table, it must span the whole dataset,
    //   so reset data selection to whole MS, and setup iterator
    if (t>0.0) {
      selectvis();
      Block<Int> columns;
      columns.resize(4);
      columns[0]=MS::ARRAY_ID;
      columns[1]=MS::TIME;
      columns[2]=MS::FIELD_ID;
      columns[3]=MS::DATA_DESC_ID;
      vs_p->resetVisIter(columns,t);
    }

    //	logSink() << "Table " << infile 
    //		  << " is of type: "<< caltype 
    //		  << LogIO::POST;

    incal_ = createSolvableVisCal(caltype,*vs_p);
    incrcal_ = createSolvableVisCal(caltype,*vs_p);
    
    // TBD: move to svj.setAccumulate?
    if ( !(incal_->accumulatable()) ) {
      ostringstream typeErr;
      typeErr << "Type " << caltype 
	      << " not yet supported in accumulate.";
      throw(AipsError(typeErr.str()));
    }

    // At this point, accumulation apparently supported,
    //   so continue...
    
    // initialize the cumulative solutions
    incal_->setAccumulate(*vs_p,intab,"",t,-1);
    

    /*
    // form selection on incr table
    String incrSel="";
    if (calFields.shape()>0) {
      
      // Assemble TaQL
      ostringstream selectstr;
      selectstr << "FIELD_ID IN [";
      for (Int iFld=0; iFld<calFields.shape(); iFld++) {
	if (iFld>0) selectstr << ", ";
	selectstr << calFields(iFld);
      }
      selectstr << "]";
      incrSel=selectstr.str();
    }
    */
    
    // fill incr table with selection
    logSink() << "Preparing to accumulate calibration from table: "
	      << incrtab
	      << LogIO::POST;

    // Set record format for calibration table application information
    RecordDesc applyparDesc;
    applyparDesc.addField ("t", TpDouble);
    applyparDesc.addField ("table", TpString);
    //    applyparDesc.addField ("select", TpString);
    applyparDesc.addField ("fieldstr", TpString);
    applyparDesc.addField ("interp", TpString);
    applyparDesc.addField ("spwmap",TpArrayInt);
    
    // Create record with the requisite field values
    Record applypar(applyparDesc);
    applypar.define ("t", t);
    applypar.define ("table", incrtab);
    //    applypar.define ("select", incrSel);
    applypar.define ("fieldstr", calFields);
    applypar.define ("interp", interp);
    applypar.define ("spwmap",spwmap);

    incrcal_->setApply(applypar);

    Vector<Int> fldidx(0);
    if (fields.length()>0)
      fldidx=getFieldIdx(fields);

    // All ready, now do the accumulation
    incal_->accumulate(incrcal_,fldidx);
    
    // ...and store the result
    logSink() << "Storing accumulated calibration in table: " 
	      << outtab
	      << LogIO::POST;

    if (outtab != "") 
      incal_->calTableName()=outtab;

    incal_->storeNCT();
    
    delete incal_;
    delete incrcal_;

    logSink() << "Finished accumulation." 
	      << LogIO::POST;

  } catch (AipsError x) {
    logSink() << LogIO::SEVERE
	      << "Caught Exception: "
	      << x.getMesg()
	      << LogIO::POST;

    if (incal_) delete incal_;
    if (incrcal_) delete incrcal_;
    
    throw(AipsError("Error in Calibrater::accumulate."));
    return;
  }
  return;

}

void OldCalibrater::specifycal(const String& type,
			       const String& caltable,
			       const String& time,
			       const String& spw,
			       const String& antenna,
			       const String& pol,
			       const Vector<Double>& parameter,
			       const String& infile,
			       const Bool& uniform) {

  logSink() << LogOrigin("Calibrater","specifycal") << LogIO::NORMAL;

  // SVJ objects:
  SolvableVisCal *cal_(NULL);

  try {
 			    
    // Set record format for calibration table application information
    RecordDesc specifyDesc;
    specifyDesc.addField ("caltable", TpString);
    specifyDesc.addField ("time", TpString);
    specifyDesc.addField ("spw", TpArrayInt);
    specifyDesc.addField ("antenna", TpArrayInt);
    specifyDesc.addField ("pol", TpString);
    specifyDesc.addField ("parameter", TpArrayDouble);
    specifyDesc.addField ("caltype",TpString);
    specifyDesc.addField ("infile",TpString);
    specifyDesc.addField ("uniform",TpBool);

    // Create record with the requisite field values
    Record specify(specifyDesc);
    specify.define ("caltable", caltable);
    specify.define ("time", time);
    if (spw=="*")
      specify.define ("spw",Vector<Int>(1,-1));
    else
      specify.define ("spw",getSpwIdx(spw));
    if (antenna=="*")
      specify.define ("antenna",Vector<Int>(1,-1) );
    else
      specify.define ("antenna",getAntIdx(antenna));
    specify.define ("pol",pol);
    specify.define ("parameter",parameter);
    specify.define ("caltype",type);
    specify.define ("infile",infile);
    specify.define ("uniform",uniform);

    // Now do it
    String utype=upcase(type);
    if (utype=="G" || utype.contains("AMP") || utype.contains("PH"))
      cal_ = createSolvableVisCal("G",*vs_p);
    else if (utype=='K' || utype.contains("SBD") || utype.contains("DELAY"))
      cal_ = createSolvableVisCal("K",*vs_p);
    else if (utype.contains("MBD"))
      cal_ = createSolvableVisCal("K",*vs_p);  // as of 5.3, KMBD is just K
    else if (utype.contains("ANTPOS"))
      cal_ = createSolvableVisCal("KANTPOS",*vs_p);
    else if (utype.contains("TSYS"))
      cal_ = createSolvableVisCal("TSYS",*vs_p);
    else if (utype.contains("EVLAGAIN") ||
	     utype.contains("SWP") ||
	     utype.contains("RQ"))
      cal_ = createSolvableVisCal("EVLASWP",*vs_p);
    else if (utype.contains("OPAC"))
      cal_ = createSolvableVisCal("TOPAC",*vs_p);
    else if (utype.contains("GC") || utype.contains("EFF"))
      cal_ = createSolvableVisCal("GAINCURVE",*vs_p);
    else if (utype.contains("TEC"))
      cal_ = createSolvableVisCal("TEC",*vs_p);
    else if (utype.contains("SDSKY_PS"))
      cal_ = createSolvableVisCal("SDSKY_PS",*vs_p);
    else if (utype.contains("SDSKY_RASTER"))
      cal_ = createSolvableVisCal("SDSKY_RASTER",*vs_p);
    else if (utype.contains("SDSKY_OTF"))
      cal_ = createSolvableVisCal("SDSKY_OTF",*vs_p);
    else
      throw(AipsError("Unrecognized caltype."));

    // set up for specification (set up the CalSet)
    cal_->setSpecify(specify);

    // fill with specified values
    cal_->specify(specify);

    // Store result
    cal_->storeNCT();

    delete cal_;

  } catch (AipsError x) {
    logSink() << LogIO::SEVERE
	      << "Caught Exception: "
	      << x.getMesg()
	      << LogIO::POST;

    if (cal_) delete cal_;
    
    throw(AipsError("Error in Calibrater::specifycal."));
    return;
  }
  return;

}

Bool OldCalibrater::smooth(const String& infile, 
			   String& outfile,  // const Bool& append,
			   const String& smoothtype,
			   const Double& smoothtime,
			   const String& fields)
{

  // TBD: support append?
  // TBD: spw selection?

  logSink() << LogOrigin("Calibrater","smooth") << LogIO::NORMAL;

  logSink() << "Beginning smoothing/interpolating method." << LogIO::POST;


  // A pointer to an SVC
  SolvableVisCal *svc(NULL);

  try {
    
    // Handle no in file 
    if (infile=="")
      throw(AipsError("Please specify an input calibration table."));

    // Handle bad smoothtype
    if (smoothtype!="mean" && smoothtype!="median")
      throw(AipsError("Unrecognized smooth type!"));

    // Handle bad smoothtime
    if (smoothtime<=0)
      throw(AipsError("Please specify a strictly positive smoothtime."));

    // Handle no outfile
    if (outfile=="") {
      outfile=infile;
      logSink() << "Will overwrite input file with smoothing result." 
		<< LogIO::POST;
    }


    svc = createSolvableVisCal(calTableType(infile),*vs_p);
    
    if (svc->smoothable()) {
      
      // Fill calibration table using setApply
      RecordDesc applyparDesc;
      applyparDesc.addField ("table", TpString);
      Record applypar(applyparDesc);
      applypar.define ("table", infile);
      svc->setApply(applypar);

      // Convert refFields/transFields to index lists
      Vector<Int> fldidx(0);
      if (fields.length()>0)
	fldidx=getFieldIdx(fields);

      // Delegate to SVC
      svc->smooth(fldidx,smoothtype,smoothtime);
      
      // Store the result on disk
      //    if (append) logSink() << "Appending result to " << outfile << LogIO::POST;
      //else 
      logSink() << "Storing result in " << outfile << LogIO::POST;
      
      
      if (outfile != "") 
	svc->calTableName()=outfile;
      svc->storeNCT();

      // Clean up
      if (svc) delete svc; svc=NULL;
      
      // Apparently, it worked
      return true;

    }
    else
      throw(AipsError("This type ("+svc->typeName()+") does not support smoothing."));

  } catch (AipsError x) {
   
    logSink() << LogIO::SEVERE
	      << "Caught Exception: "
	      << x.getMesg()
	      << LogIO::POST;
    // Clean up
    if (svc) delete svc; svc=NULL;

    throw(AipsError("Error in Calibrater::smooth."));

    return false;
  }
  return false;
}

// List a calibration table
Bool OldCalibrater::listCal(const String& infile,
			    const String& field,
			    const String& antenna,
			    const String& spw,
			    const String& listfile,
			    const Int& pagerows) {
    
    SolvableVisCal *svc(NULL);
    logSink() << LogOrigin("Calibrater","listCal");
    
    try {
        
        // Trap (currently) unsupported types
        if (upcase(calTableType(infile))=="GSPLINE" ||
            upcase(calTableType(infile))=="BPOLY")
            throw(AipsError("GSPLINE and BPOLY tables cannot currently be listed."));
        
        // Get user's selected fields, ants
        Vector<Int> ufldids=getFieldIdx(field);
        Vector<Int> uantids=getAntIdx(antenna);
        
        String newSpw = spw;
        Bool defaultSelect = false;
        if (spw.empty()) { // list all channels (default)
            defaultSelect = true;
            newSpw = "*"; 
            logSink() << LogIO::NORMAL1 << "Spws selected: ALL" << endl
                      << "Channels selected: ALL" << LogIO::POST;
        }
        // Get user's selected spw and channels
        Vector<Int> uspwids=getSpwIdx(newSpw);
        Matrix<Int> uchanids=getChanIdx(newSpw);
        if (!defaultSelect) {
            logSink() << LogIO::NORMAL1 << "Spw and Channel selection matrix: "
                      << endl << "Each rows shows: [ Spw , Start Chan , Stop Chan , Chan Step ]"
                      << endl << uchanids << LogIO::POST;
        }
        logSink() << LogIO::DEBUG2 
                  << "uspwids = "  << uspwids  << endl
                  << "uchanids = " << uchanids << LogIO::POST;
        
        // By default, do first spw, first chan
        if (uspwids.nelements()==0) {
            uchanids.resize(1,4);
            uchanids=0;
        } 
        
        // Set record format for calibration table application information
        RecordDesc applyparDesc;
        applyparDesc.addField ("table", TpString);
        
        // Create record with the requisite field values
        Record applypar(applyparDesc);
        applypar.define ("table", infile);
        
        // Generate the VisCal to be listed
        svc = createSolvableVisCal(calTableType(infile),*vs_p);  
        svc->setApply(applypar);       
        
        // list it
        svc->listCal(ufldids,uantids,uchanids,  //uchanids(0,0),uchanids(0,1),
                     listfile,pagerows);
        
        if (svc) delete svc; svc=NULL;
        
        return true;
        
    } catch (AipsError x) {
        
        logSink() << LogIO::SEVERE
                  << "Caught Exception: "
                  << x.getMesg()
                  << LogIO::POST;
        // Clean up
        if (svc) delete svc; svc=NULL;
        
        throw(AipsError("Error in Calibrater::listCal."));
        
        return false;
    }
    return false;
    
}

Bool OldCalibrater::initialize(MeasurementSet& inputMS, 
			       Bool compress,
			       Bool addScratch, Bool addModel)  {
  
  logSink() << LogOrigin("Calibrater","") << LogIO::NORMAL3;
  
  try {
    timer_p.mark();

    // Set pointer ms_p from input MeasurementSet
    if (ms_p) {
      *ms_p=inputMS;
    } else {
      ms_p = new MeasurementSet(inputMS);
      AlwaysAssert(ms_p,AipsError);
    };

    // Disabled 2016/04/19 (gmoellen): avoid direct MS.HISTORY 
    //   updates from below the python level, FOR NOW

    /*

    // Setup to write LogIO to HISTORY Table in MS
    if(!(Table::isReadable(ms_p->historyTableName()))){
      // create a new HISTORY table if its not there
      TableRecord &kws = ms_p->rwKeywordSet();
      SetupNewTable historySetup(ms_p->historyTableName(),
				 MSHistory::requiredTableDesc(),Table::New);
      kws.defineTable(MS::keywordName(MS::HISTORY), Table(historySetup));
      MSHistoryHandler::addMessage(*ms_p, "HISTORY Table added by Calibrater",
				   "Calibrater","","Calibrater::initialize()");
    }
    historytab_p=Table(ms_p->historyTableName(),
		       TableLock(TableLock::UserNoReadLocking), Table::Update);
    // jagonzal (CAS-4110): When the selectvis method throws an exception the initialize method
    // is called again to leave the calibrater in a proper state, and since there was a previous
    // initialization the history handler was already created, and has to be destroyed before
    // creating a new one to avoid leaveing the HISTORY table opened.
    if (hist_p) delete hist_p;
    hist_p= new MSHistoryHandler(*ms_p, "calibrater");

    // (2016/04/19) */

    // Remember the ms's name
    msname_p=ms_p->tableName();


    // Add/init scr cols, if requested (init is hard-wired)
    if (addScratch || addModel) {
      Bool alsoinit=true;
      VisSetUtil::addScrCols(*ms_p,addModel,addScratch,alsoinit,compress);
    }

    // Set the selected MeasurementSet to be the same initially
    // as the input MeasurementSet
    if (mssel_p) delete mssel_p;
    mssel_p=new MeasurementSet(*ms_p);
    
    logSink() << LogIO::NORMAL
	      << "Initializing nominal selection to the whole MS."
	      << LogIO::POST;


    // Create a VisSet with no selection
    // (gmoellen 2012/02/06: this merely makes a VisIter now)
    if (vs_p) {
      delete vs_p;
      vs_p=0;
    };
    Block<Int> nosort(0);
    Matrix<Int> noselection;
    Double timeInterval=0;
    // gmoellen 2012/02/06    vs_p=new VisSet(*ms_p,nosort,noselection,addScratch,timeInterval,compress, addModel);
    vs_p=new VisSet(*ms_p,nosort,noselection,false,timeInterval,false,false);

    // Size-up the chanmask PB
    initChanMask();

    // Create the associated VisEquation
    //  TBD: move to ctor and make it non-pointer
    if (ve_p) {
      delete ve_p;
      ve_p=0;
    };
    ve_p=new VisEquation();

    // Reset the apply/solve VisCals
    reset(true,true);

    return true;

  } catch (AipsError x) {
    logSink() << LogOrigin("Calibrater","initialize",WHERE) 
	      << LogIO::SEVERE << "Caught exception: " << x.getMesg() 
	      << LogIO::POST;
    cleanup();
    cleanupVisSet();
    if (ms_p) delete ms_p; ms_p=NULL;
    if (hist_p) delete hist_p; hist_p=NULL;

    throw(AipsError("Error in Calibrater::initialize()"));
    return false;
  } 
  return false;
}

Bool OldCalibrater::initCalSet(const Int& calSet) 
{

  //  logSink() << LogOrigin("Calibrater","initCalSet") << LogIO::NORMAL3;

  if (vs_p) {

    Block<Int> columns;
    // include scan iteration, for more optimal iteration
    columns.resize(5);
    columns[0]=MS::ARRAY_ID;
    columns[1]=MS::SCAN_NUMBER;
    columns[2]=MS::FIELD_ID;
    columns[3]=MS::DATA_DESC_ID;
    columns[4]=MS::TIME;
    vs_p->resetVisIter(columns,0.0);

    vs_p->initCalSet(calSet);
    return true;
  }
  else {
    throw(AipsError("Calibrater cannot initCalSet"));
    return false;
  }
}

Bool OldCalibrater::cleanupVisSet() {

  //  logSink() << LogOrigin("OldCalibrater","cleanupVisSet") << LogIO::NORMAL;

  if(vs_p) delete vs_p; vs_p=0;

  // Delete chanmask
  initChanMask();

  return true;

}

VisibilityIterator::DataColumn OldCalibrater::configureForCorrection ()
{
    if (!ok())
      throw(AipsError("Calibrater not prepared for correct!"));

    // Nominally, we write out to the CORRECTED_DATA, unless absent
    VisibilityIterator::DataColumn whichOutCol(VisibilityIterator::Corrected);

    if (!ms_p->tableDesc().isColumn("CORRECTED_DATA"))
      throw(AipsError("CORRECTED_DATA column unexpectedly absent. Cannot correct."));

    // Ensure apply list non-zero and properly sorted
    ve_p->setapply(vc_p);

    // Report the types that will be applied
    applystate();

    // Arrange for iteration over data
    Block<Int> columns;
    // include scan iteration
    columns.resize(5);
    columns[0]=MS::ARRAY_ID;
    columns[1]=MS::SCAN_NUMBER;
    columns[2]=MS::FIELD_ID;
    columns[3]=MS::DATA_DESC_ID;
    columns[4]=MS::TIME;

    // Reset the VisibilityIterator in the VisSet. 
    vs_p->resetVisIter (columns, 0.0);

    return whichOutCol;
}

void OldCalibrater::selectChannel(const String& spw) {

  // Initialize the chanmask_
  initChanMask();

  if (mss_p && mssel_p) {

    // Refresh the frequencySelections object to feed to VI2, if relevant
    frequencySelections_p.reset(new vi::FrequencySelections());

    vi::FrequencySelectionUsingChannels usingChannels;
    usingChannels.add(*mss_p,mssel_p);
    frequencySelections_p->add(usingChannels);

    //    cout << usingChannels.toString() << endl;
    //    cout << "FS.size() = " << frequencySelections_p->size() << endl;

  }

  Matrix<Int> chansel = getChanIdx(spw);
  uInt nselspw=chansel.nrow();

  if (nselspw==0)
    logSink() << "Frequency selection: Selecting all channels in all spws." 
	      << LogIO::POST;
  else {

    logSink() << "Frequency selection: " << LogIO::POST;

    // Trap non-unit step (for now)
    if (ntrue(chansel.column(3)==1)!=nselspw) {
      logSink() << LogIO::WARN
		<< "Calibration does not support non-unit channel stepping; "
		<< "using step=1."
		<< LogIO::POST;
      chansel.column(3)=1;
    }

    Int nspw=vs_p->numberSpw();
    Vector<Int> nChan0;
    nChan0 = vs_p->numberChan();

    Vector<Int> uspw(chansel.column(0));
    Vector<Int> ustart(chansel.column(1));
    Vector<Int> uend(chansel.column(2));

    Vector<Int> start(nspw,INT_MAX);
    Vector<Int> end(nspw,-INT_MAX);
    logSink() << LogIO::NORMAL;
    for (uInt i=0;i<nselspw;++i) {
      
      Int& spw=uspw(i);

      // Initialize this spw mask, if necessary (def = masked)
      if (!chanmask_[spw])
      	chanmask_[spw]=new Vector<Bool>(nChan0(spw),true);

      // revise net start/end/nchan
      start(spw)=min(start(spw),ustart(i));
      end(spw)=max(end(spw),uend(i));
      Int nchan=end(spw)-start(spw)+1;  // net inclusive nchan

      // User's 
      Int step=chansel(i,3);
      Int unchan=uend(i)-ustart(i)+1;
      
      // Update the mask (false = valid)
      (*chanmask_[spw])(Slice(ustart(i),unchan))=false;


      logSink() << ".  Spw " << spw << ":"
		<< ustart(i) << "~" << uend(i) 
		<< " (" << uend(i)-ustart(i)+1 << " channels,"
		<< " step by " << step << ")"
		<< endl;

  /*
      cout << i << " " << spw << " {" 
	   << start(spw) << " [" << ustart(i) << " " 
	   << uend(i) << "] " << end(spw) << "}" << endl;
      cout << "chanmask = ";
      for (Int j=0;j<nChan0(spw);++j) cout << (*chanmask_[spw])(j);
      cout << endl << endl;
  */
	
      // Call via VisSet (avoid call to VisIter::origin)
      vs_p->selectChannel(1,start(spw),nchan,step,spw,false);
	
    } // i
    logSink() << LogIO::POST;

  } // non-triv spw selection

  // For testing:
  if (false) {

    VisIter& vi(vs_p->iter());
    VisBuffer vb(vi);
    
    // Pass each timestamp (VisBuffer) to VisEquation for correction
    for (vi.originChunks(); vi.moreChunks(); vi.nextChunk()) {
      vi.origin();
      //      for (vi.origin(); vi.more(); vi++)
	cout << vb.spectralWindow() << " "
	     << vb.nChannel() << " "
	     << vb.channel() << " "
	     << vb.visCube().shape()
	     << endl;
    }
  }

}

void OldCalibrater::initChanMask() {

  for (uInt i=0;i<chanmask_.nelements();++i) 
    if (chanmask_[i])
      delete chanmask_[i];
  if (vs_p) {
    chanmask_.resize(vs_p->numberSpw(),true);
    chanmask_=NULL;
  }
  else {
    //    throw(AipsError("Trouble sizing chanmask!"));
    // just don't support channel masking:
    chanmask_.resize(0,true);
  }

}

// Select on channel in the VisSet
void OldCalibrater::selectChannel(const String& mode, 
			       const Int& nchan, 
			       const Int& start, const Int& step,
			       const MRadialVelocity& mStart,
			       const MRadialVelocity& mStep) {
  
  // Set data selection variables
  dataMode_p=mode;
  dataNchan_p=nchan;
  if (dataNchan_p<0) dataNchan_p=0; 
  dataStart_p=start;
  if (dataStart_p<0) dataNchan_p=0; 
  dataStep_p=step;
  if (dataStep_p<1) dataNchan_p=1; 
  
  mDataStart_p=mStart;
  mDataStep_p=mStep;
  
  // Select on frequency channel
  if(dataMode_p=="channel") {
    // *** this bit here is temporary till we unifomize data selection
    //Getting the selected SPWs
    MSMainColumns msc(*mssel_p);
    Vector<Int> dataDescID = msc.dataDescId().getColumn();
    Bool dum;
    Sort sort( dataDescID.getStorage(dum),sizeof(Int) );
    sort.sortKey((uInt)0,TpInt);
    Vector<uInt> index,uniq;
    sort.sort(index,dataDescID.nelements());
    uInt nSpw = sort.unique(uniq,index);
    
    Vector<Int> selectedSpw(nSpw);
    Vector<Int> nChan(nSpw);
    for (uInt k=0; k < nSpw; ++k) {
      selectedSpw[k]=dataDescID[index[uniq[k]]];
      nChan[k]=vs_p->numberChan()(selectedSpw[k]);
      
    }
    if(dataNchan_p==0) dataNchan_p=vs_p->numberChan()(selectedSpw[0]);
    if(dataStart_p<0) {
      logSink() << LogIO::SEVERE << "Illegal start pixel = " 
		<< dataStart_p << LogIO::POST;
    }
    Int end = Int(dataStart_p) + Int(dataNchan_p) * Int(dataStep_p);
    for (uInt k=0; k < selectedSpw.nelements() ; ++k){
      if(end < 1 || end > nChan[k]) {
	logSink() << LogIO::SEVERE << "Illegal step pixel = " << dataStep_p
		  << " in Spw " << selectedSpw[k]
		  << LogIO::POST;
      }
      logSink() << "Selecting "<< dataNchan_p
		<< " channels, starting at visibility channel "
		<< dataStart_p  << " stepped by "
		<< dataStep_p << " in Spw " << selectedSpw[k] << LogIO::POST;
      
      // Set frequency channel selection for all spectral window id's
      Int nch;
      //Vector<Int> nChan=vs_p->numberChan();
      //Int nSpw=vs_p->numberSpw();
      if (dataNchan_p==0) {
	nch=nChan(k);
      }else {
	nch=dataNchan_p;
      };
      vs_p->selectChannel(1,dataStart_p,nch,dataStep_p,selectedSpw[k]);
      
    }
  }
  // Select on velocity
  else if (dataMode_p=="velocity") {
    MVRadialVelocity mvStart(mDataStart_p.get("m/s"));
    MVRadialVelocity mvStep(mDataStep_p.get("m/s"));
    MRadialVelocity::Types
      vType((MRadialVelocity::Types)mDataStart_p.getRefPtr()->getType());
    logSink() << "Selecting "<< dataNchan_p
	      << " channels, starting at radio velocity " << mvStart
	      << " stepped by " << mvStep << ", reference frame is "
	      << MRadialVelocity::showType(vType) << LogIO::POST;
    vs_p->iter().selectVelocity(Int(dataNchan_p), mvStart, mvStep,
				vType, MDoppler::RADIO);
  }
  
  // Select on optical velocity
  else if (dataMode_p=="opticalvelocity") {
    MVRadialVelocity mvStart(mDataStart_p.get("m/s"));
    MVRadialVelocity mvStep(mDataStep_p.get("m/s"));
    MRadialVelocity::Types
      vType((MRadialVelocity::Types)mDataStart_p.getRefPtr()->getType());
    logSink() << "Selecting "<< dataNchan_p
	      << " channels, starting at optical velocity " << mvStart
	      << " stepped by " << mvStep << ", reference frame is "
	      << MRadialVelocity::showType(vType) << LogIO::POST;
    vs_p->iter().selectVelocity(Int(dataNchan_p), mvStart, mvStep,
				vType, MDoppler::OPTICAL);
  }
  
  
}

Bool OldCalibrater::ok() {

  if(vs_p && ms_p && mssel_p && ve_p) {
    return true;
  }
  else {
    logSink() << "Calibrater is not yet initialized" << LogIO::POST;
    return false;
  }
}

Bool OldCalibrater::genericGatherAndSolve() {

#ifdef _OPENMP
  Double Tsetup(0.0),Tgather(0.0),Tsolve(0.0);
  Double time0=omp_get_wtime();
#endif

  //cout << "Generic gather and solve." << endl;

  // Create the solver
  VisCalSolver vcs;
  
   // Inform logger/history
  logSink() << "Solving for " << svc_p->typeName()
	    << LogIO::POST;
  
  // Initialize the svc according to current VisSet
  //  (this counts intervals, sizes CalSet)
  Vector<Int> nChunkPerSol;
  Int nSol = svc_p->sizeUpSolve(*vs_p,nChunkPerSol);

  // Create the in-memory (New)CalTable
  svc_p->createMemCalTable();

  // The iterator, VisBuffer
  VisIter& vi(vs_p->iter());
  VisBuffer vb(vi);
  
  Vector<Int> slotidx(vs_p->numberSpw(),-1);

  // We will remember which spws couldn't be processed
  Vector<Bool> unsolspw(vi.numberSpw());	
  unsolspw.set(false);		       

  // Manage verbosity of partial channel averaging
  Vector<Bool> verb(vi.numberSpw(),true);

  Vector<Int64> nexp(vi.numberSpw(),0), natt(vi.numberSpw(),0),nsuc(vi.numberSpw(),0);

#ifdef _OPENMP
  Tsetup+=(omp_get_wtime()-time0);
#endif

  Int nGood(0);
  vi.originChunks();
  for (Int isol=0;isol<nSol && vi.moreChunks();++isol) {

#ifdef _OPENMP
    time0=omp_get_wtime();
#endif

    nexp(vi.spectralWindow())+=1;

    // capture obs, scan info so we can set it later 
    //   (and not rely on what the VB averaging code can't properly do)
    Vector<Int> scv,obsv;
    Int solscan=vi.scan(scv)(0),solobs=vi.observationId(obsv)(0);

    // Arrange to accumulate 
    //    VisBuffAccumulator vba(vs_p->numberAnt(),svc_p->preavg(),false); 
    VisBuffGroupAcc vbga(vs_p->numberAnt(),vs_p->numberSpw(),vs_p->numberFld(),svc_p->preavg()); 
    
    for (Int ichunk=0;ichunk<nChunkPerSol(isol);++ichunk) {
    
      // Current _chunk_'s spw
      Int spw(vi.spectralWindow());
    
      // Only accumulate for solve if we can pre-calibrate
      if (ve_p->spwOK(spw)) {

	// Collapse each timestamp in this chunk according to VisEq
	//  with calibration and averaging
	for (vi.origin(); vi.more(); vi++) {
	  
	  // Force read of the field Id
	  vb.fieldId();

	  // Apply the channel mask (~no-op, if unnecessary)
	  svc_p->applyChanMask(vb);
	  
	  // This forces the data/model/wt I/O, and applies
	  //   any prior calibrations
	  ve_p->collapse(vb);
	  
	  // If permitted/required by solvable component, normalize
	  if (svc_p->normalizable()) 
	    vb.normalize();
	  
	  // If this solve not freqdep, and channels not averaged yet, do so
	  if (!svc_p->freqDepMat() && vb.nChannel()>1)
	    vb.freqAveCubes();

	  if (svc_p->freqDepPar() && 
	      svc_p->fsolint()!="none" &&
	      svc_p->fintervalCh()>0.0) {
	    //	    cout << "svc_p->currSpw() = " << svc_p->currSpw() << endl;
	    if (verb(spw)) 
	      logSink() << " Reducing nchan in spw " 
			<< spw
			<< " from " << vb.nChannel();
	    vb.channelAve(svc_p->chanAveBounds(spw));

	    // Kludge for 3.4 to reset corr-indep flag to correct channel axis shape
	    // (because we use vb.flag() below, rather than vb.flagCube())
	    vb.flag().assign(operator>(partialNTrue(vb.flagCube(),IPosition(1,0)),0UL));

	    if (verb(spw)) {
	      logSink() << " to " 
			<< vb.nChannel() << LogIO::POST;
	      verb(spw)=false;  // suppress future verbosity in this spw
	    }
	  }
	  
	  // Accumulate collapsed vb in a time average
	  //  (only if the vb contains any unflagged data)
	  if (nfalse(vb.flag())>0)
	    vbga.accumulate(vb);
	  
	}
      }
      else
	// This spw not accumulated for solve
	unsolspw(spw)=true;

      // Advance the VisIter, if possible
      if (vi.moreChunks()) vi.nextChunk();

    }
    
    // Finalize the averged VisBuffer
    vbga.finalizeAverage();

    // Establish meta-data for this interval
    //  (some of this may be used _during_ solve)
    //  (this sets currSpw() in the SVC)
    Bool vbOk=(vbga.nBuf()>0 && svc_p->syncSolveMeta(vbga));

    svc_p->overrideObsScan(solobs,solscan);

#ifdef _OPENMP
    Tgather+=(omp_get_wtime()-time0);
    time0=omp_get_wtime();
#endif

    if (vbOk) {

      // Use spw of first VB in vbga
      // TBD: (currSpw==thisSpw) here??  (I.e., use svc_p->currSpw()?  currSpw is prot!)
      Int thisSpw=svc_p->spwMap()(vbga(0).spectralWindow());
    
      natt(thisSpw)+=1;

      slotidx(thisSpw)++;
      
      // Make data amp- or phase-only, if needed
      vbga.enforceAPonData(svc_p->apmode());
      
      // Select on correlation via weights, according to the svc
      vbga.enforceSolveCorrWeights(svc_p->phandonly());

      if (svc_p->useGenericSolveOne()) {
	// generic individual solve

	//cout << "Generic individual solve: isol=" << isol << endl;

	// First guess
	svc_p->guessPar(vbga(0));
	
	// Solve for each parameter channel (in curr Spw)
	
	// (NB: force const version of nChanPar()  [why?])
	//	for (Int ich=0;ich<((const SolvableVisCal*)svc_p)->nChanPar();++ich) {
	Bool totalGoodSol(false);
	for (Int ich=((const SolvableVisCal*)svc_p)->nChanPar()-1;ich>-1;--ich) {
	  // for (Int ich=0;ich<((const SolvableVisCal*)svc_p)->nChanPar();++ich) {
	  
	  // If pars chan-dep, SVC mechanisms for only one channel at a time
	  svc_p->markTimer();
	  svc_p->focusChan()=ich;
	  
	  //	  svc_p->state();
	  
	  //	  cout << "Starting solution..." << endl;

	  // Pass VE, SVC, VB to solver
	  Bool goodSoln=vcs.solve(*ve_p,*svc_p,vbga);
	  
	  //	  cout << "goodSoln= " << boolalpha << goodSoln << endl;

	  // If good... 
	  if (goodSoln) {
	    totalGoodSol=true;
	    
	    svc_p->formSolveSNR();
	    svc_p->applySNRThreshold();
	    
	    // ..and file this solution in the correct slot
	    if (svc_p->freqDepPar())
	      svc_p->keep1(ich);
	    //	    svc_p->keep(slotidx(thisSpw));
	    //      Int n=svc_p->nSlots(thisSpw);
	    //	    svc_p->printActivity(n,slotidx(thisSpw),vi.fieldId(),thisSpw,nGood);	      
	    
	  }
	  else {
	    // report where this failure occured
	    svc_p->currMetaNote();
	    if (svc_p->freqDepPar())
	      // We must record a flagged solution for this channel
	      svc_p->keep1(ich);
	  }
	  
	} // parameter channels

	if (totalGoodSol) {
	  svc_p->keepNCT();
	  nsuc(thisSpw)+=1;
	}
	
	// Count good solutions.
	if (totalGoodSol)	nGood++;
	
      }
      else {
	//cout << "Self-directed individual solve: isol=" << isol << endl;
	// self-directed individual solve
	// TBD: selfSolveOne should return T/F for "good"
	svc_p->selfSolveOne(vbga);

	// File this solution in the correct slot of the CalSet
	svc_p->keepNCT();

	nGood++;
	nsuc(thisSpw)+=1;
      } 
	
    } // vbOK

#ifdef _OPENMP
    Tsolve+=(omp_get_wtime()-time0);
#endif

  } // isol

  logSink() << "  Found good " 
	    << svc_p->typeName() << " solutions in "
	    << nGood << " slots."
	    << LogIO::POST;
#ifdef _OPENMP
 #ifdef REPORT_CAL_TIMING
  cout << "OldCalibrater::genericGatherAndSolve Timing: " << endl;
  cout << " setup=" << Tsetup
       << " gather=" << Tgather 
       << " solve=" << Tsolve 
       << " total=" << Tsetup+Tgather+Tsolve
       << endl;
 #endif
#endif

  summarize_uncalspws(unsolspw, "solv");
  
  // Store whole of result in a caltable
  if (nGood==0) {
    logSink() << "No output calibration table written."
	      << LogIO::POST;
  }
  else {

    // TBD: Remove BPOLY specificity here
    if (svc_p->typeName()!="BPOLY") {
      // Do global post-solve tinkering (e.g., phase-only, normalization, etc.)
      svc_p->globalPostSolveTinker();

      // write the table
      svc_p->storeNCT();

    }
  }

  // Fill activity record
  actRec_=Record();
  actRec_.define("origin","Calibrater::genericGatherAndSolve");
  actRec_.define("nExpected",nexp);
  actRec_.define("nAttempt",natt);
  actRec_.define("nSucceed",nsuc);

  { 
    Record solveRec=svc_p->solveActionRec();
    if (solveRec.nfields()>0)
      actRec_.merge(solveRec);
  }



  return true;

}


} //# NAMESPACE CASA - END
