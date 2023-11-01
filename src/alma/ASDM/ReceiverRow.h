
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
 * File ReceiverRow.h
 */
 
#ifndef ReceiverRow_CLASS
#define ReceiverRow_CLASS

#include <vector>
#include <string>
#include <set>

#ifndef WITHOUT_ACS
#include <asdmIDLC.h>
#endif






	 
#include <alma/ASDM/ArrayTimeInterval.h>
	

	 
#include <alma/ASDM/Frequency.h>
	

	 
#include <alma/ASDM/Tag.h>
	




	

	

	

	

	
#include <alma/Enumerations/CReceiverBand.h>
	

	

	
#include <alma/Enumerations/CReceiverSideband.h>
	

	
#include <alma/Enumerations/CNetSideband.h>
	



#include <alma/ASDM/ConversionException.h>
#include <alma/ASDM/NoSuchRow.h>
#include <alma/ASDM/IllegalAccessException.h>

#include <alma/ASDM/RowTransformer.h>
//#include <alma/ASDM/TableStreamReader.h>

/*\file Receiver.h
    \brief Generated from model's revision "-1", branch ""
*/

namespace asdm {

//class asdm::ReceiverTable;


// class asdm::SpectralWindowRow;
class SpectralWindowRow;
	

class ReceiverRow;
typedef void (ReceiverRow::*ReceiverAttributeFromBin) (EndianIStream& eis);
typedef void (ReceiverRow::*ReceiverAttributeFromText) (const std::string& s);

/**
 * The ReceiverRow class is a row of a ReceiverTable.
 * 
 * Generated from model's revision "-1", branch ""
 *
 */
class ReceiverRow {
friend class asdm::ReceiverTable;
friend class asdm::RowTransformer<ReceiverRow>;
//friend class asdm::TableStreamReader<ReceiverTable, ReceiverRow>;

public:

	virtual ~ReceiverRow();

	/**
	 * Return the table to which this row belongs.
	 */
	ReceiverTable &getTable() const;
	
	/**
	 * Has this row been added to its table ?
	 * @return true if and only if it has been added.
	 */
	bool isAdded() const;
		
	////////////////////////////////
	// Intrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute receiverId
	
	
	

	
 	/**
 	 * Get receiverId.
 	 * @return receiverId as int
 	 */
 	int getReceiverId() const;
	
 
 	
 	
	
	


	
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
  		
	
	
	


	
	// ===> Attribute name
	
	
	

	
 	/**
 	 * Get name.
 	 * @return name as std::string
 	 */
 	std::string getName() const;
	
 
 	
 	
 	/**
 	 * Set name with the specified std::string.
 	 * @param name The std::string value to which name is to be set.
 	 
 		
 			
 	 */
 	void setName (std::string name);
  		
	
	
	


	
	// ===> Attribute numLO
	
	
	

	
 	/**
 	 * Get numLO.
 	 * @return numLO as int
 	 */
 	int getNumLO() const;
	
 
 	
 	
 	/**
 	 * Set numLO with the specified int.
 	 * @param numLO The int value to which numLO is to be set.
 	 
 		
 			
 	 */
 	void setNumLO (int numLO);
  		
	
	
	


	
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
  		
	
	
	


	
	// ===> Attribute freqLO
	
	
	

	
 	/**
 	 * Get freqLO.
 	 * @return freqLO as std::vector<Frequency >
 	 */
 	std::vector<Frequency > getFreqLO() const;
	
 
 	
 	
 	/**
 	 * Set freqLO with the specified std::vector<Frequency >.
 	 * @param freqLO The std::vector<Frequency > value to which freqLO is to be set.
 	 
 		
 			
 	 */
 	void setFreqLO (std::vector<Frequency > freqLO);
  		
	
	
	


	
	// ===> Attribute receiverSideband
	
	
	

	
 	/**
 	 * Get receiverSideband.
 	 * @return receiverSideband as ReceiverSidebandMod::ReceiverSideband
 	 */
 	ReceiverSidebandMod::ReceiverSideband getReceiverSideband() const;
	
 
 	
 	
 	/**
 	 * Set receiverSideband with the specified ReceiverSidebandMod::ReceiverSideband.
 	 * @param receiverSideband The ReceiverSidebandMod::ReceiverSideband value to which receiverSideband is to be set.
 	 
 		
 			
 	 */
 	void setReceiverSideband (ReceiverSidebandMod::ReceiverSideband receiverSideband);
  		
	
	
	


	
	// ===> Attribute sidebandLO
	
	
	

	
 	/**
 	 * Get sidebandLO.
 	 * @return sidebandLO as std::vector<NetSidebandMod::NetSideband >
 	 */
 	std::vector<NetSidebandMod::NetSideband > getSidebandLO() const;
	
 
 	
 	
 	/**
 	 * Set sidebandLO with the specified std::vector<NetSidebandMod::NetSideband >.
 	 * @param sidebandLO The std::vector<NetSidebandMod::NetSideband > value to which sidebandLO is to be set.
 	 
 		
 			
 	 */
 	void setSidebandLO (std::vector<NetSidebandMod::NetSideband > sidebandLO);
  		
	
	
	


