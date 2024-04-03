
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
 * File GainTrackingRow.h
 */
 
#ifndef GainTrackingRow_CLASS
#define GainTrackingRow_CLASS

#include <vector>
#include <string>
#include <set>

#ifndef WITHOUT_ACS
#include <asdmIDLC.h>
#endif






	 
#include <alma/ASDM/ArrayTimeInterval.h>
	

	 
#include <alma/ASDM/Tag.h>
	

	
#include <alma/ASDM/ComplexWrapper.h>
	




	

	

	

	
#include <alma/Enumerations/CPolarizationType.h>
	

	

	

	

	



#include <alma/ASDM/ConversionException.h>
#include <alma/ASDM/NoSuchRow.h>
#include <alma/ASDM/IllegalAccessException.h>

#include <alma/ASDM/RowTransformer.h>
//#include <alma/ASDM/TableStreamReader.h>

/*\file GainTracking.h
    \brief Generated from model's revision "-1", branch ""
*/

namespace asdm {

//class asdm::GainTrackingTable;


// class asdm::AntennaRow;
class AntennaRow;

// class asdm::SpectralWindowRow;
class SpectralWindowRow;

// class asdm::FeedRow;
class FeedRow;
	

class GainTrackingRow;
typedef void (GainTrackingRow::*GainTrackingAttributeFromBin) (EndianIStream& eis);
typedef void (GainTrackingRow::*GainTrackingAttributeFromText) (const std::string& s);

/**
 * The GainTrackingRow class is a row of a GainTrackingTable.
 * 
 * Generated from model's revision "-1", branch ""
 *
 */
class GainTrackingRow {
friend class asdm::GainTrackingTable;
friend class asdm::RowTransformer<GainTrackingRow>;
//friend class asdm::TableStreamReader<GainTrackingTable, GainTrackingRow>;

public:

	virtual ~GainTrackingRow();

	/**
	 * Return the table to which this row belongs.
	 */
	GainTrackingTable &getTable() const;
	
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
  		
	
	
	


	
	// ===> Attribute attenuator
	
	
	

	
 	/**
 	 * Get attenuator.
 	 * @return attenuator as std::vector<float >
 	 */
 	std::vector<float > getAttenuator() const;
	
 
 	
 	
 	/**
 	 * Set attenuator with the specified std::vector<float >.
 	 * @param attenuator The std::vector<float > value to which attenuator is to be set.
 	 
 		
 			
 	 */
 	void setAttenuator (std::vector<float > attenuator);
  		
	
	
	


	
	// ===> Attribute polarizationType
	
	
	

	
 	/**
 	 * Get polarizationType.
 	 * @return polarizationType as std::vector<PolarizationTypeMod::PolarizationType >
 	 */
 	std::vector<PolarizationTypeMod::PolarizationType > getPolarizationType() const;
	
 
 	
 	
 	/**
 	 * Set polarizationType with the specified std::vector<PolarizationTypeMod::PolarizationType >.
 	 * @param polarizationType The std::vector<PolarizationTypeMod::PolarizationType > value to which polarizationType is to be set.
 	 
 		
 			
 	 */
 	void setPolarizationType (std::vector<PolarizationTypeMod::PolarizationType > polarizationType);
  		
	
	
	


	
	// ===> Attribute samplingLevel, which is optional
	
	
	
	/**
	 * The attribute samplingLevel is optional. Return true if this attribute exists.
	 * @return true if and only if the samplingLevel attribute exists. 
	 */
	bool isSamplingLevelExists() const;
	

	
 	/**
 	 * Get samplingLevel, which is optional.
 	 * @return samplingLevel as float
 	 * @throws IllegalAccessException If samplingLevel does not exist.
 	 */
 	float getSamplingLevel() const;
	
 
 	
 	
 	/**
 	 * Set samplingLevel with the specified float.
 	 * @param samplingLevel The float value to which samplingLevel is to be set.
 	 
 		
 	 */
 	void setSamplingLevel (float samplingLevel);
		
	
	
	
	/**
	 * Mark samplingLevel, which is an optional field, as non-existent.
	 */
	void clearSamplingLevel ();
	


	
	// ===> Attribute numAttFreq, which is optional
	
	
	
	/**
	 * The attribute numAttFreq is optional. Return true if this attribute exists.
	 * @return true if and only if the numAttFreq attribute exists. 
	 */
	bool isNumAttFreqExists() const;
	

	
 	/**
 	 * Get numAttFreq, which is optional.
 	 * @return numAttFreq as int
 	 * @throws IllegalAccessException If numAttFreq does not exist.
 	 */
 	int getNumAttFreq() const;
	
 
 	
 	
 	/**
 	 * Set numAttFreq with the specified int.
 	 * @param numAttFreq The int value to which numAttFreq is to be set.
 	 
 		
 	 */
 	void setNumAttFreq (int numAttFreq);
		
	
	
	
	/**
	 * Mark numAttFreq, which is an optional field, as non-existent.
	 */
	void clearNumAttFreq ();
	


	
	// ===> Attribute attFreq, which is optional
	
	
	
