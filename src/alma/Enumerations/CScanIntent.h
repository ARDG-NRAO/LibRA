
#ifndef CScanIntent_H
#define CScanIntent_H

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
 * File CScanIntent.h
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#include <iostream>
#include <string>
#include <vector>
/**
  * A namespace to encapsulate the ScanIntent enumeration.
  */
#ifndef WITHOUT_ACS
#include <almaEnumerations_IFC.h>
#else

// This part mimics the behaviour of 
namespace ScanIntentMod
{
  //! ScanIntent.
  //!  [ASDM.Scan] Scan intents
  
  const char *const revision = "-1";
  const int version = 1;
  
  enum ScanIntent
  { 
    CALIBRATE_AMPLI /*!< Amplitude calibration scan */
     ,
    CALIBRATE_ATMOSPHERE /*!< Atmosphere calibration scan */
     ,
    CALIBRATE_BANDPASS /*!< Bandpass calibration scan */
     ,
    CALIBRATE_DELAY /*!< Delay calibration scan */
     ,
    CALIBRATE_FLUX /*!< flux measurement scan. */
     ,
    CALIBRATE_FOCUS /*!< Focus calibration scan. Z coordinate to be derived */
     ,
    CALIBRATE_FOCUS_X /*!< Focus calibration scan; X focus coordinate to be derived */
     ,
    CALIBRATE_FOCUS_Y /*!< Focus calibration scan; Y focus coordinate to be derived */
     ,
    CALIBRATE_PHASE /*!< Phase calibration scan */
     ,
    CALIBRATE_POINTING /*!< Pointing calibration scan */
     ,
    CALIBRATE_POLARIZATION /*!< Polarization calibration scan */
     ,
    CALIBRATE_SIDEBAND_RATIO /*!< measure relative gains of sidebands. */
     ,
    CALIBRATE_WVR /*!< Data from the water vapor radiometers (and correlation data) are used to derive their calibration parameters. */
     ,
    DO_SKYDIP /*!< Skydip calibration scan */
     ,
    MAP_ANTENNA_SURFACE /*!< Holography calibration scan */
     ,
    MAP_PRIMARY_BEAM /*!< Data on a celestial calibration source are used to derive a map of the primary beam. */
     ,
    OBSERVE_TARGET /*!< Target source scan */
     ,
    CALIBRATE_POL_LEAKAGE /*!<  */
     ,
    CALIBRATE_POL_ANGLE /*!<  */
     ,
    TEST /*!< used for development. */
     ,
    UNSPECIFIED /*!< Unspecified scan intent */
     ,
    CALIBRATE_ANTENNA_POSITION /*!< Requested by EVLA. */
     ,
    CALIBRATE_ANTENNA_PHASE /*!< Requested by EVLA. */
     ,
    MEASURE_RFI /*!< Requested by EVLA. */
     ,
    CALIBRATE_ANTENNA_POINTING_MODEL /*!< Requested by EVLA. */
     ,
    SYSTEM_CONFIGURATION /*!< Requested by EVLA. */
     ,
    CALIBRATE_APPPHASE_ACTIVE /*!< Calculate and apply phasing solutions. Applicable at ALMA. */
     ,
    CALIBRATE_APPPHASE_PASSIVE /*!< Apply previously obtained phasing solutions. Applicable at ALMA. */
     ,
    OBSERVE_CHECK_SOURCE /*!<  */
     ,
    CALIBRATE_DIFFGAIN /*!< Enable a gain differential target type */
     
  };
  typedef ScanIntent &ScanIntent_out;
} 
#endif

namespace ScanIntentMod {
	std::ostream & operator << ( std::ostream & out, const ScanIntent& value);
	std::istream & operator >> ( std::istream & in , ScanIntent& value );
}

/** 
  * A helper class for the enumeration ScanIntent.
  * 
  */
class CScanIntent {
  public:
 
	/**
	  * Enumerators as strings.
	  */  
	
	static const std::string& sCALIBRATE_AMPLI; /*!< A const string equal to "CALIBRATE_AMPLI".*/
	
	static const std::string& sCALIBRATE_ATMOSPHERE; /*!< A const string equal to "CALIBRATE_ATMOSPHERE".*/
	
	static const std::string& sCALIBRATE_BANDPASS; /*!< A const string equal to "CALIBRATE_BANDPASS".*/
	
	static const std::string& sCALIBRATE_DELAY; /*!< A const string equal to "CALIBRATE_DELAY".*/
	
	static const std::string& sCALIBRATE_FLUX; /*!< A const string equal to "CALIBRATE_FLUX".*/
	
	static const std::string& sCALIBRATE_FOCUS; /*!< A const string equal to "CALIBRATE_FOCUS".*/
	
	static const std::string& sCALIBRATE_FOCUS_X; /*!< A const string equal to "CALIBRATE_FOCUS_X".*/
	
	static const std::string& sCALIBRATE_FOCUS_Y; /*!< A const string equal to "CALIBRATE_FOCUS_Y".*/
	
