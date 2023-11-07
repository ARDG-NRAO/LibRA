
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
 * File AntennaRow.h
 */
 
#ifndef AntennaRow_CLASS
#define AntennaRow_CLASS

#include <vector>
#include <string>
#include <set>

#ifndef WITHOUT_ACS
#include <asdmIDLC.h>
#endif






	 
#include <alma/ASDM/Length.h>
	

	 
#include <alma/ASDM/ArrayTime.h>
	

	 
#include <alma/ASDM/Tag.h>
	




	

	

	
#include <alma/Enumerations/CAntennaMake.h>
	

	
#include <alma/Enumerations/CAntennaType.h>
	

	

	

	

	



#include <alma/ASDM/ConversionException.h>
#include <alma/ASDM/NoSuchRow.h>
#include <alma/ASDM/IllegalAccessException.h>

#include <alma/ASDM/RowTransformer.h>
//#include <alma/ASDM/TableStreamReader.h>

/*\file Antenna.h
    \brief Generated from model's revision "-1", branch ""
*/

namespace asdm {

//class asdm::AntennaTable;


// class asdm::AntennaRow;
class AntennaRow;

// class asdm::StationRow;
class StationRow;
	

class AntennaRow;
typedef void (AntennaRow::*AntennaAttributeFromBin) (EndianIStream& eis);
typedef void (AntennaRow::*AntennaAttributeFromText) (const std::string& s);

/**
 * The AntennaRow class is a row of a AntennaTable.
 * 
 * Generated from model's revision "-1", branch ""
 *
 */
class AntennaRow {
friend class asdm::AntennaTable;
friend class asdm::RowTransformer<AntennaRow>;
//friend class asdm::TableStreamReader<AntennaTable, AntennaRow>;

public:

	virtual ~AntennaRow();

	/**
	 * Return the table to which this row belongs.
	 */
	AntennaTable &getTable() const;
	
	/**
	 * Has this row been added to its table ?
	 * @return true if and only if it has been added.
	 */
	bool isAdded() const;
		
	////////////////////////////////
	// Intrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute antennaId
	
	
	

	
 	/**
 	 * Get antennaId.
 	 * @return antennaId as Tag
 	 */
 	Tag getAntennaId() const;
	
 
 	
 	
	
	


	
	// ===> Attribute name
	
	
	

	
 	/**
 	 * Get name.
 	 * @return name as std::string
 	 */
 	std::string getName() const;
	
 
 	
 	
 	/**
 	 * Set name with the specified std::string.
 	 * @param name The std::string value to which name is to be set.
 	 
 		
 			
 	 */
 	void setName (std::string name);
  		
	
	
	


	
	// ===> Attribute antennaMake
	
	
	

	
 	/**
 	 * Get antennaMake.
 	 * @return antennaMake as AntennaMakeMod::AntennaMake
 	 */
 	AntennaMakeMod::AntennaMake getAntennaMake() const;
	
 
 	
 	
 	/**
 	 * Set antennaMake with the specified AntennaMakeMod::AntennaMake.
 	 * @param antennaMake The AntennaMakeMod::AntennaMake value to which antennaMake is to be set.
 	 
 		
 			
 	 */
 	void setAntennaMake (AntennaMakeMod::AntennaMake antennaMake);
  		
	
	
	


	
	// ===> Attribute antennaType
	
	
	

	
 	/**
 	 * Get antennaType.
 	 * @return antennaType as AntennaTypeMod::AntennaType
 	 */
 	AntennaTypeMod::AntennaType getAntennaType() const;
	
 
 	
 	
 	/**
 	 * Set antennaType with the specified AntennaTypeMod::AntennaType.
 	 * @param antennaType The AntennaTypeMod::AntennaType value to which antennaType is to be set.
 	 
 		
 			
 	 */
 	void setAntennaType (AntennaTypeMod::AntennaType antennaType);
  		
	
	
	


	
	// ===> Attribute dishDiameter
	
	
	

	
 	/**
 	 * Get dishDiameter.
 	 * @return dishDiameter as Length
 	 */
 	Length getDishDiameter() const;
	
 
 	
 	
 	/**
 	 * Set dishDiameter with the specified Length.
 	 * @param dishDiameter The Length value to which dishDiameter is to be set.
 	 
 		
 			
 	 */
 	void setDishDiameter (Length dishDiameter);
  		
	
	
	


	
	// ===> Attribute position
	
	
	

	
 	/**
 	 * Get position.
 	 * @return position as std::vector<Length >
 	 */
 	std::vector<Length > getPosition() const;
	
 
 	
 	
 	/**
 	 * Set position with the specified std::vector<Length >.
 	 * @param position The std::vector<Length > value to which position is to be set.
 	 
 		
 			
 	 */
 	void setPosition (std::vector<Length > position);
  		
	
	
	


	
	// ===> Attribute offset
	
	
	

	
 	/**
 	 * Get offset.
 	 * @return offset as std::vector<Length >
 	 */
 	std::vector<Length > getOffset() const;
	
 
 	
 	
 	/**
 	 * Set offset with the specified std::vector<Length >.
 	 * @param offset The std::vector<Length > value to which offset is to be set.
 	 
 		
 			
 	 */
 	void setOffset (std::vector<Length > offset);
  		
	
	
	


	
	// ===> Attribute time
	
	
	

	
 	/**
 	 * Get time.
 	 * @return time as ArrayTime
 	 */
 	ArrayTime getTime() const;
	
 
 	
 	
 	/**
 	 * Set time with the specified ArrayTime.
 	 * @param time The ArrayTime value to which time is to be set.
 	 
 		
 			
 	 */
 	void setTime (ArrayTime time);
  		
	
	
	


