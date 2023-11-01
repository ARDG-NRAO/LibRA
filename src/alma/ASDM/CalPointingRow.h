
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
 * File CalPointingRow.h
 */
 
#ifndef CalPointingRow_CLASS
#define CalPointingRow_CLASS

#include <vector>
#include <string>
#include <set>

#ifndef WITHOUT_ACS
#include <asdmIDLC.h>
#endif






	 
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
#include <alma/ASDM/NoSuchRow.h>
#include <alma/ASDM/IllegalAccessException.h>

#include <alma/ASDM/RowTransformer.h>
//#include <alma/ASDM/TableStreamReader.h>

/*\file CalPointing.h
    \brief Generated from model's revision "-1", branch ""
*/

namespace asdm {

//class asdm::CalPointingTable;


// class asdm::CalDataRow;
class CalDataRow;

// class asdm::CalReductionRow;
class CalReductionRow;
	

class CalPointingRow;
typedef void (CalPointingRow::*CalPointingAttributeFromBin) (EndianIStream& eis);
typedef void (CalPointingRow::*CalPointingAttributeFromText) (const std::string& s);

/**
 * The CalPointingRow class is a row of a CalPointingTable.
 * 
 * Generated from model's revision "-1", branch ""
 *
 */
class CalPointingRow {
friend class asdm::CalPointingTable;
friend class asdm::RowTransformer<CalPointingRow>;
//friend class asdm::TableStreamReader<CalPointingTable, CalPointingRow>;

public:

	virtual ~CalPointingRow();

	/**
	 * Return the table to which this row belongs.
	 */
	CalPointingTable &getTable() const;
	
	/**
	 * Has this row been added to its table ?
	 * @return true if and only if it has been added.
	 */
	bool isAdded() const;
		
