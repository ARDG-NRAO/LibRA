
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
 * File CalAntennaSolutionsTable.h
 */
 
#ifndef CalAntennaSolutionsTable_CLASS
#define CalAntennaSolutionsTable_CLASS

#include <string>
#include <vector>
#include <map>



	
#include <alma/ASDM/Angle.h>
	

	
#include <alma/ASDM/ArrayTime.h>
	

	
#include <alma/ASDM/Frequency.h>
	

	
#include <alma/ASDM/Tag.h>
	

	
#include <alma/ASDM/Interval.h>
	




	

	
#include <alma/Enumerations/CAtmPhaseCorrection.h>
	

	
#include <alma/Enumerations/CBasebandName.h>
	

	
#include <alma/Enumerations/CReceiverBand.h>
	

	

	

	

	

	

	

	

	
#include <alma/Enumerations/CPolarizationType.h>
	

	

	

	

	

	



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
//class asdm::CalAntennaSolutionsRow;

class ASDM;
class CalAntennaSolutionsRow;
/**
 * The CalAntennaSolutionsTable class is an Alma table.
 * <BR>
 * 
 * \par Role
 * Results of atmosphere calibration by TelCal. This calibration determines the system temperatures corrected for atmospheric absorption. Ionospheric effects are not dealt with in the Table.
 * <BR>
 
 * Generated from model's revision "-1", branch ""
 *
 * <TABLE BORDER="1">
 * <CAPTION> Attributes of CalAntennaSolutions </CAPTION>
 * <TR BGCOLOR="#AAAAAA"> <TH> Name </TH> <TH> Type </TH> <TH> Expected shape  </TH> <TH> Comment </TH></TR>
 
 * <TR> <TH BGCOLOR="#CCCCCC" colspan="4" align="center"> Key </TD></TR>
	
 * <TR>
 		
 * <TD> antennaName </TD>
 		 
 * <TD> std::string</TD>
 * <TD> &nbsp; </TD>
 * <TD> &nbsp;the name of the antenna. </TD>
 * </TR>
	
 * <TR>
 		
 * <TD> atmPhaseCorrection </TD>
 		 
 * <TD> AtmPhaseCorrectionMod::AtmPhaseCorrection</TD>
 * <TD> &nbsp; </TD>
 * <TD> &nbsp; describes how the atmospheric phase correction has been applied. </TD>
 * </TR>
	
 * <TR>
 		
 * <TD> receiverBand </TD>
 		 
 * <TD> ReceiverBandMod::ReceiverBand</TD>
 * <TD> &nbsp; </TD>
 * <TD> &nbsp;identifies the receiver band. </TD>
 * </TR>
	
 * <TR>
 		
 * <TD> basebandName </TD>
 		 
 * <TD> BasebandNameMod::BasebandName</TD>
 * <TD> &nbsp; </TD>
 * <TD> &nbsp;identifies the baseband. </TD>
 * </TR>
	
 * <TR>
 		
 * <TD> calDataId </TD>
 		 
 * <TD> Tag</TD>
 * <TD> &nbsp; </TD>
 * <TD> &nbsp;refers to a unique row in CalData Table. </TD>
 * </TR>
	
 * <TR>
 		
 * <TD> calReductionId </TD>
 		 
 * <TD> Tag</TD>
 * <TD> &nbsp; </TD>
 * <TD> &nbsp;refers to a unique row in CalReduction Table. </TD>
 * </TR>
	
 * <TR>
 		
 * <TD> spectralWindowId </TD>
 		 
 * <TD> Tag</TD>
 * <TD> &nbsp; </TD>
 * <TD> &nbsp; </TD>
 * </TR>
	


 * <TR> <TH BGCOLOR="#CCCCCC"  colspan="4" valign="center"> Value <br> (Mandatory) </TH></TR>
	
 * <TR>
 * <TD> startValidTime </TD> 
 * <TD> ArrayTime </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;the start time of result validity period. </TD>
 * </TR>
	
 * <TR>
 * <TD> endValidTime </TD> 
 * <TD> ArrayTime </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;the end time of result validity period. </TD>
 * </TR>
	
 * <TR>
 * <TD> numReceptor (numReceptor)</TD> 
 * <TD> int </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;the number of receptors. </TD>
 * </TR>
	
 * <TR>
 * <TD> refAntennaName </TD> 
 * <TD> std::string </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;the name of the antenna used as a
reference to get the antenna-based
phases.
 </TD>
 * </TR>
	
 * <TR>
 * <TD> direction </TD> 
 * <TD> std::vector<Angle > </TD>
 * <TD>  2 </TD> 
 * <TD> &nbsp;the direction of the source. </TD>
 * </TR>
	
 * <TR>
 * <TD> frequencyRange </TD> 
 * <TD> std::vector<Frequency > </TD>
 * <TD>  2 </TD> 
 * <TD> &nbsp;the frequency range. </TD>
 * </TR>
	
 * <TR>
 * <TD> integrationTime </TD> 
 * <TD> Interval </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;the integration duration for a data point. </TD>
 * </TR>
	
 * <TR>
 * <TD> polarizationTypes </TD> 
 * <TD> std::vector<PolarizationTypeMod::PolarizationType > </TD>
 * <TD>  numReceptor </TD> 
 * <TD> &nbsp;the polarizations of the receptors (an array with one value per receptor). </TD>
 * </TR>
	
 * <TR>
 * <TD> correctionValidity </TD> 
 * <TD> bool </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;the deduced validity of atmospheric path length correction (from water vapor radiometers). </TD>
 * </TR>
	
 * <TR>
 * <TD> phaseAnt </TD> 
 * <TD> std::vector<float > </TD>
 * <TD>  numReceptor </TD> 
 * <TD> &nbsp;the antenna based phase solution averaged over the scan (one value per receptor per antenna). See refAntennaName for the association of the values of this array with the antennas. </TD>
 * </TR>
	
 * <TR>
 * <TD> phaseAntRMS </TD> 
 * <TD> std::vector<float > </TD>
 * <TD>  numReceptor </TD> 
 * <TD> &nbsp;the RMS of the phase fluctuations relative to the antenna based average phase (one value per receptor per antenna). See refAntennaName for the association of the values of this array with the antennas. </TD>
 * </TR>
	
 * <TR>
 * <TD> amplitudeAnt </TD> 
 * <TD> std::vector<float > </TD>
 * <TD>  numReceptor </TD> 
 * <TD> &nbsp;the antenna based amplitude solution averaged over the scan (one value per receptor per antenna). See refAntennaName for the association of the values of this array with the antennas. </TD>
 * </TR>
	
 * <TR>
 * <TD> amplitudeAntRMS </TD> 
 * <TD> std::vector<float > </TD>
 * <TD>  numReceptor </TD> 
 * <TD> &nbsp;the antenna based amplitude solution averaged over the scan (one value per receptor per antenna). See refAntennaName for the association of the values of this array with the antennas. </TD>
 * </TR>
	


 * </TABLE>
 */
