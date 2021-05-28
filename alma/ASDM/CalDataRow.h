
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
 * File CalDataRow.h
 */
 
#ifndef CalDataRow_CLASS
#define CalDataRow_CLASS

#include <vector>
#include <string>
#include <set>

#ifndef WITHOUT_ACS
#include <asdmIDLC.h>
#endif






	 
#include <alma/ASDM/ArrayTime.h>
	

	 
#include <alma/ASDM/Tag.h>
	

	 
#include <alma/ASDM/EntityRef.h>
	




	

	

	

	

	
#include <alma/Enumerations/CCalDataOrigin.h>
	

	
#include <alma/Enumerations/CCalType.h>
	

	

	

	

	
#include <alma/Enumerations/CAssociatedCalNature.h>
	

	

	

	

	
#include <alma/Enumerations/CScanIntent.h>
	



#include <alma/ASDM/ConversionException.h>
#include <alma/ASDM/NoSuchRow.h>
#include <alma/ASDM/IllegalAccessException.h>

#include <alma/ASDM/RowTransformer.h>
//#include <alma/ASDM/TableStreamReader.h>

/*\file CalData.h
    \brief Generated from model's revision "-1", branch ""
*/

namespace asdm {

//class asdm::CalDataTable;

	

class CalDataRow;
typedef void (CalDataRow::*CalDataAttributeFromBin) (EndianIStream& eis);
typedef void (CalDataRow::*CalDataAttributeFromText) (const std::string& s);

/**
 * The CalDataRow class is a row of a CalDataTable.
 * 
 * Generated from model's revision "-1", branch ""
 *
 */
class CalDataRow {
friend class asdm::CalDataTable;
friend class asdm::RowTransformer<CalDataRow>;
//friend class asdm::TableStreamReader<CalDataTable, CalDataRow>;

public:

	virtual ~CalDataRow();

	/**
	 * Return the table to which this row belongs.
	 */
	CalDataTable &getTable() const;
	
	/**
	 * Has this row been added to its table ?
	 * @return true if and only if it has been added.
	 */
	bool isAdded() const;
		
	////////////////////////////////
	// Intrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute calDataId
	
	
	

	
 	/**
 	 * Get calDataId.
 	 * @return calDataId as Tag
 	 */
 	Tag getCalDataId() const;
	
 
 	
 	
	
	


	
	// ===> Attribute startTimeObserved
	
	
	

	
 	/**
 	 * Get startTimeObserved.
 	 * @return startTimeObserved as ArrayTime
 	 */
 	ArrayTime getStartTimeObserved() const;
	
 
 	
 	
 	/**
 	 * Set startTimeObserved with the specified ArrayTime.
 	 * @param startTimeObserved The ArrayTime value to which startTimeObserved is to be set.
 	 
 		
 			
 	 */
 	void setStartTimeObserved (ArrayTime startTimeObserved);
  		
	
	
	


	
	// ===> Attribute endTimeObserved
	
	
	

	
 	/**
 	 * Get endTimeObserved.
 	 * @return endTimeObserved as ArrayTime
 	 */
 	ArrayTime getEndTimeObserved() const;
	
 
 	
 	
 	/**
 	 * Set endTimeObserved with the specified ArrayTime.
 	 * @param endTimeObserved The ArrayTime value to which endTimeObserved is to be set.
 	 
 		
 			
 	 */
 	void setEndTimeObserved (ArrayTime endTimeObserved);
  		
	
	
	


	
	// ===> Attribute execBlockUID
	
	
	

	
 	/**
 	 * Get execBlockUID.
 	 * @return execBlockUID as EntityRef
 	 */
 	EntityRef getExecBlockUID() const;
	
 
 	
 	
 	/**
 	 * Set execBlockUID with the specified EntityRef.
 	 * @param execBlockUID The EntityRef value to which execBlockUID is to be set.
 	 
 		
 			
 	 */
 	void setExecBlockUID (EntityRef execBlockUID);
  		
	
	
	


	
	// ===> Attribute calDataType
	
	
	

	
 	/**
 	 * Get calDataType.
 	 * @return calDataType as CalDataOriginMod::CalDataOrigin
 	 */
 	CalDataOriginMod::CalDataOrigin getCalDataType() const;
	
 
 	
 	
 	/**
 	 * Set calDataType with the specified CalDataOriginMod::CalDataOrigin.
 	 * @param calDataType The CalDataOriginMod::CalDataOrigin value to which calDataType is to be set.
 	 
 		
 			
 	 */
 	void setCalDataType (CalDataOriginMod::CalDataOrigin calDataType);
  		
	
	
	


	
	// ===> Attribute calType
	
	
	

	
 	/**
 	 * Get calType.
 	 * @return calType as CalTypeMod::CalType
 	 */
 	CalTypeMod::CalType getCalType() const;
	
 
 	
 	
 	/**
 	 * Set calType with the specified CalTypeMod::CalType.
 	 * @param calType The CalTypeMod::CalType value to which calType is to be set.
 	 
 		
 			
 	 */
 	void setCalType (CalTypeMod::CalType calType);
  		
	
	
	


	
	// ===> Attribute numScan
	
	
	

	
 	/**
 	 * Get numScan.
 	 * @return numScan as int
 	 */
 	int getNumScan() const;
	
 
 	
 	
 	/**
 	 * Set numScan with the specified int.
 	 * @param numScan The int value to which numScan is to be set.
 	 
 		
 			
 	 */
 	void setNumScan (int numScan);
  		
	
	
	


	
	// ===> Attribute scanSet
	
	
	

	
 	/**
 	 * Get scanSet.
 	 * @return scanSet as std::vector<int >
 	 */
 	std::vector<int > getScanSet() const;
	
 
 	
 	
 	/**
 	 * Set scanSet with the specified std::vector<int >.
 	 * @param scanSet The std::vector<int > value to which scanSet is to be set.
 	 
 		
 			
 	 */
 	void setScanSet (std::vector<int > scanSet);
  		
	
	
	


	
	// ===> Attribute assocCalDataId, which is optional
	
	
	
