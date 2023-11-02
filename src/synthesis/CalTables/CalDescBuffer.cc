//# CalDescBuffer.cc: Implementation of CalDescBuffer.h
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

#include <synthesis/CalTables/CalDescBuffer.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

//----------------------------------------------------------------------------

CalDescBuffer::CalDescBuffer() :
  connectedToIter_p(false), calDescCol_p(NULL)
{
// Null constructor
// Output to private data:
//    connectedToIter_p  Bool               true if connected to iterator
//    calDescCol_p       CalDescColumns*    Ptr to cal_desc col accessor
//    numSpw_p           Vector<Int>        No. spw
//    numChan_p          Vector<Int>        No. chan
//    numReceptors_p     Vector<Int>        No. receptors
//    nJones_p           Vector<Int>        Jones matrix size
//    spwId_p            Matrix<Int>        Spw. id.'s
//    chanFreqMeas_p     Array<MFrequency>  Chan. freq. as Measure
//    measFreqRef_p      Vector<Int>        Freq. measures ref.
//    chanWidthQuant_p   Array<Quantity>    Chan. width as Quanta
//    chanRange_p        Array<Int>         Chan. range
//    polznType_p        Matrix<String>     Polzn. type
//    jonesType_p        Vector<String>     Jones matrix type
//    msName_p           Vector<String>     MeasurementSet name
//    numSpwOK_p         Bool               No. spw cache ok
//    numChanOK_p        Bool               No. chan cache ok
//    numReceptorsOK_p   Bool               No. receptors cache ok
//    nJonesOK_p         Bool               Jones matrix size cache ok
//    spwIdOK_p          Bool               Spw. id.'s cache ok
//    chanFreqMeasOK_p   Bool               Chan. freq. as Measure cache ok
//    measFreqRefOK_p    Bool               Freq. measures ref. cache ok
//    chanWidthQuantOK_p Bool               Chan. width as Quanta cache ok
//    chanRangeOK_p      Bool               Chan. range cache ok
//    polznTypeOK_p      Bool               Polzn. type cache ok
//    jonesTypeOK_p      Bool               Jones matrix type cache ok
//    msNameOK_p         Bool               MeasurementSet name cache ok
//
  // Invalidate cache 
  invalidate();
};

//----------------------------------------------------------------------------

CalDescBuffer::CalDescBuffer (CalIterBase& calIter) :
  connectedToIter_p(true), calDescCol_p(NULL)
{
// Construct from a calibration table iterator
// Input:
//    calIter            CalIterBase&         Calibration table iterator
// Output to private data:
//    connectedToIter_p  Bool               true if connected to iterator
//    calDescCol_p       CalDescColumns*    Ptr to cal_desc col accessor
//    numSpw_p           Vector<Int>        No. spw
//    numChan_p          Vector<Int>        No. chan
//    numReceptors_p     Vector<Int>        No. receptors
//    nJones_p           Vector<Int>        Jones matrix size
//    spwId_p            Matrix<Int>        Spw. id.'s
//    chanFreqMeas_p     Array<MFrequency>  Chan. freq. as Measure
//    measFreqRef_p      Vector<Int>        Freq. measures ref.
//    chanWidthQuant_p   Array<Quantity>    Chan. width as Quanta
//    chanRange_p        Array<Int>         Chan. range
//    polznType_p        Matrix<String>     Polzn. type
//    jonesType_p        Vector<String>     Jones matrix type
//    msName_p           Vector<String>     MeasurementSet name
//    numSpwOK_p         Bool               No. spw cache ok
//    numChanOK_p        Bool               No. chan cache ok
//    numReceptorsOK_p   Bool               No. receptors cache ok
//    nJonesOK_p         Bool               Jones matrix size cache ok
//    spwIdOK_p          Bool               Spw. id.'s cache ok
//    chanFreqMeasOK_p   Bool               Chan. freq. as Measure cache ok
//    measFreqRefOK_p    Bool               Freq. measures ref. cache ok
//    chanWidthQuantOK_p Bool               Chan. width as Quanta cache ok
//    chanRangeOK_p      Bool               Chan. range cache ok
//    polznTypeOK_p      Bool               Polzn. type cache ok
//    jonesTypeOK_p      Bool               Jones matrix type cache ok
//    msNameOK_p         Bool               MeasurementSet name cache ok
//
  // Invalidate cache
  invalidate();

  // Construct the cal_desc column accessor
  calDescCol_p = newCalDescCol (calIter.subTable());
};

//----------------------------------------------------------------------------

CalDescBuffer::~CalDescBuffer()
{
// Destructor
// Output to private data:
//    calDescCol_p       CalDescColumns*      Ptr to cal_desc col accessor
//
  if (calDescCol_p) delete calDescCol_p;
};

