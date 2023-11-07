//# Flagger.h: this defines Flagger
//# Copyright (C) 2000,2001
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
#ifndef FLAGGING_FLAGGER_H
#define FLAGGING_FLAGGER_H

#include <flagging/Flagging/RFCommon.h>
#include <flagging/Flagging/RFABase.h>
#include <flagging/Flagging/FlagVersion.h>

#include <casacore/ms/MeasurementSets/MeasurementSet.h>
#include <casacore/ms/MSSel/MSSelection.h>
#include <casacore/measures/Measures/MDirection.h>
#include <casacore/measures/Measures/MPosition.h>
#include <casacore/measures/Measures/MRadialVelocity.h>
#include <casacore/casa/Logging/LogIO.h>
#include <casacore/casa/Arrays/Vector.h>
#include <casacore/casa/Containers/Record.h>
#include <casacore/casa/Quanta/Quantum.h>

namespace casa { //# NAMESPACE CASA - BEGIN

class VisSet;
class RFChunkStats;
        
// <summary>
// Flagger: high-performance automated flagging
// </summary>

// <use visibility=global>

// <reviewed reviewer="" date="" tests="" demos="">
// </reviewed>

// <prerequisite>
//   <li> implement/Flagger
// </prerequisite>
//
// <etymology>
// casacore::MSFlagger and plain flagger were already taken.
// </etymology>
//
// <synopsis>
// Flagger performs automated flagging operations on a measurement set.
// The class is constructed from an MS. After that, the run method may be used
// to run any number of flagging agents.
// </synopsis>
//
// <example>
//        // construct casacore::MS and flagger
//        casacore::MeasurementSet ms("test.MS2",casacore::Table::Update);
//        Flagger flagger(ms);
//        // build record of global flagging options
//        casacore::Record opt(casacore::Record::Variable);
//        // build record of flagging agents to be run
//        casacore::Record selopt( flagger.defaultAgents().asRecord("select") );
//        selopt.define(RF_POLICY,"RESET");
//        selopt.define(RF_AUTOCORR,true);
//        casacore::Record agents(casacore::Record::Variable);
//        agents.defineRecord("select",selopt);
//        // perform the flagging
//        flagger.run(agents,opt);
// </example>
//
// <motivation>
// We need an automated flagging tool. Existing tools (casacore::MSFlagger and flagger.g)
// were too slow. Hence, Flagger was developed.
// </motivation>
//
// <todo asof="2001/04/16">
//   <li> add this feature
//   <li> fix this bug
//   <li> start discussion of this possible extension
// </todo>


class Flagger : public FlaggerEnums
{
protected:
// creates an agent by name
  std::shared_ptr<RFABase> createAgent ( const casacore::String &name,RFChunkStats &chunk,const casacore::RecordInterface &parms, bool &only_selector );

// sets up record of agents and default parameters
  const casacore::RecordInterface & setupAgentDefaults ();

// print flagging reports from individual agents
  void printAgentReports  ( );

  void printSummaryReport ( RFChunkStats &chunk );
  casacore::Bool selectDataChannel();
 
  casacore::MeasurementSet   ms;
  casacore::MeasurementSet   originalms;
  std::vector<std::shared_ptr<RFABase> > acc;

  //new added
  casacore::MeasurementSet *mssel_p;
  VisSet *vs_p;
  bool scan_looping;     /* Is scan number part of visiter looping? */
  casacore::String msname_p;
  casacore::Bool nullSelect_p;
  casacore::Bool setdata_p;
  casacore::Bool selectdata_p;
  casacore::String dataMode_p;  

//  casacore::Vector<casacore::Int> dataEnd_p;
//  casacore::Vector<casacore::Int> dataStart_p, dataStep_p;
//  casacore::Vector<casacore::Int> dataspectralwindowids_p;
//  casacore::Vector<casacore::Int> spwidnchans_p;
  casacore::Vector<casacore::String> correlations_p;
  casacore::Vector<casacore::Int> datafieldids_p;
  casacore::Vector<casacore::Int> datadescids_p;
  casacore::MRadialVelocity mDataStart_p;
  casacore::MRadialVelocity mDataStep_p;

  //
  casacore::uInt nant,nifr,nfeed,nfeedcorr;
  casacore::Vector<casacore::Int> ifr2ant1,ifr2ant2;
  casacore::Vector<casacore::String> antnames;
  casacore::Vector<casacore::Double> spwfreqs;
    
  casacore::Record agent_defaults;

  static casacore::LogIO os;

public:  
// default constructor 
  Flagger  ();
// construct and attach to a measurement set
  Flagger  ( casacore::MeasurementSet &ms );
  
  ~Flagger ();
  
// Change or set the casacore::MS this Flagger refers to.
  bool attach( casacore::MeasurementSet &ms, casacore::Bool setupAgentDefaults=true );

  // Set the data selection parameters
  casacore::Bool selectdata(casacore::Bool useoriginalms=false, 
                  casacore::String field="", casacore::String spw="", casacore::String array="", casacore::String feed="", casacore::String scan="",
	          casacore::String baseline="", casacore::String uvrange="", casacore::String time="",
	          casacore::String correlation="", casacore::String intent="", casacore::String observation="");