	/**
	 * The attribute assocCalDataId is optional. Return true if this attribute exists.
	 * @return true if and only if the assocCalDataId attribute exists. 
	 */
	bool isAssocCalDataIdExists() const;
	

	
 	/**
 	 * Get assocCalDataId, which is optional.
 	 * @return assocCalDataId as Tag
 	 * @throws IllegalAccessException If assocCalDataId does not exist.
 	 */
 	Tag getAssocCalDataId() const;
	
 
 	
 	
 	/**
 	 * Set assocCalDataId with the specified Tag.
 	 * @param assocCalDataId The Tag value to which assocCalDataId is to be set.
 	 
 		
 	 */
 	void setAssocCalDataId (Tag assocCalDataId);
		
	
	
	
	/**
	 * Mark assocCalDataId, which is an optional field, as non-existent.
	 */
	void clearAssocCalDataId ();
	


	
	// ===> Attribute assocCalNature, which is optional
	
	
	
	/**
	 * The attribute assocCalNature is optional. Return true if this attribute exists.
	 * @return true if and only if the assocCalNature attribute exists. 
	 */
	bool isAssocCalNatureExists() const;
	

	
 	/**
 	 * Get assocCalNature, which is optional.
 	 * @return assocCalNature as AssociatedCalNatureMod::AssociatedCalNature
 	 * @throws IllegalAccessException If assocCalNature does not exist.
 	 */
 	AssociatedCalNatureMod::AssociatedCalNature getAssocCalNature() const;
	
 
 	
 	
 	/**
 	 * Set assocCalNature with the specified AssociatedCalNatureMod::AssociatedCalNature.
 	 * @param assocCalNature The AssociatedCalNatureMod::AssociatedCalNature value to which assocCalNature is to be set.
 	 
 		
 	 */
 	void setAssocCalNature (AssociatedCalNatureMod::AssociatedCalNature assocCalNature);
		
	
	
	
	/**
	 * Mark assocCalNature, which is an optional field, as non-existent.
	 */
	void clearAssocCalNature ();
	


	
	// ===> Attribute fieldName, which is optional
	
	
	
