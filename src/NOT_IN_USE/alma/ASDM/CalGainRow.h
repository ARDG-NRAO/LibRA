
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
 * File CalGainRow.h
 */
 
#ifndef CalGainRow_CLASS
#define CalGainRow_CLASS

#include <vector>
#include <string>
#include <set>

#ifndef WITHOUT_ACS
#include <asdmIDLC.h>
#endif






	 
#include <alma/ASDM/ArrayTime.h>
	

	 
#include <alma/ASDM/Tag.h>
	




	

	

	

	

	

	

	

	

	



#include <alma/ASDM/ConversionException.h>
#include <alma/ASDM/NoSuchRow.h>
#include <alma/ASDM/IllegalAccessException.h>

#include <alma/ASDM/RowTransformer.h>
//#include <alma/ASDM/TableStreamReader.h>

/*\file CalGain.h
    \brief Generated from model's revision "-1", branch ""
*/

namespace asdm {

//class asdm::CalGainTable;


// class asdm::CalReductionRow;
class CalReductionRow;

// class asdm::CalDataRow;
class CalDataRow;
	

class CalGainRow;
typedef void (CalGainRow::*CalGainAttributeFromBin) (EndianIStream& eis);
typedef void (CalGainRow::*CalGainAttributeFromText) (const std::string& s);

/**
 * The CalGainRow class is a row of a CalGainTable.
 * 
 * Generated from model's revision "-1", branch ""
 *
 */
class CalGainRow {
friend class asdm::CalGainTable;
friend class asdm::RowTransformer<CalGainRow>;
//friend class asdm::TableStreamReader<CalGainTable, CalGainRow>;

public:

	virtual ~CalGainRow();

	/**
	 * Return the table to which this row belongs.
	 */
	CalGainTable &getTable() const;
	
	/**
	 * Has this row been added to its table ?
	 * @return true if and only if it has been added.
	 */
	bool isAdded() const;
		
	////////////////////////////////
	// Intrinsic Table Attributes //
	////////////////////////////////
	
	
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
  		
	
	
	


	
	// ===> Attribute gain
	
	
	

	
 	/**
 	 * Get gain.
 	 * @return gain as float
 	 */
 	float getGain() const;
	
 
 	
 	
 	/**
 	 * Set gain with the specified float.
 	 * @param gain The float value to which gain is to be set.
 	 
 		
 			
 	 */
 	void setGain (float gain);
  		
	
	
	


	
	// ===> Attribute gainValid
	
	
	

	
 	/**
 	 * Get gainValid.
 	 * @return gainValid as bool
 	 */
 	bool getGainValid() const;
	
 
 	
 	
 	/**
 	 * Set gainValid with the specified bool.
 	 * @param gainValid The bool value to which gainValid is to be set.
 	 
 		
 			
 	 */
 	void setGainValid (bool gainValid);
  		
	
	
	


	
	// ===> Attribute fit
	
	
	

	
 	/**
 	 * Get fit.
 	 * @return fit as float
 	 */
 	float getFit() const;
	
 
 	
 	
 	/**
 	 * Set fit with the specified float.
 	 * @param fit The float value to which fit is to be set.
 	 
 		
 			
 	 */
 	void setFit (float fit);
  		
	
	
	


	
	// ===> Attribute fitWeight
	
	
	

	
 	/**
 	 * Get fitWeight.
 	 * @return fitWeight as float
 	 */
 	float getFitWeight() const;
	
 
 	
 	
 	/**
 	 * Set fitWeight with the specified float.
 	 * @param fitWeight The float value to which fitWeight is to be set.
 	 
 		
 			
 	 */
 	void setFitWeight (float fitWeight);
  		
	
	
	


	
	// ===> Attribute totalGainValid
	
	
	

	
 	/**
 	 * Get totalGainValid.
 	 * @return totalGainValid as bool
 	 */
 	bool getTotalGainValid() const;
	
 
 	
 	
 	/**
 	 * Set totalGainValid with the specified bool.
 	 * @param totalGainValid The bool value to which totalGainValid is to be set.
 	 
 		
 			
 	 */
 	void setTotalGainValid (bool totalGainValid);
  		
	
	
	


	
	// ===> Attribute totalFit
	
	
	

	
 	/**
 	 * Get totalFit.
 	 * @return totalFit as float
 	 */
 	float getTotalFit() const;
	
 
 	
 	
 	/**
 	 * Set totalFit with the specified float.
 	 * @param totalFit The float value to which totalFit is to be set.
 	 
 		
 			
 	 */
 	void setTotalFit (float totalFit);
  		
	
	
	


	
	// ===> Attribute totalFitWeight
	
	
	

	
 	/**
 	 * Get totalFitWeight.
 	 * @return totalFitWeight as float
 	 */
 	float getTotalFitWeight() const;
	
 
 	
 	
 	/**
 	 * Set totalFitWeight with the specified float.
 	 * @param totalFitWeight The float value to which totalFitWeight is to be set.
 	 
 		
 			
 	 */
 	void setTotalFitWeight (float totalFitWeight);
  		
	
	
	


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
	 * Compare each mandatory attribute except the autoincrementable one of this CalGainRow with 
	 * the corresponding parameters and return true if there is a match and false otherwise.
	 	
