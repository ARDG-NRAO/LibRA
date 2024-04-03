
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
 * File ExecBlockRow.h
 */
 
#ifndef ExecBlockRow_CLASS
#define ExecBlockRow_CLASS

#include <vector>
#include <string>
#include <set>

#ifndef WITHOUT_ACS
#include <asdmIDLC.h>
#endif






	 
#include <alma/ASDM/Angle.h>
	

	 
#include <alma/ASDM/ArrayTime.h>
	

	 
#include <alma/ASDM/Length.h>
	

	 
#include <alma/ASDM/Tag.h>
	

	 
#include <alma/ASDM/EntityRef.h>
	




	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	



#include <alma/ASDM/ConversionException.h>
#include <alma/ASDM/NoSuchRow.h>
#include <alma/ASDM/IllegalAccessException.h>

#include <alma/ASDM/RowTransformer.h>
//#include <alma/ASDM/TableStreamReader.h>

/*\file ExecBlock.h
    \brief Generated from model's revision "-1", branch ""
*/

namespace asdm {

//class asdm::ExecBlockTable;


// class asdm::AntennaRow;
class AntennaRow;

// class asdm::SBSummaryRow;
class SBSummaryRow;

// class asdm::ScaleRow;
class ScaleRow;
	

class ExecBlockRow;
typedef void (ExecBlockRow::*ExecBlockAttributeFromBin) (EndianIStream& eis);
typedef void (ExecBlockRow::*ExecBlockAttributeFromText) (const std::string& s);

/**
 * The ExecBlockRow class is a row of a ExecBlockTable.
 * 
 * Generated from model's revision "-1", branch ""
 *
 */
class ExecBlockRow {
friend class asdm::ExecBlockTable;
friend class asdm::RowTransformer<ExecBlockRow>;
//friend class asdm::TableStreamReader<ExecBlockTable, ExecBlockRow>;

public:

	virtual ~ExecBlockRow();

	/**
	 * Return the table to which this row belongs.
	 */
	ExecBlockTable &getTable() const;
	
	/**
	 * Has this row been added to its table ?
	 * @return true if and only if it has been added.
	 */
	bool isAdded() const;
		
