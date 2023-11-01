
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
 * File CScanIntent.cpp
 */
#include <sstream>
#include <alma/Enumerations/CScanIntent.h>
#include <string>
using namespace std;

int CScanIntent::version() {
	return ScanIntentMod::version;
	}
	
string CScanIntent::revision () {
	return ScanIntentMod::revision;
}

unsigned int CScanIntent::size() {
	return 30;
	}
	
	
const std::string& CScanIntent::sCALIBRATE_AMPLI = "CALIBRATE_AMPLI";
	
const std::string& CScanIntent::sCALIBRATE_ATMOSPHERE = "CALIBRATE_ATMOSPHERE";
	
const std::string& CScanIntent::sCALIBRATE_BANDPASS = "CALIBRATE_BANDPASS";
	
const std::string& CScanIntent::sCALIBRATE_DELAY = "CALIBRATE_DELAY";
	
const std::string& CScanIntent::sCALIBRATE_FLUX = "CALIBRATE_FLUX";
	
const std::string& CScanIntent::sCALIBRATE_FOCUS = "CALIBRATE_FOCUS";
	
const std::string& CScanIntent::sCALIBRATE_FOCUS_X = "CALIBRATE_FOCUS_X";
	
const std::string& CScanIntent::sCALIBRATE_FOCUS_Y = "CALIBRATE_FOCUS_Y";
	
const std::string& CScanIntent::sCALIBRATE_PHASE = "CALIBRATE_PHASE";
	
const std::string& CScanIntent::sCALIBRATE_POINTING = "CALIBRATE_POINTING";
	
const std::string& CScanIntent::sCALIBRATE_POLARIZATION = "CALIBRATE_POLARIZATION";
	
const std::string& CScanIntent::sCALIBRATE_SIDEBAND_RATIO = "CALIBRATE_SIDEBAND_RATIO";
	
const std::string& CScanIntent::sCALIBRATE_WVR = "CALIBRATE_WVR";
	
const std::string& CScanIntent::sDO_SKYDIP = "DO_SKYDIP";
	
const std::string& CScanIntent::sMAP_ANTENNA_SURFACE = "MAP_ANTENNA_SURFACE";
	
const std::string& CScanIntent::sMAP_PRIMARY_BEAM = "MAP_PRIMARY_BEAM";
	
const std::string& CScanIntent::sOBSERVE_TARGET = "OBSERVE_TARGET";
	
const std::string& CScanIntent::sCALIBRATE_POL_LEAKAGE = "CALIBRATE_POL_LEAKAGE";
	
const std::string& CScanIntent::sCALIBRATE_POL_ANGLE = "CALIBRATE_POL_ANGLE";
	
const std::string& CScanIntent::sTEST = "TEST";
	
const std::string& CScanIntent::sUNSPECIFIED = "UNSPECIFIED";
	
const std::string& CScanIntent::sCALIBRATE_ANTENNA_POSITION = "CALIBRATE_ANTENNA_POSITION";
	
const std::string& CScanIntent::sCALIBRATE_ANTENNA_PHASE = "CALIBRATE_ANTENNA_PHASE";
	
const std::string& CScanIntent::sMEASURE_RFI = "MEASURE_RFI";
	
const std::string& CScanIntent::sCALIBRATE_ANTENNA_POINTING_MODEL = "CALIBRATE_ANTENNA_POINTING_MODEL";
	
const std::string& CScanIntent::sSYSTEM_CONFIGURATION = "SYSTEM_CONFIGURATION";
	
const std::string& CScanIntent::sCALIBRATE_APPPHASE_ACTIVE = "CALIBRATE_APPPHASE_ACTIVE";
	
const std::string& CScanIntent::sCALIBRATE_APPPHASE_PASSIVE = "CALIBRATE_APPPHASE_PASSIVE";
	
const std::string& CScanIntent::sOBSERVE_CHECK_SOURCE = "OBSERVE_CHECK_SOURCE";
	
const std::string& CScanIntent::sCALIBRATE_DIFFGAIN = "CALIBRATE_DIFFGAIN";
	
