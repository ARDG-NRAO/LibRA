
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
 * File DelayModelVariableParametersTable.h
 */
 
#ifndef DelayModelVariableParametersTable_CLASS
#define DelayModelVariableParametersTable_CLASS

#include <string>
#include <vector>
#include <map>



	
#include <alma/ASDM/Angle.h>
	

	
#include <alma/ASDM/AngularRate.h>
	

	
#include <alma/ASDM/ArrayTime.h>
	

	
#include <alma/ASDM/Tag.h>
	




	

	

	

	

	
#include <alma/Enumerations/CDifferenceType.h>
	

	

	

	

	
#include <alma/Enumerations/CDifferenceType.h>
	

	

	

	

	



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
//class asdm::DelayModelVariableParametersRow;

class ASDM;
class DelayModelVariableParametersRow;
/**
 * The DelayModelVariableParametersTable class is an Alma table.
 * <BR>
 * 
 * \par Role
 * 
 * <BR>
 
 * Generated from model's revision "-1", branch ""
 *
 * <TABLE BORDER="1">
 * <CAPTION> Attributes of DelayModelVariableParameters </CAPTION>
 * <TR BGCOLOR="#AAAAAA"> <TH> Name </TH> <TH> Type </TH> <TH> Expected shape  </TH> <TH> Comment </TH></TR>
 
 * <TR> <TH BGCOLOR="#CCCCCC" colspan="4" align="center"> Key </TD></TR>
	
 * <TR>
 		
 * <TD><I> delayModelVariableParametersId </I></TD>
 		 
 * <TD> Tag</TD>
 * <TD> &nbsp; </TD>
 * <TD> &nbsp;identifies a unique row in the table. </TD>
 * </TR>
	


 * <TR> <TH BGCOLOR="#CCCCCC"  colspan="4" valign="center"> Value <br> (Mandatory) </TH></TR>
	
 * <TR>
 * <TD> time </TD> 
 * <TD> ArrayTime </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;the day and time relevant for the data in this row. </TD>
 * </TR>
	
 * <TR>
 * <TD> ut1_utc </TD> 
 * <TD> double </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;\f$ UT1 - UTC \f$ in \f$ second \f$. </TD>
 * </TR>
	
 * <TR>
 * <TD> iat_utc </TD> 
 * <TD> double </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;\f$ IAT - UTC \f$ in \f$ second \f$. </TD>
 * </TR>
	
 * <TR>
 * <TD> timeType </TD> 
 * <TD> DifferenceTypeMod::DifferenceType </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;the type of the two time differences expressed in ut1_utc and iat_utc </TD>
 * </TR>
	
 * <TR>
 * <TD> gstAtUt0 </TD> 
 * <TD> Angle </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;in \f$ radian \f$. </TD>
 * </TR>
	
 * <TR>
 * <TD> earthRotationRate </TD> 
 * <TD> AngularRate </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;in \f$ radian \ s^{-1} \f$ (the seconds are in \f$ IAT \f$).   </TD>
 * </TR>
	
 * <TR>
 * <TD> polarOffsets </TD> 
 * <TD> std::vector<double > </TD>
 * <TD>  2 </TD> 
 * <TD> &nbsp;the \f$ X, Y \f$ polar offsets in \f$ arcsec \f$. </TD>
 * </TR>
	
 * <TR>
 * <TD> polarOffsetsType </TD> 
 * <TD> DifferenceTypeMod::DifferenceType </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;the type of the polar offsets (values found in polarOffsets). </TD>
 * </TR>
	
 * <TR>
 * <TD> delayModelFixedParametersId </TD> 
 * <TD> Tag </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;refers to a unique row of the DelayModelFixedParameters table. </TD>
 * </TR>
	


 * <TR> <TH BGCOLOR="#CCCCCC"  colspan="4" valign="center"> Value <br> (Optional) </TH></TR>
	
 * <TR>
 * <TD> nutationInLongitude</TD> 
 * <TD> Angle </TD>
 * <TD>  &nbsp; </TD>
 * <TD>&nbsp; the nutation in longitude ( the part parallel to the ecliptic) in \f$ radian \f$. </TD>
 * </TR>
	
 * <TR>
 * <TD> nutationInLongitudeRate</TD> 
 * <TD> AngularRate </TD>
 * <TD>  &nbsp; </TD>
 * <TD>&nbsp; the rate of nutation in longitude in \f$ radian \ s^{-1} \f$. </TD>
 * </TR>
	
 * <TR>
 * <TD> nutationInObliquity</TD> 
 * <TD> Angle </TD>
 * <TD>  &nbsp; </TD>
 * <TD>&nbsp; the nutation in obliquity (the part perpendicular to the ecliptic) in \f$ radian \f$. </TD>
 * </TR>
	
 * <TR>
 * <TD> nutationInObliquityRate</TD> 
 * <TD> AngularRate </TD>
 * <TD>  &nbsp; </TD>
 * <TD>&nbsp; the rate of nutation in obliquity in \f$ radian \ s^{-1} \f$. </TD>
 * </TR>
	

 * </TABLE>
 */