	////////////////////////////////
	// Intrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute execBlockId
	
	
	

	
 	/**
 	 * Get execBlockId.
 	 * @return execBlockId as Tag
 	 */
 	Tag getExecBlockId() const;
	
 
 	
 	
	
	


	
	// ===> Attribute startTime
	
	
	

	
 	/**
 	 * Get startTime.
 	 * @return startTime as ArrayTime
 	 */
 	ArrayTime getStartTime() const;
	
 
 	
 	
 	/**
 	 * Set startTime with the specified ArrayTime.
 	 * @param startTime The ArrayTime value to which startTime is to be set.
 	 
 		
 			
 	 */
 	void setStartTime (ArrayTime startTime);
  		
	
	
	


	
	// ===> Attribute endTime
	
	
	

	
 	/**
 	 * Get endTime.
 	 * @return endTime as ArrayTime
 	 */
 	ArrayTime getEndTime() const;
	
 
 	
 	
 	/**
 	 * Set endTime with the specified ArrayTime.
 	 * @param endTime The ArrayTime value to which endTime is to be set.
 	 
 		
 			
 	 */
 	void setEndTime (ArrayTime endTime);
  		
	
	
	


	
	// ===> Attribute execBlockNum
	
	
	

	
 	/**
 	 * Get execBlockNum.
 	 * @return execBlockNum as int
 	 */
 	int getExecBlockNum() const;
	
 
 	
 	
 	/**
 	 * Set execBlockNum with the specified int.
 	 * @param execBlockNum The int value to which execBlockNum is to be set.
 	 
 		
 			
 	 */
 	void setExecBlockNum (int execBlockNum);
  		
	
	
	


	
	// ===> Attribute execBlockUID
	
	
	

	
 	/**
 	 * Get execBlockUID.
 	 * @return execBlockUID as EntityRef
 	 */
 	EntityRef getExecBlockUID() const;
	
 
 	
 	
 	/**
 	 * Set execBlockUID with the specified EntityRef.
 	 * @param execBlockUID The EntityRef value to which execBlockUID is to be set.
 	 
 		
 			
 	 */
 	void setExecBlockUID (EntityRef execBlockUID);
  		
	
	
	


	
	// ===> Attribute projectUID
	
	
	

	
 	/**
 	 * Get projectUID.
 	 * @return projectUID as EntityRef
 	 */
 	EntityRef getProjectUID() const;
	
 
 	
 	
 	/**
 	 * Set projectUID with the specified EntityRef.
 	 * @param projectUID The EntityRef value to which projectUID is to be set.
 	 
 		
 			
 	 */
 	void setProjectUID (EntityRef projectUID);
  		
	
	
	


	
	// ===> Attribute configName
	
	
	

	
 	/**
 	 * Get configName.
 	 * @return configName as std::string
 	 */
 	std::string getConfigName() const;
	
 
 	
 	
 	/**
 	 * Set configName with the specified std::string.
 	 * @param configName The std::string value to which configName is to be set.
 	 
 		
 			
 	 */
 	void setConfigName (std::string configName);
  		
	
	
	


	
	// ===> Attribute telescopeName
	
	
	

	
 	/**
 	 * Get telescopeName.
 	 * @return telescopeName as std::string
 	 */
 	std::string getTelescopeName() const;
	
 
 	
 	
 	/**
 	 * Set telescopeName with the specified std::string.
 	 * @param telescopeName The std::string value to which telescopeName is to be set.
 	 
 		
 			
 	 */
 	void setTelescopeName (std::string telescopeName);
  		
	
	
	


	
	// ===> Attribute observerName
	
	
	

	
 	/**
 	 * Get observerName.
 	 * @return observerName as std::string
 	 */
 	std::string getObserverName() const;
	
 
 	
 	
 	/**
 	 * Set observerName with the specified std::string.
 	 * @param observerName The std::string value to which observerName is to be set.
 	 
 		
 			
 	 */
 	void setObserverName (std::string observerName);
  		
	
	
	


	
	// ===> Attribute numObservingLog
	
	
	

	
 	/**
 	 * Get numObservingLog.
 	 * @return numObservingLog as int
 	 */
 	int getNumObservingLog() const;
	
 
 	
 	
 	/**
 	 * Set numObservingLog with the specified int.
 	 * @param numObservingLog The int value to which numObservingLog is to be set.
 	 
 		
 			
 	 */
 	void setNumObservingLog (int numObservingLog);
  		
	
	
	


	
	// ===> Attribute observingLog
	
	
	

	
 	/**
 	 * Get observingLog.
 	 * @return observingLog as std::vector<std::string >
 	 */
 	std::vector<std::string > getObservingLog() const;
	
 
 	
 	
 	/**
 	 * Set observingLog with the specified std::vector<std::string >.
 	 * @param observingLog The std::vector<std::string > value to which observingLog is to be set.
 	 
 		
 			
 	 */
 	void setObservingLog (std::vector<std::string > observingLog);
  		
	
	
	


	
	// ===> Attribute sessionReference
	
	
	

	
 	/**
 	 * Get sessionReference.
 	 * @return sessionReference as EntityRef
 	 */
 	EntityRef getSessionReference() const;
	
 
 	
 	
 	/**
 	 * Set sessionReference with the specified EntityRef.
 	 * @param sessionReference The EntityRef value to which sessionReference is to be set.
 	 
 		
 			
 	 */
 	void setSessionReference (EntityRef sessionReference);
  		
	
	
	


	
	// ===> Attribute baseRangeMin
	
	
	

	
 	/**
 	 * Get baseRangeMin.
 	 * @return baseRangeMin as Length
 	 */
 	Length getBaseRangeMin() const;
	
 
 	
 	
 	/**
 	 * Set baseRangeMin with the specified Length.
 	 * @param baseRangeMin The Length value to which baseRangeMin is to be set.
 	 
 		
 			
 	 */
 	void setBaseRangeMin (Length baseRangeMin);
  		
	
	
	


	
	// ===> Attribute baseRangeMax
	
	
	

	
 	/**
 	 * Get baseRangeMax.
 	 * @return baseRangeMax as Length
 	 */
 	Length getBaseRangeMax() const;
	
 
 	
 	
 	/**
 	 * Set baseRangeMax with the specified Length.
 	 * @param baseRangeMax The Length value to which baseRangeMax is to be set.
 	 
 		
 			
 	 */
 	void setBaseRangeMax (Length baseRangeMax);
  		
	
	
	


	
	// ===> Attribute baseRmsMinor
	
	
	

	
 	/**
 	 * Get baseRmsMinor.
 	 * @return baseRmsMinor as Length
 	 */
 	Length getBaseRmsMinor() const;
	
 
 	
 	
 	/**
 	 * Set baseRmsMinor with the specified Length.
 	 * @param baseRmsMinor The Length value to which baseRmsMinor is to be set.
 	 
 		
 			
 	 */
 	void setBaseRmsMinor (Length baseRmsMinor);
  		
	
	
	


	
	// ===> Attribute baseRmsMajor
	
	
	

	
 	/**
 	 * Get baseRmsMajor.
 	 * @return baseRmsMajor as Length
 	 */
 	Length getBaseRmsMajor() const;
	
 
 	
 	
 	/**
 	 * Set baseRmsMajor with the specified Length.
 	 * @param baseRmsMajor The Length value to which baseRmsMajor is to be set.
 	 
 		
 			
 	 */
 	void setBaseRmsMajor (Length baseRmsMajor);
  		
	
	
	


	
	// ===> Attribute basePa
	
	
	

	
 	/**
 	 * Get basePa.
 	 * @return basePa as Angle
 	 */
 	Angle getBasePa() const;
	
 
 	
 	
 	/**
 	 * Set basePa with the specified Angle.
 	 * @param basePa The Angle value to which basePa is to be set.
 	 
 		
 			
 	 */
 	void setBasePa (Angle basePa);
  		
	
	
	


	
	// ===> Attribute aborted
	
	
	

	
 	/**
 	 * Get aborted.
 	 * @return aborted as bool
 	 */
 	bool getAborted() const;
	
 
 	
 	
 	/**
 	 * Set aborted with the specified bool.
 	 * @param aborted The bool value to which aborted is to be set.
 	 
 		
 			
 	 */
 	void setAborted (bool aborted);
  		
	
	
	


	
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
  		
	
	
	


	
	// ===> Attribute releaseDate, which is optional
	
	
	
