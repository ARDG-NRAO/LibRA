
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
 * File CStationType.cpp
 */
#include <sstream>
#include <alma/Enumerations/CStationType.h>
#include <string>
using namespace std;

int CStationType::version() {
	return StationTypeMod::version;
	}
	
string CStationType::revision () {
	return StationTypeMod::revision;
}

unsigned int CStationType::size() {
	return 3;
	}
	
	
const std::string& CStationType::sANTENNA_PAD = "ANTENNA_PAD";
	
const std::string& CStationType::sMAINTENANCE_PAD = "MAINTENANCE_PAD";
	
const std::string& CStationType::sWEATHER_STATION = "WEATHER_STATION";
	
const std::vector<std::string> CStationType::names() {
    std::vector<std::string> enumSet;
    
    enumSet.insert(enumSet.end(), CStationType::sANTENNA_PAD);
    
    enumSet.insert(enumSet.end(), CStationType::sMAINTENANCE_PAD);
    
    enumSet.insert(enumSet.end(), CStationType::sWEATHER_STATION);
        
    return enumSet;
}

std::string CStationType::name(const StationTypeMod::StationType& f) {
    switch (f) {
    
    case StationTypeMod::ANTENNA_PAD:
      return CStationType::sANTENNA_PAD;
    
    case StationTypeMod::MAINTENANCE_PAD:
      return CStationType::sMAINTENANCE_PAD;
    
    case StationTypeMod::WEATHER_STATION:
      return CStationType::sWEATHER_STATION;
    	
    }
    // Impossible siutation but....who knows with C++ enums
    throw badInt((int) f);
}

StationTypeMod::StationType CStationType::newStationType(const std::string& name) {
		
    if (name == CStationType::sANTENNA_PAD) {
        return StationTypeMod::ANTENNA_PAD;
    }
    	
    if (name == CStationType::sMAINTENANCE_PAD) {
        return StationTypeMod::MAINTENANCE_PAD;
    }
    	
    if (name == CStationType::sWEATHER_STATION) {
        return StationTypeMod::WEATHER_STATION;
    }
    
    throw badString(name);
}

StationTypeMod::StationType CStationType::literal(const std::string& name) {
		
    if (name == CStationType::sANTENNA_PAD) {
        return StationTypeMod::ANTENNA_PAD;
    }
    	
    if (name == CStationType::sMAINTENANCE_PAD) {
        return StationTypeMod::MAINTENANCE_PAD;
    }
    	
    if (name == CStationType::sWEATHER_STATION) {
        return StationTypeMod::WEATHER_STATION;
    }
    
    throw badString(name);
}

StationTypeMod::StationType CStationType::from_int(unsigned int i) {
	vector<string> names_ = names();
	if (i >= names_.size()) throw badInt(i);
	return newStationType(names_.at(i));
}

string CStationType::badString(const string& name) {
	return "'"+name+"' does not correspond to any literal in the enumeration 'StationType'.";
}

string CStationType::badInt(unsigned int i) {
	ostringstream oss ;
	oss << "'" << i << "' is out of range for the enumeration 'StationType'.";
	return oss.str();
}

namespace StationTypeMod {
	std::ostream & operator << ( std::ostream & out, const StationType& value) {
		out << CStationType::name(value);
		return out;
	}

	std::istream & operator >> ( std::istream & in , StationType& value ) {
		in.clear();
		string s ; 
  		in >> s;
  		try {
    		value = CStationType::literal(s);
  		}
  		catch (string & m) {
    		in.setstate(ios::failbit);
  		}
  		return in;
  	}
}

