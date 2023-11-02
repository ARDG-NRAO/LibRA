
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
 * File CalSeeingRow.h
 */
 
#ifndef CalSeeingRow_CLASS
#define CalSeeingRow_CLASS

#include <vector>
#include <string>
#include <set>

#ifndef WITHOUT_ACS
#include <asdmIDLC.h>
#endif






	 
#include <alma/ASDM/Angle.h>
	

	 
#include <alma/ASDM/ArrayTime.h>
	

	 
#include <alma/ASDM/Length.h>
	

	 
#include <alma/ASDM/Frequency.h>
	

	 
#include <alma/ASDM/Tag.h>
	

	 
#include <alma/ASDM/Interval.h>
	




	
#include <alma/Enumerations/CAtmPhaseCorrection.h>
	

	

	

	

	

	

	

	

	

	

	

	

	



#include <alma/ASDM/ConversionException.h>
#include <alma/ASDM/NoSuchRow.h>
#include <alma/ASDM/IllegalAccessException.h>

#include <alma/ASDM/RowTransformer.h>
//#include <alma/ASDM/TableStreamReader.h>

/*\file CalSeeing.h
    \brief Generated from model's revision "-1", branch ""
*/

namespace asdm {

//class asdm::CalSeeingTable;


// class asdm::CalDataRow;
class CalDataRow;

// class asdm::CalReductionRow;
class CalReductionRow;
	

class CalSeeingRow;
typedef void (CalSeeingRow::*CalSeeingAttributeFromBin) (EndianIStream& eis);
typedef void (CalSeeingRow::*CalSeeingAttributeFromText) (const std::string& s);

/**
 * The CalSeeingRow class is a row of a CalSeeingTable.
 * 
 * Generated from model's revision "-1", branch ""
 *
 */
class CalSeeingRow {
friend class asdm::CalSeeingTable;
friend class asdm::RowTransformer<CalSeeingRow>;
//friend class asdm::TableStreamReader<CalSeeingTable, CalSeeingRow>;

public:

	virtual ~CalSeeingRow();

	/**
	 * Return the table to which this row belongs.
	 */
	CalSeeingTable &getTable() const;
	
	/**
	 * Has this row been added to its table ?
	 * @return true if and only if it has been added.
	 */
	bool isAdded() const;
		
