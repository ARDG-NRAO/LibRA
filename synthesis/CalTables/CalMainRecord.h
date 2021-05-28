//# CalMainRecord.h: Calibration table record access and creation
//# Copyright (C) 1996,1997,1998,2001
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
//# $Id: 

#ifndef CALIBRATION_CALMAINRECORD_H
#define CALIBRATION_CALMAINRECORD_H

#include <casa/aips.h>
#include <measures/Measures/MEpoch.h>
#include <casa/Containers/Record.h>

namespace casa { //# NAMESPACE CASA - BEGIN

// <summary> 
// CalMainRecord: Calibration table record access and creation
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//   <li> <linkto class="casacore::Record">casacore::Record</linkto> module
// </prerequisite>
//
// <etymology>
// From "calibration main table" and "record".
// </etymology>
//
// <synopsis>
// The CalMainRecord classes allow access to, and creation of, records
// in the main calibration table. Specializations for baseline-based,
// time-variable and solvable VisJones calibration table record types
// are provided through inheritance.
// </etymology>
//
// <example>
// <srcblock>
// </srcblock>
// </example>
//
// <motivation>
// Encapsulate access to calibration table records.
// </motivation>
//
// <todo asof="98/01/01">
// (i) Deal with non-standard columns.
// </todo>

class CalMainRecord
{
 public:
   // Default null constructor, and destructor
   CalMainRecord();
   ~CalMainRecord() {};

   // Construct from an existing record
   CalMainRecord (const casacore::Record& inpRec);

   // Return as record
   const casacore::Record& record();

   // Field accessors
   // a) define
   void defineTime (const casacore::Double& time);
   void defineTime (const casacore::MEpoch& time);
   void defineTimeEP (const casacore::Double& timeEP);
   void defineInterval (const casacore::Double& interval);
   void defineAntenna1 (const casacore::Int& antenna1);
   void defineFeed1 (const casacore::Int& feed1);
   void defineFieldId (const casacore::Int& fieldId);
   void defineArrayId (const casacore::Int& arrayId);
   void defineObsId (const casacore::Int& obsId);
   void defineScanNo (const casacore::Int& scanNo);
   void defineProcessorId (const casacore::Int& processorId);
   void defineStateId (const casacore::Int& stateId);
   void definePhaseId (const casacore::Int& phaseId);
   void definePulsarBin (const casacore::Int& pulsarBin);
   void definePulsarGateId (const casacore::Int& pulsarGateId);
   void defineFreqGrp (const casacore::Int& freqGrp);
   void defineFreqGrpName (const casacore::String& freqGrpName);
   void defineFieldName (const casacore::String& fieldName);
   void defineFieldCode (const casacore::String& fieldCode);
   void defineSourceName (const casacore::String& sourceName);
   void defineSourceCode (const casacore::String& sourceCode);
   void defineCalGrp (const casacore::Int& calGrp);
   void defineGain (const casacore::Array<casacore::Complex>& gain);
   void defineRefAnt (const casacore::Array<casacore::Int>& refAnt);
   void defineRefFeed (const casacore::Array<casacore::Int>& refFeed);
   void defineRefReceptor (const casacore::Array<casacore::Int>& refReceptor);
   void defineRefFreq (const casacore::Array<casacore::Double>& refFreq);
   void defineMeasFreqRef (const casacore::Int& measFreqRef);
   void defineRefDir (const casacore::Array<casacore::Double>& refDir);
   void defineMeasDirRef (const casacore::Int& measDirRef);
   void defineCalDescId (const casacore::Int& calDescId);
   void defineCalHistoryId (const casacore::Int& calHistoryId);

   // b) get
   void getTime (casacore::Double& time);
   void getTimeEP (casacore::Double& timeEP);
   void getInterval (casacore::Double& interval);
   void getAntenna1 (casacore::Int& antenna1);
   void getFeed1 (casacore::Int& feed1);
   void getFieldId (casacore::Int& fieldId);
   void getArrayId (casacore::Int& arrayId);
   void getObsId (casacore::Int& obsId);
   void getScanNo (casacore::Int& scanNo);
   void getProcessorId (casacore::Int& processorId);
   void getStateId (casacore::Int& stateId);
   void getPhaseId (casacore::Int& phaseId);
   void getPulsarBin (casacore::Int& pulsarBin);
   void getPulsarGateId (casacore::Int& pulsarGateId);
   void getFreqGrp (casacore::Int& freqGrp);
   void getFreqGrpName (casacore::String& freqGrpName);
   void getFieldName (casacore::String& fieldName);
   void getFieldCode (casacore::String& fieldCode);
   void getSourceName (casacore::String& sourceName);
   void getSourceCode (casacore::String& sourceCode);
   void getCalGrp (casacore::Int& calGrp);
   void getGain (casacore::Array<casacore::Complex>& gain);
   void getRefAnt (casacore::Array<casacore::Int>& refAnt);
   void getRefFeed (casacore::Array<casacore::Int>& refFeed);
   void getRefReceptor (casacore::Array<casacore::Int>& refReceptor);
   void getRefFreq (casacore::Array<casacore::Double>& refFreq);
   void getMeasFreqRef (casacore::Int& measFreqRef);
   void getRefDir (casacore::Array<casacore::Double>& refDir);
   void getMeasDirRef (casacore::Int& measDirRef);
   void getCalDescId (casacore::Int& calDescId);
   void getCalHistoryId (casacore::Int& calHistoryId);

 protected:
   // Add to itsRecord
   void addRec (const casacore::Record& newRec);

   // Field accessors for fit parameters.
   // Used in separate parts of the inheritance tree.
   // a) define
   void dTotalSolnOk (const casacore::Bool& totalSolnOk);
   void dTotalFit (const casacore::Float& totalFit);
   void dTotalFitWgt (const casacore::Float& totalFitWgt);
   void dSolnOk (const casacore::Array <casacore::Bool>& solnOk);
   void dFit (const casacore::Array <casacore::Float>& fit);
   void dFitWgt (const casacore::Array <casacore::Float>& fitWgt);
   void dFlag (const casacore::Array <casacore::Bool>& flag);
   void dSnr (const casacore::Array <casacore::Float>& snr);

   // b) get
   void gTotalSolnOk (casacore::Bool& totalSolnOk);
   void gTotalFit (casacore::Float& totalFit);
   void gTotalFitWgt (casacore::Float& totalFitWgt);
   void gSolnOk (casacore::Array <casacore::Bool>& solnOk);
   void gFit (casacore::Array <casacore::Float>& fit);
   void gFitWgt (casacore::Array <casacore::Float>& fitWgt);
   void gFlag (casacore::Array <casacore::Bool>& flag);
   void gSnr (casacore::Array <casacore::Float>& snr);

 private:
   casacore::Record itsRecord;

 };


} //# NAMESPACE CASA - END

#endif
   
  



