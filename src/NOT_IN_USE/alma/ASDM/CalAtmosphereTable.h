
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
 * File CalAtmosphereTable.h
 */
 
#ifndef CalAtmosphereTable_CLASS
#define CalAtmosphereTable_CLASS

#include <string>
#include <vector>
#include <map>



	
#include <alma/ASDM/Temperature.h>
	

	
#include <alma/ASDM/ArrayTime.h>
	

	
#include <alma/ASDM/Pressure.h>
	

	
#include <alma/ASDM/Length.h>
	

	
#include <alma/ASDM/Humidity.h>
	

	
#include <alma/ASDM/Frequency.h>
	

	
#include <alma/ASDM/Tag.h>
	




	
#include <alma/Enumerations/CReceiverBand.h>
	

	

	
#include <alma/Enumerations/CBasebandName.h>
	

	

	

	

	

	

	

	

	

	

	

	

	
#include <alma/Enumerations/CPolarizationType.h>
	

	

	

	
#include <alma/Enumerations/CSyscalMethod.h>
	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	



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
//class asdm::CalAtmosphereRow;

class ASDM;
class CalAtmosphereRow;
/**
 * The CalAtmosphereTable class is an Alma table.
 * <BR>
 * 
 * \par Role
 * Results of atmosphere calibration by TelCal. This calibration determines the system temperatures corrected for atmospheric absorption. Ionospheric effects are not dealt with in the Table.
 * <BR>
 
 * Generated from model's revision "-1", branch ""
 *
 * <TABLE BORDER="1">
 * <CAPTION> Attributes of CalAtmosphere </CAPTION>
 * <TR BGCOLOR="#AAAAAA"> <TH> Name </TH> <TH> Type </TH> <TH> Expected shape  </TH> <TH> Comment </TH></TR>
 
 * <TR> <TH BGCOLOR="#CCCCCC" colspan="4" align="center"> Key </TD></TR>
	
 * <TR>
 		
 * <TD> antennaName </TD>
 		 
 * <TD> std::string</TD>
 * <TD> &nbsp; </TD>
 * <TD> &nbsp;the name of the antenna. </TD>
 * </TR>
	
 * <TR>
 		
 * <TD> receiverBand </TD>
 		 
 * <TD> ReceiverBandMod::ReceiverBand</TD>
 * <TD> &nbsp; </TD>
 * <TD> &nbsp;identifies the receiver band. </TD>
 * </TR>
	
 * <TR>
 		
 * <TD> basebandName </TD>
 		 
 * <TD> BasebandNameMod::BasebandName</TD>
 * <TD> &nbsp; </TD>
 * <TD> &nbsp;identifies the baseband. </TD>
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
 * <TD> numFreq (numFreq)</TD> 
 * <TD> int </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;the number of frequency points. </TD>
 * </TR>
	
 * <TR>
 * <TD> numLoad (numLoad)</TD> 
 * <TD> int </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;the number of loads. </TD>
 * </TR>
	
 * <TR>
 * <TD> numReceptor (numReceptor)</TD> 
 * <TD> int </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;the number of receptors. </TD>
 * </TR>
	
 * <TR>
 * <TD> forwardEffSpectrum </TD> 
 * <TD> std::vector<std::vector<float > > </TD>
 * <TD>  numReceptor, numFreq </TD> 
 * <TD> &nbsp;the spectra of forward efficiencies (one value per receptor, per frequency). </TD>
 * </TR>
	
 * <TR>
 * <TD> frequencyRange </TD> 
 * <TD> std::vector<Frequency > </TD>
 * <TD>  2 </TD> 
 * <TD> &nbsp;the frequency range. </TD>
 * </TR>
	
 * <TR>
 * <TD> groundPressure </TD> 
 * <TD> Pressure </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;the ground pressure. </TD>
 * </TR>
	
 * <TR>
 * <TD> groundRelHumidity </TD> 
 * <TD> Humidity </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;the ground relative humidity. </TD>
 * </TR>
	
 * <TR>
 * <TD> frequencySpectrum </TD> 
 * <TD> std::vector<Frequency > </TD>
 * <TD>  numFreq </TD> 
 * <TD> &nbsp;the frequencies. </TD>
 * </TR>
	
 * <TR>
 * <TD> groundTemperature </TD> 
 * <TD> Temperature </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;the ground temperature. </TD>
 * </TR>
	
 * <TR>
 * <TD> polarizationTypes </TD> 
 * <TD> std::vector<PolarizationTypeMod::PolarizationType > </TD>
 * <TD>  numReceptor </TD> 
 * <TD> &nbsp;the polarizations of the receptors (an array with one value per receptor). </TD>
 * </TR>
	
 * <TR>
 * <TD> powerSkySpectrum </TD> 
 * <TD> std::vector<std::vector<float > > </TD>
 * <TD>  numReceptor, numFreq </TD> 
 * <TD> &nbsp;the powers on the sky (one value per receptor per frequency). </TD>
 * </TR>
	
 * <TR>
 * <TD> powerLoadSpectrum </TD> 
 * <TD> std::vector<std::vector<std::vector<float > > > </TD>
 * <TD>  numLoad, numReceptor, numFreq </TD> 
 * <TD> &nbsp;the powers on the loads (one value per load per receptor per frequency). </TD>
 * </TR>
	
 * <TR>
 * <TD> syscalType </TD> 
 * <TD> SyscalMethodMod::SyscalMethod </TD>
 * <TD>  &nbsp;  </TD> 
 * <TD> &nbsp;the type of calibration used. </TD>
 * </TR>
	
 * <TR>
 * <TD> tAtmSpectrum </TD> 
 * <TD> std::vector<std::vector<Temperature > > </TD>
 * <TD>  numReceptor, numFreq </TD> 
 * <TD> &nbsp;the spectra of atmosphere physical  temperatures (one value per receptor per frequency). </TD>
 * </TR>
	
 * <TR>
 * <TD> tRecSpectrum </TD> 
 * <TD> std::vector<std::vector<Temperature > > </TD>
 * <TD>  numReceptor, numFreq </TD> 
 * <TD> &nbsp;the spectra of the receptors temperatures (one value  per receptor per frequency). </TD>
 * </TR>
	
 * <TR>
 * <TD> tSysSpectrum </TD> 
 * <TD> std::vector<std::vector<Temperature > > </TD>
 * <TD>  numReceptor, numFreq </TD> 
 * <TD> &nbsp;the spectra of system temperatures (one value  per receptor per frequency). </TD>
 * </TR>
	
 * <TR>
 * <TD> tauSpectrum </TD> 
 * <TD> std::vector<std::vector<float > > </TD>
 * <TD>  numReceptor, numFreq </TD> 
 * <TD> &nbsp;the spectra of atmosheric optical depths (one value  per receptor per frequency). </TD>
 * </TR>
	
 * <TR>
 * <TD> tAtm </TD> 
 * <TD> std::vector<Temperature > </TD>
 * <TD>  numReceptor </TD> 
 * <TD> &nbsp;the atmosphere physical temperatures (one value per receptor). </TD>
 * </TR>
	
 * <TR>
 * <TD> tRec </TD> 
 * <TD> std::vector<Temperature > </TD>
 * <TD>  numReceptor </TD> 
 * <TD> &nbsp;the receptors temperatures (one value per receptor). </TD>
 * </TR>
	
 * <TR>
 * <TD> tSys </TD> 
 * <TD> std::vector<Temperature > </TD>
 * <TD>  numReceptor </TD> 
 * <TD> &nbsp;the system temperatures (one value per receptor). </TD>
 * </TR>
	
 * <TR>
 * <TD> tau </TD> 
 * <TD> std::vector<float > </TD>
 * <TD>  numReceptor </TD> 
 * <TD> &nbsp;the atmospheric optical depths (one value per receptor). </TD>
 * </TR>
	
 * <TR>
 * <TD> water </TD> 
 * <TD> std::vector<Length > </TD>
 * <TD>  numReceptor </TD> 
 * <TD> &nbsp;the water vapor path lengths (one value per receptor). </TD>
 * </TR>
	
 * <TR>
 * <TD> waterError </TD> 
 * <TD> std::vector<Length > </TD>
 * <TD>  numReceptor </TD> 
 * <TD> &nbsp;the uncertainties of water vapor contents (one value per receptor). </TD>
 * </TR>
	


 * <TR> <TH BGCOLOR="#CCCCCC"  colspan="4" valign="center"> Value <br> (Optional) </TH></TR>
	
 * <TR>
 * <TD> alphaSpectrum</TD> 
 * <TD> std::vector<std::vector<float > > </TD>
 * <TD>  numReceptor, numFreq  </TD>
 * <TD>&nbsp; the alpha coefficients, two loads only (one value per receptor per frequency). </TD>
 * </TR>
	
 * <TR>
 * <TD> forwardEfficiency</TD> 
 * <TD> std::vector<float > </TD>
 * <TD>  numReceptor  </TD>
 * <TD>&nbsp; the forward efficiencies (one value per receptor). </TD>
 * </TR>
	
 * <TR>
 * <TD> forwardEfficiencyError</TD> 
 * <TD> std::vector<double > </TD>
 * <TD>  numReceptor  </TD>
 * <TD>&nbsp; the uncertainties on forwardEfficiency (one value per receptor). </TD>
 * </TR>
	
 * <TR>
 * <TD> sbGain</TD> 
 * <TD> std::vector<float > </TD>
 * <TD>  numReceptor  </TD>
 * <TD>&nbsp; the relative gains of LO1 sideband (one value per receptor). </TD>
 * </TR>
	
 * <TR>
 * <TD> sbGainError</TD> 
 * <TD> std::vector<float > </TD>
 * <TD>  numReceptor  </TD>
 * <TD>&nbsp; the uncertainties on the relative gains of LO1 sideband (one value per receptor). </TD>
 * </TR>
	
 * <TR>
 * <TD> sbGainSpectrum</TD> 
 * <TD> std::vector<std::vector<float > > </TD>
 * <TD>  numReceptor, numFreq  </TD>
 * <TD>&nbsp; the spectra of relative sideband gains (one value  per receptor per frequency). </TD>
 * </TR>
	

 * </TABLE>
 */
