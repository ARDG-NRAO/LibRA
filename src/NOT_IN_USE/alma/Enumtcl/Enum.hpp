#if     !defined(_ENUM_H)

#include <vector>
#include <map>
#include <set>
#include <iostream>
#include <sstream>
#include <limits>

#include <alma/Enumtcl/enum_set.hpp>
#include <alma/Enumtcl/enum_map.hpp>

template< typename enum_type, 
	  typename val_type,
	  typename set_traits=enum_set_traits<enum_type>,
	  typename map_traits=enum_map_traits<enum_type,val_type> >
class EnumSetVal
{
public:
  EnumSetVal()
  {
    reset();
  }
  EnumSetVal(enum_type setting)
  {
    setOnly(setting);
  }
  EnumSetVal &operator&=(const EnumSetVal &rhs)
  {
    bits &= rhs.bits;
    return *this;
  }
  EnumSetVal &operator|=(const EnumSetVal &rhs)
  {
    bits |= rhs.bits;
    return *this;
  }
  bool less (const EnumSetVal &rhs){
    if(bits.to_ulong()<rhs.bits.to_ulong())return true;
    return false;
  }
  bool operator!= (const EnumSetVal &rhs)
  {
    if(bits.to_ulong()==rhs.bits.to_ulong())return false;
    return true;
  }
  bool operator== (const EnumSetVal &rhs)
  {
    if(bits.to_ulong()==rhs.bits.to_ulong())return true;
    return false;
  }
  EnumSetVal &operator^=(const EnumSetVal &rhs)
  {
    bits ^= rhs.bits;
    return *this;
  }
  std::size_t count() const
  {
    return bits.count();
  }
  std::size_t size() const
  {
    return bits.size();
  }
  bool operator[](enum_type testing) const
  {
    return bits.test(to_bit(testing));
  }
  bool only(enum_type testing) const
  {
    if(count()!=1)return false;
    return bits.test(to_bit(testing));
  }
  bool operator^(enum_type testing) const   // xor operator (^ Python bitwise operator) 
  {
    if(only(testing))return true;
    if(bits.test(to_bit(testing)))return false;
    return true;
  }

  EnumSetVal &set()
  {
    bits.set();
    return *this;
  }
  EnumSetVal &setOnly(enum_type setting)
  {
    bits.reset();
    bits.set(to_bit(setting), true);
    return *this;
  }
  EnumSetVal &set(enum_type setting, bool value = true)
  {
    if(set_traits::maxset<set_traits::count)bits.reset();
    bits.set(to_bit(setting), value);
    return *this;
  }
  EnumSetVal &set(std::vector<enum_type> setting, bool value = true)
  {
    if(set_traits::maxset<set_traits::count)bits.reset();
    unsigned int nmax=setting.size();
    if(nmax>set_traits::maxset)nmax=set_traits::maxset;
  for(unsigned int n=0; n<nmax; n++)
    bits.set(to_bit(setting[n]), value);
    return *this;
  }
  EnumSetVal &set(std::vector<std::string> names, bool /* value = true */)
  {
    // value is not used here, commented out above to silence a compiler warning
    bits.reset();
    typename std::map<enum_type,EnumPar<val_type> >::iterator 
      it, itb(map_traits::m_.begin()), ite(map_traits::m_.end());
    std::vector<std::string>::iterator iv, ivb=names.begin(), ive=names.end();
    for(iv=ivb; iv!=ive; ++iv)
      for(it=itb; it!=ite; ++it)if(it->second.str()==*iv)
	bits.set(to_bit(it->first));
    return *this;
  }


  EnumSetVal &reset()
  {
    bits.reset();
    return *this;
  }
  EnumSetVal &reset(enum_type resetting)
  {
    bits.reset(to_bit(resetting));
    return *this;
  }
  EnumSetVal &flip()
  {
    bits.flip();
    return *this;
  }
  EnumSetVal &flip(enum_type flipping)
  {
    bits.flip(to_bit(flipping));
    return *this;
  }
  EnumSetVal operator~() const
  {
    return EnumSetVal(*this).flip();
  }
  bool any() const
  {
    return bits.any();
  }
  bool none() const
  {
    return bits.none();
  }
  static std::vector<enum_type> enumEnumerators()
  {
    typename std::vector<enum_type> e;
    typename std::map<enum_type,EnumPar<val_type> >::iterator it, itb(map_traits::m_.begin()), ite(map_traits::m_.end());
    for(it=itb; it!=ite; ++it)e.push_back(it->first);
    return e;
  }
  static std::set<std::string> enumMemberSet()
  {
    std::set<std::string> s;
    typename std::map<enum_type,EnumPar<val_type> >::iterator it, itb(map_traits::m_.begin()), ite(map_traits::m_.end());
    for(it=itb; it!=ite; ++it)s.insert(it->second.str());
    return s;
  }
  static std::vector<std::string> enumMemberList(){
    std::vector<std::string> v;
    typename std::map<enum_type,EnumPar<val_type> >::iterator it, itb(map_traits::m_.begin()), ite(map_traits::m_.end());
    for(it=itb; it!=ite; ++it)v.push_back(it->second.str());
    return v;
  }

