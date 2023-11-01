//# CalMainBuffer.cc: Implementation of CalMainBuffer.h
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

#include <synthesis/CalTables/CalMainBuffer.h>
#include <casacore/casa/Arrays/ArrayMath.h>
#include <casacore/casa/Arrays/ArrayLogical.h>
#include <casacore/casa/Arrays/MaskedArray.h>
#include <casacore/casa/Arrays/ArrayUtil.h>
#include <casacore/tables/Tables/RefRows.h>
#include <casacore/casa/Exceptions/Error.h>
#include <casacore/measures/Measures/MCFrequency.h>
#include <casacore/casa/typeinfo.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

//----------------------------------------------------------------------------

CalMainBuffer::CalMainBuffer() :
  connectedToIter_p(false), calIter_p(NULL), calMainCol_p(NULL)
{
// Null constructor
// Output to private data:
//    connectedToIter_p  Bool                 true if connected to iterator
//    calIter_p          CalIterBase*         Ptr to calibration table iterator
//    calMainCol_p       CalMainColumns*      Ptr to cal main col accessor
//    calDescBuf_p       CalDescBuffer*       Ptr to cal_desc buffer
//    calHistBuf_p       CalHistoryBuffer*    Ptr to cal_history buffer
//    timeMeas_p         Vector<MEpoch>       Time as a Measure
//    timeEPQuant_p      Vector<Quantity>     Extended prec. time as Quantum
//    intervalQuant_p    Vector<Quantity>     Time interval as Quantum
//    antenna1_p         Vector<Int>          Antenna 1
//    feed1_p            Vector<Int>          Feed 1
//    fieldId_p          Vector<Int>          Field id.
//    arrayId_p          Vector<Int>          Array id.
//    obsId_p            Vector<Int>          Observation id.
//    scanNo_p           Vector<Int>          Scan no.
//    processorId_p      Vector<Int>          Processor id.
//    stateId_p          Vector<Int>          State id.
//    phaseId_p          Vector<Int>          Phase id.
//    pulsarBin_p        Vector<Int>          Pulsar bin
//    pulsarGateId_p     Vector<Int>          Pulsar gate id.
//    freqGrp_p          Vector<Int>          Frequency group
//    freqGrpName_p      Vector<String>       Frequency group name
//    fieldName_p        Vector<String>       Field name
//    fieldCode_p        Vector<String>       Field code
//    sourceName_p       Vector<String>       Source name
//    sourceCode_p       Vector<String>       Source code
//    calGrp_p           Vector<Int>          Calibration group
//    gain_p             Array<Complex>       Gain
//    refAnt_p           Array<Int>           Reference antenna
//    refFeed_p          Array<Int>           Reference feed
//    refReceptor_p      Array<Int>           Reference receptor
//    refFreqMeas_p      Array<MFrequency>    Reference frequency as Measure
//    measFreqRef_p      Vector<Int>          Frequency measures ref.
//    refDirMeas_p       Array<MDirection>    Reference direction as Measure
//    measDirRef_p       Vector<Int>          Direction measures ref.
//    calDescId_p        Vector<Int>          CAL_DESC id.
//    calHistoryId_p     Vector<Int>          CAL_HISTORY id.
//    timeMeasOK_p       Bool                 Time as a Measure cache ok
//    timeEPQuantOK_p    Bool                 EP time as Quantum cache ok
//    intervalQuantOK_p  Bool                 Time interval as Quantum cache ok
//    antenna1OK_p       Bool                 Antenna 1 cache ok
//    feed1OK_p          Bool                 Feed 1 cache ok
//    fieldIdOK_p        Bool                 Field id. cache ok
//    arrayIdOK_p        Bool                 Array id. cache ok
//    obsIdOK_p          Bool                 Observation id. cache ok
//    scanNoOK_p         Bool                 Scan no. cache ok
//    processorIdOK_p    Bool                 Processor id. cache ok
//    stateIdOK_p        Bool                 State id. cache ok
//    phaseIdOK_p        Bool                 Phase id. cache ok
//    pulsarBinOK_p      Bool                 Pulsar bin cache ok
//    pulsarGateIdOK_p   Bool                 Pulsar gate id. cache ok
//    freqGrpOK_p        Bool                 Frequency group cache ok
//    freqGrpNameOK_p    Bool                 Frequency group name cache ok
//    fieldNameOK_p      Bool                 Field name cache ok
//    fieldCodeOK_p      Bool                 Field code cache ok
//    sourceNameOK_p     Bool                 Source name cache ok
//    sourceCodeOK_p     Bool                 Source code cache ok
//    calGrpOK_p         Bool                 Calibration group cache ok
//    gainOK_p           Bool                 Gain cache ok
//    refAntOK_p         Bool                 Reference antenna cache ok
//    refFeedOK_p        Bool                 Reference feed cache ok
//    refReceptorOK_p    Bool                 Reference receptor cache ok
//    refFreqMeasOK_p    Bool                 Ref. freq. as Measure cache ok
//    measFreqRefOK_p    Bool                 Frequency measures ref. cache ok
//    refDirMeasOK_p     Bool                 Ref. dir. as Measure cache ok
//    measDirRefOK_p     Bool                 Direction measures ref. cache ok
//    calDescIdOK_p      Bool                 CAL_DESC id. cache ok
//    calHistoryIdOK_p   Bool                 CAL_HISTORY id. cache ok
//
  // Invalidate cache 
  invalidate();

  // Construct sub-table cal_desc and cal_history buffers
  calDescBuf_p = new CalDescBuffer();
  calHistBuf_p = new CalHistoryBuffer();
};

//----------------------------------------------------------------------------

CalMainBuffer::CalMainBuffer (const Vector<Int>& calIndices, 
			      const Block<Vector<Int> >& indexValues) :
  connectedToIter_p(false), calIter_p(NULL), calMainCol_p(NULL)
{
// Construct from a set of cal buffer indices and specified index values
// Input:
//    calIndices     const Vector<Int>&      Cal indices (as specified as
//                                           enums from class MSCalEnums)
//    indexValues    const Block<Vec<Int>>&  Associated index values
// Output to private data:
//    connectedToIter_p  Bool                 true if connected to iterator
//    calIter_p          CalIterBase*         Ptr to cal. table iterator
//    calMainCol_p       CalMainColumns*      Ptr to cal main col accessor
//    calDescBuf_p       CalDescBuffer*       Ptr to cal_desc buffer
//    calHistBuf_p       CalHistoryBuffer*    Ptr to cal_history buffer
//
  // Invalidate cache 
  invalidate();

  // Span the calibration buffer using the specified indices and index values
  fillIndices(calIndices, indexValues);

  // Set non-index columns to default values
  fillAttributes(calIndices);

  // Construct sub-table cal_desc and cal_history buffers
  calDescBuf_p = new CalDescBuffer();
  calHistBuf_p = new CalHistoryBuffer();
};

//----------------------------------------------------------------------------

