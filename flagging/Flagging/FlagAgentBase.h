//# FlagAgentBase.h: This file contains the interface definition of the FlagAgentBase class.
//#
//#  CASA - Common Astronomy Software Applications (http://casa.nrao.edu/)
//#  Copyright (C) Associated Universities, Inc. Washington DC, USA 2011, All rights reserved.
//#  Copyright (C) European Southern Observatory, 2011, All rights reserved.
//#
//#  This library is free software; you can redistribute it and/or
//#  modify it under the terms of the GNU Lesser General Public
//#  License as published by the Free software Foundation; either
//#  version 2.1 of the License, or (at your option) any later version.
//#
//#  This library is distributed in the hope that it will be useful,
//#  but WITHOUT ANY WARRANTY, without even the implied warranty of
//#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//#  Lesser General Public License for more details.
//#
//#  You should have received a copy of the GNU Lesser General Public
//#  License along with this library; if not, write to the Free Software
//#  Foundation, Inc., 59 Temple Place, Suite 330, Boston,
//#  MA 02111-1307  USA
//# $Id: $

#ifndef FlagAgentBase_H_
#define FlagAgentBase_H_

#include <map>
// needed for antennaPairMap, polarizationMap, VisMapper, FlagMapper, etc. which should
// probably be split from the rather large FlagDataHandler.h
#include <flagging/Flagging/FlagDataHandler.h>

#include <msvis/MSVis/AsynchronousTools.h>
#include <flagging/Flagging/FlagReport.h>