  static std::vector<val_type> enumValues(){
    std::vector<val_type> v;
    typename std::map<enum_type,EnumPar<val_type> >::iterator it, itb(map_traits::m_.begin()), ite(map_traits::m_.end());
    for(it=itb; it!=ite; ++it)v.push_back(it->second.val());
    return v;
  }

  static unsigned int enumNum(){
    return map_traits::m_.size();
  }

  static std::string doc(enum_type t){
    typename std::map<enum_type,EnumPar<val_type> >::iterator itf(map_traits::m_.find(t));
    return itf->second.desc();
  }

  static val_type val(enum_type t){
    typename std::map<enum_type,EnumPar<val_type> >::iterator itf(map_traits::m_.find(t));
    return itf->second.val();
  }

  /** Methods using bits and associated with enum_map */
  std::vector<int> id(){
    std::vector<int> v_id;
    typename std::map<enum_type,EnumPar<val_type> >::iterator it, itb(map_traits::m_.begin()), ite(map_traits::m_.end());
    for(it=itb; it!=ite; ++it)
      if(bits[it->first])v_id.push_back(it->second.id());
    return v_id;
  }

  std::string str(){
    return toString();
  }

  std::string toString(){
    std::ostringstream os;
    typename std::map<enum_type,EnumPar<val_type> >::iterator it, itb(map_traits::m_.begin()), ite(map_traits::m_.end());
    for(it=itb; it!=ite; ++it)
      if(bits[it->first])os<<it->second.str()<<" ";
    if(os.str().length())
      return os.str().substr(0,os.str().length()-1);
    else
      return os.str();
  }
  std::vector<enum_type> toEnumType(){
    std::vector<enum_type> v;
    typename std::map<enum_type,EnumPar<val_type> >::iterator it, itb(map_traits::m_.begin()), ite(map_traits::m_.end());
    for(it=itb; it!=ite; ++it)
      if(bits[it->first])v.push_back(it->first);
    return v;
  }
  // Method to define only when traits::maxset > 1
   EnumSetVal<enum_type,val_type>& fromString(std::vector<std::string> s){
//   EnumSetVal<enum_type,val_type>& fromString(std::vector<std::string> s, bool reset){
//     int id;
//     int nmax=s.size();
//     if(reset)bits.reset();
//     if(nmax>set_traits::maxset)nmax=set_traits::maxset;
//     int k=0;
//     for(int n=0; n<nmax; n++){
//       id = map_traits::fromStringToInt(s[n]);
//       if(id!=numeric_limits<int>::max())
// 	bits.set(enum_type(id));
//     }
//     return *this;
    unsigned int nmax=s.size(); std::cout<<"nmax="<<nmax<<std::endl; 
    bits.reset();
    if(nmax>set_traits::maxset)
      std::cout<<"WARNING: the input number of enumerators, "<<nmax<<",exceeds the maximum number, "
	       <<set_traits::maxset<<",  allowed for a compound with this EnumSet<"
	       <<map_traits::typeName_<<"> type."<<std::endl;
    bool ok;
    typename std::map<enum_type,EnumPar<val_type> >::iterator it, itb(map_traits::m_.begin()), ite(map_traits::m_.end());
    unsigned int numSet=0;
    for(unsigned int n=0; n<nmax; n++){
      ok=false;
      for(it=itb;it!=ite;++it)
	if(s[n]==it->second.str()){ok=true; break;}
      if(ok){
	bits.set(it->first);
	numSet++;
      }else{
	std::cout<<"WARNING: "<<s[n]<<" is not a valid enumerator name for this enumeration "
		 <<map_traits::typeName_<<std::endl;
      }
      if(numSet==set_traits::maxset)break;
    }
    std::cout<<"numSet="<<numSet<<std::endl;
    return *this;
  }
  // Method useful when maxset>1.
  EnumSetVal<enum_type,val_type>& fromString(std::string s, bool reset){
    if(reset)bits.reset();
    int id = map_traits::fromStringToInt(s);
    if(id==std::numeric_limits<int>::max())
      bits.reset();
    else
      bits.set(enum_type(id));
    return *this;
  }
  // Method to define only when traits::maxset > 1
  EnumSetVal<enum_type,val_type>& fromString(std::string setting)
  {
    // TODO traiter le cas de 2 enumerators comme CORRECTED and UNCORRECTED  car ==> blancs avt ou apres 
    unsigned int nmax=set_traits::maxset;
    if(nmax<set_traits::count)bits.reset();
    typename std::map<enum_type,EnumPar<val_type> >::iterator it, itb(map_traits::m_.begin()), ite(map_traits::m_.end());
    for(it=itb;it!=ite;++it)
      if(setting.find(it->second.str())!=std::string::npos)bits.set(it->first);
    return *this;
  }
  // Method to get a vector of the names of the enumerators set on.
  std::vector<std::string> names()
  {
    typename std::map<enum_type,EnumPar<val_type> >::iterator it, itb(map_traits::m_.begin()), ite(map_traits::m_.end());
    std::vector<std::string> v_s;
    unsigned int numset=0; 
    for(it=itb;it!=ite;++it){
      if(bits.test(to_bit(it->first))){
	v_s.push_back(it->second.str());
	numset++; 
      }
      if(set_traits::maxset==numset)break;
      if(set_traits::last==it->first)break;
    }
    return v_s;
  }