	/**
	 * The attribute fieldName is optional. Return true if this attribute exists.
	 * @return true if and only if the fieldName attribute exists. 
	 */
	bool isFieldNameExists() const;
	

	
 	/**
 	 * Get fieldName, which is optional.
 	 * @return fieldName as std::vector<std::string >
 	 * @throws IllegalAccessException If fieldName does not exist.
 	 */
 	std::vector<std::string > getFieldName() const;
	
 
 	
 	
 	/**
 	 * Set fieldName with the specified std::vector<std::string >.
 	 * @param fieldName The std::vector<std::string > value to which fieldName is to be set.
 	 
 		
 	 */
 	void setFieldName (std::vector<std::string > fieldName);
		
	
	
	
	/**
	 * Mark fieldName, which is an optional field, as non-existent.
	 */
	void clearFieldName ();
	


	
	// ===> Attribute sourceName, which is optional
	
	
	
	/**
	 * The attribute sourceName is optional. Return true if this attribute exists.
	 * @return true if and only if the sourceName attribute exists. 
	 */
	bool isSourceNameExists() const;
	

	
 	/**
 	 * Get sourceName, which is optional.
 	 * @return sourceName as std::vector<std::string >
 	 * @throws IllegalAccessException If sourceName does not exist.
 	 */
 	std::vector<std::string > getSourceName() const;
	
 
 	
 	
 	/**
 	 * Set sourceName with the specified std::vector<std::string >.
 	 * @param sourceName The std::vector<std::string > value to which sourceName is to be set.
 	 
 		
 	 */
 	void setSourceName (std::vector<std::string > sourceName);
		
	
	
	
	/**
	 * Mark sourceName, which is an optional field, as non-existent.
	 */
	void clearSourceName ();
	


	
	// ===> Attribute sourceCode, which is optional
	
	
	
	/**
	 * The attribute sourceCode is optional. Return true if this attribute exists.
	 * @return true if and only if the sourceCode attribute exists. 
	 */
	bool isSourceCodeExists() const;
	

	
 	/**
 	 * Get sourceCode, which is optional.
 	 * @return sourceCode as std::vector<std::string >
 	 * @throws IllegalAccessException If sourceCode does not exist.
 	 */
 	std::vector<std::string > getSourceCode() const;
	
 
 	
 	
 	/**
 	 * Set sourceCode with the specified std::vector<std::string >.
 	 * @param sourceCode The std::vector<std::string > value to which sourceCode is to be set.
 	 
 		
 	 */
 	void setSourceCode (std::vector<std::string > sourceCode);
		
	
	
	
	/**
	 * Mark sourceCode, which is an optional field, as non-existent.
	 */
	void clearSourceCode ();
	


	
	// ===> Attribute scanIntent, which is optional
	
	
	
	/**
	 * The attribute scanIntent is optional. Return true if this attribute exists.
	 * @return true if and only if the scanIntent attribute exists. 
	 */
	bool isScanIntentExists() const;
	

	
 	/**
 	 * Get scanIntent, which is optional.
 	 * @return scanIntent as std::vector<ScanIntentMod::ScanIntent >
 	 * @throws IllegalAccessException If scanIntent does not exist.
 	 */
 	std::vector<ScanIntentMod::ScanIntent > getScanIntent() const;
	
 
 	
 	
 	/**
 	 * Set scanIntent with the specified std::vector<ScanIntentMod::ScanIntent >.
 	 * @param scanIntent The std::vector<ScanIntentMod::ScanIntent > value to which scanIntent is to be set.
 	 
 		
 	 */
 	void setScanIntent (std::vector<ScanIntentMod::ScanIntent > scanIntent);
		
	
	
	
	/**
	 * Mark scanIntent, which is an optional field, as non-existent.
	 */
	void clearScanIntent ();
	


	////////////////////////////////
	// Extrinsic Table Attributes //
	////////////////////////////////
	
