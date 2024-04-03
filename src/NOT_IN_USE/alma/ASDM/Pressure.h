//Pressure.h generated on 'Thu Feb 04 10:20:05 CET 2010'. Edit at your own risk.
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
 * File Pressure.h
 */
#ifndef Pressure_CLASS
#define Pressure_CLASS
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
class Pressure;
Pressure operator * ( double , const Pressure & );
std::ostream & operator << ( std::ostream &, const Pressure & );
std::istream & operator >> ( std::istream &, Pressure &);
/**
 * The Pressure class implements a quantity of pressure in hectopascals..
 * 
 * @version 1.00 Jan. 7, 2005
 * @author Allen Farris
 * 
 * @version 1.1 Aug 8, 2006
 * @author Michel Caillat 
 * added toBin/fromBin methods.
 */
class Pressure {
  /**
   * Overloading of multiplication operator.
   * @param d a value in double precision .
   * @param x a const reference to a Pressure .
   * @return a Pressure 
   */
  friend Pressure operator * ( double d, const Pressure & x );
  /**
   * Overloading of << to output the value an Pressure on an ostream.
   * @param os a reference to the ostream to be written on.
   * @param x a const reference to a Pressure.
   */
  friend std::ostream & operator << ( std::ostream & os, const Pressure & x);
  /**
   * Overloading of >> to read an Pressure from an istream.
   */
  friend std::istream & operator >> ( std::istream & is, Pressure & x);
public:
	/**
	 * The nullary constructor (default).
	 */
	Pressure();
	/**
	 * The copy constructor.
	 */
	Pressure(const Pressure &);
	/**
	 * A constructor from a string representation.
	 * The string passed in argument must be parsable into a double precision
	 * number to express the value in radian of the angle.
	 *
	 * @param s a string.
	 */
	Pressure(const std::string &s);
#ifndef WITHOUT_ACS
	/**
	 *
	 * A constructor from a CORBA/IDL representation.
	 * 
	 * @param idlPressure a cons ref to an IDLPressure.
	 */
	Pressure(const asdmIDLTypes::IDLPressure & idlPressure);
#endif
	/**
	 * A constructor from a value in double precision.
	 * The value passed in argument defines the value of the Pressure in radian.
	 */
	Pressure(double value);
	/**
	 * The destructor.
	 */
	virtual ~Pressure();
	/**
	 * A static method equivalent to the constructor from a string.
	 * @param s a string?.
	 */
	static double fromString(const std::string& s);
	/**
	 * Conversion into string.
	 * The resulting string contains the representation of the value of this Pressure.
	 *
	 * @return string
	 */
	static std::string toString(double);
	/**
	 * Parse the next (string) token of a StringTokenizer into an angle.
	 * @param st a reference to a StringTokenizer.
	 * @return an Pressure.
	 */
	static Pressure getPressure(StringTokenizer &st);
			
	/**
	 * Write the binary representation of this to an EndianOSStream .
	 * @param eoss a reference to an EndianOSStream .
	 */		
	void toBin(EndianOSStream& eoss);
	/**
	 * Write the binary representation of a vector of Pressure to a EndianOSStream.
	 * @param angle the vector of Pressure to be written
	 * @param eoss the EndianOSStream to be written to
	 */
	static void toBin(const std::vector<Pressure>& angle,  EndianOSStream& eoss);
	
	/**
	 * Write the binary representation of a vector of vector of Pressure to a EndianOSStream.
	 * @param angle the vector of vector of Pressure to be written
	 * @param eoss the EndianOSStream to be written to
	 */	
	static void toBin(const std::vector<std::vector<Pressure> >& angle,  EndianOSStream& eoss);
	
	/**
	 * Write the binary representation of a vector of vector of vector of Pressure to a EndianOSStream.
	 * @param angle the vector of vector of vector of Pressure to be written
	 * @param eoss the EndianOSStream to be written to
	 */
	static void toBin(const std::vector<std::vector<std::vector<Pressure> > >& angle,  EndianOSStream& eoss);
	/**
	 * Read the binary representation of an Pressure from a EndianIStream
	 * and use the read value to set an  Pressure.
	 * @param eis the EndianStream to be read
	 * @return an Pressure
	 */
	static Pressure fromBin(EndianIStream& eis);
	
