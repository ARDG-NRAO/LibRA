
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
 * File CalFocusModelRow.h
 */
 
#ifndef CalFocusModelRow_CLASS
#define CalFocusModelRow_CLASS

#include <vector>
#include <string>
#include <set>

#ifndef WITHOUT_ACS
#include <asdmIDLC.h>
#endif






	 
#include <alma/ASDM/ArrayTime.h>
	

	 
#include <alma/ASDM/Length.h>
	

	 
#include <alma/ASDM/Tag.h>
	




	

	
#include <alma/Enumerations/CReceiverBand.h>
	

	
#include <alma/Enumerations/CPolarizationType.h>
	

	

	

	
#include <alma/Enumerations/CAntennaMake.h>
	

	

	

	

	

	

	

	

	

	

	



#include <alma/ASDM/ConversionException.h>
#include <alma/ASDM/NoSuchRow.h>
#include <alma/ASDM/IllegalAccessException.h>

#include <alma/ASDM/RowTransformer.h>
//#include <alma/ASDM/TableStreamReader.h>

/*\file CalFocusModel.h
    \brief Generated from model's revision "-1", branch ""
*/

namespace asdm {

//class asdm::CalFocusModelTable;


// class asdm::CalReductionRow;
class CalReductionRow;

// class asdm::CalDataRow;
class CalDataRow;
	

class CalFocusModelRow;
typedef void (CalFocusModelRow::*CalFocusModelAttributeFromBin) (EndianIStream& eis);
typedef void (CalFocusModelRow::*CalFocusModelAttributeFromText) (const std::string& s);

/**
 * The CalFocusModelRow class is a row of a CalFocusModelTable.
 * 
 * Generated from model's revision "-1", branch ""
 *
 */
class CalFocusModelRow {
friend class asdm::CalFocusModelTable;
friend class asdm::RowTransformer<CalFocusModelRow>;
//friend class asdm::TableStreamReader<CalFocusModelTable, CalFocusModelRow>;

public:

	virtual ~CalFocusModelRow();

