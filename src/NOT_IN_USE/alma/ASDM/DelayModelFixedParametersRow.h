
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
 * File DelayModelFixedParametersRow.h
 */
 
#ifndef DelayModelFixedParametersRow_CLASS
#define DelayModelFixedParametersRow_CLASS

#include <vector>
#include <string>
#include <set>

#ifndef WITHOUT_ACS
#include <asdmIDLC.h>
#endif






	 
#include <alma/ASDM/Speed.h>
	

	 
#include <alma/ASDM/AngularRate.h>
	

	 
#include <alma/ASDM/Length.h>
	

	 
#include <alma/ASDM/Tag.h>
	




	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	



#include <alma/ASDM/ConversionException.h>
#include <alma/ASDM/NoSuchRow.h>
#include <alma/ASDM/IllegalAccessException.h>

#include <alma/ASDM/RowTransformer.h>
//#include <alma/ASDM/TableStreamReader.h>

/*\file DelayModelFixedParameters.h
    \brief Generated from model's revision "-1", branch ""
*/

namespace asdm {

//class asdm::DelayModelFixedParametersTable;


// class asdm::ExecBlockRow;
class ExecBlockRow;
	

class DelayModelFixedParametersRow;
typedef void (DelayModelFixedParametersRow::*DelayModelFixedParametersAttributeFromBin) (EndianIStream& eis);
typedef void (DelayModelFixedParametersRow::*DelayModelFixedParametersAttributeFromText) (const std::string& s);

/**
 * The DelayModelFixedParametersRow class is a row of a DelayModelFixedParametersTable.
 * 
 * Generated from model's revision "-1", branch ""
 *
 */
class DelayModelFixedParametersRow {
friend class asdm::DelayModelFixedParametersTable;
friend class asdm::RowTransformer<DelayModelFixedParametersRow>;
//friend class asdm::TableStreamReader<DelayModelFixedParametersTable, DelayModelFixedParametersRow>;

public:

	virtual ~DelayModelFixedParametersRow();

	/**
	 * Return the table to which this row belongs.
	 */
	DelayModelFixedParametersTable &getTable() const;
	
	/**
	 * Has this row been added to its table ?
	 * @return true if and only if it has been added.
	 */
	bool isAdded() const;
		
	////////////////////////////////
	// Intrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute delayModelFixedParametersId
	
	
	

	
 	/**
 	 * Get delayModelFixedParametersId.
 	 * @return delayModelFixedParametersId as Tag
 	 */
 	Tag getDelayModelFixedParametersId() const;
	
 
 	
 	
	
	


	
	// ===> Attribute delayModelVersion
	
	
	

	
 	/**
 	 * Get delayModelVersion.
 	 * @return delayModelVersion as std::string
 	 */
 	std::string getDelayModelVersion() const;
	
 
 	
 	
 	/**
 	 * Set delayModelVersion with the specified std::string.
 	 * @param delayModelVersion The std::string value to which delayModelVersion is to be set.
 	 
 		
 			
 	 */
 	void setDelayModelVersion (std::string delayModelVersion);
  		
	
	
	


	
	// ===> Attribute gaussConstant, which is optional
	
	
	
	/**
	 * The attribute gaussConstant is optional. Return true if this attribute exists.
	 * @return true if and only if the gaussConstant attribute exists. 
	 */
	bool isGaussConstantExists() const;
	

	
 	/**
 	 * Get gaussConstant, which is optional.
 	 * @return gaussConstant as AngularRate
 	 * @throws IllegalAccessException If gaussConstant does not exist.
 	 */
 	AngularRate getGaussConstant() const;
	
 
 	
 	
 	/**
 	 * Set gaussConstant with the specified AngularRate.
 	 * @param gaussConstant The AngularRate value to which gaussConstant is to be set.
 	 
 		
 	 */
 	void setGaussConstant (AngularRate gaussConstant);
		
	
	
	
	/**
	 * Mark gaussConstant, which is an optional field, as non-existent.
	 */
	void clearGaussConstant ();
	


	
	// ===> Attribute newtonianConstant, which is optional
	
	
	
