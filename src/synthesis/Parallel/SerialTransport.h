//# SerialTransport.h: simple/default transport model implemented as a simple queue
//# Copyright (C) 1998,1999,2000
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
//#
//# $Id$

#ifndef SYNTHESIS_SERIALTRANSPORT_H
#define SYNTHESIS_SERIALTRANSPORT_H

#include <synthesis/Parallel/PTransport.h>

namespace casa { //# NAMESPACE CASA - BEGIN

// <summary>
// Serial casacore::Data Transport Model
// </summary>

// <use visibility=local>

// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>

// <prerequisite>
//   <li> SomeClass
//   <li> SomeOtherClass
//   <li> some concept
// </prerequisite>
//
// <etymology>
// </etymology>
//
// <synopsis>
// </synopsis>
//
// <example>
// </example>
//
// <motivation>
// </motivation>
//
//
//# <todo asof="yyyy/mm/dd">
//#   <li> add this feature
//#   <li> fix this bug
//#   <li> start discussion of this possible extension
//# </todo>

// SerialTransport is your basic no-op.  We're just passing the pointers
// to avoid unnecessary data copying.

class SerialTransport : public PTransport {
 public:
  // Default constructor and destructor
  SerialTransport();
  virtual ~SerialTransport(){}

  // Default source and message tag values
  virtual casacore::Int anyTag() {return -1;};
  virtual casacore::Int anySource() {return -1;};
  
  // Define the rank of the controller process
  virtual casacore::Int controllerRank() {return 0;};

  // Get and put functions on the data transport layer
  virtual casacore::Int put(const casacore::Array<casacore::Float> &);
  virtual casacore::Int put(const casacore::Array<casacore::Double> &);
  virtual casacore::Int put(const casacore::Array<casacore::Complex> &);
  virtual casacore::Int put(const casacore::Array<casacore::DComplex> &);
  virtual casacore::Int put(const casacore::Array<casacore::Int> &);
  virtual casacore::Int put(const casacore::Float &);
  virtual casacore::Int put(const casacore::Double &);
  virtual casacore::Int put(const casacore::Complex &);
  virtual casacore::Int put(const casacore::DComplex &);
  virtual casacore::Int put(const casacore::Int &);
  virtual casacore::Int put(const casacore::String &);
  virtual casacore::Int put(const casacore::Bool &);
  virtual casacore::Int put(const casacore::Record &);

  virtual casacore::Int get(casacore::Array<casacore::Float> &);
  virtual casacore::Int get(casacore::Array<casacore::Double> &);
  virtual casacore::Int get(casacore::Array<casacore::Complex> &);
  virtual casacore::Int get(casacore::Array<casacore::DComplex> &);
  virtual casacore::Int get(casacore::Array<casacore::Int> &);
  virtual casacore::Int get(casacore::Float &);
  virtual casacore::Int get(casacore::Double &);
  virtual casacore::Int get(casacore::Complex &);
  virtual casacore::Int get(casacore::DComplex &);
  virtual casacore::Int get(casacore::Int &);
  virtual casacore::Int get(casacore::String &);
  virtual casacore::Int get(casacore::Bool &);
  virtual casacore::Int get(casacore::Record &);
  ///IsFinalized is just a dummy here as no finalization is needed
  virtual casacore::Bool isFinalized(){ return true; };
 private:
  casacore::uInt inQue;
  casacore::uInt outQue;
  casacore::uInt lastInQue;
  casacore::PtrBlock<void *> _data;
  
  casacore::Int add2Queue(void *);
  void *getFromQueue();
};

} //# NAMESPACE CASA - END

#endif
