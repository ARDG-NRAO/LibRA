
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
 * File CalPositionRow.h
 */
 
#ifndef CalPositionRow_CLASS
#define CalPositionRow_CLASS

#include <vector>
#include <string>
#include <set>

#ifndef WITHOUT_ACS
#include <asdmIDLC.h>
#endif






	 
#include <alma/ASDM/Angle.h>
	

	 
#include <alma/ASDM/ArrayTime.h>
	

	 
#include <alma/ASDM/Length.h>
	

	 
#include <alma/ASDM/Tag.h>
	




	

	
#include <alma/Enumerations/CAtmPhaseCorrection.h>
	

	

	

	

	

	

	
#include <alma/Enumerations/CPositionMethod.h>
	

	
#include <alma/Enumerations/CReceiverBand.h>
	

	

	

	

	

	

	

	

	

	

	



#include <alma/ASDM/ConversionException.h>
#include <alma/ASDM/NoSuchRow.h>
#include <alma/ASDM/IllegalAccessException.h>

#include <alma/ASDM/RowTransformer.h>
//#include <alma/ASDM/TableStreamReader.h>

/*\file CalPosition.h
    \brief Generated from model's revision "-1", branch ""
*/

namespace asdm {

//class asdm::CalPositionTable;


// class asdm::CalDataRow;
class CalDataRow;

// class asdm::CalReductionRow;
class CalReductionRow;
	

class CalPositionRow;
typedef void (CalPositionRow::*CalPositionAttributeFromBin) (EndianIStream& eis);
typedef void (CalPositionRow::*CalPositionAttributeFromText) (const std::string& s);

/**
 * The CalPositionRow class is a row of a CalPositionTable.
 * 
 * Generated from model's revision "-1", branch ""
 *
 */
class CalPositionRow {
friend class asdm::CalPositionTable;
friend class asdm::RowTransformer<CalPositionRow>;
//friend class asdm::TableStreamReader<CalPositionTable, CalPositionRow>;

public:

	virtual ~CalPositionRow();

