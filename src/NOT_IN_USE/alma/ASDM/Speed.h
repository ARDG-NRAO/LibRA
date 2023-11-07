//Speed.h generated on 'Thu Feb 04 10:20:05 CET 2010'. Edit at your own risk.
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
 * File Speed.h
 */
#ifndef Speed_CLASS
#define Speed_CLASS
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
class Speed;
Speed operator * ( double , const Speed & );
std::ostream & operator << ( std::ostream &, const Speed & );
std::istream & operator >> ( std::istream &, Speed &);
/**
 * The Speed class implements a quantity of speed in meters per second..
 * 
 * @version 1.00 Jan. 7, 2005
 * @author Allen Farris
 * 
 * @version 1.1 Aug 8, 2006
 * @author Michel Caillat 
 * added toBin/fromBin methods.
 */
class Speed {
  /**
   * Overloading of multiplication operator.
   * @param d a value in double precision .
   * @param x a const reference to a Speed .
   * @return a Speed 
   */
  friend Speed operator * ( double d, const Speed & x );
  /**
   * Overloading of << to output the value an Speed on an ostream.
   * @param os a reference to the ostream to be written on.
   * @param x a const reference to a Speed.
   */
  friend std::ostream & operator << ( std::ostream & os, const Speed & x);
  /**
   * Overloading of >> to read an Speed from an istream.
   */
  friend std::istream & operator >> ( std::istream & is, Speed & x);
public:
	/**
	 * The nullary constructor (default).
	 */
	Speed();
	/**
	 * The copy constructor.
	 */
	Speed(const Speed &);
	/**
	 * A constructor from a string representation.
	 * The string passed in argument must be parsable into a double precision
	 * number to express the value in radian of the angle.
	 *
	 * @param s a string.
	 */
	Speed(const std::string &s);
#ifndef WITHOUT_ACS
	/**
	 *
	 * A constructor from a CORBA/IDL representation.
	 * 
	 * @param idlSpeed a cons ref to an IDLSpeed.
	 */
	Speed(const asdmIDLTypes::IDLSpeed & idlSpeed);
#endif
	/**
	 * A constructor from a value in double precision.
	 * The value passed in argument defines the value of the Speed in radian.
	 */
	Speed(double value);
	/**
	 * The destructor.
	 */
	virtual ~Speed();
	/**
	 * A static method equivalent to the constructor from a string.
	 * @param s a string?.
	 */
	static double fromString(const std::string& s);
	/**
	 * Conversion into string.
	 * The resulting string contains the representation of the value of this Speed.
	 *
	 * @return string
	 */
	static std::string toString(double);
	/**
	 * Parse the next (string) token of a StringTokenizer into an angle.
	 * @param st a reference to a StringTokenizer.
	 * @return an Speed.
	 */
	static Speed getSpeed(StringTokenizer &st);
			
	/**
	 * Write the binary representation of this to an EndianOSStream .
	 * @param eoss a reference to an EndianOSStream .
	 */		
	void toBin(EndianOSStream& eoss);
	/**
	 * Write the binary representation of a vector of Speed to a EndianOSStream.
	 * @param angle the vector of Speed to be written
	 * @param eoss the EndianOSStream to be written to
	 */
	static void toBin(const std::vector<Speed>& angle,  EndianOSStream& eoss);
	
	/**
	 * Write the binary representation of a vector of vector of Speed to a EndianOSStream.
	 * @param angle the vector of vector of Speed to be written
	 * @param eoss the EndianOSStream to be written to
	 */	
	static void toBin(const std::vector<std::vector<Speed> >& angle,  EndianOSStream& eoss);
	
	/**
	 * Write the binary representation of a vector of vector of vector of Speed to a EndianOSStream.
	 * @param angle the vector of vector of vector of Speed to be written
	 * @param eoss the EndianOSStream to be written to
	 */
	static void toBin(const std::vector<std::vector<std::vector<Speed> > >& angle,  EndianOSStream& eoss);
	/**
	 * Read the binary representation of an Speed from a EndianIStream
	 * and use the read value to set an  Speed.
	 * @param eis the EndianStream to be read
	 * @return an Speed
	 */
	static Speed fromBin(EndianIStream& eis);
	
