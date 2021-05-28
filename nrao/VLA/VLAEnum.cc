//# VLAEnum.cc:
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
//# $Id$

#include <nrao/VLA/VLAEnum.h>
#include <casa/BasicSL/String.h>

String VLAEnum::name(VLAEnum::CorrMode modeEnum) {
  switch (modeEnum) {
  case VLAEnum::CONTINUUM: 
    return " ";
  case VLAEnum::A: 
    return "1A";
  case VLAEnum::B: 
    return "1B";
  case VLAEnum::C: 
    return "1C";
  case VLAEnum::D: 
    return "1D";
  case VLAEnum::AB: 
    return "2AB";
  case VLAEnum::AC: 
    return "2AC";
  case VLAEnum::AD: 
    return "2AD";
  case VLAEnum::BC: 
    return "2BC";
  case VLAEnum::BD: 
    return "2BD";
  case VLAEnum::CD: 
    return "2CD";
  case VLAEnum::ABCD: 
    return "4";
  case VLAEnum::PA: 
    return "PA";
  case VLAEnum::PB: 
    return "PB";
  case VLAEnum::OLDA: 
    return "2A";
  case VLAEnum::OLDB: 
    return "2B";
  case VLAEnum::OLDC: 
    return "2C";
  case VLAEnum::OLDD: 
    return "2D";
  default:
    return "Unknown correlator mode";
  };
}

VLAEnum::CorrMode VLAEnum::corrMode(const String& modeString) {
  String canonicalCase(modeString);
  canonicalCase.upcase();
  VLAEnum::CorrMode m;
  for (uInt i = 0; i < NUMBER_MODES; i++) {
    m = VLAEnum::CorrMode(i);
    if (canonicalCase.matches(VLAEnum::name(m))) {
	    switch(m) {
	       case VLAEnum::OLDA:
	            m = VLAEnum::AD;
		    break;
	       case VLAEnum::OLDB:
	            m = VLAEnum::BD;
		    break;
	       case VLAEnum::OLDC:
	            m = VLAEnum::CD;
		    break;
	       case VLAEnum::OLDD:
	            m = VLAEnum::AD;
		    break;
	       default :
		    break;
	    }
      return m;
    }
  }
  return VLAEnum::UNKNOWN_MODE;
}

// Local Variables: 
// compile-command: "gmake VLAEnum"
// End: 
