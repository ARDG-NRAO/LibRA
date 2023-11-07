
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
 * File DelayModelRow.h
 */
 
#ifndef DelayModelRow_CLASS
#define DelayModelRow_CLASS

#include <vector>
#include <string>
#include <set>

#ifndef WITHOUT_ACS
#include <asdmIDLC.h>
#endif






	 
#include <alma/ASDM/ArrayTimeInterval.h>
	

	 
#include <alma/ASDM/ArrayTime.h>
	

	 
#include <alma/ASDM/Frequency.h>
	

	 
#include <alma/ASDM/Tag.h>
	




	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	
#include <alma/Enumerations/CPolarizationType.h>
	

	

	

	

	

	

	



#include <alma/ASDM/ConversionException.h>
#include <alma/ASDM/NoSuchRow.h>
#include <alma/ASDM/IllegalAccessException.h>

#include <alma/ASDM/RowTransformer.h>
//#include <alma/ASDM/TableStreamReader.h>

/*\file DelayModel.h
    \brief Generated from model's revision "-1", branch ""
*/

namespace asdm {

//class asdm::DelayModelTable;


// class asdm::AntennaRow;
class AntennaRow;

// class asdm::SpectralWindowRow;
class SpectralWindowRow;

// class asdm::FieldRow;
class FieldRow;
	

class DelayModelRow;
typedef void (DelayModelRow::*DelayModelAttributeFromBin) (EndianIStream& eis);
typedef void (DelayModelRow::*DelayModelAttributeFromText) (const std::string& s);

/**
 * The DelayModelRow class is a row of a DelayModelTable.
 * 
 * Generated from model's revision "-1", branch ""
 *
 */
class DelayModelRow {
friend class asdm::DelayModelTable;
friend class asdm::RowTransformer<DelayModelRow>;
//friend class asdm::TableStreamReader<DelayModelTable, DelayModelRow>;

public:

	virtual ~DelayModelRow();

	/**
	 * Return the table to which this row belongs.
	 */
	DelayModelTable &getTable() const;
	
	/**
	 * Has this row been added to its table ?
	 * @return true if and only if it has been added.
	 */
	bool isAdded() const;
		
	////////////////////////////////
	// Intrinsic Table Attributes //
	////////////////////////////////
	
	
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
  		
	
	
	


	
	// ===> Attribute phaseDelay
	
	
	

	
 	/**
 	 * Get phaseDelay.
 	 * @return phaseDelay as std::vector<double >
 	 */
 	std::vector<double > getPhaseDelay() const;
	
 
 	
 	
 	/**
 	 * Set phaseDelay with the specified std::vector<double >.
 	 * @param phaseDelay The std::vector<double > value to which phaseDelay is to be set.
 	 
 		
 			
 	 */
 	void setPhaseDelay (std::vector<double > phaseDelay);
  		
	
	
	


	
	// ===> Attribute phaseDelayRate
	
	
	

	
 	/**
 	 * Get phaseDelayRate.
 	 * @return phaseDelayRate as std::vector<double >
 	 */
 	std::vector<double > getPhaseDelayRate() const;
	
 
 	
 	
 	/**
 	 * Set phaseDelayRate with the specified std::vector<double >.
 	 * @param phaseDelayRate The std::vector<double > value to which phaseDelayRate is to be set.
 	 
 		
 			
 	 */
 	void setPhaseDelayRate (std::vector<double > phaseDelayRate);
  		
	
	
	


	
	// ===> Attribute groupDelay
	
	
	

	
 	/**
 	 * Get groupDelay.
 	 * @return groupDelay as std::vector<double >
 	 */
 	std::vector<double > getGroupDelay() const;
	
 
 	
 	
 	/**
 	 * Set groupDelay with the specified std::vector<double >.
 	 * @param groupDelay The std::vector<double > value to which groupDelay is to be set.
 	 
 		
 			
 	 */
 	void setGroupDelay (std::vector<double > groupDelay);
  		
	
	
	


	
	// ===> Attribute groupDelayRate
	
	
	

	
 	/**
 	 * Get groupDelayRate.
 	 * @return groupDelayRate as std::vector<double >
 	 */
 	std::vector<double > getGroupDelayRate() const;
	
 
 	
 	
 	/**
 	 * Set groupDelayRate with the specified std::vector<double >.
 	 * @param groupDelayRate The std::vector<double > value to which groupDelayRate is to be set.
 	 
 		
 			
 	 */
 	void setGroupDelayRate (std::vector<double > groupDelayRate);
  		
	
	
	


	
	// ===> Attribute timeOrigin, which is optional
	
	
	
	/**
	 * The attribute timeOrigin is optional. Return true if this attribute exists.
	 * @return true if and only if the timeOrigin attribute exists. 
	 */
	bool isTimeOriginExists() const;
	

	
 	/**
 	 * Get timeOrigin, which is optional.
 	 * @return timeOrigin as ArrayTime
 	 * @throws IllegalAccessException If timeOrigin does not exist.
 	 */
 	ArrayTime getTimeOrigin() const;
	
 
 	
 	
 	/**
 	 * Set timeOrigin with the specified ArrayTime.
 	 * @param timeOrigin The ArrayTime value to which timeOrigin is to be set.
 	 
 		
 	 */
 	void setTimeOrigin (ArrayTime timeOrigin);
		
	
	
	
	/**
	 * Mark timeOrigin, which is an optional field, as non-existent.
	 */
	void clearTimeOrigin ();
	


	
	// ===> Attribute atmosphericGroupDelay, which is optional
	
	
	
