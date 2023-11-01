
#ifndef CDirectionReferenceCode_H
#define CDirectionReferenceCode_H

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
 * File CDirectionReferenceCode.h
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#include <iostream>
#include <string>
#include <vector>
/**
  * A namespace to encapsulate the DirectionReferenceCode enumeration.
  */
#ifndef WITHOUT_ACS
#include <almaEnumerations_IFC.h>
#else

// This part mimics the behaviour of 
namespace DirectionReferenceCodeMod
{
  //! DirectionReferenceCode.
  //! defines reference frames to qualify the measure of a direction.
  
  const char *const revision = "-1";
  const int version = 1;
  
  enum DirectionReferenceCode
  { 
    J2000 /*!< mean equator and equinox at J2000.0 */
     ,
    JMEAN /*!< mean equator and equinox at frame epoch. */
     ,
    JTRUE /*!< true equator and equinox at frame epoch. */
     ,
    APP /*!< apparent geocentric position. */
     ,
    B1950 /*!< mean epoch and ecliptic at B1950.0. */
     ,
    B1950_VLA /*!<  */
     ,
    BMEAN /*!< mean equator and equinox at frame epoch. */
     ,
    BTRUE /*!< true equator and equinox at frame epoch. */
     ,
    GALACTIC /*!< galactic coordinates. */
     ,
    HADEC /*!< topocentric HA and declination. */
     ,
    AZELSW /*!< topocentric Azimuth and Elevation (N through E). */
     ,
    AZELSWGEO /*!<  */
     ,
    AZELNE /*!< idem AZEL */
     ,
    AZELNEGEO /*!<  */
     ,
    JNAT /*!< geocentric natural frame. */
     ,
    ECLIPTIC /*!< ecliptic for J2000.0 equator, equinox. */
     ,
    MECLIPTIC /*!< ecliptic for mean equator of date. */
     ,
    TECLIPTIC /*!< ecliptic for true equatorof date. */
     ,
    SUPERGAL /*!< supergalactic coordinates. */
     ,
    ITRF /*!< coordinates wrt ITRF earth frame. */
     ,
    TOPO /*!< apparent topocentric position. */
     ,
    ICRS /*!<  */
     ,
    MERCURY /*!< from JPL DE table. */
     ,
    VENUS /*!<  */
     ,
    MARS /*!<  */
     ,
    JUPITER /*!<  */
     ,
    SATURN /*!<  */
     ,
    URANUS /*!<  */
     ,
    NEPTUNE /*!<  */
     ,
    PLUTO /*!<  */
     ,
    SUN /*!<  */
     ,
    MOON /*!<  */
     
  };
  typedef DirectionReferenceCode &DirectionReferenceCode_out;
} 
#endif

namespace DirectionReferenceCodeMod {
	std::ostream & operator << ( std::ostream & out, const DirectionReferenceCode& value);
	std::istream & operator >> ( std::istream & in , DirectionReferenceCode& value );
}

/** 
  * A helper class for the enumeration DirectionReferenceCode.
  * 
  */
class CDirectionReferenceCode {
  public:
 
	/**
	  * Enumerators as strings.
	  */  
	
	static const std::string& sJ2000; /*!< A const string equal to "J2000".*/
	
	static const std::string& sJMEAN; /*!< A const string equal to "JMEAN".*/
	
	static const std::string& sJTRUE; /*!< A const string equal to "JTRUE".*/
	
	static const std::string& sAPP; /*!< A const string equal to "APP".*/
	
	static const std::string& sB1950; /*!< A const string equal to "B1950".*/
	
	static const std::string& sB1950_VLA; /*!< A const string equal to "B1950_VLA".*/
	
	static const std::string& sBMEAN; /*!< A const string equal to "BMEAN".*/
	
	static const std::string& sBTRUE; /*!< A const string equal to "BTRUE".*/
	
	static const std::string& sGALACTIC; /*!< A const string equal to "GALACTIC".*/
	
	static const std::string& sHADEC; /*!< A const string equal to "HADEC".*/
	
	static const std::string& sAZELSW; /*!< A const string equal to "AZELSW".*/
	
	static const std::string& sAZELSWGEO; /*!< A const string equal to "AZELSWGEO".*/
	
	static const std::string& sAZELNE; /*!< A const string equal to "AZELNE".*/
	