	/**
	 * Read the binary representation of  a vector of  Speed from an EndianIStream
	 * and use the read value to set a vector of  Speed.
	 * @param eis a reference to the EndianIStream to be read
	 * @return a vector of Speed
	 */	 
	 static std::vector<Speed> from1DBin(EndianIStream & eis);
	 
	/**
	 * Read the binary representation of  a vector of vector of Speed from an EndianIStream
	 * and use the read value to set a vector of  vector of Speed.
	 * @param eis the EndianIStream to be read
	 * @return a vector of vector of Speed
	 */	 
	 static std::vector<std::vector<Speed> > from2DBin(EndianIStream & eis);
	 
	/**
	 * Read the binary representation of  a vector of vector of vector of Speed from an EndianIStream
	 * and use the read value to set a vector of  vector of vector of Speed.
	 * @param eis the EndianIStream to be read
	 * @return a vector of vector of vector of Speed
	 */	 
	 static std::vector<std::vector<std::vector<Speed> > > from3DBin(EndianIStream & eis);	 
	 
	 /**
	  * An assignment operator Speed = Speed.
	  * @param x a const reference to an Speed.
	  */
	 Speed & operator = (const Speed & x);
	 
	 /**
	  * An assignment operator Speed = double.
	  * @param d a value in double precision.
	  */
	 Speed & operator = (const double d);
	 /**
	  * Operator increment and assign.
	  * @param x a const reference to an Speed.
	  */
	Speed & operator += (const Speed & x);
	/**
	 * Operator decrement and assign.
	 * @param x a const reference to an Speed.
	 */
	Speed & operator -= (const Speed & x);
	/**
	 * Operator multiply and assign.
	 * @param x a value in double precision.
	 */
	Speed & operator *= (const double x);
	/**
	 * Operator divide and assign.
	 * @param x a valye in double precision.
	 */
	Speed & operator /= (const double x);
	/**
	 * Addition operator.
	 * @param x a const reference to a Speed.
	 */
	Speed operator + (const Speed & x) const;
	/**
	 * Substraction operator.
	 * @param x a const reference to a Speed.
	 */
	Speed operator - (const Speed & x) const;
	/**
	 * Multiplication operator.
	 * @param x a value in double precision.
	 */
	Speed operator * (const double x) const;
	/**
	 * Division operator.
	 * @param d a value in double precision.
	 */
	Speed operator / (const double x) const;
	/**
	 * Comparison operator. Less-than.
	 * @param x a const reference to a Speed.
	 */
	bool operator < (const Speed & x) const;
	/**
	 * Comparison operator. Greater-than.
	 * @param x a const reference to a Speed.
	 */
	bool operator > (const Speed & x) const;
	/**
	 * Comparison operator. Less-than or equal.
	 * @param x a const reference to a Speed.
	 */	
	bool operator <= (const Speed & x) const;
	/**
	 * Comparison operator. Greater-than or equal.
	 * @param x a const reference to a Speed.
	 */
	bool operator >= (const Speed & x) const;
	/**
	 * Comparision operator. Equal-to.
	 * @param x a const reference to a Speed.
	 */
	bool operator == (const Speed & x) const;
	/** 
	 * Comparison method. Equality.
	 * @param x a const reference to a Speed.
	 */
	bool equals(const Speed & x) const;
	/**
	 * Comparison operator. Not-equal.
	 * @param x a const reference to a Speed.
	 */
	bool operator != (const Speed & x) const;
	/**
	 * Comparison method. Test nullity.
	 * @return a bool.
	 */
	bool isZero() const;
	/**
	 * Unary operator. Opposite.
	 */
	Speed operator - () const;
	/**
	 * Unary operator. Unary plus.
	 */
	Speed operator + () const;
	/**
	 * Converts into a string.
	 * @return a string containing the representation of a the value in double precision.
	 */
	std::string toString() const;
	/** 
	 * Idem toString.
	 */
	std::string toStringI() const;
	/**
	 * Conversion operator.
	 * Converts into a string.
	 */
	operator std::string () const;
	/**
	 * Return the double precision value of the Speed.
	 * @return double
	 */
	double get() const;
#ifndef WITHOUT_ACS
	/**
	 * Return the IDLSpeed representation of the Speed.
	 * @return IDLSpeed 
	 */
    asdmIDLTypes::IDLSpeed toIDLSpeed() const;
#endif
	/**
	 * Returns the abbreviated name of the unit implicitely associated to any Speed.
	 * @return string
	 */
	static std::string unit();
private:
	double value;
};
// Speed constructors
inline Speed::Speed() : value(0.0) {
}
inline Speed::Speed(const Speed &t) : value(t.value) {
}
#ifndef WITHOUT_ACS
inline Speed::Speed(const asdmIDLTypes::IDLSpeed &l) : value(l.value) {
}
#endif
inline Speed::Speed(const std::string &s) : value(fromString(s)) {
}
inline Speed::Speed(double v) : value(v) {
}
// Speed destructor
inline Speed::~Speed() { }
// assignment operator
inline Speed & Speed::operator = ( const Speed &t ) {
	value = t.value;
	return *this;
}
// assignment operator
inline Speed & Speed::operator = ( const double v ) {
	value = v;
	return *this;
}
// assignment with arithmetic operators
inline Speed & Speed::operator += ( const Speed & t) {
	value += t.value;
	return *this;
}
inline Speed & Speed::operator -= ( const Speed & t) {
	value -= t.value;
	return *this;
}
inline Speed & Speed::operator *= ( const double n) {
	value *= n;
	return *this;
}
inline Speed & Speed::operator /= ( const double n) {
	value /= n;
	return *this;
}
// arithmetic functions
inline Speed Speed::operator + ( const Speed &t2 ) const {
	Speed tmp;
	tmp.value = value + t2.value;
	return tmp;
}
inline Speed Speed::operator - ( const Speed &t2 ) const {
	Speed tmp;
	tmp.value = value - t2.value;
	return tmp;
}
inline Speed Speed::operator * ( const double n) const {
	Speed tmp;
	tmp.value = value * n;
	return tmp;
}
inline Speed Speed::operator / ( const double n) const {
	Speed tmp;
	tmp.value = value / n;
	return tmp;
}
// comparison operators
inline bool Speed::operator < (const Speed & x) const {
	return (value < x.value);
}
inline bool Speed::operator > (const Speed & x) const {
	return (value > x.value);
}
inline bool Speed::operator <= (const Speed & x) const {
	return (value <= x.value);
}
inline bool Speed::operator >= (const Speed & x) const {
	return (value >= x.value);
}
inline bool Speed::equals(const Speed & x) const {
	return (value == x.value);
}
inline bool Speed::operator == (const Speed & x) const {
	return (value == x.value);
}
inline bool Speed::operator != (const Speed & x) const {
	return (value != x.value);
}
// unary - and + operators
inline Speed Speed::operator - () const {
	Speed tmp;
        tmp.value = -value;
	return tmp;
}
inline Speed Speed::operator + () const {
	Speed tmp;
    tmp.value = value;
	return tmp;
}
// Conversion functions
inline Speed::operator std::string () const {
	return toString();
}
inline std::string Speed::toString() const {
	return toString(value);
}
inline std::string Speed::toStringI() const {
	return toString(value);
}
inline double Speed::get() const {
	return value;
}
#ifndef WITHOUT_ACS
inline asdmIDLTypes::IDLSpeed Speed::toIDLSpeed() const {
    asdmIDLTypes::IDLSpeed tmp;
	tmp.value = value;
	return tmp;
}
#endif
// Friend functions
inline Speed operator * ( double n, const Speed &x) {
	Speed tmp;
	tmp.value = x.value * n;
	return tmp;
}
inline std::ostream & operator << ( std::ostream &o, const Speed &x ) {
	o << x.value;
	return o;
}
inline std::istream & operator >> ( std::istream &i, Speed &x ) {
	i >> x.value;
	return i;
}
inline std::string Speed::unit() {
	return std::string ("m/s");
}
} // End namespace asdm
#endif /* Speed_CLASS */
