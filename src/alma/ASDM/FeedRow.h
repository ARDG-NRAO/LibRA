
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
 * File FeedRow.h
 */
 
#ifndef FeedRow_CLASS
#define FeedRow_CLASS

#include <vector>
#include <string>
#include <set>

#ifndef WITHOUT_ACS
#include <asdmIDLC.h>
#endif






	 
#include <alma/ASDM/ArrayTimeInterval.h>
	

	 
#include <alma/ASDM/Angle.h>
	

	 
#include <alma/ASDM/Length.h>
	

	 
#include <alma/ASDM/Tag.h>
	

	
#include <alma/ASDM/ComplexWrapper.h>
	




	

	

	

	

	

	
#include <alma/Enumerations/CPolarizationType.h>
	

	

	

	

	

	

	

	

	



#include <alma/ASDM/ConversionException.h>
#include <alma/ASDM/NoSuchRow.h>
#include <alma/ASDM/IllegalAccessException.h>

#include <alma/ASDM/RowTransformer.h>
//#include <alma/ASDM/TableStreamReader.h>

/*\file Feed.h
    \brief Generated from model's revision "-1", branch ""
*/

namespace asdm {

//class asdm::FeedTable;


// class asdm::AntennaRow;
class AntennaRow;

// class asdm::SpectralWindowRow;
class SpectralWindowRow;

// class asdm::ReceiverRow;
class ReceiverRow;
	

class FeedRow;
typedef void (FeedRow::*FeedAttributeFromBin) (EndianIStream& eis);
typedef void (FeedRow::*FeedAttributeFromText) (const std::string& s);

/**
 * The FeedRow class is a row of a FeedTable.
 * 
 * Generated from model's revision "-1", branch ""
 *
 */
class FeedRow {
friend class asdm::FeedTable;
friend class asdm::RowTransformer<FeedRow>;
//friend class asdm::TableStreamReader<FeedTable, FeedRow>;

public:

	virtual ~FeedRow();

	/**
	 * Return the table to which this row belongs.
	 */
	FeedTable &getTable() const;
	
	/**
	 * Has this row been added to its table ?
	 * @return true if and only if it has been added.
	 */
	bool isAdded() const;
		
	////////////////////////////////
	// Intrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute feedId
	
	
	

	
 	/**
 	 * Get feedId.
 	 * @return feedId as int
 	 */
 	int getFeedId() const;
	
 
 	
 	
	
	


	
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
  		
	
	
	


	
	// ===> Attribute beamOffset
	
	
	

	
 	/**
 	 * Get beamOffset.
 	 * @return beamOffset as std::vector<std::vector<double > >
 	 */
 	std::vector<std::vector<double > > getBeamOffset() const;
	
 
 	
 	
 	/**
 	 * Set beamOffset with the specified std::vector<std::vector<double > >.
 	 * @param beamOffset The std::vector<std::vector<double > > value to which beamOffset is to be set.
 	 
 		
 			
 	 */
 	void setBeamOffset (std::vector<std::vector<double > > beamOffset);
  		
	
	
	


	
	// ===> Attribute focusReference
	
	
	

	
 	/**
 	 * Get focusReference.
 	 * @return focusReference as std::vector<std::vector<Length > >
 	 */
 	std::vector<std::vector<Length > > getFocusReference() const;
	
 
 	
 	
 	/**
 	 * Set focusReference with the specified std::vector<std::vector<Length > >.
 	 * @param focusReference The std::vector<std::vector<Length > > value to which focusReference is to be set.
 	 
 		
 			
 	 */
 	void setFocusReference (std::vector<std::vector<Length > > focusReference);
  		
	
	
	


	
	// ===> Attribute polarizationTypes
	
	
	

	
 	/**
 	 * Get polarizationTypes.
 	 * @return polarizationTypes as std::vector<PolarizationTypeMod::PolarizationType >
 	 */
 	std::vector<PolarizationTypeMod::PolarizationType > getPolarizationTypes() const;
	
 
 	
 	
 	/**
 	 * Set polarizationTypes with the specified std::vector<PolarizationTypeMod::PolarizationType >.
 	 * @param polarizationTypes The std::vector<PolarizationTypeMod::PolarizationType > value to which polarizationTypes is to be set.
 	 
 		
 			
 	 */
 	void setPolarizationTypes (std::vector<PolarizationTypeMod::PolarizationType > polarizationTypes);
  		
	
	
	


	
	// ===> Attribute polResponse
	
	
	

	
 	/**
 	 * Get polResponse.
 	 * @return polResponse as std::vector<std::vector<Complex > >
 	 */
 	std::vector<std::vector<Complex > > getPolResponse() const;
	
 
 	
 	
 	/**
 	 * Set polResponse with the specified std::vector<std::vector<Complex > >.
 	 * @param polResponse The std::vector<std::vector<Complex > > value to which polResponse is to be set.
 	 
 		
 			
 	 */
 	void setPolResponse (std::vector<std::vector<Complex > > polResponse);
  		
	
	
	


	
	// ===> Attribute receptorAngle
	
	
	

	
 	/**
 	 * Get receptorAngle.
 	 * @return receptorAngle as std::vector<Angle >
 	 */
 	std::vector<Angle > getReceptorAngle() const;
	
 
 	
 	
 	/**
 	 * Set receptorAngle with the specified std::vector<Angle >.
 	 * @param receptorAngle The std::vector<Angle > value to which receptorAngle is to be set.
 	 
 		
 			
 	 */
 	void setReceptorAngle (std::vector<Angle > receptorAngle);
  		
	
	
	


	
	// ===> Attribute feedNum, which is optional
	
	
	
