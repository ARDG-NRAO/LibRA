
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
 * File CalSeeingTable.h
 */
 
#ifndef CalSeeingTable_CLASS
#define CalSeeingTable_CLASS

#include <string>
#include <vector>
#include <map>



	
#include <alma/ASDM/Angle.h>
	

	
#include <alma/ASDM/ArrayTime.h>
	

	
#include <alma/ASDM/Length.h>
	

	
#include <alma/ASDM/Frequency.h>
	

	
#include <alma/ASDM/Tag.h>
	

	
#include <alma/ASDM/Interval.h>
	




	
#include <alma/Enumerations/CAtmPhaseCorrection.h>
	

	

	

	

	

	

	

	

	

	

	

	

	



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
//class asdm::CalSeeingRow;

class ASDM;
class CalSeeingRow;
/**
 * The CalSeeingTable class is an Alma table.
 * <BR>
 * 
 * \par Role
 * The seeing parameters deduced from TelCal calibrations.
 * <BR>
 
 * Generated from model's revision "-1", branch ""
 *
 * <TABLE BORDER="1">
 * <CAPTION> Attributes of CalSeeing </CAPTION>
 * <TR BGCOLOR="#AAAAAA"> <TH> Name </TH> <TH> Type </TH> <TH> Expected shape  </TH> <TH> Comment </TH></TR>
 
 * <TR> <TH BGCOLOR="#CCCCCC" colspan="4" align="center"> Key </TD></TR>
	
 * <TR>
 		
 * <TD> atmPhaseCorrection </TD>
 		 
 * <TD> AtmPhaseCorrectionMod::AtmPhaseCorrection</TD>
 * <TD> &nbsp; </TD>
 * <TD> &nbsp;describes how the atmospheric phase correction has been applied. </TD>
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
 * <TD> frequencyRange </TD> 
 * <TD> std::vector<Frequency > </TD>
 * <TD>  2 </TD> 
 * <TD> &nbsp;the range of frequencies over which this result is valid. </TD>
 * </TR>
	
 * <TR>
 * <TD> integrationTime </TD> 
 * <TD> Interval </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;the duration of averaging for the evaluation of the RMS. </TD>
 * </TR>
	
 * <TR>
 * <TD> numBaseLengths (numBaseLengths)</TD> 
 * <TD> int </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;the number of baselines for which the the RMS phase data is evaluated. </TD>
 * </TR>
	
 * <TR>
 * <TD> baselineLengths </TD> 
 * <TD> std::vector<Length > </TD>
 * <TD>  numBaseLengths </TD> 
 * <TD> &nbsp;the lengths of the baselines (one value per baseline). </TD>
 * </TR>
	
 * <TR>
 * <TD> phaseRMS </TD> 
 * <TD> std::vector<Angle > </TD>
 * <TD>  numBaseLengths </TD> 
 * <TD> &nbsp;the RMS of phase fluctuations (one value per baseline). </TD>
 * </TR>
	
 * <TR>
 * <TD> seeing </TD> 
 * <TD> Angle </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;the seeing parameter, deduced for the LO1. </TD>
 * </TR>
	
 * <TR>
 * <TD> seeingError </TD> 
 * <TD> Angle </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;the uncertainty on the seeing parameter. </TD>
 * </TR>
	


 * <TR> <TH BGCOLOR="#CCCCCC"  colspan="4" valign="center"> Value <br> (Optional) </TH></TR>
	
 * <TR>
 * <TD> exponent</TD> 
 * <TD> float </TD>
 * <TD>  &nbsp; </TD>
 * <TD>&nbsp; the exponent of the spatial structure function. </TD>
 * </TR>
	
 * <TR>
 * <TD> outerScale</TD> 
 * <TD> Length </TD>
 * <TD>  &nbsp; </TD>
 * <TD>&nbsp; the outer scale. </TD>
 * </TR>
	
 * <TR>
 * <TD> outerScaleRMS</TD> 
 * <TD> Angle </TD>
 * <TD>  &nbsp; </TD>
 * <TD>&nbsp; the RMS of phase fluctuations at scale length outerScale. </TD>
 * </TR>
	

 * </TABLE>
 */
class CalSeeingTable : public Representable {
	friend class ASDM;

public:


	/**
	 * Return the list of field names that make up key key
	 * as an array of strings.
	 * @return a vector of string.
	 */	
	static const std::vector<std::string>& getKeyName();


