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

#ifndef STATWTTVIVARIANCEANDWEIGHTCALCULATOR_H_
#define STATWTTVIVARIANCEANDWEIGHTCALCULATOR_H_

#include <casacore/casa/BasicSL/Complexfwd.h>
#include <casacore/scimath/StatsFramework/StatisticsAlgorithm.h>
#include <casacore/casa/Arrays/Cube.h>

#include <map>
#include <memory>
#include <utility>

namespace casa {

// This class is used internally by StatWtTVI to compute variances and weights.
// It should not be used outside of that class.

class StatWtVarianceAndWeightCalculator {

public:

    StatWtVarianceAndWeightCalculator() = delete;

    // The statAlg parameter is cloned in the constructor. The samples parameter
    // is shared between the constructed object and the caller in order to
    // optimize performance
    StatWtVarianceAndWeightCalculator(
        const std::shared_ptr<
            casacore::StatisticsAlgorithm<
                casacore::Double,
                casacore::Array<casacore::Float>::const_iterator,
                casacore::Array<casacore::Bool>::const_iterator,
                casacore::Array<casacore::Double>::const_iterator
            >
        > statAlg,
        std::shared_ptr<
            std::map<casacore::uInt, std::pair<casacore::uInt, casacore::uInt>>
        > samples, casacore::Int minSamp
    );

    ~StatWtVarianceAndWeightCalculator();

    // compute the equivalent exposure weighted varaince,
    // (var_real + var_imag)/2
    casacore::Double computeVariance(
        const casacore::Cube<casacore::Complex>& data,
        const casacore::Cube<casacore::Bool>& flags,
        const casacore::Vector<casacore::Double>& exposures, casacore::uInt spw
    ) const;

    // all the exposures are used in weighting the variance, but the
    // targetExposure is divided by the equivalent variance and returned.
    casacore::Double computeWeight(
        const casacore::Cube<casacore::Complex>& data,
        const casacore::Cube<casacore::Bool>& flags,
        const casacore::Vector<casacore::Double>& exposures,
        casacore::uInt spw, casacore::Double targetExposure
    ) const;


    // compute the exposure weighted weights.
    casacore::Vector<casacore::Double> computeWeights(
        const casacore::Cube<casacore::Complex>& data,
        const casacore::Cube<casacore::Bool>& flags,
        const casacore::Vector<casacore::Double>& exposures, casacore::uInt spw
    ) const;

private:

    std::unique_ptr<
        casacore::StatisticsAlgorithm<
            casacore::Double,
            casacore::Array<casacore::Float>::const_iterator,
            casacore::Array<casacore::Bool>::const_iterator,
            casacore::Array<casacore::Double>::const_iterator
        >
    > _statAlg {};

    mutable std::shared_ptr<
        std::map<casacore::uInt, std::pair<casacore::uInt, casacore::uInt>>
    > _samples {};

    casacore::Int _minSamp;

};

}

#endif
