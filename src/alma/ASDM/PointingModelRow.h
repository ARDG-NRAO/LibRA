
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
 * File PointingModelRow.h
 */
 
#ifndef PointingModelRow_CLASS
#define PointingModelRow_CLASS

#include <vector>
#include <string>
#include <set>

#ifndef WITHOUT_ACS
#include <asdmIDLC.h>
#endif






	 
#include <alma/ASDM/Tag.h>
	




	

	

	

	

	
#include <alma/Enumerations/CPolarizationType.h>
	

	
#include <alma/Enumerations/CReceiverBand.h>
	

	

	



#include <alma/ASDM/ConversionException.h>
#include <alma/ASDM/NoSuchRow.h>
#include <alma/ASDM/IllegalAccessException.h>

#include <alma/ASDM/RowTransformer.h>
//#include <alma/ASDM/TableStreamReader.h>

/*\file PointingModel.h
    \brief Generated from model's revision "-1", branch ""
*/

namespace asdm {

//class asdm::PointingModelTable;


// class asdm::PointingModelRow;
class PointingModelRow;

// class asdm::AntennaRow;
class AntennaRow;
	

class PointingModelRow;
typedef void (PointingModelRow::*PointingModelAttributeFromBin) (EndianIStream& eis);
typedef void (PointingModelRow::*PointingModelAttributeFromText) (const std::string& s);

/**
 * The PointingModelRow class is a row of a PointingModelTable.
 * 
 * Generated from model's revision "-1", branch ""
 *
 */
class PointingModelRow {
friend class asdm::PointingModelTable;
friend class asdm::RowTransformer<PointingModelRow>;
//friend class asdm::TableStreamReader<PointingModelTable, PointingModelRow>;

public:

	virtual ~PointingModelRow();

	/**
	 * Return the table to which this row belongs.
	 */
	PointingModelTable &getTable() const;
	
	/**
	 * Has this row been added to its table ?
	 * @return true if and only if it has been added.
	 */
	bool isAdded() const;
		
	////////////////////////////////
	// Intrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute pointingModelId
	
	
	

	
 	/**
 	 * Get pointingModelId.
 	 * @return pointingModelId as int
 	 */
 	int getPointingModelId() const;
	
 
 	
 	
	
	


	
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
  		
	
	
	


	
	// ===> Attribute coeffVal
	
	
	

	
 	/**
 	 * Get coeffVal.
 	 * @return coeffVal as std::vector<float >
 	 */
 	std::vector<float > getCoeffVal() const;
	
 
 	
 	
 	/**
 	 * Set coeffVal with the specified std::vector<float >.
 	 * @param coeffVal The std::vector<float > value to which coeffVal is to be set.
 	 
 		
 			
 	 */
 	void setCoeffVal (std::vector<float > coeffVal);
  		
	
	
	


	
	// ===> Attribute polarizationType
	
	
	

	
 	/**
 	 * Get polarizationType.
 	 * @return polarizationType as PolarizationTypeMod::PolarizationType
 	 */
 	PolarizationTypeMod::PolarizationType getPolarizationType() const;
	
 
 	
 	
 	/**
 	 * Set polarizationType with the specified PolarizationTypeMod::PolarizationType.
 	 * @param polarizationType The PolarizationTypeMod::PolarizationType value to which polarizationType is to be set.
 	 
 		
 			
 	 */
 	void setPolarizationType (PolarizationTypeMod::PolarizationType polarizationType);
  		
	
	
	


	
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
  		
	
	
	


	
	// ===> Attribute assocNature
	
	
	

	
 	/**
 	 * Get assocNature.
 	 * @return assocNature as std::string
 	 */
 	std::string getAssocNature() const;
	
 
 	
 	
 	/**
 	 * Set assocNature with the specified std::string.
 	 * @param assocNature The std::string value to which assocNature is to be set.
 	 
 		
 			
 	 */
 	void setAssocNature (std::string assocNature);
  		
	
	
	


	
	// ===> Attribute coeffFormula, which is optional
	
	
	
	/**
	 * The attribute coeffFormula is optional. Return true if this attribute exists.
	 * @return true if and only if the coeffFormula attribute exists. 
	 */
	bool isCoeffFormulaExists() const;
	

	
 	/**
 	 * Get coeffFormula, which is optional.
 	 * @return coeffFormula as std::vector<std::string >
 	 * @throws IllegalAccessException If coeffFormula does not exist.
 	 */
 	std::vector<std::string > getCoeffFormula() const;
	
 
 	
 	
 	/**
 	 * Set coeffFormula with the specified std::vector<std::string >.
 	 * @param coeffFormula The std::vector<std::string > value to which coeffFormula is to be set.
 	 
 		
 	 */
 	void setCoeffFormula (std::vector<std::string > coeffFormula);
		
	
	
	
	/**
	 * Mark coeffFormula, which is an optional field, as non-existent.
	 */
	void clearCoeffFormula ();
	


