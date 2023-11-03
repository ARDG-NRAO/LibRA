
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
 * File DataDescriptionRow.h
 */
 
#ifndef DataDescriptionRow_CLASS
#define DataDescriptionRow_CLASS

#include <vector>
#include <string>
#include <set>

#ifndef WITHOUT_ACS
#include <asdmIDLC.h>
#endif






	 
#include <alma/ASDM/Tag.h>
	




	



#include <alma/ASDM/ConversionException.h>
#include <alma/ASDM/NoSuchRow.h>
#include <alma/ASDM/IllegalAccessException.h>

#include <alma/ASDM/RowTransformer.h>
//#include <alma/ASDM/TableStreamReader.h>

/*\file DataDescription.h
    \brief Generated from model's revision "1.62", branch "HEAD"
*/

namespace asdm {

//class asdm::DataDescriptionTable;


// class asdm::PolarizationRow;
class PolarizationRow;

// class asdm::HolographyRow;
class HolographyRow;

// class asdm::SpectralWindowRow;
class SpectralWindowRow;
	
// class asdm::PulsarRow;
class PulsarRow;

class DataDescriptionRow;
typedef void (DataDescriptionRow::*DataDescriptionAttributeFromBin) (EndianIStream& eis);
typedef void (DataDescriptionRow::*DataDescriptionAttributeFromText) (const std::string& s);

/**
 * The DataDescriptionRow class is a row of a DataDescriptionTable.
 * 
 * Generated from model's revision "1.62", branch "HEAD"
 *
 */
class DataDescriptionRow {
friend class asdm::DataDescriptionTable;
friend class asdm::RowTransformer<DataDescriptionRow>;
//friend class asdm::TableStreamReader<DataDescriptionTable, DataDescriptionRow>;

public:

	virtual ~DataDescriptionRow();

	/**
	 * Return the table to which this row belongs.
	 */
	DataDescriptionTable &getTable() const;
	
	/**
	 * Has this row been added to its table ?
	 * @return true if and only if it has been added.
	 */
	bool isAdded() const;
		
	////////////////////////////////
	// Intrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute dataDescriptionId
	
	
	

	
 	/**
 	 * Get dataDescriptionId.
 	 * @return dataDescriptionId as Tag
 	 */
 	Tag getDataDescriptionId() const;
	
 
 	
 	
	
	


	////////////////////////////////
	// Extrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute polOrHoloId
	
	
	

	
 	/**
 	 * Get polOrHoloId.
 	 * @return polOrHoloId as Tag
 	 */
 	Tag getPolOrHoloId() const;
	
 
 	
 	
 	/**
 	 * Set polOrHoloId with the specified Tag.
 	 * @param polOrHoloId The Tag value to which polOrHoloId is to be set.
 	 
 		
 			
 	 */
 	void setPolOrHoloId (Tag polOrHoloId);
  		
	
	
	


	
	// ===> Attribute spectralWindowId
	
	
	

	
 	/**
 	 * Get spectralWindowId.
 	 * @return spectralWindowId as Tag
 	 */
 	Tag getSpectralWindowId() const;
	
 
 	
 	
 	/**
 	 * Set spectralWindowId with the specified Tag.
 	 * @param spectralWindowId The Tag value to which spectralWindowId is to be set.
 	 
 		
 			
 	 */
 	void setSpectralWindowId (Tag spectralWindowId);
  		
	
        ////////////////////////////////
        // Extrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute pulsarId, which is optional
	
	/**
 	 * The attribute pulsarId is optional. Return true if this attribute exists.
 	 * @return true if and only if the pulsarId attribute exists.
 	 */
	bool isPulsarIdExists() const;


	/**
 	 * Get pulsarId, which is optional.
 	 * @return pulsarId as Tag
 	 * @throws IllegalAccessException If pulsarId does not exist.
 	 */
 	Tag getPulsarId() const; 


	/**
	 * Set pulsarId with the specified Tag.
	 * @param pulsarId The Tag value to which pulsarId is to be set.
	 */
	void setPulsarId (Tag pulsarId);


	/**
 	 * Mark pulsarId, which is an optional field, as non-existent.
 	 */
	void clearPulsarId ();


	///////////
	// Links //
	///////////
	
	
	
	/**
	 * Returns pointer to the row in the Polarization table having Polarization.polarizationId == polOrHoloId
	 * @return a PolarizationRow* or 0 if polOrHoloId is an Holography Tag.
	 * 	 
	 */
	 PolarizationRow* getPolarizationUsingPolOrHoloId();
	 
	 /**
	 * Returns pointer to the row in the Holograpy table having Holography.holographyId == polOrHoloId
	 * @return a HolographyRow* or 0 if polOrHoloId is an Polarization Tag.
	 * 	 
	 */
	 HolographyRow* getHolographyUsingPolOrHoloId();
		
		
	/**
	 * spectralWindowId pointer to the row in the SpectralWindow table having SpectralWindow.spectralWindowId == spectralWindowId
	 * @return a SpectralWindowRow*
	 * 	 
	 */
	 SpectralWindowRow* getSpectralWindowUsingSpectralWindowId();
	 

