
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
 * File CDifferenceType.cpp
 */
#include <sstream>
#include <alma/Enumerations/CDifferenceType.h>
#include <string>
using namespace std;

int CDifferenceType::version() {
	return DifferenceTypeMod::version;
	}
	
string CDifferenceType::revision () {
	return DifferenceTypeMod::revision;
}

unsigned int CDifferenceType::size() {
	return 4;
	}
	
	
const std::string& CDifferenceType::sPREDICTED = "PREDICTED";
	
const std::string& CDifferenceType::sPRELIMINARY = "PRELIMINARY";
	
const std::string& CDifferenceType::sRAPID = "RAPID";
	
const std::string& CDifferenceType::sFINAL = "FINAL";
	
const std::vector<std::string> CDifferenceType::names() {
    std::vector<std::string> enumSet;
    
    enumSet.insert(enumSet.end(), CDifferenceType::sPREDICTED);
    
    enumSet.insert(enumSet.end(), CDifferenceType::sPRELIMINARY);
    
    enumSet.insert(enumSet.end(), CDifferenceType::sRAPID);
    
    enumSet.insert(enumSet.end(), CDifferenceType::sFINAL);
        
    return enumSet;
}

std::string CDifferenceType::name(const DifferenceTypeMod::DifferenceType& f) {
    switch (f) {
    
    case DifferenceTypeMod::PREDICTED:
      return CDifferenceType::sPREDICTED;
    
    case DifferenceTypeMod::PRELIMINARY:
      return CDifferenceType::sPRELIMINARY;
    
    case DifferenceTypeMod::RAPID:
      return CDifferenceType::sRAPID;
    
    case DifferenceTypeMod::FINAL:
      return CDifferenceType::sFINAL;
    	
    }
    // Impossible siutation but....who knows with C++ enums
    throw badInt((int) f);
}

DifferenceTypeMod::DifferenceType CDifferenceType::newDifferenceType(const std::string& name) {
		
    if (name == CDifferenceType::sPREDICTED) {
        return DifferenceTypeMod::PREDICTED;
    }
    	
    if (name == CDifferenceType::sPRELIMINARY) {
        return DifferenceTypeMod::PRELIMINARY;
    }
    	
    if (name == CDifferenceType::sRAPID) {
        return DifferenceTypeMod::RAPID;
    }
    	
    if (name == CDifferenceType::sFINAL) {
        return DifferenceTypeMod::FINAL;
    }
    
    throw badString(name);
}

DifferenceTypeMod::DifferenceType CDifferenceType::literal(const std::string& name) {
		
    if (name == CDifferenceType::sPREDICTED) {
        return DifferenceTypeMod::PREDICTED;
    }
    	
    if (name == CDifferenceType::sPRELIMINARY) {
        return DifferenceTypeMod::PRELIMINARY;
    }
    	
    if (name == CDifferenceType::sRAPID) {
        return DifferenceTypeMod::RAPID;
    }
    	
    if (name == CDifferenceType::sFINAL) {
        return DifferenceTypeMod::FINAL;
    }
    
    throw badString(name);
}

DifferenceTypeMod::DifferenceType CDifferenceType::from_int(unsigned int i) {
	vector<string> names_ = names();
	if (i >= names_.size()) throw badInt(i);
	return newDifferenceType(names_.at(i));
}

string CDifferenceType::badString(const string& name) {
	return "'"+name+"' does not correspond to any literal in the enumeration 'DifferenceType'.";
}

string CDifferenceType::badInt(unsigned int i) {
	ostringstream oss ;
	oss << "'" << i << "' is out of range for the enumeration 'DifferenceType'.";
	return oss.str();
}

namespace DifferenceTypeMod {
	std::ostream & operator << ( std::ostream & out, const DifferenceType& value) {
		out << CDifferenceType::name(value);
		return out;
	}

	std::istream & operator >> ( std::istream & in , DifferenceType& value ) {
		in.clear();
		string s ; 
  		in >> s;
  		try {
    		value = CDifferenceType::literal(s);
  		}
  		catch (string & m) {
    		in.setstate(ios::failbit);
  		}
  		return in;
  	}
}

