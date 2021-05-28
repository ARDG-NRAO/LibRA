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


#if     !defined(_PRIMITIVEDATATYPE_H)

#include <alma/Enumerations/CPrimitiveDataType.h>
#define _PRIMITIVEDATATYPE_H
#endif 

#if     !defined(_PRIMITIVEDATATYPE_HH)

#include <alma/Enumtcl/Enum.hpp>

template<>
 struct enum_set_traits<PrimitiveDataTypeMod::PrimitiveDataType> : public enum_set_traiter<PrimitiveDataTypeMod::PrimitiveDataType,5,PrimitiveDataTypeMod::FLOAT64_TYPE> {};

template<>
class enum_map_traits<PrimitiveDataTypeMod::PrimitiveDataType,void> : public enum_map_traiter<PrimitiveDataTypeMod::PrimitiveDataType,void> {
public:
  static bool   init_;
  static std::string typeName_;
  static std::string enumerationDesc_;
  static std::string order_;
  static std::string xsdBaseType_;
  static bool   init(){
    EnumPar<void> ep;
    m_.insert(std::pair<PrimitiveDataTypeMod::PrimitiveDataType,EnumPar<void> >
     (PrimitiveDataTypeMod::INT16_TYPE,ep((int)PrimitiveDataTypeMod::INT16_TYPE,"INT16_TYPE","un-documented")));
    m_.insert(std::pair<PrimitiveDataTypeMod::PrimitiveDataType,EnumPar<void> >
     (PrimitiveDataTypeMod::INT32_TYPE,ep((int)PrimitiveDataTypeMod::INT32_TYPE,"INT32_TYPE","un-documented")));
    m_.insert(std::pair<PrimitiveDataTypeMod::PrimitiveDataType,EnumPar<void> >
     (PrimitiveDataTypeMod::INT64_TYPE,ep((int)PrimitiveDataTypeMod::INT64_TYPE,"INT64_TYPE","un-documented")));
    m_.insert(std::pair<PrimitiveDataTypeMod::PrimitiveDataType,EnumPar<void> >
     (PrimitiveDataTypeMod::FLOAT32_TYPE,ep((int)PrimitiveDataTypeMod::FLOAT32_TYPE,"FLOAT32_TYPE","un-documented")));
    m_.insert(std::pair<PrimitiveDataTypeMod::PrimitiveDataType,EnumPar<void> >
     (PrimitiveDataTypeMod::FLOAT64_TYPE,ep((int)PrimitiveDataTypeMod::FLOAT64_TYPE,"FLOAT64_TYPE","un-documented")));
    return true;
  }
  static std::map<PrimitiveDataTypeMod::PrimitiveDataType,EnumPar<void> > m_;
};
#define _PRIMITIVEDATATYPE_HH
#endif
