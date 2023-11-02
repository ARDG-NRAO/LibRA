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


#if     !defined(_DETECTORBANDTYPE_H)

#include <alma/Enumerations/CDetectorBandType.h>
#define _DETECTORBANDTYPE_H
#endif 

#if     !defined(_DETECTORBANDTYPE_HH)

#include <alma/Enumtcl/Enum.hpp>

template<>
 struct enum_set_traits<DetectorBandTypeMod::DetectorBandType> : public enum_set_traiter<DetectorBandTypeMod::DetectorBandType,4,DetectorBandTypeMod::SUBBAND> {};

template<>
class enum_map_traits<DetectorBandTypeMod::DetectorBandType,void> : public enum_map_traiter<DetectorBandTypeMod::DetectorBandType,void> {
public:
  static bool   init_;
  static std::string typeName_;
  static std::string enumerationDesc_;
  static std::string order_;
  static std::string xsdBaseType_;
  static bool   init(){
    EnumPar<void> ep;
    m_.insert(std::pair<DetectorBandTypeMod::DetectorBandType,EnumPar<void> >
     (DetectorBandTypeMod::BASEBAND,ep((int)DetectorBandTypeMod::BASEBAND,"BASEBAND","un-documented")));
    m_.insert(std::pair<DetectorBandTypeMod::DetectorBandType,EnumPar<void> >
     (DetectorBandTypeMod::DOWN_CONVERTER,ep((int)DetectorBandTypeMod::DOWN_CONVERTER,"DOWN_CONVERTER","un-documented")));
    m_.insert(std::pair<DetectorBandTypeMod::DetectorBandType,EnumPar<void> >
     (DetectorBandTypeMod::HOLOGRAPHY_RECEIVER,ep((int)DetectorBandTypeMod::HOLOGRAPHY_RECEIVER,"HOLOGRAPHY_RECEIVER","un-documented")));
    m_.insert(std::pair<DetectorBandTypeMod::DetectorBandType,EnumPar<void> >
     (DetectorBandTypeMod::SUBBAND,ep((int)DetectorBandTypeMod::SUBBAND,"SUBBAND","un-documented")));
    return true;
  }
  static std::map<DetectorBandTypeMod::DetectorBandType,EnumPar<void> > m_;
};
#define _DETECTORBANDTYPE_HH
#endif
