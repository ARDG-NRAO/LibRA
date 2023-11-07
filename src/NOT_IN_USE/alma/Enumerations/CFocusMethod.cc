
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
 * File CFocusMethod.cpp
 */
#include <sstream>
#include <alma/Enumerations/CFocusMethod.h>
#include <string>
using namespace std;

int CFocusMethod::version() {
	return FocusMethodMod::version;
	}
	
string CFocusMethod::revision () {
	return FocusMethodMod::revision;
}

unsigned int CFocusMethod::size() {
	return 3;
	}
	
	
const std::string& CFocusMethod::sTHREE_POINT = "THREE_POINT";
	
const std::string& CFocusMethod::sFIVE_POINT = "FIVE_POINT";
	
const std::string& CFocusMethod::sHOLOGRAPHY = "HOLOGRAPHY";
	
const std::vector<std::string> CFocusMethod::names() {
    std::vector<std::string> enumSet;
    
    enumSet.insert(enumSet.end(), CFocusMethod::sTHREE_POINT);
    
    enumSet.insert(enumSet.end(), CFocusMethod::sFIVE_POINT);
    
    enumSet.insert(enumSet.end(), CFocusMethod::sHOLOGRAPHY);
        
    return enumSet;
}

std::string CFocusMethod::name(const FocusMethodMod::FocusMethod& f) {
    switch (f) {
    
    case FocusMethodMod::THREE_POINT:
      return CFocusMethod::sTHREE_POINT;
    
    case FocusMethodMod::FIVE_POINT:
      return CFocusMethod::sFIVE_POINT;
    
    case FocusMethodMod::HOLOGRAPHY:
      return CFocusMethod::sHOLOGRAPHY;
    	
    }
    // Impossible siutation but....who knows with C++ enums
    throw badInt((int) f);
}

FocusMethodMod::FocusMethod CFocusMethod::newFocusMethod(const std::string& name) {
		
    if (name == CFocusMethod::sTHREE_POINT) {
        return FocusMethodMod::THREE_POINT;
    }
    	
    if (name == CFocusMethod::sFIVE_POINT) {
        return FocusMethodMod::FIVE_POINT;
    }
    	
    if (name == CFocusMethod::sHOLOGRAPHY) {
        return FocusMethodMod::HOLOGRAPHY;
    }
    
    throw badString(name);
}

FocusMethodMod::FocusMethod CFocusMethod::literal(const std::string& name) {
		
    if (name == CFocusMethod::sTHREE_POINT) {
        return FocusMethodMod::THREE_POINT;
    }
    	
    if (name == CFocusMethod::sFIVE_POINT) {
        return FocusMethodMod::FIVE_POINT;
    }
    	
    if (name == CFocusMethod::sHOLOGRAPHY) {
        return FocusMethodMod::HOLOGRAPHY;
    }
    
    throw badString(name);
}

FocusMethodMod::FocusMethod CFocusMethod::from_int(unsigned int i) {
	vector<string> names_ = names();
	if (i >= names_.size()) throw badInt(i);
	return newFocusMethod(names_.at(i));
}

string CFocusMethod::badString(const string& name) {
	return "'"+name+"' does not correspond to any literal in the enumeration 'FocusMethod'.";
}

string CFocusMethod::badInt(unsigned int i) {
	ostringstream oss ;
	oss << "'" << i << "' is out of range for the enumeration 'FocusMethod'.";
	return oss.str();
}

namespace FocusMethodMod {
	std::ostream & operator << ( std::ostream & out, const FocusMethod& value) {
		out << CFocusMethod::name(value);
		return out;
	}

	std::istream & operator >> ( std::istream & in , FocusMethod& value ) {
		in.clear();
		string s ; 
  		in >> s;
  		try {
    		value = CFocusMethod::literal(s);
  		}
  		catch (string & m) {
    		in.setstate(ios::failbit);
  		}
  		return in;
  	}
}