	static const std::string& sCALIBRATE_PHASE; /*!< A const string equal to "CALIBRATE_PHASE".*/
	
	static const std::string& sCALIBRATE_POINTING; /*!< A const string equal to "CALIBRATE_POINTING".*/
	
	static const std::string& sCALIBRATE_POLARIZATION; /*!< A const string equal to "CALIBRATE_POLARIZATION".*/
	
	static const std::string& sCALIBRATE_SIDEBAND_RATIO; /*!< A const string equal to "CALIBRATE_SIDEBAND_RATIO".*/
	
	static const std::string& sCALIBRATE_WVR; /*!< A const string equal to "CALIBRATE_WVR".*/
	
	static const std::string& sDO_SKYDIP; /*!< A const string equal to "DO_SKYDIP".*/
	
	static const std::string& sMAP_ANTENNA_SURFACE; /*!< A const string equal to "MAP_ANTENNA_SURFACE".*/
	
	static const std::string& sMAP_PRIMARY_BEAM; /*!< A const string equal to "MAP_PRIMARY_BEAM".*/
	
	static const std::string& sOBSERVE_TARGET; /*!< A const string equal to "OBSERVE_TARGET".*/
	
	static const std::string& sCALIBRATE_POL_LEAKAGE; /*!< A const string equal to "CALIBRATE_POL_LEAKAGE".*/
	
	static const std::string& sCALIBRATE_POL_ANGLE; /*!< A const string equal to "CALIBRATE_POL_ANGLE".*/
	
	static const std::string& sTEST; /*!< A const string equal to "TEST".*/
	
	static const std::string& sUNSPECIFIED; /*!< A const string equal to "UNSPECIFIED".*/
	
	static const std::string& sCALIBRATE_ANTENNA_POSITION; /*!< A const string equal to "CALIBRATE_ANTENNA_POSITION".*/
	
	static const std::string& sCALIBRATE_ANTENNA_PHASE; /*!< A const string equal to "CALIBRATE_ANTENNA_PHASE".*/
	
	static const std::string& sMEASURE_RFI; /*!< A const string equal to "MEASURE_RFI".*/
	
	static const std::string& sCALIBRATE_ANTENNA_POINTING_MODEL; /*!< A const string equal to "CALIBRATE_ANTENNA_POINTING_MODEL".*/
	
	static const std::string& sSYSTEM_CONFIGURATION; /*!< A const string equal to "SYSTEM_CONFIGURATION".*/
	
	static const std::string& sCALIBRATE_APPPHASE_ACTIVE; /*!< A const string equal to "CALIBRATE_APPPHASE_ACTIVE".*/
	
	static const std::string& sCALIBRATE_APPPHASE_PASSIVE; /*!< A const string equal to "CALIBRATE_APPPHASE_PASSIVE".*/
	
	static const std::string& sOBSERVE_CHECK_SOURCE; /*!< A const string equal to "OBSERVE_CHECK_SOURCE".*/
	
	static const std::string& sCALIBRATE_DIFFGAIN; /*!< A const string equal to "CALIBRATE_DIFFGAIN".*/
	

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
       * Return the number of enumerators declared in ScanIntentMod::ScanIntent.
       * @return an unsigned int.
       */
       static unsigned int size() ;
       
       
    /**
      * Returns an enumerator as a string.
      * @param e an enumerator of ScanIntentMod::ScanIntent.
      * @return a string.
      */
	static std::string name(const ScanIntentMod::ScanIntent& e);
	
	/**
	  * Equivalent to the name method.
	  */
    static std::string toString(const ScanIntentMod::ScanIntent& f) { return name(f); }

	/** 
	  * Returns vector of  all the enumerators as strings. 
	  * The strings are stored in the vector in the same order than the enumerators are declared in the enumeration. 
	  * @return a vector of string.
	  */
     static const std::vector<std::string> names();	 
    
   	
   	// Create a ScanIntent enumeration object by specifying its name.
   	static ScanIntentMod::ScanIntent newScanIntent(const std::string& name);
   	
   	/*! Return a ScanIntent's enumerator  given a string.
   	  * @param name the string representation of the enumerator.
   	 *  @return a ScanIntentMod::ScanIntent's enumerator.
   	 *  @throws a string containing an error message if no enumerator could be found for this name.
   	 */
 	static ScanIntentMod::ScanIntent literal(const std::string& name);
 	
    /*! Return a ScanIntent's enumerator given an unsigned int.
      * @param i the index of the enumerator in ScanIntentMod::ScanIntent.
      * @return a ScanIntentMod::ScanIntent's enumerator.
      * @throws a string containing an error message if no enumerator could be found for this integer.
      */
 	static ScanIntentMod::ScanIntent from_int(unsigned int i);	
 	

  private:
    /* Not Implemented.  This is a pure static class. */
    CScanIntent();
    CScanIntent(const CScanIntent&);
    CScanIntent& operator=(const CScanIntent&);
    
    static std::string badString(const std::string& name) ;
  	static std::string badInt(unsigned int i) ;
  	
};
 
#endif /*!CScanIntent_H*/
