
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
 * File CalAtmosphereRow.h
 */
 
#ifndef CalAtmosphereRow_CLASS
#define CalAtmosphereRow_CLASS

#include <vector>
#include <string>
#include <set>

#ifndef WITHOUT_ACS
#include <asdmIDLC.h>
#endif






	 
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
#include <alma/ASDM/NoSuchRow.h>
#include <alma/ASDM/IllegalAccessException.h>

#include <alma/ASDM/RowTransformer.h>
//#include <alma/ASDM/TableStreamReader.h>

/*\file CalAtmosphere.h
    \brief Generated from model's revision "-1", branch ""
*/

namespace asdm {

//class asdm::CalAtmosphereTable;


// class asdm::CalReductionRow;
class CalReductionRow;

// class asdm::CalDataRow;
class CalDataRow;
	

class CalAtmosphereRow;
typedef void (CalAtmosphereRow::*CalAtmosphereAttributeFromBin) (EndianIStream& eis);
typedef void (CalAtmosphereRow::*CalAtmosphereAttributeFromText) (const std::string& s);

/**
 * The CalAtmosphereRow class is a row of a CalAtmosphereTable.
 * 
 * Generated from model's revision "-1", branch ""
 *
 */
class CalAtmosphereRow {
friend class asdm::CalAtmosphereTable;
friend class asdm::RowTransformer<CalAtmosphereRow>;
//friend class asdm::TableStreamReader<CalAtmosphereTable, CalAtmosphereRow>;

public:

	virtual ~CalAtmosphereRow();

	/**
	 * Return the table to which this row belongs.
	 */
	CalAtmosphereTable &getTable() const;
	
	/**
	 * Has this row been added to its table ?
	 * @return true if and only if it has been added.
	 */
	bool isAdded() const;
		
