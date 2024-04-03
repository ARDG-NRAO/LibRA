//# FlagCalTableHandler.h: This file contains the implementation of the FlagCalTableHandler class.
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

#include <flagging/Flagging/FlagCalTableHandler.h>

#include <synthesis/CalTables/NewCalTable.h>
#include <synthesis/CalTables/CTInterface.h>
#include <synthesis/CalTables/CTIter.h>
#include <synthesis/CalTables/CalBuffer.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

//////////////////////////////////////////
/// FlagCalTableHandler implementation ///
//////////////////////////////////////////

// -----------------------------------------------------------------------
// Default constructor
// -----------------------------------------------------------------------
FlagCalTableHandler::FlagCalTableHandler(string tablename, uShort iterationApproach, Double timeInterval):
		FlagDataHandler(tablename,iterationApproach,timeInterval)
{
	selectedCalTable_p = NULL;
	originalCalTable_p = NULL;
	calTableInterface_p = NULL;
	calBuffer_p = NULL;
	calIter_p = NULL;
	tableTye_p = CALIBRATION_TABLE;
}

// -----------------------------------------------------------------------
// Default destructor
// -----------------------------------------------------------------------
FlagCalTableHandler::~FlagCalTableHandler()
{
	logger_p->origin(LogOrigin("FlagCalTableHandler",__FUNCTION__,WHERE));
	*logger_p << LogIO::DEBUG1 << "FlagCalTableHandler::~FlagCalTableHandler()" << LogIO::POST;

	if (calBuffer_p) delete calBuffer_p;
	if (calIter_p) delete calIter_p;
	if (calTableInterface_p) delete calTableInterface_p;
	if (selectedCalTable_p) delete selectedCalTable_p;
	if (originalCalTable_p) delete originalCalTable_p;
}


// -----------------------------------------------------------------------
// Open CalTable
// -----------------------------------------------------------------------
bool
FlagCalTableHandler::open()
{
	if (originalCalTable_p) delete originalCalTable_p;
	originalCalTable_p = new NewCalTable(tablename_p,Table::Update,Table::Plain);

	// Read field names
	MSFieldColumns fieldSubTable(originalCalTable_p->field());
	fieldNames_p = new Vector<String>(fieldSubTable.name().getColumn());
	*logger_p << LogIO::DEBUG1 << "Field names are " << *fieldNames_p << LogIO::POST;

	// Read antenna names and diameters from Antenna table
	MSAntennaColumns antennaSubTable(originalCalTable_p->antenna());
	antennaNames_p = new Vector<String>(antennaSubTable.name().getColumn());
	antennaDiameters_p = new Vector<Double>(antennaSubTable.dishDiameter().getColumn());
	antennaPositions_p = new ROScalarMeasColumn<MPosition>(antennaSubTable.positionMeas());
	*logger_p << LogIO::DEBUG1 << "There are " << antennaNames_p->size() << " antennas with names: " << *antennaNames_p << LogIO::POST;

	// File the baseline to Ant1xAnt2 map
	String baseline;
	std::pair<Int,Int> ant1ant2;
	for (Int ant1Idx=0;ant1Idx < static_cast<Int>(antennaNames_p->size());ant1Idx++)
	{
		for (Int ant2Idx=ant1Idx+1;ant2Idx < static_cast<Int>(antennaNames_p->size());ant2Idx++)
		{
			ant1ant2.first = ant1Idx;
			ant1ant2.second = ant2Idx;
			baseline = antennaNames_p->operator()(ant1Idx) + "&&" + antennaNames_p->operator()(ant2Idx);
			baselineToAnt1Ant2_p[baseline] = ant1ant2;
			Ant1Ant2ToBaseline_p[ant1ant2] = baseline;
		}
	}

	// Create "dummy" correlation products list
	corrProducts_p = new std::vector<String>();
	corrProducts_p->push_back("SOL1");
	corrProducts_p->push_back("SOL2");
	corrProducts_p->push_back("SOL3");
	corrProducts_p->push_back("SOL4");

	return true;
}


