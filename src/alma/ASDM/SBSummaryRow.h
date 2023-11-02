
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
 * File SBSummaryRow.h
 */
 
#ifndef SBSummaryRow_CLASS
#define SBSummaryRow_CLASS

#include <vector>
#include <string>
#include <set>

#ifndef WITHOUT_ACS
#include <asdmIDLC.h>
#endif






	 
#include <alma/ASDM/Angle.h>
	

	 
#include <alma/ASDM/ArrayTime.h>
	

	 
#include <alma/ASDM/Tag.h>
	

	 
#include <alma/ASDM/EntityRef.h>
	

	 
#include <alma/ASDM/Interval.h>
	




	

	

	

	

	

	
#include <alma/Enumerations/CReceiverBand.h>
	

	
#include <alma/Enumerations/CSBType.h>
	

	

	

	

	

	

	

	

	

	

	
#include <alma/Enumerations/CDirectionReferenceCode.h>
	

	



#include <alma/ASDM/ConversionException.h>
#include <alma/ASDM/NoSuchRow.h>
#include <alma/ASDM/IllegalAccessException.h>

#include <alma/ASDM/RowTransformer.h>
//#include <alma/ASDM/TableStreamReader.h>

/*\file SBSummary.h
    \brief Generated from model's revision "-1", branch ""
*/

namespace asdm {

//class asdm::SBSummaryTable;

	

class SBSummaryRow;
typedef void (SBSummaryRow::*SBSummaryAttributeFromBin) (EndianIStream& eis);
typedef void (SBSummaryRow::*SBSummaryAttributeFromText) (const std::string& s);

/**
 * The SBSummaryRow class is a row of a SBSummaryTable.
 * 
 * Generated from model's revision "-1", branch ""
 *
 */
class SBSummaryRow {
friend class asdm::SBSummaryTable;
friend class asdm::RowTransformer<SBSummaryRow>;
//friend class asdm::TableStreamReader<SBSummaryTable, SBSummaryRow>;

public:

	virtual ~SBSummaryRow();

	/**
	 * Return the table to which this row belongs.
	 */
	SBSummaryTable &getTable() const;
	
	/**
	 * Has this row been added to its table ?
	 * @return true if and only if it has been added.
	 */
	bool isAdded() const;
		
