//# CalHistoryRecord.h: Cal_history table record access and creation
//# Copyright (C) 1996,1997,1998
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
//# $Id: 

#ifndef CALIBRATION_CALHISTRECORD_H
#define CALIBRATION_CALHISTRECORD_H

#include <casa/aips.h>
#include <casa/Containers/Record.h>

namespace casa { //# NAMESPACE CASA - BEGIN

// <summary> 
// CalHistoryRecord: Cal_ table record access and creation
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//   <li> <linkto class="casacore::Record">casacore::Record</linkto> module
//   <li> <linkto class="CalMainRecord">CalMainRecord</linkto> module
// </prerequisite>
//
// <etymology>
// From "calibration history sub-table" and "record".
// </etymology>
//
// <synopsis>
// The CalHistoryRecord class allows access to, and creation of, records
// in the cal_history calibration sub-table. 
// </etymology>
//
// <example>
// <srcblock>
// </srcblock>
// </example>
//
// <motivation>
// Encapsulate access to cal_history calibration table records.
// </motivation>
//
// <todo asof="98/01/01">
// </todo>

class CalHistoryRecord
{
 public:
   // Default null constructor, and destructor
   CalHistoryRecord();
   ~CalHistoryRecord() {};

   // Construct from an existing record
   CalHistoryRecord (const casacore::Record& inpRec);

   // Return as record
   const casacore::Record& record();

   // Field accessors
   // a) define
   void defineCalParms (const casacore::String& calParms);
   void defineCalTables (const casacore::String& calTables);
   void defineCalSelect (const casacore::String& calSelect);
   void defineCalNotes (const casacore::String& calNotes);
     
   // b) get
   void getCalParms (casacore::String& calParms);
   void getCalTables (casacore::String& calTables);
   void getCalSelect (casacore::String& calSelect);
   void getCalNotes (casacore::String& calNotes);

 protected:
   // Add to itsRecord
   void addRec (const casacore::Record& newRec);

 private:
   casacore::Record itsRecord;

};


} //# NAMESPACE CASA - END

#endif
   