  // Method to get the set of enumerators
  std::set<enum_type> enumSet()
  {
    typename std::map<enum_type,EnumPar<val_type> >::iterator it, itb(map_traits::m_.begin()), ite(map_traits::m_.end());
    std::set<enum_type> s_et;
    for(it=itb;it!=ite;++it)
      if(bits.test(to_bit(it->first)))s_et.insert(it->first);
    return s_et;
  }    
  // Method to get the set of enumerator values
  std::vector<val_type> enumVal()
  {
    std::vector<val_type> v_s;
    std::set<enum_type> s_et=enumSet();
    typename std::set<enum_type>::iterator its, itsb(s_et.begin()), itse(s_et.end());
    typename std::map<enum_type,EnumPar<val_type> >::iterator itf;
    for(its=itsb; its!=itse; ++its){
      itf=map_traits::m_.find(*its);
      v_s.push_back(itf->second.val());
    }
    return v_s;
  }    


protected:
  static std::size_t to_bit(enum_type value)
  {
    return (value - set_traits::first) / set_traits::step;
  }
  std::bitset<set_traits::count> bits;
};


template<typename enum_type,
	 typename val_type=void, 
	 typename set_traits=enum_set_traits<enum_type>,
	 typename map_traits=enum_map_traits<enum_type,val_type> >
class EnumSet
{
public:
  EnumSet()
  {
    reset();
  }
  EnumSet(enum_type setting)
  {
    setOnly(setting);
  }
  EnumSet &operator&=(const EnumSet &rhs)
  {
    bits &= rhs.bits;
    return *this;
  }
  EnumSet &operator|=(const EnumSet &rhs)
  {
    bits |= rhs.bits;
    return *this;
  }
  bool less (const EnumSet &rhs){
    if(bits.to_ulong()<rhs.bits.to_ulong())return true;
    return false;
  }
  bool operator!= (const EnumSet &rhs){
    if(bits.to_ulong()==rhs.bits.to_ulong())return false;
    return true;
  }
  bool operator== (const EnumSet &rhs){
    if(bits.to_ulong()==rhs.bits.to_ulong())return true;
    return false;
  }
  EnumSet &operator^=(const EnumSet &rhs)
  {
    bits ^= rhs.bits;
    return *this;
  }
  std::size_t count() const
  {
    return bits.count();
  }
  std::size_t size() const
  {
    return bits.size();
  }
  bool operator[](enum_type testing) const
  {
    return bits.test(to_bit(testing));
  }
  bool only(enum_type testing) const
  {
    if(count()!=1)return false;
    return bits.test(to_bit(testing));
  }
  bool operator^(enum_type testing) const   // xor operator (^ Python bitwise operator) 
  {
    if(only(testing))return true;
    if(bits.test(to_bit(testing)))return false;
    return true;
  }

