//# IteratingParameters.h: Interface definition of the IteratingParameters class
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

#ifndef IteratingParameters_H_
#define IteratingParameters_H_

#include <casacore/casa/aips.h>
#include <msvis/MSVis/VisibilityIterator2.h>

namespace casa { //# NAMESPACE CASA - BEGIN
namespace vi { //# NAMESPACE VI - BEGIN

class IteratingParameters {

public:

  IteratingParameters();

  IteratingParameters(casacore::Double chunkInterval, 
		      const SortColumns& sortColumns=SortColumns(), 
		      WeightScaling * weightScaling = 0);
  
  IteratingParameters(const IteratingParameters& other);
  IteratingParameters& operator=(const IteratingParameters& other);

  casacore::Double getChunkInterval() const;
  const SortColumns& getSortColumns() const;
  WeightScaling* getWeightScaling() const;
  

  void setChunkInterval(casacore::Double);
  void setSortColumns(const SortColumns&);
  void setWeightScaling(WeightScaling*);
  
private:

  void validate();

  casacore::Double chunkInterval_p;
  SortColumns sortColumns_p;
  WeightScaling* weightScaling_p;
  
};

} //# NAMESPACE VI - END
} //# NAMESPACE CASA - END

#endif /* IteratingParameters_H_ */