const std::vector<std::string> CScanIntent::names() {
    std::vector<std::string> enumSet;
    
    enumSet.insert(enumSet.end(), CScanIntent::sCALIBRATE_AMPLI);
    
    enumSet.insert(enumSet.end(), CScanIntent::sCALIBRATE_ATMOSPHERE);
    
    enumSet.insert(enumSet.end(), CScanIntent::sCALIBRATE_BANDPASS);
    
    enumSet.insert(enumSet.end(), CScanIntent::sCALIBRATE_DELAY);
    
    enumSet.insert(enumSet.end(), CScanIntent::sCALIBRATE_FLUX);
    
    enumSet.insert(enumSet.end(), CScanIntent::sCALIBRATE_FOCUS);
    
    enumSet.insert(enumSet.end(), CScanIntent::sCALIBRATE_FOCUS_X);
    
    enumSet.insert(enumSet.end(), CScanIntent::sCALIBRATE_FOCUS_Y);
    
    enumSet.insert(enumSet.end(), CScanIntent::sCALIBRATE_PHASE);
    
    enumSet.insert(enumSet.end(), CScanIntent::sCALIBRATE_POINTING);
    
    enumSet.insert(enumSet.end(), CScanIntent::sCALIBRATE_POLARIZATION);
    
    enumSet.insert(enumSet.end(), CScanIntent::sCALIBRATE_SIDEBAND_RATIO);
    
    enumSet.insert(enumSet.end(), CScanIntent::sCALIBRATE_WVR);
    
    enumSet.insert(enumSet.end(), CScanIntent::sDO_SKYDIP);
    
    enumSet.insert(enumSet.end(), CScanIntent::sMAP_ANTENNA_SURFACE);
    
    enumSet.insert(enumSet.end(), CScanIntent::sMAP_PRIMARY_BEAM);
    
    enumSet.insert(enumSet.end(), CScanIntent::sOBSERVE_TARGET);
    
    enumSet.insert(enumSet.end(), CScanIntent::sCALIBRATE_POL_LEAKAGE);
    
    enumSet.insert(enumSet.end(), CScanIntent::sCALIBRATE_POL_ANGLE);
    
    enumSet.insert(enumSet.end(), CScanIntent::sTEST);
    
    enumSet.insert(enumSet.end(), CScanIntent::sUNSPECIFIED);
    
    enumSet.insert(enumSet.end(), CScanIntent::sCALIBRATE_ANTENNA_POSITION);
    
    enumSet.insert(enumSet.end(), CScanIntent::sCALIBRATE_ANTENNA_PHASE);
    
    enumSet.insert(enumSet.end(), CScanIntent::sMEASURE_RFI);
    
    enumSet.insert(enumSet.end(), CScanIntent::sCALIBRATE_ANTENNA_POINTING_MODEL);
    
    enumSet.insert(enumSet.end(), CScanIntent::sSYSTEM_CONFIGURATION);
    
    enumSet.insert(enumSet.end(), CScanIntent::sCALIBRATE_APPPHASE_ACTIVE);
    
    enumSet.insert(enumSet.end(), CScanIntent::sCALIBRATE_APPPHASE_PASSIVE);
    
    enumSet.insert(enumSet.end(), CScanIntent::sOBSERVE_CHECK_SOURCE);
    
    enumSet.insert(enumSet.end(), CScanIntent::sCALIBRATE_DIFFGAIN);
        
    return enumSet;
}