	virtual ~CalSeeingTable();
	
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
	 * to the schema defined for CalSeeing (CalSeeingTable.xsd).
	 *
	 * @returns a string containing the XML representation.
	 * @throws ConversionException
	 */
	std::string toXML()  ;

#ifndef WITHOUT_ACS
	// Conversion Methods
	/**
	 * Convert this table into a CalSeeingTableIDL CORBA structure.
	 *
	 * @return a pointer to a CalSeeingTableIDL
	 */
	asdmIDL::CalSeeingTableIDL *toIDL() ;
	
	/**
	 * Fills the CORBA data structure passed in parameter
	 * with the content of this table.
	 *
	 * @param x a reference to the asdmIDL::CalSeeingTableIDL to be populated
	 * with the content of this.
	 */
	 void toIDL(asdmIDL::CalSeeingTableIDL& x) const;
	 
#endif

#ifndef WITHOUT_ACS
	/**
	 * Populate this table from the content of a CalSeeingTableIDL Corba structure.
	 *
	 * @throws DuplicateKey Thrown if the method tries to add a row having a key that is already in the table.
	 * @throws ConversionException
	 */	
	void fromIDL(asdmIDL::CalSeeingTableIDL x) ;
#endif
	
	//
	// ====> Row creation.
	//
	
	/**
	 * Create a new row with default values.
	 * @return a pointer on a CalSeeingRow
	 */
	CalSeeingRow *newRow();
	
	
	/**
	 * Create a new row initialized to the specified values.
	 * @return a pointer on the created and initialized row.
	
 	 * @param atmPhaseCorrection
	
 	 * @param calDataId
	
 	 * @param calReductionId
	
 	 * @param startValidTime
	
 	 * @param endValidTime
	
 	 * @param frequencyRange
	
 	 * @param integrationTime
	
 	 * @param numBaseLengths
	
 	 * @param baselineLengths
	
 	 * @param phaseRMS
	
 	 * @param seeing
	
 	 * @param seeingError
	
     */
	CalSeeingRow *newRow(AtmPhaseCorrectionMod::AtmPhaseCorrection atmPhaseCorrection, Tag calDataId, Tag calReductionId, ArrayTime startValidTime, ArrayTime endValidTime, std::vector<Frequency > frequencyRange, Interval integrationTime, int numBaseLengths, std::vector<Length > baselineLengths, std::vector<Angle > phaseRMS, Angle seeing, Angle seeingError);
	


	/**
	 * Create a new row using a copy constructor mechanism.
	 * 
	 * The method creates a new CalSeeingRow owned by this. Each attribute of the created row 
	 * is a (deep) copy of the corresponding attribute of row. The method does not add 
	 * the created row to this, its simply parents it to this, a call to the add method
	 * has to be done in order to get the row added (very likely after having modified
	 * some of its attributes).
	 * If row is null then the method returns a new CalSeeingRow with default values for its attributes. 
	 *
	 * @param row the row which is to be copied.
	 */
	 CalSeeingRow *newRow(CalSeeingRow *row); 

	//
	// ====> Append a row to its table.
	//
 
	
	/**
	 * Add a row.
	 * @param x a pointer to the CalSeeingRow to be added.
	 *
	 * @return a pointer to a CalSeeingRow. If the table contains a CalSeeingRow whose attributes (key and mandatory values) are equal to x ones
	 * then returns a pointer on that CalSeeingRow, otherwise returns x.
	 *
	 * @throw DuplicateKey { thrown when the table contains a CalSeeingRow with a key equal to the x one but having
	 * and a value section different from x one }
	 *
	
	 */
	CalSeeingRow* add(CalSeeingRow* x) ; 

 



	//
	// ====> Methods returning rows.
	//
		
	/**
	 * Get a collection of pointers on the rows of the table.
	 * @return Alls rows in a vector of pointers of CalSeeingRow. The elements of this vector are stored in the order 
	 * in which they have been added to the CalSeeingTable.
	 */
	std::vector<CalSeeingRow *> get() ;
	
	/**
	 * Get a const reference on the collection of rows pointers internally hold by the table.
	 * @return A const reference of a vector of pointers of CalSeeingRow. The elements of this vector are stored in the order 
	 * in which they have been added to the CalSeeingTable.
	 *
	 */
	 const std::vector<CalSeeingRow *>& get() const ;
	


 
	
