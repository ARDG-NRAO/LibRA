//# EVLASwPow.cc: Implementation of EVLA Switched Power Calibration
//# Copyright (C) 1996,1997,1998,1999,2000,2001,2002,2003
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

#include <synthesis/MeasurementComponents/EVLASwPow.h>

#include <synthesis/MeasurementComponents/MSMetaInfoForCal.h>

#include <ms/MeasurementSets/MSColumns.h>
#include <casa/BasicMath/Math.h>
#include <tables/Tables/Table.h>
#include <tables/Tables/TableIter.h>
#include <synthesis/CalTables/CTGlobals.h>

#include <casa/BasicSL/String.h>
#include <casa/Utilities/Assert.h>
#include <casa/Utilities/GenSort.h>
#include <casa/Exceptions/Error.h>
#include <casa/System/Aipsrc.h>
#include <casa/System/ProgressMeter.h>

#include <casa/sstream.h>

#include <casa/Logging/LogMessage.h>
#include <casa/Logging/LogSink.h>



using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN


// **********************************************************
//  EVLASwPow Implementations
//

EVLASwPow::SPType EVLASwPow::sptype(const String name) {
  String utype=upcase(name);
  if (utype.contains("SWP/RQ"))
    return EVLASwPow::SWPOVERRQ;
  if (utype.contains("SWPOW"))
    return EVLASwPow::SWPOW;
  if (utype.contains("EVLAGAIN"))
    return EVLASwPow::SWPOW;
  if (utype.contains("RQ"))
    return EVLASwPow::RQ;

  // Only get here if name unrecognized
  throw(AipsError(name+" is not among recognized EVLA Switched Power types ('swpow','evlagain','rq','swp/rq')"));

  // Should never reach here, but this is accurate (and avoids compiler warning)
  return EVLASwPow::NONE;
  
}

String EVLASwPow::sptype(EVLASwPow::SPType sptype) {
  switch (sptype) {
  case EVLASwPow::SWPOW: {
    return String("swpow");
    break;
  }
  case EVLASwPow::RQ: {
    return String("rq");
    break;
  }
  case EVLASwPow::SWPOVERRQ: {
    return String("swpow/rq");
    break;
  }
  case EVLASwPow::NONE:
  default: {
    throw(AipsError("Unrecognized EVLA Switched Power type"));
  }
  }
  // Should never reach here, but this is accurate (and avoids compiler warning)
  return String("None");
}

EVLASwPow::EVLASwPow(VisSet& vs) :
  VisCal(vs),             // virtual base
  VisMueller(vs),         // virtual base
  GJones(vs),              // immediate parent
  sysPowTabName_(vs.syspowerTableName()),
  calDevTabName_(vs.caldeviceTableName()),
  correff_(Float(0.932)),     // EVLA-specific net corr efficiency (4bit)
  frgrotscale_(Float(1.176)), // EVLA-specific fringe rotation mean _scale_
  //nyquist_(1.0),
  effChBW_()

{
  if (prtlev()>2) cout << "EVLASwPow::EVLASwPow(vs)" << endl;

  nChanParList().set(1);
  startChanList().set(0);

  // Get spw total bandwidths
  const MSSpWindowColumns& spwcols = vs.iter().msColumns().spectralWindow();
  effChBW_.resize(nSpw());
  for (Int ispw=0;ispw<nSpw();++ispw) 
    effChBW_(ispw)=Vector<Double>(spwcols.effectiveBW()(0))(0);
  
}

EVLASwPow::EVLASwPow(String msname,Int MSnAnt,Int MSnSpw) :
  VisCal(msname,MSnAnt,MSnSpw),             // virtual base
  VisMueller(msname,MSnAnt,MSnSpw),         // virtual base
  GJones(msname,MSnAnt,MSnSpw),             // immediate parent
  sysPowTabName_(""),
  calDevTabName_(""),
  correff_(Float(0.932)),     // EVLA-specific net corr efficiency (4bit)
  frgrotscale_(Float(1.176)), // EVLA-specific fringe rotation mean _scale_
  //nyquist_(1.0),
  effChBW_()

