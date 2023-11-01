
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
 * File CalFocusRow.h
 */
 
#ifndef CalFocusRow_CLASS
#define CalFocusRow_CLASS

#include <vector>
#include <string>
#include <set>

#ifndef WITHOUT_ACS
#include <asdmIDLC.h>
#endif






	 
#include <alma/ASDM/Temperature.h>
	

	 
#include <alma/ASDM/ArrayTime.h>
	

	 
#include <alma/ASDM/Angle.h>
	

	 
#include <alma/ASDM/Length.h>
	

	 
#include <alma/ASDM/Frequency.h>
	

	 
#include <alma/ASDM/Tag.h>
	




	

	

	

	
#include <alma/Enumerations/CReceiverBand.h>
	

	

	
#include <alma/Enumerations/CAtmPhaseCorrection.h>
	

	
#include <alma/Enumerations/CFocusMethod.h>
	

	

	

	

	
#include <alma/Enumerations/CPolarizationType.h>
	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	



#include <alma/ASDM/ConversionException.h>
#include <alma/ASDM/NoSuchRow.h>
#include <alma/ASDM/IllegalAccessException.h>

#include <alma/ASDM/RowTransformer.h>
//#include <alma/ASDM/TableStreamReader.h>

/*\file CalFocus.h
    \brief Generated from model's revision "-1", branch ""
*/

namespace asdm {

//class asdm::CalFocusTable;


// class asdm::CalDataRow;
class CalDataRow;

// class asdm::CalReductionRow;
class CalReductionRow;
	

class CalFocusRow;
typedef void (CalFocusRow::*CalFocusAttributeFromBin) (EndianIStream& eis);
typedef void (CalFocusRow::*CalFocusAttributeFromText) (const std::string& s);

/**
 * The CalFocusRow class is a row of a CalFocusTable.
 * 
 * Generated from model's revision "-1", branch ""
 *
 */
class CalFocusRow {
friend class asdm::CalFocusTable;
friend class asdm::RowTransformer<CalFocusRow>;
//friend class asdm::TableStreamReader<CalFocusTable, CalFocusRow>;

public:

	virtual ~CalFocusRow();

	/**
	 * Return the table to which this row belongs.
	 */
	CalFocusTable &getTable() const;
	
	/**
	 * Has this row been added to its table ?
	 * @return true if and only if it has been added.
	 */
	bool isAdded() const;
		