	/**
	 * Return the table to which this row belongs.
	 */
	CalPositionTable &getTable() const;
	
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
  		
	
	
	


	
	// ===> Attribute antennaPosition
	
	
	

	
 	/**
 	 * Get antennaPosition.
 	 * @return antennaPosition as std::vector<Length >
 	 */
 	std::vector<Length > getAntennaPosition() const;
	
 
 	
 	
 	/**
 	 * Set antennaPosition with the specified std::vector<Length >.
 	 * @param antennaPosition The std::vector<Length > value to which antennaPosition is to be set.
 	 
 		
 			
 	 */
 	void setAntennaPosition (std::vector<Length > antennaPosition);
  		
	
	
	


	
	// ===> Attribute stationName
	
	
	

	
 	/**
 	 * Get stationName.
 	 * @return stationName as std::string
 	 */
 	std::string getStationName() const;
	
 
 	
 	
 	/**
 	 * Set stationName with the specified std::string.
 	 * @param stationName The std::string value to which stationName is to be set.
 	 
 		
 			
 	 */
 	void setStationName (std::string stationName);
  		
	
	
	


	
	// ===> Attribute stationPosition
	
	
	

	
 	/**
 	 * Get stationPosition.
 	 * @return stationPosition as std::vector<Length >
 	 */
 	std::vector<Length > getStationPosition() const;
	
 
 	
 	
 	/**
 	 * Set stationPosition with the specified std::vector<Length >.
 	 * @param stationPosition The std::vector<Length > value to which stationPosition is to be set.
 	 
 		
 			
 	 */
 	void setStationPosition (std::vector<Length > stationPosition);
  		
	
	
	


	
	// ===> Attribute positionMethod
	
	
	

	
 	/**
 	 * Get positionMethod.
 	 * @return positionMethod as PositionMethodMod::PositionMethod
 	 */
 	PositionMethodMod::PositionMethod getPositionMethod() const;
	
 
 	
 	
 	/**
 	 * Set positionMethod with the specified PositionMethodMod::PositionMethod.
 	 * @param positionMethod The PositionMethodMod::PositionMethod value to which positionMethod is to be set.
 	 
 		
 			
 	 */
 	void setPositionMethod (PositionMethodMod::PositionMethod positionMethod);
  		
	
	
	


	
	// ===> Attribute receiverBand
	
	
	

	
 	/**
 	 * Get receiverBand.
 	 * @return receiverBand as ReceiverBandMod::ReceiverBand
 	 */
 	ReceiverBandMod::ReceiverBand getReceiverBand() const;
	
 
 	
 	
 	/**
 	 * Set receiverBand with the specified ReceiverBandMod::ReceiverBand.
 	 * @param receiverBand The ReceiverBandMod::ReceiverBand value to which receiverBand is to be set.
 	 
 		
 			
 	 */
 	void setReceiverBand (ReceiverBandMod::ReceiverBand receiverBand);
  		
	
	
	


	
	// ===> Attribute numAntenna
	
	
	

	
 	/**
 	 * Get numAntenna.
 	 * @return numAntenna as int
 	 */
 	int getNumAntenna() const;
	
 
 	
 	
 	/**
 	 * Set numAntenna with the specified int.
 	 * @param numAntenna The int value to which numAntenna is to be set.
 	 
 		
 			
 	 */
 	void setNumAntenna (int numAntenna);
  		
	
	
	


	
	// ===> Attribute refAntennaNames
	
	
	

	
 	/**
 	 * Get refAntennaNames.
 	 * @return refAntennaNames as std::vector<std::string >
 	 */
 	std::vector<std::string > getRefAntennaNames() const;
	
 
 	
 	
 	/**
 	 * Set refAntennaNames with the specified std::vector<std::string >.
 	 * @param refAntennaNames The std::vector<std::string > value to which refAntennaNames is to be set.
 	 
 		
 			
 	 */
 	void setRefAntennaNames (std::vector<std::string > refAntennaNames);
  		
	
	
	


	
	// ===> Attribute axesOffset
	
	
	

	
 	/**
 	 * Get axesOffset.
 	 * @return axesOffset as Length
 	 */
 	Length getAxesOffset() const;
	
 
 	
 	
 	/**
 	 * Set axesOffset with the specified Length.
 	 * @param axesOffset The Length value to which axesOffset is to be set.
 	 
 		
 			
 	 */
 	void setAxesOffset (Length axesOffset);
  		
	
	
	


	
	// ===> Attribute axesOffsetErr
	
	
	

	
 	/**
 	 * Get axesOffsetErr.
 	 * @return axesOffsetErr as Length
 	 */
 	Length getAxesOffsetErr() const;
	
 
 	
 	
 	/**
 	 * Set axesOffsetErr with the specified Length.
 	 * @param axesOffsetErr The Length value to which axesOffsetErr is to be set.
 	 
 		
 			
 	 */
 	void setAxesOffsetErr (Length axesOffsetErr);
  		
	
	
	


	
	// ===> Attribute axesOffsetFixed
	
	
	

	
 	/**
 	 * Get axesOffsetFixed.
 	 * @return axesOffsetFixed as bool
 	 */
 	bool getAxesOffsetFixed() const;
	
 
 	
 	
 	/**
 	 * Set axesOffsetFixed with the specified bool.
 	 * @param axesOffsetFixed The bool value to which axesOffsetFixed is to be set.
 	 
 		
 			
 	 */
 	void setAxesOffsetFixed (bool axesOffsetFixed);
  		
	
	
	


	
	// ===> Attribute positionOffset
	
	
	

	
 	/**
 	 * Get positionOffset.
 	 * @return positionOffset as std::vector<Length >
 	 */
 	std::vector<Length > getPositionOffset() const;
	
 
 	
 	
 	/**
 	 * Set positionOffset with the specified std::vector<Length >.
 	 * @param positionOffset The std::vector<Length > value to which positionOffset is to be set.
 	 
 		
 			
 	 */
 	void setPositionOffset (std::vector<Length > positionOffset);
  		
	
	
	


	
	// ===> Attribute positionErr
	
	
	

	
 	/**
 	 * Get positionErr.
 	 * @return positionErr as std::vector<Length >
 	 */
 	std::vector<Length > getPositionErr() const;
	
 
 	
 	
 	/**
 	 * Set positionErr with the specified std::vector<Length >.
 	 * @param positionErr The std::vector<Length > value to which positionErr is to be set.
 	 
 		
 			
 	 */
 	void setPositionErr (std::vector<Length > positionErr);
  		
	
	
	


	
	// ===> Attribute reducedChiSquared
	
	
	

	
 	/**
 	 * Get reducedChiSquared.
 	 * @return reducedChiSquared as double
 	 */
 	double getReducedChiSquared() const;
	
 
 	
 	
 	/**
 	 * Set reducedChiSquared with the specified double.
 	 * @param reducedChiSquared The double value to which reducedChiSquared is to be set.
 	 
 		
 			
 	 */
 	void setReducedChiSquared (double reducedChiSquared);
  		
	
	
	


	
	// ===> Attribute delayRms, which is optional
	
	
	
