
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
 * File CalBandpassRow.h
 */
 
#ifndef CalBandpassRow_CLASS
#define CalBandpassRow_CLASS

#include <vector>
#include <string>
#include <set>

#ifndef WITHOUT_ACS
#include <asdmIDLC.h>
#endif






	 
#include <alma/ASDM/ArrayTime.h>
	

	 
#include <alma/ASDM/Frequency.h>
	

	 
#include <alma/ASDM/Tag.h>
	




	
#include <alma/Enumerations/CBasebandName.h>
	

	
#include <alma/Enumerations/CNetSideband.h>
	

	
#include <alma/Enumerations/CAtmPhaseCorrection.h>
	

	
#include <alma/Enumerations/CCalCurveType.h>
	

	
#include <alma/Enumerations/CReceiverBand.h>
	

	

	

	

	

	

	

	

	

	
#include <alma/Enumerations/CPolarizationType.h>
	

	

	

	

	

	

	

	

	

	

	

	



#include <alma/ASDM/ConversionException.h>
#include <alma/ASDM/NoSuchRow.h>
#include <alma/ASDM/IllegalAccessException.h>

#include <alma/ASDM/RowTransformer.h>
//#include <alma/ASDM/TableStreamReader.h>

/*\file CalBandpass.h
    \brief Generated from model's revision "-1", branch ""
*/

namespace asdm {

//class asdm::CalBandpassTable;


// class asdm::CalReductionRow;
class CalReductionRow;

// class asdm::CalDataRow;
class CalDataRow;
	

class CalBandpassRow;
typedef void (CalBandpassRow::*CalBandpassAttributeFromBin) (EndianIStream& eis);
typedef void (CalBandpassRow::*CalBandpassAttributeFromText) (const std::string& s);

/**
 * The CalBandpassRow class is a row of a CalBandpassTable.
 * 
 * Generated from model's revision "-1", branch ""
 *
 */
class CalBandpassRow {
friend class asdm::CalBandpassTable;
friend class asdm::RowTransformer<CalBandpassRow>;
//friend class asdm::TableStreamReader<CalBandpassTable, CalBandpassRow>;

public:

	virtual ~CalBandpassRow();

	/**
	 * Return the table to which this row belongs.
	 */
	CalBandpassTable &getTable() const;
	
	/**
	 * Has this row been added to its table ?
	 * @return true if and only if it has been added.
	 */
	bool isAdded() const;
		