	////////////////////////////////
	// Intrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute atmPhaseCorrection
	
	
	

	
 	/**
 	 * Get atmPhaseCorrection.
 	 * @return atmPhaseCorrection as AtmPhaseCorrectionMod::AtmPhaseCorrection
 	 */
 	AtmPhaseCorrectionMod::AtmPhaseCorrection getAtmPhaseCorrection() const;
	
 
 	
 	
 	/**
 	 * Set atmPhaseCorrection with the specified AtmPhaseCorrectionMod::AtmPhaseCorrection.
 	 * @param atmPhaseCorrection The AtmPhaseCorrectionMod::AtmPhaseCorrection value to which atmPhaseCorrection is to be set.
 	 
 		
 			
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 		
 	 */
 	void setAtmPhaseCorrection (AtmPhaseCorrectionMod::AtmPhaseCorrection atmPhaseCorrection);
  		
	
	
	


	
	// ===> Attribute startValidTime
	
	
	

	
 	/**
 	 * Get startValidTime.
 	 * @return startValidTime as ArrayTime
 	 */
 	ArrayTime getStartValidTime() const;
	
 
 	
 	
 	/**
 	 * Set startValidTime with the specified ArrayTime.
 	 * @param startValidTime The ArrayTime value to which startValidTime is to be set.
 	 
 		
 			
 	 */
 	void setStartValidTime (ArrayTime startValidTime);
  		
	
	
	


	
	// ===> Attribute endValidTime
	
	
	

	
 	/**
 	 * Get endValidTime.
 	 * @return endValidTime as ArrayTime
 	 */
 	ArrayTime getEndValidTime() const;
	
 
 	
 	
 	/**
 	 * Set endValidTime with the specified ArrayTime.
 	 * @param endValidTime The ArrayTime value to which endValidTime is to be set.
 	 
 		
 			
 	 */
 	void setEndValidTime (ArrayTime endValidTime);
  		
	
	
	


	
	// ===> Attribute frequencyRange
	
	
	

	
 	/**
 	 * Get frequencyRange.
 	 * @return frequencyRange as std::vector<Frequency >
 	 */
 	std::vector<Frequency > getFrequencyRange() const;
	
 
 	
 	
 	/**
 	 * Set frequencyRange with the specified std::vector<Frequency >.
 	 * @param frequencyRange The std::vector<Frequency > value to which frequencyRange is to be set.
 	 
 		
 			
 	 */
 	void setFrequencyRange (std::vector<Frequency > frequencyRange);
  		
	
	
	


	
	// ===> Attribute integrationTime
	
	
	

	
 	/**
 	 * Get integrationTime.
 	 * @return integrationTime as Interval
 	 */
 	Interval getIntegrationTime() const;
	
 
 	
 	
 	/**
 	 * Set integrationTime with the specified Interval.
 	 * @param integrationTime The Interval value to which integrationTime is to be set.
 	 
 		
 			
 	 */
 	void setIntegrationTime (Interval integrationTime);
  		
	
	
	


	
	// ===> Attribute numBaseLengths
	
	
	

	
 	/**
 	 * Get numBaseLengths.
 	 * @return numBaseLengths as int
 	 */
 	int getNumBaseLengths() const;
	
 
 	
 	
 	/**
 	 * Set numBaseLengths with the specified int.
 	 * @param numBaseLengths The int value to which numBaseLengths is to be set.
 	 
 		
 			
 	 */
 	void setNumBaseLengths (int numBaseLengths);
  		
	
	
	


	
	// ===> Attribute baselineLengths
	
	
	

	
 	/**
 	 * Get baselineLengths.
 	 * @return baselineLengths as std::vector<Length >
 	 */
 	std::vector<Length > getBaselineLengths() const;
	
 
 	
 	
 	/**
 	 * Set baselineLengths with the specified std::vector<Length >.
 	 * @param baselineLengths The std::vector<Length > value to which baselineLengths is to be set.
 	 
 		
 			
 	 */
 	void setBaselineLengths (std::vector<Length > baselineLengths);
  		
	
	
	


	
	// ===> Attribute phaseRMS
	
	
	

	
 	/**
 	 * Get phaseRMS.
 	 * @return phaseRMS as std::vector<Angle >
 	 */
 	std::vector<Angle > getPhaseRMS() const;
	
 
 	
 	
 	/**
 	 * Set phaseRMS with the specified std::vector<Angle >.
 	 * @param phaseRMS The std::vector<Angle > value to which phaseRMS is to be set.
 	 
 		
 			
 	 */
 	void setPhaseRMS (std::vector<Angle > phaseRMS);
  		
	
	
	


	
	// ===> Attribute seeing
	
	
	

	
 	/**
 	 * Get seeing.
 	 * @return seeing as Angle
 	 */
 	Angle getSeeing() const;
	
 
 	
 	
 	/**
 	 * Set seeing with the specified Angle.
 	 * @param seeing The Angle value to which seeing is to be set.
 	 
 		
 			
 	 */
 	void setSeeing (Angle seeing);
  		
	
	
	


	
	// ===> Attribute seeingError
	
	
	

	
 	/**
 	 * Get seeingError.
 	 * @return seeingError as Angle
 	 */
 	Angle getSeeingError() const;
	
 
 	
 	
 	/**
 	 * Set seeingError with the specified Angle.
 	 * @param seeingError The Angle value to which seeingError is to be set.
 	 
 		
 			
 	 */
 	void setSeeingError (Angle seeingError);
  		
	
	
	


	
	// ===> Attribute exponent, which is optional
	
	
	
	/**
	 * The attribute exponent is optional. Return true if this attribute exists.
	 * @return true if and only if the exponent attribute exists. 
	 */
	bool isExponentExists() const;
	

	
 	/**
 	 * Get exponent, which is optional.
 	 * @return exponent as float
 	 * @throws IllegalAccessException If exponent does not exist.
 	 */
 	float getExponent() const;
	
 
 	
 	
 	/**
 	 * Set exponent with the specified float.
 	 * @param exponent The float value to which exponent is to be set.
 	 
 		
 	 */
 	void setExponent (float exponent);
		
	
	
	
	/**
	 * Mark exponent, which is an optional field, as non-existent.
	 */
	void clearExponent ();
	


	
	// ===> Attribute outerScale, which is optional
	
	
	
