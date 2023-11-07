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


#if     !defined(_HOLOGRAPHYCHANNELTYPE_H)

#include <alma/Enumerations/CHolographyChannelType.h>
#define _HOLOGRAPHYCHANNELTYPE_H
#endif 

#if     !defined(_HOLOGRAPHYCHANNELTYPE_HH)

#include <alma/Enumtcl/Enum.hpp>

template<>
 struct enum_set_traits<HolographyChannelTypeMod::HolographyChannelType> : public enum_set_traiter<HolographyChannelTypeMod::HolographyChannelType,6,HolographyChannelTypeMod::S2> {};

template<>
class enum_map_traits<HolographyChannelTypeMod::HolographyChannelType,void> : public enum_map_traiter<HolographyChannelTypeMod::HolographyChannelType,void> {
public:
  static bool   init_;
  static std::string typeName_;
  static std::string enumerationDesc_;
  static std::string order_;
  static std::string xsdBaseType_;
  static bool   init(){
    EnumPar<void> ep;
    m_.insert(std::pair<HolographyChannelTypeMod::HolographyChannelType,EnumPar<void> >
     (HolographyChannelTypeMod::Q2,ep((int)HolographyChannelTypeMod::Q2,"Q2","un-documented")));
    m_.insert(std::pair<HolographyChannelTypeMod::HolographyChannelType,EnumPar<void> >
     (HolographyChannelTypeMod::QR,ep((int)HolographyChannelTypeMod::QR,"QR","un-documented")));
    m_.insert(std::pair<HolographyChannelTypeMod::HolographyChannelType,EnumPar<void> >
     (HolographyChannelTypeMod::QS,ep((int)HolographyChannelTypeMod::QS,"QS","un-documented")));
    m_.insert(std::pair<HolographyChannelTypeMod::HolographyChannelType,EnumPar<void> >
     (HolographyChannelTypeMod::R2,ep((int)HolographyChannelTypeMod::R2,"R2","un-documented")));
    m_.insert(std::pair<HolographyChannelTypeMod::HolographyChannelType,EnumPar<void> >
     (HolographyChannelTypeMod::RS,ep((int)HolographyChannelTypeMod::RS,"RS","un-documented")));
    m_.insert(std::pair<HolographyChannelTypeMod::HolographyChannelType,EnumPar<void> >
     (HolographyChannelTypeMod::S2,ep((int)HolographyChannelTypeMod::S2,"S2","un-documented")));
    return true;
  }
  static std::map<HolographyChannelTypeMod::HolographyChannelType,EnumPar<void> > m_;
};
#define _HOLOGRAPHYCHANNELTYPE_HH
#endif
