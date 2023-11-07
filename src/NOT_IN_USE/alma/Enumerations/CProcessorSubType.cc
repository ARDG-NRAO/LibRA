
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
 * File CProcessorSubType.cpp
 */
#include <sstream>
#include <alma/Enumerations/CProcessorSubType.h>
#include <string>
using namespace std;

int CProcessorSubType::version() {
	return ProcessorSubTypeMod::version;
	}
	
string CProcessorSubType::revision () {
	return ProcessorSubTypeMod::revision;
}

unsigned int CProcessorSubType::size() {
	return 4;
	}
	
	
const std::string& CProcessorSubType::sALMA_CORRELATOR_MODE = "ALMA_CORRELATOR_MODE";
	
const std::string& CProcessorSubType::sSQUARE_LAW_DETECTOR = "SQUARE_LAW_DETECTOR";
	
const std::string& CProcessorSubType::sHOLOGRAPHY = "HOLOGRAPHY";
	
const std::string& CProcessorSubType::sALMA_RADIOMETER = "ALMA_RADIOMETER";
	
const std::vector<std::string> CProcessorSubType::names() {
    std::vector<std::string> enumSet;
    
    enumSet.insert(enumSet.end(), CProcessorSubType::sALMA_CORRELATOR_MODE);
    
    enumSet.insert(enumSet.end(), CProcessorSubType::sSQUARE_LAW_DETECTOR);
    
    enumSet.insert(enumSet.end(), CProcessorSubType::sHOLOGRAPHY);
    
    enumSet.insert(enumSet.end(), CProcessorSubType::sALMA_RADIOMETER);
        
    return enumSet;
}

std::string CProcessorSubType::name(const ProcessorSubTypeMod::ProcessorSubType& f) {
    switch (f) {
    
    case ProcessorSubTypeMod::ALMA_CORRELATOR_MODE:
      return CProcessorSubType::sALMA_CORRELATOR_MODE;
    
    case ProcessorSubTypeMod::SQUARE_LAW_DETECTOR:
      return CProcessorSubType::sSQUARE_LAW_DETECTOR;
    
    case ProcessorSubTypeMod::HOLOGRAPHY:
      return CProcessorSubType::sHOLOGRAPHY;
    
    case ProcessorSubTypeMod::ALMA_RADIOMETER:
      return CProcessorSubType::sALMA_RADIOMETER;
    	
    }
    // Impossible siutation but....who knows with C++ enums
    throw badInt((int) f);
}

ProcessorSubTypeMod::ProcessorSubType CProcessorSubType::newProcessorSubType(const std::string& name) {
		
    if (name == CProcessorSubType::sALMA_CORRELATOR_MODE) {
        return ProcessorSubTypeMod::ALMA_CORRELATOR_MODE;
    }
    	
    if (name == CProcessorSubType::sSQUARE_LAW_DETECTOR) {
        return ProcessorSubTypeMod::SQUARE_LAW_DETECTOR;
    }
    	
    if (name == CProcessorSubType::sHOLOGRAPHY) {
        return ProcessorSubTypeMod::HOLOGRAPHY;
    }
    	
    if (name == CProcessorSubType::sALMA_RADIOMETER) {
        return ProcessorSubTypeMod::ALMA_RADIOMETER;
    }
    
    throw badString(name);
}

ProcessorSubTypeMod::ProcessorSubType CProcessorSubType::literal(const std::string& name) {
		
    if (name == CProcessorSubType::sALMA_CORRELATOR_MODE) {
        return ProcessorSubTypeMod::ALMA_CORRELATOR_MODE;
    }
    	
    if (name == CProcessorSubType::sSQUARE_LAW_DETECTOR) {
        return ProcessorSubTypeMod::SQUARE_LAW_DETECTOR;
    }
    	
    if (name == CProcessorSubType::sHOLOGRAPHY) {
        return ProcessorSubTypeMod::HOLOGRAPHY;
    }
    	
    if (name == CProcessorSubType::sALMA_RADIOMETER) {
        return ProcessorSubTypeMod::ALMA_RADIOMETER;
    }
    
    throw badString(name);
}

ProcessorSubTypeMod::ProcessorSubType CProcessorSubType::from_int(unsigned int i) {
	vector<string> names_ = names();
	if (i >= names_.size()) throw badInt(i);
	return newProcessorSubType(names_.at(i));
}

string CProcessorSubType::badString(const string& name) {
	return "'"+name+"' does not correspond to any literal in the enumeration 'ProcessorSubType'.";
}

string CProcessorSubType::badInt(unsigned int i) {
	ostringstream oss ;
	oss << "'" << i << "' is out of range for the enumeration 'ProcessorSubType'.";
	return oss.str();
}

namespace ProcessorSubTypeMod {
	std::ostream & operator << ( std::ostream & out, const ProcessorSubType& value) {
		out << CProcessorSubType::name(value);
		return out;
	}

	std::istream & operator >> ( std::istream & in , ProcessorSubType& value ) {
		in.clear();
		string s ; 
  		in >> s;
  		try {
    		value = CProcessorSubType::literal(s);
  		}
  		catch (string & m) {
    		in.setstate(ios::failbit);
  		}
  		return in;
  	}
}