	 * @param calDataId
	    
	 * @param calReductionId
	    
	 * @param startValidTime
	    
	 * @param endValidTime
	    
	 * @param gain
	    
	 * @param gainValid
	    
	 * @param fit
	    
	 * @param fitWeight
	    
	 * @param totalGainValid
	    
	 * @param totalFit
	    
	 * @param totalFitWeight
	    
	 */ 
	bool compareNoAutoInc(Tag calDataId, Tag calReductionId, ArrayTime startValidTime, ArrayTime endValidTime, float gain, bool gainValid, float fit, float fitWeight, bool totalGainValid, float totalFit, float totalFitWeight);
	
	

	
	/**
	 * Compare each mandatory value (i.e. not in the key) attribute  with 
	 * the corresponding parameters and return true if there is a match and false otherwise.
	 	
	 * @param startValidTime
	    
	 * @param endValidTime
	    
	 * @param gain
	    
	 * @param gainValid
	    
	 * @param fit
	    
	 * @param fitWeight
	    
	 * @param totalGainValid
	    
	 * @param totalFit
	    
	 * @param totalFitWeight
	    
	 */ 
	bool compareRequiredValue(ArrayTime startValidTime, ArrayTime endValidTime, float gain, bool gainValid, float fit, float fitWeight, bool totalGainValid, float totalFit, float totalFitWeight); 
		 
	
	/**
	 * Return true if all required attributes of the value part are equal to their homologues
	 * in x and false otherwise.
	 *
	 * @param x a pointer on the CalGainRow whose required attributes of the value part 
	 * will be compared with those of this.
	 * @return a boolean.
	 */
	bool equalByRequiredValue(CalGainRow* x) ;
	
#ifndef WITHOUT_ACS
	/**
	 * Return this row in the form of an IDL struct.
	 * @return The values of this row as a CalGainRowIDL struct.
	 */
	asdmIDL::CalGainRowIDL *toIDL() const;
	
	/**
	 * Define the content of a CalGainRowIDL struct from the values
	 * found in this row.
	 *
	 * @param x a reference to the CalGainRowIDL struct to be set.
	 *
	 */
	 void toIDL(asdmIDL::CalGainRowIDL& x) const;
#endif
	
#ifndef WITHOUT_ACS
	/**
	 * Fill the values of this row from the IDL struct CalGainRowIDL.
	 * @param x The IDL struct containing the values used to fill this row.
	 * @throws ConversionException
	 */
	void setFromIDL (asdmIDL::CalGainRowIDL x) ;
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

	std::map<std::string, CalGainAttributeFromBin> fromBinMethods;
void calDataIdFromBin( EndianIStream& eis);
void calReductionIdFromBin( EndianIStream& eis);
void startValidTimeFromBin( EndianIStream& eis);
void endValidTimeFromBin( EndianIStream& eis);
void gainFromBin( EndianIStream& eis);
void gainValidFromBin( EndianIStream& eis);
void fitFromBin( EndianIStream& eis);
void fitWeightFromBin( EndianIStream& eis);
void totalGainValidFromBin( EndianIStream& eis);
void totalFitFromBin( EndianIStream& eis);
void totalFitWeightFromBin( EndianIStream& eis);

	

