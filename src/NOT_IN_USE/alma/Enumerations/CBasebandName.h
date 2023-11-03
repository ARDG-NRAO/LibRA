
#ifndef CBasebandName_H
#define CBasebandName_H

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
 * File CBasebandName.h
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#include <iostream>
#include <string>
#include <vector>
/**
  * A namespace to encapsulate the BasebandName enumeration.
  */
#ifndef WITHOUT_ACS
#include <almaEnumerations_IFC.h>
#else

// This part mimics the behaviour of 
namespace BasebandNameMod
{
  //! BasebandName.
  //!  Baseband names
  
  const char *const revision = "-1";
  const int version = 1;
  
  enum BasebandName
  { 
    NOBB /*!< Baseband not applicable. */
     ,
    BB_1 /*!< Baseband one */
     ,
    BB_2 /*!< Baseband two */
     ,
    BB_3 /*!< Baseband three */
     ,
    BB_4 /*!< Baseband four */
     ,
    BB_5 /*!< Baseband five (not ALMA) */
     ,
    BB_6 /*!< Baseband six (not ALMA) */
     ,
    BB_7 /*!< Baseband seven (not ALMA) */
     ,
    BB_8 /*!< Baseband eight (not ALMA) */
     ,
    BB_ALL /*!< All ALMA basebands (i.e. all available basebands) */
     ,
    A1C1_3BIT /*!<  */
     ,
    A2C2_3BIT /*!<  */
     ,
    AC_8BIT /*!<  */
     ,
    B1D1_3BIT /*!<  */
     ,
    B2D2_3BIT /*!<  */
     ,
    BD_8BIT /*!<  */
     
  };
  typedef BasebandName &BasebandName_out;
} 
#endif

namespace BasebandNameMod {
	std::ostream & operator << ( std::ostream & out, const BasebandName& value);
	std::istream & operator >> ( std::istream & in , BasebandName& value );
}

/** 
  * A helper class for the enumeration BasebandName.
  * 
  */
class CBasebandName {
  public:
 
	/**
	  * Enumerators as strings.
	  */  
	
	static const std::string& sNOBB; /*!< A const string equal to "NOBB".*/
	
	static const std::string& sBB_1; /*!< A const string equal to "BB_1".*/
	
	static const std::string& sBB_2; /*!< A const string equal to "BB_2".*/
	
	static const std::string& sBB_3; /*!< A const string equal to "BB_3".*/
	
	static const std::string& sBB_4; /*!< A const string equal to "BB_4".*/
	
	static const std::string& sBB_5; /*!< A const string equal to "BB_5".*/
	
	static const std::string& sBB_6; /*!< A const string equal to "BB_6".*/
	
	static const std::string& sBB_7; /*!< A const string equal to "BB_7".*/
	
	static const std::string& sBB_8; /*!< A const string equal to "BB_8".*/
	
	static const std::string& sBB_ALL; /*!< A const string equal to "BB_ALL".*/
	
	static const std::string& sA1C1_3BIT; /*!< A const string equal to "A1C1_3BIT".*/
	
	static const std::string& sA2C2_3BIT; /*!< A const string equal to "A2C2_3BIT".*/
	
	static const std::string& sAC_8BIT; /*!< A const string equal to "AC_8BIT".*/
	
	static const std::string& sB1D1_3BIT; /*!< A const string equal to "B1D1_3BIT".*/
	
	static const std::string& sB2D2_3BIT; /*!< A const string equal to "B2D2_3BIT".*/
	
	static const std::string& sBD_8BIT; /*!< A const string equal to "BD_8BIT".*/
	

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
       * Return the number of enumerators declared in BasebandNameMod::BasebandName.
       * @return an unsigned int.
       */
       static unsigned int size() ;
       
       
    /**
      * Returns an enumerator as a string.
      * @param e an enumerator of BasebandNameMod::BasebandName.
      * @return a string.
      */
	static std::string name(const BasebandNameMod::BasebandName& e);
	
	/**
	  * Equivalent to the name method.
	  */
    static std::string toString(const BasebandNameMod::BasebandName& f) { return name(f); }

	/** 
	  * Returns vector of  all the enumerators as strings. 
	  * The strings are stored in the vector in the same order than the enumerators are declared in the enumeration. 
	  * @return a vector of string.
	  */
     static const std::vector<std::string> names();	 
    
   	
   	// Create a BasebandName enumeration object by specifying its name.
   	static BasebandNameMod::BasebandName newBasebandName(const std::string& name);
   	
   	/*! Return a BasebandName's enumerator  given a string.
   	  * @param name the string representation of the enumerator.
   	 *  @return a BasebandNameMod::BasebandName's enumerator.
   	 *  @throws a string containing an error message if no enumerator could be found for this name.
   	 */
 	static BasebandNameMod::BasebandName literal(const std::string& name);
 	
    /*! Return a BasebandName's enumerator given an unsigned int.
      * @param i the index of the enumerator in BasebandNameMod::BasebandName.
      * @return a BasebandNameMod::BasebandName's enumerator.
      * @throws a string containing an error message if no enumerator could be found for this integer.
      */
 	static BasebandNameMod::BasebandName from_int(unsigned int i);	
 	

  private:
    /* Not Implemented.  This is a pure static class. */
    CBasebandName();
    CBasebandName(const CBasebandName&);
    CBasebandName& operator=(const CBasebandName&);
    
    static std::string badString(const std::string& name) ;
  	static std::string badInt(unsigned int i) ;
  	
};
 
#endif /*!CBasebandName_H*/
