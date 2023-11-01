//# Jones.h: Definition of Jones
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


#include <synthesis/MeasurementComponents/Jones.h>
#include <casacore/casa/aips.h>
#include <casacore/casa/BasicSL/Complex.h>
#include <iostream>
#include <casacore/casa/Exceptions/Error.h>
#include <casacore/casa/namespace.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN
  

// Constructor
Jones::Jones() : 
  j0_(NULL), 
  ok0_(NULL),
  j_(NULL), 
  ji_(NULL),
  ok_(NULL),
  oki_(NULL),
  cOne_(1.0),
  cZero_(0.0),
  scalardata_(false),
  vtmp_(VisVector::Four,true)
{}

// In place invert
void Jones::invert() {

  if (!ok_) throw(AipsError("Illegal use of Jones::invert()"));

  if (ok_[0]&&ok_[1]&&ok_[2]&&ok_[3]) {
    Complex det,tmp;
    tmp=j_[0];
    det=j_[0]*j_[3]-j_[1]*j_[2];
    if (abs(det)>0.0) {
      j_[0]=j_[3]/det;
      j_[1]=-j_[1]/det;
      j_[2]=-j_[2]/det;
      j_[3]=tmp/det;
    } else {
      zero();
      ok_[0]=ok_[1]=ok_[2]=ok_[3]=false;
    }
  }
  else {
    ok_[0]=ok_[1]=ok_[2]=ok_[3]=false;
    zero();
  }

}

// Set matrix elements according to ok flag
//   (so we don't have to check ok flags atomically in apply)
void Jones::setMatByOk() {

  // Not needed if ok_ not set
  if (!ok_) return;

  if (!ok_[0] ||
      !ok_[1] ||
      !ok_[2] ||
      !ok_[3]) {
    j_[0]=j_[3]=cOne_;
    j_[1]=j_[2]=cZero_;
  }

}

  

// In-place multipication with another Jones
void Jones::operator*=(const Jones& other) {

  switch(other.type()) {
  case Jones::General: 
  case Jones::Diagonal:
  case Jones::Scalar:
  default:
    throw(AipsError("General multiplication NYI"));
    break;
  }
}

// Apply rightward to a VisVector
void Jones::applyRight(VisVector& v) const {

  // full general right-ward apply
  //
  //  [w x][A B] = [wA+xC wB+xD]  [00+12 01+13]
  //  [y z][C D]   [yA+zC yB+ZD]  [20+32 21+23]


  // flag
  if (v.f_) flagRight(v);

  switch(v.type()) {
  case VisVector::Four: {
    vtmp_.v_[0] = v.v_[2];
    vtmp_.v_[1] = v.v_[3];
    vtmp_.v_[2] = v.v_[0];
    vtmp_.v_[3] = v.v_[1];
    
    v.v_[0]*=(j_[0]);
    v.v_[1]*=(j_[0]);
    v.v_[2]*=(j_[3]);
    v.v_[3]*=(j_[3]);
    
    vtmp_.v_[0]*=(j_[1]);
    vtmp_.v_[1]*=(j_[1]);
    vtmp_.v_[2]*=(j_[2]);
    vtmp_.v_[3]*=(j_[2]);
    
    for (Int i=0;i<4;++i)
      v.v_[i] += vtmp_.v_[i];

    break;
  }
  default:
    throw(AipsError("Jones matrix apply (J::aR) incompatible with VisVector."));
  }
}

// Apply rightward to a VisVector
void Jones::applyRight(VisVector& v, Bool& vflag) const {

  if (!ok_) throw(AipsError("Illegal use of Jones::applyRight(v,vflag)"));

  applyFlag(vflag);
  applyRight(v);
  
}
  
// Apply leftward (transposed) to a VisVector 
void Jones::applyLeft(VisVector& v) const {

  // full general left-ward (conjugate transpose) apply
  //
  //  [A B][w y*] = [Aw+Bx* Ay*+Bz]  [00+11 02+13]
  //  [C D][x* z]   [Cw+Dx* Cy*+Dz]  [20+31 22+33]


  // flag
  if (v.f_) flagLeft(v);

  switch(v.type()) {
  case VisVector::Four: {
    vtmp_.v_[0] = v.v_[1];
    vtmp_.v_[1] = v.v_[0];
    vtmp_.v_[2] = v.v_[3];
    vtmp_.v_[3] = v.v_[2];
    
    Complex c;
    c=conj(j_[0]);
    v.v_[0]*=c;
    v.v_[2]*=c;
    c=conj(j_[3]);
    v.v_[1]*=c;
    v.v_[3]*=c;
    
    c=conj(j_[1]);
    vtmp_.v_[0]*=c;
    vtmp_.v_[2]*=c;
    c=conj(j_[2]);
    vtmp_.v_[1]*=c;
    vtmp_.v_[3]*=c;
  
    for (Int i=0;i<4;++i)
      v.v_[i] += vtmp_.v_[i];

    break;
  }
  default:
    throw(AipsError("Jones matrix apply (J::aL) incompatible with VisVector."));
  }
}

