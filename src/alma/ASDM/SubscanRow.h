
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
 * File SubscanRow.h
 */
 
#ifndef SubscanRow_CLASS
#define SubscanRow_CLASS

#include <vector>
#include <string>
#include <set>

#ifndef WITHOUT_ACS
#include <asdmIDLC.h>
#endif






	 
#include <alma/ASDM/ArrayTime.h>
	

	 
#include <alma/ASDM/Tag.h>
	




	

	

	

	

	

	
#include <alma/Enumerations/CSubscanIntent.h>
	

	
#include <alma/Enumerations/CSwitchingMode.h>
	

	

	

	
#include <alma/Enumerations/CCorrelatorCalibration.h>
	



#include <alma/ASDM/ConversionException.h>
#include <alma/ASDM/NoSuchRow.h>
#include <alma/ASDM/IllegalAccessException.h>

#include <alma/ASDM/RowTransformer.h>
//#include <alma/ASDM/TableStreamReader.h>

/*\file Subscan.h
    \brief Generated from model's revision "-1", branch ""
*/

namespace asdm {

//class asdm::SubscanTable;


// class asdm::ExecBlockRow;
class ExecBlockRow;
	

class SubscanRow;
typedef void (SubscanRow::*SubscanAttributeFromBin) (EndianIStream& eis);
typedef void (SubscanRow::*SubscanAttributeFromText) (const std::string& s);

/**
 * The SubscanRow class is a row of a SubscanTable.
 * 
 * Generated from model's revision "-1", branch ""
 *
 */
class SubscanRow {
friend class asdm::SubscanTable;
friend class asdm::RowTransformer<SubscanRow>;
//friend class asdm::TableStreamReader<SubscanTable, SubscanRow>;

public:

	virtual ~SubscanRow();

	/**
	 * Return the table to which this row belongs.
	 */
	SubscanTable &getTable() const;
	
	/**
	 * Has this row been added to its table ?
	 * @return true if and only if it has been added.
	 */
	bool isAdded() const;
		
	////////////////////////////////
	// Intrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute scanNumber
	
	
	

	
 	/**
 	 * Get scanNumber.
 	 * @return scanNumber as int
 	 */
 	int getScanNumber() const;
	
 
 	
 	
 	/**
 	 * Set scanNumber with the specified int.
 	 * @param scanNumber The int value to which scanNumber is to be set.
 	 
 		
 			
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 		
 	 */
 	void setScanNumber (int scanNumber);
  		
	
	
	


	
	// ===> Attribute subscanNumber
	
	
	

	
 	/**
 	 * Get subscanNumber.
 	 * @return subscanNumber as int
 	 */
 	int getSubscanNumber() const;
	
 
 	
 	
 	/**
 	 * Set subscanNumber with the specified int.
 	 * @param subscanNumber The int value to which subscanNumber is to be set.
 	 
 		
 			
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 		
 	 */
 	void setSubscanNumber (int subscanNumber);
  		
	
	
	


	
	// ===> Attribute startTime
	
	
	

	
 	/**
 	 * Get startTime.
 	 * @return startTime as ArrayTime
 	 */
 	ArrayTime getStartTime() const;
	
 
 	
 	
 	/**
 	 * Set startTime with the specified ArrayTime.
 	 * @param startTime The ArrayTime value to which startTime is to be set.
 	 
 		
 			
 	 */
 	void setStartTime (ArrayTime startTime);
  		
	
	
	


	
	// ===> Attribute endTime
	
	
	

	
 	/**
 	 * Get endTime.
 	 * @return endTime as ArrayTime
 	 */
 	ArrayTime getEndTime() const;
	
 
 	
 	
 	/**
 	 * Set endTime with the specified ArrayTime.
 	 * @param endTime The ArrayTime value to which endTime is to be set.
 	 
 		
 			
 	 */
 	void setEndTime (ArrayTime endTime);
  		
	
	
	


	
	// ===> Attribute fieldName
	
	
	

	
 	/**
 	 * Get fieldName.
 	 * @return fieldName as std::string
 	 */
 	std::string getFieldName() const;
	
 
 	
 	
 	/**
 	 * Set fieldName with the specified std::string.
 	 * @param fieldName The std::string value to which fieldName is to be set.
 	 
 		
 			
 	 */
 	void setFieldName (std::string fieldName);
  		
	
	
	


	
	// ===> Attribute subscanIntent
	
	
	

	
 	/**
 	 * Get subscanIntent.
 	 * @return subscanIntent as SubscanIntentMod::SubscanIntent
 	 */
 	SubscanIntentMod::SubscanIntent getSubscanIntent() const;
	
 
 	
 	
 	/**
 	 * Set subscanIntent with the specified SubscanIntentMod::SubscanIntent.
 	 * @param subscanIntent The SubscanIntentMod::SubscanIntent value to which subscanIntent is to be set.
 	 
 		
 			
 	 */
 	void setSubscanIntent (SubscanIntentMod::SubscanIntent subscanIntent);
  		
	
	
	


	
	// ===> Attribute subscanMode, which is optional
	
	
	
