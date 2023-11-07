
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
 * File CDataContent.cpp
 */
#include <sstream>
#include <alma/Enumerations/CDataContent.h>
#include <string>
using namespace std;

int CDataContent::version() {
	return DataContentMod::version;
	}
	
string CDataContent::revision () {
	return DataContentMod::revision;
}

unsigned int CDataContent::size() {
	return 7;
	}
	
	
const std::string& CDataContent::sCROSS_DATA = "CROSS_DATA";
	
const std::string& CDataContent::sAUTO_DATA = "AUTO_DATA";
	
const std::string& CDataContent::sZERO_LAGS = "ZERO_LAGS";
	
const std::string& CDataContent::sACTUAL_TIMES = "ACTUAL_TIMES";
	
const std::string& CDataContent::sACTUAL_DURATIONS = "ACTUAL_DURATIONS";
	
const std::string& CDataContent::sWEIGHTS = "WEIGHTS";
	
const std::string& CDataContent::sFLAGS = "FLAGS";
	
const std::vector<std::string> CDataContent::names() {
    std::vector<std::string> enumSet;
    
    enumSet.insert(enumSet.end(), CDataContent::sCROSS_DATA);
    
    enumSet.insert(enumSet.end(), CDataContent::sAUTO_DATA);
    
    enumSet.insert(enumSet.end(), CDataContent::sZERO_LAGS);
    
    enumSet.insert(enumSet.end(), CDataContent::sACTUAL_TIMES);
    
    enumSet.insert(enumSet.end(), CDataContent::sACTUAL_DURATIONS);
    
    enumSet.insert(enumSet.end(), CDataContent::sWEIGHTS);
    
    enumSet.insert(enumSet.end(), CDataContent::sFLAGS);
        
    return enumSet;
}

std::string CDataContent::name(const DataContentMod::DataContent& f) {
    switch (f) {
    
    case DataContentMod::CROSS_DATA:
      return CDataContent::sCROSS_DATA;
    
    case DataContentMod::AUTO_DATA:
      return CDataContent::sAUTO_DATA;
    
    case DataContentMod::ZERO_LAGS:
      return CDataContent::sZERO_LAGS;
    
    case DataContentMod::ACTUAL_TIMES:
      return CDataContent::sACTUAL_TIMES;
    
    case DataContentMod::ACTUAL_DURATIONS:
      return CDataContent::sACTUAL_DURATIONS;
    
    case DataContentMod::WEIGHTS:
      return CDataContent::sWEIGHTS;
    
    case DataContentMod::FLAGS:
      return CDataContent::sFLAGS;
    	
    }
    // Impossible siutation but....who knows with C++ enums
    throw badInt((int) f);
}

DataContentMod::DataContent CDataContent::newDataContent(const std::string& name) {
		
    if (name == CDataContent::sCROSS_DATA) {
        return DataContentMod::CROSS_DATA;
    }
    	
    if (name == CDataContent::sAUTO_DATA) {
        return DataContentMod::AUTO_DATA;
    }
    	
    if (name == CDataContent::sZERO_LAGS) {
        return DataContentMod::ZERO_LAGS;
    }
    	
    if (name == CDataContent::sACTUAL_TIMES) {
        return DataContentMod::ACTUAL_TIMES;
    }
    	
    if (name == CDataContent::sACTUAL_DURATIONS) {
        return DataContentMod::ACTUAL_DURATIONS;
    }
    	
    if (name == CDataContent::sWEIGHTS) {
        return DataContentMod::WEIGHTS;
    }
    	
    if (name == CDataContent::sFLAGS) {
        return DataContentMod::FLAGS;
    }
    
    throw badString(name);
}

DataContentMod::DataContent CDataContent::literal(const std::string& name) {
		
    if (name == CDataContent::sCROSS_DATA) {
        return DataContentMod::CROSS_DATA;
    }
    	
    if (name == CDataContent::sAUTO_DATA) {
        return DataContentMod::AUTO_DATA;
    }
    	
    if (name == CDataContent::sZERO_LAGS) {
        return DataContentMod::ZERO_LAGS;
    }
    	
    if (name == CDataContent::sACTUAL_TIMES) {
        return DataContentMod::ACTUAL_TIMES;
    }
    	
    if (name == CDataContent::sACTUAL_DURATIONS) {
        return DataContentMod::ACTUAL_DURATIONS;
    }
    	
    if (name == CDataContent::sWEIGHTS) {
        return DataContentMod::WEIGHTS;
    }
    	
    if (name == CDataContent::sFLAGS) {
        return DataContentMod::FLAGS;
    }
    
    throw badString(name);
}

DataContentMod::DataContent CDataContent::from_int(unsigned int i) {
	vector<string> names_ = names();
	if (i >= names_.size()) throw badInt(i);
	return newDataContent(names_.at(i));
}

string CDataContent::badString(const string& name) {
	return "'"+name+"' does not correspond to any literal in the enumeration 'DataContent'.";
}

string CDataContent::badInt(unsigned int i) {
	ostringstream oss ;
	oss << "'" << i << "' is out of range for the enumeration 'DataContent'.";
	return oss.str();
}

namespace DataContentMod {
	std::ostream & operator << ( std::ostream & out, const DataContent& value) {
		out << CDataContent::name(value);
		return out;
	}

	std::istream & operator >> ( std::istream & in , DataContent& value ) {
		in.clear();
		string s ; 
  		in >> s;
  		try {
    		value = CDataContent::literal(s);
  		}
  		catch (string & m) {
    		in.setstate(ios::failbit);
  		}
  		return in;
  	}
}

