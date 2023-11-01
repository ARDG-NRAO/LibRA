
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
 * File CAntennaMake.cpp
 */
#include <sstream>
#include <alma/Enumerations/CAntennaMake.h>
#include <string>
using namespace std;

int CAntennaMake::version() {
	return AntennaMakeMod::version;
	}
	
string CAntennaMake::revision () {
	return AntennaMakeMod::revision;
}

unsigned int CAntennaMake::size() {
	return 9;
	}
	
	
const std::string& CAntennaMake::sAEM_12 = "AEM_12";
	
const std::string& CAntennaMake::sMITSUBISHI_7 = "MITSUBISHI_7";
	
const std::string& CAntennaMake::sMITSUBISHI_12_A = "MITSUBISHI_12_A";
	
const std::string& CAntennaMake::sMITSUBISHI_12_B = "MITSUBISHI_12_B";
	
const std::string& CAntennaMake::sVERTEX_12_ATF = "VERTEX_12_ATF";
	
const std::string& CAntennaMake::sAEM_12_ATF = "AEM_12_ATF";
	
const std::string& CAntennaMake::sVERTEX_12 = "VERTEX_12";
	
const std::string& CAntennaMake::sIRAM_15 = "IRAM_15";
	
const std::string& CAntennaMake::sUNDEFINED = "UNDEFINED";
	
const std::vector<std::string> CAntennaMake::names() {
    std::vector<std::string> enumSet;
    
    enumSet.insert(enumSet.end(), CAntennaMake::sAEM_12);
    
    enumSet.insert(enumSet.end(), CAntennaMake::sMITSUBISHI_7);
    
    enumSet.insert(enumSet.end(), CAntennaMake::sMITSUBISHI_12_A);
    
    enumSet.insert(enumSet.end(), CAntennaMake::sMITSUBISHI_12_B);
    
    enumSet.insert(enumSet.end(), CAntennaMake::sVERTEX_12_ATF);
    
    enumSet.insert(enumSet.end(), CAntennaMake::sAEM_12_ATF);
    
    enumSet.insert(enumSet.end(), CAntennaMake::sVERTEX_12);
    
    enumSet.insert(enumSet.end(), CAntennaMake::sIRAM_15);
    
    enumSet.insert(enumSet.end(), CAntennaMake::sUNDEFINED);
        
    return enumSet;
}

std::string CAntennaMake::name(const AntennaMakeMod::AntennaMake& f) {
    switch (f) {
    
    case AntennaMakeMod::AEM_12:
      return CAntennaMake::sAEM_12;
    
    case AntennaMakeMod::MITSUBISHI_7:
      return CAntennaMake::sMITSUBISHI_7;
    
    case AntennaMakeMod::MITSUBISHI_12_A:
      return CAntennaMake::sMITSUBISHI_12_A;
    
    case AntennaMakeMod::MITSUBISHI_12_B:
      return CAntennaMake::sMITSUBISHI_12_B;
    
    case AntennaMakeMod::VERTEX_12_ATF:
      return CAntennaMake::sVERTEX_12_ATF;
    
    case AntennaMakeMod::AEM_12_ATF:
      return CAntennaMake::sAEM_12_ATF;
    
    case AntennaMakeMod::VERTEX_12:
      return CAntennaMake::sVERTEX_12;
    
    case AntennaMakeMod::IRAM_15:
      return CAntennaMake::sIRAM_15;
    
    case AntennaMakeMod::UNDEFINED:
      return CAntennaMake::sUNDEFINED;
    	
    }
    // Impossible siutation but....who knows with C++ enums
    throw badInt((int) f);
}

AntennaMakeMod::AntennaMake CAntennaMake::newAntennaMake(const std::string& name) {
		
    if (name == CAntennaMake::sAEM_12) {
        return AntennaMakeMod::AEM_12;
    }
    	
    if (name == CAntennaMake::sMITSUBISHI_7) {
        return AntennaMakeMod::MITSUBISHI_7;
    }
    	
    if (name == CAntennaMake::sMITSUBISHI_12_A) {
        return AntennaMakeMod::MITSUBISHI_12_A;
    }
    	
    if (name == CAntennaMake::sMITSUBISHI_12_B) {
        return AntennaMakeMod::MITSUBISHI_12_B;
    }
    	
    if (name == CAntennaMake::sVERTEX_12_ATF) {
        return AntennaMakeMod::VERTEX_12_ATF;
    }
    	
    if (name == CAntennaMake::sAEM_12_ATF) {
        return AntennaMakeMod::AEM_12_ATF;
    }
    	
    if (name == CAntennaMake::sVERTEX_12) {
        return AntennaMakeMod::VERTEX_12;
    }
    	
    if (name == CAntennaMake::sIRAM_15) {
        return AntennaMakeMod::IRAM_15;
    }
    	
    if (name == CAntennaMake::sUNDEFINED) {
        return AntennaMakeMod::UNDEFINED;
    }
    
    throw badString(name);
}

AntennaMakeMod::AntennaMake CAntennaMake::literal(const std::string& name) {
		
    if (name == CAntennaMake::sAEM_12) {
        return AntennaMakeMod::AEM_12;
    }
    	
    if (name == CAntennaMake::sMITSUBISHI_7) {
        return AntennaMakeMod::MITSUBISHI_7;
    }
    	
    if (name == CAntennaMake::sMITSUBISHI_12_A) {
        return AntennaMakeMod::MITSUBISHI_12_A;
    }
    	
    if (name == CAntennaMake::sMITSUBISHI_12_B) {
        return AntennaMakeMod::MITSUBISHI_12_B;
    }
    	
    if (name == CAntennaMake::sVERTEX_12_ATF) {
        return AntennaMakeMod::VERTEX_12_ATF;
    }
    	
    if (name == CAntennaMake::sAEM_12_ATF) {
        return AntennaMakeMod::AEM_12_ATF;
    }
    	
    if (name == CAntennaMake::sVERTEX_12) {
        return AntennaMakeMod::VERTEX_12;
    }
    	
    if (name == CAntennaMake::sIRAM_15) {
        return AntennaMakeMod::IRAM_15;
    }
    	
    if (name == CAntennaMake::sUNDEFINED) {
        return AntennaMakeMod::UNDEFINED;
    }
    
    throw badString(name);
}

AntennaMakeMod::AntennaMake CAntennaMake::from_int(unsigned int i) {
	vector<string> names_ = names();
	if (i >= names_.size()) throw badInt(i);
	return newAntennaMake(names_.at(i));
}

string CAntennaMake::badString(const string& name) {
	return "'"+name+"' does not correspond to any literal in the enumeration 'AntennaMake'.";
}

string CAntennaMake::badInt(unsigned int i) {
	ostringstream oss ;
	oss << "'" << i << "' is out of range for the enumeration 'AntennaMake'.";
	return oss.str();
}

namespace AntennaMakeMod {
	std::ostream & operator << ( std::ostream & out, const AntennaMake& value) {
		out << CAntennaMake::name(value);
		return out;
	}

	std::istream & operator >> ( std::istream & in , AntennaMake& value ) {
		in.clear();
		string s ; 
  		in >> s;
  		try {
    		value = CAntennaMake::literal(s);
  		}
  		catch (string & m) {
    		in.setstate(ios::failbit);
  		}
  		return in;
  	}
}

