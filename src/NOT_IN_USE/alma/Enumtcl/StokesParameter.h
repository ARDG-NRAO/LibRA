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


#if     !defined(_STOKESPARAMETER_H)

#include <alma/Enumerations/CStokesParameter.h>
#define _STOKESPARAMETER_H
#endif 

#if     !defined(_STOKESPARAMETER_HH)

#include <alma/Enumtcl/Enum.hpp>

template<>
 struct enum_set_traits<StokesParameterMod::StokesParameter> : public enum_set_traiter<StokesParameterMod::StokesParameter,32,StokesParameterMod::PANGLE> {};

template<>
class enum_map_traits<StokesParameterMod::StokesParameter,void> : public enum_map_traiter<StokesParameterMod::StokesParameter,void> {
public:
  static bool   init_;
  static std::string typeName_;
  static std::string enumerationDesc_;
  static std::string order_;
  static std::string xsdBaseType_;
  static bool   init(){
    EnumPar<void> ep;
    m_.insert(std::pair<StokesParameterMod::StokesParameter,EnumPar<void> >
     (StokesParameterMod::I,ep((int)StokesParameterMod::I,"I","un-documented")));
    m_.insert(std::pair<StokesParameterMod::StokesParameter,EnumPar<void> >
     (StokesParameterMod::Q,ep((int)StokesParameterMod::Q,"Q","un-documented")));
    m_.insert(std::pair<StokesParameterMod::StokesParameter,EnumPar<void> >
     (StokesParameterMod::U,ep((int)StokesParameterMod::U,"U","un-documented")));
    m_.insert(std::pair<StokesParameterMod::StokesParameter,EnumPar<void> >
     (StokesParameterMod::V,ep((int)StokesParameterMod::V,"V","un-documented")));
    m_.insert(std::pair<StokesParameterMod::StokesParameter,EnumPar<void> >
     (StokesParameterMod::RR,ep((int)StokesParameterMod::RR,"RR","un-documented")));
    m_.insert(std::pair<StokesParameterMod::StokesParameter,EnumPar<void> >
     (StokesParameterMod::RL,ep((int)StokesParameterMod::RL,"RL","un-documented")));
    m_.insert(std::pair<StokesParameterMod::StokesParameter,EnumPar<void> >
     (StokesParameterMod::LR,ep((int)StokesParameterMod::LR,"LR","un-documented")));
    m_.insert(std::pair<StokesParameterMod::StokesParameter,EnumPar<void> >
     (StokesParameterMod::LL,ep((int)StokesParameterMod::LL,"LL","un-documented")));
    m_.insert(std::pair<StokesParameterMod::StokesParameter,EnumPar<void> >
     (StokesParameterMod::XX,ep((int)StokesParameterMod::XX,"XX","un-documented")));
    m_.insert(std::pair<StokesParameterMod::StokesParameter,EnumPar<void> >
     (StokesParameterMod::XY,ep((int)StokesParameterMod::XY,"XY","un-documented")));
    m_.insert(std::pair<StokesParameterMod::StokesParameter,EnumPar<void> >
     (StokesParameterMod::YX,ep((int)StokesParameterMod::YX,"YX","un-documented")));
    m_.insert(std::pair<StokesParameterMod::StokesParameter,EnumPar<void> >
     (StokesParameterMod::YY,ep((int)StokesParameterMod::YY,"YY","un-documented")));
    m_.insert(std::pair<StokesParameterMod::StokesParameter,EnumPar<void> >
     (StokesParameterMod::RX,ep((int)StokesParameterMod::RX,"RX","un-documented")));
    m_.insert(std::pair<StokesParameterMod::StokesParameter,EnumPar<void> >
     (StokesParameterMod::RY,ep((int)StokesParameterMod::RY,"RY","un-documented")));
    m_.insert(std::pair<StokesParameterMod::StokesParameter,EnumPar<void> >
     (StokesParameterMod::LX,ep((int)StokesParameterMod::LX,"LX","un-documented")));
    m_.insert(std::pair<StokesParameterMod::StokesParameter,EnumPar<void> >
     (StokesParameterMod::LY,ep((int)StokesParameterMod::LY,"LY","un-documented")));
    m_.insert(std::pair<StokesParameterMod::StokesParameter,EnumPar<void> >
     (StokesParameterMod::XR,ep((int)StokesParameterMod::XR,"XR","un-documented")));
    m_.insert(std::pair<StokesParameterMod::StokesParameter,EnumPar<void> >
     (StokesParameterMod::XL,ep((int)StokesParameterMod::XL,"XL","un-documented")));
    m_.insert(std::pair<StokesParameterMod::StokesParameter,EnumPar<void> >
     (StokesParameterMod::YR,ep((int)StokesParameterMod::YR,"YR","un-documented")));
    m_.insert(std::pair<StokesParameterMod::StokesParameter,EnumPar<void> >
     (StokesParameterMod::YL,ep((int)StokesParameterMod::YL,"YL","un-documented")));
    m_.insert(std::pair<StokesParameterMod::StokesParameter,EnumPar<void> >
     (StokesParameterMod::PP,ep((int)StokesParameterMod::PP,"PP","un-documented")));
    m_.insert(std::pair<StokesParameterMod::StokesParameter,EnumPar<void> >
     (StokesParameterMod::PQ,ep((int)StokesParameterMod::PQ,"PQ","un-documented")));
    m_.insert(std::pair<StokesParameterMod::StokesParameter,EnumPar<void> >
     (StokesParameterMod::QP,ep((int)StokesParameterMod::QP,"QP","un-documented")));
    m_.insert(std::pair<StokesParameterMod::StokesParameter,EnumPar<void> >
     (StokesParameterMod::QQ,ep((int)StokesParameterMod::QQ,"QQ","un-documented")));
    m_.insert(std::pair<StokesParameterMod::StokesParameter,EnumPar<void> >
     (StokesParameterMod::RCIRCULAR,ep((int)StokesParameterMod::RCIRCULAR,"RCIRCULAR","un-documented")));
    m_.insert(std::pair<StokesParameterMod::StokesParameter,EnumPar<void> >
     (StokesParameterMod::LCIRCULAR,ep((int)StokesParameterMod::LCIRCULAR,"LCIRCULAR","un-documented")));
    m_.insert(std::pair<StokesParameterMod::StokesParameter,EnumPar<void> >
     (StokesParameterMod::LINEAR,ep((int)StokesParameterMod::LINEAR,"LINEAR","un-documented")));
    m_.insert(std::pair<StokesParameterMod::StokesParameter,EnumPar<void> >
     (StokesParameterMod::PTOTAL,ep((int)StokesParameterMod::PTOTAL,"PTOTAL","un-documented")));
    m_.insert(std::pair<StokesParameterMod::StokesParameter,EnumPar<void> >
     (StokesParameterMod::PLINEAR,ep((int)StokesParameterMod::PLINEAR,"PLINEAR","un-documented")));
    m_.insert(std::pair<StokesParameterMod::StokesParameter,EnumPar<void> >
     (StokesParameterMod::PFTOTAL,ep((int)StokesParameterMod::PFTOTAL,"PFTOTAL","un-documented")));
    m_.insert(std::pair<StokesParameterMod::StokesParameter,EnumPar<void> >
     (StokesParameterMod::PFLINEAR,ep((int)StokesParameterMod::PFLINEAR,"PFLINEAR","un-documented")));
    m_.insert(std::pair<StokesParameterMod::StokesParameter,EnumPar<void> >
     (StokesParameterMod::PANGLE,ep((int)StokesParameterMod::PANGLE,"PANGLE","un-documented")));
    return true;
  }
  static std::map<StokesParameterMod::StokesParameter,EnumPar<void> > m_;
};
#define _STOKESPARAMETER_HH
#endif