	/**
	 * The attribute releaseDate is optional. Return true if this attribute exists.
	 * @return true if and only if the releaseDate attribute exists. 
	 */
	bool isReleaseDateExists() const;
	

	
 	/**
 	 * Get releaseDate, which is optional.
 	 * @return releaseDate as ArrayTime
 	 * @throws IllegalAccessException If releaseDate does not exist.
 	 */
 	ArrayTime getReleaseDate() const;
	
 
 	
 	
 	/**
 	 * Set releaseDate with the specified ArrayTime.
 	 * @param releaseDate The ArrayTime value to which releaseDate is to be set.
 	 
 		
 	 */
 	void setReleaseDate (ArrayTime releaseDate);
		
	
	
	
	/**
	 * Mark releaseDate, which is an optional field, as non-existent.
	 */
	void clearReleaseDate ();
	


	
	// ===> Attribute schedulerMode, which is optional
	
	
	
	/**
	 * The attribute schedulerMode is optional. Return true if this attribute exists.
	 * @return true if and only if the schedulerMode attribute exists. 
	 */
	bool isSchedulerModeExists() const;
	

	
 	/**
 	 * Get schedulerMode, which is optional.
 	 * @return schedulerMode as std::string
 	 * @throws IllegalAccessException If schedulerMode does not exist.
 	 */
 	std::string getSchedulerMode() const;
	
 
 	
 	
 	/**
 	 * Set schedulerMode with the specified std::string.
 	 * @param schedulerMode The std::string value to which schedulerMode is to be set.
 	 
 		
 	 */
 	void setSchedulerMode (std::string schedulerMode);
		
	
	
	
	/**
	 * Mark schedulerMode, which is an optional field, as non-existent.
	 */
	void clearSchedulerMode ();
	


	
	// ===> Attribute siteAltitude, which is optional
	
	
	
	/**
	 * The attribute siteAltitude is optional. Return true if this attribute exists.
	 * @return true if and only if the siteAltitude attribute exists. 
	 */
	bool isSiteAltitudeExists() const;
	

	
 	/**
 	 * Get siteAltitude, which is optional.
 	 * @return siteAltitude as Length
 	 * @throws IllegalAccessException If siteAltitude does not exist.
 	 */
 	Length getSiteAltitude() const;
	
 
 	
 	
 	/**
 	 * Set siteAltitude with the specified Length.
 	 * @param siteAltitude The Length value to which siteAltitude is to be set.
 	 
 		
 	 */
 	void setSiteAltitude (Length siteAltitude);
		
	
	
	
	/**
	 * Mark siteAltitude, which is an optional field, as non-existent.
	 */
	void clearSiteAltitude ();
	


	
	// ===> Attribute siteLongitude, which is optional
	
	
	
