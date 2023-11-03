
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
 * File SourceRow.h
 */
 
#ifndef SourceRow_CLASS
#define SourceRow_CLASS

#include <vector>
#include <string>
#include <set>

#ifndef WITHOUT_ACS
#include <asdmIDLC.h>
#endif






	 
#include <alma/ASDM/Speed.h>
	

	 
#include <alma/ASDM/AngularRate.h>
	

	 
#include <alma/ASDM/ArrayTime.h>
	

	 
#include <alma/ASDM/Flux.h>
	

	 
#include <alma/ASDM/ArrayTimeInterval.h>
	

	 
#include <alma/ASDM/Angle.h>
	

	 
#include <alma/ASDM/Length.h>
	

	 
#include <alma/ASDM/Frequency.h>
	

	 
#include <alma/ASDM/Tag.h>
	




	

	

	

	

	

	

	
#include <alma/Enumerations/CDirectionReferenceCode.h>
	

	

	

	

	

	

	

	

	

	

	

	
#include <alma/Enumerations/CSourceModel.h>
	

	
#include <alma/Enumerations/CFrequencyReferenceCode.h>
	

	

	

	

	

	
#include <alma/Enumerations/CStokesParameter.h>
	

	

	

	

	

	

	

	
#include <alma/Enumerations/CRadialVelocityReferenceCode.h>
	

	

	
#include <alma/Enumerations/CRadialVelocityReferenceCode.h>
	

	
#include <alma/Enumerations/CDopplerReferenceCode.h>
	

	



#include <alma/ASDM/ConversionException.h>
#include <alma/ASDM/NoSuchRow.h>
#include <alma/ASDM/IllegalAccessException.h>

#include <alma/ASDM/RowTransformer.h>
//#include <alma/ASDM/TableStreamReader.h>

/*\file Source.h
    \brief Generated from model's revision "-1", branch ""
*/

namespace asdm {

//class asdm::SourceTable;


// class asdm::SpectralWindowRow;
class SpectralWindowRow;
	

class SourceRow;
typedef void (SourceRow::*SourceAttributeFromBin) (EndianIStream& eis);
typedef void (SourceRow::*SourceAttributeFromText) (const std::string& s);

/**
 * The SourceRow class is a row of a SourceTable.
 * 
 * Generated from model's revision "-1", branch ""
 *
 */
class SourceRow {
friend class asdm::SourceTable;
friend class asdm::RowTransformer<SourceRow>;
//friend class asdm::TableStreamReader<SourceTable, SourceRow>;

public:

	virtual ~SourceRow();

	/**
	 * Return the table to which this row belongs.
	 */
	SourceTable &getTable() const;
	
	/**
	 * Has this row been added to its table ?
	 * @return true if and only if it has been added.
	 */
	bool isAdded() const;
		
	////////////////////////////////
	// Intrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute sourceId
	
	
	

	
 	/**
 	 * Get sourceId.
 	 * @return sourceId as int
 	 */
 	int getSourceId() const;
	
 
 	
 	
	
	


	
	// ===> Attribute timeInterval
	
	
	

	
 	/**
 	 * Get timeInterval.
 	 * @return timeInterval as ArrayTimeInterval
 	 */
 	ArrayTimeInterval getTimeInterval() const;
	
 
 	
 	
 	/**
 	 * Set timeInterval with the specified ArrayTimeInterval.
 	 * @param timeInterval The ArrayTimeInterval value to which timeInterval is to be set.
 	 
 		
 			
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 		
 	 */
 	void setTimeInterval (ArrayTimeInterval timeInterval);
  		
	
	
	


	
	// ===> Attribute code
	
	
	

	
 	/**
 	 * Get code.
 	 * @return code as std::string
 	 */
 	std::string getCode() const;
	
 
 	
 	
 	/**
 	 * Set code with the specified std::string.
 	 * @param code The std::string value to which code is to be set.
 	 
 		
 			
 	 */
 	void setCode (std::string code);
  		
	
	
	


	
	// ===> Attribute direction
	
	
	

	
 	/**
 	 * Get direction.
 	 * @return direction as std::vector<Angle >
 	 */
 	std::vector<Angle > getDirection() const;
	
 
 	
 	
 	/**
 	 * Set direction with the specified std::vector<Angle >.
 	 * @param direction The std::vector<Angle > value to which direction is to be set.
 	 
 		
 			
 	 */
 	void setDirection (std::vector<Angle > direction);
  		
	
	
	


	
	// ===> Attribute properMotion
	
	
	

	
 	/**
 	 * Get properMotion.
 	 * @return properMotion as std::vector<AngularRate >
 	 */
 	std::vector<AngularRate > getProperMotion() const;
	
 
 	
 	
 	/**
 	 * Set properMotion with the specified std::vector<AngularRate >.
 	 * @param properMotion The std::vector<AngularRate > value to which properMotion is to be set.
 	 
 		
 			
 	 */
 	void setProperMotion (std::vector<AngularRate > properMotion);
  		
	
	
	


	
	// ===> Attribute sourceName
	
	
	

	
 	/**
 	 * Get sourceName.
 	 * @return sourceName as std::string
 	 */
 	std::string getSourceName() const;
	
 
 	
 	
 	/**
 	 * Set sourceName with the specified std::string.
 	 * @param sourceName The std::string value to which sourceName is to be set.
 	 
 		
 			
 	 */
 	void setSourceName (std::string sourceName);
  		
	
	
	


	
	// ===> Attribute directionCode, which is optional
	
	
	
	/**
	 * The attribute directionCode is optional. Return true if this attribute exists.
	 * @return true if and only if the directionCode attribute exists. 
	 */
	bool isDirectionCodeExists() const;
	

	
 	/**
 	 * Get directionCode, which is optional.
 	 * @return directionCode as DirectionReferenceCodeMod::DirectionReferenceCode
 	 * @throws IllegalAccessException If directionCode does not exist.
 	 */
 	DirectionReferenceCodeMod::DirectionReferenceCode getDirectionCode() const;
	
 
 	
 	
 	/**
 	 * Set directionCode with the specified DirectionReferenceCodeMod::DirectionReferenceCode.
 	 * @param directionCode The DirectionReferenceCodeMod::DirectionReferenceCode value to which directionCode is to be set.
 	 
 		
 	 */
 	void setDirectionCode (DirectionReferenceCodeMod::DirectionReferenceCode directionCode);
		
	
	
	
	/**
	 * Mark directionCode, which is an optional field, as non-existent.
	 */
	void clearDirectionCode ();
	


	
	// ===> Attribute directionEquinox, which is optional
	
	
	
