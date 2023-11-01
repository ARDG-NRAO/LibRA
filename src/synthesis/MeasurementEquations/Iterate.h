//# Iterate.h:
//# Copyright (C) 1996,2000,2001
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
//# $Id$

#ifndef SYNTHESIS_ITERATE_H
#define SYNTHESIS_ITERATE_H

#include <casacore/casa/aips.h>
#include <casacore/casa/BasicSL/String.h>
#include <casacore/casa/Utilities/Fallible.h>


namespace casa { //# NAMESPACE CASA - BEGIN

// Base class for Iteration
class Iterate {
public:
  // Constructor
  Iterate() : numberIterations_(100), gain_(1.0),
       tolerance_(0.000001), threshold_(0.0), free_(false), mode_("") {};

  virtual ~Iterate() {};

  // Is this a free variable?
  void setFree()
    {free_=true;};
  void setNotFree()
    {free_=false;};
  casacore::Bool free()
    {return(free_);};

  void setNumberIterations(const casacore::Int n) {numberIterations_=n;};
  void setGain(const casacore::Float g) {gain_=g;};
  void setTolerance(const casacore::Float t) {tolerance_=t;};
  void setThreshold(const casacore::Float t) {threshold_=t;};
  void setMode(const casacore::String m) {mode_=m;};

  casacore::Int numberIterations() {return numberIterations_;};
  casacore::Float gain() {return gain_;};
  casacore::Float tolerance() {return tolerance_;};
  virtual casacore::Float threshold() {return threshold_;};
  const casacore::String mode() {return mode_;};

private:    
  casacore::Int numberIterations_;
  casacore::Double gain_;
  casacore::Double tolerance_;
  casacore::Double threshold_;
  casacore::Bool free_;
  casacore::String mode_;
};


} //# NAMESPACE CASA - END

#endif
