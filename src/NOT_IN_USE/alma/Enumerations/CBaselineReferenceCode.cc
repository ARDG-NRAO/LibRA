
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
 * File CBaselineReferenceCode.cpp
 */
#include <sstream>
#include <alma/Enumerations/CBaselineReferenceCode.h>
#include <string>
using namespace std;

int CBaselineReferenceCode::version() {
	return BaselineReferenceCodeMod::version;
	}
	
string CBaselineReferenceCode::revision () {
	return BaselineReferenceCodeMod::revision;
}

unsigned int CBaselineReferenceCode::size() {
	return 28;
	}
	
	
const std::string& CBaselineReferenceCode::sJ2000 = "J2000";
	
const std::string& CBaselineReferenceCode::sB1950 = "B1950";
	
const std::string& CBaselineReferenceCode::sGALACTIC = "GALACTIC";
	
const std::string& CBaselineReferenceCode::sSUPERGAL = "SUPERGAL";
	
const std::string& CBaselineReferenceCode::sECLIPTIC = "ECLIPTIC";
	
const std::string& CBaselineReferenceCode::sJMEAN = "JMEAN";
	
const std::string& CBaselineReferenceCode::sJTRUE = "JTRUE";
	
const std::string& CBaselineReferenceCode::sAPP = "APP";
	
const std::string& CBaselineReferenceCode::sBMEAN = "BMEAN";
	
const std::string& CBaselineReferenceCode::sBTRUE = "BTRUE";
	
const std::string& CBaselineReferenceCode::sJNAT = "JNAT";
	
const std::string& CBaselineReferenceCode::sMECLIPTIC = "MECLIPTIC";
	
const std::string& CBaselineReferenceCode::sTECLIPTIC = "TECLIPTIC";
	
const std::string& CBaselineReferenceCode::sTOPO = "TOPO";
	
const std::string& CBaselineReferenceCode::sMERCURY = "MERCURY";
	
const std::string& CBaselineReferenceCode::sVENUS = "VENUS";
	
const std::string& CBaselineReferenceCode::sMARS = "MARS";
	
const std::string& CBaselineReferenceCode::sJUPITER = "JUPITER";
	
const std::string& CBaselineReferenceCode::sSATURN = "SATURN";
	
const std::string& CBaselineReferenceCode::sNEPTUN = "NEPTUN";
	
const std::string& CBaselineReferenceCode::sSUN = "SUN";
	
const std::string& CBaselineReferenceCode::sMOON = "MOON";
	
const std::string& CBaselineReferenceCode::sHADEC = "HADEC";
	
const std::string& CBaselineReferenceCode::sAZEL = "AZEL";
	
const std::string& CBaselineReferenceCode::sAZELGEO = "AZELGEO";
	
const std::string& CBaselineReferenceCode::sAZELSW = "AZELSW";
	
const std::string& CBaselineReferenceCode::sAZELNE = "AZELNE";
	
const std::string& CBaselineReferenceCode::sITRF = "ITRF";
	
const std::vector<std::string> CBaselineReferenceCode::names() {
    std::vector<std::string> enumSet;
    
    enumSet.insert(enumSet.end(), CBaselineReferenceCode::sJ2000);
    
    enumSet.insert(enumSet.end(), CBaselineReferenceCode::sB1950);
    
    enumSet.insert(enumSet.end(), CBaselineReferenceCode::sGALACTIC);
    
    enumSet.insert(enumSet.end(), CBaselineReferenceCode::sSUPERGAL);
    
    enumSet.insert(enumSet.end(), CBaselineReferenceCode::sECLIPTIC);
    
    enumSet.insert(enumSet.end(), CBaselineReferenceCode::sJMEAN);
    
    enumSet.insert(enumSet.end(), CBaselineReferenceCode::sJTRUE);
    
    enumSet.insert(enumSet.end(), CBaselineReferenceCode::sAPP);
    
    enumSet.insert(enumSet.end(), CBaselineReferenceCode::sBMEAN);
    
    enumSet.insert(enumSet.end(), CBaselineReferenceCode::sBTRUE);
    
    enumSet.insert(enumSet.end(), CBaselineReferenceCode::sJNAT);
    
    enumSet.insert(enumSet.end(), CBaselineReferenceCode::sMECLIPTIC);
    
    enumSet.insert(enumSet.end(), CBaselineReferenceCode::sTECLIPTIC);
    
    enumSet.insert(enumSet.end(), CBaselineReferenceCode::sTOPO);
    
    enumSet.insert(enumSet.end(), CBaselineReferenceCode::sMERCURY);
    
    enumSet.insert(enumSet.end(), CBaselineReferenceCode::sVENUS);
    
    enumSet.insert(enumSet.end(), CBaselineReferenceCode::sMARS);
    
    enumSet.insert(enumSet.end(), CBaselineReferenceCode::sJUPITER);
    
    enumSet.insert(enumSet.end(), CBaselineReferenceCode::sSATURN);
    
    enumSet.insert(enumSet.end(), CBaselineReferenceCode::sNEPTUN);
    
    enumSet.insert(enumSet.end(), CBaselineReferenceCode::sSUN);
    
    enumSet.insert(enumSet.end(), CBaselineReferenceCode::sMOON);
    
    enumSet.insert(enumSet.end(), CBaselineReferenceCode::sHADEC);
    
    enumSet.insert(enumSet.end(), CBaselineReferenceCode::sAZEL);
    
    enumSet.insert(enumSet.end(), CBaselineReferenceCode::sAZELGEO);
    
    enumSet.insert(enumSet.end(), CBaselineReferenceCode::sAZELSW);
    
    enumSet.insert(enumSet.end(), CBaselineReferenceCode::sAZELNE);
    
    enumSet.insert(enumSet.end(), CBaselineReferenceCode::sITRF);
        
    return enumSet;
}

