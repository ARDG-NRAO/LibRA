
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
 * File CSidebandProcessingMode.cpp
 */
#include <sstream>
#include <alma/Enumerations/CSidebandProcessingMode.h>
#include <string>
using namespace std;

int CSidebandProcessingMode::version() {
	return SidebandProcessingModeMod::version;
	}
	
string CSidebandProcessingMode::revision () {
	return SidebandProcessingModeMod::revision;
}

unsigned int CSidebandProcessingMode::size() {
	return 5;
	}
	
	
const std::string& CSidebandProcessingMode::sNONE = "NONE";
	
const std::string& CSidebandProcessingMode::sPHASE_SWITCH_SEPARATION = "PHASE_SWITCH_SEPARATION";
	
const std::string& CSidebandProcessingMode::sFREQUENCY_OFFSET_SEPARATION = "FREQUENCY_OFFSET_SEPARATION";
	
const std::string& CSidebandProcessingMode::sPHASE_SWITCH_REJECTION = "PHASE_SWITCH_REJECTION";
	
const std::string& CSidebandProcessingMode::sFREQUENCY_OFFSET_REJECTION = "FREQUENCY_OFFSET_REJECTION";
	
const std::vector<std::string> CSidebandProcessingMode::names() {
    std::vector<std::string> enumSet;
    
    enumSet.insert(enumSet.end(), CSidebandProcessingMode::sNONE);
    
    enumSet.insert(enumSet.end(), CSidebandProcessingMode::sPHASE_SWITCH_SEPARATION);
    
    enumSet.insert(enumSet.end(), CSidebandProcessingMode::sFREQUENCY_OFFSET_SEPARATION);
    
    enumSet.insert(enumSet.end(), CSidebandProcessingMode::sPHASE_SWITCH_REJECTION);
    
    enumSet.insert(enumSet.end(), CSidebandProcessingMode::sFREQUENCY_OFFSET_REJECTION);
        
    return enumSet;
}

std::string CSidebandProcessingMode::name(const SidebandProcessingModeMod::SidebandProcessingMode& f) {
    switch (f) {
    
    case SidebandProcessingModeMod::NONE:
      return CSidebandProcessingMode::sNONE;
    
    case SidebandProcessingModeMod::PHASE_SWITCH_SEPARATION:
      return CSidebandProcessingMode::sPHASE_SWITCH_SEPARATION;
    
    case SidebandProcessingModeMod::FREQUENCY_OFFSET_SEPARATION:
      return CSidebandProcessingMode::sFREQUENCY_OFFSET_SEPARATION;
    
    case SidebandProcessingModeMod::PHASE_SWITCH_REJECTION:
      return CSidebandProcessingMode::sPHASE_SWITCH_REJECTION;
    
    case SidebandProcessingModeMod::FREQUENCY_OFFSET_REJECTION:
      return CSidebandProcessingMode::sFREQUENCY_OFFSET_REJECTION;
    	
    }
    // Impossible siutation but....who knows with C++ enums
    throw badInt((int) f);
}

SidebandProcessingModeMod::SidebandProcessingMode CSidebandProcessingMode::newSidebandProcessingMode(const std::string& name) {
		
    if (name == CSidebandProcessingMode::sNONE) {
        return SidebandProcessingModeMod::NONE;
    }
    	
    if (name == CSidebandProcessingMode::sPHASE_SWITCH_SEPARATION) {
        return SidebandProcessingModeMod::PHASE_SWITCH_SEPARATION;
    }
    	
    if (name == CSidebandProcessingMode::sFREQUENCY_OFFSET_SEPARATION) {
        return SidebandProcessingModeMod::FREQUENCY_OFFSET_SEPARATION;
    }
    	
    if (name == CSidebandProcessingMode::sPHASE_SWITCH_REJECTION) {
        return SidebandProcessingModeMod::PHASE_SWITCH_REJECTION;
    }
    	
    if (name == CSidebandProcessingMode::sFREQUENCY_OFFSET_REJECTION) {
        return SidebandProcessingModeMod::FREQUENCY_OFFSET_REJECTION;
    }
    
    throw badString(name);
}

SidebandProcessingModeMod::SidebandProcessingMode CSidebandProcessingMode::literal(const std::string& name) {
		
    if (name == CSidebandProcessingMode::sNONE) {
        return SidebandProcessingModeMod::NONE;
    }
    	
    if (name == CSidebandProcessingMode::sPHASE_SWITCH_SEPARATION) {
        return SidebandProcessingModeMod::PHASE_SWITCH_SEPARATION;
    }
    	
    if (name == CSidebandProcessingMode::sFREQUENCY_OFFSET_SEPARATION) {
        return SidebandProcessingModeMod::FREQUENCY_OFFSET_SEPARATION;
    }
    	
    if (name == CSidebandProcessingMode::sPHASE_SWITCH_REJECTION) {
        return SidebandProcessingModeMod::PHASE_SWITCH_REJECTION;
    }
    	
    if (name == CSidebandProcessingMode::sFREQUENCY_OFFSET_REJECTION) {
        return SidebandProcessingModeMod::FREQUENCY_OFFSET_REJECTION;
    }
    
    throw badString(name);
}

SidebandProcessingModeMod::SidebandProcessingMode CSidebandProcessingMode::from_int(unsigned int i) {
	vector<string> names_ = names();
	if (i >= names_.size()) throw badInt(i);
	return newSidebandProcessingMode(names_.at(i));
}

string CSidebandProcessingMode::badString(const string& name) {
	return "'"+name+"' does not correspond to any literal in the enumeration 'SidebandProcessingMode'.";
}

string CSidebandProcessingMode::badInt(unsigned int i) {
	ostringstream oss ;
	oss << "'" << i << "' is out of range for the enumeration 'SidebandProcessingMode'.";
	return oss.str();
}

namespace SidebandProcessingModeMod {
	std::ostream & operator << ( std::ostream & out, const SidebandProcessingMode& value) {
		out << CSidebandProcessingMode::name(value);
		return out;
	}

	std::istream & operator >> ( std::istream & in , SidebandProcessingMode& value ) {
		in.clear();
		string s ; 
  		in >> s;
  		try {
    		value = CSidebandProcessingMode::literal(s);
  		}
  		catch (string & m) {
    		in.setstate(ios::failbit);
  		}
  		return in;
  	}
}

