//# CalMainRecord.cc: Implementation of CalMainRecord.h
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

#include <synthesis/CalTables/CalMainRecord.h>
#include <msvis/MSVis/MSCalEnums.h>
#include <casa/Arrays.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

//----------------------------------------------------------------------------

CalMainRecord::CalMainRecord() : itsRecord()
{
// Null constructor
// Output to private data:
//    itsRecord        Record          Underlying record object
//
};

//----------------------------------------------------------------------------

CalMainRecord::CalMainRecord (const Record& inpRec) : itsRecord (inpRec)
{
// Construct from an input record
// Input:
//    inpRec           Record          Input record
// Output to private data:
//    itsRecord        Record          Underlying record object
//
};

//----------------------------------------------------------------------------

const Record& CalMainRecord::record()
{
// Return as record
// Output:
//    record           const Record&   Return underlying record object
// Input from private data:
//    itsRecord        Record          Underlying record object
//
  return itsRecord;
};

//----------------------------------------------------------------------------

void CalMainRecord::addRec (const Record& newRec)
{
// Add to underlying record object
// Input:
//    newRec           Record          Record to be added
// Output to private data:
//    itsRecord        Record          Underlying record object
//
  itsRecord.merge (newRec, RecordInterface::OverwriteDuplicates);
};

//----------------------------------------------------------------------------

void CalMainRecord::defineTime (const Double& time)
{
// Define the TIME field value (expressed as a Double)
// Input:
//    time             const Double&         TIME value
// Output to private data:
//    itsRecord        Record                Underlying record object
//
  Record newRec;
  newRec.define (MSC::fieldName (MSC::TIME), time);
  addRec (newRec);
};

//----------------------------------------------------------------------------

void CalMainRecord::defineTime (const MEpoch& time)
{
// Define the TIME field value (expressed as an MEpoch)
// Input:
//    time             const MEpoch&         TIME value
// Output to private data:
//    itsRecord        Record                Underlying record object
//
  Record newRec;
  newRec.define (MSC::fieldName (MSC::TIME), time.getValue().get());
  addRec (newRec);
};

//----------------------------------------------------------------------------

void CalMainRecord::defineTimeEP (const Double& timeEP)
{
// Define the TIME_EXTRA_PREC field value
// Input:
//    timeEP           const Double&         TIME_EXTRA_PREC value
// Output to private data:
//    itsRecord        Record                Underlying record object
//
  Record newRec;
  newRec.define (MSC::fieldName (MSC::TIME_EXTRA_PREC), timeEP);
  addRec (newRec);
};

//----------------------------------------------------------------------------

void CalMainRecord::defineInterval (const Double& interval)
{
// Define the INTERVAL field value
// Input:
//    interval         const Double&         INTERVAL value
// Output to private data:
//    itsRecord        Record                Underlying record object
//
  Record newRec;
  newRec.define (MSC::fieldName (MSC::INTERVAL), interval);
  addRec (newRec);
};

//----------------------------------------------------------------------------

void CalMainRecord::defineAntenna1 (const Int& antenna1)
{
// Define the ANTENNA1 field value
// Input:
//    antenna1         const Int&            ANTENNA1 value
// Output to private data:
//    itsRecord        Record                Underlying record object
//
  Record newRec;
  newRec.define (MSC::fieldName (MSC::ANTENNA1), antenna1);
  addRec (newRec);
};

//----------------------------------------------------------------------------

void CalMainRecord::defineFeed1 (const Int& feed1)
{
// Define the FEED1 field value
// Input:
//    antenna1         const Int&            FEED1 value
// Output to private data:
//    itsRecord        Record                Underlying record object
//
  Record newRec;
  newRec.define (MSC::fieldName (MSC::FEED1), feed1);
  addRec (newRec);
};

//----------------------------------------------------------------------------

void CalMainRecord::defineFieldId (const Int& fieldId)
{
// Define the FIELD_ID field value
// Input:
//    fieldId          const Int&            FIELD_ID value
// Output to private data:
//    itsRecord        Record                Underlying record object
//
  Record newRec;
  newRec.define (MSC::fieldName (MSC::FIELD_ID), fieldId);
  addRec (newRec);
};

//----------------------------------------------------------------------------