// -----------------------------------------------------------------------
// Close CalTable
// -----------------------------------------------------------------------
bool
FlagCalTableHandler::close()
{
	if (selectedCalTable_p)
	{
		selectedCalTable_p->flush();
		selectedCalTable_p->relinquishAutoLocks(true);
		selectedCalTable_p->unlock();
	}

	return true;
}


// -----------------------------------------------------------------------
// Generate selected CalTable
// -----------------------------------------------------------------------
bool
FlagCalTableHandler::selectData()
{
	logger_p->origin(LogOrigin("FlagCalTableHandler",__FUNCTION__,WHERE));

	if (calTableInterface_p) delete calTableInterface_p;
	calTableInterface_p = new CTInterface(*originalCalTable_p);

	if (measurementSetSelection_p) delete measurementSetSelection_p;
	const String dummyExpr = String("");
	measurementSetSelection_p = new MSSelection();
	measurementSetSelection_p->reset(*calTableInterface_p,
									MSSelection::PARSE_LATE,
									(const String)timeSelection_p,
									(const String)baselineSelection_p,
									(const String)fieldSelection_p,
									(const String)spwSelection_p,
									(const String)uvwSelection_p,
									dummyExpr, // taqlExpr
									(const String)polarizationSelection_p,
									(const String)scanSelection_p,
									(const String)arraySelection_p,
									(const String)scanIntentSelection_p,
									(const String)observationSelection_p);


	if (selectedCalTable_p) delete selectedCalTable_p;

//	try
//	{
		TableExprNode ten = measurementSetSelection_p->toTableExprNode(calTableInterface_p);
		selectedCalTable_p = new NewCalTable();
		Bool madeSelection = getSelectedTable(*selectedCalTable_p,*originalCalTable_p,ten,String(""));

		if (madeSelection == false)
		{
			*logger_p << LogIO::NORMAL << "Selection not applicable, using entire MS" << LogIO::POST;
			delete selectedCalTable_p;
			selectedCalTable_p = new NewCalTable(*originalCalTable_p);
		}
//	}
//	catch (MSSelectionError &ex)
//	{
//		*logger_p << LogIO::WARN << "Selection not supported, using entire MS (" << ex.getMesg() << ")" << LogIO::POST;
//		delete selectedCalTable_p;
//		selectedCalTable_p = new NewCalTable(*originalCalTable_p);
//	}

	// Check if selected CalTable has rows...
	if (selectedCalTable_p->nrow() == 0)
	{
		*logger_p << LogIO::WARN << "Selected CalTable doesn't have any rows " << LogIO::POST;
	}
	else
	{
		*logger_p << LogIO::NORMAL 	<< "Original CalTable has "
									<< originalCalTable_p->nrow()
									<< " rows, and selected CalTable has "
									<< selectedCalTable_p->nrow()
									<< " rows" << LogIO::POST;
	}

	// There is a new selected MS so iterators have to be regenerated
	iteratorGenerated_p = false;
	chunksInitialized_p = false;
	buffersInitialized_p = false;
	stopIteration_p = false;

	return true;
}


// -----------------------------------------------------------------------
// Parse MSSelection expression
// -----------------------------------------------------------------------
bool
FlagCalTableHandler::parseExpression(MSSelection &parser)
{
	logger_p->origin(LogOrigin("FlagCalTableHandler",__FUNCTION__,WHERE));
	CTInterface tmpCTInterface(*originalCalTable_p);

	try
	{
		TableExprNode ten = parser.toTableExprNode(&tmpCTInterface);
	}
	catch (MSSelectionError &ex)
	{
		*logger_p << LogIO::WARN << "Selection not supported, canceling filtering (" << ex.getMesg() << ")" << LogIO::POST;
		return false;
	}

	return true;
}


