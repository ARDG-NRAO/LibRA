
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
 * File MainRow.h
 */
 
#ifndef MainRow_CLASS
#define MainRow_CLASS

#include <vector>
#include <string>
#include <set>

#ifndef WITHOUT_ACS
#include <asdmIDLC.h>
#endif



#include <stdint.h>




	 
#include <alma/ASDM/ArrayTime.h>
	

	 
#include <alma/ASDM/Tag.h>
	

	 
#include <alma/ASDM/EntityRef.h>
	

	 
#include <alma/ASDM/Interval.h>
	




	

	

	
#include <alma/Enumerations/CTimeSampling.h>
	

	

	

	

	

	

	



#include <alma/ASDM/ConversionException.h>
#include <alma/ASDM/NoSuchRow.h>
#include <alma/ASDM/IllegalAccessException.h>

#include <alma/ASDM/RowTransformer.h>
//#include <alma/ASDM/TableStreamReader.h>

/*\file Main.h
    \brief Generated from model's revision "-1", branch ""
*/

namespace asdm {

//class asdm::MainTable;


// class asdm::ConfigDescriptionRow;
class ConfigDescriptionRow;

// class asdm::FieldRow;
class FieldRow;

// class asdm::StateRow;
class StateRow;

// class asdm::ExecBlockRow;
class ExecBlockRow;
	

class MainRow;
typedef void (MainRow::*MainAttributeFromBin) (EndianIStream& eis);
typedef void (MainRow::*MainAttributeFromText) (const std::string& s);

/**
 * The MainRow class is a row of a MainTable.
 * 
 * Generated from model's revision "-1", branch ""
 *
 */
class MainRow {
friend class asdm::MainTable;
friend class asdm::RowTransformer<MainRow>;
//friend class asdm::TableStreamReader<MainTable, MainRow>;

public:

	virtual ~MainRow();

	/**
	 * Return the table to which this row belongs.
	 */
	MainTable &getTable() const;
	
	/**
	 * Has this row been added to its table ?
	 * @return true if and only if it has been added.
	 */
	bool isAdded() const;
		
	////////////////////////////////
	// Intrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute time
	
	
	

	
 	/**
 	 * Get time.
 	 * @return time as ArrayTime
 	 */
 	ArrayTime getTime() const;
	
 
 	
 	
 	/**
 	 * Set time with the specified ArrayTime.
 	 * @param time The ArrayTime value to which time is to be set.
 	 
 		
 			
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 		
 	 */
 	void setTime (ArrayTime time);
  		
	
	
	


	
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
  		
	
	
	


	
	// ===> Attribute timeSampling
	
	
	

	
 	/**
 	 * Get timeSampling.
 	 * @return timeSampling as TimeSamplingMod::TimeSampling
 	 */
 	TimeSamplingMod::TimeSampling getTimeSampling() const;
	
 
 	
 	
 	/**
 	 * Set timeSampling with the specified TimeSamplingMod::TimeSampling.
 	 * @param timeSampling The TimeSamplingMod::TimeSampling value to which timeSampling is to be set.
 	 
 		
 			
 	 */
 	void setTimeSampling (TimeSamplingMod::TimeSampling timeSampling);
  		
	
	
	


	
	// ===> Attribute interval
	
	
	

	
 	/**
 	 * Get interval.
 	 * @return interval as Interval
 	 */
 	Interval getInterval() const;
	
 
 	
 	
 	/**
 	 * Set interval with the specified Interval.
 	 * @param interval The Interval value to which interval is to be set.
 	 
 		
 			
 	 */
 	void setInterval (Interval interval);
  		
	
	
	


	
	// ===> Attribute numIntegration
	
	
	

	
 	/**
 	 * Get numIntegration.
 	 * @return numIntegration as int
 	 */
 	int getNumIntegration() const;
	
 
 	
 	
 	/**
 	 * Set numIntegration with the specified int.
 	 * @param numIntegration The int value to which numIntegration is to be set.
 	 
 		
 			
 	 */
 	void setNumIntegration (int numIntegration);
  		
	
	
	


	
	// ===> Attribute scanNumber
	
	
	

	
 	/**
 	 * Get scanNumber.
 	 * @return scanNumber as int
 	 */
 	int getScanNumber() const;
	
 
 	
 	
 	/**
 	 * Set scanNumber with the specified int.
 	 * @param scanNumber The int value to which scanNumber is to be set.
 	 
 		
 			
 	 */
 	void setScanNumber (int scanNumber);
  		
	
	
	


	
	// ===> Attribute subscanNumber
	
	
	

	
 	/**
 	 * Get subscanNumber.
 	 * @return subscanNumber as int
 	 */
 	int getSubscanNumber() const;
	
 
 	
 	
 	/**
 	 * Set subscanNumber with the specified int.
 	 * @param subscanNumber The int value to which subscanNumber is to be set.
 	 
 		
 			
 	 */
 	void setSubscanNumber (int subscanNumber);
  		
	
	
	


	
	// ===> Attribute dataSize
	
	
	

	
 	/**
 	 * Get dataSize.
 	 * @return dataSize as int64_t
 	 */
 	int64_t getDataSize() const;
	
 
 	
 	
 	/**
 	 * Set dataSize with the specified int64_t.
 	 * @param dataSize The int64_t value to which dataSize is to be set.
 	 
 		
 			
 	 */
 	void setDataSize (int64_t dataSize);
  		
	
	
	


	
	// ===> Attribute dataUID
	
	
	

	
 	/**
 	 * Get dataUID.
 	 * @return dataUID as EntityRef
 	 */
 	EntityRef getDataUID() const;
	
 
 	
 	
 	/**
 	 * Set dataUID with the specified EntityRef.
 	 * @param dataUID The EntityRef value to which dataUID is to be set.
 	 
 		
 			
 	 */
 	void setDataUID (EntityRef dataUID);
  		
	
	
	


