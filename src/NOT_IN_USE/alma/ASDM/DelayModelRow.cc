
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
 * File DelayModelRow.cpp
 */
 
#include <vector>
#include <set>

#include <alma/ASDM/ASDM.h>
#include <alma/ASDM/DelayModelRow.h>
#include <alma/ASDM/DelayModelTable.h>

#include <alma/ASDM/AntennaTable.h>
#include <alma/ASDM/AntennaRow.h>

#include <alma/ASDM/SpectralWindowTable.h>
#include <alma/ASDM/SpectralWindowRow.h>

#include <alma/ASDM/FieldTable.h>
#include <alma/ASDM/FieldRow.h>
	

using asdm::ASDM;
using asdm::DelayModelRow;
using asdm::DelayModelTable;

using asdm::AntennaTable;
using asdm::AntennaRow;

using asdm::SpectralWindowTable;
using asdm::SpectralWindowRow;

using asdm::FieldTable;
using asdm::FieldRow;


#include <alma/ASDM/Parser.h>

#include <alma/ASDM/EnumerationParser.h>
#include <alma/ASDM/ASDMValuesParser.h>
 
#include <alma/ASDM/InvalidArgumentException.h>

using namespace std;

namespace asdm {
	DelayModelRow::~DelayModelRow() {
	}

	/**
	 * Return the table to which this row belongs.
	 */
	DelayModelTable &DelayModelRow::getTable() const {
		return table;
	}

	bool DelayModelRow::isAdded() const {
		return hasBeenAdded;
	}	

