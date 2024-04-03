
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
 * File TotalPowerRow.h
 */
 
#ifndef TotalPowerRow_CLASS
#define TotalPowerRow_CLASS

#include <vector>
#include <string>
#include <set>

#ifndef WITHOUT_ACS
#include <asdmIDLC.h>
#endif






	 
#include <alma/ASDM/ArrayTime.h>
	

	 
#include <alma/ASDM/Length.h>
	

	 
#include <alma/ASDM/Tag.h>
	

	 
#include <alma/ASDM/Interval.h>
	




	

	

	

	

	

	

	

	

	

	

	

	



#include <alma/ASDM/ConversionException.h>
#include <alma/ASDM/NoSuchRow.h>
#include <alma/ASDM/IllegalAccessException.h>

#include <alma/ASDM/RowTransformer.h>
//#include <alma/ASDM/TableStreamReader.h>

/*\file TotalPower.h
    \brief Generated from model's revision "-1", branch ""
*/

namespace asdm {

//class asdm::TotalPowerTable;


// class asdm::StateRow;
class StateRow;

// class asdm::FieldRow;
class FieldRow;

// class asdm::ConfigDescriptionRow;
class ConfigDescriptionRow;

// class asdm::ExecBlockRow;
class ExecBlockRow;
	

class TotalPowerRow;
typedef void (TotalPowerRow::*TotalPowerAttributeFromBin) (EndianIStream& eis);
typedef void (TotalPowerRow::*TotalPowerAttributeFromText) (const std::string& s);

/**
 * The TotalPowerRow class is a row of a TotalPowerTable.
 * 
 * Generated from model's revision "-1", branch ""
 *
 */
class TotalPowerRow {
friend class asdm::TotalPowerTable;
friend class asdm::RowTransformer<TotalPowerRow>;
//friend class asdm::TableStreamReader<TotalPowerTable, TotalPowerRow>;

public:

	virtual ~TotalPowerRow();

	/**
	 * Return the table to which this row belongs.
	 */
	TotalPowerTable &getTable() const;
	
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
  		
	
	
	


	
	// ===> Attribute integrationNumber
	
	
	

	
 	/**
 	 * Get integrationNumber.
 	 * @return integrationNumber as int
 	 */
 	int getIntegrationNumber() const;
	
 
 	
 	
 	/**
 	 * Set integrationNumber with the specified int.
 	 * @param integrationNumber The int value to which integrationNumber is to be set.
 	 
 		
 			
 	 */
 	void setIntegrationNumber (int integrationNumber);
  		
	
	
	


	
	// ===> Attribute uvw
	
	
	

	
 	/**
 	 * Get uvw.
 	 * @return uvw as std::vector<std::vector<Length > >
 	 */
 	std::vector<std::vector<Length > > getUvw() const;
	
 
 	
 	
 	/**
 	 * Set uvw with the specified std::vector<std::vector<Length > >.
 	 * @param uvw The std::vector<std::vector<Length > > value to which uvw is to be set.
 	 
 		
 			
 	 */
 	void setUvw (std::vector<std::vector<Length > > uvw);
  		
	
	
	


	
	// ===> Attribute exposure
	
	
	

	
 	/**
 	 * Get exposure.
 	 * @return exposure as std::vector<std::vector<Interval > >
 	 */
 	std::vector<std::vector<Interval > > getExposure() const;
	
 
 	
 	
 	/**
 	 * Set exposure with the specified std::vector<std::vector<Interval > >.
 	 * @param exposure The std::vector<std::vector<Interval > > value to which exposure is to be set.
 	 
 		
 			
 	 */
 	void setExposure (std::vector<std::vector<Interval > > exposure);
  		
	
	
	


	
	// ===> Attribute timeCentroid
	
	
	

	
 	/**
 	 * Get timeCentroid.
 	 * @return timeCentroid as std::vector<std::vector<ArrayTime > >
 	 */
 	std::vector<std::vector<ArrayTime > > getTimeCentroid() const;
	
 
 	
 	
 	/**
 	 * Set timeCentroid with the specified std::vector<std::vector<ArrayTime > >.
 	 * @param timeCentroid The std::vector<std::vector<ArrayTime > > value to which timeCentroid is to be set.
 	 
 		
 			
 	 */
 	void setTimeCentroid (std::vector<std::vector<ArrayTime > > timeCentroid);
  		
	
	
	


	
	// ===> Attribute floatData
	
	
	

	
 	/**
 	 * Get floatData.
 	 * @return floatData as std::vector<std::vector<std::vector<float > > >
 	 */
 	std::vector<std::vector<std::vector<float > > > getFloatData() const;
	
 
 	
 	
 	/**
 	 * Set floatData with the specified std::vector<std::vector<std::vector<float > > >.
 	 * @param floatData The std::vector<std::vector<std::vector<float > > > value to which floatData is to be set.
 	 
 		
 			
 	 */
 	void setFloatData (std::vector<std::vector<std::vector<float > > > floatData);
  		
	
	
	


	
	// ===> Attribute flagAnt
	
	
	

	
 	/**
 	 * Get flagAnt.
 	 * @return flagAnt as std::vector<int >
 	 */
 	std::vector<int > getFlagAnt() const;
	
 
 	
 	
 	/**
 	 * Set flagAnt with the specified std::vector<int >.
 	 * @param flagAnt The std::vector<int > value to which flagAnt is to be set.
 	 
 		
 			
 	 */
 	void setFlagAnt (std::vector<int > flagAnt);
  		
	
	
	


	
	// ===> Attribute flagPol
	
	
	

	
 	/**
 	 * Get flagPol.
 	 * @return flagPol as std::vector<std::vector<int > >
 	 */
 	std::vector<std::vector<int > > getFlagPol() const;
	
 
 	
 	
 	/**
 	 * Set flagPol with the specified std::vector<std::vector<int > >.
 	 * @param flagPol The std::vector<std::vector<int > > value to which flagPol is to be set.
 	 
 		
 			
 	 */
 	void setFlagPol (std::vector<std::vector<int > > flagPol);
  		
	
	
	


	
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
  		
	
	
	


	
	// ===> Attribute subintegrationNumber, which is optional
	
	
	
