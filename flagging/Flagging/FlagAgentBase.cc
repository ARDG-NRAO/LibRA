//# FlagAgentBase.h: This file contains the implementation of the FlagAgentBase class.
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

#include <flagging/Flagging/FlagAgentBase.h>

#include <stdcasa/StdCasa/CasacSupport.h>
#include <ms/MSSel/MSSelectionTools.h>

// Headers of every concrete agent, needed for the factory method (create)
#include <flagging/Flagging/FlagAgentTimeFreqCrop.h>
#include <flagging/Flagging/FlagAgentClipping.h>
#include <flagging/Flagging/FlagAgentSummary.h>
#include <flagging/Flagging/FlagAgentManual.h>
#include <flagging/Flagging/FlagAgentElevation.h>
#include <flagging/Flagging/FlagAgentQuack.h>
#include <flagging/Flagging/FlagAgentShadow.h>
#include <flagging/Flagging/FlagAgentExtension.h>
#include <flagging/Flagging/FlagAgentRFlag.h>
#if defined(CASATOOLS)
#include <flagging/Flagging/grpcFlagAgentDisplay.h>
#else
#include <flagging/Flagging/FlagAgentDisplay.h>
#endif
#include <flagging/Flagging/FlagAgentAntennaIntegrations.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

////////////////////////////////////
/// FlagAgentBase implementation ///
////////////////////////////////////

FlagAgentBase::FlagAgentBase(FlagDataHandler *dh, Record config, uShort iterationApproach, Bool writePrivateFlagCube, Bool flag):
    logger_p(new LogIO(LogOrigin("FlagAgentBase",__FUNCTION__,WHERE)))
{
	// Initialize logger
	if (config.fieldNumber ("loglevel") >= 0)
	{
		logLevel_p = (LogIO::Command)config.asuChar("loglevel");
	}
	else if (agentName_p.empty())
	{
		logLevel_p = LogIO::NORMAL;
	}

	// Initialize members
	initialize();

	// Set iteration approach
	iterationApproach_p = iterationApproach;

	// Set private flag cube (needed for flag extension)
	writePrivateFlagCube_p = writePrivateFlagCube;

	// Retrieve apply mode
	if (config.fieldNumber ("apply") >= 0)
	{
		apply_p = config.asBool("apply");
	}
	else
	{
		apply_p = true;
	}

	// Set apply/unapply
	if (apply_p == true)
	{
		flag_p = flag;
	}
	else
	{
		flag_p = !flag;
	}

	// Set flag data handler
	flagDataHandler_p = dh;

	// Set vis buffer
	visibilityBuffer_p = flagDataHandler_p->visibilityBuffer_p;

	// Set agent parameters
	setAgentParameters(config);

	// Set data selection
	setDataSelection(config);

	// Setup time/channel average iterator parameters
	if (timeavg_p)
	{
	    flagDataHandler_p->timeAverageBin_p = timebin_p;
	    flagDataHandler_p->dataColumnType_p = dataColumn_p;
        flagDataHandler_p->setTimeAverageIter(true);
	}

	// Setup time/channel average iterator parameters
	if (channelavg_p)
	{
		flagDataHandler_p->setChanAverageIter(chanbin_p);
		filterChannels_p=false; // Done in the iterator
	}


	// Check if async processing is enabled
	backgroundMode_p = false;
	AipsrcValue<Bool>::find (backgroundMode_p,"FlagAgent.background", false);

	if (backgroundMode_p)
	{
		*logger_p << LogIO::DEBUG1 << " Background mode enabled" << LogIO::POST;
	}
	else
	{
		*logger_p << LogIO::DEBUG1 << " Background mode disabled" << LogIO::POST;
	}
}

FlagAgentBase::~FlagAgentBase()
{
	if (privateFlagCube_p) delete privateFlagCube_p;
}

void
FlagAgentBase::initialize()
{
   // Initialize members
   flagDataHandler_p = NULL;
   visibilityBuffer_p = NULL;
   privateFlagCube_p = NULL;
   commonFlagCube_p = NULL;
   originalFlagCube_p = NULL;
   privateFlagRow_p = NULL;
   commonFlagRow_p = NULL;
   originalFlagRow_p = NULL;

   // Initialize selection ranges
   timeSelection_p = String("");
   baselineSelection_p = String("");
   fieldSelection_p = String("");
   // NOTE (first implementation): According to MS selection syntax, spw must be at least *
   // but since we are parsing it only if it was provided it should not be a problem
   // NOTE (after Dec 2011 testing): As far as I know spw selection does not have to be *
   // (can be empty) and in fact applying a spw selection slows down the MSSelection class
   spwSelection_p = String("");
   uvwSelection_p = String("");
   polarizationSelection_p = String("");
   scanSelection_p = String("");
   arraySelection_p = String("");
   observationSelection_p = String("");
   scanIntentSelection_p = String("");

   // Clear up indexes
   rowsIndex_p.clear();
   channelIndex_p.clear();
   polarizationIndex_p.clear();

   // Initialize filters
   antennaNegation_p = false;
   filterChannels_p = false;
   filterRows_p = false;
   filterPols_p = false;
   flagAutoCorrelations_p = false;
   uvwUnits_p = true; // Meters

	// Initialize state
   terminationRequested_p = false;
   threadTerminated_p = false;
   processing_p = false;

   // Initialize counters
   chunkFlags_p = 0;
   chunkNaNs_p = 0;
   tableFlags_p = 0;
   tableNaNs_p = 0;
   visBufferFlags_p = 0;

   // Pre-averaging parameters
   timeavg_p = false;
   timebin_p = 0.0;
   channelavg_p = false;
   chanbin_p = Vector<Int>(1,0);

   //// Initialize configuration ////

   /// Running config
   profiling_p = false;
   backgroundMode_p = false;
   iterationApproach_p = ROWS;
   multiThreading_p = false;
   prepass_p = false;
   nThreads_p = 0;
   threadId_p = 0;

   agentName_p = String("");
   summaryName_p = String("");
   /// Flag/Unflag config
   writePrivateFlagCube_p = false;
   flag_p = true;
   /// Mapping config
   dataColumn_p = "data";
   expression_p = "ABS ALL";
   dataReference_p = DATA;
   /// Profiling and testing config
   profiling_p = false;
   checkFlags_p = false;

   /////////////////////////////////

   return;
}

FlagAgentBase *
FlagAgentBase::create (FlagDataHandler *dh,Record config)
{
	String mode;
	FlagAgentBase *ret = NULL;

	// Retrieve mode
	if (config.fieldNumber ("mode") >= 0)
	{
		mode = config.asString("mode");
	}
	else
	{
		cerr << "FlagAgentFactory::" << __FUNCTION__ << " Mode not provided" << endl;
		return ret;
	}

	// Write private flags only if extension is required
	bool writePrivateFlags = false;
	if ((config.fieldNumber ("extend")>=0) and (config.asBool("extend")==true))
	{
		writePrivateFlags = true;
	}
	// Manual mode
	else if (mode.compare("manual")==0)
	{
		FlagAgentManual* agent = new FlagAgentManual(dh,config,writePrivateFlags,true);
		return agent;
	}
	// Unflag mode
	else if (mode.compare("unflag")==0)
	{
		FlagAgentManual* agent = new FlagAgentManual(dh,config,writePrivateFlags,false);
		return agent;
	}
	// TimeFreqCrop
	else if (mode.compare("tfcrop")==0)
	{
		FlagAgentTimeFreqCrop* agent = new FlagAgentTimeFreqCrop(dh,config,writePrivateFlags,true);
		return agent;
	}
	// Clip
	else if (mode.compare("clip")==0)
	{
		FlagAgentClipping* agent = new FlagAgentClipping(dh,config,writePrivateFlags,true);
		return agent;
	}
	// Summary
	else if (mode.compare("summary")==0)
	{
		FlagAgentSummary* agent = new FlagAgentSummary(dh,config);
		return agent;
	}
	// Elevation
	else if (mode.compare("elevation")==0)
	{
		FlagAgentElevation* agent = new FlagAgentElevation(dh,config,writePrivateFlags,true);
		return agent;
	}
	// Quack
	else if (mode.compare("quack")==0)
	{
		FlagAgentQuack* agent = new FlagAgentQuack(dh,config,writePrivateFlags,true);
		return agent;
	}
	// Shadow
	else if (mode.compare("shadow")==0)
	{
		FlagAgentShadow* agent = new FlagAgentShadow(dh,config,writePrivateFlags,true);
		return agent;
	}
	// Extension
	else if (mode.compare("extend")==0)
	{
		FlagAgentExtension* agent = new FlagAgentExtension(dh,config);
		return agent;
	}
	// Rflag
	else if (mode.compare("rflag")==0)
	{
		FlagAgentRFlag* agent = new FlagAgentRFlag(dh,config);
		return agent;
	}
	// Antint
	else if (mode.compare("antint")==0)
	{
		FlagAgentAntennaIntegrations* agent = new FlagAgentAntennaIntegrations(dh,config,writePrivateFlags, true);
		return agent;
	}
	// Display
	else if (mode.compare("display")==0)
	{
		FlagAgentDisplay* agent = new FlagAgentDisplay(dh,config,writePrivateFlags);
		return agent;
	}
	else
	{
		cerr << "FlagAgentFactory::" << __FUNCTION__ << " Mode " << mode << " not supported" << endl;
	}

	return ret;
}

void
FlagAgentBase::start()
{
	if (backgroundMode_p)
	{
		casa::async::Thread::startThread();
	}

	return;
}

void
FlagAgentBase::terminate ()
{
	if (backgroundMode_p)
	{
		terminationRequested_p = true;
		while (!threadTerminated_p)
		{
			sched_yield();
		}

		casa::async::Thread::terminate();
	}

	return;
}

void
FlagAgentBase::queueProcess()
{
	if (backgroundMode_p)
	{
		// Wait until we are done with previous buffer
		while (processing_p)
		{
			sched_yield();
		}

		// Enable processing to trigger flagging
		processing_p = true;
	}
	else
	{
		runCore();
	}

	return;
}

void
FlagAgentBase::completeProcess()
{
	if (backgroundMode_p)
	{
		// Wait until we are done with previous buffer
		while (processing_p)
		{
			sched_yield();
		}
	}

	return;
}

void *
FlagAgentBase::run ()
{
	if (backgroundMode_p)
	{
		while (!terminationRequested_p)
		{

			if (processing_p) // NOTE: This races with queueProcess but it is harmless
			{
				// Carry out processing
				runCore();

				// Disable processing to enter in idle mode
				processing_p = false;
			}
			else
			{
				sched_yield();
			}
		}
	}

	processing_p = false;
	threadTerminated_p = true;

	return NULL;
}

