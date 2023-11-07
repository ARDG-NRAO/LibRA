
#ifndef CCorrelatorType_H
#define CCorrelatorType_H

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
 * File CCorrelatorType.h
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#include <iostream>
#include <string>
#include <vector>
/**
  * A namespace to encapsulate the CorrelatorType enumeration.
  */
#ifndef WITHOUT_ACS
#include <almaEnumerations_IFC.h>
#else

// This part mimics the behaviour of 
namespace CorrelatorTypeMod
{
  //! CorrelatorType.
  //! defines the type of a correlator.
  
  const char *const revision = "-1";
  const int version = 1;
  
  enum CorrelatorType
  { 
    FX /*!< identifies a digital correlator of type FX. */
     ,
    XF /*!< identifies a digital correlator of type XF. */
     ,
    FXF /*!< identifies a correlator of type FXF. */
     
  };
  typedef CorrelatorType &CorrelatorType_out;
} 
#endif

namespace CorrelatorTypeMod {
	std::ostream & operator << ( std::ostream & out, const CorrelatorType& value);
	std::istream & operator >> ( std::istream & in , CorrelatorType& value );
}

/** 
  * A helper class for the enumeration CorrelatorType.
  * 
  */
class CCorrelatorType {
  public:
 
	/**
	  * Enumerators as strings.
	  */  
	
	static const std::string& sFX; /*!< A const string equal to "FX".*/
	
	static const std::string& sXF; /*!< A const string equal to "XF".*/
	
	static const std::string& sFXF; /*!< A const string equal to "FXF".*/
	

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
       * Return the number of enumerators declared in CorrelatorTypeMod::CorrelatorType.
       * @return an unsigned int.
       */
       static unsigned int size() ;
       
       
    /**
      * Returns an enumerator as a string.
      * @param e an enumerator of CorrelatorTypeMod::CorrelatorType.
      * @return a string.
      */
	static std::string name(const CorrelatorTypeMod::CorrelatorType& e);
	
	/**
	  * Equivalent to the name method.
	  */
    static std::string toString(const CorrelatorTypeMod::CorrelatorType& f) { return name(f); }

	/** 
	  * Returns vector of  all the enumerators as strings. 
	  * The strings are stored in the vector in the same order than the enumerators are declared in the enumeration. 
	  * @return a vector of string.
	  */
     static const std::vector<std::string> names();	 
    
   	
   	// Create a CorrelatorType enumeration object by specifying its name.
   	static CorrelatorTypeMod::CorrelatorType newCorrelatorType(const std::string& name);
   	
   	/*! Return a CorrelatorType's enumerator  given a string.
   	  * @param name the string representation of the enumerator.
   	 *  @return a CorrelatorTypeMod::CorrelatorType's enumerator.
   	 *  @throws a string containing an error message if no enumerator could be found for this name.
   	 */
 	static CorrelatorTypeMod::CorrelatorType literal(const std::string& name);
 	
    /*! Return a CorrelatorType's enumerator given an unsigned int.
      * @param i the index of the enumerator in CorrelatorTypeMod::CorrelatorType.
      * @return a CorrelatorTypeMod::CorrelatorType's enumerator.
      * @throws a string containing an error message if no enumerator could be found for this integer.
      */
 	static CorrelatorTypeMod::CorrelatorType from_int(unsigned int i);	
 	

  private:
    /* Not Implemented.  This is a pure static class. */
    CCorrelatorType();
    CCorrelatorType(const CCorrelatorType&);
    CCorrelatorType& operator=(const CCorrelatorType&);
    
    static std::string badString(const std::string& name) ;
  	static std::string badInt(unsigned int i) ;
  	
};
 
#endif /*!CCorrelatorType_H*/