	////////////////////////////////
	// Intrinsic Table Attributes //
	////////////////////////////////
	
	
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
  		
	
	
	


	
	// ===> Attribute ambientTemperature
	
	
	

	
 	/**
 	 * Get ambientTemperature.
 	 * @return ambientTemperature as Temperature
 	 */
 	Temperature getAmbientTemperature() const;
	
 
 	
 	
 	/**
 	 * Set ambientTemperature with the specified Temperature.
 	 * @param ambientTemperature The Temperature value to which ambientTemperature is to be set.
 	 
 		
 			
 	 */
 	void setAmbientTemperature (Temperature ambientTemperature);
  		
	
	
	


	
	// ===> Attribute antennaMake
	
	
	

	
 	/**
 	 * Get antennaMake.
 	 * @return antennaMake as AntennaMakeMod::AntennaMake
 	 */
 	AntennaMakeMod::AntennaMake getAntennaMake() const;
	
 
 	
 	
 	/**
 	 * Set antennaMake with the specified AntennaMakeMod::AntennaMake.
 	 * @param antennaMake The AntennaMakeMod::AntennaMake value to which antennaMake is to be set.
 	 
 		
 			
 	 */
 	void setAntennaMake (AntennaMakeMod::AntennaMake antennaMake);
  		
	
	
	


	
	// ===> Attribute atmPhaseCorrection
	
	
	

	
 	/**
 	 * Get atmPhaseCorrection.
 	 * @return atmPhaseCorrection as AtmPhaseCorrectionMod::AtmPhaseCorrection
 	 */
 	AtmPhaseCorrectionMod::AtmPhaseCorrection getAtmPhaseCorrection() const;
	
 
 	
 	
 	/**
 	 * Set atmPhaseCorrection with the specified AtmPhaseCorrectionMod::AtmPhaseCorrection.
 	 * @param atmPhaseCorrection The AtmPhaseCorrectionMod::AtmPhaseCorrection value to which atmPhaseCorrection is to be set.
 	 
 		
 			
 	 */
 	void setAtmPhaseCorrection (AtmPhaseCorrectionMod::AtmPhaseCorrection atmPhaseCorrection);
  		
	
	
	


	
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
  		
	
	
	


	
	// ===> Attribute pointingModelMode
	
	
	

	
 	/**
 	 * Get pointingModelMode.
 	 * @return pointingModelMode as PointingModelModeMod::PointingModelMode
 	 */
 	PointingModelModeMod::PointingModelMode getPointingModelMode() const;
	
 
 	
 	
 	/**
 	 * Set pointingModelMode with the specified PointingModelModeMod::PointingModelMode.
 	 * @param pointingModelMode The PointingModelModeMod::PointingModelMode value to which pointingModelMode is to be set.
 	 
 		
 			
 	 */
 	void setPointingModelMode (PointingModelModeMod::PointingModelMode pointingModelMode);
  		
	
	
	


	
	// ===> Attribute pointingMethod
	
	
	

	
 	/**
 	 * Get pointingMethod.
 	 * @return pointingMethod as PointingMethodMod::PointingMethod
 	 */
 	PointingMethodMod::PointingMethod getPointingMethod() const;
	
 
 	
 	
 	/**
 	 * Set pointingMethod with the specified PointingMethodMod::PointingMethod.
 	 * @param pointingMethod The PointingMethodMod::PointingMethod value to which pointingMethod is to be set.
 	 
 		
 			
 	 */
 	void setPointingMethod (PointingMethodMod::PointingMethod pointingMethod);
  		
	
	
	


	
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
  		
	
	
	


	
	// ===> Attribute collOffsetRelative
	
	
	

	
 	/**
 	 * Get collOffsetRelative.
 	 * @return collOffsetRelative as std::vector<std::vector<Angle > >
 	 */
 	std::vector<std::vector<Angle > > getCollOffsetRelative() const;
	
 
 	
 	
 	/**
 	 * Set collOffsetRelative with the specified std::vector<std::vector<Angle > >.
 	 * @param collOffsetRelative The std::vector<std::vector<Angle > > value to which collOffsetRelative is to be set.
 	 
 		
 			
 	 */
 	void setCollOffsetRelative (std::vector<std::vector<Angle > > collOffsetRelative);
  		
	
	
	


	
	// ===> Attribute collOffsetAbsolute
	
	
	

	
 	/**
 	 * Get collOffsetAbsolute.
 	 * @return collOffsetAbsolute as std::vector<std::vector<Angle > >
 	 */
 	std::vector<std::vector<Angle > > getCollOffsetAbsolute() const;
	
 
 	
 	
 	/**
 	 * Set collOffsetAbsolute with the specified std::vector<std::vector<Angle > >.
 	 * @param collOffsetAbsolute The std::vector<std::vector<Angle > > value to which collOffsetAbsolute is to be set.
 	 
 		
 			
 	 */
 	void setCollOffsetAbsolute (std::vector<std::vector<Angle > > collOffsetAbsolute);
  		
	
	
	


	
	// ===> Attribute collError
	
	
	

	
 	/**
 	 * Get collError.
 	 * @return collError as std::vector<std::vector<Angle > >
 	 */
 	std::vector<std::vector<Angle > > getCollError() const;
	
 
 	
 	
 	/**
 	 * Set collError with the specified std::vector<std::vector<Angle > >.
 	 * @param collError The std::vector<std::vector<Angle > > value to which collError is to be set.
 	 
 		
 			
 	 */
 	void setCollError (std::vector<std::vector<Angle > > collError);
  		
	
	
	


	
	// ===> Attribute collOffsetTied
	
	
	

	
 	/**
 	 * Get collOffsetTied.
 	 * @return collOffsetTied as std::vector<std::vector<bool > >
 	 */
 	std::vector<std::vector<bool > > getCollOffsetTied() const;
	
 
 	
 	
 	/**
 	 * Set collOffsetTied with the specified std::vector<std::vector<bool > >.
 	 * @param collOffsetTied The std::vector<std::vector<bool > > value to which collOffsetTied is to be set.
 	 
 		
 			
 	 */
 	void setCollOffsetTied (std::vector<std::vector<bool > > collOffsetTied);
  		
	
	
	


	
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
  		
	
	
	


	
	// ===> Attribute averagedPolarizations, which is optional
	
	
	
	/**
	 * The attribute averagedPolarizations is optional. Return true if this attribute exists.
	 * @return true if and only if the averagedPolarizations attribute exists. 
	 */
	bool isAveragedPolarizationsExists() const;
	

	
 	/**
 	 * Get averagedPolarizations, which is optional.
 	 * @return averagedPolarizations as bool
 	 * @throws IllegalAccessException If averagedPolarizations does not exist.
 	 */
 	bool getAveragedPolarizations() const;
	
 
 	
 	
 	/**
 	 * Set averagedPolarizations with the specified bool.
 	 * @param averagedPolarizations The bool value to which averagedPolarizations is to be set.
 	 
 		
 	 */
 	void setAveragedPolarizations (bool averagedPolarizations);
		
	
	
	
	/**
	 * Mark averagedPolarizations, which is an optional field, as non-existent.
	 */
	void clearAveragedPolarizations ();
	


	
	// ===> Attribute beamPA, which is optional
	
	
	
