
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
 * File FlagCmdTable.h
 */
 
#ifndef FlagCmdTable_CLASS
#define FlagCmdTable_CLASS

#include <string>
#include <vector>
#include <map>



	
#include <alma/ASDM/ArrayTimeInterval.h>
	




	

	

	

	

	

	

	



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
//class asdm::FlagCmdRow;

class ASDM;
class FlagCmdRow;
/**
 * The FlagCmdTable class is an Alma table.
 * <BR>
 * 
 * \par Role
 * Flag commands.
 * <BR>
 
 * Generated from model's revision "-1", branch ""
 *
 * <TABLE BORDER="1">
 * <CAPTION> Attributes of FlagCmd </CAPTION>
 * <TR BGCOLOR="#AAAAAA"> <TH> Name </TH> <TH> Type </TH> <TH> Expected shape  </TH> <TH> Comment </TH></TR>
 
 * <TR> <TH BGCOLOR="#CCCCCC" colspan="4" align="center"> Key </TD></TR>
	
 * <TR>
 		
 * <TD> timeInterval </TD>
 		 
 * <TD> ArrayTimeInterval</TD>
 * <TD> &nbsp; </TD>
 * <TD> &nbsp; </TD>
 * </TR>
	


 * <TR> <TH BGCOLOR="#CCCCCC"  colspan="4" valign="center"> Value <br> (Mandatory) </TH></TR>
	
 * <TR>
 * <TD> type </TD> 
 * <TD> std::string </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp; </TD>
 * </TR>
	
 * <TR>
 * <TD> reason </TD> 
 * <TD> std::string </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp; </TD>
 * </TR>
	
 * <TR>
 * <TD> level </TD> 
 * <TD> int </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp; </TD>
 * </TR>
	
 * <TR>
 * <TD> severity </TD> 
 * <TD> int </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp; </TD>
 * </TR>
	
 * <TR>
 * <TD> applied </TD> 
 * <TD> bool </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp; </TD>
 * </TR>
	
 * <TR>
 * <TD> command </TD> 
 * <TD> std::string </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp; </TD>
 * </TR>
	


 * </TABLE>
 */
class FlagCmdTable : public Representable {
	friend class ASDM;

public:


	/**
	 * Return the list of field names that make up key key
	 * as an array of strings.
	 * @return a vector of string.
	 */	
	static const std::vector<std::string>& getKeyName();


	virtual ~FlagCmdTable();
	
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
	 * to the schema defined for FlagCmd (FlagCmdTable.xsd).
	 *
	 * @returns a string containing the XML representation.
	 * @throws ConversionException
	 */
	std::string toXML()  ;

#ifndef WITHOUT_ACS
	// Conversion Methods
	/**
	 * Convert this table into a FlagCmdTableIDL CORBA structure.
	 *
	 * @return a pointer to a FlagCmdTableIDL
	 */
	asdmIDL::FlagCmdTableIDL *toIDL() ;
	
	/**
	 * Fills the CORBA data structure passed in parameter
	 * with the content of this table.
	 *
	 * @param x a reference to the asdmIDL::FlagCmdTableIDL to be populated
	 * with the content of this.
	 */
	 void toIDL(asdmIDL::FlagCmdTableIDL& x) const;
	 
#endif

#ifndef WITHOUT_ACS
	/**
	 * Populate this table from the content of a FlagCmdTableIDL Corba structure.
	 *
	 * @throws DuplicateKey Thrown if the method tries to add a row having a key that is already in the table.
	 * @throws ConversionException
	 */	
	void fromIDL(asdmIDL::FlagCmdTableIDL x) ;
#endif
	
	//
	// ====> Row creation.
	//
	
	/**
	 * Create a new row with default values.
	 * @return a pointer on a FlagCmdRow
	 */
	FlagCmdRow *newRow();
	
	
	/**
	 * Create a new row initialized to the specified values.
	 * @return a pointer on the created and initialized row.
	
 	 * @param timeInterval
	
 	 * @param type
	
 	 * @param reason
	
 	 * @param level
	
 	 * @param severity
	
 	 * @param applied
	
 	 * @param command
	
     */
	FlagCmdRow *newRow(ArrayTimeInterval timeInterval, std::string type, std::string reason, int level, int severity, bool applied, std::string command);
	


	/**
	 * Create a new row using a copy constructor mechanism.
	 * 
	 * The method creates a new FlagCmdRow owned by this. Each attribute of the created row 
	 * is a (deep) copy of the corresponding attribute of row. The method does not add 
	 * the created row to this, its simply parents it to this, a call to the add method
	 * has to be done in order to get the row added (very likely after having modified
	 * some of its attributes).
	 * If row is null then the method returns a new FlagCmdRow with default values for its attributes. 
	 *
	 * @param row the row which is to be copied.
	 */
	 FlagCmdRow *newRow(FlagCmdRow *row); 

	//
	// ====> Append a row to its table.
	//
 
	
	/**
	 * Add a row.
	 * @param x a pointer to the FlagCmdRow to be added.
	 *
	 * @return a pointer to a FlagCmdRow. If the table contains a FlagCmdRow whose attributes (key and mandatory values) are equal to x ones
	 * then returns a pointer on that FlagCmdRow, otherwise returns x.
	 *
	 * @throw DuplicateKey { thrown when the table contains a FlagCmdRow with a key equal to the x one but having
	 * and a value section different from x one }
	 *
	
	 * @note The row is inserted in the table in such a way that all the rows having the same value of
	 * (  ) are stored by ascending time.
	 * @see method getByContext.
	
	 */
	FlagCmdRow* add(FlagCmdRow* x) ; 

 