	/**
	 * The attribute subscanMode is optional. Return true if this attribute exists.
	 * @return true if and only if the subscanMode attribute exists. 
	 */
	bool isSubscanModeExists() const;
	

	
 	/**
 	 * Get subscanMode, which is optional.
 	 * @return subscanMode as SwitchingModeMod::SwitchingMode
 	 * @throws IllegalAccessException If subscanMode does not exist.
 	 */
 	SwitchingModeMod::SwitchingMode getSubscanMode() const;
	
 
 	
 	
 	/**
 	 * Set subscanMode with the specified SwitchingModeMod::SwitchingMode.
 	 * @param subscanMode The SwitchingModeMod::SwitchingMode value to which subscanMode is to be set.
 	 
 		
 	 */
 	void setSubscanMode (SwitchingModeMod::SwitchingMode subscanMode);
		
	
	
	
	/**
	 * Mark subscanMode, which is an optional field, as non-existent.
	 */
	void clearSubscanMode ();
	


	
	// ===> Attribute numIntegration
	
	
	

	
 	/**
 	 * Get numIntegration.
 	 * @return numIntegration as int
 	 */
 	int getNumIntegration() const;
	
 
 	
 	
 	/**
 	 * Set numIntegration with the specified int.
 	 * @param numIntegration The int value to which numIntegration is to be set.
 	 
 		
 			
 	 */
 	void setNumIntegration (int numIntegration);
  		
	
	
	


	
	// ===> Attribute numSubintegration
	
	
	

	
 	/**
 	 * Get numSubintegration.
 	 * @return numSubintegration as std::vector<int >
 	 */
 	std::vector<int > getNumSubintegration() const;
	
 
 	
 	
 	/**
 	 * Set numSubintegration with the specified std::vector<int >.
 	 * @param numSubintegration The std::vector<int > value to which numSubintegration is to be set.
 	 
 		
 			
 	 */
 	void setNumSubintegration (std::vector<int > numSubintegration);
  		
	
	
	


	
	// ===> Attribute correlatorCalibration, which is optional
	
	
	
	/**
	 * The attribute correlatorCalibration is optional. Return true if this attribute exists.
	 * @return true if and only if the correlatorCalibration attribute exists. 
	 */
	bool isCorrelatorCalibrationExists() const;
	

	
 	/**
 	 * Get correlatorCalibration, which is optional.
 	 * @return correlatorCalibration as CorrelatorCalibrationMod::CorrelatorCalibration
 	 * @throws IllegalAccessException If correlatorCalibration does not exist.
 	 */
 	CorrelatorCalibrationMod::CorrelatorCalibration getCorrelatorCalibration() const;
	
 
 	
 	
 	/**
 	 * Set correlatorCalibration with the specified CorrelatorCalibrationMod::CorrelatorCalibration.
 	 * @param correlatorCalibration The CorrelatorCalibrationMod::CorrelatorCalibration value to which correlatorCalibration is to be set.
 	 
 		
 	 */
 	void setCorrelatorCalibration (CorrelatorCalibrationMod::CorrelatorCalibration correlatorCalibration);
		
	
	
	
	/**
	 * Mark correlatorCalibration, which is an optional field, as non-existent.
	 */
	void clearCorrelatorCalibration ();
	


	////////////////////////////////
	// Extrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute execBlockId
	
	
	

	
 	/**
 	 * Get execBlockId.
 	 * @return execBlockId as Tag
 	 */
 	Tag getExecBlockId() const;
	
 
 	
 	
 	/**
 	 * Set execBlockId with the specified Tag.
 	 * @param execBlockId The Tag value to which execBlockId is to be set.
 	 
 		
 			
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 		
 	 */
 	void setExecBlockId (Tag execBlockId);
  		
	
	
	


