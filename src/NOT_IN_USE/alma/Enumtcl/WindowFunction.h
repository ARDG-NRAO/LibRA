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


#if     !defined(_WINDOWFUNCTION_H)

#include <alma/Enumerations/CWindowFunction.h>
#define _WINDOWFUNCTION_H
#endif 

#if     !defined(_WINDOWFUNCTION_HH)

#include <alma/Enumtcl/Enum.hpp>

template<>
 struct enum_set_traits<WindowFunctionMod::WindowFunction> : public enum_set_traiter<WindowFunctionMod::WindowFunction,7,WindowFunctionMod::WELCH> {};

template<>
class enum_map_traits<WindowFunctionMod::WindowFunction,void> : public enum_map_traiter<WindowFunctionMod::WindowFunction,void> {
public:
  static bool   init_;
  static std::string typeName_;
  static std::string enumerationDesc_;
  static std::string order_;
  static std::string xsdBaseType_;
  static bool   init(){
    EnumPar<void> ep;
    m_.insert(std::pair<WindowFunctionMod::WindowFunction,EnumPar<void> >
     (WindowFunctionMod::UNIFORM,ep((int)WindowFunctionMod::UNIFORM,"UNIFORM","un-documented")));
    m_.insert(std::pair<WindowFunctionMod::WindowFunction,EnumPar<void> >
     (WindowFunctionMod::HANNING,ep((int)WindowFunctionMod::HANNING,"HANNING","un-documented")));
    m_.insert(std::pair<WindowFunctionMod::WindowFunction,EnumPar<void> >
     (WindowFunctionMod::HAMMING,ep((int)WindowFunctionMod::HAMMING,"HAMMING","un-documented")));
    m_.insert(std::pair<WindowFunctionMod::WindowFunction,EnumPar<void> >
     (WindowFunctionMod::BARTLETT,ep((int)WindowFunctionMod::BARTLETT,"BARTLETT","un-documented")));
    m_.insert(std::pair<WindowFunctionMod::WindowFunction,EnumPar<void> >
     (WindowFunctionMod::BLACKMANN,ep((int)WindowFunctionMod::BLACKMANN,"BLACKMANN","un-documented")));
    m_.insert(std::pair<WindowFunctionMod::WindowFunction,EnumPar<void> >
     (WindowFunctionMod::BLACKMANN_HARRIS,ep((int)WindowFunctionMod::BLACKMANN_HARRIS,"BLACKMANN_HARRIS","un-documented")));
    m_.insert(std::pair<WindowFunctionMod::WindowFunction,EnumPar<void> >
     (WindowFunctionMod::WELCH,ep((int)WindowFunctionMod::WELCH,"WELCH","un-documented")));
    return true;
  }
  static std::map<WindowFunctionMod::WindowFunction,EnumPar<void> > m_;
};
#define _WINDOWFUNCTION_HH
#endif
