
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
 * File FieldRow.h
 */
 
#ifndef FieldRow_CLASS
#define FieldRow_CLASS

#include <vector>
#include <string>
#include <set>

#ifndef WITHOUT_ACS
#include <asdmIDLC.h>
#endif






	 
#include <alma/ASDM/Angle.h>
	

	 
#include <alma/ASDM/ArrayTime.h>
	

	 
#include <alma/ASDM/Tag.h>
	




	

	

	

	

	

	

	

	

	
#include <alma/Enumerations/CDirectionReferenceCode.h>
	

	

	



#include <alma/ASDM/ConversionException.h>
#include <alma/ASDM/NoSuchRow.h>
#include <alma/ASDM/IllegalAccessException.h>

#include <alma/ASDM/RowTransformer.h>
//#include <alma/ASDM/TableStreamReader.h>

/*\file Field.h
    \brief Generated from model's revision "-1", branch ""
*/

namespace asdm {

//class asdm::FieldTable;


// class asdm::EphemerisRow;
class EphemerisRow;

// class asdm::SourceRow;
class SourceRow;

// class asdm::FieldRow;
class FieldRow;
	

class FieldRow;
typedef void (FieldRow::*FieldAttributeFromBin) (EndianIStream& eis);
typedef void (FieldRow::*FieldAttributeFromText) (const std::string& s);

/**
 * The FieldRow class is a row of a FieldTable.
 * 
 * Generated from model's revision "-1", branch ""
 *
 */
class FieldRow {
friend class asdm::FieldTable;
friend class asdm::RowTransformer<FieldRow>;
//friend class asdm::TableStreamReader<FieldTable, FieldRow>;

public:

	virtual ~FieldRow();

	/**
	 * Return the table to which this row belongs.
	 */
	FieldTable &getTable() const;
	
	/**
	 * Has this row been added to its table ?
	 * @return true if and only if it has been added.
	 */
	bool isAdded() const;
		
	////////////////////////////////
	// Intrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute fieldId
	
	
	

	
 	/**
 	 * Get fieldId.
 	 * @return fieldId as Tag
 	 */
 	Tag getFieldId() const;
	
 
 	
 	
	
	


	
	// ===> Attribute fieldName
	
	
	

	
 	/**
 	 * Get fieldName.
 	 * @return fieldName as std::string
 	 */
 	std::string getFieldName() const;
	
 
 	
 	
 	/**
 	 * Set fieldName with the specified std::string.
 	 * @param fieldName The std::string value to which fieldName is to be set.
 	 
 		
 			
 	 */
 	void setFieldName (std::string fieldName);
  		
	
	
	


	
	// ===> Attribute numPoly
	
	
	

	
 	/**
 	 * Get numPoly.
 	 * @return numPoly as int
 	 */
 	int getNumPoly() const;
	
 
 	
 	
 	/**
 	 * Set numPoly with the specified int.
 	 * @param numPoly The int value to which numPoly is to be set.
 	 
 		
 			
 	 */
 	void setNumPoly (int numPoly);
  		
	
	
	


	
	// ===> Attribute delayDir
	
	
	

	
 	/**
 	 * Get delayDir.
 	 * @return delayDir as std::vector<std::vector<Angle > >
 	 */
 	std::vector<std::vector<Angle > > getDelayDir() const;
	
 
 	
 	
 	/**
 	 * Set delayDir with the specified std::vector<std::vector<Angle > >.
 	 * @param delayDir The std::vector<std::vector<Angle > > value to which delayDir is to be set.
 	 
 		
 			
 	 */
 	void setDelayDir (std::vector<std::vector<Angle > > delayDir);
  		
	
	
	


	
	// ===> Attribute phaseDir
	
	
	

	
 	/**
 	 * Get phaseDir.
 	 * @return phaseDir as std::vector<std::vector<Angle > >
 	 */
 	std::vector<std::vector<Angle > > getPhaseDir() const;
	
 
 	
 	
 	/**
 	 * Set phaseDir with the specified std::vector<std::vector<Angle > >.
 	 * @param phaseDir The std::vector<std::vector<Angle > > value to which phaseDir is to be set.
 	 
 		
 			
 	 */
 	void setPhaseDir (std::vector<std::vector<Angle > > phaseDir);
  		
	
	
	


	
	// ===> Attribute referenceDir
	
	
	

	
 	/**
 	 * Get referenceDir.
 	 * @return referenceDir as std::vector<std::vector<Angle > >
 	 */
 	std::vector<std::vector<Angle > > getReferenceDir() const;
	
 
 	
 	
 	/**
 	 * Set referenceDir with the specified std::vector<std::vector<Angle > >.
 	 * @param referenceDir The std::vector<std::vector<Angle > > value to which referenceDir is to be set.
 	 
 		
 			
 	 */
 	void setReferenceDir (std::vector<std::vector<Angle > > referenceDir);
  		
	
	
	


	
	// ===> Attribute time, which is optional
	
	
	
