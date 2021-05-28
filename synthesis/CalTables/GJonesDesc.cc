//# GJonesDesc.cc: Implementation of GJonesDesc.h
//# Copyright (C) 1996,1997,2001,2002,2003
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

#include <synthesis/CalTables/GJonesDesc.h>
#include <tables/Tables/ScaColDesc.h>
#include <tables/Tables/ArrColDesc.h>
#include <msvis/MSVis/MSCalEnums.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

//----------------------------------------------------------------------------

GJonesPolyDesc::GJonesPolyDesc() : GJonesDesc()
{
// Default null constructor for GJonesPoly cal. table description (v2.0)
};

//----------------------------------------------------------------------------

TableDesc GJonesPolyDesc::calMainDesc()
{
// Return the cal_main table descriptor for GJonesPoly
// Output:
//    calMainDesc        TableDesc          cal_mail table descriptor
//
  // Create a table descriptor containing the extra GJonesPoly parameters
  TableDesc td(defaultPolyDesc());;

  // Insert in the GJones descriptor after the GAIN column
  return insertDesc(GJonesDesc::calMainDesc(), td, "GAIN");
};

//----------------------------------------------------------------------------

GJonesSplineDesc::GJonesSplineDesc() : GJonesPolyDesc()
{
// Default null constructor for GJonesSpline cal. table description (v2.0)
};

//----------------------------------------------------------------------------

TableDesc GJonesSplineDesc::calMainDesc()
{
// Return the cal_main table descriptor for GJonesSpline
// Output:
//    calMainDesc        TableDesc          cal_mail table descriptor
//
  // Create a table descriptor containing the extra GJonesSpline parameters
  TableDesc td(defaultSplineDesc());

  // Insert in the GJonesPoly descriptor after the POLY_MODE column
  return insertDesc(GJonesPolyDesc::calMainDesc(), td, "POLY_MODE");
};

//----------------------------------------------------------------------------

} //# NAMESPACE CASA - END

