
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
 * File DelayModelVariableParametersRow.h
 */
 
#ifndef DelayModelVariableParametersRow_CLASS
#define DelayModelVariableParametersRow_CLASS

#include <vector>
#include <string>
#include <set>

#ifndef WITHOUT_ACS
#include <asdmIDLC.h>
#endif






	 
#include <alma/ASDM/Angle.h>
	

	 
#include <alma/ASDM/AngularRate.h>
	

	 
#include <alma/ASDM/ArrayTime.h>
	

	 
#include <alma/ASDM/Tag.h>
	




	

	

	

	

	
#include <alma/Enumerations/CDifferenceType.h>
	

	

	

	

	
#include <alma/Enumerations/CDifferenceType.h>
	

	

	

	

	



#include <alma/ASDM/ConversionException.h>
#include <alma/ASDM/NoSuchRow.h>
#include <alma/ASDM/IllegalAccessException.h>

#include <alma/ASDM/RowTransformer.h>
//#include <alma/ASDM/TableStreamReader.h>

/*\file DelayModelVariableParameters.h
    \brief Generated from model's revision "-1", branch ""
*/

namespace asdm {

//class asdm::DelayModelVariableParametersTable;


// class asdm::DelayModelFixedParametersRow;
class DelayModelFixedParametersRow;
	

class DelayModelVariableParametersRow;
typedef void (DelayModelVariableParametersRow::*DelayModelVariableParametersAttributeFromBin) (EndianIStream& eis);
typedef void (DelayModelVariableParametersRow::*DelayModelVariableParametersAttributeFromText) (const std::string& s);

/**
 * The DelayModelVariableParametersRow class is a row of a DelayModelVariableParametersTable.
 * 
 * Generated from model's revision "-1", branch ""
 *
 */
class DelayModelVariableParametersRow {
friend class asdm::DelayModelVariableParametersTable;
friend class asdm::RowTransformer<DelayModelVariableParametersRow>;
//friend class asdm::TableStreamReader<DelayModelVariableParametersTable, DelayModelVariableParametersRow>;

public:

	virtual ~DelayModelVariableParametersRow();

	/**
	 * Return the table to which this row belongs.
	 */
	DelayModelVariableParametersTable &getTable() const;
	
	/**
	 * Has this row been added to its table ?
	 * @return true if and only if it has been added.
	 */
	bool isAdded() const;
		
	////////////////////////////////
	// Intrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute delayModelVariableParametersId
	
	
	

	
 	/**
 	 * Get delayModelVariableParametersId.
 	 * @return delayModelVariableParametersId as Tag
 	 */
 	Tag getDelayModelVariableParametersId() const;
	
 
 	
 	
	
	


	
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
  		
	
	
	


	
	// ===> Attribute ut1_utc
	
	
	

	
 	/**
 	 * Get ut1_utc.
 	 * @return ut1_utc as double
 	 */
 	double getUt1_utc() const;
	
 
 	
 	
 	/**
 	 * Set ut1_utc with the specified double.
 	 * @param ut1_utc The double value to which ut1_utc is to be set.
 	 
 		
 			
 	 */
 	void setUt1_utc (double ut1_utc);
  		
	
	
	


	
	// ===> Attribute iat_utc
	
	
	

	
 	/**
 	 * Get iat_utc.
 	 * @return iat_utc as double
 	 */
 	double getIat_utc() const;
	
 
 	
 	
 	/**
 	 * Set iat_utc with the specified double.
 	 * @param iat_utc The double value to which iat_utc is to be set.
 	 
 		
 			
 	 */
 	void setIat_utc (double iat_utc);
  		
	
	
	


	
	// ===> Attribute timeType
	
	
	

	
 	/**
 	 * Get timeType.
 	 * @return timeType as DifferenceTypeMod::DifferenceType
 	 */
 	DifferenceTypeMod::DifferenceType getTimeType() const;
	
 
 	
 	
 	/**
 	 * Set timeType with the specified DifferenceTypeMod::DifferenceType.
 	 * @param timeType The DifferenceTypeMod::DifferenceType value to which timeType is to be set.
 	 
 		
 			
 	 */
 	void setTimeType (DifferenceTypeMod::DifferenceType timeType);
  		
	
	
	


	
	// ===> Attribute gstAtUt0
	
	
	

	
 	/**
 	 * Get gstAtUt0.
 	 * @return gstAtUt0 as Angle
 	 */
 	Angle getGstAtUt0() const;
	
 
 	
 	
 	/**
 	 * Set gstAtUt0 with the specified Angle.
 	 * @param gstAtUt0 The Angle value to which gstAtUt0 is to be set.
 	 
 		
 			
 	 */
 	void setGstAtUt0 (Angle gstAtUt0);
  		
	
	
	


	
	// ===> Attribute earthRotationRate
	
	
	

	
 	/**
 	 * Get earthRotationRate.
 	 * @return earthRotationRate as AngularRate
 	 */
 	AngularRate getEarthRotationRate() const;
	
 
 	
 	
 	/**
 	 * Set earthRotationRate with the specified AngularRate.
 	 * @param earthRotationRate The AngularRate value to which earthRotationRate is to be set.
 	 
 		
 			
 	 */
 	void setEarthRotationRate (AngularRate earthRotationRate);
  		
	
	
	


	
	// ===> Attribute polarOffsets
	
	
	

	
 	/**
 	 * Get polarOffsets.
 	 * @return polarOffsets as std::vector<double >
 	 */
 	std::vector<double > getPolarOffsets() const;
	
 
 	
 	
 	/**
 	 * Set polarOffsets with the specified std::vector<double >.
 	 * @param polarOffsets The std::vector<double > value to which polarOffsets is to be set.
 	 
 		
 			
 	 */
 	void setPolarOffsets (std::vector<double > polarOffsets);
  		
	
	
	


	
	// ===> Attribute polarOffsetsType
	
	
	

	
 	/**
 	 * Get polarOffsetsType.
 	 * @return polarOffsetsType as DifferenceTypeMod::DifferenceType
 	 */
 	DifferenceTypeMod::DifferenceType getPolarOffsetsType() const;
	
 
 	
 	
 	/**
 	 * Set polarOffsetsType with the specified DifferenceTypeMod::DifferenceType.
 	 * @param polarOffsetsType The DifferenceTypeMod::DifferenceType value to which polarOffsetsType is to be set.
 	 
 		
 			
 	 */
 	void setPolarOffsetsType (DifferenceTypeMod::DifferenceType polarOffsetsType);
  		
	
	
	


	
	// ===> Attribute nutationInLongitude, which is optional
	
	
	