	///////////
	// Links //
	///////////
	
	
	
	
	/**
	 * Compare each mandatory attribute except the autoincrementable one of this CalDataRow with 
	 * the corresponding parameters and return true if there is a match and false otherwise.
	 	
	 * @param startTimeObserved
	    
	 * @param endTimeObserved
	    
	 * @param execBlockUID
	    
	 * @param calDataType
	    
	 * @param calType
	    
	 * @param numScan
	    
	 * @param scanSet
	    
	 */ 
	bool compareNoAutoInc(ArrayTime startTimeObserved, ArrayTime endTimeObserved, EntityRef execBlockUID, CalDataOriginMod::CalDataOrigin calDataType, CalTypeMod::CalType calType, int numScan, std::vector<int > scanSet);
	
	

	
	/**
	 * Compare each mandatory value (i.e. not in the key) attribute  with 
	 * the corresponding parameters and return true if there is a match and false otherwise.
	 	
	 * @param startTimeObserved
	    
	 * @param endTimeObserved
	    
	 * @param execBlockUID
	    
	 * @param calDataType
	    
	 * @param calType
	    
	 * @param numScan
	    
	 * @param scanSet
	    
	 */ 
	bool compareRequiredValue(ArrayTime startTimeObserved, ArrayTime endTimeObserved, EntityRef execBlockUID, CalDataOriginMod::CalDataOrigin calDataType, CalTypeMod::CalType calType, int numScan, std::vector<int > scanSet); 
		 
	
	/**
	 * Return true if all required attributes of the value part are equal to their homologues
	 * in x and false otherwise.
	 *
	 * @param x a pointer on the CalDataRow whose required attributes of the value part 
	 * will be compared with those of this.
	 * @return a boolean.
	 */
	bool equalByRequiredValue(CalDataRow* x) ;
	
#ifndef WITHOUT_ACS
	/**
	 * Return this row in the form of an IDL struct.
	 * @return The values of this row as a CalDataRowIDL struct.
	 */
	asdmIDL::CalDataRowIDL *toIDL() const;
	
	/**
	 * Define the content of a CalDataRowIDL struct from the values
	 * found in this row.
	 *
	 * @param x a reference to the CalDataRowIDL struct to be set.
	 *
	 */
	 void toIDL(asdmIDL::CalDataRowIDL& x) const;
#endif
	
#ifndef WITHOUT_ACS
	/**
	 * Fill the values of this row from the IDL struct CalDataRowIDL.
	 * @param x The IDL struct containing the values used to fill this row.
	 * @throws ConversionException
	 */
	void setFromIDL (asdmIDL::CalDataRowIDL x) ;
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

	std::map<std::string, CalDataAttributeFromBin> fromBinMethods;
void calDataIdFromBin( EndianIStream& eis);
void startTimeObservedFromBin( EndianIStream& eis);
void endTimeObservedFromBin( EndianIStream& eis);
void execBlockUIDFromBin( EndianIStream& eis);
void calDataTypeFromBin( EndianIStream& eis);
void calTypeFromBin( EndianIStream& eis);
void numScanFromBin( EndianIStream& eis);
void scanSetFromBin( EndianIStream& eis);

void assocCalDataIdFromBin( EndianIStream& eis);
void assocCalNatureFromBin( EndianIStream& eis);
void fieldNameFromBin( EndianIStream& eis);
void sourceNameFromBin( EndianIStream& eis);
void sourceCodeFromBin( EndianIStream& eis);
void scanIntentFromBin( EndianIStream& eis);


	 /**
	  * Deserialize a stream of bytes read from an EndianIStream to build a PointingRow.
	  * @param eiss the EndianIStream to be read.
	  * @param table the CalDataTable to which the row built by deserialization will be parented.
	  * @param attributesSeq a vector containing the names of the attributes . The elements order defines the order 
	  * in which the attributes are written in the binary serialization.
	  */
	 static CalDataRow* fromBin(EndianIStream& eis, CalDataTable& table, const std::vector<std::string>& attributesSeq);	 
 
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
	CalDataTable &table;
	/**
	 * Whether this row has been added to the table or not.
	 */
	bool hasBeenAdded;

	// This method is used by the Table class when this row is added to the table.
	void isAdded(bool added);


	/**
	 * Create a CalDataRow.
	 * <p>
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 */ 
	CalDataRow (CalDataTable &table);

	/**
	 * Create a CalDataRow using a copy constructor mechanism.
	 * <p>
	 * Given a CalDataRow row and a CalDataTable table, the method creates a new
	 * CalDataRow owned by table. Each attribute of the created row is a copy (deep)
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
	 CalDataRow (CalDataTable &table, CalDataRow *row);
	 	
	////////////////////////////////
	// Intrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute calDataId
	
	

	Tag calDataId;

	
	
 	
 	/**
 	 * Set calDataId with the specified Tag value.
 	 * @param calDataId The Tag value to which calDataId is to be set.
		
 		
			
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 		
 	 */
 	void setCalDataId (Tag calDataId);
  		
	

	
	// ===> Attribute startTimeObserved
	
	