	////////////////////////////////
	// Intrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute receiverBand
	
	
	

	
 	/**
 	 * Get receiverBand.
 	 * @return receiverBand as ReceiverBandMod::ReceiverBand
 	 */
 	ReceiverBandMod::ReceiverBand getReceiverBand() const;
	
 
 	
 	
 	/**
 	 * Set receiverBand with the specified ReceiverBandMod::ReceiverBand.
 	 * @param receiverBand The ReceiverBandMod::ReceiverBand value to which receiverBand is to be set.
 	 
 		
 			
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 		
 	 */
 	void setReceiverBand (ReceiverBandMod::ReceiverBand receiverBand);
  		
	
	
	


	
	// ===> Attribute antennaName
	
	
	

	
 	/**
 	 * Get antennaName.
 	 * @return antennaName as std::string
 	 */
 	std::string getAntennaName() const;
	
 
 	
 	
 	/**
 	 * Set antennaName with the specified std::string.
 	 * @param antennaName The std::string value to which antennaName is to be set.
 	 
 		
 			
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 		
 	 */
 	void setAntennaName (std::string antennaName);
  		
	
	
	


	
	// ===> Attribute basebandName
	
	
	

	
 	/**
 	 * Get basebandName.
 	 * @return basebandName as BasebandNameMod::BasebandName
 	 */
 	BasebandNameMod::BasebandName getBasebandName() const;
	
 
 	
 	
 	/**
 	 * Set basebandName with the specified BasebandNameMod::BasebandName.
 	 * @param basebandName The BasebandNameMod::BasebandName value to which basebandName is to be set.
 	 
 		
 			
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 		
 	 */
 	void setBasebandName (BasebandNameMod::BasebandName basebandName);
  		
	
	
	


	
	// ===> Attribute startValidTime
	
	
	

	
 	/**
 	 * Get startValidTime.
 	 * @return startValidTime as ArrayTime
 	 */
 	ArrayTime getStartValidTime() const;
	
 
 	
 	
 	/**
 	 * Set startValidTime with the specified ArrayTime.
 	 * @param startValidTime The ArrayTime value to which startValidTime is to be set.
 	 
 		
 			
 	 */
 	void setStartValidTime (ArrayTime startValidTime);
  		
	
	
	


	
	// ===> Attribute endValidTime
	
	
	

	
 	/**
 	 * Get endValidTime.
 	 * @return endValidTime as ArrayTime
 	 */
 	ArrayTime getEndValidTime() const;
	
 
 	
 	
 	/**
 	 * Set endValidTime with the specified ArrayTime.
 	 * @param endValidTime The ArrayTime value to which endValidTime is to be set.
 	 
 		
 			
 	 */
 	void setEndValidTime (ArrayTime endValidTime);
  		
	
	
	


	
	// ===> Attribute numFreq
	
	
	

	
 	/**
 	 * Get numFreq.
 	 * @return numFreq as int
 	 */
 	int getNumFreq() const;
	
 
 	
 	
 	/**
 	 * Set numFreq with the specified int.
 	 * @param numFreq The int value to which numFreq is to be set.
 	 
 		
 			
 	 */
 	void setNumFreq (int numFreq);
  		
	
	
	


	
	// ===> Attribute numLoad
	
	
	

	
 	/**
 	 * Get numLoad.
 	 * @return numLoad as int
 	 */
 	int getNumLoad() const;
	
 
 	
 	
 	/**
 	 * Set numLoad with the specified int.
 	 * @param numLoad The int value to which numLoad is to be set.
 	 
 		
 			
 	 */
 	void setNumLoad (int numLoad);
  		
	
	
	


	
	// ===> Attribute numReceptor
	
	
	

	
 	/**
 	 * Get numReceptor.
 	 * @return numReceptor as int
 	 */
 	int getNumReceptor() const;
	
 
 	
 	
 	/**
 	 * Set numReceptor with the specified int.
 	 * @param numReceptor The int value to which numReceptor is to be set.
 	 
 		
 			
 	 */
 	void setNumReceptor (int numReceptor);
  		
	
	
	


	
	// ===> Attribute forwardEffSpectrum
	
	
	

	
 	/**
 	 * Get forwardEffSpectrum.
 	 * @return forwardEffSpectrum as std::vector<std::vector<float > >
 	 */
 	std::vector<std::vector<float > > getForwardEffSpectrum() const;
	
 
 	
 	
 	/**
 	 * Set forwardEffSpectrum with the specified std::vector<std::vector<float > >.
 	 * @param forwardEffSpectrum The std::vector<std::vector<float > > value to which forwardEffSpectrum is to be set.
 	 
 		
 			
 	 */
 	void setForwardEffSpectrum (std::vector<std::vector<float > > forwardEffSpectrum);
  		
	
	
	


	
	// ===> Attribute frequencyRange
	
	
	

	
 	/**
 	 * Get frequencyRange.
 	 * @return frequencyRange as std::vector<Frequency >
 	 */
 	std::vector<Frequency > getFrequencyRange() const;
	
 
 	
 	
 	/**
 	 * Set frequencyRange with the specified std::vector<Frequency >.
 	 * @param frequencyRange The std::vector<Frequency > value to which frequencyRange is to be set.
 	 
 		
 			
 	 */
 	void setFrequencyRange (std::vector<Frequency > frequencyRange);
  		
	
	
	


	
	// ===> Attribute groundPressure
	
	
	

	
 	/**
 	 * Get groundPressure.
 	 * @return groundPressure as Pressure
 	 */
 	Pressure getGroundPressure() const;
	
 
 	
 	
 	/**
 	 * Set groundPressure with the specified Pressure.
 	 * @param groundPressure The Pressure value to which groundPressure is to be set.
 	 
 		
 			
 	 */
 	void setGroundPressure (Pressure groundPressure);
  		
	
	
	


	
	// ===> Attribute groundRelHumidity
	
	
	

	
 	/**
 	 * Get groundRelHumidity.
 	 * @return groundRelHumidity as Humidity
 	 */
 	Humidity getGroundRelHumidity() const;
	
 
 	
 	
 	/**
 	 * Set groundRelHumidity with the specified Humidity.
 	 * @param groundRelHumidity The Humidity value to which groundRelHumidity is to be set.
 	 
 		
 			
 	 */
 	void setGroundRelHumidity (Humidity groundRelHumidity);
  		
	
	
	


	
	// ===> Attribute frequencySpectrum
	
	
	

	
 	/**
 	 * Get frequencySpectrum.
 	 * @return frequencySpectrum as std::vector<Frequency >
 	 */
 	std::vector<Frequency > getFrequencySpectrum() const;
	
 
 	
 	
 	/**
 	 * Set frequencySpectrum with the specified std::vector<Frequency >.
 	 * @param frequencySpectrum The std::vector<Frequency > value to which frequencySpectrum is to be set.
 	 
 		
 			
 	 */
 	void setFrequencySpectrum (std::vector<Frequency > frequencySpectrum);
  		
	
	
	


	
	// ===> Attribute groundTemperature
	
	
	

	
 	/**
 	 * Get groundTemperature.
 	 * @return groundTemperature as Temperature
 	 */
 	Temperature getGroundTemperature() const;
	
 
 	
 	
 	/**
 	 * Set groundTemperature with the specified Temperature.
 	 * @param groundTemperature The Temperature value to which groundTemperature is to be set.
 	 
 		
 			
 	 */
 	void setGroundTemperature (Temperature groundTemperature);
  		
	
	
	


	
	// ===> Attribute polarizationTypes
	
	
	

	
 	/**
 	 * Get polarizationTypes.
 	 * @return polarizationTypes as std::vector<PolarizationTypeMod::PolarizationType >
 	 */
 	std::vector<PolarizationTypeMod::PolarizationType > getPolarizationTypes() const;
	
 
 	
 	
 	/**
 	 * Set polarizationTypes with the specified std::vector<PolarizationTypeMod::PolarizationType >.
 	 * @param polarizationTypes The std::vector<PolarizationTypeMod::PolarizationType > value to which polarizationTypes is to be set.
 	 
 		
 			
 	 */
 	void setPolarizationTypes (std::vector<PolarizationTypeMod::PolarizationType > polarizationTypes);
  		
	
	
	


	
	// ===> Attribute powerSkySpectrum
	
	
	

	
 	/**
 	 * Get powerSkySpectrum.
 	 * @return powerSkySpectrum as std::vector<std::vector<float > >
 	 */
 	std::vector<std::vector<float > > getPowerSkySpectrum() const;
	
 
 	
 	
 	/**
 	 * Set powerSkySpectrum with the specified std::vector<std::vector<float > >.
 	 * @param powerSkySpectrum The std::vector<std::vector<float > > value to which powerSkySpectrum is to be set.
 	 
 		
 			
 	 */
 	void setPowerSkySpectrum (std::vector<std::vector<float > > powerSkySpectrum);
  		
	
	
	


	
	// ===> Attribute powerLoadSpectrum
	
	
	

	
 	/**
 	 * Get powerLoadSpectrum.
 	 * @return powerLoadSpectrum as std::vector<std::vector<std::vector<float > > >
 	 */
 	std::vector<std::vector<std::vector<float > > > getPowerLoadSpectrum() const;
	
 
 	
 	
 	/**
 	 * Set powerLoadSpectrum with the specified std::vector<std::vector<std::vector<float > > >.
 	 * @param powerLoadSpectrum The std::vector<std::vector<std::vector<float > > > value to which powerLoadSpectrum is to be set.
 	 
 		
 			
 	 */
 	void setPowerLoadSpectrum (std::vector<std::vector<std::vector<float > > > powerLoadSpectrum);
  		
	
	
	


	
	// ===> Attribute syscalType
	
	
	

	
 	/**
 	 * Get syscalType.
 	 * @return syscalType as SyscalMethodMod::SyscalMethod
 	 */
 	SyscalMethodMod::SyscalMethod getSyscalType() const;
	
 
 	
 	
 	/**
 	 * Set syscalType with the specified SyscalMethodMod::SyscalMethod.
 	 * @param syscalType The SyscalMethodMod::SyscalMethod value to which syscalType is to be set.
 	 
 		
 			
 	 */
 	void setSyscalType (SyscalMethodMod::SyscalMethod syscalType);
  		
	
	
	


	
	// ===> Attribute tAtmSpectrum
	
	
	

	
 	/**
 	 * Get tAtmSpectrum.
 	 * @return tAtmSpectrum as std::vector<std::vector<Temperature > >
 	 */
 	std::vector<std::vector<Temperature > > getTAtmSpectrum() const;
	
 
 	
 	
 	/**
 	 * Set tAtmSpectrum with the specified std::vector<std::vector<Temperature > >.
 	 * @param tAtmSpectrum The std::vector<std::vector<Temperature > > value to which tAtmSpectrum is to be set.
 	 
 		
 			
 	 */
 	void setTAtmSpectrum (std::vector<std::vector<Temperature > > tAtmSpectrum);
  		
	
	
	


	
	// ===> Attribute tRecSpectrum
	
	
	

	
 	/**
 	 * Get tRecSpectrum.
 	 * @return tRecSpectrum as std::vector<std::vector<Temperature > >
 	 */
 	std::vector<std::vector<Temperature > > getTRecSpectrum() const;
	
 
 	
 	
 	/**
 	 * Set tRecSpectrum with the specified std::vector<std::vector<Temperature > >.
 	 * @param tRecSpectrum The std::vector<std::vector<Temperature > > value to which tRecSpectrum is to be set.
 	 
 		
 			
 	 */
 	void setTRecSpectrum (std::vector<std::vector<Temperature > > tRecSpectrum);
  		
	
	
	


	
	// ===> Attribute tSysSpectrum
	
	
	

	
 	/**
 	 * Get tSysSpectrum.
 	 * @return tSysSpectrum as std::vector<std::vector<Temperature > >
 	 */
 	std::vector<std::vector<Temperature > > getTSysSpectrum() const;
	
 
 	
 	
 	/**
 	 * Set tSysSpectrum with the specified std::vector<std::vector<Temperature > >.
 	 * @param tSysSpectrum The std::vector<std::vector<Temperature > > value to which tSysSpectrum is to be set.
 	 
 		
 			
 	 */
 	void setTSysSpectrum (std::vector<std::vector<Temperature > > tSysSpectrum);
  		
	
	
	


	
	// ===> Attribute tauSpectrum
	
	
	

	
 	/**
 	 * Get tauSpectrum.
 	 * @return tauSpectrum as std::vector<std::vector<float > >
 	 */
 	std::vector<std::vector<float > > getTauSpectrum() const;
	
 
 	
 	
 	/**
 	 * Set tauSpectrum with the specified std::vector<std::vector<float > >.
 	 * @param tauSpectrum The std::vector<std::vector<float > > value to which tauSpectrum is to be set.
 	 
 		
 			
 	 */
 	void setTauSpectrum (std::vector<std::vector<float > > tauSpectrum);
  		
	
	
	


	
	// ===> Attribute tAtm
	
	
	

	
 	/**
 	 * Get tAtm.
 	 * @return tAtm as std::vector<Temperature >
 	 */
 	std::vector<Temperature > getTAtm() const;
	
 
 	
 	
 	/**
 	 * Set tAtm with the specified std::vector<Temperature >.
 	 * @param tAtm The std::vector<Temperature > value to which tAtm is to be set.
 	 
 		
 			
 	 */
 	void setTAtm (std::vector<Temperature > tAtm);
  		
	
	
	


	
	// ===> Attribute tRec
	
	
	

	
 	/**
 	 * Get tRec.
 	 * @return tRec as std::vector<Temperature >
 	 */
 	std::vector<Temperature > getTRec() const;
	
 
 	
 	
 	/**
 	 * Set tRec with the specified std::vector<Temperature >.
 	 * @param tRec The std::vector<Temperature > value to which tRec is to be set.
 	 
 		
 			
 	 */
 	void setTRec (std::vector<Temperature > tRec);
  		
	
	
	


	
	// ===> Attribute tSys
	
	
	

	
 	/**
 	 * Get tSys.
 	 * @return tSys as std::vector<Temperature >
 	 */
 	std::vector<Temperature > getTSys() const;
	
 
 	
 	
 	/**
 	 * Set tSys with the specified std::vector<Temperature >.
 	 * @param tSys The std::vector<Temperature > value to which tSys is to be set.
 	 
 		
 			
 	 */
 	void setTSys (std::vector<Temperature > tSys);
  		
	
	
	


	
	// ===> Attribute tau
	
	
	

	
 	/**
 	 * Get tau.
 	 * @return tau as std::vector<float >
 	 */
 	std::vector<float > getTau() const;
	
 
 	
 	
 	/**
 	 * Set tau with the specified std::vector<float >.
 	 * @param tau The std::vector<float > value to which tau is to be set.
 	 
 		
 			
 	 */
 	void setTau (std::vector<float > tau);
  		
	
	
	


	
	// ===> Attribute water
	
	
	

	
 	/**
 	 * Get water.
 	 * @return water as std::vector<Length >
 	 */
 	std::vector<Length > getWater() const;
	
 
 	
 	
 	/**
 	 * Set water with the specified std::vector<Length >.
 	 * @param water The std::vector<Length > value to which water is to be set.
 	 
 		
 			
 	 */
 	void setWater (std::vector<Length > water);
  		
	
	
	


	
	// ===> Attribute waterError
	
	
	

	
 	/**
 	 * Get waterError.
 	 * @return waterError as std::vector<Length >
 	 */
 	std::vector<Length > getWaterError() const;
	
 
 	
 	
 	/**
 	 * Set waterError with the specified std::vector<Length >.
 	 * @param waterError The std::vector<Length > value to which waterError is to be set.
 	 
 		
 			
 	 */
 	void setWaterError (std::vector<Length > waterError);
  		
	
	
	


	
	// ===> Attribute alphaSpectrum, which is optional
	
	
	
