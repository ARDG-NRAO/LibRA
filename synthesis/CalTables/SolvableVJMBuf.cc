//# SolvableVJMBuf.cc: Implementation of SolvableVJMBuf.h
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

#include <synthesis/CalTables/SolvableVJMBuf.h>
#include <casacore/casa/Arrays/ArrayMath.h>
#include <casacore/tables/Tables/RefRows.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

//----------------------------------------------------------------------------

SolvableVisJonesMBuf::SolvableVisJonesMBuf() : TimeVarVisJonesMBuf()
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
//    flagOK_p           Bool                 flag array cache ok
//    snrOK_p            Bool                 snr array cache ok
//
  // Invalidate cache 
  invalidate();
};

//----------------------------------------------------------------------------

SolvableVisJonesMBuf::SolvableVisJonesMBuf (const Vector<Int>& calIndices,
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

SolvableVisJonesMBuf::SolvableVisJonesMBuf (CalIterBase& calIter) 
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
//    flagOK_p           Bool                 flag array cache ok
//    snrOK_p            Bool                 snr array cache ok
//
  // Invalidate cache
  invalidate();
};

//----------------------------------------------------------------------------

Int SolvableVisJonesMBuf::append (CalTable& calTable)
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
  SolvableVisJonesMCol svjMainCol
    (dynamic_cast<SolvableVisJonesTable&>(calTable));

  // Append the current cal buffer cal_main columns
  RefRows refRows(startRow, endRow);
  svjMainCol.totalSolnOk().putColumnCells(refRows, totalSolnOk());
  svjMainCol.totalFit().putColumnCells(refRows, totalFit());
  svjMainCol.totalFitWgt().putColumnCells(refRows, totalFitWgt());
  svjMainCol.solnOk().putColumnCells(refRows, solnOk());
  svjMainCol.fit().putColumnCells(refRows, fit());
  svjMainCol.fitWgt().putColumnCells(refRows, fitWgt());
  svjMainCol.flag().putColumnCells(refRows, flag());
  svjMainCol.snr().putColumnCells(refRows, snr());
  
  return nAdded;
};

//----------------------------------------------------------------------------

Int SolvableVisJonesMBuf::nRow()
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
  Vector<Int> colLength(9);
  Int n = 0;
  colLength(n++) = nRowParent;
  colLength(n++) = totalSolnOk().nelements();
  colLength(n++) = totalFit().nelements();
  colLength(n++) = totalFitWgt().nelements();
  colLength(n++) = solnOk().shape().nelements() > 0 ? 
    solnOk().shape().getLast(1)(0) : 0;
  colLength(n++) = fit().shape().nelements() > 0 ? 
    fit().shape().getLast(1)(0) : 0;
  colLength(n++) = fitWgt().shape().nelements() > 0 ?
    fitWgt().shape().getLast(1)(0) : 0;
  colLength(n++) = flag().shape().nelements() > 0 ? 
    flag().shape().getLast(1)(0) : 0;
  colLength(n++) = snr().shape().nelements() > 0 ? 
    snr().shape().getLast(1)(0) : 0;

  return max(colLength);
};

//----------------------------------------------------------------------------

Vector<Bool>& SolvableVisJonesMBuf::totalSolnOk()
{
// TOTAL_SOLUTION_OK data field accessor
// Input from private data:
//    totalSolnOk_p      Vector<Bool>         Validity of total solution
//
  // Fill local cache for this column if cache not valid
  if (connectedToIter()) {
    if (!totalSolnOkOK_p) {
      calMainCol()->totalSolnOk().getColumn (totalSolnOk_p);
      totalSolnOkOK_p = true;
    };
  };
  return totalSolnOk_p;
};

//----------------------------------------------------------------------------

Vector<Float>& SolvableVisJonesMBuf::totalFit()
{
// TOTAL_FIT data field accessor
// Input from private data:
//    totalFit_p         Vector<Float>        Total fit
//
  // Fill local cache for this column if cache not valid
  if (connectedToIter()) {
    if (!totalFitOK_p) {
      calMainCol()->totalFit().getColumn (totalFit_p);
      totalFitOK_p = true;
    };
  };
  return totalFit_p;
};

//----------------------------------------------------------------------------

Vector<Float>& SolvableVisJonesMBuf::totalFitWgt()
{
// TOTAL_FIT_WEIGHT data field accessor
// Input from private data:
//    totalFitWgt_p      Vector<Float>        Total fit weight
//
  // Fill local cache for this column if cache not valid
  if (connectedToIter()) {
    if (!totalFitWgtOK_p) {
      calMainCol()->totalFitWgt().getColumn (totalFitWgt_p);
      totalFitWgtOK_p = true;
    };
  };
  return totalFitWgt_p;
};

