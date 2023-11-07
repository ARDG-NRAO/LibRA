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
 * File ArrayTime.cpp
 */

#include <cmath>
using std::fmod;

#include <alma/ASDM/ArrayTime.h>

#include <alma/ASDM/DoubleWrapper.h>
#include <alma/ASDM/LongWrapper.h>
#include <alma/ASDM/IntegerWrapper.h>
#include <alma/ASDM/InvalidArgumentException.h>
#include <alma/ASDM/NumberFormatException.h>

using namespace std;

namespace asdm {

#define UTCCorrectionLength 32
  static const UTCCorrection xUTCCorrectionTable [UTCCorrectionLength] = {
    // 					JD		   TAI-UTC
    UTCCorrection(2438395.5,   3.2401300),	
    UTCCorrection(2438486.5,   3.3401300),
    UTCCorrection(2438639.5,   3.4401300),
    UTCCorrection(2438761.5,   3.5401300),
    UTCCorrection(2438820.5,   3.6401300),
    UTCCorrection(2438942.5,   3.7401300),
    UTCCorrection(2439004.5,   3.8401300),
    UTCCorrection(2439126.5,   4.3131700),
    UTCCorrection(2439887.5,   4.2131700),
    UTCCorrection(2441317.5,  10.0),
    UTCCorrection(2441499.5,  11.0),
    UTCCorrection(2441683.5,  12.0),
    UTCCorrection(2442048.5,  13.0),
    UTCCorrection(2442413.5,  14.0),
    UTCCorrection(2442778.5,  15.0),
    UTCCorrection(2443144.5,  16.0),
    UTCCorrection(2443509.5,  17.0),
    UTCCorrection(2443874.5,  18.0),
    UTCCorrection(2444239.5,  19.0),
    UTCCorrection(2444786.5,  20.0),
    UTCCorrection(2445151.5,  21.0),
    UTCCorrection(2445516.5,  22.0),
    UTCCorrection(2446247.5,  23.0),
    UTCCorrection(2447161.5,  24.0),
    UTCCorrection(2447892.5,  25.0),
    UTCCorrection(2448257.5,  26.0),
    UTCCorrection(2448804.5,  27.0),
    UTCCorrection(2449169.5,  28.0),
    UTCCorrection(2449534.5,  29.0),
    UTCCorrection(2450083.5,  30.0),
    UTCCorrection(2450630.5,  31.0),
    UTCCorrection(2451179.5,  32.0)
  };

  const UTCCorrection *ArrayTime::UTCCorrectionTable = &xUTCCorrectionTable[0];
  const UTCCorrection ArrayTime::UTCLast = UTCCorrectionTable [UTCCorrectionLength - 1];

  const double ArrayTime::unitsInADay 							= 86400000000000.0;
  const double ArrayTime::unitsInADayDiv100 				= 864000000000.0;
  const double ArrayTime::julianDayOfBase 					= 2400000.5;

  /**
   * Return true if the specified year is a leap year.
   * @param year the year in the Gregorian calendar.
   * @return true if the specified year is a leap year.
   */
  bool ArrayTime::isLeapYear(int year) {
    if (year % 4 != 0)
      return false;
    if (year % 100 == 0 && year % 400 != 0)
      return false;
    return true;
  }

  /**
   * Return the Modified Julian day, given the Julian day.
   * @param jd The Julian day
   * @return The Modified Julian day
   */
  double ArrayTime::getMJD(double jd) {
    return jd - 2400000.5;
  }

  /**
   * Return the Julian day, given the Modified Julian day.
   * @param mjd The modified Julian day
   * @return the Julian day
   */
  double ArrayTime::getJD(double mjd) {
    return mjd + 2400000.5;
  }

  /**
   * Generate a new Time by adding an Interval
   * to the specified Time.
   * @param time The base Time 
   * @param interval The interval to be added to the base time.
   * @return A new Time formed by adding an Interval
   * to the specified Time. 
   */
  ArrayTime ArrayTime::add(const ArrayTime &time, const Interval & /* interval */) {
    ArrayTime t(time);
    //t.add(interval);
    return t;
  }