	////////////////////////////////
	// Extrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute configDescriptionId
	
	
	

	
 	/**
 	 * Get configDescriptionId.
 	 * @return configDescriptionId as Tag
 	 */
 	Tag getConfigDescriptionId() const;
	
 
 	
 	
 	/**
 	 * Set configDescriptionId with the specified Tag.
 	 * @param configDescriptionId The Tag value to which configDescriptionId is to be set.
 	 
 		
 			
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 		
 	 */
 	void setConfigDescriptionId (Tag configDescriptionId);
  		
	
	
	


	
	// ===> Attribute execBlockId
	
	
	

	
 	/**
 	 * Get execBlockId.
 	 * @return execBlockId as Tag
 	 */
 	Tag getExecBlockId() const;
	
 
 	
 	
 	/**
 	 * Set execBlockId with the specified Tag.
 	 * @param execBlockId The Tag value to which execBlockId is to be set.
 	 
 		
 			
 	 */
 	void setExecBlockId (Tag execBlockId);
  		
	
	
	


	
	// ===> Attribute fieldId
	
	
	

	
 	/**
 	 * Get fieldId.
 	 * @return fieldId as Tag
 	 */
 	Tag getFieldId() const;
	
 
 	
 	
 	/**
 	 * Set fieldId with the specified Tag.
 	 * @param fieldId The Tag value to which fieldId is to be set.
 	 
 		
 			
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 		
 	 */
 	void setFieldId (Tag fieldId);
  		
	
	
	


	
	// ===> Attribute stateId
	
	
	

	
 	/**
 	 * Get stateId.
 	 * @return stateId as std::vector<Tag> 
 	 */
 	std::vector<Tag>  getStateId() const;
	
 
 	
 	
 	/**
 	 * Set stateId with the specified std::vector<Tag> .
 	 * @param stateId The std::vector<Tag>  value to which stateId is to be set.
 	 
 		
 			
 	 */
 	void setStateId (std::vector<Tag>  stateId);
  		
	
	
	


	///////////
	// Links //
	///////////
	
	

	
		
	/**
	 * configDescriptionId pointer to the row in the ConfigDescription table having ConfigDescription.configDescriptionId == configDescriptionId
	 * @return a ConfigDescriptionRow*
	 * 
	 
	 */
	 ConfigDescriptionRow* getConfigDescriptionUsingConfigDescriptionId();
	 

	

	

	
		
	/**
	 * fieldId pointer to the row in the Field table having Field.fieldId == fieldId
	 * @return a FieldRow*
	 * 
	 
	 */
	 FieldRow* getFieldUsingFieldId();
	 

	

	
 		
 	/**
 	 * Set stateId[i] with the specified Tag.
 	 * @param i The index in stateId where to set the Tag value.
 	 * @param stateId The Tag value to which stateId[i] is to be set. 
	 		
 	 * @throws IndexOutOfBoundsException
  	 */
  	void setStateId (int i, Tag stateId); 
 			
	

	
		 
/**
 * Append a Tag to stateId.
 * @param id the Tag to be appended to stateId
 */
 void addStateId(Tag id); 

/**
 * Append a vector of Tag to stateId.
 * @param id an array of Tag to be appended to stateId
 */
 void addStateId(const std::vector<Tag> & id); 
 

 /**
  * Returns the Tag stored in stateId at position i.
  * @param i the position in stateId where the Tag is retrieved.
  * @return the Tag stored at position i in stateId.
  */
 const Tag getStateId(int i);
 
