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


#if     !defined(_CORRELATIONBIT_H)

#include <alma/Enumerations/CCorrelationBit.h>
#define _CORRELATIONBIT_H
#endif 

#if     !defined(_CORRELATIONBIT_HH)

#include <alma/Enumtcl/Enum.hpp>

template<>
 struct enum_set_traits<CorrelationBitMod::CorrelationBit> : public enum_set_traiter<CorrelationBitMod::CorrelationBit,3,CorrelationBitMod::BITS_4x4> {};

template<>
class enum_map_traits<CorrelationBitMod::CorrelationBit,void> : public enum_map_traiter<CorrelationBitMod::CorrelationBit,void> {
public:
  static bool   init_;
  static std::string typeName_;
  static std::string enumerationDesc_;
  static std::string order_;
  static std::string xsdBaseType_;
  static bool   init(){
    EnumPar<void> ep;
    m_.insert(std::pair<CorrelationBitMod::CorrelationBit,EnumPar<void> >
     (CorrelationBitMod::BITS_2x2,ep((int)CorrelationBitMod::BITS_2x2,"BITS_2x2","un-documented")));
    m_.insert(std::pair<CorrelationBitMod::CorrelationBit,EnumPar<void> >
     (CorrelationBitMod::BITS_3x3,ep((int)CorrelationBitMod::BITS_3x3,"BITS_3x3","un-documented")));
    m_.insert(std::pair<CorrelationBitMod::CorrelationBit,EnumPar<void> >
     (CorrelationBitMod::BITS_4x4,ep((int)CorrelationBitMod::BITS_4x4,"BITS_4x4","un-documented")));
    return true;
  }
  static std::map<CorrelationBitMod::CorrelationBit,EnumPar<void> > m_;
};
#define _CORRELATIONBIT_HH
#endif
