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


#if     !defined(_PROCESSORSUBTYPE_H)

#include <alma/Enumerations/CProcessorSubType.h>
#define _PROCESSORSUBTYPE_H
#endif 

#if     !defined(_PROCESSORSUBTYPE_HH)

#include <alma/Enumtcl/Enum.hpp>

template<>
 struct enum_set_traits<ProcessorSubTypeMod::ProcessorSubType> : public enum_set_traiter<ProcessorSubTypeMod::ProcessorSubType,4,ProcessorSubTypeMod::ALMA_RADIOMETER> {};

template<>
class enum_map_traits<ProcessorSubTypeMod::ProcessorSubType,void> : public enum_map_traiter<ProcessorSubTypeMod::ProcessorSubType,void> {
public:
  static bool   init_;
  static std::string typeName_;
  static std::string enumerationDesc_;
  static std::string order_;
  static std::string xsdBaseType_;
  static bool   init(){
    EnumPar<void> ep;
    m_.insert(std::pair<ProcessorSubTypeMod::ProcessorSubType,EnumPar<void> >
     (ProcessorSubTypeMod::ALMA_CORRELATOR_MODE,ep((int)ProcessorSubTypeMod::ALMA_CORRELATOR_MODE,"ALMA_CORRELATOR_MODE","un-documented")));
    m_.insert(std::pair<ProcessorSubTypeMod::ProcessorSubType,EnumPar<void> >
     (ProcessorSubTypeMod::SQUARE_LAW_DETECTOR,ep((int)ProcessorSubTypeMod::SQUARE_LAW_DETECTOR,"SQUARE_LAW_DETECTOR","un-documented")));
    m_.insert(std::pair<ProcessorSubTypeMod::ProcessorSubType,EnumPar<void> >
     (ProcessorSubTypeMod::HOLOGRAPHY,ep((int)ProcessorSubTypeMod::HOLOGRAPHY,"HOLOGRAPHY","un-documented")));
    m_.insert(std::pair<ProcessorSubTypeMod::ProcessorSubType,EnumPar<void> >
     (ProcessorSubTypeMod::ALMA_RADIOMETER,ep((int)ProcessorSubTypeMod::ALMA_RADIOMETER,"ALMA_RADIOMETER","un-documented")));
    return true;
  }
  static std::map<ProcessorSubTypeMod::ProcessorSubType,EnumPar<void> > m_;
};
#define _PROCESSORSUBTYPE_HH
#endif
