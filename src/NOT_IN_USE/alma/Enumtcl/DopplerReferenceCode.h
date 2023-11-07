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


#if     !defined(_DOPPLERREFERENCECODE_H)

#include <alma/Enumerations/CDopplerReferenceCode.h>
#define _DOPPLERREFERENCECODE_H
#endif 

#if     !defined(_DOPPLERREFERENCECODE_HH)

#include <alma/Enumtcl/Enum.hpp>

template<>
 struct enum_set_traits<DopplerReferenceCodeMod::DopplerReferenceCode> : public enum_set_traiter<DopplerReferenceCodeMod::DopplerReferenceCode,7,DopplerReferenceCodeMod::RELATIVISTIC> {};

template<>
class enum_map_traits<DopplerReferenceCodeMod::DopplerReferenceCode,void> : public enum_map_traiter<DopplerReferenceCodeMod::DopplerReferenceCode,void> {
public:
  static bool   init_;
  static std::string typeName_;
  static std::string enumerationDesc_;
  static std::string order_;
  static std::string xsdBaseType_;
  static bool   init(){
    EnumPar<void> ep;
    m_.insert(std::pair<DopplerReferenceCodeMod::DopplerReferenceCode,EnumPar<void> >
     (DopplerReferenceCodeMod::RADIO,ep((int)DopplerReferenceCodeMod::RADIO,"RADIO","un-documented")));
    m_.insert(std::pair<DopplerReferenceCodeMod::DopplerReferenceCode,EnumPar<void> >
     (DopplerReferenceCodeMod::Z,ep((int)DopplerReferenceCodeMod::Z,"Z","un-documented")));
    m_.insert(std::pair<DopplerReferenceCodeMod::DopplerReferenceCode,EnumPar<void> >
     (DopplerReferenceCodeMod::RATIO,ep((int)DopplerReferenceCodeMod::RATIO,"RATIO","un-documented")));
    m_.insert(std::pair<DopplerReferenceCodeMod::DopplerReferenceCode,EnumPar<void> >
     (DopplerReferenceCodeMod::BETA,ep((int)DopplerReferenceCodeMod::BETA,"BETA","un-documented")));
    m_.insert(std::pair<DopplerReferenceCodeMod::DopplerReferenceCode,EnumPar<void> >
     (DopplerReferenceCodeMod::GAMMA,ep((int)DopplerReferenceCodeMod::GAMMA,"GAMMA","un-documented")));
    m_.insert(std::pair<DopplerReferenceCodeMod::DopplerReferenceCode,EnumPar<void> >
     (DopplerReferenceCodeMod::OPTICAL,ep((int)DopplerReferenceCodeMod::OPTICAL,"OPTICAL","un-documented")));
    m_.insert(std::pair<DopplerReferenceCodeMod::DopplerReferenceCode,EnumPar<void> >
     (DopplerReferenceCodeMod::RELATIVISTIC,ep((int)DopplerReferenceCodeMod::RELATIVISTIC,"RELATIVISTIC","un-documented")));
    return true;
  }
  static std::map<DopplerReferenceCodeMod::DopplerReferenceCode,EnumPar<void> > m_;
};
#define _DOPPLERREFERENCECODE_HH
#endif