	/**
	 * The attribute subintegrationNumber is optional. Return true if this attribute exists.
	 * @return true if and only if the subintegrationNumber attribute exists. 
	 */
	bool isSubintegrationNumberExists() const;
	

	
 	/**
 	 * Get subintegrationNumber, which is optional.
 	 * @return subintegrationNumber as int
 	 * @throws IllegalAccessException If subintegrationNumber does not exist.
 	 */
 	int getSubintegrationNumber() const;
	
 
 	
 	
 	/**
 	 * Set subintegrationNumber with the specified int.
 	 * @param subintegrationNumber The int value to which subintegrationNumber is to be set.
 	 
 		
 	 */
 	void setSubintegrationNumber (int subintegrationNumber);
		
	
	
	
	/**
	 * Mark subintegrationNumber, which is an optional field, as non-existent.
	 */
	void clearSubintegrationNumber ();
	


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
	 * fieldId pointer to the row in the Field table having Field.fieldId == fieldId
	 * @return a FieldRow*
	 * 
	 
	 */
	 FieldRow* getFieldUsingFieldId();
	 

	

	

	
		
	/**
	 * configDescriptionId pointer to the row in the ConfigDescription table having ConfigDescription.configDescriptionId == configDescriptionId
	 * @return a ConfigDescriptionRow*
	 * 
	 
	 */
	 ConfigDescriptionRow* getConfigDescriptionUsingConfigDescriptionId();
	 

	

	

	
		
