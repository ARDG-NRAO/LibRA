//# SolvableVisCal.cc: Implementation of SolvableVisCal classes
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
//# $Id: VisCal.cc,v 1.15 2006/02/06 19:23:11 gmoellen Exp $

#include <synthesis/MeasurementComponents/CalCorruptor.h>
#include <synthesis/MeasurementComponents/SolvableVisCal.h>
#include <synthesis/MeasurementComponents/MSMetaInfoForCal.h>

#include <msvis/MSVis/VisBuffer.h>

#include <casacore/casa/Arrays/ArrayMath.h>
#include <casacore/casa/Arrays/MaskArrMath.h>
#include <casacore/casa/Arrays/ArrayIter.h>
#include <casacore/scimath/Mathematics/MatrixMathLA.h>
#include <casacore/scimath/Fitting/LinearFit.h>
#include <casacore/scimath/Functionals/Polynomial.h>
#include <casacore/casa/BasicSL/String.h>
#include <casacore/casa/Utilities/Assert.h>
#include <casacore/casa/Quanta/MVTime.h>
#include <casacore/casa/Exceptions/Error.h>
#include <casacore/casa/OS/Memory.h>
#include <casacore/casa/OS/File.h>
#include <casacore/casa/Utilities/GenSort.h>
#include <casacore/casa/Quanta/Quantum.h>
#include <casacore/casa/Quanta/QuantumHolder.h>
#include <casacore/tables/Tables/TableCopy.h>
#include <casacore/tables/Tables/TableUtil.h>
#include <casacore/ms/MeasurementSets/MSAntennaColumns.h>
#include <casacore/ms/MeasurementSets/MSSpWindowColumns.h>
#include <casacore/ms/MeasurementSets/MSFieldColumns.h>
#include <casacore/ms/MSOper/MSMetaData.h>
#include <synthesis/CalTables/CTMainColumns.h>
#include <synthesis/CalTables/CTColumns.h>
#include <synthesis/CalTables/CTGlobals.h>
#include <synthesis/CalTables/CTIter.h>
#include <synthesis/CalTables/CTInterface.h>
#include <synthesis/MeasurementComponents/SolveDataBuffer.h>
#include <casacore/ms/MSSel/MSSelection.h>
#include <casacore/ms/MSSel/MSSelectionTools.h>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <casacore/casa/Containers/RecordField.h>

#include <casacore/casa/Logging/LogMessage.h>
#include <casacore/casa/Logging/LogSink.h>
#include <casacore/casa/System/Aipsrc.h>
#include <casacore/casa/System/ProgressMeter.h>

#include <fstream>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN


SolNorm::SolNorm(Bool donorm, String type) :
  donorm_(donorm),
  normtype_(normTypeFromString(type))
{
  //  report();
}

SolNorm::SolNorm(const SolNorm& other) : 
  donorm_(other.donorm_),
  normtype_(other.normtype_)
{}

void SolNorm::report() {
  cout << "Forming SolNorm object:" << endl;
  cout << " donorm=" << boolalpha << donorm_ << endl
       << " normtype=" << normtypeString() << endl;
}
 
String SolNorm::normTypeAsString(Type type) {
  switch (type) {
  case MEAN: {
    return String("MEAN");
  }
  case MEDIAN: { 
    return String("MEDIAN");
  }
  default: {
    return String("UNKNOWN");
  }
  }
  return String("UNKNOWN");
}
  
SolNorm::Type SolNorm::normTypeFromString(String type) {

  String uptype=upcase(type);
  if (uptype.contains("MEAN")) return SolNorm::MEAN;
  else if (uptype.contains("MED")) return SolNorm::MEDIAN;
  else {
    throw(AipsError("Invalid normalization type specifiec!"));
  }						
  // Shouldn't reach here  return SolNorm::UNKNOWN;

}





// **********************************************************
//  FreqMetaData Implementations
//

FreqMetaData::FreqMetaData() : 
  ok_(False),   // ok requires running calcFreqMeta later
  validspws_(),
  freq_(Vector< Vector<Double> >()),
  width_(Vector< Vector<Double> >()),
  effBW_(Vector< Vector<Double> >()),
  spwfanin_()
{}


void FreqMetaData::calcFreqMeta(const Vector< Vector<Double> >& msfreq,
				const Vector< Vector<Double> >& mswidth,
				const Vector<uInt>& selspw,
				Bool freqdep,Bool combspw,
				const Vector<Int>& spwfanin) {

  // Max number of spws
  uInt nspw(msfreq.nelements());

  // We will keep track of which spws get set
  Vector<Bool> validspw(nspw,false);

  // Size up the Vector of spw freq Vectors
  freq_.resize(nspw);
  width_.resize(nspw);
  effBW_.resize(nspw);

  // We will log some pertinent info
  LogIO log;
  log << LogOrigin("FreqMetaData","calcFreqMeta") << LogIO::NORMAL << "Derived frequency meta-info for solutions:" << LogIO::POST;

  // Gather MS freq info
  for (uInt i=0;i<selspw.nelements();++i) {
    uInt ispw=selspw(i);
    uInt nchan=msfreq(ispw).nelements();

    // This ensures msfreq/mswidth consistent with selspw
    //   (each selspw has at least one channel of freq info)
    if (nchan<1)
      throw(AipsError("No frequency information for a selected spw!"));


    // Set nominal per-spw solution frequencies
    if (!freqdep && nchan>1) {

      //      cout << "Not freqdep and multi-chan data..." 
      //	   << "Soln freqs will be collapsed to single channel."  << endl;


      //  solutions NOT freq dep but  more than one MS channel to solve from
      //    --> Average chan freqs to single fiducial value
      //   (this is a parameterized solution, like delay or fringefit...)


      // Manage value scale, to avoid loss of precision
      Double freq0=msfreq(ispw)(0);  // Lop off large freq value
      Double width1=mswidth(ispw)(0);  // Divide widths by typical value (also ensures abs)
      Vector<Double> f=msfreq(ispw)-freq0;               // relative freq
      Vector<Double> w=mswidth(ispw)/width1;  // width-weights

      // output info will be single-channel
      freq_(ispw).resize(1);
      width_(ispw).resize(1);

      // calculate
      freq_(ispw).set(sum( (Array<Double>) f*w));   // weighed rel freq sum 
      width_(ispw).set(sum(w));     // sum of norm'd width weights
      freq_(ispw)/=width_(ispw);    // divide by weight sum

      // Restore offsets to final values
      freq_(ispw)+=freq0;         // add offset back in
      width_(ispw)*=width1;       // mult by width norm

    } else {
      // Solutions ARE freq-dep, or only one channel, 
      //     so just adopt MS frequencies (NB: possibly already decimated by VI2)
      //  TBD: could use reference here (only if ~combspw)?
      //cout << "Soln is freqdep, or only one channel..." 
      //<< "Soln freqs are just the VI2 data freqs" << endl;
      freq_(ispw).assign(msfreq(ispw));
      width_(ispw).assign(mswidth(ispw));
    }

    { 
      // Report spw freq-metadetails....
      ostringstream os;
      os.precision(15);
      os << " Selected spw=" << ispw << " (nchan=" << freq_(ispw).nelements() << ") has centroid freq = " << mean(freq_(ispw));
      log << LogIO::NORMAL << os  << LogIO::POST;
      //log << LogOrigin("FreqMetaData","calcFreqMeta") << LogIO::NORMAL << os  << LogIO::POST;
    }


    // Assume effective BW is just the width
    effBW_(ispw).reference(width_(ispw));

    // Remember we set this spw
    validspw(ispw)=true;

  }

  // Collapse over spws if combining them...
  if (combspw && spwfanin.nelements()>1) {

    //    cout << "Combining spws!" << endl;

    // Remember the fan-in vector
    spwfanin_.resize();
    spwfanin_.assign(spwfanin);


    // Keep track of which spws are aggregate spws
    //  (none are yet)
    Vector<Bool> isAggspw(nspw,false);

    // Keep track of fanned-in spws per agg spw
    Vector< Vector<Int> > fannedin(nspw,Vector<Int>(0));


    Vector<Double> freq0(nspw,0);
    Vector<Double> width1(nspw,0);

    for (uInt ispw=0;ispw<spwfanin.nelements();++ispw) {

      // Step over unmapped spws...
      if (spwfanin(ispw)<0)
	continue; 

      //  The following assumes fan-in is always to lowest spw id in group 
      if (Int(ispw)==spwfanin(ispw)) {	

	//cout << "Aggregate spw = " << ispw << endl;
	
	isAggspw(ispw)=true;

	// Just to be sure...
	validspw(ispw)=true;

	// Add this (agg) spw to fanned in list
	uInt nfanin=fannedin(ispw).nelements();
	fannedin(ispw).resize(nfanin+1,true);  // copies elements
	fannedin(ispw)(nfanin)=ispw;

	// This is a spw in which we'll accumulate
	
	// Offsets for precision 
	freq0(ispw)=freq_(ispw)(0);
	width1(ispw)=width_(ispw)(0);

	freq_(ispw)-=freq0(ispw);
	width_(ispw)/=width1(ispw);

	// begin to accumulate effBW
	effBW_(ispw).resize();    // break prior reference to width_
	effBW_(ispw).assign(width_(ispw));   // copy in this spw's width_

	// weight the freqs with widths
	freq_(ispw)*=width_(ispw);

	// weight the widths with the widths
	//  (we will end up with a width-weighted width over spws)
	width_(ispw)*=width_(ispw);

      }
      else {
	uInt aggspw(spwfanin(ispw));

	uInt nfanin=fannedin(aggspw).nelements();
	fannedin(aggspw).resize(nfanin+1,true);  // copies elements
	fannedin(aggspw)(nfanin)=ispw;

	if (ispw<=aggspw) 
	  throw(AipsError("Cannot accumulate into spw with a lower spw id."));


	//	cout << "Accumulating spw=" << ispw << " into aggspw="<< aggspw << endl;

	// Trap incompatible spws
	if (freq_(ispw).nelements()!=freq_(aggspw).nelements())
	  throw(AipsError("Cannont combine spws with differing nchan!"));

	//  TBD: recognize/handle sideband and width variation if nchan>1 ?
	//  (at the moment, there is no SB info in the sign of the width)

	// Offsets for precision (from aggspw!)
	Vector<Double> f(freq_(ispw)-freq0(aggspw));
	Vector<Double> w(width_(ispw)/width1(aggspw));

	// accumulate
	freq_(aggspw)+= ( (Array<Double>) f*w);
	width_(aggspw)+= ( (Array<Double>) w*w);
	effBW_(aggspw)+= w;
	
	// This acculated spw now refers to aggspw:
	freq_(ispw).reference(freq_(aggspw));
	width_(ispw).reference(width_(aggspw));
	effBW_(ispw).reference(effBW_(aggspw));

	// This spw no longer autonomously "valid"
	//   (aggspw is the relevant valid one)
	validspw(ispw)=false;
	
      }
    }

    // Finish weighted mean calculation
    for (uInt ispw=0;ispw<spwfanin.nelements();++ispw) {
      
      // Step over non-agg spws
      if (!isAggspw(ispw))
	continue;

      //      cout << "Finalizing freq averages for agg spw = " << ispw << endl;

      if (freq0(ispw)>0.0 && effBW_(ispw).nelements()>0 && allGT(effBW_(ispw),0.0)) {
	freq_(ispw)/=effBW_(ispw);
	width_(ispw)/=effBW_(ispw);

	// put offsets back in
	freq_(ispw)+=freq0(ispw);
	width_(ispw)*=width1(ispw);
	effBW_(ispw)*=width1(ispw);

	{ 
	  // Report fanin details....
	  ostringstream os;
	  os.precision(15);
	  os << " Combining spws=" << fannedin(ispw) << " into (aggregate) spw=" << ispw << " (nchan=" << freq_(ispw).nelements() << ") at centroid freq = " << mean(freq_(ispw));
	  log << LogIO::NORMAL << os << LogIO::POST;
	  //log << LogOrigin("FreqMetaData","calcFreqMeta") << LogIO::NORMAL << os << LogIO::POST;
	}

      }
      else {
	throw(AipsError("Problem completing combspw freq average."));
      }
    } // ispw (fanin spws only)
    //    cout << "Finished combining spws." << endl;

  } // combspw? 
  else {

    // For safety, make spwfanin the identity vector
    spwfanin_.resize(nspw);
    indgen(spwfanin_);

  }

  // Fill validspws_ vector    
  Vector<Int> spwlist(nspw);
  indgen(spwlist);
  validspws_.assign(spwlist(validspw).getCompressedArray());

  // If we get here, we should be ok!
  ok_=true;

}


Bool FreqMetaData::ok() const {
  if (ok_)
    return true;
  else
    throw(AipsError("FreqMetaData not initialized!"));
  return false;

}

const Vector<Int>& FreqMetaData::validSpws() const {

  if (ok() && validspws_.nelements()>0)
    return validspws_;
  else
    throw(AipsError("No valid spws in FreqMetaData!"));

}


const Vector<Double>& FreqMetaData::freq(Int spw) const {
  if (ok() && freq_(spw).nelements()>0) 
    return freq_(spw);
  else
    throw(AipsError("Bad spw in FreqMetaData!"));

}
const Vector<Double>& FreqMetaData::width(Int spw) const {
  if (ok() && width_(spw).nelements()>0) 
    return width_(spw);
  else
    throw(AipsError("Bad spw in FreqMetaData!"));

}
const Vector<Double>& FreqMetaData::effBW(Int spw) const {
  if (ok() && effBW_(spw).nelements()>0) 
    return effBW_(spw);
  else
    throw(AipsError("Bad spw in FreqMetaData!"));

}

Int FreqMetaData::fannedInSpw(Int spw) const {

  if (ok() && Int(spwfanin_.nelements())>spw && spwfanin_(spw)>-1)
    return spwfanin_(spw);
  else
    throw(AipsError("Bad spw fan-in in FreqMetaData::faninoutspw."));

}


// **********************************************************
//  SolvableVisCal Implementations
//

SolvableVisCal::SolvableVisCal(VisSet& vs) :
  VisCal(vs),
  corruptor_p(NULL),
  ct_(NULL),
  ci_(NULL),
  cpp_(NULL),
  spwOK_(vs.numberSpw(),false),
  maxTimePerSolution_p(0), 
  minTimePerSolution_p(10000000), 
  avgTimePerSolution_p(0),
  timer_p(),
  freqMetaData_(),
  calTableName_(""),
  calTableSelect_(""),
  append_(false),
  tInterpType_(""),
  fInterpType_(""),
  spwMap_(1,-1),
  refantmode_("flex"),
  urefantlist_(1,-1),
  minblperant_(4),
  solved_(false),
  byCallib_(false),
  apmode_(""),
  solmode_(""),
  rmsthresh_(0),
  usolint_("inf"),
  solint_("inf"),
  solTimeInterval_(DBL_MAX),
  fsolint_("none"),
  fintervalHz_(-1.0),
  fintervalCh_(vs.numberSpw(),0.0),
  chanAveBounds_(vs.numberSpw(),Matrix<Int>()),
  solnorm_(false,"mean"),
  minSNR_(0.0f),
  combine_(""),
  focusChan_(0),
  dataInterval_(0.0),
  fitWt_(0.0),
  fit_(0.0),
  solveCPar_(vs.numberSpw(),NULL),  // TBD: move inflation into ctor body
  solveRPar_(vs.numberSpw(),NULL),
  solveParOK_(vs.numberSpw(),NULL),
  solveParErr_(vs.numberSpw(),NULL),
  solveParSNR_(vs.numberSpw(),NULL),
  solveAllCPar_(vs.numberSpw(),NULL),
  solveAllRPar_(vs.numberSpw(),NULL),
  solveAllParOK_(vs.numberSpw(),NULL),
  solveAllParErr_(vs.numberSpw(),NULL),
  solveAllParSNR_(vs.numberSpw(),NULL),
  srcPolPar_(),
  chanmask_(NULL),
  simulated_(false),
  simint_("integration"),
  onthefly_(false)
{
  if (prtlev()>2) cout << "SVC::SVC(vs)" << endl;

  caiRC_p = Aipsrc::registerRC("calibrater.activity.interval", "3600.0");
  cafRC_p = Aipsrc::registerRC("calibrater.activity.fraction", "0.00001");
  String ca_str = Aipsrc::get(caiRC_p);
  std::sscanf(std::string(ca_str).c_str(), "%f", &userPrintActivityInterval_p);
  userPrintActivityInterval_p = abs(userPrintActivityInterval_p);

  ca_str = Aipsrc::get(cafRC_p);
  std::sscanf(std::string(ca_str).c_str(), "%f", &userPrintActivityFraction_p);
  userPrintActivityFraction_p = abs(userPrintActivityFraction_p);

  initSVC();

};

SolvableVisCal::SolvableVisCal(String msname,Int MSnAnt,Int MSnSpw) :
  VisCal(msname,MSnAnt,MSnSpw),
  corruptor_p(NULL),
  ct_(NULL),
  ci_(NULL),
  cpp_(NULL),
  spwOK_(MSnSpw,false),
  maxTimePerSolution_p(0), 
  minTimePerSolution_p(10000000), 
  avgTimePerSolution_p(0),
  timer_p(),
  calTableName_(""),
  calTableSelect_(""),
  append_(false),
  tInterpType_(""),
  fInterpType_(""),
  spwMap_(1,-1),
  refantmode_("flex"),
  urefantlist_(1,-1),
  minblperant_(4),
  solved_(false),
  byCallib_(false),
  apmode_(""),
  solmode_(""),
  rmsthresh_(0),
  usolint_("inf"),
  solint_("inf"),
  solTimeInterval_(DBL_MAX),
  fsolint_("none"),
  fintervalHz_(-1.0),
  fintervalCh_(MSnSpw,0.0),
  chanAveBounds_(MSnSpw,Matrix<Int>()),
  solnorm_(false,"mean"),
  minSNR_(0.0f),
  combine_(""),
  focusChan_(0),
  dataInterval_(0.0),
  fitWt_(0.0),
  fit_(0.0),
  solveCPar_(MSnSpw,NULL),  // TBD: move inflation into ctor body
  solveRPar_(MSnSpw,NULL),
  solveParOK_(MSnSpw,NULL),
  solveParErr_(MSnSpw,NULL),
  solveParSNR_(MSnSpw,NULL),
  solveAllCPar_(MSnSpw,NULL),
  solveAllRPar_(MSnSpw,NULL),
  solveAllParOK_(MSnSpw,NULL),
  solveAllParErr_(MSnSpw,NULL),
  solveAllParSNR_(MSnSpw,NULL),
  srcPolPar_(),
  chanmask_(NULL),
  simulated_(false),
  simint_("integration"),
  onthefly_(false)
{
  if (prtlev()>2) cout << "SVC::SVC(msname,MSnAnt,MSnSpw)" << endl;

  caiRC_p = Aipsrc::registerRC("calibrater.activity.interval", "3600.0");
  cafRC_p = Aipsrc::registerRC("calibrater.activity.fraction", "0.00001");
  String ca_str = Aipsrc::get(caiRC_p);
  std::sscanf(std::string(ca_str).c_str(), "%f", &userPrintActivityInterval_p);
  userPrintActivityInterval_p = abs(userPrintActivityInterval_p);

  ca_str = Aipsrc::get(cafRC_p);
  std::sscanf(std::string(ca_str).c_str(), "%f", &userPrintActivityFraction_p);
  userPrintActivityFraction_p = abs(userPrintActivityFraction_p);

  initSVC();

};

  

SolvableVisCal::SolvableVisCal(const MSMetaInfoForCal& msmc) :
  VisCal(msmc),
  corruptor_p(NULL),
  ct_(NULL),
  ci_(NULL),
  cpp_(NULL),
  spwOK_(nSpw(),False),
  maxTimePerSolution_p(0), 
  minTimePerSolution_p(10000000), 
  avgTimePerSolution_p(0),
  timer_p(),
  calTableName_(""),
  calTableSelect_(""),
  append_(False),
  tInterpType_(""),
  fInterpType_(""),
  spwMap_(1,-1),
  refantmode_("flex"),
  urefantlist_(1,-1),
  minblperant_(4),
  solved_(False),
  byCallib_(False),
  apmode_(""),
  solmode_(""),
  rmsthresh_(0),
  usolint_("inf"),
  solint_("inf"),
  solTimeInterval_(DBL_MAX),
  fsolint_("none"),
  fintervalHz_(-1.0),
  fintervalCh_(nSpw(),0.0),
  chanAveBounds_(nSpw(),Matrix<Int>()),
  solnorm_(false,"mean"),
  minSNR_(0.0f),
  combine_(""),
  focusChan_(0),
  dataInterval_(0.0),
  fitWt_(0.0),
  fit_(0.0),
  solveCPar_(nSpw(),NULL),  // TBD: move inflation into ctor body
  solveRPar_(nSpw(),NULL),
  solveParOK_(nSpw(),NULL),
  solveParErr_(nSpw(),NULL),
  solveParSNR_(nSpw(),NULL),
  solveAllCPar_(nSpw(),NULL),
  solveAllRPar_(nSpw(),NULL),
  solveAllParOK_(nSpw(),NULL),
  solveAllParErr_(nSpw(),NULL),
  solveAllParSNR_(nSpw(),NULL),
  srcPolPar_(),
  chanmask_(NULL),
  simulated_(False),
  simint_("integration"),
  onthefly_(False)
{
  if (prtlev()>2) cout << "SVC::SVC(msmc)" << endl;

  caiRC_p = Aipsrc::registerRC("calibrater.activity.interval", "3600.0");
  cafRC_p = Aipsrc::registerRC("calibrater.activity.fraction", "0.00001");
  String ca_str = Aipsrc::get(caiRC_p);
  std::sscanf(std::string(ca_str).c_str(), "%f", &userPrintActivityInterval_p);
  userPrintActivityInterval_p = abs(userPrintActivityInterval_p);

  ca_str = Aipsrc::get(cafRC_p);
  std::sscanf(std::string(ca_str).c_str(), "%f", &userPrintActivityFraction_p);
  userPrintActivityFraction_p = abs(userPrintActivityFraction_p);

  initSVC();

};



SolvableVisCal::SolvableVisCal(const Int& nAnt) :
  VisCal(nAnt),
  corruptor_p(NULL),
  maxTimePerSolution_p(0), 
  minTimePerSolution_p(10000000), 
  avgTimePerSolution_p(0),
  timer_p(),
  calTableName_(""),
  calTableSelect_(""),
  append_(false),
  tInterpType_(""),
  fInterpType_(""),
  spwMap_(1,-1),
  refantmode_("flex"),
  urefantlist_(1,-1),
  minblperant_(4),
  solved_(false),
  apmode_(""),
  solmode_(""),
  rmsthresh_(0),
  usolint_("inf"),
  solint_("inf"),
  solTimeInterval_(DBL_MAX),
  fsolint_("none"),
  solnorm_(false,"mean"),
  minSNR_(0.0),
  combine_(""),
  focusChan_(0),
  dataInterval_(0.0),
  fitWt_(0.0),
  fit_(0.0),
  solveCPar_(1,NULL),
  solveRPar_(1,NULL),
  solveParOK_(1,NULL),
  solveParErr_(1,NULL),
  solveParSNR_(1,NULL),
  srcPolPar_(),
  chanmask_(NULL),
  simulated_(false),
  simint_("inf"),
  onthefly_(false)
{  
  if (prtlev()>2) cout << "SVC::SVC(i,j,k)" << endl;

  caiRC_p = Aipsrc::registerRC("calibrater.activity.interval", "3600.0");
  cafRC_p = Aipsrc::registerRC("calibrater.activity.fraction", "0.00001");
  String ca_str = Aipsrc::get(caiRC_p);
  std::sscanf(std::string(ca_str).c_str(), "%f", &userPrintActivityInterval_p);
  userPrintActivityInterval_p = abs(userPrintActivityInterval_p);

  ca_str = Aipsrc::get(cafRC_p);
  std::sscanf(std::string(ca_str).c_str(), "%f", &userPrintActivityFraction_p);
  userPrintActivityFraction_p = abs(userPrintActivityFraction_p);

  initSVC();

}

SolvableVisCal::~SolvableVisCal() {

  if (prtlev()>2) cout << "SVC::~SVC()" << endl;
  
  // TODO RI do we ever have the same corruptor working on multiple VCs?  then we need some kind of 
  // multiuse locking...
  if (corruptor_p) delete corruptor_p;  

  deleteSVC();

  if (ci_)   delete ci_;   ci_=NULL;
  if (cpp_)  delete cpp_;  cpp_=NULL;
  if (ct_)   delete ct_;   ct_=NULL;

}


// Generic setapply
void SolvableVisCal::setApply() {

  if (prtlev()>2) cout << "SVC::setApply()" << endl;

  // Generic settings
  calTableName()="<none>";
  calTableSelect()="<none>";
  tInterpType()="nearest";
  indgen(spwMap());
  interval()=DBL_MAX;

  // This is apply context  
  setApplied(true);
  setSolved(false);

}



// Setapply from a Cal Table, etc.
void SolvableVisCal::setApply(const Record& apply) {
  //  Inputs:
  //    apply           Record&       Contains application params
  //    

  if (prtlev()>2) 
    cout << "SVC::setApply(apply)" << endl;

  // Call VisCal version for generic stuff
  VisCal::setApply(apply);

  // Collect Cal table parameters
  if (apply.isDefined("table")) {
    calTableName()=apply.asString("table");
    // Verify that CalTable is of correct type
    verifyCalTable(calTableName());
  }

  // Collect interpolation parameters
  if (apply.isDefined("interp")) {
    String interp=apply.asString("interp");
    if (interp.contains(',')) {
      tInterpType()=String(interp.before(','));
      fInterpType() = interp.after(',');
    }
    else
      tInterpType()=interp;
  }

  // Protect against non-specific interp
  if (tInterpType()=="")
    tInterpType()="linear";
  if (fInterpType()=="" && this->freqDepPar()) // only if we are freq-dep
    fInterpType()="linear";

  // Catch use of deprecated 'aipslin' interpolation
  if (tInterpType().contains("aips") || fInterpType().contains("aips") )
    throw(AipsError("The (peculiar) 'aipslin' interpolation type was deprecated in CASA v3.4; use 'linear'."));


  //  cout << "SVC::setApply(apply) is not yet supporting CalSelection." << endl;

  /*
  if (apply.isDefined("select"))
    calTableSelect()=apply.asString("select");

  else {
    
    calTableSelect()="";
    //    String spwsel("");
    //    if (apply.isDefined("spw")) {
    //      ostringstream os;
    //      os << Vector<Int>(apply.asArrayInt("spw"));
    //      spwsel = os.str();
    //    }
    //    cout << "spwsel = " << spwsel << endl;

    String fldsel("");
    if (apply.isDefined("field")) {
      ostringstream os;
      os << Vector<Int>(apply.asArrayInt("field"));
      if (os.str()!="[]")
	fldsel = os.str();
    }

    if (fldsel.length()>0) {
      ostringstream os;
      os << "(FIELD_ID IN " << fldsel << ")";
      calTableSelect() = os.str();
    }
  }
  */

  String fieldstr;
  String fieldtype("");
  if (apply.isDefined("fieldstr")) {
    fieldstr=apply.asString("fieldstr");
    //    cout << "SVC::setApply: fieldstr=" << fieldstr  << endl;
    if (fieldstr=="nearest") {
      fieldtype="nearest";
      fieldstr="";
    }
  }
  //cout << "SVC::setApply: fieldstr=" << fieldstr  << endl;
  //cout << "SVC::setApply: fieldtype=" << fieldtype  << endl;

  if (apply.isDefined("spwmap")) 
    spwMap().assign(apply.asArrayInt("spwmap"));

  // Catch spwmap that is too long
  if (spwMap().nelements()>uInt(nSpw()))
    throw(AipsError("Specified spwmap has more elements ("+String::toString(spwMap().nelements())+") than the number of spectral windows in the MS ("+String::toString(nSpw())+")."));

  // TBD: move interval to VisCal version?
  // TBD: change to "reach"
  if (apply.isDefined("t"))
    interval()=apply.asFloat("t");

  // This is apply context  
  setApplied(true);
  setSolved(false);

  //  TBD:  "Arranging to apply...."

  // TBD:  Move the following so as to be triggered
  //       when actual application starts?  E.g., SVC::initApply()...

  // Open the caltable
  loadMemCalTable(calTableName(),fieldstr);

  // Make the interpolation engine
  MeasurementSet ms(msName());
  ci_ = new CTPatchedInterp(*ct_,matrixType(),nPar(),tInterpType(),fInterpType(),fieldtype,ms,msmc(),spwMap(),cttifactoryptr());

  // Channel counting info 
  //  (soon will deprecate, I think, because there will be no need
  //    to do channel registration in the apply)
  startChanList()=0;  // all zero

  //  cout << "End of SVC::setApply" << endl;

}

// Setapply from a Cal Table, etc.
void SolvableVisCal::setCallib(const Record& callib,
			       const MeasurementSet& selms) {

  if (prtlev()>2) 
    cout << "SVC::setCallib(callib)" << endl;


  if (typeName().contains("BPOLY") ||
      typeName().contains("GSPLINE"))
    throw(AipsError(typeName()+" not yet supported for apply by cal library."));

  // Call VisCal version for generic stuff
  VisCal::setCallib(callib,selms);

  // signal that we are using a callib
  byCallib_=true;

  // Collect Cal table parameters
  if (callib.isDefined("tablename")) {
    calTableName()=callib.asString("tablename");
    // Verify that CalTable is of correct type
    verifyCalTable(calTableName());
  }

  // This is apply context  
  setApplied(true);
  setSolved(false);

  logSink() << LogIO::NORMAL << ".   "
	    << this->applyinfo()
	    << LogIO::POST;

  // Make the interpolation engine
  cpp_ = new CLPatchPanel(calTableName(),selms,callib,matrixType(),nPar(),cttifactoryptr());
  //cpp_->listmappings();

  // Setup ct_ in SVC private data, because some derived classes need this
  //  NB:  Not loaded into memory in the callib context (CLPatchPanel has that)
  ct_= new NewCalTable(calTableName(),Table::Old,Table::Plain);

}


// ===================================================

void SolvableVisCal::createCorruptor(const VisIter& vi,const Record& simpar, const Int nSim) {
  LogIO os(LogOrigin("SVC", "createCorruptor", WHERE));

  if (prtlev()>3) cout << "  SVC::createCorruptor:" << endl;

  // this is the generic create and init 
  // ** specialists should call this after createing their corruptor
  // and before initializing

  if (!corruptor_p) {
    corruptor_p = new CalCorruptor(nSim);  
    os << LogIO::WARN << "creating generic CalCorruptor" << LogIO::POST;
  }

  // would be nice to reduce the amount of stuff passed down to the corruptor; 
  // fnChan is used in AtmosCorruptor, currAnt and curr Spw are used generically

  corruptor_p->prtlev()=prtlev();
  corruptor_p->freqDepPar()=freqDepPar();
  corruptor_p->simpar()=simpar;
  // initialize is supposed to be called in a specialization, but 
  // if we end up only using the generic CalCorruptor and this generic 
  // createCorruptor, we still want amplitude to be passed on.
  if (simpar.isDefined("amplitude")) 
    corruptor_p->amp()=simpar.asFloat("amplitude");

  if (simpar.isDefined("mode")) 
    corruptor_p->mode()=simpar.asString("mode");

//  corruptor_p->nCorr()=vi.nCorr();
//  if (prtlev()>3) 
//    os << LogIO::POST << "nCorr= " << vi.nCorr() << LogIO::POST;      
  // what matters is nPar not nCorr - then apply uses coridx
  corruptor_p->nPar()=nPar();

  const MSSpWindowColumns& spwcols = vi.msColumns().spectralWindow();  
  //  if (prtlev()>3) cout << " SpwCols accessed:" << endl;
  //if (prtlev()>3) cout << "   nSpw()= " << nSpw() << " spwcols= " << nSpw() << endl;
  //if (prtlev()>3) cout << "   spwcols.nrow()= " << spwcols.nrow() << endl;  
  AlwaysAssert(uInt(nSpw())==spwcols.nrow(),AipsError);
  // there's a member variable in Simulator nSpw, should we verify that 
  // this is the same? probably.

  // things will break if spw mapping, ie not in same order as in vs
  corruptor_p->nSpw()=nSpw();
  corruptor_p->nAnt()=nAnt();
  corruptor_p->currAnt()=0;
  corruptor_p->currSpw()=0;
  corruptor_p->fRefFreq().resize(nSpw());
  corruptor_p->fnChan().resize(nSpw());
  corruptor_p->fWidth().resize(nSpw());
  corruptor_p->currChans().resize(nSpw());
  
  for (Int irow=0;irow<nSpw();++irow) { 
    corruptor_p->currChans()[irow]=0;
    corruptor_p->fRefFreq()[irow]=spwcols.refFrequency()(irow);
    // don't need to change fnChan to 1 if not freqDepPar()
    // because fnChan is only used in AtmosCorruptor if 
    // freqDepPar() is set i.e. in initAtm().
    corruptor_p->fnChan()[irow]=spwcols.numChan()(irow);    
    corruptor_p->fWidth()[irow]=spwcols.totalBandwidth()(irow); 
    // totalBandwidthQuant ?  in other places its assumed to be in Hz
  }
  // see MSsummary.cc for more info/examples
  if (prtlev()>3) 
    cout << "   SVC::fnChan = "<<corruptor_p->fnChan()<<" reffreq = "<<corruptor_p->fRefFreq()<<" fWidth = "<<corruptor_p->fWidth()<<endl;

  if (prtlev()>3) cout << "  ~SVC::createCorruptor:" << endl;

}



