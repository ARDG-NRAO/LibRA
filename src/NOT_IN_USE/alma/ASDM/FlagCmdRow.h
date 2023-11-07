
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
 * File FlagCmdRow.h
 */
 
#ifndef FlagCmdRow_CLASS
#define FlagCmdRow_CLASS

#include <vector>
#include <string>
#include <set>

#ifndef WITHOUT_ACS
#include <asdmIDLC.h>
#endif






	 
#include <alma/ASDM/ArrayTimeInterval.h>
	




	

	

	

	

	

	

	



#include <alma/ASDM/ConversionException.h>
#include <alma/ASDM/NoSuchRow.h>
#include <alma/ASDM/IllegalAccessException.h>

#include <alma/ASDM/RowTransformer.h>
//#include <alma/ASDM/TableStreamReader.h>

/*\file FlagCmd.h
    \brief Generated from model's revision "-1", branch ""
*/

namespace asdm {

//class asdm::FlagCmdTable;

	

class FlagCmdRow;
typedef void (FlagCmdRow::*FlagCmdAttributeFromBin) (EndianIStream& eis);
typedef void (FlagCmdRow::*FlagCmdAttributeFromText) (const std::string& s);

/**
 * The FlagCmdRow class is a row of a FlagCmdTable.
 * 
 * Generated from model's revision "-1", branch ""
 *
 */
class FlagCmdRow {
friend class asdm::FlagCmdTable;
friend class asdm::RowTransformer<FlagCmdRow>;
//friend class asdm::TableStreamReader<FlagCmdTable, FlagCmdRow>;

public:

	virtual ~FlagCmdRow();

	/**
	 * Return the table to which this row belongs.
	 */
	FlagCmdTable &getTable() const;
	
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
  		
	
	
	


	
	// ===> Attribute type
	
	
	

	
 	/**
 	 * Get type.
 	 * @return type as std::string
 	 */
 	std::string getType() const;
	
 
 	
 	
 	/**
 	 * Set type with the specified std::string.
 	 * @param type The std::string value to which type is to be set.
 	 
 		
 			
 	 */
 	void setType (std::string type);
  		
	
	
	


	
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
  		
	
	
	


	
	// ===> Attribute level
	
	
	

	
 	/**
 	 * Get level.
 	 * @return level as int
 	 */
 	int getLevel() const;
	
 
 	
 	
 	/**
 	 * Set level with the specified int.
 	 * @param level The int value to which level is to be set.
 	 
 		
 			
 	 */
 	void setLevel (int level);
  		
	
	
	


	
	// ===> Attribute severity
	
	
	

	
 	/**
 	 * Get severity.
 	 * @return severity as int
 	 */
 	int getSeverity() const;
	
 
 	
 	
 	/**
 	 * Set severity with the specified int.
 	 * @param severity The int value to which severity is to be set.
 	 
 		
 			
 	 */
 	void setSeverity (int severity);
  		
	
	
	


	
	// ===> Attribute applied
	
	
	

	
 	/**
 	 * Get applied.
 	 * @return applied as bool
 	 */
 	bool getApplied() const;
	
 
 	
 	
 	/**
 	 * Set applied with the specified bool.
 	 * @param applied The bool value to which applied is to be set.
 	 
 		
 			
 	 */
 	void setApplied (bool applied);
  		
	
	
	


	
	// ===> Attribute command
	
	
	

	
 	/**
 	 * Get command.
 	 * @return command as std::string
 	 */
 	std::string getCommand() const;
	
 
 	
 	
 	/**
 	 * Set command with the specified std::string.
 	 * @param command The std::string value to which command is to be set.
 	 
 		
 			
 	 */
 	void setCommand (std::string command);
  		
	
	
	


	////////////////////////////////
	// Extrinsic Table Attributes //
	////////////////////////////////
	
	///////////
	// Links //
	///////////
	
	
	
	
	/**
	 * Compare each mandatory attribute except the autoincrementable one of this FlagCmdRow with 
	 * the corresponding parameters and return true if there is a match and false otherwise.
	 	
	 * @param timeInterval
	    
	 * @param type
	    
	 * @param reason
	    
	 * @param level
	    
	 * @param severity
	    
	 * @param applied
	    
	 * @param command
	    
	 */ 
	bool compareNoAutoInc(ArrayTimeInterval timeInterval, std::string type, std::string reason, int level, int severity, bool applied, std::string command);
	
	

	
	/**
	 * Compare each mandatory value (i.e. not in the key) attribute  with 
	 * the corresponding parameters and return true if there is a match and false otherwise.
	 	
	 * @param type
	    
	 * @param reason
	    
	 * @param level
	    
	 * @param severity
	    
	 * @param applied
	    
	 * @param command
	    
	 */ 
	bool compareRequiredValue(std::string type, std::string reason, int level, int severity, bool applied, std::string command); 
		 
	
	/**
	 * Return true if all required attributes of the value part are equal to their homologues
	 * in x and false otherwise.
	 *
	 * @param x a pointer on the FlagCmdRow whose required attributes of the value part 
	 * will be compared with those of this.
	 * @return a boolean.
	 */
	bool equalByRequiredValue(FlagCmdRow* x) ;
	
#ifndef WITHOUT_ACS
	/**
	 * Return this row in the form of an IDL struct.
	 * @return The values of this row as a FlagCmdRowIDL struct.
	 */
	asdmIDL::FlagCmdRowIDL *toIDL() const;
	