// Apply leftward to a VisVector
void Jones::applyLeft(VisVector& v, Bool& vflag) const {

  if (!ok_) throw(AipsError("Illegal use of Jones::applyLeft(v,vflag)"));

  applyFlag(vflag);
  applyLeft(v);

}

// Set flags according to solution flags
//  (non-corr-dep flag version)
void Jones::applyFlag(Bool& vflag) const {
  vflag|=(!(ok_[0]&&ok_[1]&&ok_[2]&&ok_[3]));
}
// Corr-dep flag rightward onto a VisVector
void Jones::flagRight(VisVector& v) const {
  switch(v.type()) {
  case VisVector::Four: {

    vtmp_.f_[0]=(v.f_[0]||v.f_[2]||(!ok_[0])||(!ok_[1]));
    vtmp_.f_[1]=(v.f_[1]||v.f_[3]||(!ok_[0])||(!ok_[1]));
    vtmp_.f_[2]=(v.f_[0]||v.f_[2]||(!ok_[2])||(!ok_[3]));
    vtmp_.f_[3]=(v.f_[1]||v.f_[3]||(!ok_[2])||(!ok_[3]));
    
    for (Int i=0;i<4;++i)
      v.f_[i] |= vtmp_.f_[i];

    break;
  }
  default:
    throw(AipsError("Jones matrix apply (J::fR) incompatible with VisVector."));
  }
}
// Corr-dep flag leftward onto a VisVector
void Jones::flagLeft(VisVector& v) const {
  switch(v.type()) {
  case VisVector::Four: {

    vtmp_.f_[0]=(v.f_[0]||v.f_[1]||(!ok_[0])||(!ok_[1]));
    vtmp_.f_[1]=(v.f_[0]||v.f_[1]||(!ok_[2])||(!ok_[3]));
    vtmp_.f_[2]=(v.f_[2]||v.f_[3]||(!ok_[0])||(!ok_[1]));
    vtmp_.f_[3]=(v.f_[2]||v.f_[3]||(!ok_[2])||(!ok_[3]));
    
    for (Int i=0;i<4;++i)
      v.f_[i] |= vtmp_.f_[i];

    break;
  }
  default:
    throw(AipsError("Jones matrix apply (J::fR) incompatible with VisVector."));
  }
}


void Jones::zero() {
  ji_=j_;
  for (Int i=0;i<4;++i,++ji_)
    (*ji_)=0.0;
}
  
  
// Constructor
JonesGenLin::JonesGenLin() : Jones() {}

// In place invert
void JonesGenLin::invert() {
  
  if (!ok_) throw(AipsError("Illegal use of JonesGenLin::invert()"));

  // In linear approx, we merely negate the off-diag terms!

  /*
  if (ok_[0]&&ok_[1]) {
    j_[0]*=-1.0;
    j_[1]*=-1.0;
  } else {
    zero();
    ok_[0]=ok_[1]=false;
  }
  */

  if (ok_[0])
    j_[0]*=-1.0;
  else
    j_[0]=0.0;

  if (ok_[1]) 
    j_[1]*=-1.0;
  else
    j_[1]=0.0;

}
  
// Set matrix elements according to ok flag
//   (so we don't have to check ok flags atomically in apply)
void JonesGenLin::setMatByOk() {

  // Not needed if ok_ not set
  if (!ok_) return;

  if (!ok_[0] ||
      !ok_[1]) {
    j_[0]=j_[1]=cOne_;
  }

}


// In-place multipication with another Jones
void JonesGenLin::operator*=(const Jones& other) {

  switch(other.type()) {
  case Jones::General: 
  case Jones::GenLinear:
  case Jones::Diagonal:
  case Jones::Scalar:
  default:
    throw(AipsError("General multiplication NYI"));
    break;
  }
}
  
