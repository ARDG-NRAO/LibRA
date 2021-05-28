//# EPointMBuf.cc: Implementation of EPointMBuf.h
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

#include <synthesis/CalTables/EPointMBuf.h>
#include <casa/Arrays/ArrayMath.h>
#include <tables/Tables/RefRows.h>
using namespace casacore;
namespace casa {
//----------------------------------------------------------------------------

EPointMBuf::EPointMBuf() : TimeVarVisJonesMBuf()
{
// Null constructor
// Output to private data:
//    totalSolnOk_p      Vector<Bool>         Validity of total solution
//    totalFit_p         Vector<Float>        Total fit
//    totalFitWgt_p      Vector<Float>        Total fit weight
//    solnOk_p           Array<Bool>          Solution validity mask
//    fitOK_p            Array<Float>         Fit array
//    fitWgt_p           Array<Float>         Fit weight array
//    totalSolnOkOK_p    Bool                 Total solution vailidity cache ok
//    totalFitOK_p       Bool                 Total fit cache ok
//    totalFitWgtOK_p    Bool                 Total fit weight cache ok
//    solnOkOK_p         Bool                 Solution validity mask cache ok
//    fitOK_p            Bool                 Fit array cache ok
//    fitWgtOK_p         Bool                 Fit weight array cache ok
//
  // Invalidate cache 
  invalidate();
};

//----------------------------------------------------------------------------

EPointMBuf::EPointMBuf (const Vector<Int>& calIndices,
			const Block<Vector<Int> >& 
			indexValues) : 
  TimeVarVisJonesMBuf (calIndices, indexValues)
{
// Construct from a set of cal buffer indices and specified index values
// Output to private data:
//    TimeVarVisJonesMBuf   TimeVarVisJonesMBuf    Parent class cal main 
//                                                 table buffer
  // Set local non-index columns to default values
  fillAttributes (calIndices);
};

//----------------------------------------------------------------------------

EPointMBuf::EPointMBuf (CalIterBase& calIter) 
  : TimeVarVisJonesMBuf (calIter)
{
// Construct from a calibration table iterator
// Input:
//    calIter            CalIterBase&         Calibration table iterator
// Output to private data:
//    totalSolnOk_p      Vector<Bool>         Validity of total solution
//    totalFit_p         Vector<Float>        Total fit
//    totalFitWgt_p      Vector<Float>        Total fit weight
//    solnOk_p           Array<Bool>          Solution validity mask
//    fitOK_p            Array<Float>         Fit array
//    fitWgt_p           Array<Float>         Fit weight array
//    totalSolnOkOK_p    Bool                 Total solution vailidity cache ok
//    totalFitOK_p       Bool                 Total fit cache ok
//    totalFitWgtOK_p    Bool                 Total fit weight cache ok
//    solnOkOK_p         Bool                 Solution validity mask cache ok
//    fitOK_p            Bool                 Fit array cache ok
//    fitWgtOK_p         Bool                 Fit weight array cache ok
//
  // Invalidate cache
  invalidate();
};

//----------------------------------------------------------------------------

Int EPointMBuf::append (CalTable& calTable)
{
// Append the current calibration buffer to a calibration table
// Input:
//    calTable           CalTable&            Calibration table
// Output:
//    append             Int                  No. of rows appended
//
  // Extend the inherited parent method
  Int nAdded = CalMainBuffer::append(calTable);

  // Compute the rows added in the parent class
  Int endRow = calTable.nRowMain() - 1;
  Int startRow = endRow - nAdded + 1;

  // Attach a cal table columns accessor
  EPointMCol epointMainCol
    (dynamic_cast<EPointTable&>(calTable));

  // Append the current cal buffer cal_main columns
  RefRows refRows(startRow, endRow);
  epointMainCol.pointingOffset().putColumnCells(refRows, pointingOffset());
  
  return nAdded;
};

//----------------------------------------------------------------------------

Int EPointMBuf::nRow()
{
// Return the maximum number of rows in the cal buffer
// Input from private data:
//    totalSolnOk_p      Vector<Bool>         Validity of total solution
//    totalFit_p         Vector<Float>        Total fit
//    totalFitWgt_p      Vector<Float>        Total fit weight
//    solnOk_p           Array<Bool>          Solution validity mask
//    fitOK_p            Array<Float>         Fit array
//    fitWgt_p           Array<Float>         Fit weight array
//    totalSolnOkOK_p    Bool                 Total solution vailidity cache ok
//    totalFitOK_p       Bool                 Total fit cache ok
//    totalFitWgtOK_p    Bool                 Total fit weight cache ok
//    solnOkOK_p         Bool                 Solution validity mask cache ok
//    fitOK_p            Bool                 Fit array cache ok
//    fitWgtOK_p         Bool                 Fit weight array cache ok
// Output:
//    nRow               Int                  Max. no. of rows in the buffer
//
  // Extend the inherited parent method
  Int nRowParent = CalMainBuffer::nRow();

  // Process each local column individually
  Vector<Int> colLength(2);
  Int n = 0;
  colLength(n++) = nRowParent;
  colLength(n++) = pointingOffset().nelements();

  return max(colLength);
};

//----------------------------------------------------------------------------

Array<Float>& EPointMBuf::pointingOffset()
{
// TOTAL_SOLUTION_OK data field accessor
// Input from private data:
//    totalSolnOk_p      Vector<Bool>         Validity of total solution
//
  // Fill local cache for this column if cache not valid
  if (connectedToIter()) {
    if (!pointingOffsetOK_p) {
      calMainCol()->pointingOffset().getColumn (pointingOffset_p);
      pointingOffsetOK_p = true;
    };
  };
  return pointingOffset_p;
};


//----------------------------------------------------------------------------

void EPointMBuf::invalidate()
{
// Invalidate the current cache
// Output to private data:
//    totalSolnOkOK_p    Bool                 Total solution vailidity cache ok
//    totalFitOK_p       Bool                 Total fit cache ok
//    totalFitWgtOK_p    Bool                 Total fit weight cache ok
//    solnOkOK_p         Bool                 Solution validity mask cache ok
//    fitOK_p            Bool                 Fit array cache ok
//    fitWgtOK_p         Bool                 Fit weight array cache ok
//
  // Invalidate parent class cache
  CalMainBuffer::invalidate();

  // Set all cache flags to false
  pointingOffsetOK_p = false;
};

//----------------------------------------------------------------------------

  void EPointMBuf::fillAttributes (const Vector<Int>& /*calIndices*/)
{
// Resize all attribute columns and set to their default values
// Input:
//    calIndices         const Vector<Int.&   Vector of cal indices, specified
//                                            as enums from class MSCalEnums,
//                                            to exclude 
// Output to private data:
//    pointingOffset_p      Array<Float>      Antenna pointing offsets

  // Use the maximum number of rows currently defined in the cal buffer
  uInt nrow = nRow();

  // Process each local column separately
  //
  // TOTAL_SOLUTION_OK
  IPosition shape(2,2,nrow);
  pointingOffset().resize(shape);
  pointingOffset() = 0.0;

  return;
};
}
