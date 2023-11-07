
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
 * File CalWVRTable.h
 */
 
#ifndef CalWVRTable_CLASS
#define CalWVRTable_CLASS

#include <string>
#include <vector>
#include <map>



	
#include <alma/ASDM/Temperature.h>
	

	
#include <alma/ASDM/ArrayTime.h>
	

	
#include <alma/ASDM/Length.h>
	

	
#include <alma/ASDM/Frequency.h>
	

	
#include <alma/ASDM/Tag.h>
	




	

	

	
#include <alma/Enumerations/CWVRMethod.h>
	

	

	

	

	

	

	

	

	

	

	

	

	

	

	



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
//class asdm::CalWVRRow;

class ASDM;
class CalWVRRow;
/**
 * The CalWVRTable class is an Alma table.
 * <BR>
 * 
 * \par Role
 * Result of the water vapour radiometric  calibration performed by TelCal. 
 * <BR>
 
 * Generated from model's revision "-1", branch ""
 *
 * <TABLE BORDER="1">
 * <CAPTION> Attributes of CalWVR </CAPTION>
 * <TR BGCOLOR="#AAAAAA"> <TH> Name </TH> <TH> Type </TH> <TH> Expected shape  </TH> <TH> Comment </TH></TR>
 
 * <TR> <TH BGCOLOR="#CCCCCC" colspan="4" align="center"> Key </TD></TR>
	
 * <TR>
 		
 * <TD> antennaName </TD>
 		 
 * <TD> std::string</TD>
 * <TD> &nbsp; </TD>
 * <TD> &nbsp;the name of the antenna. </TD>
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
 * <TD> &nbsp;refers to unique row  in CalReductionTable. </TD>
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
 * <TD> wvrMethod </TD> 
 * <TD> WVRMethodMod::WVRMethod </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;identifies the method used for the calibration. </TD>
 * </TR>
	
 * <TR>
 * <TD> numInputAntennas (numInputAntennas)</TD> 
 * <TD> int </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;the number of input antennas (i.e. equiped with functional WVRs). </TD>
 * </TR>
	
 * <TR>
 * <TD> inputAntennaNames </TD> 
 * <TD> std::vector<std::string > </TD>
 * <TD>  numInputAntennas </TD> 
 * <TD> &nbsp;the names of the input antennas (one string per antenna). </TD>
 * </TR>
	
 * <TR>
 * <TD> numChan (numChan)</TD> 
 * <TD> int </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;the number of frequency channels in the WVR receiver. </TD>
 * </TR>
	
 * <TR>
 * <TD> chanFreq </TD> 
 * <TD> std::vector<Frequency > </TD>
 * <TD>  numChan </TD> 
 * <TD> &nbsp;the channel frequencies (one value per channel). </TD>
 * </TR>
	
 * <TR>
 * <TD> chanWidth </TD> 
 * <TD> std::vector<Frequency > </TD>
 * <TD>  numChan </TD> 
 * <TD> &nbsp;the widths of the channels (one value per channel). </TD>
 * </TR>
	
 * <TR>
 * <TD> refTemp </TD> 
 * <TD> std::vector<std::vector<Temperature > > </TD>
 * <TD>  numInputAntennas, numChan </TD> 
 * <TD> &nbsp;the reference temperatures (one value per input antenna per channel). </TD>
 * </TR>
	
 * <TR>
 * <TD> numPoly (numPoly)</TD> 
 * <TD> int </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;the number of polynomial coefficients. </TD>
 * </TR>
	
 * <TR>
 * <TD> pathCoeff </TD> 
 * <TD> std::vector<std::vector<std::vector<float > > > </TD>
 * <TD>  numInputAntennas, numChan, numPoly </TD> 
 * <TD> &nbsp;the path length coefficients (one value per input antenna per channel per polynomial coefficient). </TD>
 * </TR>
	
 * <TR>
 * <TD> polyFreqLimits </TD> 
 * <TD> std::vector<Frequency > </TD>
 * <TD>  2 </TD> 
 * <TD> &nbsp;the limits of the interval of frequencies for which the path length coefficients are computed. </TD>
 * </TR>
	
 * <TR>
 * <TD> wetPath </TD> 
 * <TD> std::vector<float > </TD>
 * <TD>  numPoly </TD> 
 * <TD> &nbsp;The wet path as a function frequency (expressed as a polynomial). </TD>
 * </TR>
	
 * <TR>
 * <TD> dryPath </TD> 
 * <TD> std::vector<float > </TD>
 * <TD>  numPoly </TD> 
 * <TD> &nbsp;The dry path as a function frequency (expressed as a polynomial). </TD>
 * </TR>
	
 * <TR>
 * <TD> water </TD> 
 * <TD> Length </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;The precipitable water vapor corresponding to the reference model. </TD>
 * </TR>
	


 * <TR> <TH BGCOLOR="#CCCCCC"  colspan="4" valign="center"> Value <br> (Optional) </TH></TR>
	
 * <TR>
 * <TD> tauBaseline</TD> 
 * <TD> float </TD>
 * <TD>  &nbsp; </TD>
 * <TD>&nbsp; Constant opacity term (frequency independent). </TD>
 * </TR>
	

 * </TABLE>
 */
