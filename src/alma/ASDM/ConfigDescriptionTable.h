
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
 * File ConfigDescriptionTable.h
 */
 
#ifndef ConfigDescriptionTable_CLASS
#define ConfigDescriptionTable_CLASS

#include <string>
#include <vector>
#include <map>



	
#include <alma/ASDM/Tag.h>
	




	

	

	

	
#include <alma/Enumerations/CCorrelationMode.h>
	

	

	

	
#include <alma/Enumerations/CAtmPhaseCorrection.h>
	

	
#include <alma/Enumerations/CProcessorType.h>
	

	

	
#include <alma/Enumerations/CSpectralResolutionType.h>
	

	

	
#include <alma/Enumerations/CSpectralResolutionType.h>
	



#include <alma/ASDM/ConversionException.h>
#include <alma/ASDM/DuplicateKey.h>
#include <alma/ASDM/UniquenessViolationException.h>
#include <alma/ASDM/NoSuchRow.h>
#include <alma/ASDM/DuplicateKey.h>


#ifndef WITHOUT_ACS
#include <asdmIDLC.h>
#endif

#include <alma/ASDM/Representable.h>

#include <pthread.h>

namespace asdm {

//class asdm::ASDM;
//class asdm::ConfigDescriptionRow;

class ASDM;
class ConfigDescriptionRow;
/**
 * The ConfigDescriptionTable class is an Alma table.
 * <BR>
 * 
 * \par Role
 * Defines the hardware configuration used to  obtain the science data.
 * <BR>
 
 * Generated from model's revision "-1", branch ""
 *
 * <TABLE BORDER="1">
 * <CAPTION> Attributes of ConfigDescription </CAPTION>
 * <TR BGCOLOR="#AAAAAA"> <TH> Name </TH> <TH> Type </TH> <TH> Expected shape  </TH> <TH> Comment </TH></TR>
 
 * <TR> <TH BGCOLOR="#CCCCCC" colspan="4" align="center"> Key </TD></TR>
	
 * <TR>
 		
 * <TD><I> configDescriptionId </I></TD>
 		 
 * <TD> Tag</TD>
 * <TD> &nbsp; </TD>
 * <TD> &nbsp;identifies a unique row in the table. </TD>
 * </TR>
	


 * <TR> <TH BGCOLOR="#CCCCCC"  colspan="4" valign="center"> Value <br> (Mandatory) </TH></TR>
	
 * <TR>
 * <TD> numAntenna (numAntenna)</TD> 
 * <TD> int </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;the number of antennas. </TD>
 * </TR>
	
 * <TR>
 * <TD> numDataDescription (numDataDescription)</TD> 
 * <TD> int </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;the number of data descriptions. </TD>
 * </TR>
	
 * <TR>
 * <TD> numFeed (numFeed)</TD> 
 * <TD> int </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;the number of feeds. </TD>
 * </TR>
	
 * <TR>
 * <TD> correlationMode </TD> 
 * <TD> CorrelationModeMod::CorrelationMode </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;identifies the correlation mode. </TD>
 * </TR>
	
 * <TR>
 * <TD> numAtmPhaseCorrection (numAtmPhaseCorrection)</TD> 
 * <TD> int </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;the number of descriptions of the atmospheric phase correction. </TD>
 * </TR>
	
 * <TR>
 * <TD> atmPhaseCorrection </TD> 
 * <TD> std::vector<AtmPhaseCorrectionMod::AtmPhaseCorrection > </TD>
 * <TD>  numAtmPhaseCorrection </TD> 
 * <TD> &nbsp;describe how the atmospheric phase corrections have been applied (one value per correction). </TD>
 * </TR>
	
 * <TR>
 * <TD> processorType </TD> 
 * <TD> ProcessorTypeMod::ProcessorType </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;identifies the generic processor's type. </TD>
 * </TR>
	
 * <TR>
 * <TD> spectralType </TD> 
 * <TD> SpectralResolutionTypeMod::SpectralResolutionType </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;identifies the spectral type of the data. </TD>
 * </TR>
	
 * <TR>
 * <TD> antennaId </TD> 
 * <TD> std::vector<Tag>  </TD>
 * <TD>  numAntenna </TD> 
 * <TD> &nbsp;identifies numAntenna rows in AntennaTable. </TD>
 * </TR>
	
 * <TR>
 * <TD> feedId </TD> 
 * <TD> std::vector<int>  </TD>
 * <TD>  numAntenna*numFeed </TD> 
 * <TD> &nbsp;refers to many collections of rows in the Feed Table. </TD>
 * </TR>
	
 * <TR>
 * <TD> switchCycleId </TD> 
 * <TD> std::vector<Tag>  </TD>
 * <TD>  numDataDescription </TD> 
 * <TD> &nbsp;refers to a unique row in the SwitchCycle Table. </TD>
 * </TR>
	
 * <TR>
 * <TD> dataDescriptionId </TD> 
 * <TD> std::vector<Tag>  </TD>
 * <TD>  numDataDescription </TD> 
 * <TD> &nbsp;refers to one or more rows in DataDescriptionTable. </TD>
 * </TR>
	
 * <TR>
 * <TD> processorId </TD> 
 * <TD> Tag </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;refers to a unique row in ProcessorTable. </TD>
 * </TR>
	


 * <TR> <TH BGCOLOR="#CCCCCC"  colspan="4" valign="center"> Value <br> (Optional) </TH></TR>
	
 * <TR>
 * <TD> phasedArrayList</TD> 
 * <TD> std::vector<int > </TD>
 * <TD>  numAntenna  </TD>
 * <TD>&nbsp; phased array identifiers. </TD>
 * </TR>
	
 * <TR>
 * <TD> numAssocValues(numAssocValues)</TD> 
 * <TD> int </TD>
 * <TD>  &nbsp; </TD>
 * <TD>&nbsp; the number of associated config descriptions. </TD>
 * </TR>
	
 * <TR>
 * <TD> assocNature</TD> 
 * <TD> std::vector<SpectralResolutionTypeMod::SpectralResolutionType > </TD>
 * <TD>  numAssocValues  </TD>
 * <TD>&nbsp; the natures of the associations with other config descriptions (one value per association). </TD>
 * </TR>
	
 * <TR>
 * <TD> assocConfigDescriptionId</TD> 
 * <TD> std::vector<Tag>  </TD>
 * <TD>  numAssocValues  </TD>
 * <TD>&nbsp; refers to one or more rows in ConfigDescriptionTable. </TD>
 * </TR>
	

 * </TABLE>
 */
class ConfigDescriptionTable : public Representable {
	friend class ASDM;

public:


