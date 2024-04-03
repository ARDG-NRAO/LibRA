
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
 * File EphemerisRow.h
 */
 
#ifndef EphemerisRow_CLASS
#define EphemerisRow_CLASS

#include <vector>
#include <string>
#include <set>

#ifndef WITHOUT_ACS
#include <asdmIDLC.h>
#endif






	 
#include <alma/ASDM/ArrayTimeInterval.h>
	

	 
#include <alma/ASDM/ArrayTime.h>
	




	

	

	

	

	

	

	

	

	

	

	

	



#include <alma/ASDM/ConversionException.h>
#include <alma/ASDM/NoSuchRow.h>
#include <alma/ASDM/IllegalAccessException.h>

#include <alma/ASDM/RowTransformer.h>
//#include <alma/ASDM/TableStreamReader.h>

/*\file Ephemeris.h
    \brief Generated from model's revision "-1", branch ""
*/

namespace asdm {

//class asdm::EphemerisTable;

	

class EphemerisRow;
typedef void (EphemerisRow::*EphemerisAttributeFromBin) (EndianIStream& eis);
typedef void (EphemerisRow::*EphemerisAttributeFromText) (const std::string& s);

/**
 * The EphemerisRow class is a row of a EphemerisTable.
 * 
 * Generated from model's revision "-1", branch ""
 *
 */
class EphemerisRow {
friend class asdm::EphemerisTable;
friend class asdm::RowTransformer<EphemerisRow>;
//friend class asdm::TableStreamReader<EphemerisTable, EphemerisRow>;

public:

	virtual ~EphemerisRow();

	/**
	 * Return the table to which this row belongs.
	 */
	EphemerisTable &getTable() const;
	
	/**
	 * Has this row been added to its table ?
	 * @return true if and only if it has been added.
	 */
	bool isAdded() const;
		
	////////////////////////////////
	// Intrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute timeInterval
	
	
	

	
 	/**
 	 * Get timeInterval.
 	 * @return timeInterval as ArrayTimeInterval
 	 */
 	ArrayTimeInterval getTimeInterval() const;
	
 
 	
 	
 	/**
 	 * Set timeInterval with the specified ArrayTimeInterval.
 	 * @param timeInterval The ArrayTimeInterval value to which timeInterval is to be set.
 	 
 		
 			
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 		
 	 */
 	void setTimeInterval (ArrayTimeInterval timeInterval);
  		
	
	
	


	
	// ===> Attribute ephemerisId
	
	
	

	
 	/**
 	 * Get ephemerisId.
 	 * @return ephemerisId as int
 	 */
 	int getEphemerisId() const;
	
 
 	
 	
 	/**
 	 * Set ephemerisId with the specified int.
 	 * @param ephemerisId The int value to which ephemerisId is to be set.
 	 
 		
 			
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 		
 	 */
 	void setEphemerisId (int ephemerisId);
  		
	
	
	


	
	// ===> Attribute observerLocation
	
	
	

	
 	/**
 	 * Get observerLocation.
 	 * @return observerLocation as std::vector<double >
 	 */
 	std::vector<double > getObserverLocation() const;
	
 
 	
 	
 	/**
 	 * Set observerLocation with the specified std::vector<double >.
 	 * @param observerLocation The std::vector<double > value to which observerLocation is to be set.
 	 
 		
 			
 	 */
 	void setObserverLocation (std::vector<double > observerLocation);
  		
	
	
	


	
	// ===> Attribute equinoxEquator
	
	
	

	
 	/**
 	 * Get equinoxEquator.
 	 * @return equinoxEquator as double
 	 */
 	double getEquinoxEquator() const;
	
 
 	
 	
 	/**
 	 * Set equinoxEquator with the specified double.
 	 * @param equinoxEquator The double value to which equinoxEquator is to be set.
 	 
 		
 			
 	 */
 	void setEquinoxEquator (double equinoxEquator);
  		
	
	
	


	
	// ===> Attribute numPolyDir
	
	
	

	
 	/**
 	 * Get numPolyDir.
 	 * @return numPolyDir as int
 	 */
 	int getNumPolyDir() const;
	
 
 	
 	
 	/**
 	 * Set numPolyDir with the specified int.
 	 * @param numPolyDir The int value to which numPolyDir is to be set.
 	 
 		
 			
 	 */
 	void setNumPolyDir (int numPolyDir);
  		
	
	
	


	
	// ===> Attribute dir
	
	
	

	
 	/**
 	 * Get dir.
 	 * @return dir as std::vector<std::vector<double > >
 	 */
 	std::vector<std::vector<double > > getDir() const;
	
 
 	
 	
 	/**
 	 * Set dir with the specified std::vector<std::vector<double > >.
 	 * @param dir The std::vector<std::vector<double > > value to which dir is to be set.
 	 
 		
 			
 	 */
 	void setDir (std::vector<std::vector<double > > dir);
  		
	
	
	


	
	// ===> Attribute numPolyDist
	
	
	

	
 	/**
 	 * Get numPolyDist.
 	 * @return numPolyDist as int
 	 */
 	int getNumPolyDist() const;
	
 
 	
 	
 	/**
 	 * Set numPolyDist with the specified int.
 	 * @param numPolyDist The int value to which numPolyDist is to be set.
 	 
 		
 			
 	 */
 	void setNumPolyDist (int numPolyDist);
  		
	
	
	


	
	// ===> Attribute distance
	
	
	

	
 	/**
 	 * Get distance.
 	 * @return distance as std::vector<double >
 	 */
 	std::vector<double > getDistance() const;
	
 
 	
 	
 	/**
 	 * Set distance with the specified std::vector<double >.
 	 * @param distance The std::vector<double > value to which distance is to be set.
 	 
 		
 			
 	 */
 	void setDistance (std::vector<double > distance);
  		
	
	
	


	
	// ===> Attribute timeOrigin
	
	
	

	
 	/**
 	 * Get timeOrigin.
 	 * @return timeOrigin as ArrayTime
 	 */
 	ArrayTime getTimeOrigin() const;
	
 
 	
 	
 	/**
 	 * Set timeOrigin with the specified ArrayTime.
 	 * @param timeOrigin The ArrayTime value to which timeOrigin is to be set.
 	 
 		
 			
 	 */
 	void setTimeOrigin (ArrayTime timeOrigin);
  		
	
	
	


	
	// ===> Attribute origin
	
	
	

	
 	/**
 	 * Get origin.
 	 * @return origin as std::string
 	 */
 	std::string getOrigin() const;
	
 
 	
 	
 	/**
 	 * Set origin with the specified std::string.
 	 * @param origin The std::string value to which origin is to be set.
 	 
 		
 			
 	 */
 	void setOrigin (std::string origin);
  		
	
	
	


	
	// ===> Attribute numPolyRadVel, which is optional
	
	
	
