
#ifndef CCorrelatorName_H
#define CCorrelatorName_H

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
 * File CCorrelatorName.h
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#include <iostream>
#include <string>
#include <vector>
/**
  * A namespace to encapsulate the CorrelatorName enumeration.
  */
#ifndef WITHOUT_ACS
#include <almaEnumerations_IFC.h>
#else

// This part mimics the behaviour of 
namespace CorrelatorNameMod
{
  //! CorrelatorName.
  //! 
  
  const char *const revision = "-1";
  const int version = 1;
  
  enum CorrelatorName
  { 
    ALMA_ACA /*!< ACA correlator */
     ,
    ALMA_ACASPEC /*!< ACA spectrometer */
     ,
    ALMA_BASELINE /*!<  */
     ,
    ALMA_BASELINE_ATF /*!<  */
     ,
    ALMA_BASELINE_PROTO_OSF /*!<  */
     ,
    HERSCHEL /*!<  */
     ,
    IRAM_PDB /*!<  */
     ,
    IRAM_30M_VESPA /*!< Up to 18000 channels. */
     ,
    IRAM_WILMA /*!< 2 MHz, 18x930 MHz, HERA (wide) */
     ,
    NRAO_VLA /*!< VLA correlator. */
     ,
    NRAO_WIDAR /*!< EVLA correlator. */
     
  };
  typedef CorrelatorName &CorrelatorName_out;
} 
#endif

namespace CorrelatorNameMod {
	std::ostream & operator << ( std::ostream & out, const CorrelatorName& value);
	std::istream & operator >> ( std::istream & in , CorrelatorName& value );
}

/** 
  * A helper class for the enumeration CorrelatorName.
  * 
  */
class CCorrelatorName {
  public:
 
	/**
	  * Enumerators as strings.
	  */  
	
	static const std::string& sALMA_ACA; /*!< A const string equal to "ALMA_ACA".*/
	
	static const std::string& sALMA_ACASPEC; /*!< A const string equal to "ALMA_ACASPEC".*/
	
	static const std::string& sALMA_BASELINE; /*!< A const string equal to "ALMA_BASELINE".*/
	
	static const std::string& sALMA_BASELINE_ATF; /*!< A const string equal to "ALMA_BASELINE_ATF".*/
	
	static const std::string& sALMA_BASELINE_PROTO_OSF; /*!< A const string equal to "ALMA_BASELINE_PROTO_OSF".*/
	
	static const std::string& sHERSCHEL; /*!< A const string equal to "HERSCHEL".*/
	
	static const std::string& sIRAM_PDB; /*!< A const string equal to "IRAM_PDB".*/
	
	static const std::string& sIRAM_30M_VESPA; /*!< A const string equal to "IRAM_30M_VESPA".*/
	
	static const std::string& sIRAM_WILMA; /*!< A const string equal to "IRAM_WILMA".*/
	
	static const std::string& sNRAO_VLA; /*!< A const string equal to "NRAO_VLA".*/
	
	static const std::string& sNRAO_WIDAR; /*!< A const string equal to "NRAO_WIDAR".*/
	

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
       * Return the number of enumerators declared in CorrelatorNameMod::CorrelatorName.
       * @return an unsigned int.
       */
       static unsigned int size() ;
       
       
    /**
      * Returns an enumerator as a string.
      * @param e an enumerator of CorrelatorNameMod::CorrelatorName.
      * @return a string.
      */
	static std::string name(const CorrelatorNameMod::CorrelatorName& e);
	
	/**
	  * Equivalent to the name method.
	  */
    static std::string toString(const CorrelatorNameMod::CorrelatorName& f) { return name(f); }

	/** 
	  * Returns vector of  all the enumerators as strings. 
	  * The strings are stored in the vector in the same order than the enumerators are declared in the enumeration. 
	  * @return a vector of string.
	  */
     static const std::vector<std::string> names();	 
    
   	
   	// Create a CorrelatorName enumeration object by specifying its name.
   	static CorrelatorNameMod::CorrelatorName newCorrelatorName(const std::string& name);
   	
   	/*! Return a CorrelatorName's enumerator  given a string.
   	  * @param name the string representation of the enumerator.
   	 *  @return a CorrelatorNameMod::CorrelatorName's enumerator.
   	 *  @throws a string containing an error message if no enumerator could be found for this name.
   	 */
 	static CorrelatorNameMod::CorrelatorName literal(const std::string& name);
 	
    /*! Return a CorrelatorName's enumerator given an unsigned int.
      * @param i the index of the enumerator in CorrelatorNameMod::CorrelatorName.
      * @return a CorrelatorNameMod::CorrelatorName's enumerator.
      * @throws a string containing an error message if no enumerator could be found for this integer.
      */
 	static CorrelatorNameMod::CorrelatorName from_int(unsigned int i);	
 	

  private:
    /* Not Implemented.  This is a pure static class. */
    CCorrelatorName();
    CCorrelatorName(const CCorrelatorName&);
    CCorrelatorName& operator=(const CCorrelatorName&);
    
    static std::string badString(const std::string& name) ;
  	static std::string badInt(unsigned int i) ;
  	
};
 
#endif /*!CCorrelatorName_H*/