	////////////////////////////////
	// Intrinsic Table Attributes //
	////////////////////////////////
	
	
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
  		
	
	
	


	
	// ===> Attribute focusMethod
	
	
	

	
 	/**
 	 * Get focusMethod.
 	 * @return focusMethod as FocusMethodMod::FocusMethod
 	 */
 	FocusMethodMod::FocusMethod getFocusMethod() const;
	
 
 	
 	
 	/**
 	 * Set focusMethod with the specified FocusMethodMod::FocusMethod.
 	 * @param focusMethod The FocusMethodMod::FocusMethod value to which focusMethod is to be set.
 	 
 		
 			
 	 */
 	void setFocusMethod (FocusMethodMod::FocusMethod focusMethod);
  		
	
	
	


	
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
  		
	
	
	


	
	// ===> Attribute pointingDirection
	
	
	

	
 	/**
 	 * Get pointingDirection.
 	 * @return pointingDirection as std::vector<Angle >
 	 */
 	std::vector<Angle > getPointingDirection() const;
	
 
 	
 	
 	/**
 	 * Set pointingDirection with the specified std::vector<Angle >.
 	 * @param pointingDirection The std::vector<Angle > value to which pointingDirection is to be set.
 	 
 		
 			
 	 */
 	void setPointingDirection (std::vector<Angle > pointingDirection);
  		
	
	
	


	
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
  		
	
	
	


	
	// ===> Attribute wereFixed
	
	
	

	
 	/**
 	 * Get wereFixed.
 	 * @return wereFixed as std::vector<bool >
 	 */
 	std::vector<bool > getWereFixed() const;
	
 
 	
 	
 	/**
 	 * Set wereFixed with the specified std::vector<bool >.
 	 * @param wereFixed The std::vector<bool > value to which wereFixed is to be set.
 	 
 		
 			
 	 */
 	void setWereFixed (std::vector<bool > wereFixed);
  		
	
	
	


	
	// ===> Attribute offset
	
	
	

	
 	/**
 	 * Get offset.
 	 * @return offset as std::vector<std::vector<Length > >
 	 */
 	std::vector<std::vector<Length > > getOffset() const;
	
 
 	
 	
 	/**
 	 * Set offset with the specified std::vector<std::vector<Length > >.
 	 * @param offset The std::vector<std::vector<Length > > value to which offset is to be set.
 	 
 		
 			
 	 */
 	void setOffset (std::vector<std::vector<Length > > offset);
  		
	
	
	


	
	// ===> Attribute offsetError
	
	
	

	
 	/**
 	 * Get offsetError.
 	 * @return offsetError as std::vector<std::vector<Length > >
 	 */
 	std::vector<std::vector<Length > > getOffsetError() const;
	
 
 	
 	
 	/**
 	 * Set offsetError with the specified std::vector<std::vector<Length > >.
 	 * @param offsetError The std::vector<std::vector<Length > > value to which offsetError is to be set.
 	 
 		
 			
 	 */
 	void setOffsetError (std::vector<std::vector<Length > > offsetError);
  		
	
	
	


	
	// ===> Attribute offsetWasTied
	
	
	

	
 	/**
 	 * Get offsetWasTied.
 	 * @return offsetWasTied as std::vector<std::vector<bool > >
 	 */
 	std::vector<std::vector<bool > > getOffsetWasTied() const;
	
 
 	
 	
 	/**
 	 * Set offsetWasTied with the specified std::vector<std::vector<bool > >.
 	 * @param offsetWasTied The std::vector<std::vector<bool > > value to which offsetWasTied is to be set.
 	 
 		
 			
 	 */
 	void setOffsetWasTied (std::vector<std::vector<bool > > offsetWasTied);
  		
	
	
	


	
	// ===> Attribute reducedChiSquared
	
	
	

	
 	/**
 	 * Get reducedChiSquared.
 	 * @return reducedChiSquared as std::vector<std::vector<double > >
 	 */
 	std::vector<std::vector<double > > getReducedChiSquared() const;
	
 
 	
 	
 	/**
 	 * Set reducedChiSquared with the specified std::vector<std::vector<double > >.
 	 * @param reducedChiSquared The std::vector<std::vector<double > > value to which reducedChiSquared is to be set.
 	 
 		
 			
 	 */
 	void setReducedChiSquared (std::vector<std::vector<double > > reducedChiSquared);
  		
	
	
	


	
	// ===> Attribute position
	
	
	

	
 	/**
 	 * Get position.
 	 * @return position as std::vector<std::vector<Length > >
 	 */
 	std::vector<std::vector<Length > > getPosition() const;
	
 
 	
 	
 	/**
 	 * Set position with the specified std::vector<std::vector<Length > >.
 	 * @param position The std::vector<std::vector<Length > > value to which position is to be set.
 	 
 		
 			
 	 */
 	void setPosition (std::vector<std::vector<Length > > position);
  		
	
	
	


	
	// ===> Attribute polarizationsAveraged, which is optional
	
	
	
	/**
	 * The attribute polarizationsAveraged is optional. Return true if this attribute exists.
	 * @return true if and only if the polarizationsAveraged attribute exists. 
	 */
	bool isPolarizationsAveragedExists() const;
	

	
 	/**
 	 * Get polarizationsAveraged, which is optional.
 	 * @return polarizationsAveraged as bool
 	 * @throws IllegalAccessException If polarizationsAveraged does not exist.
 	 */
 	bool getPolarizationsAveraged() const;
	
 
 	
 	
 	/**
 	 * Set polarizationsAveraged with the specified bool.
 	 * @param polarizationsAveraged The bool value to which polarizationsAveraged is to be set.
 	 
 		
 	 */
 	void setPolarizationsAveraged (bool polarizationsAveraged);
		
	
	
	
	/**
	 * Mark polarizationsAveraged, which is an optional field, as non-existent.
	 */
	void clearPolarizationsAveraged ();
	


	
	// ===> Attribute focusCurveWidth, which is optional
	
	
	
	/**
	 * The attribute focusCurveWidth is optional. Return true if this attribute exists.
	 * @return true if and only if the focusCurveWidth attribute exists. 
	 */
	bool isFocusCurveWidthExists() const;
	

	
 	/**
 	 * Get focusCurveWidth, which is optional.
 	 * @return focusCurveWidth as std::vector<std::vector<Length > >
 	 * @throws IllegalAccessException If focusCurveWidth does not exist.
 	 */
 	std::vector<std::vector<Length > > getFocusCurveWidth() const;
	
 
 	
 	
 	/**
 	 * Set focusCurveWidth with the specified std::vector<std::vector<Length > >.
 	 * @param focusCurveWidth The std::vector<std::vector<Length > > value to which focusCurveWidth is to be set.
 	 
 		
 	 */
 	void setFocusCurveWidth (std::vector<std::vector<Length > > focusCurveWidth);
		
	
	
	
	/**
	 * Mark focusCurveWidth, which is an optional field, as non-existent.
	 */
	void clearFocusCurveWidth ();
	


	
	// ===> Attribute focusCurveWidthError, which is optional
	
	
	
