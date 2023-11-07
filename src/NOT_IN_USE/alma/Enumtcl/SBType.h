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


#if     !defined(_SBTYPE_H)

#include <alma/Enumerations/CSBType.h>
#define _SBTYPE_H
#endif 

#if     !defined(_SBTYPE_HH)

#include <alma/Enumtcl/Enum.hpp>

template<>
 struct enum_set_traits<SBTypeMod::SBType> : public enum_set_traiter<SBTypeMod::SBType,3,SBTypeMod::EXPERT> {};

template<>
class enum_map_traits<SBTypeMod::SBType,void> : public enum_map_traiter<SBTypeMod::SBType,void> {
public:
  static bool   init_;
  static std::string typeName_;
  static std::string enumerationDesc_;
  static std::string order_;
  static std::string xsdBaseType_;
  static bool   init(){
    EnumPar<void> ep;
    m_.insert(std::pair<SBTypeMod::SBType,EnumPar<void> >
     (SBTypeMod::OBSERVATORY,ep((int)SBTypeMod::OBSERVATORY,"OBSERVATORY","un-documented")));
    m_.insert(std::pair<SBTypeMod::SBType,EnumPar<void> >
     (SBTypeMod::OBSERVER,ep((int)SBTypeMod::OBSERVER,"OBSERVER","un-documented")));
    m_.insert(std::pair<SBTypeMod::SBType,EnumPar<void> >
     (SBTypeMod::EXPERT,ep((int)SBTypeMod::EXPERT,"EXPERT","un-documented")));
    return true;
  }
  static std::map<SBTypeMod::SBType,EnumPar<void> > m_;
};
#define _SBTYPE_HH
#endif
