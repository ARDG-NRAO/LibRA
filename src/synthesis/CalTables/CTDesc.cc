//# CTDesc.cc: Implementation of CTDesc.h
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

#include <synthesis/CalTables/CTDesc.h>
#include <synthesis/CalTables/CTEnums.h>
#include <casacore/tables/Tables/ScaColDesc.h>
#include <casacore/tables/Tables/ArrColDesc.h>
#include <casacore/tables/Tables/SetupNewTab.h>
#include <casacore/tables/Tables/TableRecord.h>
#include <casacore/measures/TableMeasures/TableMeasRefDesc.h>
#include <casacore/measures/TableMeasures/TableMeasValueDesc.h>
#include <casacore/measures/TableMeasures/TableMeasDesc.h>
#include <casacore/measures/TableMeasures/TableQuantumDesc.h>
#include <casacore/measures/Measures/MEpoch.h>
#include <casacore/measures/Measures/MFrequency.h>
#include <casacore/measures/Measures/MDirection.h>
#include <casacore/casa/Containers/Record.h>
#include <casacore/casa/Arrays/IPosition.h>
#include <stdcasa/version.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

//----------------------------------------------------------------------------

CTDesc::CTDesc() : 
  addObsId_(true),
  itsCalMainDesc(defaultCalMain())
{
// Default null constructor for new calibration table description (v1.0)
// Output to private data:
//    itsCalMainDesc      TableDesc        Table descriptor (cal_main)
//    itsCalHistoryDesc   TableDesc        Table descriptor (cal_history)
//
};

//----------------------------------------------------------------------------

CTDesc::CTDesc(Bool addObsId) : 
  addObsId_(addObsId),
  itsCalMainDesc(defaultCalMain())
{
// Alternative ctor to support OBS_ID opt-out
// Output to private data:
//    itsCalMainDesc      TableDesc        Table descriptor (cal_main)
//    itsCalHistoryDesc   TableDesc        Table descriptor (cal_history)
//
};

//----------------------------------------------------------------------------

CTDesc::CTDesc (const String& partype,
		const String& msname,
		const String& viscal,
		const String& polbasis) :
  addObsId_(true),
  itsCalMainDesc(defaultCalMain(partype,msname,viscal,polbasis))
{
// Constructor for new calibration table description (v1.0)
// Inputs:
//    type                const String&    Cal table type (eg. "P Jones")
// Output to private data:
//    itsCalMainDesc      TableDesc        Table descriptor (cal_main)
//    itsCalHistoryDesc   TableDesc        Table descriptor (cal_history)
//
};

//----------------------------------------------------------------------------
/***
TableDesc CalTableDesc::defaultCalHistory()
{
// Generate the default table descriptor for the Cal History sub-table
// Output:
//    defaultCalHistory     TableDesc     Default Cal History descriptor
//
  TableDesc td ("Cal History", "2.0", TableDesc::Scratch);
  td.comment() = "Calibration history sub-table";
  td.addColumn (ScalarColumnDesc <String> (NCT::fieldName (NCT::CAL_PARMS),
					   ColumnDesc::Direct));
  td.addColumn (ScalarColumnDesc <String> (NCT::fieldName (NCT::CAL_TABLES),
					   ColumnDesc::Direct));
  td.addColumn (ScalarColumnDesc <String> (NCT::fieldName (NCT::CAL_SELECT),
					   ColumnDesc::Direct));
  td.addColumn (ScalarColumnDesc <String> (NCT::fieldName (NCT::CAL_NOTES),
					   ColumnDesc::Direct));
  return td;
};
***/
//----------------------------------------------------------------------------