	/**
	 * The attribute focusCurveWidthError is optional. Return true if this attribute exists.
	 * @return true if and only if the focusCurveWidthError attribute exists. 
	 */
	bool isFocusCurveWidthErrorExists() const;
	

	
 	/**
 	 * Get focusCurveWidthError, which is optional.
 	 * @return focusCurveWidthError as std::vector<std::vector<Length > >
 	 * @throws IllegalAccessException If focusCurveWidthError does not exist.
 	 */
 	std::vector<std::vector<Length > > getFocusCurveWidthError() const;
	
 
 	
 	
 	/**
 	 * Set focusCurveWidthError with the specified std::vector<std::vector<Length > >.
 	 * @param focusCurveWidthError The std::vector<std::vector<Length > > value to which focusCurveWidthError is to be set.
 	 
 		
 	 */
 	void setFocusCurveWidthError (std::vector<std::vector<Length > > focusCurveWidthError);
		
	
	
	
	/**
	 * Mark focusCurveWidthError, which is an optional field, as non-existent.
	 */
	void clearFocusCurveWidthError ();
	


	
	// ===> Attribute focusCurveWasFixed, which is optional
	
	
	
	/**
	 * The attribute focusCurveWasFixed is optional. Return true if this attribute exists.
	 * @return true if and only if the focusCurveWasFixed attribute exists. 
	 */
	bool isFocusCurveWasFixedExists() const;
	

	
 	/**
 	 * Get focusCurveWasFixed, which is optional.
 	 * @return focusCurveWasFixed as std::vector<bool >
 	 * @throws IllegalAccessException If focusCurveWasFixed does not exist.
 	 */
 	std::vector<bool > getFocusCurveWasFixed() const;
	
 
 	
 	
 	/**
 	 * Set focusCurveWasFixed with the specified std::vector<bool >.
 	 * @param focusCurveWasFixed The std::vector<bool > value to which focusCurveWasFixed is to be set.
 	 
 		
 	 */
 	void setFocusCurveWasFixed (std::vector<bool > focusCurveWasFixed);
		
	
	
	
	/**
	 * Mark focusCurveWasFixed, which is an optional field, as non-existent.
	 */
	void clearFocusCurveWasFixed ();
	


	
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
	


	
	// ===> Attribute astigmPlus, which is optional
	
	
	
	/**
	 * The attribute astigmPlus is optional. Return true if this attribute exists.
	 * @return true if and only if the astigmPlus attribute exists. 
	 */
	bool isAstigmPlusExists() const;
	

	
 	/**
 	 * Get astigmPlus, which is optional.
 	 * @return astigmPlus as std::vector<Length >
 	 * @throws IllegalAccessException If astigmPlus does not exist.
 	 */
 	std::vector<Length > getAstigmPlus() const;
	
 
 	
 	
 	/**
 	 * Set astigmPlus with the specified std::vector<Length >.
 	 * @param astigmPlus The std::vector<Length > value to which astigmPlus is to be set.
 	 
 		
 	 */
 	void setAstigmPlus (std::vector<Length > astigmPlus);
		
	
	
	
	/**
	 * Mark astigmPlus, which is an optional field, as non-existent.
	 */
	void clearAstigmPlus ();
	


	
	// ===> Attribute astigmPlusError, which is optional
	
	
	
	/**
	 * The attribute astigmPlusError is optional. Return true if this attribute exists.
	 * @return true if and only if the astigmPlusError attribute exists. 
	 */
	bool isAstigmPlusErrorExists() const;
	

	
 	/**
 	 * Get astigmPlusError, which is optional.
 	 * @return astigmPlusError as std::vector<Length >
 	 * @throws IllegalAccessException If astigmPlusError does not exist.
 	 */
 	std::vector<Length > getAstigmPlusError() const;
	
 
 	
 	
 	/**
 	 * Set astigmPlusError with the specified std::vector<Length >.
 	 * @param astigmPlusError The std::vector<Length > value to which astigmPlusError is to be set.
 	 
 		
 	 */
 	void setAstigmPlusError (std::vector<Length > astigmPlusError);
		
	
	
	
	/**
	 * Mark astigmPlusError, which is an optional field, as non-existent.
	 */
	void clearAstigmPlusError ();
	


	
	// ===> Attribute astigmMult, which is optional
	
	
	