	///////////
	// Links //
	///////////
	
	

	
		
	/**
	 * execBlockId pointer to the row in the ExecBlock table having ExecBlock.execBlockId == execBlockId
	 * @return a ExecBlockRow*
	 * 
	 
	 */
	 ExecBlockRow* getExecBlockUsingExecBlockId();
	 

	

	
	
	
	/**
	 * Compare each mandatory attribute except the autoincrementable one of this SubscanRow with 
	 * the corresponding parameters and return true if there is a match and false otherwise.
	 	
	 * @param execBlockId
	    
	 * @param scanNumber
	    
	 * @param subscanNumber
	    
	 * @param startTime
	    
	 * @param endTime
	    
	 * @param fieldName
	    
	 * @param subscanIntent
	    
	 * @param numIntegration
	    
	 * @param numSubintegration
	    
	 */ 
	bool compareNoAutoInc(Tag execBlockId, int scanNumber, int subscanNumber, ArrayTime startTime, ArrayTime endTime, std::string fieldName, SubscanIntentMod::SubscanIntent subscanIntent, int numIntegration, std::vector<int > numSubintegration);
	
	

	
	/**
	 * Compare each mandatory value (i.e. not in the key) attribute  with 
	 * the corresponding parameters and return true if there is a match and false otherwise.
	 	
	 * @param startTime
	    
	 * @param endTime
	    
	 * @param fieldName
	    
	 * @param subscanIntent
	    
	 * @param numIntegration
	    
	 * @param numSubintegration
	    
	 */ 
	bool compareRequiredValue(ArrayTime startTime, ArrayTime endTime, std::string fieldName, SubscanIntentMod::SubscanIntent subscanIntent, int numIntegration, std::vector<int > numSubintegration); 
		 
	
	/**
	 * Return true if all required attributes of the value part are equal to their homologues
	 * in x and false otherwise.
	 *
	 * @param x a pointer on the SubscanRow whose required attributes of the value part 
	 * will be compared with those of this.
	 * @return a boolean.
	 */
	bool equalByRequiredValue(SubscanRow* x) ;
	
#ifndef WITHOUT_ACS
	/**
	 * Return this row in the form of an IDL struct.
	 * @return The values of this row as a SubscanRowIDL struct.
	 */
	asdmIDL::SubscanRowIDL *toIDL() const;
	
	/**
	 * Define the content of a SubscanRowIDL struct from the values
	 * found in this row.
	 *
	 * @param x a reference to the SubscanRowIDL struct to be set.
	 *
	 */
	 void toIDL(asdmIDL::SubscanRowIDL& x) const;
#endif
	
#ifndef WITHOUT_ACS
	/**
	 * Fill the values of this row from the IDL struct SubscanRowIDL.
	 * @param x The IDL struct containing the values used to fill this row.
	 * @throws ConversionException
	 */
	void setFromIDL (asdmIDL::SubscanRowIDL x) ;
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

	std::map<std::string, SubscanAttributeFromBin> fromBinMethods;
void execBlockIdFromBin( EndianIStream& eis);
void scanNumberFromBin( EndianIStream& eis);
void subscanNumberFromBin( EndianIStream& eis);
void startTimeFromBin( EndianIStream& eis);
void endTimeFromBin( EndianIStream& eis);
void fieldNameFromBin( EndianIStream& eis);
void subscanIntentFromBin( EndianIStream& eis);
void numIntegrationFromBin( EndianIStream& eis);
void numSubintegrationFromBin( EndianIStream& eis);

void subscanModeFromBin( EndianIStream& eis);
void correlatorCalibrationFromBin( EndianIStream& eis);


	 /**
	  * Deserialize a stream of bytes read from an EndianIStream to build a PointingRow.
	  * @param eiss the EndianIStream to be read.
	  * @param table the SubscanTable to which the row built by deserialization will be parented.
	  * @param attributesSeq a vector containing the names of the attributes . The elements order defines the order 
	  * in which the attributes are written in the binary serialization.
	  */
	 static SubscanRow* fromBin(EndianIStream& eis, SubscanTable& table, const std::vector<std::string>& attributesSeq);	 
 
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
	SubscanTable &table;
	/**
	 * Whether this row has been added to the table or not.
	 */
	bool hasBeenAdded;

