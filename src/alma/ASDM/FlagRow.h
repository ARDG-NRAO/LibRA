
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
 * File FlagRow.h
 */
 
#ifndef FlagRow_CLASS
#define FlagRow_CLASS

#include <vector>
#include <string>
#include <set>

#ifndef WITHOUT_ACS
#include <asdmIDLC.h>
#endif






	 
#include <alma/ASDM/ArrayTime.h>
	

	 
#include <alma/ASDM/Tag.h>
	




	

	

	

	

	

	

	

	

	

	
#include <alma/Enumerations/CPolarizationType.h>
	

	



#include <alma/ASDM/ConversionException.h>
#include <alma/ASDM/NoSuchRow.h>
#include <alma/ASDM/IllegalAccessException.h>

#include <alma/ASDM/RowTransformer.h>
//#include <alma/ASDM/TableStreamReader.h>

/*\file Flag.h
    \brief Generated from model's revision "-1", branch ""
*/

namespace asdm {

//class asdm::FlagTable;


// class asdm::AntennaRow;
class AntennaRow;

// class asdm::AntennaRow;
class AntennaRow;

// class asdm::SpectralWindowRow;
class SpectralWindowRow;
	

class FlagRow;
typedef void (FlagRow::*FlagAttributeFromBin) (EndianIStream& eis);
typedef void (FlagRow::*FlagAttributeFromText) (const std::string& s);

/**
 * The FlagRow class is a row of a FlagTable.
 * 
 * Generated from model's revision "-1", branch ""
 *
 */
class FlagRow {
friend class asdm::FlagTable;
friend class asdm::RowTransformer<FlagRow>;
//friend class asdm::TableStreamReader<FlagTable, FlagRow>;

public:

	virtual ~FlagRow();

	/**
	 * Return the table to which this row belongs.
	 */
	FlagTable &getTable() const;
	
	/**
	 * Has this row been added to its table ?
	 * @return true if and only if it has been added.
	 */
	bool isAdded() const;
		
	////////////////////////////////
	// Intrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute flagId
	
	
	

	
 	/**
 	 * Get flagId.
 	 * @return flagId as Tag
 	 */
 	Tag getFlagId() const;
	
 
 	
 	
	
	


	
	// ===> Attribute startTime
	
	
	

	
 	/**
 	 * Get startTime.
 	 * @return startTime as ArrayTime
 	 */
 	ArrayTime getStartTime() const;
	
 
 	
 	
 	/**
 	 * Set startTime with the specified ArrayTime.
 	 * @param startTime The ArrayTime value to which startTime is to be set.
 	 
 		
 			
 	 */
 	void setStartTime (ArrayTime startTime);
  		
	
	
	


	
	// ===> Attribute endTime
	
	
	

	
 	/**
 	 * Get endTime.
 	 * @return endTime as ArrayTime
 	 */
 	ArrayTime getEndTime() const;
	
 
 	
 	
 	/**
 	 * Set endTime with the specified ArrayTime.
 	 * @param endTime The ArrayTime value to which endTime is to be set.
 	 
 		
 			
 	 */
 	void setEndTime (ArrayTime endTime);
  		
	
	
	


	
	// ===> Attribute reason
	
	
	

	
 	/**
 	 * Get reason.
 	 * @return reason as std::string
 	 */
 	std::string getReason() const;
	
 
 	
 	
 	/**
 	 * Set reason with the specified std::string.
 	 * @param reason The std::string value to which reason is to be set.
 	 
 		
 			
 	 */
 	void setReason (std::string reason);
  		
	
	
	


	
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
  		
	
	
	


	
	// ===> Attribute numPolarizationType, which is optional
	
	
	
	/**
	 * The attribute numPolarizationType is optional. Return true if this attribute exists.
	 * @return true if and only if the numPolarizationType attribute exists. 
	 */
	bool isNumPolarizationTypeExists() const;
	

	
 	/**
 	 * Get numPolarizationType, which is optional.
 	 * @return numPolarizationType as int
 	 * @throws IllegalAccessException If numPolarizationType does not exist.
 	 */
 	int getNumPolarizationType() const;
	
 
 	
 	
 	/**
 	 * Set numPolarizationType with the specified int.
 	 * @param numPolarizationType The int value to which numPolarizationType is to be set.
 	 
 		
 	 */
 	void setNumPolarizationType (int numPolarizationType);
		
	
	
	
	/**
	 * Mark numPolarizationType, which is an optional field, as non-existent.
	 */
	void clearNumPolarizationType ();
	


	
	// ===> Attribute numSpectralWindow, which is optional
	
	
	
