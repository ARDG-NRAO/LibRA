
#ifndef CBinaryDataFlags_H
#define CBinaryDataFlags_H

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
 * File CBinaryDataFlags.h
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#include <iostream>
#include <string>
#include <vector>
/**
  * A namespace to encapsulate the BinaryDataFlags enumeration.
  */
#ifndef WITHOUT_ACS
#include <almaEnumerations_IFC.h>
#else

// This part mimics the behaviour of 
namespace BinaryDataFlagsMod
{
  //! BinaryDataFlags.
  //! This enumeration declares an ordered list of  flagging conditions used to build the flag part in the BDF content.  Each enumerator is associated to one bit in a bitset. A bit set to one (resp. zero) means that the corresponding flagging condition is set (resp. unset). The current convention limits  the length of the enumeration to 32; the position (0-based) of the enumerator in the enumeration defines the bit position. Any bit whose position is greater or equal to the length of the enumeration and less than 32 should be ignored by the software since it does not correspond to any flagging condition. 
  
  const char *const revision = "-1";
  const int version = 1;
  
  enum BinaryDataFlags
  { 
    INTEGRATION_FULLY_BLANKED /*!< All dumps within an integration duration are blanked. When this flag is raised the effect is to have the bin part actualDurations containing zeros? In other words it means 'all dumps affected'.  Bit position \f$==0\f$ */
     ,
    WVR_APC /*!< Coefficients not received.Apply to all BAL involving the antenna. Bit position \f$==1\f$ */
     ,
    CORRELATOR_MISSING_STATUS /*!< Correlator status was not retrieved for the period. So  yielded data are not reliable. Apply to all  BBs handled by the correlator. Bit position \f$==2\f$ */
     ,
    MISSING_ANTENNA_EVENT /*!< Antenna delay event was not retrieved for the period. So  yielded data are not reliable. BALs including the antenna. Bit position \f$==3\f$ */
     ,
    DELTA_SIGMA_OVERFLOW /*!< In data transmission between the MTI cards, there are one or more channels whose absolute value differences between adjacent channel values are bigger than the maximum number. Bit position \f$==4\f$ */
     ,
    DELAY_CORRECTION_NOT_APPLIED /*!< no residual delay correction was applied. It implies that either base-band offset delays from TMCDB were not available or that delay events from the delay server were not received on time to compute and apply a phase rotation to base-lines in the array. \f$==5\f$ */
     ,
    SYNCRONIZATION_ERROR /*!< cdp node(s) not properly synchronized to the array timing signal (48ms.) All data produced by that node(s) are suspicious.Lags and spectral processing goes as normal, it is just the flag presence in the bdf what indicates that something is suspicious. Bit position \f$==6\f$ */
     ,
    FFT_OVERFLOW /*!< Overflowed POL and derived outputs from it. Dumps between the timestamp marked as FFT overflowed and the time back to 96msec before. Bit position \f$==7\f$ */
     ,
    TFB_SCALING_FACTOR_NOT_RETRIEVED /*!< CCC cannot retrieve scaling factors during calibration for specific antennas the calibration would still end successfully but the cdp will record the faulty scaling
 factors and add a flag to all involved base-lines. Bit position \f$==8\f$ */
     ,
    ZERO_LAG_NOT_RECEIVED /*!< CDP node handling only cross antenna intersections did  not receive lag zero information from node(s) handling auto intersections for involved antennas in that cross intersection. Bit position \f$==9\f$ */
     ,
    SIGMA_OVERFLOW /*!< Auto-correlation sigma levels makes impossible any 2 bits quantization correction on lags data. One sigma value out of range affects that antenna itself and all base-lines containing that antenna. Is it possible to merge this flags with DELTA_SIGMA_OVERFLOW? The difference seems to be the granularity. If it is POL ACACORR would have to repeat the flag for every POL  because per baseband there are several POL. Bit position \f$==10\f$ */
     ,
    UNUSABLE_CAI_OUTPUT /*!< The output spectra are made from invalid input signals, e.g., broken optical frames, missing synchronization or no input signal power. Bit position \f$==11\f$ */
     ,
    QC_FAILED /*!< Quantization correction not applied due to unsuitable lag zero value. BL-CORR note: every possible signal level should be actually accepted (too small or too big), the presence of this bit signals more a software problem than an antenna signal problem. Bit position \f$==12\f$ */
     ,
    NOISY_TDM_CHANNELS /*!< First TDM channels are normally noisy and they have a  large amplitude. If that excess of amplitude in those channels would be the sole reason for keeping the integration storage at 32 bits integers then the software clips those channels and flags the data. Thus preventing large storage for otherwise 16 bits friendly dynamic range. Bit position \f$==13\f$ */
     ,
    SPECTRAL_NORMALIZATION_FAILED /*!< Auto-correlation and zero-lags figures are required to normalize cross-correlation spectra as prescribed in Scott's 'Specifications and Clarifications of ALMA Correlator Details'. If those figures are not available on time during on-line processing then crosscorrelations are not normalized and the integration flagged. Bit position \f$==14\f$ */
     ,
    DROPPED_PACKETS /*!< T.B.D. Bit position \f$==15\f$ */
     ,
    DETECTOR_SATURATED /*!< T.B.D. Bit position \f$==16\f$ */
     ,
    NO_DATA_FROM_DIGITAL_POWER_METER /*!< The current data from digital power meter are available for the calculation of the 3-bit linearity correction. An old correction factor is applied. Bit position \f$==17\f$ */
     ,
    RESERVED_18 /*!< Not assigned. */
     ,
    RESERVED_19 /*!< Not assigned. */
     ,
    RESERVED_20 /*!< Not assigned. */
     ,
    RESERVED_21 /*!< Not assigned. */
     ,
    RESERVED_22 /*!< Not assigned. */
     ,
    RESERVED_23 /*!< Not assigned. */
     ,
    RESERVED_24 /*!< Not assigned. */
     ,
    RESERVED_25 /*!< Not assigned. */
     ,
    RESERVED_26 /*!< Not assigned. */
     ,
    RESERVED_27 /*!< Not assigned. */
     ,
    RESERVED_28 /*!< Not assigned. */
     ,
    RESERVED_29 /*!< Not assigned. */
     ,
    RESERVED_30 /*!< Not assigned. */
     ,
    ALL_PURPOSE_ERROR /*!< This bit designates data flagged in the correlator but does not provide information as to the reason for the flag. Readers are expected not to process the data when this bit is set. Bit position \f$ ==31 \f$. */
     
  };
  typedef BinaryDataFlags &BinaryDataFlags_out;
} 
#endif

