
#ifndef CAxisName_H
#define CAxisName_H

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
 * File CAxisName.h
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#include <iostream>
#include <string>
#include <vector>
/**
  * A namespace to encapsulate the AxisName enumeration.
  */
#ifndef WITHOUT_ACS
#include <almaEnumerations_IFC.h>
#else

// This part mimics the behaviour of 
namespace AxisNameMod
{
  //! AxisName.
  //!  Axis names.
  
  const char *const revision = "-1";
  const int version = 1;
  
  enum AxisName
  { 
    TIM /*!< Time axis. */
     ,
    BAL /*!< Baseline axis. */
     ,
    ANT /*!< Antenna axis. */
     ,
    BAB /*!< Baseband axis. */
     ,
    SPW /*!< Spectral window  axis. */
     ,
    SIB /*!< Sideband axis. */
     ,
    SUB /*!< Subband axis. */
     ,
    BIN /*!< Bin axis. */
     ,
    APC /*!< Atmosphere phase correction axis. */
     ,
    SPP /*!< Spectral point axis. */
     ,
    POL /*!< Polarization axis (Stokes parameters). */
     ,
    STO /*!< Stokes parameter axis. */
     ,
    HOL /*!< Holography axis. */
     
  };
  typedef AxisName &AxisName_out;
} 
#endif

namespace AxisNameMod {
	std::ostream & operator << ( std::ostream & out, const AxisName& value);
	std::istream & operator >> ( std::istream & in , AxisName& value );
}

/** 
  * A helper class for the enumeration AxisName.
  * 
  */
class CAxisName {
  public:
 
	/**
	  * Enumerators as strings.
	  */  
	
	static const std::string& sTIM; /*!< A const string equal to "TIM".*/
	
	static const std::string& sBAL; /*!< A const string equal to "BAL".*/
	
	static const std::string& sANT; /*!< A const string equal to "ANT".*/
	
	static const std::string& sBAB; /*!< A const string equal to "BAB".*/
	
	static const std::string& sSPW; /*!< A const string equal to "SPW".*/
	
	static const std::string& sSIB; /*!< A const string equal to "SIB".*/
	
	static const std::string& sSUB; /*!< A const string equal to "SUB".*/
	
	static const std::string& sBIN; /*!< A const string equal to "BIN".*/
	
	static const std::string& sAPC; /*!< A const string equal to "APC".*/
	
	static const std::string& sSPP; /*!< A const string equal to "SPP".*/
	
	static const std::string& sPOL; /*!< A const string equal to "POL".*/
	
	static const std::string& sSTO; /*!< A const string equal to "STO".*/
	
	static const std::string& sHOL; /*!< A const string equal to "HOL".*/
	

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
       * Return the number of enumerators declared in AxisNameMod::AxisName.
       * @return an unsigned int.
       */
       static unsigned int size() ;
       
       
    /**
      * Returns an enumerator as a string.
      * @param e an enumerator of AxisNameMod::AxisName.
      * @return a string.
      */
	static std::string name(const AxisNameMod::AxisName& e);
	
	/**
	  * Equivalent to the name method.
	  */
    static std::string toString(const AxisNameMod::AxisName& f) { return name(f); }

	/** 
	  * Returns vector of  all the enumerators as strings. 
	  * The strings are stored in the vector in the same order than the enumerators are declared in the enumeration. 
	  * @return a vector of string.
	  */
     static const std::vector<std::string> names();	 
    
   	
   	// Create a AxisName enumeration object by specifying its name.
   	static AxisNameMod::AxisName newAxisName(const std::string& name);
   	
   	/*! Return a AxisName's enumerator  given a string.
   	  * @param name the string representation of the enumerator.
   	 *  @return a AxisNameMod::AxisName's enumerator.
   	 *  @throws a string containing an error message if no enumerator could be found for this name.
   	 */
 	static AxisNameMod::AxisName literal(const std::string& name);
 	
    /*! Return a AxisName's enumerator given an unsigned int.
      * @param i the index of the enumerator in AxisNameMod::AxisName.
      * @return a AxisNameMod::AxisName's enumerator.
      * @throws a string containing an error message if no enumerator could be found for this integer.
      */
 	static AxisNameMod::AxisName from_int(unsigned int i);	
 	

  private:
    /* Not Implemented.  This is a pure static class. */
    CAxisName();
    CAxisName(const CAxisName&);
    CAxisName& operator=(const CAxisName&);
    
    static std::string badString(const std::string& name) ;
  	static std::string badInt(unsigned int i) ;
  	
};
 
#endif /*!CAxisName_H*/
