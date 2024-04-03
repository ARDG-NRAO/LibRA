//# CalBuffer.h: Calibration table buffer
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
//# Correspondence concerning AIPS++ should be adressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//#
//# $Id$

#ifndef CALIBRATION_CALBUFFER_H
#define CALIBRATION_CALBUFFER_H

#include <synthesis/CalTables/CalIterBase.h>
#include <synthesis/CalTables/CalTable.h>
#include <synthesis/CalTables/CalMainColumns.h>

namespace casa { //# NAMESPACE CASA - BEGIN

// <summary> 
// CalBuffer: Calibration table buffer
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//   <li> <linkto class="CalTable">CalTable</linkto> module
//   <li> <linkto class="CalIterBase">CalIterBase</linkto> module
// </prerequisite>
//
// <etymology>
// From "calibration table" and "buffer"
// </etymology>
//
// <synopsis>
// The CalBuffer class holds a buffer, optinally connected to a 
// calibration table iterator (as derived from CalIterBase). 
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
// Encapsulate calibration table data buffers.
// </motivation>
//
// <todo asof="01/08/01">
// (i) Deal with non-standard columns.
// </todo>

class CalBuffer 
{
 public:
  // Default constructor. No connection to an underlying
  // calibration table iterator in this case.
  CalBuffer();

  // Construct from a calibration table iterator. The calibration
  // buffer will remain synchronized with the iterator.
  CalBuffer (CalIterBase& calIter);

  // Default destructor
  virtual ~CalBuffer();

  // Invalidate the current calibration buffer. This signals
  // that a re-read is required as the iterator has advanced.
  virtual void invalidate();

  // Write the current buffer at the end of a specified cal table (NYI)
  virtual void append (CalTable& /*calTable*/) {};

  // casacore::Data field accessors
  casacore::Vector<casacore::Double>& time();
  //  casacore::Vector<casacore::MEpoch>& timeMeas();
  casacore::Vector<casacore::Double>& timeEP();
  casacore::Vector<casacore::Double>& interval();
  casacore::Vector<casacore::Int>& antenna1();
  casacore::Vector<casacore::Int>& feed1();
  casacore::Vector<casacore::Int>& fieldId();
  casacore::Vector<casacore::Int>& arrayId();
  casacore::Vector<casacore::Int>& obsId();
  casacore::Vector<casacore::Int>& scanNo();
  casacore::Vector<casacore::Int>& processorId();
  casacore::Vector<casacore::Int>& stateId();
  casacore::Vector<casacore::Int>& phaseId();
  casacore::Vector<casacore::Int>& pulsarBin();
  casacore::Vector<casacore::Int>& pulsarGateId();
  casacore::Vector<casacore::Int>& freqGrp();
  casacore::Vector<casacore::String>& freqGrpName();
  casacore::Vector<casacore::String>& fieldName();
  casacore::Vector<casacore::String>& fieldCode();
  casacore::Vector<casacore::String>& sourceName();
  casacore::Vector<casacore::String>& sourceCode();
  casacore::Vector<casacore::Int>& calGrp();
  casacore::Array<casacore::Complex>& gain();
  casacore::Array<casacore::Int>& refAnt();
  casacore::Array<casacore::Int>& refFeed();
  casacore::Array<casacore::Int>& refReceptor();
  casacore::Array<casacore::Double>& refFreq();
  casacore::Vector<casacore::Int>& measFreqRef();
  casacore::Array<casacore::Double>& refDir();
  casacore::Vector<casacore::Int>& measDirRef();
  casacore::Vector<casacore::Int>& calDescId();
  casacore::Vector<casacore::Int>& calHistoryId(); 

 protected:

 private:
  // true if connected to underlying iterator
  casacore::Bool connectedToIter_p;

  // Ptr to cal main columns accessor
  CalMainColumns* calMainCol_p;

  // Buffer fields
  casacore::Vector<casacore::Double> time_p;
  casacore::Vector<casacore::MEpoch> timeMeas_p;
  casacore::Vector<casacore::Double> timeEP_p;
  casacore::Vector<casacore::Double> interval_p;
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
  casacore::Array<casacore::Double> refFreq_p;
  casacore::Vector<casacore::Int> measFreqRef_p;
  casacore::Array<casacore::Double> refDir_p;
  casacore::Vector<casacore::Int> measDirRef_p;
  casacore::Vector<casacore::Int> calDescId_p;
  casacore::Vector<casacore::Int> calHistoryId_p;

  // Buffer field status flags
  casacore::Bool timeOK_p;
  casacore::Bool timeMeasOK_p;
  casacore::Bool timeEPOK_p;
  casacore::Bool intervalOK_p;
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
  casacore::Bool refFreqOK_p;
  casacore::Bool measFreqRefOK_p;
  casacore::Bool refDirOK_p;
  casacore::Bool measDirRefOK_p;
  casacore::Bool calDescIdOK_p;
  casacore::Bool calHistoryIdOK_p;
};


} //# NAMESPACE CASA - END

#endif
   
  



