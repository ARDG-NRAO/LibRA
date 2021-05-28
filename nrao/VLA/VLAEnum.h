//# VLAEnum.h: Enumerators common to the VLA Module
//# Copyright (C) 1999,2003
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
//#
//# $Id$

#ifndef NRAO_VLAENUM_H
#define NRAO_VLAENUM_H

#include <casa/aips.h>
#include <casa/namespace.h>
namespace casacore{

class String;
}

namespace casa { //# NAMESPACE CASA - BEGIN
} //# NAMESPACE CASA - END


// <summary>Enumerators common to the VLA module</summary>

// <use visibility=export>

// <reviewed reviewer="" date="yyyy/mm/dd" tests="tComponentEnum" demos="">
// </reviewed>

// <etymology>
// This class contains the enumerator definitions used in the VLA  module 
// </etymology>

// <synopsis> 
// This class does nothing.  It is merely a container for the enumerations used
// by the VLA module.  These enumerations define the standard values that
// enforce, at compile time, a stricter type checking than would be achieved by
// using integers.
// </synopsis>

// <example>
// </example>

// <todo asof="1999/08/18">
// <li> Nothing I can think of.
// </todo>
//

class VLAEnum {
public:
  // Names for all the IF's
  enum IF {
    // IF A is at freq 1 and normally is right-hand polarised
    IFA = 0,
    // IF B is at freq 2 and normally is right-hand polarised
    IFB,
    // IF C is at freq 1 and normally is left-hand polarised 
    IFC,
    // IF D is at freq 2 and normally is left-hand polarised 
    IFD
  };

  // Names for the different correlator data areas's
  enum CDA {
    // IF A is at freq 1 and normally is right-hand polarised
    CDA0 = 0,
    // IF B is at freq 2 and normally is right-hand polarised
    CDA1,
    // IF C is at freq 1 and normally is left-hand polarised 
    CDA2,
    // IF D is at freq 2 and normally is left-hand polarised 
    CDA3
  };

  // A list of the different correlator Modes. 
  // <note role=warning> 
  // Because enumerators cannot start with a numerical value the string
  // representation of the enumerator is different from the enumerator token.
  // </note>
  enum CorrMode {
    // Continuum mode. Four correlation products, at two spectral id's, using
    // IF's A,B,C and D. All other modes are spectral line.
    CONTINUUM = 0,
    // Spectral line mode with one correlation product, at one spectral id,
    // using IF A
    A,
    // Spectral line mode with one correlation product, at one spectral id,
    // using IF B
    B,
    // Spectral line mode with one correlation product, at one spectral id,
    // using IF C
    C,
    // Spectral line mode with one correlation product, at one spectral id,
    // using IF D
    D,
    // Spectral line mode with two correlation products, at two spectral id's,
    // using IF's A and B
    AB,
    // Spectral line mode with two correlation products, at one spectral id,
    // using IF's A and C
    AC,
    // Spectral line mode with two correlation products, at two spectral id's,
    // using IF's A and D
    AD,
    // Spectral line mode with two correlation products, at two spectral id's,
    // using IF's B and C
    BC,
    // Spectral line mode with two correlation products, at one spectral id,
    // using IF's B and D
    BD,
    // Spectral line mode with two correlation products, at two spectral id's,
    // using IF's C and D
    CD,
    // Spectral line mode with four correlation products, at two spectral id's,
    // using IF's A,B,C and D
    ABCD,
    // Spectral line mode with four correlation products, at one spectral id,
    // using IF's A and C
    PA,
    // Spectral line mode with four correlation products, at one spectral id,
    // using IF's B and D
    PB,
    OLDA,
    OLDB,
    OLDC,
    OLDD,
    // The number of valid modes in this enumerator
    NUMBER_MODES,
    // An unknown correlator mode
    UNKNOWN_MODE
  };

  // Convert the CorrMode enumerator to a string
  static casacore::String name(VLAEnum::CorrMode modeEnum);

  // Convert a given casacore::String to a CorrMode enumerator
  static VLAEnum::CorrMode corrMode(const casacore::String& modeString);
};
#endif