  /**
   * Generate a new Time by subtracting an Interval
   * from the specified Time.
   * @param time The base Time 
   * @param interval The interval to be subtracted from the base time.
   * @return A new Time formed by subtracting an Interval
   * from the specified Time. 
   */
  ArrayTime ArrayTime::sub(const ArrayTime &time, const Interval & /* interval */) {
    ArrayTime t(time);
    //t.sub(interval);
    return t;
  }

  ArrayTime ArrayTime::getArrayTime(StringTokenizer &t) {
    int64_t value = Long::parseLong(t.nextToken());
    return ArrayTime (value);
  }

  /**
   * Create a default Time, initialized to a value of zero.
   *
   */
  ArrayTime::ArrayTime() : Interval(0LL) {
  }

  /**
   * Create a Time from a string, which can be in one of three
   * formats:
   * <ul>
   * <li>A FITS formatted string,
   * <li>A modified Julian day, or, 
   * <li>An integer representing the number of  
   * nanoseconds since 15 October 1582 00:00:00 UTC.
   * </ul>
   * <p>
   * If the format is a FITS formatted string, its format must be 
   * of the following form:	
   * 			"YYYY-MM-DDThh:mm:ss.ssss"
   * Leading zeros are required if months, days, hours, minutes, or seconds 
   * are single digits.  The value for months ranges from "01" to "12".  
   * The "T" separting the data and time values is optional (which is a 
   * relaxation of the strict FITS standard).  If the "T" is 
   * not present, then a space MUST be present.
   * <p>
   * If the format is a modified Julian day, then the string value 
   * must be in the form of a double which MUST include a decimal point.
   * <p>
   * If the format is an interger, then it MUST represent the number of 
   * nanoseconds since 17 November 1858 00:00:00 UTC, the beginning of the 
   * modified Julian Day. 
   * 
   * @param s The string containing the initial value.
   */
  ArrayTime::ArrayTime (const string &s) {
    int64_t u = 0L;
    if (s.find(':') != string::npos)
      u = FITSString(s);
    else {
      if (s.find('.') != string::npos) {
	u = mjdToUnit(Double::parseDouble(s));
      } else {
	u = Long::parseLong(s);
      }
    }
    *this = u;
  }

  /**
   * Create a Time that is initialized to a specified Time.
   * @param t
   */
  ArrayTime::ArrayTime(const ArrayTime &t) : Interval(t.get()) {
  }

#ifndef WITHOUT_ACS
  /**
   * Create a Time from an IDL time object.
   * @param t The IDL time object.
   */
  ArrayTime::ArrayTime (const asdmIDLTypes::IDLArrayTime &t) {
    *this = t.value;
  }
#endif

  /**
   * Create a Time by specifying the year, month, and day plus the fraction of a day.
   * @param year The yeay
   * @param month The month 
   * @param day the day (and time)
   */
  ArrayTime::ArrayTime(int year, int month, double day) :
    Interval(init(year,month,day)) {
  }

  /**
   * Create a Time by specifying the calendar date and the time.
   * @param year
   * @param month
   * @param day
   * @param hour
   * @param minute
   * @param second
   */
  ArrayTime::ArrayTime(int year, int month, int day, int hour, int minute, double second) : Interval(0L) {
    if (hour < 0 || hour > 23 || minute < 0 || minute > 59 || second < 0.0 || second >= 60.0) {
      throw InvalidArgumentException("Invalid time");
    }
    *this = init(year,month,(double)(day + (((((second / 60.0) + minute) / 60.0) + hour) / 24.0)));
  }

  /**
   * Create a Time by specifying the modified Julian day.
   * @param modifiedJulianDay the modified Julian day, including fractions thereof.
   */
  ArrayTime::ArrayTime(double modifiedJulianDay) : 
    Interval(mjdToUnit(modifiedJulianDay)) {
  }

