
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
 * File FeedTable.h
 */
 
#ifndef FeedTable_CLASS
#define FeedTable_CLASS

#include <string>
#include <vector>
#include <map>



	
#include <alma/ASDM/ArrayTimeInterval.h>
	

	
#include <alma/ASDM/Angle.h>
	

	
#include <alma/ASDM/Length.h>
	

	
#include <alma/ASDM/Tag.h>
	

	
#include <alma/ASDM/ComplexWrapper.h>
	




	

	

	

	

	

	
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
//class asdm::FeedRow;

class ASDM;
class FeedRow;
/**
 * The FeedTable class is an Alma table.
 * <BR>
 * 
 * \par Role
 * Contains characteristics of the feeds.
 * <BR>
 
 * Generated from model's revision "-1", branch ""
 *
 * <TABLE BORDER="1">
 * <CAPTION> Attributes of Feed </CAPTION>
 * <TR BGCOLOR="#AAAAAA"> <TH> Name </TH> <TH> Type </TH> <TH> Expected shape  </TH> <TH> Comment </TH></TR>
 
 * <TR> <TH BGCOLOR="#CCCCCC" colspan="4" align="center"> Key </TD></TR>
	
 * <TR>
 		
 * <TD> antennaId </TD>
 		 
 * <TD> Tag</TD>
 * <TD> &nbsp; </TD>
 * <TD> &nbsp;refers to a unique row in AntennaTable. </TD>
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
 * <TD> &nbsp;the time interval of validity of the content of the row. </TD>
 * </TR>
	
 * <TR>
 		
 * <TD><I> feedId </I></TD>
 		 
 * <TD> int</TD>
 * <TD> &nbsp; </TD>
 * <TD> &nbsp;identifies a collection of rows in the table. </TD>
 * </TR>
	


 * <TR> <TH BGCOLOR="#CCCCCC"  colspan="4" valign="center"> Value <br> (Mandatory) </TH></TR>
	
 * <TR>
 * <TD> numReceptor (numReceptor)</TD> 
 * <TD> int </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;the number of receptors. </TD>
 * </TR>
	
 * <TR>
 * <TD> beamOffset </TD> 
 * <TD> std::vector<std::vector<double > > </TD>
 * <TD>  numReceptor, 2 </TD> 
 * <TD> &nbsp;the offsets of the beam (one pair per receptor). </TD>
 * </TR>
	
 * <TR>
 * <TD> focusReference </TD> 
 * <TD> std::vector<std::vector<Length > > </TD>
 * <TD>  numReceptor, 3 </TD> 
 * <TD> &nbsp;the references for the focus position (one triple per receptor). </TD>
 * </TR>
	
 * <TR>
 * <TD> polarizationTypes </TD> 
 * <TD> std::vector<PolarizationTypeMod::PolarizationType > </TD>
 * <TD>  numReceptor </TD> 
 * <TD> &nbsp;identifies the polarization types (one value per receptor). </TD>
 * </TR>
	
 * <TR>
 * <TD> polResponse </TD> 
 * <TD> std::vector<std::vector<Complex > > </TD>
 * <TD>  numReceptor, numReceptor </TD> 
 * <TD> &nbsp;the polarization response (one value per pair of receptors). </TD>
 * </TR>
	
 * <TR>
 * <TD> receptorAngle </TD> 
 * <TD> std::vector<Angle > </TD>
 * <TD>  numReceptor </TD> 
 * <TD> &nbsp;the receptors angles (one value per receptor). </TD>
 * </TR>
	
 * <TR>
 * <TD> receiverId </TD> 
 * <TD> std::vector<int>  </TD>
 * <TD>  numReceptor </TD> 
 * <TD> &nbsp;refers to one or more collections of rows in ReceiverTable. </TD>
 * </TR>
	


 * <TR> <TH BGCOLOR="#CCCCCC"  colspan="4" valign="center"> Value <br> (Optional) </TH></TR>
	
 * <TR>
 * <TD> feedNum</TD> 
 * <TD> int </TD>
 * <TD>  &nbsp; </TD>
 * <TD>&nbsp; the feed number to be used for multi-feed receivers. </TD>
 * </TR>
	
 * <TR>
 * <TD> illumOffset</TD> 
 * <TD> std::vector<Length > </TD>
 * <TD>  2  </TD>
 * <TD>&nbsp; the illumination offset. </TD>
 * </TR>
	
 * <TR>
 * <TD> position</TD> 
 * <TD> std::vector<Length > </TD>
 * <TD>  3  </TD>
 * <TD>&nbsp; the position of the feed. </TD>
 * </TR>
	
 * <TR>
 * <TD> skyCoupling</TD> 
 * <TD> float </TD>
 * <TD>  &nbsp; </TD>
 * <TD>&nbsp; the sky coupling is the coupling efficiency to the sky of the WVR radiometer's. Note that in general one expects to see whether \b no sky coupling efficiency recorded or \b only \b one of the two forms  scalar (skyCoupling) or array (skyCouplingSpectrum, numChan). </TD>
 * </TR>
	
 * <TR>
 * <TD> numChan(numChan)</TD> 
 * <TD> int </TD>
 * <TD>  &nbsp; </TD>
 * <TD>&nbsp; the size of skyCouplingSpectrum. This attribute must be present when the (array) attribute skyCouplingSpectrum is present since it defines its number of elements. The value of this attribute must be equal to the value of numChan in the row of the SpectralWindow table refered to by spectralWindowId. </TD>
 * </TR>
	
 * <TR>
 * <TD> skyCouplingSpectrum</TD> 
 * <TD> std::vector<float > </TD>
 * <TD>  numChan  </TD>
 * <TD>&nbsp; the sky coupling is the coupling efficiency to the sky of the WVR radiometer's. This column differs from the skyCoupling column because it contains one value for each of the individual channels of that spectralWindow. See the documentation of numChan for the size and the presence of this attribute. Note that in general one expects to see whether \b no sky coupling efficiency recorded or \b only \b one of the two forms  scalar (skyCoupling) or array (skyCouplingSpectrum, numChan). </TD>
 * </TR>
	

 * </TABLE>
 */
