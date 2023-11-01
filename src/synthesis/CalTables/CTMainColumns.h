//# CTMainColumns.h: Calibration table cal_main column access
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
//# Correspondence concerning AIPS++ should be adressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//#
//# $Id$

#ifndef CALIBRATION_NEWCALMAINCOLUMNS_H
#define CALIBRATION_NEWCALMAINCOLUMNS_H

#include <casacore/casa/aips.h>
#include <casacore/measures/Measures/MEpoch.h>
#include <casacore/measures/Measures/MFrequency.h>
#include <casacore/measures/Measures/MDirection.h>
#include <casacore/tables/Tables/TableColumn.h>
#include <casacore/tables/Tables/ScalarColumn.h>
#include <casacore/tables/Tables/ArrayColumn.h>
#include <casacore/measures/TableMeasures/TableMeasColumn.h>
#include <casacore/measures/TableMeasures/ScalarMeasColumn.h>
#include <casacore/measures/TableMeasures/ArrayMeasColumn.h>
#include <casacore/measures/TableMeasures/ScalarQuantColumn.h>
#include <synthesis/CalTables/NewCalTable.h>
#include <synthesis/CalTables/CTEnums.h>

namespace casa { //# NAMESPACE CASA - BEGIN

class NewCalTable;

class ROCTMainColumns
{
 public:
  // Construct from a calibration table
  ROCTMainColumns (const NewCalTable& calTable);
  
  // Default destructor
  virtual ~ROCTMainColumns() {};
  
  // Read-only column accessors
  const casacore::ScalarColumn<casacore::Double>& time() const {return time_p;};
  const casacore::ROScalarQuantColumn<casacore::Double>& timeQuant() const {return timeQuant_p;};
  const casacore::ROScalarMeasColumn<casacore::MEpoch>& timeMeas() const {return timeMeas_p;};
  const casacore::ScalarColumn<casacore::Double>& timeEP() const {return timeEP_p;};
  const casacore::ROScalarQuantColumn<casacore::Double>& timeEPQuant() const
  {return timeEPQuant_p;};
  const casacore::ScalarColumn<casacore::Double>& interval() const {return interval_p;};
  const casacore::ROScalarQuantColumn<casacore::Double>& intervalQuant() const
  {return intervalQuant_p;};
  const casacore::ScalarColumn<casacore::Int>& antenna1() const {return antenna1_p;};
  const casacore::ScalarColumn<casacore::Int>& antenna2() const {return antenna2_p;};
  const casacore::ScalarColumn<casacore::Int>& fieldId() const {return fieldId_p;};
  const casacore::ScalarColumn<casacore::Int>& spwId() const {return spwId_p;};
  const casacore::ScalarColumn<casacore::Int>& scanNo() const {return scanNo_p;};
  const casacore::ScalarColumn<casacore::Int>& obsId() const {return obsId_p;};
  const casacore::ArrayColumn<casacore::Complex>& cparam() const {return cparam_p;};
  const casacore::ArrayColumn<casacore::Float>& fparam() const {return fparam_p;};
  const casacore::ArrayColumn<casacore::Float>& paramerr() const {return paramerr_p;};
  const casacore::ArrayColumn<casacore::Bool>& flag() const {return flag_p;};
  const casacore::ArrayColumn<casacore::Float>& snr() const {return snr_p;};
  const casacore::ArrayColumn<casacore::Float>& weight() const {return weight_p;};

  // Some additional methods to extract cparam into casacore::Array<casacore::Float>
  //   what can be: "","AP"
  casacore::Array<casacore::Float> fparamArray(casacore::String what="",const casacore::Vector<casacore::uInt>& rows=casacore::Vector<casacore::uInt>());
  void fparamArray(casacore::Array<casacore::Float>& arr,casacore::String what="",const casacore::Vector<casacore::uInt>& rows=casacore::Vector<casacore::uInt>());

  // Some additional methods to extract phase-like info
  casacore::Array<casacore::Float> phase();
  void phase(casacore::Array<casacore::Float>& ph);
  casacore::Array<casacore::Float> cycles();
  void cycles(casacore::Array<casacore::Float>& cy);
  
