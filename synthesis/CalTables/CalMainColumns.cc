//# CalMainColumns.cc: Implementation of CalMainColumns.h
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

#include <synthesis/CalTables/CalMainColumns.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

//----------------------------------------------------------------------------

ROCalMainColumns::ROCalMainColumns (const CalTable& calTable) 
{
// Construct from a calibration table
// Input:
//    calTable         const CalTable&                Calibration table
// Output to private data:
//    time_p           ScalarColumn<Double>&        Time
//    timeMeas_p       ROScalarMeasColumn<MEpoch>&    Time as a Measure
//    timeEP_p         ScalarColumn<Double>&        Extended precision time
//    timeEPQuant_p    ROScalarQuantColumn<Double>&   EP time as Quantum
//    interval_p       ScalarColumn<Double>&        Time interval
//    intervalQuant_p  ROScalarQuantColumn<Double>&   Time interval as Quantum
//    antenna1_p       ScalarColumn<Int>&           Antenna 1
//    feed1_p          ScalarColumn<Int>&           Feed 1
//    fieldId_p        ScalarColumn<Int>&           Field id.
//    arrayId_p        ScalarColumn<Int>&           Array id.
//    obsId_p          ScalarColumn<Int>&           Observation id.
//    scanNo_p         ScalarColumn<Int>&           Scan no.
//    processorId_p    ScalarColumn<Int>&           Processor id.
//    stateId_p        ScalarColumn<Int>&           State id.
//    phaseId_p        ScalarColumn<Int>&           Phase id.
//    pulsarBin_p      ScalarColumn<Int>&           Pulsar bin
//    pulsarGateId_p   ScalarColumn<Int>&           Pulsar gate id.
//    freqGrp_p        ScalarColumn<Int>&           Frequency group
//    freqGrpName_p    ScalarColumn<String>&        Frequency group name
//    fieldName_p      ScalarColumn<String>&        Field name
//    fieldCode_p      ScalarColumn<String>&        Field code
//    sourceName_p     ScalarColumn<String>&        Source name
//    sourceCode_p     ScalarColumn<String>&        Source code
//    calGrp_p         ScalarColumn<Int>&           Calibration group
//    gain_p           ArrayColumn<Complex>&        Gain
//    refAnt_p         ArrayColumn<Int>&            Reference antenna
//    refFeed_p        ArrayColumn<Int>&            Reference feed
//    refReceptor_p    ArrayColumn<Int>&            Reference receptor
//    refFreq_p        ArrayColumn<Double>&         Reference frequency
//    refFreqMeas_p    ROArrayMeasColumn<MFrequency>& Ref. freq. as Measure
//    measFreqRef_p    ScalarColumn<Int>&           Frequency measures ref.
//    refDir_p         ArrayColumn<Double>&         Reference direction
//    refDirMeas_p     ROArrayMeasColumn<MDirection>& Ref. dir. as Measure
//    measDirRef_p     ScalarColumn<Int>&           Direction measures ref.
//    calDescId_p      ScalarColumn<Int>&           CAL_DESC id.
//    calHistoryId_p   ScalarColumn<Int>&           CAL_HISTORY id.
//
  // Attach all the column accessors (including required and
  // optional columns)
  attach (calTable, time_p, MSC::TIME);
  attach (calTable, timeMeas_p, MSC::TIME);
  attach (calTable, timeEP_p, MSC::TIME_EXTRA_PREC, true);
  attach (calTable, timeEPQuant_p, MSC::TIME_EXTRA_PREC, true);
  attach (calTable, interval_p, MSC::INTERVAL);
  attach (calTable, intervalQuant_p, MSC::INTERVAL);
  attach (calTable, antenna1_p, MSC::ANTENNA1);
  attach (calTable, feed1_p, MSC::FEED1);
  attach (calTable, fieldId_p, MSC::FIELD_ID);
  attach (calTable, arrayId_p, MSC::ARRAY_ID);
  attach (calTable, obsId_p, MSC::OBSERVATION_ID);
  attach (calTable, scanNo_p, MSC::SCAN_NUMBER);
  attach (calTable, processorId_p, MSC::PROCESSOR_ID);
  attach (calTable, stateId_p, MSC::STATE_ID);
  attach (calTable, phaseId_p, MSC::PHASE_ID, true);
  attach (calTable, pulsarBin_p, MSC::PULSAR_BIN, true);
  attach (calTable, pulsarGateId_p, MSC::PULSAR_GATE_ID, true);
  attach (calTable, freqGrp_p, MSC::FREQ_GROUP);
  attach (calTable, freqGrpName_p, MSC::FREQ_GROUP_NAME);
  attach (calTable, fieldName_p, MSC::FIELD_NAME);
  attach (calTable, fieldCode_p, MSC::FIELD_CODE);
  attach (calTable, sourceName_p, MSC::SOURCE_NAME);
  attach (calTable, sourceCode_p, MSC::SOURCE_CODE);
  attach (calTable, calGrp_p, MSC::CALIBRATION_GROUP);
  attach (calTable, gain_p, MSC::GAIN);
  attach (calTable, refAnt_p, MSC::REF_ANT);
  attach (calTable, refFeed_p, MSC::REF_FEED);
  attach (calTable, refReceptor_p, MSC::REF_RECEPTOR);
  attach (calTable, pulsarBin_p, MSC::PULSAR_BIN);
  attach (calTable, pulsarGateId_p, MSC::PULSAR_GATE_ID);
  attach (calTable, freqGrp_p, MSC::FREQ_GROUP);
  attach (calTable, freqGrpName_p, MSC::FREQ_GROUP_NAME);
  attach (calTable, fieldName_p, MSC::FIELD_NAME);
  attach (calTable, fieldCode_p, MSC::FIELD_CODE);
  attach (calTable, sourceName_p, MSC::SOURCE_NAME);
  attach (calTable, sourceCode_p, MSC::SOURCE_CODE);
  attach (calTable, calGrp_p, MSC::CALIBRATION_GROUP);
  attach (calTable, gain_p, MSC::GAIN);
  attach (calTable, refAnt_p, MSC::REF_ANT);
  attach (calTable, refFeed_p, MSC::REF_FEED);
  attach (calTable, refReceptor_p, MSC::REF_RECEPTOR);
  attach (calTable, refFreq_p, MSC::REF_FREQUENCY);
  attach (calTable, refFreqMeas_p, MSC::REF_FREQUENCY);
  attach (calTable, measFreqRef_p, MSC::MEAS_FREQ_REF);
  attach (calTable, refDir_p, MSC::REF_DIRECTION);
  attach (calTable, refDirMeas_p, MSC::REF_DIRECTION);
  attach (calTable, measDirRef_p, MSC::MEAS_DIR_REF);
  attach (calTable, calDescId_p, MSC::CAL_DESC_ID);
  attach (calTable, calHistoryId_p, MSC::CAL_HISTORY_ID);
};

