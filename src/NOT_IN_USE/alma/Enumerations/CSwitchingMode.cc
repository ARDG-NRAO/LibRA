
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
 * File CSwitchingMode.cpp
 */
#include <sstream>
#include <alma/Enumerations/CSwitchingMode.h>
#include <string>
using namespace std;

int CSwitchingMode::version() {
	return SwitchingModeMod::version;
	}
	
string CSwitchingMode::revision () {
	return SwitchingModeMod::revision;
}

unsigned int CSwitchingMode::size() {
	return 7;
	}
	
	
const std::string& CSwitchingMode::sNO_SWITCHING = "NO_SWITCHING";
	
const std::string& CSwitchingMode::sLOAD_SWITCHING = "LOAD_SWITCHING";
	
const std::string& CSwitchingMode::sPOSITION_SWITCHING = "POSITION_SWITCHING";
	
const std::string& CSwitchingMode::sPHASE_SWITCHING = "PHASE_SWITCHING";
	
const std::string& CSwitchingMode::sFREQUENCY_SWITCHING = "FREQUENCY_SWITCHING";
	
const std::string& CSwitchingMode::sNUTATOR_SWITCHING = "NUTATOR_SWITCHING";
	
const std::string& CSwitchingMode::sCHOPPER_WHEEL = "CHOPPER_WHEEL";
	
const std::vector<std::string> CSwitchingMode::names() {
    std::vector<std::string> enumSet;
    
    enumSet.insert(enumSet.end(), CSwitchingMode::sNO_SWITCHING);
    
    enumSet.insert(enumSet.end(), CSwitchingMode::sLOAD_SWITCHING);
    
    enumSet.insert(enumSet.end(), CSwitchingMode::sPOSITION_SWITCHING);
    
    enumSet.insert(enumSet.end(), CSwitchingMode::sPHASE_SWITCHING);
    
    enumSet.insert(enumSet.end(), CSwitchingMode::sFREQUENCY_SWITCHING);
    
    enumSet.insert(enumSet.end(), CSwitchingMode::sNUTATOR_SWITCHING);
    
    enumSet.insert(enumSet.end(), CSwitchingMode::sCHOPPER_WHEEL);
        
    return enumSet;
}

std::string CSwitchingMode::name(const SwitchingModeMod::SwitchingMode& f) {
    switch (f) {
    
    case SwitchingModeMod::NO_SWITCHING:
      return CSwitchingMode::sNO_SWITCHING;
    
    case SwitchingModeMod::LOAD_SWITCHING:
      return CSwitchingMode::sLOAD_SWITCHING;
    
    case SwitchingModeMod::POSITION_SWITCHING:
      return CSwitchingMode::sPOSITION_SWITCHING;
    
    case SwitchingModeMod::PHASE_SWITCHING:
      return CSwitchingMode::sPHASE_SWITCHING;
    
    case SwitchingModeMod::FREQUENCY_SWITCHING:
      return CSwitchingMode::sFREQUENCY_SWITCHING;
    
    case SwitchingModeMod::NUTATOR_SWITCHING:
      return CSwitchingMode::sNUTATOR_SWITCHING;
    
    case SwitchingModeMod::CHOPPER_WHEEL:
      return CSwitchingMode::sCHOPPER_WHEEL;
    	
    }
    // Impossible siutation but....who knows with C++ enums
    throw badInt((int) f);
}

SwitchingModeMod::SwitchingMode CSwitchingMode::newSwitchingMode(const std::string& name) {
		
    if (name == CSwitchingMode::sNO_SWITCHING) {
        return SwitchingModeMod::NO_SWITCHING;
    }
    	
    if (name == CSwitchingMode::sLOAD_SWITCHING) {
        return SwitchingModeMod::LOAD_SWITCHING;
    }
    	
    if (name == CSwitchingMode::sPOSITION_SWITCHING) {
        return SwitchingModeMod::POSITION_SWITCHING;
    }
    	
    if (name == CSwitchingMode::sPHASE_SWITCHING) {
        return SwitchingModeMod::PHASE_SWITCHING;
    }
    	
    if (name == CSwitchingMode::sFREQUENCY_SWITCHING) {
        return SwitchingModeMod::FREQUENCY_SWITCHING;
    }
    	
    if (name == CSwitchingMode::sNUTATOR_SWITCHING) {
        return SwitchingModeMod::NUTATOR_SWITCHING;
    }
    	
    if (name == CSwitchingMode::sCHOPPER_WHEEL) {
        return SwitchingModeMod::CHOPPER_WHEEL;
    }
    
    throw badString(name);
}

SwitchingModeMod::SwitchingMode CSwitchingMode::literal(const std::string& name) {
		
    if (name == CSwitchingMode::sNO_SWITCHING) {
        return SwitchingModeMod::NO_SWITCHING;
    }
    	
    if (name == CSwitchingMode::sLOAD_SWITCHING) {
        return SwitchingModeMod::LOAD_SWITCHING;
    }
    	
    if (name == CSwitchingMode::sPOSITION_SWITCHING) {
        return SwitchingModeMod::POSITION_SWITCHING;
    }
    	
    if (name == CSwitchingMode::sPHASE_SWITCHING) {
        return SwitchingModeMod::PHASE_SWITCHING;
    }
    	
    if (name == CSwitchingMode::sFREQUENCY_SWITCHING) {
        return SwitchingModeMod::FREQUENCY_SWITCHING;
    }
    	
    if (name == CSwitchingMode::sNUTATOR_SWITCHING) {
        return SwitchingModeMod::NUTATOR_SWITCHING;
    }
    	
    if (name == CSwitchingMode::sCHOPPER_WHEEL) {
        return SwitchingModeMod::CHOPPER_WHEEL;
    }
    
    throw badString(name);
}

SwitchingModeMod::SwitchingMode CSwitchingMode::from_int(unsigned int i) {
	vector<string> names_ = names();
	if (i >= names_.size()) throw badInt(i);
	return newSwitchingMode(names_.at(i));
}

string CSwitchingMode::badString(const string& name) {
	return "'"+name+"' does not correspond to any literal in the enumeration 'SwitchingMode'.";
}

string CSwitchingMode::badInt(unsigned int i) {
	ostringstream oss ;
	oss << "'" << i << "' is out of range for the enumeration 'SwitchingMode'.";
	return oss.str();
}

namespace SwitchingModeMod {
	std::ostream & operator << ( std::ostream & out, const SwitchingMode& value) {
		out << CSwitchingMode::name(value);
		return out;
	}

	std::istream & operator >> ( std::istream & in , SwitchingMode& value ) {
		in.clear();
		string s ; 
  		in >> s;
  		try {
    		value = CSwitchingMode::literal(s);
  		}
  		catch (string & m) {
    		in.setstate(ios::failbit);
  		}
  		return in;
  	}
}

