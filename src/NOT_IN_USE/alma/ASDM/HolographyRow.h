
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
 * File HolographyRow.h
 */
 
#ifndef HolographyRow_CLASS
#define HolographyRow_CLASS

#include <vector>
#include <string>
#include <set>

#ifndef WITHOUT_ACS
#include <asdmIDLC.h>
#endif






	 
#include <alma/ASDM/Length.h>
	

	 
#include <alma/ASDM/Tag.h>
	




	

	

	

	

	
#include <alma/Enumerations/CHolographyChannelType.h>
	



#include <alma/ASDM/ConversionException.h>
#include <alma/ASDM/NoSuchRow.h>
#include <alma/ASDM/IllegalAccessException.h>

#include <alma/ASDM/RowTransformer.h>
//#include <alma/ASDM/TableStreamReader.h>

/*\file Holography.h
    \brief Generated from model's revision "-1", branch ""
*/

namespace asdm {

//class asdm::HolographyTable;

	

class HolographyRow;
typedef void (HolographyRow::*HolographyAttributeFromBin) (EndianIStream& eis);
typedef void (HolographyRow::*HolographyAttributeFromText) (const std::string& s);

/**
 * The HolographyRow class is a row of a HolographyTable.
 * 
 * Generated from model's revision "-1", branch ""
 *
 */
class HolographyRow {
friend class asdm::HolographyTable;
friend class asdm::RowTransformer<HolographyRow>;
//friend class asdm::TableStreamReader<HolographyTable, HolographyRow>;

public:

	virtual ~HolographyRow();

	/**
	 * Return the table to which this row belongs.
	 */
	HolographyTable &getTable() const;
	
	/**
	 * Has this row been added to its table ?
	 * @return true if and only if it has been added.
	 */
	bool isAdded() const;
		
	////////////////////////////////
	// Intrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute holographyId
	
	
	

	
 	/**
 	 * Get holographyId.
 	 * @return holographyId as Tag
 	 */
 	Tag getHolographyId() const;
	
 
 	
 	
	
	


	
	// ===> Attribute distance
	
	
	

	
 	/**
 	 * Get distance.
 	 * @return distance as Length
 	 */
 	Length getDistance() const;
	
 
 	
 	
 	/**
 	 * Set distance with the specified Length.
 	 * @param distance The Length value to which distance is to be set.
 	 
 		
 			
 	 */
 	void setDistance (Length distance);
  		
	
	
	


	
	// ===> Attribute focus
	
	
	

	
 	/**
 	 * Get focus.
 	 * @return focus as Length
 	 */
 	Length getFocus() const;
	
 
 	
 	
 	/**
 	 * Set focus with the specified Length.
 	 * @param focus The Length value to which focus is to be set.
 	 
 		
 			
 	 */
 	void setFocus (Length focus);
  		
	
	
	


	
	// ===> Attribute numCorr
	
	
	

	
 	/**
 	 * Get numCorr.
 	 * @return numCorr as int
 	 */
 	int getNumCorr() const;
	
 
 	
 	
 	/**
 	 * Set numCorr with the specified int.
 	 * @param numCorr The int value to which numCorr is to be set.
 	 
 		
 			
 	 */
 	void setNumCorr (int numCorr);
  		
	
	
	


	
	// ===> Attribute type
	
	
	

	
 	/**
 	 * Get type.
 	 * @return type as std::vector<HolographyChannelTypeMod::HolographyChannelType >
 	 */
 	std::vector<HolographyChannelTypeMod::HolographyChannelType > getType() const;
	
 
 	
 	
 	/**
 	 * Set type with the specified std::vector<HolographyChannelTypeMod::HolographyChannelType >.
 	 * @param type The std::vector<HolographyChannelTypeMod::HolographyChannelType > value to which type is to be set.
 	 
 		
 			
 	 */
 	void setType (std::vector<HolographyChannelTypeMod::HolographyChannelType > type);
  		
	
	
	


	////////////////////////////////
	// Extrinsic Table Attributes //
	////////////////////////////////
	
	///////////
	// Links //
	///////////
	
	
	
	
	/**
	 * Compare each mandatory attribute except the autoincrementable one of this HolographyRow with 
	 * the corresponding parameters and return true if there is a match and false otherwise.
	 	
	 * @param distance
	    
	 * @param focus
	    
	 * @param numCorr
	    
	 * @param type
	    
	 */ 
	bool compareNoAutoInc(Length distance, Length focus, int numCorr, std::vector<HolographyChannelTypeMod::HolographyChannelType > type);
	
	

	
	/**
	 * Compare each mandatory value (i.e. not in the key) attribute  with 
	 * the corresponding parameters and return true if there is a match and false otherwise.
	 	
	 * @param distance
	    
	 * @param focus
	    
	 * @param numCorr
	    
	 * @param type
	    
	 */ 
	bool compareRequiredValue(Length distance, Length focus, int numCorr, std::vector<HolographyChannelTypeMod::HolographyChannelType > type); 
		 
	
	/**
	 * Return true if all required attributes of the value part are equal to their homologues
	 * in x and false otherwise.
	 *
	 * @param x a pointer on the HolographyRow whose required attributes of the value part 
	 * will be compared with those of this.
	 * @return a boolean.
	 */
	bool equalByRequiredValue(HolographyRow* x) ;
	
#ifndef WITHOUT_ACS
	/**
	 * Return this row in the form of an IDL struct.
	 * @return The values of this row as a HolographyRowIDL struct.
	 */
	asdmIDL::HolographyRowIDL *toIDL() const;
	
