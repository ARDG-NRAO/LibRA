
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
 * File CDirectionReferenceCode.cpp
 */
#include <sstream>
#include <alma/Enumerations/CDirectionReferenceCode.h>
#include <string>
using namespace std;

int CDirectionReferenceCode::version() {
	return DirectionReferenceCodeMod::version;
	}
	
string CDirectionReferenceCode::revision () {
	return DirectionReferenceCodeMod::revision;
}

unsigned int CDirectionReferenceCode::size() {
	return 32;
	}
	
	
const std::string& CDirectionReferenceCode::sJ2000 = "J2000";
	
const std::string& CDirectionReferenceCode::sJMEAN = "JMEAN";
	
const std::string& CDirectionReferenceCode::sJTRUE = "JTRUE";
	
const std::string& CDirectionReferenceCode::sAPP = "APP";
	
const std::string& CDirectionReferenceCode::sB1950 = "B1950";
	
const std::string& CDirectionReferenceCode::sB1950_VLA = "B1950_VLA";
	
const std::string& CDirectionReferenceCode::sBMEAN = "BMEAN";
	
const std::string& CDirectionReferenceCode::sBTRUE = "BTRUE";
	
const std::string& CDirectionReferenceCode::sGALACTIC = "GALACTIC";
	
const std::string& CDirectionReferenceCode::sHADEC = "HADEC";
	
const std::string& CDirectionReferenceCode::sAZELSW = "AZELSW";
	
const std::string& CDirectionReferenceCode::sAZELSWGEO = "AZELSWGEO";
	
const std::string& CDirectionReferenceCode::sAZELNE = "AZELNE";
	
const std::string& CDirectionReferenceCode::sAZELNEGEO = "AZELNEGEO";
	
const std::string& CDirectionReferenceCode::sJNAT = "JNAT";
	
const std::string& CDirectionReferenceCode::sECLIPTIC = "ECLIPTIC";
	
const std::string& CDirectionReferenceCode::sMECLIPTIC = "MECLIPTIC";
	
const std::string& CDirectionReferenceCode::sTECLIPTIC = "TECLIPTIC";
	
const std::string& CDirectionReferenceCode::sSUPERGAL = "SUPERGAL";
	
const std::string& CDirectionReferenceCode::sITRF = "ITRF";
	
const std::string& CDirectionReferenceCode::sTOPO = "TOPO";
	
const std::string& CDirectionReferenceCode::sICRS = "ICRS";
	
const std::string& CDirectionReferenceCode::sMERCURY = "MERCURY";
	
const std::string& CDirectionReferenceCode::sVENUS = "VENUS";
	
const std::string& CDirectionReferenceCode::sMARS = "MARS";
	
const std::string& CDirectionReferenceCode::sJUPITER = "JUPITER";
	
const std::string& CDirectionReferenceCode::sSATURN = "SATURN";
	
const std::string& CDirectionReferenceCode::sURANUS = "URANUS";
	
const std::string& CDirectionReferenceCode::sNEPTUNE = "NEPTUNE";
	
const std::string& CDirectionReferenceCode::sPLUTO = "PLUTO";
	
const std::string& CDirectionReferenceCode::sSUN = "SUN";
	
const std::string& CDirectionReferenceCode::sMOON = "MOON";
	
