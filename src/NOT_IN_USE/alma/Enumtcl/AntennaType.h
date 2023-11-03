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


#if     !defined(_ANTENNATYPE_H)

#include <alma/Enumerations/CAntennaType.h>
#define _ANTENNATYPE_H
#endif 

#if     !defined(_ANTENNATYPE_HH)

#include <alma/Enumtcl/Enum.hpp>

template<>
 struct enum_set_traits<AntennaTypeMod::AntennaType> : public enum_set_traiter<AntennaTypeMod::AntennaType,3,AntennaTypeMod::TRACKING_STN> {};

template<>
class enum_map_traits<AntennaTypeMod::AntennaType,void> : public enum_map_traiter<AntennaTypeMod::AntennaType,void> {
public:
  static bool   init_;
  static std::string typeName_;
  static std::string enumerationDesc_;
  static std::string order_;
  static std::string xsdBaseType_;
  static bool   init(){
    EnumPar<void> ep;
    m_.insert(std::pair<AntennaTypeMod::AntennaType,EnumPar<void> >
     (AntennaTypeMod::GROUND_BASED,ep((int)AntennaTypeMod::GROUND_BASED,"GROUND_BASED","un-documented")));
    m_.insert(std::pair<AntennaTypeMod::AntennaType,EnumPar<void> >
     (AntennaTypeMod::SPACE_BASED,ep((int)AntennaTypeMod::SPACE_BASED,"SPACE_BASED","un-documented")));
    m_.insert(std::pair<AntennaTypeMod::AntennaType,EnumPar<void> >
     (AntennaTypeMod::TRACKING_STN,ep((int)AntennaTypeMod::TRACKING_STN,"TRACKING_STN","un-documented")));
    return true;
  }
  static std::map<AntennaTypeMod::AntennaType,EnumPar<void> > m_;
};
#define _ANTENNATYPE_HH
#endif
