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

#ifndef STATWTTVI_H_
#define STATWTTVI_H_

#include <msvis/MSVis/TransformingVi2.h>

#include <casacore/casa/aipstype.h>
#include <casacore/casa/Arrays/ArrayIter.h>
#include <casacore/casa/BasicSL/Complex.h>
#include <casacore/ms/MSSel/MSSelection.h>
#include <casacore/scimath/StatsFramework/StatisticsAlgorithmFactory.h>

#include <msvis/MSVis/VisBuffer2.h>
#include <msvis/MSVis/VisibilityIterator2.h>
#include <mstransform/TVI/StatWtClassicalDataAggregator.h>
#include <mstransform/TVI/StatWtTypes.h>
#include <mstransform/TVI/UtilsTVI.h>
#include <stdcasa/variant.h>
#include <stdcasa/StdCasa/CasacSupport.h>

#include <map>
#include <vector>
#include <utility>
#include <set>
#include <memory>

namespace casa {

class StatWtVarianceAndWeightCalculator;

namespace vi {

class StatWtTVI : public TransformingVi2 {

public:

    static const casacore::String CHANBIN;

    // The following fields are supported in the input configuration record
    // combine           String, if contains "corr", data will be aggregated
    //                   across correlations.
    // value in CHANBIN: Int or Quantity String, describes channel bin widths
    //                   in which to aggregate data within spectral windows
    //                   (spw boundaries are not crossed). If not supplied,
    //                   data for all channels in each spectral window are
    //                   aggregated.
    // minsamp:          Int, minimum number of samples required in an
    //                   aggregated set, if less than that, stats are not
    //                   computed and the data in the sample are flagged. If
    //                   not supplied, 2 is used.
    // statalg           String representing what statistics algorithm to use.
    //                   "cl", "ch", "f", "h".
    // maxiter           Int max number of iterations for Chauvenet algorithm
    // zscore            Double zscore for Chauvenet algorithm
    // center            String center for FitToHalf algorithm, "mean",
    //                   "median", or "zero"
    // lside             Bool side to use for FitToHalf algorithm, True means
    //                   <= center side.
    // fence             Double fence value for HingesFences algorithm
    // wtrange           Zero or two element Array<Double>. Specifies the range
    //                   of "good"
    //                   weight values. Data with weights computed to be outside
    //                   this range will be flagged. Both elements must be
    //                   non-negative. If zero length, all weights are
    //                   acceptable.
    // fitspw            String. MSSelection string representing channels to
    //                   exclude from weight computation.
    // datacolumn        String. Data column to use for computing weights.
    //                   Supports 'data' or 'corrected'. Minimum match, case
    //                   insensitive. If not provided. 'corrected' is used.
    // slidetimebin      Bool. If true, use a sliding window for binning in
    //                   time.
    // timebin           Double. Width of sliding time window. Not used if
    //                   doslidetime is not supplied or if doslidetime = false;
    StatWtTVI(
        ViImplementation2* inputVii, const casacore::Record &configuration
    );

    virtual ~StatWtTVI();

    virtual casacore::String ViiType() const {
        return casacore::String("StatWt( ") + getVii()->ViiType() + " )";
    };

    void initWeightSpectrum (const casacore::Cube<casacore::Float>& wtspec);

    void initSigmaSpectrum (const casacore::Cube<casacore::Float>& sigspec);

    void next();

    void origin();

    virtual void weightSpectrum(casacore::Cube<casacore::Float>& wtsp) const;

    virtual void sigmaSpectrum(casacore::Cube<casacore::Float>& sigmaSp) const;

    virtual void weight(casacore::Matrix<casacore::Float> & wtmat) const;
    
    virtual void sigma(casacore::Matrix<casacore::Float> & sigmaMat) const;

    virtual void flag(casacore::Cube<casacore::Bool>& flagCube) const;

    virtual void flagRow (casacore::Vector<casacore::Bool> & flagRow) const;

    void summarizeFlagging() const;

    void summarizeStats(
        casacore::Double& mean, casacore::Double& variance
    ) const;

    // Override unimplemented TransformingVi2 version
    void writeBackChanges(VisBuffer2* vb);

    // these are public so that class StatWt can call them. In general, other
    // clients shouldn't call them.
    static casacore::Double getTimeBinWidthInSec(
        const casacore::Quantity& binWidth
    );

    static void checkTimeBinWidth(casacore::Double binWidth);

protected:

    void originChunks(casacore::Bool forceRewind);

