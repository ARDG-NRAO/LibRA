//# FlagMSHandler.h: This file contains the interface definition of the FlagMSHandler class.
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

#ifndef FlagMSHandler_H_
#define FlagMSHandler_H_

#include <flagging/Flagging/FlagDataHandler.h>

namespace casa { //# NAMESPACE CASA - BEGIN

namespace vi {
  class FrequencySelectionUsingChannels;
  class VisIterImpl2LayerFactory;
}

// Flag casacore::Data Handler class definition
class FlagMSHandler: public FlagDataHandler
{

public:

	// Default constructor
	// NOTE: casacore::Time interval 0 groups all time steps together in one chunk.
	FlagMSHandler(string msname, casacore::uShort iterationApproach = SUB_INTEGRATION, casacore::Double timeInterval = 0);

	// Default destructor
	~FlagMSHandler();

	// Open Measurement Set
	bool open();

	// Close Measurement Set
	bool close();

	// Generate selected Measurement Set
	bool selectData();

	// Parse casacore::MSSelection expression
	bool parseExpression(casacore::MSSelection &parser);

	// Generate Visibility Iterator
	bool generateIterator();

	// Move to next chunk
	bool nextChunk();

	// Move to next buffer
	bool nextBuffer();

	// Write flag cube into MS
	bool flushFlags();

	// Provide table name (for flag version)
	casacore::String getTableName();

	// Check if a given column is available in the MS
	// (typically for checking existence of CORRECTED_DATA
	bool checkIfColumnExists(casacore::String column);

	// Whether the per-chunk progress log line should be produced
	bool checkDoChunkLine(double progress);

	// Signal true when a per-agent partial (ongoing run) summary has to be printed
	bool summarySignal();

	// Get the casacore::MS PROCESSOR sub-table
	bool processorTable();

	// Check if SOURCE_MODEL column exists (Virtual MODEL column)
	bool checkIfSourceModelColumnExists();

	// Get a casacore::Float visCube and return a casacore::Complex one
	casacore::Cube<casacore::Complex>& weightVisCube();

	casacore::Cube<casacore::Complex> weight_spectrum_p;

private:

	// Mapping functions
	virtual void generateScanStartStopMap();

	// Swap casacore::MS to check what is the maximum RAM memory needed
	void preSweep();

	// Apply channel selection for asyn or normal iterator
	// NOTE: We always have to do this, even if there is no SPW:channel selection
	void applyChannelSelection(vi::VisIterImpl2LayerFactory *viFactory);


	// Measurement set section
	casacore::MeasurementSet *selectedMeasurementSet_p;
	casacore::MeasurementSet *originalMeasurementSet_p;

	// RO Visibility Iterator
	vi::VisibilityIterator2 *preAveragingVI_p;
	vi::VisibilityIterator2 *visibilityIterator_p;
	vi::FrequencySelectionUsingChannels *channelSelector_p;

};

} //# NAMESPACE CASA - END

#endif /* FlagMSHandler_H_ */
