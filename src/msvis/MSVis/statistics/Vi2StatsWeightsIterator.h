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
// casacore::Data provider weights iterators, based on weights column
//
#ifndef MSVIS_STATISTICS_VI2_STATS_WEIGHTS_ITERATOR_H_
#define MSVIS_STATISTICS_VI2_STATS_WEIGHTS_ITERATOR_H_

#include <casacore/casa/aips.h>
#include <msvis/MSVis/VisibilityIterator2.h>
#include <msvis/MSVis/VisBuffer2.h>
#include <iterator>

namespace casa {

// Vi2StatsWeightsIterator has the form of a CRTP base class to promote
// efficiency in iterator operations.
//
template<class T>
class Vi2StatsWeightsIterator
	: public std::iterator<std::input_iterator_tag,casacore::Float> {

public:
	Vi2StatsWeightsIterator& operator++();

	Vi2StatsWeightsIterator operator++(int);

	bool operator==(const Vi2StatsWeightsIterator& rhs);

	bool operator!=(const Vi2StatsWeightsIterator& rhs);

	casacore::Float operator*();

	bool atEnd();

protected:
	Vi2StatsWeightsIterator(vi::VisBuffer2 *vb2)
		: vb2(vb2) {};

	Vi2StatsWeightsIterator() {};

	vi::VisBuffer2 *vb2;
};

// Weights iterator over weights cube.
class Vi2StatsWeightsCubeIterator final
	: public Vi2StatsWeightsIterator<Vi2StatsWeightsCubeIterator> {

public:
	Vi2StatsWeightsCubeIterator(vi::VisBuffer2 *vb2);

	Vi2StatsWeightsCubeIterator();

	Vi2StatsWeightsCubeIterator& operator++();

	Vi2StatsWeightsCubeIterator operator++(int);

	bool operator==(const Vi2StatsWeightsCubeIterator& rhs);

	bool operator!=(const Vi2StatsWeightsCubeIterator& rhs);

	casacore::Float operator*();

	bool atEnd();

protected:
	casacore::uInt correlation;
	casacore::uInt nCorrelations;
	casacore::uInt channel;
	casacore::uInt nChannels;
	casacore::rownr_t row;
	casacore::rownr_t nRows;
};

// Weights iterator over row weights.
class Vi2StatsWeightsRowIterator final
	: public Vi2StatsWeightsIterator<Vi2StatsWeightsRowIterator> {

public:
	Vi2StatsWeightsRowIterator(vi::VisBuffer2 *vb2);

	Vi2StatsWeightsRowIterator();

	Vi2StatsWeightsRowIterator& operator++();

	Vi2StatsWeightsRowIterator operator++(int);

	bool operator==(const Vi2StatsWeightsRowIterator& rhs);

	bool operator!=(const Vi2StatsWeightsRowIterator& rhs);

	casacore::Float operator*();

	bool atEnd();

protected:
	casacore::rownr_t row;
	casacore::rownr_t nRows;
};

} // namespace casa

#endif // MSVIS_STATISTICS_VI2_STATS_WEIGHTS_ITERATOR_H_
