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


#if     !defined(_CALDATAORIGIN_H)

#include <alma/Enumerations/CCalDataOrigin.h>
#define _CALDATAORIGIN_H
#endif 

#if     !defined(_CALDATAORIGIN_HH)

#include <alma/Enumtcl/Enum.hpp>

template<>
 struct enum_set_traits<CalDataOriginMod::CalDataOrigin> : public enum_set_traiter<CalDataOriginMod::CalDataOrigin,9,CalDataOriginMod::NONE> {};

template<>
class enum_map_traits<CalDataOriginMod::CalDataOrigin,void> : public enum_map_traiter<CalDataOriginMod::CalDataOrigin,void> {
public:
  static bool   init_;
  static std::string typeName_;
  static std::string enumerationDesc_;
  static std::string order_;
  static std::string xsdBaseType_;
  static bool   init(){
    EnumPar<void> ep;
    m_.insert(std::pair<CalDataOriginMod::CalDataOrigin,EnumPar<void> >
     (CalDataOriginMod::TOTAL_POWER,ep((int)CalDataOriginMod::TOTAL_POWER,"TOTAL_POWER","un-documented")));
    m_.insert(std::pair<CalDataOriginMod::CalDataOrigin,EnumPar<void> >
     (CalDataOriginMod::WVR,ep((int)CalDataOriginMod::WVR,"WVR","un-documented")));
    m_.insert(std::pair<CalDataOriginMod::CalDataOrigin,EnumPar<void> >
     (CalDataOriginMod::CHANNEL_AVERAGE_AUTO,ep((int)CalDataOriginMod::CHANNEL_AVERAGE_AUTO,"CHANNEL_AVERAGE_AUTO","un-documented")));
    m_.insert(std::pair<CalDataOriginMod::CalDataOrigin,EnumPar<void> >
     (CalDataOriginMod::CHANNEL_AVERAGE_CROSS,ep((int)CalDataOriginMod::CHANNEL_AVERAGE_CROSS,"CHANNEL_AVERAGE_CROSS","un-documented")));
    m_.insert(std::pair<CalDataOriginMod::CalDataOrigin,EnumPar<void> >
     (CalDataOriginMod::FULL_RESOLUTION_AUTO,ep((int)CalDataOriginMod::FULL_RESOLUTION_AUTO,"FULL_RESOLUTION_AUTO","un-documented")));
    m_.insert(std::pair<CalDataOriginMod::CalDataOrigin,EnumPar<void> >
     (CalDataOriginMod::FULL_RESOLUTION_CROSS,ep((int)CalDataOriginMod::FULL_RESOLUTION_CROSS,"FULL_RESOLUTION_CROSS","un-documented")));
    m_.insert(std::pair<CalDataOriginMod::CalDataOrigin,EnumPar<void> >
     (CalDataOriginMod::OPTICAL_POINTING,ep((int)CalDataOriginMod::OPTICAL_POINTING,"OPTICAL_POINTING","un-documented")));
    m_.insert(std::pair<CalDataOriginMod::CalDataOrigin,EnumPar<void> >
     (CalDataOriginMod::HOLOGRAPHY,ep((int)CalDataOriginMod::HOLOGRAPHY,"HOLOGRAPHY","un-documented")));
    m_.insert(std::pair<CalDataOriginMod::CalDataOrigin,EnumPar<void> >
     (CalDataOriginMod::NONE,ep((int)CalDataOriginMod::NONE,"NONE","un-documented")));
    return true;
  }
  static std::map<CalDataOriginMod::CalDataOrigin,EnumPar<void> > m_;
};
#define _CALDATAORIGIN_HH
#endif