	/**
	 * The attribute alphaSpectrum is optional. Return true if this attribute exists.
	 * @return true if and only if the alphaSpectrum attribute exists. 
	 */
	bool isAlphaSpectrumExists() const;
	

	
 	/**
 	 * Get alphaSpectrum, which is optional.
 	 * @return alphaSpectrum as std::vector<std::vector<float > >
 	 * @throws IllegalAccessException If alphaSpectrum does not exist.
 	 */
 	std::vector<std::vector<float > > getAlphaSpectrum() const;
	
 
 	
 	
 	/**
 	 * Set alphaSpectrum with the specified std::vector<std::vector<float > >.
 	 * @param alphaSpectrum The std::vector<std::vector<float > > value to which alphaSpectrum is to be set.
 	 
 		
 	 */
 	void setAlphaSpectrum (std::vector<std::vector<float > > alphaSpectrum);
		
	
	
	
	/**
	 * Mark alphaSpectrum, which is an optional field, as non-existent.
	 */
	void clearAlphaSpectrum ();
	


	
	// ===> Attribute forwardEfficiency, which is optional
	
	
	
	/**
	 * The attribute forwardEfficiency is optional. Return true if this attribute exists.
	 * @return true if and only if the forwardEfficiency attribute exists. 
	 */
	bool isForwardEfficiencyExists() const;
	

	
 	/**
 	 * Get forwardEfficiency, which is optional.
 	 * @return forwardEfficiency as std::vector<float >
 	 * @throws IllegalAccessException If forwardEfficiency does not exist.
 	 */
 	std::vector<float > getForwardEfficiency() const;
	
 
 	
 	
 	/**
 	 * Set forwardEfficiency with the specified std::vector<float >.
 	 * @param forwardEfficiency The std::vector<float > value to which forwardEfficiency is to be set.
 	 
 		
 	 */
 	void setForwardEfficiency (std::vector<float > forwardEfficiency);
		
	
	
	
	/**
	 * Mark forwardEfficiency, which is an optional field, as non-existent.
	 */
	void clearForwardEfficiency ();
	


	
	// ===> Attribute forwardEfficiencyError, which is optional
	
	
	
