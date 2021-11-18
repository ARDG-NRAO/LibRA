//# AgentFlagger.h: this defines AgentFlagger
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

#include <casa/Logging/LogIO.h>
#include <casa/Arrays/Vector.h>
#include <casa/Containers/Record.h>

// This is needed for FlagAgentList, which should probably go into a separate header
#include <flagging/Flagging/FlagAgentBase.h>

// To handle variant parameters
#include <stdcasa/StdCasa/CasacSupport.h>

namespace casa { //# NAMESPACE CASA - BEGIN

class FlagAgentSummary;
class FlagDataHandler;

class FlagAgentDisplay;

// <summary>
// AgentFlagger: high-performance automated flagging
// </summary>

// <use visibility=global>

// <reviewed reviewer="" date="" tests="" demos="">
// </reviewed>

// <prerequisite>
//   <li> <linkto class="FlagDataHandler:description">FlagDataHandler</linkto>
//   <li> <linkto class="FlagMSHandlerr:description">FlagMSHandler</linkto>
//   <li> <linkto class="FlagCalTableHandler:description">FlagCalTableHandler</linkto>
// </prerequisite>
//
// <etymology>
//
// </etymology>
//
// <synopsis>
// AgentFlagger performs automated flagging operations on a measurement set or calibration
// table. The class is constructed from an casacore::MS or cal table. After that, the run method may be used
// to run any number of flagging agents.
// </synopsis>
//
// <example>
// <srcblock>
// // The following is a typical way of using this class and its methods to perform flagging.
//
// // Open the casacore::MS or a calibration file and attach it to the tool. This will create an object of the
// // FlagDataHandler (fdh) type. The constructor of the fdh takes three arguments,
// // the casacore::MS or CAL table, the iteration approach to use and the time interval. Only the MS
// // is mandatory to use. By default it will use the FlagDataHandler::SUB_INTEGRATION iteration
// // approach and 0.0 seconds as the time interval.
//
//    AgentFlagger *tf = new AgentFlagger();
//    af->open('my.ms')
//
// // Select the data where to flag. If left blank, the whole casacore::MS will be selected. This step
// // will use the casacore::MS Selection class. There are two methods to perform the selection. One takes
// // a casacore::Record of the parameters, the other takes the individual parameters as arguments.
//
// // 1) First method:
//    casacore::String spw = "0:1~10";
//    casacore::String scan = "1";
//    casacore::Record selection = casacore::Record();
//    selection.define("spw", spw);
//    selection.define("scan", scan);
//    af->selectData(selection);
//
// // 2) Second method:
//    af->selectData(spw=spw, scan=scan);
//
// // Now it is time to build a list of the agents that we want to run to process the data. This
// // step will create a list of all the agents that will be executed to flag/unflag the data.
// // This method can be called multiple times. Every call should contain the desired parameters of
// // the agent and optionally data selection parameters. When data selection parameters are present,
// // the agent will loop through only that portion of the data.
//
// // This method will check if the requested agent (mode) is known from the following list
// // (manual, clip, quack, shadow, elevation, tfcrop, rflag, extend, unflag, summary, and antint). If
// // empty or unknown, it will give a warning and return.
//
// // If any tfcrop, rflag or extend mode is present, this method will calculate the maximum value
// // of time interval (ntime) from these agents. The maximum value will be used for all agents in
// // the list.
//
// // A similar situation will happen with the combinescans parameter. If any of the combinescans is
// // true, it will be taken as true to all agents.
//
// // Async I/O will be activated if any of the modes clip, tfcrop or rflag is requested. Also for
// // these three modes, there will be a call to a function that will validate the requested
// // datacolumn parameter. It will detect if the input is an casacore::MS or a cal table and validate the
// // column. The default is the DATA column. If the input is a cal table, the function will
// // first check if FPARAM is available, then CPARAM. If none of them is available it will return
// // false and the agent will not be created.
//
// // Only for the tfcrop agent, if a correlation ALL is requested, this method will create one
// // agent for each available polarization in the MS. For example, if the casacore::MS contains polarizations
// // XX and YY and the parameter is correlation="ABS_ALL", then there will be two tfcrop agents,
// // one with correlation="ABS_XX" and the other with correlation="ABS_YY". The apply parameter
// // is set by default to true to apply the flags.
//
//     casacore::Record agent_pars = casacore::Record();
//     agent_pars.define("mode", "clip");
//     agent_pars.define("clipzeros", true);
//     agent_pars.define("apply", true);
//     af->parseAgentParameters(agent_pars);
//
//     casacore::Record agent_pars = casacore::Record();
//     agent_pars.define("mode", "manual");
//     agent_pars.define("autocorr", true);
//     af->parseAgentParameters(agent_pars);
//
//     casacore::Record agent_pars = casacore::Record();
//     agent_pars.define("mode", "summary");
//     agent_pars.define("basecnt", true);
//     af->parseAgentParameters(agent_pars);
//
// // There are convenience functions to parse the agent's parameters, one specific for each agent.
// // The above calls can also be done using these functions instead.
//
//     af->parseClipParameters(clipzeros=true, apply=true);
//     af->parseManualParameters(autocorr=true);
//     af->parseSummaryParameters(basecnt=true);
//
// // In either one of the cases, three agents will be created.
// //
// // NOTE: it is possible to add multiple summary agents to the list and gather a list of summary
// // reports when executing the tool.
//
// // We need to initialize the agents, which
// // will call the constructor of each one of them and set the parameters that were given in the previous
// // calls. Some basic checks will be performed at this stage for types and values of the parameters.
//
// // If any tfcrop, rflag, extend or display agent is in the list, the iteration approach will be
// // set to a different value depending on whether combinescans is true or not. When true, the
// // iteration approach will be set to FlagDataHandler::COMBINE_SCANS_MAP_ANTENNA_PAIRS_ONLY, otherwise
// // to FlagDataHandler::COMPLETE_SCAN_MAP_ANTENNA_PAIRS_ONLY.
//
// // This method will create agents and add them to a FlagAgentList. If for any reason, the call to
// // FlagAgentBase::create(fdh_p, agent_rec) fails, an error message will be displayed. Any agents previously
// // added to the FlagAgentList will remain there. A subsequent call to this method can be done to add
// // more agents to the same FlagAgentList.
//
//     af->initAgents();
//
// // Before next step which will write the new flags, it is advisable to create a backup of
// // the current flags in the MS.
//
//    af.saveflagversion(versionname='backup_before_manual_1',
//                       comment='Backup of flags before running manual',merge='replace')
//
// // The next step in the chain is to actually process the flags and write them or
// // not to the MS. The run method takes two parameters, writeflags and sequential.
// // The parameter writeflags controls whether to write the flags or not to the MS.
// // By default it is set to true. Setting writeflags to false is useful when one
// // wants to run the tool together with the display agent to see what is going to be
// // flagged before deciding to write or not to the MS. The sequential parameter
// // controls if the order of the agent's list needs to be preserved or not. If set to false,
// // the order will not be preserved and the framework may execute the agent's list in parallel.
// // By default sequential is set to true.
//
// // The run method gathers several reports, depending on which agents are run. The display and summary agents
// // produce reports that can be retrieved from calling the run method. The reports are returned via a Record
// // that may contain multiple reports at the same time.

//     casacore::Record myReports;
//     myReports = af->run();
//
// // To destroy the tool, call a method to execute the destructor.
//
//     af->done();
//
// </srcblock>
// </example>
//
// <motivation>
// To flag data using different algorithms.
// </motivation>
//


class AgentFlagger
{
protected:

