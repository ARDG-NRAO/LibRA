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
// Data provider weights iterators, based on sigma column
//
#include <msvis/MSVis/statistics/Vi2StatsSigmasIterator.h>

using namespace casacore;
namespace casa {

Vi2StatsSigmasCubeIterator::Vi2StatsSigmasCubeIterator(vi::VisBuffer2 *vb2)
	: Vi2StatsSigmasIterator(vb2)
	, correlation(0)
	, nCorrelations(vb2->nCorrelations())
	, channel(0)
	, nChannels(vb2->nChannels())
	, row(0)
	, nRows(vb2->nRows()) {}

Vi2StatsSigmasCubeIterator::Vi2StatsSigmasCubeIterator()
	: Vi2StatsSigmasIterator() {}

Vi2StatsSigmasCubeIterator&
Vi2StatsSigmasCubeIterator::operator++()
{
	if (++correlation == nCorrelations) {
		correlation = 0;
		if (++channel == nChannels) {
			channel = 0;
			++row;
		}
	}
	return *this;
}

Vi2StatsSigmasCubeIterator
Vi2StatsSigmasCubeIterator::operator++(int)
{
	Vi2StatsSigmasCubeIterator tmp(*this);
	operator++();
	return tmp;
}

bool
Vi2StatsSigmasCubeIterator::operator==(const Vi2StatsSigmasCubeIterator& rhs)
{
	return (correlation == rhs.correlation
	        && channel == rhs.channel
	        && row == rhs.row);
}

bool
Vi2StatsSigmasCubeIterator::operator!=(const Vi2StatsSigmasCubeIterator& rhs)
{
	return (correlation != rhs.correlation
	        || channel != rhs.channel
	        || row != rhs.row);
}

Float
Vi2StatsSigmasCubeIterator::operator*()
{
	Float sigma = vb2->getSigmaScaled(correlation, channel, row);
	return ((sigma > 0.0f) ? (1.0f / (sigma * sigma)) : 0.0f);
}

bool
Vi2StatsSigmasCubeIterator::atEnd()
{
	return (correlation == 0 && channel == 0 && row == nRows);
}



Vi2StatsSigmasRowIterator::Vi2StatsSigmasRowIterator(vi::VisBuffer2 *vb2)
	: Vi2StatsSigmasIterator(vb2)
	, nRows(vb2->nRows()) {}

Vi2StatsSigmasRowIterator::Vi2StatsSigmasRowIterator()
	: Vi2StatsSigmasIterator() {}

Vi2StatsSigmasRowIterator&
Vi2StatsSigmasRowIterator::operator++()
{
	++row;
	return *this;
}

Vi2StatsSigmasRowIterator
Vi2StatsSigmasRowIterator::operator++(int)
{
	Vi2StatsSigmasRowIterator tmp(*this);
	operator++();
	return tmp;
}

bool
Vi2StatsSigmasRowIterator::operator==(const Vi2StatsSigmasRowIterator& rhs)
{
	return row == rhs.row;
}

bool
Vi2StatsSigmasRowIterator::operator!=(const Vi2StatsSigmasRowIterator& rhs)
{
	return row != rhs.row;
}

Float
Vi2StatsSigmasRowIterator::operator*()
{
	Float sigma = vb2->getSigmaScaled(row);
	return ((sigma > 0.0f) ? (1.0f / (sigma * sigma)) : 0.0f);
}

bool
Vi2StatsSigmasRowIterator::atEnd()
{
	return row == nRows;
}

using namespace casacore;
} // namespace casa
