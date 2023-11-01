
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
 * File CFieldCode.cpp
 */
#include <sstream>
#include <alma/Enumerations/CFieldCode.h>
#include <string>
using namespace std;

int CFieldCode::version() {
	return FieldCodeMod::version;
	}
	
string CFieldCode::revision () {
	return FieldCodeMod::revision;
}

unsigned int CFieldCode::size() {
	return 1;
	}
	
	
const std::string& CFieldCode::sNONE = "NONE";
	
const std::vector<std::string> CFieldCode::names() {
    std::vector<std::string> enumSet;
    
    enumSet.insert(enumSet.end(), CFieldCode::sNONE);
        
    return enumSet;
}

std::string CFieldCode::name(const FieldCodeMod::FieldCode& f) {
    switch (f) {
    
    case FieldCodeMod::NONE:
      return CFieldCode::sNONE;
    	
    }
    // Impossible siutation but....who knows with C++ enums
    throw badInt((int) f);
}

FieldCodeMod::FieldCode CFieldCode::newFieldCode(const std::string& name) {
		
    if (name == CFieldCode::sNONE) {
        return FieldCodeMod::NONE;
    }
    
    throw badString(name);
}

FieldCodeMod::FieldCode CFieldCode::literal(const std::string& name) {
		
    if (name == CFieldCode::sNONE) {
        return FieldCodeMod::NONE;
    }
    
    throw badString(name);
}

FieldCodeMod::FieldCode CFieldCode::from_int(unsigned int i) {
	vector<string> names_ = names();
	if (i >= names_.size()) throw badInt(i);
	return newFieldCode(names_.at(i));
}

string CFieldCode::badString(const string& name) {
	return "'"+name+"' does not correspond to any literal in the enumeration 'FieldCode'.";
}

string CFieldCode::badInt(unsigned int i) {
	ostringstream oss ;
	oss << "'" << i << "' is out of range for the enumeration 'FieldCode'.";
	return oss.str();
}

namespace FieldCodeMod {
	std::ostream & operator << ( std::ostream & out, const FieldCode& value) {
		out << CFieldCode::name(value);
		return out;
	}

	std::istream & operator >> ( std::istream & in , FieldCode& value ) {
		in.clear();
		string s ; 
  		in >> s;
  		try {
    		value = CFieldCode::literal(s);
  		}
  		catch (string & m) {
    		in.setstate(ios::failbit);
  		}
  		return in;
  	}
}

