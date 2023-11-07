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


#if     !defined(_CALCURVETYPE_H)

#include <alma/Enumerations/CCalCurveType.h>
#define _CALCURVETYPE_H
#endif 

#if     !defined(_CALCURVETYPE_HH)

#include <alma/Enumtcl/Enum.hpp>

template<>
 struct enum_set_traits<CalCurveTypeMod::CalCurveType> : public enum_set_traiter<CalCurveTypeMod::CalCurveType,3,CalCurveTypeMod::UNDEFINED> {};

template<>
class enum_map_traits<CalCurveTypeMod::CalCurveType,void> : public enum_map_traiter<CalCurveTypeMod::CalCurveType,void> {
public:
  static bool   init_;
  static std::string typeName_;
  static std::string enumerationDesc_;
  static std::string order_;
  static std::string xsdBaseType_;
  static bool   init(){
    EnumPar<void> ep;
    m_.insert(std::pair<CalCurveTypeMod::CalCurveType,EnumPar<void> >
     (CalCurveTypeMod::AMPLITUDE,ep((int)CalCurveTypeMod::AMPLITUDE,"AMPLITUDE","un-documented")));
    m_.insert(std::pair<CalCurveTypeMod::CalCurveType,EnumPar<void> >
     (CalCurveTypeMod::PHASE,ep((int)CalCurveTypeMod::PHASE,"PHASE","un-documented")));
    m_.insert(std::pair<CalCurveTypeMod::CalCurveType,EnumPar<void> >
     (CalCurveTypeMod::UNDEFINED,ep((int)CalCurveTypeMod::UNDEFINED,"UNDEFINED","un-documented")));
    return true;
  }
  static std::map<CalCurveTypeMod::CalCurveType,EnumPar<void> > m_;
};
#define _CALCURVETYPE_HH
#endif