void CalMainRecord::defineArrayId (const Int& arrayId)
{
// Define the ARRAY_ID field value
// Input:
//    arrayId          const Int&            ARRAY_ID value
// Output to private data:
//    itsRecord        Record                Underlying record object
//
  Record newRec;
  newRec.define (MSC::fieldName (MSC::ARRAY_ID), arrayId);
  addRec (newRec);
};

//----------------------------------------------------------------------------

void CalMainRecord::defineObsId (const Int& obsId)
{
// Define the OBSERVATION_ID field value
// Input:
//    obsId            const Int&            OBSERVATION_ID value
// Output to private data:
//    itsRecord        Record                Underlying record object
//
  Record newRec;
  newRec.define (MSC::fieldName (MSC::OBSERVATION_ID), obsId);
  addRec (newRec);
};

//----------------------------------------------------------------------------

void CalMainRecord::defineScanNo (const Int& scanNo)
{
// Define the SCAN_NUMBER field value
// Input:
//    scanNo           const Int&            SCAN_NUMBER value
// Output to private data:
//    itsRecord        Record                Underlying record object
//
  Record newRec;
  newRec.define (MSC::fieldName (MSC::SCAN_NUMBER), scanNo);
  addRec (newRec);
};

//----------------------------------------------------------------------------

void CalMainRecord::defineProcessorId (const Int& processorId)
{
// Define the PROCESSOR_ID field value
// Input:
//    processorId      const Int&            PROCESSOR_ID value
// Output to private data:
//    itsRecord        Record                Underlying record object
//
  Record newRec;
  newRec.define (MSC::fieldName (MSC::PROCESSOR_ID), processorId);
  addRec (newRec);
};

//----------------------------------------------------------------------------

void CalMainRecord::defineStateId (const Int& stateId)
{
// Define the STATE_ID field value
// Input:
//    stateId          const Int&            STATE_ID value
// Output to private data:
//    itsRecord        Record                Underlying record object
//
  Record newRec;
  newRec.define (MSC::fieldName (MSC::STATE_ID), stateId);
  addRec (newRec);
};

//----------------------------------------------------------------------------

void CalMainRecord::definePhaseId (const Int& phaseId)
{
// Define the PHASE_ID field value
// Input:
//    phaseId          const Int&            PHASE_ID value
// Output to private data:
//    itsRecord        Record                Underlying record object
//
  Record newRec;
  newRec.define (MSC::fieldName (MSC::PHASE_ID), phaseId);
  addRec (newRec);
};

//----------------------------------------------------------------------------

void CalMainRecord::definePulsarBin (const Int& pulsarBin)
{
// Define the PULSAR_BIN field value
// Input:
//    pulsarBin        const Int&            PULSAR_BIN value
// Output to private data:
//    itsRecord        Record                Underlying record object
//
  Record newRec;
  newRec.define (MSC::fieldName (MSC::PULSAR_BIN), pulsarBin);
  addRec (newRec);
};

//----------------------------------------------------------------------------

void CalMainRecord::definePulsarGateId (const Int& pulsarGateId)
{
// Define the PULSAR_GATE_ID field value
// Input:
//    pulsarGateId     const Int&            PULSAR_GATE_ID value
// Output to private data:
//    itsRecord        Record                Underlying record object
//
  Record newRec;
  newRec.define (MSC::fieldName (MSC::PULSAR_GATE_ID), pulsarGateId);
  addRec (newRec);
};

//----------------------------------------------------------------------------

void CalMainRecord::defineFreqGrp (const Int& freqGrp)
{
// Define the FREQ_GROUP field value
// Input:
//    freqGrp          const Int&            FREQ_GROUP value
// Output to private data:
//    itsRecord        Record                Underlying record object
//
  Record newRec;
  newRec.define (MSC::fieldName (MSC::FREQ_GROUP), freqGrp);
  addRec (newRec);
};

//----------------------------------------------------------------------------

void CalMainRecord::defineFreqGrpName (const String& freqGrpName)
{
// Define the FREQ_GROUP_NAME field value
// Input:
//    freqGrpName      const String&         FREQ_GROUP_NAME value
// Output to private data:
//    itsRecord        Record                Underlying record object
//
  Record newRec;
  newRec.define (MSC::fieldName (MSC::FREQ_GROUP_NAME), freqGrpName);
  addRec (newRec);
};