	////////////////////////////////
	// Intrinsic Table Attributes //
	////////////////////////////////
	
	
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
  		
	
	
	


	
	// ===> Attribute sideband
	
	
	

	
 	/**
 	 * Get sideband.
 	 * @return sideband as NetSidebandMod::NetSideband
 	 */
 	NetSidebandMod::NetSideband getSideband() const;
	
 
 	
 	
 	/**
 	 * Set sideband with the specified NetSidebandMod::NetSideband.
 	 * @param sideband The NetSidebandMod::NetSideband value to which sideband is to be set.
 	 
 		
 			
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 		
 	 */
 	void setSideband (NetSidebandMod::NetSideband sideband);
  		
	
	
	


	
	// ===> Attribute atmPhaseCorrection
	
	
	

	
 	/**
 	 * Get atmPhaseCorrection.
 	 * @return atmPhaseCorrection as AtmPhaseCorrectionMod::AtmPhaseCorrection
 	 */
 	AtmPhaseCorrectionMod::AtmPhaseCorrection getAtmPhaseCorrection() const;
	
 
 	
 	
 	/**
 	 * Set atmPhaseCorrection with the specified AtmPhaseCorrectionMod::AtmPhaseCorrection.
 	 * @param atmPhaseCorrection The AtmPhaseCorrectionMod::AtmPhaseCorrection value to which atmPhaseCorrection is to be set.
 	 
 		
 			
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 		
 	 */
 	void setAtmPhaseCorrection (AtmPhaseCorrectionMod::AtmPhaseCorrection atmPhaseCorrection);
  		
	
	
	


	
	// ===> Attribute typeCurve
	
	
	

	
 	/**
 	 * Get typeCurve.
 	 * @return typeCurve as CalCurveTypeMod::CalCurveType
 	 */
 	CalCurveTypeMod::CalCurveType getTypeCurve() const;
	
 
 	
 	
 	/**
 	 * Set typeCurve with the specified CalCurveTypeMod::CalCurveType.
 	 * @param typeCurve The CalCurveTypeMod::CalCurveType value to which typeCurve is to be set.
 	 
 		
 			
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 		
 	 */
 	void setTypeCurve (CalCurveTypeMod::CalCurveType typeCurve);
  		
	
	
	


	
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
  		
	
	
	


	
	// ===> Attribute numAntenna
	
	
	

	
 	/**
 	 * Get numAntenna.
 	 * @return numAntenna as int
 	 */
 	int getNumAntenna() const;
	
 
 	
 	
 	/**
 	 * Set numAntenna with the specified int.
 	 * @param numAntenna The int value to which numAntenna is to be set.
 	 
 		
 			
 	 */
 	void setNumAntenna (int numAntenna);
  		
	
	
	


	
	// ===> Attribute numPoly
	
	
	

	
 	/**
 	 * Get numPoly.
 	 * @return numPoly as int
 	 */
 	int getNumPoly() const;
	
 
 	
 	
 	/**
 	 * Set numPoly with the specified int.
 	 * @param numPoly The int value to which numPoly is to be set.
 	 
 		
 			
 	 */
 	void setNumPoly (int numPoly);
  		
	
	
	


	
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
  		
	
	
	


	
	// ===> Attribute antennaNames
	
	
	

	
 	/**
 	 * Get antennaNames.
 	 * @return antennaNames as std::vector<std::string >
 	 */
 	std::vector<std::string > getAntennaNames() const;
	
 
 	
 	
 	/**
 	 * Set antennaNames with the specified std::vector<std::string >.
 	 * @param antennaNames The std::vector<std::string > value to which antennaNames is to be set.
 	 
 		
 			
 	 */
 	void setAntennaNames (std::vector<std::string > antennaNames);
  		
	
	
	


	
	// ===> Attribute refAntennaName
	
	
	

	
 	/**
 	 * Get refAntennaName.
 	 * @return refAntennaName as std::string
 	 */
 	std::string getRefAntennaName() const;
	
 
 	
 	
 	/**
 	 * Set refAntennaName with the specified std::string.
 	 * @param refAntennaName The std::string value to which refAntennaName is to be set.
 	 
 		
 			
 	 */
 	void setRefAntennaName (std::string refAntennaName);
  		
	
	
	


	
	// ===> Attribute freqLimits
	
	
	

	
 	/**
 	 * Get freqLimits.
 	 * @return freqLimits as std::vector<Frequency >
 	 */
 	std::vector<Frequency > getFreqLimits() const;
	
 
 	
 	
 	/**
 	 * Set freqLimits with the specified std::vector<Frequency >.
 	 * @param freqLimits The std::vector<Frequency > value to which freqLimits is to be set.
 	 
 		
 			
 	 */
 	void setFreqLimits (std::vector<Frequency > freqLimits);
  		
	
	
	


	
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
  		
	
	
	


	
	// ===> Attribute curve
	
	
	

	
 	/**
 	 * Get curve.
 	 * @return curve as std::vector<std::vector<std::vector<float > > >
 	 */
 	std::vector<std::vector<std::vector<float > > > getCurve() const;
	
 
 	
 	
 	/**
 	 * Set curve with the specified std::vector<std::vector<std::vector<float > > >.
 	 * @param curve The std::vector<std::vector<std::vector<float > > > value to which curve is to be set.
 	 
 		
 			
 	 */
 	void setCurve (std::vector<std::vector<std::vector<float > > > curve);
  		
	
	
	


	
	// ===> Attribute reducedChiSquared
	
	
	

	
 	/**
 	 * Get reducedChiSquared.
 	 * @return reducedChiSquared as std::vector<double >
 	 */
 	std::vector<double > getReducedChiSquared() const;
	
 
 	
 	
 	/**
 	 * Set reducedChiSquared with the specified std::vector<double >.
 	 * @param reducedChiSquared The std::vector<double > value to which reducedChiSquared is to be set.
 	 
 		
 			
 	 */
 	void setReducedChiSquared (std::vector<double > reducedChiSquared);
  		
	
	
	


	
	// ===> Attribute numBaseline, which is optional
	
	
	