// -----------------------------------------------------------------------
// Generate CalIter with a given sort order and time interval
// -----------------------------------------------------------------------
bool
FlagCalTableHandler::generateIterator()
{
	if (!iteratorGenerated_p)
	{
		// Generate CalIterator
		if (calIter_p) delete calIter_p;
		calIter_p = new CTIter(*selectedCalTable_p,getSortColumns(sortOrder_p));

		// Create CalBuffer and put VisBuffer wrapper around
		// NOTE: VisBuferAutoPtr destructor also deletes the VisBuffer inside
		if (visibilityBuffer_p) delete visibilityBuffer_p;
		calBuffer_p = new CTBuffer(calIter_p);
		visibilityBuffer_p = (vi::VisBuffer2 *)calBuffer_p;

		iteratorGenerated_p = true;
		chunksInitialized_p = false;
		buffersInitialized_p = false;
		stopIteration_p = false;
	}
	else
	{
		chunksInitialized_p = false;
		buffersInitialized_p = false;
		stopIteration_p = false;
	}

	// Do quack pre-swap
	if (mapScanStartStop_p)
	{
		calIter_p->reset();
		while (!calIter_p->pastEnd())
		{
			generateScanStartStopMap();
			calIter_p->next();
		}
	}

	return true;
}


// -----------------------------------------------------------------------
// Translate sorting columns from Block<Int> format to Block<string> format
// -----------------------------------------------------------------------
Block<String>
FlagCalTableHandler::getSortColumns(Block<Int> /*intCols*/)
{
	Block<String> strCols(4);
	strCols[0] = "OBSERVATION_ID";
	strCols[1] = "SCAN_NUMBER";
	strCols[2] = "FIELD_ID";
	strCols[3] = "SPECTRAL_WINDOW_ID";

	return strCols;
}


// -----------------------------------------------------------------------
// Move to next chunk
// -----------------------------------------------------------------------
bool
FlagCalTableHandler::nextChunk()
{
	logger_p->origin(LogOrigin("FlagCalTableHandler",__FUNCTION__,WHERE));

	chunkCounts_p = 0;
	bool moreChunks = false;
	if (stopIteration_p)
	{
		moreChunks = false;
	}
	else
	{
		if (!chunksInitialized_p)
		{
			if (!iteratorGenerated_p) generateIterator();
			calIter_p->reset();
			chunksInitialized_p = true;
			buffersInitialized_p = false;
			chunkNo_p++;
			bufferNo_p = 0;
			moreChunks = true;
		}
		else
		{
			calIter_p->next();

			if (!calIter_p->pastEnd())
			{
				buffersInitialized_p = false;
				moreChunks = true;
				chunkNo_p++;
				bufferNo_p = 0;
			}
		}
	}

	if (!moreChunks)
	{
		*logger_p << LogIO::NORMAL << "==================================================================================== " << LogIO::POST;
	}

	return moreChunks;
}