	/**
	 * The attribute directionEquinox is optional. Return true if this attribute exists.
	 * @return true if and only if the directionEquinox attribute exists. 
	 */
	bool isDirectionEquinoxExists() const;
	

	
 	/**
 	 * Get directionEquinox, which is optional.
 	 * @return directionEquinox as ArrayTime
 	 * @throws IllegalAccessException If directionEquinox does not exist.
 	 */
 	ArrayTime getDirectionEquinox() const;
	
 
 	
 	
 	/**
 	 * Set directionEquinox with the specified ArrayTime.
 	 * @param directionEquinox The ArrayTime value to which directionEquinox is to be set.
 	 
 		
 	 */
 	void setDirectionEquinox (ArrayTime directionEquinox);
		
	
	
	
	/**
	 * Mark directionEquinox, which is an optional field, as non-existent.
	 */
	void clearDirectionEquinox ();
	


	
	// ===> Attribute calibrationGroup, which is optional
	
	
	
	/**
	 * The attribute calibrationGroup is optional. Return true if this attribute exists.
	 * @return true if and only if the calibrationGroup attribute exists. 
	 */
	bool isCalibrationGroupExists() const;
	

	
 	/**
 	 * Get calibrationGroup, which is optional.
 	 * @return calibrationGroup as int
 	 * @throws IllegalAccessException If calibrationGroup does not exist.
 	 */
 	int getCalibrationGroup() const;
	
 
 	
 	
 	/**
 	 * Set calibrationGroup with the specified int.
 	 * @param calibrationGroup The int value to which calibrationGroup is to be set.
 	 
 		
 	 */
 	void setCalibrationGroup (int calibrationGroup);
		
	
	
	
	/**
	 * Mark calibrationGroup, which is an optional field, as non-existent.
	 */
	void clearCalibrationGroup ();
	


	
	// ===> Attribute catalog, which is optional
	
	
	
	/**
	 * The attribute catalog is optional. Return true if this attribute exists.
	 * @return true if and only if the catalog attribute exists. 
	 */
	bool isCatalogExists() const;
	

	
 	/**
 	 * Get catalog, which is optional.
 	 * @return catalog as std::string
 	 * @throws IllegalAccessException If catalog does not exist.
 	 */
 	std::string getCatalog() const;
	
 
 	
 	
 	/**
 	 * Set catalog with the specified std::string.
 	 * @param catalog The std::string value to which catalog is to be set.
 	 
 		
 	 */
 	void setCatalog (std::string catalog);
		
	
	
	
	/**
	 * Mark catalog, which is an optional field, as non-existent.
	 */
	void clearCatalog ();
	


	
	// ===> Attribute deltaVel, which is optional
	
	
	
	/**
	 * The attribute deltaVel is optional. Return true if this attribute exists.
	 * @return true if and only if the deltaVel attribute exists. 
	 */
	bool isDeltaVelExists() const;
	

	
 	/**
 	 * Get deltaVel, which is optional.
 	 * @return deltaVel as Speed
 	 * @throws IllegalAccessException If deltaVel does not exist.
 	 */
 	Speed getDeltaVel() const;
	
 
 	
 	
 	/**
 	 * Set deltaVel with the specified Speed.
 	 * @param deltaVel The Speed value to which deltaVel is to be set.
 	 
 		
 	 */
 	void setDeltaVel (Speed deltaVel);
		
	
	
	
	/**
	 * Mark deltaVel, which is an optional field, as non-existent.
	 */
	void clearDeltaVel ();
	


	
	// ===> Attribute position, which is optional
	
	
	
	/**
	 * The attribute position is optional. Return true if this attribute exists.
	 * @return true if and only if the position attribute exists. 
	 */
	bool isPositionExists() const;
	

	
 	/**
 	 * Get position, which is optional.
 	 * @return position as std::vector<Length >
 	 * @throws IllegalAccessException If position does not exist.
 	 */
 	std::vector<Length > getPosition() const;
	
 
 	
 	
 	/**
 	 * Set position with the specified std::vector<Length >.
 	 * @param position The std::vector<Length > value to which position is to be set.
 	 
 		
 	 */
 	void setPosition (std::vector<Length > position);
		
	
	
	
	/**
	 * Mark position, which is an optional field, as non-existent.
	 */
	void clearPosition ();
	


	
	// ===> Attribute numLines, which is optional
	
	
	
	/**
	 * The attribute numLines is optional. Return true if this attribute exists.
	 * @return true if and only if the numLines attribute exists. 
	 */
	bool isNumLinesExists() const;
	

	
 	/**
 	 * Get numLines, which is optional.
 	 * @return numLines as int
 	 * @throws IllegalAccessException If numLines does not exist.
 	 */
 	int getNumLines() const;
	
 
 	
 	
 	/**
 	 * Set numLines with the specified int.
 	 * @param numLines The int value to which numLines is to be set.
 	 
 		
 	 */
 	void setNumLines (int numLines);
		
	
	
	
	/**
	 * Mark numLines, which is an optional field, as non-existent.
	 */
	void clearNumLines ();
	


	
	// ===> Attribute transition, which is optional
	
	
	
	/**
	 * The attribute transition is optional. Return true if this attribute exists.
	 * @return true if and only if the transition attribute exists. 
	 */
	bool isTransitionExists() const;
	

	
 	/**
 	 * Get transition, which is optional.
 	 * @return transition as std::vector<std::string >
 	 * @throws IllegalAccessException If transition does not exist.
 	 */
 	std::vector<std::string > getTransition() const;
	
 
 	
 	
 	/**
 	 * Set transition with the specified std::vector<std::string >.
 	 * @param transition The std::vector<std::string > value to which transition is to be set.
 	 
 		
 	 */
 	void setTransition (std::vector<std::string > transition);
		
	
	
	
	/**
	 * Mark transition, which is an optional field, as non-existent.
	 */
	void clearTransition ();
	


	
	// ===> Attribute restFrequency, which is optional
	
	
	
