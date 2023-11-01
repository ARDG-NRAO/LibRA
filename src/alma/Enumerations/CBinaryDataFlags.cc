
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
 * File CBinaryDataFlags.cpp
 */
#include <sstream>
#include <alma/Enumerations/CBinaryDataFlags.h>
#include <string>
using namespace std;

int CBinaryDataFlags::version() {
	return BinaryDataFlagsMod::version;
	}
	
string CBinaryDataFlags::revision () {
	return BinaryDataFlagsMod::revision;
}

unsigned int CBinaryDataFlags::size() {
	return 32;
	}
	
	
const std::string& CBinaryDataFlags::sINTEGRATION_FULLY_BLANKED = "INTEGRATION_FULLY_BLANKED";
	
const std::string& CBinaryDataFlags::sWVR_APC = "WVR_APC";
	
const std::string& CBinaryDataFlags::sCORRELATOR_MISSING_STATUS = "CORRELATOR_MISSING_STATUS";
	
const std::string& CBinaryDataFlags::sMISSING_ANTENNA_EVENT = "MISSING_ANTENNA_EVENT";
	
const std::string& CBinaryDataFlags::sDELTA_SIGMA_OVERFLOW = "DELTA_SIGMA_OVERFLOW";
	
const std::string& CBinaryDataFlags::sDELAY_CORRECTION_NOT_APPLIED = "DELAY_CORRECTION_NOT_APPLIED";
	
const std::string& CBinaryDataFlags::sSYNCRONIZATION_ERROR = "SYNCRONIZATION_ERROR";
	
const std::string& CBinaryDataFlags::sFFT_OVERFLOW = "FFT_OVERFLOW";
	
const std::string& CBinaryDataFlags::sTFB_SCALING_FACTOR_NOT_RETRIEVED = "TFB_SCALING_FACTOR_NOT_RETRIEVED";
	
const std::string& CBinaryDataFlags::sZERO_LAG_NOT_RECEIVED = "ZERO_LAG_NOT_RECEIVED";
	
const std::string& CBinaryDataFlags::sSIGMA_OVERFLOW = "SIGMA_OVERFLOW";
	
const std::string& CBinaryDataFlags::sUNUSABLE_CAI_OUTPUT = "UNUSABLE_CAI_OUTPUT";
	
const std::string& CBinaryDataFlags::sQC_FAILED = "QC_FAILED";
	
const std::string& CBinaryDataFlags::sNOISY_TDM_CHANNELS = "NOISY_TDM_CHANNELS";
	
const std::string& CBinaryDataFlags::sSPECTRAL_NORMALIZATION_FAILED = "SPECTRAL_NORMALIZATION_FAILED";
	
const std::string& CBinaryDataFlags::sDROPPED_PACKETS = "DROPPED_PACKETS";
	
const std::string& CBinaryDataFlags::sDETECTOR_SATURATED = "DETECTOR_SATURATED";
	
const std::string& CBinaryDataFlags::sNO_DATA_FROM_DIGITAL_POWER_METER = "NO_DATA_FROM_DIGITAL_POWER_METER";
	
const std::string& CBinaryDataFlags::sRESERVED_18 = "RESERVED_18";
	
const std::string& CBinaryDataFlags::sRESERVED_19 = "RESERVED_19";
	
const std::string& CBinaryDataFlags::sRESERVED_20 = "RESERVED_20";
	
const std::string& CBinaryDataFlags::sRESERVED_21 = "RESERVED_21";
	
const std::string& CBinaryDataFlags::sRESERVED_22 = "RESERVED_22";
	
const std::string& CBinaryDataFlags::sRESERVED_23 = "RESERVED_23";
	
const std::string& CBinaryDataFlags::sRESERVED_24 = "RESERVED_24";
	
const std::string& CBinaryDataFlags::sRESERVED_25 = "RESERVED_25";
	
const std::string& CBinaryDataFlags::sRESERVED_26 = "RESERVED_26";
	
const std::string& CBinaryDataFlags::sRESERVED_27 = "RESERVED_27";
	
const std::string& CBinaryDataFlags::sRESERVED_28 = "RESERVED_28";
	
const std::string& CBinaryDataFlags::sRESERVED_29 = "RESERVED_29";
	