  EnumSet &set()
  {
    bits.set();
    return *this;
  }
  EnumSet &setOnly(enum_type setting)
  {
    bits.reset();
    bits.set(to_bit(setting), true);
    return *this;
  }
//   error: call of overloaded `set(StokesParameter)'
//   EnumSet &set(enum_type setting)
//   {
//     bits.set(to_bit(setting), true);
//     return *this;
//   }
  EnumSet &set(enum_type setting, bool value = true)
  {
    if(set_traits::maxset<set_traits::count)bits.reset();
    bits.set(to_bit(setting), value);
    return *this;
  }
  EnumSet &set(std::vector<enum_type> setting, bool value = true)
  {
//     if(set_traits::maxset<set_traits::count)bits.reset();
    bits.reset();
    unsigned int nmax=setting.size();
    if(nmax>set_traits::maxset)nmax=set_traits::maxset;
    for(unsigned int n=0; n<nmax; n++)
      bits.set(to_bit(setting[n]), value);
    return *this;
  }

  EnumSet &set(std::vector<std::string> names, bool /* value = true */)
  {
    // value is not used here, commented out above to silence a compiler warning
    bits.reset();
    typename std::map<enum_type,EnumPar<val_type> >::iterator 
      it, itb(map_traits::m_.begin()), ite(map_traits::m_.end());
    std::vector<std::string>::iterator iv, ivb=names.begin(), ive=names.end();
    for(iv=ivb; iv!=ive; ++iv)
      for(it=itb; it!=ite; ++it)if(it->second.str()==*iv)
	bits.set(to_bit(it->first));
    return *this;
  }

  EnumSet &reset()
  {
    bits.reset();
    return *this;
  }
  EnumSet &reset(enum_type resetting)
  {
    bits.reset(to_bit(resetting));
    return *this;
  }
  EnumSet &flip()
  {
    bits.flip();
    return *this;
  }
  EnumSet &flip(enum_type flipping)
  {
    bits.flip(to_bit(flipping));
    return *this;
  }
  EnumSet operator~() const
  {
    return EnumSet(*this).flip();
  }
  bool any() const
  {
    return bits.any();
  }
  bool none() const
  {
    return bits.none();
  }
  static void enumEnumerators(std::vector<enum_type>& v)
  {
    typename std::map<enum_type,EnumPar<val_type> >::iterator it, itb(map_traits::m_.begin()), ite(map_traits::m_.end());
    for(it=itb; it!=ite; ++it)v.push_back(it->first);
    return;
  }
  static std::set<std::string> enumMemberSet()
  {
    std::set<std::string> s;
    typename std::map<enum_type,EnumPar<val_type> >::iterator it, itb(map_traits::m_.begin()), ite(map_traits::m_.end());
    for(it=itb; it!=ite; ++it)s.insert(it->second.str());
    return s;
  }
  static std::vector<std::string> enumMemberList(){
    std::vector<std::string> v;
    typename std::map<enum_type,EnumPar<val_type> >::iterator it, itb(map_traits::m_.begin()), ite(map_traits::m_.end());
    for(it=itb; it!=ite; ++it)v.push_back(it->second.str());
    return v;
  }
  static std::string doc(enum_type t){
    typename std::map<enum_type,EnumPar<val_type> >::iterator itf(map_traits::m_.find(t));
    return itf->second.desc();
  }

//   static std::string val(enum_type t){
//     typename std::map<enum_type,EnumPar<val_type> >::iterator itf(map_traits::m_.find(t));
//     return itf->second.val();
//   }

