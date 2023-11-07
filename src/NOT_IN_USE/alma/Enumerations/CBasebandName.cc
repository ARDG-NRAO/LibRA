
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
 * File CBasebandName.cpp
 */
#include <sstream>
#include <alma/Enumerations/CBasebandName.h>
#include <string>
using namespace std;

int CBasebandName::version() {
	return BasebandNameMod::version;
	}
	
string CBasebandName::revision () {
	return BasebandNameMod::revision;
}

unsigned int CBasebandName::size() {
	return 16;
	}
	
	
const std::string& CBasebandName::sNOBB = "NOBB";
	
const std::string& CBasebandName::sBB_1 = "BB_1";
	
const std::string& CBasebandName::sBB_2 = "BB_2";
	
const std::string& CBasebandName::sBB_3 = "BB_3";
	
const std::string& CBasebandName::sBB_4 = "BB_4";
	
const std::string& CBasebandName::sBB_5 = "BB_5";
	
const std::string& CBasebandName::sBB_6 = "BB_6";
	
const std::string& CBasebandName::sBB_7 = "BB_7";
	
const std::string& CBasebandName::sBB_8 = "BB_8";
	
const std::string& CBasebandName::sBB_ALL = "BB_ALL";
	
const std::string& CBasebandName::sA1C1_3BIT = "A1C1_3BIT";
	
const std::string& CBasebandName::sA2C2_3BIT = "A2C2_3BIT";
	
const std::string& CBasebandName::sAC_8BIT = "AC_8BIT";
	
const std::string& CBasebandName::sB1D1_3BIT = "B1D1_3BIT";
	
const std::string& CBasebandName::sB2D2_3BIT = "B2D2_3BIT";
	
const std::string& CBasebandName::sBD_8BIT = "BD_8BIT";
	
const std::vector<std::string> CBasebandName::names() {
    std::vector<std::string> enumSet;
    
    enumSet.insert(enumSet.end(), CBasebandName::sNOBB);
    
    enumSet.insert(enumSet.end(), CBasebandName::sBB_1);
    
    enumSet.insert(enumSet.end(), CBasebandName::sBB_2);
    
    enumSet.insert(enumSet.end(), CBasebandName::sBB_3);
    
    enumSet.insert(enumSet.end(), CBasebandName::sBB_4);
    
    enumSet.insert(enumSet.end(), CBasebandName::sBB_5);
    
    enumSet.insert(enumSet.end(), CBasebandName::sBB_6);
    
    enumSet.insert(enumSet.end(), CBasebandName::sBB_7);
    
    enumSet.insert(enumSet.end(), CBasebandName::sBB_8);
    
    enumSet.insert(enumSet.end(), CBasebandName::sBB_ALL);
    
    enumSet.insert(enumSet.end(), CBasebandName::sA1C1_3BIT);
    
    enumSet.insert(enumSet.end(), CBasebandName::sA2C2_3BIT);
    
    enumSet.insert(enumSet.end(), CBasebandName::sAC_8BIT);
    
    enumSet.insert(enumSet.end(), CBasebandName::sB1D1_3BIT);
    
    enumSet.insert(enumSet.end(), CBasebandName::sB2D2_3BIT);
    
    enumSet.insert(enumSet.end(), CBasebandName::sBD_8BIT);
        
    return enumSet;
}

std::string CBasebandName::name(const BasebandNameMod::BasebandName& f) {
    switch (f) {
    
    case BasebandNameMod::NOBB:
      return CBasebandName::sNOBB;
    
    case BasebandNameMod::BB_1:
      return CBasebandName::sBB_1;
    
    case BasebandNameMod::BB_2:
      return CBasebandName::sBB_2;
    
    case BasebandNameMod::BB_3:
      return CBasebandName::sBB_3;
    
    case BasebandNameMod::BB_4:
      return CBasebandName::sBB_4;
    
    case BasebandNameMod::BB_5:
      return CBasebandName::sBB_5;
    
    case BasebandNameMod::BB_6:
      return CBasebandName::sBB_6;
    
    case BasebandNameMod::BB_7:
      return CBasebandName::sBB_7;
    
    case BasebandNameMod::BB_8:
      return CBasebandName::sBB_8;
    
    case BasebandNameMod::BB_ALL:
      return CBasebandName::sBB_ALL;
    
    case BasebandNameMod::A1C1_3BIT:
      return CBasebandName::sA1C1_3BIT;
    
    case BasebandNameMod::A2C2_3BIT:
      return CBasebandName::sA2C2_3BIT;
    
    case BasebandNameMod::AC_8BIT:
      return CBasebandName::sAC_8BIT;
    
    case BasebandNameMod::B1D1_3BIT:
      return CBasebandName::sB1D1_3BIT;
    
    case BasebandNameMod::B2D2_3BIT:
      return CBasebandName::sB2D2_3BIT;
    
    case BasebandNameMod::BD_8BIT:
      return CBasebandName::sBD_8BIT;
    	
    }
    // Impossible siutation but....who knows with C++ enums
    throw badInt((int) f);
}