	/**
	 * The attribute numPolyRadVel is optional. Return true if this attribute exists.
	 * @return true if and only if the numPolyRadVel attribute exists. 
	 */
	bool isNumPolyRadVelExists() const;
	

	
 	/**
 	 * Get numPolyRadVel, which is optional.
 	 * @return numPolyRadVel as int
 	 * @throws IllegalAccessException If numPolyRadVel does not exist.
 	 */
 	int getNumPolyRadVel() const;
	
 
 	
 	
 	/**
 	 * Set numPolyRadVel with the specified int.
 	 * @param numPolyRadVel The int value to which numPolyRadVel is to be set.
 	 
 		
 	 */
 	void setNumPolyRadVel (int numPolyRadVel);
		
	
	
	
	/**
	 * Mark numPolyRadVel, which is an optional field, as non-existent.
	 */
	void clearNumPolyRadVel ();
	


	
	// ===> Attribute radVel, which is optional
	
	
	
	/**
	 * The attribute radVel is optional. Return true if this attribute exists.
	 * @return true if and only if the radVel attribute exists. 
	 */
	bool isRadVelExists() const;
	

	
 	/**
 	 * Get radVel, which is optional.
 	 * @return radVel as std::vector<double >
 	 * @throws IllegalAccessException If radVel does not exist.
 	 */
 	std::vector<double > getRadVel() const;
	
 
 	
 	
 	/**
 	 * Set radVel with the specified std::vector<double >.
 	 * @param radVel The std::vector<double > value to which radVel is to be set.
 	 
 		
 	 */
 	void setRadVel (std::vector<double > radVel);
		
	
	
	
	/**
	 * Mark radVel, which is an optional field, as non-existent.
	 */
	void clearRadVel ();
	


	////////////////////////////////
	// Extrinsic Table Attributes //
	////////////////////////////////
	
