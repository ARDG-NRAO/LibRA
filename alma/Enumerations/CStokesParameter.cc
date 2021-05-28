
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
 * File CStokesParameter.cpp
 */
#include <sstream>
#include <alma/Enumerations/CStokesParameter.h>
#include <string>
using namespace std;

int CStokesParameter::version() {
	return StokesParameterMod::version;
	}
	
string CStokesParameter::revision () {
	return StokesParameterMod::revision;
}

unsigned int CStokesParameter::size() {
	return 32;
	}
	
	
const std::string& CStokesParameter::sI = "I";
	
const std::string& CStokesParameter::sQ = "Q";
	
const std::string& CStokesParameter::sU = "U";
	
const std::string& CStokesParameter::sV = "V";
	
const std::string& CStokesParameter::sRR = "RR";
	
const std::string& CStokesParameter::sRL = "RL";
	
const std::string& CStokesParameter::sLR = "LR";
	
const std::string& CStokesParameter::sLL = "LL";
	
const std::string& CStokesParameter::sXX = "XX";
	
const std::string& CStokesParameter::sXY = "XY";
	
const std::string& CStokesParameter::sYX = "YX";
	
const std::string& CStokesParameter::sYY = "YY";
	
const std::string& CStokesParameter::sRX = "RX";
	
const std::string& CStokesParameter::sRY = "RY";
	
const std::string& CStokesParameter::sLX = "LX";
	
const std::string& CStokesParameter::sLY = "LY";
	
const std::string& CStokesParameter::sXR = "XR";
	
const std::string& CStokesParameter::sXL = "XL";
	
const std::string& CStokesParameter::sYR = "YR";
	
const std::string& CStokesParameter::sYL = "YL";
	
const std::string& CStokesParameter::sPP = "PP";
	
const std::string& CStokesParameter::sPQ = "PQ";
	
const std::string& CStokesParameter::sQP = "QP";
	
const std::string& CStokesParameter::sQQ = "QQ";
	
const std::string& CStokesParameter::sRCIRCULAR = "RCIRCULAR";
	
const std::string& CStokesParameter::sLCIRCULAR = "LCIRCULAR";
	
const std::string& CStokesParameter::sLINEAR = "LINEAR";
	
const std::string& CStokesParameter::sPTOTAL = "PTOTAL";
	
const std::string& CStokesParameter::sPLINEAR = "PLINEAR";
	
const std::string& CStokesParameter::sPFTOTAL = "PFTOTAL";
	
const std::string& CStokesParameter::sPFLINEAR = "PFLINEAR";
	
const std::string& CStokesParameter::sPANGLE = "PANGLE";
	
const std::vector<std::string> CStokesParameter::names() {
    std::vector<std::string> enumSet;
    
    enumSet.insert(enumSet.end(), CStokesParameter::sI);
    
    enumSet.insert(enumSet.end(), CStokesParameter::sQ);
    
    enumSet.insert(enumSet.end(), CStokesParameter::sU);
    
    enumSet.insert(enumSet.end(), CStokesParameter::sV);
    
    enumSet.insert(enumSet.end(), CStokesParameter::sRR);
    
    enumSet.insert(enumSet.end(), CStokesParameter::sRL);
    
    enumSet.insert(enumSet.end(), CStokesParameter::sLR);
    
    enumSet.insert(enumSet.end(), CStokesParameter::sLL);
    
    enumSet.insert(enumSet.end(), CStokesParameter::sXX);
    
    enumSet.insert(enumSet.end(), CStokesParameter::sXY);
    
    enumSet.insert(enumSet.end(), CStokesParameter::sYX);
    
    enumSet.insert(enumSet.end(), CStokesParameter::sYY);
    
    enumSet.insert(enumSet.end(), CStokesParameter::sRX);
    
    enumSet.insert(enumSet.end(), CStokesParameter::sRY);
    
    enumSet.insert(enumSet.end(), CStokesParameter::sLX);
    
    enumSet.insert(enumSet.end(), CStokesParameter::sLY);
    
    enumSet.insert(enumSet.end(), CStokesParameter::sXR);
    
    enumSet.insert(enumSet.end(), CStokesParameter::sXL);
    
    enumSet.insert(enumSet.end(), CStokesParameter::sYR);
    
    enumSet.insert(enumSet.end(), CStokesParameter::sYL);
    
    enumSet.insert(enumSet.end(), CStokesParameter::sPP);
    
    enumSet.insert(enumSet.end(), CStokesParameter::sPQ);
    
    enumSet.insert(enumSet.end(), CStokesParameter::sQP);
    
    enumSet.insert(enumSet.end(), CStokesParameter::sQQ);
    
    enumSet.insert(enumSet.end(), CStokesParameter::sRCIRCULAR);
    
    enumSet.insert(enumSet.end(), CStokesParameter::sLCIRCULAR);
    
    enumSet.insert(enumSet.end(), CStokesParameter::sLINEAR);
    
    enumSet.insert(enumSet.end(), CStokesParameter::sPTOTAL);
    
    enumSet.insert(enumSet.end(), CStokesParameter::sPLINEAR);
    
    enumSet.insert(enumSet.end(), CStokesParameter::sPFTOTAL);
    
    enumSet.insert(enumSet.end(), CStokesParameter::sPFLINEAR);
    
    enumSet.insert(enumSet.end(), CStokesParameter::sPANGLE);
        
    return enumSet;
}

