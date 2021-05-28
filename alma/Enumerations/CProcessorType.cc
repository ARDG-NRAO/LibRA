
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
 * File CProcessorType.cpp
 */
#include <sstream>
#include <alma/Enumerations/CProcessorType.h>
#include <string>
using namespace std;

int CProcessorType::version() {
	return ProcessorTypeMod::version;
	}
	
string CProcessorType::revision () {
	return ProcessorTypeMod::revision;
}

unsigned int CProcessorType::size() {
	return 3;
	}
	
	
const std::string& CProcessorType::sCORRELATOR = "CORRELATOR";
	
const std::string& CProcessorType::sRADIOMETER = "RADIOMETER";
	
const std::string& CProcessorType::sSPECTROMETER = "SPECTROMETER";
	
const std::vector<std::string> CProcessorType::names() {
    std::vector<std::string> enumSet;
    
    enumSet.insert(enumSet.end(), CProcessorType::sCORRELATOR);
    
    enumSet.insert(enumSet.end(), CProcessorType::sRADIOMETER);
    
    enumSet.insert(enumSet.end(), CProcessorType::sSPECTROMETER);
        
    return enumSet;
}

std::string CProcessorType::name(const ProcessorTypeMod::ProcessorType& f) {
    switch (f) {
    
    case ProcessorTypeMod::CORRELATOR:
      return CProcessorType::sCORRELATOR;
    
    case ProcessorTypeMod::RADIOMETER:
      return CProcessorType::sRADIOMETER;
    
    case ProcessorTypeMod::SPECTROMETER:
      return CProcessorType::sSPECTROMETER;
    	
    }
    // Impossible siutation but....who knows with C++ enums
    throw badInt((int) f);
}

ProcessorTypeMod::ProcessorType CProcessorType::newProcessorType(const std::string& name) {
		
    if (name == CProcessorType::sCORRELATOR) {
        return ProcessorTypeMod::CORRELATOR;
    }
    	
    if (name == CProcessorType::sRADIOMETER) {
        return ProcessorTypeMod::RADIOMETER;
    }
    	
    if (name == CProcessorType::sSPECTROMETER) {
        return ProcessorTypeMod::SPECTROMETER;
    }
    
    throw badString(name);
}

ProcessorTypeMod::ProcessorType CProcessorType::literal(const std::string& name) {
		
    if (name == CProcessorType::sCORRELATOR) {
        return ProcessorTypeMod::CORRELATOR;
    }
    	
    if (name == CProcessorType::sRADIOMETER) {
        return ProcessorTypeMod::RADIOMETER;
    }
    	
    if (name == CProcessorType::sSPECTROMETER) {
        return ProcessorTypeMod::SPECTROMETER;
    }
    
    throw badString(name);
}

ProcessorTypeMod::ProcessorType CProcessorType::from_int(unsigned int i) {
	vector<string> names_ = names();
	if (i >= names_.size()) throw badInt(i);
	return newProcessorType(names_.at(i));
}

string CProcessorType::badString(const string& name) {
	return "'"+name+"' does not correspond to any literal in the enumeration 'ProcessorType'.";
}

string CProcessorType::badInt(unsigned int i) {
	ostringstream oss ;
	oss << "'" << i << "' is out of range for the enumeration 'ProcessorType'.";
	return oss.str();
}

namespace ProcessorTypeMod {
	std::ostream & operator << ( std::ostream & out, const ProcessorType& value) {
		out << CProcessorType::name(value);
		return out;
	}

	std::istream & operator >> ( std::istream & in , ProcessorType& value ) {
		in.clear();
		string s ; 
  		in >> s;
  		try {
    		value = CProcessorType::literal(s);
  		}
  		catch (string & m) {
    		in.setstate(ios::failbit);
  		}
  		return in;
  	}
}