	////////////////////////////////
	// Extrinsic Table Attributes //
	////////////////////////////////
	
	
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
	 * spectralWindowId pointer to the row in the SpectralWindow table having SpectralWindow.spectralWindowId == spectralWindowId
	 * @return a SpectralWindowRow*
	 * 
	 
	 */
	 SpectralWindowRow* getSpectralWindowUsingSpectralWindowId();
	 

	

	
	
	
	/**
	 * Compare each mandatory attribute except the autoincrementable one of this ReceiverRow with 
	 * the corresponding parameters and return true if there is a match and false otherwise.
	 	
	 * @param spectralWindowId
	    
	 * @param timeInterval
	    
	 * @param name
	    
	 * @param numLO
	    
	 * @param frequencyBand
	    
	 * @param freqLO
	    
	 * @param receiverSideband
	    
	 * @param sidebandLO
	    
	 */ 
	bool compareNoAutoInc(Tag spectralWindowId, ArrayTimeInterval timeInterval, std::string name, int numLO, ReceiverBandMod::ReceiverBand frequencyBand, std::vector<Frequency > freqLO, ReceiverSidebandMod::ReceiverSideband receiverSideband, std::vector<NetSidebandMod::NetSideband > sidebandLO);
	
	

	
	/**
	 * Compare each mandatory value (i.e. not in the key) attribute  with 
	 * the corresponding parameters and return true if there is a match and false otherwise.
	 	
	 * @param name
	    
	 * @param numLO
	    
	 * @param frequencyBand
	    
	 * @param freqLO
	    
	 * @param receiverSideband
	    
	 * @param sidebandLO
	    
	 */ 
	bool compareRequiredValue(std::string name, int numLO, ReceiverBandMod::ReceiverBand frequencyBand, std::vector<Frequency > freqLO, ReceiverSidebandMod::ReceiverSideband receiverSideband, std::vector<NetSidebandMod::NetSideband > sidebandLO); 
		 
	
	/**
	 * Return true if all required attributes of the value part are equal to their homologues
	 * in x and false otherwise.
	 *
	 * @param x a pointer on the ReceiverRow whose required attributes of the value part 
	 * will be compared with those of this.
	 * @return a boolean.
	 */
	bool equalByRequiredValue(ReceiverRow* x) ;
	
#ifndef WITHOUT_ACS
	/**
	 * Return this row in the form of an IDL struct.
	 * @return The values of this row as a ReceiverRowIDL struct.
	 */
	asdmIDL::ReceiverRowIDL *toIDL() const;
	
	/**
	 * Define the content of a ReceiverRowIDL struct from the values
	 * found in this row.
	 *
	 * @param x a reference to the ReceiverRowIDL struct to be set.
	 *
	 */
	 void toIDL(asdmIDL::ReceiverRowIDL& x) const;
#endif
	
#ifndef WITHOUT_ACS
	/**
	 * Fill the values of this row from the IDL struct ReceiverRowIDL.
	 * @param x The IDL struct containing the values used to fill this row.
	 * @throws ConversionException
	 */
	void setFromIDL (asdmIDL::ReceiverRowIDL x) ;
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

	std::map<std::string, ReceiverAttributeFromBin> fromBinMethods;
void receiverIdFromBin( EndianIStream& eis);
void spectralWindowIdFromBin( EndianIStream& eis);
void timeIntervalFromBin( EndianIStream& eis);
void nameFromBin( EndianIStream& eis);
void numLOFromBin( EndianIStream& eis);
void frequencyBandFromBin( EndianIStream& eis);
void freqLOFromBin( EndianIStream& eis);
void receiverSidebandFromBin( EndianIStream& eis);
void sidebandLOFromBin( EndianIStream& eis);

	

