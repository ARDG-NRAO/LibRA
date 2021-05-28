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

#include <mstransform/TVI/StatWtFloatingWindowDataAggregator.h>

#include <scimath/StatsFramework/ClassicalStatistics.h>

#include <msvis/MSVis/ViImplementation2.h>
#include <mstransform/TVI/StatWtTypes.h>

#ifdef _OPENMP
#include <omp.h>
#endif

using namespace casacore;
using namespace std;

namespace casa {

namespace vi {

StatWtFloatingWindowDataAggregator::StatWtFloatingWindowDataAggregator(
    ViImplementation2 *const vii,
    const std::map<casacore::Int, std::vector<StatWtTypes::ChanBin>>& chanBins,
    std::shared_ptr<map<uInt, pair<uInt, uInt>>>& samples,
    StatWtTypes::Column column, Bool noModel,
    const map<uInt, Cube<Bool>>& chanSelFlags, Bool combineCorr,
    shared_ptr<
        ClassicalStatistics<
            Double, Array<Float>::const_iterator,
            Array<Bool>::const_iterator
        >
    >& wtStats,
    shared_ptr<const pair<Double, Double>> wtrange,
    shared_ptr<const Double> binWidthInSeconds,
    shared_ptr<const Int> nTimeStampsInBin, const Bool timeBlockProcessing,
    shared_ptr<
        StatisticsAlgorithm<
            Double, Array<Float>::const_iterator,
            Array<Bool>::const_iterator, Array<Double>::const_iterator
        >
    >& statAlg, Int minSamp
) : StatWtDataAggregator(
       vii, chanBins, samples, column, noModel, chanSelFlags, /* mustComputeWtSp,*/
       wtStats, wtrange, combineCorr, statAlg, minSamp
    ), _binWidthInSeconds(binWidthInSeconds),
    _nTimeStampsInBin(nTimeStampsInBin),
    _timeBlockProcessing(timeBlockProcessing) {
        ThrowIf(
            ! (_binWidthInSeconds || _nTimeStampsInBin ),
            "Logic error: neither binWidthInSeconds "
            "nor nTimeStampsInBin has been specified"
        );
}

StatWtFloatingWindowDataAggregator::~StatWtFloatingWindowDataAggregator() {}

void StatWtFloatingWindowDataAggregator::aggregate() {
    auto* vb = _vii->getVisBuffer();
    // map of rowID (the index of the vector) to rowIDs that should be used to
    // compute the weight for the key rowID
    std::vector<std::set<uInt>> rowMap;
    auto firstTime = True;
    // each subchunk is guaranteed to represent exactly one time stamp
    std::vector<Double> subChunkToTimeStamp;
    // Baseline-subchunk number pair to row index in that chunk
    std::map<std::pair<StatWtTypes::Baseline, uInt>, uInt> baselineSubChunkToIndex;
    // we build up the chunk data in the chunkData and chunkFlags cubes
    Cube<Complex> chunkData;
    Cube<Bool> chunkFlags;
    std::vector<Double> exposures;
    rownr_t subchunkStartRowNum = 0;
    auto initChanSelTemplate = True;
    Cube<Bool> chanSelFlagTemplate, chanSelFlags;
    // we cannot know the spw until inside the subchunk loop
    Int spw = -1;
    _rowIDInMSToRowIndexInChunk.clear();
    Slicer sl(IPosition(3, 0), IPosition(3, 1));
    auto slStart = sl.start();
    auto slEnd = sl.end();
    std::vector<std::pair<uInt, uInt>> idToChunksNeededByIDMap,
        chunkNeededToIDsThatNeedChunkIDMap;
    _limits(idToChunksNeededByIDMap, chunkNeededToIDsThatNeedChunkIDMap);
    uInt subChunkID = 0;
    for (_vii->origin(); _vii->more(); _vii->next(), ++subChunkID) {
        if (_checkFirstSubChunk(spw, firstTime, vb)) {
            return;
        }
        if (! _mustComputeWtSp) {
            _mustComputeWtSp.reset(
                new Bool(
                    vb->existsColumn(VisBufferComponent2::WeightSpectrum)
                )
            );
        }
        _rowIDInMSToRowIndexInChunk[*vb->rowIds().begin()] = subchunkStartRowNum;
        const auto& ant1 = vb->antenna1();
        const auto& ant2 = vb->antenna2();
        // [nCorrs, nFreqs, nRows)
        const auto nrows = vb->nRows();
        // there is no guarantee a previous subchunk will be included,
        // eg if the timewidth is small enough
        // This is the first subchunk ID that should be used for averaging
        // grouping data for weight computation of the current subchunk ID.
        const auto firstChunkNeededByCurrentID
            = idToChunksNeededByIDMap[subChunkID].first;
        const auto lastChunkNeededByCurrentID
            = idToChunksNeededByIDMap[subChunkID].second;
        const auto firstChunkThatNeedsCurrentID
            = chunkNeededToIDsThatNeedChunkIDMap[subChunkID].first;
        auto subchunkTime = vb->time()[0];
        auto rowInChunk = subchunkStartRowNum;
        pair<StatWtTypes::Baseline, uInt> mypair;
        mypair.second = subChunkID;
        for (rownr_t row=0; row<nrows; ++row, ++rowInChunk) {
            // loop over rows in sub chunk, grouping baseline specific data
            // together
            const auto baseline = _baseline(ant1[row], ant2[row]);
            mypair.first = baseline;
            baselineSubChunkToIndex[mypair] = rowInChunk;
            std::set<uInt> neededRowNums;
            neededRowNums.insert(rowInChunk);
            if (subChunkID > 0) {
                auto s = min(
                    firstChunkNeededByCurrentID, firstChunkThatNeedsCurrentID
                );
                auto tpair = mypair;
                for (; s < subChunkID; ++s) {
                    const auto myend = baselineSubChunkToIndex.end();
                    tpair.second = s;
                    const auto iter = baselineSubChunkToIndex.find(tpair);
                    auto found = iter != myend;
                    if (found) {
                        const auto existingRowNum = iter->second;
                        if (
                            s >= firstChunkNeededByCurrentID
                            && s <= lastChunkNeededByCurrentID
                        ) {
                            // The subchunk data is needed for computation
                            // of the current subchunkID's weights
                            neededRowNums.insert(existingRowNum);
                        }
                        if (
                            idToChunksNeededByIDMap[s].first <= subChunkID
                            && idToChunksNeededByIDMap[s].second >= subChunkID
                        ) {
                            rowMap[existingRowNum].insert(rowInChunk);
                        }
                    }
                }
            }
            rowMap.push_back(neededRowNums);
        }
        const auto dataCube = _dataCube(vb);
        const auto resultantFlags = _getResultantFlags(
            chanSelFlagTemplate, chanSelFlags, initChanSelTemplate,
            spw, vb->flagCube()
        );
        const auto myExposures = vb->exposure().tovector();
        exposures.insert(
            exposures.end(), myExposures.begin(), myExposures.end()
        );
        const auto cubeShape = dataCube.shape();
        IPosition sliceStart(3, 0);
        auto sliceEnd = cubeShape - 1;
        // build up chunkData and chunkFlags one subchunk at a time
        if (chunkData.empty()) {
            chunkData = dataCube;
            chunkFlags = resultantFlags;
        }
        else {
            auto newShape = chunkData.shape();
            newShape[2] += nrows;
            chunkData.resize(newShape, True);
            chunkFlags.resize(newShape, True);
            slStart[2] = subchunkStartRowNum;
            sl.setStart(slStart);
            slEnd = newShape - 1;
            sl.setEnd(slEnd);
            chunkData(sl) = dataCube;
            chunkFlags(sl) = resultantFlags;
        }
        subChunkToTimeStamp.push_back(subchunkTime);
        subchunkStartRowNum += nrows;
    }
    _computeWeights(
        chunkData, chunkFlags, Vector<Double>(exposures), rowMap, spw
    );
}

void StatWtFloatingWindowDataAggregator::weightSingleChanBin(
    Matrix<Float>& wtmat, Int nrows
) const {
    Vector<rownr_t> rowIDs;
    _vii->getRowIds(rowIDs);
    const auto start = _rowIDInMSToRowIndexInChunk.find(*rowIDs.begin());
    ThrowIf(
        start == _rowIDInMSToRowIndexInChunk.end(),
        "Logic Error: Cannot find requested subchunk in stored chunk"
    );
    // this is the row index in the chunk
    auto chunkRowIndex = start->second;
    auto ncorr = wtmat.nrow();
    for (Int i=0; i<nrows; ++i, ++chunkRowIndex) {
        if (_combineCorr) {
            wtmat.column(i) = _weights(0, 0, chunkRowIndex);
        }
        else {
            for (uInt corr=0; corr<ncorr; ++corr) {
                wtmat(corr, i) = _weights(
                    corr, 0, chunkRowIndex
                );
            }
        }
    }
}

void StatWtFloatingWindowDataAggregator::_computeWeights(
    const Cube<Complex>& data, const Cube<Bool>& flags,
    const Vector<Double>& exposures, const std::vector<std::set<uInt>>& rowMap,
    uInt spw
) const {
    auto chunkShape = data.shape();
    const auto nActCorr = chunkShape[0];
    const auto ncorr = _combineCorr ? 1 : nActCorr;
    const auto& chanBins = _chanBins.find(spw)->second;
    _weights.resize(IPosition(3, ncorr, chanBins.size(), chunkShape[2]), False);
    const auto nRows = rowMap.size();
#ifdef _OPENMP
#pragma omp parallel for
    // cout << "DEBUG PARALLEL LOOPING IS OFF" << endl;
#endif

    for (size_t iRow=0; iRow<nRows; ++iRow) {
        IPosition chunkSliceStart(3, 0);
        auto chunkSliceLength = chunkShape;
        chunkSliceLength[2] = 1;
        Slicer chunkSlice(
            chunkSliceStart, chunkSliceLength, Slicer::endIsLength
        );
        auto chunkSliceEnd = chunkSlice.end();
        auto appendingSlice = chunkSlice;
        auto appendingSliceStart = appendingSlice.start();
        auto appendingSliceEnd = appendingSlice.end();
        auto intraChunkSlice = appendingSlice;
        auto intraChunkSliceStart = intraChunkSlice.start();
        auto intraChunkSliceEnd = intraChunkSlice.end();
        intraChunkSliceEnd[0] = nActCorr - 1;
        const auto& rowsToInclude = rowMap[iRow];
        auto dataShape = chunkShape;
        dataShape[2] = rowsToInclude.size();
        Cube<Complex> dataArray(dataShape);
        Cube<Bool> flagArray(dataShape);
        auto siter = rowsToInclude.begin();
        auto send = rowsToInclude.end();
        Vector<Double> exposureVector(rowsToInclude.size(), 0);
        uInt n = 0;
        // create an array with only the rows that should
        // be used in the computation of weights for the
        // current row
        for (; siter!=send; ++siter, ++n) {
            exposureVector[n] = exposures[*siter];
            appendingSliceStart[2] = n;
            appendingSlice.setStart(appendingSliceStart);
            appendingSliceEnd[2] = n;
            appendingSlice.setEnd(appendingSliceEnd);
            chunkSliceStart[2] = *siter;
            chunkSlice.setStart(chunkSliceStart);
            chunkSliceEnd[2] = *siter;
            chunkSlice.setEnd(chunkSliceEnd);
            dataArray(appendingSlice) = data(chunkSlice);
            flagArray(appendingSlice) = flags(chunkSlice);
        }
        // slice up for correlations and channel binning
        intraChunkSliceEnd[2] = dataShape[2] - 1;
        for (uInt corr=0; corr<ncorr; ++corr) {
            if (! _combineCorr) {
                intraChunkSliceStart[0] = corr;
                intraChunkSliceEnd[0] = corr;
            }
            auto citer = chanBins.begin();
            auto cend = chanBins.end();
            auto iChanBin = 0;
            for (; citer!=cend; ++citer, ++iChanBin) {
                intraChunkSliceStart[1] = citer->start;
                intraChunkSliceEnd[1] = citer->end;
                intraChunkSlice.setStart(intraChunkSliceStart);
                intraChunkSlice.setEnd(intraChunkSliceEnd);
                _weights(corr, iChanBin, iRow)
                    = _varianceComputer->computeWeight(
                        dataArray(intraChunkSlice), flagArray(intraChunkSlice),
                        exposureVector, spw, exposures[iRow]
                    );
            }
        }
    }
}

void StatWtFloatingWindowDataAggregator::weightSpectrumFlags(
    Cube<Float>& wtsp, Cube<Bool>& flagCube, Bool& checkFlags,
    const Vector<Int>& ant1, const Vector<Int>&,
    const Vector<Int>& spws, const Vector<Double>&,
    const Vector<rownr_t>& rowIDs
) const {
    // fish out the rows relevant to this subchunk
    const auto start = _rowIDInMSToRowIndexInChunk.find(*rowIDs.begin());
    ThrowIf(
        start == _rowIDInMSToRowIndexInChunk.end(),
        "Logic Error: Cannot find requested subchunk in stored chunk"
    );
    // this is the row index in the chunk
    auto chunkRowIndex = start->second;
    auto chunkRowEnd = chunkRowIndex + ant1.size();
    Slicer slice(IPosition(3, 0), flagCube.shape(), Slicer::endIsLength);
    auto sliceStart = slice.start();
    auto sliceEnd = slice.end();
    auto nCorrBins = _combineCorr ? 1 : flagCube.shape()[0];
    auto spw = *spws.begin();
    const auto& chanBins = _chanBins.find(spw)->second;
    auto subChunkRowIndex = 0;
    for (; chunkRowIndex < chunkRowEnd; ++chunkRowIndex, ++subChunkRowIndex) {
        sliceStart[2] = subChunkRowIndex;
        sliceEnd[2] = subChunkRowIndex;
        auto iChanBin = 0;
        for (const auto& chanBin: chanBins) {
            sliceStart[1] = chanBin.start;
            sliceEnd[1] = chanBin.end;
            auto corr = 0;
            for (; corr < nCorrBins; ++corr) {
                if (! _combineCorr) {
                    sliceStart[0] = corr;
                    sliceEnd[0] = corr;
                }
                slice.setStart(sliceStart);
                slice.setEnd(sliceEnd);
                _updateWtSpFlags(
                    wtsp, flagCube, checkFlags, slice,
                    _weights(corr, iChanBin, chunkRowIndex)
                );
            }
            ++iChanBin;
        }
    }
}

void StatWtFloatingWindowDataAggregator::_limits(
    std::vector<std::pair<uInt, uInt>>& idToChunksNeededByIDMap,
    std::vector<std::pair<uInt, uInt>>& chunkNeededToIDsThatNeedChunkIDMap
) const {
    pair<uInt, uInt> p, q;
    const uInt nTimes = _vii->nTimes();
    if (_nTimeStampsInBin) {
        // fixed number of time stamps specified
        if (_timeBlockProcessing) {
            // integer division
            uInt nBlocks = nTimes/(*_nTimeStampsInBin);
            if (nTimes % *_nTimeStampsInBin > 0) {
                ++nBlocks;
            }
            uInt subChunkCount = 0;
            for (uInt blockCount = 0; blockCount < nBlocks; ++blockCount) {
                if ((subChunkCount + *_nTimeStampsInBin <= nTimes)) {
                    p.first = subChunkCount;
                    p.second = subChunkCount + *_nTimeStampsInBin - 1;
                }
                else {
                    // chunk upper edge
                    p.first = nTimes < (uInt)*_nTimeStampsInBin
                        ? 0 : nTimes - *_nTimeStampsInBin;
                    p.second = nTimes - 1;
                }
                q = p;
                for (uInt i=subChunkCount; i<=p.second; ++i, ++subChunkCount) {
                    idToChunksNeededByIDMap.push_back(p);
                    chunkNeededToIDsThatNeedChunkIDMap.push_back(q);
                }
            }
        }
        else {
            // sliding time window, fixed number of time stamps (timebin
            // specified as int
            const auto isEven = *_nTimeStampsInBin % 2 == 0;
            // integer division
            const uInt halfTimeBin = *_nTimeStampsInBin/2;
            const auto nBefore = isEven
                ? (halfTimeBin - 1) : (*_nTimeStampsInBin - 1)/2;
            const auto nAfter = isEven ? halfTimeBin : nBefore;
            // integer division
            // p.first is the first sub chunk needed by the current index.
            // p.second is the first sub chunk that needs the current index
            for (uInt i=0; i<nTimes; ++i) {
                Int begin = i - nBefore;
                if (begin < 0) {
                    begin = 0;
                }
                p.second = begin + *_nTimeStampsInBin - 1;
                if (p.second >= nTimes) {
                    p.second = nTimes - 1;
                    begin = p.second - *_nTimeStampsInBin + 1;
                    if (begin < 0) {
                        begin = 0;
                    }
                }
                ThrowIf(begin < 0, "Logic Error: begin < 0");
                p.first = begin;
                if ((Int)nTimes <= *_nTimeStampsInBin) {
                    q = p;
                }
                else {
                    if ((Int)i < *_nTimeStampsInBin) {
                        q.first = 0;
                    }
                    else if (i > nTimes - *_nTimeStampsInBin) {
                        q.second = nTimes - 1;
                    }
                    else if (isEven) {
                        begin = i - nAfter;
                        q.first = begin;
                        q.second = i + nBefore;
                    }
                    else {
                        // isOdd
                        q = p;
                    }
                }
                idToChunksNeededByIDMap.push_back(p);
                chunkNeededToIDsThatNeedChunkIDMap.push_back(q);
            }
        }
    }
    else {
        if (_timeBlockProcessing) {
            // shouldn't get in here
            ThrowCc("Logic error: shouldn't have gotten into this code block");
        }
        else {
            ThrowIf(
                ! _binWidthInSeconds,
                "Logic error: _binWidthInSeconds not defined"
            );
            const auto halfBinWidth = *_binWidthInSeconds/2;
            auto vb = _vii->getVisBuffer();
            vector<Double> times;
            for (_vii->origin(); _vii->more(); _vii->next()) {
                times.push_back(vb->time()[0]);
            }
            for (uInt i=0; i<nTimes; ++i) {
                auto mytime = times[i];
                auto loit = std::lower_bound(
                    times.begin(), times.end(), mytime - halfBinWidth
                );
                ThrowIf(
                    loit == times.end(),
                    "Logic Error for std::lower_bound()"
                );
                p.first = std::distance(times.begin(), loit);
                auto upit = std::upper_bound(
                    times.begin(), times.end(), mytime + halfBinWidth
                );
                p.second = upit == times.end()
                    ? nTimes - 1 : std::distance(times.begin(), upit) - 1;
                q = p;
                idToChunksNeededByIDMap.push_back(p);
                chunkNeededToIDsThatNeedChunkIDMap.push_back(q);
            }
        }
    }
}

}

}
