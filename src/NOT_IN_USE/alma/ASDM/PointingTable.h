
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
 * File PointingTable.h
 */
 
#ifndef PointingTable_CLASS
#define PointingTable_CLASS

#include <string>
#include <vector>
#include <map>



	
#include <alma/ASDM/ArrayTimeInterval.h>
	

	
#include <alma/ASDM/Angle.h>
	

	
#include <alma/ASDM/ArrayTime.h>
	

	
#include <alma/ASDM/Tag.h>
	




	

	

	

	

	

	

	

	

	

	

	

	

	
#include <alma/Enumerations/CDirectionReferenceCode.h>
	

	

	

	



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
//class asdm::PointingRow;

class ASDM;
class PointingRow;
/**
 * The PointingTable class is an Alma table.
 * <BR>
 * 
 * \par Role
 * Antenna pointing information.
 * <BR>
 
 * Generated from model's revision "-1", branch ""
 *
 * <TABLE BORDER="1">
 * <CAPTION> Attributes of Pointing </CAPTION>
 * <TR BGCOLOR="#AAAAAA"> <TH> Name </TH> <TH> Type </TH> <TH> Expected shape  </TH> <TH> Comment </TH></TR>
 
 * <TR> <TH BGCOLOR="#CCCCCC" colspan="4" align="center"> Key </TD></TR>
	
 * <TR>
 		
 * <TD> antennaId </TD>
 		 
 * <TD> Tag</TD>
 * <TD> &nbsp; </TD>
 * <TD> &nbsp;refers to a unique row in AntennaTable. </TD>
 * </TR>
	
 * <TR>
 		
 * <TD> timeInterval </TD>
 		 
 * <TD> ArrayTimeInterval</TD>
 * <TD> &nbsp; </TD>
 * <TD> &nbsp;the time interval of validity of the row's content. </TD>
 * </TR>
	


 * <TR> <TH BGCOLOR="#CCCCCC"  colspan="4" valign="center"> Value <br> (Mandatory) </TH></TR>
	
 * <TR>
 * <TD> numSample (numSample)</TD> 
 * <TD> int </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;the number of time samples. </TD>
 * </TR>
	
 * <TR>
 * <TD> encoder </TD> 
 * <TD> std::vector<std::vector<Angle > > </TD>
 * <TD>  numSample, 2 </TD> 
 * <TD> &nbsp;Encoder values </TD>
 * </TR>
	
 * <TR>
 * <TD> pointingTracking </TD> 
 * <TD> bool </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;the antenna was in tracking mode (true) or not (false). </TD>
 * </TR>
	
 * <TR>
 * <TD> usePolynomials </TD> 
 * <TD> bool </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;use polynomials expansions (true) or not (false). </TD>
 * </TR>
	
 * <TR>
 * <TD> timeOrigin </TD> 
 * <TD> ArrayTime </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;the value used as origin in the polynomials expansions. </TD>
 * </TR>
	
 * <TR>
 * <TD> numTerm (numTerm)</TD> 
 * <TD> int </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;the number of terms of the polynomials. </TD>
 * </TR>
	
 * <TR>
 * <TD> pointingDirection </TD> 
 * <TD> std::vector<std::vector<Angle > > </TD>
 * <TD>  numTerm, 2 </TD> 
 * <TD> &nbsp;the commanded pointing direction. </TD>
 * </TR>
	
 * <TR>
 * <TD> target </TD> 
 * <TD> std::vector<std::vector<Angle > > </TD>
 * <TD>  numTerm, 2 </TD> 
 * <TD> &nbsp;the direction of the target. </TD>
 * </TR>
	
 * <TR>
 * <TD> offset </TD> 
 * <TD> std::vector<std::vector<Angle > > </TD>
 * <TD>  numTerm, 2 </TD> 
 * <TD> &nbsp;Horizon mapping offsets </TD>
 * </TR>
	
 * <TR>
 * <TD> pointingModelId </TD> 
 * <TD> int </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;refers to a collection of rows in PointingModelTable. </TD>
 * </TR>
	


 * <TR> <TH BGCOLOR="#CCCCCC"  colspan="4" valign="center"> Value <br> (Optional) </TH></TR>
	
 * <TR>
 * <TD> overTheTop</TD> 
 * <TD> bool </TD>
 * <TD>  &nbsp; </TD>
 * <TD>&nbsp; pointing ar elevations larger than 90 degrees (true) or lower (false). </TD>
 * </TR>
	
 * <TR>
 * <TD> sourceOffset</TD> 
 * <TD> std::vector<std::vector<Angle > > </TD>
 * <TD>  numTerm, 2  </TD>
 * <TD>&nbsp; sources offsets (one pair per term of the polynomial). </TD>
 * </TR>
	
 * <TR>
 * <TD> sourceOffsetReferenceCode</TD> 
 * <TD> DirectionReferenceCodeMod::DirectionReferenceCode </TD>
 * <TD>  &nbsp; </TD>
 * <TD>&nbsp; the  direction reference code associated to the source offset. </TD>
 * </TR>
	
 * <TR>
 * <TD> sourceOffsetEquinox</TD> 
 * <TD> ArrayTime </TD>
 * <TD>  &nbsp; </TD>
 * <TD>&nbsp; the equinox information (if needed by sourceReferenceCode). </TD>
 * </TR>
	
 * <TR>
 * <TD> sampledTimeInterval</TD> 
 * <TD> std::vector<ArrayTimeInterval > </TD>
 * <TD>  numSample  </TD>
 * <TD>&nbsp; an array of ArrayTimeInterval which must be given explicitly as soon as the data are irregularily sampled.  </TD>
 * </TR>
	
 * <TR>
 * <TD> atmosphericCorrection</TD> 
 * <TD> std::vector<std::vector<Angle > > </TD>
 * <TD>  numTerm, 2  </TD>
 * <TD>&nbsp; This is the correction applied to the commanded position to take into account refraction and any other atmospheric effects. This term will always be zero if there is no atmosphere. For ALMA this is the atmospheric refraction correction and will result in a correction in just the elevation axis. </TD>
 * </TR>
	

 * </TABLE>
 */