	/**
	 * The attribute forwardEfficiencyError is optional. Return true if this attribute exists.
	 * @return true if and only if the forwardEfficiencyError attribute exists. 
	 */
	bool isForwardEfficiencyErrorExists() const;
	

	
 	/**
 	 * Get forwardEfficiencyError, which is optional.
 	 * @return forwardEfficiencyError as std::vector<double >
 	 * @throws IllegalAccessException If forwardEfficiencyError does not exist.
 	 */
 	std::vector<double > getForwardEfficiencyError() const;
	
 
 	
 	
 	/**
 	 * Set forwardEfficiencyError with the specified std::vector<double >.
 	 * @param forwardEfficiencyError The std::vector<double > value to which forwardEfficiencyError is to be set.
 	 
 		
 	 */
 	void setForwardEfficiencyError (std::vector<double > forwardEfficiencyError);
		
	
	
	
	/**
	 * Mark forwardEfficiencyError, which is an optional field, as non-existent.
	 */
	void clearForwardEfficiencyError ();
	


	
	// ===> Attribute sbGain, which is optional
	
	
	
	/**
	 * The attribute sbGain is optional. Return true if this attribute exists.
	 * @return true if and only if the sbGain attribute exists. 
	 */
	bool isSbGainExists() const;
	

	
 	/**
 	 * Get sbGain, which is optional.
 	 * @return sbGain as std::vector<float >
 	 * @throws IllegalAccessException If sbGain does not exist.
 	 */
 	std::vector<float > getSbGain() const;
	
 
 	
 	
 	/**
 	 * Set sbGain with the specified std::vector<float >.
 	 * @param sbGain The std::vector<float > value to which sbGain is to be set.
 	 
 		
 	 */
 	void setSbGain (std::vector<float > sbGain);
		
	
	
	
	/**
	 * Mark sbGain, which is an optional field, as non-existent.
	 */
	void clearSbGain ();
	


	
	// ===> Attribute sbGainError, which is optional
	
	
	
