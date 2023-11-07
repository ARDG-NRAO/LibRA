
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
 * File SeeingRow.h
 */
 
#ifndef SeeingRow_CLASS
#define SeeingRow_CLASS

#include <vector>
#include <string>
#include <set>

#ifndef WITHOUT_ACS
#include <asdmIDLC.h>
#endif






	 
#include <alma/ASDM/ArrayTimeInterval.h>
	

	 
#include <alma/ASDM/Angle.h>
	

	 
#include <alma/ASDM/Length.h>
	




	

	

	

	

	

	



#include <alma/ASDM/ConversionException.h>
#include <alma/ASDM/NoSuchRow.h>
#include <alma/ASDM/IllegalAccessException.h>

#include <alma/ASDM/RowTransformer.h>
//#include <alma/ASDM/TableStreamReader.h>

/*\file Seeing.h
    \brief Generated from model's revision "-1", branch ""
*/

namespace asdm {

//class asdm::SeeingTable;

	

class SeeingRow;
typedef void (SeeingRow::*SeeingAttributeFromBin) (EndianIStream& eis);
typedef void (SeeingRow::*SeeingAttributeFromText) (const std::string& s);

/**
 * The SeeingRow class is a row of a SeeingTable.
 * 
 * Generated from model's revision "-1", branch ""
 *
 */
class SeeingRow {
friend class asdm::SeeingTable;
friend class asdm::RowTransformer<SeeingRow>;
//friend class asdm::TableStreamReader<SeeingTable, SeeingRow>;

public:

	virtual ~SeeingRow();

	/**
	 * Return the table to which this row belongs.
	 */
	SeeingTable &getTable() const;
	
	/**
	 * Has this row been added to its table ?
	 * @return true if and only if it has been added.
	 */
	bool isAdded() const;
		
	////////////////////////////////
	// Intrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute timeInterval
	
	
	

	
 	/**
 	 * Get timeInterval.
 	 * @return timeInterval as ArrayTimeInterval
 	 */
 	ArrayTimeInterval getTimeInterval() const;
	
 
 	
 	
 	/**
 	 * Set timeInterval with the specified ArrayTimeInterval.
 	 * @param timeInterval The ArrayTimeInterval value to which timeInterval is to be set.
 	 
 		
 			
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 		
 	 */
 	void setTimeInterval (ArrayTimeInterval timeInterval);
  		
	
	
	


	
	// ===> Attribute numBaseLength
	
	
	

	
 	/**
 	 * Get numBaseLength.
 	 * @return numBaseLength as int
 	 */
 	int getNumBaseLength() const;
	
 
 	
 	
 	/**
 	 * Set numBaseLength with the specified int.
 	 * @param numBaseLength The int value to which numBaseLength is to be set.
 	 
 		
 			
 	 */
 	void setNumBaseLength (int numBaseLength);
  		
	
	
	


	
	// ===> Attribute baseLength
	
	
	

	
 	/**
 	 * Get baseLength.
 	 * @return baseLength as std::vector<Length >
 	 */
 	std::vector<Length > getBaseLength() const;
	
 
 	
 	
 	/**
 	 * Set baseLength with the specified std::vector<Length >.
 	 * @param baseLength The std::vector<Length > value to which baseLength is to be set.
 	 
 		
 			
 	 */
 	void setBaseLength (std::vector<Length > baseLength);
  		
	
	
	


	
	// ===> Attribute phaseRms
	
	
	

	
 	/**
 	 * Get phaseRms.
 	 * @return phaseRms as std::vector<Angle >
 	 */
 	std::vector<Angle > getPhaseRms() const;
	
 
 	
 	
 	/**
 	 * Set phaseRms with the specified std::vector<Angle >.
 	 * @param phaseRms The std::vector<Angle > value to which phaseRms is to be set.
 	 
 		
 			
 	 */
 	void setPhaseRms (std::vector<Angle > phaseRms);
  		
	
	
	


	
	// ===> Attribute seeing
	
	
	

	
 	/**
 	 * Get seeing.
 	 * @return seeing as float
 	 */
 	float getSeeing() const;
	
 
 	
 	
 	/**
 	 * Set seeing with the specified float.
 	 * @param seeing The float value to which seeing is to be set.
 	 
 		
 			
 	 */
 	void setSeeing (float seeing);
  		
	
	
	


	
	// ===> Attribute exponent
	
	
	

	
 	/**
 	 * Get exponent.
 	 * @return exponent as float
 	 */
 	float getExponent() const;
	
 
 	
 	
 	/**
 	 * Set exponent with the specified float.
 	 * @param exponent The float value to which exponent is to be set.
 	 
 		
 			
 	 */
 	void setExponent (float exponent);
  		
	
	
	


	////////////////////////////////
	// Extrinsic Table Attributes //
	////////////////////////////////
	
