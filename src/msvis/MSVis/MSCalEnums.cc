//# MSCalEnums.cc: Implementation of MSCalEnums.h
//# Copyright (C) 1996,1997,1998,2000,2001,2002
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

#include <msvis/MSVis/MSCalEnums.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

//----------------------------------------------------------------------------

// Static data initialization
std::map <Int, String> MSCalEnums::theirFieldMap;
std::map <Int, DataType> MSCalEnums::theirTypeMap;

//----------------------------------------------------------------------------

void MSCalEnums::initMaps ()
{
// Initialize the static map containing the field names.
// Skip this step if already initialized.
//
  if ( theirFieldMap.size( ) == 0 ) {
      theirFieldMap = {
          {ANTENNA1, "ANTENNA1"},
          {ANTENNA2, "ANTENNA2"},
          { FEED1, "FEED1" },
          { FEED2, "FEED2" },
          { PULSAR_BIN, "PULSAR_BIN" },
          { SCAN_NUMBER, "SCAN_NUMBER" },
          { TIME, "TIME" },
          { TIME_EXTRA_PREC, "TIME_EXTRA_PREC" },
          { INTERVAL, "INTERVAL" },
          { ARRAY_ID, "ARRAY_ID" },
          { PROCESSOR_ID, "PROCESSOR_ID" },
          { FIELD_ID, "FIELD_ID" },
          { OBSERVATION_ID, "OBSERVATION_ID" },
          { PULSAR_GATE_ID, "PULSAR_GATE_ID" },
          { SPECTRAL_WINDOW_ID, "SPECTRAL_WINDOW_ID" },
          { PHASE_ID, "PHASE_ID" },
          { STATE_ID, "STATE_ID" },

          { FREQ_GROUP, "FREQ_GROUP" },
          { FREQ_GROUP_NAME, "FREQ_GROUP_NAME" },
          { FIELD_NAME, "FIELD_NAME" },
          { FIELD_CODE, "FIELD_CODE" },
          { SOURCE_NAME, "SOURCE_NAME" },
          { SOURCE_CODE, "SOURCE_CODE" },
          { CALIBRATION_GROUP, "CALIBRATION_GROUP" },

          { GAIN, "GAIN" },
          { REF_ANT, "REF_ANT" },
          { REF_FEED, "REF_FEED" },
          { REF_RECEPTOR, "REF_RECEPTOR" },
          { REF_FREQUENCY, "REF_FREQUENCY" },
          { MEAS_FREQ_REF, "MEAS_FREQ_REF" },
          { REF_DIRECTION, "REF_DIRECTION" },
          { MEAS_DIR_REF, "MEAS_DIR_REF" },
          { POINTING_OFFSET, "POINTING_OFFSET" },
          { MEAS_POINTING, "MEAS_POINTING" },
          { CAL_DESC_ID, "CAL_DESC_ID" },
          { CAL_HISTORY_ID, "CAL_HISTORY_ID" },
    
          { TOTAL_SOLUTION_OK, "TOTAL_SOLUTION_OK" },
          { TOTAL_FIT, "TOTAL_FIT" },
          { TOTAL_FIT_WEIGHT, "TOTAL_FIT_WEIGHT" },
          { SOLUTION_OK, "SOLUTION_OK" },
          { FIT, "FIT" },
          { FIT_WEIGHT, "FIT_WEIGHT" },
          { FLAG, "FLAG" },
          { SNR, "SNR" },
    
          { NUM_SPW, "NUM_SPW" },
          { NUM_CHAN, "NUM_CHAN" },
          { NUM_RECEPTORS, "NUM_RECEPTORS" },
          { N_JONES, "N_JONES" },
          { CHAN_FREQ, "CHAN_FREQ" },
          { CHAN_WIDTH, "CHAN_WIDTH" },
          { CHAN_RANGE, "CHAN_RANGE" },
          { JONES_TYPE, "JONES_TYPE" },
          { POLARIZATION_TYPE, "POLARIZATION_TYPE" },
          { MS_NAME, "MS_NAME" },
    
          { CAL_PARMS, "CAL_PARMS" },
          { CAL_TABLES, "CAL_TABLES" },
          { CAL_SELECT, "CAL_SELECT" },
          { CAL_NOTES, "CAL_NOTES" },
    
          { CAL_DESC, "CAL_DESC" },
          { CAL_HISTORY, "CAL_HISTORY" },
          { OBSERVATION, "OBSERVATION" },
    
          { ROT_MEASURE, "ROT_MEASURE" },
          { ROT_MEASURE_ERROR, "ROT_MEASURE_ERROR" },
          { IONOSPH_TEC, "IONOSPH_TEC" },
          { IONOSPH_TEC_ERROR, "IONOSPH_TEC_ERROR" },

          { PHASE_OFFSET, "PHASE_OFFSET" },
          { SB_DELAY, "SB_DELAY" },
          { DELAY_RATE, "DELAY_RATE" },

          { POLY_TYPE, "POLY_TYPE" },
          { POLY_MODE, "POLY_MODE" },
          { SCALE_FACTOR, "SCALE_FACTOR" },
          { VALID_DOMAIN, "VALID_DOMAIN" },
          { N_POLY_AMP, "N_POLY_AMP" },
          { N_POLY_PHASE, "N_POLY_PHASE" },
          { POLY_COEFF_AMP, "POLY_COEFF_AMP" },
          { POLY_COEFF_PHASE, "POLY_COEFF_PHASE" },
          { PHASE_UNITS, "PHASE_UNITS" },

          { SIDEBAND_REF, "SIDEBAND_REF" },

          { N_KNOTS_AMP, "N_KNOTS_AMP" },
          { N_KNOTS_PHASE, "N_KNOTS_PHASE" },
          { SPLINE_KNOTS_AMP, "SPLINE_KNOTS_AMP" },
          { SPLINE_KNOTS_PHASE, "SPLINE_KNOTS_PHASE" }
      };
  }

// Initialize the static map containing the basic field data types
// Skip this step if already initialized.
//
  if ( theirTypeMap.size( ) == 0 ) {
      theirTypeMap = {
          { ANTENNA1, TpInt },
          { ANTENNA2, TpInt },
          { FEED1, TpInt },
          { FEED2, TpInt },
          { PULSAR_BIN, TpInt },
          { SCAN_NUMBER, TpInt },
          { TIME, TpDouble },
          { TIME_EXTRA_PREC, TpDouble },
          { INTERVAL, TpDouble },
          { ARRAY_ID, TpInt },
          { PROCESSOR_ID, TpInt },
          { FIELD_ID, TpInt },
          { OBSERVATION_ID, TpInt },
          { PULSAR_GATE_ID, TpInt },
          { SPECTRAL_WINDOW_ID, TpInt },
          { PHASE_ID, TpInt },
          { STATE_ID, TpInt },

          { FREQ_GROUP, TpInt },
          { FREQ_GROUP_NAME, TpString },
          { FIELD_NAME, TpString },
          { FIELD_CODE, TpString },
          { SOURCE_NAME, TpString },
          { SOURCE_CODE, TpString },
          { CALIBRATION_GROUP, TpInt },

          { GAIN, TpComplex },
          { REF_ANT, TpInt },
          { REF_FEED, TpInt },
          { REF_RECEPTOR, TpInt },
          { REF_FREQUENCY, TpDouble },
          { MEAS_FREQ_REF, TpInt },
          { REF_DIRECTION, TpDouble },
          { MEAS_DIR_REF, TpInt },
          { CAL_DESC_ID, TpInt },
          { CAL_HISTORY_ID, TpInt },
    
          { TOTAL_SOLUTION_OK, TpBool },
          { TOTAL_FIT, TpFloat },
          { TOTAL_FIT_WEIGHT, TpFloat },
          { SOLUTION_OK, TpBool },
          { FIT, TpFloat },
          { FIT_WEIGHT, TpFloat },
          { FLAG, TpBool },
          { SNR, TpFloat },
    
          { NUM_SPW, TpInt },
          { NUM_CHAN, TpInt },
          { NUM_RECEPTORS, TpInt },
          { N_JONES, TpInt },
          { CHAN_FREQ, TpDouble },
          { CHAN_WIDTH, TpDouble },
          { CHAN_RANGE, TpInt },
          { JONES_TYPE, TpString },
          { POLARIZATION_TYPE, TpString },
          { MS_NAME, TpString },
    
          { CAL_PARMS, TpString },
          { CAL_TABLES, TpString },
          { CAL_SELECT, TpString },
          { CAL_NOTES, TpString },
    
          { CAL_DESC, TpTable },
          { CAL_HISTORY, TpTable },
          { OBSERVATION, TpTable },
    
          { ROT_MEASURE, TpFloat },
          { ROT_MEASURE_ERROR, TpFloat },
          { IONOSPH_TEC, TpFloat },
          { IONOSPH_TEC_ERROR, TpFloat },

          { PHASE_OFFSET, TpFloat },
          { SB_DELAY, TpFloat },
          { DELAY_RATE, TpFloat },

          { POLY_TYPE, TpString },
          { POLY_MODE, TpString },
          { SCALE_FACTOR, TpComplex },
          { VALID_DOMAIN, TpDouble },
          { N_POLY_AMP, TpInt },
          { N_POLY_PHASE, TpInt },
          { POLY_COEFF_AMP, TpDouble },
          { POLY_COEFF_PHASE, TpDouble },
          { PHASE_UNITS, TpString },

          { SIDEBAND_REF, TpComplex },

          { N_KNOTS_AMP, TpInt },
          { N_KNOTS_PHASE, TpInt },
          { SPLINE_KNOTS_AMP, TpDouble },
          { SPLINE_KNOTS_PHASE, TpDouble }
      };
  }

}

