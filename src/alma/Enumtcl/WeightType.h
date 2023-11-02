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


#if     !defined(_WEIGHTTYPE_H)

#include <alma/Enumerations/CWeightType.h>
#define _WEIGHTTYPE_H
#endif 

#if     !defined(_WEIGHTTYPE_HH)

#include <alma/Enumtcl/Enum.hpp>

template<>
 struct enum_set_traits<WeightTypeMod::WeightType> : public enum_set_traiter<WeightTypeMod::WeightType,3,WeightTypeMod::COUNT_WEIGHT> {};

template<>
class enum_map_traits<WeightTypeMod::WeightType,void> : public enum_map_traiter<WeightTypeMod::WeightType,void> {
public:
  static bool   init_;
  static std::string typeName_;
  static std::string enumerationDesc_;
  static std::string order_;
  static std::string xsdBaseType_;
  static bool   init(){
    EnumPar<void> ep;
    m_.insert(std::pair<WeightTypeMod::WeightType,EnumPar<void> >
     (WeightTypeMod::K,ep((int)WeightTypeMod::K,"K","un-documented")));
    m_.insert(std::pair<WeightTypeMod::WeightType,EnumPar<void> >
     (WeightTypeMod::JY,ep((int)WeightTypeMod::JY,"JY","un-documented")));
    m_.insert(std::pair<WeightTypeMod::WeightType,EnumPar<void> >
     (WeightTypeMod::COUNT_WEIGHT,ep((int)WeightTypeMod::COUNT_WEIGHT,"COUNT_WEIGHT","un-documented")));
    return true;
  }
  static std::map<WeightTypeMod::WeightType,EnumPar<void> > m_;
};
#define _WEIGHTTYPE_HH
#endif
