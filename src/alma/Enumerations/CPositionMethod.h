
#ifndef CPositionMethod_H
#define CPositionMethod_H

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
 * File CPositionMethod.h
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#include <iostream>
#include <string>
#include <vector>
/**
  * A namespace to encapsulate the PositionMethod enumeration.
  */
#ifndef WITHOUT_ACS
#include <almaEnumerations_IFC.h>
#else

// This part mimics the behaviour of 
namespace PositionMethodMod
{
  //! PositionMethod.
  //!  [CalDM.CalPositions] Method used for measuring antenna positions
  
  const char *const revision = "-1";
  const int version = 1;
  
  enum PositionMethod
  { 
    DELAY_FITTING /*!< Delays are measured for each source; the delays are used for fitting antenna position errors. */
     ,
    PHASE_FITTING /*!< Phases are measured for each source; these phases are used to fit antenna position errors. */
     
  };
  typedef PositionMethod &PositionMethod_out;
} 
#endif

namespace PositionMethodMod {
	std::ostream & operator << ( std::ostream & out, const PositionMethod& value);
	std::istream & operator >> ( std::istream & in , PositionMethod& value );
}

/** 
  * A helper class for the enumeration PositionMethod.
  * 
  */
class CPositionMethod {
  public:
 
	/**
	  * Enumerators as strings.
	  */  
	
	static const std::string& sDELAY_FITTING; /*!< A const string equal to "DELAY_FITTING".*/
	
	static const std::string& sPHASE_FITTING; /*!< A const string equal to "PHASE_FITTING".*/
	

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
       * Return the number of enumerators declared in PositionMethodMod::PositionMethod.
       * @return an unsigned int.
       */
       static unsigned int size() ;
       
       
    /**
      * Returns an enumerator as a string.
      * @param e an enumerator of PositionMethodMod::PositionMethod.
      * @return a string.
      */
	static std::string name(const PositionMethodMod::PositionMethod& e);
	
	/**
	  * Equivalent to the name method.
	  */
    static std::string toString(const PositionMethodMod::PositionMethod& f) { return name(f); }

	/** 
	  * Returns vector of  all the enumerators as strings. 
	  * The strings are stored in the vector in the same order than the enumerators are declared in the enumeration. 
	  * @return a vector of string.
	  */
     static const std::vector<std::string> names();	 
    
   	
   	// Create a PositionMethod enumeration object by specifying its name.
   	static PositionMethodMod::PositionMethod newPositionMethod(const std::string& name);
   	
   	/*! Return a PositionMethod's enumerator  given a string.
   	  * @param name the string representation of the enumerator.
   	 *  @return a PositionMethodMod::PositionMethod's enumerator.
   	 *  @throws a string containing an error message if no enumerator could be found for this name.
   	 */
 	static PositionMethodMod::PositionMethod literal(const std::string& name);
 	
    /*! Return a PositionMethod's enumerator given an unsigned int.
      * @param i the index of the enumerator in PositionMethodMod::PositionMethod.
      * @return a PositionMethodMod::PositionMethod's enumerator.
      * @throws a string containing an error message if no enumerator could be found for this integer.
      */
 	static PositionMethodMod::PositionMethod from_int(unsigned int i);	
 	

  private:
    /* Not Implemented.  This is a pure static class. */
    CPositionMethod();
    CPositionMethod(const CPositionMethod&);
    CPositionMethod& operator=(const CPositionMethod&);
    
    static std::string badString(const std::string& name) ;
  	static std::string badInt(unsigned int i) ;
  	
};
 
#endif /*!CPositionMethod_H*/