class CalWVRTable : public Representable {
	friend class ASDM;

public:


	/**
	 * Return the list of field names that make up key key
	 * as an array of strings.
	 * @return a vector of string.
	 */	
	static const std::vector<std::string>& getKeyName();


	virtual ~CalWVRTable();
	
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
	 * to the schema defined for CalWVR (CalWVRTable.xsd).
	 *
	 * @returns a string containing the XML representation.
	 * @throws ConversionException
	 */
	std::string toXML()  ;

#ifndef WITHOUT_ACS
	// Conversion Methods
	/**
	 * Convert this table into a CalWVRTableIDL CORBA structure.
	 *
	 * @return a pointer to a CalWVRTableIDL
	 */
	asdmIDL::CalWVRTableIDL *toIDL() ;
	
	/**
	 * Fills the CORBA data structure passed in parameter
	 * with the content of this table.
	 *
	 * @param x a reference to the asdmIDL::CalWVRTableIDL to be populated
	 * with the content of this.
	 */
	 void toIDL(asdmIDL::CalWVRTableIDL& x) const;
	 
#endif

#ifndef WITHOUT_ACS
	/**
	 * Populate this table from the content of a CalWVRTableIDL Corba structure.
	 *
	 * @throws DuplicateKey Thrown if the method tries to add a row having a key that is already in the table.
	 * @throws ConversionException
	 */	
	void fromIDL(asdmIDL::CalWVRTableIDL x) ;
#endif
	
	//
	// ====> Row creation.
	//
	
	/**
	 * Create a new row with default values.
	 * @return a pointer on a CalWVRRow
	 */
	CalWVRRow *newRow();
	
	
	/**
	 * Create a new row initialized to the specified values.
	 * @return a pointer on the created and initialized row.
	
 	 * @param antennaName
	
 	 * @param calDataId
	
 	 * @param calReductionId
	
 	 * @param startValidTime
	
 	 * @param endValidTime
	
 	 * @param wvrMethod
	
 	 * @param numInputAntennas
	
 	 * @param inputAntennaNames
	
 	 * @param numChan
	
 	 * @param chanFreq
	
 	 * @param chanWidth
	
 	 * @param refTemp
	
 	 * @param numPoly
	
 	 * @param pathCoeff
	
 	 * @param polyFreqLimits
	
 	 * @param wetPath
	
 	 * @param dryPath
	
 	 * @param water
	
     */
	CalWVRRow *newRow(std::string antennaName, Tag calDataId, Tag calReductionId, ArrayTime startValidTime, ArrayTime endValidTime, WVRMethodMod::WVRMethod wvrMethod, int numInputAntennas, std::vector<std::string > inputAntennaNames, int numChan, std::vector<Frequency > chanFreq, std::vector<Frequency > chanWidth, std::vector<std::vector<Temperature > > refTemp, int numPoly, std::vector<std::vector<std::vector<float > > > pathCoeff, std::vector<Frequency > polyFreqLimits, std::vector<float > wetPath, std::vector<float > dryPath, Length water);
	


	/**
	 * Create a new row using a copy constructor mechanism.
	 * 
	 * The method creates a new CalWVRRow owned by this. Each attribute of the created row 
	 * is a (deep) copy of the corresponding attribute of row. The method does not add 
	 * the created row to this, its simply parents it to this, a call to the add method
	 * has to be done in order to get the row added (very likely after having modified
	 * some of its attributes).
	 * If row is null then the method returns a new CalWVRRow with default values for its attributes. 
	 *
	 * @param row the row which is to be copied.
	 */
	 CalWVRRow *newRow(CalWVRRow *row); 

	//
	// ====> Append a row to its table.
	//
 
	
	/**
	 * Add a row.
	 * @param x a pointer to the CalWVRRow to be added.
	 *
	 * @return a pointer to a CalWVRRow. If the table contains a CalWVRRow whose attributes (key and mandatory values) are equal to x ones
	 * then returns a pointer on that CalWVRRow, otherwise returns x.
	 *
	 * @throw DuplicateKey { thrown when the table contains a CalWVRRow with a key equal to the x one but having
	 * and a value section different from x one }
	 *
	
	 */
	CalWVRRow* add(CalWVRRow* x) ; 

 



	//
	// ====> Methods returning rows.
	//
		
	/**
	 * Get a collection of pointers on the rows of the table.
	 * @return Alls rows in a vector of pointers of CalWVRRow. The elements of this vector are stored in the order 
	 * in which they have been added to the CalWVRTable.
	 */
	std::vector<CalWVRRow *> get() ;
	