CalMainBuffer::CalMainBuffer (CalIterBase& calIter) :
  connectedToIter_p(true), calIter_p(&calIter), calMainCol_p(NULL)
{
// Construct from a calibration table iterator
// Input:
//    calIter            CalIterBase&         Calibration table iterator
// Output to private data:
//    connectedToIter_p  Bool                 true if connected to iterator
//    calIter_p          CalIterBase*         Ptr to cal. table iterator
//    calMainCol_p       CalMainColumns*      Ptr to cal main col accessor
//    calDescBuf_p       CalDescBuffer*       Ptr to cal_desc buffer
//    calHistBuf_p       CalHistoryBuffer*    Ptr to cal_history buffer
//    timeMeas_p         Vector<MEpoch>       Time as a Measure
//    timeEPQuant_p      Vector<Quantity>     Extended prec. time as Quantum
//    intervalQuant_p    Vector<Quantity>     Time interval as Quantum
//    antenna1_p         Vector<Int>          Antenna 1
//    feed1_p            Vector<Int>          Feed 1
//    fieldId_p          Vector<Int>          Field id.
//    arrayId_p          Vector<Int>          Array id.
//    obsId_p            Vector<Int>          Observation id.
//    scanNo_p           Vector<Int>          Scan no.
//    processorId_p      Vector<Int>          Processor id.
//    stateId_p          Vector<Int>          State id.
//    phaseId_p          Vector<Int>          Phase id.
//    pulsarBin_p        Vector<Int>          Pulsar bin
//    pulsarGateId_p     Vector<Int>          Pulsar gate id.
//    freqGrp_p          Vector<Int>          Frequency group
//    freqGrpName_p      Vector<String>       Frequency group name
//    fieldName_p        Vector<String>       Field name
//    fieldCode_p        Vector<String>       Field code
//    sourceName_p       Vector<String>       Source name
//    sourceCode_p       Vector<String>       Source code
//    calGrp_p           Vector<Int>          Calibration group
//    gain_p             Array<Complex>       Gain
//    refAnt_p           Array<Int>           Reference antenna
//    refFeed_p          Array<Int>           Reference feed
//    refReceptor_p      Array<Int>           Reference receptor
//    refFreqMeas_p      Array<MFrequency>    Reference frequency as Measure
//    measFreqRef_p      Vector<Int>          Frequency measures ref.
//    refDirMeas_p       Array<MDirection>    Reference direction as Measure
//    measDirRef_p       Vector<Int>          Direction measures ref.
//    calDescId_p        Vector<Int>          CAL_DESC id.
//    calHistoryId_p     Vector<Int>          CAL_HISTORY id.
//    timeMeasOK_p       Bool                 Time as a Measure cache ok
//    timeEPQuantOK_p    Bool                 EP time as Quantum cache ok
//    intervalQuantOK_p  Bool                 Time interval as Quantum cache ok
//    antenna1OK_p       Bool                 Antenna 1 cache ok
//    feed1OK_p          Bool                 Feed 1 cache ok
//    fieldIdOK_p        Bool                 Field id. cache ok
//    arrayIdOK_p        Bool                 Array id. cache ok
//    obsIdOK_p          Bool                 Observation id. cache ok
//    scanNoOK_p         Bool                 Scan no. cache ok
//    processorIdOK_p    Bool                 Processor id. cache ok
//    stateIdOK_p        Bool                 State id. cache ok
//    phaseIdOK_p        Bool                 Phase id. cache ok
//    pulsarBinOK_p      Bool                 Pulsar bin cache ok
//    pulsarGateIdOK_p   Bool                 Pulsar gate id. cache ok
//    freqGrpOK_p        Bool                 Frequency group cache ok
//    freqGrpNameOK_p    Bool                 Frequency group name cache ok
//    fieldNameOK_p      Bool                 Field name cache ok
//    fieldCodeOK_p      Bool                 Field code cache ok
//    sourceNameOK_p     Bool                 Source name cache ok
//    sourceCodeOK_p     Bool                 Source code cache ok
//    calGrpOK_p         Bool                 Calibration group cache ok
//    gainOK_p           Bool                 Gain cache ok
//    refAntOK_p         Bool                 Reference antenna cache ok
//    refFeedOK_p        Bool                 Reference feed cache ok
//    refReceptorOK_p    Bool                 Reference receptor cache ok
//    refFreqMeasOK_p    Bool                 Ref. freq. as Measure cache ok
//    measFreqRefOK_p    Bool                 Frequency measures ref. cache ok
//    refDirMeasOK_p     Bool                 Ref. dir. as Measure cache ok
//    measDirRefOK_p     Bool                 Direction measures ref. cache ok
//    calDescIdOK_p      Bool                 CAL_DESC id. cache ok
//    calHistoryIdOK_p   Bool                 CAL_HISTORY id. cache ok
//
  // Invalidate cache
  invalidate();

  // Construct the sub-table cal_desc and cal_history buffers
  calDescBuf_p = new CalDescBuffer (calIter);
  calHistBuf_p = new CalHistoryBuffer (calIter);
};

//----------------------------------------------------------------------------

CalMainBuffer::~CalMainBuffer()
{
// Destructor
// Output to private data:
//    calMainCol_p       CalMainColumns*      Ptr to cal main col accessor
//
  // Delete all pointer data
  if (calMainCol_p) delete(calMainCol_p);
  //  if (calIter_p) delete(calIter_p);
  if (calDescBuf_p) delete(calDescBuf_p);
  if (calHistBuf_p) delete(calHistBuf_p);
};

//----------------------------------------------------------------------------

void CalMainBuffer::synchronize()
{
// Synchronize the buffer (if an iterator is attached)
// Output to private data:
//    calMainCol_p       CalMainColumns*      Ptr to cal main col accessor
//
  // Construct a new calibration main table columns accessor
  if (calMainCol_p) delete calMainCol_p;

  calMainCol_p = newCalMainCol (calIter_p->subTable());

  // Invalidate cache
  invalidate();
};

//----------------------------------------------------------------------------

void CalMainBuffer::fillCache()
{
// Force an explicit read to cache for all columns (if attached to an iterator)
// Output to private data:
//    timeMeas_p         Vector<MEpoch>       Time as a Measure
//    timeEPQuant_p      Vector<Quantity>     Extended prec. time as Quantum
//    intervalQuant_p    Vector<Quantity>     Time interval as Quantum
//    antenna1_p         Vector<Int>          Antenna 1
//    feed1_p            Vector<Int>          Feed 1
//    fieldId_p          Vector<Int>          Field id.
//    arrayId_p          Vector<Int>          Array id.
//    obsId_p            Vector<Int>          Observation id.
//    scanNo_p           Vector<Int>          Scan no.
//    processorId_p      Vector<Int>          Processor id.
//    stateId_p          Vector<Int>          State id.
//    phaseId_p          Vector<Int>          Phase id.
//    pulsarBin_p        Vector<Int>          Pulsar bin
//    pulsarGateId_p     Vector<Int>          Pulsar gate id.
//    freqGrp_p          Vector<Int>          Frequency group
//    freqGrpName_p      Vector<String>       Frequency group name
//    fieldName_p        Vector<String>       Field name
//    fieldCode_p        Vector<String>       Field code
//    sourceName_p       Vector<String>       Source name
//    sourceCode_p       Vector<String>       Source code
//    calGrp_p           Vector<Int>          Calibration group
//    gain_p             Array<Complex>       Gain
//    refAnt_p           Array<Int>           Reference antenna
//    refFeed_p          Array<Int>           Reference feed
//    refReceptor_p      Array<Int>           Reference receptor
//    refFreqMeas_p      Array<MFrequency>    Reference frequency as Measure
//    measFreqRef_p      Vector<Int>          Frequency measures ref.
//    refDirMeas_p       Array<MDirection>    Reference direction as Measure
//    measDirRef_p       Vector<Int>          Direction measures ref.
//    calDescId_p        Vector<Int>          CAL_DESC id.
//    calHistoryId_p     Vector<Int>          CAL_HISTORY id.
//    timeMeasOK_p       Bool                 Time as a Measure cache ok
//    timeEPQuantOK_p    Bool                 EP time as Quantum cache ok
//    intervalQuantOK_p  Bool                 Time interval as Quantum cache ok
//    antenna1OK_p       Bool                 Antenna 1 cache ok
//    feed1OK_p          Bool                 Feed 1 cache ok
//    fieldIdOK_p        Bool                 Field id. cache ok
//    arrayIdOK_p        Bool                 Array id. cache ok
//    obsIdOK_p          Bool                 Observation id. cache ok
//    scanNoOK_p         Bool                 Scan no. cache ok
//    processorIdOK_p    Bool                 Processor id. cache ok
//    stateIdOK_p        Bool                 State id. cache ok
//    phaseIdOK_p        Bool                 Phase id. cache ok
//    pulsarBinOK_p      Bool                 Pulsar bin cache ok
//    pulsarGateIdOK_p   Bool                 Pulsar gate id. cache ok
//    freqGrpOK_p        Bool                 Frequency group cache ok
//    freqGrpNameOK_p    Bool                 Frequency group name cache ok
//    fieldNameOK_p      Bool                 Field name cache ok
//    fieldCodeOK_p      Bool                 Field code cache ok
//    sourceNameOK_p     Bool                 Source name cache ok
//    sourceCodeOK_p     Bool                 Source code cache ok
//    calGrpOK_p         Bool                 Calibration group cache ok
//    gainOK_p           Bool                 Gain cache ok
//    refAntOK_p         Bool                 Reference antenna cache ok
//    refFeedOK_p        Bool                 Reference feed cache ok
//    refReceptorOK_p    Bool                 Reference receptor cache ok
//    refFreqMeasOK_p    Bool                 Ref. freq. as Measure cache ok
//    measFreqRefOK_p    Bool                 Frequency measures ref. cache ok
//    refDirMeasOK_p     Bool                 Ref. dir. as Measure cache ok
//    measDirRefOK_p     Bool                 Direction measures ref. cache ok
//    calDescIdOK_p      Bool                 CAL_DESC id. cache ok
//    calHistoryIdOK_p   Bool                 CAL_HISTORY id. cache ok
//
  // Method nRow() will implicity reference (hence fill) all cache columns
  nRow();
};

