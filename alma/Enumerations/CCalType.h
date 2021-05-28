
#ifndef CCalType_H
#define CCalType_H

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
 * File CCalType.h
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#include <iostream>
#include <string>
#include <vector>
/**
  * A namespace to encapsulate the CalType enumeration.
  */
#ifndef WITHOUT_ACS
#include <almaEnumerations_IFC.h>
#else

// This part mimics the behaviour of 
namespace CalTypeMod
{
  //! CalType.
  //!  [CalDM.CalData] Used to point to a given CalResult table
  
  const char *const revision = "-1";
  const int version = 1;
  
  enum CalType
  { 
    CAL_AMPLI /*!<  */
     ,
    CAL_ATMOSPHERE /*!<  */
     ,
    CAL_BANDPASS /*!<  */
     ,
    CAL_CURVE /*!<  */
     ,
    CAL_DELAY /*!<  */
     ,
    CAL_FLUX /*!<  */
     ,
    CAL_FOCUS /*!<  */
     ,
    CAL_FOCUS_MODEL /*!<  */
     ,
    CAL_GAIN /*!<  */
     ,
    CAL_HOLOGRAPHY /*!<  */
     ,
    CAL_PHASE /*!<  */
     ,
    CAL_POINTING /*!<  */
     ,
    CAL_POINTING_MODEL /*!<  */
     ,
    CAL_POSITION /*!<  */
     ,
    CAL_PRIMARY_BEAM /*!<  */
     ,
    CAL_SEEING /*!<  */
     ,
    CAL_WVR /*!<  */
     ,
    CAL_APPPHASE /*!< Calibration for phasing of ALMA. Applicable at ALMA. */
     
  };
  typedef CalType &CalType_out;
} 
#endif

namespace CalTypeMod {
	std::ostream & operator << ( std::ostream & out, const CalType& value);
	std::istream & operator >> ( std::istream & in , CalType& value );
}

/** 
  * A helper class for the enumeration CalType.
  * 
  */
class CCalType {
  public:
 
	/**
	  * Enumerators as strings.
	  */  
	
	static const std::string& sCAL_AMPLI; /*!< A const string equal to "CAL_AMPLI".*/
	
	static const std::string& sCAL_ATMOSPHERE; /*!< A const string equal to "CAL_ATMOSPHERE".*/
	
	static const std::string& sCAL_BANDPASS; /*!< A const string equal to "CAL_BANDPASS".*/
	
	static const std::string& sCAL_CURVE; /*!< A const string equal to "CAL_CURVE".*/
	
	static const std::string& sCAL_DELAY; /*!< A const string equal to "CAL_DELAY".*/
	
	static const std::string& sCAL_FLUX; /*!< A const string equal to "CAL_FLUX".*/
	
	static const std::string& sCAL_FOCUS; /*!< A const string equal to "CAL_FOCUS".*/
	
	static const std::string& sCAL_FOCUS_MODEL; /*!< A const string equal to "CAL_FOCUS_MODEL".*/
	
	static const std::string& sCAL_GAIN; /*!< A const string equal to "CAL_GAIN".*/
	
	static const std::string& sCAL_HOLOGRAPHY; /*!< A const string equal to "CAL_HOLOGRAPHY".*/
	
	static const std::string& sCAL_PHASE; /*!< A const string equal to "CAL_PHASE".*/
	
	static const std::string& sCAL_POINTING; /*!< A const string equal to "CAL_POINTING".*/
	
	static const std::string& sCAL_POINTING_MODEL; /*!< A const string equal to "CAL_POINTING_MODEL".*/
	
	static const std::string& sCAL_POSITION; /*!< A const string equal to "CAL_POSITION".*/
	
	static const std::string& sCAL_PRIMARY_BEAM; /*!< A const string equal to "CAL_PRIMARY_BEAM".*/
	
	static const std::string& sCAL_SEEING; /*!< A const string equal to "CAL_SEEING".*/
	
	static const std::string& sCAL_WVR; /*!< A const string equal to "CAL_WVR".*/
	
	static const std::string& sCAL_APPPHASE; /*!< A const string equal to "CAL_APPPHASE".*/
	

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
       * Return the number of enumerators declared in CalTypeMod::CalType.
       * @return an unsigned int.
       */
       static unsigned int size() ;
       
       
    /**
      * Returns an enumerator as a string.
      * @param e an enumerator of CalTypeMod::CalType.
      * @return a string.
      */
	static std::string name(const CalTypeMod::CalType& e);
	
	/**
	  * Equivalent to the name method.
	  */
    static std::string toString(const CalTypeMod::CalType& f) { return name(f); }

	/** 
	  * Returns vector of  all the enumerators as strings. 
	  * The strings are stored in the vector in the same order than the enumerators are declared in the enumeration. 
	  * @return a vector of string.
	  */
     static const std::vector<std::string> names();	 
    
   	
   	// Create a CalType enumeration object by specifying its name.
   	static CalTypeMod::CalType newCalType(const std::string& name);
   	
   	/*! Return a CalType's enumerator  given a string.
   	  * @param name the string representation of the enumerator.
   	 *  @return a CalTypeMod::CalType's enumerator.
   	 *  @throws a string containing an error message if no enumerator could be found for this name.
   	 */
 	static CalTypeMod::CalType literal(const std::string& name);
 	
    /*! Return a CalType's enumerator given an unsigned int.
      * @param i the index of the enumerator in CalTypeMod::CalType.
      * @return a CalTypeMod::CalType's enumerator.
      * @throws a string containing an error message if no enumerator could be found for this integer.
      */
 	static CalTypeMod::CalType from_int(unsigned int i);	
 	

  private:
    /* Not Implemented.  This is a pure static class. */
    CCalType();
    CCalType(const CCalType&);
    CCalType& operator=(const CCalType&);
    
    static std::string badString(const std::string& name) ;
  	static std::string badInt(unsigned int i) ;
  	
};
 
#endif /*!CCalType_H*/