	/**
	 * Define the content of a FlagCmdRowIDL struct from the values
	 * found in this row.
	 *
	 * @param x a reference to the FlagCmdRowIDL struct to be set.
	 *
	 */
	 void toIDL(asdmIDL::FlagCmdRowIDL& x) const;
#endif
	
#ifndef WITHOUT_ACS
	/**
	 * Fill the values of this row from the IDL struct FlagCmdRowIDL.
	 * @param x The IDL struct containing the values used to fill this row.
	 * @throws ConversionException
	 */
	void setFromIDL (asdmIDL::FlagCmdRowIDL x) ;
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

	std::map<std::string, FlagCmdAttributeFromBin> fromBinMethods;
void timeIntervalFromBin( EndianIStream& eis);
void typeFromBin( EndianIStream& eis);
void reasonFromBin( EndianIStream& eis);
void levelFromBin( EndianIStream& eis);
void severityFromBin( EndianIStream& eis);
void appliedFromBin( EndianIStream& eis);
void commandFromBin( EndianIStream& eis);

	

	 /**
	  * Deserialize a stream of bytes read from an EndianIStream to build a PointingRow.
	  * @param eiss the EndianIStream to be read.
	  * @param table the FlagCmdTable to which the row built by deserialization will be parented.
	  * @param attributesSeq a vector containing the names of the attributes . The elements order defines the order 
	  * in which the attributes are written in the binary serialization.
	  */
	 static FlagCmdRow* fromBin(EndianIStream& eis, FlagCmdTable& table, const std::vector<std::string>& attributesSeq);	 
 
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
	FlagCmdTable &table;
	/**
	 * Whether this row has been added to the table or not.
	 */
	bool hasBeenAdded;

	// This method is used by the Table class when this row is added to the table.
	void isAdded(bool added);


	/**
	 * Create a FlagCmdRow.
	 * <p>
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 */ 
	FlagCmdRow (FlagCmdTable &table);

	/**
	 * Create a FlagCmdRow using a copy constructor mechanism.
	 * <p>
	 * Given a FlagCmdRow row and a FlagCmdTable table, the method creates a new
	 * FlagCmdRow owned by table. Each attribute of the created row is a copy (deep)
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
	 FlagCmdRow (FlagCmdTable &table, FlagCmdRow *row);
	 	
	////////////////////////////////
	// Intrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute timeInterval
	
	

	ArrayTimeInterval timeInterval;

	
	
 	

	
	// ===> Attribute type
	
	

	std::string type;

	
	
 	

	
	// ===> Attribute reason
	
	

	std::string reason;

	
	
 	

	
	// ===> Attribute level
	
	

	int level;

	
	
 	

	
	// ===> Attribute severity
	
	

	int severity;

	
	
 	

	
	// ===> Attribute applied
	
	

	bool applied;

	
	
 	

	
	// ===> Attribute command
	
	

	std::string command;

	
	
 	

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
	std::map<std::string, FlagCmdAttributeFromBin> fromBinMethods;
void timeIntervalFromBin( EndianIStream& eis);
void typeFromBin( EndianIStream& eis);
void reasonFromBin( EndianIStream& eis);
void levelFromBin( EndianIStream& eis);
void severityFromBin( EndianIStream& eis);
void appliedFromBin( EndianIStream& eis);
void commandFromBin( EndianIStream& eis);

	
*/
	
	///////////////////////////////////
	// text-deserialization material //
	///////////////////////////////////
	std::map<std::string, FlagCmdAttributeFromText> fromTextMethods;
	
void timeIntervalFromText (const std::string & s);
	
	
void typeFromText (const std::string & s);
	
	
void reasonFromText (const std::string & s);
	
	
void levelFromText (const std::string & s);
	
	
void severityFromText (const std::string & s);
	
	
void appliedFromText (const std::string & s);
	
	
void commandFromText (const std::string & s);
	

		
	
	/**
	 * Serialize this into a stream of bytes written to an EndianOSStream.
	 * @param eoss the EndianOSStream to be written to
	 */
	 void toBin(EndianOSStream& eoss);
	 	 
	 /**
	  * Deserialize a stream of bytes read from an EndianIStream to build a PointingRow.
	  * @param eiss the EndianIStream to be read.
	  * @param table the FlagCmdTable to which the row built by deserialization will be parented.
	  * @param attributesSeq a vector containing the names of the attributes . The elements order defines the order 
	  * in which the attributes are written in the binary serialization.

	 static FlagCmdRow* fromBin(EndianIStream& eis, FlagCmdTable& table, const std::vector<std::string>& attributesSeq);	 
		*/
};

} // End namespace asdm

#endif /* FlagCmd_CLASS */