	////////////////////////////////
	// Intrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute sBSummaryId
	
	
	

	
 	/**
 	 * Get sBSummaryId.
 	 * @return sBSummaryId as Tag
 	 */
 	Tag getSBSummaryId() const;
	
 
 	
 	
	
	


	
	// ===> Attribute sbSummaryUID
	
	
	

	
 	/**
 	 * Get sbSummaryUID.
 	 * @return sbSummaryUID as EntityRef
 	 */
 	EntityRef getSbSummaryUID() const;
	
 
 	
 	
 	/**
 	 * Set sbSummaryUID with the specified EntityRef.
 	 * @param sbSummaryUID The EntityRef value to which sbSummaryUID is to be set.
 	 
 		
 			
 	 */
 	void setSbSummaryUID (EntityRef sbSummaryUID);
  		
	
	
	


	
	// ===> Attribute projectUID
	
	
	

	
 	/**
 	 * Get projectUID.
 	 * @return projectUID as EntityRef
 	 */
 	EntityRef getProjectUID() const;
	
 
 	
 	
 	/**
 	 * Set projectUID with the specified EntityRef.
 	 * @param projectUID The EntityRef value to which projectUID is to be set.
 	 
 		
 			
 	 */
 	void setProjectUID (EntityRef projectUID);
  		
	
	
	


	
	// ===> Attribute obsUnitSetUID
	
	
	

	
 	/**
 	 * Get obsUnitSetUID.
 	 * @return obsUnitSetUID as EntityRef
 	 */
 	EntityRef getObsUnitSetUID() const;
	
 
 	
 	
 	/**
 	 * Set obsUnitSetUID with the specified EntityRef.
 	 * @param obsUnitSetUID The EntityRef value to which obsUnitSetUID is to be set.
 	 
 		
 			
 	 */
 	void setObsUnitSetUID (EntityRef obsUnitSetUID);
  		
	
	
	


	
	// ===> Attribute frequency
	
	
	

	
 	/**
 	 * Get frequency.
 	 * @return frequency as double
 	 */
 	double getFrequency() const;
	
 
 	
 	
 	/**
 	 * Set frequency with the specified double.
 	 * @param frequency The double value to which frequency is to be set.
 	 
 		
 			
 	 */
 	void setFrequency (double frequency);
  		
	
	
	


	
	// ===> Attribute frequencyBand
	
	
	

	
 	/**
 	 * Get frequencyBand.
 	 * @return frequencyBand as ReceiverBandMod::ReceiverBand
 	 */
 	ReceiverBandMod::ReceiverBand getFrequencyBand() const;
	
 
 	
 	
 	/**
 	 * Set frequencyBand with the specified ReceiverBandMod::ReceiverBand.
 	 * @param frequencyBand The ReceiverBandMod::ReceiverBand value to which frequencyBand is to be set.
 	 
 		
 			
 	 */
 	void setFrequencyBand (ReceiverBandMod::ReceiverBand frequencyBand);
  		
	
	
	


	
	// ===> Attribute sbType
	
	
	

	
 	/**
 	 * Get sbType.
 	 * @return sbType as SBTypeMod::SBType
 	 */
 	SBTypeMod::SBType getSbType() const;
	
 
 	
 	
 	/**
 	 * Set sbType with the specified SBTypeMod::SBType.
 	 * @param sbType The SBTypeMod::SBType value to which sbType is to be set.
 	 
 		
 			
 	 */
 	void setSbType (SBTypeMod::SBType sbType);
  		
	
	
	


	
	// ===> Attribute sbDuration
	
	
	

	
 	/**
 	 * Get sbDuration.
 	 * @return sbDuration as Interval
 	 */
 	Interval getSbDuration() const;
	
 
 	
 	
 	/**
 	 * Set sbDuration with the specified Interval.
 	 * @param sbDuration The Interval value to which sbDuration is to be set.
 	 
 		
 			
 	 */
 	void setSbDuration (Interval sbDuration);
  		
	
	
	


	
	// ===> Attribute numObservingMode
	
	
	

	
 	/**
 	 * Get numObservingMode.
 	 * @return numObservingMode as int
 	 */
 	int getNumObservingMode() const;
	
 
 	
 	
 	/**
 	 * Set numObservingMode with the specified int.
 	 * @param numObservingMode The int value to which numObservingMode is to be set.
 	 
 		
 			
 	 */
 	void setNumObservingMode (int numObservingMode);
  		
	
	
	


	
	// ===> Attribute observingMode
	
	
	

	
 	/**
 	 * Get observingMode.
 	 * @return observingMode as std::vector<std::string >
 	 */
 	std::vector<std::string > getObservingMode() const;
	
 
 	
 	
 	/**
 	 * Set observingMode with the specified std::vector<std::string >.
 	 * @param observingMode The std::vector<std::string > value to which observingMode is to be set.
 	 
 		
 			
 	 */
 	void setObservingMode (std::vector<std::string > observingMode);
  		
	
	
	


	
	// ===> Attribute numberRepeats
	
	
	

	
 	/**
 	 * Get numberRepeats.
 	 * @return numberRepeats as int
 	 */
 	int getNumberRepeats() const;
	
 
 	
 	
 	/**
 	 * Set numberRepeats with the specified int.
 	 * @param numberRepeats The int value to which numberRepeats is to be set.
 	 
 		
 			
 	 */
 	void setNumberRepeats (int numberRepeats);
  		
	
	
	


	
	// ===> Attribute numScienceGoal
	
	
	

	
 	/**
 	 * Get numScienceGoal.
 	 * @return numScienceGoal as int
 	 */
 	int getNumScienceGoal() const;
	
 
 	
 	
 	/**
 	 * Set numScienceGoal with the specified int.
 	 * @param numScienceGoal The int value to which numScienceGoal is to be set.
 	 
 		
 			
 	 */
 	void setNumScienceGoal (int numScienceGoal);
  		
	
	
	


	
	// ===> Attribute scienceGoal
	
	
	

	
 	/**
 	 * Get scienceGoal.
 	 * @return scienceGoal as std::vector<std::string >
 	 */
 	std::vector<std::string > getScienceGoal() const;
	
 
 	
 	
 	/**
 	 * Set scienceGoal with the specified std::vector<std::string >.
 	 * @param scienceGoal The std::vector<std::string > value to which scienceGoal is to be set.
 	 
 		
 			
 	 */
 	void setScienceGoal (std::vector<std::string > scienceGoal);
  		
	
	
	


	
	// ===> Attribute numWeatherConstraint
	
	
	

	
 	/**
 	 * Get numWeatherConstraint.
 	 * @return numWeatherConstraint as int
 	 */
 	int getNumWeatherConstraint() const;
	
 
 	
 	
 	/**
 	 * Set numWeatherConstraint with the specified int.
 	 * @param numWeatherConstraint The int value to which numWeatherConstraint is to be set.
 	 
 		
 			
 	 */
 	void setNumWeatherConstraint (int numWeatherConstraint);
  		
	
	
	


	
	// ===> Attribute weatherConstraint
	
	
	

	
 	/**
 	 * Get weatherConstraint.
 	 * @return weatherConstraint as std::vector<std::string >
 	 */
 	std::vector<std::string > getWeatherConstraint() const;
	
 
 	
 	
 	/**
 	 * Set weatherConstraint with the specified std::vector<std::string >.
 	 * @param weatherConstraint The std::vector<std::string > value to which weatherConstraint is to be set.
 	 
 		
 			
 	 */
 	void setWeatherConstraint (std::vector<std::string > weatherConstraint);
  		
	
	
	


	
	// ===> Attribute centerDirection, which is optional
	
	
	