class CalAtmosphereTable : public Representable {
	friend class ASDM;

public:


	/**
	 * Return the list of field names that make up key key
	 * as an array of strings.
	 * @return a vector of string.
	 */	
	static const std::vector<std::string>& getKeyName();


	virtual ~CalAtmosphereTable();
	
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
	 * to the schema defined for CalAtmosphere (CalAtmosphereTable.xsd).
	 *
	 * @returns a string containing the XML representation.
	 * @throws ConversionException
	 */
	std::string toXML()  ;

#ifndef WITHOUT_ACS
	// Conversion Methods
	/**
	 * Convert this table into a CalAtmosphereTableIDL CORBA structure.
	 *
	 * @return a pointer to a CalAtmosphereTableIDL
	 */
	asdmIDL::CalAtmosphereTableIDL *toIDL() ;
	
	/**
	 * Fills the CORBA data structure passed in parameter
	 * with the content of this table.
	 *
	 * @param x a reference to the asdmIDL::CalAtmosphereTableIDL to be populated
	 * with the content of this.
	 */
	 void toIDL(asdmIDL::CalAtmosphereTableIDL& x) const;
	 
#endif

#ifndef WITHOUT_ACS
	/**
	 * Populate this table from the content of a CalAtmosphereTableIDL Corba structure.
	 *
	 * @throws DuplicateKey Thrown if the method tries to add a row having a key that is already in the table.
	 * @throws ConversionException
	 */	
	void fromIDL(asdmIDL::CalAtmosphereTableIDL x) ;
#endif
	
