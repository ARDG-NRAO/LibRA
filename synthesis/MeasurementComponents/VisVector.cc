//# VisVector.cc: Implementation of VisVector
//# Copyright (C) 1996,1997,2000,2001,2002,2003
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
//# Correspondence concerning AIPS++ should be adressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//#

#include <synthesis/MeasurementComponents/VisVector.h>

#include <casa/aips.h>
#include <casa/BasicSL/Complex.h>
#include <casa/Arrays/Cube.h>
#include <casa/iostream.h>
#include <casa/Exceptions/Error.h>
#include <casa/namespace.h>


using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

VisVector::VisVector(const VisType& vistype, const Bool& owner) :
  vistype_(vistype),
  owner_(owner),
  v0_(NULL),
  f0_(NULL),
  v_(NULL),
  f_(NULL)
{
  if (owner_) {
    v0_ = new Complex[vistype];
    f0_ = new Bool[vistype];
    origin();
  }
}

VisVector::~VisVector() {
  if (owner_) {
    if (v0_) delete[] v0_;
    if (f0_) delete[] f0_;
  }
}

void VisVector::setType(const VisVector::VisType& type) {

  vistype_=type;

  if (owner_) {
    delete[] v0_;
    delete[] f0_;
    v0_ = new Complex[vistype_];
    f0_ = new Bool[vistype_];
  }

  origin();

}

void VisVector::polznMap() {
  Complex vswap(v_[1]);
  v_[1]=v_[2];
  v_[2]=v_[3];
  v_[3]=vswap;
  if (f0_) {
    Bool fswap(f_[1]);
    f_[1]=f_[2];
    f_[2]=f_[3];
    f_[3]=fswap;
  }
}

void VisVector::polznUnMap() {
  Complex vswap(v_[3]);
  v_[3]=v_[2];
  v_[2]=v_[1];
  v_[1]=vswap;
  if (f0_) {
    Bool fswap(f_[3]);
    f_[3]=f_[2];
    f_[2]=f_[1];
    f_[1]=fswap;
  }
}


VisVector::VisType visType(const Int& ncorr) {

  switch (ncorr) {
  case 4:
    return VisVector::Four;
    break;
  case 2:
    return VisVector::Two;
    break;
  case 1:
    return VisVector::One;
    break;
  default:
    throw(AipsError("No such VisVector type"));
  }
}

ostream& operator<<(ostream& os, const VisVector& vec) {

  Complex *vi;
  vi=vec.v_;
  cout << "[" << *vi;
  ++vi;
  for (Int i=1;i<vec.vistype_;++i,++vi) cout << ", " << *vi;
  if (vec.f0_) {
    cout << " fl=";
    Bool *fi;
    fi=vec.f_;
    for (Int i=0;i<vec.vistype_;++i,++fi) cout << (*fi ? "T" : "F");
  }
  cout << "]";
  return os;
}

} //# NAMESPACE CASA - END