	/**
	 * The attribute astigmMult is optional. Return true if this attribute exists.
	 * @return true if and only if the astigmMult attribute exists. 
	 */
	bool isAstigmMultExists() const;
	

	
 	/**
 	 * Get astigmMult, which is optional.
 	 * @return astigmMult as std::vector<Length >
 	 * @throws IllegalAccessException If astigmMult does not exist.
 	 */
 	std::vector<Length > getAstigmMult() const;
	
 
 	
 	
 	/**
 	 * Set astigmMult with the specified std::vector<Length >.
 	 * @param astigmMult The std::vector<Length > value to which astigmMult is to be set.
 	 
 		
 	 */
 	void setAstigmMult (std::vector<Length > astigmMult);
		
	
	
	
	/**
	 * Mark astigmMult, which is an optional field, as non-existent.
	 */
	void clearAstigmMult ();
	


	
	// ===> Attribute astigmMultError, which is optional
	
	
	
	/**
	 * The attribute astigmMultError is optional. Return true if this attribute exists.
	 * @return true if and only if the astigmMultError attribute exists. 
	 */
	bool isAstigmMultErrorExists() const;
	

	
 	/**
 	 * Get astigmMultError, which is optional.
 	 * @return astigmMultError as std::vector<Length >
 	 * @throws IllegalAccessException If astigmMultError does not exist.
 	 */
 	std::vector<Length > getAstigmMultError() const;
	
 
 	
 	
 	/**
 	 * Set astigmMultError with the specified std::vector<Length >.
 	 * @param astigmMultError The std::vector<Length > value to which astigmMultError is to be set.
 	 
 		
 	 */
 	void setAstigmMultError (std::vector<Length > astigmMultError);
		
	
	
	
	/**
	 * Mark astigmMultError, which is an optional field, as non-existent.
	 */
	void clearAstigmMultError ();
	


	
	// ===> Attribute illumOffset, which is optional
	
	
	
	/**
	 * The attribute illumOffset is optional. Return true if this attribute exists.
	 * @return true if and only if the illumOffset attribute exists. 
	 */
	bool isIllumOffsetExists() const;
	

	
 	/**
 	 * Get illumOffset, which is optional.
 	 * @return illumOffset as std::vector<std::vector<Length > >
 	 * @throws IllegalAccessException If illumOffset does not exist.
 	 */
 	std::vector<std::vector<Length > > getIllumOffset() const;
	
 
 	
 	
 	/**
 	 * Set illumOffset with the specified std::vector<std::vector<Length > >.
 	 * @param illumOffset The std::vector<std::vector<Length > > value to which illumOffset is to be set.
 	 
 		
 	 */
 	void setIllumOffset (std::vector<std::vector<Length > > illumOffset);
		
	
	
	
	/**
	 * Mark illumOffset, which is an optional field, as non-existent.
	 */
	void clearIllumOffset ();
	


	
	// ===> Attribute illumOffsetError, which is optional
	
	
	
	/**
	 * The attribute illumOffsetError is optional. Return true if this attribute exists.
	 * @return true if and only if the illumOffsetError attribute exists. 
	 */
	bool isIllumOffsetErrorExists() const;
	

	
 	/**
 	 * Get illumOffsetError, which is optional.
 	 * @return illumOffsetError as std::vector<std::vector<Length > >
 	 * @throws IllegalAccessException If illumOffsetError does not exist.
 	 */
 	std::vector<std::vector<Length > > getIllumOffsetError() const;
	
 
 	
 	
 	/**
 	 * Set illumOffsetError with the specified std::vector<std::vector<Length > >.
 	 * @param illumOffsetError The std::vector<std::vector<Length > > value to which illumOffsetError is to be set.
 	 
 		
 	 */
 	void setIllumOffsetError (std::vector<std::vector<Length > > illumOffsetError);
		
	
	
	
	/**
	 * Mark illumOffsetError, which is an optional field, as non-existent.
	 */
	void clearIllumOffsetError ();
	


	
	// ===> Attribute fitRMS, which is optional
	
	
	
