//# FVisJonesDesc.h: Define the format of FVisJones cal tables
//# Copyright (C) 1996,1997,2000,2003
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

#ifndef CALIBRATION_FVISJONESDESC_H
#define CALIBRATION_FVISJONESDESC_H

#include <synthesis/CalTables/TimeVarVJDesc.h>

namespace casa { //# NAMESPACE CASA - BEGIN

// <summary> 
// FVisJonesDesc: Define the format of FVisJones cal tables
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//   <li> <linkto class="CalTableDesc">CalTableDesc</linkto> module
//   <li> <linkto class="TimeVarVisJonesDesc">TimeVarVisJonesDesc</linkto> module
// </prerequisite>
//
// <etymology>
// From "F VisJones" and "descriptor".
// </etymology>
//
// <synopsis>
// FVisJonesDesc defines the format of FVisJones calibration 
// tables in terms of table descriptors <linkto class="casacore::TableDesc">TableDesc
// </linkto>, as used in the casacore::Table system. It auguments the TimeVarVisJones
// description with additional fields for rotation measures.
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

class FVisJonesDesc : public TimeVarVisJonesDesc
{
 public:
   // Default null constructor, and destructor
   FVisJonesDesc();
   virtual ~FVisJonesDesc() {};

   // Construct from the Jones table type
   FVisJonesDesc (const casacore::String& type);

   // Return the table descriptors for the main calibration table
   virtual casacore::TableDesc calMainDesc();
   
 private:
   casacore::TableDesc itsCalDescDesc;

};

// <summary> 
// FVisJonesIonoDesc: Define the format of FVisJonesIono cal tables
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//   <li> <linkto class="CalTableDesc">CalTableDesc</linkto> module
//   <li> <linkto class="FVisJonesDesc">FVisJonesDesc</linkto> module
// </prerequisite>
//
// <etymology>
// From "F VisJones - Ionosphere" and "descriptor".
// </etymology>
//
// <synopsis>
// FVisJonesIonoDesc defines the format of FVisJonesIono calibration 
// tables in terms of table descriptors <linkto class="casacore::TableDesc">TableDesc
// </linkto>, as used in the casacore::Table system. It auguments the FVisJones
// description with additional fields for TEC, etc.
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

class FVisJonesIonoDesc : public FVisJonesDesc
{
 public:
   // Default null constructor, and destructor
   FVisJonesIonoDesc();
   virtual ~FVisJonesIonoDesc() {};

   // Construct from the Jones table type
   FVisJonesIonoDesc (const casacore::String& type);

   // Return the table descriptors for the main calibration table
   virtual casacore::TableDesc calMainDesc();

};


} //# NAMESPACE CASA - END

#endif
