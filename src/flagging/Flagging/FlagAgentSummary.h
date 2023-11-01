//# FlagAgentSummary.h: This file contains the interface definition of the FlagAgentSummary class.
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

#ifndef FlagAgentSummary_H_
#define FlagAgentSummary_H_

#include <unordered_map>

#include <flagging/Flagging/FlagAgentBase.h>

namespace casa { //# NAMESPACE CASA - BEGIN

class FlagAgentSummary : public FlagAgentBase {

	struct summary
	{
		summary()
		{
			accumflags.clear();
			accumtotal.clear();
			accumChannelflags.clear();
			accumChanneltotal.clear();
			accumPolarizationflags.clear();
			accumPolarizationtotal.clear();
			accumAntScanflags.clear();
			accumAntScantotal.clear();
			accumTotalFlags = 0;
			accumTotalCount = 0;
		}

		std::unordered_map<std::string, std::unordered_map<std::string, casacore::uInt64> > accumflags;
		std::unordered_map<std::string, std::unordered_map<std::string, casacore::uInt64> > accumtotal;

		std::unordered_map<casacore::Int, std::unordered_map<casacore::uInt, casacore::uInt64> > accumChannelflags;
		std::unordered_map<casacore::Int, std::unordered_map<casacore::uInt, casacore::uInt64> > accumChanneltotal;

		std::unordered_map<casacore::Int, std::unordered_map<std::string, casacore::uInt64> > accumPolarizationflags;
		std::unordered_map<casacore::Int, std::unordered_map<std::string, casacore::uInt64> > accumPolarizationtotal;

		std::unordered_map<casacore::Int, std::unordered_map<casacore::Int, casacore::uInt64> > accumAntScanflags;
		std::unordered_map<casacore::Int, std::unordered_map<casacore::Int, casacore::uInt64> > accumAntScantotal;

		casacore::uInt64 accumTotalFlags, accumTotalCount;
	};

public:

	FlagAgentSummary(FlagDataHandler *dh, casacore::Record config);
	~FlagAgentSummary();

	casacore::Record getResult();

protected:

	// Common functionality for each visBuffer (don't repeat at the row level)
	void preProcessBuffer(const vi::VisBuffer2 &visBuffer);

        // Add flags sum into summary fields
        void postProcessBuffer();

	// Compute flags for a given mapped visibility point
	bool computeRowFlags(const vi::VisBuffer2 &visBuffer, FlagMapper &flags, casacore::uInt row);

	// Parse configuration parameters
	void setAgentParameters(casacore::Record config);

	// Get the summary dictionary, and 'view' reports.
	FlagReport getReport();

	// Utility method to facilitate creation of sub-summaries per field
	void getResultCore(casacore::Record &summary);

private:

	// Build simple plot-reports from the summary dictionary
	FlagReport buildFlagCountPlots();
	std::unordered_map<casacore::Int , std::vector<casacore::Double> > frequencyList;

	casacore::Bool spwChannelCounts;
	casacore::Bool spwPolarizationCounts;
	casacore::Bool baselineCounts;
	casacore::Bool fieldCounts;
	casacore::String display_p;

	std::unordered_map<std::string, summary* > fieldSummaryMap;
	summary *currentSummary;
	casacore::Int arrayId;
	casacore::Int fieldId;
	casacore::Int spw;
	casacore::Int scan;
	casacore::Int observationId;
        // to count total flags (whether on/off) in the current buffer
        size_t bufferTotal = 0;
        // to count total flags (on) in the current buffer
        size_t bufferFlags = 0;

        string arrayId_str;
        string fieldId_str;
	string spw_str;
	string observationId_str;

};


} //# NAMESPACE CASA - END

#endif /* FlagAgentSummary_H_ */

