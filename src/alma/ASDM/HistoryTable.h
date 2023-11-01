
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
 * File HistoryTable.h
 */
 
#ifndef HistoryTable_CLASS
#define HistoryTable_CLASS

#include <string>
#include <vector>
#include <map>



	
#include <alma/ASDM/ArrayTime.h>
	

	
#include <alma/ASDM/Tag.h>
	




	

	

	

	

	

	

	

	



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
//class asdm::HistoryRow;

class ASDM;
class HistoryRow;
/**
 * The HistoryTable class is an Alma table.
 * <BR>
 * 
 * \par Role
 * History information.
 * <BR>
 
 * Generated from model's revision "-1", branch ""
 *
 * <TABLE BORDER="1">
 * <CAPTION> Attributes of History </CAPTION>
 * <TR BGCOLOR="#AAAAAA"> <TH> Name </TH> <TH> Type </TH> <TH> Expected shape  </TH> <TH> Comment </TH></TR>
 
 * <TR> <TH BGCOLOR="#CCCCCC" colspan="4" align="center"> Key </TD></TR>
	
 * <TR>
 		
 * <TD> execBlockId </TD>
 		 
 * <TD> Tag</TD>
 * <TD> &nbsp; </TD>
 * <TD> &nbsp; </TD>
 * </TR>
	
 * <TR>
 		
 * <TD> time </TD>
 		 
 * <TD> ArrayTime</TD>
 * <TD> &nbsp; </TD>
 * <TD> &nbsp; </TD>
 * </TR>
	


 * <TR> <TH BGCOLOR="#CCCCCC"  colspan="4" valign="center"> Value <br> (Mandatory) </TH></TR>
	
 * <TR>
 * <TD> message </TD> 
 * <TD> std::string </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp; </TD>
 * </TR>
	
 * <TR>
 * <TD> priority </TD> 
 * <TD> std::string </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp; </TD>
 * </TR>
	
 * <TR>
 * <TD> origin </TD> 
 * <TD> std::string </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp; </TD>
 * </TR>
	
 * <TR>
 * <TD> objectId </TD> 
 * <TD> std::string </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp; </TD>
 * </TR>
	
 * <TR>
 * <TD> application </TD> 
 * <TD> std::string </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp; </TD>
 * </TR>
	
 * <TR>
 * <TD> cliCommand </TD> 
 * <TD> std::string </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp; </TD>
 * </TR>
	
 * <TR>
 * <TD> appParms </TD> 
 * <TD> std::string </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp; </TD>
 * </TR>
	


 * </TABLE>
 */
class HistoryTable : public Representable {
	friend class ASDM;

public:


	/**
	 * Return the list of field names that make up key key
	 * as an array of strings.
	 * @return a vector of string.
	 */	
	static const std::vector<std::string>& getKeyName();


	virtual ~HistoryTable();
	
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
	 * to the schema defined for History (HistoryTable.xsd).
	 *
	 * @returns a string containing the XML representation.
	 * @throws ConversionException
	 */
	std::string toXML()  ;

#ifndef WITHOUT_ACS
	// Conversion Methods
	/**
	 * Convert this table into a HistoryTableIDL CORBA structure.
	 *
	 * @return a pointer to a HistoryTableIDL
	 */
	asdmIDL::HistoryTableIDL *toIDL() ;
	
	/**
	 * Fills the CORBA data structure passed in parameter
	 * with the content of this table.
	 *
	 * @param x a reference to the asdmIDL::HistoryTableIDL to be populated
	 * with the content of this.
	 */
	 void toIDL(asdmIDL::HistoryTableIDL& x) const;
	 
#endif

#ifndef WITHOUT_ACS
	/**
	 * Populate this table from the content of a HistoryTableIDL Corba structure.
	 *
	 * @throws DuplicateKey Thrown if the method tries to add a row having a key that is already in the table.
	 * @throws ConversionException
	 */	
	void fromIDL(asdmIDL::HistoryTableIDL x) ;
#endif
	
