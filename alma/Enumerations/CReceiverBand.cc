
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
 * File CReceiverBand.cpp
 */
#include <sstream>
#include <alma/Enumerations/CReceiverBand.h>
#include <string>
using namespace std;

int CReceiverBand::version() {
	return ReceiverBandMod::version;
	}
	
string CReceiverBand::revision () {
	return ReceiverBandMod::revision;
}

unsigned int CReceiverBand::size() {
	return 27;
	}
	
	
const std::string& CReceiverBand::sALMA_RB_01 = "ALMA_RB_01";
	
const std::string& CReceiverBand::sALMA_RB_02 = "ALMA_RB_02";
	
const std::string& CReceiverBand::sALMA_RB_03 = "ALMA_RB_03";
	
const std::string& CReceiverBand::sALMA_RB_04 = "ALMA_RB_04";
	
const std::string& CReceiverBand::sALMA_RB_05 = "ALMA_RB_05";
	
const std::string& CReceiverBand::sALMA_RB_06 = "ALMA_RB_06";
	
const std::string& CReceiverBand::sALMA_RB_07 = "ALMA_RB_07";
	
const std::string& CReceiverBand::sALMA_RB_08 = "ALMA_RB_08";
	
const std::string& CReceiverBand::sALMA_RB_09 = "ALMA_RB_09";
	
const std::string& CReceiverBand::sALMA_RB_10 = "ALMA_RB_10";
	
const std::string& CReceiverBand::sALMA_RB_ALL = "ALMA_RB_ALL";
	
const std::string& CReceiverBand::sALMA_HOLOGRAPHY_RECEIVER = "ALMA_HOLOGRAPHY_RECEIVER";
	
const std::string& CReceiverBand::sBURE_01 = "BURE_01";
	
const std::string& CReceiverBand::sBURE_02 = "BURE_02";
	
const std::string& CReceiverBand::sBURE_03 = "BURE_03";
	
const std::string& CReceiverBand::sBURE_04 = "BURE_04";
	
const std::string& CReceiverBand::sEVLA_4 = "EVLA_4";
	
const std::string& CReceiverBand::sEVLA_P = "EVLA_P";
	
const std::string& CReceiverBand::sEVLA_L = "EVLA_L";
	
const std::string& CReceiverBand::sEVLA_C = "EVLA_C";
	
const std::string& CReceiverBand::sEVLA_S = "EVLA_S";
	
const std::string& CReceiverBand::sEVLA_X = "EVLA_X";
	
const std::string& CReceiverBand::sEVLA_Ku = "EVLA_Ku";
	
const std::string& CReceiverBand::sEVLA_K = "EVLA_K";
	
const std::string& CReceiverBand::sEVLA_Ka = "EVLA_Ka";
	
const std::string& CReceiverBand::sEVLA_Q = "EVLA_Q";
	
const std::string& CReceiverBand::sUNSPECIFIED = "UNSPECIFIED";
	
const std::vector<std::string> CReceiverBand::names() {
    std::vector<std::string> enumSet;
    
    enumSet.insert(enumSet.end(), CReceiverBand::sALMA_RB_01);
    
    enumSet.insert(enumSet.end(), CReceiverBand::sALMA_RB_02);
    
    enumSet.insert(enumSet.end(), CReceiverBand::sALMA_RB_03);
    
    enumSet.insert(enumSet.end(), CReceiverBand::sALMA_RB_04);
    
    enumSet.insert(enumSet.end(), CReceiverBand::sALMA_RB_05);
    
    enumSet.insert(enumSet.end(), CReceiverBand::sALMA_RB_06);
    
    enumSet.insert(enumSet.end(), CReceiverBand::sALMA_RB_07);
    
    enumSet.insert(enumSet.end(), CReceiverBand::sALMA_RB_08);
    
    enumSet.insert(enumSet.end(), CReceiverBand::sALMA_RB_09);
    
    enumSet.insert(enumSet.end(), CReceiverBand::sALMA_RB_10);
    
    enumSet.insert(enumSet.end(), CReceiverBand::sALMA_RB_ALL);
    
    enumSet.insert(enumSet.end(), CReceiverBand::sALMA_HOLOGRAPHY_RECEIVER);
    
    enumSet.insert(enumSet.end(), CReceiverBand::sBURE_01);
    
    enumSet.insert(enumSet.end(), CReceiverBand::sBURE_02);
    
    enumSet.insert(enumSet.end(), CReceiverBand::sBURE_03);
    
    enumSet.insert(enumSet.end(), CReceiverBand::sBURE_04);
    
    enumSet.insert(enumSet.end(), CReceiverBand::sEVLA_4);
    
    enumSet.insert(enumSet.end(), CReceiverBand::sEVLA_P);
    
    enumSet.insert(enumSet.end(), CReceiverBand::sEVLA_L);
    
    enumSet.insert(enumSet.end(), CReceiverBand::sEVLA_C);
    
    enumSet.insert(enumSet.end(), CReceiverBand::sEVLA_S);
    
    enumSet.insert(enumSet.end(), CReceiverBand::sEVLA_X);
    
    enumSet.insert(enumSet.end(), CReceiverBand::sEVLA_Ku);
    
    enumSet.insert(enumSet.end(), CReceiverBand::sEVLA_K);
    
    enumSet.insert(enumSet.end(), CReceiverBand::sEVLA_Ka);
    
    enumSet.insert(enumSet.end(), CReceiverBand::sEVLA_Q);
    
    enumSet.insert(enumSet.end(), CReceiverBand::sUNSPECIFIED);
        
    return enumSet;
}