	/**
	 * The attribute centerDirection is optional. Return true if this attribute exists.
	 * @return true if and only if the centerDirection attribute exists. 
	 */
	bool isCenterDirectionExists() const;
	

	
 	/**
 	 * Get centerDirection, which is optional.
 	 * @return centerDirection as std::vector<Angle >
 	 * @throws IllegalAccessException If centerDirection does not exist.
 	 */
 	std::vector<Angle > getCenterDirection() const;
	
 
 	
 	
 	/**
 	 * Set centerDirection with the specified std::vector<Angle >.
 	 * @param centerDirection The std::vector<Angle > value to which centerDirection is to be set.
 	 
 		
 	 */
 	void setCenterDirection (std::vector<Angle > centerDirection);
		
	
	
	
	/**
	 * Mark centerDirection, which is an optional field, as non-existent.
	 */
	void clearCenterDirection ();
	


	
	// ===> Attribute centerDirectionCode, which is optional
	
	
	
	/**
	 * The attribute centerDirectionCode is optional. Return true if this attribute exists.
	 * @return true if and only if the centerDirectionCode attribute exists. 
	 */
	bool isCenterDirectionCodeExists() const;
	

	
 	/**
 	 * Get centerDirectionCode, which is optional.
 	 * @return centerDirectionCode as DirectionReferenceCodeMod::DirectionReferenceCode
 	 * @throws IllegalAccessException If centerDirectionCode does not exist.
 	 */
 	DirectionReferenceCodeMod::DirectionReferenceCode getCenterDirectionCode() const;
	
 
 	
 	
 	/**
 	 * Set centerDirectionCode with the specified DirectionReferenceCodeMod::DirectionReferenceCode.
 	 * @param centerDirectionCode The DirectionReferenceCodeMod::DirectionReferenceCode value to which centerDirectionCode is to be set.
 	 
 		
 	 */
 	void setCenterDirectionCode (DirectionReferenceCodeMod::DirectionReferenceCode centerDirectionCode);
		
	
	
	
	/**
	 * Mark centerDirectionCode, which is an optional field, as non-existent.
	 */
	void clearCenterDirectionCode ();
	


	
	// ===> Attribute centerDirectionEquinox, which is optional
	
	
	
