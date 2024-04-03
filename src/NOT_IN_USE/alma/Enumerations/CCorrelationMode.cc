
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
 * File CCorrelationMode.cpp
 */
#include <sstream>
#include <alma/Enumerations/CCorrelationMode.h>
#include <string>
using namespace std;

int CCorrelationMode::version() {
	return CorrelationModeMod::version;
	}
	
string CCorrelationMode::revision () {
	return CorrelationModeMod::revision;
}

unsigned int CCorrelationMode::size() {
	return 3;
	}
	
	
const std::string& CCorrelationMode::sCROSS_ONLY = "CROSS_ONLY";
	
const std::string& CCorrelationMode::sAUTO_ONLY = "AUTO_ONLY";
	
const std::string& CCorrelationMode::sCROSS_AND_AUTO = "CROSS_AND_AUTO";
	
const std::vector<std::string> CCorrelationMode::names() {
    std::vector<std::string> enumSet;
    
    enumSet.insert(enumSet.end(), CCorrelationMode::sCROSS_ONLY);
    
    enumSet.insert(enumSet.end(), CCorrelationMode::sAUTO_ONLY);
    
    enumSet.insert(enumSet.end(), CCorrelationMode::sCROSS_AND_AUTO);
        
    return enumSet;
}

std::string CCorrelationMode::name(const CorrelationModeMod::CorrelationMode& f) {
    switch (f) {
    
    case CorrelationModeMod::CROSS_ONLY:
      return CCorrelationMode::sCROSS_ONLY;
    
    case CorrelationModeMod::AUTO_ONLY:
      return CCorrelationMode::sAUTO_ONLY;
    
    case CorrelationModeMod::CROSS_AND_AUTO:
      return CCorrelationMode::sCROSS_AND_AUTO;
    	
    }
    // Impossible siutation but....who knows with C++ enums
    throw badInt((int) f);
}

CorrelationModeMod::CorrelationMode CCorrelationMode::newCorrelationMode(const std::string& name) {
		
    if (name == CCorrelationMode::sCROSS_ONLY) {
        return CorrelationModeMod::CROSS_ONLY;
    }
    	
    if (name == CCorrelationMode::sAUTO_ONLY) {
        return CorrelationModeMod::AUTO_ONLY;
    }
    	
    if (name == CCorrelationMode::sCROSS_AND_AUTO) {
        return CorrelationModeMod::CROSS_AND_AUTO;
    }
    
    throw badString(name);
}

CorrelationModeMod::CorrelationMode CCorrelationMode::literal(const std::string& name) {
		
    if (name == CCorrelationMode::sCROSS_ONLY) {
        return CorrelationModeMod::CROSS_ONLY;
    }
    	
    if (name == CCorrelationMode::sAUTO_ONLY) {
        return CorrelationModeMod::AUTO_ONLY;
    }
    	
    if (name == CCorrelationMode::sCROSS_AND_AUTO) {
        return CorrelationModeMod::CROSS_AND_AUTO;
    }
    
    throw badString(name);
}

CorrelationModeMod::CorrelationMode CCorrelationMode::from_int(unsigned int i) {
	vector<string> names_ = names();
	if (i >= names_.size()) throw badInt(i);
	return newCorrelationMode(names_.at(i));
}

string CCorrelationMode::badString(const string& name) {
	return "'"+name+"' does not correspond to any literal in the enumeration 'CorrelationMode'.";
}

string CCorrelationMode::badInt(unsigned int i) {
	ostringstream oss ;
	oss << "'" << i << "' is out of range for the enumeration 'CorrelationMode'.";
	return oss.str();
}

namespace CorrelationModeMod {
	std::ostream & operator << ( std::ostream & out, const CorrelationMode& value) {
		out << CCorrelationMode::name(value);
		return out;
	}

	std::istream & operator >> ( std::istream & in , CorrelationMode& value ) {
		in.clear();
		string s ; 
  		in >> s;
  		try {
    		value = CCorrelationMode::literal(s);
  		}
  		catch (string & m) {
    		in.setstate(ios::failbit);
  		}
  		return in;
  	}
}