//----------------------------------------------------------------------------

void ROCalMainColumns::attach (const CalTable& calTable, 
			       TableColumn& tabCol, 
			       MSCalEnums::colDef colEnum, 
			       const Bool& optional)
{
// Attach a column accessor to the calibration table
// Input:
//    calTable         const CalTable&      Calibration table
//    tabCol           TableColumn&       Table column accessor
//    colEnum          MSCalEnums::colDef   Column enum
//    optional         const Bool&          true if optional column
// Output to private data:
//
  // Convert to column name
  String colName = MSC::fieldName (colEnum);

  // Check if an optional column
  if (optional) {
    if (asTable(calTable).tableDesc().isColumn(colName)) {
      tabCol.attach (asTable(calTable), colName);
    };
  } else {
    tabCol.attach (asTable(calTable), colName);
  };
};

//----------------------------------------------------------------------------

void ROCalMainColumns::attach (const CalTable& calTable, 
			       ROArrayMeasColumn<MFrequency>& tabCol, 
			       MSCalEnums::colDef colEnum, 
			       const Bool& optional)
{
// Attach a column accessor to the calibration table
// Input:
//    calTable         const CalTable&                  Calibration table
//    tabCol           ROArrayMeasColumn<MFrequency>&   Table measures 
//                                                      column accessor
//    colEnum          MSCalEnums::colDef               Column enum
//    optional         const Bool&                      true if optional column
// Output to private data:
//
  // Convert to column name
  String colName = MSC::fieldName (colEnum);

  // Check if an optional column
  if (optional) {
    if (asTable(calTable).tableDesc().isColumn(colName)) {
      tabCol.attach (asTable(calTable), colName);
    };
  } else {
    tabCol.attach (asTable(calTable), colName);
  };
};

