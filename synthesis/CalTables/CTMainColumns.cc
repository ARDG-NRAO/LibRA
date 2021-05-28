//# CTMainColumns.cc: Implementation of CTMainColumns.h
//# Copyright (C) 2011
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

#include <synthesis/CalTables/CTMainColumns.h>
#include <synthesis/CalTables/RIorAParray.h>
#include <casa/Arrays/ArrayMath.h>


using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

//----------------------------------------------------------------------------

CTMainColumns::CTMainColumns(NewCalTable& calTable) 
{
// Construct from a new calibration table
// Input:
// Input:
//    calTable         const NewCalTable&           Calibration table
// Output to private data:
//    time_p           ScalarColumn<Double>&        Time
//    timeMeas_p       ScalarMeasColumn<MEpoch>&    Time as a Measure
//    timeEP_p         ScalarColumn<Double>&        Extended precision time
//    timeEPQuant_p    ScalarQuantColumn<Double>&   EP time as Quantum
//    interval_p       ScalarColumn<Double>&        Time interval
//    intervalQuant_p  ScalarQuantColumn<Double>&   Time interval as Quantum
//    antenna1_p       ScalarColumn<Int>&           Antenna 1
//    antenna2_p       ScalarColumn<Int>&           Antenna 2
//    fieldId_p        ScalarColumn<Int>&           Field id.
//    spwId_p          ScalarColumn<Int>&           Spectral window id.
//    scanNo_p         ScalarColumn<Int>&           Scan no.
//    obsId_p          ScalarColumn<Int>&           Obs Id
//    param_p          ArrayColumn<Complex>&        Cal solution params.
//    paramerr_p       ArrayColumn<Float>&          Cal solution error
//    flag_p           ArrayColumn<Bool>&           Ref. dir. as Measure
//    snr_p            ArrayColumn<Float>&          Solution SNR
//    weight_p         ArrayColumn<Float>&          Solution weight
//
  // Attach all the column accessors (including required and
  // optional columns)
  attach (calTable, time_p, NCT::TIME);
  attach (calTable, timeQuant_p, NCT::TIME);
  attach (calTable, timeMeas_p, NCT::TIME);
  attach (calTable, timeEP_p, NCT::TIME_EXTRA_PREC, true);
  attach (calTable, timeEPQuant_p, NCT::TIME_EXTRA_PREC, true);
  attach (calTable, interval_p, NCT::INTERVAL);
  attach (calTable, intervalQuant_p, NCT::INTERVAL);
  attach (calTable, fieldId_p, NCT::FIELD_ID);
  attach (calTable, spwId_p, NCT::SPECTRAL_WINDOW_ID);
  attach (calTable, antenna1_p, NCT::ANTENNA1);
  attach (calTable, antenna2_p, NCT::ANTENNA2);
  attach (calTable, scanNo_p, NCT::SCAN_NUMBER);
  attach (calTable, obsId_p, NCT::OBSERVATION_ID,true); // optional for now

  // {C,F}PARAM columns are optional, exactly one should
  //   be present
  attach (calTable, cparam_p, NCT::CPARAM, true);
  attach (calTable, fparam_p, NCT::FPARAM, true);

  attach (calTable, paramerr_p, NCT::PARAMERR);
  attach (calTable, flag_p, NCT::FLAG);
  attach (calTable, snr_p, NCT::SNR);
  attach (calTable, weight_p, NCT::WEIGHT);
  //cerr<<"CTMainColumns instantiated"<<endl;
  //cerr<<"CTMainColumns ctor: caltable name="<<calTable.tableName()<<endl;
};

//----------------------------------------------------------------------------