	static const std::string& sAZELNEGEO; /*!< A const string equal to "AZELNEGEO".*/
	
	static const std::string& sJNAT; /*!< A const string equal to "JNAT".*/
	
	static const std::string& sECLIPTIC; /*!< A const string equal to "ECLIPTIC".*/
	
	static const std::string& sMECLIPTIC; /*!< A const string equal to "MECLIPTIC".*/
	
	static const std::string& sTECLIPTIC; /*!< A const string equal to "TECLIPTIC".*/
	
	static const std::string& sSUPERGAL; /*!< A const string equal to "SUPERGAL".*/
	
	static const std::string& sITRF; /*!< A const string equal to "ITRF".*/
	
	static const std::string& sTOPO; /*!< A const string equal to "TOPO".*/
	
	static const std::string& sICRS; /*!< A const string equal to "ICRS".*/
	
	static const std::string& sMERCURY; /*!< A const string equal to "MERCURY".*/
	
	static const std::string& sVENUS; /*!< A const string equal to "VENUS".*/
	
	static const std::string& sMARS; /*!< A const string equal to "MARS".*/
	
	static const std::string& sJUPITER; /*!< A const string equal to "JUPITER".*/
	
	static const std::string& sSATURN; /*!< A const string equal to "SATURN".*/
	
	static const std::string& sURANUS; /*!< A const string equal to "URANUS".*/
	
	static const std::string& sNEPTUNE; /*!< A const string equal to "NEPTUNE".*/
	
	static const std::string& sPLUTO; /*!< A const string equal to "PLUTO".*/
	
	static const std::string& sSUN; /*!< A const string equal to "SUN".*/
	
	static const std::string& sMOON; /*!< A const string equal to "MOON".*/
	

	/**
	  * Return the major version number as an int.
	  * @return an int.
	  */
	  static int version() ;
	  
	  
	  /**
	    * Return the revision as a string.
	    * @return a string
	    *
	    */
	  static std::string revision() ;
	  
	  
     /**
       * Return the number of enumerators declared in DirectionReferenceCodeMod::DirectionReferenceCode.
       * @return an unsigned int.
       */
       static unsigned int size() ;
       
       
    /**
      * Returns an enumerator as a string.
      * @param e an enumerator of DirectionReferenceCodeMod::DirectionReferenceCode.
      * @return a string.
      */
	static std::string name(const DirectionReferenceCodeMod::DirectionReferenceCode& e);
	
	/**
	  * Equivalent to the name method.
	  */
    static std::string toString(const DirectionReferenceCodeMod::DirectionReferenceCode& f) { return name(f); }

	/** 
	  * Returns vector of  all the enumerators as strings. 
	  * The strings are stored in the vector in the same order than the enumerators are declared in the enumeration. 
	  * @return a vector of string.
	  */
     static const std::vector<std::string> names();	 
    
   	
   	// Create a DirectionReferenceCode enumeration object by specifying its name.
   	static DirectionReferenceCodeMod::DirectionReferenceCode newDirectionReferenceCode(const std::string& name);
   	
   	/*! Return a DirectionReferenceCode's enumerator  given a string.
   	  * @param name the string representation of the enumerator.
   	 *  @return a DirectionReferenceCodeMod::DirectionReferenceCode's enumerator.
   	 *  @throws a string containing an error message if no enumerator could be found for this name.
   	 */
 	static DirectionReferenceCodeMod::DirectionReferenceCode literal(const std::string& name);
 	
    /*! Return a DirectionReferenceCode's enumerator given an unsigned int.
      * @param i the index of the enumerator in DirectionReferenceCodeMod::DirectionReferenceCode.
      * @return a DirectionReferenceCodeMod::DirectionReferenceCode's enumerator.
      * @throws a string containing an error message if no enumerator could be found for this integer.
      */
 	static DirectionReferenceCodeMod::DirectionReferenceCode from_int(unsigned int i);	
 	

  private:
    /* Not Implemented.  This is a pure static class. */
    CDirectionReferenceCode();
    CDirectionReferenceCode(const CDirectionReferenceCode&);
    CDirectionReferenceCode& operator=(const CDirectionReferenceCode&);
    
    static std::string badString(const std::string& name) ;
  	static std::string badInt(unsigned int i) ;
  	
};
 
#endif /*!CDirectionReferenceCode_H*/
