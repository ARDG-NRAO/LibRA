
#ifndef CSwitchingMode_H
#define CSwitchingMode_H

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
 * File CSwitchingMode.h
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#include <iostream>
#include <string>
#include <vector>
/**
  * A namespace to encapsulate the SwitchingMode enumeration.
  */
#ifndef WITHOUT_ACS
#include <almaEnumerations_IFC.h>
#else

// This part mimics the behaviour of 
namespace SwitchingModeMod
{
  //! SwitchingMode.
  //!  Switching modes: there are two categories of switching modes, those at high rate (chopper wheel, nutator and frequency switch) which involve the BIN axis and those at low  rate (frequency, position, load and phase switching) unrelated to the bin axis. Note that in case of  frequency switching mode it is the context which tells in which of these two categories it is used.
  
  const char *const revision = "-1";
  const int version = 1;
  
  enum SwitchingMode
  { 
    NO_SWITCHING /*!< No switching */
     ,
    LOAD_SWITCHING /*!< Receiver beam is switched between sky and load */
     ,
    POSITION_SWITCHING /*!< Antenna (main reflector) pointing direction  is switched  */
     ,
    PHASE_SWITCHING /*!< 90 degrees phase switching  (switching mode used for sideband separation or rejection with DSB receivers) */
     ,
    FREQUENCY_SWITCHING /*!< LO frequency is switched (definition context sensitive: fast if cycle shrorter than the integration duration, slow if e.g. step one step per subscan) */
     ,
    NUTATOR_SWITCHING /*!< Switching between different directions by nutating the sub-reflector */
     ,
    CHOPPER_WHEEL /*!< Switching using a chopper wheel */
     
  };
  typedef SwitchingMode &SwitchingMode_out;
} 
#endif

namespace SwitchingModeMod {
	std::ostream & operator << ( std::ostream & out, const SwitchingMode& value);
	std::istream & operator >> ( std::istream & in , SwitchingMode& value );
}

/** 
  * A helper class for the enumeration SwitchingMode.
  * 
  */
class CSwitchingMode {
  public:
 
	/**
	  * Enumerators as strings.
	  */  
	
	static const std::string& sNO_SWITCHING; /*!< A const string equal to "NO_SWITCHING".*/
	
	static const std::string& sLOAD_SWITCHING; /*!< A const string equal to "LOAD_SWITCHING".*/
	
	static const std::string& sPOSITION_SWITCHING; /*!< A const string equal to "POSITION_SWITCHING".*/
	
	static const std::string& sPHASE_SWITCHING; /*!< A const string equal to "PHASE_SWITCHING".*/
	
	static const std::string& sFREQUENCY_SWITCHING; /*!< A const string equal to "FREQUENCY_SWITCHING".*/
	
	static const std::string& sNUTATOR_SWITCHING; /*!< A const string equal to "NUTATOR_SWITCHING".*/
	
	static const std::string& sCHOPPER_WHEEL; /*!< A const string equal to "CHOPPER_WHEEL".*/
	

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
       * Return the number of enumerators declared in SwitchingModeMod::SwitchingMode.
       * @return an unsigned int.
       */
       static unsigned int size() ;
       
       
    /**
      * Returns an enumerator as a string.
      * @param e an enumerator of SwitchingModeMod::SwitchingMode.
      * @return a string.
      */
	static std::string name(const SwitchingModeMod::SwitchingMode& e);
	
	/**
	  * Equivalent to the name method.
	  */
    static std::string toString(const SwitchingModeMod::SwitchingMode& f) { return name(f); }

	/** 
	  * Returns vector of  all the enumerators as strings. 
	  * The strings are stored in the vector in the same order than the enumerators are declared in the enumeration. 
	  * @return a vector of string.
	  */
     static const std::vector<std::string> names();	 
    
   	
   	// Create a SwitchingMode enumeration object by specifying its name.
   	static SwitchingModeMod::SwitchingMode newSwitchingMode(const std::string& name);
   	
   	/*! Return a SwitchingMode's enumerator  given a string.
   	  * @param name the string representation of the enumerator.
   	 *  @return a SwitchingModeMod::SwitchingMode's enumerator.
   	 *  @throws a string containing an error message if no enumerator could be found for this name.
   	 */
 	static SwitchingModeMod::SwitchingMode literal(const std::string& name);
 	
    /*! Return a SwitchingMode's enumerator given an unsigned int.
      * @param i the index of the enumerator in SwitchingModeMod::SwitchingMode.
      * @return a SwitchingModeMod::SwitchingMode's enumerator.
      * @throws a string containing an error message if no enumerator could be found for this integer.
      */
 	static SwitchingModeMod::SwitchingMode from_int(unsigned int i);	
 	

  private:
    /* Not Implemented.  This is a pure static class. */
    CSwitchingMode();
    CSwitchingMode(const CSwitchingMode&);
    CSwitchingMode& operator=(const CSwitchingMode&);
    
    static std::string badString(const std::string& name) ;
  	static std::string badInt(unsigned int i) ;
  	
};
 
#endif /*!CSwitchingMode_H*/
