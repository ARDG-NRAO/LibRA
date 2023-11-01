//# CalMainBuffer.h: Calibration main table buffer
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

#ifndef CALIBRATION_CALMAINBUFFER_H
#define CALIBRATION_CALMAINBUFFER_H

#include <synthesis/CalTables/CalDescBuffer.h>
#include <synthesis/CalTables/CalHistoryBuffer.h>
#include <synthesis/CalTables/CalTable.h>
#include <synthesis/CalTables/CalMainColumns.h>
#include <msvis/MSVis/MSCalEnums.h>
#include <msvis/MSVis/VisBuffer.h>
#include <synthesis/CalTables/CalIterBase.h>

namespace casa { //# NAMESPACE CASA - BEGIN

// <summary> 
// CalMainBuffer: Calibration main table buffer
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//   <li> <linkto class="CalTable">CalTable</linkto> module
//   <li> <linkto class="CalIterBase">CalIterBase</linkto> module
// </prerequisite>
//
// <etymology>
// From "calibration main table" and "buffer"
// </etymology>
//
// <synopsis>
// The CalMainBuffer class holds a buffer, optionally connected to a 
// calibration main table iterator (of base type CalIterBase). 
// Specializations for baseline-based, time-variable and solvable 
// VisJones calibration table formats are provided through 
// inheritance.
// </synopsis>
//
// <example>
// <srcblock>
// </srcblock>
// </example>
//
// <motivation>
// Encapsulate calibration main table data buffers.
// </motivation>
//
// <todo asof="01/08/01">
// (i) Deal with non-standard columns.
// </todo>

class CalMainBuffer 
{
 public:
  // Default constructor. No connection to an underlying
  // calibration table iterator in this case.
  CalMainBuffer();

  // Construct from a set of calibration buffer indices and
  // their specified values. Non-index columns will be set 
  // to default values. There is no connection to an underlying 
  // calibration table iterator in this case.
  CalMainBuffer (const casacore::Vector<casacore::Int>& calIndices, 
		 const casacore::Block<casacore::Vector<casacore::Int> >& indexValues);

  // Construct from a calibration table iterator. The calibration
  // buffer will remain synchronized with the iterator.
  CalMainBuffer (CalIterBase& calIter);

  // Default destructor
  virtual ~CalMainBuffer();

  // Synchronize the buffer (if attached to an iterator)
  virtual void synchronize();

  // Force an explicit read to cache for all columns 
  // (if attached to an iterator)
  virtual void fillCache();

  // Write the current buffer at the end of a specified cal table;
  // returns the number of rows appended.
  virtual casacore::Int append (CalTable& calTable);

  // Maximum number of rows in the calibration buffer
  virtual casacore::Int nRow();

  // Accessors for the contained cal_desc and cal_history buffers
  virtual CalDescBuffer& calDescBuffer() {return *calDescBuf_p;};
  virtual CalHistoryBuffer& calHistoryBuffer() {return *calHistBuf_p;};

