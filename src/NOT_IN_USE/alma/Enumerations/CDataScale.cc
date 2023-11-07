
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
 * File CDataScale.cpp
 */
#include <sstream>
#include <alma/Enumerations/CDataScale.h>
#include <string>
using namespace std;

int CDataScale::version() {
	return DataScaleMod::version;
	}
	
string CDataScale::revision () {
	return DataScaleMod::revision;
}

unsigned int CDataScale::size() {
	return 4;
	}
	
	
const std::string& CDataScale::sK = "K";
	
const std::string& CDataScale::sJY = "JY";
	
const std::string& CDataScale::sCORRELATION = "CORRELATION";
	
const std::string& CDataScale::sCORRELATION_COEFFICIENT = "CORRELATION_COEFFICIENT";
	
const std::vector<std::string> CDataScale::names() {
    std::vector<std::string> enumSet;
    
    enumSet.insert(enumSet.end(), CDataScale::sK);
    
    enumSet.insert(enumSet.end(), CDataScale::sJY);
    
    enumSet.insert(enumSet.end(), CDataScale::sCORRELATION);
    
    enumSet.insert(enumSet.end(), CDataScale::sCORRELATION_COEFFICIENT);
        
    return enumSet;
}

std::string CDataScale::name(const DataScaleMod::DataScale& f) {
    switch (f) {
    
    case DataScaleMod::K:
      return CDataScale::sK;
    
    case DataScaleMod::JY:
      return CDataScale::sJY;
    
    case DataScaleMod::CORRELATION:
      return CDataScale::sCORRELATION;
    
    case DataScaleMod::CORRELATION_COEFFICIENT:
      return CDataScale::sCORRELATION_COEFFICIENT;
    	
    }
    // Impossible siutation but....who knows with C++ enums
    throw badInt((int) f);
}

DataScaleMod::DataScale CDataScale::newDataScale(const std::string& name) {
		
    if (name == CDataScale::sK) {
        return DataScaleMod::K;
    }
    	
    if (name == CDataScale::sJY) {
        return DataScaleMod::JY;
    }
    	
    if (name == CDataScale::sCORRELATION) {
        return DataScaleMod::CORRELATION;
    }
    	
    if (name == CDataScale::sCORRELATION_COEFFICIENT) {
        return DataScaleMod::CORRELATION_COEFFICIENT;
    }
    
    throw badString(name);
}

DataScaleMod::DataScale CDataScale::literal(const std::string& name) {
		
    if (name == CDataScale::sK) {
        return DataScaleMod::K;
    }
    	
    if (name == CDataScale::sJY) {
        return DataScaleMod::JY;
    }
    	
    if (name == CDataScale::sCORRELATION) {
        return DataScaleMod::CORRELATION;
    }
    	
    if (name == CDataScale::sCORRELATION_COEFFICIENT) {
        return DataScaleMod::CORRELATION_COEFFICIENT;
    }
    
    throw badString(name);
}

DataScaleMod::DataScale CDataScale::from_int(unsigned int i) {
	vector<string> names_ = names();
	if (i >= names_.size()) throw badInt(i);
	return newDataScale(names_.at(i));
}

string CDataScale::badString(const string& name) {
	return "'"+name+"' does not correspond to any literal in the enumeration 'DataScale'.";
}

string CDataScale::badInt(unsigned int i) {
	ostringstream oss ;
	oss << "'" << i << "' is out of range for the enumeration 'DataScale'.";
	return oss.str();
}

namespace DataScaleMod {
	std::ostream & operator << ( std::ostream & out, const DataScale& value) {
		out << CDataScale::name(value);
		return out;
	}

	std::istream & operator >> ( std::istream & in , DataScale& value ) {
		in.clear();
		string s ; 
  		in >> s;
  		try {
    		value = CDataScale::literal(s);
  		}
  		catch (string & m) {
    		in.setstate(ios::failbit);
  		}
  		return in;
  	}
}

