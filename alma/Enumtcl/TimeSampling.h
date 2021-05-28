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


#if     !defined(_TIMESAMPLING_H)

#include <alma/Enumerations/CTimeSampling.h>
#define _TIMESAMPLING_H
#endif 

#if     !defined(_TIMESAMPLING_HH)

#include <alma/Enumtcl/Enum.hpp>

template<>
 struct enum_set_traits<TimeSamplingMod::TimeSampling> : public enum_set_traiter<TimeSamplingMod::TimeSampling,2,TimeSamplingMod::INTEGRATION> {};

template<>
class enum_map_traits<TimeSamplingMod::TimeSampling,void> : public enum_map_traiter<TimeSamplingMod::TimeSampling,void> {
public:
  static bool   init_;
  static std::string typeName_;
  static std::string enumerationDesc_;
  static std::string order_;
  static std::string xsdBaseType_;
  static bool   init(){
    EnumPar<void> ep;
    m_.insert(std::pair<TimeSamplingMod::TimeSampling,EnumPar<void> >
     (TimeSamplingMod::SUBINTEGRATION,ep((int)TimeSamplingMod::SUBINTEGRATION,"SUBINTEGRATION","un-documented")));
    m_.insert(std::pair<TimeSamplingMod::TimeSampling,EnumPar<void> >
     (TimeSamplingMod::INTEGRATION,ep((int)TimeSamplingMod::INTEGRATION,"INTEGRATION","un-documented")));
    return true;
  }
  static std::map<TimeSamplingMod::TimeSampling,EnumPar<void> > m_;
};
#define _TIMESAMPLING_HH
#endif
