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

#ifndef SYNTHESIS_JONES_H
#define SYNTHESIS_JONES_H

#include <casacore/casa/aips.h>
#include <casacore/casa/BasicSL/Complex.h>
#include <iostream>
#include <casacore/casa/Exceptions/Error.h>
//#include <synthesis/MeasurementComponents/Mueller.h>
#include <synthesis/MeasurementComponents/VisVector.h>

namespace casa { //# NAMESPACE CASA - BEGIN

class Jones {
  
public:
  
  enum JonesType{General=4,GenLinear=3,Diagonal=2,Scalar=1};

  // Construct 
  Jones();
  
  // Dtor
  virtual ~Jones() {};
  
  // Return type id
  inline virtual JonesType type() const { return Jones::General; };
  inline virtual casacore::Int typesize() const { return 4; };

  // Set scalardata_ 
  //  TBD: Handle this better; for now, we need to set this from
  //       an external call so we handle single-corr data properly
  //       when setting non-corr-dep flags
  inline void setScalarData(casacore::Bool scalardata) const { scalardata_=scalardata; };
  
  // Synchronize with leading element in external array
  inline void sync(casacore::Complex& mat) { j0_=&mat; origin(); };
  inline void sync(casacore::Complex& mat, casacore::Bool& ok) { j0_=&mat; ok0_=&ok; origin(); };
  
  // Reset to origin
  inline void origin() {j_=j0_; ok_=ok0_;};
  
  // Increment to next matrix (according to type)
  inline void operator++()    { j_+=typesize(); if (ok_) ok_+=typesize();};
  inline void operator++(int) { j_+=typesize(); if (ok_) ok_+=typesize();};

  // Advance step matrices forward (according to typesize)
  inline void advance(const casacore::Int& step) { j_+=(step*typesize()); if (ok_) ok_+=(step*typesize());};

  // In-place invert
  virtual void invert();

  // Set matrix elements according to ok flag
  //   (so we don't have to check ok flags atomically in apply)
  virtual void setMatByOk();

  // In-place multipication with another Jones
  virtual void operator*=(const Jones& other);

  // Apply rightward to a VisVector
  virtual void applyRight(VisVector& v) const;
  virtual void applyRight(VisVector& v, casacore::Bool& vflag) const;

  // Apply leftward (transposed) to a VisVector 
  virtual void applyLeft(VisVector& v) const;
  virtual void applyLeft(VisVector& v, casacore::Bool& vflag) const;

  // Set flags according to solution flags
  virtual void applyFlag(casacore::Bool& vflag) const;
  virtual void flagRight(VisVector& v) const;
  virtual void flagLeft(VisVector& v) const;

  // print it out
  friend std::ostream& operator<<(std::ostream& os, const Jones& mat);

  // Give access to Mueller formation method
  friend class Mueller;
  friend class MuellerDiag;
  friend class MuellerDiag2;
  friend class MuellerScal;
    
  friend class JonesDiag;
  friend class JonesScal;

protected:
  
  // Copy ctor protected 
  Jones(const Jones& mat);

  // Pointer to origin
  casacore::Complex *j0_;
  casacore::Bool *ok0_;
  
  // Moving pointer
  casacore::Complex *j_, *ji_;
  casacore::Bool *ok_, *oki_;

  // casacore::Complex unity, zero
  const casacore::Complex cOne_,cZero_;

  // Is data scalar?
  mutable casacore::Bool scalardata_;

private:

  // Zero the Jones matrix
  virtual void zero();

  // Temporary VisVector
  VisVector vtmp_;
  
};


class JonesGenLin : public Jones {
  
public:
  
  // Construct 
  JonesGenLin();
  
  // Dtor
  virtual ~JonesGenLin() {};
  
  // Return type id
  inline virtual JonesType type() const { return Jones::GenLinear; };
  inline virtual casacore::Int typesize() const { return 2; };

  // In-place invert
  virtual void invert();

  // Set matrix elements according to ok flag
  //   (so we don't have to check ok flags atomically in apply)
  virtual void setMatByOk();

  // In-place multipication with another Jones
  virtual void operator*=(const Jones& other);

  // Apply rightward to a VisVector
  virtual void applyRight(VisVector& v) const;
  virtual void applyRight(VisVector& v, casacore::Bool& vflag) const;