  /** Methods using bits and associated with enum_map */
  std::vector<int> id(){
    std::vector<int> v_id;
    typename std::map<enum_type,EnumPar<val_type> >::iterator it, itb(map_traits::m_.begin()), ite(map_traits::m_.end());
    for(it=itb; it!=ite; ++it)
      if(bits[it->first])v_id.push_back(it->second.id());
    return v_id;
  }
  std::string str(){
    return toString();
  }
  std::string toString(){
    std::ostringstream os;
    typename std::map<enum_type,EnumPar<val_type> >::iterator it, itb(map_traits::m_.begin()), ite(map_traits::m_.end());
    for(it=itb; it!=ite; ++it)
      if(bits[it->first])os<<it->second.str()<<" ";
    if(os.str().length())
      return os.str().substr(0,os.str().length()-1);
    else
      return os.str();
  }
  std::vector<enum_type> toEnumType(){
    std::vector<enum_type> v;
    typename std::map<enum_type,EnumPar<val_type> >::iterator it, itb(map_traits::m_.begin()), ite(map_traits::m_.end());
    for(it=itb; it!=ite; ++it)
      if(bits[it->first])v.push_back(it->first);
    return v;
  }
  // Method to define only when traits::maxset > 1
  EnumSet<enum_type,val_type>& fromString(std::vector<std::string> s){
//   EnumSet<enum_type,val_type>& fromString(std::vector<std::string> s, bool reset){
//     int id;
//     int nmax=s.size();
//     if(reset)bits.reset();
//     if(nmax>set_traits::maxset)nmax=set_traits::maxset;
//     int k=0;
//     for(int n=0; n<nmax; n++){
//       id = map_traits::fromStringToInt(s[n]);
//       if(id!=numeric_limits<int>::max())
// 	bits.set(enum_type(id));
//     }
//     return *this;
    unsigned int nmax=s.size(); std::cout<<"nmax="<<nmax<<std::endl; 
    bits.reset();
    if(nmax>set_traits::maxset)
      std::cout<<"WARNING: the input number of enumerators, "<<nmax<<",exceeds the maximum number, "
	       <<set_traits::maxset<<",  allowed for a compound with this EnumSet<"
	       <<map_traits::typeName_<<"> type."<<std::endl;
    bool ok;
    typename std::map<enum_type,EnumPar<val_type> >::iterator it, itb(map_traits::m_.begin()), ite(map_traits::m_.end());
    int numSet=0;
    for(int n=0; n<nmax; n++){
      ok=false;
      for(it=itb;it!=ite;++it)
	if(s[n]==it->second.str()){ok=true; break;}
      if(ok){
	bits.set(it->first);
	numSet++;
      }else{
	std::cout<<"WARNING: "<<s[n]<<" is not a valid enumerator name for this enumeration "
		 <<map_traits::typeName_<<std::endl;
      }
      if(numSet==set_traits::maxset)break;
    }
    std::cout<<"numSet="<<numSet<<std::endl;
    return *this;
  }
  // Method useful when maxset>1.
  EnumSet<enum_type,val_type>& fromString(std::string s, bool reset){
    if(reset)bits.reset();
    typename std::map<enum_type,EnumPar<val_type> >::iterator it, itb(map_traits::m_.begin()), ite(map_traits::m_.end());
    for(it=itb;it!=ite;++it)
      if(s.find(it->second.str())!=std::string::npos)bits.set(it->first);
    return *this;
  }
  // Method to define only when traits::maxset > 1
  EnumSet <enum_type,val_type>& fromString(std::string setting)
  {
    // TODO traiter le cas de 2 enumerators comme CORRECTED and UNCORRECTED  car ==> blancs avt ou apres 
    unsigned int nmax=set_traits::maxset;
    if(nmax<set_traits::count)bits.reset();
    typename std::map<enum_type,EnumPar<val_type> >::iterator it, itb(map_traits::m_.begin()), ite(map_traits::m_.end());
    for(it=itb;it!=ite;++it)
      if(setting.find(it->second.str())!=std::string::npos)bits.set(it->first);
    return *this;
  }
  // Method to get a vector of the names of the enumerators set on.
  std::vector<std::string> names()
  {
    typename std::map<enum_type,EnumPar<val_type> >::iterator it, itb(map_traits::m_.begin()), ite(map_traits::m_.end());
    std::vector<std::string> v_s;
    unsigned int numset=0;
    for(it=itb;it!=ite;++it){
      if(bits.test(to_bit(it->first))){
	v_s.push_back(it->second.str());
	numset++; 
      }
      if(set_traits::maxset==numset)break;
      if(set_traits::last==it->first)break;
    }
    return v_s;  
  }
  // Method to get the set of enumerators
  std::set<enum_type> enumSet()
  {
    typename std::map<enum_type,EnumPar<val_type> >::iterator it, itb(map_traits::m_.begin()), ite(map_traits::m_.end());
    std::set<enum_type> s_et;
    for(it=itb;it!=ite;++it)
      if(bits.test(to_bit(it->first)))s_et.insert(it->first);
    return s_et;
  }    
//   // Method to get the set of enumerator values
//   std::vector<std::string> enumVal()
//   {
//     std::vector<std::string> v_s;
//     std::set<enum_type> s_et=enumSet();
//     typename std::set<enum_type>::iterator its, itsb(s_et.begin()), itse(s_et.end());
//     typename std::map<enum_type,EnumPar<val_type> >::iterator itf;
//     for(its=itsb; its!=itse; ++its){
//       itf=map_traits::m_.find(*its);
//       v_s.push_back(itf->second.val());
//     }
//     return v_s;
//   }    


protected:
  static std::size_t to_bit(enum_type value)
  {
    return (value - set_traits::first) / set_traits::step;
  }
  std::bitset<set_traits::count> bits;
};