	/**
	 * Return the table to which this row belongs.
	 */
	CalFocusModelTable &getTable() const;
	
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
  		
	
	
	


	
	// ===> Attribute polarizationType
	
	
	

	
 	/**
 	 * Get polarizationType.
 	 * @return polarizationType as PolarizationTypeMod::PolarizationType
 	 */
 	PolarizationTypeMod::PolarizationType getPolarizationType() const;
	
 
 	
 	
 	/**
 	 * Set polarizationType with the specified PolarizationTypeMod::PolarizationType.
 	 * @param polarizationType The PolarizationTypeMod::PolarizationType value to which polarizationType is to be set.
 	 
 		
 			
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 		
 	 */
 	void setPolarizationType (PolarizationTypeMod::PolarizationType polarizationType);
  		
	
	
	


	
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
  		
	
	
	


	
	// ===> Attribute antennaMake
	
	
	

	
 	/**
 	 * Get antennaMake.
 	 * @return antennaMake as AntennaMakeMod::AntennaMake
 	 */
 	AntennaMakeMod::AntennaMake getAntennaMake() const;
	
 
 	
 	
 	/**
 	 * Set antennaMake with the specified AntennaMakeMod::AntennaMake.
 	 * @param antennaMake The AntennaMakeMod::AntennaMake value to which antennaMake is to be set.
 	 
 		
 			
 	 */
 	void setAntennaMake (AntennaMakeMod::AntennaMake antennaMake);
  		
	
	
	


	
	// ===> Attribute numCoeff
	
	
	

	
 	/**
 	 * Get numCoeff.
 	 * @return numCoeff as int
 	 */
 	int getNumCoeff() const;
	
 
 	
 	
 	/**
 	 * Set numCoeff with the specified int.
 	 * @param numCoeff The int value to which numCoeff is to be set.
 	 
 		
 			
 	 */
 	void setNumCoeff (int numCoeff);
  		
	
	
	


	
	// ===> Attribute numSourceObs
	
	
	

	
 	/**
 	 * Get numSourceObs.
 	 * @return numSourceObs as int
 	 */
 	int getNumSourceObs() const;
	
 
 	
 	
 	/**
 	 * Set numSourceObs with the specified int.
 	 * @param numSourceObs The int value to which numSourceObs is to be set.
 	 
 		
 			
 	 */
 	void setNumSourceObs (int numSourceObs);
  		
	
	
	


	
	// ===> Attribute coeffName
	
	
	

	
 	/**
 	 * Get coeffName.
 	 * @return coeffName as std::vector<std::string >
 	 */
 	std::vector<std::string > getCoeffName() const;
	
 
 	
 	
 	/**
 	 * Set coeffName with the specified std::vector<std::string >.
 	 * @param coeffName The std::vector<std::string > value to which coeffName is to be set.
 	 
 		
 			
 	 */
 	void setCoeffName (std::vector<std::string > coeffName);
  		
	
	
	


	
	// ===> Attribute coeffFormula
	
	
	

	
 	/**
 	 * Get coeffFormula.
 	 * @return coeffFormula as std::vector<std::string >
 	 */
 	std::vector<std::string > getCoeffFormula() const;
	
 
 	
 	
 	/**
 	 * Set coeffFormula with the specified std::vector<std::string >.
 	 * @param coeffFormula The std::vector<std::string > value to which coeffFormula is to be set.
 	 
 		
 			
 	 */
 	void setCoeffFormula (std::vector<std::string > coeffFormula);
  		
	
	
	


	
	// ===> Attribute coeffValue
	
	
	

	
 	/**
 	 * Get coeffValue.
 	 * @return coeffValue as std::vector<float >
 	 */
 	std::vector<float > getCoeffValue() const;
	
 
 	
 	
 	/**
 	 * Set coeffValue with the specified std::vector<float >.
 	 * @param coeffValue The std::vector<float > value to which coeffValue is to be set.
 	 
 		
 			
 	 */
 	void setCoeffValue (std::vector<float > coeffValue);
  		
	
	
	


	
	// ===> Attribute coeffError
	
	
	

	
 	/**
 	 * Get coeffError.
 	 * @return coeffError as std::vector<float >
 	 */
 	std::vector<float > getCoeffError() const;
	
 
 	
 	
 	/**
 	 * Set coeffError with the specified std::vector<float >.
 	 * @param coeffError The std::vector<float > value to which coeffError is to be set.
 	 
 		
 			
 	 */
 	void setCoeffError (std::vector<float > coeffError);
  		
	
	
	


	
	// ===> Attribute coeffFixed
	
	
	

	
 	/**
 	 * Get coeffFixed.
 	 * @return coeffFixed as std::vector<bool >
 	 */
 	std::vector<bool > getCoeffFixed() const;
	
 
 	
 	
 	/**
 	 * Set coeffFixed with the specified std::vector<bool >.
 	 * @param coeffFixed The std::vector<bool > value to which coeffFixed is to be set.
 	 
 		
 			
 	 */
 	void setCoeffFixed (std::vector<bool > coeffFixed);
  		
	
	
	


	
	// ===> Attribute focusModel
	
	
	

	
 	/**
 	 * Get focusModel.
 	 * @return focusModel as std::string
 	 */
 	std::string getFocusModel() const;
	
 
 	
 	
 	/**
 	 * Set focusModel with the specified std::string.
 	 * @param focusModel The std::string value to which focusModel is to be set.
 	 
 		
 			
 	 */
 	void setFocusModel (std::string focusModel);
  		
	
	
	


	
	// ===> Attribute focusRMS
	
	
	

	
 	/**
 	 * Get focusRMS.
 	 * @return focusRMS as std::vector<Length >
 	 */
 	std::vector<Length > getFocusRMS() const;
	
 
 	
 	
 	/**
 	 * Set focusRMS with the specified std::vector<Length >.
 	 * @param focusRMS The std::vector<Length > value to which focusRMS is to be set.
 	 
 		
 			
 	 */
 	void setFocusRMS (std::vector<Length > focusRMS);
  		
	
	
	


	
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
	 * calReductionId pointer to the row in the CalReduction table having CalReduction.calReductionId == calReductionId
	 * @return a CalReductionRow*
	 * 
	 
