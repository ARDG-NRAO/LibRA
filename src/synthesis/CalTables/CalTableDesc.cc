//# CalTableDesc.cc: Implementation of CalTableDesc.h
//# Copyright (C) 1996,1997,1998,1999,2000,2001,2002,2003
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

#include <synthesis/CalTables/CalTableDesc.h>
#include <msvis/MSVis/MSCalEnums.h>
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

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

//----------------------------------------------------------------------------

CalTableDesc::CalTableDesc() : itsCalMainDesc(defaultCalMain("")), 
  itsCalHistoryDesc(defaultCalHistory()),
  itsCalDescDesc(defaultCalDesc())
{
// Default null constructor for calibration table description (v2.0)
// Output to private data:
//    itsCalMainDesc      TableDesc        Table descriptor (cal_main)
//    itsCalHistoryDesc   TableDesc        Table descriptor (cal_history)
//    itsCalDescDesc      TableDesc        Table descriptor (cal_desc)
//
};

//----------------------------------------------------------------------------

CalTableDesc::CalTableDesc (const String& type) : 
  itsCalMainDesc(defaultCalMain(type)), 
  itsCalHistoryDesc(defaultCalHistory()), itsCalDescDesc(defaultCalDesc())
{
// Constructor for calibration table description (v2.0)
// Inputs:
//    type                const String&    Cal table type (eg. "P Jones")
// Output to private data:
//    itsCalMainDesc      TableDesc        Table descriptor (cal_main)
//    itsCalHistoryDesc   TableDesc        Table descriptor (cal_history)
//    itsCalDescDesc      TableDesc        Table descriptor (cal_desc)
//
};

//----------------------------------------------------------------------------

TableDesc CalTableDesc::defaultCalHistory()
{
// Generate the default table descriptor for the Cal History sub-table
// Output:
//    defaultCalHistory     TableDesc     Default Cal History descriptor
//
  TableDesc td ("Cal History", "2.0", TableDesc::Scratch);
  td.comment() = "Calibration history sub-table";
  td.addColumn (ScalarColumnDesc <String> (MSC::fieldName (MSC::CAL_PARMS),
					   ColumnDesc::Direct));
  td.addColumn (ScalarColumnDesc <String> (MSC::fieldName (MSC::CAL_TABLES),
					   ColumnDesc::Direct));
  td.addColumn (ScalarColumnDesc <String> (MSC::fieldName (MSC::CAL_SELECT),
					   ColumnDesc::Direct));
  td.addColumn (ScalarColumnDesc <String> (MSC::fieldName (MSC::CAL_NOTES),
					   ColumnDesc::Direct));
  return td;
};

//----------------------------------------------------------------------------

TableDesc CalTableDesc::defaultCalDesc()
{
// Generate the default table descriptor for the Cal Desc sub-table
// Output:
//    defaultCalDesc        TableDesc       Default Cal Desc descriptor
//
  TableDesc td ("Cal Desc", "1.0", TableDesc::Scratch);
  td.comment() = "Calibration description sub-table";
  td.addColumn (ScalarColumnDesc <Int> (MSC::fieldName (MSC::NUM_SPW),
					ColumnDesc::Direct));
  td.addColumn (ArrayColumnDesc <Int> (MSC::fieldName (MSC::NUM_CHAN)));
  td.addColumn (ScalarColumnDesc <Int> (MSC::fieldName (MSC::NUM_RECEPTORS),
					ColumnDesc::Direct));
  td.addColumn (ScalarColumnDesc <Int> (MSC::fieldName (MSC::N_JONES),
					ColumnDesc::Direct));
  td.addColumn 
    (ArrayColumnDesc <Int> (MSC::fieldName (MSC::SPECTRAL_WINDOW_ID)));
  td.addColumn (ArrayColumnDesc <Double> (MSC::fieldName (MSC::CHAN_FREQ)));
  td.addColumn (ScalarColumnDesc <Int> (MSC::fieldName (MSC::MEAS_FREQ_REF),
					ColumnDesc::Direct));
  td.addColumn (ArrayColumnDesc <Double> (MSC::fieldName (MSC::CHAN_WIDTH)));
  td.addColumn (ArrayColumnDesc <Int> (MSC::fieldName (MSC::CHAN_RANGE)));
  td.addColumn (ArrayColumnDesc <String> 
		(MSC::fieldName (MSC::POLARIZATION_TYPE)));
  td.addColumn (ScalarColumnDesc <String> (MSC::fieldName (MSC::JONES_TYPE),
					   ColumnDesc::Direct));
  td.addColumn (ScalarColumnDesc <String> (MSC::fieldName (MSC::MS_NAME),
					   ColumnDesc::Direct));

  // Add TableMeasures information for Measures/Quanta columns
  //
  // CHAN_FREQ
  TableMeasValueDesc chanFreqMeasVal (td, MSC::fieldName(MSC::CHAN_FREQ));
  TableMeasRefDesc chanFreqMeasRef (td, MSC::fieldName (MSC::MEAS_FREQ_REF));
  TableMeasDesc<MFrequency> chanFreqMeasCol (chanFreqMeasVal, chanFreqMeasRef);
  chanFreqMeasCol.write (td);
  
  // CHAN_WIDTH
  TableQuantumDesc chanWidthQuantDesc (td, MSC::fieldName (MSC::CHAN_WIDTH),
				       Unit ("Hz"));
  chanWidthQuantDesc.write (td);

  return td;
};

