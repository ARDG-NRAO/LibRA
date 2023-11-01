
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
 * File CalPointingTable.h
 */
 
#ifndef CalPointingTable_CLASS
#define CalPointingTable_CLASS

#include <string>
#include <vector>
#include <map>



	
#include <alma/ASDM/Temperature.h>
	

	
#include <alma/ASDM/ArrayTime.h>
	

	
#include <alma/ASDM/Angle.h>
	

	
#include <alma/ASDM/Frequency.h>
	

	
#include <alma/ASDM/Tag.h>
	




	

	
#include <alma/Enumerations/CReceiverBand.h>
	

	

	

	

	
#include <alma/Enumerations/CAntennaMake.h>
	

	
#include <alma/Enumerations/CAtmPhaseCorrection.h>
	

	

	

	
#include <alma/Enumerations/CPointingModelMode.h>
	

	
#include <alma/Enumerations/CPointingMethod.h>
	

	

	
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
//class asdm::CalPointingRow;

class ASDM;
class CalPointingRow;
/**
 * The CalPointingTable class is an Alma table.
 * <BR>
 * 
 * \par Role
 * Result of the pointing calibration performed on-line by TelCal.
 * <BR>
 
 * Generated from model's revision "-1", branch ""
 *
 * <TABLE BORDER="1">
 * <CAPTION> Attributes of CalPointing </CAPTION>
 * <TR BGCOLOR="#AAAAAA"> <TH> Name </TH> <TH> Type </TH> <TH> Expected shape  </TH> <TH> Comment </TH></TR>
 
 * <TR> <TH BGCOLOR="#CCCCCC" colspan="4" align="center"> Key </TD></TR>
	
 * <TR>
 		
 * <TD> antennaName </TD>
 		 
 * <TD> std::string</TD>
 * <TD> &nbsp; </TD>
 * <TD> &nbsp;Antenna Name </TD>
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
 * <TD> ambientTemperature </TD> 
 * <TD> Temperature </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;the ambient temperature. </TD>
 * </TR>
	
 * <TR>
 * <TD> antennaMake </TD> 
 * <TD> AntennaMakeMod::AntennaMake </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;identifies the antenna make. </TD>
 * </TR>
	
 * <TR>
 * <TD> atmPhaseCorrection </TD> 
 * <TD> AtmPhaseCorrectionMod::AtmPhaseCorrection </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;describes how the atmospheric phase correction has been applied. </TD>
 * </TR>
	
 * <TR>
 * <TD> direction </TD> 
 * <TD> std::vector<Angle > </TD>
 * <TD>  2 </TD> 
 * <TD> &nbsp;the antenna pointing direction. </TD>
 * </TR>
	
 * <TR>
 * <TD> frequencyRange </TD> 
 * <TD> std::vector<Frequency > </TD>
 * <TD>  2 </TD> 
 * <TD> &nbsp;the frequency range over which the result is valid. </TD>
 * </TR>
	
 * <TR>
 * <TD> pointingModelMode </TD> 
 * <TD> PointingModelModeMod::PointingModelMode </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;identifies the pointing model mode. </TD>
 * </TR>
	
 * <TR>
 * <TD> pointingMethod </TD> 
 * <TD> PointingMethodMod::PointingMethod </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;identifies the pointing method. </TD>
 * </TR>
	
 * <TR>
 * <TD> numReceptor (numReceptor)</TD> 
 * <TD> int </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;the number of receptors. </TD>
 * </TR>
	
 * <TR>
 * <TD> polarizationTypes </TD> 
 * <TD> std::vector<PolarizationTypeMod::PolarizationType > </TD>
 * <TD>  numReceptor </TD> 
 * <TD> &nbsp;identifies the polarizations types (one value per receptor). </TD>
 * </TR>
	
 * <TR>
 * <TD> collOffsetRelative </TD> 
 * <TD> std::vector<std::vector<Angle > > </TD>
 * <TD>  numReceptor, 2 </TD> 
 * <TD> &nbsp;the collimation offsets (relative) (one pair of angles  per receptor). </TD>
 * </TR>
	
 * <TR>
 * <TD> collOffsetAbsolute </TD> 
 * <TD> std::vector<std::vector<Angle > > </TD>
 * <TD>  numReceptor, 2 </TD> 
 * <TD> &nbsp;the collimation offsets (absolute) (one pair of angles per receptor). </TD>
 * </TR>
	
 * <TR>
 * <TD> collError </TD> 
 * <TD> std::vector<std::vector<Angle > > </TD>
 * <TD>  numReceptor, 2 </TD> 
 * <TD> &nbsp;the uncertainties on collimation (one pair of angles per receptor) </TD>
 * </TR>
	
 * <TR>
 * <TD> collOffsetTied </TD> 
 * <TD> std::vector<std::vector<bool > > </TD>
 * <TD>  numReceptor, 2 </TD> 
 * <TD> &nbsp;indicates if a collimation offset was tied (true) or not tied (false) to another polar (one pair of boolean values per receptor). </TD>
 * </TR>
	
 * <TR>
 * <TD> reducedChiSquared </TD> 
 * <TD> std::vector<double > </TD>
 * <TD>  numReceptor </TD> 
 * <TD> &nbsp;a measure of the quality of the least square fit. </TD>
 * </TR>
	


 * <TR> <TH BGCOLOR="#CCCCCC"  colspan="4" valign="center"> Value <br> (Optional) </TH></TR>
	
 * <TR>
 * <TD> averagedPolarizations</TD> 
 * <TD> bool </TD>
 * <TD>  &nbsp; </TD>
 * <TD>&nbsp; true when the polarizations were averaged together to improve sensitivity. </TD>
 * </TR>
	
 * <TR>
 * <TD> beamPA</TD> 
 * <TD> std::vector<Angle > </TD>
 * <TD>  numReceptor  </TD>
 * <TD>&nbsp; the fitted beam position angles (one value per receptor). </TD>
 * </TR>
	
 * <TR>
 * <TD> beamPAError</TD> 
 * <TD> std::vector<Angle > </TD>
 * <TD>  numReceptor  </TD>
 * <TD>&nbsp; the uncertaintes on the fitted beam position angles (one value per receptor). </TD>
 * </TR>
	
 * <TR>
 * <TD> beamPAWasFixed</TD> 
 * <TD> bool </TD>
 * <TD>  &nbsp; </TD>
 * <TD>&nbsp; indicates if the beam position was fixed (true) or not fixed (false). </TD>
 * </TR>
	
 * <TR>
 * <TD> beamWidth</TD> 
 * <TD> std::vector<std::vector<Angle > > </TD>
 * <TD>  numReceptor, 2  </TD>
 * <TD>&nbsp; the fitted beam widths (one pair of angles per receptor). </TD>
 * </TR>
	
 * <TR>
 * <TD> beamWidthError</TD> 
 * <TD> std::vector<std::vector<Angle > > </TD>
 * <TD>  numReceptor, 2  </TD>
 * <TD>&nbsp; the uncertainties on the fitted beam widths (one pair of angles per receptor). </TD>
 * </TR>
	
 * <TR>
 * <TD> beamWidthWasFixed</TD> 
 * <TD> std::vector<bool > </TD>
 * <TD>  2  </TD>
 * <TD>&nbsp; indicates if the beam width was fixed (true) or not fixed (true) (one pair of booleans). </TD>
 * </TR>
	
 * <TR>
 * <TD> offIntensity</TD> 
 * <TD> std::vector<Temperature > </TD>
 * <TD>  numReceptor  </TD>
 * <TD>&nbsp; the off intensity levels (one value per receptor). </TD>
 * </TR>
	
 * <TR>
 * <TD> offIntensityError</TD> 
 * <TD> std::vector<Temperature > </TD>
 * <TD>  numReceptor  </TD>
 * <TD>&nbsp; the uncertainties on the off intensity levels (one value per receptor). </TD>
 * </TR>
	
 * <TR>
 * <TD> offIntensityWasFixed</TD> 
 * <TD> bool </TD>
 * <TD>  &nbsp; </TD>
 * <TD>&nbsp; indicates if the off intensity level was fixed (true) or not fixed (false). </TD>
 * </TR>
	
 * <TR>
 * <TD> peakIntensity</TD> 
 * <TD> std::vector<Temperature > </TD>
 * <TD>  numReceptor  </TD>
 * <TD>&nbsp; the maximum intensities (one value per receptor). </TD>
 * </TR>
	
 * <TR>
 * <TD> peakIntensityError</TD> 
 * <TD> std::vector<Temperature > </TD>
 * <TD>  numReceptor  </TD>
 * <TD>&nbsp; the uncertainties on the maximum intensities (one value per receptor). </TD>
 * </TR>
	
 * <TR>
 * <TD> peakIntensityWasFixed</TD> 
 * <TD> bool </TD>
 * <TD>  &nbsp; </TD>
 * <TD>&nbsp; the maximum intensity was fixed. </TD>
 * </TR>
	

 * </TABLE>
 */
