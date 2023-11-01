
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
 * File FocusRow.h
 */
 
#ifndef FocusRow_CLASS
#define FocusRow_CLASS

#include <vector>
#include <string>
#include <set>

#ifndef WITHOUT_ACS
#include <asdmIDLC.h>
#endif






	 
#include <alma/ASDM/ArrayTimeInterval.h>
	

	 
#include <alma/ASDM/Angle.h>
	

	 
#include <alma/ASDM/Length.h>
	

	 
#include <alma/ASDM/Tag.h>
	




	

	

	

	

	

	



#include <alma/ASDM/ConversionException.h>
#include <alma/ASDM/NoSuchRow.h>
#include <alma/ASDM/IllegalAccessException.h>

#include <alma/ASDM/RowTransformer.h>
//#include <alma/ASDM/TableStreamReader.h>

/*\file Focus.h
    \brief Generated from model's revision "-1", branch ""
*/

namespace asdm {

//class asdm::FocusTable;


// class asdm::AntennaRow;
class AntennaRow;

// class asdm::FocusModelRow;
class FocusModelRow;
	

class FocusRow;
typedef void (FocusRow::*FocusAttributeFromBin) (EndianIStream& eis);
typedef void (FocusRow::*FocusAttributeFromText) (const std::string& s);

/**
 * The FocusRow class is a row of a FocusTable.
 * 
 * Generated from model's revision "-1", branch ""
 *
 */
class FocusRow {
friend class asdm::FocusTable;
friend class asdm::RowTransformer<FocusRow>;
//friend class asdm::TableStreamReader<FocusTable, FocusRow>;

public:

	virtual ~FocusRow();

	/**
	 * Return the table to which this row belongs.
	 */
	FocusTable &getTable() const;
	
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
  		
	
	
	


	
	// ===> Attribute focusTracking
	
	
	

	
 	/**
 	 * Get focusTracking.
 	 * @return focusTracking as bool
 	 */
 	bool getFocusTracking() const;
	
 
 	
 	
 	/**
 	 * Set focusTracking with the specified bool.
 	 * @param focusTracking The bool value to which focusTracking is to be set.
 	 
 		
 			
 	 */
 	void setFocusTracking (bool focusTracking);
  		
	
	
	


	
	// ===> Attribute focusOffset
	
	
	

	
 	/**
 	 * Get focusOffset.
 	 * @return focusOffset as std::vector<Length >
 	 */
 	std::vector<Length > getFocusOffset() const;
	
 
 	
 	
 	/**
 	 * Set focusOffset with the specified std::vector<Length >.
 	 * @param focusOffset The std::vector<Length > value to which focusOffset is to be set.
 	 
 		
 			
 	 */
 	void setFocusOffset (std::vector<Length > focusOffset);
  		
	
	
	


	
	// ===> Attribute focusRotationOffset
	
	
	

	
 	/**
 	 * Get focusRotationOffset.
 	 * @return focusRotationOffset as std::vector<Angle >
 	 */
 	std::vector<Angle > getFocusRotationOffset() const;
	
 
 	
 	
 	/**
 	 * Set focusRotationOffset with the specified std::vector<Angle >.
 	 * @param focusRotationOffset The std::vector<Angle > value to which focusRotationOffset is to be set.
 	 
 		
 			
 	 */
 	void setFocusRotationOffset (std::vector<Angle > focusRotationOffset);
  		
	
	
	


	
	// ===> Attribute measuredFocusPosition, which is optional
	
	
	
	/**
	 * The attribute measuredFocusPosition is optional. Return true if this attribute exists.
	 * @return true if and only if the measuredFocusPosition attribute exists. 
	 */
	bool isMeasuredFocusPositionExists() const;
	

	
 	/**
 	 * Get measuredFocusPosition, which is optional.
 	 * @return measuredFocusPosition as std::vector<Length >
 	 * @throws IllegalAccessException If measuredFocusPosition does not exist.
 	 */
 	std::vector<Length > getMeasuredFocusPosition() const;
	
 
 	
 	
 	/**
 	 * Set measuredFocusPosition with the specified std::vector<Length >.
 	 * @param measuredFocusPosition The std::vector<Length > value to which measuredFocusPosition is to be set.
 	 
 		
 	 */
 	void setMeasuredFocusPosition (std::vector<Length > measuredFocusPosition);
		
	
	
	
	/**
	 * Mark measuredFocusPosition, which is an optional field, as non-existent.
	 */
	void clearMeasuredFocusPosition ();
	


	
	// ===> Attribute measuredFocusRotation, which is optional
	
	
	
