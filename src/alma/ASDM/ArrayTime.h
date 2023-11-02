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
 * File ArrayTime.h
 */

#ifndef ArrayTime_CLASS
#define ArrayTime_CLASS

#include <stdint.h>
#include <string>

#include <vector>

#include <alma/ASDM/Interval.h> 
#include <alma/ASDM/UTCCorrection.h>

#ifndef WITHOUT_ACS
#include <asdmIDLTypesC.h>
#endif

#include <alma/ASDM/EndianStream.h>

namespace asdm {

/**
 * The ArrayTime class implements the concept of a point in time, implemented
 * as an Interval of time since 17 November 1858 00:00:00 UTC, the beginning of the 
 * modified Julian Day.
 * <p>
 * All dates are assumed to be in the Gregorian calendar, including those 
 * prior to October 15, 1582.  So, if you are interested in very old dates, 
 * this isn't the most convenient class to use. 
 * <p>
 * Internally the time is kept in units of nanoseconds (10<sup>-9</sup> seconds).
 * The base time is 17 November 1858 00:00:00 UTC, and the maximum time is to the
 * year 2151 (2151-02-25T23:47:16.854775807).  This differs from the OMG Time service 
 * The OMG time is in units of 100 nanoseconds using the beginning of the Gregorian
 * calandar,15 October 1582 00:00:00 UTC, as the base time.
 * The reason for this increased accuracy is that the Control system is capable of 
 * measuring time to an accuracy of 40 nanoseconds.  Therefore, by adhering to the 
 * representation of time used in the OMG Time Serivce we would be losing precision.
 * <p>
 * The Time class is an extension of the Interval class, since all times
 * are intervals since 17 November 1858 00:00:00 UTC.
 * <p>
 * All times in this class are assumed to be International 
 * Atomic Time (TAI).  A specific TAI time differs from the corresponding 
 * UTC time by an offset that is an integral number of seconds.  
 * <p>
 * In the methods that give various quantities associated with
 * calendar times, this class does not apply any UTC corrections. 
 * Therefore, if you use these methods to produce calendar times, the 
 * results will differ from civil time by a few seconds.  The classes 
 * UTCTime and LocalTime take the UTC and timezone corrections into 
 * account.
 * <p>
 * The main reference used in crafting these methods is 
 * Astronomical Algorithms by Jean Meeus, second edition,
 * 2000, Willmann-Bell, Inc., ISBN 0-943396-61-1.  See
 * chapter 7, "Julian day", and chapter 12, "Sidereal Time".
 * 
 * @version 1.0 12 January, 2004
 * @author Allen Farris
 * @version 1.1 Aug 8, 2006
 * @author Michel Caillat 
 * added toBin/fromBin methods.
 */
class ArrayTime : public Interval {

public:

	// Useful constants
	const static int numberSigDigitsInASecond 					= 9;
	const static int64_t unitsInASecond 						= 1000000000LL;
	const static int64_t unitsInADayL 						= 86400000000000LL;
	const static double unitsInADay ;	
	const static double unitsInADayDiv100 ;		
	const static double julianDayOfBase ;	
	const static int64_t julianDayOfBaseInUnitsInADayDiv100 	= 2073600432000000000LL;

	static  bool isLeapYear(int year);
	static  double getMJD(double jd);
	static  double getJD(double mjd);
	static  ArrayTime add(const ArrayTime &time, const Interval &interval);
	static  ArrayTime sub(const ArrayTime &time, const Interval &interval) ;
	static  ArrayTime getArrayTime(StringTokenizer &t) ;

	/**
	 * The nullary constructor.
	 *
	 * Builds an ArrayTime corresponding to the origin of time
	 * i. e. 17 November 1858 00:00:00 UTC
	 *
	 */
	ArrayTime();

	/**
	 *
	 * Builds an ArrayTime out of the content of a string.
	 *
	 * If the string contains the textual representation of a double precision value then it will be considered as an MJD date.
	 * If the strign contains the textual representation of an integer value then it will be considered as an interval of time
	 * counted in nanoseconds since the 17 November 1858 00:00:00 UTC.
	 *
	 * @param s a const reference to a string.
	 */
	ArrayTime (const std::string &s); 

	/**
	 * The copy constructor.
	 *
	 * @param s a const reference to an ArrayTime.
	 */
	ArrayTime(const ArrayTime &t);
#ifndef WITHOUT_ACS 
	/**
	 * Builds an ArrayTime from the CORBA/IDL representation of an ArrayTime.
	 *
	 * @param t a const reference to an IDLArrayTime.
	 */
	ArrayTime (const asdmIDLTypes::IDLArrayTime &t);
#endif 

	/**
	 * Builds an ArrayTime from a triple (year, month, day).
	 *
	 * @param year a positive integer value,
	 * @param month an integer in the range [1, 12],
	 * @param day a value in double precision, whose integer value
	 * must be in a valid range depending on the month.
	 *
	 * @throws InvalidArgumentException.
	 */
	ArrayTime(int year, int month, double day); 

	/**
	 * Build an ArrayTime from a sextuple (year, month, day, hour, minute, second.
	 *
	 * @param year a positive integer value,
	 * @param month an integer in the range [1, 12],
	 * @param day an integer in the usual range depending on the month,
	 * @param hour an integer in the range [0, 23],
	 * @param minute an integer value in the range [0, 59],
	 * @param second a value in double precision whose integer part must be in the range [0, 59].
	 *
	 * @throws InvalidArgumentException.
	 */
	ArrayTime(int year, int month, int day, int hour, int minute, double second);

