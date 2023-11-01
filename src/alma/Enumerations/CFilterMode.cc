
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
 * File CFilterMode.cpp
 */
#include <sstream>
#include <alma/Enumerations/CFilterMode.h>
#include <string>
using namespace std;

int CFilterMode::version() {
	return FilterModeMod::version;
	}
	
string CFilterMode::revision () {
	return FilterModeMod::revision;
}

unsigned int CFilterMode::size() {
	return 4;
	}
	
	
const std::string& CFilterMode::sFILTER_NA = "FILTER_NA";
	
const std::string& CFilterMode::sFILTER_TDM = "FILTER_TDM";
	
const std::string& CFilterMode::sFILTER_TFB = "FILTER_TFB";
	
const std::string& CFilterMode::sUNDEFINED = "UNDEFINED";
	
const std::vector<std::string> CFilterMode::names() {
    std::vector<std::string> enumSet;
    
    enumSet.insert(enumSet.end(), CFilterMode::sFILTER_NA);
    
    enumSet.insert(enumSet.end(), CFilterMode::sFILTER_TDM);
    
    enumSet.insert(enumSet.end(), CFilterMode::sFILTER_TFB);
    
    enumSet.insert(enumSet.end(), CFilterMode::sUNDEFINED);
        
    return enumSet;
}

std::string CFilterMode::name(const FilterModeMod::FilterMode& f) {
    switch (f) {
    
    case FilterModeMod::FILTER_NA:
      return CFilterMode::sFILTER_NA;
    
    case FilterModeMod::FILTER_TDM:
      return CFilterMode::sFILTER_TDM;
    
    case FilterModeMod::FILTER_TFB:
      return CFilterMode::sFILTER_TFB;
    
    case FilterModeMod::UNDEFINED:
      return CFilterMode::sUNDEFINED;
    	
    }
    // Impossible siutation but....who knows with C++ enums
    throw badInt((int) f);
}

FilterModeMod::FilterMode CFilterMode::newFilterMode(const std::string& name) {
		
    if (name == CFilterMode::sFILTER_NA) {
        return FilterModeMod::FILTER_NA;
    }
    	
    if (name == CFilterMode::sFILTER_TDM) {
        return FilterModeMod::FILTER_TDM;
    }
    	
    if (name == CFilterMode::sFILTER_TFB) {
        return FilterModeMod::FILTER_TFB;
    }
    	
    if (name == CFilterMode::sUNDEFINED) {
        return FilterModeMod::UNDEFINED;
    }
    
    throw badString(name);
}

FilterModeMod::FilterMode CFilterMode::literal(const std::string& name) {
		
    if (name == CFilterMode::sFILTER_NA) {
        return FilterModeMod::FILTER_NA;
    }
    	
    if (name == CFilterMode::sFILTER_TDM) {
        return FilterModeMod::FILTER_TDM;
    }
    	
    if (name == CFilterMode::sFILTER_TFB) {
        return FilterModeMod::FILTER_TFB;
    }
    	
    if (name == CFilterMode::sUNDEFINED) {
        return FilterModeMod::UNDEFINED;
    }
    
    throw badString(name);
}

FilterModeMod::FilterMode CFilterMode::from_int(unsigned int i) {
	vector<string> names_ = names();
	if (i >= names_.size()) throw badInt(i);
	return newFilterMode(names_.at(i));
}

string CFilterMode::badString(const string& name) {
	return "'"+name+"' does not correspond to any literal in the enumeration 'FilterMode'.";
}

string CFilterMode::badInt(unsigned int i) {
	ostringstream oss ;
	oss << "'" << i << "' is out of range for the enumeration 'FilterMode'.";
	return oss.str();
}

namespace FilterModeMod {
	std::ostream & operator << ( std::ostream & out, const FilterMode& value) {
		out << CFilterMode::name(value);
		return out;
	}

	std::istream & operator >> ( std::istream & in , FilterMode& value ) {
		in.clear();
		string s ; 
  		in >> s;
  		try {
    		value = CFilterMode::literal(s);
  		}
  		catch (string & m) {
    		in.setstate(ios::failbit);
  		}
  		return in;
  	}
}