	/**
	 * The attribute newtonianConstant is optional. Return true if this attribute exists.
	 * @return true if and only if the newtonianConstant attribute exists. 
	 */
	bool isNewtonianConstantExists() const;
	

	
 	/**
 	 * Get newtonianConstant, which is optional.
 	 * @return newtonianConstant as double
 	 * @throws IllegalAccessException If newtonianConstant does not exist.
 	 */
 	double getNewtonianConstant() const;
	
 
 	
 	
 	/**
 	 * Set newtonianConstant with the specified double.
 	 * @param newtonianConstant The double value to which newtonianConstant is to be set.
 	 
 		
 	 */
 	void setNewtonianConstant (double newtonianConstant);
		
	
	
	
	/**
	 * Mark newtonianConstant, which is an optional field, as non-existent.
	 */
	void clearNewtonianConstant ();
	


	
	// ===> Attribute gravity, which is optional
	
	
	
	/**
	 * The attribute gravity is optional. Return true if this attribute exists.
	 * @return true if and only if the gravity attribute exists. 
	 */
	bool isGravityExists() const;
	

	
 	/**
 	 * Get gravity, which is optional.
 	 * @return gravity as double
 	 * @throws IllegalAccessException If gravity does not exist.
 	 */
 	double getGravity() const;
	
 
 	
 	
 	/**
 	 * Set gravity with the specified double.
 	 * @param gravity The double value to which gravity is to be set.
 	 
 		
 	 */
 	void setGravity (double gravity);
		
	
	
	
	/**
	 * Mark gravity, which is an optional field, as non-existent.
	 */
	void clearGravity ();
	


	
	// ===> Attribute earthFlattening, which is optional
	
	
	
	/**
	 * The attribute earthFlattening is optional. Return true if this attribute exists.
	 * @return true if and only if the earthFlattening attribute exists. 
	 */
	bool isEarthFlatteningExists() const;
	

	
 	/**
 	 * Get earthFlattening, which is optional.
 	 * @return earthFlattening as double
 	 * @throws IllegalAccessException If earthFlattening does not exist.
 	 */
 	double getEarthFlattening() const;
	
 
 	
 	
 	/**
 	 * Set earthFlattening with the specified double.
 	 * @param earthFlattening The double value to which earthFlattening is to be set.
 	 
 		
 	 */
 	void setEarthFlattening (double earthFlattening);
		
	
	
	
	/**
	 * Mark earthFlattening, which is an optional field, as non-existent.
	 */
	void clearEarthFlattening ();
	


	
	// ===> Attribute earthRadius, which is optional
	
	
	
	/**
	 * The attribute earthRadius is optional. Return true if this attribute exists.
	 * @return true if and only if the earthRadius attribute exists. 
	 */
	bool isEarthRadiusExists() const;
	

	
 	/**
 	 * Get earthRadius, which is optional.
 	 * @return earthRadius as Length
 	 * @throws IllegalAccessException If earthRadius does not exist.
 	 */
 	Length getEarthRadius() const;
	
 
 	
 	
 	/**
 	 * Set earthRadius with the specified Length.
 	 * @param earthRadius The Length value to which earthRadius is to be set.
 	 
 		
 	 */
 	void setEarthRadius (Length earthRadius);
		
	
	
	
	/**
	 * Mark earthRadius, which is an optional field, as non-existent.
	 */
	void clearEarthRadius ();
	


	
	// ===> Attribute moonEarthMassRatio, which is optional
	
	
	
	/**
	 * The attribute moonEarthMassRatio is optional. Return true if this attribute exists.
	 * @return true if and only if the moonEarthMassRatio attribute exists. 
	 */
	bool isMoonEarthMassRatioExists() const;
	

	
 	/**
 	 * Get moonEarthMassRatio, which is optional.
 	 * @return moonEarthMassRatio as double
 	 * @throws IllegalAccessException If moonEarthMassRatio does not exist.
 	 */
 	double getMoonEarthMassRatio() const;
	
 
 	
 	
 	/**
 	 * Set moonEarthMassRatio with the specified double.
 	 * @param moonEarthMassRatio The double value to which moonEarthMassRatio is to be set.
 	 
 		
 	 */
 	void setMoonEarthMassRatio (double moonEarthMassRatio);
		
	
	
	
	/**
	 * Mark moonEarthMassRatio, which is an optional field, as non-existent.
	 */
	void clearMoonEarthMassRatio ();
	


	
	// ===> Attribute ephemerisEpoch, which is optional
	
	
	
