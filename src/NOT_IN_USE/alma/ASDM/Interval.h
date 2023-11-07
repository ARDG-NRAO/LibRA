/*
 * ALMA - Atacama Large Millimeter Array
 * (c) European Southern Observatory, 2002
 * (c) Associated Universities Inc., 2002
 * Copyright by ESO (in the framework of the ALMA collaboration),
 * Copyright by AUI (in the framework of the ALMA collaboration),
 * All rights reserved.
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY, without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307  USA
 *
 * File Interval.h
 */

#ifndef Interval_CLASS
#define Interval_CLASS

#include <stdint.h>
#include <vector>
#include <iostream>
#include <string>

#ifndef WITHOUT_ACS
#include <asdmIDLTypesC.h>
#endif

#include <alma/ASDM/StringTokenizer.h>
#include <alma/ASDM/NumberFormatException.h>

#include <alma/ASDM/EndianStream.h>

namespace asdm {

  class Interval;
  Interval operator * ( int64_t , const Interval & );
  std::ostream & operator << ( std::ostream &, const Interval & );
  std::istream & operator >> ( std::istream &, Interval&);

  /**
   * The Interval class implements an interval of time in units
   * of nanoseconds.
   * 
   * @version 1.00 Jan. 7, 2005
   * @author Allen Farris
   */
  class Interval {
    friend Interval operator * ( int64_t , const Interval & );
    friend std::ostream & operator << ( std::ostream &, const Interval & );
    friend std::istream & operator >> ( std::istream &, Interval&);

  public:
    static int64_t fromString(const std::string&);
    static std::string toString(int64_t);
    static Interval getInterval(StringTokenizer &t);

    Interval();                              		// default constructor
    Interval(const Interval &);						// X const X& constructor
    Interval(const std::string &s);
#ifndef WITHOUT_ACS
    Interval(const asdmIDLTypes::IDLInterval &);
#endif
    Interval(int64_t value);
    virtual ~Interval();							// destructor

    Interval& operator = (const Interval&);			// assignment operator
    Interval& operator = (const int64_t);			// assignment operator

    Interval& operator += (const Interval&);		// assignment with arithmetic
    Interval& operator -= (const Interval&);		//	operators
    Interval& operator *= (const int64_t);
    Interval& operator /= (const int64_t);

    Interval operator + (const Interval&) const;	// arithmetic operators
    Interval operator - (const Interval&) const;
    Interval operator * (const int64_t) const;
    Interval operator / (const int64_t) const;

    bool operator < (const Interval&) const;		// comparison operators
    bool operator > (const Interval&) const;
    bool operator <= (const Interval&) const;
    bool operator >= (const Interval&) const;
    bool operator == (const Interval&) const;
    bool equals(const Interval&) const;
    bool operator != (const Interval&) const;

    bool isZero() const;

    Interval operator - () const;					// unary minus
    Interval operator + () const; 				// unary plus

    std::string toString() const;
    std::string toStringI() const;
	
    /**
     * Write the binary representation of this into an EndianOSStream
     * @param eoss
     *
     */
    void  toBin(EndianOSStream& eoss) ;
	
    /**
     * Write the binary representation of a vector of Interval into a EndianOSStream.
     * @param interval
     * @param eoss
     */
    static void toBin(std::vector<Interval> interval, EndianOSStream& eoss) ;
	
    /**
     * Write the binary representation of a vector of vector  of Interval into a EndianOSStream.
     * @param interval
     * @param eoss
     */
    static void toBin(std::vector<std::vector<Interval> > interval, EndianOSStream& eoss) ;
	
    /**
     * Read the binary representation of an Interval from an EndianIStream
     * and use the read value to set an  Interval.
     * @param eis the EndianIStream to be read
     * @return an Interval
     */
    static Interval fromBin(EndianIStream& eis);
	
    /**
     * Read the binary representation of a vector of Interval from a EndianIStream
     * and use the read value to set a vector of Interval.
     * @param eis the EndianIStream to be read
     * @return a vector of Interval
     */
    static std::vector<Interval> from1DBin(EndianIStream& eis);
	
