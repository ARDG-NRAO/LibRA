//# CalMainColumns.h: Calibration table cal_main column access
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

#ifndef CALIBRATION_CALMAINCOLUMNS_H
#define CALIBRATION_CALMAINCOLUMNS_H

#include <casa/aips.h>
#include <measures/Measures/MEpoch.h>
#include <measures/Measures/MFrequency.h>
#include <measures/Measures/MDirection.h>
#include <tables/Tables/TableColumn.h>
#include <tables/Tables/ScalarColumn.h>
#include <tables/Tables/ArrayColumn.h>
#include <measures/TableMeasures/TableMeasColumn.h>
#include <measures/TableMeasures/ScalarMeasColumn.h>
#include <measures/TableMeasures/ArrayMeasColumn.h>
#include <measures/TableMeasures/ScalarQuantColumn.h>
#include <synthesis/CalTables/CalTable.h>
#include <msvis/MSVis/MSCalEnums.h>

namespace casa { //# NAMESPACE CASA - BEGIN

// <summary> 
// ROCalMainColumns: Read-only cal_main calibration table column access
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//   <li> <linkto class="CalTable">CalTable</linkto> module
// </prerequisite>
//
// <etymology>
// From "read-only", "calibration main table" and "columns".
// </etymology>
//
// <synopsis>
// The ROCalMainColumns class allows read-only access to columns
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
// Encapsulate access to calibration table columns.
// </motivation>
//
// <todo asof="01/07/01">
// (i) Deal with non-standard columns.
// </todo>

class ROCalMainColumns 
{
 public:
  // Construct from a calibration table
  ROCalMainColumns (const CalTable& calTable);

  // Default destructor
  virtual ~ROCalMainColumns() {};

  // Read-only column accessors
  const casacore::ScalarColumn<casacore::Double>& time() const {return time_p;};
  const casacore::ROScalarMeasColumn<casacore::MEpoch>& timeMeas() const {return timeMeas_p;};
  const casacore::ScalarColumn<casacore::Double>& timeEP() const {return timeEP_p;};
  const casacore::ROScalarQuantColumn<casacore::Double>& timeEPQuant() const
    {return timeEPQuant_p;};
  const casacore::ScalarColumn<casacore::Double>& interval() const {return interval_p;};
  const casacore::ROScalarQuantColumn<casacore::Double>& intervalQuant() const
    {return intervalQuant_p;};
  const casacore::ScalarColumn<casacore::Int>& antenna1() const {return antenna1_p;};
  const casacore::ScalarColumn<casacore::Int>& feed1() const {return feed1_p;};
  const casacore::ScalarColumn<casacore::Int>& fieldId() const {return fieldId_p;};
  const casacore::ScalarColumn<casacore::Int>& arrayId() const {return arrayId_p;};
  const casacore::ScalarColumn<casacore::Int>& obsId() const {return obsId_p;};
  const casacore::ScalarColumn<casacore::Int>& scanNo() const {return scanNo_p;};
  const casacore::ScalarColumn<casacore::Int>& processorId() const {return processorId_p;};
  const casacore::ScalarColumn<casacore::Int>& stateId() const {return stateId_p;};
  const casacore::ScalarColumn<casacore::Int>& phaseId() const {return phaseId_p;};
  const casacore::ScalarColumn<casacore::Int>& pulsarBin() const {return pulsarBin_p;};
  const casacore::ScalarColumn<casacore::Int>& pulsarGateId() const {return pulsarGateId_p;};
  const casacore::ScalarColumn<casacore::Int>& freqGrp() const {return freqGrp_p;};
  const casacore::ScalarColumn<casacore::String>& freqGrpName() const {return freqGrpName_p;};
  const casacore::ScalarColumn<casacore::String>& fieldName() const {return fieldName_p;};
  const casacore::ScalarColumn<casacore::String>& fieldCode() const {return fieldCode_p;};
  const casacore::ScalarColumn<casacore::String>& sourceName() const {return sourceName_p;};
  const casacore::ScalarColumn<casacore::String>& sourceCode() const {return sourceCode_p;};
  const casacore::ScalarColumn<casacore::Int>& calGrp() const {return calGrp_p;};
  const casacore::ArrayColumn<casacore::Complex>& gain() const {return gain_p;};
  const casacore::ArrayColumn<casacore::Int>& refAnt() const {return refAnt_p;};
  const casacore::ArrayColumn<casacore::Int>& refFeed() const {return refFeed_p;};
  const casacore::ArrayColumn<casacore::Int>& refReceptor() const {return refReceptor_p;};
  const casacore::ArrayColumn<casacore::Double>& refFreq() const {return refFreq_p;};
  const casacore::ROArrayMeasColumn<casacore::MFrequency>& refFreqMeas() const
    {return refFreqMeas_p;};
  const casacore::ScalarColumn<casacore::Int>& measFreqRef() const {return measFreqRef_p;};
  const casacore::ArrayColumn<casacore::Double>& refDir() const {return refDir_p;};
  const casacore::ROArrayMeasColumn<casacore::MDirection>& refDirMeas() const
    {return refDirMeas_p;};
  const casacore::ScalarColumn<casacore::Int>& measDirRef() const {return measDirRef_p;};
  const casacore::ScalarColumn<casacore::Int>& calDescId() const {return calDescId_p;};
  const casacore::ScalarColumn<casacore::Int>& calHistoryId() const {return calHistoryId_p;};

