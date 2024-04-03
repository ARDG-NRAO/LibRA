
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
 * File CalPointingModelTable.h
 */
 
#ifndef CalPointingModelTable_CLASS
#define CalPointingModelTable_CLASS

#include <string>
#include <vector>
#include <map>



	
#include <alma/ASDM/Angle.h>
	

	
#include <alma/ASDM/ArrayTime.h>
	

	
#include <alma/ASDM/Tag.h>
	




	

	
#include <alma/Enumerations/CReceiverBand.h>
	

	

	

	
#include <alma/Enumerations/CAntennaMake.h>
	

	
#include <alma/Enumerations/CPointingModelMode.h>
	

	
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
//class asdm::CalPointingModelRow;

class ASDM;
class CalPointingModelRow;
/**
 * The CalPointingModelTable class is an Alma table.
 * <BR>
 * 
 * \par Role
 * Result of pointing model calibration performed by TelCal.
 * <BR>
 
 * Generated from model's revision "-1", branch ""
 *
 * <TABLE BORDER="1">
 * <CAPTION> Attributes of CalPointingModel </CAPTION>
 * <TR BGCOLOR="#AAAAAA"> <TH> Name </TH> <TH> Type </TH> <TH> Expected shape  </TH> <TH> Comment </TH></TR>
 
 * <TR> <TH BGCOLOR="#CCCCCC" colspan="4" align="center"> Key </TD></TR>
	
 * <TR>
 		
 * <TD> antennaName </TD>
 		 
 * <TD> std::string</TD>
 * <TD> &nbsp; </TD>
 * <TD> &nbsp;the name of the antenna. </TD>
 * </TR>
	
 * <TR>
 		
 * <TD> receiverBand </TD>
 		 
 * <TD> ReceiverBandMod::ReceiverBand</TD>
 * <TD> &nbsp; </TD>
 * <TD> &nbsp;identifies the receiver band. </TD>
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
 * <TD> antennaMake </TD> 
 * <TD> AntennaMakeMod::AntennaMake </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;the antenna make. </TD>
 * </TR>
	
 * <TR>
 * <TD> pointingModelMode </TD> 
 * <TD> PointingModelModeMod::PointingModelMode </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;identifies the pointing model mode. </TD>
 * </TR>
	
 * <TR>
 * <TD> polarizationType </TD> 
 * <TD> PolarizationTypeMod::PolarizationType </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;identifies the polarization type. </TD>
 * </TR>
	
 * <TR>
 * <TD> numCoeff (numCoeff)</TD> 
 * <TD> int </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;the number of coefficients in the pointing model. </TD>
 * </TR>
	
 * <TR>
 * <TD> coeffName </TD> 
 * <TD> std::vector<std::string > </TD>
 * <TD>  numCoeff </TD> 
 * <TD> &nbsp;the names of the coefficients (one string per coefficient). </TD>
 * </TR>
	
 * <TR>
 * <TD> coeffVal </TD> 
 * <TD> std::vector<float > </TD>
 * <TD>  numCoeff </TD> 
 * <TD> &nbsp;the values of the coefficients resulting from the pointing model fitting (one value per coefficient). </TD>
 * </TR>
	
 * <TR>
 * <TD> coeffError </TD> 
 * <TD> std::vector<float > </TD>
 * <TD>  numCoeff </TD> 
 * <TD> &nbsp;the uncertainties on the pointing model coefficients (one value per coefficient). </TD>
 * </TR>
	
 * <TR>
 * <TD> coeffFixed </TD> 
 * <TD> std::vector<bool > </TD>
 * <TD>  numCoeff </TD> 
 * <TD> &nbsp;indicates if one coefficient was fixed (true) or not fixed (false) (one boolean per coefficient). </TD>
 * </TR>
	
 * <TR>
 * <TD> azimuthRMS </TD> 
 * <TD> Angle </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;Azimuth RMS (on Sky) </TD>
 * </TR>
	
 * <TR>
 * <TD> elevationRms </TD> 
 * <TD> Angle </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;Elevation rms (on Sky) </TD>
 * </TR>
	
 * <TR>
 * <TD> skyRMS </TD> 
 * <TD> Angle </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;rms on sky </TD>
 * </TR>
	
 * <TR>
 * <TD> reducedChiSquared </TD> 
 * <TD> double </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;measures the quality of the least square fit. </TD>
 * </TR>
	


 * <TR> <TH BGCOLOR="#CCCCCC"  colspan="4" valign="center"> Value <br> (Optional) </TH></TR>
	
 * <TR>
 * <TD> numObs(numObs)</TD> 
 * <TD> int </TD>
 * <TD>  &nbsp; </TD>
 * <TD>&nbsp; the number of source directions observed to derive the pointing model. </TD>
 * </TR>
	
 * <TR>
 * <TD> coeffFormula</TD> 
 * <TD> std::vector<std::string > </TD>
 * <TD>  numCoeff  </TD>
 * <TD>&nbsp; formulas used for the fitting (one string per coefficient). </TD>
 * </TR>
	

 * </TABLE>
 */