	/**
	 * The attribute atmosphericGroupDelay is optional. Return true if this attribute exists.
	 * @return true if and only if the atmosphericGroupDelay attribute exists. 
	 */
	bool isAtmosphericGroupDelayExists() const;
	

	
 	/**
 	 * Get atmosphericGroupDelay, which is optional.
 	 * @return atmosphericGroupDelay as double
 	 * @throws IllegalAccessException If atmosphericGroupDelay does not exist.
 	 */
 	double getAtmosphericGroupDelay() const;
	
 
 	
 	
 	/**
 	 * Set atmosphericGroupDelay with the specified double.
 	 * @param atmosphericGroupDelay The double value to which atmosphericGroupDelay is to be set.
 	 
 		
 	 */
 	void setAtmosphericGroupDelay (double atmosphericGroupDelay);
		
	
	
	
	/**
	 * Mark atmosphericGroupDelay, which is an optional field, as non-existent.
	 */
	void clearAtmosphericGroupDelay ();
	


	
	// ===> Attribute atmosphericGroupDelayRate, which is optional
	
	
	
	/**
	 * The attribute atmosphericGroupDelayRate is optional. Return true if this attribute exists.
	 * @return true if and only if the atmosphericGroupDelayRate attribute exists. 
	 */
	bool isAtmosphericGroupDelayRateExists() const;
	

	
 	/**
 	 * Get atmosphericGroupDelayRate, which is optional.
 	 * @return atmosphericGroupDelayRate as double
 	 * @throws IllegalAccessException If atmosphericGroupDelayRate does not exist.
 	 */
 	double getAtmosphericGroupDelayRate() const;
	
 
 	
 	
 	/**
 	 * Set atmosphericGroupDelayRate with the specified double.
 	 * @param atmosphericGroupDelayRate The double value to which atmosphericGroupDelayRate is to be set.
 	 
 		
 	 */
 	void setAtmosphericGroupDelayRate (double atmosphericGroupDelayRate);
		
	
	
	
	/**
	 * Mark atmosphericGroupDelayRate, which is an optional field, as non-existent.
	 */
	void clearAtmosphericGroupDelayRate ();
	


	
	// ===> Attribute geometricDelay, which is optional
	
	
	
	/**
	 * The attribute geometricDelay is optional. Return true if this attribute exists.
	 * @return true if and only if the geometricDelay attribute exists. 
	 */
	bool isGeometricDelayExists() const;
	

	
 	/**
 	 * Get geometricDelay, which is optional.
 	 * @return geometricDelay as double
 	 * @throws IllegalAccessException If geometricDelay does not exist.
 	 */
 	double getGeometricDelay() const;
	
 
 	
 	
 	/**
 	 * Set geometricDelay with the specified double.
 	 * @param geometricDelay The double value to which geometricDelay is to be set.
 	 
 		
 	 */
 	void setGeometricDelay (double geometricDelay);
		
	
	
	
	/**
	 * Mark geometricDelay, which is an optional field, as non-existent.
	 */
	void clearGeometricDelay ();
	


	
	// ===> Attribute geometricDelayRate, which is optional
	
	
	
	/**
	 * The attribute geometricDelayRate is optional. Return true if this attribute exists.
	 * @return true if and only if the geometricDelayRate attribute exists. 
	 */
	bool isGeometricDelayRateExists() const;
	

	
 	/**
 	 * Get geometricDelayRate, which is optional.
 	 * @return geometricDelayRate as double
 	 * @throws IllegalAccessException If geometricDelayRate does not exist.
 	 */
 	double getGeometricDelayRate() const;
	
 
 	
 	
 	/**
 	 * Set geometricDelayRate with the specified double.
 	 * @param geometricDelayRate The double value to which geometricDelayRate is to be set.
 	 
 		
 	 */
 	void setGeometricDelayRate (double geometricDelayRate);
		
	
	
	
	/**
	 * Mark geometricDelayRate, which is an optional field, as non-existent.
	 */
	void clearGeometricDelayRate ();
	


	
	// ===> Attribute numLO, which is optional
	
	
	
	/**
	 * The attribute numLO is optional. Return true if this attribute exists.
	 * @return true if and only if the numLO attribute exists. 
	 */
	bool isNumLOExists() const;
	

	
 	/**
 	 * Get numLO, which is optional.
 	 * @return numLO as int
 	 * @throws IllegalAccessException If numLO does not exist.
 	 */
 	int getNumLO() const;
	
 
 	
 	
 	/**
 	 * Set numLO with the specified int.
 	 * @param numLO The int value to which numLO is to be set.
 	 
 		
 	 */
 	void setNumLO (int numLO);
		
	
	
	
	/**
	 * Mark numLO, which is an optional field, as non-existent.
	 */
	void clearNumLO ();
	


	
	// ===> Attribute LOOffset, which is optional
	
	
	
