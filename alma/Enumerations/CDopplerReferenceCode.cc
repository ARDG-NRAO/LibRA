
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
 * File CDopplerReferenceCode.cpp
 */
#include <sstream>
#include <alma/Enumerations/CDopplerReferenceCode.h>
#include <string>
using namespace std;

int CDopplerReferenceCode::version() {
	return DopplerReferenceCodeMod::version;
	}
	
string CDopplerReferenceCode::revision () {
	return DopplerReferenceCodeMod::revision;
}

unsigned int CDopplerReferenceCode::size() {
	return 7;
	}
	
	
const std::string& CDopplerReferenceCode::sRADIO = "RADIO";
	
const std::string& CDopplerReferenceCode::sZ = "Z";
	
const std::string& CDopplerReferenceCode::sRATIO = "RATIO";
	
const std::string& CDopplerReferenceCode::sBETA = "BETA";
	
const std::string& CDopplerReferenceCode::sGAMMA = "GAMMA";
	
const std::string& CDopplerReferenceCode::sOPTICAL = "OPTICAL";
	
const std::string& CDopplerReferenceCode::sRELATIVISTIC = "RELATIVISTIC";
	
const std::vector<std::string> CDopplerReferenceCode::names() {
    std::vector<std::string> enumSet;
    
    enumSet.insert(enumSet.end(), CDopplerReferenceCode::sRADIO);
    
    enumSet.insert(enumSet.end(), CDopplerReferenceCode::sZ);
    
    enumSet.insert(enumSet.end(), CDopplerReferenceCode::sRATIO);
    
    enumSet.insert(enumSet.end(), CDopplerReferenceCode::sBETA);
    
    enumSet.insert(enumSet.end(), CDopplerReferenceCode::sGAMMA);
    
    enumSet.insert(enumSet.end(), CDopplerReferenceCode::sOPTICAL);
    
    enumSet.insert(enumSet.end(), CDopplerReferenceCode::sRELATIVISTIC);
        
    return enumSet;
}

std::string CDopplerReferenceCode::name(const DopplerReferenceCodeMod::DopplerReferenceCode& f) {
    switch (f) {
    
    case DopplerReferenceCodeMod::RADIO:
      return CDopplerReferenceCode::sRADIO;
    
    case DopplerReferenceCodeMod::Z:
      return CDopplerReferenceCode::sZ;
    
    case DopplerReferenceCodeMod::RATIO:
      return CDopplerReferenceCode::sRATIO;
    
    case DopplerReferenceCodeMod::BETA:
      return CDopplerReferenceCode::sBETA;
    
    case DopplerReferenceCodeMod::GAMMA:
      return CDopplerReferenceCode::sGAMMA;
    
    case DopplerReferenceCodeMod::OPTICAL:
      return CDopplerReferenceCode::sOPTICAL;
    
    case DopplerReferenceCodeMod::RELATIVISTIC:
      return CDopplerReferenceCode::sRELATIVISTIC;
    	
    }
    // Impossible siutation but....who knows with C++ enums
    throw badInt((int) f);
}

DopplerReferenceCodeMod::DopplerReferenceCode CDopplerReferenceCode::newDopplerReferenceCode(const std::string& name) {
		
    if (name == CDopplerReferenceCode::sRADIO) {
        return DopplerReferenceCodeMod::RADIO;
    }
    	
    if (name == CDopplerReferenceCode::sZ) {
        return DopplerReferenceCodeMod::Z;
    }
    	
    if (name == CDopplerReferenceCode::sRATIO) {
        return DopplerReferenceCodeMod::RATIO;
    }
    	
    if (name == CDopplerReferenceCode::sBETA) {
        return DopplerReferenceCodeMod::BETA;
    }
    	
    if (name == CDopplerReferenceCode::sGAMMA) {
        return DopplerReferenceCodeMod::GAMMA;
    }
    	
    if (name == CDopplerReferenceCode::sOPTICAL) {
        return DopplerReferenceCodeMod::OPTICAL;
    }
    	
    if (name == CDopplerReferenceCode::sRELATIVISTIC) {
        return DopplerReferenceCodeMod::RELATIVISTIC;
    }
    
    throw badString(name);
}

DopplerReferenceCodeMod::DopplerReferenceCode CDopplerReferenceCode::literal(const std::string& name) {
		
    if (name == CDopplerReferenceCode::sRADIO) {
        return DopplerReferenceCodeMod::RADIO;
    }
    	
    if (name == CDopplerReferenceCode::sZ) {
        return DopplerReferenceCodeMod::Z;
    }
    	
    if (name == CDopplerReferenceCode::sRATIO) {
        return DopplerReferenceCodeMod::RATIO;
    }
    	
    if (name == CDopplerReferenceCode::sBETA) {
        return DopplerReferenceCodeMod::BETA;
    }
    	
    if (name == CDopplerReferenceCode::sGAMMA) {
        return DopplerReferenceCodeMod::GAMMA;
    }
    	
    if (name == CDopplerReferenceCode::sOPTICAL) {
        return DopplerReferenceCodeMod::OPTICAL;
    }
    	
    if (name == CDopplerReferenceCode::sRELATIVISTIC) {
        return DopplerReferenceCodeMod::RELATIVISTIC;
    }
    
    throw badString(name);
}

DopplerReferenceCodeMod::DopplerReferenceCode CDopplerReferenceCode::from_int(unsigned int i) {
	vector<string> names_ = names();
	if (i >= names_.size()) throw badInt(i);
	return newDopplerReferenceCode(names_.at(i));
}

string CDopplerReferenceCode::badString(const string& name) {
	return "'"+name+"' does not correspond to any literal in the enumeration 'DopplerReferenceCode'.";
}

string CDopplerReferenceCode::badInt(unsigned int i) {
	ostringstream oss ;
	oss << "'" << i << "' is out of range for the enumeration 'DopplerReferenceCode'.";
	return oss.str();
}

namespace DopplerReferenceCodeMod {
	std::ostream & operator << ( std::ostream & out, const DopplerReferenceCode& value) {
		out << CDopplerReferenceCode::name(value);
		return out;
	}

	std::istream & operator >> ( std::istream & in , DopplerReferenceCode& value ) {
		in.clear();
		string s ; 
  		in >> s;
  		try {
    		value = CDopplerReferenceCode::literal(s);
  		}
  		catch (string & m) {
    		in.setstate(ios::failbit);
  		}
  		return in;
  	}
}

