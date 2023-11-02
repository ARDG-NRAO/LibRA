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


#if     !defined(_SYNTHPROF_H)

#include <alma/Enumerations/CSynthProf.h>
#define _SYNTHPROF_H
#endif 

#if     !defined(_SYNTHPROF_HH)

#include <alma/Enumtcl/Enum.hpp>

template<>
 struct enum_set_traits<SynthProfMod::SynthProf> : public enum_set_traiter<SynthProfMod::SynthProf,3,SynthProfMod::ACA_CDP> {};

template<>
class enum_map_traits<SynthProfMod::SynthProf,void> : public enum_map_traiter<SynthProfMod::SynthProf,void> {
public:
  static bool   init_;
  static std::string typeName_;
  static std::string enumerationDesc_;
  static std::string order_;
  static std::string xsdBaseType_;
  static bool   init(){
    EnumPar<void> ep;
    m_.insert(std::pair<SynthProfMod::SynthProf,EnumPar<void> >
     (SynthProfMod::NOSYNTH,ep((int)SynthProfMod::NOSYNTH,"NOSYNTH","un-documented")));
    m_.insert(std::pair<SynthProfMod::SynthProf,EnumPar<void> >
     (SynthProfMod::ACACORR,ep((int)SynthProfMod::ACACORR,"ACACORR","un-documented")));
    m_.insert(std::pair<SynthProfMod::SynthProf,EnumPar<void> >
     (SynthProfMod::ACA_CDP,ep((int)SynthProfMod::ACA_CDP,"ACA_CDP","un-documented")));
    return true;
  }
  static std::map<SynthProfMod::SynthProf,EnumPar<void> > m_;
};
#define _SYNTHPROF_HH
#endif