const std::vector<std::string> CDirectionReferenceCode::names() {
    std::vector<std::string> enumSet;
    
    enumSet.insert(enumSet.end(), CDirectionReferenceCode::sJ2000);
    
    enumSet.insert(enumSet.end(), CDirectionReferenceCode::sJMEAN);
    
    enumSet.insert(enumSet.end(), CDirectionReferenceCode::sJTRUE);
    
    enumSet.insert(enumSet.end(), CDirectionReferenceCode::sAPP);
    
    enumSet.insert(enumSet.end(), CDirectionReferenceCode::sB1950);
    
    enumSet.insert(enumSet.end(), CDirectionReferenceCode::sB1950_VLA);
    
    enumSet.insert(enumSet.end(), CDirectionReferenceCode::sBMEAN);
    
    enumSet.insert(enumSet.end(), CDirectionReferenceCode::sBTRUE);
    
    enumSet.insert(enumSet.end(), CDirectionReferenceCode::sGALACTIC);
    
    enumSet.insert(enumSet.end(), CDirectionReferenceCode::sHADEC);
    
    enumSet.insert(enumSet.end(), CDirectionReferenceCode::sAZELSW);
    
    enumSet.insert(enumSet.end(), CDirectionReferenceCode::sAZELSWGEO);
    
    enumSet.insert(enumSet.end(), CDirectionReferenceCode::sAZELNE);
    
    enumSet.insert(enumSet.end(), CDirectionReferenceCode::sAZELNEGEO);
    
    enumSet.insert(enumSet.end(), CDirectionReferenceCode::sJNAT);
    
    enumSet.insert(enumSet.end(), CDirectionReferenceCode::sECLIPTIC);
    
    enumSet.insert(enumSet.end(), CDirectionReferenceCode::sMECLIPTIC);
    
    enumSet.insert(enumSet.end(), CDirectionReferenceCode::sTECLIPTIC);
    
    enumSet.insert(enumSet.end(), CDirectionReferenceCode::sSUPERGAL);
    
    enumSet.insert(enumSet.end(), CDirectionReferenceCode::sITRF);
    
    enumSet.insert(enumSet.end(), CDirectionReferenceCode::sTOPO);
    
    enumSet.insert(enumSet.end(), CDirectionReferenceCode::sICRS);
    
    enumSet.insert(enumSet.end(), CDirectionReferenceCode::sMERCURY);
    
    enumSet.insert(enumSet.end(), CDirectionReferenceCode::sVENUS);
    
    enumSet.insert(enumSet.end(), CDirectionReferenceCode::sMARS);
    
    enumSet.insert(enumSet.end(), CDirectionReferenceCode::sJUPITER);
    
    enumSet.insert(enumSet.end(), CDirectionReferenceCode::sSATURN);
    
    enumSet.insert(enumSet.end(), CDirectionReferenceCode::sURANUS);
    
    enumSet.insert(enumSet.end(), CDirectionReferenceCode::sNEPTUNE);
    
    enumSet.insert(enumSet.end(), CDirectionReferenceCode::sPLUTO);
    
    enumSet.insert(enumSet.end(), CDirectionReferenceCode::sSUN);
    
    enumSet.insert(enumSet.end(), CDirectionReferenceCode::sMOON);
        
    return enumSet;
}