class PointingTable : public Representable {
	friend class ASDM;

public:


	/**
	 * Return the list of field names that make up key key
	 * as an array of strings.
	 * @return a vector of string.
	 */	
	static const std::vector<std::string>& getKeyName();


	virtual ~PointingTable();
	
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
	 * to the schema defined for Pointing (PointingTable.xsd).
	 *
	 * @returns a string containing the XML representation.
	 * @throws ConversionException
	 */
	std::string toXML()  ;

#ifndef WITHOUT_ACS
	// Conversion Methods
	/**
	 * Convert this table into a PointingTableIDL CORBA structure.
	 *
	 * @return a pointer to a PointingTableIDL
	 */
	asdmIDL::PointingTableIDL *toIDL() ;
	
	/**
	 * Fills the CORBA data structure passed in parameter
	 * with the content of this table.
	 *
	 * @param x a reference to the asdmIDL::PointingTableIDL to be populated
	 * with the content of this.
	 */
	 void toIDL(asdmIDL::PointingTableIDL& x) const;
	 
#endif

#ifndef WITHOUT_ACS
	/**
	 * Populate this table from the content of a PointingTableIDL Corba structure.
	 *
	 * @throws DuplicateKey Thrown if the method tries to add a row having a key that is already in the table.
	 * @throws ConversionException
	 */	
	void fromIDL(asdmIDL::PointingTableIDL x) ;
#endif
	
	//
	// ====> Row creation.
	//
	
