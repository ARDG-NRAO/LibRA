
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
 * File SpectralWindowTable.h
 */
 
#ifndef SpectralWindowTable_CLASS
#define SpectralWindowTable_CLASS

#include <string>
#include <vector>
#include <map>



	
#include <alma/ASDM/Frequency.h>
	

	
#include <alma/ASDM/Tag.h>
	




	

	
#include <alma/Enumerations/CBasebandName.h>
	

	
#include <alma/Enumerations/CNetSideband.h>
	

	

	

	

	
#include <alma/Enumerations/CSidebandProcessingMode.h>
	

	

	
#include <alma/Enumerations/CWindowFunction.h>
	

	

	

	

	

	

	
#include <alma/Enumerations/CCorrelationBit.h>
	

	

	

	

	

	

	
#include <alma/Enumerations/CFrequencyReferenceCode.h>
	

	

	

	

	

	

	

	

	
#include <alma/Enumerations/CSpectralResolutionType.h>
	



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
//class asdm::SpectralWindowRow;

class ASDM;
class SpectralWindowRow;
/**
 * The SpectralWindowTable class is an Alma table.
 * <BR>
 * 
 * \par Role
 * Spectral window description. The convention in ALMA is to describe the  frequency axis in the topocentric reference frame. If this is not  the case (for instance if active Doppler tracking is implemented) then  \texttt{measFreqRef} should be set accordingly.
 * <BR>
 
 * Generated from model's revision "-1", branch ""
 *
 * <TABLE BORDER="1">
 * <CAPTION> Attributes of SpectralWindow </CAPTION>
 * <TR BGCOLOR="#AAAAAA"> <TH> Name </TH> <TH> Type </TH> <TH> Expected shape  </TH> <TH> Comment </TH></TR>
 
 * <TR> <TH BGCOLOR="#CCCCCC" colspan="4" align="center"> Key </TD></TR>
	
 * <TR>
 		
 * <TD><I> spectralWindowId </I></TD>
 		 
 * <TD> Tag</TD>
 * <TD> &nbsp; </TD>
 * <TD> &nbsp;identifies a unique row in the table. </TD>
 * </TR>
	


 * <TR> <TH BGCOLOR="#CCCCCC"  colspan="4" valign="center"> Value <br> (Mandatory) </TH></TR>
	
 * <TR>
 * <TD> basebandName </TD> 
 * <TD> BasebandNameMod::BasebandName </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;identifies the baseband. </TD>
 * </TR>
	
 * <TR>
 * <TD> netSideband </TD> 
 * <TD> NetSidebandMod::NetSideband </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;identifies the net sideband. </TD>
 * </TR>
	
 * <TR>
 * <TD> numChan (numChan)</TD> 
 * <TD> int </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;the number of frequency channels. </TD>
 * </TR>
	
 * <TR>
 * <TD> refFreq </TD> 
 * <TD> Frequency </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;the reference frequency. </TD>
 * </TR>
	
 * <TR>
 * <TD> sidebandProcessingMode </TD> 
 * <TD> SidebandProcessingModeMod::SidebandProcessingMode </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;identifies the sideband processing mode. </TD>
 * </TR>
	
 * <TR>
 * <TD> totBandwidth </TD> 
 * <TD> Frequency </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;the total bandwidth. </TD>
 * </TR>
	
 * <TR>
 * <TD> windowFunction </TD> 
 * <TD> WindowFunctionMod::WindowFunction </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;identifies the window function. </TD>
 * </TR>
	


 * <TR> <TH BGCOLOR="#CCCCCC"  colspan="4" valign="center"> Value <br> (Optional) </TH></TR>
	
 * <TR>
 * <TD> numBin(numBin)</TD> 
 * <TD> int </TD>
 * <TD>  &nbsp; </TD>
 * <TD>&nbsp; the number of channels used in any post-FFT averaging. </TD>
 * </TR>
	
 * <TR>
 * <TD> chanFreqStart</TD> 
 * <TD> Frequency </TD>
 * <TD>  &nbsp; </TD>
 * <TD>&nbsp; the frequency of the first channel. </TD>
 * </TR>
	
 * <TR>
 * <TD> chanFreqStep</TD> 
 * <TD> Frequency </TD>
 * <TD>  &nbsp; </TD>
 * <TD>&nbsp; the increment between two successive frequencies. </TD>
 * </TR>
	
 * <TR>
 * <TD> chanFreqArray</TD> 
 * <TD> std::vector<Frequency > </TD>
 * <TD>  numChan  </TD>
 * <TD>&nbsp; the frequencies defined as an array (\texttt{numChan} values). </TD>
 * </TR>
	
 * <TR>
 * <TD> chanWidth</TD> 
 * <TD> Frequency </TD>
 * <TD>  &nbsp; </TD>
 * <TD>&nbsp; the nominal channel width. </TD>
 * </TR>
	
 * <TR>
 * <TD> chanWidthArray</TD> 
 * <TD> std::vector<Frequency > </TD>
 * <TD>  numChan  </TD>
 * <TD>&nbsp; Array of channel widths. </TD>
 * </TR>
	
 * <TR>
 * <TD> correlationBit</TD> 
 * <TD> CorrelationBitMod::CorrelationBit </TD>
 * <TD>  &nbsp; </TD>
 * <TD>&nbsp; identifies the number of bits used in the signal representation. </TD>
 * </TR>
	
 * <TR>
 * <TD> effectiveBw</TD> 
 * <TD> Frequency </TD>
 * <TD>  &nbsp; </TD>
 * <TD>&nbsp; the effective noise bandwidth. </TD>
 * </TR>
	
 * <TR>
 * <TD> effectiveBwArray</TD> 
 * <TD> std::vector<Frequency > </TD>
 * <TD>  numChan  </TD>
 * <TD>&nbsp; array of effective bandwidths. </TD>
 * </TR>
	
 * <TR>
 * <TD> freqGroup</TD> 
 * <TD> int </TD>
 * <TD>  &nbsp; </TD>
 * <TD>&nbsp; the frequency group number. </TD>
 * </TR>
	
 * <TR>
 * <TD> freqGroupName</TD> 
 * <TD> std::string </TD>
 * <TD>  &nbsp; </TD>
 * <TD>&nbsp; the frequency group name. </TD>
 * </TR>
	
 * <TR>
 * <TD> lineArray</TD> 
 * <TD> std::vector<bool > </TD>
 * <TD>  numChan  </TD>
 * <TD>&nbsp; indicates lines (true) versus baselines (false). </TD>
 * </TR>
	
 * <TR>
 * <TD> measFreqRef</TD> 
 * <TD> FrequencyReferenceCodeMod::FrequencyReferenceCode </TD>
 * <TD>  &nbsp; </TD>
 * <TD>&nbsp; the reference frame of the frequencies. </TD>
 * </TR>
	
 * <TR>
 * <TD> name</TD> 
 * <TD> std::string </TD>
 * <TD>  &nbsp; </TD>
 * <TD>&nbsp; a name for this spectral window. </TD>
 * </TR>
	
 * <TR>
 * <TD> oversampling</TD> 
 * <TD> bool </TD>
 * <TD>  &nbsp; </TD>
 * <TD>&nbsp; data are "oversampled" (true) or not (false). </TD>
 * </TR>
	
 * <TR>
 * <TD> quantization</TD> 
 * <TD> bool </TD>
 * <TD>  &nbsp; </TD>
 * <TD>&nbsp; a quantization correction has been applied (true) or not applied (false). </TD>
 * </TR>
	
 * <TR>
 * <TD> refChan</TD> 
 * <TD> double </TD>
 * <TD>  &nbsp; </TD>
 * <TD>&nbsp; the reference channel "number". </TD>
 * </TR>
	
 * <TR>
 * <TD> resolution</TD> 
 * <TD> Frequency </TD>
 * <TD>  &nbsp; </TD>
 * <TD>&nbsp; the effective spectral resolution of one channel (see note). </TD>
 * </TR>
	
 * <TR>
 * <TD> resolutionArray</TD> 
 * <TD> std::vector<Frequency > </TD>
 * <TD>  numChan  </TD>
 * <TD>&nbsp; the array of frequency resolution. </TD>
 * </TR>
	
 * <TR>
 * <TD> numAssocValues(numAssocValues)</TD> 
 * <TD> int </TD>
 * <TD>  &nbsp; </TD>
 * <TD>&nbsp; the number of associated values. </TD>
 * </TR>
	
 * <TR>
 * <TD> assocNature</TD> 
 * <TD> std::vector<SpectralResolutionTypeMod::SpectralResolutionType > </TD>
 * <TD>  numAssocValues  </TD>
 * <TD>&nbsp; the natures of the associations with the rows refered to by assocSpectralWindowId. </TD>
 * </TR>
	
 * <TR>
 * <TD> assocSpectralWindowId</TD> 
 * <TD> std::vector<Tag>  </TD>
 * <TD>  numAssocValues  </TD>
 * <TD>&nbsp; refers to a collection of associated rows in the table. </TD>
 * </TR>
	
 * <TR>
 * <TD> imageSpectralWindowId</TD> 
 * <TD> Tag </TD>
 * <TD>  &nbsp; </TD>
 * <TD>&nbsp; refers to a unique row in the table (image sideband description). </TD>
 * </TR>
	
 * <TR>
 * <TD> dopplerId</TD> 
 * <TD> int </TD>
 * <TD>  &nbsp; </TD>
 * <TD>&nbsp; refers to a collection of rows in DopplerTable. </TD>
 * </TR>
	

 * </TABLE>
 */
