
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
 * File CSBType.cpp
 */
#include <sstream>
#include <alma/Enumerations/CSBType.h>
#include <string>
using namespace std;

int CSBType::version() {
	return SBTypeMod::version;
	}
	
string CSBType::revision () {
	return SBTypeMod::revision;
}

unsigned int CSBType::size() {
	return 3;
	}
	
	
const std::string& CSBType::sOBSERVATORY = "OBSERVATORY";
	
const std::string& CSBType::sOBSERVER = "OBSERVER";
	
const std::string& CSBType::sEXPERT = "EXPERT";
	
const std::vector<std::string> CSBType::names() {
    std::vector<std::string> enumSet;
    
    enumSet.insert(enumSet.end(), CSBType::sOBSERVATORY);
    
    enumSet.insert(enumSet.end(), CSBType::sOBSERVER);
    
    enumSet.insert(enumSet.end(), CSBType::sEXPERT);
        
    return enumSet;
}

std::string CSBType::name(const SBTypeMod::SBType& f) {
    switch (f) {
    
    case SBTypeMod::OBSERVATORY:
      return CSBType::sOBSERVATORY;
    
    case SBTypeMod::OBSERVER:
      return CSBType::sOBSERVER;
    
    case SBTypeMod::EXPERT:
      return CSBType::sEXPERT;
    	
    }
    // Impossible siutation but....who knows with C++ enums
    throw badInt((int) f);
}

SBTypeMod::SBType CSBType::newSBType(const std::string& name) {
		
    if (name == CSBType::sOBSERVATORY) {
        return SBTypeMod::OBSERVATORY;
    }
    	
    if (name == CSBType::sOBSERVER) {
        return SBTypeMod::OBSERVER;
    }
    	
    if (name == CSBType::sEXPERT) {
        return SBTypeMod::EXPERT;
    }
    
    throw badString(name);
}

SBTypeMod::SBType CSBType::literal(const std::string& name) {
		
    if (name == CSBType::sOBSERVATORY) {
        return SBTypeMod::OBSERVATORY;
    }
    	
    if (name == CSBType::sOBSERVER) {
        return SBTypeMod::OBSERVER;
    }
    	
    if (name == CSBType::sEXPERT) {
        return SBTypeMod::EXPERT;
    }
    
    throw badString(name);
}

SBTypeMod::SBType CSBType::from_int(unsigned int i) {
	vector<string> names_ = names();
	if (i >= names_.size()) throw badInt(i);
	return newSBType(names_.at(i));
}

string CSBType::badString(const string& name) {
	return "'"+name+"' does not correspond to any literal in the enumeration 'SBType'.";
}

string CSBType::badInt(unsigned int i) {
	ostringstream oss ;
	oss << "'" << i << "' is out of range for the enumeration 'SBType'.";
	return oss.str();
}

namespace SBTypeMod {
	std::ostream & operator << ( std::ostream & out, const SBType& value) {
		out << CSBType::name(value);
		return out;
	}

	std::istream & operator >> ( std::istream & in , SBType& value ) {
		in.clear();
		string s ; 
  		in >> s;
  		try {
    		value = CSBType::literal(s);
  		}
  		catch (string & m) {
    		in.setstate(ios::failbit);
  		}
  		return in;
  	}
}

