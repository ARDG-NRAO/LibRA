//# TimeVarVJMCol.cc: Implementation of TimeVarVJMCol.h
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

#include <synthesis/CalTables/TimeVarVJMCol.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

//----------------------------------------------------------------------------

ROTimeVarVisJonesMCol::ROTimeVarVisJonesMCol (const TimeVarVisJonesTable& 
					      tvvjTable) :
  ROVisJonesMCol (tvvjTable)
{
// Construct from a calibration table
// Input:
//    tvvjTable         const TimeVarVisJonesTable&  TVVJ calibration table
// Output to private data:
//    ROVisJonesMCol    ROVisJonesMCol               Read-only VJ cal main col
};

//----------------------------------------------------------------------------

TimeVarVisJonesMCol::TimeVarVisJonesMCol (TimeVarVisJonesTable& tvvjTable) :
  VisJonesMCol (tvvjTable)
{
// Construct from a calibration table
// Input:
//    tvvjTable        TimeVarVisJonesTable&      TVVJ calibration table
// Output to private data:
//    VisJonesMCol     VisJonesMCol               Read-write VJ cal main col
};

//----------------------------------------------------------------------------

ROPJonesMCol::ROPJonesMCol (const PJonesTable& pjTable) :
  ROTimeVarVisJonesMCol (pjTable)
{
// Construct from a calibration table
// Input:
//    pjTable                const PJonesTable&             PJones cal table
// Output to private data:
//    ROTimeVarVisJonesMCol  ROTimeVarVisJonesMCol          Read-only TVVJ cal 
//                                                          main col
};

//----------------------------------------------------------------------------

PJonesMCol::PJonesMCol (PJonesTable& pjTable) :
  TimeVarVisJonesMCol (pjTable)
{
// Construct from a calibration table
// Input:
//    pjTable             PJonesTable&               PJones calibration table
// Output to private data:
//    TimeVarVisJonesMCol TimeVarVisJonesMCol        Read-write TVVJ cal 
//                                                   main col
};

//----------------------------------------------------------------------------

ROCJonesMCol::ROCJonesMCol (const CJonesTable& cjTable) :
  ROTimeVarVisJonesMCol (cjTable)
{
// Construct from a calibration table
// Input:
//    cjTable                const CJonesTable&        CJones calibration table
// Output to private data:
//    ROTimeVarVisJonesMCol  ROTimeVarVisJonesMCol     Read-only TVVJ cal 
//                                                     main col
};

//----------------------------------------------------------------------------

CJonesMCol::CJonesMCol (CJonesTable& cjTable) :
  TimeVarVisJonesMCol (cjTable)
{
// Construct from a calibration table
// Input:
//    cjTable             CJonesTable&               CJones calibration table
// Output to private data:
//    TimeVarVisJonesMCol TimeVarVisJonesMCol        Read-write TVVJ cal 
//                                                   main col
};

//----------------------------------------------------------------------------



} //# NAMESPACE CASA - END

