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
//# $Id: tSubImage.cc 20567 2009-04-09 23:12:39Z gervandiepen $

#ifndef IMAGEANALYSIS_IMAGESTATSBASE_H
#define IMAGEANALYSIS_IMAGESTATSBASE_H

#include <imageanalysis/ImageAnalysis/ImageTask.h>

#include <imageanalysis/ImageAnalysis/ImageStatsData.h>

#include <casacore/images/Images/ImageStatistics.h>
#include <casacore/casa/namespace.h>

#include <memory>

namespace casacore {

template <class T> class CountedPtr;

}

namespace casa {

class C11Timer;

template <class T> class ImageStatsBase: public ImageTask<T> {

    // <summary>
    // This adds configuration methods for statistics classes.
    // </summary>

    // <reviewed reviewer="" date="" tests="" demos="">
    // </reviewed>

    // <prerequisite>
    // </prerequisite>

    // <etymology>
    // Image statistics configuration
    // </etymology>

    // <synopsis>
    // This adds configuration methods for statistics classes.
    // </synopsis>

public:

    ImageStatsBase() = delete;

    ~ImageStatsBase();

    void configureBiweight(casacore::Int maxIter);

    void configureChauvenet(
        casacore::Double zscore, casacore::Int maxIterations
    );

    void configureClassical(ImageStatsData::PreferredClassicalAlgorithm p);

    // configure fit to half algorithm
    void configureFitToHalf(
        casacore::FitToHalfStatisticsData::CENTER centerType,
        casacore::FitToHalfStatisticsData::USE_DATA useData,
        casacore::Double centerValue
    );

    // configure hinges-fences algorithm
    void configureHingesFences(casacore::Double f);

protected:

    struct AlgConf {
        casacore::StatisticsData::ALGORITHM algorithm;
        // hinges-fences f factor
        casacore::Double hf;
        // fit to have center type
        casacore::FitToHalfStatisticsData::CENTER ct;
        // fit to half data portion to use
        casacore::FitToHalfStatisticsData::USE_DATA ud;
        // fit to half center value
        T cv;
        // Chauvenet zscore
        casacore::Double zs;
        // Chauvenet/Biweight max iterations
        casacore::Int mi;
    };

    ImageStatsBase(
        const SPCIIT image,
        const casacore::Record *const &regionPtr,
        const casacore::String& maskInp,
        const casacore::String& outname="",
        casacore::Bool overwrite=false
    );

    casacore::String _configureAlgorithm();

    std::unique_ptr<casacore::ImageStatistics<T>>& _getImageStats() {
        return _statistics;
    }

    StatisticsData::ALGORITHM _getAlgorithm() const {
        return _algConf.algorithm;
    }

    AlgConf _getAlgConf() const {
        return _algConf;
    }

    void _resetStats(ImageStatistics<T>* stat=nullptr) {
        _statistics.reset(stat);
    }

private:

    std::unique_ptr<casacore::ImageStatistics<T>> _statistics;
    AlgConf _algConf;
    ImageStatsData::PreferredClassicalAlgorithm _prefClassStatsAlg
        = ImageStatsData::AUTO;

};

}

#ifndef AIPS_NO_TEMPLATE_SRC
#include <imageanalysis/ImageAnalysis/ImageStatsBase.tcc>
#endif

#endif