  /**
   * Create a Time by specifying the modified Julian day plus an additional factor
   * that designates the number of seconds and fractions in a day. 
   * @param modifiedJulianDay the Modified Julian day expressed as an interger
   * @param secondsInADay The number of seconds (with fractions) in this day.
   */
  ArrayTime::ArrayTime(int modifiedJulianDay, double secondsInADay) : 
    Interval(modifiedJulianDay * 8640000000000LL + (long)(secondsInADay * 100000000.0)) {
  }

  /**
   * Create a Time by specifying the number of  
   * nanoseconds since 17 November 1858 00:00:00 UTC, the beginning of the 
   * modified Julian Day.
   * @param tensOfNanoseconds  The number of  
   * nanoseconds since 17 November 1858 00:00:00 UTC, the beginning of the 
   * modified Julian Day.
   */
  ArrayTime::ArrayTime(int64_t nanoseconds) : Interval (nanoseconds) {
  }

  /**
   * Return the Julian day.
   * @return The Julian day as a double.
   */
  double ArrayTime::getJD() const {
    return unitToJD(get());
  }

  /**
   * Return the Modified Julian day.
   * @return The Modified Julian day as a double.
   */
  double ArrayTime::getMJD() const {
    return unitToMJD(get());
  }

#ifndef WITHOUT_ACS
  /**
   * Return an IDL Time object.
   * @return An IDL Time object.
   */
  asdmIDLTypes::IDLArrayTime ArrayTime::toIDLArrayTime() const {
    asdmIDLTypes::IDLArrayTime x;
    x.value = get();
    return x;
  }
#endif

  /**
   * Return this Time as a FITS formatted string, which is of the
   * form "YYYY-MM-DDThh:mm:ss.ssss".
   * @return This Time as a FITS formatted string.
   */
  string ArrayTime::toFITS() const {
    int *unit = getDateTime();
    int yy = unit[0];
    int mm = unit[1];
    int dd = unit[2];
    int hh = unit[3];
    int min = unit[4];
    int sec = unit[5];
    string s = Integer::toString(yy); s += '-';
    if (mm < 10) s += '0';
    s += Integer::toString(mm); s += '-';
    if (dd < 10) s += '0';
    s += Integer::toString(dd); s += 'T';
    if (hh < 10) s += '0';
    s += Integer::toString(hh); s += ':';
    if (min < 10) s += '0';
    s += Integer::toString(min); s += ':';
    if (sec < 10) s += '0';
    s += Integer::toString(sec);
    // apply fractions of a second
    string frac = Integer::toString(unit[6]);
    s += '.';
    // The statement below is sensitive to the number of significant
    // digits in a fraction.  If units are nanoseconds,
    // then we need we will have 9 significant digits in a fraction 
    // string.
    string tmp = "0000000000000000";
    s += tmp.substr(0,numberSigDigitsInASecond - frac.length());
    s += frac;
    delete[] unit;
    return s;
  }

  void ArrayTime::toBin(EndianOSStream& eoss) {
    eoss.writeLongLong(	get());
  }

  void ArrayTime::toBin(const vector<ArrayTime>& arrayTime,  EndianOSStream& eoss) {
    eoss.writeInt((int) arrayTime.size());
    for (unsigned int i = 0; i < arrayTime.size(); i++)
      eoss.writeLongLong(arrayTime.at(i).get());
  }

  void ArrayTime::toBin(const vector<vector<ArrayTime> >& arrayTime,  EndianOSStream& eoss) {
    eoss.writeInt((int) arrayTime.size());
    eoss.writeInt((int) arrayTime.at(0).size());
    for (unsigned int i = 0; i < arrayTime.size(); i++)
      for (unsigned int j = 0; j < arrayTime.at(0).size(); j++)
	eoss.writeLongLong(arrayTime.at(i).at(j).get());
  }