class CalAntennaSolutionsTable : public Representable {
	friend class ASDM;

public:


	/**
	 * Return the list of field names that make up key key
	 * as an array of strings.
	 * @return a vector of string.
	 */	
	static const std::vector<std::string>& getKeyName();


	virtual ~CalAntennaSolutionsTable();
	
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
	 * to the schema defined for CalAntennaSolutions (CalAntennaSolutionsTable.xsd).
	 *
	 * @returns a string containing the XML representation.
	 * @throws ConversionException
	 */
	std::string toXML()  ;

#ifndef WITHOUT_ACS
	// Conversion Methods
	/**
	 * Convert this table into a CalAntennaSolutionsTableIDL CORBA structure.
	 *
	 * @return a pointer to a CalAntennaSolutionsTableIDL
	 */
	asdmIDL::CalAntennaSolutionsTableIDL *toIDL() ;
	
	/**
	 * Fills the CORBA data structure passed in parameter
	 * with the content of this table.
	 *
	 * @param x a reference to the asdmIDL::CalAntennaSolutionsTableIDL to be populated
	 * with the content of this.
	 */
	 void toIDL(asdmIDL::CalAntennaSolutionsTableIDL& x) const;
	 
#endif

#ifndef WITHOUT_ACS
	/**
	 * Populate this table from the content of a CalAntennaSolutionsTableIDL Corba structure.
	 *
	 * @throws DuplicateKey Thrown if the method tries to add a row having a key that is already in the table.
	 * @throws ConversionException
	 */	
	void fromIDL(asdmIDL::CalAntennaSolutionsTableIDL x) ;
#endif
	
	//
	// ====> Row creation.
	//
	
