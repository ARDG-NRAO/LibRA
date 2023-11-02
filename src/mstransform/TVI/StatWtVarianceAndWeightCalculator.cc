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

#include <mstransform/TVI/StatWtVarianceAndWeightCalculator.h>

#include <casacore/casa/Arrays/ArrayMath.h>
#include <casacore/casa/Arrays/Cube.h>

// DEBUG
#include <casacore/casa/IO/ArrayIO.h>

#ifdef _OPENMP
#include <omp.h>
#endif

using namespace casacore;
using namespace std;

namespace casa {

StatWtVarianceAndWeightCalculator::StatWtVarianceAndWeightCalculator(
    const shared_ptr<
        StatisticsAlgorithm<
            Double, Array<Float>::const_iterator, Array<Bool>::const_iterator,
            Array<Double>::const_iterator
        >
    > statAlg,
    shared_ptr<map<uInt, pair<uInt, uInt>>> samples, Int minSamp
) : _statAlg(statAlg->clone()), _samples(samples), _minSamp(minSamp) {}

StatWtVarianceAndWeightCalculator::~StatWtVarianceAndWeightCalculator() {}

Double StatWtVarianceAndWeightCalculator::computeVariance(
    const Cube<Complex>& data,
    const Cube<Bool>& flags, const Vector<Double>& exposures,
    casacore::uInt spw
) const {
    const auto npts = data.size();
    if ((Int)npts < _minSamp || (Int)nfalse(flags) < _minSamp) {
        // not enough points, trivial
        return 0;
    }
    // called in multi-threaded mode, so need to clone this for each thread
    std::unique_ptr<
        StatisticsAlgorithm<
            Double, Array<Float>::const_iterator,
            Array<Bool>::const_iterator, Array<Double>::const_iterator
        >
    > statAlg(_statAlg->clone());
    // some data not flagged
    const auto realPart = real(data);
    const auto imagPart = imag(data);
    const auto mask = ! flags;
    Cube<Double> exposureCube(data.shape());
    const auto nPlanes = data.nplane();
    for (size_t i=0; i<nPlanes; ++i) {
        exposureCube.xyPlane(i) = exposures[i];
    }
    auto riter = realPart.begin();
    auto iiter = imagPart.begin();
    auto miter = mask.begin();
    auto eiter = exposureCube.begin();
    statAlg->setData(riter, eiter, miter, npts);
    auto realStats = statAlg->getStatistics();
    auto realVar = realStats.nvariance/realStats.npts;
    // reset data to imaginary parts
    statAlg->setData(iiter, eiter, miter, npts);
    auto imagStats = statAlg->getStatistics();
    auto imagVar = imagStats.nvariance/imagStats.npts;
    auto varSum = realVar + imagVar;
    // _samples.second can be updated in two different places, so use
    // a local (per thread) variable and update the object's private field in one
    // place
    uInt updateSecond = False;
    if (varSum > 0) {
#ifdef _OPENMP
#pragma omp atomic
#endif
        ++((*_samples)[spw].first);
        if (imagVar == 0 || realVar == 0) {
            updateSecond = True;
        }
        else {
            auto ratio = imagVar/realVar;
            auto inverse = 1/ratio;
            updateSecond = ratio > 1.5 || inverse > 1.5;
        }
        if (updateSecond) {
#ifdef _OPENMP
#pragma omp atomic
#endif
            ++((*_samples)[spw].second);
        }
    }
    return varSum/2;
}

Double StatWtVarianceAndWeightCalculator::computeWeight(
    const Cube<Complex>& data, const Cube<Bool>& flags,
    const Vector<Double>& exposures, uInt spw, Double targetExposure
) const {
    auto varEq = computeVariance(data, flags, exposures, spw);
    return varEq == 0 ? 0 : targetExposure/varEq;
}

Vector<Double> StatWtVarianceAndWeightCalculator::computeWeights(
    const Cube<Complex>& data, const Cube<Bool>& flags,
    const Vector<Double>& exposures, uInt spw
) const {
    auto varEq = computeVariance(data, flags, exposures, spw);
    return varEq == 0 ? Vector<Double>(exposures.size(), 0) : exposures/varEq;
}

}
