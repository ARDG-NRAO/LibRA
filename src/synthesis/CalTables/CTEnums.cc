//# CTEnums.cc: Implementation of CTEnums.h
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

#include <synthesis/CalTables/CTEnums.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

//----------------------------------------------------------------------------

// Static data initialization
std::map <Int, String> CTEnums::theirFieldMap;
std::map <Int, DataType> CTEnums::theirTypeMap;

//----------------------------------------------------------------------------

void CTEnums::initMaps ()
{
// Initialize the static map containing the field names.
// Skip this step if already initialized.
//
  if ( theirFieldMap.size( ) == 0 ) {
      theirFieldMap = {
          {ANTENNA1, "ANTENNA1"},
          {ANTENNA2, "ANTENNA2"},
          {SCAN_NUMBER, "SCAN_NUMBER"},
          {TIME, "TIME"},
          {TIME_EXTRA_PREC, "TIME_EXTRA_PREC"},
          {INTERVAL, "INTERVAL"},
          {ARRAY_ID, "ARRAY_ID"},
          {FIELD_ID, "FIELD_ID"},
          {OBSERVATION_ID, "OBSERVATION_ID"},
          {SPECTRAL_WINDOW_ID, "SPECTRAL_WINDOW_ID"},

          {CPARAM, "CPARAM"},
          {FPARAM, "FPARAM"},
          {PARAMERR, "PARAMERR"},
          {FLAG, "FLAG"},
          {SNR, "SNR"},
          {WEIGHT, "WEIGHT"}
      };

  };

// Initialize the static map containing the basic field data types
// Skip this step if already initialized.
//
  if ( theirTypeMap.size( ) == 0 ) {
      theirTypeMap = {
          {ANTENNA1, TpInt},
          {ANTENNA2, TpInt},
          {SCAN_NUMBER, TpInt},
          {TIME, TpDouble},
          {TIME_EXTRA_PREC, TpDouble},
          {INTERVAL, TpDouble},
          {ARRAY_ID, TpInt},
          {FIELD_ID, TpInt},
          {OBSERVATION_ID, TpInt},
          {SPECTRAL_WINDOW_ID, TpInt},

          {CPARAM, TpComplex},
          {FPARAM, TpFloat},
          {PARAMERR, TpFloat},
          {FLAG, TpBool},
          {SNR, TpFloat},
          {WEIGHT, TpFloat}
      };

  }

};

//----------------------------------------------------------------------------

String CTEnums::fieldName (Int enumField)
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

Block<String> CTEnums::fieldNames (const Vector<Int>& enumFields)
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

DataType CTEnums::basicType (Int enumField)
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