	/**
	 * Return the list of field names that make up key key
	 * as an array of strings.
	 * @return a vector of string.
	 */	
	static const std::vector<std::string>& getKeyName();


	virtual ~ConfigDescriptionTable();
	
	/**
	 * Return the container to which this table belongs.
	 *
	 * @return the ASDM containing this table.
	 */
	ASDM &getContainer() const;
	
	/**
	 * Return the number of rows in the table.
	 *
	 * @return the number of rows in an unsigned int.
	 */
	unsigned int size() const;
	
	/**
	 * Return the name of this table.
	 *
	 * This is a instance method of the class.
	 *
	 * @return the name of this table in a string.
	 */
	std::string getName() const;
	
	/**
	 * Return the name of this table.
	 *
	 * This is a static method of the class.
	 *
	 * @return the name of this table in a string.
	 */
	static std::string name() ;	
	
	/**
	 * Return the version information about this table.
	 *
	 */
	 std::string getVersion() const ;
	
	/**
	 * Return the names of the attributes of this table.
	 *
	 * @return a vector of string
	 */
	 static const std::vector<std::string>& getAttributesNames();

	/**
	 * Return the default sorted list of attributes names in the binary representation of the table.
	 *
	 * @return a const reference to a vector of string
	 */
	 static const std::vector<std::string>& defaultAttributesNamesInBin();
	 