	/**
	 * The attribute feedNum is optional. Return true if this attribute exists.
	 * @return true if and only if the feedNum attribute exists. 
	 */
	bool isFeedNumExists() const;
	

	
 	/**
 	 * Get feedNum, which is optional.
 	 * @return feedNum as int
 	 * @throws IllegalAccessException If feedNum does not exist.
 	 */
 	int getFeedNum() const;
	
 
 	
 	
 	/**
 	 * Set feedNum with the specified int.
 	 * @param feedNum The int value to which feedNum is to be set.
 	 
 		
 	 */
 	void setFeedNum (int feedNum);
		
	
	
	
	/**
	 * Mark feedNum, which is an optional field, as non-existent.
	 */
	void clearFeedNum ();
	


	
	// ===> Attribute illumOffset, which is optional
	
	
	
	/**
	 * The attribute illumOffset is optional. Return true if this attribute exists.
	 * @return true if and only if the illumOffset attribute exists. 
	 */
	bool isIllumOffsetExists() const;
	

	
 	/**
 	 * Get illumOffset, which is optional.
 	 * @return illumOffset as std::vector<Length >
 	 * @throws IllegalAccessException If illumOffset does not exist.
 	 */
 	std::vector<Length > getIllumOffset() const;
	
 
 	
 	
 	/**
 	 * Set illumOffset with the specified std::vector<Length >.
 	 * @param illumOffset The std::vector<Length > value to which illumOffset is to be set.
 	 
 		
 	 */
 	void setIllumOffset (std::vector<Length > illumOffset);
		
	
	
	
	/**
	 * Mark illumOffset, which is an optional field, as non-existent.
	 */
	void clearIllumOffset ();
	


	
	// ===> Attribute position, which is optional
	
	
	
	/**
	 * The attribute position is optional. Return true if this attribute exists.
	 * @return true if and only if the position attribute exists. 
	 */
	bool isPositionExists() const;
	

	
 	/**
 	 * Get position, which is optional.
 	 * @return position as std::vector<Length >
 	 * @throws IllegalAccessException If position does not exist.
 	 */
 	std::vector<Length > getPosition() const;
	
 
 	
 	
 	/**
 	 * Set position with the specified std::vector<Length >.
 	 * @param position The std::vector<Length > value to which position is to be set.
 	 
 		
 	 */
 	void setPosition (std::vector<Length > position);
		
	
	
	
	/**
	 * Mark position, which is an optional field, as non-existent.
	 */
	void clearPosition ();
	


	
	// ===> Attribute skyCoupling, which is optional
	
	
	
	/**
	 * The attribute skyCoupling is optional. Return true if this attribute exists.
	 * @return true if and only if the skyCoupling attribute exists. 
	 */
	bool isSkyCouplingExists() const;
	

	
 	/**
 	 * Get skyCoupling, which is optional.
 	 * @return skyCoupling as float
 	 * @throws IllegalAccessException If skyCoupling does not exist.
 	 */
 	float getSkyCoupling() const;
	
 
 	
 	
 	/**
 	 * Set skyCoupling with the specified float.
 	 * @param skyCoupling The float value to which skyCoupling is to be set.
 	 
 		
 	 */
 	void setSkyCoupling (float skyCoupling);
		
	
	
	
	/**
	 * Mark skyCoupling, which is an optional field, as non-existent.
	 */
	void clearSkyCoupling ();
	


	
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
	


	
	// ===> Attribute skyCouplingSpectrum, which is optional
	
	
	
