
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
 * File CalDelayRow.h
 */
 
#ifndef CalDelayRow_CLASS
#define CalDelayRow_CLASS

#include <vector>
#include <string>
#include <set>

#ifndef WITHOUT_ACS
#include <asdmIDLC.h>
#endif






	 
#include <alma/ASDM/Angle.h>
	

	 
#include <alma/ASDM/ArrayTime.h>
	

	 
#include <alma/ASDM/Frequency.h>
	

	 
#include <alma/ASDM/Tag.h>
	




	

	
#include <alma/Enumerations/CAtmPhaseCorrection.h>
	

	
#include <alma/Enumerations/CBasebandName.h>
	

	
#include <alma/Enumerations/CReceiverBand.h>
	

	

	

	

	

	

	

	
#include <alma/Enumerations/CPolarizationType.h>
	

	

	

	

	

	

	

	

	
#include <alma/Enumerations/CReceiverSideband.h>
	



#include <alma/ASDM/ConversionException.h>
#include <alma/ASDM/NoSuchRow.h>
#include <alma/ASDM/IllegalAccessException.h>

#include <alma/ASDM/RowTransformer.h>
//#include <alma/ASDM/TableStreamReader.h>

/*\file CalDelay.h
    \brief Generated from model's revision "-1", branch ""
*/

namespace asdm {

//class asdm::CalDelayTable;


// class asdm::CalDataRow;
class CalDataRow;

// class asdm::CalReductionRow;
class CalReductionRow;
	

class CalDelayRow;
typedef void (CalDelayRow::*CalDelayAttributeFromBin) (EndianIStream& eis);
typedef void (CalDelayRow::*CalDelayAttributeFromText) (const std::string& s);

/**
 * The CalDelayRow class is a row of a CalDelayTable.
 * 
 * Generated from model's revision "-1", branch ""
 *
 */
class CalDelayRow {
friend class asdm::CalDelayTable;
friend class asdm::RowTransformer<CalDelayRow>;
//friend class asdm::TableStreamReader<CalDelayTable, CalDelayRow>;

public:

	virtual ~CalDelayRow();

	/**
	 * Return the table to which this row belongs.
	 */
	CalDelayTable &getTable() const;
	
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
  		
	
	
	


	
	// ===> Attribute refAntennaName
	
	
	

	
 	/**
 	 * Get refAntennaName.
 	 * @return refAntennaName as std::string
 	 */
 	std::string getRefAntennaName() const;
	
 
 	
 	
 	/**
 	 * Set refAntennaName with the specified std::string.
 	 * @param refAntennaName The std::string value to which refAntennaName is to be set.
 	 
 		
 			
 	 */
 	void setRefAntennaName (std::string refAntennaName);
  		
	
	
	


	
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
  		
	
	
	


	
	// ===> Attribute delayError
	
	
	

	
 	/**
 	 * Get delayError.
 	 * @return delayError as std::vector<double >
 	 */
 	std::vector<double > getDelayError() const;
	
 
 	
 	
 	/**
 	 * Set delayError with the specified std::vector<double >.
 	 * @param delayError The std::vector<double > value to which delayError is to be set.
 	 
 		
 			
 	 */
 	void setDelayError (std::vector<double > delayError);
  		
	
	
	


	
	// ===> Attribute delayOffset
	
	
	

	
 	/**
 	 * Get delayOffset.
 	 * @return delayOffset as std::vector<double >
 	 */
 	std::vector<double > getDelayOffset() const;
	
 
 	
 	
 	/**
 	 * Set delayOffset with the specified std::vector<double >.
 	 * @param delayOffset The std::vector<double > value to which delayOffset is to be set.
 	 
 		
 			
 	 */
 	void setDelayOffset (std::vector<double > delayOffset);
  		
	
	
	


	
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
  		
	
	
	


	
	// ===> Attribute reducedChiSquared
	
	
	

	
 	/**
 	 * Get reducedChiSquared.
 	 * @return reducedChiSquared as std::vector<double >
 	 */
 	std::vector<double > getReducedChiSquared() const;
	
 
 	
 	
 	/**
 	 * Set reducedChiSquared with the specified std::vector<double >.
 	 * @param reducedChiSquared The std::vector<double > value to which reducedChiSquared is to be set.
 	 
 		
 			
 	 */
 	void setReducedChiSquared (std::vector<double > reducedChiSquared);
  		
	
	
	


	
	// ===> Attribute appliedDelay
	
	
	

	
 	/**
 	 * Get appliedDelay.
 	 * @return appliedDelay as std::vector<double >
 	 */
 	std::vector<double > getAppliedDelay() const;
	
 
 	
 	
 	/**
 	 * Set appliedDelay with the specified std::vector<double >.
 	 * @param appliedDelay The std::vector<double > value to which appliedDelay is to be set.
 	 
 		
 			
 	 */
 	void setAppliedDelay (std::vector<double > appliedDelay);
  		
	
	
	


	
	// ===> Attribute crossDelayOffset, which is optional
	
	
	
