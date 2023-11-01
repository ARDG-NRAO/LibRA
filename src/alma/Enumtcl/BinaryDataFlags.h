/*
 *
 * /////////////////////////////////////////////////////////////////
 * // WARNING!  DO NOT MODIFY THIS FILE!                          //
 * //  ---------------------------------------------------------  //
 * // | This is generated code using a C++ template function!   | //
 * // ! Do not modify this file.                                | //
 * // | Any changes will be lost when the file is re-generated. | //
 * //  ---------------------------------------------------------  //
 * /////////////////////////////////////////////////////////////////
 *
 */


#if     !defined(_BINARYDATAFLAGS_H)

#include <alma/Enumerations/CBinaryDataFlags.h>
#define _BINARYDATAFLAGS_H
#endif 

#if     !defined(_BINARYDATAFLAGS_HH)

#include <alma/Enumtcl/Enum.hpp>

template<>
 struct enum_set_traits<BinaryDataFlagsMod::BinaryDataFlags> : public enum_set_traiter<BinaryDataFlagsMod::BinaryDataFlags,32,BinaryDataFlagsMod::ALL_PURPOSE_ERROR> {};

template<>
class enum_map_traits<BinaryDataFlagsMod::BinaryDataFlags,void> : public enum_map_traiter<BinaryDataFlagsMod::BinaryDataFlags,void> {
public:
  static bool   init_;
  static std::string typeName_;
  static std::string enumerationDesc_;
  static std::string order_;
  static std::string xsdBaseType_;
  static bool   init(){
    EnumPar<void> ep;
    m_.insert(std::pair<BinaryDataFlagsMod::BinaryDataFlags,EnumPar<void> >
     (BinaryDataFlagsMod::INTEGRATION_FULLY_BLANKED,ep((int)BinaryDataFlagsMod::INTEGRATION_FULLY_BLANKED,"INTEGRATION_FULLY_BLANKED","un-documented")));
    m_.insert(std::pair<BinaryDataFlagsMod::BinaryDataFlags,EnumPar<void> >
     (BinaryDataFlagsMod::WVR_APC,ep((int)BinaryDataFlagsMod::WVR_APC,"WVR_APC","un-documented")));
    m_.insert(std::pair<BinaryDataFlagsMod::BinaryDataFlags,EnumPar<void> >
     (BinaryDataFlagsMod::CORRELATOR_MISSING_STATUS,ep((int)BinaryDataFlagsMod::CORRELATOR_MISSING_STATUS,"CORRELATOR_MISSING_STATUS","un-documented")));
    m_.insert(std::pair<BinaryDataFlagsMod::BinaryDataFlags,EnumPar<void> >
     (BinaryDataFlagsMod::MISSING_ANTENNA_EVENT,ep((int)BinaryDataFlagsMod::MISSING_ANTENNA_EVENT,"MISSING_ANTENNA_EVENT","un-documented")));
    m_.insert(std::pair<BinaryDataFlagsMod::BinaryDataFlags,EnumPar<void> >
     (BinaryDataFlagsMod::DELTA_SIGMA_OVERFLOW,ep((int)BinaryDataFlagsMod::DELTA_SIGMA_OVERFLOW,"DELTA_SIGMA_OVERFLOW","un-documented")));
    m_.insert(std::pair<BinaryDataFlagsMod::BinaryDataFlags,EnumPar<void> >
     (BinaryDataFlagsMod::DELAY_CORRECTION_NOT_APPLIED,ep((int)BinaryDataFlagsMod::DELAY_CORRECTION_NOT_APPLIED,"DELAY_CORRECTION_NOT_APPLIED","un-documented")));
    m_.insert(std::pair<BinaryDataFlagsMod::BinaryDataFlags,EnumPar<void> >
     (BinaryDataFlagsMod::SYNCRONIZATION_ERROR,ep((int)BinaryDataFlagsMod::SYNCRONIZATION_ERROR,"SYNCRONIZATION_ERROR","un-documented")));
    m_.insert(std::pair<BinaryDataFlagsMod::BinaryDataFlags,EnumPar<void> >
     (BinaryDataFlagsMod::FFT_OVERFLOW,ep((int)BinaryDataFlagsMod::FFT_OVERFLOW,"FFT_OVERFLOW","un-documented")));
    m_.insert(std::pair<BinaryDataFlagsMod::BinaryDataFlags,EnumPar<void> >
     (BinaryDataFlagsMod::TFB_SCALING_FACTOR_NOT_RETRIEVED,ep((int)BinaryDataFlagsMod::TFB_SCALING_FACTOR_NOT_RETRIEVED,"TFB_SCALING_FACTOR_NOT_RETRIEVED","un-documented")));
    m_.insert(std::pair<BinaryDataFlagsMod::BinaryDataFlags,EnumPar<void> >
     (BinaryDataFlagsMod::ZERO_LAG_NOT_RECEIVED,ep((int)BinaryDataFlagsMod::ZERO_LAG_NOT_RECEIVED,"ZERO_LAG_NOT_RECEIVED","un-documented")));
    m_.insert(std::pair<BinaryDataFlagsMod::BinaryDataFlags,EnumPar<void> >
     (BinaryDataFlagsMod::SIGMA_OVERFLOW,ep((int)BinaryDataFlagsMod::SIGMA_OVERFLOW,"SIGMA_OVERFLOW","un-documented")));
    m_.insert(std::pair<BinaryDataFlagsMod::BinaryDataFlags,EnumPar<void> >
     (BinaryDataFlagsMod::UNUSABLE_CAI_OUTPUT,ep((int)BinaryDataFlagsMod::UNUSABLE_CAI_OUTPUT,"UNUSABLE_CAI_OUTPUT","un-documented")));
    m_.insert(std::pair<BinaryDataFlagsMod::BinaryDataFlags,EnumPar<void> >
     (BinaryDataFlagsMod::QC_FAILED,ep((int)BinaryDataFlagsMod::QC_FAILED,"QC_FAILED","un-documented")));
    m_.insert(std::pair<BinaryDataFlagsMod::BinaryDataFlags,EnumPar<void> >
     (BinaryDataFlagsMod::NOISY_TDM_CHANNELS,ep((int)BinaryDataFlagsMod::NOISY_TDM_CHANNELS,"NOISY_TDM_CHANNELS","un-documented")));
    m_.insert(std::pair<BinaryDataFlagsMod::BinaryDataFlags,EnumPar<void> >
     (BinaryDataFlagsMod::SPECTRAL_NORMALIZATION_FAILED,ep((int)BinaryDataFlagsMod::SPECTRAL_NORMALIZATION_FAILED,"SPECTRAL_NORMALIZATION_FAILED","un-documented")));
    m_.insert(std::pair<BinaryDataFlagsMod::BinaryDataFlags,EnumPar<void> >
     (BinaryDataFlagsMod::DROPPED_PACKETS,ep((int)BinaryDataFlagsMod::DROPPED_PACKETS,"DROPPED_PACKETS","un-documented")));
    m_.insert(std::pair<BinaryDataFlagsMod::BinaryDataFlags,EnumPar<void> >
     (BinaryDataFlagsMod::DETECTOR_SATURATED,ep((int)BinaryDataFlagsMod::DETECTOR_SATURATED,"DETECTOR_SATURATED","un-documented")));
    m_.insert(std::pair<BinaryDataFlagsMod::BinaryDataFlags,EnumPar<void> >
     (BinaryDataFlagsMod::NO_DATA_FROM_DIGITAL_POWER_METER,ep((int)BinaryDataFlagsMod::NO_DATA_FROM_DIGITAL_POWER_METER,"NO_DATA_FROM_DIGITAL_POWER_METER","un-documented")));
    m_.insert(std::pair<BinaryDataFlagsMod::BinaryDataFlags,EnumPar<void> >
     (BinaryDataFlagsMod::RESERVED_18,ep((int)BinaryDataFlagsMod::RESERVED_18,"RESERVED_18","un-documented")));
    m_.insert(std::pair<BinaryDataFlagsMod::BinaryDataFlags,EnumPar<void> >
     (BinaryDataFlagsMod::RESERVED_19,ep((int)BinaryDataFlagsMod::RESERVED_19,"RESERVED_19","un-documented")));
    m_.insert(std::pair<BinaryDataFlagsMod::BinaryDataFlags,EnumPar<void> >
     (BinaryDataFlagsMod::RESERVED_20,ep((int)BinaryDataFlagsMod::RESERVED_20,"RESERVED_20","un-documented")));
    m_.insert(std::pair<BinaryDataFlagsMod::BinaryDataFlags,EnumPar<void> >
     (BinaryDataFlagsMod::RESERVED_21,ep((int)BinaryDataFlagsMod::RESERVED_21,"RESERVED_21","un-documented")));
    m_.insert(std::pair<BinaryDataFlagsMod::BinaryDataFlags,EnumPar<void> >
     (BinaryDataFlagsMod::RESERVED_22,ep((int)BinaryDataFlagsMod::RESERVED_22,"RESERVED_22","un-documented")));
    m_.insert(std::pair<BinaryDataFlagsMod::BinaryDataFlags,EnumPar<void> >
     (BinaryDataFlagsMod::RESERVED_23,ep((int)BinaryDataFlagsMod::RESERVED_23,"RESERVED_23","un-documented")));
    m_.insert(std::pair<BinaryDataFlagsMod::BinaryDataFlags,EnumPar<void> >
     (BinaryDataFlagsMod::RESERVED_24,ep((int)BinaryDataFlagsMod::RESERVED_24,"RESERVED_24","un-documented")));
    m_.insert(std::pair<BinaryDataFlagsMod::BinaryDataFlags,EnumPar<void> >
     (BinaryDataFlagsMod::RESERVED_25,ep((int)BinaryDataFlagsMod::RESERVED_25,"RESERVED_25","un-documented")));
    m_.insert(std::pair<BinaryDataFlagsMod::BinaryDataFlags,EnumPar<void> >
     (BinaryDataFlagsMod::RESERVED_26,ep((int)BinaryDataFlagsMod::RESERVED_26,"RESERVED_26","un-documented")));
    m_.insert(std::pair<BinaryDataFlagsMod::BinaryDataFlags,EnumPar<void> >
     (BinaryDataFlagsMod::RESERVED_27,ep((int)BinaryDataFlagsMod::RESERVED_27,"RESERVED_27","un-documented")));
    m_.insert(std::pair<BinaryDataFlagsMod::BinaryDataFlags,EnumPar<void> >
     (BinaryDataFlagsMod::RESERVED_28,ep((int)BinaryDataFlagsMod::RESERVED_28,"RESERVED_28","un-documented")));
    m_.insert(std::pair<BinaryDataFlagsMod::BinaryDataFlags,EnumPar<void> >
     (BinaryDataFlagsMod::RESERVED_29,ep((int)BinaryDataFlagsMod::RESERVED_29,"RESERVED_29","un-documented")));
    m_.insert(std::pair<BinaryDataFlagsMod::BinaryDataFlags,EnumPar<void> >
     (BinaryDataFlagsMod::RESERVED_30,ep((int)BinaryDataFlagsMod::RESERVED_30,"RESERVED_30","un-documented")));
    m_.insert(std::pair<BinaryDataFlagsMod::BinaryDataFlags,EnumPar<void> >
     (BinaryDataFlagsMod::ALL_PURPOSE_ERROR,ep((int)BinaryDataFlagsMod::ALL_PURPOSE_ERROR,"ALL_PURPOSE_ERROR","un-documented")));
    return true;
  }
  static std::map<BinaryDataFlagsMod::BinaryDataFlags,EnumPar<void> > m_;
};
#define _BINARYDATAFLAGS_HH
#endif
