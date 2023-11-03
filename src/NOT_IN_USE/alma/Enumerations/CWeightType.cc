
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
 * File CWeightType.cpp
 */
#include <sstream>
#include <alma/Enumerations/CWeightType.h>
#include <string>
using namespace std;

int CWeightType::version() {
	return WeightTypeMod::version;
	}
	
string CWeightType::revision () {
	return WeightTypeMod::revision;
}

unsigned int CWeightType::size() {
	return 3;
	}
	
	
const std::string& CWeightType::sK = "K";
	
const std::string& CWeightType::sJY = "JY";
	
const std::string& CWeightType::sCOUNT_WEIGHT = "COUNT_WEIGHT";
	
const std::vector<std::string> CWeightType::names() {
    std::vector<std::string> enumSet;
    
    enumSet.insert(enumSet.end(), CWeightType::sK);
    
    enumSet.insert(enumSet.end(), CWeightType::sJY);
    
    enumSet.insert(enumSet.end(), CWeightType::sCOUNT_WEIGHT);
        
    return enumSet;
}

std::string CWeightType::name(const WeightTypeMod::WeightType& f) {
    switch (f) {
    
    case WeightTypeMod::K:
      return CWeightType::sK;
    
    case WeightTypeMod::JY:
      return CWeightType::sJY;
    
    case WeightTypeMod::COUNT_WEIGHT:
      return CWeightType::sCOUNT_WEIGHT;
    	
    }
    // Impossible siutation but....who knows with C++ enums
    throw badInt((int) f);
}

WeightTypeMod::WeightType CWeightType::newWeightType(const std::string& name) {
		
    if (name == CWeightType::sK) {
        return WeightTypeMod::K;
    }
    	
    if (name == CWeightType::sJY) {
        return WeightTypeMod::JY;
    }
    	
    if (name == CWeightType::sCOUNT_WEIGHT) {
        return WeightTypeMod::COUNT_WEIGHT;
    }
    
    throw badString(name);
}

WeightTypeMod::WeightType CWeightType::literal(const std::string& name) {
		
    if (name == CWeightType::sK) {
        return WeightTypeMod::K;
    }
    	
    if (name == CWeightType::sJY) {
        return WeightTypeMod::JY;
    }
    	
    if (name == CWeightType::sCOUNT_WEIGHT) {
        return WeightTypeMod::COUNT_WEIGHT;
    }
    
    throw badString(name);
}

WeightTypeMod::WeightType CWeightType::from_int(unsigned int i) {
	vector<string> names_ = names();
	if (i >= names_.size()) throw badInt(i);
	return newWeightType(names_.at(i));
}

string CWeightType::badString(const string& name) {
	return "'"+name+"' does not correspond to any literal in the enumeration 'WeightType'.";
}

string CWeightType::badInt(unsigned int i) {
	ostringstream oss ;
	oss << "'" << i << "' is out of range for the enumeration 'WeightType'.";
	return oss.str();
}

namespace WeightTypeMod {
	std::ostream & operator << ( std::ostream & out, const WeightType& value) {
		out << CWeightType::name(value);
		return out;
	}

	std::istream & operator >> ( std::istream & in , WeightType& value ) {
		in.clear();
		string s ; 
  		in >> s;
  		try {
    		value = CWeightType::literal(s);
  		}
  		catch (string & m) {
    		in.setstate(ios::failbit);
  		}
  		return in;
  	}
}

