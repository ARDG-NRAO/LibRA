
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
 * File CalAppPhaseTable.h
 */
 
#ifndef CalAppPhaseTable_CLASS
#define CalAppPhaseTable_CLASS

#include <string>
#include <vector>
#include <map>



	
#include <alma/ASDM/ArrayTime.h>
	

	
#include <alma/ASDM/Tag.h>
	




	
#include <alma/Enumerations/CBasebandName.h>
	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	



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
//class asdm::CalAppPhaseRow;

class ASDM;
class CalAppPhaseRow;
/**
 * The CalAppPhaseTable class is an Alma table.
 * <BR>
 * 
 * \par Role
 * The CalAppPhase table is relevant to the ALMA observatory when the antennas are being phased to form a coherent sum during the observation. For each scan, the table provides information about which antennas are included in the sum, their relative phase adjustments, the efficiency of the sum (relative to best performance) and the quality of each antenna participating in the system. This data is used in real-time to provide the phased sum signal, and after the observation to analyze the result.
 * <BR>
 
 * Generated from model's revision "-1", branch ""
 *
 * <TABLE BORDER="1">
 * <CAPTION> Attributes of CalAppPhase </CAPTION>
 * <TR BGCOLOR="#AAAAAA"> <TH> Name </TH> <TH> Type </TH> <TH> Expected shape  </TH> <TH> Comment </TH></TR>
 
 * <TR> <TH BGCOLOR="#CCCCCC" colspan="4" align="center"> Key </TD></TR>
	
 * <TR>
 		
 * <TD> basebandName </TD>
 		 
 * <TD> BasebandNameMod::BasebandName</TD>
 * <TD> &nbsp; </TD>
 * <TD> &nbsp;identifies the baseband. </TD>
 * </TR>
	
 * <TR>
 		
 * <TD> scanNumber </TD>
 		 
 * <TD> int</TD>
 * <TD> &nbsp; </TD>
 * <TD> &nbsp;The number of the scan processed by TELCAL. Along with an ExecBlock Id (which should be ExecBlock\_0 most of the time), the value of scanNumber can be used as the key to retrieve informations related to the scan (e.g. its start time).  </TD>
 * </TR>
	
 * <TR>
 		
 * <TD> calDataId </TD>
 		 
 * <TD> Tag</TD>
 * <TD> &nbsp; </TD>
 * <TD> &nbsp;identifies a unique row in the CalData table. </TD>
 * </TR>
	
 * <TR>
 		
 * <TD> calReductionId </TD>
 		 
 * <TD> Tag</TD>
 * <TD> &nbsp; </TD>
 * <TD> &nbsp;identifies a unique row in the CalReduction table. </TD>
 * </TR>
	


 * <TR> <TH BGCOLOR="#CCCCCC"  colspan="4" valign="center"> Value <br> (Mandatory) </TH></TR>
	
 * <TR>
 * <TD> startValidTime </TD> 
 * <TD> ArrayTime </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;start of phasing solution validity. </TD>
 * </TR>
	
 * <TR>
 * <TD> endValidTime </TD> 
 * <TD> ArrayTime </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;end of phasing solution validity. </TD>
 * </TR>
	
 * <TR>
 * <TD> adjustTime </TD> 
 * <TD> ArrayTime </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;The time of the last adjustment to the phasing analysis via the \c ParameterTuning  interface. </TD>
 * </TR>
	
 * <TR>
 * <TD> adjustToken </TD> 
 * <TD> std::string </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;A parameter supplied via the \c ParameterTuning interface to indicate the form of adjustment(s) made at adjustTime. Note that TELCAL merely passes this datum and adjustTime through to this table. </TD>
 * </TR>
	
 * <TR>
 * <TD> phasingMode </TD> 
 * <TD> std::string </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;The mode in which the phasing system is being operated. </TD>
 * </TR>
	
 * <TR>
 * <TD> numPhasedAntennas (numPhasedAntennas)</TD> 
 * <TD> int </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;the number of antennas in phased sum, \f$N_p\f$. </TD>
 * </TR>
	
 * <TR>
 * <TD> phasedAntennas </TD> 
 * <TD> std::vector<std::string > </TD>
 * <TD>  numPhasedAntennas </TD> 
 * <TD> &nbsp;the names of the phased antennas. </TD>
 * </TR>
	
 * <TR>
 * <TD> refAntennaIndex </TD> 
 * <TD> int </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;the index of the reference antenna in the array \c phasedAntennas . It must be an integer value in the interval \f$ [0, N_p-1]\f$. </TD>
 * </TR>
	
 * <TR>
 * <TD> candRefAntennaIndex </TD> 
 * <TD> int </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;tne index of a candidate (new) reference antenna in the array phasedAntennas; it must be a integer in the interval \f$[0, N_p-1]\f$. </TD>
 * </TR>
	
 * <TR>
 * <TD> phasePacking </TD> 
 * <TD> std::string </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;how to unpack \c phaseValues. </TD>
 * </TR>
	
 * <TR>
 * <TD> numReceptors (numReceptors)</TD> 
 * <TD> int </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;the number of receptors per antenna, \f$N_r\f$.The number (\f$N_r \le 2 \f$) of receptors per antenna, usually two (polarizations), but it might be one in special cases. </TD>
 * </TR>
	
 * <TR>
 * <TD> numChannels (numChannels)</TD> 
 * <TD> int </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;the number of data channels, \f$N_d\f$.  </TD>
 * </TR>
	
 * <TR>
 * <TD> numPhaseValues (numPhaseValues)</TD> 
 * <TD> int </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;The number  of phase data values present in the table, \f$N_v\f$. </TD>
 * </TR>
	
 * <TR>
 * <TD> phaseValues </TD> 
 * <TD> std::vector<float > </TD>
 * <TD>  numPhaseValues </TD> 
 * <TD> &nbsp;the array of phase data values. </TD>
 * </TR>
	
 * <TR>
 * <TD> numCompare (numCompare)</TD> 
 * <TD> int </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;the number of comparison antennas, \f$N_c\f$. </TD>
 * </TR>
	
 * <TR>
 * <TD> numEfficiencies (numEfficiencies)</TD> 
 * <TD> int </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;the number of efficiencies, \f$N_e\f$. </TD>
 * </TR>
	
 * <TR>
 * <TD> compareArray </TD> 
 * <TD> std::vector<std::string > </TD>
 * <TD>  numCompare </TD> 
 * <TD> &nbsp;the names of the comparison antennas. </TD>
 * </TR>
	
 * <TR>
 * <TD> efficiencyIndices </TD> 
 * <TD> std::vector<int > </TD>
 * <TD>  numEfficiencies </TD> 
 * <TD> &nbsp;indices of the antenna(s) in \c compareArray used to calculate \c efficiencies; they must be distinct integers in the interval \f$[0, N_c]\f$. </TD>
 * </TR>
	
 * <TR>
 * <TD> efficiencies </TD> 
 * <TD> std::vector<std::vector<float > > </TD>
 * <TD>  numEfficiencies, numChannels </TD> 
 * <TD> &nbsp;an array of efficiencies of phased sum. </TD>
 * </TR>
	
 * <TR>
 * <TD> quality </TD> 
 * <TD> std::vector<float > </TD>
 * <TD>  numPhasedAntennas+numCompare </TD> 
 * <TD> &nbsp;quality of phased antennas. </TD>
 * </TR>
	
 * <TR>
 * <TD> phasedSumAntenna </TD> 
 * <TD> std::string </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;the name of the phased sum antenna. </TD>
 * </TR>
	


 * <TR> <TH BGCOLOR="#CCCCCC"  colspan="4" valign="center"> Value <br> (Optional) </TH></TR>
	
 * <TR>
 * <TD> typeSupports</TD> 
 * <TD> std::string </TD>
 * <TD>  &nbsp; </TD>
 * <TD>&nbsp; encoding of supporting data values. </TD>
 * </TR>
	
 * <TR>
 * <TD> numSupports(numSupports)</TD> 
 * <TD> int </TD>
 * <TD>  &nbsp; </TD>
 * <TD>&nbsp; the number of supporting data values, \f$N_s\f$. </TD>
 * </TR>
	
 * <TR>
 * <TD> phaseSupports</TD> 
 * <TD> std::vector<float > </TD>
 * <TD>  numSupports  </TD>
 * <TD>&nbsp; an array of supporting data values. </TD>
 * </TR>
	

 * </TABLE>
 */
