
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
 * File CCorrelatorType.cpp
 */
#include <sstream>
#include <alma/Enumerations/CCorrelatorType.h>
#include <string>
using namespace std;

int CCorrelatorType::version() {
	return CorrelatorTypeMod::version;
	}
	
string CCorrelatorType::revision () {
	return CorrelatorTypeMod::revision;
}

unsigned int CCorrelatorType::size() {
	return 3;
	}
	
	
const std::string& CCorrelatorType::sFX = "FX";
	
const std::string& CCorrelatorType::sXF = "XF";
	
const std::string& CCorrelatorType::sFXF = "FXF";
	
const std::vector<std::string> CCorrelatorType::names() {
    std::vector<std::string> enumSet;
    
    enumSet.insert(enumSet.end(), CCorrelatorType::sFX);
    
    enumSet.insert(enumSet.end(), CCorrelatorType::sXF);
    
    enumSet.insert(enumSet.end(), CCorrelatorType::sFXF);
        
    return enumSet;
}

std::string CCorrelatorType::name(const CorrelatorTypeMod::CorrelatorType& f) {
    switch (f) {
    
    case CorrelatorTypeMod::FX:
      return CCorrelatorType::sFX;
    
    case CorrelatorTypeMod::XF:
      return CCorrelatorType::sXF;
    
    case CorrelatorTypeMod::FXF:
      return CCorrelatorType::sFXF;
    	
    }
    // Impossible siutation but....who knows with C++ enums
    throw badInt((int) f);
}

CorrelatorTypeMod::CorrelatorType CCorrelatorType::newCorrelatorType(const std::string& name) {
		
    if (name == CCorrelatorType::sFX) {
        return CorrelatorTypeMod::FX;
    }
    	
    if (name == CCorrelatorType::sXF) {
        return CorrelatorTypeMod::XF;
    }
    	
    if (name == CCorrelatorType::sFXF) {
        return CorrelatorTypeMod::FXF;
    }
    
    throw badString(name);
}

CorrelatorTypeMod::CorrelatorType CCorrelatorType::literal(const std::string& name) {
		
    if (name == CCorrelatorType::sFX) {
        return CorrelatorTypeMod::FX;
    }
    	
    if (name == CCorrelatorType::sXF) {
        return CorrelatorTypeMod::XF;
    }
    	
    if (name == CCorrelatorType::sFXF) {
        return CorrelatorTypeMod::FXF;
    }
    
    throw badString(name);
}

CorrelatorTypeMod::CorrelatorType CCorrelatorType::from_int(unsigned int i) {
	vector<string> names_ = names();
	if (i >= names_.size()) throw badInt(i);
	return newCorrelatorType(names_.at(i));
}

string CCorrelatorType::badString(const string& name) {
	return "'"+name+"' does not correspond to any literal in the enumeration 'CorrelatorType'.";
}

string CCorrelatorType::badInt(unsigned int i) {
	ostringstream oss ;
	oss << "'" << i << "' is out of range for the enumeration 'CorrelatorType'.";
	return oss.str();
}

namespace CorrelatorTypeMod {
	std::ostream & operator << ( std::ostream & out, const CorrelatorType& value) {
		out << CCorrelatorType::name(value);
		return out;
	}

	std::istream & operator >> ( std::istream & in , CorrelatorType& value ) {
		in.clear();
		string s ; 
  		in >> s;
  		try {
    		value = CCorrelatorType::literal(s);
  		}
  		catch (string & m) {
    		in.setstate(ios::failbit);
  		}
  		return in;
  	}
}