	/**
	 * The attribute time is optional. Return true if this attribute exists.
	 * @return true if and only if the time attribute exists. 
	 */
	bool isTimeExists() const;
	

	
 	/**
 	 * Get time, which is optional.
 	 * @return time as ArrayTime
 	 * @throws IllegalAccessException If time does not exist.
 	 */
 	ArrayTime getTime() const;
	
 
 	
 	
 	/**
 	 * Set time with the specified ArrayTime.
 	 * @param time The ArrayTime value to which time is to be set.
 	 
 		
 	 */
 	void setTime (ArrayTime time);
		
	
	
	
	/**
	 * Mark time, which is an optional field, as non-existent.
	 */
	void clearTime ();
	


	
	// ===> Attribute code, which is optional
	
	
	
	/**
	 * The attribute code is optional. Return true if this attribute exists.
	 * @return true if and only if the code attribute exists. 
	 */
	bool isCodeExists() const;
	

	
 	/**
 	 * Get code, which is optional.
 	 * @return code as std::string
 	 * @throws IllegalAccessException If code does not exist.
 	 */
 	std::string getCode() const;
	
 
 	
 	
 	/**
 	 * Set code with the specified std::string.
 	 * @param code The std::string value to which code is to be set.
 	 
 		
 	 */
 	void setCode (std::string code);
		
	
	
	
	/**
	 * Mark code, which is an optional field, as non-existent.
	 */
	void clearCode ();
	


	
	// ===> Attribute directionCode, which is optional
	
	
	
	/**
	 * The attribute directionCode is optional. Return true if this attribute exists.
	 * @return true if and only if the directionCode attribute exists. 
	 */
	bool isDirectionCodeExists() const;
	

	
 	/**
 	 * Get directionCode, which is optional.
 	 * @return directionCode as DirectionReferenceCodeMod::DirectionReferenceCode
 	 * @throws IllegalAccessException If directionCode does not exist.
 	 */
 	DirectionReferenceCodeMod::DirectionReferenceCode getDirectionCode() const;
	
 
 	
 	
 	/**
 	 * Set directionCode with the specified DirectionReferenceCodeMod::DirectionReferenceCode.
 	 * @param directionCode The DirectionReferenceCodeMod::DirectionReferenceCode value to which directionCode is to be set.
 	 
 		
 	 */
 	void setDirectionCode (DirectionReferenceCodeMod::DirectionReferenceCode directionCode);
		
	
	
	
	/**
	 * Mark directionCode, which is an optional field, as non-existent.
	 */
	void clearDirectionCode ();
	


	
	// ===> Attribute directionEquinox, which is optional
	
	
	
	/**
	 * The attribute directionEquinox is optional. Return true if this attribute exists.
	 * @return true if and only if the directionEquinox attribute exists. 
	 */
	bool isDirectionEquinoxExists() const;
	

	
 	/**
 	 * Get directionEquinox, which is optional.
 	 * @return directionEquinox as ArrayTime
 	 * @throws IllegalAccessException If directionEquinox does not exist.
 	 */
 	ArrayTime getDirectionEquinox() const;
	
 
 	
 	
 	/**
 	 * Set directionEquinox with the specified ArrayTime.
 	 * @param directionEquinox The ArrayTime value to which directionEquinox is to be set.
 	 
 		
 	 */
 	void setDirectionEquinox (ArrayTime directionEquinox);
		
	
	
	
	/**
	 * Mark directionEquinox, which is an optional field, as non-existent.
	 */
	void clearDirectionEquinox ();
	


	
	// ===> Attribute assocNature, which is optional
	
	
	
