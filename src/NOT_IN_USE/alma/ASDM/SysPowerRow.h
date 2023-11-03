
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
 * File SysPowerRow.h
 */
 
#ifndef SysPowerRow_CLASS
#define SysPowerRow_CLASS

#include <vector>
#include <string>
#include <set>

#ifndef WITHOUT_ACS
#include <asdmIDLC.h>
#endif






	 
#include <alma/ASDM/ArrayTimeInterval.h>
	

	 
#include <alma/ASDM/Tag.h>
	




	

	

	

	

	



#include <alma/ASDM/ConversionException.h>
#include <alma/ASDM/NoSuchRow.h>
#include <alma/ASDM/IllegalAccessException.h>

#include <alma/ASDM/RowTransformer.h>
//#include <alma/ASDM/TableStreamReader.h>

/*\file SysPower.h
    \brief Generated from model's revision "-1", branch ""
*/

namespace asdm {

//class asdm::SysPowerTable;


// class asdm::AntennaRow;
class AntennaRow;

// class asdm::SpectralWindowRow;
class SpectralWindowRow;

// class asdm::FeedRow;
class FeedRow;
	

class SysPowerRow;
typedef void (SysPowerRow::*SysPowerAttributeFromBin) (EndianIStream& eis);
typedef void (SysPowerRow::*SysPowerAttributeFromText) (const std::string& s);

/**
 * The SysPowerRow class is a row of a SysPowerTable.
 * 
 * Generated from model's revision "-1", branch ""
 *
 */
class SysPowerRow {
friend class asdm::SysPowerTable;
friend class asdm::RowTransformer<SysPowerRow>;
//friend class asdm::TableStreamReader<SysPowerTable, SysPowerRow>;

public:

	virtual ~SysPowerRow();

	/**
	 * Return the table to which this row belongs.
	 */
	SysPowerTable &getTable() const;
	
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
  		
	
	
	


	
	// ===> Attribute switchedPowerDifference, which is optional
	
	
	
	/**
	 * The attribute switchedPowerDifference is optional. Return true if this attribute exists.
	 * @return true if and only if the switchedPowerDifference attribute exists. 
	 */
	bool isSwitchedPowerDifferenceExists() const;
	

	
 	/**
 	 * Get switchedPowerDifference, which is optional.
 	 * @return switchedPowerDifference as std::vector<float >
 	 * @throws IllegalAccessException If switchedPowerDifference does not exist.
 	 */
 	std::vector<float > getSwitchedPowerDifference() const;
	
 
 	
 	
 	/**
 	 * Set switchedPowerDifference with the specified std::vector<float >.
 	 * @param switchedPowerDifference The std::vector<float > value to which switchedPowerDifference is to be set.
 	 
 		
 	 */
 	void setSwitchedPowerDifference (std::vector<float > switchedPowerDifference);
		
	
	
	
	/**
	 * Mark switchedPowerDifference, which is an optional field, as non-existent.
	 */
	void clearSwitchedPowerDifference ();
	


	
	// ===> Attribute switchedPowerSum, which is optional
	
	
	
	/**
	 * The attribute switchedPowerSum is optional. Return true if this attribute exists.
	 * @return true if and only if the switchedPowerSum attribute exists. 
	 */
	bool isSwitchedPowerSumExists() const;
	

	
 	/**
 	 * Get switchedPowerSum, which is optional.
 	 * @return switchedPowerSum as std::vector<float >
 	 * @throws IllegalAccessException If switchedPowerSum does not exist.
 	 */
 	std::vector<float > getSwitchedPowerSum() const;
	
 
 	
 	
 	/**
 	 * Set switchedPowerSum with the specified std::vector<float >.
 	 * @param switchedPowerSum The std::vector<float > value to which switchedPowerSum is to be set.
 	 
 		
 	 */
 	void setSwitchedPowerSum (std::vector<float > switchedPowerSum);
		
	
	
	
	/**
	 * Mark switchedPowerSum, which is an optional field, as non-existent.
	 */
	void clearSwitchedPowerSum ();
	


	
	// ===> Attribute requantizerGain, which is optional
	
	
	