	/**
	 * Create a new row with default values.
	 * @return a pointer on a CalAntennaSolutionsRow
	 */
	CalAntennaSolutionsRow *newRow();
	
	
	/**
	 * Create a new row initialized to the specified values.
	 * @return a pointer on the created and initialized row.
	
 	 * @param antennaName
	
 	 * @param atmPhaseCorrection
	
 	 * @param receiverBand
	
 	 * @param basebandName
	
 	 * @param calDataId
	
 	 * @param calReductionId
	
 	 * @param spectralWindowId
	
 	 * @param startValidTime
	
 	 * @param endValidTime
	
 	 * @param numReceptor
	
 	 * @param refAntennaName
	
 	 * @param direction
	
 	 * @param frequencyRange
	
 	 * @param integrationTime
	
 	 * @param polarizationTypes
	
 	 * @param correctionValidity
	
 	 * @param phaseAnt
	
 	 * @param phaseAntRMS
	
 	 * @param amplitudeAnt
	
 	 * @param amplitudeAntRMS
	
     */
	CalAntennaSolutionsRow *newRow(std::string antennaName, AtmPhaseCorrectionMod::AtmPhaseCorrection atmPhaseCorrection, ReceiverBandMod::ReceiverBand receiverBand, BasebandNameMod::BasebandName basebandName, Tag calDataId, Tag calReductionId, Tag spectralWindowId, ArrayTime startValidTime, ArrayTime endValidTime, int numReceptor, std::string refAntennaName, std::vector<Angle > direction, std::vector<Frequency > frequencyRange, Interval integrationTime, std::vector<PolarizationTypeMod::PolarizationType > polarizationTypes, bool correctionValidity, std::vector<float > phaseAnt, std::vector<float > phaseAntRMS, std::vector<float > amplitudeAnt, std::vector<float > amplitudeAntRMS);
	


	/**
	 * Create a new row using a copy constructor mechanism.
	 * 
	 * The method creates a new CalAntennaSolutionsRow owned by this. Each attribute of the created row 
	 * is a (deep) copy of the corresponding attribute of row. The method does not add 
	 * the created row to this, its simply parents it to this, a call to the add method
	 * has to be done in order to get the row added (very likely after having modified
	 * some of its attributes).
	 * If row is null then the method returns a new CalAntennaSolutionsRow with default values for its attributes. 
	 *
	 * @param row the row which is to be copied.
	 */
	 CalAntennaSolutionsRow *newRow(CalAntennaSolutionsRow *row); 

	//
	// ====> Append a row to its table.
	//
 
	
	/**
	 * Add a row.
	 * @param x a pointer to the CalAntennaSolutionsRow to be added.
	 *
	 * @return a pointer to a CalAntennaSolutionsRow. If the table contains a CalAntennaSolutionsRow whose attributes (key and mandatory values) are equal to x ones
	 * then returns a pointer on that CalAntennaSolutionsRow, otherwise returns x.
	 *
	 * @throw DuplicateKey { thrown when the table contains a CalAntennaSolutionsRow with a key equal to the x one but having
	 * and a value section different from x one }
	 *
	
	 */
	CalAntennaSolutionsRow* add(CalAntennaSolutionsRow* x) ; 

 



	//
	// ====> Methods returning rows.
	//
		
	/**
	 * Get a collection of pointers on the rows of the table.
	 * @return Alls rows in a vector of pointers of CalAntennaSolutionsRow. The elements of this vector are stored in the order 
	 * in which they have been added to the CalAntennaSolutionsTable.
	 */
	std::vector<CalAntennaSolutionsRow *> get() ;
	
	/**
	 * Get a const reference on the collection of rows pointers internally hold by the table.
	 * @return A const reference of a vector of pointers of CalAntennaSolutionsRow. The elements of this vector are stored in the order 
	 * in which they have been added to the CalAntennaSolutionsTable.
	 *
	 */
	 const std::vector<CalAntennaSolutionsRow *>& get() const ;
	


 
	
	/**
 	 * Returns a CalAntennaSolutionsRow* given a key.
 	 * @return a pointer to the row having the key whose values are passed as parameters, or 0 if
 	 * no row exists for that key.
	
	 * @param antennaName
	
	 * @param atmPhaseCorrection
	
	 * @param receiverBand
	
	 * @param basebandName
	
	 * @param calDataId
	
	 * @param calReductionId
	
	 * @param spectralWindowId
	
 	 *
	 */
 	CalAntennaSolutionsRow* getRowByKey(std::string antennaName, AtmPhaseCorrectionMod::AtmPhaseCorrection atmPhaseCorrection, ReceiverBandMod::ReceiverBand receiverBand, BasebandNameMod::BasebandName basebandName, Tag calDataId, Tag calReductionId, Tag spectralWindowId);

 	 	



