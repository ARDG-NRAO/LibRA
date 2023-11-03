
#ifndef CSubscanIntent_H
#define CSubscanIntent_H

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
 * File CSubscanIntent.h
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#include <iostream>
#include <string>
#include <vector>
/**
  * A namespace to encapsulate the SubscanIntent enumeration.
  */
#ifndef WITHOUT_ACS
#include <almaEnumerations_IFC.h>
#else

// This part mimics the behaviour of 
namespace SubscanIntentMod
{
  //! SubscanIntent.
  //! [ASDM.Subscan] Precise the intent for a subscan
  
  const char *const revision = "-1";
  const int version = 1;
  
  enum SubscanIntent
  { 
    ON_SOURCE /*!< on-source measurement */
     ,
    OFF_SOURCE /*!< off-source measurement */
     ,
    MIXED /*!< Pointing measurement, some antennas are on -ource, some off-source */
     ,
    REFERENCE /*!< reference measurement (used for boresight in holography). */
     ,
    SCANNING /*!< antennas are scanning. */
     ,
    HOT /*!< hot load measurement. */
     ,
    AMBIENT /*!< ambient load measurement. */
     ,
    SIGNAL /*!< Signal sideband measurement. */
     ,
    IMAGE /*!< Image sideband measurement. */
     ,
    TEST /*!< reserved for development. */
     ,
    UNSPECIFIED /*!< Unspecified */
     
  };
  typedef SubscanIntent &SubscanIntent_out;
} 
#endif

namespace SubscanIntentMod {
	std::ostream & operator << ( std::ostream & out, const SubscanIntent& value);
	std::istream & operator >> ( std::istream & in , SubscanIntent& value );
}

/** 
  * A helper class for the enumeration SubscanIntent.
  * 
  */
class CSubscanIntent {
  public:
 
	/**
	  * Enumerators as strings.
	  */  
	
	static const std::string& sON_SOURCE; /*!< A const string equal to "ON_SOURCE".*/
	
	static const std::string& sOFF_SOURCE; /*!< A const string equal to "OFF_SOURCE".*/
	
	static const std::string& sMIXED; /*!< A const string equal to "MIXED".*/
	
	static const std::string& sREFERENCE; /*!< A const string equal to "REFERENCE".*/
	
	static const std::string& sSCANNING; /*!< A const string equal to "SCANNING".*/
	
	static const std::string& sHOT; /*!< A const string equal to "HOT".*/
	
	static const std::string& sAMBIENT; /*!< A const string equal to "AMBIENT".*/
	
	static const std::string& sSIGNAL; /*!< A const string equal to "SIGNAL".*/
	
	static const std::string& sIMAGE; /*!< A const string equal to "IMAGE".*/
	
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
       * Return the number of enumerators declared in SubscanIntentMod::SubscanIntent.
       * @return an unsigned int.
       */
       static unsigned int size() ;
       
       
    /**
      * Returns an enumerator as a string.
      * @param e an enumerator of SubscanIntentMod::SubscanIntent.
      * @return a string.
      */
	static std::string name(const SubscanIntentMod::SubscanIntent& e);
	
	/**
	  * Equivalent to the name method.
	  */
    static std::string toString(const SubscanIntentMod::SubscanIntent& f) { return name(f); }

	/** 
	  * Returns vector of  all the enumerators as strings. 
	  * The strings are stored in the vector in the same order than the enumerators are declared in the enumeration. 
	  * @return a vector of string.
	  */
     static const std::vector<std::string> names();	 
    
   	
   	// Create a SubscanIntent enumeration object by specifying its name.
   	static SubscanIntentMod::SubscanIntent newSubscanIntent(const std::string& name);
   	
   	/*! Return a SubscanIntent's enumerator  given a string.
   	  * @param name the string representation of the enumerator.
   	 *  @return a SubscanIntentMod::SubscanIntent's enumerator.
   	 *  @throws a string containing an error message if no enumerator could be found for this name.
   	 */
 	static SubscanIntentMod::SubscanIntent literal(const std::string& name);
 	
    /*! Return a SubscanIntent's enumerator given an unsigned int.
      * @param i the index of the enumerator in SubscanIntentMod::SubscanIntent.
      * @return a SubscanIntentMod::SubscanIntent's enumerator.
      * @throws a string containing an error message if no enumerator could be found for this integer.
      */
 	static SubscanIntentMod::SubscanIntent from_int(unsigned int i);	
 	

  private:
    /* Not Implemented.  This is a pure static class. */
    CSubscanIntent();
    CSubscanIntent(const CSubscanIntent&);
    CSubscanIntent& operator=(const CSubscanIntent&);
    
    static std::string badString(const std::string& name) ;
  	static std::string badInt(unsigned int i) ;
  	
};
 
#endif /*!CSubscanIntent_H*/