	/**
	 * The attribute skyCouplingSpectrum is optional. Return true if this attribute exists.
	 * @return true if and only if the skyCouplingSpectrum attribute exists. 
	 */
	bool isSkyCouplingSpectrumExists() const;
	

	
 	/**
 	 * Get skyCouplingSpectrum, which is optional.
 	 * @return skyCouplingSpectrum as std::vector<float >
 	 * @throws IllegalAccessException If skyCouplingSpectrum does not exist.
 	 */
 	std::vector<float > getSkyCouplingSpectrum() const;
	
 
 	
 	
 	/**
 	 * Set skyCouplingSpectrum with the specified std::vector<float >.
 	 * @param skyCouplingSpectrum The std::vector<float > value to which skyCouplingSpectrum is to be set.
 	 
 		
 	 */
 	void setSkyCouplingSpectrum (std::vector<float > skyCouplingSpectrum);
		
	
	
	
	/**
	 * Mark skyCouplingSpectrum, which is an optional field, as non-existent.
	 */
	void clearSkyCouplingSpectrum ();
	


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
  		
	
	
	


	
	// ===> Attribute receiverId
	
	
	

	
 	/**
 	 * Get receiverId.
 	 * @return receiverId as std::vector<int> 
 	 */
 	std::vector<int>  getReceiverId() const;
	
 
 	
 	
 	/**
 	 * Set receiverId with the specified std::vector<int> .
 	 * @param receiverId The std::vector<int>  value to which receiverId is to be set.
 	 
 		
 			
 	 */
 	void setReceiverId (std::vector<int>  receiverId);
  		
	
	
	


	
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
	 

	

	
 		
 	/**
 	 * Set receiverId[i] with the specified int.
 	 * @param i The index in receiverId where to set the int value.
 	 * @param receiverId The int value to which receiverId[i] is to be set. 
	 		
 	 * @throws IndexOutOfBoundsException
  	 */
  	void setReceiverId (int i, int receiverId); 
 			
	

	
		

	// ===> Slices link from a row of Feed table to a collection of row of Receiver table.	

	/**
	 * Append a new id to receiverId
	 * @param id the int value to be appended to receiverId
	 */
	void addReceiverId(int id); 
	
	/**
	 * Append an array of ids to receiverId
	 * @param id a vector of int containing the values to append to receiverId.
	 */ 
	void addReceiverId(std::vector<int> id); 


	/**
	 * Get the collection of rows in the Receiver table having receiverId == receiverId[i]
	 * @return a vector of ReceiverRow *. 
	 */	 
	const std::vector <ReceiverRow *> getReceivers(int i);


	/** 
	 * Get the collection of rows in the Receiver table having receiverId == receiverId[i]
	 * for any i in [O..receiverId.size()-1].
	 * @return a vector of ReceiverRow *.
	 */
	const std::vector <ReceiverRow *> getReceivers();
	


	

	
	
	
	/**
	 * Compare each mandatory attribute except the autoincrementable one of this FeedRow with 
	 * the corresponding parameters and return true if there is a match and false otherwise.
	 	
	 * @param antennaId
	    
	 * @param spectralWindowId
	    
	 * @param timeInterval
	    
	 * @param numReceptor
	    
	 * @param beamOffset
	    
	 * @param focusReference
	    
	 * @param polarizationTypes
	    
	 * @param polResponse
	    
	 * @param receptorAngle
	    
	 * @param receiverId
	    
	 */ 
	bool compareNoAutoInc(Tag antennaId, Tag spectralWindowId, ArrayTimeInterval timeInterval, int numReceptor, std::vector<std::vector<double > > beamOffset, std::vector<std::vector<Length > > focusReference, std::vector<PolarizationTypeMod::PolarizationType > polarizationTypes, std::vector<std::vector<Complex > > polResponse, std::vector<Angle > receptorAngle, std::vector<int>  receiverId);
	
	

	
	/**
	 * Compare each mandatory value (i.e. not in the key) attribute  with 
	 * the corresponding parameters and return true if there is a match and false otherwise.
	 	
	 * @param numReceptor
	    
	 * @param beamOffset
	    
	 * @param focusReference
	    
	 * @param polarizationTypes
	    
	 * @param polResponse
	    
	 * @param receptorAngle
	    
	 * @param receiverId
	    
	 */ 
	bool compareRequiredValue(int numReceptor, std::vector<std::vector<double > > beamOffset, std::vector<std::vector<Length > > focusReference, std::vector<PolarizationTypeMod::PolarizationType > polarizationTypes, std::vector<std::vector<Complex > > polResponse, std::vector<Angle > receptorAngle, std::vector<int>  receiverId); 
		 
	
	/**
	 * Return true if all required attributes of the value part are equal to their homologues
	 * in x and false otherwise.
	 *
	 * @param x a pointer on the FeedRow whose required attributes of the value part 
	 * will be compared with those of this.
	 * @return a boolean.
	 */
	bool equalByRequiredValue(FeedRow* x) ;
	
#ifndef WITHOUT_ACS
	/**
	 * Return this row in the form of an IDL struct.
	 * @return The values of this row as a FeedRowIDL struct.
	 */
	asdmIDL::FeedRowIDL *toIDL() const;
	
