
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
 * File CalFocusRow.cpp
 */
 
#include <vector>
#include <set>

#include <alma/ASDM/ASDM.h>
#include <alma/ASDM/CalFocusRow.h>
#include <alma/ASDM/CalFocusTable.h>

#include <alma/ASDM/CalDataTable.h>
#include <alma/ASDM/CalDataRow.h>

#include <alma/ASDM/CalReductionTable.h>
#include <alma/ASDM/CalReductionRow.h>
	

using asdm::ASDM;
using asdm::CalFocusRow;
using asdm::CalFocusTable;

using asdm::CalDataTable;
using asdm::CalDataRow;

using asdm::CalReductionTable;
using asdm::CalReductionRow;


#include <alma/ASDM/Parser.h>

#include <alma/ASDM/EnumerationParser.h>
#include <alma/ASDM/ASDMValuesParser.h>
 
#include <alma/ASDM/InvalidArgumentException.h>

using namespace std;

namespace asdm {
	CalFocusRow::~CalFocusRow() {
	}

	/**
	 * Return the table to which this row belongs.
	 */
	CalFocusTable &CalFocusRow::getTable() const {
		return table;
	}

	bool CalFocusRow::isAdded() const {
		return hasBeenAdded;
	}	

	void CalFocusRow::isAdded(bool added) {
		hasBeenAdded = added;
	}
	
#ifndef WITHOUT_ACS
	using asdmIDL::CalFocusRowIDL;
#endif
	
#ifndef WITHOUT_ACS
	/**
	 * Return this row in the form of an IDL struct.
	 * @return The values of this row as a CalFocusRowIDL struct.
	 */
	CalFocusRowIDL *CalFocusRow::toIDL() const {
		CalFocusRowIDL *x = new CalFocusRowIDL ();
		
		// Fill the IDL structure.
	
		
	
  		
		
		
			
		x->startValidTime = startValidTime.toIDLArrayTime();
			
		
	

	
  		
		
		
			
		x->endValidTime = endValidTime.toIDLArrayTime();
			
		
	

	
  		
		
		
			
				
		x->antennaName = CORBA::string_dup(antennaName.c_str());
				
 			
		
	

	
  		
		
		
			
				
		x->receiverBand = receiverBand;
 				
 			
		
	

	
  		
		
		
			
		x->ambientTemperature = ambientTemperature.toIDLTemperature();
			
		
	

	
  		
		
		
			
				
		x->atmPhaseCorrection = atmPhaseCorrection;
 				
 			
		
	

	
  		
		
		
			
				
		x->focusMethod = focusMethod;
 				
 			
		
	

	
  		
		
		
			
		x->frequencyRange.length(frequencyRange.size());
		for (unsigned int i = 0; i < frequencyRange.size(); ++i) {
			
			x->frequencyRange[i] = frequencyRange.at(i).toIDLFrequency();
			
	 	}
			
		
	

	
  		
		
		
			
		x->pointingDirection.length(pointingDirection.size());
		for (unsigned int i = 0; i < pointingDirection.size(); ++i) {
			
			x->pointingDirection[i] = pointingDirection.at(i).toIDLAngle();
			
	 	}
			
		
	

	
  		
		
		
			
				
		x->numReceptor = numReceptor;
 				
 			
		
	

	
  		
		
		
			
		x->polarizationTypes.length(polarizationTypes.size());
		for (unsigned int i = 0; i < polarizationTypes.size(); ++i) {
			
				
			x->polarizationTypes[i] = polarizationTypes.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		
			
		x->wereFixed.length(wereFixed.size());
		for (unsigned int i = 0; i < wereFixed.size(); ++i) {
			
				
			x->wereFixed[i] = wereFixed.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		
			
		x->offset.length(offset.size());
		for (unsigned int i = 0; i < offset.size(); i++) {
			x->offset[i].length(offset.at(i).size());			 		
		}
		
		for (unsigned int i = 0; i < offset.size() ; i++)
			for (unsigned int j = 0; j < offset.at(i).size(); j++)
					
				x->offset[i][j]= offset.at(i).at(j).toIDLLength();
									
		
			
		
	

	
  		
		
		
			
		x->offsetError.length(offsetError.size());
		for (unsigned int i = 0; i < offsetError.size(); i++) {
			x->offsetError[i].length(offsetError.at(i).size());			 		
		}
		
		for (unsigned int i = 0; i < offsetError.size() ; i++)
			for (unsigned int j = 0; j < offsetError.at(i).size(); j++)
					
				x->offsetError[i][j]= offsetError.at(i).at(j).toIDLLength();
									
		
			
		
	

	
  		
		
		
			
		x->offsetWasTied.length(offsetWasTied.size());
		for (unsigned int i = 0; i < offsetWasTied.size(); i++) {
			x->offsetWasTied[i].length(offsetWasTied.at(i).size());			 		
		}
		
		for (unsigned int i = 0; i < offsetWasTied.size() ; i++)
			for (unsigned int j = 0; j < offsetWasTied.at(i).size(); j++)
					
						
				x->offsetWasTied[i][j] = offsetWasTied.at(i).at(j);
		 				
			 						
		
			
		
	

	
  		
		
		
			
		x->reducedChiSquared.length(reducedChiSquared.size());
		for (unsigned int i = 0; i < reducedChiSquared.size(); i++) {
			x->reducedChiSquared[i].length(reducedChiSquared.at(i).size());			 		
		}
		
		for (unsigned int i = 0; i < reducedChiSquared.size() ; i++)
			for (unsigned int j = 0; j < reducedChiSquared.at(i).size(); j++)
					
						
				x->reducedChiSquared[i][j] = reducedChiSquared.at(i).at(j);
		 				
			 						
		
			
		
	

	
  		
		
		
			
		x->position.length(position.size());
		for (unsigned int i = 0; i < position.size(); i++) {
			x->position[i].length(position.at(i).size());			 		
		}
		
		for (unsigned int i = 0; i < position.size() ; i++)
			for (unsigned int j = 0; j < position.at(i).size(); j++)
					
				x->position[i][j]= position.at(i).at(j).toIDLLength();
									
		
			
		
	

	
  		
		
		x->polarizationsAveragedExists = polarizationsAveragedExists;
		
		
			
				
		x->polarizationsAveraged = polarizationsAveraged;
 				
 			
		
	

	
  		
		
		x->focusCurveWidthExists = focusCurveWidthExists;
		
		
			
		x->focusCurveWidth.length(focusCurveWidth.size());
		for (unsigned int i = 0; i < focusCurveWidth.size(); i++) {
			x->focusCurveWidth[i].length(focusCurveWidth.at(i).size());			 		
		}
		
		for (unsigned int i = 0; i < focusCurveWidth.size() ; i++)
			for (unsigned int j = 0; j < focusCurveWidth.at(i).size(); j++)
					
				x->focusCurveWidth[i][j]= focusCurveWidth.at(i).at(j).toIDLLength();
									
		
			
		
	

	
  		
		
		x->focusCurveWidthErrorExists = focusCurveWidthErrorExists;
		
		
			
		x->focusCurveWidthError.length(focusCurveWidthError.size());
		for (unsigned int i = 0; i < focusCurveWidthError.size(); i++) {
			x->focusCurveWidthError[i].length(focusCurveWidthError.at(i).size());			 		
		}
		
		for (unsigned int i = 0; i < focusCurveWidthError.size() ; i++)
			for (unsigned int j = 0; j < focusCurveWidthError.at(i).size(); j++)
					
				x->focusCurveWidthError[i][j]= focusCurveWidthError.at(i).at(j).toIDLLength();
									
		
			
		
	

	
  		
		
		x->focusCurveWasFixedExists = focusCurveWasFixedExists;
		
		
			
		x->focusCurveWasFixed.length(focusCurveWasFixed.size());
		for (unsigned int i = 0; i < focusCurveWasFixed.size(); ++i) {
			
				
			x->focusCurveWasFixed[i] = focusCurveWasFixed.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		x->offIntensityExists = offIntensityExists;
		
		
			
		x->offIntensity.length(offIntensity.size());
		for (unsigned int i = 0; i < offIntensity.size(); ++i) {
			
			x->offIntensity[i] = offIntensity.at(i).toIDLTemperature();
			
	 	}
			
		
	

	
  		
		
		x->offIntensityErrorExists = offIntensityErrorExists;
		
		
			
		x->offIntensityError.length(offIntensityError.size());
		for (unsigned int i = 0; i < offIntensityError.size(); ++i) {
			
			x->offIntensityError[i] = offIntensityError.at(i).toIDLTemperature();
			
	 	}
			
		
	

	
  		
		
		x->offIntensityWasFixedExists = offIntensityWasFixedExists;
		
		
			
				
		x->offIntensityWasFixed = offIntensityWasFixed;
 				
 			
		
	

	
  		
		
		x->peakIntensityExists = peakIntensityExists;
		
		
			
		x->peakIntensity.length(peakIntensity.size());
		for (unsigned int i = 0; i < peakIntensity.size(); ++i) {
			
			x->peakIntensity[i] = peakIntensity.at(i).toIDLTemperature();
			
	 	}
			
		
	

	
  		
		
		x->peakIntensityErrorExists = peakIntensityErrorExists;
		
		
			
		x->peakIntensityError.length(peakIntensityError.size());
		for (unsigned int i = 0; i < peakIntensityError.size(); ++i) {
			
			x->peakIntensityError[i] = peakIntensityError.at(i).toIDLTemperature();
			
	 	}
			
		
	

	
  		
		
		x->peakIntensityWasFixedExists = peakIntensityWasFixedExists;
		
		
			
				
		x->peakIntensityWasFixed = peakIntensityWasFixed;
 				
 			
		
	

	
  		
		
		x->astigmPlusExists = astigmPlusExists;
		
		
			
		x->astigmPlus.length(astigmPlus.size());
		for (unsigned int i = 0; i < astigmPlus.size(); ++i) {
			
			x->astigmPlus[i] = astigmPlus.at(i).toIDLLength();
			
	 	}
			
		
	

	
  		
		
		x->astigmPlusErrorExists = astigmPlusErrorExists;
		
		
			
		x->astigmPlusError.length(astigmPlusError.size());
		for (unsigned int i = 0; i < astigmPlusError.size(); ++i) {
			
			x->astigmPlusError[i] = astigmPlusError.at(i).toIDLLength();
			
	 	}
			
		
	

	
  		
		
		x->astigmMultExists = astigmMultExists;
		
		
			
		x->astigmMult.length(astigmMult.size());
		for (unsigned int i = 0; i < astigmMult.size(); ++i) {
			
			x->astigmMult[i] = astigmMult.at(i).toIDLLength();
			
	 	}
			
		
	

	
  		
		
		x->astigmMultErrorExists = astigmMultErrorExists;
		
		
			
		x->astigmMultError.length(astigmMultError.size());
		for (unsigned int i = 0; i < astigmMultError.size(); ++i) {
			
			x->astigmMultError[i] = astigmMultError.at(i).toIDLLength();
			
	 	}
			
		
	

	
  		
		
		x->illumOffsetExists = illumOffsetExists;
		
		
			
		x->illumOffset.length(illumOffset.size());
		for (unsigned int i = 0; i < illumOffset.size(); i++) {
			x->illumOffset[i].length(illumOffset.at(i).size());			 		
		}
		
		for (unsigned int i = 0; i < illumOffset.size() ; i++)
			for (unsigned int j = 0; j < illumOffset.at(i).size(); j++)
					
				x->illumOffset[i][j]= illumOffset.at(i).at(j).toIDLLength();
									
		
			
		
	

	
  		
		
		x->illumOffsetErrorExists = illumOffsetErrorExists;
		
		
			
		x->illumOffsetError.length(illumOffsetError.size());
		for (unsigned int i = 0; i < illumOffsetError.size(); i++) {
			x->illumOffsetError[i].length(illumOffsetError.at(i).size());			 		
		}
		
		for (unsigned int i = 0; i < illumOffsetError.size() ; i++)
			for (unsigned int j = 0; j < illumOffsetError.at(i).size(); j++)
					
				x->illumOffsetError[i][j]= illumOffsetError.at(i).at(j).toIDLLength();
									
		
			
		
	

	
  		
		
		x->fitRMSExists = fitRMSExists;
		
		
			
		x->fitRMS.length(fitRMS.size());
		for (unsigned int i = 0; i < fitRMS.size(); ++i) {
			
			x->fitRMS[i] = fitRMS.at(i).toIDLLength();
			
	 	}
			
		
	

	
	
		
	
  	
 		
		
	 	
			
		x->calDataId = calDataId.toIDLTag();
			
	 	 		
  	

	
  	
 		
		
	 	
			
		x->calReductionId = calReductionId.toIDLTag();
			
	 	 		
  	

	
		
	

	

		
		return x;
	
	}
	
	void CalFocusRow::toIDL(asdmIDL::CalFocusRowIDL& x) const {
		// Set the x's fields.
	
		
	
  		
		
		
			
		x.startValidTime = startValidTime.toIDLArrayTime();
			
		
	

	
  		
		
		
			
		x.endValidTime = endValidTime.toIDLArrayTime();
			
		
	

	
  		
		
		
			
				
		x.antennaName = CORBA::string_dup(antennaName.c_str());
				
 			
		
	

	
  		
		
		
			
				
		x.receiverBand = receiverBand;
 				
 			
		
	

	
  		
		
		
			
		x.ambientTemperature = ambientTemperature.toIDLTemperature();
			
		
	

	
  		
		
		
			
				
		x.atmPhaseCorrection = atmPhaseCorrection;
 				
 			
		
	

	
  		
		
		
			
				
		x.focusMethod = focusMethod;
 				
 			
		
	

	
  		
		
		
			
		x.frequencyRange.length(frequencyRange.size());
		for (unsigned int i = 0; i < frequencyRange.size(); ++i) {
			
			x.frequencyRange[i] = frequencyRange.at(i).toIDLFrequency();
			
	 	}
			
		
	

	
  		
		
		
			
		x.pointingDirection.length(pointingDirection.size());
		for (unsigned int i = 0; i < pointingDirection.size(); ++i) {
			
			x.pointingDirection[i] = pointingDirection.at(i).toIDLAngle();
			
	 	}
			
		
	

	
  		
		
		
			
				
		x.numReceptor = numReceptor;
 				
 			
		
	

	
  		
		
		
			
		x.polarizationTypes.length(polarizationTypes.size());
		for (unsigned int i = 0; i < polarizationTypes.size(); ++i) {
			
				
			x.polarizationTypes[i] = polarizationTypes.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		
			
		x.wereFixed.length(wereFixed.size());
		for (unsigned int i = 0; i < wereFixed.size(); ++i) {
			
				
			x.wereFixed[i] = wereFixed.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		
			
		x.offset.length(offset.size());
		for (unsigned int i = 0; i < offset.size(); i++) {
			x.offset[i].length(offset.at(i).size());			 		
		}
		
		for (unsigned int i = 0; i < offset.size() ; i++)
			for (unsigned int j = 0; j < offset.at(i).size(); j++)
					
				x.offset[i][j]= offset.at(i).at(j).toIDLLength();
									
		
			
		
	

	
  		
		
		
			
		x.offsetError.length(offsetError.size());
		for (unsigned int i = 0; i < offsetError.size(); i++) {
			x.offsetError[i].length(offsetError.at(i).size());			 		
		}
		
		for (unsigned int i = 0; i < offsetError.size() ; i++)
			for (unsigned int j = 0; j < offsetError.at(i).size(); j++)
					
				x.offsetError[i][j]= offsetError.at(i).at(j).toIDLLength();
									
		
			
		
	

	
  		
		
		
			
		x.offsetWasTied.length(offsetWasTied.size());
		for (unsigned int i = 0; i < offsetWasTied.size(); i++) {
			x.offsetWasTied[i].length(offsetWasTied.at(i).size());			 		
		}
		
		for (unsigned int i = 0; i < offsetWasTied.size() ; i++)
			for (unsigned int j = 0; j < offsetWasTied.at(i).size(); j++)
					
						
				x.offsetWasTied[i][j] = offsetWasTied.at(i).at(j);
		 				
			 						
		
			
		
	

	
  		
		
		
			
		x.reducedChiSquared.length(reducedChiSquared.size());
		for (unsigned int i = 0; i < reducedChiSquared.size(); i++) {
			x.reducedChiSquared[i].length(reducedChiSquared.at(i).size());			 		
		}
		
		for (unsigned int i = 0; i < reducedChiSquared.size() ; i++)
			for (unsigned int j = 0; j < reducedChiSquared.at(i).size(); j++)
					
						
				x.reducedChiSquared[i][j] = reducedChiSquared.at(i).at(j);
		 				
			 						
		
			
		
	

	
  		
		
		
			
		x.position.length(position.size());
		for (unsigned int i = 0; i < position.size(); i++) {
			x.position[i].length(position.at(i).size());			 		
		}
		
		for (unsigned int i = 0; i < position.size() ; i++)
			for (unsigned int j = 0; j < position.at(i).size(); j++)
					
				x.position[i][j]= position.at(i).at(j).toIDLLength();
									
		
			
		
	

	
  		
		
		x.polarizationsAveragedExists = polarizationsAveragedExists;
		
		
			
				
		x.polarizationsAveraged = polarizationsAveraged;
 				
 			
		
	

	
  		
		
		x.focusCurveWidthExists = focusCurveWidthExists;
		
		
			
		x.focusCurveWidth.length(focusCurveWidth.size());
		for (unsigned int i = 0; i < focusCurveWidth.size(); i++) {
			x.focusCurveWidth[i].length(focusCurveWidth.at(i).size());			 		
		}
		
		for (unsigned int i = 0; i < focusCurveWidth.size() ; i++)
			for (unsigned int j = 0; j < focusCurveWidth.at(i).size(); j++)
					
				x.focusCurveWidth[i][j]= focusCurveWidth.at(i).at(j).toIDLLength();
									
		
			
		
	

	
  		
		
		x.focusCurveWidthErrorExists = focusCurveWidthErrorExists;
		
		
			
		x.focusCurveWidthError.length(focusCurveWidthError.size());
		for (unsigned int i = 0; i < focusCurveWidthError.size(); i++) {
			x.focusCurveWidthError[i].length(focusCurveWidthError.at(i).size());			 		
		}
		
		for (unsigned int i = 0; i < focusCurveWidthError.size() ; i++)
			for (unsigned int j = 0; j < focusCurveWidthError.at(i).size(); j++)
					
				x.focusCurveWidthError[i][j]= focusCurveWidthError.at(i).at(j).toIDLLength();
									
		
			
		
	

	
  		
		
		x.focusCurveWasFixedExists = focusCurveWasFixedExists;
		
		
			
		x.focusCurveWasFixed.length(focusCurveWasFixed.size());
		for (unsigned int i = 0; i < focusCurveWasFixed.size(); ++i) {
			
				
			x.focusCurveWasFixed[i] = focusCurveWasFixed.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		x.offIntensityExists = offIntensityExists;
		
		
			
		x.offIntensity.length(offIntensity.size());
		for (unsigned int i = 0; i < offIntensity.size(); ++i) {
			
			x.offIntensity[i] = offIntensity.at(i).toIDLTemperature();
			
	 	}
			
		
	

	
  		
		
		x.offIntensityErrorExists = offIntensityErrorExists;
		
		
			
		x.offIntensityError.length(offIntensityError.size());
		for (unsigned int i = 0; i < offIntensityError.size(); ++i) {
			
			x.offIntensityError[i] = offIntensityError.at(i).toIDLTemperature();
			
	 	}
			
		
	

	
  		
		
		x.offIntensityWasFixedExists = offIntensityWasFixedExists;
		
		
			
				
		x.offIntensityWasFixed = offIntensityWasFixed;
 				
 			
		
	

	
  		
		
		x.peakIntensityExists = peakIntensityExists;
		
		
			
		x.peakIntensity.length(peakIntensity.size());
		for (unsigned int i = 0; i < peakIntensity.size(); ++i) {
			
			x.peakIntensity[i] = peakIntensity.at(i).toIDLTemperature();
			
	 	}
			
		
	

	
  		
		
		x.peakIntensityErrorExists = peakIntensityErrorExists;
		
		
			
		x.peakIntensityError.length(peakIntensityError.size());
		for (unsigned int i = 0; i < peakIntensityError.size(); ++i) {
			
			x.peakIntensityError[i] = peakIntensityError.at(i).toIDLTemperature();
			
	 	}
			
		
	

	
  		
		
		x.peakIntensityWasFixedExists = peakIntensityWasFixedExists;
		
		
			
				
		x.peakIntensityWasFixed = peakIntensityWasFixed;
 				
 			
		
	

	
  		
		
		x.astigmPlusExists = astigmPlusExists;
		
		
			
		x.astigmPlus.length(astigmPlus.size());
		for (unsigned int i = 0; i < astigmPlus.size(); ++i) {
			
			x.astigmPlus[i] = astigmPlus.at(i).toIDLLength();
			
	 	}
			
		
	

	
  		
		
		x.astigmPlusErrorExists = astigmPlusErrorExists;
		
		
			
		x.astigmPlusError.length(astigmPlusError.size());
		for (unsigned int i = 0; i < astigmPlusError.size(); ++i) {
			
			x.astigmPlusError[i] = astigmPlusError.at(i).toIDLLength();
			
	 	}
			
		
	

	
  		
		
		x.astigmMultExists = astigmMultExists;
		
		
			
		x.astigmMult.length(astigmMult.size());
		for (unsigned int i = 0; i < astigmMult.size(); ++i) {
			
			x.astigmMult[i] = astigmMult.at(i).toIDLLength();
			
	 	}
			
		
	

	
  		
		
		x.astigmMultErrorExists = astigmMultErrorExists;
		
		
			
		x.astigmMultError.length(astigmMultError.size());
		for (unsigned int i = 0; i < astigmMultError.size(); ++i) {
			
			x.astigmMultError[i] = astigmMultError.at(i).toIDLLength();
			
	 	}
			
		
	

	
  		
		
		x.illumOffsetExists = illumOffsetExists;
		
		
			
		x.illumOffset.length(illumOffset.size());
		for (unsigned int i = 0; i < illumOffset.size(); i++) {
			x.illumOffset[i].length(illumOffset.at(i).size());			 		
		}
		
		for (unsigned int i = 0; i < illumOffset.size() ; i++)
			for (unsigned int j = 0; j < illumOffset.at(i).size(); j++)
					
				x.illumOffset[i][j]= illumOffset.at(i).at(j).toIDLLength();
									
		
			
		
	

	
  		
		
		x.illumOffsetErrorExists = illumOffsetErrorExists;
		
		
			
		x.illumOffsetError.length(illumOffsetError.size());
		for (unsigned int i = 0; i < illumOffsetError.size(); i++) {
			x.illumOffsetError[i].length(illumOffsetError.at(i).size());			 		
		}
		
		for (unsigned int i = 0; i < illumOffsetError.size() ; i++)
			for (unsigned int j = 0; j < illumOffsetError.at(i).size(); j++)
					
				x.illumOffsetError[i][j]= illumOffsetError.at(i).at(j).toIDLLength();
									
		
			
		
	

	
  		
		
		x.fitRMSExists = fitRMSExists;
		
		
			
		x.fitRMS.length(fitRMS.size());
		for (unsigned int i = 0; i < fitRMS.size(); ++i) {
			
			x.fitRMS[i] = fitRMS.at(i).toIDLLength();
			
	 	}
			
		
	

	
	
		
	
  	
 		
		
	 	
			
		x.calDataId = calDataId.toIDLTag();
			
	 	 		
  	

	
  	
 		
		
	 	
			
		x.calReductionId = calReductionId.toIDLTag();
			
	 	 		
  	

	
		
	

	

	
	}
#endif
	

#ifndef WITHOUT_ACS
	/**
	 * Fill the values of this row from the IDL struct CalFocusRowIDL.
	 * @param x The IDL struct containing the values used to fill this row.
	 */
	void CalFocusRow::setFromIDL (CalFocusRowIDL x){
		try {
		// Fill the values from x.
	
		
	
		
		
			
		setStartValidTime(ArrayTime (x.startValidTime));
			
 		
		
	

	
		
		
			
		setEndValidTime(ArrayTime (x.endValidTime));
			
 		
		
	

	
		
		
			
		setAntennaName(string (x.antennaName));
			
 		
		
	

	
		
		
			
		setReceiverBand(x.receiverBand);
  			
 		
		
	

	
		
		
			
		setAmbientTemperature(Temperature (x.ambientTemperature));
			
 		
		
	

	
		
		
			
		setAtmPhaseCorrection(x.atmPhaseCorrection);
  			
 		
		
	

	
		
		
			
		setFocusMethod(x.focusMethod);
  			
 		
		
	

	
		
		
			
		frequencyRange .clear();
		for (unsigned int i = 0; i <x.frequencyRange.length(); ++i) {
			
			frequencyRange.push_back(Frequency (x.frequencyRange[i]));
			
		}
			
  		
		
	

	
		
		
			
		pointingDirection .clear();
		for (unsigned int i = 0; i <x.pointingDirection.length(); ++i) {
			
			pointingDirection.push_back(Angle (x.pointingDirection[i]));
			
		}
			
  		
		
	

	
		
		
			
		setNumReceptor(x.numReceptor);
  			
 		
		
	

	
		
		
			
		polarizationTypes .clear();
		for (unsigned int i = 0; i <x.polarizationTypes.length(); ++i) {
			
			polarizationTypes.push_back(x.polarizationTypes[i]);
  			
		}
			
  		
		
	

	
		
		
			
		wereFixed .clear();
		for (unsigned int i = 0; i <x.wereFixed.length(); ++i) {
			
			wereFixed.push_back(x.wereFixed[i]);
  			
		}
			
  		
		
	

	
		
		
			
		offset .clear();
        
        vector<Length> v_aux_offset;
        
		for (unsigned int i = 0; i < x.offset.length(); ++i) {
			v_aux_offset.clear();
			for (unsigned int j = 0; j < x.offset[0].length(); ++j) {
				
				v_aux_offset.push_back(Length (x.offset[i][j]));
				
  			}
  			offset.push_back(v_aux_offset);			
		}
			
  		
		
	

	
		
		
			
		offsetError .clear();
        
        vector<Length> v_aux_offsetError;
        
		for (unsigned int i = 0; i < x.offsetError.length(); ++i) {
			v_aux_offsetError.clear();
			for (unsigned int j = 0; j < x.offsetError[0].length(); ++j) {
				
				v_aux_offsetError.push_back(Length (x.offsetError[i][j]));
				
  			}
  			offsetError.push_back(v_aux_offsetError);			
		}
			
  		
		
	

	
		
		
			
		offsetWasTied .clear();
        
        vector<bool> v_aux_offsetWasTied;
        
		for (unsigned int i = 0; i < x.offsetWasTied.length(); ++i) {
			v_aux_offsetWasTied.clear();
			for (unsigned int j = 0; j < x.offsetWasTied[0].length(); ++j) {
				
				v_aux_offsetWasTied.push_back(x.offsetWasTied[i][j]);
	  			
  			}
  			offsetWasTied.push_back(v_aux_offsetWasTied);			
		}
			
  		
		
	

	
		
		
			
		reducedChiSquared .clear();
        
        vector<double> v_aux_reducedChiSquared;
        
		for (unsigned int i = 0; i < x.reducedChiSquared.length(); ++i) {
			v_aux_reducedChiSquared.clear();
			for (unsigned int j = 0; j < x.reducedChiSquared[0].length(); ++j) {
				
				v_aux_reducedChiSquared.push_back(x.reducedChiSquared[i][j]);
	  			
  			}
  			reducedChiSquared.push_back(v_aux_reducedChiSquared);			
		}
			
  		
		
	

	
		
		
			
		position .clear();
        
        vector<Length> v_aux_position;
        
		for (unsigned int i = 0; i < x.position.length(); ++i) {
			v_aux_position.clear();
			for (unsigned int j = 0; j < x.position[0].length(); ++j) {
				
				v_aux_position.push_back(Length (x.position[i][j]));
				
  			}
  			position.push_back(v_aux_position);			
		}
			
  		
		
	

	
		
		polarizationsAveragedExists = x.polarizationsAveragedExists;
		if (x.polarizationsAveragedExists) {
		
		
			
		setPolarizationsAveraged(x.polarizationsAveraged);
  			
 		
		
		}
		
	

	
		
		focusCurveWidthExists = x.focusCurveWidthExists;
		if (x.focusCurveWidthExists) {
		
		
			
		focusCurveWidth .clear();
        
        vector<Length> v_aux_focusCurveWidth;
        
		for (unsigned int i = 0; i < x.focusCurveWidth.length(); ++i) {
			v_aux_focusCurveWidth.clear();
			for (unsigned int j = 0; j < x.focusCurveWidth[0].length(); ++j) {
				
				v_aux_focusCurveWidth.push_back(Length (x.focusCurveWidth[i][j]));
				
  			}
  			focusCurveWidth.push_back(v_aux_focusCurveWidth);			
		}
			
  		
		
		}
		
	

	
		
		focusCurveWidthErrorExists = x.focusCurveWidthErrorExists;
		if (x.focusCurveWidthErrorExists) {
		
		
			
		focusCurveWidthError .clear();
        
        vector<Length> v_aux_focusCurveWidthError;
        
		for (unsigned int i = 0; i < x.focusCurveWidthError.length(); ++i) {
			v_aux_focusCurveWidthError.clear();
			for (unsigned int j = 0; j < x.focusCurveWidthError[0].length(); ++j) {
				
				v_aux_focusCurveWidthError.push_back(Length (x.focusCurveWidthError[i][j]));
				
  			}
  			focusCurveWidthError.push_back(v_aux_focusCurveWidthError);			
		}
			
  		
		
		}
		
	

	
		
		focusCurveWasFixedExists = x.focusCurveWasFixedExists;
		if (x.focusCurveWasFixedExists) {
		
		
			
		focusCurveWasFixed .clear();
		for (unsigned int i = 0; i <x.focusCurveWasFixed.length(); ++i) {
			
			focusCurveWasFixed.push_back(x.focusCurveWasFixed[i]);
  			
		}
			
  		
		
		}
		
	

	
		
		offIntensityExists = x.offIntensityExists;
		if (x.offIntensityExists) {
		
		
			
		offIntensity .clear();
		for (unsigned int i = 0; i <x.offIntensity.length(); ++i) {
			
			offIntensity.push_back(Temperature (x.offIntensity[i]));
			
		}
			
  		
		
		}
		
	

	
		
		offIntensityErrorExists = x.offIntensityErrorExists;
		if (x.offIntensityErrorExists) {
		
		
			
		offIntensityError .clear();
		for (unsigned int i = 0; i <x.offIntensityError.length(); ++i) {
			
			offIntensityError.push_back(Temperature (x.offIntensityError[i]));
			
		}
			
  		
		
		}
		
	

	
		
		offIntensityWasFixedExists = x.offIntensityWasFixedExists;
		if (x.offIntensityWasFixedExists) {
		
		
			
		setOffIntensityWasFixed(x.offIntensityWasFixed);
  			
 		
		
		}
		
	

	
		
		peakIntensityExists = x.peakIntensityExists;
		if (x.peakIntensityExists) {
		
		
			
		peakIntensity .clear();
		for (unsigned int i = 0; i <x.peakIntensity.length(); ++i) {
			
			peakIntensity.push_back(Temperature (x.peakIntensity[i]));
			
		}
			
  		
		
		}
		
	

	
		
		peakIntensityErrorExists = x.peakIntensityErrorExists;
		if (x.peakIntensityErrorExists) {
		
		
			
		peakIntensityError .clear();
		for (unsigned int i = 0; i <x.peakIntensityError.length(); ++i) {
			
			peakIntensityError.push_back(Temperature (x.peakIntensityError[i]));
			
		}
			
  		
		
		}
		
	

	
		
		peakIntensityWasFixedExists = x.peakIntensityWasFixedExists;
		if (x.peakIntensityWasFixedExists) {
		
		
			
		setPeakIntensityWasFixed(x.peakIntensityWasFixed);
  			
 		
		
		}
		
	

	
		
		astigmPlusExists = x.astigmPlusExists;
		if (x.astigmPlusExists) {
		
		
			
		astigmPlus .clear();
		for (unsigned int i = 0; i <x.astigmPlus.length(); ++i) {
			
			astigmPlus.push_back(Length (x.astigmPlus[i]));
			
		}
			
  		
		
		}
		
	

	
		
		astigmPlusErrorExists = x.astigmPlusErrorExists;
		if (x.astigmPlusErrorExists) {
		
		
			
		astigmPlusError .clear();
		for (unsigned int i = 0; i <x.astigmPlusError.length(); ++i) {
			
			astigmPlusError.push_back(Length (x.astigmPlusError[i]));
			
		}
			
  		
		
		}
		
	

	
		
		astigmMultExists = x.astigmMultExists;
		if (x.astigmMultExists) {
		
		
			
		astigmMult .clear();
		for (unsigned int i = 0; i <x.astigmMult.length(); ++i) {
			
			astigmMult.push_back(Length (x.astigmMult[i]));
			
		}
			
  		
		
		}
		
	

	
		
		astigmMultErrorExists = x.astigmMultErrorExists;
		if (x.astigmMultErrorExists) {
		
		
			
		astigmMultError .clear();
		for (unsigned int i = 0; i <x.astigmMultError.length(); ++i) {
			
			astigmMultError.push_back(Length (x.astigmMultError[i]));
			
		}
			
  		
		
		}
		
	

	
		
		illumOffsetExists = x.illumOffsetExists;
		if (x.illumOffsetExists) {
		
		
			
		illumOffset .clear();
        
        vector<Length> v_aux_illumOffset;
        
		for (unsigned int i = 0; i < x.illumOffset.length(); ++i) {
			v_aux_illumOffset.clear();
			for (unsigned int j = 0; j < x.illumOffset[0].length(); ++j) {
				
				v_aux_illumOffset.push_back(Length (x.illumOffset[i][j]));
				
  			}
  			illumOffset.push_back(v_aux_illumOffset);			
		}
			
  		
		
		}
		
	

	
		
		illumOffsetErrorExists = x.illumOffsetErrorExists;
		if (x.illumOffsetErrorExists) {
		
		
			
		illumOffsetError .clear();
        
        vector<Length> v_aux_illumOffsetError;
        
		for (unsigned int i = 0; i < x.illumOffsetError.length(); ++i) {
			v_aux_illumOffsetError.clear();
			for (unsigned int j = 0; j < x.illumOffsetError[0].length(); ++j) {
				
				v_aux_illumOffsetError.push_back(Length (x.illumOffsetError[i][j]));
				
  			}
  			illumOffsetError.push_back(v_aux_illumOffsetError);			
		}
			
  		
		
		}
		
	

	
		
		fitRMSExists = x.fitRMSExists;
		if (x.fitRMSExists) {
		
		
			
		fitRMS .clear();
		for (unsigned int i = 0; i <x.fitRMS.length(); ++i) {
			
			fitRMS.push_back(Length (x.fitRMS[i]));
			
		}
			
  		
		
		}
		
	

	
	
		
	
		
		
			
		setCalDataId(Tag (x.calDataId));
			
 		
		
	

	
		
		
			
		setCalReductionId(Tag (x.calReductionId));
			
 		
		
	

	
		
	

	

		} catch (const IllegalAccessException &err) {
			throw ConversionException (err.getMessage(),"CalFocus");
		}
	}
#endif
	
	/**
	 * Return this row in the form of an XML string.
	 * @return The values of this row as an XML string.
	 */
	string CalFocusRow::toXML() const {
		string buf;
		buf.append("<row> \n");
		
	
		
  	
 		
		
		Parser::toXML(startValidTime, "startValidTime", buf);
		
		
	

  	
 		
		
		Parser::toXML(endValidTime, "endValidTime", buf);
		
		
	

  	
 		
		
		Parser::toXML(antennaName, "antennaName", buf);
		
		
	

  	
 		
		
			buf.append(EnumerationParser::toXML("receiverBand", receiverBand));
		
		
	

  	
 		
		
		Parser::toXML(ambientTemperature, "ambientTemperature", buf);
		
		
	

  	
 		
		
			buf.append(EnumerationParser::toXML("atmPhaseCorrection", atmPhaseCorrection));
		
		
	

  	
 		
		
			buf.append(EnumerationParser::toXML("focusMethod", focusMethod));
		
		
	

  	
 		
		
		Parser::toXML(frequencyRange, "frequencyRange", buf);
		
		
	

  	
 		
		
		Parser::toXML(pointingDirection, "pointingDirection", buf);
		
		
	

  	
 		
		
		Parser::toXML(numReceptor, "numReceptor", buf);
		
		
	

  	
 		
		
			buf.append(EnumerationParser::toXML("polarizationTypes", polarizationTypes));
		
		
	

  	
 		
		
		Parser::toXML(wereFixed, "wereFixed", buf);
		
		
	

  	
 		
		
		Parser::toXML(offset, "offset", buf);
		
		
	

  	
 		
		
		Parser::toXML(offsetError, "offsetError", buf);
		
		
	

  	
 		
		
		Parser::toXML(offsetWasTied, "offsetWasTied", buf);
		
		
	

  	
 		
		
		Parser::toXML(reducedChiSquared, "reducedChiSquared", buf);
		
		
	

  	
 		
		
		Parser::toXML(position, "position", buf);
		
		
	

  	
 		
		if (polarizationsAveragedExists) {
		
		
		Parser::toXML(polarizationsAveraged, "polarizationsAveraged", buf);
		
		
		}
		
	

  	
 		
		if (focusCurveWidthExists) {
		
		
		Parser::toXML(focusCurveWidth, "focusCurveWidth", buf);
		
		
		}
		
	

  	
 		
		if (focusCurveWidthErrorExists) {
		
		
		Parser::toXML(focusCurveWidthError, "focusCurveWidthError", buf);
		
		
		}
		
	

  	
 		
		if (focusCurveWasFixedExists) {
		
		
		Parser::toXML(focusCurveWasFixed, "focusCurveWasFixed", buf);
		
		
		}
		
	

  	
 		
		if (offIntensityExists) {
		
		
		Parser::toXML(offIntensity, "offIntensity", buf);
		
		
		}
		
	

  	
 		
		if (offIntensityErrorExists) {
		
		
		Parser::toXML(offIntensityError, "offIntensityError", buf);
		
		
		}
		
	

  	
 		
		if (offIntensityWasFixedExists) {
		
		
		Parser::toXML(offIntensityWasFixed, "offIntensityWasFixed", buf);
		
		
		}
		
	

  	
 		
		if (peakIntensityExists) {
		
		
		Parser::toXML(peakIntensity, "peakIntensity", buf);
		
		
		}
		
	

  	
 		
		if (peakIntensityErrorExists) {
		
		
		Parser::toXML(peakIntensityError, "peakIntensityError", buf);
		
		
		}
		
	

  	
 		
		if (peakIntensityWasFixedExists) {
		
		
		Parser::toXML(peakIntensityWasFixed, "peakIntensityWasFixed", buf);
		
		
		}
		
	

  	
 		
		if (astigmPlusExists) {
		
		
		Parser::toXML(astigmPlus, "astigmPlus", buf);
		
		
		}
		
	

  	
 		
		if (astigmPlusErrorExists) {
		
		
		Parser::toXML(astigmPlusError, "astigmPlusError", buf);
		
		
		}
		
	

  	
 		
		if (astigmMultExists) {
		
		
		Parser::toXML(astigmMult, "astigmMult", buf);
		
		
		}
		
	

  	
 		
		if (astigmMultErrorExists) {
		
		
		Parser::toXML(astigmMultError, "astigmMultError", buf);
		
		
		}
		
	

  	
 		
		if (illumOffsetExists) {
		
		
		Parser::toXML(illumOffset, "illumOffset", buf);
		
		
		}
		
	

  	
 		
		if (illumOffsetErrorExists) {
		
		
		Parser::toXML(illumOffsetError, "illumOffsetError", buf);
		
		
		}
		
	

  	
 		
		if (fitRMSExists) {
		
		
		Parser::toXML(fitRMS, "fitRMS", buf);
		
		
		}
		
	

	
	
		
  	
 		
		
		Parser::toXML(calDataId, "calDataId", buf);
		
		
	

  	
 		
		
		Parser::toXML(calReductionId, "calReductionId", buf);
		
		
	

	
		
	

	

		
		buf.append("</row>\n");
		return buf;
	}

	/**
	 * Fill the values of this row from an XML string 
	 * that was produced by the toXML() method.
	 * @param x The XML string being used to set the values of this row.
	 */
	void CalFocusRow::setFromXML (string rowDoc) {
		Parser row(rowDoc);
		string s = "";
		try {
	
		
	
  		
			
	  	setStartValidTime(Parser::getArrayTime("startValidTime","CalFocus",rowDoc));
			
		
	

	
  		
			
	  	setEndValidTime(Parser::getArrayTime("endValidTime","CalFocus",rowDoc));
			
		
	

	
  		
			
	  	setAntennaName(Parser::getString("antennaName","CalFocus",rowDoc));
			
		
	

	
		
		
		
		receiverBand = EnumerationParser::getReceiverBand("receiverBand","CalFocus",rowDoc);
		
		
		
	

	
  		
			
	  	setAmbientTemperature(Parser::getTemperature("ambientTemperature","CalFocus",rowDoc));
			
		
	

	
		
		
		
		atmPhaseCorrection = EnumerationParser::getAtmPhaseCorrection("atmPhaseCorrection","CalFocus",rowDoc);
		
		
		
	

	
		
		
		
		focusMethod = EnumerationParser::getFocusMethod("focusMethod","CalFocus",rowDoc);
		
		
		
	

	
  		
			
					
	  	setFrequencyRange(Parser::get1DFrequency("frequencyRange","CalFocus",rowDoc));
	  			
	  		
		
	

	
  		
			
					
	  	setPointingDirection(Parser::get1DAngle("pointingDirection","CalFocus",rowDoc));
	  			
	  		
		
	

	
  		
			
	  	setNumReceptor(Parser::getInteger("numReceptor","CalFocus",rowDoc));
			
		
	

	
		
		
		
		polarizationTypes = EnumerationParser::getPolarizationType1D("polarizationTypes","CalFocus",rowDoc);			
		
		
		
	

	
  		
			
					
	  	setWereFixed(Parser::get1DBoolean("wereFixed","CalFocus",rowDoc));
	  			
	  		
		
	

	
  		
			
					
	  	setOffset(Parser::get2DLength("offset","CalFocus",rowDoc));
	  			
	  		
		
	

	
  		
			
					
	  	setOffsetError(Parser::get2DLength("offsetError","CalFocus",rowDoc));
	  			
	  		
		
	

	
  		
			
					
	  	setOffsetWasTied(Parser::get2DBoolean("offsetWasTied","CalFocus",rowDoc));
	  			
	  		
		
	

	
  		
			
					
	  	setReducedChiSquared(Parser::get2DDouble("reducedChiSquared","CalFocus",rowDoc));
	  			
	  		
		
	

	
  		
			
					
	  	setPosition(Parser::get2DLength("position","CalFocus",rowDoc));
	  			
	  		
		
	

	
  		
        if (row.isStr("<polarizationsAveraged>")) {
			
	  		setPolarizationsAveraged(Parser::getBoolean("polarizationsAveraged","CalFocus",rowDoc));
			
		}
 		
	

	
  		
        if (row.isStr("<focusCurveWidth>")) {
			
								
	  		setFocusCurveWidth(Parser::get2DLength("focusCurveWidth","CalFocus",rowDoc));
	  			
	  		
		}
 		
	

	
  		
        if (row.isStr("<focusCurveWidthError>")) {
			
								
	  		setFocusCurveWidthError(Parser::get2DLength("focusCurveWidthError","CalFocus",rowDoc));
	  			
	  		
		}
 		
	

	
  		
        if (row.isStr("<focusCurveWasFixed>")) {
			
								
	  		setFocusCurveWasFixed(Parser::get1DBoolean("focusCurveWasFixed","CalFocus",rowDoc));
	  			
	  		
		}
 		
	

	
  		
        if (row.isStr("<offIntensity>")) {
			
								
	  		setOffIntensity(Parser::get1DTemperature("offIntensity","CalFocus",rowDoc));
	  			
	  		
		}
 		
	

	
  		
        if (row.isStr("<offIntensityError>")) {
			
								
	  		setOffIntensityError(Parser::get1DTemperature("offIntensityError","CalFocus",rowDoc));
	  			
	  		
		}
 		
	

	
  		
        if (row.isStr("<offIntensityWasFixed>")) {
			
	  		setOffIntensityWasFixed(Parser::getBoolean("offIntensityWasFixed","CalFocus",rowDoc));
			
		}
 		
	

	
  		
        if (row.isStr("<peakIntensity>")) {
			
								
	  		setPeakIntensity(Parser::get1DTemperature("peakIntensity","CalFocus",rowDoc));
	  			
	  		
		}
 		
	

	
  		
        if (row.isStr("<peakIntensityError>")) {
			
								
	  		setPeakIntensityError(Parser::get1DTemperature("peakIntensityError","CalFocus",rowDoc));
	  			
	  		
		}
 		
	

	
  		
        if (row.isStr("<peakIntensityWasFixed>")) {
			
	  		setPeakIntensityWasFixed(Parser::getBoolean("peakIntensityWasFixed","CalFocus",rowDoc));
			
		}
 		
	

	
  		
        if (row.isStr("<astigmPlus>")) {
			
								
	  		setAstigmPlus(Parser::get1DLength("astigmPlus","CalFocus",rowDoc));
	  			
	  		
		}
 		
	

	
  		
        if (row.isStr("<astigmPlusError>")) {
			
								
	  		setAstigmPlusError(Parser::get1DLength("astigmPlusError","CalFocus",rowDoc));
	  			
	  		
		}
 		
	

	
  		
        if (row.isStr("<astigmMult>")) {
			
								
	  		setAstigmMult(Parser::get1DLength("astigmMult","CalFocus",rowDoc));
	  			
	  		
		}
 		
	

	
  		
        if (row.isStr("<astigmMultError>")) {
			
								
	  		setAstigmMultError(Parser::get1DLength("astigmMultError","CalFocus",rowDoc));
	  			
	  		
		}
 		
	

	
  		
        if (row.isStr("<illumOffset>")) {
			
								
	  		setIllumOffset(Parser::get2DLength("illumOffset","CalFocus",rowDoc));
	  			
	  		
		}
 		
	

	
  		
        if (row.isStr("<illumOffsetError>")) {
			
								
	  		setIllumOffsetError(Parser::get2DLength("illumOffsetError","CalFocus",rowDoc));
	  			
	  		
		}
 		
	

	
  		
        if (row.isStr("<fitRMS>")) {
			
								
	  		setFitRMS(Parser::get1DLength("fitRMS","CalFocus",rowDoc));
	  			
	  		
		}
 		
	

	
	
		
	
  		
			
	  	setCalDataId(Parser::getTag("calDataId","CalData",rowDoc));
			
		
	

	
  		
			
	  	setCalReductionId(Parser::getTag("calReductionId","CalReduction",rowDoc));
			
		
	

	
		
	

	

		} catch (const IllegalAccessException &err) {
			throw ConversionException (err.getMessage(),"CalFocus");
		}
	}
	
	void CalFocusRow::toBin(EndianOSStream& eoss) {
	
	
	
	
		
						
			eoss.writeString(antennaName);
				
		
	

	
	
		
					
		eoss.writeString(CReceiverBand::name(receiverBand));
			/* eoss.writeInt(receiverBand); */
				
		
	

	
	
		
	calDataId.toBin(eoss);
		
	

	
	
		
	calReductionId.toBin(eoss);
		
	

	
	
		
	startValidTime.toBin(eoss);
		
	

	
	
		
	endValidTime.toBin(eoss);
		
	

	
	
		
	ambientTemperature.toBin(eoss);
		
	

	
	
		
					
		eoss.writeString(CAtmPhaseCorrection::name(atmPhaseCorrection));
			/* eoss.writeInt(atmPhaseCorrection); */
				
		
	

	
	
		
					
		eoss.writeString(CFocusMethod::name(focusMethod));
			/* eoss.writeInt(focusMethod); */
				
		
	

	
	
		
	Frequency::toBin(frequencyRange, eoss);
		
	

	
	
		
	Angle::toBin(pointingDirection, eoss);
		
	

	
	
		
						
			eoss.writeInt(numReceptor);
				
		
	

	
	
		
		
			
		eoss.writeInt((int) polarizationTypes.size());
		for (unsigned int i = 0; i < polarizationTypes.size(); i++)
				
			eoss.writeString(CPolarizationType::name(polarizationTypes.at(i)));
			/* eoss.writeInt(polarizationTypes.at(i)); */
				
				
						
		
	

	
	
		
		
			
		eoss.writeInt((int) wereFixed.size());
		for (unsigned int i = 0; i < wereFixed.size(); i++)
				
			eoss.writeBoolean(wereFixed.at(i));
				
				
						
		
	

	
	
		
	Length::toBin(offset, eoss);
		
	

	
	
		
	Length::toBin(offsetError, eoss);
		
	

	
	
		
		
			
		eoss.writeInt((int) offsetWasTied.size());
		eoss.writeInt((int) offsetWasTied.at(0).size());
		for (unsigned int i = 0; i < offsetWasTied.size(); i++) 
			for (unsigned int j = 0;  j < offsetWasTied.at(0).size(); j++) 
							 
				eoss.writeBoolean(offsetWasTied.at(i).at(j));
				
	
						
		
	

	
	
		
		
			
		eoss.writeInt((int) reducedChiSquared.size());
		eoss.writeInt((int) reducedChiSquared.at(0).size());
		for (unsigned int i = 0; i < reducedChiSquared.size(); i++) 
			for (unsigned int j = 0;  j < reducedChiSquared.at(0).size(); j++) 
							 
				eoss.writeDouble(reducedChiSquared.at(i).at(j));
				
	
						
		
	

	
	
		
	Length::toBin(position, eoss);
		
	


	
	
	eoss.writeBoolean(polarizationsAveragedExists);
	if (polarizationsAveragedExists) {
	
	
	
		
						
			eoss.writeBoolean(polarizationsAveraged);
				
		
	

	}

	eoss.writeBoolean(focusCurveWidthExists);
	if (focusCurveWidthExists) {
	
	
	
		
	Length::toBin(focusCurveWidth, eoss);
		
	

	}

	eoss.writeBoolean(focusCurveWidthErrorExists);
	if (focusCurveWidthErrorExists) {
	
	
	
		
	Length::toBin(focusCurveWidthError, eoss);
		
	

	}

	eoss.writeBoolean(focusCurveWasFixedExists);
	if (focusCurveWasFixedExists) {
	
	
	
		
		
			
		eoss.writeInt((int) focusCurveWasFixed.size());
		for (unsigned int i = 0; i < focusCurveWasFixed.size(); i++)
				
			eoss.writeBoolean(focusCurveWasFixed.at(i));
				
				
						
		
	

	}

	eoss.writeBoolean(offIntensityExists);
	if (offIntensityExists) {
	
	
	
		
	Temperature::toBin(offIntensity, eoss);
		
	

	}

	eoss.writeBoolean(offIntensityErrorExists);
	if (offIntensityErrorExists) {
	
	
	
		
	Temperature::toBin(offIntensityError, eoss);
		
	

	}

	eoss.writeBoolean(offIntensityWasFixedExists);
	if (offIntensityWasFixedExists) {
	
	
	
		
						
			eoss.writeBoolean(offIntensityWasFixed);
				
		
	

	}

	eoss.writeBoolean(peakIntensityExists);
	if (peakIntensityExists) {
	
	
	
		
	Temperature::toBin(peakIntensity, eoss);
		
	

	}

	eoss.writeBoolean(peakIntensityErrorExists);
	if (peakIntensityErrorExists) {
	
	
	
		
	Temperature::toBin(peakIntensityError, eoss);
		
	

	}

	eoss.writeBoolean(peakIntensityWasFixedExists);
	if (peakIntensityWasFixedExists) {
	
	
	
		
						
			eoss.writeBoolean(peakIntensityWasFixed);
				
		
	

	}

	eoss.writeBoolean(astigmPlusExists);
	if (astigmPlusExists) {
	
	
	
		
	Length::toBin(astigmPlus, eoss);
		
	

	}

	eoss.writeBoolean(astigmPlusErrorExists);
	if (astigmPlusErrorExists) {
	
	
	
		
	Length::toBin(astigmPlusError, eoss);
		
	

	}

	eoss.writeBoolean(astigmMultExists);
	if (astigmMultExists) {
	
	
	
		
	Length::toBin(astigmMult, eoss);
		
	

	}

	eoss.writeBoolean(astigmMultErrorExists);
	if (astigmMultErrorExists) {
	
	
	
		
	Length::toBin(astigmMultError, eoss);
		
	

	}

	eoss.writeBoolean(illumOffsetExists);
	if (illumOffsetExists) {
	
	
	
		
	Length::toBin(illumOffset, eoss);
		
	

	}

	eoss.writeBoolean(illumOffsetErrorExists);
	if (illumOffsetErrorExists) {
	
	
	
		
	Length::toBin(illumOffsetError, eoss);
		
	

	}

	eoss.writeBoolean(fitRMSExists);
	if (fitRMSExists) {
	
	
	
		
	Length::toBin(fitRMS, eoss);
		
	

	}

	}
	
void CalFocusRow::antennaNameFromBin(EndianIStream& eis) {
		
	
	
		
			
		antennaName =  eis.readString();
			
		
	
	
}
void CalFocusRow::receiverBandFromBin(EndianIStream& eis) {
		
	
	
		
			
		receiverBand = CReceiverBand::literal(eis.readString());
			
		
	
	
}
void CalFocusRow::calDataIdFromBin(EndianIStream& eis) {
		
	
		
		
		calDataId =  Tag::fromBin(eis);
		
	
	
}
void CalFocusRow::calReductionIdFromBin(EndianIStream& eis) {
		
	
		
		
		calReductionId =  Tag::fromBin(eis);
		
	
	
}
void CalFocusRow::startValidTimeFromBin(EndianIStream& eis) {
		
	
		
		
		startValidTime =  ArrayTime::fromBin(eis);
		
	
	
}
void CalFocusRow::endValidTimeFromBin(EndianIStream& eis) {
		
	
		
		
		endValidTime =  ArrayTime::fromBin(eis);
		
	
	
}
void CalFocusRow::ambientTemperatureFromBin(EndianIStream& eis) {
		
	
		
		
		ambientTemperature =  Temperature::fromBin(eis);
		
	
	
}
void CalFocusRow::atmPhaseCorrectionFromBin(EndianIStream& eis) {
		
	
	
		
			
		atmPhaseCorrection = CAtmPhaseCorrection::literal(eis.readString());
			
		
	
	
}
void CalFocusRow::focusMethodFromBin(EndianIStream& eis) {
		
	
	
		
			
		focusMethod = CFocusMethod::literal(eis.readString());
			
		
	
	
}
void CalFocusRow::frequencyRangeFromBin(EndianIStream& eis) {
		
	
		
		
			
	
	frequencyRange = Frequency::from1DBin(eis);	
	

		
	
	
}
void CalFocusRow::pointingDirectionFromBin(EndianIStream& eis) {
		
	
		
		
			
	
	pointingDirection = Angle::from1DBin(eis);	
	

		
	
	
}
void CalFocusRow::numReceptorFromBin(EndianIStream& eis) {
		
	
	
		
			
		numReceptor =  eis.readInt();
			
		
	
	
}
void CalFocusRow::polarizationTypesFromBin(EndianIStream& eis) {
		
	
	
		
			
	
		polarizationTypes.clear();
		
		unsigned int polarizationTypesDim1 = eis.readInt();
		for (unsigned int  i = 0 ; i < polarizationTypesDim1; i++)
			
			polarizationTypes.push_back(CPolarizationType::literal(eis.readString()));
			
	

		
	
	
}
void CalFocusRow::wereFixedFromBin(EndianIStream& eis) {
		
	
	
		
			
	
		wereFixed.clear();
		
		unsigned int wereFixedDim1 = eis.readInt();
		for (unsigned int  i = 0 ; i < wereFixedDim1; i++)
			
			wereFixed.push_back(eis.readBoolean());
			
	

		
	
	
}
void CalFocusRow::offsetFromBin(EndianIStream& eis) {
		
	
		
		
			
	
	offset = Length::from2DBin(eis);		
	

		
	
	
}
void CalFocusRow::offsetErrorFromBin(EndianIStream& eis) {
		
	
		
		
			
	
	offsetError = Length::from2DBin(eis);		
	

		
	
	
}
void CalFocusRow::offsetWasTiedFromBin(EndianIStream& eis) {
		
	
	
		
			
	
		offsetWasTied.clear();
		
		unsigned int offsetWasTiedDim1 = eis.readInt();
		unsigned int offsetWasTiedDim2 = eis.readInt();
        
		vector <bool> offsetWasTiedAux1;
        
		for (unsigned int i = 0; i < offsetWasTiedDim1; i++) {
			offsetWasTiedAux1.clear();
			for (unsigned int j = 0; j < offsetWasTiedDim2 ; j++)			
			
			offsetWasTiedAux1.push_back(eis.readBoolean());
			
			offsetWasTied.push_back(offsetWasTiedAux1);
		}
	
	

		
	
	
}
void CalFocusRow::reducedChiSquaredFromBin(EndianIStream& eis) {
		
	
	
		
			
	
		reducedChiSquared.clear();
		
		unsigned int reducedChiSquaredDim1 = eis.readInt();
		unsigned int reducedChiSquaredDim2 = eis.readInt();
        
		vector <double> reducedChiSquaredAux1;
        
		for (unsigned int i = 0; i < reducedChiSquaredDim1; i++) {
			reducedChiSquaredAux1.clear();
			for (unsigned int j = 0; j < reducedChiSquaredDim2 ; j++)			
			
			reducedChiSquaredAux1.push_back(eis.readDouble());
			
			reducedChiSquared.push_back(reducedChiSquaredAux1);
		}
	
	

		
	
	
}
void CalFocusRow::positionFromBin(EndianIStream& eis) {
		
	
		
		
			
	
	position = Length::from2DBin(eis);		
	

		
	
	
}

void CalFocusRow::polarizationsAveragedFromBin(EndianIStream& eis) {
		
	polarizationsAveragedExists = eis.readBoolean();
	if (polarizationsAveragedExists) {
		
	
	
		
			
		polarizationsAveraged =  eis.readBoolean();
			
		
	

	}
	
}
void CalFocusRow::focusCurveWidthFromBin(EndianIStream& eis) {
		
	focusCurveWidthExists = eis.readBoolean();
	if (focusCurveWidthExists) {
		
	
		
		
			
	
	focusCurveWidth = Length::from2DBin(eis);		
	

		
	

	}
	
}
void CalFocusRow::focusCurveWidthErrorFromBin(EndianIStream& eis) {
		
	focusCurveWidthErrorExists = eis.readBoolean();
	if (focusCurveWidthErrorExists) {
		
	
		
		
			
	
	focusCurveWidthError = Length::from2DBin(eis);		
	

		
	

	}
	
}
void CalFocusRow::focusCurveWasFixedFromBin(EndianIStream& eis) {
		
	focusCurveWasFixedExists = eis.readBoolean();
	if (focusCurveWasFixedExists) {
		
	
	
		
			
	
		focusCurveWasFixed.clear();
		
		unsigned int focusCurveWasFixedDim1 = eis.readInt();
		for (unsigned int  i = 0 ; i < focusCurveWasFixedDim1; i++)
			
			focusCurveWasFixed.push_back(eis.readBoolean());
			
	

		
	

	}
	
}
void CalFocusRow::offIntensityFromBin(EndianIStream& eis) {
		
	offIntensityExists = eis.readBoolean();
	if (offIntensityExists) {
		
	
		
		
			
	
	offIntensity = Temperature::from1DBin(eis);	
	

		
	

	}
	
}
void CalFocusRow::offIntensityErrorFromBin(EndianIStream& eis) {
		
	offIntensityErrorExists = eis.readBoolean();
	if (offIntensityErrorExists) {
		
	
		
		
			
	
	offIntensityError = Temperature::from1DBin(eis);	
	

		
	

	}
	
}
void CalFocusRow::offIntensityWasFixedFromBin(EndianIStream& eis) {
		
	offIntensityWasFixedExists = eis.readBoolean();
	if (offIntensityWasFixedExists) {
		
	
	
		
			
		offIntensityWasFixed =  eis.readBoolean();
			
		
	

	}
	
}
void CalFocusRow::peakIntensityFromBin(EndianIStream& eis) {
		
	peakIntensityExists = eis.readBoolean();
	if (peakIntensityExists) {
		
	
		
		
			
	
	peakIntensity = Temperature::from1DBin(eis);	
	

		
	

	}
	
}
void CalFocusRow::peakIntensityErrorFromBin(EndianIStream& eis) {
		
	peakIntensityErrorExists = eis.readBoolean();
	if (peakIntensityErrorExists) {
		
	
		
		
			
	
	peakIntensityError = Temperature::from1DBin(eis);	
	

		
	

	}
	
}
void CalFocusRow::peakIntensityWasFixedFromBin(EndianIStream& eis) {
		
	peakIntensityWasFixedExists = eis.readBoolean();
	if (peakIntensityWasFixedExists) {
		
	
	
		
			
		peakIntensityWasFixed =  eis.readBoolean();
			
		
	

	}
	
}
void CalFocusRow::astigmPlusFromBin(EndianIStream& eis) {
		
	astigmPlusExists = eis.readBoolean();
	if (astigmPlusExists) {
		
	
		
		
			
	
	astigmPlus = Length::from1DBin(eis);	
	

		
	

	}
	
}
void CalFocusRow::astigmPlusErrorFromBin(EndianIStream& eis) {
		
	astigmPlusErrorExists = eis.readBoolean();
	if (astigmPlusErrorExists) {
		
	
		
		
			
	
	astigmPlusError = Length::from1DBin(eis);	
	

		
	

	}
	
}
void CalFocusRow::astigmMultFromBin(EndianIStream& eis) {
		
	astigmMultExists = eis.readBoolean();
	if (astigmMultExists) {
		
	
		
		
			
	
	astigmMult = Length::from1DBin(eis);	
	

		
	

	}
	
}
void CalFocusRow::astigmMultErrorFromBin(EndianIStream& eis) {
		
	astigmMultErrorExists = eis.readBoolean();
	if (astigmMultErrorExists) {
		
	
		
		
			
	
	astigmMultError = Length::from1DBin(eis);	
	

		
	

	}
	
}
void CalFocusRow::illumOffsetFromBin(EndianIStream& eis) {
		
	illumOffsetExists = eis.readBoolean();
	if (illumOffsetExists) {
		
	
		
		
			
	
	illumOffset = Length::from2DBin(eis);		
	

		
	

	}
	
}
void CalFocusRow::illumOffsetErrorFromBin(EndianIStream& eis) {
		
	illumOffsetErrorExists = eis.readBoolean();
	if (illumOffsetErrorExists) {
		
	
		
		
			
	
	illumOffsetError = Length::from2DBin(eis);		
	

		
	

	}
	
}
void CalFocusRow::fitRMSFromBin(EndianIStream& eis) {
		
	fitRMSExists = eis.readBoolean();
	if (fitRMSExists) {
		
	
		
		
			
	
	fitRMS = Length::from1DBin(eis);	
	

		
	

	}
	
}
	
	
	CalFocusRow* CalFocusRow::fromBin(EndianIStream& eis, CalFocusTable& table, const vector<string>& attributesSeq) {
		CalFocusRow* row = new  CalFocusRow(table);
		
		map<string, CalFocusAttributeFromBin>::iterator iter ;
		for (unsigned int i = 0; i < attributesSeq.size(); i++) {
			iter = row->fromBinMethods.find(attributesSeq.at(i));
			if (iter != row->fromBinMethods.end()) {
				(row->*(row->fromBinMethods[ attributesSeq.at(i) ] ))(eis);			
			}
			else {
				BinaryAttributeReaderFunctor* functorP = table.getUnknownAttributeBinaryReader(attributesSeq.at(i));
				if (functorP)
					(*functorP)(eis);
				else
					throw ConversionException("There is not method to read an attribute '"+attributesSeq.at(i)+"'.", "CalFocusTable");
			}
				
		}				
		return row;
	}

	//
	// A collection of methods to set the value of the attributes from their textual value in the XML representation
	// of one row.
	//
	
	// Convert a string into an String 
	void CalFocusRow::antennaNameFromText(const string & s) {
		 
          
		antennaName = ASDMValuesParser::parse<string>(s);
          
		
	}
	
	
	// Convert a string into an ReceiverBand 
	void CalFocusRow::receiverBandFromText(const string & s) {
		 
          
		receiverBand = ASDMValuesParser::parse<ReceiverBandMod::ReceiverBand>(s);
          
		
	}
	
	
	// Convert a string into an Tag 
	void CalFocusRow::calDataIdFromText(const string & s) {
		 
          
		calDataId = ASDMValuesParser::parse<Tag>(s);
          
		
	}
	
	
	// Convert a string into an Tag 
	void CalFocusRow::calReductionIdFromText(const string & s) {
		 
          
		calReductionId = ASDMValuesParser::parse<Tag>(s);
          
		
	}
	
	
	// Convert a string into an ArrayTime 
	void CalFocusRow::startValidTimeFromText(const string & s) {
		 
          
		startValidTime = ASDMValuesParser::parse<ArrayTime>(s);
          
		
	}
	
	
	// Convert a string into an ArrayTime 
	void CalFocusRow::endValidTimeFromText(const string & s) {
		 
          
		endValidTime = ASDMValuesParser::parse<ArrayTime>(s);
          
		
	}
	
	
	// Convert a string into an Temperature 
	void CalFocusRow::ambientTemperatureFromText(const string & s) {
		 
          
		ambientTemperature = ASDMValuesParser::parse<Temperature>(s);
          
		
	}
	
	
	// Convert a string into an AtmPhaseCorrection 
	void CalFocusRow::atmPhaseCorrectionFromText(const string & s) {
		 
          
		atmPhaseCorrection = ASDMValuesParser::parse<AtmPhaseCorrectionMod::AtmPhaseCorrection>(s);
          
		
	}
	
	
	// Convert a string into an FocusMethod 
	void CalFocusRow::focusMethodFromText(const string & s) {
		 
          
		focusMethod = ASDMValuesParser::parse<FocusMethodMod::FocusMethod>(s);
          
		
	}
	
	
	// Convert a string into an Frequency 
	void CalFocusRow::frequencyRangeFromText(const string & s) {
		 
          
		frequencyRange = ASDMValuesParser::parse1D<Frequency>(s);
          
		
	}
	
	
	// Convert a string into an Angle 
	void CalFocusRow::pointingDirectionFromText(const string & s) {
		 
          
		pointingDirection = ASDMValuesParser::parse1D<Angle>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void CalFocusRow::numReceptorFromText(const string & s) {
		 
          
		numReceptor = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	// Convert a string into an PolarizationType 
	void CalFocusRow::polarizationTypesFromText(const string & s) {
		 
          
		polarizationTypes = ASDMValuesParser::parse1D<PolarizationTypeMod::PolarizationType>(s);
          
		
	}
	
	
	// Convert a string into an boolean 
	void CalFocusRow::wereFixedFromText(const string & s) {
		 
          
		wereFixed = ASDMValuesParser::parse1D<bool>(s);
          
		
	}
	
	
	// Convert a string into an Length 
	void CalFocusRow::offsetFromText(const string & s) {
		 
          
		offset = ASDMValuesParser::parse2D<Length>(s);
          
		
	}
	
	
	// Convert a string into an Length 
	void CalFocusRow::offsetErrorFromText(const string & s) {
		 
          
		offsetError = ASDMValuesParser::parse2D<Length>(s);
          
		
	}
	
	
	// Convert a string into an boolean 
	void CalFocusRow::offsetWasTiedFromText(const string & s) {
		 
          
		offsetWasTied = ASDMValuesParser::parse2D<bool>(s);
          
		
	}
	
	
	// Convert a string into an double 
	void CalFocusRow::reducedChiSquaredFromText(const string & s) {
		 
          
		reducedChiSquared = ASDMValuesParser::parse2D<double>(s);
          
		
	}
	
	
	// Convert a string into an Length 
	void CalFocusRow::positionFromText(const string & s) {
		 
          
		position = ASDMValuesParser::parse2D<Length>(s);
          
		
	}
	

	
	// Convert a string into an boolean 
	void CalFocusRow::polarizationsAveragedFromText(const string & s) {
		polarizationsAveragedExists = true;
		 
          
		polarizationsAveraged = ASDMValuesParser::parse<bool>(s);
          
		
	}
	
	
	// Convert a string into an Length 
	void CalFocusRow::focusCurveWidthFromText(const string & s) {
		focusCurveWidthExists = true;
		 
          
		focusCurveWidth = ASDMValuesParser::parse2D<Length>(s);
          
		
	}
	
	
	// Convert a string into an Length 
	void CalFocusRow::focusCurveWidthErrorFromText(const string & s) {
		focusCurveWidthErrorExists = true;
		 
          
		focusCurveWidthError = ASDMValuesParser::parse2D<Length>(s);
          
		
	}
	
	
	// Convert a string into an boolean 
	void CalFocusRow::focusCurveWasFixedFromText(const string & s) {
		focusCurveWasFixedExists = true;
		 
          
		focusCurveWasFixed = ASDMValuesParser::parse1D<bool>(s);
          
		
	}
	
	
	// Convert a string into an Temperature 
	void CalFocusRow::offIntensityFromText(const string & s) {
		offIntensityExists = true;
		 
          
		offIntensity = ASDMValuesParser::parse1D<Temperature>(s);
          
		
	}
	
	
	// Convert a string into an Temperature 
	void CalFocusRow::offIntensityErrorFromText(const string & s) {
		offIntensityErrorExists = true;
		 
          
		offIntensityError = ASDMValuesParser::parse1D<Temperature>(s);
          
		
	}
	
	
	// Convert a string into an boolean 
	void CalFocusRow::offIntensityWasFixedFromText(const string & s) {
		offIntensityWasFixedExists = true;
		 
          
		offIntensityWasFixed = ASDMValuesParser::parse<bool>(s);
          
		
	}
	
	
	// Convert a string into an Temperature 
	void CalFocusRow::peakIntensityFromText(const string & s) {
		peakIntensityExists = true;
		 
          
		peakIntensity = ASDMValuesParser::parse1D<Temperature>(s);
          
		
	}
	
	
	// Convert a string into an Temperature 
	void CalFocusRow::peakIntensityErrorFromText(const string & s) {
		peakIntensityErrorExists = true;
		 
          
		peakIntensityError = ASDMValuesParser::parse1D<Temperature>(s);
          
		
	}
	
	
	// Convert a string into an boolean 
	void CalFocusRow::peakIntensityWasFixedFromText(const string & s) {
		peakIntensityWasFixedExists = true;
		 
          
		peakIntensityWasFixed = ASDMValuesParser::parse<bool>(s);
          
		
	}
	
	
	// Convert a string into an Length 
	void CalFocusRow::astigmPlusFromText(const string & s) {
		astigmPlusExists = true;
		 
          
		astigmPlus = ASDMValuesParser::parse1D<Length>(s);
          
		
	}
	
	
	// Convert a string into an Length 
	void CalFocusRow::astigmPlusErrorFromText(const string & s) {
		astigmPlusErrorExists = true;
		 
          
		astigmPlusError = ASDMValuesParser::parse1D<Length>(s);
          
		
	}
	
	
	// Convert a string into an Length 
	void CalFocusRow::astigmMultFromText(const string & s) {
		astigmMultExists = true;
		 
          
		astigmMult = ASDMValuesParser::parse1D<Length>(s);
          
		
	}
	
	
	// Convert a string into an Length 
	void CalFocusRow::astigmMultErrorFromText(const string & s) {
		astigmMultErrorExists = true;
		 
          
		astigmMultError = ASDMValuesParser::parse1D<Length>(s);
          
		
	}
	
	
	// Convert a string into an Length 
	void CalFocusRow::illumOffsetFromText(const string & s) {
		illumOffsetExists = true;
		 
          
		illumOffset = ASDMValuesParser::parse2D<Length>(s);
          
		
	}
	
	
	// Convert a string into an Length 
	void CalFocusRow::illumOffsetErrorFromText(const string & s) {
		illumOffsetErrorExists = true;
		 
          
		illumOffsetError = ASDMValuesParser::parse2D<Length>(s);
          
		
	}
	
	
	// Convert a string into an Length 
	void CalFocusRow::fitRMSFromText(const string & s) {
		fitRMSExists = true;
		 
          
		fitRMS = ASDMValuesParser::parse1D<Length>(s);
          
		
	}
	
	
	
	void CalFocusRow::fromText(const std::string& attributeName, const std::string&  t) {
		map<string, CalFocusAttributeFromText>::iterator iter;
		if ((iter = fromTextMethods.find(attributeName)) == fromTextMethods.end())
			throw ConversionException("I do not know what to do with '"+attributeName+"' and its content '"+t+"' (while parsing an XML document)", "CalFocusTable");
		(this->*(iter->second))(t);
	}
			
	////////////////////////////////////////////////
	// Intrinsic Table Attributes getters/setters //
	////////////////////////////////////////////////
	
	

	
 	/**
 	 * Get startValidTime.
 	 * @return startValidTime as ArrayTime
 	 */
 	ArrayTime CalFocusRow::getStartValidTime() const {
	
  		return startValidTime;
 	}

 	/**
 	 * Set startValidTime with the specified ArrayTime.
 	 * @param startValidTime The ArrayTime value to which startValidTime is to be set.
 	 
 	
 		
 	 */
 	void CalFocusRow::setStartValidTime (ArrayTime startValidTime)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->startValidTime = startValidTime;
	
 	}
	
	

	

	
 	/**
 	 * Get endValidTime.
 	 * @return endValidTime as ArrayTime
 	 */
 	ArrayTime CalFocusRow::getEndValidTime() const {
	
  		return endValidTime;
 	}

 	/**
 	 * Set endValidTime with the specified ArrayTime.
 	 * @param endValidTime The ArrayTime value to which endValidTime is to be set.
 	 
 	
 		
 	 */
 	void CalFocusRow::setEndValidTime (ArrayTime endValidTime)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->endValidTime = endValidTime;
	
 	}
	
	

	

	
 	/**
 	 * Get antennaName.
 	 * @return antennaName as std::string
 	 */
 	std::string CalFocusRow::getAntennaName() const {
	
  		return antennaName;
 	}

 	/**
 	 * Set antennaName with the specified std::string.
 	 * @param antennaName The std::string value to which antennaName is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void CalFocusRow::setAntennaName (std::string antennaName)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("antennaName", "CalFocus");
		
  		}
  	
 		this->antennaName = antennaName;
	
 	}
	
	

	

	
 	/**
 	 * Get receiverBand.
 	 * @return receiverBand as ReceiverBandMod::ReceiverBand
 	 */
 	ReceiverBandMod::ReceiverBand CalFocusRow::getReceiverBand() const {
	
  		return receiverBand;
 	}

 	/**
 	 * Set receiverBand with the specified ReceiverBandMod::ReceiverBand.
 	 * @param receiverBand The ReceiverBandMod::ReceiverBand value to which receiverBand is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void CalFocusRow::setReceiverBand (ReceiverBandMod::ReceiverBand receiverBand)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("receiverBand", "CalFocus");
		
  		}
  	
 		this->receiverBand = receiverBand;
	
 	}
	
	

	

	
 	/**
 	 * Get ambientTemperature.
 	 * @return ambientTemperature as Temperature
 	 */
 	Temperature CalFocusRow::getAmbientTemperature() const {
	
  		return ambientTemperature;
 	}

 	/**
 	 * Set ambientTemperature with the specified Temperature.
 	 * @param ambientTemperature The Temperature value to which ambientTemperature is to be set.
 	 
 	
 		
 	 */
 	void CalFocusRow::setAmbientTemperature (Temperature ambientTemperature)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->ambientTemperature = ambientTemperature;
	
 	}
	
	

	

	
 	/**
 	 * Get atmPhaseCorrection.
 	 * @return atmPhaseCorrection as AtmPhaseCorrectionMod::AtmPhaseCorrection
 	 */
 	AtmPhaseCorrectionMod::AtmPhaseCorrection CalFocusRow::getAtmPhaseCorrection() const {
	
  		return atmPhaseCorrection;
 	}

 	/**
 	 * Set atmPhaseCorrection with the specified AtmPhaseCorrectionMod::AtmPhaseCorrection.
 	 * @param atmPhaseCorrection The AtmPhaseCorrectionMod::AtmPhaseCorrection value to which atmPhaseCorrection is to be set.
 	 
 	
 		
 	 */
 	void CalFocusRow::setAtmPhaseCorrection (AtmPhaseCorrectionMod::AtmPhaseCorrection atmPhaseCorrection)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->atmPhaseCorrection = atmPhaseCorrection;
	
 	}
	
	

	

	
 	/**
 	 * Get focusMethod.
 	 * @return focusMethod as FocusMethodMod::FocusMethod
 	 */
 	FocusMethodMod::FocusMethod CalFocusRow::getFocusMethod() const {
	
  		return focusMethod;
 	}

 	/**
 	 * Set focusMethod with the specified FocusMethodMod::FocusMethod.
 	 * @param focusMethod The FocusMethodMod::FocusMethod value to which focusMethod is to be set.
 	 
 	
 		
 	 */
 	void CalFocusRow::setFocusMethod (FocusMethodMod::FocusMethod focusMethod)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->focusMethod = focusMethod;
	
 	}
	
	

	

	
 	/**
 	 * Get frequencyRange.
 	 * @return frequencyRange as std::vector<Frequency >
 	 */
 	std::vector<Frequency > CalFocusRow::getFrequencyRange() const {
	
  		return frequencyRange;
 	}

 	/**
 	 * Set frequencyRange with the specified std::vector<Frequency >.
 	 * @param frequencyRange The std::vector<Frequency > value to which frequencyRange is to be set.
 	 
 	
 		
 	 */
 	void CalFocusRow::setFrequencyRange (std::vector<Frequency > frequencyRange)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->frequencyRange = frequencyRange;
	
 	}
	
	

	

	
 	/**
 	 * Get pointingDirection.
 	 * @return pointingDirection as std::vector<Angle >
 	 */
 	std::vector<Angle > CalFocusRow::getPointingDirection() const {
	
  		return pointingDirection;
 	}

 	/**
 	 * Set pointingDirection with the specified std::vector<Angle >.
 	 * @param pointingDirection The std::vector<Angle > value to which pointingDirection is to be set.
 	 
 	
 		
 	 */
 	void CalFocusRow::setPointingDirection (std::vector<Angle > pointingDirection)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->pointingDirection = pointingDirection;
	
 	}
	
	

	

	
 	/**
 	 * Get numReceptor.
 	 * @return numReceptor as int
 	 */
 	int CalFocusRow::getNumReceptor() const {
	
  		return numReceptor;
 	}

 	/**
 	 * Set numReceptor with the specified int.
 	 * @param numReceptor The int value to which numReceptor is to be set.
 	 
 	
 		
 	 */
 	void CalFocusRow::setNumReceptor (int numReceptor)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->numReceptor = numReceptor;
	
 	}
	
	

	

	
 	/**
 	 * Get polarizationTypes.
 	 * @return polarizationTypes as std::vector<PolarizationTypeMod::PolarizationType >
 	 */
 	std::vector<PolarizationTypeMod::PolarizationType > CalFocusRow::getPolarizationTypes() const {
	
  		return polarizationTypes;
 	}