	/**
	 * The attribute beamPA is optional. Return true if this attribute exists.
	 * @return true if and only if the beamPA attribute exists. 
	 */
	bool isBeamPAExists() const;
	

	
 	/**
 	 * Get beamPA, which is optional.
 	 * @return beamPA as std::vector<Angle >
 	 * @throws IllegalAccessException If beamPA does not exist.
 	 */
 	std::vector<Angle > getBeamPA() const;
	
 
 	
 	
 	/**
 	 * Set beamPA with the specified std::vector<Angle >.
 	 * @param beamPA The std::vector<Angle > value to which beamPA is to be set.
 	 
 		
 	 */
 	void setBeamPA (std::vector<Angle > beamPA);
		
	
	
	
	/**
	 * Mark beamPA, which is an optional field, as non-existent.
	 */
	void clearBeamPA ();
	


	
	// ===> Attribute beamPAError, which is optional
	
	
	
	/**
	 * The attribute beamPAError is optional. Return true if this attribute exists.
	 * @return true if and only if the beamPAError attribute exists. 
	 */
	bool isBeamPAErrorExists() const;
	

	
 	/**
 	 * Get beamPAError, which is optional.
 	 * @return beamPAError as std::vector<Angle >
 	 * @throws IllegalAccessException If beamPAError does not exist.
 	 */
 	std::vector<Angle > getBeamPAError() const;
	
 
 	
 	
 	/**
 	 * Set beamPAError with the specified std::vector<Angle >.
 	 * @param beamPAError The std::vector<Angle > value to which beamPAError is to be set.
 	 
 		
 	 */
 	void setBeamPAError (std::vector<Angle > beamPAError);
		
	
	
	
	/**
	 * Mark beamPAError, which is an optional field, as non-existent.
	 */
	void clearBeamPAError ();
	


	
	// ===> Attribute beamPAWasFixed, which is optional
	
	
	
	/**
	 * The attribute beamPAWasFixed is optional. Return true if this attribute exists.
	 * @return true if and only if the beamPAWasFixed attribute exists. 
	 */
	bool isBeamPAWasFixedExists() const;
	

	
 	/**
 	 * Get beamPAWasFixed, which is optional.
 	 * @return beamPAWasFixed as bool
 	 * @throws IllegalAccessException If beamPAWasFixed does not exist.
 	 */
 	bool getBeamPAWasFixed() const;
	
 
 	
 	
 	/**
 	 * Set beamPAWasFixed with the specified bool.
 	 * @param beamPAWasFixed The bool value to which beamPAWasFixed is to be set.
 	 
 		
 	 */
 	void setBeamPAWasFixed (bool beamPAWasFixed);
		
	
	
	
	/**
	 * Mark beamPAWasFixed, which is an optional field, as non-existent.
	 */
	void clearBeamPAWasFixed ();
	


	
	// ===> Attribute beamWidth, which is optional
	
	
	
	/**
	 * The attribute beamWidth is optional. Return true if this attribute exists.
	 * @return true if and only if the beamWidth attribute exists. 
	 */
	bool isBeamWidthExists() const;
	

	
 	/**
 	 * Get beamWidth, which is optional.
 	 * @return beamWidth as std::vector<std::vector<Angle > >
 	 * @throws IllegalAccessException If beamWidth does not exist.
 	 */
 	std::vector<std::vector<Angle > > getBeamWidth() const;
	
 
 	
 	
 	/**
 	 * Set beamWidth with the specified std::vector<std::vector<Angle > >.
 	 * @param beamWidth The std::vector<std::vector<Angle > > value to which beamWidth is to be set.
 	 
 		
 	 */
 	void setBeamWidth (std::vector<std::vector<Angle > > beamWidth);
		
	
	
	
	/**
	 * Mark beamWidth, which is an optional field, as non-existent.
	 */
	void clearBeamWidth ();
	


	
	// ===> Attribute beamWidthError, which is optional
	
	
	
	/**
	 * The attribute beamWidthError is optional. Return true if this attribute exists.
	 * @return true if and only if the beamWidthError attribute exists. 
	 */
	bool isBeamWidthErrorExists() const;
	

	
 	/**
 	 * Get beamWidthError, which is optional.
 	 * @return beamWidthError as std::vector<std::vector<Angle > >
 	 * @throws IllegalAccessException If beamWidthError does not exist.
 	 */
 	std::vector<std::vector<Angle > > getBeamWidthError() const;
	
 
 	
 	
 	/**
 	 * Set beamWidthError with the specified std::vector<std::vector<Angle > >.
 	 * @param beamWidthError The std::vector<std::vector<Angle > > value to which beamWidthError is to be set.
 	 
 		
 	 */
 	void setBeamWidthError (std::vector<std::vector<Angle > > beamWidthError);
		
	
	
	
	/**
	 * Mark beamWidthError, which is an optional field, as non-existent.
	 */
	void clearBeamWidthError ();
	


	
	// ===> Attribute beamWidthWasFixed, which is optional
	
	
	