	/**
	 * Define the content of a FeedRowIDL struct from the values
	 * found in this row.
	 *
	 * @param x a reference to the FeedRowIDL struct to be set.
	 *
	 */
	 void toIDL(asdmIDL::FeedRowIDL& x) const;
#endif
	
#ifndef WITHOUT_ACS
	/**
	 * Fill the values of this row from the IDL struct FeedRowIDL.
	 * @param x The IDL struct containing the values used to fill this row.
	 * @throws ConversionException
	 */
	void setFromIDL (asdmIDL::FeedRowIDL x) ;
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

	std::map<std::string, FeedAttributeFromBin> fromBinMethods;
void antennaIdFromBin( EndianIStream& eis);
void spectralWindowIdFromBin( EndianIStream& eis);
void timeIntervalFromBin( EndianIStream& eis);
void feedIdFromBin( EndianIStream& eis);
void numReceptorFromBin( EndianIStream& eis);
void beamOffsetFromBin( EndianIStream& eis);
void focusReferenceFromBin( EndianIStream& eis);
void polarizationTypesFromBin( EndianIStream& eis);
void polResponseFromBin( EndianIStream& eis);
void receptorAngleFromBin( EndianIStream& eis);
void receiverIdFromBin( EndianIStream& eis);

void feedNumFromBin( EndianIStream& eis);
void illumOffsetFromBin( EndianIStream& eis);
void positionFromBin( EndianIStream& eis);
void skyCouplingFromBin( EndianIStream& eis);
void numChanFromBin( EndianIStream& eis);
void skyCouplingSpectrumFromBin( EndianIStream& eis);


	 /**
	  * Deserialize a stream of bytes read from an EndianIStream to build a PointingRow.
	  * @param eiss the EndianIStream to be read.
	  * @param table the FeedTable to which the row built by deserialization will be parented.
	  * @param attributesSeq a vector containing the names of the attributes . The elements order defines the order 
	  * in which the attributes are written in the binary serialization.
	  */
	 static FeedRow* fromBin(EndianIStream& eis, FeedTable& table, const std::vector<std::string>& attributesSeq);	 
 
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
	FeedTable &table;
	/**
	 * Whether this row has been added to the table or not.
	 */
	bool hasBeenAdded;

	// This method is used by the Table class when this row is added to the table.
	void isAdded(bool added);


	/**
	 * Create a FeedRow.
	 * <p>
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 */ 
	FeedRow (FeedTable &table);

	/**
	 * Create a FeedRow using a copy constructor mechanism.
	 * <p>
	 * Given a FeedRow row and a FeedTable table, the method creates a new
	 * FeedRow owned by table. Each attribute of the created row is a copy (deep)
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
	 FeedRow (FeedTable &table, FeedRow *row);
	 	
	////////////////////////////////
	// Intrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute feedId
	
	

	int feedId;

	
	
 	
 	/**
 	 * Set feedId with the specified int value.
 	 * @param feedId The int value to which feedId is to be set.
		
 		
			
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 		
 	 */
 	void setFeedId (int feedId);
  		
	

	
	// ===> Attribute timeInterval
	
	

	ArrayTimeInterval timeInterval;

	
	
 	

	
	// ===> Attribute numReceptor
	
	

	int numReceptor;

	
	
 	

	
	// ===> Attribute beamOffset
	
	

	std::vector<std::vector<double > > beamOffset;

	
	
 	

	
	// ===> Attribute focusReference
	
	

	std::vector<std::vector<Length > > focusReference;

	
	
 	

	
	// ===> Attribute polarizationTypes
	
	

	std::vector<PolarizationTypeMod::PolarizationType > polarizationTypes;

	
	
 	

	
	// ===> Attribute polResponse
	
	

	std::vector<std::vector<Complex > > polResponse;

	
	
 	

	
	// ===> Attribute receptorAngle
	
	