	/**
	 * The attribute numSpectralWindow is optional. Return true if this attribute exists.
	 * @return true if and only if the numSpectralWindow attribute exists. 
	 */
	bool isNumSpectralWindowExists() const;
	

	
 	/**
 	 * Get numSpectralWindow, which is optional.
 	 * @return numSpectralWindow as int
 	 * @throws IllegalAccessException If numSpectralWindow does not exist.
 	 */
 	int getNumSpectralWindow() const;
	
 
 	
 	
 	/**
 	 * Set numSpectralWindow with the specified int.
 	 * @param numSpectralWindow The int value to which numSpectralWindow is to be set.
 	 
 		
 	 */
 	void setNumSpectralWindow (int numSpectralWindow);
		
	
	
	
	/**
	 * Mark numSpectralWindow, which is an optional field, as non-existent.
	 */
	void clearNumSpectralWindow ();
	


	
	// ===> Attribute numPairedAntenna, which is optional
	
	
	
	/**
	 * The attribute numPairedAntenna is optional. Return true if this attribute exists.
	 * @return true if and only if the numPairedAntenna attribute exists. 
	 */
	bool isNumPairedAntennaExists() const;
	

	
 	/**
 	 * Get numPairedAntenna, which is optional.
 	 * @return numPairedAntenna as int
 	 * @throws IllegalAccessException If numPairedAntenna does not exist.
 	 */
 	int getNumPairedAntenna() const;
	
 
 	
 	
 	/**
 	 * Set numPairedAntenna with the specified int.
 	 * @param numPairedAntenna The int value to which numPairedAntenna is to be set.
 	 
 		
 	 */
 	void setNumPairedAntenna (int numPairedAntenna);
		
	
	
	
	/**
	 * Mark numPairedAntenna, which is an optional field, as non-existent.
	 */
	void clearNumPairedAntenna ();
	


	
	// ===> Attribute numChan, which is optional
	
	
	
	/**
	 * The attribute numChan is optional. Return true if this attribute exists.
	 * @return true if and only if the numChan attribute exists. 
	 */
	bool isNumChanExists() const;
	

	
 	/**
 	 * Get numChan, which is optional.
 	 * @return numChan as int
 	 * @throws IllegalAccessException If numChan does not exist.
 	 */
 	int getNumChan() const;
	
 
 	
 	
 	/**
 	 * Set numChan with the specified int.
 	 * @param numChan The int value to which numChan is to be set.
 	 
 		
 	 */
 	void setNumChan (int numChan);
		
	
	
	
	/**
	 * Mark numChan, which is an optional field, as non-existent.
	 */
	void clearNumChan ();
	


	
	// ===> Attribute polarizationType, which is optional
	
	
	
	/**
	 * The attribute polarizationType is optional. Return true if this attribute exists.
	 * @return true if and only if the polarizationType attribute exists. 
	 */
	bool isPolarizationTypeExists() const;
	

	
 	/**
 	 * Get polarizationType, which is optional.
 	 * @return polarizationType as std::vector<PolarizationTypeMod::PolarizationType >
 	 * @throws IllegalAccessException If polarizationType does not exist.
 	 */
 	std::vector<PolarizationTypeMod::PolarizationType > getPolarizationType() const;
	
 
 	
 	
 	/**
 	 * Set polarizationType with the specified std::vector<PolarizationTypeMod::PolarizationType >.
 	 * @param polarizationType The std::vector<PolarizationTypeMod::PolarizationType > value to which polarizationType is to be set.
 	 
 		
 	 */
 	void setPolarizationType (std::vector<PolarizationTypeMod::PolarizationType > polarizationType);
		
	
	
	
	/**
	 * Mark polarizationType, which is an optional field, as non-existent.
	 */
	void clearPolarizationType ();
	


	
	// ===> Attribute channel, which is optional
	
	
	
