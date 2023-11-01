
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
 * File CPositionMethod.cpp
 */
#include <sstream>
#include <alma/Enumerations/CPositionMethod.h>
#include <string>
using namespace std;

int CPositionMethod::version() {
	return PositionMethodMod::version;
	}
	
string CPositionMethod::revision () {
	return PositionMethodMod::revision;
}

unsigned int CPositionMethod::size() {
	return 2;
	}
	
	
const std::string& CPositionMethod::sDELAY_FITTING = "DELAY_FITTING";
	
const std::string& CPositionMethod::sPHASE_FITTING = "PHASE_FITTING";
	
const std::vector<std::string> CPositionMethod::names() {
    std::vector<std::string> enumSet;
    
    enumSet.insert(enumSet.end(), CPositionMethod::sDELAY_FITTING);
    
    enumSet.insert(enumSet.end(), CPositionMethod::sPHASE_FITTING);
        
    return enumSet;
}

std::string CPositionMethod::name(const PositionMethodMod::PositionMethod& f) {
    switch (f) {
    
    case PositionMethodMod::DELAY_FITTING:
      return CPositionMethod::sDELAY_FITTING;
    
    case PositionMethodMod::PHASE_FITTING:
      return CPositionMethod::sPHASE_FITTING;
    	
    }
    // Impossible siutation but....who knows with C++ enums
    throw badInt((int) f);
}

PositionMethodMod::PositionMethod CPositionMethod::newPositionMethod(const std::string& name) {
		
    if (name == CPositionMethod::sDELAY_FITTING) {
        return PositionMethodMod::DELAY_FITTING;
    }
    	
    if (name == CPositionMethod::sPHASE_FITTING) {
        return PositionMethodMod::PHASE_FITTING;
    }
    
    throw badString(name);
}

PositionMethodMod::PositionMethod CPositionMethod::literal(const std::string& name) {
		
    if (name == CPositionMethod::sDELAY_FITTING) {
        return PositionMethodMod::DELAY_FITTING;
    }
    	
    if (name == CPositionMethod::sPHASE_FITTING) {
        return PositionMethodMod::PHASE_FITTING;
    }
    
    throw badString(name);
}

PositionMethodMod::PositionMethod CPositionMethod::from_int(unsigned int i) {
	vector<string> names_ = names();
	if (i >= names_.size()) throw badInt(i);
	return newPositionMethod(names_.at(i));
}

string CPositionMethod::badString(const string& name) {
	return "'"+name+"' does not correspond to any literal in the enumeration 'PositionMethod'.";
}

string CPositionMethod::badInt(unsigned int i) {
	ostringstream oss ;
	oss << "'" << i << "' is out of range for the enumeration 'PositionMethod'.";
	return oss.str();
}

namespace PositionMethodMod {
	std::ostream & operator << ( std::ostream & out, const PositionMethod& value) {
		out << CPositionMethod::name(value);
		return out;
	}

	std::istream & operator >> ( std::istream & in , PositionMethod& value ) {
		in.clear();
		string s ; 
  		in >> s;
  		try {
    		value = CPositionMethod::literal(s);
  		}
  		catch (string & m) {
    		in.setstate(ios::failbit);
  		}
  		return in;
  	}
}