	//
	// ====> Methods returning rows.
	//
		
	/**
	 * Get a collection of pointers on the rows of the table.
	 * @return Alls rows in a vector of pointers of FlagCmdRow. The elements of this vector are stored in the order 
	 * in which they have been added to the FlagCmdTable.
	 */
	std::vector<FlagCmdRow *> get() ;
	
	/**
	 * Get a const reference on the collection of rows pointers internally hold by the table.
	 * @return A const reference of a vector of pointers of FlagCmdRow. The elements of this vector are stored in the order 
	 * in which they have been added to the FlagCmdTable.
	 *
	 */
	 const std::vector<FlagCmdRow *>& get() const ;
	

	/**
	 * Returns all the rows sorted by ascending startTime for a given context. 
	 * The context is defined by a value of (  ).
	 *
	 * @return a pointer on a vector<FlagCmdRow *>. A null returned value means that the table contains
	 * no FlagCmdRow for the given (  ).
	 *
	 * @throws IllegalAccessException when a call is done to this method when it's called while the dataset has been imported with the 
	 * option checkRowUniqueness set to false.
	 */
	 std::vector <FlagCmdRow*> *getByContext();
	 


 
	
	/**
 	 * Returns a FlagCmdRow* given a key.
 	 * @return a pointer to the row having the key whose values are passed as parameters, or 0 if
 	 * no row exists for that key.
	
	 * @param timeInterval
	
 	 *
	 */
 	FlagCmdRow* getRowByKey(ArrayTimeInterval timeInterval);

 	 	



	/**
 	 * Look up the table for a row whose all attributes 
 	 * are equal to the corresponding parameters of the method.
 	 * @return a pointer on this row if any, null otherwise.
 	 *
			
 	 * @param timeInterval
 	 		
 	 * @param type
 	 		
 	 * @param reason
 	 		
 	 * @param level
 	 		
 	 * @param severity
 	 		
 	 * @param applied
 	 		
 	 * @param command
 	 		 
 	 */
	FlagCmdRow* lookup(ArrayTimeInterval timeInterval, std::string type, std::string reason, int level, int severity, bool applied, std::string command); 


	void setUnknownAttributeBinaryReader(const std::string& attributeName, BinaryAttributeReaderFunctor* barFctr);
	BinaryAttributeReaderFunctor* getUnknownAttributeBinaryReader(const std::string& attributeName) const;

private:

	/**
	 * Create a FlagCmdTable.
	 * <p>
	 * This constructor is private because only the
	 * container can create tables.  All tables must know the container
	 * to which they belong.
	 * @param container The container to which this table belongs.
	 */ 
	FlagCmdTable (ASDM & container);

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
	FlagCmdRow* checkAndAdd(FlagCmdRow* x, bool skipCheckUniqueness=false) ;
	
	/**
	 * Brutally append an FlagCmdRow x to the collection of rows already stored in this table. No uniqueness check is done !
	 *
	 * @param FlagCmdRow* x a pointer onto the FlagCmdRow to be appended.
	 */
	 void append(FlagCmdRow* x) ;
	 
	/**
	 * Brutally append an FlagCmdRow x to the collection of rows already stored in this table. No uniqueness check is done !
	 *
	 * @param FlagCmdRow* x a pointer onto the FlagCmdRow to be appended.
	 */
	 void addWithoutCheckingUnique(FlagCmdRow* x) ;
	 
	 


	
	
	/**
	 * Insert a FlagCmdRow* in a vector of FlagCmdRow* so that it's ordered by ascending time.
	 *
	 * @param FlagCmdRow* x . The pointer to be inserted.
	 * @param vector <FlagCmdRow*>& row . A reference to the vector where to insert x.
	 *
	 */
	 FlagCmdRow * insertByStartTime(FlagCmdRow* x, std::vector<FlagCmdRow* >& row);
	  


// A data structure to store the pointers on the table's rows.

// In all cases we maintain a private vector of FlagCmdRow s.
   std::vector<FlagCmdRow * > privateRows;
   

	

	
	
		
		
	std::vector <FlagCmdRow *> row;
		 
		
	
	

	
	void error() ; //throw(ConversionException);

	
	/**
	 * Populate this table from the content of a XML document that is required to
	 * be conform to the XML schema defined for a FlagCmd (FlagCmdTable.xsd).
	 * @throws ConversionException
	 * 
	 */
	void fromXML(std::string& xmlDoc) ;
		
	std::map<std::string, BinaryAttributeReaderFunctor *> unknownAttributes2Functors;

	/**
	  * Private methods involved during the build of this table out of the content
	  * of file(s) containing an external representation of a FlagCmd table.
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
	  * will be saved in a file "FlagCmd.bin" or an XML representation (fileAsBin==false) will be saved in a file "FlagCmd.xml".
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
	 * Reads and parses a file containing a representation of a FlagCmdTable as those produced  by the toFile method.
	 * This table is populated with the result of the parsing.
	 * @param directory The name of the directory containing the file te be read and parsed.
	 * @throws ConversionException If any error occurs while reading the 
	 * files in the directory or parsing them.
	 *
	 */
	 void setFromFile(const std::string& directory);	
 
};

} // End namespace asdm

#endif /* FlagCmdTable_CLASS */