	/**
	 * The attribute channel is optional. Return true if this attribute exists.
	 * @return true if and only if the channel attribute exists. 
	 */
	bool isChannelExists() const;
	

	
 	/**
 	 * Get channel, which is optional.
 	 * @return channel as std::vector<std::vector<int > >
 	 * @throws IllegalAccessException If channel does not exist.
 	 */
 	std::vector<std::vector<int > > getChannel() const;
	
 
 	
 	
 	/**
 	 * Set channel with the specified std::vector<std::vector<int > >.
 	 * @param channel The std::vector<std::vector<int > > value to which channel is to be set.
 	 
 		
 	 */
 	void setChannel (std::vector<std::vector<int > > channel);
		
	
	
	
	/**
	 * Mark channel, which is an optional field, as non-existent.
	 */
	void clearChannel ();
	


	////////////////////////////////
	// Extrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute antennaId
	
	
	

	
 	/**
 	 * Get antennaId.
 	 * @return antennaId as std::vector<Tag> 
 	 */
 	std::vector<Tag>  getAntennaId() const;
	
 
 	
 	
 	/**
 	 * Set antennaId with the specified std::vector<Tag> .
 	 * @param antennaId The std::vector<Tag>  value to which antennaId is to be set.
 	 
 		
 			
 	 */
 	void setAntennaId (std::vector<Tag>  antennaId);
  		
	
	
	


	
	// ===> Attribute pairedAntennaId, which is optional
	
	
	
	/**
	 * The attribute pairedAntennaId is optional. Return true if this attribute exists.
	 * @return true if and only if the pairedAntennaId attribute exists. 
	 */
	bool isPairedAntennaIdExists() const;
	

	
 	/**
 	 * Get pairedAntennaId, which is optional.
 	 * @return pairedAntennaId as std::vector<Tag> 
 	 * @throws IllegalAccessException If pairedAntennaId does not exist.
 	 */
 	std::vector<Tag>  getPairedAntennaId() const;
	
 
 	
 	
 	/**
 	 * Set pairedAntennaId with the specified std::vector<Tag> .
 	 * @param pairedAntennaId The std::vector<Tag>  value to which pairedAntennaId is to be set.
 	 
 		
 	 */
 	void setPairedAntennaId (std::vector<Tag>  pairedAntennaId);
		
	
	
	
	/**
	 * Mark pairedAntennaId, which is an optional field, as non-existent.
	 */
	void clearPairedAntennaId ();
	


	
	// ===> Attribute spectralWindowId, which is optional
	
	
	
	/**
	 * The attribute spectralWindowId is optional. Return true if this attribute exists.
	 * @return true if and only if the spectralWindowId attribute exists. 
	 */
	bool isSpectralWindowIdExists() const;
	

	
 	/**
 	 * Get spectralWindowId, which is optional.
 	 * @return spectralWindowId as std::vector<Tag> 
 	 * @throws IllegalAccessException If spectralWindowId does not exist.
 	 */
 	std::vector<Tag>  getSpectralWindowId() const;
	
 
 	
 	
 	/**
 	 * Set spectralWindowId with the specified std::vector<Tag> .
 	 * @param spectralWindowId The std::vector<Tag>  value to which spectralWindowId is to be set.
 	 
 		
 	 */
 	void setSpectralWindowId (std::vector<Tag>  spectralWindowId);
		
	
	
	
	/**
	 * Mark spectralWindowId, which is an optional field, as non-existent.
	 */
	void clearSpectralWindowId ();
	


	///////////
	// Links //
	///////////
	
	
 		
 	/**
 	 * Set antennaId[i] with the specified Tag.
 	 * @param i The index in antennaId where to set the Tag value.
 	 * @param antennaId The Tag value to which antennaId[i] is to be set. 
	 		
 	 * @throws IndexOutOfBoundsException
  	 */
  	void setAntennaId (int i, Tag antennaId); 
 			
	

	
		 
/**
 * Append a Tag to antennaId.
 * @param id the Tag to be appended to antennaId
 */
 void addAntennaId(Tag id); 

/**
 * Append a vector of Tag to antennaId.
 * @param id an array of Tag to be appended to antennaId
 */
 void addAntennaId(const std::vector<Tag> & id); 
 

 /**
  * Returns the Tag stored in antennaId at position i.
  * @param i the position in antennaId where the Tag is retrieved.
  * @return the Tag stored at position i in antennaId.
  */
 const Tag getAntennaId(int i);
 
