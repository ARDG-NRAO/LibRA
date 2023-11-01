
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
 * File CSyscalMethod.cpp
 */
#include <sstream>
#include <alma/Enumerations/CSyscalMethod.h>
#include <string>
using namespace std;

int CSyscalMethod::version() {
	return SyscalMethodMod::version;
	}
	
string CSyscalMethod::revision () {
	return SyscalMethodMod::revision;
}

unsigned int CSyscalMethod::size() {
	return 3;
	}
	
	
const std::string& CSyscalMethod::sTEMPERATURE_SCALE = "TEMPERATURE_SCALE";
	
const std::string& CSyscalMethod::sSKYDIP = "SKYDIP";
	
const std::string& CSyscalMethod::sSIDEBAND_RATIO = "SIDEBAND_RATIO";
	
const std::vector<std::string> CSyscalMethod::names() {
    std::vector<std::string> enumSet;
    
    enumSet.insert(enumSet.end(), CSyscalMethod::sTEMPERATURE_SCALE);
    
    enumSet.insert(enumSet.end(), CSyscalMethod::sSKYDIP);
    
    enumSet.insert(enumSet.end(), CSyscalMethod::sSIDEBAND_RATIO);
        
    return enumSet;
}

std::string CSyscalMethod::name(const SyscalMethodMod::SyscalMethod& f) {
    switch (f) {
    
    case SyscalMethodMod::TEMPERATURE_SCALE:
      return CSyscalMethod::sTEMPERATURE_SCALE;
    
    case SyscalMethodMod::SKYDIP:
      return CSyscalMethod::sSKYDIP;
    
    case SyscalMethodMod::SIDEBAND_RATIO:
      return CSyscalMethod::sSIDEBAND_RATIO;
    	
    }
    // Impossible siutation but....who knows with C++ enums
    throw badInt((int) f);
}

SyscalMethodMod::SyscalMethod CSyscalMethod::newSyscalMethod(const std::string& name) {
		
    if (name == CSyscalMethod::sTEMPERATURE_SCALE) {
        return SyscalMethodMod::TEMPERATURE_SCALE;
    }
    	
    if (name == CSyscalMethod::sSKYDIP) {
        return SyscalMethodMod::SKYDIP;
    }
    	
    if (name == CSyscalMethod::sSIDEBAND_RATIO) {
        return SyscalMethodMod::SIDEBAND_RATIO;
    }
    
    throw badString(name);
}

SyscalMethodMod::SyscalMethod CSyscalMethod::literal(const std::string& name) {
		
    if (name == CSyscalMethod::sTEMPERATURE_SCALE) {
        return SyscalMethodMod::TEMPERATURE_SCALE;
    }
    	
    if (name == CSyscalMethod::sSKYDIP) {
        return SyscalMethodMod::SKYDIP;
    }
    	
    if (name == CSyscalMethod::sSIDEBAND_RATIO) {
        return SyscalMethodMod::SIDEBAND_RATIO;
    }
    
    throw badString(name);
}

SyscalMethodMod::SyscalMethod CSyscalMethod::from_int(unsigned int i) {
	vector<string> names_ = names();
	if (i >= names_.size()) throw badInt(i);
	return newSyscalMethod(names_.at(i));
}

string CSyscalMethod::badString(const string& name) {
	return "'"+name+"' does not correspond to any literal in the enumeration 'SyscalMethod'.";
}

string CSyscalMethod::badInt(unsigned int i) {
	ostringstream oss ;
	oss << "'" << i << "' is out of range for the enumeration 'SyscalMethod'.";
	return oss.str();
}

namespace SyscalMethodMod {
	std::ostream & operator << ( std::ostream & out, const SyscalMethod& value) {
		out << CSyscalMethod::name(value);
		return out;
	}

	std::istream & operator >> ( std::istream & in , SyscalMethod& value ) {
		in.clear();
		string s ; 
  		in >> s;
  		try {
    		value = CSyscalMethod::literal(s);
  		}
  		catch (string & m) {
    		in.setstate(ios::failbit);
  		}
  		return in;
  	}
}