std::string CReceiverBand::name(const ReceiverBandMod::ReceiverBand& f) {
    switch (f) {
    
    case ReceiverBandMod::ALMA_RB_01:
      return CReceiverBand::sALMA_RB_01;
    
    case ReceiverBandMod::ALMA_RB_02:
      return CReceiverBand::sALMA_RB_02;
    
    case ReceiverBandMod::ALMA_RB_03:
      return CReceiverBand::sALMA_RB_03;
    
    case ReceiverBandMod::ALMA_RB_04:
      return CReceiverBand::sALMA_RB_04;
    
    case ReceiverBandMod::ALMA_RB_05:
      return CReceiverBand::sALMA_RB_05;
    
    case ReceiverBandMod::ALMA_RB_06:
      return CReceiverBand::sALMA_RB_06;
    
    case ReceiverBandMod::ALMA_RB_07:
      return CReceiverBand::sALMA_RB_07;
    
    case ReceiverBandMod::ALMA_RB_08:
      return CReceiverBand::sALMA_RB_08;
    
    case ReceiverBandMod::ALMA_RB_09:
      return CReceiverBand::sALMA_RB_09;
    
    case ReceiverBandMod::ALMA_RB_10:
      return CReceiverBand::sALMA_RB_10;
    
    case ReceiverBandMod::ALMA_RB_ALL:
      return CReceiverBand::sALMA_RB_ALL;
    
    case ReceiverBandMod::ALMA_HOLOGRAPHY_RECEIVER:
      return CReceiverBand::sALMA_HOLOGRAPHY_RECEIVER;
    
    case ReceiverBandMod::BURE_01:
      return CReceiverBand::sBURE_01;
    
    case ReceiverBandMod::BURE_02:
      return CReceiverBand::sBURE_02;
    
    case ReceiverBandMod::BURE_03:
      return CReceiverBand::sBURE_03;
    
    case ReceiverBandMod::BURE_04:
      return CReceiverBand::sBURE_04;
    
    case ReceiverBandMod::EVLA_4:
      return CReceiverBand::sEVLA_4;
    
    case ReceiverBandMod::EVLA_P:
      return CReceiverBand::sEVLA_P;
    
    case ReceiverBandMod::EVLA_L:
      return CReceiverBand::sEVLA_L;
    
    case ReceiverBandMod::EVLA_C:
      return CReceiverBand::sEVLA_C;
    
    case ReceiverBandMod::EVLA_S:
      return CReceiverBand::sEVLA_S;
    
    case ReceiverBandMod::EVLA_X:
      return CReceiverBand::sEVLA_X;
    
    case ReceiverBandMod::EVLA_Ku:
      return CReceiverBand::sEVLA_Ku;
    
    case ReceiverBandMod::EVLA_K:
      return CReceiverBand::sEVLA_K;
    
    case ReceiverBandMod::EVLA_Ka:
      return CReceiverBand::sEVLA_Ka;
    
    case ReceiverBandMod::EVLA_Q:
      return CReceiverBand::sEVLA_Q;
    
    case ReceiverBandMod::UNSPECIFIED:
      return CReceiverBand::sUNSPECIFIED;
    	
    }
    // Impossible siutation but....who knows with C++ enums
    throw badInt((int) f);
}