	/**
 	 * Look up the table for a row whose all attributes 
 	 * are equal to the corresponding parameters of the method.
 	 * @return a pointer on this row if any, null otherwise.
 	 *
			
 	 * @param antennaName
 	 		
 	 * @param atmPhaseCorrection
 	 		
 	 * @param receiverBand
 	 		
 	 * @param basebandName
 	 		
 	 * @param calDataId
 	 		
 	 * @param calReductionId
 	 		
 	 * @param spectralWindowId
 	 		
 	 * @param startValidTime
 	 		
 	 * @param endValidTime
 	 		
 	 * @param numReceptor
 	 		
 	 * @param refAntennaName
 	 		
 	 * @param direction
 	 		
 	 * @param frequencyRange
 	 		
 	 * @param integrationTime
 	 		
 	 * @param polarizationTypes
 	 		
 	 * @param correctionValidity
 	 		
 	 * @param phaseAnt
 	 		
 	 * @param phaseAntRMS
 	 		
 	 * @param amplitudeAnt
 	 		
 	 * @param amplitudeAntRMS
 	 		 
 	 */
	CalAntennaSolutionsRow* lookup(std::string antennaName, AtmPhaseCorrectionMod::AtmPhaseCorrection atmPhaseCorrection, ReceiverBandMod::ReceiverBand receiverBand, BasebandNameMod::BasebandName basebandName, Tag calDataId, Tag calReductionId, Tag spectralWindowId, ArrayTime startValidTime, ArrayTime endValidTime, int numReceptor, std::string refAntennaName, std::vector<Angle > direction, std::vector<Frequency > frequencyRange, Interval integrationTime, std::vector<PolarizationTypeMod::PolarizationType > polarizationTypes, bool correctionValidity, std::vector<float > phaseAnt, std::vector<float > phaseAntRMS, std::vector<float > amplitudeAnt, std::vector<float > amplitudeAntRMS); 


	void setUnknownAttributeBinaryReader(const std::string& attributeName, BinaryAttributeReaderFunctor* barFctr);
	BinaryAttributeReaderFunctor* getUnknownAttributeBinaryReader(const std::string& attributeName) const;

private:

	/**
	 * Create a CalAntennaSolutionsTable.
	 * <p>
	 * This constructor is private because only the
	 * container can create tables.  All tables must know the container
	 * to which they belong.
	 * @param container The container to which this table belongs.
	 */ 
	CalAntennaSolutionsTable (ASDM & container);

	ASDM & container;
	
	bool archiveAsBin; // If true archive binary else archive XML
	bool fileAsBin ; // If true file binary else file XML	
	
	std::string version ; 
	
	Entity entity;
	

	
	


	/**
	 * If this table has an autoincrementable attribute then check if *x verifies the rule of uniqueness and throw exception if not.
	 * Check if *x verifies the key uniqueness rule and throw an exception if not.
	 * Append x to its table.
	 * @throws DuplicateKey
	 
	 */
	CalAntennaSolutionsRow* checkAndAdd(CalAntennaSolutionsRow* x, bool skipCheckUniqueness=false) ;
	
	/**
	 * Brutally append an CalAntennaSolutionsRow x to the collection of rows already stored in this table. No uniqueness check is done !
	 *
	 * @param CalAntennaSolutionsRow* x a pointer onto the CalAntennaSolutionsRow to be appended.
	 */
	 void append(CalAntennaSolutionsRow* x) ;
	 
	/**
	 * Brutally append an CalAntennaSolutionsRow x to the collection of rows already stored in this table. No uniqueness check is done !
	 *
	 * @param CalAntennaSolutionsRow* x a pointer onto the CalAntennaSolutionsRow to be appended.
	 */
	 void addWithoutCheckingUnique(CalAntennaSolutionsRow* x) ;
	 
	 



// A data structure to store the pointers on the table's rows.

// In all cases we maintain a private vector of CalAntennaSolutionsRow s.
   std::vector<CalAntennaSolutionsRow * > privateRows;
   

			
	std::vector<CalAntennaSolutionsRow *> row;

	
	void error() ; //throw(ConversionException);

	
	/**
	 * Populate this table from the content of a XML document that is required to
	 * be conform to the XML schema defined for a CalAntennaSolutions (CalAntennaSolutionsTable.xsd).
	 * @throws ConversionException
	 * 
	 */
	void fromXML(std::string& xmlDoc) ;
		
	std::map<std::string, BinaryAttributeReaderFunctor *> unknownAttributes2Functors;

	/**
	  * Private methods involved during the build of this table out of the content
	  * of file(s) containing an external representation of a CalAntennaSolutions table.
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
	  * will be saved in a file "CalAntennaSolutions.bin" or an XML representation (fileAsBin==false) will be saved in a file "CalAntennaSolutions.xml".
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
	 * Reads and parses a file containing a representation of a CalAntennaSolutionsTable as those produced  by the toFile method.
	 * This table is populated with the result of the parsing.
	 * @param directory The name of the directory containing the file te be read and parsed.
	 * @throws ConversionException If any error occurs while reading the 
	 * files in the directory or parsing them.
	 *
	 */
	 void setFromFile(const std::string& directory);	
 
};

} // End namespace asdm

#endif /* CalAntennaSolutionsTable_CLASS */
