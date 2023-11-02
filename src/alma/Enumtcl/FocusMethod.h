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


#if     !defined(_FOCUSMETHOD_H)

#include <alma/Enumerations/CFocusMethod.h>
#define _FOCUSMETHOD_H
#endif 

#if     !defined(_FOCUSMETHOD_HH)

#include <alma/Enumtcl/Enum.hpp>

template<>
 struct enum_set_traits<FocusMethodMod::FocusMethod> : public enum_set_traiter<FocusMethodMod::FocusMethod,3,FocusMethodMod::HOLOGRAPHY> {};

template<>
class enum_map_traits<FocusMethodMod::FocusMethod,void> : public enum_map_traiter<FocusMethodMod::FocusMethod,void> {
public:
  static bool   init_;
  static std::string typeName_;
  static std::string enumerationDesc_;
  static std::string order_;
  static std::string xsdBaseType_;
  static bool   init(){
    EnumPar<void> ep;
    m_.insert(std::pair<FocusMethodMod::FocusMethod,EnumPar<void> >
     (FocusMethodMod::THREE_POINT,ep((int)FocusMethodMod::THREE_POINT,"THREE_POINT","un-documented")));
    m_.insert(std::pair<FocusMethodMod::FocusMethod,EnumPar<void> >
     (FocusMethodMod::FIVE_POINT,ep((int)FocusMethodMod::FIVE_POINT,"FIVE_POINT","un-documented")));
    m_.insert(std::pair<FocusMethodMod::FocusMethod,EnumPar<void> >
     (FocusMethodMod::HOLOGRAPHY,ep((int)FocusMethodMod::HOLOGRAPHY,"HOLOGRAPHY","un-documented")));
    return true;
  }
  static std::map<FocusMethodMod::FocusMethod,EnumPar<void> > m_;
};
#define _FOCUSMETHOD_HH
#endif