	static casacore::LogIO os;

	// variables used to initialize the FlagDataHandler
	casacore::String msname_p;
	casacore::uShort iterationApproach_p;
	casacore::Double timeInterval_p;
	casacore::Bool isMS_p;


	// members to parse to selectData
	casacore::String spw_p;
	casacore::String scan_p;
	casacore::String field_p;
	casacore::String antenna_p;
	casacore::String timerange_p;
	casacore::String correlation_p;
	casacore::String intent_p;
	casacore::String feed_p;
	casacore::String array_p;
	casacore::String uvrange_p;
	casacore::String observation_p;
	casacore::Record dataselection_p;

	// agent's members
	casacore::String mode_p;
	casacore::Record agentParams_p;
	FlagAgentSummary *summaryAgent_p;
	casacore::Bool combinescans_p;
	casacore::Bool extendflags_p;

	// true if there are apply and unapply parameters in the list
	casacore::Bool mixed_p;

	// Display agent parameters
    casa::FlagAgentDisplay *displayAgent_p;

	// variables for initAgents
	FlagDataHandler *fdh_p;
	std::vector<casacore::Record> agents_config_list_p;
	std::vector<casacore::Record> agents_config_list_copy_p;
	FlagAgentList agents_list_p;

public:  
	// default constructor
	AgentFlagger();

	// destructor
	~AgentFlagger();

	// reset everything
	void done();

	// configure the tool, open the MS
	bool open(casacore::String msname, casacore::Double ntime);

	// parse the data selection
	bool selectData(casacore::Record selrec);
	bool selectData(casacore::String field, casacore::String spw, casacore::String array, casacore::String feed, casacore::String scan,
		       	    casacore::String antenna, casacore::String uvrange, casacore::String timerange,
		       	    casacore::String correlation, casacore::String intent, casacore::String observation="");

	// parse the parameters of the agent
	bool parseAgentParameters(casacore::Record agent_params);
	casacore::String getExpressionFunction(casacore::String expression);
	bool isExpressionPolarizationAll(casacore::String expression);

        // initialize the agents list
        void checkAveragingConfig(const std::string &mode, const casacore::Record &agent_rec,
                                  const std::string &anyNotAvg);
        std::string searchAnyAgentsNotAvg(const std::vector<casacore::Record> &configs);
        bool initAgents();

	// Run the tool and write the flags to the MS
	casacore::Record run(casacore::Bool writeflags, casacore::Bool sequential=true);

	// Flag backup methods
	bool printFlagSelections();
	bool saveFlagVersion(casacore::String versionname, casacore::String comment, casacore::String merge);
	bool restoreFlagVersion(casacore::Vector<casacore::String> versionname, casacore::String merge);
	bool deleteFlagVersion(casacore::Vector<casacore::String> versionname);
	bool getFlagVersionList(casacore::Vector<casacore::String> &verlist);