	///////////
	// Links //
	///////////
	
	
	
	
	/**
	 * Compare each mandatory attribute except the autoincrementable one of this EphemerisRow with 
	 * the corresponding parameters and return true if there is a match and false otherwise.
	 	
	 * @param timeInterval
	    
	 * @param ephemerisId
	    
	 * @param observerLocation
	    
	 * @param equinoxEquator
	    
	 * @param numPolyDir
	    
	 * @param dir
	    
	 * @param numPolyDist
	    
	 * @param distance
	    
	 * @param timeOrigin
	    
	 * @param origin
	    
	 */ 
	bool compareNoAutoInc(ArrayTimeInterval timeInterval, int ephemerisId, std::vector<double > observerLocation, double equinoxEquator, int numPolyDir, std::vector<std::vector<double > > dir, int numPolyDist, std::vector<double > distance, ArrayTime timeOrigin, std::string origin);
	
	

	
	/**
	 * Compare each mandatory value (i.e. not in the key) attribute  with 
	 * the corresponding parameters and return true if there is a match and false otherwise.
	 	
	 * @param observerLocation
	    
	 * @param equinoxEquator
	    
	 * @param numPolyDir
	    
	 * @param dir
	    
	 * @param numPolyDist
	    
	 * @param distance
	    
	 * @param timeOrigin
	    
	 * @param origin
	    
	 */ 
	bool compareRequiredValue(std::vector<double > observerLocation, double equinoxEquator, int numPolyDir, std::vector<std::vector<double > > dir, int numPolyDist, std::vector<double > distance, ArrayTime timeOrigin, std::string origin); 
		 
	
	/**
	 * Return true if all required attributes of the value part are equal to their homologues
	 * in x and false otherwise.
	 *
	 * @param x a pointer on the EphemerisRow whose required attributes of the value part 
	 * will be compared with those of this.
	 * @return a boolean.
	 */
	bool equalByRequiredValue(EphemerisRow* x) ;
	
#ifndef WITHOUT_ACS
	/**
	 * Return this row in the form of an IDL struct.
	 * @return The values of this row as a EphemerisRowIDL struct.
	 */
	asdmIDL::EphemerisRowIDL *toIDL() const;
	
	/**
	 * Define the content of a EphemerisRowIDL struct from the values
	 * found in this row.
	 *
	 * @param x a reference to the EphemerisRowIDL struct to be set.
	 *
	 */
	 void toIDL(asdmIDL::EphemerisRowIDL& x) const;
#endif
	
#ifndef WITHOUT_ACS
	/**
	 * Fill the values of this row from the IDL struct EphemerisRowIDL.
	 * @param x The IDL struct containing the values used to fill this row.
	 * @throws ConversionException
	 */
	void setFromIDL (asdmIDL::EphemerisRowIDL x) ;
#endif
	
	/**
	 * Return this row in the form of an XML string.
	 * @return The values of this row as an XML string.
	 */
	std::string toXML() const;

	/**
	 * Fill the values of this row from an XML string 
	 * that was produced by the toXML() method.
	 * @param rowDoc the XML string being used to set the values of this row.
	 * @throws ConversionException
	 */
	void setFromXML (std::string rowDoc) ;

	/// @cond DISPLAY_PRIVATE	
	////////////////////////////////////////////////////////////
	// binary-deserialization material from an EndianIStream  //
	////////////////////////////////////////////////////////////

	std::map<std::string, EphemerisAttributeFromBin> fromBinMethods;
void timeIntervalFromBin( EndianIStream& eis);
void ephemerisIdFromBin( EndianIStream& eis);
void observerLocationFromBin( EndianIStream& eis);
void equinoxEquatorFromBin( EndianIStream& eis);
void numPolyDirFromBin( EndianIStream& eis);
void dirFromBin( EndianIStream& eis);
void numPolyDistFromBin( EndianIStream& eis);
void distanceFromBin( EndianIStream& eis);
void timeOriginFromBin( EndianIStream& eis);
void originFromBin( EndianIStream& eis);

void numPolyRadVelFromBin( EndianIStream& eis);
void radVelFromBin( EndianIStream& eis);


	 /**
	  * Deserialize a stream of bytes read from an EndianIStream to build a PointingRow.
	  * @param eiss the EndianIStream to be read.
	  * @param table the EphemerisTable to which the row built by deserialization will be parented.
	  * @param attributesSeq a vector containing the names of the attributes . The elements order defines the order 
	  * in which the attributes are written in the binary serialization.
	  */
	 static EphemerisRow* fromBin(EndianIStream& eis, EphemerisTable& table, const std::vector<std::string>& attributesSeq);	 
 
 	 /**
 	  * Parses a string t and assign the result of the parsing to the attribute of name attributeName.
 	  *
 	  * @param attributeName the name of the attribute whose value is going to be defined.
 	  * @param t the string to be parsed into a value given to the attribute of name attributeName.
 	  */
 	 void fromText(const std::string& attributeName, const std::string&  t);
     /// @endcond			

private:
	/**
	 * The table to which this row belongs.
	 */
	EphemerisTable &table;
	/**
	 * Whether this row has been added to the table or not.
	 */
	bool hasBeenAdded;

	// This method is used by the Table class when this row is added to the table.
	void isAdded(bool added);


	/**
	 * Create a EphemerisRow.
	 * <p>
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 */ 
	EphemerisRow (EphemerisTable &table);