	/**
	 * The attribute beamWidthWasFixed is optional. Return true if this attribute exists.
	 * @return true if and only if the beamWidthWasFixed attribute exists. 
	 */
	bool isBeamWidthWasFixedExists() const;
	

	
 	/**
 	 * Get beamWidthWasFixed, which is optional.
 	 * @return beamWidthWasFixed as std::vector<bool >
 	 * @throws IllegalAccessException If beamWidthWasFixed does not exist.
 	 */
 	std::vector<bool > getBeamWidthWasFixed() const;
	
 
 	
 	
 	/**
 	 * Set beamWidthWasFixed with the specified std::vector<bool >.
 	 * @param beamWidthWasFixed The std::vector<bool > value to which beamWidthWasFixed is to be set.
 	 
 		
 	 */
 	void setBeamWidthWasFixed (std::vector<bool > beamWidthWasFixed);
		
	
	
	
	/**
	 * Mark beamWidthWasFixed, which is an optional field, as non-existent.
	 */
	void clearBeamWidthWasFixed ();
	


	
	// ===> Attribute offIntensity, which is optional
	
	
	
	/**
	 * The attribute offIntensity is optional. Return true if this attribute exists.
	 * @return true if and only if the offIntensity attribute exists. 
	 */
	bool isOffIntensityExists() const;
	

	
 	/**
 	 * Get offIntensity, which is optional.
 	 * @return offIntensity as std::vector<Temperature >
 	 * @throws IllegalAccessException If offIntensity does not exist.
 	 */
 	std::vector<Temperature > getOffIntensity() const;
	
 
 	
 	
 	/**
 	 * Set offIntensity with the specified std::vector<Temperature >.
 	 * @param offIntensity The std::vector<Temperature > value to which offIntensity is to be set.
 	 
 		
 	 */
 	void setOffIntensity (std::vector<Temperature > offIntensity);
		
	
	
	
	/**
	 * Mark offIntensity, which is an optional field, as non-existent.
	 */
	void clearOffIntensity ();
	


	
	// ===> Attribute offIntensityError, which is optional
	
	
	
	/**
	 * The attribute offIntensityError is optional. Return true if this attribute exists.
	 * @return true if and only if the offIntensityError attribute exists. 
	 */
	bool isOffIntensityErrorExists() const;
	

	
 	/**
 	 * Get offIntensityError, which is optional.
 	 * @return offIntensityError as std::vector<Temperature >
 	 * @throws IllegalAccessException If offIntensityError does not exist.
 	 */
 	std::vector<Temperature > getOffIntensityError() const;
	
 
 	
 	
 	/**
 	 * Set offIntensityError with the specified std::vector<Temperature >.
 	 * @param offIntensityError The std::vector<Temperature > value to which offIntensityError is to be set.
 	 
 		
 	 */
 	void setOffIntensityError (std::vector<Temperature > offIntensityError);
		
	
	
	
	/**
	 * Mark offIntensityError, which is an optional field, as non-existent.
	 */
	void clearOffIntensityError ();
	


	
	// ===> Attribute offIntensityWasFixed, which is optional
	
	
	
	/**
	 * The attribute offIntensityWasFixed is optional. Return true if this attribute exists.
	 * @return true if and only if the offIntensityWasFixed attribute exists. 
	 */
	bool isOffIntensityWasFixedExists() const;
	

	
 	/**
 	 * Get offIntensityWasFixed, which is optional.
 	 * @return offIntensityWasFixed as bool
 	 * @throws IllegalAccessException If offIntensityWasFixed does not exist.
 	 */
 	bool getOffIntensityWasFixed() const;
	
 
 	
 	
 	/**
 	 * Set offIntensityWasFixed with the specified bool.
 	 * @param offIntensityWasFixed The bool value to which offIntensityWasFixed is to be set.
 	 
 		
 	 */
 	void setOffIntensityWasFixed (bool offIntensityWasFixed);
		
	
	
	
	/**
	 * Mark offIntensityWasFixed, which is an optional field, as non-existent.
	 */
	void clearOffIntensityWasFixed ();
	


	
	// ===> Attribute peakIntensity, which is optional
	
	
	
	/**
	 * The attribute peakIntensity is optional. Return true if this attribute exists.
	 * @return true if and only if the peakIntensity attribute exists. 
	 */
	bool isPeakIntensityExists() const;
	

	
 	/**
 	 * Get peakIntensity, which is optional.
 	 * @return peakIntensity as std::vector<Temperature >
 	 * @throws IllegalAccessException If peakIntensity does not exist.
 	 */
 	std::vector<Temperature > getPeakIntensity() const;
	
 
 	
 	
 	/**
 	 * Set peakIntensity with the specified std::vector<Temperature >.
 	 * @param peakIntensity The std::vector<Temperature > value to which peakIntensity is to be set.
 	 
 		
 	 */
 	void setPeakIntensity (std::vector<Temperature > peakIntensity);
		
	
	
	
	/**
	 * Mark peakIntensity, which is an optional field, as non-existent.
	 */
	void clearPeakIntensity ();
	


	
	// ===> Attribute peakIntensityError, which is optional
	
	
	