const std::string& CBinaryDataFlags::sRESERVED_30 = "RESERVED_30";
	
const std::string& CBinaryDataFlags::sALL_PURPOSE_ERROR = "ALL_PURPOSE_ERROR";
	
const std::vector<std::string> CBinaryDataFlags::names() {
    std::vector<std::string> enumSet;
    
    enumSet.insert(enumSet.end(), CBinaryDataFlags::sINTEGRATION_FULLY_BLANKED);
    
    enumSet.insert(enumSet.end(), CBinaryDataFlags::sWVR_APC);
    
    enumSet.insert(enumSet.end(), CBinaryDataFlags::sCORRELATOR_MISSING_STATUS);
    
    enumSet.insert(enumSet.end(), CBinaryDataFlags::sMISSING_ANTENNA_EVENT);
    
    enumSet.insert(enumSet.end(), CBinaryDataFlags::sDELTA_SIGMA_OVERFLOW);
    
    enumSet.insert(enumSet.end(), CBinaryDataFlags::sDELAY_CORRECTION_NOT_APPLIED);
    
    enumSet.insert(enumSet.end(), CBinaryDataFlags::sSYNCRONIZATION_ERROR);
    
    enumSet.insert(enumSet.end(), CBinaryDataFlags::sFFT_OVERFLOW);
    
    enumSet.insert(enumSet.end(), CBinaryDataFlags::sTFB_SCALING_FACTOR_NOT_RETRIEVED);
    
    enumSet.insert(enumSet.end(), CBinaryDataFlags::sZERO_LAG_NOT_RECEIVED);
    
    enumSet.insert(enumSet.end(), CBinaryDataFlags::sSIGMA_OVERFLOW);
    
    enumSet.insert(enumSet.end(), CBinaryDataFlags::sUNUSABLE_CAI_OUTPUT);
    
    enumSet.insert(enumSet.end(), CBinaryDataFlags::sQC_FAILED);
    
    enumSet.insert(enumSet.end(), CBinaryDataFlags::sNOISY_TDM_CHANNELS);
    
    enumSet.insert(enumSet.end(), CBinaryDataFlags::sSPECTRAL_NORMALIZATION_FAILED);
    
    enumSet.insert(enumSet.end(), CBinaryDataFlags::sDROPPED_PACKETS);
    
    enumSet.insert(enumSet.end(), CBinaryDataFlags::sDETECTOR_SATURATED);
    
    enumSet.insert(enumSet.end(), CBinaryDataFlags::sNO_DATA_FROM_DIGITAL_POWER_METER);
    
    enumSet.insert(enumSet.end(), CBinaryDataFlags::sRESERVED_18);
    
    enumSet.insert(enumSet.end(), CBinaryDataFlags::sRESERVED_19);
    
    enumSet.insert(enumSet.end(), CBinaryDataFlags::sRESERVED_20);
    
    enumSet.insert(enumSet.end(), CBinaryDataFlags::sRESERVED_21);
    
    enumSet.insert(enumSet.end(), CBinaryDataFlags::sRESERVED_22);
    
    enumSet.insert(enumSet.end(), CBinaryDataFlags::sRESERVED_23);
    
    enumSet.insert(enumSet.end(), CBinaryDataFlags::sRESERVED_24);
    
    enumSet.insert(enumSet.end(), CBinaryDataFlags::sRESERVED_25);
    
    enumSet.insert(enumSet.end(), CBinaryDataFlags::sRESERVED_26);
    
    enumSet.insert(enumSet.end(), CBinaryDataFlags::sRESERVED_27);
    
    enumSet.insert(enumSet.end(), CBinaryDataFlags::sRESERVED_28);
    
    enumSet.insert(enumSet.end(), CBinaryDataFlags::sRESERVED_29);
    
    enumSet.insert(enumSet.end(), CBinaryDataFlags::sRESERVED_30);
    
    enumSet.insert(enumSet.end(), CBinaryDataFlags::sALL_PURPOSE_ERROR);
        
    return enumSet;
}