//----------------------------------------------------------------------------

void CalMainRecord::defineFieldName (const String& fieldName)
{
// Define the FIELD_NAME field value
// Input:
//    fieldName        const String&         FIELD_NAME value
// Output to private data:
//    itsRecord        Record                Underlying record object
//
  Record newRec;
  newRec.define (MSC::fieldName (MSC::FIELD_NAME), fieldName);
  addRec (newRec);
};

//----------------------------------------------------------------------------

void CalMainRecord::defineFieldCode (const String& fieldCode)
{
// Define the FIELD_CODE field value
// Input:
//    fieldCode        const String&         FIELD_CODE value
// Output to private data:
//    itsRecord        Record                Underlying record object
//
  Record newRec;
  newRec.define (MSC::fieldName (MSC::FIELD_CODE), fieldCode);
  addRec (newRec);
};

//----------------------------------------------------------------------------

void CalMainRecord::defineSourceName (const String& sourceName)
{
// Define the SOURCE_NAME field value
// Input:
//    sourceName       const String&         SOURCE_NAME value
// Output to private data:
//    itsRecord        Record                Underlying record object
//
  Record newRec;
  newRec.define (MSC::fieldName (MSC::SOURCE_NAME), sourceName);
  addRec (newRec);
};

//----------------------------------------------------------------------------

void CalMainRecord::defineSourceCode (const String& sourceCode)
{
// Define the SOURCE_CODE field value
// Input:
//    sourceCode       const String&         SOURCE_CODE value
// Output to private data:
//    itsRecord        Record                Underlying record object
//
  Record newRec;
  newRec.define (MSC::fieldName (MSC::SOURCE_CODE), sourceCode);
  addRec (newRec);
};

//----------------------------------------------------------------------------

void CalMainRecord::defineCalGrp (const Int& calGrp)
{
// Define the CALIBRATION_GROUP field value
// Input:
//    calGrp           const Int&            CALIBRATION_GROUP value
// Output to private data:
//    itsRecord        Record                Underlying record object
//
  Record newRec;
  newRec.define (MSC::fieldName (MSC::CALIBRATION_GROUP), calGrp);
  addRec (newRec);
};

//----------------------------------------------------------------------------

void CalMainRecord::defineGain (const Array <Complex>& gain)
{
// Define the GAIN field value
// Input:
//    gain             const Array<Complex>&    GAIN value
// Output to private data:
//    itsRecord        Record                   Underlying record object
//
  Record newRec;
  newRec.define (MSC::fieldName (MSC::GAIN), gain);
  addRec (newRec);
};

//----------------------------------------------------------------------------

void CalMainRecord::defineRefAnt (const Array<Int>& refAnt)
{
// Define the REF_ANT field value
// Input:
//    refAnt           const Array<Int>&     REF_ANT value
// Output to private data:
//    itsRecord        Record                Underlying record object
//
  Record newRec;
  newRec.define (MSC::fieldName (MSC::REF_ANT), refAnt);
  addRec (newRec);
};

//----------------------------------------------------------------------------

void CalMainRecord::defineRefFeed (const Array<Int>& refFeed)
{
// Define the REF_FEED field value
// Input:
//    refFeed          const Array<Int>&     REF_FEED value
// Output to private data:
//    itsRecord        Record                Underlying record object
//
  Record newRec;
  newRec.define (MSC::fieldName (MSC::REF_FEED), refFeed);
  addRec (newRec);
};

//----------------------------------------------------------------------------

void CalMainRecord::defineRefReceptor (const Array<Int>& refReceptor)
{
// Define the REF_RECEPTOR field value
// Input:
//    refReceptor      const Array<Int>&     REF_RECEPTOR value
// Output to private data:
//    itsRecord        Record                Underlying record object
//
  Record newRec;
  newRec.define (MSC::fieldName (MSC::REF_RECEPTOR), refReceptor);
  addRec (newRec);
};

//----------------------------------------------------------------------------