	/**
	 * The attribute outerScale is optional. Return true if this attribute exists.
	 * @return true if and only if the outerScale attribute exists. 
	 */
	bool isOuterScaleExists() const;
	

	
 	/**
 	 * Get outerScale, which is optional.
 	 * @return outerScale as Length
 	 * @throws IllegalAccessException If outerScale does not exist.
 	 */
 	Length getOuterScale() const;
	
 
 	
 	
 	/**
 	 * Set outerScale with the specified Length.
 	 * @param outerScale The Length value to which outerScale is to be set.
 	 
 		
 	 */
 	void setOuterScale (Length outerScale);
		
	
	
	
	/**
	 * Mark outerScale, which is an optional field, as non-existent.
	 */
	void clearOuterScale ();
	


	
	// ===> Attribute outerScaleRMS, which is optional
	
	
	
	/**
	 * The attribute outerScaleRMS is optional. Return true if this attribute exists.
	 * @return true if and only if the outerScaleRMS attribute exists. 
	 */
	bool isOuterScaleRMSExists() const;
	

	
 	/**
 	 * Get outerScaleRMS, which is optional.
 	 * @return outerScaleRMS as Angle
 	 * @throws IllegalAccessException If outerScaleRMS does not exist.
 	 */
 	Angle getOuterScaleRMS() const;
	
 
 	
 	
 	/**
 	 * Set outerScaleRMS with the specified Angle.
 	 * @param outerScaleRMS The Angle value to which outerScaleRMS is to be set.
 	 
 		
 	 */
 	void setOuterScaleRMS (Angle outerScaleRMS);
		
	
	
	
	/**
	 * Mark outerScaleRMS, which is an optional field, as non-existent.
	 */
	void clearOuterScaleRMS ();
	


	////////////////////////////////
	// Extrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute calDataId
	
	
	

	
 	/**
 	 * Get calDataId.
 	 * @return calDataId as Tag
 	 */
 	Tag getCalDataId() const;
	
 
 	
 	
 	/**
 	 * Set calDataId with the specified Tag.
 	 * @param calDataId The Tag value to which calDataId is to be set.
 	 
 		
 			
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 		
 	 */
 	void setCalDataId (Tag calDataId);
  		
	
	
	


	
	// ===> Attribute calReductionId
	
	
	

	
 	/**
 	 * Get calReductionId.
 	 * @return calReductionId as Tag
 	 */
 	Tag getCalReductionId() const;
	
 
 	
 	
 	/**
 	 * Set calReductionId with the specified Tag.
 	 * @param calReductionId The Tag value to which calReductionId is to be set.
 	 
 		
 			
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 		
 	 */
 	void setCalReductionId (Tag calReductionId);
  		
	
	
	


	///////////
	// Links //
	///////////
	
	

	
		
	/**
	 * calDataId pointer to the row in the CalData table having CalData.calDataId == calDataId
	 * @return a CalDataRow*
	 * 
	 
	 */
	 CalDataRow* getCalDataUsingCalDataId();
	 

	

	

	
		
	/**
	 * calReductionId pointer to the row in the CalReduction table having CalReduction.calReductionId == calReductionId
	 * @return a CalReductionRow*
	 * 
	 
	 */
	 CalReductionRow* getCalReductionUsingCalReductionId();
	 

	

	
	
	
	/**
	 * Compare each mandatory attribute except the autoincrementable one of this CalSeeingRow with 
	 * the corresponding parameters and return true if there is a match and false otherwise.
	 	
	 * @param atmPhaseCorrection
	    
	 * @param calDataId
	    
	 * @param calReductionId
	    
	 * @param startValidTime
	    
	 * @param endValidTime
	    
	 * @param frequencyRange
	    
	 * @param integrationTime
	    
	 * @param numBaseLengths
	    
	 * @param baselineLengths
	    
	 * @param phaseRMS
	    
	 * @param seeing
	    
	 * @param seeingError
	    
	 */ 
	bool compareNoAutoInc(AtmPhaseCorrectionMod::AtmPhaseCorrection atmPhaseCorrection, Tag calDataId, Tag calReductionId, ArrayTime startValidTime, ArrayTime endValidTime, std::vector<Frequency > frequencyRange, Interval integrationTime, int numBaseLengths, std::vector<Length > baselineLengths, std::vector<Angle > phaseRMS, Angle seeing, Angle seeingError);
	
	

	
	/**
	 * Compare each mandatory value (i.e. not in the key) attribute  with 
	 * the corresponding parameters and return true if there is a match and false otherwise.
	 	
	 * @param startValidTime
	    
	 * @param endValidTime
	    
	 * @param frequencyRange
	    
	 * @param integrationTime
	    
	 * @param numBaseLengths
	    
	 * @param baselineLengths
	    
	 * @param phaseRMS
	    
	 * @param seeing
	    
	 * @param seeingError
	    
	 */ 
	bool compareRequiredValue(ArrayTime startValidTime, ArrayTime endValidTime, std::vector<Frequency > frequencyRange, Interval integrationTime, int numBaseLengths, std::vector<Length > baselineLengths, std::vector<Angle > phaseRMS, Angle seeing, Angle seeingError); 
		 
	
	/**
	 * Return true if all required attributes of the value part are equal to their homologues
	 * in x and false otherwise.
	 *
	 * @param x a pointer on the CalSeeingRow whose required attributes of the value part 
	 * will be compared with those of this.
	 * @return a boolean.
	 */
	bool equalByRequiredValue(CalSeeingRow* x) ;
	
#ifndef WITHOUT_ACS
	/**
	 * Return this row in the form of an IDL struct.
	 * @return The values of this row as a CalSeeingRowIDL struct.
	 */
	asdmIDL::CalSeeingRowIDL *toIDL() const;
	
