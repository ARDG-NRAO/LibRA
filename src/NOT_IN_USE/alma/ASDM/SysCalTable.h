
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
 * File SysCalTable.h
 */
 
#ifndef SysCalTable_CLASS
#define SysCalTable_CLASS

#include <string>
#include <vector>
#include <map>



	
#include <alma/ASDM/ArrayTimeInterval.h>
	

	
#include <alma/ASDM/Temperature.h>
	

	
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
//class asdm::SysCalRow;

class ASDM;
class SysCalRow;
/**
 * The SysCalTable class is an Alma table.
 * <BR>
 * 
 * \par Role
 * System calibration. Gives information on the conversion  of data to temperature scale. This table is reduced to follow  the contents of the Measurement Set SysCal table. Use only spectral  values (use a single channel spectral window for single numbers).   \texttt{numChan} can be found in the SpectralWindow Table.  The contents of this table are used to scale the data in the filler.
 * <BR>
 
 * Generated from model's revision "-1", branch ""
 *
 * <TABLE BORDER="1">
 * <CAPTION> Attributes of SysCal </CAPTION>
 * <TR BGCOLOR="#AAAAAA"> <TH> Name </TH> <TH> Type </TH> <TH> Expected shape  </TH> <TH> Comment </TH></TR>
 
 * <TR> <TH BGCOLOR="#CCCCCC" colspan="4" align="center"> Key </TD></TR>
	
 * <TR>
 		
 * <TD> antennaId </TD>
 		 
 * <TD> Tag</TD>
 * <TD> &nbsp; </TD>
 * <TD> &nbsp;refers to a unique row  in AntennaTable. </TD>
 * </TR>
	
 * <TR>
 		
 * <TD> spectralWindowId </TD>
 		 
 * <TD> Tag</TD>
 * <TD> &nbsp; </TD>
 * <TD> &nbsp;refers to a unique row in SpectralWindowTable. </TD>
 * </TR>
	
 * <TR>
 		
 * <TD> timeInterval </TD>
 		 
 * <TD> ArrayTimeInterval</TD>
 * <TD> &nbsp; </TD>
 * <TD> &nbsp;time interval for which the row's content is valid. </TD>
 * </TR>
	
 * <TR>
 		
 * <TD> feedId </TD>
 		 
 * <TD> int</TD>
 * <TD> &nbsp; </TD>
 * <TD> &nbsp;refers to a collection of rows in FeedTable. </TD>
 * </TR>
	


 * <TR> <TH BGCOLOR="#CCCCCC"  colspan="4" valign="center"> Value <br> (Mandatory) </TH></TR>
	
 * <TR>
 * <TD> numReceptor (numReceptor)</TD> 
 * <TD> int </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;the number of receptors. </TD>
 * </TR>
	
 * <TR>
 * <TD> numChan (numChan)</TD> 
 * <TD> int </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;the number of frequency channels. </TD>
 * </TR>
	


 * <TR> <TH BGCOLOR="#CCCCCC"  colspan="4" valign="center"> Value <br> (Optional) </TH></TR>
	
 * <TR>
 * <TD> tcalFlag</TD> 
 * <TD> bool </TD>
 * <TD>  &nbsp; </TD>
 * <TD>&nbsp; the calibration temperature flag. </TD>
 * </TR>
	
 * <TR>
 * <TD> tcalSpectrum</TD> 
 * <TD> std::vector<std::vector<Temperature > > </TD>
 * <TD>  numReceptor, numChan  </TD>
 * <TD>&nbsp; the calibration temperatures (one value per receptor per channel). </TD>
 * </TR>
	
 * <TR>
 * <TD> trxFlag</TD> 
 * <TD> bool </TD>
 * <TD>  &nbsp; </TD>
 * <TD>&nbsp; the receiver temperature flag. </TD>
 * </TR>
	
 * <TR>
 * <TD> trxSpectrum</TD> 
 * <TD> std::vector<std::vector<Temperature > > </TD>
 * <TD>  numReceptor, numChan  </TD>
 * <TD>&nbsp; the receiver temperatures (one value per receptor per channel). </TD>
 * </TR>
	
 * <TR>
 * <TD> tskyFlag</TD> 
 * <TD> bool </TD>
 * <TD>  &nbsp; </TD>
 * <TD>&nbsp; the sky temperature flag. </TD>
 * </TR>
	
 * <TR>
 * <TD> tskySpectrum</TD> 
 * <TD> std::vector<std::vector<Temperature > > </TD>
 * <TD>  numReceptor, numChan  </TD>
 * <TD>&nbsp; the sky temperatures (one value per receptor per channel). </TD>
 * </TR>
	
 * <TR>
 * <TD> tsysFlag</TD> 
 * <TD> bool </TD>
 * <TD>  &nbsp; </TD>
 * <TD>&nbsp; the system temperature flag. </TD>
 * </TR>
	
 * <TR>
 * <TD> tsysSpectrum</TD> 
 * <TD> std::vector<std::vector<Temperature > > </TD>
 * <TD>  numReceptor, numChan  </TD>
 * <TD>&nbsp; the system temperatures (one value per receptor per channel). </TD>
 * </TR>
	
 * <TR>
 * <TD> tantFlag</TD> 
 * <TD> bool </TD>
 * <TD>  &nbsp; </TD>
 * <TD>&nbsp; the tant flag. </TD>
 * </TR>
	
 * <TR>
 * <TD> tantSpectrum</TD> 
 * <TD> std::vector<std::vector<float > > </TD>
 * <TD>  numReceptor, numChan  </TD>
 * <TD>&nbsp; the Tant spectrum (one value per receptor per channel). </TD>
 * </TR>
	
 * <TR>
 * <TD> tantTsysFlag</TD> 
 * <TD> bool </TD>
 * <TD>  &nbsp; </TD>
 * <TD>&nbsp; the Tant/Tsys flag. </TD>
 * </TR>
	
 * <TR>
 * <TD> tantTsysSpectrum</TD> 
 * <TD> std::vector<std::vector<float > > </TD>
 * <TD>  numReceptor, numChan  </TD>
 * <TD>&nbsp; the Tant/Tsys spectrum(one value per receptor per channel) . </TD>
 * </TR>
	
 * <TR>
 * <TD> phaseDiffFlag</TD> 
 * <TD> bool </TD>
 * <TD>  &nbsp; </TD>
 * <TD>&nbsp; the phase difference flag. </TD>
 * </TR>
	
 * <TR>
 * <TD> phaseDiffSpectrum</TD> 
 * <TD> std::vector<std::vector<float > > </TD>
 * <TD>  numReceptor, numChan  </TD>
 * <TD>&nbsp; the phase difference spectrum (one value per receptor per channel). </TD>
 * </TR>
	

 * </TABLE>
 */