	/**
	 * The attribute numBaseline is optional. Return true if this attribute exists.
	 * @return true if and only if the numBaseline attribute exists. 
	 */
	bool isNumBaselineExists() const;
	

	
 	/**
 	 * Get numBaseline, which is optional.
 	 * @return numBaseline as int
 	 * @throws IllegalAccessException If numBaseline does not exist.
 	 */
 	int getNumBaseline() const;
	
 
 	
 	
 	/**
 	 * Set numBaseline with the specified int.
 	 * @param numBaseline The int value to which numBaseline is to be set.
 	 
 		
 	 */
 	void setNumBaseline (int numBaseline);
		
	
	
	
	/**
	 * Mark numBaseline, which is an optional field, as non-existent.
	 */
	void clearNumBaseline ();
	


	
	// ===> Attribute numFreq, which is optional
	
	
	
	/**
	 * The attribute numFreq is optional. Return true if this attribute exists.
	 * @return true if and only if the numFreq attribute exists. 
	 */
	bool isNumFreqExists() const;
	

	
 	/**
 	 * Get numFreq, which is optional.
 	 * @return numFreq as int
 	 * @throws IllegalAccessException If numFreq does not exist.
 	 */
 	int getNumFreq() const;
	
 
 	
 	
 	/**
 	 * Set numFreq with the specified int.
 	 * @param numFreq The int value to which numFreq is to be set.
 	 
 		
 	 */
 	void setNumFreq (int numFreq);
		
	
	
	
	/**
	 * Mark numFreq, which is an optional field, as non-existent.
	 */
	void clearNumFreq ();
	


	
	// ===> Attribute rms, which is optional
	
	
	
	/**
	 * The attribute rms is optional. Return true if this attribute exists.
	 * @return true if and only if the rms attribute exists. 
	 */
	bool isRmsExists() const;
	

	
 	/**
 	 * Get rms, which is optional.
 	 * @return rms as std::vector<std::vector<float > >
 	 * @throws IllegalAccessException If rms does not exist.
 	 */
 	std::vector<std::vector<float > > getRms() const;
	
 
 	
 	
 	/**
 	 * Set rms with the specified std::vector<std::vector<float > >.
 	 * @param rms The std::vector<std::vector<float > > value to which rms is to be set.
 	 
 		
 	 */
 	void setRms (std::vector<std::vector<float > > rms);
		
	
	
	
	/**
	 * Mark rms, which is an optional field, as non-existent.
	 */
	void clearRms ();
	


	
	// ===> Attribute frequencyRange, which is optional
	
	
	
	/**
	 * The attribute frequencyRange is optional. Return true if this attribute exists.
	 * @return true if and only if the frequencyRange attribute exists. 
	 */
	bool isFrequencyRangeExists() const;
	

	
 	/**
 	 * Get frequencyRange, which is optional.
 	 * @return frequencyRange as std::vector<Frequency >
 	 * @throws IllegalAccessException If frequencyRange does not exist.
 	 */
 	std::vector<Frequency > getFrequencyRange() const;
	
 
 	
 	
 	/**
 	 * Set frequencyRange with the specified std::vector<Frequency >.
 	 * @param frequencyRange The std::vector<Frequency > value to which frequencyRange is to be set.
 	 
 		
 	 */
 	void setFrequencyRange (std::vector<Frequency > frequencyRange);
		
	
	
	
	/**
	 * Mark frequencyRange, which is an optional field, as non-existent.
	 */
	void clearFrequencyRange ();
	


	
	// ===> Attribute numSpectralWindow, which is optional
	
	
	