BasebandNameMod::BasebandName CBasebandName::newBasebandName(const std::string& name) {
		
    if (name == CBasebandName::sNOBB) {
        return BasebandNameMod::NOBB;
    }
    	
    if (name == CBasebandName::sBB_1) {
        return BasebandNameMod::BB_1;
    }
    	
    if (name == CBasebandName::sBB_2) {
        return BasebandNameMod::BB_2;
    }
    	
    if (name == CBasebandName::sBB_3) {
        return BasebandNameMod::BB_3;
    }
    	
    if (name == CBasebandName::sBB_4) {
        return BasebandNameMod::BB_4;
    }
    	
    if (name == CBasebandName::sBB_5) {
        return BasebandNameMod::BB_5;
    }
    	
    if (name == CBasebandName::sBB_6) {
        return BasebandNameMod::BB_6;
    }
    	
    if (name == CBasebandName::sBB_7) {
        return BasebandNameMod::BB_7;
    }
    	
    if (name == CBasebandName::sBB_8) {
        return BasebandNameMod::BB_8;
    }
    	
    if (name == CBasebandName::sBB_ALL) {
        return BasebandNameMod::BB_ALL;
    }
    	
    if (name == CBasebandName::sA1C1_3BIT) {
        return BasebandNameMod::A1C1_3BIT;
    }
    	
    if (name == CBasebandName::sA2C2_3BIT) {
        return BasebandNameMod::A2C2_3BIT;
    }
    	
    if (name == CBasebandName::sAC_8BIT) {
        return BasebandNameMod::AC_8BIT;
    }
    	
    if (name == CBasebandName::sB1D1_3BIT) {
        return BasebandNameMod::B1D1_3BIT;
    }
    	
    if (name == CBasebandName::sB2D2_3BIT) {
        return BasebandNameMod::B2D2_3BIT;
    }
    	
    if (name == CBasebandName::sBD_8BIT) {
        return BasebandNameMod::BD_8BIT;
    }
    
    throw badString(name);
}

BasebandNameMod::BasebandName CBasebandName::literal(const std::string& name) {
		
    if (name == CBasebandName::sNOBB) {
        return BasebandNameMod::NOBB;
    }
    	
    if (name == CBasebandName::sBB_1) {
        return BasebandNameMod::BB_1;
    }
    	
    if (name == CBasebandName::sBB_2) {
        return BasebandNameMod::BB_2;
    }
    	
    if (name == CBasebandName::sBB_3) {
        return BasebandNameMod::BB_3;
    }
    	
    if (name == CBasebandName::sBB_4) {
        return BasebandNameMod::BB_4;
    }
    	
    if (name == CBasebandName::sBB_5) {
        return BasebandNameMod::BB_5;
    }
    	
    if (name == CBasebandName::sBB_6) {
        return BasebandNameMod::BB_6;
    }
    	
    if (name == CBasebandName::sBB_7) {
        return BasebandNameMod::BB_7;
    }
    	
    if (name == CBasebandName::sBB_8) {
        return BasebandNameMod::BB_8;
    }
    	
    if (name == CBasebandName::sBB_ALL) {
        return BasebandNameMod::BB_ALL;
    }
    	
    if (name == CBasebandName::sA1C1_3BIT) {
        return BasebandNameMod::A1C1_3BIT;
    }
    	
    if (name == CBasebandName::sA2C2_3BIT) {
        return BasebandNameMod::A2C2_3BIT;
    }
    	
    if (name == CBasebandName::sAC_8BIT) {
        return BasebandNameMod::AC_8BIT;
    }
    	
    if (name == CBasebandName::sB1D1_3BIT) {
        return BasebandNameMod::B1D1_3BIT;
    }
    	
    if (name == CBasebandName::sB2D2_3BIT) {
        return BasebandNameMod::B2D2_3BIT;
    }
    	
    if (name == CBasebandName::sBD_8BIT) {
        return BasebandNameMod::BD_8BIT;
    }
    
    throw badString(name);
}

BasebandNameMod::BasebandName CBasebandName::from_int(unsigned int i) {
	vector<string> names_ = names();
	if (i >= names_.size()) throw badInt(i);
	return newBasebandName(names_.at(i));
}

string CBasebandName::badString(const string& name) {
	return "'"+name+"' does not correspond to any literal in the enumeration 'BasebandName'.";
}

string CBasebandName::badInt(unsigned int i) {
	ostringstream oss ;
	oss << "'" << i << "' is out of range for the enumeration 'BasebandName'.";
	return oss.str();
}

namespace BasebandNameMod {
	std::ostream & operator << ( std::ostream & out, const BasebandName& value) {
		out << CBasebandName::name(value);
		return out;
	}

	std::istream & operator >> ( std::istream & in , BasebandName& value ) {
		in.clear();
		string s ; 
  		in >> s;
  		try {
    		value = CBasebandName::literal(s);
  		}
  		catch (string & m) {
    		in.setstate(ios::failbit);
  		}
  		return in;
  	}
}