	/**
	 * The attribute LOOffset is optional. Return true if this attribute exists.
	 * @return true if and only if the LOOffset attribute exists. 
	 */
	bool isLOOffsetExists() const;
	

	
 	/**
 	 * Get LOOffset, which is optional.
 	 * @return LOOffset as std::vector<Frequency >
 	 * @throws IllegalAccessException If LOOffset does not exist.
 	 */
 	std::vector<Frequency > getLOOffset() const;
	
 
 	
 	
 	/**
 	 * Set LOOffset with the specified std::vector<Frequency >.
 	 * @param LOOffset The std::vector<Frequency > value to which LOOffset is to be set.
 	 
 		
 	 */
 	void setLOOffset (std::vector<Frequency > LOOffset);
		
	
	
	
	/**
	 * Mark LOOffset, which is an optional field, as non-existent.
	 */
	void clearLOOffset ();
	


	
	// ===> Attribute LOOffsetRate, which is optional
	
	
	
	/**
	 * The attribute LOOffsetRate is optional. Return true if this attribute exists.
	 * @return true if and only if the LOOffsetRate attribute exists. 
	 */
	bool isLOOffsetRateExists() const;
	

	
 	/**
 	 * Get LOOffsetRate, which is optional.
 	 * @return LOOffsetRate as std::vector<Frequency >
 	 * @throws IllegalAccessException If LOOffsetRate does not exist.
 	 */
 	std::vector<Frequency > getLOOffsetRate() const;
	
 
 	
 	
 	/**
 	 * Set LOOffsetRate with the specified std::vector<Frequency >.
 	 * @param LOOffsetRate The std::vector<Frequency > value to which LOOffsetRate is to be set.
 	 
 		
 	 */
 	void setLOOffsetRate (std::vector<Frequency > LOOffsetRate);
		
	
	
	
	/**
	 * Mark LOOffsetRate, which is an optional field, as non-existent.
	 */
	void clearLOOffsetRate ();
	


	
	// ===> Attribute dispersiveDelay, which is optional
	
	
	
	/**
	 * The attribute dispersiveDelay is optional. Return true if this attribute exists.
	 * @return true if and only if the dispersiveDelay attribute exists. 
	 */
	bool isDispersiveDelayExists() const;
	

	
 	/**
 	 * Get dispersiveDelay, which is optional.
 	 * @return dispersiveDelay as double
 	 * @throws IllegalAccessException If dispersiveDelay does not exist.
 	 */
 	double getDispersiveDelay() const;
	
 
 	
 	
 	/**
 	 * Set dispersiveDelay with the specified double.
 	 * @param dispersiveDelay The double value to which dispersiveDelay is to be set.
 	 
 		
 	 */
 	void setDispersiveDelay (double dispersiveDelay);
		
	
	
	
	/**
	 * Mark dispersiveDelay, which is an optional field, as non-existent.
	 */
	void clearDispersiveDelay ();
	


	
	// ===> Attribute dispersiveDelayRate, which is optional
	
	
	
	/**
	 * The attribute dispersiveDelayRate is optional. Return true if this attribute exists.
	 * @return true if and only if the dispersiveDelayRate attribute exists. 
	 */
	bool isDispersiveDelayRateExists() const;
	

	
 	/**
 	 * Get dispersiveDelayRate, which is optional.
 	 * @return dispersiveDelayRate as double
 	 * @throws IllegalAccessException If dispersiveDelayRate does not exist.
 	 */
 	double getDispersiveDelayRate() const;
	
 
 	
 	
 	/**
 	 * Set dispersiveDelayRate with the specified double.
 	 * @param dispersiveDelayRate The double value to which dispersiveDelayRate is to be set.
 	 
 		
 	 */
 	void setDispersiveDelayRate (double dispersiveDelayRate);
		
	
	
	
	/**
	 * Mark dispersiveDelayRate, which is an optional field, as non-existent.
	 */
	void clearDispersiveDelayRate ();
	


	
	// ===> Attribute atmosphericDryDelay, which is optional
	
	
	
	/**
	 * The attribute atmosphericDryDelay is optional. Return true if this attribute exists.
	 * @return true if and only if the atmosphericDryDelay attribute exists. 
	 */
	bool isAtmosphericDryDelayExists() const;
	

	
 	/**
 	 * Get atmosphericDryDelay, which is optional.
 	 * @return atmosphericDryDelay as double
 	 * @throws IllegalAccessException If atmosphericDryDelay does not exist.
 	 */
 	double getAtmosphericDryDelay() const;
	
 
 	
 	
 	/**
 	 * Set atmosphericDryDelay with the specified double.
 	 * @param atmosphericDryDelay The double value to which atmosphericDryDelay is to be set.
 	 
 		
 	 */
 	void setAtmosphericDryDelay (double atmosphericDryDelay);
		
	
	
	
	/**
	 * Mark atmosphericDryDelay, which is an optional field, as non-existent.
	 */
	void clearAtmosphericDryDelay ();
	


	
	// ===> Attribute atmosphericWetDelay, which is optional
	
	
	
	/**
	 * The attribute atmosphericWetDelay is optional. Return true if this attribute exists.
	 * @return true if and only if the atmosphericWetDelay attribute exists. 
	 */
	bool isAtmosphericWetDelayExists() const;
	

	
 	/**
 	 * Get atmosphericWetDelay, which is optional.
 	 * @return atmosphericWetDelay as double
 	 * @throws IllegalAccessException If atmosphericWetDelay does not exist.
 	 */
 	double getAtmosphericWetDelay() const;
	
 
 	
 	
 	/**
 	 * Set atmosphericWetDelay with the specified double.
 	 * @param atmosphericWetDelay The double value to which atmosphericWetDelay is to be set.
 	 
 		
 	 */
 	void setAtmosphericWetDelay (double atmosphericWetDelay);
		
	
	
	
	/**
	 * Mark atmosphericWetDelay, which is an optional field, as non-existent.
	 */
	void clearAtmosphericWetDelay ();
	


	
	// ===> Attribute padDelay, which is optional
	
	
	