class FeedTable : public Representable {
	friend class ASDM;

public:


	/**
	 * Return the list of field names that make up key key
	 * as an array of strings.
	 * @return a vector of string.
	 */	
	static const std::vector<std::string>& getKeyName();


	virtual ~FeedTable();
	
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
	 * to the schema defined for Feed (FeedTable.xsd).
	 *
	 * @returns a string containing the XML representation.
	 * @throws ConversionException
	 */
	std::string toXML()  ;

#ifndef WITHOUT_ACS
	// Conversion Methods
	/**
	 * Convert this table into a FeedTableIDL CORBA structure.
	 *
	 * @return a pointer to a FeedTableIDL
	 */
	asdmIDL::FeedTableIDL *toIDL() ;
	
	/**
	 * Fills the CORBA data structure passed in parameter
	 * with the content of this table.
	 *
	 * @param x a reference to the asdmIDL::FeedTableIDL to be populated
	 * with the content of this.
	 */
	 void toIDL(asdmIDL::FeedTableIDL& x) const;
	 
#endif

#ifndef WITHOUT_ACS
	/**
	 * Populate this table from the content of a FeedTableIDL Corba structure.
	 *
	 * @throws DuplicateKey Thrown if the method tries to add a row having a key that is already in the table.
	 * @throws ConversionException
	 */	
	void fromIDL(asdmIDL::FeedTableIDL x) ;
#endif
	
	//
	// ====> Row creation.
	//
	
