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


#if     !defined(_ASSOCIATEDCALNATURE_H)

#include <alma/Enumerations/CAssociatedCalNature.h>
#define _ASSOCIATEDCALNATURE_H
#endif 

#if     !defined(_ASSOCIATEDCALNATURE_HH)

#include <alma/Enumtcl/Enum.hpp>

template<>
 struct enum_set_traits<AssociatedCalNatureMod::AssociatedCalNature> : public enum_set_traiter<AssociatedCalNatureMod::AssociatedCalNature,1,AssociatedCalNatureMod::ASSOCIATED_EXECBLOCK> {};

template<>
class enum_map_traits<AssociatedCalNatureMod::AssociatedCalNature,void> : public enum_map_traiter<AssociatedCalNatureMod::AssociatedCalNature,void> {
public:
  static bool   init_;
  static std::string typeName_;
  static std::string enumerationDesc_;
  static std::string order_;
  static std::string xsdBaseType_;
  static bool   init(){
    EnumPar<void> ep;
    m_.insert(std::pair<AssociatedCalNatureMod::AssociatedCalNature,EnumPar<void> >
     (AssociatedCalNatureMod::ASSOCIATED_EXECBLOCK,ep((int)AssociatedCalNatureMod::ASSOCIATED_EXECBLOCK,"ASSOCIATED_EXECBLOCK","un-documented")));
    return true;
  }
  static std::map<AssociatedCalNatureMod::AssociatedCalNature,EnumPar<void> > m_;
};
#define _ASSOCIATEDCALNATURE_HH
#endif