	/**
	 * The attribute fitRMS is optional. Return true if this attribute exists.
	 * @return true if and only if the fitRMS attribute exists. 
	 */
	bool isFitRMSExists() const;
	

	
 	/**
 	 * Get fitRMS, which is optional.
 	 * @return fitRMS as std::vector<Length >
 	 * @throws IllegalAccessException If fitRMS does not exist.
 	 */
 	std::vector<Length > getFitRMS() const;
	
 
 	
 	
 	/**
 	 * Set fitRMS with the specified std::vector<Length >.
 	 * @param fitRMS The std::vector<Length > value to which fitRMS is to be set.
 	 
 		
 	 */
 	void setFitRMS (std::vector<Length > fitRMS);
		
	
	
	
	/**
	 * Mark fitRMS, which is an optional field, as non-existent.
	 */
	void clearFitRMS ();
	


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
	 * Compare each mandatory attribute except the autoincrementable one of this CalFocusRow with 
	 * the corresponding parameters and return true if there is a match and false otherwise.
	 	
	 * @param antennaName
	    
	 * @param receiverBand
	    
	 * @param calDataId
	    
	 * @param calReductionId
	    
	 * @param startValidTime
	    
	 * @param endValidTime
	    
	 * @param ambientTemperature
	    
	 * @param atmPhaseCorrection
	    
	 * @param focusMethod
	    
	 * @param frequencyRange
	    
	 * @param pointingDirection
	    
	 * @param numReceptor
	    
	 * @param polarizationTypes
	    
	 * @param wereFixed
	    
	 * @param offset
	    
	 * @param offsetError
	    
	 * @param offsetWasTied
	    
	 * @param reducedChiSquared
	    
	 * @param position
	    
	 */ 
	bool compareNoAutoInc(std::string antennaName, ReceiverBandMod::ReceiverBand receiverBand, Tag calDataId, Tag calReductionId, ArrayTime startValidTime, ArrayTime endValidTime, Temperature ambientTemperature, AtmPhaseCorrectionMod::AtmPhaseCorrection atmPhaseCorrection, FocusMethodMod::FocusMethod focusMethod, std::vector<Frequency > frequencyRange, std::vector<Angle > pointingDirection, int numReceptor, std::vector<PolarizationTypeMod::PolarizationType > polarizationTypes, std::vector<bool > wereFixed, std::vector<std::vector<Length > > offset, std::vector<std::vector<Length > > offsetError, std::vector<std::vector<bool > > offsetWasTied, std::vector<std::vector<double > > reducedChiSquared, std::vector<std::vector<Length > > position);
	
	

	
	/**
	 * Compare each mandatory value (i.e. not in the key) attribute  with 
	 * the corresponding parameters and return true if there is a match and false otherwise.
	 	
	 * @param startValidTime
	    
	 * @param endValidTime
	    
	 * @param ambientTemperature
	    
	 * @param atmPhaseCorrection
	    
	 * @param focusMethod
	    
	 * @param frequencyRange
	    
	 * @param pointingDirection
	    
	 * @param numReceptor
	    
	 * @param polarizationTypes
	    
	 * @param wereFixed
	    
	 * @param offset
	    
	 * @param offsetError
	    
	 * @param offsetWasTied
	    
	 * @param reducedChiSquared
	    
	 * @param position
	    
	 */ 
	bool compareRequiredValue(ArrayTime startValidTime, ArrayTime endValidTime, Temperature ambientTemperature, AtmPhaseCorrectionMod::AtmPhaseCorrection atmPhaseCorrection, FocusMethodMod::FocusMethod focusMethod, std::vector<Frequency > frequencyRange, std::vector<Angle > pointingDirection, int numReceptor, std::vector<PolarizationTypeMod::PolarizationType > polarizationTypes, std::vector<bool > wereFixed, std::vector<std::vector<Length > > offset, std::vector<std::vector<Length > > offsetError, std::vector<std::vector<bool > > offsetWasTied, std::vector<std::vector<double > > reducedChiSquared, std::vector<std::vector<Length > > position); 
		 
	
	/**
	 * Return true if all required attributes of the value part are equal to their homologues
	 * in x and false otherwise.
	 *
	 * @param x a pointer on the CalFocusRow whose required attributes of the value part 
	 * will be compared with those of this.
	 * @return a boolean.
	 */
	bool equalByRequiredValue(CalFocusRow* x) ;
	
#ifndef WITHOUT_ACS
	/**
	 * Return this row in the form of an IDL struct.
	 * @return The values of this row as a CalFocusRowIDL struct.
	 */
	asdmIDL::CalFocusRowIDL *toIDL() const;
	
