//# CalHistoryBuffer.cc: Implementation of CalHistoryBuffer.h
//# Copyright (C) 1996,1997,1998,2001,2003
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

#include <synthesis/CalTables/CalHistoryBuffer.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

//----------------------------------------------------------------------------

CalHistoryBuffer::CalHistoryBuffer() :
  connectedToIter_p(false), calHistCol_p(NULL)
{
// Null constructor
// Output to private data:
//    connectedToIter_p  Bool                 true if connected to iterator
//    calHistCol_p       CalHistoryColumns*   Ptr to cal_history col accessor
//    calParms_p         Vector<String>       Solver parameters
//    calTables_p        Vector<String>       Associated cal. tables
//    calSelect_p        Vector<String>       Cal. selection
//    calNotes_p         Vector<String>       Cal. notes
//    calParmsOK_p       Bool                 Solver parms cache ok
//    calTablesOK_p      Bool                 Assoc. cal. tables cache ok
//    calSelectOK_p      Bool                 Cal. selection cache ok
//    calNotesOK_p       Bool                 Cal. notes cache ok
//
  // Invalidate cache 
  invalidate();
};

//----------------------------------------------------------------------------

CalHistoryBuffer::CalHistoryBuffer (CalIterBase& calIter) :
  connectedToIter_p(true), calHistCol_p(NULL)
{
// Construct from a calibration table iterator
// Input:
//    calIter            CalIterBase&         Calibration table iterator
// Output to private data:
//    connectedToIter_p  Bool                 true if connected to iterator
//    calHistCol_p       CalHistoryColumns*   Ptr to cal_history col accessor
//    calParms_p         Vector<String>       Solver parameters
//    calTables_p        Vector<String>       Associated cal. tables
//    calSelect_p        Vector<String>       Cal. selection
//    calNotes_p         Vector<String>       Cal. notes
//    calParmsOK_p       Bool                 Solver parms cache ok
//    calTablesOK_p      Bool                 Assoc. cal. tables cache ok
//    calSelectOK_p      Bool                 Cal. selection cache ok
//    calNotesOK_p       Bool                 Cal. notes cache ok
//
  // Invalidate cache
  invalidate();

  // Construct the cal_history column accessor
  calHistCol_p = newCalHistoryCol (calIter.subTable());
};

//----------------------------------------------------------------------------

CalHistoryBuffer::~CalHistoryBuffer()
{
// Destructor
// Output to private data:
//    calHistCol_p       CalHistoryColumns*      Ptr to cal main col accessor
//
  if (calHistCol_p) delete calHistCol_p;
};

//----------------------------------------------------------------------------

void CalHistoryBuffer::invalidate()
{
// Invalidate the current cache
// Output to private data:
//    calParmsOK_p       Bool                 Solver parms cache ok
//    calTablesOK_p      Bool                 Assoc. cal. tables cache ok
//    calSelectOK_p      Bool                 Cal. selection cache ok
//    calNotesOK_p       Bool                 Cal. notes cache ok
//
  // Set all cache flags to false
  calParmsOK_p = false;
  calTablesOK_p = false;
  calSelectOK_p = false;
  calNotesOK_p = false;
};

//----------------------------------------------------------------------------

Vector<String>& CalHistoryBuffer::calParms()
{
// CAL_PARMS data field accessor
// Input from private data:
//    connectedToIter_p  Bool                 true if connected to iterator
//    calParms_p         Vector<String>&      Solver parms
//
  // Fill local cache for this column if cache not valid
  if (connectedToIter()) {
    if (!calParmsOK_p) {
      calHistCol()->calParms().getColumn (calParms_p);
      calParmsOK_p = true;
    };
  };
  return calParms_p;
};

//----------------------------------------------------------------------------

Vector<String>& CalHistoryBuffer::calTables()
{
// CAL_TABLES data field accessor
// Input from private data:
//    connectedToIter_p  Bool                 true if connected to iterator
//    calTables_p        Vector<String>&      Associated cal. tables
//
  // Fill local cache for this column if cache not valid
  if (connectedToIter()) {
    if (!calTablesOK_p) {
      calHistCol()->calTables().getColumn (calTables_p);
      calTablesOK_p = true;
    };
  };
  return calTables_p;
};

//----------------------------------------------------------------------------

Vector<String>& CalHistoryBuffer::calSelect()
{
// CAL_SELECT data field accessor
// Input from private data:
//    connectedToIter_p  Bool                 true if connected to iterator
//    calSelect_p        Vector<String>&      Cal. selection
//
  // Fill local cache for this column if cache not valid
  if (connectedToIter()) {
    if (!calSelectOK_p) {
      calHistCol()->calSelect().getColumn (calSelect_p);
      calSelectOK_p = true;
    };
  };
  return calSelect_p;
};

//----------------------------------------------------------------------------

Vector<String>& CalHistoryBuffer::calNotes()
{
// CAL_PARMS data field accessor
// Input from private data:
//    connectedToIter_p  Bool                 true if connected to iterator
//    calNotes_p         Vector<String>&      Cal. notes
//
  // Fill local cache for this column if cache not valid
  if (connectedToIter()) {
    if (!calNotesOK_p) {
      calHistCol()->calNotes().getColumn (calNotes_p);
      calNotesOK_p = true;
    };
  };
  return calNotes_p;
};

//----------------------------------------------------------------------------

} //# NAMESPACE CASA - END

