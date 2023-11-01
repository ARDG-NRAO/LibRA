
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
 * File ScaleRow.h
 */
 
#ifndef ScaleRow_CLASS
#define ScaleRow_CLASS

#include <vector>
#include <string>
#include <set>

#ifndef WITHOUT_ACS
#include <asdmIDLC.h>
#endif






	 
#include <alma/ASDM/Tag.h>
	




	

	
#include <alma/Enumerations/CTimeScale.h>
	

	
#include <alma/Enumerations/CDataScale.h>
	

	
#include <alma/Enumerations/CDataScale.h>
	

	
#include <alma/Enumerations/CWeightType.h>
	



#include <alma/ASDM/ConversionException.h>
#include <alma/ASDM/NoSuchRow.h>
#include <alma/ASDM/IllegalAccessException.h>

#include <alma/ASDM/RowTransformer.h>
//#include <alma/ASDM/TableStreamReader.h>

/*\file Scale.h
    \brief Generated from model's revision "-1", branch ""
*/

namespace asdm {

//class asdm::ScaleTable;

	

class ScaleRow;
typedef void (ScaleRow::*ScaleAttributeFromBin) (EndianIStream& eis);
typedef void (ScaleRow::*ScaleAttributeFromText) (const std::string& s);

/**
 * The ScaleRow class is a row of a ScaleTable.
 * 
 * Generated from model's revision "-1", branch ""
 *
 */
class ScaleRow {
friend class asdm::ScaleTable;
friend class asdm::RowTransformer<ScaleRow>;
//friend class asdm::TableStreamReader<ScaleTable, ScaleRow>;

public:

	virtual ~ScaleRow();

	/**
	 * Return the table to which this row belongs.
	 */
	ScaleTable &getTable() const;
	
	/**
	 * Has this row been added to its table ?
	 * @return true if and only if it has been added.
	 */
	bool isAdded() const;
		
	////////////////////////////////
	// Intrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute scaleId
	
	
	

	
 	/**
 	 * Get scaleId.
 	 * @return scaleId as Tag
 	 */
 	Tag getScaleId() const;
	
 
 	
 	
	
	


	
	// ===> Attribute timeScale
	
	
	

	
 	/**
 	 * Get timeScale.
 	 * @return timeScale as TimeScaleMod::TimeScale
 	 */
 	TimeScaleMod::TimeScale getTimeScale() const;
	
 
 	
 	
 	/**
 	 * Set timeScale with the specified TimeScaleMod::TimeScale.
 	 * @param timeScale The TimeScaleMod::TimeScale value to which timeScale is to be set.
 	 
 		
 			
 	 */
 	void setTimeScale (TimeScaleMod::TimeScale timeScale);
  		
	
	
	


	
	// ===> Attribute crossDataScale
	
	
	

	
 	/**
 	 * Get crossDataScale.
 	 * @return crossDataScale as DataScaleMod::DataScale
 	 */
 	DataScaleMod::DataScale getCrossDataScale() const;
	
 
 	
 	
 	/**
 	 * Set crossDataScale with the specified DataScaleMod::DataScale.
 	 * @param crossDataScale The DataScaleMod::DataScale value to which crossDataScale is to be set.
 	 
 		
 			
 	 */
 	void setCrossDataScale (DataScaleMod::DataScale crossDataScale);
  		
	
	
	


	
	// ===> Attribute autoDataScale
	
	
	

	
 	/**
 	 * Get autoDataScale.
 	 * @return autoDataScale as DataScaleMod::DataScale
 	 */
 	DataScaleMod::DataScale getAutoDataScale() const;
	
 
 	
 	
 	/**
 	 * Set autoDataScale with the specified DataScaleMod::DataScale.
 	 * @param autoDataScale The DataScaleMod::DataScale value to which autoDataScale is to be set.
 	 
 		
 			
 	 */
 	void setAutoDataScale (DataScaleMod::DataScale autoDataScale);
  		
	
	
	


	
	// ===> Attribute weightType
	
	
	

	
 	/**
 	 * Get weightType.
 	 * @return weightType as WeightTypeMod::WeightType
 	 */
 	WeightTypeMod::WeightType getWeightType() const;
	
 
 	
 	
 	/**
 	 * Set weightType with the specified WeightTypeMod::WeightType.
 	 * @param weightType The WeightTypeMod::WeightType value to which weightType is to be set.
 	 
 		
 			
 	 */
 	void setWeightType (WeightTypeMod::WeightType weightType);
  		
	
	
	


	////////////////////////////////
	// Extrinsic Table Attributes //
	////////////////////////////////
	