	/**
	 * The attribute crossDelayOffset is optional. Return true if this attribute exists.
	 * @return true if and only if the crossDelayOffset attribute exists. 
	 */
	bool isCrossDelayOffsetExists() const;
	

	
 	/**
 	 * Get crossDelayOffset, which is optional.
 	 * @return crossDelayOffset as double
 	 * @throws IllegalAccessException If crossDelayOffset does not exist.
 	 */
 	double getCrossDelayOffset() const;
	
 
 	
 	
 	/**
 	 * Set crossDelayOffset with the specified double.
 	 * @param crossDelayOffset The double value to which crossDelayOffset is to be set.
 	 
 		
 	 */
 	void setCrossDelayOffset (double crossDelayOffset);
		
	
	
	
	/**
	 * Mark crossDelayOffset, which is an optional field, as non-existent.
	 */
	void clearCrossDelayOffset ();
	


	
	// ===> Attribute crossDelayOffsetError, which is optional
	
	
	
	/**
	 * The attribute crossDelayOffsetError is optional. Return true if this attribute exists.
	 * @return true if and only if the crossDelayOffsetError attribute exists. 
	 */
	bool isCrossDelayOffsetErrorExists() const;
	

	
 	/**
 	 * Get crossDelayOffsetError, which is optional.
 	 * @return crossDelayOffsetError as double
 	 * @throws IllegalAccessException If crossDelayOffsetError does not exist.
 	 */
 	double getCrossDelayOffsetError() const;
	
 
 	
 	
 	/**
 	 * Set crossDelayOffsetError with the specified double.
 	 * @param crossDelayOffsetError The double value to which crossDelayOffsetError is to be set.
 	 
 		
 	 */
 	void setCrossDelayOffsetError (double crossDelayOffsetError);
		
	
	
	
	/**
	 * Mark crossDelayOffsetError, which is an optional field, as non-existent.
	 */
	void clearCrossDelayOffsetError ();
	


	
	// ===> Attribute numSideband, which is optional
	
	
	
	/**
	 * The attribute numSideband is optional. Return true if this attribute exists.
	 * @return true if and only if the numSideband attribute exists. 
	 */
	bool isNumSidebandExists() const;
	

	
 	/**
 	 * Get numSideband, which is optional.
 	 * @return numSideband as int
 	 * @throws IllegalAccessException If numSideband does not exist.
 	 */
 	int getNumSideband() const;
	
 
 	
 	
 	/**
 	 * Set numSideband with the specified int.
 	 * @param numSideband The int value to which numSideband is to be set.
 	 
 		
 	 */
 	void setNumSideband (int numSideband);
		
	
	
	
	/**
	 * Mark numSideband, which is an optional field, as non-existent.
	 */
	void clearNumSideband ();
	


	
	// ===> Attribute refFreq, which is optional
	
	
	
	/**
	 * The attribute refFreq is optional. Return true if this attribute exists.
	 * @return true if and only if the refFreq attribute exists. 
	 */
	bool isRefFreqExists() const;
	

	
 	/**
 	 * Get refFreq, which is optional.
 	 * @return refFreq as std::vector<Frequency >
 	 * @throws IllegalAccessException If refFreq does not exist.
 	 */
 	std::vector<Frequency > getRefFreq() const;
	
 
 	
 	
 	/**
 	 * Set refFreq with the specified std::vector<Frequency >.
 	 * @param refFreq The std::vector<Frequency > value to which refFreq is to be set.
 	 
 		
 	 */
 	void setRefFreq (std::vector<Frequency > refFreq);
		
	
	
	
	/**
	 * Mark refFreq, which is an optional field, as non-existent.
	 */
	void clearRefFreq ();
	


	
	// ===> Attribute refFreqPhase, which is optional
	
	
	