	/**
	 * The attribute peakIntensityError is optional. Return true if this attribute exists.
	 * @return true if and only if the peakIntensityError attribute exists. 
	 */
	bool isPeakIntensityErrorExists() const;
	

	
 	/**
 	 * Get peakIntensityError, which is optional.
 	 * @return peakIntensityError as std::vector<Temperature >
 	 * @throws IllegalAccessException If peakIntensityError does not exist.
 	 */
 	std::vector<Temperature > getPeakIntensityError() const;
	
 
 	
 	
 	/**
 	 * Set peakIntensityError with the specified std::vector<Temperature >.
 	 * @param peakIntensityError The std::vector<Temperature > value to which peakIntensityError is to be set.
 	 
 		
 	 */
 	void setPeakIntensityError (std::vector<Temperature > peakIntensityError);
		
	
	
	
	/**
	 * Mark peakIntensityError, which is an optional field, as non-existent.
	 */
	void clearPeakIntensityError ();
	


	
	// ===> Attribute peakIntensityWasFixed, which is optional
	
	
	
	/**
	 * The attribute peakIntensityWasFixed is optional. Return true if this attribute exists.
	 * @return true if and only if the peakIntensityWasFixed attribute exists. 
	 */
	bool isPeakIntensityWasFixedExists() const;
	

	
 	/**
 	 * Get peakIntensityWasFixed, which is optional.
 	 * @return peakIntensityWasFixed as bool
 	 * @throws IllegalAccessException If peakIntensityWasFixed does not exist.
 	 */
 	bool getPeakIntensityWasFixed() const;
	
 
 	
 	
 	/**
 	 * Set peakIntensityWasFixed with the specified bool.
 	 * @param peakIntensityWasFixed The bool value to which peakIntensityWasFixed is to be set.
 	 
 		
 	 */
 	void setPeakIntensityWasFixed (bool peakIntensityWasFixed);
		
	
	
	
	/**
	 * Mark peakIntensityWasFixed, which is an optional field, as non-existent.
	 */
	void clearPeakIntensityWasFixed ();
	


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
	 * calDataId pointer to the row in the CalData table having CalData.calDataId == calDataId
	 * @return a CalDataRow*
	 * 
	 
	 */
	 CalDataRow* getCalDataUsingCalDataId();
	 

	

	

	
		
	/**
	 * calReductionId pointer to the row in the CalReduction table having CalReduction.calReductionId == calReductionId
	 * @return a CalReductionRow*
	 * 
	 
	 */
	 CalReductionRow* getCalReductionUsingCalReductionId();
	 

	

	
	
	
	/**
	 * Compare each mandatory attribute except the autoincrementable one of this CalPointingRow with 
	 * the corresponding parameters and return true if there is a match and false otherwise.
	 	
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
	bool compareNoAutoInc(std::string antennaName, ReceiverBandMod::ReceiverBand receiverBand, Tag calDataId, Tag calReductionId, ArrayTime startValidTime, ArrayTime endValidTime, Temperature ambientTemperature, AntennaMakeMod::AntennaMake antennaMake, AtmPhaseCorrectionMod::AtmPhaseCorrection atmPhaseCorrection, std::vector<Angle > direction, std::vector<Frequency > frequencyRange, PointingModelModeMod::PointingModelMode pointingModelMode, PointingMethodMod::PointingMethod pointingMethod, int numReceptor, std::vector<PolarizationTypeMod::PolarizationType > polarizationTypes, std::vector<std::vector<Angle > > collOffsetRelative, std::vector<std::vector<Angle > > collOffsetAbsolute, std::vector<std::vector<Angle > > collError, std::vector<std::vector<bool > > collOffsetTied, std::vector<double > reducedChiSquared);
	
	

	
	/**
	 * Compare each mandatory value (i.e. not in the key) attribute  with 
	 * the corresponding parameters and return true if there is a match and false otherwise.
	 	
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
	bool compareRequiredValue(ArrayTime startValidTime, ArrayTime endValidTime, Temperature ambientTemperature, AntennaMakeMod::AntennaMake antennaMake, AtmPhaseCorrectionMod::AtmPhaseCorrection atmPhaseCorrection, std::vector<Angle > direction, std::vector<Frequency > frequencyRange, PointingModelModeMod::PointingModelMode pointingModelMode, PointingMethodMod::PointingMethod pointingMethod, int numReceptor, std::vector<PolarizationTypeMod::PolarizationType > polarizationTypes, std::vector<std::vector<Angle > > collOffsetRelative, std::vector<std::vector<Angle > > collOffsetAbsolute, std::vector<std::vector<Angle > > collError, std::vector<std::vector<bool > > collOffsetTied, std::vector<double > reducedChiSquared); 
		 
	
	/**
	 * Return true if all required attributes of the value part are equal to their homologues
	 * in x and false otherwise.
	 *
	 * @param x a pointer on the CalPointingRow whose required attributes of the value part 
	 * will be compared with those of this.
	 * @return a boolean.
	 */
	bool equalByRequiredValue(CalPointingRow* x) ;
	
#ifndef WITHOUT_ACS
	/**
	 * Return this row in the form of an IDL struct.
	 * @return The values of this row as a CalPointingRowIDL struct.
	 */
	asdmIDL::CalPointingRowIDL *toIDL() const;
	