	/**
	 * Get a const reference on the collection of rows pointers internally hold by the table.
	 * @return A const reference of a vector of pointers of CalWVRRow. The elements of this vector are stored in the order 
	 * in which they have been added to the CalWVRTable.
	 *
	 */
	 const std::vector<CalWVRRow *>& get() const ;
	


 
	
	/**
 	 * Returns a CalWVRRow* given a key.
 	 * @return a pointer to the row having the key whose values are passed as parameters, or 0 if
 	 * no row exists for that key.
	
	 * @param antennaName
	
	 * @param calDataId
	
	 * @param calReductionId
	
 	 *
	 */
 	CalWVRRow* getRowByKey(std::string antennaName, Tag calDataId, Tag calReductionId);

 	 	



	/**
 	 * Look up the table for a row whose all attributes 
 	 * are equal to the corresponding parameters of the method.
 	 * @return a pointer on this row if any, null otherwise.
 	 *
			
 	 * @param antennaName
 	 		
 	 * @param calDataId
 	 		
 	 * @param calReductionId
 	 		
 	 * @param startValidTime
 	 		
 	 * @param endValidTime
 	 		
 	 * @param wvrMethod
 	 		
 	 * @param numInputAntennas
 	 		
 	 * @param inputAntennaNames
 	 		
 	 * @param numChan
 	 		
 	 * @param chanFreq
 	 		
 	 * @param chanWidth
 	 		
 	 * @param refTemp
 	 		
 	 * @param numPoly
 	 		
 	 * @param pathCoeff
 	 		
 	 * @param polyFreqLimits
 	 		
 	 * @param wetPath
 	 		
 	 * @param dryPath
 	 		
 	 * @param water
 	 		 
 	 */
	CalWVRRow* lookup(std::string antennaName, Tag calDataId, Tag calReductionId, ArrayTime startValidTime, ArrayTime endValidTime, WVRMethodMod::WVRMethod wvrMethod, int numInputAntennas, std::vector<std::string > inputAntennaNames, int numChan, std::vector<Frequency > chanFreq, std::vector<Frequency > chanWidth, std::vector<std::vector<Temperature > > refTemp, int numPoly, std::vector<std::vector<std::vector<float > > > pathCoeff, std::vector<Frequency > polyFreqLimits, std::vector<float > wetPath, std::vector<float > dryPath, Length water); 


	void setUnknownAttributeBinaryReader(const std::string& attributeName, BinaryAttributeReaderFunctor* barFctr);
	BinaryAttributeReaderFunctor* getUnknownAttributeBinaryReader(const std::string& attributeName) const;

private:

	/**
	 * Create a CalWVRTable.
	 * <p>
	 * This constructor is private because only the
	 * container can create tables.  All tables must know the container
	 * to which they belong.
	 * @param container The container to which this table belongs.
	 */ 
	CalWVRTable (ASDM & container);

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
	CalWVRRow* checkAndAdd(CalWVRRow* x, bool skipCheckUniqueness=false) ;
	
	/**
	 * Brutally append an CalWVRRow x to the collection of rows already stored in this table. No uniqueness check is done !
	 *
	 * @param CalWVRRow* x a pointer onto the CalWVRRow to be appended.
	 */
	 void append(CalWVRRow* x) ;
	 
	/**
	 * Brutally append an CalWVRRow x to the collection of rows already stored in this table. No uniqueness check is done !
	 *
	 * @param CalWVRRow* x a pointer onto the CalWVRRow to be appended.
	 */
	 void addWithoutCheckingUnique(CalWVRRow* x) ;
	 
	 



// A data structure to store the pointers on the table's rows.

// In all cases we maintain a private vector of CalWVRRow s.
   std::vector<CalWVRRow * > privateRows;
   

			
	std::vector<CalWVRRow *> row;

	
	void error() ; //throw(ConversionException);

	
	/**
	 * Populate this table from the content of a XML document that is required to
	 * be conform to the XML schema defined for a CalWVR (CalWVRTable.xsd).
	 * @throws ConversionException
	 * 
	 */
	void fromXML(std::string& xmlDoc) ;
		
	std::map<std::string, BinaryAttributeReaderFunctor *> unknownAttributes2Functors;

	/**
	  * Private methods involved during the build of this table out of the content
	  * of file(s) containing an external representation of a CalWVR table.
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
	  * will be saved in a file "CalWVR.bin" or an XML representation (fileAsBin==false) will be saved in a file "CalWVR.xml".
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
	 * Reads and parses a file containing a representation of a CalWVRTable as those produced  by the toFile method.
	 * This table is populated with the result of the parsing.
	 * @param directory The name of the directory containing the file te be read and parsed.
	 * @throws ConversionException If any error occurs while reading the 
	 * files in the directory or parsing them.
	 *
	 */
	 void setFromFile(const std::string& directory);	
 
};

} // End namespace asdm

#endif /* CalWVRTable_CLASS */