 	/**
 	 * Set polarizationTypes with the specified std::vector<PolarizationTypeMod::PolarizationType >.
 	 * @param polarizationTypes The std::vector<PolarizationTypeMod::PolarizationType > value to which polarizationTypes is to be set.
 	 
 	
 		
 	 */
 	void CalFocusRow::setPolarizationTypes (std::vector<PolarizationTypeMod::PolarizationType > polarizationTypes)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->polarizationTypes = polarizationTypes;
	
 	}
	
	

	

	
 	/**
 	 * Get wereFixed.
 	 * @return wereFixed as std::vector<bool >
 	 */
 	std::vector<bool > CalFocusRow::getWereFixed() const {
	
  		return wereFixed;
 	}

 	/**
 	 * Set wereFixed with the specified std::vector<bool >.
 	 * @param wereFixed The std::vector<bool > value to which wereFixed is to be set.
 	 
 	
 		
 	 */
 	void CalFocusRow::setWereFixed (std::vector<bool > wereFixed)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->wereFixed = wereFixed;
	
 	}
	
	

	

	
 	/**
 	 * Get offset.
 	 * @return offset as std::vector<std::vector<Length > >
 	 */
 	std::vector<std::vector<Length > > CalFocusRow::getOffset() const {
	
  		return offset;
 	}

 	/**
 	 * Set offset with the specified std::vector<std::vector<Length > >.
 	 * @param offset The std::vector<std::vector<Length > > value to which offset is to be set.
 	 
 	
 		
 	 */
 	void CalFocusRow::setOffset (std::vector<std::vector<Length > > offset)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->offset = offset;
	
 	}
	
	

	

	
 	/**
 	 * Get offsetError.
 	 * @return offsetError as std::vector<std::vector<Length > >
 	 */
 	std::vector<std::vector<Length > > CalFocusRow::getOffsetError() const {
	
  		return offsetError;
 	}

 	/**
 	 * Set offsetError with the specified std::vector<std::vector<Length > >.
 	 * @param offsetError The std::vector<std::vector<Length > > value to which offsetError is to be set.
 	 
 	
 		
 	 */
 	void CalFocusRow::setOffsetError (std::vector<std::vector<Length > > offsetError)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->offsetError = offsetError;
	
 	}
	
	

	

	
 	/**
 	 * Get offsetWasTied.
 	 * @return offsetWasTied as std::vector<std::vector<bool > >
 	 */
 	std::vector<std::vector<bool > > CalFocusRow::getOffsetWasTied() const {
	
  		return offsetWasTied;
 	}

 	/**
 	 * Set offsetWasTied with the specified std::vector<std::vector<bool > >.
 	 * @param offsetWasTied The std::vector<std::vector<bool > > value to which offsetWasTied is to be set.
 	 
 	
 		
 	 */
 	void CalFocusRow::setOffsetWasTied (std::vector<std::vector<bool > > offsetWasTied)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->offsetWasTied = offsetWasTied;
	
 	}
	
	

	

	
 	/**
 	 * Get reducedChiSquared.
 	 * @return reducedChiSquared as std::vector<std::vector<double > >
 	 */
 	std::vector<std::vector<double > > CalFocusRow::getReducedChiSquared() const {
	
  		return reducedChiSquared;
 	}

 	/**
 	 * Set reducedChiSquared with the specified std::vector<std::vector<double > >.
 	 * @param reducedChiSquared The std::vector<std::vector<double > > value to which reducedChiSquared is to be set.
 	 
 	
 		
 	 */
 	void CalFocusRow::setReducedChiSquared (std::vector<std::vector<double > > reducedChiSquared)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->reducedChiSquared = reducedChiSquared;
	
 	}
	
	

	

	
 	/**
 	 * Get position.
 	 * @return position as std::vector<std::vector<Length > >
 	 */
 	std::vector<std::vector<Length > > CalFocusRow::getPosition() const {
	
  		return position;
 	}

 	/**
 	 * Set position with the specified std::vector<std::vector<Length > >.
 	 * @param position The std::vector<std::vector<Length > > value to which position is to be set.
 	 
 	
 		
 	 */
 	void CalFocusRow::setPosition (std::vector<std::vector<Length > > position)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->position = position;
	
 	}
	
	

	
	/**
	 * The attribute polarizationsAveraged is optional. Return true if this attribute exists.
	 * @return true if and only if the polarizationsAveraged attribute exists. 
	 */
	bool CalFocusRow::isPolarizationsAveragedExists() const {
		return polarizationsAveragedExists;
	}
	

	
 	/**
 	 * Get polarizationsAveraged, which is optional.
 	 * @return polarizationsAveraged as bool
 	 * @throw IllegalAccessException If polarizationsAveraged does not exist.
 	 */
 	bool CalFocusRow::getPolarizationsAveraged() const  {
		if (!polarizationsAveragedExists) {
			throw IllegalAccessException("polarizationsAveraged", "CalFocus");
		}
	
  		return polarizationsAveraged;
 	}

 	/**
 	 * Set polarizationsAveraged with the specified bool.
 	 * @param polarizationsAveraged The bool value to which polarizationsAveraged is to be set.
 	 
 	
 	 */
 	void CalFocusRow::setPolarizationsAveraged (bool polarizationsAveraged) {
	
 		this->polarizationsAveraged = polarizationsAveraged;
	
		polarizationsAveragedExists = true;
	
 	}
	
	
	/**
	 * Mark polarizationsAveraged, which is an optional field, as non-existent.
	 */
	void CalFocusRow::clearPolarizationsAveraged () {
		polarizationsAveragedExists = false;
	}
	

	
	/**
	 * The attribute focusCurveWidth is optional. Return true if this attribute exists.
	 * @return true if and only if the focusCurveWidth attribute exists. 
	 */
	bool CalFocusRow::isFocusCurveWidthExists() const {
		return focusCurveWidthExists;
	}
	

	
 	/**
 	 * Get focusCurveWidth, which is optional.
 	 * @return focusCurveWidth as std::vector<std::vector<Length > >
 	 * @throw IllegalAccessException If focusCurveWidth does not exist.
 	 */
 	std::vector<std::vector<Length > > CalFocusRow::getFocusCurveWidth() const  {
		if (!focusCurveWidthExists) {
			throw IllegalAccessException("focusCurveWidth", "CalFocus");
		}
	
  		return focusCurveWidth;
 	}

 	/**
 	 * Set focusCurveWidth with the specified std::vector<std::vector<Length > >.
 	 * @param focusCurveWidth The std::vector<std::vector<Length > > value to which focusCurveWidth is to be set.
 	 
 	
 	 */
 	void CalFocusRow::setFocusCurveWidth (std::vector<std::vector<Length > > focusCurveWidth) {
	
 		this->focusCurveWidth = focusCurveWidth;
	
		focusCurveWidthExists = true;
	
 	}
	
	
	/**
	 * Mark focusCurveWidth, which is an optional field, as non-existent.
	 */
	void CalFocusRow::clearFocusCurveWidth () {
		focusCurveWidthExists = false;
	}
	

	
	/**
	 * The attribute focusCurveWidthError is optional. Return true if this attribute exists.
	 * @return true if and only if the focusCurveWidthError attribute exists. 
	 */
	bool CalFocusRow::isFocusCurveWidthErrorExists() const {
		return focusCurveWidthErrorExists;
	}
	

	
 	/**
 	 * Get focusCurveWidthError, which is optional.
 	 * @return focusCurveWidthError as std::vector<std::vector<Length > >
 	 * @throw IllegalAccessException If focusCurveWidthError does not exist.
 	 */
 	std::vector<std::vector<Length > > CalFocusRow::getFocusCurveWidthError() const  {
		if (!focusCurveWidthErrorExists) {
			throw IllegalAccessException("focusCurveWidthError", "CalFocus");
		}
	
  		return focusCurveWidthError;
 	}

 	/**
 	 * Set focusCurveWidthError with the specified std::vector<std::vector<Length > >.
 	 * @param focusCurveWidthError The std::vector<std::vector<Length > > value to which focusCurveWidthError is to be set.
 	 
 	
 	 */
 	void CalFocusRow::setFocusCurveWidthError (std::vector<std::vector<Length > > focusCurveWidthError) {
	
 		this->focusCurveWidthError = focusCurveWidthError;
	
		focusCurveWidthErrorExists = true;
	
 	}
	
	
	/**
	 * Mark focusCurveWidthError, which is an optional field, as non-existent.
	 */
	void CalFocusRow::clearFocusCurveWidthError () {
		focusCurveWidthErrorExists = false;
	}
	

	
	/**
	 * The attribute focusCurveWasFixed is optional. Return true if this attribute exists.
	 * @return true if and only if the focusCurveWasFixed attribute exists. 
	 */
	bool CalFocusRow::isFocusCurveWasFixedExists() const {
		return focusCurveWasFixedExists;
	}
	

	
 	/**
 	 * Get focusCurveWasFixed, which is optional.
 	 * @return focusCurveWasFixed as std::vector<bool >
 	 * @throw IllegalAccessException If focusCurveWasFixed does not exist.
 	 */
 	std::vector<bool > CalFocusRow::getFocusCurveWasFixed() const  {
		if (!focusCurveWasFixedExists) {
			throw IllegalAccessException("focusCurveWasFixed", "CalFocus");
		}
	
  		return focusCurveWasFixed;
 	}

 	/**
 	 * Set focusCurveWasFixed with the specified std::vector<bool >.
 	 * @param focusCurveWasFixed The std::vector<bool > value to which focusCurveWasFixed is to be set.
 	 
 	
 	 */
 	void CalFocusRow::setFocusCurveWasFixed (std::vector<bool > focusCurveWasFixed) {
	
 		this->focusCurveWasFixed = focusCurveWasFixed;
	
		focusCurveWasFixedExists = true;
	
 	}
	
	
	/**
	 * Mark focusCurveWasFixed, which is an optional field, as non-existent.
	 */
	void CalFocusRow::clearFocusCurveWasFixed () {
		focusCurveWasFixedExists = false;
	}
	

	
	/**
	 * The attribute offIntensity is optional. Return true if this attribute exists.
	 * @return true if and only if the offIntensity attribute exists. 
	 */
	bool CalFocusRow::isOffIntensityExists() const {
		return offIntensityExists;
	}
	

	
 	/**
 	 * Get offIntensity, which is optional.
 	 * @return offIntensity as std::vector<Temperature >
 	 * @throw IllegalAccessException If offIntensity does not exist.
 	 */
 	std::vector<Temperature > CalFocusRow::getOffIntensity() const  {
		if (!offIntensityExists) {
			throw IllegalAccessException("offIntensity", "CalFocus");
		}
	
  		return offIntensity;
 	}

 	/**
 	 * Set offIntensity with the specified std::vector<Temperature >.
 	 * @param offIntensity The std::vector<Temperature > value to which offIntensity is to be set.
 	 
 	
 	 */
 	void CalFocusRow::setOffIntensity (std::vector<Temperature > offIntensity) {
	
 		this->offIntensity = offIntensity;
	
		offIntensityExists = true;
	
 	}
	
	
	/**
	 * Mark offIntensity, which is an optional field, as non-existent.
	 */
	void CalFocusRow::clearOffIntensity () {
		offIntensityExists = false;
	}
	

	
	/**
	 * The attribute offIntensityError is optional. Return true if this attribute exists.
	 * @return true if and only if the offIntensityError attribute exists. 
	 */
	bool CalFocusRow::isOffIntensityErrorExists() const {
		return offIntensityErrorExists;
	}
	

	
 	/**
 	 * Get offIntensityError, which is optional.
 	 * @return offIntensityError as std::vector<Temperature >
 	 * @throw IllegalAccessException If offIntensityError does not exist.
 	 */
 	std::vector<Temperature > CalFocusRow::getOffIntensityError() const  {
		if (!offIntensityErrorExists) {
			throw IllegalAccessException("offIntensityError", "CalFocus");
		}
	
  		return offIntensityError;
 	}

 	/**
 	 * Set offIntensityError with the specified std::vector<Temperature >.
 	 * @param offIntensityError The std::vector<Temperature > value to which offIntensityError is to be set.
 	 
 	
 	 */
 	void CalFocusRow::setOffIntensityError (std::vector<Temperature > offIntensityError) {
	
 		this->offIntensityError = offIntensityError;
	
		offIntensityErrorExists = true;
	
 	}
	
	
	/**
	 * Mark offIntensityError, which is an optional field, as non-existent.
	 */
	void CalFocusRow::clearOffIntensityError () {
		offIntensityErrorExists = false;
	}
	

	
	/**
	 * The attribute offIntensityWasFixed is optional. Return true if this attribute exists.
	 * @return true if and only if the offIntensityWasFixed attribute exists. 
	 */
	bool CalFocusRow::isOffIntensityWasFixedExists() const {
		return offIntensityWasFixedExists;
	}
	

	
 	/**
 	 * Get offIntensityWasFixed, which is optional.
 	 * @return offIntensityWasFixed as bool
 	 * @throw IllegalAccessException If offIntensityWasFixed does not exist.
 	 */
 	bool CalFocusRow::getOffIntensityWasFixed() const  {
		if (!offIntensityWasFixedExists) {
			throw IllegalAccessException("offIntensityWasFixed", "CalFocus");
		}
	
  		return offIntensityWasFixed;
 	}

 	/**
 	 * Set offIntensityWasFixed with the specified bool.
 	 * @param offIntensityWasFixed The bool value to which offIntensityWasFixed is to be set.
 	 
 	
 	 */
 	void CalFocusRow::setOffIntensityWasFixed (bool offIntensityWasFixed) {
	
 		this->offIntensityWasFixed = offIntensityWasFixed;
	
		offIntensityWasFixedExists = true;
	
 	}
	
	
	/**
	 * Mark offIntensityWasFixed, which is an optional field, as non-existent.
	 */
	void CalFocusRow::clearOffIntensityWasFixed () {
		offIntensityWasFixedExists = false;
	}
	

	
	/**
	 * The attribute peakIntensity is optional. Return true if this attribute exists.
	 * @return true if and only if the peakIntensity attribute exists. 
	 */
	bool CalFocusRow::isPeakIntensityExists() const {
		return peakIntensityExists;
	}
	

	
 	/**
 	 * Get peakIntensity, which is optional.
 	 * @return peakIntensity as std::vector<Temperature >
 	 * @throw IllegalAccessException If peakIntensity does not exist.
 	 */
 	std::vector<Temperature > CalFocusRow::getPeakIntensity() const  {
		if (!peakIntensityExists) {
			throw IllegalAccessException("peakIntensity", "CalFocus");
		}
	
  		return peakIntensity;
 	}

 	/**
 	 * Set peakIntensity with the specified std::vector<Temperature >.
 	 * @param peakIntensity The std::vector<Temperature > value to which peakIntensity is to be set.
 	 
 	
 	 */
 	void CalFocusRow::setPeakIntensity (std::vector<Temperature > peakIntensity) {
	
 		this->peakIntensity = peakIntensity;
	
		peakIntensityExists = true;
	
 	}
	
	
	/**
	 * Mark peakIntensity, which is an optional field, as non-existent.
	 */
	void CalFocusRow::clearPeakIntensity () {
		peakIntensityExists = false;
	}
	

	
	/**
	 * The attribute peakIntensityError is optional. Return true if this attribute exists.
	 * @return true if and only if the peakIntensityError attribute exists. 
	 */
	bool CalFocusRow::isPeakIntensityErrorExists() const {
		return peakIntensityErrorExists;
	}
	

	
 	/**
 	 * Get peakIntensityError, which is optional.
 	 * @return peakIntensityError as std::vector<Temperature >
 	 * @throw IllegalAccessException If peakIntensityError does not exist.
 	 */
 	std::vector<Temperature > CalFocusRow::getPeakIntensityError() const  {
		if (!peakIntensityErrorExists) {
			throw IllegalAccessException("peakIntensityError", "CalFocus");
		}
	
  		return peakIntensityError;
 	}

 	/**
 	 * Set peakIntensityError with the specified std::vector<Temperature >.
 	 * @param peakIntensityError The std::vector<Temperature > value to which peakIntensityError is to be set.
 	 
 	
 	 */
 	void CalFocusRow::setPeakIntensityError (std::vector<Temperature > peakIntensityError) {
	
 		this->peakIntensityError = peakIntensityError;
	
		peakIntensityErrorExists = true;
	
 	}
	
	
	/**
	 * Mark peakIntensityError, which is an optional field, as non-existent.
	 */
	void CalFocusRow::clearPeakIntensityError () {
		peakIntensityErrorExists = false;
	}
	

	
	/**
	 * The attribute peakIntensityWasFixed is optional. Return true if this attribute exists.
	 * @return true if and only if the peakIntensityWasFixed attribute exists. 
	 */
	bool CalFocusRow::isPeakIntensityWasFixedExists() const {
		return peakIntensityWasFixedExists;
	}
	

	
 	/**
 	 * Get peakIntensityWasFixed, which is optional.
 	 * @return peakIntensityWasFixed as bool
 	 * @throw IllegalAccessException If peakIntensityWasFixed does not exist.
 	 */
 	bool CalFocusRow::getPeakIntensityWasFixed() const  {
		if (!peakIntensityWasFixedExists) {
			throw IllegalAccessException("peakIntensityWasFixed", "CalFocus");
		}
	
  		return peakIntensityWasFixed;
 	}

 	/**
 	 * Set peakIntensityWasFixed with the specified bool.
 	 * @param peakIntensityWasFixed The bool value to which peakIntensityWasFixed is to be set.
 	 
 	
 	 */
 	void CalFocusRow::setPeakIntensityWasFixed (bool peakIntensityWasFixed) {
	
 		this->peakIntensityWasFixed = peakIntensityWasFixed;
	
		peakIntensityWasFixedExists = true;
	
 	}
	
	
	/**
	 * Mark peakIntensityWasFixed, which is an optional field, as non-existent.
	 */
	void CalFocusRow::clearPeakIntensityWasFixed () {
		peakIntensityWasFixedExists = false;
	}
	

	
	/**
	 * The attribute astigmPlus is optional. Return true if this attribute exists.
	 * @return true if and only if the astigmPlus attribute exists. 
	 */
	bool CalFocusRow::isAstigmPlusExists() const {
		return astigmPlusExists;
	}
	

	
 	/**
 	 * Get astigmPlus, which is optional.
 	 * @return astigmPlus as std::vector<Length >
 	 * @throw IllegalAccessException If astigmPlus does not exist.
 	 */
 	std::vector<Length > CalFocusRow::getAstigmPlus() const  {
		if (!astigmPlusExists) {
			throw IllegalAccessException("astigmPlus", "CalFocus");
		}
	
  		return astigmPlus;
 	}

 	/**
 	 * Set astigmPlus with the specified std::vector<Length >.
 	 * @param astigmPlus The std::vector<Length > value to which astigmPlus is to be set.
 	 
 	
 	 */
 	void CalFocusRow::setAstigmPlus (std::vector<Length > astigmPlus) {
	
 		this->astigmPlus = astigmPlus;
	
		astigmPlusExists = true;
	
 	}
	
	
	/**
	 * Mark astigmPlus, which is an optional field, as non-existent.
	 */
	void CalFocusRow::clearAstigmPlus () {
		astigmPlusExists = false;
	}
	

	
	/**
	 * The attribute astigmPlusError is optional. Return true if this attribute exists.
	 * @return true if and only if the astigmPlusError attribute exists. 
	 */
	bool CalFocusRow::isAstigmPlusErrorExists() const {
		return astigmPlusErrorExists;
	}
	

	
 	/**
 	 * Get astigmPlusError, which is optional.
 	 * @return astigmPlusError as std::vector<Length >
 	 * @throw IllegalAccessException If astigmPlusError does not exist.
 	 */
 	std::vector<Length > CalFocusRow::getAstigmPlusError() const  {
		if (!astigmPlusErrorExists) {
			throw IllegalAccessException("astigmPlusError", "CalFocus");
		}
	
  		return astigmPlusError;
 	}

 	/**
 	 * Set astigmPlusError with the specified std::vector<Length >.
 	 * @param astigmPlusError The std::vector<Length > value to which astigmPlusError is to be set.
 	 
 	
 	 */
 	void CalFocusRow::setAstigmPlusError (std::vector<Length > astigmPlusError) {
	
 		this->astigmPlusError = astigmPlusError;
	
		astigmPlusErrorExists = true;
	
 	}
	
	
	/**
	 * Mark astigmPlusError, which is an optional field, as non-existent.
	 */
	void CalFocusRow::clearAstigmPlusError () {
		astigmPlusErrorExists = false;
	}
	

	
	/**
	 * The attribute astigmMult is optional. Return true if this attribute exists.
	 * @return true if and only if the astigmMult attribute exists. 
	 */
	bool CalFocusRow::isAstigmMultExists() const {
		return astigmMultExists;
	}
	

	
 	/**
 	 * Get astigmMult, which is optional.
 	 * @return astigmMult as std::vector<Length >
 	 * @throw IllegalAccessException If astigmMult does not exist.
 	 */
 	std::vector<Length > CalFocusRow::getAstigmMult() const  {
		if (!astigmMultExists) {
			throw IllegalAccessException("astigmMult", "CalFocus");
		}
	
  		return astigmMult;
 	}

 	/**
 	 * Set astigmMult with the specified std::vector<Length >.
 	 * @param astigmMult The std::vector<Length > value to which astigmMult is to be set.
 	 
 	
 	 */
 	void CalFocusRow::setAstigmMult (std::vector<Length > astigmMult) {
	
 		this->astigmMult = astigmMult;
	
		astigmMultExists = true;
	
 	}
	
	
	/**
	 * Mark astigmMult, which is an optional field, as non-existent.
	 */
	void CalFocusRow::clearAstigmMult () {
		astigmMultExists = false;
	}
	

	
	/**
	 * The attribute astigmMultError is optional. Return true if this attribute exists.
	 * @return true if and only if the astigmMultError attribute exists. 
	 */
	bool CalFocusRow::isAstigmMultErrorExists() const {
		return astigmMultErrorExists;
	}
	

	
 	/**
 	 * Get astigmMultError, which is optional.
 	 * @return astigmMultError as std::vector<Length >
 	 * @throw IllegalAccessException If astigmMultError does not exist.
 	 */
 	std::vector<Length > CalFocusRow::getAstigmMultError() const  {
		if (!astigmMultErrorExists) {
			throw IllegalAccessException("astigmMultError", "CalFocus");
		}
	
  		return astigmMultError;
 	}

 	/**
 	 * Set astigmMultError with the specified std::vector<Length >.
 	 * @param astigmMultError The std::vector<Length > value to which astigmMultError is to be set.
 	 
 	
 	 */
 	void CalFocusRow::setAstigmMultError (std::vector<Length > astigmMultError) {
	
 		this->astigmMultError = astigmMultError;
	
		astigmMultErrorExists = true;
	
 	}
	
	
	/**
	 * Mark astigmMultError, which is an optional field, as non-existent.
	 */
	void CalFocusRow::clearAstigmMultError () {
		astigmMultErrorExists = false;
	}
	

	
	/**
	 * The attribute illumOffset is optional. Return true if this attribute exists.
	 * @return true if and only if the illumOffset attribute exists. 
	 */
	bool CalFocusRow::isIllumOffsetExists() const {
		return illumOffsetExists;
	}
	

	
 	/**
 	 * Get illumOffset, which is optional.
 	 * @return illumOffset as std::vector<std::vector<Length > >
 	 * @throw IllegalAccessException If illumOffset does not exist.
 	 */
 	std::vector<std::vector<Length > > CalFocusRow::getIllumOffset() const  {
		if (!illumOffsetExists) {
			throw IllegalAccessException("illumOffset", "CalFocus");
		}
	
  		return illumOffset;
 	}

 	/**
 	 * Set illumOffset with the specified std::vector<std::vector<Length > >.
 	 * @param illumOffset The std::vector<std::vector<Length > > value to which illumOffset is to be set.
 	 
 	
 	 */
 	void CalFocusRow::setIllumOffset (std::vector<std::vector<Length > > illumOffset) {
	
 		this->illumOffset = illumOffset;
	
		illumOffsetExists = true;
	
 	}
	
	
	/**
	 * Mark illumOffset, which is an optional field, as non-existent.
	 */
	void CalFocusRow::clearIllumOffset () {
		illumOffsetExists = false;
	}
	

	
	/**
	 * The attribute illumOffsetError is optional. Return true if this attribute exists.
	 * @return true if and only if the illumOffsetError attribute exists. 
	 */
	bool CalFocusRow::isIllumOffsetErrorExists() const {
		return illumOffsetErrorExists;
	}
	

	
 	/**
 	 * Get illumOffsetError, which is optional.
 	 * @return illumOffsetError as std::vector<std::vector<Length > >
 	 * @throw IllegalAccessException If illumOffsetError does not exist.
 	 */
 	std::vector<std::vector<Length > > CalFocusRow::getIllumOffsetError() const  {
		if (!illumOffsetErrorExists) {
			throw IllegalAccessException("illumOffsetError", "CalFocus");
		}
	
  		return illumOffsetError;
 	}

 	/**
 	 * Set illumOffsetError with the specified std::vector<std::vector<Length > >.
 	 * @param illumOffsetError The std::vector<std::vector<Length > > value to which illumOffsetError is to be set.
 	 
 	
 	 */
 	void CalFocusRow::setIllumOffsetError (std::vector<std::vector<Length > > illumOffsetError) {
	
 		this->illumOffsetError = illumOffsetError;
	
		illumOffsetErrorExists = true;
	
 	}
	
	
	/**
	 * Mark illumOffsetError, which is an optional field, as non-existent.
	 */
	void CalFocusRow::clearIllumOffsetError () {
		illumOffsetErrorExists = false;
	}
	

	
	/**
	 * The attribute fitRMS is optional. Return true if this attribute exists.
	 * @return true if and only if the fitRMS attribute exists. 
	 */
	bool CalFocusRow::isFitRMSExists() const {
		return fitRMSExists;
	}
	

	
 	/**
 	 * Get fitRMS, which is optional.
 	 * @return fitRMS as std::vector<Length >
 	 * @throw IllegalAccessException If fitRMS does not exist.
 	 */
 	std::vector<Length > CalFocusRow::getFitRMS() const  {
		if (!fitRMSExists) {
			throw IllegalAccessException("fitRMS", "CalFocus");
		}
	
  		return fitRMS;
 	}

 	/**
 	 * Set fitRMS with the specified std::vector<Length >.
 	 * @param fitRMS The std::vector<Length > value to which fitRMS is to be set.
 	 
 	
 	 */
 	void CalFocusRow::setFitRMS (std::vector<Length > fitRMS) {
	
 		this->fitRMS = fitRMS;
	
		fitRMSExists = true;
	
 	}
	
	
	/**
	 * Mark fitRMS, which is an optional field, as non-existent.
	 */
	void CalFocusRow::clearFitRMS () {
		fitRMSExists = false;
	}
	

	
	///////////////////////////////////////////////
	// Extrinsic Table Attributes getters/setters//
	///////////////////////////////////////////////
	
	

	
 	/**
 	 * Get calDataId.
 	 * @return calDataId as Tag
 	 */
 	Tag CalFocusRow::getCalDataId() const {
	
  		return calDataId;
 	}

 	/**
 	 * Set calDataId with the specified Tag.
 	 * @param calDataId The Tag value to which calDataId is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void CalFocusRow::setCalDataId (Tag calDataId)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("calDataId", "CalFocus");
		
  		}
  	
 		this->calDataId = calDataId;
	
 	}
	
	

	

	
 	/**
 	 * Get calReductionId.
 	 * @return calReductionId as Tag
 	 */
 	Tag CalFocusRow::getCalReductionId() const {
	
  		return calReductionId;
 	}

 	/**
 	 * Set calReductionId with the specified Tag.
 	 * @param calReductionId The Tag value to which calReductionId is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void CalFocusRow::setCalReductionId (Tag calReductionId)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("calReductionId", "CalFocus");
		
  		}
  	
 		this->calReductionId = calReductionId;
	
 	}
	
	


	//////////////////////////////////////
	// Links Attributes getters/setters //
	//////////////////////////////////////
	
	
	
	
		

	/**
	 * Returns the pointer to the row in the CalData table having CalData.calDataId == calDataId
	 * @return a CalDataRow*
	 * 
	 
	 */
	 CalDataRow* CalFocusRow::getCalDataUsingCalDataId() {
	 
	 	return table.getContainer().getCalData().getRowByKey(calDataId);
	 }
	 

	

	
	
	
		

	/**
	 * Returns the pointer to the row in the CalReduction table having CalReduction.calReductionId == calReductionId
	 * @return a CalReductionRow*
	 * 
	 
	 */
	 CalReductionRow* CalFocusRow::getCalReductionUsingCalReductionId() {
	 
	 	return table.getContainer().getCalReduction().getRowByKey(calReductionId);
	 }
	 

	

	
	/**
	 * Create a CalFocusRow.
	 * <p>
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 */ 
	CalFocusRow::CalFocusRow (CalFocusTable &t) : table(t) {
		hasBeenAdded = false;
		
	
	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	
		polarizationsAveragedExists = false;
	

	
		focusCurveWidthExists = false;
	

	
		focusCurveWidthErrorExists = false;
	

	
		focusCurveWasFixedExists = false;
	

	
		offIntensityExists = false;
	

	
		offIntensityErrorExists = false;
	

	
		offIntensityWasFixedExists = false;
	

	
		peakIntensityExists = false;
	

	
		peakIntensityErrorExists = false;
	

	
		peakIntensityWasFixedExists = false;
	

	
		astigmPlusExists = false;
	

	
		astigmPlusErrorExists = false;
	

	
		astigmMultExists = false;
	

	
		astigmMultErrorExists = false;
	

	
		illumOffsetExists = false;
	

	
		illumOffsetErrorExists = false;
	

	
		fitRMSExists = false;
	

	
	

	

	
	
	
	

	

	

	
// This attribute is scalar and has an enumeration type. Let's initialize it to some valid value (the 1st of the enumeration).		
receiverBand = CReceiverBand::from_int(0);
	

	

	
// This attribute is scalar and has an enumeration type. Let's initialize it to some valid value (the 1st of the enumeration).		
atmPhaseCorrection = CAtmPhaseCorrection::from_int(0);
	

	
// This attribute is scalar and has an enumeration type. Let's initialize it to some valid value (the 1st of the enumeration).		
focusMethod = CFocusMethod::from_int(0);
	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	
	
	 fromBinMethods["antennaName"] = &CalFocusRow::antennaNameFromBin; 
	 fromBinMethods["receiverBand"] = &CalFocusRow::receiverBandFromBin; 
	 fromBinMethods["calDataId"] = &CalFocusRow::calDataIdFromBin; 
	 fromBinMethods["calReductionId"] = &CalFocusRow::calReductionIdFromBin; 
	 fromBinMethods["startValidTime"] = &CalFocusRow::startValidTimeFromBin; 
	 fromBinMethods["endValidTime"] = &CalFocusRow::endValidTimeFromBin; 
	 fromBinMethods["ambientTemperature"] = &CalFocusRow::ambientTemperatureFromBin; 
	 fromBinMethods["atmPhaseCorrection"] = &CalFocusRow::atmPhaseCorrectionFromBin; 
	 fromBinMethods["focusMethod"] = &CalFocusRow::focusMethodFromBin; 
	 fromBinMethods["frequencyRange"] = &CalFocusRow::frequencyRangeFromBin; 
	 fromBinMethods["pointingDirection"] = &CalFocusRow::pointingDirectionFromBin; 
	 fromBinMethods["numReceptor"] = &CalFocusRow::numReceptorFromBin; 
	 fromBinMethods["polarizationTypes"] = &CalFocusRow::polarizationTypesFromBin; 
	 fromBinMethods["wereFixed"] = &CalFocusRow::wereFixedFromBin; 
	 fromBinMethods["offset"] = &CalFocusRow::offsetFromBin; 
	 fromBinMethods["offsetError"] = &CalFocusRow::offsetErrorFromBin; 
	 fromBinMethods["offsetWasTied"] = &CalFocusRow::offsetWasTiedFromBin; 
	 fromBinMethods["reducedChiSquared"] = &CalFocusRow::reducedChiSquaredFromBin; 
	 fromBinMethods["position"] = &CalFocusRow::positionFromBin; 
		
	
	 fromBinMethods["polarizationsAveraged"] = &CalFocusRow::polarizationsAveragedFromBin; 
	 fromBinMethods["focusCurveWidth"] = &CalFocusRow::focusCurveWidthFromBin; 
	 fromBinMethods["focusCurveWidthError"] = &CalFocusRow::focusCurveWidthErrorFromBin; 
	 fromBinMethods["focusCurveWasFixed"] = &CalFocusRow::focusCurveWasFixedFromBin; 
	 fromBinMethods["offIntensity"] = &CalFocusRow::offIntensityFromBin; 
	 fromBinMethods["offIntensityError"] = &CalFocusRow::offIntensityErrorFromBin; 
	 fromBinMethods["offIntensityWasFixed"] = &CalFocusRow::offIntensityWasFixedFromBin; 
	 fromBinMethods["peakIntensity"] = &CalFocusRow::peakIntensityFromBin; 
	 fromBinMethods["peakIntensityError"] = &CalFocusRow::peakIntensityErrorFromBin; 
	 fromBinMethods["peakIntensityWasFixed"] = &CalFocusRow::peakIntensityWasFixedFromBin; 
	 fromBinMethods["astigmPlus"] = &CalFocusRow::astigmPlusFromBin; 
	 fromBinMethods["astigmPlusError"] = &CalFocusRow::astigmPlusErrorFromBin; 
	 fromBinMethods["astigmMult"] = &CalFocusRow::astigmMultFromBin; 
	 fromBinMethods["astigmMultError"] = &CalFocusRow::astigmMultErrorFromBin; 
	 fromBinMethods["illumOffset"] = &CalFocusRow::illumOffsetFromBin; 
	 fromBinMethods["illumOffsetError"] = &CalFocusRow::illumOffsetErrorFromBin; 
	 fromBinMethods["fitRMS"] = &CalFocusRow::fitRMSFromBin; 
	
	
	
	
				 
	fromTextMethods["antennaName"] = &CalFocusRow::antennaNameFromText;
		 
	
				 
	fromTextMethods["receiverBand"] = &CalFocusRow::receiverBandFromText;
		 
	
				 
	fromTextMethods["calDataId"] = &CalFocusRow::calDataIdFromText;
		 
	
				 
	fromTextMethods["calReductionId"] = &CalFocusRow::calReductionIdFromText;
		 
	
				 
	fromTextMethods["startValidTime"] = &CalFocusRow::startValidTimeFromText;
		 
	
				 
	fromTextMethods["endValidTime"] = &CalFocusRow::endValidTimeFromText;
		 
	
				 
	fromTextMethods["ambientTemperature"] = &CalFocusRow::ambientTemperatureFromText;
		 
	
				 
	fromTextMethods["atmPhaseCorrection"] = &CalFocusRow::atmPhaseCorrectionFromText;
		 
	
				 
	fromTextMethods["focusMethod"] = &CalFocusRow::focusMethodFromText;
		 
	
				 
	fromTextMethods["frequencyRange"] = &CalFocusRow::frequencyRangeFromText;
		 
	
				 
	fromTextMethods["pointingDirection"] = &CalFocusRow::pointingDirectionFromText;
		 
	
				 
	fromTextMethods["numReceptor"] = &CalFocusRow::numReceptorFromText;
		 
	
				 
	fromTextMethods["polarizationTypes"] = &CalFocusRow::polarizationTypesFromText;
		 
	
				 
	fromTextMethods["wereFixed"] = &CalFocusRow::wereFixedFromText;
		 
	
				 
	fromTextMethods["offset"] = &CalFocusRow::offsetFromText;
		 
	
				 
	fromTextMethods["offsetError"] = &CalFocusRow::offsetErrorFromText;
		 
	
				 
	fromTextMethods["offsetWasTied"] = &CalFocusRow::offsetWasTiedFromText;
		 
	
				 
	fromTextMethods["reducedChiSquared"] = &CalFocusRow::reducedChiSquaredFromText;
		 
	
				 
	fromTextMethods["position"] = &CalFocusRow::positionFromText;
		 
	

	 
				
	fromTextMethods["polarizationsAveraged"] = &CalFocusRow::polarizationsAveragedFromText;
		 	
	 
				
	fromTextMethods["focusCurveWidth"] = &CalFocusRow::focusCurveWidthFromText;
		 	
	 
				
	fromTextMethods["focusCurveWidthError"] = &CalFocusRow::focusCurveWidthErrorFromText;
		 	
	 
				
	fromTextMethods["focusCurveWasFixed"] = &CalFocusRow::focusCurveWasFixedFromText;
		 	
	 
				
	fromTextMethods["offIntensity"] = &CalFocusRow::offIntensityFromText;
		 	
	 
				
	fromTextMethods["offIntensityError"] = &CalFocusRow::offIntensityErrorFromText;
		 	
	 
				
	fromTextMethods["offIntensityWasFixed"] = &CalFocusRow::offIntensityWasFixedFromText;
		 	
	 
				
	fromTextMethods["peakIntensity"] = &CalFocusRow::peakIntensityFromText;
		 	
	 
				
	fromTextMethods["peakIntensityError"] = &CalFocusRow::peakIntensityErrorFromText;
		 	
	 
				
	fromTextMethods["peakIntensityWasFixed"] = &CalFocusRow::peakIntensityWasFixedFromText;
		 	
	 
				
	fromTextMethods["astigmPlus"] = &CalFocusRow::astigmPlusFromText;
		 	
	 
				
	fromTextMethods["astigmPlusError"] = &CalFocusRow::astigmPlusErrorFromText;
		 	
	 
				
	fromTextMethods["astigmMult"] = &CalFocusRow::astigmMultFromText;
		 	
	 
				
	fromTextMethods["astigmMultError"] = &CalFocusRow::astigmMultErrorFromText;
		 	
	 
				
	fromTextMethods["illumOffset"] = &CalFocusRow::illumOffsetFromText;
		 	
	 
				
	fromTextMethods["illumOffsetError"] = &CalFocusRow::illumOffsetErrorFromText;
		 	
	 
				
	fromTextMethods["fitRMS"] = &CalFocusRow::fitRMSFromText;
		 	
		
	}
	
	CalFocusRow::CalFocusRow (CalFocusTable &t, CalFocusRow *row) : table(t) {
		hasBeenAdded = false;
		
		if (row == 0) {
	
	
	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	
		polarizationsAveragedExists = false;
	

	
		focusCurveWidthExists = false;
	

	
		focusCurveWidthErrorExists = false;
	

	
		focusCurveWasFixedExists = false;
	

	
		offIntensityExists = false;
	

	
		offIntensityErrorExists = false;
	

	
		offIntensityWasFixedExists = false;
	

	
		peakIntensityExists = false;
	

	
		peakIntensityErrorExists = false;
	

	
		peakIntensityWasFixedExists = false;
	

	
		astigmPlusExists = false;
	

	
		astigmPlusErrorExists = false;
	

	
		astigmMultExists = false;
	

	
		astigmMultErrorExists = false;
	

	
		illumOffsetExists = false;
	

	
		illumOffsetErrorExists = false;
	

	
		fitRMSExists = false;
	

	
	

	
		
		}
		else {
	
		
			antennaName = row->antennaName;
		
			receiverBand = row->receiverBand;
		
			calDataId = row->calDataId;
		
			calReductionId = row->calReductionId;
		
		
		
		
			startValidTime = row->startValidTime;
		
			endValidTime = row->endValidTime;
		
			ambientTemperature = row->ambientTemperature;
		
			atmPhaseCorrection = row->atmPhaseCorrection;
		
			focusMethod = row->focusMethod;
		
			frequencyRange = row->frequencyRange;
		
			pointingDirection = row->pointingDirection;
		
			numReceptor = row->numReceptor;
		
			polarizationTypes = row->polarizationTypes;
		
			wereFixed = row->wereFixed;
		
			offset = row->offset;
		
			offsetError = row->offsetError;
		
			offsetWasTied = row->offsetWasTied;
		
			reducedChiSquared = row->reducedChiSquared;
		
			position = row->position;
		
		
		
		
		if (row->polarizationsAveragedExists) {
			polarizationsAveraged = row->polarizationsAveraged;		
			polarizationsAveragedExists = true;
		}
		else
			polarizationsAveragedExists = false;
		
		if (row->focusCurveWidthExists) {
			focusCurveWidth = row->focusCurveWidth;		
			focusCurveWidthExists = true;
		}
		else
			focusCurveWidthExists = false;
		
		if (row->focusCurveWidthErrorExists) {
			focusCurveWidthError = row->focusCurveWidthError;		
			focusCurveWidthErrorExists = true;
		}
		else
			focusCurveWidthErrorExists = false;
		
		if (row->focusCurveWasFixedExists) {
			focusCurveWasFixed = row->focusCurveWasFixed;		
			focusCurveWasFixedExists = true;
		}
		else
			focusCurveWasFixedExists = false;
		
		if (row->offIntensityExists) {
			offIntensity = row->offIntensity;		
			offIntensityExists = true;
		}
		else
			offIntensityExists = false;
		
		if (row->offIntensityErrorExists) {
			offIntensityError = row->offIntensityError;		
			offIntensityErrorExists = true;
		}
		else
			offIntensityErrorExists = false;
		
		if (row->offIntensityWasFixedExists) {
			offIntensityWasFixed = row->offIntensityWasFixed;		
			offIntensityWasFixedExists = true;
		}
		else
			offIntensityWasFixedExists = false;
		
		if (row->peakIntensityExists) {
			peakIntensity = row->peakIntensity;		
			peakIntensityExists = true;
		}
		else
			peakIntensityExists = false;
		
		if (row->peakIntensityErrorExists) {
			peakIntensityError = row->peakIntensityError;		
			peakIntensityErrorExists = true;
		}
		else
			peakIntensityErrorExists = false;
		
		if (row->peakIntensityWasFixedExists) {
			peakIntensityWasFixed = row->peakIntensityWasFixed;		
			peakIntensityWasFixedExists = true;
		}
		else
			peakIntensityWasFixedExists = false;
		
		if (row->astigmPlusExists) {
			astigmPlus = row->astigmPlus;		
			astigmPlusExists = true;
		}
		else
			astigmPlusExists = false;
		
		if (row->astigmPlusErrorExists) {
			astigmPlusError = row->astigmPlusError;		
			astigmPlusErrorExists = true;
		}
		else
			astigmPlusErrorExists = false;
		
		if (row->astigmMultExists) {
			astigmMult = row->astigmMult;		
			astigmMultExists = true;
		}
		else
			astigmMultExists = false;
		
		if (row->astigmMultErrorExists) {
			astigmMultError = row->astigmMultError;		
			astigmMultErrorExists = true;
		}
		else
			astigmMultErrorExists = false;
		
		if (row->illumOffsetExists) {
			illumOffset = row->illumOffset;		
			illumOffsetExists = true;
		}
		else
			illumOffsetExists = false;
		
		if (row->illumOffsetErrorExists) {
			illumOffsetError = row->illumOffsetError;		
			illumOffsetErrorExists = true;
		}
		else
			illumOffsetErrorExists = false;
		
		if (row->fitRMSExists) {
			fitRMS = row->fitRMS;		
			fitRMSExists = true;
		}
		else
			fitRMSExists = false;
		
		}
		
		 fromBinMethods["antennaName"] = &CalFocusRow::antennaNameFromBin; 
		 fromBinMethods["receiverBand"] = &CalFocusRow::receiverBandFromBin; 
		 fromBinMethods["calDataId"] = &CalFocusRow::calDataIdFromBin; 
		 fromBinMethods["calReductionId"] = &CalFocusRow::calReductionIdFromBin; 
		 fromBinMethods["startValidTime"] = &CalFocusRow::startValidTimeFromBin; 
		 fromBinMethods["endValidTime"] = &CalFocusRow::endValidTimeFromBin; 
		 fromBinMethods["ambientTemperature"] = &CalFocusRow::ambientTemperatureFromBin; 
		 fromBinMethods["atmPhaseCorrection"] = &CalFocusRow::atmPhaseCorrectionFromBin; 
		 fromBinMethods["focusMethod"] = &CalFocusRow::focusMethodFromBin; 
		 fromBinMethods["frequencyRange"] = &CalFocusRow::frequencyRangeFromBin; 
		 fromBinMethods["pointingDirection"] = &CalFocusRow::pointingDirectionFromBin; 
		 fromBinMethods["numReceptor"] = &CalFocusRow::numReceptorFromBin; 
		 fromBinMethods["polarizationTypes"] = &CalFocusRow::polarizationTypesFromBin; 
		 fromBinMethods["wereFixed"] = &CalFocusRow::wereFixedFromBin; 
		 fromBinMethods["offset"] = &CalFocusRow::offsetFromBin; 
		 fromBinMethods["offsetError"] = &CalFocusRow::offsetErrorFromBin; 
		 fromBinMethods["offsetWasTied"] = &CalFocusRow::offsetWasTiedFromBin; 
		 fromBinMethods["reducedChiSquared"] = &CalFocusRow::reducedChiSquaredFromBin; 
		 fromBinMethods["position"] = &CalFocusRow::positionFromBin; 
			
	
		 fromBinMethods["polarizationsAveraged"] = &CalFocusRow::polarizationsAveragedFromBin; 
		 fromBinMethods["focusCurveWidth"] = &CalFocusRow::focusCurveWidthFromBin; 
		 fromBinMethods["focusCurveWidthError"] = &CalFocusRow::focusCurveWidthErrorFromBin; 
		 fromBinMethods["focusCurveWasFixed"] = &CalFocusRow::focusCurveWasFixedFromBin; 
		 fromBinMethods["offIntensity"] = &CalFocusRow::offIntensityFromBin; 
		 fromBinMethods["offIntensityError"] = &CalFocusRow::offIntensityErrorFromBin; 
		 fromBinMethods["offIntensityWasFixed"] = &CalFocusRow::offIntensityWasFixedFromBin; 
		 fromBinMethods["peakIntensity"] = &CalFocusRow::peakIntensityFromBin; 
		 fromBinMethods["peakIntensityError"] = &CalFocusRow::peakIntensityErrorFromBin; 
		 fromBinMethods["peakIntensityWasFixed"] = &CalFocusRow::peakIntensityWasFixedFromBin; 
		 fromBinMethods["astigmPlus"] = &CalFocusRow::astigmPlusFromBin; 
		 fromBinMethods["astigmPlusError"] = &CalFocusRow::astigmPlusErrorFromBin; 
		 fromBinMethods["astigmMult"] = &CalFocusRow::astigmMultFromBin; 
		 fromBinMethods["astigmMultError"] = &CalFocusRow::astigmMultErrorFromBin; 
		 fromBinMethods["illumOffset"] = &CalFocusRow::illumOffsetFromBin; 
		 fromBinMethods["illumOffsetError"] = &CalFocusRow::illumOffsetErrorFromBin; 
		 fromBinMethods["fitRMS"] = &CalFocusRow::fitRMSFromBin; 
			
	}

	
	bool CalFocusRow::compareNoAutoInc(std::string antennaName, ReceiverBandMod::ReceiverBand receiverBand, Tag calDataId, Tag calReductionId, ArrayTime startValidTime, ArrayTime endValidTime, Temperature ambientTemperature, AtmPhaseCorrectionMod::AtmPhaseCorrection atmPhaseCorrection, FocusMethodMod::FocusMethod focusMethod, std::vector<Frequency > frequencyRange, std::vector<Angle > pointingDirection, int numReceptor, std::vector<PolarizationTypeMod::PolarizationType > polarizationTypes, std::vector<bool > wereFixed, std::vector<std::vector<Length > > offset, std::vector<std::vector<Length > > offsetError, std::vector<std::vector<bool > > offsetWasTied, std::vector<std::vector<double > > reducedChiSquared, std::vector<std::vector<Length > > position) {
		bool result;
		result = true;
		
	
		
		result = result && (this->antennaName == antennaName);
		
		if (!result) return false;
	

	
		
		result = result && (this->receiverBand == receiverBand);
		
		if (!result) return false;
	

	
		
		result = result && (this->calDataId == calDataId);
		
		if (!result) return false;
	

	
		
		result = result && (this->calReductionId == calReductionId);
		
		if (!result) return false;
	

	
		
		result = result && (this->startValidTime == startValidTime);
		
		if (!result) return false;
	

	
		
		result = result && (this->endValidTime == endValidTime);
		
		if (!result) return false;
	

	
		
		result = result && (this->ambientTemperature == ambientTemperature);
		
		if (!result) return false;
	

	
		
		result = result && (this->atmPhaseCorrection == atmPhaseCorrection);
		
		if (!result) return false;
	

	
		
		result = result && (this->focusMethod == focusMethod);
		
		if (!result) return false;
	

	
		
		result = result && (this->frequencyRange == frequencyRange);
		
		if (!result) return false;
	

	
		
		result = result && (this->pointingDirection == pointingDirection);
		
		if (!result) return false;
	

	
		
		result = result && (this->numReceptor == numReceptor);
		
		if (!result) return false;
	

	
		
		result = result && (this->polarizationTypes == polarizationTypes);
		
		if (!result) return false;
	

	
		
		result = result && (this->wereFixed == wereFixed);
		
		if (!result) return false;
	

	
		
		result = result && (this->offset == offset);
		
		if (!result) return false;
	

	
		
		result = result && (this->offsetError == offsetError);
		
		if (!result) return false;
	

	
		
		result = result && (this->offsetWasTied == offsetWasTied);
		
		if (!result) return false;
	

	
		
		result = result && (this->reducedChiSquared == reducedChiSquared);
		
		if (!result) return false;
	

	
		
		result = result && (this->position == position);
		
		if (!result) return false;
	

		return result;
	}	
	
	
	
	bool CalFocusRow::compareRequiredValue(ArrayTime startValidTime, ArrayTime endValidTime, Temperature ambientTemperature, AtmPhaseCorrectionMod::AtmPhaseCorrection atmPhaseCorrection, FocusMethodMod::FocusMethod focusMethod, std::vector<Frequency > frequencyRange, std::vector<Angle > pointingDirection, int numReceptor, std::vector<PolarizationTypeMod::PolarizationType > polarizationTypes, std::vector<bool > wereFixed, std::vector<std::vector<Length > > offset, std::vector<std::vector<Length > > offsetError, std::vector<std::vector<bool > > offsetWasTied, std::vector<std::vector<double > > reducedChiSquared, std::vector<std::vector<Length > > position) {
		bool result;
		result = true;
		
	
		if (!(this->startValidTime == startValidTime)) return false;
	

	
		if (!(this->endValidTime == endValidTime)) return false;
	

	
		if (!(this->ambientTemperature == ambientTemperature)) return false;
	

	
		if (!(this->atmPhaseCorrection == atmPhaseCorrection)) return false;
	

	
		if (!(this->focusMethod == focusMethod)) return false;
	

	
		if (!(this->frequencyRange == frequencyRange)) return false;
	

	
		if (!(this->pointingDirection == pointingDirection)) return false;
	

	
		if (!(this->numReceptor == numReceptor)) return false;
	

	
		if (!(this->polarizationTypes == polarizationTypes)) return false;
	

	
		if (!(this->wereFixed == wereFixed)) return false;
	

	
		if (!(this->offset == offset)) return false;
	

	
		if (!(this->offsetError == offsetError)) return false;
	

	
		if (!(this->offsetWasTied == offsetWasTied)) return false;
	

	
		if (!(this->reducedChiSquared == reducedChiSquared)) return false;
	

	
		if (!(this->position == position)) return false;
	

		return result;
	}
	
	
	/**
	 * Return true if all required attributes of the value part are equal to their homologues
	 * in x and false otherwise.
	 *

	 * @param x a pointer on the CalFocusRow whose required attributes of the value part 

	 * will be compared with those of this.
	 * @return a boolean.
	 */
	bool CalFocusRow::equalByRequiredValue(CalFocusRow*  x ) {
		
			
		if (this->startValidTime != x->startValidTime) return false;
			
		if (this->endValidTime != x->endValidTime) return false;
			
		if (this->ambientTemperature != x->ambientTemperature) return false;
			
		if (this->atmPhaseCorrection != x->atmPhaseCorrection) return false;
			
		if (this->focusMethod != x->focusMethod) return false;
			
		if (this->frequencyRange != x->frequencyRange) return false;
			
		if (this->pointingDirection != x->pointingDirection) return false;
			
		if (this->numReceptor != x->numReceptor) return false;
			
		if (this->polarizationTypes != x->polarizationTypes) return false;
			
		if (this->wereFixed != x->wereFixed) return false;
			
		if (this->offset != x->offset) return false;
			
		if (this->offsetError != x->offsetError) return false;
			
		if (this->offsetWasTied != x->offsetWasTied) return false;
			
		if (this->reducedChiSquared != x->reducedChiSquared) return false;
			
		if (this->position != x->position) return false;
			
		
		return true;
	}	
	
