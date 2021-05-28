//# Pixon.h: Definition for Pixon
//# Copyright (C) 1996,1997,1998,1999,2000,2001
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

#ifndef SYNTHESIS_PIXONPROCESSOR_H
#define SYNTHESIS_PIXONPROCESSOR_H

#include <synthesis/IDL/IDL.h>
#include <synthesis/DataSampling/DataSampling.h>

namespace casa { //# NAMESPACE CASA - BEGIN

// <summary> Allows processing via the Pixon algorithm
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//   <li> <linkto class=IDL>IDL</linkto> class
//   <li> <linkto class=DataSampling>DataSampling</linkto> class
// </prerequisite>
//
// <etymology>
// Performs processing via the Pixon algorithm
// </etymology>
//
// <synopsis> 
// </synopsis> 
//
// <example>
// </example>
//
// <motivation>
// Allow pixon processing in AIPS++
// </motivation>
//
// <todo asof="01/03/03">
// <ul> None
// </todo>

class PixonProcessor  {
public:

  PixonProcessor();

  PixonProcessor(casacore::ImageInterface<casacore::Float>& model);

  // Copy constructor
  PixonProcessor(const PixonProcessor &other);

  // Assignment operator
  PixonProcessor &operator=(const PixonProcessor &other);

  virtual ~PixonProcessor();

  casacore::Bool calculate(const DataSampling& dsm, casacore::Array<casacore::Float>& a);
  
  casacore::Bool save(const DataSampling& ds, const casacore::String& saveName);
  
  casacore::Bool standardTest();

protected:

  casacore::String IDLPixonHome;

  casacore::String IDLPublicHome;

  IDL idl;

  casacore::ImageInterface<casacore::Float>* model_p;

  void ok();

};

} //# NAMESPACE CASA - END

#endif
