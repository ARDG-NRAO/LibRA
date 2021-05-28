//# CalIter.cc: Implementation of CalIter.h
//# Copyright (C) 1996,1997,1998,2001,2002,2003
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

#include <synthesis/CalTables/CalIter.h>
#include <casa/Containers/Block.h>
#include <msvis/MSVis/MSCalEnums.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

//----------------------------------------------------------------------------

CalIter::CalIter (CalTable& calTable) : calTable_p(&calTable)
{
// Construct from a calibration table
// Input:
//    calTable       const CalTable&              Calibration table
// Output to private data:
//    calTable_p     CalTable*                    Ptr to calibration table
//
};

//----------------------------------------------------------------------------

CalIter::CalIter (CalTable& calTable, const Vector<Int>& iterIndices,
		  Sort::Order order, Sort::Option option) : 
  calTable_p(NULL)
{
// Construct from a calibration table and set of iteration indices
// Input:
//    calTable       CalTable&                    Calibration table
//    iterIndices    const Vector<Int>&           Iteration indices
//    order          Sort::Order                  Iteration order
//    option         Sort::Option                 Iteration sort option
// Output to private data:
//    calTable_p     CalTable*                    Ptr to calibration table
//
  // Sort the input calibration table on the specified iteration indices
  calTable_p = new CalTable (calTable.sort (MSC::fieldNames(iterIndices), 
					    order, option));
};

//----------------------------------------------------------------------------

CalIter::~CalIter()
{
// Null destructor
};

//----------------------------------------------------------------------------

CalTable& CalIter::subTable() 
{
// Return the current iteration sub-table
// Input from private data:
//    calTable_p     CalTable*                    Ptr to calibration table
//
  return *calTable_p;
};

//----------------------------------------------------------------------------

} //# NAMESPACE CASA - END

