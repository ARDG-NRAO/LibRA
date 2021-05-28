
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
 * File CPrimaryBeamDescription.cpp
 */
#include <sstream>
#include <alma/Enumerations/CPrimaryBeamDescription.h>
#include <string>
using namespace std;

int CPrimaryBeamDescription::version() {
	return PrimaryBeamDescriptionMod::version;
	}
	
string CPrimaryBeamDescription::revision () {
	return PrimaryBeamDescriptionMod::revision;
}

unsigned int CPrimaryBeamDescription::size() {
	return 2;
	}
	
	
const std::string& CPrimaryBeamDescription::sCOMPLEX_FIELD_PATTERN = "COMPLEX_FIELD_PATTERN";
	
const std::string& CPrimaryBeamDescription::sAPERTURE_FIELD_DISTRIBUTION = "APERTURE_FIELD_DISTRIBUTION";
	
const std::vector<std::string> CPrimaryBeamDescription::names() {
    std::vector<std::string> enumSet;
    
    enumSet.insert(enumSet.end(), CPrimaryBeamDescription::sCOMPLEX_FIELD_PATTERN);
    
    enumSet.insert(enumSet.end(), CPrimaryBeamDescription::sAPERTURE_FIELD_DISTRIBUTION);
        
    return enumSet;
}

std::string CPrimaryBeamDescription::name(const PrimaryBeamDescriptionMod::PrimaryBeamDescription& f) {
    switch (f) {
    
    case PrimaryBeamDescriptionMod::COMPLEX_FIELD_PATTERN:
      return CPrimaryBeamDescription::sCOMPLEX_FIELD_PATTERN;
    
    case PrimaryBeamDescriptionMod::APERTURE_FIELD_DISTRIBUTION:
      return CPrimaryBeamDescription::sAPERTURE_FIELD_DISTRIBUTION;
    	
    }
    // Impossible siutation but....who knows with C++ enums
    throw badInt((int) f);
}

PrimaryBeamDescriptionMod::PrimaryBeamDescription CPrimaryBeamDescription::newPrimaryBeamDescription(const std::string& name) {
		
    if (name == CPrimaryBeamDescription::sCOMPLEX_FIELD_PATTERN) {
        return PrimaryBeamDescriptionMod::COMPLEX_FIELD_PATTERN;
    }
    	
    if (name == CPrimaryBeamDescription::sAPERTURE_FIELD_DISTRIBUTION) {
        return PrimaryBeamDescriptionMod::APERTURE_FIELD_DISTRIBUTION;
    }
    
    throw badString(name);
}

PrimaryBeamDescriptionMod::PrimaryBeamDescription CPrimaryBeamDescription::literal(const std::string& name) {
		
    if (name == CPrimaryBeamDescription::sCOMPLEX_FIELD_PATTERN) {
        return PrimaryBeamDescriptionMod::COMPLEX_FIELD_PATTERN;
    }
    	
    if (name == CPrimaryBeamDescription::sAPERTURE_FIELD_DISTRIBUTION) {
        return PrimaryBeamDescriptionMod::APERTURE_FIELD_DISTRIBUTION;
    }
    
    throw badString(name);
}

PrimaryBeamDescriptionMod::PrimaryBeamDescription CPrimaryBeamDescription::from_int(unsigned int i) {
	vector<string> names_ = names();
	if (i >= names_.size()) throw badInt(i);
	return newPrimaryBeamDescription(names_.at(i));
}

string CPrimaryBeamDescription::badString(const string& name) {
	return "'"+name+"' does not correspond to any literal in the enumeration 'PrimaryBeamDescription'.";
}

string CPrimaryBeamDescription::badInt(unsigned int i) {
	ostringstream oss ;
	oss << "'" << i << "' is out of range for the enumeration 'PrimaryBeamDescription'.";
	return oss.str();
}

namespace PrimaryBeamDescriptionMod {
	std::ostream & operator << ( std::ostream & out, const PrimaryBeamDescription& value) {
		out << CPrimaryBeamDescription::name(value);
		return out;
	}

	std::istream & operator >> ( std::istream & in , PrimaryBeamDescription& value ) {
		in.clear();
		string s ; 
  		in >> s;
  		try {
    		value = CPrimaryBeamDescription::literal(s);
  		}
  		catch (string & m) {
    		in.setstate(ios::failbit);
  		}
  		return in;
  	}
}