//----------------------------------------------------------------------------

Array<Bool>& SolvableVisJonesMBuf::solnOk()
{
// SOLUTION_OK data field accessor
// Input from private data:
//    solnOk_p           Array<Bool>          Solution validity mask
//
  // Fill local cache for this column if cache not valid
  if (connectedToIter()) {
    if (!solnOkOK_p) {
      calMainCol()->solnOk().getColumn (solnOk_p);
      solnOkOK_p = true;
    };
  };
  return solnOk_p;
};

//----------------------------------------------------------------------------

Array<Float>& SolvableVisJonesMBuf::fit()
{
// FIT data field accessor
// Input from private data:
//    fit_p              Array<Float>         Fit array
//
  // Fill local cache for this column if cache not valid
  if (connectedToIter()) {
    if (!fitOK_p) {
      calMainCol()->fit().getColumn (fit_p);
      fitOK_p = true;
    };
  };
  return fit_p;
};

//----------------------------------------------------------------------------

Array<Float>& SolvableVisJonesMBuf::fitWgt()
{
// FIT_WGT data field accessor
// Input from private data:
//    fitWgt_p           Array<Float>         Fit weight array
//
  // Fill local cache for this column if cache not valid
  if (connectedToIter()) {
    if (!fitWgtOK_p) {
      calMainCol()->fitWgt().getColumn (fitWgt_p);
      fitWgtOK_p = true;
    };
  };
  return fitWgt_p;
};

//----------------------------------------------------------------------------

Array<Bool>& SolvableVisJonesMBuf::flag()
{
// FLAG data field accessor
// Input from private data:
//    flag_p           Array<Bool>          Solution flags mask
//
  // Fill local cache for this column if cache not valid
  if (connectedToIter()) {
    if (!flagOK_p) {
      calMainCol()->flag().getColumn (flag_p);
      flagOK_p = true;
    };
  };
  return flag_p;
};

//----------------------------------------------------------------------------

Array<Float>& SolvableVisJonesMBuf::snr()
{
// SNR data field accessor
// Input from private data:
//    snr_p           Array<Float>         Snr array
//
  // Fill local cache for this column if cache not valid
  if (connectedToIter()) {
    if (!snrOK_p) {
      calMainCol()->snr().getColumn (snr_p);
      snrOK_p = true;
    };
  };
  return snr_p;
};

//----------------------------------------------------------------------------

void SolvableVisJonesMBuf::invalidate()
{
// Invalidate the current cache
// Output to private data:
//    totalSolnOkOK_p    Bool                 Total solution vailidity cache ok
//    totalFitOK_p       Bool                 Total fit cache ok
//    totalFitWgtOK_p    Bool                 Total fit weight cache ok
//    solnOkOK_p         Bool                 Solution validity mask cache ok
//    fitOK_p            Bool                 Fit array cache ok
//    fitWgtOK_p         Bool                 Fit weight array cache ok
//    flagOK_p           Bool                 Flag array cache ok
//    snrOK_p            Bool                 Snr array cache ok
//
  // Invalidate parent class cache
  CalMainBuffer::invalidate();

  // Set all cache flags to false
  totalSolnOkOK_p = false;
  totalFitOK_p = false;   
  totalFitWgtOK_p = false;
  solnOkOK_p = false;     
  fitOK_p = false;      
  fitWgtOK_p = false;     
  flagOK_p = false;
  snrOK_p = false;
};

//----------------------------------------------------------------------------

void SolvableVisJonesMBuf::fillAttributes (const Vector<Int>& /*calIndices*/)
{
// Resize all attribute columns and set to their default values
// Input:
//    calIndices         const Vector<Int.&   Vector of cal indices, specified
//                                            as enums from class MSCalEnums,
//                                            to exclude 
// Output to private data:
//    totalSolnOk_p      Vector<Bool>         Validity of total solution
//    totalFit_p         Vector<Float>        Total fit
//    totalFitWgt_p      Vector<Float>        Total fit weight
//    solnOk_p           Array<Bool>          Solution validity mask
//    fitOK_p            Array<Float>         Fit array
//    fitWgt_p           Array<Float>         Fit weight array
//
  // Use the maximum number of rows currently defined in the cal buffer
  uInt nrow = nRow();

  // Process each local column separately
  //
  // TOTAL_SOLUTION_OK
  totalSolnOk().resize(nrow);
  totalSolnOk() = false;

  // TOTAL_FIT
  totalFit().resize(nrow);
  totalFit() = 0;

  // TOTAL_FIT_WEIGHT
  totalFitWgt().resize(nrow);
  totalFitWgt() = 0;

  // Array-based columns; set to default unit dimensions
  uInt numSpw = 1;
  uInt numChan = 1;
  uInt nJones = 1;
  IPosition sizeA(5, nJones, nJones, numSpw, numChan, nrow);

  // SOLUTION_OK
  solnOk().resize(sizeA);
  solnOk() = false;

  // FIT
  fit().resize(sizeA);
  fit() = 0;

  // FIT_WEIGHT
  fitWgt().resize(sizeA);
  fitWgt() = 0;

  // FLAG
  flag().resize(sizeA);
  flag() = true;

  // SNR
  snr().resize(sizeA);
  snr() = 0;


  return;
};