class CalAppPhaseTable : public Representable {
	friend class ASDM;

public:


	/**
	 * Return the list of field names that make up key key
	 * as an array of strings.
	 * @return a vector of string.
	 */	
	static const std::vector<std::string>& getKeyName();


	virtual ~CalAppPhaseTable();
	
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
	 * to the schema defined for CalAppPhase (CalAppPhaseTable.xsd).
	 *
	 * @returns a string containing the XML representation.
	 * @throws ConversionException
	 */
	std::string toXML()  ;

#ifndef WITHOUT_ACS
	// Conversion Methods
	/**
	 * Convert this table into a CalAppPhaseTableIDL CORBA structure.
	 *
	 * @return a pointer to a CalAppPhaseTableIDL
	 */
	asdmIDL::CalAppPhaseTableIDL *toIDL() ;
	
	/**
	 * Fills the CORBA data structure passed in parameter
	 * with the content of this table.
	 *
	 * @param x a reference to the asdmIDL::CalAppPhaseTableIDL to be populated
	 * with the content of this.
	 */
	 void toIDL(asdmIDL::CalAppPhaseTableIDL& x) const;
	 
#endif

#ifndef WITHOUT_ACS
	/**
	 * Populate this table from the content of a CalAppPhaseTableIDL Corba structure.
	 *
	 * @throws DuplicateKey Thrown if the method tries to add a row having a key that is already in the table.
	 * @throws ConversionException
	 */	
	void fromIDL(asdmIDL::CalAppPhaseTableIDL x) ;
#endif
	
