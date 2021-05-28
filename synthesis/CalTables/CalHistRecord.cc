//# CalHistRecord.cc: Implementation of CalHistRecord.h
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
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//# $Id$
//----------------------------------------------------------------------------

#include <synthesis/CalTables/CalHistRecord.h>
#include <msvis/MSVis/MSCalEnums.h>
#include <casa/Arrays.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

//----------------------------------------------------------------------------

CalHistoryRecord::CalHistoryRecord() : itsRecord()
{
// Null constructor
// Output to private data:
//    itsRecord        Record          Underlying record object
//
};

//----------------------------------------------------------------------------

CalHistoryRecord::CalHistoryRecord (const Record& inpRec) : itsRecord (inpRec)
{
// Construct from an input record
// Input:
//    inpRec           Record          Input record
// Output to private data:
//    itsRecord        Record          Underlying record object
//
};

//----------------------------------------------------------------------------

const Record& CalHistoryRecord::record()
{
// Return as record
// Output:
//    record           const Record&   Return underlying record object
// Input from private data:
//    itsRecord        Record          Underlying record object
//
  return itsRecord;
};

//----------------------------------------------------------------------------

void CalHistoryRecord::addRec (const Record& newRec)
{
// Add to underlying record object
// Input:
//    newRec           Record          Record to be added
// Output to private data:
//    itsRecord        Record          Underlying record object
//
  itsRecord.merge (newRec, RecordInterface::OverwriteDuplicates);
};

//----------------------------------------------------------------------------

void CalHistoryRecord::defineCalParms (const String& calParms)
{
// Define the CAL_PARMS field value
// Input:
//    calParms         const String&         CAL_PARMS value
// Output to private data:
//    itsRecord        Record                Underlying record object
//
  Record newRec;
  newRec.define (MSC::fieldName (MSC::CAL_PARMS), calParms);
  addRec (newRec);
};

//----------------------------------------------------------------------------

void CalHistoryRecord::defineCalTables (const String& calTables)
{
// Define the CAL_TABLES field value
// Input:
//    calTables        const String&         CAL_TABLES value
// Output to private data:
//    itsRecord        Record                Underlying record object
//
  Record newRec;
  newRec.define (MSC::fieldName (MSC::CAL_TABLES), calTables);
  addRec (newRec);
};

//----------------------------------------------------------------------------

void CalHistoryRecord::defineCalSelect (const String& calSelect)
{
// Define the CAL_SELECT field value
// Input:
//    calSelect        const String&         CAL_SELECT value
// Output to private data:
//    itsRecord        Record                Underlying record object
//
  Record newRec;
  newRec.define (MSC::fieldName (MSC::CAL_SELECT), calSelect);
  addRec (newRec);
};

//----------------------------------------------------------------------------

void CalHistoryRecord::defineCalNotes (const String& calNotes)
{
// Define the CAL_NOTES field value
// Input:
//    calNotes         const String&         CAL_NOTES value
// Output to private data:
//    itsRecord        Record                Underlying record object
//
  Record newRec;
  newRec.define (MSC::fieldName (MSC::CAL_NOTES), calNotes);
  addRec (newRec);
};

//----------------------------------------------------------------------------

void CalHistoryRecord::getCalParms (String& calParms)
{
// Get the CAL_PARMS field value
// Output:
//    calParms         String&               CAL_PARMS value
// Input from private data:
//    itsRecord        Record                Underlying record object
//
  record().get (MSC::fieldName (MSC::CAL_PARMS), calParms);
};

//----------------------------------------------------------------------------

void CalHistoryRecord::getCalTables (String& calTables)
{
// Get the CAL_TABLES field value
// Output:
//    calTables        String&               CAL_TABLES value
// Input from private data:
//    itsRecord        Record                Underlying record object
//
  record().get (MSC::fieldName (MSC::CAL_TABLES), calTables);
};

//----------------------------------------------------------------------------

void CalHistoryRecord::getCalSelect (String& calSelect)
{
// Get the CAL_SELECT field value
// Output:
//    calSelect        String&               CAL_SELECT value
// Input from private data:
//    itsRecord        Record                Underlying record object
//
  record().get (MSC::fieldName (MSC::CAL_SELECT), calSelect);
};

//----------------------------------------------------------------------------

void CalHistoryRecord::getCalNotes (String& calNotes)
{
// Get the CAL_NOTES field value
// Output:
//    calNotes         String&               CAL_NOTES value
// Input from private data:
//    itsRecord        Record                Underlying record object
//
  record().get (MSC::fieldName (MSC::CAL_NOTES), calNotes);
};

//----------------------------------------------------------------------------


} //# NAMESPACE CASA - END