	/**
	 * The attribute padDelay is optional. Return true if this attribute exists.
	 * @return true if and only if the padDelay attribute exists. 
	 */
	bool isPadDelayExists() const;
	

	
 	/**
 	 * Get padDelay, which is optional.
 	 * @return padDelay as double
 	 * @throws IllegalAccessException If padDelay does not exist.
 	 */
 	double getPadDelay() const;
	
 
 	
 	
 	/**
 	 * Set padDelay with the specified double.
 	 * @param padDelay The double value to which padDelay is to be set.
 	 
 		
 	 */
 	void setPadDelay (double padDelay);
		
	
	
	
	/**
	 * Mark padDelay, which is an optional field, as non-existent.
	 */
	void clearPadDelay ();
	


	
	// ===> Attribute antennaDelay, which is optional
	
	
	
	/**
	 * The attribute antennaDelay is optional. Return true if this attribute exists.
	 * @return true if and only if the antennaDelay attribute exists. 
	 */
	bool isAntennaDelayExists() const;
	

	
 	/**
 	 * Get antennaDelay, which is optional.
 	 * @return antennaDelay as double
 	 * @throws IllegalAccessException If antennaDelay does not exist.
 	 */
 	double getAntennaDelay() const;
	
 
 	
 	
 	/**
 	 * Set antennaDelay with the specified double.
 	 * @param antennaDelay The double value to which antennaDelay is to be set.
 	 
 		
 	 */
 	void setAntennaDelay (double antennaDelay);
		
	
	
	
	/**
	 * Mark antennaDelay, which is an optional field, as non-existent.
	 */
	void clearAntennaDelay ();
	


	
	// ===> Attribute numReceptor, which is optional
	
	
	
	/**
	 * The attribute numReceptor is optional. Return true if this attribute exists.
	 * @return true if and only if the numReceptor attribute exists. 
	 */
	bool isNumReceptorExists() const;
	

	
 	/**
 	 * Get numReceptor, which is optional.
 	 * @return numReceptor as int
 	 * @throws IllegalAccessException If numReceptor does not exist.
 	 */
 	int getNumReceptor() const;
	
 
 	
 	
 	/**
 	 * Set numReceptor with the specified int.
 	 * @param numReceptor The int value to which numReceptor is to be set.
 	 
 		
 	 */
 	void setNumReceptor (int numReceptor);
		
	
	
	
	/**
	 * Mark numReceptor, which is an optional field, as non-existent.
	 */
	void clearNumReceptor ();
	


	
	// ===> Attribute polarizationType, which is optional
	
	
	
	/**
	 * The attribute polarizationType is optional. Return true if this attribute exists.
	 * @return true if and only if the polarizationType attribute exists. 
	 */
	bool isPolarizationTypeExists() const;
	

	
 	/**
 	 * Get polarizationType, which is optional.
 	 * @return polarizationType as std::vector<PolarizationTypeMod::PolarizationType >
 	 * @throws IllegalAccessException If polarizationType does not exist.
 	 */
 	std::vector<PolarizationTypeMod::PolarizationType > getPolarizationType() const;
	
 
 	
 	
 	/**
 	 * Set polarizationType with the specified std::vector<PolarizationTypeMod::PolarizationType >.
 	 * @param polarizationType The std::vector<PolarizationTypeMod::PolarizationType > value to which polarizationType is to be set.
 	 
 		
 	 */
 	void setPolarizationType (std::vector<PolarizationTypeMod::PolarizationType > polarizationType);
		
	
	
	
	/**
	 * Mark polarizationType, which is an optional field, as non-existent.
	 */
	void clearPolarizationType ();
	


	
	// ===> Attribute electronicDelay, which is optional
	
	
	
	/**
	 * The attribute electronicDelay is optional. Return true if this attribute exists.
	 * @return true if and only if the electronicDelay attribute exists. 
	 */
	bool isElectronicDelayExists() const;
	

	
 	/**
 	 * Get electronicDelay, which is optional.
 	 * @return electronicDelay as std::vector<double >
 	 * @throws IllegalAccessException If electronicDelay does not exist.
 	 */
 	std::vector<double > getElectronicDelay() const;
	
 
 	
 	
 	/**
 	 * Set electronicDelay with the specified std::vector<double >.
 	 * @param electronicDelay The std::vector<double > value to which electronicDelay is to be set.
 	 
 		
 	 */
 	void setElectronicDelay (std::vector<double > electronicDelay);
		
	
	
	
	/**
	 * Mark electronicDelay, which is an optional field, as non-existent.
	 */
	void clearElectronicDelay ();
	


	
	// ===> Attribute electronicDelayRate, which is optional
	
	
	
