
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
 * File CSynthProf.cpp
 */
#include <sstream>
#include <alma/Enumerations/CSynthProf.h>
#include <string>
using namespace std;

int CSynthProf::version() {
	return SynthProfMod::version;
	}
	
string CSynthProf::revision () {
	return SynthProfMod::revision;
}

unsigned int CSynthProf::size() {
	return 3;
	}
	
	
const std::string& CSynthProf::sNOSYNTH = "NOSYNTH";
	
const std::string& CSynthProf::sACACORR = "ACACORR";
	
const std::string& CSynthProf::sACA_CDP = "ACA_CDP";
	
const std::vector<std::string> CSynthProf::names() {
    std::vector<std::string> enumSet;
    
    enumSet.insert(enumSet.end(), CSynthProf::sNOSYNTH);
    
    enumSet.insert(enumSet.end(), CSynthProf::sACACORR);
    
    enumSet.insert(enumSet.end(), CSynthProf::sACA_CDP);
        
    return enumSet;
}

std::string CSynthProf::name(const SynthProfMod::SynthProf& f) {
    switch (f) {
    
    case SynthProfMod::NOSYNTH:
      return CSynthProf::sNOSYNTH;
    
    case SynthProfMod::ACACORR:
      return CSynthProf::sACACORR;
    
    case SynthProfMod::ACA_CDP:
      return CSynthProf::sACA_CDP;
    	
    }
    // Impossible siutation but....who knows with C++ enums
    throw badInt((int) f);
}

SynthProfMod::SynthProf CSynthProf::newSynthProf(const std::string& name) {
		
    if (name == CSynthProf::sNOSYNTH) {
        return SynthProfMod::NOSYNTH;
    }
    	
    if (name == CSynthProf::sACACORR) {
        return SynthProfMod::ACACORR;
    }
    	
    if (name == CSynthProf::sACA_CDP) {
        return SynthProfMod::ACA_CDP;
    }
    
    throw badString(name);
}

SynthProfMod::SynthProf CSynthProf::literal(const std::string& name) {
		
    if (name == CSynthProf::sNOSYNTH) {
        return SynthProfMod::NOSYNTH;
    }
    	
    if (name == CSynthProf::sACACORR) {
        return SynthProfMod::ACACORR;
    }
    	
    if (name == CSynthProf::sACA_CDP) {
        return SynthProfMod::ACA_CDP;
    }
    
    throw badString(name);
}

SynthProfMod::SynthProf CSynthProf::from_int(unsigned int i) {
	vector<string> names_ = names();
	if (i >= names_.size()) throw badInt(i);
	return newSynthProf(names_.at(i));
}

string CSynthProf::badString(const string& name) {
	return "'"+name+"' does not correspond to any literal in the enumeration 'SynthProf'.";
}

string CSynthProf::badInt(unsigned int i) {
	ostringstream oss ;
	oss << "'" << i << "' is out of range for the enumeration 'SynthProf'.";
	return oss.str();
}

namespace SynthProfMod {
	std::ostream & operator << ( std::ostream & out, const SynthProf& value) {
		out << CSynthProf::name(value);
		return out;
	}

	std::istream & operator >> ( std::istream & in , SynthProf& value ) {
		in.clear();
		string s ; 
  		in >> s;
  		try {
    		value = CSynthProf::literal(s);
  		}
  		catch (string & m) {
    		in.setstate(ios::failbit);
  		}
  		return in;
  	}
}