	ArrayTime startTimeObserved;

	
	
 	

	
	// ===> Attribute endTimeObserved
	
	

	ArrayTime endTimeObserved;

	
	
 	

	
	// ===> Attribute execBlockUID
	
	

	EntityRef execBlockUID;

	
	
 	

	
	// ===> Attribute calDataType
	
	

	CalDataOriginMod::CalDataOrigin calDataType;

	
	
 	

	
	// ===> Attribute calType
	
	

	CalTypeMod::CalType calType;

	
	
 	

	
	// ===> Attribute numScan
	
	

	int numScan;

	
	
 	

	
	// ===> Attribute scanSet
	
	

	std::vector<int > scanSet;

	
	
 	

	
	// ===> Attribute assocCalDataId, which is optional
	
	
	bool assocCalDataIdExists;
	

	Tag assocCalDataId;

	
	
 	

	
	// ===> Attribute assocCalNature, which is optional
	
	
	bool assocCalNatureExists;
	

	AssociatedCalNatureMod::AssociatedCalNature assocCalNature;

	
	
 	

	
	// ===> Attribute fieldName, which is optional
	
	
	bool fieldNameExists;
	

	std::vector<std::string > fieldName;

	
	
 	

	
	// ===> Attribute sourceName, which is optional
	
	
	bool sourceNameExists;
	

	std::vector<std::string > sourceName;

	
	
 	

	
	// ===> Attribute sourceCode, which is optional
	
	
	bool sourceCodeExists;
	

	std::vector<std::string > sourceCode;

	
	
 	

	
	// ===> Attribute scanIntent, which is optional
	
	
	bool scanIntentExists;
	

	std::vector<ScanIntentMod::ScanIntent > scanIntent;

	
	
 	

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
	std::map<std::string, CalDataAttributeFromBin> fromBinMethods;
void calDataIdFromBin( EndianIStream& eis);
void startTimeObservedFromBin( EndianIStream& eis);
void endTimeObservedFromBin( EndianIStream& eis);
void execBlockUIDFromBin( EndianIStream& eis);
void calDataTypeFromBin( EndianIStream& eis);
void calTypeFromBin( EndianIStream& eis);
void numScanFromBin( EndianIStream& eis);
void scanSetFromBin( EndianIStream& eis);

void assocCalDataIdFromBin( EndianIStream& eis);
void assocCalNatureFromBin( EndianIStream& eis);
void fieldNameFromBin( EndianIStream& eis);
void sourceNameFromBin( EndianIStream& eis);
void sourceCodeFromBin( EndianIStream& eis);
void scanIntentFromBin( EndianIStream& eis);

*/
	
	///////////////////////////////////
	// text-deserialization material //
	///////////////////////////////////
	std::map<std::string, CalDataAttributeFromText> fromTextMethods;
	
void calDataIdFromText (const std::string & s);
	
	
void startTimeObservedFromText (const std::string & s);
	
	
void endTimeObservedFromText (const std::string & s);
	
	
	
void calDataTypeFromText (const std::string & s);
	
	
void calTypeFromText (const std::string & s);
	
	
void numScanFromText (const std::string & s);
	
	
void scanSetFromText (const std::string & s);
	

	
void assocCalDataIdFromText (const std::string & s);
	
	
void assocCalNatureFromText (const std::string & s);
	
	
void fieldNameFromText (const std::string & s);
	
	
void sourceNameFromText (const std::string & s);
	
	
void sourceCodeFromText (const std::string & s);
	
	
void scanIntentFromText (const std::string & s);
	
	
	
	/**
	 * Serialize this into a stream of bytes written to an EndianOSStream.
	 * @param eoss the EndianOSStream to be written to
	 */
	 void toBin(EndianOSStream& eoss);
	 	 
	 /**
	  * Deserialize a stream of bytes read from an EndianIStream to build a PointingRow.
	  * @param eiss the EndianIStream to be read.
	  * @param table the CalDataTable to which the row built by deserialization will be parented.
	  * @param attributesSeq a vector containing the names of the attributes . The elements order defines the order 
	  * in which the attributes are written in the binary serialization.

	 static CalDataRow* fromBin(EndianIStream& eis, CalDataTable& table, const std::vector<std::string>& attributesSeq);	 
		*/
};

} // End namespace asdm

#endif /* CalData_CLASS */