//----------------------------------------------------------------------------

TableDesc CalTableDesc::defaultCalMain (const String& type)
{
// Generate the default table descriptor for the Cal Main sub-table
// Input:
//    type             const String&      Cal table type (eg. "P Jones")
// Output:
//    defaultCalMain   TableDesc          Default Cal Main descriptor
//
  // Set up table descriptor and add comment field
  TableDesc td (type, "1.0", TableDesc::Scratch);
  td.comment() = type + " calibration table";

  // Define keywords
  Record keyWordRec;
  // Cal_desc and cal_history indices
  keyWordRec.define (MSC::fieldName (MSC::CAL_DESC_ID), 0);
  keyWordRec.define (MSC::fieldName (MSC::CAL_HISTORY_ID), 0);
  // Add to table descriptor
  td.rwKeywordSet().assign (keyWordRec);

  // Cal Main columns (MS Main indices)
  td.addColumn (ScalarColumnDesc <Double> (MSC::fieldName (MSC::TIME),
					   ColumnDesc::Direct));
  td.addColumn 
    (ScalarColumnDesc <Double> (MSC::fieldName (MSC::TIME_EXTRA_PREC),
				ColumnDesc::Direct));
  td.addColumn (ScalarColumnDesc <Double> (MSC::fieldName (MSC::INTERVAL),
					   ColumnDesc::Direct));
  td.addColumn (ScalarColumnDesc <Int> (MSC::fieldName (MSC::ANTENNA1),
					ColumnDesc::Direct));
  td.addColumn (ScalarColumnDesc <Int> (MSC::fieldName (MSC::FEED1),
					ColumnDesc::Direct));
  td.addColumn (ScalarColumnDesc <Int> (MSC::fieldName (MSC::FIELD_ID),
					ColumnDesc::Direct));
  td.addColumn (ScalarColumnDesc <Int> (MSC::fieldName (MSC::ARRAY_ID),
					ColumnDesc::Direct));
  td.addColumn (ScalarColumnDesc <Int> (MSC::fieldName (MSC::OBSERVATION_ID),
					ColumnDesc::Direct));
  td.addColumn (ScalarColumnDesc <Int> (MSC::fieldName (MSC::SCAN_NUMBER),
					ColumnDesc::Direct));
  td.addColumn (ScalarColumnDesc <Int> (MSC::fieldName (MSC::PROCESSOR_ID),
					ColumnDesc::Direct));
  td.addColumn (ScalarColumnDesc <Int> (MSC::fieldName (MSC::STATE_ID),
					ColumnDesc::Direct));
  td.addColumn (ScalarColumnDesc <Int> (MSC::fieldName (MSC::PHASE_ID),
					ColumnDesc::Direct));
  td.addColumn (ScalarColumnDesc <Int> (MSC::fieldName (MSC::PULSAR_BIN),
					ColumnDesc::Direct));
  td.addColumn (ScalarColumnDesc <Int> (MSC::fieldName (MSC::PULSAR_GATE_ID),
					ColumnDesc::Direct));

  // Secondary MS indices (from MS sub-tables)
  td.addColumn (ScalarColumnDesc <Int> (MSC::fieldName (MSC::FREQ_GROUP),
					ColumnDesc::Direct));
  td.addColumn (ScalarColumnDesc<String> (MSC::fieldName(MSC::FREQ_GROUP_NAME),
					  ColumnDesc::Direct));
  td.addColumn (ScalarColumnDesc <String> (MSC::fieldName (MSC::FIELD_NAME),
					   ColumnDesc::Direct));
  td.addColumn (ScalarColumnDesc <String> (MSC::fieldName (MSC::FIELD_CODE),
					   ColumnDesc::Direct));
  td.addColumn (ScalarColumnDesc <String> (MSC::fieldName (MSC::SOURCE_NAME),
					   ColumnDesc::Direct));
  td.addColumn (ScalarColumnDesc <String> (MSC::fieldName (MSC::SOURCE_CODE),
					   ColumnDesc::Direct));
  td.addColumn (ScalarColumnDesc <Int> (MSC::fieldName(MSC::CALIBRATION_GROUP),
					ColumnDesc::Direct));

  // Gain values
  td.addColumn (ArrayColumnDesc <Complex> (MSC::fieldName (MSC::GAIN)));

  // Reference frame for antenna-based corrections
  td.addColumn (ArrayColumnDesc <Int> (MSC::fieldName (MSC::REF_ANT)));
  td.addColumn (ArrayColumnDesc <Int> (MSC::fieldName (MSC::REF_FEED)));
  td.addColumn (ArrayColumnDesc <Int> (MSC::fieldName (MSC::REF_RECEPTOR)));
  td.addColumn (ArrayColumnDesc <Double> (MSC::fieldName(MSC::REF_FREQUENCY)));
  td.addColumn (ScalarColumnDesc <Int> (MSC::fieldName (MSC::MEAS_FREQ_REF),
					ColumnDesc::Direct));
  td.addColumn (ArrayColumnDesc <Double> (MSC::fieldName(MSC::REF_DIRECTION)));
  td.addColumn (ScalarColumnDesc <Int> (MSC::fieldName (MSC::MEAS_DIR_REF),
					ColumnDesc::Direct));

  // Pointers to Cal_Desc and Cal_History sub-tables
  td.addColumn (ScalarColumnDesc <Int> (MSC::fieldName (MSC::CAL_DESC_ID),
					ColumnDesc::Direct));
  td.addColumn (ScalarColumnDesc <Int> (MSC::fieldName (MSC::CAL_HISTORY_ID),
					ColumnDesc::Direct));

  // Add TableMeasures information for designated Measures/Quanta columns
  // 
  // TIME
  TableMeasValueDesc timeMeasVal (td, MSC::fieldName (MSC::TIME));
  TableMeasRefDesc timeMeasRef (MEpoch::DEFAULT);
  TableMeasDesc<MEpoch> timeMeasCol (timeMeasVal, timeMeasRef);
  timeMeasCol.write (td);

  // TIME fix Unit
  TableQuantumDesc timeQuantDesc (td, MSC::fieldName (MSC::TIME),
                                  Unit ("s"));
  timeQuantDesc.write (td);

  // TIME_EXTRA_PREC
  TableQuantumDesc timeEPQuantDesc (td, MSC::fieldName (MSC::TIME_EXTRA_PREC),
				    Unit ("s"));
  timeEPQuantDesc.write (td);

  // INTERVAL
  TableQuantumDesc intervalQuantDesc (td, MSC::fieldName(MSC::INTERVAL),
				      Unit ("s"));
  intervalQuantDesc.write (td);

  // REF_FREQUENCY
  TableMeasValueDesc refFreqMeasVal (td, MSC::fieldName (MSC::REF_FREQUENCY));
  TableMeasRefDesc refFreqMeasRef (td, MSC::fieldName (MSC::MEAS_FREQ_REF));
  TableMeasDesc<MFrequency> refFreqMeasCol (refFreqMeasVal, refFreqMeasRef);
  refFreqMeasCol.write (td);

  // REF_DIRECTION
  TableMeasValueDesc refDirMeasVal (td, MSC::fieldName (MSC::REF_DIRECTION));
  TableMeasRefDesc refDirMeasRef (td, MSC::fieldName (MSC::MEAS_DIR_REF));
  TableMeasDesc<MDirection> refDirMeasCol (refDirMeasVal, refDirMeasRef);
  refDirMeasCol.write (td);

  return td;
};

