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
// Data provider of uvrange values, based on uvw column
//
#include <msvis/MSVis/statistics/Vi2StatsUVRangeIterator.h>
#include <casacore/casa/Arrays/Matrix.h>
#include <cmath>

using namespace casacore;
namespace casa {

Vi2StatsUVRangeIterator::Vi2StatsUVRangeIterator(const Array<Double>& a)
	: array(&a)
	, array_iter(array->begin())
	, end_iter(array->end()) {}

Vi2StatsUVRangeIterator::Vi2StatsUVRangeIterator()
	: Vi2StatsUVRangeIterator(empty_array) {}

Vi2StatsUVRangeIterator&
Vi2StatsUVRangeIterator::operator++()
{
	++array_iter;
	++array_iter;
	++array_iter;
	return *this;
}

Vi2StatsUVRangeIterator
Vi2StatsUVRangeIterator::operator++(int)
{
	Vi2StatsUVRangeIterator tmp(*this);
	operator++();
	return tmp;
}

bool
Vi2StatsUVRangeIterator::operator==(const Vi2StatsUVRangeIterator& rhs)
{
	return array_iter == rhs.array_iter;
}

bool
Vi2StatsUVRangeIterator::operator!=(const Vi2StatsUVRangeIterator& rhs)
{
	return array_iter != rhs.array_iter;
}

Double
Vi2StatsUVRangeIterator::operator*()
{
	Matrix<Double>::const_iterator iter = array_iter;
	Double u = *iter;
	Double v = *(++iter);
	return std::hypot(u, v);
}

bool
Vi2StatsUVRangeIterator::atEnd()
{
	return array_iter == end_iter;
}

uInt64
Vi2StatsUVRangeIterator::getCount()
{
	return array->size() / 3;
}

const Array<Double> Vi2StatsUVRangeIterator::empty_array;

using namespace casacore;
} // namespace casa