	/**
	 * Define the content of a HolographyRowIDL struct from the values
	 * found in this row.
	 *
	 * @param x a reference to the HolographyRowIDL struct to be set.
	 *
	 */
	 void toIDL(asdmIDL::HolographyRowIDL& x) const;
#endif
	
#ifndef WITHOUT_ACS
	/**
	 * Fill the values of this row from the IDL struct HolographyRowIDL.
	 * @param x The IDL struct containing the values used to fill this row.
	 * @throws ConversionException
	 */
	void setFromIDL (asdmIDL::HolographyRowIDL x) ;
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

	std::map<std::string, HolographyAttributeFromBin> fromBinMethods;
void holographyIdFromBin( EndianIStream& eis);
void distanceFromBin( EndianIStream& eis);
void focusFromBin( EndianIStream& eis);
void numCorrFromBin( EndianIStream& eis);
void typeFromBin( EndianIStream& eis);

	

	 /**
	  * Deserialize a stream of bytes read from an EndianIStream to build a PointingRow.
	  * @param eiss the EndianIStream to be read.
	  * @param table the HolographyTable to which the row built by deserialization will be parented.
	  * @param attributesSeq a vector containing the names of the attributes . The elements order defines the order 
	  * in which the attributes are written in the binary serialization.
	  */
	 static HolographyRow* fromBin(EndianIStream& eis, HolographyTable& table, const std::vector<std::string>& attributesSeq);	 
 
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
	HolographyTable &table;
	/**
	 * Whether this row has been added to the table or not.
	 */
	bool hasBeenAdded;

	// This method is used by the Table class when this row is added to the table.
	void isAdded(bool added);


	/**
	 * Create a HolographyRow.
	 * <p>
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 */ 
	HolographyRow (HolographyTable &table);

	/**
	 * Create a HolographyRow using a copy constructor mechanism.
	 * <p>
	 * Given a HolographyRow row and a HolographyTable table, the method creates a new
	 * HolographyRow owned by table. Each attribute of the created row is a copy (deep)
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
	 HolographyRow (HolographyTable &table, HolographyRow *row);
	 	
	////////////////////////////////
	// Intrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute holographyId
	
	

	Tag holographyId;

	
	
 	
 	/**
 	 * Set holographyId with the specified Tag value.
 	 * @param holographyId The Tag value to which holographyId is to be set.
		
 		
			
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 		
 	 */
 	void setHolographyId (Tag holographyId);
  		
	

	
	// ===> Attribute distance
	
	

	Length distance;

	
	
 	

	
	// ===> Attribute focus
	
	

	Length focus;

	
	
 	

	
	// ===> Attribute numCorr
	
	

	int numCorr;

	
	
 	

	
	// ===> Attribute type
	
	

	std::vector<HolographyChannelTypeMod::HolographyChannelType > type;

	
	
 	

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
	std::map<std::string, HolographyAttributeFromBin> fromBinMethods;
void holographyIdFromBin( EndianIStream& eis);
void distanceFromBin( EndianIStream& eis);
void focusFromBin( EndianIStream& eis);
void numCorrFromBin( EndianIStream& eis);
void typeFromBin( EndianIStream& eis);

	
*/
	
	///////////////////////////////////
	// text-deserialization material //
	///////////////////////////////////
	std::map<std::string, HolographyAttributeFromText> fromTextMethods;
	
void holographyIdFromText (const std::string & s);
	
	
void distanceFromText (const std::string & s);
	
	
void focusFromText (const std::string & s);
	
	
void numCorrFromText (const std::string & s);
	
	
void typeFromText (const std::string & s);
	

		
	
	/**
	 * Serialize this into a stream of bytes written to an EndianOSStream.
	 * @param eoss the EndianOSStream to be written to
	 */
	 void toBin(EndianOSStream& eoss);
	 	 
	 /**
	  * Deserialize a stream of bytes read from an EndianIStream to build a PointingRow.
	  * @param eiss the EndianIStream to be read.
	  * @param table the HolographyTable to which the row built by deserialization will be parented.
	  * @param attributesSeq a vector containing the names of the attributes . The elements order defines the order 
	  * in which the attributes are written in the binary serialization.

	 static HolographyRow* fromBin(EndianIStream& eis, HolographyTable& table, const std::vector<std::string>& attributesSeq);	 
		*/
};

} // End namespace asdm

#endif /* Holography_CLASS */