//----------------------------------------------------------------------------

Int CalMainBuffer::append (CalTable& calTable)
{
// Append the current calibration buffer to a calibration table
// Input:
//    calTable           CalTable&            Calibration table
// Output:
//    append             Int                  No. of rows appended
//
  // Process the CAL_DESC sub-table
  if (max(calDescId()) >= 0) {
  };

  // Process the CAL_HISTORY sub-table
  if (max(calHistoryId()) >= 0) {
  };

  // Extend the cal table by the number of rows in the current cal buffer
  uInt startRow = calTable.nRowMain();
  calTable.addRowMain(nRow());
  uInt endRow = calTable.nRowMain() - 1;
  
  // Attach a cal table columns accessor
  CalMainColumns calMainCol(calTable);

  // Append the current cal buffer cal_main columns
  RefRows refRows(startRow, endRow);
  calMainCol.antenna1().putColumnCells(refRows, antenna1());
  calMainCol.feed1().putColumnCells(refRows, feed1());
  calMainCol.fieldId().putColumnCells(refRows, fieldId());
  calMainCol.arrayId().putColumnCells(refRows, arrayId());
  calMainCol.obsId().putColumnCells(refRows, obsId());
  calMainCol.scanNo().putColumnCells(refRows, scanNo());
  calMainCol.processorId().putColumnCells(refRows, processorId());
  calMainCol.stateId().putColumnCells(refRows, stateId());
  calMainCol.phaseId().putColumnCells(refRows, phaseId());
  calMainCol.pulsarBin().putColumnCells(refRows, pulsarBin());
  calMainCol.pulsarGateId().putColumnCells(refRows, pulsarGateId());
  calMainCol.freqGrp().putColumnCells(refRows, freqGrp());
  calMainCol.freqGrpName().putColumnCells(refRows, freqGrpName());
  calMainCol.fieldName().putColumnCells(refRows, fieldName());
  calMainCol.fieldCode().putColumnCells(refRows, fieldCode());
  calMainCol.sourceName().putColumnCells(refRows, sourceName());
  calMainCol.sourceCode().putColumnCells(refRows, sourceCode());
  calMainCol.calGrp().putColumnCells(refRows, calGrp());
  calMainCol.gain().putColumnCells(refRows, gain());
  calMainCol.refAnt().putColumnCells(refRows, refAnt());
  calMainCol.refFeed().putColumnCells(refRows, refFeed());
  calMainCol.refReceptor().putColumnCells(refRows, refReceptor());
  calMainCol.calDescId().putColumnCells(refRows, calDescId());
  calMainCol.calHistoryId().putColumnCells(refRows, calHistoryId());


  // Do the measures columns by cell (no putColumnCells() method 
  // available for TableMeasures columns)
  IPosition blcRefFreq(refFreqMeas().shape());
  blcRefFreq = 0;
  IPosition trcRefFreq = refFreqMeas().shape() - 1;
  IPosition blcRefDir(refDirMeas().shape());
  blcRefDir = 0;
  IPosition trcRefDir = refDirMeas().shape() - 1;
  uInt lastAxis = trcRefDir.nelements() - 1;

  for (uInt row=startRow; row <= endRow; row++) {
    Int indx = row - startRow;
    calMainCol.timeMeas().put(row, timeMeas()(indx));
    calMainCol.timeEPQuant().put(row, timeEPQuant()(indx));
    calMainCol.intervalQuant().put(row, intervalQuant()(indx));
    IPosition currIndxPos(1, indx);
    blcRefFreq.setLast(currIndxPos);
    trcRefFreq.setLast(currIndxPos);
    Array<MFrequency> rowRefFreq = 
      refFreqMeas()(blcRefFreq,trcRefFreq).nonDegenerate(lastAxis);
    calMainCol.refFreqMeas().put(row, rowRefFreq);
    blcRefDir.setLast(currIndxPos);
    trcRefDir.setLast(currIndxPos);
    Array<MDirection> rowRefDir = 
      refDirMeas()(blcRefDir,trcRefDir).nonDegenerate(lastAxis);
    calMainCol.refDirMeas().put(row, rowRefDir);
  };

  return (endRow-startRow+1);
};

//----------------------------------------------------------------------------

Int CalMainBuffer::nRow()
{
// Return the maximum number of rows in the calibration buffer
// Input from private data:
//    timeMeas_p         Vector<MEpoch>       Time as a Measure
//    timeEPQuant_p      Vector<Quantity>     Extended prec. time as Quantum
//    intervalQuant_p    Vector<Quantity>     Time interval as Quantum
//    antenna1_p         Vector<Int>          Antenna 1
//    feed1_p            Vector<Int>          Feed 1
//    fieldId_p          Vector<Int>          Field id.
//    arrayId_p          Vector<Int>          Array id.
//    obsId_p            Vector<Int>          Observation id.
//    scanNo_p           Vector<Int>          Scan no.
//    processorId_p      Vector<Int>          Processor id.
//    stateId_p          Vector<Int>          State id.
//    phaseId_p          Vector<Int>          Phase id.
//    pulsarBin_p        Vector<Int>          Pulsar bin
//    pulsarGateId_p     Vector<Int>          Pulsar gate id.
//    freqGrp_p          Vector<Int>          Frequency group
//    freqGrpName_p      Vector<String>       Frequency group name
//    fieldName_p        Vector<String>       Field name
//    fieldCode_p        Vector<String>       Field code
//    sourceName_p       Vector<String>       Source name
//    sourceCode_p       Vector<String>       Source code
//    calGrp_p           Vector<Int>          Calibration group
//    gain_p             Array<Complex>       Gain
//    refAnt_p           Array<Int>           Reference antenna
//    refFeed_p          Array<Int>           Reference feed
//    refReceptor_p      Array<Int>           Reference receptor
//    refFreqMeas_p      Array<MFrequency>    Reference frequency as Measure
//    measFreqRef_p      Vector<Int>          Frequency measures ref.
//    refDirMeas_p       Array<MDirection>    Reference direction as Measure
//    measDirRef_p       Vector<Int>          Direction measures ref.
//    calDescId_p        Vector<Int>          CAL_DESC id.
//    calHistoryId_p     Vector<Int>          CAL_HISTORY id.
//
  // Process each column individually
  Vector<Int> colLength(31, 0);
  Int n = 0;
  colLength(n++) = timeMeas().nelements();
  colLength(n++) = timeEPQuant().nelements();
  colLength(n++) = intervalQuant().nelements();
  colLength(n++) = antenna1().nelements();
  colLength(n++) = feed1().nelements();
  colLength(n++) = fieldId().nelements();
  colLength(n++) = arrayId().nelements();
  colLength(n++) = obsId().nelements();
  colLength(n++) = scanNo().nelements();
  colLength(n++) = processorId().nelements();
  colLength(n++) = stateId().nelements();
  colLength(n++) = phaseId().nelements();
  colLength(n++) = pulsarBin().nelements();
  colLength(n++) = pulsarGateId().nelements();
  colLength(n++) = freqGrp().nelements();
  colLength(n++) = freqGrpName().nelements();
  colLength(n++) = fieldName().nelements();
  colLength(n++) = fieldCode().nelements();
  colLength(n++) = sourceName().nelements();
  colLength(n++) = sourceCode().nelements();
  colLength(n++) = calGrp().nelements();
  colLength(n++) = gain().shape().nelements() > 0 ? 
    gain().shape().getLast(1)(0) : 0;
  colLength(n++) = refAnt().shape().nelements() > 0 ?
    refAnt().shape().getLast(1)(0) : 0;
  colLength(n++) = refFeed().shape().nelements() > 0 ? 
    refFeed().shape().getLast(1)(0) : 0;
  colLength(n++) = refReceptor().shape().nelements() > 0 ? 
    refReceptor().shape().getLast(1)(0) : 0;
  colLength(n++) = refFreqMeas().shape().nelements() > 0 ? 
    refFreqMeas().shape().getLast(1)(0) : 0;
  colLength(n++) = measFreqRef().nelements();
  colLength(n++) = refDirMeas().shape().nelements() > 1 ? 
    refDirMeas().shape().getLast(1)(0) : 0;
  colLength(n++) = measDirRef().nelements();
  colLength(n++) = calDescId().nelements();
  colLength(n++) = calHistoryId().nelements();

  return max(colLength);
};