  // Apply leftward (transposed) to a VisVector
  virtual void applyLeft(VisVector& v) const;
  virtual void applyLeft(VisVector& v, casacore::Bool& vflag) const;

  // Set flags according to solution flags
  virtual void applyFlag(casacore::Bool& vflag) const;
  virtual void flagRight(VisVector& v) const;
  virtual void flagLeft(VisVector& v) const;

  // Give access to Mueller formation methods
  friend class MuellerDiag;
  friend class MuellerDiag2;
    
protected:
  
  // Copy ctor protected 
  JonesGenLin(const JonesGenLin& mat);

private:

  // Zero the Jones matrix
  virtual void zero();

};


class JonesDiag : public Jones {
  
public:
  
  // Construct 
  JonesDiag();
  
  // Dtor
  virtual ~JonesDiag() {};
  
  // Return type id
  inline virtual JonesType type() const { return Jones::Diagonal; };
  inline virtual casacore::Int typesize() const { return 2; };

  // In-place invert
  virtual void invert();

  // Set matrix elements according to ok flag
  //   (so we don't have to check ok flags atomically in apply)
  virtual void setMatByOk();

  // In-place multipication with another Jones
  virtual void operator*=(const Jones& other);

  // Apply rightward to a VisVector
  virtual void applyRight(VisVector& v) const;
  virtual void applyRight(VisVector& v, casacore::Bool& vflag) const;

  // Apply leftward (transposed) to a VisVector
  virtual void applyLeft(VisVector& v) const;
  virtual void applyLeft(VisVector& v, casacore::Bool& vflag) const;

  // Set flags according to solution flags
  virtual void applyFlag(casacore::Bool& vflag) const;
  virtual void flagRight(VisVector& v) const;
  virtual void flagLeft(VisVector& v) const;

  // Give access to Mueller formation methods
  friend class MuellerDiag;
  friend class MuellerDiag2;
    
protected:
  
  // Copy ctor protected 
  JonesDiag(const JonesDiag& mat);

private:

  // Zero the Jones matrix
  virtual void zero();

};


class JonesScal : public JonesDiag {
  
public:
  
  // Construct 
  JonesScal();
  
  // Dtor
  virtual ~JonesScal() {};
  
  // Return type id
  inline virtual JonesType type() const { return Jones::Scalar; };
  inline virtual casacore::Int typesize() const { return 1; };

  // In-place invert
  virtual void invert();

  // Set matrix elements according to ok flag
  //   (so we don't have to check ok flags atomically in apply)
  virtual void setMatByOk();

  // In-place multipication with another Jones
  virtual void operator*=(const Jones& other);

  // Apply rightward to a VisVector
  virtual void applyRight(VisVector& v) const;
  virtual void applyRight(VisVector& v, casacore::Bool& vflag) const;

  // Apply leftward (transposed) to a VisVector
  virtual void applyLeft(VisVector& v) const;
  virtual void applyLeft(VisVector& v, casacore::Bool& vflag) const;

  // Set flags according to solution flags
  virtual void applyFlag(casacore::Bool& vflag) const;
  virtual void flagRight(VisVector& v) const;
  virtual void flagLeft(VisVector& v) const { flagRight(v); };  // flagging commutes

  // Give access to Mueller formation methods
  friend class MuellerScal;
    
protected:
  
  // Copy ctor protected 
  JonesScal(const JonesScal& mat);

private:

  // Zero the Jones matrix
  virtual void zero();

};


// Global methods:

// Factory method for creation of Jones
Jones* createJones(const Jones::JonesType& jtype);

// Apply a pair of Jones to a VisVector:
void apply(const Jones& j1, VisVector& v, const Jones& j2);
void apply(const Jones& j1, VisVector& v, const Jones& j2, casacore::Bool& vflag);

// Return enum from integer
Jones::JonesType jonesType(const casacore::Int& n);

// Return parameter count from 
inline casacore::Int jonesNPar(const Jones::JonesType& jtype) {
  switch (jtype) {
  case Jones::General:
    return 4;
    break;
  case Jones::GenLinear:
  case Jones::Diagonal:
    return 2;
    break;
  case Jones::Scalar:
    return 1;
    break;
  }
  // must return something
  return 0;
}


} //# NAMESPACE CASA - END

#endif