std::string CBaselineReferenceCode::name(const BaselineReferenceCodeMod::BaselineReferenceCode& f) {
    switch (f) {
    
    case BaselineReferenceCodeMod::J2000:
      return CBaselineReferenceCode::sJ2000;
    
    case BaselineReferenceCodeMod::B1950:
      return CBaselineReferenceCode::sB1950;
    
    case BaselineReferenceCodeMod::GALACTIC:
      return CBaselineReferenceCode::sGALACTIC;
    
    case BaselineReferenceCodeMod::SUPERGAL:
      return CBaselineReferenceCode::sSUPERGAL;
    
    case BaselineReferenceCodeMod::ECLIPTIC:
      return CBaselineReferenceCode::sECLIPTIC;
    
    case BaselineReferenceCodeMod::JMEAN:
      return CBaselineReferenceCode::sJMEAN;
    
    case BaselineReferenceCodeMod::JTRUE:
      return CBaselineReferenceCode::sJTRUE;
    
    case BaselineReferenceCodeMod::APP:
      return CBaselineReferenceCode::sAPP;
    
    case BaselineReferenceCodeMod::BMEAN:
      return CBaselineReferenceCode::sBMEAN;
    
    case BaselineReferenceCodeMod::BTRUE:
      return CBaselineReferenceCode::sBTRUE;
    
    case BaselineReferenceCodeMod::JNAT:
      return CBaselineReferenceCode::sJNAT;
    
    case BaselineReferenceCodeMod::MECLIPTIC:
      return CBaselineReferenceCode::sMECLIPTIC;
    
    case BaselineReferenceCodeMod::TECLIPTIC:
      return CBaselineReferenceCode::sTECLIPTIC;
    
    case BaselineReferenceCodeMod::TOPO:
      return CBaselineReferenceCode::sTOPO;
    
    case BaselineReferenceCodeMod::MERCURY:
      return CBaselineReferenceCode::sMERCURY;
    
    case BaselineReferenceCodeMod::VENUS:
      return CBaselineReferenceCode::sVENUS;
    
    case BaselineReferenceCodeMod::MARS:
      return CBaselineReferenceCode::sMARS;
    
    case BaselineReferenceCodeMod::JUPITER:
      return CBaselineReferenceCode::sJUPITER;
    
    case BaselineReferenceCodeMod::SATURN:
      return CBaselineReferenceCode::sSATURN;
    
    case BaselineReferenceCodeMod::NEPTUN:
      return CBaselineReferenceCode::sNEPTUN;
    
    case BaselineReferenceCodeMod::SUN:
      return CBaselineReferenceCode::sSUN;
    
    case BaselineReferenceCodeMod::MOON:
      return CBaselineReferenceCode::sMOON;
    
    case BaselineReferenceCodeMod::HADEC:
      return CBaselineReferenceCode::sHADEC;
    
    case BaselineReferenceCodeMod::AZEL:
      return CBaselineReferenceCode::sAZEL;
    
    case BaselineReferenceCodeMod::AZELGEO:
      return CBaselineReferenceCode::sAZELGEO;
    
    case BaselineReferenceCodeMod::AZELSW:
      return CBaselineReferenceCode::sAZELSW;
    
    case BaselineReferenceCodeMod::AZELNE:
      return CBaselineReferenceCode::sAZELNE;
    
    case BaselineReferenceCodeMod::ITRF:
      return CBaselineReferenceCode::sITRF;
    	
    }
    // Impossible siutation but....who knows with C++ enums
    throw badInt((int) f);
}

