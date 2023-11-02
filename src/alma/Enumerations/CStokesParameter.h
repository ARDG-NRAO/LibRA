
#ifndef CStokesParameter_H
#define CStokesParameter_H

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
 * File CStokesParameter.h
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#include <iostream>
#include <string>
#include <vector>
/**
  * A namespace to encapsulate the StokesParameter enumeration.
  */
#ifndef WITHOUT_ACS
#include <almaEnumerations_IFC.h>
#else

// This part mimics the behaviour of 
namespace StokesParameterMod
{
  //! StokesParameter.
  //!  Stokes parameters (CASA definition)
  
  const char *const revision = "-1";
  const int version = 1;
  
  enum StokesParameter
  { 
    I /*!<  */
     ,
    Q /*!<  */
     ,
    U /*!<  */
     ,
    V /*!<  */
     ,
    RR /*!<  */
     ,
    RL /*!<  */
     ,
    LR /*!<  */
     ,
    LL /*!<  */
     ,
    XX /*!< Linear correlation product */
     ,
    XY /*!<  */
     ,
    YX /*!<  */
     ,
    YY /*!<  */
     ,
    RX /*!< Mixed correlation product */
     ,
    RY /*!< Mixed correlation product */
     ,
    LX /*!< Mixed LX product */
     ,
    LY /*!< Mixed LY correlation product */
     ,
    XR /*!< Mixed XR correlation product */
     ,
    XL /*!< Mixed XL correlation product */
     ,
    YR /*!< Mixed YR correlation product */
     ,
    YL /*!< Mixel YL correlation product */
     ,
    PP /*!<  */
     ,
    PQ /*!<  */
     ,
    QP /*!<  */
     ,
    QQ /*!<  */
     ,
    RCIRCULAR /*!<  */
     ,
    LCIRCULAR /*!<  */
     ,
    LINEAR /*!< single dish polarization type */
     ,
    PTOTAL /*!< Polarized intensity ((Q^2+U^2+V^2)^(1/2)) */
     ,
    PLINEAR /*!< Linearly Polarized intensity ((Q^2+U^2)^(1/2)) */
     ,
    PFTOTAL /*!< Polarization Fraction (Ptotal/I) */
     ,
    PFLINEAR /*!< Linear Polarization Fraction (Plinear/I) */
     ,
    PANGLE /*!< Linear Polarization Angle (0.5 arctan(U/Q)) (in radians) */
     
  };
  typedef StokesParameter &StokesParameter_out;
} 
#endif

namespace StokesParameterMod {
	std::ostream & operator << ( std::ostream & out, const StokesParameter& value);
	std::istream & operator >> ( std::istream & in , StokesParameter& value );
}

/** 
  * A helper class for the enumeration StokesParameter.
  * 
  */
class CStokesParameter {
  public:
 
	/**
	  * Enumerators as strings.
	  */  
	
	static const std::string& sI; /*!< A const string equal to "I".*/
	
	static const std::string& sQ; /*!< A const string equal to "Q".*/
	
	static const std::string& sU; /*!< A const string equal to "U".*/
	
	static const std::string& sV; /*!< A const string equal to "V".*/
	
	static const std::string& sRR; /*!< A const string equal to "RR".*/
	
	static const std::string& sRL; /*!< A const string equal to "RL".*/
	
	static const std::string& sLR; /*!< A const string equal to "LR".*/
	
	static const std::string& sLL; /*!< A const string equal to "LL".*/
	
	static const std::string& sXX; /*!< A const string equal to "XX".*/
	
	static const std::string& sXY; /*!< A const string equal to "XY".*/
	
	static const std::string& sYX; /*!< A const string equal to "YX".*/
	
	static const std::string& sYY; /*!< A const string equal to "YY".*/
	
	static const std::string& sRX; /*!< A const string equal to "RX".*/
	
	static const std::string& sRY; /*!< A const string equal to "RY".*/
	
	static const std::string& sLX; /*!< A const string equal to "LX".*/
	
	static const std::string& sLY; /*!< A const string equal to "LY".*/
	
	static const std::string& sXR; /*!< A const string equal to "XR".*/
	
	static const std::string& sXL; /*!< A const string equal to "XL".*/
	
	static const std::string& sYR; /*!< A const string equal to "YR".*/
	
	static const std::string& sYL; /*!< A const string equal to "YL".*/
	
	static const std::string& sPP; /*!< A const string equal to "PP".*/
	
	static const std::string& sPQ; /*!< A const string equal to "PQ".*/
	
	static const std::string& sQP; /*!< A const string equal to "QP".*/
	
	static const std::string& sQQ; /*!< A const string equal to "QQ".*/
	
	static const std::string& sRCIRCULAR; /*!< A const string equal to "RCIRCULAR".*/
	
	static const std::string& sLCIRCULAR; /*!< A const string equal to "LCIRCULAR".*/
	
	static const std::string& sLINEAR; /*!< A const string equal to "LINEAR".*/
	
	static const std::string& sPTOTAL; /*!< A const string equal to "PTOTAL".*/
	
	static const std::string& sPLINEAR; /*!< A const string equal to "PLINEAR".*/
	
	static const std::string& sPFTOTAL; /*!< A const string equal to "PFTOTAL".*/
	
	static const std::string& sPFLINEAR; /*!< A const string equal to "PFLINEAR".*/
	
	static const std::string& sPANGLE; /*!< A const string equal to "PANGLE".*/
	

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
       * Return the number of enumerators declared in StokesParameterMod::StokesParameter.
       * @return an unsigned int.
       */
       static unsigned int size() ;
       
       
    /**
      * Returns an enumerator as a string.
      * @param e an enumerator of StokesParameterMod::StokesParameter.
      * @return a string.
      */
	static std::string name(const StokesParameterMod::StokesParameter& e);
	
	/**
	  * Equivalent to the name method.
	  */
    static std::string toString(const StokesParameterMod::StokesParameter& f) { return name(f); }

	/** 
	  * Returns vector of  all the enumerators as strings. 
	  * The strings are stored in the vector in the same order than the enumerators are declared in the enumeration. 
	  * @return a vector of string.
	  */
     static const std::vector<std::string> names();	 
    
   	
   	// Create a StokesParameter enumeration object by specifying its name.
   	static StokesParameterMod::StokesParameter newStokesParameter(const std::string& name);
   	
   	/*! Return a StokesParameter's enumerator  given a string.
   	  * @param name the string representation of the enumerator.
   	 *  @return a StokesParameterMod::StokesParameter's enumerator.
   	 *  @throws a string containing an error message if no enumerator could be found for this name.
   	 */
 	static StokesParameterMod::StokesParameter literal(const std::string& name);
 	
    /*! Return a StokesParameter's enumerator given an unsigned int.
      * @param i the index of the enumerator in StokesParameterMod::StokesParameter.
      * @return a StokesParameterMod::StokesParameter's enumerator.
      * @throws a string containing an error message if no enumerator could be found for this integer.
      */
 	static StokesParameterMod::StokesParameter from_int(unsigned int i);	
 	

  private:
    /* Not Implemented.  This is a pure static class. */
    CStokesParameter();
    CStokesParameter(const CStokesParameter&);
    CStokesParameter& operator=(const CStokesParameter&);
    
    static std::string badString(const std::string& name) ;
  	static std::string badInt(unsigned int i) ;
  	
};
 
#endif /*!CStokesParameter_H*/
