//Frequency.h generated on 'Thu Feb 04 10:20:05 CET 2010'. Edit at your own risk.
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
 * File Frequency.h
 */
#ifndef Frequency_CLASS
#define Frequency_CLASS
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
class Frequency;
Frequency operator * ( double , const Frequency & );
std::ostream & operator << ( std::ostream &, const Frequency & );
std::istream & operator >> ( std::istream &, Frequency &);
/**
 * The Frequency class implements a quantity of frequency in hertz..
 * 
 * @version 1.00 Jan. 7, 2005
 * @author Allen Farris
 * 
 * @version 1.1 Aug 8, 2006
 * @author Michel Caillat 
 * added toBin/fromBin methods.
 */
class Frequency {
  /**
   * Overloading of multiplication operator.
   * @param d a value in double precision .
   * @param x a const reference to a Frequency .
   * @return a Frequency 
   */
  friend Frequency operator * ( double d, const Frequency & x );
  /**
   * Overloading of << to output the value an Frequency on an ostream.
   * @param os a reference to the ostream to be written on.
   * @param x a const reference to a Frequency.
   */
  friend std::ostream & operator << ( std::ostream & os, const Frequency & x);
  /**
   * Overloading of >> to read an Frequency from an istream.
   */
  friend std::istream & operator >> ( std::istream & is, Frequency & x);
public:
	/**
	 * The nullary constructor (default).
	 */
	Frequency();
	/**
	 * The copy constructor.
	 */
	Frequency(const Frequency &);
	/**
	 * A constructor from a string representation.
	 * The string passed in argument must be parsable into a double precision
	 * number to express the value in radian of the angle.
	 *
	 * @param s a string.
	 */
	Frequency(const std::string &s);
#ifndef WITHOUT_ACS
	/**
	 *
	 * A constructor from a CORBA/IDL representation.
	 * 
	 * @param idlFrequency a cons ref to an IDLFrequency.
	 */
	Frequency(const asdmIDLTypes::IDLFrequency & idlFrequency);
#endif
	/**
	 * A constructor from a value in double precision.
	 * The value passed in argument defines the value of the Frequency in radian.
	 */
	Frequency(double value);
	/**
	 * The destructor.
	 */
	virtual ~Frequency();
	/**
	 * A static method equivalent to the constructor from a string.
	 * @param s a string?.
	 */
	static double fromString(const std::string& s);
	/**
	 * Conversion into string.
	 * The resulting string contains the representation of the value of this Frequency.
	 *
	 * @return string
	 */
	static std::string toString(double);
	/**
	 * Parse the next (string) token of a StringTokenizer into an angle.
	 * @param st a reference to a StringTokenizer.
	 * @return an Frequency.
	 */
	static Frequency getFrequency(StringTokenizer &st);
			
	/**
	 * Write the binary representation of this to an EndianOSStream .
	 * @param eoss a reference to an EndianOSStream .
	 */		
	void toBin(EndianOSStream& eoss);
	/**
	 * Write the binary representation of a vector of Frequency to a EndianOSStream.
	 * @param angle the vector of Frequency to be written
	 * @param eoss the EndianOSStream to be written to
	 */
	static void toBin(const std::vector<Frequency>& angle,  EndianOSStream& eoss);
	
	/**
	 * Write the binary representation of a vector of vector of Frequency to a EndianOSStream.
	 * @param angle the vector of vector of Frequency to be written
	 * @param eoss the EndianOSStream to be written to
	 */	
	static void toBin(const std::vector<std::vector<Frequency> >& angle,  EndianOSStream& eoss);
	
	/**
	 * Write the binary representation of a vector of vector of vector of Frequency to a EndianOSStream.
	 * @param angle the vector of vector of vector of Frequency to be written
	 * @param eoss the EndianOSStream to be written to
	 */
	static void toBin(const std::vector<std::vector<std::vector<Frequency> > >& angle,  EndianOSStream& eoss);
	/**
	 * Read the binary representation of an Frequency from a EndianIStream
	 * and use the read value to set an  Frequency.
	 * @param eis the EndianStream to be read
	 * @return an Frequency
	 */
	static Frequency fromBin(EndianIStream& eis);
	
