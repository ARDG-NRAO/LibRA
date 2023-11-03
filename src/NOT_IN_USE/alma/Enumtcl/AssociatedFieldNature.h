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


#if     !defined(_ASSOCIATEDFIELDNATURE_H)

#include <alma/Enumerations/CAssociatedFieldNature.h>
#define _ASSOCIATEDFIELDNATURE_H
#endif 

#if     !defined(_ASSOCIATEDFIELDNATURE_HH)

#include <alma/Enumtcl/Enum.hpp>

template<>
 struct enum_set_traits<AssociatedFieldNatureMod::AssociatedFieldNature> : public enum_set_traiter<AssociatedFieldNatureMod::AssociatedFieldNature,3,AssociatedFieldNatureMod::PHASE_REFERENCE> {};

template<>
class enum_map_traits<AssociatedFieldNatureMod::AssociatedFieldNature,void> : public enum_map_traiter<AssociatedFieldNatureMod::AssociatedFieldNature,void> {
public:
  static bool   init_;
  static std::string typeName_;
  static std::string enumerationDesc_;
  static std::string order_;
  static std::string xsdBaseType_;
  static bool   init(){
    EnumPar<void> ep;
    m_.insert(std::pair<AssociatedFieldNatureMod::AssociatedFieldNature,EnumPar<void> >
     (AssociatedFieldNatureMod::ON,ep((int)AssociatedFieldNatureMod::ON,"ON","un-documented")));
    m_.insert(std::pair<AssociatedFieldNatureMod::AssociatedFieldNature,EnumPar<void> >
     (AssociatedFieldNatureMod::OFF,ep((int)AssociatedFieldNatureMod::OFF,"OFF","un-documented")));
    m_.insert(std::pair<AssociatedFieldNatureMod::AssociatedFieldNature,EnumPar<void> >
     (AssociatedFieldNatureMod::PHASE_REFERENCE,ep((int)AssociatedFieldNatureMod::PHASE_REFERENCE,"PHASE_REFERENCE","un-documented")));
    return true;
  }
  static std::map<AssociatedFieldNatureMod::AssociatedFieldNature,EnumPar<void> > m_;
};
#define _ASSOCIATEDFIELDNATURE_HH
#endif
