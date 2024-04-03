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


#if     !defined(_ANTENNAMOTIONPATTERN_H)

#include <alma/Enumerations/CAntennaMotionPattern.h>
#define _ANTENNAMOTIONPATTERN_H
#endif 

#if     !defined(_ANTENNAMOTIONPATTERN_HH)

#include <alma/Enumtcl/Enum.hpp>

template<>
 struct enum_set_traits<AntennaMotionPatternMod::AntennaMotionPattern> : public enum_set_traiter<AntennaMotionPatternMod::AntennaMotionPattern,11,AntennaMotionPatternMod::LISSAJOUS> {};

template<>
class enum_map_traits<AntennaMotionPatternMod::AntennaMotionPattern,void> : public enum_map_traiter<AntennaMotionPatternMod::AntennaMotionPattern,void> {
public:
  static bool   init_;
  static std::string typeName_;
  static std::string enumerationDesc_;
  static std::string order_;
  static std::string xsdBaseType_;
  static bool   init(){
    EnumPar<void> ep;
    m_.insert(std::pair<AntennaMotionPatternMod::AntennaMotionPattern,EnumPar<void> >
     (AntennaMotionPatternMod::NONE,ep((int)AntennaMotionPatternMod::NONE,"NONE","un-documented")));
    m_.insert(std::pair<AntennaMotionPatternMod::AntennaMotionPattern,EnumPar<void> >
     (AntennaMotionPatternMod::CROSS_SCAN,ep((int)AntennaMotionPatternMod::CROSS_SCAN,"CROSS_SCAN","un-documented")));
    m_.insert(std::pair<AntennaMotionPatternMod::AntennaMotionPattern,EnumPar<void> >
     (AntennaMotionPatternMod::SPIRAL,ep((int)AntennaMotionPatternMod::SPIRAL,"SPIRAL","un-documented")));
    m_.insert(std::pair<AntennaMotionPatternMod::AntennaMotionPattern,EnumPar<void> >
     (AntennaMotionPatternMod::CIRCLE,ep((int)AntennaMotionPatternMod::CIRCLE,"CIRCLE","un-documented")));
    m_.insert(std::pair<AntennaMotionPatternMod::AntennaMotionPattern,EnumPar<void> >
     (AntennaMotionPatternMod::THREE_POINTS,ep((int)AntennaMotionPatternMod::THREE_POINTS,"THREE_POINTS","un-documented")));
    m_.insert(std::pair<AntennaMotionPatternMod::AntennaMotionPattern,EnumPar<void> >
     (AntennaMotionPatternMod::FOUR_POINTS,ep((int)AntennaMotionPatternMod::FOUR_POINTS,"FOUR_POINTS","un-documented")));
    m_.insert(std::pair<AntennaMotionPatternMod::AntennaMotionPattern,EnumPar<void> >
     (AntennaMotionPatternMod::FIVE_POINTS,ep((int)AntennaMotionPatternMod::FIVE_POINTS,"FIVE_POINTS","un-documented")));
    m_.insert(std::pair<AntennaMotionPatternMod::AntennaMotionPattern,EnumPar<void> >
     (AntennaMotionPatternMod::TEST,ep((int)AntennaMotionPatternMod::TEST,"TEST","un-documented")));
    m_.insert(std::pair<AntennaMotionPatternMod::AntennaMotionPattern,EnumPar<void> >
     (AntennaMotionPatternMod::UNSPECIFIED,ep((int)AntennaMotionPatternMod::UNSPECIFIED,"UNSPECIFIED","un-documented")));
    m_.insert(std::pair<AntennaMotionPatternMod::AntennaMotionPattern,EnumPar<void> >
     (AntennaMotionPatternMod::STAR,ep((int)AntennaMotionPatternMod::STAR,"STAR","un-documented")));
    m_.insert(std::pair<AntennaMotionPatternMod::AntennaMotionPattern,EnumPar<void> >
     (AntennaMotionPatternMod::LISSAJOUS,ep((int)AntennaMotionPatternMod::LISSAJOUS,"LISSAJOUS","un-documented")));
    return true;
  }
  static std::map<AntennaMotionPatternMod::AntennaMotionPattern,EnumPar<void> > m_;
};
#define _ANTENNAMOTIONPATTERN_HH
#endif