	/**
	 * execBlockId pointer to the row in the ExecBlock table having ExecBlock.execBlockId == execBlockId
	 * @return a ExecBlockRow*
	 * 
	 
	 */
	 ExecBlockRow* getExecBlockUsingExecBlockId();
	 

	

	
	
	
	/**
	 * Compare each mandatory attribute except the autoincrementable one of this TotalPowerRow with 
	 * the corresponding parameters and return true if there is a match and false otherwise.
	 	
	 * @param time
	    
	 * @param configDescriptionId
	    
	 * @param fieldId
	    
	 * @param scanNumber
	    
	 * @param subscanNumber
	    
	 * @param integrationNumber
	    
	 * @param uvw
	    
	 * @param exposure
	    
	 * @param timeCentroid
	    
	 * @param floatData
	    
	 * @param flagAnt
	    
	 * @param flagPol
	    
	 * @param interval
	    
	 * @param stateId
	    
	 * @param execBlockId
	    
	 */ 
	bool compareNoAutoInc(ArrayTime time, Tag configDescriptionId, Tag fieldId, int scanNumber, int subscanNumber, int integrationNumber, std::vector<std::vector<Length > > uvw, std::vector<std::vector<Interval > > exposure, std::vector<std::vector<ArrayTime > > timeCentroid, std::vector<std::vector<std::vector<float > > > floatData, std::vector<int > flagAnt, std::vector<std::vector<int > > flagPol, Interval interval, std::vector<Tag>  stateId, Tag execBlockId);
	
	

	
	/**
	 * Compare each mandatory value (i.e. not in the key) attribute  with 
	 * the corresponding parameters and return true if there is a match and false otherwise.
	 	
	 * @param scanNumber
	    
	 * @param subscanNumber
	    
	 * @param integrationNumber
	    
	 * @param uvw
	    
	 * @param exposure
	    
	 * @param timeCentroid
	    
	 * @param floatData
	    
	 * @param flagAnt
	    
	 * @param flagPol
	    
	 * @param interval
	    
	 * @param stateId
	    
	 * @param execBlockId
	    
	 */ 
	bool compareRequiredValue(int scanNumber, int subscanNumber, int integrationNumber, std::vector<std::vector<Length > > uvw, std::vector<std::vector<Interval > > exposure, std::vector<std::vector<ArrayTime > > timeCentroid, std::vector<std::vector<std::vector<float > > > floatData, std::vector<int > flagAnt, std::vector<std::vector<int > > flagPol, Interval interval, std::vector<Tag>  stateId, Tag execBlockId); 
		 
	
	/**
	 * Return true if all required attributes of the value part are equal to their homologues
	 * in x and false otherwise.
	 *
	 * @param x a pointer on the TotalPowerRow whose required attributes of the value part 
	 * will be compared with those of this.
	 * @return a boolean.
	 */
	bool equalByRequiredValue(TotalPowerRow* x) ;
	
#ifndef WITHOUT_ACS
	/**
	 * Return this row in the form of an IDL struct.
	 * @return The values of this row as a TotalPowerRowIDL struct.
	 */
	asdmIDL::TotalPowerRowIDL *toIDL() const;
	
	/**
	 * Define the content of a TotalPowerRowIDL struct from the values
	 * found in this row.
	 *
	 * @param x a reference to the TotalPowerRowIDL struct to be set.
	 *
	 */
	 void toIDL(asdmIDL::TotalPowerRowIDL& x) const;
#endif
	
#ifndef WITHOUT_ACS
	/**
	 * Fill the values of this row from the IDL struct TotalPowerRowIDL.
	 * @param x The IDL struct containing the values used to fill this row.
	 * @throws ConversionException
	 */
	void setFromIDL (asdmIDL::TotalPowerRowIDL x) ;
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

	std::map<std::string, TotalPowerAttributeFromBin> fromBinMethods;
void timeFromBin( EndianIStream& eis);
void configDescriptionIdFromBin( EndianIStream& eis);
void fieldIdFromBin( EndianIStream& eis);
void scanNumberFromBin( EndianIStream& eis);
void subscanNumberFromBin( EndianIStream& eis);
void integrationNumberFromBin( EndianIStream& eis);
void uvwFromBin( EndianIStream& eis);
void exposureFromBin( EndianIStream& eis);
void timeCentroidFromBin( EndianIStream& eis);
void floatDataFromBin( EndianIStream& eis);
void flagAntFromBin( EndianIStream& eis);
void flagPolFromBin( EndianIStream& eis);
void intervalFromBin( EndianIStream& eis);
void stateIdFromBin( EndianIStream& eis);
void execBlockIdFromBin( EndianIStream& eis);

void subintegrationNumberFromBin( EndianIStream& eis);


	 /**
	  * Deserialize a stream of bytes read from an EndianIStream to build a PointingRow.
	  * @param eiss the EndianIStream to be read.
	  * @param table the TotalPowerTable to which the row built by deserialization will be parented.
	  * @param attributesSeq a vector containing the names of the attributes . The elements order defines the order 
	  * in which the attributes are written in the binary serialization.
	  */
	 static TotalPowerRow* fromBin(EndianIStream& eis, TotalPowerTable& table, const std::vector<std::string>& attributesSeq);	 
 
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
	TotalPowerTable &table;
	/**
	 * Whether this row has been added to the table or not.
	 */
	bool hasBeenAdded;

	// This method is used by the Table class when this row is added to the table.
	void isAdded(bool added);


	/**
	 * Create a TotalPowerRow.
	 * <p>
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 */ 
	TotalPowerRow (TotalPowerTable &table);