std::string CBinaryDataFlags::name(const BinaryDataFlagsMod::BinaryDataFlags& f) {
    switch (f) {
    
    case BinaryDataFlagsMod::INTEGRATION_FULLY_BLANKED:
      return CBinaryDataFlags::sINTEGRATION_FULLY_BLANKED;
    
    case BinaryDataFlagsMod::WVR_APC:
      return CBinaryDataFlags::sWVR_APC;
    
    case BinaryDataFlagsMod::CORRELATOR_MISSING_STATUS:
      return CBinaryDataFlags::sCORRELATOR_MISSING_STATUS;
    
    case BinaryDataFlagsMod::MISSING_ANTENNA_EVENT:
      return CBinaryDataFlags::sMISSING_ANTENNA_EVENT;
    
    case BinaryDataFlagsMod::DELTA_SIGMA_OVERFLOW:
      return CBinaryDataFlags::sDELTA_SIGMA_OVERFLOW;
    
    case BinaryDataFlagsMod::DELAY_CORRECTION_NOT_APPLIED:
      return CBinaryDataFlags::sDELAY_CORRECTION_NOT_APPLIED;
    
    case BinaryDataFlagsMod::SYNCRONIZATION_ERROR:
      return CBinaryDataFlags::sSYNCRONIZATION_ERROR;
    
    case BinaryDataFlagsMod::FFT_OVERFLOW:
      return CBinaryDataFlags::sFFT_OVERFLOW;
    
    case BinaryDataFlagsMod::TFB_SCALING_FACTOR_NOT_RETRIEVED:
      return CBinaryDataFlags::sTFB_SCALING_FACTOR_NOT_RETRIEVED;
    
    case BinaryDataFlagsMod::ZERO_LAG_NOT_RECEIVED:
      return CBinaryDataFlags::sZERO_LAG_NOT_RECEIVED;
    
    case BinaryDataFlagsMod::SIGMA_OVERFLOW:
      return CBinaryDataFlags::sSIGMA_OVERFLOW;
    
    case BinaryDataFlagsMod::UNUSABLE_CAI_OUTPUT:
      return CBinaryDataFlags::sUNUSABLE_CAI_OUTPUT;
    
    case BinaryDataFlagsMod::QC_FAILED:
      return CBinaryDataFlags::sQC_FAILED;
    
    case BinaryDataFlagsMod::NOISY_TDM_CHANNELS:
      return CBinaryDataFlags::sNOISY_TDM_CHANNELS;
    
    case BinaryDataFlagsMod::SPECTRAL_NORMALIZATION_FAILED:
      return CBinaryDataFlags::sSPECTRAL_NORMALIZATION_FAILED;
    
    case BinaryDataFlagsMod::DROPPED_PACKETS:
      return CBinaryDataFlags::sDROPPED_PACKETS;
    
    case BinaryDataFlagsMod::DETECTOR_SATURATED:
      return CBinaryDataFlags::sDETECTOR_SATURATED;
    
    case BinaryDataFlagsMod::NO_DATA_FROM_DIGITAL_POWER_METER:
      return CBinaryDataFlags::sNO_DATA_FROM_DIGITAL_POWER_METER;
    
    case BinaryDataFlagsMod::RESERVED_18:
      return CBinaryDataFlags::sRESERVED_18;
    
    case BinaryDataFlagsMod::RESERVED_19:
      return CBinaryDataFlags::sRESERVED_19;
    
    case BinaryDataFlagsMod::RESERVED_20:
      return CBinaryDataFlags::sRESERVED_20;
    
    case BinaryDataFlagsMod::RESERVED_21:
      return CBinaryDataFlags::sRESERVED_21;
    
    case BinaryDataFlagsMod::RESERVED_22:
      return CBinaryDataFlags::sRESERVED_22;
    
    case BinaryDataFlagsMod::RESERVED_23:
      return CBinaryDataFlags::sRESERVED_23;
    
    case BinaryDataFlagsMod::RESERVED_24:
      return CBinaryDataFlags::sRESERVED_24;
    
    case BinaryDataFlagsMod::RESERVED_25:
      return CBinaryDataFlags::sRESERVED_25;
    
    case BinaryDataFlagsMod::RESERVED_26:
      return CBinaryDataFlags::sRESERVED_26;
    
    case BinaryDataFlagsMod::RESERVED_27:
      return CBinaryDataFlags::sRESERVED_27;
    
    case BinaryDataFlagsMod::RESERVED_28:
      return CBinaryDataFlags::sRESERVED_28;
    
    case BinaryDataFlagsMod::RESERVED_29:
      return CBinaryDataFlags::sRESERVED_29;
    
    case BinaryDataFlagsMod::RESERVED_30:
      return CBinaryDataFlags::sRESERVED_30;
    
    case BinaryDataFlagsMod::ALL_PURPOSE_ERROR:
      return CBinaryDataFlags::sALL_PURPOSE_ERROR;
    	
    }
    // Impossible siutation but....who knows with C++ enums
    throw badInt((int) f);
}

