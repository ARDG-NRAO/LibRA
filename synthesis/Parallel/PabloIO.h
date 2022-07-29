//# PabloIO.h: this defines PabloIO which inits and terminates Pablo
//# Copyright (C) 1999,2000
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

#ifndef SYNTHESIS_PABLOIO_H
#define SYNTHESIS_PABLOIO_H

# if defined(PABLO_IO)

//# Forward Declarations

// <summary>
// Container for hiding Pablo calls.
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>

// <prerequisite>
//   <li> SomeClass
//   <li> SomeOtherClass
//   <li> Pablo
// </prerequisite>
//
// <etymology>
// </etymology>
//
// <synopsis>
// Hides the initialization and termination of the Pablo IO statistics collection
// package
// </synopsis>
//
// <example>
// PabloIO::init(argc, argv);
// ...
// PabloIO::terminate();
// </example>
//
// <motivation>
// Makes it relatively easy to instrument clients or parts of clients using Pablo
// </motivation>
//
// <thrown>
//    <li> Nothing
// </thrown>
//
// <todo asof="2000/01/27">
//   <li> Add some MPI awareness?
//   <li> better documentation?
// </todo>

#include <casacore/casa/aips.h>

namespace casa { //# NAMESPACE CASA - BEGIN

class PabloIO
{
public:
   static void init(casacore::Int, casacore::Char **, casacore::Int tracenode = 0);
   static void terminate();

};


} //# NAMESPACE CASA - END

# endif

#endif