  // casacore::Data field accessors
  virtual casacore::Vector<casacore::MEpoch>& timeMeas();
  virtual casacore::Vector<casacore::Quantity>& timeEPQuant();
  virtual casacore::Vector<casacore::Quantity>& intervalQuant();
  virtual casacore::Vector<casacore::Int>& antenna1();
  virtual casacore::Vector<casacore::Int>& feed1();
  virtual casacore::Vector<casacore::Int>& fieldId();
  virtual casacore::Vector<casacore::Int>& arrayId();
  virtual casacore::Vector<casacore::Int>& obsId();
  virtual casacore::Vector<casacore::Int>& scanNo();
  virtual casacore::Vector<casacore::Int>& processorId();
  virtual casacore::Vector<casacore::Int>& stateId();
  virtual casacore::Vector<casacore::Int>& phaseId();
  virtual casacore::Vector<casacore::Int>& pulsarBin();
  virtual casacore::Vector<casacore::Int>& pulsarGateId();
  virtual casacore::Vector<casacore::Int>& freqGrp();
  virtual casacore::Vector<casacore::String>& freqGrpName();
  virtual casacore::Vector<casacore::String>& fieldName();
  virtual casacore::Vector<casacore::String>& fieldCode();
  virtual casacore::Vector<casacore::String>& sourceName();
  virtual casacore::Vector<casacore::String>& sourceCode();
  virtual casacore::Vector<casacore::Int>& calGrp();
  virtual casacore::Array<casacore::Complex>& gain();
  virtual casacore::Array<casacore::Int>& refAnt();
  virtual casacore::Array<casacore::Int>& refFeed();
  virtual casacore::Array<casacore::Int>& refReceptor();
  virtual casacore::Array<casacore::MFrequency>& refFreqMeas();
  virtual casacore::Vector<casacore::Int>& measFreqRef();
  virtual casacore::Array<casacore::MDirection>& refDirMeas();
  virtual casacore::Vector<casacore::Int>& measDirRef();
  virtual casacore::Vector<casacore::Int>& calDescId();
  virtual casacore::Vector<casacore::Int>& calHistoryId(); 

  // Generic access to integer columns by MSCalEnums enumeration. Throws
  // an exception if the column is not recognized or is not an integer column.
  virtual casacore::Vector<casacore::Int>& asVecInt (const MSCalEnums::colDef& calEnum);

  // Generic access to string columns by MSCalEnums enumeration. Throws
  // an exception if the column is not recognized or is not a string column.
  virtual casacore::Vector<casacore::String>& asVecString (const MSCalEnums::colDef& calEnum);

  // Find the matching rows for a given antenna1 id.
  virtual casacore::Vector<casacore::Int> matchAntenna1 (const casacore::Int& antennaId);

  // Find the matching rows for a given antenna1 id. and field id. pair
  virtual casacore::Vector<casacore::Int> matchAntenna1AndFieldId (const casacore::Int& antennaId,
					       const casacore::Int& fldId);

  // Find the matching rows for a given antenna1 id. and freq. group name
  virtual casacore::Vector<casacore::Int> matchAntenna1AndFreqGrp (const casacore::Int& antennaId,
					       const casacore::String& frqGrpName);

 protected:
  // Factory method to create a columns accessor object of the appropriate type
  virtual CalMainColumns* newCalMainCol (CalTable& calTable)
    {return new CalMainColumns (calTable);};

  // Access to the columns accessor object
  virtual CalMainColumns* calMainCol() {return calMainCol_p;};

  // Is the buffer connected to an underlying iterator ?
  casacore::Bool connectedToIter() {return connectedToIter_p;};

  // Invalidate the current cache. 
  virtual void invalidate();

  // <group>
  // Span an empty cal buffer by taking all permutations of the
  // values of a specified set of cal indices, specified as
  // enums from class MSCalEnums:
  //
  // Use a visibility buffer to define the index values 
  virtual void fillIndices (const casacore::Vector<casacore::Int>& calIndices,
			    const VisBuffer& vb);

  // Define the index values directly
  virtual void fillIndices (const casacore::Vector<casacore::Int>& calIndices,
			    const casacore::Block<casacore::Vector<casacore::Int> >& indexValues);
  // </group>

  // <group>
  // Fill the cal buffer attribute columns in an empty cal buffer,
  // after the cal indices have been set using fillIndices(). The 
  // cal indices, specified as enums from class MSCalEnums, are excluded
  // as non-attribute columns.
  //
  // Use a visibility buffer to define the attribute values
  virtual void fillAttributes(const casacore::Vector<casacore::Int>& calIndices,
			      const VisBuffer& vb);

  // Set default attribute values
  virtual void fillAttributes(const casacore::Vector<casacore::Int>& calIndices);

  // </group>

  // Utility function to test for membership in a list of calibration enums
  casacore::Bool excluded(const MSCalEnums::colDef& calEnum, 
		const casacore::Vector<casacore::Int>& excludeIndices);

