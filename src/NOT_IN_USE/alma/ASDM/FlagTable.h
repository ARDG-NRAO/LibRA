
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
 * File FlagTable.h
 */
 
#ifndef FlagTable_CLASS
#define FlagTable_CLASS

#include <string>
#include <vector>
#include <map>



	
#include <alma/ASDM/ArrayTime.h>
	

	
#include <alma/ASDM/Tag.h>
	




	

	

	

	

	

	

	

	

	

	
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
//class asdm::FlagRow;

class ASDM;
class FlagRow;
/**
 * The FlagTable class is an Alma table.
 * <BR>
 * 
 * \par Role
 * This table is used for flagging visibility data and is used in addition to the Binary Data Format flags produced by the correlator software.
 * <BR>
 
 * Generated from model's revision "-1", branch ""
 *
 * <TABLE BORDER="1">
 * <CAPTION> Attributes of Flag </CAPTION>
 * <TR BGCOLOR="#AAAAAA"> <TH> Name </TH> <TH> Type </TH> <TH> Expected shape  </TH> <TH> Comment </TH></TR>
 
 * <TR> <TH BGCOLOR="#CCCCCC" colspan="4" align="center"> Key </TD></TR>
	
 * <TR>
 		
 * <TD><I> flagId </I></TD>
 		 
 * <TD> Tag</TD>
 * <TD> &nbsp; </TD>
 * <TD> &nbsp;identifies a unique row in the table. </TD>
 * </TR>
	


 * <TR> <TH BGCOLOR="#CCCCCC"  colspan="4" valign="center"> Value <br> (Mandatory) </TH></TR>
	
 * <TR>
 * <TD> startTime </TD> 
 * <TD> ArrayTime </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;the start time of the flagging period. </TD>
 * </TR>
	
 * <TR>
 * <TD> endTime </TD> 
 * <TD> ArrayTime </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;the end time of the flagging period. </TD>
 * </TR>
	
 * <TR>
 * <TD> reason </TD> 
 * <TD> std::string </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;Extensible list of flagging conditions. </TD>
 * </TR>
	
 * <TR>
 * <TD> numAntenna (numAntenna)</TD> 
 * <TD> int </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;The number of antennas to which the flagging refers.By convention numAntenna== 0 means that the flag applies to all the existing antennas, in such a case the array antennaId can be left empty. </TD>
 * </TR>
	
 * <TR>
 * <TD> antennaId </TD> 
 * <TD> std::vector<Tag>  </TD>
 * <TD>  numAntenna </TD> 
 * <TD> &nbsp;An array of Tag which refers to a collection of rows in the Antenna table. The flag applies to the antennas described in these rows. It is an error to have different elements with a same value in this array. </TD>
 * </TR>
	


 * <TR> <TH BGCOLOR="#CCCCCC"  colspan="4" valign="center"> Value <br> (Optional) </TH></TR>
	
 * <TR>
 * <TD> numPolarizationType(numPolarizationType)</TD> 
 * <TD> int </TD>
 * <TD>  &nbsp; </TD>
 * <TD>&nbsp; The number of polarization types , i.e. the size of the attribute polarizationType. By convention numPolarizationType == 0 means that the flag applies to all the defined polarization types. \b Remark : numPolarizationType and polarizationType, both optional, must be both present or both absent in one same row of the table, except if numPolarizationType==0 in which case all the defined polarization types are involved in the flagging. </TD>
 * </TR>
	
 * <TR>
 * <TD> numSpectralWindow(numSpectralWindow)</TD> 
 * <TD> int </TD>
 * <TD>  &nbsp; </TD>
 * <TD>&nbsp; The number of spectral windows targeted by the flagging. By convention numSpectralWindow == 0 means that the flag applies to all the existing spectral windows. \b Remark : numSpectralWindow and spectralWindow, both optional, must be both present or both absent in one same row of the table, except if numSpectralWindow==0, in which case all the declared spectral windows are involved in the flagging. </TD>
 * </TR>
	
 * <TR>
 * <TD> numPairedAntenna(numPairedAntenna)</TD> 
 * <TD> int </TD>
 * <TD>  &nbsp; </TD>
 * <TD>&nbsp; The number of antennas to be paired with to form the flagged baselines. By convention, numPairedAntenna == 0 means that the flag applies to all baselines built on the antennas declared in the attribute antennaId. \b Remark: numPairedAntenna and pairedAntenna, both optional, must be both present or both absent except if numPairedAntenna==0 in which case one has to consider all the baselines defined upon the antennas announced in  antennaId. </TD>
 * </TR>
	
 * <TR>
 * <TD> numChan(numChan)</TD> 
 * <TD> int </TD>
 * <TD>  &nbsp; </TD>
 * <TD>&nbsp; Number of channels to be flaggged. </TD>
 * </TR>
	
 * <TR>
 * <TD> polarizationType</TD> 
 * <TD> std::vector<PolarizationTypeMod::PolarizationType > </TD>
 * <TD>  numPolarizationType  </TD>
 * <TD>&nbsp; An array of values of type PolarizationType. It specifies the polarization types where the flagging applies. It is an error to have different elements with a same value in this array.  </TD>
 * </TR>
	
 * <TR>
 * <TD> channel</TD> 
 * <TD> std::vector<std::vector<int > > </TD>
 * <TD>  numChan, 3  </TD>
 * <TD>&nbsp; An array of triplets where the first element is the number spectralWindowId. The second and third values are the startChannel and endChannel, 
respectively, which specify
the channels where flagging applies. It is
an error to have different elements with a
same value in this array.
 </TD>
 * </TR>
	
 * <TR>
 * <TD> pairedAntennaId</TD> 
 * <TD> std::vector<Tag>  </TD>
 * <TD>  numPairedAntenna  </TD>
 * <TD>&nbsp; An array of Tag which refers to rows in the Antenna table. These rows contain the description of the antennas which are paired to form the flagged baselines. It is an error to have distinct elements with a same value in this array. </TD>
 * </TR>
	
 * <TR>
 * <TD> spectralWindowId</TD> 
 * <TD> std::vector<Tag>  </TD>
 * <TD>  numSpectralWindow  </TD>
 * <TD>&nbsp; An array of Tag  which refers to a collection of rows in the SpectralWindow table. The flag applies to the spectral windows described in these rows. It is an error to have different elements with a same value in this array.   </TD>
 * </TR>
	

 * </TABLE>
 */