template< typename enum_type, 
	  typename val_type,
	  typename set_traits=enum_set_traits<enum_type>,
	  typename map_traits=enum_map_traits<enum_type,val_type> >
class EnumVal
{
public:
  EnumVal()
  {
  }
  EnumVal(enum_type setting)
  {
    set(setting);
  }
  EnumVal &operator&=(const EnumVal &rhs)
  {
    bits &= rhs.bits;
    return *this;
  }
  EnumVal &operator|=(const EnumVal &rhs)
  {
    bits |= rhs.bits;
    return *this;
  }
  bool less (const EnumVal &rhs){
    if(bits.to_ulong()<rhs.bits.to_ulong())return true;
    return false;
  }
  bool operator!= (const EnumVal &rhs){
    if(bits.to_ulong()==rhs.bits.to_ulong())return false;
    return true;
  }
  bool operator== (const EnumVal &rhs){
    if(bits.to_ulong()==rhs.bits.to_ulong())return true;
    return false;
  }
  EnumVal &operator^=(const EnumVal &rhs)
  {
    bits ^= rhs.bits;
    return *this;
  }
  std::size_t count() const
  {
    return bits.count();
  }
  std::size_t size() const
  {
    return bits.size();
  }
  bool operator[](enum_type testing) const
  {
    return bits.test(to_bit(testing));
  }
  bool only(enum_type testing) const
  {
    if(count()!=1)return false;
    return bits.test(to_bit(testing));
  }
  bool operator^(enum_type testing) const   // xor operator (^ Python bitwise operator) 
  {
    if(only(testing))return true;
    if(bits.test(to_bit(testing)))return false;
    return true;
  }

  EnumVal &set()
  {
    bits.set();
    return *this;
  }
  /** Store a boolean value as the new value for bit at position setting.
   * @pre if the number if bits set is smaller than the maximum
   *      allowed number of bit set then the bitset is first reset
   * @param setting the position in the bitset where "value" must be stored
   * @param value the boolean value to store
   * @post all positions are unset (i.e. false), except the position "setting"
   *       would value=true
   */
  EnumVal &set(enum_type setting, bool value = true)
  {
    if(set_traits::maxset<set_traits::count)bits.reset();
    bits.set(to_bit(setting), value);
    return *this;
  }
  /** Store "true" at a set of positions.
   * @param setting vector of positions in the bitset (Order positions 
   *        are counted from the rightmost bit, which is order position 0.
   * @post the value "true" stored at these positions in the bitset.
   */
  EnumVal &set(std::vector<enum_type> setting)
  {
    bits.reset();
    unsigned int nmax=setting.size();
    if(nmax>set_traits::maxset)nmax=set_traits::maxset;
    for(int n=1; n<nmax; n++)
      bits.set(to_bit(setting[n]),true);
    return *this;
  }

  EnumVal &reset()
  {
    bits.reset();
    return *this;
  }
  EnumVal &reset(enum_type resetting)
  {
    bits.reset(to_bit(resetting));
    return *this;
  }
  EnumVal &flip()
  {
    bits.flip();
    return *this;
  }
  EnumVal &flip(enum_type flipping)
  {
    bits.flip(to_bit(flipping));
    return *this;
  }
  EnumVal operator~() const
  {
    return EnumVal(*this).flip();
  }
  bool any() const
  {
    return bits.any();
  }
  bool none() const
  {
    return bits.none();
  }
  static std::set<std::string> enumMemberSet()
  {
    std::set<std::string> s;
    typename std::map<enum_type,EnumPar<val_type> >::iterator it, itb(map_traits::m_.begin()), ite(map_traits::m_.end());
    for(it=itb; it!=ite; ++it)s.insert(it->second.str());
    return s;
  }
  static std::vector<std::string> enumMemberList(){
    std::vector<std::string> v;
    typename std::map<enum_type,EnumPar<val_type> >::iterator it, itb(map_traits::m_.begin()), ite(map_traits::m_.end());
    for(it=itb; it!=ite; ++it)v.push_back(it->second.str());
    return v;
  }

