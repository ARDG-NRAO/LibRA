//# StatWtTVI.cc: This file contains the implementation of the StatWtTVI class.
//#
//#  CASA - Common Astronomy Software Applications (http://casa.nrao.edu/)
//#  Copyright (C) Associated Universities, Inc. Washington DC, USA 2011, All
//#  rights reserved.
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

#include <mstransform/TVI/StatWtTVI.h>

#include <casacore/casa/Arrays/ArrayLogical.h>
#include <casacore/casa/Quanta/QuantumHolder.h>
#include <casacore/ms/MSOper/MSMetaData.h>
#include <casacore/tables/Tables/ArrColDesc.h>

#include <mstransform/TVI/StatWtClassicalDataAggregator.h>
#include <mstransform/TVI/StatWtFloatingWindowDataAggregator.h>
#include <mstransform/TVI/StatWtVarianceAndWeightCalculator.h>

#ifdef _OPENMP
#include <omp.h>
#endif

#include <iomanip>

using namespace casacore;
using namespace casac;

namespace casa { 
namespace vi { 

const String StatWtTVI::CHANBIN = "stchanbin";

StatWtTVI::StatWtTVI(ViImplementation2 * inputVii, const Record &configuration)
    : TransformingVi2 (inputVii) {
	// Parse and check configuration parameters
	// Note: if a constructor finishes by throwing an exception, the memory
	// associated with the object itself is cleaned up there is no memory leak.
    ThrowIf(
        ! _parseConfiguration(configuration),
        "Error parsing StatWtTVI configuration"
    );
    LogIO log(LogOrigin("StatWtTVI", __func__));
    log << LogIO::NORMAL << "Using " << StatWtTypes::asString(_column)
        << " to compute weights" << LogIO::POST;
    // FIXME when the TVI framework has methods to
    // check for metadata, like the existence of
    // columns, remove references to the original MS
    const auto& origMS = ms();
    // FIXME uses original MS explicitly
    ThrowIf(
        (_column == StatWtTypes::CORRECTED || _column == StatWtTypes::RESIDUAL)
        && ! origMS.isColumn(MSMainEnums::CORRECTED_DATA),
        "StatWtTVI requires the MS to have a CORRECTED_DATA column. This MS "
        "does not"
    );
    // FIXME uses original MS explicitly
    ThrowIf(
        (_column == StatWtTypes::DATA || _column == StatWtTypes::RESIDUAL_DATA)
        && ! origMS.isColumn(MSMainEnums::DATA),
        "StatWtTVI requires the MS to have a DATA column. This MS does not"
    );
    _mustComputeSigma = (
        _column == StatWtTypes::DATA || _column == StatWtTypes::RESIDUAL_DATA
    );
    // FIXME uses original MS explicitly
    _updateWeight = ! _mustComputeSigma 
        || (_mustComputeSigma && ! origMS.isColumn(MSMainEnums::CORRECTED_DATA));
    _noModel = (
            _column == StatWtTypes::RESIDUAL || _column == StatWtTypes::RESIDUAL_DATA
        ) && ! origMS.isColumn(MSMainEnums::MODEL_DATA)
        && ! origMS.source().isColumn(MSSourceEnums::SOURCE_MODEL);
	// Initialize attached VisBuffer
	setVisBuffer(createAttachedVisBuffer(VbRekeyable));
}

StatWtTVI::~StatWtTVI() {}

Bool StatWtTVI::_parseConfiguration(const Record& config) {
     String field = CHANBIN;
    if (config.isDefined(field)) {
        // channel binning
        auto fieldNum = config.fieldNumber(field);
        switch (config.type(fieldNum)) {
        case DataType::TpArrayBool:
            // because this is the actual default variant type, no matter
            // what is specified in the xml
            ThrowIf(
                ! config.asArrayBool(field).empty(),
                "Unsupported data type for " + field
            );
            _setDefaultChanBinMap();
            break;
        case DataType::TpInt:
            Int binWidth;
            config.get(CHANBIN, binWidth);
            _setChanBinMap(binWidth);
            break;
        case DataType::TpString:
        {
            auto chanbin = config.asString(field);
            if (chanbin == "spw") {
                // bin using entire spws
                _setDefaultChanBinMap();
                break;
            }
            else {
                QuantumHolder qh(casaQuantity(chanbin));
                _setChanBinMap(qh.asQuantity());
            }
            break;
        }
        default:
            ThrowCc("Unsupported data type for " + field);
        }
    }
    else {
        _setDefaultChanBinMap();
    }
    field = "minsamp";
    if (config.isDefined(field)) {
        config.get(field, _minSamp);
        ThrowIf(_minSamp < 2, "Minimum size of sample must be >= 2.");
    }
    field = "combine";
    if (config.isDefined(field)) {
        ThrowIf(
            config.type(config.fieldNumber(field)) != TpString,
            "Unsupported data type for combine"
        );
        _combineCorr = config.asString(field).contains("corr");
    }
    field = "wtrange";
    if (config.isDefined(field)) {
        ThrowIf(
            config.type(config.fieldNumber(field)) != TpArrayDouble,
            "Unsupported type for field '" + field + "'"
        );
        auto myrange = config.asArrayDouble(field);
        if (! myrange.empty()) {
            ThrowIf(
                myrange.size() != 2,
                "Array specified in '" + field
                + "' must have exactly two values"
            );
            ThrowIf(
                casacore::anyLT(myrange, 0.0),
                "Both values specified in '" + field
                + "' array must be non-negative"
            );
            std::set<Double> rangeset(myrange.begin(), myrange.end());
            ThrowIf(
                rangeset.size() == 1, "Values specified in '" + field
                + "' array must be unique"
            );
            auto iter = rangeset.begin();
            _wtrange.reset(new std::pair<Double, Double>(*iter, *(++iter)));
        }
    }
    auto excludeChans = False;
    field = "excludechans";
    if (config.isDefined(field)) {
        ThrowIf(
            config.type(config.fieldNumber(field)) != TpBool,
            "Unsupported type for field '" + field + "'"
        );
        excludeChans = config.asBool(field);
    }
    field = "fitspw";
    if (config.isDefined(field)) {
        ThrowIf(
            config.type(config.fieldNumber(field)) != TpString,
            "Unsupported type for field '" + field + "'"
        );
        auto val = config.asString(field);
        if (! val.empty()) {
            // FIXME references underlying MS
            const auto& myms = ms();
            MSSelection sel(myms);
            sel.setSpwExpr(val);
            auto chans = sel.getChanList();
            auto nrows = chans.nrow();
            MSMetaData md(&myms, 50);
            auto nchans = md.nChans();
            IPosition start(3, 0);
            IPosition stop(3, 0);
            IPosition step(3, 1);
            for (size_t i=0; i<nrows; ++i) {
                auto row = chans.row(i);
                const auto& spw = row[0];
                if (_chanSelFlags.find(spw) == _chanSelFlags.end()) {
                    _chanSelFlags[spw]
                        = Cube<Bool>(1, nchans[spw], 1, ! excludeChans);
                }
                start[1] = row[1];
                ThrowIf(
                    start[1] < 0, "Invalid channel selection in spw "
                    + String::toString(spw))
                ;
                stop[1] = row[2];
                step[1] = row[3];
                Slicer slice(start, stop, step, Slicer::endIsLast);
                _chanSelFlags[spw](slice) = excludeChans;
            }
        }
    }
    field = "datacolumn";
    if (config.isDefined(field)) {
        ThrowIf(
            config.type(config.fieldNumber(field)) != TpString,
            "Unsupported type for field '" + field + "'"
        );
        auto val = config.asString(field);
        if (! val.empty()) {
            val.downcase();
            ThrowIf (
                ! (
                    val.startsWith("c") || val.startsWith("d")
                    || val.startsWith("residual") || val.startsWith("residual_")
                ),
                "Unsupported value for " + field + ": " + val
            );
            _column = val.startsWith("c") ? StatWtTypes::CORRECTED
                : val.startsWith("d") ? StatWtTypes::DATA
                : val.startsWith("residual_") ? StatWtTypes::RESIDUAL_DATA
                : StatWtTypes::RESIDUAL;

        }
    }
    field = "slidetimebin";
    ThrowIf(
        ! config.isDefined(field), "Config param " + field + " must be defined"
    );
    ThrowIf(
        config.type(config.fieldNumber(field)) != TpBool,
        "Unsupported type for field '" + field + "'"
    );
    _timeBlockProcessing = ! config.asBool(field);
    field = "timebin";
    ThrowIf(
        ! config.isDefined(field), "Config param " + field + " must be defined"
    );
    auto mytype = config.type(config.fieldNumber(field));
    ThrowIf(
        ! (
            mytype == TpString || mytype == TpDouble
            || mytype == TpInt
        ),
        "Unsupported type for field '" + field + "'"
    );
    switch(mytype) {
    case TpDouble: {
        _binWidthInSeconds.reset(new Double(config.asDouble(field)));
        break;
    }
    case TpInt:
        _nTimeStampsInBin.reset(new Int(config.asInt(field)));
        ThrowIf(
            *_nTimeStampsInBin <= 0,
            "Logic Error: nTimeStamps must be positive"
        );
        break;
    case TpString: {
        QuantumHolder qh(casaQuantity(config.asString(field)));
        _binWidthInSeconds.reset(
            new Double(getTimeBinWidthInSec(qh.asQuantity()))
        );
        break;
    }
    default:
        ThrowCc("Logic Error: Unhandled type for timebin");

    }
    _doClassicVIVB = _binWidthInSeconds && _timeBlockProcessing;
    _configureStatAlg(config);
    if (_doClassicVIVB) {
        _dataAggregator.reset(
            new StatWtClassicalDataAggregator(
                getVii(), _chanBins, _samples, _column, _noModel, _chanSelFlags,
                _wtStats, _wtrange, _combineCorr, _statAlg, _minSamp
            )
        );
    }
    else {
        _dataAggregator.reset(
               new StatWtFloatingWindowDataAggregator(
                getVii(), _chanBins, _samples, _column, _noModel, _chanSelFlags,
                _combineCorr, _wtStats, _wtrange, _binWidthInSeconds,
                _nTimeStampsInBin, _timeBlockProcessing, _statAlg, _minSamp
            )
        );
    }
    _dataAggregator->setMustComputeWtSp(_mustComputeWtSp);
    return True;
}

void StatWtTVI::_configureStatAlg(const Record& config) {
    String field = "statalg";
    if (config.isDefined(field)) {
        ThrowIf(
            config.type(config.fieldNumber(field)) != TpString,
            "Unsupported type for field '" + field + "'"
        );
        auto alg = config.asString(field);
        alg.downcase();
        if (alg.startsWith("cl")) {
            _statAlg.reset(
                new ClassicalStatistics<
                    Double, Array<Float>::const_iterator,
                    Array<Bool>::const_iterator, Array<Double>::const_iterator
                >()
            );
        }
        else {
            casacore::StatisticsAlgorithmFactory<
                Double, Array<Float>::const_iterator,
                Array<Bool>::const_iterator, Array<Double>::const_iterator
            > saf;
            if (alg.startsWith("ch")) {
                Int maxiter = -1;
                field = "maxiter";
                if (config.isDefined(field)) {
                    ThrowIf(
                        config.type(config.fieldNumber(field)) != TpInt,
                        "Unsupported type for field '" + field + "'"
                    );
                    maxiter = config.asInt(field);
                }
                Double zscore = -1;
                field = "zscore";
                if (config.isDefined(field)) {
                    ThrowIf(
                        config.type(config.fieldNumber(field)) != TpDouble,
                        "Unsupported type for field '" + field + "'"
                    );
                    zscore = config.asDouble(field);
                }
                saf.configureChauvenet(zscore, maxiter);
            }
            else if (alg.startsWith("f")) {
                auto center = FitToHalfStatisticsData::CMEAN;
                field = "center";
                if (config.isDefined(field)) {
                    ThrowIf(
                        config.type(config.fieldNumber(field)) != TpString,
                        "Unsupported type for field '" + field + "'"
                    );
                    auto cs = config.asString(field);
                    cs.downcase();
                    if (cs == "mean") {
                        center = FitToHalfStatisticsData::CMEAN;
                    }
                    else if (cs == "median") {
                        center = FitToHalfStatisticsData::CMEDIAN;
                    }
                    else if (cs == "zero") {
                        center = FitToHalfStatisticsData::CVALUE;
                    }
                    else {
                        ThrowCc("Unsupported value for '" + field + "'");
                    }
                }
                field = "lside";
                auto ud = FitToHalfStatisticsData::LE_CENTER;
                if (config.isDefined(field)) {
                    ThrowIf(
                        config.type(config.fieldNumber(field)) != TpBool,
                        "Unsupported type for field '" + field + "'"
                    );
                    ud = config.asBool(field)
                        ? FitToHalfStatisticsData::LE_CENTER
                        : FitToHalfStatisticsData::GE_CENTER;
                }
                saf.configureFitToHalf(center, ud, 0);
            }
            else if (alg.startsWith("h")) {
                Double fence = -1;
                field = "fence";
                if (config.isDefined(field)) {
                    ThrowIf(
                        config.type(config.fieldNumber(field)) != TpDouble,
                        "Unsupported type for field '" + field + "'"
                    );
                    fence = config.asDouble(field);
                }
                saf.configureHingesFences(fence);
            }
            else {
                ThrowCc("Unsupported value for 'statalg'");
            }
            // clone needed for CountedPtr -> shared_ptr hand off
            _statAlg.reset(saf.createStatsAlgorithm()->clone());
        }
    }
    else {
        _statAlg.reset(
            new ClassicalStatistics<
                Double, Array<Float>::const_iterator,
                Array<Bool>::const_iterator, Array<Double>::const_iterator
            >());
    }
    std::set<StatisticsData::STATS> stats {StatisticsData::VARIANCE};
    _statAlg->setStatsToCalculate(stats);
    // also configure the _wtStats object here
    // FIXME? Does not include exposure weighting
    _wtStats.reset(
        new ClassicalStatistics<
            Double, Array<Float>::const_iterator,
            Array<Bool>::const_iterator
        >()
    );
    stats.insert(StatisticsData::MEAN);
    _wtStats->setStatsToCalculate(stats);
    _wtStats->setCalculateAsAdded(True);
}

void StatWtTVI::_logUsedChannels() const {
    // FIXME uses underlying MS
    MSMetaData msmd(&ms(), 100.0);
    const auto nchan = msmd.nChans();
    LogIO log(LogOrigin("StatWtTVI", __func__));
    log << LogIO::NORMAL << "Weights are being computed using ";
    const auto cend = _chanSelFlags.cend();
    const auto nspw = _samples->size();
    uInt spwCount = 0;
    for (const auto& kv: *_samples) {
        const auto spw = kv.first;
        log << "SPW " << spw << ", channels ";
        const auto flagCube = _chanSelFlags.find(spw);
        if (flagCube == cend) {
            log << "0~" << (nchan[spw] - 1);
        }
        else {
            vector<pair<uInt, uInt>> startEnd;
            const auto flags = flagCube->second.tovector();
            bool started = false;
            std::unique_ptr<pair<uInt, uInt>> curPair;
            for (uInt j=0; j<nchan[spw]; ++j) {
                if (started) {
                    if (flags[j]) {
                        // found a bad channel, end current range
                        startEnd.push_back(*curPair);
                        started = false;
                    }
                    else {
                        // found a "good" channel, update end of current range
                        curPair->second = j;
                    }
                }
                else if (! flags[j]) {
                    // found a good channel, start new range
                    started = true;
                    curPair.reset(new pair<uInt, uInt>(j, j));
                }
            }
            if (curPair) {
                if (started) {
                    // The last pair won't get added inside the previous loop, 
                    // so add it here
                    startEnd.push_back(*curPair);
                }
                auto nPairs = startEnd.size();
                for (uInt i=0; i<nPairs; ++i) {
                    log  << startEnd[i].first << "~" << startEnd[i].second;
                    if (i < nPairs - 1) {
                        log << ", ";
                    }
                }
            }
            else {
                // if the pointer never got set, all the channels are bad
                log << "no channels";
            }
        }
        if (spwCount < (nspw - 1)) {
            log << ";";
        }
        ++spwCount;
    }
    log << LogIO::POST;
}

void StatWtTVI::_setChanBinMap(const casacore::Quantity& binWidth) {
    if (! binWidth.isConform(Unit("Hz"))) {
        ostringstream oss;
        oss << "If specified as a quantity, channel bin width must have "
            << "frequency units. " << binWidth << " does not.";
        ThrowCc(oss.str());
    }
    ThrowIf(binWidth.getValue() <= 0, "channel bin width must be positive");
    MSMetaData msmd(&ms(), 100.0);
    auto chanFreqs = msmd.getChanFreqs();
    auto nspw = chanFreqs.size();
    auto binWidthHz = binWidth.getValue("Hz");
    for (uInt i=0; i<nspw; ++i) {
        auto cfs = chanFreqs[i].getValue("Hz");
        auto citer = cfs.begin();
        auto cend = cfs.end();
        StatWtTypes::ChanBin bin;
        bin.start = 0;
        bin.end = 0;
        uInt chanNum = 0;
        auto startFreq = *citer;
        auto nchan = cfs.size();
        for (; citer!=cend; ++citer, ++chanNum) {
            // both could be true, in which case both conditionals
            // must be executed
            if (abs(*citer - startFreq) > binWidthHz) {
                // add bin to list
                bin.end = chanNum - 1;
                _chanBins[i].push_back(bin);
                bin.start = chanNum;
                startFreq = *citer;
            }
            if (chanNum + 1 == nchan) {
                // add last bin
                bin.end = chanNum;
                _chanBins[i].push_back(bin);
            }
        }
    }
    // weight spectrum must be computed
    _mustComputeWtSp.reset(new Bool(True));
}

void StatWtTVI::_setChanBinMap(Int binWidth) {
    ThrowIf(binWidth < 1, "Channel bin width must be positive");
    MSMetaData msmd(&ms(), 100.0);
    auto nchans = msmd.nChans();
    auto nspw = nchans.size();
    StatWtTypes::ChanBin bin;
    for (uInt i=0; i<nspw; ++i) {
        auto lastChan = nchans[i]-1;
        for (uInt j=0; j<nchans[i]; j += binWidth) {
            bin.start = j;
            bin.end = min(j+binWidth-1, lastChan);
            _chanBins[i].push_back(bin);
        }
    }
    // weight spectrum must be computed
    _mustComputeWtSp.reset(new Bool(True));
}

void StatWtTVI::_setDefaultChanBinMap() {
    // FIXME uses underlying MS
    MSMetaData msmd(&ms(), 0.0);
    auto nchans = msmd.nChans();
    auto niter = nchans.begin();
    auto nend = nchans.end();
    Int i = 0;
    StatWtTypes::ChanBin bin;
    bin.start = 0;
    for (; niter!=nend; ++niter, ++i) {
        bin.end = *niter - 1;
        _chanBins[i].push_back(bin);
    }
}

Double StatWtTVI::getTimeBinWidthInSec(const casacore::Quantity& binWidth) {
    ThrowIf(
        ! binWidth.isConform(Unit("s")),
        "Time bin width unit must be a unit of time"
    );
    auto v = binWidth.getValue("s");
    checkTimeBinWidth(v);
    return v;
}

void StatWtTVI::checkTimeBinWidth(Double binWidth) {
    ThrowIf(binWidth <= 0, "time bin width must be positive");
}

void StatWtTVI::sigmaSpectrum(Cube<Float>& sigmaSp) const {
    if (_mustComputeSigma) {
        {
            Cube<Float> wtsp;
            // this computes _newWtsp, ignore wtsp
            weightSpectrum(wtsp);
        }
        sigmaSp = Float(1.0)/sqrt(_newWtSp);
        if (anyEQ(_newWtSp, Float(0))) {
            auto iter = sigmaSp.begin();
            auto end = sigmaSp.end();
            auto witer = _newWtSp.cbegin();
            for ( ; iter != end; ++iter, ++witer) {
                if (*witer == 0) {
                    *iter = -1;
                }
            }
        }
    }
    else {
        TransformingVi2::sigmaSpectrum(sigmaSp);
    }
}

void StatWtTVI::weightSpectrum(Cube<Float>& newWtsp) const {
    ThrowIf(! _weightsComputed, "Weights have not been computed yet");
    if (! _dataAggregator->mustComputeWtSp()) {
        newWtsp.resize(IPosition(3, 0));
        return;
    }
    if (! _newWtSp.empty()) {
        // already calculated
        if (_updateWeight) {
            newWtsp = _newWtSp.copy();
        }
        else {
            TransformingVi2::weightSpectrum(newWtsp);
        }
        return;
    }
    _computeWeightSpectrumAndFlags();
    if (_updateWeight) {
        newWtsp = _newWtSp.copy();
    }
    else {
        TransformingVi2::weightSpectrum(newWtsp);
    }
}

void StatWtTVI::_computeWeightSpectrumAndFlags() const {
    size_t nOrigFlagged;
    auto mypair = _getLowerLayerWtSpFlags(nOrigFlagged);
    auto& wtsp = mypair.first;
    auto& flagCube = mypair.second;
    if (_dataAggregator->mustComputeWtSp() && wtsp.empty()) {
        // This can happen in preview mode if
        // WEIGHT_SPECTRUM doesn't exist or is empty
        wtsp.resize(flagCube.shape());
    }
    auto checkFlags = False;
    Vector<Int> ant1, ant2, spws;
    antenna1(ant1);
    antenna2(ant2);
    spectralWindows(spws);
    Vector<rownr_t> rowIDs;
    getRowIds(rowIDs);
    Vector<Double> exposures;
    exposure(exposures);
    _dataAggregator->weightSpectrumFlags(
        wtsp, flagCube, checkFlags, ant1, ant2, spws, exposures, rowIDs
    );
    if (checkFlags) {
        _nNewFlaggedPts += ntrue(flagCube) - nOrigFlagged;
    }
    _newWtSp = wtsp;
    _newFlag = flagCube;
}

std::pair<Cube<Float>, Cube<Bool>> StatWtTVI::_getLowerLayerWtSpFlags(
    size_t& nOrigFlagged
) const {
    auto mypair = std::make_pair(Cube<Float>(), Cube<Bool>());
    if (_dataAggregator->mustComputeWtSp()) {
        getVii()->weightSpectrum(mypair.first);
    }
    getVii()->flag(mypair.second);
    _nTotalPts += mypair.second.size();
    nOrigFlagged = ntrue(mypair.second);
    _nOrigFlaggedPts += nOrigFlagged;
    return mypair;
}

void StatWtTVI::sigma(Matrix<Float>& sigmaMat) const {
    if (_mustComputeSigma) {
        if (_newWt.empty()) {
            Matrix<Float> wtmat;
            weight(wtmat);
        }
        sigmaMat = Float(1.0)/sqrt(_newWt);
        if (anyEQ(_newWt, Float(0))) {
            Matrix<Float>::iterator iter = sigmaMat.begin();
            Matrix<Float>::iterator end = sigmaMat.end();
            Matrix<Float>::iterator witer = _newWt.begin();
            for ( ; iter != end; ++iter, ++witer) {
                if (*witer == 0) {
                    *iter = -1;
                }
            }
        }
    }
    else {
        TransformingVi2::sigma(sigmaMat);
    }
}

void StatWtTVI::weight(Matrix<Float> & wtmat) const {
    ThrowIf(! _weightsComputed, "Weights have not been computed yet");
    if (! _newWt.empty()) {
        if (_updateWeight) {
            wtmat = _newWt.copy();
        }
        else {
            TransformingVi2::weight(wtmat);
        }
        return;
    }
    auto nrows = nRows();
    getVii()->weight(wtmat);
    if (_dataAggregator->mustComputeWtSp()) {
        // always use classical algorithm to get median for weights
        ClassicalStatistics<
            Double, Array<Float>::const_iterator, Array<Bool>::const_iterator
        > cs;
        Cube<Float> wtsp;
        Cube<Bool> flagCube;
        // this computes _newWtsP which is what we will use, so
        // just ignore wtsp
        weightSpectrum(wtsp);
        flag(flagCube);
        IPosition blc(3, 0);
        IPosition trc = _newWtSp.shape() - 1;
        const auto ncorr = _newWtSp.shape()[0];
        for (rownr_t i=0; i<nrows; ++i) {
            blc[2] = i;
            trc[2] = i;
            if (_combineCorr) {
                auto flags = flagCube(blc, trc);
                if (allTrue(flags)) {
                    wtmat.column(i) = 0;
                }
                else {
                    auto weights = _newWtSp(blc, trc);
                    auto mask = ! flags;
                    cs.setData(weights.begin(), mask.begin(), weights.size());
                    wtmat.column(i) = cs.getMedian();
                }
            }
            else {
                for (uInt corr=0; corr<ncorr; ++corr) {
                    blc[0] = corr;
                    trc[0] = corr;
                    auto weights = _newWtSp(blc, trc);
                    auto flags = flagCube(blc, trc);
                    if (allTrue(flags)) {
                        wtmat(corr, i) = 0;
                    }
                    else {
                        auto mask = ! flags;
                        cs.setData(
                            weights.begin(), mask.begin(), weights.size()
                        );
                        wtmat(corr, i) = cs.getMedian();
                    }
                }
            }
        }
    }
    else {
        // the only way this can happen is if there is a single channel bin
        // for each baseline/spw pair
        _dataAggregator->weightSingleChanBin(wtmat, nrows);
    }
    _newWt = wtmat.copy();
    if (! _updateWeight) {
        wtmat = Matrix<Float>(wtmat.shape()); 
        TransformingVi2::weight(wtmat);
    }
}

void StatWtTVI::flag(Cube<Bool>& flagCube) const {
    ThrowIf(! _weightsComputed, "Weights have not been computed yet");
    if (! _newFlag.empty()) {
        flagCube = _newFlag.copy();
        return;
    }
    _computeWeightSpectrumAndFlags();
    flagCube = _newFlag.copy();
}

void StatWtTVI::flagRow(Vector<Bool>& flagRow) const {
    ThrowIf(! _weightsComputed, "Weights have not been computed yet");
    if (! _newFlagRow.empty()) {
        flagRow = _newFlagRow.copy();
        return;
    }
    Cube<Bool> flags;
    flag(flags);
    getVii()->flagRow(flagRow);
    auto nrows = nRows();
    for (rownr_t i=0; i<nrows; ++i) {
        flagRow[i] = allTrue(flags.xyPlane(i));
    }
    _newFlagRow = flagRow.copy();
}

void StatWtTVI::originChunks(Bool forceRewind) {
    // Drive next lower layer
    getVii()->originChunks(forceRewind);
    _weightsComputed = False;
    _dataAggregator->aggregate();
    _weightsComputed = True;
    _clearCache();
    // re-origin this chunk in next layer
    //  (ensures wider scopes see start of the this chunk)
    getVii()->origin();
}

void StatWtTVI::nextChunk() {
    // Drive next lower layer
    getVii()->nextChunk();
    _weightsComputed = False;
    _dataAggregator->aggregate();
    _weightsComputed = True;
    _clearCache();
    // re-origin this chunk next layer
    //  (ensures wider scopes see start of the this chunk)
    getVii()->origin();
}

void StatWtTVI::_clearCache() {
    _newWtSp.resize(0, 0, 0);
    _newWt.resize(0, 0);
    _newFlag.resize(0, 0, 0);
    _newFlagRow.resize(0);
}

Cube<Bool> StatWtTVI::_getResultantFlags(
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

void StatWtTVI::initWeightSpectrum (const Cube<Float>& wtspec) {
    // Pass to next layer down
    getVii()->initWeightSpectrum(wtspec);
}

void StatWtTVI::initSigmaSpectrum (const Cube<Float>& sigspec) {
    // Pass to next layer down
    getVii()->initSigmaSpectrum(sigspec);
}


void StatWtTVI::writeBackChanges(VisBuffer2 *vb) {
    // Pass to next layer down
    getVii()->writeBackChanges(vb);
}

void StatWtTVI::summarizeFlagging() const {
    auto orig = (Double)_nOrigFlaggedPts/(Double)_nTotalPts*100;
    auto stwt = (Double)_nNewFlaggedPts/(Double)_nTotalPts*100;
    auto total = orig + stwt;
    LogIO log(LogOrigin("StatWtTVI", __func__));
    log << LogIO::NORMAL << "Originally, " << orig
        << "% of the data were flagged. StatWtTVI flagged an "
        << "additional " << stwt << "%."  << LogIO::POST;
    log << LogIO::NORMAL << "TOTAL FLAGGED DATA AFTER RUNNING STATWT: "
        << total << "%" << LogIO::POST;
    log << LogIO::NORMAL << std::endl << LogIO::POST;
    if (_nOrigFlaggedPts == _nTotalPts) {
        log << LogIO::WARN << "IT APPEARS THAT ALL THE DATA IN THE INPUT "
            << "MS/SELECTION WERE FLAGGED PRIOR TO RUNNING STATWT"
            << LogIO::POST;
        log << LogIO::NORMAL << std::endl << LogIO::POST;
    }
    else if (_nOrigFlaggedPts + _nNewFlaggedPts == _nTotalPts) {
        log << LogIO::WARN << "IT APPEARS THAT STATWT FLAGGED ALL THE DATA "
            "IN THE REQUESTED SELECTION THAT WASN'T ORIGINALLY FLAGGED"
            << LogIO::POST;
        log << LogIO::NORMAL << std::endl << LogIO::POST;
    }
    String col0 = "SPECTRAL_WINDOW";
    String col1 = "SAMPLES_WITH_NON-ZERO_VARIANCE";
    String col2 = "SAMPLES_WHERE_REAL_PART_VARIANCE_DIFFERS_BY_>50%_FROM_"
        "IMAGINARY_PART";
    log << LogIO::NORMAL << col0 << " " << col1 << " " << col2 << LogIO::POST;
    auto n0 = col0.size();
    auto n1 = col1.size();
    auto n2 = col2.size();
    for (const auto& sample: *_samples) {
        ostringstream oss;
        oss << std::setw(n0) << sample.first << " " << std::setw(n1)
            << sample.second.first << " " << std::setw(n2)
            << sample.second.second;
        log << LogIO::NORMAL << oss.str() << LogIO::POST;
    }
}

void StatWtTVI::summarizeStats(Double& mean, Double& variance) const {
    LogIO log(LogOrigin("StatWtTVI", __func__));
    _logUsedChannels();
    try {
        mean = _wtStats->getStatistic(StatisticsData::MEAN);
        variance = _wtStats->getStatistic(StatisticsData::VARIANCE);
        log << LogIO::NORMAL << "The mean of the computed weights is "
            << mean << LogIO::POST;
        log << LogIO::NORMAL << "The variance of the computed weights is "
            << variance << LogIO::POST;
        log << LogIO::NORMAL << "Weights which had corresponding flags of True "
            << "prior to running this application were not used to compute these "
            << "stats." << LogIO::POST;
    }
    catch (const AipsError& x) {
        log << LogIO::WARN << "There was a problem calculating the mean and "
            << "variance of the weights computed by this application. Perhaps there "
            << "was something amiss with the input MS and/or the selection criteria. "
            << "Examples of such issues are that all the data were originally flagged "
            << "or that the sample size was consistently too small for computations "
            << "of variances" << LogIO::POST;
        setNaN(mean);
        setNaN(variance);
    }
}

void StatWtTVI::origin() {
    // Drive underlying ViImplementation2
    getVii()->origin();
    // Synchronize own VisBuffer
    configureNewSubchunk();
    _clearCache();
}

void StatWtTVI::next() {
    // Drive underlying ViImplementation2
    getVii()->next();
    // Synchronize own VisBuffer
    configureNewSubchunk();
    _clearCache();
}

}

}
