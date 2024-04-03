//# TJonesDesc.h: Define the format of TJones cal tables
//# Copyright (C) 1996,1997,2002,2003
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

#ifndef CALIBRATION_TJONESDESC_H
#define CALIBRATION_TJONESDESC_H

#include <synthesis/CalTables/SolvableVJDesc.h>

namespace casa { //# NAMESPACE CASA - BEGIN

// <summary> 
// TJonesPolyDesc: Define the format of TJonesPoly cal tables
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//<li><linkto class="CalTableDesc">CalTableDesc</linkto> module
//<li><linkto class="VisJonesDesc">VisJonesDesc</linkto> module
//<li><linkto class="TimeVarVisJonesDesc">TimeVarVisJonesDesc</linkto> module
//<li><linkto class="SolvableVisJonesDesc">SolvableVisJonesDesc</linkto> module
//<li><linkto class="TJonesDesc">TJonesDesc</linkto> module
//
// </prerequisite>
//
// <etymology>
// From "TJones", "polynomial" and "descriptor".
// </etymology>
//
// <synopsis>
// TJonesPolyDesc defines the format of TJonesPoly calibration tables 
// in terms of table descriptors <linkto class="casacore::TableDesc">casacore::TableDesc</linkto>, 
// as used in the casacore::Table system. TJonesPoly matrices represent atmospheric
// gain corrections in the Measurement Equation formalism expressed as
// polynomials over time.
// </synopsis>
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
// <todo asof="02/01/01">
// i) Move TJonesDesc to this file
// </todo>

class TJonesPolyDesc : public TJonesDesc
{
 public:
   // Default null constructor, and destructor
   TJonesPolyDesc();
   virtual ~TJonesPolyDesc() {};

   // Return the table descriptor for the main calibration table
   virtual casacore::TableDesc calMainDesc();
};

// <summary> 
// TJonesSplineDesc: Define the format of TJonesSpline cal tables
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//<li><linkto class="CalTableDesc">CalTableDesc</linkto> module
//<li><linkto class="VisJonesDesc">VisJonesDesc</linkto> module
//<li><linkto class="TimeVarVisJonesDesc">TimeVarVisJonesDesc</linkto> module
//<li><linkto class="SolvableVisJonesDesc">SolvableVisJonesDesc</linkto> module
//<li><linkto class="TJonesDesc">TJonesDesc</linkto> module
//<li><linkto class="TJonesPolyDesc">TJonesPolyDesc</linkto> module
//
// </prerequisite>
//
// <etymology>
// From "TJones", "polynomial" and "descriptor".
// </etymology>
//
// <synopsis>
// TJonesSplineDesc defines the format of TJonesSpline calibration tables 
// in terms of table descriptors <linkto class="casacore::TableDesc">casacore::TableDesc</linkto>, 
// as used in the casacore::Table system. TJonesSpline matrices represent atmospheric
// gain corrections in the Measurement Equation formalism expressed as
// spline polynomials over time.
// </synopsis>
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
// <todo asof="02/01/01">
// i) Move TJonesDesc to this file
// </todo>

class TJonesSplineDesc : public TJonesPolyDesc
{
 public:
   // Default null constructor, and destructor
   TJonesSplineDesc();
   virtual ~TJonesSplineDesc() {};

   // Return the table descriptor for the main calibration table
   virtual casacore::TableDesc calMainDesc();
};


} //# NAMESPACE CASA - END

#endif
   
  