	/**
	 * The attribute delayRms is optional. Return true if this attribute exists.
	 * @return true if and only if the delayRms attribute exists. 
	 */
	bool isDelayRmsExists() const;
	

	
 	/**
 	 * Get delayRms, which is optional.
 	 * @return delayRms as double
 	 * @throws IllegalAccessException If delayRms does not exist.
 	 */
 	double getDelayRms() const;
	
 
 	
 	
 	/**
 	 * Set delayRms with the specified double.
 	 * @param delayRms The double value to which delayRms is to be set.
 	 
 		
 	 */
 	void setDelayRms (double delayRms);
		
	
	
	
	/**
	 * Mark delayRms, which is an optional field, as non-existent.
	 */
	void clearDelayRms ();
	


	
	// ===> Attribute phaseRms, which is optional
	
	
	
	/**
	 * The attribute phaseRms is optional. Return true if this attribute exists.
	 * @return true if and only if the phaseRms attribute exists. 
	 */
	bool isPhaseRmsExists() const;
	

	
 	/**
 	 * Get phaseRms, which is optional.
 	 * @return phaseRms as Angle
 	 * @throws IllegalAccessException If phaseRms does not exist.
 	 */
 	Angle getPhaseRms() const;
	
 
 	
 	
 	/**
 	 * Set phaseRms with the specified Angle.
 	 * @param phaseRms The Angle value to which phaseRms is to be set.
 	 
 		
 	 */
 	void setPhaseRms (Angle phaseRms);
		
	
	
	
	/**
	 * Mark phaseRms, which is an optional field, as non-existent.
	 */
	void clearPhaseRms ();
	


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
	 * Compare each mandatory attribute except the autoincrementable one of this CalPositionRow with 
	 * the corresponding parameters and return true if there is a match and false otherwise.
	 	
	 * @param antennaName
	    
	 * @param atmPhaseCorrection
	    
	 * @param calDataId
	    
	 * @param calReductionId
	    
	 * @param startValidTime
	    
	 * @param endValidTime
	    
	 * @param antennaPosition
	    
	 * @param stationName
	    
	 * @param stationPosition
	    
	 * @param positionMethod
	    
	 * @param receiverBand
	    
	 * @param numAntenna
	    
	 * @param refAntennaNames
	    
	 * @param axesOffset
	    
	 * @param axesOffsetErr
	    
	 * @param axesOffsetFixed
	    
	 * @param positionOffset
	    
	 * @param positionErr
	    
	 * @param reducedChiSquared
	    
	 */ 
	bool compareNoAutoInc(std::string antennaName, AtmPhaseCorrectionMod::AtmPhaseCorrection atmPhaseCorrection, Tag calDataId, Tag calReductionId, ArrayTime startValidTime, ArrayTime endValidTime, std::vector<Length > antennaPosition, std::string stationName, std::vector<Length > stationPosition, PositionMethodMod::PositionMethod positionMethod, ReceiverBandMod::ReceiverBand receiverBand, int numAntenna, std::vector<std::string > refAntennaNames, Length axesOffset, Length axesOffsetErr, bool axesOffsetFixed, std::vector<Length > positionOffset, std::vector<Length > positionErr, double reducedChiSquared);
	
	

	
	/**
	 * Compare each mandatory value (i.e. not in the key) attribute  with 
	 * the corresponding parameters and return true if there is a match and false otherwise.
	 	
	 * @param startValidTime
	    
	 * @param endValidTime
	    
	 * @param antennaPosition
	    
	 * @param stationName
	    
	 * @param stationPosition
	    
	 * @param positionMethod
	    
	 * @param receiverBand
	    
	 * @param numAntenna
	    
	 * @param refAntennaNames
	    
	 * @param axesOffset
	    
	 * @param axesOffsetErr
	    
	 * @param axesOffsetFixed
	    
	 * @param positionOffset
	    
	 * @param positionErr
	    
	 * @param reducedChiSquared
	    
	 */ 
	bool compareRequiredValue(ArrayTime startValidTime, ArrayTime endValidTime, std::vector<Length > antennaPosition, std::string stationName, std::vector<Length > stationPosition, PositionMethodMod::PositionMethod positionMethod, ReceiverBandMod::ReceiverBand receiverBand, int numAntenna, std::vector<std::string > refAntennaNames, Length axesOffset, Length axesOffsetErr, bool axesOffsetFixed, std::vector<Length > positionOffset, std::vector<Length > positionErr, double reducedChiSquared); 
		 
	
	/**
	 * Return true if all required attributes of the value part are equal to their homologues
	 * in x and false otherwise.
	 *
	 * @param x a pointer on the CalPositionRow whose required attributes of the value part 
	 * will be compared with those of this.
	 * @return a boolean.
	 */
	bool equalByRequiredValue(CalPositionRow* x) ;
	
#ifndef WITHOUT_ACS
	/**
	 * Return this row in the form of an IDL struct.
	 * @return The values of this row as a CalPositionRowIDL struct.
	 */
	asdmIDL::CalPositionRowIDL *toIDL() const;
	
