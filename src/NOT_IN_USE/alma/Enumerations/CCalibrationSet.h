
#ifndef CCalibrationSet_H
#define CCalibrationSet_H

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
 * File CCalibrationSet.h
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#include <iostream>
#include <string>
#include <vector>
/**
  * A namespace to encapsulate the CalibrationSet enumeration.
  */
#ifndef WITHOUT_ACS
#include <almaEnumerations_IFC.h>
#else

// This part mimics the behaviour of 
namespace CalibrationSetMod
{
  //! CalibrationSet.
  //! Defines sets of calibration scans to be reduced together for a result.
  
  const char *const revision = "-1";
  const int version = 1;
  
  enum CalibrationSet
  { 
    NONE /*!< Scan is not part of a calibration set. */
     ,
    AMPLI_CURVE /*!< Amplitude calibration scan (calibration curve to be derived). */
     ,
    ANTENNA_POSITIONS /*!< Antenna positions measurement. */
     ,
    PHASE_CURVE /*!< Phase calibration scan (calibration curve to be derived). */
     ,
    POINTING_MODEL /*!< Pointing calibration scan (pointing model to be derived). */
     ,
    ACCUMULATE /*!< Accumulate a scan in a calibration set. */
     ,
    TEST /*!< Reserved for development. */
     ,
    UNSPECIFIED /*!< Unspecified calibration intent. */
     
  };
  typedef CalibrationSet &CalibrationSet_out;
} 
#endif

namespace CalibrationSetMod {
	std::ostream & operator << ( std::ostream & out, const CalibrationSet& value);
	std::istream & operator >> ( std::istream & in , CalibrationSet& value );
}

/** 
  * A helper class for the enumeration CalibrationSet.
  * 
  */
class CCalibrationSet {
  public:
 
	/**
	  * Enumerators as strings.
	  */  
	
	static const std::string& sNONE; /*!< A const string equal to "NONE".*/
	
	static const std::string& sAMPLI_CURVE; /*!< A const string equal to "AMPLI_CURVE".*/
	
	static const std::string& sANTENNA_POSITIONS; /*!< A const string equal to "ANTENNA_POSITIONS".*/
	
	static const std::string& sPHASE_CURVE; /*!< A const string equal to "PHASE_CURVE".*/
	
	static const std::string& sPOINTING_MODEL; /*!< A const string equal to "POINTING_MODEL".*/
	
	static const std::string& sACCUMULATE; /*!< A const string equal to "ACCUMULATE".*/
	
	static const std::string& sTEST; /*!< A const string equal to "TEST".*/
	
	static const std::string& sUNSPECIFIED; /*!< A const string equal to "UNSPECIFIED".*/
	

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
       * Return the number of enumerators declared in CalibrationSetMod::CalibrationSet.
       * @return an unsigned int.
       */
       static unsigned int size() ;
       
       
    /**
      * Returns an enumerator as a string.
      * @param e an enumerator of CalibrationSetMod::CalibrationSet.
      * @return a string.
      */
	static std::string name(const CalibrationSetMod::CalibrationSet& e);
	
	/**
	  * Equivalent to the name method.
	  */
    static std::string toString(const CalibrationSetMod::CalibrationSet& f) { return name(f); }

	/** 
	  * Returns vector of  all the enumerators as strings. 
	  * The strings are stored in the vector in the same order than the enumerators are declared in the enumeration. 
	  * @return a vector of string.
	  */
     static const std::vector<std::string> names();	 
    
   	
   	// Create a CalibrationSet enumeration object by specifying its name.
   	static CalibrationSetMod::CalibrationSet newCalibrationSet(const std::string& name);
   	
   	/*! Return a CalibrationSet's enumerator  given a string.
   	  * @param name the string representation of the enumerator.
   	 *  @return a CalibrationSetMod::CalibrationSet's enumerator.
   	 *  @throws a string containing an error message if no enumerator could be found for this name.
   	 */
 	static CalibrationSetMod::CalibrationSet literal(const std::string& name);
 	
    /*! Return a CalibrationSet's enumerator given an unsigned int.
      * @param i the index of the enumerator in CalibrationSetMod::CalibrationSet.
      * @return a CalibrationSetMod::CalibrationSet's enumerator.
      * @throws a string containing an error message if no enumerator could be found for this integer.
      */
 	static CalibrationSetMod::CalibrationSet from_int(unsigned int i);	
 	

  private:
    /* Not Implemented.  This is a pure static class. */
    CCalibrationSet();
    CCalibrationSet(const CCalibrationSet&);
    CCalibrationSet& operator=(const CCalibrationSet&);
    
    static std::string badString(const std::string& name) ;
  	static std::string badInt(unsigned int i) ;
  	
};
 
#endif /*!CCalibrationSet_H*/
