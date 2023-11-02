//# SolvableMJDesc.cc: Implementation of SolvableMJDesc.h
//# Copyright (C) 1996,1997,1998,2001,2003
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

#include <synthesis/CalTables/SolvableMJDesc.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

//----------------------------------------------------------------------------

SolvableMJonesDesc::SolvableMJonesDesc() : TimeVarMJonesDesc(),
  itsFitDesc()
{
// Default null constructor for solvable MJones calibration 
// table description (v2.0)
// Output to private data:
//    itsFitDesc      TableDesc      Table descriptor for fit parameters
//
  // Default fit statistics and weights 
  itsFitDesc.add (defaultFitDesc());
};

//----------------------------------------------------------------------------

TableDesc SolvableMJonesDesc::calMainDesc()
{
// Return the cal_main table descriptor
// Output:
//    calMainDesc        TableDesc       cal_main table descriptor
//
  // Insert fit statistics and weights after column "GAIN" in
  // TimeVarMJones table descriptor.
  TableDesc tdout (insertDesc (TimeVarMJonesDesc::calMainDesc(), itsFitDesc,
			       "GAIN"));
  
  return tdout;
};

//----------------------------------------------------------------------------

MIfrDesc::MIfrDesc() : SolvableMJonesDesc()
{
// Default null constructor for MIfr calibration table description (v2.0)
};

//----------------------------------------------------------------------------


} //# NAMESPACE CASA - END

