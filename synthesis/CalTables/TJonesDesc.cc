//# TJonesDesc.cc: Implementation of TJonesDesc.h
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

#include <synthesis/CalTables/TJonesDesc.h>
#include <casacore/tables/Tables/ScaColDesc.h>
#include <casacore/tables/Tables/ArrColDesc.h>
#include <msvis/MSVis/MSCalEnums.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

//----------------------------------------------------------------------------

TJonesPolyDesc::TJonesPolyDesc() : TJonesDesc()
{
// Default null constructor for TJonesPoly cal. table description (v2.0)
};

//----------------------------------------------------------------------------

TableDesc TJonesPolyDesc::calMainDesc()
{
// Return the cal_main table descriptor for TJonesPoly
// Output:
//    calMainDesc        TableDesc          cal_mail table descriptor
//
  // Create a table descriptor containing the extra TJonesPoly parameters
  TableDesc td(defaultPolyDesc());;

  // Insert in the TJones descriptor after the GAIN column
  return insertDesc(TJonesDesc::calMainDesc(), td, "GAIN");
};

//----------------------------------------------------------------------------

TJonesSplineDesc::TJonesSplineDesc() : TJonesPolyDesc()
{
// Default null constructor for TJonesSpline cal. table description (v2.0)
};

//----------------------------------------------------------------------------

TableDesc TJonesSplineDesc::calMainDesc()
{
// Return the cal_main table descriptor for TJonesSpline
// Output:
//    calMainDesc        TableDesc          cal_mail table descriptor
//
  // Create a table descriptor containing the extra TJonesSpline parameters
  TableDesc td(defaultSplineDesc());

  // Insert in the TJonesPoly descriptor after the POLY_MODE column
  return insertDesc(TJonesPolyDesc::calMainDesc(), td, "POLY_MODE");
};

//----------------------------------------------------------------------------

} //# NAMESPACE CASA - END

