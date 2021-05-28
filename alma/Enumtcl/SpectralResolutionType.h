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


#if     !defined(_SPECTRALRESOLUTIONTYPE_H)

#include <alma/Enumerations/CSpectralResolutionType.h>
#define _SPECTRALRESOLUTIONTYPE_H
#endif 

#if     !defined(_SPECTRALRESOLUTIONTYPE_HH)

#include <alma/Enumtcl/Enum.hpp>

template<>
 struct enum_set_traits<SpectralResolutionTypeMod::SpectralResolutionType> : public enum_set_traiter<SpectralResolutionTypeMod::SpectralResolutionType,3,SpectralResolutionTypeMod::FULL_RESOLUTION> {};

template<>
class enum_map_traits<SpectralResolutionTypeMod::SpectralResolutionType,void> : public enum_map_traiter<SpectralResolutionTypeMod::SpectralResolutionType,void> {
public:
  static bool   init_;
  static std::string typeName_;
  static std::string enumerationDesc_;
  static std::string order_;
  static std::string xsdBaseType_;
  static bool   init(){
    EnumPar<void> ep;
    m_.insert(std::pair<SpectralResolutionTypeMod::SpectralResolutionType,EnumPar<void> >
     (SpectralResolutionTypeMod::CHANNEL_AVERAGE,ep((int)SpectralResolutionTypeMod::CHANNEL_AVERAGE,"CHANNEL_AVERAGE","un-documented")));
    m_.insert(std::pair<SpectralResolutionTypeMod::SpectralResolutionType,EnumPar<void> >
     (SpectralResolutionTypeMod::BASEBAND_WIDE,ep((int)SpectralResolutionTypeMod::BASEBAND_WIDE,"BASEBAND_WIDE","un-documented")));
    m_.insert(std::pair<SpectralResolutionTypeMod::SpectralResolutionType,EnumPar<void> >
     (SpectralResolutionTypeMod::FULL_RESOLUTION,ep((int)SpectralResolutionTypeMod::FULL_RESOLUTION,"FULL_RESOLUTION","un-documented")));
    return true;
  }
  static std::map<SpectralResolutionTypeMod::SpectralResolutionType,EnumPar<void> > m_;
};
#define _SPECTRALRESOLUTIONTYPE_HH
#endif