void
FlagAgentBase::runCore()
{
	// Set pointer to common flag cube
	commonFlagCube_p = flagDataHandler_p->getModifiedFlagCube();
	originalFlagCube_p = flagDataHandler_p->getOriginalFlagCube();

	// Set pointer to common flag row
	commonFlagRow_p = flagDataHandler_p->getModifiedFlagRow();
	originalFlagRow_p = flagDataHandler_p->getOriginalFlagRow();

	// Set vis buffer
	visibilityBuffer_p = flagDataHandler_p->visibilityBuffer_p;

	// Reset VisBuffer flag counters
	visBufferFlags_p = 0;

	if (checkIfProcessBuffer())
	{
		// Generate indexes applying data selection filters
		generateAllIndex();

		if ((!rowsIndex_p.size()) || (!channelIndex_p.size()) || (!polarizationIndex_p.size()))
		{
			return;
		}

		// Set pointer to private flag cube
		if (writePrivateFlagCube_p)
		{
			if (privateFlagCube_p) delete privateFlagCube_p;
			privateFlagCube_p = new Cube<Bool>(commonFlagCube_p->shape(),!flag_p);
		}

		switch (iterationApproach_p)
		{
			// Iterate inside every row (i.e. channels) applying a mapping expression
			// clipping
			case IN_ROWS:
			{
				preProcessBuffer(*(flagDataHandler_p->visibilityBuffer_p));
				iterateInRows();
				break;
			}
			// Iterate through rows (i.e. baselines)
			// manual, quack
			case ROWS:
			{
				iterateRows();
				break;
			}
			// Iterate through rows (i.e. baselines) doing a common pre-processing before
			// elevation, shadow, summary, antint
			case ROWS_PREPROCESS_BUFFER:
			{
				preProcessBuffer(*(flagDataHandler_p->visibilityBuffer_p));
				iterateRows();
				postProcessBuffer();
				break;
			}
			// Iterate through (time,freq) maps per antenna pair
			// tfcrop,rflag
			case ANTENNA_PAIRS:
			{
				prepass_p = false;

				iterateAntennaPairs();

				// Do a second pass if the previous one was a pre-pass
				if (prepass_p)
				{
					prepass_p = false;
					passIntermediate(*(flagDataHandler_p->visibilityBuffer_p));
					iterateAntennaPairs();
					passFinal(*(flagDataHandler_p->visibilityBuffer_p));
				}

				break;
			}
			// Iterate through (time,freq) maps per antenna pair
			// extension
			case ANTENNA_PAIRS_FLAGS:
			{
				iterateAntennaPairsFlags();
				break;
			}
			// Navigate through (time,freq) maps per antenna pair
			// display
			case ANTENNA_PAIRS_INTERACTIVE:
			{
				preProcessBuffer(*(flagDataHandler_p->visibilityBuffer_p));
				iterateAntennaPairsInteractive(flagDataHandler_p->getAntennaPairMap());
				break;
			}
			// Iterate through (time,freq) maps per antenna pair doing a common pre-processing before
			// Not used by any of the available agents at the moment
			case ANTENNA_PAIRS_PREPROCESS_BUFFER:
			{
				preProcessBuffer(*(flagDataHandler_p->visibilityBuffer_p));
				iterateAntennaPairs();
				break;
			}
			// Iterate inside every row (i.e. channels) applying a mapping expression doing a common pre-processing before
			// Not used by any of the available agents at the moment
			case IN_ROWS_PREPROCESS_BUFFER:
			{
				preProcessBuffer(*(flagDataHandler_p->visibilityBuffer_p));
				iterateInRows();
				break;
			}
			default:
			{
				throw AipsError("Unknown iteration approach requested");
				break;
			}
		}

		// If any row was flag, then we have to flush the flagRow
		if (flagRow_p) flagDataHandler_p->flushFlagRow_p = true;

		// jagonzal: CAS-3913 We have to reset flagRow
		flagRow_p = false;

		// If any flag was raised, then we have to flush the flagCube
		if (visBufferFlags_p>0) flagDataHandler_p->flushFlags_p = true;

		// Update chunk counter
		chunkFlags_p += visBufferFlags_p;
                if (logger_p->priority() >= LogMessage::DEBUG2 &&
                    visBufferFlags_p > 0) {
                    LogIO os(LogOrigin("FlagAgentBase", __FUNCTION__));
                    os << LogIO::DEBUG2 << " buffer -> chunk flag counter += "
                       << visBufferFlags_p << " (chunk flags: " << chunkFlags_p << ")"
                       << LogIO::POST;
                }
	}

	return;
}

// -----------------------------------------------------------------------
// Set Data Selection parameters
// -----------------------------------------------------------------------
void
FlagAgentBase::setDataSelection(Record config)
{
	logger_p->origin(LogOrigin(agentName_p,__FUNCTION__,WHERE));

	int exists;
	// Set the MS Selection error handler to catch spw IDs or names that are
    // not present in the MS in an expression that contains valid spw values.
	// This will issue a WARNING and not fail.
    MSSelectionLogError mssLESPW;
	MSSelection parser;

	exists = config.fieldNumber ("array");
	if (exists >= 0)
	{
		config.get (config.fieldNumber ("array"), arraySelection_p);

		if (arraySelection_p.empty())
		{
			*logger_p << LogIO::DEBUG1 << " no array selection" << LogIO::POST;
		}
		else
		{
			parser.setArrayExpr(arraySelection_p);
			if (flagDataHandler_p->parseExpression(parser))
			{
				arrayList_p=parser.getSubArrayList();
				filterRows_p=true;

				// Request to pre-load ArrayId
				flagDataHandler_p->preLoadColumn(VisBufferComponent2::ArrayId);

				*logger_p << LogIO::DEBUG1 << " array selection is " << arraySelection_p << LogIO::POST;
				*logger_p << LogIO::DEBUG1 << " array ids are " << arrayList_p << LogIO::POST;
			}
		}
	}
	else
	{
		*logger_p << LogIO::DEBUG1 << " no array selection" << LogIO::POST;
	}

	exists = config.fieldNumber ("field");
	if (exists >= 0)
	{
		config.get (config.fieldNumber ("field"), fieldSelection_p);

		if (fieldSelection_p.empty())
		{
			*logger_p << LogIO::DEBUG1 << " no field selection" << LogIO::POST;
		}
		else
		{
			parser.setFieldExpr(fieldSelection_p);
			if (flagDataHandler_p->parseExpression(parser))
			{
				fieldList_p=parser.getFieldList();
				filterRows_p=true;

				// Request to pre-load FieldId
				flagDataHandler_p->preLoadColumn(VisBufferComponent2::FieldId);

				*logger_p << LogIO::DEBUG1 << " field selection is " << fieldSelection_p << LogIO::POST;
				*logger_p << LogIO::DEBUG1 << " field ids are " << fieldList_p << LogIO::POST;
			}

		}
	}
	else
	{
		*logger_p << LogIO::DEBUG1 << " no field selection" << LogIO::POST;
	}

	exists = config.fieldNumber ("scan");
	if (exists >= 0)
	{
		config.get (config.fieldNumber ("scan"), scanSelection_p);

		if (scanSelection_p.empty())
		{
			*logger_p << LogIO::DEBUG1 << " no scan selection" << LogIO::POST;
		}
		else
		{
			parser.setScanExpr(scanSelection_p);
			if (flagDataHandler_p->parseExpression(parser))
			{
				scanList_p=parser.getScanList();
				filterRows_p=true;

				// Request to pre-load scan
				flagDataHandler_p->preLoadColumn(VisBufferComponent2::Scan);

				*logger_p << LogIO::DEBUG1 << " scan selection is " << scanSelection_p << LogIO::POST;
				*logger_p << LogIO::DEBUG1 << " scan ids are " << scanList_p << LogIO::POST;
			}
		}
	}
	else
	{
		*logger_p << LogIO::DEBUG1 << " no scan selection" << LogIO::POST;
	}

	exists = config.fieldNumber ("timerange");
	if (exists >= 0)
	{
		config.get (config.fieldNumber ("timerange"), timeSelection_p);

		if (timeSelection_p.empty())
		{
			*logger_p << LogIO::DEBUG1 << " no time selection" << LogIO::POST;
		}
		else
		{
			parser.setTimeExpr(timeSelection_p);
			if (flagDataHandler_p->parseExpression(parser))
			{
				timeList_p=parser.getTimeList();
				filterRows_p=true;

				// Request to pre-load time
				flagDataHandler_p->preLoadColumn(VisBufferComponent2::Time);

				*logger_p << LogIO::DEBUG1 << " timerange selection is " << timeSelection_p << LogIO::POST;
				*logger_p << LogIO::DEBUG1 << " time ranges in MJD are " << timeList_p << LogIO::POST;
			}
		}
	}
	else
	{
		*logger_p << LogIO::DEBUG1 << " no time selection" << LogIO::POST;
	}

	exists = config.fieldNumber ("spw");
	if (exists >= 0)
	{
		config.get (config.fieldNumber ("spw"), spwSelection_p);

		if (spwSelection_p.empty())
		{
			*logger_p << LogIO::DEBUG1 << " no spw selection" << LogIO::POST;
		}
		else
		{
		    parser.setErrorHandler(MSSelection::SPW_EXPR, &mssLESPW, true);
			parser.setSpwExpr(spwSelection_p);
			if (flagDataHandler_p->parseExpression(parser))
			{
				spwList_p=parser.getSpwList();
				filterRows_p=true;

				channelList_p=parser.getChanList();
				filterChannels_p=true;

				// Request to pre-load spw
				flagDataHandler_p->preLoadColumn(VisBufferComponent2::SpectralWindows);

				*logger_p << LogIO::DEBUG1 << " spw selection is " << spwSelection_p << LogIO::POST;
				*logger_p << LogIO::DEBUG1 << " channel selection are " << channelList_p << LogIO::POST;
			}
		}
	}
	else
	{
		*logger_p << LogIO::DEBUG1 << " no spw selection" << LogIO::POST;
	}

	exists = config.fieldNumber ("antenna");
	if (exists >= 0)
	{
		config.get (config.fieldNumber ("antenna"), baselineSelection_p);

		if (baselineSelection_p.empty())
		{
			*logger_p << LogIO::DEBUG1 << " no antenna selection" << LogIO::POST;
		}
		else
		{

			// Remove antenna negation operator (!) and set antenna negation flag
			size_t pos = baselineSelection_p.find(String("!"));
			while (pos != String::npos)
			{
				antennaNegation_p = true;
				baselineSelection_p.replace(pos,1,String(""));
				*logger_p << LogIO::DEBUG1 << " antenna selection is the negation of " << baselineSelection_p << LogIO::POST;
				pos = baselineSelection_p.find(String("!"));
			}

			parser.setAntennaExpr(baselineSelection_p);
			if (flagDataHandler_p->parseExpression(parser))
			{
				antenna1List_p=parser.getAntenna1List();
				antenna2List_p=parser.getAntenna2List();
				baselineList_p=parser.getBaselineList();
				filterRows_p=true;

				// Request to pre-load antenna1/2
				flagDataHandler_p->preLoadColumn(VisBufferComponent2::Antenna1);
				flagDataHandler_p->preLoadColumn(VisBufferComponent2::Antenna2);

				*logger_p << LogIO::DEBUG1 << " selected antenna1 list is " << antenna1List_p << LogIO::POST;
				*logger_p << LogIO::DEBUG1 << " selected antenna2 list is " << antenna2List_p << LogIO::POST;
				*logger_p << LogIO::DEBUG1 << " selected baselines are " << baselineList_p << LogIO::POST;
			}
		}
	}
	else
	{
		*logger_p << LogIO::DEBUG1 << " no baseline selection" << LogIO::POST;
	}

	exists = config.fieldNumber ("uvrange");
	if (exists >= 0)
	{
		config.get (config.fieldNumber ("uvrange"), uvwSelection_p);

		if (uvwSelection_p.empty())
		{
			*logger_p << LogIO::DEBUG1 << " no uvw selection" << LogIO::POST;
		}
		else
		{
			parser.setUvDistExpr(uvwSelection_p);
			if (flagDataHandler_p->parseExpression(parser))
			{
				uvwList_p=parser.getUVList();
				Vector<Bool> units = parser.getUVUnitsList();
				if (units[0]==1)
				{
					uvwUnits_p = true; //Meters
				}
				else
				{
					uvwUnits_p = false; //Lambda
				}

				filterRows_p=true;

				// Request to pre-load uvw
				flagDataHandler_p->preLoadColumn(VisBufferComponent2::Uvw);

				*logger_p << LogIO::DEBUG1 << " uvrange selection is " << uvwSelection_p << LogIO::POST;
				*logger_p << LogIO::DEBUG1 << " uvrange ids are " << uvwList_p << LogIO::POST;
				*logger_p << LogIO::DEBUG1 << " uvunits are " << units << LogIO::POST;
			}
		}
	}
	else
	{
		*logger_p << LogIO::DEBUG1 << " no uvw selection" << LogIO::POST;
	}

	exists = config.fieldNumber ("correlation");
	if (exists >= 0)
	{
		config.get (config.fieldNumber ("correlation"), polarizationSelection_p);

		if (polarizationSelection_p.empty())
		{
			*logger_p << LogIO::DEBUG1 << " no correlation selection" << LogIO::POST;
		}


		// Only process the polarization selection as in-row selection if there is no complex operator
		else if ((polarizationSelection_p.find("REAL") == string::npos) and
				(polarizationSelection_p.find("IMAG") == string::npos) and
				(polarizationSelection_p.find("ARG") == string::npos) and
				(polarizationSelection_p.find("ABS") == string::npos) and
				(polarizationSelection_p.find("NORM") == string::npos))
		{
			// jagonzal (CAS-4234): Sanitize correlation expressions
			String sanitizedExpression;
			if (flagDataHandler_p->tableTye_p == FlagDataHandler::MEASUREMENT_SET)
			{
				sanitizedExpression = sanitizeCorrExpression(polarizationSelection_p,flagDataHandler_p->corrProducts_p);
			}
			else
			{
				sanitizedExpression = polarizationSelection_p;
			}

			if (sanitizedExpression.size() > 0)
			{
				polarizationSelection_p = sanitizedExpression;
				parser.setPolnExpr(polarizationSelection_p);
				// parseExpression should not be called for a Cal table
				// until MS Selection can handle correlation parameter for
				// cal tables.
				if (flagDataHandler_p->tableTye_p == FlagDataHandler::MEASUREMENT_SET and
						flagDataHandler_p->parseExpression(parser))
				{

					polarizationList_p=parser.getPolMap();
					filterPols_p=true;

					// Request to pre-load CorrType
					flagDataHandler_p->preLoadColumn(VisBufferComponent2::CorrType);

					ostringstream polarizationListToPrint (ios::in | ios::out);
					for (const auto &item : polarizationList_p)
                                            polarizationListToPrint << item.first << "=" << item.second << " ";
					*logger_p << LogIO::DEBUG1 << " correlation selection is " << polarizationSelection_p << LogIO::POST;
					*logger_p << LogIO::DEBUG1 << " correlation ids are " << polarizationListToPrint.str() << LogIO::POST;
				}
				else {
					*logger_p << LogIO::DEBUG1 << " solution selection is " << polarizationSelection_p << LogIO::POST;
				}

			}
			else
			{
				AipsError exception(String("None of the requested correlation products (" + polarizationSelection_p + ") is available"));
				throw (exception);
			}
		}
	}
	else
	{
		*logger_p << LogIO::DEBUG1 << " no polarization selection" << LogIO::POST;
	}

	exists = config.fieldNumber ("observation");
	if (exists >= 0)
	{
		config.get (config.fieldNumber ("observation"), observationSelection_p);

		if (observationSelection_p.empty())
		{
			*logger_p << LogIO::DEBUG1 << " no observation selection" << LogIO::POST;
		}
		else
		{
			parser.setObservationExpr(observationSelection_p);
			if (flagDataHandler_p->parseExpression(parser))
			{
				observationList_p=parser.getObservationList();
				filterRows_p=true;

				// Request to pre-load ObservationId
				flagDataHandler_p->preLoadColumn(VisBufferComponent2::ObservationId);

				*logger_p << LogIO::DEBUG1 << " observation selection is " << observationList_p << LogIO::POST;
				*logger_p << LogIO::DEBUG1 << " observation ids are " << observationList_p << LogIO::POST;
			}
		}
	}
	else
	{
		*logger_p << LogIO::DEBUG1 << " no observation selection" << LogIO::POST;
	}

	exists = config.fieldNumber ("intent");
	if (exists >= 0)
	{
		config.get (config.fieldNumber ("intent"), scanIntentSelection_p);

		if (scanIntentSelection_p.empty())
		{
			*logger_p << LogIO::DEBUG1 << " no intent selection" << LogIO::POST;
		}
		else
		{
			parser.setStateExpr(scanIntentSelection_p);
			if (flagDataHandler_p->parseExpression(parser))
			{
				scanIntentList_p=parser.getStateObsModeList();
				filterRows_p=true;

				// Request to pre-load StateId
				flagDataHandler_p->preLoadColumn(VisBufferComponent2::StateId);

				*logger_p << LogIO::DEBUG1 << " scan intent selection is " << scanIntentList_p << LogIO::POST;
				*logger_p << LogIO::DEBUG1 << " scan intent ids are " << scanIntentList_p << LogIO::POST;
			}
		}
	}
	else
	{
		*logger_p << LogIO::DEBUG1 << " no scan intent selection" << LogIO::POST;
	}

	return;
}