	/**
	 * The attribute centerDirectionEquinox is optional. Return true if this attribute exists.
	 * @return true if and only if the centerDirectionEquinox attribute exists. 
	 */
	bool isCenterDirectionEquinoxExists() const;
	

	
 	/**
 	 * Get centerDirectionEquinox, which is optional.
 	 * @return centerDirectionEquinox as ArrayTime
 	 * @throws IllegalAccessException If centerDirectionEquinox does not exist.
 	 */
 	ArrayTime getCenterDirectionEquinox() const;
	
 
 	
 	
 	/**
 	 * Set centerDirectionEquinox with the specified ArrayTime.
 	 * @param centerDirectionEquinox The ArrayTime value to which centerDirectionEquinox is to be set.
 	 
 		
 	 */
 	void setCenterDirectionEquinox (ArrayTime centerDirectionEquinox);
		
	
	
	
	/**
	 * Mark centerDirectionEquinox, which is an optional field, as non-existent.
	 */
	void clearCenterDirectionEquinox ();
	


	////////////////////////////////
	// Extrinsic Table Attributes //
	////////////////////////////////
	
	///////////
	// Links //
	///////////
	
	
	
	
	/**
	 * Compare each mandatory attribute except the autoincrementable one of this SBSummaryRow with 
	 * the corresponding parameters and return true if there is a match and false otherwise.
	 	
	 * @param sbSummaryUID
	    
	 * @param projectUID
	    
	 * @param obsUnitSetUID
	    
	 * @param frequency
	    
	 * @param frequencyBand
	    
	 * @param sbType
	    
	 * @param sbDuration
	    
	 * @param numObservingMode
	    
	 * @param observingMode
	    
	 * @param numberRepeats
	    
	 * @param numScienceGoal
	    
	 * @param scienceGoal
	    
	 * @param numWeatherConstraint
	    
	 * @param weatherConstraint
	    
	 */ 
	bool compareNoAutoInc(EntityRef sbSummaryUID, EntityRef projectUID, EntityRef obsUnitSetUID, double frequency, ReceiverBandMod::ReceiverBand frequencyBand, SBTypeMod::SBType sbType, Interval sbDuration, int numObservingMode, std::vector<std::string > observingMode, int numberRepeats, int numScienceGoal, std::vector<std::string > scienceGoal, int numWeatherConstraint, std::vector<std::string > weatherConstraint);
	
	

	
	/**
	 * Compare each mandatory value (i.e. not in the key) attribute  with 
	 * the corresponding parameters and return true if there is a match and false otherwise.
	 	
	 * @param sbSummaryUID
	    
	 * @param projectUID
	    
	 * @param obsUnitSetUID
	    
	 * @param frequency
	    
	 * @param frequencyBand
	    
	 * @param sbType
	    
	 * @param sbDuration
	    
	 * @param numObservingMode
	    
	 * @param observingMode
	    
	 * @param numberRepeats
	    
	 * @param numScienceGoal
	    
	 * @param scienceGoal
	    
	 * @param numWeatherConstraint
	    
	 * @param weatherConstraint
	    
	 */ 
	bool compareRequiredValue(EntityRef sbSummaryUID, EntityRef projectUID, EntityRef obsUnitSetUID, double frequency, ReceiverBandMod::ReceiverBand frequencyBand, SBTypeMod::SBType sbType, Interval sbDuration, int numObservingMode, std::vector<std::string > observingMode, int numberRepeats, int numScienceGoal, std::vector<std::string > scienceGoal, int numWeatherConstraint, std::vector<std::string > weatherConstraint); 
		 
	
	/**
	 * Return true if all required attributes of the value part are equal to their homologues
	 * in x and false otherwise.
	 *
	 * @param x a pointer on the SBSummaryRow whose required attributes of the value part 
	 * will be compared with those of this.
	 * @return a boolean.
	 */
	bool equalByRequiredValue(SBSummaryRow* x) ;
	
#ifndef WITHOUT_ACS
	/**
	 * Return this row in the form of an IDL struct.
	 * @return The values of this row as a SBSummaryRowIDL struct.
	 */
	asdmIDL::SBSummaryRowIDL *toIDL() const;
	