std::string CStokesParameter::name(const StokesParameterMod::StokesParameter& f) {
    switch (f) {
    
    case StokesParameterMod::I:
      return CStokesParameter::sI;
    
    case StokesParameterMod::Q:
      return CStokesParameter::sQ;
    
    case StokesParameterMod::U:
      return CStokesParameter::sU;
    
    case StokesParameterMod::V:
      return CStokesParameter::sV;
    
    case StokesParameterMod::RR:
      return CStokesParameter::sRR;
    
    case StokesParameterMod::RL:
      return CStokesParameter::sRL;
    
    case StokesParameterMod::LR:
      return CStokesParameter::sLR;
    
    case StokesParameterMod::LL:
      return CStokesParameter::sLL;
    
    case StokesParameterMod::XX:
      return CStokesParameter::sXX;
    
    case StokesParameterMod::XY:
      return CStokesParameter::sXY;
    
    case StokesParameterMod::YX:
      return CStokesParameter::sYX;
    
    case StokesParameterMod::YY:
      return CStokesParameter::sYY;
    
    case StokesParameterMod::RX:
      return CStokesParameter::sRX;
    
    case StokesParameterMod::RY:
      return CStokesParameter::sRY;
    
    case StokesParameterMod::LX:
      return CStokesParameter::sLX;
    
    case StokesParameterMod::LY:
      return CStokesParameter::sLY;
    
    case StokesParameterMod::XR:
      return CStokesParameter::sXR;
    
    case StokesParameterMod::XL:
      return CStokesParameter::sXL;
    
    case StokesParameterMod::YR:
      return CStokesParameter::sYR;
    
    case StokesParameterMod::YL:
      return CStokesParameter::sYL;
    
    case StokesParameterMod::PP:
      return CStokesParameter::sPP;
    
    case StokesParameterMod::PQ:
      return CStokesParameter::sPQ;
    
    case StokesParameterMod::QP:
      return CStokesParameter::sQP;
    
    case StokesParameterMod::QQ:
      return CStokesParameter::sQQ;
    
    case StokesParameterMod::RCIRCULAR:
      return CStokesParameter::sRCIRCULAR;
    
    case StokesParameterMod::LCIRCULAR:
      return CStokesParameter::sLCIRCULAR;
    
    case StokesParameterMod::LINEAR:
      return CStokesParameter::sLINEAR;
    
    case StokesParameterMod::PTOTAL:
      return CStokesParameter::sPTOTAL;
    
    case StokesParameterMod::PLINEAR:
      return CStokesParameter::sPLINEAR;
    
    case StokesParameterMod::PFTOTAL:
      return CStokesParameter::sPFTOTAL;
    
    case StokesParameterMod::PFLINEAR:
      return CStokesParameter::sPFLINEAR;
    
    case StokesParameterMod::PANGLE:
      return CStokesParameter::sPANGLE;
    	
    }
    // Impossible siutation but....who knows with C++ enums
    throw badInt((int) f);
}