void CalMainRecord::defineRefFreq (const Array<Double>& refFreq)
{
// Define the REF_FREQUENCY field value
// Input:
//    refFreq          const Array<Double>&  REF_FREQUENCY value
// Output to private data:
//    itsRecord        Record                Underlying record object
//
  Record newRec;
  newRec.define (MSC::fieldName (MSC::REF_FREQUENCY), refFreq);
  addRec (newRec);
};

//----------------------------------------------------------------------------

void CalMainRecord::defineMeasFreqRef (const Int& measFreqRef)
{
// Define the MEAS_FREQ_REF field value
// Input:
//    measFreqRef      const Int&            MEAS_FREQ_REF value
// Output to private data:
//    itsRecord        Record                Underlying record object
//
  Record newRec;
  newRec.define (MSC::fieldName (MSC::MEAS_FREQ_REF), measFreqRef);
  addRec (newRec);
};

//----------------------------------------------------------------------------

void CalMainRecord::defineRefDir (const Array <Double>& refDir)
{
// Define the REF_DIRECTION field value
// Input:
//    refDir           const Array <Double>&   REF_DIRECTION value    
// Output to private data:
//    itsRecord        Record                  Underlying record object
//
  Record newRec;
  newRec.define (MSC::fieldName (MSC::REF_DIRECTION), refDir);
  addRec (newRec);
};

//----------------------------------------------------------------------------

void CalMainRecord::defineMeasDirRef (const Int& measDirRef)
{
// Define the MEAS_DIR_REF field value
// Input:
//    measDirRef       const Int&            MEAS_DIR_REF value
// Output to private data:
//    itsRecord        Record                Underlying record object
//
  Record newRec;
  newRec.define (MSC::fieldName (MSC::MEAS_DIR_REF), measDirRef);
  addRec (newRec);
};

//----------------------------------------------------------------------------

void CalMainRecord::defineCalDescId (const Int& calDescId)
{
// Define the CAL_DESC_ID field value
// Input:
//    calDescId        const Int&            CAL_DESC_ID value
// Output to private data:
//    itsRecord        Record                Underlying record object
//
  Record newRec;
  newRec.define (MSC::fieldName (MSC::CAL_DESC_ID), calDescId);
  addRec (newRec);
};

//----------------------------------------------------------------------------

void CalMainRecord::defineCalHistoryId (const Int& calHistoryId)
{
// Define the CAL_HISTORY_ID field value
// Input:
//    calHistoryId     const Int&            CAL_HISTORY_ID value
// Output to private data:
//    itsRecord        Record                Underlying record object
//
  Record newRec;
  newRec.define (MSC::fieldName (MSC::CAL_HISTORY_ID), calHistoryId);
  addRec (newRec);
};

//----------------------------------------------------------------------------

void CalMainRecord::getTime (Double& time)
{
// Get the TIME field value
// Output:
//    time             Double&               TIME value
// Input from private data:
//    itsRecord        Record                Underlying record object
//
  record().get (MSC::fieldName (MSC::TIME), time);
};

//----------------------------------------------------------------------------

void CalMainRecord::getTimeEP (Double& timeEP)
{
// Get the TIME_EXTRA_PREC field value
// Output:
//    timeEP           Double&               TIME_EXTRA_PREC value
// Input from private data:
//    itsRecord        Record                Underlying record object
//
  record().get (MSC::fieldName (MSC::TIME_EXTRA_PREC), timeEP);
};

//----------------------------------------------------------------------------

void CalMainRecord::getInterval (Double& interval)
{
// Get the INTERVAL field value
// Output:
//    interval         Double&               INTERVAL value
// Input from private data:
//    itsRecord        Record                Underlying record object
//
  record().get (MSC::fieldName (MSC::INTERVAL), interval);
};

//----------------------------------------------------------------------------

void CalMainRecord::getAntenna1 (Int& antenna1)
{
// Get the ANTENNA1 field value
// Output:
//    antenna1         Int&                  ANTENNA1 value
// Input from private data:
//    itsRecord        Record                Underlying record object
//
  record().get (MSC::fieldName (MSC::ANTENNA1), antenna1);
};

//----------------------------------------------------------------------------