	/**
	 * Define the content of a CalFocusRowIDL struct from the values
	 * found in this row.
	 *
	 * @param x a reference to the CalFocusRowIDL struct to be set.
	 *
	 */
	 void toIDL(asdmIDL::CalFocusRowIDL& x) const;
#endif
	
#ifndef WITHOUT_ACS
	/**
	 * Fill the values of this row from the IDL struct CalFocusRowIDL.
	 * @param x The IDL struct containing the values used to fill this row.
	 * @throws ConversionException
	 */
	void setFromIDL (asdmIDL::CalFocusRowIDL x) ;
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

	std::map<std::string, CalFocusAttributeFromBin> fromBinMethods;
void antennaNameFromBin( EndianIStream& eis);
void receiverBandFromBin( EndianIStream& eis);
void calDataIdFromBin( EndianIStream& eis);
void calReductionIdFromBin( EndianIStream& eis);
void startValidTimeFromBin( EndianIStream& eis);
void endValidTimeFromBin( EndianIStream& eis);
void ambientTemperatureFromBin( EndianIStream& eis);
void atmPhaseCorrectionFromBin( EndianIStream& eis);
void focusMethodFromBin( EndianIStream& eis);
void frequencyRangeFromBin( EndianIStream& eis);
void pointingDirectionFromBin( EndianIStream& eis);
void numReceptorFromBin( EndianIStream& eis);
void polarizationTypesFromBin( EndianIStream& eis);
void wereFixedFromBin( EndianIStream& eis);
void offsetFromBin( EndianIStream& eis);
void offsetErrorFromBin( EndianIStream& eis);
void offsetWasTiedFromBin( EndianIStream& eis);
void reducedChiSquaredFromBin( EndianIStream& eis);
void positionFromBin( EndianIStream& eis);

void polarizationsAveragedFromBin( EndianIStream& eis);
void focusCurveWidthFromBin( EndianIStream& eis);
void focusCurveWidthErrorFromBin( EndianIStream& eis);
void focusCurveWasFixedFromBin( EndianIStream& eis);
void offIntensityFromBin( EndianIStream& eis);
void offIntensityErrorFromBin( EndianIStream& eis);
void offIntensityWasFixedFromBin( EndianIStream& eis);
void peakIntensityFromBin( EndianIStream& eis);
void peakIntensityErrorFromBin( EndianIStream& eis);
void peakIntensityWasFixedFromBin( EndianIStream& eis);
void astigmPlusFromBin( EndianIStream& eis);
void astigmPlusErrorFromBin( EndianIStream& eis);
void astigmMultFromBin( EndianIStream& eis);
void astigmMultErrorFromBin( EndianIStream& eis);
void illumOffsetFromBin( EndianIStream& eis);
void illumOffsetErrorFromBin( EndianIStream& eis);
void fitRMSFromBin( EndianIStream& eis);


	 /**
	  * Deserialize a stream of bytes read from an EndianIStream to build a PointingRow.
	  * @param eiss the EndianIStream to be read.
	  * @param table the CalFocusTable to which the row built by deserialization will be parented.
	  * @param attributesSeq a vector containing the names of the attributes . The elements order defines the order 
	  * in which the attributes are written in the binary serialization.
	  */
	 static CalFocusRow* fromBin(EndianIStream& eis, CalFocusTable& table, const std::vector<std::string>& attributesSeq);	 
 
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
	CalFocusTable &table;
	/**
	 * Whether this row has been added to the table or not.
	 */
	bool hasBeenAdded;

	// This method is used by the Table class when this row is added to the table.
	void isAdded(bool added);


	/**
	 * Create a CalFocusRow.
	 * <p>
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 */ 
	CalFocusRow (CalFocusTable &table);

