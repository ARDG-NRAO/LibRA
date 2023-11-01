//# CalDescBuffer.h: Calibration table cal_desc buffer
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
//# Correspondence concerning AIPS++ should be adressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//#
//# $Id$

#ifndef CALIBRATION_CALDESCBUFFER_H
#define CALIBRATION_CALDESCBUFFER_H

#include <casacore/casa/Arrays/Matrix.h>
#include <synthesis/CalTables/CalIterBase.h>
#include <synthesis/CalTables/CalTable.h>
#include <synthesis/CalTables/CalDescColumns.h>

namespace casa { //# NAMESPACE CASA - BEGIN

// <summary> 
// CalDescBuffer: Calibration table cal_desc buffer
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//   <li> <linkto class="CalTable">CalTable</linkto> module
//   <li> <linkto class="CalIterBase">CalIterBase</linkto> module
// </prerequisite>
//
// <etymology>
// From "calibration description table" and "buffer"
// </etymology>
//
// <synopsis>
// The CalDescBuffer class holds a buffer, optionally connected to a 
// calibration table cal_desc iterator (of base type CalIterBase). 
// </synopsis>
//
// <example>
// <srcblock>
// </srcblock>
// </example>
//
// <motivation>
// Encapsulate calibration table cal_desc data buffers.
// </motivation>
//
// <todo asof="01/08/01">
// (i) Deal with non-standard columns.
// </todo>

class CalDescBuffer 
{
 public:
  // Default constructor. No connection to an underlying
  // calibration table iterator in this case.
  CalDescBuffer();

  // Construct from a calibration table iterator. The calibration
  // buffer will remain synchronized with the iterator.
  CalDescBuffer (CalIterBase& calIter);

  // Default destructor
  virtual ~CalDescBuffer();

  // Invalidate the current calibration table cal_desc buffer. 
  // This signals that a re-read is required as the iterator has advanced.
  virtual void invalidate();

  // Write the current buffer at the end of a specified calibration
  // table cal_desc subtable(NYI)
  virtual void append (CalTable& /*calTable*/) {};

  // casacore::Data field accessors
  casacore::Vector<casacore::Int>& numSpw();
  casacore::Vector<casacore::Int>& numChan();
  casacore::Vector<casacore::Int>& numReceptors();
  casacore::Vector<casacore::Int>& nJones();
  casacore::Matrix<casacore::Int>& spwId();
  casacore::Array<casacore::MFrequency>& chanFreqMeas();
  casacore::Vector<casacore::Int>& measFreqRef();
  casacore::Array<casacore::Quantity>& chanWidthQuant();
  casacore::Array<casacore::Int>& chanRange();
  casacore::Matrix<casacore::String>& polznType();
  casacore::Vector<casacore::String>& jonesType();
  casacore::Vector<casacore::String>& msName();

 protected:
  // Factory method to create a columns accessor object of the appropriate type
  virtual CalDescColumns* newCalDescCol (CalTable& calTable)
    {return new CalDescColumns (calTable);};

  // Access to the columns accessor object
  virtual CalDescColumns* calDescCol() {return calDescCol_p;};

  // Is the buffer connected to an underlying iterator ?
  casacore::Bool connectedToIter() {return connectedToIter_p;};

 private:
  // true if connected to underlying iterator
  casacore::Bool connectedToIter_p;

  // Ptr to cal main columns accessor
  CalDescColumns* calDescCol_p;

  // Buffer fields
  casacore::Vector<casacore::Int> numSpw_p;
  casacore::Vector<casacore::Int> numChan_p;
  casacore::Vector<casacore::Int> numReceptors_p;
  casacore::Vector<casacore::Int> nJones_p;
  casacore::Matrix<casacore::Int> spwId_p;
  casacore::Array<casacore::MFrequency> chanFreqMeas_p;
  casacore::Vector<casacore::Int> measFreqRef_p;
  casacore::Array<casacore::Quantity> chanWidthQuant_p;
  casacore::Array<casacore::Int> chanRange_p;
  casacore::Matrix<casacore::String> polznType_p;
  casacore::Vector<casacore::String> jonesType_p;
  casacore::Vector<casacore::String> msName_p;

  // Buffer field status flags
  casacore::Bool numSpwOK_p;
  casacore::Bool numChanOK_p;
  casacore::Bool numReceptorsOK_p;
  casacore::Bool nJonesOK_p;
  casacore::Bool spwIdOK_p;
  casacore::Bool chanFreqMeasOK_p;
  casacore::Bool measFreqRefOK_p;
  casacore::Bool chanWidthQuantOK_p;
  casacore::Bool chanRangeOK_p;
  casacore::Bool polznTypeOK_p;
  casacore::Bool jonesTypeOK_p;
  casacore::Bool msNameOK_p;
};


} //# NAMESPACE CASA - END

#endif