	////////////////////////////////
	// Extrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute assocAntennaId, which is optional
	
	
	
	/**
	 * The attribute assocAntennaId is optional. Return true if this attribute exists.
	 * @return true if and only if the assocAntennaId attribute exists. 
	 */
	bool isAssocAntennaIdExists() const;
	

	
 	/**
 	 * Get assocAntennaId, which is optional.
 	 * @return assocAntennaId as Tag
 	 * @throws IllegalAccessException If assocAntennaId does not exist.
 	 */
 	Tag getAssocAntennaId() const;
	
 
 	
 	
 	/**
 	 * Set assocAntennaId with the specified Tag.
 	 * @param assocAntennaId The Tag value to which assocAntennaId is to be set.
 	 
 		
 	 */
 	void setAssocAntennaId (Tag assocAntennaId);
		
	
	
	
	/**
	 * Mark assocAntennaId, which is an optional field, as non-existent.
	 */
	void clearAssocAntennaId ();
	


	
	// ===> Attribute stationId
	
	
	

	
 	/**
 	 * Get stationId.
 	 * @return stationId as Tag
 	 */
 	Tag getStationId() const;
	
 
 	
 	
 	/**
 	 * Set stationId with the specified Tag.
 	 * @param stationId The Tag value to which stationId is to be set.
 	 
 		
 			
 	 */
 	void setStationId (Tag stationId);
  		
	
	
	


	///////////
	// Links //
	///////////
	
	

	
		
		
			
	// ===> Optional link from a row of Antenna table to a row of Antenna table.

	/**
	 * The link to table Antenna is optional. Return true if this link exists.
	 * @return true if and only if the Antenna link exists. 
	 */
	bool isAssociatedAntennaExists() const;

	/**
	 * Get the optional row in table Antenna by traversing the defined link to that table.
	 * @return A row in Antenna table.
	 * @throws NoSuchRow if there is no such row in table Antenna or the link does not exist.
	 */
	AntennaRow *getAssociatedAntenna() const;
	
	/**
	 * Set the values of the link attributes needed to link this row to a row in table Antenna.
	 */
	void setAssociatedAntennaLink(Tag assocAntennaId);


		
		
	

	

	
		