//----------------------------------------------------------------------------

GJonesMBuf::GJonesMBuf() : SolvableVisJonesMBuf()
{
// Null constructor
};

//----------------------------------------------------------------------------

GJonesMBuf::GJonesMBuf (const Vector<Int>& calIndices,
			const Block<Vector<Int> >& indexValues) : 
  SolvableVisJonesMBuf (calIndices, indexValues)
{
// Construct from a set of cal buffer indices and specified index values
// Output to private data:
//    SolvableVisJonesMBuf  SolvableVisJonesMBuf    Parent class cal main 
//                                                  table buffer
};

//----------------------------------------------------------------------------

GJonesMBuf::GJonesMBuf (CalIterBase& calIter) 
  : SolvableVisJonesMBuf (calIter)
{
// Construct from a calibration table iterator
// Input:
//    calIter            CalIterBase&         Calibration table iterator
};

//----------------------------------------------------------------------------

Bool GJonesMBuf::fillMatchingRows (const Vector<Int>& matchingRows,
				   const String& sFreqGrpName,
				   const Complex& sGain,
				   const MFrequency& sRefFreq,
				   const Int& sRefAnt)
{
// Update the calibration solution in each of a set of buffer rows
// Input:
//    matchingRows        const Vec<Int>&        Buffer rows to update
//    sFreqGrpName        const String&          Freq. group name 
//    sGain               const Complex&         Correction factor
//    sRefFreq            const MFrequency&      Reference frequency
//    sRefAnt             const Int &            Reference antenna id.
//
  // Initialization
  Bool retval = false;
  Int nMatch = matchingRows.nelements();

  if (nMatch > 0) {
    retval = true;
    // Update each matched row
    for (Int i=0; i < nMatch; i++) {
      uInt row = matchingRows(i);
      freqGrpName()(row) = sFreqGrpName;

      // Update all array elements
      IPosition gainShape = gain().shape();

      for (Int recep=0; recep < gainShape(0); recep++) {
	for (Int spw=0; spw < gainShape(1); spw++) {
	  IPosition ipos3(3, recep, spw, row);
	  // Reference frequency and antenna
	  refFreqMeas()(ipos3) = sRefFreq;
	  refAnt()(ipos3) = sRefAnt;
	  
	  // Gain factor
	  IPosition ipos4(4, recep, recep, spw, row);
	  gain()(ipos4) = sGain;
	};
      };
    };
  };
  return retval;
};
//----------------------------------------------------------------------------

DJonesMBuf::DJonesMBuf() : SolvableVisJonesMBuf()
{
// Null constructor
};

//----------------------------------------------------------------------------

DJonesMBuf::DJonesMBuf (const Vector<Int>& calIndices,
			const Block<Vector<Int> >& indexValues) : 
  SolvableVisJonesMBuf (calIndices, indexValues)
{
// Construct from a set of cal buffer indices and specified index values
// Output to private data:
//    SolvableVisJonesMBuf  SolvableVisJonesMBuf    Parent class cal main 
//                                                  table buffer
};

//----------------------------------------------------------------------------

DJonesMBuf::DJonesMBuf (CalIterBase& calIter) 
  : SolvableVisJonesMBuf (calIter)
{
// Construct from a calibration table iterator
// Input:
//    calIter            CalIterBase&         Calibration table iterator
};

//----------------------------------------------------------------------------

TJonesMBuf::TJonesMBuf() : SolvableVisJonesMBuf()
{
// Null constructor
};

//----------------------------------------------------------------------------

TJonesMBuf::TJonesMBuf (const Vector<Int>& calIndices,
			const Block<Vector<Int> >& indexValues) : 
  SolvableVisJonesMBuf (calIndices, indexValues)
{
// Construct from a set of cal buffer indices and specified index values
// Output to private data:
//    SolvableVisJonesMBuf  SolvableVisJonesMBuf    Parent class cal main 
//                                                  table buffer
};

//----------------------------------------------------------------------------

TJonesMBuf::TJonesMBuf (CalIterBase& calIter) 
  : SolvableVisJonesMBuf (calIter)
{
// Construct from a calibration table iterator
// Input:
//    calIter            CalIterBase&         Calibration table iterator
};

//----------------------------------------------------------------------------



} //# NAMESPACE CASA - END

