
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
 * File CAssociatedFieldNature.cpp
 */
#include <sstream>
#include <alma/Enumerations/CAssociatedFieldNature.h>
#include <string>
using namespace std;

int CAssociatedFieldNature::version() {
	return AssociatedFieldNatureMod::version;
	}
	
string CAssociatedFieldNature::revision () {
	return AssociatedFieldNatureMod::revision;
}

unsigned int CAssociatedFieldNature::size() {
	return 3;
	}
	
	
const std::string& CAssociatedFieldNature::sON = "ON";
	
const std::string& CAssociatedFieldNature::sOFF = "OFF";
	
const std::string& CAssociatedFieldNature::sPHASE_REFERENCE = "PHASE_REFERENCE";
	
const std::vector<std::string> CAssociatedFieldNature::names() {
    std::vector<std::string> enumSet;
    
    enumSet.insert(enumSet.end(), CAssociatedFieldNature::sON);
    
    enumSet.insert(enumSet.end(), CAssociatedFieldNature::sOFF);
    
    enumSet.insert(enumSet.end(), CAssociatedFieldNature::sPHASE_REFERENCE);
        
    return enumSet;
}

std::string CAssociatedFieldNature::name(const AssociatedFieldNatureMod::AssociatedFieldNature& f) {
    switch (f) {
    
    case AssociatedFieldNatureMod::ON:
      return CAssociatedFieldNature::sON;
    
    case AssociatedFieldNatureMod::OFF:
      return CAssociatedFieldNature::sOFF;
    
    case AssociatedFieldNatureMod::PHASE_REFERENCE:
      return CAssociatedFieldNature::sPHASE_REFERENCE;
    	
    }
    // Impossible siutation but....who knows with C++ enums
    throw badInt((int) f);
}

AssociatedFieldNatureMod::AssociatedFieldNature CAssociatedFieldNature::newAssociatedFieldNature(const std::string& name) {
		
    if (name == CAssociatedFieldNature::sON) {
        return AssociatedFieldNatureMod::ON;
    }
    	
    if (name == CAssociatedFieldNature::sOFF) {
        return AssociatedFieldNatureMod::OFF;
    }
    	
    if (name == CAssociatedFieldNature::sPHASE_REFERENCE) {
        return AssociatedFieldNatureMod::PHASE_REFERENCE;
    }
    
    throw badString(name);
}

AssociatedFieldNatureMod::AssociatedFieldNature CAssociatedFieldNature::literal(const std::string& name) {
		
    if (name == CAssociatedFieldNature::sON) {
        return AssociatedFieldNatureMod::ON;
    }
    	
    if (name == CAssociatedFieldNature::sOFF) {
        return AssociatedFieldNatureMod::OFF;
    }
    	
    if (name == CAssociatedFieldNature::sPHASE_REFERENCE) {
        return AssociatedFieldNatureMod::PHASE_REFERENCE;
    }
    
    throw badString(name);
}

AssociatedFieldNatureMod::AssociatedFieldNature CAssociatedFieldNature::from_int(unsigned int i) {
	vector<string> names_ = names();
	if (i >= names_.size()) throw badInt(i);
	return newAssociatedFieldNature(names_.at(i));
}

string CAssociatedFieldNature::badString(const string& name) {
	return "'"+name+"' does not correspond to any literal in the enumeration 'AssociatedFieldNature'.";
}

string CAssociatedFieldNature::badInt(unsigned int i) {
	ostringstream oss ;
	oss << "'" << i << "' is out of range for the enumeration 'AssociatedFieldNature'.";
	return oss.str();
}

namespace AssociatedFieldNatureMod {
	std::ostream & operator << ( std::ostream & out, const AssociatedFieldNature& value) {
		out << CAssociatedFieldNature::name(value);
		return out;
	}

	std::istream & operator >> ( std::istream & in , AssociatedFieldNature& value ) {
		in.clear();
		string s ; 
  		in >> s;
  		try {
    		value = CAssociatedFieldNature::literal(s);
  		}
  		catch (string & m) {
    		in.setstate(ios::failbit);
  		}
  		return in;
  	}
}

