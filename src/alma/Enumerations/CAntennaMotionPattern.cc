
/*
 * ALMA - Atacama Large Millimeter Array
 * (c) European Southern Observatory, 2002
 * (c) Associated Universities Inc., 2002
 * Copyright by ESO (in the framework of the ALMA collaboration),
 * Copyright by AUI (in the framework of the ALMA collaboration),
 * All rights reserved.
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY, without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307  USA
 * 
 * /////////////////////////////////////////////////////////////////
 * // WARNING!  DO NOT MODIFY THIS FILE!                          //
 * //  ---------------------------------------------------------  //
 * // | This is generated code!  Do not modify this file.       | //
 * // | Any changes will be lost when the file is re-generated. | //
 * //  ---------------------------------------------------------  //
 * /////////////////////////////////////////////////////////////////
 *
 * File CAntennaMotionPattern.cpp
 */
#include <sstream>
#include <alma/Enumerations/CAntennaMotionPattern.h>
#include <string>
using namespace std;

int CAntennaMotionPattern::version() {
	return AntennaMotionPatternMod::version;
	}
	
string CAntennaMotionPattern::revision () {
	return AntennaMotionPatternMod::revision;
}

unsigned int CAntennaMotionPattern::size() {
	return 11;
	}
	
	
const std::string& CAntennaMotionPattern::sNONE = "NONE";
	
const std::string& CAntennaMotionPattern::sCROSS_SCAN = "CROSS_SCAN";
	
const std::string& CAntennaMotionPattern::sSPIRAL = "SPIRAL";
	
const std::string& CAntennaMotionPattern::sCIRCLE = "CIRCLE";
	
const std::string& CAntennaMotionPattern::sTHREE_POINTS = "THREE_POINTS";
	
const std::string& CAntennaMotionPattern::sFOUR_POINTS = "FOUR_POINTS";
	
const std::string& CAntennaMotionPattern::sFIVE_POINTS = "FIVE_POINTS";
	
const std::string& CAntennaMotionPattern::sTEST = "TEST";
	
const std::string& CAntennaMotionPattern::sUNSPECIFIED = "UNSPECIFIED";
	
const std::string& CAntennaMotionPattern::sSTAR = "STAR";
	
const std::string& CAntennaMotionPattern::sLISSAJOUS = "LISSAJOUS";
	
const std::vector<std::string> CAntennaMotionPattern::names() {
    std::vector<std::string> enumSet;
    
    enumSet.insert(enumSet.end(), CAntennaMotionPattern::sNONE);
    
    enumSet.insert(enumSet.end(), CAntennaMotionPattern::sCROSS_SCAN);
    
    enumSet.insert(enumSet.end(), CAntennaMotionPattern::sSPIRAL);
    
    enumSet.insert(enumSet.end(), CAntennaMotionPattern::sCIRCLE);
    
    enumSet.insert(enumSet.end(), CAntennaMotionPattern::sTHREE_POINTS);
    
    enumSet.insert(enumSet.end(), CAntennaMotionPattern::sFOUR_POINTS);
    
    enumSet.insert(enumSet.end(), CAntennaMotionPattern::sFIVE_POINTS);
    
    enumSet.insert(enumSet.end(), CAntennaMotionPattern::sTEST);
    
    enumSet.insert(enumSet.end(), CAntennaMotionPattern::sUNSPECIFIED);
    
    enumSet.insert(enumSet.end(), CAntennaMotionPattern::sSTAR);
    
    enumSet.insert(enumSet.end(), CAntennaMotionPattern::sLISSAJOUS);
        
    return enumSet;
}

std::string CAntennaMotionPattern::name(const AntennaMotionPatternMod::AntennaMotionPattern& f) {
    switch (f) {
    
    case AntennaMotionPatternMod::NONE:
      return CAntennaMotionPattern::sNONE;
    
    case AntennaMotionPatternMod::CROSS_SCAN:
      return CAntennaMotionPattern::sCROSS_SCAN;
    
    case AntennaMotionPatternMod::SPIRAL:
      return CAntennaMotionPattern::sSPIRAL;
    
    case AntennaMotionPatternMod::CIRCLE:
      return CAntennaMotionPattern::sCIRCLE;
    
    case AntennaMotionPatternMod::THREE_POINTS:
      return CAntennaMotionPattern::sTHREE_POINTS;
    
    case AntennaMotionPatternMod::FOUR_POINTS:
      return CAntennaMotionPattern::sFOUR_POINTS;
    
    case AntennaMotionPatternMod::FIVE_POINTS:
      return CAntennaMotionPattern::sFIVE_POINTS;
    
    case AntennaMotionPatternMod::TEST:
      return CAntennaMotionPattern::sTEST;
    
    case AntennaMotionPatternMod::UNSPECIFIED:
      return CAntennaMotionPattern::sUNSPECIFIED;
    
    case AntennaMotionPatternMod::STAR:
      return CAntennaMotionPattern::sSTAR;
    
    case AntennaMotionPatternMod::LISSAJOUS:
      return CAntennaMotionPattern::sLISSAJOUS;
    	
    }
    // Impossible siutation but....who knows with C++ enums
    throw badInt((int) f);
}