ReceiverBandMod::ReceiverBand CReceiverBand::newReceiverBand(const std::string& name) {
		
    if (name == CReceiverBand::sALMA_RB_01) {
        return ReceiverBandMod::ALMA_RB_01;
    }
    	
    if (name == CReceiverBand::sALMA_RB_02) {
        return ReceiverBandMod::ALMA_RB_02;
    }
    	
    if (name == CReceiverBand::sALMA_RB_03) {
        return ReceiverBandMod::ALMA_RB_03;
    }
    	
    if (name == CReceiverBand::sALMA_RB_04) {
        return ReceiverBandMod::ALMA_RB_04;
    }
    	
    if (name == CReceiverBand::sALMA_RB_05) {
        return ReceiverBandMod::ALMA_RB_05;
    }
    	
    if (name == CReceiverBand::sALMA_RB_06) {
        return ReceiverBandMod::ALMA_RB_06;
    }
    	
    if (name == CReceiverBand::sALMA_RB_07) {
        return ReceiverBandMod::ALMA_RB_07;
    }
    	
    if (name == CReceiverBand::sALMA_RB_08) {
        return ReceiverBandMod::ALMA_RB_08;
    }
    	
    if (name == CReceiverBand::sALMA_RB_09) {
        return ReceiverBandMod::ALMA_RB_09;
    }
    	
    if (name == CReceiverBand::sALMA_RB_10) {
        return ReceiverBandMod::ALMA_RB_10;
    }
    	
    if (name == CReceiverBand::sALMA_RB_ALL) {
        return ReceiverBandMod::ALMA_RB_ALL;
    }
    	
    if (name == CReceiverBand::sALMA_HOLOGRAPHY_RECEIVER) {
        return ReceiverBandMod::ALMA_HOLOGRAPHY_RECEIVER;
    }
    	
    if (name == CReceiverBand::sBURE_01) {
        return ReceiverBandMod::BURE_01;
    }
    	
    if (name == CReceiverBand::sBURE_02) {
        return ReceiverBandMod::BURE_02;
    }
    	
    if (name == CReceiverBand::sBURE_03) {
        return ReceiverBandMod::BURE_03;
    }
    	
    if (name == CReceiverBand::sBURE_04) {
        return ReceiverBandMod::BURE_04;
    }
    	
    if (name == CReceiverBand::sEVLA_4) {
        return ReceiverBandMod::EVLA_4;
    }
    	
    if (name == CReceiverBand::sEVLA_P) {
        return ReceiverBandMod::EVLA_P;
    }
    	
    if (name == CReceiverBand::sEVLA_L) {
        return ReceiverBandMod::EVLA_L;
    }
    	
    if (name == CReceiverBand::sEVLA_C) {
        return ReceiverBandMod::EVLA_C;
    }
    	
    if (name == CReceiverBand::sEVLA_S) {
        return ReceiverBandMod::EVLA_S;
    }
    	
    if (name == CReceiverBand::sEVLA_X) {
        return ReceiverBandMod::EVLA_X;
    }
    	
    if (name == CReceiverBand::sEVLA_Ku) {
        return ReceiverBandMod::EVLA_Ku;
    }
    	
    if (name == CReceiverBand::sEVLA_K) {
        return ReceiverBandMod::EVLA_K;
    }
    	
    if (name == CReceiverBand::sEVLA_Ka) {
        return ReceiverBandMod::EVLA_Ka;
    }
    	
    if (name == CReceiverBand::sEVLA_Q) {
        return ReceiverBandMod::EVLA_Q;
    }
    	
    if (name == CReceiverBand::sUNSPECIFIED) {
        return ReceiverBandMod::UNSPECIFIED;
    }
    
    throw badString(name);
}

