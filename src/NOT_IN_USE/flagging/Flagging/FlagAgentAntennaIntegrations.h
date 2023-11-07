//# FlagAgentAntennaIntegrations.h: This file contains the interface definition of the FlagAgentAntennaIntegrations class.
//#
//#  CASA - Common Astronomy Software Applications (http://casa.nrao.edu/)
//#  Copyright (C) Associated Universities, Inc. Washington DC, USA 2017, All rights reserved.
//#  Copyright (C) European Southern Observatory, 2017, All rights reserved.
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

#ifndef FlagAgentAntennaIntegrations_H_
#define FlagAgentAntennaIntegrations_H_

#include <flagging/Flagging/FlagAgentBase.h>
#include <unordered_map>

namespace casa { //# NAMESPACE CASA - BEGIN

/**
 * <summary>
 * A flag agent that implements the 'antint' flagdata mode.
 * </summary>
 *
 * Uses the FlagAgentBase::ROWS_PREPROCESS_BUFFER iteration approach.
 *
 * The pre-processing in preProcessBuffer() checks what channels are
 * flagged for every baseline and saves in a data structure what times
 * should be flagged.
 * Then computeRowFlags() flags/returns true for all the rows with
 * times/integrations that should be flagged. Following the 'antint'
 * approach.
 *
 * This implementation uses as reference the task 'antintflag'
 * (https://doi.org/10.5281/zenodo.163546,
 * https://github.com/chrishales/antintflag)
 */
class FlagAgentAntennaIntegrations : public FlagAgentBase {

  public:

    FlagAgentAntennaIntegrations(FlagDataHandler *dh, casacore::Record config, casacore::Bool writePrivateFlagCube = false, casacore::Bool flag = true);

    ~FlagAgentAntennaIntegrations() = default;

  private:

    void setAgentParameters(casacore::Record config,
			    casacore::Vector<casacore::String> *antennaNames);

    // To use iteration approach FlagAgentBase::ROWS_PREPROCESS_BUFFER
    void preProcessBuffer(const vi::VisBuffer2 &visBuffer);

    // Compute flags afor a given mapped visibility point
    bool computeRowFlags(const vi::VisBuffer2 &visBuffer, FlagMapper &flags, casacore::uInt row);

    typedef std::unordered_map<casacore::Double, bool> FlaggedTimesMap;

    typedef std::vector<std::vector<bool>> TableFlagPerBaselinePerChannel;

    void doPreProcessingTimePoint(FlaggedTimesMap &flaggedTimes, 
				  casacore::Double rowTime,
				  TableFlagPerBaselinePerChannel &flagPerBaselinePerChannel);

    void doPreProcessingTimePointMultiChannel(FlaggedTimesMap &flaggedTimes, 
					      casacore::Double rowTime,
					      const TableFlagPerBaselinePerChannel &flagPerBaselinePerChannel);

    void doPreProcessingTimePointSingleChannel(FlaggedTimesMap &flaggedTimes, 
					       casacore::Double rowTime,
					       const TableFlagPerBaselinePerChannel &flagPerBaselinePerChannel);

    void checkAnyPolarizationFlagged(const casacore::Cube<casacore::Bool> &polChanRowFlags, 
				     TableFlagPerBaselinePerChannel &flagPerBaselinePerChannel,
				     casacore::uInt row, casacore::uInt baselineIdx);


    casacore::uInt
    findAntennaID(const casacore::String &name,
		  const casacore::Vector<casacore::String> *antennaNames);

    // Time points that should be flagged
    FlaggedTimesMap doFlagTime_p;

    // The antenna of interest. Pairs with this one are checked against the flag-threshold
    casacore::Int antIdx_p;
    // what fraction of channels need to be flagged to consider a
    // polarization product flagged
    casacore::Double minChanThreshold_p = 0.6;
    // verbosity
    bool verbose_p = false;
};


} //# NAMESPACE CASA - END

#endif /* FlagAgentAntennaIntegrations_H_ */
