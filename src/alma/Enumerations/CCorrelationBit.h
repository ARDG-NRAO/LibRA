
#ifndef CCorrelationBit_H
#define CCorrelationBit_H

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
 * File CCorrelationBit.h
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#include <iostream>
#include <string>
#include <vector>
/**
  * A namespace to encapsulate the CorrelationBit enumeration.
  */
#ifndef WITHOUT_ACS
#include <almaEnumerations_IFC.h>
#else

// This part mimics the behaviour of 
namespace CorrelationBitMod
{
  //! CorrelationBit.
  //!  [APDM] Number of bits used for correlation
  
  const char *const revision = "-1";
  const int version = 1;
  
  enum CorrelationBit
  { 
    BITS_2x2 /*!< two bit correlation */
     ,
    BITS_3x3 /*!<  three bit correlation */
     ,
    BITS_4x4 /*!< four bit correlation */
     
  };
  typedef CorrelationBit &CorrelationBit_out;
} 
#endif

namespace CorrelationBitMod {
	std::ostream & operator << ( std::ostream & out, const CorrelationBit& value);
	std::istream & operator >> ( std::istream & in , CorrelationBit& value );
}

/** 
  * A helper class for the enumeration CorrelationBit.
  * 
  */
class CCorrelationBit {
  public:
 
	/**
	  * Enumerators as strings.
	  */  
	
	static const std::string& sBITS_2x2; /*!< A const string equal to "BITS_2x2".*/
	
	static const std::string& sBITS_3x3; /*!< A const string equal to "BITS_3x3".*/
	
	static const std::string& sBITS_4x4; /*!< A const string equal to "BITS_4x4".*/
	

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
       * Return the number of enumerators declared in CorrelationBitMod::CorrelationBit.
       * @return an unsigned int.
       */
       static unsigned int size() ;
       
       
    /**
      * Returns an enumerator as a string.
      * @param e an enumerator of CorrelationBitMod::CorrelationBit.
      * @return a string.
      */
	static std::string name(const CorrelationBitMod::CorrelationBit& e);
	
	/**
	  * Equivalent to the name method.
	  */
    static std::string toString(const CorrelationBitMod::CorrelationBit& f) { return name(f); }

	/** 
	  * Returns vector of  all the enumerators as strings. 
	  * The strings are stored in the vector in the same order than the enumerators are declared in the enumeration. 
	  * @return a vector of string.
	  */
     static const std::vector<std::string> names();	 
    
   	
   	// Create a CorrelationBit enumeration object by specifying its name.
   	static CorrelationBitMod::CorrelationBit newCorrelationBit(const std::string& name);
   	
   	/*! Return a CorrelationBit's enumerator  given a string.
   	  * @param name the string representation of the enumerator.
   	 *  @return a CorrelationBitMod::CorrelationBit's enumerator.
   	 *  @throws a string containing an error message if no enumerator could be found for this name.
   	 */
 	static CorrelationBitMod::CorrelationBit literal(const std::string& name);
 	
    /*! Return a CorrelationBit's enumerator given an unsigned int.
      * @param i the index of the enumerator in CorrelationBitMod::CorrelationBit.
      * @return a CorrelationBitMod::CorrelationBit's enumerator.
      * @throws a string containing an error message if no enumerator could be found for this integer.
      */
 	static CorrelationBitMod::CorrelationBit from_int(unsigned int i);	
 	

  private:
    /* Not Implemented.  This is a pure static class. */
    CCorrelationBit();
    CCorrelationBit(const CCorrelationBit&);
    CCorrelationBit& operator=(const CCorrelationBit&);
    
    static std::string badString(const std::string& name) ;
  	static std::string badInt(unsigned int i) ;
  	
};
 
#endif /*!CCorrelationBit_H*/
