//# CTInterface.cc: Implementation of CTInterface class
//# Copyright (C) 1996,1997,1998,1999,2000,2001,2003
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

#include <synthesis/CalTables/CTInterface.h>
#include <casacore/ms/MeasurementSets/MSColumns.h>

#include <unordered_set>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

  //
  //----------------------------------------------------------------------------
  //
  CTInterface::CTInterface(const Table& table)
    :MSSelectableTable(table), fakeDDSubTable(),  ctMainCols_p(NULL) 
  {makeDDSubTable();};
  //
  //----------------------------------------------------------------------------
  //
  CTInterface::~CTInterface() 
  {if (ctMainCols_p) delete ctMainCols_p;};
  //
  //----------------------------------------------------------------------------
  //
  const NewCalTable* CTInterface::asCT()
  {
    return static_cast<const NewCalTable *>(table());
  };
  //
  //----------------------------------------------------------------------------
  //
  void CTInterface::makeDDSubTable()
  {
    //
    // Make an in-memory DataDescription table.
    //
    SetupNewTable setup(String(""), MSDataDescription::requiredTableDesc(), 
      Table::Scratch);
    const Table tmpTab(setup, Table::Memory, spectralWindow().nrow());
    fakeDDSubTable = MSDataDescription(tmpTab);
    //
    // Call the in-memory table what it is.
    //
    TableInfo& tabInfo=fakeDDSubTable.tableInfo();
    tabInfo.setType("FAKE_DATA_DESCRIPTION_ID");
    //
    // Fill the required columns (flagRow and spectralWindowId).
    // These should ideally be just referenced, but I (SB) could not
    // figure out how to do it.
    //
    Vector<Bool> spwFlagRow=MSSpWindowColumns(spectralWindow()).flagRow().getColumn();
    MSDataDescColumns(fakeDDSubTable).flagRow().putColumn(spwFlagRow);

    Vector<Int> spwId(fakeDDSubTable.nrow()); indgen(spwId);
    MSDataDescColumns(fakeDDSubTable).spectralWindowId().putColumn(spwId);
  }
  //
  //----------------------------------------------------------------------------
  //
  const MSDataDescription& CTInterface::dataDescription() 
  {
    return fakeDDSubTable;
  }
  //
  //----------------------------------------------------------------------------
  // For CalTables, MS::DATA_DESC_ID maps to
  // CTEnums::SPECTRAL_WINDOW_ID.  So do only that translation.
  //
  String CTInterface::columnName(MSMainEnums::PredefinedColumns nameEnum) 
  {
    if (nameEnum == MS::DATA_DESC_ID)
      return CTEnums::fieldName(CTEnums::SPECTRAL_WINDOW_ID);
    else
      return MS::columnName(nameEnum);
  }
  //
  //----------------------------------------------------------------------------
  // Return the type of the CalTable.  Possibilities are defined in
  // the MSSelectableTable::MSSDataType enum.
  //  o For MSSelectableTable::MSSDataType::PURE_ANTENNA_BASED, only 
  //    ANTENN1 is matched and the "&" operators in selection expressions
  //    are ignored.
  //  o For MSSelectableTable::MSSDataType::REF_ANTENNA_BASED, ANTENN2
  //    is interpreted as a reference antenna and matched against the
  //    the ANT2 in a ANT2&ANT2 type expressions.
  //
  // This is the method called in the MSSelection module to determine
  // the appropriate rule-resolution.  Any CalTable type based
  // determination of the MSDataType should be done here.

  MSSelectableTable::MSSDataType CTInterface::dataType()
  {
    const NewCalTable* nct = asCT();
    casacore::ScalarColumn<casacore::Int> ant1col(*nct, "ANTENNA1");
    casacore::Vector<casacore::Int> ant1 = ant1col.getColumn();
    casacore::ScalarColumn<casacore::Int> ant2col(*nct, "ANTENNA2");
    casacore::Vector<casacore::Int> ant2 = ant2col.getColumn();

    // Check if ant2 is -1, all antennas, or subset of antennas
    // For SD caltables where ANT1=ANT2 in all rows, treat as pure
    if ((ant2(0) == -1) || allEQ(ant1, ant2)) {
      return MSSelectableTable::PURE_ANTENNA_BASED;
    } else {
      std::unordered_set<casacore::Int> ant2set(ant2.begin(), ant2.end());
      if (ant2set.size() == nct->antenna().nrow()) {
        return MSSelectableTable::BASELINE_BASED;
      } else {
        return MSSelectableTable::REF_ANTENNA_BASED;
      }
    }
  }

  //
  //----------------------------------------------------------------------------
  // CalTables have no OBSERVATION sub-table.  So throw a tantrum if
  // it is asked for.
  //
  //  const MSObservation& CTInterface::observation()
  //  {
  //    throw(AipsError("Internal error: OBSERVATION sub-table for CalTables requested"));
  //  };
  //----------------------------------------------------------------------------
} //# NAMESPACE CASA - END

