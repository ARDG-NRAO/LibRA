
#ifndef CRadialVelocityReferenceCode_H
#define CRadialVelocityReferenceCode_H

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
 * File CRadialVelocityReferenceCode.h
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#include <iostream>
#include <string>
#include <vector>
/**
  * A namespace to encapsulate the RadialVelocityReferenceCode enumeration.
  */
#ifndef WITHOUT_ACS
#include <almaEnumerations_IFC.h>
#else

// This part mimics the behaviour of 
namespace RadialVelocityReferenceCodeMod
{
  //! RadialVelocityReferenceCode.
  //! 
  
  const char *const revision = "-1";
  const int version = 1;
  
  enum RadialVelocityReferenceCode
  { 
    LSRD /*!< dynamic local standard of rest. */
     ,
    LSRK /*!< kinematic local standard of rest. */
     ,
    GALACTO /*!< galactocentric frequency. */
     ,
    BARY /*!< barycentric frequency. */
     ,
    GEO /*!< geocentric frequency. */
     ,
    TOPO /*!< topocentric frequency. */
     
  };
  typedef RadialVelocityReferenceCode &RadialVelocityReferenceCode_out;
} 
#endif

namespace RadialVelocityReferenceCodeMod {
	std::ostream & operator << ( std::ostream & out, const RadialVelocityReferenceCode& value);
	std::istream & operator >> ( std::istream & in , RadialVelocityReferenceCode& value );
}

/** 
  * A helper class for the enumeration RadialVelocityReferenceCode.
  * 
  */
class CRadialVelocityReferenceCode {
  public:
 
	/**
	  * Enumerators as strings.
	  */  
	
	static const std::string& sLSRD; /*!< A const string equal to "LSRD".*/
	
	static const std::string& sLSRK; /*!< A const string equal to "LSRK".*/
	
	static const std::string& sGALACTO; /*!< A const string equal to "GALACTO".*/
	
	static const std::string& sBARY; /*!< A const string equal to "BARY".*/
	
	static const std::string& sGEO; /*!< A const string equal to "GEO".*/
	
	static const std::string& sTOPO; /*!< A const string equal to "TOPO".*/
	

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
       * Return the number of enumerators declared in RadialVelocityReferenceCodeMod::RadialVelocityReferenceCode.
       * @return an unsigned int.
       */
       static unsigned int size() ;
       
       
    /**
      * Returns an enumerator as a string.
      * @param e an enumerator of RadialVelocityReferenceCodeMod::RadialVelocityReferenceCode.
      * @return a string.
      */
	static std::string name(const RadialVelocityReferenceCodeMod::RadialVelocityReferenceCode& e);
	
	/**
	  * Equivalent to the name method.
	  */
    static std::string toString(const RadialVelocityReferenceCodeMod::RadialVelocityReferenceCode& f) { return name(f); }

	/** 
	  * Returns vector of  all the enumerators as strings. 
	  * The strings are stored in the vector in the same order than the enumerators are declared in the enumeration. 
	  * @return a vector of string.
	  */
     static const std::vector<std::string> names();	 
    
   	
   	// Create a RadialVelocityReferenceCode enumeration object by specifying its name.
   	static RadialVelocityReferenceCodeMod::RadialVelocityReferenceCode newRadialVelocityReferenceCode(const std::string& name);
   	
   	/*! Return a RadialVelocityReferenceCode's enumerator  given a string.
   	  * @param name the string representation of the enumerator.
   	 *  @return a RadialVelocityReferenceCodeMod::RadialVelocityReferenceCode's enumerator.
   	 *  @throws a string containing an error message if no enumerator could be found for this name.
   	 */
 	static RadialVelocityReferenceCodeMod::RadialVelocityReferenceCode literal(const std::string& name);
 	
    /*! Return a RadialVelocityReferenceCode's enumerator given an unsigned int.
      * @param i the index of the enumerator in RadialVelocityReferenceCodeMod::RadialVelocityReferenceCode.
      * @return a RadialVelocityReferenceCodeMod::RadialVelocityReferenceCode's enumerator.
      * @throws a string containing an error message if no enumerator could be found for this integer.
      */
 	static RadialVelocityReferenceCodeMod::RadialVelocityReferenceCode from_int(unsigned int i);	
 	

  private:
    /* Not Implemented.  This is a pure static class. */
    CRadialVelocityReferenceCode();
    CRadialVelocityReferenceCode(const CRadialVelocityReferenceCode&);
    CRadialVelocityReferenceCode& operator=(const CRadialVelocityReferenceCode&);
    
    static std::string badString(const std::string& name) ;
  	static std::string badInt(unsigned int i) ;
  	
};
 
#endif /*!CRadialVelocityReferenceCode_H*/
