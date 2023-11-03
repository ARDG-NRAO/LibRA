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


#if     !defined(_SOURCEMODEL_H)

#include <alma/Enumerations/CSourceModel.h>
#define _SOURCEMODEL_H
#endif 

#if     !defined(_SOURCEMODEL_HH)

#include <alma/Enumtcl/Enum.hpp>

template<>
 struct enum_set_traits<SourceModelMod::SourceModel> : public enum_set_traiter<SourceModelMod::SourceModel,3,SourceModelMod::DISK> {};

template<>
class enum_map_traits<SourceModelMod::SourceModel,void> : public enum_map_traiter<SourceModelMod::SourceModel,void> {
public:
  static bool   init_;
  static std::string typeName_;
  static std::string enumerationDesc_;
  static std::string order_;
  static std::string xsdBaseType_;
  static bool   init(){
    EnumPar<void> ep;
    m_.insert(std::pair<SourceModelMod::SourceModel,EnumPar<void> >
     (SourceModelMod::GAUSSIAN,ep((int)SourceModelMod::GAUSSIAN,"GAUSSIAN","un-documented")));
    m_.insert(std::pair<SourceModelMod::SourceModel,EnumPar<void> >
     (SourceModelMod::POINT,ep((int)SourceModelMod::POINT,"POINT","un-documented")));
    m_.insert(std::pair<SourceModelMod::SourceModel,EnumPar<void> >
     (SourceModelMod::DISK,ep((int)SourceModelMod::DISK,"DISK","un-documented")));
    return true;
  }
  static std::map<SourceModelMod::SourceModel,EnumPar<void> > m_;
};
#define _SOURCEMODEL_HH
#endif