// -----------------------------------------------------------------------
// Sanitize correlation expression
// -----------------------------------------------------------------------
String
FlagAgentBase::sanitizeCorrExpression(String corrExpression, std::vector<String> *corrProducts)
{
	logger_p->origin(LogOrigin(agentName_p,__FUNCTION__,WHERE));

	String sanitizedExpression = String("");
	bool didSanitize = false;

	if (corrExpression.find("RR") != string::npos)
	{
		if (std::find(corrProducts->begin(),corrProducts->end(),String("RR")) != corrProducts->end())
		{
			if (sanitizedExpression.size() == 0)
			{
				sanitizedExpression += String("RR");
			}
			else
			{
				sanitizedExpression += String(",RR");
			}
		}
		else
		{
			didSanitize = true;
			*logger_p << LogIO::WARN <<  "Correlation product [RR] not available " << LogIO::POST;
		}
	}

	if (corrExpression.find("LL") != string::npos)
	{
		if (std::find(corrProducts->begin(),corrProducts->end(),String("LL")) != corrProducts->end())
		{
			if (sanitizedExpression.size() == 0)
			{
				sanitizedExpression += String("LL");
			}
			else
			{
				sanitizedExpression += String(",LL");
			}
		}
		else
		{
			didSanitize = true;
			*logger_p << LogIO::WARN <<  "Correlation product [LL] not available " << LogIO::POST;
		}
	}

	if (corrExpression.find("RL") != string::npos)
	{
		if (std::find(corrProducts->begin(),corrProducts->end(),String("RL")) != corrProducts->end())
		{
			if (sanitizedExpression.size() == 0)
			{
				sanitizedExpression += String("RL");
			}
			else
			{
				sanitizedExpression += String(",RL");
			}
		}
		else
		{
			didSanitize = true;
			*logger_p << LogIO::WARN <<  "Correlation product [RL] not available " << LogIO::POST;
		}
	}

	if (corrExpression.find("LR") != string::npos)
	{
		if (std::find(corrProducts->begin(),corrProducts->end(),String("LR")) != corrProducts->end())
		{
			if (sanitizedExpression.size() == 0)
			{
				sanitizedExpression += String("LR");
			}
			else
			{
				sanitizedExpression += String(",LR");
			}
		}
		else
		{
			didSanitize = true;
			*logger_p << LogIO::WARN <<  "Correlation product [LR] not available " << LogIO::POST;
		}
	}

	if (corrExpression.find("XX") != string::npos)
	{
		if (std::find(corrProducts->begin(),corrProducts->end(),String("XX")) != corrProducts->end())
		{
			if (sanitizedExpression.size() == 0)
			{
				sanitizedExpression += String("XX");
			}
			else
			{
				sanitizedExpression += String(",XX");
			}
		}
		else
		{
			didSanitize = true;
			*logger_p << LogIO::WARN <<  "Correlation product [XX] not available " << LogIO::POST;
		}
	}

	if (corrExpression.find("YY") != string::npos)
	{
		if (std::find(corrProducts->begin(),corrProducts->end(),String("YY")) != corrProducts->end())
		{
			if (sanitizedExpression.size() == 0)
			{
				sanitizedExpression += String("YY");
			}
			else
			{
				sanitizedExpression += String(",YY");
			}
		}
		else
		{
			didSanitize = true;
			*logger_p << LogIO::WARN <<  "Correlation product [YY] not available " << LogIO::POST;
		}
	}

	if (corrExpression.find("XY") != string::npos)
	{
		if (std::find(corrProducts->begin(),corrProducts->end(),String("XY")) != corrProducts->end())
		{
			if (sanitizedExpression.size() == 0)
			{
				sanitizedExpression += String("XY");
			}
			else
			{
				sanitizedExpression += String(",XY");
			}
		}
		else
		{
			didSanitize = true;
			*logger_p << LogIO::WARN <<  "Correlation product [XY] not available " << LogIO::POST;
		}
	}

	if (corrExpression.find("YX") != string::npos)
	{
		if (std::find(corrProducts->begin(),corrProducts->end(),String("YX")) != corrProducts->end())
		{
			if (sanitizedExpression.size() == 0)
			{
				sanitizedExpression += String("YX");
			}
			else
			{
				sanitizedExpression += String(",YX");
			}
		}
		else
		{
			didSanitize = true;
			*logger_p << LogIO::WARN <<  "Correlation product [YX] not available " << LogIO::POST;
		}
	}

	if (corrExpression.find("I") != string::npos)
	{
		if (std::find(corrProducts->begin(),corrProducts->end(),String("I")) != corrProducts->end())
		{
			if (sanitizedExpression.size() == 0)
			{
				sanitizedExpression += String("I");
			}
			else
			{
				sanitizedExpression += String(",I");
			}
		}
		else
		{
			didSanitize = true;
			*logger_p << LogIO::WARN <<  "Correlation product [I] not available " << LogIO::POST;
		}
	}

	if ( (didSanitize) and (sanitizedExpression.size() > 0) )
	{
		*logger_p << LogIO::NORMAL <<  "Sanitized correlation expression is: " << sanitizedExpression << LogIO::POST;
	}


	return sanitizedExpression;
}

