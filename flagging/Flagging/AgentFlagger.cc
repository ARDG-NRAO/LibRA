///# AgentFlagger.cc: this defines AgentFlagger
//# Copyright (C) 2000,2001,2002
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
#include <casacore/casa/Arrays/Vector.h>
#include <casacore/casa/Arrays/ArrayMath.h>
#include <casacore/casa/Arrays/ArrayLogical.h>
#include <casacore/casa/BasicSL/Complex.h>
#include <casacore/casa/Utilities/Regex.h>
#include <casacore/casa/OS/HostInfo.h>
#include <casacore/casa/Exceptions/Error.h>
#include <flagging/Flagging/AgentFlagger.h>
#include <flagging/Flagging/FlagVersion.h>
#include <casa/stdio.h>
#include <casa/math.h>
#include <stdarg.h>
#include <sstream>
#include <iostream>
#include <vector>

#include <flagging/Flagging/FlagCalTableHandler.h>
#include <flagging/Flagging/FlagMSHandler.h>
#if defined(CASATOOLS)
#include <flagging/Flagging/grpcFlagAgentDisplay.h>
#else
#include <flagging/Flagging/FlagAgentDisplay.h>
#endif

using namespace casacore;
namespace casa {

const bool AgentFlagger::dbg = false;


// -----------------------------------------------------------------------
// Default Constructor
// -----------------------------------------------------------------------
AgentFlagger::AgentFlagger ()
{
	fdh_p = NULL;
	summaryAgent_p = NULL;
	displayAgent_p = NULL;

	done();
}


// -----------------------------------------------------------------------
// Default Destructor
// -----------------------------------------------------------------------
AgentFlagger::~AgentFlagger ()
{
	done();
}

void
AgentFlagger::done()
{
	if(fdh_p){
		delete fdh_p;
		fdh_p = NULL;
	}

	// Default values of parameters
	msname_p = "";
	iterationApproach_p = FlagDataHandler::SUB_INTEGRATION;
	timeInterval_p = 0.0;
	isMS_p = true;
	combinescans_p = false;
	extendflags_p = true;
	spw_p = "";
	scan_p = "";
	field_p = "";
	antenna_p = "";
	timerange_p = "";
	correlation_p = "";
	intent_p = "";
	feed_p = "";
	array_p = "";
	uvrange_p = "";
	observation_p = "";

	max_p = 0.0;

	dataselection_p = Record();

	/*
	if (! dataselection_p.empty()) {
		Record temp;
		dataselection_p = temp;
	}
	 */

	if (! agentParams_p.empty()) {
		Record temp;
		agentParams_p = temp;
	}

	if(summaryAgent_p){
		summaryAgent_p = NULL;
	}

	if(displayAgent_p){
		displayAgent_p = NULL;
	}

	mode_p = "";
	agents_config_list_p.clear();
	agents_config_list_copy_p.clear();
	agents_list_p.clear();

	return;
}

// ---------------------------------------------------------------------
// AgentFlagger::open
// Create a FlagDataHandler object based on the input type:
// MS or a calibration file. Open the MS or cal table and
// attach it to the tool.
// ---------------------------------------------------------------------
bool
AgentFlagger::open(String msname, Double ntime)
{

	LogIO os(LogOrigin("AgentFlagger", __FUNCTION__));

	if (msname.empty()) {
		os << LogIO::SEVERE << "No Measurement Set has been parsed"
				<< LogIO::POST;
		return false;
	}

	msname_p = msname;

	if (ntime)
		timeInterval_p = ntime;

	max_p = timeInterval_p;

	if(dbg){
		os << LogIO::NORMAL << "msname = " << msname_p << " ntime = " << timeInterval_p << LogIO::POST;
	}

	if(fdh_p) delete fdh_p;

	// create a FlagDataHandler object
	Table table(msname_p,TableLock(TableLock::AutoNoReadLocking));
	TableInfo& info = table.tableInfo();
	String type = info.type();
	table.flush();
	table.relinquishAutoLocks(true);
	table.unlock();
	os << LogIO::NORMAL << "Table type is " << type << LogIO::POST;

	// For a measurement set
	if (type == "Measurement Set")
	{
		isMS_p = true;
		fdh_p = new FlagMSHandler(msname_p, iterationApproach_p, timeInterval_p);
	}
	// For a calibration file
	else
	{
		isMS_p = false;
		fdh_p = new FlagCalTableHandler(msname_p, iterationApproach_p, timeInterval_p);
	}

	// Open the MS
	fdh_p->open();

	return true;
}

// ---------------------------------------------------------------------
// AgentFlagger::selectData
// Get a record with data selection parameters and
// Parse it to the FlagDataHandler to select the data
// ---------------------------------------------------------------------
bool
AgentFlagger::selectData(Record selrec)
{

	LogIO os(LogOrigin("AgentFlagger", __FUNCTION__));
	if (dbg)
		os << LogIO::NORMAL << "Called from selectData(Record)" << LogIO::POST;

	if (! fdh_p){
		os << LogIO::SEVERE << "There is no MS attached. Please run af.open first." << LogIO::POST;
		return false;
	}


	if (! selrec.empty()) {

		dataselection_p = selrec;

		// Check if all the parameters are in the record. If not,
		// use the default values
		if (dataselection_p.isDefined("spw"))
			dataselection_p.get("spw", spw_p);
		if (dataselection_p.isDefined("scan"))
			dataselection_p.get("scan", scan_p);
		if (dataselection_p.isDefined("field"))
			dataselection_p.get("field", field_p);
		if (dataselection_p.isDefined("antenna"))
			dataselection_p.get("antenna", antenna_p);
		if (dataselection_p.isDefined("timerange"))
			dataselection_p.get("timerange", timerange_p);
		if (dataselection_p.isDefined("correlation"))
			dataselection_p.get("correlation", correlation_p);
		if (dataselection_p.isDefined("intent"))
			dataselection_p.get("intent", intent_p);
		if (dataselection_p.isDefined("feed"))
			dataselection_p.get("feed", feed_p);
		if (dataselection_p.isDefined("array"))
			dataselection_p.get("array", array_p);
		if (dataselection_p.isDefined("uvrange"))
			dataselection_p.get("uvrange", uvrange_p);
		if (dataselection_p.isDefined("observation"))
			dataselection_p.get("observation", observation_p);

	}

	bool ret_status = true;

	// Set the data selection
	ret_status = fdh_p->setDataSelection(dataselection_p);
	if (!ret_status) {
		os << LogIO::SEVERE << "Failed to set the data selection."
				<< LogIO::POST;
		return false;
	}

	// Select the data
	ret_status = fdh_p->selectData();
	if (!ret_status) {
		os << LogIO::SEVERE << "Failed to select the data."
				<< LogIO::POST;
		return false;
	}


	return true;
}


// ---------------------------------------------------------------------
// AgentFlagger::selectData
// Create a record with data selection parameters.
// ---------------------------------------------------------------------
bool
AgentFlagger::selectData(String field, String spw, String array,
		String feed, String scan, String antenna,
		String uvrange,  String timerange, String correlation,
		String intent, String observation)
{

	LogIO os(LogOrigin("AgentFlagger", __FUNCTION__));

	if (dbg)
		os << LogIO::NORMAL << "Called from selectData(String....)" << LogIO::POST;

	// Create a record with the parameters
	Record selection = Record();

	selection.define("spw", spw);
	selection.define("scan", scan);
	selection.define("field", field);
	selection.define("antenna", antenna);
	selection.define("timerange", timerange);
	selection.define("correlation", correlation);
	selection.define("intent", intent);
	selection.define("feed", feed);
	selection.define("array", array);
	selection.define("uvrange", uvrange);
	selection.define("observation", observation);

	// Call the main selectData() method
	selectData(selection);

	return true;

}


// ---------------------------------------------------------------------
// AgentFlagger::parseAgentParameters
// Create a vector of agents and parameters
// Each input record contains data selection parameters
// and agent's specific parameters
// ---------------------------------------------------------------------
bool
AgentFlagger::parseAgentParameters(Record agent_params)
{
	LogIO os(LogOrigin("AgentFlagger", __FUNCTION__));

	if (! fdh_p){
		os << LogIO::SEVERE << "There is no MS attached. Please run af.open first." << LogIO::POST;
		return false;
	}

	// Default values for some parameters
	String mode = "";
	String agent_name = "";
	Bool apply = true;
	String datacolumn = "DATA";

	// create a temporary vector of agents
	std::vector<Record> listOfAgents;


	if(agent_params.empty()){
		os << LogIO::SEVERE << "No agent's record has been provided."
				<< LogIO::POST;
		return false;
	}

	// Use the given Record of parameters
	agentParams_p = agent_params;

	if (! agentParams_p.isDefined("mode")) {
		os << LogIO::WARN << "No mode has been provided"
				<< LogIO::POST;
		return false;
	}

	// Get mode
	agentParams_p.get("mode", mode);

	// Validate mode against known modes
	if (! isModeValid(mode)){
		os << LogIO::WARN << "Mode "<< mode << " is not valid or doesn't exist"
				<< LogIO::POST;
		return false;
	}

    // Name for the logging output
    if (! agentParams_p.isDefined("agentname")){
        agent_name = mode;
        agent_name.capitalize();
        agentParams_p.define("agentname", agent_name);
    }

    agentParams_p.get("agentname", agent_name);

    os << LogIO::DEBUG1 << "Parsing parameters for agent: "<<mode<< LogIO::POST;

	// Validate datacolumn
	if (mode.compare("tfcrop") == 0 or mode.compare("clip") == 0 or mode.compare("rflag") == 0
			or mode.compare("antint") == 0 or mode.compare("display") == 0){

		if (agentParams_p.isDefined("datacolumn"))
			agentParams_p.get("datacolumn", datacolumn);

		os << LogIO::NORMAL3 << "Validating data column "<<datacolumn<<" based on input type"<< LogIO::POST;

		if(!validateDataColumn(datacolumn)){
			os << LogIO::WARN<< "Cannot parse agent parameters "<< LogIO::POST;
			return false;
		}

		os << LogIO::DEBUG1 << "Will use data column "<< datacolumn <<  LogIO::POST;
		agentParams_p.define("datacolumn", datacolumn);
	}

	// Enforce a default value for the apply parameter
	if (! agentParams_p.isDefined("apply")){
		agentParams_p.define("apply", apply);
	}

	// If there is a tfcrop, extend or rflag agent in the list,
	// get the maximum value of ntime and the combinescans parameter
	if (mode.compare("tfcrop") == 0 or mode.compare("extend") == 0 or mode.compare("rflag") == 0) {
		Double ntime;
		if (agentParams_p.isDefined("ntime")){
			agentParams_p.get("ntime", ntime);
			getMax(ntime);
		}

		// Get the combinescans parameter. If any of them is true,
		// it will be true for the whole list
		Bool combine = false;
		if (agentParams_p.isDefined("combinescans"))
			agentParams_p.get("combinescans", combine);

		combinescans_p = combinescans_p || combine;

		os << LogIO::DEBUG2 << "max ntime="<<max_p<<" and combinescans="<<
				combinescans_p << LogIO::POST;

	}

	// Activate async i/o if tfcrop/clip/rflag is present
	if (mode.compare("tfcrop") == 0 or mode.compare("clip") == 0 or mode.compare("rflag") == 0)
	{
		fdh_p->enableAsyncIO(true);
	}

	// Default for all modes
	String correlation = "";

	// Make correlation always uppercase
	if (agentParams_p.isDefined("correlation")) {
		agentParams_p.get("correlation", correlation);
		correlation.upcase();
		agentParams_p.define("correlation", correlation);
	}


	// Create one agent for each polarization
	if (mode.compare("tfcrop") == 0) {

		if (not agentParams_p.isDefined("correlation")) {
			// Default for tfcrop
			correlation = "ABS_ALL";
			if (datacolumn.compare("FLOAT_DATA") == 0)
			    correlation = "REAL_ALL";
			agentParams_p.define("correlation", correlation);
		}
		if (dbg){
			cout << "mode="<<mode<<" correlation="<<correlation<<endl;
		}

		// Is the expression polarization an ALL?
		if (isExpressionPolarizationAll(correlation)) {

			// Get the complex unitary function (ABS, NORM, REAL, IMAG, ARG)
			String function = getExpressionFunction(correlation);

			// Get all the polarizations in the MS
			std::vector<String> *allpol = fdh_p->corrProducts_p;

			for (size_t i=0; i < allpol->size(); i++){
				// compose the full expression
				String func = function;
				String pol = allpol->at(i);
				String exp = func.append("_");
				exp = func.append(pol);

				// Save the record to a list of agents
				agentParams_p.define("correlation", exp);
				String name = agent_name;
				name = name.append("_");
				name = name.append(pol);
				agentParams_p.define("agentname", name);
				listOfAgents.push_back(agentParams_p);
			}
		}
	}

	// Check if extend agent should be added to the list later
	if (mode.compare("tfcrop") == 0 or mode.compare("rflag") == 0){
		if (agentParams_p.isDefined("extendflags"))
				agentParams_p.get("extendflags", extendflags_p);
	}

	if (listOfAgents.size() > 0) {
		// add the agent(s) to the list
		for (size_t i=0; i < listOfAgents.size(); i++) {
			agents_config_list_p.push_back(listOfAgents.at(i));
		}
	}
	else {
		agents_config_list_p.push_back(agentParams_p);
	}

	if (dbg){
		for (size_t i=0; i < agents_config_list_p.size(); i++) {
			ostringstream os;
			agents_config_list_p.at(i).print(os);
			String str(os.str());
			cout << str << endl;
		}
	}

	// Keep a copy of the vector of Agents parameters
	agents_config_list_copy_p = agents_config_list_p;

	// Parse the parameters of the extend agent
	if ((mode.compare("tfcrop") == 0 or mode.compare("rflag") == 0) and
			extendflags_p==true){

		os << LogIO::NORMAL << "Will extend the flags after " << mode <<
				"; extendflags=true" << LogIO::POST;

		extendflags_p = false;
		parseExtendParameters(
				"", // field
				"",	// spw
				"",	// array
				"",	// feed
				"",	// scan
				"",	// antenna
				"",	// uvrange
				"",	// timerange
				"",	// correlation
				"",	// intent
				"",	// observation
				max_p,	// ntime
				combinescans_p,	// combinescans
				true,	// extendpols
				50.0,	// growtime
				80.0	// growfreq
//				false,	// growaround
//				false,	// flagneartime
//				false,	// flagnearfreq
//				true	// apply
				);
	}

	return true;
}

/*
 * Build a string with a list of agents, for logging purposes. Uses the names they have in
 * the mode parameter, for example display, rflag, summary, etc. rather than the more
 * internal names of the agent classes. When the list of agents is very long, a short
 * cut of the list is produced.
 *
 * @param agents list of agents (items as casacore records, with a "mode" field)
 * @return a comma separated list of agents
 */
std::string buildListAgentNames(const std::vector<Record> &agents) {
    const size_t MAX_LIST_PRINT = 10;

    std::string all;
    if (!agents.empty()) {
        auto elem = agents.cbegin();
        String mode;
        elem->get("mode", mode);
        all += mode;
        size_t count = 1;

        while (++elem != agents.cend() && ++count <= MAX_LIST_PRINT) {
            elem->get("mode", mode);
            all += ", " + mode;
        }
        if (agents.size() > MAX_LIST_PRINT) {
            all += ", ... (" + std::to_string(agents.size()-MAX_LIST_PRINT) +
                " more, not shown)";
        }
    }
    return all;
}

/*
 * For a mode (agent) configuration check:
 * if using time or channel averaging in auto-flagging modes (clip, tfcrop, rflag),
 * the agents can only be used in combination with a subset of other agents. These
 * 'whitelisted' agents are: + display + extend + antint.
 * See CAS-12294 for discussions.
 *
 * Additionally, check that timeavg and channelavg are never set for
 * any of the non-autof-lagging methods. This is in principle never
 * possible via the task interface but the list mode parser is weaks
 * and accepts such misconfigurations.

 *
 * @param mode flagging mode, using flagdata naming convention
 * @param agent_rec record with the agent configuration
 * @param anyNotAvg non-empty name if any avg-disallowed agent is in the list
 *
 * @throws AipsError if there is any configuration error
 */
void AgentFlagger::checkAveragingConfig(const std::string &mode, const Record &agent_rec,
                                        const std::string &anyNotAvg) {
    Bool tavg = false;
    int exists = agent_rec.fieldNumber ("timeavg");
    if (exists >= 0) {
        agent_rec.get("timeavg", tavg);
    }

    Bool cavg = false;
    exists = agent_rec.fieldNumber ("channelavg");
    if (exists >= 0) {
        agent_rec.get("channelavg", cavg);
    }

    if ("clip" == mode or "rflag" == mode or "tfcrop" == mode) {
        if ((tavg or cavg) and not anyNotAvg.empty()) {
            ostringstream msg;
            msg << "Cannot use " << mode << " mode with timeavg=True or channelavg=True "
                "and additional modes other than extend, display, and antint. timeavg="
                << tavg << ", channelavg=" << cavg <<  ", and the following mode has been "
                "set up: " << anyNotAvg + ". Refusing to accept this configuration.";
            throw AipsError(msg);
        }
    } else {
        if ((tavg or cavg)) {
            ostringstream msg;
            msg << "Cannot use timeavg or channelavg in mode " << mode << ". timeavg="
                << tavg << ", channelavg=" << cavg <<  ". These averaging "
                "options are only allowed for auto-flagging methods. Refusing this "
                "configuration.";
            throw AipsError(msg);
        }
    }
}

/*
 * Check: the auto-flagging agents (clip, tfcrop, rflag) can only be combined with some
 * other selected agents: display + extend + antint.
 * See CAS-12294 for discussions.
 *
 * @param configs list of agent configurations
 *
 * @throws AipsError if there is any configuration error
 */
std::string AgentFlagger::searchAnyAgentsNotAvg(const std::vector<Record> &configs)
{
    const std::vector<std::string> whitelist = { "clip", "rflag", "tfcrop", "extend", "display", "antint" };

    for (const auto &agent : configs) {
        String recname;
        agent.get("mode", recname);
        std::string name = recname;
        const auto &found = std::find(whitelist.cbegin(), whitelist.cend(), name);
        if (found == whitelist.cend()) {
            return name;
        }
    }

    return "";
}

// ---------------------------------------------------------------------
// AgentFlagger::initAgents
// Initialize the Agents
// Call parseAgentParameters and selectData first
// ---------------------------------------------------------------------
bool
AgentFlagger::initAgents()
{

	LogIO os(LogOrigin("AgentFlagger",__FUNCTION__));

	if (! fdh_p){
		os << LogIO::SEVERE << "There is no MS attached. Please run af.open first." << LogIO::POST;
		return false;
	}

	if (dataselection_p.empty()){
		os << LogIO::SEVERE << "There is no MS Selection available." << LogIO::POST;
		return false;
	}

	if (agents_config_list_p.empty()){
		return false;
	}


	os << LogIO::NORMAL << "There are initially "<< agents_config_list_p.size()<<
            " agents in the list. Agents: "
           << buildListAgentNames(agents_config_list_p) << LogIO::POST;


        // Check once here, to re-use then with every agent in the list
        const auto &anyAgentNotAvg = searchAnyAgentsNotAvg(agents_config_list_p);

	size_t list_size = agents_config_list_p.size();

	// Send the logging of the re-applying agents to the debug
	// as we are only interested in seeing the unapply action (flagcmd)
	Bool retstate = true;
        // Just to log this info
        std::vector<Record> agents_config_list_filtered;
        // To set some parameters only once
        bool itersetDone = false;
        bool timesetDone = false;
	// Loop through the vector of agents
	for (size_t i=0; i < list_size; i++) {

		// Get agent record
		Record agent_rec = agents_config_list_p[i];
		if (dbg){
			os<< LogIO::NORMAL<< "Record["<<i<<"].nfields()="<<agent_rec.nfields()<<LogIO::POST;
			ostringstream os;
			agent_rec.print(os);
			String str(os.str());
			cout << str << endl;

		}

		// Get the mode
		String mode;
		agent_rec.get("mode", mode);

        /*
         * Special constraints for some agents
         */

        // constraints that will produce exceptions if not met
        checkAveragingConfig(mode, agent_rec, anyAgentNotAvg);

        // If quack mode with quackincrement = true, skip it
        if (mode.compare("quack") == 0 and i > 0 and list_size > 1){
        	Bool quackincrement = false;
        	int exists = agent_rec.fieldNumber ("quackincrement");
        	if (exists >= 0) agent_rec.get("quackincrement", quackincrement);

                if (quackincrement){
                    os << LogIO::WARN << "Cannot have quackincrement=True in list mode. Agent quack will be ignored!" << LogIO::POST;
                    continue;
        	}
        }

		// Set the new time interval only once
		if (!timesetDone and (mode.compare("tfcrop") == 0 or mode.compare("extend") == 0 or
				mode.compare("rflag") == 0)) {
			fdh_p->setTimeInterval(max_p);
			timesetDone = true;
		}

		// Change the new iteration approach only once
		if (!itersetDone and (mode.compare("tfcrop") == 0 or mode.compare("extend") == 0
				or mode.compare("rflag") == 0 or mode.compare("display") == 0)) {
			if (combinescans_p)
				fdh_p->setIterationApproach(FlagDataHandler::COMBINE_SCANS_MAP_ANTENNA_PAIRS_ONLY);
			else
				fdh_p->setIterationApproach(FlagDataHandler::COMPLETE_SCAN_MAP_ANTENNA_PAIRS_ONLY);

			itersetDone = true;
		}

		// Agent's name
		String agent_name;
		agent_rec.get("agentname",agent_name);

		FlagAgentBase *fa = 0;

		try
		{
			// CAS-3943 (flagdata seg-faults when non-existent data column is to be read)
			Bool createAgent = true;
			if (((mode.compare("tfcrop") == 0 or mode.compare("rflag") == 0
					or mode.compare("clip") == 0 or mode.compare("display") == 0 or
			      mode.compare("antint") == 0))
					and (agent_rec.fieldNumber ("datacolumn") >= 0))
			{
				String datacolumn;
				agent_rec.get("datacolumn", datacolumn);
				datacolumn.upcase();

				if ((datacolumn.compare("CORRECTED") == 0) or (datacolumn.compare("RESIDUAL") == 0))
				{
					createAgent = fdh_p->checkIfColumnExists(MS::columnName(MS::CORRECTED_DATA));
					if (!createAgent)
					{
						os << LogIO::WARN << "Agent " << agent_name << " cannot be created, necessary CORRECTED_DATA column is not available" << LogIO::POST;
						os << LogIO::WARN << "If wishing to flag residuals, use datacolumn=\'RESIDUAL_DATA\', which will calculate DATA-MODEL" << LogIO::POST;
					}
				}
			}

			// Create this agent if the necessary columns are available
			if (createAgent)
			{
				FlagAgentBase *tfa = FlagAgentBase::create(fdh_p, agent_rec);
				fa = tfa;
			}
		}
		catch(AipsError x)
		{
			fa = 0;
			// Send out a useful message, and stop adding agents to the list.
			// All valid agents before the problematic one, will remain in agents_list_p
			// A subsequent call to initAgents() will add to the list.
			ostringstream oss;
			agent_rec.print(oss);
			String recstr(oss.str());
			while(recstr.contains('\n'))
			{
				recstr = recstr.replace(recstr.index('\n'),1,", ");
			}

			os << LogIO::WARN << "Cannot create agent "<< agent_name << ": "
					<< x.getMesg() << LogIO::POST;
			os << LogIO::DEBUG1<< "Input parameters : " << recstr << LogIO::POST;
			continue;

		}

		if (fa == 0){
			os << LogIO::WARN << "Agent "<< agent_name<< " is NULL. Skipping it."<<LogIO::POST;
			continue;
		}

/*
		// Get the last summary agent to list the results back to the task
		if (mode.compare("summary") == 0) {
			summaryAgent_p = (FlagAgentSummary *) fa;
		}
*/

		// Get the display agent.
		if (mode.compare("display") == 0){
			displayAgent_p = (FlagAgentDisplay *) fa;
		}

		// Add the agent to the FlagAgentList
		agents_list_p.push_back(fa);
                agents_config_list_filtered.push_back(agent_rec);
	}

	os << LogIO::NORMAL << "There are " << agents_list_p.size()
	   << " valid agents in the list. Agents: "
	   << buildListAgentNames(agents_config_list_filtered) << LogIO::POST;

	// Clear the list so that this method cannot be called twice
	agents_config_list_p.clear();

	if (agents_list_p.size() < 1)
		retstate = false;

	return retstate;
}


// ---------------------------------------------------------------------
// AgentFlagger::run
// Run the agents
// It assumes that initAgents has been called first
// ---------------------------------------------------------------------
Record
AgentFlagger::run(Bool writeflags, Bool sequential)
{

	LogIO os(LogOrigin("AgentFlagger", __FUNCTION__));

	if (! fdh_p){
		os << LogIO::SEVERE << "There is no MS attached. Please run af.open first." << LogIO::POST;
		return Record();
	}

	if (agents_list_p.empty()) {
		os << LogIO::SEVERE << "There is no agent to run in list"<< LogIO::POST;
		return Record();
	}


	// Use the maximum ntime of the list
	os << LogIO::DEBUG2 << "ntime for all agents will be "<< max_p << LogIO::POST;
	os << LogIO::DEBUG2 << "combinescans for all agents will be "<< combinescans_p << LogIO::POST;

	// Report to return
	FlagReport combinedReport;


	// Generate the iterators
	// It will iterate through the data to evaluate the necessary memory
	// and get the START and STOP values of the scans for the quack agent
	fdh_p->generateIterator();

	agents_list_p.start();
	os << LogIO::DEBUGGING << "Generated iterators. Start loop over chunks / agents. "
            "Size of agents list is " << agents_list_p.size() << LogIO::POST;

	// iterate over chunks
	while (fdh_p->nextChunk())
	{
		// iterate over visBuffers
		while (fdh_p->nextBuffer())
		{

			// Apply or unapply the flags, in sequential or in parallel.
			// By default it is set to true, which will preserve the order
			// in which the agents are added to the list.
			agents_list_p.apply(sequential);

			// Flush flags to MS
			if (writeflags) {
				fdh_p->flushFlags();
                        }
		}

		// Print the chunk summary stats
		if (fdh_p->summarySignal())
		{
			agents_list_p.chunkSummary();
			fdh_p->progressCounts_p = 0;
		}
	}

	// Print the MS summary stats
	agents_list_p.tableSummary();
	if (writeflags)
		os << LogIO::NORMAL << "=> " << "Writing flags to the MS" << LogIO::POST;

	agents_list_p.terminate();
	agents_list_p.join();

	// Gather the display reports from all agents
	combinedReport = agents_list_p.gatherReports();

	// Send reports to display agent
	if (displayAgent_p)
		displayAgent_p->displayReports(combinedReport);

	agents_list_p.clear();

	return combinedReport;
}

// ---------------------------------------------------------------------
// AgentFlagger::isExpressionPolarizationAll
// Returns true if expression contains a polarization ALL
//
// ---------------------------------------------------------------------
bool
AgentFlagger::isExpressionPolarizationAll(String correlation)
{

	if (correlation.find("ALL") == string::npos){
		return false;
	}

	return true;
}

// ---------------------------------------------------------------------
// AgentFlagger::getExpressionFunction
// Get the unitary function of a polarization expression
// returns a String with the function name
//
// ---------------------------------------------------------------------
String
AgentFlagger::getExpressionFunction(String expression)
{

	String func;

	// Parse complex unitary function
	if (expression.find("REAL") != string::npos)
	{
		func = "REAL";
	}
	else if (expression.find("IMAG") != string::npos)
	{
		func = "IMAG";
	}
	else if (expression.find("ARG") != string::npos)
	{
		func = "ARG";
	}
	else if (expression.find("ABS") != string::npos)
	{
		func = "ABS";
	}
	else if (expression.find("NORM") != string::npos)
	{
		func = "NORM";
	}
	else
	{
		return "";
	}

	return func;
}

// ---------------------------------------------------------------------
// AgentFlagger::getMax
// Get the maximum between two values and
// assign it to the max_p class member.
// ---------------------------------------------------------------------
void
AgentFlagger::getMax(Double value)
{
	if (value > max_p)
		max_p = value;

	return;

}


// ---------------------------------------------------------------------
// AgentFlagger::getFlagVersionList
// Get the flag versions list from the file FLAG_VERSION_LIST in the
// MS directory
//
// ---------------------------------------------------------------------
bool
AgentFlagger::getFlagVersionList(Vector<String> &verlist)
{

	LogIO os(LogOrigin("AgentFlagger", __FUNCTION__, WHERE));

	try
	{

		if (! fdh_p){
			os << LogIO::SEVERE << "There is no MS attached. Please run af.open first." << LogIO::POST;
			return false;
		}

		verlist.resize(0);
		Int num;

		FlagVersion fv(fdh_p->getTableName(),"FLAG","FLAG_ROW");
		Vector<String> vlist = fv.getVersionList();

		num = verlist.nelements();
		verlist.resize( num + vlist.nelements() + 1, true );
		verlist[num] = String("\nMS : ") + fdh_p->getTableName() + String("\n");

		for(Int j=0; j<(Int)vlist.nelements(); j++)
			verlist[num+j+1] = vlist[j];
	}
	catch (AipsError x)
	{
		os << LogIO::SEVERE << "Could not get Flag Version List : " << x.getMesg() << LogIO::POST;
		return false;
	}

	return true;
}


// ---------------------------------------------------------------------
// AgentFlagger::printFlagSelection
// Print the flag versions list
//
// ---------------------------------------------------------------------
bool
AgentFlagger::printFlagSelections()
{

	LogIO os(LogOrigin("AgentFlagger", __FUNCTION__, WHERE));

	// Use the copy of the agent records list because the original
	// was deallocated in the init() method.
	if (! agents_config_list_copy_p.empty())
	{
		os << "Current list of agents : "  << LogIO::POST;

		// TODO: loop through list
		// Duplicate the vector... ???
		for (size_t i=0; i < agents_config_list_copy_p.size(); i++) {
			ostringstream out;
			Record agent_rec;
			agent_rec = agents_config_list_copy_p.at(i);
			agent_rec.print(out);
			os << out.str() << LogIO::POST;
		}
		if (dbg)
			cout << "Size of original list " << agents_config_list_copy_p.size() << endl;

	}
	else os << " No current agents " << LogIO::POST;

	return true;
}


// ---------------------------------------------------------------------
// AgentFlagger::saveFlagVersion
// Save the flag version
//
// ---------------------------------------------------------------------
bool
AgentFlagger::saveFlagVersion(String versionname, String comment, String merge)
{
	LogIO os(LogOrigin("AgentFlagger", __FUNCTION__, WHERE));

	if (! fdh_p){
		os << LogIO::SEVERE << "There is no MS attached. Please run af.open first." << LogIO::POST;
		return false;
	}

	FlagVersion fv(fdh_p->getTableName(),"FLAG","FLAG_ROW");
	fv.saveFlagVersion(versionname, comment, merge);

	return true;
}


// ---------------------------------------------------------------------
// AgentFlagger::restoreFlagVersion
// Restore the flag version
//
// ---------------------------------------------------------------------
bool
AgentFlagger::restoreFlagVersion(Vector<String> versionname, String merge)
{
	LogIO os(LogOrigin("AgentFlagger", __FUNCTION__, WHERE));

	if (! fdh_p){
		os << LogIO::SEVERE << "There is no MS attached. Please run af.open first." << LogIO::POST;
		return false;
	}

	try
	{
		FlagVersion fv(fdh_p->getTableName(),"FLAG","FLAG_ROW");
		for(Int j=0;j<(Int)versionname.nelements();j++)
			fv.restoreFlagVersion(versionname[j], merge);
	}
	catch (AipsError x)
	{
		os << LogIO::SEVERE << "Could not restore Flag Version : " << x.getMesg() << LogIO::POST;
		throw AipsError(x);
//		return false;
	}
	return true;
}

// ---------------------------------------------------------------------
// AgentFlagger::deleteFlagVersion
// Delete the flag version
//
// ---------------------------------------------------------------------
bool
AgentFlagger::deleteFlagVersion(Vector<String> versionname)
{

	LogIO os(LogOrigin("AgentFlagger", __FUNCTION__, WHERE));

	if (! fdh_p){
		os << LogIO::SEVERE << "There is no MS attached. Please run af.open first." << LogIO::POST;
		return false;
	}

	try
	{
		FlagVersion fv(fdh_p->getTableName(),"FLAG","FLAG_ROW");
		for(Int j=0;j<(Int)versionname.nelements();j++)
			fv.deleteFlagVersion(versionname[j]);
	}
	catch (AipsError x)
	{
		os << LogIO::SEVERE << "Could not delete Flag Version : " << x.getMesg() << LogIO::POST;
		return false;
	}
	return true;
}

// ---------------------------------------------------------------------
// AgentFlagger::isModeValid
// Check if mode is valid.
// Return false if not in the list
// ---------------------------------------------------------------------
bool
AgentFlagger::isModeValid(String mode)
{
	bool ret;

	// The validation depends on the type of input
	if (isMS_p) {
		if (mode.compare("manual") == 0 or mode.compare("clip") == 0 or
				mode.compare("quack") == 0 or mode.compare("shadow") == 0 or
				mode.compare("elevation") == 0 or 
				mode.compare("tfcrop") == 0 or 
				mode.compare("extend") == 0 or mode.compare("rflag") == 0 or
				mode.compare("antint") == 0 or 
				mode.compare("unflag") == 0 or mode.compare("summary") == 0
				or mode.compare("display") == 0) {

			ret = true;
		}
		else
			ret = false;
	}
	else {
		if (mode.compare("manual") == 0 or mode.compare("clip") == 0 or
				mode.compare("quack") == 0 or mode.compare("tfcrop") == 0 or
				mode.compare("extend") == 0 or mode.compare("rflag") == 0 or
				mode.compare("antint") == 0 or 
				mode.compare("unflag") == 0 or mode.compare("summary") == 0
				or mode.compare("display") == 0) {

			ret = true;
		}
		else
			ret = false;

	}

	return ret;
}


Bool
AgentFlagger::validateDataColumn(String datacol)
{

    String datacolumn = "";
    Bool ret = false;
    datacol.upcase();

    LogIO os(LogOrigin("AgentFlagger", __FUNCTION__));

    // The validation depends on the type of input
    if (isMS_p) {
        if (datacol.compare("DATA") == 0)
            datacolumn = "DATA";
        else if(datacol.compare("CORRECTED") == 0)
            datacolumn = "CORRECTED_DATA";
        else if(datacol.compare("MODEL") == 0)
            datacolumn = "MODEL_DATA";
         else if(datacol.compare("FLOAT_DATA") == 0)
            datacolumn = "FLOAT_DATA";
        else if(datacol.compare("WEIGHT_SPECTRUM") == 0)
            datacolumn = "WEIGHT_SPECTRUM";
        else if(datacol.compare("WEIGHT") == 0)
            datacolumn = "WEIGHT";
        else if(datacol.compare("RESIDUAL") == 0)
             datacolumn = "CORRECTED_DATA";
         else if(datacol.compare("RESIDUAL_DATA") == 0)
             datacolumn = "DATA";
       else
            datacolumn = "";
    }
    else {
        // cal tables
        if (datacol.compare("FPARAM") == 0 or (datacol.compare("CPARAM") == 0) or
                (datacol.compare("SNR") == 0))
            datacolumn = datacol;
    }

    // Check if main tables exist
    if (fdh_p->checkIfColumnExists(datacolumn)) {
            ret = true;
    }
    else {
    	os << LogIO::WARN << "Data column "<< datacolumn << " does not exist in input data"  << LogIO::POST;
    }

    // Check if other columns were requested also
    // RESIDUAL is calculated later from CORRECTED-MODEL
    // RESIDUAL_DATA is calculated later from DATA-MODEL

    if (datacol.compare("RESIDUAL") == 0 or datacol.compare("RESIDUAL_DATA")==0){
    	ret = false;
    	// check if  MODEL _DATA or virtual MODEL_DATA exist
    	if (fdh_p->checkIfColumnExists("MODEL_DATA") or fdh_p->checkIfSourceModelColumnExists()){
    		ret = true;
    	}
    	else {
	        os << LogIO::WARN << "Data column MODEL_DATA or virtual MODEL_DATA  does not exist in input data" << LogIO::POST;
    	}

    }

    return ret;
}


// ---------------------------------------------------------------------
// AgentFlagger::parseManualParameters
// Parse data selection parameters and specific manual parameters
//
// ---------------------------------------------------------------------
bool
AgentFlagger::parseManualParameters(String field, String spw, String array,
		String feed, String scan, String antenna,
		String uvrange,  String timerange, String correlation,
		String intent, String observation, Bool autocorr,
		Bool apply)
{

	LogIO os(LogOrigin("AgentFlagger", __FUNCTION__));

	// Default values for some parameters
	String mode = "manual";
	String agent_name = "Manual";

	// Create a record with the parameters
	Record agent_record = Record();

	agent_record.define("mode", mode);
	agent_record.define("spw", spw);
	agent_record.define("scan", scan);
	agent_record.define("field", field);
	agent_record.define("antenna", antenna);
	agent_record.define("timerange", timerange);
	agent_record.define("correlation", correlation);
	agent_record.define("intent", intent);
	agent_record.define("feed", feed);
	agent_record.define("array", array);
	agent_record.define("uvrange", uvrange);
	agent_record.define("observation", observation);
	agent_record.define("apply", apply);
	agent_record.define("agentname", agent_name);

	agent_record.define("autocorr", autocorr);

	// Call the main method
	parseAgentParameters(agent_record);

	return true;
}


// ---------------------------------------------------------------------
// AgentFlagger::parseClipParameters
// Parse data selection parameters and specific clip parameters
//
// ---------------------------------------------------------------------
bool
AgentFlagger::parseClipParameters(String field, String spw, String array, String feed, String scan,
		String antenna, String uvrange, String timerange, String correlation,
		String intent, String observation, String datacolumn,
		Vector<Double> clipminmax, Bool clipoutside, Bool channelavg, casac::variant chanbin, Bool timeavg,
		String timebin, Bool clipzeros, Bool apply)
{

	LogIO os(LogOrigin("AgentFlagger", __FUNCTION__));

	// Default values for some parameters
	String mode = "clip";
	String agent_name = "Clip";

	// Create a record with the parameters
	Record agent_record = Record();

	agent_record.define("mode", mode);
	agent_record.define("spw", spw);
	agent_record.define("scan", scan);
	agent_record.define("field", field);
	agent_record.define("antenna", antenna);
	agent_record.define("timerange", timerange);
	agent_record.define("correlation", correlation);
	agent_record.define("intent", intent);
	agent_record.define("feed", feed);
	agent_record.define("array", array);
	agent_record.define("uvrange", uvrange);
	agent_record.define("observation", observation);
	agent_record.define("apply", apply);
	agent_record.define("agentname", agent_name);

	agent_record.define("datacolumn", datacolumn);
	agent_record.define("clipminmax", clipminmax);
	agent_record.define("clipoutside", clipoutside);
	agent_record.define("channelavg", channelavg);

	if (chanbin.type() == casac::variant::INT)
	{
		/*** need range check ***/
		agent_record.define("chanbin", (int) chanbin.toInt());
	}
	else if (chanbin.type() == casac::variant::INTVEC)
	{
		/*** need range check ***/
		auto cb = chanbin.toIntVec( );
        std::vector<int> vec;
        std::for_each( cb.begin( ), cb.end( ), [&](long x){vec.push_back((int)x);} );
		agent_record.define("chanbin", Array<Int>(vec));
	}

    agent_record.define("timeavg", timeavg);
    agent_record.define("timebin", timebin);
	agent_record.define("clipzeros", clipzeros);


	// Call the main method
	parseAgentParameters(agent_record);

	return true;

}

// ---------------------------------------------------------------------
// AgentFlagger::parseQuackParameters
// Parse data selection parameters and specific quack parameters
//
// ---------------------------------------------------------------------
bool
AgentFlagger::parseQuackParameters(String field, String spw, String array, String feed, String scan,
		String antenna, String uvrange, String timerange, String correlation,
		String intent, String observation, String quackmode, Double quackinterval,
		Bool quackincrement, Bool apply)
{

	LogIO os(LogOrigin("AgentFlagger", __FUNCTION__));

	// Default values for some parameters
	String mode = "quack";
	String agent_name = "Quack";

	// Create a record with the parameters
	Record agent_record = Record();

	agent_record.define("mode", mode);
	agent_record.define("spw", spw);
	agent_record.define("scan", scan);
	agent_record.define("field", field);
	agent_record.define("antenna", antenna);
	agent_record.define("timerange", timerange);
	agent_record.define("correlation", correlation);
	agent_record.define("intent", intent);
	agent_record.define("feed", feed);
	agent_record.define("array", array);
	agent_record.define("uvrange", uvrange);
	agent_record.define("observation", observation);
	agent_record.define("apply", apply);
	agent_record.define("agentname", agent_name);

	agent_record.define("quackmode", quackmode);
	agent_record.define("quackinterval", quackinterval);
	agent_record.define("quackincrement", quackincrement);

	// Call the main method
	parseAgentParameters(agent_record);

	return true;

}

// ---------------------------------------------------------------------
// AgentFlagger::parseElevationParameters
// Parse data selection parameters and specific elevation parameters
//
// ---------------------------------------------------------------------
bool
AgentFlagger::parseElevationParameters(String field, String spw, String array, String feed, String scan,
		String antenna, String uvrange, String timerange,String correlation,
		String intent, String observation, Double lowerlimit, Double upperlimit, Bool apply)
{

	LogIO os(LogOrigin("AgentFlagger", __FUNCTION__));

	// Default values for some parameters
	String mode = "elevation";
	String agent_name = "Elevation";

	// Create a record with the parameters
	Record agent_record = Record();

	agent_record.define("mode", mode);
	agent_record.define("spw", spw);
	agent_record.define("scan", scan);
	agent_record.define("field", field);
	agent_record.define("antenna", antenna);
	agent_record.define("timerange", timerange);
	agent_record.define("correlation", correlation);
	agent_record.define("intent", intent);
	agent_record.define("feed", feed);
	agent_record.define("array", array);
	agent_record.define("uvrange", uvrange);
	agent_record.define("observation", observation);
	agent_record.define("apply", apply);
	agent_record.define("agentname", agent_name);

	agent_record.define("lowerlimit", lowerlimit);
	agent_record.define("upperlimit", upperlimit);

	// Call the main method
	parseAgentParameters(agent_record);

	return true;

}

// ---------------------------------------------------------------------
// AgentFlagger::parseTfcropParameters
// Parse data selection parameters and specific tfcrop parameters
//
// ---------------------------------------------------------------------
bool
AgentFlagger::parseTfcropParameters(String field, String spw, String array, String feed, String scan,
		String antenna, String uvrange, String timerange, String correlation,
		String intent, String observation, Double ntime, Bool combinescans,
		String datacolumn, Double timecutoff, Double freqcutoff, String timefit,
		String freqfit, Int maxnpieces, String flagdimension, String usewindowstats, Int halfwin,
		Bool extendflags, Bool apply, Bool channelavg, casac::variant chanbin, Bool timeavg,
   	    String timebin)
{

	LogIO os(LogOrigin("AgentFlagger", __FUNCTION__));

	// Default values for some parameters
	String mode = "tfcrop";
	String agent_name = "Tfcrop";

	// Create a record with the parameters
	Record agent_record = Record();

	agent_record.define("mode", mode);
	agent_record.define("spw", spw);
	agent_record.define("scan", scan);
	agent_record.define("field", field);
	agent_record.define("antenna", antenna);
	agent_record.define("timerange", timerange);
	agent_record.define("correlation", correlation);
	agent_record.define("intent", intent);
	agent_record.define("feed", feed);
	agent_record.define("array", array);
	agent_record.define("uvrange", uvrange);
	agent_record.define("observation", observation);
	agent_record.define("apply", apply);
	agent_record.define("agentname", agent_name);

	agent_record.define("ntime", ntime);
	agent_record.define("combinescans", combinescans);
	agent_record.define("datacolumn", datacolumn);
	agent_record.define("timecutoff", timecutoff);
	agent_record.define("freqcutoff", freqcutoff);
	agent_record.define("timefit", timefit);
	agent_record.define("freqfit", freqfit);
	agent_record.define("maxnpieces", maxnpieces);
	agent_record.define("flagdimension", flagdimension);
	agent_record.define("usewindowstats", usewindowstats);
	agent_record.define("halfwin", halfwin);
	agent_record.define("extendflags", extendflags);
	agent_record.define("apply", apply);

	if (chanbin.type() == casac::variant::INT)
	{
		/*** need range check ***/
		agent_record.define("chanbin", (int) chanbin.toInt());
	}
	else if (chanbin.type() == casac::variant::INTVEC)
	{
		/*** need range check ***/
		auto cb = chanbin.toIntVec( );
        std::vector<int> vec;
        std::for_each( cb.begin( ), cb.end( ), [&](long x){vec.push_back((int)x);} );
		agent_record.define("chanbin", Array<Int>(vec));
	}

	agent_record.define("channelavg", channelavg);
    agent_record.define("timeavg", timeavg);
    agent_record.define("timebin", timebin);

	// Call the main method
	parseAgentParameters(agent_record);

	return true;

}

// ---------------------------------------------------------------------
// AgentFlagger::parseAntIntParameters
// Parse data selection parameters and specific antint parameters
//
// ---------------------------------------------------------------------
bool
AgentFlagger::parseAntIntParameters(String field, String spw, String /* array */, 
				    String /* feed */, String scan, String antenna,
				    String uvrange, String timerange, 
				    String correlation, String intent,
				    String observation, String antint_ref_antenna,
				    Double minchanfrac,
				    Bool verbose, Bool apply)
{

	LogIO os(LogOrigin("AgentFlagger", __FUNCTION__));

	// Mode specific values for some general parameters
	String mode = "antint";
	String agent_name = "AntennaIntegrations";

	// Create a record with the parameters for this mode
	Record agent_record = Record();

	agent_record.define("mode", mode);
	agent_record.define("agentname", agent_name);
	agent_record.define("field", field);
	agent_record.define("spw", spw);

	agent_record.define("scan", scan);
	agent_record.define("antenna", antenna);
	agent_record.define("uvrange", uvrange);
	agent_record.define("timerange", timerange);
	agent_record.define("correlation", correlation);
	agent_record.define("intent", intent);
	agent_record.define("observation", observation);
	agent_record.define("antint_ref_antenna", antint_ref_antenna);
	agent_record.define("minchanfrac", minchanfrac);
	agent_record.define("verbose", verbose);
	agent_record.define("apply", apply);

	// Call the main method
	parseAgentParameters(agent_record);

	return true;
}

// ---------------------------------------------------------------------
// AgentFlagger::parseExtendParameters
// Parse data selection parameters and specific extend parameters
//
// This method is called from parseAgentParameters, and it calls parseAgentParameters
// again, passing a record set up for a "extend" agent. That way, an extend agent
// is added after the main (RFlag or TFcrop) agent.
// ---------------------------------------------------------------------
bool
AgentFlagger::parseExtendParameters(String field, String spw, String array, String feed, String scan,
		String antenna, String uvrange, String timerange, String correlation, String intent,
		String observation, Double ntime, Bool combinescans, Bool extendpols, Double growtime,
		Double growfreq, Bool growaround, Bool flagneartime, Bool flagnearfreq, Bool apply)
{

	LogIO os(LogOrigin("AgentFlagger", __FUNCTION__));

	// Default values for some parameters
	String mode = "extend";
	String agent_name = "Extend";

	// Create a record with the parameters
	Record agent_record = Record();

	agent_record.define("mode", mode);
	agent_record.define("spw", spw);
	agent_record.define("scan", scan);
	agent_record.define("field", field);
	agent_record.define("antenna", antenna);
	agent_record.define("timerange", timerange);
	agent_record.define("correlation", correlation);
	agent_record.define("intent", intent);
	agent_record.define("feed", feed);
	agent_record.define("array", array);
	agent_record.define("uvrange", uvrange);
	agent_record.define("observation", observation);
	agent_record.define("apply", apply);
	agent_record.define("agentname", agent_name);

	agent_record.define("ntime", ntime);
	agent_record.define("combinescans", combinescans);
	agent_record.define("extendpols", extendpols);
	agent_record.define("growtime", growtime);
	agent_record.define("growfreq", growfreq);
	agent_record.define("growaround", growaround);
	agent_record.define("flagneartime", flagneartime);
	agent_record.define("flagnearfreq", flagnearfreq);
	agent_record.define("apply", apply);

	// Call the main method
	parseAgentParameters(agent_record);

	return true;

}



// ---------------------------------------------------------------------
// AgentFlagger::parseSummaryParameters
// Parse data selection parameters and specific summary parameters
//
// ---------------------------------------------------------------------
bool
AgentFlagger::parseSummaryParameters(String field, String spw, String array,
		String feed, String scan, String antenna,
		String uvrange,  String timerange, String correlation,
		String intent, String observation,
		Bool spwchan, Bool spwcorr, Bool basecnt, Bool fieldcnt, String name)
{

	LogIO os(LogOrigin("AgentFlagger", __FUNCTION__));

	// Default values for some parameters
	String mode = "summary";
	String agent_name = "Summary";

	// Create a record with the parameters
	Record agent_record = Record();

	agent_record.define("mode", mode);
	agent_record.define("spw", spw);
	agent_record.define("scan", scan);
	agent_record.define("field", field);
	agent_record.define("antenna", antenna);
	agent_record.define("timerange", timerange);
	agent_record.define("correlation", correlation);
	agent_record.define("intent", intent);
	agent_record.define("feed", feed);
	agent_record.define("array", array);
	agent_record.define("uvrange", uvrange);
	agent_record.define("observation", observation);
	agent_record.define("agentname", agent_name);

	agent_record.define("spwchan", spwchan);
	agent_record.define("spwcorr", spwcorr);
	agent_record.define("basecnt", basecnt);
    agent_record.define("fieldcnt", fieldcnt);
	agent_record.define("name", name);

	// Call the main method
	parseAgentParameters(agent_record);

	return true;
}


} //#end casa namespace