	/**
	 * The attribute nutationInLongitude is optional. Return true if this attribute exists.
	 * @return true if and only if the nutationInLongitude attribute exists. 
	 */
	bool isNutationInLongitudeExists() const;
	

	
 	/**
 	 * Get nutationInLongitude, which is optional.
 	 * @return nutationInLongitude as Angle
 	 * @throws IllegalAccessException If nutationInLongitude does not exist.
 	 */
 	Angle getNutationInLongitude() const;
	
 
 	
 	
 	/**
 	 * Set nutationInLongitude with the specified Angle.
 	 * @param nutationInLongitude The Angle value to which nutationInLongitude is to be set.
 	 
 		
 	 */
 	void setNutationInLongitude (Angle nutationInLongitude);
		
	
	
	
	/**
	 * Mark nutationInLongitude, which is an optional field, as non-existent.
	 */
	void clearNutationInLongitude ();
	


	
	// ===> Attribute nutationInLongitudeRate, which is optional
	
	
	
	/**
	 * The attribute nutationInLongitudeRate is optional. Return true if this attribute exists.
	 * @return true if and only if the nutationInLongitudeRate attribute exists. 
	 */
	bool isNutationInLongitudeRateExists() const;
	

	
 	/**
 	 * Get nutationInLongitudeRate, which is optional.
 	 * @return nutationInLongitudeRate as AngularRate
 	 * @throws IllegalAccessException If nutationInLongitudeRate does not exist.
 	 */
 	AngularRate getNutationInLongitudeRate() const;
	
 
 	
 	
 	/**
 	 * Set nutationInLongitudeRate with the specified AngularRate.
 	 * @param nutationInLongitudeRate The AngularRate value to which nutationInLongitudeRate is to be set.
 	 
 		
 	 */
 	void setNutationInLongitudeRate (AngularRate nutationInLongitudeRate);
		
	
	
	
	/**
	 * Mark nutationInLongitudeRate, which is an optional field, as non-existent.
	 */
	void clearNutationInLongitudeRate ();
	


	
	// ===> Attribute nutationInObliquity, which is optional
	
	
	