//----------------------------------------------------------------------------

Vector<MEpoch>& CalMainBuffer::timeMeas()
{
// TIME data field accessor (as Measure)
// Input from private data:
//    connectedToIter_p  Bool                 true if connected to iterator
//    timeMeas_p         Vector<MEpoch>&      Time as Measure
//
  // Fill local cache for this column if cache not valid
  if (connectedToIter()) {
    if (!timeMeasOK_p) {
      Int nrow = calMainCol()->time().nrow();
      timeMeas_p.resize(nrow);
      for (Int row=0; row < nrow; row++) {
	calMainCol()->timeMeas().get (row, timeMeas_p(row));
      };
      timeMeasOK_p = true;
    };
  };
  return timeMeas_p;
};

//----------------------------------------------------------------------------

Vector<Quantity>& CalMainBuffer::timeEPQuant()
{
// TIME_EXTRA_PREC data field accessor (as Quantum)
// Input from private data:
//    connectedToIter_p  Bool                 true if connected to iterator
//    timeEPQuant_p      Vector<Quantity>     Time EP as Quantum
//
  // Fill local cache for this column if cache not valid
  if (connectedToIter()) {
    if (!timeEPQuantOK_p) {
      uInt nrow = calMainCol()->time().nrow();
      timeEPQuant_p.resize(nrow);
      for (uInt row=0; row < nrow; row++) {
	calMainCol()->timeEPQuant().get (row, timeEPQuant_p(row));
      };
      timeEPQuantOK_p = true;
    };
  };
  return timeEPQuant_p;
};

//----------------------------------------------------------------------------

Vector<Quantity>& CalMainBuffer::intervalQuant()
{
// INTERVAL data field accessor (as Quantum)
// Input from private data:
//    connectedToIter_p  Bool                 true if connected to iterator
//    intervalQuant_p    Vector<Quantity>     Interval as Quantum
//
  // Fill local cache for this column if cache not valid
  if (connectedToIter()) {
    if (!intervalQuantOK_p) {
      uInt nrow = calMainCol()->time().nrow();
      intervalQuant_p.resize(nrow);
      for (uInt row = 0; row < nrow; row++) {
	calMainCol()->intervalQuant().get (row, intervalQuant_p(row));
      };
      intervalQuantOK_p = true;
    };
  };
  return intervalQuant_p;
};

//----------------------------------------------------------------------------

Vector<Int>& CalMainBuffer::antenna1()
{
// ANTENNA1 data field accessor
// Input from private data:
//    connectedToIter_p  Bool                 true if connected to iterator
//    antenna1_p         Vector<Int>          Antenna1
//
  // Fill local cache for this column if cache not valid
  if (connectedToIter()) {
    if (!antenna1OK_p) {
      calMainCol()->antenna1().getColumn (antenna1_p);
      antenna1OK_p = true;
    };
  };
  return antenna1_p;
};

//----------------------------------------------------------------------------

Vector<Int>& CalMainBuffer::feed1()
{
// FEED1 data field accessor
// Input from private data:
//    connectedToIter_p  Bool                 true if connected to iterator
//    feed1_p            Vector<Int>          Feed1
//
  // Fill local cache for this column if cache not valid
  if (connectedToIter()) {
    if (!feed1OK_p) {
      calMainCol()->feed1().getColumn (feed1_p);
      feed1OK_p = true;
    };
  };
  return feed1_p;
};

//----------------------------------------------------------------------------

Vector<Int>& CalMainBuffer::fieldId()
{
// FIELD_ID data field accessor
// Input from private data:
//    connectedToIter_p  Bool                 true if connected to iterator
//    fieldId_p          Vector<Int>          Field id.
//
  // Fill local cache for this column if cache not valid
  if (connectedToIter()) {
    if (!fieldIdOK_p) {
      calMainCol()->fieldId().getColumn (fieldId_p);
      fieldIdOK_p = true;
    };
  };
  return fieldId_p;
};

//----------------------------------------------------------------------------

Vector<Int>& CalMainBuffer::arrayId()
{
// ARRAY_ID data field accessor
// Input from private data:
//    connectedToIter_p  Bool                 true if connected to iterator
//    arrayId_p          Vector<Int>          Array id.
//
  // Fill local cache for this column if cache not valid
  if (connectedToIter()) {
    if (!arrayIdOK_p) {
      calMainCol()->arrayId().getColumn (arrayId_p);
      arrayIdOK_p = true;
    };
  };
  return arrayId_p;
};

//----------------------------------------------------------------------------

Vector<Int>& CalMainBuffer::obsId()
{
// OBSERVATION_ID data field accessor
// Input from private data:
//    connectedToIter_p  Bool                 true if connected to iterator
//    obsId_p            Vector<Int>          Obs id.
//
  // Fill local cache for this column if cache not valid
  if (connectedToIter()) {
    if (!obsIdOK_p) {
      calMainCol()->obsId().getColumn (obsId_p);
      obsIdOK_p = true;
    };
  };
  return obsId_p;
};

//----------------------------------------------------------------------------

Vector<Int>& CalMainBuffer::scanNo()
{
// SCAN_NUMBER data field accessor
// Input from private data:
//    connectedToIter_p  Bool                 true if connected to iterator
//    scanNo_p           Vector<Int>          Scan no.
//
  // Fill local cache for this column if cache not valid
  if (connectedToIter()) {
    if (!scanNoOK_p) {
      calMainCol()->scanNo().getColumn (scanNo_p);
      scanNoOK_p = true;
    };
  };
  return scanNo_p;
};

//----------------------------------------------------------------------------

Vector<Int>& CalMainBuffer::processorId()
{
// PROCESSOR_ID data field accessor
// Input from private data:
//    connectedToIter_p  Bool                 true if connected to iterator
//    processorId_p      Vector<Int>          Processor id.
//
  // Fill local cache for this column if cache not valid
  if (connectedToIter()) {
    if (!processorIdOK_p) {
      calMainCol()->processorId().getColumn (processorId_p);
      processorIdOK_p = true;
    };
  };
  return processorId_p;
};

//----------------------------------------------------------------------------

Vector<Int>& CalMainBuffer::stateId()
{
// STATE_ID data field accessor
// Input from private data:
//    connectedToIter_p  Bool                 true if connected to iterator
//    stateId_p          Vector<Int>          State id.
//
  // Fill local cache for this column if cache not valid
  if (connectedToIter()) {
    if (!stateIdOK_p) {
      calMainCol()->stateId().getColumn (stateId_p);
      stateIdOK_p = true;
    };
  };
  return stateId_p;
};

//----------------------------------------------------------------------------

Vector<Int>& CalMainBuffer::phaseId()
{
// PHASE_ID data field accessor
// Input from private data:
//    connectedToIter_p  Bool                 true if connected to iterator
//    phaseId_p          Vector<Int>          Phase id.
//
  // Fill local cache for this column if cache not valid
  if (connectedToIter()) {
    if (!phaseIdOK_p) {
      calMainCol()->phaseId().getColumn (phaseId_p);
      phaseIdOK_p = true;
    };
  };
  return phaseId_p;
};

//----------------------------------------------------------------------------

Vector<Int>& CalMainBuffer::pulsarBin()
{
// PULSAR_BIN data field accessor
// Input from private data:
//    connectedToIter_p  Bool                 true if connected to iterator
//    pulsarBin_p        Vector<Int>          Pulsar bin
//
  // Fill local cache for this column if cache not valid
  if (connectedToIter()) {
    if (!pulsarBinOK_p) {
      calMainCol()->pulsarBin().getColumn (pulsarBin_p);
      pulsarBinOK_p = true;
    };
  };
  return pulsarBin_p;
};

//----------------------------------------------------------------------------