	/**
	 * Define the content of a CalPointingRowIDL struct from the values
	 * found in this row.
	 *
	 * @param x a reference to the CalPointingRowIDL struct to be set.
	 *
	 */
	 void toIDL(asdmIDL::CalPointingRowIDL& x) const;
#endif
	
#ifndef WITHOUT_ACS
	/**
	 * Fill the values of this row from the IDL struct CalPointingRowIDL.
	 * @param x The IDL struct containing the values used to fill this row.
	 * @throws ConversionException
	 */
	void setFromIDL (asdmIDL::CalPointingRowIDL x) ;
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

	std::map<std::string, CalPointingAttributeFromBin> fromBinMethods;
void antennaNameFromBin( EndianIStream& eis);
void receiverBandFromBin( EndianIStream& eis);
void calDataIdFromBin( EndianIStream& eis);
void calReductionIdFromBin( EndianIStream& eis);
void startValidTimeFromBin( EndianIStream& eis);
void endValidTimeFromBin( EndianIStream& eis);
void ambientTemperatureFromBin( EndianIStream& eis);
void antennaMakeFromBin( EndianIStream& eis);
void atmPhaseCorrectionFromBin( EndianIStream& eis);
void directionFromBin( EndianIStream& eis);
void frequencyRangeFromBin( EndianIStream& eis);
void pointingModelModeFromBin( EndianIStream& eis);
void pointingMethodFromBin( EndianIStream& eis);
void numReceptorFromBin( EndianIStream& eis);
void polarizationTypesFromBin( EndianIStream& eis);
void collOffsetRelativeFromBin( EndianIStream& eis);
void collOffsetAbsoluteFromBin( EndianIStream& eis);
void collErrorFromBin( EndianIStream& eis);
void collOffsetTiedFromBin( EndianIStream& eis);
void reducedChiSquaredFromBin( EndianIStream& eis);

void averagedPolarizationsFromBin( EndianIStream& eis);
void beamPAFromBin( EndianIStream& eis);
void beamPAErrorFromBin( EndianIStream& eis);
void beamPAWasFixedFromBin( EndianIStream& eis);
void beamWidthFromBin( EndianIStream& eis);
void beamWidthErrorFromBin( EndianIStream& eis);
void beamWidthWasFixedFromBin( EndianIStream& eis);
void offIntensityFromBin( EndianIStream& eis);
void offIntensityErrorFromBin( EndianIStream& eis);
void offIntensityWasFixedFromBin( EndianIStream& eis);
void peakIntensityFromBin( EndianIStream& eis);
void peakIntensityErrorFromBin( EndianIStream& eis);
void peakIntensityWasFixedFromBin( EndianIStream& eis);


	 /**
	  * Deserialize a stream of bytes read from an EndianIStream to build a PointingRow.
	  * @param eiss the EndianIStream to be read.
	  * @param table the CalPointingTable to which the row built by deserialization will be parented.
	  * @param attributesSeq a vector containing the names of the attributes . The elements order defines the order 
	  * in which the attributes are written in the binary serialization.
	  */
	 static CalPointingRow* fromBin(EndianIStream& eis, CalPointingTable& table, const std::vector<std::string>& attributesSeq);	 
 
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
	CalPointingTable &table;
	/**
	 * Whether this row has been added to the table or not.
	 */
	bool hasBeenAdded;

	// This method is used by the Table class when this row is added to the table.
	void isAdded(bool added);


	/**
	 * Create a CalPointingRow.
	 * <p>
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 */ 
	CalPointingRow (CalPointingTable &table);

