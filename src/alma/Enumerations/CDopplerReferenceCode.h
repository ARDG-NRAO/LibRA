
#ifndef CDopplerReferenceCode_H
#define CDopplerReferenceCode_H

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
 * File CDopplerReferenceCode.h
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#include <iostream>
#include <string>
#include <vector>
/**
  * A namespace to encapsulate the DopplerReferenceCode enumeration.
  */
#ifndef WITHOUT_ACS
#include <almaEnumerations_IFC.h>
#else

// This part mimics the behaviour of 
namespace DopplerReferenceCodeMod
{
  //! DopplerReferenceCode.
  //! defines reference frames to qualify the measure of a radial velocity expressed as doppler shift.
  
  const char *const revision = "-1";
  const int version = 1;
  
  enum DopplerReferenceCode
  { 
    RADIO /*!< radio definition : \f$ 1 - F \f$
 */
     ,
    Z /*!< redshift : \f$ - 1 + 1 / F \f$ */
     ,
    RATIO /*!< frequency ratio : \f$ F \f$
 */
     ,
    BETA /*!< relativistic : \f$(1 - F^2) / (1 + F^2) \f$ */
     ,
    GAMMA /*!< \f$(1 + F^2)/(2*F)\f$ */
     ,
    OPTICAL /*!< \f$Z\f$Z */
     ,
    RELATIVISTIC /*!< idem BETA */
     
  };
  typedef DopplerReferenceCode &DopplerReferenceCode_out;
} 
#endif

namespace DopplerReferenceCodeMod {
	std::ostream & operator << ( std::ostream & out, const DopplerReferenceCode& value);
	std::istream & operator >> ( std::istream & in , DopplerReferenceCode& value );
}

/** 
  * A helper class for the enumeration DopplerReferenceCode.
  * 
  */
class CDopplerReferenceCode {
  public:
 
	/**
	  * Enumerators as strings.
	  */  
	
	static const std::string& sRADIO; /*!< A const string equal to "RADIO".*/
	
	static const std::string& sZ; /*!< A const string equal to "Z".*/
	
	static const std::string& sRATIO; /*!< A const string equal to "RATIO".*/
	
	static const std::string& sBETA; /*!< A const string equal to "BETA".*/
	
	static const std::string& sGAMMA; /*!< A const string equal to "GAMMA".*/
	
	static const std::string& sOPTICAL; /*!< A const string equal to "OPTICAL".*/
	
	static const std::string& sRELATIVISTIC; /*!< A const string equal to "RELATIVISTIC".*/
	

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
       * Return the number of enumerators declared in DopplerReferenceCodeMod::DopplerReferenceCode.
       * @return an unsigned int.
       */
       static unsigned int size() ;
       
       
    /**
      * Returns an enumerator as a string.
      * @param e an enumerator of DopplerReferenceCodeMod::DopplerReferenceCode.
      * @return a string.
      */
	static std::string name(const DopplerReferenceCodeMod::DopplerReferenceCode& e);
	
	/**
	  * Equivalent to the name method.
	  */
    static std::string toString(const DopplerReferenceCodeMod::DopplerReferenceCode& f) { return name(f); }

	/** 
	  * Returns vector of  all the enumerators as strings. 
	  * The strings are stored in the vector in the same order than the enumerators are declared in the enumeration. 
	  * @return a vector of string.
	  */
     static const std::vector<std::string> names();	 
    
   	
   	// Create a DopplerReferenceCode enumeration object by specifying its name.
   	static DopplerReferenceCodeMod::DopplerReferenceCode newDopplerReferenceCode(const std::string& name);
   	
   	/*! Return a DopplerReferenceCode's enumerator  given a string.
   	  * @param name the string representation of the enumerator.
   	 *  @return a DopplerReferenceCodeMod::DopplerReferenceCode's enumerator.
   	 *  @throws a string containing an error message if no enumerator could be found for this name.
   	 */
 	static DopplerReferenceCodeMod::DopplerReferenceCode literal(const std::string& name);
 	
    /*! Return a DopplerReferenceCode's enumerator given an unsigned int.
      * @param i the index of the enumerator in DopplerReferenceCodeMod::DopplerReferenceCode.
      * @return a DopplerReferenceCodeMod::DopplerReferenceCode's enumerator.
      * @throws a string containing an error message if no enumerator could be found for this integer.
      */
 	static DopplerReferenceCodeMod::DopplerReferenceCode from_int(unsigned int i);	
 	

  private:
    /* Not Implemented.  This is a pure static class. */
    CDopplerReferenceCode();
    CDopplerReferenceCode(const CDopplerReferenceCode&);
    CDopplerReferenceCode& operator=(const CDopplerReferenceCode&);
    
    static std::string badString(const std::string& name) ;
  	static std::string badInt(unsigned int i) ;
  	
};
 
#endif /*!CDopplerReferenceCode_H*/