Vector<Int>& CalMainBuffer::pulsarGateId()
{
// PULSAR_GATE_ID data field accessor
// Input from private data:
//    connectedToIter_p  Bool                 true if connected to iterator
//    pulsarGateId_p     Vector<Int>          Pulsar gate id.
//
  // Fill local cache for this column if cache not valid
  if (connectedToIter()) {
    if (!pulsarGateIdOK_p) {
      calMainCol()->pulsarGateId().getColumn (pulsarGateId_p);
      pulsarGateIdOK_p = true;
    };
  };
  return pulsarGateId_p;
};

//----------------------------------------------------------------------------

Vector<Int>& CalMainBuffer::freqGrp()
{
// FREQ_GROUP data field accessor
// Input from private data:
//    connectedToIter_p  Bool                 true if connected to iterator
//    freqGrp_p          Vector<Int>          Frequency group
//
  // Fill local cache for this column if cache not valid
  if (connectedToIter()) {
    if (!freqGrpOK_p) {
      calMainCol()->freqGrp().getColumn (freqGrp_p);
      freqGrpOK_p = true;
    };
  };
  return freqGrp_p;
};

//----------------------------------------------------------------------------

Vector<String>& CalMainBuffer::freqGrpName()
{
// FREQ_GROUP_NAME data field accessor
// Input from private data:
//    connectedToIter_p  Bool                 true if connected to iterator
//    freqGrpName_p      Vector<String>       Frequency group name
//
  // Fill local cache for this column if cache not valid
  if (connectedToIter()) {
    if (!freqGrpNameOK_p) {
      calMainCol()->freqGrpName().getColumn (freqGrpName_p);
      freqGrpNameOK_p = true;
    };
  };
  return freqGrpName_p;
};

//----------------------------------------------------------------------------

Vector<String>& CalMainBuffer::fieldName()
{
// FIELD_NAME data field accessor
// Input from private data:
//    connectedToIter_p  Bool                 true if connected to iterator
//    fieldName_p        Vector<String>       Field name
//
  // Fill local cache for this column if cache not valid
  if (connectedToIter()) {
    if (!fieldNameOK_p) {
      calMainCol()->fieldName().getColumn (fieldName_p);
      fieldNameOK_p = true;
    };
  };
  return fieldName_p;
};

//----------------------------------------------------------------------------

Vector<String>& CalMainBuffer::fieldCode()
{
// FIELD_CODE data field accessor
// Input from private data:
//    connectedToIter_p  Bool                 true if connected to iterator
//    fieldCode_p        Vector<String>       Field code
//
  // Fill local cache for this column if cache not valid
  if (connectedToIter()) {
    if (!fieldCodeOK_p) {
      calMainCol()->fieldCode().getColumn (fieldCode_p);
      fieldCodeOK_p = true;
    };
  };
  return fieldCode_p;
};

//----------------------------------------------------------------------------

Vector<String>& CalMainBuffer::sourceName()
{
// SOURCE_NAME data field accessor
// Input from private data:
//    connectedToIter_p  Bool                 true if connected to iterator
//    sourceName_p       Vector<String>       Source name
//
  // Fill local cache for this column if cache not valid
  if (connectedToIter()) {
    if (!sourceNameOK_p) {
      calMainCol()->sourceName().getColumn (sourceName_p);
      sourceNameOK_p = true;
    };
  };
  return sourceName_p;
};

//----------------------------------------------------------------------------

Vector<String>& CalMainBuffer::sourceCode()
{
// SOURCE_CODE data field accessor
// Input from private data:
//    connectedToIter_p  Bool                 true if connected to iterator
//    sourceCode_p       Vector<String>       Source code
//
  // Fill local cache for this column if cache not valid
  if (connectedToIter()) {
    if (!sourceCodeOK_p) {
      calMainCol()->sourceCode().getColumn (sourceCode_p);
      sourceCodeOK_p = true;
    };
  };
  return sourceCode_p;
};

//----------------------------------------------------------------------------

Vector<Int>& CalMainBuffer::calGrp()
{
// CALIBRATION_GROUP data field accessor
// Input from private data:
//    connectedToIter_p  Bool                 true if connected to iterator
//    calGrp_p          Vector<Int>           Calibration group
//
  // Fill local cache for this column if cache not valid
  if (connectedToIter()) {
    if (!calGrpOK_p) {
      calMainCol()->calGrp().getColumn (calGrp_p);
      calGrpOK_p = true;
    };
  };
  return calGrp_p;
};

//----------------------------------------------------------------------------

Array<Complex>& CalMainBuffer::gain()
{
// GAIN data field accessor
// Input from private data:
//    connectedToIter_p  Bool                 true if connected to iterator
//    gain_p             Array<Complex>       Calibration gain
//
  // Fill local cache for this column if cache not valid
  if (connectedToIter()) {
    if (!gainOK_p) {
      calMainCol()->gain().getColumn (gain_p);
      gainOK_p = true;
    };
  };
  return gain_p;
};

//----------------------------------------------------------------------------

Array<Int>& CalMainBuffer::refAnt()
{
// REF_ANT data field accessor
// Input from private data:
//    connectedToIter_p  Bool                 true if connected to iterator
//    refAnt_p           Array<Int>           Reference antenna
//
  // Fill local cache for this column if cache not valid
  if (connectedToIter()) {
    if (!refAntOK_p) {
      calMainCol()->refAnt().getColumn (refAnt_p);
      refAntOK_p = true;
    };
  };
  return refAnt_p;
};

//----------------------------------------------------------------------------

Array<Int>& CalMainBuffer::refFeed()
{
// REF_FEED data field accessor
// Input from private data:
//    connectedToIter_p  Bool                 true if connected to iterator
//    refFeed_p          Array<Int>           Reference feed
//
  // Fill local cache for this column if cache not valid
  if (connectedToIter()) {
    if (!refFeedOK_p) {
      calMainCol()->refFeed().getColumn (refFeed_p);
      refFeedOK_p = true;
    };
  };
  return refFeed_p;
};

//----------------------------------------------------------------------------

Array<Int>& CalMainBuffer::refReceptor()
{
// REF_FEED data field accessor
// Input from private data:
//    connectedToIter_p  Bool                 true if connected to iterator
//    refReceptor_p      Array<Int>           Reference receptor
//
  // Fill local cache for this column if cache not valid
  if (connectedToIter()) {
    if (!refReceptorOK_p) {
      calMainCol()->refReceptor().getColumn (refReceptor_p);
      refReceptorOK_p = true;
    };
  };
  return refReceptor_p;
};

//----------------------------------------------------------------------------

Array<MFrequency>& CalMainBuffer::refFreqMeas()
{
// REF_FREQUENCY data field accessor (as Measure)
// Input from private data:
//    connectedToIter_p  Bool                 true if connected to iterator
//    refFreqMeas_p      Array<MFrequency>    Reference frequency as Measure
//
  // Fill local cache for this column if cache not valid
  if (connectedToIter()) {
    if (!refFreqMeasOK_p) {
      uInt nrow = calMainCol()->time().nrow();

      // Assume fixed shape for Array calibration columns
      Array<MFrequency> rowRefFreq;
      calMainCol()->refFreqMeas().get (0, rowRefFreq);
      IPosition rowShape = rowRefFreq.shape();
      rowShape.append (IPosition(1,nrow));
      refFreqMeas_p.resize(rowShape);
      IPosition blc = rowShape;
      blc = 0;
      IPosition trc = rowShape - 1;
      
      for (uInt row = 0; row < nrow; row++) {
	calMainCol()->refFreqMeas().get (row, rowRefFreq);
	blc.setLast(IPosition(1,row));
	trc.setLast(IPosition(1,row));
	refFreqMeas_p(blc,trc) = rowRefFreq.addDegenerate(1).copy();
      };
      refFreqMeasOK_p = true;
    };
  };
  return refFreqMeas_p;
};

//----------------------------------------------------------------------------

Vector<Int>& CalMainBuffer::measFreqRef()
{
// REF_FEED data field accessor
// Input from private data:
//    connectedToIter_p  Bool                 true if connected to iterator
//    measFreqRef_p      Vector<Int>          Reference frequency frame
//
  // Fill local cache for this column if cache not valid
  if (connectedToIter()) {
    if (!measFreqRefOK_p) {
      calMainCol()->measFreqRef().getColumn (measFreqRef_p);
      measFreqRefOK_p = true;
    };
  };
  return measFreqRef_p;
};

//----------------------------------------------------------------------------

