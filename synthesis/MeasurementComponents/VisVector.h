//# VisVector.h: Definition of VisVector
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

#ifndef SYNTHESIS_VISVECTOR_H
#define SYNTHESIS_VISVECTOR_H

#include <casa/aips.h>
#include <casa/BasicSL/Complex.h>
#include <casa/Arrays/Cube.h>
#include <casa/iostream.h>
#include <casa/Exceptions/Error.h>

namespace casa { //# NAMESPACE CASA - BEGIN

class VisVector {
  
public:
  
  enum VisType{One=1, Two=2, Four=4};
  
  // Construct from length 
  VisVector(const VisType& len, const casacore::Bool& owner=false);

  // Dtor
  ~VisVector();

  // Assignment (data copy)
  inline VisVector& operator=(const VisVector& vv) {
    for (casacore::Int i=0;i<vistype_;i++) {
      v_[i]=vv.v_[i];
      if (f0_ && vv.f0_) f_[i]=vv.f_[i];
    }
    return *this;
  };

  // Set type id:
  void setType(const VisVector::VisType& type);

  // Return type id
  inline VisType& type() { return vistype_; };
  
  // Reassign origin
  inline void sync(casacore::Complex& vis) { 
    if (!owner_)  {v0_=&vis; f0_=NULL; origin();}
    else          {throw(casacore::AipsError("Illegal VisVector sync")); }
  };
  
  // Reassign origin
  inline void sync(casacore::Complex& vis, casacore::Bool& flag) { 
    if (!owner_)  {v0_=&vis; f0_=&flag; origin();}
    else          {throw(casacore::AipsError("Illegal VisVector sync")); }
  };
  

  // Go to origin
  inline void origin() {v_=v0_;f_=f0_;};
  
  // Increment to next vector
  //  (use function pointers in ctor to handle owner_ case?)
  inline void operator++() { 
    if (!owner_)  {v_+=vistype_; if (f0_) f_+=vistype_;}
    else          throw(casacore::AipsError("Illegal VisVector ++")); 
  };
  inline void operator++(int) { 
    if (!owner_)  {v_+=vistype_; if (f0_) f_+=vistype_;}
    else          throw(casacore::AipsError("Illegal VisVector ++")); 
  };

  // Advance step vectors forward
  inline void advance(const casacore::Int& step) { 
    if (!owner_)  {v_+=(step*vistype_); if (f0_) f_+=(step*vistype_);}
    else          throw(casacore::AipsError("Illegal VisVector advance")); 
  };

  // Re-order elements
  void polznMap();
  void polznUnMap();

  inline void zero() { 
    for (casacore::Int i=0;i<vistype_;i++) {v_[i]=casacore::Complex(0.0);if (f0_) f_[i]=true;} 
  };

  // Print it out
  friend std::ostream& operator<<(std::ostream& os, const VisVector& vec);

  // Give access to Mueller,Jones classes for application
  friend class Mueller;
  friend class MuellerDiag;
  friend class MuellerDiag2;
  friend class AddMuellerDiag;
  friend class AddMuellerDiag2;
  friend class MuellerScal;
  friend class Jones;
  friend class JonesGenLin;
  friend class JonesDiag;
  friend class JonesScal;


  
private:

  // Default ctor private to avoid use
  VisVector() {};

  // VisVector length (4, 2, or 1)
  VisType vistype_;
  
  // Does the VisVector own the storage, or are
  //  we pointing to something external
  casacore::Bool owner_;

  // Pointer to origin
  casacore::Complex *v0_;
  casacore::Bool *f0_;

  // Moving pointer
  casacore::Complex *v_;
  casacore::Bool *f_;

  
};

// Globals:

// Return VisType according to length of data array corr axis
VisVector::VisType visType(const casacore::Int& ncorr);


} //# NAMESPACE CASA - END

#endif