	// Agent's specific parsing methods (for convenience only)
	// Parse parameters for manual
	bool parseManualParameters(casacore::String field, casacore::String spw, casacore::String array, casacore::String feed, casacore::String scan,
       	    casacore::String antenna, casacore::String uvrange, casacore::String timerange,casacore::String correlation,
       	    casacore::String intent, casacore::String observation, casacore::Bool autocorr, casacore::Bool apply);

	// Parse parameters for clip
	bool parseClipParameters(casacore::String field, casacore::String spw, casacore::String array, casacore::String feed, casacore::String scan,
       	    casacore::String antenna, casacore::String uvrange, casacore::String timerange,casacore::String correlation,
       	    casacore::String intent, casacore::String observation, casacore::String datacolumn,
       	    casacore::Vector<casacore::Double> clipminmax, casacore::Bool clipoutside, casacore::Bool channelavg, casac::variant chanbin, casacore::Bool timeavg,
       	    casacore::String timebin, casacore::Bool clipzeros, casacore::Bool apply);

	// Parse parameters for quack
	bool parseQuackParameters(casacore::String field, casacore::String spw, casacore::String array, casacore::String feed, casacore::String scan,
       	    casacore::String antenna, casacore::String uvrange, casacore::String timerange,casacore::String correlation,
       	    casacore::String intent, casacore::String observation, casacore::String quackmode, casacore::Double quackinterval,
       	    casacore::Bool quackincrement, casacore::Bool apply);

	// Parse parameters for elevation
	bool parseElevationParameters(casacore::String field, casacore::String spw, casacore::String array, casacore::String feed,
			casacore::String scan, casacore::String antenna, casacore::String uvrange, casacore::String timerange, casacore::String correlation,
       	    casacore::String intent, casacore::String observation, casacore::Double lowerlimit, casacore::Double upperlimit,
       	    casacore::Bool apply);

	// Parse parameters for tfcrop
	bool parseTfcropParameters(casacore::String field, casacore::String spw, casacore::String array, casacore::String feed,
			casacore::String scan, casacore::String antenna, casacore::String uvrange, casacore::String timerange, casacore::String correlation,
       	    casacore::String intent, casacore::String observation, casacore::Double ntime, casacore::Bool combinescans,
       	    casacore::String datacolumn, casacore::Double timecutoff, casacore::Double freqcutoff, casacore::String timefit,
       	    casacore::String freqfit, casacore::Int maxnpieces, casacore::String flagdimension, casacore::String usewindowstats,
       	    casacore::Int halfwin, casacore::Bool extendflags, casacore::Bool apply, casacore::Bool channelavg, casac::variant chanbin, casacore::Bool timeavg,
       	    casacore::String timebin);

	// Parse parameters for the 'antint' mode (antenna integrations)
	bool parseAntIntParameters(casacore::String field, casacore::String spw,
				   casacore::String array, casacore::String feed,
				   casacore::String scan, casacore::String antenna,
				   casacore::String uvrange, casacore::String timerange, 
				   casacore::String correlation, casacore::String intent,
				   casacore::String observation,
				   casacore::String antint_ref_antenna,
				   casacore::Double minchanfrac,
				   casacore::Bool verbose, casacore::Bool apply);

	// Parse parameters for extend
	bool parseExtendParameters(casacore::String field="", casacore::String spw="", casacore::String array="", casacore::String feed="",
			casacore::String scan="", casacore::String antenna="", casacore::String uvrange="", casacore::String timerange="", casacore::String correlation="",
       	    casacore::String intent="", casacore::String observation="", casacore::Double ntime=0.0, casacore::Bool combinescans=false,
       	    casacore::Bool extendpols=true, casacore::Double growtime=50.0, casacore::Double growfreq=50.0, casacore::Bool growaround=false,
       	    casacore::Bool flagneartime=false, casacore::Bool flagnearfreq=false, casacore::Bool apply=true);

	// Parse parameters for summary
	bool parseSummaryParameters(casacore::String field, casacore::String spw, casacore::String array, casacore::String feed,
			casacore::String scan, casacore::String antenna, casacore::String uvrange, casacore::String timerange, casacore::String correlation,
       	    casacore::String intent, casacore::String observation, casacore::Bool spwchan, casacore::Bool spwcorr, casacore::Bool basecnt, casacore::Bool fieldcnt,
       	    casacore::String name);

private:

	AgentFlagger(const AgentFlagger &) {};

	AgentFlagger& operator=(const AgentFlagger &)  {return *this;};

	// Maximum between two numbers
	void getMax(casacore::Double value);

	// Check if mode is valid against a list of known modes
	bool isModeValid(casacore::String mode);

	casacore::Bool validateDataColumn(casacore::String datacol);

	// Sink used to store history
	casacore::LogSink logSink_p;

	// Debug message flag
	static const bool dbg;

	// Store the temporary maximum value
	casacore::Double max_p;
};


} //# NAMESPACE CASA - END

#endif

