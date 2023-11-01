
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
 * File VLAWVRRow.h
 */
 
#ifndef VLAWVRRow_CLASS
#define VLAWVRRow_CLASS

#include <vector>
#include <string>
#include <set>

#ifndef WITHOUT_ACS
#include <asdmIDLC.h>
#endif






	 
#include <alma/ASDM/ArrayTimeInterval.h>
	

	 
#include <alma/ASDM/Frequency.h>
	

	 
#include <alma/ASDM/Tag.h>
	




	

	

	

	

	

	

	



#include <alma/ASDM/ConversionException.h>
#include <alma/ASDM/NoSuchRow.h>
#include <alma/ASDM/IllegalAccessException.h>

#include <alma/ASDM/RowTransformer.h>
//#include <alma/ASDM/TableStreamReader.h>

/*\file VLAWVR.h
    \brief Generated from model's revision "-1", branch ""
*/

namespace asdm {

//class asdm::VLAWVRTable;


// class asdm::AntennaRow;
class AntennaRow;
	

class VLAWVRRow;
typedef void (VLAWVRRow::*VLAWVRAttributeFromBin) (EndianIStream& eis);
typedef void (VLAWVRRow::*VLAWVRAttributeFromText) (const std::string& s);

/**
 * The VLAWVRRow class is a row of a VLAWVRTable.
 * 
 * Generated from model's revision "-1", branch ""
 *
 */
class VLAWVRRow {
friend class asdm::VLAWVRTable;
friend class asdm::RowTransformer<VLAWVRRow>;
//friend class asdm::TableStreamReader<VLAWVRTable, VLAWVRRow>;

public:

	virtual ~VLAWVRRow();

	/**
	 * Return the table to which this row belongs.
	 */
	VLAWVRTable &getTable() const;
	
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
  		
	
	
	


	
	// ===> Attribute numChan
	
	
	

	
 	/**
 	 * Get numChan.
 	 * @return numChan as int
 	 */
 	int getNumChan() const;
	
 
 	
 	
 	/**
 	 * Set numChan with the specified int.
 	 * @param numChan The int value to which numChan is to be set.
 	 
 		
 			
 	 */
 	void setNumChan (int numChan);
  		
	
	
	


	
	// ===> Attribute hiValues
	
	
	

	
 	/**
 	 * Get hiValues.
 	 * @return hiValues as std::vector<float >
 	 */
 	std::vector<float > getHiValues() const;
	
 
 	
 	
 	/**
 	 * Set hiValues with the specified std::vector<float >.
 	 * @param hiValues The std::vector<float > value to which hiValues is to be set.
 	 
 		
 			
 	 */
 	void setHiValues (std::vector<float > hiValues);
  		
	
	
	


	
	// ===> Attribute loValues
	
	
	

	
 	/**
 	 * Get loValues.
 	 * @return loValues as std::vector<float >
 	 */
 	std::vector<float > getLoValues() const;
	
 
 	
 	
 	/**
 	 * Set loValues with the specified std::vector<float >.
 	 * @param loValues The std::vector<float > value to which loValues is to be set.
 	 
 		
 			
 	 */
 	void setLoValues (std::vector<float > loValues);
  		
	
	
	


	
	// ===> Attribute chanFreqCenter, which is optional
	
	
	
	/**
	 * The attribute chanFreqCenter is optional. Return true if this attribute exists.
	 * @return true if and only if the chanFreqCenter attribute exists. 
	 */
	bool isChanFreqCenterExists() const;
	

	
 	/**
 	 * Get chanFreqCenter, which is optional.
 	 * @return chanFreqCenter as std::vector<Frequency >
 	 * @throws IllegalAccessException If chanFreqCenter does not exist.
 	 */
 	std::vector<Frequency > getChanFreqCenter() const;
	
 
 	
 	
 	/**
 	 * Set chanFreqCenter with the specified std::vector<Frequency >.
 	 * @param chanFreqCenter The std::vector<Frequency > value to which chanFreqCenter is to be set.
 	 
 		
 	 */
 	void setChanFreqCenter (std::vector<Frequency > chanFreqCenter);
		
	
	
	
	/**
	 * Mark chanFreqCenter, which is an optional field, as non-existent.
	 */
	void clearChanFreqCenter ();
	


	
	// ===> Attribute chanWidth, which is optional
	
	
	
