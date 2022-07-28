/* -*- mode: c++ -*- */
//# MultiParamFielditerator.h: Multiple parameter iterator over Records
//# Copyright (C) 1997,1998,1999,2000,2001,2002,2003
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
#ifndef MULTI_PARAM_FIELD_ITERATOR_H_
#define MULTI_PARAM_FIELD_ITERATOR_H_

#include <synthesis/ImagerObjects/ParamFieldIterator.h>
#include <casacore/casa/Containers/Record.h>
#include <iterator>
#include <array>

namespace casa {

template<size_t N>
class MultiParamFieldIterator
	: public std::iterator<std::forward_iterator_tag, std::array<casacore::Record *,N>,
	                       int> {
	std::array<casacore::Record *,N> records;
	casacore::String prefix;
	casacore::uInt field_index;

public:
	MultiParamFieldIterator()
		: records(std::array<casacore::Record *,N> {})
		, prefix(casacore::String(""))
		, field_index(0) {};

	MultiParamFieldIterator(
		std::array<casacore::Record *,N> &recs, const string prefix = "")
		: records(recs)
		, prefix(casacore::String(prefix))
		, field_index(0) {};

	MultiParamFieldIterator(const MultiParamFieldIterator &fit)
		: records(fit.records)
		, prefix(fit.prefix)
		, field_index(fit.field_index) {};

	MultiParamFieldIterator operator++() {
		++field_index;
		return *this;
	};

	MultiParamFieldIterator operator++(int) {
		MultiParamFieldIterator tmp(*this);
		operator++();
		return tmp;
	};

	bool operator==(const MultiParamFieldIterator &rhs) {
		return records == rhs.records
			&& field_index == rhs.field_index
			&& prefix == rhs.prefix;
	};

	bool operator!=(const MultiParamFieldIterator &rhs) {
		return !operator==(rhs);
	};

	std::array<casacore::Record *,N> operator*() {
		std::array<casacore::Record *,N> result;
		casacore::String field_name = prefix + casacore::String::toString(field_index);
		for (size_t i = 0; i < N; ++i)
			result[i] = &records[i]->rwSubRecord(field_name);
		return result;
	};

	static MultiParamFieldIterator<N> begin(std::array<casacore::Record *,N> &recs,
	                                        const string &prefix = "") {
		return MultiParamFieldIterator(recs, prefix);
	};

	static MultiParamFieldIterator<N> end(std::array<casacore::Record *,N> &recs,
	                                      const string &prefix = "") {
		MultiParamFieldIterator result(recs, prefix);
		result.field_index = recs[0]->nfields();
		return result;
	};
};

} // namespace casa

#endif // MULTI_PARAM_FIELD_ITERATOR_H_
