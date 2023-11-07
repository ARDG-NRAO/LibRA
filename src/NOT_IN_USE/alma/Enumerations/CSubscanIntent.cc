
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
 * File CSubscanIntent.cpp
 */
#include <sstream>
#include <alma/Enumerations/CSubscanIntent.h>
#include <string>
using namespace std;

int CSubscanIntent::version() {
	return SubscanIntentMod::version;
	}
	
string CSubscanIntent::revision () {
	return SubscanIntentMod::revision;
}

unsigned int CSubscanIntent::size() {
	return 11;
	}
	
	
const std::string& CSubscanIntent::sON_SOURCE = "ON_SOURCE";
	
const std::string& CSubscanIntent::sOFF_SOURCE = "OFF_SOURCE";
	
const std::string& CSubscanIntent::sMIXED = "MIXED";
	
const std::string& CSubscanIntent::sREFERENCE = "REFERENCE";
	
const std::string& CSubscanIntent::sSCANNING = "SCANNING";
	
const std::string& CSubscanIntent::sHOT = "HOT";
	
const std::string& CSubscanIntent::sAMBIENT = "AMBIENT";
	
const std::string& CSubscanIntent::sSIGNAL = "SIGNAL";
	
const std::string& CSubscanIntent::sIMAGE = "IMAGE";
	
const std::string& CSubscanIntent::sTEST = "TEST";
	
const std::string& CSubscanIntent::sUNSPECIFIED = "UNSPECIFIED";
	
const std::vector<std::string> CSubscanIntent::names() {
    std::vector<std::string> enumSet;
    
    enumSet.insert(enumSet.end(), CSubscanIntent::sON_SOURCE);
    
    enumSet.insert(enumSet.end(), CSubscanIntent::sOFF_SOURCE);
    
    enumSet.insert(enumSet.end(), CSubscanIntent::sMIXED);
    
    enumSet.insert(enumSet.end(), CSubscanIntent::sREFERENCE);
    
    enumSet.insert(enumSet.end(), CSubscanIntent::sSCANNING);
    
    enumSet.insert(enumSet.end(), CSubscanIntent::sHOT);
    
    enumSet.insert(enumSet.end(), CSubscanIntent::sAMBIENT);
    
    enumSet.insert(enumSet.end(), CSubscanIntent::sSIGNAL);
    
    enumSet.insert(enumSet.end(), CSubscanIntent::sIMAGE);
    
    enumSet.insert(enumSet.end(), CSubscanIntent::sTEST);
    
    enumSet.insert(enumSet.end(), CSubscanIntent::sUNSPECIFIED);
        
    return enumSet;
}

std::string CSubscanIntent::name(const SubscanIntentMod::SubscanIntent& f) {
    switch (f) {
    
    case SubscanIntentMod::ON_SOURCE:
      return CSubscanIntent::sON_SOURCE;
    
    case SubscanIntentMod::OFF_SOURCE:
      return CSubscanIntent::sOFF_SOURCE;
    
    case SubscanIntentMod::MIXED:
      return CSubscanIntent::sMIXED;
    
    case SubscanIntentMod::REFERENCE:
      return CSubscanIntent::sREFERENCE;
    
    case SubscanIntentMod::SCANNING:
      return CSubscanIntent::sSCANNING;
    
    case SubscanIntentMod::HOT:
      return CSubscanIntent::sHOT;
    
    case SubscanIntentMod::AMBIENT:
      return CSubscanIntent::sAMBIENT;
    
    case SubscanIntentMod::SIGNAL:
      return CSubscanIntent::sSIGNAL;
    
    case SubscanIntentMod::IMAGE:
      return CSubscanIntent::sIMAGE;
    
    case SubscanIntentMod::TEST:
      return CSubscanIntent::sTEST;
    
    case SubscanIntentMod::UNSPECIFIED:
      return CSubscanIntent::sUNSPECIFIED;
    	
    }
    // Impossible siutation but....who knows with C++ enums
    throw badInt((int) f);
}

