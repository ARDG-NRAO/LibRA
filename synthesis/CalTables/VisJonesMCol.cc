//# VisJonesMCol.cc: Implementation of VisJonesMCol.h
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

#include <synthesis/CalTables/VisJonesMCol.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

//----------------------------------------------------------------------------

ROVisJonesMCol::ROVisJonesMCol (const CalTable& calTable) :
  ROCalMainColumns (calTable)
{
// Construct from a calibration table
// Input:
//    calTable          const CalTable&           Calibration table
// Output to private data:
//    ROCalMainColumns  ROCalMainColumns          Read-only cal main col
};

//----------------------------------------------------------------------------

VisJonesMCol::VisJonesMCol (CalTable& calTable) :
  CalMainColumns (calTable)
{
// Construct from a calibration table
// Input:
//    calTable         CalTable&                  Calibration table
// Output to private data:
//    CalMainColumns   CalMainColumns             Read-write cal main col
};

//----------------------------------------------------------------------------


} //# NAMESPACE CASA - END