Array<MDirection>& CalMainBuffer::refDirMeas()
{
// REF_DIRECTION data field accessor (as Measure)
// Input from private data:
//    connectedToIter_p  Bool                 true if connected to iterator
//    refDirMeas_p       Array<MDirection>    Reference direction as Measure
//
  // Fill local cache for this column if cache not valid
  if (connectedToIter()) {
    if (!refDirMeasOK_p) {
      uInt nrow = calMainCol()->time().nrow();

      // Assume fixed shape for Array calibration columns
      Array<MDirection> rowRefDir;
      calMainCol()->refDirMeas().get (0, rowRefDir);
      IPosition rowShape = rowRefDir.shape();
      rowShape.append (IPosition(1,nrow));
      refDirMeas_p.resize(rowShape);
      IPosition blc = rowShape;
      blc = 0;
      IPosition trc = rowShape - 1;
      
      for (uInt row = 0; row < nrow; row++) {
	calMainCol()->refDirMeas().get (row, rowRefDir);
	blc.setLast(IPosition(1,row));
	trc.setLast(IPosition(1,row));
	refDirMeas_p(blc,trc) = rowRefDir.addDegenerate(1).copy();
      };
      refDirMeasOK_p = true;
    };
  };
  return refDirMeas_p;
};

//----------------------------------------------------------------------------

Vector<Int>& CalMainBuffer::measDirRef()
{
// REF_DIRECTION data field accessor
// Input from private data:
//    connectedToIter_p  Bool                 true if connected to iterator
//    measDirRef_p      Vector<Int>           Reference direction frame
//
  // Fill local cache for this column if cache not valid
  if (connectedToIter()) {
    if (!measDirRefOK_p) {
      calMainCol()->measDirRef().getColumn (measDirRef_p);
      measDirRefOK_p = true;
    };
  };
  return measDirRef_p;
};

//----------------------------------------------------------------------------

Vector<Int>& CalMainBuffer::calDescId()
{
// CAL_DESC_ID data field accessor
// Input from private data:
//    connectedToIter_p  Bool                 true if connected to iterator
//    calDescId_p        Vector<Int>          Calibration format descriptor
//
  // Fill local cache for this column if cache not valid
  if (connectedToIter()) {
    if (!calDescIdOK_p) {
      calMainCol()->calDescId().getColumn (calDescId_p);
      calDescIdOK_p = true;
    };
  };
  return calDescId_p;
};

//----------------------------------------------------------------------------

Vector<Int>& CalMainBuffer::calHistoryId()
{
// CAL_HISTORY_ID data field accessor
// Input from private data:
//    connectedToIter_p  Bool                 true if connected to iterator
//    calHistoryId_p     Vector<Int>          Calibration history identifier
//
  // Fill local cache for this column if cache not valid
  if (connectedToIter()) {
    if (!calHistoryIdOK_p) {
      calMainCol()->calHistoryId().getColumn (calHistoryId_p);
      calHistoryIdOK_p = true;
    };
  };
  return calHistoryId_p;
};

//----------------------------------------------------------------------------

Vector<Int>& CalMainBuffer::asVecInt (const MSCalEnums::colDef& calEnum)
{
// Generic access to integer-type cal buffer columns
// Input:
//    calEnum     const MSCalEnums::colDef&   Calibration enum
// Output:
//    asVecInt    Vector<Int>&                Reference to the specified
//                                            calibration buffer column
//

  // Case cal enum of:
  switch (calEnum) {

  // ANTENNA1:
  case MSC::ANTENNA1: {
    return antenna1();
    break;
  };

  // FEED1
  case MSC::FEED1: {
    return feed1();
    break;
  };

  // FIELD_ID
  case MSC::FIELD_ID: {
    return fieldId();
    break;
  };

  // ARRAY_ID
  case MSC::ARRAY_ID: {
    return arrayId();
    break;
  };

  // OBSERVATION_ID
  case MSC::OBSERVATION_ID: {
    return obsId();
    break;
  };

  // SCAN_NUMBER
  case MSC::SCAN_NUMBER: {
    return scanNo();
    break;
  };

  // PROCESSOR_ID
  case MSC::PROCESSOR_ID: {
    return processorId();
    break;
  };

  // PHASE_ID
  case MSC::PHASE_ID: {
    return phaseId();
    break;
  };

  // STATE_ID
  case MSC::STATE_ID: {
    return stateId();
    break;
  };

  // PULSAR_BIN
  case MSC::PULSAR_BIN: {
    return pulsarBin();
    break;
  };

  // PULSAR_GATE_ID
  case MSC::PULSAR_GATE_ID: {
    return pulsarGateId();
    break;
  };

  // FREQ_GROUP
  case MSC::FREQ_GROUP: {
    return freqGrp();
    break;
  };

  // CALIBRATION_GROUP
  case MSC::CALIBRATION_GROUP: {
    return calGrp();
    break;
  };

  // MEAS_FREQ_REF
  case MSC::MEAS_FREQ_REF: {
    return measFreqRef();
    break;
  };

  // MEAS_DIR_REF
  case MSC::MEAS_DIR_REF: {
    return measDirRef();
    break;
  };

  // CAL_DESC_ID
  case MSC::CAL_DESC_ID: {
    return calDescId();
    break;
  };

  // CAL_HISTORY_ID
  case MSC::CAL_HISTORY_ID: {
    return calHistoryId();
    break;
  };

  // Unrecognized column enum
  default: {
    throw (AipsError("Unrecognized integer calibration column"));
  };
  };
};

//----------------------------------------------------------------------------

Vector<String>& CalMainBuffer::asVecString (const MSCalEnums::colDef& calEnum)
{
// Generic access to string-type cal buffer columns
// Input:
//    calEnum     const MSCalEnums::colDef&   Calibration enum
// Output:
//    asVecString Vector<String>&             Reference to the specified
//                                            calibration buffer column
//
  // Case cal enum of:
  switch (calEnum) {

  // FREQ_GROUP_NAME
  case MSC::FREQ_GROUP_NAME: {
    return freqGrpName();
    break;
  };

  // FIELD_NAME
  case MSC::FIELD_NAME: {
    return fieldName();
    break;
  };

  // FIELD_CODE
  case MSC::FIELD_CODE: {
    return fieldCode();
    break;
  };

  // SOURCE_NAME
  case MSC::SOURCE_NAME: {
    return sourceName();
    break;
  };

  // SOURCE_CODE
  case MSC::SOURCE_CODE: {
    return sourceCode();
    break;
  };

  // Unrecognized column enum
  default: {
    throw (AipsError("Unrecognized string calibration column"));
  };
  };
};

//----------------------------------------------------------------------------

Vector<Int> CalMainBuffer::matchAntenna1 (const Int& antennaId)
{
// Find matching rows for a given antenna 1 id.
// Input:
//    antennaId          const Int&             Antenna 1 id. to match
// Output:
//    matchAntenna1      Vector<Int>            Matching buffer row numbers
//
  // Do the match
  LogicalArray maskArray = (antenna1() == antennaId);
  Vector<Int> rowNo(nRow());
  indgen(rowNo);
  MaskedArray<Int> maskRowNo(rowNo, maskArray);
  return maskRowNo.getCompressedArray();
};


//----------------------------------------------------------------------------

Vector<Int> CalMainBuffer::matchAntenna1AndFieldId (const Int& antennaId,
						    const Int& fldId)
{
// Find matching rows for a given antenna 1 id. and field id. pair
// Input:
//    antennaId                 const Int&      Antenna 1 id. to match
//    fldId                     const Int&      Field id. to match
// Output:
//    matchAntenna1AndFieldId   Vector<Int>     Matching buffer row numbers
//
  // Do the match
  LogicalArray maskArray = (antenna1() == antennaId && fieldId() == fldId);
  Vector<Int> rowNo(nRow());
  indgen(rowNo);
  MaskedArray<Int> maskRowNo(rowNo, maskArray);
  Vector<Int> tmp(maskRowNo.getCompressedArray());
  return tmp;
};

//----------------------------------------------------------------------------

Vector<Int> CalMainBuffer::matchAntenna1AndFreqGrp (const Int& antennaId,
						    const String& frqGrpName)
{
// Find matching rows for a given antenna 1 id. and freq. group name
// Input:
//    antennaId                 const Int&      Antenna 1 id. to match
//    frqGrpName                const String&   Freq. group name to match
// Output:
//    matchAntenna1AndFreqGrp   Vector<Int>     Matching buffer row numbers
//
  // Do the match
  LogicalArray maskArray = (antenna1() == antennaId && 
			    freqGrpName() == frqGrpName);
  Vector<Int> rowNo(nRow());
  indgen(rowNo);
  MaskedArray<Int> maskRowNo(rowNo, maskArray);
  return maskRowNo.getCompressedArray();
};

