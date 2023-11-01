//#  CASA - Common Astronomy Software Applications (http://casa.nrao.edu/)
//#  Copyright (C) Associated Universities, Inc. Washington DC, USA 2011, All
//# rights reserved.
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

#include <mstransform/TVI/StatWtDataAggregator.h>

#include <casacore/casa/Arrays/ArrayMath.h>
#include <casacore/casa/Arrays/Cube.h>
// debug
#include <casacore/casa/IO/ArrayIO.h>

#ifdef _OPENMP
#include <omp.h>
#endif

using namespace casacore;
using namespace std;

namespace casa {

namespace vi {

StatWtDataAggregator::StatWtDataAggregator(
    ViImplementation2 *const vii,
    const map<Int, vector<StatWtTypes::ChanBin>>& chanBins,
    std::shared_ptr<map<uInt, pair<uInt, uInt>>>& samples,
    StatWtTypes::Column column, Bool noModel,
    const map<uInt, Cube<Bool>>& chanSelFlags,
    std::shared_ptr<
        casacore::ClassicalStatistics<casacore::Double,
        casacore::Array<casacore::Float>::const_iterator,
        casacore::Array<casacore::Bool>::const_iterator>
    >& wtStats,
    shared_ptr<const pair<Double, Double>> wtrange,
    Bool combineCorr,
    shared_ptr<
        StatisticsAlgorithm<
            Double, Array<Float>::const_iterator,
            Array<Bool>::const_iterator, Array<Double>::const_iterator
        >
    >& statAlg, Int minSamp
) : _vii(vii), _chanBins(chanBins), _samples(samples),
    _varianceComputer(
        new StatWtVarianceAndWeightCalculator(statAlg, samples, minSamp)
    ),
    _column(column),_noModel(noModel), _chanSelFlags(chanSelFlags),
    _wtStats(wtStats), _wtrange(wtrange), _combineCorr(combineCorr) {}


StatWtDataAggregator::~StatWtDataAggregator() {}

Bool StatWtDataAggregator::mustComputeWtSp() const {
    return *_mustComputeWtSp;
}

void StatWtDataAggregator::setMustComputeWtSp(
    std::shared_ptr<casacore::Bool> mcwp
) {
    _mustComputeWtSp = mcwp;
}

StatWtTypes::Baseline StatWtDataAggregator::_baseline(
    uInt ant1, uInt ant2
) {
    return StatWtTypes::Baseline(min(ant1, ant2), max(ant1, ant2));
}

Bool StatWtDataAggregator::_checkFirstSubChunk(
    Int& spw, Bool& firstTime, const VisBuffer2 * const vb
) const {
    if (! firstTime) {
        // this chunk has already been checked, it has not
        // been processed previously
        return False;
    }
    const auto& rowIDs = vb->rowIds();
    if (_processedRowIDs.find(rowIDs[0]) == _processedRowIDs.end()) {
        // haven't processed this chunk
        _processedRowIDs.insert(rowIDs[0]);
        // the spw is the same for all subchunks, so it only needs to
        // be set once
        spw = *vb->spectralWindows().begin();
        if (_samples->find(spw) == _samples->end()) {
            (*_samples)[spw].first = 0;
            (*_samples)[spw].second = 0;
        }
        firstTime = False;
        return False;
    }
    else {
        // this chunk has been processed, this can happen at the end
        // when the last chunk is processed twice
        return True;
    }
}

const Cube<Complex> StatWtDataAggregator::_dataCube(
    const VisBuffer2 *const vb
) const {
    switch (_column) {
    case StatWtTypes::CORRECTED:
        return vb->visCubeCorrected();
    case StatWtTypes::DATA:
        return vb->visCube();
    case StatWtTypes::RESIDUAL:
        if (_noModel) {
            return vb->visCubeCorrected();
        }
        else {
            return vb->visCubeCorrected() - vb->visCubeModel();
        }
    case StatWtTypes::RESIDUAL_DATA:
        if(_noModel) {
            return vb->visCube();
        }
        else {
            return vb->visCube() - vb->visCubeModel();
        }
    default:
        ThrowCc("Logic error: column type not handled");
    }
}

Cube<Bool> StatWtDataAggregator::_getResultantFlags(
    Cube<Bool>& chanSelFlagTemplate, Cube<Bool>& chanSelFlags,
    Bool& initTemplate, Int spw, const Cube<Bool>& flagCube
) const {
    if (_chanSelFlags.find(spw) == _chanSelFlags.cend()) {
        // no selection of channels to ignore
        return flagCube;
    }
    if (initTemplate) {
        // this can be done just once per chunk because all the rows
        // in the chunk are guaranteed to have the same spw
        // because each subchunk is guaranteed to have a single
        // data description ID.
        chanSelFlagTemplate = _chanSelFlags.find(spw)->second;
        initTemplate = False;
    }
    auto dataShape = flagCube.shape();
    chanSelFlags.resize(dataShape, False);
    auto ncorr = dataShape[0];
    auto nrows = dataShape[2];
    IPosition start(3, 0);
    IPosition end = dataShape - 1;
    Slicer sl(start, end, Slicer::endIsLast);
    for (uInt corr=0; corr<ncorr; ++corr) {
        start[0] = corr;
        end[0] = corr;
        for (Int row=0; row<nrows; ++row) {
            start[2] = row;
            end[2] = row;
            sl.setStart(start);
            sl.setEnd(end);
            chanSelFlags(sl) = chanSelFlagTemplate;
        }
    }
    return flagCube || chanSelFlags;
}

void StatWtDataAggregator::_updateWtSpFlags(
    Cube<Float>& wtsp, Cube<Bool>& flags, Bool& checkFlags,
    const Slicer& slice, Float wt
) const {
    // writable array reference
    auto flagSlice = flags(slice);
    if (*_mustComputeWtSp) {
        // writable array reference
        auto wtSlice = wtsp(slice);
        wtSlice = wt;
        // update global stats before we potentially flag data
        auto mask = ! flagSlice;
        _wtStats->addData(wtSlice.begin(), mask.begin(), wtSlice.size());
    }
    else if (! allTrue(flagSlice)) {
        // we don't need to compute WEIGHT_SPECTRUM, and the slice isn't
        // entirely flagged, so we need to update the WEIGHT column stats
        _wtStats->addData(Array<Float>(IPosition(1, 1), wt).begin(), 1);
    }
    if (
        wt == 0
        || (_wtrange && (wt < _wtrange->first || wt > _wtrange->second))
    ) {
        if (*_mustComputeWtSp) {
            wtsp(slice) = 0;
        }
        checkFlags = True;
        flagSlice = True;
    }

}

}

}
