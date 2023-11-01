
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
 * File CAccumMode.cpp
 */
#include <sstream>
#include <alma/Enumerations/CAccumMode.h>
#include <string>
using namespace std;

int CAccumMode::version() {
	return AccumModeMod::version;
	}
	
string CAccumMode::revision () {
	return AccumModeMod::revision;
}

unsigned int CAccumMode::size() {
	return 3;
	}
	
	
const std::string& CAccumMode::sFAST = "FAST";
	
const std::string& CAccumMode::sNORMAL = "NORMAL";
	
const std::string& CAccumMode::sUNDEFINED = "UNDEFINED";
	
const std::vector<std::string> CAccumMode::names() {
    std::vector<std::string> enumSet;
    
    enumSet.insert(enumSet.end(), CAccumMode::sFAST);
    
    enumSet.insert(enumSet.end(), CAccumMode::sNORMAL);
    
    enumSet.insert(enumSet.end(), CAccumMode::sUNDEFINED);
        
    return enumSet;
}

std::string CAccumMode::name(const AccumModeMod::AccumMode& f) {
    switch (f) {
    
    case AccumModeMod::FAST:
      return CAccumMode::sFAST;
    
    case AccumModeMod::NORMAL:
      return CAccumMode::sNORMAL;
    
    case AccumModeMod::UNDEFINED:
      return CAccumMode::sUNDEFINED;
    	
    }
    // Impossible siutation but....who knows with C++ enums
    throw badInt((int) f);
}

AccumModeMod::AccumMode CAccumMode::newAccumMode(const std::string& name) {
		
    if (name == CAccumMode::sFAST) {
        return AccumModeMod::FAST;
    }
    	
    if (name == CAccumMode::sNORMAL) {
        return AccumModeMod::NORMAL;
    }
    	
    if (name == CAccumMode::sUNDEFINED) {
        return AccumModeMod::UNDEFINED;
    }
    
    throw badString(name);
}

AccumModeMod::AccumMode CAccumMode::literal(const std::string& name) {
		
    if (name == CAccumMode::sFAST) {
        return AccumModeMod::FAST;
    }
    	
    if (name == CAccumMode::sNORMAL) {
        return AccumModeMod::NORMAL;
    }
    	
    if (name == CAccumMode::sUNDEFINED) {
        return AccumModeMod::UNDEFINED;
    }
    
    throw badString(name);
}

AccumModeMod::AccumMode CAccumMode::from_int(unsigned int i) {
	vector<string> names_ = names();
	if (i >= names_.size()) throw badInt(i);
	return newAccumMode(names_.at(i));
}

string CAccumMode::badString(const string& name) {
	return "'"+name+"' does not correspond to any literal in the enumeration 'AccumMode'.";
}

string CAccumMode::badInt(unsigned int i) {
	ostringstream oss ;
	oss << "'" << i << "' is out of range for the enumeration 'AccumMode'.";
	return oss.str();
}

namespace AccumModeMod {
	std::ostream & operator << ( std::ostream & out, const AccumMode& value) {
		out << CAccumMode::name(value);
		return out;
	}

	std::istream & operator >> ( std::istream & in , AccumMode& value ) {
		in.clear();
		string s ; 
  		in >> s;
  		try {
    		value = CAccumMode::literal(s);
  		}
  		catch (string & m) {
    		in.setstate(ios::failbit);
  		}
  		return in;
  	}
}