	/**
	 * Create a new row with default values.
	 * @return a pointer on a FeedRow
	 */
	FeedRow *newRow();
	
	
	/**
	 * Create a new row initialized to the specified values.
	 * @return a pointer on the created and initialized row.
	
 	 * @param antennaId
	
 	 * @param spectralWindowId
	
 	 * @param timeInterval
	
 	 * @param numReceptor
	
 	 * @param beamOffset
	
 	 * @param focusReference
	
 	 * @param polarizationTypes
	
 	 * @param polResponse
	
 	 * @param receptorAngle
	
 	 * @param receiverId
	
     */
	FeedRow *newRow(Tag antennaId, Tag spectralWindowId, ArrayTimeInterval timeInterval, int numReceptor, std::vector<std::vector<double > > beamOffset, std::vector<std::vector<Length > > focusReference, std::vector<PolarizationTypeMod::PolarizationType > polarizationTypes, std::vector<std::vector<Complex > > polResponse, std::vector<Angle > receptorAngle, std::vector<int>  receiverId);
	


	/**
	 * Create a new row using a copy constructor mechanism.
	 * 
	 * The method creates a new FeedRow owned by this. Each attribute of the created row 
	 * is a (deep) copy of the corresponding attribute of row. The method does not add 
	 * the created row to this, its simply parents it to this, a call to the add method
	 * has to be done in order to get the row added (very likely after having modified
	 * some of its attributes).
	 * If row is null then the method returns a new FeedRow with default values for its attributes. 
	 *
	 * @param row the row which is to be copied.
	 */
	 FeedRow *newRow(FeedRow *row); 

	//
	// ====> Append a row to its table.
	//

	
	 
	
	
	/** 
	 * Add a row.
	 * If there table contains a row whose key's fields except² feedId are equal
	 * to x's ones then return a pointer on this row (i.e. no actual insertion is performed) 
	 * otherwise add x to the table and return x.
	 * @param x . A pointer on the row to be added.
 	 * @returns a pointer to a FeedRow.
	 */
	 
 	 FeedRow* add(FeedRow* x) ;



	//
	// ====> Methods returning rows.
	//
		
	/**
	 * Get a collection of pointers on the rows of the table.
	 * @return Alls rows in a vector of pointers of FeedRow. The elements of this vector are stored in the order 
	 * in which they have been added to the FeedTable.
	 */
	std::vector<FeedRow *> get() ;
	
	/**
	 * Get a const reference on the collection of rows pointers internally hold by the table.
	 * @return A const reference of a vector of pointers of FeedRow. The elements of this vector are stored in the order 
	 * in which they have been added to the FeedTable.
	 *
	 */
	 const std::vector<FeedRow *>& get() const ;
	


 
	
	/**
 	 * Returns a FeedRow* given a key.
 	 * @return a pointer to the row having the key whose values are passed as parameters, or 0 if
 	 * no row exists for that key.
	
	 * @param antennaId
	
	 * @param spectralWindowId
	
	 * @param timeInterval
	
	 * @param feedId
	
 	 *
	 */
 	FeedRow* getRowByKey(Tag antennaId, Tag spectralWindowId, ArrayTimeInterval timeInterval, int feedId);

 	 	
 	
	/**
 	 * Returns a vector of pointers on rows whose key element feedId 
	 * is equal to the parameter feedId.
	 * @return a vector of vector <FeedRow *>. A returned vector of size 0 means that no row has been found.
	 * @param feedId int contains the value of
	 * the autoincrementable attribute that is looked up in the table.
	 */
 	std::vector <FeedRow *>  getRowByFeedId(int);



	/**
 	 * Look up the table for a row whose all attributes  except the autoincrementable one 
 	 * are equal to the corresponding parameters of the method.
 	 * @return a pointer on this row if any, null otherwise.
 	 *
			
 	 * @param antennaId
 	 		
 	 * @param spectralWindowId
 	 		
 	 * @param timeInterval
 	 		
 	 * @param numReceptor
 	 		
 	 * @param beamOffset
 	 		
 	 * @param focusReference
 	 		
 	 * @param polarizationTypes
 	 		
 	 * @param polResponse
 	 		
 	 * @param receptorAngle
 	 		
 	 * @param receiverId
 	 		 
 	 */
	FeedRow* lookup(Tag antennaId, Tag spectralWindowId, ArrayTimeInterval timeInterval, int numReceptor, std::vector<std::vector<double > > beamOffset, std::vector<std::vector<Length > > focusReference, std::vector<PolarizationTypeMod::PolarizationType > polarizationTypes, std::vector<std::vector<Complex > > polResponse, std::vector<Angle > receptorAngle, std::vector<int>  receiverId); 