void
FlagAgentBase::setAgentParameters(Record config)
{
	// NOTE: This method must be re-implemented in the derived classes for
	// the specific parameters although here we handle the common ones

	int exists;

    // Retrieve agent name
    exists = config.fieldNumber ("agentname");
    if (exists >= 0)
    {
        agentName_p = config.asString("agentname");
    }
    else if (agentName_p.empty())
    {
        agentName_p = "FlagAgentUnknown";
    }
    logger_p->origin(LogOrigin(agentName_p,__FUNCTION__,WHERE));

    // Retrieve name for summary report
    exists = config.fieldNumber ("name");
    if (exists >= 0)
    {
        summaryName_p = config.asString("name");
    }
    else if (summaryName_p.empty())
    {
        summaryName_p = agentName_p;
    }

	// Retrieve mode
	exists = config.fieldNumber ("mode");
	if (config.fieldNumber ("mode") >= 0)
	{
		mode_p = config.asString("mode");
	}
	else
	{
		mode_p = config.asString("manual");
		*logger_p << LogIO::WARN << " Mode not specified, defaulting to manual" << LogIO::POST;
	}

	exists = config.fieldNumber ("nThreads");
	if (exists >= 0)
	{
		nThreads_p = atoi(config.asString("nThreads").c_str());
		*logger_p << logLevel_p << " nThreads is " << nThreads_p << LogIO::POST;

		if (nThreads_p > 0)
		{
			multiThreading_p = true;
			exists = config.fieldNumber ("threadId");
			if (exists >= 0)
			{
				threadId_p = atoi(config.asString("threadId").c_str());
				*logger_p << logLevel_p << " threadId is " << threadId_p << LogIO::POST;

				if (threadId_p < 0 or threadId_p>=nThreads_p)
				{
					*logger_p << LogIO::WARN << " Thread Id range is [0,nThreads-1], disabling multithreading" << LogIO::POST;
				}
			}
			else
			{
				*logger_p << LogIO::WARN << " Thread Id not provided, disabling multithreading" << LogIO::POST;
				multiThreading_p = false;
			}
		}
		else
		{
			*logger_p << LogIO::WARN << " Number of threads must be positive, disabling multithreading" << LogIO::POST;
			dataColumn_p = "data";
		}
	}


	if (	(iterationApproach_p == IN_ROWS) or
			(iterationApproach_p == ANTENNA_PAIRS) or
			(iterationApproach_p == ANTENNA_PAIRS_INTERACTIVE) or
			(iterationApproach_p == IN_ROWS_PREPROCESS_BUFFER) or
			(iterationApproach_p == ANTENNA_PAIRS_PREPROCESS_BUFFER))
	{

		exists = config.fieldNumber ("datacolumn");
		if (exists >= 0)
		{
			dataColumn_p = config.asString("datacolumn");
		}
		else if (flagDataHandler_p->tableTye_p == FlagDataHandler::MEASUREMENT_SET)
		{
			dataColumn_p = "data";
		}
		else
		{
			dataColumn_p = "fparam";
		}

		dataColumn_p.upcase();

		// Check if dataColumn_p is one of the supported columns (or residues)
		if (dataColumn_p.compare("DATA") == 0)
		{
			dataReference_p = DATA;

			// Request to pre-load ObservedCube
			flagDataHandler_p->preLoadColumn(VisBufferComponent2::VisibilityCubeObserved);
		}
		else if (dataColumn_p.compare("CORRECTED") == 0)
		{
			dataReference_p = CORRECTED;

			// Request to pre-load CorrectedCube
			flagDataHandler_p->preLoadColumn(VisBufferComponent2::VisibilityCubeCorrected);
		}
		else if (dataColumn_p.compare("MODEL") == 0)
		{
			dataReference_p = MODEL;

			// Request to pre-load ModelCube
			flagDataHandler_p->preLoadColumn(VisBufferComponent2::VisibilityCubeModel);
		}
		else if (dataColumn_p.compare("RESIDUAL") == 0)
		{
			dataReference_p = RESIDUAL;

			// Request to pre-load CorrectedCube and ModelCube
			flagDataHandler_p->preLoadColumn(VisBufferComponent2::VisibilityCubeCorrected);
			flagDataHandler_p->preLoadColumn(VisBufferComponent2::VisibilityCubeModel);
		}
		else if (dataColumn_p.compare("RESIDUAL_DATA") == 0)
		{
			dataReference_p = RESIDUAL_DATA;

			// Request to pre-load ObservedCube and ModelCube
			flagDataHandler_p->preLoadColumn(VisBufferComponent2::VisibilityCubeObserved);
			flagDataHandler_p->preLoadColumn(VisBufferComponent2::VisibilityCubeModel);
		}
		else if (dataColumn_p.compare("FPARAM") == 0)
		{
			dataReference_p = DATA;

			// Request to pre-load ObservedCube
			flagDataHandler_p->preLoadColumn(VisBufferComponent2::VisibilityCubeObserved);
		}
		else if (dataColumn_p.compare("CPARAM") == 0)
		{
			dataReference_p = CORRECTED;

			// Request to pre-load CorrectedCube
			flagDataHandler_p->preLoadColumn(VisBufferComponent2::VisibilityCubeCorrected);
		}
		else if (dataColumn_p.compare("SNR") == 0)
		{
			dataReference_p = MODEL;

			// Request to pre-load ModelCube
			flagDataHandler_p->preLoadColumn(VisBufferComponent2::VisibilityCubeModel);
		}
		else if (dataColumn_p.compare("WEIGHT_SPECTRUM") == 0)
		{
			dataReference_p = WEIGHT_SPECTRUM;

			// Request to pre-load WeightSpectrum
			flagDataHandler_p->preLoadColumn(VisBufferComponent2::WeightSpectrum);
		}
		else if (dataColumn_p.compare("WEIGHT") == 0)
		{
			dataReference_p = WEIGHT_SPECTRUM;

			// Request to pre-load WeightSpectrum instead of Weight
			flagDataHandler_p->preLoadColumn(VisBufferComponent2::WeightSpectrum);
		}
		else if (dataColumn_p.compare("FLOAT_DATA") == 0)

		{
			dataReference_p = DATA;

			// Request to pre-load ObservedCube
			flagDataHandler_p->preLoadColumn(VisBufferComponent2::VisibilityCubeObserved);
		}
		else
		{
			*logger_p << LogIO::WARN <<
					" Unsupported data column: " <<
					dataColumn_p << ", using data by default. Supported columns: data,corrected,model,residual,residual_data" << LogIO::POST;
			dataColumn_p = "data";

			// Request to pre-load ObservedCube
			flagDataHandler_p->preLoadColumn(VisBufferComponent2::VisibilityCubeObserved);
		}

		*logger_p << logLevel_p << " data column is " << dataColumn_p << LogIO::POST;

		// Check if user provided an expression
		exists = config.fieldNumber ("correlation");
		if (exists >= 0)
		{
			expression_p = config.asString("correlation");
		}
		else if (flagDataHandler_p->tableTye_p == FlagDataHandler::MEASUREMENT_SET and
		        dataColumn_p.compare("FLOAT_DATA") == 0)
		{
		    expression_p = "REAL ALL";
		}
		else if (flagDataHandler_p->tableTye_p == FlagDataHandler::MEASUREMENT_SET)
		{
			expression_p = "ABS ALL";
		}
		else if (dataColumn_p.compare("CPARAM") == 0)
		{
			// CPARAM is a complex column
			expression_p = "ABS ALL";
		}
		else {
			expression_p = "REAL ALL";
		}

		// Replace empty correlation with default
		if (expression_p.compare("") == 0)
			expression_p = "ALL";

		expression_p.upcase();

		// These are the float columns that do not support complex operators
		// It should fall back to the default REAL
		if (	(dataColumn_p.compare("FPARAM") == 0) or
				(dataColumn_p.compare("SNR") == 0) or
				(dataColumn_p.compare("WEIGHT_SPECTRUM") == 0) or
				(dataColumn_p.compare("WEIGHT") == 0) or
				(dataColumn_p.compare("FLOAT_DATA") == 0))
		{
			// Check if expression is one of the supported operators
			if (	(expression_p.find("IMAG") != string::npos) or
					(expression_p.find("ARG") != string::npos) or
					(expression_p.find("ABS") != string::npos) or
					(expression_p.find("NORM") != string::npos))
			{
				*logger_p 	<< LogIO::WARN
							<< " Unsupported visibility expression: " << expression_p
							<< "; selecting REAL by default. "
							<< " Complex operators are not supported for FLOAT_DATA/FPARAM/SNR/WEIGHT_SPECTRUM/WEIGHT"
							<< LogIO::POST;

				String new_expression;
				if (expression_p.find("_") != string::npos)
					new_expression = expression_p.after("_");
				else
					new_expression = expression_p.after(" ");

				expression_p = "REAL " + new_expression;
			}
			else if (expression_p.find("REAL") == string::npos)
			{
				expression_p = "REAL " + expression_p;
			}
		}
		else
		{
			// Check if expression is one of the supported operators
			if ((expression_p.find("REAL") == string::npos) and
					(expression_p.find("IMAG") == string::npos) and
					(expression_p.find("ARG") == string::npos) and
					(expression_p.find("ABS") == string::npos) and
					(expression_p.find("NORM") == string::npos) and
					// jagonzal: Rflag does not need complex operator
					(mode_p.find("rflag") == string::npos) )
			{
				*logger_p 	<< LogIO::WARN
							<< " Unsupported complex operator: " << expression_p
							<< "; using ABS by default. "
							<< " Supported expressions: REAL,IMAG,ARG,ABS,NORM."
							<< LogIO::POST;
				expression_p = "ABS " + expression_p;
			}
		}


		// Replace "ALL" by applicable correlations
		if (expression_p.find("ALL") != string::npos)
		{
			if (expression_p.find("REAL") != string::npos)
			{
				expression_p = String("REAL ");
			}
			else if (expression_p.find("IMAG") != string::npos)
			{
				expression_p = String("IMAG ");
			}
			else if (expression_p.find("ARG") != string::npos)
			{
				expression_p = String("ARG ");
			}
			else if (expression_p.find("ABS") != string::npos)
			{
				expression_p = String("ABS ");
			}
			else if (expression_p.find("NORM") != string::npos)
			{
				expression_p = String("NORM ");
			}

			bool expressionInitialized = false;
			for (uInt corr_i=0;corr_i<flagDataHandler_p->corrProducts_p->size();corr_i++)
			{
				// jagonzal (CAS-4234): Now we have the I corr product in the list
				// but we have to skip it when expanding the "ABS ALL" expressions
				// because the user must specify WVR implicitly
				if (flagDataHandler_p->corrProducts_p->at(corr_i) != "I")
				{
					if (expressionInitialized)
					{
						expression_p += String(",") + flagDataHandler_p->corrProducts_p->at(corr_i);
					}
					else
					{
						expression_p += flagDataHandler_p->corrProducts_p->at(corr_i);
						expressionInitialized = true;
					}
				}
			}
		}

		expression_p.upcase();

		*logger_p << logLevel_p << " Visibility expression is " << expression_p << LogIO::POST;

		// Request to pre-load spw and corrType
		flagDataHandler_p->preLoadColumn(VisBufferComponent2::SpectralWindows);
		flagDataHandler_p->preLoadColumn(VisBufferComponent2::CorrType);

	}

	exists = config.fieldNumber ("autocorr");
	if (exists >= 0)
	{
		flagAutoCorrelations_p = config.asBool("autocorr");
		*logger_p << logLevel_p << "autocorr is " << flagAutoCorrelations_p
				<< LogIO::POST;
		if (flagAutoCorrelations_p) {
			filterRows_p=true;
			*logger_p << logLevel_p << "Will only apply auto-correlation flagging to data with processor==CORRELATOR"
					<< LogIO::POST;
		}
	}

	// Channel average parameters
	exists = config.fieldNumber ("channelavg");
	if (exists >= 0)
	{
		if( config.type(exists) != TpBool )
		{
			throw( AipsError ( "Parameter 'channelavg' must be of type 'bool'" ) );
		}

		channelavg_p = config.asBool("channelavg");
	}
	else
	{
		channelavg_p = false;
	}


	if (channelavg_p)
	{
		exists = config.fieldNumber ("chanbin");
		if (exists >= 0)
		{
			if ( config.type(exists) == casacore::TpInt )
			{
				Int chanbin;
				config.get (exists, chanbin);
				chanbin_p = Vector<Int>(1,chanbin);
			}
			else if ( config.type(exists) == casacore::TpArrayInt)
			{
				config.get (exists, chanbin_p);
			}
			else
			{
				*logger_p 	<< LogIO::WARN
							<< "Wrong format for chanbin parameter "
							<< " (only Int and arrayInt are supported)" << LogIO::POST;
			}

			*logger_p 	<< LogIO::NORMAL << "Channel average bin is " << chanbin_p << LogIO::POST;
		}
	}


	// Time average parameters
    exists = config.fieldNumber ("timeavg");
    if (exists >= 0)
    {
        if( config.type(exists) != TpBool )
        {
            throw( AipsError ( "Parameter 'timeavg' must be of type 'bool'" ) );
        }

        timeavg_p = config.asBool("timeavg");

    }
    else
    {
        timeavg_p = false;
    }

    if (timeavg_p)
    {
        exists = config.fieldNumber ("timebin");
        if (exists >= 0)
        {
            String timebin;
            config.get(exists, timebin);
            timebin_p = casaQuantity(timebin).get("s").getValue();
        }

		*logger_p 	<< LogIO::NORMAL << "Time average bin is " << timebin_p << LogIO::POST;

     }

	return;
}

void
FlagAgentBase::generateAllIndex()
{
	Int nPolarizations,nChannels,nRows;
	commonFlagCube_p->shape(nPolarizations,nChannels,nRows);
	generateRowsIndex(nRows);
	generateChannelIndex(nChannels);
	generatePolarizationIndex(nPolarizations);

	return;
}