void SolvableVisCal::setSimulate(VisSet& vs, Record& simpar, Vector<Double>& solTimes) {

  LogIO os(LogOrigin("SVC["+typeName()+"]", "setSimulate()", WHERE));
  if (prtlev()>2) cout << " SVC::setSimulate(simpar)" << endl;

  //  cout << "SVC::setSimulate" << endl;

  // Extract calWt
  if (simpar.isDefined("calwt"))
    calWt()=simpar.asBool("calwt");
  
  // (copied from SolvableVisCal::setSolve)
  if (simpar.isDefined("simint"))
    simint()=simpar.asString("simint");
  
  if (upcase(simint()).contains("INF") || simint()=="") {
    simint()="infinite";
    interval()=-1.0;
  } else if (upcase(simint()).contains("INT")) {
    simint()="integration";
    interval()=0.0;
  } else {
    QuantumHolder qhsimint;
    String error;
    Quantity qsimint;
    qhsimint.fromString(error,simint());
    if (error.length()!=0)
      throw(AipsError("Unrecognized units for simint."));
    qsimint=qhsimint.asQuantumDouble();
    
    if (qsimint.isConform("s"))
      interval()=qsimint.get("s").getValue();
    else {
      if (qsimint.getUnit().length()==0) {
	// when no units specified, assume seconds
	// assume seconds
	interval()=qsimint.getValue();
	simint()=simint()+"s";
      }
      else
	// unrecognized units:
	throw(AipsError("Unrecognized units for simint (e.g., use 'min', not 'm', for minutes)"));
    }
  }

  // check if want to write a table:
  if (simpar.isDefined("caltable")) {
    calTableName()=simpar.asString("caltable");
    // RI todo SVC::setSimulate deal with over-writing existing caltables
    // verifyCalTable(calTableName());
    append()=false;
  } else {
    calTableName()="<none>";
  }
  if (calTableName().length()==0)
    calTableName()="<none>";
   
  // on the fly (only implemented for ANoise 20100817)
  simOnTheFly()=false;
  if (simpar.isDefined("onthefly")) {
    if (simpar.asBool("onthefly")) {
      if (type() != VisCal::ANoise) {
	throw(AipsError("Logic Error: onthefly simulation not available for type "+typeName()));
      } else {
	simOnTheFly()=true;
	os << LogIO::DEBUG1 << " using OTF simulation" << LogIO::POST;  
	calTableName()="<none>";
      }
    }
  }

  setSolved(false);
  // this has to be true for some of George's VE stuff 
  // but be careful about VC structure and inflation!
  setApplied(true); 
  setSimulated(true);

  // without this, CI gets created without a sensible time
  // interpolation, and ends up bombing
  tInterpType()="nearest";


  // ----------------
  // assess size of ms:

  // how to combine data in sizeUp e.g. SPW,SCAN
  if (simpar.isDefined("combine"))
    combine()=simpar.asString("combine");
  else
    throw(AipsError("MS combination information not set"));

  // GM: organize calibration correction/corruption according to 
  // multi-spw consistency; e.g. move time ahead of data_desc_id so that 
  // data_desc_id (spw) changes faster than time, even within scans.

  // RI todo double-check logic in case of spwmap and multi-spw
  
  // Arrange for iteration over data
  Block<Int> columns;
  // include scan iteration
  columns.resize(5);
  columns[0]=MS::ARRAY_ID;
  columns[1]=MS::SCAN_NUMBER;
  columns[2]=MS::FIELD_ID;
  // GM's order:
  //columns[3]=MS::DATA_DESC_ID;
  //columns[4]=MS::TIME;
  // RI put spw after time
  columns[4]=MS::DATA_DESC_ID;
  columns[3]=MS::TIME;
  

  // drop chunking time interval down to the simulation interval, else will 
  // chunk by entire scans.
  Double iterInterval(max(interval(),DBL_MIN));
  if (interval() < 0.0) {   // means no interval (infinite solint)
    iterInterval=0.0;
    interval()=DBL_MAX;   
  }
  vs.resetVisIter(columns,iterInterval);

  Vector<Int> nChunkPerSol;
  Int nSim = 1;
  // independent of simpar details

  nSim=sizeUpSim(vs,nChunkPerSol,solTimes);
  if (prtlev()>1 and prtlev()<=4) cout << "  VisCal sized for Simulation with " << nSim << " slots." << endl;
  if (prtlev()>4) cout << "  solTimes = " << solTimes-solTimes[0] << endl;  
  
  if (!(simpar.isDefined("startTime"))) {    
    throw(AipsError("can't add startTime field to Record"));
    // Record seems to have been designed strangely, so this doesn't work:
    //    RecordDesc simParDesc = simpar.description();
    //    simParDesc.addField("startTime",TpDouble);
    //    simpar.restructure(simParDesc);
  }
  simpar.define("startTime",min(solTimes));
  if (!(simpar.isDefined("stopTime"))) {    
    throw(AipsError("can't add stopTime field to Record"));
  }
  simpar.define("stopTime",max(solTimes));
  
  // assume only one ms attached to the VI. need vi for mscolumns in createCorruptor
  // note - sizeUpSim seems to break the reference to mscolumns inside of VI, 
  // so we're better off resetting it here, I think, just to make sure?
  VisIter& vi(vs.iter());
  os << LogIO::DEBUG1 << " number of spw in VI (checking validity of mscolumns) = " 
     << vi.numberSpw() << LogIO::POST;  
  vi.origin();
  os << LogIO::DEBUG1 << " number of spw in VI (after resetting to origin = " 
     << vi.numberSpw() << LogIO::POST;  

  // -------------------
  // create (and initialize) a corruptor in a VC-specific way - 
  // specializations need to call the generic SVC::createCorruptor to get 
  // spw etc information passed down.
  createCorruptor(vi,simpar,nSim);
  
  // set times in the corruptor if createCorruptor didn't:
  if (!corruptor_p->times_initialized()) {
    corruptor_p->curr_slot()=0;
    corruptor_p->slot_times().resize(nSim);
    corruptor_p->slot_times()=solTimes;
    corruptor_p->startTime()=min(solTimes);
    corruptor_p->stopTime()=max(solTimes);
    corruptor_p->times_initialized()=true;
  }

  if (simOnTheFly()) {

    calTableName()="<none>";

  } else {
   
    if (prtlev()>5) 
      cout << "  slot_times= " 
	   << corruptor_p->slot_time(1)-corruptor_p->slot_time(0) << " " 
	   << corruptor_p->slot_time(2)-corruptor_p->slot_time(0) << " " 
	   << corruptor_p->slot_time(3)-corruptor_p->slot_time(0) << " " 
	   << corruptor_p->slot_time(4)-corruptor_p->slot_time(0) << " " 
	   << corruptor_p->slot_time(5)-corruptor_p->slot_time(0) << " " 
	   << corruptor_p->slot_time(6)-corruptor_p->slot_time(0) << " " 
	   << corruptor_p->slot_time(7)-corruptor_p->slot_time(0) << " " 
	   << endl;  
 
    os << LogIO::NORMAL << "Calculating corruption terms for " << siminfo() << LogIO::POST;
    //-------------------
    // actually calculate the calset
    // which was inflated by sizeupSim to the right size
    
    Vector<Int> slotidx(nSpw(),-1);
    
    vi.originChunks();
    
    Vector<Int> a1;
    Vector<Int> a2;
    Matrix<Bool> flags;
    
    ProgressMeter meter(0.,1. , "Simulating "+nameOfType(type())+" ", "", "", "", true, 1);

    // check if it's possible to simulate ACs
    Bool knownACtype(false);
    String mode(corruptor_p->mode());
    if (type()==VisCal::ANoise)
     knownACtype = true;
    else if (type()==VisCal::T && (mode=="tsys-manual" || mode=="tsys-atm"))
      knownACtype = true;
    
    for (Int isim=0;isim<nSim && vi.moreChunks();++isim) {      
    
      Int thisSpw=spwMap()(vi.spectralWindow());
      currSpw()=thisSpw;
      corruptor_p->currSpw()=thisSpw;
      slotidx(thisSpw)++;
    
      IPosition cparshape=solveCPar().shape();

      Vector<Double> timevec;
      Double starttime,stoptime;
      starttime=vi.time(timevec)[0];
      
      //IPosition blc(3,0,       0,0); // par,chan=focuschan,elem=ant
      //IPosition trc(3,nPar()-1,0,0);
      IPosition blc(3,0,       0,           0); // par,chan=focuschan,elem=ant
      IPosition trc(3,nPar()-1,nChanPar()-1,0);
      IPosition gpos(3,0,0,0);
      
      Bool useBase(false);
      if (nElem()==nBln()) useBase=true;
    
      for (Int ichunk=0;ichunk<nChunkPerSol[isim];++ichunk) {
        // RI todo: SVC:setSim deal with spwmap and spwcomb() here
    
        for (vi.origin(); vi.more(); vi++) {
    
    	if (prtlev()>5) cout << "  vi++"<<endl;
    	vi.antenna1(a1);
          vi.antenna2(a2);
          vi.flag(flags);
          vi.time(timevec);
    	// assume that the corruptor slot i.e. time is the same for all rows.
    	// (to the accuracy of simint())
    	
    	// set things for SVC::keep:
    	Int tvsize;
    	timevec.shape(tvsize);
    	stoptime=timevec[tvsize-1];
    	refTime() = 0.5*(starttime+stoptime);
    	interval() = (stoptime-starttime);
    	currField() = vi.fieldId();
    
    	// make sure we have the right slot in the corruptor 
    	// RI todo can the corruptor slot be the same for all chunks?
    	// were setting curr_time() to timevec[0], but I think refTime is more 
    	// accurate
	// 20100831 make corruptor->setCurrtime() which does slot if ness, 
	// and * invalidates any aux matrices like airmass in atmcorr, 
	// if ness *

    	if (corruptor_p->curr_time()!=refTime()) 
	  corruptor_p->setCurrTime(refTime());
    	
    	solveCPar()=Complex(0.0);
    	solveParOK()=false;
    	
    	for (Int irow=0;irow<vi.nRow();++irow) {
    	  
    	  if (nfalse(flags.column(irow))> 0 ) {
    	    
    	    corruptor_p->currAnt()=a1(irow);
    	    // only used for baseline-based SVCs
    	    corruptor_p->currAnt2()=a2(irow);
    	    
    	    // baseline or antenna-based?
    	    if (useBase) {
    	      gpos(2)=blnidx(a1(irow),a2(irow));
    	    } else {
    	      gpos(2)=a1(irow);
    	    }
    	    
    	    // RI TODO make some freqDepPar VCs return all ch at once
    	    //if not freqDepPar, then nChanPar=1 
    	    for (Int ich=nChanPar()-1;ich>-1;--ich) {		
    	      focusChan()=ich;
    	      corruptor_p->setFocusChan(ich);
    	      gpos(1)=ich;

	      // gpos is (ipar, ich, iant|ibln)
    	      for (Int ipar=0;ipar<nPar();ipar++) {
    		gpos(0)=ipar;
    		if ( a1(irow)==a2(irow) ) {
    		  // autocorrels should get 1. for multiplicative VC
//     		  if (type()==VisCal::ANoise or type()==VisCal::A)
    		  if (type()==VisCal::A)
    		    solveCPar()(gpos)=0.0;
		  else if (knownACtype)
		    solveCPar()(gpos) = corruptor_p->simPar(vi,type(),ipar);
    		  else
    		    solveCPar()(gpos)=1.0;
		  solveParOK()(gpos)=true;		     
    		} else {
    		  // specialized simPar for each VC - may depend on mode etc
    		  solveCPar()(gpos) = corruptor_p->simPar(vi,type(),ipar); 
		  solveParOK()(gpos)=true;	      

		  // if MS doesn't have ACs we need to fill ant2 b/c it'll 
		  // never get selected in this loop over ant1
		  // TODO clean this up
		  if (not useBase) {
		    gpos(2)=a2(irow);
		    if (solveCPar()(gpos)==Complex(0.0)) {
		      corruptor_p->currAnt()=a2(irow);
		      solveCPar()(gpos) = corruptor_p->simPar(vi,type(),ipar); 
		      solveParOK()(gpos)=true;	      
		      corruptor_p->currAnt()=a1(irow);		      
		    }
		    gpos(2)=a1(irow);
		  }
    		}
    	      }


// 20101006 
//    	      if ( a1(irow)==a2(irow) ) {
//    		// autocorrels should get 1. for multiplicative VC
//    		if (type()==VisCal::ANoise or type()==VisCal::A)
//    		  solveCPar()(blc,trc)=0.0;
//    		else
//    		  solveCPar()(blc,trc)=1.0;
//    	      } else {
//    		// specialized simPar for each VC - may depend on mode etc
//    		for (Int ipar=0;ipar<nPar();ipar++) 
//		  // RI TODO left-hand operand of comma has no effect:
//    		  (solveCPar()(blc,trc))[ipar,0,0] = corruptor_p->simPar(vi,type(),ipar);		
//    	      }

    	    } //ich

//    	    if (prtlev()>5) cout << "  row "<<irow<< " set; cparshape="<<solveCPar().shape()<<endl;
    	    // if using gpos and not changing these then they stay set this way
    	    //blc(1)=0;
    	    //trc(1)=nChanPar()-1;
	    //    	    blc(2)=gpos(2);
	    //    	    trc(2)=gpos(2);
    	    //solveParOK()(blc,trc)=true;
    	  }// if not flagged
    	}// row

	// Reference solveCPar, etc. for keepNCT
	solveAllCPar().reference(solveCPar());
	solveAllParOK().reference(solveParOK());
	solveAllParErr().reference(solveParErr());
	solveAllParSNR().reference(solveParSNR());
	currScan()=-1;
	currObs()=0;

	keepNCT();

        }// vi
        if (vi.moreChunks()) vi.nextChunk();
      } // chunk loop
    
      // progress indicator
      meter.update(Double(isim)/nSim);
      
    }// nsim loop
  }

  if (calTableName()!="<none>") {      
    // RI todo SVC::setSimulate check if user wants to overwrite calTable
    os << LogIO::NORMAL 
       << "Writing calTable = "+calTableName()+" ("+typeName()+")" 
       << endl << LogIO::POST;      
    // write the table
    append()=false; 
    storeNCT();
  } else {
    os << LogIO::NORMAL 
       << "calTable name not set - not writing to disk (note: ";
    if (simOnTheFly()) 
      os << "OTF sim - not creating Calset either)";
    else
      os << "NOT OTF sim - still creating Calset)";  
    os << LogIO::POST;
  }  



  if (not simOnTheFly()) {

    // Create the interpolator
    if (ci_)
      delete ci_;

    MeasurementSet ms(msName());
    ci_=new CTPatchedInterp(*ct_,matrixType(),nPar(),tInterpType(),"linear","",ms,spwMap(),cttifactoryptr());

  }

  //  cout << "End of SVC::setSimulate" << endl;


  if (prtlev()>2) cout << " ~SVC::setSimulate(simpar)" << endl;
}







String SolvableVisCal::siminfo() {

  ostringstream o;
  o << "simulated " << typeName() 
    << ": output table="      << calTableName()
    << " simint="     << simint()
    << " t="          << interval();
  return String(o);
}

// ===================================================






String SolvableVisCal::applyinfo() {

  ostringstream o;
  o << typeName()
    << ": table="  << calTableName();

  if (byCallib_)
    o << " (by cal library)";
  else {
    o << " select=" << calTableSelect()
      << " interp=" << tInterpType();
    if (this->freqDepPar())
      o << "," << fInterpType();
    o << " spwmap=" << spwMap();
      
  }

  o << boolalpha << " calWt=" << calWt();
    //    << " t="      << interval();

  return String(o);

}

// NEWCALTABLE ?????
void SolvableVisCal::setSolve() {

  if (prtlev()>2) cout << "SVC::setSolve()" << endl;

  interval()=10.0;
  urefantlist_.resize(1);
  urefantlist_(0)=-1;
  apmode()="AP";
  calTableName()="<none>";
  solnorm_=SolNorm(false,String("mean"));
  minSNR()=0.0f;

  // This is the solve context
  setSolved(true);
  setApplied(false);
  setSimulated(false);

}

void SolvableVisCal::setSolve(const Record& solve) 
{

  if (prtlev()>2) cout << "SVC::setSolve(solve)" << endl;

  // Collect parameters
  if (solve.isDefined("table"))
    calTableName()=solve.asString("table");

  if (calTableName().length()==0)
    throw(AipsError("Please specify a name for the output calibration table!"));

  // Internal default solint 
  solint()="inf";
  fsolint()="none";
  if (solve.isDefined("solint")) {
    usolint_=solve.asString("solint");
    if (usolint_.contains(',')) {
      // both time and freq solint specified
      solint()=usolint_.before(',');
      fsolint()=usolint_.after(',');
    }
    else
      // interpret as only time-dep solint
      solint()=usolint_;
  }

  // Handle solint format
  if (upcase(solint()).contains("INF") || solint()=="") {
    solint()="inf";
    interval()=-1.0;
  }
  else if (upcase(solint()).contains("INT"))
    interval()=0.0;
  else {
    QuantumHolder qhsolint;
    String error;
    Quantity qsolint;
    qhsolint.fromString(error,solint());
    if (error.length()!=0)
      throw(AipsError("Unrecognized units for time-dep solint."));
    qsolint=qhsolint.asQuantumDouble();
    
    if (qsolint.isConform("s"))
      interval()=qsolint.get("s").getValue();
    else {
      if (qsolint.getUnit().length()==0) {
	// when no units specified, assume seconds
	interval()=qsolint.getValue();
	solint()=solint()+"s";
      }
      else
	// unrecognized units:
	throw(AipsError("Unrecognized units for solint (e.g., use 'min', not 'm', for minutes)"));
    }
  }

  // Handle fsolint format
  if (upcase(fsolint()).contains("NONE") || !freqDepPar()) {
    fsolint()="none";
    fintervalCh_.set(0.0); 
    fintervalHz_=-1.0;
  }
  else {
    if (freqDepPar()) {
      // Try to parse it
      if (upcase(fsolint()).contains("CH")) {
	String fsolintstr=upcase(fsolint());
	fintervalCh_.set(String::toDouble(upcase(fsolint()).before("CH")));
	fintervalHz_=-1.0;  // Don't know in Hz, and don't really care
	fsolint()=downcase(fsolint());
      }
      else {
	QuantumHolder qhFsolint;
	String error;
	qhFsolint.fromString(error,fsolint());
	if (error.length()!=0)
	  throw(AipsError("Unrecognized units for freq-dep solint."));
	Quantity qFsolint;
	qFsolint=qhFsolint.asQuantumDouble();
	
	if (qFsolint.isConform("Hz")) {
	  fintervalHz_=qFsolint.get("Hz").getValue();
	  fintervalCh_.set(-1.0);
	  //	  throw(AipsError("Not able to convert freq-dep solint from Hz to channel yet."));
	}
	else {
	  if (qFsolint.getUnit().length()==0) {
	    // when no units specified, assume channel
	    fintervalCh_.set(qFsolint.getValue());
	    fsolint()=fsolint()+"ch";
	  }
	  else
	    // unrecognized units:
	    throw(AipsError("Unrecognized units for freq-dep solint"));
	} // Hz vs. Ch via Quantum
      } // parse by Quantum
    } // freqDepPar
    /*
    cout << "Freq-dep solint: " << fsolint() 
	 << " Ch=" << fintervalCh_ 
	 << " Hz=" << fintervalHz() 
	 << endl;
    */
  } // user set something

  if (solve.isDefined("preavg"))
    preavg()=solve.asFloat("preavg");

  if (solve.isDefined("refantmode")) {
    refantmode_=solve.asString("refantmode");
  }
  if (solve.isDefined("refant")) {
    refantlist().resize();
    refantlist()=solve.asArrayInt("refant");
  }
  if (solve.isDefined("minblperant"))
    minblperant()=solve.asInt("minblperant");

  if (solve.isDefined("apmode"))
    apmode()=solve.asString("apmode");
  apmode().upcase();

  if (solve.isDefined("append"))
    append()=solve.asBool("append");

  if (solve.isDefined("solnorm")) {
    Bool solnorm=solve.asBool("solnorm");

    // normtype="mean" if not specified
    String normtype("mean");
    if (solve.isDefined("normtype")) 
      normtype=solve.asString("normtype");

    // Set the SolNorm object
    solnorm_=SolNorm(solnorm,normtype);

  }

  if (solve.isDefined("minsnr"))
    minSNR()=solve.asFloat("minsnr");

  if (solve.isDefined("combine"))
    combine()=solve.asString("combine");

  //  cout << "SVC::setsolve: minSNR() = " << minSNR() << endl;

  // TBD: Warn if table exists (and append=F)!

  // If normalizable & preavg<0, use full pre-averaging
  //  (or handle this per type, e.g. D)
  // TBD: make a nice log message concerning preavg
  if (normalizable() && preavg()<0.0)
    preavg()=DBL_MAX;

  // This is the solve context
  setSolved(true);
  setApplied(false);

  //  state();

}

String SolvableVisCal::solveinfo() {

  // Get the refant name from the MS
  String refantNames("none");
  if (refant()>-1) {
    refantNames="";
    Int nra=refantlist().nelements();
    for (Int i=0;i<nra;++i) {
      refantNames+=msmc().antennaName(refantlist()(i));
      if (i<nra-1) refantNames+=",";
    }
  }

  ostringstream o;
  o << boolalpha
    << typeName()
    << ": table="      << calTableName()
    << " append="     << append()
    << " solint="     << solint()
    << (freqDepPar() ? (","+fsolint()) : "")
    //    << " t="          << interval()
    //    << " preavg="     << preavg()
    << " refantmode="     << "'" << refantmode_ << "'"
    << " refant="     << "'" << refantNames << "'" // (id=" << refant() << ")"
    << " minsnr=" << minSNR()
    << " apmode="  << apmode()
    << " solnorm=" << solnorm()
    << (solnorm() ? " normtype="+solNorm().normtypeString() : "");
  return String(o);

}


void SolvableVisCal::setAccumulate(VisSet& vs,
				   const String& table,
				   const String& select,
				   const Double& t,
				   const Int&) {


  LogMessage message(LogOrigin("SolvableVisCal","setAccumulate"));

  // meta-info
  calTableName()=table;
  calTableSelect()=select;
  interval()=t;

  // Not actually applying or solving
  setSolved(false);
  setApplied(false);

  // If interval<0, this signals an existing input cumulative table
  if (interval()<0.0) {

    //    throw(AipsError("Accum is temporarily disabled."));
  
    logSink() << "Loading existing " << typeName()
	      << " table: " << table
	      << " for accumulation."
	      << LogIO::POST;


    // Load the exiting table
    loadMemCalTable(calTableName(),"");

    // The following should be for trivial types only!    
    nChanMatList()=nChanParList();


  }

  // else, we are creating a cumulative table from scratch (the VisSet)
  else {

    logSink() << "Creating " << typeName()
	      << " table for accumulation."
	      << LogIO::POST;

    // Creat an empty caltable
    createMemCalTable();

    // Setup channelization (as if solving)
    setSolveChannelization(vs);
    nChanMatList()=nChanParList();

    // Initialize solvePar shapes
    initSolvePar();

    // Inflate it by iteratin over the dataset
    inflateNCTwithMetaData(vs);

  }

}


void SolvableVisCal::setSpecify(const Record& specify) {

  LogMessage message(LogOrigin("SolvableVisCal","setSpecify"));

  // Not actually applying or solving
  setSolved(false);
  setApplied(false);

  // Collect Cal table parameters
  Bool tableExists(false);
  if (specify.isDefined("caltable")) {
    calTableName()=specify.asString("caltable");

    // Detect existence of the table
    tableExists=Table::isReadable(calTableName());

  }

  if (tableExists) {

    // Verify table has correct type
    verifyCalTable(calTableName());

    logSink() << "Loading existing " << typeName()
	      << " table: " << calTableName()
	      << " (to be updated)."
	      << LogIO::POST;

    // Open it
    loadMemCalTable(calTableName());

    // Fill solveParArrays

    Block<String> sortcols(1);
    sortcols[0]="SPECTRAL_WINDOW_ID";
    ROCTIter ctiter(*ct_,sortcols);
    while (!ctiter.pastEnd()) {
      currSpw()=ctiter.thisSpw();
      nChanPar()=ctiter.nchan();
      switch(parType()) {
      case VisCalEnum::COMPLEX: {
	ctiter.cparam(solveAllCPar());
	break;
      }
      case VisCalEnum::REAL: {
	ctiter.fparam(solveAllRPar());
	break;
      }
      default: {
	throw(AipsError("Internal SVC::setSpecify(...) error: Got invalid VisCalEnum"));
	break;
      }
      }
      ctiter.paramErr(solveAllParErr());
      ctiter.snr(solveAllParSNR());
      solveAllParOK().assign(!ctiter.flag());

      // Advance iterator
      ctiter.next();
    }

    // Delete old mem caltable (it will be replaced)
    if (ct_) delete ct_;
    else throw(AipsError("SVC::setSpecify: unknown error on caltable delete"));
    ct_=NULL;

  } // tableExists
  else {

    nChanParList()=Vector<Int>(nSpw(),1);
    startChanList()=Vector<Int>(nSpw(),0);

    // we are creating a table from scratch
    logSink() << "Creating " << typeName()
	      << " table from specified parameters."
	      << LogIO::POST;
  
    // Size up the solve arrays
    initSolvePar();

    for (Int ispw=0;ispw<nSpw();++ispw) {
      currSpw()=ispw;
      refTime()=0.0;
      currField()=-1;
      currScan()=-1;
      currObs()=0;

      switch(parType()) {
      case VisCalEnum::COMPLEX: {
	solveAllCPar().set(defaultCPar());
	break;
      }
      case VisCalEnum::REAL: {
	solveAllRPar().set(defaultRPar());
	break;
      }
      default: {
	throw(AipsError("Internal SVC::setAccumulate(...) error: Got invalid VisCalEnum"));
      }
      }
      solveAllParOK().set(true);
      solveAllParSNR().set(1.0);
      solveAllParErr().set(0.0);
      
    } // ispw
  } // !tableExists

  // Create the caltable
  createMemCalTable();

}

void SolvableVisCal::specify(const Record& specify) {

  LogMessage message(LogOrigin("SolvableVisCal","specify"));

  Vector<Int> spws;
  Vector<Int> antennas;
  Vector<Int> pols;
  Vector<Double> parameters;

  Int nUserSpw(1);
  Int Ntime(1);
  Int Nant(0);
  Int Npol(1);
  
  Bool repspw(false);
  
  IPosition ip0(3,0,0,0);
  IPosition ip1(3,0,0,0);

  if (specify.isDefined("caltype")) {
    String caltype=specify.asString("caltype");
    logSink() << "Generating '" << caltype << "' corrections." << LogIO::POST;
    if (upcase(caltype).contains("PH"))
      apmode()="P";
    else
      apmode()="A";
  }

 /*
  if (specify.isDefined("time")) {
    // TBD: the time label
    cout << "time = " << specify.asString("time") << endl;
    cout << "refTime() = " << refTime() << endl;
  }
 */
/**
  if (specify.isDefined("time")) {
    // TBD: the time label
    //cout << "time = " << specify.asString("time") << endl;
    cout << "refTime() = " << refTime() << endl;
    currTime()=specify.asDouble("time");
  }
**/

  if (specify.isDefined("spw")) {
    // TBD: the spws (in order) identifying the solutions
    spws=specify.asArrayInt("spw");
    nUserSpw=spws.nelements();
    if (nUserSpw<1) {
      // None specified, so loop over all, repetitively
      //  (We ought to optimize this...)
      logSink() << 
	"Specified parameter(s) (per antenna and pol) repeated on all spws." 
		<< LogIO::POST;
      repspw=true;
      nUserSpw=nSpw();
      spws.resize(nUserSpw);
      indgen(spws);
    }
  }


  if (specify.isDefined("antenna")) {
    // TBD: the antennas (in order) identifying the solutions
    antennas=specify.asArrayInt("antenna");
    //    cout << "antenna indices = " << antennas << endl;
    Nant=antennas.nelements();
    if (Nant<1) {
      // Use specified values for _all_ antennas implicitly
      logSink() << 
	"Specified parameter(s) (per spw and pol) repeated on all antennas." 
		<< LogIO::POST;
      Nant=1;   // For the antenna loop below
      ip0(2)=0;
      ip1(2)=nAnt()-1;
    }
    else {
      // Point to first antenna
      ip0(2)=antennas(0);
      ip1(2)=ip0(2);
    }
  }
  if (specify.isDefined("pol")) {
    // TBD: the pols (in order) identifying the solutions
    String polstr=specify.asString("pol");
    //    cout << "pol = " << polstr << endl;
    if (polstr=="R" || polstr=="X") 
      // Fill in only first pol
      pols=Vector<Int>(1,0);
    else if (polstr=="L" || polstr=="Y") 
      // Fill in only second pol
      pols=Vector<Int>(1,1);
    else if (polstr=="R,L" || polstr=="X,Y") {
      // Fill in both pols explicity
      pols=Vector<Int>(2,0);
      pols(1)=1;
    }
    else if (polstr=="L,R" || polstr=="Y,X") {
      // Fill in both pols explicity
      pols=Vector<Int>(2,0);
      pols(0)=1;
    }
    else if (polstr=="")
      // Fill in both pols implicitly
      pols=Vector<Int>();
    else
      throw(AipsError("Invalid pol specification"));
    
    Npol=pols.nelements();
    if (Npol<1) {
      // No pol axis specified
      logSink() << 
	"Specified parameter(s) (per spw and antenna) repeated on all polarizations." 
		<< LogIO::POST;
      Npol=1;
      ip0(0)=0;
      ip1(0)=nPar()-1;
    }
    else {
      // Point to the first polarization
      ip0(0)=pols(0);
      ip1(0)=ip0(0);
    }
  }
  if (specify.isDefined("parameter")) {
    // TBD: the actual cal values
    parameters=specify.asArrayDouble("parameter");
  }
  Int nparam=parameters.nelements();

  // Test for correct number of specified parameters
  //  Either all params are enumerated, or one is specified
  //  for all, [TBD:or a polarization pair is specified for all]
  //  else throw
  if (nparam!=(repspw ? (Ntime*Nant*Npol) : (nUserSpw*Ntime*Nant*Npol)) && 
      nparam!=1 )                // one for all
    //      (Npol==2 && nparam%2!=0) )  // poln pair for all
    throw(AipsError("Inconsistent number of parameters specified."));


  // Fill in user-specifed parameters in order
  Int ipar(0);
  for (Int iUspw=0;iUspw<nUserSpw;++iUspw) {

    currSpw()=spws(iUspw);

    // reset par index if we are repeating for all spws
    if (repspw) ipar=0;
    
    // Loop over specified antennas
    for (Int iant=0;iant<Nant;++iant) {
      if (Nant>1)
	ip1(2)=ip0(2)=antennas(iant);
      
      // Loop over specified polarizations
      for (Int ipol=0;ipol<Npol;++ipol) {
	if (Npol>1)
	  ip1(0)=ip0(0)=pols(ipol);
	
	// Report details as compactly as possible
	if (!repspw || iUspw==0)
	  logSink() << "spwId=" 
		    << (repspw ? "<all>" : String::toString(currSpw()) )
		    << " antId=" 
		    << (antennas.nelements()>0 ? String::toString(antennas(iant)) : "<all>")
		    << " polId=" 
		    << (pols.nelements()>0 ? String::toString(pols(ipol)) : "<all>")
		    << " parameter= " << parameters(ipar)
		    << "   (ip0,ip1 = " << ip0 << "," << ip1 << ")"
		    << LogIO::POST;

	switch(parType()) {
	case VisCalEnum::COMPLEX: {
	  Array<Complex> sl(solveAllCPar()(ip0,ip1));
	  // Multiply ipar-th parameter onto the selecte slice
	  if (apmode()=="P") {
	    // Phases have been specified
	    Double phase=parameters(ipar)*C::pi/180.0;
	    sl*=Complex(cos(phase),sin(phase));
	  }
	  else
	    // Assume amplitude
	    sl*=Complex(parameters(ipar));
	  break;
	}
	case VisCalEnum::REAL: {
	  // Add ipar-th parameter onto the selected slice
	  Array<Float> sl(solveAllRPar()(ip0,ip1));
	  sl+=Float(parameters(ipar));
	  break;
	}
	default: {
	  throw(AipsError("Internal SVC::setAccumulate(...) error: Got invalid VisCalEnum"));
	  break;
	}
	}

	// increment ipar, but be sure not to run off the end
	++ipar;
	ipar = ipar%nparam;


      } // ipol
    } // iant

  } // iUspw


  // Now write keep _all_ spws
  for (Int ispw=0;ispw<nSpw();++ispw) {
    // Keep this result
    currSpw()=ispw;
    keepNCT();
  }

}

Int SolvableVisCal::sizeUpSolve(VisSet& vs, Vector<Int>& nChunkPerSol) {

  // New version that counts solutions (which may overlap in 
  //   field and/or ddid) rather than chunks

  Bool verby(false);

  // Set Nominal per-spw channelization
  setSolveChannelization(vs);

  sortVisSet(vs, verby);
  
  // Number of VisIter chunks per spw
  Vector<Int> nChunkPerSpw(vs.numberSpw(),0);

  Vector<Int> nSolPerSpw(vs.numberSpw(),0);

  // Number of VisIter chunks per solution
  nChunkPerSol.resize(100);
  nChunkPerSol=0;

  VisIter& vi(vs.iter());

  /*
  Block< Vector<Int> > g,st,nch,icr,spw;
  vi.getChannelSelection(g,st,nch,icr,spw);
  for (uInt isel=0;isel<spw.nelements();++isel)
    cout << isel << ":" << endl
	 << " " << "nGrp =" << g[isel]
	 << " " << "start=" << st[isel]
	 << " " << "nchan=" << nch[isel]
	 << " " << "icrem=" << icr[isel]
	 << " " << "spw  =" << spw[isel]
	 << endl;
  */

  VisBuffer vb(vi);
  vi.originChunks();
  vi.origin();
    
  Double time0(86400.0*floor(vb.time()(0)/86400.0));
  Double time1(0.0),time(0.0);

  Int thisobs(-1),lastobs(-1);
  Int thisscan(-1),lastscan(-1);
  Int thisfld(-1),lastfld(-1);
  Int thisspw(-1),lastspw(-1);
  Int chunk(0);
  Int sol(-1);
  Double soltime1(-1.0);
  for (vi.originChunks(); vi.moreChunks(); vi.nextChunk(),chunk++) {
    vi.origin();
    time1=vb.time()(0);  // first time in this chunk
    thisobs=vb.observationId()(0);
    thisscan=vb.scan()(0);
    thisfld=vb.fieldId();
    thisspw=vb.spectralWindow();
    
    nChunkPerSpw(thisspw)++;

    // New chunk means new sol interval, IF....
    if ( (!combfld() && !combspw()) ||              // not combing fld nor spw, OR
	 ((time1-soltime1)>interval()) ||           // (combing fld and/or spw) and solint exceeded, OR
	 ((time1-soltime1)<0.0) ||                  // a negative time step occurs, OR
	 (!combobs() && (thisobs!=lastobs)) ||      // not combing obs, and new obs encountered OR
	 (!combscan() && (thisscan!=lastscan)) ||   // not combing scans, and new scan encountered OR
	 (!combspw() && (thisspw!=lastspw)) ||      // not combing spws, and new spw encountered  OR
	 (!combfld() && (thisfld!=lastfld)) ||      // not combing fields, and new field encountered OR 
	 (sol==-1))  {                              // this is the first interval
      soltime1=time1;
      sol++;

      // Increment solution count per spw
      nSolPerSpw(thisspw)++;

      if (verby) {
	cout << "--------------------------------" << endl;
	cout << "sol = " << sol << endl;
      }
      // increase size of nChunkPerSol array, if needed
      if (nChunkPerSol.nelements()<uInt(sol+1))
	nChunkPerSol.resize(nChunkPerSol.nelements()+100,true);
      nChunkPerSol(sol)=0;
    }

    // Increment chunk-per-sol count for current solution
    nChunkPerSol(sol)++;

    if (verby) {
      cout << "          ck=" << chunk << " " << soltime1-time0 << endl;
      
      Int iter(0);
      for (vi.origin(); vi.more();vi++,iter++) {
	time=vb.time()(0);
	cout  << "                 " << "vb=" << iter << " ";
	cout << "ob=" << vb.observationId()(0) << " ";
	cout << "ar=" << vb.arrayId() << " ";
	cout << "ob=" << vb.observationId()(0) << " ";
	cout << "sc=" << vb.scan()(0) << " ";
	if (!combfld()) cout << "fl=" << vb.fieldId() << " ";
	if (!combspw()) cout << "sp=" << vb.spectralWindow() << " ";
	cout << "t=" << floor(time-time0)  << " (" << floor(time-soltime1) << ") ";
	if (combfld()) cout << "fl=" << vb.fieldId() << " ";
	if (combspw()) cout << "sp=" << vb.spectralWindow() << " ";
	cout << endl;
      }
    }
    
    lastobs=thisobs;
    lastscan=thisscan;
    lastfld=thisfld;
    lastspw=thisspw;
    
  }
  
  if (verby) {
    cout << "nSolPerSpw = " << nSolPerSpw << endl;
    cout << "nChunkPerSpw = " << nChunkPerSpw << " " << sum(nChunkPerSpw) << endl;
    cout << "nchunk = " << chunk << endl;
  }

  Int nSol(sol+1);
  
  nChunkPerSol.resize(nSol,true);
  
  spwMap().resize(vs.numberSpw());
  indgen(spwMap());
  Vector<Int> spwlist=spwMap()(nChunkPerSpw>0).getCompressedArray();
  Int spwlab=0;
  if (combspw()) {
    while (nChunkPerSpw(spwlab)<1) spwlab++;
    if (verby) cout << "Obtaining " << nSol << " solutions, labelled as spw=" << spwlab  << endl;
    spwMap()=-1;  // TBD: needed?
    spwMap()(nChunkPerSpw>0)=spwlab;

    if (verby)
      cout << "nChanParList = " << nChanParList()(nChunkPerSpw>0).getCompressedArray() 
	   << "==" << nChanParList()(spwlab) <<  endl;

    // Verify that all spws have same number of channels (so they can be combined!)
    if (!allEQ(nChanParList()(spwMap()>-1).getCompressedArray(),nChanParList()(spwlab)))
      throw(AipsError("Spws with different selected channelizations cannot be combined."));

    //    nChunkPerSpw = 0;
    //    nChunkPerSpw(spwlab)=nSol;

    nSolPerSpw=0;
    nSolPerSpw(spwlab)=nSol;

  }

  if (verby) {
    cout << " spwMap()  = " << spwMap() << endl;
    cout << " spwlist = " << spwlist << endl;
    cout << "nSolPerSpw = " << nSolPerSpw << endl;
    cout << "nChunkPerSpw = " << nChunkPerSpw << " " << sum(nChunkPerSpw) << endl;
    cout << "Total solutions = " << nSol << endl;
    cout << "nChunkPerSol = " << nChunkPerSol << endl;
  }

  if (combobs())
    logSink() << "Combining observation Ids." << LogIO::POST;
  if (combscan())
    logSink() << "Combining scans." << LogIO::POST;
  if (combspw()) 
    logSink() << "Combining spws: " << spwlist << " -> " << spwlab << LogIO::POST;
  if (combfld()) 
    logSink() << "Combining fields." << LogIO::POST;
  

  //if (!isSimulated()) {
  logSink() << "For solint = " << solint() << ", found "
	    <<  nSol << " solution intervals."
	    << LogIO::POST;
  //}

  // Size the solvePar arrays
  initSolvePar();

  // Return the total number of solution intervals
  return nSol;

}

void SolvableVisCal::sortVisSet(VisSet& vs, const Bool verbose)
{
  // Interpret solution interval for the VisIter
  Double iterInterval(max(interval(),DBL_MIN));
  if (interval() < 0.0) {   // means no interval (infinite solint)
    iterInterval=0.0;
    interval()=DBL_MAX;
  }

  if (verbose) {
    cout << "   interval() = " << interval() ;
    cout << boolalpha << "   combobs()  = " << combobs();
    cout << boolalpha << "   combscan() = " << combscan();
    cout << boolalpha << "   combfld()  = " << combfld() ;
    cout << boolalpha << "   combspw()  = " << combspw() ;
  }

  Int nsortcol(4+(combscan()?0:1)+(combobs()?0:1) );  // include room for scan,obs
  Block<Int> columns(nsortcol);
  Int i(0);
  columns[i++]=MS::ARRAY_ID;
  if (!combobs()) columns[i++]=MS::OBSERVATION_ID;  // force obsid boundaries
  if (!combscan()) columns[i++]=MS::SCAN_NUMBER;  // force scan boundaries
  if (!combfld()) columns[i++]=MS::FIELD_ID;      // force field boundaries
  if (!combspw()) columns[i++]=MS::DATA_DESC_ID;  // force spw boundaries
  columns[i++]=MS::TIME;
  if (combspw() || combfld()) iterInterval=DBL_MIN;  // force per-timestamp chunks
  if (combfld()) columns[i++]=MS::FIELD_ID;      // effectively ignore field boundaries
  if (combspw()) columns[i++]=MS::DATA_DESC_ID;  // effectively ignore spw boundaries
  
  if (verbose) {
    cout << " sort columns: ";
    for (Int i=0;i<nsortcol;++i) 
      cout << columns[i] << " ";
    cout << endl;
  }

  // this sets the vi to send chunks by iterInterval (e.g. integration time)
  // instead of default which would go until the scan changed
  vs.resetVisIter(columns,iterInterval);  
}