	///////////
	// Links //
	///////////
	
	
	
	
	/**
	 * Compare each mandatory attribute except the autoincrementable one of this ScaleRow with 
	 * the corresponding parameters and return true if there is a match and false otherwise.
	 	
	 * @param timeScale
	    
	 * @param crossDataScale
	    
	 * @param autoDataScale
	    
	 * @param weightType
	    
	 */ 
	bool compareNoAutoInc(TimeScaleMod::TimeScale timeScale, DataScaleMod::DataScale crossDataScale, DataScaleMod::DataScale autoDataScale, WeightTypeMod::WeightType weightType);
	
	

	
	/**
	 * Compare each mandatory value (i.e. not in the key) attribute  with 
	 * the corresponding parameters and return true if there is a match and false otherwise.
	 	
	 * @param timeScale
	    
	 * @param crossDataScale
	    
	 * @param autoDataScale
	    
	 * @param weightType
	    
	 */ 
	bool compareRequiredValue(TimeScaleMod::TimeScale timeScale, DataScaleMod::DataScale crossDataScale, DataScaleMod::DataScale autoDataScale, WeightTypeMod::WeightType weightType); 
		 
	
	/**
	 * Return true if all required attributes of the value part are equal to their homologues
	 * in x and false otherwise.
	 *
	 * @param x a pointer on the ScaleRow whose required attributes of the value part 
	 * will be compared with those of this.
	 * @return a boolean.
	 */
	bool equalByRequiredValue(ScaleRow* x) ;
	
#ifndef WITHOUT_ACS
	/**
	 * Return this row in the form of an IDL struct.
	 * @return The values of this row as a ScaleRowIDL struct.
	 */
	asdmIDL::ScaleRowIDL *toIDL() const;
	
	/**
	 * Define the content of a ScaleRowIDL struct from the values
	 * found in this row.
	 *
	 * @param x a reference to the ScaleRowIDL struct to be set.
	 *
	 */
	 void toIDL(asdmIDL::ScaleRowIDL& x) const;
#endif
	
#ifndef WITHOUT_ACS
	/**
	 * Fill the values of this row from the IDL struct ScaleRowIDL.
	 * @param x The IDL struct containing the values used to fill this row.
	 * @throws ConversionException
	 */
	void setFromIDL (asdmIDL::ScaleRowIDL x) ;
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

	std::map<std::string, ScaleAttributeFromBin> fromBinMethods;
void scaleIdFromBin( EndianIStream& eis);
void timeScaleFromBin( EndianIStream& eis);
void crossDataScaleFromBin( EndianIStream& eis);
void autoDataScaleFromBin( EndianIStream& eis);
void weightTypeFromBin( EndianIStream& eis);

	

	 /**
	  * Deserialize a stream of bytes read from an EndianIStream to build a PointingRow.
	  * @param eiss the EndianIStream to be read.
	  * @param table the ScaleTable to which the row built by deserialization will be parented.
	  * @param attributesSeq a vector containing the names of the attributes . The elements order defines the order 
	  * in which the attributes are written in the binary serialization.
	  */
	 static ScaleRow* fromBin(EndianIStream& eis, ScaleTable& table, const std::vector<std::string>& attributesSeq);	 
 
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
	ScaleTable &table;
	/**
	 * Whether this row has been added to the table or not.
	 */
	bool hasBeenAdded;

	// This method is used by the Table class when this row is added to the table.
	void isAdded(bool added);


	/**
	 * Create a ScaleRow.
	 * <p>
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 */ 
	ScaleRow (ScaleTable &table);

	/**
	 * Create a ScaleRow using a copy constructor mechanism.
	 * <p>
	 * Given a ScaleRow row and a ScaleTable table, the method creates a new
	 * ScaleRow owned by table. Each attribute of the created row is a copy (deep)
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
	 ScaleRow (ScaleTable &table, ScaleRow *row);
	 	
	////////////////////////////////
	// Intrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute scaleId
	
	

	Tag scaleId;

	
	
 	
 	/**
 	 * Set scaleId with the specified Tag value.
 	 * @param scaleId The Tag value to which scaleId is to be set.
		
 		
			
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 		
 	 */
 	void setScaleId (Tag scaleId);
  		
	

	
	// ===> Attribute timeScale
	
	

	TimeScaleMod::TimeScale timeScale;

	
	
 	

	
	// ===> Attribute crossDataScale
	
	

	DataScaleMod::DataScale crossDataScale;

	
	
 	

	
	// ===> Attribute autoDataScale
	
	

	DataScaleMod::DataScale autoDataScale;

	
	
 	

	
	// ===> Attribute weightType
	
	

	WeightTypeMod::WeightType weightType;

	
	
 	

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
	std::map<std::string, ScaleAttributeFromBin> fromBinMethods;
void scaleIdFromBin( EndianIStream& eis);
void timeScaleFromBin( EndianIStream& eis);
void crossDataScaleFromBin( EndianIStream& eis);
void autoDataScaleFromBin( EndianIStream& eis);
void weightTypeFromBin( EndianIStream& eis);

	
*/
	
	///////////////////////////////////
	// text-deserialization material //
	///////////////////////////////////
	std::map<std::string, ScaleAttributeFromText> fromTextMethods;
	
void scaleIdFromText (const std::string & s);
	
	
void timeScaleFromText (const std::string & s);
	
	
void crossDataScaleFromText (const std::string & s);
	
	
void autoDataScaleFromText (const std::string & s);
	
	
void weightTypeFromText (const std::string & s);
	

		
	
	/**
	 * Serialize this into a stream of bytes written to an EndianOSStream.
	 * @param eoss the EndianOSStream to be written to
	 */
	 void toBin(EndianOSStream& eoss);
	 	 
	 /**
	  * Deserialize a stream of bytes read from an EndianIStream to build a PointingRow.
	  * @param eiss the EndianIStream to be read.
	  * @param table the ScaleTable to which the row built by deserialization will be parented.
	  * @param attributesSeq a vector containing the names of the attributes . The elements order defines the order 
	  * in which the attributes are written in the binary serialization.

	 static ScaleRow* fromBin(EndianIStream& eis, ScaleTable& table, const std::vector<std::string>& attributesSeq);	 
		*/
};

} // End namespace asdm

#endif /* Scale_CLASS */
