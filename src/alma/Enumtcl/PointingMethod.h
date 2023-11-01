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


#if     !defined(_POINTINGMETHOD_H)

#include <alma/Enumerations/CPointingMethod.h>
#define _POINTINGMETHOD_H
#endif 

#if     !defined(_POINTINGMETHOD_HH)

#include <alma/Enumtcl/Enum.hpp>

template<>
 struct enum_set_traits<PointingMethodMod::PointingMethod> : public enum_set_traiter<PointingMethodMod::PointingMethod,6,PointingMethodMod::HOLOGRAPHY> {};

template<>
class enum_map_traits<PointingMethodMod::PointingMethod,void> : public enum_map_traiter<PointingMethodMod::PointingMethod,void> {
public:
  static bool   init_;
  static std::string typeName_;
  static std::string enumerationDesc_;
  static std::string order_;
  static std::string xsdBaseType_;
  static bool   init(){
    EnumPar<void> ep;
    m_.insert(std::pair<PointingMethodMod::PointingMethod,EnumPar<void> >
     (PointingMethodMod::THREE_POINT,ep((int)PointingMethodMod::THREE_POINT,"THREE_POINT","un-documented")));
    m_.insert(std::pair<PointingMethodMod::PointingMethod,EnumPar<void> >
     (PointingMethodMod::FOUR_POINT,ep((int)PointingMethodMod::FOUR_POINT,"FOUR_POINT","un-documented")));
    m_.insert(std::pair<PointingMethodMod::PointingMethod,EnumPar<void> >
     (PointingMethodMod::FIVE_POINT,ep((int)PointingMethodMod::FIVE_POINT,"FIVE_POINT","un-documented")));
    m_.insert(std::pair<PointingMethodMod::PointingMethod,EnumPar<void> >
     (PointingMethodMod::CROSS,ep((int)PointingMethodMod::CROSS,"CROSS","un-documented")));
    m_.insert(std::pair<PointingMethodMod::PointingMethod,EnumPar<void> >
     (PointingMethodMod::CIRCLE,ep((int)PointingMethodMod::CIRCLE,"CIRCLE","un-documented")));
    m_.insert(std::pair<PointingMethodMod::PointingMethod,EnumPar<void> >
     (PointingMethodMod::HOLOGRAPHY,ep((int)PointingMethodMod::HOLOGRAPHY,"HOLOGRAPHY","un-documented")));
    return true;
  }
  static std::map<PointingMethodMod::PointingMethod,EnumPar<void> > m_;
};
#define _POINTINGMETHOD_HH
#endif
