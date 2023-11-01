//# BJonesDesc.cc: Implementation of BJonesDesc.h
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

#include <synthesis/CalTables/BJonesDesc.h>
#include <casacore/tables/Tables/ScaColDesc.h>
#include <casacore/tables/Tables/ArrColDesc.h>
#include <msvis/MSVis/MSCalEnums.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

//----------------------------------------------------------------------------

BJonesDesc::BJonesDesc() : SolvableVisJonesDesc("B Jones")
{
// Default null constructor for BJones calibration table description (v2.0)
};

//----------------------------------------------------------------------------

BJonesPolyDesc::BJonesPolyDesc() : BJonesDesc()
{
// Default null constructor for BJonesPoly calibration table description (v2.0)
};

//----------------------------------------------------------------------------

TableDesc BJonesPolyDesc::calMainDesc()
{
// Return the cal_main table descriptor for BJonesPoly
// Output:
//    calMainDesc        TableDesc          cal_mail table descriptor
//
  // Construct a table descriptor holding the extra parameters
  // required for general polynomial calibration
  TableDesc td(defaultPolyDesc());

  // Add the sideband reference phasor value
  td.addColumn(ScalarColumnDesc<Complex>(MSC::fieldName(MSC::SIDEBAND_REF)));

  return insertDesc(BJonesDesc::calMainDesc(), td, "GAIN");
};

//----------------------------------------------------------------------------

} //# NAMESPACE CASA - END