Int SolvableVisCal::sizeUpSim(VisSet& vs, Vector<Int>& nChunkPerSol, Vector<Double>& solTimes) {

  // New version that counts solutions (which may overlap in 
  //   field and/or ddid) rather than chunks
  LogIO os(LogOrigin("SVC", "sizeUpSim()", WHERE));

  if (prtlev()>2) cout << "  SVC::sizeUpSim" << endl;
  
  sortVisSet(vs, prtlev() > 2);

  VisIter& vi(vs.iter());
  vi.originChunks();
  vi.origin();
  VisBuffer vb(vi);

  // Number of VisIter chunks per spw
  Vector<Int> nChunkPerSpw(vs.numberSpw(),0);

  Int nSol(1);

  if (simOnTheFly()) {
    nChunkPerSol.resize(1);
    nChunkPerSol=1;

    vi.origin();
    solTimes.resize(1);
    solTimes(0)=vb.time()(0);  // first time in this chunk

  } else {


  // Number of VisIter chunks per solution
  nChunkPerSol.resize(100);
  nChunkPerSol=0;
    
  Double time0(86400.0*floor(vb.time()(0)/86400.0));
  Double time1(0.0),time(0.0);

  Int thisscan(-1),lastscan(-1);
  Int thisfld(-1),lastfld(-1);
  Int thisspw(-1),lastspw(-1);
  Int chunk(0);
  Int sol(-1);
  Double soltime1(-1.0);
  for (vi.originChunks(); vi.moreChunks(); vi.nextChunk(),chunk++) {
    vi.origin();
    time1=vb.time()(0);  // first time in this chunk
    thisscan=vb.scan()(0);
    thisfld=vb.fieldId();
    thisspw=vb.spectralWindow();
    
    nChunkPerSpw(thisspw)++;

    // New chunk means new sol interval, IF....
    if ( (!combfld() && !combspw()) ||              // not combing fld nor spw, OR
	 ((time1-soltime1)>interval()) ||           // (combing fld and/or spw) and solint exceeded, OR
	 ((time1-soltime1)<0.0) ||                  // a negative time step occurs, OR
	 (!combscan() && (thisscan!=lastscan)) ||   // not combing scans, and new scan encountered OR
	 (!combspw() && (thisspw!=lastspw)) ||      // not combing spws, and new spw encountered  OR
	 (!combfld() && (thisfld!=lastfld)) ||      // not combing fields, and new field encountered OR 
	 (sol==-1))  {                              // this is the first interval
      soltime1=time1;
      sol++;
      if (prtlev()>5) {
	cout << "--------------------------------" << endl;
	cout << "   sol = " << sol << endl;
      }
      // increase size of nChunkPerSol array, if needed
      if (nChunkPerSol.nelements()<uInt(sol+1))
	nChunkPerSol.resize(nChunkPerSol.nelements()+100,true);
      nChunkPerSol(sol)=0;
      // keep the times!
      if (solTimes.nelements()<uInt(sol+1))
	solTimes.resize(solTimes.nelements()+100,true);
      solTimes(sol)=soltime1;
    }

    // Increment chunk-per-sol count for current solution
    nChunkPerSol(sol)++;

    if (prtlev()>5) {
      cout << "          ck=" << chunk << " " << soltime1-time0 << endl;
      
      Int iter(0);
      for (vi.origin(); vi.more();vi++,iter++) {
	time=vb.time()(0);
	cout  << "                 " << "vb=" << iter << " ";
	cout << "ar=" << vb.arrayId() << " ";
	cout << "sc=" << vb.scan()(0) << " ";
	if (!combfld()) cout << "fl=" << vb.fieldId() << " ";
	if (!combspw()) cout << "sp=" << vb.spectralWindow() << " ";
	cout << "t=" << floor(time-time0)  << " (" << floor(time-soltime1) << ") ";
	if (combfld()) cout << "fl=" << vb.fieldId() << " ";
	if (combspw()) cout << "sp=" << vb.spectralWindow() << " ";
	cout << endl;
      }
    }
    
    lastscan=thisscan;
    lastfld=thisfld;
    lastspw=thisspw;
    
  }
  
  nSol = sol+1;

  nChunkPerSol.resize(nSol,true);
  solTimes.resize(nSol,true);

  if (prtlev()>5) {
    cout << "   solTimes = " << solTimes-solTimes[0] << endl;
    cout << "   nChunkPerSol = " << nChunkPerSol << " " << sum(nChunkPerSol) << endl;
  }
  } // if not inTheFly

  // Set Nominal per-spw channelization - this does set chanParList to full
  // # chans
  setSolveChannelization(vs);
  if (prtlev()>3) cout<<"   freqDepPar="<<freqDepPar()<<endl;
  if (prtlev()>2) cout<<"   nSpw()="<<nSpw()
		      <<" nPar()="<<nPar()<<" nChanParList="<<nChanParList()
		      <<" nElem()="<<nElem()<<" nSol="<<nSol
		      <<" approx size = "<<(nSpw()*(nChanParList().size())*nElem()*nSol*nPar())
		      <<"x size(complex)"<<endl;
  
  if (not simOnTheFly()) {

    if (ct_) 
      delete ct_;

    // Make a caltable into which simulated solutions will be deposited
    // !freqDepPar controls channelization in SPW subtable  (T == single channel)
    ct_=new NewCalTable(calTableName()+"_sim_temp",parType(),typeName(),msName(),!freqDepPar());

  }


  spwMap().resize(vs.numberSpw());
  indgen(spwMap());
  Vector<Int> spwlist=spwMap()(nChunkPerSpw>0).getCompressedArray();
  Int spwlab=0;
  if (combspw()) {
    while (nChunkPerSpw(spwlab)<1) spwlab++;
    if (prtlev()>2) cout << "   obtaining " << nSol << " solutions, labelled as spw=" << spwlab  << endl;
    spwMap()=-1;  // TBD: needed?
    spwMap()(nChunkPerSpw>0)=spwlab;

    if (prtlev()>2)
      cout << "   nChanParList = " << nChanParList()(nChunkPerSpw>0).getCompressedArray() 
	   << "==" << nChanParList()(spwlab) <<  endl;

    // Verify that all spws have same number of channels (so they can be combined!)
    if (!allEQ(nChanParList()(spwMap()>-1).getCompressedArray(),nChanParList()(spwlab)))
      throw(AipsError("Spws with different selected channelizations cannot be combined."));

    nChunkPerSpw = 0;
    nChunkPerSpw(spwlab)=nSol;
  }

  if (prtlev()>2) {
    cout << "   spwMap()  = " << spwMap() ;
    cout << " spwlist = " << spwlist ;
    cout << " nChunkPerSpw = " << nChunkPerSpw << " " << sum(nChunkPerSpw) << " = " << nSol << endl;
    //cout << "Total solutions = " << nSol << endl;
  }
  if (prtlev()>5) 
    cout << "   nChunkPerSim = " << nChunkPerSol << endl;
  
  
  if (combscan())
    os << "Combining scans." << LogIO::POST;
  if (combspw()) 
    os << "Combining spws: " << spwlist << " -> " << spwlab << LogIO::POST;
  if (combfld()) 
    os << "Combining fields." << LogIO::POST;
  
  os << "For simint = " << simint() << ", found "
     <<  nSol << " solution intervals."
     << LogIO::POST;
  
  // Size the solvePar arrays
  // Jones' insists on having 1 channel, but mullers have lots.  
  // initSolvePar();
  // so i have to copy this from initsolvepar and make it all chans

  for (Int ispw=0;ispw<nSpw();++ispw) {
    
    currSpw()=ispw;

    switch(parType())
      {
      case VisCalEnum::COMPLEX:
	{
	  os << LogIO::DEBUG1 << "spw " << currSpw() 
	     << " nPar=" << nPar() << "nChanPar=" << nChanPar() 
	     << " nElem=" << nElem() << LogIO::POST;
	  
	    solveCPar().resize(nPar(),nChanPar(),nElem());
	   solveParOK().resize(nPar(),nChanPar(),nElem());
	  solveParErr().resize(nPar(),nChanPar(),nElem());
	  solveParSNR().resize(nPar(),nChanPar(),nElem());
	  	  
	  solveCPar()=Complex(1.0);
	  solveParOK()=true;
	  solveParErr()=0.0;
	  solveParSNR()=0.0;
	  break;
	}
      case VisCalEnum::REAL:
	{
	    solveRPar().resize(nPar(),nChanPar(),nElem());
	   solveParOK().resize(nPar(),nChanPar(),nElem());
	  solveParErr().resize(nPar(),nChanPar(),nElem());
	  solveParSNR().resize(nPar(),nChanPar(),nElem());
	  
	  solveRPar()=0.0;
	  solveParOK()=true;
	  solveParErr()=0.0;
	  solveParSNR()=0.0;
	  break;
	}
      case VisCalEnum::COMPLEXREAL:
         throw(AipsError("Internal error(Calibrater Module): Unsupported parameter type COMPLEXREAL found"));
         break;
      }
  }

  
  if (not simOnTheFly()) {
    os << LogIO::DEBUG1 
      //       << "calset shape = " << cs().shape(0) 
       << " solveCPar shape = " << solveCPar().shape() 
       << LogIO::POST;
  }

  if (prtlev()>2) cout << "  ~SVC::sizeUpSim" << endl;

  // Return the total number of solution intervals
  return nSol;

}

//  VI2-related refactorings------------------

void SolvableVisCal::setMeta(Int obs, Int scan, Double time,
			     Int spw, const Vector<Double>& freq,
			     Int fld) {

  VisCal::setMeta(obs,scan,time,spw,freq,fld);

  refTime()=time;  // current time for solving is _refTime()_
  
  //  refFreq()=???

}


// Setup solvePar shape for a spw
Int SolvableVisCal::sizeSolveParCurrSpw(Int nVisChan) {

  // Sizes the solvePar arrays for the currSpw()
  
  if (prtlev()>3) cout << " SVJ::sizeSolveParCurrSpw()" << endl;

  // Use nVisChan only for freqDepPar() types
  Int nChan = ( freqDepPar() ? nVisChan : 1);

  // Keep old way informed (needed?)
  nChanPar()=nChan;

  // Now, size the arrays:

  IPosition parsh(3,nPar(),nChan,nElem());  // multi-chan
  IPosition parsh1(3,nPar(),1,nElem());     // single-chan
  switch (parType()) {
  case VisCalEnum::COMPLEX: {
    solveAllCPar().resize(parsh);
    solveAllCPar()=Complex(1.0);
    if (nChan==1)
      solveCPar().reference(solveAllCPar());
    else {
      solveCPar().resize(parsh1);
      solveCPar()=Complex(1.0);
    }
    break;
  }
  case VisCalEnum::REAL: {
    solveAllRPar().resize(parsh);
    solveAllRPar()=0.0;
    if (nChanPar()==1)
      solveRPar().reference(solveAllRPar());
    else {
      solveRPar().resize(parsh1);
      solveRPar()=0.0;
    }
    break;
  }
  default:
    throw(AipsError("Internal error(Calibrater Module): Unsupported parameter type "
		    "COMPLEXREAL found in SVC::sizeSolveParCurrSpw"));
  }
    
  solveAllParOK().resize(parsh);
  solveAllParErr().resize(parsh);
  solveAllParSNR().resize(parsh);
  solveAllParOK()=True;
  solveAllParErr()=0.0;
  solveAllParSNR()=0.0;
  if (nChan==1) {
      solveParOK().reference(solveAllParOK());
      solveParErr().reference(solveAllParErr());
      solveParSNR().reference(solveAllParSNR());
  }
  else {
    // solving many channels, one at a time
    solveParOK().resize(parsh1);
    solveParErr().resize(parsh1);
    solveParSNR().resize(parsh1);
    solveParOK()=True;
    solveParErr()=0.0;
    solveParSNR()=0.0;
  }

  // return the realized nChan
  return nChan;

}

void SolvableVisCal::setDefSolveParCurrSpw(Bool sync) {

  // TBD: generalize for type-dep def values, etc.

  switch (parType()) {
  case VisCalEnum::COMPLEX: {
    AlwaysAssert(solveCPar().nelements()>0,AipsError);
    solveCPar().set(1.0);  //  def=1+0j
    break;
  }
  case VisCalEnum::REAL: {
    AlwaysAssert(solveRPar().nelements()>0,AipsError);
    solveRPar().set(0.0);  //  def=0
    break;
  }
  default:
    throw(AipsError("Internal error(Calibrater Module): Unsupported parameter type "
		    "COMPLEXREAL found in SVC::setDefSolveParCurrSpw"));
  }
  solveParOK().set(True);

  if (sync)
    syncSolveCal();

}

// Parse solint in VI2 context
void SolvableVisCal::reParseSolintForVI2() {

  // Internal default solint 
  solint()="inf";
  fsolint()="none";
  if (usolint_.contains(',')) {
    // both time and freq solint specified
    solint()=usolint_.before(',');
    fsolint()=usolint_.after(',');
  }
  else
    // interpret as only time-dep solint
    solint()=usolint_;

  // solint is always "int" for single dish calibration
  if (longTypeName().startsWith("SDGAIN_OTFD")) {
    //return;
    solint() = "int";
  }

  // Handle solint format
  if (upcase(solint()).contains("INF") || solint()=="") {
    solint()="inf";
    solTimeInterval_=DBL_MAX;
  }
  else if (upcase(solint()).contains("INT"))
    solTimeInterval_=FLT_MIN;  // implausibly small; forces chunk boundaries at integrations
  else {
    QuantumHolder qhsolint;
    String error;
    Quantity qsolint;
    qhsolint.fromString(error,solint());
    if (error.length()!=0)
      throw(AipsError("Unrecognized units for time-dep solint."));
    qsolint=qhsolint.asQuantumDouble();
    
    if (qsolint.isConform("s"))
      solTimeInterval_=qsolint.get("s").getValue();
    else {
      if (qsolint.getUnit().length()==0) {
        // when no units specified, assume seconds
        solTimeInterval_=qsolint.getValue();
        solint()=solint()+"s";
      }
      else
        // unrecognized units:
        throw(AipsError("Unrecognized units for solint (e.g., use 'min', not 'm', for minutes)"));
    }
  }

  //  cout << "******* VI2: Review fsolint parsing..." << endl;

  // Maybe should just parse it, and then work out logic re freqDepPar, etc.

  // Handle fsolint format
  // TBD:  compare to logic in Calibrater::genericGatherAndSolve (line ~2298)
  if (upcase(fsolint()).contains("NONE") ||   // unspecified  OR  
      !freqDepMat()) {                        // cal is entirely unchannelizedb 
    fsolint()="none";
    fintervalCh_.set(-1.0);    // signals full averaging (this is different from old way)
    fintervalHz_=-1.0;      // don't care
  }
  else {
    // something specified OR freqDepMat
    //   if pars are freq-dep, specification indicates desired soln resolution
    if (freqDepPar()) {
      // Try to parse it
      if (upcase(fsolint()).contains("CH")) {
	String fsolintstr=upcase(fsolint());
	fintervalCh_.set(String::toDouble(upcase(fsolint()).before("CH")));
	fintervalHz_=-1.0;  // Don't know in Hz, and don't really care
	fsolint()=downcase(fsolint());
      }
      else {
	QuantumHolder qhFsolint;
	String error;
	qhFsolint.fromString(error,fsolint());
	if (error.length()!=0)
	  throw(AipsError("Unrecognized units for freq-dep solint."));
	Quantity qFsolint;
	qFsolint=qhFsolint.asQuantumDouble();
	
	if (qFsolint.isConform("Hz")) {
	  fintervalHz_=qFsolint.get("Hz").getValue();
	  convertHzToCh();
	}
	else {
	  if (qFsolint.getUnit().length()==0) {
	    // when no units specified, assume channel
	    fintervalCh_.set(qFsolint.getValue());
	    fsolint()=fsolint()+"ch";
	  }
	  else
	    // unrecognized units:
	    throw(AipsError("Unrecognized units for freq-dep solint"));
	} // Hz vs. Ch via Quantum
      } // parse by Quantum
    } // freqDepPar
  } // user set something
  /*
  cout << "Freq-dep solint: " << fsolint() 
       << " Ch=" << fintervalCh_ 
       << " Hz=" << fintervalHz() 
       << endl;
  //*/

}


// Generate the in-memory caltable (empty)
//  NB: no subtable revisions
void SolvableVisCal::createMemCalTable2() {

  //  cout << "createMemCalTable" << endl;

  // Set up description
  String partype = ((parType()==VisCalEnum::COMPLEX) ? "Complex" : "Float");
  CTDesc caltabdesc(partype,Path(msName()).baseName(),typeName(),"unknown");
  ct_ = new NewCalTable("tempNCT.tab",caltabdesc,Table::Scratch,Table::Memory);

  if (msmc().msOk())
    ct_->setMetaInfo(msName());
  else {
    ct_->fillGenericObs(1);
    ct_->fillGenericField(msmc().nFld());
    ct_->fillGenericAntenna(msmc().nAnt());
    Vector<Int> nchan(msmc().nSpw(),1);
    ct_->fillGenericSpw(msmc().nSpw(),nchan);
  }

  // Flag all SPW subtable rows; we'll set them OTF
  CTColumns ncc(*ct_);

  // Set FLAG_ROW in SPW subtable
  Vector<Bool> flr=ncc.spectralWindow().flagRow().getColumn();
  flr.set(True);
  ncc.spectralWindow().flagRow().putColumn(flr);

  // Collapse channel axis info in all rows for unchan'd 
  //  calibration, so columns are "clean" (uniform shape)
  // NB: some of this info will be revised during data iteration
  CTSpWindowColumns& spwcol(ncc.spectralWindow());
  if (!freqDepPar()) {
    Int nspw=ncc.spectralWindow().nrow();
    for (Int ispw=0;ispw<nspw;++ispw) {
      Vector<Double> chfr,chwid,chres,cheff;

      spwcol.chanFreq().get(ispw,chfr);
      spwcol.chanWidth().get(ispw,chwid);
      spwcol.resolution().get(ispw,chres);
      spwcol.effectiveBW().get(ispw,cheff);

      spwcol.chanFreq().put(ispw,Vector<Double>(1,mean(chfr)));
      spwcol.chanWidth().put(ispw,Vector<Double>(1,sum(chwid)));
      spwcol.resolution().put(ispw,Vector<Double>(1,sum(chres)));
      spwcol.effectiveBW().put(ispw,Vector<Double>(1,sum(cheff)));
    }
    
    // One channel per spw
    spwcol.numChan().putColumn(Vector<Int>(nspw,1));
  }

}

void SolvableVisCal::setOrVerifyCTFrequencies(Int spw) {

  //  cout << "SVC::setOrVerifyCTFrequencies......." << endl;


  // Assumes currFreq() is set properly (see syncSolveMeta)

  // Access SPW subtable columns
  CTColumns ctcol(*ct_);
  CTSpWindowColumns& spwcol(ctcol.spectralWindow());

  // If row is flagged, then it hasn't been set yet...
  Bool needToSet(spwcol.flagRow().get(spw));

  // How many solution channels?
  Int nChan=currFreq().nelements();

  Vector<Double> currFreqHz;
  currFreqHz.assign(currFreq());  // currFreq is in GHz!!
  currFreqHz*=1e9;                // currFreqHz is in Hz

  if (needToSet) {

    //cout << "Setting freqs in spw=" << spw << endl;
    
    // Existing values (from the _unaveraged_ MS)
    Vector<Double> chfr,chwid,chres,cheff;
    Double totbw;
    spwcol.chanFreq().get(spw,chfr);
    spwcol.chanWidth().get(spw,chwid);
    spwcol.resolution().get(spw,chres);
    spwcol.effectiveBW().get(spw,cheff);
    totbw=spwcol.totalBandwidth().get(spw);

    // Setup freq info for caltable accordingly
    if (nChan>1) {
      // Incoming data is channelized...
      Double df=currFreqHz(1)-currFreqHz(0); // apparent width
      totbw=(currFreqHz(nChan-1)-currFreqHz(0))+df;  // total span (ignoring gaps!)
      if (freqDepPar()) {
	// solution is channelized
	
	// Assumes uniform width
	// TBD: do better job here for quirky non-gridded cases!
	chfr.resize(nChan); chfr.assign(currFreqHz);
	chwid.resize(nChan); chwid.set(df);
	chres.resize(nChan); chres.set(df);
	cheff.resize(nChan); cheff.set(df);
      }
      else {
	// Data channelized, but solution is not  (e.g., delays)
	chfr.resize(1); chfr.set(mean(currFreqHz));  // The ~centroid freq
	chwid.resize(1); chwid.set(totbw);
	chres.resize(1); chres.set(totbw);
	cheff.resize(1); cheff.set(totbw);
      }
    }
    else {
      // Incoming data has only one channel

      // Assume full collapse of existing freq axis
      // NB: Using UN-SELECTED MS total bandwidth here!!
      // TBD:  this is wrong for partially selected channels....
      AlwaysAssert(currFreqHz.nelements()==1,AipsError);
      chfr.resize(1);   chfr.assign(currFreqHz);
      chwid.resize(1);  chwid.set(totbw);
      chres.resize(1);  chres.set(totbw);
      cheff.resize(1);  cheff.set(totbw);
    }

    // Export revised values to the table
    spwcol.chanFreq().put(spw,chfr);
    spwcol.chanWidth().put(spw,chwid);
    spwcol.resolution().put(spw,chres);
    spwcol.effectiveBW().put(spw,cheff);
    spwcol.numChan().put(spw,(freqDepPar()?nChan:1)); // handles unchan'd par case
    spwcol.totalBandwidth().put(spw,totbw);
    spwcol.flagRow().put(spw,False);

  }
  else {
    // Only verify that freqs haven't changed

    //cout << "Verifying freqs in spw=" << spw << endl;

    Vector<Double> currCTFreq;
    spwcol.chanFreq().get(spw,currCTFreq);


    if (!freqDepPar()) {
      Double currFreqHz1=mean(currFreqHz);
      currFreqHz.resize(1);
      currFreqHz.set(currFreqHz1);
    }

    if (!allEQ(currCTFreq,currFreqHz)) {
      cout << "For spw=" << spw << ":" << endl;
      cout << "Current CalTable nchan= " << currCTFreq.nelements() << endl;
      cout << "Current CalTable freq = " << currCTFreq << endl;
      cout << "Current Solution nchan= " << (freqDepPar() ? nChan : 1) << endl;
      cout << "Current Solution freq = " << currFreqHz << endl;
      cout << "Diff = " << currFreqHz-currCTFreq << endl;

      
      throw(AipsError("Mismatch between Solution frequencies and existing CalTable frequencies for spw="+String::toString(spw)));
    }
  }
}

void SolvableVisCal::setCTFrequencies(Int spw) {

  //  cout << "SVC::setCTFrequencies......." << endl;


  // Discern spw to which to write
  Int outspw=spw;
  // If combining spws, freqMetaData_ knows the fan-in
  if (combspw())
    outspw=freqMetaData_.fannedInSpw(spw);

  // Access SPW subtable columns
  CTColumns ctcol(*ct_);
  CTSpWindowColumns& spwcol(ctcol.spectralWindow());

  // If row is flagged, then it hasn't been set yet (in this execution or any prior)
  Bool needToSet(spwcol.flagRow().get(outspw));


  //  cout << "needToSet = " << boolalpha << needToSet << endl;


  if (needToSet) {

    //cout << "Setting freqs for spw=" << outspw << endl;

    // const references to freq info from freqMetaData_
    const Vector<Double>& chfr(freqMetaData_.freq(outspw));
    const Vector<Double>& chwid(freqMetaData_.width(outspw));
    const Vector<Double>& cheff(freqMetaData_.effBW(outspw));

    // Derived info
    const Vector<Double>& chres(chfr);   // resolution same as freq
    Int numChan=chfr.nelements();
    Double totbw=sum(chfr);

    // TBD: add some sanity checks, e.g., numChan==1 if !freqDepPar, etc.

    // Set freq info in the table
    spwcol.chanFreq().put(outspw,chfr);
    spwcol.chanWidth().put(outspw,chwid);
    spwcol.resolution().put(outspw,chres);
    spwcol.effectiveBW().put(outspw,cheff);
    spwcol.numChan().put(outspw,numChan);
    spwcol.totalBandwidth().put(outspw,totbw);
    spwcol.flagRow().put(outspw,False);


  }
  else {

    // outspw already set (not yet written to disk, unless (possibly) append=True; checked elsewhere)
    //  Verify that info matches...  this shouldn't be necessary, in general,
    //  as discernAndSetSolutionFrequencies sets the rigorously....
    // TBD: test against _apparent_ frequencies in this soluton interval, e.g.,
    //   are available spws consistent....  (and merely warn)

    //cout << "Verifying freqs for spw=" << outspw << endl;

    // Existing freqs in table...
    Vector<Double> currCTFreq;
    spwcol.chanFreq().get(outspw,currCTFreq);
    Int numChan;
    spwcol.numChan().get(outspw,numChan);

    // Current solution freqs
    const Vector<Double>& chfr(freqMetaData_.freq(outspw));
    const Int numChanOut(chfr.nelements());

    // If values mismatch, we need to abort
    if (numChan!=numChanOut || !allEQ(currCTFreq,chfr)) {
      cout << "For spw=" << outspw << ":" << endl;
      cout << "Current CalTable nchan= " << numChan << endl;
      cout << "Current CalTable freq = " << currCTFreq << endl;
      cout << "Current Solution nchan= " << numChan << endl;
      cout << "Current Solution freq = " << chfr << endl;
      cout << "Diff = " << chfr-currCTFreq << endl;
      
      throw(AipsError("Mismatch between Solution frequencies and existing CalTable frequencies for spw="+String::toString(spw)));
    }

  } // !needToSet

}

// Discern detailed frequency meta info for solutions (solve context)
void SolvableVisCal::discernAndSetSolnFrequencies(const vi::VisibilityIterator2& vi, const Vector<uInt>& selspws) {

  //  cout << endl << "SVC::discernAndSetSolnFrequencies---------------------------" << endl;

  // TBD:  When aggregating, we should verify that all frames are consistent
  Vector<Int> frames(vi.spectralWindowSubtablecols().measFreqRef().getColumn());
  //cout << "vi.spectralWindowSubtablecols().measFreqRef() = " << frames << endl;

  // Gather MS freq info
  Vector< Vector<Double> > MSfreq(nSpw(),Vector<Double>()), MSwidth(nSpw(),Vector<Double>());
  for (uInt i=0;i<selspws.nelements();++i) {
    uInt ispw=selspws(i);

    MSfreq(ispw).assign(vi.getImpl()->getFrequencies(-1.0e0,frames(ispw),ispw,0));
    MSwidth(ispw).assign(vi.getImpl()->getChanWidths(-1.0e0,frames(ispw),ispw,0));

    //    cout << "Spw=" << ispw << ":" << endl 
    //	 << " MSfreq=" << MSfreq(ispw) << endl
    //	 << " MSwidth=" << MSwidth(ispw) << endl;
  }

  // Nominally empty spwFanIn...
  Vector<Int> spwFanIn;

  // Work out spwFanIn from selecte spws, if we are combining spws...
  if (combspw()) {

    // We're combining, so set up the "spwmap" for spw fan-in
    spwFanIn.resize(nSpw());
    spwFanIn.set(-1);  // -1 means not included

    // Use the MINIMUM spwid from selection for the aggregate
    Int aggSpw=min(selspws);
    for (uInt iselspw=0;iselspw<selspws.nelements();++iselspw) 
      spwFanIn(selspws(iselspw))=aggSpw;

  }

  // Now delegate the freq meta calculation to the FreqMetaData object
  freqMetaData_.calcFreqMeta(MSfreq,MSwidth,selspws,freqDepPar(),combspw(),spwFanIn);


  // If appending, check current/pending freq meta data againt existing info on disk
  if (append()) {
    const CTSpectralWindow ctspw(calTableName()+"/SPECTRAL_WINDOW");
    const CTSpWindowColumns& spwcol(ctspw);

    // Which spws are we procssing now?
    Vector<Int> validfMDspws(freqMetaData_.validSpws());

    // If current spws already in disk table, freq meta must match!
    for (uInt i=0;i<validfMDspws.nelements();++i) {
      const Int& ispw(validfMDspws(i));

      // If disk table already has this spw set (not flagged), we need to check for a match
      if (!spwcol.flagRow().get(ispw)) {
	
	// disk table info
	const uInt numChan(spwcol.numChan().get(ispw));
	const Vector<Double> currCTFreq(spwcol.chanFreq().get(ispw));

	// current pending freq info
	const Vector<Double>& fMDfreq(freqMetaData_.freq(ispw));

	// TBD: check other freq meta info (width, resoln, effBW)?

	// Insist nchan and freq(s) match!
	if (numChan!=fMDfreq.nelements() || !allEQ(currCTFreq,fMDfreq))
	  throw(AipsError("Mismatch with frequency meta-data in append to "+
			  calTableName()+" detected in spw="+String::toString(ispw)+". Check spw selection."));

      } // !flagged in disk table

    } // validfMDspws(i}

  } // append?



  // TBD: Add more log info, probably inside FreqMetaData...

}



//  VI2------------------------^



// The inflate methods will soon deprecate (gmoellen, 20121212)
//   (the are assumed to exist only by LJJones and EPJones, which
//    are not yet NewCalTable-compliant)

// Inflate the internal CalSet according to VisSet info
void SolvableVisCal::inflate(VisSet& vs, const Bool& /* fillMeta */) {

  if (prtlev()>3) cout << " SVC::inflate(vs)" << endl;

  // This method sets up various shape parameters
  //  according to the current VisSet.  It is necessary
  //  to run this after Calibrater::setdata and before
  //  the main part of the solve.  (In case the setdata
  //  was run after the setsolve.)  This method calls
  //  a generic version to interpret the data shapes
  //  in the proper context-dependent way and size
  //  the CalSet.

  //  TBD: Move this slot counting exercise out to Calibrater?
  //   --> Not clear we should do this...

  //  TBD: How do we generalize this to create bracketing
  //   slots at scan start/stop (for accumulation context)?

  // Count slots in the VisIter
  Vector<Int> nSlot(nSpw(),0);
  {
    VisIter& vi(vs.iter());
    for (vi.originChunks(); vi.moreChunks(); vi.nextChunk())
      nSlot(vi.spectralWindow())++;
    vi.originChunks();

    logSink() << "For interval of "<<interval()<<" seconds, found "
	      <<  sum(nSlot)<<" slots"
	      << LogIO::POST;
  }

  // Call generic version to actually inflate the CalSet
  //  (assumes nChanParList()/startChanList() already valid!)
  inflate(nChanParList(),startChanList(),nSlot);

}


// Inflate the internal CalSet generically
void SolvableVisCal::inflate(const Vector<Int>& /*nChan*/,
			     const Vector<Int>& /*startChan*/,
			     const Vector<Int>& /*nSlot*/) {

  if (prtlev()>3) cout << "  SVC::inflate(,,)" << endl;

  throw(AipsError("Attempt to use deprecated SVC::inflate method."));

}

void SolvableVisCal::setSolveChannelization(VisSet& vs) {

  //  TBD: include anticipated decimation when partial freq ave supported?
  //      (NB: note difference between chan-ave on selection [VisSet] and
  //       chan-ave on-the-fly with vb.freqAve())


  Vector<Int> nDatChan(vs.numberChan());
  Vector<Int> startDatChan(vs.startChan());

  // Figure out channel axis shapes (solve context!):

  // If multi-channel pars, this is a frequency-sampled calibration (e.g., B)
  if (freqDepPar()) {
    // Overall par shape follows data shape
    nChanParList() = nDatChan;
    startChanList() = startDatChan;

    // Handle partial freq average
    if (fsolint()!="none" && (allGT(fintervalCh_,0.0)||fintervalHz_>0.0))
      setFracChanAve();
    
    // However, for solving, we will only consider one channel at a time:
    nChanMatList() = 1;

  }
  else {
    // Pars are not themselves channel-dependent
    nChanParList() = 1;

    // Check if matrices may still be freq-dep:
    if (freqDepMat()) {
      // cal is an explicit f(freq) (e.g., like delay)
      nChanMatList()  = nDatChan;
      startChanList() = startDatChan;
    } else {
      // cal has no freq dep at all
      nChanMatList()  = Vector<Int>(nSpw(),1);
      startChanList() = Vector<Int>(nSpw(),0);
    }

  }

  // At this point:
  //  1. nChanParList() represents the (per-Spw) overall length of the
  //     output parameter channel axis, appropriate for shaping the
  //     output NewCalTable.  This value is irrelevant during the solve, since
  //     we will only solve for one parameter channel at a time (or 
  //     there is only one channel to solver for).
  //  2. nChanMatList() represents the per-Spw  matrix channel axis length to
  //     be used during the solve, independent of the parameter channel
  //     axis length.  In the solve context, nChanMat()>1 when there is
  //     more than one channel of data upon which the (single channel)
  //     solve parameters depend (e.g., delay, polynomial bandpass, etc.)

}




void SolvableVisCal::convertHzToCh() {

  //  cout << "convertHzToCh!" << endl;

  // Access the channel widths vis msmc, etc.
  vector<QVD> chanwidths=msmc().msmd().getChanWidths();

  logSink() << LogIO::NORMAL;
  logSink() << " Frequency solint parsing:" << LogIO::POST;
  for (Int ispw=0;ispw<nSpw();++ispw) {
    currSpw()=ispw;
    // Calculate channel increment from Hz
    if (fintervalCh()<0.0 && fintervalHz()>0.0) {
      // Assumes constant chan width in each spw!
      Double datawidthHz=abs(chanwidths[ispw][0].get("Hz").getValue()); 
      fintervalCh()=floor(fintervalHz()/datawidthHz);
      if (fintervalCh()<1.0) fintervalCh()=1.0;  // nothing fractional <1.0

      logSink() << ".  Spw " << ispw << ": "
		<< " (freq solint: " << fintervalHz() << " Hz) / (data width: " << datawidthHz << " Hz)"
		<< " = " << fintervalCh() << " data channels per solution channel."
		<< LogIO::POST;
    }
  } // ispw  

}




void SolvableVisCal::setFracChanAve() {

  // TBD: calculate fintervalCh from fintervalHz
  MeasurementSet ms(msName());
  MSSpWindowColumns spwcol(ms.spectralWindow());

  //  cout << "setFracChanAve!" << endl;
  for (Int ispw=0;ispw<nSpw();++ispw) {
    //    cout << "ispw=" << ispw << ":" << endl;
    //    cout << " nChanData      = " << nChanPar() << endl;
    //    cout << " startChan()    = " << startChan() << endl;
    currSpw()=ispw;

    // Calculate channel increment from Hz
    if (fintervalCh()<0.0 && fintervalHz()>0.0) {
      Double datawidth=abs(spwcol.chanWidth()(ispw)(IPosition(1,0)));
      cout << "ispw=" << ispw << " datawidth=" << datawidth << flush;
      fintervalCh()=floor(fintervalHz()/datawidth);
      if (fintervalCh()<1.0) fintervalCh()=1.0;
      cout << " dHz=" << fintervalHz() << " --> " << fintervalCh() << " channels." << endl;
    }

    Int extrach=nChanPar()%Int(fintervalCh());
    Int nChanOut=nChanPar()/Int(fintervalCh()) + (extrach > 0 ? 1 : 0);

    //    cout << " fintervalCh()  = " << fintervalCh() << endl;
    //    cout << " extrach        = " << extrach << endl;
    //    cout << " nChanOut       = " << nChanOut << endl;
    
    chanAveBounds_(ispw).resize(nChanOut,2);
    Matrix<Int> bounds(chanAveBounds_(ispw));
    bounds.column(0).set(startChan());
    bounds.column(1).set(startChan()+Int(fintervalCh()-1));
    for (Int ochan=1;ochan<nChanOut;++ochan) {
      Vector<Int> col(bounds.row(ochan));
      col+=Int(ochan*fintervalCh());
    }
    if (extrach>0) bounds(nChanOut-1,1)+=(extrach-Int(fintervalCh()));
    
    //    for (int ochan=0;ochan<nChanOut;++ochan) 
    //      cout << "    ochan="<<ochan<< " bounds=" 
    //	   << bounds.row(ochan) 
    //	   << " n=" << bounds(ochan,1)-bounds(ochan,0)+1
    //	   << endl;

    // Revise nChanPar()
    nChanPar()=nChanOut;
  }
  currSpw()=0;
  //  cout << "nChanParList() = " << nChanParList() << endl;
  //  cout << "chanAveBounds_ = " << chanAveBounds_ << endl;
}

// Inflate an empty Caltable w/ meta-data from a VisSet
void SolvableVisCal::inflateNCTwithMetaData(VisSet& vs) {

  if (prtlev()>3) cout << "  SVC::inflateNCTwithMetaData(vs)" << endl;

  // NB: Currently, this is only used for the accumulate
  //     context; in solve, meta-data is filled on-the-fly
  //     (this ensures more accurate timestamps, etc.)

  // Fill the Calset with meta info
  VisIter& vi(vs.iter());
  vi.originChunks();
  VisBuffer vb(vi);
  Vector<Int> islot(nSpw(),0);
  for (vi.originChunks(); vi.moreChunks(); vi.nextChunk()) {

    vi.origin();
    currSpw()=vi.spectralWindow();
    currField()=vi.fieldId();
    currScan()=vb.scan0();
    currObs()=vb.observationId()(0);
    
    // Derive average time info
    Double timeStamp(0.0);
    Int ntime(0);
    for (vi.origin(); vi.more(); vi++,++ntime) timeStamp+=vb.time()(0);
    if (ntime>0)
      refTime()=timeStamp/Double(ntime);
    else
      refTime()=0.0;

    // Initialize parameters
    switch(parType()) {
    case VisCalEnum::COMPLEX: {
      solveAllCPar().set(defaultPar());
      break;
    }
    case VisCalEnum::REAL: {
      solveAllCPar().set(defaultPar());
      break;
    }
    default:
      break;
    }
    solveAllParOK().set(true);
    solveAllParErr().set(Float(0.0));
    solveAllParSNR().set(1.0);

    /*
    cout << "Spw=" << currSpw()
	 << " Fld=" << currField()
	 << " Scan=" << currScan()
	 << " Time=" << MVTime(refTime()/C::day).string(MVTime::YMD,7)
	 << endl;
    */

    // Add this interval to the NCT
    if (refTime()>0.0)
      keepNCT();

  }

}

Bool SolvableVisCal::syncSolveMeta(VisBuffGroupAcc& vbga) {

  // Adopt meta data from FIRST CalVisBuffer in the VBGA, for now
  currSpw()=spwMap()(vbga(0).spectralWindow());
  currField()=vbga(0).fieldId();

  // The timestamp really is global, in any case
  Double& rTime(vbga.globalTimeStamp());
  if (rTime > 0.0) {
    refTime()=rTime;
    return true;
  }
  else
    return false;

}

void SolvableVisCal::syncSolveMeta(SDBList& sdbs) {  // VI2
  
  //  cout << "spwMap() = " << spwMap() << endl;

  // Ask the sdbs
  Vector<Double> freqs;
  if (freqDepPar()) 
    // nominally channelized
    freqs.reference(sdbs.freqs());
  else
    // a single aggregate frequency (as a Vector)
    freqs.reference(Vector<Double>(1,sdbs.aggregateCentroidFreq()));

  setMeta(sdbs.aggregateObsId(),
	  sdbs.aggregateScan(),
	  //sdbs.aggregateTime(),   
	  sdbs.aggregateTimeCentroid(),
	  sdbs.aggregateSpw(),
	  freqs,        
	  sdbs.aggregateFld());
}



