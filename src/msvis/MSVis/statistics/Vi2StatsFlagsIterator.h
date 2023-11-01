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
// casacore::Data provider mask iterators, based on a flag column.
//
#ifndef MSVIS_STATISTICS_VI2_STATS_FLAGS_ITERATOR_H_
#define MSVIS_STATISTICS_VI2_STATS_FLAGS_ITERATOR_H_

#include <casacore/casa/aips.h>
#include <casacore/casa/Arrays/Array.h>
#include <msvis/MSVis/VisibilityIterator2.h>
#include <msvis/MSVis/VisBuffer2.h>
#include <memory>
#include <iterator>

namespace casa {

// Vi2StatsFlagsIterator has the form of a CRTP base class to promote
// efficiency in iterator operations.
//
template <class T>
class Vi2StatsFlagsIterator
	: public std::iterator<std::input_iterator_tag,casacore::Bool> {

public:
	Vi2StatsFlagsIterator& operator++();

	Vi2StatsFlagsIterator operator++(int);

	bool operator==(const Vi2StatsFlagsIterator& rhs);

	bool operator!=(const Vi2StatsFlagsIterator& rhs);

	casacore::Bool operator*();

	bool atEnd();

protected:
	Vi2StatsFlagsIterator()
		: flags_array(&empty_array)
		, flags_iter(empty_array.begin())
		, end_iter(empty_array.end()) {};

	const casacore::Array<casacore::Bool>* flags_array;

	casacore::Array<casacore::Bool>::const_iterator flags_iter;

	casacore::Array<casacore::Bool>::const_iterator end_iter;

	static const casacore::Array<casacore::Bool> empty_array;
};

template <class T>
const casacore::Array<casacore::Bool> Vi2StatsFlagsIterator<T>::empty_array;

// Mask iterator over flag cube. If the flag cube column is not present, this
// iterator will provide values as if the flag cube were present by replicating
// flag row values in order to mimic the shape of the flag cube.
class Vi2StatsFlagsCubeIterator final
	: public Vi2StatsFlagsIterator<Vi2StatsFlagsCubeIterator> {
public:
	Vi2StatsFlagsCubeIterator(vi::VisBuffer2 *vb2);

	Vi2StatsFlagsCubeIterator();

	Vi2StatsFlagsCubeIterator& operator++();

	Vi2StatsFlagsCubeIterator operator++(int);

	bool operator==(const Vi2StatsFlagsCubeIterator& rhs);

	bool operator!=(const Vi2StatsFlagsCubeIterator& rhs);

	casacore::Bool operator*();

	bool atEnd();

protected:
	casacore::uInt expansion_factor;

private:
	casacore::uInt replicate_count;
};

// Mask iterator over row flags. If the row flags column is not present, this
// iterator will provide values as if the row flags column were present by
// reducing the flag cube values to mimic the shape of the flag row column.
class Vi2StatsFlagsRowIterator final
	: public Vi2StatsFlagsIterator<Vi2StatsFlagsRowIterator> {
public:
	Vi2StatsFlagsRowIterator(vi::VisBuffer2 *vb2);

	Vi2StatsFlagsRowIterator();

	Vi2StatsFlagsRowIterator& operator++();

	Vi2StatsFlagsRowIterator operator++(int);

	bool operator==(const Vi2StatsFlagsRowIterator& rhs);

	bool operator!=(const Vi2StatsFlagsRowIterator& rhs);

	casacore::Bool operator*();

	bool atEnd();

protected:
	casacore::uInt reduction_factor;

private:
	casacore::Bool rowFlag;
	void prepareNextRow();
};

} // namespace casa


#endif // MSVIS_STATISTICS_VI2_STATS_FLAGS_ITERATOR_H_