BinaryDataFlagsMod::BinaryDataFlags CBinaryDataFlags::newBinaryDataFlags(const std::string& name) {
		
    if (name == CBinaryDataFlags::sINTEGRATION_FULLY_BLANKED) {
        return BinaryDataFlagsMod::INTEGRATION_FULLY_BLANKED;
    }
    	
    if (name == CBinaryDataFlags::sWVR_APC) {
        return BinaryDataFlagsMod::WVR_APC;
    }
    	
    if (name == CBinaryDataFlags::sCORRELATOR_MISSING_STATUS) {
        return BinaryDataFlagsMod::CORRELATOR_MISSING_STATUS;
    }
    	
    if (name == CBinaryDataFlags::sMISSING_ANTENNA_EVENT) {
        return BinaryDataFlagsMod::MISSING_ANTENNA_EVENT;
    }
    	
    if (name == CBinaryDataFlags::sDELTA_SIGMA_OVERFLOW) {
        return BinaryDataFlagsMod::DELTA_SIGMA_OVERFLOW;
    }
    	
    if (name == CBinaryDataFlags::sDELAY_CORRECTION_NOT_APPLIED) {
        return BinaryDataFlagsMod::DELAY_CORRECTION_NOT_APPLIED;
    }
    	
    if (name == CBinaryDataFlags::sSYNCRONIZATION_ERROR) {
        return BinaryDataFlagsMod::SYNCRONIZATION_ERROR;
    }
    	
    if (name == CBinaryDataFlags::sFFT_OVERFLOW) {
        return BinaryDataFlagsMod::FFT_OVERFLOW;
    }
    	
    if (name == CBinaryDataFlags::sTFB_SCALING_FACTOR_NOT_RETRIEVED) {
        return BinaryDataFlagsMod::TFB_SCALING_FACTOR_NOT_RETRIEVED;
    }
    	
    if (name == CBinaryDataFlags::sZERO_LAG_NOT_RECEIVED) {
        return BinaryDataFlagsMod::ZERO_LAG_NOT_RECEIVED;
    }
    	
    if (name == CBinaryDataFlags::sSIGMA_OVERFLOW) {
        return BinaryDataFlagsMod::SIGMA_OVERFLOW;
    }
    	
    if (name == CBinaryDataFlags::sUNUSABLE_CAI_OUTPUT) {
        return BinaryDataFlagsMod::UNUSABLE_CAI_OUTPUT;
    }
    	
    if (name == CBinaryDataFlags::sQC_FAILED) {
        return BinaryDataFlagsMod::QC_FAILED;
    }
    	
    if (name == CBinaryDataFlags::sNOISY_TDM_CHANNELS) {
        return BinaryDataFlagsMod::NOISY_TDM_CHANNELS;
    }
    	
    if (name == CBinaryDataFlags::sSPECTRAL_NORMALIZATION_FAILED) {
        return BinaryDataFlagsMod::SPECTRAL_NORMALIZATION_FAILED;
    }
    	
    if (name == CBinaryDataFlags::sDROPPED_PACKETS) {
        return BinaryDataFlagsMod::DROPPED_PACKETS;
    }
    	
    if (name == CBinaryDataFlags::sDETECTOR_SATURATED) {
        return BinaryDataFlagsMod::DETECTOR_SATURATED;
    }
    	
    if (name == CBinaryDataFlags::sNO_DATA_FROM_DIGITAL_POWER_METER) {
        return BinaryDataFlagsMod::NO_DATA_FROM_DIGITAL_POWER_METER;
    }
    	
    if (name == CBinaryDataFlags::sRESERVED_18) {
        return BinaryDataFlagsMod::RESERVED_18;
    }
    	
    if (name == CBinaryDataFlags::sRESERVED_19) {
        return BinaryDataFlagsMod::RESERVED_19;
    }
    	
    if (name == CBinaryDataFlags::sRESERVED_20) {
        return BinaryDataFlagsMod::RESERVED_20;
    }
    	
    if (name == CBinaryDataFlags::sRESERVED_21) {
        return BinaryDataFlagsMod::RESERVED_21;
    }
    	
    if (name == CBinaryDataFlags::sRESERVED_22) {
        return BinaryDataFlagsMod::RESERVED_22;
    }
    	
    if (name == CBinaryDataFlags::sRESERVED_23) {
        return BinaryDataFlagsMod::RESERVED_23;
    }
    	
    if (name == CBinaryDataFlags::sRESERVED_24) {
        return BinaryDataFlagsMod::RESERVED_24;
    }
    	
    if (name == CBinaryDataFlags::sRESERVED_25) {
        return BinaryDataFlagsMod::RESERVED_25;
    }
    	
    if (name == CBinaryDataFlags::sRESERVED_26) {
        return BinaryDataFlagsMod::RESERVED_26;
    }
    	
    if (name == CBinaryDataFlags::sRESERVED_27) {
        return BinaryDataFlagsMod::RESERVED_27;
    }
    	
    if (name == CBinaryDataFlags::sRESERVED_28) {
        return BinaryDataFlagsMod::RESERVED_28;
    }
    	
    if (name == CBinaryDataFlags::sRESERVED_29) {
        return BinaryDataFlagsMod::RESERVED_29;
    }
    	
    if (name == CBinaryDataFlags::sRESERVED_30) {
        return BinaryDataFlagsMod::RESERVED_30;
    }
    	
    if (name == CBinaryDataFlags::sALL_PURPOSE_ERROR) {
        return BinaryDataFlagsMod::ALL_PURPOSE_ERROR;
    }
    
    throw badString(name);
}

