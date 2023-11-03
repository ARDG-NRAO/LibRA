
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
 * File CCalDataOrigin.cpp
 */
#include <sstream>
#include <alma/Enumerations/CCalDataOrigin.h>
#include <string>
using namespace std;

int CCalDataOrigin::version() {
	return CalDataOriginMod::version;
	}
	
string CCalDataOrigin::revision () {
	return CalDataOriginMod::revision;
}

unsigned int CCalDataOrigin::size() {
	return 9;
	}
	
	
const std::string& CCalDataOrigin::sTOTAL_POWER = "TOTAL_POWER";
	
const std::string& CCalDataOrigin::sWVR = "WVR";
	
const std::string& CCalDataOrigin::sCHANNEL_AVERAGE_AUTO = "CHANNEL_AVERAGE_AUTO";
	
const std::string& CCalDataOrigin::sCHANNEL_AVERAGE_CROSS = "CHANNEL_AVERAGE_CROSS";
	
const std::string& CCalDataOrigin::sFULL_RESOLUTION_AUTO = "FULL_RESOLUTION_AUTO";
	
const std::string& CCalDataOrigin::sFULL_RESOLUTION_CROSS = "FULL_RESOLUTION_CROSS";
	
const std::string& CCalDataOrigin::sOPTICAL_POINTING = "OPTICAL_POINTING";
	
const std::string& CCalDataOrigin::sHOLOGRAPHY = "HOLOGRAPHY";
	
const std::string& CCalDataOrigin::sNONE = "NONE";
	
const std::vector<std::string> CCalDataOrigin::names() {
    std::vector<std::string> enumSet;
    
    enumSet.insert(enumSet.end(), CCalDataOrigin::sTOTAL_POWER);
    
    enumSet.insert(enumSet.end(), CCalDataOrigin::sWVR);
    
    enumSet.insert(enumSet.end(), CCalDataOrigin::sCHANNEL_AVERAGE_AUTO);
    
    enumSet.insert(enumSet.end(), CCalDataOrigin::sCHANNEL_AVERAGE_CROSS);
    
    enumSet.insert(enumSet.end(), CCalDataOrigin::sFULL_RESOLUTION_AUTO);
    
    enumSet.insert(enumSet.end(), CCalDataOrigin::sFULL_RESOLUTION_CROSS);
    
    enumSet.insert(enumSet.end(), CCalDataOrigin::sOPTICAL_POINTING);
    
    enumSet.insert(enumSet.end(), CCalDataOrigin::sHOLOGRAPHY);
    
    enumSet.insert(enumSet.end(), CCalDataOrigin::sNONE);
        
    return enumSet;
}

std::string CCalDataOrigin::name(const CalDataOriginMod::CalDataOrigin& f) {
    switch (f) {
    
    case CalDataOriginMod::TOTAL_POWER:
      return CCalDataOrigin::sTOTAL_POWER;
    
    case CalDataOriginMod::WVR:
      return CCalDataOrigin::sWVR;
    
    case CalDataOriginMod::CHANNEL_AVERAGE_AUTO:
      return CCalDataOrigin::sCHANNEL_AVERAGE_AUTO;
    
    case CalDataOriginMod::CHANNEL_AVERAGE_CROSS:
      return CCalDataOrigin::sCHANNEL_AVERAGE_CROSS;
    
    case CalDataOriginMod::FULL_RESOLUTION_AUTO:
      return CCalDataOrigin::sFULL_RESOLUTION_AUTO;
    
    case CalDataOriginMod::FULL_RESOLUTION_CROSS:
      return CCalDataOrigin::sFULL_RESOLUTION_CROSS;
    
    case CalDataOriginMod::OPTICAL_POINTING:
      return CCalDataOrigin::sOPTICAL_POINTING;
    
    case CalDataOriginMod::HOLOGRAPHY:
      return CCalDataOrigin::sHOLOGRAPHY;
    
    case CalDataOriginMod::NONE:
      return CCalDataOrigin::sNONE;
    	
    }
    // Impossible siutation but....who knows with C++ enums
    throw badInt((int) f);
}

