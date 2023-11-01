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


#if     !defined(_FLUXCALIBRATIONMETHOD_H)

#include <alma/Enumerations/CFluxCalibrationMethod.h>
#define _FLUXCALIBRATIONMETHOD_H
#endif 

#if     !defined(_FLUXCALIBRATIONMETHOD_HH)

#include <alma/Enumtcl/Enum.hpp>

template<>
 struct enum_set_traits<FluxCalibrationMethodMod::FluxCalibrationMethod> : public enum_set_traiter<FluxCalibrationMethodMod::FluxCalibrationMethod,3,FluxCalibrationMethodMod::EFFICIENCY> {};

template<>
class enum_map_traits<FluxCalibrationMethodMod::FluxCalibrationMethod,void> : public enum_map_traiter<FluxCalibrationMethodMod::FluxCalibrationMethod,void> {
public:
  static bool   init_;
  static std::string typeName_;
  static std::string enumerationDesc_;
  static std::string order_;
  static std::string xsdBaseType_;
  static bool   init(){
    EnumPar<void> ep;
    m_.insert(std::pair<FluxCalibrationMethodMod::FluxCalibrationMethod,EnumPar<void> >
     (FluxCalibrationMethodMod::ABSOLUTE,ep((int)FluxCalibrationMethodMod::ABSOLUTE,"ABSOLUTE","un-documented")));
    m_.insert(std::pair<FluxCalibrationMethodMod::FluxCalibrationMethod,EnumPar<void> >
     (FluxCalibrationMethodMod::RELATIVE,ep((int)FluxCalibrationMethodMod::RELATIVE,"RELATIVE","un-documented")));
    m_.insert(std::pair<FluxCalibrationMethodMod::FluxCalibrationMethod,EnumPar<void> >
     (FluxCalibrationMethodMod::EFFICIENCY,ep((int)FluxCalibrationMethodMod::EFFICIENCY,"EFFICIENCY","un-documented")));
    return true;
  }
  static std::map<FluxCalibrationMethodMod::FluxCalibrationMethod,EnumPar<void> > m_;
};
#define _FLUXCALIBRATIONMETHOD_HH
#endif
