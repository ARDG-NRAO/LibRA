
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
 * File StateRow.h
 */
 
#ifndef StateRow_CLASS
#define StateRow_CLASS

#include <vector>
#include <string>
#include <set>

#ifndef WITHOUT_ACS
#include <asdmIDLC.h>
#endif






	 
#include <alma/ASDM/Tag.h>
	




	

	
#include <alma/Enumerations/CCalibrationDevice.h>
	

	

	

	

	



#include <alma/ASDM/ConversionException.h>
#include <alma/ASDM/NoSuchRow.h>
#include <alma/ASDM/IllegalAccessException.h>

#include <alma/ASDM/RowTransformer.h>
//#include <alma/ASDM/TableStreamReader.h>

/*\file State.h
    \brief Generated from model's revision "-1", branch ""
*/

namespace asdm {

//class asdm::StateTable;

	

class StateRow;
typedef void (StateRow::*StateAttributeFromBin) (EndianIStream& eis);
typedef void (StateRow::*StateAttributeFromText) (const std::string& s);

/**
 * The StateRow class is a row of a StateTable.
 * 
 * Generated from model's revision "-1", branch ""
 *
 */
class StateRow {
friend class asdm::StateTable;
friend class asdm::RowTransformer<StateRow>;
//friend class asdm::TableStreamReader<StateTable, StateRow>;

public:

	virtual ~StateRow();

	/**
	 * Return the table to which this row belongs.
	 */
	StateTable &getTable() const;
	
	/**
	 * Has this row been added to its table ?
	 * @return true if and only if it has been added.
	 */
	bool isAdded() const;
		
	////////////////////////////////
	// Intrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute stateId
	
	
	

	
 	/**
 	 * Get stateId.
 	 * @return stateId as Tag
 	 */
 	Tag getStateId() const;
	
 
 	
 	
	
	


	
	// ===> Attribute calDeviceName
	
	
	

	
 	/**
 	 * Get calDeviceName.
 	 * @return calDeviceName as CalibrationDeviceMod::CalibrationDevice
 	 */
 	CalibrationDeviceMod::CalibrationDevice getCalDeviceName() const;
	
 
 	
 	
 	/**
 	 * Set calDeviceName with the specified CalibrationDeviceMod::CalibrationDevice.
 	 * @param calDeviceName The CalibrationDeviceMod::CalibrationDevice value to which calDeviceName is to be set.
 	 
 		
 			
 	 */
 	void setCalDeviceName (CalibrationDeviceMod::CalibrationDevice calDeviceName);
  		
	
	
	


	
	// ===> Attribute sig
	
	
	

	
 	/**
 	 * Get sig.
 	 * @return sig as bool
 	 */
 	bool getSig() const;
	
 
 	
 	
 	/**
 	 * Set sig with the specified bool.
 	 * @param sig The bool value to which sig is to be set.
 	 
 		
 			
 	 */
 	void setSig (bool sig);
  		
	
	
	


	
	// ===> Attribute ref
	
	
	

	
 	/**
 	 * Get ref.
 	 * @return ref as bool
 	 */
 	bool getRef() const;
	
 
 	
 	
 	/**
 	 * Set ref with the specified bool.
 	 * @param ref The bool value to which ref is to be set.
 	 
 		
 			
 	 */
 	void setRef (bool ref);
  		
	
	
	


	
	// ===> Attribute onSky
	
	
	

	
 	/**
 	 * Get onSky.
 	 * @return onSky as bool
 	 */
 	bool getOnSky() const;
	
 
 	
 	
 	/**
 	 * Set onSky with the specified bool.
 	 * @param onSky The bool value to which onSky is to be set.
 	 
 		
 			
 	 */
 	void setOnSky (bool onSky);
  		
	
	
	


	
	// ===> Attribute weight, which is optional
	
	
	
	/**
	 * The attribute weight is optional. Return true if this attribute exists.
	 * @return true if and only if the weight attribute exists. 
	 */
	bool isWeightExists() const;
	

	
 	/**
 	 * Get weight, which is optional.
 	 * @return weight as float
 	 * @throws IllegalAccessException If weight does not exist.
 	 */
 	float getWeight() const;
	
 
 	
 	
 	/**
 	 * Set weight with the specified float.
 	 * @param weight The float value to which weight is to be set.
 	 
 		
 	 */
 	void setWeight (float weight);
		
	
	
	
	/**
	 * Mark weight, which is an optional field, as non-existent.
	 */
	void clearWeight ();
	


	////////////////////////////////
	// Extrinsic Table Attributes //
	////////////////////////////////
	