	//
	// ====> Row creation.
	//
	
	/**
	 * Create a new row with default values.
	 * @return a pointer on a CalAppPhaseRow
	 */
	CalAppPhaseRow *newRow();
	
	
	/**
	 * Create a new row initialized to the specified values.
	 * @return a pointer on the created and initialized row.
	
 	 * @param basebandName
	
 	 * @param scanNumber
	
 	 * @param calDataId
	
 	 * @param calReductionId
	
 	 * @param startValidTime
	
 	 * @param endValidTime
	
 	 * @param adjustTime
	
 	 * @param adjustToken
	
 	 * @param phasingMode
	
 	 * @param numPhasedAntennas
	
 	 * @param phasedAntennas
	
 	 * @param refAntennaIndex
	
 	 * @param candRefAntennaIndex
	
 	 * @param phasePacking
	
 	 * @param numReceptors
	
 	 * @param numChannels
	
 	 * @param numPhaseValues
	
 	 * @param phaseValues
	
 	 * @param numCompare
	
 	 * @param numEfficiencies
	
 	 * @param compareArray
	
 	 * @param efficiencyIndices
	
 	 * @param efficiencies
	
 	 * @param quality
	
 	 * @param phasedSumAntenna
	
     */
	CalAppPhaseRow *newRow(BasebandNameMod::BasebandName basebandName, int scanNumber, Tag calDataId, Tag calReductionId, ArrayTime startValidTime, ArrayTime endValidTime, ArrayTime adjustTime, std::string adjustToken, std::string phasingMode, int numPhasedAntennas, std::vector<std::string > phasedAntennas, int refAntennaIndex, int candRefAntennaIndex, std::string phasePacking, int numReceptors, int numChannels, int numPhaseValues, std::vector<float > phaseValues, int numCompare, int numEfficiencies, std::vector<std::string > compareArray, std::vector<int > efficiencyIndices, std::vector<std::vector<float > > efficiencies, std::vector<float > quality, std::string phasedSumAntenna);
	


