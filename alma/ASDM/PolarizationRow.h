
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
 * File PolarizationRow.h
 */
 
#ifndef PolarizationRow_CLASS
#define PolarizationRow_CLASS

#include <vector>
#include <string>
#include <set>

#ifndef WITHOUT_ACS
#include <asdmIDLC.h>
#endif






	 
#include <alma/ASDM/Tag.h>
	




	

	

	
#include <alma/Enumerations/CStokesParameter.h>
	

	
#include <alma/Enumerations/CPolarizationType.h>
	



#include <alma/ASDM/ConversionException.h>
#include <alma/ASDM/NoSuchRow.h>
#include <alma/ASDM/IllegalAccessException.h>

#include <alma/ASDM/RowTransformer.h>
//#include <alma/ASDM/TableStreamReader.h>

/*\file Polarization.h
    \brief Generated from model's revision "-1", branch ""
*/

namespace asdm {

//class asdm::PolarizationTable;

	

class PolarizationRow;
typedef void (PolarizationRow::*PolarizationAttributeFromBin) (EndianIStream& eis);
typedef void (PolarizationRow::*PolarizationAttributeFromText) (const std::string& s);

/**
 * The PolarizationRow class is a row of a PolarizationTable.
 * 
 * Generated from model's revision "-1", branch ""
 *
 */
class PolarizationRow {
friend class asdm::PolarizationTable;
friend class asdm::RowTransformer<PolarizationRow>;
//friend class asdm::TableStreamReader<PolarizationTable, PolarizationRow>;

public:

	virtual ~PolarizationRow();

	/**
	 * Return the table to which this row belongs.
	 */
	PolarizationTable &getTable() const;
	
	/**
	 * Has this row been added to its table ?
	 * @return true if and only if it has been added.
	 */
	bool isAdded() const;
		
	////////////////////////////////
	// Intrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute polarizationId
	
	
	

	
 	/**
 	 * Get polarizationId.
 	 * @return polarizationId as Tag
 	 */
 	Tag getPolarizationId() const;
	
 
 	
 	
	
	


	
	// ===> Attribute numCorr
	
	
	

	
 	/**
 	 * Get numCorr.
 	 * @return numCorr as int
 	 */
 	int getNumCorr() const;
	
 
 	
 	
 	/**
 	 * Set numCorr with the specified int.
 	 * @param numCorr The int value to which numCorr is to be set.
 	 
 		
 			
 	 */
 	void setNumCorr (int numCorr);
  		
	
	
	


	
	// ===> Attribute corrType
	
	
	

	
 	/**
 	 * Get corrType.
 	 * @return corrType as std::vector<StokesParameterMod::StokesParameter >
 	 */
 	std::vector<StokesParameterMod::StokesParameter > getCorrType() const;
	
 
 	
 	
 	/**
 	 * Set corrType with the specified std::vector<StokesParameterMod::StokesParameter >.
 	 * @param corrType The std::vector<StokesParameterMod::StokesParameter > value to which corrType is to be set.
 	 
 		
 			
 	 */
 	void setCorrType (std::vector<StokesParameterMod::StokesParameter > corrType);
  		
	
	
	


	
	// ===> Attribute corrProduct
	
	
	

	
 	/**
 	 * Get corrProduct.
 	 * @return corrProduct as std::vector<std::vector<PolarizationTypeMod::PolarizationType > >
 	 */
 	std::vector<std::vector<PolarizationTypeMod::PolarizationType > > getCorrProduct() const;
	
 
 	
 	
 	/**
 	 * Set corrProduct with the specified std::vector<std::vector<PolarizationTypeMod::PolarizationType > >.
 	 * @param corrProduct The std::vector<std::vector<PolarizationTypeMod::PolarizationType > > value to which corrProduct is to be set.
 	 
 		
 			
 	 */
 	void setCorrProduct (std::vector<std::vector<PolarizationTypeMod::PolarizationType > > corrProduct);
  		
	
	
	


	////////////////////////////////
	// Extrinsic Table Attributes //
	////////////////////////////////
	
	///////////
	// Links //
	///////////
	
	
	
	
	/**
	 * Compare each mandatory attribute except the autoincrementable one of this PolarizationRow with 
	 * the corresponding parameters and return true if there is a match and false otherwise.
	 	
	 * @param numCorr
	    
	 * @param corrType
	    
	 * @param corrProduct
	    
	 */ 
	bool compareNoAutoInc(int numCorr, std::vector<StokesParameterMod::StokesParameter > corrType, std::vector<std::vector<PolarizationTypeMod::PolarizationType > > corrProduct);
	
	

	
	/**
	 * Compare each mandatory value (i.e. not in the key) attribute  with 
	 * the corresponding parameters and return true if there is a match and false otherwise.
	 	
	 * @param numCorr
	    
	 * @param corrType
	    
	 * @param corrProduct
	    
	 */ 
	bool compareRequiredValue(int numCorr, std::vector<StokesParameterMod::StokesParameter > corrType, std::vector<std::vector<PolarizationTypeMod::PolarizationType > > corrProduct); 
		 
	
	/**
	 * Return true if all required attributes of the value part are equal to their homologues
	 * in x and false otherwise.
	 *
	 * @param x a pointer on the PolarizationRow whose required attributes of the value part 
	 * will be compared with those of this.
	 * @return a boolean.
	 */
	bool equalByRequiredValue(PolarizationRow* x) ;
	
#ifndef WITHOUT_ACS
	/**
	 * Return this row in the form of an IDL struct.
	 * @return The values of this row as a PolarizationRowIDL struct.
	 */
	asdmIDL::PolarizationRowIDL *toIDL() const;
	