	/**
	 * Read the binary representation of  a vector of  Pressure from an EndianIStream
	 * and use the read value to set a vector of  Pressure.
	 * @param eis a reference to the EndianIStream to be read
	 * @return a vector of Pressure
	 */	 
	 static std::vector<Pressure> from1DBin(EndianIStream & eis);
	 
	/**
	 * Read the binary representation of  a vector of vector of Pressure from an EndianIStream
	 * and use the read value to set a vector of  vector of Pressure.
	 * @param eis the EndianIStream to be read
	 * @return a vector of vector of Pressure
	 */	 
	 static std::vector<std::vector<Pressure> > from2DBin(EndianIStream & eis);
	 
	/**
	 * Read the binary representation of  a vector of vector of vector of Pressure from an EndianIStream
	 * and use the read value to set a vector of  vector of vector of Pressure.
	 * @param eis the EndianIStream to be read
	 * @return a vector of vector of vector of Pressure
	 */	 
	 static std::vector<std::vector<std::vector<Pressure> > > from3DBin(EndianIStream & eis);	 
	 
	 /**
	  * An assignment operator Pressure = Pressure.
	  * @param x a const reference to an Pressure.
	  */
	 Pressure & operator = (const Pressure & x);
	 
	 /**
	  * An assignment operator Pressure = double.
	  * @param d a value in double precision.
	  */
	 Pressure & operator = (const double d);
	 /**
	  * Operator increment and assign.
	  * @param x a const reference to an Pressure.
	  */
	Pressure & operator += (const Pressure & x);
	/**
	 * Operator decrement and assign.
	 * @param x a const reference to an Pressure.
	 */
	Pressure & operator -= (const Pressure & x);
	/**
	 * Operator multiply and assign.
	 * @param x a value in double precision.
	 */
	Pressure & operator *= (const double x);
	/**
	 * Operator divide and assign.
	 * @param x a valye in double precision.
	 */
	Pressure & operator /= (const double x);
	/**
	 * Addition operator.
	 * @param x a const reference to a Pressure.
	 */
	Pressure operator + (const Pressure & x) const;
	/**
	 * Substraction operator.
	 * @param x a const reference to a Pressure.
	 */
	Pressure operator - (const Pressure & x) const;
	/**
	 * Multiplication operator.
	 * @param x a value in double precision.
	 */
	Pressure operator * (const double x) const;
	/**
	 * Division operator.
	 * @param d a value in double precision.
	 */
	Pressure operator / (const double x) const;
	/**
	 * Comparison operator. Less-than.
	 * @param x a const reference to a Pressure.
	 */
	bool operator < (const Pressure & x) const;
	/**
	 * Comparison operator. Greater-than.
	 * @param x a const reference to a Pressure.
	 */
	bool operator > (const Pressure & x) const;
	/**
	 * Comparison operator. Less-than or equal.
	 * @param x a const reference to a Pressure.
	 */	
	bool operator <= (const Pressure & x) const;
	/**
	 * Comparison operator. Greater-than or equal.
	 * @param x a const reference to a Pressure.
	 */
	bool operator >= (const Pressure & x) const;
	/**
	 * Comparision operator. Equal-to.
	 * @param x a const reference to a Pressure.
	 */
	bool operator == (const Pressure & x) const;
	/** 
	 * Comparison method. Equality.
	 * @param x a const reference to a Pressure.
	 */
	bool equals(const Pressure & x) const;
	/**
	 * Comparison operator. Not-equal.
	 * @param x a const reference to a Pressure.
	 */
	bool operator != (const Pressure & x) const;
	/**
	 * Comparison method. Test nullity.
	 * @return a bool.
	 */
	bool isZero() const;
	/**
	 * Unary operator. Opposite.
	 */
	Pressure operator - () const;
	/**
	 * Unary operator. Unary plus.
	 */
	Pressure operator + () const;
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
	 * Return the double precision value of the Pressure.
	 * @return double
	 */
	double get() const;
#ifndef WITHOUT_ACS
	/**
	 * Return the IDLPressure representation of the Pressure.
	 * @return IDLPressure 
	 */
    asdmIDLTypes::IDLPressure toIDLPressure() const;
#endif
	/**
	 * Returns the abbreviated name of the unit implicitely associated to any Pressure.
	 * @return string
	 */
	static std::string unit();
private:
	double value;
};
// Pressure constructors
inline Pressure::Pressure() : value(0.0) {
}
inline Pressure::Pressure(const Pressure &t) : value(t.value) {
}
#ifndef WITHOUT_ACS
inline Pressure::Pressure(const asdmIDLTypes::IDLPressure &l) : value(l.value) {
}
#endif
inline Pressure::Pressure(const std::string &s) : value(fromString(s)) {
}
inline Pressure::Pressure(double v) : value(v) {
}
// Pressure destructor
inline Pressure::~Pressure() { }
// assignment operator
inline Pressure & Pressure::operator = ( const Pressure &t ) {
	value = t.value;
	return *this;
}
// assignment operator
inline Pressure & Pressure::operator = ( const double v ) {
	value = v;
	return *this;
}
// assignment with arithmetic operators
inline Pressure & Pressure::operator += ( const Pressure & t) {
	value += t.value;
	return *this;
}
inline Pressure & Pressure::operator -= ( const Pressure & t) {
	value -= t.value;
	return *this;
}
inline Pressure & Pressure::operator *= ( const double n) {
	value *= n;
	return *this;
}
inline Pressure & Pressure::operator /= ( const double n) {
	value /= n;
	return *this;
}
// arithmetic functions
inline Pressure Pressure::operator + ( const Pressure &t2 ) const {
	Pressure tmp;
	tmp.value = value + t2.value;
	return tmp;
}
inline Pressure Pressure::operator - ( const Pressure &t2 ) const {
	Pressure tmp;
	tmp.value = value - t2.value;
	return tmp;
}
inline Pressure Pressure::operator * ( const double n) const {
	Pressure tmp;
	tmp.value = value * n;
	return tmp;
}
inline Pressure Pressure::operator / ( const double n) const {
	Pressure tmp;
	tmp.value = value / n;
	return tmp;
}
// comparison operators
inline bool Pressure::operator < (const Pressure & x) const {
	return (value < x.value);
}
inline bool Pressure::operator > (const Pressure & x) const {
	return (value > x.value);
}
inline bool Pressure::operator <= (const Pressure & x) const {
	return (value <= x.value);
}
inline bool Pressure::operator >= (const Pressure & x) const {
	return (value >= x.value);
}
inline bool Pressure::equals(const Pressure & x) const {
	return (value == x.value);
}
inline bool Pressure::operator == (const Pressure & x) const {
	return (value == x.value);
}
inline bool Pressure::operator != (const Pressure & x) const {
	return (value != x.value);
}
// unary - and + operators
inline Pressure Pressure::operator - () const {
	Pressure tmp;
        tmp.value = -value;
	return tmp;
}
inline Pressure Pressure::operator + () const {
	Pressure tmp;
    tmp.value = value;
	return tmp;
}
// Conversion functions
inline Pressure::operator std::string () const {
	return toString();
}
inline std::string Pressure::toString() const {
	return toString(value);
}
inline std::string Pressure::toStringI() const {
	return toString(value);
}
inline double Pressure::get() const {
	return value;
}
#ifndef WITHOUT_ACS
inline asdmIDLTypes::IDLPressure Pressure::toIDLPressure() const {
    asdmIDLTypes::IDLPressure tmp;
	tmp.value = value;
	return tmp;
}
#endif
// Friend functions
inline Pressure operator * ( double n, const Pressure &x) {
	Pressure tmp;
	tmp.value = x.value * n;
	return tmp;
}
inline std::ostream & operator << ( std::ostream &o, const Pressure &x ) {
	o << x.value;
	return o;
}
inline std::istream & operator >> ( std::istream &i, Pressure &x ) {
	i >> x.value;
	return i;
}
inline std::string Pressure::unit() {
	return std::string ("hPa");
}
} // End namespace asdm
#endif /* Pressure_CLASS */
