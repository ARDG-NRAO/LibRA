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
// Data iterators for Vi2ChunkDataProvider
//
#ifndef MSVIS_STATISTICS_VI2_STATS_DATA_ITERATOR_H_
#define MSVIS_STATISTICS_VI2_STATS_DATA_ITERATOR_H_

#include <casacore/casa/aips.h>
#include <casacore/casa/Arrays/Array.h>
#include <iterator>


namespace casa {

//
// Vi2StatsDataIterator is designed to support an on-the-fly application of a
// transformation between the values in the source array and the values provided
// by the iterator. This feature allows a Vi2ChunkDataProvider to provide, for
// example, visibility amplitudes during iteration over visibilities.
//
template<class Transformed, class Data>
class Vi2StatsDataIterator
	: public std::iterator<std::input_iterator_tag,Transformed> {

public:
	typedef Transformed AccumType;
	typedef Data DataType;

	Vi2StatsDataIterator(const casacore::Array<Data>& a)
		: array(&a)
		, array_iter(array->begin())
		, end_iter(array->end()) {}

	Vi2StatsDataIterator()
		: Vi2StatsDataIterator(empty_array) {}

	Vi2StatsDataIterator& operator++() {
		++array_iter;
		return *this;
	}

	Vi2StatsDataIterator operator++(int) {
		Vi2StatsDataIterator tmp(*this);
		operator++();
		return tmp;
	}

	bool operator==(const Vi2StatsDataIterator& rhs) {
		return array_iter == rhs.array_iter;
	}

	bool operator!=(const Vi2StatsDataIterator& rhs) {
		return array_iter != rhs.array_iter;
	}

	Transformed operator*();

	bool atEnd() {
		return array_iter == end_iter;
	}

	casacore::uInt64 getCount() {
		return array->size();
	}

protected:

	const casacore::Array<Data>* array;

	typename casacore::Array<Data>::const_iterator array_iter;

	typename casacore::Array<Data>::const_iterator end_iter;

	static const casacore::Array<Data> empty_array;

};


template<class Transformed, class Data>
const casacore::Array<Data> Vi2StatsDataIterator<Transformed,Data>::empty_array;

// Simple non-transforming (widening excepted) data iterator types.
//
template <class T>
class DataIteratorMixin : public T {

public:
	using T::T;

	typename T::AccumType operator*() {
		return *T::array_iter;
	}
};

typedef DataIteratorMixin< Vi2StatsDataIterator<casacore::Double,casacore::Float> >
Vi2StatsFloatIterator;

typedef DataIteratorMixin< Vi2StatsDataIterator<casacore::Double,casacore::Double> >
Vi2StatsDoubleIterator;

typedef DataIteratorMixin< Vi2StatsDataIterator<casacore::Double,casacore::Int> >
Vi2StatsIntIterator;

}


#endif // MSVIS_STATISTICS_VI2_STATS_DATA_ITERATOR_H_