	/**
	 * Return this table's Entity.
	 */
	Entity getEntity() const;

	/**
	 * Set this table's Entity.
	 * @param e An entity. 
	 */
	void setEntity(Entity e);
		
	/**
	 * Produces an XML representation conform
	 * to the schema defined for ConfigDescription (ConfigDescriptionTable.xsd).
	 *
	 * @returns a string containing the XML representation.
	 * @throws ConversionException
	 */
	std::string toXML()  ;

#ifndef WITHOUT_ACS
	// Conversion Methods
	/**
	 * Convert this table into a ConfigDescriptionTableIDL CORBA structure.
	 *
	 * @return a pointer to a ConfigDescriptionTableIDL
	 */
	asdmIDL::ConfigDescriptionTableIDL *toIDL() ;
	
	/**
	 * Fills the CORBA data structure passed in parameter
	 * with the content of this table.
	 *
	 * @param x a reference to the asdmIDL::ConfigDescriptionTableIDL to be populated
	 * with the content of this.
	 */
	 void toIDL(asdmIDL::ConfigDescriptionTableIDL& x) const;
	 
#endif

#ifndef WITHOUT_ACS
	/**
	 * Populate this table from the content of a ConfigDescriptionTableIDL Corba structure.
	 *
	 * @throws DuplicateKey Thrown if the method tries to add a row having a key that is already in the table.
	 * @throws ConversionException
	 */	
	void fromIDL(asdmIDL::ConfigDescriptionTableIDL x) ;
#endif
	
	//
	// ====> Row creation.
	//
	
	/**
	 * Create a new row with default values.
	 * @return a pointer on a ConfigDescriptionRow
	 */
	ConfigDescriptionRow *newRow();
	
	
	/**
	 * Create a new row initialized to the specified values.
	 * @return a pointer on the created and initialized row.
	
 	 * @param numAntenna
	
 	 * @param numDataDescription
	
 	 * @param numFeed
	
 	 * @param correlationMode
	
 	 * @param numAtmPhaseCorrection
	
 	 * @param atmPhaseCorrection
	
 	 * @param processorType
	
 	 * @param spectralType
	
 	 * @param antennaId
	
 	 * @param feedId
	
 	 * @param switchCycleId
	
 	 * @param dataDescriptionId
	
 	 * @param processorId
	
     */
	ConfigDescriptionRow *newRow(int numAntenna, int numDataDescription, int numFeed, CorrelationModeMod::CorrelationMode correlationMode, int numAtmPhaseCorrection, std::vector<AtmPhaseCorrectionMod::AtmPhaseCorrection > atmPhaseCorrection, ProcessorTypeMod::ProcessorType processorType, SpectralResolutionTypeMod::SpectralResolutionType spectralType, std::vector<Tag>  antennaId, std::vector<int>  feedId, std::vector<Tag>  switchCycleId, std::vector<Tag>  dataDescriptionId, Tag processorId);
	


	/**
	 * Create a new row using a copy constructor mechanism.
	 * 
	 * The method creates a new ConfigDescriptionRow owned by this. Each attribute of the created row 
	 * is a (deep) copy of the corresponding attribute of row. The method does not add 
	 * the created row to this, its simply parents it to this, a call to the add method
	 * has to be done in order to get the row added (very likely after having modified
	 * some of its attributes).
	 * If row is null then the method returns a new ConfigDescriptionRow with default values for its attributes. 
	 *
	 * @param row the row which is to be copied.
	 */
	 ConfigDescriptionRow *newRow(ConfigDescriptionRow *row); 

	//
	// ====> Append a row to its table.
	//

	
	
	
	/** 
	 * Add a row.
	 * If there table contains a row whose key's fields are equal
	 * to x's ones then return a pointer on this row (i.e. no actual insertion is performed) 
	 * otherwise add x to the table and return x.
	 * @param x . A pointer on the row to be added.
 	 * @returns a pointer to a ConfigDescriptionRow.	 
	 */	 
	 
