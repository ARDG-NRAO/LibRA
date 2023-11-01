
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
 * File CPointingModelMode.cpp
 */
#include <sstream>
#include <alma/Enumerations/CPointingModelMode.h>
#include <string>
using namespace std;

int CPointingModelMode::version() {
	return PointingModelModeMod::version;
	}
	
string CPointingModelMode::revision () {
	return PointingModelModeMod::revision;
}

unsigned int CPointingModelMode::size() {
	return 2;
	}
	
	
const std::string& CPointingModelMode::sRADIO = "RADIO";
	
const std::string& CPointingModelMode::sOPTICAL = "OPTICAL";
	
const std::vector<std::string> CPointingModelMode::names() {
    std::vector<std::string> enumSet;
    
    enumSet.insert(enumSet.end(), CPointingModelMode::sRADIO);
    
    enumSet.insert(enumSet.end(), CPointingModelMode::sOPTICAL);
        
    return enumSet;
}

std::string CPointingModelMode::name(const PointingModelModeMod::PointingModelMode& f) {
    switch (f) {
    
    case PointingModelModeMod::RADIO:
      return CPointingModelMode::sRADIO;
    
    case PointingModelModeMod::OPTICAL:
      return CPointingModelMode::sOPTICAL;
    	
    }
    // Impossible siutation but....who knows with C++ enums
    throw badInt((int) f);
}

PointingModelModeMod::PointingModelMode CPointingModelMode::newPointingModelMode(const std::string& name) {
		
    if (name == CPointingModelMode::sRADIO) {
        return PointingModelModeMod::RADIO;
    }
    	
    if (name == CPointingModelMode::sOPTICAL) {
        return PointingModelModeMod::OPTICAL;
    }
    
    throw badString(name);
}

PointingModelModeMod::PointingModelMode CPointingModelMode::literal(const std::string& name) {
		
    if (name == CPointingModelMode::sRADIO) {
        return PointingModelModeMod::RADIO;
    }
    	
    if (name == CPointingModelMode::sOPTICAL) {
        return PointingModelModeMod::OPTICAL;
    }
    
    throw badString(name);
}

PointingModelModeMod::PointingModelMode CPointingModelMode::from_int(unsigned int i) {
	vector<string> names_ = names();
	if (i >= names_.size()) throw badInt(i);
	return newPointingModelMode(names_.at(i));
}

string CPointingModelMode::badString(const string& name) {
	return "'"+name+"' does not correspond to any literal in the enumeration 'PointingModelMode'.";
}

string CPointingModelMode::badInt(unsigned int i) {
	ostringstream oss ;
	oss << "'" << i << "' is out of range for the enumeration 'PointingModelMode'.";
	return oss.str();
}

namespace PointingModelModeMod {
	std::ostream & operator << ( std::ostream & out, const PointingModelMode& value) {
		out << CPointingModelMode::name(value);
		return out;
	}

	std::istream & operator >> ( std::istream & in , PointingModelMode& value ) {
		in.clear();
		string s ; 
  		in >> s;
  		try {
    		value = CPointingModelMode::literal(s);
  		}
  		catch (string & m) {
    		in.setstate(ios::failbit);
  		}
  		return in;
  	}
}

