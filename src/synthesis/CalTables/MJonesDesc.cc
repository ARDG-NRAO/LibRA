//# MJonesDesc.cc: Implementation of MJonesDesc.h
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

#include <synthesis/CalTables/MJonesDesc.h>
#include <msvis/MSVis/MSCalEnums.h>
#include <casacore/tables/Tables/ScaColDesc.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

//----------------------------------------------------------------------------

MJonesDesc::MJonesDesc() : CalTableDesc(), itsAnt2Desc()
{
// Default null constructor for the MJones cal table description (v2.0)
// Output to private data:
//    itsAnt2Desc     TableDesc    Table descriptor for second ant/feed index
//
  // Use the default second antenna index descriptor
  itsAnt2Desc.add (defaultAnt2Desc());
};

//----------------------------------------------------------------------------

TableDesc MJonesDesc::calMainDesc()
{
// Return the cal_main descriptor
// Output:
//    calMainDesc     TableDesc    cal_main table descriptor
//
  // Insert the second antenna/feed descriptor after the "FEED1"
  // column in the CalTable descriptor.
  TableDesc tdout (insertDesc (CalTableDesc::calMainDesc(), itsAnt2Desc,
			       "FEED1"));
  return tdout;
};

//----------------------------------------------------------------------------

TableDesc MJonesDesc::defaultAnt2Desc()
{
// Generate the default descriptor for the second antenna/feed index
// Output:
//    defaultAnt2Desc    TableDesc    Default second ant/feed descriptor
//
  TableDesc td;
  td.addColumn (ScalarColumnDesc <Int> (MSC::fieldName (MSC::ANTENNA2),
					ColumnDesc::Direct));
  td.addColumn (ScalarColumnDesc <Int> (MSC::fieldName (MSC::FEED2),
					ColumnDesc::Direct));
  return td;
};

//----------------------------------------------------------------------------

} //# NAMESPACE CASA - END