	/**
	 * The attribute restFrequency is optional. Return true if this attribute exists.
	 * @return true if and only if the restFrequency attribute exists. 
	 */
	bool isRestFrequencyExists() const;
	

	
 	/**
 	 * Get restFrequency, which is optional.
 	 * @return restFrequency as std::vector<Frequency >
 	 * @throws IllegalAccessException If restFrequency does not exist.
 	 */
 	std::vector<Frequency > getRestFrequency() const;
	
 
 	
 	
 	/**
 	 * Set restFrequency with the specified std::vector<Frequency >.
 	 * @param restFrequency The std::vector<Frequency > value to which restFrequency is to be set.
 	 
 		
 	 */
 	void setRestFrequency (std::vector<Frequency > restFrequency);
		
	
	
	
	/**
	 * Mark restFrequency, which is an optional field, as non-existent.
	 */
	void clearRestFrequency ();
	


	
	// ===> Attribute sysVel, which is optional
	
	
	
	/**
	 * The attribute sysVel is optional. Return true if this attribute exists.
	 * @return true if and only if the sysVel attribute exists. 
	 */
	bool isSysVelExists() const;
	

	
 	/**
 	 * Get sysVel, which is optional.
 	 * @return sysVel as std::vector<Speed >
 	 * @throws IllegalAccessException If sysVel does not exist.
 	 */
 	std::vector<Speed > getSysVel() const;
	
 
 	
 	
 	/**
 	 * Set sysVel with the specified std::vector<Speed >.
 	 * @param sysVel The std::vector<Speed > value to which sysVel is to be set.
 	 
 		
 	 */
 	void setSysVel (std::vector<Speed > sysVel);
		
	
	
	
	/**
	 * Mark sysVel, which is an optional field, as non-existent.
	 */
	void clearSysVel ();
	


	
	// ===> Attribute rangeVel, which is optional
	
	
	
	/**
	 * The attribute rangeVel is optional. Return true if this attribute exists.
	 * @return true if and only if the rangeVel attribute exists. 
	 */
	bool isRangeVelExists() const;
	

	
 	/**
 	 * Get rangeVel, which is optional.
 	 * @return rangeVel as std::vector<Speed >
 	 * @throws IllegalAccessException If rangeVel does not exist.
 	 */
 	std::vector<Speed > getRangeVel() const;
	
 
 	
 	
 	/**
 	 * Set rangeVel with the specified std::vector<Speed >.
 	 * @param rangeVel The std::vector<Speed > value to which rangeVel is to be set.
 	 
 		
 	 */
 	void setRangeVel (std::vector<Speed > rangeVel);
		
	
	
	
	/**
	 * Mark rangeVel, which is an optional field, as non-existent.
	 */
	void clearRangeVel ();
	


	
	// ===> Attribute sourceModel, which is optional
	
	
	
	/**
	 * The attribute sourceModel is optional. Return true if this attribute exists.
	 * @return true if and only if the sourceModel attribute exists. 
	 */
	bool isSourceModelExists() const;
	

	
 	/**
 	 * Get sourceModel, which is optional.
 	 * @return sourceModel as SourceModelMod::SourceModel
 	 * @throws IllegalAccessException If sourceModel does not exist.
 	 */
 	SourceModelMod::SourceModel getSourceModel() const;
	
 
 	
 	
 	/**
 	 * Set sourceModel with the specified SourceModelMod::SourceModel.
 	 * @param sourceModel The SourceModelMod::SourceModel value to which sourceModel is to be set.
 	 
 		
 	 */
 	void setSourceModel (SourceModelMod::SourceModel sourceModel);
		
	
	
	
	/**
	 * Mark sourceModel, which is an optional field, as non-existent.
	 */
	void clearSourceModel ();
	


	
	// ===> Attribute frequencyRefCode, which is optional
	
	
	
	/**
	 * The attribute frequencyRefCode is optional. Return true if this attribute exists.
	 * @return true if and only if the frequencyRefCode attribute exists. 
	 */
	bool isFrequencyRefCodeExists() const;
	

	
 	/**
 	 * Get frequencyRefCode, which is optional.
 	 * @return frequencyRefCode as FrequencyReferenceCodeMod::FrequencyReferenceCode
 	 * @throws IllegalAccessException If frequencyRefCode does not exist.
 	 */
 	FrequencyReferenceCodeMod::FrequencyReferenceCode getFrequencyRefCode() const;
	
 
 	
 	
 	/**
 	 * Set frequencyRefCode with the specified FrequencyReferenceCodeMod::FrequencyReferenceCode.
 	 * @param frequencyRefCode The FrequencyReferenceCodeMod::FrequencyReferenceCode value to which frequencyRefCode is to be set.
 	 
 		
 	 */
 	void setFrequencyRefCode (FrequencyReferenceCodeMod::FrequencyReferenceCode frequencyRefCode);
		
	
	
	
	/**
	 * Mark frequencyRefCode, which is an optional field, as non-existent.
	 */
	void clearFrequencyRefCode ();
	


	
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
	


	
	// ===> Attribute numStokes, which is optional
	
	
	
	/**
	 * The attribute numStokes is optional. Return true if this attribute exists.
	 * @return true if and only if the numStokes attribute exists. 
	 */
	bool isNumStokesExists() const;
	

	
 	/**
 	 * Get numStokes, which is optional.
 	 * @return numStokes as int
 	 * @throws IllegalAccessException If numStokes does not exist.
 	 */
 	int getNumStokes() const;
	
 
 	
 	
 	/**
 	 * Set numStokes with the specified int.
 	 * @param numStokes The int value to which numStokes is to be set.
 	 
 		
 	 */
 	void setNumStokes (int numStokes);
		
	
	
	
	/**
	 * Mark numStokes, which is an optional field, as non-existent.
	 */
	void clearNumStokes ();
	


	
	// ===> Attribute frequency, which is optional
	
	
	