//----------------------------------------------------------------------------

void CalMainBuffer::invalidate()
{
// Invalidate the current cache
// Output to private data:
//    timeMeasOK_p       Bool                 Time as a Measure cache ok
//    timeEPQuantOK_p    Bool                 EP time as Quantum cache ok
//    intervalQuantOK_p  Bool                 Time interval as Quantum cache ok
//    antenna1OK_p       Bool                 Antenna 1 cache ok
//    feed1OK_p          Bool                 Feed 1 cache ok
//    fieldIdOK_p        Bool                 Field id. cache ok
//    arrayIdOK_p        Bool                 Array id. cache ok
//    obsIdOK_p          Bool                 Observation id. cache ok
//    scanNoOK_p         Bool                 Scan no. cache ok
//    processorIdOK_p    Bool                 Processor id. cache ok
//    stateIdOK_p        Bool                 State id. cache ok
//    phaseIdOK_p        Bool                 Phase id. cache ok
//    pulsarBinOK_p      Bool                 Pulsar bin cache ok
//    pulsarGateIdOK_p   Bool                 Pulsar gate id. cache ok
//    freqGrpOK_p        Bool                 Frequency group cache ok
//    freqGrpNameOK_p    Bool                 Frequency group name cache ok
//    fieldNameOK_p      Bool                 Field name cache ok
//    fieldCodeOK_p      Bool                 Field code cache ok
//    sourceNameOK_p     Bool                 Source name cache ok
//    sourceCodeOK_p     Bool                 Source code cache ok
//    calGrpOK_p         Bool                 Calibration group cache ok
//    gainOK_p           Bool                 Gain cache ok
//    refAntOK_p         Bool                 Reference antenna cache ok
//    refFeedOK_p        Bool                 Reference feed cache ok
//    refReceptorOK_p    Bool                 Reference receptor cache ok
//    refFreqMeasOK_p    Bool                 Ref. freq. as Measure cache ok
//    measFreqRefOK_p    Bool                 Frequency measures ref. cache ok
//    refDirMeasOK_p     Bool                 Ref. dir. as Measure cache ok
//    measDirRefOK_p     Bool                 Direction measures ref. cache ok
//    calDescIdOK_p      Bool                 CAL_DESC id. cache ok
//    calHistoryIdOK_p   Bool                 CAL_HISTORY id. cache ok
//
  // Set all cache flags to false
  timeMeasOK_p = false;
  timeEPQuantOK_p = false;
  intervalQuantOK_p = false;
  antenna1OK_p = false;
  feed1OK_p = false;
  fieldIdOK_p = false;
  arrayIdOK_p = false;
  obsIdOK_p = false;
  scanNoOK_p = false;
  processorIdOK_p = false;
  stateIdOK_p = false;
  phaseIdOK_p = false;
  pulsarBinOK_p = false;
  pulsarGateIdOK_p = false;
  freqGrpOK_p = false;
  freqGrpNameOK_p = false;
  fieldNameOK_p = false;
  fieldCodeOK_p = false;
  sourceNameOK_p = false;
  sourceCodeOK_p = false;
  calGrpOK_p = false;
  gainOK_p = false;
  refAntOK_p = false;
  refFeedOK_p = false;
  refReceptorOK_p = false;
  refFreqMeasOK_p = false;
  measFreqRefOK_p = false;
  refDirMeasOK_p = false;
  measDirRefOK_p = false;
  calDescIdOK_p = false;
  calHistoryIdOK_p = false;
};

//----------------------------------------------------------------------------

void CalMainBuffer::fillIndices (const Vector<Int>& calIndices,
				 const VisBuffer& vb)
{
// 
// Input:
//    calIndices         const Vector<Int>&   Vector of cal indices, specified
//                                            as enums from class MSCalEnums
//    vb                 const VisBuffer&     Visibility data buffer
//
  // Number of input cal indices
  Int nCalIndices = calIndices.nelements();

  if (nCalIndices > 0) {
    // Extract the index ranges from the visibility buffer for each index
    Block <Vector<Int> > indexValues(nCalIndices);
    for (Int i=0; i < nCalIndices; i++) {
      MSC::colDef calEnum = static_cast<MSCalEnums::colDef> (calIndices(i));
      // This method requires indices of integer type
      if (MSC::basicType(calEnum) == TpInt) {
	// Case index type of:
	switch (calEnum) {
	  // ANTENNA1: (requires reading ANTENNA1 and ANTENNA2)
	case MSC::ANTENNA1: 
	  indexValues[i] = vb.antIdRange();
	  break;
	  // Other integer columns
	default:
	  indexValues[i] = vb.vecIntRange(calEnum);
	  break;
	};

      } else {
	throw AipsError("Calibration buffer initialization error");
      };
    };
    
    // Update the indices in the cal buffer
    fillIndices(calIndices, indexValues);
  };
  return;
};


//----------------------------------------------------------------------------

void CalMainBuffer::fillIndices (const Vector<Int>& calIndices,
				 const Block<Vector<Int> >& indexValues)
{
// 
// Input:
//    calIndices    const Vector<Int>&       Vector of cal indices, specified
//                                           as enums from class MSCalEnums
//    indexValues   const Block<Vec<Int>>&   Index ranges
//
// Output to private data:
//    Possibly several integer columns (via CalMainBuffer::asVecInt()).
//
  // Number of mapped cal indices
  Int nMap = indexValues.nelements();
  if (nMap > 0) {

    // Fill the cal buffer index columns with all permutations of
    // the cal indices. First compute the product of all index
    // vector lengths up to each index number (minus one)
    Vector<Int> nProd(nMap+1);
    nProd(0) = 1;
    for (Int i=1; i < (nMap+1); i++) {
      nProd(i) = nProd(i-1) * indexValues[i-1].nelements();
    };

    // Determine the permuted indices for this row, and update
    // each one in the cal buffer
    for (Int row=0; row < nProd(nMap); row++) {
      Int reduce = row;
      for (Int j=nMap-1; j >= 0; j--) {
	Int index = reduce / nProd(j);
	reduce = reduce - index * nProd(j);
	// Update this index in the cal buffer
	MSCalEnums::colDef calEnum = 
	  static_cast<MSCalEnums::colDef> (calIndices(j));
	Vector<Int>& ref = asVecInt(calEnum);
	if (ref.nelements() < static_cast<uInt>(row+1)) {
	  ref.resize (row+1, true);
	};
	asVecInt(calEnum)(row) = indexValues[j](index);
      };
    };
  };
  return;
};

//----------------------------------------------------------------------------