class SpectralWindowTable : public Representable {
	friend class ASDM;

public:


	/**
	 * Return the list of field names that make up key key
	 * as an array of strings.
	 * @return a vector of string.
	 */	
	static const std::vector<std::string>& getKeyName();


	virtual ~SpectralWindowTable();
	
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
	 * to the schema defined for SpectralWindow (SpectralWindowTable.xsd).
	 *
	 * @returns a string containing the XML representation.
	 * @throws ConversionException
	 */
	std::string toXML()  ;

#ifndef WITHOUT_ACS
	// Conversion Methods
	/**
	 * Convert this table into a SpectralWindowTableIDL CORBA structure.
	 *
	 * @return a pointer to a SpectralWindowTableIDL
	 */
	asdmIDL::SpectralWindowTableIDL *toIDL() ;
	
	/**
	 * Fills the CORBA data structure passed in parameter
	 * with the content of this table.
	 *
	 * @param x a reference to the asdmIDL::SpectralWindowTableIDL to be populated
	 * with the content of this.
	 */
	 void toIDL(asdmIDL::SpectralWindowTableIDL& x) const;
	 
#endif

#ifndef WITHOUT_ACS
	/**
	 * Populate this table from the content of a SpectralWindowTableIDL Corba structure.
	 *
	 * @throws DuplicateKey Thrown if the method tries to add a row having a key that is already in the table.
	 * @throws ConversionException
	 */	
	void fromIDL(asdmIDL::SpectralWindowTableIDL x) ;
#endif
	