StokesParameterMod::StokesParameter CStokesParameter::newStokesParameter(const std::string& name) {
		
    if (name == CStokesParameter::sI) {
        return StokesParameterMod::I;
    }
    	
    if (name == CStokesParameter::sQ) {
        return StokesParameterMod::Q;
    }
    	
    if (name == CStokesParameter::sU) {
        return StokesParameterMod::U;
    }
    	
    if (name == CStokesParameter::sV) {
        return StokesParameterMod::V;
    }
    	
    if (name == CStokesParameter::sRR) {
        return StokesParameterMod::RR;
    }
    	
    if (name == CStokesParameter::sRL) {
        return StokesParameterMod::RL;
    }
    	
    if (name == CStokesParameter::sLR) {
        return StokesParameterMod::LR;
    }
    	
    if (name == CStokesParameter::sLL) {
        return StokesParameterMod::LL;
    }
    	
    if (name == CStokesParameter::sXX) {
        return StokesParameterMod::XX;
    }
    	
    if (name == CStokesParameter::sXY) {
        return StokesParameterMod::XY;
    }
    	
    if (name == CStokesParameter::sYX) {
        return StokesParameterMod::YX;
    }
    	
    if (name == CStokesParameter::sYY) {
        return StokesParameterMod::YY;
    }
    	
    if (name == CStokesParameter::sRX) {
        return StokesParameterMod::RX;
    }
    	
    if (name == CStokesParameter::sRY) {
        return StokesParameterMod::RY;
    }
    	
    if (name == CStokesParameter::sLX) {
        return StokesParameterMod::LX;
    }
    	
    if (name == CStokesParameter::sLY) {
        return StokesParameterMod::LY;
    }
    	
    if (name == CStokesParameter::sXR) {
        return StokesParameterMod::XR;
    }
    	
    if (name == CStokesParameter::sXL) {
        return StokesParameterMod::XL;
    }
    	
    if (name == CStokesParameter::sYR) {
        return StokesParameterMod::YR;
    }
    	
    if (name == CStokesParameter::sYL) {
        return StokesParameterMod::YL;
    }
    	
    if (name == CStokesParameter::sPP) {
        return StokesParameterMod::PP;
    }
    	
    if (name == CStokesParameter::sPQ) {
        return StokesParameterMod::PQ;
    }
    	
    if (name == CStokesParameter::sQP) {
        return StokesParameterMod::QP;
    }
    	
    if (name == CStokesParameter::sQQ) {
        return StokesParameterMod::QQ;
    }
    	
    if (name == CStokesParameter::sRCIRCULAR) {
        return StokesParameterMod::RCIRCULAR;
    }
    	
    if (name == CStokesParameter::sLCIRCULAR) {
        return StokesParameterMod::LCIRCULAR;
    }
    	
    if (name == CStokesParameter::sLINEAR) {
        return StokesParameterMod::LINEAR;
    }
    	
    if (name == CStokesParameter::sPTOTAL) {
        return StokesParameterMod::PTOTAL;
    }
    	
    if (name == CStokesParameter::sPLINEAR) {
        return StokesParameterMod::PLINEAR;
    }
    	
    if (name == CStokesParameter::sPFTOTAL) {
        return StokesParameterMod::PFTOTAL;
    }
    	
    if (name == CStokesParameter::sPFLINEAR) {
        return StokesParameterMod::PFLINEAR;
    }
    	
    if (name == CStokesParameter::sPANGLE) {
        return StokesParameterMod::PANGLE;
    }
    
    throw badString(name);
}

