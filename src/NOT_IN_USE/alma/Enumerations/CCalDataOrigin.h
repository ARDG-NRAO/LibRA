
#ifndef CCalDataOrigin_H
#define CCalDataOrigin_H

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
 * File CCalDataOrigin.h
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#include <iostream>
#include <string>
#include <vector>
/**
  * A namespace to encapsulate the CalDataOrigin enumeration.
  */
#ifndef WITHOUT_ACS
#include <almaEnumerations_IFC.h>
#else

// This part mimics the behaviour of 
namespace CalDataOriginMod
{
  //! CalDataOrigin.
  //! 
  
  const char *const revision = "-1";
  const int version = 1;
  
  enum CalDataOrigin
  { 
    TOTAL_POWER /*!< Total Power data (from detectors) */
     ,
    WVR /*!< Water vapour radiometrers */
     ,
    CHANNEL_AVERAGE_AUTO /*!< Autocorrelations from channel average data */
     ,
    CHANNEL_AVERAGE_CROSS /*!< Crosscorrelations from channel average data */
     ,
    FULL_RESOLUTION_AUTO /*!< Autocorrelations from full-resolution data */
     ,
    FULL_RESOLUTION_CROSS /*!< Cross correlations from full-resolution data */
     ,
    OPTICAL_POINTING /*!< Optical pointing data */
     ,
    HOLOGRAPHY /*!< data from holography receivers */
     ,
    NONE /*!< Not applicable */
     
  };
  typedef CalDataOrigin &CalDataOrigin_out;
} 
#endif

namespace CalDataOriginMod {
	std::ostream & operator << ( std::ostream & out, const CalDataOrigin& value);
	std::istream & operator >> ( std::istream & in , CalDataOrigin& value );
}

/** 
  * A helper class for the enumeration CalDataOrigin.
  * 
  */
class CCalDataOrigin {
  public:
 
	/**
	  * Enumerators as strings.
	  */  
	
	static const std::string& sTOTAL_POWER; /*!< A const string equal to "TOTAL_POWER".*/
	
	static const std::string& sWVR; /*!< A const string equal to "WVR".*/
	
	static const std::string& sCHANNEL_AVERAGE_AUTO; /*!< A const string equal to "CHANNEL_AVERAGE_AUTO".*/
	
	static const std::string& sCHANNEL_AVERAGE_CROSS; /*!< A const string equal to "CHANNEL_AVERAGE_CROSS".*/
	
	static const std::string& sFULL_RESOLUTION_AUTO; /*!< A const string equal to "FULL_RESOLUTION_AUTO".*/
	
	static const std::string& sFULL_RESOLUTION_CROSS; /*!< A const string equal to "FULL_RESOLUTION_CROSS".*/
	
	static const std::string& sOPTICAL_POINTING; /*!< A const string equal to "OPTICAL_POINTING".*/
	
	static const std::string& sHOLOGRAPHY; /*!< A const string equal to "HOLOGRAPHY".*/
	
	static const std::string& sNONE; /*!< A const string equal to "NONE".*/
	

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
       * Return the number of enumerators declared in CalDataOriginMod::CalDataOrigin.
       * @return an unsigned int.
       */
       static unsigned int size() ;
       
       
    /**
      * Returns an enumerator as a string.
      * @param e an enumerator of CalDataOriginMod::CalDataOrigin.
      * @return a string.
      */
	static std::string name(const CalDataOriginMod::CalDataOrigin& e);
	
	/**
	  * Equivalent to the name method.
	  */
    static std::string toString(const CalDataOriginMod::CalDataOrigin& f) { return name(f); }

	/** 
	  * Returns vector of  all the enumerators as strings. 
	  * The strings are stored in the vector in the same order than the enumerators are declared in the enumeration. 
	  * @return a vector of string.
	  */
     static const std::vector<std::string> names();	 
    
   	
   	// Create a CalDataOrigin enumeration object by specifying its name.
   	static CalDataOriginMod::CalDataOrigin newCalDataOrigin(const std::string& name);
   	
   	/*! Return a CalDataOrigin's enumerator  given a string.
   	  * @param name the string representation of the enumerator.
   	 *  @return a CalDataOriginMod::CalDataOrigin's enumerator.
   	 *  @throws a string containing an error message if no enumerator could be found for this name.
   	 */
 	static CalDataOriginMod::CalDataOrigin literal(const std::string& name);
 	
    /*! Return a CalDataOrigin's enumerator given an unsigned int.
      * @param i the index of the enumerator in CalDataOriginMod::CalDataOrigin.
      * @return a CalDataOriginMod::CalDataOrigin's enumerator.
      * @throws a string containing an error message if no enumerator could be found for this integer.
      */
 	static CalDataOriginMod::CalDataOrigin from_int(unsigned int i);	
 	

  private:
    /* Not Implemented.  This is a pure static class. */
    CCalDataOrigin();
    CCalDataOrigin(const CCalDataOrigin&);
    CCalDataOrigin& operator=(const CCalDataOrigin&);
    
    static std::string badString(const std::string& name) ;
  	static std::string badInt(unsigned int i) ;
  	
};
 
#endif /*!CCalDataOrigin_H*/
