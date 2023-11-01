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


#if     !defined(_CORRELATIONMODE_H)

#include <alma/Enumerations/CCorrelationMode.h>
#define _CORRELATIONMODE_H
#endif 

#if     !defined(_CORRELATIONMODE_HH)

#include <alma/Enumtcl/Enum.hpp>

template<>
 struct enum_set_traits<CorrelationModeMod::CorrelationMode> : public enum_set_traiter<CorrelationModeMod::CorrelationMode,3,CorrelationModeMod::CROSS_AND_AUTO> {};

template<>
class enum_map_traits<CorrelationModeMod::CorrelationMode,void> : public enum_map_traiter<CorrelationModeMod::CorrelationMode,void> {
public:
  static bool   init_;
  static std::string typeName_;
  static std::string enumerationDesc_;
  static std::string order_;
  static std::string xsdBaseType_;
  static bool   init(){
    EnumPar<void> ep;
    m_.insert(std::pair<CorrelationModeMod::CorrelationMode,EnumPar<void> >
     (CorrelationModeMod::CROSS_ONLY,ep((int)CorrelationModeMod::CROSS_ONLY,"CROSS_ONLY","un-documented")));
    m_.insert(std::pair<CorrelationModeMod::CorrelationMode,EnumPar<void> >
     (CorrelationModeMod::AUTO_ONLY,ep((int)CorrelationModeMod::AUTO_ONLY,"AUTO_ONLY","un-documented")));
    m_.insert(std::pair<CorrelationModeMod::CorrelationMode,EnumPar<void> >
     (CorrelationModeMod::CROSS_AND_AUTO,ep((int)CorrelationModeMod::CROSS_AND_AUTO,"CROSS_AND_AUTO","un-documented")));
    return true;
  }
  static std::map<CorrelationModeMod::CorrelationMode,EnumPar<void> > m_;
};
#define _CORRELATIONMODE_HH
#endif