CalDataOriginMod::CalDataOrigin CCalDataOrigin::newCalDataOrigin(const std::string& name) {
		
    if (name == CCalDataOrigin::sTOTAL_POWER) {
        return CalDataOriginMod::TOTAL_POWER;
    }
    	
    if (name == CCalDataOrigin::sWVR) {
        return CalDataOriginMod::WVR;
    }
    	
    if (name == CCalDataOrigin::sCHANNEL_AVERAGE_AUTO) {
        return CalDataOriginMod::CHANNEL_AVERAGE_AUTO;
    }
    	
    if (name == CCalDataOrigin::sCHANNEL_AVERAGE_CROSS) {
        return CalDataOriginMod::CHANNEL_AVERAGE_CROSS;
    }
    	
    if (name == CCalDataOrigin::sFULL_RESOLUTION_AUTO) {
        return CalDataOriginMod::FULL_RESOLUTION_AUTO;
    }
    	
    if (name == CCalDataOrigin::sFULL_RESOLUTION_CROSS) {
        return CalDataOriginMod::FULL_RESOLUTION_CROSS;
    }
    	
    if (name == CCalDataOrigin::sOPTICAL_POINTING) {
        return CalDataOriginMod::OPTICAL_POINTING;
    }
    	
    if (name == CCalDataOrigin::sHOLOGRAPHY) {
        return CalDataOriginMod::HOLOGRAPHY;
    }
    	
    if (name == CCalDataOrigin::sNONE) {
        return CalDataOriginMod::NONE;
    }
    
    throw badString(name);
}

CalDataOriginMod::CalDataOrigin CCalDataOrigin::literal(const std::string& name) {
		
    if (name == CCalDataOrigin::sTOTAL_POWER) {
        return CalDataOriginMod::TOTAL_POWER;
    }
    	
    if (name == CCalDataOrigin::sWVR) {
        return CalDataOriginMod::WVR;
    }
    	
    if (name == CCalDataOrigin::sCHANNEL_AVERAGE_AUTO) {
        return CalDataOriginMod::CHANNEL_AVERAGE_AUTO;
    }
    	
    if (name == CCalDataOrigin::sCHANNEL_AVERAGE_CROSS) {
        return CalDataOriginMod::CHANNEL_AVERAGE_CROSS;
    }
    	
    if (name == CCalDataOrigin::sFULL_RESOLUTION_AUTO) {
        return CalDataOriginMod::FULL_RESOLUTION_AUTO;
    }
    	
    if (name == CCalDataOrigin::sFULL_RESOLUTION_CROSS) {
        return CalDataOriginMod::FULL_RESOLUTION_CROSS;
    }
    	
    if (name == CCalDataOrigin::sOPTICAL_POINTING) {
        return CalDataOriginMod::OPTICAL_POINTING;
    }
    	
    if (name == CCalDataOrigin::sHOLOGRAPHY) {
        return CalDataOriginMod::HOLOGRAPHY;
    }
    	
    if (name == CCalDataOrigin::sNONE) {
        return CalDataOriginMod::NONE;
    }
    
    throw badString(name);
}

CalDataOriginMod::CalDataOrigin CCalDataOrigin::from_int(unsigned int i) {
	vector<string> names_ = names();
	if (i >= names_.size()) throw badInt(i);
	return newCalDataOrigin(names_.at(i));
}

string CCalDataOrigin::badString(const string& name) {
	return "'"+name+"' does not correspond to any literal in the enumeration 'CalDataOrigin'.";
}

string CCalDataOrigin::badInt(unsigned int i) {
	ostringstream oss ;
	oss << "'" << i << "' is out of range for the enumeration 'CalDataOrigin'.";
	return oss.str();
}

namespace CalDataOriginMod {
	std::ostream & operator << ( std::ostream & out, const CalDataOrigin& value) {
		out << CCalDataOrigin::name(value);
		return out;
	}

	std::istream & operator >> ( std::istream & in , CalDataOrigin& value ) {
		in.clear();
		string s ; 
  		in >> s;
  		try {
    		value = CCalDataOrigin::literal(s);
  		}
  		catch (string & m) {
    		in.setstate(ios::failbit);
  		}
  		return in;
  	}
}