namespace casa { //# NAMESPACE CASA - BEGIN

class FlagDataHandler;

// <summary>
// A top level class defining the interface for flagging agents
// </summary>
//
// <use visibility=export>
//
// <prerequisite>
//   <li> <linkto class="VisBuffer2:description">FlagDataHandler</linkto>
//   <li> <linkto class="VisBuffer2:description">FlagReport</linkto>
// </prerequisite>
//
// <etymology>
// FlagAgentBase stands for a generic class, specific to the flagging operations
// </etymology>
//
// <synopsis>
//
// This is a top-level class defining the interface for flagging agents.
// There are various methods (virtual) that must be re-implemented by the specific derived
// classes, depending on the implemented algorithm. Here we find three categories:
//
// -# Iteration approach methods:
//
//   - computeRowFlags(const VisBuffer &visBuffer, FlagMapper &flags, casacore::uInt row)
//     - For agents that only depend on meta-data for their flagging operations (for FlagAgentManual,FlagAgentElevation,FlagAgentShadow,FlagAgentQuack)
//     - This iteration method can also be used by agents that have to inspect the already existing flags (for FlagAgentSummary, FlagAgentExtension)
//
//   - computeInRowFlags(const VisBuffer &visBuffer, VisMapper &visibilities,FlagMapper &flags, casacore::uInt row);
//     - For agents that have to look into the visibility points, but regardless of their source baseline, like FlagAgentDisplay
//
//   - computeAntennaPairFlags(const VisBuffer &visBuffer,FlagMapper &flags,casacore::Int antenna1,casacore::Int antenna2,vector<casacore::uInt> &rows);
//     - For agents that have to look into the visibility points grouped by baseline (FlagAgentTimeFreqCrop,FlagAgentRFlag)
//
//   - computeAntennaPairFlags(const VisBuffer &visBuffer, VisMapper &visibilities,FlagMapper &flags,casacore::Int antenna1,casacore::Int antenna2,vector<casacore::uInt> &rows)
//     - For agents that have to look into the visibility points grouped by baseline, allowing user-driven navigation (FlagAgentDisplay)
//     - NOTE: This method has to be used in combination with iterateAntennaPairsInteractive(antennaPairMap *antennaPairMap_ptr)
//
// -# Configuration methods:
//
//   - FlagAgentBase::FlagAgentBase
//     - Even though each derived agent has its specific constructor, it is necessary to call the base class constructor to set:
//       - The FlagDataHandler implementation pointer
//       - The iteration approach: For this the FlagAgentBase class contains an enumeration FlagAgentBase::iteration with the following modes:
//         - FlagAgentBase::ROWS: Iterate row by row and flag depending on the corresponding meta-data (for FlagAgentManual,FlagAgentQuack)
//         - FlagAgentBase::ROWS_PREPROCESS_BUFFER: Iterate row by row doing a pre-processing common to all the rows of each chunk (for FlagAgentElevation, FlagAgentShadow, FlagAgentSummary)
//         - FlagAgentBase::IN_ROWS: Iterate row by row and flag depending on the data column (for FlagAgentClipping)
//         - FlagAgentBase::ANTENNA_PAIRS: Iterate per baselines and flag depending on the data column (for FlagAgentTimeFreqCrop, FlagAgentRFlag)
//         - FlagAgentBase::ANTENNA_PAIRS_FLAGS: Iterate per baselines accessing the individual flag points (for  FlagAgentExtension)
//         - FlagAgentBase::ANTENNA_PAIRS_INTERACTIVE: Iterate per baselines interactively accessing the data column (for FlagAgentDisplay)
//
//   - setAgentParameters(casacore::Record config)
//     - To parse the agent-specific parameters, although there is also an implementation of
//       this method in the base class which has to be called to handle the following parameters:
//        - datacolumn: To specify the column in which the agent has to operate (see FlagAgentBase::datacolumn enumeration)
//        - correlation: To specify the correlation to be inspected for flagging (this also includes visibility expressions)
//        - meta-data selection parameters (field, spw, scan, baseline, etc): To feed the agent-level data selection engine (row filtering)
//
// -# Information methods
//
//   - FlagReport getReport()
//     - To return a specific report to be shown by the display agent, or containing accumulated information (e.g.: summaries, rflag tresholds)
//
// Additionally there are public non-virtual methods to:
//
// - Handle processing in 'background' mode (for parallel flagging)
//   - start(): Start service (start thread run method)
//   - terminate(): Terminate service (forcing run method to finish)
//   - queueProcess(): To signal flagging of the current VisBuffer
//   - completeProcess(): Wait until completion of flagging process for the current VisBuffer
//
// - Print out percentage of flags produced by chunk or in total
//   - chunkSummary(): Accumulates statistics for each chunk
//   - tableSummary(): Accumulates statistics across the entire table selection
//
// </synopsis>
//
// <motivation>
// The motivation for the FlagAgentBase class is having all the iteration and filtering capabilities
// grouped in one single class, with a common interface for all the agents w/o introducing anything
// specific to the implementation of each algorithm, thus improving modularization and maintainability.
// </motivation>
//
//
// <example>
// The top level interface of a flagging agent is quite simple once it is configured, this is due to
// the fact that most of the complexity lies in the FlagDataHandler-FlagAgentBase interaction,
// which is hidden from the application layer (already explained in the FlagDataHandler documentation).
//
// <srcblock>
//
// // Create FlagDataHandler
// FlagDataHandler *dh = new FlagMSHandler(inputFile,iterationMode);
//
// // First of all define a configuration record (e.g.: quack)
// casacore::Record agentConfig;
// agentConfig.define("mode","quack");
// agentConfig.define("quackinterval",(casacore::Double)20);
//
// // Use the factory method to create the agent, and put it into a FlagAgentList
// FlagAgentList agentList;
// FlagAgentBase *agent = FlagAgentBase::create(dh,agentConfig);
// agentList.push_back(agent);
//
// // Iterate over chunks
// while (dh->nextChunk())
// {
//    // Iterates over buffers
//	  while (dh->nextBuffer())
//    {
//       // Apply agents on current VisBuffer
//       agentList.apply();
//
//       // Flush flags (only effective if there is a write access to the flag cube)
//       dh->flushFlags();
//    }
//
//	// Print chunk stats from each agent
//	agentList.chunkSummary();
// }
//
// // Print total stats from each agent
// agentList.tableSummary();
//
// // Stop flag agent
// agentList.terminate();
//
// </srcblock>
// </example>


class FlagAgentBase : public casa::async::Thread {

public:

	enum datacolumn {

		DATA=0,
		CORRECTED,
		MODEL,
		RESIDUAL,
		RESIDUAL_DATA,
		FPARAM,
		CPARAM,
		PARAMERR,
		SNR,
		WEIGHT_SPECTRUM,
		FLOAT_DATA
	};

	enum iteration {

		ROWS=0,
		ROWS_PREPROCESS_BUFFER,
		IN_ROWS,
		IN_ROWS_PREPROCESS_BUFFER,
		ANTENNA_PAIRS,
		ANTENNA_PAIRS_FLAGS,
		ANTENNA_PAIRS_INTERACTIVE,
		ANTENNA_PAIRS_PREPROCESS_BUFFER
	};

	FlagAgentBase(FlagDataHandler *dh, casacore::Record config, casacore::uShort iterationApproach, casacore::Bool writePrivateFlagCube = false, casacore::Bool flag = true);
	virtual ~FlagAgentBase ();
	static FlagAgentBase *create (FlagDataHandler *dh,casacore::Record config);

	void start();
	void terminate ();
	void queueProcess();
	void chunkSummary();
	void tableSummary();
	void completeProcess();
	void * run ();

	// Set function to activate profiling
	void setProfiling(bool enable) {profiling_p = enable;}

	// Set function to activate check mode
	void setCheckMode(bool enable) {checkFlags_p = enable;}

	// Externally visible configuration
	casacore::Bool backgroundMode_p;
	casacore::LogIO::Command logLevel_p;
	casacore::Bool apply_p;
	casacore::Bool flag_p;

