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

#ifndef STATWTFLOATINGWINDOWDATAAGGREGATOR_H_
#define STATWTFLOATINGWINDOWDATAAGGREGATOR_H_

#include <mstransform/TVI/StatWtDataAggregator.h>

#include <casacore/casa/Arrays/Cube.h>

#include <map>

namespace casa {

namespace vi {

// Used by StatWt. Developers should not directly use this API.
// Aggregates data allowing a sliding window inside the chunk, which allows
// aggregating data that span subchunks. This algorithm is used when timebin is
// an int and or when slidetimewindow is true.

class StatWtFloatingWindowDataAggregator: public StatWtDataAggregator {

public:
    
    StatWtFloatingWindowDataAggregator() = delete;

    // out of necessity, the passed in pointer-like variables are shared with
    // the caller.
    StatWtFloatingWindowDataAggregator(
        ViImplementation2 *const vii,
        const std::map<
            casacore::Int, std::vector<StatWtTypes::ChanBin>
        >& chanBins,
        std::shared_ptr<
            std::map<casacore::uInt, std::pair<casacore::uInt, casacore::uInt>>
        >& samples,
        StatWtTypes::Column column, casacore::Bool noModel,
        const std::map<casacore::uInt, casacore::Cube<casacore::Bool>>&
            chanSelFlags,
        casacore::Bool combineCorr,
        std::shared_ptr<
            casacore::ClassicalStatistics<casacore::Double,
            casacore::Array<casacore::Float>::const_iterator,
            casacore::Array<casacore::Bool>::const_iterator>
        >& wtStats,
        std::shared_ptr<
            const std::pair<casacore::Double, casacore::Double>
        > wtrange,
        std::shared_ptr<const casacore::Double> binWidthInSeconds,
        std::shared_ptr<const casacore::Int> nTimeStampsInBin,
        const casacore::Bool timeBlockProcessing,
        std::shared_ptr<
            casacore::StatisticsAlgorithm<
                casacore::Double,
                casacore::Array<casacore::Float>::const_iterator,
                casacore::Array<casacore::Bool>::const_iterator,
                casacore::Array<casacore::Double>::const_iterator
            >
        >& statAlg, casacore::Int minSamp
    );

    ~StatWtFloatingWindowDataAggregator();

    // aggregates the data and computes the weights
    void aggregate();

    void weightSingleChanBin(
        casacore::Matrix<casacore::Float>& wtmat, casacore::Int nrows
    ) const;

    void weightSpectrumFlags(
        casacore::Cube<casacore::Float>& wtsp,
        casacore::Cube<casacore::Bool>& flagCube, casacore::Bool& checkFlags,
        const casacore::Vector<casacore::Int>& ant1,
        const casacore::Vector<casacore::Int>& ant2,
        const casacore::Vector<casacore::Int>& spws,
        const casacore::Vector<casacore::Double>& exposures,
        const casacore::Vector<casacore::rownr_t>& rowIDs
     ) const;

private:

    std::shared_ptr<const casacore::Double> _binWidthInSeconds {};

    mutable casacore::Cube<casacore::Double> _weights {};

    std::shared_ptr<const casacore::Int> _nTimeStampsInBin {};

    // TODO can probably get rid of this in StatWtTVI
    // for running time window, for each subchunk, map the rowID (in the MS)
    // to the row index in the chunk
    mutable std::map<casacore::uInt, casacore::uInt>
        _rowIDInMSToRowIndexInChunk {};

    const casacore::Bool _timeBlockProcessing;

    // TODO can probably get rid of this in StatWtTVI
    void _computeWeights(
        const casacore::Cube<casacore::Complex>& data,
        const casacore::Cube<casacore::Bool>& flags,
        const casacore::Vector<casacore::Double>& exposures,
        const std::vector<std::set<casacore::uInt>>& rowMap, casacore::uInt spw
    ) const;

    // idToChunksNeededByIDMap maps subchunkIDs to the range of subchunk IDs
    // they need. chunkNeededToIDsThatNeedChunkIDMap maps subchunk IDs that are
    // needed to the subchunkIDs that need them. min/max IDs (.first/.second)
    // in both cases
    void _limits(
        std::vector<
            std::pair<casacore::uInt, casacore::uInt>
        >& idToChunksNeededByIDMap,
        std::vector<
            std::pair<casacore::uInt, casacore::uInt>
        >& chunkNeededToIDsThatNeedChunkIDMap
    ) const;

};

}

}

#endif 