// Some additional methods to extract into Array<Float> generically
//   what can be: "","AP",...TBD
Array<Float> ROCTMainColumns::fparamArray(String what,const Vector<uInt>& rows) {
  // Delegate to in-place method
  Array<Float> f;
  fparamArray(f,what,rows);
  return f;
}
void ROCTMainColumns::fparamArray(Array<Float>& arr,String what,const Vector<uInt>& rows) {
  Bool byrow=(rows.nelements()>0);
  if (what=="") {
    // Get out Float values however possible
    if (!fparam().isNull()) {
      if (byrow)
	fparam().getColumnCells(RefRows(rows),arr);
      else
	fparam().getColumn(arr);
    }
    else if (!cparam().isNull()) {
      // Get amp/phase (tracked) by default
      if (byrow) {
	RIorAPArray ap(cparam().getColumnCells(RefRows(rows)));
	arr=ap.f(true);  // Tracks phase
      }
      else {
	RIorAPArray ap(cparam().getColumn());
	arr=ap.f(true);  // Tracks phase
      }
    }
    else
      throw(AipsError("Someting wrong with the caltable!"));

    return;
  }
  if (what=="AP") {
    // Convert to Amp/Ph, with Ph tracked
    if (byrow) {
      RIorAPArray ap(cparam().getColumnCells(RefRows(rows)));
      arr=ap.f(true);  // Tracks phase
    }
    else {
      RIorAPArray ap(cparam().getColumn());
      arr=ap.f(true);  // Tracks phase
    }
    return;
  }
}

Array<Float> ROCTMainColumns::phase() {
  Array<Float> ph;
  this->phase(ph);
  return ph;
}

void ROCTMainColumns::phase(Array<Float>& ph) {
  if (!cparam().isNull()) 
    ph.assign(casacore::phase(cparam().getColumn()));
  else
    throw(AipsError("ROCTMainColumns cannot extract phase without CPARAM column."));
}

Array<Float> ROCTMainColumns::cycles() {
  Array<Float> c;
  this->cycles(c);
  return c;
}

void ROCTMainColumns::cycles(Array<Float>& cy) {
  this->phase(cy);    // Extract phase
  cy+=Float(C::pi);   // add a half-cycle
  cy/=Float(C::_2pi); // divide by rad/cycle
  cy=floor(cy);       // integer part is cycles
  cy*=Float(C::_2pi); // in radians
}


//----------------------------------------------------------------------------

void CTMainColumns::attach (NewCalTable& calTable, 
				 TableColumn& tabCol, 
				 CTEnums::colDef colEnum, 
				 const Bool& optional)
{
// Attach a column accessor to the calibration table
// Input:
//    calTable         CalTable&                  Calibration table
//    tabCol           TableColumn&               Table column accessor
//    colEnum          CTEnums::colDef   Column enum
//    optional         const Bool&                true if optional column
// Output to private data:
//
  // Convert to column name
  String colName = NCT::fieldName (colEnum);

  // Check if an optional column
  if (optional) {
    if (calTable.tableDesc().isColumn(colName)) {
      tabCol.attach (calTable, colName);
    };
  } else {
    tabCol.attach (calTable, colName);
  };
};

//----------------------------------------------------------------------------

void CTMainColumns::attach (NewCalTable& calTable, 
				 ArrayMeasColumn<MEpoch>& tabCol, 
				 CTEnums::colDef colEnum, 
				 const Bool& optional)
{
// Attach a column accessor to the calibration table
// Input:
//    calTable         NewCalTable&                  Calibration table
//    tabCol           ArrayMeasColumn<MEpoch>&      Table measures column 
//                                                   accessor
//    colEnum          CTEnums::colDef      Column enum
//    optional         const Bool&                   true if optional column
// Output to private data:
//
  // Convert to column name
  String colName = NCT::fieldName (colEnum);

  // Check if an optional column
  if (optional) {
    if (calTable.tableDesc().isColumn(colName)) {
      tabCol.attach (calTable, colName);
    };
  } else {
    tabCol.attach (calTable, colName);
  };
};

//----------------------------------------------------------------------------

