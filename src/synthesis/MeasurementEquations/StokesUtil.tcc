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

#include <casacore/casa/aips.h>
#include <synthesis/MeasurementEquations/StokesUtil.h>
#include <casacore/casa/Arrays/IPosition.h>
#include <casacore/casa/Arrays/Vector.h>
#include <casacore/casa/Exceptions/Error.h>

namespace casa { //# NAMESPACE CASA - BEGIN

template<class T, class U> void
unpackStokes(casacore::Array<U>& output, const casacore::Array<T>& input){
  // Create space for the output array
  {
    casacore::IPosition outSize(input.shape());
    outSize.append(casacore::IPosition(1,4));
    output.resize(outSize);
  }
  // Create a reference to the arrays that allows the data to be accessed
  // linearly
  casacore::uInt nelements = input.nelements();
  casacore::Vector<T> linput(input.reform(casacore::IPosition(1,nelements), casacore::IPosition(1,0)));
  casacore::Vector<U> loutput(output.reform(casacore::IPosition(1,output.nelements()), 
				  casacore::IPosition(1,0)));
  // Now copy the data
  casacore::uInt j = 0;
  for (casacore::uInt pol = 0; pol < 4; pol++){
    for (casacore::uInt i = 0; i < nelements; i++){
      loutput(j++) = linput(i)(pol);
    }
  }
};

template<class T, class U> void
repackStokes(casacore::Array<U>& output, const casacore::Array<T>& input){
  // Find which axis is the stokes axis
  casacore::uInt stokesAxis;
  casacore::IPosition inputSize = input.shape();
  // First remove any trailing degenerate axes
  stokesAxis = inputSize.nelements();
  while ((stokesAxis > 0) && (inputSize(stokesAxis-1) == 1))
    stokesAxis--; 
  // Then check that the input array has a slowest dimension of 4
  if (inputSize(stokesAxis-1) != 4) throw(casacore::AipsError(
     "StokesVector::repackStokes() last dimension does not have 4 elements"));
  // resize the output array to suit (include trailing axes)
  {
    casacore::IPosition outSize = inputSize.getFirst(stokesAxis-1);
    if (stokesAxis < input.ndim()){
      outSize.append(inputSize.getLast(input.ndim() - stokesAxis));
    }
    output.resize(outSize);
  }
  
  // Create a reference to the arrays that allows the data to be accessed 
  // linearly
  casacore::Vector<T> linput(input.reform(casacore::IPosition(1,input.nelements()), 
				casacore::IPosition(1,0)));
  casacore::Vector<U> loutput(output.reform(casacore::IPosition(1,output.nelements()), 
				  casacore::IPosition(1,0)));
  // Now copy the data
  casacore::uInt nelements = output.nelements();
  casacore::uInt j = 0;
  for (casacore::uInt pol = 0; pol < 4; pol++){
    for (casacore::uInt i = 0; i < nelements; i++){
      loutput(i)(pol) = linput(j++);
    }
  }
};

} //# NAMESPACE CASA - END

