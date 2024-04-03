
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
 * File CWindowFunction.cpp
 */
#include <sstream>
#include <alma/Enumerations/CWindowFunction.h>
#include <string>
using namespace std;

int CWindowFunction::version() {
	return WindowFunctionMod::version;
	}
	
string CWindowFunction::revision () {
	return WindowFunctionMod::revision;
}

unsigned int CWindowFunction::size() {
	return 7;
	}
	
	
const std::string& CWindowFunction::sUNIFORM = "UNIFORM";
	
const std::string& CWindowFunction::sHANNING = "HANNING";
	
const std::string& CWindowFunction::sHAMMING = "HAMMING";
	
const std::string& CWindowFunction::sBARTLETT = "BARTLETT";
	
const std::string& CWindowFunction::sBLACKMANN = "BLACKMANN";
	
const std::string& CWindowFunction::sBLACKMANN_HARRIS = "BLACKMANN_HARRIS";
	
const std::string& CWindowFunction::sWELCH = "WELCH";
	
const std::vector<std::string> CWindowFunction::names() {
    std::vector<std::string> enumSet;
    
    enumSet.insert(enumSet.end(), CWindowFunction::sUNIFORM);
    
    enumSet.insert(enumSet.end(), CWindowFunction::sHANNING);
    
    enumSet.insert(enumSet.end(), CWindowFunction::sHAMMING);
    
    enumSet.insert(enumSet.end(), CWindowFunction::sBARTLETT);
    
    enumSet.insert(enumSet.end(), CWindowFunction::sBLACKMANN);
    
    enumSet.insert(enumSet.end(), CWindowFunction::sBLACKMANN_HARRIS);
    
    enumSet.insert(enumSet.end(), CWindowFunction::sWELCH);
        
    return enumSet;
}

std::string CWindowFunction::name(const WindowFunctionMod::WindowFunction& f) {
    switch (f) {
    
    case WindowFunctionMod::UNIFORM:
      return CWindowFunction::sUNIFORM;
    
    case WindowFunctionMod::HANNING:
      return CWindowFunction::sHANNING;
    
    case WindowFunctionMod::HAMMING:
      return CWindowFunction::sHAMMING;
    
    case WindowFunctionMod::BARTLETT:
      return CWindowFunction::sBARTLETT;
    
    case WindowFunctionMod::BLACKMANN:
      return CWindowFunction::sBLACKMANN;
    
    case WindowFunctionMod::BLACKMANN_HARRIS:
      return CWindowFunction::sBLACKMANN_HARRIS;
    
    case WindowFunctionMod::WELCH:
      return CWindowFunction::sWELCH;
    	
    }
    // Impossible siutation but....who knows with C++ enums
    throw badInt((int) f);
}

WindowFunctionMod::WindowFunction CWindowFunction::newWindowFunction(const std::string& name) {
		
    if (name == CWindowFunction::sUNIFORM) {
        return WindowFunctionMod::UNIFORM;
    }
    	
    if (name == CWindowFunction::sHANNING) {
        return WindowFunctionMod::HANNING;
    }
    	
    if (name == CWindowFunction::sHAMMING) {
        return WindowFunctionMod::HAMMING;
    }
    	
    if (name == CWindowFunction::sBARTLETT) {
        return WindowFunctionMod::BARTLETT;
    }
    	
    if (name == CWindowFunction::sBLACKMANN) {
        return WindowFunctionMod::BLACKMANN;
    }
    	
    if (name == CWindowFunction::sBLACKMANN_HARRIS) {
        return WindowFunctionMod::BLACKMANN_HARRIS;
    }
    	
    if (name == CWindowFunction::sWELCH) {
        return WindowFunctionMod::WELCH;
    }
    
    throw badString(name);
}

WindowFunctionMod::WindowFunction CWindowFunction::literal(const std::string& name) {
		
    if (name == CWindowFunction::sUNIFORM) {
        return WindowFunctionMod::UNIFORM;
    }
    	
    if (name == CWindowFunction::sHANNING) {
        return WindowFunctionMod::HANNING;
    }
    	
    if (name == CWindowFunction::sHAMMING) {
        return WindowFunctionMod::HAMMING;
    }
    	
    if (name == CWindowFunction::sBARTLETT) {
        return WindowFunctionMod::BARTLETT;
    }
    	
    if (name == CWindowFunction::sBLACKMANN) {
        return WindowFunctionMod::BLACKMANN;
    }
    	
    if (name == CWindowFunction::sBLACKMANN_HARRIS) {
        return WindowFunctionMod::BLACKMANN_HARRIS;
    }
    	
    if (name == CWindowFunction::sWELCH) {
        return WindowFunctionMod::WELCH;
    }
    
    throw badString(name);
}

WindowFunctionMod::WindowFunction CWindowFunction::from_int(unsigned int i) {
	vector<string> names_ = names();
	if (i >= names_.size()) throw badInt(i);
	return newWindowFunction(names_.at(i));
}

string CWindowFunction::badString(const string& name) {
	return "'"+name+"' does not correspond to any literal in the enumeration 'WindowFunction'.";
}

string CWindowFunction::badInt(unsigned int i) {
	ostringstream oss ;
	oss << "'" << i << "' is out of range for the enumeration 'WindowFunction'.";
	return oss.str();
}

namespace WindowFunctionMod {
	std::ostream & operator << ( std::ostream & out, const WindowFunction& value) {
		out << CWindowFunction::name(value);
		return out;
	}

	std::istream & operator >> ( std::istream & in , WindowFunction& value ) {
		in.clear();
		string s ; 
  		in >> s;
  		try {
    		value = CWindowFunction::literal(s);
  		}
  		catch (string & m) {
    		in.setstate(ios::failbit);
  		}
  		return in;
  	}
}

