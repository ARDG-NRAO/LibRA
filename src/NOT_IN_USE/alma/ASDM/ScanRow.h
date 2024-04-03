
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
 * File ScanRow.h
 */
 
#ifndef ScanRow_CLASS
#define ScanRow_CLASS

#include <vector>
#include <string>
#include <set>

#ifndef WITHOUT_ACS
#include <asdmIDLC.h>
#endif






	 
#include <alma/ASDM/ArrayTime.h>
	

	 
#include <alma/ASDM/Tag.h>
	




	

	

	

	

	

	
#include <alma/Enumerations/CScanIntent.h>
	

	
#include <alma/Enumerations/CCalDataOrigin.h>
	

	

	
#include <alma/Enumerations/CCalibrationFunction.h>
	

	
#include <alma/Enumerations/CCalibrationSet.h>
	

	
#include <alma/Enumerations/CAntennaMotionPattern.h>
	

	

	

	



#include <alma/ASDM/ConversionException.h>
#include <alma/ASDM/NoSuchRow.h>
#include <alma/ASDM/IllegalAccessException.h>

#include <alma/ASDM/RowTransformer.h>
//#include <alma/ASDM/TableStreamReader.h>

/*\file Scan.h
    \brief Generated from model's revision "-1", branch ""
*/

namespace asdm {

//class asdm::ScanTable;


// class asdm::ExecBlockRow;
class ExecBlockRow;
	

class ScanRow;
typedef void (ScanRow::*ScanAttributeFromBin) (EndianIStream& eis);
typedef void (ScanRow::*ScanAttributeFromText) (const std::string& s);

/**
 * The ScanRow class is a row of a ScanTable.
 * 
 * Generated from model's revision "-1", branch ""
 *
 */
class ScanRow {
friend class asdm::ScanTable;
friend class asdm::RowTransformer<ScanRow>;
//friend class asdm::TableStreamReader<ScanTable, ScanRow>;

public:

	virtual ~ScanRow();

	/**
	 * Return the table to which this row belongs.
	 */
	ScanTable &getTable() const;
	
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
  		
	
	
	


	
	// ===> Attribute numIntent
	
	
	

	
 	/**
 	 * Get numIntent.
 	 * @return numIntent as int
 	 */
 	int getNumIntent() const;
	
 
 	
 	
 	/**
 	 * Set numIntent with the specified int.
 	 * @param numIntent The int value to which numIntent is to be set.
 	 
 		
 			
 	 */
 	void setNumIntent (int numIntent);
  		
	
	
	


	
	// ===> Attribute numSubscan
	
	
	

	
 	/**
 	 * Get numSubscan.
 	 * @return numSubscan as int
 	 */
 	int getNumSubscan() const;
	
 
 	
 	
 	/**
 	 * Set numSubscan with the specified int.
 	 * @param numSubscan The int value to which numSubscan is to be set.
 	 
 		
 			
 	 */
 	void setNumSubscan (int numSubscan);
  		
	
	
	


	
	// ===> Attribute scanIntent
	
	
	

	
 	/**
 	 * Get scanIntent.
 	 * @return scanIntent as std::vector<ScanIntentMod::ScanIntent >
 	 */
 	std::vector<ScanIntentMod::ScanIntent > getScanIntent() const;
	
 
 	
 	
 	/**
 	 * Set scanIntent with the specified std::vector<ScanIntentMod::ScanIntent >.
 	 * @param scanIntent The std::vector<ScanIntentMod::ScanIntent > value to which scanIntent is to be set.
 	 
 		
 			
 	 */
 	void setScanIntent (std::vector<ScanIntentMod::ScanIntent > scanIntent);
  		
	
	
	


	
	// ===> Attribute calDataType
	
	
	

	
 	/**
 	 * Get calDataType.
 	 * @return calDataType as std::vector<CalDataOriginMod::CalDataOrigin >
 	 */
 	std::vector<CalDataOriginMod::CalDataOrigin > getCalDataType() const;
	
 
 	
 	
 	/**
 	 * Set calDataType with the specified std::vector<CalDataOriginMod::CalDataOrigin >.
 	 * @param calDataType The std::vector<CalDataOriginMod::CalDataOrigin > value to which calDataType is to be set.
 	 
 		
 			
 	 */
 	void setCalDataType (std::vector<CalDataOriginMod::CalDataOrigin > calDataType);
  		
	
	
	


	
	// ===> Attribute calibrationOnLine
	
	
	

	
 	/**
 	 * Get calibrationOnLine.
 	 * @return calibrationOnLine as std::vector<bool >
 	 */
 	std::vector<bool > getCalibrationOnLine() const;
	
 
 	
 	
 	/**
 	 * Set calibrationOnLine with the specified std::vector<bool >.
 	 * @param calibrationOnLine The std::vector<bool > value to which calibrationOnLine is to be set.
 	 
 		
 			
 	 */
 	void setCalibrationOnLine (std::vector<bool > calibrationOnLine);
  		
	
	
	


	
	// ===> Attribute calibrationFunction, which is optional
	
	
	
