#if     !defined(_ENUMMAP_HPP)

#include <map>
#include <limits>

#include <alma/Enumtcl/enum_par.hpp>

template<typename enum_type, typename val_type>
class enum_map_traits : public std::map<enum_type,EnumPar<val_type> > {
public:
  enum_map_traits(){}
  static bool init_;
  static std::string typeName_;
  static std::string enumerationDesc_;
  static std::string order_;
  static std::string xsdBaseType_;
  static std::map<enum_type,EnumPar<val_type> > m_;
};


template<typename type, typename val_type>
class enum_map_traiter : public std::map<type,EnumPar<val_type> >, std::map<type,EnumPar<val_type> >::iterator
{
public:
  typedef type enum_type;
  static bool init_;
  static std::string typeName_;
  static std::string enumerationDesc_;
  static std::string order_;
  static std::string xsdBaseType_;

  static std::set<std::string> enumMemberSet(){
    std::set<std::string> s;
    typename std::map<enum_type,EnumPar<val_type> >::iterator it, itb(m_.begin()), ite(m_.end());
    for(it=itb; it!=ite; ++it)s.insert(it->second.str());
    return s;
  }

  static std::vector<std::string> enumMemberList(){
    std::vector<std::string> v;
    typename std::map<enum_type,EnumPar<val_type> >::iterator it, itb(m_.begin()), ite(m_.end());
    for(it=itb; it!=ite; ++it)v.push_back(it->second.str());
    return v;
  }

  static std::vector<val_type> enumValues(){
    std::vector<val_type> v;
    typename std::map<enum_type,EnumPar<val_type> >::iterator it, itb(m_.begin()), ite(m_.end());
    for(it=itb; it!=ite; ++it)v.push_back(it->second.val());
    return v;
  }

  static unsigned int enumNum(){
    return m_.size();
  }

  static std::map<type,EnumPar<val_type> > m_;
  // ne doit pas etre static! Je n'arrive plus a trouver la raison...
  int fromStringToInt(std::string s){
    typename 
    std::map<enum_type,EnumPar<val_type> >::iterator it, itb(m_.begin()), ite(m_.end());
    for(it=itb;it!=ite;++it)
      if(it->second.str()==s)return (int) it->first;
    return std::numeric_limits<int>::max();
  }

};

#define _ENUMMAP_HPP
#endif