	 */
	 CalReductionRow* getCalReductionUsingCalReductionId();
	 

	

	

	
		
	/**
	 * calDataId pointer to the row in the CalData table having CalData.calDataId == calDataId
	 * @return a CalDataRow*
	 * 
	 
	 */
	 CalDataRow* getCalDataUsingCalDataId();
	 

	

	
	
	
	/**
	 * Compare each mandatory attribute except the autoincrementable one of this CalFocusModelRow with 
	 * the corresponding parameters and return true if there is a match and false otherwise.
	 	
	 * @param antennaName
	    
	 * @param receiverBand
	    
	 * @param polarizationType
	    
	 * @param calDataId
	    
	 * @param calReductionId
	    
	 * @param startValidTime
	    
	 * @param endValidTime
	    
	 * @param antennaMake
	    
	 * @param numCoeff
	    
	 * @param numSourceObs
	    
	 * @param coeffName
	    
	 * @param coeffFormula
	    
	 * @param coeffValue
	    
	 * @param coeffError
	    
	 * @param coeffFixed
	    
	 * @param focusModel
	    
	 * @param focusRMS
	    
	 * @param reducedChiSquared
	    
	 */ 
	bool compareNoAutoInc(std::string antennaName, ReceiverBandMod::ReceiverBand receiverBand, PolarizationTypeMod::PolarizationType polarizationType, Tag calDataId, Tag calReductionId, ArrayTime startValidTime, ArrayTime endValidTime, AntennaMakeMod::AntennaMake antennaMake, int numCoeff, int numSourceObs, std::vector<std::string > coeffName, std::vector<std::string > coeffFormula, std::vector<float > coeffValue, std::vector<float > coeffError, std::vector<bool > coeffFixed, std::string focusModel, std::vector<Length > focusRMS, double reducedChiSquared);
	
	

	
	/**
	 * Compare each mandatory value (i.e. not in the key) attribute  with 
	 * the corresponding parameters and return true if there is a match and false otherwise.
	 	
	 * @param startValidTime
	    
	 * @param endValidTime
	    
	 * @param antennaMake
	    
	 * @param numCoeff
	    
	 * @param numSourceObs
	    
	 * @param coeffName
	    
	 * @param coeffFormula
	    
	 * @param coeffValue
	    
	 * @param coeffError
	    
	 * @param coeffFixed
	    
	 * @param focusModel
	    
	 * @param focusRMS
	    
	 * @param reducedChiSquared
	    
	 */ 
	bool compareRequiredValue(ArrayTime startValidTime, ArrayTime endValidTime, AntennaMakeMod::AntennaMake antennaMake, int numCoeff, int numSourceObs, std::vector<std::string > coeffName, std::vector<std::string > coeffFormula, std::vector<float > coeffValue, std::vector<float > coeffError, std::vector<bool > coeffFixed, std::string focusModel, std::vector<Length > focusRMS, double reducedChiSquared); 
		 
	
	/**
	 * Return true if all required attributes of the value part are equal to their homologues
	 * in x and false otherwise.
	 *
	 * @param x a pointer on the CalFocusModelRow whose required attributes of the value part 
	 * will be compared with those of this.
	 * @return a boolean.
	 */
	bool equalByRequiredValue(CalFocusModelRow* x) ;
	
#ifndef WITHOUT_ACS
	/**
	 * Return this row in the form of an IDL struct.
	 * @return The values of this row as a CalFocusModelRowIDL struct.
	 */
	asdmIDL::CalFocusModelRowIDL *toIDL() const;
	