	/**
	 * The attribute electronicDelayRate is optional. Return true if this attribute exists.
	 * @return true if and only if the electronicDelayRate attribute exists. 
	 */
	bool isElectronicDelayRateExists() const;
	

	
 	/**
 	 * Get electronicDelayRate, which is optional.
 	 * @return electronicDelayRate as std::vector<double >
 	 * @throws IllegalAccessException If electronicDelayRate does not exist.
 	 */
 	std::vector<double > getElectronicDelayRate() const;
	
 
 	
 	
 	/**
 	 * Set electronicDelayRate with the specified std::vector<double >.
 	 * @param electronicDelayRate The std::vector<double > value to which electronicDelayRate is to be set.
 	 
 		
 	 */
 	void setElectronicDelayRate (std::vector<double > electronicDelayRate);
		
	
	
	
	/**
	 * Mark electronicDelayRate, which is an optional field, as non-existent.
	 */
	void clearElectronicDelayRate ();
	


	
	// ===> Attribute receiverDelay, which is optional
	
	
	
	/**
	 * The attribute receiverDelay is optional. Return true if this attribute exists.
	 * @return true if and only if the receiverDelay attribute exists. 
	 */
	bool isReceiverDelayExists() const;
	

	
 	/**
 	 * Get receiverDelay, which is optional.
 	 * @return receiverDelay as std::vector<double >
 	 * @throws IllegalAccessException If receiverDelay does not exist.
 	 */
 	std::vector<double > getReceiverDelay() const;
	
 
 	
 	
 	/**
 	 * Set receiverDelay with the specified std::vector<double >.
 	 * @param receiverDelay The std::vector<double > value to which receiverDelay is to be set.
 	 
 		
 	 */
 	void setReceiverDelay (std::vector<double > receiverDelay);
		
	
	
	
	/**
	 * Mark receiverDelay, which is an optional field, as non-existent.
	 */
	void clearReceiverDelay ();
	


	
	// ===> Attribute IFDelay, which is optional
	
	
	
	/**
	 * The attribute IFDelay is optional. Return true if this attribute exists.
	 * @return true if and only if the IFDelay attribute exists. 
	 */
	bool isIFDelayExists() const;
	

	
 	/**
 	 * Get IFDelay, which is optional.
 	 * @return IFDelay as std::vector<double >
 	 * @throws IllegalAccessException If IFDelay does not exist.
 	 */
 	std::vector<double > getIFDelay() const;
	
 
 	
 	
 	/**
 	 * Set IFDelay with the specified std::vector<double >.
 	 * @param IFDelay The std::vector<double > value to which IFDelay is to be set.
 	 
 		
 	 */
 	void setIFDelay (std::vector<double > IFDelay);
		
	
	
	
	/**
	 * Mark IFDelay, which is an optional field, as non-existent.
	 */
	void clearIFDelay ();
	


	
	// ===> Attribute LODelay, which is optional
	
	
	
	/**
	 * The attribute LODelay is optional. Return true if this attribute exists.
	 * @return true if and only if the LODelay attribute exists. 
	 */
	bool isLODelayExists() const;
	

	
 	/**
 	 * Get LODelay, which is optional.
 	 * @return LODelay as std::vector<double >
 	 * @throws IllegalAccessException If LODelay does not exist.
 	 */
 	std::vector<double > getLODelay() const;
	
 
 	
 	
 	/**
 	 * Set LODelay with the specified std::vector<double >.
 	 * @param LODelay The std::vector<double > value to which LODelay is to be set.
 	 
 		
 	 */
 	void setLODelay (std::vector<double > LODelay);
		
	
	
	
	/**
	 * Mark LODelay, which is an optional field, as non-existent.
	 */
	void clearLODelay ();
	


	
	// ===> Attribute crossPolarizationDelay, which is optional
	
	
	
	/**
	 * The attribute crossPolarizationDelay is optional. Return true if this attribute exists.
	 * @return true if and only if the crossPolarizationDelay attribute exists. 
	 */
	bool isCrossPolarizationDelayExists() const;
	

	
 	/**
 	 * Get crossPolarizationDelay, which is optional.
 	 * @return crossPolarizationDelay as double
 	 * @throws IllegalAccessException If crossPolarizationDelay does not exist.
 	 */
 	double getCrossPolarizationDelay() const;
	
 
 	
 	
 	/**
 	 * Set crossPolarizationDelay with the specified double.
 	 * @param crossPolarizationDelay The double value to which crossPolarizationDelay is to be set.
 	 
 		
 	 */
 	void setCrossPolarizationDelay (double crossPolarizationDelay);
		
	
	
	
	/**
	 * Mark crossPolarizationDelay, which is an optional field, as non-existent.
	 */
	void clearCrossPolarizationDelay ();
	


	////////////////////////////////
	// Extrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute antennaId
	
	
	

	
 	/**
 	 * Get antennaId.
 	 * @return antennaId as Tag
 	 */
 	Tag getAntennaId() const;
	
 
 	
 	
 	/**
 	 * Set antennaId with the specified Tag.
 	 * @param antennaId The Tag value to which antennaId is to be set.
 	 
 		
 			
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 		
 	 */
 	void setAntennaId (Tag antennaId);
  		
	
	
	


	
	// ===> Attribute fieldId
	
	
	

	
 	/**
 	 * Get fieldId.
 	 * @return fieldId as Tag
 	 */
 	Tag getFieldId() const;
	
 
 	
 	
 	/**
 	 * Set fieldId with the specified Tag.
 	 * @param fieldId The Tag value to which fieldId is to be set.
 	 
 		
 			
 	 */
 	void setFieldId (Tag fieldId);
  		
	
	
	


	
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
	 * antennaId pointer to the row in the Antenna table having Antenna.antennaId == antennaId
	 * @return a AntennaRow*
	 * 
	 
