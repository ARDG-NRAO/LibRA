//Humidity.h generated on 'Thu Feb 04 10:20:05 CET 2010'. Edit at your own risk.
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
 * File Humidity.h
 */
#ifndef Humidity_CLASS
#define Humidity_CLASS
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
class Humidity;
Humidity operator * ( double , const Humidity & );
std::ostream & operator << ( std::ostream &, const Humidity & );
std::istream & operator >> ( std::istream &, Humidity &);
/**
 * The Humidity class implements a quantity of humidity in percent..
 * 
 * @version 1.00 Jan. 7, 2005
 * @author Allen Farris
 * 
 * @version 1.1 Aug 8, 2006
 * @author Michel Caillat 
 * added toBin/fromBin methods.
 */
class Humidity {
  /**
   * Overloading of multiplication operator.
   * @param d a value in double precision .
   * @param x a const reference to a Humidity .
   * @return a Humidity 
   */
  friend Humidity operator * ( double d, const Humidity & x );
  /**
   * Overloading of << to output the value an Humidity on an ostream.
   * @param os a reference to the ostream to be written on.
   * @param x a const reference to a Humidity.
   */
  friend std::ostream & operator << ( std::ostream & os, const Humidity & x);
  /**
   * Overloading of >> to read an Humidity from an istream.
   */
  friend std::istream & operator >> ( std::istream & is, Humidity & x);
public:
	/**
	 * The nullary constructor (default).
	 */
	Humidity();
	/**
	 * The copy constructor.
	 */
	Humidity(const Humidity &);
	/**
	 * A constructor from a string representation.
	 * The string passed in argument must be parsable into a double precision
	 * number to express the value in radian of the angle.
	 *
	 * @param s a string.
	 */
	Humidity(const std::string &s);
#ifndef WITHOUT_ACS
	/**
	 *
	 * A constructor from a CORBA/IDL representation.
	 * 
	 * @param idlHumidity a cons ref to an IDLHumidity.
	 */
	Humidity(const asdmIDLTypes::IDLHumidity & idlHumidity);
#endif
	/**
	 * A constructor from a value in double precision.
	 * The value passed in argument defines the value of the Humidity in radian.
	 */
	Humidity(double value);
	/**
	 * The destructor.
	 */
	virtual ~Humidity();
	/**
	 * A static method equivalent to the constructor from a string.
	 * @param s a string?.
	 */
	static double fromString(const std::string& s);
	/**
	 * Conversion into string.
	 * The resulting string contains the representation of the value of this Humidity.
	 *
	 * @return string
	 */
	static std::string toString(double);
	/**
	 * Parse the next (string) token of a StringTokenizer into an angle.
	 * @param st a reference to a StringTokenizer.
	 * @return an Humidity.
	 */
	static Humidity getHumidity(StringTokenizer &st);
			
	/**
	 * Write the binary representation of this to an EndianOSStream .
	 * @param eoss a reference to an EndianOSStream .
	 */		
	void toBin(EndianOSStream& eoss);
	/**
	 * Write the binary representation of a vector of Humidity to a EndianOSStream.
	 * @param angle the vector of Humidity to be written
	 * @param eoss the EndianOSStream to be written to
	 */
	static void toBin(const std::vector<Humidity>& angle,  EndianOSStream& eoss);
	
	/**
	 * Write the binary representation of a vector of vector of Humidity to a EndianOSStream.
	 * @param angle the vector of vector of Humidity to be written
	 * @param eoss the EndianOSStream to be written to
	 */	
	static void toBin(const std::vector<std::vector<Humidity> >& angle,  EndianOSStream& eoss);
	
	/**
	 * Write the binary representation of a vector of vector of vector of Humidity to a EndianOSStream.
	 * @param angle the vector of vector of vector of Humidity to be written
	 * @param eoss the EndianOSStream to be written to
	 */
	static void toBin(const std::vector<std::vector<std::vector<Humidity> > >& angle,  EndianOSStream& eoss);
	/**
	 * Read the binary representation of an Humidity from a EndianIStream
	 * and use the read value to set an  Humidity.
	 * @param eis the EndianStream to be read
	 * @return an Humidity
	 */
	static Humidity fromBin(EndianIStream& eis);
	
