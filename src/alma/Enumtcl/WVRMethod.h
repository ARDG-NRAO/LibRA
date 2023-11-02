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


#if     !defined(_WVRMETHOD_H)

#include <alma/Enumerations/CWVRMethod.h>
#define _WVRMETHOD_H
#endif 

#if     !defined(_WVRMETHOD_HH)

#include <alma/Enumtcl/Enum.hpp>

template<>
 struct enum_set_traits<WVRMethodMod::WVRMethod> : public enum_set_traiter<WVRMethodMod::WVRMethod,2,WVRMethodMod::EMPIRICAL> {};

template<>
class enum_map_traits<WVRMethodMod::WVRMethod,void> : public enum_map_traiter<WVRMethodMod::WVRMethod,void> {
public:
  static bool   init_;
  static std::string typeName_;
  static std::string enumerationDesc_;
  static std::string order_;
  static std::string xsdBaseType_;
  static bool   init(){
    EnumPar<void> ep;
    m_.insert(std::pair<WVRMethodMod::WVRMethod,EnumPar<void> >
     (WVRMethodMod::ATM_MODEL,ep((int)WVRMethodMod::ATM_MODEL,"ATM_MODEL","un-documented")));
    m_.insert(std::pair<WVRMethodMod::WVRMethod,EnumPar<void> >
     (WVRMethodMod::EMPIRICAL,ep((int)WVRMethodMod::EMPIRICAL,"EMPIRICAL","un-documented")));
    return true;
  }
  static std::map<WVRMethodMod::WVRMethod,EnumPar<void> > m_;
};
#define _WVRMETHOD_HH
#endif
