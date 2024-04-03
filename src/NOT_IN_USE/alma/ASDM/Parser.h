
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
 * Warning!
 *  -------------------------------------------------------------------- 
 * | This is generated code!  Do not modify this file.                  |
 * | If you do, all changes will be lost when the file is re-generated. |
 *  --------------------------------------------------------------------
 *
 * File Parser.h
 */
#ifndef Parser_CLASS
#define Parser_CLASS

#include <stdint.h>
#include <string>
#include <vector>
#include <set>

#include <alma/ASDM/Angle.h>
#include <alma/ASDM/AngularRate.h>
#include <alma/ASDM/ArrayTime.h>
#include <alma/ASDM/ArrayTimeInterval.h>
#include <alma/ASDM/ComplexWrapper.h>
#include <alma/ASDM/Entity.h>
#include <alma/ASDM/EntityId.h>
#include <alma/ASDM/EntityRef.h>
#include <alma/ASDM/Flux.h>
#include <alma/ASDM/Frequency.h>
#include <alma/ASDM/Humidity.h>
#include <alma/ASDM/Interval.h>
#include <alma/ASDM/Length.h>
#include <alma/ASDM/Pressure.h>
#include <alma/ASDM/Speed.h>
#include <alma/ASDM/Tag.h>
#include <alma/ASDM/Temperature.h>

#include <alma/ASDM/StringTokenizer.h>
#include <alma/ASDM/OutOfBoundsException.h>
#include <alma/ASDM/LongWrapper.h>
#include <alma/ASDM/ConversionException.h>

namespace asdm {
/**
 * A Parser of for XML representation of ASDM datasets.
 */
class Parser {

public:

	Parser(const std::string &s);

	/**
	 * Is s in the string being parsed?
	 */
	bool isStr(const std::string &) const;

	/**
	 * Get the portion of the string bounded by s1 and s2, inclusive.
	 * @param s1
	 * @param s2
	 * @return
	 */
	std::string getElement(const std::string &s1, const std::string &s2);

	/**
	 * Get the portion of the string bounded by s1 and s2, exclusive.
	 * @param s1
	 * @param s2
	 * @return
	 */
	std::string getElementContent(const std::string &s1, const std::string &s2);

	std::string getField(const std::string &field);

	static std::string getField(const std::string &xml, const std::string &field);