	/**
	 * The attribute chanWidth is optional. Return true if this attribute exists.
	 * @return true if and only if the chanWidth attribute exists. 
	 */
	bool isChanWidthExists() const;
	

	
 	/**
 	 * Get chanWidth, which is optional.
 	 * @return chanWidth as std::vector<Frequency >
 	 * @throws IllegalAccessException If chanWidth does not exist.
 	 */
 	std::vector<Frequency > getChanWidth() const;
	
 
 	
 	
 	/**
 	 * Set chanWidth with the specified std::vector<Frequency >.
 	 * @param chanWidth The std::vector<Frequency > value to which chanWidth is to be set.
 	 
 		
 	 */
 	void setChanWidth (std::vector<Frequency > chanWidth);
		
	
	
	
	/**
	 * Mark chanWidth, which is an optional field, as non-existent.
	 */
	void clearChanWidth ();
	


	
	// ===> Attribute wvrId, which is optional
	
	
	
	/**
	 * The attribute wvrId is optional. Return true if this attribute exists.
	 * @return true if and only if the wvrId attribute exists. 
	 */
	bool isWvrIdExists() const;
	

	
 	/**
 	 * Get wvrId, which is optional.
 	 * @return wvrId as std::string
 	 * @throws IllegalAccessException If wvrId does not exist.
 	 */
 	std::string getWvrId() const;
	
 
 	
 	
 	/**
 	 * Set wvrId with the specified std::string.
 	 * @param wvrId The std::string value to which wvrId is to be set.
 	 
 		
 	 */
 	void setWvrId (std::string wvrId);
		
	
	
	
	/**
	 * Mark wvrId, which is an optional field, as non-existent.
	 */
	void clearWvrId ();
	


	////////////////////////////////
	// Extrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute antennaId
	
	
	

	
 	/**
 	 * Get antennaId.
 	 * @return antennaId as Tag
 	 */
 	Tag getAntennaId() const;
	
 
 	
 	
 	/**
 	 * Set antennaId with the specified Tag.
 	 * @param antennaId The Tag value to which antennaId is to be set.
 	 
 		
 			
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 		
 	 */
 	void setAntennaId (Tag antennaId);
  		
	
	
	


	///////////
	// Links //
	///////////
	
	

	
		
	/**
	 * antennaId pointer to the row in the Antenna table having Antenna.antennaId == antennaId
	 * @return a AntennaRow*
	 * 
	 
	 */
	 AntennaRow* getAntennaUsingAntennaId();
	 

	

	
	
	
	/**
	 * Compare each mandatory attribute except the autoincrementable one of this VLAWVRRow with 
	 * the corresponding parameters and return true if there is a match and false otherwise.
	 	
	 * @param antennaId
	    
	 * @param timeInterval
	    
	 * @param numChan
	    
	 * @param hiValues
	    
	 * @param loValues
	    
	 */ 
	bool compareNoAutoInc(Tag antennaId, ArrayTimeInterval timeInterval, int numChan, std::vector<float > hiValues, std::vector<float > loValues);
	
	

	
	/**
	 * Compare each mandatory value (i.e. not in the key) attribute  with 
	 * the corresponding parameters and return true if there is a match and false otherwise.
	 	
	 * @param numChan
	    
	 * @param hiValues
	    
	 * @param loValues
	    
	 */ 
	bool compareRequiredValue(int numChan, std::vector<float > hiValues, std::vector<float > loValues); 
		 
	
	/**
	 * Return true if all required attributes of the value part are equal to their homologues
	 * in x and false otherwise.
	 *
	 * @param x a pointer on the VLAWVRRow whose required attributes of the value part 
	 * will be compared with those of this.
	 * @return a boolean.
	 */
	bool equalByRequiredValue(VLAWVRRow* x) ;
	
#ifndef WITHOUT_ACS
	/**
	 * Return this row in the form of an IDL struct.
	 * @return The values of this row as a VLAWVRRowIDL struct.
	 */
	asdmIDL::VLAWVRRowIDL *toIDL() const;
	
	/**
	 * Define the content of a VLAWVRRowIDL struct from the values
	 * found in this row.
	 *
	 * @param x a reference to the VLAWVRRowIDL struct to be set.
	 *
	 */
	 void toIDL(asdmIDL::VLAWVRRowIDL& x) const;
#endif
	
#ifndef WITHOUT_ACS
	/**
	 * Fill the values of this row from the IDL struct VLAWVRRowIDL.
	 * @param x The IDL struct containing the values used to fill this row.
	 * @throws ConversionException
	 */
	void setFromIDL (asdmIDL::VLAWVRRowIDL x) ;
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

	std::map<std::string, VLAWVRAttributeFromBin> fromBinMethods;
void antennaIdFromBin( EndianIStream& eis);
void timeIntervalFromBin( EndianIStream& eis);
void numChanFromBin( EndianIStream& eis);
void hiValuesFromBin( EndianIStream& eis);
void loValuesFromBin( EndianIStream& eis);

void chanFreqCenterFromBin( EndianIStream& eis);
void chanWidthFromBin( EndianIStream& eis);
void wvrIdFromBin( EndianIStream& eis);