	/**
	 * The attribute calibrationFunction is optional. Return true if this attribute exists.
	 * @return true if and only if the calibrationFunction attribute exists. 
	 */
	bool isCalibrationFunctionExists() const;
	

	
 	/**
 	 * Get calibrationFunction, which is optional.
 	 * @return calibrationFunction as std::vector<CalibrationFunctionMod::CalibrationFunction >
 	 * @throws IllegalAccessException If calibrationFunction does not exist.
 	 */
 	std::vector<CalibrationFunctionMod::CalibrationFunction > getCalibrationFunction() const;
	
 
 	
 	
 	/**
 	 * Set calibrationFunction with the specified std::vector<CalibrationFunctionMod::CalibrationFunction >.
 	 * @param calibrationFunction The std::vector<CalibrationFunctionMod::CalibrationFunction > value to which calibrationFunction is to be set.
 	 
 		
 	 */
 	void setCalibrationFunction (std::vector<CalibrationFunctionMod::CalibrationFunction > calibrationFunction);
		
	
	
	
	/**
	 * Mark calibrationFunction, which is an optional field, as non-existent.
	 */
	void clearCalibrationFunction ();
	


	
	// ===> Attribute calibrationSet, which is optional
	
	
	
	/**
	 * The attribute calibrationSet is optional. Return true if this attribute exists.
	 * @return true if and only if the calibrationSet attribute exists. 
	 */
	bool isCalibrationSetExists() const;
	

	
 	/**
 	 * Get calibrationSet, which is optional.
 	 * @return calibrationSet as std::vector<CalibrationSetMod::CalibrationSet >
 	 * @throws IllegalAccessException If calibrationSet does not exist.
 	 */
 	std::vector<CalibrationSetMod::CalibrationSet > getCalibrationSet() const;
	
 
 	
 	
 	/**
 	 * Set calibrationSet with the specified std::vector<CalibrationSetMod::CalibrationSet >.
 	 * @param calibrationSet The std::vector<CalibrationSetMod::CalibrationSet > value to which calibrationSet is to be set.
 	 
 		
 	 */
 	void setCalibrationSet (std::vector<CalibrationSetMod::CalibrationSet > calibrationSet);
		
	
	
	
	/**
	 * Mark calibrationSet, which is an optional field, as non-existent.
	 */
	void clearCalibrationSet ();
	


	
	// ===> Attribute calPattern, which is optional
	
	
	
	/**
	 * The attribute calPattern is optional. Return true if this attribute exists.
	 * @return true if and only if the calPattern attribute exists. 
	 */
	bool isCalPatternExists() const;
	

	
 	/**
 	 * Get calPattern, which is optional.
 	 * @return calPattern as std::vector<AntennaMotionPatternMod::AntennaMotionPattern >
 	 * @throws IllegalAccessException If calPattern does not exist.
 	 */
 	std::vector<AntennaMotionPatternMod::AntennaMotionPattern > getCalPattern() const;
	
 
 	
 	
 	/**
 	 * Set calPattern with the specified std::vector<AntennaMotionPatternMod::AntennaMotionPattern >.
 	 * @param calPattern The std::vector<AntennaMotionPatternMod::AntennaMotionPattern > value to which calPattern is to be set.
 	 
 		
 	 */
 	void setCalPattern (std::vector<AntennaMotionPatternMod::AntennaMotionPattern > calPattern);
		
	
	
	
	/**
	 * Mark calPattern, which is an optional field, as non-existent.
	 */
	void clearCalPattern ();
	


	
	// ===> Attribute numField, which is optional
	
	
	
	/**
	 * The attribute numField is optional. Return true if this attribute exists.
	 * @return true if and only if the numField attribute exists. 
	 */
	bool isNumFieldExists() const;
	

	
 	/**
 	 * Get numField, which is optional.
 	 * @return numField as int
 	 * @throws IllegalAccessException If numField does not exist.
 	 */
 	int getNumField() const;
	
 
 	
 	
 	/**
 	 * Set numField with the specified int.
 	 * @param numField The int value to which numField is to be set.
 	 
 		
 	 */
 	void setNumField (int numField);
		
	
	
	
	/**
	 * Mark numField, which is an optional field, as non-existent.
	 */
	void clearNumField ();
	


	
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
 	 * @return sourceName as std::string
 	 * @throws IllegalAccessException If sourceName does not exist.
 	 */
 	std::string getSourceName() const;
	
 
 	
 	
 	/**
 	 * Set sourceName with the specified std::string.
 	 * @param sourceName The std::string value to which sourceName is to be set.
 	 
 		
 	 */
 	void setSourceName (std::string sourceName);
		
	
	
	
	/**
	 * Mark sourceName, which is an optional field, as non-existent.
	 */
	void clearSourceName ();
	


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
	 * Compare each mandatory attribute except the autoincrementable one of this ScanRow with 
	 * the corresponding parameters and return true if there is a match and false otherwise.
	 	