	//
	// ====> Row creation.
	//
	
	/**
	 * Create a new row with default values.
	 * @return a pointer on a SpectralWindowRow
	 */
	SpectralWindowRow *newRow();
	
	
	/**
	 * Create a new row initialized to the specified values.
	 * @return a pointer on the created and initialized row.
	
 	 * @param basebandName
	
 	 * @param netSideband
	
 	 * @param numChan
	
 	 * @param refFreq
	
 	 * @param sidebandProcessingMode
	
 	 * @param totBandwidth
	
 	 * @param windowFunction
	
     */
	SpectralWindowRow *newRow(BasebandNameMod::BasebandName basebandName, NetSidebandMod::NetSideband netSideband, int numChan, Frequency refFreq, SidebandProcessingModeMod::SidebandProcessingMode sidebandProcessingMode, Frequency totBandwidth, WindowFunctionMod::WindowFunction windowFunction);
	


	/**
	 * Create a new row using a copy constructor mechanism.
	 * 
	 * The method creates a new SpectralWindowRow owned by this. Each attribute of the created row 
	 * is a (deep) copy of the corresponding attribute of row. The method does not add 
	 * the created row to this, its simply parents it to this, a call to the add method
	 * has to be done in order to get the row added (very likely after having modified
	 * some of its attributes).
	 * If row is null then the method returns a new SpectralWindowRow with default values for its attributes. 
	 *
	 * @param row the row which is to be copied.
	 */
	 SpectralWindowRow *newRow(SpectralWindowRow *row); 

	//
	// ====> Append a row to its table.
	//

	
	
	
	/** 
	 * Add a row.
	 * If there table contains a row whose key's fields are equal
	 * to x's ones then return a pointer on this row (i.e. no actual insertion is performed) 
	 * otherwise add x to the table and return x.
	 * @param x . A pointer on the row to be added.
 	 * @returns a pointer to a SpectralWindowRow.	 
	 */	 
	 
 	 SpectralWindowRow* add(SpectralWindowRow* x) ;



	//
	// ====> Methods returning rows.
	//
		
	/**
	 * Get a collection of pointers on the rows of the table.
	 * @return Alls rows in a vector of pointers of SpectralWindowRow. The elements of this vector are stored in the order 
	 * in which they have been added to the SpectralWindowTable.
	 */
	std::vector<SpectralWindowRow *> get() ;
	
	/**
	 * Get a const reference on the collection of rows pointers internally hold by the table.
	 * @return A const reference of a vector of pointers of SpectralWindowRow. The elements of this vector are stored in the order 
	 * in which they have been added to the SpectralWindowTable.
	 *
	 */
	 const std::vector<SpectralWindowRow *>& get() const ;
	


 
	