  static std::vector<val_type> enumValues(){
    std::vector<val_type> v;
    typename std::map<enum_type,EnumPar<val_type> >::iterator it, itb(map_traits::m_.begin()), ite(map_traits::m_.end());
    for(it=itb; it!=ite; ++it)v.push_back(it->second.val());
    return v;
  }

  static unsigned int enumNum(){
    return map_traits::m_.size();
  }

  static std::string doc(enum_type t){
    typename std::map<enum_type,EnumPar<val_type> >::iterator itf(map_traits::m_.find(t));
    return itf->second.desc();
  }

  static val_type val(enum_type t){
    typename std::map<enum_type,EnumPar<val_type> >::iterator itf(map_traits::m_.find(t));
    return itf->second.val();
  }

  // Method to get the name of the enumerator set on, if set, else return empty string
  std::string name()
  {
    typename std::map<enum_type,EnumPar<val_type> >::iterator it, itb(map_traits::m_.begin()), ite(map_traits::m_.end());
    std::string s="";
    for(it=itb;it!=ite;++it){
      if(bits.test(to_bit(it->first)))
	return it->second.str();
      if(set_traits::last==it->first)break;
    }
    return s;
  }
  /** Methods using bits and associated with enum_map */
  int id(){
    int id;
    typename std::map<enum_type,EnumPar<val_type> >::iterator it, itb(map_traits::m_.begin()), ite(map_traits::m_.end());
    for(it=itb; it!=ite; ++it)
      if(bits[it->first])id=it->second.id();
    return id;
  }
  std::string str(){
    return toString();
  }
  std::string toString(){
    std::ostringstream os;
    typename std::map<enum_type,EnumPar<val_type> >::iterator it, itb(map_traits::m_.begin()), ite(map_traits::m_.end());
    for(it=itb; it!=ite; ++it)
      if(bits[it->first])os<<it->second.str();
    return os.str();
  }
  enum_type toEnumType(){
    typename std::map<enum_type,EnumPar<val_type> >::iterator it, itb(map_traits::m_.begin()), ite(map_traits::m_.end());
    for(it=itb; it!=ite; ++it)
      if(bits[it->first])return it->first;
    std::cout<<"ERROR: state with no enumerator set"<<std::endl;
  }

  EnumVal<enum_type,val_type>& fromString(std::string setting)
  {
    bits.reset();
    typename std::map<enum_type,EnumPar<val_type> >::iterator it, itb(map_traits::m_.begin()), ite(map_traits::m_.end());
    for(it=itb;it!=ite;++it)
      if(setting.find(it->second.str())!=std::string::npos)bits.set(it->first);
    return *this;
  }
  // Method to get the enumerator value for the bit currently set
  std::vector<val_type> enumVal()
  {
    std::vector<val_type> v;
    typename std::map<enum_type,EnumPar<val_type> >::iterator 
      it, 
      itb=map_traits::m_.begin(), 
      ite=map_traits::m_.end();
    for(it=itb; it!=ite; ++it)
      if(bits.test(it->first))v.push_back(it->second.val());
    return v;
  }    


protected:
  static std::size_t to_bit(enum_type value)
  {
    return (value - set_traits::first) / set_traits::step;
  }
  std::bitset<set_traits::count> bits;
};


template<typename enum_type,
	 typename val_type=void, 
	 typename set_traits=enum_set_traits<enum_type>,
	 typename map_traits=enum_map_traits<enum_type,val_type> >
class Enum
{
public:
  Enum()
  {
  }
  Enum(const enum_type setting)
  {
    set(setting);
  }
  Enum &operator&=(const Enum &rhs)
  {
    bits &= rhs.bits;
    return *this;
  }
  Enum &operator|=(const Enum &rhs)
  {
    bits |= rhs.bits;
    return *this;
  }
  bool less (const Enum &rhs){
    if(bits.to_ulong()<rhs.bits.to_ulong())return true;
    return false;
  }
  bool operator!= (const Enum &rhs){
    if(bits.to_ulong()==rhs.bits.to_ulong())return false;
    return true;
  }
  bool operator== (const Enum &rhs){
    if(bits.to_ulong()==rhs.bits.to_ulong())return true;
    return false;
  }
  Enum &operator^=(const Enum &rhs)
  {
    bits ^= rhs.bits;
    return *this;
  }
  std::size_t count() const
  {
    return bits.count();
  }
  std::size_t size() const
  {
    return bits.size();
  }
  bool operator[](enum_type testing) const
  {
    return bits.test(to_bit(testing));
  }
  bool only(enum_type testing) const
  {
    if(count()!=1)return false;
    return bits.test(to_bit(testing));
  }
  bool operator^(enum_type testing) const   // xor operator (^ Python bitwise operator) 
  {
    if(only(testing))return true;
    if(bits.test(to_bit(testing)))return false;
    return true;
  }

