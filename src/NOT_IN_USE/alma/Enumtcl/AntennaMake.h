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


#if     !defined(_ANTENNAMAKE_H)

#include <alma/Enumerations/CAntennaMake.h>
#define _ANTENNAMAKE_H
#endif 

#if     !defined(_ANTENNAMAKE_HH)

#include <alma/Enumtcl/Enum.hpp>

template<>
 struct enum_set_traits<AntennaMakeMod::AntennaMake> : public enum_set_traiter<AntennaMakeMod::AntennaMake,9,AntennaMakeMod::UNDEFINED> {};

template<>
class enum_map_traits<AntennaMakeMod::AntennaMake,void> : public enum_map_traiter<AntennaMakeMod::AntennaMake,void> {
public:
  static bool   init_;
  static std::string typeName_;
  static std::string enumerationDesc_;
  static std::string order_;
  static std::string xsdBaseType_;
  static bool   init(){
    EnumPar<void> ep;
    m_.insert(std::pair<AntennaMakeMod::AntennaMake,EnumPar<void> >
     (AntennaMakeMod::AEM_12,ep((int)AntennaMakeMod::AEM_12,"AEM_12","un-documented")));
    m_.insert(std::pair<AntennaMakeMod::AntennaMake,EnumPar<void> >
     (AntennaMakeMod::MITSUBISHI_7,ep((int)AntennaMakeMod::MITSUBISHI_7,"MITSUBISHI_7","un-documented")));
    m_.insert(std::pair<AntennaMakeMod::AntennaMake,EnumPar<void> >
     (AntennaMakeMod::MITSUBISHI_12_A,ep((int)AntennaMakeMod::MITSUBISHI_12_A,"MITSUBISHI_12_A","un-documented")));
    m_.insert(std::pair<AntennaMakeMod::AntennaMake,EnumPar<void> >
     (AntennaMakeMod::MITSUBISHI_12_B,ep((int)AntennaMakeMod::MITSUBISHI_12_B,"MITSUBISHI_12_B","un-documented")));
    m_.insert(std::pair<AntennaMakeMod::AntennaMake,EnumPar<void> >
     (AntennaMakeMod::VERTEX_12_ATF,ep((int)AntennaMakeMod::VERTEX_12_ATF,"VERTEX_12_ATF","un-documented")));
    m_.insert(std::pair<AntennaMakeMod::AntennaMake,EnumPar<void> >
     (AntennaMakeMod::AEM_12_ATF,ep((int)AntennaMakeMod::AEM_12_ATF,"AEM_12_ATF","un-documented")));
    m_.insert(std::pair<AntennaMakeMod::AntennaMake,EnumPar<void> >
     (AntennaMakeMod::VERTEX_12,ep((int)AntennaMakeMod::VERTEX_12,"VERTEX_12","un-documented")));
    m_.insert(std::pair<AntennaMakeMod::AntennaMake,EnumPar<void> >
     (AntennaMakeMod::IRAM_15,ep((int)AntennaMakeMod::IRAM_15,"IRAM_15","un-documented")));
    m_.insert(std::pair<AntennaMakeMod::AntennaMake,EnumPar<void> >
     (AntennaMakeMod::UNDEFINED,ep((int)AntennaMakeMod::UNDEFINED,"UNDEFINED","un-documented")));
    return true;
  }
  static std::map<AntennaMakeMod::AntennaMake,EnumPar<void> > m_;
};
#define _ANTENNAMAKE_HH
#endif