	/**
	 * The attribute siteLongitude is optional. Return true if this attribute exists.
	 * @return true if and only if the siteLongitude attribute exists. 
	 */
	bool isSiteLongitudeExists() const;
	

	
 	/**
 	 * Get siteLongitude, which is optional.
 	 * @return siteLongitude as Angle
 	 * @throws IllegalAccessException If siteLongitude does not exist.
 	 */
 	Angle getSiteLongitude() const;
	
 
 	
 	
 	/**
 	 * Set siteLongitude with the specified Angle.
 	 * @param siteLongitude The Angle value to which siteLongitude is to be set.
 	 
 		
 	 */
 	void setSiteLongitude (Angle siteLongitude);
		
	
	
	
	/**
	 * Mark siteLongitude, which is an optional field, as non-existent.
	 */
	void clearSiteLongitude ();
	


	
	// ===> Attribute siteLatitude, which is optional
	
	
	
	/**
	 * The attribute siteLatitude is optional. Return true if this attribute exists.
	 * @return true if and only if the siteLatitude attribute exists. 
	 */
	bool isSiteLatitudeExists() const;
	

	
 	/**
 	 * Get siteLatitude, which is optional.
 	 * @return siteLatitude as Angle
 	 * @throws IllegalAccessException If siteLatitude does not exist.
 	 */
 	Angle getSiteLatitude() const;
	
 
 	
 	
 	/**
 	 * Set siteLatitude with the specified Angle.
 	 * @param siteLatitude The Angle value to which siteLatitude is to be set.
 	 
 		
 	 */
 	void setSiteLatitude (Angle siteLatitude);
		
	
	
	
	/**
	 * Mark siteLatitude, which is an optional field, as non-existent.
	 */
	void clearSiteLatitude ();
	


	
	// ===> Attribute observingScript, which is optional
	
	
	
	/**
	 * The attribute observingScript is optional. Return true if this attribute exists.
	 * @return true if and only if the observingScript attribute exists. 
	 */
	bool isObservingScriptExists() const;
	

	
 	/**
 	 * Get observingScript, which is optional.
 	 * @return observingScript as std::string
 	 * @throws IllegalAccessException If observingScript does not exist.
 	 */
 	std::string getObservingScript() const;
	
 
 	
 	
 	/**
 	 * Set observingScript with the specified std::string.
 	 * @param observingScript The std::string value to which observingScript is to be set.
 	 
 		
 	 */
 	void setObservingScript (std::string observingScript);
		
	
	
	
	/**
	 * Mark observingScript, which is an optional field, as non-existent.
	 */
	void clearObservingScript ();
	


	
	// ===> Attribute observingScriptUID, which is optional
	
	
	
	/**
	 * The attribute observingScriptUID is optional. Return true if this attribute exists.
	 * @return true if and only if the observingScriptUID attribute exists. 
	 */
	bool isObservingScriptUIDExists() const;
	

	
 	/**
 	 * Get observingScriptUID, which is optional.
 	 * @return observingScriptUID as EntityRef
 	 * @throws IllegalAccessException If observingScriptUID does not exist.
 	 */
 	EntityRef getObservingScriptUID() const;
	
 
 	
 	
 	/**
 	 * Set observingScriptUID with the specified EntityRef.
 	 * @param observingScriptUID The EntityRef value to which observingScriptUID is to be set.
 	 
 		
 	 */
 	void setObservingScriptUID (EntityRef observingScriptUID);
		
	
	
	
	/**
	 * Mark observingScriptUID, which is an optional field, as non-existent.
	 */
	void clearObservingScriptUID ();
	


	////////////////////////////////
	// Extrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute antennaId
	
	
	

	
 	/**
 	 * Get antennaId.
 	 * @return antennaId as std::vector<Tag> 
 	 */
 	std::vector<Tag>  getAntennaId() const;
	
 
 	
 	
 	/**
 	 * Set antennaId with the specified std::vector<Tag> .
 	 * @param antennaId The std::vector<Tag>  value to which antennaId is to be set.
 	 
 		
 			
 	 */
 	void setAntennaId (std::vector<Tag>  antennaId);
  		
	
	
	


	
	// ===> Attribute sBSummaryId
	
	
	

	
 	/**
 	 * Get sBSummaryId.
 	 * @return sBSummaryId as Tag
 	 */
 	Tag getSBSummaryId() const;
	
 
 	
 	
 	/**
 	 * Set sBSummaryId with the specified Tag.
 	 * @param sBSummaryId The Tag value to which sBSummaryId is to be set.
 	 
 		
 			
 	 */
 	void setSBSummaryId (Tag sBSummaryId);
  		
	
	
	


	
	// ===> Attribute scaleId, which is optional
	
	
	