{
  if (prtlev()>2) cout << "EVLASwPow::EVLASwPow(msname,MSnAnt,MSnSpw)" << endl;

  nChanParList().set(1);
  startChanList().set(0);

  // Temporary MS to get some info
  MeasurementSet ms(msname);

  // The relevant subtable names (there must be a better way...)
  sysPowTabName_ = ms.rwKeywordSet().asTable("SYSPOWER").tableName();
  calDevTabName_ = ms.rwKeywordSet().asTable("CALDEVICE").tableName();

  // Get spw total bandwidths
  MSColumns mscol(ms);
  const MSSpWindowColumns& spwcols = mscol.spectralWindow();
  effChBW_.resize(nSpw());
  for (Int ispw=0;ispw<nSpw();++ispw) 
    effChBW_(ispw)=Vector<Double>(spwcols.effectiveBW()(0))(0);
  
}

EVLASwPow::EVLASwPow(const MSMetaInfoForCal& msmc) :
  VisCal(msmc),             // virtual base
  VisMueller(msmc),         // virtual base
  GJones(msmc),             // immediate parent
  sysPowTabName_(""),
  calDevTabName_(""),
  correff_(Float(0.932)),     // EVLA-specific net corr efficiency (4bit)
  frgrotscale_(Float(1.176)), // EVLA-specific fringe rotation mean _scale_
  //nyquist_(1.0),
  effChBW_()

{
  if (prtlev()>2) cout << "EVLASwPow::EVLASwPow(msmc)" << endl;

  nChanParList().set(1);
  startChanList().set(0);

  // Temporary MS to get some info
  MeasurementSet ms(this->msmc().msname());

  // The relevant subtable names (there must be a better way...)
  sysPowTabName_ = ms.rwKeywordSet().asTable("SYSPOWER").tableName();
  calDevTabName_ = ms.rwKeywordSet().asTable("CALDEVICE").tableName();

  // Get spw total bandwidths
  MSColumns mscol(ms);
  const MSSpWindowColumns& spwcols = mscol.spectralWindow();
  effChBW_.resize(nSpw());
  for (Int ispw=0;ispw<nSpw();++ispw) 
    effChBW_(ispw)=Vector<Double>(spwcols.effectiveBW()(0))(0);
  
}

EVLASwPow::EVLASwPow(const Int& nAnt) :
  VisCal(nAnt), 
  VisMueller(nAnt),
  GJones(nAnt)
{
  if (prtlev()>2) cout << "EVLASwPow::EVLASwPow(nAnt)" << endl;

  throw(AipsError("Cannot use EVLASwPow with generic ctor."));

}

EVLASwPow::~EVLASwPow() {
  if (prtlev()>2) cout << "EVLASwPow::~EVLASwPow()" << endl;
}

void EVLASwPow::setSpecify(const Record& specify) {

  LogMessage message(LogOrigin("EVLASwPow","setSpecify"));

  // Escape if SYSPOWER or CALDEVICE tables absent
  if (!Table::isReadable(sysPowTabName_))
    throw(AipsError("The SYSPOWER subtable is not present in the specified MS."));
  if (!Table::isReadable(calDevTabName_))
    throw(AipsError("The CALDEVICE subtable is not present in the specified MS."));

  // Not actually applying or solving
  setSolved(false);
  setApplied(false);

  // Collect Cal table parameters
  if (specify.isDefined("caltable")) {
    calTableName()=specify.asString("caltable");

    if (Table::isReadable(calTableName()))
      logSink() << "FYI: We are going to overwrite an existing CalTable: "
		<< calTableName()
		<< LogIO::POST;
  }

  // we are creating a table from scratch
  logSink() << "Creating " << typeName()
    //	    << " table from CANNED VALUES."
    	    << " table from MS SYSPOWER/CALDEVICE subtables."
	    << LogIO::POST;

  // Create a caltable to fill up
  createMemCalTable();

  // Init the shapes of solveAllRPar, etc.
  initSolvePar();

}

