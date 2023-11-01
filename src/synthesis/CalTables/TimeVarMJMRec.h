//# TimeVarMJMRec.h: TimeVarMJones cal_main table record access and creation
//# Copyright (C) 1996,1997,1998,2003
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

#ifndef CALIBRATION_TIMEVARMJMREC_H
#define CALIBRATION_TIMEVARMJMREC_H

#include <synthesis/CalTables/MJonesMRec.h>

namespace casa { //# NAMESPACE CASA - BEGIN

// <summary> 
// TimeVarMJonesMRec: TimeVarMJones cal_main table record access & creation
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//   <li> <linkto class="CalMainRecord">CalMainRecord</linkto> module
//   <li> <linkto class="MJonesMRec">MJonesMRec</linkto> module
// </prerequisite>
//
// <etymology>
// From "time-variable MJones" and "main record".
// </etymology>
//
// <synopsis>
// The TimeVarMJonesMRec class allows the creation of records for the
// main calibration table of TimeVarMJones type, and provides access
// to the individual record fields. Specializations for solvable
// MJones matrix records are provided through inheritance.
// </etymology>
//
// <example>
// <srcblock>
// </srcblock>
// </example>
//
// <motivation>
// Encapsulate access to TimeVarMJones calibration table records.
// </motivation>
//
// <todo asof="98/01/01">
// (i) Deal with non-standard columns.
// </todo>

class TimeVarMJonesMRec : public MJonesMRec
{
 public:
   // Default null constructor, and destructor
   TimeVarMJonesMRec();
   ~TimeVarMJonesMRec() {};

   // Construct from an existing record
   TimeVarMJonesMRec (const casacore::Record& inpRec);

 };


} //# NAMESPACE CASA - END

#endif
   
  