	 */
	 AntennaRow* getAntennaUsingAntennaId();
	 

	

	

	
		
	/**
	 * spectralWindowId pointer to the row in the SpectralWindow table having SpectralWindow.spectralWindowId == spectralWindowId
	 * @return a SpectralWindowRow*
	 * 
	 
	 */
	 SpectralWindowRow* getSpectralWindowUsingSpectralWindowId();
	 

	

	

	
		
	/**
	 * fieldId pointer to the row in the Field table having Field.fieldId == fieldId
	 * @return a FieldRow*
	 * 
	 
	 */
	 FieldRow* getFieldUsingFieldId();
	 

	

	
	
	
	/**
	 * Compare each mandatory attribute except the autoincrementable one of this DelayModelRow with 
	 * the corresponding parameters and return true if there is a match and false otherwise.
	 	
	 * @param antennaId
	    
	 * @param spectralWindowId
	    
	 * @param timeInterval
	    
	 * @param numPoly
	    
	 * @param phaseDelay
	    
	 * @param phaseDelayRate
	    
	 * @param groupDelay
	    
	 * @param groupDelayRate
	    
	 * @param fieldId
	    
	 */ 
	bool compareNoAutoInc(Tag antennaId, Tag spectralWindowId, ArrayTimeInterval timeInterval, int numPoly, std::vector<double > phaseDelay, std::vector<double > phaseDelayRate, std::vector<double > groupDelay, std::vector<double > groupDelayRate, Tag fieldId);
	
	

	
	/**
	 * Compare each mandatory value (i.e. not in the key) attribute  with 
	 * the corresponding parameters and return true if there is a match and false otherwise.
	 	
	 * @param numPoly
	    
	 * @param phaseDelay
	    
	 * @param phaseDelayRate
	    
	 * @param groupDelay
	    
	 * @param groupDelayRate
	    
	 * @param fieldId
	    
	 */ 
	bool compareRequiredValue(int numPoly, std::vector<double > phaseDelay, std::vector<double > phaseDelayRate, std::vector<double > groupDelay, std::vector<double > groupDelayRate, Tag fieldId); 
		 
	
	/**
	 * Return true if all required attributes of the value part are equal to their homologues
	 * in x and false otherwise.
	 *
	 * @param x a pointer on the DelayModelRow whose required attributes of the value part 
	 * will be compared with those of this.
	 * @return a boolean.
	 */
	bool equalByRequiredValue(DelayModelRow* x) ;
	
#ifndef WITHOUT_ACS
	/**
	 * Return this row in the form of an IDL struct.
	 * @return The values of this row as a DelayModelRowIDL struct.
	 */
	asdmIDL::DelayModelRowIDL *toIDL() const;
	
	/**
	 * Define the content of a DelayModelRowIDL struct from the values
	 * found in this row.
	 *
	 * @param x a reference to the DelayModelRowIDL struct to be set.
	 *
	 */
	 void toIDL(asdmIDL::DelayModelRowIDL& x) const;
#endif
	
#ifndef WITHOUT_ACS
	/**
	 * Fill the values of this row from the IDL struct DelayModelRowIDL.
	 * @param x The IDL struct containing the values used to fill this row.
	 * @throws ConversionException
	 */
	void setFromIDL (asdmIDL::DelayModelRowIDL x) ;
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

	std::map<std::string, DelayModelAttributeFromBin> fromBinMethods;
void antennaIdFromBin( EndianIStream& eis);
void spectralWindowIdFromBin( EndianIStream& eis);
void timeIntervalFromBin( EndianIStream& eis);
void numPolyFromBin( EndianIStream& eis);
void phaseDelayFromBin( EndianIStream& eis);
void phaseDelayRateFromBin( EndianIStream& eis);
void groupDelayFromBin( EndianIStream& eis);
void groupDelayRateFromBin( EndianIStream& eis);
void fieldIdFromBin( EndianIStream& eis);

void timeOriginFromBin( EndianIStream& eis);
void atmosphericGroupDelayFromBin( EndianIStream& eis);
void atmosphericGroupDelayRateFromBin( EndianIStream& eis);
void geometricDelayFromBin( EndianIStream& eis);
void geometricDelayRateFromBin( EndianIStream& eis);
void numLOFromBin( EndianIStream& eis);
void LOOffsetFromBin( EndianIStream& eis);
void LOOffsetRateFromBin( EndianIStream& eis);
void dispersiveDelayFromBin( EndianIStream& eis);
void dispersiveDelayRateFromBin( EndianIStream& eis);
void atmosphericDryDelayFromBin( EndianIStream& eis);
void atmosphericWetDelayFromBin( EndianIStream& eis);
void padDelayFromBin( EndianIStream& eis);
void antennaDelayFromBin( EndianIStream& eis);
void numReceptorFromBin( EndianIStream& eis);
void polarizationTypeFromBin( EndianIStream& eis);
void electronicDelayFromBin( EndianIStream& eis);
void electronicDelayRateFromBin( EndianIStream& eis);
void receiverDelayFromBin( EndianIStream& eis);
void IFDelayFromBin( EndianIStream& eis);
void LODelayFromBin( EndianIStream& eis);
void crossPolarizationDelayFromBin( EndianIStream& eis);


