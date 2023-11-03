#if     !defined(_ENUMSET_H)

#include <bitset>
#include <vector>

template<typename type>
struct enum_set_traits
{
  typedef type enum_type;
  static const bool         is_specialized = false;
  static const type         first = type();
  static const type         last = type();
  static const int          step = 0;
  static const std::size_t  count = 0;
  static const unsigned int maxset;
};

template< typename type, unsigned int maxset_value,
	  type last_value, type first_value = type(), 
	  int step_value = 1>
struct enum_set_traiter
{
  typedef type enum_type;
  static const bool         is_specialized = true;
  static const type         first  = first_value;
  static const type         last   = last_value;
  static const int          step   = step_value;
  static const std::size_t  count  = (last - first) / step + 1;
  static const unsigned int maxset = maxset_value;
};


template<typename enum_type,
	 typename set_traits=enum_set_traits<enum_type> >
class enum_set
{
public:
  enum_set()
  {
  }
  enum_set(enum_type setting)
  {
    set(setting);
  }
  enum_set &operator&=(const enum_set &rhs)
  {
    bits &= rhs.bits;
    return *this;
  }
  enum_set &operator|=(const enum_set &rhs)
  {
    bits |= rhs.bits;
    return *this;
  }
  enum_set &operator^=(const enum_set &rhs)
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
  enum_set &set()
  {
    bits.set();
    return *this;
  }
  enum_set &set(enum_type setting, bool value = true)
  {
    if(set_traits::maxset<set_traits::count)bits.reset();
    bits.set(to_bit(setting), value);
    return *this;
  }
  enum_set &set(std::vector<enum_type> setting, bool value = true)
  {
    if(set_traits::maxset<set_traits::count)bits.reset();
    unsigned int nmax=setting.size();
    if(nmax>set_traits::maxset)nmax=set_traits::maxset;
  for(unsigned int n=0; n<nmax; n++)
    bits.set(to_bit(setting[n]), value);
    return *this;
  }

  enum_set &reset()
  {
    bits.reset();
    return *this;
  }
  enum_set &reset(enum_type resetting)
  {
    bits.reset(to_bit(resetting));
    return *this;
  }
  enum_set &flip()
  {
    bits.flip();
    return *this;
  }
  enum_set &flip(enum_type flipping)
  {
    bits.flip(to_bit(flipping));
    return *this;
  }
  enum_set operator~() const
  {
    return enum_set(*this).flip();
  }
  bool any() const
  {
    return bits.any();
  }
  bool none() const
  {
    return bits.none();
  }
  /** ... */
protected:
  static std::size_t to_bit(enum_type value)
  {
    return (value - set_traits::first) / set_traits::step;
  }
  std::bitset<set_traits::count> bits;
};

#define _ENUMSET_H
#endif