	/**
	 * Create a CalFocusRow using a copy constructor mechanism.
	 * <p>
	 * Given a CalFocusRow row and a CalFocusTable table, the method creates a new
	 * CalFocusRow owned by table. Each attribute of the created row is a copy (deep)
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
	 CalFocusRow (CalFocusTable &table, CalFocusRow *row);
	 	
	////////////////////////////////
	// Intrinsic Table Attributes //
	////////////////////////////////
	
	
	// ===> Attribute startValidTime
	
	

	ArrayTime startValidTime;

	
	
 	

	
	// ===> Attribute endValidTime
	
	

	ArrayTime endValidTime;

	
	
 	

	
	// ===> Attribute antennaName
	
	

	std::string antennaName;

	
	
 	

	
	// ===> Attribute receiverBand
	
	

	ReceiverBandMod::ReceiverBand receiverBand;

	
	
 	

	
	// ===> Attribute ambientTemperature
	
	

	Temperature ambientTemperature;

	
	
 	

	
	// ===> Attribute atmPhaseCorrection
	
	

	AtmPhaseCorrectionMod::AtmPhaseCorrection atmPhaseCorrection;

	
	
 	

	
	// ===> Attribute focusMethod
	
	

	FocusMethodMod::FocusMethod focusMethod;

	
	
 	

	
	// ===> Attribute frequencyRange
	
	

	std::vector<Frequency > frequencyRange;

	
	
 	

	
	// ===> Attribute pointingDirection
	
	

	std::vector<Angle > pointingDirection;

	
	
 	

	
	// ===> Attribute numReceptor
	
	

	int numReceptor;

	
	
 	

	
	// ===> Attribute polarizationTypes
	
	

	std::vector<PolarizationTypeMod::PolarizationType > polarizationTypes;

	
	
 	

	
	// ===> Attribute wereFixed
	
	

	std::vector<bool > wereFixed;

	
	
 	

	
	// ===> Attribute offset
	
	

	std::vector<std::vector<Length > > offset;

	
	
 	

	
	// ===> Attribute offsetError
	
	

	std::vector<std::vector<Length > > offsetError;

	
	
 	

	
	// ===> Attribute offsetWasTied
	
	

	std::vector<std::vector<bool > > offsetWasTied;

	
	
 	

	
	// ===> Attribute reducedChiSquared
	
	

	std::vector<std::vector<double > > reducedChiSquared;

	
	
 	

	
	// ===> Attribute position
	
	

	std::vector<std::vector<Length > > position;

	
	
 	

	
	// ===> Attribute polarizationsAveraged, which is optional
	
	
	bool polarizationsAveragedExists;
	

	bool polarizationsAveraged;

	
	
 	

	
	// ===> Attribute focusCurveWidth, which is optional
	
	
	bool focusCurveWidthExists;
	

	std::vector<std::vector<Length > > focusCurveWidth;

	
	
 	

	
	// ===> Attribute focusCurveWidthError, which is optional
	
	
	bool focusCurveWidthErrorExists;
	

	std::vector<std::vector<Length > > focusCurveWidthError;

	
	
 	

	
	// ===> Attribute focusCurveWasFixed, which is optional
	
	
	bool focusCurveWasFixedExists;
	

	std::vector<bool > focusCurveWasFixed;

	
	
 	

	
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

	
	
 	

	
	// ===> Attribute astigmPlus, which is optional
	
	
	bool astigmPlusExists;
	

	std::vector<Length > astigmPlus;

	
	
 	

	
	// ===> Attribute astigmPlusError, which is optional
	
	
	bool astigmPlusErrorExists;
	

	std::vector<Length > astigmPlusError;

	
	
 	

	
	// ===> Attribute astigmMult, which is optional
	
	
	bool astigmMultExists;
	

	std::vector<Length > astigmMult;

	
	
 	

	
	// ===> Attribute astigmMultError, which is optional
	
	
	bool astigmMultErrorExists;
	

	std::vector<Length > astigmMultError;

	
	
 	

	
	// ===> Attribute illumOffset, which is optional
	
	
	bool illumOffsetExists;
	

	std::vector<std::vector<Length > > illumOffset;

	
	
 	

	
	// ===> Attribute illumOffsetError, which is optional
	
	
	bool illumOffsetErrorExists;
	

	std::vector<std::vector<Length > > illumOffsetError;

	
	
 	

	
	// ===> Attribute fitRMS, which is optional
	
	
	bool fitRMSExists;
	

	std::vector<Length > fitRMS;

	
	
 	

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
	std::map<std::string, CalFocusAttributeFromBin> fromBinMethods;
void antennaNameFromBin( EndianIStream& eis);
void receiverBandFromBin( EndianIStream& eis);
void calDataIdFromBin( EndianIStream& eis);
void calReductionIdFromBin( EndianIStream& eis);
void startValidTimeFromBin( EndianIStream& eis);
void endValidTimeFromBin( EndianIStream& eis);
void ambientTemperatureFromBin( EndianIStream& eis);
void atmPhaseCorrectionFromBin( EndianIStream& eis);
void focusMethodFromBin( EndianIStream& eis);
void frequencyRangeFromBin( EndianIStream& eis);
void pointingDirectionFromBin( EndianIStream& eis);
void numReceptorFromBin( EndianIStream& eis);
void polarizationTypesFromBin( EndianIStream& eis);
void wereFixedFromBin( EndianIStream& eis);
void offsetFromBin( EndianIStream& eis);
void offsetErrorFromBin( EndianIStream& eis);
void offsetWasTiedFromBin( EndianIStream& eis);
void reducedChiSquaredFromBin( EndianIStream& eis);
void positionFromBin( EndianIStream& eis);

void polarizationsAveragedFromBin( EndianIStream& eis);
void focusCurveWidthFromBin( EndianIStream& eis);
void focusCurveWidthErrorFromBin( EndianIStream& eis);
void focusCurveWasFixedFromBin( EndianIStream& eis);
void offIntensityFromBin( EndianIStream& eis);
void offIntensityErrorFromBin( EndianIStream& eis);
void offIntensityWasFixedFromBin( EndianIStream& eis);
void peakIntensityFromBin( EndianIStream& eis);
void peakIntensityErrorFromBin( EndianIStream& eis);
void peakIntensityWasFixedFromBin( EndianIStream& eis);
void astigmPlusFromBin( EndianIStream& eis);
void astigmPlusErrorFromBin( EndianIStream& eis);
void astigmMultFromBin( EndianIStream& eis);
void astigmMultErrorFromBin( EndianIStream& eis);
void illumOffsetFromBin( EndianIStream& eis);
void illumOffsetErrorFromBin( EndianIStream& eis);
void fitRMSFromBin( EndianIStream& eis);

*/
	