 protected:
  // Prohibit public use of the null constructor, which
  // does not produce a usable object.
  ROCalMainColumns() {};

  // Return a CalTable as a casacore::Table reference. Utilizes friendship
  // relationship with class CalTable.
  const casacore::Table& asTable(const CalTable& calTable) 
    {return calTable.calMainAsTable();}

  // Attach a table column accessor
  void attach (const CalTable& calTable, casacore::TableColumn& tabCol, 
	       MSCalEnums::colDef colEnum, const casacore::Bool& optional = false);
  void attach (const CalTable& calTable, 
	       casacore::ROArrayMeasColumn<casacore::MEpoch>& tabCol,
	       MSCalEnums::colDef colEnum, const casacore::Bool& optional = false);
  void attach (const CalTable& calTable, 
	       casacore::ROArrayMeasColumn<casacore::MFrequency>& tabCol,
	       MSCalEnums::colDef colEnum, const casacore::Bool& optional = false);
  void attach (const CalTable& calTable, 
	       casacore::ROArrayMeasColumn<casacore::MDirection>& tabCol,
	       MSCalEnums::colDef colEnum, const casacore::Bool& optional = false);
  void attach (const CalTable& calTable, casacore::ROScalarMeasColumn<casacore::MEpoch>& tabCol,
	       MSCalEnums::colDef colEnum, const casacore::Bool& optional = false);
  void attach (const CalTable& calTable, casacore::ROScalarQuantColumn<casacore::Double>& tabCol,
	       MSCalEnums::colDef colEnum, const casacore::Bool& optional = false);

 private:
  // Prohibit copy constructor and assignment operator 
  ROCalMainColumns (const ROCalMainColumns&);
  ROCalMainColumns& operator= (const ROCalMainColumns&);

  // Private column accessors
  casacore::ScalarColumn<casacore::Double> time_p;
  casacore::ROScalarMeasColumn<casacore::MEpoch> timeMeas_p;
  casacore::ScalarColumn<casacore::Double> timeEP_p;
  casacore::ROScalarQuantColumn<casacore::Double> timeEPQuant_p;
  casacore::ScalarColumn<casacore::Double> interval_p;
  casacore::ROScalarQuantColumn<casacore::Double> intervalQuant_p;
  casacore::ScalarColumn<casacore::Int> antenna1_p;
  casacore::ScalarColumn<casacore::Int> feed1_p;
  casacore::ScalarColumn<casacore::Int> fieldId_p;
  casacore::ScalarColumn<casacore::Int> arrayId_p;
  casacore::ScalarColumn<casacore::Int> obsId_p;
  casacore::ScalarColumn<casacore::Int> scanNo_p;
  casacore::ScalarColumn<casacore::Int> processorId_p;
  casacore::ScalarColumn<casacore::Int> stateId_p;
  casacore::ScalarColumn<casacore::Int> phaseId_p;
  casacore::ScalarColumn<casacore::Int> pulsarBin_p;
  casacore::ScalarColumn<casacore::Int> pulsarGateId_p;
  casacore::ScalarColumn<casacore::Int> freqGrp_p;
  casacore::ScalarColumn<casacore::String> freqGrpName_p;
  casacore::ScalarColumn<casacore::String> fieldName_p;
  casacore::ScalarColumn<casacore::String> fieldCode_p;
  casacore::ScalarColumn<casacore::String> sourceName_p;
  casacore::ScalarColumn<casacore::String> sourceCode_p;
  casacore::ScalarColumn<casacore::Int> calGrp_p;
  casacore::ArrayColumn<casacore::Complex> gain_p;
  casacore::ArrayColumn<casacore::Int> refAnt_p;
  casacore::ArrayColumn<casacore::Int> refFeed_p;
  casacore::ArrayColumn<casacore::Int> refReceptor_p;
  casacore::ArrayColumn<casacore::Double> refFreq_p;
  casacore::ROArrayMeasColumn<casacore::MFrequency> refFreqMeas_p;
  casacore::ScalarColumn<casacore::Int> measFreqRef_p;
  casacore::ArrayColumn<casacore::Double> refDir_p;
  casacore::ROArrayMeasColumn<casacore::MDirection> refDirMeas_p;
  casacore::ScalarColumn<casacore::Int> measDirRef_p;
  casacore::ScalarColumn<casacore::Int> calDescId_p;
  casacore::ScalarColumn<casacore::Int> calHistoryId_p;
};

// <summary> 
// CalMainColumns: Read-write cal_main calibration table column access
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//   <li> <linkto class="CalTable">CalTable</linkto> module
// </prerequisite>
//
// <etymology>
// From "calibration main table" and "columns".
// </etymology>
//
// <synopsis>
// The CalMainColumns class allows read-write access to columns
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
// Encapsulate access to calibration table columns.
// </motivation>
//
// <todo asof="01/07/01">
// (i) Deal with non-standard columns.
// </todo>

class CalMainColumns 
{
 public:
  // Construct from a calibration table
  CalMainColumns (CalTable& calTable);