	/**
	 * The attribute numSpectralWindow is optional. Return true if this attribute exists.
	 * @return true if and only if the numSpectralWindow attribute exists. 
	 */
	bool isNumSpectralWindowExists() const;
	

	
 	/**
 	 * Get numSpectralWindow, which is optional.
 	 * @return numSpectralWindow as int
 	 * @throws IllegalAccessException If numSpectralWindow does not exist.
 	 */
 	int getNumSpectralWindow() const;
	
 
 	
 	
 	/**
 	 * Set numSpectralWindow with the specified int.
 	 * @param numSpectralWindow The int value to which numSpectralWindow is to be set.
 	 
 		
 	 */
 	void setNumSpectralWindow (int numSpectralWindow);
		
	
	
	
	/**
	 * Mark numSpectralWindow, which is an optional field, as non-existent.
	 */
	void clearNumSpectralWindow ();
	


	
	// ===> Attribute chanFreqStart, which is optional
	
	
	
	/**
	 * The attribute chanFreqStart is optional. Return true if this attribute exists.
	 * @return true if and only if the chanFreqStart attribute exists. 
	 */
	bool isChanFreqStartExists() const;
	

	
 	/**
 	 * Get chanFreqStart, which is optional.
 	 * @return chanFreqStart as std::vector<Frequency >
 	 * @throws IllegalAccessException If chanFreqStart does not exist.
 	 */
 	std::vector<Frequency > getChanFreqStart() const;
	
 
 	
 	
 	/**
 	 * Set chanFreqStart with the specified std::vector<Frequency >.
 	 * @param chanFreqStart The std::vector<Frequency > value to which chanFreqStart is to be set.
 	 
 		
 	 */
 	void setChanFreqStart (std::vector<Frequency > chanFreqStart);
		
	
	
	
	/**
	 * Mark chanFreqStart, which is an optional field, as non-existent.
	 */
	void clearChanFreqStart ();
	


	
	// ===> Attribute chanFreqStep, which is optional
	
	
	
	/**
	 * The attribute chanFreqStep is optional. Return true if this attribute exists.
	 * @return true if and only if the chanFreqStep attribute exists. 
	 */
	bool isChanFreqStepExists() const;
	

	
 	/**
 	 * Get chanFreqStep, which is optional.
 	 * @return chanFreqStep as std::vector<Frequency >
 	 * @throws IllegalAccessException If chanFreqStep does not exist.
 	 */
 	std::vector<Frequency > getChanFreqStep() const;
	
 
 	
 	
 	/**
 	 * Set chanFreqStep with the specified std::vector<Frequency >.
 	 * @param chanFreqStep The std::vector<Frequency > value to which chanFreqStep is to be set.
 	 
 		
 	 */
 	void setChanFreqStep (std::vector<Frequency > chanFreqStep);
		
	
	
	
	/**
	 * Mark chanFreqStep, which is an optional field, as non-existent.
	 */
	void clearChanFreqStep ();
	


	
	// ===> Attribute numSpectralWindowChan, which is optional
	
	
	
	/**
	 * The attribute numSpectralWindowChan is optional. Return true if this attribute exists.
	 * @return true if and only if the numSpectralWindowChan attribute exists. 
	 */
	bool isNumSpectralWindowChanExists() const;
	

	
 	/**
 	 * Get numSpectralWindowChan, which is optional.
 	 * @return numSpectralWindowChan as std::vector<int >
 	 * @throws IllegalAccessException If numSpectralWindowChan does not exist.
 	 */
 	std::vector<int > getNumSpectralWindowChan() const;
	
 
 	
 	
 	/**
 	 * Set numSpectralWindowChan with the specified std::vector<int >.
 	 * @param numSpectralWindowChan The std::vector<int > value to which numSpectralWindowChan is to be set.
 	 
 		
 	 */
 	void setNumSpectralWindowChan (std::vector<int > numSpectralWindowChan);
		
	
	
	
	/**
	 * Mark numSpectralWindowChan, which is an optional field, as non-existent.
	 */
	void clearNumSpectralWindowChan ();
	


	
	// ===> Attribute spectrum, which is optional
	
	
	
