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


#if     !defined(_CALIBRATIONFUNCTION_H)

#include <alma/Enumerations/CCalibrationFunction.h>
#define _CALIBRATIONFUNCTION_H
#endif 

#if     !defined(_CALIBRATIONFUNCTION_HH)

#include <alma/Enumtcl/Enum.hpp>

template<>
 struct enum_set_traits<CalibrationFunctionMod::CalibrationFunction> : public enum_set_traiter<CalibrationFunctionMod::CalibrationFunction,3,CalibrationFunctionMod::UNSPECIFIED> {};

template<>
class enum_map_traits<CalibrationFunctionMod::CalibrationFunction,void> : public enum_map_traiter<CalibrationFunctionMod::CalibrationFunction,void> {
public:
  static bool   init_;
  static std::string typeName_;
  static std::string enumerationDesc_;
  static std::string order_;
  static std::string xsdBaseType_;
  static bool   init(){
    EnumPar<void> ep;
    m_.insert(std::pair<CalibrationFunctionMod::CalibrationFunction,EnumPar<void> >
     (CalibrationFunctionMod::FIRST,ep((int)CalibrationFunctionMod::FIRST,"FIRST","un-documented")));
    m_.insert(std::pair<CalibrationFunctionMod::CalibrationFunction,EnumPar<void> >
     (CalibrationFunctionMod::LAST,ep((int)CalibrationFunctionMod::LAST,"LAST","un-documented")));
    m_.insert(std::pair<CalibrationFunctionMod::CalibrationFunction,EnumPar<void> >
     (CalibrationFunctionMod::UNSPECIFIED,ep((int)CalibrationFunctionMod::UNSPECIFIED,"UNSPECIFIED","un-documented")));
    return true;
  }
  static std::map<CalibrationFunctionMod::CalibrationFunction,EnumPar<void> > m_;
};
#define _CALIBRATIONFUNCTION_HH
#endif