	/**
	 * Define the content of a CalSeeingRowIDL struct from the values
	 * found in this row.
	 *
	 * @param x a reference to the CalSeeingRowIDL struct to be set.
	 *
	 */
	 void toIDL(asdmIDL::CalSeeingRowIDL& x) const;
#endif
	
#ifndef WITHOUT_ACS
	/**
	 * Fill the values of this row from the IDL struct CalSeeingRowIDL.
	 * @param x The IDL struct containing the values used to fill this row.
	 * @throws ConversionException
	 */
	void setFromIDL (asdmIDL::CalSeeingRowIDL x) ;
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

	std::map<std::string, CalSeeingAttributeFromBin> fromBinMethods;
void atmPhaseCorrectionFromBin( EndianIStream& eis);
void calDataIdFromBin( EndianIStream& eis);
void calReductionIdFromBin( EndianIStream& eis);
void startValidTimeFromBin( EndianIStream& eis);
void endValidTimeFromBin( EndianIStream& eis);
void frequencyRangeFromBin( EndianIStream& eis);
void integrationTimeFromBin( EndianIStream& eis);
void numBaseLengthsFromBin( EndianIStream& eis);
void baselineLengthsFromBin( EndianIStream& eis);
void phaseRMSFromBin( EndianIStream& eis);
void seeingFromBin( EndianIStream& eis);
void seeingErrorFromBin( EndianIStream& eis);

void exponentFromBin( EndianIStream& eis);
void outerScaleFromBin( EndianIStream& eis);
void outerScaleRMSFromBin( EndianIStream& eis);


	 /**
	  * Deserialize a stream of bytes read from an EndianIStream to build a PointingRow.
	  * @param eiss the EndianIStream to be read.
	  * @param table the CalSeeingTable to which the row built by deserialization will be parented.
	  * @param attributesSeq a vector containing the names of the attributes . The elements order defines the order 
	  * in which the attributes are written in the binary serialization.
	  */
	 static CalSeeingRow* fromBin(EndianIStream& eis, CalSeeingTable& table, const std::vector<std::string>& attributesSeq);	 
 
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
	CalSeeingTable &table;
	/**
	 * Whether this row has been added to the table or not.
	 */
	bool hasBeenAdded;

	// This method is used by the Table class when this row is added to the table.
	void isAdded(bool added);


	/**
	 * Create a CalSeeingRow.
	 * <p>
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 */ 
	CalSeeingRow (CalSeeingTable &table);

