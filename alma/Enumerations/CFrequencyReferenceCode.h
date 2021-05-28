
#ifndef CFrequencyReferenceCode_H
#define CFrequencyReferenceCode_H

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
 * File CFrequencyReferenceCode.h
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#include <iostream>
#include <string>
#include <vector>
/**
  * A namespace to encapsulate the FrequencyReferenceCode enumeration.
  */
#ifndef WITHOUT_ACS
#include <almaEnumerations_IFC.h>
#else

// This part mimics the behaviour of 
namespace FrequencyReferenceCodeMod
{
  //! FrequencyReferenceCode.
  //! defines reference frames to qualify the measure of a frequency.
  
  const char *const revision = "-1";
  const int version = 1;
  
  enum FrequencyReferenceCode
  { 
    LABREST /*!< spectral line rest frequency. */
     ,
    LSRD /*!< dynamic local standard of rest. */
     ,
    LSRK /*!< kinematic local standard rest. */
     ,
    BARY /*!< barycentric frequency. */
     ,
    REST /*!< spectral line frequency */
     ,
    GEO /*!< geocentric frequency. */
     ,
    GALACTO /*!< galactocentric frequency. */
     ,
    TOPO /*!< topocentric frequency. */
     
  };
  typedef FrequencyReferenceCode &FrequencyReferenceCode_out;
} 
#endif

namespace FrequencyReferenceCodeMod {
	std::ostream & operator << ( std::ostream & out, const FrequencyReferenceCode& value);
	std::istream & operator >> ( std::istream & in , FrequencyReferenceCode& value );
}

/** 
  * A helper class for the enumeration FrequencyReferenceCode.
  * 
  */
class CFrequencyReferenceCode {
  public:
 
	/**
	  * Enumerators as strings.
	  */  
	
	static const std::string& sLABREST; /*!< A const string equal to "LABREST".*/
	
	static const std::string& sLSRD; /*!< A const string equal to "LSRD".*/
	
	static const std::string& sLSRK; /*!< A const string equal to "LSRK".*/
	
	static const std::string& sBARY; /*!< A const string equal to "BARY".*/
	
	static const std::string& sREST; /*!< A const string equal to "REST".*/
	
	static const std::string& sGEO; /*!< A const string equal to "GEO".*/
	
	static const std::string& sGALACTO; /*!< A const string equal to "GALACTO".*/
	
	static const std::string& sTOPO; /*!< A const string equal to "TOPO".*/
	

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
       * Return the number of enumerators declared in FrequencyReferenceCodeMod::FrequencyReferenceCode.
       * @return an unsigned int.
       */
       static unsigned int size() ;
       
       
    /**
      * Returns an enumerator as a string.
      * @param e an enumerator of FrequencyReferenceCodeMod::FrequencyReferenceCode.
      * @return a string.
      */
	static std::string name(const FrequencyReferenceCodeMod::FrequencyReferenceCode& e);
	
	/**
	  * Equivalent to the name method.
	  */
    static std::string toString(const FrequencyReferenceCodeMod::FrequencyReferenceCode& f) { return name(f); }

	/** 
	  * Returns vector of  all the enumerators as strings. 
	  * The strings are stored in the vector in the same order than the enumerators are declared in the enumeration. 
	  * @return a vector of string.
	  */
     static const std::vector<std::string> names();	 
    
   	
   	// Create a FrequencyReferenceCode enumeration object by specifying its name.
   	static FrequencyReferenceCodeMod::FrequencyReferenceCode newFrequencyReferenceCode(const std::string& name);
   	
   	/*! Return a FrequencyReferenceCode's enumerator  given a string.
   	  * @param name the string representation of the enumerator.
   	 *  @return a FrequencyReferenceCodeMod::FrequencyReferenceCode's enumerator.
   	 *  @throws a string containing an error message if no enumerator could be found for this name.
   	 */
 	static FrequencyReferenceCodeMod::FrequencyReferenceCode literal(const std::string& name);
 	
    /*! Return a FrequencyReferenceCode's enumerator given an unsigned int.
      * @param i the index of the enumerator in FrequencyReferenceCodeMod::FrequencyReferenceCode.
      * @return a FrequencyReferenceCodeMod::FrequencyReferenceCode's enumerator.
      * @throws a string containing an error message if no enumerator could be found for this integer.
      */
 	static FrequencyReferenceCodeMod::FrequencyReferenceCode from_int(unsigned int i);	
 	

  private:
    /* Not Implemented.  This is a pure static class. */
    CFrequencyReferenceCode();
    CFrequencyReferenceCode(const CFrequencyReferenceCode&);
    CFrequencyReferenceCode& operator=(const CFrequencyReferenceCode&);
    
    static std::string badString(const std::string& name) ;
  	static std::string badInt(unsigned int i) ;
  	
};
 
#endif /*!CFrequencyReferenceCode_H*/
