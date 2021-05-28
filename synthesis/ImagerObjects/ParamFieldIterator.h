/* -*- mode: c++ -*- */
//# ParamFieldIterator.h: Single field parameter iterator over Records
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
#ifndef PARAM_FIELD_ITERATOR_H_
#define PARAM_FIELD_ITERATOR_H_

#include <casa/Containers/Record.h>
#include <iterator>

namespace casa {

class ParamFieldIterator
	: public std::iterator<std::forward_iterator_tag, casacore::Record *, int> {
	casacore::Record *record;
	casacore::String prefix;
	casacore::uInt field_index;

public:
	ParamFieldIterator()
		: record(nullptr)
		, prefix("")
		, field_index(0) {};

	ParamFieldIterator(casacore::Record *rec, const string &prefix = "")
		: record(rec)
		, prefix(casacore::String(prefix))
		, field_index(0) {};

	ParamFieldIterator(const ParamFieldIterator &fit)
		: record(fit.record)
		, prefix(fit.prefix)
		, field_index(fit.field_index) {};

	ParamFieldIterator & operator++() {
		++field_index;
		return *this;
	};

	ParamFieldIterator operator++(int) {
		ParamFieldIterator tmp(*this);
		operator++();
		return tmp;
	};

	bool operator==(const ParamFieldIterator &rhs) {
		return record == rhs.record
			&& field_index == rhs.field_index
			&& prefix == rhs.prefix;
	};

	bool operator!=(const ParamFieldIterator &rhs) {
		return !operator==(rhs);
	};

	casacore::Record & operator*() {
		return record->rwSubRecord(prefix + casacore::String::toString(field_index));
	};

	static ParamFieldIterator begin(casacore::Record *rec, const string &prefix = "") {
		return ParamFieldIterator(rec, prefix);
	};

	static ParamFieldIterator end(casacore::Record *rec, const string &prefix = "") {
		ParamFieldIterator result(rec, prefix);
		result.field_index = rec->nfields();
		return result;
	};
};

}

#endif // FIELD_ITERATOR_H_
