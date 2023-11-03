
#ifndef CDataContent_H
#define CDataContent_H

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
 * File CDataContent.h
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#include <iostream>
#include <string>
#include <vector>
/**
  * A namespace to encapsulate the DataContent enumeration.
  */
#ifndef WITHOUT_ACS
#include <almaEnumerations_IFC.h>
#else

// This part mimics the behaviour of 
namespace DataContentMod
{
  //! DataContent.
  //!  [ASDM.Binaries] Contents of binary data attachment
  
  const char *const revision = "-1";
  const int version = 1;
  
  enum DataContent
  { 
    CROSS_DATA /*!< Cross-correlation data */
     ,
    AUTO_DATA /*!< Auto-correlation data */
     ,
    ZERO_LAGS /*!< Zero-lag data */
     ,
    ACTUAL_TIMES /*!< :Actual times (mid points of integrations) */
     ,
    ACTUAL_DURATIONS /*!< Actual duration of integrations */
     ,
    WEIGHTS /*!< Weights */
     ,
    FLAGS /*!< Baseband based flags */
     
  };
  typedef DataContent &DataContent_out;
} 
#endif

namespace DataContentMod {
	std::ostream & operator << ( std::ostream & out, const DataContent& value);
	std::istream & operator >> ( std::istream & in , DataContent& value );
}

/** 
  * A helper class for the enumeration DataContent.
  * 
  */
class CDataContent {
  public:
 
	/**
	  * Enumerators as strings.
	  */  
	
	static const std::string& sCROSS_DATA; /*!< A const string equal to "CROSS_DATA".*/
	
	static const std::string& sAUTO_DATA; /*!< A const string equal to "AUTO_DATA".*/
	
	static const std::string& sZERO_LAGS; /*!< A const string equal to "ZERO_LAGS".*/
	
	static const std::string& sACTUAL_TIMES; /*!< A const string equal to "ACTUAL_TIMES".*/
	
	static const std::string& sACTUAL_DURATIONS; /*!< A const string equal to "ACTUAL_DURATIONS".*/
	
	static const std::string& sWEIGHTS; /*!< A const string equal to "WEIGHTS".*/
	
	static const std::string& sFLAGS; /*!< A const string equal to "FLAGS".*/
	

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
       * Return the number of enumerators declared in DataContentMod::DataContent.
       * @return an unsigned int.
       */
       static unsigned int size() ;
       
       
    /**
      * Returns an enumerator as a string.
      * @param e an enumerator of DataContentMod::DataContent.
      * @return a string.
      */
	static std::string name(const DataContentMod::DataContent& e);
	
	/**
	  * Equivalent to the name method.
	  */
    static std::string toString(const DataContentMod::DataContent& f) { return name(f); }

	/** 
	  * Returns vector of  all the enumerators as strings. 
	  * The strings are stored in the vector in the same order than the enumerators are declared in the enumeration. 
	  * @return a vector of string.
	  */
     static const std::vector<std::string> names();	 
    
   	
   	// Create a DataContent enumeration object by specifying its name.
   	static DataContentMod::DataContent newDataContent(const std::string& name);
   	
   	/*! Return a DataContent's enumerator  given a string.
   	  * @param name the string representation of the enumerator.
   	 *  @return a DataContentMod::DataContent's enumerator.
   	 *  @throws a string containing an error message if no enumerator could be found for this name.
   	 */
 	static DataContentMod::DataContent literal(const std::string& name);
 	
    /*! Return a DataContent's enumerator given an unsigned int.
      * @param i the index of the enumerator in DataContentMod::DataContent.
      * @return a DataContentMod::DataContent's enumerator.
      * @throws a string containing an error message if no enumerator could be found for this integer.
      */
 	static DataContentMod::DataContent from_int(unsigned int i);	
 	

  private:
    /* Not Implemented.  This is a pure static class. */
    CDataContent();
    CDataContent(const CDataContent&);
    CDataContent& operator=(const CDataContent&);
    
    static std::string badString(const std::string& name) ;
  	static std::string badInt(unsigned int i) ;
  	
};
 
#endif /*!CDataContent_H*/