	/**
	 * Create a new row with default values.
	 * @return a pointer on a PointingRow
	 */
	PointingRow *newRow();
	
	
	/**
	 * Create a new row initialized to the specified values.
	 * @return a pointer on the created and initialized row.
	
 	 * @param antennaId
	
 	 * @param timeInterval
	
 	 * @param numSample
	
 	 * @param encoder
	
 	 * @param pointingTracking
	
 	 * @param usePolynomials
	
 	 * @param timeOrigin
	
 	 * @param numTerm
	
 	 * @param pointingDirection
	
 	 * @param target
	
 	 * @param offset
	
 	 * @param pointingModelId
	
     */
	PointingRow *newRow(Tag antennaId, ArrayTimeInterval timeInterval, int numSample, std::vector<std::vector<Angle > > encoder, bool pointingTracking, bool usePolynomials, ArrayTime timeOrigin, int numTerm, std::vector<std::vector<Angle > > pointingDirection, std::vector<std::vector<Angle > > target, std::vector<std::vector<Angle > > offset, int pointingModelId);
	


	/**
	 * Create a new row using a copy constructor mechanism.
	 * 
	 * The method creates a new PointingRow owned by this. Each attribute of the created row 
	 * is a (deep) copy of the corresponding attribute of row. The method does not add 
	 * the created row to this, its simply parents it to this, a call to the add method
	 * has to be done in order to get the row added (very likely after having modified
	 * some of its attributes).
	 * If row is null then the method returns a new PointingRow with default values for its attributes. 
	 *
	 * @param row the row which is to be copied.
	 */
	 PointingRow *newRow(PointingRow *row); 

	//
	// ====> Append a row to its table.
	//
 
	
	/**
	 * Add a row.
	 * @param x a pointer to the PointingRow to be added.
	 *
	 * @return a pointer to a PointingRow. If the table contains a PointingRow whose attributes (key and mandatory values) are equal to x ones
	 * then returns a pointer on that PointingRow, otherwise returns x.
	 *
	 * @throw DuplicateKey { thrown when the table contains a PointingRow with a key equal to the x one but having
	 * and a value section different from x one }
	 *
	
	 * @note The row is inserted in the table in such a way that all the rows having the same value of
	 * ( antennaId ) are stored by ascending time.
	 * @see method getByContext.
	
	 */
	PointingRow* add(PointingRow* x) ; 

 



	//
	// ====> Methods returning rows.
	//
		
	/**
	 * Get a collection of pointers on the rows of the table.
	 * @return Alls rows in a vector of pointers of PointingRow. The elements of this vector are stored in the order 
	 * in which they have been added to the PointingTable.
	 */
	std::vector<PointingRow *> get() ;
	
	/**
	 * Get a const reference on the collection of rows pointers internally hold by the table.
	 * @return A const reference of a vector of pointers of PointingRow. The elements of this vector are stored in the order 
	 * in which they have been added to the PointingTable.
	 *
	 */
	 const std::vector<PointingRow *>& get() const ;
	

	/**
	 * Returns all the rows sorted by ascending startTime for a given context. 
	 * The context is defined by a value of ( antennaId ).
	 *
	 * @return a pointer on a vector<PointingRow *>. A null returned value means that the table contains
	 * no PointingRow for the given ( antennaId ).
	 *
	 * @throws IllegalAccessException when a call is done to this method when it's called while the dataset has been imported with the 
	 * option checkRowUniqueness set to false.
	 */
	 std::vector <PointingRow*> *getByContext(Tag antennaId);
	 


 
	
	/**
 	 * Returns a PointingRow* given a key.
 	 * @return a pointer to the row having the key whose values are passed as parameters, or 0 if
 	 * no row exists for that key.
	
	 * @param antennaId
	
	 * @param timeInterval
	
 	 *
	 */
 	PointingRow* getRowByKey(Tag antennaId, ArrayTimeInterval timeInterval);

 	 	



	/**
 	 * Look up the table for a row whose all attributes 
 	 * are equal to the corresponding parameters of the method.
 	 * @return a pointer on this row if any, null otherwise.
 	 *
			
 	 * @param antennaId
 	 		
 	 * @param timeInterval
 	 		
 	 * @param numSample
 	 		
 	 * @param encoder
 	 		
 	 * @param pointingTracking
 	 		
 	 * @param usePolynomials
 	 		
 	 * @param timeOrigin
 	 		
 	 * @param numTerm
 	 		
 	 * @param pointingDirection
 	 		
 	 * @param target
 	 		
 	 * @param offset
 	 		
 	 * @param pointingModelId
 	 		 
 	 */
	PointingRow* lookup(Tag antennaId, ArrayTimeInterval timeInterval, int numSample, std::vector<std::vector<Angle > > encoder, bool pointingTracking, bool usePolynomials, ArrayTime timeOrigin, int numTerm, std::vector<std::vector<Angle > > pointingDirection, std::vector<std::vector<Angle > > target, std::vector<std::vector<Angle > > offset, int pointingModelId); 


