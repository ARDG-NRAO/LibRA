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
#include <msvis/MSVis/statistics/Vi2StatsWeightsIterator.h>

using namespace casacore;
namespace casa {

Vi2StatsWeightsCubeIterator::Vi2StatsWeightsCubeIterator(vi::VisBuffer2 *vb2)
	: Vi2StatsWeightsIterator(vb2)
	, correlation(0)
	, nCorrelations(vb2->nCorrelations())
	, channel(0)
	, nChannels(vb2->nChannels())
	, row(0)
	, nRows(vb2->nRows()) {}

Vi2StatsWeightsCubeIterator::Vi2StatsWeightsCubeIterator()
	: Vi2StatsWeightsIterator() {}

Vi2StatsWeightsCubeIterator&
Vi2StatsWeightsCubeIterator::operator++()
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

Vi2StatsWeightsCubeIterator
Vi2StatsWeightsCubeIterator::operator++(int)
{
	Vi2StatsWeightsCubeIterator tmp(*this);
	operator++();
	return tmp;
}

bool
Vi2StatsWeightsCubeIterator::operator==(const Vi2StatsWeightsCubeIterator& rhs)
{
	return (correlation == rhs.correlation
	        && channel == rhs.channel
	        && row == rhs.row);
}

bool
Vi2StatsWeightsCubeIterator::operator!=(const Vi2StatsWeightsCubeIterator& rhs)
{
	return (correlation != rhs.correlation
	        || channel != rhs.channel
	        || row != rhs.row);
}

Float
Vi2StatsWeightsCubeIterator::operator*()
{
	return vb2->getWeightScaled(correlation, channel, row);
}

bool
Vi2StatsWeightsCubeIterator::atEnd()
{
	return (correlation == 0 && channel == 0 && row == nRows);
}



Vi2StatsWeightsRowIterator::Vi2StatsWeightsRowIterator(vi::VisBuffer2 *vb2)
	: Vi2StatsWeightsIterator(vb2)
	, nRows(vb2->nRows()) {}

Vi2StatsWeightsRowIterator::Vi2StatsWeightsRowIterator()
	: Vi2StatsWeightsIterator() {}

Vi2StatsWeightsRowIterator&
Vi2StatsWeightsRowIterator::operator++()
{
	++row;
	return *this;
}

Vi2StatsWeightsRowIterator
Vi2StatsWeightsRowIterator::operator++(int)
{
	Vi2StatsWeightsRowIterator tmp(*this);
	operator++();
	return tmp;
}

bool
Vi2StatsWeightsRowIterator::operator==(const Vi2StatsWeightsRowIterator& rhs)
{
	return row == rhs.row;
}

bool
Vi2StatsWeightsRowIterator::operator!=(const Vi2StatsWeightsRowIterator& rhs)
{
	return row != rhs.row;
}

Float
Vi2StatsWeightsRowIterator::operator*()
{
	return vb2->getWeightScaled(row);
}

bool
Vi2StatsWeightsRowIterator::atEnd()
{
	return row == nRows;
}

using namespace casacore;
} // namespace casa
