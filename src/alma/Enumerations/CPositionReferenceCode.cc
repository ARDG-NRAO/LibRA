
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
 * File CPositionReferenceCode.cpp
 */
#include <sstream>
#include <alma/Enumerations/CPositionReferenceCode.h>
#include <string>
using namespace std;

int CPositionReferenceCode::version() {
	return PositionReferenceCodeMod::version;
	}
	
string CPositionReferenceCode::revision () {
	return PositionReferenceCodeMod::revision;
}

unsigned int CPositionReferenceCode::size() {
	return 6;
	}
	
	
const std::string& CPositionReferenceCode::sITRF = "ITRF";
	
const std::string& CPositionReferenceCode::sWGS84 = "WGS84";
	
const std::string& CPositionReferenceCode::sSITE = "SITE";
	
const std::string& CPositionReferenceCode::sSTATION = "STATION";
	
const std::string& CPositionReferenceCode::sYOKE = "YOKE";
	
const std::string& CPositionReferenceCode::sREFLECTOR = "REFLECTOR";
	
const std::vector<std::string> CPositionReferenceCode::names() {
    std::vector<std::string> enumSet;
    
    enumSet.insert(enumSet.end(), CPositionReferenceCode::sITRF);
    
    enumSet.insert(enumSet.end(), CPositionReferenceCode::sWGS84);
    
    enumSet.insert(enumSet.end(), CPositionReferenceCode::sSITE);
    
    enumSet.insert(enumSet.end(), CPositionReferenceCode::sSTATION);
    
    enumSet.insert(enumSet.end(), CPositionReferenceCode::sYOKE);
    
    enumSet.insert(enumSet.end(), CPositionReferenceCode::sREFLECTOR);
        
    return enumSet;
}

std::string CPositionReferenceCode::name(const PositionReferenceCodeMod::PositionReferenceCode& f) {
    switch (f) {
    
    case PositionReferenceCodeMod::ITRF:
      return CPositionReferenceCode::sITRF;
    
    case PositionReferenceCodeMod::WGS84:
      return CPositionReferenceCode::sWGS84;
    
    case PositionReferenceCodeMod::SITE:
      return CPositionReferenceCode::sSITE;
    
    case PositionReferenceCodeMod::STATION:
      return CPositionReferenceCode::sSTATION;
    
    case PositionReferenceCodeMod::YOKE:
      return CPositionReferenceCode::sYOKE;
    
    case PositionReferenceCodeMod::REFLECTOR:
      return CPositionReferenceCode::sREFLECTOR;
    	
    }
    // Impossible siutation but....who knows with C++ enums
    throw badInt((int) f);
}

PositionReferenceCodeMod::PositionReferenceCode CPositionReferenceCode::newPositionReferenceCode(const std::string& name) {
		
    if (name == CPositionReferenceCode::sITRF) {
        return PositionReferenceCodeMod::ITRF;
    }
    	
    if (name == CPositionReferenceCode::sWGS84) {
        return PositionReferenceCodeMod::WGS84;
    }
    	
    if (name == CPositionReferenceCode::sSITE) {
        return PositionReferenceCodeMod::SITE;
    }
    	
    if (name == CPositionReferenceCode::sSTATION) {
        return PositionReferenceCodeMod::STATION;
    }
    	
    if (name == CPositionReferenceCode::sYOKE) {
        return PositionReferenceCodeMod::YOKE;
    }
    	
    if (name == CPositionReferenceCode::sREFLECTOR) {
        return PositionReferenceCodeMod::REFLECTOR;
    }
    
    throw badString(name);
}

PositionReferenceCodeMod::PositionReferenceCode CPositionReferenceCode::literal(const std::string& name) {
		
    if (name == CPositionReferenceCode::sITRF) {
        return PositionReferenceCodeMod::ITRF;
    }
    	
    if (name == CPositionReferenceCode::sWGS84) {
        return PositionReferenceCodeMod::WGS84;
    }
    	
    if (name == CPositionReferenceCode::sSITE) {
        return PositionReferenceCodeMod::SITE;
    }
    	
    if (name == CPositionReferenceCode::sSTATION) {
        return PositionReferenceCodeMod::STATION;
    }
    	
    if (name == CPositionReferenceCode::sYOKE) {
        return PositionReferenceCodeMod::YOKE;
    }
    	
    if (name == CPositionReferenceCode::sREFLECTOR) {
        return PositionReferenceCodeMod::REFLECTOR;
    }
    
    throw badString(name);
}

PositionReferenceCodeMod::PositionReferenceCode CPositionReferenceCode::from_int(unsigned int i) {
	vector<string> names_ = names();
	if (i >= names_.size()) throw badInt(i);
	return newPositionReferenceCode(names_.at(i));
}

string CPositionReferenceCode::badString(const string& name) {
	return "'"+name+"' does not correspond to any literal in the enumeration 'PositionReferenceCode'.";
}

string CPositionReferenceCode::badInt(unsigned int i) {
	ostringstream oss ;
	oss << "'" << i << "' is out of range for the enumeration 'PositionReferenceCode'.";
	return oss.str();
}

namespace PositionReferenceCodeMod {
	std::ostream & operator << ( std::ostream & out, const PositionReferenceCode& value) {
		out << CPositionReferenceCode::name(value);
		return out;
	}

	std::istream & operator >> ( std::istream & in , PositionReferenceCode& value ) {
		in.clear();
		string s ; 
  		in >> s;
  		try {
    		value = CPositionReferenceCode::literal(s);
  		}
  		catch (string & m) {
    		in.setstate(ios::failbit);
  		}
  		return in;
  	}
}

