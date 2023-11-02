
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
 * File CCalibrationSet.cpp
 */
#include <sstream>
#include <alma/Enumerations/CCalibrationSet.h>
#include <string>
using namespace std;

int CCalibrationSet::version() {
	return CalibrationSetMod::version;
	}
	
string CCalibrationSet::revision () {
	return CalibrationSetMod::revision;
}

unsigned int CCalibrationSet::size() {
	return 8;
	}
	
	
const std::string& CCalibrationSet::sNONE = "NONE";
	
const std::string& CCalibrationSet::sAMPLI_CURVE = "AMPLI_CURVE";
	
const std::string& CCalibrationSet::sANTENNA_POSITIONS = "ANTENNA_POSITIONS";
	
const std::string& CCalibrationSet::sPHASE_CURVE = "PHASE_CURVE";
	
const std::string& CCalibrationSet::sPOINTING_MODEL = "POINTING_MODEL";
	
const std::string& CCalibrationSet::sACCUMULATE = "ACCUMULATE";
	
const std::string& CCalibrationSet::sTEST = "TEST";
	
const std::string& CCalibrationSet::sUNSPECIFIED = "UNSPECIFIED";
	
const std::vector<std::string> CCalibrationSet::names() {
    std::vector<std::string> enumSet;
    
    enumSet.insert(enumSet.end(), CCalibrationSet::sNONE);
    
    enumSet.insert(enumSet.end(), CCalibrationSet::sAMPLI_CURVE);
    
    enumSet.insert(enumSet.end(), CCalibrationSet::sANTENNA_POSITIONS);
    
    enumSet.insert(enumSet.end(), CCalibrationSet::sPHASE_CURVE);
    
    enumSet.insert(enumSet.end(), CCalibrationSet::sPOINTING_MODEL);
    
    enumSet.insert(enumSet.end(), CCalibrationSet::sACCUMULATE);
    
    enumSet.insert(enumSet.end(), CCalibrationSet::sTEST);
    
    enumSet.insert(enumSet.end(), CCalibrationSet::sUNSPECIFIED);
        
    return enumSet;
}

std::string CCalibrationSet::name(const CalibrationSetMod::CalibrationSet& f) {
    switch (f) {
    
    case CalibrationSetMod::NONE:
      return CCalibrationSet::sNONE;
    
    case CalibrationSetMod::AMPLI_CURVE:
      return CCalibrationSet::sAMPLI_CURVE;
    
    case CalibrationSetMod::ANTENNA_POSITIONS:
      return CCalibrationSet::sANTENNA_POSITIONS;
    
    case CalibrationSetMod::PHASE_CURVE:
      return CCalibrationSet::sPHASE_CURVE;
    
    case CalibrationSetMod::POINTING_MODEL:
      return CCalibrationSet::sPOINTING_MODEL;
    
    case CalibrationSetMod::ACCUMULATE:
      return CCalibrationSet::sACCUMULATE;
    
    case CalibrationSetMod::TEST:
      return CCalibrationSet::sTEST;
    
    case CalibrationSetMod::UNSPECIFIED:
      return CCalibrationSet::sUNSPECIFIED;
    	
    }
    // Impossible siutation but....who knows with C++ enums
    throw badInt((int) f);
}

CalibrationSetMod::CalibrationSet CCalibrationSet::newCalibrationSet(const std::string& name) {
		
    if (name == CCalibrationSet::sNONE) {
        return CalibrationSetMod::NONE;
    }
    	
    if (name == CCalibrationSet::sAMPLI_CURVE) {
        return CalibrationSetMod::AMPLI_CURVE;
    }
    	
    if (name == CCalibrationSet::sANTENNA_POSITIONS) {
        return CalibrationSetMod::ANTENNA_POSITIONS;
    }
    	
    if (name == CCalibrationSet::sPHASE_CURVE) {
        return CalibrationSetMod::PHASE_CURVE;
    }
    	
    if (name == CCalibrationSet::sPOINTING_MODEL) {
        return CalibrationSetMod::POINTING_MODEL;
    }
    	
    if (name == CCalibrationSet::sACCUMULATE) {
        return CalibrationSetMod::ACCUMULATE;
    }
    	
    if (name == CCalibrationSet::sTEST) {
        return CalibrationSetMod::TEST;
    }
    	
    if (name == CCalibrationSet::sUNSPECIFIED) {
        return CalibrationSetMod::UNSPECIFIED;
    }
    
    throw badString(name);
}

CalibrationSetMod::CalibrationSet CCalibrationSet::literal(const std::string& name) {
		
    if (name == CCalibrationSet::sNONE) {
        return CalibrationSetMod::NONE;
    }
    	
    if (name == CCalibrationSet::sAMPLI_CURVE) {
        return CalibrationSetMod::AMPLI_CURVE;
    }
    	
    if (name == CCalibrationSet::sANTENNA_POSITIONS) {
        return CalibrationSetMod::ANTENNA_POSITIONS;
    }
    	
    if (name == CCalibrationSet::sPHASE_CURVE) {
        return CalibrationSetMod::PHASE_CURVE;
    }
    	
    if (name == CCalibrationSet::sPOINTING_MODEL) {
        return CalibrationSetMod::POINTING_MODEL;
    }
    	
    if (name == CCalibrationSet::sACCUMULATE) {
        return CalibrationSetMod::ACCUMULATE;
    }
    	
    if (name == CCalibrationSet::sTEST) {
        return CalibrationSetMod::TEST;
    }
    	
    if (name == CCalibrationSet::sUNSPECIFIED) {
        return CalibrationSetMod::UNSPECIFIED;
    }
    
    throw badString(name);
}

CalibrationSetMod::CalibrationSet CCalibrationSet::from_int(unsigned int i) {
	vector<string> names_ = names();
	if (i >= names_.size()) throw badInt(i);
	return newCalibrationSet(names_.at(i));
}

string CCalibrationSet::badString(const string& name) {
	return "'"+name+"' does not correspond to any literal in the enumeration 'CalibrationSet'.";
}

string CCalibrationSet::badInt(unsigned int i) {
	ostringstream oss ;
	oss << "'" << i << "' is out of range for the enumeration 'CalibrationSet'.";
	return oss.str();
}

namespace CalibrationSetMod {
	std::ostream & operator << ( std::ostream & out, const CalibrationSet& value) {
		out << CCalibrationSet::name(value);
		return out;
	}

	std::istream & operator >> ( std::istream & in , CalibrationSet& value ) {
		in.clear();
		string s ; 
  		in >> s;
  		try {
    		value = CCalibrationSet::literal(s);
  		}
  		catch (string & m) {
    		in.setstate(ios::failbit);
  		}
  		return in;
  	}
}