// Apply rightward to a VisVector
void JonesGenLin::applyRight(VisVector& v) const {
  switch(v.type()) {
  case VisVector::Four: {
    
    // Only adjust cross-hands by pars times parallel hands
    //
    //  [1 X][A B] =  [A    B+XD]
    //  [Y 1][C D]    [YA+C D   ]

    // flag
    if (v.f_) flagRight(v);

    v.v_[1]+=(j_[0]*v.v_[3]);
    v.v_[2]+=(j_[1]*v.v_[0]);

    break;
  }
  default:
    throw(AipsError("JonesGenLin matrix apply (J::aR) incompatible with VisVector."));
  }
}
  
// Apply rightward to a VisVector
void JonesGenLin::applyRight(VisVector& v, Bool& vflag) const {
  
  if (!ok_) throw(AipsError("Illegal use of JonesGenLin::applyRight(v,vflag)"));

  applyFlag(vflag);
  applyRight(v);
  
}

// Apply leftward (conjugate transposed) to a VisVector 
void JonesGenLin::applyLeft(VisVector& v) const {
  switch(v.type()) {
  case VisVector::Four: {
    
    // Only adjust cross-hands by pars times parallel hands
    //
    //  [A B] [1  Y*]  =  [A      B+AY*]
    //  [C D] [X* 1 ]     [C+DX*  D    ]

    // flag
    if (v.f_) flagLeft(v);

    v.v_[1]+=(conj(j_[1])*v.v_[0]);
    v.v_[2]+=(conj(j_[0])*v.v_[3]);
    
    break;
  }
  default:
    throw(AipsError("JonesGenLin matrix apply (J::aL) incompatible with VisVector."));
  }
}

// Apply leftward to a VisVector
void JonesGenLin::applyLeft(VisVector& v, Bool& vflag) const {

  if (!ok_) throw(AipsError("Illegal use of JonesGenLin::applyLeft(v,vflag)"));

  applyFlag(vflag);
  applyLeft(v);

}

// Set flags according to solution flags
//  (non-corr-dep flag version)
void JonesGenLin::applyFlag(Bool& vflag) const {
  vflag|=(!(ok_[0]&&ok_[1]));
}

// Corr-dep flag rightward to a VisVector
void JonesGenLin::flagRight(VisVector& v) const {
  switch(v.type()) {
  case VisVector::Four: {

    v.f_[1] |= ((!ok_[0])||v.f_[3]);
    v.f_[2] |= ((!ok_[1])||v.f_[0]);

    break;
  }
  default:
    throw(AipsError("JonesGenLin matrix apply (JGL::fR) incompatible with VisVector."));
  }
}
// Corr-dep flag leftward to a VisVector 
void JonesGenLin::flagLeft(VisVector& v) const {
  switch(v.type()) {
  case VisVector::Four: {
    
    v.f_[1] |= ((!ok_[1])||v.f_[0]);
    v.f_[2] |= ((!ok_[0])||v.f_[3]);
    
    break;
  }
  default:
    throw(AipsError("JonesGenLin matrix apply (JGL::fL) incompatible with VisVector."));
  }
}

void JonesGenLin::zero() {
  ji_=j_;
  for (Int i=0;i<2;++i,++ji_)
    (*ji_)=0.0;
}


// Constructor
JonesDiag::JonesDiag() : Jones() {}

// In place invert
void JonesDiag::invert() {

  if (!ok_) throw(AipsError("Illegal use of JonesDiag::invert()"));

  ji_=j_;
  oki_=ok_;
  for (Int i=0;i<2;++i,++ji_,++oki_)
    if ((*oki_) && abs(*ji_)>0.0)
      (*ji_)=cOne_/(*ji_);
    else {
      (*ji_)=cZero_;
      (*oki_)=false;
    }

}

// Set matrix elements according to ok flag
//   (so we don't have to check ok flags atomically in apply)
void JonesDiag::setMatByOk() {

  // Not needed if ok_ not set
  if (!ok_) return;

  if (!ok_[0]) j_[0]=cOne_;
  if (!ok_[1]) j_[1]=cOne_;

}


// In-place multipication with another Jones
void JonesDiag::operator*=(const Jones& other) {

  if (!ok_) throw(AipsError("Illegal use of JonesDiag::operator*=()"));

  switch(other.type()) {
  case Jones::General: 
  case Jones::GenLinear: 
    throw(AipsError("Can't multiply Diagonal by General Jones matrix."));
    break;
  case Jones::Diagonal: {
    if (ok_[0]&=other.ok_[0]) j_[0]*=other.j_[0];
    if (ok_[1]&=other.ok_[1]) j_[1]*=other.j_[1];
    break;
  }
  case Jones::Scalar: {
    if (ok_[0]&=other.ok_[0]) j_[0]*=other.j_[0];
    if (ok_[1]&=other.ok_[0]) j_[1]*=other.j_[0];
    break;
  }
  }
}