	/**
	 * The attribute scaleId is optional. Return true if this attribute exists.
	 * @return true if and only if the scaleId attribute exists. 
	 */
	bool isScaleIdExists() const;
	

	
 	/**
 	 * Get scaleId, which is optional.
 	 * @return scaleId as Tag
 	 * @throws IllegalAccessException If scaleId does not exist.
 	 */
 	Tag getScaleId() const;
	
 
 	
 	
 	/**
 	 * Set scaleId with the specified Tag.
 	 * @param scaleId The Tag value to which scaleId is to be set.
 	 
 		
 	 */
 	void setScaleId (Tag scaleId);
		
	
	
	
	/**
	 * Mark scaleId, which is an optional field, as non-existent.
	 */
	void clearScaleId ();
	


	///////////
	// Links //
	///////////
	
	
 		
 	/**
 	 * Set antennaId[i] with the specified Tag.
 	 * @param i The index in antennaId where to set the Tag value.
 	 * @param antennaId The Tag value to which antennaId[i] is to be set. 
	 		
 	 * @throws IndexOutOfBoundsException
  	 */
  	void setAntennaId (int i, Tag antennaId); 
 			
	

	
		 
/**
 * Append a Tag to antennaId.
 * @param id the Tag to be appended to antennaId
 */
 void addAntennaId(Tag id); 

/**
 * Append a vector of Tag to antennaId.
 * @param id an array of Tag to be appended to antennaId
 */
 void addAntennaId(const std::vector<Tag> & id); 
 

 /**
  * Returns the Tag stored in antennaId at position i.
  * @param i the position in antennaId where the Tag is retrieved.
  * @return the Tag stored at position i in antennaId.
  */
 const Tag getAntennaId(int i);
 
 /**
  * Returns the AntennaRow linked to this row via the tag stored in antennaId
  * at position i.
  * @param i the position in antennaId.
  * @return a pointer on a AntennaRow whose key (a Tag) is equal to the Tag stored at position
  * i in the antennaId. 
  */
 AntennaRow* getAntennaUsingAntennaId(int i); 
 
 /**
  * Returns the vector of AntennaRow* linked to this row via the Tags stored in antennaId
  * @return an array of pointers on AntennaRow.
  */
 std::vector<AntennaRow *> getAntennasUsingAntennaId(); 
  

	

	

	
		
	/**
	 * sBSummaryId pointer to the row in the SBSummary table having SBSummary.sBSummaryId == sBSummaryId
	 * @return a SBSummaryRow*
	 * 
	 
	 */
	 SBSummaryRow* getSBSummaryUsingSBSummaryId();
	 

	

	

	
		
