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


#if     !defined(_STATIONTYPE_H)

#include <alma/Enumerations/CStationType.h>
#define _STATIONTYPE_H
#endif 

#if     !defined(_STATIONTYPE_HH)

#include <alma/Enumtcl/Enum.hpp>

template<>
 struct enum_set_traits<StationTypeMod::StationType> : public enum_set_traiter<StationTypeMod::StationType,3,StationTypeMod::WEATHER_STATION> {};

template<>
class enum_map_traits<StationTypeMod::StationType,void> : public enum_map_traiter<StationTypeMod::StationType,void> {
public:
  static bool   init_;
  static std::string typeName_;
  static std::string enumerationDesc_;
  static std::string order_;
  static std::string xsdBaseType_;
  static bool   init(){
    EnumPar<void> ep;
    m_.insert(std::pair<StationTypeMod::StationType,EnumPar<void> >
     (StationTypeMod::ANTENNA_PAD,ep((int)StationTypeMod::ANTENNA_PAD,"ANTENNA_PAD","un-documented")));
    m_.insert(std::pair<StationTypeMod::StationType,EnumPar<void> >
     (StationTypeMod::MAINTENANCE_PAD,ep((int)StationTypeMod::MAINTENANCE_PAD,"MAINTENANCE_PAD","un-documented")));
    m_.insert(std::pair<StationTypeMod::StationType,EnumPar<void> >
     (StationTypeMod::WEATHER_STATION,ep((int)StationTypeMod::WEATHER_STATION,"WEATHER_STATION","un-documented")));
    return true;
  }
  static std::map<StationTypeMod::StationType,EnumPar<void> > m_;
};
#define _STATIONTYPE_HH
#endif
