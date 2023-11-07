
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
 * File CHolographyChannelType.cpp
 */
#include <sstream>
#include <alma/Enumerations/CHolographyChannelType.h>
#include <string>
using namespace std;

int CHolographyChannelType::version() {
	return HolographyChannelTypeMod::version;
	}
	
string CHolographyChannelType::revision () {
	return HolographyChannelTypeMod::revision;
}

unsigned int CHolographyChannelType::size() {
	return 6;
	}
	
	
const std::string& CHolographyChannelType::sQ2 = "Q2";
	
const std::string& CHolographyChannelType::sQR = "QR";
	
const std::string& CHolographyChannelType::sQS = "QS";
	
const std::string& CHolographyChannelType::sR2 = "R2";
	
const std::string& CHolographyChannelType::sRS = "RS";
	
const std::string& CHolographyChannelType::sS2 = "S2";
	
const std::vector<std::string> CHolographyChannelType::names() {
    std::vector<std::string> enumSet;
    
    enumSet.insert(enumSet.end(), CHolographyChannelType::sQ2);
    
    enumSet.insert(enumSet.end(), CHolographyChannelType::sQR);
    
    enumSet.insert(enumSet.end(), CHolographyChannelType::sQS);
    
    enumSet.insert(enumSet.end(), CHolographyChannelType::sR2);
    
    enumSet.insert(enumSet.end(), CHolographyChannelType::sRS);
    
    enumSet.insert(enumSet.end(), CHolographyChannelType::sS2);
        
    return enumSet;
}

std::string CHolographyChannelType::name(const HolographyChannelTypeMod::HolographyChannelType& f) {
    switch (f) {
    
    case HolographyChannelTypeMod::Q2:
      return CHolographyChannelType::sQ2;
    
    case HolographyChannelTypeMod::QR:
      return CHolographyChannelType::sQR;
    
    case HolographyChannelTypeMod::QS:
      return CHolographyChannelType::sQS;
    
    case HolographyChannelTypeMod::R2:
      return CHolographyChannelType::sR2;
    
    case HolographyChannelTypeMod::RS:
      return CHolographyChannelType::sRS;
    
    case HolographyChannelTypeMod::S2:
      return CHolographyChannelType::sS2;
    	
    }
    // Impossible siutation but....who knows with C++ enums
    throw badInt((int) f);
}

HolographyChannelTypeMod::HolographyChannelType CHolographyChannelType::newHolographyChannelType(const std::string& name) {
		
    if (name == CHolographyChannelType::sQ2) {
        return HolographyChannelTypeMod::Q2;
    }
    	
    if (name == CHolographyChannelType::sQR) {
        return HolographyChannelTypeMod::QR;
    }
    	
    if (name == CHolographyChannelType::sQS) {
        return HolographyChannelTypeMod::QS;
    }
    	
    if (name == CHolographyChannelType::sR2) {
        return HolographyChannelTypeMod::R2;
    }
    	
    if (name == CHolographyChannelType::sRS) {
        return HolographyChannelTypeMod::RS;
    }
    	
    if (name == CHolographyChannelType::sS2) {
        return HolographyChannelTypeMod::S2;
    }
    
    throw badString(name);
}

HolographyChannelTypeMod::HolographyChannelType CHolographyChannelType::literal(const std::string& name) {
		
    if (name == CHolographyChannelType::sQ2) {
        return HolographyChannelTypeMod::Q2;
    }
    	
    if (name == CHolographyChannelType::sQR) {
        return HolographyChannelTypeMod::QR;
    }
    	
    if (name == CHolographyChannelType::sQS) {
        return HolographyChannelTypeMod::QS;
    }
    	
    if (name == CHolographyChannelType::sR2) {
        return HolographyChannelTypeMod::R2;
    }
    	
    if (name == CHolographyChannelType::sRS) {
        return HolographyChannelTypeMod::RS;
    }
    	
    if (name == CHolographyChannelType::sS2) {
        return HolographyChannelTypeMod::S2;
    }
    
    throw badString(name);
}

HolographyChannelTypeMod::HolographyChannelType CHolographyChannelType::from_int(unsigned int i) {
	vector<string> names_ = names();
	if (i >= names_.size()) throw badInt(i);
	return newHolographyChannelType(names_.at(i));
}

string CHolographyChannelType::badString(const string& name) {
	return "'"+name+"' does not correspond to any literal in the enumeration 'HolographyChannelType'.";
}

string CHolographyChannelType::badInt(unsigned int i) {
	ostringstream oss ;
	oss << "'" << i << "' is out of range for the enumeration 'HolographyChannelType'.";
	return oss.str();
}

namespace HolographyChannelTypeMod {
	std::ostream & operator << ( std::ostream & out, const HolographyChannelType& value) {
		out << CHolographyChannelType::name(value);
		return out;
	}

	std::istream & operator >> ( std::istream & in , HolographyChannelType& value ) {
		in.clear();
		string s ; 
  		in >> s;
  		try {
    		value = CHolographyChannelType::literal(s);
  		}
  		catch (string & m) {
    		in.setstate(ios::failbit);
  		}
  		return in;
  	}
}