	/**
	 * scaleId pointer to the row in the Scale table having Scale.scaleId == scaleId
	 * @return a ScaleRow*
	 * 
	 
	 * throws IllegalAccessException
	 
	 */
	 ScaleRow* getScaleUsingScaleId();
	 

	

	
	
	
	/**
	 * Compare each mandatory attribute except the autoincrementable one of this ExecBlockRow with 
	 * the corresponding parameters and return true if there is a match and false otherwise.
	 	
	 * @param startTime
	    
	 * @param endTime
	    
	 * @param execBlockNum
	    
	 * @param execBlockUID
	    
	 * @param projectUID
	    
	 * @param configName
	    
	 * @param telescopeName
	    
	 * @param observerName
	    
	 * @param numObservingLog
	    
	 * @param observingLog
	    
	 * @param sessionReference
	    
	 * @param baseRangeMin
	    
	 * @param baseRangeMax
	    
	 * @param baseRmsMinor
	    
	 * @param baseRmsMajor
	    
	 * @param basePa
	    
	 * @param aborted
	    
	 * @param numAntenna
	    
	 * @param antennaId
	    
	 * @param sBSummaryId
	    
	 */ 
	bool compareNoAutoInc(ArrayTime startTime, ArrayTime endTime, int execBlockNum, EntityRef execBlockUID, EntityRef projectUID, std::string configName, std::string telescopeName, std::string observerName, int numObservingLog, std::vector<std::string > observingLog, EntityRef sessionReference, Length baseRangeMin, Length baseRangeMax, Length baseRmsMinor, Length baseRmsMajor, Angle basePa, bool aborted, int numAntenna, std::vector<Tag>  antennaId, Tag sBSummaryId);
	
	

	
	/**
	 * Compare each mandatory value (i.e. not in the key) attribute  with 
	 * the corresponding parameters and return true if there is a match and false otherwise.
	 	
	 * @param startTime
	    
	 * @param endTime
	    
	 * @param execBlockNum
	    
	 * @param execBlockUID
	    
	 * @param projectUID
	    
	 * @param configName
	    
	 * @param telescopeName
	    
	 * @param observerName
	    
	 * @param numObservingLog
	    
	 * @param observingLog
	    
	 * @param sessionReference
	    
	 * @param baseRangeMin
	    
	 * @param baseRangeMax
	    
	 * @param baseRmsMinor
	    
	 * @param baseRmsMajor
	    
	 * @param basePa
	    
	 * @param aborted
	    
	 * @param numAntenna
	    
	 * @param antennaId
	    
	 * @param sBSummaryId
	    
	 */ 
	bool compareRequiredValue(ArrayTime startTime, ArrayTime endTime, int execBlockNum, EntityRef execBlockUID, EntityRef projectUID, std::string configName, std::string telescopeName, std::string observerName, int numObservingLog, std::vector<std::string > observingLog, EntityRef sessionReference, Length baseRangeMin, Length baseRangeMax, Length baseRmsMinor, Length baseRmsMajor, Angle basePa, bool aborted, int numAntenna, std::vector<Tag>  antennaId, Tag sBSummaryId); 
		 
	
	/**
	 * Return true if all required attributes of the value part are equal to their homologues
	 * in x and false otherwise.
	 *
	 * @param x a pointer on the ExecBlockRow whose required attributes of the value part 
	 * will be compared with those of this.
	 * @return a boolean.
	 */
	bool equalByRequiredValue(ExecBlockRow* x) ;
	
#ifndef WITHOUT_ACS
	/**
	 * Return this row in the form of an IDL struct.
	 * @return The values of this row as a ExecBlockRowIDL struct.
	 */
	asdmIDL::ExecBlockRowIDL *toIDL() const;
	
	/**
	 * Define the content of a ExecBlockRowIDL struct from the values
	 * found in this row.
	 *
	 * @param x a reference to the ExecBlockRowIDL struct to be set.
	 *
	 */
	 void toIDL(asdmIDL::ExecBlockRowIDL& x) const;
#endif
	
#ifndef WITHOUT_ACS
	/**
	 * Fill the values of this row from the IDL struct ExecBlockRowIDL.
	 * @param x The IDL struct containing the values used to fill this row.
	 * @throws ConversionException
	 */
	void setFromIDL (asdmIDL::ExecBlockRowIDL x) ;
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

	std::map<std::string, ExecBlockAttributeFromBin> fromBinMethods;
void execBlockIdFromBin( EndianIStream& eis);
void startTimeFromBin( EndianIStream& eis);
void endTimeFromBin( EndianIStream& eis);
void execBlockNumFromBin( EndianIStream& eis);
void execBlockUIDFromBin( EndianIStream& eis);
void projectUIDFromBin( EndianIStream& eis);
void configNameFromBin( EndianIStream& eis);
void telescopeNameFromBin( EndianIStream& eis);
void observerNameFromBin( EndianIStream& eis);
void numObservingLogFromBin( EndianIStream& eis);
void observingLogFromBin( EndianIStream& eis);
void sessionReferenceFromBin( EndianIStream& eis);
void baseRangeMinFromBin( EndianIStream& eis);
void baseRangeMaxFromBin( EndianIStream& eis);
void baseRmsMinorFromBin( EndianIStream& eis);
void baseRmsMajorFromBin( EndianIStream& eis);
void basePaFromBin( EndianIStream& eis);
void abortedFromBin( EndianIStream& eis);
void numAntennaFromBin( EndianIStream& eis);
void antennaIdFromBin( EndianIStream& eis);
void sBSummaryIdFromBin( EndianIStream& eis);

void releaseDateFromBin( EndianIStream& eis);
void schedulerModeFromBin( EndianIStream& eis);
void siteAltitudeFromBin( EndianIStream& eis);
void siteLongitudeFromBin( EndianIStream& eis);
void siteLatitudeFromBin( EndianIStream& eis);
void observingScriptFromBin( EndianIStream& eis);
void observingScriptUIDFromBin( EndianIStream& eis);
void scaleIdFromBin( EndianIStream& eis);