	/**
	 * Define the content of a CalPositionRowIDL struct from the values
	 * found in this row.
	 *
	 * @param x a reference to the CalPositionRowIDL struct to be set.
	 *
	 */
	 void toIDL(asdmIDL::CalPositionRowIDL& x) const;
#endif
	
#ifndef WITHOUT_ACS
	/**
	 * Fill the values of this row from the IDL struct CalPositionRowIDL.
	 * @param x The IDL struct containing the values used to fill this row.
	 * @throws ConversionException
	 */
	void setFromIDL (asdmIDL::CalPositionRowIDL x) ;
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

	std::map<std::string, CalPositionAttributeFromBin> fromBinMethods;
void antennaNameFromBin( EndianIStream& eis);
void atmPhaseCorrectionFromBin( EndianIStream& eis);
void calDataIdFromBin( EndianIStream& eis);
void calReductionIdFromBin( EndianIStream& eis);
void startValidTimeFromBin( EndianIStream& eis);
void endValidTimeFromBin( EndianIStream& eis);
void antennaPositionFromBin( EndianIStream& eis);
void stationNameFromBin( EndianIStream& eis);
void stationPositionFromBin( EndianIStream& eis);
void positionMethodFromBin( EndianIStream& eis);
void receiverBandFromBin( EndianIStream& eis);
void numAntennaFromBin( EndianIStream& eis);
void refAntennaNamesFromBin( EndianIStream& eis);
void axesOffsetFromBin( EndianIStream& eis);
void axesOffsetErrFromBin( EndianIStream& eis);
void axesOffsetFixedFromBin( EndianIStream& eis);
void positionOffsetFromBin( EndianIStream& eis);
void positionErrFromBin( EndianIStream& eis);
void reducedChiSquaredFromBin( EndianIStream& eis);

void delayRmsFromBin( EndianIStream& eis);
void phaseRmsFromBin( EndianIStream& eis);


	 /**
	  * Deserialize a stream of bytes read from an EndianIStream to build a PointingRow.
	  * @param eiss the EndianIStream to be read.
	  * @param table the CalPositionTable to which the row built by deserialization will be parented.
	  * @param attributesSeq a vector containing the names of the attributes . The elements order defines the order 
	  * in which the attributes are written in the binary serialization.
	  */
	 static CalPositionRow* fromBin(EndianIStream& eis, CalPositionTable& table, const std::vector<std::string>& attributesSeq);	 
 
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
	CalPositionTable &table;
	/**
	 * Whether this row has been added to the table or not.
	 */
	bool hasBeenAdded;

	// This method is used by the Table class when this row is added to the table.
	void isAdded(bool added);


	/**
	 * Create a CalPositionRow.
	 * <p>
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 */ 
	CalPositionRow (CalPositionTable &table);

