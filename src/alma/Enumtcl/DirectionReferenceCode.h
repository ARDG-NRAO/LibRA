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


#if     !defined(_DIRECTIONREFERENCECODE_H)

#include <alma/Enumerations/CDirectionReferenceCode.h>
#define _DIRECTIONREFERENCECODE_H
#endif 

#if     !defined(_DIRECTIONREFERENCECODE_HH)

#include <alma/Enumtcl/Enum.hpp>

template<>
 struct enum_set_traits<DirectionReferenceCodeMod::DirectionReferenceCode> : public enum_set_traiter<DirectionReferenceCodeMod::DirectionReferenceCode,32,DirectionReferenceCodeMod::MOON> {};

template<>
class enum_map_traits<DirectionReferenceCodeMod::DirectionReferenceCode,void> : public enum_map_traiter<DirectionReferenceCodeMod::DirectionReferenceCode,void> {
public:
  static bool   init_;
  static std::string typeName_;
  static std::string enumerationDesc_;
  static std::string order_;
  static std::string xsdBaseType_;
  static bool   init(){
    EnumPar<void> ep;
    m_.insert(std::pair<DirectionReferenceCodeMod::DirectionReferenceCode,EnumPar<void> >
     (DirectionReferenceCodeMod::J2000,ep((int)DirectionReferenceCodeMod::J2000,"J2000","un-documented")));
    m_.insert(std::pair<DirectionReferenceCodeMod::DirectionReferenceCode,EnumPar<void> >
     (DirectionReferenceCodeMod::JMEAN,ep((int)DirectionReferenceCodeMod::JMEAN,"JMEAN","un-documented")));
    m_.insert(std::pair<DirectionReferenceCodeMod::DirectionReferenceCode,EnumPar<void> >
     (DirectionReferenceCodeMod::JTRUE,ep((int)DirectionReferenceCodeMod::JTRUE,"JTRUE","un-documented")));
    m_.insert(std::pair<DirectionReferenceCodeMod::DirectionReferenceCode,EnumPar<void> >
     (DirectionReferenceCodeMod::APP,ep((int)DirectionReferenceCodeMod::APP,"APP","un-documented")));
    m_.insert(std::pair<DirectionReferenceCodeMod::DirectionReferenceCode,EnumPar<void> >
     (DirectionReferenceCodeMod::B1950,ep((int)DirectionReferenceCodeMod::B1950,"B1950","un-documented")));
    m_.insert(std::pair<DirectionReferenceCodeMod::DirectionReferenceCode,EnumPar<void> >
     (DirectionReferenceCodeMod::B1950_VLA,ep((int)DirectionReferenceCodeMod::B1950_VLA,"B1950_VLA","un-documented")));
    m_.insert(std::pair<DirectionReferenceCodeMod::DirectionReferenceCode,EnumPar<void> >
     (DirectionReferenceCodeMod::BMEAN,ep((int)DirectionReferenceCodeMod::BMEAN,"BMEAN","un-documented")));
    m_.insert(std::pair<DirectionReferenceCodeMod::DirectionReferenceCode,EnumPar<void> >
     (DirectionReferenceCodeMod::BTRUE,ep((int)DirectionReferenceCodeMod::BTRUE,"BTRUE","un-documented")));
    m_.insert(std::pair<DirectionReferenceCodeMod::DirectionReferenceCode,EnumPar<void> >
     (DirectionReferenceCodeMod::GALACTIC,ep((int)DirectionReferenceCodeMod::GALACTIC,"GALACTIC","un-documented")));
    m_.insert(std::pair<DirectionReferenceCodeMod::DirectionReferenceCode,EnumPar<void> >
     (DirectionReferenceCodeMod::HADEC,ep((int)DirectionReferenceCodeMod::HADEC,"HADEC","un-documented")));
    m_.insert(std::pair<DirectionReferenceCodeMod::DirectionReferenceCode,EnumPar<void> >
     (DirectionReferenceCodeMod::AZELSW,ep((int)DirectionReferenceCodeMod::AZELSW,"AZELSW","un-documented")));
    m_.insert(std::pair<DirectionReferenceCodeMod::DirectionReferenceCode,EnumPar<void> >
     (DirectionReferenceCodeMod::AZELSWGEO,ep((int)DirectionReferenceCodeMod::AZELSWGEO,"AZELSWGEO","un-documented")));
    m_.insert(std::pair<DirectionReferenceCodeMod::DirectionReferenceCode,EnumPar<void> >
     (DirectionReferenceCodeMod::AZELNE,ep((int)DirectionReferenceCodeMod::AZELNE,"AZELNE","un-documented")));
    m_.insert(std::pair<DirectionReferenceCodeMod::DirectionReferenceCode,EnumPar<void> >
     (DirectionReferenceCodeMod::AZELNEGEO,ep((int)DirectionReferenceCodeMod::AZELNEGEO,"AZELNEGEO","un-documented")));
    m_.insert(std::pair<DirectionReferenceCodeMod::DirectionReferenceCode,EnumPar<void> >
     (DirectionReferenceCodeMod::JNAT,ep((int)DirectionReferenceCodeMod::JNAT,"JNAT","un-documented")));
    m_.insert(std::pair<DirectionReferenceCodeMod::DirectionReferenceCode,EnumPar<void> >
     (DirectionReferenceCodeMod::ECLIPTIC,ep((int)DirectionReferenceCodeMod::ECLIPTIC,"ECLIPTIC","un-documented")));
    m_.insert(std::pair<DirectionReferenceCodeMod::DirectionReferenceCode,EnumPar<void> >
     (DirectionReferenceCodeMod::MECLIPTIC,ep((int)DirectionReferenceCodeMod::MECLIPTIC,"MECLIPTIC","un-documented")));
    m_.insert(std::pair<DirectionReferenceCodeMod::DirectionReferenceCode,EnumPar<void> >
     (DirectionReferenceCodeMod::TECLIPTIC,ep((int)DirectionReferenceCodeMod::TECLIPTIC,"TECLIPTIC","un-documented")));
    m_.insert(std::pair<DirectionReferenceCodeMod::DirectionReferenceCode,EnumPar<void> >
     (DirectionReferenceCodeMod::SUPERGAL,ep((int)DirectionReferenceCodeMod::SUPERGAL,"SUPERGAL","un-documented")));
    m_.insert(std::pair<DirectionReferenceCodeMod::DirectionReferenceCode,EnumPar<void> >
     (DirectionReferenceCodeMod::ITRF,ep((int)DirectionReferenceCodeMod::ITRF,"ITRF","un-documented")));
    m_.insert(std::pair<DirectionReferenceCodeMod::DirectionReferenceCode,EnumPar<void> >
     (DirectionReferenceCodeMod::TOPO,ep((int)DirectionReferenceCodeMod::TOPO,"TOPO","un-documented")));
    m_.insert(std::pair<DirectionReferenceCodeMod::DirectionReferenceCode,EnumPar<void> >
     (DirectionReferenceCodeMod::ICRS,ep((int)DirectionReferenceCodeMod::ICRS,"ICRS","un-documented")));
    m_.insert(std::pair<DirectionReferenceCodeMod::DirectionReferenceCode,EnumPar<void> >
     (DirectionReferenceCodeMod::MERCURY,ep((int)DirectionReferenceCodeMod::MERCURY,"MERCURY","un-documented")));
    m_.insert(std::pair<DirectionReferenceCodeMod::DirectionReferenceCode,EnumPar<void> >
     (DirectionReferenceCodeMod::VENUS,ep((int)DirectionReferenceCodeMod::VENUS,"VENUS","un-documented")));
    m_.insert(std::pair<DirectionReferenceCodeMod::DirectionReferenceCode,EnumPar<void> >
     (DirectionReferenceCodeMod::MARS,ep((int)DirectionReferenceCodeMod::MARS,"MARS","un-documented")));
    m_.insert(std::pair<DirectionReferenceCodeMod::DirectionReferenceCode,EnumPar<void> >
     (DirectionReferenceCodeMod::JUPITER,ep((int)DirectionReferenceCodeMod::JUPITER,"JUPITER","un-documented")));
    m_.insert(std::pair<DirectionReferenceCodeMod::DirectionReferenceCode,EnumPar<void> >
     (DirectionReferenceCodeMod::SATURN,ep((int)DirectionReferenceCodeMod::SATURN,"SATURN","un-documented")));
    m_.insert(std::pair<DirectionReferenceCodeMod::DirectionReferenceCode,EnumPar<void> >
     (DirectionReferenceCodeMod::URANUS,ep((int)DirectionReferenceCodeMod::URANUS,"URANUS","un-documented")));
    m_.insert(std::pair<DirectionReferenceCodeMod::DirectionReferenceCode,EnumPar<void> >
     (DirectionReferenceCodeMod::NEPTUNE,ep((int)DirectionReferenceCodeMod::NEPTUNE,"NEPTUNE","un-documented")));
    m_.insert(std::pair<DirectionReferenceCodeMod::DirectionReferenceCode,EnumPar<void> >
     (DirectionReferenceCodeMod::PLUTO,ep((int)DirectionReferenceCodeMod::PLUTO,"PLUTO","un-documented")));
    m_.insert(std::pair<DirectionReferenceCodeMod::DirectionReferenceCode,EnumPar<void> >
     (DirectionReferenceCodeMod::SUN,ep((int)DirectionReferenceCodeMod::SUN,"SUN","un-documented")));
    m_.insert(std::pair<DirectionReferenceCodeMod::DirectionReferenceCode,EnumPar<void> >
     (DirectionReferenceCodeMod::MOON,ep((int)DirectionReferenceCodeMod::MOON,"MOON","un-documented")));
    return true;
  }
  static std::map<DirectionReferenceCodeMod::DirectionReferenceCode,EnumPar<void> > m_;
};
#define _DIRECTIONREFERENCECODE_HH
#endif