	/**
	 * Create a TotalPowerRow using a copy constructor mechanism.
	 * <p>
	 * Given a TotalPowerRow row and a TotalPowerTable table, the method creates a new
	 * TotalPowerRow owned by table. Each attribute of the created row is a copy (deep)
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
	 TotalPowerRow (TotalPowerTable &table, TotalPowerRow *row);
	 	
	////////////////////////////////
	// Intrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute time
	
	

	ArrayTime time;

	
	
 	

	
	// ===> Attribute scanNumber
	
	

	int scanNumber;

	
	
 	

	
	// ===> Attribute subscanNumber
	
	

	int subscanNumber;

	
	
 	

	
	// ===> Attribute integrationNumber
	
	

	int integrationNumber;

	
	
 	

	
	// ===> Attribute uvw
	
	

	std::vector<std::vector<Length > > uvw;

	
	
 	

	
	// ===> Attribute exposure
	
	

	std::vector<std::vector<Interval > > exposure;

	
	
 	

	
	// ===> Attribute timeCentroid
	
	

	std::vector<std::vector<ArrayTime > > timeCentroid;

	
	
 	

	
	// ===> Attribute floatData
	
	

	std::vector<std::vector<std::vector<float > > > floatData;

	
	
 	

	
	// ===> Attribute flagAnt
	
	

	std::vector<int > flagAnt;

	
	
 	

	
	// ===> Attribute flagPol
	
	

	std::vector<std::vector<int > > flagPol;

	
	
 	

	
	// ===> Attribute interval
	
	

	Interval interval;

	
	
 	

	
	// ===> Attribute subintegrationNumber, which is optional
	
	
	bool subintegrationNumberExists;
	

	int subintegrationNumber;

	
	
 	

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
	std::map<std::string, TotalPowerAttributeFromBin> fromBinMethods;
void timeFromBin( EndianIStream& eis);
void configDescriptionIdFromBin( EndianIStream& eis);
void fieldIdFromBin( EndianIStream& eis);
void scanNumberFromBin( EndianIStream& eis);
void subscanNumberFromBin( EndianIStream& eis);
void integrationNumberFromBin( EndianIStream& eis);
void uvwFromBin( EndianIStream& eis);
void exposureFromBin( EndianIStream& eis);
void timeCentroidFromBin( EndianIStream& eis);
void floatDataFromBin( EndianIStream& eis);
void flagAntFromBin( EndianIStream& eis);
void flagPolFromBin( EndianIStream& eis);
void intervalFromBin( EndianIStream& eis);
void stateIdFromBin( EndianIStream& eis);
void execBlockIdFromBin( EndianIStream& eis);

void subintegrationNumberFromBin( EndianIStream& eis);

*/
	
	///////////////////////////////////
	// text-deserialization material //
	///////////////////////////////////
	std::map<std::string, TotalPowerAttributeFromText> fromTextMethods;
	
void timeFromText (const std::string & s);
	
	
void configDescriptionIdFromText (const std::string & s);
	
	
void fieldIdFromText (const std::string & s);
	
	
void scanNumberFromText (const std::string & s);
	
	
void subscanNumberFromText (const std::string & s);
	
	
void integrationNumberFromText (const std::string & s);
	
	
void uvwFromText (const std::string & s);
	
	
void exposureFromText (const std::string & s);
	
	
void timeCentroidFromText (const std::string & s);
	
	
void floatDataFromText (const std::string & s);
	
	
void flagAntFromText (const std::string & s);
	
	
void flagPolFromText (const std::string & s);
	
	
void intervalFromText (const std::string & s);
	
	
void stateIdFromText (const std::string & s);
	
	
void execBlockIdFromText (const std::string & s);
	

	
void subintegrationNumberFromText (const std::string & s);
	
	
	
	/**
	 * Serialize this into a stream of bytes written to an EndianOSStream.
	 * @param eoss the EndianOSStream to be written to
	 */
	 void toBin(EndianOSStream& eoss);
	 	 
	 /**
	  * Deserialize a stream of bytes read from an EndianIStream to build a PointingRow.
	  * @param eiss the EndianIStream to be read.
	  * @param table the TotalPowerTable to which the row built by deserialization will be parented.
	  * @param attributesSeq a vector containing the names of the attributes . The elements order defines the order 
	  * in which the attributes are written in the binary serialization.

	 static TotalPowerRow* fromBin(EndianIStream& eis, TotalPowerTable& table, const std::vector<std::string>& attributesSeq);	 
		*/
};

} // End namespace asdm

#endif /* TotalPower_CLASS */