	/**
	 * The attribute measuredFocusRotation is optional. Return true if this attribute exists.
	 * @return true if and only if the measuredFocusRotation attribute exists. 
	 */
	bool isMeasuredFocusRotationExists() const;
	

	
 	/**
 	 * Get measuredFocusRotation, which is optional.
 	 * @return measuredFocusRotation as std::vector<Angle >
 	 * @throws IllegalAccessException If measuredFocusRotation does not exist.
 	 */
 	std::vector<Angle > getMeasuredFocusRotation() const;
	
 
 	
 	
 	/**
 	 * Set measuredFocusRotation with the specified std::vector<Angle >.
 	 * @param measuredFocusRotation The std::vector<Angle > value to which measuredFocusRotation is to be set.
 	 
 		
 	 */
 	void setMeasuredFocusRotation (std::vector<Angle > measuredFocusRotation);
		
	
	
	
	/**
	 * Mark measuredFocusRotation, which is an optional field, as non-existent.
	 */
	void clearMeasuredFocusRotation ();
	


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
  		
	
	
	


	
	// ===> Attribute focusModelId
	
	
	

	
 	/**
 	 * Get focusModelId.
 	 * @return focusModelId as int
 	 */
 	int getFocusModelId() const;
	
 
 	
 	
 	/**
 	 * Set focusModelId with the specified int.
 	 * @param focusModelId The int value to which focusModelId is to be set.
 	 
 		
 			
 	 */
 	void setFocusModelId (int focusModelId);
  		
	
	
	


	///////////
	// Links //
	///////////
	
	

	
		
	/**
	 * antennaId pointer to the row in the Antenna table having Antenna.antennaId == antennaId
	 * @return a AntennaRow*
	 * 
	 
	 */
	 AntennaRow* getAntennaUsingAntennaId();
	 

	

	

	
		
	// ===> Slice link from a row of Focus table to a collection of row of FocusModel table.
	
	/**
	 * Get the collection of row in the FocusModel table having focusModelId == this.focusModelId
	 * 
	 * @return a vector of FocusModelRow *
	 */
	std::vector <FocusModelRow *> getFocusModels();
	
	

	

	
	
	
	/**
	 * Compare each mandatory attribute except the autoincrementable one of this FocusRow with 
	 * the corresponding parameters and return true if there is a match and false otherwise.
	 	
	 * @param antennaId
	    
	 * @param timeInterval
	    
	 * @param focusTracking
	    
	 * @param focusOffset
	    
	 * @param focusRotationOffset
	    
	 * @param focusModelId
	    
	 */ 
	bool compareNoAutoInc(Tag antennaId, ArrayTimeInterval timeInterval, bool focusTracking, std::vector<Length > focusOffset, std::vector<Angle > focusRotationOffset, int focusModelId);
	
	

	
	/**
	 * Compare each mandatory value (i.e. not in the key) attribute  with 
	 * the corresponding parameters and return true if there is a match and false otherwise.
	 	
	 * @param focusTracking
	    
	 * @param focusOffset
	    
	 * @param focusRotationOffset
	    
	 * @param focusModelId
	    
	 */ 
	bool compareRequiredValue(bool focusTracking, std::vector<Length > focusOffset, std::vector<Angle > focusRotationOffset, int focusModelId); 
		 
	
	/**
	 * Return true if all required attributes of the value part are equal to their homologues
	 * in x and false otherwise.
	 *
	 * @param x a pointer on the FocusRow whose required attributes of the value part 
	 * will be compared with those of this.
	 * @return a boolean.
	 */
	bool equalByRequiredValue(FocusRow* x) ;
	
#ifndef WITHOUT_ACS
	/**
	 * Return this row in the form of an IDL struct.
	 * @return The values of this row as a FocusRowIDL struct.
	 */
	asdmIDL::FocusRowIDL *toIDL() const;
	
	/**
	 * Define the content of a FocusRowIDL struct from the values
	 * found in this row.
	 *
	 * @param x a reference to the FocusRowIDL struct to be set.
	 *
	 */
	 void toIDL(asdmIDL::FocusRowIDL& x) const;
#endif
	
#ifndef WITHOUT_ACS
	/**
	 * Fill the values of this row from the IDL struct FocusRowIDL.
	 * @param x The IDL struct containing the values used to fill this row.
	 * @throws ConversionException
	 */
	void setFromIDL (asdmIDL::FocusRowIDL x) ;
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

	std::map<std::string, FocusAttributeFromBin> fromBinMethods;
void antennaIdFromBin( EndianIStream& eis);
void timeIntervalFromBin( EndianIStream& eis);
void focusTrackingFromBin( EndianIStream& eis);
void focusOffsetFromBin( EndianIStream& eis);
void focusRotationOffsetFromBin( EndianIStream& eis);
void focusModelIdFromBin( EndianIStream& eis);

void measuredFocusPositionFromBin( EndianIStream& eis);
void measuredFocusRotationFromBin( EndianIStream& eis);