	/**
	 * The attribute frequency is optional. Return true if this attribute exists.
	 * @return true if and only if the frequency attribute exists. 
	 */
	bool isFrequencyExists() const;
	

	
 	/**
 	 * Get frequency, which is optional.
 	 * @return frequency as std::vector<Frequency >
 	 * @throws IllegalAccessException If frequency does not exist.
 	 */
 	std::vector<Frequency > getFrequency() const;
	
 
 	
 	
 	/**
 	 * Set frequency with the specified std::vector<Frequency >.
 	 * @param frequency The std::vector<Frequency > value to which frequency is to be set.
 	 
 		
 	 */
 	void setFrequency (std::vector<Frequency > frequency);
		
	
	
	
	/**
	 * Mark frequency, which is an optional field, as non-existent.
	 */
	void clearFrequency ();
	


	
	// ===> Attribute frequencyInterval, which is optional
	
	
	
	/**
	 * The attribute frequencyInterval is optional. Return true if this attribute exists.
	 * @return true if and only if the frequencyInterval attribute exists. 
	 */
	bool isFrequencyIntervalExists() const;
	

	
 	/**
 	 * Get frequencyInterval, which is optional.
 	 * @return frequencyInterval as std::vector<Frequency >
 	 * @throws IllegalAccessException If frequencyInterval does not exist.
 	 */
 	std::vector<Frequency > getFrequencyInterval() const;
	
 
 	
 	
 	/**
 	 * Set frequencyInterval with the specified std::vector<Frequency >.
 	 * @param frequencyInterval The std::vector<Frequency > value to which frequencyInterval is to be set.
 	 
 		
 	 */
 	void setFrequencyInterval (std::vector<Frequency > frequencyInterval);
		
	
	
	
	/**
	 * Mark frequencyInterval, which is an optional field, as non-existent.
	 */
	void clearFrequencyInterval ();
	


	
	// ===> Attribute stokesParameter, which is optional
	
	
	
	/**
	 * The attribute stokesParameter is optional. Return true if this attribute exists.
	 * @return true if and only if the stokesParameter attribute exists. 
	 */
	bool isStokesParameterExists() const;
	

	
 	/**
 	 * Get stokesParameter, which is optional.
 	 * @return stokesParameter as std::vector<StokesParameterMod::StokesParameter >
 	 * @throws IllegalAccessException If stokesParameter does not exist.
 	 */
 	std::vector<StokesParameterMod::StokesParameter > getStokesParameter() const;
	
 
 	
 	
 	/**
 	 * Set stokesParameter with the specified std::vector<StokesParameterMod::StokesParameter >.
 	 * @param stokesParameter The std::vector<StokesParameterMod::StokesParameter > value to which stokesParameter is to be set.
 	 
 		
 	 */
 	void setStokesParameter (std::vector<StokesParameterMod::StokesParameter > stokesParameter);
		
	
	
	
	/**
	 * Mark stokesParameter, which is an optional field, as non-existent.
	 */
	void clearStokesParameter ();
	


	
	// ===> Attribute flux, which is optional
	
	
	
	/**
	 * The attribute flux is optional. Return true if this attribute exists.
	 * @return true if and only if the flux attribute exists. 
	 */
	bool isFluxExists() const;
	

	
 	/**
 	 * Get flux, which is optional.
 	 * @return flux as std::vector<std::vector<Flux > >
 	 * @throws IllegalAccessException If flux does not exist.
 	 */
 	std::vector<std::vector<Flux > > getFlux() const;
	
 
 	
 	
 	/**
 	 * Set flux with the specified std::vector<std::vector<Flux > >.
 	 * @param flux The std::vector<std::vector<Flux > > value to which flux is to be set.
 	 
 		
 	 */
 	void setFlux (std::vector<std::vector<Flux > > flux);
		
	
	
	
	/**
	 * Mark flux, which is an optional field, as non-existent.
	 */
	void clearFlux ();
	


	
	// ===> Attribute fluxErr, which is optional
	
	
	
	/**
	 * The attribute fluxErr is optional. Return true if this attribute exists.
	 * @return true if and only if the fluxErr attribute exists. 
	 */
	bool isFluxErrExists() const;
	

	
 	/**
 	 * Get fluxErr, which is optional.
 	 * @return fluxErr as std::vector<std::vector<Flux > >
 	 * @throws IllegalAccessException If fluxErr does not exist.
 	 */
 	std::vector<std::vector<Flux > > getFluxErr() const;
	
 
 	
 	
 	/**
 	 * Set fluxErr with the specified std::vector<std::vector<Flux > >.
 	 * @param fluxErr The std::vector<std::vector<Flux > > value to which fluxErr is to be set.
 	 
 		
 	 */
 	void setFluxErr (std::vector<std::vector<Flux > > fluxErr);
		
	
	
	
	/**
	 * Mark fluxErr, which is an optional field, as non-existent.
	 */
	void clearFluxErr ();
	


	
	// ===> Attribute positionAngle, which is optional
	
	
	
	/**
	 * The attribute positionAngle is optional. Return true if this attribute exists.
	 * @return true if and only if the positionAngle attribute exists. 
	 */
	bool isPositionAngleExists() const;
	

	
 	/**
 	 * Get positionAngle, which is optional.
 	 * @return positionAngle as std::vector<Angle >
 	 * @throws IllegalAccessException If positionAngle does not exist.
 	 */
 	std::vector<Angle > getPositionAngle() const;
	
 
 	
 	
 	/**
 	 * Set positionAngle with the specified std::vector<Angle >.
 	 * @param positionAngle The std::vector<Angle > value to which positionAngle is to be set.
 	 
 		
 	 */
 	void setPositionAngle (std::vector<Angle > positionAngle);
		
	
	
	
	/**
	 * Mark positionAngle, which is an optional field, as non-existent.
	 */
	void clearPositionAngle ();
	


	
	// ===> Attribute positionAngleErr, which is optional
	
	
	
	/**
	 * The attribute positionAngleErr is optional. Return true if this attribute exists.
	 * @return true if and only if the positionAngleErr attribute exists. 
	 */
	bool isPositionAngleErrExists() const;
	

	
 	/**
 	 * Get positionAngleErr, which is optional.
 	 * @return positionAngleErr as std::vector<Angle >
 	 * @throws IllegalAccessException If positionAngleErr does not exist.
 	 */
 	std::vector<Angle > getPositionAngleErr() const;
	
 
 	
 	
 	/**
 	 * Set positionAngleErr with the specified std::vector<Angle >.
 	 * @param positionAngleErr The std::vector<Angle > value to which positionAngleErr is to be set.
 	 
 		
 	 */
 	void setPositionAngleErr (std::vector<Angle > positionAngleErr);
		
	
	
	
	/**
	 * Mark positionAngleErr, which is an optional field, as non-existent.
	 */
	void clearPositionAngleErr ();
	


	
	// ===> Attribute size, which is optional
	
	
	
