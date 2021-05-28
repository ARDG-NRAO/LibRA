// -*- mode: c++ -*-
//# Copyright (C) 1996,1997,1998,1999,2000,2002,2003,2015
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This library is free software; you can redistribute it and/or modify it
//# under the terms of the GNU Library General Public License as published by
//# the Free Software Foundation; either version 2 of the License, or (at your
//# option) any later version.
//#
//# This library is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
//# License for more details.
//#
//# You should have received a copy of the GNU Library General Public License
//# along with this library; if not, write to the Free Software Foundation,
//# Inc., 675 Massachusetts Ave, Cambridge, MA 02139, USA.
//#
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//
// casacore::Float visibilities column data provider
//
#ifndef MSVIS_STATISTICS_VI2_FLOAT_VIS_DATA_PROVIDER_H
#define MSVIS_STATISTICS_VI2_FLOAT_VIS_DATA_PROVIDER_H

#include <casacore/casa/aips.h>
#include <casacore/casa/Arrays/Cube.h>
#include <msvis/MSVis/VisibilityIterator2.h>
#include <msvis/MSVis/statistics/Vi2DataProvider.h>
#include <msvis/MSVis/statistics/Vi2StatsDataIterator.h>

namespace casa {

// Note that, contrary to Vi2CorrectedVisDataProvider and friends, this
// class does not support transformations of the visibility data as that data is
// handed off to the statistics algorithm.
class Vi2FloatVisDataProvider final
	: public Vi2SigmasCubeDataProvider<Vi2StatsFloatIterator> {

public:
	Vi2FloatVisDataProvider(
		vi::VisibilityIterator2 *vi2,
		const std::set<casacore::MSMainEnums::PredefinedColumns> &mergedColumns,
		casacore::Bool omit_flagged_data,
		casacore::Bool use_data_weights);

	const casacore::Cube<casacore::Float>& dataArray();
};

} // namespace casa

#endif // MSVIS_STATISTICS_VI2_FLOAT_VIS_DATA_PROVIDER_H