	/**
	 * The attribute refFreqPhase is optional. Return true if this attribute exists.
	 * @return true if and only if the refFreqPhase attribute exists. 
	 */
	bool isRefFreqPhaseExists() const;
	

	
 	/**
 	 * Get refFreqPhase, which is optional.
 	 * @return refFreqPhase as std::vector<Angle >
 	 * @throws IllegalAccessException If refFreqPhase does not exist.
 	 */
 	std::vector<Angle > getRefFreqPhase() const;
	
 
 	
 	
 	/**
 	 * Set refFreqPhase with the specified std::vector<Angle >.
 	 * @param refFreqPhase The std::vector<Angle > value to which refFreqPhase is to be set.
 	 
 		
 	 */
 	void setRefFreqPhase (std::vector<Angle > refFreqPhase);
		
	
	
	
	/**
	 * Mark refFreqPhase, which is an optional field, as non-existent.
	 */
	void clearRefFreqPhase ();
	


	
	// ===> Attribute sidebands, which is optional
	
	
	
	/**
	 * The attribute sidebands is optional. Return true if this attribute exists.
	 * @return true if and only if the sidebands attribute exists. 
	 */
	bool isSidebandsExists() const;
	

	
 	/**
 	 * Get sidebands, which is optional.
 	 * @return sidebands as std::vector<ReceiverSidebandMod::ReceiverSideband >
 	 * @throws IllegalAccessException If sidebands does not exist.
 	 */
 	std::vector<ReceiverSidebandMod::ReceiverSideband > getSidebands() const;
	
 
 	
 	
 	/**
 	 * Set sidebands with the specified std::vector<ReceiverSidebandMod::ReceiverSideband >.
 	 * @param sidebands The std::vector<ReceiverSidebandMod::ReceiverSideband > value to which sidebands is to be set.
 	 
 		
 	 */
 	void setSidebands (std::vector<ReceiverSidebandMod::ReceiverSideband > sidebands);
		
	
	
	
	/**
	 * Mark sidebands, which is an optional field, as non-existent.
	 */
	void clearSidebands ();
	


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
	 * Compare each mandatory attribute except the autoincrementable one of this CalDelayRow with 
	 * the corresponding parameters and return true if there is a match and false otherwise.
	 	
	 * @param antennaName
	    
	 * @param atmPhaseCorrection
	    
	 * @param basebandName
	    
	 * @param receiverBand
	    
	 * @param calDataId
	    
	 * @param calReductionId
	    
	 * @param startValidTime
	    
	 * @param endValidTime
	    
	 * @param refAntennaName
	    
	 * @param numReceptor
	    
	 * @param delayError
	    
	 * @param delayOffset
	    
	 * @param polarizationTypes
	    
	 * @param reducedChiSquared
	    
	 * @param appliedDelay
	    
	 */ 
	bool compareNoAutoInc(std::string antennaName, AtmPhaseCorrectionMod::AtmPhaseCorrection atmPhaseCorrection, BasebandNameMod::BasebandName basebandName, ReceiverBandMod::ReceiverBand receiverBand, Tag calDataId, Tag calReductionId, ArrayTime startValidTime, ArrayTime endValidTime, std::string refAntennaName, int numReceptor, std::vector<double > delayError, std::vector<double > delayOffset, std::vector<PolarizationTypeMod::PolarizationType > polarizationTypes, std::vector<double > reducedChiSquared, std::vector<double > appliedDelay);
	
	

	
	/**
	 * Compare each mandatory value (i.e. not in the key) attribute  with 
	 * the corresponding parameters and return true if there is a match and false otherwise.
	 	
	 * @param startValidTime
	    
	 * @param endValidTime
	    
	 * @param refAntennaName
	    
	 * @param numReceptor
	    
	 * @param delayError
	    
	 * @param delayOffset
	    
	 * @param polarizationTypes
	    
	 * @param reducedChiSquared
	    
	 * @param appliedDelay
	    
	 */ 
	bool compareRequiredValue(ArrayTime startValidTime, ArrayTime endValidTime, std::string refAntennaName, int numReceptor, std::vector<double > delayError, std::vector<double > delayOffset, std::vector<PolarizationTypeMod::PolarizationType > polarizationTypes, std::vector<double > reducedChiSquared, std::vector<double > appliedDelay); 
		 
	
	/**
	 * Return true if all required attributes of the value part are equal to their homologues
	 * in x and false otherwise.
	 *
	 * @param x a pointer on the CalDelayRow whose required attributes of the value part 
	 * will be compared with those of this.
	 * @return a boolean.
	 */
	bool equalByRequiredValue(CalDelayRow* x) ;
	
#ifndef WITHOUT_ACS
	/**
	 * Return this row in the form of an IDL struct.
	 * @return The values of this row as a CalDelayRowIDL struct.
	 */
	asdmIDL::CalDelayRowIDL *toIDL() const;
	