std::string CScanIntent::name(const ScanIntentMod::ScanIntent& f) {
    switch (f) {
    
    case ScanIntentMod::CALIBRATE_AMPLI:
      return CScanIntent::sCALIBRATE_AMPLI;
    
    case ScanIntentMod::CALIBRATE_ATMOSPHERE:
      return CScanIntent::sCALIBRATE_ATMOSPHERE;
    
    case ScanIntentMod::CALIBRATE_BANDPASS:
      return CScanIntent::sCALIBRATE_BANDPASS;
    
    case ScanIntentMod::CALIBRATE_DELAY:
      return CScanIntent::sCALIBRATE_DELAY;
    
    case ScanIntentMod::CALIBRATE_FLUX:
      return CScanIntent::sCALIBRATE_FLUX;
    
    case ScanIntentMod::CALIBRATE_FOCUS:
      return CScanIntent::sCALIBRATE_FOCUS;
    
    case ScanIntentMod::CALIBRATE_FOCUS_X:
      return CScanIntent::sCALIBRATE_FOCUS_X;
    
    case ScanIntentMod::CALIBRATE_FOCUS_Y:
      return CScanIntent::sCALIBRATE_FOCUS_Y;
    
    case ScanIntentMod::CALIBRATE_PHASE:
      return CScanIntent::sCALIBRATE_PHASE;
    
    case ScanIntentMod::CALIBRATE_POINTING:
      return CScanIntent::sCALIBRATE_POINTING;
    
    case ScanIntentMod::CALIBRATE_POLARIZATION:
      return CScanIntent::sCALIBRATE_POLARIZATION;
    
    case ScanIntentMod::CALIBRATE_SIDEBAND_RATIO:
      return CScanIntent::sCALIBRATE_SIDEBAND_RATIO;
    
    case ScanIntentMod::CALIBRATE_WVR:
      return CScanIntent::sCALIBRATE_WVR;
    
    case ScanIntentMod::DO_SKYDIP:
      return CScanIntent::sDO_SKYDIP;
    
    case ScanIntentMod::MAP_ANTENNA_SURFACE:
      return CScanIntent::sMAP_ANTENNA_SURFACE;
    
    case ScanIntentMod::MAP_PRIMARY_BEAM:
      return CScanIntent::sMAP_PRIMARY_BEAM;
    
    case ScanIntentMod::OBSERVE_TARGET:
      return CScanIntent::sOBSERVE_TARGET;
    
    case ScanIntentMod::CALIBRATE_POL_LEAKAGE:
      return CScanIntent::sCALIBRATE_POL_LEAKAGE;
    
    case ScanIntentMod::CALIBRATE_POL_ANGLE:
      return CScanIntent::sCALIBRATE_POL_ANGLE;
    
    case ScanIntentMod::TEST:
      return CScanIntent::sTEST;
    
    case ScanIntentMod::UNSPECIFIED:
      return CScanIntent::sUNSPECIFIED;
    
    case ScanIntentMod::CALIBRATE_ANTENNA_POSITION:
      return CScanIntent::sCALIBRATE_ANTENNA_POSITION;
    
    case ScanIntentMod::CALIBRATE_ANTENNA_PHASE:
      return CScanIntent::sCALIBRATE_ANTENNA_PHASE;
    
    case ScanIntentMod::MEASURE_RFI:
      return CScanIntent::sMEASURE_RFI;
    
    case ScanIntentMod::CALIBRATE_ANTENNA_POINTING_MODEL:
      return CScanIntent::sCALIBRATE_ANTENNA_POINTING_MODEL;
    
    case ScanIntentMod::SYSTEM_CONFIGURATION:
      return CScanIntent::sSYSTEM_CONFIGURATION;
    
    case ScanIntentMod::CALIBRATE_APPPHASE_ACTIVE:
      return CScanIntent::sCALIBRATE_APPPHASE_ACTIVE;
    
    case ScanIntentMod::CALIBRATE_APPPHASE_PASSIVE:
      return CScanIntent::sCALIBRATE_APPPHASE_PASSIVE;
    
    case ScanIntentMod::OBSERVE_CHECK_SOURCE:
      return CScanIntent::sOBSERVE_CHECK_SOURCE;
    
    case ScanIntentMod::CALIBRATE_DIFFGAIN:
      return CScanIntent::sCALIBRATE_DIFFGAIN;
    	
    }
    // Impossible siutation but....who knows with C++ enums
    throw badInt((int) f);
}

