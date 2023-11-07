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


#if     !defined(_SCANINTENT_H)

#include <alma/Enumerations/CScanIntent.h>
#define _SCANINTENT_H
#endif 

#if     !defined(_SCANINTENT_HH)

#include <alma/Enumtcl/Enum.hpp>

template<>
 struct enum_set_traits<ScanIntentMod::ScanIntent> : public enum_set_traiter<ScanIntentMod::ScanIntent,30,ScanIntentMod::CALIBRATE_DIFFGAIN> {};

template<>
class enum_map_traits<ScanIntentMod::ScanIntent,void> : public enum_map_traiter<ScanIntentMod::ScanIntent,void> {
public:
  static bool   init_;
  static std::string typeName_;
  static std::string enumerationDesc_;
  static std::string order_;
  static std::string xsdBaseType_;
  static bool   init(){
    EnumPar<void> ep;
    m_.insert(std::pair<ScanIntentMod::ScanIntent,EnumPar<void> >
     (ScanIntentMod::CALIBRATE_AMPLI,ep((int)ScanIntentMod::CALIBRATE_AMPLI,"CALIBRATE_AMPLI","un-documented")));
    m_.insert(std::pair<ScanIntentMod::ScanIntent,EnumPar<void> >
     (ScanIntentMod::CALIBRATE_ATMOSPHERE,ep((int)ScanIntentMod::CALIBRATE_ATMOSPHERE,"CALIBRATE_ATMOSPHERE","un-documented")));
    m_.insert(std::pair<ScanIntentMod::ScanIntent,EnumPar<void> >
     (ScanIntentMod::CALIBRATE_BANDPASS,ep((int)ScanIntentMod::CALIBRATE_BANDPASS,"CALIBRATE_BANDPASS","un-documented")));
    m_.insert(std::pair<ScanIntentMod::ScanIntent,EnumPar<void> >
     (ScanIntentMod::CALIBRATE_DELAY,ep((int)ScanIntentMod::CALIBRATE_DELAY,"CALIBRATE_DELAY","un-documented")));
    m_.insert(std::pair<ScanIntentMod::ScanIntent,EnumPar<void> >
     (ScanIntentMod::CALIBRATE_FLUX,ep((int)ScanIntentMod::CALIBRATE_FLUX,"CALIBRATE_FLUX","un-documented")));
    m_.insert(std::pair<ScanIntentMod::ScanIntent,EnumPar<void> >
     (ScanIntentMod::CALIBRATE_FOCUS,ep((int)ScanIntentMod::CALIBRATE_FOCUS,"CALIBRATE_FOCUS","un-documented")));
    m_.insert(std::pair<ScanIntentMod::ScanIntent,EnumPar<void> >
     (ScanIntentMod::CALIBRATE_FOCUS_X,ep((int)ScanIntentMod::CALIBRATE_FOCUS_X,"CALIBRATE_FOCUS_X","un-documented")));
    m_.insert(std::pair<ScanIntentMod::ScanIntent,EnumPar<void> >
     (ScanIntentMod::CALIBRATE_FOCUS_Y,ep((int)ScanIntentMod::CALIBRATE_FOCUS_Y,"CALIBRATE_FOCUS_Y","un-documented")));
    m_.insert(std::pair<ScanIntentMod::ScanIntent,EnumPar<void> >
     (ScanIntentMod::CALIBRATE_PHASE,ep((int)ScanIntentMod::CALIBRATE_PHASE,"CALIBRATE_PHASE","un-documented")));
    m_.insert(std::pair<ScanIntentMod::ScanIntent,EnumPar<void> >
     (ScanIntentMod::CALIBRATE_POINTING,ep((int)ScanIntentMod::CALIBRATE_POINTING,"CALIBRATE_POINTING","un-documented")));
    m_.insert(std::pair<ScanIntentMod::ScanIntent,EnumPar<void> >
     (ScanIntentMod::CALIBRATE_POLARIZATION,ep((int)ScanIntentMod::CALIBRATE_POLARIZATION,"CALIBRATE_POLARIZATION","un-documented")));
    m_.insert(std::pair<ScanIntentMod::ScanIntent,EnumPar<void> >
     (ScanIntentMod::CALIBRATE_SIDEBAND_RATIO,ep((int)ScanIntentMod::CALIBRATE_SIDEBAND_RATIO,"CALIBRATE_SIDEBAND_RATIO","un-documented")));
    m_.insert(std::pair<ScanIntentMod::ScanIntent,EnumPar<void> >
     (ScanIntentMod::CALIBRATE_WVR,ep((int)ScanIntentMod::CALIBRATE_WVR,"CALIBRATE_WVR","un-documented")));
    m_.insert(std::pair<ScanIntentMod::ScanIntent,EnumPar<void> >
     (ScanIntentMod::DO_SKYDIP,ep((int)ScanIntentMod::DO_SKYDIP,"DO_SKYDIP","un-documented")));
    m_.insert(std::pair<ScanIntentMod::ScanIntent,EnumPar<void> >
     (ScanIntentMod::MAP_ANTENNA_SURFACE,ep((int)ScanIntentMod::MAP_ANTENNA_SURFACE,"MAP_ANTENNA_SURFACE","un-documented")));
    m_.insert(std::pair<ScanIntentMod::ScanIntent,EnumPar<void> >
     (ScanIntentMod::MAP_PRIMARY_BEAM,ep((int)ScanIntentMod::MAP_PRIMARY_BEAM,"MAP_PRIMARY_BEAM","un-documented")));
    m_.insert(std::pair<ScanIntentMod::ScanIntent,EnumPar<void> >
     (ScanIntentMod::OBSERVE_TARGET,ep((int)ScanIntentMod::OBSERVE_TARGET,"OBSERVE_TARGET","un-documented")));
    m_.insert(std::pair<ScanIntentMod::ScanIntent,EnumPar<void> >
     (ScanIntentMod::CALIBRATE_POL_LEAKAGE,ep((int)ScanIntentMod::CALIBRATE_POL_LEAKAGE,"CALIBRATE_POL_LEAKAGE","un-documented")));
    m_.insert(std::pair<ScanIntentMod::ScanIntent,EnumPar<void> >
     (ScanIntentMod::CALIBRATE_POL_ANGLE,ep((int)ScanIntentMod::CALIBRATE_POL_ANGLE,"CALIBRATE_POL_ANGLE","un-documented")));
    m_.insert(std::pair<ScanIntentMod::ScanIntent,EnumPar<void> >
     (ScanIntentMod::TEST,ep((int)ScanIntentMod::TEST,"TEST","un-documented")));
    m_.insert(std::pair<ScanIntentMod::ScanIntent,EnumPar<void> >
     (ScanIntentMod::UNSPECIFIED,ep((int)ScanIntentMod::UNSPECIFIED,"UNSPECIFIED","un-documented")));
    m_.insert(std::pair<ScanIntentMod::ScanIntent,EnumPar<void> >
     (ScanIntentMod::CALIBRATE_ANTENNA_POSITION,ep((int)ScanIntentMod::CALIBRATE_ANTENNA_POSITION,"CALIBRATE_ANTENNA_POSITION","un-documented")));
    m_.insert(std::pair<ScanIntentMod::ScanIntent,EnumPar<void> >
     (ScanIntentMod::CALIBRATE_ANTENNA_PHASE,ep((int)ScanIntentMod::CALIBRATE_ANTENNA_PHASE,"CALIBRATE_ANTENNA_PHASE","un-documented")));
    m_.insert(std::pair<ScanIntentMod::ScanIntent,EnumPar<void> >
     (ScanIntentMod::MEASURE_RFI,ep((int)ScanIntentMod::MEASURE_RFI,"MEASURE_RFI","un-documented")));
    m_.insert(std::pair<ScanIntentMod::ScanIntent,EnumPar<void> >
     (ScanIntentMod::CALIBRATE_ANTENNA_POINTING_MODEL,ep((int)ScanIntentMod::CALIBRATE_ANTENNA_POINTING_MODEL,"CALIBRATE_ANTENNA_POINTING_MODEL","un-documented")));
    m_.insert(std::pair<ScanIntentMod::ScanIntent,EnumPar<void> >
     (ScanIntentMod::SYSTEM_CONFIGURATION,ep((int)ScanIntentMod::SYSTEM_CONFIGURATION,"SYSTEM_CONFIGURATION","un-documented")));
    m_.insert(std::pair<ScanIntentMod::ScanIntent,EnumPar<void> >
     (ScanIntentMod::CALIBRATE_APPPHASE_ACTIVE,ep((int)ScanIntentMod::CALIBRATE_APPPHASE_ACTIVE,"CALIBRATE_APPPHASE_ACTIVE","un-documented")));
    m_.insert(std::pair<ScanIntentMod::ScanIntent,EnumPar<void> >
     (ScanIntentMod::CALIBRATE_APPPHASE_PASSIVE,ep((int)ScanIntentMod::CALIBRATE_APPPHASE_PASSIVE,"CALIBRATE_APPPHASE_PASSIVE","un-documented")));
    m_.insert(std::pair<ScanIntentMod::ScanIntent,EnumPar<void> >
     (ScanIntentMod::OBSERVE_CHECK_SOURCE,ep((int)ScanIntentMod::OBSERVE_CHECK_SOURCE,"OBSERVE_CHECK_SOURCE","un-documented")));
    m_.insert(std::pair<ScanIntentMod::ScanIntent,EnumPar<void> >
     (ScanIntentMod::CALIBRATE_DIFFGAIN,ep((int)ScanIntentMod::CALIBRATE_DIFFGAIN,"CALIBRATE_DIFFGAIN","un-documented")));
    return true;
  }
  static std::map<ScanIntentMod::ScanIntent,EnumPar<void> > m_;
};
#define _SCANINTENT_HH
#endif