 private:
  // true if connected to underlying iterator
  casacore::Bool connectedToIter_p;

  // Ptr to calibration table iterator
  CalIterBase* calIter_p;

  // Ptr to cal main columns accessor
  CalMainColumns* calMainCol_p;

  // Contained sub-table cal_desc and cal_history buffers
  CalDescBuffer* calDescBuf_p;
  CalHistoryBuffer* calHistBuf_p;

  // Buffer fields
  casacore::Vector<casacore::MEpoch> timeMeas_p;
  casacore::Vector<casacore::Quantity> timeEPQuant_p;
  casacore::Vector<casacore::Quantity> intervalQuant_p;
  casacore::Vector<casacore::Int> antenna1_p;
  casacore::Vector<casacore::Int> feed1_p;
  casacore::Vector<casacore::Int> fieldId_p;
  casacore::Vector<casacore::Int> arrayId_p;
  casacore::Vector<casacore::Int> obsId_p;
  casacore::Vector<casacore::Int> scanNo_p;
  casacore::Vector<casacore::Int> processorId_p;
  casacore::Vector<casacore::Int> stateId_p;
  casacore::Vector<casacore::Int> phaseId_p;
  casacore::Vector<casacore::Int> pulsarBin_p;
  casacore::Vector<casacore::Int> pulsarGateId_p;
  casacore::Vector<casacore::Int> freqGrp_p;
  casacore::Vector<casacore::String> freqGrpName_p;
  casacore::Vector<casacore::String> fieldName_p;
  casacore::Vector<casacore::String> fieldCode_p;
  casacore::Vector<casacore::String> sourceName_p;
  casacore::Vector<casacore::String> sourceCode_p;
  casacore::Vector<casacore::Int> calGrp_p;
  casacore::Array<casacore::Complex> gain_p;
  casacore::Array<casacore::Int> refAnt_p;
  casacore::Array<casacore::Int> refFeed_p;
  casacore::Array<casacore::Int> refReceptor_p;
  casacore::Array<casacore::MFrequency> refFreqMeas_p;
  casacore::Vector<casacore::Int> measFreqRef_p;
  casacore::Array<casacore::MDirection> refDirMeas_p;
  casacore::Vector<casacore::Int> measDirRef_p;
  casacore::Vector<casacore::Int> calDescId_p;
  casacore::Vector<casacore::Int> calHistoryId_p;

  // Buffer field status flags
  casacore::Bool timeMeasOK_p;
  casacore::Bool timeEPQuantOK_p;
  casacore::Bool intervalQuantOK_p;
  casacore::Bool antenna1OK_p;
  casacore::Bool feed1OK_p;
  casacore::Bool fieldIdOK_p;
  casacore::Bool arrayIdOK_p;
  casacore::Bool obsIdOK_p;
  casacore::Bool scanNoOK_p;
  casacore::Bool processorIdOK_p;
  casacore::Bool stateIdOK_p;
  casacore::Bool phaseIdOK_p;
  casacore::Bool pulsarBinOK_p;
  casacore::Bool pulsarGateIdOK_p;
  casacore::Bool freqGrpOK_p;
  casacore::Bool freqGrpNameOK_p;
  casacore::Bool fieldNameOK_p;
  casacore::Bool fieldCodeOK_p;
  casacore::Bool sourceNameOK_p;
  casacore::Bool sourceCodeOK_p;
  casacore::Bool calGrpOK_p;
  casacore::Bool gainOK_p;
  casacore::Bool refAntOK_p;
  casacore::Bool refFeedOK_p;
  casacore::Bool refReceptorOK_p;
  casacore::Bool refFreqMeasOK_p;
  casacore::Bool measFreqRefOK_p;
  casacore::Bool refDirMeasOK_p;
  casacore::Bool measDirRefOK_p;
  casacore::Bool calDescIdOK_p;
  casacore::Bool calHistoryIdOK_p;
};


} //# NAMESPACE CASA - END

#endif
   
  



