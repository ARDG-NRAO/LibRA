//# StokesUtil.cc:  
//# Copyright (C) 1996,1999
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
//# $Id$

#include <casa/aips.h>
#include <synthesis/MeasurementEquations/StokesUtil.h>
#include <casa/Arrays/IPosition.h>
#include <casa/Arrays/Vector.h>
#include <casa/Arrays/ArrayLogical.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

Array<StokesVector> operator* (const Array<Float> &farray, 
			       const StokesVector &sv){
  Array<StokesVector> svarray(farray.shape());
  uInt nelem = farray.nelements();
  IPosition vsize(1,nelem);
  Vector<StokesVector> lsvarray(svarray.reform(vsize));
  Vector<Float> lfarray(farray.reform(vsize));
  for (uInt i = 0; i < nelem; i++){
    lsvarray(i) = sv*lfarray(i);
  }
  return svarray;
}

Bool nearAbs(const StokesVector& val1, const StokesVector& val2, 
	      Double tol){
  return allNearAbs(val1.vector(), val2.vector(), tol);
}

Bool near(const StokesVector& val1, const StokesVector& val2, 
	  Double tol){
  return allNear(val1.vector(), val2.vector(), tol);
}

// define comparison operators on StokesVectors using their 
// length in 4-space
Bool operator>(const StokesVector& left,const StokesVector& right){
  if (innerProduct(left,left) > innerProduct(right, right))
    return true;
  else
    return false;
}


} //# NAMESPACE CASA - END

