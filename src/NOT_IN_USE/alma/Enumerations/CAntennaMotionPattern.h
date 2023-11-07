
#ifndef CAntennaMotionPattern_H
#define CAntennaMotionPattern_H

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
 * File CAntennaMotionPattern.h
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#include <iostream>
#include <string>
#include <vector>
/**
  * A namespace to encapsulate the AntennaMotionPattern enumeration.
  */
#ifndef WITHOUT_ACS
#include <almaEnumerations_IFC.h>
#else

// This part mimics the behaviour of 
namespace AntennaMotionPatternMod
{
  //! AntennaMotionPattern.
  //! Motion pattern of antenna , e.g. in a calibration scan.
  
  const char *const revision = "-1";
  const int version = 1;
  
  enum AntennaMotionPattern
  { 
    NONE /*!< No pattern. */
     ,
    CROSS_SCAN /*!< Crossed scan (continuous pattern) */
     ,
    SPIRAL /*!< Spiral pattern */
     ,
    CIRCLE /*!< Circular pattern */
     ,
    THREE_POINTS /*!< Three points pattern. */
     ,
    FOUR_POINTS /*!< Four points pattern. */
     ,
    FIVE_POINTS /*!< Five points pattern. */
     ,
    TEST /*!< Reserved for development. */
     ,
    UNSPECIFIED /*!< Unspecified pattern. */
     ,
    STAR /*!<  */
     ,
    LISSAJOUS /*!<  */
     
  };
  typedef AntennaMotionPattern &AntennaMotionPattern_out;
} 
#endif

namespace AntennaMotionPatternMod {
	std::ostream & operator << ( std::ostream & out, const AntennaMotionPattern& value);
	std::istream & operator >> ( std::istream & in , AntennaMotionPattern& value );
}

/** 
  * A helper class for the enumeration AntennaMotionPattern.
  * 
  */
class CAntennaMotionPattern {
  public:
 
	/**
	  * Enumerators as strings.
	  */  
	
	static const std::string& sNONE; /*!< A const string equal to "NONE".*/
	
	static const std::string& sCROSS_SCAN; /*!< A const string equal to "CROSS_SCAN".*/
	
	static const std::string& sSPIRAL; /*!< A const string equal to "SPIRAL".*/
	
	static const std::string& sCIRCLE; /*!< A const string equal to "CIRCLE".*/
	
	static const std::string& sTHREE_POINTS; /*!< A const string equal to "THREE_POINTS".*/
	
	static const std::string& sFOUR_POINTS; /*!< A const string equal to "FOUR_POINTS".*/
	
	static const std::string& sFIVE_POINTS; /*!< A const string equal to "FIVE_POINTS".*/
	
	static const std::string& sTEST; /*!< A const string equal to "TEST".*/
	
	static const std::string& sUNSPECIFIED; /*!< A const string equal to "UNSPECIFIED".*/
	
	static const std::string& sSTAR; /*!< A const string equal to "STAR".*/
	
	static const std::string& sLISSAJOUS; /*!< A const string equal to "LISSAJOUS".*/
	

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
       * Return the number of enumerators declared in AntennaMotionPatternMod::AntennaMotionPattern.
       * @return an unsigned int.
       */
       static unsigned int size() ;
       
       
    /**
      * Returns an enumerator as a string.
      * @param e an enumerator of AntennaMotionPatternMod::AntennaMotionPattern.
      * @return a string.
      */
	static std::string name(const AntennaMotionPatternMod::AntennaMotionPattern& e);
	
	/**
	  * Equivalent to the name method.
	  */
    static std::string toString(const AntennaMotionPatternMod::AntennaMotionPattern& f) { return name(f); }

	/** 
	  * Returns vector of  all the enumerators as strings. 
	  * The strings are stored in the vector in the same order than the enumerators are declared in the enumeration. 
	  * @return a vector of string.
	  */
     static const std::vector<std::string> names();	 
    
   	
   	// Create a AntennaMotionPattern enumeration object by specifying its name.
   	static AntennaMotionPatternMod::AntennaMotionPattern newAntennaMotionPattern(const std::string& name);
   	
   	/*! Return a AntennaMotionPattern's enumerator  given a string.
   	  * @param name the string representation of the enumerator.
   	 *  @return a AntennaMotionPatternMod::AntennaMotionPattern's enumerator.
   	 *  @throws a string containing an error message if no enumerator could be found for this name.
   	 */
 	static AntennaMotionPatternMod::AntennaMotionPattern literal(const std::string& name);
 	
    /*! Return a AntennaMotionPattern's enumerator given an unsigned int.
      * @param i the index of the enumerator in AntennaMotionPatternMod::AntennaMotionPattern.
      * @return a AntennaMotionPatternMod::AntennaMotionPattern's enumerator.
      * @throws a string containing an error message if no enumerator could be found for this integer.
      */
 	static AntennaMotionPatternMod::AntennaMotionPattern from_int(unsigned int i);	
 	

  private:
    /* Not Implemented.  This is a pure static class. */
    CAntennaMotionPattern();
    CAntennaMotionPattern(const CAntennaMotionPattern&);
    CAntennaMotionPattern& operator=(const CAntennaMotionPattern&);
    
    static std::string badString(const std::string& name) ;
  	static std::string badInt(unsigned int i) ;
  	
};
 
#endif /*!CAntennaMotionPattern_H*/
