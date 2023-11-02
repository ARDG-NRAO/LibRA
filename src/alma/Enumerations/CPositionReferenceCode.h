
#ifndef CPositionReferenceCode_H
#define CPositionReferenceCode_H

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
 * File CPositionReferenceCode.h
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#include <iostream>
#include <string>
#include <vector>
/**
  * A namespace to encapsulate the PositionReferenceCode enumeration.
  */
#ifndef WITHOUT_ACS
#include <almaEnumerations_IFC.h>
#else

// This part mimics the behaviour of 
namespace PositionReferenceCodeMod
{
  //! PositionReferenceCode.
  //! defines reference frames to qualify the measure of a position.

  
  const char *const revision = "-1";
  const int version = 1;
  
  enum PositionReferenceCode
  { 
    ITRF /*!< International Terrestrial Reference Frame. */
     ,
    WGS84 /*!< World Geodetic System. */
     ,
    SITE /*!< Site reference coordinate system (ALMA-80.05.00.00-009-B-SPE). */
     ,
    STATION /*!< Antenna station reference coordinate system (ALMA-80.05.00.00-009-SPE). */
     ,
    YOKE /*!< Antenna yoke reference coordinate system (ALMA-980.05.00.00-009-B-SPE) */
     ,
    REFLECTOR /*!< Antenna reflector reference coordinate system (ALMA-80.05.00.00-009-B-SPE). */
     
  };
  typedef PositionReferenceCode &PositionReferenceCode_out;
} 
#endif

namespace PositionReferenceCodeMod {
	std::ostream & operator << ( std::ostream & out, const PositionReferenceCode& value);
	std::istream & operator >> ( std::istream & in , PositionReferenceCode& value );
}

/** 
  * A helper class for the enumeration PositionReferenceCode.
  * 
  */
class CPositionReferenceCode {
  public:
 
	/**
	  * Enumerators as strings.
	  */  
	
	static const std::string& sITRF; /*!< A const string equal to "ITRF".*/
	
	static const std::string& sWGS84; /*!< A const string equal to "WGS84".*/
	
	static const std::string& sSITE; /*!< A const string equal to "SITE".*/
	
	static const std::string& sSTATION; /*!< A const string equal to "STATION".*/
	
	static const std::string& sYOKE; /*!< A const string equal to "YOKE".*/
	
	static const std::string& sREFLECTOR; /*!< A const string equal to "REFLECTOR".*/
	

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
       * Return the number of enumerators declared in PositionReferenceCodeMod::PositionReferenceCode.
       * @return an unsigned int.
       */
       static unsigned int size() ;
       
       
    /**
      * Returns an enumerator as a string.
      * @param e an enumerator of PositionReferenceCodeMod::PositionReferenceCode.
      * @return a string.
      */
	static std::string name(const PositionReferenceCodeMod::PositionReferenceCode& e);
	
	/**
	  * Equivalent to the name method.
	  */
    static std::string toString(const PositionReferenceCodeMod::PositionReferenceCode& f) { return name(f); }

	/** 
	  * Returns vector of  all the enumerators as strings. 
	  * The strings are stored in the vector in the same order than the enumerators are declared in the enumeration. 
	  * @return a vector of string.
	  */
     static const std::vector<std::string> names();	 
    
   	
   	// Create a PositionReferenceCode enumeration object by specifying its name.
   	static PositionReferenceCodeMod::PositionReferenceCode newPositionReferenceCode(const std::string& name);
   	
   	/*! Return a PositionReferenceCode's enumerator  given a string.
   	  * @param name the string representation of the enumerator.
   	 *  @return a PositionReferenceCodeMod::PositionReferenceCode's enumerator.
   	 *  @throws a string containing an error message if no enumerator could be found for this name.
   	 */
 	static PositionReferenceCodeMod::PositionReferenceCode literal(const std::string& name);
 	
    /*! Return a PositionReferenceCode's enumerator given an unsigned int.
      * @param i the index of the enumerator in PositionReferenceCodeMod::PositionReferenceCode.
      * @return a PositionReferenceCodeMod::PositionReferenceCode's enumerator.
      * @throws a string containing an error message if no enumerator could be found for this integer.
      */
 	static PositionReferenceCodeMod::PositionReferenceCode from_int(unsigned int i);	
 	

  private:
    /* Not Implemented.  This is a pure static class. */
    CPositionReferenceCode();
    CPositionReferenceCode(const CPositionReferenceCode&);
    CPositionReferenceCode& operator=(const CPositionReferenceCode&);
    
    static std::string badString(const std::string& name) ;
  	static std::string badInt(unsigned int i) ;
  	
};
 
#endif /*!CPositionReferenceCode_H*/