	/**
	 * The attribute nutationInObliquity is optional. Return true if this attribute exists.
	 * @return true if and only if the nutationInObliquity attribute exists. 
	 */
	bool isNutationInObliquityExists() const;
	

	
 	/**
 	 * Get nutationInObliquity, which is optional.
 	 * @return nutationInObliquity as Angle
 	 * @throws IllegalAccessException If nutationInObliquity does not exist.
 	 */
 	Angle getNutationInObliquity() const;
	
 
 	
 	
 	/**
 	 * Set nutationInObliquity with the specified Angle.
 	 * @param nutationInObliquity The Angle value to which nutationInObliquity is to be set.
 	 
 		
 	 */
 	void setNutationInObliquity (Angle nutationInObliquity);
		
	
	
	
	/**
	 * Mark nutationInObliquity, which is an optional field, as non-existent.
	 */
	void clearNutationInObliquity ();
	


	
	// ===> Attribute nutationInObliquityRate, which is optional
	
	
	
	/**
	 * The attribute nutationInObliquityRate is optional. Return true if this attribute exists.
	 * @return true if and only if the nutationInObliquityRate attribute exists. 
	 */
	bool isNutationInObliquityRateExists() const;
	

	
 	/**
 	 * Get nutationInObliquityRate, which is optional.
 	 * @return nutationInObliquityRate as AngularRate
 	 * @throws IllegalAccessException If nutationInObliquityRate does not exist.
 	 */
 	AngularRate getNutationInObliquityRate() const;
	
 
 	
 	
 	/**
 	 * Set nutationInObliquityRate with the specified AngularRate.
 	 * @param nutationInObliquityRate The AngularRate value to which nutationInObliquityRate is to be set.
 	 
 		
 	 */
 	void setNutationInObliquityRate (AngularRate nutationInObliquityRate);
		
	
	
	
	/**
	 * Mark nutationInObliquityRate, which is an optional field, as non-existent.
	 */
	void clearNutationInObliquityRate ();
	


	////////////////////////////////
	// Extrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute delayModelFixedParametersId
	
	
	

	
 	/**
 	 * Get delayModelFixedParametersId.
 	 * @return delayModelFixedParametersId as Tag
 	 */
 	Tag getDelayModelFixedParametersId() const;
	
 
 	
 	
 	/**
 	 * Set delayModelFixedParametersId with the specified Tag.
 	 * @param delayModelFixedParametersId The Tag value to which delayModelFixedParametersId is to be set.
 	 
 		
 			
 	 */
 	void setDelayModelFixedParametersId (Tag delayModelFixedParametersId);
  		
	
	
	


	///////////
	// Links //
	///////////
	
	

	
		
	/**
	 * delayModelFixedParametersId pointer to the row in the DelayModelFixedParameters table having DelayModelFixedParameters.delayModelFixedParametersId == delayModelFixedParametersId
	 * @return a DelayModelFixedParametersRow*
	 * 
	 
	 */
	 DelayModelFixedParametersRow* getDelayModelFixedParametersUsingDelayModelFixedParametersId();
	 

	

	
	
	
	/**
	 * Compare each mandatory attribute except the autoincrementable one of this DelayModelVariableParametersRow with 
	 * the corresponding parameters and return true if there is a match and false otherwise.
	 	
	 * @param time
	    
	 * @param ut1_utc
	    
	 * @param iat_utc
	    
	 * @param timeType
	    
	 * @param gstAtUt0
	    
	 * @param earthRotationRate
	    
	 * @param polarOffsets
	    
	 * @param polarOffsetsType
	    
	 * @param delayModelFixedParametersId
	    
	 */ 
	bool compareNoAutoInc(ArrayTime time, double ut1_utc, double iat_utc, DifferenceTypeMod::DifferenceType timeType, Angle gstAtUt0, AngularRate earthRotationRate, std::vector<double > polarOffsets, DifferenceTypeMod::DifferenceType polarOffsetsType, Tag delayModelFixedParametersId);
	
	

	
	/**
	 * Compare each mandatory value (i.e. not in the key) attribute  with 
	 * the corresponding parameters and return true if there is a match and false otherwise.
	 	
	 * @param time
	    
	 * @param ut1_utc
	    
	 * @param iat_utc
	    
	 * @param timeType
	    
	 * @param gstAtUt0
	    
	 * @param earthRotationRate
	    
	 * @param polarOffsets
	    
	 * @param polarOffsetsType
	    
	 * @param delayModelFixedParametersId
	    
	 */ 
	bool compareRequiredValue(ArrayTime time, double ut1_utc, double iat_utc, DifferenceTypeMod::DifferenceType timeType, Angle gstAtUt0, AngularRate earthRotationRate, std::vector<double > polarOffsets, DifferenceTypeMod::DifferenceType polarOffsetsType, Tag delayModelFixedParametersId); 
		 
	
	/**
	 * Return true if all required attributes of the value part are equal to their homologues
	 * in x and false otherwise.
	 *
	 * @param x a pointer on the DelayModelVariableParametersRow whose required attributes of the value part 
	 * will be compared with those of this.
	 * @return a boolean.
	 */
	bool equalByRequiredValue(DelayModelVariableParametersRow* x) ;
	
#ifndef WITHOUT_ACS
	/**
	 * Return this row in the form of an IDL struct.
	 * @return The values of this row as a DelayModelVariableParametersRowIDL struct.
	 */
	asdmIDL::DelayModelVariableParametersRowIDL *toIDL() const;
	
