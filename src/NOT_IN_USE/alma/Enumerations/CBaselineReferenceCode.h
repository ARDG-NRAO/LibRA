
#ifndef CBaselineReferenceCode_H
#define CBaselineReferenceCode_H

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
 * File CBaselineReferenceCode.h
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#include <iostream>
#include <string>
#include <vector>
/**
  * A namespace to encapsulate the BaselineReferenceCode enumeration.
  */
#ifndef WITHOUT_ACS
#include <almaEnumerations_IFC.h>
#else

// This part mimics the behaviour of 
namespace BaselineReferenceCodeMod
{
  //! BaselineReferenceCode.
  //! defines reference frames to qualify the measure of a baseline.
  
  const char *const revision = "-1";
  const int version = 1;
  
  enum BaselineReferenceCode
  { 
    J2000 /*!< mean equator, equinox J2000.0 */
     ,
    B1950 /*!< mean equator, equinox B1950.0 */
     ,
    GALACTIC /*!< galactic coordinates. */
     ,
    SUPERGAL /*!< supergalactic coordinates. */
     ,
    ECLIPTIC /*!< ecliptic for J2000.0 */
     ,
    JMEAN /*!< mean equator. */
     ,
    JTRUE /*!< true equator. */
     ,
    APP /*!< apparent geocentric. */
     ,
    BMEAN /*!< mean equator. */
     ,
    BTRUE /*!< true equator. */
     ,
    JNAT /*!< geocentric natural frame. */
     ,
    MECLIPTIC /*!< ecliptic for mean equator. */
     ,
    TECLIPTIC /*!< ecliptic for true equator. */
     ,
    TOPO /*!< apparent geocentric */
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
    NEPTUN /*!<  */
     ,
    SUN /*!<  */
     ,
    MOON /*!<  */
     ,
    HADEC /*!<  */
     ,
    AZEL /*!<  */
     ,
    AZELGEO /*!<  */
     ,
    AZELSW /*!< topocentric Az/El (N => E). */
     ,
    AZELNE /*!< idem AZEL. */
     ,
    ITRF /*!< ITRF earth frame. */
     
  };
  typedef BaselineReferenceCode &BaselineReferenceCode_out;
} 
#endif

namespace BaselineReferenceCodeMod {
	std::ostream & operator << ( std::ostream & out, const BaselineReferenceCode& value);
	std::istream & operator >> ( std::istream & in , BaselineReferenceCode& value );
}

/** 
  * A helper class for the enumeration BaselineReferenceCode.
  * 
  */
class CBaselineReferenceCode {
  public:
 
	/**
	  * Enumerators as strings.
	  */  
	
	static const std::string& sJ2000; /*!< A const string equal to "J2000".*/
	
	static const std::string& sB1950; /*!< A const string equal to "B1950".*/
	
	static const std::string& sGALACTIC; /*!< A const string equal to "GALACTIC".*/
	
	static const std::string& sSUPERGAL; /*!< A const string equal to "SUPERGAL".*/
	
	static const std::string& sECLIPTIC; /*!< A const string equal to "ECLIPTIC".*/
	
	static const std::string& sJMEAN; /*!< A const string equal to "JMEAN".*/
	
	static const std::string& sJTRUE; /*!< A const string equal to "JTRUE".*/
	
	static const std::string& sAPP; /*!< A const string equal to "APP".*/
	
	static const std::string& sBMEAN; /*!< A const string equal to "BMEAN".*/
	
	static const std::string& sBTRUE; /*!< A const string equal to "BTRUE".*/
	
	static const std::string& sJNAT; /*!< A const string equal to "JNAT".*/
	
	static const std::string& sMECLIPTIC; /*!< A const string equal to "MECLIPTIC".*/
	
	static const std::string& sTECLIPTIC; /*!< A const string equal to "TECLIPTIC".*/
	
	static const std::string& sTOPO; /*!< A const string equal to "TOPO".*/
	
	static const std::string& sMERCURY; /*!< A const string equal to "MERCURY".*/
	
	static const std::string& sVENUS; /*!< A const string equal to "VENUS".*/
	
	static const std::string& sMARS; /*!< A const string equal to "MARS".*/
	
	static const std::string& sJUPITER; /*!< A const string equal to "JUPITER".*/
	
	static const std::string& sSATURN; /*!< A const string equal to "SATURN".*/
	
	static const std::string& sNEPTUN; /*!< A const string equal to "NEPTUN".*/
	
	static const std::string& sSUN; /*!< A const string equal to "SUN".*/
	
	static const std::string& sMOON; /*!< A const string equal to "MOON".*/
	
	static const std::string& sHADEC; /*!< A const string equal to "HADEC".*/
	
	static const std::string& sAZEL; /*!< A const string equal to "AZEL".*/
	
	static const std::string& sAZELGEO; /*!< A const string equal to "AZELGEO".*/
	
	static const std::string& sAZELSW; /*!< A const string equal to "AZELSW".*/
	
	static const std::string& sAZELNE; /*!< A const string equal to "AZELNE".*/
	
	static const std::string& sITRF; /*!< A const string equal to "ITRF".*/
	

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
       * Return the number of enumerators declared in BaselineReferenceCodeMod::BaselineReferenceCode.
       * @return an unsigned int.
       */
       static unsigned int size() ;
       
       
    /**
      * Returns an enumerator as a string.
      * @param e an enumerator of BaselineReferenceCodeMod::BaselineReferenceCode.
      * @return a string.
      */
	static std::string name(const BaselineReferenceCodeMod::BaselineReferenceCode& e);
	
	/**
	  * Equivalent to the name method.
	  */
    static std::string toString(const BaselineReferenceCodeMod::BaselineReferenceCode& f) { return name(f); }

	/** 
	  * Returns vector of  all the enumerators as strings. 
	  * The strings are stored in the vector in the same order than the enumerators are declared in the enumeration. 
	  * @return a vector of string.
	  */
     static const std::vector<std::string> names();	 
    
   	
   	// Create a BaselineReferenceCode enumeration object by specifying its name.
   	static BaselineReferenceCodeMod::BaselineReferenceCode newBaselineReferenceCode(const std::string& name);
   	
   	/*! Return a BaselineReferenceCode's enumerator  given a string.
   	  * @param name the string representation of the enumerator.
   	 *  @return a BaselineReferenceCodeMod::BaselineReferenceCode's enumerator.
   	 *  @throws a string containing an error message if no enumerator could be found for this name.
   	 */
 	static BaselineReferenceCodeMod::BaselineReferenceCode literal(const std::string& name);
 	
    /*! Return a BaselineReferenceCode's enumerator given an unsigned int.
      * @param i the index of the enumerator in BaselineReferenceCodeMod::BaselineReferenceCode.
      * @return a BaselineReferenceCodeMod::BaselineReferenceCode's enumerator.
      * @throws a string containing an error message if no enumerator could be found for this integer.
      */
 	static BaselineReferenceCodeMod::BaselineReferenceCode from_int(unsigned int i);	
 	

  private:
    /* Not Implemented.  This is a pure static class. */
    CBaselineReferenceCode();
    CBaselineReferenceCode(const CBaselineReferenceCode&);
    CBaselineReferenceCode& operator=(const CBaselineReferenceCode&);
    
    static std::string badString(const std::string& name) ;
  	static std::string badInt(unsigned int i) ;
  	
};
 
#endif /*!CBaselineReferenceCode_H*/
