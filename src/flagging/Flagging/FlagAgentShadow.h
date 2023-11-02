//# FlagAgentShadow.h: This file contains the interface definition of the FlagAgentShadow class.
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

#ifndef FlagAgentShadow_H_
#define FlagAgentShadow_H_

#include <flagging/Flagging/FlagAgentBase.h>

namespace casa { //# NAMESPACE CASA - BEGIN

class FlagAgentShadow : public FlagAgentBase {

public:

	FlagAgentShadow(FlagDataHandler *dh, casacore::Record config, casacore::Bool writePrivateFlagCube = false, casacore::Bool flag = true);
	~FlagAgentShadow();

protected:

	// Common functionality for each visBuffer (don't repeat at the row level)
	void preProcessBuffer(const vi::VisBuffer2 &visBuffer);

	// Common preProcessing code for the single/multiple agent cases
	void preProcessBufferCore(const vi::VisBuffer2 &visBuffer);

	// Compute flags afor a given mapped visibility point
	bool computeRowFlags(const vi::VisBuffer2 &visBuffer, FlagMapper &flags, casacore::uInt row);

	// Parse configuration parameters
	void setAgentParameters(casacore::Record config);

private:

        // casacore::Function to compute antenna UVW values for the current timestep
        casacore::Bool computeAntUVW(const vi::VisBuffer2 &vb, casacore::Int rownr);
        // casacore::Function to compute shadowed antennas, given a list of antenna UVWs.
        void calculateShadowedAntennas(const vi::VisBuffer2 &visBuffer, casacore::Int rownr);
        // casacore::Function to decide if the 'behind' antenna is shadowed or not, for one baseline
        void decideBaselineShadow(casacore::Double uvDistance, casacore::Double w, casacore::Int antenna1, casacore::Int antenna2);
        // casacore::Function to return baseline index.
        casacore::uInt baselineIndex(casacore::uInt nAnt, casacore::uInt a1, casacore::uInt a2);
 
	/// casacore::Input parameters ///
	casacore::Double shadowTolerance_p;
        casacore::Record additionalAntennas_p;

        // Copies of antenna-information lists, containing extra antennas if specified.
        ///casacore::Vector<casacore::String> shadowAntennaNames_p;
        casacore::Vector<casacore::Double> shadowAntennaDiameters_p;
        casacore::Vector<casacore::MPosition> shadowAntennaPositions_p;

	// Declaration of static members for common pre-processing
	casacore::uShort agentNumber_p;
	static std::vector<casacore::Int> shadowedAntennas_p;
	static casa::async::Mutex staticMembersMutex_p;
	static std::vector<bool> startedProcessing_p;
	static bool preProcessingDone_p;
	static casacore::uShort nAgents_p;
        
        // Private variables that change with each timestep
        casacore::Matrix<casacore::Double> uvwAnt_p;
        casacore::Double currTime_p;

        casacore::Bool firststep_p; // helper variable to control a debug print statement
    
};

} //# NAMESPACE CASA - END

#endif /* FlagAgentShadow_H_ */