	/**
	 * Define the content of a SBSummaryRowIDL struct from the values
	 * found in this row.
	 *
	 * @param x a reference to the SBSummaryRowIDL struct to be set.
	 *
	 */
	 void toIDL(asdmIDL::SBSummaryRowIDL& x) const;
#endif
	
#ifndef WITHOUT_ACS
	/**
	 * Fill the values of this row from the IDL struct SBSummaryRowIDL.
	 * @param x The IDL struct containing the values used to fill this row.
	 * @throws ConversionException
	 */
	void setFromIDL (asdmIDL::SBSummaryRowIDL x) ;
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

	std::map<std::string, SBSummaryAttributeFromBin> fromBinMethods;
void sBSummaryIdFromBin( EndianIStream& eis);
void sbSummaryUIDFromBin( EndianIStream& eis);
void projectUIDFromBin( EndianIStream& eis);
void obsUnitSetUIDFromBin( EndianIStream& eis);
void frequencyFromBin( EndianIStream& eis);
void frequencyBandFromBin( EndianIStream& eis);
void sbTypeFromBin( EndianIStream& eis);
void sbDurationFromBin( EndianIStream& eis);
void numObservingModeFromBin( EndianIStream& eis);
void observingModeFromBin( EndianIStream& eis);
void numberRepeatsFromBin( EndianIStream& eis);
void numScienceGoalFromBin( EndianIStream& eis);
void scienceGoalFromBin( EndianIStream& eis);
void numWeatherConstraintFromBin( EndianIStream& eis);
void weatherConstraintFromBin( EndianIStream& eis);

void centerDirectionFromBin( EndianIStream& eis);
void centerDirectionCodeFromBin( EndianIStream& eis);
void centerDirectionEquinoxFromBin( EndianIStream& eis);


	 /**
	  * Deserialize a stream of bytes read from an EndianIStream to build a PointingRow.
	  * @param eiss the EndianIStream to be read.
	  * @param table the SBSummaryTable to which the row built by deserialization will be parented.
	  * @param attributesSeq a vector containing the names of the attributes . The elements order defines the order 
	  * in which the attributes are written in the binary serialization.
	  */
	 static SBSummaryRow* fromBin(EndianIStream& eis, SBSummaryTable& table, const std::vector<std::string>& attributesSeq);	 
 
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
	SBSummaryTable &table;
	/**
	 * Whether this row has been added to the table or not.
	 */
	bool hasBeenAdded;

	// This method is used by the Table class when this row is added to the table.
	void isAdded(bool added);


	/**
	 * Create a SBSummaryRow.
	 * <p>
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 */ 
	SBSummaryRow (SBSummaryTable &table);

	/**
	 * Create a SBSummaryRow using a copy constructor mechanism.
	 * <p>
	 * Given a SBSummaryRow row and a SBSummaryTable table, the method creates a new
	 * SBSummaryRow owned by table. Each attribute of the created row is a copy (deep)
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
	 SBSummaryRow (SBSummaryTable &table, SBSummaryRow *row);
	 	
	////////////////////////////////
	// Intrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute sBSummaryId
	
	

	Tag sBSummaryId;

	
	
 	
 	/**
 	 * Set sBSummaryId with the specified Tag value.
 	 * @param sBSummaryId The Tag value to which sBSummaryId is to be set.
		
 		
			
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 		
 	 */
 	void setSBSummaryId (Tag sBSummaryId);
  		
	

	
	// ===> Attribute sbSummaryUID
	
	

	EntityRef sbSummaryUID;

	
	
 	

	
	// ===> Attribute projectUID
	
	

	EntityRef projectUID;

	
	
 	

	
	// ===> Attribute obsUnitSetUID
	
	

	EntityRef obsUnitSetUID;

	
	
 	

	
	// ===> Attribute frequency
	
	

	double frequency;

	
	
 	

	
	// ===> Attribute frequencyBand
	
	

	ReceiverBandMod::ReceiverBand frequencyBand;

	
	
 	

	
	// ===> Attribute sbType
	
	

	SBTypeMod::SBType sbType;

	
	
 	

	
	// ===> Attribute sbDuration
	
	

	Interval sbDuration;

	
	
 	

	
	// ===> Attribute numObservingMode
	
	

	int numObservingMode;

	
	
 	

	
	// ===> Attribute observingMode
	
	

	std::vector<std::string > observingMode;

	
	
 	

	
	// ===> Attribute numberRepeats
	
	