	/**
	 * The attribute attFreq is optional. Return true if this attribute exists.
	 * @return true if and only if the attFreq attribute exists. 
	 */
	bool isAttFreqExists() const;
	

	
 	/**
 	 * Get attFreq, which is optional.
 	 * @return attFreq as std::vector<double >
 	 * @throws IllegalAccessException If attFreq does not exist.
 	 */
 	std::vector<double > getAttFreq() const;
	
 
 	
 	
 	/**
 	 * Set attFreq with the specified std::vector<double >.
 	 * @param attFreq The std::vector<double > value to which attFreq is to be set.
 	 
 		
 	 */
 	void setAttFreq (std::vector<double > attFreq);
		
	
	
	
	/**
	 * Mark attFreq, which is an optional field, as non-existent.
	 */
	void clearAttFreq ();
	


	
	// ===> Attribute attSpectrum, which is optional
	
	
	
	/**
	 * The attribute attSpectrum is optional. Return true if this attribute exists.
	 * @return true if and only if the attSpectrum attribute exists. 
	 */
	bool isAttSpectrumExists() const;
	

	
 	/**
 	 * Get attSpectrum, which is optional.
 	 * @return attSpectrum as std::vector<Complex >
 	 * @throws IllegalAccessException If attSpectrum does not exist.
 	 */
 	std::vector<Complex > getAttSpectrum() const;
	
 
 	
 	
 	/**
 	 * Set attSpectrum with the specified std::vector<Complex >.
 	 * @param attSpectrum The std::vector<Complex > value to which attSpectrum is to be set.
 	 
 		
 	 */
 	void setAttSpectrum (std::vector<Complex > attSpectrum);
		
	
	
	
	/**
	 * Mark attSpectrum, which is an optional field, as non-existent.
	 */
	void clearAttSpectrum ();
	


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
	 

	

	

	
		
	// ===> Slice link from a row of GainTracking table to a collection of row of Feed table.
	
	/**
	 * Get the collection of row in the Feed table having feedId == this.feedId
	 * 
	 * @return a vector of FeedRow *
	 */
	std::vector <FeedRow *> getFeeds();
	
	

	

	
	
	
	/**
	 * Compare each mandatory attribute except the autoincrementable one of this GainTrackingRow with 
	 * the corresponding parameters and return true if there is a match and false otherwise.
	 	
	 * @param antennaId
	    
	 * @param spectralWindowId
	    
	 * @param timeInterval
	    
	 * @param feedId
	    
	 * @param numReceptor
	    
	 * @param attenuator
	    
	 * @param polarizationType
	    
	 */ 
	bool compareNoAutoInc(Tag antennaId, Tag spectralWindowId, ArrayTimeInterval timeInterval, int feedId, int numReceptor, std::vector<float > attenuator, std::vector<PolarizationTypeMod::PolarizationType > polarizationType);
	
	

	
	/**
	 * Compare each mandatory value (i.e. not in the key) attribute  with 
	 * the corresponding parameters and return true if there is a match and false otherwise.
	 	
	 * @param numReceptor
	    
	 * @param attenuator
	    
	 * @param polarizationType
	    
	 */ 
	bool compareRequiredValue(int numReceptor, std::vector<float > attenuator, std::vector<PolarizationTypeMod::PolarizationType > polarizationType); 
		 
	
	/**
	 * Return true if all required attributes of the value part are equal to their homologues
	 * in x and false otherwise.
	 *
	 * @param x a pointer on the GainTrackingRow whose required attributes of the value part 
	 * will be compared with those of this.
	 * @return a boolean.
	 */
	bool equalByRequiredValue(GainTrackingRow* x) ;
	
#ifndef WITHOUT_ACS
	/**
	 * Return this row in the form of an IDL struct.
	 * @return The values of this row as a GainTrackingRowIDL struct.
	 */
	asdmIDL::GainTrackingRowIDL *toIDL() const;
	
	/**
	 * Define the content of a GainTrackingRowIDL struct from the values
	 * found in this row.
	 *
	 * @param x a reference to the GainTrackingRowIDL struct to be set.
	 *
	 */
	 void toIDL(asdmIDL::GainTrackingRowIDL& x) const;
#endif
	
#ifndef WITHOUT_ACS
	/**
	 * Fill the values of this row from the IDL struct GainTrackingRowIDL.
	 * @param x The IDL struct containing the values used to fill this row.
	 * @throws ConversionException
	 */
	void setFromIDL (asdmIDL::GainTrackingRowIDL x) ;
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

	std::map<std::string, GainTrackingAttributeFromBin> fromBinMethods;
void antennaIdFromBin( EndianIStream& eis);
void spectralWindowIdFromBin( EndianIStream& eis);
void timeIntervalFromBin( EndianIStream& eis);
void feedIdFromBin( EndianIStream& eis);
void numReceptorFromBin( EndianIStream& eis);
void attenuatorFromBin( EndianIStream& eis);
void polarizationTypeFromBin( EndianIStream& eis);

void samplingLevelFromBin( EndianIStream& eis);
void numAttFreqFromBin( EndianIStream& eis);
void attFreqFromBin( EndianIStream& eis);
void attSpectrumFromBin( EndianIStream& eis);