	/**
	 * The attribute requantizerGain is optional. Return true if this attribute exists.
	 * @return true if and only if the requantizerGain attribute exists. 
	 */
	bool isRequantizerGainExists() const;
	

	
 	/**
 	 * Get requantizerGain, which is optional.
 	 * @return requantizerGain as std::vector<float >
 	 * @throws IllegalAccessException If requantizerGain does not exist.
 	 */
 	std::vector<float > getRequantizerGain() const;
	
 
 	
 	
 	/**
 	 * Set requantizerGain with the specified std::vector<float >.
 	 * @param requantizerGain The std::vector<float > value to which requantizerGain is to be set.
 	 
 		
 	 */
 	void setRequantizerGain (std::vector<float > requantizerGain);
		
	
	
	
	/**
	 * Mark requantizerGain, which is an optional field, as non-existent.
	 */
	void clearRequantizerGain ();
	


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
  		
	
	
	


	
	// ===> Attribute feedId
	
	
	

	
 	/**
 	 * Get feedId.
 	 * @return feedId as int
 	 */
 	int getFeedId() const;
	
 
 	
 	
 	/**
 	 * Set feedId with the specified int.
 	 * @param feedId The int value to which feedId is to be set.
 	 
 		
 			
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 		
 	 */
 	void setFeedId (int feedId);
  		
	
	
	


	
	// ===> Attribute spectralWindowId
	
	
	

	
 	/**
 	 * Get spectralWindowId.
 	 * @return spectralWindowId as Tag
 	 */
 	Tag getSpectralWindowId() const;
	
 
 	
 	
 	/**
 	 * Set spectralWindowId with the specified Tag.
 	 * @param spectralWindowId The Tag value to which spectralWindowId is to be set.
 	 
 		
 			
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 		
 	 */
 	void setSpectralWindowId (Tag spectralWindowId);
  		
	
	
	


	///////////
	// Links //
	///////////
	
	

	
		
	/**
	 * antennaId pointer to the row in the Antenna table having Antenna.antennaId == antennaId
	 * @return a AntennaRow*
	 * 
	 
	 */
	 AntennaRow* getAntennaUsingAntennaId();
	 

	

	

	
		
	/**
	 * spectralWindowId pointer to the row in the SpectralWindow table having SpectralWindow.spectralWindowId == spectralWindowId
	 * @return a SpectralWindowRow*
	 * 
	 
	 */
	 SpectralWindowRow* getSpectralWindowUsingSpectralWindowId();
	 

	

	

	
		
	// ===> Slice link from a row of SysPower table to a collection of row of Feed table.
	
	/**
	 * Get the collection of row in the Feed table having feedId == this.feedId
	 * 
	 * @return a vector of FeedRow *
	 */
	std::vector <FeedRow *> getFeeds();
	
	

	

	
	
	
	/**
	 * Compare each mandatory attribute except the autoincrementable one of this SysPowerRow with 
	 * the corresponding parameters and return true if there is a match and false otherwise.
	 	
	 * @param antennaId
	    
	 * @param spectralWindowId
	    
	 * @param feedId
	    
	 * @param timeInterval
	    
	 * @param numReceptor
	    
	 */ 
	bool compareNoAutoInc(Tag antennaId, Tag spectralWindowId, int feedId, ArrayTimeInterval timeInterval, int numReceptor);
	
	

	
	/**
	 * Compare each mandatory value (i.e. not in the key) attribute  with 
	 * the corresponding parameters and return true if there is a match and false otherwise.
	 	
	 * @param numReceptor
	    
	 */ 
	bool compareRequiredValue(int numReceptor); 
		 
	
	/**
	 * Return true if all required attributes of the value part are equal to their homologues
	 * in x and false otherwise.
	 *
	 * @param x a pointer on the SysPowerRow whose required attributes of the value part 
	 * will be compared with those of this.
	 * @return a boolean.
	 */
	bool equalByRequiredValue(SysPowerRow* x) ;
	
#ifndef WITHOUT_ACS
	/**
	 * Return this row in the form of an IDL struct.
	 * @return The values of this row as a SysPowerRowIDL struct.
	 */
	asdmIDL::SysPowerRowIDL *toIDL() const;
	
	/**
	 * Define the content of a SysPowerRowIDL struct from the values
	 * found in this row.
	 *
	 * @param x a reference to the SysPowerRowIDL struct to be set.
	 *
	 */
	 void toIDL(asdmIDL::SysPowerRowIDL& x) const;
#endif
	
#ifndef WITHOUT_ACS
	/**
	 * Fill the values of this row from the IDL struct SysPowerRowIDL.
	 * @param x The IDL struct containing the values used to fill this row.
	 * @throws ConversionException
	 */
	void setFromIDL (asdmIDL::SysPowerRowIDL x) ;
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

	std::map<std::string, SysPowerAttributeFromBin> fromBinMethods;
void antennaIdFromBin( EndianIStream& eis);
void spectralWindowIdFromBin( EndianIStream& eis);
void feedIdFromBin( EndianIStream& eis);
void timeIntervalFromBin( EndianIStream& eis);
void numReceptorFromBin( EndianIStream& eis);

void switchedPowerDifferenceFromBin( EndianIStream& eis);
void switchedPowerSumFromBin( EndianIStream& eis);
void requantizerGainFromBin( EndianIStream& eis);


