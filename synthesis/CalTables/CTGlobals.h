//# CTGlobals.h: Declaration of CTGlobals
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

#ifndef SYNTHESIS_CTGLOBALS_H
#define SYNTHESIS_CTGLOBALS_H

#include <casa/aips.h>
#include <synthesis/CalTables/NewCalTable.h>
#include <casa/Arrays/Vector.h>

namespace casa { //# NAMESPACE CASA - BEGIN

// Create a specialized VisCal from VisSet
void smoothCT(NewCalTable ct,
	      const casacore::String& smtype,
	      const casacore::Double& smtime,
	      casacore::Vector<casacore::Int> selfields);

// Extract time-dep scan and field id info from the specified casacore::MS,
//   and assign them to the NewCalTable
void assignCTScanField(NewCalTable& ct, casacore::String msName, 
		       casacore::Bool doField=true, 
		       casacore::Bool doScan=true,
		       casacore::Bool doObs=true);

} //# NAMESPACE CASA - END

#endif