	 /**
	  * Deserialize a stream of bytes read from an EndianIStream to build a PointingRow.
	  * @param eiss the EndianIStream to be read.
	  * @param table the CalGainTable to which the row built by deserialization will be parented.
	  * @param attributesSeq a vector containing the names of the attributes . The elements order defines the order 
	  * in which the attributes are written in the binary serialization.
	  */
	 static CalGainRow* fromBin(EndianIStream& eis, CalGainTable& table, const std::vector<std::string>& attributesSeq);	 
 
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
	CalGainTable &table;
	/**
	 * Whether this row has been added to the table or not.
	 */
	bool hasBeenAdded;

	// This method is used by the Table class when this row is added to the table.
	void isAdded(bool added);


	/**
	 * Create a CalGainRow.
	 * <p>
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 */ 
	CalGainRow (CalGainTable &table);

	/**
	 * Create a CalGainRow using a copy constructor mechanism.
	 * <p>
	 * Given a CalGainRow row and a CalGainTable table, the method creates a new
	 * CalGainRow owned by table. Each attribute of the created row is a copy (deep)
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
	 CalGainRow (CalGainTable &table, CalGainRow *row);
	 	
	////////////////////////////////
	// Intrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute startValidTime
	
	

	ArrayTime startValidTime;

	
	
 	

	
	// ===> Attribute endValidTime
	
	

	ArrayTime endValidTime;

	
	
 	

	
	// ===> Attribute gain
	
	

	float gain;

	
	
 	

	
	// ===> Attribute gainValid
	
	

	bool gainValid;

	
	
 	

	
	// ===> Attribute fit
	
	

	float fit;

	
	
 	

	
	// ===> Attribute fitWeight
	
	

	float fitWeight;

	
	
 	

	
	// ===> Attribute totalGainValid
	
	

	bool totalGainValid;

	
	
 	

	
	// ===> Attribute totalFit
	
	

	float totalFit;

	
	
 	

	
	// ===> Attribute totalFitWeight
	
	

	float totalFitWeight;

	
	
 	

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
	std::map<std::string, CalGainAttributeFromBin> fromBinMethods;
void calDataIdFromBin( EndianIStream& eis);
void calReductionIdFromBin( EndianIStream& eis);
void startValidTimeFromBin( EndianIStream& eis);
void endValidTimeFromBin( EndianIStream& eis);
void gainFromBin( EndianIStream& eis);
void gainValidFromBin( EndianIStream& eis);
void fitFromBin( EndianIStream& eis);
void fitWeightFromBin( EndianIStream& eis);
void totalGainValidFromBin( EndianIStream& eis);
void totalFitFromBin( EndianIStream& eis);
void totalFitWeightFromBin( EndianIStream& eis);

	
*/
	
	///////////////////////////////////
	// text-deserialization material //
	///////////////////////////////////
	std::map<std::string, CalGainAttributeFromText> fromTextMethods;
	
void calDataIdFromText (const std::string & s);
	
	
void calReductionIdFromText (const std::string & s);
	
	
void startValidTimeFromText (const std::string & s);
	
	
void endValidTimeFromText (const std::string & s);
	
	
void gainFromText (const std::string & s);
	
	
void gainValidFromText (const std::string & s);
	
	
void fitFromText (const std::string & s);
	
	
void fitWeightFromText (const std::string & s);
	
	
void totalGainValidFromText (const std::string & s);
	
	
void totalFitFromText (const std::string & s);
	
	
void totalFitWeightFromText (const std::string & s);
	

		
	
	/**
	 * Serialize this into a stream of bytes written to an EndianOSStream.
	 * @param eoss the EndianOSStream to be written to
	 */
	 void toBin(EndianOSStream& eoss);
	 	 
	 /**
	  * Deserialize a stream of bytes read from an EndianIStream to build a PointingRow.
	  * @param eiss the EndianIStream to be read.
	  * @param table the CalGainTable to which the row built by deserialization will be parented.
	  * @param attributesSeq a vector containing the names of the attributes . The elements order defines the order 
	  * in which the attributes are written in the binary serialization.

	 static CalGainRow* fromBin(EndianIStream& eis, CalGainTable& table, const std::vector<std::string>& attributesSeq);	 
		*/
};

} // End namespace asdm

#endif /* CalGain_CLASS */