class CalPointingTable : public Representable {
	friend class ASDM;

public:


	/**
	 * Return the list of field names that make up key key
	 * as an array of strings.
	 * @return a vector of string.
	 */	
	static const std::vector<std::string>& getKeyName();


	virtual ~CalPointingTable();
	
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
	 * to the schema defined for CalPointing (CalPointingTable.xsd).
	 *
	 * @returns a string containing the XML representation.
	 * @throws ConversionException
	 */
	std::string toXML()  ;

#ifndef WITHOUT_ACS
	// Conversion Methods
	/**
	 * Convert this table into a CalPointingTableIDL CORBA structure.
	 *
	 * @return a pointer to a CalPointingTableIDL
	 */
	asdmIDL::CalPointingTableIDL *toIDL() ;
	
	/**
	 * Fills the CORBA data structure passed in parameter
	 * with the content of this table.
	 *
	 * @param x a reference to the asdmIDL::CalPointingTableIDL to be populated
	 * with the content of this.
	 */
	 void toIDL(asdmIDL::CalPointingTableIDL& x) const;
	 
#endif

#ifndef WITHOUT_ACS
	/**
	 * Populate this table from the content of a CalPointingTableIDL Corba structure.
	 *
	 * @throws DuplicateKey Thrown if the method tries to add a row having a key that is already in the table.
	 * @throws ConversionException
	 */	
	void fromIDL(asdmIDL::CalPointingTableIDL x) ;
#endif
	