	/**
	 * stationId pointer to the row in the Station table having Station.stationId == stationId
	 * @return a StationRow*
	 * 
	 
	 */
	 StationRow* getStationUsingStationId();
	 

	

	
	
	
	/**
	 * Compare each mandatory attribute except the autoincrementable one of this AntennaRow with 
	 * the corresponding parameters and return true if there is a match and false otherwise.
	 	
	 * @param name
	    
	 * @param antennaMake
	    
	 * @param antennaType
	    
	 * @param dishDiameter
	    
	 * @param position
	    
	 * @param offset
	    
	 * @param time
	    
	 * @param stationId
	    
	 */ 
	bool compareNoAutoInc(std::string name, AntennaMakeMod::AntennaMake antennaMake, AntennaTypeMod::AntennaType antennaType, Length dishDiameter, std::vector<Length > position, std::vector<Length > offset, ArrayTime time, Tag stationId);
	
	

	
	/**
	 * Compare each mandatory value (i.e. not in the key) attribute  with 
	 * the corresponding parameters and return true if there is a match and false otherwise.
	 	
	 * @param name
	    
	 * @param antennaMake
	    
	 * @param antennaType
	    
	 * @param dishDiameter
	    
	 * @param position
	    
	 * @param offset
	    
	 * @param time
	    
	 * @param stationId
	    
	 */ 
	bool compareRequiredValue(std::string name, AntennaMakeMod::AntennaMake antennaMake, AntennaTypeMod::AntennaType antennaType, Length dishDiameter, std::vector<Length > position, std::vector<Length > offset, ArrayTime time, Tag stationId); 
		 
	
	/**
	 * Return true if all required attributes of the value part are equal to their homologues
	 * in x and false otherwise.
	 *
	 * @param x a pointer on the AntennaRow whose required attributes of the value part 
	 * will be compared with those of this.
	 * @return a boolean.
	 */
	bool equalByRequiredValue(AntennaRow* x) ;
	
#ifndef WITHOUT_ACS
	/**
	 * Return this row in the form of an IDL struct.
	 * @return The values of this row as a AntennaRowIDL struct.
	 */
	asdmIDL::AntennaRowIDL *toIDL() const;
	
	/**
	 * Define the content of a AntennaRowIDL struct from the values
	 * found in this row.
	 *
	 * @param x a reference to the AntennaRowIDL struct to be set.
	 *
	 */
	 void toIDL(asdmIDL::AntennaRowIDL& x) const;
#endif
	
#ifndef WITHOUT_ACS
	/**
	 * Fill the values of this row from the IDL struct AntennaRowIDL.
	 * @param x The IDL struct containing the values used to fill this row.
	 * @throws ConversionException
	 */
	void setFromIDL (asdmIDL::AntennaRowIDL x) ;
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

	std::map<std::string, AntennaAttributeFromBin> fromBinMethods;
void antennaIdFromBin( EndianIStream& eis);
void nameFromBin( EndianIStream& eis);
void antennaMakeFromBin( EndianIStream& eis);
void antennaTypeFromBin( EndianIStream& eis);
void dishDiameterFromBin( EndianIStream& eis);
void positionFromBin( EndianIStream& eis);
void offsetFromBin( EndianIStream& eis);
void timeFromBin( EndianIStream& eis);
void stationIdFromBin( EndianIStream& eis);

void assocAntennaIdFromBin( EndianIStream& eis);


	 /**
	  * Deserialize a stream of bytes read from an EndianIStream to build a PointingRow.
	  * @param eiss the EndianIStream to be read.
	  * @param table the AntennaTable to which the row built by deserialization will be parented.
	  * @param attributesSeq a vector containing the names of the attributes . The elements order defines the order 
	  * in which the attributes are written in the binary serialization.
	  */
	 static AntennaRow* fromBin(EndianIStream& eis, AntennaTable& table, const std::vector<std::string>& attributesSeq);	 
 
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
	AntennaTable &table;
	/**
	 * Whether this row has been added to the table or not.
	 */
	bool hasBeenAdded;

	// This method is used by the Table class when this row is added to the table.
	void isAdded(bool added);


	/**
	 * Create a AntennaRow.
	 * <p>
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 */ 
	AntennaRow (AntennaTable &table);