 /**
  * Returns the AntennaRow linked to this row via the tag stored in antennaId
  * at position i.
  * @param i the position in antennaId.
  * @return a pointer on a AntennaRow whose key (a Tag) is equal to the Tag stored at position
  * i in the antennaId. 
  */
 AntennaRow* getAntennaUsingAntennaId(int i); 
 
 /**
  * Returns the vector of AntennaRow* linked to this row via the Tags stored in antennaId
  * @return an array of pointers on AntennaRow.
  */
 std::vector<AntennaRow *> getAntennasUsingAntennaId(); 
  

	

	
 		
 	/**
 	 * Set pairedAntennaId[i] with the specified Tag.
 	 * @param i The index in pairedAntennaId where to set the Tag value.
 	 * @param pairedAntennaId The Tag value to which pairedAntennaId[i] is to be set. 
 	 * @throws OutOfBoundsException
  	 */
  	void setPairedAntennaId (int i, Tag pairedAntennaId)  ;
 			
	

	
		 
/**
 * Append a Tag to pairedAntennaId.
 * @param id the Tag to be appended to pairedAntennaId
 */
 void addPairedAntennaId(Tag id); 

/**
 * Append a vector of Tag to pairedAntennaId.
 * @param id an array of Tag to be appended to pairedAntennaId
 */
 void addPairedAntennaId(const std::vector<Tag> & id); 
 

 /**
  * Returns the Tag stored in pairedAntennaId at position i.
  * @param i the position in pairedAntennaId where the Tag is retrieved.
  * @return the Tag stored at position i in pairedAntennaId.
  */
 const Tag getPairedAntennaId(int i);
 
 /**
  * Returns the AntennaRow linked to this row via the tag stored in pairedAntennaId
  * at position i.
  * @param i the position in pairedAntennaId.
  * @return a pointer on a AntennaRow whose key (a Tag) is equal to the Tag stored at position
  * i in the pairedAntennaId. 
  */
 AntennaRow* getAntennaUsingPairedAntennaId(int i); 
 
 /**
  * Returns the vector of AntennaRow* linked to this row via the Tags stored in pairedAntennaId
  * @return an array of pointers on AntennaRow.
  */
 std::vector<AntennaRow *> getAntennasUsingPairedAntennaId(); 
  

	

	
 		
 	/**
 	 * Set spectralWindowId[i] with the specified Tag.
 	 * @param i The index in spectralWindowId where to set the Tag value.
 	 * @param spectralWindowId The Tag value to which spectralWindowId[i] is to be set. 
 	 * @throws OutOfBoundsException
  	 */
  	void setSpectralWindowId (int i, Tag spectralWindowId)  ;
 			
	

	
		 
/**
 * Append a Tag to spectralWindowId.
 * @param id the Tag to be appended to spectralWindowId
 */
 void addSpectralWindowId(Tag id); 

/**
 * Append a vector of Tag to spectralWindowId.
 * @param id an array of Tag to be appended to spectralWindowId
 */
 void addSpectralWindowId(const std::vector<Tag> & id); 
 

 /**
  * Returns the Tag stored in spectralWindowId at position i.
  * @param i the position in spectralWindowId where the Tag is retrieved.
  * @return the Tag stored at position i in spectralWindowId.
  */
 const Tag getSpectralWindowId(int i);
 
 /**
  * Returns the SpectralWindowRow linked to this row via the tag stored in spectralWindowId
  * at position i.
  * @param i the position in spectralWindowId.
  * @return a pointer on a SpectralWindowRow whose key (a Tag) is equal to the Tag stored at position
  * i in the spectralWindowId. 
  */
 SpectralWindowRow* getSpectralWindowUsingSpectralWindowId(int i); 
 
