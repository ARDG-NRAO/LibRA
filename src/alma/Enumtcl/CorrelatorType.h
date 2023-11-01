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


#if     !defined(_CORRELATORTYPE_H)

#include <alma/Enumerations/CCorrelatorType.h>
#define _CORRELATORTYPE_H
#endif 

#if     !defined(_CORRELATORTYPE_HH)

#include <alma/Enumtcl/Enum.hpp>

template<>
 struct enum_set_traits<CorrelatorTypeMod::CorrelatorType> : public enum_set_traiter<CorrelatorTypeMod::CorrelatorType,3,CorrelatorTypeMod::FXF> {};

template<>
class enum_map_traits<CorrelatorTypeMod::CorrelatorType,void> : public enum_map_traiter<CorrelatorTypeMod::CorrelatorType,void> {
public:
  static bool   init_;
  static std::string typeName_;
  static std::string enumerationDesc_;
  static std::string order_;
  static std::string xsdBaseType_;
  static bool   init(){
    EnumPar<void> ep;
    m_.insert(std::pair<CorrelatorTypeMod::CorrelatorType,EnumPar<void> >
     (CorrelatorTypeMod::FX,ep((int)CorrelatorTypeMod::FX,"FX","un-documented")));
    m_.insert(std::pair<CorrelatorTypeMod::CorrelatorType,EnumPar<void> >
     (CorrelatorTypeMod::XF,ep((int)CorrelatorTypeMod::XF,"XF","un-documented")));
    m_.insert(std::pair<CorrelatorTypeMod::CorrelatorType,EnumPar<void> >
     (CorrelatorTypeMod::FXF,ep((int)CorrelatorTypeMod::FXF,"FXF","un-documented")));
    return true;
  }
  static std::map<CorrelatorTypeMod::CorrelatorType,EnumPar<void> > m_;
};
#define _CORRELATORTYPE_HH
#endif