	/**
	 * Build an ArrayTime from an MJD date.
	 *
	 * @param modifieJulianDay a value in double precision.
	 */
	ArrayTime(double modifiedJulianDay);

	/**
	 * Build an ArrayTime from an MJD date.
	 *
	 * @param modifiedJulianDay an integer value to express the integer part of the MJD date,
	 * @param secondsInADay a value in double precision to express the decimal part of the MJD date.
	 */
	ArrayTime(int modifiedJulianDay, double secondsInADay);

	/**
	 * Build an ArrayTime from a number of nanoseconds.
	 *
	 * @param nanoseconds a int64_t integer value to express a number of nanoseconds
	 * since 17 November 1858 00:00:00 UTC.
	 */
	ArrayTime(int64_t nanoseconds);

	/**
	 * Return the julian day.
	 *
	 *  @return a value in double precision expressing this in julian day.
	 */
	double getJD() const; 

	/**
	 * Return the modified julian day.
	 *
	 *  @return a value in double precision expressing this in modified julian day.
	 */
	double getMJD() const; 

/*
	double getJDI() const; 
	double getMJDI() const; 
*/

#ifndef WITHOUT_ACS
    asdmIDLTypes::IDLArrayTime toIDLArrayTime() const; 
#endif
	std::string toFITS() const; 
	
			
	/**
	 * Write the binary representation of this to a EndianOSStream.
	 */		
	void toBin(EndianOSStream& eoss);

	/**
	 * Write the binary representation of a vector of ArrayTime to a EndianOSStream.
	 * @param arrayTime the vector of ArrayTime to be written
	 * @param eoss the EndianOSStream to be written to
	 */
	static void toBin(const std::vector<ArrayTime>& arrayTime,  EndianOSStream& eoss);
	
	/**
	 * Write the binary representation of a vector of vector of ArrayTime to a EndianOSStream.
	 * @param arrayTime the vector of vector of ArrayTime to be written
	 * @param eoss the EndianOSStream to be written to
	 */	
	static void toBin(const std::vector<std::vector<ArrayTime> >& arrayTime,  EndianOSStream& eoss);
	
	/**
	 * Write the binary representation of a vector of vector of vector of ArrayTime to a EndianOSStream.
	 * @param arrayTime the vector of vector of vector of ArrayTime to be written
	 * @param eoss the EndianOSStream to be written to
	 */
	static void toBin(const std::vector<std::vector<std::vector<ArrayTime> > >& arrayTime,  EndianOSStream& eoss);

	/**
	 * Read the binary representation of an ArrayTime from a EndianIStream
	 * and use the read value to set an  ArrayTime.
	 * @param eis the EndianStream to be read
	 * @return an ArrayTime
	 */
	static ArrayTime fromBin(EndianIStream& eis);
	
	/**
	 * Read the binary representation of  a vector of  ArrayTime from an EndianIStream
	 * and use the read value to set a vector of  ArrayTime.
	 * @param dis the EndianIStream to be read
	 * @return a vector of ArrayTime
	 */	 
	 static std::vector<ArrayTime> from1DBin(EndianIStream & eis);
	 
	/**
	 * Read the binary representation of  a vector of vector of ArrayTime from an EndianIStream
	 * and use the read value to set a vector of  vector of ArrayTime.
	 * @param eiis the EndianIStream to be read
	 * @return a vector of vector of ArrayTime
	 */	 
	 static std::vector<std::vector<ArrayTime> > from2DBin(EndianIStream & eis);
	 
	/**
	 * Read the binary representation of  a vector of vector of vector of ArrayTime from an EndianIStream
	 * and use the read value to set a vector of  vector of vector of ArrayTime.
	 * @param eis the EndianIStream to be read
	 * @return a vector of vector of vector of ArrayTime
	 */	 
	 static std::vector<std::vector<std::vector<ArrayTime> > > from3DBin(EndianIStream & eis);	
	 	
	int *getDateTime() const; 
	double getTimeOfDay() const; 
	int getDayOfWeek() const; 
	int getDayOfYear() const; 
	std::string timeOfDayToString() const;
	double getLocalSiderealTime(double longitudeInHours) const; 
	double getGreenwichMeanSiderealTime() const; 

	static double unitToJD(int64_t unit);
	static double unitToMJD(int64_t unit);
	static int64_t jdToUnit(double jd);
	static int64_t mjdToUnit(double mjd);

	static double utcCorrection(double jd); 

private:

	static int64_t init(int year, int month, double day);
	static int64_t init(int year, int month, int day, int hour, int minute, double second);
	int64_t FITSString(std::string t) const;

/*
	static const int numberSigDigitsInASecond;
	static const int64_t unitsInASecond;
	static const int64_t unitsInADayL;
	static const double unitsInADay;
	static const double unitsInADayDiv100;
	static const double julianDayOfBase;
	static const int64_t julianDayOfBaseInUnitsInADayDiv100;
*/
	static const UTCCorrection *UTCCorrectionTable;
	static const UTCCorrection UTCLast;

};

/*
inline double ArrayTime::getJDI() const {return getJD();}
inline double ArrayTime::getMJDI()const  {return getMJD();}
*/
} // End namespace asdm

#endif /* ArrayTime_CLASS */