BinaryDataFlagsMod::BinaryDataFlags CBinaryDataFlags::literal(const std::string& name) {
		
    if (name == CBinaryDataFlags::sINTEGRATION_FULLY_BLANKED) {
        return BinaryDataFlagsMod::INTEGRATION_FULLY_BLANKED;
    }
    	
    if (name == CBinaryDataFlags::sWVR_APC) {
        return BinaryDataFlagsMod::WVR_APC;
    }
    	
    if (name == CBinaryDataFlags::sCORRELATOR_MISSING_STATUS) {
        return BinaryDataFlagsMod::CORRELATOR_MISSING_STATUS;
    }
    	
    if (name == CBinaryDataFlags::sMISSING_ANTENNA_EVENT) {
        return BinaryDataFlagsMod::MISSING_ANTENNA_EVENT;
    }
    	
    if (name == CBinaryDataFlags::sDELTA_SIGMA_OVERFLOW) {
        return BinaryDataFlagsMod::DELTA_SIGMA_OVERFLOW;
    }
    	
    if (name == CBinaryDataFlags::sDELAY_CORRECTION_NOT_APPLIED) {
        return BinaryDataFlagsMod::DELAY_CORRECTION_NOT_APPLIED;
    }
    	
    if (name == CBinaryDataFlags::sSYNCRONIZATION_ERROR) {
        return BinaryDataFlagsMod::SYNCRONIZATION_ERROR;
    }
    	
    if (name == CBinaryDataFlags::sFFT_OVERFLOW) {
        return BinaryDataFlagsMod::FFT_OVERFLOW;
    }
    	
    if (name == CBinaryDataFlags::sTFB_SCALING_FACTOR_NOT_RETRIEVED) {
        return BinaryDataFlagsMod::TFB_SCALING_FACTOR_NOT_RETRIEVED;
    }
    	
    if (name == CBinaryDataFlags::sZERO_LAG_NOT_RECEIVED) {
        return BinaryDataFlagsMod::ZERO_LAG_NOT_RECEIVED;
    }
    	
    if (name == CBinaryDataFlags::sSIGMA_OVERFLOW) {
        return BinaryDataFlagsMod::SIGMA_OVERFLOW;
    }
    	
    if (name == CBinaryDataFlags::sUNUSABLE_CAI_OUTPUT) {
        return BinaryDataFlagsMod::UNUSABLE_CAI_OUTPUT;
    }
    	
    if (name == CBinaryDataFlags::sQC_FAILED) {
        return BinaryDataFlagsMod::QC_FAILED;
    }
    	
    if (name == CBinaryDataFlags::sNOISY_TDM_CHANNELS) {
        return BinaryDataFlagsMod::NOISY_TDM_CHANNELS;
    }
    	
    if (name == CBinaryDataFlags::sSPECTRAL_NORMALIZATION_FAILED) {
        return BinaryDataFlagsMod::SPECTRAL_NORMALIZATION_FAILED;
    }
    	
    if (name == CBinaryDataFlags::sDROPPED_PACKETS) {
        return BinaryDataFlagsMod::DROPPED_PACKETS;
    }
    	
    if (name == CBinaryDataFlags::sDETECTOR_SATURATED) {
        return BinaryDataFlagsMod::DETECTOR_SATURATED;
    }
    	
    if (name == CBinaryDataFlags::sNO_DATA_FROM_DIGITAL_POWER_METER) {
        return BinaryDataFlagsMod::NO_DATA_FROM_DIGITAL_POWER_METER;
    }
    	
    if (name == CBinaryDataFlags::sRESERVED_18) {
        return BinaryDataFlagsMod::RESERVED_18;
    }
    	
    if (name == CBinaryDataFlags::sRESERVED_19) {
        return BinaryDataFlagsMod::RESERVED_19;
    }
    	
    if (name == CBinaryDataFlags::sRESERVED_20) {
        return BinaryDataFlagsMod::RESERVED_20;
    }
    	
    if (name == CBinaryDataFlags::sRESERVED_21) {
        return BinaryDataFlagsMod::RESERVED_21;
    }
    	
    if (name == CBinaryDataFlags::sRESERVED_22) {
        return BinaryDataFlagsMod::RESERVED_22;
    }
    	
    if (name == CBinaryDataFlags::sRESERVED_23) {
        return BinaryDataFlagsMod::RESERVED_23;
    }
    	
    if (name == CBinaryDataFlags::sRESERVED_24) {
        return BinaryDataFlagsMod::RESERVED_24;
    }
    	
    if (name == CBinaryDataFlags::sRESERVED_25) {
        return BinaryDataFlagsMod::RESERVED_25;
    }
    	
    if (name == CBinaryDataFlags::sRESERVED_26) {
        return BinaryDataFlagsMod::RESERVED_26;
    }
    	
    if (name == CBinaryDataFlags::sRESERVED_27) {
        return BinaryDataFlagsMod::RESERVED_27;
    }
    	
    if (name == CBinaryDataFlags::sRESERVED_28) {
        return BinaryDataFlagsMod::RESERVED_28;
    }
    	
    if (name == CBinaryDataFlags::sRESERVED_29) {
        return BinaryDataFlagsMod::RESERVED_29;
    }
    	
    if (name == CBinaryDataFlags::sRESERVED_30) {
        return BinaryDataFlagsMod::RESERVED_30;
    }
    	
    if (name == CBinaryDataFlags::sALL_PURPOSE_ERROR) {
        return BinaryDataFlagsMod::ALL_PURPOSE_ERROR;
    }
    
    throw badString(name);
}

BinaryDataFlagsMod::BinaryDataFlags CBinaryDataFlags::from_int(unsigned int i) {
	vector<string> names_ = names();
	if (i >= names_.size()) throw badInt(i);
	return newBinaryDataFlags(names_.at(i));
}

string CBinaryDataFlags::badString(const string& name) {
	return "'"+name+"' does not correspond to any literal in the enumeration 'BinaryDataFlags'.";
}

string CBinaryDataFlags::badInt(unsigned int i) {
	ostringstream oss ;
	oss << "'" << i << "' is out of range for the enumeration 'BinaryDataFlags'.";
	return oss.str();
}

namespace BinaryDataFlagsMod {
	std::ostream & operator << ( std::ostream & out, const BinaryDataFlags& value) {
		out << CBinaryDataFlags::name(value);
		return out;
	}

	std::istream & operator >> ( std::istream & in , BinaryDataFlags& value ) {
		in.clear();
		string s ; 
  		in >> s;
  		try {
    		value = CBinaryDataFlags::literal(s);
  		}
  		catch (string & m) {
    		in.setstate(ios::failbit);
  		}
  		return in;
  	}
}