	/**
	 * The attribute assocNature is optional. Return true if this attribute exists.
	 * @return true if and only if the assocNature attribute exists. 
	 */
	bool isAssocNatureExists() const;
	

	
 	/**
 	 * Get assocNature, which is optional.
 	 * @return assocNature as std::string
 	 * @throws IllegalAccessException If assocNature does not exist.
 	 */
 	std::string getAssocNature() const;
	
 
 	
 	
 	/**
 	 * Set assocNature with the specified std::string.
 	 * @param assocNature The std::string value to which assocNature is to be set.
 	 
 		
 	 */
 	void setAssocNature (std::string assocNature);
		
	
	
	
	/**
	 * Mark assocNature, which is an optional field, as non-existent.
	 */
	void clearAssocNature ();
	


	////////////////////////////////
	// Extrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute assocFieldId, which is optional
	
	
	
	/**
	 * The attribute assocFieldId is optional. Return true if this attribute exists.
	 * @return true if and only if the assocFieldId attribute exists. 
	 */
	bool isAssocFieldIdExists() const;
	

	
 	/**
 	 * Get assocFieldId, which is optional.
 	 * @return assocFieldId as Tag
 	 * @throws IllegalAccessException If assocFieldId does not exist.
 	 */
 	Tag getAssocFieldId() const;
	
 
 	
 	
 	/**
 	 * Set assocFieldId with the specified Tag.
 	 * @param assocFieldId The Tag value to which assocFieldId is to be set.
 	 
 		
 	 */
 	void setAssocFieldId (Tag assocFieldId);
		
	
	
	
	/**
	 * Mark assocFieldId, which is an optional field, as non-existent.
	 */
	void clearAssocFieldId ();
	


	
	// ===> Attribute ephemerisId, which is optional
	
	
	
	/**
	 * The attribute ephemerisId is optional. Return true if this attribute exists.
	 * @return true if and only if the ephemerisId attribute exists. 
	 */
	bool isEphemerisIdExists() const;
	

	
 	/**
 	 * Get ephemerisId, which is optional.
 	 * @return ephemerisId as int
 	 * @throws IllegalAccessException If ephemerisId does not exist.
 	 */
 	int getEphemerisId() const;
	
 
 	
 	
 	/**
 	 * Set ephemerisId with the specified int.
 	 * @param ephemerisId The int value to which ephemerisId is to be set.
 	 
 		
 	 */
 	void setEphemerisId (int ephemerisId);
		
	
	
	
	/**
	 * Mark ephemerisId, which is an optional field, as non-existent.
	 */
	void clearEphemerisId ();
	


	
	// ===> Attribute sourceId, which is optional
	
	
	
	/**
	 * The attribute sourceId is optional. Return true if this attribute exists.
	 * @return true if and only if the sourceId attribute exists. 
	 */
	bool isSourceIdExists() const;
	

	
 	/**
 	 * Get sourceId, which is optional.
 	 * @return sourceId as int
 	 * @throws IllegalAccessException If sourceId does not exist.
 	 */
 	int getSourceId() const;
	
 
 	
 	
 	/**
 	 * Set sourceId with the specified int.
 	 * @param sourceId The int value to which sourceId is to be set.
 	 
 		
 	 */
 	void setSourceId (int sourceId);
		
	
	
	
	/**
	 * Mark sourceId, which is an optional field, as non-existent.
	 */
	void clearSourceId ();
	


	///////////
	// Links //
	///////////
	
	

	
		
	// ===> Slice link from a row of Field table to a collection of row of Ephemeris table.
	
	/**
	 * Get the collection of row in the Ephemeris table having ephemerisId == this.ephemerisId
	 * 
	 * @return a vector of EphemerisRow *
	 */
	std::vector <EphemerisRow *> getEphemeriss();
	
	

	

	

	
		
	// ===> Slice link from a row of Field table to a collection of row of Source table.
	
	/**
	 * Get the collection of row in the Source table having sourceId == this.sourceId
	 * 
	 * @return a vector of SourceRow *
	 */
	std::vector <SourceRow *> getSources();
	
	

	

	

	
		
