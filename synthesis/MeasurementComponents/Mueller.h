//# Mueller.h: Definition of Mueller
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

#ifndef SYNTHESIS_MUELLER_H
#define SYNTHESIS_MUELLER_H

#include <casa/aips.h>
#include <casa/BasicSL/Complex.h>
#include <casa/iostream.h>
#include <casa/Exceptions/Error.h>
#include <synthesis/MeasurementComponents/VisVector.h>
#include <synthesis/MeasurementComponents/Jones.h>

namespace casa { //# NAMESPACE CASA - BEGIN

class Mueller {
  
public:
  
  enum MuellerType{AddDiag2=6,AddDiag=5,General=4,Diagonal=3,Diag2=2,Scalar=1};
 
  // Construct 
  Mueller();
  
  // Dtor
  virtual ~Mueller() {};
  
  // Return type id
  inline virtual MuellerType type() const { return Mueller::General; };
  inline virtual casacore::uInt typesize() const { return 16; };

  // Set scalardata_ 
  //  TBD: Handle this better; for now, we need to set this from
  //       an external call so we handle single-corr data properly
  //       when setting non-corr-dep flags
  inline void setScalarData(casacore::Bool scalardata) const { scalardata_=scalardata; };
  
  // Synchronize with leading element in external array
  inline void sync(casacore::Complex& mat) { m0_=&mat; origin(); };
  inline void sync(casacore::Complex& mat, casacore::Bool& ok) { m0_=&mat; ok0_=&ok; origin(); };
  
  // Reset to origin
  inline void origin() {m_=m0_;ok_=ok0_;};
  
  // Increment to next vector (according to len)
  inline void operator++()    { m_+=typesize(); if (ok_) ok_+=typesize();};
  inline void operator++(int) { m_+=typesize(); if (ok_) ok_+=typesize();};

  // Advance step matrices forward (according to len)
  inline void advance(const casacore::Int& step) { m_+=(step*typesize()); if (ok_) ok_+=(step*typesize());};

  // Formation from Jones matrix outer product: General version
  virtual void fromJones(const Jones& jones1, const Jones& jones2);

  // In-place invert
  virtual void invert();

  // Set matrix elements according to ok flag
  //  (so we don't have to check ok flags atomically in apply)
  virtual void setMatByOk();

  // In-place multiply onto a VisVector: General version
  virtual void apply(VisVector& v);
  virtual void apply(VisVector& v, casacore::Bool& vflag);

  // Apply only flags according to cal flags
  virtual void applyFlag(casacore::Bool& vflag);
  virtual void flag(VisVector& v);

  // Multiply onto a vis VisVector, preserving input (copy then in-place apply)
  virtual void apply(VisVector& out, const VisVector& in);

  // print it out
  friend std::ostream& operator<<(std::ostream& os, const Mueller& mat);
    
protected:
  
  // Copy ctor protected 
  Mueller(const Mueller& mat);

  // Pointer to origin
  casacore::Complex *m0_;
  casacore::Bool *ok0_;

  // Moving pointer
  casacore::Complex *m_, *mi_;
  casacore::Bool *ok_, *oki_;

  // casacore::Complex unity, zero (for use in invert and similar methods)
  const casacore::Complex cOne_,cZero_;

  mutable casacore::Bool scalardata_;

private: 

  // Zero the whole Mueller
  virtual void zero();

  // VisVector temporary (only relevant for Mueller::General)
  VisVector vtmp_;


  
};

class MuellerDiag : public Mueller {

public:
  
  // Construct 
  MuellerDiag();
  
  // Dtor
  virtual ~MuellerDiag() {};
  
  // Return type id
  inline virtual MuellerType type() const { return Mueller::Diagonal; };
  inline virtual casacore::uInt typesize() const { return 4; };

  // Formation from Jones matrix outer product: optimized Diagonal version
  virtual void fromJones(const Jones& jones1, const Jones& jones2);
  
  // In-place invert
  virtual void invert();

  // Set matrix elements according to ok flag
  //  (so we don't have to check ok flags atomically in apply)
  virtual void setMatByOk();

  // In-place multiply onto a VisVector: optimized Diagonal version
  virtual void apply(VisVector& v);
  virtual void apply(VisVector& v, casacore::Bool& vflag);
  using Mueller::apply;

  // Apply only flags according to cal flags
  virtual void applyFlag(casacore::Bool& vflag);
  virtual void flag(VisVector& v);

protected:
  
  // Default/Copy ctors are protected 
  MuellerDiag(const MuellerDiag& mat);

private: 

  // Zero the whole Mueller
  virtual void zero();
  
};

class MuellerDiag2 : public MuellerDiag {
  
public:

  // Construct 
  MuellerDiag2();

