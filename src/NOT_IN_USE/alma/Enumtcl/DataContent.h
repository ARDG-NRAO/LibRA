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


#if     !defined(_DATACONTENT_H)

#include <alma/Enumerations/CDataContent.h>
#define _DATACONTENT_H
#endif 

#if     !defined(_DATACONTENT_HH)

#include <alma/Enumtcl/Enum.hpp>

template<>
 struct enum_set_traits<DataContentMod::DataContent> : public enum_set_traiter<DataContentMod::DataContent,7,DataContentMod::FLAGS> {};

template<>
class enum_map_traits<DataContentMod::DataContent,void> : public enum_map_traiter<DataContentMod::DataContent,void> {
public:
  static bool   init_;
  static std::string typeName_;
  static std::string enumerationDesc_;
  static std::string order_;
  static std::string xsdBaseType_;
  static bool   init(){
    EnumPar<void> ep;
    m_.insert(std::pair<DataContentMod::DataContent,EnumPar<void> >
     (DataContentMod::CROSS_DATA,ep((int)DataContentMod::CROSS_DATA,"CROSS_DATA","un-documented")));
    m_.insert(std::pair<DataContentMod::DataContent,EnumPar<void> >
     (DataContentMod::AUTO_DATA,ep((int)DataContentMod::AUTO_DATA,"AUTO_DATA","un-documented")));
    m_.insert(std::pair<DataContentMod::DataContent,EnumPar<void> >
     (DataContentMod::ZERO_LAGS,ep((int)DataContentMod::ZERO_LAGS,"ZERO_LAGS","un-documented")));
    m_.insert(std::pair<DataContentMod::DataContent,EnumPar<void> >
     (DataContentMod::ACTUAL_TIMES,ep((int)DataContentMod::ACTUAL_TIMES,"ACTUAL_TIMES","un-documented")));
    m_.insert(std::pair<DataContentMod::DataContent,EnumPar<void> >
     (DataContentMod::ACTUAL_DURATIONS,ep((int)DataContentMod::ACTUAL_DURATIONS,"ACTUAL_DURATIONS","un-documented")));
    m_.insert(std::pair<DataContentMod::DataContent,EnumPar<void> >
     (DataContentMod::WEIGHTS,ep((int)DataContentMod::WEIGHTS,"WEIGHTS","un-documented")));
    m_.insert(std::pair<DataContentMod::DataContent,EnumPar<void> >
     (DataContentMod::FLAGS,ep((int)DataContentMod::FLAGS,"FLAGS","un-documented")));
    return true;
  }
  static std::map<DataContentMod::DataContent,EnumPar<void> > m_;
};
#define _DATACONTENT_HH
#endif
