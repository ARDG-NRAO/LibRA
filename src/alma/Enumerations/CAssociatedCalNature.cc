
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
 * File CAssociatedCalNature.cpp
 */
#include <sstream>
#include <alma/Enumerations/CAssociatedCalNature.h>
#include <string>
using namespace std;

int CAssociatedCalNature::version() {
	return AssociatedCalNatureMod::version;
	}
	
string CAssociatedCalNature::revision () {
	return AssociatedCalNatureMod::revision;
}

unsigned int CAssociatedCalNature::size() {
	return 1;
	}
	
	
const std::string& CAssociatedCalNature::sASSOCIATED_EXECBLOCK = "ASSOCIATED_EXECBLOCK";
	
const std::vector<std::string> CAssociatedCalNature::names() {
    std::vector<std::string> enumSet;
    
    enumSet.insert(enumSet.end(), CAssociatedCalNature::sASSOCIATED_EXECBLOCK);
        
    return enumSet;
}

std::string CAssociatedCalNature::name(const AssociatedCalNatureMod::AssociatedCalNature& f) {
    switch (f) {
    
    case AssociatedCalNatureMod::ASSOCIATED_EXECBLOCK:
      return CAssociatedCalNature::sASSOCIATED_EXECBLOCK;
    	
    }
    // Impossible siutation but....who knows with C++ enums
    throw badInt((int) f);
}

AssociatedCalNatureMod::AssociatedCalNature CAssociatedCalNature::newAssociatedCalNature(const std::string& name) {
		
    if (name == CAssociatedCalNature::sASSOCIATED_EXECBLOCK) {
        return AssociatedCalNatureMod::ASSOCIATED_EXECBLOCK;
    }
    
    throw badString(name);
}

AssociatedCalNatureMod::AssociatedCalNature CAssociatedCalNature::literal(const std::string& name) {
		
    if (name == CAssociatedCalNature::sASSOCIATED_EXECBLOCK) {
        return AssociatedCalNatureMod::ASSOCIATED_EXECBLOCK;
    }
    
    throw badString(name);
}

AssociatedCalNatureMod::AssociatedCalNature CAssociatedCalNature::from_int(unsigned int i) {
	vector<string> names_ = names();
	if (i >= names_.size()) throw badInt(i);
	return newAssociatedCalNature(names_.at(i));
}

string CAssociatedCalNature::badString(const string& name) {
	return "'"+name+"' does not correspond to any literal in the enumeration 'AssociatedCalNature'.";
}

string CAssociatedCalNature::badInt(unsigned int i) {
	ostringstream oss ;
	oss << "'" << i << "' is out of range for the enumeration 'AssociatedCalNature'.";
	return oss.str();
}

namespace AssociatedCalNatureMod {
	std::ostream & operator << ( std::ostream & out, const AssociatedCalNature& value) {
		out << CAssociatedCalNature::name(value);
		return out;
	}

	std::istream & operator >> ( std::istream & in , AssociatedCalNature& value ) {
		in.clear();
		string s ; 
  		in >> s;
  		try {
    		value = CAssociatedCalNature::literal(s);
  		}
  		catch (string & m) {
    		in.setstate(ios::failbit);
  		}
  		return in;
  	}
}

