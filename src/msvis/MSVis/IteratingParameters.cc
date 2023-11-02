//# IteratingParameters.cc: Implementation of the IteratingParameters class
//#
//#  CASA - Common Astronomy Software Applications (http://casa.nrao.edu/)
//#  Copyright (C) Associated Universities, Inc. Washington DC, USA 2015, All rights reserved.
//#  Copyright (C) European Southern Observatory, 2015, All rights reserved.
//#
//#  This library is free software; you can redistribute it and/or
//#  modify it under the terms of the GNU Lesser General Public
//#  License as published by the Free software Foundation; either
//#  version 2.1 of the License, or (at your option) any later version.
//#
//#  This library is distributed in the hope that it will be useful,
//#  but WITHOUT ANY WARRANTY, without even the implied warranty of
//#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//#  Lesser General Public License for more details.
//#
//#  You should have received a copy of the GNU Lesser General Public
//#  License along with this library; if not, write to the Free Software
//#  Foundation, Inc., 59 Temple Place, Suite 330, Boston,
//#  MA 02111-1307  USA
//# $Id: $

#include <msvis/MSVis/IteratingParameters.h>
#include <stdcasa/UtilJ.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN
namespace vi { //# NAMESPACE VI - BEGIN

IteratingParameters::IteratingParameters() :
  chunkInterval_p(0.0),
  sortColumns_p(SortColumns()),
  weightScaling_p(0)
{}

IteratingParameters::IteratingParameters(Double chunkInterval, 
					 const SortColumns& sortColumns, 
					 WeightScaling * weightScaling) :
  chunkInterval_p(chunkInterval),
  sortColumns_p(sortColumns),
  weightScaling_p(weightScaling)
{
  validate();
}
  
IteratingParameters::IteratingParameters(const IteratingParameters& other)
{
  *this = other;
}

IteratingParameters& IteratingParameters::operator=(const IteratingParameters& other)
{
  if (this != &other) {
    chunkInterval_p = other.chunkInterval_p;
    sortColumns_p = other.sortColumns_p;
    weightScaling_p = other.weightScaling_p;
    validate();
  }
  return *this;
}

Double IteratingParameters::getChunkInterval() const
{
  return chunkInterval_p;
}
const SortColumns& IteratingParameters::getSortColumns() const
{
  return sortColumns_p;
}
WeightScaling* IteratingParameters::getWeightScaling() const
{
  return weightScaling_p;
}
  

void IteratingParameters::setChunkInterval(Double chunkInterval)
{
  ThrowIf (chunkInterval>=0, "chunkInterval must be >= 0.0");
  chunkInterval_p=chunkInterval;
}

void IteratingParameters::setSortColumns(const SortColumns& sortColumns)
{
  sortColumns_p=sortColumns;
}
void IteratingParameters::setWeightScaling(WeightScaling* weightScaling)
{
  weightScaling_p = weightScaling;
}
  
void IteratingParameters::validate()
{
  Assert(chunkInterval_p>=0.0);
}

} //# NAMESPACE VI - END
} //# NAMESPACE CASA - END


