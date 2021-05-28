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
// Data provider mask iterators, based on a flag column.
//
#include <casacore/casa/Arrays/Cube.h>
#include <casacore/casa/Arrays/Vector.h>
#include <msvis/MSVis/statistics/Vi2StatsFlagsIterator.h>

using namespace casacore;
namespace casa {

Vi2StatsFlagsCubeIterator::Vi2StatsFlagsCubeIterator(vi::VisBuffer2 *vb2)
{
	if (vb2->existsColumn(vi::VisBufferComponent2::FlagCube)) {
		flags_array = &vb2->flagCube();
		expansion_factor = 1;
	} else {
		flags_array = &vb2->flagRow();
		expansion_factor = vb2->nChannels() * vb2->nCorrelations();
	}
	flags_iter = flags_array->begin();
	end_iter = flags_array->end();
	replicate_count = expansion_factor;
}

Vi2StatsFlagsCubeIterator::Vi2StatsFlagsCubeIterator()
	: Vi2StatsFlagsIterator() {}

Vi2StatsFlagsCubeIterator&
Vi2StatsFlagsCubeIterator::operator++()
{
	--replicate_count;
	if (replicate_count == 0) {
		++flags_iter;
		replicate_count = expansion_factor;
	}
	return *this;
}

Vi2StatsFlagsCubeIterator
Vi2StatsFlagsCubeIterator::operator++(int)
{
	Vi2StatsFlagsCubeIterator tmp(*this);
	operator++();
	return tmp;
}

bool
Vi2StatsFlagsCubeIterator::operator==(const Vi2StatsFlagsCubeIterator& rhs)
{
	return (flags_iter == rhs.flags_iter
	        && replicate_count == rhs.replicate_count);
}

bool
Vi2StatsFlagsCubeIterator::operator!=(const Vi2StatsFlagsCubeIterator& rhs)
{
	return (flags_iter != rhs.flags_iter
	        || replicate_count != rhs.replicate_count);
}

Bool
Vi2StatsFlagsCubeIterator::operator*() {
	// Since this class is intended to be used in a StatsDataProvider instance,
	// and StatisticsAlgorithm classes expect a "True" mask value to indicate a
	// valid sample, the value returned by this method must be the inverted
	// value of the current flag value.
	return !*flags_iter;
}

bool
Vi2StatsFlagsCubeIterator::atEnd()
{
	return flags_iter == end_iter;
}



Vi2StatsFlagsRowIterator::Vi2StatsFlagsRowIterator(vi::VisBuffer2 *vb2)
{
	if (vb2->existsColumn(vi::VisBufferComponent2::FlagCube)) {
		flags_array = &vb2->flagCube();
		reduction_factor = vb2->nChannels() * vb2->nCorrelations();
	} else {
		flags_array = &vb2->flagRow();
		reduction_factor = 1;
	}
	flags_iter = flags_array->begin();
	end_iter = flags_array->end();
	prepareNextRow();
}

Vi2StatsFlagsRowIterator::Vi2StatsFlagsRowIterator()
	: Vi2StatsFlagsIterator() {}

Vi2StatsFlagsRowIterator&
Vi2StatsFlagsRowIterator::operator++()
{
	prepareNextRow();
	return *this;
}

Vi2StatsFlagsRowIterator
Vi2StatsFlagsRowIterator::operator++(int)
{
	Vi2StatsFlagsRowIterator tmp(*this);
	operator++();
	return tmp;
}

bool
Vi2StatsFlagsRowIterator::operator==(const Vi2StatsFlagsRowIterator& rhs)
{
	return flags_iter == rhs.flags_iter;
}

bool
Vi2StatsFlagsRowIterator::operator!=(const Vi2StatsFlagsRowIterator& rhs)
{
	return flags_iter != rhs.flags_iter;
}

Bool
Vi2StatsFlagsRowIterator::operator*() {
	// Since this class is intended to be used in a StatsDataProvider instance,
	// and StatisticsAlgorithm classes expect a "True" mask value to indicate a
	// valid sample, the value returned by this method must be the inverted
	// value of the current flag value.
	return !rowFlag;
}

bool
Vi2StatsFlagsRowIterator::atEnd()
{
	return flags_iter == end_iter;
}

void
Vi2StatsFlagsRowIterator::prepareNextRow() {
	rowFlag = *flags_iter;
	for (uInt n = reduction_factor - 1; n > 0; --n) {
		++flags_iter;
		rowFlag = rowFlag && *flags_iter;
	}
	++flags_iter;
}

using namespace casacore;
} // namespace casa