void CalMainRecord::getFeed1 (Int& feed1)
{
// Get the FEED1 field value
// Output:
//    feed1            Int&                  FEED1 value
// Input from private data:
//    itsRecord        Record                Underlying record object
//
  record().get (MSC::fieldName (MSC::FEED1), feed1);
};

//----------------------------------------------------------------------------

void CalMainRecord::getFieldId (Int& fieldId)
{
// Get the FIELD_ID field value
// Output:
//    fieldId          Int&                  FIELD_ID value
// Input from private data:
//    itsRecord        Record                Underlying record object
//
  record().get (MSC::fieldName (MSC::FIELD_ID), fieldId);
};

//----------------------------------------------------------------------------

void CalMainRecord::getArrayId (Int& arrayId)
{
// Get the ARRAY_ID field value
// Output:
//    arrayId          Int&                  ARRAY_ID value
// Input from private data:
//    itsRecord        Record                Underlying record object
//
  record().get (MSC::fieldName (MSC::ARRAY_ID), arrayId);
};

//----------------------------------------------------------------------------

void CalMainRecord::getObsId (Int& obsId)
{
// Get the OBSERVATION_ID field value
// Output:
//    obsId            Int&                  OBSERVATION_ID value
// Input from private data:
//    itsRecord        Record                Underlying record object
//
  record().get (MSC::fieldName (MSC::OBSERVATION_ID), obsId);
};

//----------------------------------------------------------------------------

void CalMainRecord::getScanNo (Int& scanNo)
{
// Get the SCAN_NUMBER field value
// Output:
//    scanNo           Int&                  SCAN_NUMBER value
// Input from private data:
//    itsRecord        Record                Underlying record object
//
  record().get (MSC::fieldName (MSC::SCAN_NUMBER), scanNo);
};

//----------------------------------------------------------------------------

void CalMainRecord::getProcessorId (Int& processorId)
{
// Get the PROCESSOR_ID field value
// Output:
//    processorId      Int&                  PROCESSOR_ID value
// Input from private data:
//    itsRecord        Record                Underlying record object
//
  record().get (MSC::fieldName (MSC::PROCESSOR_ID), processorId);
};

//----------------------------------------------------------------------------

void CalMainRecord::getStateId (Int& stateId)
{
// Get the STATE_ID field value
// Output:
//    stateId          Int&                  STATE_ID value
// Input from private data:
//    itsRecord        Record                Underlying record object
//
  record().get (MSC::fieldName (MSC::STATE_ID), stateId);
};

//----------------------------------------------------------------------------

void CalMainRecord::getPhaseId (Int& phaseId)
{
// Get the PHASE_ID field value
// Output:
//    phaseId          Int&                  PHASE_ID value
// Input from private data:
//    itsRecord        Record                Underlying record object
//
  record().get (MSC::fieldName (MSC::PHASE_ID), phaseId);
};

//----------------------------------------------------------------------------

void CalMainRecord::getPulsarBin (Int& pulsarBin)
{
// Get the PULSAR_BIN field value
// Output:
//    pulsarBin        Int&                  PULSAR_BIN value
// Input from private data:
//    itsRecord        Record                Underlying record object
//
  record().get (MSC::fieldName (MSC::PULSAR_BIN), pulsarBin);
};

//----------------------------------------------------------------------------

void CalMainRecord::getPulsarGateId (Int& pulsarGateId)
{
// Get the PULSAR_GATE_ID field value
// Output:
//    pulsarGateId     Int&                  PULSAR_GATE_ID value
// Input from private data:
//    itsRecord        Record                Underlying record object
//
  record().get (MSC::fieldName (MSC::PULSAR_GATE_ID), pulsarGateId);
};

//----------------------------------------------------------------------------

void CalMainRecord::getFreqGrp (Int& freqGrp)
{
// Get the FREQUENCY_GROUP field value
// Output:
//    freqGrp          Int&                  FREQUENCY_GROUP value
// Input from private data:
//    itsRecord        Record                Underlying record object
//
  record().get (MSC::fieldName (MSC::FREQ_GROUP), freqGrp);
};

//----------------------------------------------------------------------------

void CalMainRecord::getFreqGrpName (String& freqGrpName)
{
// Get the FREQ_GROUP_NAME field value
// Output:
//    freqGrpName      String&               FREQ_GROUP_NAME value
// Input from private data:
//    itsRecord        Record                Underlying record object
//
  record().get (MSC::fieldName (MSC::FREQ_GROUP_NAME), freqGrpName);
};