void EVLASwPow::specify(const Record& specify) {

  // Escape if SYSPOWER or CALDEVICE tables absent
  if (!Table::isReadable(sysPowTabName_))
    throw(AipsError("The SYSPOWER subtable is not present in the specified MS."));
  if (!Table::isReadable(calDevTabName_))
    throw(AipsError("The CALDEVICE subtable is not present in the specified MS."));
 
  // Fill the Tcals first
  fillTcals();

  // Discern which kind of calibration to calculate
  SPType swptype(EVLASwPow::SWPOW);
  if (specify.isDefined("caltype")) {
    String ctype=specify.asString("caltype");
    swptype=sptype(ctype);
    //cout << "caltype=" << ctype << " " << swptype << " " << sptype(swptype) << endl;
  }

  logSink() << "Filling switched-power (" << sptype(swptype) << ") data from the SYSPOWER table." << LogIO::POST;

  // The net digital factor for antenna-based (voltage) gain
  Float dig=sqrt(correff_*frgrotscale_);

  // Keep a count of the number of Tsys found per spw/ant
  Matrix<Int> goodcount(nSpw(),nElem(),0), badcount(nSpw(),nElem(),0);

  Block<String> columns(2);
  columns[0] = "TIME";
  columns[1] = "SPECTRAL_WINDOW_ID";
  Table sysPowTab(sysPowTabName_,Table::Old);
  TableIterator sysPowIter(sysPowTab,columns);

  // Count iterations
  Int niter(0);
  while (!sysPowIter.pastEnd()) {
    ++niter;
    sysPowIter.next();
  }
  sysPowIter.reset();

  logSink() << "Found " << niter << " TIME/SPW switched-power samples in SYSPOWER table" << LogIO::POST;

  // Iterate
  // Vectors for referencing slices of working info
  Vector<Float> currpsum,currpdif,currrq,currtcal,gain,tsys;  

  // Emit progress meter reports (at least until we improve performance)
  cerr << "Switched-Power ("+sptype(swptype)+") calculation: 0";
  ProgressMeter pm(0.,niter , "", "", "", "", true, niter/100);

  Int iter(0);
  while (!sysPowIter.pastEnd()) {

    // Update the progress meter
    pm.update(iter);

    Table itab(sysPowIter.table());

    ScalarColumn<Int> spwCol(itab,"SPECTRAL_WINDOW_ID");
    ScalarColumn<Double> timeCol(itab,"TIME");
    ScalarColumn<Double> intervalCol(itab,"INTERVAL");
    ScalarColumn<Int> antCol(itab,"ANTENNA_ID");
    ArrayColumn<Float> swsumCol(itab,"SWITCHED_SUM");
    ArrayColumn<Float> swdiffCol(itab,"SWITCHED_DIFF");
    ArrayColumn<Float> rqCol(itab,"REQUANTIZER_GAIN");

    Int ispw=spwCol(0);

    Double timestamp=timeCol(0);
    //    Double interval=intervalCol(0);

    Vector<Int> ants;
    antCol.getColumn(ants);

    Matrix<Float> psum,pdif,rq;
    swsumCol.getColumn(psum);
    swdiffCol.getColumn(pdif);
    rqCol.getColumn(rq);
    IPosition psumshape(psum.shape());
    IPosition pdifshape(pdif.shape());

    AlwaysAssert(psumshape.isEqual(pdifshape),AipsError);

    // the number of receptors
    Int nrec=psumshape(0);

    // Now prepare to record pars in the caltable
    currSpw()=ispw;
    refTime()=timestamp;
    currField()=msmc().fieldIdAtTime(timestamp);
    currScan()=msmc().scanNumberAtTime(timestamp);

    // Initialize solveAllRPar, etc.
    solveAllRPar()=1.0;
    solveAllParOK()=false;  
    solveAllParErr()=0.0;  // what should we use here?  ~1/bandwidth?
    solveAllParSNR()=1.0;

    IPosition blc(3,0,0,0),trc(3,2*nrec-1,0,0),stp(3,nrec,1,1);
    for (uInt iant=0;iant<ants.nelements();++iant) {
      Int thisant=ants(iant);

      // Reference this ant's info
      currpsum.reference(psum.column(iant));
      currpdif.reference(pdif.column(iant));
      currrq.reference(rq.column(iant));
      currtcal.reference(tcals_.xyPlane(ispw).column(thisant));

      // If any of the required values are goofy, we'll skip this antenna
      Bool good;
      switch (swptype) {
      case EVLASwPow::SWPOW: {
	good=(allGT(currtcal,FLT_EPSILON) &&
	      allGT(currpdif,FLT_EPSILON) &&
	      allGT(currpsum,FLT_EPSILON));
      }
      case EVLASwPow::SWPOVERRQ:{
	good=(allGT(currtcal,FLT_EPSILON) &&
	      allGT(currpdif,FLT_EPSILON) &&
	      allGT(currpsum,FLT_EPSILON) &&
              allGT(currrq,FLT_EPSILON));
	break;
      }
      case EVLASwPow::RQ: {
	good=allGT(currrq,FLT_EPSILON);
	break;
      }
      default: {
	throw(AipsError("Unrecognized EVLA Switched Power type"));
	break;
      }
      }
      blc(2)=trc(2)=thisant; // the MS ant index (not loop index)

      blc(0)=0;
      gain.reference(solveAllRPar()(blc,trc,stp).nonDegenerate(1)); // 'gain'
      blc(0)=1;
      tsys.reference(solveAllRPar()(blc,trc,stp).nonDegenerate(1)); // 'tsys'

      if (!good) {

	// ensure transparent values
        gain=1.0;  
	tsys=1.0;
	solveAllParOK().xyPlane(thisant)=false;
	
	// Increment bad counter
	++badcount(ispw,thisant);
      }
      else {
	
	switch (swptype) {
	case EVLASwPow::SWPOW: {
	  gain=sqrt(currpdif/currtcal);
	  gain*=dig; // scale by net digital factor
	  tsys=(currtcal*currpsum/currpdif/2.0);  // 'tsys'
	  break;
	}
	case EVLASwPow::RQ: {
	  gain=currrq;    // RQ gain only!
	  tsys=1.0;       // ignore Tsys
	  break;
	}
	case EVLASwPow::SWPOVERRQ: {
	  gain=sqrt(currpdif/currtcal);     // ordinary sw power gain
	  gain/=currrq;                     // remove rq effect
	  gain*=dig;                        // scale by net digital factor
	  tsys=(currtcal*currpsum/currpdif/2.0);  // 'tsys'
	  break;
	}
	default: {
	  throw(AipsError("Unrecognized EVLA Switched Power type"));
	  break;
	}
	}
	solveAllParOK().xyPlane(thisant)=true;
      
	// Increment good counter
	++goodcount(ispw,thisant);

      }

    }
    
    // Record in the memory caltable
    keepNCT();

    sysPowIter.next();
    ++iter;

  }

  // Set scan and fieldid info
  //  assignCTScanField(*ct_,msName());

  logSink() << "GOOD gain counts per spw for antenna Ids 0-"<<nElem()-1<<":" << LogIO::POST;
  for (Int ispw=0;ispw<nSpw();++ispw) {
    Vector<Int> goodcountspw(goodcount.row(ispw));
    if (sum(goodcountspw)>0)
      logSink() << "  Spw " << ispw << ": " << goodcountspw 
		<< " (" << sum(goodcountspw) << ")" 
		<< LogIO::POST;
    else
      logSink() << "Spw " << ispw << ": NONE." << LogIO::POST;
  }

  logSink() << "BAD gain counts per spw for antenna Ids 0-"<<nElem()-1<<":" << LogIO::POST;
  for (Int ispw=0;ispw<nSpw();++ispw) {
    Vector<Int> badcountspw(badcount.row(ispw));
    if (sum(badcountspw)>0)
      logSink() << "  Spw " << ispw << ": " << badcountspw 
		<< " (" << sum(badcountspw) << ")" 
		<< LogIO::POST;
  }

  logSink() << "BAD gain count FRACTION per spw for antenna Ids 0-"<<nElem()-1<<":" << LogIO::POST;
  for (Int ispw=0;ispw<nSpw();++ispw) {
    Vector<Float> badcountspw(badcount.row(ispw).shape());
    Vector<Float> goodcountspw(goodcount.row(ispw).shape());
    convertArray(badcountspw,badcount.row(ispw));
    convertArray(goodcountspw,goodcount.row(ispw));
    if (sum(badcountspw)>0.0f) {
      Vector<Float> fracbad=badcountspw/(badcountspw+goodcountspw);
      fracbad=floor(1000.0f*fracbad)/1000.0f;
      Float fracbadsum=sum(badcountspw)/(sum(badcountspw)+sum(goodcountspw));
      fracbadsum=floor(1000.0f*fracbadsum)/1000.0f;
      logSink() << "  Spw " << ispw << ": " << fracbad
		<< " (" << fracbadsum << ")" 
		<< LogIO::POST;
    }
  }


}