	// This method is used by the Table class when this row is added to the table.
	void isAdded(bool added);


	/**
	 * Create a SubscanRow.
	 * <p>
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 */ 
	SubscanRow (SubscanTable &table);

	/**
	 * Create a SubscanRow using a copy constructor mechanism.
	 * <p>
	 * Given a SubscanRow row and a SubscanTable table, the method creates a new
	 * SubscanRow owned by table. Each attribute of the created row is a copy (deep)
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
	 SubscanRow (SubscanTable &table, SubscanRow *row);
	 	
	////////////////////////////////
	// Intrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute scanNumber
	
	

	int scanNumber;

	
	
 	

	
	// ===> Attribute subscanNumber
	
	

	int subscanNumber;

	
	
 	

	
	// ===> Attribute startTime
	
	

	ArrayTime startTime;

	
	
 	

	
	// ===> Attribute endTime
	
	

	ArrayTime endTime;

	
	
 	

	
	// ===> Attribute fieldName
	
	

	std::string fieldName;

	
	
 	

	
	// ===> Attribute subscanIntent
	
	

	SubscanIntentMod::SubscanIntent subscanIntent;

	
	
 	

	
	// ===> Attribute subscanMode, which is optional
	
	
	bool subscanModeExists;
	

	SwitchingModeMod::SwitchingMode subscanMode;

	
	
 	

	
	// ===> Attribute numIntegration
	
	

	int numIntegration;

	
	
 	

	
	// ===> Attribute numSubintegration
	
	

	std::vector<int > numSubintegration;

	
	
 	

	
	// ===> Attribute correlatorCalibration, which is optional
	
	
	bool correlatorCalibrationExists;
	

	CorrelatorCalibrationMod::CorrelatorCalibration correlatorCalibration;

	
	
 	

	////////////////////////////////
	// Extrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute execBlockId
	
	

	Tag execBlockId;

	
	
 	

	///////////
	// Links //
	///////////
	
	
		

	 

	

	
/*
	////////////////////////////////////////////////////////////
	// binary-deserialization material from an EndianIStream  //
	////////////////////////////////////////////////////////////
	std::map<std::string, SubscanAttributeFromBin> fromBinMethods;
void execBlockIdFromBin( EndianIStream& eis);
void scanNumberFromBin( EndianIStream& eis);
void subscanNumberFromBin( EndianIStream& eis);
void startTimeFromBin( EndianIStream& eis);
void endTimeFromBin( EndianIStream& eis);
void fieldNameFromBin( EndianIStream& eis);
void subscanIntentFromBin( EndianIStream& eis);
void numIntegrationFromBin( EndianIStream& eis);
void numSubintegrationFromBin( EndianIStream& eis);

void subscanModeFromBin( EndianIStream& eis);
void correlatorCalibrationFromBin( EndianIStream& eis);

*/
	
	///////////////////////////////////
	// text-deserialization material //
	///////////////////////////////////
	std::map<std::string, SubscanAttributeFromText> fromTextMethods;
	
void execBlockIdFromText (const std::string & s);
	
	
void scanNumberFromText (const std::string & s);
	
	
void subscanNumberFromText (const std::string & s);
	
	
void startTimeFromText (const std::string & s);
	
	
void endTimeFromText (const std::string & s);
	
	
void fieldNameFromText (const std::string & s);
	
	
void subscanIntentFromText (const std::string & s);
	
	
void numIntegrationFromText (const std::string & s);
	
	
void numSubintegrationFromText (const std::string & s);
	

	
void subscanModeFromText (const std::string & s);
	
	
void correlatorCalibrationFromText (const std::string & s);
	
	
	
	/**
	 * Serialize this into a stream of bytes written to an EndianOSStream.
	 * @param eoss the EndianOSStream to be written to
	 */
	 void toBin(EndianOSStream& eoss);
	 	 
	 /**
	  * Deserialize a stream of bytes read from an EndianIStream to build a PointingRow.
	  * @param eiss the EndianIStream to be read.
	  * @param table the SubscanTable to which the row built by deserialization will be parented.
	  * @param attributesSeq a vector containing the names of the attributes . The elements order defines the order 
	  * in which the attributes are written in the binary serialization.

	 static SubscanRow* fromBin(EndianIStream& eis, SubscanTable& table, const std::vector<std::string>& attributesSeq);	 
		*/
};

} // End namespace asdm

#endif /* Subscan_CLASS */