	///////////
	// Links //
	///////////
	
	
	
	
	/**
	 * Compare each mandatory attribute except the autoincrementable one of this StateRow with 
	 * the corresponding parameters and return true if there is a match and false otherwise.
	 	
	 * @param calDeviceName
	    
	 * @param sig
	    
	 * @param ref
	    
	 * @param onSky
	    
	 */ 
	bool compareNoAutoInc(CalibrationDeviceMod::CalibrationDevice calDeviceName, bool sig, bool ref, bool onSky);
	
	

	
	/**
	 * Compare each mandatory value (i.e. not in the key) attribute  with 
	 * the corresponding parameters and return true if there is a match and false otherwise.
	 	
	 * @param calDeviceName
	    
	 * @param sig
	    
	 * @param ref
	    
	 * @param onSky
	    
	 */ 
	bool compareRequiredValue(CalibrationDeviceMod::CalibrationDevice calDeviceName, bool sig, bool ref, bool onSky); 
		 
	
	/**
	 * Return true if all required attributes of the value part are equal to their homologues
	 * in x and false otherwise.
	 *
	 * @param x a pointer on the StateRow whose required attributes of the value part 
	 * will be compared with those of this.
	 * @return a boolean.
	 */
	bool equalByRequiredValue(StateRow* x) ;
	
#ifndef WITHOUT_ACS
	/**
	 * Return this row in the form of an IDL struct.
	 * @return The values of this row as a StateRowIDL struct.
	 */
	asdmIDL::StateRowIDL *toIDL() const;
	
	/**
	 * Define the content of a StateRowIDL struct from the values
	 * found in this row.
	 *
	 * @param x a reference to the StateRowIDL struct to be set.
	 *
	 */
	 void toIDL(asdmIDL::StateRowIDL& x) const;
#endif
	
#ifndef WITHOUT_ACS
	/**
	 * Fill the values of this row from the IDL struct StateRowIDL.
	 * @param x The IDL struct containing the values used to fill this row.
	 * @throws ConversionException
	 */
	void setFromIDL (asdmIDL::StateRowIDL x) ;
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

	std::map<std::string, StateAttributeFromBin> fromBinMethods;
void stateIdFromBin( EndianIStream& eis);
void calDeviceNameFromBin( EndianIStream& eis);
void sigFromBin( EndianIStream& eis);
void refFromBin( EndianIStream& eis);
void onSkyFromBin( EndianIStream& eis);

void weightFromBin( EndianIStream& eis);


	 /**
	  * Deserialize a stream of bytes read from an EndianIStream to build a PointingRow.
	  * @param eiss the EndianIStream to be read.
	  * @param table the StateTable to which the row built by deserialization will be parented.
	  * @param attributesSeq a vector containing the names of the attributes . The elements order defines the order 
	  * in which the attributes are written in the binary serialization.
	  */
	 static StateRow* fromBin(EndianIStream& eis, StateTable& table, const std::vector<std::string>& attributesSeq);	 
 
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
	StateTable &table;
	/**
	 * Whether this row has been added to the table or not.
	 */
	bool hasBeenAdded;

	// This method is used by the Table class when this row is added to the table.
	void isAdded(bool added);


	/**
	 * Create a StateRow.
	 * <p>
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 */ 
	StateRow (StateTable &table);

	/**
	 * Create a StateRow using a copy constructor mechanism.
	 * <p>
	 * Given a StateRow row and a StateTable table, the method creates a new
	 * StateRow owned by table. Each attribute of the created row is a copy (deep)
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
	 StateRow (StateTable &table, StateRow *row);
	 	
	////////////////////////////////
	// Intrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute stateId
	
	

	Tag stateId;

	
	
 	
 	/**
 	 * Set stateId with the specified Tag value.
 	 * @param stateId The Tag value to which stateId is to be set.
		
 		
			
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 		
 	 */
 	void setStateId (Tag stateId);
  		
	

	
	// ===> Attribute calDeviceName
	
	

	CalibrationDeviceMod::CalibrationDevice calDeviceName;

	
	
 	

	
	// ===> Attribute sig
	
	

	bool sig;

	
	
 	

	
	// ===> Attribute ref
	
	

	bool ref;

	
	
 	

	
	// ===> Attribute onSky
	
	

	bool onSky;

	
	
 	

	
	// ===> Attribute weight, which is optional
	
	
	bool weightExists;
	

	float weight;

	
	
 	

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
	std::map<std::string, StateAttributeFromBin> fromBinMethods;
void stateIdFromBin( EndianIStream& eis);
void calDeviceNameFromBin( EndianIStream& eis);
void sigFromBin( EndianIStream& eis);
void refFromBin( EndianIStream& eis);
void onSkyFromBin( EndianIStream& eis);

void weightFromBin( EndianIStream& eis);

*/
	
	///////////////////////////////////
	// text-deserialization material //
	///////////////////////////////////
	std::map<std::string, StateAttributeFromText> fromTextMethods;
	
void stateIdFromText (const std::string & s);
	
	
void calDeviceNameFromText (const std::string & s);
	
	
void sigFromText (const std::string & s);
	
	
void refFromText (const std::string & s);
	
	
void onSkyFromText (const std::string & s);
	

	
void weightFromText (const std::string & s);
	
	
	
	/**
	 * Serialize this into a stream of bytes written to an EndianOSStream.
	 * @param eoss the EndianOSStream to be written to
	 */
	 void toBin(EndianOSStream& eoss);
	 	 
	 /**
	  * Deserialize a stream of bytes read from an EndianIStream to build a PointingRow.
	  * @param eiss the EndianIStream to be read.
	  * @param table the StateTable to which the row built by deserialization will be parented.
	  * @param attributesSeq a vector containing the names of the attributes . The elements order defines the order 
	  * in which the attributes are written in the binary serialization.

	 static StateRow* fromBin(EndianIStream& eis, StateTable& table, const std::vector<std::string>& attributesSeq);	 
		*/
};

} // End namespace asdm

#endif /* State_CLASS */
