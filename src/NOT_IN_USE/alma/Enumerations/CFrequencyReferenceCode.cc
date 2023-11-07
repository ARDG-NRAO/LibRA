
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
 * File CFrequencyReferenceCode.cpp
 */
#include <sstream>
#include <alma/Enumerations/CFrequencyReferenceCode.h>
#include <string>
using namespace std;

int CFrequencyReferenceCode::version() {
	return FrequencyReferenceCodeMod::version;
	}
	
string CFrequencyReferenceCode::revision () {
	return FrequencyReferenceCodeMod::revision;
}

unsigned int CFrequencyReferenceCode::size() {
	return 8;
	}
	
	
const std::string& CFrequencyReferenceCode::sLABREST = "LABREST";
	
const std::string& CFrequencyReferenceCode::sLSRD = "LSRD";
	
const std::string& CFrequencyReferenceCode::sLSRK = "LSRK";
	
const std::string& CFrequencyReferenceCode::sBARY = "BARY";
	
const std::string& CFrequencyReferenceCode::sREST = "REST";
	
const std::string& CFrequencyReferenceCode::sGEO = "GEO";
	
const std::string& CFrequencyReferenceCode::sGALACTO = "GALACTO";
	
const std::string& CFrequencyReferenceCode::sTOPO = "TOPO";
	
const std::vector<std::string> CFrequencyReferenceCode::names() {
    std::vector<std::string> enumSet;
    
    enumSet.insert(enumSet.end(), CFrequencyReferenceCode::sLABREST);
    
    enumSet.insert(enumSet.end(), CFrequencyReferenceCode::sLSRD);
    
    enumSet.insert(enumSet.end(), CFrequencyReferenceCode::sLSRK);
    
    enumSet.insert(enumSet.end(), CFrequencyReferenceCode::sBARY);
    
    enumSet.insert(enumSet.end(), CFrequencyReferenceCode::sREST);
    
    enumSet.insert(enumSet.end(), CFrequencyReferenceCode::sGEO);
    
    enumSet.insert(enumSet.end(), CFrequencyReferenceCode::sGALACTO);
    
    enumSet.insert(enumSet.end(), CFrequencyReferenceCode::sTOPO);
        
    return enumSet;
}

std::string CFrequencyReferenceCode::name(const FrequencyReferenceCodeMod::FrequencyReferenceCode& f) {
    switch (f) {
    
    case FrequencyReferenceCodeMod::LABREST:
      return CFrequencyReferenceCode::sLABREST;
    
    case FrequencyReferenceCodeMod::LSRD:
      return CFrequencyReferenceCode::sLSRD;
    
    case FrequencyReferenceCodeMod::LSRK:
      return CFrequencyReferenceCode::sLSRK;
    
    case FrequencyReferenceCodeMod::BARY:
      return CFrequencyReferenceCode::sBARY;
    
    case FrequencyReferenceCodeMod::REST:
      return CFrequencyReferenceCode::sREST;
    
    case FrequencyReferenceCodeMod::GEO:
      return CFrequencyReferenceCode::sGEO;
    
    case FrequencyReferenceCodeMod::GALACTO:
      return CFrequencyReferenceCode::sGALACTO;
    
    case FrequencyReferenceCodeMod::TOPO:
      return CFrequencyReferenceCode::sTOPO;
    	
    }
    // Impossible siutation but....who knows with C++ enums
    throw badInt((int) f);
}

FrequencyReferenceCodeMod::FrequencyReferenceCode CFrequencyReferenceCode::newFrequencyReferenceCode(const std::string& name) {
		
    if (name == CFrequencyReferenceCode::sLABREST) {
        return FrequencyReferenceCodeMod::LABREST;
    }
    	
    if (name == CFrequencyReferenceCode::sLSRD) {
        return FrequencyReferenceCodeMod::LSRD;
    }
    	
    if (name == CFrequencyReferenceCode::sLSRK) {
        return FrequencyReferenceCodeMod::LSRK;
    }
    	
    if (name == CFrequencyReferenceCode::sBARY) {
        return FrequencyReferenceCodeMod::BARY;
    }
    	
    if (name == CFrequencyReferenceCode::sREST) {
        return FrequencyReferenceCodeMod::REST;
    }
    	
    if (name == CFrequencyReferenceCode::sGEO) {
        return FrequencyReferenceCodeMod::GEO;
    }
    	
    if (name == CFrequencyReferenceCode::sGALACTO) {
        return FrequencyReferenceCodeMod::GALACTO;
    }
    	
    if (name == CFrequencyReferenceCode::sTOPO) {
        return FrequencyReferenceCodeMod::TOPO;
    }
    
    throw badString(name);
}

FrequencyReferenceCodeMod::FrequencyReferenceCode CFrequencyReferenceCode::literal(const std::string& name) {
		
    if (name == CFrequencyReferenceCode::sLABREST) {
        return FrequencyReferenceCodeMod::LABREST;
    }
    	
    if (name == CFrequencyReferenceCode::sLSRD) {
        return FrequencyReferenceCodeMod::LSRD;
    }
    	
    if (name == CFrequencyReferenceCode::sLSRK) {
        return FrequencyReferenceCodeMod::LSRK;
    }
    	
    if (name == CFrequencyReferenceCode::sBARY) {
        return FrequencyReferenceCodeMod::BARY;
    }
    	
    if (name == CFrequencyReferenceCode::sREST) {
        return FrequencyReferenceCodeMod::REST;
    }
    	
    if (name == CFrequencyReferenceCode::sGEO) {
        return FrequencyReferenceCodeMod::GEO;
    }
    	
    if (name == CFrequencyReferenceCode::sGALACTO) {
        return FrequencyReferenceCodeMod::GALACTO;
    }
    	
    if (name == CFrequencyReferenceCode::sTOPO) {
        return FrequencyReferenceCodeMod::TOPO;
    }
    
    throw badString(name);
}

FrequencyReferenceCodeMod::FrequencyReferenceCode CFrequencyReferenceCode::from_int(unsigned int i) {
	vector<string> names_ = names();
	if (i >= names_.size()) throw badInt(i);
	return newFrequencyReferenceCode(names_.at(i));
}

string CFrequencyReferenceCode::badString(const string& name) {
	return "'"+name+"' does not correspond to any literal in the enumeration 'FrequencyReferenceCode'.";
}

string CFrequencyReferenceCode::badInt(unsigned int i) {
	ostringstream oss ;
	oss << "'" << i << "' is out of range for the enumeration 'FrequencyReferenceCode'.";
	return oss.str();
}

namespace FrequencyReferenceCodeMod {
	std::ostream & operator << ( std::ostream & out, const FrequencyReferenceCode& value) {
		out << CFrequencyReferenceCode::name(value);
		return out;
	}

	std::istream & operator >> ( std::istream & in , FrequencyReferenceCode& value ) {
		in.clear();
		string s ; 
  		in >> s;
  		try {
    		value = CFrequencyReferenceCode::literal(s);
  		}
  		catch (string & m) {
    		in.setstate(ios::failbit);
  		}
  		return in;
  	}
}