	/**
	 * Create a CalPointingRow using a copy constructor mechanism.
	 * <p>
	 * Given a CalPointingRow row and a CalPointingTable table, the method creates a new
	 * CalPointingRow owned by table. Each attribute of the created row is a copy (deep)
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
	 CalPointingRow (CalPointingTable &table, CalPointingRow *row);
	 	
	////////////////////////////////
	// Intrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute antennaName
	
	

	std::string antennaName;

	
	
 	

	
	// ===> Attribute receiverBand
	
	

	ReceiverBandMod::ReceiverBand receiverBand;

	
	
 	

	
	// ===> Attribute startValidTime
	
	

	ArrayTime startValidTime;

	
	
 	

	
	// ===> Attribute endValidTime
	
	

	ArrayTime endValidTime;

	
	
 	

	
	// ===> Attribute ambientTemperature
	
	

	Temperature ambientTemperature;

	
	
 	

	
	// ===> Attribute antennaMake
	
	

	AntennaMakeMod::AntennaMake antennaMake;

	
	
 	

	
	// ===> Attribute atmPhaseCorrection
	
	

	AtmPhaseCorrectionMod::AtmPhaseCorrection atmPhaseCorrection;

	
	
 	

	
	// ===> Attribute direction
	
	

	std::vector<Angle > direction;

	
	
 	

	
	// ===> Attribute frequencyRange
	
	

	std::vector<Frequency > frequencyRange;

	
	
 	

	
	// ===> Attribute pointingModelMode
	
	

	PointingModelModeMod::PointingModelMode pointingModelMode;

	
	
 	

	
	// ===> Attribute pointingMethod
	
	

	PointingMethodMod::PointingMethod pointingMethod;

	
	
 	

	
	// ===> Attribute numReceptor
	
	

	int numReceptor;

	
	
 	

	
	// ===> Attribute polarizationTypes
	
	

	std::vector<PolarizationTypeMod::PolarizationType > polarizationTypes;

	
	
 	

	
	// ===> Attribute collOffsetRelative
	
	

	std::vector<std::vector<Angle > > collOffsetRelative;

	
	
 	

	
	// ===> Attribute collOffsetAbsolute
	
	

	std::vector<std::vector<Angle > > collOffsetAbsolute;

	
	
 	

	
	// ===> Attribute collError
	
	

	std::vector<std::vector<Angle > > collError;

	
	
 	

	
	// ===> Attribute collOffsetTied
	
	

	std::vector<std::vector<bool > > collOffsetTied;

	
	
 	

	
	// ===> Attribute reducedChiSquared
	
	

	std::vector<double > reducedChiSquared;

	
	
 	

	
	// ===> Attribute averagedPolarizations, which is optional
	
	
	bool averagedPolarizationsExists;
	

	bool averagedPolarizations;

	
	
 	

	
	// ===> Attribute beamPA, which is optional
	
	
	bool beamPAExists;
	

	std::vector<Angle > beamPA;

	
	
 	

	
	// ===> Attribute beamPAError, which is optional
	
	
	bool beamPAErrorExists;
	

	std::vector<Angle > beamPAError;

	
	
 	

	
	// ===> Attribute beamPAWasFixed, which is optional
	
	
	bool beamPAWasFixedExists;
	

	bool beamPAWasFixed;

	
	
 	

	
	// ===> Attribute beamWidth, which is optional
	
	
	bool beamWidthExists;
	

	std::vector<std::vector<Angle > > beamWidth;

	
	
 	

	
	// ===> Attribute beamWidthError, which is optional
	
	
	bool beamWidthErrorExists;
	

	std::vector<std::vector<Angle > > beamWidthError;

	
	
 	

	
	// ===> Attribute beamWidthWasFixed, which is optional
	
	
	bool beamWidthWasFixedExists;
	

	std::vector<bool > beamWidthWasFixed;

	
	
 	

	
	// ===> Attribute offIntensity, which is optional
	
	
	bool offIntensityExists;
	

	std::vector<Temperature > offIntensity;

	
	
 	

	
	// ===> Attribute offIntensityError, which is optional
	
	
	bool offIntensityErrorExists;
	

	std::vector<Temperature > offIntensityError;

	
	
 	

	
	// ===> Attribute offIntensityWasFixed, which is optional
	
	
	bool offIntensityWasFixedExists;
	

	bool offIntensityWasFixed;

	
	
 	

	
	// ===> Attribute peakIntensity, which is optional
	
	
	bool peakIntensityExists;
	

	std::vector<Temperature > peakIntensity;

	
	
 	

	
	// ===> Attribute peakIntensityError, which is optional
	
	
	bool peakIntensityErrorExists;
	

	std::vector<Temperature > peakIntensityError;

	
	
 	

	
	// ===> Attribute peakIntensityWasFixed, which is optional
	
	
	bool peakIntensityWasFixedExists;
	

	bool peakIntensityWasFixed;

	
	
 	

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
	std::map<std::string, CalPointingAttributeFromBin> fromBinMethods;
void antennaNameFromBin( EndianIStream& eis);
void receiverBandFromBin( EndianIStream& eis);
void calDataIdFromBin( EndianIStream& eis);
void calReductionIdFromBin( EndianIStream& eis);
void startValidTimeFromBin( EndianIStream& eis);
void endValidTimeFromBin( EndianIStream& eis);
void ambientTemperatureFromBin( EndianIStream& eis);
void antennaMakeFromBin( EndianIStream& eis);
void atmPhaseCorrectionFromBin( EndianIStream& eis);
void directionFromBin( EndianIStream& eis);
void frequencyRangeFromBin( EndianIStream& eis);
void pointingModelModeFromBin( EndianIStream& eis);
void pointingMethodFromBin( EndianIStream& eis);
void numReceptorFromBin( EndianIStream& eis);
void polarizationTypesFromBin( EndianIStream& eis);
void collOffsetRelativeFromBin( EndianIStream& eis);
void collOffsetAbsoluteFromBin( EndianIStream& eis);
void collErrorFromBin( EndianIStream& eis);
void collOffsetTiedFromBin( EndianIStream& eis);
void reducedChiSquaredFromBin( EndianIStream& eis);

void averagedPolarizationsFromBin( EndianIStream& eis);
void beamPAFromBin( EndianIStream& eis);
void beamPAErrorFromBin( EndianIStream& eis);
void beamPAWasFixedFromBin( EndianIStream& eis);
void beamWidthFromBin( EndianIStream& eis);
void beamWidthErrorFromBin( EndianIStream& eis);
void beamWidthWasFixedFromBin( EndianIStream& eis);
void offIntensityFromBin( EndianIStream& eis);
void offIntensityErrorFromBin( EndianIStream& eis);
void offIntensityWasFixedFromBin( EndianIStream& eis);
void peakIntensityFromBin( EndianIStream& eis);
void peakIntensityErrorFromBin( EndianIStream& eis);
void peakIntensityWasFixedFromBin( EndianIStream& eis);

*/
	
