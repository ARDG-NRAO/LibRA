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


#if     !defined(_CALIBRATIONDEVICE_H)

#include <alma/Enumerations/CCalibrationDevice.h>
#define _CALIBRATIONDEVICE_H
#endif 

#if     !defined(_CALIBRATIONDEVICE_HH)

#include <alma/Enumtcl/Enum.hpp>

template<>
 struct enum_set_traits<CalibrationDeviceMod::CalibrationDevice> : public enum_set_traiter<CalibrationDeviceMod::CalibrationDevice,7,CalibrationDeviceMod::NONE> {};

template<>
class enum_map_traits<CalibrationDeviceMod::CalibrationDevice,void> : public enum_map_traiter<CalibrationDeviceMod::CalibrationDevice,void> {
public:
  static bool   init_;
  static std::string typeName_;
  static std::string enumerationDesc_;
  static std::string order_;
  static std::string xsdBaseType_;
  static bool   init(){
    EnumPar<void> ep;
    m_.insert(std::pair<CalibrationDeviceMod::CalibrationDevice,EnumPar<void> >
     (CalibrationDeviceMod::AMBIENT_LOAD,ep((int)CalibrationDeviceMod::AMBIENT_LOAD,"AMBIENT_LOAD","un-documented")));
    m_.insert(std::pair<CalibrationDeviceMod::CalibrationDevice,EnumPar<void> >
     (CalibrationDeviceMod::COLD_LOAD,ep((int)CalibrationDeviceMod::COLD_LOAD,"COLD_LOAD","un-documented")));
    m_.insert(std::pair<CalibrationDeviceMod::CalibrationDevice,EnumPar<void> >
     (CalibrationDeviceMod::HOT_LOAD,ep((int)CalibrationDeviceMod::HOT_LOAD,"HOT_LOAD","un-documented")));
    m_.insert(std::pair<CalibrationDeviceMod::CalibrationDevice,EnumPar<void> >
     (CalibrationDeviceMod::NOISE_TUBE_LOAD,ep((int)CalibrationDeviceMod::NOISE_TUBE_LOAD,"NOISE_TUBE_LOAD","un-documented")));
    m_.insert(std::pair<CalibrationDeviceMod::CalibrationDevice,EnumPar<void> >
     (CalibrationDeviceMod::QUARTER_WAVE_PLATE,ep((int)CalibrationDeviceMod::QUARTER_WAVE_PLATE,"QUARTER_WAVE_PLATE","un-documented")));
    m_.insert(std::pair<CalibrationDeviceMod::CalibrationDevice,EnumPar<void> >
     (CalibrationDeviceMod::SOLAR_FILTER,ep((int)CalibrationDeviceMod::SOLAR_FILTER,"SOLAR_FILTER","un-documented")));
    m_.insert(std::pair<CalibrationDeviceMod::CalibrationDevice,EnumPar<void> >
     (CalibrationDeviceMod::NONE,ep((int)CalibrationDeviceMod::NONE,"NONE","un-documented")));
    return true;
  }
  static std::map<CalibrationDeviceMod::CalibrationDevice,EnumPar<void> > m_;
};
#define _CALIBRATIONDEVICE_HH
#endif