// -----------------------------------------------------------------------
// Move to next buffer
// -----------------------------------------------------------------------
bool
FlagCalTableHandler::nextBuffer()
{
	bool moreBuffers = false;
	if (stopIteration_p)
	{
		moreBuffers = false;
	}
	else
	{
		if (!buffersInitialized_p)
		{
			((CTBuffer *)visibilityBuffer_p)->invalidate();
			if (!asyncio_enabled_p) preFetchColumns();
			if (mapPolarizations_p) generatePolarizationsMap();
			if (mapAntennaPairs_p) generateAntennaPairMap();
			buffersInitialized_p = true;
			flushFlags_p = false;
			flushFlagRow_p = false;
			bufferNo_p++;

			moreBuffers = true;
		}
		else
		{
			// In CalTables there is only one iteration level
			moreBuffers = false;
		}
	}

	// Print chunk characteristics
	if (moreBuffers)
	{
		// Get flag  (WARNING: We have to modify the shape of the cube before re-assigning it)
		Cube<Bool> curentFlagCube= visibilityBuffer_p->flagCube();
		modifiedFlagCube_p.resize(curentFlagCube.shape());
		modifiedFlagCube_p = curentFlagCube;
		originalFlagCube_p.resize(curentFlagCube.shape());
		originalFlagCube_p = curentFlagCube;

		// Get flag row (WARNING: We have to modify the shape of the cube before re-assigning it)
		// NOTE: There is no FlagRow in CalTables yet, but we have it here for compatibility reasons
		modifiedFlagRow_p.resize(visibilityBuffer_p->nRows());
		originalFlagRow_p.resize(visibilityBuffer_p->nRows());

		// Compute total number of flags per buffer to be used for generating the agents stats
		Int64 currentBufferCounts = curentFlagCube.shape().product();
		chunkCounts_p += currentBufferCounts;
		progressCounts_p += currentBufferCounts;
		msCounts_p += currentBufferCounts;

		// Print chunk characteristics
		if (bufferNo_p == 1)
		{
			// jagonzal: This is correct because in CalTables there is only one iteration level
			processedRows_p += visibilityBuffer_p->nRows();

			if (printChunkSummary_p)
			{
				logger_p->origin(LogOrigin("FlagCalTableHandler",""));
				Vector<Int> scan = visibilityBuffer_p->scan();
				Vector<Int> observation = visibilityBuffer_p->observationId();
				String corrs = "[ ";
				for (uInt corr_i=0;corr_i<(uInt) visibilityBuffer_p->nCorrelations();corr_i++)
				{
					corrs += (*polarizationIndexMap_p)[corr_i] + " ";
				}
				corrs += "]";

				Double progress  = 100.0* ((Double) processedRows_p / (Double) selectedCalTable_p->nrow());

				*logger_p << LogIO::NORMAL <<
						"------------------------------------------------------------------------------------ " << LogIO::POST;
				*logger_p << LogIO::NORMAL <<
						"Chunk = " << chunkNo_p << " [progress: " << (Int)progress << "%]"
						", Observation = " << observation[0] << "~" << observation[observation.size()-1] <<
						", Scan = " << scan[0] << "~" << scan[scan.size()-1] <<
						", Field = " << visibilityBuffer_p->fieldId()(0) << " (" << fieldNames_p->operator()(visibilityBuffer_p->fieldId()) << ")"
						", Spw = " << visibilityBuffer_p->spectralWindows()(0) <<
						", Channels = " << visibilityBuffer_p->nChannels() <<
						", CalSolutions = " << corrs <<
						", Total Rows = " << visibilityBuffer_p->nRows() << LogIO::POST;
			}
		}
	}

	return moreBuffers;
}