	/**
	 * The attribute ephemerisEpoch is optional. Return true if this attribute exists.
	 * @return true if and only if the ephemerisEpoch attribute exists. 
	 */
	bool isEphemerisEpochExists() const;
	

	
 	/**
 	 * Get ephemerisEpoch, which is optional.
 	 * @return ephemerisEpoch as std::string
 	 * @throws IllegalAccessException If ephemerisEpoch does not exist.
 	 */
 	std::string getEphemerisEpoch() const;
	
 
 	
 	
 	/**
 	 * Set ephemerisEpoch with the specified std::string.
 	 * @param ephemerisEpoch The std::string value to which ephemerisEpoch is to be set.
 	 
 		
 	 */
 	void setEphemerisEpoch (std::string ephemerisEpoch);
		
	
	
	
	/**
	 * Mark ephemerisEpoch, which is an optional field, as non-existent.
	 */
	void clearEphemerisEpoch ();
	


	
	// ===> Attribute earthTideLag, which is optional
	
	
	
	/**
	 * The attribute earthTideLag is optional. Return true if this attribute exists.
	 * @return true if and only if the earthTideLag attribute exists. 
	 */
	bool isEarthTideLagExists() const;
	

	
 	/**
 	 * Get earthTideLag, which is optional.
 	 * @return earthTideLag as double
 	 * @throws IllegalAccessException If earthTideLag does not exist.
 	 */
 	double getEarthTideLag() const;
	
 
 	
 	
 	/**
 	 * Set earthTideLag with the specified double.
 	 * @param earthTideLag The double value to which earthTideLag is to be set.
 	 
 		
 	 */
 	void setEarthTideLag (double earthTideLag);
		
	
	
	
	/**
	 * Mark earthTideLag, which is an optional field, as non-existent.
	 */
	void clearEarthTideLag ();
	


	
	// ===> Attribute earthGM, which is optional
	
	
	
	/**
	 * The attribute earthGM is optional. Return true if this attribute exists.
	 * @return true if and only if the earthGM attribute exists. 
	 */
	bool isEarthGMExists() const;
	

	
 	/**
 	 * Get earthGM, which is optional.
 	 * @return earthGM as double
 	 * @throws IllegalAccessException If earthGM does not exist.
 	 */
 	double getEarthGM() const;
	
 
 	
 	
 	/**
 	 * Set earthGM with the specified double.
 	 * @param earthGM The double value to which earthGM is to be set.
 	 
 		
 	 */
 	void setEarthGM (double earthGM);
		
	
	
	
	/**
	 * Mark earthGM, which is an optional field, as non-existent.
	 */
	void clearEarthGM ();
	


	
	// ===> Attribute moonGM, which is optional
	
	
	
	/**
	 * The attribute moonGM is optional. Return true if this attribute exists.
	 * @return true if and only if the moonGM attribute exists. 
	 */
	bool isMoonGMExists() const;
	

	
 	/**
 	 * Get moonGM, which is optional.
 	 * @return moonGM as double
 	 * @throws IllegalAccessException If moonGM does not exist.
 	 */
 	double getMoonGM() const;
	
 
 	
 	
 	/**
 	 * Set moonGM with the specified double.
 	 * @param moonGM The double value to which moonGM is to be set.
 	 
 		
 	 */
 	void setMoonGM (double moonGM);
		
	
	
	
	/**
	 * Mark moonGM, which is an optional field, as non-existent.
	 */
	void clearMoonGM ();
	


	
	// ===> Attribute sunGM, which is optional
	
	
	
	/**
	 * The attribute sunGM is optional. Return true if this attribute exists.
	 * @return true if and only if the sunGM attribute exists. 
	 */
	bool isSunGMExists() const;
	

	
 	/**
 	 * Get sunGM, which is optional.
 	 * @return sunGM as double
 	 * @throws IllegalAccessException If sunGM does not exist.
 	 */
 	double getSunGM() const;
	
 
 	
 	
 	/**
 	 * Set sunGM with the specified double.
 	 * @param sunGM The double value to which sunGM is to be set.
 	 
 		
 	 */
 	void setSunGM (double sunGM);
		
	
	
	
	/**
	 * Mark sunGM, which is an optional field, as non-existent.
	 */
	void clearSunGM ();
	


	
	// ===> Attribute loveNumberH, which is optional
	
	
	