  // Default destructor
  virtual ~CalMainColumns() {};

  // Read-write column accessors
  casacore::ScalarColumn<casacore::Double>& time() {return time_p;};
  casacore::ScalarMeasColumn<casacore::MEpoch>& timeMeas() {return timeMeas_p;};
  casacore::ScalarColumn<casacore::Double>& timeEP() {return timeEP_p;};
  casacore::ScalarQuantColumn<casacore::Double>& timeEPQuant() {return timeEPQuant_p;};
  casacore::ScalarColumn<casacore::Double>& interval() {return interval_p;};
  casacore::ScalarQuantColumn<casacore::Double>& intervalQuant() {return intervalQuant_p;};
  casacore::ScalarColumn<casacore::Int>& antenna1() {return antenna1_p;};
  casacore::ScalarColumn<casacore::Int>& feed1() {return feed1_p;};
  casacore::ScalarColumn<casacore::Int>& fieldId() {return fieldId_p;};
  casacore::ScalarColumn<casacore::Int>& arrayId() {return arrayId_p;};
  casacore::ScalarColumn<casacore::Int>& obsId() {return obsId_p;};
  casacore::ScalarColumn<casacore::Int>& scanNo() {return scanNo_p;};
  casacore::ScalarColumn<casacore::Int>& processorId() {return processorId_p;};
  casacore::ScalarColumn<casacore::Int>& stateId() {return stateId_p;};
  casacore::ScalarColumn<casacore::Int>& phaseId() {return phaseId_p;};
  casacore::ScalarColumn<casacore::Int>& pulsarBin() {return pulsarBin_p;};
  casacore::ScalarColumn<casacore::Int>& pulsarGateId() {return pulsarGateId_p;};
  casacore::ScalarColumn<casacore::Int>& freqGrp() {return freqGrp_p;};
  casacore::ScalarColumn<casacore::String>& freqGrpName() {return freqGrpName_p;};
  casacore::ScalarColumn<casacore::String>& fieldName() {return fieldName_p;};
  casacore::ScalarColumn<casacore::String>& fieldCode() {return fieldCode_p;};
  casacore::ScalarColumn<casacore::String>& sourceName() {return sourceName_p;};
  casacore::ScalarColumn<casacore::String>& sourceCode() {return sourceCode_p;};
  casacore::ScalarColumn<casacore::Int>& calGrp() {return calGrp_p;};
  casacore::ArrayColumn<casacore::Complex>& gain() {return gain_p;};
  casacore::ArrayColumn<casacore::Int>& refAnt() {return refAnt_p;};
  casacore::ArrayColumn<casacore::Int>& refFeed() {return refFeed_p;};
  casacore::ArrayColumn<casacore::Int>& refReceptor() {return refReceptor_p;};
  casacore::ArrayColumn<casacore::Double>& refFreq() {return refFreq_p;};
  casacore::ArrayMeasColumn<casacore::MFrequency>& refFreqMeas() {return refFreqMeas_p;};
  casacore::ScalarColumn<casacore::Int>& measFreqRef() {return measFreqRef_p;};
  casacore::ArrayColumn<casacore::Double>& refDir() {return refDir_p;};
  casacore::ArrayMeasColumn<casacore::MDirection>& refDirMeas() {return refDirMeas_p;};
  casacore::ScalarColumn<casacore::Int>& measDirRef() {return measDirRef_p;};
  casacore::ScalarColumn<casacore::Int>& calDescId() {return calDescId_p;};
  casacore::ScalarColumn<casacore::Int>& calHistoryId() {return calHistoryId_p;}; 
  