	/**
	 * Create a EphemerisRow using a copy constructor mechanism.
	 * <p>
	 * Given a EphemerisRow row and a EphemerisTable table, the method creates a new
	 * EphemerisRow owned by table. Each attribute of the created row is a copy (deep)
	 * of the corresponding attribute of row. The method does not add the created
	 * row to its table, its simply parents it to table, a call to the add method
	 * has to be done in order to get the row added (very likely after having modified
	 * some of its attributes).
	 * If row is null then the method returns a row with default values for its attributes. 
	 *
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 * @param row  The row which is to be copied.
	 */
	 EphemerisRow (EphemerisTable &table, EphemerisRow *row);
	 	
	////////////////////////////////
	// Intrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute timeInterval
	
	

	ArrayTimeInterval timeInterval;

	
	
 	

	
	// ===> Attribute ephemerisId
	
	

	int ephemerisId;

	
	
 	

	
	// ===> Attribute observerLocation
	
	

	std::vector<double > observerLocation;

	
	
 	

	
	// ===> Attribute equinoxEquator
	
	

	double equinoxEquator;

	
	
 	

	
	// ===> Attribute numPolyDir
	
	

	int numPolyDir;

	
	
 	

	
	// ===> Attribute dir
	
	

	std::vector<std::vector<double > > dir;

	
	
 	

	
	// ===> Attribute numPolyDist
	
	

	int numPolyDist;

	
	
 	

	
	// ===> Attribute distance
	
	

	std::vector<double > distance;

	
	
 	

	
	// ===> Attribute timeOrigin
	
	

	ArrayTime timeOrigin;

	
	
 	

	
	// ===> Attribute origin
	
	

	std::string origin;

	
	
 	

	
	// ===> Attribute numPolyRadVel, which is optional
	
	
	bool numPolyRadVelExists;
	

	int numPolyRadVel;

	
	
 	

	
	// ===> Attribute radVel, which is optional
	
	
	bool radVelExists;
	

	std::vector<double > radVel;

	
	
 	

	////////////////////////////////
	// Extrinsic Table Attributes //
	////////////////////////////////
	
	///////////
	// Links //
	///////////
	
	
/*
	////////////////////////////////////////////////////////////
	// binary-deserialization material from an EndianIStream  //
	////////////////////////////////////////////////////////////
	std::map<std::string, EphemerisAttributeFromBin> fromBinMethods;
void timeIntervalFromBin( EndianIStream& eis);
void ephemerisIdFromBin( EndianIStream& eis);
void observerLocationFromBin( EndianIStream& eis);
void equinoxEquatorFromBin( EndianIStream& eis);
void numPolyDirFromBin( EndianIStream& eis);
void dirFromBin( EndianIStream& eis);
void numPolyDistFromBin( EndianIStream& eis);
void distanceFromBin( EndianIStream& eis);
void timeOriginFromBin( EndianIStream& eis);
void originFromBin( EndianIStream& eis);

void numPolyRadVelFromBin( EndianIStream& eis);
void radVelFromBin( EndianIStream& eis);

*/
	
	///////////////////////////////////
	// text-deserialization material //
	///////////////////////////////////
	std::map<std::string, EphemerisAttributeFromText> fromTextMethods;
	
void timeIntervalFromText (const std::string & s);
	
	
void ephemerisIdFromText (const std::string & s);
	
	
void observerLocationFromText (const std::string & s);
	
	
void equinoxEquatorFromText (const std::string & s);
	
	
void numPolyDirFromText (const std::string & s);
	
	
void dirFromText (const std::string & s);
	
	
void numPolyDistFromText (const std::string & s);
	
	
void distanceFromText (const std::string & s);
	
	
void timeOriginFromText (const std::string & s);
	
	
void originFromText (const std::string & s);
	

	
void numPolyRadVelFromText (const std::string & s);
	
	
void radVelFromText (const std::string & s);
	
	
	
	/**
	 * Serialize this into a stream of bytes written to an EndianOSStream.
	 * @param eoss the EndianOSStream to be written to
	 */
	 void toBin(EndianOSStream& eoss);
	 	 
	 /**
	  * Deserialize a stream of bytes read from an EndianIStream to build a PointingRow.
	  * @param eiss the EndianIStream to be read.
	  * @param table the EphemerisTable to which the row built by deserialization will be parented.
	  * @param attributesSeq a vector containing the names of the attributes . The elements order defines the order 
	  * in which the attributes are written in the binary serialization.

	 static EphemerisRow* fromBin(EndianIStream& eis, EphemerisTable& table, const std::vector<std::string>& attributesSeq);	 
		*/
};

} // End namespace asdm

#endif /* Ephemeris_CLASS */