class SysCalTable : public Representable {
	friend class ASDM;

public:


	/**
	 * Return the list of field names that make up key key
	 * as an array of strings.
	 * @return a vector of string.
	 */	
	static const std::vector<std::string>& getKeyName();


	virtual ~SysCalTable();
	
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
	 * to the schema defined for SysCal (SysCalTable.xsd).
	 *
	 * @returns a string containing the XML representation.
	 * @throws ConversionException
	 */
	std::string toXML()  ;

#ifndef WITHOUT_ACS
	// Conversion Methods
	/**
	 * Convert this table into a SysCalTableIDL CORBA structure.
	 *
	 * @return a pointer to a SysCalTableIDL
	 */
	asdmIDL::SysCalTableIDL *toIDL() ;
	
	/**
	 * Fills the CORBA data structure passed in parameter
	 * with the content of this table.
	 *
	 * @param x a reference to the asdmIDL::SysCalTableIDL to be populated
	 * with the content of this.
	 */
	 void toIDL(asdmIDL::SysCalTableIDL& x) const;
	 
#endif

#ifndef WITHOUT_ACS
	/**
	 * Populate this table from the content of a SysCalTableIDL Corba structure.
	 *
	 * @throws DuplicateKey Thrown if the method tries to add a row having a key that is already in the table.
	 * @throws ConversionException
	 */	
	void fromIDL(asdmIDL::SysCalTableIDL x) ;
#endif
	