	/**
	 * The attribute sbGainError is optional. Return true if this attribute exists.
	 * @return true if and only if the sbGainError attribute exists. 
	 */
	bool isSbGainErrorExists() const;
	

	
 	/**
 	 * Get sbGainError, which is optional.
 	 * @return sbGainError as std::vector<float >
 	 * @throws IllegalAccessException If sbGainError does not exist.
 	 */
 	std::vector<float > getSbGainError() const;
	
 
 	
 	
 	/**
 	 * Set sbGainError with the specified std::vector<float >.
 	 * @param sbGainError The std::vector<float > value to which sbGainError is to be set.
 	 
 		
 	 */
 	void setSbGainError (std::vector<float > sbGainError);
		
	
	
	
	/**
	 * Mark sbGainError, which is an optional field, as non-existent.
	 */
	void clearSbGainError ();
	


	
	// ===> Attribute sbGainSpectrum, which is optional
	
	
	
	/**
	 * The attribute sbGainSpectrum is optional. Return true if this attribute exists.
	 * @return true if and only if the sbGainSpectrum attribute exists. 
	 */
	bool isSbGainSpectrumExists() const;
	

	
 	/**
 	 * Get sbGainSpectrum, which is optional.
 	 * @return sbGainSpectrum as std::vector<std::vector<float > >
 	 * @throws IllegalAccessException If sbGainSpectrum does not exist.
 	 */
 	std::vector<std::vector<float > > getSbGainSpectrum() const;
	
 
 	
 	
 	/**
 	 * Set sbGainSpectrum with the specified std::vector<std::vector<float > >.
 	 * @param sbGainSpectrum The std::vector<std::vector<float > > value to which sbGainSpectrum is to be set.
 	 
 		
 	 */
 	void setSbGainSpectrum (std::vector<std::vector<float > > sbGainSpectrum);
		
	
	
	
	/**
	 * Mark sbGainSpectrum, which is an optional field, as non-existent.
	 */
	void clearSbGainSpectrum ();
	


	////////////////////////////////
	// Extrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute calDataId
	
	
	

	
 	/**
 	 * Get calDataId.
 	 * @return calDataId as Tag
 	 */
 	Tag getCalDataId() const;
	
 
 	
 	
 	/**
 	 * Set calDataId with the specified Tag.
 	 * @param calDataId The Tag value to which calDataId is to be set.
 	 
 		
 			
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 		
 	 */
 	void setCalDataId (Tag calDataId);
  		
	
	
	


	
	// ===> Attribute calReductionId
	
	
	

	
 	/**
 	 * Get calReductionId.
 	 * @return calReductionId as Tag
 	 */
 	Tag getCalReductionId() const;
	
 
 	
 	
 	/**
 	 * Set calReductionId with the specified Tag.
 	 * @param calReductionId The Tag value to which calReductionId is to be set.
 	 
 		
 			
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 		
 	 */
 	void setCalReductionId (Tag calReductionId);
  		
	
	
	


	///////////
	// Links //
	///////////
	
	

	
		
	/**
	 * calReductionId pointer to the row in the CalReduction table having CalReduction.calReductionId == calReductionId
	 * @return a CalReductionRow*
	 * 
	 
	 */
	 CalReductionRow* getCalReductionUsingCalReductionId();
	 

	

	

	
		
	/**
	 * calDataId pointer to the row in the CalData table having CalData.calDataId == calDataId
	 * @return a CalDataRow*
	 * 
	 
	 */
	 CalDataRow* getCalDataUsingCalDataId();
	 

	

	
	
	
	/**
	 * Compare each mandatory attribute except the autoincrementable one of this CalAtmosphereRow with 
	 * the corresponding parameters and return true if there is a match and false otherwise.
	 	
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
	bool compareNoAutoInc(std::string antennaName, ReceiverBandMod::ReceiverBand receiverBand, BasebandNameMod::BasebandName basebandName, Tag calDataId, Tag calReductionId, ArrayTime startValidTime, ArrayTime endValidTime, int numFreq, int numLoad, int numReceptor, std::vector<std::vector<float > > forwardEffSpectrum, std::vector<Frequency > frequencyRange, Pressure groundPressure, Humidity groundRelHumidity, std::vector<Frequency > frequencySpectrum, Temperature groundTemperature, std::vector<PolarizationTypeMod::PolarizationType > polarizationTypes, std::vector<std::vector<float > > powerSkySpectrum, std::vector<std::vector<std::vector<float > > > powerLoadSpectrum, SyscalMethodMod::SyscalMethod syscalType, std::vector<std::vector<Temperature > > tAtmSpectrum, std::vector<std::vector<Temperature > > tRecSpectrum, std::vector<std::vector<Temperature > > tSysSpectrum, std::vector<std::vector<float > > tauSpectrum, std::vector<Temperature > tAtm, std::vector<Temperature > tRec, std::vector<Temperature > tSys, std::vector<float > tau, std::vector<Length > water, std::vector<Length > waterError);
	
	

	
	/**
	 * Compare each mandatory value (i.e. not in the key) attribute  with 
	 * the corresponding parameters and return true if there is a match and false otherwise.
	 	
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
	bool compareRequiredValue(ArrayTime startValidTime, ArrayTime endValidTime, int numFreq, int numLoad, int numReceptor, std::vector<std::vector<float > > forwardEffSpectrum, std::vector<Frequency > frequencyRange, Pressure groundPressure, Humidity groundRelHumidity, std::vector<Frequency > frequencySpectrum, Temperature groundTemperature, std::vector<PolarizationTypeMod::PolarizationType > polarizationTypes, std::vector<std::vector<float > > powerSkySpectrum, std::vector<std::vector<std::vector<float > > > powerLoadSpectrum, SyscalMethodMod::SyscalMethod syscalType, std::vector<std::vector<Temperature > > tAtmSpectrum, std::vector<std::vector<Temperature > > tRecSpectrum, std::vector<std::vector<Temperature > > tSysSpectrum, std::vector<std::vector<float > > tauSpectrum, std::vector<Temperature > tAtm, std::vector<Temperature > tRec, std::vector<Temperature > tSys, std::vector<float > tau, std::vector<Length > water, std::vector<Length > waterError); 
		 
	
	/**
	 * Return true if all required attributes of the value part are equal to their homologues
	 * in x and false otherwise.
	 *
	 * @param x a pointer on the CalAtmosphereRow whose required attributes of the value part 
	 * will be compared with those of this.
	 * @return a boolean.
	 */
	bool equalByRequiredValue(CalAtmosphereRow* x) ;
	
#ifndef WITHOUT_ACS
	/**
	 * Return this row in the form of an IDL struct.
	 * @return The values of this row as a CalAtmosphereRowIDL struct.
	 */
	asdmIDL::CalAtmosphereRowIDL *toIDL() const;
	