        // Get a report casacore::Record from the agent, at the end of the run
        // The report returned by getReport() can be of multiple types
        //   -- a single report of type "none"  : FlagReport("none",agentName_p)
        //   -- a single report of type "plot" : FlagReport("plot",agentName_p)
        //   -- a list of reports  : 
        //          FlagReport repList("list");
        //          repList.addReport( FlagReport("plot",agentName_p) );
        //          repList.addReport( FlagReport("plot",agentName_p) );
        virtual FlagReport getReport();

protected:

	void initialize();

	// Convenience function to be shared by parallel/non-parallel mode
	void runCore();

	void setDataSelection(casacore::Record config);
	// TODO: This class must be re-implemented in the derived classes
	virtual void setAgentParameters(casacore::Record config);
	// Method to sanitize correlation expression and keep going
	casacore::String sanitizeCorrExpression(casacore::String corrExpression, std::vector<casacore::String> *corrProducts);

	void generateAllIndex();
	void generateRowsIndex(casacore::uInt nRows);
	void generateChannelIndex(casacore::uInt nChannels);
	void generatePolarizationIndex(casacore::uInt nPolarizations);
	std::vector<casacore::uInt> * generateAntennaPairRowsIndex(casacore::Int antenna1, casacore::Int antenna2);

	// Generate index for all rows
	void indigen(std::vector<casacore::uInt> &index, casacore::uInt size);

	// For checking ids
	bool find(const casacore::Vector<casacore::Int> &validRange, casacore::Int element);

	// For checking ranges
	bool find(const casacore::Matrix<casacore::Double> &validRange, casacore::Double element);

	// For checking pairs
	bool find(const casacore::Matrix<casacore::Int> &validPairs, casacore::Int element1, casacore::Int element2);

	// For checking columns
	bool find(const casacore::Block<int> &columns, int col);

	// Check if a given number is nan (for visibilities,gains and Tsys primarily)
	bool isNaN(casacore::Double number);
	bool isNaN(casacore::Float number);
	bool isZero(casacore::Double number);
	bool isZero(casacore::Float number);
	bool isNaNOrZero(casacore::Float number);
	bool isNaNOrZero(casacore::Double number);

	// Check if buffer has to be processed
	bool checkIfProcessBuffer();

	// Common functionality for each visBuffer (don't repeat at the row level)
	virtual void preProcessBuffer(const vi::VisBuffer2 &visBuffer);

        // Final consolidation steps after iterating through the buffer rows
        virtual void postProcessBuffer();

	// Iterate trough list of rows
	void iterateRows();

	// Iterate trough visibilities mapper
	void iterateInRows();

	// Iterate trough list of antenna pairs
	void iterateAntennaPairs();

	// Iterate trough list of antenna pairs w/o loading visibilities
	void iterateAntennaPairsFlags();

	// Methods to interactively iterate trough list of antenna pairs
	void processAntennaPair(casacore::Int antenna1,casacore::Int antenna2);
	virtual void iterateAntennaPairsInteractive(antennaPairMap *antennaPairMap_ptr);

	// Iter-passes method
	virtual void passIntermediate(const vi::VisBuffer2 &visBuffer);
	virtual void passFinal(const vi::VisBuffer2 &visBuffer);

	// Mapping functions (abs, real, imag, etc.) as requested by Urvashi
	void setVisibilitiesMap(std::vector<casacore::uInt> *rows,VisMapper *visMap);
	void setFlagsMap(std::vector<casacore::uInt> *rows, FlagMapper *flagMap);
	casacore::Bool checkVisExpression(polarizationMap *polMap);

	// Compute flags for a given visibilities point
	virtual bool computeRowFlags(const vi::VisBuffer2 &visBuffer, FlagMapper &flags, casacore::uInt row);

	// Compute flags for a given visibilities point
	virtual bool computeInRowFlags(const vi::VisBuffer2 &visBuffer, VisMapper &visibilities,FlagMapper &flags, casacore::uInt row);

	// Compute flags for a given (time,freq) antenna pair map
	virtual bool computeAntennaPairFlags(const vi::VisBuffer2 &visBuffer, VisMapper &visibilities,FlagMapper &flags,casacore::Int antenna1,casacore::Int antenna2,std::vector<casacore::uInt> &rows);

	// Compute flags for a given (time,freq) antenna pair map w/o using visibilities
	virtual bool computeAntennaPairFlags(const vi::VisBuffer2 &visBuffer,FlagMapper &flags,casacore::Int antenna1,casacore::Int antenna2,std::vector<casacore::uInt> &rows);

