
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
 * File CalAmpliRow.h
 */
 
#ifndef CalAmpliRow_CLASS
#define CalAmpliRow_CLASS

#include <vector>
#include <string>
#include <set>

#ifndef WITHOUT_ACS
#include <asdmIDLC.h>
#endif






	 
#include <alma/ASDM/ArrayTime.h>
	

	 
#include <alma/ASDM/Frequency.h>
	

	 
#include <alma/ASDM/Tag.h>
	




	

	
#include <alma/Enumerations/CAtmPhaseCorrection.h>
	

	
#include <alma/Enumerations/CReceiverBand.h>
	

	
#include <alma/Enumerations/CBasebandName.h>
	

	

	
#include <alma/Enumerations/CPolarizationType.h>
	

	

	

	

	

	

	



#include <alma/ASDM/ConversionException.h>
#include <alma/ASDM/NoSuchRow.h>
#include <alma/ASDM/IllegalAccessException.h>

#include <alma/ASDM/RowTransformer.h>
//#include <alma/ASDM/TableStreamReader.h>

/*\file CalAmpli.h
    \brief Generated from model's revision "-1", branch ""
*/

namespace asdm {

//class asdm::CalAmpliTable;


// class asdm::CalDataRow;
class CalDataRow;

// class asdm::CalReductionRow;
class CalReductionRow;
	

class CalAmpliRow;
typedef void (CalAmpliRow::*CalAmpliAttributeFromBin) (EndianIStream& eis);
typedef void (CalAmpliRow::*CalAmpliAttributeFromText) (const std::string& s);

/**
 * The CalAmpliRow class is a row of a CalAmpliTable.
 * 
 * Generated from model's revision "-1", branch ""
 *
 */
class CalAmpliRow {
friend class asdm::CalAmpliTable;
friend class asdm::RowTransformer<CalAmpliRow>;
//friend class asdm::TableStreamReader<CalAmpliTable, CalAmpliRow>;

public:

	virtual ~CalAmpliRow();

	/**
	 * Return the table to which this row belongs.
	 */
	CalAmpliTable &getTable() const;
	
	/**
	 * Has this row been added to its table ?
	 * @return true if and only if it has been added.
	 */
	bool isAdded() const;
		