	/**
	 * The attribute loveNumberH is optional. Return true if this attribute exists.
	 * @return true if and only if the loveNumberH attribute exists. 
	 */
	bool isLoveNumberHExists() const;
	

	
 	/**
 	 * Get loveNumberH, which is optional.
 	 * @return loveNumberH as double
 	 * @throws IllegalAccessException If loveNumberH does not exist.
 	 */
 	double getLoveNumberH() const;
	
 
 	
 	
 	/**
 	 * Set loveNumberH with the specified double.
 	 * @param loveNumberH The double value to which loveNumberH is to be set.
 	 
 		
 	 */
 	void setLoveNumberH (double loveNumberH);
		
	
	
	
	/**
	 * Mark loveNumberH, which is an optional field, as non-existent.
	 */
	void clearLoveNumberH ();
	


	
	// ===> Attribute loveNumberL, which is optional
	
	
	
	/**
	 * The attribute loveNumberL is optional. Return true if this attribute exists.
	 * @return true if and only if the loveNumberL attribute exists. 
	 */
	bool isLoveNumberLExists() const;
	

	
 	/**
 	 * Get loveNumberL, which is optional.
 	 * @return loveNumberL as double
 	 * @throws IllegalAccessException If loveNumberL does not exist.
 	 */
 	double getLoveNumberL() const;
	
 
 	
 	
 	/**
 	 * Set loveNumberL with the specified double.
 	 * @param loveNumberL The double value to which loveNumberL is to be set.
 	 
 		
 	 */
 	void setLoveNumberL (double loveNumberL);
		
	
	
	
	/**
	 * Mark loveNumberL, which is an optional field, as non-existent.
	 */
	void clearLoveNumberL ();
	


	
	// ===> Attribute precessionConstant, which is optional
	
	
	
	/**
	 * The attribute precessionConstant is optional. Return true if this attribute exists.
	 * @return true if and only if the precessionConstant attribute exists. 
	 */
	bool isPrecessionConstantExists() const;
	

	
 	/**
 	 * Get precessionConstant, which is optional.
 	 * @return precessionConstant as AngularRate
 	 * @throws IllegalAccessException If precessionConstant does not exist.
 	 */
 	AngularRate getPrecessionConstant() const;
	
 
 	
 	
 	/**
 	 * Set precessionConstant with the specified AngularRate.
 	 * @param precessionConstant The AngularRate value to which precessionConstant is to be set.
 	 
 		
 	 */
 	void setPrecessionConstant (AngularRate precessionConstant);
		
	
	
	
	/**
	 * Mark precessionConstant, which is an optional field, as non-existent.
	 */
	void clearPrecessionConstant ();
	


	
	// ===> Attribute lightTime1AU, which is optional
	
	
	
	/**
	 * The attribute lightTime1AU is optional. Return true if this attribute exists.
	 * @return true if and only if the lightTime1AU attribute exists. 
	 */
	bool isLightTime1AUExists() const;
	

	
 	/**
 	 * Get lightTime1AU, which is optional.
 	 * @return lightTime1AU as double
 	 * @throws IllegalAccessException If lightTime1AU does not exist.
 	 */
 	double getLightTime1AU() const;
	
 
 	
 	
 	/**
 	 * Set lightTime1AU with the specified double.
 	 * @param lightTime1AU The double value to which lightTime1AU is to be set.
 	 
 		
 	 */
 	void setLightTime1AU (double lightTime1AU);
		
	
	
	
	/**
	 * Mark lightTime1AU, which is an optional field, as non-existent.
	 */
	void clearLightTime1AU ();
	


	
	// ===> Attribute speedOfLight, which is optional
	
	
	
	/**
	 * The attribute speedOfLight is optional. Return true if this attribute exists.
	 * @return true if and only if the speedOfLight attribute exists. 
	 */
	bool isSpeedOfLightExists() const;
	

	
 	/**
 	 * Get speedOfLight, which is optional.
 	 * @return speedOfLight as Speed
 	 * @throws IllegalAccessException If speedOfLight does not exist.
 	 */
 	Speed getSpeedOfLight() const;
	
 
 	
 	
 	/**
 	 * Set speedOfLight with the specified Speed.
 	 * @param speedOfLight The Speed value to which speedOfLight is to be set.
 	 
 		
 	 */
 	void setSpeedOfLight (Speed speedOfLight);
		
	
	
	
	/**
	 * Mark speedOfLight, which is an optional field, as non-existent.
	 */
	void clearSpeedOfLight ();
	


	
	// ===> Attribute delayModelFlags, which is optional
	
	
	