class DelayModelVariableParametersTable : public Representable {
	friend class ASDM;

public:


	/**
	 * Return the list of field names that make up key key
	 * as an array of strings.
	 * @return a vector of string.
	 */	
	static const std::vector<std::string>& getKeyName();


	virtual ~DelayModelVariableParametersTable();
	
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
	 * to the schema defined for DelayModelVariableParameters (DelayModelVariableParametersTable.xsd).
	 *
	 * @returns a string containing the XML representation.
	 * @throws ConversionException
	 */
	std::string toXML()  ;

#ifndef WITHOUT_ACS
	// Conversion Methods
	/**
	 * Convert this table into a DelayModelVariableParametersTableIDL CORBA structure.
	 *
	 * @return a pointer to a DelayModelVariableParametersTableIDL
	 */
	asdmIDL::DelayModelVariableParametersTableIDL *toIDL() ;
	
	/**
	 * Fills the CORBA data structure passed in parameter
	 * with the content of this table.
	 *
	 * @param x a reference to the asdmIDL::DelayModelVariableParametersTableIDL to be populated
	 * with the content of this.
	 */
	 void toIDL(asdmIDL::DelayModelVariableParametersTableIDL& x) const;
	 
#endif

#ifndef WITHOUT_ACS
	/**
	 * Populate this table from the content of a DelayModelVariableParametersTableIDL Corba structure.
	 *
	 * @throws DuplicateKey Thrown if the method tries to add a row having a key that is already in the table.
	 * @throws ConversionException
	 */	
	void fromIDL(asdmIDL::DelayModelVariableParametersTableIDL x) ;
#endif
	
	//
	// ====> Row creation.
	//
	
	/**
	 * Create a new row with default values.
	 * @return a pointer on a DelayModelVariableParametersRow
	 */
	DelayModelVariableParametersRow *newRow();
	
	
	/**
	 * Create a new row initialized to the specified values.
	 * @return a pointer on the created and initialized row.
	
 	 * @param time
	
 	 * @param ut1_utc
	
 	 * @param iat_utc
	
 	 * @param timeType
	
 	 * @param gstAtUt0
	
 	 * @param earthRotationRate
	
 	 * @param polarOffsets
	
 	 * @param polarOffsetsType
	
 	 * @param delayModelFixedParametersId
	
     */
	DelayModelVariableParametersRow *newRow(ArrayTime time, double ut1_utc, double iat_utc, DifferenceTypeMod::DifferenceType timeType, Angle gstAtUt0, AngularRate earthRotationRate, std::vector<double > polarOffsets, DifferenceTypeMod::DifferenceType polarOffsetsType, Tag delayModelFixedParametersId);
	


	/**
	 * Create a new row using a copy constructor mechanism.
	 * 
	 * The method creates a new DelayModelVariableParametersRow owned by this. Each attribute of the created row 
	 * is a (deep) copy of the corresponding attribute of row. The method does not add 
	 * the created row to this, its simply parents it to this, a call to the add method
	 * has to be done in order to get the row added (very likely after having modified
	 * some of its attributes).
	 * If row is null then the method returns a new DelayModelVariableParametersRow with default values for its attributes. 
	 *
	 * @param row the row which is to be copied.
	 */
	 DelayModelVariableParametersRow *newRow(DelayModelVariableParametersRow *row); 

	//
	// ====> Append a row to its table.
	//

	
	
	
	/** 
	 * Add a row.
	 * If there table contains a row whose key's fields are equal
	 * to x's ones then return a pointer on this row (i.e. no actual insertion is performed) 
	 * otherwise add x to the table and return x.
	 * @param x . A pointer on the row to be added.
 	 * @returns a pointer to a DelayModelVariableParametersRow.	 
	 */	 
	 
 	 DelayModelVariableParametersRow* add(DelayModelVariableParametersRow* x) ;



	//
	// ====> Methods returning rows.
	//
		
	/**
	 * Get a collection of pointers on the rows of the table.
	 * @return Alls rows in a vector of pointers of DelayModelVariableParametersRow. The elements of this vector are stored in the order 
	 * in which they have been added to the DelayModelVariableParametersTable.
	 */
	std::vector<DelayModelVariableParametersRow *> get() ;
	
	/**
	 * Get a const reference on the collection of rows pointers internally hold by the table.
	 * @return A const reference of a vector of pointers of DelayModelVariableParametersRow. The elements of this vector are stored in the order 
	 * in which they have been added to the DelayModelVariableParametersTable.
	 *
	 */
	 const std::vector<DelayModelVariableParametersRow *>& get() const ;
	


 
	