	 /**
	  * Deserialize a stream of bytes read from an EndianIStream to build a PointingRow.
	  * @param eiss the EndianIStream to be read.
	  * @param table the VLAWVRTable to which the row built by deserialization will be parented.
	  * @param attributesSeq a vector containing the names of the attributes . The elements order defines the order 
	  * in which the attributes are written in the binary serialization.
	  */
	 static VLAWVRRow* fromBin(EndianIStream& eis, VLAWVRTable& table, const std::vector<std::string>& attributesSeq);	 
 
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
	VLAWVRTable &table;
	/**
	 * Whether this row has been added to the table or not.
	 */
	bool hasBeenAdded;

	// This method is used by the Table class when this row is added to the table.
	void isAdded(bool added);


	/**
	 * Create a VLAWVRRow.
	 * <p>
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 */ 
	VLAWVRRow (VLAWVRTable &table);

	/**
	 * Create a VLAWVRRow using a copy constructor mechanism.
	 * <p>
	 * Given a VLAWVRRow row and a VLAWVRTable table, the method creates a new
	 * VLAWVRRow owned by table. Each attribute of the created row is a copy (deep)
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
	 VLAWVRRow (VLAWVRTable &table, VLAWVRRow *row);
	 	
	////////////////////////////////
	// Intrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute timeInterval
	
	

	ArrayTimeInterval timeInterval;

	
	
 	

	
	// ===> Attribute numChan
	
	

	int numChan;

	
	
 	

	
	// ===> Attribute hiValues
	
	

	std::vector<float > hiValues;

	
	
 	

	
	// ===> Attribute loValues
	
	

	std::vector<float > loValues;

	
	
 	

	
	// ===> Attribute chanFreqCenter, which is optional
	
	
	bool chanFreqCenterExists;
	

	std::vector<Frequency > chanFreqCenter;

	
	
 	

	
	// ===> Attribute chanWidth, which is optional
	
	
	bool chanWidthExists;
	

	std::vector<Frequency > chanWidth;

	
	
 	

	
	// ===> Attribute wvrId, which is optional
	
	
	bool wvrIdExists;
	

	std::string wvrId;

	
	
 	

	////////////////////////////////
	// Extrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute antennaId
	
	

	Tag antennaId;

	
	
 	

	///////////
	// Links //
	///////////
	
	
		

	 

	

	
/*
	////////////////////////////////////////////////////////////
	// binary-deserialization material from an EndianIStream  //
	////////////////////////////////////////////////////////////
	std::map<std::string, VLAWVRAttributeFromBin> fromBinMethods;
void antennaIdFromBin( EndianIStream& eis);
void timeIntervalFromBin( EndianIStream& eis);
void numChanFromBin( EndianIStream& eis);
void hiValuesFromBin( EndianIStream& eis);
void loValuesFromBin( EndianIStream& eis);

void chanFreqCenterFromBin( EndianIStream& eis);
void chanWidthFromBin( EndianIStream& eis);
void wvrIdFromBin( EndianIStream& eis);

*/
	
	///////////////////////////////////
	// text-deserialization material //
	///////////////////////////////////
	std::map<std::string, VLAWVRAttributeFromText> fromTextMethods;
	
void antennaIdFromText (const std::string & s);
	
	
void timeIntervalFromText (const std::string & s);
	
	
void numChanFromText (const std::string & s);
	
	
void hiValuesFromText (const std::string & s);
	
	
void loValuesFromText (const std::string & s);
	

	
void chanFreqCenterFromText (const std::string & s);
	
	
void chanWidthFromText (const std::string & s);
	
	
void wvrIdFromText (const std::string & s);
	
	
	
	/**
	 * Serialize this into a stream of bytes written to an EndianOSStream.
	 * @param eoss the EndianOSStream to be written to
	 */
	 void toBin(EndianOSStream& eoss);
	 	 
	 /**
	  * Deserialize a stream of bytes read from an EndianIStream to build a PointingRow.
	  * @param eiss the EndianIStream to be read.
	  * @param table the VLAWVRTable to which the row built by deserialization will be parented.
	  * @param attributesSeq a vector containing the names of the attributes . The elements order defines the order 
	  * in which the attributes are written in the binary serialization.

	 static VLAWVRRow* fromBin(EndianIStream& eis, VLAWVRTable& table, const std::vector<std::string>& attributesSeq);	 
		*/
};

} // End namespace asdm

#endif /* VLAWVR_CLASS */
