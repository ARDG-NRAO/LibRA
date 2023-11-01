//# StatWt.h: Class which implements statistical reweighting
//#
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
//# $Id: $

#ifndef STATWT_H_
#define STATWT_H_

#include <casacore/casa/Quanta/Quantum.h>
#include <casacore/scimath/StatsFramework/StatisticsAlgorithmFactory.h>

#include <msvis/MSVis/LayeredVi2Factory.h>

#include <memory>

namespace casacore {
    class LogIO;
    class MeasurementSet;
}

namespace casa {

class StatWtColConfig;

namespace vi {
    class StatWtTVILayerFactory;
    class VisibilityIterator2;
}

// This class implements reweighting of visibilities based on the statwt
// algorithm.

class StatWt {

public:

	StatWt(
	    casacore::MeasurementSet* ms,
	    const StatWtColConfig* const statwtColConfig
	);

    ~StatWt();

    // set columns for which to ignore changes when aggregating data
    void setCombine(const casacore::String& combine);

    void setOutputMS(const casacore::String& outname);

    // set preview mode (True) or not (False)
    void setPreview(casacore::Bool preview);

    void setTimeBinWidth(const casacore::Quantity& binWidth);

    // binWidth must be in seconds
    void setTimeBinWidth(casacore::Double binWidth);

    // set the StatWtTVI config record
    void setTVIConfig(const casacore::Record& config);

    casacore::Record writeWeights();

private:
    casacore::MeasurementSet* _ms;
    casacore::String _outname = "";
    // time bin width in seconds
    casacore::Double _timeBinWidth = 1;
    casacore::LogIO _log;
    std::unique_ptr<casacore::Int> _chanBinWidthInt  {};
    std::unique_ptr<casacore::Record> _chanBinWidthQ = {};
    casacore::String _combine = "";
    casacore::StatisticsAlgorithmFactory<
        casacore::Double, casacore::Array<casacore::Float>::const_iterator,
        casacore::Array<casacore::Bool>::const_iterator
    > _saf;
    std::unique_ptr<std::pair<casacore::Double, casacore::Double>> _wtrange {};
    casacore::Record _tviConfig;
    casacore::Bool _preview = false;
    const StatWtColConfig* _statwtColConfig = nullptr;

    // Construct the iterator
    void _constructVi(
        std::shared_ptr<vi::VisibilityIterator2>& vi,
        std::shared_ptr<vi::StatWtTVILayerFactory>& factory
    ) const;

};

}

#endif 