	/**
	 * The attribute spectrum is optional. Return true if this attribute exists.
	 * @return true if and only if the spectrum attribute exists. 
	 */
	bool isSpectrumExists() const;
	

	
 	/**
 	 * Get spectrum, which is optional.
 	 * @return spectrum as std::vector<std::vector<std::vector<float > > >
 	 * @throws IllegalAccessException If spectrum does not exist.
 	 */
 	std::vector<std::vector<std::vector<float > > > getSpectrum() const;
	
 
 	
 	
 	/**
 	 * Set spectrum with the specified std::vector<std::vector<std::vector<float > > >.
 	 * @param spectrum The std::vector<std::vector<std::vector<float > > > value to which spectrum is to be set.
 	 
 		
 	 */
 	void setSpectrum (std::vector<std::vector<std::vector<float > > > spectrum);
		
	
	
	
	/**
	 * Mark spectrum, which is an optional field, as non-existent.
	 */
	void clearSpectrum ();
	


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
	 * Compare each mandatory attribute except the autoincrementable one of this CalBandpassRow with 
	 * the corresponding parameters and return true if there is a match and false otherwise.
	 	
	 * @param basebandName
	    
	 * @param sideband
	    
	 * @param atmPhaseCorrection
	    
	 * @param typeCurve
	    
	 * @param receiverBand
	    
	 * @param calDataId
	    
	 * @param calReductionId
	    
	 * @param startValidTime
	    
	 * @param endValidTime
	    
	 * @param numAntenna
	    
	 * @param numPoly
	    
	 * @param numReceptor
	    
	 * @param antennaNames
	    
	 * @param refAntennaName
	    
	 * @param freqLimits
	    
	 * @param polarizationTypes
	    
	 * @param curve
	    
	 * @param reducedChiSquared
	    
	 */ 
	bool compareNoAutoInc(BasebandNameMod::BasebandName basebandName, NetSidebandMod::NetSideband sideband, AtmPhaseCorrectionMod::AtmPhaseCorrection atmPhaseCorrection, CalCurveTypeMod::CalCurveType typeCurve, ReceiverBandMod::ReceiverBand receiverBand, Tag calDataId, Tag calReductionId, ArrayTime startValidTime, ArrayTime endValidTime, int numAntenna, int numPoly, int numReceptor, std::vector<std::string > antennaNames, std::string refAntennaName, std::vector<Frequency > freqLimits, std::vector<PolarizationTypeMod::PolarizationType > polarizationTypes, std::vector<std::vector<std::vector<float > > > curve, std::vector<double > reducedChiSquared);
	
	

	
	/**
	 * Compare each mandatory value (i.e. not in the key) attribute  with 
	 * the corresponding parameters and return true if there is a match and false otherwise.
	 	
	 * @param startValidTime
	    
	 * @param endValidTime
	    
	 * @param numAntenna
	    
	 * @param numPoly
	    
	 * @param numReceptor
	    
	 * @param antennaNames
	    
	 * @param refAntennaName
	    
	 * @param freqLimits
	    
	 * @param polarizationTypes
	    
	 * @param curve
	    
	 * @param reducedChiSquared
	    
	 */ 
	bool compareRequiredValue(ArrayTime startValidTime, ArrayTime endValidTime, int numAntenna, int numPoly, int numReceptor, std::vector<std::string > antennaNames, std::string refAntennaName, std::vector<Frequency > freqLimits, std::vector<PolarizationTypeMod::PolarizationType > polarizationTypes, std::vector<std::vector<std::vector<float > > > curve, std::vector<double > reducedChiSquared); 
		 
	
	/**
	 * Return true if all required attributes of the value part are equal to their homologues
	 * in x and false otherwise.
	 *
	 * @param x a pointer on the CalBandpassRow whose required attributes of the value part 
	 * will be compared with those of this.
	 * @return a boolean.
	 */
	bool equalByRequiredValue(CalBandpassRow* x) ;
	
#ifndef WITHOUT_ACS
	/**
	 * Return this row in the form of an IDL struct.
	 * @return The values of this row as a CalBandpassRowIDL struct.
	 */
	asdmIDL::CalBandpassRowIDL *toIDL() const;
	
