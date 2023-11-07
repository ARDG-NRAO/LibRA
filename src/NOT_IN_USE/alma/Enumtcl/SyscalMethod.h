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


#if     !defined(_SYSCALMETHOD_H)

#include <alma/Enumerations/CSyscalMethod.h>
#define _SYSCALMETHOD_H
#endif 

#if     !defined(_SYSCALMETHOD_HH)

#include <alma/Enumtcl/Enum.hpp>

template<>
 struct enum_set_traits<SyscalMethodMod::SyscalMethod> : public enum_set_traiter<SyscalMethodMod::SyscalMethod,3,SyscalMethodMod::SIDEBAND_RATIO> {};

template<>
class enum_map_traits<SyscalMethodMod::SyscalMethod,void> : public enum_map_traiter<SyscalMethodMod::SyscalMethod,void> {
public:
  static bool   init_;
  static std::string typeName_;
  static std::string enumerationDesc_;
  static std::string order_;
  static std::string xsdBaseType_;
  static bool   init(){
    EnumPar<void> ep;
    m_.insert(std::pair<SyscalMethodMod::SyscalMethod,EnumPar<void> >
     (SyscalMethodMod::TEMPERATURE_SCALE,ep((int)SyscalMethodMod::TEMPERATURE_SCALE,"TEMPERATURE_SCALE","un-documented")));
    m_.insert(std::pair<SyscalMethodMod::SyscalMethod,EnumPar<void> >
     (SyscalMethodMod::SKYDIP,ep((int)SyscalMethodMod::SKYDIP,"SKYDIP","un-documented")));
    m_.insert(std::pair<SyscalMethodMod::SyscalMethod,EnumPar<void> >
     (SyscalMethodMod::SIDEBAND_RATIO,ep((int)SyscalMethodMod::SIDEBAND_RATIO,"SIDEBAND_RATIO","un-documented")));
    return true;
  }
  static std::map<SyscalMethodMod::SyscalMethod,EnumPar<void> > m_;
};
#define _SYSCALMETHOD_HH
#endif