Bool SolvableVisCal::syncSolveMeta(VisBuffer& vb, 
				   const Int&) {

  if (prtlev()>2) cout << "SVC::syncSolveMeta(,,)" << endl;

  // Returns true, only if sum of weights is positive,
  //  i.e., there is data to solve with

  // TBD: freq info, etc.

  currSpw()=spwMap()(vb.spectralWindow());
  currField()=vb.fieldId();
  currScan()=vb.scan0();
  currObs()=vb.observationId()(0);

  // Row weights as a Doubles
  Vector<Double> dWts;
  dWts.resize(vb.weight().shape());
  convertArray(dWts,vb.weight());
  Vector<Double> times;
  times = vb.time();
  
  // The following assumes flagRow is accurate
  LogicalArray gRows(!vb.flagRow());
  Double sumWts(0.0);
  MaskedArray<Double> gTimes(times,gRows);
  MaskedArray<Double> gWts(dWts,gRows);

  if (sum(gRows)>0) {
    sumWts=sum(gWts);
  }

  if (sumWts>0.0) {
    gTimes*=gWts;
    refTime()=sum(gTimes);
    refTime()/=sumWts;
    return true;
  }
  else
    return false;

}

void SolvableVisCal::overrideObsScan(Int obs,Int scan) {
  currObs()=obs;
  currScan()=scan;
}

void SolvableVisCal::enforceAPonData(VisBuffer& vb) {

  // TBD: migrate this to VisEquation?

  // ONLY if something to do
  if (apmode()=="A" || apmode()=="P") {
    Int nCorr(vb.corrType().nelements());
    Float amp(1.0);
    Complex cor(1.0);
    Bool *flR=vb.flagRow().data();
    Bool *fl =vb.flag().data();
    Vector<Float> ampCorr(nCorr);
    Vector<Int> n(nCorr,0);
    for (Int irow=0;irow<vb.nRow();++irow,++flR) {
      if (!vb.flagRow()(irow)) {
	ampCorr=0.0f;
	n=0;
	for (Int ich=0;ich<vb.nChannel();++ich,++fl) {
	  if (!vb.flag()(ich,irow)) {
	    for (Int icorr=0;icorr<nCorr;icorr++) {
	      
	      amp=abs(vb.visCube()(icorr,ich,irow));
	      if (amp>0.0f) {
		if (apmode()=="P") {
		  // we will scale by amp to make data phase-only
		  cor=Complex(amp,0.0);
		  // keep track for weight adjustment
		  ampCorr(icorr)+=abs(cor); // amp;
		  n(icorr)++;
		}
		else if (apmode()=="A")
		  // we will scale by "phase" to make data amp-only
		  cor=vb.visCube()(icorr,ich,irow)/amp;
		
		// Apply the complex scaling
		vb.visCube()(icorr,ich,irow)/=cor;
	      }
	    } // icorr
	  } // !*fl
	} // ich
	// Make appropriate weight adjustment
	//  Only for phase-only since only it rescales data
	if (apmode()=="P") {
	  for (Int icorr=0;icorr<nCorr;icorr++)
	    if (n(icorr)>0)
	      // weights adjusted by square of the mean(amp)
	      vb.weightMat()(icorr,irow)*=square(ampCorr(icorr)/Float(n(icorr)));
	    else
	      // weights now zero
	      vb.weightMat()(icorr,irow)=0.0f;
	}
      } // !*flR
    } // irow

  } // phase- or amp-only

  //  cout << "amp(vb.visCube())=" << amplitude(vb.visCube().reform(IPosition(1,vb.visCube().nelements()))) << endl;


}

void SolvableVisCal::setUpForPolSolve(VisBuffer& vb) {

  // TBD: migrate this to VisEquation?

  // NB (2016Nov29, gmoellen): No, should actually move this very specific
  //   activity into DJones where it is specifically relevant.
  //   VB2 version has done this.
  
  // Divide model and data by (scalar) stokes I (which may be resolved!), 
  //  and set model cross-hands to (1,0) so we can solve for fractional
  //  pol factors.

  // Only if solving for Q an U
  //  (leave cross-hands alone if just solving for X)
  if (solvePol()>1) {

    Int nCorr(vb.corrType().nelements());
    Bool *flR=vb.flagRow().data();
    Bool *fl =vb.flag().data();
    Vector<Float> ampCorr(nCorr);
    Vector<Int> n(nCorr,0);
    Complex sI(0.0);
    for (Int irow=0;irow<vb.nRow();++irow,++flR) {
      if (!vb.flagRow()(irow)) {
	ampCorr=0.0f;
	n=0;
	for (Int ich=0;ich<vb.nChannel();++ich,++fl) {
	  if (!vb.flag()(ich,irow)) {
	    
	    sI=(vb.modelVisCube()(0,ich,irow)+vb.modelVisCube()(3,ich,irow))/Complex(2.0);
	    if (abs(sI)>0.0) {
	      for (Int icorr=0;icorr<nCorr;icorr++) {
		vb.visCube()(icorr,ich,irow)/=sI;
		ampCorr(icorr)+=abs(sI);
		n(icorr)++;
	      } // icorr
	    }
	    else
	      vb.flag()(ich,irow)=true;
	    
	  } // !*fl
	} // ich
	// Make appropriate weight adjustment
	for (Int icorr=0;icorr<nCorr;icorr++)
	  if (n(icorr)>0)
	    // weights adjusted by square of the mean(amp)
	    vb.weightMat()(icorr,irow)*=square(ampCorr(icorr)/Float(n(icorr)));
	  else
	    // weights now zero
	    vb.weightMat()(icorr,irow)=0.0f;
      } // !*flR
    } // irow
    
    // Model is now all unity  (Is this ok for flagged data? Probably.)
    vb.modelVisCube()=Complex(1.0);

  }

}

Bool SolvableVisCal::verifyConstraints(VisBuffGroupAcc& vbag) {

  // TBD: handle multi-channel infocusFlag properly
  // TBD: optimize array access
  
  // Assemble nominal baseline weights distribution
  Matrix<Double> blwtsum(nAnt(),nAnt(),0.0);
  for (Int ivb=0;ivb<vbag.nBuf();++ivb) {
    CalVisBuffer& cvb(vbag(ivb));

    cvb.setFocusChan(focusChan());

    for (Int irow=0;irow<cvb.nRow();++irow) {
      Int& a1(cvb.antenna1()(irow));
      Int& a2(cvb.antenna2()(irow));
      if (!cvb.flagRow()(irow) && a1!=a2) {
	if (!cvb.infocusFlag()(0,irow)) {
	  Double wt=Double(sum(cvb.weightMat().column(irow)));
	  blwtsum(a2,a1)+=wt;
	  blwtsum(a1,a2)+=wt;
	} // flag
      } // flagRow 
    } // irow
  } // ivb

  // Recursively apply threshold on baselines per antenna
  //  Currently, we insist on at least 3 baselines per antenna
  //  (This will eventually be a user-specified parameter: blperant)
  Vector<Bool> antOK(nAnt(),true);  // nominally OK
  Vector<Int> blperant(nAnt(),0);
  Int iant=0;
  while (iant<nAnt()) {
    if (antOK(iant)) {   // avoid reconsidering already bad ones
      Int nbl=ntrue(blwtsum.column(iant)>0.0);
      blperant(iant)=nbl;
      if (nbl<minblperant()) {
	// some baselines available, but not enough
	//  so eliminate this antenna 
	antOK(iant)=false;
	blwtsum.row(iant)=0.0;
	blwtsum.column(iant)=0.0;
	blperant(iant)=0;
	// ensure we begin recount at first antenna again
	iant=-1;
      }
    }      
    ++iant;
  }

  //  cout << "  blperant = " << blperant << " (minblperant = " << minblperant() << endl;
  //  cout << "  antOK    = " << antOK << endl;
  //  cout << "  ntrue(antOK) = " << ntrue(antOK) << endl;

  // Apply constraints results to solutions and data
  solveParOK()=false;   // Solutions nominally bad
  for (Int iant=0;iant<nAnt();++iant) {
    if (antOK(iant)) {
      // set solution good
      solveParOK().xyPlane(iant) = true;
    }
    else {
      // This ant not ok, set soln to zero
      if (parType()==VisCalEnum::COMPLEX)
      	solveCPar().xyPlane(iant)=1.0;
      else if (parType()==VisCalEnum::REAL)
      	solveRPar().xyPlane(iant)=0.0;

      // Flag corresponding data
      for (Int ivb=0;ivb<vbag.nBuf();++ivb) {
	CalVisBuffer& cvb(vbag(ivb));
	Vector<Int>& a1(cvb.antenna1());
	Vector<Int>& a2(cvb.antenna2());
	for (Int irow=0;irow<cvb.nRow();++irow) {
	  if (a1(irow)==iant || a2(irow)==iant)
	    cvb.infocusFlag()(0,irow)=true;
	}
	// the following didn't work because row(0) behaved
	//  as contiguous and set the wrong flags for multi-chan data!
	//	cvb.infocusFlag().row(0)(a1==iant)=true;
	//	cvb.infocusFlag().row(0)(a2==iant)=true;
      } // ivb

    } // antOK
  } // iant
  
  // We return sum(antOK)>0 here because it is not how many 
  //  good ants there are, but rather how many good baselines 
  //  per ant there are.  The above counting exercise will 
  //  reduce sum(antOK) to zero when the baseline counts 
  //  constraint is violated over enough of the whole array.  
  //  so as to make the solution impossible.  Otherwise
  //  there will be at least blperant+1 (>0) good antennas.

  return (ntrue(antOK)>0);

}

Bool SolvableVisCal::verifyConstraints(SDBList& sdbs) {  // VI2

  // TBD: handle multi-channel infocusFlag properly
  // TBD: optimize array access
  
  // Assemble nominal baseline weights distribution
  Matrix<Double> blwtsum(nAnt(),nAnt(),0.0);
  for (Int isdb=0;isdb<sdbs.nSDB();++isdb) {
    SolveDataBuffer& sdb(sdbs(isdb));

    sdb.setFocusChan(focusChan());

    // TBD: do this per cal parameter, rather than just per ant

    Int nRow(sdb.nRows());
    Int nCorr(sdb.nCorrelations());

    for (Int irow=0;irow<nRow;++irow) {
      const Int& a1(sdb.antenna1()(irow));
      const Int& a2(sdb.antenna2()(irow));
      if (!sdb.flagRow()(irow) && a1!=a2) {
	// Currently insist _any_ unflagged correlations; need to refine!
	if (sum(sdb.infocusFlagCube()(Slice(),Slice(),irow))<nCorr) {
	  Double wt=Double(sum(sdb.infocusWtSpec()(Slice(),Slice(),irow)));
	  blwtsum(a2,a1)+=wt;
	  blwtsum(a1,a2)+=wt;
	} // flag
      } // flagRow 
    } // irow
  } // ivb

  // Recursively apply threshold on baselines per antenna
  Vector<Bool> antOK(nAnt(),True);  // nominally OK
  Vector<Int> blperant(nAnt(),0);
  Int iant=0;
  while (iant<nAnt()) {
    if (antOK(iant)) {   // avoid reconsidering already bad ones
      Int nbl=ntrue(blwtsum.column(iant)>0.0);
      blperant(iant)=nbl;
      if (nbl<minblperant()) {
	// some baselines available, but not enough
	//  so eliminate this antenna 
	antOK(iant)=False;
	blwtsum.row(iant)=0.0;
	blwtsum.column(iant)=0.0;
	blperant(iant)=0;
	// ensure we begin recount at first antenna again
	iant=-1;
      }
    }      
    ++iant;
  }

  //  cout << "  blperant = " << blperant << " (minblperant = " << minblperant() << endl;
  //  cout << "  antOK    = " << antOK << endl;
  //  cout << "  ntrue(antOK) = " << ntrue(antOK) << endl;

  // Apply constraints results to solutions and data
  solveParOK()=False;   // Solutions nominally bad
  for (Int iant=0;iant<nAnt();++iant) {
    if (antOK(iant)) {
      // set solution good
      solveParOK().xyPlane(iant) = True;
    }
    else {
      // This ant not ok, set soln to zero
      if (parType()==VisCalEnum::COMPLEX)
      	solveCPar().xyPlane(iant)=1.0;
      else if (parType()==VisCalEnum::REAL)
      	solveRPar().xyPlane(iant)=0.0;

      // Flag corresponding data
      for (Int isdb=0;isdb<sdbs.nSDB();++isdb) {
	SolveDataBuffer& sdb(sdbs(isdb));
	const Vector<Int>& a1(sdb.antenna1());
	const Vector<Int>& a2(sdb.antenna2());
	for (Int irow=0;irow<sdb.nRows();++irow) {
	  if (a1(irow)==iant || a2(irow)==iant)
	    sdb.infocusFlagCube()(Slice(),Slice(),irow)=True;
	}
	// the following didn't work because row(0) behaved
	//  as contiguous and set the wrong flags for multi-chan data!
	//	cvb.infocusFlag().row(0)(a1==iant)=True;
	//	cvb.infocusFlag().row(0)(a2==iant)=True;
      } // ivb

    } // antOK
  } // iant
  
  // We return sum(antOK)>0 here because it is not how many 
  //  good ants there are, but rather how many good baselines 
  //  per ant there are.  The above counting exercise will 
  //  reduce sum(antOK) to zero when the baseline counts 
  //  constraint is violated over enough of the whole array.  
  //  so as to make the solution impossible.  Otherwise
  //  there will be at least blperant+1 (>0) good antennas.

  return (ntrue(antOK)>0);

}



// Verify VisBuffer data sufficient for solving (wts, etc.)
Bool SolvableVisCal::verifyForSolve(VisBuffer& vb) {

  //  cout << "verifyForSolve..." << endl;

  Int nAntForSolveFinal(-1);
  Int nAntForSolve(0);

  // We will count baselines and weights per ant
  //   and set solveParOK accordingly
  Vector<Int> blperant(nAnt(),0);
  Vector<Double> wtperant(nAnt(),0.0);
  Vector<Bool> antOK(nAnt(),false);
    
  while (nAntForSolve!=nAntForSolveFinal) {

    nAntForSolveFinal=nAntForSolve;
    nAntForSolve=0;


    // TBD: optimize indexing with pointers in the following
    blperant=0;
    wtperant=0.0;

    for (Int irow=0;irow<vb.nRow();++irow) {
      Int a1=vb.antenna1()(irow);
      Int a2=vb.antenna2()(irow);
      if (!vb.flagRow()(irow) && a1!=a2) {
	
	if (!vb.flag()(focusChan(),irow)) {
	  
	  blperant(a1)+=1;
	  blperant(a2)+=1;
	  
	  wtperant(a1)+=Double(sum(vb.weightMat().column(irow)));
	  wtperant(a2)+=Double(sum(vb.weightMat().column(irow)));
	  
	}
      }
    }
    
    antOK=false;
    for (Int iant=0;iant<nAnt();++iant) {
      if (blperant(iant)>3 &&
	  wtperant(iant)>0.0) {
	// This antenna is good, keep it
	nAntForSolve+=1;
	antOK(iant)=true;
      }
      else {
	// This antenna under-represented; flag it
	vb.flag().row(focusChan())(vb.antenna1()==iant)=true;
	vb.flag().row(focusChan())(vb.antenna2()==iant)=true;
	//	vb.flagRow()(vb.antenna1()==iant)=true;
	//	vb.flagRow()(vb.antenna2()==iant)=true;
      }
    }

    //    cout << "blperant     = " << blperant << endl;
    //    cout << "wtperant = " << wtperant << endl;
    //    cout << "nAntForSolve = " << nAntForSolve << " " << antOK << endl;

  }

  // We've converged on the correct good antenna count
  nAntForSolveFinal=nAntForSolve;

  // Set a priori solution flags  
  solveParOK() = false;
  for (Int iant=0;iant<nAnt();++iant)
    if (antOK(iant))
      // This ant ok
      solveParOK().xyPlane(iant) = true;
    else
      // This ant not ok, set soln to zero
      if (parType()==VisCalEnum::COMPLEX)
      	solveCPar().xyPlane(iant)=1.0;
      else if (parType()==VisCalEnum::REAL)
      	solveRPar().xyPlane(iant)=0.0;
  //  cout << "antOK = " << antOK << endl;
  //  cout << "solveParOK() = " << solveParOK() << endl;
  //  cout << "amp(solveCPar()) = " << amplitude(solveCPar()) << endl;

  if (nAntForSolve<4) cout << "Only " << nAntForSolve 
			   << "/" << nAnt() 
			   << " antennas (" 
			   << floor(100*Float(nAntForSolve/nAnt()))
			   << "%) have sufficient baselines at " 
			   << MVTime(refTime()/C::day).string(MVTime::YMD,7)
			   << endl;
  return (nAntForSolve>3);
    
}

void SolvableVisCal::selfGatherAndSolve(VisSet&, VisEquation&) {
    
  if (useGenericGatherForSolve())
    throw(AipsError("Spurious call to selfGatherAndSolve() with useGenericGatherForSolve()=T."));
  else
    throw(AipsError("Attempt to call un-implemented selfGatherAndSolve()"));

}
void SolvableVisCal::selfSolveOne(VisBuffGroupAcc&) {
    
  if (useGenericSolveOne())
    throw(AipsError("Spurious call to selfSolveOne() with useGenericSolveOne()=T."));
  else
    throw(AipsError("Attempt to call un-implemented selfSolveOne()"));

}

     
void SolvableVisCal::updatePar(const Vector<Complex> dCalPar,const Vector<Complex> dSrcPar) {

  AlwaysAssert((solveCPar().nelements()==dCalPar.nelements()),AipsError);

  Cube<Complex> dparcube(dCalPar.reform(solveCPar().shape()));

  // zero flagged increments
  dparcube(!solveParOK())=Complex(0.0);
  
  // Add the increment
  solveCPar()+=dparcube;

  // Update source params
  if (solvePol()) {
    srcPolPar()+=dSrcPar;
    //    cout << "Updated Q,U = " << real(srcPolPar()) << endl;
  }

  // The matrices are nominally out-of-sync now
  invalidateCalMat();

  // Ensure phaseonly-ness, if necessary
  //  if (apmode()=='P') {   
  //  NB: Disable this, for the moment (07May24); testing a fix for
  //      a problem Kumar noticed.  See VC::makeSolnPhaseOnly(), etc.
  if (false) {
    Float amp(0.0);
    for (Int iant=0;iant<nAnt();++iant) {
      for (Int ipar=0;ipar<nPar();++ipar) {
	if (solveParOK()(ipar,0,iant)) {
	  amp=abs(solveCPar()(ipar,0,iant));
	  if (amp>0.0)
	    solveCPar()(ipar,0,iant)/=amp;
	}
      }
    }
  }
}

void SolvableVisCal::updatePar(const Vector<Complex> dPar) { // VI2

  AlwaysAssert((solveCPar().nelements()==dPar.nelements()),AipsError);

  Cube<Complex> dparcube(dPar.reform(solveCPar().shape()));

  // zero flagged increments
  dparcube(!solveParOK())=Complex(0.0);
  
  // Add the increment
  solveCPar()+=dparcube;

  // The matrices are nominally out-of-sync now
  invalidateCalMat();

  // Ensure phaseonly-ness, if necessary
  //  if (apmode()=='P') {   
  //  NB: Disable this, for the moment (07May24); testing a fix for
  //      a problem Kumar noticed.  See VC::makeSolnPhaseOnly(), etc.
  if (False) {
    Float amp(0.0);
    for (Int iant=0;iant<nAnt();++iant) {
      for (Int ipar=0;ipar<nPar();++ipar) {
	if (solveParOK()(ipar,0,iant)) {
	  amp=abs(solveCPar()(ipar,0,iant));
	  if (amp>0.0)
	    solveCPar()(ipar,0,iant)/=amp;
	}
      }
    }
  }
}


void SolvableVisCal::formSolveSNR() {

  // Nominally zero
  solveParSNR()=0.0;

  for (Int iant=0;iant<nAnt();++iant) {
    for (Int ipar=0;ipar<nPar();++ipar) {
      if (solveParOK()(ipar,0,iant)) {
	if (solveParErr()(ipar,0,iant)>0.0f) 
	  solveParSNR()(ipar,0,iant)=abs(solveCPar()(ipar,0,iant))/solveParErr()(ipar,0,iant);
	else 
	  // if error is zero, SNR is officially infinite; use a (large) special value here
	  solveParSNR()(ipar,0,iant)=9999999.0;
      } // ok
    } // ipar
  } // iant

}

void SolvableVisCal::applySNRThreshold() {

  Int nOk1(ntrue(solveParOK()));
  
  for (Int iant=0;iant<nAnt();++iant)
    for (Int ipar=0;ipar<nPar();++ipar)
      if (solveParOK()(ipar,0,iant))
	solveParOK()(ipar,0,iant)=(solveParSNR()(ipar,0,iant)>minSNR());
  
  Int nOk2(ntrue(solveParOK()));
  Int nFail=nOk1-nOk2;    
  
  if (false) {
    // Report some stuff re SNR
    cout << endl 
	 << "Time = " << MVTime(refTime()/C::day).string(MVTime::YMD,7) << endl;
    cout << "SNR      = " << solveParSNR() << endl;
    cout << nOk1 << " " << nOk2 << " " << nFail << endl;
    Float meansnr(0.0f);
    if (ntrue(solveParOK())>0) {
      meansnr=mean(solveParSNR()(solveParOK()));
      cout << "mean solution SNR = " << meansnr
	   << " (passing threshold)."
	   << endl;
    }
  }
  
  if (nFail>0) {
    cout << nFail << " of " << nOk1
	 << " solutions flagged due to SNR < " << minSNR() 
	 << " in spw=" << currSpw();
    // if multi-chan, report channel
    if (freqDepPar())
      cout << " (chan="<<focusChan()<<")";

    cout << " at " << MVTime(refTime()/C::day).string(MVTime::YMD,7)
	 << endl;
  }

}

// Return the cal flag record, with tableName included
Record SolvableVisCal::actionRec() {
  Record cf;
  cf.define("table",calTableName());
  cf.merge(VisCal::actionRec());
  return cf;
}

Record SolvableVisCal::solveActionRec() {

  // Return empty record
  //  TBD: consider returning various _generic_ info
  //  NB: specialization may add particulars via merge
  Record r;
  return r;
}





void SolvableVisCal::smooth(Vector<Int>& fields,
			    const String& smtype,
			    const Double& smtime) {

  if (smoothable()) 
    // Call NewCalTable's global smooth method
    casa::smoothCT(*ct_,smtype,smtime,fields);
  else
    throw(AipsError("This type "+this->typeName()+" does not support smoothing!"));

}

void SolvableVisCal::syncSolveCal() {

  if (prtlev()>4) cout << "    SVC::syncSolveCal()" << endl;
  
  // Ensure parameters are ready
  syncSolvePar();

  if (!PValid())
    throw(AipsError("No valid parameters in syncSolveCal"));

  // Sync up matrices using current params
  syncCalMat(false);    // NEVER invert in solve context
  syncDiffMat();

}
void SolvableVisCal::syncSolvePar() {

  if (prtlev()>5) cout << "      SVC::syncSolvePar()" << endl;

  // In solve context, reference solveCPar(), etc.
  AlwaysAssert((solveCPar().nelements()>0 || solveRPar().nelements()>0),AipsError);
  currCPar().reference(solveCPar());
  currRPar().reference(solveRPar());
  currParOK().reference(solveParOK());
  validateP();

}

void SolvableVisCal::calcPar() {

  if (simOnTheFly() and isSimulated()) {
    if (prtlev()>3) cout << "SVC:calcPar triggered simOTF with isSolved=" << isSolved()
	 << " isApplied=" << isApplied() << " isSimulated=" << isSimulated() << endl;
    syncSolvePar(); // OTF simulation context RI 20100831    
  } else { 

  if (prtlev()>6) cout << "      SVC::calcPar()" << endl;

  // This method is relevant only to the apply (& simulate) context

  // If we have a CLPatchPanel, use it
  if (cpp_)
    this->calcParByCLPP();
  else { // use CTPatchedInterp

  Bool newcal(false);

  // Interpolate solution   (CTPatchedInterp)
  if (freqDepPar()) {

    //cout << "currFreq() = " << currFreq().shape() << " " << currFreq() << endl;

    // Call w/ freq-dep
    if (fInterpType().contains("rel")) {
      // Relative freq
      Double freqOff(msmc().centerFreq(currSpw()));
      Double SBfactor(1.0);
      if (currFreq().nelements()>1 && currFreq()(0)>currFreq()(1))
	SBfactor=-1.0f;
      //cout << "freqOff=" << freqOff << " SBfactor=" << SBfactor << " netSB=" << msmc().msmd().getNetSidebands()[currSpw()] << endl;
      newcal=ci_->interpolate(currObs(),currField(),currSpw(),currTime(),(currFreq()-freqOff)*SBfactor);
    }
    else
      // absolute freq
      newcal=ci_->interpolate(currObs(),currField(),currSpw(),currTime(),currFreq());

    //    cout.precision(12);
    //    cout << typeName() << " t="<< currTime() << " newcal=" << boolalpha << newcal << endl;
  }
  else {
    if (parType()==VisCalEnum::COMPLEX)
      // Call w/ fiducial freq for phase-delay correction
      // TBD: improve freq spec, e.g., use spw center freq rather than _selected_ center
      newcal=ci_->interpolate(currObs(),currField(),currSpw(),currTime(),1.0e9*currFreq()(currFreq().nelements()/2));
    else
      // No freq info at all
      newcal=ci_->interpolate(currObs(),currField(),currSpw(),currTime());
  }

  // TBD: signal failure to find calibration??  (e.g., for a spw?)

  // Parameters now (or still) valid (independent of newcal!!)
  validateP();

  // If new cal parameters, use them 
  if (newcal) {

    //    cout << "Found new cal!" << endl;
    
    // Reference result
    if (parType()==VisCalEnum::COMPLEX) 
      currCPar().reference(ci_->resultC(currObs(),currField(),currSpw()));
    else if (parType()==VisCalEnum::REAL) 
      currRPar().reference(ci_->resultF(currObs(),currField(),currSpw()));
    else
      throw(AipsError("Bad parType() in SVC::calcPar"));

    // Assign _inverse_ of parameter flags
    currParOK().reference(!ci_->rflag(currObs(),currField(),currSpw()));

    // Ensure shapes recorded correctly
    // (New interpolation generates cal samples for all data channels...revisit?
    IPosition ip=currCPar().shape();
    nChanPar()=ip(1);
    startChan()=0;

    // In case we need solution timestamp
    // NB: w/ new caltables, we use currTime() here.
    refTime() = currTime();

    // If new parameters, matrices (generically) are necessarily invalid now
    invalidateCalMat();
  }
  }
  }

}

void SolvableVisCal::calcParByCLPP() {

  Bool newcal(false);
  Cube<Bool> resFlag;

  // Interpolate solution   
  switch (parType()) {
  case VisCalEnum::COMPLEX: {

    if (freqDepPar()) {
      // Call w/ freq-dep
      newcal=cpp_->interpolate(currCPar(),resFlag,currObs(),currField(),currIntent(),currSpw(),currTime(),currFreq());
    }
    else {
      // Call w/ fiducial freq for phase-delay correction
      Double freq=1.0e9*currFreq()(currFreq().nelements()/2);
      newcal=cpp_->interpolate(currCPar(),resFlag,currObs(),currField(),currIntent(),currSpw(),currTime(),freq);
    }
    break;
  }
  case VisCalEnum::REAL: {
    // Interpolate solution   
    if (freqDepPar()) {
      // Call w/ freq-dep
      newcal=cpp_->interpolate(currRPar(),resFlag,currObs(),currField(),currIntent(),currSpw(),currTime(),currFreq());
    }
    else {
      newcal=cpp_->interpolate(currRPar(),resFlag,currObs(),currField(),currIntent(),currSpw(),currTime(),-1.0);
    }
    break;
  }
  default:
    throw(AipsError("Unhandled parType()")); // users should never see this
    break;
  }

  // TBD: signal failure to find calibration??  (e.g., for a spw?)

  // Parameters now (or still) valid (independent of newcal!!)
  validateP();

  if (newcal) {
 
    // Assign _inverse_ of parameter flags
    currParOK().reference(!resFlag);

    // Ensure shapes recorded correctly
    // (New interpolation generates cal samples for all data channels...revisit?
    //  IS THIS NEEDED?
    IPosition ip=currCPar().shape();
    nChanPar()=ip(1);
    startChan()=0;

    // In case we need solution timestamp
    // NB: w/ new caltables, we use currTime() here.
    refTime() = currTime();

    // If new parameters, matrices (generically) are necessarily invalid now
    invalidateCalMat();
  }

}


// Report solved-for QU
void SolvableVisCal::reportSolvedQU() {

  String fldname(msmc().fieldName(currField()));

  if (solvePol()==2) {
    Float Q=real(srcPolPar()(0));
    Float U=real(srcPolPar()(1));
    Float P=sqrt(Q*Q + U*U);
    Float X=atan2(U,Q)/2.0*180.0/C::pi;

    logSink() << "Fractional polarization solution for " << fldname
	      << " (spw = " << currSpw() << "): "
	      << ": Q = " << Q
	      << ", U = " << U
	      << "  (P = " << P
	      << ", X = " << X << " deg)"
	      << LogIO::POST;
  }
  else if (solvePol()==1) {
    logSink() << "Position angle offset solution for " << fldname
	      << " (spw = " << currSpw() << ") = "
	      << real(srcPolPar()(0))*180.0/C::pi/2.0
	      << " deg."
	      << LogIO::POST;
  }
}

void SolvableVisCal::createMemCalTable() {

  //  cout << "createMemCalTable" << endl;

  // Set up description
  String partype = ((parType()==VisCalEnum::COMPLEX) ? "Complex" : "Float");

  if (msName()!="<noms>") {
    CTDesc caltabdesc(partype,Path(msName()).baseName(),typeName(),"unknown");
    ct_ = new NewCalTable("tempNCT.tab",caltabdesc,Table::Scratch,Table::Memory)
;
    ct_->setMetaInfo(msName());
  }
  else {
    ct_ = new NewCalTable("tempNCT.tab",partype,typeName(),msmc().ssp(),
			  false,true);
  }

  CTColumns ncc(*ct_);

  // Revise SPW frequencies
  for (Int ispw=0;ispw<nSpw();++ispw) {

    currSpw()=ispw;

    // MS freqs
    Vector<Double> chfr,chwid,chres,cheff;
    ncc.spectralWindow().chanFreq().get(ispw,chfr);
    ncc.spectralWindow().chanWidth().get(ispw,chwid);
    ncc.spectralWindow().resolution().get(ispw,chres);
    ncc.spectralWindow().effectiveBW().get(ispw,cheff);

    Int nchan=0;
    Vector<Double> calfreq,calwid,calres,caleff;
    if (startChan()>-1 && nChanPar()>0) {

      if (freqDepPar()) { 
	if (fsolint()!="none") {
	  IPosition blc(1,0),trc(1,0);
	  Matrix<Int> bounds(chanAveBounds());
	  nchan=bounds.nrow();
	  calfreq.resize(nchan);
	  calwid.resize(nchan);
	  calres.resize(nchan);
	  caleff.resize(nchan);
	  cout.precision(12);
	  for (Int ochan=0;ochan<nchan;++ochan) {
	    blc(0)=bounds(ochan,0);
	    trc(0)=bounds(ochan,1);
	    calfreq(ochan)=mean(chfr(blc,trc));
	    calwid(ochan)=sum(chwid(blc,trc));
	    calres(ochan)=sum(chres(blc,trc));
	    caleff(ochan)=sum(cheff(blc,trc));
	  }
	}
	else {
	  nchan=nChanPar();
	  if (nChanPar()<Int(chfr.nelements())) {
	    calfreq=chfr(Slice(startChan(),nChanPar()));
	    calwid=chwid(Slice(startChan(),nChanPar()));
	    calres=chres(Slice(startChan(),nChanPar()));
	    caleff=cheff(Slice(startChan(),nChanPar()));
	  }
	  else {
	    calfreq.reference(chfr);
	    calwid.reference(chwid);
	    calres.reference(chres);
	    caleff.reference(cheff);
	  }
	}
      }
      else {
	nchan=1;
	calfreq.resize(1);
	calwid.resize(1);
	calres.resize(1);
	caleff.resize(1);
	// Use simple mean freq, and aggregate bandwidth
	calfreq(0)=mean(chfr);
	calwid(0)=sum(chwid);
	calres(0)=sum(chres);
	caleff(0)=sum(cheff);
      }
    }
    //    cout << "nchan=" << nchan << " calfreq.nelements() = " << calfreq.nelements() << " " << calfreq << endl;
    if (nchan>0) {
      ncc.spectralWindow().chanFreq().setShape(ispw,IPosition(1,nchan));
      //      cout << "ncc.spectralWindow().chanFreq().shape(ispw)         = " << ncc.spectralWindow().chanFreq().shape(ispw) << endl;
      ncc.spectralWindow().chanFreq().put(ispw,calfreq);
      ncc.spectralWindow().chanWidth().put(ispw,calwid);
      ncc.spectralWindow().resolution().put(ispw,calres);  // same as chanWidth, for now
      ncc.spectralWindow().effectiveBW().put(ispw,caleff);  // same as chanWidth, for now
      ncc.spectralWindow().numChan().put(ispw,nchan);
      ncc.spectralWindow().totalBandwidth().put(ispw,abs(sum(calwid)));
    }
    else
      ncc.spectralWindow().flagRow().put(ispw,true);
  }

  // When combining in spw, further revise freq info to average over combined spws
  //  Only for freqDepPar()=false types, for now (nothing bandpass-like)
  //  NB: Currently, this will only handle a single aggregation..., e.g., not CAS-5687
  if (combspw() && !freqDepPar()) {

    try {

    Matrix<Double> chanFreq,chanWidth,effectiveBW;
    ncc.spectralWindow().chanFreq().getColumn(chanFreq,true);
    ncc.spectralWindow().chanWidth().getColumn(chanWidth,true);
    ncc.spectralWindow().effectiveBW().getColumn(effectiveBW,true);

    // Insist on a single channel!   (NO BANDPASS-LIKE TYPES!)
    Int nChan=chanFreq.shape()[0];
    AlwaysAssert(nChan==1,AipsError);

    Vector<Bool> spwmask=(spwMap()>-1);
    Int outSpw=Vector<Int>(spwMap()(spwmask).getCompressedArray())[0];

    // If only one chan, then the chanWidth should span the the whole spw range
    //  NB: this is done before the chanFreq are revised, below
    //  NB: Disabled for now, since centroid freq +/- wid/2 doesn't actually
    //      cover the input spw range, in general....more thought necessary here
    //      So keep the single spw width, for now, for which there remains
    //      a reasonable argument (the phase correction is ~appropriate over this
    //      limited range without further refinement...)  (the width isn't used
    //      decisively anywhere yet, in any case)
    if (false) {
      
      Vector<Double> f(chanFreq.row(0));
      Vector<Double> chW(chanWidth.row(0));
      Vector<Double> flo=f-chW/2.0;
      Vector<Double> fhi=f+chW/2.0;

      if (allGT(fhi,flo))
	// USB  (positive result)
	chW(outSpw)=max(fhi(spwmask))-min(flo(spwmask));
      else
	// LSB  (negative result)
	chW(outSpw)=min(fhi(spwmask))-max(flo(spwmask));

    }

    // Assumes single channel!
    Vector<Double> f(chanFreq.row(0));
    Vector<Double> ebw(effectiveBW.row(0));
    Double meanfreq=sum(f(spwmask)*ebw(spwmask));
    Double sumebw=sum(ebw(spwmask));
    meanfreq/=sumebw;
    
    f(outSpw)=meanfreq;
    ebw(outSpw)=sumebw;

    /*
    cout << "spwmask = " << boolalpha << spwmask << endl;
    cout << "chanFreq(:,outSpw)    = " << chanFreq.column(outSpw) << endl;
    cout << "chanWidth(:,outSpw)   = " << chanWidth.column(outSpw) << endl;
    cout << "effectiveBW(:,outSpw) = " << effectiveBW.column(outSpw) << endl;
    */


    ncc.spectralWindow().chanFreq().putColumn(chanFreq);
    ncc.spectralWindow().chanWidth().putColumn(chanWidth);
    ncc.spectralWindow().effectiveBW().putColumn(effectiveBW);
    }
    catch (...) {
      throw(AipsError("Error calculating solution frequencies w/ combine='spw'"));
    }
  }

}

// File a single-channel solved solution into the multi-channel space for it
void SolvableVisCal::keep1(Int ichan) {
  if (parType()==VisCalEnum::COMPLEX)
    solveAllCPar()(Slice(),Slice(ichan,1),Slice())=solveCPar();
  else if (parType()==VisCalEnum::REAL)
    solveAllRPar()(Slice(),Slice(ichan,1),Slice())=solveRPar();

  solveAllParOK()(Slice(),Slice(ichan,1),Slice())=solveParOK();
  solveAllParErr()(Slice(),Slice(ichan,1),Slice())=solveParErr();
  solveAllParSNR()(Slice(),Slice(ichan,1),Slice())=solveParSNR();
}

Bool SolvableVisCal::spwOK(Int ispw) {

  if (isSolved())
    return spwOK_(ispw);

  if (isApplied() && ci_)
    // Get it from the interpolator (w/ spwmap)
    return spwOK_(ispw)=ci_->spwOK(ispw);
  else {
    // Assume ok (e.g., non-ci_ types like TOpac, GainCurve)
    // TBD: be more careful by specializing this method
    return true;
  }
}

// Is the data in this VB calibrate-able?  This replaces the old spwOK and
//   is more specific as regards obs, fld, intent.  Used at wide scopes
//   (Calibrater/VisEquation) to control entering expensive loops.
// "OK for Cal" or "Calibrate-able" here means:
//   a) this SVC can actually calibrate the data within the VB because
//       there are solutions explicitly available to do it (see
//       SVC::calAvailable(vb) below)
// OR
//   b) that this SVC is agnostic about the data within the VB according
//       to arrangements made by CalLibrary specifictions and
//       embodied within the CLPatchPanel (cpp_).  Agnosticism is
//       supported ONLY when using the CalLibrary and CLPatchPanel
Bool SolvableVisCal::VBOKforCalApply(vi::VisBuffer2& vb) {

  // Current spw
  //  TBD: Use syncMeta2?
  const Int& ispw(vb.spectralWindows()(0));

  if (isSolved())
    return spwOK_(ispw);

  if (isApplied()) {

    if (ci_)
      // Get it from the old-fashioned CTPatchedInterp
      return spwOK_(ispw)=ci_->spwOK(ispw);

    else if (cpp_) {
      // Get it from the new CLPatchPanel, which has
      //  obs, fld, intent specificity, too!
      const Int& iobs(vb.observationId()(0));
      const Int& ifld(vb.fieldId()(0));
      const Int& ient(vb.stateId()(0));
      return cpp_->MSIndicesOK(iobs,ifld,ient,ispw,-1); // all ants
    }
    else
      // Assume ok for non-interpolable types
      return true;
  }

  // Shouldn't reach here, assume true (could trigger failure elsewhere)
  return true;

}