void CTMainColumns::attach (NewCalTable& calTable, 
				 ArrayMeasColumn<MFrequency>& tabCol, 
				 CTEnums::colDef colEnum, 
				 const Bool& optional)
{
// Attach a column accessor to the calibration table
// Input:
//    calTable         CalTable&                     Calibration table
//    tabCol           ArrayMeasColumn<MFrequency>&  Table measures column 
//                                                   accessor
//    colEnum          CTEnums::colDef            Column enum
//    optional         const Bool&                   true if optional column
// Output to private data:
//
  // Convert to column name
  String colName = NCT::fieldName (colEnum);

  // Check if an optional column
  if (optional) {
    if (calTable.tableDesc().isColumn(colName)) {
      tabCol.attach (calTable, colName);
    };
  } else {
    tabCol.attach (calTable, colName);
  };
};

//----------------------------------------------------------------------------

void CTMainColumns::attach (NewCalTable& calTable, 
				 ArrayMeasColumn<MDirection>& tabCol, 
				 CTEnums::colDef colEnum, 
				 const Bool& optional)
{
// Attach a column accessor to the calibration table
// Input:
//    calTable         NewCalTable&                  Calibration table
//    tabCol           ArrayMeasColumn<MDirection>&  Table measures column 
//                                                   accessor
//    colEnum          CTEnums::colDef            Column enum
//    optional         const Bool&                   true if optional column
// Output to private data:
//
  // Convert to column name
  String colName = NCT::fieldName (colEnum);

  // Check if an optional column
  if (optional) {
    if (calTable.tableDesc().isColumn(colName)) {
      tabCol.attach (calTable, colName);
    };
  } else {
    tabCol.attach (calTable, colName);
  };
};

//----------------------------------------------------------------------------

void CTMainColumns::attach (NewCalTable& calTable, 
				 ScalarMeasColumn<MEpoch>& tabCol, 
				 CTEnums::colDef colEnum, 
				 const Bool& optional)
{
// Attach a column accessor to the calibration table
// Input:
//    calTable         CalTable&                   Calibration table
//    tabCol           ScalarMeasColumn<MEpoch>&   Table measures column 
//                                                 accessor
//    colEnum          CTEnums::colDef          Column enum
//    optional         const Bool&                 true if optional column
// Output to private data:
//
  // Convert to column name
  String colName = NCT::fieldName (colEnum);

  // Check if an optional column
  if (optional) {
    if (calTable.tableDesc().isColumn(colName)) {
      tabCol.attach (calTable, colName);
    };
  } else {
    tabCol.attach (calTable, colName);
  };
};

//----------------------------------------------------------------------------

void CTMainColumns::attach (NewCalTable& calTable, 
				 ScalarQuantColumn<Double>& tabCol, 
				 CTEnums::colDef colEnum, 
				 const Bool& optional)
{
// Attach a column accessor to the calibration table
// Input:
//    calTable    CalTable&                    Calibration table
//    tabCol      ScalarQuantColumn<Double>&   Table measures column accessor
//    colEnum     CTEnums::colDef           Column enum
//    optional    const Bool&                  true if optional column
// Output to private data:
//
  // Convert to column name
  String colName = NCT::fieldName (colEnum);

  // Check if an optional column
  if (optional) {
    if (calTable.tableDesc().isColumn(colName)) {
      tabCol.attach (calTable, colName);
    };
  } else {
    tabCol.attach (calTable, colName);
  };
};

//----------------------------------------------------------------------------

//----------------------------------------------------------------------------