TableDesc CTDesc::defaultCalMain (const String& partype,
				  const String& msname,
				  const String& viscal,
				  const String& polbasis)
{
// Generate the default table descriptor for the Cal Main sub-table
// Input:
//    type             const String&      Cal table type (eg. "P Jones")
// Output:
//    defaultCalMain   TableDesc          Default Cal Main descriptor
//
  // Set up table descriptor and add comment field
  TableDesc td (viscal, "1.0", TableDesc::Scratch);
  td.comment() = "New calibration table";

  // Define keywords
  Record keyWordRec;
  // Cal_desc and cal_history indices
  // calibration type
  keyWordRec.define ("ParType", partype);
  keyWordRec.define ("MSName",msname);
  keyWordRec.define ("VisCal",viscal);
  keyWordRec.define ("PolBasis",polbasis);
  keyWordRec.define ("CASA_Version",VersionInfo::str());  // Basic CASA version string
  // Add to table descriptor
  td.rwKeywordSet().assign (keyWordRec);

  // Cal Main columns (MS Main indices)
  // keys
  td.addColumn (ScalarColumnDesc <Double> (NCT::fieldName (NCT::TIME),
					   ColumnDesc::Direct));
  td.addColumn (ScalarColumnDesc <Int> (NCT::fieldName (NCT::FIELD_ID),
					ColumnDesc::Direct));
  td.addColumn (ScalarColumnDesc <Int> (NCT::fieldName (NCT::SPECTRAL_WINDOW_ID),
					ColumnDesc::Direct));
  td.addColumn (ScalarColumnDesc <Int> (NCT::fieldName (NCT::ANTENNA1),
					ColumnDesc::Direct));
  // ANTENNA2 is refant (when relevant), or 2nd antenna for baseline-based
  td.addColumn (ScalarColumnDesc <Int> (NCT::fieldName (NCT::ANTENNA2),
					ColumnDesc::Direct));
  //non keys
  td.addColumn (ScalarColumnDesc <Double> (NCT::fieldName (NCT::INTERVAL),
					   ColumnDesc::Direct));
  td.addColumn (ScalarColumnDesc <Int> (NCT::fieldName (NCT::SCAN_NUMBER),
					ColumnDesc::Direct));

  // We might be opting out of OBS_ID... (usually not)
  //   (usually not; this is to maintain support for older existing
  //    cal tables that don't have this column)
  if (addObsId_) {
    td.addColumn (ScalarColumnDesc <Int> (NCT::fieldName (NCT::OBSERVATION_ID),
					  ColumnDesc::Direct));
  }
  //Parameter columns
  if (partype=="Complex")
    td.addColumn (ArrayColumnDesc <Complex>  (NCT::fieldName (NCT::CPARAM)));
  else if (partype=="Float")
    td.addColumn (ArrayColumnDesc <Float>  (NCT::fieldName (NCT::FPARAM)));

  // TBD: we should handle improper partype better, but conformant() 
  //  currently requires neither PARAM column in case of default defaultCalMain...
  //  else
  //    throw(AipsError("CTDesc(): Unknown ParType requested."));

  td.addColumn (ArrayColumnDesc <Float>  (NCT::fieldName (NCT::PARAMERR)));
  td.addColumn (ArrayColumnDesc <Bool>  (NCT::fieldName (NCT::FLAG)));
  td.addColumn (ArrayColumnDesc <Float>  (NCT::fieldName (NCT::SNR)));
  td.addColumn (ArrayColumnDesc <Float>  (NCT::fieldName (NCT::WEIGHT)));

  // Add TableMeasures information for designated Measures/Quanta columns
  // 
  // TIME
  TableMeasValueDesc timeMeasVal (td, NCT::fieldName (NCT::TIME));
  TableMeasRefDesc timeMeasRef (MEpoch::DEFAULT);
  TableMeasDesc<MEpoch> timeMeasCol (timeMeasVal, timeMeasRef);
  timeMeasCol.write (td);

  // TIME fix Unit
  TableQuantumDesc timeQuantDesc (td, NCT::fieldName (NCT::TIME),
                                  Unit ("s"));
  timeQuantDesc.write (td);
  // INTERVAL
  TableQuantumDesc intervalQuantDesc (td, NCT::fieldName(NCT::INTERVAL),
				      Unit ("s"));
  intervalQuantDesc.write (td);

  return td;
};

//----------------------------------------------------------------------------

TableDesc CTDesc::calMainDesc()
{
// Return the main calibration table descriptor
// Output:
//    calMainDesc     TableDesc        Table descriptor (cal_main)
//
  return itsCalMainDesc;
};

//----------------------------------------------------------------------------
/***
TableDesc CTDesc::calHistoryDesc()
{
// Return the calibration history sub-table descriptor
// Output:
//    calHistoryDesc  TableDesc        Table descriptor (cal_history)
//
  return itsCalHistoryDesc;
};
***/
//----------------------------------------------------------------------------

TableDesc CTDesc::insertDesc (const TableDesc& tableDesc, 
			      const TableDesc& insert, 
			      const String& insertAfter)
{
// Insert one table descriptor into another after a specified
// column name.
// Input:
//    tableDesc       const TableDesc&      Input table descriptor
//    insert          const TableDesc&      Table descriptor to be inserted
//    insertAfter     const String&         Column name to insert after
// Output:
//    insertDesc      TableDesc             Output table descriptor
//
  TableDesc tdout;
  ColumnDesc* colDesc;
  Int ncol = tableDesc.ncolumn();
  Int jcol;

  // Loop over the number of columns in the input table descriptor
  for (jcol = 0; jcol < ncol; jcol++) {
    colDesc = new ColumnDesc (tableDesc.columnDesc (jcol));

    // Add column to output descriptor
    tdout.addColumn (*colDesc);

    // Insert second descriptor if appropriate
    if (colDesc->name() == insertAfter) {
      tdout.add (insert);
    };
    delete colDesc;
  };

  return tdout;
};

//----------------------------------------------------------------------------


} //# NAMESPACE CASA - END