	//
	// ====> Row creation.
	//
	
	/**
	 * Create a new row with default values.
	 * @return a pointer on a CalAtmosphereRow
	 */
	CalAtmosphereRow *newRow();
	
	
	/**
	 * Create a new row initialized to the specified values.
	 * @return a pointer on the created and initialized row.
	
 	 * @param antennaName
	
 	 * @param receiverBand
	
 	 * @param basebandName
	
 	 * @param calDataId
	
 	 * @param calReductionId
	
 	 * @param startValidTime
	
 	 * @param endValidTime
	
 	 * @param numFreq
	
 	 * @param numLoad
	
 	 * @param numReceptor
	
 	 * @param forwardEffSpectrum
	
 	 * @param frequencyRange
	
 	 * @param groundPressure
	
 	 * @param groundRelHumidity
	
 	 * @param frequencySpectrum
	
 	 * @param groundTemperature
	
 	 * @param polarizationTypes
	
 	 * @param powerSkySpectrum
	
 	 * @param powerLoadSpectrum
	
 	 * @param syscalType
	
 	 * @param tAtmSpectrum
	
 	 * @param tRecSpectrum
	
 	 * @param tSysSpectrum
	
 	 * @param tauSpectrum
	
 	 * @param tAtm
	
 	 * @param tRec
	
 	 * @param tSys
	
 	 * @param tau
	
 	 * @param water
	
 	 * @param waterError
	
     */
	CalAtmosphereRow *newRow(std::string antennaName, ReceiverBandMod::ReceiverBand receiverBand, BasebandNameMod::BasebandName basebandName, Tag calDataId, Tag calReductionId, ArrayTime startValidTime, ArrayTime endValidTime, int numFreq, int numLoad, int numReceptor, std::vector<std::vector<float > > forwardEffSpectrum, std::vector<Frequency > frequencyRange, Pressure groundPressure, Humidity groundRelHumidity, std::vector<Frequency > frequencySpectrum, Temperature groundTemperature, std::vector<PolarizationTypeMod::PolarizationType > polarizationTypes, std::vector<std::vector<float > > powerSkySpectrum, std::vector<std::vector<std::vector<float > > > powerLoadSpectrum, SyscalMethodMod::SyscalMethod syscalType, std::vector<std::vector<Temperature > > tAtmSpectrum, std::vector<std::vector<Temperature > > tRecSpectrum, std::vector<std::vector<Temperature > > tSysSpectrum, std::vector<std::vector<float > > tauSpectrum, std::vector<Temperature > tAtm, std::vector<Temperature > tRec, std::vector<Temperature > tSys, std::vector<float > tau, std::vector<Length > water, std::vector<Length > waterError);
	