  void ArrayTime::toBin(const vector< vector<vector<ArrayTime> > >& arrayTime,  EndianOSStream& eoss) {
    eoss.writeInt((int) arrayTime.size());
    eoss.writeInt((int) arrayTime.at(0).size());
    eoss.writeInt((int) arrayTime.at(0).at(0).size());	
    for (unsigned int i = 0; i < arrayTime.size(); i++)
      for (unsigned int j = 0; j < arrayTime.at(0).size(); j++)
	for (unsigned int k = 0; k < arrayTime.at(0).at(0).size(); j++)
	  eoss.writeLongLong(arrayTime.at(i).at(j).at(k).get());
  }

  ArrayTime ArrayTime::fromBin(EndianIStream & eis) {
    return ArrayTime(eis.readLongLong());
  }

  vector<ArrayTime> ArrayTime::from1DBin(EndianIStream & eis) {
    int dim1 = eis.readInt();
    vector<ArrayTime> result;
    for (int i = 0; i < dim1; i++)
      result.push_back(ArrayTime(eis.readLongLong()));
    return result;	
  }

  vector<vector<ArrayTime > > ArrayTime::from2DBin(EndianIStream & eis) {
    int dim1 = eis.readInt();
    int dim2 = eis.readInt();
    vector< vector<ArrayTime> >result;
    vector <ArrayTime> aux;
    for (int i = 0; i < dim1; i++) {
      aux.clear();
      for (int j = 0; j < dim2; j++)
	aux.push_back(ArrayTime(eis.readLongLong()));
      result.push_back(aux);
    }
    return result;	
  }

  vector<vector<vector<ArrayTime > > > ArrayTime::from3DBin(EndianIStream & eis) {
    int dim1 = eis.readInt();
    int dim2 = eis.readInt();
    int dim3 = eis.readInt();
    vector<vector< vector<ArrayTime> > >result;
    vector < vector<ArrayTime> >aux1;
    vector <ArrayTime> aux2;
    for (int i = 0; i < dim1; i++) {
      aux1.clear();
      for (int j = 0; j < dim2; j++) {
	aux2.clear();
	for (int k = 0; k < dim3; k++)
	  aux2.push_back(ArrayTime(eis.readLongLong()));
	aux1.push_back(aux2);
      }
      result.push_back(aux1);
    }
    return result;	
  }

  /**
   * Return this time as an array of integers denoting the following:
   * <ul>
   * <li>year,
   * <li>month (varies from 1 to 12),
   * <li>day (varies from 1 to 28, 29, 30, or 31),
   * <li>hour (varies from 0 to 23),
   * <li>minute (varies from 0 to 59),
   * <li>second (varies from 0 to 59), and
   * <li>the number of nanoseconds that remain in this fraction of a second.
   * </ul>
   * @return This time as an array of integers denoting year, month, day, hour, minute
   * second, and fraction of a second.
   */
  int *ArrayTime::getDateTime() const {
    int *n = new int [7];
    int64_t fractionOfADay = get() % unitsInADayL;
    if (fractionOfADay < 0)
      fractionOfADay = unitsInADayL - fractionOfADay;
    int64_t nsec = fractionOfADay / unitsInASecond;
    n[6] = (int)(fractionOfADay - (nsec * 1000000000LL));
    int64_t nmin = nsec / 60L;
    n[5] = (int)(nsec - nmin * 60L);
    int64_t nhr = nmin / 60L;
    n[4] = (int)(nmin - nhr * 60L);
    n[3] = (int)nhr;

    double jd = unitToJD(get());

    // For this algorithm see Meeus, chapter 7, p. 63.
    double x = jd + 0.5; // Make the 12h UT adjustment.
    int Z = (int)x;
    double F = x - Z;
    int A = Z;
    int alpha = 0;
    if (Z >= 2299161) {
      alpha = (int)((Z - 1867216.25) / 36524.25);
      A = Z + 1 + alpha - (int)(alpha / 4);
    }
    int B = A + 1524;
    int C = (int)((B - 122.1) / 365.25);
    int D = (int)(365.25 * C);
    int E = (int)((B - D) / 30.6001);
    double day = B - D - (int)(30.6001 * E) + F;
    int month = (E < 14) ? E - 1 : E - 13;
    int year = (month > 2) ? C - 4716 : C - 4715;

    n[2] = (int)day;
    n[1] = month;
    n[0] = year;

    return n;
  }

