//# EPointDesc.cc: Implementation of EPointDesc.h
//# Copyright (C) 1996,1997,1998,1999,2000,2001,2003
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

#include <synthesis/CalTables/EPointDesc.h>
#include <synthesis/CalTables/SolvableVJDesc.h>
#include <msvis/MSVis/MSCalEnums.h>
#include <tables/Tables/ArrColDesc.h>

using namespace casacore;
namespace casa{

EPointDesc::EPointDesc() :  SolvableVisJonesDesc("EPoint Jones")
  , itsTableDesc()
{
  itsTableDesc.add(SolvableVisJonesDesc::calMainDesc());
  //  addCols(itsTableDesc);
};

EPointDesc::EPointDesc (const String& type) : 
  SolvableVisJonesDesc (type), itsTableDesc()
{
// Constructor for time-variable VisJones calibration table description (v2.0)
// Inputs:
//    type         const String&    Cal table type (eg. "P Jones")
// Output to private data:
//    itsFitDesc   TableDesc        Table descriptor for fit parameters
//
  // Default fit statistics and weights 
  //  itsFitDesc.add (defaultFitDesc());
  itsTableDesc.add(SolvableVisJonesDesc::calMainDesc());
  //  addCols(itsTableDesc);
  //  cerr << "EPointDesc::EPointDesc()" << endl;
};
//----------------------------------------------------------------------------

TableDesc EPointDesc::calMainDesc()
{
  return addCols(itsTableDesc);
};

TableDesc EPointDesc::addCols(TableDesc& td)
{
  //  TableDesc td;

  // GJones descriptor
  //  td.add (SolvableVisJonesDesc::calMainDesc());

  //  Add POINTING_OFFSET column
  //  cerr << "EPointDesc::addCols() " << MSC::fieldName(MSC::POINTING_OFFSET) << endl;
  td.addColumn (ArrayColumnDesc<Float>
		(MSC::fieldName (MSC::POINTING_OFFSET)));
  return td;
}
}
