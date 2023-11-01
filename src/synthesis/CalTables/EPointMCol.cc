//# EPointJonesMCol.cc: Implementation of EPointJonesMCol.h
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

#include <synthesis/CalTables/EPointMCol.h>
using namespace casacore;
namespace casa {
//----------------------------------------------------------------------------

ROEPointMCol::ROEPointMCol (const EPointTable& epTable) :
  ROSolvableVisJonesMCol (epTable)
{
  // Construct from a calibration table
  // Input:
  //    svjTable              const SolvableVisJonesTable&  SVJ calibration table
  // Output to private data:
  //    ROTimeVarVisJonesMCol ROTimeVarVisJonesMCol         Read-only TVVJ 
  //                                                        cal main col
  // Attach all column accessors for additional SolvableVisJones columns
  // (all of which are required columns)
  attach (epTable, pointingOffset_p, MSC::POINTING_OFFSET, false);
  //  cerr << "ROEPointMCol::ROEPointMCol: " 
  //       << asTable(epTable).tableDesc().isColumn(MSC::fieldName(MSC::POINTING_OFFSET))
  //       << endl;
};

//----------------------------------------------------------------------------

EPointMCol::EPointMCol (EPointTable& epTable) :
  SolvableVisJonesMCol (epTable)
{
  // Construct from a calibration table
  // Input:
  //    svjTable             SolvableVisJonesTable&      SVJ calibration table
  // Output to private data:
  //    TimeVarVisJonesMCol  TimeVarVisJonesMCol         Read-write TVVJ 
  //                                                     cal main col
  // Attach all column accessors for additional SolvableVisJones columns
  // (all of which are required columns)
  attach (epTable, pointingOffset_p, MSC::POINTING_OFFSET, false);
  //  cerr << "EPointMCol::EPointMCol: " 
  //       << asTable(epTable).tableDesc().isColumn(MSC::fieldName(MSC::POINTING_OFFSET))
  //       << endl;
};

//----------------------------------------------------------------------------

}