	/**
	 * Define the content of a CalFocusModelRowIDL struct from the values
	 * found in this row.
	 *
	 * @param x a reference to the CalFocusModelRowIDL struct to be set.
	 *
	 */
	 void toIDL(asdmIDL::CalFocusModelRowIDL& x) const;
#endif
	
#ifndef WITHOUT_ACS
	/**
	 * Fill the values of this row from the IDL struct CalFocusModelRowIDL.
	 * @param x The IDL struct containing the values used to fill this row.
	 * @throws ConversionException
	 */
	void setFromIDL (asdmIDL::CalFocusModelRowIDL x) ;
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

	std::map<std::string, CalFocusModelAttributeFromBin> fromBinMethods;
void antennaNameFromBin( EndianIStream& eis);
void receiverBandFromBin( EndianIStream& eis);
void polarizationTypeFromBin( EndianIStream& eis);
void calDataIdFromBin( EndianIStream& eis);
void calReductionIdFromBin( EndianIStream& eis);
void startValidTimeFromBin( EndianIStream& eis);
void endValidTimeFromBin( EndianIStream& eis);
void antennaMakeFromBin( EndianIStream& eis);
void numCoeffFromBin( EndianIStream& eis);
void numSourceObsFromBin( EndianIStream& eis);
void coeffNameFromBin( EndianIStream& eis);
void coeffFormulaFromBin( EndianIStream& eis);
void coeffValueFromBin( EndianIStream& eis);
void coeffErrorFromBin( EndianIStream& eis);
void coeffFixedFromBin( EndianIStream& eis);
void focusModelFromBin( EndianIStream& eis);
void focusRMSFromBin( EndianIStream& eis);
void reducedChiSquaredFromBin( EndianIStream& eis);

	

	 /**
	  * Deserialize a stream of bytes read from an EndianIStream to build a PointingRow.
	  * @param eiss the EndianIStream to be read.
	  * @param table the CalFocusModelTable to which the row built by deserialization will be parented.
	  * @param attributesSeq a vector containing the names of the attributes . The elements order defines the order 
	  * in which the attributes are written in the binary serialization.
	  */
	 static CalFocusModelRow* fromBin(EndianIStream& eis, CalFocusModelTable& table, const std::vector<std::string>& attributesSeq);	 
 
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
	CalFocusModelTable &table;
	/**
	 * Whether this row has been added to the table or not.
	 */
	bool hasBeenAdded;

	// This method is used by the Table class when this row is added to the table.
	void isAdded(bool added);


	/**
	 * Create a CalFocusModelRow.
	 * <p>
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 */ 
	CalFocusModelRow (CalFocusModelTable &table);

