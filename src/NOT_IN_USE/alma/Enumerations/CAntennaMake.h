
#ifndef CAntennaMake_H
#define CAntennaMake_H

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
 * File CAntennaMake.h
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#include <iostream>
#include <string>
#include <vector>
/**
  * A namespace to encapsulate the AntennaMake enumeration.
  */
#ifndef WITHOUT_ACS
#include <almaEnumerations_IFC.h>
#else

// This part mimics the behaviour of 
namespace AntennaMakeMod
{
  //! AntennaMake.
  //! The physical types of antenna
  
  const char *const revision = "-1";
  const int version = 1;
  
  enum AntennaMake
  { 
    AEM_12 /*!< 12m AEM antenna */
     ,
    MITSUBISHI_7 /*!< 7-m Mitsubishi antenna (ACA) */
     ,
    MITSUBISHI_12_A /*!< 12-m Mitsubishi antenna (ACA) (refurbished prototype) */
     ,
    MITSUBISHI_12_B /*!< 12-m Mitsubishi antenna (ACA) (production) */
     ,
    VERTEX_12_ATF /*!< 12-m Vertex antenna prototype */
     ,
    AEM_12_ATF /*!< 12-m AEM  antenna prototype */
     ,
    VERTEX_12 /*!< 12-m Vertex antenna */
     ,
    IRAM_15 /*!< 15-m IRAM antenna */
     ,
    UNDEFINED /*!< Not defined or not applicable. */
     
  };
  typedef AntennaMake &AntennaMake_out;
} 
#endif

namespace AntennaMakeMod {
	std::ostream & operator << ( std::ostream & out, const AntennaMake& value);
	std::istream & operator >> ( std::istream & in , AntennaMake& value );
}

/** 
  * A helper class for the enumeration AntennaMake.
  * 
  */
class CAntennaMake {
  public:
 
	/**
	  * Enumerators as strings.
	  */  
	
	static const std::string& sAEM_12; /*!< A const string equal to "AEM_12".*/
	
	static const std::string& sMITSUBISHI_7; /*!< A const string equal to "MITSUBISHI_7".*/
	
	static const std::string& sMITSUBISHI_12_A; /*!< A const string equal to "MITSUBISHI_12_A".*/
	
	static const std::string& sMITSUBISHI_12_B; /*!< A const string equal to "MITSUBISHI_12_B".*/
	
	static const std::string& sVERTEX_12_ATF; /*!< A const string equal to "VERTEX_12_ATF".*/
	
	static const std::string& sAEM_12_ATF; /*!< A const string equal to "AEM_12_ATF".*/
	
	static const std::string& sVERTEX_12; /*!< A const string equal to "VERTEX_12".*/
	
	static const std::string& sIRAM_15; /*!< A const string equal to "IRAM_15".*/
	
	static const std::string& sUNDEFINED; /*!< A const string equal to "UNDEFINED".*/
	

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
       * Return the number of enumerators declared in AntennaMakeMod::AntennaMake.
       * @return an unsigned int.
       */
       static unsigned int size() ;
       
       
    /**
      * Returns an enumerator as a string.
      * @param e an enumerator of AntennaMakeMod::AntennaMake.
      * @return a string.
      */
	static std::string name(const AntennaMakeMod::AntennaMake& e);
	
	/**
	  * Equivalent to the name method.
	  */
    static std::string toString(const AntennaMakeMod::AntennaMake& f) { return name(f); }

	/** 
	  * Returns vector of  all the enumerators as strings. 
	  * The strings are stored in the vector in the same order than the enumerators are declared in the enumeration. 
	  * @return a vector of string.
	  */
     static const std::vector<std::string> names();	 
    
   	
   	// Create a AntennaMake enumeration object by specifying its name.
   	static AntennaMakeMod::AntennaMake newAntennaMake(const std::string& name);
   	
   	/*! Return a AntennaMake's enumerator  given a string.
   	  * @param name the string representation of the enumerator.
   	 *  @return a AntennaMakeMod::AntennaMake's enumerator.
   	 *  @throws a string containing an error message if no enumerator could be found for this name.
   	 */
 	static AntennaMakeMod::AntennaMake literal(const std::string& name);
 	
    /*! Return a AntennaMake's enumerator given an unsigned int.
      * @param i the index of the enumerator in AntennaMakeMod::AntennaMake.
      * @return a AntennaMakeMod::AntennaMake's enumerator.
      * @throws a string containing an error message if no enumerator could be found for this integer.
      */
 	static AntennaMakeMod::AntennaMake from_int(unsigned int i);	
 	

  private:
    /* Not Implemented.  This is a pure static class. */
    CAntennaMake();
    CAntennaMake(const CAntennaMake&);
    CAntennaMake& operator=(const CAntennaMake&);
    
    static std::string badString(const std::string& name) ;
  	static std::string badInt(unsigned int i) ;
  	
};
 
#endif /*!CAntennaMake_H*/