	int numberRepeats;

	
	
 	

	
	// ===> Attribute numScienceGoal
	
	

	int numScienceGoal;

	
	
 	

	
	// ===> Attribute scienceGoal
	
	

	std::vector<std::string > scienceGoal;

	
	
 	

	
	// ===> Attribute numWeatherConstraint
	
	

	int numWeatherConstraint;

	
	
 	

	
	// ===> Attribute weatherConstraint
	
	

	std::vector<std::string > weatherConstraint;

	
	
 	

	
	// ===> Attribute centerDirection, which is optional
	
	
	bool centerDirectionExists;
	

	std::vector<Angle > centerDirection;

	
	
 	

	
	// ===> Attribute centerDirectionCode, which is optional
	
	
	bool centerDirectionCodeExists;
	

	DirectionReferenceCodeMod::DirectionReferenceCode centerDirectionCode;

	
	
 	

	
	// ===> Attribute centerDirectionEquinox, which is optional
	
	
	bool centerDirectionEquinoxExists;
	

	ArrayTime centerDirectionEquinox;

	
	
 	

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
	std::map<std::string, SBSummaryAttributeFromBin> fromBinMethods;
void sBSummaryIdFromBin( EndianIStream& eis);
void sbSummaryUIDFromBin( EndianIStream& eis);
void projectUIDFromBin( EndianIStream& eis);
void obsUnitSetUIDFromBin( EndianIStream& eis);
void frequencyFromBin( EndianIStream& eis);
void frequencyBandFromBin( EndianIStream& eis);
void sbTypeFromBin( EndianIStream& eis);
void sbDurationFromBin( EndianIStream& eis);
void numObservingModeFromBin( EndianIStream& eis);
void observingModeFromBin( EndianIStream& eis);
void numberRepeatsFromBin( EndianIStream& eis);
void numScienceGoalFromBin( EndianIStream& eis);
void scienceGoalFromBin( EndianIStream& eis);
void numWeatherConstraintFromBin( EndianIStream& eis);
void weatherConstraintFromBin( EndianIStream& eis);

void centerDirectionFromBin( EndianIStream& eis);
void centerDirectionCodeFromBin( EndianIStream& eis);
void centerDirectionEquinoxFromBin( EndianIStream& eis);

*/
	
	///////////////////////////////////
	// text-deserialization material //
	///////////////////////////////////
	std::map<std::string, SBSummaryAttributeFromText> fromTextMethods;
	
void sBSummaryIdFromText (const std::string & s);
	
	
	
	
	
void frequencyFromText (const std::string & s);
	
	
void frequencyBandFromText (const std::string & s);
	
	
void sbTypeFromText (const std::string & s);
	
	
void sbDurationFromText (const std::string & s);
	
	
void numObservingModeFromText (const std::string & s);
	
	
void observingModeFromText (const std::string & s);
	
	
void numberRepeatsFromText (const std::string & s);
	
	
void numScienceGoalFromText (const std::string & s);
	
	
void scienceGoalFromText (const std::string & s);
	
	
void numWeatherConstraintFromText (const std::string & s);
	
	
void weatherConstraintFromText (const std::string & s);
	

	
void centerDirectionFromText (const std::string & s);
	
	
void centerDirectionCodeFromText (const std::string & s);
	
	
void centerDirectionEquinoxFromText (const std::string & s);
	
	
	
	/**
	 * Serialize this into a stream of bytes written to an EndianOSStream.
	 * @param eoss the EndianOSStream to be written to
	 */
	 void toBin(EndianOSStream& eoss);
	 	 
	 /**
	  * Deserialize a stream of bytes read from an EndianIStream to build a PointingRow.
	  * @param eiss the EndianIStream to be read.
	  * @param table the SBSummaryTable to which the row built by deserialization will be parented.
	  * @param attributesSeq a vector containing the names of the attributes . The elements order defines the order 
	  * in which the attributes are written in the binary serialization.

	 static SBSummaryRow* fromBin(EndianIStream& eis, SBSummaryTable& table, const std::vector<std::string>& attributesSeq);	 
		*/
};

} // End namespace asdm

#endif /* SBSummary_CLASS */
