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


#if     !defined(_PRIMARYBEAMDESCRIPTION_H)

#include <alma/Enumerations/CPrimaryBeamDescription.h>
#define _PRIMARYBEAMDESCRIPTION_H
#endif 

#if     !defined(_PRIMARYBEAMDESCRIPTION_HH)

#include <alma/Enumtcl/Enum.hpp>

template<>
 struct enum_set_traits<PrimaryBeamDescriptionMod::PrimaryBeamDescription> : public enum_set_traiter<PrimaryBeamDescriptionMod::PrimaryBeamDescription,2,PrimaryBeamDescriptionMod::APERTURE_FIELD_DISTRIBUTION> {};

template<>
class enum_map_traits<PrimaryBeamDescriptionMod::PrimaryBeamDescription,void> : public enum_map_traiter<PrimaryBeamDescriptionMod::PrimaryBeamDescription,void> {
public:
  static bool   init_;
  static std::string typeName_;
  static std::string enumerationDesc_;
  static std::string order_;
  static std::string xsdBaseType_;
  static bool   init(){
    EnumPar<void> ep;
    m_.insert(std::pair<PrimaryBeamDescriptionMod::PrimaryBeamDescription,EnumPar<void> >
     (PrimaryBeamDescriptionMod::COMPLEX_FIELD_PATTERN,ep((int)PrimaryBeamDescriptionMod::COMPLEX_FIELD_PATTERN,"COMPLEX_FIELD_PATTERN","un-documented")));
    m_.insert(std::pair<PrimaryBeamDescriptionMod::PrimaryBeamDescription,EnumPar<void> >
     (PrimaryBeamDescriptionMod::APERTURE_FIELD_DISTRIBUTION,ep((int)PrimaryBeamDescriptionMod::APERTURE_FIELD_DISTRIBUTION,"APERTURE_FIELD_DISTRIBUTION","un-documented")));
    return true;
  }
  static std::map<PrimaryBeamDescriptionMod::PrimaryBeamDescription,EnumPar<void> > m_;
};
#define _PRIMARYBEAMDESCRIPTION_HH
#endif