//----------------------------------------------------------------------------

void ROCalMainColumns::attach (const CalTable& calTable, 
			       ROArrayMeasColumn<MEpoch>& tabCol, 
			       MSCalEnums::colDef colEnum, 
			       const Bool& optional)
{
// Attach a column accessor to the calibration table
// Input:
//    calTable         const CalTable&                  Calibration table
//    tabCol           ROArrayMeasColumn<MEpoch>&       Table measures 
//                                                      column accessor
//    colEnum          MSCalEnums::colDef               Column enum
//    optional         const Bool&                      true if optional column
// Output to private data:
//
  // Convert to column name
  String colName = MSC::fieldName (colEnum);

  // Check if an optional column
  if (optional) {
    if (asTable(calTable).tableDesc().isColumn(colName)) {
      tabCol.attach (asTable(calTable), colName);
    };
  } else {
    tabCol.attach (asTable(calTable), colName);
  };
};

//----------------------------------------------------------------------------

void ROCalMainColumns::attach (const CalTable& calTable, 
			       ROArrayMeasColumn<MDirection>& tabCol, 
			       MSCalEnums::colDef colEnum, 
			       const Bool& optional)
{
// Attach a column accessor to the calibration table
// Input:
//    calTable         const CalTable&                  Calibration table
//    tabCol           ROArrayMeasColumn<MDirection>&   Table measures 
//                                                      column accessor
//    colEnum          MSCalEnums::colDef               Column enum
//    optional         const Bool&                      true if optional column
// Output to private data:
//
  // Convert to column name
  String colName = MSC::fieldName (colEnum);

  // Check if an optional column
  if (optional) {
    if (asTable(calTable).tableDesc().isColumn(colName)) {
      tabCol.attach (asTable(calTable), colName);
    };
  } else {
    tabCol.attach (asTable(calTable), colName);
  };
};

//----------------------------------------------------------------------------

void ROCalMainColumns::attach (const CalTable& calTable, 
			       ROScalarMeasColumn<MEpoch>& tabCol, 
			       MSCalEnums::colDef colEnum, 
			       const Bool& optional)
{
// Attach a column accessor to the calibration table
// Input:
//    calTable         const CalTable&              Calibration table
//    tabCol           ROScalarMeasColumn<MEpoch>&  Table measures column 
//                                                  accessor
//    colEnum          MSCalEnums::colDef           Column enum
//    optional         const Bool&                  true if optional column
// Output to private data:
//
  // Convert to column name
  String colName = MSC::fieldName (colEnum);

  // Check if an optional column
  if (optional) {
    if (asTable(calTable).tableDesc().isColumn(colName)) {
      tabCol.attach (asTable(calTable), colName);
    };
  } else {
    tabCol.attach (asTable(calTable), colName);
  };
};

//----------------------------------------------------------------------------

void ROCalMainColumns::attach (const CalTable& calTable, 
			       ROScalarQuantColumn<Double>& tabCol, 
			       MSCalEnums::colDef colEnum, 
			       const Bool& optional)
{
// Attach a column accessor to the calibration table
// Input:
//    calTable    const CalTable&                Calibration table
//    tabCol      ROScalarQuantColumn<Double>&   Scalar quantum column accessor
//    colEnum     MSCalEnums::colDef             Column enum
//    optional    const Bool&                    true if optional column
// Output to private data:
//
  // Convert to column name
  String colName = MSC::fieldName (colEnum);

  // Check if an optional column
  if (optional) {
    if (asTable(calTable).tableDesc().isColumn(colName)) {
      tabCol.attach (asTable(calTable), colName);
    };
  } else {
    tabCol.attach (asTable(calTable), colName);
  };
};