	/**
	 * The attribute delayModelFlags is optional. Return true if this attribute exists.
	 * @return true if and only if the delayModelFlags attribute exists. 
	 */
	bool isDelayModelFlagsExists() const;
	

	
 	/**
 	 * Get delayModelFlags, which is optional.
 	 * @return delayModelFlags as std::string
 	 * @throws IllegalAccessException If delayModelFlags does not exist.
 	 */
 	std::string getDelayModelFlags() const;
	
 
 	
 	
 	/**
 	 * Set delayModelFlags with the specified std::string.
 	 * @param delayModelFlags The std::string value to which delayModelFlags is to be set.
 	 
 		
 	 */
 	void setDelayModelFlags (std::string delayModelFlags);
		
	
	
	
	/**
	 * Mark delayModelFlags, which is an optional field, as non-existent.
	 */
	void clearDelayModelFlags ();
	


	////////////////////////////////
	// Extrinsic Table Attributes //
	////////////////////////////////
	
	
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
  		
	
	
	


	///////////
	// Links //
	///////////
	
	

	
		
	/**
	 * execBlockId pointer to the row in the ExecBlock table having ExecBlock.execBlockId == execBlockId
	 * @return a ExecBlockRow*
	 * 
	 
	 */
	 ExecBlockRow* getExecBlockUsingExecBlockId();
	 

	

	
	
	
	/**
	 * Compare each mandatory attribute except the autoincrementable one of this DelayModelFixedParametersRow with 
	 * the corresponding parameters and return true if there is a match and false otherwise.
	 	
	 * @param delayModelVersion
	    
	 * @param execBlockId
	    
	 */ 
	bool compareNoAutoInc(std::string delayModelVersion, Tag execBlockId);
	
	

	
	/**
	 * Compare each mandatory value (i.e. not in the key) attribute  with 
	 * the corresponding parameters and return true if there is a match and false otherwise.
	 	
	 * @param delayModelVersion
	    
	 * @param execBlockId
	    
	 */ 
	bool compareRequiredValue(std::string delayModelVersion, Tag execBlockId); 
		 
	
	/**
	 * Return true if all required attributes of the value part are equal to their homologues
	 * in x and false otherwise.
	 *
	 * @param x a pointer on the DelayModelFixedParametersRow whose required attributes of the value part 
	 * will be compared with those of this.
	 * @return a boolean.
	 */
	bool equalByRequiredValue(DelayModelFixedParametersRow* x) ;
	
#ifndef WITHOUT_ACS
	/**
	 * Return this row in the form of an IDL struct.
	 * @return The values of this row as a DelayModelFixedParametersRowIDL struct.
	 */
	asdmIDL::DelayModelFixedParametersRowIDL *toIDL() const;
	
	/**
	 * Define the content of a DelayModelFixedParametersRowIDL struct from the values
	 * found in this row.
	 *
	 * @param x a reference to the DelayModelFixedParametersRowIDL struct to be set.
	 *
	 */
	 void toIDL(asdmIDL::DelayModelFixedParametersRowIDL& x) const;
#endif
	
#ifndef WITHOUT_ACS
	/**
	 * Fill the values of this row from the IDL struct DelayModelFixedParametersRowIDL.
	 * @param x The IDL struct containing the values used to fill this row.
	 * @throws ConversionException
	 */
	void setFromIDL (asdmIDL::DelayModelFixedParametersRowIDL x) ;
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

	std::map<std::string, DelayModelFixedParametersAttributeFromBin> fromBinMethods;
void delayModelFixedParametersIdFromBin( EndianIStream& eis);
void delayModelVersionFromBin( EndianIStream& eis);
void execBlockIdFromBin( EndianIStream& eis);

void gaussConstantFromBin( EndianIStream& eis);
void newtonianConstantFromBin( EndianIStream& eis);
void gravityFromBin( EndianIStream& eis);
void earthFlatteningFromBin( EndianIStream& eis);
void earthRadiusFromBin( EndianIStream& eis);
void moonEarthMassRatioFromBin( EndianIStream& eis);
void ephemerisEpochFromBin( EndianIStream& eis);
void earthTideLagFromBin( EndianIStream& eis);
void earthGMFromBin( EndianIStream& eis);
void moonGMFromBin( EndianIStream& eis);
void sunGMFromBin( EndianIStream& eis);
void loveNumberHFromBin( EndianIStream& eis);
void loveNumberLFromBin( EndianIStream& eis);
void precessionConstantFromBin( EndianIStream& eis);
void lightTime1AUFromBin( EndianIStream& eis);
void speedOfLightFromBin( EndianIStream& eis);
void delayModelFlagsFromBin( EndianIStream& eis);