  Enum &set()
  {
    bits.set();
    return *this;
  }
  Enum &set(enum_type setting)
  {
    bits.reset();
    bits.set(to_bit(setting), true);
    return *this;
  }
  Enum &reset()
  {
    bits.reset();
    return *this;
  }
  Enum &reset(enum_type resetting)
  {
    bits.reset(to_bit(resetting));
    return *this;
  }
  Enum &flip()
  {
    bits.flip();
    return *this;
  }
  Enum &flip(enum_type flipping)
  {
    bits.flip(to_bit(flipping));
    return *this;
  }
  Enum operator~() const
  {
    return Enum(*this).flip();
  }
  bool any() const
  {
    return bits.any();
  }
  bool none() const
  {
    return bits.none();
  }
  static std::set<std::string> enumMemberSet()
  {
    std::set<std::string> s;
    typename std::map<enum_type,EnumPar<val_type> >::iterator it, itb(map_traits::m_.begin()), ite(map_traits::m_.end());
    for(it=itb; it!=ite; ++it)s.insert(it->second.str());
    return s;
  }
  static std::vector<std::string> enumMemberList(){
    std::vector<std::string> v;
    typename std::map<enum_type,EnumPar<val_type> >::iterator it, itb(map_traits::m_.begin()), ite(map_traits::m_.end());
    for(it=itb; it!=ite; ++it)v.push_back(it->second.str());
    return v;
  }
  static std::string doc(enum_type t){
    typename std::map<enum_type,EnumPar<val_type> >::iterator itf(map_traits::m_.find(t));
    return itf->second.desc();
  }

  // Method to get the name of the enumerator set on, if set, else return empty string
  std::string name()
  {
    typename std::map<enum_type,EnumPar<val_type> >::iterator it, itb(map_traits::m_.begin()), ite(map_traits::m_.end());
    std::string s="";
    for(it=itb;it!=ite;++it){
      if(bits.test(to_bit(it->first)))
	return it->second.str();
      if(set_traits::last==it->first)break;
    }
    return s;
  }

  unsigned int hash() {
    std::string s = str();
    unsigned int hash = 0;
    for(size_t i = 0; i < s.size(); ++i) 
      hash = 65599 * hash + s[i];
    return hash ^ (hash >> 16);
  }

  /** Methods using bits and associated with enum_map */
  int id(){
    int id;
    typename std::map<enum_type,EnumPar<val_type> >::iterator it, itb(map_traits::m_.begin()), ite(map_traits::m_.end());
    for(it=itb; it!=ite; ++it)
      if(bits[it->first])id=it->second.id();
    return id;
  }
  std::string str(){
    return toString();
  }
  std::string toString(){
    std::ostringstream os;
    typename std::map<enum_type,EnumPar<val_type> >::iterator it, itb(map_traits::m_.begin()), ite(map_traits::m_.end());
    for(it=itb; it!=ite; ++it)
      if(bits[it->first])os<<it->second.str();
    return os.str();
  }
  enum_type toEnumType() const{
    typename std::map<enum_type,EnumPar<val_type> >::iterator it, itb(map_traits::m_.begin()), ite(map_traits::m_.end());
    for(it=itb; it!=ite; ++it)
      if(bits[it->first])return it->first;
    std::cout<<"ERROR: state with no enumerator set"<<std::endl;
  }
  // Method useful when maxset>1.
  Enum<enum_type,val_type>& fromString(std::string s){
    bits.reset();
    typename std::map<enum_type,EnumPar<val_type> >::iterator it, itb(map_traits::m_.begin()), ite(map_traits::m_.end());
    for(it=itb;it!=ite;++it)
      if(s.find(it->second.str())!=std::string::npos)bits.set(it->first);
    return *this;
  }


protected:
  static std::size_t to_bit(enum_type value)
  {
    return (value - set_traits::first) / set_traits::step;
  }
  std::bitset<set_traits::count> bits;
};



#define _ENUM_H
#endif