	void DelayModelRow::isAdded(bool added) {
		hasBeenAdded = added;
	}
	
#ifndef WITHOUT_ACS
	using asdmIDL::DelayModelRowIDL;
#endif
	
#ifndef WITHOUT_ACS
	/**
	 * Return this row in the form of an IDL struct.
	 * @return The values of this row as a DelayModelRowIDL struct.
	 */
	DelayModelRowIDL *DelayModelRow::toIDL() const {
		DelayModelRowIDL *x = new DelayModelRowIDL ();
		
		// Fill the IDL structure.
	
		
	
  		
		
		
			
		x->timeInterval = timeInterval.toIDLArrayTimeInterval();
			
		
	

	
  		
		
		
			
				
		x->numPoly = numPoly;
 				
 			
		
	

	
  		
		
		
			
		x->phaseDelay.length(phaseDelay.size());
		for (unsigned int i = 0; i < phaseDelay.size(); ++i) {
			
				
			x->phaseDelay[i] = phaseDelay.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		
			
		x->phaseDelayRate.length(phaseDelayRate.size());
		for (unsigned int i = 0; i < phaseDelayRate.size(); ++i) {
			
				
			x->phaseDelayRate[i] = phaseDelayRate.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		
			
		x->groupDelay.length(groupDelay.size());
		for (unsigned int i = 0; i < groupDelay.size(); ++i) {
			
				
			x->groupDelay[i] = groupDelay.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		
			
		x->groupDelayRate.length(groupDelayRate.size());
		for (unsigned int i = 0; i < groupDelayRate.size(); ++i) {
			
				
			x->groupDelayRate[i] = groupDelayRate.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		x->timeOriginExists = timeOriginExists;
		
		
			
		x->timeOrigin = timeOrigin.toIDLArrayTime();
			
		
	

	
  		
		
		x->atmosphericGroupDelayExists = atmosphericGroupDelayExists;
		
		
			
				
		x->atmosphericGroupDelay = atmosphericGroupDelay;
 				
 			
		
	

	
  		
		
		x->atmosphericGroupDelayRateExists = atmosphericGroupDelayRateExists;
		
		
			
				
		x->atmosphericGroupDelayRate = atmosphericGroupDelayRate;
 				
 			
		
	

	
  		
		
		x->geometricDelayExists = geometricDelayExists;
		
		
			
				
		x->geometricDelay = geometricDelay;
 				
 			
		
	

	
  		
		
		x->geometricDelayRateExists = geometricDelayRateExists;
		
		
			
				
		x->geometricDelayRate = geometricDelayRate;
 				
 			
		
	

	
  		
		
		x->numLOExists = numLOExists;
		
		
			
				
		x->numLO = numLO;
 				
 			
		
	

	
  		
		
		x->LOOffsetExists = LOOffsetExists;
		
		
			
		x->LOOffset.length(LOOffset.size());
		for (unsigned int i = 0; i < LOOffset.size(); ++i) {
			
			x->LOOffset[i] = LOOffset.at(i).toIDLFrequency();
			
	 	}
			
		
	

	
  		
		
		x->LOOffsetRateExists = LOOffsetRateExists;
		
		
			
		x->LOOffsetRate.length(LOOffsetRate.size());
		for (unsigned int i = 0; i < LOOffsetRate.size(); ++i) {
			
			x->LOOffsetRate[i] = LOOffsetRate.at(i).toIDLFrequency();
			
	 	}
			
		
	

	
  		
		
		x->dispersiveDelayExists = dispersiveDelayExists;
		
		
			
				
		x->dispersiveDelay = dispersiveDelay;
 				
 			
		
	

	
  		
		
		x->dispersiveDelayRateExists = dispersiveDelayRateExists;
		
		
			
				
		x->dispersiveDelayRate = dispersiveDelayRate;
 				
 			
		
	

	
  		
		
		x->atmosphericDryDelayExists = atmosphericDryDelayExists;
		
		
			
				
		x->atmosphericDryDelay = atmosphericDryDelay;
 				
 			
		
	

	
  		
		
		x->atmosphericWetDelayExists = atmosphericWetDelayExists;
		
		
			
				
		x->atmosphericWetDelay = atmosphericWetDelay;
 				
 			
		
	

	
  		
		
		x->padDelayExists = padDelayExists;
		
		
			
				
		x->padDelay = padDelay;
 				
 			
		
	

	
  		
		
		x->antennaDelayExists = antennaDelayExists;
		
		
			
				
		x->antennaDelay = antennaDelay;
 				
 			
		
	

	
  		
		
		x->numReceptorExists = numReceptorExists;
		
		
			
				
		x->numReceptor = numReceptor;
 				
 			
		
	

	
  		
		
		x->polarizationTypeExists = polarizationTypeExists;
		
		
			
		x->polarizationType.length(polarizationType.size());
		for (unsigned int i = 0; i < polarizationType.size(); ++i) {
			
				
			x->polarizationType[i] = polarizationType.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		x->electronicDelayExists = electronicDelayExists;
		
		
			
		x->electronicDelay.length(electronicDelay.size());
		for (unsigned int i = 0; i < electronicDelay.size(); ++i) {
			
				
			x->electronicDelay[i] = electronicDelay.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		x->electronicDelayRateExists = electronicDelayRateExists;
		
		
			
		x->electronicDelayRate.length(electronicDelayRate.size());
		for (unsigned int i = 0; i < electronicDelayRate.size(); ++i) {
			
				
			x->electronicDelayRate[i] = electronicDelayRate.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		x->receiverDelayExists = receiverDelayExists;
		
		
			
		x->receiverDelay.length(receiverDelay.size());
		for (unsigned int i = 0; i < receiverDelay.size(); ++i) {
			
				
			x->receiverDelay[i] = receiverDelay.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		x->IFDelayExists = IFDelayExists;
		
		
			
		x->IFDelay.length(IFDelay.size());
		for (unsigned int i = 0; i < IFDelay.size(); ++i) {
			
				
			x->IFDelay[i] = IFDelay.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		x->LODelayExists = LODelayExists;
		
		
			
		x->LODelay.length(LODelay.size());
		for (unsigned int i = 0; i < LODelay.size(); ++i) {
			
				
			x->LODelay[i] = LODelay.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		x->crossPolarizationDelayExists = crossPolarizationDelayExists;
		
		
			
				
		x->crossPolarizationDelay = crossPolarizationDelay;
 				
 			
		
	

	
	
		
	
  	
 		
		
	 	
			
		x->antennaId = antennaId.toIDLTag();
			
	 	 		
  	

	
  	
 		
		
	 	
			
		x->fieldId = fieldId.toIDLTag();
			
	 	 		
  	

	
  	
 		
		
	 	
			
		x->spectralWindowId = spectralWindowId.toIDLTag();
			
	 	 		
  	

	
		
	

	

	

		
		return x;
	
	}
	
	void DelayModelRow::toIDL(asdmIDL::DelayModelRowIDL& x) const {
		// Set the x's fields.
	
		
	
  		
		
		
			
		x.timeInterval = timeInterval.toIDLArrayTimeInterval();
			
		
	

	
  		
		
		
			
				
		x.numPoly = numPoly;
 				
 			
		
	

	
  		
		
		
			
		x.phaseDelay.length(phaseDelay.size());
		for (unsigned int i = 0; i < phaseDelay.size(); ++i) {
			
				
			x.phaseDelay[i] = phaseDelay.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		
			
		x.phaseDelayRate.length(phaseDelayRate.size());
		for (unsigned int i = 0; i < phaseDelayRate.size(); ++i) {
			
				
			x.phaseDelayRate[i] = phaseDelayRate.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		
			
		x.groupDelay.length(groupDelay.size());
		for (unsigned int i = 0; i < groupDelay.size(); ++i) {
			
				
			x.groupDelay[i] = groupDelay.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		
			
		x.groupDelayRate.length(groupDelayRate.size());
		for (unsigned int i = 0; i < groupDelayRate.size(); ++i) {
			
				
			x.groupDelayRate[i] = groupDelayRate.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		x.timeOriginExists = timeOriginExists;
		
		
			
		x.timeOrigin = timeOrigin.toIDLArrayTime();
			
		
	

	
  		
		
		x.atmosphericGroupDelayExists = atmosphericGroupDelayExists;
		
		
			
				
		x.atmosphericGroupDelay = atmosphericGroupDelay;
 				
 			
		
	

	
  		
		
		x.atmosphericGroupDelayRateExists = atmosphericGroupDelayRateExists;
		
		
			
				
		x.atmosphericGroupDelayRate = atmosphericGroupDelayRate;
 				
 			
		
	

	
  		
		
		x.geometricDelayExists = geometricDelayExists;
		
		
			
				
		x.geometricDelay = geometricDelay;
 				
 			
		
	

	
  		
		
		x.geometricDelayRateExists = geometricDelayRateExists;
		
		
			
				
		x.geometricDelayRate = geometricDelayRate;
 				
 			
		
	

	
  		
		
		x.numLOExists = numLOExists;
		
		
			
				
		x.numLO = numLO;
 				
 			
		
	

	
  		
		
		x.LOOffsetExists = LOOffsetExists;
		
		
			
		x.LOOffset.length(LOOffset.size());
		for (unsigned int i = 0; i < LOOffset.size(); ++i) {
			
			x.LOOffset[i] = LOOffset.at(i).toIDLFrequency();
			
	 	}
			
		
	

	
  		
		
		x.LOOffsetRateExists = LOOffsetRateExists;
		
		
			
		x.LOOffsetRate.length(LOOffsetRate.size());
		for (unsigned int i = 0; i < LOOffsetRate.size(); ++i) {
			
			x.LOOffsetRate[i] = LOOffsetRate.at(i).toIDLFrequency();
			
	 	}
			
		
	

	
  		
		
		x.dispersiveDelayExists = dispersiveDelayExists;
		
		
			
				
		x.dispersiveDelay = dispersiveDelay;
 				
 			
		
	

	
  		
		
		x.dispersiveDelayRateExists = dispersiveDelayRateExists;
		
		
			
				
		x.dispersiveDelayRate = dispersiveDelayRate;
 				
 			
		
	

	
  		
		
		x.atmosphericDryDelayExists = atmosphericDryDelayExists;
		
		
			
				
		x.atmosphericDryDelay = atmosphericDryDelay;
 				
 			
		
	

	
  		
		
		x.atmosphericWetDelayExists = atmosphericWetDelayExists;
		
		
			
				
		x.atmosphericWetDelay = atmosphericWetDelay;
 				
 			
		
	

	
  		
		
		x.padDelayExists = padDelayExists;
		
		
			
				
		x.padDelay = padDelay;
 				
 			
		
	

	
  		
		
		x.antennaDelayExists = antennaDelayExists;
		
		
			
				
		x.antennaDelay = antennaDelay;
 				
 			
		
	

	
  		
		
		x.numReceptorExists = numReceptorExists;
		
		
			
				
		x.numReceptor = numReceptor;
 				
 			
		
	

	
  		
		
		x.polarizationTypeExists = polarizationTypeExists;
		
		
			
		x.polarizationType.length(polarizationType.size());
		for (unsigned int i = 0; i < polarizationType.size(); ++i) {
			
				
			x.polarizationType[i] = polarizationType.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		x.electronicDelayExists = electronicDelayExists;
		
		
			
		x.electronicDelay.length(electronicDelay.size());
		for (unsigned int i = 0; i < electronicDelay.size(); ++i) {
			
				
			x.electronicDelay[i] = electronicDelay.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		x.electronicDelayRateExists = electronicDelayRateExists;
		
		
			
		x.electronicDelayRate.length(electronicDelayRate.size());
		for (unsigned int i = 0; i < electronicDelayRate.size(); ++i) {
			
				
			x.electronicDelayRate[i] = electronicDelayRate.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		x.receiverDelayExists = receiverDelayExists;
		
		
			
		x.receiverDelay.length(receiverDelay.size());
		for (unsigned int i = 0; i < receiverDelay.size(); ++i) {
			
				
			x.receiverDelay[i] = receiverDelay.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		x.IFDelayExists = IFDelayExists;
		
		
			
		x.IFDelay.length(IFDelay.size());
		for (unsigned int i = 0; i < IFDelay.size(); ++i) {
			
				
			x.IFDelay[i] = IFDelay.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		x.LODelayExists = LODelayExists;
		
		
			
		x.LODelay.length(LODelay.size());
		for (unsigned int i = 0; i < LODelay.size(); ++i) {
			
				
			x.LODelay[i] = LODelay.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		x.crossPolarizationDelayExists = crossPolarizationDelayExists;
		
		
			
				
		x.crossPolarizationDelay = crossPolarizationDelay;
 				
 			
		
	

	
	
		
	
  	
 		
		
	 	
			
		x.antennaId = antennaId.toIDLTag();
			
	 	 		
  	

	
  	
 		
		
	 	
			
		x.fieldId = fieldId.toIDLTag();
			
	 	 		
  	

	
  	
 		
		
	 	
			
		x.spectralWindowId = spectralWindowId.toIDLTag();
			
	 	 		
  	

	
		
	

	

	

	
	}
#endif
	

#ifndef WITHOUT_ACS
	/**
	 * Fill the values of this row from the IDL struct DelayModelRowIDL.
	 * @param x The IDL struct containing the values used to fill this row.
	 */
	void DelayModelRow::setFromIDL (DelayModelRowIDL x){
		try {
		// Fill the values from x.
	
		
	
		
		
			
		setTimeInterval(ArrayTimeInterval (x.timeInterval));
			
 		
		
	

	
		
		
			
		setNumPoly(x.numPoly);
  			
 		
		
	

	
		
		
			
		phaseDelay .clear();
		for (unsigned int i = 0; i <x.phaseDelay.length(); ++i) {
			
			phaseDelay.push_back(x.phaseDelay[i]);
  			
		}
			
  		
		
	

	
		
		
			
		phaseDelayRate .clear();
		for (unsigned int i = 0; i <x.phaseDelayRate.length(); ++i) {
			
			phaseDelayRate.push_back(x.phaseDelayRate[i]);
  			
		}
			
  		
		
	

	
		
		
			
		groupDelay .clear();
		for (unsigned int i = 0; i <x.groupDelay.length(); ++i) {
			
			groupDelay.push_back(x.groupDelay[i]);
  			
		}
			
  		
		
	

	
		
		
			
		groupDelayRate .clear();
		for (unsigned int i = 0; i <x.groupDelayRate.length(); ++i) {
			
			groupDelayRate.push_back(x.groupDelayRate[i]);
  			
		}
			
  		
		
	

	
		
		timeOriginExists = x.timeOriginExists;
		if (x.timeOriginExists) {
		
		
			
		setTimeOrigin(ArrayTime (x.timeOrigin));
			
 		
		
		}
		
	

	
		
		atmosphericGroupDelayExists = x.atmosphericGroupDelayExists;
		if (x.atmosphericGroupDelayExists) {
		
		
			
		setAtmosphericGroupDelay(x.atmosphericGroupDelay);
  			
 		
		
		}
		
	

	
		
		atmosphericGroupDelayRateExists = x.atmosphericGroupDelayRateExists;
		if (x.atmosphericGroupDelayRateExists) {
		
		
			
		setAtmosphericGroupDelayRate(x.atmosphericGroupDelayRate);
  			
 		
		
		}
		
	

	
		
		geometricDelayExists = x.geometricDelayExists;
		if (x.geometricDelayExists) {
		
		
			
		setGeometricDelay(x.geometricDelay);
  			
 		
		
		}
		
	

	
		
		geometricDelayRateExists = x.geometricDelayRateExists;
		if (x.geometricDelayRateExists) {
		
		
			
		setGeometricDelayRate(x.geometricDelayRate);
  			
 		
		
		}
		
	

	
		
		numLOExists = x.numLOExists;
		if (x.numLOExists) {
		
		
			
		setNumLO(x.numLO);
  			
 		
		
		}
		
	

	
		
		LOOffsetExists = x.LOOffsetExists;
		if (x.LOOffsetExists) {
		
		
			
		LOOffset .clear();
		for (unsigned int i = 0; i <x.LOOffset.length(); ++i) {
			
			LOOffset.push_back(Frequency (x.LOOffset[i]));
			
		}
			
  		
		
		}
		
	

	
		
		LOOffsetRateExists = x.LOOffsetRateExists;
		if (x.LOOffsetRateExists) {
		
		
			
		LOOffsetRate .clear();
		for (unsigned int i = 0; i <x.LOOffsetRate.length(); ++i) {
			
			LOOffsetRate.push_back(Frequency (x.LOOffsetRate[i]));
			
		}
			
  		
		
		}
		
	

	
		
		dispersiveDelayExists = x.dispersiveDelayExists;
		if (x.dispersiveDelayExists) {
		
		
			
		setDispersiveDelay(x.dispersiveDelay);
  			
 		
		
		}
		
	

	
		
		dispersiveDelayRateExists = x.dispersiveDelayRateExists;
		if (x.dispersiveDelayRateExists) {
		
		
			
		setDispersiveDelayRate(x.dispersiveDelayRate);
  			
 		
		
		}
		
	

	
		
		atmosphericDryDelayExists = x.atmosphericDryDelayExists;
		if (x.atmosphericDryDelayExists) {
		
		
			
		setAtmosphericDryDelay(x.atmosphericDryDelay);
  			
 		
		
		}
		
	

	
		
		atmosphericWetDelayExists = x.atmosphericWetDelayExists;
		if (x.atmosphericWetDelayExists) {
		
		
			
		setAtmosphericWetDelay(x.atmosphericWetDelay);
  			
 		
		
		}
		
	

	
		
		padDelayExists = x.padDelayExists;
		if (x.padDelayExists) {
		
		
			
		setPadDelay(x.padDelay);
  			
 		
		
		}
		
	

	
		
		antennaDelayExists = x.antennaDelayExists;
		if (x.antennaDelayExists) {
		
		
			
		setAntennaDelay(x.antennaDelay);
  			
 		
		
		}
		
	

	
		
		numReceptorExists = x.numReceptorExists;
		if (x.numReceptorExists) {
		
		
			
		setNumReceptor(x.numReceptor);
  			
 		
		
		}
		
	

	
		
		polarizationTypeExists = x.polarizationTypeExists;
		if (x.polarizationTypeExists) {
		
		
			
		polarizationType .clear();
		for (unsigned int i = 0; i <x.polarizationType.length(); ++i) {
			
			polarizationType.push_back(x.polarizationType[i]);
  			
		}
			
  		
		
		}
		
	

	
		
		electronicDelayExists = x.electronicDelayExists;
		if (x.electronicDelayExists) {
		
		
			
		electronicDelay .clear();
		for (unsigned int i = 0; i <x.electronicDelay.length(); ++i) {
			
			electronicDelay.push_back(x.electronicDelay[i]);
  			
		}
			
  		
		
		}
		
	

	
		
		electronicDelayRateExists = x.electronicDelayRateExists;
		if (x.electronicDelayRateExists) {
		
		
			
		electronicDelayRate .clear();
		for (unsigned int i = 0; i <x.electronicDelayRate.length(); ++i) {
			
			electronicDelayRate.push_back(x.electronicDelayRate[i]);
  			
		}
			
  		
		
		}
		
	

	
		
		receiverDelayExists = x.receiverDelayExists;
		if (x.receiverDelayExists) {
		
		
			
		receiverDelay .clear();
		for (unsigned int i = 0; i <x.receiverDelay.length(); ++i) {
			
			receiverDelay.push_back(x.receiverDelay[i]);
  			
		}
			
  		
		
		}
		
	

	
		
		IFDelayExists = x.IFDelayExists;
		if (x.IFDelayExists) {
		
		
			
		IFDelay .clear();
		for (unsigned int i = 0; i <x.IFDelay.length(); ++i) {
			
			IFDelay.push_back(x.IFDelay[i]);
  			
		}
			
  		
		
		}
		
	

	
		
		LODelayExists = x.LODelayExists;
		if (x.LODelayExists) {
		
		
			
		LODelay .clear();
		for (unsigned int i = 0; i <x.LODelay.length(); ++i) {
			
			LODelay.push_back(x.LODelay[i]);
  			
		}
			
  		
		
		}
		
	

	
		
		crossPolarizationDelayExists = x.crossPolarizationDelayExists;
		if (x.crossPolarizationDelayExists) {
		
		
			
		setCrossPolarizationDelay(x.crossPolarizationDelay);
  			
 		
		
		}
		
	

	
	
		
	
		
		
			
		setAntennaId(Tag (x.antennaId));
			
 		
		
	

	
		
		
			
		setFieldId(Tag (x.fieldId));
			
 		
		
	

	
		
		
			
		setSpectralWindowId(Tag (x.spectralWindowId));
			
 		
		
	

	
		
	

	

	

		} catch (const IllegalAccessException &err) {
			throw ConversionException (err.getMessage(),"DelayModel");
		}
	}
#endif
	
	/**
	 * Return this row in the form of an XML string.
	 * @return The values of this row as an XML string.
	 */
	string DelayModelRow::toXML() const {
		string buf;
		buf.append("<row> \n");
		
	
		
  	
 		
		
		Parser::toXML(timeInterval, "timeInterval", buf);
		
		
	

  	
 		
		
		Parser::toXML(numPoly, "numPoly", buf);
		
		
	

  	
 		
		
		Parser::toXML(phaseDelay, "phaseDelay", buf);
		
		
	

  	
 		
		
		Parser::toXML(phaseDelayRate, "phaseDelayRate", buf);
		
		
	

  	
 		
		
		Parser::toXML(groupDelay, "groupDelay", buf);
		
		
	

  	
 		
		
		Parser::toXML(groupDelayRate, "groupDelayRate", buf);
		
		
	

  	
 		
		if (timeOriginExists) {
		
		
		Parser::toXML(timeOrigin, "timeOrigin", buf);
		
		
		}
		
	

  	
 		
		if (atmosphericGroupDelayExists) {
		
		
		Parser::toXML(atmosphericGroupDelay, "atmosphericGroupDelay", buf);
		
		
		}
		
	

  	
 		
		if (atmosphericGroupDelayRateExists) {
		
		
		Parser::toXML(atmosphericGroupDelayRate, "atmosphericGroupDelayRate", buf);
		
		
		}
		
	

  	
 		
		if (geometricDelayExists) {
		
		
		Parser::toXML(geometricDelay, "geometricDelay", buf);
		
		
		}
		
	

  	
 		
		if (geometricDelayRateExists) {
		
		
		Parser::toXML(geometricDelayRate, "geometricDelayRate", buf);
		
		
		}
		
	

  	
 		
		if (numLOExists) {
		
		
		Parser::toXML(numLO, "numLO", buf);
		
		
		}
		
	

  	
 		
		if (LOOffsetExists) {
		
		
		Parser::toXML(LOOffset, "LOOffset", buf);
		
		
		}
		
	

  	
 		
		if (LOOffsetRateExists) {
		
		
		Parser::toXML(LOOffsetRate, "LOOffsetRate", buf);
		
		
		}
		
	

  	
 		
		if (dispersiveDelayExists) {
		
		
		Parser::toXML(dispersiveDelay, "dispersiveDelay", buf);
		
		
		}
		
	

  	
 		
		if (dispersiveDelayRateExists) {
		
		
		Parser::toXML(dispersiveDelayRate, "dispersiveDelayRate", buf);
		
		
		}
		
	

  	
 		
		if (atmosphericDryDelayExists) {
		
		
		Parser::toXML(atmosphericDryDelay, "atmosphericDryDelay", buf);
		
		
		}
		
	

  	
 		
		if (atmosphericWetDelayExists) {
		
		
		Parser::toXML(atmosphericWetDelay, "atmosphericWetDelay", buf);
		
		
		}
		
	

  	
 		
		if (padDelayExists) {
		
		
		Parser::toXML(padDelay, "padDelay", buf);
		
		
		}
		
	

  	
 		
		if (antennaDelayExists) {
		
		
		Parser::toXML(antennaDelay, "antennaDelay", buf);
		
		
		}
		
	

  	
 		
		if (numReceptorExists) {
		
		
		Parser::toXML(numReceptor, "numReceptor", buf);
		
		
		}
		
	

  	
 		
		if (polarizationTypeExists) {
		
		
			buf.append(EnumerationParser::toXML("polarizationType", polarizationType));
		
		
		}
		
	

  	
 		
		if (electronicDelayExists) {
		
		
		Parser::toXML(electronicDelay, "electronicDelay", buf);
		
		
		}
		
	

  	
 		
		if (electronicDelayRateExists) {
		
		
		Parser::toXML(electronicDelayRate, "electronicDelayRate", buf);
		
		
		}
		
	

  	
 		
		if (receiverDelayExists) {
		
		
		Parser::toXML(receiverDelay, "receiverDelay", buf);
		
		
		}
		
	

  	
 		
		if (IFDelayExists) {
		
		
		Parser::toXML(IFDelay, "IFDelay", buf);
		
		
		}
		
	

  	
 		
		if (LODelayExists) {
		
		
		Parser::toXML(LODelay, "LODelay", buf);
		
		
		}
		
	

  	
 		
		if (crossPolarizationDelayExists) {
		
		
		Parser::toXML(crossPolarizationDelay, "crossPolarizationDelay", buf);
		
		
		}
		
	

	
	
		
  	
 		
		
		Parser::toXML(antennaId, "antennaId", buf);
		
		
	

  	
 		
		
		Parser::toXML(fieldId, "fieldId", buf);
		
		
	

  	
 		
		
		Parser::toXML(spectralWindowId, "spectralWindowId", buf);
		
		
	

	
		
	

	

	

		
		buf.append("</row>\n");
		return buf;
	}

	/**
	 * Fill the values of this row from an XML string 
	 * that was produced by the toXML() method.
	 * @param x The XML string being used to set the values of this row.
	 */
	void DelayModelRow::setFromXML (string rowDoc) {
		Parser row(rowDoc);
		string s = "";
		try {
	
		
	
  		
			
	  	setTimeInterval(Parser::getArrayTimeInterval("timeInterval","DelayModel",rowDoc));
			
		
	

	
  		
			
	  	setNumPoly(Parser::getInteger("numPoly","DelayModel",rowDoc));
			
		
	

	
  		
			
					
	  	setPhaseDelay(Parser::get1DDouble("phaseDelay","DelayModel",rowDoc));
	  			
	  		
		
	

	
  		
			
					
	  	setPhaseDelayRate(Parser::get1DDouble("phaseDelayRate","DelayModel",rowDoc));
	  			
	  		
		
	

	
  		
			
					
	  	setGroupDelay(Parser::get1DDouble("groupDelay","DelayModel",rowDoc));
	  			
	  		
		
	

	
  		
			
					
	  	setGroupDelayRate(Parser::get1DDouble("groupDelayRate","DelayModel",rowDoc));
	  			
	  		
		
	

	
  		
        if (row.isStr("<timeOrigin>")) {
			
	  		setTimeOrigin(Parser::getArrayTime("timeOrigin","DelayModel",rowDoc));
			
		}
 		
	

	
  		
        if (row.isStr("<atmosphericGroupDelay>")) {
			
	  		setAtmosphericGroupDelay(Parser::getDouble("atmosphericGroupDelay","DelayModel",rowDoc));
			
		}
 		
	

	
  		
        if (row.isStr("<atmosphericGroupDelayRate>")) {
			
	  		setAtmosphericGroupDelayRate(Parser::getDouble("atmosphericGroupDelayRate","DelayModel",rowDoc));
			
		}
 		
	

	
  		
        if (row.isStr("<geometricDelay>")) {
			
	  		setGeometricDelay(Parser::getDouble("geometricDelay","DelayModel",rowDoc));
			
		}
 		
	

	
  		
        if (row.isStr("<geometricDelayRate>")) {
			
	  		setGeometricDelayRate(Parser::getDouble("geometricDelayRate","DelayModel",rowDoc));
			
		}
 		
	

	
  		
        if (row.isStr("<numLO>")) {
			
	  		setNumLO(Parser::getInteger("numLO","DelayModel",rowDoc));
			
		}
 		
	

	
  		
        if (row.isStr("<LOOffset>")) {
			
								
	  		setLOOffset(Parser::get1DFrequency("LOOffset","DelayModel",rowDoc));
	  			
	  		
		}
 		
	

	
  		
        if (row.isStr("<LOOffsetRate>")) {
			
								
	  		setLOOffsetRate(Parser::get1DFrequency("LOOffsetRate","DelayModel",rowDoc));
	  			
	  		
		}
 		
	

	
  		
        if (row.isStr("<dispersiveDelay>")) {
			
	  		setDispersiveDelay(Parser::getDouble("dispersiveDelay","DelayModel",rowDoc));
			
		}
 		
	

	
  		
        if (row.isStr("<dispersiveDelayRate>")) {
			
	  		setDispersiveDelayRate(Parser::getDouble("dispersiveDelayRate","DelayModel",rowDoc));
			
		}
 		
	

	
  		
        if (row.isStr("<atmosphericDryDelay>")) {
			
	  		setAtmosphericDryDelay(Parser::getDouble("atmosphericDryDelay","DelayModel",rowDoc));
			
		}
 		
	

	
  		
        if (row.isStr("<atmosphericWetDelay>")) {
			
	  		setAtmosphericWetDelay(Parser::getDouble("atmosphericWetDelay","DelayModel",rowDoc));
			
		}
 		
	

	
  		
        if (row.isStr("<padDelay>")) {
			
	  		setPadDelay(Parser::getDouble("padDelay","DelayModel",rowDoc));
			
		}
 		
	

	
  		
        if (row.isStr("<antennaDelay>")) {
			
	  		setAntennaDelay(Parser::getDouble("antennaDelay","DelayModel",rowDoc));
			
		}
 		
	

	
  		
        if (row.isStr("<numReceptor>")) {
			
	  		setNumReceptor(Parser::getInteger("numReceptor","DelayModel",rowDoc));
			
		}
 		
	

	
		
	if (row.isStr("<polarizationType>")) {
		
		
		
		polarizationType = EnumerationParser::getPolarizationType1D("polarizationType","DelayModel",rowDoc);			
		
		
		
		polarizationTypeExists = true;
	}
		
	

	
  		
        if (row.isStr("<electronicDelay>")) {
			
								
	  		setElectronicDelay(Parser::get1DDouble("electronicDelay","DelayModel",rowDoc));
	  			
	  		
		}
 		
	

	
  		
        if (row.isStr("<electronicDelayRate>")) {
			
								
	  		setElectronicDelayRate(Parser::get1DDouble("electronicDelayRate","DelayModel",rowDoc));
	  			
	  		
		}
 		
	

	
  		
        if (row.isStr("<receiverDelay>")) {
			
								
	  		setReceiverDelay(Parser::get1DDouble("receiverDelay","DelayModel",rowDoc));
	  			
	  		
		}
 		
	

	
  		
        if (row.isStr("<IFDelay>")) {
			
								
	  		setIFDelay(Parser::get1DDouble("IFDelay","DelayModel",rowDoc));
	  			
	  		
		}
 		
	

	
  		
        if (row.isStr("<LODelay>")) {
			
								
	  		setLODelay(Parser::get1DDouble("LODelay","DelayModel",rowDoc));
	  			
	  		
		}
 		
	

	
  		
        if (row.isStr("<crossPolarizationDelay>")) {
			
	  		setCrossPolarizationDelay(Parser::getDouble("crossPolarizationDelay","DelayModel",rowDoc));
			
		}
 		
	

	
	
		
	
  		
			
	  	setAntennaId(Parser::getTag("antennaId","Antenna",rowDoc));
			
		
	

	
  		
			
	  	setFieldId(Parser::getTag("fieldId","DelayModel",rowDoc));
			
		
	

	
  		
			
	  	setSpectralWindowId(Parser::getTag("spectralWindowId","SpectralWindow",rowDoc));
			
		
	

	
		
	

	

	

		} catch (const IllegalAccessException &err) {
			throw ConversionException (err.getMessage(),"DelayModel");
		}
	}
	
	void DelayModelRow::toBin(EndianOSStream& eoss) {
	
	
	
	
		
	antennaId.toBin(eoss);
		
	

	
	
		
	spectralWindowId.toBin(eoss);
		
	

	
	
		
	timeInterval.toBin(eoss);
		
	

	
	
		
						
			eoss.writeInt(numPoly);
				
		
	

	
	
		
		
			
		eoss.writeInt((int) phaseDelay.size());
		for (unsigned int i = 0; i < phaseDelay.size(); i++)
				
			eoss.writeDouble(phaseDelay.at(i));
				
				
						
		
	

	
	
		
		
			
		eoss.writeInt((int) phaseDelayRate.size());
		for (unsigned int i = 0; i < phaseDelayRate.size(); i++)
				
			eoss.writeDouble(phaseDelayRate.at(i));
				
				
						
		
	

	
	
		
		
			
		eoss.writeInt((int) groupDelay.size());
		for (unsigned int i = 0; i < groupDelay.size(); i++)
				
			eoss.writeDouble(groupDelay.at(i));
				
				
						
		
	

	
	
		
		
			
		eoss.writeInt((int) groupDelayRate.size());
		for (unsigned int i = 0; i < groupDelayRate.size(); i++)
				
			eoss.writeDouble(groupDelayRate.at(i));
				
				
						
		
	

	
	
		
	fieldId.toBin(eoss);
		
	


	
	
	eoss.writeBoolean(timeOriginExists);
	if (timeOriginExists) {
	
	
	
		
	timeOrigin.toBin(eoss);
		
	

	}

	eoss.writeBoolean(atmosphericGroupDelayExists);
	if (atmosphericGroupDelayExists) {
	
	
	
		
						
			eoss.writeDouble(atmosphericGroupDelay);
				
		
	

	}

	eoss.writeBoolean(atmosphericGroupDelayRateExists);
	if (atmosphericGroupDelayRateExists) {
	
	
	
		
						
			eoss.writeDouble(atmosphericGroupDelayRate);
				
		
	

	}

	eoss.writeBoolean(geometricDelayExists);
	if (geometricDelayExists) {
	
	
	
		
						
			eoss.writeDouble(geometricDelay);
				
		
	

	}

	eoss.writeBoolean(geometricDelayRateExists);
	if (geometricDelayRateExists) {
	
	
	
		
						
			eoss.writeDouble(geometricDelayRate);
				
		
	

	}

	eoss.writeBoolean(numLOExists);
	if (numLOExists) {
	
	
	
		
						
			eoss.writeInt(numLO);
				
		
	

	}

	eoss.writeBoolean(LOOffsetExists);
	if (LOOffsetExists) {
	
	
	
		
	Frequency::toBin(LOOffset, eoss);
		
	

	}

	eoss.writeBoolean(LOOffsetRateExists);
	if (LOOffsetRateExists) {
	
	
	
		
	Frequency::toBin(LOOffsetRate, eoss);
		
	

	}

	eoss.writeBoolean(dispersiveDelayExists);
	if (dispersiveDelayExists) {
	
	
	
		
						
			eoss.writeDouble(dispersiveDelay);
				
		
	

	}

	eoss.writeBoolean(dispersiveDelayRateExists);
	if (dispersiveDelayRateExists) {
	
	
	
		
						
			eoss.writeDouble(dispersiveDelayRate);
				
		
	

	}

	eoss.writeBoolean(atmosphericDryDelayExists);
	if (atmosphericDryDelayExists) {
	
	
	
		
						
			eoss.writeDouble(atmosphericDryDelay);
				
		
	

	}

	eoss.writeBoolean(atmosphericWetDelayExists);
	if (atmosphericWetDelayExists) {
	
	
	
		
						
			eoss.writeDouble(atmosphericWetDelay);
				
		
	

	}

	eoss.writeBoolean(padDelayExists);
	if (padDelayExists) {
	
	
	
		
						
			eoss.writeDouble(padDelay);
				
		
	

	}

	eoss.writeBoolean(antennaDelayExists);
	if (antennaDelayExists) {
	
	
	
		
						
			eoss.writeDouble(antennaDelay);
				
		
	

	}

	eoss.writeBoolean(numReceptorExists);
	if (numReceptorExists) {
	
	
	
		
						
			eoss.writeInt(numReceptor);
				
		
	

	}

	eoss.writeBoolean(polarizationTypeExists);
	if (polarizationTypeExists) {
	
	
	
		
		
			
		eoss.writeInt((int) polarizationType.size());
		for (unsigned int i = 0; i < polarizationType.size(); i++)
				
			eoss.writeString(CPolarizationType::name(polarizationType.at(i)));
			/* eoss.writeInt(polarizationType.at(i)); */
				
				
						
		
	

	}

	eoss.writeBoolean(electronicDelayExists);
	if (electronicDelayExists) {
	
	
	
		
		
			
		eoss.writeInt((int) electronicDelay.size());
		for (unsigned int i = 0; i < electronicDelay.size(); i++)
				
			eoss.writeDouble(electronicDelay.at(i));
				
				
						
		
	

	}

	eoss.writeBoolean(electronicDelayRateExists);
	if (electronicDelayRateExists) {
	
	
	
		
		
			
		eoss.writeInt((int) electronicDelayRate.size());
		for (unsigned int i = 0; i < electronicDelayRate.size(); i++)
				
			eoss.writeDouble(electronicDelayRate.at(i));
				
				
						
		
	

	}

	eoss.writeBoolean(receiverDelayExists);
	if (receiverDelayExists) {
	
	
	
		
		
			
		eoss.writeInt((int) receiverDelay.size());
		for (unsigned int i = 0; i < receiverDelay.size(); i++)
				
			eoss.writeDouble(receiverDelay.at(i));
				
				
						
		
	

	}

	eoss.writeBoolean(IFDelayExists);
	if (IFDelayExists) {
	
	
	
		
		
			
		eoss.writeInt((int) IFDelay.size());
		for (unsigned int i = 0; i < IFDelay.size(); i++)
				
			eoss.writeDouble(IFDelay.at(i));
				
				
						
		
	

	}

	eoss.writeBoolean(LODelayExists);
	if (LODelayExists) {
	
	
	
		
		
			
		eoss.writeInt((int) LODelay.size());
		for (unsigned int i = 0; i < LODelay.size(); i++)
				
			eoss.writeDouble(LODelay.at(i));
				
				
						
		
	

	}

	eoss.writeBoolean(crossPolarizationDelayExists);
	if (crossPolarizationDelayExists) {
	
	
	
		
						
			eoss.writeDouble(crossPolarizationDelay);
				
		
	

	}

	}
	
void DelayModelRow::antennaIdFromBin(EndianIStream& eis) {
		
	
		
		
		antennaId =  Tag::fromBin(eis);
		
	
	
}
void DelayModelRow::spectralWindowIdFromBin(EndianIStream& eis) {
		
	
		
		
		spectralWindowId =  Tag::fromBin(eis);
		
	
	
}
void DelayModelRow::timeIntervalFromBin(EndianIStream& eis) {
		
	
		
		
		timeInterval =  ArrayTimeInterval::fromBin(eis);
		
	
	
}
void DelayModelRow::numPolyFromBin(EndianIStream& eis) {
		
	
	
		
			
		numPoly =  eis.readInt();
			
		
	
	
}
void DelayModelRow::phaseDelayFromBin(EndianIStream& eis) {
		
	
	
		
			
	
		phaseDelay.clear();
		
		unsigned int phaseDelayDim1 = eis.readInt();
		for (unsigned int  i = 0 ; i < phaseDelayDim1; i++)
			
			phaseDelay.push_back(eis.readDouble());
			
	

		
	
	
}
void DelayModelRow::phaseDelayRateFromBin(EndianIStream& eis) {
		
	
	
		
			
	
		phaseDelayRate.clear();
		
		unsigned int phaseDelayRateDim1 = eis.readInt();
		for (unsigned int  i = 0 ; i < phaseDelayRateDim1; i++)
			
			phaseDelayRate.push_back(eis.readDouble());
			
	

		
	
	
}
void DelayModelRow::groupDelayFromBin(EndianIStream& eis) {
		
	
	
		
			
	
		groupDelay.clear();
		
		unsigned int groupDelayDim1 = eis.readInt();
		for (unsigned int  i = 0 ; i < groupDelayDim1; i++)
			
			groupDelay.push_back(eis.readDouble());
			
	

		
	
	
}
void DelayModelRow::groupDelayRateFromBin(EndianIStream& eis) {
		
	
	
		
			
	
		groupDelayRate.clear();
		
		unsigned int groupDelayRateDim1 = eis.readInt();
		for (unsigned int  i = 0 ; i < groupDelayRateDim1; i++)
			
			groupDelayRate.push_back(eis.readDouble());
			
	

		
	
	
}
void DelayModelRow::fieldIdFromBin(EndianIStream& eis) {
		
	
		
		
		fieldId =  Tag::fromBin(eis);
		
	
	
}

void DelayModelRow::timeOriginFromBin(EndianIStream& eis) {
		
	timeOriginExists = eis.readBoolean();
	if (timeOriginExists) {
		
	
		
		
		timeOrigin =  ArrayTime::fromBin(eis);
		
	

	}
	
}
void DelayModelRow::atmosphericGroupDelayFromBin(EndianIStream& eis) {
		
	atmosphericGroupDelayExists = eis.readBoolean();
	if (atmosphericGroupDelayExists) {
		
	
	
		
			
		atmosphericGroupDelay =  eis.readDouble();
			
		
	

	}
	
}
void DelayModelRow::atmosphericGroupDelayRateFromBin(EndianIStream& eis) {
		
	atmosphericGroupDelayRateExists = eis.readBoolean();
	if (atmosphericGroupDelayRateExists) {
		
	
	
		
			
		atmosphericGroupDelayRate =  eis.readDouble();
			
		
	

	}
	
}
void DelayModelRow::geometricDelayFromBin(EndianIStream& eis) {
		
	geometricDelayExists = eis.readBoolean();
	if (geometricDelayExists) {
		
	
	
		
			
		geometricDelay =  eis.readDouble();
			
		
	

	}
	
}
void DelayModelRow::geometricDelayRateFromBin(EndianIStream& eis) {
		
	geometricDelayRateExists = eis.readBoolean();
	if (geometricDelayRateExists) {
		
	
	
		
			
		geometricDelayRate =  eis.readDouble();
			
		
	

	}
	
}
void DelayModelRow::numLOFromBin(EndianIStream& eis) {
		
	numLOExists = eis.readBoolean();
	if (numLOExists) {
		
	
	
		
			
		numLO =  eis.readInt();
			
		
	

	}
	
}
void DelayModelRow::LOOffsetFromBin(EndianIStream& eis) {
		
	LOOffsetExists = eis.readBoolean();
	if (LOOffsetExists) {
		
	
		
		
			
	
	LOOffset = Frequency::from1DBin(eis);	
	

		
	

	}
	
}
void DelayModelRow::LOOffsetRateFromBin(EndianIStream& eis) {
		
	LOOffsetRateExists = eis.readBoolean();
	if (LOOffsetRateExists) {
		
	
		
		
			
	
	LOOffsetRate = Frequency::from1DBin(eis);	
	

		
	

	}
	
}
void DelayModelRow::dispersiveDelayFromBin(EndianIStream& eis) {
		
	dispersiveDelayExists = eis.readBoolean();
	if (dispersiveDelayExists) {
		
	
	
		
			
		dispersiveDelay =  eis.readDouble();
			
		
	

	}
	
}
void DelayModelRow::dispersiveDelayRateFromBin(EndianIStream& eis) {
		
	dispersiveDelayRateExists = eis.readBoolean();
	if (dispersiveDelayRateExists) {
		
	
	
		
			
		dispersiveDelayRate =  eis.readDouble();
			
		
	

	}
	
}
void DelayModelRow::atmosphericDryDelayFromBin(EndianIStream& eis) {
		
	atmosphericDryDelayExists = eis.readBoolean();
	if (atmosphericDryDelayExists) {
		
	
	
		
			
		atmosphericDryDelay =  eis.readDouble();
			
		
	

	}
	
}
void DelayModelRow::atmosphericWetDelayFromBin(EndianIStream& eis) {
		
	atmosphericWetDelayExists = eis.readBoolean();
	if (atmosphericWetDelayExists) {
		
	
	
		
			
		atmosphericWetDelay =  eis.readDouble();
			
		
	

	}
	
}
void DelayModelRow::padDelayFromBin(EndianIStream& eis) {
		
	padDelayExists = eis.readBoolean();
	if (padDelayExists) {
		
	
	
		
			
		padDelay =  eis.readDouble();
			
		
	

	}
	
}
void DelayModelRow::antennaDelayFromBin(EndianIStream& eis) {
		
	antennaDelayExists = eis.readBoolean();
	if (antennaDelayExists) {
		
	
	
		
			
		antennaDelay =  eis.readDouble();
			
		
	

	}
	
}
void DelayModelRow::numReceptorFromBin(EndianIStream& eis) {
		
	numReceptorExists = eis.readBoolean();
	if (numReceptorExists) {
		
	
	
		
			
		numReceptor =  eis.readInt();
			
		
	

	}
	
}
void DelayModelRow::polarizationTypeFromBin(EndianIStream& eis) {
		
	polarizationTypeExists = eis.readBoolean();
	if (polarizationTypeExists) {
		
	
	
		
			
	
		polarizationType.clear();
		
		unsigned int polarizationTypeDim1 = eis.readInt();
		for (unsigned int  i = 0 ; i < polarizationTypeDim1; i++)
			
			polarizationType.push_back(CPolarizationType::literal(eis.readString()));
			
	

		
	

	}
	
}
void DelayModelRow::electronicDelayFromBin(EndianIStream& eis) {
		
	electronicDelayExists = eis.readBoolean();
	if (electronicDelayExists) {
		
	
	
		
			
	
		electronicDelay.clear();
		
		unsigned int electronicDelayDim1 = eis.readInt();
		for (unsigned int  i = 0 ; i < electronicDelayDim1; i++)
			
			electronicDelay.push_back(eis.readDouble());
			
	

		
	

	}
	
}
void DelayModelRow::electronicDelayRateFromBin(EndianIStream& eis) {
		
	electronicDelayRateExists = eis.readBoolean();
	if (electronicDelayRateExists) {
		
	
	
		
			
	
		electronicDelayRate.clear();
		
		unsigned int electronicDelayRateDim1 = eis.readInt();
		for (unsigned int  i = 0 ; i < electronicDelayRateDim1; i++)
			
			electronicDelayRate.push_back(eis.readDouble());
			
	

		
	

	}
	
}
void DelayModelRow::receiverDelayFromBin(EndianIStream& eis) {
		
	receiverDelayExists = eis.readBoolean();
	if (receiverDelayExists) {
		
	
	
		
			
	
		receiverDelay.clear();
		
		unsigned int receiverDelayDim1 = eis.readInt();
		for (unsigned int  i = 0 ; i < receiverDelayDim1; i++)
			
			receiverDelay.push_back(eis.readDouble());
			
	

		
	

	}
	
}
void DelayModelRow::IFDelayFromBin(EndianIStream& eis) {
		
	IFDelayExists = eis.readBoolean();
	if (IFDelayExists) {
		
	
	
		
			
	
		IFDelay.clear();
		
		unsigned int IFDelayDim1 = eis.readInt();
		for (unsigned int  i = 0 ; i < IFDelayDim1; i++)
			
			IFDelay.push_back(eis.readDouble());
			
	

		
	

	}
	
}
void DelayModelRow::LODelayFromBin(EndianIStream& eis) {
		
	LODelayExists = eis.readBoolean();
	if (LODelayExists) {
		
	
	
		
			
	
		LODelay.clear();
		
		unsigned int LODelayDim1 = eis.readInt();
		for (unsigned int  i = 0 ; i < LODelayDim1; i++)
			
			LODelay.push_back(eis.readDouble());
			
	

		
	

	}
	
}
void DelayModelRow::crossPolarizationDelayFromBin(EndianIStream& eis) {
		
	crossPolarizationDelayExists = eis.readBoolean();
	if (crossPolarizationDelayExists) {
		
	
	
		
			
		crossPolarizationDelay =  eis.readDouble();
			
		
	

	}
	
}
	
	
	DelayModelRow* DelayModelRow::fromBin(EndianIStream& eis, DelayModelTable& table, const vector<string>& attributesSeq) {
		DelayModelRow* row = new  DelayModelRow(table);
		
		map<string, DelayModelAttributeFromBin>::iterator iter ;
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
					throw ConversionException("There is not method to read an attribute '"+attributesSeq.at(i)+"'.", "DelayModelTable");
			}
				
		}				
		return row;
	}

	//
	// A collection of methods to set the value of the attributes from their textual value in the XML representation
	// of one row.
	//
	
	// Convert a string into an Tag 
	void DelayModelRow::antennaIdFromText(const string & s) {
		 
          
		antennaId = ASDMValuesParser::parse<Tag>(s);
          
		
	}
	
	
	// Convert a string into an Tag 
	void DelayModelRow::spectralWindowIdFromText(const string & s) {
		 
          
		spectralWindowId = ASDMValuesParser::parse<Tag>(s);
          
		
	}
	
	
	// Convert a string into an ArrayTimeInterval 
	void DelayModelRow::timeIntervalFromText(const string & s) {
		 
          
		timeInterval = ASDMValuesParser::parse<ArrayTimeInterval>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void DelayModelRow::numPolyFromText(const string & s) {
		 
          
		numPoly = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	// Convert a string into an double 
	void DelayModelRow::phaseDelayFromText(const string & s) {
		 
          
		phaseDelay = ASDMValuesParser::parse1D<double>(s);
          
		
	}
	
	
	// Convert a string into an double 
	void DelayModelRow::phaseDelayRateFromText(const string & s) {
		 
          
		phaseDelayRate = ASDMValuesParser::parse1D<double>(s);
          
		
	}
	
	
	// Convert a string into an double 
	void DelayModelRow::groupDelayFromText(const string & s) {
		 
          
		groupDelay = ASDMValuesParser::parse1D<double>(s);
          
		
	}
	
	
	// Convert a string into an double 
	void DelayModelRow::groupDelayRateFromText(const string & s) {
		 
          
		groupDelayRate = ASDMValuesParser::parse1D<double>(s);
          
		
	}
	
	
	// Convert a string into an Tag 
	void DelayModelRow::fieldIdFromText(const string & s) {
		 
          
		fieldId = ASDMValuesParser::parse<Tag>(s);
          
		
	}
	

	
	// Convert a string into an ArrayTime 
	void DelayModelRow::timeOriginFromText(const string & s) {
		timeOriginExists = true;
		 
          
		timeOrigin = ASDMValuesParser::parse<ArrayTime>(s);
          
		
	}
	
	
	// Convert a string into an double 
	void DelayModelRow::atmosphericGroupDelayFromText(const string & s) {
		atmosphericGroupDelayExists = true;
		 
          
		atmosphericGroupDelay = ASDMValuesParser::parse<double>(s);
          
		
	}
	
	
	// Convert a string into an double 
	void DelayModelRow::atmosphericGroupDelayRateFromText(const string & s) {
		atmosphericGroupDelayRateExists = true;
		 
          
		atmosphericGroupDelayRate = ASDMValuesParser::parse<double>(s);
          
		
	}
	
	
	// Convert a string into an double 
	void DelayModelRow::geometricDelayFromText(const string & s) {
		geometricDelayExists = true;
		 
          
		geometricDelay = ASDMValuesParser::parse<double>(s);
          
		
	}
	
	
	// Convert a string into an double 
	void DelayModelRow::geometricDelayRateFromText(const string & s) {
		geometricDelayRateExists = true;
		 
          
		geometricDelayRate = ASDMValuesParser::parse<double>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void DelayModelRow::numLOFromText(const string & s) {
		numLOExists = true;
		 
          
		numLO = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	// Convert a string into an Frequency 
	void DelayModelRow::LOOffsetFromText(const string & s) {
		LOOffsetExists = true;
		 
          
		LOOffset = ASDMValuesParser::parse1D<Frequency>(s);
          
		
	}
	
	
	// Convert a string into an Frequency 
	void DelayModelRow::LOOffsetRateFromText(const string & s) {
		LOOffsetRateExists = true;
		 
          
		LOOffsetRate = ASDMValuesParser::parse1D<Frequency>(s);
          
		
	}
	
	
	// Convert a string into an double 
	void DelayModelRow::dispersiveDelayFromText(const string & s) {
		dispersiveDelayExists = true;
		 
          
		dispersiveDelay = ASDMValuesParser::parse<double>(s);
          
		
	}
	
	
	// Convert a string into an double 
	void DelayModelRow::dispersiveDelayRateFromText(const string & s) {
		dispersiveDelayRateExists = true;
		 
          
		dispersiveDelayRate = ASDMValuesParser::parse<double>(s);
          
		
	}
	
	
	// Convert a string into an double 
	void DelayModelRow::atmosphericDryDelayFromText(const string & s) {
		atmosphericDryDelayExists = true;
		 
          
		atmosphericDryDelay = ASDMValuesParser::parse<double>(s);
          
		
	}
	
	
	// Convert a string into an double 
	void DelayModelRow::atmosphericWetDelayFromText(const string & s) {
		atmosphericWetDelayExists = true;
		 
          
		atmosphericWetDelay = ASDMValuesParser::parse<double>(s);
          
		
	}
	
	
	// Convert a string into an double 
	void DelayModelRow::padDelayFromText(const string & s) {
		padDelayExists = true;
		 
          
		padDelay = ASDMValuesParser::parse<double>(s);
          
		
	}
	
	
	// Convert a string into an double 
	void DelayModelRow::antennaDelayFromText(const string & s) {
		antennaDelayExists = true;
		 
          
		antennaDelay = ASDMValuesParser::parse<double>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void DelayModelRow::numReceptorFromText(const string & s) {
		numReceptorExists = true;
		 
          
		numReceptor = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	// Convert a string into an PolarizationType 
	void DelayModelRow::polarizationTypeFromText(const string & s) {
		polarizationTypeExists = true;
		 
          
		polarizationType = ASDMValuesParser::parse1D<PolarizationTypeMod::PolarizationType>(s);
          
		
	}
	
	
	// Convert a string into an double 
	void DelayModelRow::electronicDelayFromText(const string & s) {
		electronicDelayExists = true;
		 
          
		electronicDelay = ASDMValuesParser::parse1D<double>(s);
          
		
	}
	
	
	// Convert a string into an double 
	void DelayModelRow::electronicDelayRateFromText(const string & s) {
		electronicDelayRateExists = true;
		 
          
		electronicDelayRate = ASDMValuesParser::parse1D<double>(s);
          
		
	}
	
	
	// Convert a string into an double 
	void DelayModelRow::receiverDelayFromText(const string & s) {
		receiverDelayExists = true;
		 
          
		receiverDelay = ASDMValuesParser::parse1D<double>(s);
          
		
	}
	
	
	// Convert a string into an double 
	void DelayModelRow::IFDelayFromText(const string & s) {
		IFDelayExists = true;
		 
          
		IFDelay = ASDMValuesParser::parse1D<double>(s);
          
		
	}
	
	
	// Convert a string into an double 
	void DelayModelRow::LODelayFromText(const string & s) {
		LODelayExists = true;
		 
          
		LODelay = ASDMValuesParser::parse1D<double>(s);
          
		
	}
	
	
	// Convert a string into an double 
	void DelayModelRow::crossPolarizationDelayFromText(const string & s) {
		crossPolarizationDelayExists = true;
		 
          
		crossPolarizationDelay = ASDMValuesParser::parse<double>(s);
          
		
	}
	
	
	
	void DelayModelRow::fromText(const std::string& attributeName, const std::string&  t) {
		map<string, DelayModelAttributeFromText>::iterator iter;
		if ((iter = fromTextMethods.find(attributeName)) == fromTextMethods.end())
			throw ConversionException("I do not know what to do with '"+attributeName+"' and its content '"+t+"' (while parsing an XML document)", "DelayModelTable");
		(this->*(iter->second))(t);
	}
			
	////////////////////////////////////////////////
	// Intrinsic Table Attributes getters/setters //
	////////////////////////////////////////////////
	
	

	
 	/**
 	 * Get timeInterval.
 	 * @return timeInterval as ArrayTimeInterval
 	 */
 	ArrayTimeInterval DelayModelRow::getTimeInterval() const {
	
  		return timeInterval;
 	}

 	/**
 	 * Set timeInterval with the specified ArrayTimeInterval.
 	 * @param timeInterval The ArrayTimeInterval value to which timeInterval is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void DelayModelRow::setTimeInterval (ArrayTimeInterval timeInterval)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("timeInterval", "DelayModel");
		
  		}
  	
 		this->timeInterval = timeInterval;
	
 	}
	
	

	

	
 	/**
 	 * Get numPoly.
 	 * @return numPoly as int
 	 */
 	int DelayModelRow::getNumPoly() const {
	
  		return numPoly;
 	}

 	/**
 	 * Set numPoly with the specified int.
 	 * @param numPoly The int value to which numPoly is to be set.
 	 
 	
 		
 	 */
 	void DelayModelRow::setNumPoly (int numPoly)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->numPoly = numPoly;
	
 	}
	
	

	

	
 	/**
 	 * Get phaseDelay.
 	 * @return phaseDelay as std::vector<double >
 	 */
 	std::vector<double > DelayModelRow::getPhaseDelay() const {
	
  		return phaseDelay;
 	}

 	/**
 	 * Set phaseDelay with the specified std::vector<double >.
 	 * @param phaseDelay The std::vector<double > value to which phaseDelay is to be set.
 	 
 	
 		
 	 */
 	void DelayModelRow::setPhaseDelay (std::vector<double > phaseDelay)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->phaseDelay = phaseDelay;
	
 	}
	
	

	

	
 	/**
 	 * Get phaseDelayRate.
 	 * @return phaseDelayRate as std::vector<double >
 	 */
 	std::vector<double > DelayModelRow::getPhaseDelayRate() const {
	
  		return phaseDelayRate;
 	}

 	/**
 	 * Set phaseDelayRate with the specified std::vector<double >.
 	 * @param phaseDelayRate The std::vector<double > value to which phaseDelayRate is to be set.
 	 
 	
 		
 	 */
 	void DelayModelRow::setPhaseDelayRate (std::vector<double > phaseDelayRate)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->phaseDelayRate = phaseDelayRate;
	
 	}
	
	

	

	
 	/**
 	 * Get groupDelay.
 	 * @return groupDelay as std::vector<double >
 	 */
 	std::vector<double > DelayModelRow::getGroupDelay() const {
	
  		return groupDelay;
 	}

 	/**
 	 * Set groupDelay with the specified std::vector<double >.
 	 * @param groupDelay The std::vector<double > value to which groupDelay is to be set.
 	 
 	
 		
 	 */
 	void DelayModelRow::setGroupDelay (std::vector<double > groupDelay)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->groupDelay = groupDelay;
	
 	}
	
	

	

	
 	/**
 	 * Get groupDelayRate.
 	 * @return groupDelayRate as std::vector<double >
 	 */
 	std::vector<double > DelayModelRow::getGroupDelayRate() const {
	
  		return groupDelayRate;
 	}

 	/**
 	 * Set groupDelayRate with the specified std::vector<double >.
 	 * @param groupDelayRate The std::vector<double > value to which groupDelayRate is to be set.
 	 
 	
 		
 	 */
 	void DelayModelRow::setGroupDelayRate (std::vector<double > groupDelayRate)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->groupDelayRate = groupDelayRate;
	
 	}
	
	

	
	/**
	 * The attribute timeOrigin is optional. Return true if this attribute exists.
	 * @return true if and only if the timeOrigin attribute exists. 
	 */
	bool DelayModelRow::isTimeOriginExists() const {
		return timeOriginExists;
	}
	

	
 	/**
 	 * Get timeOrigin, which is optional.
 	 * @return timeOrigin as ArrayTime
 	 * @throw IllegalAccessException If timeOrigin does not exist.
 	 */
 	ArrayTime DelayModelRow::getTimeOrigin() const  {
		if (!timeOriginExists) {
			throw IllegalAccessException("timeOrigin", "DelayModel");
		}
	
  		return timeOrigin;
 	}

 	/**
 	 * Set timeOrigin with the specified ArrayTime.
 	 * @param timeOrigin The ArrayTime value to which timeOrigin is to be set.
 	 
 	
 	 */
 	void DelayModelRow::setTimeOrigin (ArrayTime timeOrigin) {
	
 		this->timeOrigin = timeOrigin;
	
		timeOriginExists = true;
	
 	}
	
	
	/**
	 * Mark timeOrigin, which is an optional field, as non-existent.
	 */
	void DelayModelRow::clearTimeOrigin () {
		timeOriginExists = false;
	}
	

	
	/**
	 * The attribute atmosphericGroupDelay is optional. Return true if this attribute exists.
	 * @return true if and only if the atmosphericGroupDelay attribute exists. 
	 */
	bool DelayModelRow::isAtmosphericGroupDelayExists() const {
		return atmosphericGroupDelayExists;
	}
	

	
 	/**
 	 * Get atmosphericGroupDelay, which is optional.
 	 * @return atmosphericGroupDelay as double
 	 * @throw IllegalAccessException If atmosphericGroupDelay does not exist.
 	 */
 	double DelayModelRow::getAtmosphericGroupDelay() const  {
		if (!atmosphericGroupDelayExists) {
			throw IllegalAccessException("atmosphericGroupDelay", "DelayModel");
		}
	
  		return atmosphericGroupDelay;
 	}

 	/**
 	 * Set atmosphericGroupDelay with the specified double.
 	 * @param atmosphericGroupDelay The double value to which atmosphericGroupDelay is to be set.
 	 
 	
 	 */
 	void DelayModelRow::setAtmosphericGroupDelay (double atmosphericGroupDelay) {
	
 		this->atmosphericGroupDelay = atmosphericGroupDelay;
	
		atmosphericGroupDelayExists = true;
	
 	}
	
	
	/**
	 * Mark atmosphericGroupDelay, which is an optional field, as non-existent.
	 */
	void DelayModelRow::clearAtmosphericGroupDelay () {
		atmosphericGroupDelayExists = false;
	}
	

	
	/**
	 * The attribute atmosphericGroupDelayRate is optional. Return true if this attribute exists.
	 * @return true if and only if the atmosphericGroupDelayRate attribute exists. 
	 */
	bool DelayModelRow::isAtmosphericGroupDelayRateExists() const {
		return atmosphericGroupDelayRateExists;
	}
	

	
 	/**
 	 * Get atmosphericGroupDelayRate, which is optional.
 	 * @return atmosphericGroupDelayRate as double
 	 * @throw IllegalAccessException If atmosphericGroupDelayRate does not exist.
 	 */
 	double DelayModelRow::getAtmosphericGroupDelayRate() const  {
		if (!atmosphericGroupDelayRateExists) {
			throw IllegalAccessException("atmosphericGroupDelayRate", "DelayModel");
		}
	
  		return atmosphericGroupDelayRate;
 	}

 	/**
 	 * Set atmosphericGroupDelayRate with the specified double.
 	 * @param atmosphericGroupDelayRate The double value to which atmosphericGroupDelayRate is to be set.
 	 
 	
 	 */
 	void DelayModelRow::setAtmosphericGroupDelayRate (double atmosphericGroupDelayRate) {
	
 		this->atmosphericGroupDelayRate = atmosphericGroupDelayRate;
	
		atmosphericGroupDelayRateExists = true;
	
 	}
	
	
	/**
	 * Mark atmosphericGroupDelayRate, which is an optional field, as non-existent.
	 */
	void DelayModelRow::clearAtmosphericGroupDelayRate () {
		atmosphericGroupDelayRateExists = false;
	}
	

	
	/**
	 * The attribute geometricDelay is optional. Return true if this attribute exists.
	 * @return true if and only if the geometricDelay attribute exists. 
	 */
	bool DelayModelRow::isGeometricDelayExists() const {
		return geometricDelayExists;
	}
	

	
 	/**
 	 * Get geometricDelay, which is optional.
 	 * @return geometricDelay as double
 	 * @throw IllegalAccessException If geometricDelay does not exist.
 	 */
 	double DelayModelRow::getGeometricDelay() const  {
		if (!geometricDelayExists) {
			throw IllegalAccessException("geometricDelay", "DelayModel");
		}
	
  		return geometricDelay;
 	}

 	/**
 	 * Set geometricDelay with the specified double.
 	 * @param geometricDelay The double value to which geometricDelay is to be set.
 	 
 	
 	 */
 	void DelayModelRow::setGeometricDelay (double geometricDelay) {
	
 		this->geometricDelay = geometricDelay;
	
		geometricDelayExists = true;
	
 	}
	
	
	/**
	 * Mark geometricDelay, which is an optional field, as non-existent.
	 */
	void DelayModelRow::clearGeometricDelay () {
		geometricDelayExists = false;
	}
	

	
	/**
	 * The attribute geometricDelayRate is optional. Return true if this attribute exists.
	 * @return true if and only if the geometricDelayRate attribute exists. 
	 */
	bool DelayModelRow::isGeometricDelayRateExists() const {
		return geometricDelayRateExists;
	}
	

	
 	/**
 	 * Get geometricDelayRate, which is optional.
 	 * @return geometricDelayRate as double
 	 * @throw IllegalAccessException If geometricDelayRate does not exist.
 	 */
 	double DelayModelRow::getGeometricDelayRate() const  {
		if (!geometricDelayRateExists) {
			throw IllegalAccessException("geometricDelayRate", "DelayModel");
		}
	
  		return geometricDelayRate;
 	}

 	/**
 	 * Set geometricDelayRate with the specified double.
 	 * @param geometricDelayRate The double value to which geometricDelayRate is to be set.
 	 
 	
 	 */
 	void DelayModelRow::setGeometricDelayRate (double geometricDelayRate) {
	
 		this->geometricDelayRate = geometricDelayRate;
	
		geometricDelayRateExists = true;
	
 	}
	
	
	/**
	 * Mark geometricDelayRate, which is an optional field, as non-existent.
	 */
	void DelayModelRow::clearGeometricDelayRate () {
		geometricDelayRateExists = false;
	}
	

	
	/**
	 * The attribute numLO is optional. Return true if this attribute exists.
	 * @return true if and only if the numLO attribute exists. 
	 */
	bool DelayModelRow::isNumLOExists() const {
		return numLOExists;
	}
	

	
 	/**
 	 * Get numLO, which is optional.
 	 * @return numLO as int
 	 * @throw IllegalAccessException If numLO does not exist.
 	 */
 	int DelayModelRow::getNumLO() const  {
		if (!numLOExists) {
			throw IllegalAccessException("numLO", "DelayModel");
		}
	
  		return numLO;
 	}

 	/**
 	 * Set numLO with the specified int.
 	 * @param numLO The int value to which numLO is to be set.
 	 
 	
 	 */
 	void DelayModelRow::setNumLO (int numLO) {
	
 		this->numLO = numLO;
	
		numLOExists = true;
	
 	}
	
	
	/**
	 * Mark numLO, which is an optional field, as non-existent.
	 */
	void DelayModelRow::clearNumLO () {
		numLOExists = false;
	}
	

	
	/**
	 * The attribute LOOffset is optional. Return true if this attribute exists.
	 * @return true if and only if the LOOffset attribute exists. 
	 */
	bool DelayModelRow::isLOOffsetExists() const {
		return LOOffsetExists;
	}
	

	
 	/**
 	 * Get LOOffset, which is optional.
 	 * @return LOOffset as std::vector<Frequency >
 	 * @throw IllegalAccessException If LOOffset does not exist.
 	 */
 	std::vector<Frequency > DelayModelRow::getLOOffset() const  {
		if (!LOOffsetExists) {
			throw IllegalAccessException("LOOffset", "DelayModel");
		}
	
  		return LOOffset;
 	}

 	/**
 	 * Set LOOffset with the specified std::vector<Frequency >.
 	 * @param LOOffset The std::vector<Frequency > value to which LOOffset is to be set.
 	 
 	
 	 */
 	void DelayModelRow::setLOOffset (std::vector<Frequency > LOOffset) {
	
 		this->LOOffset = LOOffset;
	
		LOOffsetExists = true;
	
 	}
	
	
	/**
	 * Mark LOOffset, which is an optional field, as non-existent.
	 */
	void DelayModelRow::clearLOOffset () {
		LOOffsetExists = false;
	}
	

	
	/**
	 * The attribute LOOffsetRate is optional. Return true if this attribute exists.
	 * @return true if and only if the LOOffsetRate attribute exists. 
	 */
	bool DelayModelRow::isLOOffsetRateExists() const {
		return LOOffsetRateExists;
	}
	

	
 	/**
 	 * Get LOOffsetRate, which is optional.
 	 * @return LOOffsetRate as std::vector<Frequency >
 	 * @throw IllegalAccessException If LOOffsetRate does not exist.
 	 */
 	std::vector<Frequency > DelayModelRow::getLOOffsetRate() const  {
		if (!LOOffsetRateExists) {
			throw IllegalAccessException("LOOffsetRate", "DelayModel");
		}
	
  		return LOOffsetRate;
 	}

 	/**
 	 * Set LOOffsetRate with the specified std::vector<Frequency >.
 	 * @param LOOffsetRate The std::vector<Frequency > value to which LOOffsetRate is to be set.
 	 
 	
 	 */
 	void DelayModelRow::setLOOffsetRate (std::vector<Frequency > LOOffsetRate) {
	
 		this->LOOffsetRate = LOOffsetRate;
	
		LOOffsetRateExists = true;
	
 	}
	
	
	/**
	 * Mark LOOffsetRate, which is an optional field, as non-existent.
	 */
	void DelayModelRow::clearLOOffsetRate () {
		LOOffsetRateExists = false;
	}
	

	
	/**
	 * The attribute dispersiveDelay is optional. Return true if this attribute exists.
	 * @return true if and only if the dispersiveDelay attribute exists. 
	 */
	bool DelayModelRow::isDispersiveDelayExists() const {
		return dispersiveDelayExists;
	}
	

	
 	/**
 	 * Get dispersiveDelay, which is optional.
 	 * @return dispersiveDelay as double
 	 * @throw IllegalAccessException If dispersiveDelay does not exist.
 	 */
 	double DelayModelRow::getDispersiveDelay() const  {
		if (!dispersiveDelayExists) {
			throw IllegalAccessException("dispersiveDelay", "DelayModel");
		}
	
  		return dispersiveDelay;
 	}

 	/**
 	 * Set dispersiveDelay with the specified double.
 	 * @param dispersiveDelay The double value to which dispersiveDelay is to be set.
 	 
 	
 	 */
 	void DelayModelRow::setDispersiveDelay (double dispersiveDelay) {
	
 		this->dispersiveDelay = dispersiveDelay;
	
		dispersiveDelayExists = true;
	
 	}
	
	
	/**
	 * Mark dispersiveDelay, which is an optional field, as non-existent.
	 */
	void DelayModelRow::clearDispersiveDelay () {
		dispersiveDelayExists = false;
	}
	

	
	/**
	 * The attribute dispersiveDelayRate is optional. Return true if this attribute exists.
	 * @return true if and only if the dispersiveDelayRate attribute exists. 
	 */
	bool DelayModelRow::isDispersiveDelayRateExists() const {
		return dispersiveDelayRateExists;
	}
	

	
 	/**
 	 * Get dispersiveDelayRate, which is optional.
 	 * @return dispersiveDelayRate as double
 	 * @throw IllegalAccessException If dispersiveDelayRate does not exist.
 	 */
 	double DelayModelRow::getDispersiveDelayRate() const  {
		if (!dispersiveDelayRateExists) {
			throw IllegalAccessException("dispersiveDelayRate", "DelayModel");
		}
	
  		return dispersiveDelayRate;
 	}

 	/**
 	 * Set dispersiveDelayRate with the specified double.
 	 * @param dispersiveDelayRate The double value to which dispersiveDelayRate is to be set.
 	 
 	
 	 */
 	void DelayModelRow::setDispersiveDelayRate (double dispersiveDelayRate) {
	
 		this->dispersiveDelayRate = dispersiveDelayRate;
	
		dispersiveDelayRateExists = true;
	
 	}
	
	
	/**
	 * Mark dispersiveDelayRate, which is an optional field, as non-existent.
	 */
	void DelayModelRow::clearDispersiveDelayRate () {
		dispersiveDelayRateExists = false;
	}
	

	
	/**
	 * The attribute atmosphericDryDelay is optional. Return true if this attribute exists.
	 * @return true if and only if the atmosphericDryDelay attribute exists. 
	 */
	bool DelayModelRow::isAtmosphericDryDelayExists() const {
		return atmosphericDryDelayExists;
	}
	

	
 	/**
 	 * Get atmosphericDryDelay, which is optional.
 	 * @return atmosphericDryDelay as double
 	 * @throw IllegalAccessException If atmosphericDryDelay does not exist.
 	 */
 	double DelayModelRow::getAtmosphericDryDelay() const  {
		if (!atmosphericDryDelayExists) {
			throw IllegalAccessException("atmosphericDryDelay", "DelayModel");
		}
	
  		return atmosphericDryDelay;
 	}

 	/**
 	 * Set atmosphericDryDelay with the specified double.
 	 * @param atmosphericDryDelay The double value to which atmosphericDryDelay is to be set.
 	 
 	
 	 */
 	void DelayModelRow::setAtmosphericDryDelay (double atmosphericDryDelay) {
	
 		this->atmosphericDryDelay = atmosphericDryDelay;
	
		atmosphericDryDelayExists = true;
	
 	}
	
	
	/**
	 * Mark atmosphericDryDelay, which is an optional field, as non-existent.
	 */
	void DelayModelRow::clearAtmosphericDryDelay () {
		atmosphericDryDelayExists = false;
	}
	

	
	/**
	 * The attribute atmosphericWetDelay is optional. Return true if this attribute exists.
	 * @return true if and only if the atmosphericWetDelay attribute exists. 
	 */
	bool DelayModelRow::isAtmosphericWetDelayExists() const {
		return atmosphericWetDelayExists;
	}
	

	
 	/**
 	 * Get atmosphericWetDelay, which is optional.
 	 * @return atmosphericWetDelay as double
 	 * @throw IllegalAccessException If atmosphericWetDelay does not exist.
 	 */
 	double DelayModelRow::getAtmosphericWetDelay() const  {
		if (!atmosphericWetDelayExists) {
			throw IllegalAccessException("atmosphericWetDelay", "DelayModel");
		}
	
  		return atmosphericWetDelay;
 	}

 	/**
 	 * Set atmosphericWetDelay with the specified double.
 	 * @param atmosphericWetDelay The double value to which atmosphericWetDelay is to be set.
 	 
 	
 	 */
 	void DelayModelRow::setAtmosphericWetDelay (double atmosphericWetDelay) {
	
 		this->atmosphericWetDelay = atmosphericWetDelay;
	
		atmosphericWetDelayExists = true;
	
 	}
	
	
	/**
	 * Mark atmosphericWetDelay, which is an optional field, as non-existent.
	 */
	void DelayModelRow::clearAtmosphericWetDelay () {
		atmosphericWetDelayExists = false;
	}
	

	
	/**
	 * The attribute padDelay is optional. Return true if this attribute exists.
	 * @return true if and only if the padDelay attribute exists. 
	 */
	bool DelayModelRow::isPadDelayExists() const {
		return padDelayExists;
	}
	

	
 	/**
 	 * Get padDelay, which is optional.
 	 * @return padDelay as double
 	 * @throw IllegalAccessException If padDelay does not exist.
 	 */
 	double DelayModelRow::getPadDelay() const  {
		if (!padDelayExists) {
			throw IllegalAccessException("padDelay", "DelayModel");
		}
	
  		return padDelay;
 	}

 	/**
 	 * Set padDelay with the specified double.
 	 * @param padDelay The double value to which padDelay is to be set.
 	 
 	
 	 */
 	void DelayModelRow::setPadDelay (double padDelay) {
	
 		this->padDelay = padDelay;
	
		padDelayExists = true;
	
 	}
	
	
	/**
	 * Mark padDelay, which is an optional field, as non-existent.
	 */
	void DelayModelRow::clearPadDelay () {
		padDelayExists = false;
	}
	

	
	/**
	 * The attribute antennaDelay is optional. Return true if this attribute exists.
	 * @return true if and only if the antennaDelay attribute exists. 
	 */
	bool DelayModelRow::isAntennaDelayExists() const {
		return antennaDelayExists;
	}
	

	
 	/**
 	 * Get antennaDelay, which is optional.
 	 * @return antennaDelay as double
 	 * @throw IllegalAccessException If antennaDelay does not exist.
 	 */
 	double DelayModelRow::getAntennaDelay() const  {
		if (!antennaDelayExists) {
			throw IllegalAccessException("antennaDelay", "DelayModel");
		}
	
  		return antennaDelay;
 	}

 	/**
 	 * Set antennaDelay with the specified double.
 	 * @param antennaDelay The double value to which antennaDelay is to be set.
 	 
 	
 	 */
 	void DelayModelRow::setAntennaDelay (double antennaDelay) {
	
 		this->antennaDelay = antennaDelay;
	
		antennaDelayExists = true;
	
 	}
	
	
	/**
	 * Mark antennaDelay, which is an optional field, as non-existent.
	 */
	void DelayModelRow::clearAntennaDelay () {
		antennaDelayExists = false;
	}
	

	
	/**
	 * The attribute numReceptor is optional. Return true if this attribute exists.
	 * @return true if and only if the numReceptor attribute exists. 
	 */
	bool DelayModelRow::isNumReceptorExists() const {
		return numReceptorExists;
	}
	

	
 	/**
 	 * Get numReceptor, which is optional.
 	 * @return numReceptor as int
 	 * @throw IllegalAccessException If numReceptor does not exist.
 	 */
 	int DelayModelRow::getNumReceptor() const  {
		if (!numReceptorExists) {
			throw IllegalAccessException("numReceptor", "DelayModel");
		}
	
  		return numReceptor;
 	}

 	/**
 	 * Set numReceptor with the specified int.
 	 * @param numReceptor The int value to which numReceptor is to be set.
 	 
 	
 	 */
 	void DelayModelRow::setNumReceptor (int numReceptor) {
	
 		this->numReceptor = numReceptor;
	
		numReceptorExists = true;
	
 	}
	
	
	/**
	 * Mark numReceptor, which is an optional field, as non-existent.
	 */
	void DelayModelRow::clearNumReceptor () {
		numReceptorExists = false;
	}
	

	
	/**
	 * The attribute polarizationType is optional. Return true if this attribute exists.
	 * @return true if and only if the polarizationType attribute exists. 
	 */
	bool DelayModelRow::isPolarizationTypeExists() const {
		return polarizationTypeExists;
	}
	

	
 	/**
 	 * Get polarizationType, which is optional.
 	 * @return polarizationType as std::vector<PolarizationTypeMod::PolarizationType >
 	 * @throw IllegalAccessException If polarizationType does not exist.
 	 */
 	std::vector<PolarizationTypeMod::PolarizationType > DelayModelRow::getPolarizationType() const  {
		if (!polarizationTypeExists) {
			throw IllegalAccessException("polarizationType", "DelayModel");
		}
	
  		return polarizationType;
 	}

 	/**
 	 * Set polarizationType with the specified std::vector<PolarizationTypeMod::PolarizationType >.
 	 * @param polarizationType The std::vector<PolarizationTypeMod::PolarizationType > value to which polarizationType is to be set.
 	 
 	
 	 */
 	void DelayModelRow::setPolarizationType (std::vector<PolarizationTypeMod::PolarizationType > polarizationType) {
	
 		this->polarizationType = polarizationType;
	
		polarizationTypeExists = true;
	
 	}
	
	
	/**
	 * Mark polarizationType, which is an optional field, as non-existent.
	 */
	void DelayModelRow::clearPolarizationType () {
		polarizationTypeExists = false;
	}
	

	
	/**
	 * The attribute electronicDelay is optional. Return true if this attribute exists.
	 * @return true if and only if the electronicDelay attribute exists. 
	 */
	bool DelayModelRow::isElectronicDelayExists() const {
		return electronicDelayExists;
	}
	

	
 	/**
 	 * Get electronicDelay, which is optional.
 	 * @return electronicDelay as std::vector<double >
 	 * @throw IllegalAccessException If electronicDelay does not exist.
 	 */
 	std::vector<double > DelayModelRow::getElectronicDelay() const  {
		if (!electronicDelayExists) {
			throw IllegalAccessException("electronicDelay", "DelayModel");
		}
	
  		return electronicDelay;
 	}

 	/**
 	 * Set electronicDelay with the specified std::vector<double >.
 	 * @param electronicDelay The std::vector<double > value to which electronicDelay is to be set.
 	 
 	
 	 */
 	void DelayModelRow::setElectronicDelay (std::vector<double > electronicDelay) {
	
 		this->electronicDelay = electronicDelay;
	
		electronicDelayExists = true;
	
 	}
	
	
	/**
	 * Mark electronicDelay, which is an optional field, as non-existent.
	 */
	void DelayModelRow::clearElectronicDelay () {
		electronicDelayExists = false;
	}
	

	
	/**
	 * The attribute electronicDelayRate is optional. Return true if this attribute exists.
	 * @return true if and only if the electronicDelayRate attribute exists. 
	 */
	bool DelayModelRow::isElectronicDelayRateExists() const {
		return electronicDelayRateExists;
	}
	

	
 	/**
 	 * Get electronicDelayRate, which is optional.
 	 * @return electronicDelayRate as std::vector<double >
 	 * @throw IllegalAccessException If electronicDelayRate does not exist.
 	 */
 	std::vector<double > DelayModelRow::getElectronicDelayRate() const  {
		if (!electronicDelayRateExists) {
			throw IllegalAccessException("electronicDelayRate", "DelayModel");
		}
	
  		return electronicDelayRate;
 	}

 	/**
 	 * Set electronicDelayRate with the specified std::vector<double >.
 	 * @param electronicDelayRate The std::vector<double > value to which electronicDelayRate is to be set.
 	 
 	
 	 */
 	void DelayModelRow::setElectronicDelayRate (std::vector<double > electronicDelayRate) {
	
 		this->electronicDelayRate = electronicDelayRate;
	
		electronicDelayRateExists = true;
	
 	}
	
	
	/**
	 * Mark electronicDelayRate, which is an optional field, as non-existent.
	 */
	void DelayModelRow::clearElectronicDelayRate () {
		electronicDelayRateExists = false;
	}
	

	
	/**
	 * The attribute receiverDelay is optional. Return true if this attribute exists.
	 * @return true if and only if the receiverDelay attribute exists. 
	 */
	bool DelayModelRow::isReceiverDelayExists() const {
		return receiverDelayExists;
	}
	

	
 	/**
 	 * Get receiverDelay, which is optional.
 	 * @return receiverDelay as std::vector<double >
 	 * @throw IllegalAccessException If receiverDelay does not exist.
 	 */
 	std::vector<double > DelayModelRow::getReceiverDelay() const  {
		if (!receiverDelayExists) {
			throw IllegalAccessException("receiverDelay", "DelayModel");
		}
	
  		return receiverDelay;
 	}

 	/**
 	 * Set receiverDelay with the specified std::vector<double >.
 	 * @param receiverDelay The std::vector<double > value to which receiverDelay is to be set.
 	 
 	
 	 */
 	void DelayModelRow::setReceiverDelay (std::vector<double > receiverDelay) {
	
 		this->receiverDelay = receiverDelay;
	
		receiverDelayExists = true;
	
 	}
	
	
	/**
	 * Mark receiverDelay, which is an optional field, as non-existent.
	 */
	void DelayModelRow::clearReceiverDelay () {
		receiverDelayExists = false;
	}
	

	
	/**
	 * The attribute IFDelay is optional. Return true if this attribute exists.
	 * @return true if and only if the IFDelay attribute exists. 
	 */
	bool DelayModelRow::isIFDelayExists() const {
		return IFDelayExists;
	}
	

	
 	/**
 	 * Get IFDelay, which is optional.
 	 * @return IFDelay as std::vector<double >
 	 * @throw IllegalAccessException If IFDelay does not exist.
 	 */
 	std::vector<double > DelayModelRow::getIFDelay() const  {
		if (!IFDelayExists) {
			throw IllegalAccessException("IFDelay", "DelayModel");
		}
	
  		return IFDelay;
 	}

 	/**
 	 * Set IFDelay with the specified std::vector<double >.
 	 * @param IFDelay The std::vector<double > value to which IFDelay is to be set.
 	 
 	
 	 */
 	void DelayModelRow::setIFDelay (std::vector<double > IFDelay) {
	
 		this->IFDelay = IFDelay;
	
		IFDelayExists = true;
	
 	}
	
	
	/**
	 * Mark IFDelay, which is an optional field, as non-existent.
	 */
	void DelayModelRow::clearIFDelay () {
		IFDelayExists = false;
	}
	

	
	/**
	 * The attribute LODelay is optional. Return true if this attribute exists.
	 * @return true if and only if the LODelay attribute exists. 
	 */
	bool DelayModelRow::isLODelayExists() const {
		return LODelayExists;
	}
	

	
 	/**
 	 * Get LODelay, which is optional.
 	 * @return LODelay as std::vector<double >
 	 * @throw IllegalAccessException If LODelay does not exist.
 	 */
 	std::vector<double > DelayModelRow::getLODelay() const  {
		if (!LODelayExists) {
			throw IllegalAccessException("LODelay", "DelayModel");
		}
	
  		return LODelay;
 	}

 	/**
 	 * Set LODelay with the specified std::vector<double >.
 	 * @param LODelay The std::vector<double > value to which LODelay is to be set.
 	 
 	
 	 */
 	void DelayModelRow::setLODelay (std::vector<double > LODelay) {
	
 		this->LODelay = LODelay;
	
		LODelayExists = true;
	
 	}
	
	
	/**
	 * Mark LODelay, which is an optional field, as non-existent.
	 */
	void DelayModelRow::clearLODelay () {
		LODelayExists = false;
	}
	

	
	/**
	 * The attribute crossPolarizationDelay is optional. Return true if this attribute exists.
	 * @return true if and only if the crossPolarizationDelay attribute exists. 
	 */
	bool DelayModelRow::isCrossPolarizationDelayExists() const {
		return crossPolarizationDelayExists;
	}
	

	
 	/**
 	 * Get crossPolarizationDelay, which is optional.
 	 * @return crossPolarizationDelay as double
 	 * @throw IllegalAccessException If crossPolarizationDelay does not exist.
 	 */
 	double DelayModelRow::getCrossPolarizationDelay() const  {
		if (!crossPolarizationDelayExists) {
			throw IllegalAccessException("crossPolarizationDelay", "DelayModel");
		}
	
  		return crossPolarizationDelay;
 	}

 	/**
 	 * Set crossPolarizationDelay with the specified double.
 	 * @param crossPolarizationDelay The double value to which crossPolarizationDelay is to be set.
 	 
 	
 	 */
 	void DelayModelRow::setCrossPolarizationDelay (double crossPolarizationDelay) {
	
 		this->crossPolarizationDelay = crossPolarizationDelay;
	
		crossPolarizationDelayExists = true;
	
 	}
	
	
	/**
	 * Mark crossPolarizationDelay, which is an optional field, as non-existent.
	 */
	void DelayModelRow::clearCrossPolarizationDelay () {
		crossPolarizationDelayExists = false;
	}
	

	
	///////////////////////////////////////////////
	// Extrinsic Table Attributes getters/setters//
	///////////////////////////////////////////////
	
	

	
 	/**
 	 * Get antennaId.
 	 * @return antennaId as Tag
 	 */
 	Tag DelayModelRow::getAntennaId() const {
	
  		return antennaId;
 	}

 	/**
 	 * Set antennaId with the specified Tag.
 	 * @param antennaId The Tag value to which antennaId is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void DelayModelRow::setAntennaId (Tag antennaId)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("antennaId", "DelayModel");
		
  		}
  	
 		this->antennaId = antennaId;
	
 	}
	
	

	

	
 	/**
 	 * Get fieldId.
 	 * @return fieldId as Tag
 	 */
 	Tag DelayModelRow::getFieldId() const {
	
  		return fieldId;
 	}

 	/**
 	 * Set fieldId with the specified Tag.
 	 * @param fieldId The Tag value to which fieldId is to be set.
 	 
 	
 		
 	 */
 	void DelayModelRow::setFieldId (Tag fieldId)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->fieldId = fieldId;
	
 	}
	
	

	

	
 	/**
 	 * Get spectralWindowId.
 	 * @return spectralWindowId as Tag
 	 */
 	Tag DelayModelRow::getSpectralWindowId() const {
	
  		return spectralWindowId;
 	}

 	/**
 	 * Set spectralWindowId with the specified Tag.
 	 * @param spectralWindowId The Tag value to which spectralWindowId is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void DelayModelRow::setSpectralWindowId (Tag spectralWindowId)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("spectralWindowId", "DelayModel");
		
  		}
  	
 		this->spectralWindowId = spectralWindowId;
	
 	}
	
	


	//////////////////////////////////////
	// Links Attributes getters/setters //
	//////////////////////////////////////
	
	
	
	
		

	/**
	 * Returns the pointer to the row in the Antenna table having Antenna.antennaId == antennaId
	 * @return a AntennaRow*
	 * 
	 
	 */
	 AntennaRow* DelayModelRow::getAntennaUsingAntennaId() {
	 
	 	return table.getContainer().getAntenna().getRowByKey(antennaId);
	 }
	 

	

	
	
	
		

	/**
	 * Returns the pointer to the row in the SpectralWindow table having SpectralWindow.spectralWindowId == spectralWindowId
	 * @return a SpectralWindowRow*
	 * 
	 
	 */
	 SpectralWindowRow* DelayModelRow::getSpectralWindowUsingSpectralWindowId() {
	 
	 	return table.getContainer().getSpectralWindow().getRowByKey(spectralWindowId);
	 }
	 

	

	
	
	
		

	/**
	 * Returns the pointer to the row in the Field table having Field.fieldId == fieldId
	 * @return a FieldRow*
	 * 
	 
	 */
	 FieldRow* DelayModelRow::getFieldUsingFieldId() {
	 
	 	return table.getContainer().getField().getRowByKey(fieldId);
	 }
	 

	

	
	/**
	 * Create a DelayModelRow.
	 * <p>
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 */ 
	DelayModelRow::DelayModelRow (DelayModelTable &t) : table(t) {
		hasBeenAdded = false;
		
	
	

	

	

	

	

	

	
		timeOriginExists = false;
	

	
		atmosphericGroupDelayExists = false;
	

	
		atmosphericGroupDelayRateExists = false;
	

	
		geometricDelayExists = false;
	

	
		geometricDelayRateExists = false;
	

	
		numLOExists = false;
	

	
		LOOffsetExists = false;
	

	
		LOOffsetRateExists = false;
	

	
		dispersiveDelayExists = false;
	

	
		dispersiveDelayRateExists = false;
	

	
		atmosphericDryDelayExists = false;
	

	
		atmosphericWetDelayExists = false;
	

	
		padDelayExists = false;
	

	
		antennaDelayExists = false;
	

	
		numReceptorExists = false;
	

	
		polarizationTypeExists = false;
	

	
		electronicDelayExists = false;
	

	
		electronicDelayRateExists = false;
	

	
		receiverDelayExists = false;
	

	
		IFDelayExists = false;
	

	
		LODelayExists = false;
	

	
		crossPolarizationDelayExists = false;
	

	
	

	

	

	
	
	
	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	
	
	 fromBinMethods["antennaId"] = &DelayModelRow::antennaIdFromBin; 
	 fromBinMethods["spectralWindowId"] = &DelayModelRow::spectralWindowIdFromBin; 
	 fromBinMethods["timeInterval"] = &DelayModelRow::timeIntervalFromBin; 
	 fromBinMethods["numPoly"] = &DelayModelRow::numPolyFromBin; 
	 fromBinMethods["phaseDelay"] = &DelayModelRow::phaseDelayFromBin; 
	 fromBinMethods["phaseDelayRate"] = &DelayModelRow::phaseDelayRateFromBin; 
	 fromBinMethods["groupDelay"] = &DelayModelRow::groupDelayFromBin; 
	 fromBinMethods["groupDelayRate"] = &DelayModelRow::groupDelayRateFromBin; 
	 fromBinMethods["fieldId"] = &DelayModelRow::fieldIdFromBin; 
		
	
	 fromBinMethods["timeOrigin"] = &DelayModelRow::timeOriginFromBin; 
	 fromBinMethods["atmosphericGroupDelay"] = &DelayModelRow::atmosphericGroupDelayFromBin; 
	 fromBinMethods["atmosphericGroupDelayRate"] = &DelayModelRow::atmosphericGroupDelayRateFromBin; 
	 fromBinMethods["geometricDelay"] = &DelayModelRow::geometricDelayFromBin; 
	 fromBinMethods["geometricDelayRate"] = &DelayModelRow::geometricDelayRateFromBin; 
	 fromBinMethods["numLO"] = &DelayModelRow::numLOFromBin; 
	 fromBinMethods["LOOffset"] = &DelayModelRow::LOOffsetFromBin; 
	 fromBinMethods["LOOffsetRate"] = &DelayModelRow::LOOffsetRateFromBin; 
	 fromBinMethods["dispersiveDelay"] = &DelayModelRow::dispersiveDelayFromBin; 
	 fromBinMethods["dispersiveDelayRate"] = &DelayModelRow::dispersiveDelayRateFromBin; 
	 fromBinMethods["atmosphericDryDelay"] = &DelayModelRow::atmosphericDryDelayFromBin; 
	 fromBinMethods["atmosphericWetDelay"] = &DelayModelRow::atmosphericWetDelayFromBin; 
	 fromBinMethods["padDelay"] = &DelayModelRow::padDelayFromBin; 
	 fromBinMethods["antennaDelay"] = &DelayModelRow::antennaDelayFromBin; 
	 fromBinMethods["numReceptor"] = &DelayModelRow::numReceptorFromBin; 
	 fromBinMethods["polarizationType"] = &DelayModelRow::polarizationTypeFromBin; 
	 fromBinMethods["electronicDelay"] = &DelayModelRow::electronicDelayFromBin; 
	 fromBinMethods["electronicDelayRate"] = &DelayModelRow::electronicDelayRateFromBin; 
	 fromBinMethods["receiverDelay"] = &DelayModelRow::receiverDelayFromBin; 
	 fromBinMethods["IFDelay"] = &DelayModelRow::IFDelayFromBin; 
	 fromBinMethods["LODelay"] = &DelayModelRow::LODelayFromBin; 
	 fromBinMethods["crossPolarizationDelay"] = &DelayModelRow::crossPolarizationDelayFromBin; 
	
	
	
	
				 
	fromTextMethods["antennaId"] = &DelayModelRow::antennaIdFromText;
		 
	
				 
	fromTextMethods["spectralWindowId"] = &DelayModelRow::spectralWindowIdFromText;
		 
	
				 
	fromTextMethods["timeInterval"] = &DelayModelRow::timeIntervalFromText;
		 
	
				 
	fromTextMethods["numPoly"] = &DelayModelRow::numPolyFromText;
		 
	
				 
	fromTextMethods["phaseDelay"] = &DelayModelRow::phaseDelayFromText;
		 
	
				 
	fromTextMethods["phaseDelayRate"] = &DelayModelRow::phaseDelayRateFromText;
		 
	
				 
	fromTextMethods["groupDelay"] = &DelayModelRow::groupDelayFromText;
		 
	
				 
	fromTextMethods["groupDelayRate"] = &DelayModelRow::groupDelayRateFromText;
		 
	
				 
	fromTextMethods["fieldId"] = &DelayModelRow::fieldIdFromText;
		 
	

	 
				
	fromTextMethods["timeOrigin"] = &DelayModelRow::timeOriginFromText;
		 	
	 
				
	fromTextMethods["atmosphericGroupDelay"] = &DelayModelRow::atmosphericGroupDelayFromText;
		 	
	 
				
	fromTextMethods["atmosphericGroupDelayRate"] = &DelayModelRow::atmosphericGroupDelayRateFromText;
		 	
	 
				
	fromTextMethods["geometricDelay"] = &DelayModelRow::geometricDelayFromText;
		 	
	 
				
	fromTextMethods["geometricDelayRate"] = &DelayModelRow::geometricDelayRateFromText;
		 	
	 
				
	fromTextMethods["numLO"] = &DelayModelRow::numLOFromText;
		 	
	 
				
	fromTextMethods["LOOffset"] = &DelayModelRow::LOOffsetFromText;
		 	
	 
				
	fromTextMethods["LOOffsetRate"] = &DelayModelRow::LOOffsetRateFromText;
		 	
	 
				
	fromTextMethods["dispersiveDelay"] = &DelayModelRow::dispersiveDelayFromText;
		 	
	 
				
	fromTextMethods["dispersiveDelayRate"] = &DelayModelRow::dispersiveDelayRateFromText;
		 	
	 
				
	fromTextMethods["atmosphericDryDelay"] = &DelayModelRow::atmosphericDryDelayFromText;
		 	
	 
				
	fromTextMethods["atmosphericWetDelay"] = &DelayModelRow::atmosphericWetDelayFromText;
		 	
	 
				
	fromTextMethods["padDelay"] = &DelayModelRow::padDelayFromText;
		 	
	 
				
	fromTextMethods["antennaDelay"] = &DelayModelRow::antennaDelayFromText;
		 	
	 
				
	fromTextMethods["numReceptor"] = &DelayModelRow::numReceptorFromText;
		 	
	 
				
	fromTextMethods["polarizationType"] = &DelayModelRow::polarizationTypeFromText;
		 	
	 
				
	fromTextMethods["electronicDelay"] = &DelayModelRow::electronicDelayFromText;
		 	
	 
				
	fromTextMethods["electronicDelayRate"] = &DelayModelRow::electronicDelayRateFromText;
		 	
	 
				
	fromTextMethods["receiverDelay"] = &DelayModelRow::receiverDelayFromText;
		 	
	 
				
	fromTextMethods["IFDelay"] = &DelayModelRow::IFDelayFromText;
		 	
	 
				
	fromTextMethods["LODelay"] = &DelayModelRow::LODelayFromText;
		 	
	 
				
	fromTextMethods["crossPolarizationDelay"] = &DelayModelRow::crossPolarizationDelayFromText;
		 	
		
	}
	
	DelayModelRow::DelayModelRow (DelayModelTable &t, DelayModelRow *row) : table(t) {
		hasBeenAdded = false;
		
		if (row == 0) {
	
	
	

	

	

	

	

	

	
		timeOriginExists = false;
	

	
		atmosphericGroupDelayExists = false;
	

	
		atmosphericGroupDelayRateExists = false;
	

	
		geometricDelayExists = false;
	

	
		geometricDelayRateExists = false;
	

	
		numLOExists = false;
	

	
		LOOffsetExists = false;
	

	
		LOOffsetRateExists = false;
	

	
		dispersiveDelayExists = false;
	

	
		dispersiveDelayRateExists = false;
	

	
		atmosphericDryDelayExists = false;
	

	
		atmosphericWetDelayExists = false;
	

	
		padDelayExists = false;
	

	
		antennaDelayExists = false;
	

	
		numReceptorExists = false;
	

	
		polarizationTypeExists = false;
	

	
		electronicDelayExists = false;
	

	
		electronicDelayRateExists = false;
	

	
		receiverDelayExists = false;
	

	
		IFDelayExists = false;
	

	
		LODelayExists = false;
	

	
		crossPolarizationDelayExists = false;
	

	
	

	

	
		
		}
		else {
	
		
			antennaId = row->antennaId;
		
			spectralWindowId = row->spectralWindowId;
		
			timeInterval = row->timeInterval;
		
		
		
		
			numPoly = row->numPoly;
		
			phaseDelay = row->phaseDelay;
		
			phaseDelayRate = row->phaseDelayRate;
		
			groupDelay = row->groupDelay;
		
			groupDelayRate = row->groupDelayRate;
		
			fieldId = row->fieldId;
		
		
		
		
		if (row->timeOriginExists) {
			timeOrigin = row->timeOrigin;		
			timeOriginExists = true;
		}
		else
			timeOriginExists = false;
		
		if (row->atmosphericGroupDelayExists) {
			atmosphericGroupDelay = row->atmosphericGroupDelay;		
			atmosphericGroupDelayExists = true;
		}
		else
			atmosphericGroupDelayExists = false;
		
		if (row->atmosphericGroupDelayRateExists) {
			atmosphericGroupDelayRate = row->atmosphericGroupDelayRate;		
			atmosphericGroupDelayRateExists = true;
		}
		else
			atmosphericGroupDelayRateExists = false;
		
		if (row->geometricDelayExists) {
			geometricDelay = row->geometricDelay;		
			geometricDelayExists = true;
		}
		else
			geometricDelayExists = false;
		
		if (row->geometricDelayRateExists) {
			geometricDelayRate = row->geometricDelayRate;		
			geometricDelayRateExists = true;
		}
		else
			geometricDelayRateExists = false;
		
		if (row->numLOExists) {
			numLO = row->numLO;		
			numLOExists = true;
		}
		else
			numLOExists = false;
		
		if (row->LOOffsetExists) {
			LOOffset = row->LOOffset;		
			LOOffsetExists = true;
		}
		else
			LOOffsetExists = false;
		
		if (row->LOOffsetRateExists) {
			LOOffsetRate = row->LOOffsetRate;		
			LOOffsetRateExists = true;
		}
		else
			LOOffsetRateExists = false;
		
		if (row->dispersiveDelayExists) {
			dispersiveDelay = row->dispersiveDelay;		
			dispersiveDelayExists = true;
		}
		else
			dispersiveDelayExists = false;
		
		if (row->dispersiveDelayRateExists) {
			dispersiveDelayRate = row->dispersiveDelayRate;		
			dispersiveDelayRateExists = true;
		}
		else
			dispersiveDelayRateExists = false;
		
		if (row->atmosphericDryDelayExists) {
			atmosphericDryDelay = row->atmosphericDryDelay;		
			atmosphericDryDelayExists = true;
		}
		else
			atmosphericDryDelayExists = false;
		
		if (row->atmosphericWetDelayExists) {
			atmosphericWetDelay = row->atmosphericWetDelay;		
			atmosphericWetDelayExists = true;
		}
		else
			atmosphericWetDelayExists = false;
		
		if (row->padDelayExists) {
			padDelay = row->padDelay;		
			padDelayExists = true;
		}
		else
			padDelayExists = false;
		
		if (row->antennaDelayExists) {
			antennaDelay = row->antennaDelay;		
			antennaDelayExists = true;
		}
		else
			antennaDelayExists = false;
		
		if (row->numReceptorExists) {
			numReceptor = row->numReceptor;		
			numReceptorExists = true;
		}
		else
			numReceptorExists = false;
		
		if (row->polarizationTypeExists) {
			polarizationType = row->polarizationType;		
			polarizationTypeExists = true;
		}
		else
			polarizationTypeExists = false;
		
		if (row->electronicDelayExists) {
			electronicDelay = row->electronicDelay;		
			electronicDelayExists = true;
		}
		else
			electronicDelayExists = false;
		
		if (row->electronicDelayRateExists) {
			electronicDelayRate = row->electronicDelayRate;		
			electronicDelayRateExists = true;
		}
		else
			electronicDelayRateExists = false;
		
		if (row->receiverDelayExists) {
			receiverDelay = row->receiverDelay;		
			receiverDelayExists = true;
		}
		else
			receiverDelayExists = false;
		
		if (row->IFDelayExists) {
			IFDelay = row->IFDelay;		
			IFDelayExists = true;
		}
		else
			IFDelayExists = false;
		
		if (row->LODelayExists) {
			LODelay = row->LODelay;		
			LODelayExists = true;
		}
		else
			LODelayExists = false;
		
		if (row->crossPolarizationDelayExists) {
			crossPolarizationDelay = row->crossPolarizationDelay;		
			crossPolarizationDelayExists = true;
		}
		else
			crossPolarizationDelayExists = false;
		
		}
		
		 fromBinMethods["antennaId"] = &DelayModelRow::antennaIdFromBin; 
		 fromBinMethods["spectralWindowId"] = &DelayModelRow::spectralWindowIdFromBin; 
		 fromBinMethods["timeInterval"] = &DelayModelRow::timeIntervalFromBin; 
		 fromBinMethods["numPoly"] = &DelayModelRow::numPolyFromBin; 
		 fromBinMethods["phaseDelay"] = &DelayModelRow::phaseDelayFromBin; 
		 fromBinMethods["phaseDelayRate"] = &DelayModelRow::phaseDelayRateFromBin; 
		 fromBinMethods["groupDelay"] = &DelayModelRow::groupDelayFromBin; 
		 fromBinMethods["groupDelayRate"] = &DelayModelRow::groupDelayRateFromBin; 
		 fromBinMethods["fieldId"] = &DelayModelRow::fieldIdFromBin; 
			
	
		 fromBinMethods["timeOrigin"] = &DelayModelRow::timeOriginFromBin; 
		 fromBinMethods["atmosphericGroupDelay"] = &DelayModelRow::atmosphericGroupDelayFromBin; 
		 fromBinMethods["atmosphericGroupDelayRate"] = &DelayModelRow::atmosphericGroupDelayRateFromBin; 
		 fromBinMethods["geometricDelay"] = &DelayModelRow::geometricDelayFromBin; 
		 fromBinMethods["geometricDelayRate"] = &DelayModelRow::geometricDelayRateFromBin; 
		 fromBinMethods["numLO"] = &DelayModelRow::numLOFromBin; 
		 fromBinMethods["LOOffset"] = &DelayModelRow::LOOffsetFromBin; 
		 fromBinMethods["LOOffsetRate"] = &DelayModelRow::LOOffsetRateFromBin; 
		 fromBinMethods["dispersiveDelay"] = &DelayModelRow::dispersiveDelayFromBin; 
		 fromBinMethods["dispersiveDelayRate"] = &DelayModelRow::dispersiveDelayRateFromBin; 
		 fromBinMethods["atmosphericDryDelay"] = &DelayModelRow::atmosphericDryDelayFromBin; 
		 fromBinMethods["atmosphericWetDelay"] = &DelayModelRow::atmosphericWetDelayFromBin; 
		 fromBinMethods["padDelay"] = &DelayModelRow::padDelayFromBin; 
		 fromBinMethods["antennaDelay"] = &DelayModelRow::antennaDelayFromBin; 
		 fromBinMethods["numReceptor"] = &DelayModelRow::numReceptorFromBin; 
		 fromBinMethods["polarizationType"] = &DelayModelRow::polarizationTypeFromBin; 
		 fromBinMethods["electronicDelay"] = &DelayModelRow::electronicDelayFromBin; 
		 fromBinMethods["electronicDelayRate"] = &DelayModelRow::electronicDelayRateFromBin; 
		 fromBinMethods["receiverDelay"] = &DelayModelRow::receiverDelayFromBin; 
		 fromBinMethods["IFDelay"] = &DelayModelRow::IFDelayFromBin; 
		 fromBinMethods["LODelay"] = &DelayModelRow::LODelayFromBin; 
		 fromBinMethods["crossPolarizationDelay"] = &DelayModelRow::crossPolarizationDelayFromBin; 
			
	}

	
	bool DelayModelRow::compareNoAutoInc(Tag antennaId, Tag spectralWindowId, ArrayTimeInterval timeInterval, int numPoly, std::vector<double > phaseDelay, std::vector<double > phaseDelayRate, std::vector<double > groupDelay, std::vector<double > groupDelayRate, Tag fieldId) {
		bool result;
		result = true;
		
	
		
		result = result && (this->antennaId == antennaId);
		
		if (!result) return false;
	

	
		
		result = result && (this->spectralWindowId == spectralWindowId);
		
		if (!result) return false;
	

	
		
		result = result && (this->timeInterval.overlaps(timeInterval));
		
		if (!result) return false;
	

	
		
		result = result && (this->numPoly == numPoly);
		
		if (!result) return false;
	

	
		
		result = result && (this->phaseDelay == phaseDelay);
		
		if (!result) return false;
	

	
		
		result = result && (this->phaseDelayRate == phaseDelayRate);
		
		if (!result) return false;
	

	
		
		result = result && (this->groupDelay == groupDelay);
		
		if (!result) return false;
	

	
		
		result = result && (this->groupDelayRate == groupDelayRate);
		
		if (!result) return false;
	

	
		
		result = result && (this->fieldId == fieldId);
		
		if (!result) return false;
	

		return result;
	}	
	
	
	
	bool DelayModelRow::compareRequiredValue(int numPoly, std::vector<double > phaseDelay, std::vector<double > phaseDelayRate, std::vector<double > groupDelay, std::vector<double > groupDelayRate, Tag fieldId) {
		bool result;
		result = true;
		
	
		if (!(this->numPoly == numPoly)) return false;
	

	
		if (!(this->phaseDelay == phaseDelay)) return false;
	

	
		if (!(this->phaseDelayRate == phaseDelayRate)) return false;
	

	
		if (!(this->groupDelay == groupDelay)) return false;
	

	
		if (!(this->groupDelayRate == groupDelayRate)) return false;
	

	
		if (!(this->fieldId == fieldId)) return false;
	

		return result;
	}
	
	
	/**
	 * Return true if all required attributes of the value part are equal to their homologues
	 * in x and false otherwise.
	 *

	 * @param x a pointer on the DelayModelRow whose required attributes of the value part 

	 * will be compared with those of this.
	 * @return a boolean.
	 */
	bool DelayModelRow::equalByRequiredValue(DelayModelRow*  x ) {
		
			
		if (this->numPoly != x->numPoly) return false;
			
		if (this->phaseDelay != x->phaseDelay) return false;
			
		if (this->phaseDelayRate != x->phaseDelayRate) return false;
			
		if (this->groupDelay != x->groupDelay) return false;
			
		if (this->groupDelayRate != x->groupDelayRate) return false;
			
		if (this->fieldId != x->fieldId) return false;
			
		
		return true;
	}	
	
/*
	 map<string, DelayModelAttributeFromBin> DelayModelRow::initFromBinMethods() {
		map<string, DelayModelAttributeFromBin> result;
		
		result["antennaId"] = &DelayModelRow::antennaIdFromBin;
		result["spectralWindowId"] = &DelayModelRow::spectralWindowIdFromBin;
		result["timeInterval"] = &DelayModelRow::timeIntervalFromBin;
		result["numPoly"] = &DelayModelRow::numPolyFromBin;
		result["phaseDelay"] = &DelayModelRow::phaseDelayFromBin;
		result["phaseDelayRate"] = &DelayModelRow::phaseDelayRateFromBin;
		result["groupDelay"] = &DelayModelRow::groupDelayFromBin;
		result["groupDelayRate"] = &DelayModelRow::groupDelayRateFromBin;
		result["fieldId"] = &DelayModelRow::fieldIdFromBin;
		
		
		result["timeOrigin"] = &DelayModelRow::timeOriginFromBin;
		result["atmosphericGroupDelay"] = &DelayModelRow::atmosphericGroupDelayFromBin;
		result["atmosphericGroupDelayRate"] = &DelayModelRow::atmosphericGroupDelayRateFromBin;
		result["geometricDelay"] = &DelayModelRow::geometricDelayFromBin;
		result["geometricDelayRate"] = &DelayModelRow::geometricDelayRateFromBin;
		result["numLO"] = &DelayModelRow::numLOFromBin;
		result["LOOffset"] = &DelayModelRow::LOOffsetFromBin;
		result["LOOffsetRate"] = &DelayModelRow::LOOffsetRateFromBin;
		result["dispersiveDelay"] = &DelayModelRow::dispersiveDelayFromBin;
		result["dispersiveDelayRate"] = &DelayModelRow::dispersiveDelayRateFromBin;
		result["atmosphericDryDelay"] = &DelayModelRow::atmosphericDryDelayFromBin;
		result["atmosphericWetDelay"] = &DelayModelRow::atmosphericWetDelayFromBin;
		result["padDelay"] = &DelayModelRow::padDelayFromBin;
		result["antennaDelay"] = &DelayModelRow::antennaDelayFromBin;
		result["numReceptor"] = &DelayModelRow::numReceptorFromBin;
		result["polarizationType"] = &DelayModelRow::polarizationTypeFromBin;
		result["electronicDelay"] = &DelayModelRow::electronicDelayFromBin;
		result["electronicDelayRate"] = &DelayModelRow::electronicDelayRateFromBin;
		result["receiverDelay"] = &DelayModelRow::receiverDelayFromBin;
		result["IFDelay"] = &DelayModelRow::IFDelayFromBin;
		result["LODelay"] = &DelayModelRow::LODelayFromBin;
		result["crossPolarizationDelay"] = &DelayModelRow::crossPolarizationDelayFromBin;
			
		
		return result;	
	}
*/	
} // End namespace asdm
 
