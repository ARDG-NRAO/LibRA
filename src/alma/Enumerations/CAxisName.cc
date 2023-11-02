
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
 * File CAxisName.cpp
 */
#include <sstream>
#include <alma/Enumerations/CAxisName.h>
#include <string>
using namespace std;

int CAxisName::version() {
	return AxisNameMod::version;
	}
	
string CAxisName::revision () {
	return AxisNameMod::revision;
}

unsigned int CAxisName::size() {
	return 13;
	}
	
	
const std::string& CAxisName::sTIM = "TIM";
	
const std::string& CAxisName::sBAL = "BAL";
	
const std::string& CAxisName::sANT = "ANT";
	
const std::string& CAxisName::sBAB = "BAB";
	
const std::string& CAxisName::sSPW = "SPW";
	
const std::string& CAxisName::sSIB = "SIB";
	
const std::string& CAxisName::sSUB = "SUB";
	
const std::string& CAxisName::sBIN = "BIN";
	
const std::string& CAxisName::sAPC = "APC";
	
const std::string& CAxisName::sSPP = "SPP";
	
const std::string& CAxisName::sPOL = "POL";
	
const std::string& CAxisName::sSTO = "STO";
	
const std::string& CAxisName::sHOL = "HOL";
	
const std::vector<std::string> CAxisName::names() {
    std::vector<std::string> enumSet;
    
    enumSet.insert(enumSet.end(), CAxisName::sTIM);
    
    enumSet.insert(enumSet.end(), CAxisName::sBAL);
    
    enumSet.insert(enumSet.end(), CAxisName::sANT);
    
    enumSet.insert(enumSet.end(), CAxisName::sBAB);
    
    enumSet.insert(enumSet.end(), CAxisName::sSPW);
    
    enumSet.insert(enumSet.end(), CAxisName::sSIB);
    
    enumSet.insert(enumSet.end(), CAxisName::sSUB);
    
    enumSet.insert(enumSet.end(), CAxisName::sBIN);
    
    enumSet.insert(enumSet.end(), CAxisName::sAPC);
    
    enumSet.insert(enumSet.end(), CAxisName::sSPP);
    
    enumSet.insert(enumSet.end(), CAxisName::sPOL);
    
    enumSet.insert(enumSet.end(), CAxisName::sSTO);
    
    enumSet.insert(enumSet.end(), CAxisName::sHOL);
        
    return enumSet;
}

std::string CAxisName::name(const AxisNameMod::AxisName& f) {
    switch (f) {
    
    case AxisNameMod::TIM:
      return CAxisName::sTIM;
    
    case AxisNameMod::BAL:
      return CAxisName::sBAL;
    
    case AxisNameMod::ANT:
      return CAxisName::sANT;
    
    case AxisNameMod::BAB:
      return CAxisName::sBAB;
    
    case AxisNameMod::SPW:
      return CAxisName::sSPW;
    
    case AxisNameMod::SIB:
      return CAxisName::sSIB;
    
    case AxisNameMod::SUB:
      return CAxisName::sSUB;
    
    case AxisNameMod::BIN:
      return CAxisName::sBIN;
    
    case AxisNameMod::APC:
      return CAxisName::sAPC;
    
    case AxisNameMod::SPP:
      return CAxisName::sSPP;
    
    case AxisNameMod::POL:
      return CAxisName::sPOL;
    
    case AxisNameMod::STO:
      return CAxisName::sSTO;
    
    case AxisNameMod::HOL:
      return CAxisName::sHOL;
    	
    }
    // Impossible siutation but....who knows with C++ enums
    throw badInt((int) f);
}

AxisNameMod::AxisName CAxisName::newAxisName(const std::string& name) {
		
    if (name == CAxisName::sTIM) {
        return AxisNameMod::TIM;
    }
    	
    if (name == CAxisName::sBAL) {
        return AxisNameMod::BAL;
    }
    	
    if (name == CAxisName::sANT) {
        return AxisNameMod::ANT;
    }
    	
    if (name == CAxisName::sBAB) {
        return AxisNameMod::BAB;
    }
    	
    if (name == CAxisName::sSPW) {
        return AxisNameMod::SPW;
    }
    	
    if (name == CAxisName::sSIB) {
        return AxisNameMod::SIB;
    }
    	
    if (name == CAxisName::sSUB) {
        return AxisNameMod::SUB;
    }
    	
    if (name == CAxisName::sBIN) {
        return AxisNameMod::BIN;
    }
    	
    if (name == CAxisName::sAPC) {
        return AxisNameMod::APC;
    }
    	
    if (name == CAxisName::sSPP) {
        return AxisNameMod::SPP;
    }
    	
    if (name == CAxisName::sPOL) {
        return AxisNameMod::POL;
    }
    	
    if (name == CAxisName::sSTO) {
        return AxisNameMod::STO;
    }
    	
    if (name == CAxisName::sHOL) {
        return AxisNameMod::HOL;
    }
    
    throw badString(name);
}

AxisNameMod::AxisName CAxisName::literal(const std::string& name) {
		
    if (name == CAxisName::sTIM) {
        return AxisNameMod::TIM;
    }
    	
    if (name == CAxisName::sBAL) {
        return AxisNameMod::BAL;
    }
    	
    if (name == CAxisName::sANT) {
        return AxisNameMod::ANT;
    }
    	
    if (name == CAxisName::sBAB) {
        return AxisNameMod::BAB;
    }
    	
    if (name == CAxisName::sSPW) {
        return AxisNameMod::SPW;
    }
    	
    if (name == CAxisName::sSIB) {
        return AxisNameMod::SIB;
    }
    	
    if (name == CAxisName::sSUB) {
        return AxisNameMod::SUB;
    }
    	
    if (name == CAxisName::sBIN) {
        return AxisNameMod::BIN;
    }
    	
    if (name == CAxisName::sAPC) {
        return AxisNameMod::APC;
    }
    	
    if (name == CAxisName::sSPP) {
        return AxisNameMod::SPP;
    }
    	
    if (name == CAxisName::sPOL) {
        return AxisNameMod::POL;
    }
    	
    if (name == CAxisName::sSTO) {
        return AxisNameMod::STO;
    }
    	
    if (name == CAxisName::sHOL) {
        return AxisNameMod::HOL;
    }
    
    throw badString(name);
}

AxisNameMod::AxisName CAxisName::from_int(unsigned int i) {
	vector<string> names_ = names();
	if (i >= names_.size()) throw badInt(i);
	return newAxisName(names_.at(i));
}

string CAxisName::badString(const string& name) {
	return "'"+name+"' does not correspond to any literal in the enumeration 'AxisName'.";
}

string CAxisName::badInt(unsigned int i) {
	ostringstream oss ;
	oss << "'" << i << "' is out of range for the enumeration 'AxisName'.";
	return oss.str();
}

namespace AxisNameMod {
	std::ostream & operator << ( std::ostream & out, const AxisName& value) {
		out << CAxisName::name(value);
		return out;
	}

	std::istream & operator >> ( std::istream & in , AxisName& value ) {
		in.clear();
		string s ; 
  		in >> s;
  		try {
    		value = CAxisName::literal(s);
  		}
  		catch (string & m) {
    		in.setstate(ios::failbit);
  		}
  		return in;
  	}
}

