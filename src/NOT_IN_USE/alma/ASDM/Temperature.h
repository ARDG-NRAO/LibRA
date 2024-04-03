//Temperature.h generated on 'Thu Feb 04 10:20:05 CET 2010'. Edit at your own risk.
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
 * File Temperature.h
 */
#ifndef Temperature_CLASS
#define Temperature_CLASS
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
class Temperature;
Temperature operator * ( double , const Temperature & );
std::ostream & operator << ( std::ostream &, const Temperature & );
std::istream & operator >> ( std::istream &, Temperature &);
/**
 * The Temperature class implements a quantity of temperature in degrees Kelvin..
 * 
 * @version 1.00 Jan. 7, 2005
 * @author Allen Farris
 * 
 * @version 1.1 Aug 8, 2006
 * @author Michel Caillat 
 * added toBin/fromBin methods.
 */
class Temperature {
  /**
   * Overloading of multiplication operator.
   * @param d a value in double precision .
   * @param x a const reference to a Temperature .
   * @return a Temperature 
   */
  friend Temperature operator * ( double d, const Temperature & x );
  /**
   * Overloading of << to output the value an Temperature on an ostream.
   * @param os a reference to the ostream to be written on.
   * @param x a const reference to a Temperature.
   */
  friend std::ostream & operator << ( std::ostream & os, const Temperature & x);
  /**
   * Overloading of >> to read an Temperature from an istream.
   */
  friend std::istream & operator >> ( std::istream & is, Temperature & x);
public:
	/**
	 * The nullary constructor (default).
	 */
	Temperature();
	/**
	 * The copy constructor.
	 */
	Temperature(const Temperature &);
	/**
	 * A constructor from a string representation.
	 * The string passed in argument must be parsable into a double precision
	 * number to express the value in radian of the angle.
	 *
	 * @param s a string.
	 */
	Temperature(const std::string &s);
#ifndef WITHOUT_ACS
	/**
	 *
	 * A constructor from a CORBA/IDL representation.
	 * 
	 * @param idlTemperature a cons ref to an IDLTemperature.
	 */
	Temperature(const asdmIDLTypes::IDLTemperature & idlTemperature);
#endif
	/**
	 * A constructor from a value in double precision.
	 * The value passed in argument defines the value of the Temperature in radian.
	 */
	Temperature(double value);
	/**
	 * The destructor.
	 */
	virtual ~Temperature();
	/**
	 * A static method equivalent to the constructor from a string.
	 * @param s a string?.
	 */
	static double fromString(const std::string& s);
	/**
	 * Conversion into string.
	 * The resulting string contains the representation of the value of this Temperature.
	 *
	 * @return string
	 */
	static std::string toString(double);
	/**
	 * Parse the next (string) token of a StringTokenizer into an angle.
	 * @param st a reference to a StringTokenizer.
	 * @return an Temperature.
	 */
	static Temperature getTemperature(StringTokenizer &st);
			
	/**
	 * Write the binary representation of this to an EndianOSStream .
	 * @param eoss a reference to an EndianOSStream .
	 */		
	void toBin(EndianOSStream& eoss);
	/**
	 * Write the binary representation of a vector of Temperature to a EndianOSStream.
	 * @param angle the vector of Temperature to be written
	 * @param eoss the EndianOSStream to be written to
	 */
	static void toBin(const std::vector<Temperature>& angle,  EndianOSStream& eoss);
	
	/**
	 * Write the binary representation of a vector of vector of Temperature to a EndianOSStream.
	 * @param angle the vector of vector of Temperature to be written
	 * @param eoss the EndianOSStream to be written to
	 */	
	static void toBin(const std::vector<std::vector<Temperature> >& angle,  EndianOSStream& eoss);
	
	/**
	 * Write the binary representation of a vector of vector of vector of Temperature to a EndianOSStream.
	 * @param angle the vector of vector of vector of Temperature to be written
	 * @param eoss the EndianOSStream to be written to
	 */
	static void toBin(const std::vector<std::vector<std::vector<Temperature> > >& angle,  EndianOSStream& eoss);
	/**
	 * Read the binary representation of an Temperature from a EndianIStream
	 * and use the read value to set an  Temperature.
	 * @param eis the EndianStream to be read
	 * @return an Temperature
	 */
	static Temperature fromBin(EndianIStream& eis);
	