	 /**
	  * Deserialize a stream of bytes read from an EndianIStream to build a PointingRow.
	  * @param eiss the EndianIStream to be read.
	  * @param table the SysPowerTable to which the row built by deserialization will be parented.
	  * @param attributesSeq a vector containing the names of the attributes . The elements order defines the order 
	  * in which the attributes are written in the binary serialization.
	  */
	 static SysPowerRow* fromBin(EndianIStream& eis, SysPowerTable& table, const std::vector<std::string>& attributesSeq);	 
 
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
	SysPowerTable &table;
	/**
	 * Whether this row has been added to the table or not.
	 */
	bool hasBeenAdded;

	// This method is used by the Table class when this row is added to the table.
	void isAdded(bool added);


	/**
	 * Create a SysPowerRow.
	 * <p>
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 */ 
	SysPowerRow (SysPowerTable &table);

	/**
	 * Create a SysPowerRow using a copy constructor mechanism.
	 * <p>
	 * Given a SysPowerRow row and a SysPowerTable table, the method creates a new
	 * SysPowerRow owned by table. Each attribute of the created row is a copy (deep)
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
	 SysPowerRow (SysPowerTable &table, SysPowerRow *row);
	 	
	////////////////////////////////
	// Intrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute timeInterval
	
	

	ArrayTimeInterval timeInterval;

	
	
 	

	
	// ===> Attribute numReceptor
	
	

	int numReceptor;

	
	
 	

	
	// ===> Attribute switchedPowerDifference, which is optional
	
	
	bool switchedPowerDifferenceExists;
	

	std::vector<float > switchedPowerDifference;

	
	
 	

	
	// ===> Attribute switchedPowerSum, which is optional
	
	
	bool switchedPowerSumExists;
	

	std::vector<float > switchedPowerSum;

	
	
 	

	
	// ===> Attribute requantizerGain, which is optional
	
	
	bool requantizerGainExists;
	

	std::vector<float > requantizerGain;

	
	
 	

	////////////////////////////////
	// Extrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute antennaId
	
	

	Tag antennaId;

	
	
 	

	
	// ===> Attribute feedId
	
	

	int feedId;

	
	
 	

	
	// ===> Attribute spectralWindowId
	
	

	Tag spectralWindowId;

	
	
 	

	///////////
	// Links //
	///////////
	
	
		

	 

	

	
		

	 

	

	
		


	

	
/*
	////////////////////////////////////////////////////////////
	// binary-deserialization material from an EndianIStream  //
	////////////////////////////////////////////////////////////
	std::map<std::string, SysPowerAttributeFromBin> fromBinMethods;
void antennaIdFromBin( EndianIStream& eis);
void spectralWindowIdFromBin( EndianIStream& eis);
void feedIdFromBin( EndianIStream& eis);
void timeIntervalFromBin( EndianIStream& eis);
void numReceptorFromBin( EndianIStream& eis);

void switchedPowerDifferenceFromBin( EndianIStream& eis);
void switchedPowerSumFromBin( EndianIStream& eis);
void requantizerGainFromBin( EndianIStream& eis);

*/
	
	///////////////////////////////////
	// text-deserialization material //
	///////////////////////////////////
	std::map<std::string, SysPowerAttributeFromText> fromTextMethods;
	
void antennaIdFromText (const std::string & s);
	
	
void spectralWindowIdFromText (const std::string & s);
	
	
void feedIdFromText (const std::string & s);
	
	
void timeIntervalFromText (const std::string & s);
	
	
void numReceptorFromText (const std::string & s);
	

	
void switchedPowerDifferenceFromText (const std::string & s);
	
	
void switchedPowerSumFromText (const std::string & s);
	
	
void requantizerGainFromText (const std::string & s);
	
	
	
	/**
	 * Serialize this into a stream of bytes written to an EndianOSStream.
	 * @param eoss the EndianOSStream to be written to
	 */
	 void toBin(EndianOSStream& eoss);
	 	 
	 /**
	  * Deserialize a stream of bytes read from an EndianIStream to build a PointingRow.
	  * @param eiss the EndianIStream to be read.
	  * @param table the SysPowerTable to which the row built by deserialization will be parented.
	  * @param attributesSeq a vector containing the names of the attributes . The elements order defines the order 
	  * in which the attributes are written in the binary serialization.

	 static SysPowerRow* fromBin(EndianIStream& eis, SysPowerTable& table, const std::vector<std::string>& attributesSeq);	 
		*/
};

} // End namespace asdm

#endif /* SysPower_CLASS */
