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


#if     !defined(_RADIALVELOCITYREFERENCECODE_H)

#include <alma/Enumerations/CRadialVelocityReferenceCode.h>
#define _RADIALVELOCITYREFERENCECODE_H
#endif 

#if     !defined(_RADIALVELOCITYREFERENCECODE_HH)

#include <alma/Enumtcl/Enum.hpp>

template<>
 struct enum_set_traits<RadialVelocityReferenceCodeMod::RadialVelocityReferenceCode> : public enum_set_traiter<RadialVelocityReferenceCodeMod::RadialVelocityReferenceCode,6,RadialVelocityReferenceCodeMod::TOPO> {};

template<>
class enum_map_traits<RadialVelocityReferenceCodeMod::RadialVelocityReferenceCode,void> : public enum_map_traiter<RadialVelocityReferenceCodeMod::RadialVelocityReferenceCode,void> {
public:
  static bool   init_;
  static std::string typeName_;
  static std::string enumerationDesc_;
  static std::string order_;
  static std::string xsdBaseType_;
  static bool   init(){
    EnumPar<void> ep;
    m_.insert(std::pair<RadialVelocityReferenceCodeMod::RadialVelocityReferenceCode,EnumPar<void> >
     (RadialVelocityReferenceCodeMod::LSRD,ep((int)RadialVelocityReferenceCodeMod::LSRD,"LSRD","un-documented")));
    m_.insert(std::pair<RadialVelocityReferenceCodeMod::RadialVelocityReferenceCode,EnumPar<void> >
     (RadialVelocityReferenceCodeMod::LSRK,ep((int)RadialVelocityReferenceCodeMod::LSRK,"LSRK","un-documented")));
    m_.insert(std::pair<RadialVelocityReferenceCodeMod::RadialVelocityReferenceCode,EnumPar<void> >
     (RadialVelocityReferenceCodeMod::GALACTO,ep((int)RadialVelocityReferenceCodeMod::GALACTO,"GALACTO","un-documented")));
    m_.insert(std::pair<RadialVelocityReferenceCodeMod::RadialVelocityReferenceCode,EnumPar<void> >
     (RadialVelocityReferenceCodeMod::BARY,ep((int)RadialVelocityReferenceCodeMod::BARY,"BARY","un-documented")));
    m_.insert(std::pair<RadialVelocityReferenceCodeMod::RadialVelocityReferenceCode,EnumPar<void> >
     (RadialVelocityReferenceCodeMod::GEO,ep((int)RadialVelocityReferenceCodeMod::GEO,"GEO","un-documented")));
    m_.insert(std::pair<RadialVelocityReferenceCodeMod::RadialVelocityReferenceCode,EnumPar<void> >
     (RadialVelocityReferenceCodeMod::TOPO,ep((int)RadialVelocityReferenceCodeMod::TOPO,"TOPO","un-documented")));
    return true;
  }
  static std::map<RadialVelocityReferenceCodeMod::RadialVelocityReferenceCode,EnumPar<void> > m_;
};
#define _RADIALVELOCITYREFERENCECODE_HH
#endif