	/**
	 * The attribute size is optional. Return true if this attribute exists.
	 * @return true if and only if the size attribute exists. 
	 */
	bool isSizeExists() const;
	

	
 	/**
 	 * Get size, which is optional.
 	 * @return size as std::vector<std::vector<Angle > >
 	 * @throws IllegalAccessException If size does not exist.
 	 */
 	std::vector<std::vector<Angle > > getSize() const;
	
 
 	
 	
 	/**
 	 * Set size with the specified std::vector<std::vector<Angle > >.
 	 * @param size The std::vector<std::vector<Angle > > value to which size is to be set.
 	 
 		
 	 */
 	void setSize (std::vector<std::vector<Angle > > size);
		
	
	
	
	/**
	 * Mark size, which is an optional field, as non-existent.
	 */
	void clearSize ();
	


	
	// ===> Attribute sizeErr, which is optional
	
	
	
	/**
	 * The attribute sizeErr is optional. Return true if this attribute exists.
	 * @return true if and only if the sizeErr attribute exists. 
	 */
	bool isSizeErrExists() const;
	

	
 	/**
 	 * Get sizeErr, which is optional.
 	 * @return sizeErr as std::vector<std::vector<Angle > >
 	 * @throws IllegalAccessException If sizeErr does not exist.
 	 */
 	std::vector<std::vector<Angle > > getSizeErr() const;
	
 
 	
 	
 	/**
 	 * Set sizeErr with the specified std::vector<std::vector<Angle > >.
 	 * @param sizeErr The std::vector<std::vector<Angle > > value to which sizeErr is to be set.
 	 
 		
 	 */
 	void setSizeErr (std::vector<std::vector<Angle > > sizeErr);
		
	
	
	
	/**
	 * Mark sizeErr, which is an optional field, as non-existent.
	 */
	void clearSizeErr ();
	


	
	// ===> Attribute velRefCode, which is optional
	
	
	
	/**
	 * The attribute velRefCode is optional. Return true if this attribute exists.
	 * @return true if and only if the velRefCode attribute exists. 
	 */
	bool isVelRefCodeExists() const;
	

	
 	/**
 	 * Get velRefCode, which is optional.
 	 * @return velRefCode as RadialVelocityReferenceCodeMod::RadialVelocityReferenceCode
 	 * @throws IllegalAccessException If velRefCode does not exist.
 	 */
 	RadialVelocityReferenceCodeMod::RadialVelocityReferenceCode getVelRefCode() const;
	
 
 	
 	
 	/**
 	 * Set velRefCode with the specified RadialVelocityReferenceCodeMod::RadialVelocityReferenceCode.
 	 * @param velRefCode The RadialVelocityReferenceCodeMod::RadialVelocityReferenceCode value to which velRefCode is to be set.
 	 
 		
 	 */
 	void setVelRefCode (RadialVelocityReferenceCodeMod::RadialVelocityReferenceCode velRefCode);
		
	
	
	
	/**
	 * Mark velRefCode, which is an optional field, as non-existent.
	 */
	void clearVelRefCode ();
	


	
	// ===> Attribute dopplerVelocity, which is optional
	
	
	
	/**
	 * The attribute dopplerVelocity is optional. Return true if this attribute exists.
	 * @return true if and only if the dopplerVelocity attribute exists. 
	 */
	bool isDopplerVelocityExists() const;
	

	
 	/**
 	 * Get dopplerVelocity, which is optional.
 	 * @return dopplerVelocity as std::vector<Speed >
 	 * @throws IllegalAccessException If dopplerVelocity does not exist.
 	 */
 	std::vector<Speed > getDopplerVelocity() const;
	
 
 	
 	
 	/**
 	 * Set dopplerVelocity with the specified std::vector<Speed >.
 	 * @param dopplerVelocity The std::vector<Speed > value to which dopplerVelocity is to be set.
 	 
 		
 	 */
 	void setDopplerVelocity (std::vector<Speed > dopplerVelocity);
		
	
	
	
	/**
	 * Mark dopplerVelocity, which is an optional field, as non-existent.
	 */
	void clearDopplerVelocity ();
	


	
	// ===> Attribute dopplerReferenceSystem, which is optional
	
	
	
	/**
	 * The attribute dopplerReferenceSystem is optional. Return true if this attribute exists.
	 * @return true if and only if the dopplerReferenceSystem attribute exists. 
	 */
	bool isDopplerReferenceSystemExists() const;
	

	
 	/**
 	 * Get dopplerReferenceSystem, which is optional.
 	 * @return dopplerReferenceSystem as RadialVelocityReferenceCodeMod::RadialVelocityReferenceCode
 	 * @throws IllegalAccessException If dopplerReferenceSystem does not exist.
 	 */
 	RadialVelocityReferenceCodeMod::RadialVelocityReferenceCode getDopplerReferenceSystem() const;
	
 
 	
 	
 	/**
 	 * Set dopplerReferenceSystem with the specified RadialVelocityReferenceCodeMod::RadialVelocityReferenceCode.
 	 * @param dopplerReferenceSystem The RadialVelocityReferenceCodeMod::RadialVelocityReferenceCode value to which dopplerReferenceSystem is to be set.
 	 
 		
 	 */
 	void setDopplerReferenceSystem (RadialVelocityReferenceCodeMod::RadialVelocityReferenceCode dopplerReferenceSystem);
		
	
	
	
	/**
	 * Mark dopplerReferenceSystem, which is an optional field, as non-existent.
	 */
	void clearDopplerReferenceSystem ();
	


	
	// ===> Attribute dopplerCalcType, which is optional
	
	
	
