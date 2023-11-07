
#ifndef CACAPolarization_H
#define CACAPolarization_H

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
 * File CACAPolarization.h
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#include <iostream>
#include <string>
#include <vector>
/**
  * A namespace to encapsulate the ACAPolarization enumeration.
  */
#ifndef WITHOUT_ACS
#include <almaEnumerations_IFC.h>
#else

// This part mimics the behaviour of 
namespace ACAPolarizationMod
{
  //! ACAPolarization.
  //!  ACA-specific ways to store pre-processed data products
  
  const char *const revision = "-1";
  const int version = 1;
  
  enum ACAPolarization
  { 
    ACA_STANDARD /*!< Data product is the standard way (it is a standard observed Stokes parameter) */
     ,
    ACA_XX_YY_SUM /*!< ACA has calculated I by averaging XX and YY */
     ,
    ACA_XX_50 /*!< ACA has averaged XX and XX delayed by half a FFT period */
     ,
    ACA_YY_50 /*!< ACA has averaged YY and YY delayed by half a FFT period */
     
  };
  typedef ACAPolarization &ACAPolarization_out;
} 
#endif

namespace ACAPolarizationMod {
	std::ostream & operator << ( std::ostream & out, const ACAPolarization& value);
	std::istream & operator >> ( std::istream & in , ACAPolarization& value );
}

/** 
  * A helper class for the enumeration ACAPolarization.
  * 
  */
class CACAPolarization {
  public:
 
	/**
	  * Enumerators as strings.
	  */  
	
	static const std::string& sACA_STANDARD; /*!< A const string equal to "ACA_STANDARD".*/
	
	static const std::string& sACA_XX_YY_SUM; /*!< A const string equal to "ACA_XX_YY_SUM".*/
	
	static const std::string& sACA_XX_50; /*!< A const string equal to "ACA_XX_50".*/
	
	static const std::string& sACA_YY_50; /*!< A const string equal to "ACA_YY_50".*/
	

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
       * Return the number of enumerators declared in ACAPolarizationMod::ACAPolarization.
       * @return an unsigned int.
       */
       static unsigned int size() ;
       
       
    /**
      * Returns an enumerator as a string.
      * @param e an enumerator of ACAPolarizationMod::ACAPolarization.
      * @return a string.
      */
	static std::string name(const ACAPolarizationMod::ACAPolarization& e);
	
	/**
	  * Equivalent to the name method.
	  */
    static std::string toString(const ACAPolarizationMod::ACAPolarization& f) { return name(f); }

	/** 
	  * Returns vector of  all the enumerators as strings. 
	  * The strings are stored in the vector in the same order than the enumerators are declared in the enumeration. 
	  * @return a vector of string.
	  */
     static const std::vector<std::string> names();	 
    
   	
   	// Create a ACAPolarization enumeration object by specifying its name.
   	static ACAPolarizationMod::ACAPolarization newACAPolarization(const std::string& name);
   	
   	/*! Return a ACAPolarization's enumerator  given a string.
   	  * @param name the string representation of the enumerator.
   	 *  @return a ACAPolarizationMod::ACAPolarization's enumerator.
   	 *  @throws a string containing an error message if no enumerator could be found for this name.
   	 */
 	static ACAPolarizationMod::ACAPolarization literal(const std::string& name);
 	
    /*! Return a ACAPolarization's enumerator given an unsigned int.
      * @param i the index of the enumerator in ACAPolarizationMod::ACAPolarization.
      * @return a ACAPolarizationMod::ACAPolarization's enumerator.
      * @throws a string containing an error message if no enumerator could be found for this integer.
      */
 	static ACAPolarizationMod::ACAPolarization from_int(unsigned int i);	
 	

  private:
    /* Not Implemented.  This is a pure static class. */
    CACAPolarization();
    CACAPolarization(const CACAPolarization&);
    CACAPolarization& operator=(const CACAPolarization&);
    
    static std::string badString(const std::string& name) ;
  	static std::string badInt(unsigned int i) ;
  	
};
 
#endif /*!CACAPolarization_H*/