	 * @param execBlockId
	    
	 * @param scanNumber
	    
	 * @param startTime
	    
	 * @param endTime
	    
	 * @param numIntent
	    
	 * @param numSubscan
	    
	 * @param scanIntent
	    
	 * @param calDataType
	    
	 * @param calibrationOnLine
	    
	 */ 
	bool compareNoAutoInc(Tag execBlockId, int scanNumber, ArrayTime startTime, ArrayTime endTime, int numIntent, int numSubscan, std::vector<ScanIntentMod::ScanIntent > scanIntent, std::vector<CalDataOriginMod::CalDataOrigin > calDataType, std::vector<bool > calibrationOnLine);
	
	

	
	/**
	 * Compare each mandatory value (i.e. not in the key) attribute  with 
	 * the corresponding parameters and return true if there is a match and false otherwise.
	 	
	 * @param startTime
	    
	 * @param endTime
	    
	 * @param numIntent
	    
	 * @param numSubscan
	    
	 * @param scanIntent
	    
	 * @param calDataType
	    
	 * @param calibrationOnLine
	    
	 */ 
	bool compareRequiredValue(ArrayTime startTime, ArrayTime endTime, int numIntent, int numSubscan, std::vector<ScanIntentMod::ScanIntent > scanIntent, std::vector<CalDataOriginMod::CalDataOrigin > calDataType, std::vector<bool > calibrationOnLine); 
		 
	
	/**
	 * Return true if all required attributes of the value part are equal to their homologues
	 * in x and false otherwise.
	 *
	 * @param x a pointer on the ScanRow whose required attributes of the value part 
	 * will be compared with those of this.
	 * @return a boolean.
	 */
	bool equalByRequiredValue(ScanRow* x) ;
	
#ifndef WITHOUT_ACS
	/**
	 * Return this row in the form of an IDL struct.
	 * @return The values of this row as a ScanRowIDL struct.
	 */
	asdmIDL::ScanRowIDL *toIDL() const;
	
	/**
	 * Define the content of a ScanRowIDL struct from the values
	 * found in this row.
	 *
	 * @param x a reference to the ScanRowIDL struct to be set.
	 *
	 */
	 void toIDL(asdmIDL::ScanRowIDL& x) const;
#endif
	
#ifndef WITHOUT_ACS
	/**
	 * Fill the values of this row from the IDL struct ScanRowIDL.
	 * @param x The IDL struct containing the values used to fill this row.
	 * @throws ConversionException
	 */
	void setFromIDL (asdmIDL::ScanRowIDL x) ;
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

	std::map<std::string, ScanAttributeFromBin> fromBinMethods;
void execBlockIdFromBin( EndianIStream& eis);
void scanNumberFromBin( EndianIStream& eis);
void startTimeFromBin( EndianIStream& eis);
void endTimeFromBin( EndianIStream& eis);
void numIntentFromBin( EndianIStream& eis);
void numSubscanFromBin( EndianIStream& eis);
void scanIntentFromBin( EndianIStream& eis);
void calDataTypeFromBin( EndianIStream& eis);
void calibrationOnLineFromBin( EndianIStream& eis);

void calibrationFunctionFromBin( EndianIStream& eis);
void calibrationSetFromBin( EndianIStream& eis);
void calPatternFromBin( EndianIStream& eis);
void numFieldFromBin( EndianIStream& eis);
void fieldNameFromBin( EndianIStream& eis);
void sourceNameFromBin( EndianIStream& eis);


	 /**
	  * Deserialize a stream of bytes read from an EndianIStream to build a PointingRow.
	  * @param eiss the EndianIStream to be read.
	  * @param table the ScanTable to which the row built by deserialization will be parented.
	  * @param attributesSeq a vector containing the names of the attributes . The elements order defines the order 
	  * in which the attributes are written in the binary serialization.
	  */
	 static ScanRow* fromBin(EndianIStream& eis, ScanTable& table, const std::vector<std::string>& attributesSeq);	 
 
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
	ScanTable &table;
	/**
	 * Whether this row has been added to the table or not.
	 */
	bool hasBeenAdded;

	// This method is used by the Table class when this row is added to the table.
	void isAdded(bool added);


	/**
	 * Create a ScanRow.
	 * <p>
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 */ 
	ScanRow (ScanTable &table);

