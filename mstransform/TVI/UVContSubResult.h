//# UtilsTVI.h: declaration of the UVContSubResult class
//#
//#  CASA - Common Astronomy Software Applications (http://casa.nrao.edu/)
//#  Copyright (C) Associated Universities, Inc. Washington DC, USA 2021, All rights reserved.
//#  Copyright (C) European Southern Observatory, 2021, All rights reserved.
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

#ifndef UVCContSubResult_H_
#define UVCContSubResult_H_

// casacore types and containers
#include <casacore/casa/BasicSL/Complex.h>
#include <casacore/casa/Containers/Record.h>

#include <unordered_map>

namespace casa { //# NAMESPACE CASA - BEGIN

namespace vi { //# NAMESPACE VI - BEGIN

using std::unordered_map;
  
class UVContSubResult
{
 public:
  void addOneFit(int field, int scan, int spw, int pol, casacore::Complex chiSquared);

  casacore::Record getAccumulatedResult() const;

 private:
  struct FitResultAcc {
    size_t count;
    casacore::Complex chiSqAvg;
    casacore::Complex chiSqMin;
    casacore::Complex chiSqMax;
  };

  // chi_square per field, scan, spw, polarization, real/imag
  unordered_map<int,
    unordered_map<int, unordered_map<int, unordered_map<int, FitResultAcc>>>> accum;
};

} //# NAMESPACE VI - END

} //# NAMESPACE CASA - END

#endif /* UVCContSubResult_H_ */