	/**
	 * Define the content of a CalDelayRowIDL struct from the values
	 * found in this row.
	 *
	 * @param x a reference to the CalDelayRowIDL struct to be set.
	 *
	 */
	 void toIDL(asdmIDL::CalDelayRowIDL& x) const;
#endif
	
#ifndef WITHOUT_ACS
	/**
	 * Fill the values of this row from the IDL struct CalDelayRowIDL.
	 * @param x The IDL struct containing the values used to fill this row.
	 * @throws ConversionException
	 */
	void setFromIDL (asdmIDL::CalDelayRowIDL x) ;
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

	std::map<std::string, CalDelayAttributeFromBin> fromBinMethods;
void antennaNameFromBin( EndianIStream& eis);
void atmPhaseCorrectionFromBin( EndianIStream& eis);
void basebandNameFromBin( EndianIStream& eis);
void receiverBandFromBin( EndianIStream& eis);
void calDataIdFromBin( EndianIStream& eis);
void calReductionIdFromBin( EndianIStream& eis);
void startValidTimeFromBin( EndianIStream& eis);
void endValidTimeFromBin( EndianIStream& eis);
void refAntennaNameFromBin( EndianIStream& eis);
void numReceptorFromBin( EndianIStream& eis);
void delayErrorFromBin( EndianIStream& eis);
void delayOffsetFromBin( EndianIStream& eis);
void polarizationTypesFromBin( EndianIStream& eis);
void reducedChiSquaredFromBin( EndianIStream& eis);
void appliedDelayFromBin( EndianIStream& eis);

void crossDelayOffsetFromBin( EndianIStream& eis);
void crossDelayOffsetErrorFromBin( EndianIStream& eis);
void numSidebandFromBin( EndianIStream& eis);
void refFreqFromBin( EndianIStream& eis);
void refFreqPhaseFromBin( EndianIStream& eis);
void sidebandsFromBin( EndianIStream& eis);


	 /**
	  * Deserialize a stream of bytes read from an EndianIStream to build a PointingRow.
	  * @param eiss the EndianIStream to be read.
	  * @param table the CalDelayTable to which the row built by deserialization will be parented.
	  * @param attributesSeq a vector containing the names of the attributes . The elements order defines the order 
	  * in which the attributes are written in the binary serialization.
	  */
	 static CalDelayRow* fromBin(EndianIStream& eis, CalDelayTable& table, const std::vector<std::string>& attributesSeq);	 
 
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
	CalDelayTable &table;
	/**
	 * Whether this row has been added to the table or not.
	 */
	bool hasBeenAdded;

	// This method is used by the Table class when this row is added to the table.
	void isAdded(bool added);


	/**
	 * Create a CalDelayRow.
	 * <p>
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 */ 
	CalDelayRow (CalDelayTable &table);