	/**
	 * Create a new row using a copy constructor mechanism.
	 * 
	 * The method creates a new CalAppPhaseRow owned by this. Each attribute of the created row 
	 * is a (deep) copy of the corresponding attribute of row. The method does not add 
	 * the created row to this, its simply parents it to this, a call to the add method
	 * has to be done in order to get the row added (very likely after having modified
	 * some of its attributes).
	 * If row is null then the method returns a new CalAppPhaseRow with default values for its attributes. 
	 *
	 * @param row the row which is to be copied.
	 */
	 CalAppPhaseRow *newRow(CalAppPhaseRow *row); 

	//
	// ====> Append a row to its table.
	//
 
	
	/**
	 * Add a row.
	 * @param x a pointer to the CalAppPhaseRow to be added.
	 *
	 * @return a pointer to a CalAppPhaseRow. If the table contains a CalAppPhaseRow whose attributes (key and mandatory values) are equal to x ones
	 * then returns a pointer on that CalAppPhaseRow, otherwise returns x.
	 *
	 * @throw DuplicateKey { thrown when the table contains a CalAppPhaseRow with a key equal to the x one but having
	 * and a value section different from x one }
	 *
	
	 */
	CalAppPhaseRow* add(CalAppPhaseRow* x) ; 

 



	//
	// ====> Methods returning rows.
	//
		
	/**
	 * Get a collection of pointers on the rows of the table.
	 * @return Alls rows in a vector of pointers of CalAppPhaseRow. The elements of this vector are stored in the order 
	 * in which they have been added to the CalAppPhaseTable.
	 */
	std::vector<CalAppPhaseRow *> get() ;
	
	/**
	 * Get a const reference on the collection of rows pointers internally hold by the table.
	 * @return A const reference of a vector of pointers of CalAppPhaseRow. The elements of this vector are stored in the order 
	 * in which they have been added to the CalAppPhaseTable.
	 *
	 */
	 const std::vector<CalAppPhaseRow *>& get() const ;
	


 
	
	/**
 	 * Returns a CalAppPhaseRow* given a key.
 	 * @return a pointer to the row having the key whose values are passed as parameters, or 0 if
 	 * no row exists for that key.
	
	 * @param basebandName
	
	 * @param scanNumber
	
	 * @param calDataId
	
	 * @param calReductionId
	
 	 *
	 */
 	CalAppPhaseRow* getRowByKey(BasebandNameMod::BasebandName basebandName, int scanNumber, Tag calDataId, Tag calReductionId);

 	 	



