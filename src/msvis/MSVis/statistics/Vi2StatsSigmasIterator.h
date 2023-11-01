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
// casacore::Data provider weights iterators, based on sigma column
//
#ifndef MSVIS_STATISTICS_VI2_STATS_SIGMAS_ITERATOR_H_
#define MSVIS_STATISTICS_VI2_STATS_SIGMAS_ITERATOR_H_

#include <casacore/casa/aips.h>
#include <msvis/MSVis/VisibilityIterator2.h>
#include <msvis/MSVis/VisBuffer2.h>
#include <iterator>

namespace casa {

// Vi2StatsSigmasIterator has the form of a CRTP base class to promote
// efficiency in iterator operations.  The weights provided by these iterators
// are converted from sigmas according to 1/(sigma^2) (except for sigma == 0).
//
template<class T>
class Vi2StatsSigmasIterator
	: public std::iterator<std::input_iterator_tag,casacore::Float> {

public:
	Vi2StatsSigmasIterator& operator++();

	Vi2StatsSigmasIterator operator++(int);

	bool operator==(const Vi2StatsSigmasIterator& rhs);

	bool operator!=(const Vi2StatsSigmasIterator& rhs);

	casacore::Float operator*();

	bool atEnd();

protected:
	Vi2StatsSigmasIterator(vi::VisBuffer2 *vb2)
		: vb2(vb2) {};

	Vi2StatsSigmasIterator() {};

	vi::VisBuffer2 *vb2;
};

// Weights iterator over sigma cube.
class Vi2StatsSigmasCubeIterator final
	: public Vi2StatsSigmasIterator<Vi2StatsSigmasCubeIterator> {

public:
	Vi2StatsSigmasCubeIterator(vi::VisBuffer2 *vb2);

	Vi2StatsSigmasCubeIterator();

	Vi2StatsSigmasCubeIterator& operator++();

	Vi2StatsSigmasCubeIterator operator++(int);

	bool operator==(const Vi2StatsSigmasCubeIterator& rhs);

	bool operator!=(const Vi2StatsSigmasCubeIterator& rhs);

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

// Weights iterator over row sigmas.
class Vi2StatsSigmasRowIterator final
	: public Vi2StatsSigmasIterator<Vi2StatsSigmasRowIterator> {

public:
	Vi2StatsSigmasRowIterator(vi::VisBuffer2 *vb2);

	Vi2StatsSigmasRowIterator();

	Vi2StatsSigmasRowIterator& operator++();

	Vi2StatsSigmasRowIterator operator++(int);

	bool operator==(const Vi2StatsSigmasRowIterator& rhs);

	bool operator!=(const Vi2StatsSigmasRowIterator& rhs);

	casacore::Float operator*();

	bool atEnd();

protected:
	casacore::rownr_t row;
	casacore::rownr_t nRows;
};

} // namespace casa

#endif // MSVIS_STATISTICS_VI2_STATS_SIGMAS_ITERATOR_H_