//----------------------------------------------------------------------------

CalMainColumns::CalMainColumns (CalTable& calTable) 
{
// Construct from a calibration table
// Input:
// Input:
//    calTable         const CalTable&              Calibration table
// Output to private data:
//    time_p           ScalarColumn<Double>&        Time
//    timeMeas_p       ScalarMeasColumn<MEpoch>&    Time as a Measure
//    timeEP_p         ScalarColumn<Double>&        Extended precision time
//    timeEPQuant_p    ScalarQuantColumn<Double>&   EP time as Quantum
//    interval_p       ScalarColumn<Double>&        Time interval
//    intervalQuant_p  ScalarQuantColumn<Double>&   Time interval as Quantum
//    antenna1_p       ScalarColumn<Int>&           Antenna 1
//    feed1_p          ScalarColumn<Int>&           Feed 1
//    fieldId_p        ScalarColumn<Int>&           Field id.
//    arrayId_p        ScalarColumn<Int>&           Array id.
//    obsId_p          ScalarColumn<Int>&           Observation id.
//    scanNo_p         ScalarColumn<Int>&           Scan no.
//    processorId_p    ScalarColumn<Int>&           Processor id.
//    stateId_p        ScalarColumn<Int>&           State id.
//    phaseId_p        ScalarColumn<Int>&           Phase id.
//    pulsarBin_p      ScalarColumn<Int>&           Pulsar bin
//    pulsarGateId_p   ScalarColumn<Int>&           Pulsar gate id.
//    freqGrp_p        ScalarColumn<Int>&           Frequency group
//    freqGrpName_p    ScalarColumn<String>&        Frequency group name
//    fieldName_p      ScalarColumn<String>&        Field name
//    fieldCode_p      ScalarColumn<String>&        Field code
//    sourceName_p     ScalarColumn<String>&        Source name
//    sourceCode_p     ScalarColumn<String>&        Source code
//    calGrp_p         ScalarColumn<Int>&           Calibration group
//    gain_p           ArrayColumn<Complex>&        Gain
//    refAnt_p         ArrayColumn<Int>&            Reference antenna
//    refFeed_p        ArrayColumn<Int>&            Reference feed
//    refReceptor_p    ArrayColumn<Int>&            Reference receptor
//    refFreq_p        ArrayColumn<Double>&         Reference frequency
//    refFreqMeas_p    ArrayMeasColumn<MFrequency>& Ref. freq. as Measure
//    measFreqRef_p    ScalarColumn<Int>&           Frequency measures ref.
//    refDir_p         ArrayColumn<Double>&         Reference direction
//    refDirMeas_p     ArrayMeasColumn<MDirection>& Ref. dir. as Measure
//    measDirRef_p     ScalarColumn<Int>&           Direction measures ref.
//    calDescId_p      ScalarColumn<Int>&           CAL_DESC id.
//    calHistoryId_p   ScalarColumn<Int>&           CAL_HISTORY id.
//
  // Attach all the column accessors (including required and
  // optional columns)
  attach (calTable, time_p, MSC::TIME);
  attach (calTable, timeMeas_p, MSC::TIME);
  attach (calTable, timeEP_p, MSC::TIME_EXTRA_PREC, true);
  attach (calTable, timeEPQuant_p, MSC::TIME_EXTRA_PREC, true);
  attach (calTable, interval_p, MSC::INTERVAL);
  attach (calTable, intervalQuant_p, MSC::INTERVAL);
  attach (calTable, antenna1_p, MSC::ANTENNA1);
  attach (calTable, feed1_p, MSC::FEED1);
  attach (calTable, fieldId_p, MSC::FIELD_ID);
  attach (calTable, arrayId_p, MSC::ARRAY_ID);
  attach (calTable, obsId_p, MSC::OBSERVATION_ID);
  attach (calTable, scanNo_p, MSC::SCAN_NUMBER);
  attach (calTable, processorId_p, MSC::PROCESSOR_ID);
  attach (calTable, stateId_p, MSC::STATE_ID);
  attach (calTable, phaseId_p, MSC::PHASE_ID, true);
  attach (calTable, pulsarBin_p, MSC::PULSAR_BIN, true);
  attach (calTable, pulsarGateId_p, MSC::PULSAR_GATE_ID, true);
  attach (calTable, freqGrp_p, MSC::FREQ_GROUP);
  attach (calTable, freqGrpName_p, MSC::FREQ_GROUP_NAME);
  attach (calTable, fieldName_p, MSC::FIELD_NAME);
  attach (calTable, fieldCode_p, MSC::FIELD_CODE);
  attach (calTable, sourceName_p, MSC::SOURCE_NAME);
  attach (calTable, sourceCode_p, MSC::SOURCE_CODE);
  attach (calTable, calGrp_p, MSC::CALIBRATION_GROUP);
  attach (calTable, gain_p, MSC::GAIN);
  attach (calTable, refAnt_p, MSC::REF_ANT);
  attach (calTable, refFeed_p, MSC::REF_FEED);
  attach (calTable, refReceptor_p, MSC::REF_RECEPTOR);
  attach (calTable, pulsarBin_p, MSC::PULSAR_BIN);
  attach (calTable, pulsarGateId_p, MSC::PULSAR_GATE_ID);
  attach (calTable, freqGrp_p, MSC::FREQ_GROUP);
  attach (calTable, freqGrpName_p, MSC::FREQ_GROUP_NAME);
  attach (calTable, fieldName_p, MSC::FIELD_NAME);
  attach (calTable, fieldCode_p, MSC::FIELD_CODE);
  attach (calTable, sourceName_p, MSC::SOURCE_NAME);
  attach (calTable, sourceCode_p, MSC::SOURCE_CODE);
  attach (calTable, calGrp_p, MSC::CALIBRATION_GROUP);
  attach (calTable, gain_p, MSC::GAIN);
  attach (calTable, refAnt_p, MSC::REF_ANT);
  attach (calTable, refFeed_p, MSC::REF_FEED);
  attach (calTable, refReceptor_p, MSC::REF_RECEPTOR);
  attach (calTable, refFreq_p, MSC::REF_FREQUENCY);
  attach (calTable, refFreqMeas_p, MSC::REF_FREQUENCY);
  attach (calTable, measFreqRef_p, MSC::MEAS_FREQ_REF);
  attach (calTable, refDir_p, MSC::REF_DIRECTION);
  attach (calTable, refDirMeas_p, MSC::REF_DIRECTION);
  attach (calTable, measDirRef_p, MSC::MEAS_DIR_REF);
  attach (calTable, calDescId_p, MSC::CAL_DESC_ID);
  attach (calTable, calHistoryId_p, MSC::CAL_HISTORY_ID);
};

