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


#if     !defined(_PROCESSORTYPE_H)

#include <alma/Enumerations/CProcessorType.h>
#define _PROCESSORTYPE_H
#endif 

#if     !defined(_PROCESSORTYPE_HH)

#include <alma/Enumtcl/Enum.hpp>

template<>
 struct enum_set_traits<ProcessorTypeMod::ProcessorType> : public enum_set_traiter<ProcessorTypeMod::ProcessorType,3,ProcessorTypeMod::SPECTROMETER> {};

template<>
class enum_map_traits<ProcessorTypeMod::ProcessorType,void> : public enum_map_traiter<ProcessorTypeMod::ProcessorType,void> {
public:
  static bool   init_;
  static std::string typeName_;
  static std::string enumerationDesc_;
  static std::string order_;
  static std::string xsdBaseType_;
  static bool   init(){
    EnumPar<void> ep;
    m_.insert(std::pair<ProcessorTypeMod::ProcessorType,EnumPar<void> >
     (ProcessorTypeMod::CORRELATOR,ep((int)ProcessorTypeMod::CORRELATOR,"CORRELATOR","un-documented")));
    m_.insert(std::pair<ProcessorTypeMod::ProcessorType,EnumPar<void> >
     (ProcessorTypeMod::RADIOMETER,ep((int)ProcessorTypeMod::RADIOMETER,"RADIOMETER","un-documented")));
    m_.insert(std::pair<ProcessorTypeMod::ProcessorType,EnumPar<void> >
     (ProcessorTypeMod::SPECTROMETER,ep((int)ProcessorTypeMod::SPECTROMETER,"SPECTROMETER","un-documented")));
    return true;
  }
  static std::map<ProcessorTypeMod::ProcessorType,EnumPar<void> > m_;
};
#define _PROCESSORTYPE_HH
#endif