BaselineReferenceCodeMod::BaselineReferenceCode CBaselineReferenceCode::newBaselineReferenceCode(const std::string& name) {
		
    if (name == CBaselineReferenceCode::sJ2000) {
        return BaselineReferenceCodeMod::J2000;
    }
    	
    if (name == CBaselineReferenceCode::sB1950) {
        return BaselineReferenceCodeMod::B1950;
    }
    	
    if (name == CBaselineReferenceCode::sGALACTIC) {
        return BaselineReferenceCodeMod::GALACTIC;
    }
    	
    if (name == CBaselineReferenceCode::sSUPERGAL) {
        return BaselineReferenceCodeMod::SUPERGAL;
    }
    	
    if (name == CBaselineReferenceCode::sECLIPTIC) {
        return BaselineReferenceCodeMod::ECLIPTIC;
    }
    	
    if (name == CBaselineReferenceCode::sJMEAN) {
        return BaselineReferenceCodeMod::JMEAN;
    }
    	
    if (name == CBaselineReferenceCode::sJTRUE) {
        return BaselineReferenceCodeMod::JTRUE;
    }
    	
    if (name == CBaselineReferenceCode::sAPP) {
        return BaselineReferenceCodeMod::APP;
    }
    	
    if (name == CBaselineReferenceCode::sBMEAN) {
        return BaselineReferenceCodeMod::BMEAN;
    }
    	
    if (name == CBaselineReferenceCode::sBTRUE) {
        return BaselineReferenceCodeMod::BTRUE;
    }
    	
    if (name == CBaselineReferenceCode::sJNAT) {
        return BaselineReferenceCodeMod::JNAT;
    }
    	
    if (name == CBaselineReferenceCode::sMECLIPTIC) {
        return BaselineReferenceCodeMod::MECLIPTIC;
    }
    	
    if (name == CBaselineReferenceCode::sTECLIPTIC) {
        return BaselineReferenceCodeMod::TECLIPTIC;
    }
    	
    if (name == CBaselineReferenceCode::sTOPO) {
        return BaselineReferenceCodeMod::TOPO;
    }
    	
    if (name == CBaselineReferenceCode::sMERCURY) {
        return BaselineReferenceCodeMod::MERCURY;
    }
    	
    if (name == CBaselineReferenceCode::sVENUS) {
        return BaselineReferenceCodeMod::VENUS;
    }
    	
    if (name == CBaselineReferenceCode::sMARS) {
        return BaselineReferenceCodeMod::MARS;
    }
    	
    if (name == CBaselineReferenceCode::sJUPITER) {
        return BaselineReferenceCodeMod::JUPITER;
    }
    	
    if (name == CBaselineReferenceCode::sSATURN) {
        return BaselineReferenceCodeMod::SATURN;
    }
    	
    if (name == CBaselineReferenceCode::sNEPTUN) {
        return BaselineReferenceCodeMod::NEPTUN;
    }
    	
    if (name == CBaselineReferenceCode::sSUN) {
        return BaselineReferenceCodeMod::SUN;
    }
    	
    if (name == CBaselineReferenceCode::sMOON) {
        return BaselineReferenceCodeMod::MOON;
    }
    	
    if (name == CBaselineReferenceCode::sHADEC) {
        return BaselineReferenceCodeMod::HADEC;
    }
    	
    if (name == CBaselineReferenceCode::sAZEL) {
        return BaselineReferenceCodeMod::AZEL;
    }
    	
    if (name == CBaselineReferenceCode::sAZELGEO) {
        return BaselineReferenceCodeMod::AZELGEO;
    }
    	
    if (name == CBaselineReferenceCode::sAZELSW) {
        return BaselineReferenceCodeMod::AZELSW;
    }
    	
    if (name == CBaselineReferenceCode::sAZELNE) {
        return BaselineReferenceCodeMod::AZELNE;
    }
    	
    if (name == CBaselineReferenceCode::sITRF) {
        return BaselineReferenceCodeMod::ITRF;
    }
    
    throw badString(name);
}