	/**
	 * Define the content of a PolarizationRowIDL struct from the values
	 * found in this row.
	 *
	 * @param x a reference to the PolarizationRowIDL struct to be set.
	 *
	 */
	 void toIDL(asdmIDL::PolarizationRowIDL& x) const;
#endif
	
#ifndef WITHOUT_ACS
	/**
	 * Fill the values of this row from the IDL struct PolarizationRowIDL.
	 * @param x The IDL struct containing the values used to fill this row.
	 * @throws ConversionException
	 */
	void setFromIDL (asdmIDL::PolarizationRowIDL x) ;
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

	std::map<std::string, PolarizationAttributeFromBin> fromBinMethods;
void polarizationIdFromBin( EndianIStream& eis);
void numCorrFromBin( EndianIStream& eis);
void corrTypeFromBin( EndianIStream& eis);
void corrProductFromBin( EndianIStream& eis);

	

	 /**
	  * Deserialize a stream of bytes read from an EndianIStream to build a PointingRow.
	  * @param eiss the EndianIStream to be read.
	  * @param table the PolarizationTable to which the row built by deserialization will be parented.
	  * @param attributesSeq a vector containing the names of the attributes . The elements order defines the order 
	  * in which the attributes are written in the binary serialization.
	  */
	 static PolarizationRow* fromBin(EndianIStream& eis, PolarizationTable& table, const std::vector<std::string>& attributesSeq);	 
 
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
	PolarizationTable &table;
	/**
	 * Whether this row has been added to the table or not.
	 */
	bool hasBeenAdded;

	// This method is used by the Table class when this row is added to the table.
	void isAdded(bool added);


	/**
	 * Create a PolarizationRow.
	 * <p>
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 */ 
	PolarizationRow (PolarizationTable &table);

	/**
	 * Create a PolarizationRow using a copy constructor mechanism.
	 * <p>
	 * Given a PolarizationRow row and a PolarizationTable table, the method creates a new
	 * PolarizationRow owned by table. Each attribute of the created row is a copy (deep)
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
	 PolarizationRow (PolarizationTable &table, PolarizationRow *row);
	 	
	////////////////////////////////
	// Intrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute polarizationId
	
	

	Tag polarizationId;

	
	
 	
 	/**
 	 * Set polarizationId with the specified Tag value.
 	 * @param polarizationId The Tag value to which polarizationId is to be set.
		
 		
			
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 		
 	 */
 	void setPolarizationId (Tag polarizationId);
  		
	

	
	// ===> Attribute numCorr
	
	

	int numCorr;

	
	
 	

	
	// ===> Attribute corrType
	
	

	std::vector<StokesParameterMod::StokesParameter > corrType;

	
	
 	

	
	// ===> Attribute corrProduct
	
	

	std::vector<std::vector<PolarizationTypeMod::PolarizationType > > corrProduct;

	
	
 	

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
	std::map<std::string, PolarizationAttributeFromBin> fromBinMethods;
void polarizationIdFromBin( EndianIStream& eis);
void numCorrFromBin( EndianIStream& eis);
void corrTypeFromBin( EndianIStream& eis);
void corrProductFromBin( EndianIStream& eis);

	
*/
	
	///////////////////////////////////
	// text-deserialization material //
	///////////////////////////////////
	std::map<std::string, PolarizationAttributeFromText> fromTextMethods;
	
void polarizationIdFromText (const std::string & s);
	
	
void numCorrFromText (const std::string & s);
	
	
void corrTypeFromText (const std::string & s);
	
	
void corrProductFromText (const std::string & s);
	

		
	
	/**
	 * Serialize this into a stream of bytes written to an EndianOSStream.
	 * @param eoss the EndianOSStream to be written to
	 */
	 void toBin(EndianOSStream& eoss);
	 	 
	 /**
	  * Deserialize a stream of bytes read from an EndianIStream to build a PointingRow.
	  * @param eiss the EndianIStream to be read.
	  * @param table the PolarizationTable to which the row built by deserialization will be parented.
	  * @param attributesSeq a vector containing the names of the attributes . The elements order defines the order 
	  * in which the attributes are written in the binary serialization.

	 static PolarizationRow* fromBin(EndianIStream& eis, PolarizationTable& table, const std::vector<std::string>& attributesSeq);	 
		*/
};

} // End namespace asdm

#endif /* Polarization_CLASS */