class CalPointingModelTable : public Representable {
	friend class ASDM;

public:


	/**
	 * Return the list of field names that make up key key
	 * as an array of strings.
	 * @return a vector of string.
	 */	
	static const std::vector<std::string>& getKeyName();


	virtual ~CalPointingModelTable();
	
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
	 * to the schema defined for CalPointingModel (CalPointingModelTable.xsd).
	 *
	 * @returns a string containing the XML representation.
	 * @throws ConversionException
	 */
	std::string toXML()  ;

#ifndef WITHOUT_ACS
	// Conversion Methods
	/**
	 * Convert this table into a CalPointingModelTableIDL CORBA structure.
	 *
	 * @return a pointer to a CalPointingModelTableIDL
	 */
	asdmIDL::CalPointingModelTableIDL *toIDL() ;
	
	/**
	 * Fills the CORBA data structure passed in parameter
	 * with the content of this table.
	 *
	 * @param x a reference to the asdmIDL::CalPointingModelTableIDL to be populated
	 * with the content of this.
	 */
	 void toIDL(asdmIDL::CalPointingModelTableIDL& x) const;
	 
#endif

#ifndef WITHOUT_ACS
	/**
	 * Populate this table from the content of a CalPointingModelTableIDL Corba structure.
	 *
	 * @throws DuplicateKey Thrown if the method tries to add a row having a key that is already in the table.
	 * @throws ConversionException
	 */	
	void fromIDL(asdmIDL::CalPointingModelTableIDL x) ;
#endif
	
	//
	// ====> Row creation.
	//
	
	/**
	 * Create a new row with default values.
	 * @return a pointer on a CalPointingModelRow
	 */
	CalPointingModelRow *newRow();
	
	
	/**
	 * Create a new row initialized to the specified values.
	 * @return a pointer on the created and initialized row.
	
 	 * @param antennaName
	
 	 * @param receiverBand
	
 	 * @param calDataId
	
 	 * @param calReductionId
	
 	 * @param startValidTime
	
 	 * @param endValidTime
	
 	 * @param antennaMake
	
 	 * @param pointingModelMode
	
 	 * @param polarizationType
	
 	 * @param numCoeff
	
 	 * @param coeffName
	
 	 * @param coeffVal
	
 	 * @param coeffError
	
 	 * @param coeffFixed
	
 	 * @param azimuthRMS
	
 	 * @param elevationRms
	
 	 * @param skyRMS
	
 	 * @param reducedChiSquared
	
     */
	CalPointingModelRow *newRow(std::string antennaName, ReceiverBandMod::ReceiverBand receiverBand, Tag calDataId, Tag calReductionId, ArrayTime startValidTime, ArrayTime endValidTime, AntennaMakeMod::AntennaMake antennaMake, PointingModelModeMod::PointingModelMode pointingModelMode, PolarizationTypeMod::PolarizationType polarizationType, int numCoeff, std::vector<std::string > coeffName, std::vector<float > coeffVal, std::vector<float > coeffError, std::vector<bool > coeffFixed, Angle azimuthRMS, Angle elevationRms, Angle skyRMS, double reducedChiSquared);
	


	/**
	 * Create a new row using a copy constructor mechanism.
	 * 
	 * The method creates a new CalPointingModelRow owned by this. Each attribute of the created row 
	 * is a (deep) copy of the corresponding attribute of row. The method does not add 
	 * the created row to this, its simply parents it to this, a call to the add method
	 * has to be done in order to get the row added (very likely after having modified
	 * some of its attributes).
	 * If row is null then the method returns a new CalPointingModelRow with default values for its attributes. 
	 *
	 * @param row the row which is to be copied.
	 */
	 CalPointingModelRow *newRow(CalPointingModelRow *row); 

	//
	// ====> Append a row to its table.
	//
 
	
	/**
	 * Add a row.
	 * @param x a pointer to the CalPointingModelRow to be added.
	 *
	 * @return a pointer to a CalPointingModelRow. If the table contains a CalPointingModelRow whose attributes (key and mandatory values) are equal to x ones
	 * then returns a pointer on that CalPointingModelRow, otherwise returns x.
	 *
	 * @throw DuplicateKey { thrown when the table contains a CalPointingModelRow with a key equal to the x one but having
	 * and a value section different from x one }
	 *
	
	 */
	CalPointingModelRow* add(CalPointingModelRow* x) ; 

 



	//
	// ====> Methods returning rows.
	//
		
	/**
	 * Get a collection of pointers on the rows of the table.
	 * @return Alls rows in a vector of pointers of CalPointingModelRow. The elements of this vector are stored in the order 
	 * in which they have been added to the CalPointingModelTable.
	 */
	std::vector<CalPointingModelRow *> get() ;
	
	/**
	 * Get a const reference on the collection of rows pointers internally hold by the table.
	 * @return A const reference of a vector of pointers of CalPointingModelRow. The elements of this vector are stored in the order 
	 * in which they have been added to the CalPointingModelTable.
	 *
	 */
	 const std::vector<CalPointingModelRow *>& get() const ;
	


 
	