	/**
	 * Read the binary representation of  a vector of  Temperature from an EndianIStream
	 * and use the read value to set a vector of  Temperature.
	 * @param eis a reference to the EndianIStream to be read
	 * @return a vector of Temperature
	 */	 
	 static std::vector<Temperature> from1DBin(EndianIStream & eis);
	 
	/**
	 * Read the binary representation of  a vector of vector of Temperature from an EndianIStream
	 * and use the read value to set a vector of  vector of Temperature.
	 * @param eis the EndianIStream to be read
	 * @return a vector of vector of Temperature
	 */	 
	 static std::vector<std::vector<Temperature> > from2DBin(EndianIStream & eis);
	 
	/**
	 * Read the binary representation of  a vector of vector of vector of Temperature from an EndianIStream
	 * and use the read value to set a vector of  vector of vector of Temperature.
	 * @param eis the EndianIStream to be read
	 * @return a vector of vector of vector of Temperature
	 */	 
	 static std::vector<std::vector<std::vector<Temperature> > > from3DBin(EndianIStream & eis);	 
	 
	 /**
	  * An assignment operator Temperature = Temperature.
	  * @param x a const reference to an Temperature.
	  */
	 Temperature & operator = (const Temperature & x);
	 
	 /**
	  * An assignment operator Temperature = double.
	  * @param d a value in double precision.
	  */
	 Temperature & operator = (const double d);
	 /**
	  * Operator increment and assign.
	  * @param x a const reference to an Temperature.
	  */
	Temperature & operator += (const Temperature & x);
	/**
	 * Operator decrement and assign.
	 * @param x a const reference to an Temperature.
	 */
	Temperature & operator -= (const Temperature & x);
	/**
	 * Operator multiply and assign.
	 * @param x a value in double precision.
	 */
	Temperature & operator *= (const double x);
	/**
	 * Operator divide and assign.
	 * @param x a valye in double precision.
	 */
	Temperature & operator /= (const double x);
	/**
	 * Addition operator.
	 * @param x a const reference to a Temperature.
	 */
	Temperature operator + (const Temperature & x) const;
	/**
	 * Substraction operator.
	 * @param x a const reference to a Temperature.
	 */
	Temperature operator - (const Temperature & x) const;
	/**
	 * Multiplication operator.
	 * @param x a value in double precision.
	 */
	Temperature operator * (const double x) const;
	/**
	 * Division operator.
	 * @param d a value in double precision.
	 */
	Temperature operator / (const double x) const;
	/**
	 * Comparison operator. Less-than.
	 * @param x a const reference to a Temperature.
	 */
	bool operator < (const Temperature & x) const;
	/**
	 * Comparison operator. Greater-than.
	 * @param x a const reference to a Temperature.
	 */
	bool operator > (const Temperature & x) const;
	/**
	 * Comparison operator. Less-than or equal.
	 * @param x a const reference to a Temperature.
	 */	
	bool operator <= (const Temperature & x) const;
	/**
	 * Comparison operator. Greater-than or equal.
	 * @param x a const reference to a Temperature.
	 */
	bool operator >= (const Temperature & x) const;
	/**
	 * Comparision operator. Equal-to.
	 * @param x a const reference to a Temperature.
	 */
	bool operator == (const Temperature & x) const;
	/** 
	 * Comparison method. Equality.
	 * @param x a const reference to a Temperature.
	 */
	bool equals(const Temperature & x) const;
	/**
	 * Comparison operator. Not-equal.
	 * @param x a const reference to a Temperature.
	 */
	bool operator != (const Temperature & x) const;
	/**
	 * Comparison method. Test nullity.
	 * @return a bool.
	 */
	bool isZero() const;
	/**
	 * Unary operator. Opposite.
	 */
	Temperature operator - () const;
	/**
	 * Unary operator. Unary plus.
	 */
	Temperature operator + () const;
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
	 * Return the double precision value of the Temperature.
	 * @return double
	 */
	double get() const;
#ifndef WITHOUT_ACS
	/**
	 * Return the IDLTemperature representation of the Temperature.
	 * @return IDLTemperature 
	 */
    asdmIDLTypes::IDLTemperature toIDLTemperature() const;
#endif
	/**
	 * Returns the abbreviated name of the unit implicitely associated to any Temperature.
	 * @return string
	 */
	static std::string unit();
private:
	double value;
};
// Temperature constructors
inline Temperature::Temperature() : value(0.0) {
}
inline Temperature::Temperature(const Temperature &t) : value(t.value) {
}
#ifndef WITHOUT_ACS
inline Temperature::Temperature(const asdmIDLTypes::IDLTemperature &l) : value(l.value) {
}
#endif
inline Temperature::Temperature(const std::string &s) : value(fromString(s)) {
}
inline Temperature::Temperature(double v) : value(v) {
}
// Temperature destructor
inline Temperature::~Temperature() { }
// assignment operator
inline Temperature & Temperature::operator = ( const Temperature &t ) {
	value = t.value;
	return *this;
}
// assignment operator
inline Temperature & Temperature::operator = ( const double v ) {
	value = v;
	return *this;
}
// assignment with arithmetic operators
inline Temperature & Temperature::operator += ( const Temperature & t) {
	value += t.value;
	return *this;
}
inline Temperature & Temperature::operator -= ( const Temperature & t) {
	value -= t.value;
	return *this;
}
inline Temperature & Temperature::operator *= ( const double n) {
	value *= n;
	return *this;
}
inline Temperature & Temperature::operator /= ( const double n) {
	value /= n;
	return *this;
}
// arithmetic functions
inline Temperature Temperature::operator + ( const Temperature &t2 ) const {
	Temperature tmp;
	tmp.value = value + t2.value;
	return tmp;
}
inline Temperature Temperature::operator - ( const Temperature &t2 ) const {
	Temperature tmp;
	tmp.value = value - t2.value;
	return tmp;
}
inline Temperature Temperature::operator * ( const double n) const {
	Temperature tmp;
	tmp.value = value * n;
	return tmp;
}
inline Temperature Temperature::operator / ( const double n) const {
	Temperature tmp;
	tmp.value = value / n;
	return tmp;
}
// comparison operators
inline bool Temperature::operator < (const Temperature & x) const {
	return (value < x.value);
}
inline bool Temperature::operator > (const Temperature & x) const {
	return (value > x.value);
}
inline bool Temperature::operator <= (const Temperature & x) const {
	return (value <= x.value);
}
inline bool Temperature::operator >= (const Temperature & x) const {
	return (value >= x.value);
}
inline bool Temperature::equals(const Temperature & x) const {
	return (value == x.value);
}
inline bool Temperature::operator == (const Temperature & x) const {
	return (value == x.value);
}
inline bool Temperature::operator != (const Temperature & x) const {
	return (value != x.value);
}
// unary - and + operators
inline Temperature Temperature::operator - () const {
	Temperature tmp;
        tmp.value = -value;
	return tmp;
}
inline Temperature Temperature::operator + () const {
	Temperature tmp;
    tmp.value = value;
	return tmp;
}
// Conversion functions
inline Temperature::operator std::string () const {
	return toString();
}
inline std::string Temperature::toString() const {
	return toString(value);
}
inline std::string Temperature::toStringI() const {
	return toString(value);
}
inline double Temperature::get() const {
	return value;
}
#ifndef WITHOUT_ACS
inline asdmIDLTypes::IDLTemperature Temperature::toIDLTemperature() const {
    asdmIDLTypes::IDLTemperature tmp;
	tmp.value = value;
	return tmp;
}
#endif
// Friend functions
inline Temperature operator * ( double n, const Temperature &x) {
	Temperature tmp;
	tmp.value = x.value * n;
	return tmp;
}
inline std::ostream & operator << ( std::ostream &o, const Temperature &x ) {
	o << x.value;
	return o;
}
inline std::istream & operator >> ( std::istream &i, Temperature &x ) {
	i >> x.value;
	return i;
}
inline std::string Temperature::unit() {
	return std::string ("K");
}
} // End namespace asdm
#endif /* Temperature_CLASS */
