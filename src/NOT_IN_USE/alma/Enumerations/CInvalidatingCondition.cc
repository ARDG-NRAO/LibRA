
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
 * File CInvalidatingCondition.cpp
 */
#include <sstream>
#include <alma/Enumerations/CInvalidatingCondition.h>
#include <string>
using namespace std;

int CInvalidatingCondition::version() {
	return InvalidatingConditionMod::version;
	}
	
string CInvalidatingCondition::revision () {
	return InvalidatingConditionMod::revision;
}

unsigned int CInvalidatingCondition::size() {
	return 5;
	}
	
	
const std::string& CInvalidatingCondition::sANTENNA_DISCONNECT = "ANTENNA_DISCONNECT";
	
const std::string& CInvalidatingCondition::sANTENNA_MOVE = "ANTENNA_MOVE";
	
const std::string& CInvalidatingCondition::sANTENNA_POWER_DOWN = "ANTENNA_POWER_DOWN";
	
const std::string& CInvalidatingCondition::sRECEIVER_EXCHANGE = "RECEIVER_EXCHANGE";
	
const std::string& CInvalidatingCondition::sRECEIVER_POWER_DOWN = "RECEIVER_POWER_DOWN";
	
const std::vector<std::string> CInvalidatingCondition::names() {
    std::vector<std::string> enumSet;
    
    enumSet.insert(enumSet.end(), CInvalidatingCondition::sANTENNA_DISCONNECT);
    
    enumSet.insert(enumSet.end(), CInvalidatingCondition::sANTENNA_MOVE);
    
    enumSet.insert(enumSet.end(), CInvalidatingCondition::sANTENNA_POWER_DOWN);
    
    enumSet.insert(enumSet.end(), CInvalidatingCondition::sRECEIVER_EXCHANGE);
    
    enumSet.insert(enumSet.end(), CInvalidatingCondition::sRECEIVER_POWER_DOWN);
        
    return enumSet;
}

std::string CInvalidatingCondition::name(const InvalidatingConditionMod::InvalidatingCondition& f) {
    switch (f) {
    
    case InvalidatingConditionMod::ANTENNA_DISCONNECT:
      return CInvalidatingCondition::sANTENNA_DISCONNECT;
    
    case InvalidatingConditionMod::ANTENNA_MOVE:
      return CInvalidatingCondition::sANTENNA_MOVE;
    
    case InvalidatingConditionMod::ANTENNA_POWER_DOWN:
      return CInvalidatingCondition::sANTENNA_POWER_DOWN;
    
    case InvalidatingConditionMod::RECEIVER_EXCHANGE:
      return CInvalidatingCondition::sRECEIVER_EXCHANGE;
    
    case InvalidatingConditionMod::RECEIVER_POWER_DOWN:
      return CInvalidatingCondition::sRECEIVER_POWER_DOWN;
    	
    }
    // Impossible siutation but....who knows with C++ enums
    throw badInt((int) f);
}

InvalidatingConditionMod::InvalidatingCondition CInvalidatingCondition::newInvalidatingCondition(const std::string& name) {
		
    if (name == CInvalidatingCondition::sANTENNA_DISCONNECT) {
        return InvalidatingConditionMod::ANTENNA_DISCONNECT;
    }
    	
    if (name == CInvalidatingCondition::sANTENNA_MOVE) {
        return InvalidatingConditionMod::ANTENNA_MOVE;
    }
    	
    if (name == CInvalidatingCondition::sANTENNA_POWER_DOWN) {
        return InvalidatingConditionMod::ANTENNA_POWER_DOWN;
    }
    	
    if (name == CInvalidatingCondition::sRECEIVER_EXCHANGE) {
        return InvalidatingConditionMod::RECEIVER_EXCHANGE;
    }
    	
    if (name == CInvalidatingCondition::sRECEIVER_POWER_DOWN) {
        return InvalidatingConditionMod::RECEIVER_POWER_DOWN;
    }
    
    throw badString(name);
}

InvalidatingConditionMod::InvalidatingCondition CInvalidatingCondition::literal(const std::string& name) {
		
    if (name == CInvalidatingCondition::sANTENNA_DISCONNECT) {
        return InvalidatingConditionMod::ANTENNA_DISCONNECT;
    }
    	
    if (name == CInvalidatingCondition::sANTENNA_MOVE) {
        return InvalidatingConditionMod::ANTENNA_MOVE;
    }
    	
    if (name == CInvalidatingCondition::sANTENNA_POWER_DOWN) {
        return InvalidatingConditionMod::ANTENNA_POWER_DOWN;
    }
    	
    if (name == CInvalidatingCondition::sRECEIVER_EXCHANGE) {
        return InvalidatingConditionMod::RECEIVER_EXCHANGE;
    }
    	
    if (name == CInvalidatingCondition::sRECEIVER_POWER_DOWN) {
        return InvalidatingConditionMod::RECEIVER_POWER_DOWN;
    }
    
    throw badString(name);
}

InvalidatingConditionMod::InvalidatingCondition CInvalidatingCondition::from_int(unsigned int i) {
	vector<string> names_ = names();
	if (i >= names_.size()) throw badInt(i);
	return newInvalidatingCondition(names_.at(i));
}

string CInvalidatingCondition::badString(const string& name) {
	return "'"+name+"' does not correspond to any literal in the enumeration 'InvalidatingCondition'.";
}

string CInvalidatingCondition::badInt(unsigned int i) {
	ostringstream oss ;
	oss << "'" << i << "' is out of range for the enumeration 'InvalidatingCondition'.";
	return oss.str();
}

namespace InvalidatingConditionMod {
	std::ostream & operator << ( std::ostream & out, const InvalidatingCondition& value) {
		out << CInvalidatingCondition::name(value);
		return out;
	}

	std::istream & operator >> ( std::istream & in , InvalidatingCondition& value ) {
		in.clear();
		string s ; 
  		in >> s;
  		try {
    		value = CInvalidatingCondition::literal(s);
  		}
  		catch (string & m) {
    		in.setstate(ios::failbit);
  		}
  		return in;
  	}
}

