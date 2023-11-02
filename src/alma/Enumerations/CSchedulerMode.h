
#ifndef CSchedulerMode_H
#define CSchedulerMode_H

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
 * File CSchedulerMode.h
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#include <iostream>
#include <string>
#include <vector>
/**
  * A namespace to encapsulate the SchedulerMode enumeration.
  */
#ifndef WITHOUT_ACS
#include <almaEnumerations_IFC.h>
#else

// This part mimics the behaviour of 
namespace SchedulerModeMod
{
  //! SchedulerMode.
  //!  [ASDM.SBSummary] Scheduler operation mode
  
  const char *const revision = "-1";
  const int version = 1;
  
  enum SchedulerMode
  { 
    DYNAMIC /*!< Dynamic scheduling */
     ,
    INTERACTIVE /*!< Interactive scheduling */
     ,
    MANUAL /*!< Manual scheduling */
     ,
    QUEUED /*!< Queued scheduling */
     
  };
  typedef SchedulerMode &SchedulerMode_out;
} 
#endif

namespace SchedulerModeMod {
	std::ostream & operator << ( std::ostream & out, const SchedulerMode& value);
	std::istream & operator >> ( std::istream & in , SchedulerMode& value );
}

/** 
  * A helper class for the enumeration SchedulerMode.
  * 
  */
class CSchedulerMode {
  public:
 
	/**
	  * Enumerators as strings.
	  */  
	
	static const std::string& sDYNAMIC; /*!< A const string equal to "DYNAMIC".*/
	
	static const std::string& sINTERACTIVE; /*!< A const string equal to "INTERACTIVE".*/
	
	static const std::string& sMANUAL; /*!< A const string equal to "MANUAL".*/
	
	static const std::string& sQUEUED; /*!< A const string equal to "QUEUED".*/
	

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
       * Return the number of enumerators declared in SchedulerModeMod::SchedulerMode.
       * @return an unsigned int.
       */
       static unsigned int size() ;
       
       
    /**
      * Returns an enumerator as a string.
      * @param e an enumerator of SchedulerModeMod::SchedulerMode.
      * @return a string.
      */
	static std::string name(const SchedulerModeMod::SchedulerMode& e);
	
	/**
	  * Equivalent to the name method.
	  */
    static std::string toString(const SchedulerModeMod::SchedulerMode& f) { return name(f); }

	/** 
	  * Returns vector of  all the enumerators as strings. 
	  * The strings are stored in the vector in the same order than the enumerators are declared in the enumeration. 
	  * @return a vector of string.
	  */
     static const std::vector<std::string> names();	 
    
   	
   	// Create a SchedulerMode enumeration object by specifying its name.
   	static SchedulerModeMod::SchedulerMode newSchedulerMode(const std::string& name);
   	
   	/*! Return a SchedulerMode's enumerator  given a string.
   	  * @param name the string representation of the enumerator.
   	 *  @return a SchedulerModeMod::SchedulerMode's enumerator.
   	 *  @throws a string containing an error message if no enumerator could be found for this name.
   	 */
 	static SchedulerModeMod::SchedulerMode literal(const std::string& name);
 	
    /*! Return a SchedulerMode's enumerator given an unsigned int.
      * @param i the index of the enumerator in SchedulerModeMod::SchedulerMode.
      * @return a SchedulerModeMod::SchedulerMode's enumerator.
      * @throws a string containing an error message if no enumerator could be found for this integer.
      */
 	static SchedulerModeMod::SchedulerMode from_int(unsigned int i);	
 	

  private:
    /* Not Implemented.  This is a pure static class. */
    CSchedulerMode();
    CSchedulerMode(const CSchedulerMode&);
    CSchedulerMode& operator=(const CSchedulerMode&);
    
    static std::string badString(const std::string& name) ;
  	static std::string badInt(unsigned int i) ;
  	
};
 
#endif /*!CSchedulerMode_H*/
