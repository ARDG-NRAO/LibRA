
#ifndef CReceiverBand_H
#define CReceiverBand_H

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
 * File CReceiverBand.h
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#include <iostream>
#include <string>
#include <vector>
/**
  * A namespace to encapsulate the ReceiverBand enumeration.
  */
#ifndef WITHOUT_ACS
#include <almaEnumerations_IFC.h>
#else

// This part mimics the behaviour of 
namespace ReceiverBandMod
{
  //! ReceiverBand.
  //!  [ASDM.Receiver] Receiver band names
  
  const char *const revision = "-1";
  const int version = 1;
  
  enum ReceiverBand
  { 
    ALMA_RB_01 /*!< ALMA Receiver band 01 */
     ,
    ALMA_RB_02 /*!< ALMA Receiver band 02 */
     ,
    ALMA_RB_03 /*!< ALMA Receiver band 03 */
     ,
    ALMA_RB_04 /*!< ALMA Receiver band 04 */
     ,
    ALMA_RB_05 /*!< ALMA Receiver band 05 */
     ,
    ALMA_RB_06 /*!< ALMA Receiver band 06 */
     ,
    ALMA_RB_07 /*!< ALMA Receiver band 07 */
     ,
    ALMA_RB_08 /*!< ALMA Receiver band 08 */
     ,
    ALMA_RB_09 /*!< ALMA Receiver band 09 */
     ,
    ALMA_RB_10 /*!< ALMA Receiver band 10 */
     ,
    ALMA_RB_ALL /*!< all ALMA receiver bands. */
     ,
    ALMA_HOLOGRAPHY_RECEIVER /*!< Alma transmitter Holography receiver. */
     ,
    BURE_01 /*!< Plateau de Bure receiver band #1. */
     ,
    BURE_02 /*!< Plateau de Bure receiver band #2. */
     ,
    BURE_03 /*!< Plateau de Bure receiver band #3. */
     ,
    BURE_04 /*!< Plateau de Bure receiver band #4 */
     ,
    EVLA_4 /*!<  */
     ,
    EVLA_P /*!<  */
     ,
    EVLA_L /*!<  */
     ,
    EVLA_C /*!<  */
     ,
    EVLA_S /*!<  */
     ,
    EVLA_X /*!<  */
     ,
    EVLA_Ku /*!<  */
     ,
    EVLA_K /*!<  */
     ,
    EVLA_Ka /*!<  */
     ,
    EVLA_Q /*!<  */
     ,
    UNSPECIFIED /*!< receiver band of unspecified origin. */
     
  };
  typedef ReceiverBand &ReceiverBand_out;
} 
#endif

namespace ReceiverBandMod {
	std::ostream & operator << ( std::ostream & out, const ReceiverBand& value);
	std::istream & operator >> ( std::istream & in , ReceiverBand& value );
}

/** 
  * A helper class for the enumeration ReceiverBand.
  * 
  */
class CReceiverBand {
  public:
 
	/**
	  * Enumerators as strings.
	  */  
	
	static const std::string& sALMA_RB_01; /*!< A const string equal to "ALMA_RB_01".*/
	
	static const std::string& sALMA_RB_02; /*!< A const string equal to "ALMA_RB_02".*/
	
	static const std::string& sALMA_RB_03; /*!< A const string equal to "ALMA_RB_03".*/
	
	static const std::string& sALMA_RB_04; /*!< A const string equal to "ALMA_RB_04".*/
	
	static const std::string& sALMA_RB_05; /*!< A const string equal to "ALMA_RB_05".*/
	
	static const std::string& sALMA_RB_06; /*!< A const string equal to "ALMA_RB_06".*/
	
	static const std::string& sALMA_RB_07; /*!< A const string equal to "ALMA_RB_07".*/
	
	static const std::string& sALMA_RB_08; /*!< A const string equal to "ALMA_RB_08".*/
	
	static const std::string& sALMA_RB_09; /*!< A const string equal to "ALMA_RB_09".*/
	