	/**
	 * Define the content of a DelayModelVariableParametersRowIDL struct from the values
	 * found in this row.
	 *
	 * @param x a reference to the DelayModelVariableParametersRowIDL struct to be set.
	 *
	 */
	 void toIDL(asdmIDL::DelayModelVariableParametersRowIDL& x) const;
#endif
	
#ifndef WITHOUT_ACS
	/**
	 * Fill the values of this row from the IDL struct DelayModelVariableParametersRowIDL.
	 * @param x The IDL struct containing the values used to fill this row.
	 * @throws ConversionException
	 */
	void setFromIDL (asdmIDL::DelayModelVariableParametersRowIDL x) ;
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

	std::map<std::string, DelayModelVariableParametersAttributeFromBin> fromBinMethods;
void delayModelVariableParametersIdFromBin( EndianIStream& eis);
void timeFromBin( EndianIStream& eis);
void ut1_utcFromBin( EndianIStream& eis);
void iat_utcFromBin( EndianIStream& eis);
void timeTypeFromBin( EndianIStream& eis);
void gstAtUt0FromBin( EndianIStream& eis);
void earthRotationRateFromBin( EndianIStream& eis);
void polarOffsetsFromBin( EndianIStream& eis);
void polarOffsetsTypeFromBin( EndianIStream& eis);
void delayModelFixedParametersIdFromBin( EndianIStream& eis);

void nutationInLongitudeFromBin( EndianIStream& eis);
void nutationInLongitudeRateFromBin( EndianIStream& eis);
void nutationInObliquityFromBin( EndianIStream& eis);
void nutationInObliquityRateFromBin( EndianIStream& eis);


	 /**
	  * Deserialize a stream of bytes read from an EndianIStream to build a PointingRow.
	  * @param eiss the EndianIStream to be read.
	  * @param table the DelayModelVariableParametersTable to which the row built by deserialization will be parented.
	  * @param attributesSeq a vector containing the names of the attributes . The elements order defines the order 
	  * in which the attributes are written in the binary serialization.
	  */
	 static DelayModelVariableParametersRow* fromBin(EndianIStream& eis, DelayModelVariableParametersTable& table, const std::vector<std::string>& attributesSeq);	 
 
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
	DelayModelVariableParametersTable &table;
	/**
	 * Whether this row has been added to the table or not.
	 */
	bool hasBeenAdded;

	// This method is used by the Table class when this row is added to the table.
	void isAdded(bool added);


	/**
	 * Create a DelayModelVariableParametersRow.
	 * <p>
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 */ 
	DelayModelVariableParametersRow (DelayModelVariableParametersTable &table);

	/**
	 * Create a DelayModelVariableParametersRow using a copy constructor mechanism.
	 * <p>
	 * Given a DelayModelVariableParametersRow row and a DelayModelVariableParametersTable table, the method creates a new
	 * DelayModelVariableParametersRow owned by table. Each attribute of the created row is a copy (deep)
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
	 DelayModelVariableParametersRow (DelayModelVariableParametersTable &table, DelayModelVariableParametersRow *row);
	 	
	////////////////////////////////
	// Intrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute delayModelVariableParametersId
	
	

	Tag delayModelVariableParametersId;

	
	
 	
 	/**
 	 * Set delayModelVariableParametersId with the specified Tag value.
 	 * @param delayModelVariableParametersId The Tag value to which delayModelVariableParametersId is to be set.
		
 		
			
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 		
 	 */
 	void setDelayModelVariableParametersId (Tag delayModelVariableParametersId);
  		
	

	
	// ===> Attribute time
	
	

	ArrayTime time;

	
	
 	

	
	// ===> Attribute ut1_utc
	
	

	double ut1_utc;

	
	
 	

	
	// ===> Attribute iat_utc
	
	

	double iat_utc;

	
	
 	

	
	// ===> Attribute timeType
	
	