/*
	 map<string, CalFocusAttributeFromBin> CalFocusRow::initFromBinMethods() {
		map<string, CalFocusAttributeFromBin> result;
		
		result["antennaName"] = &CalFocusRow::antennaNameFromBin;
		result["receiverBand"] = &CalFocusRow::receiverBandFromBin;
		result["calDataId"] = &CalFocusRow::calDataIdFromBin;
		result["calReductionId"] = &CalFocusRow::calReductionIdFromBin;
		result["startValidTime"] = &CalFocusRow::startValidTimeFromBin;
		result["endValidTime"] = &CalFocusRow::endValidTimeFromBin;
		result["ambientTemperature"] = &CalFocusRow::ambientTemperatureFromBin;
		result["atmPhaseCorrection"] = &CalFocusRow::atmPhaseCorrectionFromBin;
		result["focusMethod"] = &CalFocusRow::focusMethodFromBin;
		result["frequencyRange"] = &CalFocusRow::frequencyRangeFromBin;
		result["pointingDirection"] = &CalFocusRow::pointingDirectionFromBin;
		result["numReceptor"] = &CalFocusRow::numReceptorFromBin;
		result["polarizationTypes"] = &CalFocusRow::polarizationTypesFromBin;
		result["wereFixed"] = &CalFocusRow::wereFixedFromBin;
		result["offset"] = &CalFocusRow::offsetFromBin;
		result["offsetError"] = &CalFocusRow::offsetErrorFromBin;
		result["offsetWasTied"] = &CalFocusRow::offsetWasTiedFromBin;
		result["reducedChiSquared"] = &CalFocusRow::reducedChiSquaredFromBin;
		result["position"] = &CalFocusRow::positionFromBin;
		
		
		result["polarizationsAveraged"] = &CalFocusRow::polarizationsAveragedFromBin;
		result["focusCurveWidth"] = &CalFocusRow::focusCurveWidthFromBin;
		result["focusCurveWidthError"] = &CalFocusRow::focusCurveWidthErrorFromBin;
		result["focusCurveWasFixed"] = &CalFocusRow::focusCurveWasFixedFromBin;
		result["offIntensity"] = &CalFocusRow::offIntensityFromBin;
		result["offIntensityError"] = &CalFocusRow::offIntensityErrorFromBin;
		result["offIntensityWasFixed"] = &CalFocusRow::offIntensityWasFixedFromBin;
		result["peakIntensity"] = &CalFocusRow::peakIntensityFromBin;
		result["peakIntensityError"] = &CalFocusRow::peakIntensityErrorFromBin;
		result["peakIntensityWasFixed"] = &CalFocusRow::peakIntensityWasFixedFromBin;
		result["astigmPlus"] = &CalFocusRow::astigmPlusFromBin;
		result["astigmPlusError"] = &CalFocusRow::astigmPlusErrorFromBin;
		result["astigmMult"] = &CalFocusRow::astigmMultFromBin;
		result["astigmMultError"] = &CalFocusRow::astigmMultErrorFromBin;
		result["illumOffset"] = &CalFocusRow::illumOffsetFromBin;
		result["illumOffsetError"] = &CalFocusRow::illumOffsetErrorFromBin;
		result["fitRMS"] = &CalFocusRow::fitRMSFromBin;
			
		
		return result;	
	}
*/	
} // End namespace asdm
 
