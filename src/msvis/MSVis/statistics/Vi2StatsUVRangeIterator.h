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
// casacore::Data provider of uvrange values, based on uvw column
//
#ifndef MSVIS_STATISTICS_VI2_STATS_UV_RANGE_ITERATOR_H_
#define MSVIS_STATISTICS_VI2_STATS_UV_RANGE_ITERATOR_H_

#include <casacore/casa/aips.h>
#include <casacore/casa/Arrays/Array.h>
#include <iterator>
#include <cmath>

namespace casa {

class Vi2StatsUVRangeIterator final
	: public std::iterator<std::input_iterator_tag,casacore::Double> {

public:

	typedef casacore::Double AccumType;
	typedef casacore::Double DataType;

	Vi2StatsUVRangeIterator(const casacore::Array<casacore::Double>& a);

	Vi2StatsUVRangeIterator();

	Vi2StatsUVRangeIterator& operator++();

	Vi2StatsUVRangeIterator operator++(int);

	bool operator==(const Vi2StatsUVRangeIterator& rhs);

	bool operator!=(const Vi2StatsUVRangeIterator& rhs);

	casacore::Double operator*();

	bool atEnd();

	casacore::uInt64 getCount();

protected:

	const casacore::Array<casacore::Double>* array;

	casacore::Array<casacore::Double>::const_iterator array_iter;

	casacore::Array<casacore::Double>::const_iterator end_iter;

	static const casacore::Array<casacore::Double> empty_array;
};

} // namespace casa

#endif // MSVIS_STATISTICS_VI2_STATS_UV_RANGE_ITERATOR_H_