 /**
  * Returns the StateRow linked to this row via the tag stored in stateId
  * at position i.
  * @param i the position in stateId.
  * @return a pointer on a StateRow whose key (a Tag) is equal to the Tag stored at position
  * i in the stateId. 
  */
 StateRow* getStateUsingStateId(int i); 
 
 /**
  * Returns the vector of StateRow* linked to this row via the Tags stored in stateId
  * @return an array of pointers on StateRow.
  */
 std::vector<StateRow *> getStatesUsingStateId(); 
  

	

	

	
		
	/**
	 * execBlockId pointer to the row in the ExecBlock table having ExecBlock.execBlockId == execBlockId
	 * @return a ExecBlockRow*
	 * 
	 
	 */
	 ExecBlockRow* getExecBlockUsingExecBlockId();
	 

	

	
	
	
	/**
	 * Compare each mandatory attribute except the autoincrementable one of this MainRow with 
	 * the corresponding parameters and return true if there is a match and false otherwise.
	 	
	 * @param time
	    
	 * @param configDescriptionId
	    
	 * @param fieldId
	    
	 * @param numAntenna
	    
	 * @param timeSampling
	    
	 * @param interval
	    
	 * @param numIntegration
	    
	 * @param scanNumber
	    
	 * @param subscanNumber
	    
	 * @param dataSize
	    
	 * @param dataUID
	    
	 * @param stateId
	    
	 * @param execBlockId
	    
	 */ 
	bool compareNoAutoInc(ArrayTime time, Tag configDescriptionId, Tag fieldId, int numAntenna, TimeSamplingMod::TimeSampling timeSampling, Interval interval, int numIntegration, int scanNumber, int subscanNumber, int64_t dataSize, EntityRef dataUID, std::vector<Tag>  stateId, Tag execBlockId);
	
	

	
	/**
	 * Compare each mandatory value (i.e. not in the key) attribute  with 
	 * the corresponding parameters and return true if there is a match and false otherwise.
	 	
	 * @param numAntenna
	    
	 * @param timeSampling
	    
	 * @param interval
	    
	 * @param numIntegration
	    
	 * @param scanNumber
	    
	 * @param subscanNumber
	    
	 * @param dataSize
	    
	 * @param dataUID
	    
	 * @param stateId
	    
	 * @param execBlockId
	    
	 */ 
	bool compareRequiredValue(int numAntenna, TimeSamplingMod::TimeSampling timeSampling, Interval interval, int numIntegration, int scanNumber, int subscanNumber, int64_t dataSize, EntityRef dataUID, std::vector<Tag>  stateId, Tag execBlockId); 
		 
	
	/**
	 * Return true if all required attributes of the value part are equal to their homologues
	 * in x and false otherwise.
	 *
	 * @param x a pointer on the MainRow whose required attributes of the value part 
	 * will be compared with those of this.
	 * @return a boolean.
	 */
	bool equalByRequiredValue(MainRow* x) ;
	
#ifndef WITHOUT_ACS
	/**
	 * Return this row in the form of an IDL struct.
	 * @return The values of this row as a MainRowIDL struct.
	 */
	asdmIDL::MainRowIDL *toIDL() const;
	
	/**
	 * Define the content of a MainRowIDL struct from the values
	 * found in this row.
	 *
	 * @param x a reference to the MainRowIDL struct to be set.
	 *
	 */
	 void toIDL(asdmIDL::MainRowIDL& x) const;
#endif
	
#ifndef WITHOUT_ACS
	/**
	 * Fill the values of this row from the IDL struct MainRowIDL.
	 * @param x The IDL struct containing the values used to fill this row.
	 * @throws ConversionException
	 */
	void setFromIDL (asdmIDL::MainRowIDL x) ;
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

	std::map<std::string, MainAttributeFromBin> fromBinMethods;
void timeFromBin( EndianIStream& eis);
void configDescriptionIdFromBin( EndianIStream& eis);
void fieldIdFromBin( EndianIStream& eis);
void numAntennaFromBin( EndianIStream& eis);
void timeSamplingFromBin( EndianIStream& eis);
void intervalFromBin( EndianIStream& eis);
void numIntegrationFromBin( EndianIStream& eis);
void scanNumberFromBin( EndianIStream& eis);
void subscanNumberFromBin( EndianIStream& eis);
void dataSizeFromBin( EndianIStream& eis);
void dataUIDFromBin( EndianIStream& eis);
void stateIdFromBin( EndianIStream& eis);
void execBlockIdFromBin( EndianIStream& eis);

	

	 /**
	  * Deserialize a stream of bytes read from an EndianIStream to build a PointingRow.
	  * @param eiss the EndianIStream to be read.
	  * @param table the MainTable to which the row built by deserialization will be parented.
	  * @param attributesSeq a vector containing the names of the attributes . The elements order defines the order 
	  * in which the attributes are written in the binary serialization.
	  */
	 static MainRow* fromBin(EndianIStream& eis, MainTable& table, const std::vector<std::string>& attributesSeq);	 
 
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
	MainTable &table;
	/**
	 * Whether this row has been added to the table or not.
	 */
	bool hasBeenAdded;