	/**
	 * The attribute dopplerCalcType is optional. Return true if this attribute exists.
	 * @return true if and only if the dopplerCalcType attribute exists. 
	 */
	bool isDopplerCalcTypeExists() const;
	

	
 	/**
 	 * Get dopplerCalcType, which is optional.
 	 * @return dopplerCalcType as DopplerReferenceCodeMod::DopplerReferenceCode
 	 * @throws IllegalAccessException If dopplerCalcType does not exist.
 	 */
 	DopplerReferenceCodeMod::DopplerReferenceCode getDopplerCalcType() const;
	
 
 	
 	
 	/**
 	 * Set dopplerCalcType with the specified DopplerReferenceCodeMod::DopplerReferenceCode.
 	 * @param dopplerCalcType The DopplerReferenceCodeMod::DopplerReferenceCode value to which dopplerCalcType is to be set.
 	 
 		
 	 */
 	void setDopplerCalcType (DopplerReferenceCodeMod::DopplerReferenceCode dopplerCalcType);
		
	
	
	
	/**
	 * Mark dopplerCalcType, which is an optional field, as non-existent.
	 */
	void clearDopplerCalcType ();
	


	
	// ===> Attribute parallax, which is optional
	
	
	
	/**
	 * The attribute parallax is optional. Return true if this attribute exists.
	 * @return true if and only if the parallax attribute exists. 
	 */
	bool isParallaxExists() const;
	

	
 	/**
 	 * Get parallax, which is optional.
 	 * @return parallax as std::vector<Angle >
 	 * @throws IllegalAccessException If parallax does not exist.
 	 */
 	std::vector<Angle > getParallax() const;
	
 
 	
 	
 	/**
 	 * Set parallax with the specified std::vector<Angle >.
 	 * @param parallax The std::vector<Angle > value to which parallax is to be set.
 	 
 		
 	 */
 	void setParallax (std::vector<Angle > parallax);
		
	
	
	
	/**
	 * Mark parallax, which is an optional field, as non-existent.
	 */
	void clearParallax ();
	


	////////////////////////////////
	// Extrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute spectralWindowId
	
	
	

	
 	/**
 	 * Get spectralWindowId.
 	 * @return spectralWindowId as Tag
 	 */
 	Tag getSpectralWindowId() const;
	
 
 	
 	
 	/**
 	 * Set spectralWindowId with the specified Tag.
 	 * @param spectralWindowId The Tag value to which spectralWindowId is to be set.
 	 
 		
 			
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 		
 	 */
 	void setSpectralWindowId (Tag spectralWindowId);
  		
	
	
	


	///////////
	// Links //
	///////////
	
	

	
		
	/**
	 * spectralWindowId pointer to the row in the SpectralWindow table having SpectralWindow.spectralWindowId == spectralWindowId
	 * @return a SpectralWindowRow*
	 * 
	 
	 */
	 SpectralWindowRow* getSpectralWindowUsingSpectralWindowId();
	 

	

	
	
	
	/**
	 * Compare each mandatory attribute except the autoincrementable one of this SourceRow with 
	 * the corresponding parameters and return true if there is a match and false otherwise.
	 	
	 * @param timeInterval
	    
	 * @param spectralWindowId
	    
	 * @param code
	    
	 * @param direction
	    
	 * @param properMotion
	    
	 * @param sourceName
	    
	 */ 
	bool compareNoAutoInc(ArrayTimeInterval timeInterval, Tag spectralWindowId, std::string code, std::vector<Angle > direction, std::vector<AngularRate > properMotion, std::string sourceName);
	
	

	
	/**
	 * Compare each mandatory value (i.e. not in the key) attribute  with 
	 * the corresponding parameters and return true if there is a match and false otherwise.
	 	
	 * @param code
	    
	 * @param direction
	    
	 * @param properMotion
	    
	 * @param sourceName
	    
	 */ 
	bool compareRequiredValue(std::string code, std::vector<Angle > direction, std::vector<AngularRate > properMotion, std::string sourceName); 
		 
	
	/**
	 * Return true if all required attributes of the value part are equal to their homologues
	 * in x and false otherwise.
	 *
	 * @param x a pointer on the SourceRow whose required attributes of the value part 
	 * will be compared with those of this.
	 * @return a boolean.
	 */
	bool equalByRequiredValue(SourceRow* x) ;
	
#ifndef WITHOUT_ACS
	/**
	 * Return this row in the form of an IDL struct.
	 * @return The values of this row as a SourceRowIDL struct.
	 */
	asdmIDL::SourceRowIDL *toIDL() const;
	
	/**
	 * Define the content of a SourceRowIDL struct from the values
	 * found in this row.
	 *
	 * @param x a reference to the SourceRowIDL struct to be set.
	 *
	 */
	 void toIDL(asdmIDL::SourceRowIDL& x) const;
#endif
	
#ifndef WITHOUT_ACS
	/**
	 * Fill the values of this row from the IDL struct SourceRowIDL.
	 * @param x The IDL struct containing the values used to fill this row.
	 * @throws ConversionException
	 */
	void setFromIDL (asdmIDL::SourceRowIDL x) ;
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

	std::map<std::string, SourceAttributeFromBin> fromBinMethods;
void sourceIdFromBin( EndianIStream& eis);
void timeIntervalFromBin( EndianIStream& eis);
void spectralWindowIdFromBin( EndianIStream& eis);
void codeFromBin( EndianIStream& eis);
void directionFromBin( EndianIStream& eis);
void properMotionFromBin( EndianIStream& eis);
void sourceNameFromBin( EndianIStream& eis);

void directionCodeFromBin( EndianIStream& eis);
void directionEquinoxFromBin( EndianIStream& eis);
void calibrationGroupFromBin( EndianIStream& eis);
void catalogFromBin( EndianIStream& eis);
void deltaVelFromBin( EndianIStream& eis);
void positionFromBin( EndianIStream& eis);
void numLinesFromBin( EndianIStream& eis);
void transitionFromBin( EndianIStream& eis);
void restFrequencyFromBin( EndianIStream& eis);
void sysVelFromBin( EndianIStream& eis);
void rangeVelFromBin( EndianIStream& eis);
void sourceModelFromBin( EndianIStream& eis);
void frequencyRefCodeFromBin( EndianIStream& eis);
void numFreqFromBin( EndianIStream& eis);
void numStokesFromBin( EndianIStream& eis);
void frequencyFromBin( EndianIStream& eis);
void frequencyIntervalFromBin( EndianIStream& eis);
void stokesParameterFromBin( EndianIStream& eis);
void fluxFromBin( EndianIStream& eis);
void fluxErrFromBin( EndianIStream& eis);
void positionAngleFromBin( EndianIStream& eis);
void positionAngleErrFromBin( EndianIStream& eis);
void sizeFromBin( EndianIStream& eis);
void sizeErrFromBin( EndianIStream& eis);
void velRefCodeFromBin( EndianIStream& eis);
void dopplerVelocityFromBin( EndianIStream& eis);
void dopplerReferenceSystemFromBin( EndianIStream& eis);
void dopplerCalcTypeFromBin( EndianIStream& eis);
void parallaxFromBin( EndianIStream& eis);


