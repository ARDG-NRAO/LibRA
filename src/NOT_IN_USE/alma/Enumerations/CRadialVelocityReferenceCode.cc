
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
 * File CRadialVelocityReferenceCode.cpp
 */
#include <sstream>
#include <alma/Enumerations/CRadialVelocityReferenceCode.h>
#include <string>
using namespace std;

int CRadialVelocityReferenceCode::version() {
	return RadialVelocityReferenceCodeMod::version;
	}
	
string CRadialVelocityReferenceCode::revision () {
	return RadialVelocityReferenceCodeMod::revision;
}

unsigned int CRadialVelocityReferenceCode::size() {
	return 6;
	}
	
	
const std::string& CRadialVelocityReferenceCode::sLSRD = "LSRD";
	
const std::string& CRadialVelocityReferenceCode::sLSRK = "LSRK";
	
const std::string& CRadialVelocityReferenceCode::sGALACTO = "GALACTO";
	
const std::string& CRadialVelocityReferenceCode::sBARY = "BARY";
	
const std::string& CRadialVelocityReferenceCode::sGEO = "GEO";
	
const std::string& CRadialVelocityReferenceCode::sTOPO = "TOPO";
	
const std::vector<std::string> CRadialVelocityReferenceCode::names() {
    std::vector<std::string> enumSet;
    
    enumSet.insert(enumSet.end(), CRadialVelocityReferenceCode::sLSRD);
    
    enumSet.insert(enumSet.end(), CRadialVelocityReferenceCode::sLSRK);
    
    enumSet.insert(enumSet.end(), CRadialVelocityReferenceCode::sGALACTO);
    
    enumSet.insert(enumSet.end(), CRadialVelocityReferenceCode::sBARY);
    
    enumSet.insert(enumSet.end(), CRadialVelocityReferenceCode::sGEO);
    
    enumSet.insert(enumSet.end(), CRadialVelocityReferenceCode::sTOPO);
        
    return enumSet;
}

std::string CRadialVelocityReferenceCode::name(const RadialVelocityReferenceCodeMod::RadialVelocityReferenceCode& f) {
    switch (f) {
    
    case RadialVelocityReferenceCodeMod::LSRD:
      return CRadialVelocityReferenceCode::sLSRD;
    
    case RadialVelocityReferenceCodeMod::LSRK:
      return CRadialVelocityReferenceCode::sLSRK;
    
    case RadialVelocityReferenceCodeMod::GALACTO:
      return CRadialVelocityReferenceCode::sGALACTO;
    
    case RadialVelocityReferenceCodeMod::BARY:
      return CRadialVelocityReferenceCode::sBARY;
    
    case RadialVelocityReferenceCodeMod::GEO:
      return CRadialVelocityReferenceCode::sGEO;
    
    case RadialVelocityReferenceCodeMod::TOPO:
      return CRadialVelocityReferenceCode::sTOPO;
    	
    }
    // Impossible siutation but....who knows with C++ enums
    throw badInt((int) f);
}

RadialVelocityReferenceCodeMod::RadialVelocityReferenceCode CRadialVelocityReferenceCode::newRadialVelocityReferenceCode(const std::string& name) {
		
    if (name == CRadialVelocityReferenceCode::sLSRD) {
        return RadialVelocityReferenceCodeMod::LSRD;
    }
    	
    if (name == CRadialVelocityReferenceCode::sLSRK) {
        return RadialVelocityReferenceCodeMod::LSRK;
    }
    	
    if (name == CRadialVelocityReferenceCode::sGALACTO) {
        return RadialVelocityReferenceCodeMod::GALACTO;
    }
    	
    if (name == CRadialVelocityReferenceCode::sBARY) {
        return RadialVelocityReferenceCodeMod::BARY;
    }
    	
    if (name == CRadialVelocityReferenceCode::sGEO) {
        return RadialVelocityReferenceCodeMod::GEO;
    }
    	
    if (name == CRadialVelocityReferenceCode::sTOPO) {
        return RadialVelocityReferenceCodeMod::TOPO;
    }
    
    throw badString(name);
}

RadialVelocityReferenceCodeMod::RadialVelocityReferenceCode CRadialVelocityReferenceCode::literal(const std::string& name) {
		
    if (name == CRadialVelocityReferenceCode::sLSRD) {
        return RadialVelocityReferenceCodeMod::LSRD;
    }
    	
    if (name == CRadialVelocityReferenceCode::sLSRK) {
        return RadialVelocityReferenceCodeMod::LSRK;
    }
    	
    if (name == CRadialVelocityReferenceCode::sGALACTO) {
        return RadialVelocityReferenceCodeMod::GALACTO;
    }
    	
    if (name == CRadialVelocityReferenceCode::sBARY) {
        return RadialVelocityReferenceCodeMod::BARY;
    }
    	
    if (name == CRadialVelocityReferenceCode::sGEO) {
        return RadialVelocityReferenceCodeMod::GEO;
    }
    	
    if (name == CRadialVelocityReferenceCode::sTOPO) {
        return RadialVelocityReferenceCodeMod::TOPO;
    }
    
    throw badString(name);
}

RadialVelocityReferenceCodeMod::RadialVelocityReferenceCode CRadialVelocityReferenceCode::from_int(unsigned int i) {
	vector<string> names_ = names();
	if (i >= names_.size()) throw badInt(i);
	return newRadialVelocityReferenceCode(names_.at(i));
}

string CRadialVelocityReferenceCode::badString(const string& name) {
	return "'"+name+"' does not correspond to any literal in the enumeration 'RadialVelocityReferenceCode'.";
}

string CRadialVelocityReferenceCode::badInt(unsigned int i) {
	ostringstream oss ;
	oss << "'" << i << "' is out of range for the enumeration 'RadialVelocityReferenceCode'.";
	return oss.str();
}

namespace RadialVelocityReferenceCodeMod {
	std::ostream & operator << ( std::ostream & out, const RadialVelocityReferenceCode& value) {
		out << CRadialVelocityReferenceCode::name(value);
		return out;
	}

	std::istream & operator >> ( std::istream & in , RadialVelocityReferenceCode& value ) {
		in.clear();
		string s ; 
  		in >> s;
  		try {
    		value = CRadialVelocityReferenceCode::literal(s);
  		}
  		catch (string & m) {
    		in.setstate(ios::failbit);
  		}
  		return in;
  	}
}