void EVLASwPow::fillTcals() {

  logSink() << "Filling Tcals from the CALDEVICE table." << LogIO::POST;

  Block<String> columns(2);
  columns[0] = "SPECTRAL_WINDOW_ID";
  columns[1] = "ANTENNA_ID";
  Table calDevTab(calDevTabName_,Table::Old);
  TableIterator calDevIter(calDevTab,columns);

  tcals_.resize(2,nElem(),nSpw());
  tcals_.set(-1.0f);

  // Iterate over CALDEVICE table
  Int iter(0);
  Vector<Int> islot(nSpw(),0);

  Bool ignoreSolar(false);
  while (!calDevIter.pastEnd()) {

    Table itab(calDevIter.table());

    ScalarColumn<Int> spwCol(itab,"SPECTRAL_WINDOW_ID");
    ScalarColumn<Double> timeCol(itab,"TIME");
    ScalarColumn<Double> intervalCol(itab,"INTERVAL");
    ScalarColumn<Int> antCol(itab,"ANTENNA_ID");
    ScalarColumn<Int> numLoadCol(itab,"NUM_CAL_LOAD");

    ArrayColumn<Float> noiseCalCol(itab,"NOISE_CAL");

    Int ispw=spwCol(0);
    Int iant=antCol(0);
    Int nTcal=noiseCalCol(0).nelements();

    if (nTcal==1) {
      // One value (not clear this was ever the case)
      Vector<Float> thisTcal=noiseCalCol(0);
      AlwaysAssert(thisTcal.nelements()==1,AipsError);
      tcals_.xyPlane(ispw).column(iant)=thisTcal(0);
    }
    else if (nTcal==2) {
      // Pre-solar Tcal support: two values
      Vector<Float> thisTcal=noiseCalCol(0);
      AlwaysAssert(thisTcal.nelements()==2,AipsError);
      tcals_.xyPlane(ispw).column(iant)=thisTcal;
    }
    else if (nTcal==4) {
      ignoreSolar=true;  // we will ignore solar Tcals
      Matrix<Float> thisTcalMat=noiseCalCol(0);
      AlwaysAssert(thisTcalMat.shape()==IPosition(2,2,2),AipsError);
      Int tcalset(0);  // first pair, for now, which is ordinary non-solar Tcals
      tcals_.xyPlane(ispw).column(iant)=thisTcalMat(Slice(tcalset,1,1),Slice());
    }

    // Increment the iterator
    ++iter;
    calDevIter.next();
  }

  // Report if we ignored solar Tcals
  if (ignoreSolar)
    logSink() << "Ignoring the SOLAR Tcals, which seem to be present in CALDEVICE." << LogIO::POST;


}