	////////////////////////////////
	// Intrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute antennaName
	
	
	

	
 	/**
 	 * Get antennaName.
 	 * @return antennaName as std::string
 	 */
 	std::string getAntennaName() const;
	
 
 	
 	
 	/**
 	 * Set antennaName with the specified std::string.
 	 * @param antennaName The std::string value to which antennaName is to be set.
 	 
 		
 			
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 		
 	 */
 	void setAntennaName (std::string antennaName);
  		
	
	
	


	
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
  		
	
	
	


	
	// ===> Attribute receiverBand
	
	
	

	
 	/**
 	 * Get receiverBand.
 	 * @return receiverBand as ReceiverBandMod::ReceiverBand
 	 */
 	ReceiverBandMod::ReceiverBand getReceiverBand() const;
	
 
 	
 	
 	/**
 	 * Set receiverBand with the specified ReceiverBandMod::ReceiverBand.
 	 * @param receiverBand The ReceiverBandMod::ReceiverBand value to which receiverBand is to be set.
 	 
 		
 			
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 		
 	 */
 	void setReceiverBand (ReceiverBandMod::ReceiverBand receiverBand);
  		
	
	
	


	
	// ===> Attribute basebandName
	
	
	

	
 	/**
 	 * Get basebandName.
 	 * @return basebandName as BasebandNameMod::BasebandName
 	 */
 	BasebandNameMod::BasebandName getBasebandName() const;
	
 
 	
 	
 	/**
 	 * Set basebandName with the specified BasebandNameMod::BasebandName.
 	 * @param basebandName The BasebandNameMod::BasebandName value to which basebandName is to be set.
 	 
 		
 			
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 		
 	 */
 	void setBasebandName (BasebandNameMod::BasebandName basebandName);
  		
	
	
	


	
	// ===> Attribute numReceptor
	
	
	

	
 	/**
 	 * Get numReceptor.
 	 * @return numReceptor as int
 	 */
 	int getNumReceptor() const;
	
 
 	
 	
 	/**
 	 * Set numReceptor with the specified int.
 	 * @param numReceptor The int value to which numReceptor is to be set.
 	 
 		
 			
 	 */
 	void setNumReceptor (int numReceptor);
  		
	
	
	


	
	// ===> Attribute polarizationTypes
	
	
	

	
 	/**
 	 * Get polarizationTypes.
 	 * @return polarizationTypes as std::vector<PolarizationTypeMod::PolarizationType >
 	 */
 	std::vector<PolarizationTypeMod::PolarizationType > getPolarizationTypes() const;
	
 
 	
 	
 	/**
 	 * Set polarizationTypes with the specified std::vector<PolarizationTypeMod::PolarizationType >.
 	 * @param polarizationTypes The std::vector<PolarizationTypeMod::PolarizationType > value to which polarizationTypes is to be set.
 	 
 		
 			
 	 */
 	void setPolarizationTypes (std::vector<PolarizationTypeMod::PolarizationType > polarizationTypes);
  		
	
	
	


	
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
  		
	
	
	


	
	// ===> Attribute apertureEfficiency
	
	
	

	
 	/**
 	 * Get apertureEfficiency.
 	 * @return apertureEfficiency as std::vector<float >
 	 */
 	std::vector<float > getApertureEfficiency() const;
	
 
 	
 	
 	/**
 	 * Set apertureEfficiency with the specified std::vector<float >.
 	 * @param apertureEfficiency The std::vector<float > value to which apertureEfficiency is to be set.
 	 
 		
 			
 	 */
 	void setApertureEfficiency (std::vector<float > apertureEfficiency);
  		
	
	
	


	
	// ===> Attribute apertureEfficiencyError
	
	
	

	
 	/**
 	 * Get apertureEfficiencyError.
 	 * @return apertureEfficiencyError as std::vector<float >
 	 */
 	std::vector<float > getApertureEfficiencyError() const;
	
 
 	
 	
 	/**
 	 * Set apertureEfficiencyError with the specified std::vector<float >.
 	 * @param apertureEfficiencyError The std::vector<float > value to which apertureEfficiencyError is to be set.
 	 
 		
 			
 	 */
 	void setApertureEfficiencyError (std::vector<float > apertureEfficiencyError);
  		
	
	
	


	
	// ===> Attribute correctionValidity, which is optional
	
	
	
	/**
	 * The attribute correctionValidity is optional. Return true if this attribute exists.
	 * @return true if and only if the correctionValidity attribute exists. 
	 */
	bool isCorrectionValidityExists() const;
	

	
 	/**
 	 * Get correctionValidity, which is optional.
 	 * @return correctionValidity as bool
 	 * @throws IllegalAccessException If correctionValidity does not exist.
 	 */
 	bool getCorrectionValidity() const;
	
 
 	
 	
 	/**
 	 * Set correctionValidity with the specified bool.
 	 * @param correctionValidity The bool value to which correctionValidity is to be set.
 	 
 		
 	 */
 	void setCorrectionValidity (bool correctionValidity);
		
	
	
	
	/**
	 * Mark correctionValidity, which is an optional field, as non-existent.
	 */
	void clearCorrectionValidity ();
	


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
	 * Compare each mandatory attribute except the autoincrementable one of this CalAmpliRow with 
	 * the corresponding parameters and return true if there is a match and false otherwise.
	 	
	 * @param antennaName
	    
	 * @param atmPhaseCorrection
	    
	 * @param receiverBand
	    
	 * @param basebandName
	    
	 * @param calDataId
	    
	 * @param calReductionId
	    
	 * @param numReceptor
	    
	 * @param polarizationTypes
	    
	 * @param startValidTime
	    
	 * @param endValidTime
	    
	 * @param frequencyRange
	    
	 * @param apertureEfficiency
	    
	 * @param apertureEfficiencyError
	    