	 /**
	  * Deserialize a stream of bytes read from an EndianIStream to build a PointingRow.
	  * @param eiss the EndianIStream to be read.
	  * @param table the SourceTable to which the row built by deserialization will be parented.
	  * @param attributesSeq a vector containing the names of the attributes . The elements order defines the order 
	  * in which the attributes are written in the binary serialization.
	  */
	 static SourceRow* fromBin(EndianIStream& eis, SourceTable& table, const std::vector<std::string>& attributesSeq);	 
 
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
	SourceTable &table;
	/**
	 * Whether this row has been added to the table or not.
	 */
	bool hasBeenAdded;

	// This method is used by the Table class when this row is added to the table.
	void isAdded(bool added);


	/**
	 * Create a SourceRow.
	 * <p>
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 */ 
	SourceRow (SourceTable &table);

	/**
	 * Create a SourceRow using a copy constructor mechanism.
	 * <p>
	 * Given a SourceRow row and a SourceTable table, the method creates a new
	 * SourceRow owned by table. Each attribute of the created row is a copy (deep)
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
	 SourceRow (SourceTable &table, SourceRow *row);
	 	
	////////////////////////////////
	// Intrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute sourceId
	
	

	int sourceId;

	
	
 	
 	/**
 	 * Set sourceId with the specified int value.
 	 * @param sourceId The int value to which sourceId is to be set.
		
 		
			
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 		
 	 */
 	void setSourceId (int sourceId);
  		
	

	
	// ===> Attribute timeInterval
	
	

	ArrayTimeInterval timeInterval;

	
	
 	

	
	// ===> Attribute code
	
	

	std::string code;

	
	
 	

	
	// ===> Attribute direction
	
	

	std::vector<Angle > direction;

	
	
 	

	
	// ===> Attribute properMotion
	
	

	std::vector<AngularRate > properMotion;

	
	
 	

	
	// ===> Attribute sourceName
	
	

	std::string sourceName;

	
	
 	

	
	// ===> Attribute directionCode, which is optional
	
	
	bool directionCodeExists;
	

	DirectionReferenceCodeMod::DirectionReferenceCode directionCode;

	
	
 	

	
	// ===> Attribute directionEquinox, which is optional
	
	
	bool directionEquinoxExists;
	

	ArrayTime directionEquinox;

	
	
 	

	
	// ===> Attribute calibrationGroup, which is optional
	
	
	bool calibrationGroupExists;
	

	int calibrationGroup;

	
	
 	

	
	// ===> Attribute catalog, which is optional
	
	
	bool catalogExists;
	

	std::string catalog;

	
	
 	

	
	// ===> Attribute deltaVel, which is optional
	
	
	bool deltaVelExists;
	

	Speed deltaVel;

	
	
 	

	
	// ===> Attribute position, which is optional
	
	
	bool positionExists;
	

	std::vector<Length > position;

	
	
 	

	
	// ===> Attribute numLines, which is optional
	
	
	bool numLinesExists;
	

	int numLines;

	
	
 	

	
	// ===> Attribute transition, which is optional
	
	
	bool transitionExists;
	

	std::vector<std::string > transition;

	
	
 	

	
	// ===> Attribute restFrequency, which is optional
	
	
	bool restFrequencyExists;
	

	std::vector<Frequency > restFrequency;

	
	
 	

	
	// ===> Attribute sysVel, which is optional
	
	
	bool sysVelExists;
	

	std::vector<Speed > sysVel;

	
	
 	

	
	// ===> Attribute rangeVel, which is optional
	
	
	bool rangeVelExists;
	

	std::vector<Speed > rangeVel;

	
	
 	

	
	// ===> Attribute sourceModel, which is optional
	
	
	bool sourceModelExists;
	

	SourceModelMod::SourceModel sourceModel;

	
	
 	

	
	// ===> Attribute frequencyRefCode, which is optional
	
	
	bool frequencyRefCodeExists;
	

	FrequencyReferenceCodeMod::FrequencyReferenceCode frequencyRefCode;

	
	
 	

	
	// ===> Attribute numFreq, which is optional
	
	
	bool numFreqExists;
	

	int numFreq;

	
	
 	

	
	// ===> Attribute numStokes, which is optional
	
	
	bool numStokesExists;
	

	int numStokes;

	
	
 	

	
	// ===> Attribute frequency, which is optional
	
	
	bool frequencyExists;
	

	std::vector<Frequency > frequency;

	
	
 	

	
	// ===> Attribute frequencyInterval, which is optional
	
	
	bool frequencyIntervalExists;
	

	std::vector<Frequency > frequencyInterval;

	
	
 	

	
	// ===> Attribute stokesParameter, which is optional
	
	
	bool stokesParameterExists;
	

	std::vector<StokesParameterMod::StokesParameter > stokesParameter;

	
	
 	

	
	// ===> Attribute flux, which is optional
	
	
	bool fluxExists;
	

	std::vector<std::vector<Flux > > flux;

	
	
 	

	
	// ===> Attribute fluxErr, which is optional
	
	
	bool fluxErrExists;
	

	std::vector<std::vector<Flux > > fluxErr;

	
	
 	

	
	// ===> Attribute positionAngle, which is optional
	
	
	bool positionAngleExists;
	

	std::vector<Angle > positionAngle;

	
	
 	

	
	// ===> Attribute positionAngleErr, which is optional
	
	
	bool positionAngleErrExists;
	

	std::vector<Angle > positionAngleErr;

	
	
 	

	
	// ===> Attribute size, which is optional
	
	
	bool sizeExists;
	

	std::vector<std::vector<Angle > > size;

	
	
 	

	
	// ===> Attribute sizeErr, which is optional
	
	
	bool sizeErrExists;
	

	std::vector<std::vector<Angle > > sizeErr;

	
	
 	

	
	// ===> Attribute velRefCode, which is optional
	
	
	bool velRefCodeExists;
	

