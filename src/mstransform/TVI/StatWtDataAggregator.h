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

#ifndef STATWTDATAAGGREGATOR_H_
#define STATWTDATAAGGREGATOR_H_

#include <msvis/MSVis/VisBuffer2.h>
#include <casacore/scimath/StatsFramework/ClassicalStatistics.h>

#include <mstransform/TVI/StatWtTypes.h>
#include <mstransform/TVI/StatWtVarianceAndWeightCalculator.h>
#include <casacore/tables/Tables/RowNumbers.h>

#include <map>

namespace casa {

namespace vi {

// Pure virtual base class of various statwt data aggregators. Only StatWt
// needs to use this; developers should not use this code directly.

class StatWtDataAggregator {

public:
    
    StatWtDataAggregator() = delete;

    StatWtDataAggregator(
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
        // std::shared_ptr<casacore::Bool>& mustComputeWtSp,
        std::shared_ptr<
            casacore::ClassicalStatistics<casacore::Double,
            casacore::Array<casacore::Float>::const_iterator,
            casacore::Array<casacore::Bool>::const_iterator>
        >& wtStats,
        std::shared_ptr<
            const std::pair<casacore::Double, casacore::Double>
        > wtrange,
        casacore::Bool combineCorr,
        std::shared_ptr<
            casacore::StatisticsAlgorithm<
                casacore::Double,
                casacore::Array<casacore::Float>::const_iterator,
                casacore::Array<casacore::Bool>::const_iterator,
                casacore::Array<casacore::Double>::const_iterator
            >
        >& statAlg, casacore::Int minSamp
    );

    virtual ~StatWtDataAggregator();

    // aggregates the data and computes the weights
    virtual void aggregate() = 0;
    
    casacore::Bool mustComputeWtSp() const;

    void setMustComputeWtSp(std::shared_ptr<casacore::Bool> mcwp);

    // gets data TVI needs
    virtual void weightSpectrumFlags(
        casacore::Cube<casacore::Float>& wtsp,
        casacore::Cube<casacore::Bool>& flagCube, casacore::Bool& checkFlags,
        const casacore::Vector<casacore::Int>& ant1,
        const casacore::Vector<casacore::Int>& ant2,
        const casacore::Vector<casacore::Int>& spws,
        const casacore::Vector<casacore::Double>& exposures,
        const casacore::Vector<casacore::rownr_t>& rowIDs
    ) const = 0;

    virtual void weightSingleChanBin(
        casacore::Matrix<casacore::Float>& wtmat, casacore::Int nrows
    ) const = 0;

protected:

    ViImplementation2 *const _vii;
    const std::map<casacore::Int, std::vector<StatWtTypes::ChanBin>> _chanBins;
    mutable std::set<casacore::uInt> _processedRowIDs {};
    mutable std::shared_ptr<
        std::map<casacore::uInt, std::pair<casacore::uInt, casacore::uInt>>
    > _samples {};
    std::unique_ptr<StatWtVarianceAndWeightCalculator> _varianceComputer;
    const StatWtTypes::Column _column;
    // TODO you can probably get rid of this in StatWtTVI
    const casacore::Bool _noModel;
    // The _chanSelFlags key is the spw. The value is a Cube for convenience
    // for subchunk computations that require the same shaped cube of flags to
    // be applied. The dimension that counts is the second (zero-based 1) as it
    // has length equal to the number of channels in the spw. A value of True
    // indicates that the channel is "flagged", ie should not be used.
    const std::map<casacore::uInt, casacore::Cube<casacore::Bool>>
        _chanSelFlags;
    std::shared_ptr<casacore::Bool> _mustComputeWtSp {};
    std::shared_ptr<
        casacore::ClassicalStatistics<casacore::Double,
        casacore::Array<casacore::Float>::const_iterator,
        casacore::Array<casacore::Bool>::const_iterator>
    > _wtStats;
    // TODO you can probably get rid of this in StatWtTVI
    std::shared_ptr<const std::pair<casacore::Double, casacore::Double>>
        _wtrange;
    const casacore::Bool _combineCorr;

    // swaps ant1/ant2 if necessary
    static StatWtTypes::Baseline _baseline(
        casacore::uInt ant1, casacore::uInt ant2
    );

    // returns True if this chunk has already been processed. This can happen
    // for the last chunk.
    casacore::Bool _checkFirstSubChunk(
        casacore::Int& spw, casacore::Bool& firstTime,
        const vi::VisBuffer2 * const vb
    ) const;

    const casacore::Cube<casacore::Complex> _dataCube(
        const VisBuffer2 *const vb
    ) const;

    // combines the flag cube with the channel selection flags (if any)
    casacore::Cube<casacore::Bool> _getResultantFlags(
        casacore::Cube<casacore::Bool>& chanSelFlagTemplate,
        casacore::Cube<casacore::Bool>& chanSelFlags,
        casacore::Bool& initChanSelFlags, casacore::Int spw,
        const casacore::Cube<casacore::Bool>& flagCube
    ) const;

    void _updateWtSpFlags(
        casacore::Cube<casacore::Float>& wtsp,
        casacore::Cube<casacore::Bool>& flags, casacore::Bool& checkFlags,
        const casacore::Slicer& slice, casacore::Float wt
    ) const;

};

}

}

#endif 

