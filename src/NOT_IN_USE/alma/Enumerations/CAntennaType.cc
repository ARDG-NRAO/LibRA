
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
 * File CAntennaType.cpp
 */
#include <sstream>
#include <alma/Enumerations/CAntennaType.h>
#include <string>
using namespace std;

int CAntennaType::version() {
	return AntennaTypeMod::version;
	}
	
string CAntennaType::revision () {
	return AntennaTypeMod::revision;
}

unsigned int CAntennaType::size() {
	return 3;
	}
	
	
const std::string& CAntennaType::sGROUND_BASED = "GROUND_BASED";
	
const std::string& CAntennaType::sSPACE_BASED = "SPACE_BASED";
	
const std::string& CAntennaType::sTRACKING_STN = "TRACKING_STN";
	
const std::vector<std::string> CAntennaType::names() {
    std::vector<std::string> enumSet;
    
    enumSet.insert(enumSet.end(), CAntennaType::sGROUND_BASED);
    
    enumSet.insert(enumSet.end(), CAntennaType::sSPACE_BASED);
    
    enumSet.insert(enumSet.end(), CAntennaType::sTRACKING_STN);
        
    return enumSet;
}

std::string CAntennaType::name(const AntennaTypeMod::AntennaType& f) {
    switch (f) {
    
    case AntennaTypeMod::GROUND_BASED:
      return CAntennaType::sGROUND_BASED;
    
    case AntennaTypeMod::SPACE_BASED:
      return CAntennaType::sSPACE_BASED;
    
    case AntennaTypeMod::TRACKING_STN:
      return CAntennaType::sTRACKING_STN;
    	
    }
    // Impossible siutation but....who knows with C++ enums
    throw badInt((int) f);
}

AntennaTypeMod::AntennaType CAntennaType::newAntennaType(const std::string& name) {
		
    if (name == CAntennaType::sGROUND_BASED) {
        return AntennaTypeMod::GROUND_BASED;
    }
    	
    if (name == CAntennaType::sSPACE_BASED) {
        return AntennaTypeMod::SPACE_BASED;
    }
    	
    if (name == CAntennaType::sTRACKING_STN) {
        return AntennaTypeMod::TRACKING_STN;
    }
    
    throw badString(name);
}

AntennaTypeMod::AntennaType CAntennaType::literal(const std::string& name) {
		
    if (name == CAntennaType::sGROUND_BASED) {
        return AntennaTypeMod::GROUND_BASED;
    }
    	
    if (name == CAntennaType::sSPACE_BASED) {
        return AntennaTypeMod::SPACE_BASED;
    }
    	
    if (name == CAntennaType::sTRACKING_STN) {
        return AntennaTypeMod::TRACKING_STN;
    }
    
    throw badString(name);
}

AntennaTypeMod::AntennaType CAntennaType::from_int(unsigned int i) {
	vector<string> names_ = names();
	if (i >= names_.size()) throw badInt(i);
	return newAntennaType(names_.at(i));
}

string CAntennaType::badString(const string& name) {
	return "'"+name+"' does not correspond to any literal in the enumeration 'AntennaType'.";
}

string CAntennaType::badInt(unsigned int i) {
	ostringstream oss ;
	oss << "'" << i << "' is out of range for the enumeration 'AntennaType'.";
	return oss.str();
}

namespace AntennaTypeMod {
	std::ostream & operator << ( std::ostream & out, const AntennaType& value) {
		out << CAntennaType::name(value);
		return out;
	}

	std::istream & operator >> ( std::istream & in , AntennaType& value ) {
		in.clear();
		string s ; 
  		in >> s;
  		try {
    		value = CAntennaType::literal(s);
  		}
  		catch (string & m) {
    		in.setstate(ios::failbit);
  		}
  		return in;
  	}
}

