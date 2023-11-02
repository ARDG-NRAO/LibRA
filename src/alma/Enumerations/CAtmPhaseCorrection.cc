
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
 * File CAtmPhaseCorrection.cpp
 */
#include <sstream>
#include <alma/Enumerations/CAtmPhaseCorrection.h>
#include <string>
using namespace std;

int CAtmPhaseCorrection::version() {
	return AtmPhaseCorrectionMod::version;
	}
	
string CAtmPhaseCorrection::revision () {
	return AtmPhaseCorrectionMod::revision;
}

unsigned int CAtmPhaseCorrection::size() {
	return 2;
	}
	
	
const std::string& CAtmPhaseCorrection::sAP_UNCORRECTED = "AP_UNCORRECTED";
	
const std::string& CAtmPhaseCorrection::sAP_CORRECTED = "AP_CORRECTED";
	
const std::vector<std::string> CAtmPhaseCorrection::names() {
    std::vector<std::string> enumSet;
    
    enumSet.insert(enumSet.end(), CAtmPhaseCorrection::sAP_UNCORRECTED);
    
    enumSet.insert(enumSet.end(), CAtmPhaseCorrection::sAP_CORRECTED);
        
    return enumSet;
}

std::string CAtmPhaseCorrection::name(const AtmPhaseCorrectionMod::AtmPhaseCorrection& f) {
    switch (f) {
    
    case AtmPhaseCorrectionMod::AP_UNCORRECTED:
      return CAtmPhaseCorrection::sAP_UNCORRECTED;
    
    case AtmPhaseCorrectionMod::AP_CORRECTED:
      return CAtmPhaseCorrection::sAP_CORRECTED;
    	
    }
    // Impossible siutation but....who knows with C++ enums
    throw badInt((int) f);
}

AtmPhaseCorrectionMod::AtmPhaseCorrection CAtmPhaseCorrection::newAtmPhaseCorrection(const std::string& name) {
		
    if (name == CAtmPhaseCorrection::sAP_UNCORRECTED) {
        return AtmPhaseCorrectionMod::AP_UNCORRECTED;
    }
    	
    if (name == CAtmPhaseCorrection::sAP_CORRECTED) {
        return AtmPhaseCorrectionMod::AP_CORRECTED;
    }
    
    throw badString(name);
}

AtmPhaseCorrectionMod::AtmPhaseCorrection CAtmPhaseCorrection::literal(const std::string& name) {
		
    if (name == CAtmPhaseCorrection::sAP_UNCORRECTED) {
        return AtmPhaseCorrectionMod::AP_UNCORRECTED;
    }
    	
    if (name == CAtmPhaseCorrection::sAP_CORRECTED) {
        return AtmPhaseCorrectionMod::AP_CORRECTED;
    }
    
    throw badString(name);
}

AtmPhaseCorrectionMod::AtmPhaseCorrection CAtmPhaseCorrection::from_int(unsigned int i) {
	vector<string> names_ = names();
	if (i >= names_.size()) throw badInt(i);
	return newAtmPhaseCorrection(names_.at(i));
}

string CAtmPhaseCorrection::badString(const string& name) {
	return "'"+name+"' does not correspond to any literal in the enumeration 'AtmPhaseCorrection'.";
}

string CAtmPhaseCorrection::badInt(unsigned int i) {
	ostringstream oss ;
	oss << "'" << i << "' is out of range for the enumeration 'AtmPhaseCorrection'.";
	return oss.str();
}

namespace AtmPhaseCorrectionMod {
	std::ostream & operator << ( std::ostream & out, const AtmPhaseCorrection& value) {
		out << CAtmPhaseCorrection::name(value);
		return out;
	}

	std::istream & operator >> ( std::istream & in , AtmPhaseCorrection& value ) {
		in.clear();
		string s ; 
  		in >> s;
  		try {
    		value = CAtmPhaseCorrection::literal(s);
  		}
  		catch (string & m) {
    		in.setstate(ios::failbit);
  		}
  		return in;
  	}
}