	/**
	 * Define the content of a CalAtmosphereRowIDL struct from the values
	 * found in this row.
	 *
	 * @param x a reference to the CalAtmosphereRowIDL struct to be set.
	 *
	 */
	 void toIDL(asdmIDL::CalAtmosphereRowIDL& x) const;
#endif
	
#ifndef WITHOUT_ACS
	/**
	 * Fill the values of this row from the IDL struct CalAtmosphereRowIDL.
	 * @param x The IDL struct containing the values used to fill this row.
	 * @throws ConversionException
	 */
	void setFromIDL (asdmIDL::CalAtmosphereRowIDL x) ;
#endif
	
	/**
	 * Return this row in the form of an XML string.
	 * @return The values of this row as an XML string.
	 */
	std::string toXML() const;

	/**
	 * Fill the values of this row from an XML string 
	 * that was produced by the toXML() method.
	 * @param rowDoc the XML string being used to set the values of this row.
	 * @throws ConversionException
	 */
	void setFromXML (std::string rowDoc) ;

	/// @cond DISPLAY_PRIVATE	
	////////////////////////////////////////////////////////////
	// binary-deserialization material from an EndianIStream  //
	////////////////////////////////////////////////////////////

	std::map<std::string, CalAtmosphereAttributeFromBin> fromBinMethods;
void antennaNameFromBin( EndianIStream& eis);
void receiverBandFromBin( EndianIStream& eis);
void basebandNameFromBin( EndianIStream& eis);
void calDataIdFromBin( EndianIStream& eis);
void calReductionIdFromBin( EndianIStream& eis);
void startValidTimeFromBin( EndianIStream& eis);
void endValidTimeFromBin( EndianIStream& eis);
void numFreqFromBin( EndianIStream& eis);
void numLoadFromBin( EndianIStream& eis);
void numReceptorFromBin( EndianIStream& eis);
void forwardEffSpectrumFromBin( EndianIStream& eis);
void frequencyRangeFromBin( EndianIStream& eis);
void groundPressureFromBin( EndianIStream& eis);
void groundRelHumidityFromBin( EndianIStream& eis);
void frequencySpectrumFromBin( EndianIStream& eis);
void groundTemperatureFromBin( EndianIStream& eis);
void polarizationTypesFromBin( EndianIStream& eis);
void powerSkySpectrumFromBin( EndianIStream& eis);
void powerLoadSpectrumFromBin( EndianIStream& eis);
void syscalTypeFromBin( EndianIStream& eis);
void tAtmSpectrumFromBin( EndianIStream& eis);
void tRecSpectrumFromBin( EndianIStream& eis);
void tSysSpectrumFromBin( EndianIStream& eis);
void tauSpectrumFromBin( EndianIStream& eis);
void tAtmFromBin( EndianIStream& eis);
void tRecFromBin( EndianIStream& eis);
void tSysFromBin( EndianIStream& eis);
void tauFromBin( EndianIStream& eis);
void waterFromBin( EndianIStream& eis);
void waterErrorFromBin( EndianIStream& eis);

void alphaSpectrumFromBin( EndianIStream& eis);
void forwardEfficiencyFromBin( EndianIStream& eis);
void forwardEfficiencyErrorFromBin( EndianIStream& eis);
void sbGainFromBin( EndianIStream& eis);
void sbGainErrorFromBin( EndianIStream& eis);
void sbGainSpectrumFromBin( EndianIStream& eis);


	 /**
	  * Deserialize a stream of bytes read from an EndianIStream to build a PointingRow.
	  * @param eiss the EndianIStream to be read.
	  * @param table the CalAtmosphereTable to which the row built by deserialization will be parented.
	  * @param attributesSeq a vector containing the names of the attributes . The elements order defines the order 
	  * in which the attributes are written in the binary serialization.
	  */
	 static CalAtmosphereRow* fromBin(EndianIStream& eis, CalAtmosphereTable& table, const std::vector<std::string>& attributesSeq);	 
 
 	 /**
 	  * Parses a string t and assign the result of the parsing to the attribute of name attributeName.
 	  *
 	  * @param attributeName the name of the attribute whose value is going to be defined.
 	  * @param t the string to be parsed into a value given to the attribute of name attributeName.
 	  */
 	 void fromText(const std::string& attributeName, const std::string&  t);
     /// @endcond			

private:
	/**
	 * The table to which this row belongs.
	 */
	CalAtmosphereTable &table;
	/**
	 * Whether this row has been added to the table or not.
	 */
	bool hasBeenAdded;