 protected:
  // Prohibit public use of the null constructor, which
  // does not produce a usable object.
  CalMainColumns() {};

  // Return a CalTable as a casacore::Table reference. Utilizes friendship
  // relationship with class CalTable.
  casacore::Table& asTable(CalTable& calTable) {return calTable.calMainAsTable();}

  // Attach a table column accessor
  void attach (CalTable& calTable, casacore::TableColumn& tabCol, 
	       MSCalEnums::colDef colEnum, const casacore::Bool& optional = false);
  void attach (CalTable& calTable, casacore::ArrayMeasColumn<casacore::MEpoch>& tabCol, 
	       MSCalEnums::colDef colEnum, const casacore::Bool& optional = false);
  void attach (CalTable& calTable, casacore::ArrayMeasColumn<casacore::MFrequency>& tabCol, 
	       MSCalEnums::colDef colEnum, const casacore::Bool& optional = false);
  void attach (CalTable& calTable, casacore::ArrayMeasColumn<casacore::MDirection>& tabCol, 
	       MSCalEnums::colDef colEnum, const casacore::Bool& optional = false);
  void attach (CalTable& calTable, casacore::ScalarMeasColumn<casacore::MEpoch>& tabCol, 
	       MSCalEnums::colDef colEnum, const casacore::Bool& optional = false);
  void attach (CalTable& calTable, casacore::ScalarQuantColumn<casacore::Double>& tabCol, 
	       MSCalEnums::colDef colEnum, const casacore::Bool& optional = false);

 private:
  // Prohibit copy constructor and assignment operator 
  CalMainColumns (const CalMainColumns&);
  CalMainColumns& operator= (const CalMainColumns&);

  // Private column accessors
  casacore::ScalarColumn<casacore::Double> time_p;
  casacore::ScalarMeasColumn<casacore::MEpoch> timeMeas_p;
  casacore::ScalarColumn<casacore::Double> timeEP_p;
  casacore::ScalarQuantColumn<casacore::Double> timeEPQuant_p;
  casacore::ScalarColumn<casacore::Double> interval_p;
  casacore::ScalarQuantColumn<casacore::Double> intervalQuant_p;
  casacore::ScalarColumn<casacore::Int> antenna1_p;
  casacore::ScalarColumn<casacore::Int> feed1_p;
  casacore::ScalarColumn<casacore::Int> fieldId_p;
  casacore::ScalarColumn<casacore::Int> arrayId_p;
  casacore::ScalarColumn<casacore::Int> obsId_p;
  casacore::ScalarColumn<casacore::Int> scanNo_p;
  casacore::ScalarColumn<casacore::Int> processorId_p;
  casacore::ScalarColumn<casacore::Int> stateId_p;
  casacore::ScalarColumn<casacore::Int> phaseId_p;
  casacore::ScalarColumn<casacore::Int> pulsarBin_p;
  casacore::ScalarColumn<casacore::Int> pulsarGateId_p;
  casacore::ScalarColumn<casacore::Int> freqGrp_p;
  casacore::ScalarColumn<casacore::String> freqGrpName_p;
  casacore::ScalarColumn<casacore::String> fieldName_p;
  casacore::ScalarColumn<casacore::String> fieldCode_p;
  casacore::ScalarColumn<casacore::String> sourceName_p;
  casacore::ScalarColumn<casacore::String> sourceCode_p;
  casacore::ScalarColumn<casacore::Int> calGrp_p;
  casacore::ArrayColumn<casacore::Complex> gain_p;
  casacore::ArrayColumn<casacore::Int> refAnt_p;
  casacore::ArrayColumn<casacore::Int> refFeed_p;
  casacore::ArrayColumn<casacore::Int> refReceptor_p;
  casacore::ArrayColumn<casacore::Double> refFreq_p;
  casacore::ArrayMeasColumn<casacore::MFrequency> refFreqMeas_p;
  casacore::ScalarColumn<casacore::Int> measFreqRef_p;
  casacore::ArrayColumn<casacore::Double> refDir_p;
  casacore::ArrayMeasColumn<casacore::MDirection> refDirMeas_p;
  casacore::ScalarColumn<casacore::Int> measDirRef_p;
  casacore::ScalarColumn<casacore::Int> calDescId_p;
  casacore::ScalarColumn<casacore::Int> calHistoryId_p;
};


} //# NAMESPACE CASA - END

#endif
   
  