	/**
	 * Define the content of a CalBandpassRowIDL struct from the values
	 * found in this row.
	 *
	 * @param x a reference to the CalBandpassRowIDL struct to be set.
	 *
	 */
	 void toIDL(asdmIDL::CalBandpassRowIDL& x) const;
#endif
	
#ifndef WITHOUT_ACS
	/**
	 * Fill the values of this row from the IDL struct CalBandpassRowIDL.
	 * @param x The IDL struct containing the values used to fill this row.
	 * @throws ConversionException
	 */
	void setFromIDL (asdmIDL::CalBandpassRowIDL x) ;
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

	std::map<std::string, CalBandpassAttributeFromBin> fromBinMethods;
void basebandNameFromBin( EndianIStream& eis);
void sidebandFromBin( EndianIStream& eis);
void atmPhaseCorrectionFromBin( EndianIStream& eis);
void typeCurveFromBin( EndianIStream& eis);
void receiverBandFromBin( EndianIStream& eis);
void calDataIdFromBin( EndianIStream& eis);
void calReductionIdFromBin( EndianIStream& eis);
void startValidTimeFromBin( EndianIStream& eis);
void endValidTimeFromBin( EndianIStream& eis);
void numAntennaFromBin( EndianIStream& eis);
void numPolyFromBin( EndianIStream& eis);
void numReceptorFromBin( EndianIStream& eis);
void antennaNamesFromBin( EndianIStream& eis);
void refAntennaNameFromBin( EndianIStream& eis);
void freqLimitsFromBin( EndianIStream& eis);
void polarizationTypesFromBin( EndianIStream& eis);
void curveFromBin( EndianIStream& eis);
void reducedChiSquaredFromBin( EndianIStream& eis);

void numBaselineFromBin( EndianIStream& eis);
void numFreqFromBin( EndianIStream& eis);
void rmsFromBin( EndianIStream& eis);
void frequencyRangeFromBin( EndianIStream& eis);
void numSpectralWindowFromBin( EndianIStream& eis);
void chanFreqStartFromBin( EndianIStream& eis);
void chanFreqStepFromBin( EndianIStream& eis);
void numSpectralWindowChanFromBin( EndianIStream& eis);
void spectrumFromBin( EndianIStream& eis);


	 /**
	  * Deserialize a stream of bytes read from an EndianIStream to build a PointingRow.
	  * @param eiss the EndianIStream to be read.
	  * @param table the CalBandpassTable to which the row built by deserialization will be parented.
	  * @param attributesSeq a vector containing the names of the attributes . The elements order defines the order 
	  * in which the attributes are written in the binary serialization.
	  */
	 static CalBandpassRow* fromBin(EndianIStream& eis, CalBandpassTable& table, const std::vector<std::string>& attributesSeq);	 
 
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
	CalBandpassTable &table;
	/**
	 * Whether this row has been added to the table or not.
	 */
	bool hasBeenAdded;

	// This method is used by the Table class when this row is added to the table.
	void isAdded(bool added);


	/**
	 * Create a CalBandpassRow.
	 * <p>
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 */ 
	CalBandpassRow (CalBandpassTable &table);