ScanIntentMod::ScanIntent CScanIntent::newScanIntent(const std::string& name) {
		
    if (name == CScanIntent::sCALIBRATE_AMPLI) {
        return ScanIntentMod::CALIBRATE_AMPLI;
    }
    	
    if (name == CScanIntent::sCALIBRATE_ATMOSPHERE) {
        return ScanIntentMod::CALIBRATE_ATMOSPHERE;
    }
    	
    if (name == CScanIntent::sCALIBRATE_BANDPASS) {
        return ScanIntentMod::CALIBRATE_BANDPASS;
    }
    	
    if (name == CScanIntent::sCALIBRATE_DELAY) {
        return ScanIntentMod::CALIBRATE_DELAY;
    }
    	
    if (name == CScanIntent::sCALIBRATE_FLUX) {
        return ScanIntentMod::CALIBRATE_FLUX;
    }
    	
    if (name == CScanIntent::sCALIBRATE_FOCUS) {
        return ScanIntentMod::CALIBRATE_FOCUS;
    }
    	
    if (name == CScanIntent::sCALIBRATE_FOCUS_X) {
        return ScanIntentMod::CALIBRATE_FOCUS_X;
    }
    	
    if (name == CScanIntent::sCALIBRATE_FOCUS_Y) {
        return ScanIntentMod::CALIBRATE_FOCUS_Y;
    }
    	
    if (name == CScanIntent::sCALIBRATE_PHASE) {
        return ScanIntentMod::CALIBRATE_PHASE;
    }
    	
    if (name == CScanIntent::sCALIBRATE_POINTING) {
        return ScanIntentMod::CALIBRATE_POINTING;
    }
    	
    if (name == CScanIntent::sCALIBRATE_POLARIZATION) {
        return ScanIntentMod::CALIBRATE_POLARIZATION;
    }
    	
    if (name == CScanIntent::sCALIBRATE_SIDEBAND_RATIO) {
        return ScanIntentMod::CALIBRATE_SIDEBAND_RATIO;
    }
    	
    if (name == CScanIntent::sCALIBRATE_WVR) {
        return ScanIntentMod::CALIBRATE_WVR;
    }
    	
    if (name == CScanIntent::sDO_SKYDIP) {
        return ScanIntentMod::DO_SKYDIP;
    }
    	
    if (name == CScanIntent::sMAP_ANTENNA_SURFACE) {
        return ScanIntentMod::MAP_ANTENNA_SURFACE;
    }
    	
    if (name == CScanIntent::sMAP_PRIMARY_BEAM) {
        return ScanIntentMod::MAP_PRIMARY_BEAM;
    }
    	
    if (name == CScanIntent::sOBSERVE_TARGET) {
        return ScanIntentMod::OBSERVE_TARGET;
    }
    	
    if (name == CScanIntent::sCALIBRATE_POL_LEAKAGE) {
        return ScanIntentMod::CALIBRATE_POL_LEAKAGE;
    }
    	
    if (name == CScanIntent::sCALIBRATE_POL_ANGLE) {
        return ScanIntentMod::CALIBRATE_POL_ANGLE;
    }
    	
    if (name == CScanIntent::sTEST) {
        return ScanIntentMod::TEST;
    }
    	
    if (name == CScanIntent::sUNSPECIFIED) {
        return ScanIntentMod::UNSPECIFIED;
    }
    	
    if (name == CScanIntent::sCALIBRATE_ANTENNA_POSITION) {
        return ScanIntentMod::CALIBRATE_ANTENNA_POSITION;
    }
    	
    if (name == CScanIntent::sCALIBRATE_ANTENNA_PHASE) {
        return ScanIntentMod::CALIBRATE_ANTENNA_PHASE;
    }
    	
    if (name == CScanIntent::sMEASURE_RFI) {
        return ScanIntentMod::MEASURE_RFI;
    }
    	
    if (name == CScanIntent::sCALIBRATE_ANTENNA_POINTING_MODEL) {
        return ScanIntentMod::CALIBRATE_ANTENNA_POINTING_MODEL;
    }
    	
    if (name == CScanIntent::sSYSTEM_CONFIGURATION) {
        return ScanIntentMod::SYSTEM_CONFIGURATION;
    }
    	
    if (name == CScanIntent::sCALIBRATE_APPPHASE_ACTIVE) {
        return ScanIntentMod::CALIBRATE_APPPHASE_ACTIVE;
    }
    	
    if (name == CScanIntent::sCALIBRATE_APPPHASE_PASSIVE) {
        return ScanIntentMod::CALIBRATE_APPPHASE_PASSIVE;
    }
    	
    if (name == CScanIntent::sOBSERVE_CHECK_SOURCE) {
        return ScanIntentMod::OBSERVE_CHECK_SOURCE;
    }
    	
    if (name == CScanIntent::sCALIBRATE_DIFFGAIN) {
        return ScanIntentMod::CALIBRATE_DIFFGAIN;
    }
    
    throw badString(name);
}

