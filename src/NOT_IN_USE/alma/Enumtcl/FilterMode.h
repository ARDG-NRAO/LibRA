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


#if     !defined(_FILTERMODE_H)

#include <alma/Enumerations/CFilterMode.h>
#define _FILTERMODE_H
#endif 

#if     !defined(_FILTERMODE_HH)

#include <alma/Enumtcl/Enum.hpp>

template<>
 struct enum_set_traits<FilterModeMod::FilterMode> : public enum_set_traiter<FilterModeMod::FilterMode,4,FilterModeMod::UNDEFINED> {};

template<>
class enum_map_traits<FilterModeMod::FilterMode,void> : public enum_map_traiter<FilterModeMod::FilterMode,void> {
public:
  static bool   init_;
  static std::string typeName_;
  static std::string enumerationDesc_;
  static std::string order_;
  static std::string xsdBaseType_;
  static bool   init(){
    EnumPar<void> ep;
    m_.insert(std::pair<FilterModeMod::FilterMode,EnumPar<void> >
     (FilterModeMod::FILTER_NA,ep((int)FilterModeMod::FILTER_NA,"FILTER_NA","un-documented")));
    m_.insert(std::pair<FilterModeMod::FilterMode,EnumPar<void> >
     (FilterModeMod::FILTER_TDM,ep((int)FilterModeMod::FILTER_TDM,"FILTER_TDM","un-documented")));
    m_.insert(std::pair<FilterModeMod::FilterMode,EnumPar<void> >
     (FilterModeMod::FILTER_TFB,ep((int)FilterModeMod::FILTER_TFB,"FILTER_TFB","un-documented")));
    m_.insert(std::pair<FilterModeMod::FilterMode,EnumPar<void> >
     (FilterModeMod::UNDEFINED,ep((int)FilterModeMod::UNDEFINED,"UNDEFINED","un-documented")));
    return true;
  }
  static std::map<FilterModeMod::FilterMode,EnumPar<void> > m_;
};
#define _FILTERMODE_HH
#endif