	 /**
	  * Deserialize a stream of bytes read from an EndianIStream to build a PointingRow.
	  * @param eiss the EndianIStream to be read.
	  * @param table the ExecBlockTable to which the row built by deserialization will be parented.
	  * @param attributesSeq a vector containing the names of the attributes . The elements order defines the order 
	  * in which the attributes are written in the binary serialization.
	  */
	 static ExecBlockRow* fromBin(EndianIStream& eis, ExecBlockTable& table, const std::vector<std::string>& attributesSeq);	 
 
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
	ExecBlockTable &table;
	/**
	 * Whether this row has been added to the table or not.
	 */
	bool hasBeenAdded;

	// This method is used by the Table class when this row is added to the table.
	void isAdded(bool added);


	/**
	 * Create a ExecBlockRow.
	 * <p>
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 */ 
	ExecBlockRow (ExecBlockTable &table);

	/**
	 * Create a ExecBlockRow using a copy constructor mechanism.
	 * <p>
	 * Given a ExecBlockRow row and a ExecBlockTable table, the method creates a new
	 * ExecBlockRow owned by table. Each attribute of the created row is a copy (deep)
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
	 ExecBlockRow (ExecBlockTable &table, ExecBlockRow *row);
	 	
	////////////////////////////////
	// Intrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute execBlockId
	
	

	Tag execBlockId;

	
	
 	
 	/**
 	 * Set execBlockId with the specified Tag value.
 	 * @param execBlockId The Tag value to which execBlockId is to be set.
		
 		
			
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 		
 	 */
 	void setExecBlockId (Tag execBlockId);
  		
	

	
	// ===> Attribute startTime
	
	

	ArrayTime startTime;

	
	
 	

	
	// ===> Attribute endTime
	
	

	ArrayTime endTime;

	
	
 	

	
	// ===> Attribute execBlockNum
	
	

	int execBlockNum;

	
	
 	

	
	// ===> Attribute execBlockUID
	
	

	EntityRef execBlockUID;

	
	
 	

	
	// ===> Attribute projectUID
	
	

	EntityRef projectUID;

	
	
 	

	
	// ===> Attribute configName
	
	

	std::string configName;

	
	
 	

	
	// ===> Attribute telescopeName
	
	

	std::string telescopeName;

	
	
 	

	
	// ===> Attribute observerName
	
	

	std::string observerName;

	
	
 	

	
	// ===> Attribute numObservingLog
	
	

	int numObservingLog;

	
	
 	

	
	// ===> Attribute observingLog
	
	

	std::vector<std::string > observingLog;

	
	
 	

	
	// ===> Attribute sessionReference
	
	

	EntityRef sessionReference;

	
	
 	

	
	// ===> Attribute baseRangeMin
	
	

	Length baseRangeMin;

	
	
 	

	
	// ===> Attribute baseRangeMax
	
	

	Length baseRangeMax;

	
	
 	

	
	// ===> Attribute baseRmsMinor
	
	

	Length baseRmsMinor;

	
	
 	

	
	// ===> Attribute baseRmsMajor
	
	

	Length baseRmsMajor;

	
	
 	

	
	// ===> Attribute basePa
	
	

	Angle basePa;

	
	
 	

	
	// ===> Attribute aborted
	
	

	bool aborted;

	
	
 	

	
	// ===> Attribute numAntenna
	
	

	int numAntenna;

	
	
 	

	
	// ===> Attribute releaseDate, which is optional
	
	
	bool releaseDateExists;
	

	ArrayTime releaseDate;

	
	
 	

	
	// ===> Attribute schedulerMode, which is optional
	
	
	bool schedulerModeExists;
	

	std::string schedulerMode;

	
	
 	

	
	// ===> Attribute siteAltitude, which is optional
	
	
	bool siteAltitudeExists;
	

	Length siteAltitude;

	
	
 	

	
	// ===> Attribute siteLongitude, which is optional
	
	
	bool siteLongitudeExists;
	

	Angle siteLongitude;

	
	
 	

	
	// ===> Attribute siteLatitude, which is optional
	
	
	bool siteLatitudeExists;
	

	Angle siteLatitude;

	
	
 	

	
	// ===> Attribute observingScript, which is optional
	
	
	bool observingScriptExists;
	

	std::string observingScript;

	
	
 	

	
	// ===> Attribute observingScriptUID, which is optional
	
	
	bool observingScriptUIDExists;
	

	EntityRef observingScriptUID;

	
	
 	

	////////////////////////////////
	// Extrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute antennaId
	
	

	std::vector<Tag>  antennaId;

	
	
 	

	
	// ===> Attribute sBSummaryId
	
	

	Tag sBSummaryId;

	
	
 	

	
	// ===> Attribute scaleId, which is optional
	
	
	bool scaleIdExists;
	