void
FlagAgentBase::generateRowsIndex(uInt nRows)
{
	// For uvw range filter
	Matrix<Double> uvw;

	rowsIndex_p.clear();
	if (filterRows_p)
	{
		Double u,v,uvDistance;
		for (uInt row_i=0;row_i<nRows;row_i++)
		{
			// Check observation id
			if ((observationList_p.size()>0) and (find(flagDataHandler_p->sortOrder_p,MS::OBSERVATION_ID)==false) )
			{
				if (!find(observationList_p,visibilityBuffer_p->observationId()[row_i])) continue;
			}

			// Check scan intent
			if (scanIntentList_p.size())
			{
				if (!find(scanIntentList_p,visibilityBuffer_p->stateId()[row_i])) continue;
			}

			// Check scan id
			if ( (scanList_p.size()>0) and (find(flagDataHandler_p->sortOrder_p,MS::SCAN_NUMBER)==false) )
			{
				if (!find(scanList_p,visibilityBuffer_p->scan()[row_i])) continue;
			}

			// Check time range
			if ( (timeList_p.size()>0) and (flagDataHandler_p->groupTimeSteps_p==true) )
			{
				if (!find(timeList_p,visibilityBuffer_p->time()[row_i])) continue;
			}

			// Check baseline
			if (baselineList_p.size() and (flagDataHandler_p->tableTye_p == FlagDataHandler::MEASUREMENT_SET))
			{
				if (!find(baselineList_p,visibilityBuffer_p->antenna1()[row_i],visibilityBuffer_p->antenna2()[row_i])) continue;
			}
			else if (antenna1List_p.size() and (flagDataHandler_p->tableTye_p == FlagDataHandler::CALIBRATION_TABLE))
			{
				if (!find(antenna1List_p,visibilityBuffer_p->antenna1()[row_i])) continue;
			}

			// Check uvw range
			if (uvwList_p.size())
			{
				// NOTE: uvw from vis buffer is in meters, so we only support uv distance
				// (MS Selection does not return normalized ranges)
				uvw = visibilityBuffer_p->uvw();
				u = uvw(0,row_i);
				v = uvw(1,row_i);
				uvDistance = sqrt(u*u + v*v);

				// CAS-4270: Convert uvdist in lambda units
				if (uvwUnits_p == false)
				{
					Int spw = visibilityBuffer_p->spectralWindows()(0);
					Double Lambda = (*flagDataHandler_p->getLambdaMap())[spw];
					uvDistance /= Lambda;
				}

				if (!find(uvwList_p,uvDistance)) continue;
			}

			// Check auto-correlations
			if (flagAutoCorrelations_p)
			{
				// if not an auto-corr, do not add row to the vector
				if (visibilityBuffer_p->antenna1()[row_i] != visibilityBuffer_p->antenna2()[row_i]) {
					continue;
				}
				// Only for MSs, not for cal tables
				if (flagDataHandler_p->tableTye_p == FlagDataHandler::MEASUREMENT_SET){

					// CAS-5286: only flag auto-corrs when processor TYPE is CORRELATOR
					int proc_id = visibilityBuffer_p->processorId()[row_i];

					if (flagDataHandler_p->processorTableExist_p == true and
							flagDataHandler_p->isCorrelatorType_p(proc_id) == false){
						// skip non-CORRELATOR data
						continue;
					}
				}

			}

			// If all the filters passed, add the row to the list
			rowsIndex_p.push_back(row_i);
		}

	}
	else
	{
		indigen(rowsIndex_p,nRows);
	}

	return;
}

void
FlagAgentBase::generateChannelIndex(uInt nChannels)
{
	channelIndex_p.clear();
	if (filterChannels_p)
	{
		// First find channel start and stop for this spw
		Int currentSpw = visibilityBuffer_p->spectralWindows()(0);
		Int nSpw,width;
		uInt channelStart = 0,channelStop = UINT_MAX;
		channelList_p.shape(nSpw,width);
		for (uShort spw_i=0;spw_i<nSpw;spw_i++)
		{
			if (channelList_p(spw_i,0) == currentSpw)
			{
				channelStart = channelList_p(spw_i,1);
				channelStop = channelList_p(spw_i,2);
				for (uInt channel_i=0;channel_i<nChannels;channel_i++)
				{
					if ((channel_i>=channelStart) and (channel_i<=channelStop)) channelIndex_p.push_back(channel_i);
				}
			}
		}
	}
	else
	{
		indigen(channelIndex_p,nChannels);
	}

	return;
}

void
FlagAgentBase::generatePolarizationIndex(uInt nPolarizations)
{
	polarizationIndex_p.clear();
	if (filterPols_p)
	{
		// NOTE: Polarization ID should be accessible from the visibility buffer
		// but this functionality is not implemented yet, therefore we are getting
		// it from the RW Visibility Iterator which is always a conventional one
		// (not asyn I/O which does not implement it)
		Int polId = visibilityBuffer_p->polarizationId();

                // This will be empty when the 'correlation=' selection takes only
                // polarizations that are not present in the current SPW.
                const auto &polarizations = polarizationList_p[polId];

		// Get accepted polarizations
		for (uInt polarization_i=0;polarization_i<nPolarizations;polarization_i++)
		{
			if (!find(polarizations,polarization_i)) continue;
			polarizationIndex_p.push_back(polarization_i);
		}

	}
	else
	{
		indigen(polarizationIndex_p,nPolarizations);
	}

	return;
}

std::vector<uInt> *
FlagAgentBase::generateAntennaPairRowsIndex(Int antenna1, Int antenna2)
{
	// Retrieve common rows for this antenna pair from FlagDataHandler
	std::vector<uInt> commonRows = (*flagDataHandler_p->getAntennaPairMap())[std::make_pair(antenna1,antenna2)];

	// Second step: Filter out unnecessary rows
	std::vector<uInt> *privateRows = NULL;
	if (filterRows_p)
	{
		privateRows = new std::vector<uInt>();
		for (std::vector<uInt>::iterator iter=commonRows.begin();iter!=commonRows.end();iter++)
		{
			if (std::find(rowsIndex_p.begin(),rowsIndex_p.end(),*iter) != rowsIndex_p.end())
			{
				privateRows->push_back(*iter);
			}
		}
	}
	else
	{
		privateRows = new std::vector<uInt>((*flagDataHandler_p->getAntennaPairMap())[std::make_pair(antenna1,antenna2)]);
	}

	return privateRows;
}

void
FlagAgentBase::indigen(vector<uInt> &index, uInt size)
{
	index.reserve(size);
	for (uInt i=0; i<size; i++ )
	{
		index.push_back(i);
	}

	return;
}

bool
FlagAgentBase::isZero(Float number)
{
	int type = std::fpclassify(number);
	switch (type)
	{
		case FP_NORMAL:
			if (number <= FLT_MIN)
			{
				return true;
			}
			else
			{
				return false;
			}
		case FP_ZERO:
			return true;
		case FP_SUBNORMAL:
			return true;
		case FP_INFINITE:
			return false;
		case FP_NAN:
			return false;
		default:
			return false;
	}
}

bool
FlagAgentBase::isZero(Double number)
{
	int type = std::fpclassify(number);
	switch (type)
	{
		case FP_NORMAL:
			if (number <= FLT_EPSILON)
			{
				return true;
			}
			else
			{
				return false;
			}
		case FP_ZERO:
			return true;
		case FP_SUBNORMAL:
			return true;
		case FP_INFINITE:
			return false;
		case FP_NAN:
			return false;
		default:
			return false;
	}
}

bool
FlagAgentBase::isNaN(Float number)
{
	int type = std::fpclassify(number);
	switch (type)
	{
		case FP_NORMAL:
			return false;
		case FP_ZERO:
			return false;
		case FP_SUBNORMAL:
			return false;
		case FP_INFINITE:
			chunkNaNs_p += 1;
			return true;
		case FP_NAN:
			chunkNaNs_p += 1;
			return true;
		default:
			chunkNaNs_p += 1;
			return true;
	}
}

bool
FlagAgentBase::isNaN(Double number)
{
	int type = std::fpclassify(number);
	switch (type)
	{
		case FP_NORMAL:
			return false;
		case FP_ZERO:
			return false;
		case FP_SUBNORMAL:
			return false;
		case FP_INFINITE:
			chunkNaNs_p += 1;
			return true;
		case FP_NAN:
			chunkNaNs_p += 1;
			return true;
		default:
			chunkNaNs_p += 1;
			return true;
	}
}

bool
FlagAgentBase::isNaNOrZero(Float number)
{
	int type = std::fpclassify(number);
	switch (type)
	{
		case FP_NORMAL:
			if (number <= FLT_EPSILON)
			{
				return true;
			}
			else
			{
				return false;
			}
		case FP_ZERO:
			return true;
		case FP_SUBNORMAL:
			return true;
		case FP_INFINITE:
			chunkNaNs_p += 1;
			return true;
		case FP_NAN:
			chunkNaNs_p += 1;
			return true;
		default:
			chunkNaNs_p += 1;
			return true;
	}
}

bool
FlagAgentBase::isNaNOrZero(Double number)
{
	int type = std::fpclassify(number);
	switch (type)
	{
		case FP_NORMAL:
			if (number <= FLT_EPSILON)
			{
				return true;
			}
			else
			{
				return false;
			}
		case FP_ZERO:
			return true;
		case FP_SUBNORMAL:
			return true;
		case FP_INFINITE:
			chunkNaNs_p += 1;
			return true;
		case FP_NAN:
			chunkNaNs_p += 1;
			return true;
		default:
			chunkNaNs_p += 1;
			return true;
	}
}

void
FlagAgentBase::chunkSummary()
{
    logger_p->origin(LogOrigin(agentName_p,__FUNCTION__));

    // With this check we skip cases like summary or display
    if (chunkFlags_p > 0)
    {
        tableFlags_p +=  chunkFlags_p;
        std::string flagStr = "unflagged";
        if (flag_p) {
            flagStr = "flagged";
        }
        *logger_p << LogIO::NORMAL << "=> "  << "Data " << flagStr << " so far "
                  << 100.0*chunkFlags_p/flagDataHandler_p->progressCounts_p<< "%"
                  << " (" << chunkFlags_p << "/" << flagDataHandler_p->progressCounts_p
                  << ")" << LogIO::POST;
	}

    // Only the clipping agent is capable of detecting this, and besides in general
    // we should not have NaNs, so it is better not to print this log if possible
    if (chunkNaNs_p > 0)
    {
        tableNaNs_p += chunkNaNs_p;
        *logger_p << LogIO::NORMAL << "=> "  << "Number of NaNs detected so far: " <<  (Double)chunkNaNs_p << LogIO::POST;
    }

    chunkFlags_p = 0;
    chunkNaNs_p = 0;
    visBufferFlags_p = 0;
}

void
FlagAgentBase::tableSummary()
{
	logger_p->origin(LogOrigin(agentName_p,__FUNCTION__));

	// Update values just in case chunkSummary was not called before
	tableFlags_p +=  chunkFlags_p;
	tableNaNs_p += chunkNaNs_p;

	// With this check we skip cases like summary or display
	if (tableFlags_p > 0)
	{
		if (flag_p)
		{
			*logger_p << LogIO::NORMAL << "=> "  << "Percentage of data flagged in table selection: " <<  100.0*tableFlags_p/flagDataHandler_p->msCounts_p<< "%" << LogIO::POST;
		}
		else
		{
			*logger_p << LogIO::NORMAL << "=> "  << "Percentage of data un-flagged in table selection: " <<  100.0*tableFlags_p/flagDataHandler_p->msCounts_p<< "%" << LogIO::POST;
		}
	}

	if (tableNaNs_p > 0)
	{
		*logger_p << LogIO::NORMAL << "=> "  << "Total number NaNs detected in table selection: " <<  (Double)tableNaNs_p << LogIO::POST;
	}

	tableFlags_p = 0;
	tableNaNs_p = 0;
	return;
}

bool
FlagAgentBase::find(const Vector<Int> &validRange, Int element)
{
	for (uShort idx=0;idx<validRange.size(); idx++)
	{
		if (element == validRange[idx]) return true;
	}
	return false;
}

bool
FlagAgentBase::find(const Matrix<Double> &validRange, Double element)
{
	IPosition size = validRange.shape();

	for (uInt timeSel_i=0;timeSel_i<size(1);timeSel_i++)
	{
		if (element>=validRange(0,timeSel_i) and element<=validRange(1,timeSel_i)) return true;
	}

	return false;
}

bool
FlagAgentBase::find(const Matrix<Int> &validPairs, Int element1, Int element2)
{
	Int x,y;
	validPairs.shape(x,y);

	for (Int i=0;i<x;i++)
	{
		if ((validPairs(i,0) == element1) and (validPairs(i,1) == element2)) return !antennaNegation_p;
		if ((validPairs(i,0) == element2) and (validPairs(i,1) == element1)) return !antennaNegation_p;
	}

	return antennaNegation_p;
}

bool
FlagAgentBase::find(const Block<int> &columns, int col)
{
	for (uInt i=0; i<columns.nelements(); i++)
	{
		if (columns[i] == col) return true;
	}
	return false;
}