namespace BinaryDataFlagsMod {
	std::ostream & operator << ( std::ostream & out, const BinaryDataFlags& value);
	std::istream & operator >> ( std::istream & in , BinaryDataFlags& value );
}

/** 
  * A helper class for the enumeration BinaryDataFlags.
  * 
  */
class CBinaryDataFlags {
  public:
 
	/**
	  * Enumerators as strings.
	  */  
	
	static const std::string& sINTEGRATION_FULLY_BLANKED; /*!< A const string equal to "INTEGRATION_FULLY_BLANKED".*/
	
	static const std::string& sWVR_APC; /*!< A const string equal to "WVR_APC".*/
	
	static const std::string& sCORRELATOR_MISSING_STATUS; /*!< A const string equal to "CORRELATOR_MISSING_STATUS".*/
	
	static const std::string& sMISSING_ANTENNA_EVENT; /*!< A const string equal to "MISSING_ANTENNA_EVENT".*/
	
	static const std::string& sDELTA_SIGMA_OVERFLOW; /*!< A const string equal to "DELTA_SIGMA_OVERFLOW".*/
	
	static const std::string& sDELAY_CORRECTION_NOT_APPLIED; /*!< A const string equal to "DELAY_CORRECTION_NOT_APPLIED".*/
	
	static const std::string& sSYNCRONIZATION_ERROR; /*!< A const string equal to "SYNCRONIZATION_ERROR".*/
	
	static const std::string& sFFT_OVERFLOW; /*!< A const string equal to "FFT_OVERFLOW".*/
	
	static const std::string& sTFB_SCALING_FACTOR_NOT_RETRIEVED; /*!< A const string equal to "TFB_SCALING_FACTOR_NOT_RETRIEVED".*/
	
	static const std::string& sZERO_LAG_NOT_RECEIVED; /*!< A const string equal to "ZERO_LAG_NOT_RECEIVED".*/
	
	static const std::string& sSIGMA_OVERFLOW; /*!< A const string equal to "SIGMA_OVERFLOW".*/
	
	static const std::string& sUNUSABLE_CAI_OUTPUT; /*!< A const string equal to "UNUSABLE_CAI_OUTPUT".*/
	
	static const std::string& sQC_FAILED; /*!< A const string equal to "QC_FAILED".*/
	