  // Make a data selection
  casacore::Bool setdata(casacore::String field, casacore::String spw, casacore::String array, casacore::String feed, casacore::String scan,
	       casacore::String baseline, casacore::String uvrange, casacore::String time,
	       casacore::String correlation, casacore::String intent, casacore::String observation="");
  
  // Make a selection for manual flagging
  casacore::Bool setmanualflags(casacore::Bool autocorr,
                      casacore::Bool unflag, 
                      casacore::String clipexpr, 
                      casacore::Vector<casacore::Double> cliprange, 
                      casacore::String clipcolumn, 
                      casacore::Bool outside, 
                      casacore::Bool channel_average,
                      casacore::Double quackinterval=0.0, 
                      casacore::String quackmode=casacore::String("beg"),
                      casacore::Bool quackincrement=casacore::Bool(false),
                      casacore::String opmode=casacore::String("flag"),
                      casacore::Double diameter = -1.0,
                      casacore::Double lowerlimit = -1.0,
                      casacore::Double upperlimit = 91.0);

  // Clean up all agents of type "select".
  //casacore::Bool clearflagselections(casacore::Vector<casacore::Int> &recordlist,casacore::Vector<casacore::String> &agentlist);
  casacore::Bool clearflagselections(casacore::Int recordindex);
  casacore::Bool printflagselections();

  // Fill the selection record to attach to the list of agents
  casacore::Bool fillSelections(casacore::Record &rec);

  // Set autoflag params
  casacore::Bool setautoflagparams(casacore::String algorithm, casacore::Record &parameters);

  // Get default autoflag params
  casacore::Record getautoflagparams(casacore::String algorithm);

  casacore::Bool addAgent(casacore::RecordInterface &newAgent);
  
  // Detaches from the casacore::MS  
  void detach();
  
  casacore::Record run(casacore::Bool trial, casacore::Bool reset);    

  void summary ( const casacore::RecordInterface &agents ); 

    // flag version support.
  casacore::Bool  saveFlagVersion(casacore::String versionname, casacore::String comment, casacore::String merge);
  casacore::Bool  restoreFlagVersion(casacore::Vector<casacore::String> versionname, casacore::String merge);
  casacore::Bool  deleteFlagVersion(casacore::Vector<casacore::String> versionname);
  casacore::Bool  getFlagVersionList( casacore::Vector<casacore::String> &verlist);
 
  
  // returns current MS
  const casacore::MeasurementSet & measSet() const { return ms; }
    
// number of antennas in MS
  casacore::uInt numAnt    () const 
      { return nant; };

// number of IFRs in MS
  casacore::uInt numIfr    () const 
      { return nifr; };

// number of feeds in MS
  casacore::uInt numFeed    () const 
      { return nfeed; };

// number of feed correlations in MS
  casacore::uInt numFeedCorr    () const 
      { return nfeedcorr; };

// names of antennas
  const casacore::Vector<casacore::String> & antNames() const 
      { return antnames; };
  
// derives a flat IFR index from two antenna indices
  casacore::uInt ifrNumber ( casacore::Int ant1,casacore::Int ant2 ) const;

// vector version of above
  casacore::Vector<casacore::Int> ifrNumbers ( casacore::Vector<casacore::Int> ant1,casacore::Vector<casacore::Int> ant2 ) const;

// derives antenna indices from a flat IFR index
  void ifrToAnt ( casacore::uInt &ant1,casacore::uInt &ant2,casacore::uInt ifr ) const;

// returns a record with all available agents and their default options
  const casacore::RecordInterface & defaultAgents () const 
      { return agent_defaults; }

// returns a record of available options
  static const casacore::RecordInterface & defaultOptions ();

// returns the log sink 
  static casacore::LogIO & logSink ()       { return os; }

  /* Get rid of negative indices (meaning negation of antenna) in baselinelist */
  static void reform_baselinelist(casacore::Matrix<casacore::Int> &baselinelist, unsigned nant);
  
  static int my_aipspp_sum(const casacore::Array<casacore::Bool> &a);

private:
    
  Flagger( const Flagger & )          {};

  Flagger& operator=(const Flagger &)  { return *this; };

  void printAgentRecord(casacore::String &, casacore::uInt, const casacore::RecordInterface &);

  // Sink used to store history
  casacore::LogSink logSink_p;

  // Hold the original ms 
  casacore::MeasurementSet *originalms_p;
  
  // casacore::MS Selection
  casacore::MSSelection *msselection_p;
  bool spw_selection;  //non-trivial spw-selection

  // casacore::List of Agents
  casacore::Record *agents_p;
  casacore::Int agentCount_p;

  // casacore::List of extra options
  casacore::Record *opts_p;

  // Debug Message flag
  static const bool dbg;

  casacore::Bool quack_agent_exists;
  /* More initialization is required, if there exists a quacking agent */
};


} //# NAMESPACE CASA - END

#endif