 protected:
  // Prohibit public use of the null constructor, which
  // does not produce a usable object.
  ROCTMainColumns() {};
  
  // Return a CalTable as a casacore::Table reference. Utilizes friendship
  // relationship with class CalTable.
  //const casacore::Table& asTable(const CalTable2& calTable) 
  //{return calTable.calMainAsTable();}
  
  // Attach a table column accessor
  void attach (const NewCalTable& calTable, casacore::TableColumn& tabCol, 
	       CTEnums::colDef colEnum, const casacore::Bool& optional = false);
  void attach (const NewCalTable& calTable, 
	       casacore::ROArrayMeasColumn<casacore::MEpoch>& tabCol,
	       CTEnums::colDef colEnum, const casacore::Bool& optional = false);
  void attach (const NewCalTable& calTable, 
	       casacore::ROArrayMeasColumn<casacore::MFrequency>& tabCol,
	       CTEnums::colDef colEnum, const casacore::Bool& optional = false);
  void attach (const NewCalTable& calTable, 
	       casacore::ROArrayMeasColumn<casacore::MDirection>& tabCol,
	       CTEnums::colDef colEnum, const casacore::Bool& optional = false);
  void attach (const NewCalTable& calTable, casacore::ROScalarMeasColumn<casacore::MEpoch>& tabCol,
	       CTEnums::colDef colEnum, const casacore::Bool& optional = false);
  void attach (const NewCalTable& calTable, casacore::ROScalarQuantColumn<casacore::Double>& tabCol,
	       CTEnums::colDef colEnum, const casacore::Bool& optional = false);
  
 private:
  // Prohibit copy constructor and assignment operator 
  ROCTMainColumns (const ROCTMainColumns&);
  ROCTMainColumns& operator= (const ROCTMainColumns&);
  
  // Private column accessors
  casacore::ScalarColumn<casacore::Double> time_p;
  casacore::ROScalarQuantColumn<casacore::Double> timeQuant_p;
  casacore::ROScalarMeasColumn<casacore::MEpoch> timeMeas_p;
  casacore::ScalarColumn<casacore::Double> timeEP_p;
  casacore::ROScalarQuantColumn<casacore::Double> timeEPQuant_p;
  casacore::ScalarColumn<casacore::Double> interval_p;
  casacore::ROScalarQuantColumn<casacore::Double> intervalQuant_p;
  casacore::ScalarColumn<casacore::Int> antenna1_p;
  casacore::ScalarColumn<casacore::Int> antenna2_p;
  casacore::ScalarColumn<casacore::Int> fieldId_p;
  casacore::ScalarColumn<casacore::Int> spwId_p;
  casacore::ScalarColumn<casacore::Int> scanNo_p;
  casacore::ScalarColumn<casacore::Int> obsId_p;
  casacore::ArrayColumn<casacore::Complex> cparam_p;
  casacore::ArrayColumn<casacore::Float> fparam_p;
  casacore::ArrayColumn<casacore::Float> paramerr_p;
  casacore::ArrayColumn<casacore::Bool> flag_p;
  casacore::ArrayColumn<casacore::Float> snr_p;
  casacore::ArrayColumn<casacore::Float> weight_p;
};

class CTMainColumns
{
  public:
    // Construct from a calibration table
    CTMainColumns (NewCalTable& calTable);
    
    // Default destructor
    virtual ~CTMainColumns() {};
    