    /**
     * Read the binary representation of a vector of vector of Interval from a EndianIStream
     * and use the read value to set a vector of vector of Interval.
     * @param eis the EndianIStream  to be read
     * @return a vector of vector of Interval
     */
    static std::vector<std::vector<Interval> >from2DBin(EndianIStream& eis);	
	
    operator std::string () const;
    int64_t get() const;
#ifndef WITHOUT_ACS
    asdmIDLTypes::IDLInterval toIDLInterval() const;
#endif
    static std::string unit();

  private:
    int64_t value;

  };

  // Interval constructors
  inline Interval::Interval() : value(0L) {
  }

  inline Interval::Interval(const Interval &t) : value(t.value) {
  }

#ifndef WITHOUT_ACS
  inline Interval::Interval(const asdmIDLTypes::IDLInterval &l) : value(l.value) {
  }
#endif

  inline Interval::Interval(const std::string &s) : value(fromString(s)) {
  }

  inline Interval::Interval(int64_t v) : value(v) {
  }

  // Interval destructor
  inline Interval::~Interval() { }

  // assignment operator
  inline Interval& Interval::operator = ( const Interval &t ) {
    value = t.value;
    return *this;
  }

  // assignment operator
  inline Interval& Interval::operator = ( const int64_t v ) {
    value = v;
    return *this;
  }

  // assignment with arithmetic operators
  inline Interval& Interval::operator += ( const Interval& t) {
    value += t.value;
    return *this;
  }

  inline Interval& Interval::operator -= ( const Interval& t) {
    value -= t.value;
    return *this;
  }

  inline Interval& Interval::operator *= ( const int64_t n) {
    value *= n;
    return *this;
  }

  inline Interval& Interval::operator /= ( const int64_t n) {
    value /= n;
    return *this;
  }

  // arithmetic functions
  inline Interval Interval::operator + ( const Interval &t2 ) const {
    Interval tmp;
    tmp.value = value + t2.value;
    return tmp;
  }

  inline Interval Interval::operator - ( const Interval &t2 ) const {
    Interval tmp;
    tmp.value = value - t2.value;
    return tmp;
  }
  inline Interval Interval::operator * ( const int64_t n) const {
    Interval tmp;
    tmp.value = value * n;
    return tmp;
  }

  inline Interval Interval::operator / ( const int64_t n) const {
    Interval tmp;
    tmp.value = value / n;
    return tmp;
  }

  // comparison operators
  inline bool Interval::operator < (const Interval& x) const {
    return (value < x.value);
  }

  inline bool Interval::operator > (const Interval& x) const {
    return (value > x.value);
  }

  inline bool Interval::operator <= (const Interval& x) const {
    return (value <= x.value);
  }

  inline bool Interval::operator >= (const Interval& x) const {
    return (value >= x.value);
  }

  inline bool Interval::operator == (const Interval& x) const {
    return (value == x.value);
  }
  inline bool Interval::equals(const Interval& x) const {
    return (value == x.value);
  }

  inline bool Interval::operator != (const Interval& x) const {
    return (value != x.value);
  }

  // unary - and + operators
  inline Interval Interval::operator - () const {
    Interval tmp;
    tmp.value = -value;
    return tmp;
  }

  inline Interval Interval::operator + () const {
    Interval tmp;
    tmp.value = value;
    return tmp;
  }

  // Conversion functions
  inline Interval::operator std::string () const {
    return toString();
  }

  inline std::string Interval::toString() const {
    return toString(value);
  }

  inline std::string Interval::toStringI() const {
    return toString(value);
  }

  inline int64_t Interval::get() const {
    return value;
  }

#ifndef WITHOUT_ACS
  inline asdmIDLTypes::IDLInterval Interval::toIDLInterval() const {
    asdmIDLTypes::IDLInterval tmp;
    tmp.value = value;
    return tmp;
  }
#endif

  // Friend functions

  inline Interval operator * ( int64_t n, const Interval &x) {
    Interval tmp;
    tmp.value = x.value * n;
    return tmp;
  }

  inline std::ostream & operator << ( std::ostream &o, const Interval &x ) {
    o << x.value;
    return o;
  }

  inline std::istream & operator >> ( std::istream &i, Interval &x ) {
    i >> x.value;
    return i;
  }

  inline std::string Interval::unit() {
    return std::string ("nanosec");
  }

} // End namespace asdm

#endif /* Interval_CLASS */