	/**
 	 * Look up the table for a row whose all attributes 
 	 * are equal to the corresponding parameters of the method.
 	 * @return a pointer on this row if any, null otherwise.
 	 *
			
 	 * @param basebandName
 	 		
 	 * @param scanNumber
 	 		
 	 * @param calDataId
 	 		
 	 * @param calReductionId
 	 		
 	 * @param startValidTime
 	 		
 	 * @param endValidTime
 	 		
 	 * @param adjustTime
 	 		
 	 * @param adjustToken
 	 		
 	 * @param phasingMode
 	 		
 	 * @param numPhasedAntennas
 	 		
 	 * @param phasedAntennas
 	 		
 	 * @param refAntennaIndex
 	 		
 	 * @param candRefAntennaIndex
 	 		
 	 * @param phasePacking
 	 		
 	 * @param numReceptors
 	 		
 	 * @param numChannels
 	 		
 	 * @param numPhaseValues
 	 		
 	 * @param phaseValues
 	 		
 	 * @param numCompare
 	 		
 	 * @param numEfficiencies
 	 		
 	 * @param compareArray
 	 		
 	 * @param efficiencyIndices
 	 		
 	 * @param efficiencies
 	 		
 	 * @param quality
 	 		
 	 * @param phasedSumAntenna
 	 		 
 	 */
	CalAppPhaseRow* lookup(BasebandNameMod::BasebandName basebandName, int scanNumber, Tag calDataId, Tag calReductionId, ArrayTime startValidTime, ArrayTime endValidTime, ArrayTime adjustTime, std::string adjustToken, std::string phasingMode, int numPhasedAntennas, std::vector<std::string > phasedAntennas, int refAntennaIndex, int candRefAntennaIndex, std::string phasePacking, int numReceptors, int numChannels, int numPhaseValues, std::vector<float > phaseValues, int numCompare, int numEfficiencies, std::vector<std::string > compareArray, std::vector<int > efficiencyIndices, std::vector<std::vector<float > > efficiencies, std::vector<float > quality, std::string phasedSumAntenna); 


	void setUnknownAttributeBinaryReader(const std::string& attributeName, BinaryAttributeReaderFunctor* barFctr);
	BinaryAttributeReaderFunctor* getUnknownAttributeBinaryReader(const std::string& attributeName) const;

private:

	/**
	 * Create a CalAppPhaseTable.
	 * <p>
	 * This constructor is private because only the
	 * container can create tables.  All tables must know the container
	 * to which they belong.
	 * @param container The container to which this table belongs.
	 */ 
	CalAppPhaseTable (ASDM & container);

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
	CalAppPhaseRow* checkAndAdd(CalAppPhaseRow* x, bool skipCheckUniqueness=false) ;
	
	/**
	 * Brutally append an CalAppPhaseRow x to the collection of rows already stored in this table. No uniqueness check is done !
	 *
	 * @param CalAppPhaseRow* x a pointer onto the CalAppPhaseRow to be appended.
	 */
	 void append(CalAppPhaseRow* x) ;
	 
	/**
	 * Brutally append an CalAppPhaseRow x to the collection of rows already stored in this table. No uniqueness check is done !
	 *
	 * @param CalAppPhaseRow* x a pointer onto the CalAppPhaseRow to be appended.
	 */
	 void addWithoutCheckingUnique(CalAppPhaseRow* x) ;
	 
	 



// A data structure to store the pointers on the table's rows.

// In all cases we maintain a private vector of CalAppPhaseRow s.
   std::vector<CalAppPhaseRow * > privateRows;
   

			
	std::vector<CalAppPhaseRow *> row;

	
	void error() ; //throw(ConversionException);

	
	/**
	 * Populate this table from the content of a XML document that is required to
	 * be conform to the XML schema defined for a CalAppPhase (CalAppPhaseTable.xsd).
	 * @throws ConversionException
	 * 
	 */
	void fromXML(std::string& xmlDoc) ;
		
	std::map<std::string, BinaryAttributeReaderFunctor *> unknownAttributes2Functors;

	/**
	  * Private methods involved during the build of this table out of the content
	  * of file(s) containing an external representation of a CalAppPhase table.
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
	  * will be saved in a file "CalAppPhase.bin" or an XML representation (fileAsBin==false) will be saved in a file "CalAppPhase.xml".
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
	 * Reads and parses a file containing a representation of a CalAppPhaseTable as those produced  by the toFile method.
	 * This table is populated with the result of the parsing.
	 * @param directory The name of the directory containing the file te be read and parsed.
	 * @throws ConversionException If any error occurs while reading the 
	 * files in the directory or parsing them.
	 *
	 */
	 void setFromFile(const std::string& directory);	
 
};

} // End namespace asdm

#endif /* CalAppPhaseTable_CLASS */
