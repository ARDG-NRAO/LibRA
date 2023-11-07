
#ifndef CPrimitiveDataType_H
#define CPrimitiveDataType_H

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
 * File CPrimitiveDataType.h
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#include <iostream>
#include <string>
#include <vector>
/**
  * A namespace to encapsulate the PrimitiveDataType enumeration.
  */
#ifndef WITHOUT_ACS
#include <almaEnumerations_IFC.h>
#else

// This part mimics the behaviour of 
namespace PrimitiveDataTypeMod
{
  //! PrimitiveDataType.
  //!  [ASDM.Binaries] Primitive data types for binary MIME attachments
  
  const char *const revision = "-1";
  const int version = 1;
  
  enum PrimitiveDataType
  { 
    INT16_TYPE /*!< 2 bytes signed integer (short). */
     ,
    INT32_TYPE /*!< 4 bytes signed integer (int). */
     ,
    INT64_TYPE /*!< 8 bytes signed integer (long long). */
     ,
    FLOAT32_TYPE /*!< 4 bytes float (float). */
     ,
    FLOAT64_TYPE /*!< 8 bytes float (double). */
     
  };
  typedef PrimitiveDataType &PrimitiveDataType_out;
} 
#endif

namespace PrimitiveDataTypeMod {
	std::ostream & operator << ( std::ostream & out, const PrimitiveDataType& value);
	std::istream & operator >> ( std::istream & in , PrimitiveDataType& value );
}

/** 
  * A helper class for the enumeration PrimitiveDataType.
  * 
  */
class CPrimitiveDataType {
  public:
 
	/**
	  * Enumerators as strings.
	  */  
	
	static const std::string& sINT16_TYPE; /*!< A const string equal to "INT16_TYPE".*/
	
	static const std::string& sINT32_TYPE; /*!< A const string equal to "INT32_TYPE".*/
	
	static const std::string& sINT64_TYPE; /*!< A const string equal to "INT64_TYPE".*/
	
	static const std::string& sFLOAT32_TYPE; /*!< A const string equal to "FLOAT32_TYPE".*/
	
	static const std::string& sFLOAT64_TYPE; /*!< A const string equal to "FLOAT64_TYPE".*/
	

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
       * Return the number of enumerators declared in PrimitiveDataTypeMod::PrimitiveDataType.
       * @return an unsigned int.
       */
       static unsigned int size() ;
       
       
    /**
      * Returns an enumerator as a string.
      * @param e an enumerator of PrimitiveDataTypeMod::PrimitiveDataType.
      * @return a string.
      */
	static std::string name(const PrimitiveDataTypeMod::PrimitiveDataType& e);
	
	/**
	  * Equivalent to the name method.
	  */
    static std::string toString(const PrimitiveDataTypeMod::PrimitiveDataType& f) { return name(f); }

	/** 
	  * Returns vector of  all the enumerators as strings. 
	  * The strings are stored in the vector in the same order than the enumerators are declared in the enumeration. 
	  * @return a vector of string.
	  */
     static const std::vector<std::string> names();	 
    
   	
   	// Create a PrimitiveDataType enumeration object by specifying its name.
   	static PrimitiveDataTypeMod::PrimitiveDataType newPrimitiveDataType(const std::string& name);
   	
   	/*! Return a PrimitiveDataType's enumerator  given a string.
   	  * @param name the string representation of the enumerator.
   	 *  @return a PrimitiveDataTypeMod::PrimitiveDataType's enumerator.
   	 *  @throws a string containing an error message if no enumerator could be found for this name.
   	 */
 	static PrimitiveDataTypeMod::PrimitiveDataType literal(const std::string& name);
 	
    /*! Return a PrimitiveDataType's enumerator given an unsigned int.
      * @param i the index of the enumerator in PrimitiveDataTypeMod::PrimitiveDataType.
      * @return a PrimitiveDataTypeMod::PrimitiveDataType's enumerator.
      * @throws a string containing an error message if no enumerator could be found for this integer.
      */
 	static PrimitiveDataTypeMod::PrimitiveDataType from_int(unsigned int i);	
 	

  private:
    /* Not Implemented.  This is a pure static class. */
    CPrimitiveDataType();
    CPrimitiveDataType(const CPrimitiveDataType&);
    CPrimitiveDataType& operator=(const CPrimitiveDataType&);
    
    static std::string badString(const std::string& name) ;
  	static std::string badInt(unsigned int i) ;
  	
};
 
#endif /*!CPrimitiveDataType_H*/