// Is calibration for the specified VB actually available?
//  This returns true when condition "a" described above
//  is true.  If the CLPatchPanel is agnostic, this returns
//  false.  This is used by VisCal::correct2 control whether
//  calibration update and algebraic apply can/should be done. 
//  In the CalLibrary context, this enables agnosticism (when
//  false)
//  The implementation here is almost the same as VBOKforCal,
//   differing only in the call to cpp_->calAvailable.
Bool SolvableVisCal::calAvailable(vi::VisBuffer2& vb) {

  // Current spw
  //  TBD: Use syncMeta2?
  const Int& ispw(vb.spectralWindows()(0));

  if (isSolved())
    return spwOK_(ispw);

  if (isApplied()) {

    if (ci_)
      // Get it from the old-fashioned CTPatchedInterp
      return spwOK_(ispw)=ci_->spwOK(ispw);

    else if (cpp_) {
      // Get it from the new CLPatchPanel, which has
      //  obs, fld, intent specificity, too!
      const Int& iobs(vb.observationId()(0));
      const Int& ifld(vb.fieldId()(0));
      const Int& ient(vb.stateId()(0));
      return cpp_->calAvailable(iobs,ifld,ient,ispw,-1); // all ants
    }
    else
      // Assume ok for non-interpolable types
      return true;
  }

  // Shouldn't reach here, assume true (could trigger failure elsewhere)
  return true;

}



// File a solved solution (and meta-data) into the in-memory Caltable
void SolvableVisCal::keepNCT() {

  // TBD: set CalTable freq info here
  //  if (!spwOK(currSpw()))
  //    setSpwFreqInCT(currSpw(),currFreq());

  if (prtlev()>4) 
    cout << " SVC::keepNCT" << endl;    

  // Add some rows to fill 
  //  nElem() gets it right for both baseline- and antenna-based
  ct_->addRow(nElem());

  CTMainColumns ncmc(*ct_);

  // We are adding to the most-recently added rows
  RefRows rows(ct_->nrow()-nElem(),ct_->nrow()-1,1); 

  // Meta-info
  ncmc.time().putColumnCells(rows,Vector<Double>(nElem(),refTime()));
  ncmc.fieldId().putColumnCells(rows,Vector<Int>(nElem(),currField()));
  ncmc.spwId().putColumnCells(rows,Vector<Int>(nElem(),currSpw()));
  ncmc.scanNo().putColumnCells(rows,Vector<Int>(nElem(),currScan()));
  ncmc.obsId().putColumnCells(rows,Vector<Int>(nElem(),currObs()));
  ncmc.interval().putColumnCells(rows,Vector<Double>(nElem(),0.0));

  // Params
  if (parType()==VisCalEnum::COMPLEX)
    // Fill Complex column
    ncmc.cparam().putColumnCells(rows,solveAllCPar());
  else if (parType()==VisCalEnum::REAL)
    // Fill Float column
    ncmc.fparam().putColumnCells(rows,solveAllRPar());

  // Stats
  ncmc.paramerr().putColumnCells(rows,solveAllParErr());
  ncmc.snr().putColumnCells(rows,solveAllParSNR());
  ncmc.flag().putColumnCells(rows,!solveAllParOK());

  // This spw now has some solutions in it
  spwOK_(currSpw())=true;

}

void SolvableVisCal::globalPostSolveTinker() {

  // Make solutions phase- or amp-only, if required
  if (apmode()!="AP") enforceAPonSoln();
  
  // Apply normalization
  if (solnorm()) normalize();

}

// Divide all solutions by their amplitudes to make them "phase-only"
void SolvableVisCal::enforceAPonSoln() {

  // VI2: review initializatin of apmode!  (and similar!)
  if (apmode()=="") return;

  // Only if we have a CalTable, and it is not empty
  if (ct_ && ct_->nrow()>0) {

    // TBD: trap attempts to enforceAPonSoln a caltable containing FPARAM (non-Complex)?

    logSink() << "Enforcing apmode on solutions." 
	      << LogIO::POST;

    // In this generic version, one normalization factor per spw
    Block<String> col(3);
    col[0]="SPECTRAL_WINDOW_ID";
    col[1]="TIME";
    col[2]="ANTENNA1";
    CTIter ctiter(*ct_,col);

    while (!ctiter.pastEnd()) {

      Array<Complex> par(ctiter.cparam());
      Array<Bool> fl(ctiter.flag());
      Array<Float> amps(amplitude(par));
      par(amps==0.0f)=Complex(1.0);
      par(fl)=Complex(1.0);
      amps(amps==0.0f)=1.0f;
      amps(fl)=1.0f;

      Array<Complex> cor(amps.shape());
      if (apmode()=='P')
	// we will scale solns by amp to make them phase-only
	convertArray(cor,amps);
      else if (apmode()=='A') {
	// we will scale solns by "phase" to make them amp-only
	cor=par;
	cor/=amps;
      }
      
      if (ntrue(amplitude(cor)==0.0f)==0)
	par/=cor;
      else
	throw(AipsError("enforceAPonSoln divide-by-zero error."));
      
      // put result back
      ctiter.setcparam(par);

      // advance iterator
      ctiter.next();
    }


  }
  else 
    throw(AipsError("Solution apmode enforcement not supported."));

}

void SolvableVisCal::normalize() {

  // Only if we have a CalTable, and it is not empty
  if (ct_ && ct_->nrow()>0) {

    // TBD: trap attempts to normalize a caltable containing FPARAM (non-Complex)?

    logSink() << "Normalizing solution amplitudes per spw with " << solNorm().normtypeString()
	      << LogIO::POST;

    // In this generic version, one normalization factor per spw
    Block<String> col(1);
    col[0]="SPECTRAL_WINDOW_ID";
    CTIter ctiter(*ct_,col);

    while (!ctiter.pastEnd()) {
      Cube<Complex> p(ctiter.cparam());
      Cube<Bool> fl(ctiter.flag());
      if (nfalse(fl)>0) {
	Complex normfactor=normSolnArray(p,!fl,false);  // don't do phase
	logSink() << " Normalization factor (" << solNorm().normtypeString() << ") for spw " << ctiter.thisSpw() << " = " << abs(normfactor)
	      << LogIO::POST;

	// record result...
	ctiter.setcparam(p);
      }
      ctiter.next();
    }

  }
}

void SolvableVisCal::storeNCT() {

  if (prtlev()>3) cout << " SVC::storeNCT()" << endl;

  // Escape from attempt to write to an empty name,
  //  because this may delete more than one wants
  if (calTableName().empty())
    throw(AipsError("Empty string provided for caltable name; this is not allowed."));

  // Flag spws that didn't occur (should do for other meta-info!)
  ct_->flagAbsentSpws();

  // If append=T and the specified table exists...
  if (append() && Table::isReadable(calTableName())) {

    // Verify the same type
    verifyCalTable(calTableName());
    
    logSink() << "Appending solutions to table: " << calTableName()
	      << LogIO::POST;
    
    // Keep the new in-memory caltable locally
    NewCalTable *newct=ct_;
    ct_=NULL;

    // Load the existing table (ct_)
    loadMemCalTable(calTableName());
    
    // Verify that both caltables come from the same MS
    try {
      String msn0=ct_->keywordSet().asString("MSName");
      String msn1=newct->keywordSet().asString("MSName");
      AlwaysAssert( msn0==msn1, AipsError);
    }
    catch ( AipsError err ) {
      delete newct;  // ct_ will be deleted in dtor
      throw(AipsError("Cannot append solutions from different MS."));
    }

    // Merge spw info (carefully) from newct to ct_
    try {
      ct_->mergeSpwMetaInfo(*newct);
    }
    catch ( AipsError err ) {
      logSink() << err.getMesg() << LogIO::SEVERE;
      throw(AipsError("Error attempting append=T"));
    }
    
    // copy the new onto the existing...
    TableCopy::copyRows(*ct_,*newct,ct_->nrow(),0,newct->nrow());
    
    // Delete the local pointer to the new solutions
    //  NB: ct_ will be deleted by dtor (after writeToDisk below)
    delete newct;
   
    // At this point, ct_ contains old and new solutions in memory

  }
  else
    logSink() << "Writing solutions to table: " << calTableName()
	      << LogIO::POST;
  
  // Write out the table to disk (regardless of what happened above)
  //  (this will sort)
  ct_->writeToDisk(calTableName());

}

void SolvableVisCal::storeNCT(const String& table,const Bool& append) {

  if (prtlev()>3) cout << " SVC::store(table,append)" << endl;

  // Override tablename
  calTableName()=table;
  SolvableVisCal::append()=append;

  // Call conventional store
  storeNCT();

}

void SolvableVisCal::loadMemCalTable(String ctname,String field) {
  if (field.length()>0) {
    // Open whole table (on disk);
    NewCalTable wholect(NewCalTable::createCT(ctname,Table::Old,Table::Memory)); 
    ct_ = new NewCalTable(wholect);  // Make sure ct_ contains a real object

    // Prepare to select on it
    CTInterface cti(wholect);
    MSSelection mss;
    //    mss.reset(cti,MSSelection::PARSE_LATE,"","",field);
    mss.setFieldExpr(field);
    TableExprNode ten=mss.toTableExprNode(&cti);
    //cout << "Selected field list: " << mss.getFieldList() << endl;

    // Apply selection to table
    try {
      getSelectedTable(*ct_,wholect,ten,"");
    } catch (AipsError x) {
      logSink() << x.getMesg() << LogIO::SEVERE;
      throw(AipsError("Error selecting on caltable: "+ctname+"... "));
    }

  }
  else
    // No selection
    ct_ = new NewCalTable(NewCalTable::createCT(ctname,Table::Old,Table::Memory)); 



  // Fill nChanParList from the Caltable
  //   (this may be revised by calcPar)

  // This should not be needed anymore (and it breaks portability)
  //  MSSpWindowColumns spwcol(ct_->spectralWindow());
  //  nChanParList().assign(spwcol.numChan().getColumn());

}

void SolvableVisCal::stateSVC(const Bool& doVC) {
  
  // If requested, report VisCal state
  if (doVC) VisCal::state();

  if (prtlev()>3) cout << "SVC::stateSVC():" << endl;
  cout << boolalpha;

  // Now SVC-specific stuff:
  cout << "  isSolved() = " << isSolved() << endl;
  cout << "  calTableName() = " << calTableName() << endl;
  cout << "  calTableSelect() = " << calTableSelect() << endl;
  cout << "  apmode() = " << apmode() << endl;
  cout << "  phandonly() = " << phandonly() << endl;
  cout << "  tInterpType() = " << tInterpType() << endl;
  cout << "  fInterpType() = " << fInterpType() << endl;
  cout << "  spwMap() = " << spwMap() << endl;
  cout << "  refantmode() = " << refantmode() << endl;
  cout << "  refant() = " << refant() << endl;
  cout << "  refantlist() = " << refantlist() << endl;
  cout << "  solmode = " << solmode() << endl;
  cout << "  rmsthresh = " << rmsthresh() << endl;

  cout << "  solveCPar().shape()   = " << solveCPar().shape() 
       << " (" << solveCPar().data() << ")" << endl;
  cout << "  solveRPar().shape()   = " << solveRPar().shape() 
       << " (" << solveRPar().data() << ")" << endl;
  cout << "  solveParOK().shape() = " << solveParOK().shape()
       << " (" << solveParOK().data() << ") "
       << " (ntrue=" << ntrue(solveParOK()) << ")" << endl;

  cout << "- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -" << endl;

}

Complex SolvableVisCal::normSolnArray(Array<Complex>& sol, 
				      const Array<Bool>& solOK,
				      const Bool doPhase) {

  // Only do something if 2 or more good solutions
  Complex factor(1.0);
  if (ntrue(solOK)>1) {

    
    Array<Float> amp(amplitude(sol));

    // If desired, determine phase part of the normalization
    if (doPhase) {
      // Prepare to divide by amplitudes indiscriminately
      amp(!solOK)=1.0f;
      Array<Complex> sol1=sol/amp;
      sol1(!solOK)=Complex(0.0);
      factor=sum(sol1);
      factor/=abs(factor);
    }

    // Determine amplitude normalization factor
    factor*=calcPowerNorm(amp,solOK);
    
    // Apply the normalization factor, if non-zero
    if (abs(factor) > 0.0)
      sol/=factor;
    
  } // ntrue > 0

  // Return the net normlization factor
  return factor;
  
}



void SolvableVisCal::currMetaNote() {

  cout << "   ("
       << "time=" << MVTime(refTime()/C::day).string(MVTime::YMD,7)
       << " field=" << currField()
       << " spw=" << currSpw()
       << " chan=" << focusChan()
       << ")"
       << endl;

}

void SolvableVisCal::initSVC() {

  if (prtlev()>2) cout << " SVC::initSVC()" << endl;

  for (Int ispw=0;ispw<nSpw(); ispw++) {

    // TBD: Would like to make this parType()-dependent,
    //  but parType() isn't initialized yet...
  
    //    if (parType()==VisCalEnum::COMPLEX) {
      solveCPar_[ispw] = new Cube<Complex>();
      solveAllCPar_[ispw] = new Cube<Complex>();
    //    }
    //   else if (parType()==VisCalEnum::REAL) {
      solveRPar_[ispw] = new Cube<Float>();
      solveAllRPar_[ispw] = new Cube<Float>();
    //    }
    solveParOK_[ispw] = new Cube<Bool>();
    solveParErr_[ispw] = new Cube<Float>();
    solveParSNR_[ispw] = new Cube<Float>();
    solveAllParOK_[ispw] = new Cube<Bool>();
    solveAllParErr_[ispw] = new Cube<Float>();
    solveAllParSNR_[ispw] = new Cube<Float>();
  }
  
  parType_=VisCalEnum::COMPLEX;

}

void SolvableVisCal::deleteSVC() {

  if (prtlev()>2) cout << " SVC::deleteSVC()" << endl;

  for (Int ispw=0; ispw<nSpw(); ispw++) {
    if (solveCPar_[ispw])  delete solveCPar_[ispw];
    if (solveRPar_[ispw])  delete solveRPar_[ispw];
    if (solveParOK_[ispw]) delete solveParOK_[ispw];
    if (solveParErr_[ispw]) delete solveParErr_[ispw];
    if (solveParSNR_[ispw]) delete solveParSNR_[ispw];
    if (solveAllCPar_[ispw])  delete solveAllCPar_[ispw];
    if (solveAllRPar_[ispw])  delete solveAllRPar_[ispw];
    if (solveAllParOK_[ispw]) delete solveAllParOK_[ispw];
    if (solveAllParErr_[ispw]) delete solveAllParErr_[ispw];
    if (solveAllParSNR_[ispw]) delete solveAllParSNR_[ispw];
  }
  solveCPar_=NULL;
  solveRPar_=NULL;
  solveParOK_=NULL;
  solveParErr_=NULL;
  solveParSNR_=NULL;
  solveAllCPar_=NULL;
  solveAllRPar_=NULL;
  solveAllParOK_=NULL;
  solveAllParErr_=NULL;
  solveAllParSNR_=NULL;
}

void SolvableVisCal::verifyCalTable(const String& caltablename) {

  // Call external method to get type (will throw if bad table)
  String calType=calTableType(caltablename);

  // Check if proper Calibration type...
  if (calType!=typeName()) {
    ostringstream o;
    o << "Table " << caltablename 
      << " has wrong Calibration type: " << calType
      << " (expected: " << typeName() << ")";
    throw(AipsError(String(o)));
  }
}

void SolvableVisCal::applyChanMask(VisBuffer& vb) {

  if (chanmask_) {

    // A reference to de-referenced pointer
    PtrBlock<Vector<Bool>*>& chmask(*chanmask_);
  
    Int spw=vb.spectralWindow();
    Int chan0=vb.channel()(0);
    Int nchan=vb.nChannel();
    if (chmask.nelements()==uInt(nSpw()) &&
	chmask[spw] &&
	sum((*chmask[spw])(Slice(chan0,nchan))) > 0 ) {
      // There are some channels to mask...
      Vector<Bool> fr(vb.flagRow());
      Matrix<Bool> f(vb.flag());
      Vector<Bool> fc;
      Vector<Bool> chm((*(*chanmask_)[spw])(Slice(chan0,nchan)));
      for (Int irow=0;irow<vb.nRow();++irow)
	if (!fr(irow)) {
	  fc.reference(f.column(irow));
	  fc = fc||chm;
	  
	  //	cout << irow << ": ";
	  //	for (Int j=0;j<nchan;++j) cout << fc(j);
	  //	cout << endl;
	  
	}
    }
  }

}
  //
  //-----------------------------------------------------------------------
  //  
  void SolvableVisCal::printActivity(const Int nSlots, const Int slotNo, 
				     const Int fieldId, const Int spw, 
				     const Int nSolutions)
  {
    Int nMesg;

    //    nSlots = rcs().nTime(spw);
    
    Double timeTaken = timer_p.all();
    if (maxTimePerSolution_p < timeTaken) maxTimePerSolution_p = timeTaken;
    if (minTimePerSolution_p > timeTaken) minTimePerSolution_p = timeTaken;
    avgTimePerSolution_p += timeTaken;
    Double avgT =  avgTimePerSolution_p/(nSolutions>0?nSolutions:1);
    //
    // Boost the no. of messages printed if the next message, based on
    // the average time per solution, is going to appear after a time
    // longer than your patience would permit!  The limit of
    // patience defaults to 1h.
    //
    Float boost = userPrintActivityInterval_p/avgT;
    boost = userPrintActivityInterval_p/avgT;
    boost = (boost < 1.0)? 1.0 : nSlots*userPrintActivityFraction_p;
    nMesg = (Int)boost;
    
    Int tmp=abs(nSlots-slotNo); Bool print;
    print = false;
    if (nMesg <= 0) print=false;
    else if ((slotNo == 0) || (slotNo == nSlots-1))  print=true;
    else if ((tmp > 0 ) && ((slotNo+1)%nMesg ==0)) print=true;
    else print=false;

    if (print)
      {
	Int f = (Int)(100*(slotNo+1)/(nSlots>0?nSlots:1));
	logSink()<< LogIO::NORMAL 
		 << "Spw=" << spw << " slot=" << slotNo << "/" << nSlots
		 << " field=" << fieldId << ". Done " << f << "%"
		 << " Time taken per solution (max/min/avg): "
		 << maxTimePerSolution_p << "/" 
		 << (minTimePerSolution_p<0?1:minTimePerSolution_p) << "/"
		 << avgT << " sec" << LogIO::POST;
      }
  }
  
// **********************************************************
//  SolvableVisMueller Implementations
//


SolvableVisMueller::SolvableVisMueller(VisSet& vs) :
  VisCal(vs),
  VisMueller(vs),
  SolvableVisCal(vs),
  dM_(NULL),
  diffMElem_(),
  DMValid_(false)
{
  if (prtlev()>2) cout << "SVM::SVM(vs)" << endl;
}

SolvableVisMueller::SolvableVisMueller(String msname,Int MSnAnt,Int MSnSpw) :
  VisCal(msname,MSnAnt,MSnSpw),
  VisMueller(msname,MSnAnt,MSnSpw),
  SolvableVisCal(msname,MSnAnt,MSnSpw),
  dM_(NULL),
  diffMElem_(),
  DMValid_(false)
{
  if (prtlev()>2) cout << "SVM::SVM(msname,MSnAnt,MSnSpw)" << endl;
}

SolvableVisMueller::SolvableVisMueller(const MSMetaInfoForCal& msmc) :
  VisCal(msmc),
  VisMueller(msmc),
  SolvableVisCal(msmc),
  dM_(NULL),
  diffMElem_(),
  DMValid_(False)
{
  if (prtlev()>2) cout << "SVM::SVM(msmc)" << endl;
}

SolvableVisMueller::SolvableVisMueller(const Int& nAnt) :
  VisCal(nAnt),
  VisMueller(nAnt),
  SolvableVisCal(nAnt),
  dM_(NULL),
  diffMElem_(),
  DMValid_(false)
{
  if (prtlev()>2) cout << "SVM::SVM(i,j,k)" << endl;
}

SolvableVisMueller::~SolvableVisMueller() {

  if (prtlev()>2) cout << "SVM::~SVM()" << endl;

}

// Setup solvePar shape (Mueller version)
void SolvableVisMueller::initSolvePar() {

  // TBD: NCT: attention to solveAllPar

  if (prtlev()>3) cout << " SVM::initSolvePar()" << endl;
  
  for (Int ispw=0;ispw<nSpw();++ispw) {
    
    currSpw()=ispw;

    switch(parType()) {
    case VisCalEnum::COMPLEX: {
      solveAllCPar().resize(nPar(),nChanPar(),nBln());
      solveAllCPar()=Complex(1.0);
      solveCPar().reference(solveAllCPar());
      break;
    }
    case VisCalEnum::REAL: {
      solveAllRPar().resize(nPar(),nChanPar(),nBln());
      solveAllRPar()=0.0;
      solveRPar().reference(solveAllRPar());
      break;
    }
    case VisCalEnum::COMPLEXREAL: {
      throw(AipsError("Internal error(Calibrater Module): Unsupported parameter type "
		      "COMPLEXREAL found in SolvableVisMueller::initSolvePar()"));
      break;
    }
    }//switch

    solveAllParOK().resize(nPar(),nChanPar(),nBln());
    solveAllParErr().resize(nPar(),nChanPar(),nBln());
    solveAllParSNR().resize(nPar(),nChanPar(),nBln());
    solveAllParOK()=true;
    solveAllParErr()=0.0;
    solveAllParSNR()=0.0;
    solveParOK().reference(solveAllParOK());
    solveParErr().reference(solveAllParErr());
    solveParSNR().reference(solveAllParSNR());
  }
  currSpw()=0;
}

void SolvableVisMueller::syncDiffMat() {

  if (prtlev()>5) cout << "     SVM::syncDiffMat()" 
		       << " (DMValid()=" << DMValid() << ")" << endl;

  // Sync the diff'd Mueller matrices
  if (!DMValid()) syncDiffMueller();

}

void SolvableVisMueller::syncDiffMueller() {

  if (prtlev()>6) cout << "      SVM::syncDiffMueller()" << endl;

  // TBD:  validateDM() for trivialMuellerElem()=true??
  //    (cf. where does invalidateDM() occur?)

  if (trivialDM())
    // Ensure trivial matrices ready
    initTrivDM();
  else {
    diffMElem().resize(IPosition(4,muellerNPar(muellerType()),nPar(),nChanMat(),nCalMat()));
    diffMElem().unique();
    invalidateDM();

    // Calculate for all blns/chans
    calcAllDiffMueller();

  }

  // Ensure diff'd Mueller matrix renders are OK
  createDiffMueller();

  // diff'd Mueller matrices now valid
  validateDM();

}

void SolvableVisMueller::calcAllDiffMueller() {

  if (prtlev()>6) cout << "       SVM::calcAllDiffMueller" << endl;

  // Should handle OK flags in this method, and only
  //  do  calc if OK

  // Sanity check on parameter channel axis
  AlwaysAssert((solveCPar().shape()(1)==1),AipsError);

  // Referencing arrays, per baseline
  Matrix<Complex> oneDM;       //  (nElem,nPar)
  Vector<Complex> onePar;      //  (nPar)

  ArrayIterator<Complex> dMiter(diffMElem(),2);
  ArrayIterator<Complex> Piter(solveCPar(),1);
  
  for (Int ibln=0; ibln<nCalMat(); ++ibln) {

    // Solving parameters are NEVER channel-dependent
    //  (even if data & matrices are)
    onePar.reference(Piter.array());
      
    for (Int ich=0; ich<nChanMat(); ich++) {
      
      oneDM.reference(dMiter.array());

      // Calculate the DM matrices for each par on this bln/chan
      calcOneDiffMueller(oneDM,onePar);

      // Advance iterators
      dMiter.next();

    }
    Piter.next();
  }

}

void SolvableVisMueller::calcOneDiffMueller(Matrix<Complex>&, 
					    const Vector<Complex>&) {

  if (prtlev()>10) cout << "        SVM::calcOneDiffMueller()" << endl;

  // If Mueller matrix is trivial, shouldn't get here
  if (trivialMuellerElem()) 
    throw(AipsError("Trivial Mueller Matrix logic error."));

  // Otherwise, this method apparently hasn't been specialized, as required
  else
    throw(AipsError("Unknown non-trivial dMueller-from-parameter calculation requested."));

}

void SolvableVisMueller::createDiffMueller() {

  if (prtlev()>6) cout << "       SVM::createDiffMueller()" << endl;

  Mueller::MuellerType mtype(muellerType());

  // Delete if wrong type
  if (dM_ && dM().type() != mtype) delete dM_;
  
  // If needed, construct the correct diff Mueller
  if (!dM_) dM_ = casa::createMueller(mtype);
      
}

void SolvableVisMueller::initTrivDM() {

  if (prtlev()>7) cout << "        SVM::initTrivDM()" << endl;

  // If DM matrice not trivial, shouldn't get here
  if (!trivialDM()) 
    throw(AipsError("Trivial Mueller Matrix logic error."));

  // Otherwise, this method apparently hasn't been specialized, as required
  else
    throw(AipsError("Unknown trivial dM initialization requested."));

}

// File a solved solution (and meta-data) into the in-memory Caltable
void SolvableVisMueller::keepNCT() {
  
  // Call parent to do general stuff
  //  This adds nElem() rows
  SolvableVisCal::keepNCT();

  if (prtlev()>4) 
    cout << " SVM::keepNCT" << endl;

  // Form antenna pairs
  Vector<Int> a1(nElem()),a2(nElem());
  Int k=0;
  for (Int i=0;i<nAnt();++i)
    for (Int j=i;j<nAnt();++j) {
      a1(k)=i;
      a2(k)=j;
      ++k;
    }

  // We are adding to the most-recently added rows
  RefRows rows(ct_->nrow()-nElem(),ct_->nrow()-1,1); 

  // Write to table
  CTMainColumns ncmc(*ct_);
  ncmc.antenna1().putColumnCells(rows,a1);
  ncmc.antenna2().putColumnCells(rows,a2);

}


void SolvableVisMueller::stateSVM(const Bool& doVC) {
  
  // If requested, report VisCal state
  if (doVC) VisMueller::state();
  
  // Get parent's state (w/out VC):
  SolvableVisCal::stateSVC(false);

  if (applyByMueller()) {
    if (prtlev()>3) cout << "SVM::stateSVM()" << endl;
    cout << boolalpha;

  // Now SVM-specific stuff:
    cout << "DMValid() = " << DMValid() << endl;
    cout << "diffMElem().shape() = " << diffMElem().shape() 
	 << " (" << diffMElem().data() << ")" << endl;
    
    cout << "- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -" << endl;
  }
}

Float SolvableVisMueller::calcPowerNorm(Array<Float>& amp, const Array<Bool>& ok) {

  // SVM version assumes amp already in power units
  Array<Float> a2;
  a2.assign(amp);
  a2(!ok)=0.0; // zero flagged samples

  Float norm(1.0);
  switch (solNorm().normtype()) {
  case SolNorm::MEAN: {
    Float n=Float(ntrue(ok));
    if (n>0.0)
      norm=sum(a2)/n;
    break;
  }
  case SolNorm::MEDIAN: {
    MaskedArray<Float> a2masked(a2,ok);
    norm=median(a2masked,false,true);  // unsorted, do mean when even
    break;
  }
  default:
    throw(AipsError("Proper normalization type not specified."));
    break;
  }
  return norm;
}


// **********************************************************
//  SolvableVisJones Implementations
//


SolvableVisJones::SolvableVisJones(VisSet& vs) :
  VisCal(vs),                           // virtual base
  VisMueller(vs),                       // virtual base
  SolvableVisMueller(vs),               // immediate parent
  VisJones(vs),                         // immediate parent
  dJ1_(NULL),                           // data...
  dJ2_(NULL),
  diffJElem_(),
  DJValid_(false)
{
  if (prtlev()>2) cout << "SVJ::SVJ(vs)" << endl;
}

SolvableVisJones::SolvableVisJones(String msname,Int MSnAnt,Int MSnSpw) :
  VisCal(msname,MSnAnt,MSnSpw),             // virtual base
  VisMueller(msname,MSnAnt,MSnSpw),         // virtual base
  SolvableVisMueller(msname,MSnAnt,MSnSpw), // immediate parent
  VisJones(msname,MSnAnt,MSnSpw),           // immediate parent
  dJ1_(NULL),                           // data...
  dJ2_(NULL),
  diffJElem_(),
  DJValid_(false)
{
  if (prtlev()>2) cout << "SVJ::SVJ(msname,MSnAnt,MSnSpw)" << endl;
}

SolvableVisJones::SolvableVisJones(const MSMetaInfoForCal& msmc) :
  VisCal(msmc),             // virtual base
  VisMueller(msmc),         // virtual base
  SolvableVisMueller(msmc), // immediate parent
  VisJones(msmc),           // immediate parent
  dJ1_(NULL),               // data...
  dJ2_(NULL),
  diffJElem_(),
  DJValid_(False)
{
  if (prtlev()>2) cout << "SVJ::SVJ(msmc)" << endl;
}


SolvableVisJones::SolvableVisJones(const Int& nAnt) : 
  VisCal(nAnt),               // virtual base
  VisMueller(nAnt),           // virtual base
  SolvableVisMueller(nAnt),   // immediate parent
  VisJones(nAnt),             // immediate parent
  dJ1_(NULL),                 // data...
  dJ2_(NULL),
  diffJElem_(),
  DJValid_(false)
{
  if (prtlev()>2) cout << "SVJ::SVJ(i,j,k)" << endl;
}

SolvableVisJones::~SolvableVisJones() {

  if (prtlev()>2) cout << "SVJ::~SVJ()" << endl;
  if (dJ1_)
      delete dJ1_;
  if (dJ2_)
      delete dJ2_;
}

void SolvableVisJones::reReference() {

  // TBD: Handle single-poln referencing
  // TBD: Handle missing refant

  // Generic version for trivial types
  if (trivialJonesElem()) {

    // Determine multiplicative complex phase
    Matrix<Complex> refgain;
    refgain=solveCPar().xyPlane(refant());

    //    cout << "refgain add:  " << refgain.data() << " " << solveCPar().xyPlane(refant()).data() << endl;

    Float amp(1.0);
    Complex* rg=refgain.data();
    // TBD: clean this up: there is only 1 solvePar chan!
    //    for (Int ich=0;ich<nChanPar();++ich) {
      for (Int ip=0;ip<nPar();++ip,++rg) {
	amp=abs(*rg);
	if (amp>0.0) {
	  *rg/=amp;
	  *rg=conj(*rg);
	}
	else
	  *rg=Complex(1.0);
      }
      //    }

      //      cout << "amp(refgain) = " << amplitude(refgain) << endl;


    // Apply complex phase to each ant
    Matrix<Complex> antgain;
    for (Int ia=0;ia<nAnt();++ia) {
      antgain.reference(solveCPar().xyPlane(ia));
      antgain*=refgain;
    }
  }
  else 
    throw(AipsError("Attempt to reference non-trivial calibration type."));


}


void SolvableVisJones::differentiate(CalVisBuffer& cvb) {

  if (prtlev()>3) cout << "  SVJ::differentiate(CVB)" << endl;

  // NB: For freqDepPar()=true, the data and solutions are
  //     multi-channel, but nChanMat()=1 because we only 
  //     consider one channel at a time.  In this case,
  //     focusChan is the specific channel under consideration.
  //     Otherwise, we will use all channels in the vb 
  //     simultaneously

  // Some vb shape info
  Int& nRow(cvb.nRow());
  Int& nCorr(cvb.nCorr());

  // Size (diff)residuals workspace in the CVB
  cvb.setFocusChan(focusChan());
  cvb.sizeResiduals(nPar(),2);    // 2 sets of nPar() derivatives per baseline

  // Copy in-focus model to residual workspace
  cvb.initResidWithModel();


  // References to workspaces
  Cube<Complex>& Vout(cvb.residuals());
  Array<Complex>& dVout(cvb.diffResiduals());
  Matrix<Bool>& Vflg(cvb.residFlag());
  
  // "Apply" the current Q,U or X estimates to the crosshand model
  if (solvePol()>0) {
    Complex pol(1.0);

    if (solvePol()==2)  // pol = Q+iU
      pol=Complex(real(srcPolPar()(0)),real(srcPolPar()(1)));
    else if (solvePol()==1)   // pol = exp(iX)
      pol=exp(Complex(0.0,real(srcPolPar()(0))));
    
    IPosition blc(3,1,0,0), trc(3,1,nChanMat()-1,nRow-1);
    Array<Complex> RL(Vout(blc,trc));
    RL*=pol;
    blc(0)=trc(0)=2;
    Array<Complex> LR(Vout(blc,trc));
    LR*=conj(pol);
  }
  
  // Visibility vector renderers
  VisVector::VisType vt(visType(nCorr));
  VisVector cVm(vt);  // The model data corrupted by trial solution
  VisVector dV1(vt);  // The deriv of V wrt pars of 1st ant in bln 
  VisVector dV2(vt);  // The deriv of V wrt pars of 2nd ant in bln 

  // Temporary non-iterating VisVectors to hold partial applies
  VisVector J1V(vt,true);
  VisVector VJ2(vt,true);

  // Starting synchronization for output visibility data
  cVm.sync(Vout(0,0,0));
  dV1.sync(dVout(IPosition(5,0,0,0,0,0)));
  dV2.sync(dVout(IPosition(5,0,0,0,0,1)));

  // Synchronize current calibration pars/matrices
  syncSolveCal();

  // Nominal synchronization of dJs
  dJ1().sync(diffJElem()(IPosition(4,0,0,0,0)));
  dJ2().sync(diffJElem()(IPosition(4,0,0,0,0)));

  // Inform Jones matrices if data is scalar
  Bool scalar(vt==VisVector::One);
  J1().setScalarData(scalar);
  J2().setScalarData(scalar);
  dJ1().setScalarData(scalar);
  dJ2().setScalarData(scalar);

  // VisBuffer indices
  Double* time=  cvb.time().data();
  Int*    a1=    cvb.antenna1().data();
  Int*    a2=    cvb.antenna2().data();
  Bool*   flagR= cvb.flagRow().data();
  Bool*   flag=  Vflg.data();            // via local reference
  
  // TBD: set weights according to flags??

  // iterate rows
  for (Int irow=0; irow<nRow; irow++,flagR++,a1++,a2++,time++) {
    
    // Avoid ACs
    if (*a1==*a2) *flagR=true;

    if (!*flagR) {  // if this row unflagged
	
      // Re-update matrices if time changes
      if (timeDepMat() && *time != lastTime()) {
	currTime()=*time;
	invalidateDiffCalMat();
	syncCalMat();
	syncDiffMat();
	lastTime()=currTime();
      }

      // Synchronize Jones renderers for the ants on this baseline
      J1().sync(currJElem()(0,0,*a1),currJElemOK()(0,0,*a1));
      J2().sync(currJElem()(0,0,*a2),currJElemOK()(0,0,*a2));

      // Synchronize differentiated Jones renderers for this baseline
      if (trivialDJ()) {
	dJ1().origin();
	dJ2().origin();
      } else {
	dJ1().sync(diffJElem()(IPosition(4,0,0,0,*a1)));
	dJ2().sync(diffJElem()(IPosition(4,0,0,0,*a2)));
      }

      // Assumes all iterating quantities have nChanMat() channelization
      for (Int ich=0; ich<nChanMat();ich++,flag++,
	     cVm++, J1()++, J2()++) {
	     
	// if channel unflagged an cal ok
	//	if (!*flag && (*J1Ok && *J2Ok) ) {  
	if (!*flag) { 
	  
	  // Partial applies for repeated use below
	  VJ2=cVm;                    
	  J2().applyLeft(VJ2,*flag);      // VJ2 = Vm*J2, used below

	  J1().applyRight(cVm,*flag);     
	  J1V=cVm;                        // J1V = J1*Vm, used below

	  // Finish trial corruption
	  J2().applyLeft(cVm,*flag);      // cVm = (J1*Vm)*J2

	}

	// Only continue with diff-ing, if we aren't flagged yet
	if (!*flag) {

	  // Differentiation per par
	  for (Int ip=0;ip<nPar();ip++,
		 dV1++,dJ1()++,
		 dV2++,dJ2()++) {
	    
	    dV1=VJ2;
	    dJ1().applyRight(dV1);  // dV1 = dJ1(ip)*(Vm*J2)
	    
	    dV2=J1V;
	    dJ2().applyLeft(dV2);   // dV2 = (J1*Vm)*dJ2(ip)
	  }
	  
	} // (!*flag)
	else {
	  // set trial corruption to zero
	  cVm.zero();
	  
	  // Advance all par-dep pointers over flagged channel
	  dV1.advance(nPar());
	  dV2.advance(nPar());
	  dJ1().advance(nPar());
	  dJ2().advance(nPar());
	}

      } // chn
		
    } // !*flagR
    else {
      // Must advance all chan-, par-dep pointers over flagged row
      flag+=nChanMat(); 
      cVm.advance(nChanMat());
      J1().advance(nChanMat());
      J2().advance(nChanMat());
      Int chpar(nChanMat()*nPar());
      dV1.advance(chpar);
      dV2.advance(chpar);
      dJ1().advance(chpar);
      dJ2().advance(chpar);
    }
  }

  // Subtract the obs'd data from the trial-corrupted model
  //  to form residuals
  cvb.finalizeResiduals();

}