//----------------------------------------------------------------------------

String MSCalEnums::fieldName (Int enumField)
{
// Static function to look up the field name:
// Inputs:
//    enumField   Int     Field enumeration.
// Outputs:
//    fieldName   String  Field name.
// Exceptions:
//    Exception if invalid field enumeration.
//
  // Initialize map if empty
  if ( theirFieldMap.size( ) == 0 ) initMaps();
  
  // Return the column name
  return theirFieldMap[enumField];
};

//----------------------------------------------------------------------------

Block<String> MSCalEnums::fieldNames (const Vector<Int>& enumFields)
{
// Static function to look up a set of field names:
// Inputs:
//    enumFields  const Vector<Int>&     Field enumerations.
// Outputs:
//    fieldNames  Block<String>          Field names.
// Exceptions:
//    Exception if invalid field enumeration.
//
  // Return the column names
  uInt nFields = enumFields.nelements();
  Block<String> names(nFields);
  for (uInt i=0; i < nFields; i++) {
    names[i] = fieldName (enumFields(i));
  };
  return names;
};

//----------------------------------------------------------------------------

DataType MSCalEnums::basicType (Int enumField)
{
// Static function to look up the basic field data type:
// Inputs:
//    enumField   Int        Field enumeration.
// Outputs:
//    basicType   DataType   Basic data type
// Exceptions:
//    Exception if invalid field enumeration.
//
  // Initialize map if empty
  if ( theirTypeMap.size( ) == 0 ) initMaps();
  
  // Return the column name
  return theirTypeMap[enumField];
};

//----------------------------------------------------------------------------







} //# NAMESPACE CASA - END