	static const std::string& sALMA_RB_10; /*!< A const string equal to "ALMA_RB_10".*/
	
	static const std::string& sALMA_RB_ALL; /*!< A const string equal to "ALMA_RB_ALL".*/
	
	static const std::string& sALMA_HOLOGRAPHY_RECEIVER; /*!< A const string equal to "ALMA_HOLOGRAPHY_RECEIVER".*/
	
	static const std::string& sBURE_01; /*!< A const string equal to "BURE_01".*/
	
	static const std::string& sBURE_02; /*!< A const string equal to "BURE_02".*/
	
	static const std::string& sBURE_03; /*!< A const string equal to "BURE_03".*/
	
	static const std::string& sBURE_04; /*!< A const string equal to "BURE_04".*/
	
	static const std::string& sEVLA_4; /*!< A const string equal to "EVLA_4".*/
	
	static const std::string& sEVLA_P; /*!< A const string equal to "EVLA_P".*/
	
	static const std::string& sEVLA_L; /*!< A const string equal to "EVLA_L".*/
	
	static const std::string& sEVLA_C; /*!< A const string equal to "EVLA_C".*/
	
	static const std::string& sEVLA_S; /*!< A const string equal to "EVLA_S".*/
	
	static const std::string& sEVLA_X; /*!< A const string equal to "EVLA_X".*/
	
	static const std::string& sEVLA_Ku; /*!< A const string equal to "EVLA_Ku".*/
	
	static const std::string& sEVLA_K; /*!< A const string equal to "EVLA_K".*/
	
	static const std::string& sEVLA_Ka; /*!< A const string equal to "EVLA_Ka".*/
	
	static const std::string& sEVLA_Q; /*!< A const string equal to "EVLA_Q".*/
	
	static const std::string& sUNSPECIFIED; /*!< A const string equal to "UNSPECIFIED".*/
	

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
       * Return the number of enumerators declared in ReceiverBandMod::ReceiverBand.
       * @return an unsigned int.
       */
       static unsigned int size() ;
       
       
    /**
      * Returns an enumerator as a string.
      * @param e an enumerator of ReceiverBandMod::ReceiverBand.
      * @return a string.
      */
	static std::string name(const ReceiverBandMod::ReceiverBand& e);
	
	/**
	  * Equivalent to the name method.
	  */
    static std::string toString(const ReceiverBandMod::ReceiverBand& f) { return name(f); }

	/** 
	  * Returns vector of  all the enumerators as strings. 
	  * The strings are stored in the vector in the same order than the enumerators are declared in the enumeration. 
	  * @return a vector of string.
	  */
     static const std::vector<std::string> names();	 
    
   	
   	// Create a ReceiverBand enumeration object by specifying its name.
   	static ReceiverBandMod::ReceiverBand newReceiverBand(const std::string& name);
   	
   	/*! Return a ReceiverBand's enumerator  given a string.
   	  * @param name the string representation of the enumerator.
   	 *  @return a ReceiverBandMod::ReceiverBand's enumerator.
   	 *  @throws a string containing an error message if no enumerator could be found for this name.
   	 */
 	static ReceiverBandMod::ReceiverBand literal(const std::string& name);
 	
    /*! Return a ReceiverBand's enumerator given an unsigned int.
      * @param i the index of the enumerator in ReceiverBandMod::ReceiverBand.
      * @return a ReceiverBandMod::ReceiverBand's enumerator.
      * @throws a string containing an error message if no enumerator could be found for this integer.
      */
 	static ReceiverBandMod::ReceiverBand from_int(unsigned int i);	
 	

  private:
    /* Not Implemented.  This is a pure static class. */
    CReceiverBand();
    CReceiverBand(const CReceiverBand&);
    CReceiverBand& operator=(const CReceiverBand&);
    
    static std::string badString(const std::string& name) ;
  	static std::string badInt(unsigned int i) ;
  	
};
 
#endif /*!CReceiverBand_H*/