//----------------------------------------------------------------------------

void CalDescBuffer::invalidate()
{
// Invalidate the current cache
// Output to private data:
//    numSpwOK_p         Bool               No. spw cache ok
//    numChanOK_p        Bool               No. chan cache ok
//    numReceptorsOK_p   Bool               No. receptors cache ok
//    nJonesOK_p         Bool               Jones matrix size cache ok
//    spwIdOK_p          Bool               Spw. id.'s cache ok
//    chanFreqMeasOK_p   Bool               Chan. freq. as Measure cache ok
//    measFreqRefOK_p    Bool               Freq. measures ref. cache ok
//    chanWidthQuantOK_p Bool               Chan. width as Quanta cache ok
//    chanRangeOK_p      Bool               Chan. range cache ok
//    polznTypeOK_p      Bool               Polzn. type cache ok
//    jonesTypeOK_p      Bool               Jones matrix type cache ok
//    msNameOK_p         Bool               MeasurementSet name cache ok
//
  // Set all cache flags to false
  numSpwOK_p = false;
  numChanOK_p = false;
  numReceptorsOK_p = false;
  nJonesOK_p = false;
  spwIdOK_p = false;
  chanFreqMeasOK_p = false;
  measFreqRefOK_p = false;
  chanWidthQuantOK_p = false;
  chanRangeOK_p = false;
  polznTypeOK_p = false;
  jonesTypeOK_p = false;
  msNameOK_p = false;
};

//----------------------------------------------------------------------------

Vector<Int>& CalDescBuffer::numSpw()
{
// NUM_SPW data field accessor
// Input from private data:
//    connectedToIter_p   Bool                 true if connected to iterator
//    numSpw_p            Vector<Int>          No. spw
//
  // Fill local cache for this column if cache not valid
  if (connectedToIter()) {
    if (!numSpwOK_p) {
      calDescCol()->numSpw().getColumn (numSpw_p);
      numSpwOK_p = true;
    };
  };
  return numSpw_p;
};

//----------------------------------------------------------------------------

Vector<Int>& CalDescBuffer::numChan()
{
// NUM_CHAN data field accessor
// Input from private data:
//    connectedToIter_p   Bool                 true if connected to iterator
//    numChan_p           Vector<Int>          No. chan
//
  // Fill local cache for this column if cache not valid
  if (connectedToIter()) {
    if (!numChanOK_p) {
      calDescCol()->numChan().getColumn (numChan_p);
      numChanOK_p = true;
    };
  };
  return numChan_p;
};

//----------------------------------------------------------------------------

Vector<Int>& CalDescBuffer::numReceptors()
{
// NUM_RECEPTORS data field accessor
// Input from private data:
//    connectedToIter_p    Bool                 true if connected to iterator
//    numReceptors_p       Vector<Int>          No. receptors
//
  // Fill local cache for this column if cache not valid
  if (connectedToIter()) {
    if (!numReceptorsOK_p) {
      calDescCol()->numReceptors().getColumn (numReceptors_p);
      numReceptorsOK_p = true;
    };
  };
  return numReceptors_p;
};

//----------------------------------------------------------------------------

Vector<Int>& CalDescBuffer::nJones()
{
// N_JONES data field accessor
// Input from private data:
//    connectedToIter_p  Bool                 true if connected to iterator
//    nJones_p           Vector<Int>          No. spw
//
  // Fill local cache for this column if cache not valid
  if (connectedToIter()) {
    if (!nJonesOK_p) {
      calDescCol()->nJones().getColumn (nJones_p);
      nJonesOK_p = true;
    };
  };
  return nJones_p;
};

//----------------------------------------------------------------------------

Matrix<Int>& CalDescBuffer::spwId()
{
// NUM_SPW data field accessor
// Input from private data:
//    connectedToIter_p  Bool                 true if connected to iterator
//    spwId_p            Vector<Int>          Spw. id.'s
//
  // Fill local cache for this column if cache not valid
  if (connectedToIter()) {
    if (!spwIdOK_p) {
      calDescCol()->spwId().getColumn (spwId_p);
      spwIdOK_p = true;
    };
  };
  return spwId_p;
};

//----------------------------------------------------------------------------