    void nextChunk();
    
private:

    mutable casacore::Bool _weightsComputed = false;
    mutable std::shared_ptr<casacore::Bool> _mustComputeWtSp {};
    mutable casacore::Cube<casacore::Float> _newWtSp {};
    mutable casacore::Matrix<casacore::Float> _newWt {};
    mutable casacore::Cube<casacore::Bool> _newFlag {};
    mutable casacore::Vector<casacore::Bool> _newFlagRow {};
    // The key refers to the spw, the value vector refers to the
    // channel numbers within that spw that are the first, last channel pair
    // in their respective bins
    std::map<casacore::Int, std::vector<StatWtTypes::ChanBin>> _chanBins {};
    casacore::Int _minSamp = 2;
    casacore::Bool _combineCorr {false};
    std::shared_ptr<
        casacore::StatisticsAlgorithm<
            casacore::Double, casacore::Array<casacore::Float>::const_iterator,
            casacore::Array<casacore::Bool>::const_iterator,
            casacore::Array<casacore::Double>::const_iterator
        >
    > _statAlg {} ;
    std::shared_ptr<std::pair<casacore::Double, casacore::Double>> _wtrange {};
    // The _chanSelFlags key is the spw. The value is a Cube for convenience
    // for subchunk computations that require the same shaped cube of flags to
    // be applied. The dimension that counts is the second (zero-based 1) as it
    // has length equal to the number of channels in the spw. A value of True
    // indicates that the channel is "flagged", ie should not be used.
    std::map<casacore::uInt, casacore::Cube<casacore::Bool>> _chanSelFlags {};

    mutable size_t _nTotalPts = 0;
    mutable size_t _nNewFlaggedPts = 0;
    mutable size_t _nOrigFlaggedPts = 0;
    mutable StatWtTypes::Column _column = StatWtTypes::CORRECTED;
    mutable std::shared_ptr<
            std::map<casacore::uInt, std::pair<casacore::uInt, casacore::uInt>>
        > _samples {
        new std::map<
            casacore::uInt, std::pair<casacore::uInt, casacore::uInt>
        >()
    };
    // mutable std::set<casacore::uInt> _processedRowIDs {};
    // mutable std::vector<std::vector<casacore::Double>> _timeWindowWts {};
    // mutable casacore::Cube<casacore::Double> _multiLoopWeights {};
    // if False, the a sliding time window is being used
    casacore::Bool _timeBlockProcessing = true;
    // we can process using classical VI/VB2 algorithm. Only happens if
    // we are not using a sliding time window and if we are not using an
    // integer number of time bins
    casacore::Bool _doClassicVIVB = true;
    // if defined means we are using a window width in seconds
    std::shared_ptr<casacore::Double> _binWidthInSeconds {};
    // if defined means we are using an integer number of timestamps for the
    // bin width
    std::shared_ptr<casacore::Int> _nTimeStampsInBin {};

    casacore::Bool _mustComputeSigma = casacore::False;
    casacore::Bool _updateWeight = casacore::True;
    casacore::Bool _noModel = casacore::False;

    std::shared_ptr<StatWtDataAggregator> _dataAggregator {};

    std::shared_ptr<
        casacore::ClassicalStatistics<casacore::Double,
        casacore::Array<casacore::Float>::const_iterator,
        casacore::Array<casacore::Bool>::const_iterator>
    > _wtStats {};

    void _computeWeightSpectrumAndFlags() const;

    // combines the flag cube with the channel selection flags (if any)
    casacore::Cube<casacore::Bool> _getResultantFlags(
        casacore::Cube<casacore::Bool>& chanSelFlagTemplate,
        casacore::Cube<casacore::Bool>& chanSelFlags,
        casacore::Bool& initChanSelFlags, casacore::Int spw,
        const casacore::Cube<casacore::Bool>& flagCube
    ) const;

    // CAS-12358
    void _logUsedChannels() const;

    casacore::Bool _parseConfiguration(const casacore::Record &configuration);
	
    std::pair<
        casacore::Cube<casacore::Float>, casacore::Cube<casacore::Bool>
    > _getLowerLayerWtSpFlags(size_t& nOrigFlagged) const;

    void _setChanBinMap(casacore::Int binWidth);

    void _setChanBinMap(const casacore::Quantity& binWidth);

    void _setDefaultChanBinMap();

    void _clearCache();

    void _configureStatAlg(const casacore::Record& config);

};

}

}

#endif 

