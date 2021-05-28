
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
 * File CCalCurveType.cpp
 */
#include <sstream>
#include <alma/Enumerations/CCalCurveType.h>
#include <string>
using namespace std;

int CCalCurveType::version() {
	return CalCurveTypeMod::version;
	}
	
string CCalCurveType::revision () {
	return CalCurveTypeMod::revision;
}

unsigned int CCalCurveType::size() {
	return 3;
	}
	
	
const std::string& CCalCurveType::sAMPLITUDE = "AMPLITUDE";
	
const std::string& CCalCurveType::sPHASE = "PHASE";
	
const std::string& CCalCurveType::sUNDEFINED = "UNDEFINED";
	
const std::vector<std::string> CCalCurveType::names() {
    std::vector<std::string> enumSet;
    
    enumSet.insert(enumSet.end(), CCalCurveType::sAMPLITUDE);
    
    enumSet.insert(enumSet.end(), CCalCurveType::sPHASE);
    
    enumSet.insert(enumSet.end(), CCalCurveType::sUNDEFINED);
        
    return enumSet;
}

std::string CCalCurveType::name(const CalCurveTypeMod::CalCurveType& f) {
    switch (f) {
    
    case CalCurveTypeMod::AMPLITUDE:
      return CCalCurveType::sAMPLITUDE;
    
    case CalCurveTypeMod::PHASE:
      return CCalCurveType::sPHASE;
    
    case CalCurveTypeMod::UNDEFINED:
      return CCalCurveType::sUNDEFINED;
    	
    }
    // Impossible siutation but....who knows with C++ enums
    throw badInt((int) f);
}

CalCurveTypeMod::CalCurveType CCalCurveType::newCalCurveType(const std::string& name) {
		
    if (name == CCalCurveType::sAMPLITUDE) {
        return CalCurveTypeMod::AMPLITUDE;
    }
    	
    if (name == CCalCurveType::sPHASE) {
        return CalCurveTypeMod::PHASE;
    }
    	
    if (name == CCalCurveType::sUNDEFINED) {
        return CalCurveTypeMod::UNDEFINED;
    }
    
    throw badString(name);
}

CalCurveTypeMod::CalCurveType CCalCurveType::literal(const std::string& name) {
		
    if (name == CCalCurveType::sAMPLITUDE) {
        return CalCurveTypeMod::AMPLITUDE;
    }
    	
    if (name == CCalCurveType::sPHASE) {
        return CalCurveTypeMod::PHASE;
    }
    	
    if (name == CCalCurveType::sUNDEFINED) {
        return CalCurveTypeMod::UNDEFINED;
    }
    
    throw badString(name);
}

CalCurveTypeMod::CalCurveType CCalCurveType::from_int(unsigned int i) {
	vector<string> names_ = names();
	if (i >= names_.size()) throw badInt(i);
	return newCalCurveType(names_.at(i));
}

string CCalCurveType::badString(const string& name) {
	return "'"+name+"' does not correspond to any literal in the enumeration 'CalCurveType'.";
}

string CCalCurveType::badInt(unsigned int i) {
	ostringstream oss ;
	oss << "'" << i << "' is out of range for the enumeration 'CalCurveType'.";
	return oss.str();
}

namespace CalCurveTypeMod {
	std::ostream & operator << ( std::ostream & out, const CalCurveType& value) {
		out << CCalCurveType::name(value);
		return out;
	}

	std::istream & operator >> ( std::istream & in , CalCurveType& value ) {
		in.clear();
		string s ; 
  		in >> s;
  		try {
    		value = CCalCurveType::literal(s);
  		}
  		catch (string & m) {
    		in.setstate(ios::failbit);
  		}
  		return in;
  	}
}