	 /**
	  * Deserialize a stream of bytes read from an EndianIStream to build a PointingRow.
	  * @param eiss the EndianIStream to be read.
	  * @param table the FocusTable to which the row built by deserialization will be parented.
	  * @param attributesSeq a vector containing the names of the attributes . The elements order defines the order 
	  * in which the attributes are written in the binary serialization.
	  */
	 static FocusRow* fromBin(EndianIStream& eis, FocusTable& table, const std::vector<std::string>& attributesSeq);	 
 
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
	FocusTable &table;
	/**
	 * Whether this row has been added to the table or not.
	 */
	bool hasBeenAdded;

	// This method is used by the Table class when this row is added to the table.
	void isAdded(bool added);


	/**
	 * Create a FocusRow.
	 * <p>
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 */ 
	FocusRow (FocusTable &table);

	/**
	 * Create a FocusRow using a copy constructor mechanism.
	 * <p>
	 * Given a FocusRow row and a FocusTable table, the method creates a new
	 * FocusRow owned by table. Each attribute of the created row is a copy (deep)
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
	 FocusRow (FocusTable &table, FocusRow *row);
	 	
	////////////////////////////////
	// Intrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute timeInterval
	
	

	ArrayTimeInterval timeInterval;

	
	
 	

	
	// ===> Attribute focusTracking
	
	

	bool focusTracking;

	
	
 	

	
	// ===> Attribute focusOffset
	
	

	std::vector<Length > focusOffset;

	
	
 	

	
	// ===> Attribute focusRotationOffset
	
	

	std::vector<Angle > focusRotationOffset;

	
	
 	

	
	// ===> Attribute measuredFocusPosition, which is optional
	
	
	bool measuredFocusPositionExists;
	

	std::vector<Length > measuredFocusPosition;

	
	
 	

	
	// ===> Attribute measuredFocusRotation, which is optional
	
	
	bool measuredFocusRotationExists;
	

	std::vector<Angle > measuredFocusRotation;

	
	
 	

	////////////////////////////////
	// Extrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute antennaId
	
	

	Tag antennaId;

	
	
 	

	
	// ===> Attribute focusModelId
	
	

	int focusModelId;

	
	
 	

	///////////
	// Links //
	///////////
	
	
		

	 

	

	
		


	

	
/*
	////////////////////////////////////////////////////////////
	// binary-deserialization material from an EndianIStream  //
	////////////////////////////////////////////////////////////
	std::map<std::string, FocusAttributeFromBin> fromBinMethods;
void antennaIdFromBin( EndianIStream& eis);
void timeIntervalFromBin( EndianIStream& eis);
void focusTrackingFromBin( EndianIStream& eis);
void focusOffsetFromBin( EndianIStream& eis);
void focusRotationOffsetFromBin( EndianIStream& eis);
void focusModelIdFromBin( EndianIStream& eis);

void measuredFocusPositionFromBin( EndianIStream& eis);
void measuredFocusRotationFromBin( EndianIStream& eis);

*/
	
	///////////////////////////////////
	// text-deserialization material //
	///////////////////////////////////
	std::map<std::string, FocusAttributeFromText> fromTextMethods;
	
void antennaIdFromText (const std::string & s);
	
	
void timeIntervalFromText (const std::string & s);
	
	
void focusTrackingFromText (const std::string & s);
	
	
void focusOffsetFromText (const std::string & s);
	
	
void focusRotationOffsetFromText (const std::string & s);
	
	
void focusModelIdFromText (const std::string & s);
	

	
void measuredFocusPositionFromText (const std::string & s);
	
	
void measuredFocusRotationFromText (const std::string & s);
	
	
	
	/**
	 * Serialize this into a stream of bytes written to an EndianOSStream.
	 * @param eoss the EndianOSStream to be written to
	 */
	 void toBin(EndianOSStream& eoss);
	 	 
	 /**
	  * Deserialize a stream of bytes read from an EndianIStream to build a PointingRow.
	  * @param eiss the EndianIStream to be read.
	  * @param table the FocusTable to which the row built by deserialization will be parented.
	  * @param attributesSeq a vector containing the names of the attributes . The elements order defines the order 
	  * in which the attributes are written in the binary serialization.

	 static FocusRow* fromBin(EndianIStream& eis, FocusTable& table, const std::vector<std::string>& attributesSeq);	 
		*/
};

} // End namespace asdm

#endif /* Focus_CLASS */
