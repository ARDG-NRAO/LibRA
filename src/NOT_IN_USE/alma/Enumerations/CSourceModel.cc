
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
 * File CSourceModel.cpp
 */
#include <sstream>
#include <alma/Enumerations/CSourceModel.h>
#include <string>
using namespace std;

int CSourceModel::version() {
	return SourceModelMod::version;
	}
	
string CSourceModel::revision () {
	return SourceModelMod::revision;
}

unsigned int CSourceModel::size() {
	return 3;
	}
	
	
const std::string& CSourceModel::sGAUSSIAN = "GAUSSIAN";
	
const std::string& CSourceModel::sPOINT = "POINT";
	
const std::string& CSourceModel::sDISK = "DISK";
	
const std::vector<std::string> CSourceModel::names() {
    std::vector<std::string> enumSet;
    
    enumSet.insert(enumSet.end(), CSourceModel::sGAUSSIAN);
    
    enumSet.insert(enumSet.end(), CSourceModel::sPOINT);
    
    enumSet.insert(enumSet.end(), CSourceModel::sDISK);
        
    return enumSet;
}

std::string CSourceModel::name(const SourceModelMod::SourceModel& f) {
    switch (f) {
    
    case SourceModelMod::GAUSSIAN:
      return CSourceModel::sGAUSSIAN;
    
    case SourceModelMod::POINT:
      return CSourceModel::sPOINT;
    
    case SourceModelMod::DISK:
      return CSourceModel::sDISK;
    	
    }
    // Impossible siutation but....who knows with C++ enums
    throw badInt((int) f);
}

SourceModelMod::SourceModel CSourceModel::newSourceModel(const std::string& name) {
		
    if (name == CSourceModel::sGAUSSIAN) {
        return SourceModelMod::GAUSSIAN;
    }
    	
    if (name == CSourceModel::sPOINT) {
        return SourceModelMod::POINT;
    }
    	
    if (name == CSourceModel::sDISK) {
        return SourceModelMod::DISK;
    }
    
    throw badString(name);
}

SourceModelMod::SourceModel CSourceModel::literal(const std::string& name) {
		
    if (name == CSourceModel::sGAUSSIAN) {
        return SourceModelMod::GAUSSIAN;
    }
    	
    if (name == CSourceModel::sPOINT) {
        return SourceModelMod::POINT;
    }
    	
    if (name == CSourceModel::sDISK) {
        return SourceModelMod::DISK;
    }
    
    throw badString(name);
}

SourceModelMod::SourceModel CSourceModel::from_int(unsigned int i) {
	vector<string> names_ = names();
	if (i >= names_.size()) throw badInt(i);
	return newSourceModel(names_.at(i));
}

string CSourceModel::badString(const string& name) {
	return "'"+name+"' does not correspond to any literal in the enumeration 'SourceModel'.";
}

string CSourceModel::badInt(unsigned int i) {
	ostringstream oss ;
	oss << "'" << i << "' is out of range for the enumeration 'SourceModel'.";
	return oss.str();
}

namespace SourceModelMod {
	std::ostream & operator << ( std::ostream & out, const SourceModel& value) {
		out << CSourceModel::name(value);
		return out;
	}

	std::istream & operator >> ( std::istream & in , SourceModel& value ) {
		in.clear();
		string s ; 
  		in >> s;
  		try {
    		value = CSourceModel::literal(s);
  		}
  		catch (string & m) {
    		in.setstate(ios::failbit);
  		}
  		return in;
  	}
}