	/**
 	 * Returns a CalPointingModelRow* given a key.
 	 * @return a pointer to the row having the key whose values are passed as parameters, or 0 if
 	 * no row exists for that key.
	
	 * @param antennaName
	
	 * @param receiverBand
	
	 * @param calDataId
	
	 * @param calReductionId
	
 	 *
	 */
 	CalPointingModelRow* getRowByKey(std::string antennaName, ReceiverBandMod::ReceiverBand receiverBand, Tag calDataId, Tag calReductionId);

 	 	



	/**
 	 * Look up the table for a row whose all attributes 
 	 * are equal to the corresponding parameters of the method.
 	 * @return a pointer on this row if any, null otherwise.
 	 *
			
 	 * @param antennaName
 	 		
 	 * @param receiverBand
 	 		
 	 * @param calDataId
 	 		
 	 * @param calReductionId
 	 		
 	 * @param startValidTime
 	 		
 	 * @param endValidTime
 	 		
 	 * @param antennaMake
 	 		
 	 * @param pointingModelMode
 	 		
 	 * @param polarizationType
 	 		
 	 * @param numCoeff
 	 		
 	 * @param coeffName
 	 		
 	 * @param coeffVal
 	 		
 	 * @param coeffError
 	 		
 	 * @param coeffFixed
 	 		
 	 * @param azimuthRMS
 	 		
 	 * @param elevationRms
 	 		
 	 * @param skyRMS
 	 		
 	 * @param reducedChiSquared
 	 		 
 	 */
	CalPointingModelRow* lookup(std::string antennaName, ReceiverBandMod::ReceiverBand receiverBand, Tag calDataId, Tag calReductionId, ArrayTime startValidTime, ArrayTime endValidTime, AntennaMakeMod::AntennaMake antennaMake, PointingModelModeMod::PointingModelMode pointingModelMode, PolarizationTypeMod::PolarizationType polarizationType, int numCoeff, std::vector<std::string > coeffName, std::vector<float > coeffVal, std::vector<float > coeffError, std::vector<bool > coeffFixed, Angle azimuthRMS, Angle elevationRms, Angle skyRMS, double reducedChiSquared); 


	void setUnknownAttributeBinaryReader(const std::string& attributeName, BinaryAttributeReaderFunctor* barFctr);
	BinaryAttributeReaderFunctor* getUnknownAttributeBinaryReader(const std::string& attributeName) const;

private:

	/**
	 * Create a CalPointingModelTable.
	 * <p>
	 * This constructor is private because only the
	 * container can create tables.  All tables must know the container
	 * to which they belong.
	 * @param container The container to which this table belongs.
	 */ 
	CalPointingModelTable (ASDM & container);

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
	CalPointingModelRow* checkAndAdd(CalPointingModelRow* x, bool skipCheckUniqueness=false) ;
	
	/**
	 * Brutally append an CalPointingModelRow x to the collection of rows already stored in this table. No uniqueness check is done !
	 *
	 * @param CalPointingModelRow* x a pointer onto the CalPointingModelRow to be appended.
	 */
	 void append(CalPointingModelRow* x) ;
	 
	/**
	 * Brutally append an CalPointingModelRow x to the collection of rows already stored in this table. No uniqueness check is done !
	 *
	 * @param CalPointingModelRow* x a pointer onto the CalPointingModelRow to be appended.
	 */
	 void addWithoutCheckingUnique(CalPointingModelRow* x) ;
	 
	 



// A data structure to store the pointers on the table's rows.

// In all cases we maintain a private vector of CalPointingModelRow s.
   std::vector<CalPointingModelRow * > privateRows;
   

			
	std::vector<CalPointingModelRow *> row;

	
	void error() ; //throw(ConversionException);

	
	/**
	 * Populate this table from the content of a XML document that is required to
	 * be conform to the XML schema defined for a CalPointingModel (CalPointingModelTable.xsd).
	 * @throws ConversionException
	 * 
	 */
	void fromXML(std::string& xmlDoc) ;
		
	std::map<std::string, BinaryAttributeReaderFunctor *> unknownAttributes2Functors;

	/**
	  * Private methods involved during the build of this table out of the content
	  * of file(s) containing an external representation of a CalPointingModel table.
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
	  * will be saved in a file "CalPointingModel.bin" or an XML representation (fileAsBin==false) will be saved in a file "CalPointingModel.xml".
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
	 * Reads and parses a file containing a representation of a CalPointingModelTable as those produced  by the toFile method.
	 * This table is populated with the result of the parsing.
	 * @param directory The name of the directory containing the file te be read and parsed.
	 * @throws ConversionException If any error occurs while reading the 
	 * files in the directory or parsing them.
	 *
	 */
	 void setFromFile(const std::string& directory);	
 
};

} // End namespace asdm

#endif /* CalPointingModelTable_CLASS */