void SolvableVisJones::differentiate(SolveDataBuffer& sdb) {  // VI2

  if (prtlev()>3) cout << "  SVJ::differentiate(SDB)" << endl;

  // NB: For freqDepPar()=True, the data and solutions are
  //     multi-channel, but nChanMat()=1 because we only 
  //     consider one channel at a time.  In this case,
  //     focusChan is the specific channel under consideration.
  //     Otherwise, we will use all channels in the vb 
  //     simultaneously

  // Some vb shape info
  const Int& nRow(sdb.nRows());
  const Int& nCorr(sdb.nCorrelations());

  // Size (diff)residuals workspace in the CVB
  sdb.setFocusChan(focusChan());
  sdb.sizeResiduals(nPar(),2);    // 2 sets of nPar() derivatives per baseline

  // Copy in-focus model to residual workspace
  sdb.initResidWithModel();

  // References to workspaces
  Cube<Complex>& Vout(sdb.residuals());
  Array<Complex>& dVout(sdb.diffResiduals());
  
  // "Apply" the current Q,U or X estimates to the crosshand model
  // NB:  This is circular-basis specific!!

  /*   2016Nov29 (gmoellen):  MOVED TO DJones::guessPar(SDBList)

  if (solvePol()>0) {
    Complex pol(1.0);

    if (solvePol()==2)  // pol = Q+iU
      pol=Complex(real(srcPolPar()(0)),real(srcPolPar()(1)));
    else if (solvePol()==1)   // pol = exp(iX)
      pol=exp(Complex(0.0,real(srcPolPar()(0))));
    
    IPosition blc(3,1,0,0), trc(3,1,nChanMat()-1,nRow-1);
    Array<Complex> RL(Vout(blc,trc));
    RL*=pol;
    blc(0)=trc(0)=2;
    Array<Complex> LR(Vout(blc,trc));
    LR*=conj(pol);
  }
  */  


  // Visibility vector renderers
  VisVector::VisType vt(visType(nCorr));
  VisVector cVm(vt);  // The model data corrupted by trial solution
  VisVector dV1(vt);  // The deriv of V wrt pars of 1st ant in bln 
  VisVector dV2(vt);  // The deriv of V wrt pars of 2nd ant in bln 

  // Temporary non-iterating VisVectors to hold partial applies
  VisVector J1V(vt,True);
  VisVector VJ2(vt,True);

  // Starting synchronization for output visibility data
  cVm.sync(Vout(0,0,0));
  dV1.sync(dVout(IPosition(5,0,0,0,0,0)));
  dV2.sync(dVout(IPosition(5,0,0,0,0,1)));

  // Synchronize current calibration pars/matrices
  syncSolveCal();

  // Nominal synchronization of dJs
  dJ1().sync(diffJElem()(IPosition(4,0,0,0,0)));
  dJ2().sync(diffJElem()(IPosition(4,0,0,0,0)));

  // Inform Jones matrices if data is scalar
  Bool scalar(vt==VisVector::One);
  J1().setScalarData(scalar);
  J2().setScalarData(scalar);
  dJ1().setScalarData(scalar);
  dJ2().setScalarData(scalar);

  // VisBuffer indices
  const Double* time=  sdb.time().data();
  const Int*    a1=    sdb.antenna1().data();
  const Int*    a2=    sdb.antenna2().data();
  const Bool*   flagR= sdb.flagRow().data();
  
  // TBD: set weights according to flags??

  // iterate rows
  for (Int irow=0; irow<nRow; irow++,flagR++,a1++,a2++,time++) {
    
    // Avoid ACs and flagged rows
    if (*a1!=*a2 && !*flagR) {  
	
      // Re-update matrices if time changes
      //  E.g.?
      if (timeDepMat() && *time != lastTime()) {
	currTime()=*time;
	invalidateDiffCalMat();
	syncCalMat();
	syncDiffMat();
	lastTime()=currTime();
      }

      // Synchronize Jones renderers for the ants on this baseline
      J1().sync(currJElem()(0,0,*a1),currJElemOK()(0,0,*a1));
      J2().sync(currJElem()(0,0,*a2),currJElemOK()(0,0,*a2));

      // Synchronize differentiated Jones renderers for this baseline
      if (trivialDJ()) {
	dJ1().origin();
	dJ2().origin();
      } else {
	dJ1().sync(diffJElem()(IPosition(4,0,0,0,*a1)));
	dJ2().sync(diffJElem()(IPosition(4,0,0,0,*a2)));
      }

      // Assumes all iterating quantities have nChanMat() channelization
      for (Int ich=0; ich<nChanMat();ich++,
	     cVm++, J1()++, J2()++) {
	     
	// NB: Ignoring vis flag state  (OK?)
	  
	// Partial applies for repeated use below
	VJ2=cVm;                    
	J2().applyLeft(VJ2);      // VJ2 = Vm*J2, used below

	J1().applyRight(cVm);     
	J1V=cVm;                        // J1V = J1*Vm, used below

	// Finish trial corruption
	J2().applyLeft(cVm);      // cVm = (J1*Vm)*J2

	// Differentiation per par
	for (Int ip=0;ip<nPar();ip++,
	       dV1++,dJ1()++,
	       dV2++,dJ2()++) {
	  
	  dV1=VJ2;
	  dJ1().applyRight(dV1);  // dV1 = dJ1(ip)*(Vm*J2)
	  
	  dV2=J1V;
	  dJ2().applyLeft(dV2);   // dV2 = (J1*Vm)*dJ2(ip)
	}
	  
      } // chn
		
    } // !*flagR
    else {
      // Must advance all chan-, par-dep pointers over flagged row
      cVm.advance(nChanMat());
      J1().advance(nChanMat());
      J2().advance(nChanMat());
      Int chpar(nChanMat()*nPar());
      dV1.advance(chpar);
      dV2.advance(chpar);
      dJ1().advance(chpar);
      dJ2().advance(chpar);
    }
  }

  // Subtract the obs'd data from the trial-corrupted model
  //  to form residuals
  sdb.finalizeResiduals();

}


void SolvableVisJones::differentiate(VisBuffer& vb,
				     Cube<Complex>& Vout, 
				     Array<Complex>& dVout,
				     Matrix<Bool>& Vflg) {
    
  if (prtlev()>3) cout << "  SVJ::differentiate()" << endl;

  // NB: For freqDepPar()=true, the data and solutions are
  //     multi-channel, but nChanMat()=1 because we only 
  //     consider one channel at a time.  In this case,
  //     focusChan is the specific channel under consideration.
  //     Otherwise, we will use all channels in the vb 
  //     simultaneously

  // Some vb shape info
  Int& nRow(vb.nRow());
  Int nCorr(vb.corrType().nelements());

  // Size up the output data arrays
  // Vout = [nCorr,nChan,nRow]
  // dVout = [nCorr,nPar,nChan,nRow,2]   (1 set of dV for both ants on baseline)
  Vout.resize(IPosition(3,nCorr,nChanMat(),nRow));
  Vout.unique();             // ensure unique storage

  dVout.resize(IPosition(5,nCorr,nPar(),nChanMat(),nRow,2));
  dVout.unique();
  dVout=Complex(0.0);
  
  // Copy the input model data from the VisBuffer to Vout
  //  for in-place application (do this according to focusChan)
  //  (also flags)
  Matrix<Bool> fl;
  if (freqDepPar()) {
    // Copy just the focusChan; all work below is single-channel
    AlwaysAssert((nChanMat()==1),AipsError);  // sanity
    AlwaysAssert((focusChan()>-1),AipsError);       // sanity

    Vout = vb.modelVisCube()(IPosition(3,0,      focusChan(),0     ),
			     IPosition(3,nCorr-1,focusChan(),nRow-1));

    Vflg.resize(IPosition(2,1,nRow));   // proper single channel size
    Vflg.unique();                      // unique storage
    Vflg = vb.flag()(IPosition(2,focusChan(),0     ),
		     IPosition(2,focusChan(),nRow-1));
  }
  else {
    // Copy all channels in the vb
    Vout = vb.modelVisCube();
    Vflg.reference(vb.flag());   // Just reference whole flag array
  }

  // "Apply" the current Q,U or X estimates to the crosshand model
  if (solvePol()>0) {
    Complex pol(1.0);

    if (solvePol()==2)  // pol = Q+iU
      pol=Complex(real(srcPolPar()(0)),real(srcPolPar()(1)));
    else if (solvePol()==1)   // pol = exp(iX)
      pol=exp(Complex(0.0,real(srcPolPar()(0))));
    
    IPosition blc(3,1,0,0), trc(3,1,nChanMat()-1,nRow-1);
    Array<Complex> RL(Vout(blc,trc));
    RL*=pol;
    blc(0)=trc(0)=2;
    Array<Complex> LR(Vout(blc,trc));
    LR*=conj(pol);
  }
  
  // Visibility vector renderers
  VisVector::VisType vt(visType(nCorr));
  VisVector cVm(vt);  // The model data corrupted by trial solution
  VisVector dV1(vt);  // The deriv of V wrt pars of 1st ant in bln 
  VisVector dV2(vt);  // The deriv of V wrt pars of 2nd ant in bln 

  // Temporary non-iterating VisVectors to hold partial applies
  VisVector J1V(vt,true);
  VisVector VJ2(vt,true);

  // Starting synchronization for output visibility data
  cVm.sync(Vout(0,0,0));
  dV1.sync(dVout(IPosition(5,0,0,0,0,0)));
  dV2.sync(dVout(IPosition(5,0,0,0,0,1)));

  // Synchronize current calibration pars/matrices
  syncSolveCal();

  // Nominal synchronization of dJs
  dJ1().sync(diffJElem()(IPosition(4,0,0,0,0)));
  dJ2().sync(diffJElem()(IPosition(4,0,0,0,0)));

  // Inform Jones matrices if data is scalar
  Bool scalar(vt==VisVector::One);
  J1().setScalarData(scalar);
  J2().setScalarData(scalar);
  dJ1().setScalarData(scalar);
  dJ2().setScalarData(scalar);

  // VisBuffer indices
  Double* time=  vb.time().data();
  Int*    a1=    vb.antenna1().data();
  Int*    a2=    vb.antenna2().data();
  Bool*   flagR= vb.flagRow().data();
  Bool*   flag=  Vflg.data();            // via local reference
  
  // TBD: set weights according to flags??

  // iterate rows
  for (Int irow=0; irow<nRow; irow++,flagR++,a1++,a2++,time++) {
    
    // Avoid ACs
    if (*a1==*a2) *flagR=true;

    if (!*flagR) {  // if this row unflagged
	
      // Re-update matrices if time changes
      if (timeDepMat() && *time != lastTime()) {
	currTime()=*time;
	invalidateDiffCalMat();
	syncCalMat();
	syncDiffMat();
	lastTime()=currTime();
      }

      // Synchronize Jones renderers for the ants on this baseline
      J1().sync(currJElem()(0,0,*a1),currJElemOK()(0,0,*a1));
      J2().sync(currJElem()(0,0,*a2),currJElemOK()(0,0,*a2));

      // Synchronize differentiated Jones renderers for this baseline
      if (trivialDJ()) {
	dJ1().origin();
	dJ2().origin();
      } else {
	dJ1().sync(diffJElem()(IPosition(4,0,0,0,*a1)));
	dJ2().sync(diffJElem()(IPosition(4,0,0,0,*a2)));
      }

      // Assumes all iterating quantities have nChanMat() channelization
      for (Int ich=0; ich<nChanMat();ich++,flag++,
	     cVm++, J1()++, J2()++) {
	     
	// if channel unflagged an cal ok
	//	if (!*flag && (*J1Ok && *J2Ok) ) {  
	if (!*flag) { 
	  
	  // Partial applies for repeated use below
	  VJ2=cVm;                    
	  J2().applyLeft(VJ2,*flag);      // VJ2 = Vm*J2, used below

	  J1().applyRight(cVm,*flag);     
	  J1V=cVm;                        // J1V = J1*Vm, used below

	  // Finish trial corruption
	  J2().applyLeft(cVm,*flag);      // cVm = (J1*Vm)*J2

	}

	// Only continue with diff-ing, if we aren't flagged yet
	if (!*flag) {

	  // Differentiation per par
	  for (Int ip=0;ip<nPar();ip++,
		 dV1++,dJ1()++,
		 dV2++,dJ2()++) {
	    
	    dV1=VJ2;
	    dJ1().applyRight(dV1);  // dV1 = dJ1(ip)*(Vm*J2)
	    
	    dV2=J1V;
	    dJ2().applyLeft(dV2);   // dV2 = (J1*Vm)*dJ2(ip)
	  }
	  
	} // (!*flag)
	else {
	  // set trial corruption to zero
	  cVm.zero();
	  
	  // Advance all par-dep pointers over flagged channel
	  dV1.advance(nPar());
	  dV2.advance(nPar());
	  dJ1().advance(nPar());
	  dJ2().advance(nPar());
	}

      } // chn
		
    } // !*flagR
    else {
      // Must advance all chan-, par-dep pointers over flagged row
      flag+=nChanMat(); 
      cVm.advance(nChanMat());
      J1().advance(nChanMat());
      J2().advance(nChanMat());
      Int chpar(nChanMat()*nPar());
      dV1.advance(chpar);
      dV2.advance(chpar);
      dJ1().advance(chpar);
      dJ2().advance(chpar);
    }
  }

}

void SolvableVisJones::diffSrc(VisBuffer& vb,
			       Array<Complex>& dVout) {
    
  if (prtlev()>3) cout << "  SVJ::diffSrc()" << endl;

  // Some vb shape info
  Int& nRow(vb.nRow());
  Int nCorr(vb.corrType().nelements());

  // Size up the output data arrays
  dVout.resize(IPosition(4,nCorr,nChanMat(),nRow,solvePol()));
  dVout.unique();
  dVout=Complex(0.0);
  
  // For now, we don't actually need gradients w.r.t. the source
  return;

  IPosition blc(4,0,0,0,0), trc(4,0,nChanMat()-1,nRow-1,0);

  if (solvePol()==2) {
    blc(3)=trc(3)=0;
    blc(0)=1;trc(0)=2;
    dVout(blc,trc)=Complex(1.0);   // Q part (both RL & LR)
    blc(3)=trc(3)=1;
    blc(0)=trc(0)=1;
    dVout(blc,trc)=Complex(0.0,1.0);  // U part (in RL)
    blc(0)=trc(0)=2;
    dVout(blc,trc)=Complex(0.0,-1.0); // U part (in LR)
  }
  else if (solvePol()==1) {
    Complex dX=Complex(0.0,1.0)*exp(Complex(0.0,real(srcPolPar()(0))));
    blc(3)=trc(3)=0;
    blc(0)=trc(0)=1;
    dVout(blc,trc)=dX;  // multiplying RL
    blc(0)=trc(0)=2;
    dVout(blc,trc)=conj(dX); // multiplying LR
  }

  // Visibility vector renderers
  VisVector::VisType vt(visType(nCorr));
  VisVector dSm1(vt);  // The model data corrupted by trial solution
  VisVector dSm2(vt);  // The model data corrupted by trial solution

  // Starting synchronization for output visibility data
  dSm1.sync(dVout(IPosition(4,0,0,0,0)));
  if (solvePol()>1)
    dSm2.sync(dVout(IPosition(4,0,0,0,1)));

  // Synchronize current calibration pars/matrices
  syncSolveCal();

  // Nominal synchronization of dJs
  dJ1().sync(diffJElem()(IPosition(4,0,0,0,0)));
  dJ2().sync(diffJElem()(IPosition(4,0,0,0,0)));

  // Inform Jones matrices if data is scalar
  Bool scalar(vt==VisVector::One);
  J1().setScalarData(scalar);
  J2().setScalarData(scalar);
  dJ1().setScalarData(scalar);
  dJ2().setScalarData(scalar);

  // VisBuffer indices
  Double* time=  vb.time().data();
  Int*    a1=    vb.antenna1().data();
  Int*    a2=    vb.antenna2().data();
  Bool*   flagR= vb.flagRow().data();
  Bool*   flag=  vb.flag().data();
  
  // TBD: set weights according to flags??

  // iterate rows
  for (Int irow=0; irow<nRow; irow++,flagR++,a1++,a2++,time++) {
    
    // Avoid ACs
    if (*a1==*a2) *flagR=true;

    if (!*flagR) {  // if this row unflagged
	
      // Re-update matrices if time changes
      if (timeDepMat() && *time != lastTime()) {
	currTime()=*time;
	invalidateDiffCalMat();
	syncCalMat();
	syncDiffMat();
	lastTime()=currTime();
      }

      // Synchronize Jones renderers for the ants on this baseline
      J1().sync(currJElem()(0,0,*a1),currJElemOK()(0,0,*a1));
      J2().sync(currJElem()(0,0,*a2),currJElemOK()(0,0,*a2));

      // Assumes all iterating quantities have nChanMat() channelization
      for (Int ich=0; ich<nChanMat();ich++,flag++,
	     dSm1++, dSm2++, J1()++, J2()++) {
	     
	// if channel unflagged an cal ok
	if (!*flag) { 
	  
	  J1().applyRight(dSm1);
	  J2().applyLeft(dSm1);
	  if (solvePol()>1) {
	    J1().applyRight(dSm2);
	    J2().applyLeft(dSm2);
	  }
	}

      } // chn
		
    } // !*flagR
    else {
      // Must advance all chan-, par-dep pointers over flagged row
      flag+=nChanMat(); 
      dSm1.advance(nChanMat());
      dSm2.advance(nChanMat());
      J1().advance(nChanMat());
      J2().advance(nChanMat());
    }
  }

}

void SolvableVisJones::accumulate(SolvableVisCal* incr,
                                  const Vector<Int>& fields) {

  // Use SVJ interface for the incremental component
  //  (this should always be safe at this point?)
  SolvableVisJones* svj = dynamic_cast<SolvableVisJones*>(incr);

  // Catch bad SVJ conversion or fundamental type mismatch
  if (svj==NULL || svj->type() != this->type())
    throw(AipsError("Incremental calibration is not of compatible type."));

  Int nfield(fields.nelements());

  Bool fldok(true);

  // TBD: Iterate over the ct_
  Block<String> cols(2);
  cols[0]="SPECTRAL_WINDOW_ID";
  cols[1]="TIME";
  CTIter ctiter(*ct_,cols);

  cout << boolalpha;
  Int piter(0);
  Int prow(0);
  while (!ctiter.pastEnd()) {
    
    currSpw()=ctiter.thisSpw();
    currTime()=ctiter.thisTime();

    /*    
    cout << "Spw=" << currSpw() << " spwok=" << svj->spwOK(currSpw());
    cout << " Time=" << MVTime(currTime()/C::day).string(MVTime::YMD,7);
    cout << " nrow=" << ctiter.nrow();
    */

    // Only update spws which are available in the incr table:
    if (svj->spwOK(currSpw())) {
	
      currField()=ctiter.thisField();

      // Is current field among those we need to update?
      fldok = (nfield==0 || anyEQ(fields,currField()));	

      //      cout << " Fld=" << currField() << " fldok=" << fldok;

      if (fldok) {

	currFreq()=ctiter.freq();

	currCPar().assign(ctiter.cparam());
	currParOK().assign(!ctiter.flag());

	syncCalMat(false);  // a reference!!
	  
	// Sync svj with this
	svj->syncCal(*this);

	AlwaysAssert( (nChanMat()==svj->nChanMat()), AipsError);
	  
	// Do the multiplication each ant, chan
	for (Int iant=0; iant<nAnt(); iant++) {
	  for (Int ichan=0; ichan<svj->nChanMat(); ichan++) {
	    J1()*=(svj->J1());
	    J1()++;
	    svj->J1()++;
	  } // ichan
	} // iant

	//cout << "  keep";
	
	ctiter.setcparam(currCPar());  // assumes matrices are references
	ctiter.setflag(!currParOK());

	piter+=1;
	prow+=ctiter.nrow();

      } // fldok
    } // spwOK

    //    cout << endl;

    // Advance iterator
    ctiter.next();
    
  } // ispw

  //  cout << "Processed " << prow << " rows in " << piter << " iterations." << endl;

}



// Setup solvePar shape (Jones version)
void SolvableVisJones::initSolvePar() {

  if (prtlev()>3) cout << " SVJ::initSolvePar()" << endl;

  for (Int ispw=0;ispw<nSpw();++ispw) {

    currSpw()=ispw;

    switch (parType()) {
    case VisCalEnum::COMPLEX: {
      solveAllCPar().resize(nPar(),nChanPar(),nAnt());
      solveAllCPar()=Complex(1.0);
      if (nChanPar()==1)
	solveCPar().reference(solveAllCPar());
      else {
	solveCPar().resize(nPar(),1,nAnt());
	solveCPar()=Complex(1.0);
      }
      break;
    }
    case VisCalEnum::REAL: {
      solveAllRPar().resize(nPar(),nChanPar(),nAnt());
      solveAllRPar()=0.0;
      if (nChanPar()==1)
	solveRPar().reference(solveAllRPar());
      else {
	solveRPar().resize(nPar(),1,nAnt());
	solveRPar()=0.0;
      }
      break;
    }
    default:
      throw(AipsError("Internal error(Calibrater Module): Unsupported parameter type "
		      "COMPLEXREAL found in SolvableVisJones::initSolvePar()"));
    }
    
    solveAllParOK().resize(nPar(),nChanPar(),nAnt());
    solveAllParErr().resize(nPar(),nChanPar(),nAnt());
    solveAllParSNR().resize(nPar(),nChanPar(),nAnt());
    solveAllParOK()=true;
    solveAllParErr()=0.0;
    solveAllParSNR()=0.0;
    if (nChanPar()==1) {
      solveParOK().reference(solveAllParOK());
      solveParErr().reference(solveAllParErr());
      solveParSNR().reference(solveAllParSNR());
    }
    else {
      // solving many channels, one at a time
      solveParOK().resize(nPar(),1,nAnt());
      solveParErr().resize(nPar(),1,nAnt());
      solveParSNR().resize(nPar(),1,nAnt());
      solveParOK()=true;
      solveParErr()=0.0;
      solveParSNR()=0.0;
    }
  }
  currSpw()=0;

}

void SolvableVisJones::syncDiffMat() {

  if (prtlev()>5) cout << "     SVJ::syncDiffMat()" 
		       << " (DJValid()=" << DJValid() << ")" << endl;

  // Sync the diff'd Jones matrices
  if (!DJValid()) syncDiffJones();

  // Sync up Muellers, if necessary
  //  if (applyByMueller()) 
      // Do nothing for now!  --All dJ applied directly in differentiate
      // if (!DMValid()) syncDiffMueller();

}

void SolvableVisJones::syncDiffJones() {

  if (prtlev()>6) cout << "      SVJ::syncDiffJones()" << endl;

  // If differentiated Jones are trivial, we are
  // already referencing the type-dep trivial versions
  //  TBD: Review this for D, where trivialJonesElem()=false,
  //   but diffJ is "trivial-ish"!!!  (Probably need trivDiffJonesElem(),
  //   or override this method in D to make it no-op)

  if (trivialDJ())
    // Ensure trivial matrices ready
    initTrivDJ();
  else {
    diffJElem().resize(IPosition(4,jonesNPar(jonesType()),nPar(),nChanMat(),nCalMat()));
    diffJElem().unique();
    invalidateDJ();

    // Calculate for all ants/chans
    calcAllDiffJones();

  }

  // Ensure diff'd Jones matrix renders are OK
  createDiffJones();

  // diff'd Jones matrices now valid
  validateDJ();
  invalidateDM();   // dMs still invalid, probably forever

}

void SolvableVisJones::calcAllDiffJones() {

  if (prtlev()>6) cout << "       SVJ::calcAllDiffJones" << endl;

  // Should handle OK flags in this method, and only
  //  do  calc if OK

  Matrix<Complex> oneDJ;   //  (nElem,nPar)
  Vector<Complex> onePar;      //  (nPar)

  ArrayIterator<Complex> dJiter(diffJElem(),2);
  ArrayIterator<Complex> Piter(currCPar(),1);
  
  for (Int iant=0; iant<nCalMat(); iant++) {

    // Solving parameters are NEVER channel-dependent
    //  (even if data & matrices are)
    onePar.reference(Piter.array());
      
    for (Int ich=0; ich<nChanMat(); ich++) {
      
      oneDJ.reference(dJiter.array());

      // Calculate the DJ matrices w.r.t. each par on this ant/chan
      calcOneDiffJones(oneDJ,onePar);

      // Advance iterators
      dJiter.next();

    }
    Piter.next();
  }

}

void SolvableVisJones::calcOneDiffJones(Matrix<Complex>&, 
					const Vector<Complex>&) {

  if (prtlev()>10) cout << "        SVJ::calcOneDiffJones()" << endl;

  // If Jones matrix is trivial, shouldn't get here
  if (trivialJonesElem()) 
    throw(AipsError("Trivial Jones Matrix logic error."));

  // Otherwise, this method apparently hasn't been specialized, as required
  else
    throw(AipsError("Unknown non-trivial dJones-from-parameter calculation requested."));

}

void SolvableVisJones::createDiffJones() {

  if (prtlev()>6) cout << "       SVJ::createDiffJones()" << endl;

  Jones::JonesType jtype(jonesType());

  // Delete if wrong type
  if (dJ1_ && dJ1().type() != jtype) delete dJ1_;
  if (dJ2_ && dJ2().type() != jtype) delete dJ2_;
  
  // If needed, construct the correct diff Jones
  if (!dJ1_) dJ1_ = casa::createJones(jtype);
  if (!dJ2_) dJ2_ = casa::createJones(jtype);
      
}

void SolvableVisJones::initTrivDJ() {

  if (prtlev()>7) cout << "        SVJ::initTrivDJ()" << endl;

  // If DJ matrice not trivial, shouldn't get here
  if (!trivialDJ()) 
    throw(AipsError("Trivial Jones Matrix logic error."));

  // Otherwise, this method apparently hasn't been specialized, as required
  else
    throw(AipsError("Unknown trivial dJ initialization requested."));

}

// File a solved solution (and meta-data) into the in-memory Caltable
void SolvableVisJones::keepNCT() {
  
  // Call parent to do general stuff
  SolvableVisCal::keepNCT();

  if (prtlev()>4) 
    cout << " SVJ::keepNCT" << endl;

  // Antenna id sequence
  Vector<Int> a1(nAnt());
  indgen(a1);

  // We are adding to the most-recently added rows
  RefRows rows(ct_->nrow()-nElem(),ct_->nrow()-1,1); 

  // Write to table
  CTMainColumns ncmc(*ct_);
  ncmc.antenna1().putColumnCells(rows,a1);
  ncmc.antenna2().putColumnCells(rows,Vector<Int>(nAnt(),-1));  // Unknown but nominally unform
 
  // NB: a2 will be set separately, e.g., by applyRefAnt

}

void SolvableVisJones::stateSVJ(const Bool& doVC) {
  
  // If requested, report VisCal state
  if (doVC) VisJones::state();

  // Get parent's state (w/out VC):
  SolvableVisMueller::stateSVM(false);

  if (applyByJones()) {
    if (prtlev()>3) cout << "SVJ::stateSVJ()" << endl;
    cout << boolalpha;
    
    // Now SVJ-specific stuff:
    cout << "  DJValid() = " << DJValid() << endl;
    
    cout << "  diffJElem().shape() = " << diffJElem().shape() 
	 << " (" << diffJElem().data() << ")" << endl;
    cout << "- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -" << endl;
  }
}

Float SolvableVisJones::calcPowerNorm(Array<Float>& amp, const Array<Bool>& ok) {

  // SVJ version asumes amps are voltages, so square them
  Array<Float> a2(square(amp));
  a2(!ok)=0.0; // zero flagged samples


  Float norm2(1.0);
  switch (solNorm().normtype()) {
  case SolNorm::MEAN: {
    Float n=Float(ntrue(ok));
    if (n>0.0)
      norm2=sum(a2)/n;
    break;
  }
  case SolNorm::MEDIAN: {
    MaskedArray<Float> a2masked(a2,ok);
    norm2=median(a2masked,false,true);  // unsorted, do mean when even
    break;
  }
  default:
    throw(AipsError("Proper normalization type not specified."));
    break;
  }

  // Return sqrt, because Jones are voltages
  return sqrt(norm2); 
}

void SolvableVisJones::globalPostSolveTinker() {

  // Re-reference the phase, if requested
  if (refantlist()(0)>-1) applyRefAnt();

  // Apply more general post-solve stuff
  SolvableVisCal::globalPostSolveTinker();

}

void SolvableVisJones::applyRefAnt() {

  // TBD:
  // 1. Synchronize refant changes on par axis
  // 2. Implement minimum mean deviation algorithm

  if (refantlist()(0)<0) 
    throw(AipsError("No refant specified."));

  Int nUserRefant=refantlist().nelements();

  // Get the preferred refant names from the MS
  String refantName(msmc().antennaName(refantlist()(0)));
  if (nUserRefant>1) {
    refantName+=" (";
    for (Int i=1;i<nUserRefant;++i) {
      refantName+=msmc().antennaName(refantlist()(i));
      if (i<nUserRefant-1) refantName+=",";
    }
    refantName+=")";
  }

  logSink() << "Applying refant: " << refantName
	    << " refantmode = " << refantmode();
  if (refantmode()=="flex")
    logSink() << " (hold alternate refants' phase constant) when refant flagged";
  if (refantmode()=="strict")
    logSink() << " (flag all antennas when refant flagged)";
  logSink() << LogIO::POST;

  // Generate a prioritized refant choice list
  //  The first entry in this list is the user's primary refant,
  //   the second entry is the refant used on the previous interval,
  //   and the rest is a prioritized list of alternate refants,
  //   starting with the user's secondary (if provided) refants,
  //   followed by the rest of the array, in distance order.   This
  //   makes the priorities correct all the time, and prevents
  //   a semi-stochastic alternation (by preferring the last-used
  //   alternate, even if nominally higher-priority refants become
  //   available)


  // Extract antenna positions
  Matrix<Double> xyz;
  if (msName()!="<noms>") {
    MeasurementSet ms(msName());
    MSAntennaColumns msant(ms.antenna());
    msant.position().getColumn(xyz);
  }
  else {
    // TBD RO*
    CTColumns ctcol(*ct_);
    CTAntennaColumns& antcol(ctcol.antenna());
    antcol.position().getColumn(xyz);
  }

  // Calculate (squared) antenna distances, relative
  //  to last preferred antenna
  Vector<Double> dist2(xyz.ncolumn(),0.0);
  for (Int i=0;i<3;++i) {
    Vector<Double> row=xyz.row(i);
    row-=row(refantlist()(nUserRefant-1));
    dist2+=square(row);
  }
  // Move preferred antennas to a large distance
  for (Int i=0;i<nUserRefant;++i)
    dist2(refantlist()(i))=DBL_MAX;

  // Generated sorted index
  Vector<uInt> ord;
  genSort(ord,dist2);

  // Assemble the whole choices list
  Int nchoices=nUserRefant+1+ord.nelements();
  Vector<Int> refantchoices(nchoices,0);
  Vector<Int> r(refantchoices(IPosition(1,nUserRefant+1),IPosition(1,refantchoices.nelements()-1)));
  convertArray(r,ord);

  // set first two to primary preferred refant
  refantchoices(0)=refantchoices(1)=refantlist()(0);

  // set user's secondary refants (if any)
  if (nUserRefant>1) 
    refantchoices(IPosition(1,2),IPosition(1,nUserRefant))=
      refantlist()(IPosition(1,1),IPosition(1,nUserRefant-1));

  //cout << "refantchoices = " << refantchoices << endl;


  if (refantmode()=="strict") {
    nchoices=1;
    refantchoices.resize(1,True);
  }

  Vector<Int> nPol(nSpw(),nPar());  // TBD:or 1, if data was single pol

  if (nPar()==2) {
    // Verify that 2nd poln has unflagged solutions, PER SPW
    ROCTMainColumns ctmc(*ct_);

    Block<String> cols(1);
    cols[0]="SPECTRAL_WINDOW_ID";
    CTIter ctiter(*ct_,cols);
    Cube<Bool> fl;

    while (!ctiter.pastEnd()) {

      Int ispw=ctiter.thisSpw();
      fl.assign(ctiter.flag());

      IPosition blc(3,0,0,0), trc(fl.shape());
      trc-=1; trc(0)=blc(0)=1;
      
      //      cout << "ispw = " << ispw << " nfalse(fl(1,:,:)) = " << nfalse(fl(blc,trc)) << endl;
      
      // If there are no unflagged solutions in 2nd pol, 
      //   avoid it in refant calculations
      if (nfalse(fl(blc,trc))==0)
	nPol(ispw)=1;

      ctiter.next();      
    }
  }
  //  cout << "nPol = " << nPol << endl;

  Bool usedaltrefant(false);
  Int currrefant(refantchoices(0)), lastrefant(-1);

  Block<String> cols(2);
  cols[0]="SPECTRAL_WINDOW_ID";
  cols[1]="TIME";
  CTIter ctiter(*ct_,cols);

  // Arrays to hold per-timestamp solutions
  Cube<Complex> solA, solB;
  Cube<Bool> flA, flB;
  Vector<Int> ant1A, ant1B, ant2B;
  Matrix<Complex> refPhsr;  // the reference phasor [npol,nchan] 
  Int lastspw(-1);
  Bool first(true);
  while (!ctiter.pastEnd()) {
    Int ispw=ctiter.thisSpw();
    if (ispw!=lastspw) first=true;  // spw changed, start over

    // Read in the current sol, fl, ant1:
    solB.assign(ctiter.cparam());
    flB.assign(ctiter.flag());
    ant1B.assign(ctiter.antenna1());
    ant2B.assign(ctiter.antenna2()); 

    // First time thru, 'previous' solution same as 'current'
    if (first) {
      solA.reference(solB);
      flA.reference(flB);
      ant1A.reference(ant1B);
    }
    IPosition shB(solB.shape());
    IPosition shA(solA.shape());

    // Find a good refant at this time
    //  A good refant is one that is unflagged in all polarizations
    //     in the current(B) and previous(A) intervals (so they can be connected)
    Int irefA(0),irefB(0);  // index on 3rd axis of solution arrays
    Int ichoice(0);  // index in refantchoicelist
    Bool found(false);
    IPosition blcA(3,0,0,0),trcA(shA),blcB(3,0,0,0),trcB(shB);
    trcA-=1; trcA(0)=trcA(2)=0;
    trcB-=1; trcB(0)=trcB(2)=0;
    ichoice=0;
    while (!found && ichoice<nchoices) { 
      // Find index of current refant choice
      irefA=irefB=0;
      while (ant1A(irefA)!=refantchoices(ichoice) && irefA<shA(2)) ++irefA;
      while (ant1B(irefB)!=refantchoices(ichoice) && irefB<shB(2)) ++irefB;

      if (irefA<shA(2) && irefB<shB(2)) {

	//	cout << " Trial irefA,irefB: " << irefA << "," << irefB 
	//	     << "   Ants=" << ant1A(irefA) << "," << ant1B(irefB) << endl;

	blcA(2)=trcA(2)=irefA;
	blcB(2)=trcB(2)=irefB;
	found=true;  // maybe
	for (Int ipol=0;ipol<nPol(ispw);++ipol) {
	  blcA(0)=trcA(0)=blcB(0)=trcB(0)=ipol;
	  found &= (nfalse(flA(blcA,trcA))>0);  // previous interval
	  found &= (nfalse(flB(blcB,trcB))>0);  // current interval
	} 
      }
      else
	// irefA or irefB out-of-range
	found=false;  // Just to be sure

      if (!found) ++ichoice;  // try next choice next round

    }

    if (found) {
      // at this point, irefA/irefB point to a good refant
      
      // Keep track
      usedaltrefant|=(ichoice>0);
      currrefant=refantchoices(ichoice);
      refantchoices(1)=currrefant;  // 2nd priorty next time

      //      cout << " currrefant = " << currrefant << " (" << ichoice << ")" << endl;

      //      cout << " Final irefA,irefB: " << irefA << "," << irefB 
      //	   << "   Ants=" << ant1A(irefA) << "," << ant1B(irefB) << endl;


      // Only report if using an alternate refant
      if (currrefant!=lastrefant && ichoice>0) {
	logSink() 
	  << "At " 
	  << MVTime(ctiter.thisTime()/C::day).string(MVTime::YMD,7) 
	  << " ("
	  << "Spw=" << ctiter.thisSpw()
	  << ", Fld=" << ctiter.thisField()
	  << ")"
	  << ", using refant " << msmc().antennaName(currrefant)
	  << " (id=" << currrefant 
	  << ")" << " (alternate)"
	  << LogIO::POST;
      }  

      // Form reference phasor [nPar,nChan]
      Matrix<Complex> rA,rB;
      Matrix<Float> ampA,ampB;
      Matrix<Bool> rflA,rflB;
      rB.assign(solB.xyPlane(irefB));
      rflB.assign(flB.xyPlane(irefB));
      switch (this->type()) {
      case VisCal::G:
      case VisCal::B:
      case VisCal::T: {
	ampB=amplitude(rB);
	rflB(ampB<FLT_EPSILON)=true; // flag... 
	rB(rflB)=Complex(1.0);       //  ...and reset zeros
	ampB(rflB)=1.0;
	rB/=ampB;  // rB now normalized ("phase"-only)
	break;
      }
      case VisCal::D: {
	// Fill 2nd pol with negative conj of 1st pol
	rB.row(1)=-conj(rB.row(0));
	break;
      }
      default:
	throw(AipsError("SVC::applyRefAnt attempted for inapplicable type"));
      }

      if (!first) {
	// Get and condition previous phasor for the current refant
	rA.assign(solA.xyPlane(irefA));
	rflA.assign(flA.xyPlane(irefA));
	switch (this->type()) {
	case VisCal::G:
	case VisCal::B:
	case VisCal::T: {
	  ampA=amplitude(rA);
	  rflA(ampA<FLT_EPSILON)=true;  // flag...
	  rA(rflA)=Complex(1.0);        // ...and reset zeros
	  ampA(rflA)=1.0;
	  rA/=ampA; // rA now normalized ("phase"-only)
	
	  // Phase difference (as complex) relative to last
	  rB/=rA;
	  break;
	}
	case VisCal::D: {
	  // 
	  rA.row(1)=-conj(rA.row(0));

	  // Complex difference relative to last
	  rB-=rA;
	  break;
	}
	default:
	  throw(AipsError("SVC::applyRefAnt attempted for inapplicable type"));
	}

	// Accumulate flags
	rflB&=rflA;
      }
      
      //      cout << " rB = " << rB << endl;
      //      cout << boolalpha << " rflB = " << rflB << endl;
      // TBD: fillChanGaps?
      
      // Now apply reference phasor to all antennas
      Matrix<Complex> thissol;
      for (Int iant=0;iant<shB(2);++iant) {
	thissol.reference(solB.xyPlane(iant));
	switch (this->type()) {
	case VisCal::G:
	case VisCal::B:
	case VisCal::T: {
	  // Complex division == phase subtraction
	  thissol/=rB;
	  break;
	}
	case VisCal::D: {
	  // Complex subtraction
	  thissol-=rB;
	  break;
	}
	default:
	  throw(AipsError("SVC::applyRefAnt attempted for inapplicable type"));
	}
      }
      
      // Set refant, so we can put it back
      ant2B=currrefant;
      
      // put back referenced solutions
      ctiter.setcparam(solB);
      ctiter.setantenna2(ant2B);

      // Next time thru, solB is previous
      solA.reference(solB);
      flA.reference(flB);
      ant1A.reference(ant1B);
      solB.resize();  // (break references)
      flB.resize();
      ant1B.resize();
	
      lastrefant=currrefant;
      first=false;  // avoid first-pass stuff from now on
      
    } // found
    else {
      logSink() 
	<< "At " 
	<< MVTime(ctiter.thisTime()/C::day).string(MVTime::YMD,7) 
	<< " ("
	<< "Spw=" << ctiter.thisSpw()
	<< ", Fld=" << ctiter.thisField()
	<< ")"
	<< ", refant (id=" << currrefant 
	<< ") was flagged; flagging all antennas strictly." 
	<< LogIO::POST;
      // Flag all solutions in this interval
      flB.set(True);
      ctiter.setflag(flB);
    }

    // advance to the next interval
    lastspw=ispw;
    ctiter.next();
  }

  if (usedaltrefant)
    logSink() << LogIO::NORMAL
	      << " NB: An alternate refant was used at least once to maintain" << endl
	      << "  phase continuity where the user's preferred refant drops out." << endl
	      << "  Alternate refants are held constant in phase (_not_ zeroed)" << endl
	      << "  during these periods, and the preferred refant may return at" << endl
	      << "  a non-zero phase.  This is generally harmless."
	      << LogIO::POST;

  return;

}