	//
	// ====> Row creation.
	//
	
	/**
	 * Create a new row with default values.
	 * @return a pointer on a SysCalRow
	 */
	SysCalRow *newRow();
	
	
	/**
	 * Create a new row initialized to the specified values.
	 * @return a pointer on the created and initialized row.
	
 	 * @param antennaId
	
 	 * @param spectralWindowId
	
 	 * @param timeInterval
	
 	 * @param feedId
	
 	 * @param numReceptor
	
 	 * @param numChan
	
     */
	SysCalRow *newRow(Tag antennaId, Tag spectralWindowId, ArrayTimeInterval timeInterval, int feedId, int numReceptor, int numChan);
	


	/**
	 * Create a new row using a copy constructor mechanism.
	 * 
	 * The method creates a new SysCalRow owned by this. Each attribute of the created row 
	 * is a (deep) copy of the corresponding attribute of row. The method does not add 
	 * the created row to this, its simply parents it to this, a call to the add method
	 * has to be done in order to get the row added (very likely after having modified
	 * some of its attributes).
	 * If row is null then the method returns a new SysCalRow with default values for its attributes. 
	 *
	 * @param row the row which is to be copied.
	 */
	 SysCalRow *newRow(SysCalRow *row); 

	//
	// ====> Append a row to its table.
	//
 
	
	/**
	 * Add a row.
	 * @param x a pointer to the SysCalRow to be added.
	 *
	 * @return a pointer to a SysCalRow. If the table contains a SysCalRow whose attributes (key and mandatory values) are equal to x ones
	 * then returns a pointer on that SysCalRow, otherwise returns x.
	 *
	 * @throw DuplicateKey { thrown when the table contains a SysCalRow with a key equal to the x one but having
	 * and a value section different from x one }
	 *
	
	 * @note The row is inserted in the table in such a way that all the rows having the same value of
	 * ( antennaId, spectralWindowId, feedId ) are stored by ascending time.
	 * @see method getByContext.
	
	 */
	SysCalRow* add(SysCalRow* x) ; 

 



	//
	// ====> Methods returning rows.
	//
		
	/**
	 * Get a collection of pointers on the rows of the table.
	 * @return Alls rows in a vector of pointers of SysCalRow. The elements of this vector are stored in the order 
	 * in which they have been added to the SysCalTable.
	 */
	std::vector<SysCalRow *> get() ;
	
	/**
	 * Get a const reference on the collection of rows pointers internally hold by the table.
	 * @return A const reference of a vector of pointers of SysCalRow. The elements of this vector are stored in the order 
	 * in which they have been added to the SysCalTable.
	 *
	 */
	 const std::vector<SysCalRow *>& get() const ;
	

	/**
	 * Returns all the rows sorted by ascending startTime for a given context. 
	 * The context is defined by a value of ( antennaId, spectralWindowId, feedId ).
	 *
	 * @return a pointer on a vector<SysCalRow *>. A null returned value means that the table contains
	 * no SysCalRow for the given ( antennaId, spectralWindowId, feedId ).
	 *
	 * @throws IllegalAccessException when a call is done to this method when it's called while the dataset has been imported with the 
	 * option checkRowUniqueness set to false.
	 */
	 std::vector <SysCalRow*> *getByContext(Tag antennaId, Tag spectralWindowId, int feedId);
	 


 
	
	/**
 	 * Returns a SysCalRow* given a key.
 	 * @return a pointer to the row having the key whose values are passed as parameters, or 0 if
 	 * no row exists for that key.
	
	 * @param antennaId
	
	 * @param spectralWindowId
	
	 * @param timeInterval
	
	 * @param feedId
	
 	 *
	 */
 	SysCalRow* getRowByKey(Tag antennaId, Tag spectralWindowId, ArrayTimeInterval timeInterval, int feedId);

 	 	