	///////////////////////////////////
	// text-deserialization material //
	///////////////////////////////////
	std::map<std::string, CalPointingAttributeFromText> fromTextMethods;
	
void antennaNameFromText (const std::string & s);
	
	
void receiverBandFromText (const std::string & s);
	
	
void calDataIdFromText (const std::string & s);
	
	
void calReductionIdFromText (const std::string & s);
	
	
void startValidTimeFromText (const std::string & s);
	
	
void endValidTimeFromText (const std::string & s);
	
	
void ambientTemperatureFromText (const std::string & s);
	
	
void antennaMakeFromText (const std::string & s);
	
	
void atmPhaseCorrectionFromText (const std::string & s);
	
	
void directionFromText (const std::string & s);
	
	
void frequencyRangeFromText (const std::string & s);
	
	
void pointingModelModeFromText (const std::string & s);
	
	
void pointingMethodFromText (const std::string & s);
	
	
void numReceptorFromText (const std::string & s);
	
	
void polarizationTypesFromText (const std::string & s);
	
	
void collOffsetRelativeFromText (const std::string & s);
	
	
void collOffsetAbsoluteFromText (const std::string & s);
	
	
void collErrorFromText (const std::string & s);
	
	
void collOffsetTiedFromText (const std::string & s);
	
	
void reducedChiSquaredFromText (const std::string & s);
	

	
void averagedPolarizationsFromText (const std::string & s);
	
	
void beamPAFromText (const std::string & s);
	
	
void beamPAErrorFromText (const std::string & s);
	
	
void beamPAWasFixedFromText (const std::string & s);
	
	
void beamWidthFromText (const std::string & s);
	
	
void beamWidthErrorFromText (const std::string & s);
	
	
void beamWidthWasFixedFromText (const std::string & s);
	
	
void offIntensityFromText (const std::string & s);
	
	
void offIntensityErrorFromText (const std::string & s);
	
	
void offIntensityWasFixedFromText (const std::string & s);
	
	
void peakIntensityFromText (const std::string & s);
	
	
void peakIntensityErrorFromText (const std::string & s);
	
	
void peakIntensityWasFixedFromText (const std::string & s);
	
	
	
	/**
	 * Serialize this into a stream of bytes written to an EndianOSStream.
	 * @param eoss the EndianOSStream to be written to
	 */
	 void toBin(EndianOSStream& eoss);
	 	 
	 /**
	  * Deserialize a stream of bytes read from an EndianIStream to build a PointingRow.
	  * @param eiss the EndianIStream to be read.
	  * @param table the CalPointingTable to which the row built by deserialization will be parented.
	  * @param attributesSeq a vector containing the names of the attributes . The elements order defines the order 
	  * in which the attributes are written in the binary serialization.

	 static CalPointingRow* fromBin(EndianIStream& eis, CalPointingTable& table, const std::vector<std::string>& attributesSeq);	 
		*/
};

} // End namespace asdm

#endif /* CalPointing_CLASS */