	// This method is used by the Table class when this row is added to the table.
	void isAdded(bool added);


	/**
	 * Create a MainRow.
	 * <p>
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 */ 
	MainRow (MainTable &table);

	/**
	 * Create a MainRow using a copy constructor mechanism.
	 * <p>
	 * Given a MainRow row and a MainTable table, the method creates a new
	 * MainRow owned by table. Each attribute of the created row is a copy (deep)
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
	 MainRow (MainTable &table, MainRow *row);
	 	
	////////////////////////////////
	// Intrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute time
	
	

	ArrayTime time;

	
	
 	

	
	// ===> Attribute numAntenna
	
	

	int numAntenna;

	
	
 	

	
	// ===> Attribute timeSampling
	
	

	TimeSamplingMod::TimeSampling timeSampling;

	
	
 	

	
	// ===> Attribute interval
	
	

	Interval interval;

	
	
 	

	
	// ===> Attribute numIntegration
	
	

	int numIntegration;

	
	
 	

	
	// ===> Attribute scanNumber
	
	

	int scanNumber;

	
	
 	

	
	// ===> Attribute subscanNumber
	
	

	int subscanNumber;

	
	
 	

	
	// ===> Attribute dataSize
	
	

	int64_t dataSize;

	
	
 	

	
	// ===> Attribute dataUID
	
	

	EntityRef dataUID;

	
	
 	

	////////////////////////////////
	// Extrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute configDescriptionId
	
	

	Tag configDescriptionId;

	
	
 	

	
	// ===> Attribute execBlockId
	
	

	Tag execBlockId;

	
	
 	

	
	// ===> Attribute fieldId
	
	

	Tag fieldId;

	
	
 	

	
	// ===> Attribute stateId
	
	

	std::vector<Tag>  stateId;

	
	
 	

	///////////
	// Links //
	///////////
	
	
		

	 

	

	
		

	 

	

	
		


	

	
		

	 

	

	
/*
	////////////////////////////////////////////////////////////
	// binary-deserialization material from an EndianIStream  //
	////////////////////////////////////////////////////////////
	std::map<std::string, MainAttributeFromBin> fromBinMethods;
void timeFromBin( EndianIStream& eis);
void configDescriptionIdFromBin( EndianIStream& eis);
void fieldIdFromBin( EndianIStream& eis);
void numAntennaFromBin( EndianIStream& eis);
void timeSamplingFromBin( EndianIStream& eis);
void intervalFromBin( EndianIStream& eis);
void numIntegrationFromBin( EndianIStream& eis);
void scanNumberFromBin( EndianIStream& eis);
void subscanNumberFromBin( EndianIStream& eis);
void dataSizeFromBin( EndianIStream& eis);
void dataUIDFromBin( EndianIStream& eis);
void stateIdFromBin( EndianIStream& eis);
void execBlockIdFromBin( EndianIStream& eis);

	
*/
	
	///////////////////////////////////
	// text-deserialization material //
	///////////////////////////////////
	std::map<std::string, MainAttributeFromText> fromTextMethods;
	
void timeFromText (const std::string & s);
	
	
void configDescriptionIdFromText (const std::string & s);
	
	
void fieldIdFromText (const std::string & s);
	
	
void numAntennaFromText (const std::string & s);
	
	
void timeSamplingFromText (const std::string & s);
	
	
void intervalFromText (const std::string & s);
	
	
void numIntegrationFromText (const std::string & s);
	
	
void scanNumberFromText (const std::string & s);
	
	
void subscanNumberFromText (const std::string & s);
	
	
void dataSizeFromText (const std::string & s);
	
	
	
void stateIdFromText (const std::string & s);
	
	
void execBlockIdFromText (const std::string & s);
	

		
	
	/**
	 * Serialize this into a stream of bytes written to an EndianOSStream.
	 * @param eoss the EndianOSStream to be written to
	 */
	 void toBin(EndianOSStream& eoss);
	 	 
	 /**
	  * Deserialize a stream of bytes read from an EndianIStream to build a PointingRow.
	  * @param eiss the EndianIStream to be read.
	  * @param table the MainTable to which the row built by deserialization will be parented.
	  * @param attributesSeq a vector containing the names of the attributes . The elements order defines the order 
	  * in which the attributes are written in the binary serialization.

	 static MainRow* fromBin(EndianIStream& eis, MainTable& table, const std::vector<std::string>& attributesSeq);	 
		*/
};

} // End namespace asdm

#endif /* Main_CLASS */
