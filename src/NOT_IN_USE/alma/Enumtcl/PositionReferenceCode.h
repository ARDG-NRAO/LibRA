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


#if     !defined(_POSITIONREFERENCECODE_H)

#include <alma/Enumerations/CPositionReferenceCode.h>
#define _POSITIONREFERENCECODE_H
#endif 

#if     !defined(_POSITIONREFERENCECODE_HH)

#include <alma/Enumtcl/Enum.hpp>

template<>
 struct enum_set_traits<PositionReferenceCodeMod::PositionReferenceCode> : public enum_set_traiter<PositionReferenceCodeMod::PositionReferenceCode,6,PositionReferenceCodeMod::REFLECTOR> {};

template<>
class enum_map_traits<PositionReferenceCodeMod::PositionReferenceCode,void> : public enum_map_traiter<PositionReferenceCodeMod::PositionReferenceCode,void> {
public:
  static bool   init_;
  static std::string typeName_;
  static std::string enumerationDesc_;
  static std::string order_;
  static std::string xsdBaseType_;
  static bool   init(){
    EnumPar<void> ep;
    m_.insert(std::pair<PositionReferenceCodeMod::PositionReferenceCode,EnumPar<void> >
     (PositionReferenceCodeMod::ITRF,ep((int)PositionReferenceCodeMod::ITRF,"ITRF","un-documented")));
    m_.insert(std::pair<PositionReferenceCodeMod::PositionReferenceCode,EnumPar<void> >
     (PositionReferenceCodeMod::WGS84,ep((int)PositionReferenceCodeMod::WGS84,"WGS84","un-documented")));
    m_.insert(std::pair<PositionReferenceCodeMod::PositionReferenceCode,EnumPar<void> >
     (PositionReferenceCodeMod::SITE,ep((int)PositionReferenceCodeMod::SITE,"SITE","un-documented")));
    m_.insert(std::pair<PositionReferenceCodeMod::PositionReferenceCode,EnumPar<void> >
     (PositionReferenceCodeMod::STATION,ep((int)PositionReferenceCodeMod::STATION,"STATION","un-documented")));
    m_.insert(std::pair<PositionReferenceCodeMod::PositionReferenceCode,EnumPar<void> >
     (PositionReferenceCodeMod::YOKE,ep((int)PositionReferenceCodeMod::YOKE,"YOKE","un-documented")));
    m_.insert(std::pair<PositionReferenceCodeMod::PositionReferenceCode,EnumPar<void> >
     (PositionReferenceCodeMod::REFLECTOR,ep((int)PositionReferenceCodeMod::REFLECTOR,"REFLECTOR","un-documented")));
    return true;
  }
  static std::map<PositionReferenceCodeMod::PositionReferenceCode,EnumPar<void> > m_;
};
#define _POSITIONREFERENCECODE_HH
#endif