 /**
  * Returns the vector of SpectralWindowRow* linked to this row via the Tags stored in spectralWindowId
  * @return an array of pointers on SpectralWindowRow.
  */
 std::vector<SpectralWindowRow *> getSpectralWindowsUsingSpectralWindowId(); 
  

	

	
	
	
	/**
	 * Compare each mandatory attribute except the autoincrementable one of this FlagRow with 
	 * the corresponding parameters and return true if there is a match and false otherwise.
	 	
	 * @param startTime
	    
	 * @param endTime
	    
	 * @param reason
	    
	 * @param numAntenna
	    
	 * @param antennaId
	    
	 */ 
	bool compareNoAutoInc(ArrayTime startTime, ArrayTime endTime, std::string reason, int numAntenna, std::vector<Tag>  antennaId);
	
	

	
	/**
	 * Compare each mandatory value (i.e. not in the key) attribute  with 
	 * the corresponding parameters and return true if there is a match and false otherwise.
	 	
	 * @param startTime
	    
	 * @param endTime
	    
	 * @param reason
	    
	 * @param numAntenna
	    
	 * @param antennaId
	    
	 */ 
	bool compareRequiredValue(ArrayTime startTime, ArrayTime endTime, std::string reason, int numAntenna, std::vector<Tag>  antennaId); 
		 
	
	/**
	 * Return true if all required attributes of the value part are equal to their homologues
	 * in x and false otherwise.
	 *
	 * @param x a pointer on the FlagRow whose required attributes of the value part 
	 * will be compared with those of this.
	 * @return a boolean.
	 */
	bool equalByRequiredValue(FlagRow* x) ;
	
#ifndef WITHOUT_ACS
	/**
	 * Return this row in the form of an IDL struct.
	 * @return The values of this row as a FlagRowIDL struct.
	 */
	asdmIDL::FlagRowIDL *toIDL() const;
	
	/**
	 * Define the content of a FlagRowIDL struct from the values
	 * found in this row.
	 *
	 * @param x a reference to the FlagRowIDL struct to be set.
	 *
	 */
	 void toIDL(asdmIDL::FlagRowIDL& x) const;
#endif
	
#ifndef WITHOUT_ACS
	/**
	 * Fill the values of this row from the IDL struct FlagRowIDL.
	 * @param x The IDL struct containing the values used to fill this row.
	 * @throws ConversionException
	 */
	void setFromIDL (asdmIDL::FlagRowIDL x) ;
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

	std::map<std::string, FlagAttributeFromBin> fromBinMethods;
void flagIdFromBin( EndianIStream& eis);
void startTimeFromBin( EndianIStream& eis);
void endTimeFromBin( EndianIStream& eis);
void reasonFromBin( EndianIStream& eis);
void numAntennaFromBin( EndianIStream& eis);
void antennaIdFromBin( EndianIStream& eis);

void numPolarizationTypeFromBin( EndianIStream& eis);
void numSpectralWindowFromBin( EndianIStream& eis);
void numPairedAntennaFromBin( EndianIStream& eis);
void numChanFromBin( EndianIStream& eis);
void polarizationTypeFromBin( EndianIStream& eis);
void channelFromBin( EndianIStream& eis);
void pairedAntennaIdFromBin( EndianIStream& eis);
void spectralWindowIdFromBin( EndianIStream& eis);


	 /**
	  * Deserialize a stream of bytes read from an EndianIStream to build a PointingRow.
	  * @param eiss the EndianIStream to be read.
	  * @param table the FlagTable to which the row built by deserialization will be parented.
	  * @param attributesSeq a vector containing the names of the attributes . The elements order defines the order 
	  * in which the attributes are written in the binary serialization.
	  */
	 static FlagRow* fromBin(EndianIStream& eis, FlagTable& table, const std::vector<std::string>& attributesSeq);	 
 
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
	FlagTable &table;
	/**
	 * Whether this row has been added to the table or not.
	 */
	bool hasBeenAdded;

	// This method is used by the Table class when this row is added to the table.
	void isAdded(bool added);


	/**
	 * Create a FlagRow.
	 * <p>
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 */ 
	FlagRow (FlagTable &table);

	/**
	 * Create a FlagRow using a copy constructor mechanism.
	 * <p>
	 * Given a FlagRow row and a FlagTable table, the method creates a new
	 * FlagRow owned by table. Each attribute of the created row is a copy (deep)
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
	 FlagRow (FlagTable &table, FlagRow *row);
	 	
	////////////////////////////////
	// Intrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute flagId
	
	

	Tag flagId;

	
	
 	
 	/**
 	 * Set flagId with the specified Tag value.
 	 * @param flagId The Tag value to which flagId is to be set.
		
 		
			
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 		
 	 */
 	void setFlagId (Tag flagId);
  		
	

	
	// ===> Attribute startTime
	
	

	ArrayTime startTime;

	
	
 	

	
	// ===> Attribute endTime
	
	

	ArrayTime endTime;

	
	
 	

	
	// ===> Attribute reason
	
	

	std::string reason;

	
	
 	

	
	// ===> Attribute numAntenna
	
	

