
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
 * File CPrimitiveDataType.cpp
 */
#include <sstream>
#include <alma/Enumerations/CPrimitiveDataType.h>
#include <string>
using namespace std;

int CPrimitiveDataType::version() {
	return PrimitiveDataTypeMod::version;
	}
	
string CPrimitiveDataType::revision () {
	return PrimitiveDataTypeMod::revision;
}

unsigned int CPrimitiveDataType::size() {
	return 5;
	}
	
	
const std::string& CPrimitiveDataType::sINT16_TYPE = "INT16_TYPE";
	
const std::string& CPrimitiveDataType::sINT32_TYPE = "INT32_TYPE";
	
const std::string& CPrimitiveDataType::sINT64_TYPE = "INT64_TYPE";
	
const std::string& CPrimitiveDataType::sFLOAT32_TYPE = "FLOAT32_TYPE";
	
const std::string& CPrimitiveDataType::sFLOAT64_TYPE = "FLOAT64_TYPE";
	
const std::vector<std::string> CPrimitiveDataType::names() {
    std::vector<std::string> enumSet;
    
    enumSet.insert(enumSet.end(), CPrimitiveDataType::sINT16_TYPE);
    
    enumSet.insert(enumSet.end(), CPrimitiveDataType::sINT32_TYPE);
    
    enumSet.insert(enumSet.end(), CPrimitiveDataType::sINT64_TYPE);
    
    enumSet.insert(enumSet.end(), CPrimitiveDataType::sFLOAT32_TYPE);
    
    enumSet.insert(enumSet.end(), CPrimitiveDataType::sFLOAT64_TYPE);
        
    return enumSet;
}

std::string CPrimitiveDataType::name(const PrimitiveDataTypeMod::PrimitiveDataType& f) {
    switch (f) {
    
    case PrimitiveDataTypeMod::INT16_TYPE:
      return CPrimitiveDataType::sINT16_TYPE;
    
    case PrimitiveDataTypeMod::INT32_TYPE:
      return CPrimitiveDataType::sINT32_TYPE;
    
    case PrimitiveDataTypeMod::INT64_TYPE:
      return CPrimitiveDataType::sINT64_TYPE;
    
    case PrimitiveDataTypeMod::FLOAT32_TYPE:
      return CPrimitiveDataType::sFLOAT32_TYPE;
    
    case PrimitiveDataTypeMod::FLOAT64_TYPE:
      return CPrimitiveDataType::sFLOAT64_TYPE;
    	
    }
    // Impossible siutation but....who knows with C++ enums
    throw badInt((int) f);
}

PrimitiveDataTypeMod::PrimitiveDataType CPrimitiveDataType::newPrimitiveDataType(const std::string& name) {
		
    if (name == CPrimitiveDataType::sINT16_TYPE) {
        return PrimitiveDataTypeMod::INT16_TYPE;
    }
    	
    if (name == CPrimitiveDataType::sINT32_TYPE) {
        return PrimitiveDataTypeMod::INT32_TYPE;
    }
    	
    if (name == CPrimitiveDataType::sINT64_TYPE) {
        return PrimitiveDataTypeMod::INT64_TYPE;
    }
    	
    if (name == CPrimitiveDataType::sFLOAT32_TYPE) {
        return PrimitiveDataTypeMod::FLOAT32_TYPE;
    }
    	
    if (name == CPrimitiveDataType::sFLOAT64_TYPE) {
        return PrimitiveDataTypeMod::FLOAT64_TYPE;
    }
    
    throw badString(name);
}

PrimitiveDataTypeMod::PrimitiveDataType CPrimitiveDataType::literal(const std::string& name) {
		
    if (name == CPrimitiveDataType::sINT16_TYPE) {
        return PrimitiveDataTypeMod::INT16_TYPE;
    }
    	
    if (name == CPrimitiveDataType::sINT32_TYPE) {
        return PrimitiveDataTypeMod::INT32_TYPE;
    }
    	
    if (name == CPrimitiveDataType::sINT64_TYPE) {
        return PrimitiveDataTypeMod::INT64_TYPE;
    }
    	
    if (name == CPrimitiveDataType::sFLOAT32_TYPE) {
        return PrimitiveDataTypeMod::FLOAT32_TYPE;
    }
    	
    if (name == CPrimitiveDataType::sFLOAT64_TYPE) {
        return PrimitiveDataTypeMod::FLOAT64_TYPE;
    }
    
    throw badString(name);
}

PrimitiveDataTypeMod::PrimitiveDataType CPrimitiveDataType::from_int(unsigned int i) {
	vector<string> names_ = names();
	if (i >= names_.size()) throw badInt(i);
	return newPrimitiveDataType(names_.at(i));
}

string CPrimitiveDataType::badString(const string& name) {
	return "'"+name+"' does not correspond to any literal in the enumeration 'PrimitiveDataType'.";
}

string CPrimitiveDataType::badInt(unsigned int i) {
	ostringstream oss ;
	oss << "'" << i << "' is out of range for the enumeration 'PrimitiveDataType'.";
	return oss.str();
}

namespace PrimitiveDataTypeMod {
	std::ostream & operator << ( std::ostream & out, const PrimitiveDataType& value) {
		out << CPrimitiveDataType::name(value);
		return out;
	}

	std::istream & operator >> ( std::istream & in , PrimitiveDataType& value ) {
		in.clear();
		string s ; 
  		in >> s;
  		try {
    		value = CPrimitiveDataType::literal(s);
  		}
  		catch (string & m) {
    		in.setstate(ios::failbit);
  		}
  		return in;
  	}
}