	/**
	 * Create a new row using a copy constructor mechanism.
	 * 
	 * The method creates a new CalAtmosphereRow owned by this. Each attribute of the created row 
	 * is a (deep) copy of the corresponding attribute of row. The method does not add 
	 * the created row to this, its simply parents it to this, a call to the add method
	 * has to be done in order to get the row added (very likely after having modified
	 * some of its attributes).
	 * If row is null then the method returns a new CalAtmosphereRow with default values for its attributes. 
	 *
	 * @param row the row which is to be copied.
	 */
	 CalAtmosphereRow *newRow(CalAtmosphereRow *row); 

	//
	// ====> Append a row to its table.
	//
 
	
	/**
	 * Add a row.
	 * @param x a pointer to the CalAtmosphereRow to be added.
	 *
	 * @return a pointer to a CalAtmosphereRow. If the table contains a CalAtmosphereRow whose attributes (key and mandatory values) are equal to x ones
	 * then returns a pointer on that CalAtmosphereRow, otherwise returns x.
	 *
	 * @throw DuplicateKey { thrown when the table contains a CalAtmosphereRow with a key equal to the x one but having
	 * and a value section different from x one }
	 *
	
	 */
	CalAtmosphereRow* add(CalAtmosphereRow* x) ; 

 



	//
	// ====> Methods returning rows.
	//
		
	/**
	 * Get a collection of pointers on the rows of the table.
	 * @return Alls rows in a vector of pointers of CalAtmosphereRow. The elements of this vector are stored in the order 
	 * in which they have been added to the CalAtmosphereTable.
	 */
	std::vector<CalAtmosphereRow *> get() ;
	
	/**
	 * Get a const reference on the collection of rows pointers internally hold by the table.
	 * @return A const reference of a vector of pointers of CalAtmosphereRow. The elements of this vector are stored in the order 
	 * in which they have been added to the CalAtmosphereTable.
	 *
	 */
	 const std::vector<CalAtmosphereRow *>& get() const ;
	


 
	
	/**
 	 * Returns a CalAtmosphereRow* given a key.
 	 * @return a pointer to the row having the key whose values are passed as parameters, or 0 if
 	 * no row exists for that key.
	
	 * @param antennaName
	
	 * @param receiverBand
	
	 * @param basebandName
	
	 * @param calDataId
	
	 * @param calReductionId
	
 	 *
	 */
 	CalAtmosphereRow* getRowByKey(std::string antennaName, ReceiverBandMod::ReceiverBand receiverBand, BasebandNameMod::BasebandName basebandName, Tag calDataId, Tag calReductionId);

 	 	



