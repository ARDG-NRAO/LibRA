//# CTEnums.h: Field name dictionary for NewCalTable
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

#ifndef CALIBRATION_NEWCALTABLENUMS_H
#define CALIBRATION_NEWCALTABLENUMS_H

#include <map>
#include <casacore/casa/aips.h>
#include <casacore/casa/Containers/Block.h>
#include <casacore/casa/Arrays/Vector.h>
#include <casacore/casa/BasicSL/String.h>
#include <casacore/casa/Utilities/DataType.h>

namespace casa { //# NAMESPACE CASA - BEGIN

// <summary> 
// CTEnums: Field name dictionary for NewCalTable
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//   <li> <linkto class="NewCalTable">NewCalTable</linkto> module
// </prerequisite>
//
// <etymology>
// From calibration enumerations.
// </etymology>
//
// <synopsis>
// CTEnums defines a global dictionary of field names and properties
// used by the NewCalTable. Field names
// and properties are accessed through enumerated constants, thus
// ensuring a consistent and uniform interface. Both column names and
// keywords are included here. All data are stored in static maps.
// </synopsis>
//
// <example>
// <srcblock>
//      // Determine the column name for the SPECTRAL_WINDOW_ID column
//      casacore::String name = MSC::fieldName (MSC::SPECTRAL_WINDOW_ID));
//
// </srcblock>
// </example>
//
// <motivation>
// </motivation>
//
// <todo asof="11/07/13">
// 
// </todo>


class CTEnums;
typedef CTEnums NCT;

class CTEnums
{
 public:
  // Enumerate all relevant data fields (columns and keywords)
  enum colDef {
  // new cal table columns
  TIME = 0,
  // Extra time precision
  TIME_EXTRA_PREC = 1,
  // Field identifier
  FIELD_ID = 2,
  // Spectral window identifier
  SPECTRAL_WINDOW_ID = 3,
  // Antenna number
  ANTENNA1 = 4,
  ANTENNA2 = 5,
  // Interval
  INTERVAL = 6,
  // scan number
  SCAN_NUMBER = 7,
  // Observation identifier
  OBSERVATION_ID = 8,
  // casacore::Array identifier
  ARRAY_ID = 9,
  
  // Solution parameters and weights
  // cal solution params. 
  CPARAM = 100,
  FPARAM = 101,
  // cal solution param errors
  PARAMERR = 102,
  // flag
  FLAG = 103, 
  // Solution SNR
  SNR = 104,
  // solution weight
  WEIGHT = 105,
  
  };

  // Access functions for column/keyword description data
  static casacore::String fieldName (casacore::Int enumField);
  static casacore::Block<casacore::String> fieldNames (const casacore::Vector<casacore::Int>& enumFields);
  static casacore::DataType basicType (casacore::Int enumField);
 
 private:
  // Initialize the static maps
  static void initMaps();
  
  // Static ordered maps containing field descriptions
  // Enum => Field Name
  static std::map <casacore::Int, casacore::String> theirFieldMap;
  // Enum => Basic data type
  static std::map <casacore::Int, casacore::DataType> theirTypeMap;
};


} //# NAMESPACE CASA - END

#endif
