
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
 * File CPointingMethod.cpp
 */
#include <sstream>
#include <alma/Enumerations/CPointingMethod.h>
#include <string>
using namespace std;

int CPointingMethod::version() {
	return PointingMethodMod::version;
	}
	
string CPointingMethod::revision () {
	return PointingMethodMod::revision;
}

unsigned int CPointingMethod::size() {
	return 6;
	}
	
	
const std::string& CPointingMethod::sTHREE_POINT = "THREE_POINT";
	
const std::string& CPointingMethod::sFOUR_POINT = "FOUR_POINT";
	
const std::string& CPointingMethod::sFIVE_POINT = "FIVE_POINT";
	
const std::string& CPointingMethod::sCROSS = "CROSS";
	
const std::string& CPointingMethod::sCIRCLE = "CIRCLE";
	
const std::string& CPointingMethod::sHOLOGRAPHY = "HOLOGRAPHY";
	
const std::vector<std::string> CPointingMethod::names() {
    std::vector<std::string> enumSet;
    
    enumSet.insert(enumSet.end(), CPointingMethod::sTHREE_POINT);
    
    enumSet.insert(enumSet.end(), CPointingMethod::sFOUR_POINT);
    
    enumSet.insert(enumSet.end(), CPointingMethod::sFIVE_POINT);
    
    enumSet.insert(enumSet.end(), CPointingMethod::sCROSS);
    
    enumSet.insert(enumSet.end(), CPointingMethod::sCIRCLE);
    
    enumSet.insert(enumSet.end(), CPointingMethod::sHOLOGRAPHY);
        
    return enumSet;
}

std::string CPointingMethod::name(const PointingMethodMod::PointingMethod& f) {
    switch (f) {
    
    case PointingMethodMod::THREE_POINT:
      return CPointingMethod::sTHREE_POINT;
    
    case PointingMethodMod::FOUR_POINT:
      return CPointingMethod::sFOUR_POINT;
    
    case PointingMethodMod::FIVE_POINT:
      return CPointingMethod::sFIVE_POINT;
    
    case PointingMethodMod::CROSS:
      return CPointingMethod::sCROSS;
    
    case PointingMethodMod::CIRCLE:
      return CPointingMethod::sCIRCLE;
    
    case PointingMethodMod::HOLOGRAPHY:
      return CPointingMethod::sHOLOGRAPHY;
    	
    }
    // Impossible siutation but....who knows with C++ enums
    throw badInt((int) f);
}

PointingMethodMod::PointingMethod CPointingMethod::newPointingMethod(const std::string& name) {
		
    if (name == CPointingMethod::sTHREE_POINT) {
        return PointingMethodMod::THREE_POINT;
    }
    	
    if (name == CPointingMethod::sFOUR_POINT) {
        return PointingMethodMod::FOUR_POINT;
    }
    	
    if (name == CPointingMethod::sFIVE_POINT) {
        return PointingMethodMod::FIVE_POINT;
    }
    	
    if (name == CPointingMethod::sCROSS) {
        return PointingMethodMod::CROSS;
    }
    	
    if (name == CPointingMethod::sCIRCLE) {
        return PointingMethodMod::CIRCLE;
    }
    	
    if (name == CPointingMethod::sHOLOGRAPHY) {
        return PointingMethodMod::HOLOGRAPHY;
    }
    
    throw badString(name);
}

PointingMethodMod::PointingMethod CPointingMethod::literal(const std::string& name) {
		
    if (name == CPointingMethod::sTHREE_POINT) {
        return PointingMethodMod::THREE_POINT;
    }
    	
    if (name == CPointingMethod::sFOUR_POINT) {
        return PointingMethodMod::FOUR_POINT;
    }
    	
    if (name == CPointingMethod::sFIVE_POINT) {
        return PointingMethodMod::FIVE_POINT;
    }
    	
    if (name == CPointingMethod::sCROSS) {
        return PointingMethodMod::CROSS;
    }
    	
    if (name == CPointingMethod::sCIRCLE) {
        return PointingMethodMod::CIRCLE;
    }
    	
    if (name == CPointingMethod::sHOLOGRAPHY) {
        return PointingMethodMod::HOLOGRAPHY;
    }
    
    throw badString(name);
}

PointingMethodMod::PointingMethod CPointingMethod::from_int(unsigned int i) {
	vector<string> names_ = names();
	if (i >= names_.size()) throw badInt(i);
	return newPointingMethod(names_.at(i));
}

string CPointingMethod::badString(const string& name) {
	return "'"+name+"' does not correspond to any literal in the enumeration 'PointingMethod'.";
}

string CPointingMethod::badInt(unsigned int i) {
	ostringstream oss ;
	oss << "'" << i << "' is out of range for the enumeration 'PointingMethod'.";
	return oss.str();
}

namespace PointingMethodMod {
	std::ostream & operator << ( std::ostream & out, const PointingMethod& value) {
		out << CPointingMethod::name(value);
		return out;
	}

	std::istream & operator >> ( std::istream & in , PointingMethod& value ) {
		in.clear();
		string s ; 
  		in >> s;
  		try {
    		value = CPointingMethod::literal(s);
  		}
  		catch (string & m) {
    		in.setstate(ios::failbit);
  		}
  		return in;
  	}
}

