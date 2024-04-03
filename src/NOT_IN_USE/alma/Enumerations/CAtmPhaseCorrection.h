
#ifndef CAtmPhaseCorrection_H
#define CAtmPhaseCorrection_H

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
 * File CAtmPhaseCorrection.h
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#include <iostream>
#include <string>
#include <vector>
/**
  * A namespace to encapsulate the AtmPhaseCorrection enumeration.
  */
#ifndef WITHOUT_ACS
#include <almaEnumerations_IFC.h>
#else

// This part mimics the behaviour of 
namespace AtmPhaseCorrectionMod
{
  //! AtmPhaseCorrection.
  //!  Status of Phase correction
  
  const char *const revision = "-1";
  const int version = 1;
  
  enum AtmPhaseCorrection
  { 
    AP_UNCORRECTED /*!< Data has no WVR phase correction */
     ,
    AP_CORRECTED /*!< Data phases have been corrected using WVR data */
     
  };
  typedef AtmPhaseCorrection &AtmPhaseCorrection_out;
} 
#endif

namespace AtmPhaseCorrectionMod {
	std::ostream & operator << ( std::ostream & out, const AtmPhaseCorrection& value);
	std::istream & operator >> ( std::istream & in , AtmPhaseCorrection& value );
}

/** 
  * A helper class for the enumeration AtmPhaseCorrection.
  * 
  */
class CAtmPhaseCorrection {
  public:
 
	/**
	  * Enumerators as strings.
	  */  
	
	static const std::string& sAP_UNCORRECTED; /*!< A const string equal to "AP_UNCORRECTED".*/
	
	static const std::string& sAP_CORRECTED; /*!< A const string equal to "AP_CORRECTED".*/
	

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
       * Return the number of enumerators declared in AtmPhaseCorrectionMod::AtmPhaseCorrection.
       * @return an unsigned int.
       */
       static unsigned int size() ;
       
       
    /**
      * Returns an enumerator as a string.
      * @param e an enumerator of AtmPhaseCorrectionMod::AtmPhaseCorrection.
      * @return a string.
      */
	static std::string name(const AtmPhaseCorrectionMod::AtmPhaseCorrection& e);
	
	/**
	  * Equivalent to the name method.
	  */
    static std::string toString(const AtmPhaseCorrectionMod::AtmPhaseCorrection& f) { return name(f); }

	/** 
	  * Returns vector of  all the enumerators as strings. 
	  * The strings are stored in the vector in the same order than the enumerators are declared in the enumeration. 
	  * @return a vector of string.
	  */
     static const std::vector<std::string> names();	 
    
   	
   	// Create a AtmPhaseCorrection enumeration object by specifying its name.
   	static AtmPhaseCorrectionMod::AtmPhaseCorrection newAtmPhaseCorrection(const std::string& name);
   	
   	/*! Return a AtmPhaseCorrection's enumerator  given a string.
   	  * @param name the string representation of the enumerator.
   	 *  @return a AtmPhaseCorrectionMod::AtmPhaseCorrection's enumerator.
   	 *  @throws a string containing an error message if no enumerator could be found for this name.
   	 */
 	static AtmPhaseCorrectionMod::AtmPhaseCorrection literal(const std::string& name);
 	
    /*! Return a AtmPhaseCorrection's enumerator given an unsigned int.
      * @param i the index of the enumerator in AtmPhaseCorrectionMod::AtmPhaseCorrection.
      * @return a AtmPhaseCorrectionMod::AtmPhaseCorrection's enumerator.
      * @throws a string containing an error message if no enumerator could be found for this integer.
      */
 	static AtmPhaseCorrectionMod::AtmPhaseCorrection from_int(unsigned int i);	
 	

  private:
    /* Not Implemented.  This is a pure static class. */
    CAtmPhaseCorrection();
    CAtmPhaseCorrection(const CAtmPhaseCorrection&);
    CAtmPhaseCorrection& operator=(const CAtmPhaseCorrection&);
    
    static std::string badString(const std::string& name) ;
  	static std::string badInt(unsigned int i) ;
  	
};
 
#endif /*!CAtmPhaseCorrection_H*/
