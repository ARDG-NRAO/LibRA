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


#if     !defined(_TIMESCALE_H)

#include <alma/Enumerations/CTimeScale.h>
#define _TIMESCALE_H
#endif 

#if     !defined(_TIMESCALE_HH)

#include <alma/Enumtcl/Enum.hpp>

template<>
 struct enum_set_traits<TimeScaleMod::TimeScale> : public enum_set_traiter<TimeScaleMod::TimeScale,2,TimeScaleMod::TAI> {};

template<>
class enum_map_traits<TimeScaleMod::TimeScale,void> : public enum_map_traiter<TimeScaleMod::TimeScale,void> {
public:
  static bool   init_;
  static std::string typeName_;
  static std::string enumerationDesc_;
  static std::string order_;
  static std::string xsdBaseType_;
  static bool   init(){
    EnumPar<void> ep;
    m_.insert(std::pair<TimeScaleMod::TimeScale,EnumPar<void> >
     (TimeScaleMod::UTC,ep((int)TimeScaleMod::UTC,"UTC","un-documented")));
    m_.insert(std::pair<TimeScaleMod::TimeScale,EnumPar<void> >
     (TimeScaleMod::TAI,ep((int)TimeScaleMod::TAI,"TAI","un-documented")));
    return true;
  }
  static std::map<TimeScaleMod::TimeScale,EnumPar<void> > m_;
};
#define _TIMESCALE_HH
#endif