	///////////////////////////////////
	// text-deserialization material //
	///////////////////////////////////
	std::map<std::string, CalFocusAttributeFromText> fromTextMethods;
	
void antennaNameFromText (const std::string & s);
	
	
void receiverBandFromText (const std::string & s);
	
	
void calDataIdFromText (const std::string & s);
	
	
void calReductionIdFromText (const std::string & s);
	
	
void startValidTimeFromText (const std::string & s);
	
	
void endValidTimeFromText (const std::string & s);
	
	
void ambientTemperatureFromText (const std::string & s);
	
	
void atmPhaseCorrectionFromText (const std::string & s);
	
	
void focusMethodFromText (const std::string & s);
	
	
void frequencyRangeFromText (const std::string & s);
	
	
void pointingDirectionFromText (const std::string & s);
	
	
void numReceptorFromText (const std::string & s);
	
	
void polarizationTypesFromText (const std::string & s);
	
	
void wereFixedFromText (const std::string & s);
	
	
void offsetFromText (const std::string & s);
	
	
void offsetErrorFromText (const std::string & s);
	
	
void offsetWasTiedFromText (const std::string & s);
	
	
void reducedChiSquaredFromText (const std::string & s);
	
	
void positionFromText (const std::string & s);
	

	
void polarizationsAveragedFromText (const std::string & s);
	
	
void focusCurveWidthFromText (const std::string & s);
	
	
void focusCurveWidthErrorFromText (const std::string & s);
	
	
void focusCurveWasFixedFromText (const std::string & s);
	
	
void offIntensityFromText (const std::string & s);
	
	
void offIntensityErrorFromText (const std::string & s);
	
	
void offIntensityWasFixedFromText (const std::string & s);
	
	
void peakIntensityFromText (const std::string & s);
	
	
void peakIntensityErrorFromText (const std::string & s);
	
	
void peakIntensityWasFixedFromText (const std::string & s);
	
	
void astigmPlusFromText (const std::string & s);
	
	
void astigmPlusErrorFromText (const std::string & s);
	
	
void astigmMultFromText (const std::string & s);
	
	
void astigmMultErrorFromText (const std::string & s);
	
	
void illumOffsetFromText (const std::string & s);
	
	
void illumOffsetErrorFromText (const std::string & s);
	
	
void fitRMSFromText (const std::string & s);
	
	
	
	/**
	 * Serialize this into a stream of bytes written to an EndianOSStream.
	 * @param eoss the EndianOSStream to be written to
	 */
	 void toBin(EndianOSStream& eoss);
	 	 
	 /**
	  * Deserialize a stream of bytes read from an EndianIStream to build a PointingRow.
	  * @param eiss the EndianIStream to be read.
	  * @param table the CalFocusTable to which the row built by deserialization will be parented.
	  * @param attributesSeq a vector containing the names of the attributes . The elements order defines the order 
	  * in which the attributes are written in the binary serialization.

	 static CalFocusRow* fromBin(EndianIStream& eis, CalFocusTable& table, const std::vector<std::string>& attributesSeq);	 
		*/
};

} // End namespace asdm

#endif /* CalFocus_CLASS */