	/**
	 * Read the binary representation of  a vector of  Frequency from an EndianIStream
	 * and use the read value to set a vector of  Frequency.
	 * @param eis a reference to the EndianIStream to be read
	 * @return a vector of Frequency
	 */	 
	 static std::vector<Frequency> from1DBin(EndianIStream & eis);
	 
	/**
	 * Read the binary representation of  a vector of vector of Frequency from an EndianIStream
	 * and use the read value to set a vector of  vector of Frequency.
	 * @param eis the EndianIStream to be read
	 * @return a vector of vector of Frequency
	 */	 
	 static std::vector<std::vector<Frequency> > from2DBin(EndianIStream & eis);
	 
	/**
	 * Read the binary representation of  a vector of vector of vector of Frequency from an EndianIStream
	 * and use the read value to set a vector of  vector of vector of Frequency.
	 * @param eis the EndianIStream to be read
	 * @return a vector of vector of vector of Frequency
	 */	 
	 static std::vector<std::vector<std::vector<Frequency> > > from3DBin(EndianIStream & eis);	 
	 
	 /**
	  * An assignment operator Frequency = Frequency.
	  * @param x a const reference to an Frequency.
	  */
	 Frequency & operator = (const Frequency & x);
	 
	 /**
	  * An assignment operator Frequency = double.
	  * @param d a value in double precision.
	  */
	 Frequency & operator = (const double d);
	 /**
	  * Operator increment and assign.
	  * @param x a const reference to an Frequency.
	  */
	Frequency & operator += (const Frequency & x);
	/**
	 * Operator decrement and assign.
	 * @param x a const reference to an Frequency.
	 */
	Frequency & operator -= (const Frequency & x);
	/**
	 * Operator multiply and assign.
	 * @param x a value in double precision.
	 */
	Frequency & operator *= (const double x);
	/**
	 * Operator divide and assign.
	 * @param x a valye in double precision.
	 */
	Frequency & operator /= (const double x);
	/**
	 * Addition operator.
	 * @param x a const reference to a Frequency.
	 */
	Frequency operator + (const Frequency & x) const;
	/**
	 * Substraction operator.
	 * @param x a const reference to a Frequency.
	 */
	Frequency operator - (const Frequency & x) const;
	/**
	 * Multiplication operator.
	 * @param x a value in double precision.
	 */
	Frequency operator * (const double x) const;
	/**
	 * Division operator.
	 * @param d a value in double precision.
	 */
	Frequency operator / (const double x) const;
	/**
	 * Comparison operator. Less-than.
	 * @param x a const reference to a Frequency.
	 */
	bool operator < (const Frequency & x) const;
	/**
	 * Comparison operator. Greater-than.
	 * @param x a const reference to a Frequency.
	 */
	bool operator > (const Frequency & x) const;
	/**
	 * Comparison operator. Less-than or equal.
	 * @param x a const reference to a Frequency.
	 */	
	bool operator <= (const Frequency & x) const;
	/**
	 * Comparison operator. Greater-than or equal.
	 * @param x a const reference to a Frequency.
	 */
	bool operator >= (const Frequency & x) const;
	/**
	 * Comparision operator. Equal-to.
	 * @param x a const reference to a Frequency.
	 */
	bool operator == (const Frequency & x) const;
	/** 
	 * Comparison method. Equality.
	 * @param x a const reference to a Frequency.
	 */
	bool equals(const Frequency & x) const;
	/**
	 * Comparison operator. Not-equal.
	 * @param x a const reference to a Frequency.
	 */
	bool operator != (const Frequency & x) const;
	/**
	 * Comparison method. Test nullity.
	 * @return a bool.
	 */
	bool isZero() const;
	/**
	 * Unary operator. Opposite.
	 */
	Frequency operator - () const;
	/**
	 * Unary operator. Unary plus.
	 */
	Frequency operator + () const;
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
	 * Return the double precision value of the Frequency.
	 * @return double
	 */
	double get() const;
#ifndef WITHOUT_ACS
	/**
	 * Return the IDLFrequency representation of the Frequency.
	 * @return IDLFrequency 
	 */
    asdmIDLTypes::IDLFrequency toIDLFrequency() const;
#endif
	/**
	 * Returns the abbreviated name of the unit implicitely associated to any Frequency.
	 * @return string
	 */
	static std::string unit();
private:
	double value;
};
// Frequency constructors
inline Frequency::Frequency() : value(0.0) {
}
inline Frequency::Frequency(const Frequency &t) : value(t.value) {
}
#ifndef WITHOUT_ACS
inline Frequency::Frequency(const asdmIDLTypes::IDLFrequency &l) : value(l.value) {
}
#endif
inline Frequency::Frequency(const std::string &s) : value(fromString(s)) {
}
inline Frequency::Frequency(double v) : value(v) {
}
// Frequency destructor
inline Frequency::~Frequency() { }
// assignment operator
inline Frequency & Frequency::operator = ( const Frequency &t ) {
	value = t.value;
	return *this;
}
// assignment operator
inline Frequency & Frequency::operator = ( const double v ) {
	value = v;
	return *this;
}
// assignment with arithmetic operators
inline Frequency & Frequency::operator += ( const Frequency & t) {
	value += t.value;
	return *this;
}
inline Frequency & Frequency::operator -= ( const Frequency & t) {
	value -= t.value;
	return *this;
}
inline Frequency & Frequency::operator *= ( const double n) {
	value *= n;
	return *this;
}
inline Frequency & Frequency::operator /= ( const double n) {
	value /= n;
	return *this;
}
// arithmetic functions
inline Frequency Frequency::operator + ( const Frequency &t2 ) const {
	Frequency tmp;
	tmp.value = value + t2.value;
	return tmp;
}
inline Frequency Frequency::operator - ( const Frequency &t2 ) const {
	Frequency tmp;
	tmp.value = value - t2.value;
	return tmp;
}
inline Frequency Frequency::operator * ( const double n) const {
	Frequency tmp;
	tmp.value = value * n;
	return tmp;
}
inline Frequency Frequency::operator / ( const double n) const {
	Frequency tmp;
	tmp.value = value / n;
	return tmp;
}
// comparison operators
inline bool Frequency::operator < (const Frequency & x) const {
	return (value < x.value);
}
inline bool Frequency::operator > (const Frequency & x) const {
	return (value > x.value);
}
inline bool Frequency::operator <= (const Frequency & x) const {
	return (value <= x.value);
}
inline bool Frequency::operator >= (const Frequency & x) const {
	return (value >= x.value);
}
inline bool Frequency::equals(const Frequency & x) const {
	return (value == x.value);
}
inline bool Frequency::operator == (const Frequency & x) const {
	return (value == x.value);
}
inline bool Frequency::operator != (const Frequency & x) const {
	return (value != x.value);
}
// unary - and + operators
inline Frequency Frequency::operator - () const {
	Frequency tmp;
        tmp.value = -value;
	return tmp;
}
inline Frequency Frequency::operator + () const {
	Frequency tmp;
    tmp.value = value;
	return tmp;
}
// Conversion functions
inline Frequency::operator std::string () const {
	return toString();
}
inline std::string Frequency::toString() const {
	return toString(value);
}
inline std::string Frequency::toStringI() const {
	return toString(value);
}
inline double Frequency::get() const {
	return value;
}
#ifndef WITHOUT_ACS
inline asdmIDLTypes::IDLFrequency Frequency::toIDLFrequency() const {
    asdmIDLTypes::IDLFrequency tmp;
	tmp.value = value;
	return tmp;
}
#endif
// Friend functions
inline Frequency operator * ( double n, const Frequency &x) {
	Frequency tmp;
	tmp.value = x.value * n;
	return tmp;
}
inline std::ostream & operator << ( std::ostream &o, const Frequency &x ) {
	o << x.value;
	return o;
}
inline std::istream & operator >> ( std::istream &i, Frequency &x ) {
	i >> x.value;
	return i;
}
inline std::string Frequency::unit() {
	return std::string ("Hz");
}
} // End namespace asdm
#endif /* Frequency_CLASS */