	int numAntenna;

	
	
 	

	
	// ===> Attribute numPolarizationType, which is optional
	
	
	bool numPolarizationTypeExists;
	

	int numPolarizationType;

	
	
 	

	
	// ===> Attribute numSpectralWindow, which is optional
	
	
	bool numSpectralWindowExists;
	

	int numSpectralWindow;

	
	
 	

	
	// ===> Attribute numPairedAntenna, which is optional
	
	
	bool numPairedAntennaExists;
	

	int numPairedAntenna;

	
	
 	

	
	// ===> Attribute numChan, which is optional
	
	
	bool numChanExists;
	

	int numChan;

	
	
 	

	
	// ===> Attribute polarizationType, which is optional
	
	
	bool polarizationTypeExists;
	

	std::vector<PolarizationTypeMod::PolarizationType > polarizationType;

	
	
 	

	
	// ===> Attribute channel, which is optional
	
	
	bool channelExists;
	

	std::vector<std::vector<int > > channel;

	
	
 	

	////////////////////////////////
	// Extrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute antennaId
	
	

	std::vector<Tag>  antennaId;

	
	
 	

	
	// ===> Attribute pairedAntennaId, which is optional
	
	
	bool pairedAntennaIdExists;
	

	std::vector<Tag>  pairedAntennaId;

	
	
 	

	
	// ===> Attribute spectralWindowId, which is optional
	
	
	bool spectralWindowIdExists;
	

	std::vector<Tag>  spectralWindowId;

	
	
 	

	///////////
	// Links //
	///////////
	
	
		


	

	
		


	

	
		


	

	
/*
	////////////////////////////////////////////////////////////
	// binary-deserialization material from an EndianIStream  //
	////////////////////////////////////////////////////////////
	std::map<std::string, FlagAttributeFromBin> fromBinMethods;
void flagIdFromBin( EndianIStream& eis);
void startTimeFromBin( EndianIStream& eis);
void endTimeFromBin( EndianIStream& eis);
void reasonFromBin( EndianIStream& eis);
void numAntennaFromBin( EndianIStream& eis);
void antennaIdFromBin( EndianIStream& eis);

void numPolarizationTypeFromBin( EndianIStream& eis);
void numSpectralWindowFromBin( EndianIStream& eis);
void numPairedAntennaFromBin( EndianIStream& eis);
void numChanFromBin( EndianIStream& eis);
void polarizationTypeFromBin( EndianIStream& eis);
void channelFromBin( EndianIStream& eis);
void pairedAntennaIdFromBin( EndianIStream& eis);
void spectralWindowIdFromBin( EndianIStream& eis);

*/
	
	///////////////////////////////////
	// text-deserialization material //
	///////////////////////////////////
	std::map<std::string, FlagAttributeFromText> fromTextMethods;
	
void flagIdFromText (const std::string & s);
	
	
void startTimeFromText (const std::string & s);
	
	
void endTimeFromText (const std::string & s);
	
	
void reasonFromText (const std::string & s);
	
	
void numAntennaFromText (const std::string & s);
	
	
void antennaIdFromText (const std::string & s);
	

	
void numPolarizationTypeFromText (const std::string & s);
	
	
void numSpectralWindowFromText (const std::string & s);
	
	
void numPairedAntennaFromText (const std::string & s);
	
	
void numChanFromText (const std::string & s);
	
	
void polarizationTypeFromText (const std::string & s);
	
	
void channelFromText (const std::string & s);
	
	
void pairedAntennaIdFromText (const std::string & s);
	
	
void spectralWindowIdFromText (const std::string & s);
	
	
	
	/**
	 * Serialize this into a stream of bytes written to an EndianOSStream.
	 * @param eoss the EndianOSStream to be written to
	 */
	 void toBin(EndianOSStream& eoss);
	 	 
	 /**
	  * Deserialize a stream of bytes read from an EndianIStream to build a PointingRow.
	  * @param eiss the EndianIStream to be read.
	  * @param table the FlagTable to which the row built by deserialization will be parented.
	  * @param attributesSeq a vector containing the names of the attributes . The elements order defines the order 
	  * in which the attributes are written in the binary serialization.

	 static FlagRow* fromBin(EndianIStream& eis, FlagTable& table, const std::vector<std::string>& attributesSeq);	 
		*/
};

} // End namespace asdm

#endif /* Flag_CLASS */