	/**
	 * Read the binary representation of  a vector of  Humidity from an EndianIStream
	 * and use the read value to set a vector of  Humidity.
	 * @param eis a reference to the EndianIStream to be read
	 * @return a vector of Humidity
	 */	 
	 static std::vector<Humidity> from1DBin(EndianIStream & eis);
	 
	/**
	 * Read the binary representation of  a vector of vector of Humidity from an EndianIStream
	 * and use the read value to set a vector of  vector of Humidity.
	 * @param eis the EndianIStream to be read
	 * @return a vector of vector of Humidity
	 */	 
	 static std::vector<std::vector<Humidity> > from2DBin(EndianIStream & eis);
	 
	/**
	 * Read the binary representation of  a vector of vector of vector of Humidity from an EndianIStream
	 * and use the read value to set a vector of  vector of vector of Humidity.
	 * @param eis the EndianIStream to be read
	 * @return a vector of vector of vector of Humidity
	 */	 
	 static std::vector<std::vector<std::vector<Humidity> > > from3DBin(EndianIStream & eis);	 
	 
	 /**
	  * An assignment operator Humidity = Humidity.
	  * @param x a const reference to an Humidity.
	  */
	 Humidity & operator = (const Humidity & x);
	 
	 /**
	  * An assignment operator Humidity = double.
	  * @param d a value in double precision.
	  */
	 Humidity & operator = (const double d);
	 /**
	  * Operator increment and assign.
	  * @param x a const reference to an Humidity.
	  */
	Humidity & operator += (const Humidity & x);
	/**
	 * Operator decrement and assign.
	 * @param x a const reference to an Humidity.
	 */
	Humidity & operator -= (const Humidity & x);
	/**
	 * Operator multiply and assign.
	 * @param x a value in double precision.
	 */
	Humidity & operator *= (const double x);
	/**
	 * Operator divide and assign.
	 * @param x a valye in double precision.
	 */
	Humidity & operator /= (const double x);
	/**
	 * Addition operator.
	 * @param x a const reference to a Humidity.
	 */
	Humidity operator + (const Humidity & x) const;
	/**
	 * Substraction operator.
	 * @param x a const reference to a Humidity.
	 */
	Humidity operator - (const Humidity & x) const;
	/**
	 * Multiplication operator.
	 * @param x a value in double precision.
	 */
	Humidity operator * (const double x) const;
	/**
	 * Division operator.
	 * @param d a value in double precision.
	 */
	Humidity operator / (const double x) const;
	/**
	 * Comparison operator. Less-than.
	 * @param x a const reference to a Humidity.
	 */
	bool operator < (const Humidity & x) const;
	/**
	 * Comparison operator. Greater-than.
	 * @param x a const reference to a Humidity.
	 */
	bool operator > (const Humidity & x) const;
	/**
	 * Comparison operator. Less-than or equal.
	 * @param x a const reference to a Humidity.
	 */	
	bool operator <= (const Humidity & x) const;
	/**
	 * Comparison operator. Greater-than or equal.
	 * @param x a const reference to a Humidity.
	 */
	bool operator >= (const Humidity & x) const;
	/**
	 * Comparision operator. Equal-to.
	 * @param x a const reference to a Humidity.
	 */
	bool operator == (const Humidity & x) const;
	/** 
	 * Comparison method. Equality.
	 * @param x a const reference to a Humidity.
	 */
	bool equals(const Humidity & x) const;
	/**
	 * Comparison operator. Not-equal.
	 * @param x a const reference to a Humidity.
	 */
	bool operator != (const Humidity & x) const;
	/**
	 * Comparison method. Test nullity.
	 * @return a bool.
	 */
	bool isZero() const;
	/**
	 * Unary operator. Opposite.
	 */
	Humidity operator - () const;
	/**
	 * Unary operator. Unary plus.
	 */
	Humidity operator + () const;
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
	 * Return the double precision value of the Humidity.
	 * @return double
	 */
	double get() const;
#ifndef WITHOUT_ACS
	/**
	 * Return the IDLHumidity representation of the Humidity.
	 * @return IDLHumidity 
	 */
    asdmIDLTypes::IDLHumidity toIDLHumidity() const;
#endif
	/**
	 * Returns the abbreviated name of the unit implicitely associated to any Humidity.
	 * @return string
	 */
	static std::string unit();
private:
	double value;
};
// Humidity constructors
inline Humidity::Humidity() : value(0.0) {
}
inline Humidity::Humidity(const Humidity &t) : value(t.value) {
}
#ifndef WITHOUT_ACS
inline Humidity::Humidity(const asdmIDLTypes::IDLHumidity &l) : value(l.value) {
}
#endif
inline Humidity::Humidity(const std::string &s) : value(fromString(s)) {
}
inline Humidity::Humidity(double v) : value(v) {
}
// Humidity destructor
inline Humidity::~Humidity() { }
// assignment operator
inline Humidity & Humidity::operator = ( const Humidity &t ) {
	value = t.value;
	return *this;
}
// assignment operator
inline Humidity & Humidity::operator = ( const double v ) {
	value = v;
	return *this;
}
// assignment with arithmetic operators
inline Humidity & Humidity::operator += ( const Humidity & t) {
	value += t.value;
	return *this;
}
inline Humidity & Humidity::operator -= ( const Humidity & t) {
	value -= t.value;
	return *this;
}
inline Humidity & Humidity::operator *= ( const double n) {
	value *= n;
	return *this;
}
inline Humidity & Humidity::operator /= ( const double n) {
	value /= n;
	return *this;
}
// arithmetic functions
inline Humidity Humidity::operator + ( const Humidity &t2 ) const {
	Humidity tmp;
	tmp.value = value + t2.value;
	return tmp;
}
inline Humidity Humidity::operator - ( const Humidity &t2 ) const {
	Humidity tmp;
	tmp.value = value - t2.value;
	return tmp;
}
inline Humidity Humidity::operator * ( const double n) const {
	Humidity tmp;
	tmp.value = value * n;
	return tmp;
}
inline Humidity Humidity::operator / ( const double n) const {
	Humidity tmp;
	tmp.value = value / n;
	return tmp;
}
// comparison operators
inline bool Humidity::operator < (const Humidity & x) const {
	return (value < x.value);
}
inline bool Humidity::operator > (const Humidity & x) const {
	return (value > x.value);
}
inline bool Humidity::operator <= (const Humidity & x) const {
	return (value <= x.value);
}
inline bool Humidity::operator >= (const Humidity & x) const {
	return (value >= x.value);
}
inline bool Humidity::equals(const Humidity & x) const {
	return (value == x.value);
}
inline bool Humidity::operator == (const Humidity & x) const {
	return (value == x.value);
}
inline bool Humidity::operator != (const Humidity & x) const {
	return (value != x.value);
}
// unary - and + operators
inline Humidity Humidity::operator - () const {
	Humidity tmp;
        tmp.value = -value;
	return tmp;
}
inline Humidity Humidity::operator + () const {
	Humidity tmp;
    tmp.value = value;
	return tmp;
}
// Conversion functions
inline Humidity::operator std::string () const {
	return toString();
}
inline std::string Humidity::toString() const {
	return toString(value);
}
inline std::string Humidity::toStringI() const {
	return toString(value);
}
inline double Humidity::get() const {
	return value;
}
#ifndef WITHOUT_ACS
inline asdmIDLTypes::IDLHumidity Humidity::toIDLHumidity() const {
    asdmIDLTypes::IDLHumidity tmp;
	tmp.value = value;
	return tmp;
}
#endif
// Friend functions
inline Humidity operator * ( double n, const Humidity &x) {
	Humidity tmp;
	tmp.value = x.value * n;
	return tmp;
}
inline std::ostream & operator << ( std::ostream &o, const Humidity &x ) {
	o << x.value;
	return o;
}
inline std::istream & operator >> ( std::istream &i, Humidity &x ) {
	i >> x.value;
	return i;
}
inline std::string Humidity::unit() {
	return std::string ("%");
}
} // End namespace asdm
#endif /* Humidity_CLASS */