	 /**
	  * Deserialize a stream of bytes read from an EndianIStream to build a PointingRow.
	  * @param eiss the EndianIStream to be read.
	  * @param table the DelayModelTable to which the row built by deserialization will be parented.
	  * @param attributesSeq a vector containing the names of the attributes . The elements order defines the order 
	  * in which the attributes are written in the binary serialization.
	  */
	 static DelayModelRow* fromBin(EndianIStream& eis, DelayModelTable& table, const std::vector<std::string>& attributesSeq);	 
 
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
	DelayModelTable &table;
	/**
	 * Whether this row has been added to the table or not.
	 */
	bool hasBeenAdded;

	// This method is used by the Table class when this row is added to the table.
	void isAdded(bool added);


	/**
	 * Create a DelayModelRow.
	 * <p>
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 */ 
	DelayModelRow (DelayModelTable &table);

	/**
	 * Create a DelayModelRow using a copy constructor mechanism.
	 * <p>
	 * Given a DelayModelRow row and a DelayModelTable table, the method creates a new
	 * DelayModelRow owned by table. Each attribute of the created row is a copy (deep)
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
	 DelayModelRow (DelayModelTable &table, DelayModelRow *row);
	 	
	////////////////////////////////
	// Intrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute timeInterval
	
	

	ArrayTimeInterval timeInterval;

	
	
 	

	
	// ===> Attribute numPoly
	
	

	int numPoly;

	
	
 	

	
	// ===> Attribute phaseDelay
	
	

	std::vector<double > phaseDelay;

	
	
 	

	
	// ===> Attribute phaseDelayRate
	
	

	std::vector<double > phaseDelayRate;

	
	
 	

	
	// ===> Attribute groupDelay
	
	

	std::vector<double > groupDelay;

	
	
 	

	
	// ===> Attribute groupDelayRate
	
	

	std::vector<double > groupDelayRate;

	
	
 	

	
	// ===> Attribute timeOrigin, which is optional
	
	
	bool timeOriginExists;
	

	ArrayTime timeOrigin;

	
	
 	

	
	// ===> Attribute atmosphericGroupDelay, which is optional
	
	
	bool atmosphericGroupDelayExists;
	

	double atmosphericGroupDelay;

	
	
 	

	
	// ===> Attribute atmosphericGroupDelayRate, which is optional
	
	
	bool atmosphericGroupDelayRateExists;
	

	double atmosphericGroupDelayRate;

	
	
 	

	
	// ===> Attribute geometricDelay, which is optional
	
	
	bool geometricDelayExists;
	

	double geometricDelay;

	
	
 	

	
	// ===> Attribute geometricDelayRate, which is optional
	
	
	bool geometricDelayRateExists;
	

	double geometricDelayRate;

	
	
 	

	
	// ===> Attribute numLO, which is optional
	
	
	bool numLOExists;
	

	int numLO;

	
	
 	

	
	// ===> Attribute LOOffset, which is optional
	
	
	bool LOOffsetExists;
	

	std::vector<Frequency > LOOffset;

	
	
 	

	
	// ===> Attribute LOOffsetRate, which is optional
	
	
	bool LOOffsetRateExists;
	

	std::vector<Frequency > LOOffsetRate;

	
	
 	

	
	// ===> Attribute dispersiveDelay, which is optional
	
	
	bool dispersiveDelayExists;
	

	double dispersiveDelay;

	
	
 	

	
	// ===> Attribute dispersiveDelayRate, which is optional
	
	
	bool dispersiveDelayRateExists;
	

	double dispersiveDelayRate;

	
	
 	

	
	// ===> Attribute atmosphericDryDelay, which is optional
	
	
	bool atmosphericDryDelayExists;
	

	double atmosphericDryDelay;

	
	
 	

	
	// ===> Attribute atmosphericWetDelay, which is optional
	
	
	bool atmosphericWetDelayExists;
	

	double atmosphericWetDelay;

	
	
 	

	
	// ===> Attribute padDelay, which is optional
	
	
	bool padDelayExists;
	

	double padDelay;

	
	
 	

	
	// ===> Attribute antennaDelay, which is optional
	
	
	bool antennaDelayExists;
	

	double antennaDelay;

	
	
 	

	
	// ===> Attribute numReceptor, which is optional
	
	
	bool numReceptorExists;
	

	int numReceptor;

	
	
 	

	
	// ===> Attribute polarizationType, which is optional
	
	
	bool polarizationTypeExists;
	

	std::vector<PolarizationTypeMod::PolarizationType > polarizationType;

	
	
 	

	
	// ===> Attribute electronicDelay, which is optional
	
	
	bool electronicDelayExists;
	

	std::vector<double > electronicDelay;

	
	
 	

	
	// ===> Attribute electronicDelayRate, which is optional
	
	
	bool electronicDelayRateExists;
	

	std::vector<double > electronicDelayRate;

	
	
 	

	
	// ===> Attribute receiverDelay, which is optional
	
	
	bool receiverDelayExists;
	

	std::vector<double > receiverDelay;

	
	
 	

	
	// ===> Attribute IFDelay, which is optional
	
	
	bool IFDelayExists;
	

	std::vector<double > IFDelay;

	
	
 	

	
	// ===> Attribute LODelay, which is optional
	
	
	bool LODelayExists;
	

	std::vector<double > LODelay;

	
	
 	

	
	// ===> Attribute crossPolarizationDelay, which is optional
	
	
	bool crossPolarizationDelayExists;
	

	double crossPolarizationDelay;

	
	
 	

	////////////////////////////////
	// Extrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute antennaId
	
	

	Tag antennaId;

	
	
 	

	
	// ===> Attribute fieldId
	
	

	Tag fieldId;

	
	
 	

	
	// ===> Attribute spectralWindowId
	
	

	Tag spectralWindowId;

	
	
 	

	///////////
	// Links //
	///////////
	
	
		

	 

	

	
		

	 

	

	
		

	 

	

	
/*
	////////////////////////////////////////////////////////////
	// binary-deserialization material from an EndianIStream  //
	////////////////////////////////////////////////////////////
	std::map<std::string, DelayModelAttributeFromBin> fromBinMethods;
void antennaIdFromBin( EndianIStream& eis);
void spectralWindowIdFromBin( EndianIStream& eis);
void timeIntervalFromBin( EndianIStream& eis);
void numPolyFromBin( EndianIStream& eis);
void phaseDelayFromBin( EndianIStream& eis);
void phaseDelayRateFromBin( EndianIStream& eis);
void groupDelayFromBin( EndianIStream& eis);
void groupDelayRateFromBin( EndianIStream& eis);
void fieldIdFromBin( EndianIStream& eis);

void timeOriginFromBin( EndianIStream& eis);
void atmosphericGroupDelayFromBin( EndianIStream& eis);
void atmosphericGroupDelayRateFromBin( EndianIStream& eis);
void geometricDelayFromBin( EndianIStream& eis);
void geometricDelayRateFromBin( EndianIStream& eis);
void numLOFromBin( EndianIStream& eis);
void LOOffsetFromBin( EndianIStream& eis);
void LOOffsetRateFromBin( EndianIStream& eis);
void dispersiveDelayFromBin( EndianIStream& eis);
void dispersiveDelayRateFromBin( EndianIStream& eis);
void atmosphericDryDelayFromBin( EndianIStream& eis);
void atmosphericWetDelayFromBin( EndianIStream& eis);
void padDelayFromBin( EndianIStream& eis);
void antennaDelayFromBin( EndianIStream& eis);
void numReceptorFromBin( EndianIStream& eis);
void polarizationTypeFromBin( EndianIStream& eis);
void electronicDelayFromBin( EndianIStream& eis);
void electronicDelayRateFromBin( EndianIStream& eis);
void receiverDelayFromBin( EndianIStream& eis);
void IFDelayFromBin( EndianIStream& eis);
void LODelayFromBin( EndianIStream& eis);
void crossPolarizationDelayFromBin( EndianIStream& eis);

*/
	
