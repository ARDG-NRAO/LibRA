
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
 * File CSchedulerMode.cpp
 */
#include <sstream>
#include <alma/Enumerations/CSchedulerMode.h>
#include <string>
using namespace std;

int CSchedulerMode::version() {
	return SchedulerModeMod::version;
	}
	
string CSchedulerMode::revision () {
	return SchedulerModeMod::revision;
}

unsigned int CSchedulerMode::size() {
	return 4;
	}
	
	
const std::string& CSchedulerMode::sDYNAMIC = "DYNAMIC";
	
const std::string& CSchedulerMode::sINTERACTIVE = "INTERACTIVE";
	
const std::string& CSchedulerMode::sMANUAL = "MANUAL";
	
const std::string& CSchedulerMode::sQUEUED = "QUEUED";
	
const std::vector<std::string> CSchedulerMode::names() {
    std::vector<std::string> enumSet;
    
    enumSet.insert(enumSet.end(), CSchedulerMode::sDYNAMIC);
    
    enumSet.insert(enumSet.end(), CSchedulerMode::sINTERACTIVE);
    
    enumSet.insert(enumSet.end(), CSchedulerMode::sMANUAL);
    
    enumSet.insert(enumSet.end(), CSchedulerMode::sQUEUED);
        
    return enumSet;
}

std::string CSchedulerMode::name(const SchedulerModeMod::SchedulerMode& f) {
    switch (f) {
    
    case SchedulerModeMod::DYNAMIC:
      return CSchedulerMode::sDYNAMIC;
    
    case SchedulerModeMod::INTERACTIVE:
      return CSchedulerMode::sINTERACTIVE;
    
    case SchedulerModeMod::MANUAL:
      return CSchedulerMode::sMANUAL;
    
    case SchedulerModeMod::QUEUED:
      return CSchedulerMode::sQUEUED;
    	
    }
    // Impossible siutation but....who knows with C++ enums
    throw badInt((int) f);
}

SchedulerModeMod::SchedulerMode CSchedulerMode::newSchedulerMode(const std::string& name) {
		
    if (name == CSchedulerMode::sDYNAMIC) {
        return SchedulerModeMod::DYNAMIC;
    }
    	
    if (name == CSchedulerMode::sINTERACTIVE) {
        return SchedulerModeMod::INTERACTIVE;
    }
    	
    if (name == CSchedulerMode::sMANUAL) {
        return SchedulerModeMod::MANUAL;
    }
    	
    if (name == CSchedulerMode::sQUEUED) {
        return SchedulerModeMod::QUEUED;
    }
    
    throw badString(name);
}

SchedulerModeMod::SchedulerMode CSchedulerMode::literal(const std::string& name) {
		
    if (name == CSchedulerMode::sDYNAMIC) {
        return SchedulerModeMod::DYNAMIC;
    }
    	
    if (name == CSchedulerMode::sINTERACTIVE) {
        return SchedulerModeMod::INTERACTIVE;
    }
    	
    if (name == CSchedulerMode::sMANUAL) {
        return SchedulerModeMod::MANUAL;
    }
    	
    if (name == CSchedulerMode::sQUEUED) {
        return SchedulerModeMod::QUEUED;
    }
    
    throw badString(name);
}

SchedulerModeMod::SchedulerMode CSchedulerMode::from_int(unsigned int i) {
	vector<string> names_ = names();
	if (i >= names_.size()) throw badInt(i);
	return newSchedulerMode(names_.at(i));
}

string CSchedulerMode::badString(const string& name) {
	return "'"+name+"' does not correspond to any literal in the enumeration 'SchedulerMode'.";
}

string CSchedulerMode::badInt(unsigned int i) {
	ostringstream oss ;
	oss << "'" << i << "' is out of range for the enumeration 'SchedulerMode'.";
	return oss.str();
}

namespace SchedulerModeMod {
	std::ostream & operator << ( std::ostream & out, const SchedulerMode& value) {
		out << CSchedulerMode::name(value);
		return out;
	}

	std::istream & operator >> ( std::istream & in , SchedulerMode& value ) {
		in.clear();
		string s ; 
  		in >> s;
  		try {
    		value = CSchedulerMode::literal(s);
  		}
  		catch (string & m) {
    		in.setstate(ios::failbit);
  		}
  		return in;
  	}
}