class FlagTable : public Representable {
	friend class ASDM;

public:


	/**
	 * Return the list of field names that make up key key
	 * as an array of strings.
	 * @return a vector of string.
	 */	
	static const std::vector<std::string>& getKeyName();


	virtual ~FlagTable();
	
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
	 * to the schema defined for Flag (FlagTable.xsd).
	 *
	 * @returns a string containing the XML representation.
	 * @throws ConversionException
	 */
	std::string toXML()  ;

#ifndef WITHOUT_ACS
	// Conversion Methods
	/**
	 * Convert this table into a FlagTableIDL CORBA structure.
	 *
	 * @return a pointer to a FlagTableIDL
	 */
	asdmIDL::FlagTableIDL *toIDL() ;
	
	/**
	 * Fills the CORBA data structure passed in parameter
	 * with the content of this table.
	 *
	 * @param x a reference to the asdmIDL::FlagTableIDL to be populated
	 * with the content of this.
	 */
	 void toIDL(asdmIDL::FlagTableIDL& x) const;
	 
#endif

#ifndef WITHOUT_ACS
	/**
	 * Populate this table from the content of a FlagTableIDL Corba structure.
	 *
	 * @throws DuplicateKey Thrown if the method tries to add a row having a key that is already in the table.
	 * @throws ConversionException
	 */	
	void fromIDL(asdmIDL::FlagTableIDL x) ;
#endif
	
	//
	// ====> Row creation.
	//
	
	/**
	 * Create a new row with default values.
	 * @return a pointer on a FlagRow
	 */
	FlagRow *newRow();
	
	
	/**
	 * Create a new row initialized to the specified values.
	 * @return a pointer on the created and initialized row.
	
 	 * @param startTime
	
 	 * @param endTime
	
 	 * @param reason
	
 	 * @param numAntenna
	
 	 * @param antennaId
	
     */
	FlagRow *newRow(ArrayTime startTime, ArrayTime endTime, std::string reason, int numAntenna, std::vector<Tag>  antennaId);
	


	/**
	 * Create a new row using a copy constructor mechanism.
	 * 
	 * The method creates a new FlagRow owned by this. Each attribute of the created row 
	 * is a (deep) copy of the corresponding attribute of row. The method does not add 
	 * the created row to this, its simply parents it to this, a call to the add method
	 * has to be done in order to get the row added (very likely after having modified
	 * some of its attributes).
	 * If row is null then the method returns a new FlagRow with default values for its attributes. 
	 *
	 * @param row the row which is to be copied.
	 */
	 FlagRow *newRow(FlagRow *row); 

	//
	// ====> Append a row to its table.
	//

	
	
	
	/** 
	 * Add a row.
	 * If there table contains a row whose key's fields are equal
	 * to x's ones then return a pointer on this row (i.e. no actual insertion is performed) 
	 * otherwise add x to the table and return x.
	 * @param x . A pointer on the row to be added.
 	 * @returns a pointer to a FlagRow.	 
	 */	 
	 