// -----------------------------------------------------------------------
// Generate scan start stop map
// -----------------------------------------------------------------------
void
FlagCalTableHandler::generateScanStartStopMap()
{
	Int scan;
	Double start,stop;
	Vector<Int> scans;
	Vector<Double> times;

	Cube<Bool> flags;
	uInt scanStartRow;
	uInt scanStopRow;
	uInt ncorrs,nchannels,nrows;
	Bool stopSearch;

	if (scanStartStopMap_p == NULL) scanStartStopMap_p = new scanStartStopMap();

	scans = calIter_p->scan();
	times = calIter_p->time();

	// Check if anything is flagged in this buffer
	scanStartRow = 0;
	scanStopRow = times.size()-1;
	if (mapScanStartStopFlagged_p)
	{
		calIter_p->flag(flags);
		IPosition shape = flags.shape();
		ncorrs = shape[0];
		nchannels = shape[1];
		nrows = shape[2];

		// Look for effective scan start
		stopSearch = false;
		for (uInt row_i=0;row_i<nrows;row_i++)
		{
			if (stopSearch) break;

			for (uInt channel_i=0;channel_i<nchannels;channel_i++)
			{
				if (stopSearch) break;

				for (uInt corr_i=0;corr_i<ncorrs;corr_i++)
				{
					if (stopSearch) break;

					if (!flags(corr_i,channel_i,row_i))
					{
						scanStartRow = row_i;
						stopSearch = true;
					}
				}
			}
		}

		// If none of the rows were un-flagged we don't continue checking from the end
		// As a consequence of this some scans may not be present in the map, and have
		// to be skipped in the flagging process because they are already flagged.
		if (!stopSearch) return;

		// Look for effective scan stop
		stopSearch = false;
		for (uInt row_i=0;row_i<nrows;row_i++)
		{
			if (stopSearch) break;

			for (uInt channel_i=0;channel_i<nchannels;channel_i++)
			{
				if (stopSearch) break;

				for (uInt corr_i=0;corr_i<ncorrs;corr_i++)
				{
					if (stopSearch) break;

					if (!flags(corr_i,channel_i,nrows-1-row_i))
					{
						scanStopRow = nrows-1-row_i;
						stopSearch = true;
					}
				}
			}
		}
	}

	// Check scan start/stop times
	scan = scans[0];
	start = times[scanStartRow];
	stop = times[scanStopRow];

	if (scanStartStopMap_p->find(scan) == scanStartStopMap_p->end())
	{
		(*scanStartStopMap_p)[scan].push_back(start);
		(*scanStartStopMap_p)[scan].push_back(stop);
	}
	else
	{
		// Check if we have a better start time
		if ((*scanStartStopMap_p)[scan][0] > start)
		{
			(*scanStartStopMap_p)[scan][0] = start;
		}
		// Check if we have a better stop time
		if ((*scanStartStopMap_p)[scan][1] < stop)
		{
			(*scanStartStopMap_p)[scan][1] = stop;
		}
	}

	return;
}


// -----------------------------------------------------------------------
// Flush flags to CalTable
// -----------------------------------------------------------------------
bool
FlagCalTableHandler::flushFlags()
{
	if (flushFlags_p)
	{
		calIter_p->setflag(modifiedFlagCube_p);
		flushFlags_p = false;
	}

	return true;
}


// -----------------------------------------------------------------------
// Flush flags to CalTable
// -----------------------------------------------------------------------
String
FlagCalTableHandler::getTableName()
{
	return originalCalTable_p->tableName();
}

// -----------------------------------------------------------------------
// Signal true when a progress summary has to be printed
// -----------------------------------------------------------------------
bool
FlagCalTableHandler::summarySignal()
{
	Double progress = 100.0* ((Double) processedRows_p / (Double) selectedCalTable_p->nrow());
	if ((progress >= summaryThreshold_p) or (logger_p->priority() >= LogMessage::DEBUG1))
	{
		summaryThreshold_p += 10;
		printChunkSummary_p = true;
		return true;
	}
	else
	{
		printChunkSummary_p = false;
		return false;
	}
}


// -----------------------------------------------------------------------
// Check what data columns exist
// -----------------------------------------------------------------------
bool
FlagCalTableHandler::checkIfColumnExists(String column)
{
	return originalCalTable_p->tableDesc().isColumn(column);
}

//////////////////////////////////////////
//////// CTBuffer implementation ////////
//////////////////////////////////////////

CTCache::CTCache(CTIter *calIter): calIter_p(calIter)
{
	invalidate();
}

CTCache::~CTCache()
{

}

Int CTCache::arrayId()
{
	return -1;
}

Int CTCache::fieldId()
{
	if (!CTfieldIdOK_p)
	{
		field0_p = calIter_p->field()[0];
		CTfieldIdOK_p = false;
	}

	return field0_p;
}