	////////////////////////////////
	// Extrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute antennaId
	
	
	

	
 	/**
 	 * Get antennaId.
 	 * @return antennaId as Tag
 	 */
 	Tag getAntennaId() const;
	
 
 	
 	
 	/**
 	 * Set antennaId with the specified Tag.
 	 * @param antennaId The Tag value to which antennaId is to be set.
 	 
 		
 			
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 		
 	 */
 	void setAntennaId (Tag antennaId);
  		
	
	
	


	
	// ===> Attribute assocPointingModelId
	
	
	

	
 	/**
 	 * Get assocPointingModelId.
 	 * @return assocPointingModelId as int
 	 */
 	int getAssocPointingModelId() const;
	
 
 	
 	
 	/**
 	 * Set assocPointingModelId with the specified int.
 	 * @param assocPointingModelId The int value to which assocPointingModelId is to be set.
 	 
 		
 			
 	 */
 	void setAssocPointingModelId (int assocPointingModelId);
  		
	
	
	


	///////////
	// Links //
	///////////
	
	

	
		
	// ===> Slice link from a row of PointingModel table to a collection of row of PointingModel table.
	
	/**
	 * Get the collection of row in the PointingModel table having pointingModelId == this.pointingModelId
	 * 
	 * @return a vector of PointingModelRow *
	 */
	std::vector <PointingModelRow *> getPointingModels();
	
	

	

	

	
		
	/**
	 * antennaId pointer to the row in the Antenna table having Antenna.antennaId == antennaId
	 * @return a AntennaRow*
	 * 
	 
	 */
	 AntennaRow* getAntennaUsingAntennaId();
	 

	

	
	
	
	/**
	 * Compare each mandatory attribute except the autoincrementable one of this PointingModelRow with 
	 * the corresponding parameters and return true if there is a match and false otherwise.
	 	
	 * @param antennaId
	    
	 * @param numCoeff
	    
	 * @param coeffName
	    
	 * @param coeffVal
	    
	 * @param polarizationType
	    
	 * @param receiverBand
	    
	 * @param assocNature
	    
	 * @param assocPointingModelId
	    
	 */ 
	bool compareNoAutoInc(Tag antennaId, int numCoeff, std::vector<std::string > coeffName, std::vector<float > coeffVal, PolarizationTypeMod::PolarizationType polarizationType, ReceiverBandMod::ReceiverBand receiverBand, std::string assocNature, int assocPointingModelId);
	
	

	
	/**
	 * Compare each mandatory value (i.e. not in the key) attribute  with 
	 * the corresponding parameters and return true if there is a match and false otherwise.
	 	
	 * @param numCoeff
	    
	 * @param coeffName
	    
	 * @param coeffVal
	    
	 * @param polarizationType
	    
	 * @param receiverBand
	    
	 * @param assocNature
	    
	 * @param assocPointingModelId
	    
	 */ 
	bool compareRequiredValue(int numCoeff, std::vector<std::string > coeffName, std::vector<float > coeffVal, PolarizationTypeMod::PolarizationType polarizationType, ReceiverBandMod::ReceiverBand receiverBand, std::string assocNature, int assocPointingModelId); 
		 
	
	/**
	 * Return true if all required attributes of the value part are equal to their homologues
	 * in x and false otherwise.
	 *
	 * @param x a pointer on the PointingModelRow whose required attributes of the value part 
	 * will be compared with those of this.
	 * @return a boolean.
	 */
	bool equalByRequiredValue(PointingModelRow* x) ;
	
#ifndef WITHOUT_ACS
	/**
	 * Return this row in the form of an IDL struct.
	 * @return The values of this row as a PointingModelRowIDL struct.
	 */
	asdmIDL::PointingModelRowIDL *toIDL() const;
	
	/**
	 * Define the content of a PointingModelRowIDL struct from the values
	 * found in this row.
	 *
	 * @param x a reference to the PointingModelRowIDL struct to be set.
	 *
	 */
	 void toIDL(asdmIDL::PointingModelRowIDL& x) const;
#endif
	
#ifndef WITHOUT_ACS
	/**
	 * Fill the values of this row from the IDL struct PointingModelRowIDL.
	 * @param x The IDL struct containing the values used to fill this row.
	 * @throws ConversionException
	 */
	void setFromIDL (asdmIDL::PointingModelRowIDL x) ;
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

	std::map<std::string, PointingModelAttributeFromBin> fromBinMethods;
void antennaIdFromBin( EndianIStream& eis);
void pointingModelIdFromBin( EndianIStream& eis);
void numCoeffFromBin( EndianIStream& eis);
void coeffNameFromBin( EndianIStream& eis);
void coeffValFromBin( EndianIStream& eis);
void polarizationTypeFromBin( EndianIStream& eis);
void receiverBandFromBin( EndianIStream& eis);
void assocNatureFromBin( EndianIStream& eis);
void assocPointingModelIdFromBin( EndianIStream& eis);

void coeffFormulaFromBin( EndianIStream& eis);