	static const std::string& sNOISY_TDM_CHANNELS; /*!< A const string equal to "NOISY_TDM_CHANNELS".*/
	
	static const std::string& sSPECTRAL_NORMALIZATION_FAILED; /*!< A const string equal to "SPECTRAL_NORMALIZATION_FAILED".*/
	
	static const std::string& sDROPPED_PACKETS; /*!< A const string equal to "DROPPED_PACKETS".*/
	
	static const std::string& sDETECTOR_SATURATED; /*!< A const string equal to "DETECTOR_SATURATED".*/
	
	static const std::string& sNO_DATA_FROM_DIGITAL_POWER_METER; /*!< A const string equal to "NO_DATA_FROM_DIGITAL_POWER_METER".*/
	
	static const std::string& sRESERVED_18; /*!< A const string equal to "RESERVED_18".*/
	
	static const std::string& sRESERVED_19; /*!< A const string equal to "RESERVED_19".*/
	
	static const std::string& sRESERVED_20; /*!< A const string equal to "RESERVED_20".*/
	
	static const std::string& sRESERVED_21; /*!< A const string equal to "RESERVED_21".*/
	
	static const std::string& sRESERVED_22; /*!< A const string equal to "RESERVED_22".*/
	
	static const std::string& sRESERVED_23; /*!< A const string equal to "RESERVED_23".*/
	
	static const std::string& sRESERVED_24; /*!< A const string equal to "RESERVED_24".*/
	
	static const std::string& sRESERVED_25; /*!< A const string equal to "RESERVED_25".*/
	
	static const std::string& sRESERVED_26; /*!< A const string equal to "RESERVED_26".*/
	
	static const std::string& sRESERVED_27; /*!< A const string equal to "RESERVED_27".*/
	
	static const std::string& sRESERVED_28; /*!< A const string equal to "RESERVED_28".*/
	
	static const std::string& sRESERVED_29; /*!< A const string equal to "RESERVED_29".*/
	
	static const std::string& sRESERVED_30; /*!< A const string equal to "RESERVED_30".*/
	
	static const std::string& sALL_PURPOSE_ERROR; /*!< A const string equal to "ALL_PURPOSE_ERROR".*/
	

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
       * Return the number of enumerators declared in BinaryDataFlagsMod::BinaryDataFlags.
       * @return an unsigned int.
       */
       static unsigned int size() ;
       
       
    /**
      * Returns an enumerator as a string.
      * @param e an enumerator of BinaryDataFlagsMod::BinaryDataFlags.
      * @return a string.
      */
	static std::string name(const BinaryDataFlagsMod::BinaryDataFlags& e);
	
	/**
	  * Equivalent to the name method.
	  */
    static std::string toString(const BinaryDataFlagsMod::BinaryDataFlags& f) { return name(f); }

	/** 
	  * Returns vector of  all the enumerators as strings. 
	  * The strings are stored in the vector in the same order than the enumerators are declared in the enumeration. 
	  * @return a vector of string.
	  */
     static const std::vector<std::string> names();	 
    
   	
   	// Create a BinaryDataFlags enumeration object by specifying its name.
   	static BinaryDataFlagsMod::BinaryDataFlags newBinaryDataFlags(const std::string& name);
   	
   	/*! Return a BinaryDataFlags's enumerator  given a string.
   	  * @param name the string representation of the enumerator.
   	 *  @return a BinaryDataFlagsMod::BinaryDataFlags's enumerator.
   	 *  @throws a string containing an error message if no enumerator could be found for this name.
   	 */
 	static BinaryDataFlagsMod::BinaryDataFlags literal(const std::string& name);
 	
    /*! Return a BinaryDataFlags's enumerator given an unsigned int.
      * @param i the index of the enumerator in BinaryDataFlagsMod::BinaryDataFlags.
      * @return a BinaryDataFlagsMod::BinaryDataFlags's enumerator.
      * @throws a string containing an error message if no enumerator could be found for this integer.
      */
 	static BinaryDataFlagsMod::BinaryDataFlags from_int(unsigned int i);	
 	

  private:
    /* Not Implemented.  This is a pure static class. */
    CBinaryDataFlags();
    CBinaryDataFlags(const CBinaryDataFlags&);
    CBinaryDataFlags& operator=(const CBinaryDataFlags&);
    
    static std::string badString(const std::string& name) ;
  	static std::string badInt(unsigned int i) ;
  	
};
 
#endif /*!CBinaryDataFlags_H*/