AntennaMotionPatternMod::AntennaMotionPattern CAntennaMotionPattern::newAntennaMotionPattern(const std::string& name) {
		
    if (name == CAntennaMotionPattern::sNONE) {
        return AntennaMotionPatternMod::NONE;
    }
    	
    if (name == CAntennaMotionPattern::sCROSS_SCAN) {
        return AntennaMotionPatternMod::CROSS_SCAN;
    }
    	
    if (name == CAntennaMotionPattern::sSPIRAL) {
        return AntennaMotionPatternMod::SPIRAL;
    }
    	
    if (name == CAntennaMotionPattern::sCIRCLE) {
        return AntennaMotionPatternMod::CIRCLE;
    }
    	
    if (name == CAntennaMotionPattern::sTHREE_POINTS) {
        return AntennaMotionPatternMod::THREE_POINTS;
    }
    	
    if (name == CAntennaMotionPattern::sFOUR_POINTS) {
        return AntennaMotionPatternMod::FOUR_POINTS;
    }
    	
    if (name == CAntennaMotionPattern::sFIVE_POINTS) {
        return AntennaMotionPatternMod::FIVE_POINTS;
    }
    	
    if (name == CAntennaMotionPattern::sTEST) {
        return AntennaMotionPatternMod::TEST;
    }
    	
    if (name == CAntennaMotionPattern::sUNSPECIFIED) {
        return AntennaMotionPatternMod::UNSPECIFIED;
    }
    	
    if (name == CAntennaMotionPattern::sSTAR) {
        return AntennaMotionPatternMod::STAR;
    }
    	
    if (name == CAntennaMotionPattern::sLISSAJOUS) {
        return AntennaMotionPatternMod::LISSAJOUS;
    }
    
    throw badString(name);
}

AntennaMotionPatternMod::AntennaMotionPattern CAntennaMotionPattern::literal(const std::string& name) {
		
    if (name == CAntennaMotionPattern::sNONE) {
        return AntennaMotionPatternMod::NONE;
    }
    	
    if (name == CAntennaMotionPattern::sCROSS_SCAN) {
        return AntennaMotionPatternMod::CROSS_SCAN;
    }
    	
    if (name == CAntennaMotionPattern::sSPIRAL) {
        return AntennaMotionPatternMod::SPIRAL;
    }
    	
    if (name == CAntennaMotionPattern::sCIRCLE) {
        return AntennaMotionPatternMod::CIRCLE;
    }
    	
    if (name == CAntennaMotionPattern::sTHREE_POINTS) {
        return AntennaMotionPatternMod::THREE_POINTS;
    }
    	
    if (name == CAntennaMotionPattern::sFOUR_POINTS) {
        return AntennaMotionPatternMod::FOUR_POINTS;
    }
    	
    if (name == CAntennaMotionPattern::sFIVE_POINTS) {
        return AntennaMotionPatternMod::FIVE_POINTS;
    }
    	
    if (name == CAntennaMotionPattern::sTEST) {
        return AntennaMotionPatternMod::TEST;
    }
    	
    if (name == CAntennaMotionPattern::sUNSPECIFIED) {
        return AntennaMotionPatternMod::UNSPECIFIED;
    }
    	
    if (name == CAntennaMotionPattern::sSTAR) {
        return AntennaMotionPatternMod::STAR;
    }
    	
    if (name == CAntennaMotionPattern::sLISSAJOUS) {
        return AntennaMotionPatternMod::LISSAJOUS;
    }
    
    throw badString(name);
}

AntennaMotionPatternMod::AntennaMotionPattern CAntennaMotionPattern::from_int(unsigned int i) {
	vector<string> names_ = names();
	if (i >= names_.size()) throw badInt(i);
	return newAntennaMotionPattern(names_.at(i));
}

string CAntennaMotionPattern::badString(const string& name) {
	return "'"+name+"' does not correspond to any literal in the enumeration 'AntennaMotionPattern'.";
}

string CAntennaMotionPattern::badInt(unsigned int i) {
	ostringstream oss ;
	oss << "'" << i << "' is out of range for the enumeration 'AntennaMotionPattern'.";
	return oss.str();
}

namespace AntennaMotionPatternMod {
	std::ostream & operator << ( std::ostream & out, const AntennaMotionPattern& value) {
		out << CAntennaMotionPattern::name(value);
		return out;
	}

	std::istream & operator >> ( std::istream & in , AntennaMotionPattern& value ) {
		in.clear();
		string s ; 
  		in >> s;
  		try {
    		value = CAntennaMotionPattern::literal(s);
  		}
  		catch (string & m) {
    		in.setstate(ios::failbit);
  		}
  		return in;
  	}
}