	// Common used members that must be accessible to derived classes
	FlagDataHandler *flagDataHandler_p;
	std::unique_ptr<casacore::LogIO> logger_p;
	casacore::String agentName_p;
	casacore::String summaryName_p;
	casacore::String mode_p;

	// Flag counters
	casacore::uInt64 chunkFlags_p;
	casacore::uInt64 chunkNaNs_p;
	casacore::uInt64 tableFlags_p;
	casacore::uInt64 tableNaNs_p;
	casacore::uInt64 visBufferFlags_p;
	bool flagRow_p;

	// Multithreading configuration and agent id
	casacore::Bool multiThreading_p;
	casacore::Int nThreads_p;
	casacore::Int threadId_p;

	// Running configuration
	casacore::Bool prepass_p;

private:
	
	vi::VisBuffer2 *visibilityBuffer_p;

	// casacore::MS-related objects
	casacore::Cube<casacore::Bool> *commonFlagCube_p;
	casacore::Cube<casacore::Bool> *originalFlagCube_p;
	casacore::Cube<casacore::Bool> *privateFlagCube_p;

	casacore::Vector<casacore::Bool> *commonFlagRow_p;
	casacore::Vector<casacore::Bool> *originalFlagRow_p;
	casacore::Vector<casacore::Bool> *privateFlagRow_p;

	// Own data selection ranges
	casacore::String arraySelection_p;
	casacore::String fieldSelection_p;
	casacore::String scanSelection_p;
	casacore::String timeSelection_p;
	casacore::String spwSelection_p;
	casacore::String channelSelection_p;
	casacore::String baselineSelection_p;
	casacore::String uvwSelection_p;
	casacore::String polarizationSelection_p;
	casacore::String observationSelection_p;
	casacore::String scanIntentSelection_p;
	bool filterRows_p;
	bool filterPols_p;
	bool filterChannels_p;
	bool flagAutoCorrelations_p;
	casacore::Bool antennaNegation_p;

	// Own data selection indexes
	casacore::Vector<casacore::Int> arrayList_p;
	casacore::Vector<casacore::Int> fieldList_p;
	casacore::Vector<casacore::Int> scanList_p;
	casacore::Matrix<casacore::Double> timeList_p;
	casacore::Vector<casacore::Int> spwList_p;
	casacore::Matrix<casacore::Int> channelList_p;
	casacore::Vector<casacore::Int> antenna1List_p;
	casacore::Vector<casacore::Int> antenna2List_p;
	casacore::Matrix<casacore::Int> baselineList_p;
	casacore::Matrix<casacore::Double> uvwList_p;
	casacore::Bool uvwUnits_p;
    std::map<casacore::Int, casacore::Vector<casacore::Int> > polarizationList_p;
	casacore::Vector<casacore::Int> observationList_p;
	casacore::Vector<casacore::Int> scanIntentList_p;

	// Thread state parameters
	volatile casacore::Bool terminationRequested_p;
	volatile casacore::Bool threadTerminated_p;
	volatile casacore::Bool processing_p;

	// casacore::Data source configuration
	// selection expression to pass to the VisMapper
	casacore::String expression_p;
	casacore::uShort dataReference_p;

	// Debugging configuration
	casacore::Bool profiling_p;
	casacore::Bool checkFlags_p;

	// Running mode configuration
	casacore::uShort iterationApproach_p;

	// Flagging mode configuration
	casacore::Bool writePrivateFlagCube_p;

protected:
	// Lists of elements to process
	// jagonzal (CAS-4312): We need channelIndex_p available for the Rflag agent,
	// in order to take into account channel selection for the frequency mapping
    std::vector<casacore::uInt> rowsIndex_p;
    std::vector<casacore::uInt> channelIndex_p;
    std::vector<casacore::uInt> polarizationIndex_p;

	// Needed to be protected for timeavg in clip
    casacore::String dataColumn_p;

	// Pre-averaging parameters
	casacore::Bool timeavg_p;
	casacore::Double timebin_p;
	casacore::Bool channelavg_p;
	casacore::Vector<casacore::Int> chanbin_p;


};

class FlagAgentList
{
	public:
		FlagAgentList();
		~FlagAgentList();

		// Methods to mimic vector
		void push_back(FlagAgentBase *agent_i);
		void pop_back();
		void clear();
		bool empty();
		size_t size();

		// Methods to mimic FlagAgentBase
		void start();
		void terminate ();
		void join ();
		void apply(bool sequential = false);
		void chunkSummary();
		void tableSummary();
		void setProfiling(bool enable);
		void setCheckMode(bool enable);

                // Method to accumulate reports from all agents
                FlagReport gatherReports();

	protected:

	private:
                std::vector<FlagAgentBase *> container_p;
                std::vector<FlagAgentBase *>::iterator iterator_p;
};

} //# NAMESPACE CASA - END

#endif /* FlagAgentBase_H_ */