	RadialVelocityReferenceCodeMod::RadialVelocityReferenceCode velRefCode;

	
	
 	

	
	// ===> Attribute dopplerVelocity, which is optional
	
	
	bool dopplerVelocityExists;
	

	std::vector<Speed > dopplerVelocity;

	
	
 	

	
	// ===> Attribute dopplerReferenceSystem, which is optional
	
	
	bool dopplerReferenceSystemExists;
	

	RadialVelocityReferenceCodeMod::RadialVelocityReferenceCode dopplerReferenceSystem;

	
	
 	

	
	// ===> Attribute dopplerCalcType, which is optional
	
	
	bool dopplerCalcTypeExists;
	

	DopplerReferenceCodeMod::DopplerReferenceCode dopplerCalcType;

	
	
 	

	
	// ===> Attribute parallax, which is optional
	
	
	bool parallaxExists;
	

	std::vector<Angle > parallax;

	
	
 	

	////////////////////////////////
	// Extrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute spectralWindowId
	
	

	Tag spectralWindowId;

	
	
 	

	///////////
	// Links //
	///////////
	
	
		

	 

	

	
/*
	////////////////////////////////////////////////////////////
	// binary-deserialization material from an EndianIStream  //
	////////////////////////////////////////////////////////////
	std::map<std::string, SourceAttributeFromBin> fromBinMethods;
void sourceIdFromBin( EndianIStream& eis);
void timeIntervalFromBin( EndianIStream& eis);
void spectralWindowIdFromBin( EndianIStream& eis);
void codeFromBin( EndianIStream& eis);
void directionFromBin( EndianIStream& eis);
void properMotionFromBin( EndianIStream& eis);
void sourceNameFromBin( EndianIStream& eis);

void directionCodeFromBin( EndianIStream& eis);
void directionEquinoxFromBin( EndianIStream& eis);
void calibrationGroupFromBin( EndianIStream& eis);
void catalogFromBin( EndianIStream& eis);
void deltaVelFromBin( EndianIStream& eis);
void positionFromBin( EndianIStream& eis);
void numLinesFromBin( EndianIStream& eis);
void transitionFromBin( EndianIStream& eis);
void restFrequencyFromBin( EndianIStream& eis);
void sysVelFromBin( EndianIStream& eis);
void rangeVelFromBin( EndianIStream& eis);
void sourceModelFromBin( EndianIStream& eis);
void frequencyRefCodeFromBin( EndianIStream& eis);
void numFreqFromBin( EndianIStream& eis);
void numStokesFromBin( EndianIStream& eis);
void frequencyFromBin( EndianIStream& eis);
void frequencyIntervalFromBin( EndianIStream& eis);
void stokesParameterFromBin( EndianIStream& eis);
void fluxFromBin( EndianIStream& eis);
void fluxErrFromBin( EndianIStream& eis);
void positionAngleFromBin( EndianIStream& eis);
void positionAngleErrFromBin( EndianIStream& eis);
void sizeFromBin( EndianIStream& eis);
void sizeErrFromBin( EndianIStream& eis);
void velRefCodeFromBin( EndianIStream& eis);
void dopplerVelocityFromBin( EndianIStream& eis);
void dopplerReferenceSystemFromBin( EndianIStream& eis);
void dopplerCalcTypeFromBin( EndianIStream& eis);
void parallaxFromBin( EndianIStream& eis);

*/
	
	///////////////////////////////////
	// text-deserialization material //
	///////////////////////////////////
	std::map<std::string, SourceAttributeFromText> fromTextMethods;
	
void sourceIdFromText (const std::string & s);
	
	
void timeIntervalFromText (const std::string & s);
	
	
void spectralWindowIdFromText (const std::string & s);
	
	
void codeFromText (const std::string & s);
	
	
void directionFromText (const std::string & s);
	
	
void properMotionFromText (const std::string & s);
	
	
void sourceNameFromText (const std::string & s);
	

	
void directionCodeFromText (const std::string & s);
	
	
void directionEquinoxFromText (const std::string & s);
	
	
void calibrationGroupFromText (const std::string & s);
	
	
void catalogFromText (const std::string & s);
	
	
void deltaVelFromText (const std::string & s);
	
	
void positionFromText (const std::string & s);
	
	
void numLinesFromText (const std::string & s);
	
	
void transitionFromText (const std::string & s);
	
	
void restFrequencyFromText (const std::string & s);
	
	
void sysVelFromText (const std::string & s);
	
	
void rangeVelFromText (const std::string & s);
	
	
void sourceModelFromText (const std::string & s);
	
	
void frequencyRefCodeFromText (const std::string & s);
	
	
void numFreqFromText (const std::string & s);
	
	
void numStokesFromText (const std::string & s);
	
	
void frequencyFromText (const std::string & s);
	
	
void frequencyIntervalFromText (const std::string & s);
	
	
void stokesParameterFromText (const std::string & s);
	
	
void fluxFromText (const std::string & s);
	
	
void fluxErrFromText (const std::string & s);
	
	
void positionAngleFromText (const std::string & s);
	
	
void positionAngleErrFromText (const std::string & s);
	
	
void sizeFromText (const std::string & s);
	
	
void sizeErrFromText (const std::string & s);
	
	
void velRefCodeFromText (const std::string & s);
	
	
void dopplerVelocityFromText (const std::string & s);
	
	
void dopplerReferenceSystemFromText (const std::string & s);
	
	
void dopplerCalcTypeFromText (const std::string & s);
	
	
void parallaxFromText (const std::string & s);
	
	
	
	/**
	 * Serialize this into a stream of bytes written to an EndianOSStream.
	 * @param eoss the EndianOSStream to be written to
	 */
	 void toBin(EndianOSStream& eoss);
	 	 
	 /**
	  * Deserialize a stream of bytes read from an EndianIStream to build a PointingRow.
	  * @param eiss the EndianIStream to be read.
	  * @param table the SourceTable to which the row built by deserialization will be parented.
	  * @param attributesSeq a vector containing the names of the attributes . The elements order defines the order 
	  * in which the attributes are written in the binary serialization.

	 static SourceRow* fromBin(EndianIStream& eis, SourceTable& table, const std::vector<std::string>& attributesSeq);	 
		*/
};

} // End namespace asdm

#endif /* Source_CLASS */