std::string CDirectionReferenceCode::name(const DirectionReferenceCodeMod::DirectionReferenceCode& f) {
    switch (f) {
    
    case DirectionReferenceCodeMod::J2000:
      return CDirectionReferenceCode::sJ2000;
    
    case DirectionReferenceCodeMod::JMEAN:
      return CDirectionReferenceCode::sJMEAN;
    
    case DirectionReferenceCodeMod::JTRUE:
      return CDirectionReferenceCode::sJTRUE;
    
    case DirectionReferenceCodeMod::APP:
      return CDirectionReferenceCode::sAPP;
    
    case DirectionReferenceCodeMod::B1950:
      return CDirectionReferenceCode::sB1950;
    
    case DirectionReferenceCodeMod::B1950_VLA:
      return CDirectionReferenceCode::sB1950_VLA;
    
    case DirectionReferenceCodeMod::BMEAN:
      return CDirectionReferenceCode::sBMEAN;
    
    case DirectionReferenceCodeMod::BTRUE:
      return CDirectionReferenceCode::sBTRUE;
    
    case DirectionReferenceCodeMod::GALACTIC:
      return CDirectionReferenceCode::sGALACTIC;
    
    case DirectionReferenceCodeMod::HADEC:
      return CDirectionReferenceCode::sHADEC;
    
    case DirectionReferenceCodeMod::AZELSW:
      return CDirectionReferenceCode::sAZELSW;
    
    case DirectionReferenceCodeMod::AZELSWGEO:
      return CDirectionReferenceCode::sAZELSWGEO;
    
    case DirectionReferenceCodeMod::AZELNE:
      return CDirectionReferenceCode::sAZELNE;
    
    case DirectionReferenceCodeMod::AZELNEGEO:
      return CDirectionReferenceCode::sAZELNEGEO;
    
    case DirectionReferenceCodeMod::JNAT:
      return CDirectionReferenceCode::sJNAT;
    
    case DirectionReferenceCodeMod::ECLIPTIC:
      return CDirectionReferenceCode::sECLIPTIC;
    
    case DirectionReferenceCodeMod::MECLIPTIC:
      return CDirectionReferenceCode::sMECLIPTIC;
    
    case DirectionReferenceCodeMod::TECLIPTIC:
      return CDirectionReferenceCode::sTECLIPTIC;
    
    case DirectionReferenceCodeMod::SUPERGAL:
      return CDirectionReferenceCode::sSUPERGAL;
    
    case DirectionReferenceCodeMod::ITRF:
      return CDirectionReferenceCode::sITRF;
    
    case DirectionReferenceCodeMod::TOPO:
      return CDirectionReferenceCode::sTOPO;
    
    case DirectionReferenceCodeMod::ICRS:
      return CDirectionReferenceCode::sICRS;
    
    case DirectionReferenceCodeMod::MERCURY:
      return CDirectionReferenceCode::sMERCURY;
    
    case DirectionReferenceCodeMod::VENUS:
      return CDirectionReferenceCode::sVENUS;
    
    case DirectionReferenceCodeMod::MARS:
      return CDirectionReferenceCode::sMARS;
    
    case DirectionReferenceCodeMod::JUPITER:
      return CDirectionReferenceCode::sJUPITER;
    
    case DirectionReferenceCodeMod::SATURN:
      return CDirectionReferenceCode::sSATURN;
    
    case DirectionReferenceCodeMod::URANUS:
      return CDirectionReferenceCode::sURANUS;
    
    case DirectionReferenceCodeMod::NEPTUNE:
      return CDirectionReferenceCode::sNEPTUNE;
    
    case DirectionReferenceCodeMod::PLUTO:
      return CDirectionReferenceCode::sPLUTO;
    
    case DirectionReferenceCodeMod::SUN:
      return CDirectionReferenceCode::sSUN;
    
    case DirectionReferenceCodeMod::MOON:
      return CDirectionReferenceCode::sMOON;
    	
    }
    // Impossible siutation but....who knows with C++ enums
    throw badInt((int) f);
}