Int CTCache::spectralWindow()
{
	if (!CTspectralWindowOK_p)
	{
		Vector<Int> tmp = calIter_p->spw();
		spw_p.resize(tmp.size(),false);
		spw_p = tmp;
		spw0_p = spw_p[0];
		CTspectralWindowOK_p = true;
	}

	return spw0_p;
}

Vector<Int>& CTCache::scan()
{
	if (!CTscanOK_p)
	{
		Vector<Int> tmp = calIter_p->scan();
		scan_p.resize(tmp.size(),false);
		scan_p = tmp;
		CTscanOK_p = true;
	}

	return scan_p;
}

Vector<Double>& CTCache::time()
{
	if (!CTtimeOK_p)
	{
		Vector<Double> tmp = calIter_p->time();
		time_p.resize(tmp.size(),false);
		time_p = tmp;
		CTtimeOK_p = true;
	}

	return time_p;
}

Vector<Int>& CTCache::antenna1()
{
	if (!CTantenna1OK_p)
	{
		Vector<Int> tmp = calIter_p->antenna1();
		antenna1_p.resize(tmp.size(),false);
		antenna1_p = tmp;
		CTantenna1OK_p = true;
	}

	return antenna1_p;
}

Vector<Int>& CTCache::antenna2()
{
	if (!CTantenna2OK_p)
	{
		Vector<Int> tmp = calIter_p->antenna2();
		if (tmp[0] < 0) tmp = calIter_p->antenna1();
		antenna2_p.resize(tmp.size(),false);
		antenna2_p = tmp;
		CTantenna2OK_p = true;
	}

	return antenna2_p;
}

Cube<Bool>& CTCache::flagCube()
{
	if (!CTflagCubeOk_p)
	{
		Cube<Bool> tmp = calIter_p->flag();
		flagCube_p.resize(tmp.shape(),false);
		flagCube_p = tmp;
		CTflagCubeOk_p = true;

		// Also fill shapes
		nRow_p = flagCube_p.shape()[2];
		nRowChunk_p = flagCube_p.shape()[2];
		nChannel_p = flagCube_p.shape()[1];
		nCorr_p = flagCube_p.shape()[0];

		CTnRowOK_p = true;
		CTnRowChunkOK_p = true;
		CTnChannelOK_p = true;
		CTnCorrOK_p = true;
	}

	return flagCube_p;
}

Vector<Int>& CTCache::observationId()
{
	if (!CTobservationIdOK_p)
	{
		Vector<Int> tmp = calIter_p->obs();
		observationId_p.resize(tmp.size(),false);
		observationId_p = tmp;
		CTobservationIdOK_p = true;
	}

	return observationId_p;
}

Vector<Int>& CTCache::correlationTypes()
{
	if (!CTcorrTypeOK_p)
	{
		if (!CTnRowOK_p) nCorrelations();
		corrType_p.resize(nCorr_p,false);
		for (uInt corr_i=0;corr_i<(uInt) nCorr_p;corr_i++)
		{
			corrType_p[corr_i] =  Stokes::NumberOfTypes+corr_i;
		}
		CTcorrTypeOK_p = true;
	}

	return corrType_p;
}

Vector<Int>& CTCache::getChannelNumbers(Int /*rowInBuffer*/)
{
	if (!CTchannelOK_p)
	{
		Vector<Int> tmp = calIter_p->chan();
		channel_p.resize(tmp.size(),false);
		channel_p = tmp;
		CTchannelOK_p = true;
	}

	return channel_p;
}

Vector<Double>& CTCache::getFrequencies(Int /*rowInBuffer*/, Int /*frame*/)
{
	if (!CTfrequencyOK_p)
	{
		Vector<Double> tmp = calIter_p->freq();
		frequency_p.resize(tmp.size(),false);
		frequency_p = tmp;
		CTfrequencyOK_p = true;
	}

	return frequency_p;
}