	/**
	 * Create a CalBandpassRow using a copy constructor mechanism.
	 * <p>
	 * Given a CalBandpassRow row and a CalBandpassTable table, the method creates a new
	 * CalBandpassRow owned by table. Each attribute of the created row is a copy (deep)
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
	 CalBandpassRow (CalBandpassTable &table, CalBandpassRow *row);
	 	
	////////////////////////////////
	// Intrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute basebandName
	
	

	BasebandNameMod::BasebandName basebandName;

	
	
 	

	
	// ===> Attribute sideband
	
	

	NetSidebandMod::NetSideband sideband;

	
	
 	

	
	// ===> Attribute atmPhaseCorrection
	
	

	AtmPhaseCorrectionMod::AtmPhaseCorrection atmPhaseCorrection;

	
	
 	

	
	// ===> Attribute typeCurve
	
	

	CalCurveTypeMod::CalCurveType typeCurve;

	
	
 	

	
	// ===> Attribute receiverBand
	
	

	ReceiverBandMod::ReceiverBand receiverBand;

	
	
 	

	
	// ===> Attribute startValidTime
	
	

	ArrayTime startValidTime;

	
	
 	

	
	// ===> Attribute endValidTime
	
	

	ArrayTime endValidTime;

	
	
 	

	
	// ===> Attribute numAntenna
	
	

	int numAntenna;

	
	
 	

	
	// ===> Attribute numPoly
	
	

	int numPoly;

	
	
 	

	
	// ===> Attribute numReceptor
	
	

	int numReceptor;

	
	
 	

	
	// ===> Attribute antennaNames
	
	

	std::vector<std::string > antennaNames;

	
	
 	

	
	// ===> Attribute refAntennaName
	
	

	std::string refAntennaName;

	
	
 	

	
	// ===> Attribute freqLimits
	
	

	std::vector<Frequency > freqLimits;

	
	
 	

	
	// ===> Attribute polarizationTypes
	
	

	std::vector<PolarizationTypeMod::PolarizationType > polarizationTypes;

	
	
 	

	
	// ===> Attribute curve
	
	

	std::vector<std::vector<std::vector<float > > > curve;

	
	
 	

	
	// ===> Attribute reducedChiSquared
	
	

	std::vector<double > reducedChiSquared;

	
	
 	

	
	// ===> Attribute numBaseline, which is optional
	
	
	bool numBaselineExists;
	

	int numBaseline;

	
	
 	

	
	// ===> Attribute numFreq, which is optional
	
	
	bool numFreqExists;
	

	int numFreq;

	
	
 	

	
	// ===> Attribute rms, which is optional
	
	
	bool rmsExists;
	

	std::vector<std::vector<float > > rms;

	
	
 	

	
	// ===> Attribute frequencyRange, which is optional
	
	
	bool frequencyRangeExists;
	

	std::vector<Frequency > frequencyRange;

	
	
 	

	
	// ===> Attribute numSpectralWindow, which is optional
	
	
	bool numSpectralWindowExists;
	

	int numSpectralWindow;

	
	
 	

	
	// ===> Attribute chanFreqStart, which is optional
	
	
	bool chanFreqStartExists;
	

	std::vector<Frequency > chanFreqStart;

	
	
 	

	
	// ===> Attribute chanFreqStep, which is optional
	
	
	bool chanFreqStepExists;
	

	std::vector<Frequency > chanFreqStep;

	
	
 	

	
	// ===> Attribute numSpectralWindowChan, which is optional
	
	
	bool numSpectralWindowChanExists;
	

	std::vector<int > numSpectralWindowChan;

	
	
 	

	
	// ===> Attribute spectrum, which is optional
	
	
	bool spectrumExists;
	

	std::vector<std::vector<std::vector<float > > > spectrum;

	
	
 	

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
	std::map<std::string, CalBandpassAttributeFromBin> fromBinMethods;
void basebandNameFromBin( EndianIStream& eis);
void sidebandFromBin( EndianIStream& eis);
void atmPhaseCorrectionFromBin( EndianIStream& eis);
void typeCurveFromBin( EndianIStream& eis);
void receiverBandFromBin( EndianIStream& eis);
void calDataIdFromBin( EndianIStream& eis);
void calReductionIdFromBin( EndianIStream& eis);
void startValidTimeFromBin( EndianIStream& eis);
void endValidTimeFromBin( EndianIStream& eis);
void numAntennaFromBin( EndianIStream& eis);
void numPolyFromBin( EndianIStream& eis);
void numReceptorFromBin( EndianIStream& eis);
void antennaNamesFromBin( EndianIStream& eis);
void refAntennaNameFromBin( EndianIStream& eis);
void freqLimitsFromBin( EndianIStream& eis);
void polarizationTypesFromBin( EndianIStream& eis);
void curveFromBin( EndianIStream& eis);
void reducedChiSquaredFromBin( EndianIStream& eis);

void numBaselineFromBin( EndianIStream& eis);
void numFreqFromBin( EndianIStream& eis);
void rmsFromBin( EndianIStream& eis);
void frequencyRangeFromBin( EndianIStream& eis);
void numSpectralWindowFromBin( EndianIStream& eis);
void chanFreqStartFromBin( EndianIStream& eis);
void chanFreqStepFromBin( EndianIStream& eis);
void numSpectralWindowChanFromBin( EndianIStream& eis);
void spectrumFromBin( EndianIStream& eis);

*/
	