StokesParameterMod::StokesParameter CStokesParameter::literal(const std::string& name) {
		
    if (name == CStokesParameter::sI) {
        return StokesParameterMod::I;
    }
    	
    if (name == CStokesParameter::sQ) {
        return StokesParameterMod::Q;
    }
    	
    if (name == CStokesParameter::sU) {
        return StokesParameterMod::U;
    }
    	
    if (name == CStokesParameter::sV) {
        return StokesParameterMod::V;
    }
    	
    if (name == CStokesParameter::sRR) {
        return StokesParameterMod::RR;
    }
    	
    if (name == CStokesParameter::sRL) {
        return StokesParameterMod::RL;
    }
    	
    if (name == CStokesParameter::sLR) {
        return StokesParameterMod::LR;
    }
    	
    if (name == CStokesParameter::sLL) {
        return StokesParameterMod::LL;
    }
    	
    if (name == CStokesParameter::sXX) {
        return StokesParameterMod::XX;
    }
    	
    if (name == CStokesParameter::sXY) {
        return StokesParameterMod::XY;
    }
    	
    if (name == CStokesParameter::sYX) {
        return StokesParameterMod::YX;
    }
    	
    if (name == CStokesParameter::sYY) {
        return StokesParameterMod::YY;
    }
    	
    if (name == CStokesParameter::sRX) {
        return StokesParameterMod::RX;
    }
    	
    if (name == CStokesParameter::sRY) {
        return StokesParameterMod::RY;
    }
    	
    if (name == CStokesParameter::sLX) {
        return StokesParameterMod::LX;
    }
    	
    if (name == CStokesParameter::sLY) {
        return StokesParameterMod::LY;
    }
    	
    if (name == CStokesParameter::sXR) {
        return StokesParameterMod::XR;
    }
    	
    if (name == CStokesParameter::sXL) {
        return StokesParameterMod::XL;
    }
    	
    if (name == CStokesParameter::sYR) {
        return StokesParameterMod::YR;
    }
    	
    if (name == CStokesParameter::sYL) {
        return StokesParameterMod::YL;
    }
    	
    if (name == CStokesParameter::sPP) {
        return StokesParameterMod::PP;
    }
    	
    if (name == CStokesParameter::sPQ) {
        return StokesParameterMod::PQ;
    }
    	
    if (name == CStokesParameter::sQP) {
        return StokesParameterMod::QP;
    }
    	
    if (name == CStokesParameter::sQQ) {
        return StokesParameterMod::QQ;
    }
    	
    if (name == CStokesParameter::sRCIRCULAR) {
        return StokesParameterMod::RCIRCULAR;
    }
    	
    if (name == CStokesParameter::sLCIRCULAR) {
        return StokesParameterMod::LCIRCULAR;
    }
    	
    if (name == CStokesParameter::sLINEAR) {
        return StokesParameterMod::LINEAR;
    }
    	
    if (name == CStokesParameter::sPTOTAL) {
        return StokesParameterMod::PTOTAL;
    }
    	
    if (name == CStokesParameter::sPLINEAR) {
        return StokesParameterMod::PLINEAR;
    }
    	
    if (name == CStokesParameter::sPFTOTAL) {
        return StokesParameterMod::PFTOTAL;
    }
    	
    if (name == CStokesParameter::sPFLINEAR) {
        return StokesParameterMod::PFLINEAR;
    }
    	
    if (name == CStokesParameter::sPANGLE) {
        return StokesParameterMod::PANGLE;
    }
    
    throw badString(name);
}

StokesParameterMod::StokesParameter CStokesParameter::from_int(unsigned int i) {
	vector<string> names_ = names();
	if (i >= names_.size()) throw badInt(i);
	return newStokesParameter(names_.at(i));
}

string CStokesParameter::badString(const string& name) {
	return "'"+name+"' does not correspond to any literal in the enumeration 'StokesParameter'.";
}

string CStokesParameter::badInt(unsigned int i) {
	ostringstream oss ;
	oss << "'" << i << "' is out of range for the enumeration 'StokesParameter'.";
	return oss.str();
}

namespace StokesParameterMod {
	std::ostream & operator << ( std::ostream & out, const StokesParameter& value) {
		out << CStokesParameter::name(value);
		return out;
	}

	std::istream & operator >> ( std::istream & in , StokesParameter& value ) {
		in.clear();
		string s ; 
  		in >> s;
  		try {
    		value = CStokesParameter::literal(s);
  		}
  		catch (string & m) {
    		in.setstate(ios::failbit);
  		}
  		return in;
  	}
}