  /**
   * Return the time of day in hours and fractions thereof. 
   * @return The time of day in hours.
   */
  double ArrayTime::getTimeOfDay() const {
    double x = unitToJD(get()) + 0.5;
    return (x - (int)x) * 24.0;
  }

  /**
   * Return the day number of the week of this Time.
   * The day numbers are 0-Sunday, 1-Monday, 2-Tuesday,
   * 3-Wednesday, 4-Thursday, 5-Friday, and 6-Saturday.
   * @return The day number of the week of this Time.
   */
  int ArrayTime::getDayOfWeek() const {
    return ((int)(unitToJD(get()) + 1.5)) % 7;
  }

  /**
   * Return the day number of the year of this Time.
   * @return The day number of the year of this Time.
   */
  int ArrayTime::getDayOfYear() const {
    int *n = getDateTime();
    int year = n[0];
    int month = n[1];
    int day = n[2];
    delete n;
    return ((275 * month) / 9) - 
      ((isLeapYear(year) ? 1 : 2) * ((month + 9) / 12)) + 
      day - 30;
  }

  /**
   * Return the time of day as a string, in the form
   * "hh:mm:ss".
   * @return The time of day as a string.
   */
  string ArrayTime::timeOfDayToString() const {
    int *n = getDateTime();
    int hh = n[3];
    int min = n[4];
    int sec = n[5];
    string s = "";
    if (hh < 10) s += '0'; else s += Integer::toString(hh); s += ':';
    if (min < 10) s += '0'; else s += Integer::toString(min); s += ':';
    if (sec < 10) s += '0'; else s += Integer::toString(sec);
    delete n;
    return s;
  }

  /**
   * Return the local sidereal time for this Time
   * in hours and fractions of an hour at the specified longitude.
   * @param longitudeInHours The desired longitude in hours.
   * @return The local sidereal time in hours.
   */
  double ArrayTime::getLocalSiderealTime(double longitudeInHours) const {
    return getGreenwichMeanSiderealTime() - longitudeInHours;
  }

  /**
   * Return the Greenwich mean sidereal time for this Time
   * in hours and fractions of an hour.
   * @return The Greenwich mean sidereal time in hours.
   */
  double ArrayTime::getGreenwichMeanSiderealTime() const {
    double jd = unitToJD(get());
    double t0 = jd - 2451545.0;
    double t = t0 / 36525.0;
    double tt = t * t;
    double x = (280.46061837 + 
		360.98564736629 * t0 + 
		tt * (0.000387933 - (t / 38710000.0))) / 15.0 ;
    double y = fmod(x,24.0);
    if (y < 0)
      y = 24.0 + y;	   
    return y;
  }

  int64_t ArrayTime::init(int year, int month, double day) {
    // For this algorithm see Meeus, chapter 7, p. 61.
    int iday = (int)day;
    if (month < 1 || month > 12)
      throw InvalidArgumentException ("Illegal value of month");
    if ( (iday < 1 || iday > 31) ||
	 ((month == 4 || month == 6 || month == 9 || month == 11) && iday > 30) ||
	 (month == 2 && (iday > ((isLeapYear(year) ? 29 : 28)))) )
      throw InvalidArgumentException ("Illegal value of day");
    if (month <= 2) {
      --year;
      month += 12;
    }
    int A = year / 100;
    int B = 2 - A + (A / 4);
    double jd = (int)(365.25 * (year + 4716)) + (int)(30.6001 * (month + 1)) + iday + B - 1524.5;
    int64_t u = jdToUnit(jd);
    // Now add the fraction of a day.
    u += (int64_t)((day - iday) * unitsInADay + 0.5);
    return u;
  }

