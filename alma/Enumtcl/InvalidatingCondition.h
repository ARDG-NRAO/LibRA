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


#if     !defined(_INVALIDATINGCONDITION_H)

#include <alma/Enumerations/CInvalidatingCondition.h>
#define _INVALIDATINGCONDITION_H
#endif 

#if     !defined(_INVALIDATINGCONDITION_HH)

#include <alma/Enumtcl/Enum.hpp>

template<>
 struct enum_set_traits<InvalidatingConditionMod::InvalidatingCondition> : public enum_set_traiter<InvalidatingConditionMod::InvalidatingCondition,5,InvalidatingConditionMod::RECEIVER_POWER_DOWN> {};

template<>
class enum_map_traits<InvalidatingConditionMod::InvalidatingCondition,void> : public enum_map_traiter<InvalidatingConditionMod::InvalidatingCondition,void> {
public:
  static bool   init_;
  static std::string typeName_;
  static std::string enumerationDesc_;
  static std::string order_;
  static std::string xsdBaseType_;
  static bool   init(){
    EnumPar<void> ep;
    m_.insert(std::pair<InvalidatingConditionMod::InvalidatingCondition,EnumPar<void> >
     (InvalidatingConditionMod::ANTENNA_DISCONNECT,ep((int)InvalidatingConditionMod::ANTENNA_DISCONNECT,"ANTENNA_DISCONNECT","un-documented")));
    m_.insert(std::pair<InvalidatingConditionMod::InvalidatingCondition,EnumPar<void> >
     (InvalidatingConditionMod::ANTENNA_MOVE,ep((int)InvalidatingConditionMod::ANTENNA_MOVE,"ANTENNA_MOVE","un-documented")));
    m_.insert(std::pair<InvalidatingConditionMod::InvalidatingCondition,EnumPar<void> >
     (InvalidatingConditionMod::ANTENNA_POWER_DOWN,ep((int)InvalidatingConditionMod::ANTENNA_POWER_DOWN,"ANTENNA_POWER_DOWN","un-documented")));
    m_.insert(std::pair<InvalidatingConditionMod::InvalidatingCondition,EnumPar<void> >
     (InvalidatingConditionMod::RECEIVER_EXCHANGE,ep((int)InvalidatingConditionMod::RECEIVER_EXCHANGE,"RECEIVER_EXCHANGE","un-documented")));
    m_.insert(std::pair<InvalidatingConditionMod::InvalidatingCondition,EnumPar<void> >
     (InvalidatingConditionMod::RECEIVER_POWER_DOWN,ep((int)InvalidatingConditionMod::RECEIVER_POWER_DOWN,"RECEIVER_POWER_DOWN","un-documented")));
    return true;
  }
  static std::map<InvalidatingConditionMod::InvalidatingCondition,EnumPar<void> > m_;
};
#define _INVALIDATINGCONDITION_HH
#endif