//----------------------------------------------------------------------------

void CalMainRecord::getFieldName (String& fieldName)
{
// Get the FIELD_NAME field value
// Output:
//    fieldName        String&               FIELD_NAME value
// Input from private data:
//    itsRecord        Record                Underlying record object
//
  record().get (MSC::fieldName (MSC::FIELD_NAME), fieldName);
};

//----------------------------------------------------------------------------

void CalMainRecord::getFieldCode (String& fieldCode)
{
// Get the FIELD_CODE field value
// Output:
//    fieldCode        String&               FIELD_CODE value
// Input from private data:
//    itsRecord        Record                Underlying record object
//
  record().get (MSC::fieldName (MSC::FIELD_NAME), fieldCode);
};

//----------------------------------------------------------------------------

void CalMainRecord::getSourceName (String& sourceName)
{
// Get the SOURCE_NAME field value
// Output:
//    sourceName       String&               SOURCE_NAME value
// Input from private data:
//    itsRecord        Record                Underlying record object
//
  record().get (MSC::fieldName (MSC::SOURCE_NAME), sourceName);
};

//----------------------------------------------------------------------------

void CalMainRecord::getSourceCode (String& sourceCode)
{
// Get the SOURCE_CODE field value
// Output:
//    fieldName        String&               SOURCE_CODE value
// Input from private data:
//    itsRecord        Record                Underlying record object
//
  record().get (MSC::fieldName (MSC::SOURCE_CODE), sourceCode);
};

//----------------------------------------------------------------------------

void CalMainRecord::getCalGrp (Int& calGrp)
{
// Get the CALIBRATION_GROUP field value
// Output:
//    calGrp           String&               CALIBRATION_GROUP value
// Input from private data:
//    itsRecord        Record                Underlying record object
//
  record().get (MSC::fieldName (MSC::CALIBRATION_GROUP), calGrp);
};

//----------------------------------------------------------------------------

void CalMainRecord::getGain (Array <Complex>& gain)
{
// Get the GAIN field value
// Output:
//    gain             Array <Complex>&      GAIN value
// Input from private data:
//    itsRecord        Record                Underlying record object
//
  record().get (MSC::fieldName (MSC::GAIN), gain);
};

//----------------------------------------------------------------------------

void CalMainRecord::getRefAnt (Array<Int>& refAnt)
{
// Get the REF_ANT field value
// Output:
//    refAnt           Array<Int>&           REF_ANT value
// Input from private data:
//    itsRecord        Record                Underlying record object
//
  record().get (MSC::fieldName (MSC::REF_ANT), refAnt);
};

//----------------------------------------------------------------------------

void CalMainRecord::getRefFeed (Array<Int>& refFeed)
{
// Get the REF_FEED field value
// Output:
//    refFeed          Array<Int>&           REF_FEED value
// Input from private data:
//    itsRecord        Record                Underlying record object
//
  record().get (MSC::fieldName (MSC::REF_FEED), refFeed);
};

//----------------------------------------------------------------------------

void CalMainRecord::getRefReceptor (Array<Int>& refReceptor)
{
// Get the REF_RECEPTOR field value
// Output:
//    refReceptor      Array<Int>&           REF_RECEPTOR value
// Input from private data:
//    itsRecord        Record                Underlying record object
//
  record().get (MSC::fieldName (MSC::REF_RECEPTOR), refReceptor);
};

//----------------------------------------------------------------------------

void CalMainRecord::getRefFreq (Array<Double>& refFreq)
{
// Get the REF_FREQUENCY field value
// Output:
//    refFreq          Array<Double>&        REF_FREQUENCY value
// Input from private data:
//    itsRecord        Record                Underlying record object
//
  record().get (MSC::fieldName (MSC::REF_FREQUENCY), refFreq);
};

//----------------------------------------------------------------------------

void CalMainRecord::getMeasFreqRef (Int& measFreqRef)
{
// Get the MEAS_FREQ_REF field value
// Output:
//    measFreqRef      Int&                  MEAS_FREQ_REF value
// Input from private data:
//    itsRecord        Record                Underlying record object
//
  record().get (MSC::fieldName (MSC::MEAS_FREQ_REF), measFreqRef);
};