	void setUnknownAttributeBinaryReader(const std::string& attributeName, BinaryAttributeReaderFunctor* barFctr);
	BinaryAttributeReaderFunctor* getUnknownAttributeBinaryReader(const std::string& attributeName) const;

private:

	/**
	 * Create a PointingTable.
	 * <p>
	 * This constructor is private because only the
	 * container can create tables.  All tables must know the container
	 * to which they belong.
	 * @param container The container to which this table belongs.
	 */ 
	PointingTable (ASDM & container);

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
	PointingRow* checkAndAdd(PointingRow* x, bool skipCheckUniqueness=false) ;
	
	/**
	 * Brutally append an PointingRow x to the collection of rows already stored in this table. No uniqueness check is done !
	 *
	 * @param PointingRow* x a pointer onto the PointingRow to be appended.
	 */
	 void append(PointingRow* x) ;
	 
	/**
	 * Brutally append an PointingRow x to the collection of rows already stored in this table. No uniqueness check is done !
	 *
	 * @param PointingRow* x a pointer onto the PointingRow to be appended.
	 */
	 void addWithoutCheckingUnique(PointingRow* x) ;
	 
	 


	
	
	/**
	 * Insert a PointingRow* in a vector of PointingRow* so that it's ordered by ascending time.
	 *
	 * @param PointingRow* x . The pointer to be inserted.
	 * @param vector <PointingRow*>& row . A reference to the vector where to insert x.
	 *
	 */
	 PointingRow * insertByStartTime(PointingRow* x, std::vector<PointingRow* >& row);
	  


// A data structure to store the pointers on the table's rows.

// In all cases we maintain a private vector of PointingRow s.
   std::vector<PointingRow * > privateRows;
   

	

	
	
		
				
	typedef std::vector <PointingRow* > TIME_ROWS;
	std::map<std::string, TIME_ROWS > context;
		
	/** 
	 * Returns a string built by concatenating the ascii representation of the
	 * parameters values suffixed with a "_" character.
	 */
	 std::string Key(Tag antennaId) ;
		 
		
	
	
	/**
	 * Fills the vector vout (passed by reference) with pointers on elements of vin 
	 * whose attributes are equal to the corresponding parameters of the method.
	 *
	 */
	void getByKeyNoAutoIncNoTime(std::vector <PointingRow*>& vin, std::vector <PointingRow*>& vout,  Tag antennaId);
	

	
	void error() ; //throw(ConversionException);

	
	/**
	 * Populate this table from the content of a XML document that is required to
	 * be conform to the XML schema defined for a Pointing (PointingTable.xsd).
	 * @throws ConversionException
	 * 
	 */
	void fromXML(std::string& xmlDoc) ;
		
	std::map<std::string, BinaryAttributeReaderFunctor *> unknownAttributes2Functors;

	/**
	  * Private methods involved during the build of this table out of the content
	  * of file(s) containing an external representation of a Pointing table.
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
	  * will be saved in a file "Pointing.bin" or an XML representation (fileAsBin==false) will be saved in a file "Pointing.xml".
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
	 * Reads and parses a file containing a representation of a PointingTable as those produced  by the toFile method.
	 * This table is populated with the result of the parsing.
	 * @param directory The name of the directory containing the file te be read and parsed.
	 * @throws ConversionException If any error occurs while reading the 
	 * files in the directory or parsing them.
	 *
	 */
	 void setFromFile(const std::string& directory);	
 
};

} // End namespace asdm

#endif /* PointingTable_CLASS */
