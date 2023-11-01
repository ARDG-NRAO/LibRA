//# VisJonesMBuf.cc: Implementation of VisJonesMBuf.h
//# Copyright (C) 1996,1997,1998,2001,2002,2003
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

#include <synthesis/CalTables/VisJonesMBuf.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

//----------------------------------------------------------------------------

VisJonesMBuf::VisJonesMBuf() : CalMainBuffer()
{
// Null constructor
};

//----------------------------------------------------------------------------

VisJonesMBuf::VisJonesMBuf (const Vector<Int>& calIndices,
			    const Block<Vector<Int> >& indexValues) : 
  CalMainBuffer (calIndices, indexValues)
{
// Construct from a set of cal buffer indices and specified index values
// Output to private data:
//    CalMainBuffer    CalMainBuffer       Parent class cal main table buffer
};

//----------------------------------------------------------------------------

VisJonesMBuf::VisJonesMBuf (CalIterBase& calIter) : CalMainBuffer (calIter)
{
// Construct from a calibration table iterator
// Input:
//    calIter            CalIterBase&         Calibration table iterator
};

//----------------------------------------------------------------------------



} //# NAMESPACE CASA - END

