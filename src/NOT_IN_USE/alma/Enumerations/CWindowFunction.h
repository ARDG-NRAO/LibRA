
#ifndef CWindowFunction_H
#define CWindowFunction_H

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
 * File CWindowFunction.h
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#include <iostream>
#include <string>
#include <vector>
/**
  * A namespace to encapsulate the WindowFunction enumeration.
  */
#ifndef WITHOUT_ACS
#include <almaEnumerations_IFC.h>
#else

// This part mimics the behaviour of 
namespace WindowFunctionMod
{
  //! WindowFunction.
  //! [APDM; ASDM.ALmaCorrelatorMode] Windowing functions for spectral data apodization 
  
  const char *const revision = "-1";
  const int version = 1;
  
  enum WindowFunction
  { 
    UNIFORM /*!< No windowing */
     ,
    HANNING /*!< Raised cosine: \f$0.5*(1-cos(x))\f$ where \f$x = 2*\pi*i/(N-1)\f$ */
     ,
    HAMMING /*!< The classic Hamming window is \f$W_M(x) = 0.54 - 0.46*\cos(x)\f$. This is generalized to \f$W_M(x) = \beta - (1-\beta)*\cos(x)\f$ where \f$\beta\f$ can take any value in the range \f$[0,1]\f$. \f$\beta=0.5\f$ corresponds to the Hanning window. */
     ,
    BARTLETT /*!< The Bartlett (triangular) window is \f$1 - |x/\pi|\f$, where \f$x = 2*\pi*i/(N-1)\f$. */
     ,
    BLACKMANN /*!< The window function is \f$W_B(x) = (0.5 - \beta) - 0.5*\cos(x_j) + \beta*\cos(2x_j)\f$, where \f$x_j=2*\pi*j/(N-1)\f$. The classic Blackman window is given by \f$\beta=0.08\f$. */
     ,
    BLACKMANN_HARRIS /*!< The BLACKMANN_HARRIS window is \f$1.0 - 1.36109*\cos(x) + 0.39381*\cos(2*x) - 0.032557*\cos(3*x)\f$, where \f$x = 2*\pi*i/(N-1)\f$. */
     ,
    WELCH /*!< The Welch window (parabolic) is \f$1 - (2*i/N)^2\f$. */
     
  };
  typedef WindowFunction &WindowFunction_out;
} 
#endif

namespace WindowFunctionMod {
	std::ostream & operator << ( std::ostream & out, const WindowFunction& value);
	std::istream & operator >> ( std::istream & in , WindowFunction& value );
}

/** 
  * A helper class for the enumeration WindowFunction.
  * 
  */
class CWindowFunction {
  public:
 
	/**
	  * Enumerators as strings.
	  */  
	
	static const std::string& sUNIFORM; /*!< A const string equal to "UNIFORM".*/
	
	static const std::string& sHANNING; /*!< A const string equal to "HANNING".*/
	
	static const std::string& sHAMMING; /*!< A const string equal to "HAMMING".*/
	
	static const std::string& sBARTLETT; /*!< A const string equal to "BARTLETT".*/
	
	static const std::string& sBLACKMANN; /*!< A const string equal to "BLACKMANN".*/
	
	static const std::string& sBLACKMANN_HARRIS; /*!< A const string equal to "BLACKMANN_HARRIS".*/
	
	static const std::string& sWELCH; /*!< A const string equal to "WELCH".*/
	

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
       * Return the number of enumerators declared in WindowFunctionMod::WindowFunction.
       * @return an unsigned int.
       */
       static unsigned int size() ;
       
       
    /**
      * Returns an enumerator as a string.
      * @param e an enumerator of WindowFunctionMod::WindowFunction.
      * @return a string.
      */
	static std::string name(const WindowFunctionMod::WindowFunction& e);
	
	/**
	  * Equivalent to the name method.
	  */
    static std::string toString(const WindowFunctionMod::WindowFunction& f) { return name(f); }

	/** 
	  * Returns vector of  all the enumerators as strings. 
	  * The strings are stored in the vector in the same order than the enumerators are declared in the enumeration. 
	  * @return a vector of string.
	  */
     static const std::vector<std::string> names();	 
    
   	
   	// Create a WindowFunction enumeration object by specifying its name.
   	static WindowFunctionMod::WindowFunction newWindowFunction(const std::string& name);
   	
   	/*! Return a WindowFunction's enumerator  given a string.
   	  * @param name the string representation of the enumerator.
   	 *  @return a WindowFunctionMod::WindowFunction's enumerator.
   	 *  @throws a string containing an error message if no enumerator could be found for this name.
   	 */
 	static WindowFunctionMod::WindowFunction literal(const std::string& name);
 	
    /*! Return a WindowFunction's enumerator given an unsigned int.
      * @param i the index of the enumerator in WindowFunctionMod::WindowFunction.
      * @return a WindowFunctionMod::WindowFunction's enumerator.
      * @throws a string containing an error message if no enumerator could be found for this integer.
      */
 	static WindowFunctionMod::WindowFunction from_int(unsigned int i);	
 	

  private:
    /* Not Implemented.  This is a pure static class. */
    CWindowFunction();
    CWindowFunction(const CWindowFunction&);
    CWindowFunction& operator=(const CWindowFunction&);
    
    static std::string badString(const std::string& name) ;
  	static std::string badInt(unsigned int i) ;
  	
};
 
#endif /*!CWindowFunction_H*/
