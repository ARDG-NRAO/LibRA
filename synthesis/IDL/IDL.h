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

#ifndef SYNTHESIS_IDL_H
#define SYNTHESIS_IDL_H

#include <casacore/casa/Arrays/Vector.h>
#include <casacore/casa/BasicSL/String.h>

#ifdef HAVE_IDL_LIB
#include <stdio.h>
#include "export.h"
#endif

namespace casa { //# NAMESPACE CASA - BEGIN

// <summary> A class to start IDL and execute commands, etc.
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//   <li> IDL libraries
// </prerequisite>
//
// <etymology>
// IDL is the class for running IDL from inside AIPS++
// </etymology>
//
// <synopsis> 
// We can start IDL, execute commands, send and retreive named
// arrays, etc using this class. The initial use of this class
// is to allow Pixon processing to be performed using the IDL
// code.
// </synopsis> 
//
// <example>
// </example>
//
// <motivation>
// Get access to IDL functionality
// </motivation>
//
// <todo asof="01/03/03">
// <ul> Add more interface functions
// <ul> Turn into DO
// </todo>

class IDL  {
public:

  // The constructor will start IDL.
  IDL(casacore::Int options=0);

  // Copy constructor
  IDL(const IDL &other);

  // Assignment operator
  IDL &operator=(const IDL &other);

  // The destructor will stop IDL.
  ~IDL();

  // Run a sequence of IDL commands
  casacore::Bool runCommands(const casacore::Vector<casacore::String>& commands, casacore::Bool log=true);

  // Run a single IDL command
  casacore::Bool runCommand(const casacore::String& command, casacore::Bool log=true);

  // Send an array to IDL and name it
  casacore::Bool sendArray(const casacore::Array<casacore::Float>& a, casacore::String aname);

  // Get an array from IDL
  casacore::Array<casacore::Float> getArray(casacore::String aname);

  // Get an scalar from IDL
  casacore::Float getFloat(casacore::String aname);

  // Get an scalar from IDL
  casacore::Int getInt(casacore::String aname);

  // Set the path inside IDL
  casacore::Bool setPath(const casacore::Vector<casacore::String>&);

protected:

  char* getIDLName(const casacore::String);

#ifdef HAVE_IDL_LIB
  IDL_VPTR getPointer(const casacore::String);
#endif

  void ok();

};

} //# NAMESPACE CASA - END

#endif