	///////////////////////////////////
	// text-deserialization material //
	///////////////////////////////////
	std::map<std::string, CalBandpassAttributeFromText> fromTextMethods;
	
void basebandNameFromText (const std::string & s);
	
	
void sidebandFromText (const std::string & s);
	
	
void atmPhaseCorrectionFromText (const std::string & s);
	
	
void typeCurveFromText (const std::string & s);
	
	
void receiverBandFromText (const std::string & s);
	
	
void calDataIdFromText (const std::string & s);
	
	
void calReductionIdFromText (const std::string & s);
	
	
void startValidTimeFromText (const std::string & s);
	
	
void endValidTimeFromText (const std::string & s);
	
	
void numAntennaFromText (const std::string & s);
	
	
void numPolyFromText (const std::string & s);
	
	
void numReceptorFromText (const std::string & s);
	
	
void antennaNamesFromText (const std::string & s);
	
	
void refAntennaNameFromText (const std::string & s);
	
	
void freqLimitsFromText (const std::string & s);
	
	
void polarizationTypesFromText (const std::string & s);
	
	
void curveFromText (const std::string & s);
	
	
void reducedChiSquaredFromText (const std::string & s);
	

	
void numBaselineFromText (const std::string & s);
	
	
void numFreqFromText (const std::string & s);
	
	
void rmsFromText (const std::string & s);
	
	
void frequencyRangeFromText (const std::string & s);
	
	
void numSpectralWindowFromText (const std::string & s);
	
	
void chanFreqStartFromText (const std::string & s);
	
	
void chanFreqStepFromText (const std::string & s);
	
	
void numSpectralWindowChanFromText (const std::string & s);
	
	
void spectrumFromText (const std::string & s);
	
	
	
	/**
	 * Serialize this into a stream of bytes written to an EndianOSStream.
	 * @param eoss the EndianOSStream to be written to
	 */
	 void toBin(EndianOSStream& eoss);
	 	 
	 /**
	  * Deserialize a stream of bytes read from an EndianIStream to build a PointingRow.
	  * @param eiss the EndianIStream to be read.
	  * @param table the CalBandpassTable to which the row built by deserialization will be parented.
	  * @param attributesSeq a vector containing the names of the attributes . The elements order defines the order 
	  * in which the attributes are written in the binary serialization.

	 static CalBandpassRow* fromBin(EndianIStream& eis, CalBandpassTable& table, const std::vector<std::string>& attributesSeq);	 
		*/
};

} // End namespace asdm

#endif /* CalBandpass_CLASS */
