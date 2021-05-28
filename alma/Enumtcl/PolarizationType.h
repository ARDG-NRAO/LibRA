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


#if     !defined(_POLARIZATIONTYPE_H)

#include <alma/Enumerations/CPolarizationType.h>
#define _POLARIZATIONTYPE_H
#endif 

#if     !defined(_POLARIZATIONTYPE_HH)

#include <alma/Enumtcl/Enum.hpp>

template<>
 struct enum_set_traits<PolarizationTypeMod::PolarizationType> : public enum_set_traiter<PolarizationTypeMod::PolarizationType,5,PolarizationTypeMod::BOTH> {};

template<>
class enum_map_traits<PolarizationTypeMod::PolarizationType,void> : public enum_map_traiter<PolarizationTypeMod::PolarizationType,void> {
public:
  static bool   init_;
  static std::string typeName_;
  static std::string enumerationDesc_;
  static std::string order_;
  static std::string xsdBaseType_;
  static bool   init(){
    EnumPar<void> ep;
    m_.insert(std::pair<PolarizationTypeMod::PolarizationType,EnumPar<void> >
     (PolarizationTypeMod::R,ep((int)PolarizationTypeMod::R,"R","un-documented")));
    m_.insert(std::pair<PolarizationTypeMod::PolarizationType,EnumPar<void> >
     (PolarizationTypeMod::L,ep((int)PolarizationTypeMod::L,"L","un-documented")));
    m_.insert(std::pair<PolarizationTypeMod::PolarizationType,EnumPar<void> >
     (PolarizationTypeMod::X,ep((int)PolarizationTypeMod::X,"X","un-documented")));
    m_.insert(std::pair<PolarizationTypeMod::PolarizationType,EnumPar<void> >
     (PolarizationTypeMod::Y,ep((int)PolarizationTypeMod::Y,"Y","un-documented")));
    m_.insert(std::pair<PolarizationTypeMod::PolarizationType,EnumPar<void> >
     (PolarizationTypeMod::BOTH,ep((int)PolarizationTypeMod::BOTH,"BOTH","un-documented")));
    return true;
  }
  static std::map<PolarizationTypeMod::PolarizationType,EnumPar<void> > m_;
};
#define _POLARIZATIONTYPE_HH
#endif