void SolvableVisJones::fluxscale(const String& outfile,
                                 const Vector<Int>& refFieldIn,
				 const Vector<Int>& tranFieldIn,
				 const Vector<Int>& inRefSpwMap,
				 const Vector<String>& fldNames,
                                 const Float& inGainThres,
                                 const String& antSel,
                                 const String& timerangeSel,
                                 const String& scanSel,
				 fluxScaleStruct& oFluxScaleStruct,
				 const String& oListFile,
                                 const Bool& incremental,
                                 const Int& fitorder, 
                                 const Bool& display) {

  //String outCalTabName="_tmp_testfluxscaletab";
  String outCalTabName=outfile;

  //timerange
  //String timerange("");
  //String scanSel("");
  // turn on incremental caltable mode
  //Bool incremental = true;
  //Bool fitperchan = true;

  // threshold for gain (amplitude) to be used in 
  // fluxscale determination
  // -1: no threshold
  // plot histogram of gain ratio
  Bool report_p=display;

  if (incremental) {
    logSink() << LogIO::NORMAL
              << "Generating an incremental caltable"
              << LogIO::POST;
  }

  if (!ct_ || ct_->nrow()<1)
    throw(AipsError("SVJ:fluxscale: Empty or absent caltable specified"));

  // For updating the MS History Table
  //  LogSink logSink_p = LogSink(LogMessage::NORMAL, false);
  //  logSink_p.clearLocally();
  //  LogIO oss(LogOrigin("calibrater", "fluxscale()"), logSink_p);

  // PtrBlocks to hold mean gain moduli and related
  PtrBlock< Cube<Bool>* >   MGOK;
  PtrBlock< Cube<Double>* > MG;
  PtrBlock< Cube<Double>* > MG2;
  PtrBlock< Cube<Double>* > MGWT;
  PtrBlock< Cube<Double>* > MGVAR;
  PtrBlock< Cube<Int>* >    MGN;
  PtrBlock< Cube<Int>* >    MGNALL;

  Int nMSFld; fldNames.shape(nMSFld);

  // Assemble available field list from the NewCalTable
  ROCTMainColumns mcols(*ct_);
  Vector<Int> fldList;
  mcols.fieldId().getColumn(fldList);
  Int nFldList=genSort(fldList,Sort::Ascending,(Sort::QuickSort | Sort::NoDuplicates));
  fldList.resize(nFldList,true);

  Int nFld=max(fldList)+1;

  //get Antenna names
  MSAntennaColumns antcol(ct_->antenna());
  Vector<String> antNames(antcol.name().getColumn());

  Vector<Double> solFreq(nSpw(),-1.0);
  Vector<Double> mgreft(nFld,0);
 

  try {

    // Resize, NULL-initialize PtrBlocks
    MGOK.resize(nFld);   MGOK=NULL;
    MG.resize(nFld);     MG=NULL;
    MG2.resize(nFld);    MG2=NULL;
    MGWT.resize(nFld);   MGWT=NULL;
    MGVAR.resize(nFld);  MGVAR=NULL;
    MGN.resize(nFld);    MGN=NULL;
    MGNALL.resize(nFld); MGNALL=NULL;

    // sort user-specified fields
    Vector<Int> refField; refField = refFieldIn;
    Int nRef,nTran;
    nRef=genSort(refField,Sort::Ascending,(Sort::QuickSort | Sort::NoDuplicates));
    // temp copy of tranFieldIn
    std::vector<Int> tmpTranField;
    tranFieldIn.tovector(tmpTranField);
    for (Int iRef=0; iRef<nRef; iRef++) {
        auto iidx = std::find(tmpTranField.begin(),tmpTranField.end(),refField(iRef));
        if (iidx != tmpTranField.end()) { 
          logSink() << "The reference field, "<<fldNames(*iidx)
                    << " , is also listed in the transfer fields. "
                    <<" It will be ignored for further scaling process."
	             << LogIO::POST;
          tmpTranField.erase(iidx);
        }
    }
    //Vector<Int> tranField; tranField = tranFieldIn;
    Vector<Int> tranField(tmpTranField);
    nTran=genSort(tranField,Sort::Ascending,(Sort::QuickSort | Sort::NoDuplicates));

    // make masks for ref/tran among available fields
    Vector<Bool> tranmask(nFldList,true);
    Vector<Bool> refmask(nFldList,false);
    for (Int iFld=0; iFld<nFldList; iFld++) {
      if ( anyEQ(refField,fldList(iFld)) ) {
        // this is a ref field
        refmask(iFld)=true;
        tranmask(iFld)=false;
      }
    }

    // Check availability of all ref fields
    if (ntrue(refmask)==0) {
      throw(AipsError(" Cannot find specified reference field(s)"));
    }
    // Any fields present other than ref fields?
    if (ntrue(tranmask)==0) {
      throw(AipsError(" Cannot find solutions for transfer field(s)"));
    }

    // make implicit reference field list
    MaskedArray<Int> mRefFldList(fldList,LogicalArray(refmask));
    Vector<Int> implRefField(mRefFldList.getCompressedArray());

    // Check for missing reference fields
    if (Int(ntrue(refmask)) < nRef) {
      ostringstream x;
      for (Int iRef=0; iRef<nRef; iRef++) {
        if ( !anyEQ(fldList,refField(iRef)) ) {
          if (refField(iRef)>-1 && refField(iRef) < nMSFld) x << fldNames(refField(iRef)) << " ";
          else x << "Index="<<refField(iRef)+1<<"=out-of-range ";
        }
      }
      String noRefSol=x.str();
      logSink() << LogIO::WARN
		<< " The following reference fields have no solutions available: "
		<< noRefSol
		<< LogIO::POST;
      refField.reference(implRefField);
    }
    refField.shape(nRef);

    // make implicit tranfer field list
    MaskedArray<Int> mTranFldList(fldList,LogicalArray(tranmask));
    Vector<Int> implTranField(mTranFldList.getCompressedArray());
    Int nImplTran; implTranField.shape(nImplTran);

    //    cout << "implTranField = " << implTranField << endl;

    // Check availability of transfer fields

    // If user specified no transfer fields, use implicit 
    //  transfer field list, ELSE check for missing tran fields
    //  among those they specified
    if (nTran==0) {
      tranField.reference(implTranField);
      logSink() << LogIO::NORMAL
		<< " Assuming all non-reference fields are transfer fields."
		<< LogIO::POST;
    } else {
      if ( !(nTran==nImplTran &&
             allEQ(tranField,implTranField)) ) {
        ostringstream x;
        for (Int iTran=0; iTran<nTran; iTran++) {
          if ( !anyEQ(implTranField,tranField(iTran)) ) {
            if (tranField(iTran)>-1 && tranField(iTran) < nMSFld) x << fldNames(tranField(iTran)) << " ";
            else x << "Index="<<tranField(iTran)+1<<"=out-of-range ";
          }
        }
        String noTranSol=x.str();
        if (x!="") {
	logSink() << LogIO::WARN
		  << " The following transfer fields have no solutions available: "
		  << noTranSol
		  << LogIO::POST;
        }
        //tranField.reference(implTranField);
      }
    }
    tranField.shape(nTran);
  
    // make a combined field list 
    std::vector<Int> allfields(refField.begin(), refField.end());
    allfields.insert(allfields.end(), tranField.begin(), tranField.end());
    
    // Report ref, tran field info
    String refNames(fldNames(refField(0)));
    for (Int iRef=1; iRef<nRef; iRef++) {
      refNames+=" ";
      refNames+=fldNames(refField(iRef));
    }
    logSink() << " Found reference field(s): " << refNames 
	      << LogIO::POST;
    String tranNames(fldNames(tranField(0)));
    for (Int iTran=1; iTran<nTran; iTran++) {
      tranNames+=" ";
      tranNames+=fldNames(tranField(iTran));
    }
    logSink() << " Found transfer field(s):  " << tranNames 
	      << LogIO::POST;

    // Handle spw referencing
    Vector<Int> refSpwMap;
    refSpwMap.resize(nSpw());
    indgen(refSpwMap);

    if (inRefSpwMap(0)>-1) {
      if (inRefSpwMap.nelements()==1) {
        refSpwMap=inRefSpwMap(0);
        logSink() << " All spectral windows will be referenced to spw=" << inRefSpwMap(0) 
		  << LogIO::POST;
      } else {
        for (Int i=0; i<Int(inRefSpwMap.nelements()); i++) {
          if (inRefSpwMap(i)>-1 && inRefSpwMap(i)!=i) {
            refSpwMap(i)=inRefSpwMap(i);
	    logSink() << " Spw=" << i << " will be referenced to spw=" << inRefSpwMap(i) 
		      << LogIO::POST;
          }
        }
      }
    }
 
    // Field names for log messages

    //    cout << "Filling mgnorms....";

    Matrix<Float> medianGains(nFld,nSpw(),0.0); //keeps median (amplitude) gains for each field for each spw
    { // make an inner scope
    Block<String> cols(4);
    cols[0]="SPECTRAL_WINDOW_ID";
    cols[1]="TIME";
    cols[2]="FIELD_ID"; // should usually be degenerate with TIME?
    cols[3]="ANTENNA1";
    //ROCTIter ctiter(*ct_,cols);

    // Loop over solutions and fill the calculation
    Cube<Bool>   mgok;   // For referencing PtrBlocks...
    Cube<Double> mg;  
    Cube<Double> mg2; 
    Cube<Double> mgwt;   
    Cube<Int>    mgn;    
    Cube<Int>    mgnall;    
    Int prevFld(-1);

    Int lastFld(-1);

    // determine median gain amplitude for each field 
    // use CTinterface to appy selection with MSSelection syntax
    NewCalTable selct(*ct_);
    CTInterface cti(*ct_);
    Vector<Int> selAntList;
    Vector<Int> deselAntList;
    Vector<Int> allAntList(nElem());
    indgen(allAntList);
    vector<Int> tmpAllAntList(allAntList.begin(),allAntList.end());
    // 
    // Check if antenna specific time/scan selection is needed.
    // If so get selected time col for later use to flag the data. 
    // The negation '!' is used as a keyword to trigger such a selection mode. 
    Bool doPerAntSel(false);
    Vector<Double> selTime;
    if (antSel!="" ) {
      if (antSel.contains(casacore::Regex("^!")) && (timerangeSel!="" || scanSel!="")) {
        doPerAntSel = true;
      // if doPerAntSel time/scan sel only applied to deselected ant in antSel
      // so need to construct selected table based on that
        MSSelection msssub;
        String antsel=antSel;
        antsel.ltrim('!');
        msssub.setAntennaExpr(antsel);
        if (timerangeSel != "") msssub.setTimeExpr(timerangeSel);
        if (scanSel != "") msssub.setScanExpr(scanSel);
        TableExprNode tensub=msssub.toTableExprNode(&cti);
        getSelectedTable(selct,*ct_,tensub,"");
        ROCTMainColumns ctmc(selct);
        selTime=ctmc.time().getColumn();
        deselAntList=ctmc.antenna1().getColumn();
	Int ndeselAnt=genSort(deselAntList,Sort::Ascending,(Sort::QuickSort | Sort::NoDuplicates));
	deselAntList.resize(ndeselAnt,true);
      }
    }         

    Bool firstpass(true);
    for (Int iFld=0; iFld<nFld; iFld++) {

      MSSelection mss;
      //String antSel("!ea25");
      //String antSel("0~26");
      //String antSel("");
      mss.setFieldExpr(String::toString(iFld));     
      if (antSel!="") {
        if (!doPerAntSel) {
          // applied selections globally 
          mss.setAntennaExpr(antSel);
          if (timerangeSel!="")  mss.setTimeExpr(timerangeSel);
          if (scanSel!="") mss.setScanExpr(scanSel);
        }
      } else {
        selAntList=allAntList;
      } 
      std::vector<Int> tmpSelAntList;
      Vector<Bool> validSels(nSpw(),true);
      for (Int iSpw=0; iSpw<nSpw(); iSpw++) {
        //reset MSSelection
        mss.clear(MSSelection::SPW_EXPR);
        //mss.setFieldExpr(String::toString(iFld));     
        mss.setSpwExpr(String::toString(iSpw));     
        try {
          TableExprNode ten=mss.toTableExprNode(&cti);
          getSelectedTable(selct,*ct_,ten,"");
          ROCTMainColumns ctmc(selct);
          Array<Float> outparams;
          Array<Bool> flagcol=ctmc.flag().getColumn();
          Vector<Double> timecol=ctmc.time().getColumn();
          Vector<Int> antenna1=ctmc.antenna1().getColumn();
          // Do antenna-specific the selections in time 
          IPosition flshp = flagcol.shape();
          if (doPerAntSel && selTime.nelements()!=0) {
            for (uInt ifg = 0; ifg < flshp(2); ifg++) {
              for (Int ipar = 0; ipar < nPar(); ipar++) {
                if (anyEQ(deselAntList, antenna1(ifg)) && !anyEQ(selTime, timecol(ifg)))
                  //outflag(IPosition(3,ipar,0,ifg))=false;
                  flagcol(IPosition(3,ipar,0,ifg))=true;
              }
            }
          }
          // reverse the flag for a masked array
          LogicalArray outflag(!flagcol);
          //cerr<<"ntrue outflag ="<<ntrue(outflag)<<endl;
          ctmc.fparamArray(outparams,"AP");
          // get subset (amp only) of the array
          IPosition arshp = outparams.shape();
          IPosition start(3,0,0,0);
          Int pinc = nPar()!=1 ? 2: 1;
          Int pshp = arshp(0)!=1 ? Int(arshp(0)/2) : 1; 
          //IPosition length(3,Int(arshp(0)/pinc),1,arshp(arshp.nelements()-1));
          IPosition length(3,pshp,1,arshp(arshp.nelements()-1));
          IPosition stride(3,pinc,1,1);
          Slicer slicer(start,length,stride);
          Array<Float> subarr=outparams(slicer);
          MaskedArray<Float> moutparams(subarr,outflag);
          //Vector<Float> subarr2 = moutparams.getCompressedArray();
          // Get selected antenna list in ant ids.
          // While it is applied to all fields and spws
          // the actual selections happen per spw. So we do it here but only for the first
          // successful selection for the data and use it for the rest of the process.
          if (antSel!="" && firstpass) {
            Vector<Int> selantlist=ctmc.antenna1().getColumn();
	    Int nSelAnt=genSort(selantlist,Sort::Ascending,(Sort::QuickSort | Sort::NoDuplicates));
	    selantlist.resize(nSelAnt,true);
            selAntList=selantlist;
            //cerr<<"selantlist.nelements()="<<selantlist.nelements()<<endl;
            String oMsg( "" );
            if ( doPerAntSel ) {
              oMsg+="Selected data range for antenna(s): "+String::toString(deselAntList)+",";
              if ( timerangeSel!="") oMsg+= " time range:"+timerangeSel;
              if ( scanSel!="") oMsg+= " scan(s):"+scanSel;
            }
            else { 
              oMsg+=" Selected antennas: "+String::toString(selAntList);
            }
            logSink() << oMsg << LogIO::POST;
            firstpass=false;
          }

          //medianGains(iFld,iSpw)=median(subarr2);
          medianGains(iFld,iSpw)=median(moutparams);
          String oMsg( "" );
          oMsg+="median(field="+String::toString(iFld)+",spw="+String::toString(iSpw)+")="+\
             String::toString(medianGains(iFld,iSpw));
          logSink() << LogIO::NORMAL3<< oMsg << LogIO::POST;
        } catch (...) {
         if (anyEQ(tranField,iFld)) validSels[iSpw]=false;
        }
      }
      if (!ntrue(validSels)) 
            throw(AipsError("The input selections results in empy data selection for the transfer field="
                  +String::toString(iFld)));
    } //for-iFld 
    NewCalTable selct2(*ct_);
    CTInterface cti2(*ct_);
    MSSelection mss2;
    String fieldstr;
    for (uInt iselfld=0; iselfld<allfields.size();iselfld++) {
      fieldstr+=String::toString(allfields[iselfld]);
      if (iselfld!=allfields.size()-1) fieldstr+=',';
    } 
    mss2.setFieldExpr(fieldstr);
    if (!doPerAntSel) {
      mss2.setAntennaExpr(antSel);
      mss2.setTimeExpr(timerangeSel);
      mss2.setScanExpr(scanSel);
    }
    TableExprNode ten2=mss2.toTableExprNode(&cti2);
    getSelectedTable(selct2,*ct_,ten2,"");

    ROCTIter ctiter(selct2,cols);
    while (!ctiter.pastEnd()) {
      Int iSpw(ctiter.thisSpw());
      Int iFld(ctiter.thisField());
      Int iAnt(ctiter.thisAntenna1());
      //refTime_ = ctiter.thisTime();
      if (iFld > prevFld) {
        mgreft = ctiter.thisTime();
      }
      prevFld = iFld;

      if (solFreq(iSpw)<0.0) {
	Vector<Double> freq;
	ctiter.freq(freq);
	uInt nFrq=freq.nelements();
	solFreq(iSpw)=freq(nFrq/2);
      }
	
      if (MGOK[iFld]==NULL) {
	// First time this field, allocate ant/spw matrices
	MGOK[iFld]   = new Cube<Bool>(nPar(),nElem(),nSpw(),false);
	MG[iFld]     = new Cube<Double>(nPar(),nElem(),nSpw(),0.0);
	MG2[iFld]    = new Cube<Double>(nPar(),nElem(),nSpw(),0.0);
	MGWT[iFld]   = new Cube<Double>(nPar(),nElem(),nSpw(),0.0);
	MGVAR[iFld]  = new Cube<Double>(nPar(),nElem(),nSpw(),0.0);
	MGN[iFld]    = new Cube<Int>(nPar(),nElem(),nSpw(),0);
        // for reporting numbers of solution used
        MGNALL[iFld] = new Cube<Int>(nPar(),nElem(),nSpw(),0);
	
      }
      // References to PBs for syntactical convenience
      //  TBD: should need to do this for each iteration (nAnt times redundant!)
      if (iFld!=lastFld) {
	mgok.reference(*(MGOK[iFld]));
	mg.reference(*(MG[iFld]));
	mg2.reference(*(MG2[iFld]));
	mgwt.reference(*(MGWT[iFld]));
	mgn.reference(*(MGN[iFld]));
        mgnall.reference(*(MGNALL[iFld]));
      }

      // References to PBs for syntactical convenience
      // TBD: Handle "iFitwt" from NewCalTable?
      // Double wt=cs().iFitwt(iSpw)(iAnt,islot);
      Double wt=1;
	      
      // amps, flags  [npar]  (one channel, one antenna)
      // check for data shape (e.g. duplicated entries)
      Cube<Complex> CParam(ctiter.cparam());
      IPosition testShape=CParam.shape();
      if (testShape[2]!=1) {
        if (testShape[2]>1) { 
          // possible cause: append=true in gaincal
          throw(AipsError("Found multiple gain solutions in a single timestamp for fieldid="+String::toString(iFld)+". Please check the input Caltable."));
        }
        else {
          throw(AipsError("Found gain solution array shape, "+String::toString(testShape)+" while expected [2,1,1]. Please check the input Caltable."));
        }
      } 
      //Vector<Float> amp(amplitude(ctiter.cparam())[0]);
      Vector<Float> amp(amplitude(CParam));
      Vector<Bool> fl(ctiter.flag());
      for (Int ipar=0; ipar<nPar(); ipar++) {
	if (!fl(ipar)) {
	  Double gn=amp(ipar); // converts to Double
          // evaluate input gain to be within the threshold
          Float lowbound= (inGainThres >0 and inGainThres <1.0)? 1.0 - inGainThres : 0.0;
          if (inGainThres==0) lowbound=1.0;
          //if ((anyEQ(selAntList,iAnt) &&  && (inGainThres < 0 || 
          if (inGainThres < 0 || 
           //     (gn >= lowbound*medianGains(iFld,iSpw)))  { 
           // take both lower and upper bounds
              (gn >= lowbound*medianGains(iFld,iSpw)  and 
               gn <= (1.0 + inGainThres) * medianGains(iFld,iSpw)))  {
              
            if (doPerAntSel && selTime.nelements() != 0) {
               if (anyEQ(deselAntList,iAnt) && !anyEQ(selTime,ctiter.thisTime())) {
                  logSink()<<LogIO::NORMAL3<<"skipped "<<ctiter.thisTime()<<" for iAnt="<<iAnt<<LogIO::POST;
                  continue;
               }
            } 
            //cerr<<"fld="<<iFld<<" spw="<<iSpw<<" ant="<<iAnt<<" gn="<<gn<<" median="<<medianGains(iFld,iSpw)<<endl;
	    mgok(ipar,iAnt,iSpw)=true;
	    mg(ipar,iAnt,iSpw) += (wt*gn);
	    mg2(ipar,iAnt,iSpw)+= (wt*gn*gn);
	    mgn(ipar,iAnt,iSpw)++;
	    mgwt(ipar,iAnt,iSpw)+=wt;
          }
          else {
            String debugMsg( "" ); 
            debugMsg+="Rejected field="+String::toString(iFld)+" spw="+String::toString(iSpw)+" antenna="+String::toString(iAnt)+
                      "; gain(amp)="+String::toString(gn)+" is outside the accepted range:"+String::toString(lowbound*medianGains(iFld,iSpw))+
                      " ~ "+String::toString((1.0 + inGainThres) * medianGains(iFld,iSpw));
            logSink() << LogIO::DEBUG1 << debugMsg << LogIO::POST;
          }
          mgnall(ipar,iAnt,iSpw)++;
	}
      }
      lastFld=iFld;
      ctiter.next();
    }//end of while
    } // end inner scope 
   
    //for reporting only
    if (inGainThres>=0.0) {
      String oMsg( "" );
      oMsg+=" Applying gain threshold="+String::toString(inGainThres);
      logSink() << oMsg << LogIO::POST;
      Bool hasFlaggedData(false);
      //for (Int iFld=0; iFld<nFld; iFld++) {
      Int iFld;
      for (int idx=0; idx < (int) allfields.size(); idx++) {
        iFld = allfields[idx];
        if (MGOK[iFld]!=NULL) {
          Cube<Bool>    mgok;   mgok.reference(*(MGOK[iFld]));
          Cube<Int>    mgn;    mgn.reference(*(MGN[iFld]));
          Cube<Int>    mgnall;    mgnall.reference(*(MGNALL[iFld]));
          //cerr<<"ntrue="<<ntrue(mgok)<< " shape="<<mgok.shape()<<endl;
          //cerr<<"mgn shape="<<mgn.shape()<<endl;
          //cerr<<"mgnall shape="<<mgnall.shape()<<endl;
          for (Int iSpw=0; iSpw<nSpw(); iSpw++) {
            //cerr<<"median gain="<<medianGains(iFld,iSpw)<<endl;
            for (Int iAnt=0; iAnt<nElem(); iAnt++) {
              IPosition start(3,0,iAnt,iSpw);
              IPosition length(3,nPar(),1,1);
              IPosition stride(3,1,1,1);
              Slicer slicer(start,length,stride);
              //if (ntrue(mgok(slicer))) {
              if ( sum(mgn(slicer))<sum(mgnall(slicer)) ) {
                //cerr<<"iFld:"<<iFld<<" iAnt:"<<iAnt<<"  sum(mgn)="<<sum(mgn(slicer))<<" sum(mgnall(slicer))="<<sum(mgnall(slicer))<<endl;
                Float frac=Float (sum(mgn(slicer)))/Float (sum(mgnall(slicer)));
                ostringstream fracstream;
                fracstream.precision(3);
                if (frac<1.0) {
                  // report a fraction flagged
                  fracstream << (1.0-frac)*100.0;
                  oMsg="";
                  //cerr<<"iFld="<<iFld<<" iAnt="<<iAnt<<": "<<frac*100.0<<"% of "<<sum(mgnall(slicer))<<" will be excluded"<<endl;
                  //oMsg+="  Field ID="+String::toString(tranField(iFld))+" Antenna ID="+String::toString(iAnt)+": ";
                  //oMsg+="  Field ID="+String::toString(allfields[idx])+" Antenna ID="+String::toString(iAnt)+": ";
                  oMsg+="  "+fldNames(allfields[idx])+"(id="+String::toString(allfields[idx])+") Antenna:"+antNames(iAnt)+"(id="+String::toString(iAnt)+"): ";
                  oMsg+=fracstream.str()+" % of ";
                  //oMsg+=String::toString(sum(mgnall(slicer)) )+" solutions will be used";
                  oMsg+=String::toString(sum(mgnall(slicer)) )+" solution(s) will be excluded";
                  logSink() << oMsg << LogIO::POST;
                }
                hasFlaggedData=true;
              }//ntrue()
            } //iAnt 
          } //iSpw
        }
      }//iFld
      if (!hasFlaggedData) {
        oMsg=" None of the gains were exceeded the threshold.";
        logSink() << oMsg << LogIO::POST;
      }
    }//gainthreshold 
  /*

    // fill per-ant -fld, -spw  mean gain moduli
    for (Int iSpw=0; iSpw<nSpw(); iSpw++) {

      if (cs().nTime(iSpw) > 0 ) {

        for (Int islot=0; islot<cs().nTime(iSpw); islot++) {
          Int iFld=cs().fieldId(iSpw)(islot);
          if (MGOK[iFld]==NULL) {
            // First time this field, allocate ant/spw matrices
            MGOK[iFld]   = new Cube<Bool>(nPar(),nElem(),nSpw(),false);
            MG[iFld]     = new Cube<Double>(nPar(),nElem(),nSpw(),0.0);
            MG2[iFld]    = new Cube<Double>(nPar(),nElem(),nSpw(),0.0);
            MGWT[iFld]   = new Cube<Double>(nPar(),nElem(),nSpw(),0.0);
            MGVAR[iFld]  = new Cube<Double>(nPar(),nElem(),nSpw(),0.0);
            MGN[iFld]    = new Cube<Int>(nPar(),nElem(),nSpw(),0);

          }
          // References to PBs for syntactical convenience
          Cube<Bool>   mgok;   mgok.reference(*(MGOK[iFld]));
          Cube<Double> mg;  mg.reference(*(MG[iFld]));
          Cube<Double> mg2; mg2.reference(*(MG2[iFld]));
          Cube<Double> mgwt;   mgwt.reference(*(MGWT[iFld]));
          Cube<Int>    mgn;    mgn.reference(*(MGN[iFld]));

          for (Int iAnt=0; iAnt<nElem(); iAnt++) {
	    if (true) { // || antmask(iAnt)) {
	      Double wt=cs().iFitwt(iSpw)(iAnt,islot);
	      
	      for (Int ipar=0; ipar<nPar(); ipar++) {
		IPosition ip(4,ipar,0,iAnt,islot);
		if (cs().parOK(iSpw)(ip)) {
		  Double gn=abs( cs().par(iSpw)(ip) );
		  mgok(ipar,iAnt,iSpw)=true;
		  mg(ipar,iAnt,iSpw) += (wt*gn);
		  mg2(ipar,iAnt,iSpw)+= (wt*gn*gn);
		  mgn(ipar,iAnt,iSpw)++;
		  mgwt(ipar,iAnt,iSpw)+=wt;
		}
	      }
	    }
	  }
        }
      }
    }
  */

    //    cout << "done." << endl;

    //    cout << "Normalizing mgs...";


    // normalize mg
    for (Int iFld=0; iFld<nFld; iFld++) {

      //      cout << "iFld = " << iFld << " " << MGOK[iFld]->column(0) << endl;

      // Have data for this field?
      if (MGOK[iFld]!=NULL) {
        // References to PBs for syntactical convenience
        Cube<Bool>   mgok;   mgok.reference(*(MGOK[iFld]));
        Cube<Double> mg;  mg.reference(*(MG[iFld]));
        Cube<Double> mg2; mg2.reference(*(MG2[iFld]));
        Cube<Double> mgwt;   mgwt.reference(*(MGWT[iFld]));
        Cube<Double> mgvar;  mgvar.reference(*(MGVAR[iFld]));
        Cube<Int>    mgn;    mgn.reference(*(MGN[iFld]));

        for (Int iSpw=0; iSpw<nSpw(); iSpw++) {
          for (Int iAnt=0; iAnt<nElem(); iAnt++) {
	    for (Int ipar=0;ipar<nPar(); ++ipar) {
	      if ( mgok(ipar,iAnt,iSpw) && mgwt(ipar,iAnt,iSpw)>0.0 ) {
		mg(ipar,iAnt,iSpw)/=mgwt(ipar,iAnt,iSpw);
		mg2(ipar,iAnt,iSpw)/=mgwt(ipar,iAnt,iSpw);
		// Per-ant, per-spw variance (non-zero only if sufficient data)
		if (mgn(ipar,iAnt,iSpw) > 2) {
		  mgvar(ipar,iAnt,iSpw) = (mg2(ipar,iAnt,iSpw) - pow(mg(ipar,iAnt,iSpw),2.0))/(mgn(ipar,iAnt,iSpw)-1);
		}
	      } else {
		mg(ipar,iAnt,iSpw)=0.0;
		mgwt(ipar,iAnt,iSpw)=0.0;
		mgok(ipar,iAnt,iSpw)=false;
	      }
	    }
   /*
            cout << " iSpw = " << iSpw << " iFld = " << iFld;
            cout << " iAnt = " << iAnt;
            cout << " mg = " << mg(iAnt,iSpw);
            cout << " +/- " << sqrt(1.0/mgwt(iAnt,iSpw));
            cout << " SNR = " << mg(iAnt,iSpw)/sqrt(1.0/mgwt(iAnt,iSpw));
            cout << "  " << mgn(iAnt,iSpw);
            cout << endl;
   */
          }
        }

      } //if-MGOK end
    } 


    //    cout << "done." << endl;
    //    cout << "nTran = " << nTran << endl;

    //    cout << "Calculating scale factors...";

    // Collapse ref field mg's into a single ref
    Cube<Double> mgref;
    Cube<Bool>  mgrefok;
    mgref.reference(*MG[refField(0)]);
    mgrefok.reference(*MGOK[refField(0)]);

    if (nRef>1) {
      //Store no. of fields that is not flagged per pol,ant,and spw
      Cube<Double> nokref(nPar(),nElem(),nSpw(),0);
      // Add on additional ref fields
      for (Int iref=1;iref<nRef;++iref) {

	Cube<Bool> mgokR; mgokR.reference(*MGOK[refField(iref)]);
	Cube<Double> mgR;   mgR.reference(*MG[refField(iref)]);

	for (Int ispw=0;ispw<nSpw();++ispw) {
	  for (Int iant=0;iant<nAnt();++iant) {
	    for (Int ipar=0;ipar<nPar();++ipar) {
              if (iref==1) {
	        if (mgrefok(ipar,iant,ispw)) {
                  nokref(ipar,iant,ispw)+=1.0;   
                }
                else {// the first ref field of this gain is flagged
                  mgref(ipar,iant,ispw)=0.0;
                }
              }

              if (mgokR(ipar,iant,ispw)) {
	        mgref(ipar,iant,ispw)+=mgR(ipar,iant,ispw);
                nokref(ipar,iant,ispw)+=1.0;   
              }

              // at the last ref field...
              if(iref==nRef-1 ) {
                if (nokref(ipar,iant,ispw)==0.0) {
	          mgrefok(ipar,iant,ispw)=false;
                  mgref(ipar,iant,ispw)=0.0;
                } 
                else {
                  // overwrite to turn to true for the case of mgrefok=false for refField(0)
                  mgrefok(ipar,iant,ispw)=true;
                }
              }

              // Replaced this with above to support flagged ref field case(CAS-4758) - TT
	      //if (mgrefok(ipar,iant,ispw) && mgokR(ipar,iant,ispw))
	      //  mgref(ipar,iant,ispw)+=mgR(ipar,iant,ispw);
	      // else {
	      //  mgrefok(ipar,iant,ispw)=false;
              //  mgref(ipar,iant,ispw)=0.0;
	      //} 

	    } // ipar
	  } // iant
	} // ispw
      } // iref
      // Complete the average:
      //mgref/=Double(nRef);
      mgref/=nokref; // only count unflagged ones
    } // nRef > 1

    // Scale factor calculation, per trans fld, per spw
    Matrix<Double> fd( nSpw(), nFld, -1.0 );
    Matrix<Double> fderr( nSpw(), nFld, -1.0 );
    Matrix<Double> fdrms(nSpw(),nFld,-1.0);
    Matrix<Int> numSol( nSpw(), nFld, -1 );
//    fd.resize(nSpw(),nFld);
//    fd.set(-1.0);
//    fderr.resize( nSpw(), nFld );
//    fderr.set( -1.0 );
//    numSol.resize( nSpw() );
//    numSol.set( -1 );

    Matrix<Bool> scaleOK(nSpw(),nFld,false);
    Matrix<Double> mgratio(nSpw(),nFld,-1.0);
    Matrix<Double> mgrms(nSpw(),nFld,-1.0);
    Matrix<Double> mgerr(nSpw(),nFld,-1.0);

    for (Int iTran=0; iTran<nTran; iTran++) {
      
      Int tranidx=tranField(iTran);
      if (MGOK[tranidx]!=NULL) {
      // References to PBs for syntactical convenience
      Cube<Bool>   mgokT;  mgokT.reference(*(MGOK[tranidx]));
      Cube<Double> mgT;    mgT.reference(*(MG[tranidx]));
      Cube<Double> mgvarT; mgvarT.reference(*(MGVAR[tranidx]));
      Cube<Double> mgwtT;  mgwtT.reference(*(MGWT[tranidx]));
      if (report_p) {
        setupPlotter();
      }
      int countvalidspw = 0;
      for (Int ispw=0; ispw<nSpw(); ispw++) {
        // Reference spw may be different
        Int refSpw(refSpwMap(ispw));
        
        // Only if anything good for this spw
        if (ntrue(mgokT.xyPlane(ispw)) > 0) {

	  for (Int iant=0;iant<nAnt();++iant) {
	    for (Int ipar=0;ipar<nPar();++ipar) {
	      if (mgokT(ipar,iant,ispw) && 
		  mgrefok(ipar,iant,refSpw) &&
		  mgref(ipar,iant,refSpw)>0.0 ) {
		mgT(ipar,iant,ispw)/=mgref(ipar,iant,refSpw);
	      }
	      else {
		mgT(ipar,iant,ispw)=0.0;
		mgokT(ipar,iant,ispw)=false;
	      }
	    } // ipar
	  } // iant
	} // ntrue>0

	// Form the mean gain ratio
	Matrix<Double> mgTspw(mgT.xyPlane(ispw));
	Matrix<Bool> mgokTspw(mgokT.xyPlane(ispw));
	cout.precision(6);
	cout.setf(ios::fixed,ios::floatfield);
	Int nPA=ntrue(mgokTspw);
	if (nPA>0) {

          // for plotting
          if (report_p) {
            //cerr<<"plotting for each spw..."<<endl;
            String hlab = "Fld:"+String::toString(tranidx)+" Spw:"+String::toString(ispw)+
                          " median="+String::toString(medianGains(tranidx,ispw));
            Vector<Double> tempvec;
            tempvec=mgTspw(mgokTspw).getCompressedArray();
            // determine nbins by Scott's rule
            Double minv, maxv;
            minMax(minv,maxv,tempvec);
            Double binw = 3.49*stddev(tempvec)/pow(Double (tempvec.nelements()), 1./3.);
            Int inNbins = Int (ceil ((maxv-minv)/binw));
            plotHistogram(hlab,countvalidspw,tempvec,inNbins);
            countvalidspw++;
          }

	  //	  cout << "mgTspw = " << mgTspw << endl;
	  scaleOK(ispw,tranidx)=true;
	  //mgratio(ispw,tranidx)=mean(mgTspw(mgokTspw));
	  mgratio(ispw,tranidx)=median(mgTspw(mgokTspw));
          if (nPA==1) { // flux scaling based on a single gain ratio... 
	    mgrms(ispw,tranidx)=0.0;
	    mgerr(ispw,tranidx)=0.0;
          }
          else {
	    mgrms(ispw,tranidx)=stddev(mgTspw(mgokTspw));
	    mgerr(ispw,tranidx)=mgrms(ispw,tranidx)/sqrt(Double(nPA-1));
          }
	  // ...and flux density estimate
	  fd(ispw,tranidx)=mgratio(ispw,tranidx)*mgratio(ispw,tranidx);
	  fdrms(ispw,tranidx)=2.0*mgrms(ispw,tranidx);
          if (nPA==1) {
	    fderr(ispw,tranidx)=0.0;
          }
          else {
	    fderr(ispw,tranidx)=fdrms(ispw,tranidx)/sqrt(Double(nPA-1));
          }
	  numSol(ispw,tranidx) = nPA;
	}

	// Compose the fit message for the list file and the log

	String oMsg( "" );

	oMsg += " Flux density for ";
	oMsg += fldNames(tranidx);
	oMsg += " in SpW=";
	oMsg += String::toString<Int>( ispw );
        if (scaleOK(ispw,tranidx)) {
          oMsg += " (freq=";
          oMsg += String::toString<Double>(solFreq(ispw));
          oMsg += " Hz)";
        }

	if ( refSpw != ispw ) {
	  oMsg += " (ref SpW=";
	  oMsg += String::toString<Int>( refSpw );
	  oMsg += ")";
	}

	oMsg += " is: ";

	if ( scaleOK(ispw,tranidx) ) {
	  oMsg += String::toString<Double>( fd(ispw,tranidx) );
	  oMsg += " +/- ";
	  oMsg += String::toString<Double>( fderr(ispw,tranidx) );
	  oMsg += " (SNR = ";
	  oMsg += String::toString<Double>( fd(ispw,tranidx)/fderr(ispw,tranidx) );
	  oMsg += ", N = ";
	  oMsg += String::toString<Int>( nPA );
	  oMsg += ")";
	} else {
	  oMsg += " INSUFFICIENT DATA ";
	}

	// Write the fit message to the output list file if the file name is
	// non-null.  In the first iteration (first transfer field and spw),
	// open the list file and truncate any previous version.  In subsequent
	// iterations, append the messages.

	if ( oListFile != "" ) {
	  ofstream oStream;
	  if ( iTran == 0 && ispw == 0 ) {
	    oStream.open( oListFile.chars(), ios::out|ios::trunc );
	  } else {
	    oStream.open( oListFile.chars(), ios::out|ios::app );
	  }
	  oStream << "#" << oMsg << endl << flush;
	  oStream.close();
	}


	// Write the fit message to the logger

	logSink() << oMsg << LogIO::POST;

/*
	// Report flux densities to logger
	logSink() << " Flux density for " << fldNames(tranidx)
		  << " in SpW=" << ispw;
	if (refSpw!=ispw) 
	  logSink() << " (ref SpW=" << refSpw << ")";
	
	logSink() << " is: ";
	if (scaleOK(ispw,tranidx)) {
	  logSink() << fd(ispw,tranidx)
		    << " +/- " << fderr(ispw,tranidx)
		    << " (SNR = " << fd(ispw,tranidx)/fderr(ispw,tranidx)
		    << ", N= " << nPA << ")";
	}
	else
	  logSink() << " INSUFFICIENT DATA ";

	logSink() << LogIO::POST;
*/
      } // ispw
      }		  
    } // iTran
    // max 3 coefficients
    //Matrix<Double> spidx(nFld,3,0.0);
    //Matrix<Double> spidxerr(nFld,3,0.0);
    Matrix<Double> spidx(nFld,fitorder+1,0.0);
    Matrix<Double> spidxerr(nFld,fitorder+1,0.0);
    Matrix<Double> covar;
    Vector<Double> fitFluxD(nFld,0.0);
    Vector<Double> fitFluxDErr(nFld,0.0);
    Vector<Double> fitRefFreq(nFld,0.0); 
    //PtrBlock<Matrix<Double> *> covarList(nFld);
    Vector<Matrix<Double> > covarList(nFld);
    //Vector<Double> refFreq(nFld,0.0);
   
    for (Int iTran=0; iTran<nTran; iTran++) {
      uInt tranidx=tranField(iTran);
      Int nValidFlux=ntrue(scaleOK.column(tranidx));

      String oFitMsg;
      logSink()<<LogIO::DEBUG1<<"nValidFLux="<<nValidFlux<<LogIO::POST;

      //if (nValidFlux>1) { 
      if (nValidFlux>0) { 

	// Make fd and freq lists
	Vector<Double> fds;
	fds=fd.column(tranidx)(scaleOK.column(tranidx)).getCompressedArray();
	Vector<Double> fderrs;
	fderrs=fderr.column(tranidx)(scaleOK.column(tranidx)).getCompressedArray();
	Vector<Double> freqs;
	freqs=solFreq(scaleOK.column(tranidx)).getCompressedArray();

	// shift mean(log(freq)) later
	// Reference frequency is first in the list
	//refFreq(tranidx)=freqs[0];
	//freqs/=refFreq(tranidx);
        //
        // calculate spectral index
        // fit the per-spw fluxes to get spectral index
        if (nValidFlux==1) {
          fitFluxD(tranidx) = fds(0);  
          fitRefFreq(tranidx) = freqs(0);  
        }
        else {
          // single spw so no fitting is performed, just fill flux and frequency 
          // to fit result record
        LinearFit<Double> fitter;
        uInt myfitorder = 0; 
        if (fitorder < 0) {
          logSink() << LogIO::WARN
                    << "fitorder=" << fitorder 
                    << " not supported. Using fitorder=1" 
                    << LogIO::POST;    
          myfitorder = 1;
         }
         else if (nValidFlux==2 && fitorder>1) {
             // note that myfitorder does not get set in this conditional branch, is that 
             // the correct thing not to do? (myfitorder was prevously unitialized at this point).
          logSink() << LogIO::WARN
                   << "Not enough number of valid flux density data for the requested fitorder:"<<fitorder
                   << ". Use fitorder=1." <<LogIO::POST;
         } 
         else {
          myfitorder = (uInt)fitorder;
          
          //if (fitorder < nValidFlux) {
          //  myfitorder = (uInt)fitorder;
          //}
          //else {
          //if (fitorder > nValidFlux) {
          //  myfitorder = (uInt)(nValidFlux-1);
          //  logSink() << LogIO::WARN
          //            << "Not enough number of valid flux density data for the requested fitorder:"<<fitorder
          //            <<". Using a lower fitorder="<<myfitorder<<LogIO::POST;
        }
        // set fitting for spectral index, alpha and beta(curvature)
        // with S = S_o*f/f0^(alpha+beta*log(f/fo))
        // fitting y = c + alpha*x + beta*x^2
        // log(S/S0)=alpha*log(f/f0) + beta*log(f/f0)**2
        Polynomial< AutoDiff<Double> > bp(myfitorder);
        fitter.setFunction(bp);

        // shift the zero point to the mean of log(freq)
        Double meanLogFreq=mean(log10(freqs));
        Vector<Double> log_relsolFreq=log10(freqs)-meanLogFreq;
        //Vector<Double> log_relsolFreq=log10(freqs);
        Vector<Double> log_fd=log10(fds);
        
	// The error in the log of fds is fderrs/fds
        Vector<Double> soln=fitter.fit(log_relsolFreq, log_fd, fderrs/fds);
        Vector<Double> errs=fitter.errors();
        Matrix<Double> covar=fitter.compuCovariance();

        for (uInt i=0; i<soln.nelements(); i++) {
           spidx(tranidx,i) = soln(i);
           spidxerr(tranidx,i) = errs(i);
        } 
        fitFluxD(tranidx) = pow(10.0,(soln(0)));
//	correct for the proper propagation of error
        fitFluxDErr(tranidx) = (errs(0)>0.0 ? log(10)*pow(10.0,(soln(0)))*errs(0) : 0.0);
	fitRefFreq(tranidx) = pow(10.0,meanLogFreq);
        //covarList[tranidx] = &covar;
        covarList(tranidx) = covar;
        oFitMsg =" Fitted spectrum for ";
	oFitMsg += fldNames(tranidx);
        oFitMsg += " with fitorder="+String::toString<Int>(myfitorder)+": ";
//	oFitMsg += "Flux density = "+String::toString<Double>(pow(10.0,(soln(0))));
	oFitMsg += "Flux density = "+String::toString<Double>(fitFluxD(tranidx));
//	Double ferr=(errs(0)>0.0 ? exp10(errs(0)) : 0.0);
//      Double ferr=(errs(0)>0.0 ? pow(10.0,(errs(0))) : 0.0);
	oFitMsg += " +/- "+String::toString<Double>(fitFluxDErr(tranidx)); 
//	oFitMsg += " (freq="+String::toString<Double>(refFreq(tranidx)/1.0e9)+" GHz)";
//	oFitMsg += " (freq="+String::toString<Double>(pow(10.0,meanLogFreq)/1.0e9)+" GHz)";
	oFitMsg += " (freq="+String::toString<Double>(fitRefFreq(tranidx)/1.0e9)+" GHz)";
        //oFitMsg += " soln.nelements="+String::toString<Int>(soln.nelements()); 
        oFitMsg += " spidx:";
        for (uInt j=1; j<soln.nelements();j++) {
          String coefname=" a_"+String::toString<Int>(j);
          if (j==1) coefname += " (spectral index) "; 
          oFitMsg += coefname+"="+String::toString<Double>(soln(j)); 
	  oFitMsg += " +/- "+String::toString<Double>(errs(j)); 
	  //if (nValidFlux > (Int)(j+1)) {
	  //    oFitMsg += " +/- "+String::toString<Double>(errs(j)); 
          //}
	  //else {
	  //    oFitMsg += " (degenerate)";
          //}
        }
        Int sh1, sh2;
        covar.shape(sh1,sh2);
        if (sh1 > 1) {
          oFitMsg += " covariance matrix for the fit: ";
          for (Int i=0;i<sh1; i++) {
            for (Int j=0;j<sh2; j++) {
              oFitMsg += " covar("+String::toString(i)+","+String::toString(j)+")="+String::toString<Double>(covar(i,j));
            }
         }
        }
        if ( oListFile != "" ) {
          ofstream oStream;
	  oStream.open( oListFile.chars(), ios::out|ios::app );
	  oStream << "#" << oFitMsg << endl << flush;
	  oStream.close();
        }
        logSink() << oFitMsg << LogIO::POST;
      }
      }// nValidFlux
      /**
      Int sh1, sh2;
      covar->shape(sh1,sh2);

      for (Int i=0;i<sh1; i++) {
        for (Int j=0;j<sh2; j++) {
          logSink() << LogIO::DEBUG1 
            <<"covar("<<i<<","<<j<<")="<<*covar(i,j) << LogIO::POST;
        }
      }
      **/
    }//iTran

    //store determined quantities for returned output
    oFluxScaleStruct.fd = fd.copy();
    oFluxScaleStruct.fderr = fderr.copy();
    oFluxScaleStruct.numSol = numSol.copy();
    oFluxScaleStruct.freq = solFreq.copy();
    oFluxScaleStruct.spidx  = spidx.copy();
    oFluxScaleStruct.spidxerr  = spidxerr.copy();
    oFluxScaleStruct.fitfd = fitFluxD.copy();
    oFluxScaleStruct.fitfderr = fitFluxDErr.copy();
    oFluxScaleStruct.fitreffreq = fitRefFreq.copy();
    //oFluxScaleStruct.covarmat = covarList;
    oFluxScaleStruct.covarmat = covarList.copy();
    // quit if no scale factors found
    if (ntrue(scaleOK) == 0) throw(AipsError("No scale factors determined!"));

    //    cout << "done." << endl;

    //    cout << "Adjusting gains...";

    // Adjust tran field's gains here

    //create incremental caltable
    if (incremental) {
      //MSSpWindowColumns spwcol(ct_->spectralWindow());
      //Vector<Int> NCHAN=spwcol.numChan().getColumn();
      //nChanPar()=NCHAN(0);

      delete ct_;
      // setup and fill a record
      // set record description
      Vector<Int> spwlist(nSpw());
      indgen(spwlist);

      RecordDesc fsparDesc;
      fsparDesc.addField ("caltable", TpString);
      fsparDesc.addField ("time", TpDouble);
      fsparDesc.addField ("spw", TpArrayInt);
      fsparDesc.addField ("antenna", TpArrayInt);
      fsparDesc.addField ("pol", TpString);
      fsparDesc.addField ("parameter", TpArrayDouble);
      fsparDesc.addField ("paramerr", TpArrayDouble);
      fsparDesc.addField ("caltype", TpString);

      // create record with field values
      Record fspar(fsparDesc);
      fspar.define("caltable",outCalTabName);
      //fspar.define("time",tc(0));
      fspar.define("spw", spwlist);
      fspar.define("caltype", "G Cal");
      setSpecify(fspar);
        //
      { // generate per chan factor taking account for spectral index
          // for(Int ich=0;ich<nchan;ich++); fl = soln(0) + alpha*chanf(ich) * beta*chanf(ich)*chanf(ich)
          // fact = sqrt(fl) at each ich for each spw and each field
          // and store 1/fact in bcal-like table 
      }

      // Only do fields that occurred in the input caltable
      for (uInt ifld=0;ifld<fldList.nelements();ifld++) {
	Int iFld=fldList(ifld);
        setCurrField(iFld);
        //
        initSolvePar(); // somewhat redundant but needed to reset solveCPar

        //currField()=iFld; 
        //refTime()=tc(0);
        //currTime()=tc(0);  
        refTime()=mgreft(iFld);
        // for future time support in specify
        fspar.define("time",mgreft(iFld)); 

         // for only looping thru field id (set all spw for each field)
        fspar.define("parameter", abs(1./mgratio.column(iFld)));
        fspar.define("paramerr", 1./mgerr.column(iFld));
        //
        specify(fspar);
      }
/***
      Block<String> cols(3);
      cols[0]="SPECTRAL_WINDOW_ID";
      cols[1]="TIME";
      cols[2]="FIELD_ID";
      CTIter ctiter(*ct_,cols);
      while (!ctiter.pastEnd()) {
        Int iSpw(ctiter.thisSpw());
        Int iFld(ctiter.thisField());
        cerr<<"iFld last="<<iFld<<endl;
        //Cube<Complex> cpar(ctiter.cparam());
        Cube<Float> fpar(ctiter.fparam());
        //cpar = Complex(Float(mgratio(iSpw,iFld)));
        fpar = Float(mgratio(iSpw,iFld));
        //ctiter.setcparam(cpar);
        ctiter.setfparam(fpar);
        ctiter.next(); 
      }
      storeNCT(outfile,false);
***/
    }
    else {
      // older behavior
      Block<String> cols(3);
      cols[0]="SPECTRAL_WINDOW_ID";
      cols[1]="TIME";
      cols[2]="FIELD_ID";
      CTIter ctiter(*ct_,cols);

      while (!ctiter.pastEnd()) {
	Int iSpw(ctiter.thisSpw());
	Int iFld(ctiter.thisField());

	if (scaleOK(iSpw,iFld)) {
          Cube<Complex> cpar(ctiter.cparam());
          cpar/=Complex(Float(mgratio(iSpw,iFld)));
          ctiter.setcparam(cpar);
        }
        ctiter.next();
      }
    } // scope

    //    cout << "done." << endl;

    //    cout << "Cleaning up...";

    // Clean up PtrBlocks
    for (Int iFld=0; iFld<nFld; iFld++) {
      if (MGOK[iFld]!=NULL) {
        delete MGOK[iFld];
        delete MG[iFld];
        delete MG2[iFld];
        delete MGWT[iFld];
        delete MGVAR[iFld];
        delete MGN[iFld];
        delete MGNALL[iFld];
      }
    } 

    //    cout << "done." << endl;

    // Avoid this since problem with <msname>/SELECTED_TABLE (06Feb02 gmoellen)
    /*
    {

      MeasurementSet ms(vs_->msName().before("/SELECTED"));
      Table historytab = Table(ms.historyTableName(),
                               TableLock(TableLock::UserNoReadLocking),
                               Table::Update);
      MSHistoryHandler hist = MSHistoryHandler(ms, "calibrater");
      historytab.lock(true);
      oss.postLocally();
      hist.addMessage(oss);
      historytab.unlock();
    }
    */
  }
  catch (AipsError x) {

    // Clean up PtrBlocks
    for (Int iFld=0; iFld<nFld; iFld++) {
      if (MGOK[iFld]!=NULL) {
        delete MGOK[iFld];
        delete MG[iFld];
        delete MG2[iFld];
        delete MGWT[iFld];
        delete MGVAR[iFld];
        delete MGN[iFld];
        delete MGNALL[iFld];
      }
    }

    throw(x);

  }

  return;

}