	/**
	 * Create a CalFocusModelRow using a copy constructor mechanism.
	 * <p>
	 * Given a CalFocusModelRow row and a CalFocusModelTable table, the method creates a new
	 * CalFocusModelRow owned by table. Each attribute of the created row is a copy (deep)
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
	 CalFocusModelRow (CalFocusModelTable &table, CalFocusModelRow *row);
	 	
	////////////////////////////////
	// Intrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute antennaName
	
	

	std::string antennaName;

	
	
 	

	
	// ===> Attribute receiverBand
	
	

	ReceiverBandMod::ReceiverBand receiverBand;

	
	
 	

	
	// ===> Attribute polarizationType
	
	

	PolarizationTypeMod::PolarizationType polarizationType;

	
	
 	

	
	// ===> Attribute startValidTime
	
	

	ArrayTime startValidTime;

	
	
 	

	
	// ===> Attribute endValidTime
	
	

	ArrayTime endValidTime;

	
	
 	

	
	// ===> Attribute antennaMake
	
	

	AntennaMakeMod::AntennaMake antennaMake;

	
	
 	

	
	// ===> Attribute numCoeff
	
	

	int numCoeff;

	
	
 	

	
	// ===> Attribute numSourceObs
	
	

	int numSourceObs;

	
	
 	

	
	// ===> Attribute coeffName
	
	

	std::vector<std::string > coeffName;

	
	
 	

	
	// ===> Attribute coeffFormula
	
	

	std::vector<std::string > coeffFormula;

	
	
 	

	
	// ===> Attribute coeffValue
	
	

	std::vector<float > coeffValue;

	
	
 	

	
	// ===> Attribute coeffError
	
	

	std::vector<float > coeffError;

	
	
 	

	
	// ===> Attribute coeffFixed
	
	

	std::vector<bool > coeffFixed;

	
	
 	

	
	// ===> Attribute focusModel
	
	

	std::string focusModel;

	
	
 	

	
	// ===> Attribute focusRMS
	
	

	std::vector<Length > focusRMS;

	
	
 	

	
	// ===> Attribute reducedChiSquared
	
	

	double reducedChiSquared;

	
	
 	

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
	std::map<std::string, CalFocusModelAttributeFromBin> fromBinMethods;
void antennaNameFromBin( EndianIStream& eis);
void receiverBandFromBin( EndianIStream& eis);
void polarizationTypeFromBin( EndianIStream& eis);
void calDataIdFromBin( EndianIStream& eis);
void calReductionIdFromBin( EndianIStream& eis);
void startValidTimeFromBin( EndianIStream& eis);
void endValidTimeFromBin( EndianIStream& eis);
void antennaMakeFromBin( EndianIStream& eis);
void numCoeffFromBin( EndianIStream& eis);
void numSourceObsFromBin( EndianIStream& eis);
void coeffNameFromBin( EndianIStream& eis);
void coeffFormulaFromBin( EndianIStream& eis);
void coeffValueFromBin( EndianIStream& eis);
void coeffErrorFromBin( EndianIStream& eis);
void coeffFixedFromBin( EndianIStream& eis);
void focusModelFromBin( EndianIStream& eis);
void focusRMSFromBin( EndianIStream& eis);
void reducedChiSquaredFromBin( EndianIStream& eis);

	
*/
	
	///////////////////////////////////
	// text-deserialization material //
	///////////////////////////////////
	std::map<std::string, CalFocusModelAttributeFromText> fromTextMethods;
	
void antennaNameFromText (const std::string & s);
	
	
void receiverBandFromText (const std::string & s);
	
	
void polarizationTypeFromText (const std::string & s);
	
	
void calDataIdFromText (const std::string & s);
	
	
void calReductionIdFromText (const std::string & s);
	
	
void startValidTimeFromText (const std::string & s);
	
	
void endValidTimeFromText (const std::string & s);
	
	
void antennaMakeFromText (const std::string & s);
	
	
void numCoeffFromText (const std::string & s);
	
	
void numSourceObsFromText (const std::string & s);
	
	
void coeffNameFromText (const std::string & s);
	
	
void coeffFormulaFromText (const std::string & s);
	
	
void coeffValueFromText (const std::string & s);
	
	
void coeffErrorFromText (const std::string & s);
	
	
void coeffFixedFromText (const std::string & s);
	
	
void focusModelFromText (const std::string & s);
	
	
void focusRMSFromText (const std::string & s);
	
	
void reducedChiSquaredFromText (const std::string & s);
	

		
	
	/**
	 * Serialize this into a stream of bytes written to an EndianOSStream.
	 * @param eoss the EndianOSStream to be written to
	 */
	 void toBin(EndianOSStream& eoss);
	 	 
	 /**
	  * Deserialize a stream of bytes read from an EndianIStream to build a PointingRow.
	  * @param eiss the EndianIStream to be read.
	  * @param table the CalFocusModelTable to which the row built by deserialization will be parented.
	  * @param attributesSeq a vector containing the names of the attributes . The elements order defines the order 
	  * in which the attributes are written in the binary serialization.

	 static CalFocusModelRow* fromBin(EndianIStream& eis, CalFocusModelTable& table, const std::vector<std::string>& attributesSeq);	 
		*/
};

} // End namespace asdm

#endif /* CalFocusModel_CLASS */