	// This method is used by the Table class when this row is added to the table.
	void isAdded(bool added);


	/**
	 * Create a CalAtmosphereRow.
	 * <p>
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 */ 
	CalAtmosphereRow (CalAtmosphereTable &table);

	/**
	 * Create a CalAtmosphereRow using a copy constructor mechanism.
	 * <p>
	 * Given a CalAtmosphereRow row and a CalAtmosphereTable table, the method creates a new
	 * CalAtmosphereRow owned by table. Each attribute of the created row is a copy (deep)
	 * of the corresponding attribute of row. The method does not add the created
	 * row to its table, its simply parents it to table, a call to the add method
	 * has to be done in order to get the row added (very likely after having modified
	 * some of its attributes).
	 * If row is null then the method returns a row with default values for its attributes. 
	 *
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 * @param row  The row which is to be copied.
	 */
	 CalAtmosphereRow (CalAtmosphereTable &table, CalAtmosphereRow *row);
	 	
	////////////////////////////////
	// Intrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute receiverBand
	
	

	ReceiverBandMod::ReceiverBand receiverBand;

	
	
 	

	
	// ===> Attribute antennaName
	
	

	std::string antennaName;

	
	
 	

	
	// ===> Attribute basebandName
	
	

	BasebandNameMod::BasebandName basebandName;

	
	
 	

	
	// ===> Attribute startValidTime
	
	

	ArrayTime startValidTime;

	
	
 	

	
	// ===> Attribute endValidTime
	
	

	ArrayTime endValidTime;

	
	
 	

	
	// ===> Attribute numFreq
	
	

	int numFreq;

	
	
 	

	
	// ===> Attribute numLoad
	
	

	int numLoad;

	
	
 	

	
	// ===> Attribute numReceptor
	
	

	int numReceptor;

	
	
 	

	
	// ===> Attribute forwardEffSpectrum
	
	

	std::vector<std::vector<float > > forwardEffSpectrum;

	
	
 	

	
	// ===> Attribute frequencyRange
	
	

	std::vector<Frequency > frequencyRange;

	
	
 	

	
	// ===> Attribute groundPressure
	
	

	Pressure groundPressure;

	
	
 	

	
	// ===> Attribute groundRelHumidity
	
	

	Humidity groundRelHumidity;

	
	
 	

	
	// ===> Attribute frequencySpectrum
	
	

	std::vector<Frequency > frequencySpectrum;

	
	
 	

	
	// ===> Attribute groundTemperature
	
	

	Temperature groundTemperature;

	
	
 	

	
	// ===> Attribute polarizationTypes
	
	

	std::vector<PolarizationTypeMod::PolarizationType > polarizationTypes;

	
	
 	

	
	// ===> Attribute powerSkySpectrum
	
	

	std::vector<std::vector<float > > powerSkySpectrum;

	
	
 	

	
	// ===> Attribute powerLoadSpectrum
	
	

	std::vector<std::vector<std::vector<float > > > powerLoadSpectrum;

	
	
 	

	
	// ===> Attribute syscalType
	
	

	SyscalMethodMod::SyscalMethod syscalType;

	
	
 	

	
	// ===> Attribute tAtmSpectrum
	
	

	std::vector<std::vector<Temperature > > tAtmSpectrum;

	
	
 	

	
	// ===> Attribute tRecSpectrum
	
	

	std::vector<std::vector<Temperature > > tRecSpectrum;

	
	
 	

	
	// ===> Attribute tSysSpectrum
	
	

	std::vector<std::vector<Temperature > > tSysSpectrum;

	
	
 	

	
	// ===> Attribute tauSpectrum
	
	

	std::vector<std::vector<float > > tauSpectrum;

	
	
 	

	
	// ===> Attribute tAtm
	
	

	std::vector<Temperature > tAtm;

	
	
 	

	
	// ===> Attribute tRec
	
	

	std::vector<Temperature > tRec;

	
	
 	

	
	// ===> Attribute tSys
	
	

	std::vector<Temperature > tSys;

	
	
 	

	
	// ===> Attribute tau
	
	

	std::vector<float > tau;

	
	
 	

	
	// ===> Attribute water
	
	

	std::vector<Length > water;

	
	
 	

	
	// ===> Attribute waterError
	
	

	std::vector<Length > waterError;

	
	
 	

	
	// ===> Attribute alphaSpectrum, which is optional
	
	
	bool alphaSpectrumExists;
	

	std::vector<std::vector<float > > alphaSpectrum;

	
	
 	

	
	// ===> Attribute forwardEfficiency, which is optional
	
	
	bool forwardEfficiencyExists;
	

	std::vector<float > forwardEfficiency;

	
	
 	

	
	// ===> Attribute forwardEfficiencyError, which is optional
	
	
	bool forwardEfficiencyErrorExists;
	

	std::vector<double > forwardEfficiencyError;

	
	
 	

	
	// ===> Attribute sbGain, which is optional
	
	
	bool sbGainExists;
	

	std::vector<float > sbGain;

	
	
 	

	
	// ===> Attribute sbGainError, which is optional
	
	
	bool sbGainErrorExists;
	

	std::vector<float > sbGainError;

	
	
 	

	
	// ===> Attribute sbGainSpectrum, which is optional
	
	
	bool sbGainSpectrumExists;
	

	std::vector<std::vector<float > > sbGainSpectrum;

	
	
 	

	////////////////////////////////
	// Extrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute calDataId
	
	

	Tag calDataId;

	
	
 	

	
	// ===> Attribute calReductionId
	
	

	Tag calReductionId;

	
	
 	