	//
	// ====> Row creation.
	//
	
	/**
	 * Create a new row with default values.
	 * @return a pointer on a CalPointingRow
	 */
	CalPointingRow *newRow();
	
	
	/**
	 * Create a new row initialized to the specified values.
	 * @return a pointer on the created and initialized row.
	
 	 * @param antennaName
	
 	 * @param receiverBand
	
 	 * @param calDataId
	
 	 * @param calReductionId
	
 	 * @param startValidTime
	
 	 * @param endValidTime
	
 	 * @param ambientTemperature
	
 	 * @param antennaMake
	
 	 * @param atmPhaseCorrection
	
 	 * @param direction
	
 	 * @param frequencyRange
	
 	 * @param pointingModelMode
	
 	 * @param pointingMethod
	
 	 * @param numReceptor
	
 	 * @param polarizationTypes
	
 	 * @param collOffsetRelative
	
 	 * @param collOffsetAbsolute
	
 	 * @param collError
	
 	 * @param collOffsetTied
	
 	 * @param reducedChiSquared
	
     */
	CalPointingRow *newRow(std::string antennaName, ReceiverBandMod::ReceiverBand receiverBand, Tag calDataId, Tag calReductionId, ArrayTime startValidTime, ArrayTime endValidTime, Temperature ambientTemperature, AntennaMakeMod::AntennaMake antennaMake, AtmPhaseCorrectionMod::AtmPhaseCorrection atmPhaseCorrection, std::vector<Angle > direction, std::vector<Frequency > frequencyRange, PointingModelModeMod::PointingModelMode pointingModelMode, PointingMethodMod::PointingMethod pointingMethod, int numReceptor, std::vector<PolarizationTypeMod::PolarizationType > polarizationTypes, std::vector<std::vector<Angle > > collOffsetRelative, std::vector<std::vector<Angle > > collOffsetAbsolute, std::vector<std::vector<Angle > > collError, std::vector<std::vector<bool > > collOffsetTied, std::vector<double > reducedChiSquared);
	


	/**
	 * Create a new row using a copy constructor mechanism.
	 * 
	 * The method creates a new CalPointingRow owned by this. Each attribute of the created row 
	 * is a (deep) copy of the corresponding attribute of row. The method does not add 
	 * the created row to this, its simply parents it to this, a call to the add method
	 * has to be done in order to get the row added (very likely after having modified
	 * some of its attributes).
	 * If row is null then the method returns a new CalPointingRow with default values for its attributes. 
	 *
	 * @param row the row which is to be copied.
	 */
	 CalPointingRow *newRow(CalPointingRow *row); 

	//
	// ====> Append a row to its table.
	//
 
	
	/**
	 * Add a row.
	 * @param x a pointer to the CalPointingRow to be added.
	 *
	 * @return a pointer to a CalPointingRow. If the table contains a CalPointingRow whose attributes (key and mandatory values) are equal to x ones
	 * then returns a pointer on that CalPointingRow, otherwise returns x.
	 *
	 * @throw DuplicateKey { thrown when the table contains a CalPointingRow with a key equal to the x one but having
	 * and a value section different from x one }
	 *
	
	 */
	CalPointingRow* add(CalPointingRow* x) ; 

 



	//
	// ====> Methods returning rows.
	//
		
	/**
	 * Get a collection of pointers on the rows of the table.
	 * @return Alls rows in a vector of pointers of CalPointingRow. The elements of this vector are stored in the order 
	 * in which they have been added to the CalPointingTable.
	 */
	std::vector<CalPointingRow *> get() ;
	
	/**
	 * Get a const reference on the collection of rows pointers internally hold by the table.
	 * @return A const reference of a vector of pointers of CalPointingRow. The elements of this vector are stored in the order 
	 * in which they have been added to the CalPointingTable.
	 *
	 */
	 const std::vector<CalPointingRow *>& get() const ;
	


 
	