//----------------------------------------------------------------------------

TableDesc CalTableDesc::calMainDesc()
{
// Return the main calibration table descriptor
// Output:
//    calMainDesc     TableDesc        Table descriptor (cal_main)
//
  return itsCalMainDesc;
};

//----------------------------------------------------------------------------

TableDesc CalTableDesc::calHistoryDesc()
{
// Return the calibration history sub-table descriptor
// Output:
//    calHistoryDesc  TableDesc        Table descriptor (cal_history)
//
  return itsCalHistoryDesc;
};

//----------------------------------------------------------------------------

TableDesc CalTableDesc::calDescDesc()
{
// Return the calibration description sub-table descriptor
// Output:
//    calDescDesc     TableDesc        Table descriptor (cal_desc)
//
  return itsCalDescDesc;
};

//----------------------------------------------------------------------------

TableDesc CalTableDesc::defaultFitDesc()
{
// Generate the default table descriptor for fit parameters
// Output:
//    defaultFitDesc      TableDesc        Table descriptor (fit parameters)
// 
  TableDesc td;
  td.addColumn (ScalarColumnDesc <Bool> 
		(MSC::fieldName (MSC::TOTAL_SOLUTION_OK), ColumnDesc::Direct));
  td.addColumn (ScalarColumnDesc <Float> (MSC::fieldName (MSC::TOTAL_FIT),
					  ColumnDesc::Direct));
  td.addColumn (ScalarColumnDesc <Float> 
		(MSC::fieldName (MSC::TOTAL_FIT_WEIGHT), ColumnDesc::Direct));
  td.addColumn (ArrayColumnDesc <Bool> (MSC::fieldName (MSC::SOLUTION_OK)));
  td.addColumn (ArrayColumnDesc <Float> (MSC::fieldName (MSC::FIT)));
  td.addColumn (ArrayColumnDesc <Float> (MSC::fieldName (MSC::FIT_WEIGHT)));
  td.addColumn (ArrayColumnDesc <Bool> (MSC::fieldName (MSC::FLAG)));
  td.addColumn (ArrayColumnDesc <Float> (MSC::fieldName (MSC::SNR)));
  
  return td;
};

