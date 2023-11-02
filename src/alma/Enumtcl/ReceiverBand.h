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


#if     !defined(_RECEIVERBAND_H)

#include <alma/Enumerations/CReceiverBand.h>
#define _RECEIVERBAND_H
#endif 

#if     !defined(_RECEIVERBAND_HH)

#include <alma/Enumtcl/Enum.hpp>

template<>
 struct enum_set_traits<ReceiverBandMod::ReceiverBand> : public enum_set_traiter<ReceiverBandMod::ReceiverBand,27,ReceiverBandMod::UNSPECIFIED> {};

template<>
class enum_map_traits<ReceiverBandMod::ReceiverBand,void> : public enum_map_traiter<ReceiverBandMod::ReceiverBand,void> {
public:
  static bool   init_;
  static std::string typeName_;
  static std::string enumerationDesc_;
  static std::string order_;
  static std::string xsdBaseType_;
  static bool   init(){
    EnumPar<void> ep;
    m_.insert(std::pair<ReceiverBandMod::ReceiverBand,EnumPar<void> >
     (ReceiverBandMod::ALMA_RB_01,ep((int)ReceiverBandMod::ALMA_RB_01,"ALMA_RB_01","un-documented")));
    m_.insert(std::pair<ReceiverBandMod::ReceiverBand,EnumPar<void> >
     (ReceiverBandMod::ALMA_RB_02,ep((int)ReceiverBandMod::ALMA_RB_02,"ALMA_RB_02","un-documented")));
    m_.insert(std::pair<ReceiverBandMod::ReceiverBand,EnumPar<void> >
     (ReceiverBandMod::ALMA_RB_03,ep((int)ReceiverBandMod::ALMA_RB_03,"ALMA_RB_03","un-documented")));
    m_.insert(std::pair<ReceiverBandMod::ReceiverBand,EnumPar<void> >
     (ReceiverBandMod::ALMA_RB_04,ep((int)ReceiverBandMod::ALMA_RB_04,"ALMA_RB_04","un-documented")));
    m_.insert(std::pair<ReceiverBandMod::ReceiverBand,EnumPar<void> >
     (ReceiverBandMod::ALMA_RB_05,ep((int)ReceiverBandMod::ALMA_RB_05,"ALMA_RB_05","un-documented")));
    m_.insert(std::pair<ReceiverBandMod::ReceiverBand,EnumPar<void> >
     (ReceiverBandMod::ALMA_RB_06,ep((int)ReceiverBandMod::ALMA_RB_06,"ALMA_RB_06","un-documented")));
    m_.insert(std::pair<ReceiverBandMod::ReceiverBand,EnumPar<void> >
     (ReceiverBandMod::ALMA_RB_07,ep((int)ReceiverBandMod::ALMA_RB_07,"ALMA_RB_07","un-documented")));
    m_.insert(std::pair<ReceiverBandMod::ReceiverBand,EnumPar<void> >
     (ReceiverBandMod::ALMA_RB_08,ep((int)ReceiverBandMod::ALMA_RB_08,"ALMA_RB_08","un-documented")));
    m_.insert(std::pair<ReceiverBandMod::ReceiverBand,EnumPar<void> >
     (ReceiverBandMod::ALMA_RB_09,ep((int)ReceiverBandMod::ALMA_RB_09,"ALMA_RB_09","un-documented")));
    m_.insert(std::pair<ReceiverBandMod::ReceiverBand,EnumPar<void> >
     (ReceiverBandMod::ALMA_RB_10,ep((int)ReceiverBandMod::ALMA_RB_10,"ALMA_RB_10","un-documented")));
    m_.insert(std::pair<ReceiverBandMod::ReceiverBand,EnumPar<void> >
     (ReceiverBandMod::ALMA_RB_ALL,ep((int)ReceiverBandMod::ALMA_RB_ALL,"ALMA_RB_ALL","un-documented")));
    m_.insert(std::pair<ReceiverBandMod::ReceiverBand,EnumPar<void> >
     (ReceiverBandMod::ALMA_HOLOGRAPHY_RECEIVER,ep((int)ReceiverBandMod::ALMA_HOLOGRAPHY_RECEIVER,"ALMA_HOLOGRAPHY_RECEIVER","un-documented")));
    m_.insert(std::pair<ReceiverBandMod::ReceiverBand,EnumPar<void> >
     (ReceiverBandMod::BURE_01,ep((int)ReceiverBandMod::BURE_01,"BURE_01","un-documented")));
    m_.insert(std::pair<ReceiverBandMod::ReceiverBand,EnumPar<void> >
     (ReceiverBandMod::BURE_02,ep((int)ReceiverBandMod::BURE_02,"BURE_02","un-documented")));
    m_.insert(std::pair<ReceiverBandMod::ReceiverBand,EnumPar<void> >
     (ReceiverBandMod::BURE_03,ep((int)ReceiverBandMod::BURE_03,"BURE_03","un-documented")));
    m_.insert(std::pair<ReceiverBandMod::ReceiverBand,EnumPar<void> >
     (ReceiverBandMod::BURE_04,ep((int)ReceiverBandMod::BURE_04,"BURE_04","un-documented")));
    m_.insert(std::pair<ReceiverBandMod::ReceiverBand,EnumPar<void> >
     (ReceiverBandMod::EVLA_4,ep((int)ReceiverBandMod::EVLA_4,"EVLA_4","un-documented")));
    m_.insert(std::pair<ReceiverBandMod::ReceiverBand,EnumPar<void> >
     (ReceiverBandMod::EVLA_P,ep((int)ReceiverBandMod::EVLA_P,"EVLA_P","un-documented")));
    m_.insert(std::pair<ReceiverBandMod::ReceiverBand,EnumPar<void> >
     (ReceiverBandMod::EVLA_L,ep((int)ReceiverBandMod::EVLA_L,"EVLA_L","un-documented")));
    m_.insert(std::pair<ReceiverBandMod::ReceiverBand,EnumPar<void> >
     (ReceiverBandMod::EVLA_C,ep((int)ReceiverBandMod::EVLA_C,"EVLA_C","un-documented")));
    m_.insert(std::pair<ReceiverBandMod::ReceiverBand,EnumPar<void> >
     (ReceiverBandMod::EVLA_S,ep((int)ReceiverBandMod::EVLA_S,"EVLA_S","un-documented")));
    m_.insert(std::pair<ReceiverBandMod::ReceiverBand,EnumPar<void> >
     (ReceiverBandMod::EVLA_X,ep((int)ReceiverBandMod::EVLA_X,"EVLA_X","un-documented")));
    m_.insert(std::pair<ReceiverBandMod::ReceiverBand,EnumPar<void> >
     (ReceiverBandMod::EVLA_Ku,ep((int)ReceiverBandMod::EVLA_Ku,"EVLA_Ku","un-documented")));
    m_.insert(std::pair<ReceiverBandMod::ReceiverBand,EnumPar<void> >
     (ReceiverBandMod::EVLA_K,ep((int)ReceiverBandMod::EVLA_K,"EVLA_K","un-documented")));
    m_.insert(std::pair<ReceiverBandMod::ReceiverBand,EnumPar<void> >
     (ReceiverBandMod::EVLA_Ka,ep((int)ReceiverBandMod::EVLA_Ka,"EVLA_Ka","un-documented")));
    m_.insert(std::pair<ReceiverBandMod::ReceiverBand,EnumPar<void> >
     (ReceiverBandMod::EVLA_Q,ep((int)ReceiverBandMod::EVLA_Q,"EVLA_Q","un-documented")));
    m_.insert(std::pair<ReceiverBandMod::ReceiverBand,EnumPar<void> >
     (ReceiverBandMod::UNSPECIFIED,ep((int)ReceiverBandMod::UNSPECIFIED,"UNSPECIFIED","un-documented")));
    return true;
  }
  static std::map<ReceiverBandMod::ReceiverBand,EnumPar<void> > m_;
};
#define _RECEIVERBAND_HH
#endif
