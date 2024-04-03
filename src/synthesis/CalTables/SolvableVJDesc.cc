//# SolvableVJDesc.cc: Implementation of SolvableVJDesc.h
//# Copyright (C) 1996,1997,1998,2000,2001,2003
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

#include <synthesis/CalTables/SolvableVJDesc.h>
#include <msvis/MSVis/MSCalEnums.h>
#include <casacore/tables/Tables/ArrColDesc.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

//----------------------------------------------------------------------------

SolvableVisJonesDesc::SolvableVisJonesDesc() : TimeVarVisJonesDesc(),
  itsFitDesc()
{
// Default null constructor for solvable VisJones calibration 
// table description (v2.0)
// Output to private data:
//    itsFitDesc      TableDesc      Table descriptor for fit parameters
//
  // Default fit statistics and weights 
  itsFitDesc.add (defaultFitDesc());
};

//----------------------------------------------------------------------------

SolvableVisJonesDesc::SolvableVisJonesDesc (const String& type) : 
  TimeVarVisJonesDesc (type), itsFitDesc()
{
// Constructor for time-variable VisJones calibration table description (v2.0)
// Inputs:
//    type         const String&    Cal table type (eg. "P Jones")
// Output to private data:
//    itsFitDesc   TableDesc        Table descriptor for fit parameters
//
  // Default fit statistics and weights 
  itsFitDesc.add (defaultFitDesc());
};

//----------------------------------------------------------------------------

TableDesc SolvableVisJonesDesc::calMainDesc()
{
// Return the cal_main table descriptor
// Output:
//    calMainDesc        TableDesc       cal_main table descriptor
//
  // TimeVarVisJones descriptor
  TableDesc td (TimeVarVisJonesDesc::calMainDesc());
  
  // Add fit statistics and weights
  td.add (itsFitDesc);

  return td;
};

//----------------------------------------------------------------------------

GJonesDesc::GJonesDesc() : SolvableVisJonesDesc("G Jones")
{
// Default null constructor for GJones calibration table description (v2.0)
};

//----------------------------------------------------------------------------

DJonesDesc::DJonesDesc() : SolvableVisJonesDesc("D Jones")
{
// Default null constructor for DJones calibration table description (v2.0)
};

//----------------------------------------------------------------------------

TJonesDesc::TJonesDesc() : SolvableVisJonesDesc("T Jones")
{
// Default null constructor for TJones calibration table description (v2.0)
};

//----------------------------------------------------------------------------

GJonesDelayRateSBDesc::GJonesDelayRateSBDesc() : GJonesDesc()
{
};

//----------------------------------------------------------------------------

TableDesc GJonesDelayRateSBDesc::calMainDesc()
{
// Return the cal_main table descriptor
// Output:
//    calMainDesc    TableDesc cal_main      table descriptor
  TableDesc td;

  // GJones descriptor
  td.add (GJonesDesc::calMainDesc());

  //  Add PHASE_OFFSET, SB_DELAY, DELAY_RATE columns:
  td.addColumn (ArrayColumnDesc <Float> 
                (MSC::fieldName (MSC::PHASE_OFFSET)));
  td.addColumn (ArrayColumnDesc <Float> 
                (MSC::fieldName (MSC::SB_DELAY)));
  td.addColumn (ArrayColumnDesc <Float> 
                (MSC::fieldName (MSC::DELAY_RATE)));

  return td;
};







} //# NAMESPACE CASA - END