	/**
 	 * Returns a DelayModelVariableParametersRow* given a key.
 	 * @return a pointer to the row having the key whose values are passed as parameters, or 0 if
 	 * no row exists for that key.
	
	 * @param delayModelVariableParametersId
	
 	 *
	 */
 	DelayModelVariableParametersRow* getRowByKey(Tag delayModelVariableParametersId);

 	 	



	/**
 	 * Look up the table for a row whose all attributes  except the autoincrementable one 
 	 * are equal to the corresponding parameters of the method.
 	 * @return a pointer on this row if any, null otherwise.
 	 *
			
 	 * @param time
 	 		
 	 * @param ut1_utc
 	 		
 	 * @param iat_utc
 	 		
 	 * @param timeType
 	 		
 	 * @param gstAtUt0
 	 		
 	 * @param earthRotationRate
 	 		
 	 * @param polarOffsets
 	 		
 	 * @param polarOffsetsType
 	 		
 	 * @param delayModelFixedParametersId
 	 		 
 	 */
	DelayModelVariableParametersRow* lookup(ArrayTime time, double ut1_utc, double iat_utc, DifferenceTypeMod::DifferenceType timeType, Angle gstAtUt0, AngularRate earthRotationRate, std::vector<double > polarOffsets, DifferenceTypeMod::DifferenceType polarOffsetsType, Tag delayModelFixedParametersId); 


	void setUnknownAttributeBinaryReader(const std::string& attributeName, BinaryAttributeReaderFunctor* barFctr);
	BinaryAttributeReaderFunctor* getUnknownAttributeBinaryReader(const std::string& attributeName) const;

private:

	/**
	 * Create a DelayModelVariableParametersTable.
	 * <p>
	 * This constructor is private because only the
	 * container can create tables.  All tables must know the container
	 * to which they belong.
	 * @param container The container to which this table belongs.
	 */ 
	DelayModelVariableParametersTable (ASDM & container);

	ASDM & container;
	
	bool archiveAsBin; // If true archive binary else archive XML
	bool fileAsBin ; // If true file binary else file XML	
	
	std::string version ; 
	
	Entity entity;
	

	
	

	// A map for the autoincrementation algorithm
	std::map<std::string,int>  noAutoIncIds;
	void autoIncrement(std::string key, DelayModelVariableParametersRow* x);


	/**
	 * If this table has an autoincrementable attribute then check if *x verifies the rule of uniqueness and throw exception if not.
	 * Check if *x verifies the key uniqueness rule and throw an exception if not.
	 * Append x to its table.
	 * @throws DuplicateKey
	 
	 * @throws UniquenessViolationException
	 
	 */
	DelayModelVariableParametersRow* checkAndAdd(DelayModelVariableParametersRow* x, bool skipCheckUniqueness=false) ;
	
	/**
	 * Brutally append an DelayModelVariableParametersRow x to the collection of rows already stored in this table. No uniqueness check is done !
	 *
	 * @param DelayModelVariableParametersRow* x a pointer onto the DelayModelVariableParametersRow to be appended.
	 */
	 void append(DelayModelVariableParametersRow* x) ;
	 
	/**
	 * Brutally append an DelayModelVariableParametersRow x to the collection of rows already stored in this table. No uniqueness check is done !
	 *
	 * @param DelayModelVariableParametersRow* x a pointer onto the DelayModelVariableParametersRow to be appended.
	 */
	 void addWithoutCheckingUnique(DelayModelVariableParametersRow* x) ;
	 
	 



// A data structure to store the pointers on the table's rows.

// In all cases we maintain a private vector of DelayModelVariableParametersRow s.
   std::vector<DelayModelVariableParametersRow * > privateRows;
   

			
	std::vector<DelayModelVariableParametersRow *> row;

	
	void error() ; //throw(ConversionException);

	
	/**
	 * Populate this table from the content of a XML document that is required to
	 * be conform to the XML schema defined for a DelayModelVariableParameters (DelayModelVariableParametersTable.xsd).
	 * @throws ConversionException
	 * 
	 */
	void fromXML(std::string& xmlDoc) ;
		
	std::map<std::string, BinaryAttributeReaderFunctor *> unknownAttributes2Functors;

	/**
	  * Private methods involved during the build of this table out of the content
	  * of file(s) containing an external representation of a DelayModelVariableParameters table.
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
	  * will be saved in a file "DelayModelVariableParameters.bin" or an XML representation (fileAsBin==false) will be saved in a file "DelayModelVariableParameters.xml".
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
	 * Reads and parses a file containing a representation of a DelayModelVariableParametersTable as those produced  by the toFile method.
	 * This table is populated with the result of the parsing.
	 * @param directory The name of the directory containing the file te be read and parsed.
	 * @throws ConversionException If any error occurs while reading the 
	 * files in the directory or parsing them.
	 *
	 */
	 void setFromFile(const std::string& directory);	
 
};

} // End namespace asdm

#endif /* DelayModelVariableParametersTable_CLASS */