Array<MFrequency>& CalDescBuffer::chanFreqMeas()
{
// CHAN_FREQ data field accessor
// Input from private data:
//    connectedToIter_p  Bool                 true if connected to iterator
//    chanFreqMeas_p     Array<MFrequency>    Chan. freq. as Measure
//
  // Fill local cache for this column if cache not valid
  if (connectedToIter()) {
    if (!chanFreqMeasOK_p) {
      uInt nrow = calDescCol()->numSpw().nrow();

      // Assume fixed shape for Array calibration columns
      Array<MFrequency> rowChanFreq;
      calDescCol()->chanFreqMeas().get (0, rowChanFreq);
      IPosition rowShape = rowChanFreq.shape();
      rowShape.prepend (IPosition(1,nrow));
      chanFreqMeas_p.resize(rowShape);
      IPosition start = rowShape;
      
      for (uInt row = 0; row < nrow; row++) {
	start(0) = row;
	calDescCol()->chanFreqMeas().get (row, rowChanFreq);
	chanFreqMeas_p(start,rowShape) = rowChanFreq.copy();
      };
      chanFreqMeasOK_p = true;
    };
  };
  return chanFreqMeas_p;
};

//----------------------------------------------------------------------------

Vector<Int>& CalDescBuffer::measFreqRef()
{
// MEAS_FREQ_REF data field accessor
// Input from private data:
//    connectedToIter_p  Bool                 true if connected to iterator
//    measFreqRef_p      Vector<Int>          Measures freq. ref.
//
  // Fill local cache for this column if cache not valid
  if (connectedToIter()) {
    if (!measFreqRefOK_p) {
      calDescCol()->measFreqRef().getColumn (measFreqRef_p);
      measFreqRefOK_p = true;
    };
  };
  return measFreqRef_p;
};

//----------------------------------------------------------------------------

Array<Quantity>& CalDescBuffer::chanWidthQuant()
{
// CHAN_WIDTH data field accessor (as Quanta)
// Input from private data:
//    connectedToIter_p  Bool                 true if connected to iterator
//    chanWidthQuant_p   Array<Quantity>      Channel width as Quanta
//
  // Fill local cache for this column if cache not valid
  if (connectedToIter()) {
    if (!chanWidthQuantOK_p) {
      uInt nrow = calDescCol()->numSpw().nrow();

      // Assume fixed shape for Array calibration columns
      Array<Quantity> rowChanWidth;
      calDescCol()->chanWidthQuant().get (0, rowChanWidth);
      IPosition rowShape = rowChanWidth.shape();
      rowShape.prepend (IPosition(1,nrow));
      chanWidthQuant_p.resize(rowShape);
      IPosition start = rowShape;
      
      for (uInt row = 0; row < nrow; row++) {
	start(0) = row;
	calDescCol()->chanWidthQuant().get (row, rowChanWidth);
	chanWidthQuant_p(start,rowShape) = rowChanWidth.copy();
      };
      chanWidthQuantOK_p = true;
    };
  };
  return chanWidthQuant_p;
};

//----------------------------------------------------------------------------

Array<Int>& CalDescBuffer::chanRange()
{
// CHAN_RANGE data field accessor
// Input from private data:
//    connectedToIter_p  Bool                 true if connected to iterator
//    chanRange_p        Array<Int>           Chan. range
//
  // Fill local cache for this column if cache not valid
  if (connectedToIter()) {
    if (!chanRangeOK_p) {
      calDescCol()->chanRange().getColumn (chanRange_p);
      chanRangeOK_p = true;
    };
  };
  return chanRange_p;
};

//----------------------------------------------------------------------------

Matrix<String>& CalDescBuffer::polznType()
{
// POLARIZATION_TYPE data field accessor
// Input from private data:
//    connectedToIter_p  Bool                 true if connected to iterator
//    polznType_p        Vector<String>       Polzn. type
//
  // Fill local cache for this column if cache not valid
  if (connectedToIter()) {
    if (!polznTypeOK_p) {
      calDescCol()->polznType().getColumn (polznType_p);
      polznTypeOK_p = true;
    };
  };
  return polznType_p;
};

//----------------------------------------------------------------------------

Vector<String>& CalDescBuffer::jonesType()
{
// JONES_TYPE data field accessor
// Input from private data:
//    connectedToIter_p  Bool                 true if connected to iterator
//    jonesType_p           Vector<String>    Jones matrix type
//
  // Fill local cache for this column if cache not valid
  if (connectedToIter()) {
    if (!jonesTypeOK_p) {
      calDescCol()->jonesType().getColumn (jonesType_p);
      jonesTypeOK_p = true;
    };
  };
  return jonesType_p;
};

//----------------------------------------------------------------------------

Vector<String>& CalDescBuffer::msName()
{
// MS_NAME data field accessor
// Input from private data:
//    connectedToIter_p  Bool                 true if connected to iterator
//    msName_p           Vector<String>       MeasurementSet name
//
  // Fill local cache for this column if cache not valid
  if (connectedToIter()) {
    if (!msNameOK_p) {
      calDescCol()->msName().getColumn (msName_p);
      msNameOK_p = true;
    };
  };
  return msName_p;
};

//----------------------------------------------------------------------------

} //# NAMESPACE CASA - END