	// The follwing is a special case.
	static std::string getString(const std::string &name, const std::string &tableName, const std::string &xmlDoc) ;
	//  throw (ConversionException);
	static std::vector<std::string> get1DString(const std::string &name, const std::string &tableName, const std::string &xmlDoc) ;
	//	throw (ConversionException);
	static std::vector<std::vector<std::string> > get2DString(const std::string &name, const std::string &tableName, const std::string &xmlDoc) ;
	//	throw (ConversionException);
	static std::vector<std::vector<std::vector<std::string> > > get3DString(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	//	throw (ConversionException);

	// Generated methods for conversion to and from XML
	// data representations for all types, both primitive
	// and extended.  Also included are 1, 2, and 3 
	// dimensional arrays of these types.

	
	// Field type: int

	static void toXML(int data, const std::string &name, std::string &buf);

	static void toXML(std::vector<int> data, const std::string &name, std::string &buf);

	static void toXML(std::vector<std::vector<int> > data, const std::string &name, std::string &buf);
	
	static void toXML(std::vector<std::vector<std::vector<int> > > data, const std::string &name, std::string &buf);
	
	static void toXML(std::vector<std::vector<std::vector<std::vector<int> > > >data, const std::string &name, std::string &buf);
	
	
	
	static void toXML(std::set< int > data, const std::string &name, std::string &buf);
	

		
	
	
	
	static std::set< int >  getIntSet(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	

			
	static int getInteger(const std::string &name, const std::string &tableName, const std::string &xmlDoc) ;
	// throw (ConversionException);

	static std::vector<int> get1DInteger(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);
		
	static std::vector<std::vector<int> > get2DInteger(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);	
	
	static std::vector<std::vector<std::vector<int> > > get3DInteger(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);	
	
	static std::vector<std::vector<std::vector<std::vector<int> > > >get4DInteger(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);	

		


	// Field type: short

	static void toXML(short data, const std::string &name, std::string &buf);

	static void toXML(std::vector<short> data, const std::string &name, std::string &buf);

	static void toXML(std::vector<std::vector<short> > data, const std::string &name, std::string &buf);
	
	static void toXML(std::vector<std::vector<std::vector<short> > > data, const std::string &name, std::string &buf);
	
	static void toXML(std::vector<std::vector<std::vector<std::vector<short> > > >data, const std::string &name, std::string &buf);
	
	
	

		
	
	
	

			
	static short getShort(const std::string &name, const std::string &tableName, const std::string &xmlDoc) ;
	// throw (ConversionException);

	static std::vector<short> get1DShort(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);
		
	static std::vector<std::vector<short> > get2DShort(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);	
	
	static std::vector<std::vector<std::vector<short> > > get3DShort(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);	
	
	static std::vector<std::vector<std::vector<std::vector<short> > > >get4DShort(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);	

		


	// Field type: int64_t

	static void toXML(int64_t data, const std::string &name, std::string &buf);

	static void toXML(std::vector<int64_t> data, const std::string &name, std::string &buf);

	static void toXML(std::vector<std::vector<int64_t> > data, const std::string &name, std::string &buf);
	
	static void toXML(std::vector<std::vector<std::vector<int64_t> > > data, const std::string &name, std::string &buf);
	
	static void toXML(std::vector<std::vector<std::vector<std::vector<int64_t> > > >data, const std::string &name, std::string &buf);
	
	
	

		
	
	
	

			
	static int64_t getLong(const std::string &name, const std::string &tableName, const std::string &xmlDoc) ;
	// throw (ConversionException);

	static std::vector<int64_t> get1DLong(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);
		
	static std::vector<std::vector<int64_t> > get2DLong(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);	
	
	static std::vector<std::vector<std::vector<int64_t> > > get3DLong(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);	
	
	static std::vector<std::vector<std::vector<std::vector<int64_t> > > >get4DLong(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);	

		


	// Field type: char

	static void toXML(char data, const std::string &name, std::string &buf);

	static void toXML(std::vector<char> data, const std::string &name, std::string &buf);

	static void toXML(std::vector<std::vector<char> > data, const std::string &name, std::string &buf);
	
	static void toXML(std::vector<std::vector<std::vector<char> > > data, const std::string &name, std::string &buf);
	
	static void toXML(std::vector<std::vector<std::vector<std::vector<char> > > >data, const std::string &name, std::string &buf);
	
	
	

		
	
	
	

			
	static char getByte(const std::string &name, const std::string &tableName, const std::string &xmlDoc) ;
	// throw (ConversionException);

	static std::vector<char> get1DByte(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);
		
	static std::vector<std::vector<char> > get2DByte(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);	
	
	static std::vector<std::vector<std::vector<char> > > get3DByte(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);	
	
	static std::vector<std::vector<std::vector<std::vector<char> > > >get4DByte(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);	

		


	// Field type: float

	static void toXML(float data, const std::string &name, std::string &buf);

	static void toXML(std::vector<float> data, const std::string &name, std::string &buf);

	static void toXML(std::vector<std::vector<float> > data, const std::string &name, std::string &buf);
	
	static void toXML(std::vector<std::vector<std::vector<float> > > data, const std::string &name, std::string &buf);
	
	static void toXML(std::vector<std::vector<std::vector<std::vector<float> > > >data, const std::string &name, std::string &buf);
	
	
	

		
	
	
	

			
	static float getFloat(const std::string &name, const std::string &tableName, const std::string &xmlDoc) ;
	// throw (ConversionException);

	static std::vector<float> get1DFloat(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);
		
	static std::vector<std::vector<float> > get2DFloat(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);	
	
	static std::vector<std::vector<std::vector<float> > > get3DFloat(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);	
	
	static std::vector<std::vector<std::vector<std::vector<float> > > >get4DFloat(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);	

		


	// Field type: double

	static void toXML(double data, const std::string &name, std::string &buf);

	static void toXML(std::vector<double> data, const std::string &name, std::string &buf);

	static void toXML(std::vector<std::vector<double> > data, const std::string &name, std::string &buf);
	
	static void toXML(std::vector<std::vector<std::vector<double> > > data, const std::string &name, std::string &buf);
	
	static void toXML(std::vector<std::vector<std::vector<std::vector<double> > > >data, const std::string &name, std::string &buf);
	
	
	

		
	
	
	

			
	static double getDouble(const std::string &name, const std::string &tableName, const std::string &xmlDoc) ;
	// throw (ConversionException);

	static std::vector<double> get1DDouble(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);
		
	static std::vector<std::vector<double> > get2DDouble(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);	
	
	static std::vector<std::vector<std::vector<double> > > get3DDouble(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);	
	
	static std::vector<std::vector<std::vector<std::vector<double> > > >get4DDouble(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);	

		


	// Field type: unsigned char

	static void toXML(unsigned char data, const std::string &name, std::string &buf);

	static void toXML(std::vector<unsigned char> data, const std::string &name, std::string &buf);

	static void toXML(std::vector<std::vector<unsigned char> > data, const std::string &name, std::string &buf);
	
	static void toXML(std::vector<std::vector<std::vector<unsigned char> > > data, const std::string &name, std::string &buf);
	
	static void toXML(std::vector<std::vector<std::vector<std::vector<unsigned char> > > >data, const std::string &name, std::string &buf);
	
	
	

		
	
	
	

			
	static unsigned char getCharacter(const std::string &name, const std::string &tableName, const std::string &xmlDoc) ;
	// throw (ConversionException);

	static std::vector<unsigned char> get1DCharacter(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);
		
	static std::vector<std::vector<unsigned char> > get2DCharacter(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);	
	
	static std::vector<std::vector<std::vector<unsigned char> > > get3DCharacter(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);	
	
	static std::vector<std::vector<std::vector<std::vector<unsigned char> > > >get4DCharacter(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);	

		


	// Field type: bool

	static void toXML(bool data, const std::string &name, std::string &buf);

	static void toXML(std::vector<bool> data, const std::string &name, std::string &buf);

	static void toXML(std::vector<std::vector<bool> > data, const std::string &name, std::string &buf);
	
	static void toXML(std::vector<std::vector<std::vector<bool> > > data, const std::string &name, std::string &buf);
	
	static void toXML(std::vector<std::vector<std::vector<std::vector<bool> > > >data, const std::string &name, std::string &buf);
	
	
	

		
	
	
	

			
	static bool getBoolean(const std::string &name, const std::string &tableName, const std::string &xmlDoc) ;
	// throw (ConversionException);

	static std::vector<bool> get1DBoolean(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);
		
	static std::vector<std::vector<bool> > get2DBoolean(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);	
	
	static std::vector<std::vector<std::vector<bool> > > get3DBoolean(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);	
	
	static std::vector<std::vector<std::vector<std::vector<bool> > > >get4DBoolean(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);	

		


	// Field type: std::string

	static void toXML(std::string data, const std::string &name, std::string &buf);

	static void toXML(std::vector<std::string> data, const std::string &name, std::string &buf);

	static void toXML(std::vector<std::vector<std::string> > data, const std::string &name, std::string &buf);
	
	static void toXML(std::vector<std::vector<std::vector<std::string> > > data, const std::string &name, std::string &buf);
	
	static void toXML(std::vector<std::vector<std::vector<std::vector<std::string> > > >data, const std::string &name, std::string &buf);
	
	
	

		


	// Field type: Angle

	static void toXML(Angle data, const std::string &name, std::string &buf);

	static void toXML(std::vector<Angle> data, const std::string &name, std::string &buf);

	static void toXML(std::vector<std::vector<Angle> > data, const std::string &name, std::string &buf);
	
	static void toXML(std::vector<std::vector<std::vector<Angle> > > data, const std::string &name, std::string &buf);
	
	static void toXML(std::vector<std::vector<std::vector<std::vector<Angle> > > >data, const std::string &name, std::string &buf);
	
	
	

		
	
	
	

			
	static Angle getAngle(const std::string &name, const std::string &tableName, const std::string &xmlDoc) ;
	// throw (ConversionException);

	static std::vector<Angle> get1DAngle(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);
		
	static std::vector<std::vector<Angle> > get2DAngle(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);	
	
	static std::vector<std::vector<std::vector<Angle> > > get3DAngle(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);	
	
	static std::vector<std::vector<std::vector<std::vector<Angle> > > >get4DAngle(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);	

		


	// Field type: AngularRate

	static void toXML(AngularRate data, const std::string &name, std::string &buf);

	static void toXML(std::vector<AngularRate> data, const std::string &name, std::string &buf);

	static void toXML(std::vector<std::vector<AngularRate> > data, const std::string &name, std::string &buf);
	
	static void toXML(std::vector<std::vector<std::vector<AngularRate> > > data, const std::string &name, std::string &buf);
	
	static void toXML(std::vector<std::vector<std::vector<std::vector<AngularRate> > > >data, const std::string &name, std::string &buf);
	
	
	

		
	
	
	

			
	static AngularRate getAngularRate(const std::string &name, const std::string &tableName, const std::string &xmlDoc) ;
	// throw (ConversionException);

	static std::vector<AngularRate> get1DAngularRate(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);
		
	static std::vector<std::vector<AngularRate> > get2DAngularRate(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);	
	
	static std::vector<std::vector<std::vector<AngularRate> > > get3DAngularRate(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);	
	
	static std::vector<std::vector<std::vector<std::vector<AngularRate> > > >get4DAngularRate(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);	

		


	// Field type: ArrayTime

	static void toXML(ArrayTime data, const std::string &name, std::string &buf);

	static void toXML(std::vector<ArrayTime> data, const std::string &name, std::string &buf);

	static void toXML(std::vector<std::vector<ArrayTime> > data, const std::string &name, std::string &buf);
	
	static void toXML(std::vector<std::vector<std::vector<ArrayTime> > > data, const std::string &name, std::string &buf);
	
	static void toXML(std::vector<std::vector<std::vector<std::vector<ArrayTime> > > >data, const std::string &name, std::string &buf);
	
	
	

		
	
	
	

			
	static ArrayTime getArrayTime(const std::string &name, const std::string &tableName, const std::string &xmlDoc) ;
	// throw (ConversionException);

	static std::vector<ArrayTime> get1DArrayTime(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);
		
	static std::vector<std::vector<ArrayTime> > get2DArrayTime(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);	
	
	static std::vector<std::vector<std::vector<ArrayTime> > > get3DArrayTime(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);	
	
	static std::vector<std::vector<std::vector<std::vector<ArrayTime> > > >get4DArrayTime(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);	

		


	// Field type: ArrayTimeInterval

	static void toXML(ArrayTimeInterval data, const std::string &name, std::string &buf);

	static void toXML(std::vector<ArrayTimeInterval> data, const std::string &name, std::string &buf);

	static void toXML(std::vector<std::vector<ArrayTimeInterval> > data, const std::string &name, std::string &buf);
	
	static void toXML(std::vector<std::vector<std::vector<ArrayTimeInterval> > > data, const std::string &name, std::string &buf);
	
	static void toXML(std::vector<std::vector<std::vector<std::vector<ArrayTimeInterval> > > >data, const std::string &name, std::string &buf);
	
	
	

		
	
	
	

			
	static ArrayTimeInterval getArrayTimeInterval(const std::string &name, const std::string &tableName, const std::string &xmlDoc) ;
	// throw (ConversionException);

	static std::vector<ArrayTimeInterval> get1DArrayTimeInterval(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);
		
	static std::vector<std::vector<ArrayTimeInterval> > get2DArrayTimeInterval(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);	
	
	static std::vector<std::vector<std::vector<ArrayTimeInterval> > > get3DArrayTimeInterval(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);	
	
	static std::vector<std::vector<std::vector<std::vector<ArrayTimeInterval> > > >get4DArrayTimeInterval(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);	

		


	// Field type: Complex

	static void toXML(Complex data, const std::string &name, std::string &buf);

	static void toXML(std::vector<Complex> data, const std::string &name, std::string &buf);

	static void toXML(std::vector<std::vector<Complex> > data, const std::string &name, std::string &buf);
	
	static void toXML(std::vector<std::vector<std::vector<Complex> > > data, const std::string &name, std::string &buf);
	
	static void toXML(std::vector<std::vector<std::vector<std::vector<Complex> > > >data, const std::string &name, std::string &buf);
	
	
	

		
	
	
	

			
	static Complex getComplex(const std::string &name, const std::string &tableName, const std::string &xmlDoc) ;
	// throw (ConversionException);

	static std::vector<Complex> get1DComplex(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);
		
	static std::vector<std::vector<Complex> > get2DComplex(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);	
	
	static std::vector<std::vector<std::vector<Complex> > > get3DComplex(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);	
	
	static std::vector<std::vector<std::vector<std::vector<Complex> > > >get4DComplex(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);	

		


	// Field type: Entity

	static void toXML(Entity data, const std::string &name, std::string &buf);

	static void toXML(std::vector<Entity> data, const std::string &name, std::string &buf);

	static void toXML(std::vector<std::vector<Entity> > data, const std::string &name, std::string &buf);
	
	static void toXML(std::vector<std::vector<std::vector<Entity> > > data, const std::string &name, std::string &buf);
	
	static void toXML(std::vector<std::vector<std::vector<std::vector<Entity> > > >data, const std::string &name, std::string &buf);
	
	
	

		
	
	
	

			
	static Entity getEntity(const std::string &name, const std::string &tableName, const std::string &xmlDoc) ;
	// throw (ConversionException);

	static std::vector<Entity> get1DEntity(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);
		
	static std::vector<std::vector<Entity> > get2DEntity(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);	
	
	static std::vector<std::vector<std::vector<Entity> > > get3DEntity(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);	
	
	static std::vector<std::vector<std::vector<std::vector<Entity> > > >get4DEntity(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);	

		


	// Field type: EntityId

	static void toXML(EntityId data, const std::string &name, std::string &buf);

	static void toXML(std::vector<EntityId> data, const std::string &name, std::string &buf);

	static void toXML(std::vector<std::vector<EntityId> > data, const std::string &name, std::string &buf);
	
	static void toXML(std::vector<std::vector<std::vector<EntityId> > > data, const std::string &name, std::string &buf);
	
	static void toXML(std::vector<std::vector<std::vector<std::vector<EntityId> > > >data, const std::string &name, std::string &buf);
	
	
	

		
	
	
	

			
	static EntityId getEntityId(const std::string &name, const std::string &tableName, const std::string &xmlDoc) ;
	// throw (ConversionException);

	static std::vector<EntityId> get1DEntityId(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);
		
	static std::vector<std::vector<EntityId> > get2DEntityId(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);	
	
	static std::vector<std::vector<std::vector<EntityId> > > get3DEntityId(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);	
	
	static std::vector<std::vector<std::vector<std::vector<EntityId> > > >get4DEntityId(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);	

		


	// Field type: EntityRef

	static void toXML(EntityRef data, const std::string &name, std::string &buf);

	static void toXML(std::vector<EntityRef> data, const std::string &name, std::string &buf);

	static void toXML(std::vector<std::vector<EntityRef> > data, const std::string &name, std::string &buf);
	
	static void toXML(std::vector<std::vector<std::vector<EntityRef> > > data, const std::string &name, std::string &buf);
	
	static void toXML(std::vector<std::vector<std::vector<std::vector<EntityRef> > > >data, const std::string &name, std::string &buf);
	
	
	

		
	
	
	

			
	static EntityRef getEntityRef(const std::string &name, const std::string &tableName, const std::string &xmlDoc) ;
	// throw (ConversionException);

	static std::vector<EntityRef> get1DEntityRef(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);
		
	static std::vector<std::vector<EntityRef> > get2DEntityRef(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);	
	
	static std::vector<std::vector<std::vector<EntityRef> > > get3DEntityRef(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);	
	
	static std::vector<std::vector<std::vector<std::vector<EntityRef> > > >get4DEntityRef(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);	

		


	// Field type: Flux

	static void toXML(Flux data, const std::string &name, std::string &buf);

	static void toXML(std::vector<Flux> data, const std::string &name, std::string &buf);

	static void toXML(std::vector<std::vector<Flux> > data, const std::string &name, std::string &buf);
	
	static void toXML(std::vector<std::vector<std::vector<Flux> > > data, const std::string &name, std::string &buf);
	
	static void toXML(std::vector<std::vector<std::vector<std::vector<Flux> > > >data, const std::string &name, std::string &buf);
	
	
	

		
	
	
	

			
	static Flux getFlux(const std::string &name, const std::string &tableName, const std::string &xmlDoc) ;
	// throw (ConversionException);

	static std::vector<Flux> get1DFlux(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);
		
	static std::vector<std::vector<Flux> > get2DFlux(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);	
	
	static std::vector<std::vector<std::vector<Flux> > > get3DFlux(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);	
	
	static std::vector<std::vector<std::vector<std::vector<Flux> > > >get4DFlux(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);	

		


	// Field type: Frequency

	static void toXML(Frequency data, const std::string &name, std::string &buf);

	static void toXML(std::vector<Frequency> data, const std::string &name, std::string &buf);

	static void toXML(std::vector<std::vector<Frequency> > data, const std::string &name, std::string &buf);
	
	static void toXML(std::vector<std::vector<std::vector<Frequency> > > data, const std::string &name, std::string &buf);
	
	static void toXML(std::vector<std::vector<std::vector<std::vector<Frequency> > > >data, const std::string &name, std::string &buf);
	
	
	

		
	
	
	

			
	static Frequency getFrequency(const std::string &name, const std::string &tableName, const std::string &xmlDoc) ;
	// throw (ConversionException);

	static std::vector<Frequency> get1DFrequency(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);
		
	static std::vector<std::vector<Frequency> > get2DFrequency(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);	
	
	static std::vector<std::vector<std::vector<Frequency> > > get3DFrequency(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);	
	
	static std::vector<std::vector<std::vector<std::vector<Frequency> > > >get4DFrequency(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);	

		


	// Field type: Humidity

	static void toXML(Humidity data, const std::string &name, std::string &buf);

	static void toXML(std::vector<Humidity> data, const std::string &name, std::string &buf);

	static void toXML(std::vector<std::vector<Humidity> > data, const std::string &name, std::string &buf);
	
	static void toXML(std::vector<std::vector<std::vector<Humidity> > > data, const std::string &name, std::string &buf);
	
	static void toXML(std::vector<std::vector<std::vector<std::vector<Humidity> > > >data, const std::string &name, std::string &buf);
	
	
	

		
	
	
	

			
	static Humidity getHumidity(const std::string &name, const std::string &tableName, const std::string &xmlDoc) ;
	// throw (ConversionException);

	static std::vector<Humidity> get1DHumidity(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);
		
	static std::vector<std::vector<Humidity> > get2DHumidity(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);	
	
	static std::vector<std::vector<std::vector<Humidity> > > get3DHumidity(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);	
	
	static std::vector<std::vector<std::vector<std::vector<Humidity> > > >get4DHumidity(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);	

		


	// Field type: Interval

	static void toXML(Interval data, const std::string &name, std::string &buf);

	static void toXML(std::vector<Interval> data, const std::string &name, std::string &buf);

	static void toXML(std::vector<std::vector<Interval> > data, const std::string &name, std::string &buf);
	
	static void toXML(std::vector<std::vector<std::vector<Interval> > > data, const std::string &name, std::string &buf);
	
	static void toXML(std::vector<std::vector<std::vector<std::vector<Interval> > > >data, const std::string &name, std::string &buf);
	
	
	

		
	
	
	

			
	static Interval getInterval(const std::string &name, const std::string &tableName, const std::string &xmlDoc) ;
	// throw (ConversionException);

	static std::vector<Interval> get1DInterval(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);
		
	static std::vector<std::vector<Interval> > get2DInterval(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);	
	
	static std::vector<std::vector<std::vector<Interval> > > get3DInterval(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);	
	
	static std::vector<std::vector<std::vector<std::vector<Interval> > > >get4DInterval(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);	

		


	// Field type: Length

	static void toXML(Length data, const std::string &name, std::string &buf);

	static void toXML(std::vector<Length> data, const std::string &name, std::string &buf);

	static void toXML(std::vector<std::vector<Length> > data, const std::string &name, std::string &buf);
	
	static void toXML(std::vector<std::vector<std::vector<Length> > > data, const std::string &name, std::string &buf);
	
	static void toXML(std::vector<std::vector<std::vector<std::vector<Length> > > >data, const std::string &name, std::string &buf);
	
	
	

		
	
	
	

			
	static Length getLength(const std::string &name, const std::string &tableName, const std::string &xmlDoc) ;
	// throw (ConversionException);

	static std::vector<Length> get1DLength(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);
		
	static std::vector<std::vector<Length> > get2DLength(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);	
	
	static std::vector<std::vector<std::vector<Length> > > get3DLength(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);	
	
	static std::vector<std::vector<std::vector<std::vector<Length> > > >get4DLength(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);	

		


	// Field type: Pressure

	static void toXML(Pressure data, const std::string &name, std::string &buf);

	static void toXML(std::vector<Pressure> data, const std::string &name, std::string &buf);

	static void toXML(std::vector<std::vector<Pressure> > data, const std::string &name, std::string &buf);
	
	static void toXML(std::vector<std::vector<std::vector<Pressure> > > data, const std::string &name, std::string &buf);
	
	static void toXML(std::vector<std::vector<std::vector<std::vector<Pressure> > > >data, const std::string &name, std::string &buf);
	
	
	

		
	
	
	

			
	static Pressure getPressure(const std::string &name, const std::string &tableName, const std::string &xmlDoc) ;
	// throw (ConversionException);

	static std::vector<Pressure> get1DPressure(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);
		
	static std::vector<std::vector<Pressure> > get2DPressure(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);	
	
	static std::vector<std::vector<std::vector<Pressure> > > get3DPressure(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);	
	
	static std::vector<std::vector<std::vector<std::vector<Pressure> > > >get4DPressure(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);	

		


	// Field type: Speed

	static void toXML(Speed data, const std::string &name, std::string &buf);

	static void toXML(std::vector<Speed> data, const std::string &name, std::string &buf);

	static void toXML(std::vector<std::vector<Speed> > data, const std::string &name, std::string &buf);
	
	static void toXML(std::vector<std::vector<std::vector<Speed> > > data, const std::string &name, std::string &buf);
	
	static void toXML(std::vector<std::vector<std::vector<std::vector<Speed> > > >data, const std::string &name, std::string &buf);
	
	
	

		
	
	
	

			
	static Speed getSpeed(const std::string &name, const std::string &tableName, const std::string &xmlDoc) ;
	// throw (ConversionException);

	static std::vector<Speed> get1DSpeed(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);
		
	static std::vector<std::vector<Speed> > get2DSpeed(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);	
	
	static std::vector<std::vector<std::vector<Speed> > > get3DSpeed(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);	
	
	static std::vector<std::vector<std::vector<std::vector<Speed> > > >get4DSpeed(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);	

		


	// Field type: Tag

	static void toXML(Tag data, const std::string &name, std::string &buf);

	static void toXML(std::vector<Tag> data, const std::string &name, std::string &buf);

	static void toXML(std::vector<std::vector<Tag> > data, const std::string &name, std::string &buf);
	
	static void toXML(std::vector<std::vector<std::vector<Tag> > > data, const std::string &name, std::string &buf);
	
	static void toXML(std::vector<std::vector<std::vector<std::vector<Tag> > > >data, const std::string &name, std::string &buf);
	
	
	
	static void toXML(std::set< Tag > data, const std::string &name, std::string &buf);
	

		
	
	
	
	static std::set< Tag >  getTagSet(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	

			
	static Tag getTag(const std::string &name, const std::string &tableName, const std::string &xmlDoc) ;
	// throw (ConversionException);

	static std::vector<Tag> get1DTag(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);
		
	static std::vector<std::vector<Tag> > get2DTag(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);	
	
	static std::vector<std::vector<std::vector<Tag> > > get3DTag(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);	
	
	static std::vector<std::vector<std::vector<std::vector<Tag> > > >get4DTag(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);	

		


	// Field type: Temperature

	static void toXML(Temperature data, const std::string &name, std::string &buf);

	static void toXML(std::vector<Temperature> data, const std::string &name, std::string &buf);

	static void toXML(std::vector<std::vector<Temperature> > data, const std::string &name, std::string &buf);
	
	static void toXML(std::vector<std::vector<std::vector<Temperature> > > data, const std::string &name, std::string &buf);
	
	static void toXML(std::vector<std::vector<std::vector<std::vector<Temperature> > > >data, const std::string &name, std::string &buf);
	
	
	

		
	
	
	

			
	static Temperature getTemperature(const std::string &name, const std::string &tableName, const std::string &xmlDoc) ;
	// throw (ConversionException);

	static std::vector<Temperature> get1DTemperature(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);
		
	static std::vector<std::vector<Temperature> > get2DTemperature(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);	
	
	static std::vector<std::vector<std::vector<Temperature> > > get3DTemperature(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);	
	
	static std::vector<std::vector<std::vector<std::vector<Temperature> > > >get4DTemperature(const std::string &name, const std::string &tableName, const std::string &xmlDoc);
	// throw (ConversionException);	

		


	
	// Generated methods for conversion to and from XML
	// data representations with  a Base64 encoded content.
	// The methods are generated only for 1, 2 and 3 dimensional arrays
	// of data whose BasicType have a non null BaseWrapperName.
	// In practice this represents data whose type is one of the basic numeric types
	// or is built upon a basic numeric type. 
	
	
	
	static void toXMLBase64(std::vector<int> data, const std::string &name, std::string &buf);
	static void toXMLBase64(std::vector<std::vector<int> > data, const std::string &name, std::string &buf);
	static void toXMLBase64(std::vector<std::vector<std::vector<int> > > data, const std::string &name, std::string &buf);
	static void toXMLBase64(std::vector<std::vector<std::vector<std::vector<int> > > >data, const std::string &name, std::string &buf);
	
    
	static std::vector<int>& get1DIntegerFromBase64(const std::string &name, const std::string &tableName, const std::string &xmlDoc, std::vector<int>& attribute);
	// throw (ConversionException);
	
	 
	static std::vector<std::vector<int> >& get2DIntegerFromBase64(const std::string &name, const std::string &tableName, const std::string &xmlDoc, std::vector<std::vector<int> >& attribute);
	// throw (ConversionException);
	
 	
	static std::vector<std::vector<std::vector<int> > >& get3DIntegerFromBase64(const std::string &name, const std::string &tableName, const std::string &xmlDoc, std::vector<std::vector<std::vector<int> > >& attribute);
	// throw (ConversionException);
	
	
	static std::vector<std::vector<std::vector<std::vector<int> > > >& get4DIntegerFromBase64(const std::string &name, const std::string &tableName, const std::string &xmlDoc, std::vector<std::vector<std::vector<std::vector<int> > > >& attribute);
	// throw (ConversionException);
	

	
	
	static void toXMLBase64(std::vector<short> data, const std::string &name, std::string &buf);
	static void toXMLBase64(std::vector<std::vector<short> > data, const std::string &name, std::string &buf);
	static void toXMLBase64(std::vector<std::vector<std::vector<short> > > data, const std::string &name, std::string &buf);
	static void toXMLBase64(std::vector<std::vector<std::vector<std::vector<short> > > >data, const std::string &name, std::string &buf);
	
    
	static std::vector<short>& get1DShortFromBase64(const std::string &name, const std::string &tableName, const std::string &xmlDoc, std::vector<short>& attribute);
	// throw (ConversionException);
	
	 
	static std::vector<std::vector<short> >& get2DShortFromBase64(const std::string &name, const std::string &tableName, const std::string &xmlDoc, std::vector<std::vector<short> >& attribute);
	// throw (ConversionException);
	
 	
	static std::vector<std::vector<std::vector<short> > >& get3DShortFromBase64(const std::string &name, const std::string &tableName, const std::string &xmlDoc, std::vector<std::vector<std::vector<short> > >& attribute);
	// throw (ConversionException);
	
	
	static std::vector<std::vector<std::vector<std::vector<short> > > >& get4DShortFromBase64(const std::string &name, const std::string &tableName, const std::string &xmlDoc, std::vector<std::vector<std::vector<std::vector<short> > > >& attribute);
	// throw (ConversionException);
	

	
	
	static void toXMLBase64(std::vector<int64_t> data, const std::string &name, std::string &buf);
	static void toXMLBase64(std::vector<std::vector<int64_t> > data, const std::string &name, std::string &buf);
	static void toXMLBase64(std::vector<std::vector<std::vector<int64_t> > > data, const std::string &name, std::string &buf);
	static void toXMLBase64(std::vector<std::vector<std::vector<std::vector<int64_t> > > >data, const std::string &name, std::string &buf);
	
    
	static std::vector<int64_t>& get1DLongFromBase64(const std::string &name, const std::string &tableName, const std::string &xmlDoc, std::vector<int64_t>& attribute);
	// throw (ConversionException);
	
	 
	static std::vector<std::vector<int64_t> >& get2DLongFromBase64(const std::string &name, const std::string &tableName, const std::string &xmlDoc, std::vector<std::vector<int64_t> >& attribute);
	// throw (ConversionException);
	
 	
	static std::vector<std::vector<std::vector<int64_t> > >& get3DLongFromBase64(const std::string &name, const std::string &tableName, const std::string &xmlDoc, std::vector<std::vector<std::vector<int64_t> > >& attribute);
	// throw (ConversionException);
	
	
	static std::vector<std::vector<std::vector<std::vector<int64_t> > > >& get4DLongFromBase64(const std::string &name, const std::string &tableName, const std::string &xmlDoc, std::vector<std::vector<std::vector<std::vector<int64_t> > > >& attribute);
	// throw (ConversionException);
	

	
	
	static void toXMLBase64(std::vector<char> data, const std::string &name, std::string &buf);
	static void toXMLBase64(std::vector<std::vector<char> > data, const std::string &name, std::string &buf);
	static void toXMLBase64(std::vector<std::vector<std::vector<char> > > data, const std::string &name, std::string &buf);
	static void toXMLBase64(std::vector<std::vector<std::vector<std::vector<char> > > >data, const std::string &name, std::string &buf);
	
    
	static std::vector<char>& get1DByteFromBase64(const std::string &name, const std::string &tableName, const std::string &xmlDoc, std::vector<char>& attribute);
	// throw (ConversionException);
	
	 
	static std::vector<std::vector<char> >& get2DByteFromBase64(const std::string &name, const std::string &tableName, const std::string &xmlDoc, std::vector<std::vector<char> >& attribute);
	// throw (ConversionException);
	
 	
	static std::vector<std::vector<std::vector<char> > >& get3DByteFromBase64(const std::string &name, const std::string &tableName, const std::string &xmlDoc, std::vector<std::vector<std::vector<char> > >& attribute);
	// throw (ConversionException);
	
	
	static std::vector<std::vector<std::vector<std::vector<char> > > >& get4DByteFromBase64(const std::string &name, const std::string &tableName, const std::string &xmlDoc, std::vector<std::vector<std::vector<std::vector<char> > > >& attribute);
	// throw (ConversionException);
	

	
	
	static void toXMLBase64(std::vector<float> data, const std::string &name, std::string &buf);
	static void toXMLBase64(std::vector<std::vector<float> > data, const std::string &name, std::string &buf);
	static void toXMLBase64(std::vector<std::vector<std::vector<float> > > data, const std::string &name, std::string &buf);
	static void toXMLBase64(std::vector<std::vector<std::vector<std::vector<float> > > >data, const std::string &name, std::string &buf);
	
    
	static std::vector<float>& get1DFloatFromBase64(const std::string &name, const std::string &tableName, const std::string &xmlDoc, std::vector<float>& attribute);
	// throw (ConversionException);
	
	 
	static std::vector<std::vector<float> >& get2DFloatFromBase64(const std::string &name, const std::string &tableName, const std::string &xmlDoc, std::vector<std::vector<float> >& attribute);
	// throw (ConversionException);
	
 	
	static std::vector<std::vector<std::vector<float> > >& get3DFloatFromBase64(const std::string &name, const std::string &tableName, const std::string &xmlDoc, std::vector<std::vector<std::vector<float> > >& attribute);
	// throw (ConversionException);
	
	
	static std::vector<std::vector<std::vector<std::vector<float> > > >& get4DFloatFromBase64(const std::string &name, const std::string &tableName, const std::string &xmlDoc, std::vector<std::vector<std::vector<std::vector<float> > > >& attribute);
	// throw (ConversionException);
	

	
	
	static void toXMLBase64(std::vector<double> data, const std::string &name, std::string &buf);
	static void toXMLBase64(std::vector<std::vector<double> > data, const std::string &name, std::string &buf);
	static void toXMLBase64(std::vector<std::vector<std::vector<double> > > data, const std::string &name, std::string &buf);
	static void toXMLBase64(std::vector<std::vector<std::vector<std::vector<double> > > >data, const std::string &name, std::string &buf);
	
    
	static std::vector<double>& get1DDoubleFromBase64(const std::string &name, const std::string &tableName, const std::string &xmlDoc, std::vector<double>& attribute);
	// throw (ConversionException);
	
	 
	static std::vector<std::vector<double> >& get2DDoubleFromBase64(const std::string &name, const std::string &tableName, const std::string &xmlDoc, std::vector<std::vector<double> >& attribute);
	// throw (ConversionException);
	
 	
	static std::vector<std::vector<std::vector<double> > >& get3DDoubleFromBase64(const std::string &name, const std::string &tableName, const std::string &xmlDoc, std::vector<std::vector<std::vector<double> > >& attribute);
	// throw (ConversionException);
	
	
	static std::vector<std::vector<std::vector<std::vector<double> > > >& get4DDoubleFromBase64(const std::string &name, const std::string &tableName, const std::string &xmlDoc, std::vector<std::vector<std::vector<std::vector<double> > > >& attribute);
	// throw (ConversionException);
	

	
	
	static void toXMLBase64(std::vector<unsigned char> data, const std::string &name, std::string &buf);
	static void toXMLBase64(std::vector<std::vector<unsigned char> > data, const std::string &name, std::string &buf);
	static void toXMLBase64(std::vector<std::vector<std::vector<unsigned char> > > data, const std::string &name, std::string &buf);
	static void toXMLBase64(std::vector<std::vector<std::vector<std::vector<unsigned char> > > >data, const std::string &name, std::string &buf);
	
    
	static std::vector<unsigned char>& get1DCharacterFromBase64(const std::string &name, const std::string &tableName, const std::string &xmlDoc, std::vector<unsigned char>& attribute);
	// throw (ConversionException);
	
	 
	static std::vector<std::vector<unsigned char> >& get2DCharacterFromBase64(const std::string &name, const std::string &tableName, const std::string &xmlDoc, std::vector<std::vector<unsigned char> >& attribute);
	// throw (ConversionException);
	
 	
	static std::vector<std::vector<std::vector<unsigned char> > >& get3DCharacterFromBase64(const std::string &name, const std::string &tableName, const std::string &xmlDoc, std::vector<std::vector<std::vector<unsigned char> > >& attribute);
	// throw (ConversionException);
	
	
	static std::vector<std::vector<std::vector<std::vector<unsigned char> > > >& get4DCharacterFromBase64(const std::string &name, const std::string &tableName, const std::string &xmlDoc, std::vector<std::vector<std::vector<std::vector<unsigned char> > > >& attribute);
	// throw (ConversionException);
	

	
	
	static void toXMLBase64(std::vector<bool> data, const std::string &name, std::string &buf);
	static void toXMLBase64(std::vector<std::vector<bool> > data, const std::string &name, std::string &buf);
	static void toXMLBase64(std::vector<std::vector<std::vector<bool> > > data, const std::string &name, std::string &buf);
	static void toXMLBase64(std::vector<std::vector<std::vector<std::vector<bool> > > >data, const std::string &name, std::string &buf);
	
    
	static std::vector<bool>& get1DBooleanFromBase64(const std::string &name, const std::string &tableName, const std::string &xmlDoc, std::vector<bool>& attribute);
	// throw (ConversionException);
	
	 
	static std::vector<std::vector<bool> >& get2DBooleanFromBase64(const std::string &name, const std::string &tableName, const std::string &xmlDoc, std::vector<std::vector<bool> >& attribute);
	// throw (ConversionException);
	
 	
	static std::vector<std::vector<std::vector<bool> > >& get3DBooleanFromBase64(const std::string &name, const std::string &tableName, const std::string &xmlDoc, std::vector<std::vector<std::vector<bool> > >& attribute);
	// throw (ConversionException);
	
	
	static std::vector<std::vector<std::vector<std::vector<bool> > > >& get4DBooleanFromBase64(const std::string &name, const std::string &tableName, const std::string &xmlDoc, std::vector<std::vector<std::vector<std::vector<bool> > > >& attribute);
	// throw (ConversionException);
	

	
	

	
	
	static void toXMLBase64(std::vector<Angle> data, const std::string &name, std::string &buf);
	static void toXMLBase64(std::vector<std::vector<Angle> > data, const std::string &name, std::string &buf);
	static void toXMLBase64(std::vector<std::vector<std::vector<Angle> > > data, const std::string &name, std::string &buf);
	static void toXMLBase64(std::vector<std::vector<std::vector<std::vector<Angle> > > >data, const std::string &name, std::string &buf);
	
    
	static std::vector<Angle>& get1DAngleFromBase64(const std::string &name, const std::string &tableName, const std::string &xmlDoc, std::vector<Angle>& attribute);
	// throw (ConversionException);
	
	 
	static std::vector<std::vector<Angle> >& get2DAngleFromBase64(const std::string &name, const std::string &tableName, const std::string &xmlDoc, std::vector<std::vector<Angle> >& attribute);
	// throw (ConversionException);
	
 	
	static std::vector<std::vector<std::vector<Angle> > >& get3DAngleFromBase64(const std::string &name, const std::string &tableName, const std::string &xmlDoc, std::vector<std::vector<std::vector<Angle> > >& attribute);
	// throw (ConversionException);
	
	
	static std::vector<std::vector<std::vector<std::vector<Angle> > > >& get4DAngleFromBase64(const std::string &name, const std::string &tableName, const std::string &xmlDoc, std::vector<std::vector<std::vector<std::vector<Angle> > > >& attribute);
	// throw (ConversionException);
	

	
	
	static void toXMLBase64(std::vector<AngularRate> data, const std::string &name, std::string &buf);
	static void toXMLBase64(std::vector<std::vector<AngularRate> > data, const std::string &name, std::string &buf);
	static void toXMLBase64(std::vector<std::vector<std::vector<AngularRate> > > data, const std::string &name, std::string &buf);
	static void toXMLBase64(std::vector<std::vector<std::vector<std::vector<AngularRate> > > >data, const std::string &name, std::string &buf);
	
    
	static std::vector<AngularRate>& get1DAngularRateFromBase64(const std::string &name, const std::string &tableName, const std::string &xmlDoc, std::vector<AngularRate>& attribute);
	// throw (ConversionException);
	
	 
	static std::vector<std::vector<AngularRate> >& get2DAngularRateFromBase64(const std::string &name, const std::string &tableName, const std::string &xmlDoc, std::vector<std::vector<AngularRate> >& attribute);
	// throw (ConversionException);
	
 	
	static std::vector<std::vector<std::vector<AngularRate> > >& get3DAngularRateFromBase64(const std::string &name, const std::string &tableName, const std::string &xmlDoc, std::vector<std::vector<std::vector<AngularRate> > >& attribute);
	// throw (ConversionException);
	
	
	static std::vector<std::vector<std::vector<std::vector<AngularRate> > > >& get4DAngularRateFromBase64(const std::string &name, const std::string &tableName, const std::string &xmlDoc, std::vector<std::vector<std::vector<std::vector<AngularRate> > > >& attribute);
	// throw (ConversionException);
	

	
	
	static void toXMLBase64(std::vector<ArrayTime> data, const std::string &name, std::string &buf);
	static void toXMLBase64(std::vector<std::vector<ArrayTime> > data, const std::string &name, std::string &buf);
	static void toXMLBase64(std::vector<std::vector<std::vector<ArrayTime> > > data, const std::string &name, std::string &buf);
	static void toXMLBase64(std::vector<std::vector<std::vector<std::vector<ArrayTime> > > >data, const std::string &name, std::string &buf);
	
    
	static std::vector<ArrayTime>& get1DArrayTimeFromBase64(const std::string &name, const std::string &tableName, const std::string &xmlDoc, std::vector<ArrayTime>& attribute);
	// throw (ConversionException);
	
	 
	static std::vector<std::vector<ArrayTime> >& get2DArrayTimeFromBase64(const std::string &name, const std::string &tableName, const std::string &xmlDoc, std::vector<std::vector<ArrayTime> >& attribute);
	// throw (ConversionException);
	
 	
	static std::vector<std::vector<std::vector<ArrayTime> > >& get3DArrayTimeFromBase64(const std::string &name, const std::string &tableName, const std::string &xmlDoc, std::vector<std::vector<std::vector<ArrayTime> > >& attribute);
	// throw (ConversionException);
	
	
	static std::vector<std::vector<std::vector<std::vector<ArrayTime> > > >& get4DArrayTimeFromBase64(const std::string &name, const std::string &tableName, const std::string &xmlDoc, std::vector<std::vector<std::vector<std::vector<ArrayTime> > > >& attribute);
	// throw (ConversionException);
	

	
	

	
	

	
	

	
	

	
	

	
	
	static void toXMLBase64(std::vector<Flux> data, const std::string &name, std::string &buf);
	static void toXMLBase64(std::vector<std::vector<Flux> > data, const std::string &name, std::string &buf);
	static void toXMLBase64(std::vector<std::vector<std::vector<Flux> > > data, const std::string &name, std::string &buf);
	static void toXMLBase64(std::vector<std::vector<std::vector<std::vector<Flux> > > >data, const std::string &name, std::string &buf);
	
    
	static std::vector<Flux>& get1DFluxFromBase64(const std::string &name, const std::string &tableName, const std::string &xmlDoc, std::vector<Flux>& attribute);
	// throw (ConversionException);
	
	 
	static std::vector<std::vector<Flux> >& get2DFluxFromBase64(const std::string &name, const std::string &tableName, const std::string &xmlDoc, std::vector<std::vector<Flux> >& attribute);
	// throw (ConversionException);
	
 	
	static std::vector<std::vector<std::vector<Flux> > >& get3DFluxFromBase64(const std::string &name, const std::string &tableName, const std::string &xmlDoc, std::vector<std::vector<std::vector<Flux> > >& attribute);
	// throw (ConversionException);
	
	
	static std::vector<std::vector<std::vector<std::vector<Flux> > > >& get4DFluxFromBase64(const std::string &name, const std::string &tableName, const std::string &xmlDoc, std::vector<std::vector<std::vector<std::vector<Flux> > > >& attribute);
	// throw (ConversionException);
	

	
	
	static void toXMLBase64(std::vector<Frequency> data, const std::string &name, std::string &buf);
	static void toXMLBase64(std::vector<std::vector<Frequency> > data, const std::string &name, std::string &buf);
	static void toXMLBase64(std::vector<std::vector<std::vector<Frequency> > > data, const std::string &name, std::string &buf);
	static void toXMLBase64(std::vector<std::vector<std::vector<std::vector<Frequency> > > >data, const std::string &name, std::string &buf);
	
    
	static std::vector<Frequency>& get1DFrequencyFromBase64(const std::string &name, const std::string &tableName, const std::string &xmlDoc, std::vector<Frequency>& attribute);
	// throw (ConversionException);
	
	 
	static std::vector<std::vector<Frequency> >& get2DFrequencyFromBase64(const std::string &name, const std::string &tableName, const std::string &xmlDoc, std::vector<std::vector<Frequency> >& attribute);
	// throw (ConversionException);
	
 	
	static std::vector<std::vector<std::vector<Frequency> > >& get3DFrequencyFromBase64(const std::string &name, const std::string &tableName, const std::string &xmlDoc, std::vector<std::vector<std::vector<Frequency> > >& attribute);
	// throw (ConversionException);
	
	
	static std::vector<std::vector<std::vector<std::vector<Frequency> > > >& get4DFrequencyFromBase64(const std::string &name, const std::string &tableName, const std::string &xmlDoc, std::vector<std::vector<std::vector<std::vector<Frequency> > > >& attribute);
	// throw (ConversionException);
	

	
	
	static void toXMLBase64(std::vector<Humidity> data, const std::string &name, std::string &buf);
	static void toXMLBase64(std::vector<std::vector<Humidity> > data, const std::string &name, std::string &buf);
	static void toXMLBase64(std::vector<std::vector<std::vector<Humidity> > > data, const std::string &name, std::string &buf);
	static void toXMLBase64(std::vector<std::vector<std::vector<std::vector<Humidity> > > >data, const std::string &name, std::string &buf);
	
    
	static std::vector<Humidity>& get1DHumidityFromBase64(const std::string &name, const std::string &tableName, const std::string &xmlDoc, std::vector<Humidity>& attribute);
	// throw (ConversionException);
	
	 
	static std::vector<std::vector<Humidity> >& get2DHumidityFromBase64(const std::string &name, const std::string &tableName, const std::string &xmlDoc, std::vector<std::vector<Humidity> >& attribute);
	// throw (ConversionException);
	
 	
	static std::vector<std::vector<std::vector<Humidity> > >& get3DHumidityFromBase64(const std::string &name, const std::string &tableName, const std::string &xmlDoc, std::vector<std::vector<std::vector<Humidity> > >& attribute);
	// throw (ConversionException);
	
	
	static std::vector<std::vector<std::vector<std::vector<Humidity> > > >& get4DHumidityFromBase64(const std::string &name, const std::string &tableName, const std::string &xmlDoc, std::vector<std::vector<std::vector<std::vector<Humidity> > > >& attribute);
	// throw (ConversionException);
	

	
	
	static void toXMLBase64(std::vector<Interval> data, const std::string &name, std::string &buf);
	static void toXMLBase64(std::vector<std::vector<Interval> > data, const std::string &name, std::string &buf);
	static void toXMLBase64(std::vector<std::vector<std::vector<Interval> > > data, const std::string &name, std::string &buf);
	static void toXMLBase64(std::vector<std::vector<std::vector<std::vector<Interval> > > >data, const std::string &name, std::string &buf);
	
    
	static std::vector<Interval>& get1DIntervalFromBase64(const std::string &name, const std::string &tableName, const std::string &xmlDoc, std::vector<Interval>& attribute);
	// throw (ConversionException);
	
	 
	static std::vector<std::vector<Interval> >& get2DIntervalFromBase64(const std::string &name, const std::string &tableName, const std::string &xmlDoc, std::vector<std::vector<Interval> >& attribute);
	// throw (ConversionException);
	
 	
	static std::vector<std::vector<std::vector<Interval> > >& get3DIntervalFromBase64(const std::string &name, const std::string &tableName, const std::string &xmlDoc, std::vector<std::vector<std::vector<Interval> > >& attribute);
	// throw (ConversionException);
	
	
	static std::vector<std::vector<std::vector<std::vector<Interval> > > >& get4DIntervalFromBase64(const std::string &name, const std::string &tableName, const std::string &xmlDoc, std::vector<std::vector<std::vector<std::vector<Interval> > > >& attribute);
	// throw (ConversionException);
	

	
	
	static void toXMLBase64(std::vector<Length> data, const std::string &name, std::string &buf);
	static void toXMLBase64(std::vector<std::vector<Length> > data, const std::string &name, std::string &buf);
	static void toXMLBase64(std::vector<std::vector<std::vector<Length> > > data, const std::string &name, std::string &buf);
	static void toXMLBase64(std::vector<std::vector<std::vector<std::vector<Length> > > >data, const std::string &name, std::string &buf);
	
    
	static std::vector<Length>& get1DLengthFromBase64(const std::string &name, const std::string &tableName, const std::string &xmlDoc, std::vector<Length>& attribute);
	// throw (ConversionException);
	
	 
	static std::vector<std::vector<Length> >& get2DLengthFromBase64(const std::string &name, const std::string &tableName, const std::string &xmlDoc, std::vector<std::vector<Length> >& attribute);
	// throw (ConversionException);
	
 	
	static std::vector<std::vector<std::vector<Length> > >& get3DLengthFromBase64(const std::string &name, const std::string &tableName, const std::string &xmlDoc, std::vector<std::vector<std::vector<Length> > >& attribute);
	// throw (ConversionException);
	
	
	static std::vector<std::vector<std::vector<std::vector<Length> > > >& get4DLengthFromBase64(const std::string &name, const std::string &tableName, const std::string &xmlDoc, std::vector<std::vector<std::vector<std::vector<Length> > > >& attribute);
	// throw (ConversionException);
	

	
	
	static void toXMLBase64(std::vector<Pressure> data, const std::string &name, std::string &buf);
	static void toXMLBase64(std::vector<std::vector<Pressure> > data, const std::string &name, std::string &buf);
	static void toXMLBase64(std::vector<std::vector<std::vector<Pressure> > > data, const std::string &name, std::string &buf);
	static void toXMLBase64(std::vector<std::vector<std::vector<std::vector<Pressure> > > >data, const std::string &name, std::string &buf);
	
    
	static std::vector<Pressure>& get1DPressureFromBase64(const std::string &name, const std::string &tableName, const std::string &xmlDoc, std::vector<Pressure>& attribute);
	// throw (ConversionException);
	
	 
	static std::vector<std::vector<Pressure> >& get2DPressureFromBase64(const std::string &name, const std::string &tableName, const std::string &xmlDoc, std::vector<std::vector<Pressure> >& attribute);
	// throw (ConversionException);
	
 	
	static std::vector<std::vector<std::vector<Pressure> > >& get3DPressureFromBase64(const std::string &name, const std::string &tableName, const std::string &xmlDoc, std::vector<std::vector<std::vector<Pressure> > >& attribute);
	// throw (ConversionException);
	
	
	static std::vector<std::vector<std::vector<std::vector<Pressure> > > >& get4DPressureFromBase64(const std::string &name, const std::string &tableName, const std::string &xmlDoc, std::vector<std::vector<std::vector<std::vector<Pressure> > > >& attribute);
	// throw (ConversionException);
	

	
	
	static void toXMLBase64(std::vector<Speed> data, const std::string &name, std::string &buf);
	static void toXMLBase64(std::vector<std::vector<Speed> > data, const std::string &name, std::string &buf);
	static void toXMLBase64(std::vector<std::vector<std::vector<Speed> > > data, const std::string &name, std::string &buf);
	static void toXMLBase64(std::vector<std::vector<std::vector<std::vector<Speed> > > >data, const std::string &name, std::string &buf);
	
    
	static std::vector<Speed>& get1DSpeedFromBase64(const std::string &name, const std::string &tableName, const std::string &xmlDoc, std::vector<Speed>& attribute);
	// throw (ConversionException);
	
	 
	static std::vector<std::vector<Speed> >& get2DSpeedFromBase64(const std::string &name, const std::string &tableName, const std::string &xmlDoc, std::vector<std::vector<Speed> >& attribute);
	// throw (ConversionException);
	
 	
	static std::vector<std::vector<std::vector<Speed> > >& get3DSpeedFromBase64(const std::string &name, const std::string &tableName, const std::string &xmlDoc, std::vector<std::vector<std::vector<Speed> > >& attribute);
	// throw (ConversionException);
	
	
	static std::vector<std::vector<std::vector<std::vector<Speed> > > >& get4DSpeedFromBase64(const std::string &name, const std::string &tableName, const std::string &xmlDoc, std::vector<std::vector<std::vector<std::vector<Speed> > > >& attribute);
	// throw (ConversionException);
	

	
	

	
	
	static void toXMLBase64(std::vector<Temperature> data, const std::string &name, std::string &buf);
	static void toXMLBase64(std::vector<std::vector<Temperature> > data, const std::string &name, std::string &buf);
	static void toXMLBase64(std::vector<std::vector<std::vector<Temperature> > > data, const std::string &name, std::string &buf);
	static void toXMLBase64(std::vector<std::vector<std::vector<std::vector<Temperature> > > >data, const std::string &name, std::string &buf);
	
    
	static std::vector<Temperature>& get1DTemperatureFromBase64(const std::string &name, const std::string &tableName, const std::string &xmlDoc, std::vector<Temperature>& attribute);
	// throw (ConversionException);
	
	 
	static std::vector<std::vector<Temperature> >& get2DTemperatureFromBase64(const std::string &name, const std::string &tableName, const std::string &xmlDoc, std::vector<std::vector<Temperature> >& attribute);
	// throw (ConversionException);
	
 	
	static std::vector<std::vector<std::vector<Temperature> > >& get3DTemperatureFromBase64(const std::string &name, const std::string &tableName, const std::string &xmlDoc, std::vector<std::vector<std::vector<Temperature> > >& attribute);
	// throw (ConversionException);
	
	
	static std::vector<std::vector<std::vector<std::vector<Temperature> > > >& get4DTemperatureFromBase64(const std::string &name, const std::string &tableName, const std::string &xmlDoc, std::vector<std::vector<std::vector<std::vector<Temperature> > > >& attribute);
	// throw (ConversionException);
	

	
private:
	std::string str;		// The string being parsed.
	std::string::size_type pos;		// The current position in the string.
	std::string::size_type beg;		// The beginning and end of a fragement
	std::string::size_type end;		//    in the string.
	
public:
	static std::string substring(const std::string &s, int a, int b);
	static std::string trim(const std::string &s);

	// encode special characters for use in XML
	static std::string encode(const std::string &s);
	// decode special characters used in XML
	static std::string decode(const std::string &s, const std::string &tableName);

}; // End class Parser

inline Parser::Parser(const std::string &s) : str(s), pos(0), beg(0), end(0) {
}

inline bool Parser::isStr(const std::string &s) const {
	return str.find(s,pos) == std::string::npos ? false : true;
}

} // End namespace asdm

#endif /* Parser_CLASS */