//----------------------------------------------------------------------------

TableDesc CalTableDesc::defaultPolyDesc()
{
// Generate the default table descriptor for general polynomial parameters
// Output:
//    defaultPolyDesc      TableDesc        Table descriptor (poly parameters)
// 
  TableDesc td;
  
  // Additional parameters required for general polynomials
  // POLY_TYPE              - polynomial type (e.g. Chebyshev or spline)
  // POLY_MODE              - polynomial y-value (e.g. A&P)
  // SCALE_FACTOR           - overall polynomial scale factor
  // VALID_DOMAIN           - valid polynomial domain [x_0, x_1]
  // N_POLY_AMP             - polynomial degree for amplitude
  // N_POLY_PHASE           - polynomial degree for phase
  // POLY_COEFF_AMP         - polynomial coefficients for amplitude
  // POLY_COEFF_PHASE       - polynomial coefficients for phase
  // PHASE_UNITS            - units for the phase polynomial
  //
  td.addColumn(ScalarColumnDesc<String>(MSC::fieldName(MSC::POLY_TYPE)));
  td.addColumn(ScalarColumnDesc<String>(MSC::fieldName(MSC::POLY_MODE)));
  td.addColumn(ScalarColumnDesc<Complex>(MSC::fieldName(MSC::SCALE_FACTOR)));
  td.addColumn(ArrayColumnDesc<Double>(MSC::fieldName(MSC::VALID_DOMAIN)));
  td.addColumn(ScalarColumnDesc<Int>(MSC::fieldName(MSC::N_POLY_AMP)));
  td.addColumn(ScalarColumnDesc<Int>(MSC::fieldName(MSC::N_POLY_PHASE)));
  td.addColumn(ArrayColumnDesc<Double>(MSC::fieldName(MSC::POLY_COEFF_AMP)));
  td.addColumn(ArrayColumnDesc<Double>(MSC::fieldName(MSC::POLY_COEFF_PHASE)));
  td.addColumn(ScalarColumnDesc<String>(MSC::fieldName(MSC::PHASE_UNITS)));
  
  return td;
};

//----------------------------------------------------------------------------

TableDesc CalTableDesc::defaultSplineDesc()
{
// Generate the default table descriptor for spline polynomial parameters
// Output:
//    defaultSplineDesc      TableDesc     Table descriptor (spline parameters)
// 
  TableDesc td;

  // Additional parameters required for spline polynomials.
  // N_KNOTS_AMP            - number of spline knots in amplitude
  // N_KNOTS_PHASE          - number of spline knots in phase
  // SPLINE_KNOTS_AMP       - spline knot positions for amplitude
  // SPLINE_KNOTS_PHASE     - spline knot positions for phase
  //
  td.addColumn(ScalarColumnDesc<Int>(MSC::fieldName(MSC::N_KNOTS_AMP)));
  td.addColumn(ScalarColumnDesc<Int>(MSC::fieldName(MSC::N_KNOTS_PHASE)));
  td.addColumn(ArrayColumnDesc<Double>(MSC::fieldName(MSC::SPLINE_KNOTS_AMP)));
  td.addColumn(ArrayColumnDesc<Double>
	       (MSC::fieldName(MSC::SPLINE_KNOTS_PHASE)));
  
  return td;
};

//----------------------------------------------------------------------------

TableDesc CalTableDesc::insertDesc (const TableDesc& tableDesc, 
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

