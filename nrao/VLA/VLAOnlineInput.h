//# VLAOnlineInput.h: This class reads VLA data from the online computers
//# Copyright (C) 2001
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
//# $Id: VLAOnlineInput.h,v 19.3.14.2 2006/02/17 23:46:40 wyoung Exp $

#ifndef NRAO_VLAONLINEINPUT_H
#define NRAO_VLAONLINEINPUT_H

#include <casa/aips.h>
#include <casa/IO/StreamIO.h>
#include <casa/BasicSL/String.h>
#include <nrao/VLA/VLAArchiveInput.h>

#include <casa/namespace.h>
namespace casacore{

class Path;
}

namespace casa { //# NAMESPACE CASA - BEGIN
} //# NAMESPACE CASA - END


// <summary>This class reads VLA archive records from the online computers</summary>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
// <ol>
//   <li> The IO Module
// </ol>
// </prerequisite>
//
// <etymology>
// This class is designed to reads VLA archive records from a Disk file
// </etymology>
//
// <synopsis> 

// </synopsis> 
//
// <example>
// </example>
//
// <motivation>
// </motivation>
//
// <todo asof="">
// </todo>

class VLAOnlineInput: public VLAArchiveInput
{
public: 

  // Create an object that reads its data from the vis-server disk
  // An exception is thrown if a connection cannot be made.
  VLAOnlineInput(casacore::String &online, casacore::Int afiles=0);
    
  // The destructor closes the file.
  virtual ~VLAOnlineInput();

  // Reads the next logical record from specified IO source. Returns false if
  // there was a problem assembling the next record ie., it returns the value
  // of the hasData() member function.
  virtual casacore::Bool read();

private: 
  //# The copy constructor is private and undefined
  VLAOnlineInput(const VLAOnlineInput& other);

  //# The assignment operator is private and undefined
  VLAOnlineInput& operator=(const VLAOnlineInput& other);

  casacore::Bool   onlineFill;
  casacore::String visDir;
  casacore::Int oldDay;
  casacore::Int previousDay;

  casacore::String getTodaysFile(int relDay=0);
  casacore::Int whatsToday();

};

#endif