bool
FlagAgentBase::checkIfProcessBuffer()
{
	// array,field and spw are common and unique in a given vis buffer,
	// so we can use them to discard all the rows in a vis buffer.

	if (arrayList_p.size())
	{
		if (!find(arrayList_p,visibilityBuffer_p->arrayId()(0))) return false;
	}

	if (fieldList_p.size())
	{
		if (!find(fieldList_p,visibilityBuffer_p->fieldId()(0))) return false;
	}

	if (spwList_p.size())
	{
		if (!find(spwList_p,visibilityBuffer_p->spectralWindows()(0))) return false;
	}

	// If scan is constant check only 1st row
	if ( (scanList_p.size()>0) and (find(flagDataHandler_p->sortOrder_p,MS::SCAN_NUMBER)==true) )
	{
		if (!find(scanList_p,visibilityBuffer_p->scan()[0])) return false;
	}

	// If observation is constant check only 1st row
	if ((observationList_p.size()>0) and (find(flagDataHandler_p->sortOrder_p,MS::OBSERVATION_ID)==true) )
	{
		if (!find(observationList_p,visibilityBuffer_p->observationId()[0])) return false;
	}

	// If time is constant check only 1st row
	if ( (timeList_p.size()>0) and (flagDataHandler_p->groupTimeSteps_p==false) )
	{
		if (!find(timeList_p,visibilityBuffer_p->time()[0])) return false;
	}

	return true;
}

void
FlagAgentBase::preProcessBuffer(const vi::VisBuffer2 &/*visBuffer*/)
{
}

void
FlagAgentBase::postProcessBuffer()
{
}

void
FlagAgentBase::iterateRows()
{
	logger_p->origin(LogOrigin(agentName_p,__FUNCTION__,WHERE));

	// Create FlagMapper objects and parse the correlation selection
	vector< vector<uInt> > selectedCorrelations;
	for (uInt pol_i=0;pol_i<polarizationIndex_p.size();pol_i++)
	{
		vector<uInt> correlationProduct;
		correlationProduct.push_back(polarizationIndex_p[pol_i]);
		selectedCorrelations.push_back(correlationProduct);
	}
	FlagMapper flagsMap = FlagMapper(flag_p,selectedCorrelations);

	// Set CubeViews in FlagMapper
	setFlagsMap(NULL,&flagsMap);

	// Activate check mode
	if (checkFlags_p) flagsMap.activateCheckMode();

	// Some log info
        const auto logprio = logger_p->priority();
        if (logprio <= LogMessage::DEBUG2) {
            if (multiThreading_p)
            {
		*logger_p << LogIO::DEBUG2 << agentName_p.c_str() << "::" << __FUNCTION__
                          <<  " Thread Id " << threadId_p << ":" << nThreads_p
                          << " Will process every " << nThreads_p << " rows starting with row " << threadId_p << " from a total of " <<
                    rowsIndex_p.size() << " rows (" << rowsIndex_p[0] << "-" << rowsIndex_p[rowsIndex_p.size()-1] << ") " <<
                    channelIndex_p.size() << " channels (" << channelIndex_p[0] << "-" << channelIndex_p[channelIndex_p.size()-1] << ") " <<
                    polarizationIndex_p.size() << " polarizations (" << polarizationIndex_p[0] << "-" << polarizationIndex_p[polarizationIndex_p.size()-1] << ")" << LogIO::POST;
            }
            else
            {
		// Some logging info
		*logger_p 	<< LogIO::DEBUG2 << " Going to process a buffer with: " <<
                    rowsIndex_p.size() << " rows (" << rowsIndex_p[0] << "-" << rowsIndex_p[rowsIndex_p.size()-1] << ") " <<
                    channelIndex_p.size() << " channels (" << channelIndex_p[0] << "-" << channelIndex_p[channelIndex_p.size()-1] << ") " <<
                    polarizationIndex_p.size() << " polarizations (" << polarizationIndex_p[0] << "-" << polarizationIndex_p[polarizationIndex_p.size()-1] << ")" << LogIO::POST;
            }
        }

	// Loop through selected rows
	Int rowIdx = 0;
	bool flagRow = false;
	vector<uInt>::iterator rowIter;
	for (rowIter = rowsIndex_p.begin(); rowIter != rowsIndex_p.end(); ++rowIter)
	{
		if (multiThreading_p and (rowIdx % nThreads_p != threadId_p))
		{
			// Increment row index
			rowIdx++;

			// Continue with next row
			continue;
		}

		// Compute flags for this row
		flagRow = false;
		flagRow = computeRowFlags(*(flagDataHandler_p->visibilityBuffer_p), flagsMap,*rowIter);
		if (flagRow)
		{
			flagsMap.applyFlagInRow(*rowIter);
			visBufferFlags_p += flagsMap.flagsPerRow();
			if ((filterChannels_p == false) and (filterPols_p == false))
			{
				flagsMap.applyFlagRow(*rowIter);
				flagRow_p = true;
			}
		}

		// Increment row index
		rowIdx++;
	}

	return;
}

void
FlagAgentBase::iterateInRows()
{
	logger_p->origin(LogOrigin(agentName_p,__FUNCTION__,WHERE));

	// Check if the visibility expression is suitable for this spw
	if (!checkVisExpression(flagDataHandler_p->getPolarizationMap())) return;

	// Create VisMapper and FlagMapper objects and parse the polarization expression
	VisMapper visibilitiesMap = VisMapper(expression_p,flagDataHandler_p->getPolarizationMap());
	FlagMapper flagsMap = FlagMapper(flag_p,visibilitiesMap.getSelectedCorrelations());

	// Set CubeViews in VisMapper
	setVisibilitiesMap(NULL,&visibilitiesMap);

	// Set CubeViews in FlagMapper
	setFlagsMap(NULL,&flagsMap);

	// Activate check mode
	if (checkFlags_p) flagsMap.activateCheckMode();

	// Some log info
	if (multiThreading_p)
	{
		*logger_p << LogIO::DEBUG2 << agentName_p.c_str() << "::" << __FUNCTION__
				<<  " Thread Id " << threadId_p << ":" << nThreads_p
				<< " Will process every " << nThreads_p << " rows starting with row " << threadId_p
				<< " from a total of " << rowsIndex_p.size() << " rows with " << channelIndex_p.size() << " channels ("
				<< channelIndex_p[0] << "-" << channelIndex_p[channelIndex_p.size()-1] << ") each one" << LogIO::POST;
	}
	else
	{
		// Some logging info
		*logger_p 	<< LogIO::DEBUG2 << " Going to process a buffer with: " <<
				rowsIndex_p.size() << " rows (" << rowsIndex_p[0] << "-" << rowsIndex_p[rowsIndex_p.size()-1] << ") " <<
				channelIndex_p.size() << " channels (" << channelIndex_p[0] << "-" << channelIndex_p[channelIndex_p.size()-1] << ") "<< LogIO::POST;
	}

	// Iterate through rows
	Int rowIdx = 0;
	vector<uInt>::iterator rowIter;
	for (rowIter = rowsIndex_p.begin();rowIter != rowsIndex_p.end();rowIter++)
	{
		if (multiThreading_p and (rowIdx % nThreads_p != threadId_p))
		{
			// Increment row index
			rowIdx++;

			// Continue with next row
			continue;
		}

		// Compute flags for this row
		computeInRowFlags(*(flagDataHandler_p->visibilityBuffer_p),visibilitiesMap,flagsMap,*rowIter);

		// jagonzal (CAS-4913, CAS-5344): If we are unflagging FLAG_ROWS must be unset
		if (not flag_p)
		{
			flagsMap.applyFlagRow(rowIdx);
			flagRow_p = true;
		}

		// Increment row index
		rowIdx++;
	}

	return;
}

void
FlagAgentBase::iterateAntennaPairs()
{
	logger_p->origin(LogOrigin(agentName_p,__FUNCTION__,WHERE));

	// Check if the visibility expression is suitable for this spw
	if (!checkVisExpression(flagDataHandler_p->getPolarizationMap())) return;

	antennaPairMapIterator myAntennaPairMapIterator;
	std::pair<Int,Int> antennaPair;
	std::vector<uInt> *antennaRows = NULL;
	IPosition cubeShape;

	// Create VisMapper and FlagMapper objects and parse the polarization expression
	VisMapper visibilitiesMap = VisMapper(expression_p,flagDataHandler_p->getPolarizationMap());
	FlagMapper flagsMap = FlagMapper(flag_p,visibilitiesMap.getSelectedCorrelations());

	// Activate check mode
	if (checkFlags_p) flagsMap.activateCheckMode();

	// Some log info
	if (multiThreading_p)
	{
		*logger_p << LogIO::DEBUG2 << agentName_p.c_str() << "::" << __FUNCTION__
				<<  " Thread Id " << threadId_p << ":" << nThreads_p
				<< " Will process every " << nThreads_p << " baselines starting with baseline " << threadId_p
				<< " from a total of " << flagDataHandler_p->getAntennaPairMap()->size() << LogIO::POST;
	}
	else
	{
		*logger_p << LogIO::DEBUG2 <<  " Iterating through " << flagDataHandler_p->getAntennaPairMap()->size() <<  " antenna pair maps " << LogIO::POST;
	}


	uShort antennaPairdIdx = 0;
	for (myAntennaPairMapIterator=flagDataHandler_p->getAntennaPairMap()->begin(); myAntennaPairMapIterator != flagDataHandler_p->getAntennaPairMap()->end(); ++myAntennaPairMapIterator)
	{
		if (multiThreading_p and (antennaPairdIdx % nThreads_p != threadId_p))
		{
			// Increment antenna pair index
			antennaPairdIdx++;

			// Continue with next antenna pair
			continue;
		}

		// Get antenna pair from map
		antennaPair = myAntennaPairMapIterator->first;

		// Check if antenna pair is in the baselines list of this agent
		if (baselineList_p.size()>0)
		{
			if (!find(baselineList_p,antennaPair.first,antennaPair.second)) continue;
		}

		// Get rows corresponding to this antenna pair
		antennaRows = generateAntennaPairRowsIndex(antennaPair.first,antennaPair.second);

		// If none of the antenna pair rows were eligible then go to next pair
		if (antennaRows->empty())
		{
			*logger_p << LogIO::WARN <<  " Requested baseline (" << antennaPair.first << "," << antennaPair.second << ") does not have any rows in this chunk" << LogIO::POST;

			// Increment antenna pair index
			antennaPairdIdx++;

			// Delete antenna pair rows
			delete antennaRows;

			// Continue with next antenna pair
			continue;
		}

		// Set CubeViews in VisMapper
		setVisibilitiesMap(antennaRows,&visibilitiesMap);

		// Set CubeViews in FlagMapper
		setFlagsMap(antennaRows,&flagsMap);

		// Flag map
		computeAntennaPairFlags(*(flagDataHandler_p->visibilityBuffer_p),visibilitiesMap,flagsMap,antennaPair.first,antennaPair.second,*antennaRows);

		// Increment antenna pair index
		antennaPairdIdx++;

		// jagonzal (CAS-4913, CAS-5344): If we are unflagging FLAG_ROWS must be unset
		if (not flag_p)
		{
			for (uInt baselineRowIdx=0;baselineRowIdx<antennaRows->size();baselineRowIdx++)
			{
				flagsMap.applyFlagRow(baselineRowIdx);
			}
			flagRow_p = true;
		}


		// Delete antenna pair rows
		delete antennaRows;
	}

	return;
}

