
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
 * File CDetectorBandType.cpp
 */
#include <sstream>
#include <alma/Enumerations/CDetectorBandType.h>
#include <string>
using namespace std;

int CDetectorBandType::version() {
	return DetectorBandTypeMod::version;
	}
	
string CDetectorBandType::revision () {
	return DetectorBandTypeMod::revision;
}

unsigned int CDetectorBandType::size() {
	return 4;
	}
	
	
const std::string& CDetectorBandType::sBASEBAND = "BASEBAND";
	
const std::string& CDetectorBandType::sDOWN_CONVERTER = "DOWN_CONVERTER";
	
const std::string& CDetectorBandType::sHOLOGRAPHY_RECEIVER = "HOLOGRAPHY_RECEIVER";
	
const std::string& CDetectorBandType::sSUBBAND = "SUBBAND";
	
const std::vector<std::string> CDetectorBandType::names() {
    std::vector<std::string> enumSet;
    
    enumSet.insert(enumSet.end(), CDetectorBandType::sBASEBAND);
    
    enumSet.insert(enumSet.end(), CDetectorBandType::sDOWN_CONVERTER);
    
    enumSet.insert(enumSet.end(), CDetectorBandType::sHOLOGRAPHY_RECEIVER);
    
    enumSet.insert(enumSet.end(), CDetectorBandType::sSUBBAND);
        
    return enumSet;
}

std::string CDetectorBandType::name(const DetectorBandTypeMod::DetectorBandType& f) {
    switch (f) {
    
    case DetectorBandTypeMod::BASEBAND:
      return CDetectorBandType::sBASEBAND;
    
    case DetectorBandTypeMod::DOWN_CONVERTER:
      return CDetectorBandType::sDOWN_CONVERTER;
    
    case DetectorBandTypeMod::HOLOGRAPHY_RECEIVER:
      return CDetectorBandType::sHOLOGRAPHY_RECEIVER;
    
    case DetectorBandTypeMod::SUBBAND:
      return CDetectorBandType::sSUBBAND;
    	
    }
    // Impossible siutation but....who knows with C++ enums
    throw badInt((int) f);
}

DetectorBandTypeMod::DetectorBandType CDetectorBandType::newDetectorBandType(const std::string& name) {
		
    if (name == CDetectorBandType::sBASEBAND) {
        return DetectorBandTypeMod::BASEBAND;
    }
    	
    if (name == CDetectorBandType::sDOWN_CONVERTER) {
        return DetectorBandTypeMod::DOWN_CONVERTER;
    }
    	
    if (name == CDetectorBandType::sHOLOGRAPHY_RECEIVER) {
        return DetectorBandTypeMod::HOLOGRAPHY_RECEIVER;
    }
    	
    if (name == CDetectorBandType::sSUBBAND) {
        return DetectorBandTypeMod::SUBBAND;
    }
    
    throw badString(name);
}

DetectorBandTypeMod::DetectorBandType CDetectorBandType::literal(const std::string& name) {
		
    if (name == CDetectorBandType::sBASEBAND) {
        return DetectorBandTypeMod::BASEBAND;
    }
    	
    if (name == CDetectorBandType::sDOWN_CONVERTER) {
        return DetectorBandTypeMod::DOWN_CONVERTER;
    }
    	
    if (name == CDetectorBandType::sHOLOGRAPHY_RECEIVER) {
        return DetectorBandTypeMod::HOLOGRAPHY_RECEIVER;
    }
    	
    if (name == CDetectorBandType::sSUBBAND) {
        return DetectorBandTypeMod::SUBBAND;
    }
    
    throw badString(name);
}

DetectorBandTypeMod::DetectorBandType CDetectorBandType::from_int(unsigned int i) {
	vector<string> names_ = names();
	if (i >= names_.size()) throw badInt(i);
	return newDetectorBandType(names_.at(i));
}

string CDetectorBandType::badString(const string& name) {
	return "'"+name+"' does not correspond to any literal in the enumeration 'DetectorBandType'.";
}

string CDetectorBandType::badInt(unsigned int i) {
	ostringstream oss ;
	oss << "'" << i << "' is out of range for the enumeration 'DetectorBandType'.";
	return oss.str();
}

namespace DetectorBandTypeMod {
	std::ostream & operator << ( std::ostream & out, const DetectorBandType& value) {
		out << CDetectorBandType::name(value);
		return out;
	}

	std::istream & operator >> ( std::istream & in , DetectorBandType& value ) {
		in.clear();
		string s ; 
  		in >> s;
  		try {
    		value = CDetectorBandType::literal(s);
  		}
  		catch (string & m) {
    		in.setstate(ios::failbit);
  		}
  		return in;
  	}
}

