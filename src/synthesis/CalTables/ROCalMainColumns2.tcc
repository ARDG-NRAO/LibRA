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

#include <synthesis/CalTables/ROCalMainColumns2.h>

namespace casa { //# NAMESPACE CASA - BEGIN

//----------------------------------------------------------------------------

template<class T> 
ROCalMainColumns2<T>::ROCalMainColumns2(const CalTable2& calTable) 
{
// Construct from a calibration table
// casacore::Input:
//    calTable         const CalTable&                Calibration table
// Output to private data:
//    time_p           casacore::ScalarColumn<casacore::Double>&        Time
//    timeMeas_p       casacore::ROScalarMeasColumn<casacore::MEpoch>&    casacore::Time as a Measure
//    timeEP_p         casacore::ScalarColumn<casacore::Double>&        Extended precision time
//    timeEPQuant_p    casacore::ROScalarQuantColumn<casacore::Double>&   EP time as Quantum
//    interval_p       casacore::ScalarColumn<casacore::Double>&        casacore::Time interval
//    intervalQuant_p  casacore::ROScalarQuantColumn<casacore::Double>&   casacore::Time interval as Quantum
//    antenna1_p       casacore::ScalarColumn<casacore::Int>&           Antenna 1
//    feed1_p          casacore::ScalarColumn<casacore::Int>&           Feed 1
//    fieldId_p        casacore::ScalarColumn<casacore::Int>&           Field id.
//    arrayId_p        casacore::ScalarColumn<casacore::Int>&           casacore::Array id.
//    obsId_p          casacore::ScalarColumn<casacore::Int>&           Observation id.
//    scanNo_p         casacore::ScalarColumn<casacore::Int>&           Scan no.
//    processorId_p    casacore::ScalarColumn<casacore::Int>&           Processor id.
//    stateId_p        casacore::ScalarColumn<casacore::Int>&           State id.
//    phaseId_p        casacore::ScalarColumn<casacore::Int>&           Phase id.
//    pulsarBin_p      casacore::ScalarColumn<casacore::Int>&           Pulsar bin
//    pulsarGateId_p   casacore::ScalarColumn<casacore::Int>&           Pulsar gate id.
//    freqGrp_p        casacore::ScalarColumn<casacore::Int>&           Frequency group
//    freqGrpName_p    casacore::ScalarColumn<casacore::String>&        Frequency group name
//    fieldName_p      casacore::ScalarColumn<casacore::String>&        Field name
//    fieldCode_p      casacore::ScalarColumn<casacore::String>&        Field code
//    sourceName_p     casacore::ScalarColumn<casacore::String>&        Source name
//    sourceCode_p     casacore::ScalarColumn<casacore::String>&        Source code
//    calGrp_p         casacore::ScalarColumn<casacore::Int>&           Calibration group
//    gain_p           casacore::ArrayColumn<casacore::Complex>&        Gain
//    refAnt_p         casacore::ArrayColumn<casacore::Int>&            Reference antenna
//    refFeed_p        casacore::ArrayColumn<casacore::Int>&            Reference feed
//    refReceptor_p    casacore::ArrayColumn<casacore::Int>&            Reference receptor
//    refFreq_p        casacore::ArrayColumn<casacore::Double>&         Reference frequency
//    refFreqMeas_p    casacore::ROArrayMeasColumn<casacore::MFrequency>& Ref. freq. as Measure
//    measFreqRef_p    casacore::ScalarColumn<casacore::Int>&           Frequency measures ref.
//    refDir_p         casacore::ArrayColumn<casacore::Double>&         Reference direction
//    refDirMeas_p     casacore::ROArrayMeasColumn<casacore::MDirection>& Ref. dir. as Measure
//    measDirRef_p     casacore::ScalarColumn<casacore::Int>&           Direction measures ref.
//    calDescId_p      casacore::ScalarColumn<casacore::Int>&           CAL_DESC id.
//    calHistoryId_p   casacore::ScalarColumn<casacore::Int>&           CAL_HISTORY id.
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
//   casacore::Int pType = ((CalTable2)calTable).parType();
//   if (pType == 0) attach (calTable, gain_p, MSC::GAIN);
//   else attach (calTable, solvePar_p, MSC::SOLVE_PAR);
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

template<class T>
void ROCalMainColumns2<T>::attach (const CalTable2& calTable, 
				   casacore::TableColumn& tabCol, 
				   MSCalEnums::colDef colEnum, 
				   const casacore::Bool& optional)
{
// Attach a column accessor to the calibration table
// casacore::Input:
//    calTable         const CalTable&      Calibration table
//    tabCol           casacore::TableColumn&       casacore::Table column accessor
//    colEnum          MSCalEnums::colDef   Column enum
//    optional         const casacore::Bool&          true if optional column
// Output to private data:
//
  // Convert to column name
  casacore::String colName = MSC::fieldName (colEnum);

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

template<class T>
void ROCalMainColumns2<T>::attach (const CalTable2& calTable, 
				   casacore::ROArrayMeasColumn<casacore::MFrequency>& tabCol,
				   MSCalEnums::colDef colEnum, 
				   const casacore::Bool& optional)
{
// Attach a column accessor to the calibration table
// casacore::Input:
//    calTable         const CalTable&                  Calibration table
//    tabCol           casacore::ROArrayMeasColumn<casacore::MFrequency>&   casacore::Table measures
//                                                      column accessor
//    colEnum          MSCalEnums::colDef               Column enum
//    optional         const casacore::Bool&                      true if optional column
// Output to private data:
//
  // Convert to column name
  casacore::String colName = MSC::fieldName (colEnum);

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

template<class T>
void ROCalMainColumns2<T>::attach (const CalTable2& calTable, 
				   casacore::ROArrayMeasColumn<casacore::MEpoch>& tabCol,
				   MSCalEnums::colDef colEnum, 
				   const casacore::Bool& optional)
{
// Attach a column accessor to the calibration table
// casacore::Input:
//    calTable         const CalTable&                  Calibration table
//    tabCol           casacore::ROArrayMeasColumn<casacore::MEpoch>&       casacore::Table measures
//                                                      column accessor
//    colEnum          MSCalEnums::colDef               Column enum
//    optional         const casacore::Bool&                      true if optional column
// Output to private data:
//
  // Convert to column name
  casacore::String colName = MSC::fieldName (colEnum);

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

template<class T>
void ROCalMainColumns2<T>::attach (const CalTable2& calTable, 
				   casacore::ROArrayMeasColumn<casacore::MDirection>& tabCol,
				   MSCalEnums::colDef colEnum, 
				   const casacore::Bool& optional)
{
// Attach a column accessor to the calibration table
// casacore::Input:
//    calTable         const CalTable&                  Calibration table
//    tabCol           casacore::ROArrayMeasColumn<casacore::MDirection>&   casacore::Table measures
//                                                      column accessor
//    colEnum          MSCalEnums::colDef               Column enum
//    optional         const casacore::Bool&                      true if optional column
// Output to private data:
//
  // Convert to column name
  casacore::String colName = MSC::fieldName (colEnum);

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

template<class T>
void ROCalMainColumns2<T>::attach (const CalTable2& calTable, 
				   casacore::ROScalarMeasColumn<casacore::MEpoch>& tabCol,
				   MSCalEnums::colDef colEnum, 
				   const casacore::Bool& optional)
{
// Attach a column accessor to the calibration table
// casacore::Input:
//    calTable         const CalTable&              Calibration table
//    tabCol           casacore::ROScalarMeasColumn<casacore::MEpoch>&  casacore::Table measures column
//                                                  accessor
//    colEnum          MSCalEnums::colDef           Column enum
//    optional         const casacore::Bool&                  true if optional column
// Output to private data:
//
  // Convert to column name
  casacore::String colName = MSC::fieldName (colEnum);

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

template<class T>
void ROCalMainColumns2<T>::attach (const CalTable2& calTable, 
				   casacore::ROScalarQuantColumn<casacore::Double>& tabCol,
				   MSCalEnums::colDef colEnum, 
				   const casacore::Bool& optional)
{
// Attach a column accessor to the calibration table
// casacore::Input:
//    calTable    const CalTable&                Calibration table
//    tabCol      casacore::ROScalarQuantColumn<casacore::Double>&   Scalar quantum column accessor
//    colEnum     MSCalEnums::colDef             Column enum
//    optional    const casacore::Bool&                    true if optional column
// Output to private data:
//
  // Convert to column name
  casacore::String colName = MSC::fieldName (colEnum);

  // Check if an optional column
  if (optional) {
    if (asTable(calTable).tableDesc().isColumn(colName)) {
      tabCol.attach (asTable(calTable), colName);
    };
  } else {
    tabCol.attach (asTable(calTable), colName);
  };
};
};