//----------------------------------------------------------------------------

void CalMainRecord::getRefDir (Array <Double>& refDir)
{
// Get the REF_DIRECTION field value
// Output:
//    refDir           Array <Double>&       REF_DIRECTION value
// Input from private data:
//    itsRecord        Record                Underlying record object
//
  record().get (MSC::fieldName (MSC::REF_DIRECTION), refDir);
};

//----------------------------------------------------------------------------

void CalMainRecord::getMeasDirRef (Int& measDirRef)
{
// Get the MEAS_DIR_REF field value
// Output:
//    measDirRef       Int&                  MEAS_DIR_REF value
// Input from private data:
//    itsRecord        Record                Underlying record object
//
  record().get (MSC::fieldName (MSC::MEAS_DIR_REF), measDirRef);
};

//----------------------------------------------------------------------------

void CalMainRecord::getCalDescId (Int& calDescId)
{
// Get the CAL_DESC_ID field value
// Output:
//    calDescId        Int&                  CAL_DESC_ID value
// Input from private data:
//    itsRecord        Record                Underlying record object
//
  record().get (MSC::fieldName (MSC::CAL_DESC_ID), calDescId);
};

//----------------------------------------------------------------------------

void CalMainRecord::getCalHistoryId (Int& calHistoryId)
{
// Get the CAL_HISTORY_ID field value
// Output:
//    calHistoryId     Int&                  CAL_HISTORY_ID value
// Input from private data:
//    itsRecord        Record                Underlying record object
//
  record().get (MSC::fieldName (MSC::CAL_HISTORY_ID), calHistoryId);
};

//----------------------------------------------------------------------------

void CalMainRecord::dTotalSolnOk (const Bool& totalSolnOk)
{
// Define the TOTAL_SOLUTION_OK field value
// Input:
//    totalSolnOk      const Bool&           TOTAL_SOLUTION_OK value
// Output:
//    itsRecord        Record                Underlying record object
//
  Record newRec;
  newRec.define (MSC::fieldName (MSC::TOTAL_SOLUTION_OK), totalSolnOk);
  addRec (newRec);
};

//----------------------------------------------------------------------------

void CalMainRecord::dTotalFit (const Float& totalFit)
{
// Define the TOTAL_FIT field value
// Input:
//    totalFit         const Float&          TOTAL_FIT value
// Output:
//    itsRecord        Record                Underlying record object
//
  Record newRec;
  newRec.define (MSC::fieldName (MSC::TOTAL_FIT), totalFit);
  addRec (newRec);
};

//----------------------------------------------------------------------------

void CalMainRecord::dTotalFitWgt (const Float& totalFitWgt)
{
// Define the TOTAL_FIT_WEIGHT field value
// Input:
//    totalFitWgt      const Float&          TOTAL_FIT_WEIGHT value
// Output:
//    itsRecord        Record                Underlying record object
//
  Record newRec;
  newRec.define (MSC::fieldName (MSC::TOTAL_FIT_WEIGHT), totalFitWgt);
  addRec (newRec);
};

//----------------------------------------------------------------------------

void CalMainRecord::dSolnOk (const Array <Bool>& solnOk)
{
// Define the SOLUTION_OK field value
// Input:
//    solnOk           const Array <Bool>&   SOLUTION_OK value
// Output:
//    itsRecord        Record                Underlying record object
//
  Record newRec;
  newRec.define (MSC::fieldName (MSC::SOLUTION_OK), solnOk);
  addRec (newRec);
};

//----------------------------------------------------------------------------

void CalMainRecord::dFit (const Array <Float>& fit)
{
// Define the FIT field value
// Input:
//    fit              const Array <Float>&  FIT value
// Output:
//    itsRecord        Record                Underlying record object
//
  Record newRec;
  newRec.define (MSC::fieldName (MSC::FIT), fit);
  addRec (newRec);
};

//----------------------------------------------------------------------------