	/**
 	 * Returns a SpectralWindowRow* given a key.
 	 * @return a pointer to the row having the key whose values are passed as parameters, or 0 if
 	 * no row exists for that key.
	
	 * @param spectralWindowId
	
 	 *
	 */
 	SpectralWindowRow* getRowByKey(Tag spectralWindowId);

 	 	



	/**
 	 * Look up the table for a row whose all attributes  except the autoincrementable one 
 	 * are equal to the corresponding parameters of the method.
 	 * @return a pointer on this row if any, null otherwise.
 	 *
			
 	 * @param basebandName
 	 		
 	 * @param netSideband
 	 		
 	 * @param numChan
 	 		
 	 * @param refFreq
 	 		
 	 * @param sidebandProcessingMode
 	 		
 	 * @param totBandwidth
 	 		
 	 * @param windowFunction
 	 		 
 	 */
	SpectralWindowRow* lookup(BasebandNameMod::BasebandName basebandName, NetSidebandMod::NetSideband netSideband, int numChan, Frequency refFreq, SidebandProcessingModeMod::SidebandProcessingMode sidebandProcessingMode, Frequency totBandwidth, WindowFunctionMod::WindowFunction windowFunction); 


	void setUnknownAttributeBinaryReader(const std::string& attributeName, BinaryAttributeReaderFunctor* barFctr);
	BinaryAttributeReaderFunctor* getUnknownAttributeBinaryReader(const std::string& attributeName) const;

private:

	/**
	 * Create a SpectralWindowTable.
	 * <p>
	 * This constructor is private because only the
	 * container can create tables.  All tables must know the container
	 * to which they belong.
	 * @param container The container to which this table belongs.
	 */ 
	SpectralWindowTable (ASDM & container);

	ASDM & container;
	
	bool archiveAsBin; // If true archive binary else archive XML
	bool fileAsBin ; // If true file binary else file XML	
	
	std::string version ; 
	
	Entity entity;
	

	
	

	// A map for the autoincrementation algorithm
	std::map<std::string,int>  noAutoIncIds;
	void autoIncrement(std::string key, SpectralWindowRow* x);


	/**
	 * If this table has an autoincrementable attribute then check if *x verifies the rule of uniqueness and throw exception if not.
	 * Check if *x verifies the key uniqueness rule and throw an exception if not.
	 * Append x to its table.
	 * @throws DuplicateKey
	 
	 * @throws UniquenessViolationException
	 
	 */
	SpectralWindowRow* checkAndAdd(SpectralWindowRow* x, bool skipCheckUniqueness=false) ;
	
	/**
	 * Brutally append an SpectralWindowRow x to the collection of rows already stored in this table. No uniqueness check is done !
	 *
	 * @param SpectralWindowRow* x a pointer onto the SpectralWindowRow to be appended.
	 */
	 void append(SpectralWindowRow* x) ;
	 
	/**
	 * Brutally append an SpectralWindowRow x to the collection of rows already stored in this table. No uniqueness check is done !
	 *
	 * @param SpectralWindowRow* x a pointer onto the SpectralWindowRow to be appended.
	 */
	 void addWithoutCheckingUnique(SpectralWindowRow* x) ;
	 
	 



// A data structure to store the pointers on the table's rows.

// In all cases we maintain a private vector of SpectralWindowRow s.
   std::vector<SpectralWindowRow * > privateRows;
   

			
	std::vector<SpectralWindowRow *> row;

	
	void error() ; //throw(ConversionException);

	
	/**
	 * Populate this table from the content of a XML document that is required to
	 * be conform to the XML schema defined for a SpectralWindow (SpectralWindowTable.xsd).
	 * @throws ConversionException
	 * 
	 */
	void fromXML(std::string& xmlDoc) ;
		
	std::map<std::string, BinaryAttributeReaderFunctor *> unknownAttributes2Functors;

	/**
	  * Private methods involved during the build of this table out of the content
	  * of file(s) containing an external representation of a SpectralWindow table.
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
	  * will be saved in a file "SpectralWindow.bin" or an XML representation (fileAsBin==false) will be saved in a file "SpectralWindow.xml".
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
	 * Reads and parses a file containing a representation of a SpectralWindowTable as those produced  by the toFile method.
	 * This table is populated with the result of the parsing.
	 * @param directory The name of the directory containing the file te be read and parsed.
	 * @throws ConversionException If any error occurs while reading the 
	 * files in the directory or parsing them.
	 *
	 */
	 void setFromFile(const std::string& directory);	
 
};

} // End namespace asdm

#endif /* SpectralWindowTable_CLASS */