ROCTMainColumns::ROCTMainColumns(const NewCalTable& calTable) 
{
// Construct from a calibration table
// Input:
//    calTable         const NewCalTable&                Calibration table
// Output to private data:
//    time_p           ScalarColumn<Double>&        Time
//    timeMeas_p       ROScalarMeasColumn<MEpoch>&    Time as a Measure
//    timeEP_p         ScalarColumn<Double>&        Extended precision time
//    timeEPQuant_p    ROScalarQuantColumn<Double>&   EP time as Quantum
//    interval_p       ScalarColumn<Double>&        Time interval
//    intervalQuant_p  ROScalarQuantColumn<Double>&   Time interval as Quantum
//    antenna1_p       ScalarColumn<Int>&           Antenna 1
//    antenna2_p       ScalarColumn<Int>&           Antenna 2
//    fieldId_p        ScalarColumn<Int>&           Field id.
//    spwId_p          ScalarColumn<Int>&           Spectral Window id.
//    scanNo_p         ScalarColumn<Int>&           Scan no.
//    obsId_p          ScalarColumn<Int>&           Obs Id
//    param_p          ArrayColumn<Complex>&          Solution params
//    paramerr_p       ArrayColumn<Float>&          Solution param error
//    flag_p           ArrayColumn<Bool>&           flag
//    snr_p            ArrayColumn<Float>&          Solution SNR
//    weight_p         ArrayColumn<Float>&          Solution weight
//
  // Attach all the column accessors (including required and
  // optional columns)
  attach (calTable, time_p, NCT::TIME);
  attach (calTable, timeQuant_p, NCT::TIME);
  attach (calTable, timeMeas_p, NCT::TIME);
  attach (calTable, timeEP_p, NCT::TIME_EXTRA_PREC, true);
  attach (calTable, timeEPQuant_p, NCT::TIME_EXTRA_PREC, true);
  attach (calTable, interval_p, NCT::INTERVAL);
  attach (calTable, intervalQuant_p, NCT::INTERVAL);
  attach (calTable, antenna1_p, NCT::ANTENNA1);
  attach (calTable, antenna2_p, NCT::ANTENNA2);
  attach (calTable, fieldId_p, NCT::FIELD_ID);
  attach (calTable, spwId_p, NCT::SPECTRAL_WINDOW_ID);
  attach (calTable, scanNo_p, NCT::SCAN_NUMBER);
  attach (calTable, obsId_p, NCT::OBSERVATION_ID,true);

  // {C,F}PARAM columns are optional, exactly one should
  //   be present
  attach (calTable, cparam_p, NCT::CPARAM, true);
  attach (calTable, fparam_p, NCT::FPARAM, true);

  attach (calTable, paramerr_p, NCT::PARAMERR);
  attach (calTable, flag_p, NCT::FLAG);
  attach (calTable, snr_p, NCT::SNR);
  attach (calTable, weight_p, NCT::WEIGHT);
};

//----------------------------------------------------------------------------

void ROCTMainColumns::attach (const NewCalTable& calTable, 
				   TableColumn& tabCol, 
				   CTEnums::colDef colEnum, 
				   const Bool& optional)
{
// Attach a column accessor to the calibration table
// Input:
//    calTable         const NewCalTable&         Calibration table
//    tabCol           TableColumn&             Table column accessor
//    colEnum          CTEnums::colDef   Column enum
//    optional         const Bool&                true if optional column
// Output to private data:
//
  // Convert to column name
  String colName = NCT::fieldName (colEnum);

  // Check if an optional column
  if (optional) {
    if (calTable.tableDesc().isColumn(colName)) {
      tabCol.attach (calTable, colName);
    };
  } else {
    tabCol.attach (calTable, colName);
  };
};

//----------------------------------------------------------------------------

void ROCTMainColumns::attach (const NewCalTable& calTable, 
				   ROArrayMeasColumn<MFrequency>& tabCol, 
				   CTEnums::colDef colEnum, 
				   const Bool& optional)
{
// Attach a column accessor to the calibration table
// Input:
//    calTable         const NewCalTable&               Calibration table
//    tabCol           ROArrayMeasColumn<MFrequency>&   Table measures 
//                                                      column accessor
//    colEnum          CTEnums::colDef         Column enum
//    optional         const Bool&                      true if optional column
// Output to private data:
//
  // Convert to column name
  String colName = NCT::fieldName (colEnum);

  // Check if an optional column
  if (optional) {
    if (calTable.tableDesc().isColumn(colName)) {
      tabCol.attach (calTable, colName);
    };
  } else {
    tabCol.attach (calTable, colName);
  };
};