void
FlagAgentBase::iterateAntennaPairsFlags()
{
	logger_p->origin(LogOrigin(agentName_p,__FUNCTION__,WHERE));

	antennaPairMapIterator myAntennaPairMapIterator;
	std::pair<Int,Int> antennaPair;
	std::vector<uInt> *antennaRows = NULL;
	IPosition cubeShape;

	// Create VisMapper and FlagMapper objects and parse the polarization expression
	vector< vector<uInt> > selectedCorrelations;
	for (uInt pol_i=0;pol_i<polarizationIndex_p.size();pol_i++)
	{
		vector<uInt> correlationProduct;
		correlationProduct.push_back(polarizationIndex_p[pol_i]);
		selectedCorrelations.push_back(correlationProduct);
	}
	FlagMapper flagsMap = FlagMapper(flag_p,selectedCorrelations);

	// Activate check mode
	if (checkFlags_p) flagsMap.activateCheckMode();

	// Some log info
	if (multiThreading_p)
	{
		*logger_p << LogIO::DEBUG2 << agentName_p.c_str() << "::" << __FUNCTION__
				<<  " Thread Id " << threadId_p << ":" << nThreads_p
				<< " Will process every " << nThreads_p << " baselines starting with baseline " << threadId_p
				<< " from a total of " << flagDataHandler_p->getAntennaPairMap()->size() << LogIO::POST;
	}
	else
	{
		*logger_p << LogIO::DEBUG2 <<  " Iterating through " << flagDataHandler_p->getAntennaPairMap()->size() <<  " antenna pair maps " << LogIO::POST;
	}


	uShort antennaPairdIdx = 0;
	for (myAntennaPairMapIterator=flagDataHandler_p->getAntennaPairMap()->begin(); myAntennaPairMapIterator != flagDataHandler_p->getAntennaPairMap()->end(); ++myAntennaPairMapIterator)
	{
		if (multiThreading_p and (antennaPairdIdx % nThreads_p != threadId_p))
		{
			// Increment antenna pair index
			antennaPairdIdx++;

			// Continue with next antenna pair
			continue;
		}

		// Get antenna pair from map
		antennaPair = myAntennaPairMapIterator->first;

		// Check if antenna pair is in the baselines list of this agent
		if (baselineList_p.size()>0)
		{
			if (!find(baselineList_p,antennaPair.first,antennaPair.second)) continue;
		}

		// Get rows corresponding to this antenna pair
		antennaRows = generateAntennaPairRowsIndex(antennaPair.first,antennaPair.second);

		// If none of the antenna pair rows were eligible then go to next pair
		if (antennaRows->empty())
		{
			*logger_p << LogIO::WARN <<  " Requested baseline (" << antennaPair.first << "," << antennaPair.second << ") does not have any rows in this chunk" << LogIO::POST;

			// Increment antenna pair index
			antennaPairdIdx++;

			// Delete antenna pair rows
			delete antennaRows;

			// Continue with next antenna pair
			continue;
		}

		// Set CubeViews in FlagMapper
		setFlagsMap(antennaRows,&flagsMap);

		// Flag map
		computeAntennaPairFlags(*(flagDataHandler_p->visibilityBuffer_p),flagsMap,antennaPair.first,antennaPair.second,*antennaRows);

		// Increment antenna pair index
		antennaPairdIdx++;

		// jagonzal (CAS-4913, CAS-5344): If we are unflagging FLAG_ROWS must be unset
		if (not flag_p)
		{
			for (uInt baselineRowIdx=0;baselineRowIdx<antennaRows->size();baselineRowIdx++)
			{
				flagsMap.applyFlagRow(baselineRowIdx);
			}
			flagRow_p = true;
		}

		// Delete antenna pair rows
		delete antennaRows;
	}

	return;
}

void
FlagAgentBase::iterateAntennaPairsInteractive(antennaPairMap *antennaPairMap_ptr)
{
	// Check if the visibility expression is suitable for this spw
	if (!checkVisExpression(flagDataHandler_p->getPolarizationMap())) return;

	// Iterate through antenna pair map
	std::pair<Int,Int> antennaPair;
	antennaPairMapIterator myAntennaPairMapIterator;
	for (myAntennaPairMapIterator=antennaPairMap_ptr->begin(); myAntennaPairMapIterator != antennaPairMap_ptr->end(); ++myAntennaPairMapIterator)
	{
		// Get antenna pair from map
		antennaPair = myAntennaPairMapIterator->first;

		// Process antenna pair
		processAntennaPair(antennaPair.first,antennaPair.second);
	}

	return;
}

void
FlagAgentBase::processAntennaPair(Int antenna1,Int antenna2)
{
	logger_p->origin(LogOrigin(agentName_p,__FUNCTION__,WHERE));

	std::pair<Int,Int> antennaPair = std::make_pair(antenna1,antenna2);
	antennaPairMapIterator index = flagDataHandler_p->getAntennaPairMap()->find(antennaPair);
	if (index != flagDataHandler_p->getAntennaPairMap()->end())
	{
		std::vector<uInt> *antennaRows = generateAntennaPairRowsIndex(antennaPair.first,antennaPair.second);
		if (antennaRows->empty())
		{
			*logger_p << LogIO::WARN <<  " Requested baseline (" << antennaPair.first << "," << antennaPair.second << ") does not have any rows in this chunk" << LogIO::POST;
		}
		else
		{
			// Check if antenna pair is in the baselines list of this agent
			if ((baselineList_p.size()>0) and (!find(baselineList_p,antennaPair.first,antennaPair.second)))
			{
				*logger_p << logLevel_p <<  "Requested baseline (" << antennaPair.first << "," << antennaPair.second << ") is not included in the selected baseline range" << LogIO::POST;
			}
			else
			{
				*logger_p << logLevel_p <<  " Going to process requested baseline (" << antennaPair.first << "," << antennaPair.second << ") " << LogIO::POST;

				// Create VisMapper and FlagMapper objects and parse the polarization expression
				VisMapper visibilitiesMap = VisMapper(expression_p,flagDataHandler_p->getPolarizationMap());
				FlagMapper flagsMap = FlagMapper(flag_p,visibilitiesMap.getSelectedCorrelations());

				// Set CubeViews in VisMapper
				setVisibilitiesMap(antennaRows,&visibilitiesMap);

				// Set CubeViews in FlagMapper
				setFlagsMap(antennaRows,&flagsMap);

				// Flag map
				computeAntennaPairFlags(*(flagDataHandler_p->visibilityBuffer_p),visibilitiesMap,flagsMap,antennaPair.first,antennaPair.second,*antennaRows);
			}
		}

                // Delete antenna pair rows - prob generateAntennaPairRowsIndex should
                // not return a pointer, but that requires a long cascade of changes elsewhere
                delete antennaRows;

	}
	else
	{
		*logger_p << LogIO::WARN <<  " Requested baseline (" << antennaPair.first << "," << antennaPair.second << ") is not available in this chunk " << LogIO::POST;
	}

	return;
}

void
FlagAgentBase::passIntermediate(const vi::VisBuffer2 & /*visBuffer*/)
{
	// TODO: This method must be re-implemented in the derived classes
	return;
}

void
FlagAgentBase::passFinal(const vi::VisBuffer2 & /*visBuffer*/)
{
	// TODO: This method must be re-implemented in the derived classes
	return;
}

void
FlagAgentBase::setVisibilitiesMap(std::vector<uInt> *rows,VisMapper *visMap)
{
	// First step: Get reference visibility cubes
	Cube<Complex> *leftVisCube = NULL;
	Cube<Complex> *rightVisCube = NULL;
	switch (dataReference_p)
	{
		case DATA:
		{
			leftVisCube = const_cast<Cube<Complex> *>(&(visibilityBuffer_p->visCube()));
			break;
		}
		case CORRECTED:
		{
			leftVisCube = const_cast<Cube<Complex> *>(&(visibilityBuffer_p->visCubeCorrected()));
			break;
		}
		case MODEL:
		{
			leftVisCube = const_cast<Cube<Complex> *>(&(visibilityBuffer_p->visCubeModel()));
			break;
		}
		case RESIDUAL:
		{
			leftVisCube = const_cast<Cube<Complex> *>(&(visibilityBuffer_p->visCubeCorrected()));
			rightVisCube = const_cast<Cube<Complex> *>(&(visibilityBuffer_p->visCubeModel()));
			break;
		}
		case RESIDUAL_DATA:
		{
			leftVisCube = const_cast<Cube<Complex> *>(&(visibilityBuffer_p->visCube()));
			rightVisCube = const_cast<Cube<Complex> *>(&(visibilityBuffer_p->visCubeModel()));
			break;
		}
		case FPARAM:
		{
			leftVisCube = const_cast<Cube<Complex> *>(&(visibilityBuffer_p->visCube()));
			break;
		}
		case CPARAM:
		{
			leftVisCube = const_cast<Cube<Complex> *>(&(visibilityBuffer_p->visCubeCorrected()));
			break;
		}
		case SNR:
		{
			leftVisCube = const_cast<Cube<Complex> *>(&(visibilityBuffer_p->visCubeModel()));
			break;
		}
		case WEIGHT_SPECTRUM:
		{
			// Cast the Cube<Float> to Cube<Complex> in the DataHandler
			leftVisCube = const_cast<Cube<Complex> *>(&(flagDataHandler_p->weightVisCube()));
			break;
		}
		default:
		{
			leftVisCube = const_cast<Cube<Complex> *>(&(visibilityBuffer_p->visCube()));
			break;
		}
	}

	// Second step create CubeViews from selected vis cubes
	CubeView<Complex> *leftVisCubeView = NULL;
	CubeView<Complex> *rightVisCubeView = NULL;
	leftVisCubeView = new CubeView<Complex>(leftVisCube,rows,&channelIndex_p);
	if (rightVisCube != NULL) rightVisCubeView = new CubeView<Complex>(rightVisCube,rows,&channelIndex_p);

	// Third step: Set CubeViews in mapper
	visMap->setParentCubes(leftVisCubeView,rightVisCubeView);
	return;
}

void
FlagAgentBase::setFlagsMap(std::vector<uInt> *rows,FlagMapper *flagMap)
{
	// First step create common/private CubeViews
	CubeView<Bool> *commonFlagCube = NULL;
	CubeView<Bool> *originalFlagCube = NULL;
	CubeView<Bool> *privateFlagCube = NULL;

	// Second step create CubeViews from selected vis cubes
	commonFlagCube= new CubeView<Bool>(commonFlagCube_p,rows,&channelIndex_p);
	originalFlagCube= new CubeView<Bool>(originalFlagCube_p,rows,&channelIndex_p);
	if (writePrivateFlagCube_p) privateFlagCube= new CubeView<Bool>(privateFlagCube_p,rows,&channelIndex_p);

	// Third step: Set CubeViews in mapper
	flagMap->setParentCubes(commonFlagCube,originalFlagCube,privateFlagCube);

	// 4th step create common/private CubeViews
	VectorView<Bool> *commonFlagRow = NULL;
	VectorView<Bool> *originalFlagRow = NULL;
	VectorView<Bool> *privateFlagRow = NULL;

	// 5th step create CubeViews from selected vis cubes
	commonFlagRow= new VectorView<Bool>(commonFlagRow_p,rows);
	originalFlagRow= new VectorView<Bool>(originalFlagRow_p,rows);
	if (writePrivateFlagCube_p) privateFlagRow= new VectorView<Bool>(privateFlagRow_p,rows);

	// 6th step: Set CubeViews in mapper
	flagMap->setParentFlagRow(commonFlagRow,originalFlagRow,privateFlagRow);

	return;
}

