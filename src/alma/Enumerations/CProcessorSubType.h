
#ifndef CProcessorSubType_H
#define CProcessorSubType_H

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
 * File CProcessorSubType.h
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#include <iostream>
#include <string>
#include <vector>
/**
  * A namespace to encapsulate the ProcessorSubType enumeration.
  */
#ifndef WITHOUT_ACS
#include <almaEnumerations_IFC.h>
#else

// This part mimics the behaviour of 
namespace ProcessorSubTypeMod
{
  //! ProcessorSubType.
  //!  [ASDM.Processor] The tables used to contain device configuration data
  
  const char *const revision = "-1";
  const int version = 1;
  
  enum ProcessorSubType
  { 
    ALMA_CORRELATOR_MODE /*!< ALMA correlator. */
     ,
    SQUARE_LAW_DETECTOR /*!< Square law detector. */
     ,
    HOLOGRAPHY /*!< Holography. */
     ,
    ALMA_RADIOMETER /*!< ALMA radiometer. */
     
  };
  typedef ProcessorSubType &ProcessorSubType_out;
} 
#endif

namespace ProcessorSubTypeMod {
	std::ostream & operator << ( std::ostream & out, const ProcessorSubType& value);
	std::istream & operator >> ( std::istream & in , ProcessorSubType& value );
}

/** 
  * A helper class for the enumeration ProcessorSubType.
  * 
  */
class CProcessorSubType {
  public:
 
	/**
	  * Enumerators as strings.
	  */  
	
	static const std::string& sALMA_CORRELATOR_MODE; /*!< A const string equal to "ALMA_CORRELATOR_MODE".*/
	
	static const std::string& sSQUARE_LAW_DETECTOR; /*!< A const string equal to "SQUARE_LAW_DETECTOR".*/
	
	static const std::string& sHOLOGRAPHY; /*!< A const string equal to "HOLOGRAPHY".*/
	
	static const std::string& sALMA_RADIOMETER; /*!< A const string equal to "ALMA_RADIOMETER".*/
	

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
       * Return the number of enumerators declared in ProcessorSubTypeMod::ProcessorSubType.
       * @return an unsigned int.
       */
       static unsigned int size() ;
       
       
    /**
      * Returns an enumerator as a string.
      * @param e an enumerator of ProcessorSubTypeMod::ProcessorSubType.
      * @return a string.
      */
	static std::string name(const ProcessorSubTypeMod::ProcessorSubType& e);
	
	/**
	  * Equivalent to the name method.
	  */
    static std::string toString(const ProcessorSubTypeMod::ProcessorSubType& f) { return name(f); }

	/** 
	  * Returns vector of  all the enumerators as strings. 
	  * The strings are stored in the vector in the same order than the enumerators are declared in the enumeration. 
	  * @return a vector of string.
	  */
     static const std::vector<std::string> names();	 
    
   	
   	// Create a ProcessorSubType enumeration object by specifying its name.
   	static ProcessorSubTypeMod::ProcessorSubType newProcessorSubType(const std::string& name);
   	
   	/*! Return a ProcessorSubType's enumerator  given a string.
   	  * @param name the string representation of the enumerator.
   	 *  @return a ProcessorSubTypeMod::ProcessorSubType's enumerator.
   	 *  @throws a string containing an error message if no enumerator could be found for this name.
   	 */
 	static ProcessorSubTypeMod::ProcessorSubType literal(const std::string& name);
 	
    /*! Return a ProcessorSubType's enumerator given an unsigned int.
      * @param i the index of the enumerator in ProcessorSubTypeMod::ProcessorSubType.
      * @return a ProcessorSubTypeMod::ProcessorSubType's enumerator.
      * @throws a string containing an error message if no enumerator could be found for this integer.
      */
 	static ProcessorSubTypeMod::ProcessorSubType from_int(unsigned int i);	
 	

  private:
    /* Not Implemented.  This is a pure static class. */
    CProcessorSubType();
    CProcessorSubType(const CProcessorSubType&);
    CProcessorSubType& operator=(const CProcessorSubType&);
    
    static std::string badString(const std::string& name) ;
  	static std::string badInt(unsigned int i) ;
  	
};
 
#endif /*!CProcessorSubType_H*/