DirectionReferenceCodeMod::DirectionReferenceCode CDirectionReferenceCode::newDirectionReferenceCode(const std::string& name) {
		
    if (name == CDirectionReferenceCode::sJ2000) {
        return DirectionReferenceCodeMod::J2000;
    }
    	
    if (name == CDirectionReferenceCode::sJMEAN) {
        return DirectionReferenceCodeMod::JMEAN;
    }
    	
    if (name == CDirectionReferenceCode::sJTRUE) {
        return DirectionReferenceCodeMod::JTRUE;
    }
    	
    if (name == CDirectionReferenceCode::sAPP) {
        return DirectionReferenceCodeMod::APP;
    }
    	
    if (name == CDirectionReferenceCode::sB1950) {
        return DirectionReferenceCodeMod::B1950;
    }
    	
    if (name == CDirectionReferenceCode::sB1950_VLA) {
        return DirectionReferenceCodeMod::B1950_VLA;
    }
    	
    if (name == CDirectionReferenceCode::sBMEAN) {
        return DirectionReferenceCodeMod::BMEAN;
    }
    	
    if (name == CDirectionReferenceCode::sBTRUE) {
        return DirectionReferenceCodeMod::BTRUE;
    }
    	
    if (name == CDirectionReferenceCode::sGALACTIC) {
        return DirectionReferenceCodeMod::GALACTIC;
    }
    	
    if (name == CDirectionReferenceCode::sHADEC) {
        return DirectionReferenceCodeMod::HADEC;
    }
    	
    if (name == CDirectionReferenceCode::sAZELSW) {
        return DirectionReferenceCodeMod::AZELSW;
    }
    	
    if (name == CDirectionReferenceCode::sAZELSWGEO) {
        return DirectionReferenceCodeMod::AZELSWGEO;
    }
    	
    if (name == CDirectionReferenceCode::sAZELNE) {
        return DirectionReferenceCodeMod::AZELNE;
    }
    	
    if (name == CDirectionReferenceCode::sAZELNEGEO) {
        return DirectionReferenceCodeMod::AZELNEGEO;
    }
    	
    if (name == CDirectionReferenceCode::sJNAT) {
        return DirectionReferenceCodeMod::JNAT;
    }
    	
    if (name == CDirectionReferenceCode::sECLIPTIC) {
        return DirectionReferenceCodeMod::ECLIPTIC;
    }
    	
    if (name == CDirectionReferenceCode::sMECLIPTIC) {
        return DirectionReferenceCodeMod::MECLIPTIC;
    }
    	
    if (name == CDirectionReferenceCode::sTECLIPTIC) {
        return DirectionReferenceCodeMod::TECLIPTIC;
    }
    	
    if (name == CDirectionReferenceCode::sSUPERGAL) {
        return DirectionReferenceCodeMod::SUPERGAL;
    }
    	
    if (name == CDirectionReferenceCode::sITRF) {
        return DirectionReferenceCodeMod::ITRF;
    }
    	
    if (name == CDirectionReferenceCode::sTOPO) {
        return DirectionReferenceCodeMod::TOPO;
    }
    	
    if (name == CDirectionReferenceCode::sICRS) {
        return DirectionReferenceCodeMod::ICRS;
    }
    	
    if (name == CDirectionReferenceCode::sMERCURY) {
        return DirectionReferenceCodeMod::MERCURY;
    }
    	
    if (name == CDirectionReferenceCode::sVENUS) {
        return DirectionReferenceCodeMod::VENUS;
    }
    	
    if (name == CDirectionReferenceCode::sMARS) {
        return DirectionReferenceCodeMod::MARS;
    }
    	
    if (name == CDirectionReferenceCode::sJUPITER) {
        return DirectionReferenceCodeMod::JUPITER;
    }
    	
    if (name == CDirectionReferenceCode::sSATURN) {
        return DirectionReferenceCodeMod::SATURN;
    }
    	
    if (name == CDirectionReferenceCode::sURANUS) {
        return DirectionReferenceCodeMod::URANUS;
    }
    	
    if (name == CDirectionReferenceCode::sNEPTUNE) {
        return DirectionReferenceCodeMod::NEPTUNE;
    }
    	
    if (name == CDirectionReferenceCode::sPLUTO) {
        return DirectionReferenceCodeMod::PLUTO;
    }
    	
    if (name == CDirectionReferenceCode::sSUN) {
        return DirectionReferenceCodeMod::SUN;
    }
    	
    if (name == CDirectionReferenceCode::sMOON) {
        return DirectionReferenceCodeMod::MOON;
    }
    
    throw badString(name);
}

