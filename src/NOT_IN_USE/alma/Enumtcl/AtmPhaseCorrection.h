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


#if     !defined(_ATMPHASECORRECTION_H)

#include <alma/Enumerations/CAtmPhaseCorrection.h>
#define _ATMPHASECORRECTION_H
#endif 

#if     !defined(_ATMPHASECORRECTION_HH)

#include <alma/Enumtcl/Enum.hpp>

template<>
 struct enum_set_traits<AtmPhaseCorrectionMod::AtmPhaseCorrection> : public enum_set_traiter<AtmPhaseCorrectionMod::AtmPhaseCorrection,2,AtmPhaseCorrectionMod::AP_CORRECTED> {};

template<>
class enum_map_traits<AtmPhaseCorrectionMod::AtmPhaseCorrection,void> : public enum_map_traiter<AtmPhaseCorrectionMod::AtmPhaseCorrection,void> {
public:
  static bool   init_;
  static std::string typeName_;
  static std::string enumerationDesc_;
  static std::string order_;
  static std::string xsdBaseType_;
  static bool   init(){
    EnumPar<void> ep;
    m_.insert(std::pair<AtmPhaseCorrectionMod::AtmPhaseCorrection,EnumPar<void> >
     (AtmPhaseCorrectionMod::AP_UNCORRECTED,ep((int)AtmPhaseCorrectionMod::AP_UNCORRECTED,"AP_UNCORRECTED","un-documented")));
    m_.insert(std::pair<AtmPhaseCorrectionMod::AtmPhaseCorrection,EnumPar<void> >
     (AtmPhaseCorrectionMod::AP_CORRECTED,ep((int)AtmPhaseCorrectionMod::AP_CORRECTED,"AP_CORRECTED","un-documented")));
    return true;
  }
  static std::map<AtmPhaseCorrectionMod::AtmPhaseCorrection,EnumPar<void> > m_;
};
#define _ATMPHASECORRECTION_HH
#endif