void CalMainRecord::dFitWgt (const Array <Float>& fitWgt)
{
// Define the FIT_WEIGHT field value
// Input:
//    fitWgt           const Array <Float>&  FIT_WEIGHT value
// Output:
//    itsRecord        Record                Underlying record object
//
  Record newRec;
  newRec.define (MSC::fieldName (MSC::FIT_WEIGHT), fitWgt);
  addRec (newRec);
};

//----------------------------------------------------------------------------

void CalMainRecord::dFlag (const Array <Bool>& flag)
{
// Define the Flag field value
// Input:
//    flag             const Array <Bool>&   FLAG value
// Output:
//    itsRecord        Record                Underlying record object
//
  Record newRec;
  newRec.define (MSC::fieldName (MSC::FLAG), flag);
  addRec (newRec);
};

//----------------------------------------------------------------------------
void CalMainRecord::dSnr (const Array <Float>& snr)
{
// Define the SNR field value
// Input:
//    snr              const Array <Float>&  SNR value
// Output:
//    itsRecord        Record                Underlying record object
//
  Record newRec;
  newRec.define (MSC::fieldName (MSC::SNR), snr);
  addRec (newRec);
};


//----------------------------------------------------------------------------

void CalMainRecord::gTotalSolnOk (Bool& totalSolnOk)
{
// Get the TOTAL_SOLUTION_OK field value
// Output:
//    totalSolnOk      Bool&                 TOTAL_SOLUTION_OK value
// Input from private data:
//    itsRecord        Record                Underlying record object
//
  record().get (MSC::fieldName (MSC::TOTAL_SOLUTION_OK), totalSolnOk);
};

//----------------------------------------------------------------------------

void CalMainRecord::gTotalFit (Float& totalFit)
{
// Get the TOTAL_FIT field value
// Output:
//    totalFit         Float&                TOTAL_FIT value
// Input from private data:
//    itsRecord        Record                Underlying record object
//
  record().get (MSC::fieldName (MSC::TOTAL_FIT), totalFit);
};

//----------------------------------------------------------------------------

void CalMainRecord::gTotalFitWgt (Float& totalFitWgt)
{
// Get the TOTAL_FIT_WEIGHT field value
// Output:
//    totalFitWgt      Float&                TOTAL_FIT_WEIGHT value
// Input from private data:
//    itsRecord        Record                Underlying record object
//
  record().get (MSC::fieldName (MSC::TOTAL_FIT_WEIGHT), totalFitWgt);
};

//----------------------------------------------------------------------------

void CalMainRecord::gSolnOk (Array <Bool>& solnOk)
{
// Get the SOLUTION_OK field value
// Output:
//    solnOk           Array <Bool>&         SOLUTION_OK value
// Input from private data:
//    itsRecord        Record                Underlying record object
//
  record().get (MSC::fieldName (MSC::SOLUTION_OK), solnOk);
};

//----------------------------------------------------------------------------

void CalMainRecord::gFit (Array <Float>& fit)
{
// Get the FIT field value
// Output:
//    fit              Array <Float>&        FIT value
// Input from private data:
//    itsRecord        Record                Underlying record object
//
  record().get (MSC::fieldName (MSC::FIT), fit);
};

//----------------------------------------------------------------------------

void CalMainRecord::gFitWgt (Array <Float>& fitWgt)
{
// Get the FIT_WEIGHT field value
// Output:
//    fitWgt           Array <Float>&        FIT_WEIGHT value
// Input from private data:
//    itsRecord        Record                Underlying record object
//
  record().get (MSC::fieldName (MSC::FIT_WEIGHT), fitWgt);
};

//----------------------------------------------------------------------------

void CalMainRecord::gFlag (Array <Bool>& flag)
{
// Get the Flag field value
// Output:
//    flag             Array <Bool>&         FLAG value
// Input:
//    itsRecord        Record                Underlying record object
//
  record().get (MSC::fieldName (MSC::FLAG), flag);
};

//----------------------------------------------------------------------------
void CalMainRecord::gSnr (Array <Float>& snr)
{
// Define the SNR field value
// Output:
//    snr              Array <Float>&        SNR value
// Output:
//    itsRecord        Record                Underlying record object
//
  record().get (MSC::fieldName (MSC::SNR), snr);
};

//----------------------------------------------------------------------------



} //# NAMESPACE CASA - END