	///////////////////////////////////
	// text-deserialization material //
	///////////////////////////////////
	std::map<std::string, DelayModelAttributeFromText> fromTextMethods;
	
void antennaIdFromText (const std::string & s);
	
	
void spectralWindowIdFromText (const std::string & s);
	
	
void timeIntervalFromText (const std::string & s);
	
	
void numPolyFromText (const std::string & s);
	
	
void phaseDelayFromText (const std::string & s);
	
	
void phaseDelayRateFromText (const std::string & s);
	
	
void groupDelayFromText (const std::string & s);
	
	
void groupDelayRateFromText (const std::string & s);
	
	
void fieldIdFromText (const std::string & s);
	

	
void timeOriginFromText (const std::string & s);
	
	
void atmosphericGroupDelayFromText (const std::string & s);
	
	
void atmosphericGroupDelayRateFromText (const std::string & s);
	
	
void geometricDelayFromText (const std::string & s);
	
	
void geometricDelayRateFromText (const std::string & s);
	
	
void numLOFromText (const std::string & s);
	
	
void LOOffsetFromText (const std::string & s);
	
	
void LOOffsetRateFromText (const std::string & s);
	
	
void dispersiveDelayFromText (const std::string & s);
	
	
void dispersiveDelayRateFromText (const std::string & s);
	
	
void atmosphericDryDelayFromText (const std::string & s);
	
	
void atmosphericWetDelayFromText (const std::string & s);
	
	
void padDelayFromText (const std::string & s);
	
	
void antennaDelayFromText (const std::string & s);
	
	
void numReceptorFromText (const std::string & s);
	
	
void polarizationTypeFromText (const std::string & s);
	
	
void electronicDelayFromText (const std::string & s);
	
	
void electronicDelayRateFromText (const std::string & s);
	
	
void receiverDelayFromText (const std::string & s);
	
	
void IFDelayFromText (const std::string & s);
	
	
void LODelayFromText (const std::string & s);
	
	
void crossPolarizationDelayFromText (const std::string & s);
	
	
	
	/**
	 * Serialize this into a stream of bytes written to an EndianOSStream.
	 * @param eoss the EndianOSStream to be written to
	 */
	 void toBin(EndianOSStream& eoss);
	 	 
	 /**
	  * Deserialize a stream of bytes read from an EndianIStream to build a PointingRow.
	  * @param eiss the EndianIStream to be read.
	  * @param table the DelayModelTable to which the row built by deserialization will be parented.
	  * @param attributesSeq a vector containing the names of the attributes . The elements order defines the order 
	  * in which the attributes are written in the binary serialization.

	 static DelayModelRow* fromBin(EndianIStream& eis, DelayModelTable& table, const std::vector<std::string>& attributesSeq);	 
		*/
};

} // End namespace asdm

#endif /* DelayModel_CLASS */