	std::vector<Angle > receptorAngle;

	
	
 	

	
	// ===> Attribute feedNum, which is optional
	
	
	bool feedNumExists;
	

	int feedNum;

	
	
 	

	
	// ===> Attribute illumOffset, which is optional
	
	
	bool illumOffsetExists;
	

	std::vector<Length > illumOffset;

	
	
 	

	
	// ===> Attribute position, which is optional
	
	
	bool positionExists;
	

	std::vector<Length > position;

	
	
 	

	
	// ===> Attribute skyCoupling, which is optional
	
	
	bool skyCouplingExists;
	

	float skyCoupling;

	
	
 	

	
	// ===> Attribute numChan, which is optional
	
	
	bool numChanExists;
	

	int numChan;

	
	
 	

	
	// ===> Attribute skyCouplingSpectrum, which is optional
	
	
	bool skyCouplingSpectrumExists;
	

	std::vector<float > skyCouplingSpectrum;

	
	
 	

	////////////////////////////////
	// Extrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute antennaId
	
	

	Tag antennaId;

	
	
 	

	
	// ===> Attribute receiverId
	
	

	std::vector<int>  receiverId;

	
	
 	

	
	// ===> Attribute spectralWindowId
	
	

	Tag spectralWindowId;

	
	
 	

	///////////
	// Links //
	///////////
	
	
		

	 

	

	
		

	 

	

	
		
	

	

	
/*
	////////////////////////////////////////////////////////////
	// binary-deserialization material from an EndianIStream  //
	////////////////////////////////////////////////////////////
	std::map<std::string, FeedAttributeFromBin> fromBinMethods;
void antennaIdFromBin( EndianIStream& eis);
void spectralWindowIdFromBin( EndianIStream& eis);
void timeIntervalFromBin( EndianIStream& eis);
void feedIdFromBin( EndianIStream& eis);
void numReceptorFromBin( EndianIStream& eis);
void beamOffsetFromBin( EndianIStream& eis);
void focusReferenceFromBin( EndianIStream& eis);
void polarizationTypesFromBin( EndianIStream& eis);
void polResponseFromBin( EndianIStream& eis);
void receptorAngleFromBin( EndianIStream& eis);
void receiverIdFromBin( EndianIStream& eis);

void feedNumFromBin( EndianIStream& eis);
void illumOffsetFromBin( EndianIStream& eis);
void positionFromBin( EndianIStream& eis);
void skyCouplingFromBin( EndianIStream& eis);
void numChanFromBin( EndianIStream& eis);
void skyCouplingSpectrumFromBin( EndianIStream& eis);

*/
	
	///////////////////////////////////
	// text-deserialization material //
	///////////////////////////////////
	std::map<std::string, FeedAttributeFromText> fromTextMethods;
	
void antennaIdFromText (const std::string & s);
	
	
void spectralWindowIdFromText (const std::string & s);
	
	
void timeIntervalFromText (const std::string & s);
	
	
void feedIdFromText (const std::string & s);
	
	
void numReceptorFromText (const std::string & s);
	
	
void beamOffsetFromText (const std::string & s);
	
	
void focusReferenceFromText (const std::string & s);
	
	
void polarizationTypesFromText (const std::string & s);
	
	
void polResponseFromText (const std::string & s);
	
	
void receptorAngleFromText (const std::string & s);
	
	
void receiverIdFromText (const std::string & s);
	

	
void feedNumFromText (const std::string & s);
	
	
void illumOffsetFromText (const std::string & s);
	
	
void positionFromText (const std::string & s);
	
	
void skyCouplingFromText (const std::string & s);
	
	
void numChanFromText (const std::string & s);
	
	
void skyCouplingSpectrumFromText (const std::string & s);
	
	
	
	/**
	 * Serialize this into a stream of bytes written to an EndianOSStream.
	 * @param eoss the EndianOSStream to be written to
	 */
	 void toBin(EndianOSStream& eoss);
	 	 
	 /**
	  * Deserialize a stream of bytes read from an EndianIStream to build a PointingRow.
	  * @param eiss the EndianIStream to be read.
	  * @param table the FeedTable to which the row built by deserialization will be parented.
	  * @param attributesSeq a vector containing the names of the attributes . The elements order defines the order 
	  * in which the attributes are written in the binary serialization.

	 static FeedRow* fromBin(EndianIStream& eis, FeedTable& table, const std::vector<std::string>& attributesSeq);	 
		*/
};

} // End namespace asdm

#endif /* Feed_CLASS */