//----------------------------------------------------------------------------

void CalMainColumns::attach (CalTable& calTable, 
			     TableColumn& tabCol, 
			     MSCalEnums::colDef colEnum, 
			     const Bool& optional)
{
// Attach a column accessor to the calibration table
// Input:
//    calTable         CalTable&            Calibration table
//    tabCol           TableColumn&         Table column accessor
//    colEnum          MSCalEnums::colDef   Column enum
//    optional         const Bool&          true if optional column
// Output to private data:
//
  // Convert to column name
  String colName = MSC::fieldName (colEnum);

  // Check if an optional column
  if (optional) {
    if (asTable(calTable).tableDesc().isColumn(colName)) {
      tabCol.attach (asTable(calTable), colName);
    };
  } else {
    tabCol.attach (asTable(calTable), colName);
  };
};

//----------------------------------------------------------------------------

void CalMainColumns::attach (CalTable& calTable, 
			     ArrayMeasColumn<MEpoch>& tabCol, 
			     MSCalEnums::colDef colEnum, 
			     const Bool& optional)
{
// Attach a column accessor to the calibration table
// Input:
//    calTable         CalTable&                     Calibration table
//    tabCol           ArrayMeasColumn<MEpoch>&      Table measures column 
//                                                   accessor
//    colEnum          MSCalEnums::colDef            Column enum
//    optional         const Bool&                   true if optional column
// Output to private data:
//
  // Convert to column name
  String colName = MSC::fieldName (colEnum);

  // Check if an optional column
  if (optional) {
    if (asTable(calTable).tableDesc().isColumn(colName)) {
      tabCol.attach (asTable(calTable), colName);
    };
  } else {
    tabCol.attach (asTable(calTable), colName);
  };
};