DirectionReferenceCodeMod::DirectionReferenceCode CDirectionReferenceCode::literal(const std::string& name) {
		
    if (name == CDirectionReferenceCode::sJ2000) {
        return DirectionReferenceCodeMod::J2000;
    }
    	
    if (name == CDirectionReferenceCode::sJMEAN) {
        return DirectionReferenceCodeMod::JMEAN;
    }
    	
    if (name == CDirectionReferenceCode::sJTRUE) {
        return DirectionReferenceCodeMod::JTRUE;
    }
    	
    if (name == CDirectionReferenceCode::sAPP) {
        return DirectionReferenceCodeMod::APP;
    }
    	
    if (name == CDirectionReferenceCode::sB1950) {
        return DirectionReferenceCodeMod::B1950;
    }
    	
    if (name == CDirectionReferenceCode::sB1950_VLA) {
        return DirectionReferenceCodeMod::B1950_VLA;
    }
    	
    if (name == CDirectionReferenceCode::sBMEAN) {
        return DirectionReferenceCodeMod::BMEAN;
    }
    	
    if (name == CDirectionReferenceCode::sBTRUE) {
        return DirectionReferenceCodeMod::BTRUE;
    }
    	
    if (name == CDirectionReferenceCode::sGALACTIC) {
        return DirectionReferenceCodeMod::GALACTIC;
    }
    	
    if (name == CDirectionReferenceCode::sHADEC) {
        return DirectionReferenceCodeMod::HADEC;
    }
    	
    if (name == CDirectionReferenceCode::sAZELSW) {
        return DirectionReferenceCodeMod::AZELSW;
    }
    	
    if (name == CDirectionReferenceCode::sAZELSWGEO) {
        return DirectionReferenceCodeMod::AZELSWGEO;
    }
    	
    if (name == CDirectionReferenceCode::sAZELNE) {
        return DirectionReferenceCodeMod::AZELNE;
    }
    	
    if (name == CDirectionReferenceCode::sAZELNEGEO) {
        return DirectionReferenceCodeMod::AZELNEGEO;
    }
    	
    if (name == CDirectionReferenceCode::sJNAT) {
        return DirectionReferenceCodeMod::JNAT;
    }
    	
    if (name == CDirectionReferenceCode::sECLIPTIC) {
        return DirectionReferenceCodeMod::ECLIPTIC;
    }
    	
    if (name == CDirectionReferenceCode::sMECLIPTIC) {
        return DirectionReferenceCodeMod::MECLIPTIC;
    }
    	
    if (name == CDirectionReferenceCode::sTECLIPTIC) {
        return DirectionReferenceCodeMod::TECLIPTIC;
    }
    	
    if (name == CDirectionReferenceCode::sSUPERGAL) {
        return DirectionReferenceCodeMod::SUPERGAL;
    }
    	
    if (name == CDirectionReferenceCode::sITRF) {
        return DirectionReferenceCodeMod::ITRF;
    }
    	
    if (name == CDirectionReferenceCode::sTOPO) {
        return DirectionReferenceCodeMod::TOPO;
    }
    	
    if (name == CDirectionReferenceCode::sICRS) {
        return DirectionReferenceCodeMod::ICRS;
    }
    	
    if (name == CDirectionReferenceCode::sMERCURY) {
        return DirectionReferenceCodeMod::MERCURY;
    }
    	
    if (name == CDirectionReferenceCode::sVENUS) {
        return DirectionReferenceCodeMod::VENUS;
    }
    	
    if (name == CDirectionReferenceCode::sMARS) {
        return DirectionReferenceCodeMod::MARS;
    }
    	
    if (name == CDirectionReferenceCode::sJUPITER) {
        return DirectionReferenceCodeMod::JUPITER;
    }
    	
    if (name == CDirectionReferenceCode::sSATURN) {
        return DirectionReferenceCodeMod::SATURN;
    }
    	
    if (name == CDirectionReferenceCode::sURANUS) {
        return DirectionReferenceCodeMod::URANUS;
    }
    	
    if (name == CDirectionReferenceCode::sNEPTUNE) {
        return DirectionReferenceCodeMod::NEPTUNE;
    }
    	
    if (name == CDirectionReferenceCode::sPLUTO) {
        return DirectionReferenceCodeMod::PLUTO;
    }
    	
    if (name == CDirectionReferenceCode::sSUN) {
        return DirectionReferenceCodeMod::SUN;
    }
    	
    if (name == CDirectionReferenceCode::sMOON) {
        return DirectionReferenceCodeMod::MOON;
    }
    
    throw badString(name);
}

DirectionReferenceCodeMod::DirectionReferenceCode CDirectionReferenceCode::from_int(unsigned int i) {
	vector<string> names_ = names();
	if (i >= names_.size()) throw badInt(i);
	return newDirectionReferenceCode(names_.at(i));
}

string CDirectionReferenceCode::badString(const string& name) {
	return "'"+name+"' does not correspond to any literal in the enumeration 'DirectionReferenceCode'.";
}

string CDirectionReferenceCode::badInt(unsigned int i) {
	ostringstream oss ;
	oss << "'" << i << "' is out of range for the enumeration 'DirectionReferenceCode'.";
	return oss.str();
}

namespace DirectionReferenceCodeMod {
	std::ostream & operator << ( std::ostream & out, const DirectionReferenceCode& value) {
		out << CDirectionReferenceCode::name(value);
		return out;
	}

	std::istream & operator >> ( std::istream & in , DirectionReferenceCode& value ) {
		in.clear();
		string s ; 
  		in >> s;
  		try {
    		value = CDirectionReferenceCode::literal(s);
  		}
  		catch (string & m) {
    		in.setstate(ios::failbit);
  		}
  		return in;
  	}
}

