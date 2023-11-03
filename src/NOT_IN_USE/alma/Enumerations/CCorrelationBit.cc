
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
 * File CCorrelationBit.cpp
 */
#include <sstream>
#include <alma/Enumerations/CCorrelationBit.h>
#include <string>
using namespace std;

int CCorrelationBit::version() {
	return CorrelationBitMod::version;
	}
	
string CCorrelationBit::revision () {
	return CorrelationBitMod::revision;
}

unsigned int CCorrelationBit::size() {
	return 3;
	}
	
	
const std::string& CCorrelationBit::sBITS_2x2 = "BITS_2x2";
	
const std::string& CCorrelationBit::sBITS_3x3 = "BITS_3x3";
	
const std::string& CCorrelationBit::sBITS_4x4 = "BITS_4x4";
	
const std::vector<std::string> CCorrelationBit::names() {
    std::vector<std::string> enumSet;
    
    enumSet.insert(enumSet.end(), CCorrelationBit::sBITS_2x2);
    
    enumSet.insert(enumSet.end(), CCorrelationBit::sBITS_3x3);
    
    enumSet.insert(enumSet.end(), CCorrelationBit::sBITS_4x4);
        
    return enumSet;
}

std::string CCorrelationBit::name(const CorrelationBitMod::CorrelationBit& f) {
    switch (f) {
    
    case CorrelationBitMod::BITS_2x2:
      return CCorrelationBit::sBITS_2x2;
    
    case CorrelationBitMod::BITS_3x3:
      return CCorrelationBit::sBITS_3x3;
    
    case CorrelationBitMod::BITS_4x4:
      return CCorrelationBit::sBITS_4x4;
    	
    }
    // Impossible siutation but....who knows with C++ enums
    throw badInt((int) f);
}

CorrelationBitMod::CorrelationBit CCorrelationBit::newCorrelationBit(const std::string& name) {
		
    if (name == CCorrelationBit::sBITS_2x2) {
        return CorrelationBitMod::BITS_2x2;
    }
    	
    if (name == CCorrelationBit::sBITS_3x3) {
        return CorrelationBitMod::BITS_3x3;
    }
    	
    if (name == CCorrelationBit::sBITS_4x4) {
        return CorrelationBitMod::BITS_4x4;
    }
    
    throw badString(name);
}

CorrelationBitMod::CorrelationBit CCorrelationBit::literal(const std::string& name) {
		
    if (name == CCorrelationBit::sBITS_2x2) {
        return CorrelationBitMod::BITS_2x2;
    }
    	
    if (name == CCorrelationBit::sBITS_3x3) {
        return CorrelationBitMod::BITS_3x3;
    }
    	
    if (name == CCorrelationBit::sBITS_4x4) {
        return CorrelationBitMod::BITS_4x4;
    }
    
    throw badString(name);
}

CorrelationBitMod::CorrelationBit CCorrelationBit::from_int(unsigned int i) {
	vector<string> names_ = names();
	if (i >= names_.size()) throw badInt(i);
	return newCorrelationBit(names_.at(i));
}

string CCorrelationBit::badString(const string& name) {
	return "'"+name+"' does not correspond to any literal in the enumeration 'CorrelationBit'.";
}

string CCorrelationBit::badInt(unsigned int i) {
	ostringstream oss ;
	oss << "'" << i << "' is out of range for the enumeration 'CorrelationBit'.";
	return oss.str();
}

namespace CorrelationBitMod {
	std::ostream & operator << ( std::ostream & out, const CorrelationBit& value) {
		out << CCorrelationBit::name(value);
		return out;
	}

	std::istream & operator >> ( std::istream & in , CorrelationBit& value ) {
		in.clear();
		string s ; 
  		in >> s;
  		try {
    		value = CCorrelationBit::literal(s);
  		}
  		catch (string & m) {
    		in.setstate(ios::failbit);
  		}
  		return in;
  	}
}