	//
	// ====> Row creation.
	//
	
	/**
	 * Create a new row with default values.
	 * @return a pointer on a HistoryRow
	 */
	HistoryRow *newRow();
	
	
	/**
	 * Create a new row initialized to the specified values.
	 * @return a pointer on the created and initialized row.
	
 	 * @param execBlockId
	
 	 * @param time
	
 	 * @param message
	
 	 * @param priority
	
 	 * @param origin
	
 	 * @param objectId
	
 	 * @param application
	
 	 * @param cliCommand
	
 	 * @param appParms
	
     */
	HistoryRow *newRow(Tag execBlockId, ArrayTime time, std::string message, std::string priority, std::string origin, std::string objectId, std::string application, std::string cliCommand, std::string appParms);
	


	/**
	 * Create a new row using a copy constructor mechanism.
	 * 
	 * The method creates a new HistoryRow owned by this. Each attribute of the created row 
	 * is a (deep) copy of the corresponding attribute of row. The method does not add 
	 * the created row to this, its simply parents it to this, a call to the add method
	 * has to be done in order to get the row added (very likely after having modified
	 * some of its attributes).
	 * If row is null then the method returns a new HistoryRow with default values for its attributes. 
	 *
	 * @param row the row which is to be copied.
	 */
	 HistoryRow *newRow(HistoryRow *row); 

	//
	// ====> Append a row to its table.
	//
 
	
	/**
	 * Add a row.
	 * @param x a pointer to the HistoryRow to be added.
	 *
	 * @return a pointer to a HistoryRow. If the table contains a HistoryRow whose attributes (key and mandatory values) are equal to x ones
	 * then returns a pointer on that HistoryRow, otherwise returns x.
	 *
	 * @throw DuplicateKey { thrown when the table contains a HistoryRow with a key equal to the x one but having
	 * and a value section different from x one }
	 *
	
	 * @note The row is inserted in the table in such a way that all the rows having the same value of
	 * ( execBlockId ) are stored by ascending time.
	 * @see method getByContext.
	
	 */
	HistoryRow* add(HistoryRow* x) ; 

 



	//
	// ====> Methods returning rows.
	//
		
	/**
	 * Get a collection of pointers on the rows of the table.
	 * @return Alls rows in a vector of pointers of HistoryRow. The elements of this vector are stored in the order 
	 * in which they have been added to the HistoryTable.
	 */
	std::vector<HistoryRow *> get() ;
	
	/**
	 * Get a const reference on the collection of rows pointers internally hold by the table.
	 * @return A const reference of a vector of pointers of HistoryRow. The elements of this vector are stored in the order 
	 * in which they have been added to the HistoryTable.
	 *
	 */
	 const std::vector<HistoryRow *>& get() const ;
	

	/**
	 * Returns all the rows sorted by ascending startTime for a given context. 
	 * The context is defined by a value of ( execBlockId ).
	 *
	 * @return a pointer on a vector<HistoryRow *>. A null returned value means that the table contains
	 * no HistoryRow for the given ( execBlockId ).
	 *
	 * @throws IllegalAccessException when a call is done to this method when it's called while the dataset has been imported with the 
	 * option checkRowUniqueness set to false.
	 */
	 std::vector <HistoryRow*> *getByContext(Tag execBlockId);
	 


 
	
	/**
 	 * Returns a HistoryRow* given a key.
 	 * @return a pointer to the row having the key whose values are passed as parameters, or 0 if
 	 * no row exists for that key.
	
	 * @param execBlockId
	
	 * @param time
	
 	 *
	 */
 	HistoryRow* getRowByKey(Tag execBlockId, ArrayTime time);

 	 	



