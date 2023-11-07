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


#if     !defined(_NETSIDEBAND_H)

#include <alma/Enumerations/CNetSideband.h>
#define _NETSIDEBAND_H
#endif 

#if     !defined(_NETSIDEBAND_HH)

#include <alma/Enumtcl/Enum.hpp>

template<>
 struct enum_set_traits<NetSidebandMod::NetSideband> : public enum_set_traiter<NetSidebandMod::NetSideband,4,NetSidebandMod::DSB> {};

template<>
class enum_map_traits<NetSidebandMod::NetSideband,void> : public enum_map_traiter<NetSidebandMod::NetSideband,void> {
public:
  static bool   init_;
  static std::string typeName_;
  static std::string enumerationDesc_;
  static std::string order_;
  static std::string xsdBaseType_;
  static bool   init(){
    EnumPar<void> ep;
    m_.insert(std::pair<NetSidebandMod::NetSideband,EnumPar<void> >
     (NetSidebandMod::NOSB,ep((int)NetSidebandMod::NOSB,"NOSB","un-documented")));
    m_.insert(std::pair<NetSidebandMod::NetSideband,EnumPar<void> >
     (NetSidebandMod::LSB,ep((int)NetSidebandMod::LSB,"LSB","un-documented")));
    m_.insert(std::pair<NetSidebandMod::NetSideband,EnumPar<void> >
     (NetSidebandMod::USB,ep((int)NetSidebandMod::USB,"USB","un-documented")));
    m_.insert(std::pair<NetSidebandMod::NetSideband,EnumPar<void> >
     (NetSidebandMod::DSB,ep((int)NetSidebandMod::DSB,"DSB","un-documented")));
    return true;
  }
  static std::map<NetSidebandMod::NetSideband,EnumPar<void> > m_;
};
#define _NETSIDEBAND_HH
#endif
