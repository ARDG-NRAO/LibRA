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


#if     !defined(_SUBSCANINTENT_H)

#include <alma/Enumerations/CSubscanIntent.h>
#define _SUBSCANINTENT_H
#endif 

#if     !defined(_SUBSCANINTENT_HH)

#include <alma/Enumtcl/Enum.hpp>

template<>
 struct enum_set_traits<SubscanIntentMod::SubscanIntent> : public enum_set_traiter<SubscanIntentMod::SubscanIntent,11,SubscanIntentMod::UNSPECIFIED> {};

template<>
class enum_map_traits<SubscanIntentMod::SubscanIntent,void> : public enum_map_traiter<SubscanIntentMod::SubscanIntent,void> {
public:
  static bool   init_;
  static std::string typeName_;
  static std::string enumerationDesc_;
  static std::string order_;
  static std::string xsdBaseType_;
  static bool   init(){
    EnumPar<void> ep;
    m_.insert(std::pair<SubscanIntentMod::SubscanIntent,EnumPar<void> >
     (SubscanIntentMod::ON_SOURCE,ep((int)SubscanIntentMod::ON_SOURCE,"ON_SOURCE","un-documented")));
    m_.insert(std::pair<SubscanIntentMod::SubscanIntent,EnumPar<void> >
     (SubscanIntentMod::OFF_SOURCE,ep((int)SubscanIntentMod::OFF_SOURCE,"OFF_SOURCE","un-documented")));
    m_.insert(std::pair<SubscanIntentMod::SubscanIntent,EnumPar<void> >
     (SubscanIntentMod::MIXED,ep((int)SubscanIntentMod::MIXED,"MIXED","un-documented")));
    m_.insert(std::pair<SubscanIntentMod::SubscanIntent,EnumPar<void> >
     (SubscanIntentMod::REFERENCE,ep((int)SubscanIntentMod::REFERENCE,"REFERENCE","un-documented")));
    m_.insert(std::pair<SubscanIntentMod::SubscanIntent,EnumPar<void> >
     (SubscanIntentMod::SCANNING,ep((int)SubscanIntentMod::SCANNING,"SCANNING","un-documented")));
    m_.insert(std::pair<SubscanIntentMod::SubscanIntent,EnumPar<void> >
     (SubscanIntentMod::HOT,ep((int)SubscanIntentMod::HOT,"HOT","un-documented")));
    m_.insert(std::pair<SubscanIntentMod::SubscanIntent,EnumPar<void> >
     (SubscanIntentMod::AMBIENT,ep((int)SubscanIntentMod::AMBIENT,"AMBIENT","un-documented")));
    m_.insert(std::pair<SubscanIntentMod::SubscanIntent,EnumPar<void> >
     (SubscanIntentMod::SIGNAL,ep((int)SubscanIntentMod::SIGNAL,"SIGNAL","un-documented")));
    m_.insert(std::pair<SubscanIntentMod::SubscanIntent,EnumPar<void> >
     (SubscanIntentMod::IMAGE,ep((int)SubscanIntentMod::IMAGE,"IMAGE","un-documented")));
    m_.insert(std::pair<SubscanIntentMod::SubscanIntent,EnumPar<void> >
     (SubscanIntentMod::TEST,ep((int)SubscanIntentMod::TEST,"TEST","un-documented")));
    m_.insert(std::pair<SubscanIntentMod::SubscanIntent,EnumPar<void> >
     (SubscanIntentMod::UNSPECIFIED,ep((int)SubscanIntentMod::UNSPECIFIED,"UNSPECIFIED","un-documented")));
    return true;
  }
  static std::map<SubscanIntentMod::SubscanIntent,EnumPar<void> > m_;
};
#define _SUBSCANINTENT_HH
#endif
