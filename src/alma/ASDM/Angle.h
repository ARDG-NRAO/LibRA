//Angle.h generated on 'Thu Feb 04 10:20:05 CET 2010'. Edit at your own risk.
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
 * File Angle.h
 */
#ifndef Angle_CLASS
#define Angle_CLASS
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
class Angle;
Angle operator * ( double , const Angle & );
std::ostream & operator << ( std::ostream &, const Angle & );
std::istream & operator >> ( std::istream &, Angle &);
/**
 * The Angle class implements a quantity of angle in radians..
 * 
 * @version 1.00 Jan. 7, 2005
 * @author Allen Farris
 * 
 * @version 1.1 Aug 8, 2006
 * @author Michel Caillat 
 * added toBin/fromBin methods.
 */
class Angle {
  /**
   * Overloading of multiplication operator.
   * @param d a value in double precision .
   * @param x a const reference to a Angle .
   * @return a Angle 
   */
  friend Angle operator * ( double d, const Angle & x );
  /**
   * Overloading of << to output the value an Angle on an ostream.
   * @param os a reference to the ostream to be written on.
   * @param x a const reference to a Angle.
   */
  friend std::ostream & operator << ( std::ostream & os, const Angle & x);
  /**
   * Overloading of >> to read an Angle from an istream.
   */
  friend std::istream & operator >> ( std::istream & is, Angle & x);
public:
	/**
	 * The nullary constructor (default).
	 */
	Angle();
	/**
	 * The copy constructor.
	 */
	Angle(const Angle &);
	/**
	 * A constructor from a string representation.
	 * The string passed in argument must be parsable into a double precision
	 * number to express the value in radian of the angle.
	 *
	 * @param s a string.
	 */
	Angle(const std::string &s);
#ifndef WITHOUT_ACS
	/**
	 *
	 * A constructor from a CORBA/IDL representation.
	 * 
	 * @param idlAngle a cons ref to an IDLAngle.
	 */
	Angle(const asdmIDLTypes::IDLAngle & idlAngle);
#endif
	/**
	 * A constructor from a value in double precision.
	 * The value passed in argument defines the value of the Angle in radian.
	 */
	Angle(double value);
	/**
	 * The destructor.
	 */
	virtual ~Angle();
	/**
	 * A static method equivalent to the constructor from a string.
	 * @param s a string?.
	 */
	static double fromString(const std::string& s);
	/**
	 * Conversion into string.
	 * The resulting string contains the representation of the value of this Angle.
	 *
	 * @return string
	 */
	static std::string toString(double);
	/**
	 * Parse the next (string) token of a StringTokenizer into an angle.
	 * @param st a reference to a StringTokenizer.
	 * @return an Angle.
	 */
	static Angle getAngle(StringTokenizer &st);
			
	/**
	 * Write the binary representation of this to an EndianOSStream .
	 * @param eoss a reference to an EndianOSStream .
	 */		
	void toBin(EndianOSStream& eoss);
	/**
	 * Write the binary representation of a vector of Angle to a EndianOSStream.
	 * @param angle the vector of Angle to be written
	 * @param eoss the EndianOSStream to be written to
	 */
	static void toBin(const std::vector<Angle>& angle,  EndianOSStream& eoss);
	
	/**
	 * Write the binary representation of a vector of vector of Angle to a EndianOSStream.
	 * @param angle the vector of vector of Angle to be written
	 * @param eoss the EndianOSStream to be written to
	 */	
	static void toBin(const std::vector<std::vector<Angle> >& angle,  EndianOSStream& eoss);
	
	/**
	 * Write the binary representation of a vector of vector of vector of Angle to a EndianOSStream.
	 * @param angle the vector of vector of vector of Angle to be written
	 * @param eoss the EndianOSStream to be written to
	 */
	static void toBin(const std::vector<std::vector<std::vector<Angle> > >& angle,  EndianOSStream& eoss);
	/**
	 * Read the binary representation of an Angle from a EndianIStream
	 * and use the read value to set an  Angle.
	 * @param eis the EndianStream to be read
	 * @return an Angle
	 */
	static Angle fromBin(EndianIStream& eis);
	