	/**
	 * Create a ScanRow using a copy constructor mechanism.
	 * <p>
	 * Given a ScanRow row and a ScanTable table, the method creates a new
	 * ScanRow owned by table. Each attribute of the created row is a copy (deep)
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
	 ScanRow (ScanTable &table, ScanRow *row);
	 	
	////////////////////////////////
	// Intrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute scanNumber
	
	

	int scanNumber;

	
	
 	

	
	// ===> Attribute startTime
	
	

	ArrayTime startTime;

	
	
 	

	
	// ===> Attribute endTime
	
	

	ArrayTime endTime;

	
	
 	

	
	// ===> Attribute numIntent
	
	

	int numIntent;

	
	
 	

	
	// ===> Attribute numSubscan
	
	

	int numSubscan;

	
	
 	

	
	// ===> Attribute scanIntent
	
	

	std::vector<ScanIntentMod::ScanIntent > scanIntent;

	
	
 	

	
	// ===> Attribute calDataType
	
	

	std::vector<CalDataOriginMod::CalDataOrigin > calDataType;

	
	
 	

	
	// ===> Attribute calibrationOnLine
	
	

	std::vector<bool > calibrationOnLine;

	
	
 	

	
	// ===> Attribute calibrationFunction, which is optional
	
	
	bool calibrationFunctionExists;
	

	std::vector<CalibrationFunctionMod::CalibrationFunction > calibrationFunction;

	
	
 	

	
	// ===> Attribute calibrationSet, which is optional
	
	
	bool calibrationSetExists;
	

	std::vector<CalibrationSetMod::CalibrationSet > calibrationSet;

	
	
 	

	
	// ===> Attribute calPattern, which is optional
	
	
	bool calPatternExists;
	

	std::vector<AntennaMotionPatternMod::AntennaMotionPattern > calPattern;

	
	
 	

	
	// ===> Attribute numField, which is optional
	
	
	bool numFieldExists;
	

	int numField;

	
	
 	

	
	// ===> Attribute fieldName, which is optional
	
	
	bool fieldNameExists;
	

	std::vector<std::string > fieldName;

	
	
 	

	
	// ===> Attribute sourceName, which is optional
	
	
	bool sourceNameExists;
	

	std::string sourceName;

	
	
 	

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
	std::map<std::string, ScanAttributeFromBin> fromBinMethods;
void execBlockIdFromBin( EndianIStream& eis);
void scanNumberFromBin( EndianIStream& eis);
void startTimeFromBin( EndianIStream& eis);
void endTimeFromBin( EndianIStream& eis);
void numIntentFromBin( EndianIStream& eis);
void numSubscanFromBin( EndianIStream& eis);
void scanIntentFromBin( EndianIStream& eis);
void calDataTypeFromBin( EndianIStream& eis);
void calibrationOnLineFromBin( EndianIStream& eis);

void calibrationFunctionFromBin( EndianIStream& eis);
void calibrationSetFromBin( EndianIStream& eis);
void calPatternFromBin( EndianIStream& eis);
void numFieldFromBin( EndianIStream& eis);
void fieldNameFromBin( EndianIStream& eis);
void sourceNameFromBin( EndianIStream& eis);

*/
	
	///////////////////////////////////
	// text-deserialization material //
	///////////////////////////////////
	std::map<std::string, ScanAttributeFromText> fromTextMethods;
	
void execBlockIdFromText (const std::string & s);
	
	
void scanNumberFromText (const std::string & s);
	
	
void startTimeFromText (const std::string & s);
	
	
void endTimeFromText (const std::string & s);
	
	
void numIntentFromText (const std::string & s);
	
	
void numSubscanFromText (const std::string & s);
	
	
void scanIntentFromText (const std::string & s);
	
	
void calDataTypeFromText (const std::string & s);
	
	
void calibrationOnLineFromText (const std::string & s);
	

	
void calibrationFunctionFromText (const std::string & s);
	
	
void calibrationSetFromText (const std::string & s);
	
	
void calPatternFromText (const std::string & s);
	
	
void numFieldFromText (const std::string & s);
	
	
void fieldNameFromText (const std::string & s);
	
	
void sourceNameFromText (const std::string & s);
	
	
	
	/**
	 * Serialize this into a stream of bytes written to an EndianOSStream.
	 * @param eoss the EndianOSStream to be written to
	 */
	 void toBin(EndianOSStream& eoss);
	 	 
	 /**
	  * Deserialize a stream of bytes read from an EndianIStream to build a PointingRow.
	  * @param eiss the EndianIStream to be read.
	  * @param table the ScanTable to which the row built by deserialization will be parented.
	  * @param attributesSeq a vector containing the names of the attributes . The elements order defines the order 
	  * in which the attributes are written in the binary serialization.

	 static ScanRow* fromBin(EndianIStream& eis, ScanTable& table, const std::vector<std::string>& attributesSeq);	 
		*/
};

} // End namespace asdm

#endif /* Scan_CLASS */
