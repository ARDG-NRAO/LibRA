
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
 * File CReceiverSideband.cpp
 */
#include <sstream>
#include <alma/Enumerations/CReceiverSideband.h>
#include <string>
using namespace std;

int CReceiverSideband::version() {
	return ReceiverSidebandMod::version;
	}
	
string CReceiverSideband::revision () {
	return ReceiverSidebandMod::revision;
}

unsigned int CReceiverSideband::size() {
	return 4;
	}
	
	
const std::string& CReceiverSideband::sNOSB = "NOSB";
	
const std::string& CReceiverSideband::sDSB = "DSB";
	
const std::string& CReceiverSideband::sSSB = "SSB";
	
const std::string& CReceiverSideband::sTSB = "TSB";
	
const std::vector<std::string> CReceiverSideband::names() {
    std::vector<std::string> enumSet;
    
    enumSet.insert(enumSet.end(), CReceiverSideband::sNOSB);
    
    enumSet.insert(enumSet.end(), CReceiverSideband::sDSB);
    
    enumSet.insert(enumSet.end(), CReceiverSideband::sSSB);
    
    enumSet.insert(enumSet.end(), CReceiverSideband::sTSB);
        
    return enumSet;
}

std::string CReceiverSideband::name(const ReceiverSidebandMod::ReceiverSideband& f) {
    switch (f) {
    
    case ReceiverSidebandMod::NOSB:
      return CReceiverSideband::sNOSB;
    
    case ReceiverSidebandMod::DSB:
      return CReceiverSideband::sDSB;
    
    case ReceiverSidebandMod::SSB:
      return CReceiverSideband::sSSB;
    
    case ReceiverSidebandMod::TSB:
      return CReceiverSideband::sTSB;
    	
    }
    // Impossible siutation but....who knows with C++ enums
    throw badInt((int) f);
}

ReceiverSidebandMod::ReceiverSideband CReceiverSideband::newReceiverSideband(const std::string& name) {
		
    if (name == CReceiverSideband::sNOSB) {
        return ReceiverSidebandMod::NOSB;
    }
    	
    if (name == CReceiverSideband::sDSB) {
        return ReceiverSidebandMod::DSB;
    }
    	
    if (name == CReceiverSideband::sSSB) {
        return ReceiverSidebandMod::SSB;
    }
    	
    if (name == CReceiverSideband::sTSB) {
        return ReceiverSidebandMod::TSB;
    }
    
    throw badString(name);
}

ReceiverSidebandMod::ReceiverSideband CReceiverSideband::literal(const std::string& name) {
		
    if (name == CReceiverSideband::sNOSB) {
        return ReceiverSidebandMod::NOSB;
    }
    	
    if (name == CReceiverSideband::sDSB) {
        return ReceiverSidebandMod::DSB;
    }
    	
    if (name == CReceiverSideband::sSSB) {
        return ReceiverSidebandMod::SSB;
    }
    	
    if (name == CReceiverSideband::sTSB) {
        return ReceiverSidebandMod::TSB;
    }
    
    throw badString(name);
}

ReceiverSidebandMod::ReceiverSideband CReceiverSideband::from_int(unsigned int i) {
	vector<string> names_ = names();
	if (i >= names_.size()) throw badInt(i);
	return newReceiverSideband(names_.at(i));
}

string CReceiverSideband::badString(const string& name) {
	return "'"+name+"' does not correspond to any literal in the enumeration 'ReceiverSideband'.";
}

string CReceiverSideband::badInt(unsigned int i) {
	ostringstream oss ;
	oss << "'" << i << "' is out of range for the enumeration 'ReceiverSideband'.";
	return oss.str();
}

namespace ReceiverSidebandMod {
	std::ostream & operator << ( std::ostream & out, const ReceiverSideband& value) {
		out << CReceiverSideband::name(value);
		return out;
	}

	std::istream & operator >> ( std::istream & in , ReceiverSideband& value ) {
		in.clear();
		string s ; 
  		in >> s;
  		try {
    		value = CReceiverSideband::literal(s);
  		}
  		catch (string & m) {
    		in.setstate(ios::failbit);
  		}
  		return in;
  	}
}

