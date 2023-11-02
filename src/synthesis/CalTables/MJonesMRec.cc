//# MJonesMRec.cc: Implementation of MJonesMRec.h
//# Copyright (C) 1996,1997,1998,2003
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

#include <synthesis/CalTables/MJonesMRec.h>
#include <msvis/MSVis/MSCalEnums.h> 

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

//----------------------------------------------------------------------------

MJonesMRec::MJonesMRec() : CalMainRecord()
{
// Null constructor
//
};

//----------------------------------------------------------------------------

MJonesMRec::MJonesMRec (const Record& inpRec) : CalMainRecord (inpRec)
{
// Construct from an input record
// Input:
//    inpRec           Record          Input record
//
};

//----------------------------------------------------------------------------

void MJonesMRec::defineAntenna2 (const Int& antenna2)
{
// Define the ANTENNA2 field value
// Input:
//    antenna2         const Int&      ANTENNA2 value
// Output to private data:
//    itsRecord        Record          Underlying record object
//
  Record newRec;
  newRec.define (MSC::fieldName (MSC::ANTENNA2), antenna2);
  addRec (newRec);
};

//----------------------------------------------------------------------------

void MJonesMRec::defineFeed2 (const Int& feed2)
{
// Define the FEED2 field value
// Input:
//    feed2            const Int&      FEED2 value
// Output to private data:
//    itsRecord        Record          Underlying record object
//
  Record newRec;
  newRec.define (MSC::fieldName (MSC::FEED2), feed2);
  addRec (newRec);
};

//----------------------------------------------------------------------------

void MJonesMRec::getAntenna2 (Int& antenna2)
{
// Get the ANTENNA2 field value
// Output:
//    antenna2         Int&            ANTENNA2 value
// Input from private data:
//    itsRecord        Record          Underlying record object
//
  record().get (MSC::fieldName (MSC::ANTENNA2), antenna2);
};

//----------------------------------------------------------------------------

void MJonesMRec::getFeed2 (Int& feed2)
{
// Get the FEED2 field value
// Output:
//    feed2            Int&            FEED2 value
// Input from private data:
//    itsRecord        Record          Underlying record object
//
  record().get (MSC::fieldName (MSC::FEED2), feed2);
};

//----------------------------------------------------------------------------


} //# NAMESPACE CASA - END

