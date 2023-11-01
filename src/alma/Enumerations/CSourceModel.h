
#ifndef CSourceModel_H
#define CSourceModel_H

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
 * File CSourceModel.h
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#include <iostream>
#include <string>
#include <vector>
/**
  * A namespace to encapsulate the SourceModel enumeration.
  */
#ifndef WITHOUT_ACS
#include <almaEnumerations_IFC.h>
#else

// This part mimics the behaviour of 
namespace SourceModelMod
{
  //! SourceModel.
  //!  [CalDM.CalFlux] Source Model
  
  const char *const revision = "-1";
  const int version = 1;
  
  enum SourceModel
  { 
    GAUSSIAN /*!< Gaussian source */
     ,
    POINT /*!< Point Source */
     ,
    DISK /*!< Uniform Disk */
     
  };
  typedef SourceModel &SourceModel_out;
} 
#endif

namespace SourceModelMod {
	std::ostream & operator << ( std::ostream & out, const SourceModel& value);
	std::istream & operator >> ( std::istream & in , SourceModel& value );
}

/** 
  * A helper class for the enumeration SourceModel.
  * 
  */
class CSourceModel {
  public:
 
	/**
	  * Enumerators as strings.
	  */  
	
	static const std::string& sGAUSSIAN; /*!< A const string equal to "GAUSSIAN".*/
	
	static const std::string& sPOINT; /*!< A const string equal to "POINT".*/
	
	static const std::string& sDISK; /*!< A const string equal to "DISK".*/
	

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
       * Return the number of enumerators declared in SourceModelMod::SourceModel.
       * @return an unsigned int.
       */
       static unsigned int size() ;
       
       
    /**
      * Returns an enumerator as a string.
      * @param e an enumerator of SourceModelMod::SourceModel.
      * @return a string.
      */
	static std::string name(const SourceModelMod::SourceModel& e);
	
	/**
	  * Equivalent to the name method.
	  */
    static std::string toString(const SourceModelMod::SourceModel& f) { return name(f); }

	/** 
	  * Returns vector of  all the enumerators as strings. 
	  * The strings are stored in the vector in the same order than the enumerators are declared in the enumeration. 
	  * @return a vector of string.
	  */
     static const std::vector<std::string> names();	 
    
   	
   	// Create a SourceModel enumeration object by specifying its name.
   	static SourceModelMod::SourceModel newSourceModel(const std::string& name);
   	
   	/*! Return a SourceModel's enumerator  given a string.
   	  * @param name the string representation of the enumerator.
   	 *  @return a SourceModelMod::SourceModel's enumerator.
   	 *  @throws a string containing an error message if no enumerator could be found for this name.
   	 */
 	static SourceModelMod::SourceModel literal(const std::string& name);
 	
    /*! Return a SourceModel's enumerator given an unsigned int.
      * @param i the index of the enumerator in SourceModelMod::SourceModel.
      * @return a SourceModelMod::SourceModel's enumerator.
      * @throws a string containing an error message if no enumerator could be found for this integer.
      */
 	static SourceModelMod::SourceModel from_int(unsigned int i);	
 	

  private:
    /* Not Implemented.  This is a pure static class. */
    CSourceModel();
    CSourceModel(const CSourceModel&);
    CSourceModel& operator=(const CSourceModel&);
    
    static std::string badString(const std::string& name) ;
  	static std::string badInt(unsigned int i) ;
  	
};
 
#endif /*!CSourceModel_H*/