	Tag scaleId;

	
	
 	

	///////////
	// Links //
	///////////
	
	
		


	

	
		

	 

	

	
		

	 

	

	
/*
	////////////////////////////////////////////////////////////
	// binary-deserialization material from an EndianIStream  //
	////////////////////////////////////////////////////////////
	std::map<std::string, ExecBlockAttributeFromBin> fromBinMethods;
void execBlockIdFromBin( EndianIStream& eis);
void startTimeFromBin( EndianIStream& eis);
void endTimeFromBin( EndianIStream& eis);
void execBlockNumFromBin( EndianIStream& eis);
void execBlockUIDFromBin( EndianIStream& eis);
void projectUIDFromBin( EndianIStream& eis);
void configNameFromBin( EndianIStream& eis);
void telescopeNameFromBin( EndianIStream& eis);
void observerNameFromBin( EndianIStream& eis);
void numObservingLogFromBin( EndianIStream& eis);
void observingLogFromBin( EndianIStream& eis);
void sessionReferenceFromBin( EndianIStream& eis);
void baseRangeMinFromBin( EndianIStream& eis);
void baseRangeMaxFromBin( EndianIStream& eis);
void baseRmsMinorFromBin( EndianIStream& eis);
void baseRmsMajorFromBin( EndianIStream& eis);
void basePaFromBin( EndianIStream& eis);
void abortedFromBin( EndianIStream& eis);
void numAntennaFromBin( EndianIStream& eis);
void antennaIdFromBin( EndianIStream& eis);
void sBSummaryIdFromBin( EndianIStream& eis);

void releaseDateFromBin( EndianIStream& eis);
void schedulerModeFromBin( EndianIStream& eis);
void siteAltitudeFromBin( EndianIStream& eis);
void siteLongitudeFromBin( EndianIStream& eis);
void siteLatitudeFromBin( EndianIStream& eis);
void observingScriptFromBin( EndianIStream& eis);
void observingScriptUIDFromBin( EndianIStream& eis);
void scaleIdFromBin( EndianIStream& eis);

*/
	
	///////////////////////////////////
	// text-deserialization material //
	///////////////////////////////////
	std::map<std::string, ExecBlockAttributeFromText> fromTextMethods;
	
void execBlockIdFromText (const std::string & s);
	
	
void startTimeFromText (const std::string & s);
	
	
void endTimeFromText (const std::string & s);
	
	
void execBlockNumFromText (const std::string & s);
	
	
	
	
void configNameFromText (const std::string & s);
	
	
void telescopeNameFromText (const std::string & s);
	
	
void observerNameFromText (const std::string & s);
	
	
void numObservingLogFromText (const std::string & s);
	
	
void observingLogFromText (const std::string & s);
	
	
	
void baseRangeMinFromText (const std::string & s);
	
	
void baseRangeMaxFromText (const std::string & s);
	
	
void baseRmsMinorFromText (const std::string & s);
	
	
void baseRmsMajorFromText (const std::string & s);
	
	
void basePaFromText (const std::string & s);
	
	
void abortedFromText (const std::string & s);
	
	
void numAntennaFromText (const std::string & s);
	
	
void antennaIdFromText (const std::string & s);
	
	
void sBSummaryIdFromText (const std::string & s);
	

	
void releaseDateFromText (const std::string & s);
	
	
void schedulerModeFromText (const std::string & s);
	
	
void siteAltitudeFromText (const std::string & s);
	
	
void siteLongitudeFromText (const std::string & s);
	
	
void siteLatitudeFromText (const std::string & s);
	
	
void observingScriptFromText (const std::string & s);
	
	
	
void scaleIdFromText (const std::string & s);
	
	
	
	/**
	 * Serialize this into a stream of bytes written to an EndianOSStream.
	 * @param eoss the EndianOSStream to be written to
	 */
	 void toBin(EndianOSStream& eoss);
	 	 
	 /**
	  * Deserialize a stream of bytes read from an EndianIStream to build a PointingRow.
	  * @param eiss the EndianIStream to be read.
	  * @param table the ExecBlockTable to which the row built by deserialization will be parented.
	  * @param attributesSeq a vector containing the names of the attributes . The elements order defines the order 
	  * in which the attributes are written in the binary serialization.

	 static ExecBlockRow* fromBin(EndianIStream& eis, ExecBlockTable& table, const std::vector<std::string>& attributesSeq);	 
		*/
};

} // End namespace asdm

#endif /* ExecBlock_CLASS */