	/**
	 * Create a AntennaRow using a copy constructor mechanism.
	 * <p>
	 * Given a AntennaRow row and a AntennaTable table, the method creates a new
	 * AntennaRow owned by table. Each attribute of the created row is a copy (deep)
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
	 AntennaRow (AntennaTable &table, AntennaRow *row);
	 	
	////////////////////////////////
	// Intrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute antennaId
	
	

	Tag antennaId;

	
	
 	
 	/**
 	 * Set antennaId with the specified Tag value.
 	 * @param antennaId The Tag value to which antennaId is to be set.
		
 		
			
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 		
 	 */
 	void setAntennaId (Tag antennaId);
  		
	

	
	// ===> Attribute name
	
	

	std::string name;

	
	
 	

	
	// ===> Attribute antennaMake
	
	

	AntennaMakeMod::AntennaMake antennaMake;

	
	
 	

	
	// ===> Attribute antennaType
	
	

	AntennaTypeMod::AntennaType antennaType;

	
	
 	

	
	// ===> Attribute dishDiameter
	
	

	Length dishDiameter;

	
	
 	

	
	// ===> Attribute position
	
	

	std::vector<Length > position;

	
	
 	

	
	// ===> Attribute offset
	
	

	std::vector<Length > offset;

	
	
 	

	
	// ===> Attribute time
	
	

	ArrayTime time;

	
	
 	

	////////////////////////////////
	// Extrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute assocAntennaId, which is optional
	
	
	bool assocAntennaIdExists;
	

	Tag assocAntennaId;

	
	
 	

	
	// ===> Attribute stationId
	
	

	Tag stationId;

	
	
 	

	///////////
	// Links //
	///////////
	
		
		
	

	
		

	 

	

	
/*
	////////////////////////////////////////////////////////////
	// binary-deserialization material from an EndianIStream  //
	////////////////////////////////////////////////////////////
	std::map<std::string, AntennaAttributeFromBin> fromBinMethods;
void antennaIdFromBin( EndianIStream& eis);
void nameFromBin( EndianIStream& eis);
void antennaMakeFromBin( EndianIStream& eis);
void antennaTypeFromBin( EndianIStream& eis);
void dishDiameterFromBin( EndianIStream& eis);
void positionFromBin( EndianIStream& eis);
void offsetFromBin( EndianIStream& eis);
void timeFromBin( EndianIStream& eis);
void stationIdFromBin( EndianIStream& eis);

void assocAntennaIdFromBin( EndianIStream& eis);

*/
	
	///////////////////////////////////
	// text-deserialization material //
	///////////////////////////////////
	std::map<std::string, AntennaAttributeFromText> fromTextMethods;
	
void antennaIdFromText (const std::string & s);
	
	
void nameFromText (const std::string & s);
	
	
void antennaMakeFromText (const std::string & s);
	
	
void antennaTypeFromText (const std::string & s);
	
	
void dishDiameterFromText (const std::string & s);
	
	
void positionFromText (const std::string & s);
	
	
void offsetFromText (const std::string & s);
	
	
void timeFromText (const std::string & s);
	
	
void stationIdFromText (const std::string & s);
	

	
void assocAntennaIdFromText (const std::string & s);
	
	
	
	/**
	 * Serialize this into a stream of bytes written to an EndianOSStream.
	 * @param eoss the EndianOSStream to be written to
	 */
	 void toBin(EndianOSStream& eoss);
	 	 
	 /**
	  * Deserialize a stream of bytes read from an EndianIStream to build a PointingRow.
	  * @param eiss the EndianIStream to be read.
	  * @param table the AntennaTable to which the row built by deserialization will be parented.
	  * @param attributesSeq a vector containing the names of the attributes . The elements order defines the order 
	  * in which the attributes are written in the binary serialization.

	 static AntennaRow* fromBin(EndianIStream& eis, AntennaTable& table, const std::vector<std::string>& attributesSeq);	 
		*/
};

} // End namespace asdm

#endif /* Antenna_CLASS */