	/**
 	 * pulsarId pointer to the row in the Pulsar table having Pulsar.pulsarId == pulsarId
 	 * @return a PulsarRow*
         *
         * throws IllegalAccessException
 	 */
	 PulsarRow* getPulsarUsingPulsarId();	

	
	
	
	/**
	 * Compare each mandatory attribute except the autoincrementable one of this DataDescriptionRow with 
	 * the corresponding parameters and return true if there is a match and false otherwise.
	 	
	 * @param polOrHoloId
	    
	 * @param spectralWindowId
	    
	 */ 
	bool compareNoAutoInc(Tag polOrHoloId, Tag spectralWindowId);
	
	

	
	/**
	 * Compare each mandatory value (i.e. not in the key) attribute  with 
	 * the corresponding parameters and return true if there is a match and false otherwise.
	 	
	 * @param polOrHoloId
	    
	 * @param spectralWindowId
	    
	 */ 
	bool compareRequiredValue(Tag polOrHoloId, Tag spectralWindowId); 
		 
	
	/**
	 * Return true if all required attributes of the value part are equal to their homologues
	 * in x and false otherwise.
	 *
	 * @param x a pointer on the DataDescriptionRow whose required attributes of the value part 
	 * will be compared with those of this.
	 * @return a boolean.
	 */
	bool equalByRequiredValue(DataDescriptionRow* x) ;
	
#ifndef WITHOUT_ACS
	/**
	 * Return this row in the form of an IDL struct.
	 * @return The values of this row as a DataDescriptionRowIDL struct.
	 */
	asdmIDL::DataDescriptionRowIDL *toIDL() const;

	/**
	 * Define the content of a DataDescriptionRowIDL struct from the values
	 * found in this row.
	 *
	 * @param x a reference to the DataDescriptionRowIDL struct to be set.
	 *
	 */
	void toIDL(asdmIDL::DataDescriptionRowIDL& x) const;
#endif
	
#ifndef WITHOUT_ACS
	/**
	 * Fill the values of this row from the IDL struct DataDescriptionRowIDL.
	 * @param x The IDL struct containing the values used to fill this row.
	 * @throws ConversionException
	 */
	void setFromIDL (asdmIDL::DataDescriptionRowIDL x) ;
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

	std::map<std::string, DataDescriptionAttributeFromBin> fromBinMethods;
void dataDescriptionIdFromBin( EndianIStream& eis);
void polOrHoloIdFromBin( EndianIStream& eis);
void spectralWindowIdFromBin( EndianIStream& eis);
void pulsarIdFromBin( EndianIStream& eis);
	

	 /**
	  * Deserialize a stream of bytes read from an EndianIStream to build a PointingRow.
	  * @param eiss the EndianIStream to be read.
	  * @param table the DataDescriptionTable to which the row built by deserialization will be parented.
	  * @param attributesSeq a vector containing the names of the attributes . The elements order defines the order 
	  * in which the attributes are written in the binary serialization.
	  */
	 static DataDescriptionRow* fromBin(EndianIStream& eis, DataDescriptionTable& table, const std::vector<std::string>& attributesSeq);	 
 
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
	DataDescriptionTable &table;
	/**
	 * Whether this row has been added to the table or not.
	 */
	bool hasBeenAdded;

	// This method is used by the Table class when this row is added to the table.
	void isAdded(bool added);


	/**
	 * Create a DataDescriptionRow.
	 * <p>
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 */ 
	DataDescriptionRow (DataDescriptionTable &table);

	/**
	 * Create a DataDescriptionRow using a copy constructor mechanism.
	 * <p>
	 * Given a DataDescriptionRow row and a DataDescriptionTable table, the method creates a new
	 * DataDescriptionRow owned by table. Each attribute of the created row is a copy (deep)
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
	 DataDescriptionRow (DataDescriptionTable &table, DataDescriptionRow *row);
	 	
	////////////////////////////////
	// Intrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute dataDescriptionId
	
	

	Tag dataDescriptionId;

	
	
 	
 	/**
 	 * Set dataDescriptionId with the specified Tag value.
 	 * @param dataDescriptionId The Tag value to which dataDescriptionId is to be set.
		
 		
			
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 		
 	 */
 	void setDataDescriptionId (Tag dataDescriptionId);
  		
	

	////////////////////////////////
	// Extrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute polOrHoloId
	
	

	Tag polOrHoloId;

	
	
 	

	
	// ===> Attribute spectralWindowId
	
	

	Tag spectralWindowId;

	
	
 	
	// ===> Attribute pulsarId, which is optional
	

	bool pulsarIdExists;

	Tag pulsarId;


	///////////
	// Links //
	///////////
	
	
		

	 

	

	
		

	 

	

	
/*
	////////////////////////////////////////////////////////////
	// binary-deserialization material from an EndianIStream  //
	////////////////////////////////////////////////////////////
	std::map<std::string, DataDescriptionAttributeFromBin> fromBinMethods;
void dataDescriptionIdFromBin( EndianIStream& eis);
void polOrHoloIdFromBin( EndianIStream& eis);
void spectralWindowIdFromBin( EndianIStream& eis);
void pulsarIdFromBin( EndianIStream& eis);

	
*/
	
	///////////////////////////////////
	// text-deserialization material //
	///////////////////////////////////
	std::map<std::string, DataDescriptionAttributeFromText> fromTextMethods;
	
void dataDescriptionIdFromText (const std::string & s);
	
	
void polOrHoloIdFromText (const std::string & s);
	
	
void spectralWindowIdFromText (const std::string & s);
	
void pulsarIdFromText (const std::string & s);
		
	
	/**
	 * Serialize this into a stream of bytes written to an EndianOSStream.
	 * @param eoss the EndianOSStream to be written to
	 */
	 void toBin(EndianOSStream& eoss);
	 	 
	 /**
	  * Deserialize a stream of bytes read from an EndianIStream to build a PointingRow.
	  * @param eiss the EndianIStream to be read.
	  * @param table the DataDescriptionTable to which the row built by deserialization will be parented.
	  * @param attributesSeq a vector containing the names of the attributes . The elements order defines the order 
	  * in which the attributes are written in the binary serialization.

	 static DataDescriptionRow* fromBin(EndianIStream& eis, DataDescriptionTable& table, const std::vector<std::string>& attributesSeq);	 
		*/
};

} // End namespace asdm

#endif /* DataDescription_CLASS */