SubscanIntentMod::SubscanIntent CSubscanIntent::newSubscanIntent(const std::string& name) {
		
    if (name == CSubscanIntent::sON_SOURCE) {
        return SubscanIntentMod::ON_SOURCE;
    }
    	
    if (name == CSubscanIntent::sOFF_SOURCE) {
        return SubscanIntentMod::OFF_SOURCE;
    }
    	
    if (name == CSubscanIntent::sMIXED) {
        return SubscanIntentMod::MIXED;
    }
    	
    if (name == CSubscanIntent::sREFERENCE) {
        return SubscanIntentMod::REFERENCE;
    }
    	
    if (name == CSubscanIntent::sSCANNING) {
        return SubscanIntentMod::SCANNING;
    }
    	
    if (name == CSubscanIntent::sHOT) {
        return SubscanIntentMod::HOT;
    }
    	
    if (name == CSubscanIntent::sAMBIENT) {
        return SubscanIntentMod::AMBIENT;
    }
    	
    if (name == CSubscanIntent::sSIGNAL) {
        return SubscanIntentMod::SIGNAL;
    }
    	
    if (name == CSubscanIntent::sIMAGE) {
        return SubscanIntentMod::IMAGE;
    }
    	
    if (name == CSubscanIntent::sTEST) {
        return SubscanIntentMod::TEST;
    }
    	
    if (name == CSubscanIntent::sUNSPECIFIED) {
        return SubscanIntentMod::UNSPECIFIED;
    }
    
    throw badString(name);
}

SubscanIntentMod::SubscanIntent CSubscanIntent::literal(const std::string& name) {
		
    if (name == CSubscanIntent::sON_SOURCE) {
        return SubscanIntentMod::ON_SOURCE;
    }
    	
    if (name == CSubscanIntent::sOFF_SOURCE) {
        return SubscanIntentMod::OFF_SOURCE;
    }
    	
    if (name == CSubscanIntent::sMIXED) {
        return SubscanIntentMod::MIXED;
    }
    	
    if (name == CSubscanIntent::sREFERENCE) {
        return SubscanIntentMod::REFERENCE;
    }
    	
    if (name == CSubscanIntent::sSCANNING) {
        return SubscanIntentMod::SCANNING;
    }
    	
    if (name == CSubscanIntent::sHOT) {
        return SubscanIntentMod::HOT;
    }
    	
    if (name == CSubscanIntent::sAMBIENT) {
        return SubscanIntentMod::AMBIENT;
    }
    	
    if (name == CSubscanIntent::sSIGNAL) {
        return SubscanIntentMod::SIGNAL;
    }
    	
    if (name == CSubscanIntent::sIMAGE) {
        return SubscanIntentMod::IMAGE;
    }
    	
    if (name == CSubscanIntent::sTEST) {
        return SubscanIntentMod::TEST;
    }
    	
    if (name == CSubscanIntent::sUNSPECIFIED) {
        return SubscanIntentMod::UNSPECIFIED;
    }
    
    throw badString(name);
}

SubscanIntentMod::SubscanIntent CSubscanIntent::from_int(unsigned int i) {
	vector<string> names_ = names();
	if (i >= names_.size()) throw badInt(i);
	return newSubscanIntent(names_.at(i));
}

string CSubscanIntent::badString(const string& name) {
	return "'"+name+"' does not correspond to any literal in the enumeration 'SubscanIntent'.";
}

string CSubscanIntent::badInt(unsigned int i) {
	ostringstream oss ;
	oss << "'" << i << "' is out of range for the enumeration 'SubscanIntent'.";
	return oss.str();
}

namespace SubscanIntentMod {
	std::ostream & operator << ( std::ostream & out, const SubscanIntent& value) {
		out << CSubscanIntent::name(value);
		return out;
	}

	std::istream & operator >> ( std::istream & in , SubscanIntent& value ) {
		in.clear();
		string s ; 
  		in >> s;
  		try {
    		value = CSubscanIntent::literal(s);
  		}
  		catch (string & m) {
    		in.setstate(ios::failbit);
  		}
  		return in;
  	}
}