BaselineReferenceCodeMod::BaselineReferenceCode CBaselineReferenceCode::literal(const std::string& name) {
		
    if (name == CBaselineReferenceCode::sJ2000) {
        return BaselineReferenceCodeMod::J2000;
    }
    	
    if (name == CBaselineReferenceCode::sB1950) {
        return BaselineReferenceCodeMod::B1950;
    }
    	
    if (name == CBaselineReferenceCode::sGALACTIC) {
        return BaselineReferenceCodeMod::GALACTIC;
    }
    	
    if (name == CBaselineReferenceCode::sSUPERGAL) {
        return BaselineReferenceCodeMod::SUPERGAL;
    }
    	
    if (name == CBaselineReferenceCode::sECLIPTIC) {
        return BaselineReferenceCodeMod::ECLIPTIC;
    }
    	
    if (name == CBaselineReferenceCode::sJMEAN) {
        return BaselineReferenceCodeMod::JMEAN;
    }
    	
    if (name == CBaselineReferenceCode::sJTRUE) {
        return BaselineReferenceCodeMod::JTRUE;
    }
    	
    if (name == CBaselineReferenceCode::sAPP) {
        return BaselineReferenceCodeMod::APP;
    }
    	
    if (name == CBaselineReferenceCode::sBMEAN) {
        return BaselineReferenceCodeMod::BMEAN;
    }
    	
    if (name == CBaselineReferenceCode::sBTRUE) {
        return BaselineReferenceCodeMod::BTRUE;
    }
    	
    if (name == CBaselineReferenceCode::sJNAT) {
        return BaselineReferenceCodeMod::JNAT;
    }
    	
    if (name == CBaselineReferenceCode::sMECLIPTIC) {
        return BaselineReferenceCodeMod::MECLIPTIC;
    }
    	
    if (name == CBaselineReferenceCode::sTECLIPTIC) {
        return BaselineReferenceCodeMod::TECLIPTIC;
    }
    	
    if (name == CBaselineReferenceCode::sTOPO) {
        return BaselineReferenceCodeMod::TOPO;
    }
    	
    if (name == CBaselineReferenceCode::sMERCURY) {
        return BaselineReferenceCodeMod::MERCURY;
    }
    	
    if (name == CBaselineReferenceCode::sVENUS) {
        return BaselineReferenceCodeMod::VENUS;
    }
    	
    if (name == CBaselineReferenceCode::sMARS) {
        return BaselineReferenceCodeMod::MARS;
    }
    	
    if (name == CBaselineReferenceCode::sJUPITER) {
        return BaselineReferenceCodeMod::JUPITER;
    }
    	
    if (name == CBaselineReferenceCode::sSATURN) {
        return BaselineReferenceCodeMod::SATURN;
    }
    	
    if (name == CBaselineReferenceCode::sNEPTUN) {
        return BaselineReferenceCodeMod::NEPTUN;
    }
    	
    if (name == CBaselineReferenceCode::sSUN) {
        return BaselineReferenceCodeMod::SUN;
    }
    	
    if (name == CBaselineReferenceCode::sMOON) {
        return BaselineReferenceCodeMod::MOON;
    }
    	
    if (name == CBaselineReferenceCode::sHADEC) {
        return BaselineReferenceCodeMod::HADEC;
    }
    	
    if (name == CBaselineReferenceCode::sAZEL) {
        return BaselineReferenceCodeMod::AZEL;
    }
    	
    if (name == CBaselineReferenceCode::sAZELGEO) {
        return BaselineReferenceCodeMod::AZELGEO;
    }
    	
    if (name == CBaselineReferenceCode::sAZELSW) {
        return BaselineReferenceCodeMod::AZELSW;
    }
    	
    if (name == CBaselineReferenceCode::sAZELNE) {
        return BaselineReferenceCodeMod::AZELNE;
    }
    	
    if (name == CBaselineReferenceCode::sITRF) {
        return BaselineReferenceCodeMod::ITRF;
    }
    
    throw badString(name);
}

BaselineReferenceCodeMod::BaselineReferenceCode CBaselineReferenceCode::from_int(unsigned int i) {
	vector<string> names_ = names();
	if (i >= names_.size()) throw badInt(i);
	return newBaselineReferenceCode(names_.at(i));
}

string CBaselineReferenceCode::badString(const string& name) {
	return "'"+name+"' does not correspond to any literal in the enumeration 'BaselineReferenceCode'.";
}

string CBaselineReferenceCode::badInt(unsigned int i) {
	ostringstream oss ;
	oss << "'" << i << "' is out of range for the enumeration 'BaselineReferenceCode'.";
	return oss.str();
}

namespace BaselineReferenceCodeMod {
	std::ostream & operator << ( std::ostream & out, const BaselineReferenceCode& value) {
		out << CBaselineReferenceCode::name(value);
		return out;
	}

	std::istream & operator >> ( std::istream & in , BaselineReferenceCode& value ) {
		in.clear();
		string s ; 
  		in >> s;
  		try {
    		value = CBaselineReferenceCode::literal(s);
  		}
  		catch (string & m) {
    		in.setstate(ios::failbit);
  		}
  		return in;
  	}
}