ReceiverBandMod::ReceiverBand CReceiverBand::literal(const std::string& name) {
		
    if (name == CReceiverBand::sALMA_RB_01) {
        return ReceiverBandMod::ALMA_RB_01;
    }
    	
    if (name == CReceiverBand::sALMA_RB_02) {
        return ReceiverBandMod::ALMA_RB_02;
    }
    	
    if (name == CReceiverBand::sALMA_RB_03) {
        return ReceiverBandMod::ALMA_RB_03;
    }
    	
    if (name == CReceiverBand::sALMA_RB_04) {
        return ReceiverBandMod::ALMA_RB_04;
    }
    	
    if (name == CReceiverBand::sALMA_RB_05) {
        return ReceiverBandMod::ALMA_RB_05;
    }
    	
    if (name == CReceiverBand::sALMA_RB_06) {
        return ReceiverBandMod::ALMA_RB_06;
    }
    	
    if (name == CReceiverBand::sALMA_RB_07) {
        return ReceiverBandMod::ALMA_RB_07;
    }
    	
    if (name == CReceiverBand::sALMA_RB_08) {
        return ReceiverBandMod::ALMA_RB_08;
    }
    	
    if (name == CReceiverBand::sALMA_RB_09) {
        return ReceiverBandMod::ALMA_RB_09;
    }
    	
    if (name == CReceiverBand::sALMA_RB_10) {
        return ReceiverBandMod::ALMA_RB_10;
    }
    	
    if (name == CReceiverBand::sALMA_RB_ALL) {
        return ReceiverBandMod::ALMA_RB_ALL;
    }
    	
    if (name == CReceiverBand::sALMA_HOLOGRAPHY_RECEIVER) {
        return ReceiverBandMod::ALMA_HOLOGRAPHY_RECEIVER;
    }
    	
    if (name == CReceiverBand::sBURE_01) {
        return ReceiverBandMod::BURE_01;
    }
    	
    if (name == CReceiverBand::sBURE_02) {
        return ReceiverBandMod::BURE_02;
    }
    	
    if (name == CReceiverBand::sBURE_03) {
        return ReceiverBandMod::BURE_03;
    }
    	
    if (name == CReceiverBand::sBURE_04) {
        return ReceiverBandMod::BURE_04;
    }
    	
    if (name == CReceiverBand::sEVLA_4) {
        return ReceiverBandMod::EVLA_4;
    }
    	
    if (name == CReceiverBand::sEVLA_P) {
        return ReceiverBandMod::EVLA_P;
    }
    	
    if (name == CReceiverBand::sEVLA_L) {
        return ReceiverBandMod::EVLA_L;
    }
    	
    if (name == CReceiverBand::sEVLA_C) {
        return ReceiverBandMod::EVLA_C;
    }
    	
    if (name == CReceiverBand::sEVLA_S) {
        return ReceiverBandMod::EVLA_S;
    }
    	
    if (name == CReceiverBand::sEVLA_X) {
        return ReceiverBandMod::EVLA_X;
    }
    	
    if (name == CReceiverBand::sEVLA_Ku) {
        return ReceiverBandMod::EVLA_Ku;
    }
    	
    if (name == CReceiverBand::sEVLA_K) {
        return ReceiverBandMod::EVLA_K;
    }
    	
    if (name == CReceiverBand::sEVLA_Ka) {
        return ReceiverBandMod::EVLA_Ka;
    }
    	
    if (name == CReceiverBand::sEVLA_Q) {
        return ReceiverBandMod::EVLA_Q;
    }
    	
    if (name == CReceiverBand::sUNSPECIFIED) {
        return ReceiverBandMod::UNSPECIFIED;
    }
    
    throw badString(name);
}

ReceiverBandMod::ReceiverBand CReceiverBand::from_int(unsigned int i) {
	vector<string> names_ = names();
	if (i >= names_.size()) throw badInt(i);
	return newReceiverBand(names_.at(i));
}

string CReceiverBand::badString(const string& name) {
	return "'"+name+"' does not correspond to any literal in the enumeration 'ReceiverBand'.";
}

string CReceiverBand::badInt(unsigned int i) {
	ostringstream oss ;
	oss << "'" << i << "' is out of range for the enumeration 'ReceiverBand'.";
	return oss.str();
}

namespace ReceiverBandMod {
	std::ostream & operator << ( std::ostream & out, const ReceiverBand& value) {
		out << CReceiverBand::name(value);
		return out;
	}

	std::istream & operator >> ( std::istream & in , ReceiverBand& value ) {
		in.clear();
		string s ; 
  		in >> s;
  		try {
    		value = CReceiverBand::literal(s);
  		}
  		catch (string & m) {
    		in.setstate(ios::failbit);
  		}
  		return in;
  	}
}

