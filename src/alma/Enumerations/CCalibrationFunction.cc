
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
 * File CCalibrationFunction.cpp
 */
#include <sstream>
#include <alma/Enumerations/CCalibrationFunction.h>
#include <string>
using namespace std;

int CCalibrationFunction::version() {
	return CalibrationFunctionMod::version;
	}
	
string CCalibrationFunction::revision () {
	return CalibrationFunctionMod::revision;
}

unsigned int CCalibrationFunction::size() {
	return 3;
	}
	
	
const std::string& CCalibrationFunction::sFIRST = "FIRST";
	
const std::string& CCalibrationFunction::sLAST = "LAST";
	
const std::string& CCalibrationFunction::sUNSPECIFIED = "UNSPECIFIED";
	
const std::vector<std::string> CCalibrationFunction::names() {
    std::vector<std::string> enumSet;
    
    enumSet.insert(enumSet.end(), CCalibrationFunction::sFIRST);
    
    enumSet.insert(enumSet.end(), CCalibrationFunction::sLAST);
    
    enumSet.insert(enumSet.end(), CCalibrationFunction::sUNSPECIFIED);
        
    return enumSet;
}

std::string CCalibrationFunction::name(const CalibrationFunctionMod::CalibrationFunction& f) {
    switch (f) {
    
    case CalibrationFunctionMod::FIRST:
      return CCalibrationFunction::sFIRST;
    
    case CalibrationFunctionMod::LAST:
      return CCalibrationFunction::sLAST;
    
    case CalibrationFunctionMod::UNSPECIFIED:
      return CCalibrationFunction::sUNSPECIFIED;
    	
    }
    // Impossible siutation but....who knows with C++ enums
    throw badInt((int) f);
}

CalibrationFunctionMod::CalibrationFunction CCalibrationFunction::newCalibrationFunction(const std::string& name) {
		
    if (name == CCalibrationFunction::sFIRST) {
        return CalibrationFunctionMod::FIRST;
    }
    	
    if (name == CCalibrationFunction::sLAST) {
        return CalibrationFunctionMod::LAST;
    }
    	
    if (name == CCalibrationFunction::sUNSPECIFIED) {
        return CalibrationFunctionMod::UNSPECIFIED;
    }
    
    throw badString(name);
}

CalibrationFunctionMod::CalibrationFunction CCalibrationFunction::literal(const std::string& name) {
		
    if (name == CCalibrationFunction::sFIRST) {
        return CalibrationFunctionMod::FIRST;
    }
    	
    if (name == CCalibrationFunction::sLAST) {
        return CalibrationFunctionMod::LAST;
    }
    	
    if (name == CCalibrationFunction::sUNSPECIFIED) {
        return CalibrationFunctionMod::UNSPECIFIED;
    }
    
    throw badString(name);
}

CalibrationFunctionMod::CalibrationFunction CCalibrationFunction::from_int(unsigned int i) {
	vector<string> names_ = names();
	if (i >= names_.size()) throw badInt(i);
	return newCalibrationFunction(names_.at(i));
}

string CCalibrationFunction::badString(const string& name) {
	return "'"+name+"' does not correspond to any literal in the enumeration 'CalibrationFunction'.";
}

string CCalibrationFunction::badInt(unsigned int i) {
	ostringstream oss ;
	oss << "'" << i << "' is out of range for the enumeration 'CalibrationFunction'.";
	return oss.str();
}

namespace CalibrationFunctionMod {
	std::ostream & operator << ( std::ostream & out, const CalibrationFunction& value) {
		out << CCalibrationFunction::name(value);
		return out;
	}

	std::istream & operator >> ( std::istream & in , CalibrationFunction& value ) {
		in.clear();
		string s ; 
  		in >> s;
  		try {
    		value = CCalibrationFunction::literal(s);
  		}
  		catch (string & m) {
    		in.setstate(ios::failbit);
  		}
  		return in;
  	}
}