	DifferenceTypeMod::DifferenceType timeType;

	
	
 	

	
	// ===> Attribute gstAtUt0
	
	

	Angle gstAtUt0;

	
	
 	

	
	// ===> Attribute earthRotationRate
	
	

	AngularRate earthRotationRate;

	
	
 	

	
	// ===> Attribute polarOffsets
	
	

	std::vector<double > polarOffsets;

	
	
 	

	
	// ===> Attribute polarOffsetsType
	
	

	DifferenceTypeMod::DifferenceType polarOffsetsType;

	
	
 	

	
	// ===> Attribute nutationInLongitude, which is optional
	
	
	bool nutationInLongitudeExists;
	

	Angle nutationInLongitude;

	
	
 	

	
	// ===> Attribute nutationInLongitudeRate, which is optional
	
	
	bool nutationInLongitudeRateExists;
	

	AngularRate nutationInLongitudeRate;

	
	
 	

	
	// ===> Attribute nutationInObliquity, which is optional
	
	
	bool nutationInObliquityExists;
	

	Angle nutationInObliquity;

	
	
 	

	
	// ===> Attribute nutationInObliquityRate, which is optional
	
	
	bool nutationInObliquityRateExists;
	

	AngularRate nutationInObliquityRate;

	
	
 	

	////////////////////////////////
	// Extrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute delayModelFixedParametersId
	
	

	Tag delayModelFixedParametersId;

	
	
 	

	///////////
	// Links //
	///////////
	
	
		

	 

	

	
/*
	////////////////////////////////////////////////////////////
	// binary-deserialization material from an EndianIStream  //
	////////////////////////////////////////////////////////////
	std::map<std::string, DelayModelVariableParametersAttributeFromBin> fromBinMethods;
void delayModelVariableParametersIdFromBin( EndianIStream& eis);
void timeFromBin( EndianIStream& eis);
void ut1_utcFromBin( EndianIStream& eis);
void iat_utcFromBin( EndianIStream& eis);
void timeTypeFromBin( EndianIStream& eis);
void gstAtUt0FromBin( EndianIStream& eis);
void earthRotationRateFromBin( EndianIStream& eis);
void polarOffsetsFromBin( EndianIStream& eis);
void polarOffsetsTypeFromBin( EndianIStream& eis);
void delayModelFixedParametersIdFromBin( EndianIStream& eis);

void nutationInLongitudeFromBin( EndianIStream& eis);
void nutationInLongitudeRateFromBin( EndianIStream& eis);
void nutationInObliquityFromBin( EndianIStream& eis);
void nutationInObliquityRateFromBin( EndianIStream& eis);

*/
	
	///////////////////////////////////
	// text-deserialization material //
	///////////////////////////////////
	std::map<std::string, DelayModelVariableParametersAttributeFromText> fromTextMethods;
	
void delayModelVariableParametersIdFromText (const std::string & s);
	
	
void timeFromText (const std::string & s);
	
	
void ut1_utcFromText (const std::string & s);
	
	
void iat_utcFromText (const std::string & s);
	
	
void timeTypeFromText (const std::string & s);
	
	
void gstAtUt0FromText (const std::string & s);
	
	
void earthRotationRateFromText (const std::string & s);
	
	
void polarOffsetsFromText (const std::string & s);
	
	
void polarOffsetsTypeFromText (const std::string & s);
	
	
void delayModelFixedParametersIdFromText (const std::string & s);
	

	
void nutationInLongitudeFromText (const std::string & s);
	
	
void nutationInLongitudeRateFromText (const std::string & s);
	
	
void nutationInObliquityFromText (const std::string & s);
	
	
void nutationInObliquityRateFromText (const std::string & s);
	
	
	
	/**
	 * Serialize this into a stream of bytes written to an EndianOSStream.
	 * @param eoss the EndianOSStream to be written to
	 */
	 void toBin(EndianOSStream& eoss);
	 	 
	 /**
	  * Deserialize a stream of bytes read from an EndianIStream to build a PointingRow.
	  * @param eiss the EndianIStream to be read.
	  * @param table the DelayModelVariableParametersTable to which the row built by deserialization will be parented.
	  * @param attributesSeq a vector containing the names of the attributes . The elements order defines the order 
	  * in which the attributes are written in the binary serialization.

	 static DelayModelVariableParametersRow* fromBin(EndianIStream& eis, DelayModelVariableParametersTable& table, const std::vector<std::string>& attributesSeq);	 
		*/
};

} // End namespace asdm

#endif /* DelayModelVariableParameters_CLASS */