	/**
 	 * Returns a CalPointingRow* given a key.
 	 * @return a pointer to the row having the key whose values are passed as parameters, or 0 if
 	 * no row exists for that key.
	
	 * @param antennaName
	
	 * @param receiverBand
	
	 * @param calDataId
	
	 * @param calReductionId
	
 	 *
	 */
 	CalPointingRow* getRowByKey(std::string antennaName, ReceiverBandMod::ReceiverBand receiverBand, Tag calDataId, Tag calReductionId);

 	 	



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
 	 		
 	 * @param ambientTemperature
 	 		
 	 * @param antennaMake
 	 		
 	 * @param atmPhaseCorrection
 	 		
 	 * @param direction
 	 		
 	 * @param frequencyRange
 	 		
 	 * @param pointingModelMode
 	 		
 	 * @param pointingMethod
 	 		
 	 * @param numReceptor
 	 		
 	 * @param polarizationTypes
 	 		
 	 * @param collOffsetRelative
 	 		
 	 * @param collOffsetAbsolute
 	 		
 	 * @param collError
 	 		
 	 * @param collOffsetTied
 	 		
 	 * @param reducedChiSquared
 	 		 
 	 */
	CalPointingRow* lookup(std::string antennaName, ReceiverBandMod::ReceiverBand receiverBand, Tag calDataId, Tag calReductionId, ArrayTime startValidTime, ArrayTime endValidTime, Temperature ambientTemperature, AntennaMakeMod::AntennaMake antennaMake, AtmPhaseCorrectionMod::AtmPhaseCorrection atmPhaseCorrection, std::vector<Angle > direction, std::vector<Frequency > frequencyRange, PointingModelModeMod::PointingModelMode pointingModelMode, PointingMethodMod::PointingMethod pointingMethod, int numReceptor, std::vector<PolarizationTypeMod::PolarizationType > polarizationTypes, std::vector<std::vector<Angle > > collOffsetRelative, std::vector<std::vector<Angle > > collOffsetAbsolute, std::vector<std::vector<Angle > > collError, std::vector<std::vector<bool > > collOffsetTied, std::vector<double > reducedChiSquared); 


	void setUnknownAttributeBinaryReader(const std::string& attributeName, BinaryAttributeReaderFunctor* barFctr);
	BinaryAttributeReaderFunctor* getUnknownAttributeBinaryReader(const std::string& attributeName) const;

private:

	/**
	 * Create a CalPointingTable.
	 * <p>
	 * This constructor is private because only the
	 * container can create tables.  All tables must know the container
	 * to which they belong.
	 * @param container The container to which this table belongs.
	 */ 
	CalPointingTable (ASDM & container);

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
	CalPointingRow* checkAndAdd(CalPointingRow* x, bool skipCheckUniqueness=false) ;
	
	/**
	 * Brutally append an CalPointingRow x to the collection of rows already stored in this table. No uniqueness check is done !
	 *
	 * @param CalPointingRow* x a pointer onto the CalPointingRow to be appended.
	 */
	 void append(CalPointingRow* x) ;
	 
	/**
	 * Brutally append an CalPointingRow x to the collection of rows already stored in this table. No uniqueness check is done !
	 *
	 * @param CalPointingRow* x a pointer onto the CalPointingRow to be appended.
	 */
	 void addWithoutCheckingUnique(CalPointingRow* x) ;
	 
	 



// A data structure to store the pointers on the table's rows.

// In all cases we maintain a private vector of CalPointingRow s.
   std::vector<CalPointingRow * > privateRows;
   

			
	std::vector<CalPointingRow *> row;

	
	void error() ; //throw(ConversionException);

	
	/**
	 * Populate this table from the content of a XML document that is required to
	 * be conform to the XML schema defined for a CalPointing (CalPointingTable.xsd).
	 * @throws ConversionException
	 * 
	 */
	void fromXML(std::string& xmlDoc) ;
		
	std::map<std::string, BinaryAttributeReaderFunctor *> unknownAttributes2Functors;

	/**
	  * Private methods involved during the build of this table out of the content
	  * of file(s) containing an external representation of a CalPointing table.
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
	  * will be saved in a file "CalPointing.bin" or an XML representation (fileAsBin==false) will be saved in a file "CalPointing.xml".
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
	 * Reads and parses a file containing a representation of a CalPointingTable as those produced  by the toFile method.
	 * This table is populated with the result of the parsing.
	 * @param directory The name of the directory containing the file te be read and parsed.
	 * @throws ConversionException If any error occurs while reading the 
	 * files in the directory or parsing them.
	 *
	 */
	 void setFromFile(const std::string& directory);	
 
};

} // End namespace asdm

#endif /* CalPointingTable_CLASS */