	void setUnknownAttributeBinaryReader(const std::string& attributeName, BinaryAttributeReaderFunctor* barFctr);
	BinaryAttributeReaderFunctor* getUnknownAttributeBinaryReader(const std::string& attributeName) const;

private:

	/**
	 * Create a FeedTable.
	 * <p>
	 * This constructor is private because only the
	 * container can create tables.  All tables must know the container
	 * to which they belong.
	 * @param container The container to which this table belongs.
	 */ 
	FeedTable (ASDM & container);

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
	 
	 * @throws UniquenessViolationException
	 
	 */
	FeedRow* checkAndAdd(FeedRow* x, bool skipCheckUniqueness=false) ;
	
	/**
	 * Brutally append an FeedRow x to the collection of rows already stored in this table. No uniqueness check is done !
	 *
	 * @param FeedRow* x a pointer onto the FeedRow to be appended.
	 */
	 void append(FeedRow* x) ;
	 
	/**
	 * Brutally append an FeedRow x to the collection of rows already stored in this table. No uniqueness check is done !
	 *
	 * @param FeedRow* x a pointer onto the FeedRow to be appended.
	 */
	 void addWithoutCheckingUnique(FeedRow* x) ;
	 
	 


	
	
	/**
	 * Insert a FeedRow* in a vector of FeedRow* so that it's ordered by ascending time.
	 *
	 * @param FeedRow* x . The pointer to be inserted.
	 * @param vector <FeedRow*>& row . A reference to the vector where to insert x.
	 *
	 */
	 FeedRow * insertByStartTime(FeedRow* x, std::vector<FeedRow* >& row);
	  


// A data structure to store the pointers on the table's rows.

// In all cases we maintain a private vector of FeedRow s.
   std::vector<FeedRow * > privateRows;
   

	

	
	
		
		
	typedef std::vector <std::vector <FeedRow* > > ID_TIME_ROWS;
	std::map<std::string, ID_TIME_ROWS > context;
	
	/** 
	 * Returns a string built by concatenating the ascii representation of the
	 * parameters values suffixed with a "_" character.
	 */
	 std::string Key(Tag antennaId, Tag spectralWindowId) ;
	 	
		
	
	
	/**
	 * Fills the vector vout (passed by reference) with pointers on elements of vin 
	 * whose attributes are equal to the corresponding parameters of the method.
	 *
	 */
	void getByKeyNoAutoIncNoTime(std::vector <FeedRow*>& vin, std::vector <FeedRow*>& vout,  Tag antennaId, Tag spectralWindowId);
	

	
	void error() ; //throw(ConversionException);

	
	/**
	 * Populate this table from the content of a XML document that is required to
	 * be conform to the XML schema defined for a Feed (FeedTable.xsd).
	 * @throws ConversionException
	 * 
	 */
	void fromXML(std::string& xmlDoc) ;
		
	std::map<std::string, BinaryAttributeReaderFunctor *> unknownAttributes2Functors;

	/**
	  * Private methods involved during the build of this table out of the content
	  * of file(s) containing an external representation of a Feed table.
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
	  * will be saved in a file "Feed.bin" or an XML representation (fileAsBin==false) will be saved in a file "Feed.xml".
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
	 * Reads and parses a file containing a representation of a FeedTable as those produced  by the toFile method.
	 * This table is populated with the result of the parsing.
	 * @param directory The name of the directory containing the file te be read and parsed.
	 * @throws ConversionException If any error occurs while reading the 
	 * files in the directory or parsing them.
	 *
	 */
	 void setFromFile(const std::string& directory);	
 
};

} // End namespace asdm

#endif /* FeedTable_CLASS */
