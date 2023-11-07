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


#if     !defined(_POINTINGMODELMODE_H)

#include <alma/Enumerations/CPointingModelMode.h>
#define _POINTINGMODELMODE_H
#endif 

#if     !defined(_POINTINGMODELMODE_HH)

#include <alma/Enumtcl/Enum.hpp>

template<>
 struct enum_set_traits<PointingModelModeMod::PointingModelMode> : public enum_set_traiter<PointingModelModeMod::PointingModelMode,2,PointingModelModeMod::OPTICAL> {};

template<>
class enum_map_traits<PointingModelModeMod::PointingModelMode,void> : public enum_map_traiter<PointingModelModeMod::PointingModelMode,void> {
public:
  static bool   init_;
  static std::string typeName_;
  static std::string enumerationDesc_;
  static std::string order_;
  static std::string xsdBaseType_;
  static bool   init(){
    EnumPar<void> ep;
    m_.insert(std::pair<PointingModelModeMod::PointingModelMode,EnumPar<void> >
     (PointingModelModeMod::RADIO,ep((int)PointingModelModeMod::RADIO,"RADIO","un-documented")));
    m_.insert(std::pair<PointingModelModeMod::PointingModelMode,EnumPar<void> >
     (PointingModelModeMod::OPTICAL,ep((int)PointingModelModeMod::OPTICAL,"OPTICAL","un-documented")));
    return true;
  }
  static std::map<PointingModelModeMod::PointingModelMode,EnumPar<void> > m_;
};
#define _POINTINGMODELMODE_HH
#endif