ScanIntentMod::ScanIntent CScanIntent::literal(const std::string& name) {
		
    if (name == CScanIntent::sCALIBRATE_AMPLI) {
        return ScanIntentMod::CALIBRATE_AMPLI;
    }
    	
    if (name == CScanIntent::sCALIBRATE_ATMOSPHERE) {
        return ScanIntentMod::CALIBRATE_ATMOSPHERE;
    }
    	
    if (name == CScanIntent::sCALIBRATE_BANDPASS) {
        return ScanIntentMod::CALIBRATE_BANDPASS;
    }
    	
    if (name == CScanIntent::sCALIBRATE_DELAY) {
        return ScanIntentMod::CALIBRATE_DELAY;
    }
    	
    if (name == CScanIntent::sCALIBRATE_FLUX) {
        return ScanIntentMod::CALIBRATE_FLUX;
    }
    	
    if (name == CScanIntent::sCALIBRATE_FOCUS) {
        return ScanIntentMod::CALIBRATE_FOCUS;
    }
    	
    if (name == CScanIntent::sCALIBRATE_FOCUS_X) {
        return ScanIntentMod::CALIBRATE_FOCUS_X;
    }
    	
    if (name == CScanIntent::sCALIBRATE_FOCUS_Y) {
        return ScanIntentMod::CALIBRATE_FOCUS_Y;
    }
    	
    if (name == CScanIntent::sCALIBRATE_PHASE) {
        return ScanIntentMod::CALIBRATE_PHASE;
    }
    	
    if (name == CScanIntent::sCALIBRATE_POINTING) {
        return ScanIntentMod::CALIBRATE_POINTING;
    }
    	
    if (name == CScanIntent::sCALIBRATE_POLARIZATION) {
        return ScanIntentMod::CALIBRATE_POLARIZATION;
    }
    	
    if (name == CScanIntent::sCALIBRATE_SIDEBAND_RATIO) {
        return ScanIntentMod::CALIBRATE_SIDEBAND_RATIO;
    }
    	
    if (name == CScanIntent::sCALIBRATE_WVR) {
        return ScanIntentMod::CALIBRATE_WVR;
    }
    	
    if (name == CScanIntent::sDO_SKYDIP) {
        return ScanIntentMod::DO_SKYDIP;
    }
    	
    if (name == CScanIntent::sMAP_ANTENNA_SURFACE) {
        return ScanIntentMod::MAP_ANTENNA_SURFACE;
    }
    	
    if (name == CScanIntent::sMAP_PRIMARY_BEAM) {
        return ScanIntentMod::MAP_PRIMARY_BEAM;
    }
    	
    if (name == CScanIntent::sOBSERVE_TARGET) {
        return ScanIntentMod::OBSERVE_TARGET;
    }
    	
    if (name == CScanIntent::sCALIBRATE_POL_LEAKAGE) {
        return ScanIntentMod::CALIBRATE_POL_LEAKAGE;
    }
    	
    if (name == CScanIntent::sCALIBRATE_POL_ANGLE) {
        return ScanIntentMod::CALIBRATE_POL_ANGLE;
    }
    	
    if (name == CScanIntent::sTEST) {
        return ScanIntentMod::TEST;
    }
    	
    if (name == CScanIntent::sUNSPECIFIED) {
        return ScanIntentMod::UNSPECIFIED;
    }
    	
    if (name == CScanIntent::sCALIBRATE_ANTENNA_POSITION) {
        return ScanIntentMod::CALIBRATE_ANTENNA_POSITION;
    }
    	
    if (name == CScanIntent::sCALIBRATE_ANTENNA_PHASE) {
        return ScanIntentMod::CALIBRATE_ANTENNA_PHASE;
    }
    	
    if (name == CScanIntent::sMEASURE_RFI) {
        return ScanIntentMod::MEASURE_RFI;
    }
    	
    if (name == CScanIntent::sCALIBRATE_ANTENNA_POINTING_MODEL) {
        return ScanIntentMod::CALIBRATE_ANTENNA_POINTING_MODEL;
    }
    	
    if (name == CScanIntent::sSYSTEM_CONFIGURATION) {
        return ScanIntentMod::SYSTEM_CONFIGURATION;
    }
    	
    if (name == CScanIntent::sCALIBRATE_APPPHASE_ACTIVE) {
        return ScanIntentMod::CALIBRATE_APPPHASE_ACTIVE;
    }
    	
    if (name == CScanIntent::sCALIBRATE_APPPHASE_PASSIVE) {
        return ScanIntentMod::CALIBRATE_APPPHASE_PASSIVE;
    }
    	
    if (name == CScanIntent::sOBSERVE_CHECK_SOURCE) {
        return ScanIntentMod::OBSERVE_CHECK_SOURCE;
    }
    	
    if (name == CScanIntent::sCALIBRATE_DIFFGAIN) {
        return ScanIntentMod::CALIBRATE_DIFFGAIN;
    }
    
    throw badString(name);
}

ScanIntentMod::ScanIntent CScanIntent::from_int(unsigned int i) {
	vector<string> names_ = names();
	if (i >= names_.size()) throw badInt(i);
	return newScanIntent(names_.at(i));
}

string CScanIntent::badString(const string& name) {
	return "'"+name+"' does not correspond to any literal in the enumeration 'ScanIntent'.";
}

string CScanIntent::badInt(unsigned int i) {
	ostringstream oss ;
	oss << "'" << i << "' is out of range for the enumeration 'ScanIntent'.";
	return oss.str();
}

namespace ScanIntentMod {
	std::ostream & operator << ( std::ostream & out, const ScanIntent& value) {
		out << CScanIntent::name(value);
		return out;
	}

	std::istream & operator >> ( std::istream & in , ScanIntent& value ) {
		in.clear();
		string s ; 
  		in >> s;
  		try {
    		value = CScanIntent::literal(s);
  		}
  		catch (string & m) {
    		in.setstate(ios::failbit);
  		}
  		return in;
  	}
}