void SolvableVisJones::setupPlotter() {
}

void SolvableVisJones::plotHistogram(const String& title,
                                     const Int index,
                                     const Vector<Double>& data,
                                     const Int nbins) {
// construct histogram in multiple panels 
  std::string legendloc = "bottom";
  std::string zoomloc = "";
  if (index==0) {
    std::vector<std::string> loc;
    loc.push_back("top");
    //plotter_->loaddock( dock_xml_p, "bottom", loc, panels_id_[0].getInt());
  }
  else {
     
    // multirow panels
    /***
    if (index<3) {
      //cerr<<"panels_id[index-1].getInt()="<<panels_id_[index-1].getInt()<<endl;
      panels_id_[index] = plotter_->panel( title, "ratio", "N", "",
      std::vector<int>( ), legendloc,zoomloc,panels_id_[index-1].getInt(),false,false);
    }       
    else {
      if (index==3) {
        panels_id_[index] = plotter_->panel( title, "ratio", "N", "",
                       std::vector<int>( ), legendloc,zoomloc,panels_id_[0].getInt(),true,false);
      }
      else {
        panels_id_[index] = plotter_->panel( title, "ratio", "N", "",
                                     std::vector<int>( ), legendloc,zoomloc,panels_id_[index-1].getInt(),false,false);
      } 
    }
    ***/
  }
  // plot histogram

}

void SolvableVisJones::listCal(const Vector<Int> ufldids,
				  const Vector<Int> uantids,
				  const Matrix<Int> uchanids, 
				  const String& listfile,
				  const Int& maxScrRows) {

  //  cout << "listcal disabled for the moment." << endl;

  //  return;

  if (typeName().contains("BPOLY") ||
      typeName().contains("GSPLINE"))
    throw(AipsError(typeName()+" not yet supported."));

    char cfill = cout.fill(' '); // Set fill character for terminal output
    
    //Int uSpwID = uchanids(0,0);
    //Int chan = uchanids(0,1);
    
    // The number of spws; that is, the number of rows in matrix uchanids.
    // Actually, this is the number of spw/channel selections made in the
    // spw selection parameter.  The rows of uchanids may contain the same
    // spw.
    uInt nSpws = uchanids.nrow();
    
    // Prep for listing
    Bool endOutput = false; // if true, end output immediately
    Bool prompt = true; // if true, issue prompt
    Int isol = 0; // total solution counter
    Int scrRows=0; // screen row counter, reset after continue prompt
    
    // Redirect cout to listfile
    ofstream file;
    streambuf* sbuf = cout.rdbuf();
    if(listfile!="") { // non-interactive
        prompt = false;
        // Guard against trampling existing file
        File diskfile(listfile);
        if (diskfile.exists()) {
            String errmsg = "File: " + listfile + 
                " already exists; delete it or choose a different name.";
            throw(AipsError(errmsg));
        }
        else
            cout << "Writing output to file: " << listfile << endl;
        logSink() << LogIO::NORMAL2 << "Redirecting output to "
                  << diskfile.path().originalName().data() << LogIO::POST;
        file.open(diskfile.path().originalName().data());
        cout.rdbuf(file.rdbuf());
    } else { 
        logSink() << LogIO::DEBUG1 
                  << "Not redirecting output: cout remains untouched."
                  << LogIO::POST;
    }
    
    Vector<String> antname;
    msmc().antennaNames(antname);
    Vector<String> fldname;
    msmc().fieldNames(fldname);

    // Default header info.
    logSink() << LogIO::NORMAL1 << "MS name = " << msName() << LogIO::POST;        
        
    logSink() << LogIO::DEBUG1 << "Number of spectral window selections (may not be unique) = " 
              << nSpws << LogIO::POST;
    logSink() << LogIO::DEBUG1 << "Number of (unique) spws in cal table = "
              << ct_->spectralWindow().nrow() << LogIO::POST;



    // Get nchan from the subtable
    MSSpWindowColumns spwcol(ct_->spectralWindow());
    Vector<Int> NCHAN=spwcol.numChan().getColumn();

    Int NANT=ct_->antenna().nrow();


    Block<String> cols(1);
    cols[0]="SPECTRAL_WINDOW_ID";
    ROCTIter ctiter(*ct_,cols);


    while (!ctiter.pastEnd()) {

      Int spwID=ctiter.thisSpw();
      if (anyEQ(uchanids.column(0),spwID)) {
	// spwID among selected spws
	uInt iUchanids=0;
	while (uchanids(iUchanids,0)!=spwID) ++iUchanids;
        Vector<Int> RowUchanids = uchanids.row(iUchanids);
	
        if (ctiter.nrow()<1) {
	  // shouldn't happen
            String errMsg;
            errMsg = "Nothing to list for selected SpwID: " 
                            + errMsg.toString(spwID);
            logSink() << LogIO::NORMAL1 << errMsg << LogIO::POST;
        } else { // we have something to list

            // Handle channel selection here.
	  const uInt nchan= NCHAN(spwID);
            logSink() << LogIO::DEBUG1 << "For Spw ID " << spwID 
                      << ": Number of spectral channels in calibration table = "
                      << nchan << LogIO::POST;

            // Extract channel selection info from uchanids
            Int stepChan   = RowUchanids(3);
            if (stepChan == 0) { stepChan = 1; } // one channel at a time (default)
            else if (stepChan < 0) {
                throw(AipsError("Stepping backwards through channels not supported."));
            }
            // Cal table channels are a subset of the MS channels.
            // MS indices of channels in cal table.
            const uInt msCalStartChan = 0;
            const uInt msCalStopChan  = nchan-1;
            // MS indices of selected channels
            uInt msSelStartChan = RowUchanids(1);
            uInt msSelStopChan  = RowUchanids(2);
            // Cal table indices of selected channels
            Int startChan = msSelStartChan - msCalStartChan;
            Int stopChan  = msSelStopChan - msCalStartChan;
            if (startChan > stopChan) { 
                throw(AipsError("Start channel must be less than or equal to stop channel."));
            } else if ((stopChan < 0) || (startChan > (Int)nchan - 1)) { 
                // All selected channels out of range
                String errMsg = "None of the selected channels are present in cal table.";
                logSink() << LogIO::SEVERE << errMsg
                          << endl << "Selected channel range = [" 
                          << msSelStartChan << "," << msSelStopChan << "]" << endl
                          << "Cal table channel range = [" 
                          << msCalStartChan << "," << msCalStopChan << "]" << LogIO::POST;
                throw(AipsError(errMsg));
            } 
            if (startChan < 0) {
                logSink() << LogIO::NORMAL1 << "Start channel ( " << msSelStartChan
                          << " ) below allowed range." << endl
                          << "Increasing start channel to " << msCalStartChan
                          << LogIO::POST;
                startChan = 0;
            }
            if (stopChan > (Int)nchan - 1) {
                logSink() << LogIO::NORMAL1 << "Stop channel ( " << msSelStopChan
                          << " ) above allowed range." << endl
                          << "Decreasing stop channel to " << msCalStopChan
                          << LogIO::POST;
                stopChan = (Int)nchan - 1;
            }
            // Number of channels selected
            Int numChans = ((stopChan - startChan) / stepChan) + 1; 
            if (numChans > Int(nchan)) {
                logSink() << LogIO::NORMAL1 
                          << "More channels requested than are present in the cal table."
                          << endl << "Cal table channels for this spw: "
                          << msCalStartChan << " to " << msCalStopChan
                          << LogIO::POST;
            } 
            
            logSink() << LogIO::DEBUG1 << "For spwID " << spwID << endl
                      << "  startChan = " << startChan << endl
                      << "  stopChan = " << stopChan << endl
                      << "  stepChan = " << stepChan << endl
                      << "  Number of channels to list: numChans = " << numChans
                      << LogIO::POST;
            
            // Setup the column widths.  Check width of each string.        
            uInt precAmp, precPhase; // Column precision
            uInt oAmp, oPhase; // Column order
            
            wTime_p = 10; // set width of time column
            // set the field column width (looking at the whole cal table)
            wField_p = 0; 

	    Vector<Int> fldids;
	    fldids=ctiter.field();
	    Int nUniqFlds=genSort(fldids,Sort::Ascending,(Sort::QuickSort | Sort::NoDuplicates));
	    fldids.resize(nUniqFlds,true);  // shrink the Vector

	    for (Int ifld=0;ifld<nUniqFlds;++ifld) {
	      String fldstr=(fldname(ifld)); 
	      uInt fldstrLength = fldstr.length();
	      if (wField_p < fldstrLength) { wField_p = fldstrLength; }
	    }

            wChan_p = (uInt)max(3,(Int)rint(log10(nchan)+0.5));
            wPreAnt_p = wTime_p + 1 + wField_p + 1 + wChan_p; // do not count final pipe ("|")
            logSink() << LogIO::DEBUG1 << "Column widths:" << endl
                      << "  time = "    << wTime_p  << endl
                      << "  field = "   << wField_p << endl
                      << "  channel = " << wChan_p << LogIO::POST;
            
            // For multiple channels, I think I will need to write a method that 
            // looks through all spws to determine the necessary order for the 
            // Amplitude and Phase.

	    if (ct_->isComplex()) {
	      oAmp = 1;
	      precAmp = 3; 
	      oPhase = 4;
	      precPhase = 1; 
	    }
	    else {
	      // only "amp" column matters (and may have -ve sign)
	      oAmp = 4;
	      precAmp = 5; 
	      oPhase = 0;
	      precPhase = 0; 
	    }
            
            // set width of amplitude column
            wAmp_p = oAmp + precAmp + 1; // order + precision + decimal point
            
            // set width of phase column
            wPhase_p = oPhase + precPhase + 1; // order + precision + decimal point
            
            wFlag_p=1;
            wPol_p = wAmp_p + 1 + wPhase_p + 1 + wFlag_p + 1; 
	    wAntCol_p = wPol_p*nPar();
	    //cerr << "wAntCol_p = " <<wAntCol_p << endl;
            
            logSink() << LogIO::DEBUG1 << "oAmp = " << oAmp 
                << ", precAmp = " << precAmp 
                << ", wAmp_p = " << wAmp_p << endl
                << "oPhase = " << oPhase 
                << ", precPhase = " << precPhase 
                << ", wPhase_p = " << wPhase_p
                << LogIO::POST;
            
            //uInt numAntCols = 4; // Number of antenna columns
            uInt numAntCols = 8/nPar(); // Number of antenna columns
            wTotal_p = wPreAnt_p + 1 + wAntCol_p*numAntCols;

	    //cerr << "wTotal_p = " << wTotal_p << endl;
            
            // Construct the horizontal separator
            String hSeparator=replicate('-',wTotal_p); 
            uInt colPos=0;
            colPos+=wPreAnt_p; hSeparator[colPos]='|'; colPos++;
            for(uInt iPol=0; iPol<numAntCols*nPar(); iPol++) {
                colPos+=wPol_p-1;
                hSeparator[colPos]='|';
                colPos++;
            }
            
            logSink() << LogIO::DEBUG1
                << "Listing CalTable: " << calTableName()
                << "   (" << typeName() << ") "
                << LogIO::POST;
            
            String dateTimeStr0=MVTime(ctiter.thisTime()/C::day).string(MVTime::YMD,7);
            String dateStr0=dateTimeStr0.substr(0,10);
            
            String headLine = "SpwID = " + String::toString(spwID) + ", "
                              "Date = " + dateStr0 + ",  " +
                              "CalTable = " + calTableName() + " (" + typeName() + "), " +
                              "MS name = " + msName();
            cout.setf(ios::left,ios::adjustfield);
	    //            cout << setw(wTotal_p) << headLine << endl
            cout << headLine << endl
                 << replicate('-',wTotal_p) << endl;
            scrRows+=2;
            
            // labels for flagged solutions
            Vector<String> flagstr(2); 
            flagstr(0)="F";
            flagstr(1)=" ";
            
	    // The following is a klugey way to 
	    // make the times, fields, and gains look like
	    // what the CalTable's used to be

	    Int NTIME=ctiter.nrow()/NANT;
	    Vector<Double> timelist;
	    timelist=ctiter.time();
	    Vector<Int> fieldlist;
	    fieldlist=ctiter.field();
	    for (Int i=1;i<NTIME;++i) {
	      timelist(i)=timelist(i*NANT);
	      fieldlist(i)=fieldlist(i*NANT);
	    }
	    timelist.resize(NTIME,true);
	    fieldlist.resize(NTIME,true);

	    Array<Float> v1,v2;
	    Array<Bool> vok;
	    Int npar=nPar();
	    if (ct_->isComplex()) {
	      Cube<Complex> cparam;
	      ctiter.cparam(cparam);
	      IPosition sh(cparam.shape());
	      sh.append(IPosition(1,NTIME));
	      sh(2)=NANT;
	      Array<Complex> calGains;
	      calGains.reference(cparam.reform(sh));
	      v1.assign(amplitude(calGains));
	      v2.assign(phase(calGains)*180.0/C::pi);
	      Cube<Bool> sok;
	      sok=!ctiter.flag();
	      Array<Bool> calGainOK;
	      calGainOK.reference(sok.reform(sh));
	      vok.reference(calGainOK);
	    }
	    else {
	      Cube<Float> fparam;
	      Array<Float> fparam4;
	      ctiter.fparam(fparam);
	      IPosition sh(fparam.shape());
	      Cube<Bool> sok;
	      Array<Bool> sok4;
	      sok=!ctiter.flag();
	      sh.append(IPosition(1,NTIME));
	      sh(2)=NANT;
	      fparam4.reference(fparam.reform(sh));
	      sok4.reference(sok.reform(sh));

	      // only one val per par
	      sh.append(IPosition(1,NTIME));
	      sh(2)=NANT;
	      v1.reference(fparam4);
	      v2.resize();
	      vok.reference(sok4);
	    }

            IPosition gidx(4,0,0,0,0); // 4-element IPosition, all zeros
            
            // Setup a Vector of antenna ID's to ease the process of printing 
            // multiple antenna columns per row.
            uInt numAnts; // Hold number of antennas to be output.
            Vector<Int> pAntids(NANT); // Vector of antenna ID's.
            if (uantids.nelements()==0) { // Print all antennas.
                numAnts = NANT; 
                for(uInt i=0; i< numAnts; i++) { // Fill pAntids with all antenna IDs.
                    pAntids[i] = i;
                }
            } else { // Use the user-specified antenna ID's.
                numAnts = uantids.nelements(); 
                pAntids.resize(numAnts);
                pAntids = uantids;
            }
            
            // loop over antenna elements
            for (uInt iElem=0;iElem<numAnts;iElem+=numAntCols) { 
                gidx(2)=pAntids(iElem);
                
                Bool header=true; // New antenna, require print header
                
                // If antenna element is among selected antennas, print it
                if (uantids.nelements()==0 || anyEQ(uantids,pAntids(iElem))) {

                    // Begin loop over time
                    for (Int itime=0;itime<NTIME;++itime) { 
                        gidx(3)=itime;
                        
                        Int fldid(fieldlist(itime));
                        
                        // Get date-time string
                        String dateTimeStr=MVTime(timelist(itime)/C::day).string(MVTime::YMD,7);
                        String dateStr=dateTimeStr.substr(0,10);
                        String timeStr=dateTimeStr.substr(11,10);
                        // Get field string
			String fldStr="";
			if (fldid>-1)
			  fldStr=(fldname(fldid));
                        
                        String tmp_timestr = timeStr; // tmp_ variables get reset during the loop
                        String tmp_fldstr = fldStr; //
                        
                        // If no user-specified fields, or fldid is in user's list
                        if (ufldids.nelements()==0 || anyEQ(ufldids,fldid) ) {
                            
                            // Set i/o flags for writing data
                            cout << setiosflags(ios::fixed) << setiosflags(ios::right);
                            
                            // Loop over channels
                            for (uInt iChan=startChan; iChan<=uInt(stopChan); iChan+=stepChan) {
                                
                                // If beginning new screen, print the header
                                if (scrRows == 0 || header) {
                                    header=false;
                                    // Write Antenna line (put spaces over the time, field cols)
                                    for(uInt k=0; k<(wPreAnt_p); k++) { cout<<" "; }
                                    cout << "|";
                                    for(uInt k=0; (k<numAntCols) and (iElem+k<=numAnts-1); k++) {
                                        String tAntName = " Ant = " + String(antname(pAntids(iElem+k)));
                                        cout.setf(ios::left,ios::adjustfield);
                                        cout << setw(wAntCol_p-1) << tAntName << "|";
                                    }
                                    cout << endl;    scrRows++;
                                    
                                    // Write part of header with no data
                                    scrRows += writeHeader(numAntCols, numAnts, iElem);
                                }
                                
                                
                                gidx(1)=iChan;
                                // Write the Time, Field, and Channel for each row
                                cout << setw(wTime_p) << timeStr << " "
                                     << setw(wField_p) << fldStr << " "
                                     << setw(wChan_p) << msCalStartChan + iChan << "|";

                                // Write data for each antenna column
                                for (uInt kelem=iElem; (kelem<iElem+numAntCols) and (kelem<=numAnts-1); 
                                     kelem++) {
                                    gidx(2)=pAntids(kelem);
                                    // Loop over polarization
                                    for (Int ipar=0;ipar<npar;++ipar) {
                                        gidx(0)=ipar; 
                                        
                                        // WRITE THE DATA
                                             // amplitude
                                        cout << setprecision(precAmp) << setw(wAmp_p) << v1(gidx) << " ";

					if (v2.nelements()>0)
                                             // phase
					  cout<< setprecision(precPhase) << setw(wPhase_p) << v2(gidx) << " ";
					else
					  cout<< setprecision(precPhase) << setw(wPhase_p) << replicate(" ",wPhase_p) << " ";
					// flag
					cout << flagstr(Int(vok(gidx))) << " ";
                                    } // end ipar loop
                                    
                                } // end kelem loop
                                
                                cout << resetiosflags(ios::right) << endl;
                                isol=isol+numAntCols; scrRows++;
                                
                                // If at end of screen prompt user or new header
                                if (maxScrRows>0 && (scrRows >= maxScrRows-1) ) { // scrRows counts from 0
                                    scrRows = 0; // signal a new page
                                    if (prompt) { // query the user, if we are interactive
                                        string contStr;
                                        cout << "Type Q to quit, A to list all, or RETURN to continue [continue]: ";
                                        getline(cin,contStr);
                                        if ( (contStr.compare(0,1,"q") == 0) or 
                                             (contStr.compare(0,1,"Q") == 0) ) { endOutput=true; }
                                        if ( (contStr.compare(0,1,"a") == 0) or 
                                             (contStr.compare(0,1,"A") == 0) ) { prompt = false; }
                                    }
                                }
                            } // end iChan loop
                            
                        } // end if (field)
                        
                        if (endOutput) {break;} // break out of itime loop
                        
                    } // itime
                    
                } // end if (antenna)
                
                if (endOutput) {break;} // break out of ielem loop
                
            } // end iElem loop
            
            if (endOutput) {break;} // break out of spw loop

        } // end spw if (verification) block
        
      } // end spw loop   
    
      // advance iterator (spw)
      ctiter.next();
    } // ctiter

    cout << endl
         << "Listed " << isol << " antenna solutions." 
         << endl << endl;
         
    if (listfile!="") cout.rdbuf(sbuf); // restore cout
    cout.fill(cfill);

}// end function listCal

int SolvableVisJones::writeHeader(const uInt numAntCols, 
                                  const uInt numAnts,
                                  const uInt iElem) {
    uInt lineCount=0;
    // Write time and field headings (only once)
    cout << setiosflags(ios::left)
         << setw(wTime_p) << "Time"  << " "
         << setw(wField_p) << "Field" << " "
         << setw(wChan_p) << "Chn" << "|";
    
    // Write Amp and Phase headings for each antenna column
    Int nh(2);
    Vector<String> vh(nh);
    vh[0]="Amp ";
    vh[1]="Phs ";
    if (this->typeName().contains("EVLAGAIN")) {
      nh=8;
      vh.resize(nh);
      vh[0]=vh[4]="Gain";
      vh[2]=vh[6]="Tsys";
      vh[1]=vh[3]=vh[5]=vh[7]=" ";
    }
    else if (this->typeName()=="K Jones") {
      vh[0]="Delay ";
      vh[1]=" ";
    }
    else if (this->typeName().contains("Opac")) {
      vh[0]="Opac ";
      vh[1]=" ";
    }
    else if (this->typeName().contains("KAntPos")) {
      nh=6;
      vh.resize(nh);
      vh[0]="dX m";
      vh[2]="dY m";
      vh[4]="dZ m";
      vh[1]=vh[3]=vh[5]=" ";
    }
    else if (this->typeName().contains("EGainCurve")) {
      nh=8;
      vh.resize(nh);
      vh[0]="c[0]";
      vh[2]="c[1]";
      vh[4]="c[2]";
      vh[6]="c[3]";
      vh[1]=vh[3]=vh[5]=vh[7]=" ";
    }
    else if (this->typeName().contains("TSYS")) {
      vh[0]="Tsys";
      vh[1]=" ";
    }

    cout.setf(ios::right, ios::adjustfield);
    for(uInt k=0; (k<numAntCols) and (iElem+k<=numAnts-1); k++) {
      for (Int ip=0;ip<nPar();++ip) {
	if (ip>0) cout << " ";
	cout << setw(wAmp_p)   << vh[(2*ip)%nh] << " " 
	     << setw(wPhase_p) << vh[(2*ip+1)%nh] << " " 
	     << "F";
      }
      cout << "|";
    }
    cout << endl;    lineCount++;
    // Construct the horizontal separator
    String hSeparator=replicate('-',wTotal_p); 
    uInt colPos=0;
    colPos+=wTime_p; hSeparator[colPos]='|'; colPos++;
    colPos+=wField_p; hSeparator[colPos]='|'; colPos++;
    colPos+=wChan_p; hSeparator[colPos]='|'; colPos++;
    for(uInt iPol=0; iPol<numAntCols*nPar(); iPol++) {
        colPos+=wPol_p-1;
        hSeparator[colPos]='|';
        colPos++;
    }
    // Write horizontal separator
    cout << hSeparator << endl;   lineCount++;
    return lineCount;
} // end writeHeader

// Globals

// Return a cal table's type, verifying its existence
String calTableType(const String& tablename) {

  // Check existence...
  if (!Table::isReadable(tablename)) {
    ostringstream o;
    o << "Table " << tablename
      << " does not exist.";
    throw(AipsError(String(o)));
  }

  // Table exists...
  
  TableInfo ti(TableUtil::tableInfo(tablename));
  
  // ...Check if Calibration table....
  if (ti.type()!="Calibration") {
    ostringstream o;
    o << "Table " << tablename
      << " is not a valid Calibration table."
      << " (expected type = \"Calibration\"; type found = \""
      << ti.type() << "\")";
    throw(AipsError(String(o)));
    
  }
  
  // If we get here, we have a calibration table,
  //  so return its type
  return ti.subType();

}


} //# NAMESPACE CASA - END