void EVLASwPow::calcAllJones() {

  // 0th and 2nd pars are the gains
  //  currJElem()=currRPar()(Slice(0,2,2),Slice(),Slice()); // NEWCALTABLE
  convertArray(currJElem(),currRPar()(Slice(0,2,2),Slice(),Slice()));
  currJElemOK()=currParOK()(Slice(0,2,2),Slice(),Slice());

}

void EVLASwPow::syncWtScale() {

  Int nPolWt=currRPar().shape()(0)/2;
  currWtScale().resize(nPolWt,1,nAnt());

  Cube<Float> Tsys(currRPar()(Slice(1,2,2),Slice(),Slice()));
  Tsys(Tsys<FLT_MIN)=1.0;  // OK?

  currWtScale() = 1.0f/Tsys;
  currWtScale()*=correff_; // reduce by correlator efficiency (per ant)

  //  cout << "Tsys = " << Tsys << endl;
  //  cout << "currWtScale() = " << currWtScale() << endl;

}

/*
void EVLASwPow::updateWt(Vector<Float>& wt,const Int& a1,const Int& a2) {

  Vector<Float> ws1(currWtScale().column(a1));
  Vector<Float> ws2(currWtScale().column(a2));

  if (a1==0 && a2==1) {
    cout << "wt = " << wt << endl;
    cout << "ws1 = " << ws1 << endl;
    cout << "ws2 = " << ws2 << endl;
  }

  VisJones::updateWt(wt,a1,a2);

  if (a1==0 && a2==1) {
    cout << "wt = " << wt << endl;
  }
}
*/

} //# NAMESPACE CASA - END
