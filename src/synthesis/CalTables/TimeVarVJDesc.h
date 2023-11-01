//# TimeVarVJDesc.h: Define the format of TimeVarVisJones cal tables
//# Copyright (C) 1996,1997,2003
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

#ifndef CALIBRATION_TIMEVARVJDESC_H
#define CALIBRATION_TIMEVARVJDESC_H

#include <synthesis/CalTables/VisJonesDesc.h>

namespace casa { //# NAMESPACE CASA - BEGIN

// <summary> 
// TimeVarVisJonesDesc: Define the format of TimeVarVisJones cal tables
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//   <li> <linkto class="CalTableDesc">CalTableDesc</linkto> module
//   <li> <linkto class="VisJonesDesc">VisJonesDesc</linkto> module
// </prerequisite>
//
// <etymology>
// From "time variable VisJones" and "descriptor".
// </etymology>
//
// <synopsis>
// TimeVarVisJonesDesc defines the format of TimeVarVisJones calibration 
// tables in terms of table descriptors <linkto class="casacore::TableDesc">TableDesc
// </linkto>, as used in the casacore::Table system. Specializations for 
// time-variable and solvable VisJones calibration tables are provided 
// through inheritance. At present, this set of classes returns only the 
// default table formats.
// </etymology>
//
// <example>
// <srcblock>
// </srcblock>
// </example>
//
// <motivation>
// See CalTableDesc.
// </motivation>
//
// <todo asof="98/01/01">
// Generate calibration table descriptors from a VisSet or VisBuffer object
// </todo>

class TimeVarVisJonesDesc : public VisJonesDesc
{
 public:
   // Default null constructor, and destructor
   TimeVarVisJonesDesc();
   virtual ~TimeVarVisJonesDesc() {};

   // Construct from the Jones table type
   TimeVarVisJonesDesc (const casacore::String& type);

};

// <summary> 
// PJonesDesc: Define the format of PJones cal tables
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
// <li> <linkto class="CalTableDesc">CalTableDesc</linkto> module
// <li> <linkto class="VisJonesDesc">VisJonesDesc</linkto> module
// <li> <linkto class="TimeVarVisJonesDesc">TimeVarVisJonesDesc</linkto> module
// </prerequisite>
//
// <etymology>
// From "PJones" and "descriptor".
// </etymology>
//
// <synopsis>
// PJonesDesc defines the format of PJones calibration tables in terms 
// of table descriptors <linkto class="casacore::TableDesc">casacore::TableDesc</linkto>, 
// as used in the casacore::Table system. PJones matrices define the parallactic
// angle corrections in the Measurement Equation formalism. At present, 
// this class returns only the default table formats.
// </etymology>
//
// <example>
// <srcblock>
// </srcblock>
// </example>
//
// <motivation>
// See CalTableDesc.
// </motivation>
//
// <todo asof="98/01/01">
// Generate calibration table descriptors from a VisSet or VisBuffer object
// </todo>

class PJonesDesc : public TimeVarVisJonesDesc
{
 public:
   // Default null constructor, and destructor
   PJonesDesc();
   virtual ~PJonesDesc() {};

};

// <summary> 
// CJonesDesc: Define the format of CJones cal tables
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
// <li> <linkto class="CalTableDesc">CalTableDesc</linkto> module
// <li> <linkto class="VisJonesDesc">VisJonesDesc</linkto> module
// <li> <linkto class="TimeVarVisJonesDesc">TimeVarVisJonesDesc</linkto> module
// </prerequisite>
//
// <etymology>
// From "CJones" and "descriptor".
// </etymology>
//
// <synopsis>
// CJonesDesc defines the format of CJones calibration tables in terms 
// of table descriptors <linkto class="casacore::TableDesc">casacore::TableDesc</linkto>, 
// as used in the casacore::Table system. CJones matrices define the polarization
// configuration in the Measurement Equation formalism. At present, 
// this class returns only the default table formats.
// </etymology>
//
// <example>
// <srcblock>
// </srcblock>
// </example>
//
// <motivation>
// See CalTableDesc.
// </motivation>
//
// <todo asof="98/01/01">
// Generate calibration table descriptors from a VisSet or VisBuffer object
// </todo>

class CJonesDesc : public TimeVarVisJonesDesc
{
 public:
   // Default null constructor, and destructor
   CJonesDesc();
   virtual ~CJonesDesc() {};

};


} //# NAMESPACE CASA - END

#endif
   
  



