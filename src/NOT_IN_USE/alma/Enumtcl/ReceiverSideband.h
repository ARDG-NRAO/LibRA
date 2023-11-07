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


#if     !defined(_RECEIVERSIDEBAND_H)

#include <alma/Enumerations/CReceiverSideband.h>
#define _RECEIVERSIDEBAND_H
#endif 

#if     !defined(_RECEIVERSIDEBAND_HH)

#include <alma/Enumtcl/Enum.hpp>

template<>
 struct enum_set_traits<ReceiverSidebandMod::ReceiverSideband> : public enum_set_traiter<ReceiverSidebandMod::ReceiverSideband,4,ReceiverSidebandMod::TSB> {};

template<>
class enum_map_traits<ReceiverSidebandMod::ReceiverSideband,void> : public enum_map_traiter<ReceiverSidebandMod::ReceiverSideband,void> {
public:
  static bool   init_;
  static std::string typeName_;
  static std::string enumerationDesc_;
  static std::string order_;
  static std::string xsdBaseType_;
  static bool   init(){
    EnumPar<void> ep;
    m_.insert(std::pair<ReceiverSidebandMod::ReceiverSideband,EnumPar<void> >
     (ReceiverSidebandMod::NOSB,ep((int)ReceiverSidebandMod::NOSB,"NOSB","un-documented")));
    m_.insert(std::pair<ReceiverSidebandMod::ReceiverSideband,EnumPar<void> >
     (ReceiverSidebandMod::DSB,ep((int)ReceiverSidebandMod::DSB,"DSB","un-documented")));
    m_.insert(std::pair<ReceiverSidebandMod::ReceiverSideband,EnumPar<void> >
     (ReceiverSidebandMod::SSB,ep((int)ReceiverSidebandMod::SSB,"SSB","un-documented")));
    m_.insert(std::pair<ReceiverSidebandMod::ReceiverSideband,EnumPar<void> >
     (ReceiverSidebandMod::TSB,ep((int)ReceiverSidebandMod::TSB,"TSB","un-documented")));
    return true;
  }
  static std::map<ReceiverSidebandMod::ReceiverSideband,EnumPar<void> > m_;
};
#define _RECEIVERSIDEBAND_HH
#endif