	 /**
	  * Deserialize a stream of bytes read from an EndianIStream to build a PointingRow.
	  * @param eiss the EndianIStream to be read.
	  * @param table the DelayModelFixedParametersTable to which the row built by deserialization will be parented.
	  * @param attributesSeq a vector containing the names of the attributes . The elements order defines the order 
	  * in which the attributes are written in the binary serialization.
	  */
	 static DelayModelFixedParametersRow* fromBin(EndianIStream& eis, DelayModelFixedParametersTable& table, const std::vector<std::string>& attributesSeq);	 
 
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
	DelayModelFixedParametersTable &table;
	/**
	 * Whether this row has been added to the table or not.
	 */
	bool hasBeenAdded;

	// This method is used by the Table class when this row is added to the table.
	void isAdded(bool added);


	/**
	 * Create a DelayModelFixedParametersRow.
	 * <p>
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 */ 
	DelayModelFixedParametersRow (DelayModelFixedParametersTable &table);

	/**
	 * Create a DelayModelFixedParametersRow using a copy constructor mechanism.
	 * <p>
	 * Given a DelayModelFixedParametersRow row and a DelayModelFixedParametersTable table, the method creates a new
	 * DelayModelFixedParametersRow owned by table. Each attribute of the created row is a copy (deep)
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
	 DelayModelFixedParametersRow (DelayModelFixedParametersTable &table, DelayModelFixedParametersRow *row);
	 	
	////////////////////////////////
	// Intrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute delayModelFixedParametersId
	
	

	Tag delayModelFixedParametersId;

	
	
 	
 	/**
 	 * Set delayModelFixedParametersId with the specified Tag value.
 	 * @param delayModelFixedParametersId The Tag value to which delayModelFixedParametersId is to be set.
		
 		
			
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 		
 	 */
 	void setDelayModelFixedParametersId (Tag delayModelFixedParametersId);
  		
	

	
	// ===> Attribute delayModelVersion
	
	

	std::string delayModelVersion;

	
	
 	

	
	// ===> Attribute gaussConstant, which is optional
	
	
	bool gaussConstantExists;
	

	AngularRate gaussConstant;

	
	
 	

	
	// ===> Attribute newtonianConstant, which is optional
	
	
	bool newtonianConstantExists;
	

	double newtonianConstant;

	
	
 	

	
	// ===> Attribute gravity, which is optional
	
	
	bool gravityExists;
	

	double gravity;

	
	
 	

	
	// ===> Attribute earthFlattening, which is optional
	
	
	bool earthFlatteningExists;
	

	double earthFlattening;

	
	
 	

	
	// ===> Attribute earthRadius, which is optional
	
	
	bool earthRadiusExists;
	

	Length earthRadius;

	
	
 	

	
	// ===> Attribute moonEarthMassRatio, which is optional
	
	
	bool moonEarthMassRatioExists;
	

	double moonEarthMassRatio;

	
	
 	

	
	// ===> Attribute ephemerisEpoch, which is optional
	
	
	bool ephemerisEpochExists;
	

	std::string ephemerisEpoch;

	
	
 	

	
	// ===> Attribute earthTideLag, which is optional
	
	
	bool earthTideLagExists;
	

	double earthTideLag;

	
	
 	

	
	// ===> Attribute earthGM, which is optional
	
	
	bool earthGMExists;
	

	double earthGM;

	
	
 	

	
	// ===> Attribute moonGM, which is optional
	
	
	bool moonGMExists;
	

	double moonGM;

	
	
 	

	
	// ===> Attribute sunGM, which is optional
	
	
	bool sunGMExists;
	

	double sunGM;

	
	
 	

	
	// ===> Attribute loveNumberH, which is optional
	
	
	bool loveNumberHExists;
	

	double loveNumberH;

	
	
 	

	
	// ===> Attribute loveNumberL, which is optional
	
	
	bool loveNumberLExists;
	

	double loveNumberL;

	
	
 	

	
	// ===> Attribute precessionConstant, which is optional
	
	
	bool precessionConstantExists;
	