	/**
	 * Read the binary representation of  a vector of  Angle from an EndianIStream
	 * and use the read value to set a vector of  Angle.
	 * @param eis a reference to the EndianIStream to be read
	 * @return a vector of Angle
	 */	 
	 static std::vector<Angle> from1DBin(EndianIStream & eis);
	 
	/**
	 * Read the binary representation of  a vector of vector of Angle from an EndianIStream
	 * and use the read value to set a vector of  vector of Angle.
	 * @param eis the EndianIStream to be read
	 * @return a vector of vector of Angle
	 */	 
	 static std::vector<std::vector<Angle> > from2DBin(EndianIStream & eis);
	 
	/**
	 * Read the binary representation of  a vector of vector of vector of Angle from an EndianIStream
	 * and use the read value to set a vector of  vector of vector of Angle.
	 * @param eis the EndianIStream to be read
	 * @return a vector of vector of vector of Angle
	 */	 
	 static std::vector<std::vector<std::vector<Angle> > > from3DBin(EndianIStream & eis);	 
	 
	 /**
	  * An assignment operator Angle = Angle.
	  * @param x a const reference to an Angle.
	  */
	 Angle & operator = (const Angle & x);
	 
	 /**
	  * An assignment operator Angle = double.
	  * @param d a value in double precision.
	  */
	 Angle & operator = (const double d);
	 /**
	  * Operator increment and assign.
	  * @param x a const reference to an Angle.
	  */
	Angle & operator += (const Angle & x);
	/**
	 * Operator decrement and assign.
	 * @param x a const reference to an Angle.
	 */
	Angle & operator -= (const Angle & x);
	/**
	 * Operator multiply and assign.
	 * @param x a value in double precision.
	 */
	Angle & operator *= (const double x);
	/**
	 * Operator divide and assign.
	 * @param x a valye in double precision.
	 */
	Angle & operator /= (const double x);
	/**
	 * Addition operator.
	 * @param x a const reference to a Angle.
	 */
	Angle operator + (const Angle & x) const;
	/**
	 * Substraction operator.
	 * @param x a const reference to a Angle.
	 */
	Angle operator - (const Angle & x) const;
	/**
	 * Multiplication operator.
	 * @param x a value in double precision.
	 */
	Angle operator * (const double x) const;
	/**
	 * Division operator.
	 * @param d a value in double precision.
	 */
	Angle operator / (const double x) const;
	/**
	 * Comparison operator. Less-than.
	 * @param x a const reference to a Angle.
	 */
	bool operator < (const Angle & x) const;
	/**
	 * Comparison operator. Greater-than.
	 * @param x a const reference to a Angle.
	 */
	bool operator > (const Angle & x) const;
	/**
	 * Comparison operator. Less-than or equal.
	 * @param x a const reference to a Angle.
	 */	
	bool operator <= (const Angle & x) const;
	/**
	 * Comparison operator. Greater-than or equal.
	 * @param x a const reference to a Angle.
	 */
	bool operator >= (const Angle & x) const;
	/**
	 * Comparision operator. Equal-to.
	 * @param x a const reference to a Angle.
	 */
	bool operator == (const Angle & x) const;
	/** 
	 * Comparison method. Equality.
	 * @param x a const reference to a Angle.
	 */
	bool equals(const Angle & x) const;
	/**
	 * Comparison operator. Not-equal.
	 * @param x a const reference to a Angle.
	 */
	bool operator != (const Angle & x) const;
	/**
	 * Comparison method. Test nullity.
	 * @return a bool.
	 */
	bool isZero() const;
	/**
	 * Unary operator. Opposite.
	 */
	Angle operator - () const;
	/**
	 * Unary operator. Unary plus.
	 */
	Angle operator + () const;
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
	 * Return the double precision value of the Angle.
	 * @return double
	 */
	double get() const;
#ifndef WITHOUT_ACS
	/**
	 * Return the IDLAngle representation of the Angle.
	 * @return IDLAngle 
	 */
    asdmIDLTypes::IDLAngle toIDLAngle() const;
#endif
	/**
	 * Returns the abbreviated name of the unit implicitely associated to any Angle.
	 * @return string
	 */
	static std::string unit();
private:
	double value;
};
// Angle constructors
inline Angle::Angle() : value(0.0) {
}
inline Angle::Angle(const Angle &t) : value(t.value) {
}
#ifndef WITHOUT_ACS
inline Angle::Angle(const asdmIDLTypes::IDLAngle &l) : value(l.value) {
}
#endif
inline Angle::Angle(const std::string &s) : value(fromString(s)) {
}
inline Angle::Angle(double v) : value(v) {
}
// Angle destructor
inline Angle::~Angle() { }
// assignment operator
inline Angle & Angle::operator = ( const Angle &t ) {
	value = t.value;
	return *this;
}
// assignment operator
inline Angle & Angle::operator = ( const double v ) {
	value = v;
	return *this;
}
// assignment with arithmetic operators
inline Angle & Angle::operator += ( const Angle & t) {
	value += t.value;
	return *this;
}
inline Angle & Angle::operator -= ( const Angle & t) {
	value -= t.value;
	return *this;
}
inline Angle & Angle::operator *= ( const double n) {
	value *= n;
	return *this;
}
inline Angle & Angle::operator /= ( const double n) {
	value /= n;
	return *this;
}
// arithmetic functions
inline Angle Angle::operator + ( const Angle &t2 ) const {
	Angle tmp;
	tmp.value = value + t2.value;
	return tmp;
}
inline Angle Angle::operator - ( const Angle &t2 ) const {
	Angle tmp;
	tmp.value = value - t2.value;
	return tmp;
}
inline Angle Angle::operator * ( const double n) const {
	Angle tmp;
	tmp.value = value * n;
	return tmp;
}
inline Angle Angle::operator / ( const double n) const {
	Angle tmp;
	tmp.value = value / n;
	return tmp;
}
// comparison operators
inline bool Angle::operator < (const Angle & x) const {
	return (value < x.value);
}
inline bool Angle::operator > (const Angle & x) const {
	return (value > x.value);
}
inline bool Angle::operator <= (const Angle & x) const {
	return (value <= x.value);
}
inline bool Angle::operator >= (const Angle & x) const {
	return (value >= x.value);
}
inline bool Angle::equals(const Angle & x) const {
	return (value == x.value);
}
inline bool Angle::operator == (const Angle & x) const {
	return (value == x.value);
}
inline bool Angle::operator != (const Angle & x) const {
	return (value != x.value);
}
// unary - and + operators
inline Angle Angle::operator - () const {
	Angle tmp;
        tmp.value = -value;
	return tmp;
}
inline Angle Angle::operator + () const {
	Angle tmp;
    tmp.value = value;
	return tmp;
}
// Conversion functions
inline Angle::operator std::string () const {
	return toString();
}
inline std::string Angle::toString() const {
	return toString(value);
}
inline std::string Angle::toStringI() const {
	return toString(value);
}
inline double Angle::get() const {
	return value;
}
#ifndef WITHOUT_ACS
inline asdmIDLTypes::IDLAngle Angle::toIDLAngle() const {
    asdmIDLTypes::IDLAngle tmp;
	tmp.value = value;
	return tmp;
}
#endif
// Friend functions
inline Angle operator * ( double n, const Angle &x) {
	Angle tmp;
	tmp.value = x.value * n;
	return tmp;
}
inline std::ostream & operator << ( std::ostream &o, const Angle &x ) {
	o << x.value;
	return o;
}
inline std::istream & operator >> ( std::istream &i, Angle &x ) {
	i >> x.value;
	return i;
}
inline std::string Angle::unit() {
	return std::string ("rad");
}
} // End namespace asdm
#endif /* Angle_CLASS */
