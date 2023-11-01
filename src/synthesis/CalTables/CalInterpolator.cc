//# CalInterpolator.cc: Implementation of CalInterpolator.h
//# Copyright (C) 1996,1997,1998,2000,2001,2002,2003
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

#include <synthesis/CalTables/CalInterpolator.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

//----------------------------------------------------------------------------

CalInterpolator::CalInterpolator (const CalInterpolation& calInterp) :
  calInterp_p(calInterp),
  vb_p(NULL)
{
// Construct from a set of calibration interpolation parameters
// Input:
//    calInterp         const CalInterpolation&     Calibration interpolation
//                                                  parameters
// Output to private data:
//    calInterp_p       CalInterpolation&           Reference to calibration
//                                                  interpolation parameters
//    vb_p              VisBuffer*                  Pointer to the current
//                                                  visibility buffer
//
  // Null constructor
};

//----------------------------------------------------------------------------


} //# NAMESPACE CASA - END