    // Read-write column accessors
    casacore::ScalarColumn<casacore::Double>& time() {return time_p;};
    casacore::ScalarQuantColumn<casacore::Double>& timeQuant() {return timeQuant_p;};
    casacore::ScalarMeasColumn<casacore::MEpoch>& timeMeas() {return timeMeas_p;};
    casacore::ScalarColumn<casacore::Double>& timeEP() {return timeEP_p;};
    casacore::ScalarQuantColumn<casacore::Double>& timeEPQuant() {return timeEPQuant_p;};
    casacore::ScalarColumn<casacore::Double>& interval() {return interval_p;};
    casacore::ScalarQuantColumn<casacore::Double>& intervalQuant() {return intervalQuant_p;};
    casacore::ScalarColumn<casacore::Int>& fieldId() {return fieldId_p;};
    casacore::ScalarColumn<casacore::Int>& spwId() {return spwId_p;};
    casacore::ScalarColumn<casacore::Int>& antenna1() {return antenna1_p;};
    casacore::ScalarColumn<casacore::Int>& antenna2() {return antenna2_p;};
    casacore::ScalarColumn<casacore::Int>& scanNo() {return scanNo_p;};
    casacore::ScalarColumn<casacore::Int>& obsId() {return obsId_p;};
    casacore::ArrayColumn<casacore::Complex>& cparam() {return cparam_p;};
    casacore::ArrayColumn<casacore::Float>& fparam() {return fparam_p;};
    casacore::ArrayColumn<casacore::Float>& paramerr() {return paramerr_p;};
    casacore::ArrayColumn<casacore::Bool>& flag() {return flag_p;};
    casacore::ArrayColumn<casacore::Float>& snr() {return snr_p;};
    casacore::ArrayColumn<casacore::Float>& weight() {return weight_p;};
    
  protected:
    // Prohibit public use of the null constructor, which
    // does not produce a usable object.
    CTMainColumns() {};
    
    // Return a CalTable as a casacore::Table reference. Utilizes friendship
    // relationship with class CalTable.
    //casacore::Table& asTable(CalTable2& calTable) {return calTable.calMainAsTable();}
    
    // Attach a table column accessor
    void attach (NewCalTable& calTable, casacore::TableColumn& tabCol, 
		 CTEnums::colDef colEnum, const casacore::Bool& optional = false);
    void attach (NewCalTable& calTable, casacore::ArrayMeasColumn<casacore::MEpoch>& tabCol, 
		 CTEnums::colDef colEnum, const casacore::Bool& optional = false);
    void attach (NewCalTable& calTable, casacore::ArrayMeasColumn<casacore::MFrequency>& tabCol, 
		 CTEnums::colDef colEnum, const casacore::Bool& optional = false);
    void attach (NewCalTable& calTable, casacore::ArrayMeasColumn<casacore::MDirection>& tabCol, 
		 CTEnums::colDef colEnum, const casacore::Bool& optional = false);
    void attach (NewCalTable& calTable, casacore::ScalarMeasColumn<casacore::MEpoch>& tabCol, 
		 CTEnums::colDef colEnum, const casacore::Bool& optional = false);
    void attach (NewCalTable& calTable, casacore::ScalarQuantColumn<casacore::Double>& tabCol, 
		 CTEnums::colDef colEnum, const casacore::Bool& optional = false);
    
  private:
    // Prohibit copy constructor and assignment operator 
    CTMainColumns (const CTMainColumns&);
    CTMainColumns& operator= (const CTMainColumns&);
    
    // Private column accessors
    casacore::ScalarColumn<casacore::Double> time_p;
    casacore::ScalarQuantColumn<casacore::Double> timeQuant_p;
    casacore::ScalarMeasColumn<casacore::MEpoch> timeMeas_p;
    casacore::ScalarColumn<casacore::Double> timeEP_p;
    casacore::ScalarQuantColumn<casacore::Double> timeEPQuant_p;
    casacore::ScalarColumn<casacore::Double> interval_p;
    casacore::ScalarQuantColumn<casacore::Double> intervalQuant_p;
    casacore::ScalarColumn<casacore::Int> fieldId_p;
    casacore::ScalarColumn<casacore::Int> spwId_p;
    casacore::ScalarColumn<casacore::Int> antenna1_p;
    casacore::ScalarColumn<casacore::Int> antenna2_p;
    casacore::ScalarColumn<casacore::Int> scanNo_p;
    casacore::ScalarColumn<casacore::Int> obsId_p;
    casacore::ArrayColumn<casacore::Complex> cparam_p;
    casacore::ArrayColumn<casacore::Float> fparam_p;
    casacore::ArrayColumn<casacore::Float> paramerr_p;
    casacore::ArrayColumn<casacore::Bool> flag_p;
    casacore::ArrayColumn<casacore::Float> snr_p;
    casacore::ArrayColumn<casacore::Float> weight_p;
  };
} //# NAMESPACE CASA - END
#endif