Cube<Complex>& CTCache::visCube()
{
	if (!CTVisCubeOK_p)
	{
		Cube<Float> tmp = calIter_p->fparam();

		// Transform Cube<Float> into Cube<Complex>
		Cube<Complex> tmpTrans(tmp.shape());
		for (uInt idx1=0;idx1<tmp.shape()[0];idx1++)
		{
			for (uInt idx2=0;idx2<tmp.shape()[1];idx2++)
			{
				for (uInt idx3=0;idx3<tmp.shape()[2];idx3++)
				{
					tmpTrans(idx1,idx2,idx3) = Complex(tmp(idx1,idx2,idx3),0);
				}
			}
		}

		fparam_p.resize(tmpTrans.shape(),false);
		fparam_p = tmpTrans;
		CTVisCubeOK_p = true;
	}

	return fparam_p;
}

Cube<Complex>& CTCache::visCubeCorrected()
{
	if (!CTcorrectedVisCubeOK_p)
	{
		Cube<Complex> tmp = calIter_p->cparam();
		cparam_p.resize(tmp.shape(),false);
		cparam_p = tmp;
		CTcorrectedVisCubeOK_p = true;
	}

	return cparam_p;
}

Cube<Complex>& CTCache::visCubeModel()
{
	if (!CTmodelVisCubeOK_p)
	{
		Cube<Float> tmp = calIter_p->snr();

		// Transform Cube<Float> into Cube<Complex>
		Cube<Complex> tmpTrans(tmp.shape());
		for (uInt idx1=0;idx1<tmp.shape()[0];idx1++)
		{
			for (uInt idx2=0;idx2<tmp.shape()[1];idx2++)
			{
				for (uInt idx3=0;idx3<tmp.shape()[2];idx3++)
				{
					tmpTrans(idx1,idx2,idx3) = Complex(tmp(idx1,idx2,idx3),0);
				}
			}
		}

		snr_p.resize(tmpTrans.shape(),false);
		snr_p = tmpTrans;
		CTmodelVisCubeOK_p = true;
	}

	return snr_p;
}

Int CTCache::nRowChunk()
{
	if (!CTnRowChunkOK_p)
	{
		if (!CTflagCubeOk_p) flagCube();
		nRowChunk_p = flagCube_p.shape()[2];
		CTnRowChunkOK_p = true;
	}

	return nRowChunk_p;
}

Int CTCache::nRows()
{
	if (!CTnRowOK_p)
	{
		if (!CTflagCubeOk_p) flagCube();
		nRow_p = flagCube_p.shape()[2];
		CTnRowOK_p = true;
	}

	return nRow_p;
}

Int CTCache::nChannels()
{
	if (!CTnChannelOK_p)
	{
		if (!CTflagCubeOk_p) flagCube();
		nChannel_p = flagCube_p.shape()[1];
		CTnChannelOK_p = true;
	}

	return nChannel_p;
}

Int CTCache::nCorrelations()
{
	if (!CTnCorrOK_p)
	{
		if (!CTflagCubeOk_p) flagCube();
		nCorr_p = flagCube_p.shape()[0];
		CTnCorrOK_p = true;
	}

	return nCorr_p;
}

void CTCache::invalidate()
{
	CTfieldIdOK_p = false;
	CTspectralWindowOK_p = false;
	CTscanOK_p = false;
	CTtimeOK_p = false;
	CTantenna1OK_p = false;
	CTantenna2OK_p = false;
	CTflagCubeOk_p = false;
	CTobservationIdOK_p = false;
	CTcorrTypeOK_p = false;
	CTchannelOK_p = false;
	CTfrequencyOK_p = false;
	CTVisCubeOK_p = false;
	CTcorrectedVisCubeOK_p = false;
	CTmodelVisCubeOK_p = false;
	CTnRowChunkOK_p = false;
	CTnRowOK_p = false;
	CTnChannelOK_p = false;
	CTnCorrOK_p = false;

	return;
}


} //# NAMESPACE CASA - END