	///////////
	// Links //
	///////////
	
	
	
	
	/**
	 * Compare each mandatory attribute except the autoincrementable one of this SeeingRow with 
	 * the corresponding parameters and return true if there is a match and false otherwise.
	 	
	 * @param timeInterval
	    
	 * @param numBaseLength
	    
	 * @param baseLength
	    
	 * @param phaseRms
	    
	 * @param seeing
	    
	 * @param exponent
	    
	 */ 
	bool compareNoAutoInc(ArrayTimeInterval timeInterval, int numBaseLength, std::vector<Length > baseLength, std::vector<Angle > phaseRms, float seeing, float exponent);
	
	

	
	/**
	 * Compare each mandatory value (i.e. not in the key) attribute  with 
	 * the corresponding parameters and return true if there is a match and false otherwise.
	 	
	 * @param numBaseLength
	    
	 * @param baseLength
	    
	 * @param phaseRms
	    
	 * @param seeing
	    
	 * @param exponent
	    
	 */ 
	bool compareRequiredValue(int numBaseLength, std::vector<Length > baseLength, std::vector<Angle > phaseRms, float seeing, float exponent); 
		 
	
	/**
	 * Return true if all required attributes of the value part are equal to their homologues
	 * in x and false otherwise.
	 *
	 * @param x a pointer on the SeeingRow whose required attributes of the value part 
	 * will be compared with those of this.
	 * @return a boolean.
	 */
	bool equalByRequiredValue(SeeingRow* x) ;
	
#ifndef WITHOUT_ACS
	/**
	 * Return this row in the form of an IDL struct.
	 * @return The values of this row as a SeeingRowIDL struct.
	 */
	asdmIDL::SeeingRowIDL *toIDL() const;
	
	/**
	 * Define the content of a SeeingRowIDL struct from the values
	 * found in this row.
	 *
	 * @param x a reference to the SeeingRowIDL struct to be set.
	 *
	 */
	 void toIDL(asdmIDL::SeeingRowIDL& x) const;
#endif
	
#ifndef WITHOUT_ACS
	/**
	 * Fill the values of this row from the IDL struct SeeingRowIDL.
	 * @param x The IDL struct containing the values used to fill this row.
	 * @throws ConversionException
	 */
	void setFromIDL (asdmIDL::SeeingRowIDL x) ;
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

	std::map<std::string, SeeingAttributeFromBin> fromBinMethods;
void timeIntervalFromBin( EndianIStream& eis);
void numBaseLengthFromBin( EndianIStream& eis);
void baseLengthFromBin( EndianIStream& eis);
void phaseRmsFromBin( EndianIStream& eis);
void seeingFromBin( EndianIStream& eis);
void exponentFromBin( EndianIStream& eis);

	

	 /**
	  * Deserialize a stream of bytes read from an EndianIStream to build a PointingRow.
	  * @param eiss the EndianIStream to be read.
	  * @param table the SeeingTable to which the row built by deserialization will be parented.
	  * @param attributesSeq a vector containing the names of the attributes . The elements order defines the order 
	  * in which the attributes are written in the binary serialization.
	  */
	 static SeeingRow* fromBin(EndianIStream& eis, SeeingTable& table, const std::vector<std::string>& attributesSeq);	 
 
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
	SeeingTable &table;
	/**
	 * Whether this row has been added to the table or not.
	 */
	bool hasBeenAdded;

	// This method is used by the Table class when this row is added to the table.
	void isAdded(bool added);


	/**
	 * Create a SeeingRow.
	 * <p>
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 */ 
	SeeingRow (SeeingTable &table);

	/**
	 * Create a SeeingRow using a copy constructor mechanism.
	 * <p>
	 * Given a SeeingRow row and a SeeingTable table, the method creates a new
	 * SeeingRow owned by table. Each attribute of the created row is a copy (deep)
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
	 SeeingRow (SeeingTable &table, SeeingRow *row);
	 	
	////////////////////////////////
	// Intrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute timeInterval
	
	

	ArrayTimeInterval timeInterval;

	
	
 	

	
	// ===> Attribute numBaseLength
	
	

	int numBaseLength;

	
	
 	

	
	// ===> Attribute baseLength
	
	

	std::vector<Length > baseLength;

	
	
 	

	
	// ===> Attribute phaseRms
	
	

	std::vector<Angle > phaseRms;

	
	
 	

	
	// ===> Attribute seeing
	
	

	float seeing;

	
	
 	

	
	// ===> Attribute exponent
	
	

	float exponent;

	
	
 	

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
	std::map<std::string, SeeingAttributeFromBin> fromBinMethods;
void timeIntervalFromBin( EndianIStream& eis);
void numBaseLengthFromBin( EndianIStream& eis);
void baseLengthFromBin( EndianIStream& eis);
void phaseRmsFromBin( EndianIStream& eis);
void seeingFromBin( EndianIStream& eis);
void exponentFromBin( EndianIStream& eis);

	
*/
	
	///////////////////////////////////
	// text-deserialization material //
	///////////////////////////////////
	std::map<std::string, SeeingAttributeFromText> fromTextMethods;
	
void timeIntervalFromText (const std::string & s);
	
	
void numBaseLengthFromText (const std::string & s);
	
	
void baseLengthFromText (const std::string & s);
	
	
void phaseRmsFromText (const std::string & s);
	
	
void seeingFromText (const std::string & s);
	
	
void exponentFromText (const std::string & s);
	

		
	
	/**
	 * Serialize this into a stream of bytes written to an EndianOSStream.
	 * @param eoss the EndianOSStream to be written to
	 */
	 void toBin(EndianOSStream& eoss);
	 	 
	 /**
	  * Deserialize a stream of bytes read from an EndianIStream to build a PointingRow.
	  * @param eiss the EndianIStream to be read.
	  * @param table the SeeingTable to which the row built by deserialization will be parented.
	  * @param attributesSeq a vector containing the names of the attributes . The elements order defines the order 
	  * in which the attributes are written in the binary serialization.

	 static SeeingRow* fromBin(EndianIStream& eis, SeeingTable& table, const std::vector<std::string>& attributesSeq);	 
		*/
};

} // End namespace asdm

#endif /* Seeing_CLASS */