  // Dtor
  virtual ~MuellerDiag2() {};
  
  // Return type id
  inline virtual MuellerType type() const { return Mueller::Diag2; };
  inline virtual casacore::uInt typesize() const { return 2; };

  // Formation from Jones matrix outer product: optimized Diag2 version
  virtual void fromJones(const Jones& jones1, const Jones& jones2);
  
  // In-place invert
  virtual void invert();

  // Set matrix elements according to ok flag
  //  (so we don't have to check ok flags atomically in apply)
  virtual void setMatByOk();

  // In-place multiply onto a VisVector: optimized Diag2 version
  virtual void apply(VisVector& v);
  virtual void apply(VisVector& v, casacore::Bool& vflag);
  using MuellerDiag::apply;

  // Apply only flags according to cal flags
  virtual void applyFlag(casacore::Bool& vflag);
  virtual void flag(VisVector& v);

protected:
  
  // Default/Copy ctors are protected 
  MuellerDiag2(const MuellerDiag2& mat);
  
private: 

  // Zero the whole Mueller
  virtual void zero();

};


class MuellerScal : public MuellerDiag {
 
public:

  // Construct 
  MuellerScal();

  // Dtor
  virtual ~MuellerScal() {};

  // Return type id
  inline virtual MuellerType type() const { return Mueller::Scalar; }
  inline virtual casacore::uInt typesize() const { return 1; };

  // Formation from Jones matrix outer product: optimized Scalar version
  virtual void fromJones(const Jones& jones1, const Jones& jones2);

  // In-place invert
  virtual void invert();

  // Set matrix elements according to ok flag
  //  (so we don't have to check ok flags atomically in apply)
  virtual void setMatByOk();

  // In-place multiply onto a VisVector: optimized Scalar version
  virtual void apply(VisVector& v);
  virtual void apply(VisVector& v, casacore::Bool& vflag);
  using MuellerDiag::apply;

  // Apply only flags according to cal flags
  virtual void applyFlag(casacore::Bool& vflag);
  virtual void flag(VisVector& v);

protected:
  
  // Default/Copy ctors are protected 
  MuellerScal(const MuellerScal& mat);

private: 

  // Zero the whole Mueller
  virtual void zero();

};


// Parallel-hands only "additive Mueller"
class AddMuellerDiag2 : public MuellerDiag2 {
  
public:

  // Construct 
  AddMuellerDiag2();

  // Dtor
  virtual ~AddMuellerDiag2() {};
  
  // Return type id
  inline virtual MuellerType type() const { return Mueller::AddDiag2; };

  // In-place invert (negate)
  virtual void invert();

  // Set matrix elements according to ok flag
  //  (so we don't have to check ok flags atomically in apply)
  virtual void setMatByOk();

  // In-place add onto a VisVector: optimized Diag2 version
  virtual void apply(VisVector& v);
  using MuellerDiag2::apply;

protected:
  
  // Default/Copy ctors are protected 
  AddMuellerDiag2(const AddMuellerDiag2& mat);
  
};


// Full polarization "additive Mueller"
class AddMuellerDiag : public MuellerDiag {
  
public:

  // Construct 
  AddMuellerDiag();

  // Dtor
  virtual ~AddMuellerDiag() {};
  
  // Return type id
  inline virtual MuellerType type() const { return Mueller::AddDiag; };

  // In-place invert (negate)
  virtual void invert();

  // Set matrix elements according to ok flag
  //  (so we don't have to check ok flags atomically in apply)
  virtual void setMatByOk();

  // In-place add onto a VisVector:
  virtual void apply(VisVector& v);
  using MuellerDiag::apply;

protected:
  
  // Default/Copy ctors are protected 
  AddMuellerDiag(const AddMuellerDiag& mat);
  
};




// Globals

// Factory method
Mueller* createMueller(const Mueller::MuellerType& mtype);

// Return Mueller type according to Int
//Mueller::MuellerType muellerType(const casacore::Int& n);

// Return parameter count according to type
inline casacore::Int muellerNPar(const Mueller::MuellerType& mtype) {
  switch (mtype) {
  case Mueller::General:
    return 16;
    break;
  case Mueller::Diagonal:
  case Mueller::AddDiag:
    return 4;
    break;
  case Mueller::Diag2:
  case Mueller::AddDiag2:
    return 2;
    break;
  case Mueller::Scalar:
    return 1;
    break;
  }
  // must return something (shouldn't reach here)
  return 0;
}
    

// Return Mueller type according to underlying Jones and VisVector types
Mueller::MuellerType muellerType(const Jones::JonesType& jtype, const VisVector::VisType& vtype);

} //# NAMESPACE CASA - END

#endif