//----------------------------------------------------------------------------

void ROCTMainColumns::attach (const NewCalTable& calTable, 
				   ROArrayMeasColumn<MEpoch>& tabCol, 
				   CTEnums::colDef colEnum, 
				   const Bool& optional)
{
// Attach a column accessor to the calibration table
// Input:
//    calTable         const NewCalTable&               Calibration table
//    tabCol           ROArrayMeasColumn<MEpoch>&       Table measures 
//                                                      column accessor
//    colEnum          CTEnums::colDef         Column enum
//    optional         const Bool&                      true if optional column
// Output to private data:
//
  // Convert to column name
  String colName = NCT::fieldName (colEnum);

  // Check if an optional column
  if (optional) {
    if (calTable.tableDesc().isColumn(colName)) {
      tabCol.attach (calTable, colName);
    };
  } else {
    tabCol.attach (calTable, colName);
  };
};

//----------------------------------------------------------------------------

void ROCTMainColumns::attach (const NewCalTable& calTable, 
				   ROArrayMeasColumn<MDirection>& tabCol, 
				   CTEnums::colDef colEnum, 
				   const Bool& optional)
{
// Attach a column accessor to the calibration table
// Input:
//    calTable         const NewCalTable&               Calibration table
//    tabCol           ROArrayMeasColumn<MDirection>&   Table measures 
//                                                      column accessor
//    colEnum          CTEnums::colDef         Column enum
//    optional         const Bool&                      true if optional column
// Output to private data:
//
  // Convert to column name
  String colName = NCT::fieldName (colEnum);

  // Check if an optional column
  if (optional) {
    if (calTable.tableDesc().isColumn(colName)) {
      tabCol.attach (calTable, colName);
    };
  } else {
    tabCol.attach (calTable, colName);
  };
};

//----------------------------------------------------------------------------

void ROCTMainColumns::attach (const NewCalTable& calTable, 
				   ROScalarMeasColumn<MEpoch>& tabCol, 
				   CTEnums::colDef colEnum, 
				   const Bool& optional)
{
// Attach a column accessor to the calibration table
// Input:
//    calTable         const NewCalTable&           Calibration table
//    tabCol           ROScalarMeasColumn<MEpoch>&  Table measures column 
//                                                  accessor
//    colEnum          CTEnums::colDef     Column enum
//    optional         const Bool&                  true if optional column
// Output to private data:
//
  // Convert to column name
  String colName = NCT::fieldName (colEnum);

  // Check if an optional column
  if (optional) {
    if (calTable.tableDesc().isColumn(colName)) {
      tabCol.attach (calTable, colName);
    };
  } else {
    tabCol.attach (calTable, colName);
  };
};

//----------------------------------------------------------------------------

void ROCTMainColumns::attach (const NewCalTable& calTable, 
			      ROScalarQuantColumn<Double>& tabCol, 
			      CTEnums::colDef colEnum, 
			      const Bool& optional)
{
// Attach a column accessor to the calibration table
// Input:
//    calTable    const NewCalTable&             Calibration table
//    tabCol      ROScalarQuantColumn<Double>&   Scalar quantum column accessor
//    colEnum     CTEnums::colDef       Column enum
//    optional    const Bool&                    true if optional column
// Output to private data:
//
  // Convert to column name
  String colName = NCT::fieldName (colEnum);

  // Check if an optional column
  if (optional) {
    if (calTable.tableDesc().isColumn(colName)) {
      tabCol.attach (calTable, colName);
    };
  } else {
    tabCol.attach (calTable, colName);
  };
};

} //# NAMESPACE CASA - END