	 /**
	  * Deserialize a stream of bytes read from an EndianIStream to build a PointingRow.
	  * @param eiss the EndianIStream to be read.
	  * @param table the PointingModelTable to which the row built by deserialization will be parented.
	  * @param attributesSeq a vector containing the names of the attributes . The elements order defines the order 
	  * in which the attributes are written in the binary serialization.
	  */
	 static PointingModelRow* fromBin(EndianIStream& eis, PointingModelTable& table, const std::vector<std::string>& attributesSeq);	 
 
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
	PointingModelTable &table;
	/**
	 * Whether this row has been added to the table or not.
	 */
	bool hasBeenAdded;

	// This method is used by the Table class when this row is added to the table.
	void isAdded(bool added);


	/**
	 * Create a PointingModelRow.
	 * <p>
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 */ 
	PointingModelRow (PointingModelTable &table);

	/**
	 * Create a PointingModelRow using a copy constructor mechanism.
	 * <p>
	 * Given a PointingModelRow row and a PointingModelTable table, the method creates a new
	 * PointingModelRow owned by table. Each attribute of the created row is a copy (deep)
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
	 PointingModelRow (PointingModelTable &table, PointingModelRow *row);
	 	
	////////////////////////////////
	// Intrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute pointingModelId
	
	

	int pointingModelId;

	
	
 	
 	/**
 	 * Set pointingModelId with the specified int value.
 	 * @param pointingModelId The int value to which pointingModelId is to be set.
		
 		
			
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 		
 	 */
 	void setPointingModelId (int pointingModelId);
  		
	

	
	// ===> Attribute numCoeff
	
	

	int numCoeff;

	
	
 	

	
	// ===> Attribute coeffName
	
	

	std::vector<std::string > coeffName;

	
	
 	

	
	// ===> Attribute coeffVal
	
	

	std::vector<float > coeffVal;

	
	
 	

	
	// ===> Attribute polarizationType
	
	

	PolarizationTypeMod::PolarizationType polarizationType;

	
	
 	

	
	// ===> Attribute receiverBand
	
	

	ReceiverBandMod::ReceiverBand receiverBand;

	
	
 	

	
	// ===> Attribute assocNature
	
	

	std::string assocNature;

	
	
 	

	
	// ===> Attribute coeffFormula, which is optional
	
	
	bool coeffFormulaExists;
	

	std::vector<std::string > coeffFormula;

	
	
 	

	////////////////////////////////
	// Extrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute antennaId
	
	

	Tag antennaId;

	
	
 	

	
	// ===> Attribute assocPointingModelId
	
	

	int assocPointingModelId;

	
	
 	

	///////////
	// Links //
	///////////
	
	
		


	

	
		

	 

	

	
/*
	////////////////////////////////////////////////////////////
	// binary-deserialization material from an EndianIStream  //
	////////////////////////////////////////////////////////////
	std::map<std::string, PointingModelAttributeFromBin> fromBinMethods;
void antennaIdFromBin( EndianIStream& eis);
void pointingModelIdFromBin( EndianIStream& eis);
void numCoeffFromBin( EndianIStream& eis);
void coeffNameFromBin( EndianIStream& eis);
void coeffValFromBin( EndianIStream& eis);
void polarizationTypeFromBin( EndianIStream& eis);
void receiverBandFromBin( EndianIStream& eis);
void assocNatureFromBin( EndianIStream& eis);
void assocPointingModelIdFromBin( EndianIStream& eis);

void coeffFormulaFromBin( EndianIStream& eis);

*/
	
	///////////////////////////////////
	// text-deserialization material //
	///////////////////////////////////
	std::map<std::string, PointingModelAttributeFromText> fromTextMethods;
	
void antennaIdFromText (const std::string & s);
	
	
void pointingModelIdFromText (const std::string & s);
	
	
void numCoeffFromText (const std::string & s);
	
	
void coeffNameFromText (const std::string & s);
	
	
void coeffValFromText (const std::string & s);
	
	
void polarizationTypeFromText (const std::string & s);
	
	
void receiverBandFromText (const std::string & s);
	
	
void assocNatureFromText (const std::string & s);
	
	
void assocPointingModelIdFromText (const std::string & s);
	

	
void coeffFormulaFromText (const std::string & s);
	
	
	
	/**
	 * Serialize this into a stream of bytes written to an EndianOSStream.
	 * @param eoss the EndianOSStream to be written to
	 */
	 void toBin(EndianOSStream& eoss);
	 	 
	 /**
	  * Deserialize a stream of bytes read from an EndianIStream to build a PointingRow.
	  * @param eiss the EndianIStream to be read.
	  * @param table the PointingModelTable to which the row built by deserialization will be parented.
	  * @param attributesSeq a vector containing the names of the attributes . The elements order defines the order 
	  * in which the attributes are written in the binary serialization.

	 static PointingModelRow* fromBin(EndianIStream& eis, PointingModelTable& table, const std::vector<std::string>& attributesSeq);	 
		*/
};

} // End namespace asdm

#endif /* PointingModel_CLASS */
