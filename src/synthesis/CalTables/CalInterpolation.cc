//# CalInterpolation.cc: Implementation of CalInterpolation.h
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

#include <synthesis/CalTables/CalInterpolation.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

//----------------------------------------------------------------------------

CalInterpolation::CalInterpolation()
{
// Null constructor
// Output to private data:
//    axes_p             Vector<MSCalEnums::colDef>     Axes
//    type_p             CalInterpolation::Type         Type
//    weighting_p        CalInterpolation::Weighting    Weighting
//    windows_p          Vector<Quantity>               Windows
//    npoly_p            Int                            Polynomial order
//    msIndex_p          Vector<MSSelection>            MS match indices
//    calIndex_p         Vector<MSSelection>            Cal match indices
//
};

//----------------------------------------------------------------------------

CalInterpolation::~CalInterpolation()
{
// Destructor
//
};

//----------------------------------------------------------------------------

  CalInterpolation::CalInterpolation (const CalInterpolation& /*other*/)
{
// Copy constructor
// Input:
//    other    const CalInterpolation&       Existing CalInterpolation object
//
};

//----------------------------------------------------------------------------

CalInterpolation& CalInterpolation::operator= (const CalInterpolation& other)
{
// Assignment operator
// Input:
//    other    const CalInterpolation&       RHS CalInterpolation object
// Output to private data:
//    itsMainTable     Table*                Ptr to cal_main Table object
//    itsDescTable     Table*                Ptr to cal_desc Table object
//    itsHistoryTable  Table*                Ptr to cal_history Table object
//
  Bool identity = (this == &other);
  if (!identity) {};
  return *this;
};

//----------------------------------------------------------------------------

} //# NAMESPACE CASA - END