	/**
 	 * Returns a CalSeeingRow* given a key.
 	 * @return a pointer to the row having the key whose values are passed as parameters, or 0 if
 	 * no row exists for that key.
	
	 * @param atmPhaseCorrection
	
	 * @param calDataId
	
	 * @param calReductionId
	
 	 *
	 */
 	CalSeeingRow* getRowByKey(AtmPhaseCorrectionMod::AtmPhaseCorrection atmPhaseCorrection, Tag calDataId, Tag calReductionId);

 	 	



	/**
 	 * Look up the table for a row whose all attributes 
 	 * are equal to the corresponding parameters of the method.
 	 * @return a pointer on this row if any, null otherwise.
 	 *
			
 	 * @param atmPhaseCorrection
 	 		
 	 * @param calDataId
 	 		
 	 * @param calReductionId
 	 		
 	 * @param startValidTime
 	 		
 	 * @param endValidTime
 	 		
 	 * @param frequencyRange
 	 		
 	 * @param integrationTime
 	 		
 	 * @param numBaseLengths
 	 		
 	 * @param baselineLengths
 	 		
 	 * @param phaseRMS
 	 		
 	 * @param seeing
 	 		
 	 * @param seeingError
 	 		 
 	 */
	CalSeeingRow* lookup(AtmPhaseCorrectionMod::AtmPhaseCorrection atmPhaseCorrection, Tag calDataId, Tag calReductionId, ArrayTime startValidTime, ArrayTime endValidTime, std::vector<Frequency > frequencyRange, Interval integrationTime, int numBaseLengths, std::vector<Length > baselineLengths, std::vector<Angle > phaseRMS, Angle seeing, Angle seeingError); 


	void setUnknownAttributeBinaryReader(const std::string& attributeName, BinaryAttributeReaderFunctor* barFctr);
	BinaryAttributeReaderFunctor* getUnknownAttributeBinaryReader(const std::string& attributeName) const;

private:

	/**
	 * Create a CalSeeingTable.
	 * <p>
	 * This constructor is private because only the
	 * container can create tables.  All tables must know the container
	 * to which they belong.
	 * @param container The container to which this table belongs.
	 */ 
	CalSeeingTable (ASDM & container);

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
	CalSeeingRow* checkAndAdd(CalSeeingRow* x, bool skipCheckUniqueness=false) ;
	
	/**
	 * Brutally append an CalSeeingRow x to the collection of rows already stored in this table. No uniqueness check is done !
	 *
	 * @param CalSeeingRow* x a pointer onto the CalSeeingRow to be appended.
	 */
	 void append(CalSeeingRow* x) ;
	 
	/**
	 * Brutally append an CalSeeingRow x to the collection of rows already stored in this table. No uniqueness check is done !
	 *
	 * @param CalSeeingRow* x a pointer onto the CalSeeingRow to be appended.
	 */
	 void addWithoutCheckingUnique(CalSeeingRow* x) ;
	 
	 



// A data structure to store the pointers on the table's rows.

// In all cases we maintain a private vector of CalSeeingRow s.
   std::vector<CalSeeingRow * > privateRows;
   

			
	std::vector<CalSeeingRow *> row;

	
	void error() ; //throw(ConversionException);

	
	/**
	 * Populate this table from the content of a XML document that is required to
	 * be conform to the XML schema defined for a CalSeeing (CalSeeingTable.xsd).
	 * @throws ConversionException
	 * 
	 */
	void fromXML(std::string& xmlDoc) ;
		
	std::map<std::string, BinaryAttributeReaderFunctor *> unknownAttributes2Functors;

	/**
	  * Private methods involved during the build of this table out of the content
	  * of file(s) containing an external representation of a CalSeeing table.
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
	  * will be saved in a file "CalSeeing.bin" or an XML representation (fileAsBin==false) will be saved in a file "CalSeeing.xml".
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
	 * Reads and parses a file containing a representation of a CalSeeingTable as those produced  by the toFile method.
	 * This table is populated with the result of the parsing.
	 * @param directory The name of the directory containing the file te be read and parsed.
	 * @throws ConversionException If any error occurs while reading the 
	 * files in the directory or parsing them.
	 *
	 */
	 void setFromFile(const std::string& directory);	
 
};

} // End namespace asdm

#endif /* CalSeeingTable_CLASS */