	/**
	 * Create a CalPositionRow using a copy constructor mechanism.
	 * <p>
	 * Given a CalPositionRow row and a CalPositionTable table, the method creates a new
	 * CalPositionRow owned by table. Each attribute of the created row is a copy (deep)
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
	 CalPositionRow (CalPositionTable &table, CalPositionRow *row);
	 	
	////////////////////////////////
	// Intrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute antennaName
	
	

	std::string antennaName;

	
	
 	

	
	// ===> Attribute atmPhaseCorrection
	
	

	AtmPhaseCorrectionMod::AtmPhaseCorrection atmPhaseCorrection;

	
	
 	

	
	// ===> Attribute startValidTime
	
	

	ArrayTime startValidTime;

	
	
 	

	
	// ===> Attribute endValidTime
	
	

	ArrayTime endValidTime;

	
	
 	

	
	// ===> Attribute antennaPosition
	
	

	std::vector<Length > antennaPosition;

	
	
 	

	
	// ===> Attribute stationName
	
	

	std::string stationName;

	
	
 	

	
	// ===> Attribute stationPosition
	
	

	std::vector<Length > stationPosition;

	
	
 	

	
	// ===> Attribute positionMethod
	
	

	PositionMethodMod::PositionMethod positionMethod;

	
	
 	

	
	// ===> Attribute receiverBand
	
	

	ReceiverBandMod::ReceiverBand receiverBand;

	
	
 	

	
	// ===> Attribute numAntenna
	
	

	int numAntenna;

	
	
 	

	
	// ===> Attribute refAntennaNames
	
	

	std::vector<std::string > refAntennaNames;

	
	
 	

	
	// ===> Attribute axesOffset
	
	

	Length axesOffset;

	
	
 	

	
	// ===> Attribute axesOffsetErr
	
	

	Length axesOffsetErr;

	
	
 	

	
	// ===> Attribute axesOffsetFixed
	
	

	bool axesOffsetFixed;

	
	
 	

	
	// ===> Attribute positionOffset
	
	

	std::vector<Length > positionOffset;

	
	
 	

	
	// ===> Attribute positionErr
	
	

	std::vector<Length > positionErr;

	
	
 	

	
	// ===> Attribute reducedChiSquared
	
	

	double reducedChiSquared;

	
	
 	

	
	// ===> Attribute delayRms, which is optional
	
	
	bool delayRmsExists;
	

	double delayRms;

	
	
 	

	
	// ===> Attribute phaseRms, which is optional
	
	
	bool phaseRmsExists;
	

	Angle phaseRms;

	
	
 	

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
	std::map<std::string, CalPositionAttributeFromBin> fromBinMethods;
void antennaNameFromBin( EndianIStream& eis);
void atmPhaseCorrectionFromBin( EndianIStream& eis);
void calDataIdFromBin( EndianIStream& eis);
void calReductionIdFromBin( EndianIStream& eis);
void startValidTimeFromBin( EndianIStream& eis);
void endValidTimeFromBin( EndianIStream& eis);
void antennaPositionFromBin( EndianIStream& eis);
void stationNameFromBin( EndianIStream& eis);
void stationPositionFromBin( EndianIStream& eis);
void positionMethodFromBin( EndianIStream& eis);
void receiverBandFromBin( EndianIStream& eis);
void numAntennaFromBin( EndianIStream& eis);
void refAntennaNamesFromBin( EndianIStream& eis);
void axesOffsetFromBin( EndianIStream& eis);
void axesOffsetErrFromBin( EndianIStream& eis);
void axesOffsetFixedFromBin( EndianIStream& eis);
void positionOffsetFromBin( EndianIStream& eis);
void positionErrFromBin( EndianIStream& eis);
void reducedChiSquaredFromBin( EndianIStream& eis);

void delayRmsFromBin( EndianIStream& eis);
void phaseRmsFromBin( EndianIStream& eis);

*/
	
	///////////////////////////////////
	// text-deserialization material //
	///////////////////////////////////
	std::map<std::string, CalPositionAttributeFromText> fromTextMethods;
	
void antennaNameFromText (const std::string & s);
	
	
void atmPhaseCorrectionFromText (const std::string & s);
	
	
void calDataIdFromText (const std::string & s);
	
	
void calReductionIdFromText (const std::string & s);
	
	
void startValidTimeFromText (const std::string & s);
	
	
void endValidTimeFromText (const std::string & s);
	
	
void antennaPositionFromText (const std::string & s);
	
	
void stationNameFromText (const std::string & s);
	
	
void stationPositionFromText (const std::string & s);
	
	
void positionMethodFromText (const std::string & s);
	
	
void receiverBandFromText (const std::string & s);
	
	
void numAntennaFromText (const std::string & s);
	
	
void refAntennaNamesFromText (const std::string & s);
	
	
void axesOffsetFromText (const std::string & s);
	
	
void axesOffsetErrFromText (const std::string & s);
	
	
void axesOffsetFixedFromText (const std::string & s);
	
	
void positionOffsetFromText (const std::string & s);
	
	
void positionErrFromText (const std::string & s);
	
	
void reducedChiSquaredFromText (const std::string & s);
	

	
void delayRmsFromText (const std::string & s);
	
	
void phaseRmsFromText (const std::string & s);
	
	
	
	/**
	 * Serialize this into a stream of bytes written to an EndianOSStream.
	 * @param eoss the EndianOSStream to be written to
	 */
	 void toBin(EndianOSStream& eoss);
	 	 
	 /**
	  * Deserialize a stream of bytes read from an EndianIStream to build a PointingRow.
	  * @param eiss the EndianIStream to be read.
	  * @param table the CalPositionTable to which the row built by deserialization will be parented.
	  * @param attributesSeq a vector containing the names of the attributes . The elements order defines the order 
	  * in which the attributes are written in the binary serialization.

	 static CalPositionRow* fromBin(EndianIStream& eis, CalPositionTable& table, const std::vector<std::string>& attributesSeq);	 
		*/
};

} // End namespace asdm

#endif /* CalPosition_CLASS */