  int64_t ArrayTime::init(int year, int month, int day, int hour, int minute, double second) {
    if (hour < 0 || hour > 23 || minute < 0 || minute > 59 || second < 0.0 || second >= 60.0) {
      throw InvalidArgumentException("Invalid time");
    }
    return init(year,month,(double)(day + (((((second / 60.0) + minute) / 60.0) + hour) / 24.0)));
  }

  /**
   * Return a unit of time, as a long, from a FITS-formatted string that 
   * specifies the time.  The format must be of the form:	
   * 			YYYY-MM-DDThh:mm:ss.ssss
   * Leading zeros are required if months, days, hours, minutes, or seconds 
   * are single digits.  The value for months ranges from "01" to "12".  
   * The "T" separting the data and time values is optional.  If the "T" is 
   * not present, then a space MUST be present.
   * 
   * An IllegalArgumentException is thrown is the string is not a valid 
   * time.
   */
  int64_t ArrayTime::FITSString(string t) const {
    if (t.length() < 19 || t.at(4) != '-' || t.at(7) != '-' || 
	(t.at(10) != 'T' && t.at(10) != ' ') || 
	t.at(13) != ':' || t.at(16) != ':')
      throw InvalidArgumentException("Invalid time format: " + t);
    int yyyy = 0;
    int mm = 0;
    int dd = 0;
    int hh = 0;
    int min = 0;
    double sec = 0.0;
    try {
      yyyy = Integer::parseInt(t.substr(0,4));
      mm   = Integer::parseInt(t.substr(5,2));
      dd   = Integer::parseInt(t.substr(8,2));
      hh   = Integer::parseInt(t.substr(11,2));
      min  = Integer::parseInt(t.substr(14,2));
      sec  = Double::parseDouble(t.substr(17,(t.length() - 17)));
    } catch (const NumberFormatException &err) {
      throw InvalidArgumentException("Invalid time format: " + t);
    }
    return init(yyyy,mm,dd,hh,min,sec);
  }

  /**
   * Convert a unit of time in units since the base time to a Julian day.
   * @param unit The unit to be converted.
   * @return The Julian day corresponding to the specified unit of time.
   */
  double ArrayTime::unitToJD(int64_t unit) {
    return (double)(unit) / unitsInADay + julianDayOfBase;
  }

  /**
   * Convert a unit of time in units since the base time to a Modified Julian day.
   * @param unit The unit to be converted.
   * @return The Modified Julian day corresponding to the specified unit of time.
   */
  double ArrayTime::unitToMJD(int64_t unit) {
    return (double)(unit) / unitsInADay;
  }

  /**
   * Convert a Julian day to a unit of time in tens of nanoseconds 
   * since 15 October 1582 00:00:00 UTC.
   * @param jd The Julian day to be converted.
   * @return The unit corresponding to the specified Julian day.
   */
  int64_t ArrayTime::jdToUnit(double jd) {
    return ((int64_t)(jd * unitsInADayDiv100) - julianDayOfBaseInUnitsInADayDiv100) * 100L;
  }

  /**
   * Convert a Modified Julian day to units since the base time.
   * @param mjd The Modified Julian day to be converted.
   * @return The unit corresponding to the specified Modified Julian day.
   */
  int64_t ArrayTime::mjdToUnit(double mjd) {
    return (int64_t)(mjd * unitsInADay);
  }

  /**
   * Return the TAI to UTC correction in units of seconds that must be applied
   * to a specified Julian Day, in other words return TAI time minus UCT time.
   * @param jd The Julian day for the TAI time.
   * @return The number of seconds that must be subtracted from the TAI time to get the 
   * correct UTC time.
   */
  double ArrayTime::utcCorrection(double jd) {
    if (jd > UTCLast.getJD())
      return UTCLast.getTAIMinusUTC();
    int i = UTCCorrectionLength - 1;
    for (; i >= 0; --i) {
      if (jd <= UTCCorrectionTable[i].getJD())
	break;
    }
    return UTCCorrectionTable[i].getTAIMinusUTC();
  }
  /*
  istream & operator >> ( istream &is, ArrayTime& x) {
    Interval::operator >> (is, x);
  }  
  */
} // End namespace asdm