 	 ConfigDescriptionRow* add(ConfigDescriptionRow* x) ;



	//
	// ====> Methods returning rows.
	//
		
	/**
	 * Get a collection of pointers on the rows of the table.
	 * @return Alls rows in a vector of pointers of ConfigDescriptionRow. The elements of this vector are stored in the order 
	 * in which they have been added to the ConfigDescriptionTable.
	 */
	std::vector<ConfigDescriptionRow *> get() ;
	
	/**
	 * Get a const reference on the collection of rows pointers internally hold by the table.
	 * @return A const reference of a vector of pointers of ConfigDescriptionRow. The elements of this vector are stored in the order 
	 * in which they have been added to the ConfigDescriptionTable.
	 *
	 */
	 const std::vector<ConfigDescriptionRow *>& get() const ;
	


 
	
	/**
 	 * Returns a ConfigDescriptionRow* given a key.
 	 * @return a pointer to the row having the key whose values are passed as parameters, or 0 if
 	 * no row exists for that key.
	
	 * @param configDescriptionId
	
 	 *
	 */
 	ConfigDescriptionRow* getRowByKey(Tag configDescriptionId);

 	 	



	/**
 	 * Look up the table for a row whose all attributes  except the autoincrementable one 
 	 * are equal to the corresponding parameters of the method.
 	 * @return a pointer on this row if any, null otherwise.
 	 *
			
 	 * @param numAntenna
 	 		
 	 * @param numDataDescription
 	 		
 	 * @param numFeed
 	 		
 	 * @param correlationMode
 	 		
 	 * @param numAtmPhaseCorrection
 	 		
 	 * @param atmPhaseCorrection
 	 		
 	 * @param processorType
 	 		
 	 * @param spectralType
 	 		
 	 * @param antennaId
 	 		
 	 * @param feedId
 	 		
 	 * @param switchCycleId
 	 		
 	 * @param dataDescriptionId
 	 		
 	 * @param processorId
 	 		 
 	 */
	ConfigDescriptionRow* lookup(int numAntenna, int numDataDescription, int numFeed, CorrelationModeMod::CorrelationMode correlationMode, int numAtmPhaseCorrection, std::vector<AtmPhaseCorrectionMod::AtmPhaseCorrection > atmPhaseCorrection, ProcessorTypeMod::ProcessorType processorType, SpectralResolutionTypeMod::SpectralResolutionType spectralType, std::vector<Tag>  antennaId, std::vector<int>  feedId, std::vector<Tag>  switchCycleId, std::vector<Tag>  dataDescriptionId, Tag processorId); 


	void setUnknownAttributeBinaryReader(const std::string& attributeName, BinaryAttributeReaderFunctor* barFctr);
	BinaryAttributeReaderFunctor* getUnknownAttributeBinaryReader(const std::string& attributeName) const;

private:

	/**
	 * Create a ConfigDescriptionTable.
	 * <p>
	 * This constructor is private because only the
	 * container can create tables.  All tables must know the container
	 * to which they belong.
	 * @param container The container to which this table belongs.
	 */ 
	ConfigDescriptionTable (ASDM & container);

	ASDM & container;
	
	bool archiveAsBin; // If true archive binary else archive XML
	bool fileAsBin ; // If true file binary else file XML	
	
	std::string version ; 
	
	Entity entity;
	

	
	

	// A map for the autoincrementation algorithm
	std::map<std::string,int>  noAutoIncIds;
	void autoIncrement(std::string key, ConfigDescriptionRow* x);


	/**
	 * If this table has an autoincrementable attribute then check if *x verifies the rule of uniqueness and throw exception if not.
	 * Check if *x verifies the key uniqueness rule and throw an exception if not.
	 * Append x to its table.
	 * @throws DuplicateKey
	 
	 * @throws UniquenessViolationException
	 
	 */
	ConfigDescriptionRow* checkAndAdd(ConfigDescriptionRow* x, bool skipCheckUniqueness=false) ;
	