	 */ 
	bool compareNoAutoInc(std::string antennaName, AtmPhaseCorrectionMod::AtmPhaseCorrection atmPhaseCorrection, ReceiverBandMod::ReceiverBand receiverBand, BasebandNameMod::BasebandName basebandName, Tag calDataId, Tag calReductionId, int numReceptor, std::vector<PolarizationTypeMod::PolarizationType > polarizationTypes, ArrayTime startValidTime, ArrayTime endValidTime, std::vector<Frequency > frequencyRange, std::vector<float > apertureEfficiency, std::vector<float > apertureEfficiencyError);
	
	

	
	/**
	 * Compare each mandatory value (i.e. not in the key) attribute  with 
	 * the corresponding parameters and return true if there is a match and false otherwise.
	 	
	 * @param numReceptor
	    
	 * @param polarizationTypes
	    
	 * @param startValidTime
	    
	 * @param endValidTime
	    
	 * @param frequencyRange
	    
	 * @param apertureEfficiency
	    
	 * @param apertureEfficiencyError
	    
	 */ 
	bool compareRequiredValue(int numReceptor, std::vector<PolarizationTypeMod::PolarizationType > polarizationTypes, ArrayTime startValidTime, ArrayTime endValidTime, std::vector<Frequency > frequencyRange, std::vector<float > apertureEfficiency, std::vector<float > apertureEfficiencyError); 
		 
	
	/**
	 * Return true if all required attributes of the value part are equal to their homologues
	 * in x and false otherwise.
	 *
	 * @param x a pointer on the CalAmpliRow whose required attributes of the value part 
	 * will be compared with those of this.
	 * @return a boolean.
	 */
	bool equalByRequiredValue(CalAmpliRow* x) ;
	
#ifndef WITHOUT_ACS
	/**
	 * Return this row in the form of an IDL struct.
	 * @return The values of this row as a CalAmpliRowIDL struct.
	 */
	asdmIDL::CalAmpliRowIDL *toIDL() const;
	
	/**
	 * Define the content of a CalAmpliRowIDL struct from the values
	 * found in this row.
	 *
	 * @param x a reference to the CalAmpliRowIDL struct to be set.
	 *
	 */
	 void toIDL(asdmIDL::CalAmpliRowIDL& x) const;
#endif
	
#ifndef WITHOUT_ACS
	/**
	 * Fill the values of this row from the IDL struct CalAmpliRowIDL.
	 * @param x The IDL struct containing the values used to fill this row.
	 * @throws ConversionException
	 */
	void setFromIDL (asdmIDL::CalAmpliRowIDL x) ;
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

	std::map<std::string, CalAmpliAttributeFromBin> fromBinMethods;
void antennaNameFromBin( EndianIStream& eis);
void atmPhaseCorrectionFromBin( EndianIStream& eis);
void receiverBandFromBin( EndianIStream& eis);
void basebandNameFromBin( EndianIStream& eis);
void calDataIdFromBin( EndianIStream& eis);
void calReductionIdFromBin( EndianIStream& eis);
void numReceptorFromBin( EndianIStream& eis);
void polarizationTypesFromBin( EndianIStream& eis);
void startValidTimeFromBin( EndianIStream& eis);
void endValidTimeFromBin( EndianIStream& eis);
void frequencyRangeFromBin( EndianIStream& eis);
void apertureEfficiencyFromBin( EndianIStream& eis);
void apertureEfficiencyErrorFromBin( EndianIStream& eis);

void correctionValidityFromBin( EndianIStream& eis);


	 /**
	  * Deserialize a stream of bytes read from an EndianIStream to build a PointingRow.
	  * @param eiss the EndianIStream to be read.
	  * @param table the CalAmpliTable to which the row built by deserialization will be parented.
	  * @param attributesSeq a vector containing the names of the attributes . The elements order defines the order 
	  * in which the attributes are written in the binary serialization.
	  */
	 static CalAmpliRow* fromBin(EndianIStream& eis, CalAmpliTable& table, const std::vector<std::string>& attributesSeq);	 
 
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
	CalAmpliTable &table;
	/**
	 * Whether this row has been added to the table or not.
	 */
	bool hasBeenAdded;

	// This method is used by the Table class when this row is added to the table.
	void isAdded(bool added);


	/**
	 * Create a CalAmpliRow.
	 * <p>
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 */ 
	CalAmpliRow (CalAmpliTable &table);