	AngularRate precessionConstant;

	
	
 	

	
	// ===> Attribute lightTime1AU, which is optional
	
	
	bool lightTime1AUExists;
	

	double lightTime1AU;

	
	
 	

	
	// ===> Attribute speedOfLight, which is optional
	
	
	bool speedOfLightExists;
	

	Speed speedOfLight;

	
	
 	

	
	// ===> Attribute delayModelFlags, which is optional
	
	
	bool delayModelFlagsExists;
	

	std::string delayModelFlags;

	
	
 	

	////////////////////////////////
	// Extrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute execBlockId
	
	

	Tag execBlockId;

	
	
 	

	///////////
	// Links //
	///////////
	
	
		

	 

	

	
/*
	////////////////////////////////////////////////////////////
	// binary-deserialization material from an EndianIStream  //
	////////////////////////////////////////////////////////////
	std::map<std::string, DelayModelFixedParametersAttributeFromBin> fromBinMethods;
void delayModelFixedParametersIdFromBin( EndianIStream& eis);
void delayModelVersionFromBin( EndianIStream& eis);
void execBlockIdFromBin( EndianIStream& eis);

void gaussConstantFromBin( EndianIStream& eis);
void newtonianConstantFromBin( EndianIStream& eis);
void gravityFromBin( EndianIStream& eis);
void earthFlatteningFromBin( EndianIStream& eis);
void earthRadiusFromBin( EndianIStream& eis);
void moonEarthMassRatioFromBin( EndianIStream& eis);
void ephemerisEpochFromBin( EndianIStream& eis);
void earthTideLagFromBin( EndianIStream& eis);
void earthGMFromBin( EndianIStream& eis);
void moonGMFromBin( EndianIStream& eis);
void sunGMFromBin( EndianIStream& eis);
void loveNumberHFromBin( EndianIStream& eis);
void loveNumberLFromBin( EndianIStream& eis);
void precessionConstantFromBin( EndianIStream& eis);
void lightTime1AUFromBin( EndianIStream& eis);
void speedOfLightFromBin( EndianIStream& eis);
void delayModelFlagsFromBin( EndianIStream& eis);

*/
	
	///////////////////////////////////
	// text-deserialization material //
	///////////////////////////////////
	std::map<std::string, DelayModelFixedParametersAttributeFromText> fromTextMethods;
	
void delayModelFixedParametersIdFromText (const std::string & s);
	
	
void delayModelVersionFromText (const std::string & s);
	
	
void execBlockIdFromText (const std::string & s);
	

	
void gaussConstantFromText (const std::string & s);
	
	
void newtonianConstantFromText (const std::string & s);
	
	
void gravityFromText (const std::string & s);
	
	
void earthFlatteningFromText (const std::string & s);
	
	
void earthRadiusFromText (const std::string & s);
	
	
void moonEarthMassRatioFromText (const std::string & s);
	
	
void ephemerisEpochFromText (const std::string & s);
	
	
void earthTideLagFromText (const std::string & s);
	
	
void earthGMFromText (const std::string & s);
	
	
void moonGMFromText (const std::string & s);
	
	
void sunGMFromText (const std::string & s);
	
	
void loveNumberHFromText (const std::string & s);
	
	
void loveNumberLFromText (const std::string & s);
	
	
void precessionConstantFromText (const std::string & s);
	
	
void lightTime1AUFromText (const std::string & s);
	
	
void speedOfLightFromText (const std::string & s);
	
	
void delayModelFlagsFromText (const std::string & s);
	
	
	
	/**
	 * Serialize this into a stream of bytes written to an EndianOSStream.
	 * @param eoss the EndianOSStream to be written to
	 */
	 void toBin(EndianOSStream& eoss);
	 	 
	 /**
	  * Deserialize a stream of bytes read from an EndianIStream to build a PointingRow.
	  * @param eiss the EndianIStream to be read.
	  * @param table the DelayModelFixedParametersTable to which the row built by deserialization will be parented.
	  * @param attributesSeq a vector containing the names of the attributes . The elements order defines the order 
	  * in which the attributes are written in the binary serialization.

	 static DelayModelFixedParametersRow* fromBin(EndianIStream& eis, DelayModelFixedParametersTable& table, const std::vector<std::string>& attributesSeq);	 
		*/
};

} // End namespace asdm

#endif /* DelayModelFixedParameters_CLASS */
