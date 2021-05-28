//# DataSampling.cc: Implementation of DataSampling class
//# Copyright (C) 1997,1998,1999,2000,2001,2003
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

#include <synthesis/DataSampling/DataSampling.h>
#include <images/Images/ImageInterface.h>
#include <casa/Arrays/ArrayLogical.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/MaskedArray.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/Vector.h>
#include <casa/Arrays/Matrix.h>
#include <casa/BasicSL/String.h>
#include <casa/Utilities/Assert.h>
#include <casa/Exceptions/Error.h>
#include <casa/sstream.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

DataSampling::DataSampling(){}

//---------------------------------------------------------------------- 
DataSampling& DataSampling::operator=(const DataSampling& other)
{
  if(this!=&other) {
    prf_p=other.prf_p;
    dx_p=other.dx_p;
    data_p=other.data_p;
    sigma_p=other.sigma_p;
  };
  return *this;
};

//----------------------------------------------------------------------
DataSampling::DataSampling(const DataSampling& other)
{
  operator=(other);
}

//----------------------------------------------------------------------
DataSampling::~DataSampling() {
}

const Array<Float>& DataSampling::getPRF() const
{
  return prf_p;
}

const Array<Float>& DataSampling::getData() const
{
  return data_p;
}

const Array<Float>& DataSampling::getSigma() const
{
  return sigma_p;
}

const Array<Float>& DataSampling::getDX() const
{
  return dx_p;
}

void DataSampling::ok() {
}

} //# NAMESPACE CASA - END