	/**
	 * Create a CalAmpliRow using a copy constructor mechanism.
	 * <p>
	 * Given a CalAmpliRow row and a CalAmpliTable table, the method creates a new
	 * CalAmpliRow owned by table. Each attribute of the created row is a copy (deep)
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
	 CalAmpliRow (CalAmpliTable &table, CalAmpliRow *row);
	 	
	////////////////////////////////
	// Intrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute antennaName
	
	

	std::string antennaName;

	
	
 	

	
	// ===> Attribute atmPhaseCorrection
	
	

	AtmPhaseCorrectionMod::AtmPhaseCorrection atmPhaseCorrection;

	
	
 	

	
	// ===> Attribute receiverBand
	
	

	ReceiverBandMod::ReceiverBand receiverBand;

	
	
 	

	
	// ===> Attribute basebandName
	
	

	BasebandNameMod::BasebandName basebandName;

	
	
 	

	
	// ===> Attribute numReceptor
	
	

	int numReceptor;

	
	
 	

	
	// ===> Attribute polarizationTypes
	
	

	std::vector<PolarizationTypeMod::PolarizationType > polarizationTypes;

	
	
 	

	
	// ===> Attribute startValidTime
	
	

	ArrayTime startValidTime;

	
	
 	

	
	// ===> Attribute endValidTime
	
	

	ArrayTime endValidTime;

	
	
 	

	
	// ===> Attribute frequencyRange
	
	

	std::vector<Frequency > frequencyRange;

	
	
 	

	
	// ===> Attribute apertureEfficiency
	
	

	std::vector<float > apertureEfficiency;

	
	
 	

	
	// ===> Attribute apertureEfficiencyError
	
	

	std::vector<float > apertureEfficiencyError;

	
	
 	

	
	// ===> Attribute correctionValidity, which is optional
	
	
	bool correctionValidityExists;
	

	bool correctionValidity;

	
	
 	

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
	std::map<std::string, CalAmpliAttributeFromBin> fromBinMethods;
void antennaNameFromBin( EndianIStream& eis);
void atmPhaseCorrectionFromBin( EndianIStream& eis);
void receiverBandFromBin( EndianIStream& eis);
void basebandNameFromBin( EndianIStream& eis);
void calDataIdFromBin( EndianIStream& eis);
void calReductionIdFromBin( EndianIStream& eis);
void numReceptorFromBin( EndianIStream& eis);
void polarizationTypesFromBin( EndianIStream& eis);
void startValidTimeFromBin( EndianIStream& eis);
void endValidTimeFromBin( EndianIStream& eis);
void frequencyRangeFromBin( EndianIStream& eis);
void apertureEfficiencyFromBin( EndianIStream& eis);
void apertureEfficiencyErrorFromBin( EndianIStream& eis);

void correctionValidityFromBin( EndianIStream& eis);

*/
	
	///////////////////////////////////
	// text-deserialization material //
	///////////////////////////////////
	std::map<std::string, CalAmpliAttributeFromText> fromTextMethods;
	
void antennaNameFromText (const std::string & s);
	
	
void atmPhaseCorrectionFromText (const std::string & s);
	
	
void receiverBandFromText (const std::string & s);
	
	
void basebandNameFromText (const std::string & s);
	
	
void calDataIdFromText (const std::string & s);
	
	
void calReductionIdFromText (const std::string & s);
	
	
void numReceptorFromText (const std::string & s);
	
	
void polarizationTypesFromText (const std::string & s);
	
	
void startValidTimeFromText (const std::string & s);
	
	
void endValidTimeFromText (const std::string & s);
	
	
void frequencyRangeFromText (const std::string & s);
	
	
void apertureEfficiencyFromText (const std::string & s);
	
	
void apertureEfficiencyErrorFromText (const std::string & s);
	

	
void correctionValidityFromText (const std::string & s);
	
	
	
	/**
	 * Serialize this into a stream of bytes written to an EndianOSStream.
	 * @param eoss the EndianOSStream to be written to
	 */
	 void toBin(EndianOSStream& eoss);
	 	 
	 /**
	  * Deserialize a stream of bytes read from an EndianIStream to build a PointingRow.
	  * @param eiss the EndianIStream to be read.
	  * @param table the CalAmpliTable to which the row built by deserialization will be parented.
	  * @param attributesSeq a vector containing the names of the attributes . The elements order defines the order 
	  * in which the attributes are written in the binary serialization.

	 static CalAmpliRow* fromBin(EndianIStream& eis, CalAmpliTable& table, const std::vector<std::string>& attributesSeq);	 
		*/
};

} // End namespace asdm

#endif /* CalAmpli_CLASS */