Bool
FlagAgentBase::checkVisExpression(polarizationMap *polMap)
{
	logger_p->origin(LogOrigin(agentName_p,__FUNCTION__,WHERE));

	// If we find I directly in the polarization map we assume is ALMA Water Vapor Radiometer data
	// And we only process it if the user requested WVR
	if (expression_p.find("WVR") != string::npos)
	{
		if (polMap->find(Stokes::I) != polMap->end())
		{
			*logger_p << LogIO::DEBUG1 <<  " Detected Water Vapor data in spw (" <<
					visibilityBuffer_p->spectralWindows()(0) << "), will be flagged" << LogIO::POST;
			return true;
		}
		else
		{
			return false;
		}
	}
	else if (polMap->find(Stokes::I) != polMap->end())
	{
		*logger_p << LogIO::DEBUG1 <<  " Detected Water Vapor data in spw (" <<
					visibilityBuffer_p->spectralWindows()(0) << "), won't be flagged" << LogIO::POST;
		return false;
	}

	// After WVR - I products check we go ahead with the rest of the generic cases
	if (expression_p.find("XX") != string::npos)
	{
		if (polMap->find(Stokes::XX) != polMap->end())
		{
			return true;
		}
		else
		{
			*logger_p << LogIO::WARN <<  " Requested correlation (XX) not available in current spectral window (" <<
					visibilityBuffer_p->spectralWindows()(0) << ") " << LogIO::POST;
			return false;
		}
	}
	else if (expression_p.find("YY") != string::npos)
	{
		if (polMap->find(Stokes::YY) != polMap->end())
		{
			return true;
		}
		else
		{
			*logger_p << LogIO::WARN <<  " Requested correlation (YY) not available in current spectral window (" <<
					visibilityBuffer_p->spectralWindows()(0) << ") " << LogIO::POST;
			return false;
		}

	}
	else if (expression_p.find("XY") != string::npos)
	{
		if (polMap->find(Stokes::XY) != polMap->end())
		{
			return true;
		}
		else
		{
			*logger_p << LogIO::WARN <<  " Requested correlation (XY) not available in current spectral window (" <<
					visibilityBuffer_p->spectralWindows()(0) << ") " << LogIO::POST;
			return false;
		}
	}
	else if (expression_p.find("YX") != string::npos)
	{
		if (polMap->find(Stokes::YX) != polMap->end())
		{
			return true;
		}
		else
		{
			*logger_p << LogIO::WARN <<  " Requested correlation (YX) not available in current spectral window (" <<
					visibilityBuffer_p->spectralWindows()(0) << ") " << LogIO::POST;
			return false;
		}
	}
	else if (expression_p.find("RR") != string::npos)
	{
		if (polMap->find(Stokes::RR) != polMap->end())
		{
			return true;
		}
		else
		{
			*logger_p << LogIO::WARN <<  " Requested correlation (RR) not available in current spectral window (" <<
					visibilityBuffer_p->spectralWindows()(0) << ") " << LogIO::POST;
			return false;
		}
	}
	else if (expression_p.find("LL") != string::npos)
	{
		if (polMap->find(Stokes::LL) != polMap->end())
		{
			return true;
		}
		else
		{
			*logger_p << LogIO::WARN <<  " Requested correlation (LL) not available in current spectral window (" <<
					visibilityBuffer_p->spectralWindows()(0) << ") " << LogIO::POST;
			return false;
		}
	}
	else if (expression_p.find("LR") != string::npos)
	{
		if (polMap->find(Stokes::LR) != polMap->end())
		{
			return true;
		}
		else
		{
			*logger_p << LogIO::WARN <<  " Requested correlation (LR) not available in current spectral window (" <<
					visibilityBuffer_p->spectralWindows()(0) << ") " << LogIO::POST;
			return false;
		}
	}
	else if (expression_p.find("RL") != string::npos)
	{
		if (polMap->find(Stokes::RL) != polMap->end())
		{
			return true;
		}
		else
		{
			*logger_p << LogIO::WARN <<  " Requested correlation (RL) not available in current spectral window (" <<
					visibilityBuffer_p->spectralWindows()(0) << ") " << LogIO::POST;
			return false;
		}
	}
	else if (expression_p.find("I") != string::npos)
	{
		if (polMap->find(Stokes::I) != polMap->end())
		{
			return true;
		}
		else if ((polMap->find(Stokes::XX) != polMap->end()) and (polMap->find(Stokes::YY) != polMap->end()))
		{
			return true;
		}
		else if ((polMap->find(Stokes::RR) != polMap->end()) and (polMap->find(Stokes::LL) != polMap->end()))
		{

			return true;
		}
		else
		{
			*logger_p << LogIO::WARN <<  " Requested Stokes parameter (I) cannot be computed from available polarizations in current spectral window (" <<
					visibilityBuffer_p->spectralWindows()(0) << ") " << LogIO::POST;
			return false;
		}
	}
	else if (expression_p.find("Q") != string::npos)
	{
		if (polMap->find(Stokes::Q) != polMap->end())
		{
			return true;
		}
		else if ((polMap->find(Stokes::XX) != polMap->end()) and (polMap->find(Stokes::YY) != polMap->end()))
		{
			return true;
		}
		else if ((polMap->find(Stokes::RL) != polMap->end()) and (polMap->find(Stokes::LR) != polMap->end()))
		{
			return true;
		}
		else
		{
			*logger_p << LogIO::WARN <<  " Requested Stokes parameter (Q) cannot be computed from available polarizations in current spectral window (" <<
					visibilityBuffer_p->spectralWindows()(0) << ") " << LogIO::POST;
			return false;
		}
	}
	else if (expression_p.find("U") != string::npos)
	{
		if (polMap->find(Stokes::U) != polMap->end())
		{
			return true;
		}
		else if ((polMap->find(Stokes::XY) != polMap->end()) and (polMap->find(Stokes::YX) != polMap->end()))
		{
			return true;
		}
		else if ((polMap->find(Stokes::RL) != polMap->end()) and (polMap->find(Stokes::LR) != polMap->end()))
		{
			return true;
		}
		else
		{
			*logger_p << LogIO::WARN <<  " Requested Stokes parameter (U) cannot be computed from available polarizations in current spectral window (" <<
					visibilityBuffer_p->spectralWindows()(0) << ") " << LogIO::POST;
			return false;
		}
	}
	else if ((expression_p.find("V") != string::npos) and (expression_p.find("WVR") == string::npos))
	{
		if (polMap->find(Stokes::V) != polMap->end())
		{
			return true;
		}
		else if ((polMap->find(Stokes::XY) != polMap->end()) and (polMap->find(Stokes::YX) != polMap->end()))
		{
			return true;
		}
		else if ((polMap->find(Stokes::RR) != polMap->end()) and (polMap->find(Stokes::LL) != polMap->end()))
		{
			return true;
		}
		else
		{
			*logger_p << LogIO::WARN <<  " Requested Stokes parameter (V) cannot be computed from available polarizations in current spectral window (" <<
					visibilityBuffer_p->spectralWindows()(0) << ") " << LogIO::POST;
			return false;
		}
	}
	else if (expression_p.find("SOL1") != string::npos)
	{
		if (polMap->find(VisMapper::CALSOL1) != polMap->end())
		{
			return true;
		}
		else
		{
			*logger_p << LogIO::WARN <<  " Requested Calibration solution element (SOL1) not available" << LogIO::POST;
			return false;
		}
	}
	else if (expression_p.find("SOL2") != string::npos)
	{
		if (polMap->find(VisMapper::CALSOL2) != polMap->end())
		{
			return true;
		}
		else
		{
			*logger_p << LogIO::WARN <<  " Requested Calibration solution element (SOL2) not available" << LogIO::POST;
			return false;
		}
	}
	else if (expression_p.find("SOL3") != string::npos)
	{
		if (polMap->find(VisMapper::CALSOL3) != polMap->end())
		{
			return true;
		}
		else
		{
			*logger_p << LogIO::WARN <<  " Requested Calibration solution element (SOL3) not available" << LogIO::POST;
			return false;
		}
	}
	else if (expression_p.find("SOL4") != string::npos)
	{
		if (polMap->find(VisMapper::CALSOL4) != polMap->end())
		{
			return true;
		}
		else
		{
			*logger_p << LogIO::WARN <<  " Requested Calibration solution element (SOL4) not available" << LogIO::POST;
			return false;
		}
	}
	else
	{
		throw AipsError("Unknown polarization requested, (" + expression_p + ") supported types are: XX,YY,XY,YX,RR,LL,RL,LR,I,Q,U,V");
		return false;
	}
	return false;
}

bool
FlagAgentBase::computeRowFlags(const vi::VisBuffer2 &/*visBuffer*/, FlagMapper &/*flags*/, uInt /*row*/)
{
	// TODO: This method must be re-implemented in the derived classes
	return false;
}

bool
FlagAgentBase::computeInRowFlags(const vi::VisBuffer2 &/*visBuffer*/, VisMapper &/*visibilities*/,
                                 FlagMapper &/*flags*/, uInt /*row*/)
{
	// TODO: This method must be re-implemented in the derived classes
	return false;
}

bool
FlagAgentBase::computeAntennaPairFlags(const vi::VisBuffer2 &/*visBuffer*/, VisMapper &/*visibilities*/,
                                       FlagMapper &/*flags*/,Int /*antenna1*/,Int /*antenna2*/,
                                       vector<uInt> &/*rows*/)
{
	// TODO: This method must be re-implemented in the derived classes
	return false;
}

bool
FlagAgentBase::computeAntennaPairFlags(const vi::VisBuffer2 &/*visBuffer*/,FlagMapper &/*flags*/,
                                       Int /*antenna1*/,Int /*antenna2*/,vector<uInt> &/*rows*/)
{
	// TODO: This method must be re-implemented in the derived classes
	return false;
}

FlagReport
FlagAgentBase::getReport()
{
	// TODO: This method must be re-implemented in the derived classes
	return FlagReport(String("none"),agentName_p);
}


////////////////////////////////////
/// FlagAgentList implementation ///
////////////////////////////////////

FlagAgentList::FlagAgentList()
{
	container_p.clear();
}

FlagAgentList::~FlagAgentList()
{
	clear();
}

void FlagAgentList::push_back(FlagAgentBase *agent_i)
{
	container_p.push_back(agent_i);

	return;
}

void FlagAgentList::pop_back()
{
	container_p.pop_back();

	return;
}

void FlagAgentList::clear()
{
	for (iterator_p = container_p.begin();iterator_p != container_p.end(); iterator_p++)
	{
		delete (*iterator_p);
	}
	container_p.clear();

	return;
}

bool FlagAgentList::empty()
{
	return container_p.empty();
}

size_t FlagAgentList::size()
{
	return container_p.size();
}

void FlagAgentList::start()
{
	for (iterator_p = container_p.begin();iterator_p != container_p.end(); iterator_p++)
	{
		(*iterator_p)->start();
	}
	return;
}

void FlagAgentList::terminate()
{
	for (iterator_p = container_p.begin();iterator_p != container_p.end(); iterator_p++)
	{
		(*iterator_p)->terminate();
	}

	return;
}

void FlagAgentList::join()
{
	for (iterator_p = container_p.begin();iterator_p != container_p.end(); iterator_p++)
	{
		if ((*iterator_p)->backgroundMode_p)
		{
			(*iterator_p)->join();
		}
	}

	return;
}

void FlagAgentList::apply(bool sequential)
{
	if (sequential)
	{
		for (iterator_p = container_p.begin();iterator_p != container_p.end(); iterator_p++)
		{
			(*iterator_p)->queueProcess();
			(*iterator_p)->completeProcess();
		}
	}
	else
	{
		for (iterator_p = container_p.begin();iterator_p != container_p.end(); iterator_p++)
		{
			if ((*iterator_p)->flag_p == false) (*iterator_p)->queueProcess();
		}

		for (iterator_p = container_p.begin();iterator_p != container_p.end(); iterator_p++)
		{
			if ((*iterator_p)->flag_p == false) (*iterator_p)->completeProcess();
		}

		for (iterator_p = container_p.begin();iterator_p != container_p.end(); iterator_p++)
		{
			if ((*iterator_p)->flag_p == true) (*iterator_p)->queueProcess();
		}

		for (iterator_p = container_p.begin();iterator_p != container_p.end(); iterator_p++)
		{
			if ((*iterator_p)->flag_p == true) (*iterator_p)->completeProcess();
		}
	}

	return;
}

void FlagAgentList::chunkSummary()
{
	for (iterator_p = container_p.begin();iterator_p != container_p.end(); iterator_p++)
	{
		(*iterator_p)->chunkSummary();
	}

	return;
}

void FlagAgentList::tableSummary()
{
	for (iterator_p = container_p.begin();iterator_p != container_p.end(); iterator_p++)
	{
		(*iterator_p)->tableSummary();
	}

	return;
}

void FlagAgentList::setProfiling(bool enable)
{
	for (iterator_p = container_p.begin();iterator_p != container_p.end(); iterator_p++)
	{
		(*iterator_p)->setProfiling(enable);
	}

	return;
}

void FlagAgentList::setCheckMode(bool enable)
{
	for (iterator_p = container_p.begin();iterator_p != container_p.end(); iterator_p++)
	{
		(*iterator_p)->setCheckMode(enable);
	}

	return;
}

FlagReport FlagAgentList::gatherReports()
{
	FlagReport combinedReport("list");

	for (iterator_p = container_p.begin();iterator_p != container_p.end(); iterator_p++)
	{
		combinedReport.addReport( (*iterator_p)->getReport() );
	}

	return combinedReport;
}


} //# NAMESPACE CASA - END


