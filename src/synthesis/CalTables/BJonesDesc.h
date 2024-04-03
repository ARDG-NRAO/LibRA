//# BJonesDesc.h: Define the format of BJones cal tables
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

#ifndef CALIBRATION_BJONESDESC_H
#define CALIBRATION_BJONESDESC_H

#include <synthesis/CalTables/SolvableVJDesc.h>

namespace casa { //# NAMESPACE CASA - BEGIN

// <summary> 
// BJonesDesc: Define the format of BJones cal tables
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//<li><linkto class="CalTableDesc">CalTableDesc</linkto> module
//<li><linkto class="VisJonesDesc">VisJonesDesc</linkto> module
//<li><linkto class="TimeVarVisJonesDesc">TimeVarVisJonesDesc</linkto> module
//<li><linkto class="SolvableVisJonesDesc">SolvableVisJonesDesc</linkto> module
//
// </prerequisite>
//
// <etymology>
// From "BJones" and "descriptor".
// </etymology>
//
// <synopsis>
// BJonesDesc defines the format of BJones calibration tables in terms 
// of table descriptors <linkto class="casacore::TableDesc">casacore::TableDesc</linkto>, 
// as used in the casacore::Table system. BJones matrices represent bandpass
// corrections in the Measurement Equation formalism. At present, 
// this set of classes returns only the default table formats.
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

class BJonesDesc : public SolvableVisJonesDesc
{
 public:
   // Default null constructor, and destructor
   BJonesDesc();
   virtual ~BJonesDesc() {};
};


// <summary> 
// BJonesPolyDesc: Define the format of BJonesPoly cal tables
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//<li><linkto class="CalTableDesc">CalTableDesc</linkto> module
//<li><linkto class="VisJonesDesc">VisJonesDesc</linkto> module
//<li><linkto class="TimeVarVisJonesDesc">TimeVarVisJonesDesc</linkto> module
//<li><linkto class="SolvableVisJonesDesc">SolvableVisJonesDesc</linkto> module
//<li><linkto class="BJonesDesc">BJonesDesc</linkto> module
//
// </prerequisite>
//
// <etymology>
// From "BJones", "polynomial" and "descriptor".
// </etymology>
//
// <synopsis>
// BJonesPolyDesc defines the format of BJonesPoly calibration tables 
// in terms of table descriptors <linkto class="casacore::TableDesc">casacore::TableDesc</linkto>, 
// as used in the casacore::Table system. BJonesPoly matrices represent bandpass
// corrections in the Measurement Equation formalism in polynomial form. 
// At present, these include b-spline and casacore::Chebyshev polynomial representations,
// using the solver developed for Plateau de Bure interferometer data by
// Anterrieu et al.
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
// <todo asof="02/01/01">
// Generate calibration table descriptors from a VisSet or VisBuffer object
// </todo>

class BJonesPolyDesc : public BJonesDesc
{
 public:
   // Default null constructor, and destructor
   BJonesPolyDesc();
   virtual ~BJonesPolyDesc() {};

   // Return the table descriptor for the main calibration table
   virtual casacore::TableDesc calMainDesc();
};


} //# NAMESPACE CASA - END

#endif
   
  