	 /**
	  * Deserialize a stream of bytes read from an EndianIStream to build a PointingRow.
	  * @param eiss the EndianIStream to be read.
	  * @param table the ReceiverTable to which the row built by deserialization will be parented.
	  * @param attributesSeq a vector containing the names of the attributes . The elements order defines the order 
	  * in which the attributes are written in the binary serialization.
	  */
	 static ReceiverRow* fromBin(EndianIStream& eis, ReceiverTable& table, const std::vector<std::string>& attributesSeq);	 
 
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
	ReceiverTable &table;
	/**
	 * Whether this row has been added to the table or not.
	 */
	bool hasBeenAdded;

	// This method is used by the Table class when this row is added to the table.
	void isAdded(bool added);


	/**
	 * Create a ReceiverRow.
	 * <p>
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 */ 
	ReceiverRow (ReceiverTable &table);

	/**
	 * Create a ReceiverRow using a copy constructor mechanism.
	 * <p>
	 * Given a ReceiverRow row and a ReceiverTable table, the method creates a new
	 * ReceiverRow owned by table. Each attribute of the created row is a copy (deep)
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
	 ReceiverRow (ReceiverTable &table, ReceiverRow *row);
	 	
	////////////////////////////////
	// Intrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute receiverId
	
	

	int receiverId;

	
	
 	
 	/**
 	 * Set receiverId with the specified int value.
 	 * @param receiverId The int value to which receiverId is to be set.
		
 		
			
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 		
 	 */
 	void setReceiverId (int receiverId);
  		
	

	
	// ===> Attribute timeInterval
	
	

	ArrayTimeInterval timeInterval;

	
	
 	

	
	// ===> Attribute name
	
	

	std::string name;

	
	
 	

	
	// ===> Attribute numLO
	
	

	int numLO;

	
	
 	

	
	// ===> Attribute frequencyBand
	
	

	ReceiverBandMod::ReceiverBand frequencyBand;

	
	
 	

	
	// ===> Attribute freqLO
	
	

	std::vector<Frequency > freqLO;

	
	
 	

	
	// ===> Attribute receiverSideband
	
	

	ReceiverSidebandMod::ReceiverSideband receiverSideband;

	
	
 	

	
	// ===> Attribute sidebandLO
	
	

	std::vector<NetSidebandMod::NetSideband > sidebandLO;

	
	
 	

	////////////////////////////////
	// Extrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute spectralWindowId
	
	

	Tag spectralWindowId;

	
	
 	

	///////////
	// Links //
	///////////
	
	
		

	 

	

	
/*
	////////////////////////////////////////////////////////////
	// binary-deserialization material from an EndianIStream  //
	////////////////////////////////////////////////////////////
	std::map<std::string, ReceiverAttributeFromBin> fromBinMethods;
void receiverIdFromBin( EndianIStream& eis);
void spectralWindowIdFromBin( EndianIStream& eis);
void timeIntervalFromBin( EndianIStream& eis);
void nameFromBin( EndianIStream& eis);
void numLOFromBin( EndianIStream& eis);
void frequencyBandFromBin( EndianIStream& eis);
void freqLOFromBin( EndianIStream& eis);
void receiverSidebandFromBin( EndianIStream& eis);
void sidebandLOFromBin( EndianIStream& eis);

	
*/
	
	///////////////////////////////////
	// text-deserialization material //
	///////////////////////////////////
	std::map<std::string, ReceiverAttributeFromText> fromTextMethods;
	
void receiverIdFromText (const std::string & s);
	
	
void spectralWindowIdFromText (const std::string & s);
	
	
void timeIntervalFromText (const std::string & s);
	
	
void nameFromText (const std::string & s);
	
	
void numLOFromText (const std::string & s);
	
	
void frequencyBandFromText (const std::string & s);
	
	
void freqLOFromText (const std::string & s);
	
	
void receiverSidebandFromText (const std::string & s);
	
	
void sidebandLOFromText (const std::string & s);
	

		
	
	/**
	 * Serialize this into a stream of bytes written to an EndianOSStream.
	 * @param eoss the EndianOSStream to be written to
	 */
	 void toBin(EndianOSStream& eoss);
	 	 
	 /**
	  * Deserialize a stream of bytes read from an EndianIStream to build a PointingRow.
	  * @param eiss the EndianIStream to be read.
	  * @param table the ReceiverTable to which the row built by deserialization will be parented.
	  * @param attributesSeq a vector containing the names of the attributes . The elements order defines the order 
	  * in which the attributes are written in the binary serialization.

	 static ReceiverRow* fromBin(EndianIStream& eis, ReceiverTable& table, const std::vector<std::string>& attributesSeq);	 
		*/
};

} // End namespace asdm

#endif /* Receiver_CLASS */
