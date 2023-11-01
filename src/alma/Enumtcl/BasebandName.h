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


#if     !defined(_BASEBANDNAME_H)

#include <alma/Enumerations/CBasebandName.h>
#define _BASEBANDNAME_H
#endif 

#if     !defined(_BASEBANDNAME_HH)

#include <alma/Enumtcl/Enum.hpp>

template<>
 struct enum_set_traits<BasebandNameMod::BasebandName> : public enum_set_traiter<BasebandNameMod::BasebandName,16,BasebandNameMod::BD_8BIT> {};

template<>
class enum_map_traits<BasebandNameMod::BasebandName,void> : public enum_map_traiter<BasebandNameMod::BasebandName,void> {
public:
  static bool   init_;
  static std::string typeName_;
  static std::string enumerationDesc_;
  static std::string order_;
  static std::string xsdBaseType_;
  static bool   init(){
    EnumPar<void> ep;
    m_.insert(std::pair<BasebandNameMod::BasebandName,EnumPar<void> >
     (BasebandNameMod::NOBB,ep((int)BasebandNameMod::NOBB,"NOBB","un-documented")));
    m_.insert(std::pair<BasebandNameMod::BasebandName,EnumPar<void> >
     (BasebandNameMod::BB_1,ep((int)BasebandNameMod::BB_1,"BB_1","un-documented")));
    m_.insert(std::pair<BasebandNameMod::BasebandName,EnumPar<void> >
     (BasebandNameMod::BB_2,ep((int)BasebandNameMod::BB_2,"BB_2","un-documented")));
    m_.insert(std::pair<BasebandNameMod::BasebandName,EnumPar<void> >
     (BasebandNameMod::BB_3,ep((int)BasebandNameMod::BB_3,"BB_3","un-documented")));
    m_.insert(std::pair<BasebandNameMod::BasebandName,EnumPar<void> >
     (BasebandNameMod::BB_4,ep((int)BasebandNameMod::BB_4,"BB_4","un-documented")));
    m_.insert(std::pair<BasebandNameMod::BasebandName,EnumPar<void> >
     (BasebandNameMod::BB_5,ep((int)BasebandNameMod::BB_5,"BB_5","un-documented")));
    m_.insert(std::pair<BasebandNameMod::BasebandName,EnumPar<void> >
     (BasebandNameMod::BB_6,ep((int)BasebandNameMod::BB_6,"BB_6","un-documented")));
    m_.insert(std::pair<BasebandNameMod::BasebandName,EnumPar<void> >
     (BasebandNameMod::BB_7,ep((int)BasebandNameMod::BB_7,"BB_7","un-documented")));
    m_.insert(std::pair<BasebandNameMod::BasebandName,EnumPar<void> >
     (BasebandNameMod::BB_8,ep((int)BasebandNameMod::BB_8,"BB_8","un-documented")));
    m_.insert(std::pair<BasebandNameMod::BasebandName,EnumPar<void> >
     (BasebandNameMod::BB_ALL,ep((int)BasebandNameMod::BB_ALL,"BB_ALL","un-documented")));
    m_.insert(std::pair<BasebandNameMod::BasebandName,EnumPar<void> >
     (BasebandNameMod::A1C1_3BIT,ep((int)BasebandNameMod::A1C1_3BIT,"A1C1_3BIT","un-documented")));
    m_.insert(std::pair<BasebandNameMod::BasebandName,EnumPar<void> >
     (BasebandNameMod::A2C2_3BIT,ep((int)BasebandNameMod::A2C2_3BIT,"A2C2_3BIT","un-documented")));
    m_.insert(std::pair<BasebandNameMod::BasebandName,EnumPar<void> >
     (BasebandNameMod::AC_8BIT,ep((int)BasebandNameMod::AC_8BIT,"AC_8BIT","un-documented")));
    m_.insert(std::pair<BasebandNameMod::BasebandName,EnumPar<void> >
     (BasebandNameMod::B1D1_3BIT,ep((int)BasebandNameMod::B1D1_3BIT,"B1D1_3BIT","un-documented")));
    m_.insert(std::pair<BasebandNameMod::BasebandName,EnumPar<void> >
     (BasebandNameMod::B2D2_3BIT,ep((int)BasebandNameMod::B2D2_3BIT,"B2D2_3BIT","un-documented")));
    m_.insert(std::pair<BasebandNameMod::BasebandName,EnumPar<void> >
     (BasebandNameMod::BD_8BIT,ep((int)BasebandNameMod::BD_8BIT,"BD_8BIT","un-documented")));
    return true;
  }
  static std::map<BasebandNameMod::BasebandName,EnumPar<void> > m_;
};
#define _BASEBANDNAME_HH
#endif