	/**
	 * assocFieldId pointer to the row in the Field table having Field.assocFieldId == assocFieldId
	 * @return a FieldRow*
	 * 
	 
	 * throws IllegalAccessException
	 
	 */
	 FieldRow* getFieldUsingAssocFieldId();
	 

	

	
	
	
	/**
	 * Compare each mandatory attribute except the autoincrementable one of this FieldRow with 
	 * the corresponding parameters and return true if there is a match and false otherwise.
	 	
	 * @param fieldName
	    
	 * @param numPoly
	    
	 * @param delayDir
	    
	 * @param phaseDir
	    
	 * @param referenceDir
	    
	 */ 
	bool compareNoAutoInc(std::string fieldName, int numPoly, std::vector<std::vector<Angle > > delayDir, std::vector<std::vector<Angle > > phaseDir, std::vector<std::vector<Angle > > referenceDir);
	
	

	
	/**
	 * Compare each mandatory value (i.e. not in the key) attribute  with 
	 * the corresponding parameters and return true if there is a match and false otherwise.
	 	
	 * @param fieldName
	    
	 * @param numPoly
	    
	 * @param delayDir
	    
	 * @param phaseDir
	    
	 * @param referenceDir
	    
	 */ 
	bool compareRequiredValue(std::string fieldName, int numPoly, std::vector<std::vector<Angle > > delayDir, std::vector<std::vector<Angle > > phaseDir, std::vector<std::vector<Angle > > referenceDir); 
		 
	
	/**
	 * Return true if all required attributes of the value part are equal to their homologues
	 * in x and false otherwise.
	 *
	 * @param x a pointer on the FieldRow whose required attributes of the value part 
	 * will be compared with those of this.
	 * @return a boolean.
	 */
	bool equalByRequiredValue(FieldRow* x) ;
	
#ifndef WITHOUT_ACS
	/**
	 * Return this row in the form of an IDL struct.
	 * @return The values of this row as a FieldRowIDL struct.
	 */
	asdmIDL::FieldRowIDL *toIDL() const;
	
	/**
	 * Define the content of a FieldRowIDL struct from the values
	 * found in this row.
	 *
	 * @param x a reference to the FieldRowIDL struct to be set.
	 *
	 */
	 void toIDL(asdmIDL::FieldRowIDL& x) const;
#endif
	
#ifndef WITHOUT_ACS
	/**
	 * Fill the values of this row from the IDL struct FieldRowIDL.
	 * @param x The IDL struct containing the values used to fill this row.
	 * @throws ConversionException
	 */
	void setFromIDL (asdmIDL::FieldRowIDL x) ;
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

	std::map<std::string, FieldAttributeFromBin> fromBinMethods;
void fieldIdFromBin( EndianIStream& eis);
void fieldNameFromBin( EndianIStream& eis);
void numPolyFromBin( EndianIStream& eis);
void delayDirFromBin( EndianIStream& eis);
void phaseDirFromBin( EndianIStream& eis);
void referenceDirFromBin( EndianIStream& eis);

void timeFromBin( EndianIStream& eis);
void codeFromBin( EndianIStream& eis);
void directionCodeFromBin( EndianIStream& eis);
void directionEquinoxFromBin( EndianIStream& eis);
void assocNatureFromBin( EndianIStream& eis);
void ephemerisIdFromBin( EndianIStream& eis);
void sourceIdFromBin( EndianIStream& eis);
void assocFieldIdFromBin( EndianIStream& eis);


	 /**
	  * Deserialize a stream of bytes read from an EndianIStream to build a PointingRow.
	  * @param eiss the EndianIStream to be read.
	  * @param table the FieldTable to which the row built by deserialization will be parented.
	  * @param attributesSeq a vector containing the names of the attributes . The elements order defines the order 
	  * in which the attributes are written in the binary serialization.
	  */
	 static FieldRow* fromBin(EndianIStream& eis, FieldTable& table, const std::vector<std::string>& attributesSeq);	 
 
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
	FieldTable &table;
	/**
	 * Whether this row has been added to the table or not.
	 */
	bool hasBeenAdded;

	// This method is used by the Table class when this row is added to the table.
	void isAdded(bool added);


	/**
	 * Create a FieldRow.
	 * <p>
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 */ 
	FieldRow (FieldTable &table);

	/**
	 * Create a FieldRow using a copy constructor mechanism.
	 * <p>
	 * Given a FieldRow row and a FieldTable table, the method creates a new
	 * FieldRow owned by table. Each attribute of the created row is a copy (deep)
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
	 FieldRow (FieldTable &table, FieldRow *row);
	 	
	////////////////////////////////
	// Intrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute fieldId
	
	

	Tag fieldId;

	
	
 	
 	/**
 	 * Set fieldId with the specified Tag value.
 	 * @param fieldId The Tag value to which fieldId is to be set.
		
 		
			
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 		
 	 */
 	void setFieldId (Tag fieldId);
  		
	

	
	// ===> Attribute fieldName
	
	

	std::string fieldName;

	
	
 	

	
	// ===> Attribute numPoly
	
	