	/**
 	 * Look up the table for a row whose all attributes 
 	 * are equal to the corresponding parameters of the method.
 	 * @return a pointer on this row if any, null otherwise.
 	 *
			
 	 * @param antennaId
 	 		
 	 * @param spectralWindowId
 	 		
 	 * @param timeInterval
 	 		
 	 * @param feedId
 	 		
 	 * @param numReceptor
 	 		
 	 * @param numChan
 	 		 
 	 */
	SysCalRow* lookup(Tag antennaId, Tag spectralWindowId, ArrayTimeInterval timeInterval, int feedId, int numReceptor, int numChan); 


	void setUnknownAttributeBinaryReader(const std::string& attributeName, BinaryAttributeReaderFunctor* barFctr);
	BinaryAttributeReaderFunctor* getUnknownAttributeBinaryReader(const std::string& attributeName) const;

private:

	/**
	 * Create a SysCalTable.
	 * <p>
	 * This constructor is private because only the
	 * container can create tables.  All tables must know the container
	 * to which they belong.
	 * @param container The container to which this table belongs.
	 */ 
	SysCalTable (ASDM & container);

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
	SysCalRow* checkAndAdd(SysCalRow* x, bool skipCheckUniqueness=false) ;
	
	/**
	 * Brutally append an SysCalRow x to the collection of rows already stored in this table. No uniqueness check is done !
	 *
	 * @param SysCalRow* x a pointer onto the SysCalRow to be appended.
	 */
	 void append(SysCalRow* x) ;
	 
	/**
	 * Brutally append an SysCalRow x to the collection of rows already stored in this table. No uniqueness check is done !
	 *
	 * @param SysCalRow* x a pointer onto the SysCalRow to be appended.
	 */
	 void addWithoutCheckingUnique(SysCalRow* x) ;
	 
	 


	
	
	/**
	 * Insert a SysCalRow* in a vector of SysCalRow* so that it's ordered by ascending time.
	 *
	 * @param SysCalRow* x . The pointer to be inserted.
	 * @param vector <SysCalRow*>& row . A reference to the vector where to insert x.
	 *
	 */
	 SysCalRow * insertByStartTime(SysCalRow* x, std::vector<SysCalRow* >& row);
	  


// A data structure to store the pointers on the table's rows.

// In all cases we maintain a private vector of SysCalRow s.
   std::vector<SysCalRow * > privateRows;
   

	

	
	
		
				
	typedef std::vector <SysCalRow* > TIME_ROWS;
	std::map<std::string, TIME_ROWS > context;
		
	/** 
	 * Returns a string built by concatenating the ascii representation of the
	 * parameters values suffixed with a "_" character.
	 */
	 std::string Key(Tag antennaId, Tag spectralWindowId, int feedId) ;
		 
		
	
	
	/**
	 * Fills the vector vout (passed by reference) with pointers on elements of vin 
	 * whose attributes are equal to the corresponding parameters of the method.
	 *
	 */
	void getByKeyNoAutoIncNoTime(std::vector <SysCalRow*>& vin, std::vector <SysCalRow*>& vout,  Tag antennaId, Tag spectralWindowId, int feedId);
	

	
	void error() ; //throw(ConversionException);

	
	/**
	 * Populate this table from the content of a XML document that is required to
	 * be conform to the XML schema defined for a SysCal (SysCalTable.xsd).
	 * @throws ConversionException
	 * 
	 */
	void fromXML(std::string& xmlDoc) ;
		
	std::map<std::string, BinaryAttributeReaderFunctor *> unknownAttributes2Functors;

	/**
	  * Private methods involved during the build of this table out of the content
	  * of file(s) containing an external representation of a SysCal table.
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
	  * will be saved in a file "SysCal.bin" or an XML representation (fileAsBin==false) will be saved in a file "SysCal.xml".
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
	 * Reads and parses a file containing a representation of a SysCalTable as those produced  by the toFile method.
	 * This table is populated with the result of the parsing.
	 * @param directory The name of the directory containing the file te be read and parsed.
	 * @throws ConversionException If any error occurs while reading the 
	 * files in the directory or parsing them.
	 *
	 */
	 void setFromFile(const std::string& directory);	
 
};

} // End namespace asdm

#endif /* SysCalTable_CLASS */