	/**
	 * Create a CalDelayRow using a copy constructor mechanism.
	 * <p>
	 * Given a CalDelayRow row and a CalDelayTable table, the method creates a new
	 * CalDelayRow owned by table. Each attribute of the created row is a copy (deep)
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
	 CalDelayRow (CalDelayTable &table, CalDelayRow *row);
	 	
	////////////////////////////////
	// Intrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute antennaName
	
	

	std::string antennaName;

	
	
 	

	
	// ===> Attribute atmPhaseCorrection
	
	

	AtmPhaseCorrectionMod::AtmPhaseCorrection atmPhaseCorrection;

	
	
 	

	
	// ===> Attribute basebandName
	
	

	BasebandNameMod::BasebandName basebandName;

	
	
 	

	
	// ===> Attribute receiverBand
	
	

	ReceiverBandMod::ReceiverBand receiverBand;

	
	
 	

	
	// ===> Attribute startValidTime
	
	

	ArrayTime startValidTime;

	
	
 	

	
	// ===> Attribute endValidTime
	
	

	ArrayTime endValidTime;

	
	
 	

	
	// ===> Attribute refAntennaName
	
	

	std::string refAntennaName;

	
	
 	

	
	// ===> Attribute numReceptor
	
	

	int numReceptor;

	
	
 	

	
	// ===> Attribute delayError
	
	

	std::vector<double > delayError;

	
	
 	

	
	// ===> Attribute delayOffset
	
	

	std::vector<double > delayOffset;

	
	
 	

	
	// ===> Attribute polarizationTypes
	
	

	std::vector<PolarizationTypeMod::PolarizationType > polarizationTypes;

	
	
 	

	
	// ===> Attribute reducedChiSquared
	
	

	std::vector<double > reducedChiSquared;

	
	
 	

	
	// ===> Attribute appliedDelay
	
	

	std::vector<double > appliedDelay;

	
	
 	

	
	// ===> Attribute crossDelayOffset, which is optional
	
	
	bool crossDelayOffsetExists;
	

	double crossDelayOffset;

	
	
 	

	
	// ===> Attribute crossDelayOffsetError, which is optional
	
	
	bool crossDelayOffsetErrorExists;
	

	double crossDelayOffsetError;

	
	
 	

	
	// ===> Attribute numSideband, which is optional
	
	
	bool numSidebandExists;
	

	int numSideband;

	
	
 	

	
	// ===> Attribute refFreq, which is optional
	
	
	bool refFreqExists;
	

	std::vector<Frequency > refFreq;

	
	
 	

	
	// ===> Attribute refFreqPhase, which is optional
	
	
	bool refFreqPhaseExists;
	

	std::vector<Angle > refFreqPhase;

	
	
 	

	
	// ===> Attribute sidebands, which is optional
	
	
	bool sidebandsExists;
	

	std::vector<ReceiverSidebandMod::ReceiverSideband > sidebands;

	
	
 	

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
	std::map<std::string, CalDelayAttributeFromBin> fromBinMethods;
void antennaNameFromBin( EndianIStream& eis);
void atmPhaseCorrectionFromBin( EndianIStream& eis);
void basebandNameFromBin( EndianIStream& eis);
void receiverBandFromBin( EndianIStream& eis);
void calDataIdFromBin( EndianIStream& eis);
void calReductionIdFromBin( EndianIStream& eis);
void startValidTimeFromBin( EndianIStream& eis);
void endValidTimeFromBin( EndianIStream& eis);
void refAntennaNameFromBin( EndianIStream& eis);
void numReceptorFromBin( EndianIStream& eis);
void delayErrorFromBin( EndianIStream& eis);
void delayOffsetFromBin( EndianIStream& eis);
void polarizationTypesFromBin( EndianIStream& eis);
void reducedChiSquaredFromBin( EndianIStream& eis);
void appliedDelayFromBin( EndianIStream& eis);

void crossDelayOffsetFromBin( EndianIStream& eis);
void crossDelayOffsetErrorFromBin( EndianIStream& eis);
void numSidebandFromBin( EndianIStream& eis);
void refFreqFromBin( EndianIStream& eis);
void refFreqPhaseFromBin( EndianIStream& eis);
void sidebandsFromBin( EndianIStream& eis);

*/
	
	///////////////////////////////////
	// text-deserialization material //
	///////////////////////////////////
	std::map<std::string, CalDelayAttributeFromText> fromTextMethods;
	
void antennaNameFromText (const std::string & s);
	
	
void atmPhaseCorrectionFromText (const std::string & s);
	
	
void basebandNameFromText (const std::string & s);
	
	
void receiverBandFromText (const std::string & s);
	
	
void calDataIdFromText (const std::string & s);
	
	
void calReductionIdFromText (const std::string & s);
	
	
void startValidTimeFromText (const std::string & s);
	
	
void endValidTimeFromText (const std::string & s);
	
	
void refAntennaNameFromText (const std::string & s);
	
	
void numReceptorFromText (const std::string & s);
	
	
void delayErrorFromText (const std::string & s);
	
	
void delayOffsetFromText (const std::string & s);
	
	
void polarizationTypesFromText (const std::string & s);
	
	
void reducedChiSquaredFromText (const std::string & s);
	
	
void appliedDelayFromText (const std::string & s);
	

	
void crossDelayOffsetFromText (const std::string & s);
	
	
void crossDelayOffsetErrorFromText (const std::string & s);
	
	
void numSidebandFromText (const std::string & s);
	
	
void refFreqFromText (const std::string & s);
	
	
void refFreqPhaseFromText (const std::string & s);
	
	
void sidebandsFromText (const std::string & s);
	
	
	
	/**
	 * Serialize this into a stream of bytes written to an EndianOSStream.
	 * @param eoss the EndianOSStream to be written to
	 */
	 void toBin(EndianOSStream& eoss);
	 	 
	 /**
	  * Deserialize a stream of bytes read from an EndianIStream to build a PointingRow.
	  * @param eiss the EndianIStream to be read.
	  * @param table the CalDelayTable to which the row built by deserialization will be parented.
	  * @param attributesSeq a vector containing the names of the attributes . The elements order defines the order 
	  * in which the attributes are written in the binary serialization.

	 static CalDelayRow* fromBin(EndianIStream& eis, CalDelayTable& table, const std::vector<std::string>& attributesSeq);	 
		*/
};

} // End namespace asdm

#endif /* CalDelay_CLASS */