	///////////
	// Links //
	///////////
	
	
		

	 

	

	
		

	 

	

	
/*
	////////////////////////////////////////////////////////////
	// binary-deserialization material from an EndianIStream  //
	////////////////////////////////////////////////////////////
	std::map<std::string, CalAtmosphereAttributeFromBin> fromBinMethods;
void antennaNameFromBin( EndianIStream& eis);
void receiverBandFromBin( EndianIStream& eis);
void basebandNameFromBin( EndianIStream& eis);
void calDataIdFromBin( EndianIStream& eis);
void calReductionIdFromBin( EndianIStream& eis);
void startValidTimeFromBin( EndianIStream& eis);
void endValidTimeFromBin( EndianIStream& eis);
void numFreqFromBin( EndianIStream& eis);
void numLoadFromBin( EndianIStream& eis);
void numReceptorFromBin( EndianIStream& eis);
void forwardEffSpectrumFromBin( EndianIStream& eis);
void frequencyRangeFromBin( EndianIStream& eis);
void groundPressureFromBin( EndianIStream& eis);
void groundRelHumidityFromBin( EndianIStream& eis);
void frequencySpectrumFromBin( EndianIStream& eis);
void groundTemperatureFromBin( EndianIStream& eis);
void polarizationTypesFromBin( EndianIStream& eis);
void powerSkySpectrumFromBin( EndianIStream& eis);
void powerLoadSpectrumFromBin( EndianIStream& eis);
void syscalTypeFromBin( EndianIStream& eis);
void tAtmSpectrumFromBin( EndianIStream& eis);
void tRecSpectrumFromBin( EndianIStream& eis);
void tSysSpectrumFromBin( EndianIStream& eis);
void tauSpectrumFromBin( EndianIStream& eis);
void tAtmFromBin( EndianIStream& eis);
void tRecFromBin( EndianIStream& eis);
void tSysFromBin( EndianIStream& eis);
void tauFromBin( EndianIStream& eis);
void waterFromBin( EndianIStream& eis);
void waterErrorFromBin( EndianIStream& eis);

void alphaSpectrumFromBin( EndianIStream& eis);
void forwardEfficiencyFromBin( EndianIStream& eis);
void forwardEfficiencyErrorFromBin( EndianIStream& eis);
void sbGainFromBin( EndianIStream& eis);
void sbGainErrorFromBin( EndianIStream& eis);
void sbGainSpectrumFromBin( EndianIStream& eis);

*/
	
	///////////////////////////////////
	// text-deserialization material //
	///////////////////////////////////
	std::map<std::string, CalAtmosphereAttributeFromText> fromTextMethods;
	
void antennaNameFromText (const std::string & s);
	
	
void receiverBandFromText (const std::string & s);
	
	
void basebandNameFromText (const std::string & s);
	
	
void calDataIdFromText (const std::string & s);
	
	
void calReductionIdFromText (const std::string & s);
	
	
void startValidTimeFromText (const std::string & s);
	
	
void endValidTimeFromText (const std::string & s);
	
	
void numFreqFromText (const std::string & s);
	
	
void numLoadFromText (const std::string & s);
	
	
void numReceptorFromText (const std::string & s);
	
	
void forwardEffSpectrumFromText (const std::string & s);
	
	
void frequencyRangeFromText (const std::string & s);
	
	
void groundPressureFromText (const std::string & s);
	
	
void groundRelHumidityFromText (const std::string & s);
	
	
void frequencySpectrumFromText (const std::string & s);
	
	
void groundTemperatureFromText (const std::string & s);
	
	
void polarizationTypesFromText (const std::string & s);
	
	
void powerSkySpectrumFromText (const std::string & s);
	
	
void powerLoadSpectrumFromText (const std::string & s);
	
	
void syscalTypeFromText (const std::string & s);
	
	
void tAtmSpectrumFromText (const std::string & s);
	
	
void tRecSpectrumFromText (const std::string & s);
	
	
void tSysSpectrumFromText (const std::string & s);
	
	
void tauSpectrumFromText (const std::string & s);
	
	
void tAtmFromText (const std::string & s);
	
	
void tRecFromText (const std::string & s);
	
	
void tSysFromText (const std::string & s);
	
	
void tauFromText (const std::string & s);
	
	
void waterFromText (const std::string & s);
	
	
void waterErrorFromText (const std::string & s);
	

	
void alphaSpectrumFromText (const std::string & s);
	
	
void forwardEfficiencyFromText (const std::string & s);
	
	
void forwardEfficiencyErrorFromText (const std::string & s);
	
	
void sbGainFromText (const std::string & s);
	
	
void sbGainErrorFromText (const std::string & s);
	
	
void sbGainSpectrumFromText (const std::string & s);
	
	
	
	/**
	 * Serialize this into a stream of bytes written to an EndianOSStream.
	 * @param eoss the EndianOSStream to be written to
	 */
	 void toBin(EndianOSStream& eoss);
	 	 
	 /**
	  * Deserialize a stream of bytes read from an EndianIStream to build a PointingRow.
	  * @param eiss the EndianIStream to be read.
	  * @param table the CalAtmosphereTable to which the row built by deserialization will be parented.
	  * @param attributesSeq a vector containing the names of the attributes . The elements order defines the order 
	  * in which the attributes are written in the binary serialization.

	 static CalAtmosphereRow* fromBin(EndianIStream& eis, CalAtmosphereTable& table, const std::vector<std::string>& attributesSeq);	 
		*/
};

} // End namespace asdm

#endif /* CalAtmosphere_CLASS */