	/**
 	 * Look up the table for a row whose all attributes 
 	 * are equal to the corresponding parameters of the method.
 	 * @return a pointer on this row if any, null otherwise.
 	 *
			
 	 * @param execBlockId
 	 		
 	 * @param time
 	 		
 	 * @param message
 	 		
 	 * @param priority
 	 		
 	 * @param origin
 	 		
 	 * @param objectId
 	 		
 	 * @param application
 	 		
 	 * @param cliCommand
 	 		
 	 * @param appParms
 	 		 
 	 */
	HistoryRow* lookup(Tag execBlockId, ArrayTime time, std::string message, std::string priority, std::string origin, std::string objectId, std::string application, std::string cliCommand, std::string appParms); 


	void setUnknownAttributeBinaryReader(const std::string& attributeName, BinaryAttributeReaderFunctor* barFctr);
	BinaryAttributeReaderFunctor* getUnknownAttributeBinaryReader(const std::string& attributeName) const;

private:

	/**
	 * Create a HistoryTable.
	 * <p>
	 * This constructor is private because only the
	 * container can create tables.  All tables must know the container
	 * to which they belong.
	 * @param container The container to which this table belongs.
	 */ 
	HistoryTable (ASDM & container);

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
	HistoryRow* checkAndAdd(HistoryRow* x, bool skipCheckUniqueness=false) ;
	
	/**
	 * Brutally append an HistoryRow x to the collection of rows already stored in this table. No uniqueness check is done !
	 *
	 * @param HistoryRow* x a pointer onto the HistoryRow to be appended.
	 */
	 void append(HistoryRow* x) ;
	 
	/**
	 * Brutally append an HistoryRow x to the collection of rows already stored in this table. No uniqueness check is done !
	 *
	 * @param HistoryRow* x a pointer onto the HistoryRow to be appended.
	 */
	 void addWithoutCheckingUnique(HistoryRow* x) ;
	 
	 


	
   	
   /**
	 * Insert a HistoryRow* in a vector of HistoryRow* so that it's ordered by ascending time.
	 *
	 * @param HistoryRow* x . The pointer to be inserted.
	 * @param vector <HistoryRow*>& row . A reference to the vector where to insert x.
	 *
	 */
 	HistoryRow * insertByTime(HistoryRow* x, std::vector<HistoryRow *>&row );
 	 


// A data structure to store the pointers on the table's rows.

// In all cases we maintain a private vector of HistoryRow s.
   std::vector<HistoryRow * > privateRows;
   

	

	
	
		
				
	typedef std::vector <HistoryRow* > TIME_ROWS;
	std::map<std::string, TIME_ROWS > context;
		
	/** 
	 * Returns a string built by concatenating the ascii representation of the
	 * parameters values suffixed with a "_" character.
	 */
	 std::string Key(Tag execBlockId) ;
		 
		
	
	
	/**
	 * Fills the vector vout (passed by reference) with pointers on elements of vin 
	 * whose attributes are equal to the corresponding parameters of the method.
	 *
	 */
	void getByKeyNoAutoIncNoTime(std::vector <HistoryRow*>& vin, std::vector <HistoryRow*>& vout,  Tag execBlockId);
	

	
	void error() ; //throw(ConversionException);

	
	/**
	 * Populate this table from the content of a XML document that is required to
	 * be conform to the XML schema defined for a History (HistoryTable.xsd).
	 * @throws ConversionException
	 * 
	 */
	void fromXML(std::string& xmlDoc) ;
		
	std::map<std::string, BinaryAttributeReaderFunctor *> unknownAttributes2Functors;

	/**
	  * Private methods involved during the build of this table out of the content
	  * of file(s) containing an external representation of a History table.
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
	  * will be saved in a file "History.bin" or an XML representation (fileAsBin==false) will be saved in a file "History.xml".
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
	 * Reads and parses a file containing a representation of a HistoryTable as those produced  by the toFile method.
	 * This table is populated with the result of the parsing.
	 * @param directory The name of the directory containing the file te be read and parsed.
	 * @throws ConversionException If any error occurs while reading the 
	 * files in the directory or parsing them.
	 *
	 */
	 void setFromFile(const std::string& directory);	
 
};

} // End namespace asdm

#endif /* HistoryTable_CLASS */