// Apply rightward to a VisVector
void JonesDiag::applyRight(VisVector& v) const {
  if (v.f_) flagRight(v);
  switch(v.type()) {
  case VisVector::Four: {
    v.v_[0]*=j_[0];
    v.v_[1]*=j_[0];
    v.v_[2]*=j_[1];
    v.v_[3]*=j_[1];
    break;
  }
  case VisVector::Two: {
    v.v_[0]*=j_[0];
    v.v_[1]*=j_[1];
    break;
  }
  case VisVector::One: {
    v.v_[0]*=(*j_);
    break;
  }
  default:
    throw(AipsError("Jones matrix apply (JD::aR) incompatible with VisVector."));
  }    
}

void JonesDiag::applyRight(VisVector& v, Bool& vflag) const {

  if (!ok_) throw(AipsError("Illegal use of JonesDiag::applyRight(v,vflag)"));

  applyFlag(vflag);
  applyRight(v);

}

// Apply leftward (transposed) to a VisVector 
void JonesDiag::applyLeft(VisVector& v) const {
  if (v.f_) flagLeft(v);
  switch(v.type()) {
  case VisVector::Four: {
    Complex c;
    c=conj(j_[0]);
    v.v_[0]*=c;
    v.v_[2]*=c;
    c=conj(j_[1]);
    v.v_[1]*=c;
    v.v_[3]*=c;
    break;
  }
  case VisVector::Two: {
    v.v_[0]*=conj(j_[0]);
    v.v_[1]*=conj(j_[1]);
    break;
  }
  case VisVector::One: {
    v.v_[0]*=conj(*j_);
    break;
  }
  default:
    throw(AipsError("Jones matrix apply (JD::aL) incompatible with VisVector."));
  }    
}

void JonesDiag::applyLeft(VisVector& v, Bool& vflag) const {

  if (!ok_) throw(AipsError("Illegal use of JonesDiag::applyLeft(v,vflag)"));

  applyFlag(vflag);
  applyLeft(v);
}


// Set flags according to solution flags
//  (non-corr-dep flag version)
void JonesDiag::applyFlag(Bool& vflag) const {
  if (scalardata_)
    vflag|=(!ok_[0]);
  else
    vflag|=(!(ok_[0]&&ok_[1]));
}
//  Corr-dep flag right-ward onto a VisVector
void JonesDiag::flagRight(VisVector& v) const {
  switch(v.type()) {
  case VisVector::Four: {
    v.f_[0]|=(!ok_[0]);
    v.f_[1]|=(!ok_[0]);
    v.f_[2]|=(!ok_[1]);
    v.f_[3]|=(!ok_[1]);
    break;
  }
  case VisVector::Two: {
    v.f_[0]|=(!ok_[0]);
    v.f_[1]|=(!ok_[1]);
    break;
  }
  case VisVector::One: {
    v.f_[0]|=(!ok_[0]);
    break;
  }
  default:
    throw(AipsError("Jones matrix apply (JD::aF) incompatible with VisVector."));
  }    
}

//  Corr-dep flag left-ward onto a VisVector
void JonesDiag::flagLeft(VisVector& v) const {
  switch(v.type()) {
  case VisVector::Four: {
    v.f_[0]|=(!ok_[0]);
    v.f_[1]|=(!ok_[1]);
    v.f_[2]|=(!ok_[0]);
    v.f_[3]|=(!ok_[1]);
    break;
  }
  case VisVector::Two: {
    v.f_[0]|=(!ok_[0]);
    v.f_[1]|=(!ok_[1]);
    break;
  }
  case VisVector::One: {
    v.f_[0]|=(!ok_[0]);
    break;
  }
  default:
    throw(AipsError("Jones matrix apply (JD::aF) incompatible with VisVector."));
  }    
}



void JonesDiag::zero() {
  ji_=j_;
  for (Int i=0;i<2;++i,++ji_)
    (*ji_)=0.0;
}


// Constructor
JonesScal::JonesScal() : JonesDiag() {}

// In place invert
void JonesScal::invert() {
  
  if (!ok_) throw(AipsError("Illegal use of JonesScal::invert()"));

  if ((*ok_) && abs(*j_)>0.0)
    (*j_)=cOne_/(*j_);
  else {
    (*j_)=cZero_;
    (*ok_)=false;
  }

}