 	 FlagRow* add(FlagRow* x) ;



	//
	// ====> Methods returning rows.
	//
		
	/**
	 * Get a collection of pointers on the rows of the table.
	 * @return Alls rows in a vector of pointers of FlagRow. The elements of this vector are stored in the order 
	 * in which they have been added to the FlagTable.
	 */
	std::vector<FlagRow *> get() ;
	
	/**
	 * Get a const reference on the collection of rows pointers internally hold by the table.
	 * @return A const reference of a vector of pointers of FlagRow. The elements of this vector are stored in the order 
	 * in which they have been added to the FlagTable.
	 *
	 */
	 const std::vector<FlagRow *>& get() const ;
	


 
	
	/**
 	 * Returns a FlagRow* given a key.
 	 * @return a pointer to the row having the key whose values are passed as parameters, or 0 if
 	 * no row exists for that key.
	
	 * @param flagId
	
 	 *
	 */
 	FlagRow* getRowByKey(Tag flagId);

 	 	



	/**
 	 * Look up the table for a row whose all attributes  except the autoincrementable one 
 	 * are equal to the corresponding parameters of the method.
 	 * @return a pointer on this row if any, null otherwise.
 	 *
			
 	 * @param startTime
 	 		
 	 * @param endTime
 	 		
 	 * @param reason
 	 		
 	 * @param numAntenna
 	 		
 	 * @param antennaId
 	 		 
 	 */
	FlagRow* lookup(ArrayTime startTime, ArrayTime endTime, std::string reason, int numAntenna, std::vector<Tag>  antennaId); 


	void setUnknownAttributeBinaryReader(const std::string& attributeName, BinaryAttributeReaderFunctor* barFctr);
	BinaryAttributeReaderFunctor* getUnknownAttributeBinaryReader(const std::string& attributeName) const;

private:

	/**
	 * Create a FlagTable.
	 * <p>
	 * This constructor is private because only the
	 * container can create tables.  All tables must know the container
	 * to which they belong.
	 * @param container The container to which this table belongs.
	 */ 
	FlagTable (ASDM & container);

	ASDM & container;
	
	bool archiveAsBin; // If true archive binary else archive XML
	bool fileAsBin ; // If true file binary else file XML	
	
	std::string version ; 
	
	Entity entity;
	

	
	

	// A map for the autoincrementation algorithm
	std::map<std::string,int>  noAutoIncIds;
	void autoIncrement(std::string key, FlagRow* x);


	/**
	 * If this table has an autoincrementable attribute then check if *x verifies the rule of uniqueness and throw exception if not.
	 * Check if *x verifies the key uniqueness rule and throw an exception if not.
	 * Append x to its table.
	 * @throws DuplicateKey
	 
	 * @throws UniquenessViolationException
	 
	 */
	FlagRow* checkAndAdd(FlagRow* x, bool skipCheckUniqueness=false) ;
	
	/**
	 * Brutally append an FlagRow x to the collection of rows already stored in this table. No uniqueness check is done !
	 *
	 * @param FlagRow* x a pointer onto the FlagRow to be appended.
	 */
	 void append(FlagRow* x) ;
	 
	/**
	 * Brutally append an FlagRow x to the collection of rows already stored in this table. No uniqueness check is done !
	 *
	 * @param FlagRow* x a pointer onto the FlagRow to be appended.
	 */
	 void addWithoutCheckingUnique(FlagRow* x) ;
	 
	 



// A data structure to store the pointers on the table's rows.

// In all cases we maintain a private vector of FlagRow s.
   std::vector<FlagRow * > privateRows;
   

			
	std::vector<FlagRow *> row;

	
	void error() ; //throw(ConversionException);

	
	/**
	 * Populate this table from the content of a XML document that is required to
	 * be conform to the XML schema defined for a Flag (FlagTable.xsd).
	 * @throws ConversionException
	 * 
	 */
	void fromXML(std::string& xmlDoc) ;
		
	std::map<std::string, BinaryAttributeReaderFunctor *> unknownAttributes2Functors;

	/**
	  * Private methods involved during the build of this table out of the content
	  * of file(s) containing an external representation of a Flag table.
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
	  * will be saved in a file "Flag.bin" or an XML representation (fileAsBin==false) will be saved in a file "Flag.xml".
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
	 * Reads and parses a file containing a representation of a FlagTable as those produced  by the toFile method.
	 * This table is populated with the result of the parsing.
	 * @param directory The name of the directory containing the file te be read and parsed.
	 * @throws ConversionException If any error occurs while reading the 
	 * files in the directory or parsing them.
	 *
	 */
	 void setFromFile(const std::string& directory);	
 
};

} // End namespace asdm

#endif /* FlagTable_CLASS */
