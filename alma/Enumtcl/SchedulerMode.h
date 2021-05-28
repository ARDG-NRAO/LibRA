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


#if     !defined(_SCHEDULERMODE_H)

#include <alma/Enumerations/CSchedulerMode.h>
#define _SCHEDULERMODE_H
#endif 

#if     !defined(_SCHEDULERMODE_HH)

#include <alma/Enumtcl/Enum.hpp>

template<>
 struct enum_set_traits<SchedulerModeMod::SchedulerMode> : public enum_set_traiter<SchedulerModeMod::SchedulerMode,4,SchedulerModeMod::QUEUED> {};

template<>
class enum_map_traits<SchedulerModeMod::SchedulerMode,void> : public enum_map_traiter<SchedulerModeMod::SchedulerMode,void> {
public:
  static bool   init_;
  static std::string typeName_;
  static std::string enumerationDesc_;
  static std::string order_;
  static std::string xsdBaseType_;
  static bool   init(){
    EnumPar<void> ep;
    m_.insert(std::pair<SchedulerModeMod::SchedulerMode,EnumPar<void> >
     (SchedulerModeMod::DYNAMIC,ep((int)SchedulerModeMod::DYNAMIC,"DYNAMIC","un-documented")));
    m_.insert(std::pair<SchedulerModeMod::SchedulerMode,EnumPar<void> >
     (SchedulerModeMod::INTERACTIVE,ep((int)SchedulerModeMod::INTERACTIVE,"INTERACTIVE","un-documented")));
    m_.insert(std::pair<SchedulerModeMod::SchedulerMode,EnumPar<void> >
     (SchedulerModeMod::MANUAL,ep((int)SchedulerModeMod::MANUAL,"MANUAL","un-documented")));
    m_.insert(std::pair<SchedulerModeMod::SchedulerMode,EnumPar<void> >
     (SchedulerModeMod::QUEUED,ep((int)SchedulerModeMod::QUEUED,"QUEUED","un-documented")));
    return true;
  }
  static std::map<SchedulerModeMod::SchedulerMode,EnumPar<void> > m_;
};
#define _SCHEDULERMODE_HH
#endif