	int numPoly;

	
	
 	

	
	// ===> Attribute delayDir
	
	

	std::vector<std::vector<Angle > > delayDir;

	
	
 	

	
	// ===> Attribute phaseDir
	
	

	std::vector<std::vector<Angle > > phaseDir;

	
	
 	

	
	// ===> Attribute referenceDir
	
	

	std::vector<std::vector<Angle > > referenceDir;

	
	
 	

	
	// ===> Attribute time, which is optional
	
	
	bool timeExists;
	

	ArrayTime time;

	
	
 	

	
	// ===> Attribute code, which is optional
	
	
	bool codeExists;
	

	std::string code;

	
	
 	

	
	// ===> Attribute directionCode, which is optional
	
	
	bool directionCodeExists;
	

	DirectionReferenceCodeMod::DirectionReferenceCode directionCode;

	
	
 	

	
	// ===> Attribute directionEquinox, which is optional
	
	
	bool directionEquinoxExists;
	

	ArrayTime directionEquinox;

	
	
 	

	
	// ===> Attribute assocNature, which is optional
	
	
	bool assocNatureExists;
	

	std::string assocNature;

	
	
 	

	////////////////////////////////
	// Extrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute assocFieldId, which is optional
	
	
	bool assocFieldIdExists;
	

	Tag assocFieldId;

	
	
 	

	
	// ===> Attribute ephemerisId, which is optional
	
	
	bool ephemerisIdExists;
	

	int ephemerisId;

	
	
 	

	
	// ===> Attribute sourceId, which is optional
	
	
	bool sourceIdExists;
	

	int sourceId;

	
	
 	

	///////////
	// Links //
	///////////
	
	
		


	

	
		


	

	
		

	 

	

	
/*
	////////////////////////////////////////////////////////////
	// binary-deserialization material from an EndianIStream  //
	////////////////////////////////////////////////////////////
	std::map<std::string, FieldAttributeFromBin> fromBinMethods;
void fieldIdFromBin( EndianIStream& eis);
void fieldNameFromBin( EndianIStream& eis);
void numPolyFromBin( EndianIStream& eis);
void delayDirFromBin( EndianIStream& eis);
void phaseDirFromBin( EndianIStream& eis);
void referenceDirFromBin( EndianIStream& eis);

void timeFromBin( EndianIStream& eis);
void codeFromBin( EndianIStream& eis);
void directionCodeFromBin( EndianIStream& eis);
void directionEquinoxFromBin( EndianIStream& eis);
void assocNatureFromBin( EndianIStream& eis);
void ephemerisIdFromBin( EndianIStream& eis);
void sourceIdFromBin( EndianIStream& eis);
void assocFieldIdFromBin( EndianIStream& eis);

*/
	
	///////////////////////////////////
	// text-deserialization material //
	///////////////////////////////////
	std::map<std::string, FieldAttributeFromText> fromTextMethods;
	
void fieldIdFromText (const std::string & s);
	
	
void fieldNameFromText (const std::string & s);
	
	
void numPolyFromText (const std::string & s);
	
	
void delayDirFromText (const std::string & s);
	
	
void phaseDirFromText (const std::string & s);
	
	
void referenceDirFromText (const std::string & s);
	

	
void timeFromText (const std::string & s);
	
	
void codeFromText (const std::string & s);
	
	
void directionCodeFromText (const std::string & s);
	
	
void directionEquinoxFromText (const std::string & s);
	
	
void assocNatureFromText (const std::string & s);
	
	
void ephemerisIdFromText (const std::string & s);
	
	
void sourceIdFromText (const std::string & s);
	
	
void assocFieldIdFromText (const std::string & s);
	
	
	
	/**
	 * Serialize this into a stream of bytes written to an EndianOSStream.
	 * @param eoss the EndianOSStream to be written to
	 */
	 void toBin(EndianOSStream& eoss);
	 	 
	 /**
	  * Deserialize a stream of bytes read from an EndianIStream to build a PointingRow.
	  * @param eiss the EndianIStream to be read.
	  * @param table the FieldTable to which the row built by deserialization will be parented.
	  * @param attributesSeq a vector containing the names of the attributes . The elements order defines the order 
	  * in which the attributes are written in the binary serialization.

	 static FieldRow* fromBin(EndianIStream& eis, FieldTable& table, const std::vector<std::string>& attributesSeq);	 
		*/
};

} // End namespace asdm

#endif /* Field_CLASS */
