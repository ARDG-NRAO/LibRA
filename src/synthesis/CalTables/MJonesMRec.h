//# MJonesMRec.h: MJones cal_main table record access and creation
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

#ifndef CALIBRATION_MJONESMREC_H
#define CALIBRATION_MJONESMREC_H

#include <casacore/casa/aips.h>
#include <synthesis/CalTables/CalMainRecord.h>

namespace casa { //# NAMESPACE CASA - BEGIN

// <summary> 
// MJonesMRec: MJones cal_main table record access and creation
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//   <li> <linkto class="CalMainRecord">CalMainRecord</linkto> module
// </prerequisite>
//
// <etymology>
// From "MJones" and "main record".
// </etymology>
//
// <synopsis>
// The MJonesMRec class allows the creation of records for the
// main calibration table of MJones type, and provides access
// to the individual record fields. Specializations for solvable
// MJones calibration table records are provided through
// inheritance.
// </etymology>
//
// <example>
// <srcblock>
// </srcblock>
// </example>
//
// <motivation>
// Encapsulate access to MJones calibration table records.
// </motivation>
//
// <todo asof="98/01/01">
// (i) Deal with non-standard columns.
// </todo>

class MJonesMRec : public CalMainRecord
{
 public:
   // Default null constructor, and destructor
   MJonesMRec();
   ~MJonesMRec() {};

   // Construct from an existing record
   MJonesMRec (const casacore::Record& inpRec);

   // Field accessors
   // a) define
   void defineAntenna2 (const casacore::Int& antenna2);
   void defineFeed2 (const casacore::Int& feed2);

   // b) get
   void getAntenna2 (casacore::Int& antenna2);
   void getFeed2 (casacore::Int& feed2);

 };


} //# NAMESPACE CASA - END

#endif
   
  



