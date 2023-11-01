
#ifndef CSidebandProcessingMode_H
#define CSidebandProcessingMode_H

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
 * File CSidebandProcessingMode.h
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#include <iostream>
#include <string>
#include <vector>
/**
  * A namespace to encapsulate the SidebandProcessingMode enumeration.
  */
#ifndef WITHOUT_ACS
#include <almaEnumerations_IFC.h>
#else

// This part mimics the behaviour of 
namespace SidebandProcessingModeMod
{
  //! SidebandProcessingMode.
  //!  [ASDM.SpectralWindow] Real-time processing to derive sideband data
  
  const char *const revision = "-1";
  const int version = 1;
  
  enum SidebandProcessingMode
  { 
    NONE /*!< No processing */
     ,
    PHASE_SWITCH_SEPARATION /*!< Side band separation using 90-degree phase switching */
     ,
    FREQUENCY_OFFSET_SEPARATION /*!< Side band separation using offsets of first ans second oscillators */
     ,
    PHASE_SWITCH_REJECTION /*!< Side band rejection 90-degree phase switching */
     ,
    FREQUENCY_OFFSET_REJECTION /*!< Side band rejection using offsets of first and second oscillators */
     
  };
  typedef SidebandProcessingMode &SidebandProcessingMode_out;
} 
#endif

namespace SidebandProcessingModeMod {
	std::ostream & operator << ( std::ostream & out, const SidebandProcessingMode& value);
	std::istream & operator >> ( std::istream & in , SidebandProcessingMode& value );
}

/** 
  * A helper class for the enumeration SidebandProcessingMode.
  * 
  */
class CSidebandProcessingMode {
  public:
 
	/**
	  * Enumerators as strings.
	  */  
	
	static const std::string& sNONE; /*!< A const string equal to "NONE".*/
	
	static const std::string& sPHASE_SWITCH_SEPARATION; /*!< A const string equal to "PHASE_SWITCH_SEPARATION".*/
	
	static const std::string& sFREQUENCY_OFFSET_SEPARATION; /*!< A const string equal to "FREQUENCY_OFFSET_SEPARATION".*/
	
	static const std::string& sPHASE_SWITCH_REJECTION; /*!< A const string equal to "PHASE_SWITCH_REJECTION".*/
	
	static const std::string& sFREQUENCY_OFFSET_REJECTION; /*!< A const string equal to "FREQUENCY_OFFSET_REJECTION".*/
	

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
       * Return the number of enumerators declared in SidebandProcessingModeMod::SidebandProcessingMode.
       * @return an unsigned int.
       */
       static unsigned int size() ;
       
       
    /**
      * Returns an enumerator as a string.
      * @param e an enumerator of SidebandProcessingModeMod::SidebandProcessingMode.
      * @return a string.
      */
	static std::string name(const SidebandProcessingModeMod::SidebandProcessingMode& e);
	
	/**
	  * Equivalent to the name method.
	  */
    static std::string toString(const SidebandProcessingModeMod::SidebandProcessingMode& f) { return name(f); }

	/** 
	  * Returns vector of  all the enumerators as strings. 
	  * The strings are stored in the vector in the same order than the enumerators are declared in the enumeration. 
	  * @return a vector of string.
	  */
     static const std::vector<std::string> names();	 
    
   	
   	// Create a SidebandProcessingMode enumeration object by specifying its name.
   	static SidebandProcessingModeMod::SidebandProcessingMode newSidebandProcessingMode(const std::string& name);
   	
   	/*! Return a SidebandProcessingMode's enumerator  given a string.
   	  * @param name the string representation of the enumerator.
   	 *  @return a SidebandProcessingModeMod::SidebandProcessingMode's enumerator.
   	 *  @throws a string containing an error message if no enumerator could be found for this name.
   	 */
 	static SidebandProcessingModeMod::SidebandProcessingMode literal(const std::string& name);
 	
    /*! Return a SidebandProcessingMode's enumerator given an unsigned int.
      * @param i the index of the enumerator in SidebandProcessingModeMod::SidebandProcessingMode.
      * @return a SidebandProcessingModeMod::SidebandProcessingMode's enumerator.
      * @throws a string containing an error message if no enumerator could be found for this integer.
      */
 	static SidebandProcessingModeMod::SidebandProcessingMode from_int(unsigned int i);	
 	

  private:
    /* Not Implemented.  This is a pure static class. */
    CSidebandProcessingMode();
    CSidebandProcessingMode(const CSidebandProcessingMode&);
    CSidebandProcessingMode& operator=(const CSidebandProcessingMode&);
    
    static std::string badString(const std::string& name) ;
  	static std::string badInt(unsigned int i) ;
  	
};
 
#endif /*!CSidebandProcessingMode_H*/