void CalMainBuffer::fillAttributes (const Vector<Int>& calIndices,
				    const VisBuffer& vb)
{
// 
// Input:
//    calIndices         const Vector<Int>&   Vector of cal indices, specified
//                                            as enums from class MSCalEnums,
//                                            to exclude
//    vb                 const VisBuffer&     Visibility data buffer
// Output to private data:
//    timeMeas_p         Vector<MEpoch>       Time as a Measure
//    timeEPQuant_p      Vector<Quantity>     Extended prec. time as Quantum
//    intervalQuant_p    Vector<Quantity>     Time interval as Quantum
//    antenna1_p         Vector<Int>          Antenna 1
//    feed1_p            Vector<Int>          Feed 1
//    fieldId_p          Vector<Int>          Field id.
//    arrayId_p          Vector<Int>          Array id.
//    obsId_p            Vector<Int>          Observation id.
//    scanNo_p           Vector<Int>          Scan no.
//    processorId_p      Vector<Int>          Processor id.
//    stateId_p          Vector<Int>          State id.
//    phaseId_p          Vector<Int>          Phase id.
//    pulsarBin_p        Vector<Int>          Pulsar bin
//    pulsarGateId_p     Vector<Int>          Pulsar gate id.
//    freqGrp_p          Vector<Int>          Frequency group
//    freqGrpName_p      Vector<String>       Frequency group name
//    fieldName_p        Vector<String>       Field name
//    fieldCode_p        Vector<String>       Field code
//    sourceName_p       Vector<String>       Source name
//    sourceCode_p       Vector<String>       Source code
//    calGrp_p           Vector<Int>          Calibration group
//    gain_p             Array<Complex>       Gain
//    refAnt_p           Array<Int>           Reference antenna
//    refFeed_p          Array<Int>           Reference feed
//    refReceptor_p      Array<Int>           Reference receptor
//    refFreqMeas_p      Array<MFrequency>    Reference frequency as Measure
//    measFreqRef_p      Vector<Int>          Frequency measures ref.
//    refDirMeas_p       Array<MDirection>    Reference direction as Measure
//    measDirRef_p       Vector<Int>          Direction measures ref.
//    calDescId_p        Vector<Int>          CAL_DESC id.
//    calHistoryId_p     Vector<Int>          CAL_HISTORY id.
//
  // Resize all attribute columns, and set to their default values
  fillAttributes(calIndices);

  // Process each cal buffer column separately
  //
  // TIME, TIME_EXTRA_PREC and INTERVAL
  MEpoch time;
  MVEpoch timeEP, interval;
  if (vb.timeRange(time, timeEP, interval)) {
    if (!excluded(MSC::TIME, calIndices)) {
      timeMeas() = time;
    };
    if (!excluded(MSC::TIME_EXTRA_PREC, calIndices)) {
      timeEPQuant() = timeEP.getTime("s");
    };
    if (!excluded(MSC::INTERVAL, calIndices)) {
      intervalQuant() = interval.getTime("s");
    };
  } else {
    throw(AipsError("Invalid TIME column in uv-data buffer"));
  };

  // ANTENNA1, FEED1, FIELD_ID, ARRAY_ID, OBSERVATION_ID, SCAN_NUMBER,
  // PROCESSOR_ID, PHASE_ID, STATE_ID, PULSAR_BIN, PULSAR_GATE_ID
  // (all of Int type)
  Vector<Int> colInt(13);
  colInt(0) = MSC::ANTENNA1;
  colInt(1) = MSC::FEED1;
  colInt(2) = MSC::FIELD_ID;
  colInt(3) = MSC::ARRAY_ID;
  colInt(4) = MSC::OBSERVATION_ID;
  colInt(5) = MSC::SCAN_NUMBER;
  colInt(6) = MSC::PROCESSOR_ID;
  colInt(7) = MSC::PHASE_ID;
  colInt(8) = MSC::STATE_ID;
  colInt(9) = MSC::PULSAR_BIN;
  colInt(10) = MSC::PULSAR_GATE_ID;
  colInt(11) = MSC::FREQ_GROUP;
  colInt(12) = MSC::CALIBRATION_GROUP;
  
  for (uInt i=0; i < colInt.nelements(); i++) {
    MSCalEnums::colDef calEnum = static_cast<MSCalEnums::colDef>(colInt(i));
    if (!excluded(calEnum, calIndices)) {
      Vector<Int> range = vb.vecIntRange(calEnum);
      if (range.nelements() == 1) {
	// Single unique value found
	asVecInt(calEnum) = range(0);
      } else {
	// Multiple values; denote by -1
	asVecInt(calEnum) = -1;
      };
    };
  };

  return;
};

//----------------------------------------------------------------------------

void CalMainBuffer::fillAttributes (const Vector<Int>& calIndices)
{
// Resize all attribute columns and set to their default values
// Input:
//    calIndices         const Vector<Int>&   Vector of cal indices, specified
//                                            as enums from class MSCalEnums,
//                                            to exclude
// Output to private data:
//    Possibly several integer columns (via CalMainBuffer::asVecInt()).
//
  // Use the maximum number of rows currently defined in the cal buffer
  uInt nrow = nRow();

  // Process each cal buffer column separately
  //
  // TIME
  timeMeas().resize(nrow);
  timeMeas() = MEpoch(Quantity(0, "s"));

  // TIME_EXTRA_PREC
  timeEPQuant().resize(nrow);
  timeEPQuant() = Quantity(0, "s");

  // INTERVAL
  intervalQuant().resize(nrow);
  intervalQuant() = Quantity(0, "s");

  // ANTENNA1, FEED1, FIELD_ID, ARRAY_ID, OBSERVATION_ID, SCAN_NUMBER,
  // PROCESSOR_ID, PHASE_ID, STATE_ID, PULSAR_BIN, PULSAR_GATE_ID, 
  // FREQ_GROUP, CALIBRATION_GROUP, CAL_DESC_ID and CAL_HISTORY_ID
  // (all of Int type)
  Vector<Int> colInt(15);
  colInt(0) = MSC::ANTENNA1;
  colInt(1) = MSC::FEED1;
  colInt(2) = MSC::FIELD_ID;
  colInt(3) = MSC::ARRAY_ID;
  colInt(4) = MSC::OBSERVATION_ID;
  colInt(5) = MSC::SCAN_NUMBER;
  colInt(6) = MSC::PROCESSOR_ID;
  colInt(7) = MSC::PHASE_ID;
  colInt(8) = MSC::STATE_ID;
  colInt(9) = MSC::PULSAR_BIN;
  colInt(10) = MSC::PULSAR_GATE_ID;
  colInt(11) = MSC::FREQ_GROUP;
  colInt(12) = MSC::CALIBRATION_GROUP;
  colInt(13) = MSC::CAL_DESC_ID;
  colInt(14) = MSC::CAL_HISTORY_ID;
  
  for (uInt i=0; i < colInt.nelements(); i++) {
    MSCalEnums::colDef calEnum = static_cast<MSCalEnums::colDef>(colInt(i));
    if (!excluded(calEnum, calIndices)) {
      asVecInt(calEnum).resize(nrow);
      asVecInt(calEnum) = -1;
    };
  };

  // FREQ_GROUP_NAME, FIELD_NAME, FIELD_CODE, SOURCE_NAME and SOURCE_CODE
  // (all of String type)
  Vector<Int> colString(5);
  colString(0) = MSC::FREQ_GROUP_NAME;
  colString(1) = MSC::FIELD_NAME;
  colString(2) = MSC::FIELD_CODE;
  colString(3) = MSC::SOURCE_NAME;
  colString(4) = MSC::SOURCE_CODE;

  for (uInt i=0; i < colString.nelements(); i++) {
    MSCalEnums::colDef calEnum = static_cast<MSCalEnums::colDef>(colString(i));
    if (!excluded(calEnum, calIndices)) {
      asVecString(calEnum).resize(nrow);
      asVecString(calEnum) = "";
    };
  };

  // Array-based columns (set to default unit length in each dimension)
  uInt numSpw = 1;
  uInt numChan = 1;
  uInt numReceptors = 1;
  uInt nJones = 1;
  IPosition sizeA(5, nJones, nJones, numSpw, numChan, nrow);
  IPosition sizeB(4, numReceptors, numSpw, numChan, nrow);

  // GAIN
  gain().resize(sizeA);
  gain() = 0;

  // REF_ANT
  refAnt().resize(sizeB);
  refAnt() = 0;

  // REF_FEED
  refFeed().resize(sizeB);
  refFeed() = 0;
  
  // REF_RECEPTOR
  refReceptor().resize(sizeB);
  refReceptor() = 0;

  // REF_FREQUENCY and MEAS_FREQ_REF
  refFreqMeas().resize(sizeB);
  refFreqMeas() = MFrequency(Quantity(0, "Hz"));
  measFreqRef().resize(nrow);
  measFreqRef() = 0;
  
  // REF_DIRECTION and MEAS_DIR_REF
  refDirMeas().resize(sizeB);
  refDirMeas() = MDirection();
  measDirRef().resize(nrow);
  measDirRef() = 0;

  return;
};

//----------------------------------------------------------------------------

Bool CalMainBuffer::excluded (const MSCalEnums::colDef& calEnum, 
			      const Vector<Int>& excludeIndices)
{
// 
// Input:
//    calEnum     const MSCalEnums::colDef&  Calibration enum (from MSCalEnums)
//    calIndices  const Vector<Int>&         Vector of excluded calibration
//                                           enums (from class MSCalEnums)
// Output:
//    excluded    Bool                       true if enum is present in the
//                                           list of excluded enums
//
  // Check for membership
  Int calEnumInt = static_cast<Int>(calEnum);
  return anyEQ(excludeIndices, calEnumInt);
};

//----------------------------------------------------------------------------

} //# NAMESPACE CASA - END