	/**
	 * Create a CalSeeingRow using a copy constructor mechanism.
	 * <p>
	 * Given a CalSeeingRow row and a CalSeeingTable table, the method creates a new
	 * CalSeeingRow owned by table. Each attribute of the created row is a copy (deep)
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
	 CalSeeingRow (CalSeeingTable &table, CalSeeingRow *row);
	 	
	////////////////////////////////
	// Intrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute atmPhaseCorrection
	
	

	AtmPhaseCorrectionMod::AtmPhaseCorrection atmPhaseCorrection;

	
	
 	

	
	// ===> Attribute startValidTime
	
	

	ArrayTime startValidTime;

	
	
 	

	
	// ===> Attribute endValidTime
	
	

	ArrayTime endValidTime;

	
	
 	

	
	// ===> Attribute frequencyRange
	
	

	std::vector<Frequency > frequencyRange;

	
	
 	

	
	// ===> Attribute integrationTime
	
	

	Interval integrationTime;

	
	
 	

	
	// ===> Attribute numBaseLengths
	
	

	int numBaseLengths;

	
	
 	

	
	// ===> Attribute baselineLengths
	
	

	std::vector<Length > baselineLengths;

	
	
 	

	
	// ===> Attribute phaseRMS
	
	

	std::vector<Angle > phaseRMS;

	
	
 	

	
	// ===> Attribute seeing
	
	

	Angle seeing;

	
	
 	

	
	// ===> Attribute seeingError
	
	

	Angle seeingError;

	
	
 	

	
	// ===> Attribute exponent, which is optional
	
	
	bool exponentExists;
	

	float exponent;

	
	
 	

	
	// ===> Attribute outerScale, which is optional
	
	
	bool outerScaleExists;
	

	Length outerScale;

	
	
 	

	
	// ===> Attribute outerScaleRMS, which is optional
	
	
	bool outerScaleRMSExists;
	

	Angle outerScaleRMS;

	
	
 	

	////////////////////////////////
	// Extrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute calDataId
	
	

	Tag calDataId;

	
	
 	

	
	// ===> Attribute calReductionId
	
	

	Tag calReductionId;

	
	
 	

	///////////
	// Links //
	///////////
	
	
		

	 

	

	
		

	 

	

	
/*
	////////////////////////////////////////////////////////////
	// binary-deserialization material from an EndianIStream  //
	////////////////////////////////////////////////////////////
	std::map<std::string, CalSeeingAttributeFromBin> fromBinMethods;
void atmPhaseCorrectionFromBin( EndianIStream& eis);
void calDataIdFromBin( EndianIStream& eis);
void calReductionIdFromBin( EndianIStream& eis);
void startValidTimeFromBin( EndianIStream& eis);
void endValidTimeFromBin( EndianIStream& eis);
void frequencyRangeFromBin( EndianIStream& eis);
void integrationTimeFromBin( EndianIStream& eis);
void numBaseLengthsFromBin( EndianIStream& eis);
void baselineLengthsFromBin( EndianIStream& eis);
void phaseRMSFromBin( EndianIStream& eis);
void seeingFromBin( EndianIStream& eis);
void seeingErrorFromBin( EndianIStream& eis);

void exponentFromBin( EndianIStream& eis);
void outerScaleFromBin( EndianIStream& eis);
void outerScaleRMSFromBin( EndianIStream& eis);

*/
	
	///////////////////////////////////
	// text-deserialization material //
	///////////////////////////////////
	std::map<std::string, CalSeeingAttributeFromText> fromTextMethods;
	
void atmPhaseCorrectionFromText (const std::string & s);
	
	
void calDataIdFromText (const std::string & s);
	
	
void calReductionIdFromText (const std::string & s);
	
	
void startValidTimeFromText (const std::string & s);
	
	
void endValidTimeFromText (const std::string & s);
	
	
void frequencyRangeFromText (const std::string & s);
	
	
void integrationTimeFromText (const std::string & s);
	
	
void numBaseLengthsFromText (const std::string & s);
	
	
void baselineLengthsFromText (const std::string & s);
	
	
void phaseRMSFromText (const std::string & s);
	
	
void seeingFromText (const std::string & s);
	
	
void seeingErrorFromText (const std::string & s);
	

	
void exponentFromText (const std::string & s);
	
	
void outerScaleFromText (const std::string & s);
	
	
void outerScaleRMSFromText (const std::string & s);
	
	
	
	/**
	 * Serialize this into a stream of bytes written to an EndianOSStream.
	 * @param eoss the EndianOSStream to be written to
	 */
	 void toBin(EndianOSStream& eoss);
	 	 
	 /**
	  * Deserialize a stream of bytes read from an EndianIStream to build a PointingRow.
	  * @param eiss the EndianIStream to be read.
	  * @param table the CalSeeingTable to which the row built by deserialization will be parented.
	  * @param attributesSeq a vector containing the names of the attributes . The elements order defines the order 
	  * in which the attributes are written in the binary serialization.

	 static CalSeeingRow* fromBin(EndianIStream& eis, CalSeeingTable& table, const std::vector<std::string>& attributesSeq);	 
		*/
};

} // End namespace asdm

#endif /* CalSeeing_CLASS */