// Set matrix elements according to ok flag
//   (so we don't have to check ok flags atomically in apply)
void JonesScal::setMatByOk() {

  // Not needed if ok_ not set
  if (!ok_) return;

  if (!ok_[0]) j_[0]=cOne_;

}


// In-place multipication with another Jones
void JonesScal::operator*=(const Jones& other) {

  if (!ok_) throw(AipsError("Illegal use of JonesScal::operator*="));

  switch(other.type()) {
  case Jones::General: 
  case Jones::GenLinear: 
    throw(AipsError("Can't multiply Scalar Jones by General Jones matrix."));
    break;
  case Jones::Diagonal:
    throw(AipsError("Can't multiply Scalar Jones by Diagonal Jones matrix."));
    break;
  case Jones::Scalar: {
    if ((*ok_)&=(*other.ok_)) (*j_)*=(*other.j_);
    break;
  }
  }
}


// Apply rightward to a VisVector
void JonesScal::applyRight(VisVector& v) const {
  if (v.f_) flagRight(v);
  for (Int i=0;i<v.vistype_;++i) 
    v.v_[i] *= (*j_);
}

void JonesScal::applyRight(VisVector& v, Bool& vflag) const {

  if (!ok_) throw(AipsError("Illegal use of JonesScal::applyRight(v,vflag)"));

  applyFlag(vflag);
  applyRight(v);
}

// Apply leftward (transposed) to a VisVector 
void JonesScal::applyLeft(VisVector& v) const {
  if (v.f_) flagLeft(v);
  Complex c;
  c=conj(*j_);
  for (Int i=0;i<v.vistype_;++i) 
    v.v_[i] *= c;
}

void JonesScal::applyLeft(VisVector& v, Bool& vflag) const {

  if (!ok_) throw(AipsError("Illegal use of JonesScal::applyLeft(v,vflag)"));

  applyFlag(vflag);
  applyLeft(v);

}

// Set flags according to solution flags
//  (non-corr-dep flag version)
void JonesScal::applyFlag(Bool& vflag) const {
  vflag|=(!*ok_);
}
//  Corr-dep flag rightward onto a VisVector
//  (NB: flagLeft call this since flagging commutes for scalars)
void JonesScal::flagRight(VisVector& v) const {
  for (Int i=0;i<v.vistype_;++i) v.f_[i]|=(!*ok_);
}


void JonesScal::zero() {
    (*j_)=0.0;
}


// GLOBALS---------------------------


Jones* createJones(const Jones::JonesType& jtype) {
  Jones *j(NULL);
  switch (jtype) {
  case Jones::General:
    j = new Jones();
    break;
  case Jones::GenLinear:
    j = new JonesGenLin();
    break;
  case Jones::Diagonal:
    j = new JonesDiag();
    break;
  case Jones::Scalar:
    j = new JonesScal();
    break;
  }
  return j;
}

void apply(const Jones& j1, VisVector& v, const Jones& j2) {

  // Apply Jones matrix from right, then left
  j1.applyRight(v);
  j2.applyLeft(v);

}

void apply(const Jones& j1, VisVector& v, const Jones& j2, Bool& vflag) {

  // Apply Jones matrix from right, then left
  j1.applyRight(v,vflag);
  j2.applyLeft(v,vflag);

}

// print out a Jones matrix (according to type)
ostream& operator<<(ostream& os, const Jones& mat) {

  Complex *ji;
  ji=mat.j_;

  switch (mat.type()) {
  case Jones::General:
    cout << "General Jones: " << endl;
    for (Int i=0;i<2;++i) {
      cout << " [" << *ji++;
      cout << ", " << *ji++ << "]";
      if (i<1) cout << endl;
    }
    break;
  case Jones::GenLinear:
    cout << "GenLinear Jones: (off diag) " << endl;
    cout << " [" << *ji++;
    cout << ", " << *ji << "]";
    break;
  case Jones::Diagonal:
    cout << "Diagonal Jones: " << endl;
    cout << " [" << *ji++;
    cout << ", " << *ji << "]";
    break;
  case Jones::Scalar:
    cout << "Scalar Jones: " << endl;
    cout << "[" << *ji << "]";
    break;
  }

  return os;
}

// Return the enum for from an int
Jones::JonesType jonesType(const Int& n) {
  switch (n) {
  case 1:
    return Jones::Scalar;
  case 2:
    return Jones::Diagonal;
  case 4:
    return Jones::General;
  default:
    throw(AipsError("Bad JonesType."));

  }

}

} //# NAMESPACE CASA - END