//----------------------------------------------------------------------------

void CalMainColumns::attach (CalTable& calTable, 
			     ArrayMeasColumn<MFrequency>& tabCol, 
			     MSCalEnums::colDef colEnum, 
			     const Bool& optional)
{
// Attach a column accessor to the calibration table
// Input:
//    calTable         CalTable&                     Calibration table
//    tabCol           ArrayMeasColumn<MFrequency>&  Table measures column 
//                                                   accessor
//    colEnum          MSCalEnums::colDef            Column enum
//    optional         const Bool&                   true if optional column
// Output to private data:
//
  // Convert to column name
  String colName = MSC::fieldName (colEnum);

  // Check if an optional column
  if (optional) {
    if (asTable(calTable).tableDesc().isColumn(colName)) {
      tabCol.attach (asTable(calTable), colName);
    };
  } else {
    tabCol.attach (asTable(calTable), colName);
  };
};

//----------------------------------------------------------------------------

void CalMainColumns::attach (CalTable& calTable, 
			     ArrayMeasColumn<MDirection>& tabCol, 
			     MSCalEnums::colDef colEnum, 
			     const Bool& optional)
{
// Attach a column accessor to the calibration table
// Input:
//    calTable         CalTable&                     Calibration table
//    tabCol           ArrayMeasColumn<MDirection>&  Table measures column 
//                                                   accessor
//    colEnum          MSCalEnums::colDef            Column enum
//    optional         const Bool&                   true if optional column
// Output to private data:
//
  // Convert to column name
  String colName = MSC::fieldName (colEnum);

  // Check if an optional column
  if (optional) {
    if (asTable(calTable).tableDesc().isColumn(colName)) {
      tabCol.attach (asTable(calTable), colName);
    };
  } else {
    tabCol.attach (asTable(calTable), colName);
  };
};

//----------------------------------------------------------------------------

void CalMainColumns::attach (CalTable& calTable, 
			     ScalarMeasColumn<MEpoch>& tabCol, 
			     MSCalEnums::colDef colEnum, 
			     const Bool& optional)
{
// Attach a column accessor to the calibration table
// Input:
//    calTable         CalTable&                   Calibration table
//    tabCol           ScalarMeasColumn<MEpoch>&   Table measures column 
//                                                 accessor
//    colEnum          MSCalEnums::colDef          Column enum
//    optional         const Bool&                 true if optional column
// Output to private data:
//
  // Convert to column name
  String colName = MSC::fieldName (colEnum);

  // Check if an optional column
  if (optional) {
    if (asTable(calTable).tableDesc().isColumn(colName)) {
      tabCol.attach (asTable(calTable), colName);
    };
  } else {
    tabCol.attach (asTable(calTable), colName);
  };
};

//----------------------------------------------------------------------------

void CalMainColumns::attach (CalTable& calTable, 
			     ScalarQuantColumn<Double>& tabCol, 
			     MSCalEnums::colDef colEnum, 
			     const Bool& optional)
{
// Attach a column accessor to the calibration table
// Input:
//    calTable    CalTable&                    Calibration table
//    tabCol      ScalarQuantColumn<Double>&   Table measures column accessor
//    colEnum     MSCalEnums::colDef           Column enum
//    optional    const Bool&                  true if optional column
// Output to private data:
//
  // Convert to column name
  String colName = MSC::fieldName (colEnum);

  // Check if an optional column
  if (optional) {
    if (asTable(calTable).tableDesc().isColumn(colName)) {
      tabCol.attach (asTable(calTable), colName);
    };
  } else {
    tabCol.attach (asTable(calTable), colName);
  };
};

//----------------------------------------------------------------------------


} //# NAMESPACE CASA - END