	/**
	 * Brutally append an ConfigDescriptionRow x to the collection of rows already stored in this table. No uniqueness check is done !
	 *
	 * @param ConfigDescriptionRow* x a pointer onto the ConfigDescriptionRow to be appended.
	 */
	 void append(ConfigDescriptionRow* x) ;
	 
	/**
	 * Brutally append an ConfigDescriptionRow x to the collection of rows already stored in this table. No uniqueness check is done !
	 *
	 * @param ConfigDescriptionRow* x a pointer onto the ConfigDescriptionRow to be appended.
	 */
	 void addWithoutCheckingUnique(ConfigDescriptionRow* x) ;
	 
	 



// A data structure to store the pointers on the table's rows.

// In all cases we maintain a private vector of ConfigDescriptionRow s.
   std::vector<ConfigDescriptionRow * > privateRows;
   

			
	std::vector<ConfigDescriptionRow *> row;

	
	void error() ; //throw(ConversionException);

	
	/**
	 * Populate this table from the content of a XML document that is required to
	 * be conform to the XML schema defined for a ConfigDescription (ConfigDescriptionTable.xsd).
	 * @throws ConversionException
	 * 
	 */
	void fromXML(std::string& xmlDoc) ;
		
	std::map<std::string, BinaryAttributeReaderFunctor *> unknownAttributes2Functors;

	/**
	  * Private methods involved during the build of this table out of the content
	  * of file(s) containing an external representation of a ConfigDescription table.
	  */
	void setFromMIMEFile(const std::string& directory);
	/*
	void openMIMEFile(const std::string& directory);
	*/
	void setFromXMLFile(const std::string& directory);
	
		 /**
	 * Serialize this into a stream of bytes and encapsulates that stream into a MIME message.
	 * @returns a string containing the MIME message.
	 *
	 * @param byteOrder a const pointer to a static instance of the class ByteOrder.
	 * 
	 */
	std::string toMIME(const asdm::ByteOrder* byteOrder=asdm::ByteOrder::Machine_Endianity);
  
	
   /** 
     * Extracts the binary part of a MIME message and deserialize its content
	 * to fill this with the result of the deserialization. 
	 * @param mimeMsg the string containing the MIME message.
	 * @throws ConversionException
	 */
	 void setFromMIME(const std::string & mimeMsg);
	
	/**
	  * Private methods involved during the export of this table into disk file(s).
	  */
	std::string MIMEXMLPart(const asdm::ByteOrder* byteOrder=asdm::ByteOrder::Machine_Endianity);
	
	/**
	  * Stores a representation (binary or XML) of this table into a file.
	  *
	  * Depending on the boolean value of its private field fileAsBin a binary serialization  of this (fileAsBin==true)  
	  * will be saved in a file "ConfigDescription.bin" or an XML representation (fileAsBin==false) will be saved in a file "ConfigDescription.xml".
	  * The file is always written in a directory whose name is passed as a parameter.
	 * @param directory The name of directory  where the file containing the table's representation will be saved.
	  * 
	  */
	  void toFile(std::string directory);
	  
	  /**
	   * Load the table in memory if necessary.
	   */
	  bool loadInProgress;
	  void checkPresenceInMemory() {
		if (!presentInMemory && !loadInProgress) {
			loadInProgress = true;
			setFromFile(getContainer().getDirectory());
			presentInMemory = true;
			loadInProgress = false;
	  	}
	  }
	/**
	 * Reads and parses a file containing a representation of a ConfigDescriptionTable as those produced  by the toFile method.
	 * This table is populated with the result of the parsing.
	 * @param directory The name of the directory containing the file te be read and parsed.
	 * @throws ConversionException If any error occurs while reading the 
	 * files in the directory or parsing them.
	 *
	 */
	 void setFromFile(const std::string& directory);	
 
};

} // End namespace asdm

#endif /* ConfigDescriptionTable_CLASS */
