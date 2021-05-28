//# Copyright (C) 1998,1999,2000,2001,2003
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This program is free software; you can redistribute it and/or modify it
//# under the terms of the GNU General Public License as published by the Free
//# Software Foundation; either version 2 of the License, or (at your option)
//# any later version.
//#
//# This program is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
//# more details.
//#
//# You should have received a copy of the GNU General Public License along
//# with this program; if not, write to the Free Software Foundation, Inc.,
//# 675 Massachusetts Ave, Cambridge, MA 02139, USA.
//#
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//# $Id: $

#ifndef IMAGEANALYSIS_IMAGESTATSBASE_TCC
#define IMAGEANALYSIS_IMAGESTATSBASE_TCC

#include <imageanalysis/ImageAnalysis/ImageStatsBase.h>

namespace casa {

template <class T> ImageStatsBase<T>::ImageStatsBase(
    const SPCIIT image, const Record *const &regionPtr, const String& maskInp,
    const casacore::String& outname, casacore::Bool overwrite
) : ImageTask<T>(
        image, regionPtr, maskInp, outname, overwrite
    ), _statistics(), _algConf() {
    _algConf.algorithm = StatisticsData::CLASSICAL;
}

template <class T> ImageStatsBase<T>::~ImageStatsBase() {}

template <class T> void ImageStatsBase<T>::configureBiweight(Int maxIter) {
    if (
        _algConf.algorithm != StatisticsData::BIWEIGHT
        || maxIter != _algConf.mi
    ) {
        _algConf.algorithm = StatisticsData::BIWEIGHT;
        _algConf.mi = maxIter;
        _statistics.reset();
    }
}

template <class T> void ImageStatsBase<T>::configureChauvenet(
    casacore::Double zscore, casacore::Int maxIterations
) {
    if (
        _algConf.algorithm != StatisticsData::CHAUVENETCRITERION
        || ! casacore::near(_algConf.zs, zscore)
        || maxIterations != _algConf.mi
    ) {
        _algConf.algorithm = StatisticsData::CHAUVENETCRITERION;
        _algConf.zs = zscore;
        _algConf.mi = maxIterations;
        _statistics.reset();
    }
}

template <class T> void ImageStatsBase<T>::configureClassical(
    ImageStatsData::PreferredClassicalAlgorithm p
) {
    if (
        _algConf.algorithm != casacore::StatisticsData::CLASSICAL
        || p != _prefClassStatsAlg
    ) {
        _algConf.algorithm = casacore::StatisticsData::CLASSICAL;
        _prefClassStatsAlg = p;
        _statistics.reset();
    }
}

template <class T> void ImageStatsBase<T>::configureFitToHalf(
    FitToHalfStatisticsData::CENTER centerType,
    FitToHalfStatisticsData::USE_DATA useData,
    casacore::Double centerValue
) {
    if (
        _algConf.algorithm != StatisticsData::FITTOHALF
        || _algConf.ct != centerType
        || _algConf.ud != useData
        || (
            centerType == FitToHalfStatisticsData::CVALUE
            && ! casacore::near(_algConf.cv, centerValue)
        )
    ) {
        _algConf.algorithm = StatisticsData::FITTOHALF;
        _algConf.ct = centerType;
        _algConf.ud = useData;
        _algConf.cv = centerValue;
        _statistics.reset();
    }
}

template <class T>
void ImageStatsBase<T>::configureHingesFences(casacore::Double f) {
    if (
        _algConf.algorithm != StatisticsData::HINGESFENCES
        || ! casacore::near(_algConf.hf, f)
    ) {
        _algConf.algorithm = StatisticsData::HINGESFENCES;
        _algConf.hf = f;
        _statistics.reset();
    }
}

template <class T> String ImageStatsBase<T>::_configureAlgorithm() {
    String myAlg;
    switch (_algConf.algorithm) {
    case StatisticsData::BIWEIGHT:
        _statistics->configureBiweight(_algConf.mi, 6.0);
        myAlg = "Biweight";
        break;
    case StatisticsData::CHAUVENETCRITERION:
        _statistics->configureChauvenet(_algConf.zs, _algConf.mi);
        myAlg = "Chauvenet Criterion/Z-score";
        break;
    case StatisticsData::CLASSICAL:
        switch (_prefClassStatsAlg) {
        case ImageStatsData::AUTO:
            _statistics->configureClassical();
            break;
        case ImageStatsData::TILED_APPLY:
            _statistics->configureClassical(0, 0, 1, 1);
            break;
        case ImageStatsData::STATS_FRAMEWORK:
            _statistics->configureClassical(1, 1, 0, 0);
            break;
        default:
            ThrowCc("Unhandled classical stats type");
        }
        myAlg = "Classic";
        break;
    case StatisticsData::FITTOHALF:
        _statistics->configureFitToHalf(_algConf.ct, _algConf.ud, _algConf.cv);
        myAlg = "Fit-to-Half";
        break;
    case StatisticsData::HINGESFENCES:
        _statistics->configureHingesFences(_algConf.hf);
        myAlg = "Hinges-Fences";
        break;
    default:
        ThrowCc(
            "Logic Error: Unhandled statistics algorithm "
            + String::toString(_algConf.algorithm)
        );
    }
    return myAlg;
}

}

#endif
