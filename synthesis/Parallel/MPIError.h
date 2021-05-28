//# MPIError.h: casacore::Data manager error classes
//# Copyright (C) 1994,1995,1996,1999,2000
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
//# $Id$

#ifndef SYNTHESIS_MPIERROR_H
#define SYNTHESIS_MPIERROR_H

//# Includes
#include <casa/aips.h>
#include <casa/Exceptions/Error.h>

namespace casa { //# NAMESPACE CASA - BEGIN

//# This header file defines the error classes used in the
//# MPI transport


// <summary>
// Base error class for MPI
// </summary>
// <use visibility=export>
// </reviewed>

// <synopsis>
// This is the generic MPI exception; catching this one means
// catching all MPI* exceptions.
// Note that you have to catch casacore::AipsError to catch all possible exceptions.
// </synopsis>

class MPIError : public casacore::AipsError {
public:
    // The default constructor generates the message "MPI error".
    MPIError (Category c=GENERAL);
    // Construct with given message.
    MPIError (const casacore::String& message,Category c=GENERAL);
    ~MPIError () throw();
};


} //# NAMESPACE CASA - END

#endif