	 /**
	  * Deserialize a stream of bytes read from an EndianIStream to build a PointingRow.
	  * @param eiss the EndianIStream to be read.
	  * @param table the GainTrackingTable to which the row built by deserialization will be parented.
	  * @param attributesSeq a vector containing the names of the attributes . The elements order defines the order 
	  * in which the attributes are written in the binary serialization.
	  */
	 static GainTrackingRow* fromBin(EndianIStream& eis, GainTrackingTable& table, const std::vector<std::string>& attributesSeq);	 
 
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
	GainTrackingTable &table;
	/**
	 * Whether this row has been added to the table or not.
	 */
	bool hasBeenAdded;

	// This method is used by the Table class when this row is added to the table.
	void isAdded(bool added);


	/**
	 * Create a GainTrackingRow.
	 * <p>
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 */ 
	GainTrackingRow (GainTrackingTable &table);

	/**
	 * Create a GainTrackingRow using a copy constructor mechanism.
	 * <p>
	 * Given a GainTrackingRow row and a GainTrackingTable table, the method creates a new
	 * GainTrackingRow owned by table. Each attribute of the created row is a copy (deep)
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
	 GainTrackingRow (GainTrackingTable &table, GainTrackingRow *row);
	 	
	////////////////////////////////
	// Intrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute timeInterval
	
	

	ArrayTimeInterval timeInterval;

	
	
 	

	
	// ===> Attribute numReceptor
	
	

	int numReceptor;

	
	
 	

	
	// ===> Attribute attenuator
	
	

	std::vector<float > attenuator;

	
	
 	

	
	// ===> Attribute polarizationType
	
	

	std::vector<PolarizationTypeMod::PolarizationType > polarizationType;

	
	
 	

	
	// ===> Attribute samplingLevel, which is optional
	
	
	bool samplingLevelExists;
	

	float samplingLevel;

	
	
 	

	
	// ===> Attribute numAttFreq, which is optional
	
	
	bool numAttFreqExists;
	

	int numAttFreq;

	
	
 	

	
	// ===> Attribute attFreq, which is optional
	
	
	bool attFreqExists;
	

	std::vector<double > attFreq;

	
	
 	

	
	// ===> Attribute attSpectrum, which is optional
	
	
	bool attSpectrumExists;
	

	std::vector<Complex > attSpectrum;

	
	
 	

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
	std::map<std::string, GainTrackingAttributeFromBin> fromBinMethods;
void antennaIdFromBin( EndianIStream& eis);
void spectralWindowIdFromBin( EndianIStream& eis);
void timeIntervalFromBin( EndianIStream& eis);
void feedIdFromBin( EndianIStream& eis);
void numReceptorFromBin( EndianIStream& eis);
void attenuatorFromBin( EndianIStream& eis);
void polarizationTypeFromBin( EndianIStream& eis);

void samplingLevelFromBin( EndianIStream& eis);
void numAttFreqFromBin( EndianIStream& eis);
void attFreqFromBin( EndianIStream& eis);
void attSpectrumFromBin( EndianIStream& eis);

*/
	
	///////////////////////////////////
	// text-deserialization material //
	///////////////////////////////////
	std::map<std::string, GainTrackingAttributeFromText> fromTextMethods;
	
void antennaIdFromText (const std::string & s);
	
	
void spectralWindowIdFromText (const std::string & s);
	
	
void timeIntervalFromText (const std::string & s);
	
	
void feedIdFromText (const std::string & s);
	
	
void numReceptorFromText (const std::string & s);
	
	
void attenuatorFromText (const std::string & s);
	
	
void polarizationTypeFromText (const std::string & s);
	

	
void samplingLevelFromText (const std::string & s);
	
	
void numAttFreqFromText (const std::string & s);
	
	
void attFreqFromText (const std::string & s);
	
	
void attSpectrumFromText (const std::string & s);
	
	
	
	/**
	 * Serialize this into a stream of bytes written to an EndianOSStream.
	 * @param eoss the EndianOSStream to be written to
	 */
	 void toBin(EndianOSStream& eoss);
	 	 
	 /**
	  * Deserialize a stream of bytes read from an EndianIStream to build a PointingRow.
	  * @param eiss the EndianIStream to be read.
	  * @param table the GainTrackingTable to which the row built by deserialization will be parented.
	  * @param attributesSeq a vector containing the names of the attributes . The elements order defines the order 
	  * in which the attributes are written in the binary serialization.

	 static GainTrackingRow* fromBin(EndianIStream& eis, GainTrackingTable& table, const std::vector<std::string>& attributesSeq);	 
		*/
};

} // End namespace asdm

#endif /* GainTracking_CLASS */