	/**
 	 * Look up the table for a row whose all attributes 
 	 * are equal to the corresponding parameters of the method.
 	 * @return a pointer on this row if any, null otherwise.
 	 *
			
 	 * @param antennaName
 	 		
 	 * @param receiverBand
 	 		
 	 * @param basebandName
 	 		
 	 * @param calDataId
 	 		
 	 * @param calReductionId
 	 		
 	 * @param startValidTime
 	 		
 	 * @param endValidTime
 	 		
 	 * @param numFreq
 	 		
 	 * @param numLoad
 	 		
 	 * @param numReceptor
 	 		
 	 * @param forwardEffSpectrum
 	 		
 	 * @param frequencyRange
 	 		
 	 * @param groundPressure
 	 		
 	 * @param groundRelHumidity
 	 		
 	 * @param frequencySpectrum
 	 		
 	 * @param groundTemperature
 	 		
 	 * @param polarizationTypes
 	 		
 	 * @param powerSkySpectrum
 	 		
 	 * @param powerLoadSpectrum
 	 		
 	 * @param syscalType
 	 		
 	 * @param tAtmSpectrum
 	 		
 	 * @param tRecSpectrum
 	 		
 	 * @param tSysSpectrum
 	 		
 	 * @param tauSpectrum
 	 		
 	 * @param tAtm
 	 		
 	 * @param tRec
 	 		
 	 * @param tSys
 	 		
 	 * @param tau
 	 		
 	 * @param water
 	 		
 	 * @param waterError
 	 		 
 	 */
	CalAtmosphereRow* lookup(std::string antennaName, ReceiverBandMod::ReceiverBand receiverBand, BasebandNameMod::BasebandName basebandName, Tag calDataId, Tag calReductionId, ArrayTime startValidTime, ArrayTime endValidTime, int numFreq, int numLoad, int numReceptor, std::vector<std::vector<float > > forwardEffSpectrum, std::vector<Frequency > frequencyRange, Pressure groundPressure, Humidity groundRelHumidity, std::vector<Frequency > frequencySpectrum, Temperature groundTemperature, std::vector<PolarizationTypeMod::PolarizationType > polarizationTypes, std::vector<std::vector<float > > powerSkySpectrum, std::vector<std::vector<std::vector<float > > > powerLoadSpectrum, SyscalMethodMod::SyscalMethod syscalType, std::vector<std::vector<Temperature > > tAtmSpectrum, std::vector<std::vector<Temperature > > tRecSpectrum, std::vector<std::vector<Temperature > > tSysSpectrum, std::vector<std::vector<float > > tauSpectrum, std::vector<Temperature > tAtm, std::vector<Temperature > tRec, std::vector<Temperature > tSys, std::vector<float > tau, std::vector<Length > water, std::vector<Length > waterError); 


	void setUnknownAttributeBinaryReader(const std::string& attributeName, BinaryAttributeReaderFunctor* barFctr);
	BinaryAttributeReaderFunctor* getUnknownAttributeBinaryReader(const std::string& attributeName) const;

private:

	/**
	 * Create a CalAtmosphereTable.
	 * <p>
	 * This constructor is private because only the
	 * container can create tables.  All tables must know the container
	 * to which they belong.
	 * @param container The container to which this table belongs.
	 */ 
	CalAtmosphereTable (ASDM & container);

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
	CalAtmosphereRow* checkAndAdd(CalAtmosphereRow* x, bool skipCheckUniqueness=false) ;
	
	/**
	 * Brutally append an CalAtmosphereRow x to the collection of rows already stored in this table. No uniqueness check is done !
	 *
	 * @param CalAtmosphereRow* x a pointer onto the CalAtmosphereRow to be appended.
	 */
	 void append(CalAtmosphereRow* x) ;
	 
	/**
	 * Brutally append an CalAtmosphereRow x to the collection of rows already stored in this table. No uniqueness check is done !
	 *
	 * @param CalAtmosphereRow* x a pointer onto the CalAtmosphereRow to be appended.
	 */
	 void addWithoutCheckingUnique(CalAtmosphereRow* x) ;
	 
	 



// A data structure to store the pointers on the table's rows.

// In all cases we maintain a private vector of CalAtmosphereRow s.
   std::vector<CalAtmosphereRow * > privateRows;
   

			
	std::vector<CalAtmosphereRow *> row;

	
	void error() ; //throw(ConversionException);

	
	/**
	 * Populate this table from the content of a XML document that is required to
	 * be conform to the XML schema defined for a CalAtmosphere (CalAtmosphereTable.xsd).
	 * @throws ConversionException
	 * 
	 */
	void fromXML(std::string& xmlDoc) ;
		
	std::map<std::string, BinaryAttributeReaderFunctor *> unknownAttributes2Functors;

	/**
	  * Private methods involved during the build of this table out of the content
	  * of file(s) containing an external representation of a CalAtmosphere table.
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
	  * will be saved in a file "CalAtmosphere.bin" or an XML representation (fileAsBin==false) will be saved in a file "CalAtmosphere.xml".
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
	 * Reads and parses a file containing a representation of a CalAtmosphereTable as those produced  by the toFile method.
	 * This table is populated with the result of the parsing.
	 * @param directory The name of the directory containing the file te be read and parsed.
	 * @throws ConversionException If any error occurs while reading the 
	 * files in the directory or parsing them.
	 *
	 */
	 void setFromFile(const std::string& directory);	
 
};

} // End namespace asdm

#endif /* CalAtmosphereTable_CLASS */
