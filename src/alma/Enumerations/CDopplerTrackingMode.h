
#ifndef CDopplerTrackingMode_H
#define CDopplerTrackingMode_H

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
 * File CDopplerTrackingMode.h
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#include <iostream>
#include <string>
#include <vector>
/**
  * A namespace to encapsulate the DopplerTrackingMode enumeration.
  */
#ifndef WITHOUT_ACS
#include <almaEnumerations_IFC.h>
#else

// This part mimics the behaviour of 
namespace DopplerTrackingModeMod
{
  //! DopplerTrackingMode.
  //! Enumerations of different modes used in doppler tracking.
  
  const char *const revision = "-1";
  const int version = 1;
  
  enum DopplerTrackingMode
  { 
    NONE /*!< No Doppler tracking. */
     ,
    CONTINUOUS /*!< Continuous (every integration) Doppler tracking. */
     ,
    SCAN_BASED /*!< Doppler tracking only at scan boundaries.  This means we update  the observing frequency to the correct value, but only at scan boundaries. */
     ,
    SB_BASED /*!< Doppler tracking only at the beginning of the Scheduling Block.  We set the frequency at the beginning of the observation but leave it fixed thereafter.  For the EVLA this is referred to as  'Doppler setting'. */
     
  };
  typedef DopplerTrackingMode &DopplerTrackingMode_out;
} 
#endif

namespace DopplerTrackingModeMod {
	std::ostream & operator << ( std::ostream & out, const DopplerTrackingMode& value);
	std::istream & operator >> ( std::istream & in , DopplerTrackingMode& value );
}

/** 
  * A helper class for the enumeration DopplerTrackingMode.
  * 
  */
class CDopplerTrackingMode {
  public:
 
	/**
	  * Enumerators as strings.
	  */  
	
	static const std::string& sNONE; /*!< A const string equal to "NONE".*/
	
	static const std::string& sCONTINUOUS; /*!< A const string equal to "CONTINUOUS".*/
	
	static const std::string& sSCAN_BASED; /*!< A const string equal to "SCAN_BASED".*/
	
	static const std::string& sSB_BASED; /*!< A const string equal to "SB_BASED".*/
	

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
       * Return the number of enumerators declared in DopplerTrackingModeMod::DopplerTrackingMode.
       * @return an unsigned int.
       */
       static unsigned int size() ;
       
       
    /**
      * Returns an enumerator as a string.
      * @param e an enumerator of DopplerTrackingModeMod::DopplerTrackingMode.
      * @return a string.
      */
	static std::string name(const DopplerTrackingModeMod::DopplerTrackingMode& e);
	
	/**
	  * Equivalent to the name method.
	  */
    static std::string toString(const DopplerTrackingModeMod::DopplerTrackingMode& f) { return name(f); }

	/** 
	  * Returns vector of  all the enumerators as strings. 
	  * The strings are stored in the vector in the same order than the enumerators are declared in the enumeration. 
	  * @return a vector of string.
	  */
     static const std::vector<std::string> names();	 
    
   	
   	// Create a DopplerTrackingMode enumeration object by specifying its name.
   	static DopplerTrackingModeMod::DopplerTrackingMode newDopplerTrackingMode(const std::string& name);
   	
   	/*! Return a DopplerTrackingMode's enumerator  given a string.
   	  * @param name the string representation of the enumerator.
   	 *  @return a DopplerTrackingModeMod::DopplerTrackingMode's enumerator.
   	 *  @throws a string containing an error message if no enumerator could be found for this name.
   	 */
 	static DopplerTrackingModeMod::DopplerTrackingMode literal(const std::string& name);
 	
    /*! Return a DopplerTrackingMode's enumerator given an unsigned int.
      * @param i the index of the enumerator in DopplerTrackingModeMod::DopplerTrackingMode.
      * @return a DopplerTrackingModeMod::DopplerTrackingMode's enumerator.
      * @throws a string containing an error message if no enumerator could be found for this integer.
      */
 	static DopplerTrackingModeMod::DopplerTrackingMode from_int(unsigned int i);	
 	

  private:
    /* Not Implemented.  This is a pure static class. */
    CDopplerTrackingMode();
    CDopplerTrackingMode(const CDopplerTrackingMode&);
    CDopplerTrackingMode& operator=(const CDopplerTrackingMode&);
    
    static std::string badString(const std::string& name) ;
  	static std::string badInt(unsigned int i) ;
  	
};
 
#endif /*!CDopplerTrackingMode_H*/
