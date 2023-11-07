
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
 * File CalPhaseRow.cpp
 */
 
#include <vector>
#include <set>

#include <alma/ASDM/ASDM.h>
#include <alma/ASDM/CalPhaseRow.h>
#include <alma/ASDM/CalPhaseTable.h>

#include <alma/ASDM/CalDataTable.h>
#include <alma/ASDM/CalDataRow.h>

#include <alma/ASDM/CalReductionTable.h>
#include <alma/ASDM/CalReductionRow.h>
	

using asdm::ASDM;
using asdm::CalPhaseRow;
using asdm::CalPhaseTable;

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
	CalPhaseRow::~CalPhaseRow() {
	}

	/**
	 * Return the table to which this row belongs.
	 */
	CalPhaseTable &CalPhaseRow::getTable() const {
		return table;
	}

	bool CalPhaseRow::isAdded() const {
		return hasBeenAdded;
	}	

	void CalPhaseRow::isAdded(bool added) {
		hasBeenAdded = added;
	}
	
#ifndef WITHOUT_ACS
	using asdmIDL::CalPhaseRowIDL;
#endif
	
#ifndef WITHOUT_ACS
	/**
	 * Return this row in the form of an IDL struct.
	 * @return The values of this row as a CalPhaseRowIDL struct.
	 */
	CalPhaseRowIDL *CalPhaseRow::toIDL() const {
		CalPhaseRowIDL *x = new CalPhaseRowIDL ();
		
		// Fill the IDL structure.
	
		
	
  		
		
		
			
				
		x->basebandName = basebandName;
 				
 			
		
	

	
  		
		
		
			
				
		x->receiverBand = receiverBand;
 				
 			
		
	

	
  		
		
		
			
				
		x->atmPhaseCorrection = atmPhaseCorrection;
 				
 			
		
	

	
  		
		
		
			
		x->startValidTime = startValidTime.toIDLArrayTime();
			
		
	

	
  		
		
		
			
		x->endValidTime = endValidTime.toIDLArrayTime();
			
		
	

	
  		
		
		
			
				
		x->numBaseline = numBaseline;
 				
 			
		
	

	
  		
		
		
			
				
		x->numReceptor = numReceptor;
 				
 			
		
	

	
  		
		
		
			
		x->ampli.length(ampli.size());
		for (unsigned int i = 0; i < ampli.size(); i++) {
			x->ampli[i].length(ampli.at(i).size());			 		
		}
		
		for (unsigned int i = 0; i < ampli.size() ; i++)
			for (unsigned int j = 0; j < ampli.at(i).size(); j++)
					
						
				x->ampli[i][j] = ampli.at(i).at(j);
		 				
			 						
		
			
		
	

	
  		
		
		
			
		x->antennaNames.length(antennaNames.size());
		for (unsigned int i = 0; i < antennaNames.size(); i++) {
			x->antennaNames[i].length(antennaNames.at(i).size());			 		
		}
		
		for (unsigned int i = 0; i < antennaNames.size() ; i++)
			for (unsigned int j = 0; j < antennaNames.at(i).size(); j++)
					
						
				x->antennaNames[i][j] = CORBA::string_dup(antennaNames.at(i).at(j).c_str());
						
			 						
		
			
		
	

	
  		
		
		
			
		x->baselineLengths.length(baselineLengths.size());
		for (unsigned int i = 0; i < baselineLengths.size(); ++i) {
			
			x->baselineLengths[i] = baselineLengths.at(i).toIDLLength();
			
	 	}
			
		
	

	
  		
		
		
			
		x->decorrelationFactor.length(decorrelationFactor.size());
		for (unsigned int i = 0; i < decorrelationFactor.size(); i++) {
			x->decorrelationFactor[i].length(decorrelationFactor.at(i).size());			 		
		}
		
		for (unsigned int i = 0; i < decorrelationFactor.size() ; i++)
			for (unsigned int j = 0; j < decorrelationFactor.at(i).size(); j++)
					
						
				x->decorrelationFactor[i][j] = decorrelationFactor.at(i).at(j);
		 				
			 						
		
			
		
	

	
  		
		
		
			
		x->direction.length(direction.size());
		for (unsigned int i = 0; i < direction.size(); ++i) {
			
			x->direction[i] = direction.at(i).toIDLAngle();
			
	 	}
			
		
	

	
  		
		
		
			
		x->frequencyRange.length(frequencyRange.size());
		for (unsigned int i = 0; i < frequencyRange.size(); ++i) {
			
			x->frequencyRange[i] = frequencyRange.at(i).toIDLFrequency();
			
	 	}
			
		
	

	
  		
		
		
			
		x->integrationTime = integrationTime.toIDLInterval();
			
		
	

	
  		
		
		
			
		x->phase.length(phase.size());
		for (unsigned int i = 0; i < phase.size(); i++) {
			x->phase[i].length(phase.at(i).size());			 		
		}
		
		for (unsigned int i = 0; i < phase.size() ; i++)
			for (unsigned int j = 0; j < phase.at(i).size(); j++)
					
						
				x->phase[i][j] = phase.at(i).at(j);
		 				
			 						
		
			
		
	

	
  		
		
		
			
		x->polarizationTypes.length(polarizationTypes.size());
		for (unsigned int i = 0; i < polarizationTypes.size(); ++i) {
			
				
			x->polarizationTypes[i] = polarizationTypes.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		
			
		x->phaseRMS.length(phaseRMS.size());
		for (unsigned int i = 0; i < phaseRMS.size(); i++) {
			x->phaseRMS[i].length(phaseRMS.at(i).size());			 		
		}
		
		for (unsigned int i = 0; i < phaseRMS.size() ; i++)
			for (unsigned int j = 0; j < phaseRMS.at(i).size(); j++)
					
						
				x->phaseRMS[i][j] = phaseRMS.at(i).at(j);
		 				
			 						
		
			
		
	

	
  		
		
		
			
		x->statPhaseRMS.length(statPhaseRMS.size());
		for (unsigned int i = 0; i < statPhaseRMS.size(); i++) {
			x->statPhaseRMS[i].length(statPhaseRMS.at(i).size());			 		
		}
		
		for (unsigned int i = 0; i < statPhaseRMS.size() ; i++)
			for (unsigned int j = 0; j < statPhaseRMS.at(i).size(); j++)
					
						
				x->statPhaseRMS[i][j] = statPhaseRMS.at(i).at(j);
		 				
			 						
		
			
		
	

	
  		
		
		x->correctionValidityExists = correctionValidityExists;
		
		
			
		x->correctionValidity.length(correctionValidity.size());
		for (unsigned int i = 0; i < correctionValidity.size(); ++i) {
			
				
			x->correctionValidity[i] = correctionValidity.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		x->numAntennaExists = numAntennaExists;
		
		
			
				
		x->numAntenna = numAntenna;
 				
 			
		
	

	
  		
		
		x->singleAntennaNameExists = singleAntennaNameExists;
		
		
			
		x->singleAntennaName.length(singleAntennaName.size());
		for (unsigned int i = 0; i < singleAntennaName.size(); ++i) {
			
				
			x->singleAntennaName[i] = CORBA::string_dup(singleAntennaName.at(i).c_str());
				
	 		
	 	}
			
		
	

	
  		
		
		x->refAntennaNameExists = refAntennaNameExists;
		
		
			
				
		x->refAntennaName = CORBA::string_dup(refAntennaName.c_str());
				
 			
		
	

	
  		
		
		x->phaseAntExists = phaseAntExists;
		
		
			
		x->phaseAnt.length(phaseAnt.size());
		for (unsigned int i = 0; i < phaseAnt.size(); i++) {
			x->phaseAnt[i].length(phaseAnt.at(i).size());			 		
		}
		
		for (unsigned int i = 0; i < phaseAnt.size() ; i++)
			for (unsigned int j = 0; j < phaseAnt.at(i).size(); j++)
					
						
				x->phaseAnt[i][j] = phaseAnt.at(i).at(j);
		 				
			 						
		
			
		
	

	
  		
		
		x->phaseAntRMSExists = phaseAntRMSExists;
		
		
			
		x->phaseAntRMS.length(phaseAntRMS.size());
		for (unsigned int i = 0; i < phaseAntRMS.size(); i++) {
			x->phaseAntRMS[i].length(phaseAntRMS.at(i).size());			 		
		}
		
		for (unsigned int i = 0; i < phaseAntRMS.size() ; i++)
			for (unsigned int j = 0; j < phaseAntRMS.at(i).size(); j++)
					
						
				x->phaseAntRMS[i][j] = phaseAntRMS.at(i).at(j);
		 				
			 						
		
			
		
	

	
	
		
	
  	
 		
		
	 	
			
		x->calDataId = calDataId.toIDLTag();
			
	 	 		
  	

	
  	
 		
		
	 	
			
		x->calReductionId = calReductionId.toIDLTag();
			
	 	 		
  	

	
		
	

	

		
		return x;
	
	}
	
	void CalPhaseRow::toIDL(asdmIDL::CalPhaseRowIDL& x) const {
		// Set the x's fields.
	
		
	
  		
		
		
			
				
		x.basebandName = basebandName;
 				
 			
		
	

	
  		
		
		
			
				
		x.receiverBand = receiverBand;
 				
 			
		
	

	
  		
		
		
			
				
		x.atmPhaseCorrection = atmPhaseCorrection;
 				
 			
		
	

	
  		
		
		
			
		x.startValidTime = startValidTime.toIDLArrayTime();
			
		
	

	
  		
		
		
			
		x.endValidTime = endValidTime.toIDLArrayTime();
			
		
	

	
  		
		
		
			
				
		x.numBaseline = numBaseline;
 				
 			
		
	

	
  		
		
		
			
				
		x.numReceptor = numReceptor;
 				
 			
		
	

	
  		
		
		
			
		x.ampli.length(ampli.size());
		for (unsigned int i = 0; i < ampli.size(); i++) {
			x.ampli[i].length(ampli.at(i).size());			 		
		}
		
		for (unsigned int i = 0; i < ampli.size() ; i++)
			for (unsigned int j = 0; j < ampli.at(i).size(); j++)
					
						
				x.ampli[i][j] = ampli.at(i).at(j);
		 				
			 						
		
			
		
	

	
  		
		
		
			
		x.antennaNames.length(antennaNames.size());
		for (unsigned int i = 0; i < antennaNames.size(); i++) {
			x.antennaNames[i].length(antennaNames.at(i).size());			 		
		}
		
		for (unsigned int i = 0; i < antennaNames.size() ; i++)
			for (unsigned int j = 0; j < antennaNames.at(i).size(); j++)
					
						
				x.antennaNames[i][j] = CORBA::string_dup(antennaNames.at(i).at(j).c_str());
						
			 						
		
			
		
	

	
  		
		
		
			
		x.baselineLengths.length(baselineLengths.size());
		for (unsigned int i = 0; i < baselineLengths.size(); ++i) {
			
			x.baselineLengths[i] = baselineLengths.at(i).toIDLLength();
			
	 	}
			
		
	

	
  		
		
		
			
		x.decorrelationFactor.length(decorrelationFactor.size());
		for (unsigned int i = 0; i < decorrelationFactor.size(); i++) {
			x.decorrelationFactor[i].length(decorrelationFactor.at(i).size());			 		
		}
		
		for (unsigned int i = 0; i < decorrelationFactor.size() ; i++)
			for (unsigned int j = 0; j < decorrelationFactor.at(i).size(); j++)
					
						
				x.decorrelationFactor[i][j] = decorrelationFactor.at(i).at(j);
		 				
			 						
		
			
		
	

	
  		
		
		
			
		x.direction.length(direction.size());
		for (unsigned int i = 0; i < direction.size(); ++i) {
			
			x.direction[i] = direction.at(i).toIDLAngle();
			
	 	}
			
		
	

	
  		
		
		
			
		x.frequencyRange.length(frequencyRange.size());
		for (unsigned int i = 0; i < frequencyRange.size(); ++i) {
			
			x.frequencyRange[i] = frequencyRange.at(i).toIDLFrequency();
			
	 	}
			
		
	

	
  		
		
		
			
		x.integrationTime = integrationTime.toIDLInterval();
			
		
	

	
  		
		
		
			
		x.phase.length(phase.size());
		for (unsigned int i = 0; i < phase.size(); i++) {
			x.phase[i].length(phase.at(i).size());			 		
		}
		
		for (unsigned int i = 0; i < phase.size() ; i++)
			for (unsigned int j = 0; j < phase.at(i).size(); j++)
					
						
				x.phase[i][j] = phase.at(i).at(j);
		 				
			 						
		
			
		
	

	
  		
		
		
			
		x.polarizationTypes.length(polarizationTypes.size());
		for (unsigned int i = 0; i < polarizationTypes.size(); ++i) {
			
				
			x.polarizationTypes[i] = polarizationTypes.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		
			
		x.phaseRMS.length(phaseRMS.size());
		for (unsigned int i = 0; i < phaseRMS.size(); i++) {
			x.phaseRMS[i].length(phaseRMS.at(i).size());			 		
		}
		
		for (unsigned int i = 0; i < phaseRMS.size() ; i++)
			for (unsigned int j = 0; j < phaseRMS.at(i).size(); j++)
					
						
				x.phaseRMS[i][j] = phaseRMS.at(i).at(j);
		 				
			 						
		
			
		
	

	
  		
		
		
			
		x.statPhaseRMS.length(statPhaseRMS.size());
		for (unsigned int i = 0; i < statPhaseRMS.size(); i++) {
			x.statPhaseRMS[i].length(statPhaseRMS.at(i).size());			 		
		}
		
		for (unsigned int i = 0; i < statPhaseRMS.size() ; i++)
			for (unsigned int j = 0; j < statPhaseRMS.at(i).size(); j++)
					
						
				x.statPhaseRMS[i][j] = statPhaseRMS.at(i).at(j);
		 				
			 						
		
			
		
	

	
  		
		
		x.correctionValidityExists = correctionValidityExists;
		
		
			
		x.correctionValidity.length(correctionValidity.size());
		for (unsigned int i = 0; i < correctionValidity.size(); ++i) {
			
				
			x.correctionValidity[i] = correctionValidity.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		x.numAntennaExists = numAntennaExists;
		
		
			
				
		x.numAntenna = numAntenna;
 				
 			
		
	

	
  		
		
		x.singleAntennaNameExists = singleAntennaNameExists;
		
		
			
		x.singleAntennaName.length(singleAntennaName.size());
		for (unsigned int i = 0; i < singleAntennaName.size(); ++i) {
			
				
			x.singleAntennaName[i] = CORBA::string_dup(singleAntennaName.at(i).c_str());
				
	 		
	 	}
			
		
	

	
  		
		
		x.refAntennaNameExists = refAntennaNameExists;
		
		
			
				
		x.refAntennaName = CORBA::string_dup(refAntennaName.c_str());
				
 			
		
	

	
  		
		
		x.phaseAntExists = phaseAntExists;
		
		
			
		x.phaseAnt.length(phaseAnt.size());
		for (unsigned int i = 0; i < phaseAnt.size(); i++) {
			x.phaseAnt[i].length(phaseAnt.at(i).size());			 		
		}
		
		for (unsigned int i = 0; i < phaseAnt.size() ; i++)
			for (unsigned int j = 0; j < phaseAnt.at(i).size(); j++)
					
						
				x.phaseAnt[i][j] = phaseAnt.at(i).at(j);
		 				
			 						
		
			
		
	

	
  		
		
		x.phaseAntRMSExists = phaseAntRMSExists;
		
		
			
		x.phaseAntRMS.length(phaseAntRMS.size());
		for (unsigned int i = 0; i < phaseAntRMS.size(); i++) {
			x.phaseAntRMS[i].length(phaseAntRMS.at(i).size());			 		
		}
		
		for (unsigned int i = 0; i < phaseAntRMS.size() ; i++)
			for (unsigned int j = 0; j < phaseAntRMS.at(i).size(); j++)
					
						
				x.phaseAntRMS[i][j] = phaseAntRMS.at(i).at(j);
		 				
			 						
		
			
		
	

	
	
		
	
  	
 		
		
	 	
			
		x.calDataId = calDataId.toIDLTag();
			
	 	 		
  	

	
  	
 		
		
	 	
			
		x.calReductionId = calReductionId.toIDLTag();
			
	 	 		
  	

	
		
	

	

	
	}
#endif
	

#ifndef WITHOUT_ACS
	/**
	 * Fill the values of this row from the IDL struct CalPhaseRowIDL.
	 * @param x The IDL struct containing the values used to fill this row.
	 */
	void CalPhaseRow::setFromIDL (CalPhaseRowIDL x){
		try {
		// Fill the values from x.
	
		
	
		
		
			
		setBasebandName(x.basebandName);
  			
 		
		
	

	
		
		
			
		setReceiverBand(x.receiverBand);
  			
 		
		
	

	
		
		
			
		setAtmPhaseCorrection(x.atmPhaseCorrection);
  			
 		
		
	

	
		
		
			
		setStartValidTime(ArrayTime (x.startValidTime));
			
 		
		
	

	
		
		
			
		setEndValidTime(ArrayTime (x.endValidTime));
			
 		
		
	

	
		
		
			
		setNumBaseline(x.numBaseline);
  			
 		
		
	

	
		
		
			
		setNumReceptor(x.numReceptor);
  			
 		
		
	

	
		
		
			
		ampli .clear();
        
        vector<float> v_aux_ampli;
        
		for (unsigned int i = 0; i < x.ampli.length(); ++i) {
			v_aux_ampli.clear();
			for (unsigned int j = 0; j < x.ampli[0].length(); ++j) {
				
				v_aux_ampli.push_back(x.ampli[i][j]);
	  			
  			}
  			ampli.push_back(v_aux_ampli);			
		}
			
  		
		
	

	
		
		
			
		antennaNames .clear();
        
        vector<string> v_aux_antennaNames;
        
		for (unsigned int i = 0; i < x.antennaNames.length(); ++i) {
			v_aux_antennaNames.clear();
			for (unsigned int j = 0; j < x.antennaNames[0].length(); ++j) {
				
				v_aux_antennaNames.push_back(string (x.antennaNames[i][j]));
				
  			}
  			antennaNames.push_back(v_aux_antennaNames);			
		}
			
  		
		
	

	
		
		
			
		baselineLengths .clear();
		for (unsigned int i = 0; i <x.baselineLengths.length(); ++i) {
			
			baselineLengths.push_back(Length (x.baselineLengths[i]));
			
		}
			
  		
		
	

	
		
		
			
		decorrelationFactor .clear();
        
        vector<float> v_aux_decorrelationFactor;
        
		for (unsigned int i = 0; i < x.decorrelationFactor.length(); ++i) {
			v_aux_decorrelationFactor.clear();
			for (unsigned int j = 0; j < x.decorrelationFactor[0].length(); ++j) {
				
				v_aux_decorrelationFactor.push_back(x.decorrelationFactor[i][j]);
	  			
  			}
  			decorrelationFactor.push_back(v_aux_decorrelationFactor);			
		}
			
  		
		
	

	
		
		
			
		direction .clear();
		for (unsigned int i = 0; i <x.direction.length(); ++i) {
			
			direction.push_back(Angle (x.direction[i]));
			
		}
			
  		
		
	

	
		
		
			
		frequencyRange .clear();
		for (unsigned int i = 0; i <x.frequencyRange.length(); ++i) {
			
			frequencyRange.push_back(Frequency (x.frequencyRange[i]));
			
		}
			
  		
		
	

	
		
		
			
		setIntegrationTime(Interval (x.integrationTime));
			
 		
		
	

	
		
		
			
		phase .clear();
        
        vector<float> v_aux_phase;
        
		for (unsigned int i = 0; i < x.phase.length(); ++i) {
			v_aux_phase.clear();
			for (unsigned int j = 0; j < x.phase[0].length(); ++j) {
				
				v_aux_phase.push_back(x.phase[i][j]);
	  			
  			}
  			phase.push_back(v_aux_phase);			
		}
			
  		
		
	

	
		
		
			
		polarizationTypes .clear();
		for (unsigned int i = 0; i <x.polarizationTypes.length(); ++i) {
			
			polarizationTypes.push_back(x.polarizationTypes[i]);
  			
		}
			
  		
		
	

	
		
		
			
		phaseRMS .clear();
        
        vector<float> v_aux_phaseRMS;
        
		for (unsigned int i = 0; i < x.phaseRMS.length(); ++i) {
			v_aux_phaseRMS.clear();
			for (unsigned int j = 0; j < x.phaseRMS[0].length(); ++j) {
				
				v_aux_phaseRMS.push_back(x.phaseRMS[i][j]);
	  			
  			}
  			phaseRMS.push_back(v_aux_phaseRMS);			
		}
			
  		
		
	

	
		
		
			
		statPhaseRMS .clear();
        
        vector<float> v_aux_statPhaseRMS;
        
		for (unsigned int i = 0; i < x.statPhaseRMS.length(); ++i) {
			v_aux_statPhaseRMS.clear();
			for (unsigned int j = 0; j < x.statPhaseRMS[0].length(); ++j) {
				
				v_aux_statPhaseRMS.push_back(x.statPhaseRMS[i][j]);
	  			
  			}
  			statPhaseRMS.push_back(v_aux_statPhaseRMS);			
		}
			
  		
		
	

	
		
		correctionValidityExists = x.correctionValidityExists;
		if (x.correctionValidityExists) {
		
		
			
		correctionValidity .clear();
		for (unsigned int i = 0; i <x.correctionValidity.length(); ++i) {
			
			correctionValidity.push_back(x.correctionValidity[i]);
  			
		}
			
  		
		
		}
		
	

	
		
		numAntennaExists = x.numAntennaExists;
		if (x.numAntennaExists) {
		
		
			
		setNumAntenna(x.numAntenna);
  			
 		
		
		}
		
	

	
		
		singleAntennaNameExists = x.singleAntennaNameExists;
		if (x.singleAntennaNameExists) {
		
		
			
		singleAntennaName .clear();
		for (unsigned int i = 0; i <x.singleAntennaName.length(); ++i) {
			
			singleAntennaName.push_back(string (x.singleAntennaName[i]));
			
		}
			
  		
		
		}
		
	

	
		
		refAntennaNameExists = x.refAntennaNameExists;
		if (x.refAntennaNameExists) {
		
		
			
		setRefAntennaName(string (x.refAntennaName));
			
 		
		
		}
		
	

	
		
		phaseAntExists = x.phaseAntExists;
		if (x.phaseAntExists) {
		
		
			
		phaseAnt .clear();
        
        vector<float> v_aux_phaseAnt;
        
		for (unsigned int i = 0; i < x.phaseAnt.length(); ++i) {
			v_aux_phaseAnt.clear();
			for (unsigned int j = 0; j < x.phaseAnt[0].length(); ++j) {
				
				v_aux_phaseAnt.push_back(x.phaseAnt[i][j]);
	  			
  			}
  			phaseAnt.push_back(v_aux_phaseAnt);			
		}
			
  		
		
		}
		
	

	
		
		phaseAntRMSExists = x.phaseAntRMSExists;
		if (x.phaseAntRMSExists) {
		
		
			
		phaseAntRMS .clear();
        
        vector<float> v_aux_phaseAntRMS;
        
		for (unsigned int i = 0; i < x.phaseAntRMS.length(); ++i) {
			v_aux_phaseAntRMS.clear();
			for (unsigned int j = 0; j < x.phaseAntRMS[0].length(); ++j) {
				
				v_aux_phaseAntRMS.push_back(x.phaseAntRMS[i][j]);
	  			
  			}
  			phaseAntRMS.push_back(v_aux_phaseAntRMS);			
		}
			
  		
		
		}
		
	

	
	
		
	
		
		
			
		setCalDataId(Tag (x.calDataId));
			
 		
		
	

	
		
		
			
		setCalReductionId(Tag (x.calReductionId));
			
 		
		
	

	
		
	

	

		} catch (const IllegalAccessException &err) {
			throw ConversionException (err.getMessage(),"CalPhase");
		}
	}
#endif
	
	/**
	 * Return this row in the form of an XML string.
	 * @return The values of this row as an XML string.
	 */
	string CalPhaseRow::toXML() const {
		string buf;
		buf.append("<row> \n");
		
	
		
  	
 		
		
			buf.append(EnumerationParser::toXML("basebandName", basebandName));
		
		
	

  	
 		
		
			buf.append(EnumerationParser::toXML("receiverBand", receiverBand));
		
		
	

  	
 		
		
			buf.append(EnumerationParser::toXML("atmPhaseCorrection", atmPhaseCorrection));
		
		
	

  	
 		
		
		Parser::toXML(startValidTime, "startValidTime", buf);
		
		
	

  	
 		
		
		Parser::toXML(endValidTime, "endValidTime", buf);
		
		
	

  	
 		
		
		Parser::toXML(numBaseline, "numBaseline", buf);
		
		
	

  	
 		
		
		Parser::toXML(numReceptor, "numReceptor", buf);
		
		
	

  	
 		
		
		Parser::toXML(ampli, "ampli", buf);
		
		
	

  	
 		
		
		Parser::toXML(antennaNames, "antennaNames", buf);
		
		
	

  	
 		
		
		Parser::toXML(baselineLengths, "baselineLengths", buf);
		
		
	

  	
 		
		
		Parser::toXML(decorrelationFactor, "decorrelationFactor", buf);
		
		
	

  	
 		
		
		Parser::toXML(direction, "direction", buf);
		
		
	

  	
 		
		
		Parser::toXML(frequencyRange, "frequencyRange", buf);
		
		
	

  	
 		
		
		Parser::toXML(integrationTime, "integrationTime", buf);
		
		
	

  	
 		
		
		Parser::toXML(phase, "phase", buf);
		
		
	

  	
 		
		
			buf.append(EnumerationParser::toXML("polarizationTypes", polarizationTypes));
		
		
	

  	
 		
		
		Parser::toXML(phaseRMS, "phaseRMS", buf);
		
		
	

  	
 		
		
		Parser::toXML(statPhaseRMS, "statPhaseRMS", buf);
		
		
	

  	
 		
		if (correctionValidityExists) {
		
		
		Parser::toXML(correctionValidity, "correctionValidity", buf);
		
		
		}
		
	

  	
 		
		if (numAntennaExists) {
		
		
		Parser::toXML(numAntenna, "numAntenna", buf);
		
		
		}
		
	

  	
 		
		if (singleAntennaNameExists) {
		
		
		Parser::toXML(singleAntennaName, "singleAntennaName", buf);
		
		
		}
		
	

  	
 		
		if (refAntennaNameExists) {
		
		
		Parser::toXML(refAntennaName, "refAntennaName", buf);
		
		
		}
		
	

  	
 		
		if (phaseAntExists) {
		
		
		Parser::toXML(phaseAnt, "phaseAnt", buf);
		
		
		}
		
	

  	
 		
		if (phaseAntRMSExists) {
		
		
		Parser::toXML(phaseAntRMS, "phaseAntRMS", buf);
		
		
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
	void CalPhaseRow::setFromXML (string rowDoc) {
		Parser row(rowDoc);
		string s = "";
		try {
	
		
	
		
		
		
		basebandName = EnumerationParser::getBasebandName("basebandName","CalPhase",rowDoc);
		
		
		
	

	
		
		
		
		receiverBand = EnumerationParser::getReceiverBand("receiverBand","CalPhase",rowDoc);
		
		
		
	

	
		
		
		
		atmPhaseCorrection = EnumerationParser::getAtmPhaseCorrection("atmPhaseCorrection","CalPhase",rowDoc);
		
		
		
	

	
  		
			
	  	setStartValidTime(Parser::getArrayTime("startValidTime","CalPhase",rowDoc));
			
		
	

	
  		
			
	  	setEndValidTime(Parser::getArrayTime("endValidTime","CalPhase",rowDoc));
			
		
	

	
  		
			
	  	setNumBaseline(Parser::getInteger("numBaseline","CalPhase",rowDoc));
			
		
	

	
  		
			
	  	setNumReceptor(Parser::getInteger("numReceptor","CalPhase",rowDoc));
			
		
	

	
  		
			
					
	  	setAmpli(Parser::get2DFloat("ampli","CalPhase",rowDoc));
	  			
	  		
		
	

	
  		
			
					
	  	setAntennaNames(Parser::get2DString("antennaNames","CalPhase",rowDoc));
	  			
	  		
		
	

	
  		
			
					
	  	setBaselineLengths(Parser::get1DLength("baselineLengths","CalPhase",rowDoc));
	  			
	  		
		
	

	
  		
			
					
	  	setDecorrelationFactor(Parser::get2DFloat("decorrelationFactor","CalPhase",rowDoc));
	  			
	  		
		
	

	
  		
			
					
	  	setDirection(Parser::get1DAngle("direction","CalPhase",rowDoc));
	  			
	  		
		
	

	
  		
			
					
	  	setFrequencyRange(Parser::get1DFrequency("frequencyRange","CalPhase",rowDoc));
	  			
	  		
		
	

	
  		
			
	  	setIntegrationTime(Parser::getInterval("integrationTime","CalPhase",rowDoc));
			
		
	

	
  		
			
					
	  	setPhase(Parser::get2DFloat("phase","CalPhase",rowDoc));
	  			
	  		
		
	

	
		
		
		
		polarizationTypes = EnumerationParser::getPolarizationType1D("polarizationTypes","CalPhase",rowDoc);			
		
		
		
	

	
  		
			
					
	  	setPhaseRMS(Parser::get2DFloat("phaseRMS","CalPhase",rowDoc));
	  			
	  		
		
	

	
  		
			
					
	  	setStatPhaseRMS(Parser::get2DFloat("statPhaseRMS","CalPhase",rowDoc));
	  			
	  		
		
	

	
  		
        if (row.isStr("<correctionValidity>")) {
			
								
	  		setCorrectionValidity(Parser::get1DBoolean("correctionValidity","CalPhase",rowDoc));
	  			
	  		
		}
 		
	

	
  		
        if (row.isStr("<numAntenna>")) {
			
	  		setNumAntenna(Parser::getInteger("numAntenna","CalPhase",rowDoc));
			
		}
 		
	

	
  		
        if (row.isStr("<singleAntennaName>")) {
			
								
	  		setSingleAntennaName(Parser::get1DString("singleAntennaName","CalPhase",rowDoc));
	  			
	  		
		}
 		
	

	
  		
        if (row.isStr("<refAntennaName>")) {
			
	  		setRefAntennaName(Parser::getString("refAntennaName","CalPhase",rowDoc));
			
		}
 		
	

	
  		
        if (row.isStr("<phaseAnt>")) {
			
								
	  		setPhaseAnt(Parser::get2DFloat("phaseAnt","CalPhase",rowDoc));
	  			
	  		
		}
 		
	

	
  		
        if (row.isStr("<phaseAntRMS>")) {
			
								
	  		setPhaseAntRMS(Parser::get2DFloat("phaseAntRMS","CalPhase",rowDoc));
	  			
	  		
		}
 		
	

	
	
		
	
  		
			
	  	setCalDataId(Parser::getTag("calDataId","CalData",rowDoc));
			
		
	

	
  		
			
	  	setCalReductionId(Parser::getTag("calReductionId","CalReduction",rowDoc));
			
		
	

	
		
	

	

		} catch (const IllegalAccessException &err) {
			throw ConversionException (err.getMessage(),"CalPhase");
		}
	}
	
	void CalPhaseRow::toBin(EndianOSStream& eoss) {
	
	
	
	
		
					
		eoss.writeString(CBasebandName::name(basebandName));
			/* eoss.writeInt(basebandName); */
				
		
	

	
	
		
					
		eoss.writeString(CReceiverBand::name(receiverBand));
			/* eoss.writeInt(receiverBand); */
				
		
	

	
	
		
					
		eoss.writeString(CAtmPhaseCorrection::name(atmPhaseCorrection));
			/* eoss.writeInt(atmPhaseCorrection); */
				
		
	

	
	
		
	calDataId.toBin(eoss);
		
	

	
	
		
	calReductionId.toBin(eoss);
		
	

	
	
		
	startValidTime.toBin(eoss);
		
	

	
	
		
	endValidTime.toBin(eoss);
		
	

	
	
		
						
			eoss.writeInt(numBaseline);
				
		
	

	
	
		
						
			eoss.writeInt(numReceptor);
				
		
	

	
	
		
		
			
		eoss.writeInt((int) ampli.size());
		eoss.writeInt((int) ampli.at(0).size());
		for (unsigned int i = 0; i < ampli.size(); i++) 
			for (unsigned int j = 0;  j < ampli.at(0).size(); j++) 
							 
				eoss.writeFloat(ampli.at(i).at(j));
				
	
						
		
	

	
	
		
		
			
		eoss.writeInt((int) antennaNames.size());
		eoss.writeInt((int) antennaNames.at(0).size());
		for (unsigned int i = 0; i < antennaNames.size(); i++) 
			for (unsigned int j = 0;  j < antennaNames.at(0).size(); j++) 
							 
				eoss.writeString(antennaNames.at(i).at(j));
				
	
						
		
	

	
	
		
	Length::toBin(baselineLengths, eoss);
		
	

	
	
		
		
			
		eoss.writeInt((int) decorrelationFactor.size());
		eoss.writeInt((int) decorrelationFactor.at(0).size());
		for (unsigned int i = 0; i < decorrelationFactor.size(); i++) 
			for (unsigned int j = 0;  j < decorrelationFactor.at(0).size(); j++) 
							 
				eoss.writeFloat(decorrelationFactor.at(i).at(j));
				
	
						
		
	

	
	
		
	Angle::toBin(direction, eoss);
		
	

	
	
		
	Frequency::toBin(frequencyRange, eoss);
		
	

	
	
		
	integrationTime.toBin(eoss);
		
	

	
	
		
		
			
		eoss.writeInt((int) phase.size());
		eoss.writeInt((int) phase.at(0).size());
		for (unsigned int i = 0; i < phase.size(); i++) 
			for (unsigned int j = 0;  j < phase.at(0).size(); j++) 
							 
				eoss.writeFloat(phase.at(i).at(j));
				
	
						
		
	

	
	
		
		
			
		eoss.writeInt((int) polarizationTypes.size());
		for (unsigned int i = 0; i < polarizationTypes.size(); i++)
				
			eoss.writeString(CPolarizationType::name(polarizationTypes.at(i)));
			/* eoss.writeInt(polarizationTypes.at(i)); */
				
				
						
		
	

	
	
		
		
			
		eoss.writeInt((int) phaseRMS.size());
		eoss.writeInt((int) phaseRMS.at(0).size());
		for (unsigned int i = 0; i < phaseRMS.size(); i++) 
			for (unsigned int j = 0;  j < phaseRMS.at(0).size(); j++) 
							 
				eoss.writeFloat(phaseRMS.at(i).at(j));
				
	
						
		
	

	
	
		
		
			
		eoss.writeInt((int) statPhaseRMS.size());
		eoss.writeInt((int) statPhaseRMS.at(0).size());
		for (unsigned int i = 0; i < statPhaseRMS.size(); i++) 
			for (unsigned int j = 0;  j < statPhaseRMS.at(0).size(); j++) 
							 
				eoss.writeFloat(statPhaseRMS.at(i).at(j));
				
	
						
		
	


	
	
	eoss.writeBoolean(correctionValidityExists);
	if (correctionValidityExists) {
	
	
	
		
		
			
		eoss.writeInt((int) correctionValidity.size());
		for (unsigned int i = 0; i < correctionValidity.size(); i++)
				
			eoss.writeBoolean(correctionValidity.at(i));
				
				
						
		
	

	}

	eoss.writeBoolean(numAntennaExists);
	if (numAntennaExists) {
	
	
	
		
						
			eoss.writeInt(numAntenna);
				
		
	

	}

	eoss.writeBoolean(singleAntennaNameExists);
	if (singleAntennaNameExists) {
	
	
	
		
		
			
		eoss.writeInt((int) singleAntennaName.size());
		for (unsigned int i = 0; i < singleAntennaName.size(); i++)
				
			eoss.writeString(singleAntennaName.at(i));
				
				
						
		
	

	}

	eoss.writeBoolean(refAntennaNameExists);
	if (refAntennaNameExists) {
	
	
	
		
						
			eoss.writeString(refAntennaName);
				
		
	

	}

	eoss.writeBoolean(phaseAntExists);
	if (phaseAntExists) {
	
	
	
		
		
			
		eoss.writeInt((int) phaseAnt.size());
		eoss.writeInt((int) phaseAnt.at(0).size());
		for (unsigned int i = 0; i < phaseAnt.size(); i++) 
			for (unsigned int j = 0;  j < phaseAnt.at(0).size(); j++) 
							 
				eoss.writeFloat(phaseAnt.at(i).at(j));
				
	
						
		
	

	}

	eoss.writeBoolean(phaseAntRMSExists);
	if (phaseAntRMSExists) {
	
	
	
		
		
			
		eoss.writeInt((int) phaseAntRMS.size());
		eoss.writeInt((int) phaseAntRMS.at(0).size());
		for (unsigned int i = 0; i < phaseAntRMS.size(); i++) 
			for (unsigned int j = 0;  j < phaseAntRMS.at(0).size(); j++) 
							 
				eoss.writeFloat(phaseAntRMS.at(i).at(j));
				
	
						
		
	

	}

	}
	
void CalPhaseRow::basebandNameFromBin(EndianIStream& eis) {
		
	
	
		
			
		basebandName = CBasebandName::literal(eis.readString());
			
		
	
	
}
void CalPhaseRow::receiverBandFromBin(EndianIStream& eis) {
		
	
	
		
			
		receiverBand = CReceiverBand::literal(eis.readString());
			
		
	
	
}
void CalPhaseRow::atmPhaseCorrectionFromBin(EndianIStream& eis) {
		
	
	
		
			
		atmPhaseCorrection = CAtmPhaseCorrection::literal(eis.readString());
			
		
	
	
}
void CalPhaseRow::calDataIdFromBin(EndianIStream& eis) {
		
	
		
		
		calDataId =  Tag::fromBin(eis);
		
	
	
}
void CalPhaseRow::calReductionIdFromBin(EndianIStream& eis) {
		
	
		
		
		calReductionId =  Tag::fromBin(eis);
		
	
	
}
void CalPhaseRow::startValidTimeFromBin(EndianIStream& eis) {
		
	
		
		
		startValidTime =  ArrayTime::fromBin(eis);
		
	
	
}
void CalPhaseRow::endValidTimeFromBin(EndianIStream& eis) {
		
	
		
		
		endValidTime =  ArrayTime::fromBin(eis);
		
	
	
}
void CalPhaseRow::numBaselineFromBin(EndianIStream& eis) {
		
	
	
		
			
		numBaseline =  eis.readInt();
			
		
	
	
}
void CalPhaseRow::numReceptorFromBin(EndianIStream& eis) {
		
	
	
		
			
		numReceptor =  eis.readInt();
			
		
	
	
}
void CalPhaseRow::ampliFromBin(EndianIStream& eis) {
		
	
	
		
			
	
		ampli.clear();
		
		unsigned int ampliDim1 = eis.readInt();
		unsigned int ampliDim2 = eis.readInt();
        
		vector <float> ampliAux1;
        
		for (unsigned int i = 0; i < ampliDim1; i++) {
			ampliAux1.clear();
			for (unsigned int j = 0; j < ampliDim2 ; j++)			
			
			ampliAux1.push_back(eis.readFloat());
			
			ampli.push_back(ampliAux1);
		}
	
	

		
	
	
}
void CalPhaseRow::antennaNamesFromBin(EndianIStream& eis) {
		
	
	
		
			
	
		antennaNames.clear();
		
		unsigned int antennaNamesDim1 = eis.readInt();
		unsigned int antennaNamesDim2 = eis.readInt();
        
		vector <string> antennaNamesAux1;
        
		for (unsigned int i = 0; i < antennaNamesDim1; i++) {
			antennaNamesAux1.clear();
			for (unsigned int j = 0; j < antennaNamesDim2 ; j++)			
			
			antennaNamesAux1.push_back(eis.readString());
			
			antennaNames.push_back(antennaNamesAux1);
		}
	
	

		
	
	
}
void CalPhaseRow::baselineLengthsFromBin(EndianIStream& eis) {
		
	
		
		
			
	
	baselineLengths = Length::from1DBin(eis);	
	

		
	
	
}
void CalPhaseRow::decorrelationFactorFromBin(EndianIStream& eis) {
		
	
	
		
			
	
		decorrelationFactor.clear();
		
		unsigned int decorrelationFactorDim1 = eis.readInt();
		unsigned int decorrelationFactorDim2 = eis.readInt();
        
		vector <float> decorrelationFactorAux1;
        
		for (unsigned int i = 0; i < decorrelationFactorDim1; i++) {
			decorrelationFactorAux1.clear();
			for (unsigned int j = 0; j < decorrelationFactorDim2 ; j++)			
			
			decorrelationFactorAux1.push_back(eis.readFloat());
			
			decorrelationFactor.push_back(decorrelationFactorAux1);
		}
	
	

		
	
	
}
void CalPhaseRow::directionFromBin(EndianIStream& eis) {
		
	
		
		
			
	
	direction = Angle::from1DBin(eis);	
	

		
	
	
}
void CalPhaseRow::frequencyRangeFromBin(EndianIStream& eis) {
		
	
		
		
			
	
	frequencyRange = Frequency::from1DBin(eis);	
	

		
	
	
}
void CalPhaseRow::integrationTimeFromBin(EndianIStream& eis) {
		
	
		
		
		integrationTime =  Interval::fromBin(eis);
		
	
	
}
void CalPhaseRow::phaseFromBin(EndianIStream& eis) {
		
	
	
		
			
	
		phase.clear();
		
		unsigned int phaseDim1 = eis.readInt();
		unsigned int phaseDim2 = eis.readInt();
        
		vector <float> phaseAux1;
        
		for (unsigned int i = 0; i < phaseDim1; i++) {
			phaseAux1.clear();
			for (unsigned int j = 0; j < phaseDim2 ; j++)			
			
			phaseAux1.push_back(eis.readFloat());
			
			phase.push_back(phaseAux1);
		}
	
	

		
	
	
}
void CalPhaseRow::polarizationTypesFromBin(EndianIStream& eis) {
		
	
	
		
			
	
		polarizationTypes.clear();
		
		unsigned int polarizationTypesDim1 = eis.readInt();
		for (unsigned int  i = 0 ; i < polarizationTypesDim1; i++)
			
			polarizationTypes.push_back(CPolarizationType::literal(eis.readString()));
			
	

		
	
	
}
void CalPhaseRow::phaseRMSFromBin(EndianIStream& eis) {
		
	
	
		
			
	
		phaseRMS.clear();
		
		unsigned int phaseRMSDim1 = eis.readInt();
		unsigned int phaseRMSDim2 = eis.readInt();
        
		vector <float> phaseRMSAux1;
        
		for (unsigned int i = 0; i < phaseRMSDim1; i++) {
			phaseRMSAux1.clear();
			for (unsigned int j = 0; j < phaseRMSDim2 ; j++)			
			
			phaseRMSAux1.push_back(eis.readFloat());
			
			phaseRMS.push_back(phaseRMSAux1);
		}
	
	

		
	
	
}
void CalPhaseRow::statPhaseRMSFromBin(EndianIStream& eis) {
		
	
	
		
			
	
		statPhaseRMS.clear();
		
		unsigned int statPhaseRMSDim1 = eis.readInt();
		unsigned int statPhaseRMSDim2 = eis.readInt();
        
		vector <float> statPhaseRMSAux1;
        
		for (unsigned int i = 0; i < statPhaseRMSDim1; i++) {
			statPhaseRMSAux1.clear();
			for (unsigned int j = 0; j < statPhaseRMSDim2 ; j++)			
			
			statPhaseRMSAux1.push_back(eis.readFloat());
			
			statPhaseRMS.push_back(statPhaseRMSAux1);
		}
	
	

		
	
	
}

void CalPhaseRow::correctionValidityFromBin(EndianIStream& eis) {
		
	correctionValidityExists = eis.readBoolean();
	if (correctionValidityExists) {
		
	
	
		
			
	
		correctionValidity.clear();
		
		unsigned int correctionValidityDim1 = eis.readInt();
		for (unsigned int  i = 0 ; i < correctionValidityDim1; i++)
			
			correctionValidity.push_back(eis.readBoolean());
			
	

		
	

	}
	
}
void CalPhaseRow::numAntennaFromBin(EndianIStream& eis) {
		
	numAntennaExists = eis.readBoolean();
	if (numAntennaExists) {
		
	
	
		
			
		numAntenna =  eis.readInt();
			
		
	

	}
	
}
void CalPhaseRow::singleAntennaNameFromBin(EndianIStream& eis) {
		
	singleAntennaNameExists = eis.readBoolean();
	if (singleAntennaNameExists) {
		
	
	
		
			
	
		singleAntennaName.clear();
		
		unsigned int singleAntennaNameDim1 = eis.readInt();
		for (unsigned int  i = 0 ; i < singleAntennaNameDim1; i++)
			
			singleAntennaName.push_back(eis.readString());
			
	

		
	

	}
	
}
void CalPhaseRow::refAntennaNameFromBin(EndianIStream& eis) {
		
	refAntennaNameExists = eis.readBoolean();
	if (refAntennaNameExists) {
		
	
	
		
			
		refAntennaName =  eis.readString();
			
		
	

	}
	
}
void CalPhaseRow::phaseAntFromBin(EndianIStream& eis) {
		
	phaseAntExists = eis.readBoolean();
	if (phaseAntExists) {
		
	
	
		
			
	
		phaseAnt.clear();
		
		unsigned int phaseAntDim1 = eis.readInt();
		unsigned int phaseAntDim2 = eis.readInt();
        
		vector <float> phaseAntAux1;
        
		for (unsigned int i = 0; i < phaseAntDim1; i++) {
			phaseAntAux1.clear();
			for (unsigned int j = 0; j < phaseAntDim2 ; j++)			
			
			phaseAntAux1.push_back(eis.readFloat());
			
			phaseAnt.push_back(phaseAntAux1);
		}
	
	

		
	

	}
	
}
void CalPhaseRow::phaseAntRMSFromBin(EndianIStream& eis) {
		
	phaseAntRMSExists = eis.readBoolean();
	if (phaseAntRMSExists) {
		
	
	
		
			
	
		phaseAntRMS.clear();
		
		unsigned int phaseAntRMSDim1 = eis.readInt();
		unsigned int phaseAntRMSDim2 = eis.readInt();
        
		vector <float> phaseAntRMSAux1;
        
		for (unsigned int i = 0; i < phaseAntRMSDim1; i++) {
			phaseAntRMSAux1.clear();
			for (unsigned int j = 0; j < phaseAntRMSDim2 ; j++)			
			
			phaseAntRMSAux1.push_back(eis.readFloat());
			
			phaseAntRMS.push_back(phaseAntRMSAux1);
		}
	
	

		
	

	}
	
}
	
	
	CalPhaseRow* CalPhaseRow::fromBin(EndianIStream& eis, CalPhaseTable& table, const vector<string>& attributesSeq) {
		CalPhaseRow* row = new  CalPhaseRow(table);
		
		map<string, CalPhaseAttributeFromBin>::iterator iter ;
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
					throw ConversionException("There is not method to read an attribute '"+attributesSeq.at(i)+"'.", "CalPhaseTable");
			}
				
		}				
		return row;
	}

	//
	// A collection of methods to set the value of the attributes from their textual value in the XML representation
	// of one row.
	//
	
	// Convert a string into an BasebandName 
	void CalPhaseRow::basebandNameFromText(const string & s) {
		 
          
		basebandName = ASDMValuesParser::parse<BasebandNameMod::BasebandName>(s);
          
		
	}
	
	
	// Convert a string into an ReceiverBand 
	void CalPhaseRow::receiverBandFromText(const string & s) {
		 
          
		receiverBand = ASDMValuesParser::parse<ReceiverBandMod::ReceiverBand>(s);
          
		
	}
	
	
	// Convert a string into an AtmPhaseCorrection 
	void CalPhaseRow::atmPhaseCorrectionFromText(const string & s) {
		 
          
		atmPhaseCorrection = ASDMValuesParser::parse<AtmPhaseCorrectionMod::AtmPhaseCorrection>(s);
          
		
	}
	
	
	// Convert a string into an Tag 
	void CalPhaseRow::calDataIdFromText(const string & s) {
		 
          
		calDataId = ASDMValuesParser::parse<Tag>(s);
          
		
	}
	
	
	// Convert a string into an Tag 
	void CalPhaseRow::calReductionIdFromText(const string & s) {
		 
          
		calReductionId = ASDMValuesParser::parse<Tag>(s);
          
		
	}
	
	
	// Convert a string into an ArrayTime 
	void CalPhaseRow::startValidTimeFromText(const string & s) {
		 
          
		startValidTime = ASDMValuesParser::parse<ArrayTime>(s);
          
		
	}
	
	
	// Convert a string into an ArrayTime 
	void CalPhaseRow::endValidTimeFromText(const string & s) {
		 
          
		endValidTime = ASDMValuesParser::parse<ArrayTime>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void CalPhaseRow::numBaselineFromText(const string & s) {
		 
          
		numBaseline = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void CalPhaseRow::numReceptorFromText(const string & s) {
		 
          
		numReceptor = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	// Convert a string into an float 
	void CalPhaseRow::ampliFromText(const string & s) {
		 
          
		ampli = ASDMValuesParser::parse2D<float>(s);
          
		
	}
	
	
	// Convert a string into an String 
	void CalPhaseRow::antennaNamesFromText(const string & s) {
		 
          
		antennaNames = ASDMValuesParser::parse2D<string>(s);
          
		
	}
	
	
	// Convert a string into an Length 
	void CalPhaseRow::baselineLengthsFromText(const string & s) {
		 
          
		baselineLengths = ASDMValuesParser::parse1D<Length>(s);
          
		
	}
	
	
	// Convert a string into an float 
	void CalPhaseRow::decorrelationFactorFromText(const string & s) {
		 
          
		decorrelationFactor = ASDMValuesParser::parse2D<float>(s);
          
		
	}
	
	
	// Convert a string into an Angle 
	void CalPhaseRow::directionFromText(const string & s) {
		 
          
		direction = ASDMValuesParser::parse1D<Angle>(s);
          
		
	}
	
	
	// Convert a string into an Frequency 
	void CalPhaseRow::frequencyRangeFromText(const string & s) {
		 
          
		frequencyRange = ASDMValuesParser::parse1D<Frequency>(s);
          
		
	}
	
	
	// Convert a string into an Interval 
	void CalPhaseRow::integrationTimeFromText(const string & s) {
		 
          
		integrationTime = ASDMValuesParser::parse<Interval>(s);
          
		
	}
	
	
	// Convert a string into an float 
	void CalPhaseRow::phaseFromText(const string & s) {
		 
          
		phase = ASDMValuesParser::parse2D<float>(s);
          
		
	}
	
	
	// Convert a string into an PolarizationType 
	void CalPhaseRow::polarizationTypesFromText(const string & s) {
		 
          
		polarizationTypes = ASDMValuesParser::parse1D<PolarizationTypeMod::PolarizationType>(s);
          
		
	}
	
	
	// Convert a string into an float 
	void CalPhaseRow::phaseRMSFromText(const string & s) {
		 
          
		phaseRMS = ASDMValuesParser::parse2D<float>(s);
          
		
	}
	
	
	// Convert a string into an float 
	void CalPhaseRow::statPhaseRMSFromText(const string & s) {
		 
          
		statPhaseRMS = ASDMValuesParser::parse2D<float>(s);
          
		
	}
	

	
	// Convert a string into an boolean 
	void CalPhaseRow::correctionValidityFromText(const string & s) {
		correctionValidityExists = true;
		 
          
		correctionValidity = ASDMValuesParser::parse1D<bool>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void CalPhaseRow::numAntennaFromText(const string & s) {
		numAntennaExists = true;
		 
          
		numAntenna = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	// Convert a string into an String 
	void CalPhaseRow::singleAntennaNameFromText(const string & s) {
		singleAntennaNameExists = true;
		 
          
		singleAntennaName = ASDMValuesParser::parse1D<string>(s);
          
		
	}
	
	
	// Convert a string into an String 
	void CalPhaseRow::refAntennaNameFromText(const string & s) {
		refAntennaNameExists = true;
		 
          
		refAntennaName = ASDMValuesParser::parse<string>(s);
          
		
	}
	
	
	// Convert a string into an float 
	void CalPhaseRow::phaseAntFromText(const string & s) {
		phaseAntExists = true;
		 
          
		phaseAnt = ASDMValuesParser::parse2D<float>(s);
          
		
	}
	
	
	// Convert a string into an float 
	void CalPhaseRow::phaseAntRMSFromText(const string & s) {
		phaseAntRMSExists = true;
		 
          
		phaseAntRMS = ASDMValuesParser::parse2D<float>(s);
          
		
	}
	
	
	
	void CalPhaseRow::fromText(const std::string& attributeName, const std::string&  t) {
		map<string, CalPhaseAttributeFromText>::iterator iter;
		if ((iter = fromTextMethods.find(attributeName)) == fromTextMethods.end())
			throw ConversionException("I do not know what to do with '"+attributeName+"' and its content '"+t+"' (while parsing an XML document)", "CalPhaseTable");
		(this->*(iter->second))(t);
	}
			
	////////////////////////////////////////////////
	// Intrinsic Table Attributes getters/setters //
	////////////////////////////////////////////////
	
	

	
 	/**
 	 * Get basebandName.
 	 * @return basebandName as BasebandNameMod::BasebandName
 	 */
 	BasebandNameMod::BasebandName CalPhaseRow::getBasebandName() const {
	
  		return basebandName;
 	}

 	/**
 	 * Set basebandName with the specified BasebandNameMod::BasebandName.
 	 * @param basebandName The BasebandNameMod::BasebandName value to which basebandName is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void CalPhaseRow::setBasebandName (BasebandNameMod::BasebandName basebandName)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("basebandName", "CalPhase");
		
  		}
  	
 		this->basebandName = basebandName;
	
 	}
	
	

	

	
 	/**
 	 * Get receiverBand.
 	 * @return receiverBand as ReceiverBandMod::ReceiverBand
 	 */
 	ReceiverBandMod::ReceiverBand CalPhaseRow::getReceiverBand() const {
	
  		return receiverBand;
 	}

 	/**
 	 * Set receiverBand with the specified ReceiverBandMod::ReceiverBand.
 	 * @param receiverBand The ReceiverBandMod::ReceiverBand value to which receiverBand is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void CalPhaseRow::setReceiverBand (ReceiverBandMod::ReceiverBand receiverBand)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("receiverBand", "CalPhase");
		
  		}
  	
 		this->receiverBand = receiverBand;
	
 	}
	
	

	

	
 	/**
 	 * Get atmPhaseCorrection.
 	 * @return atmPhaseCorrection as AtmPhaseCorrectionMod::AtmPhaseCorrection
 	 */
 	AtmPhaseCorrectionMod::AtmPhaseCorrection CalPhaseRow::getAtmPhaseCorrection() const {
	
  		return atmPhaseCorrection;
 	}

 	/**
 	 * Set atmPhaseCorrection with the specified AtmPhaseCorrectionMod::AtmPhaseCorrection.
 	 * @param atmPhaseCorrection The AtmPhaseCorrectionMod::AtmPhaseCorrection value to which atmPhaseCorrection is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void CalPhaseRow::setAtmPhaseCorrection (AtmPhaseCorrectionMod::AtmPhaseCorrection atmPhaseCorrection)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("atmPhaseCorrection", "CalPhase");
		
  		}
  	
 		this->atmPhaseCorrection = atmPhaseCorrection;
	
 	}
	
	

	

	
 	/**
 	 * Get startValidTime.
 	 * @return startValidTime as ArrayTime
 	 */
 	ArrayTime CalPhaseRow::getStartValidTime() const {
	
  		return startValidTime;
 	}

 	/**
 	 * Set startValidTime with the specified ArrayTime.
 	 * @param startValidTime The ArrayTime value to which startValidTime is to be set.
 	 
 	
 		
 	 */
 	void CalPhaseRow::setStartValidTime (ArrayTime startValidTime)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->startValidTime = startValidTime;
	
 	}
	
	

	

	
 	/**
 	 * Get endValidTime.
 	 * @return endValidTime as ArrayTime
 	 */
 	ArrayTime CalPhaseRow::getEndValidTime() const {
	
  		return endValidTime;
 	}

 	/**
 	 * Set endValidTime with the specified ArrayTime.
 	 * @param endValidTime The ArrayTime value to which endValidTime is to be set.
 	 
 	
 		
 	 */
 	void CalPhaseRow::setEndValidTime (ArrayTime endValidTime)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->endValidTime = endValidTime;
	
 	}
	
	

	

	
 	/**
 	 * Get numBaseline.
 	 * @return numBaseline as int
 	 */
 	int CalPhaseRow::getNumBaseline() const {
	
  		return numBaseline;
 	}

 	/**
 	 * Set numBaseline with the specified int.
 	 * @param numBaseline The int value to which numBaseline is to be set.
 	 
 	
 		
 	 */
 	void CalPhaseRow::setNumBaseline (int numBaseline)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->numBaseline = numBaseline;
	
 	}
	
	

	

	
 	/**
 	 * Get numReceptor.
 	 * @return numReceptor as int
 	 */
 	int CalPhaseRow::getNumReceptor() const {
	
  		return numReceptor;
 	}

 	/**
 	 * Set numReceptor with the specified int.
 	 * @param numReceptor The int value to which numReceptor is to be set.
 	 
 	
 		
 	 */
 	void CalPhaseRow::setNumReceptor (int numReceptor)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->numReceptor = numReceptor;
	
 	}
	
	

	

	
 	/**
 	 * Get ampli.
 	 * @return ampli as std::vector<std::vector<float > >
 	 */
 	std::vector<std::vector<float > > CalPhaseRow::getAmpli() const {
	
  		return ampli;
 	}

 	/**
 	 * Set ampli with the specified std::vector<std::vector<float > >.
 	 * @param ampli The std::vector<std::vector<float > > value to which ampli is to be set.
 	 
 	
 		
 	 */
 	void CalPhaseRow::setAmpli (std::vector<std::vector<float > > ampli)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->ampli = ampli;
	
 	}
	
	

	

	
 	/**
 	 * Get antennaNames.
 	 * @return antennaNames as std::vector<std::vector<std::string > >
 	 */
 	std::vector<std::vector<std::string > > CalPhaseRow::getAntennaNames() const {
	
  		return antennaNames;
 	}

 	/**
 	 * Set antennaNames with the specified std::vector<std::vector<std::string > >.
 	 * @param antennaNames The std::vector<std::vector<std::string > > value to which antennaNames is to be set.
 	 
 	
 		
 	 */
 	void CalPhaseRow::setAntennaNames (std::vector<std::vector<std::string > > antennaNames)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->antennaNames = antennaNames;
	
 	}
	
	

	

	
 	/**
 	 * Get baselineLengths.
 	 * @return baselineLengths as std::vector<Length >
 	 */
 	std::vector<Length > CalPhaseRow::getBaselineLengths() const {
	
  		return baselineLengths;
 	}

 	/**
 	 * Set baselineLengths with the specified std::vector<Length >.
 	 * @param baselineLengths The std::vector<Length > value to which baselineLengths is to be set.
 	 
 	
 		
 	 */
 	void CalPhaseRow::setBaselineLengths (std::vector<Length > baselineLengths)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->baselineLengths = baselineLengths;
	
 	}
	
	

	

	
 	/**
 	 * Get decorrelationFactor.
 	 * @return decorrelationFactor as std::vector<std::vector<float > >
 	 */
 	std::vector<std::vector<float > > CalPhaseRow::getDecorrelationFactor() const {
	
  		return decorrelationFactor;
 	}

 	/**
 	 * Set decorrelationFactor with the specified std::vector<std::vector<float > >.
 	 * @param decorrelationFactor The std::vector<std::vector<float > > value to which decorrelationFactor is to be set.
 	 
 	
 		
 	 */
 	void CalPhaseRow::setDecorrelationFactor (std::vector<std::vector<float > > decorrelationFactor)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->decorrelationFactor = decorrelationFactor;
	
 	}
	
	

	

	
 	/**
 	 * Get direction.
 	 * @return direction as std::vector<Angle >
 	 */
 	std::vector<Angle > CalPhaseRow::getDirection() const {
	
  		return direction;
 	}

 	/**
 	 * Set direction with the specified std::vector<Angle >.
 	 * @param direction The std::vector<Angle > value to which direction is to be set.
 	 
 	
 		
 	 */
 	void CalPhaseRow::setDirection (std::vector<Angle > direction)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->direction = direction;
	
 	}
	
	

	

	
 	/**
 	 * Get frequencyRange.
 	 * @return frequencyRange as std::vector<Frequency >
 	 */
 	std::vector<Frequency > CalPhaseRow::getFrequencyRange() const {
	
  		return frequencyRange;
 	}

 	/**
 	 * Set frequencyRange with the specified std::vector<Frequency >.
 	 * @param frequencyRange The std::vector<Frequency > value to which frequencyRange is to be set.
 	 
 	
 		
 	 */
 	void CalPhaseRow::setFrequencyRange (std::vector<Frequency > frequencyRange)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->frequencyRange = frequencyRange;
	
 	}
	
	

	

	
 	/**
 	 * Get integrationTime.
 	 * @return integrationTime as Interval
 	 */
 	Interval CalPhaseRow::getIntegrationTime() const {
	
  		return integrationTime;
 	}

 	/**
 	 * Set integrationTime with the specified Interval.
 	 * @param integrationTime The Interval value to which integrationTime is to be set.
 	 
 	
 		
 	 */
 	void CalPhaseRow::setIntegrationTime (Interval integrationTime)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->integrationTime = integrationTime;
	
 	}
	
	

	

	
 	/**
 	 * Get phase.
 	 * @return phase as std::vector<std::vector<float > >
 	 */
 	std::vector<std::vector<float > > CalPhaseRow::getPhase() const {
	
  		return phase;
 	}

 	/**
 	 * Set phase with the specified std::vector<std::vector<float > >.
 	 * @param phase The std::vector<std::vector<float > > value to which phase is to be set.
 	 
 	
 		
 	 */
 	void CalPhaseRow::setPhase (std::vector<std::vector<float > > phase)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->phase = phase;
	
 	}
	
	

	

	
 	/**
 	 * Get polarizationTypes.
 	 * @return polarizationTypes as std::vector<PolarizationTypeMod::PolarizationType >
 	 */
 	std::vector<PolarizationTypeMod::PolarizationType > CalPhaseRow::getPolarizationTypes() const {
	
  		return polarizationTypes;
 	}

 	/**
 	 * Set polarizationTypes with the specified std::vector<PolarizationTypeMod::PolarizationType >.
 	 * @param polarizationTypes The std::vector<PolarizationTypeMod::PolarizationType > value to which polarizationTypes is to be set.
 	 
 	
 		
 	 */
 	void CalPhaseRow::setPolarizationTypes (std::vector<PolarizationTypeMod::PolarizationType > polarizationTypes)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->polarizationTypes = polarizationTypes;
	
 	}
	
	

	

	
 	/**
 	 * Get phaseRMS.
 	 * @return phaseRMS as std::vector<std::vector<float > >
 	 */
 	std::vector<std::vector<float > > CalPhaseRow::getPhaseRMS() const {
	
  		return phaseRMS;
 	}

 	/**
 	 * Set phaseRMS with the specified std::vector<std::vector<float > >.
 	 * @param phaseRMS The std::vector<std::vector<float > > value to which phaseRMS is to be set.
 	 
 	
 		
 	 */
 	void CalPhaseRow::setPhaseRMS (std::vector<std::vector<float > > phaseRMS)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->phaseRMS = phaseRMS;
	
 	}
	
	

	

	
 	/**
 	 * Get statPhaseRMS.
 	 * @return statPhaseRMS as std::vector<std::vector<float > >
 	 */
 	std::vector<std::vector<float > > CalPhaseRow::getStatPhaseRMS() const {
	
  		return statPhaseRMS;
 	}

 	/**
 	 * Set statPhaseRMS with the specified std::vector<std::vector<float > >.
 	 * @param statPhaseRMS The std::vector<std::vector<float > > value to which statPhaseRMS is to be set.
 	 
 	
 		
 	 */
 	void CalPhaseRow::setStatPhaseRMS (std::vector<std::vector<float > > statPhaseRMS)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->statPhaseRMS = statPhaseRMS;
	
 	}
	
	

	
	/**
	 * The attribute correctionValidity is optional. Return true if this attribute exists.
	 * @return true if and only if the correctionValidity attribute exists. 
	 */
	bool CalPhaseRow::isCorrectionValidityExists() const {
		return correctionValidityExists;
	}
	

	
 	/**
 	 * Get correctionValidity, which is optional.
 	 * @return correctionValidity as std::vector<bool >
 	 * @throw IllegalAccessException If correctionValidity does not exist.
 	 */
 	std::vector<bool > CalPhaseRow::getCorrectionValidity() const  {
		if (!correctionValidityExists) {
			throw IllegalAccessException("correctionValidity", "CalPhase");
		}
	
  		return correctionValidity;
 	}

 	/**
 	 * Set correctionValidity with the specified std::vector<bool >.
 	 * @param correctionValidity The std::vector<bool > value to which correctionValidity is to be set.
 	 
 	
 	 */
 	void CalPhaseRow::setCorrectionValidity (std::vector<bool > correctionValidity) {
	
 		this->correctionValidity = correctionValidity;
	
		correctionValidityExists = true;
	
 	}
	
	
	/**
	 * Mark correctionValidity, which is an optional field, as non-existent.
	 */
	void CalPhaseRow::clearCorrectionValidity () {
		correctionValidityExists = false;
	}
	

	
	/**
	 * The attribute numAntenna is optional. Return true if this attribute exists.
	 * @return true if and only if the numAntenna attribute exists. 
	 */
	bool CalPhaseRow::isNumAntennaExists() const {
		return numAntennaExists;
	}
	

	
 	/**
 	 * Get numAntenna, which is optional.
 	 * @return numAntenna as int
 	 * @throw IllegalAccessException If numAntenna does not exist.
 	 */
 	int CalPhaseRow::getNumAntenna() const  {
		if (!numAntennaExists) {
			throw IllegalAccessException("numAntenna", "CalPhase");
		}
	
  		return numAntenna;
 	}

 	/**
 	 * Set numAntenna with the specified int.
 	 * @param numAntenna The int value to which numAntenna is to be set.
 	 
 	
 	 */
 	void CalPhaseRow::setNumAntenna (int numAntenna) {
	
 		this->numAntenna = numAntenna;
	
		numAntennaExists = true;
	
 	}
	
	
	/**
	 * Mark numAntenna, which is an optional field, as non-existent.
	 */
	void CalPhaseRow::clearNumAntenna () {
		numAntennaExists = false;
	}
	

	
	/**
	 * The attribute singleAntennaName is optional. Return true if this attribute exists.
	 * @return true if and only if the singleAntennaName attribute exists. 
	 */
	bool CalPhaseRow::isSingleAntennaNameExists() const {
		return singleAntennaNameExists;
	}
	

	
 	/**
 	 * Get singleAntennaName, which is optional.
 	 * @return singleAntennaName as std::vector<std::string >
 	 * @throw IllegalAccessException If singleAntennaName does not exist.
 	 */
 	std::vector<std::string > CalPhaseRow::getSingleAntennaName() const  {
		if (!singleAntennaNameExists) {
			throw IllegalAccessException("singleAntennaName", "CalPhase");
		}
	
  		return singleAntennaName;
 	}

 	/**
 	 * Set singleAntennaName with the specified std::vector<std::string >.
 	 * @param singleAntennaName The std::vector<std::string > value to which singleAntennaName is to be set.
 	 
 	
 	 */
 	void CalPhaseRow::setSingleAntennaName (std::vector<std::string > singleAntennaName) {
	
 		this->singleAntennaName = singleAntennaName;
	
		singleAntennaNameExists = true;
	
 	}
	
	
	/**
	 * Mark singleAntennaName, which is an optional field, as non-existent.
	 */
	void CalPhaseRow::clearSingleAntennaName () {
		singleAntennaNameExists = false;
	}
	

	
	/**
	 * The attribute refAntennaName is optional. Return true if this attribute exists.
	 * @return true if and only if the refAntennaName attribute exists. 
	 */
	bool CalPhaseRow::isRefAntennaNameExists() const {
		return refAntennaNameExists;
	}
	

	
 	/**
 	 * Get refAntennaName, which is optional.
 	 * @return refAntennaName as std::string
 	 * @throw IllegalAccessException If refAntennaName does not exist.
 	 */
 	std::string CalPhaseRow::getRefAntennaName() const  {
		if (!refAntennaNameExists) {
			throw IllegalAccessException("refAntennaName", "CalPhase");
		}
	
  		return refAntennaName;
 	}

 	/**
 	 * Set refAntennaName with the specified std::string.
 	 * @param refAntennaName The std::string value to which refAntennaName is to be set.
 	 
 	
 	 */
 	void CalPhaseRow::setRefAntennaName (std::string refAntennaName) {
	
 		this->refAntennaName = refAntennaName;
	
		refAntennaNameExists = true;
	
 	}
	
	
	/**
	 * Mark refAntennaName, which is an optional field, as non-existent.
	 */
	void CalPhaseRow::clearRefAntennaName () {
		refAntennaNameExists = false;
	}
	

	
	/**
	 * The attribute phaseAnt is optional. Return true if this attribute exists.
	 * @return true if and only if the phaseAnt attribute exists. 
	 */
	bool CalPhaseRow::isPhaseAntExists() const {
		return phaseAntExists;
	}
	

	
 	/**
 	 * Get phaseAnt, which is optional.
 	 * @return phaseAnt as std::vector<std::vector<float > >
 	 * @throw IllegalAccessException If phaseAnt does not exist.
 	 */
 	std::vector<std::vector<float > > CalPhaseRow::getPhaseAnt() const  {
		if (!phaseAntExists) {
			throw IllegalAccessException("phaseAnt", "CalPhase");
		}
	
  		return phaseAnt;
 	}

 	/**
 	 * Set phaseAnt with the specified std::vector<std::vector<float > >.
 	 * @param phaseAnt The std::vector<std::vector<float > > value to which phaseAnt is to be set.
 	 
 	
 	 */
 	void CalPhaseRow::setPhaseAnt (std::vector<std::vector<float > > phaseAnt) {
	
 		this->phaseAnt = phaseAnt;
	
		phaseAntExists = true;
	
 	}
	
	
	/**
	 * Mark phaseAnt, which is an optional field, as non-existent.
	 */
	void CalPhaseRow::clearPhaseAnt () {
		phaseAntExists = false;
	}
	

	
	/**
	 * The attribute phaseAntRMS is optional. Return true if this attribute exists.
	 * @return true if and only if the phaseAntRMS attribute exists. 
	 */
	bool CalPhaseRow::isPhaseAntRMSExists() const {
		return phaseAntRMSExists;
	}
	

	
 	/**
 	 * Get phaseAntRMS, which is optional.
 	 * @return phaseAntRMS as std::vector<std::vector<float > >
 	 * @throw IllegalAccessException If phaseAntRMS does not exist.
 	 */
 	std::vector<std::vector<float > > CalPhaseRow::getPhaseAntRMS() const  {
		if (!phaseAntRMSExists) {
			throw IllegalAccessException("phaseAntRMS", "CalPhase");
		}
	
  		return phaseAntRMS;
 	}

 	/**
 	 * Set phaseAntRMS with the specified std::vector<std::vector<float > >.
 	 * @param phaseAntRMS The std::vector<std::vector<float > > value to which phaseAntRMS is to be set.
 	 
 	
 	 */
 	void CalPhaseRow::setPhaseAntRMS (std::vector<std::vector<float > > phaseAntRMS) {
	
 		this->phaseAntRMS = phaseAntRMS;
	
		phaseAntRMSExists = true;
	
 	}
	
	
	/**
	 * Mark phaseAntRMS, which is an optional field, as non-existent.
	 */
	void CalPhaseRow::clearPhaseAntRMS () {
		phaseAntRMSExists = false;
	}
	

	
	///////////////////////////////////////////////
	// Extrinsic Table Attributes getters/setters//
	///////////////////////////////////////////////
	
	

	
 	/**
 	 * Get calDataId.
 	 * @return calDataId as Tag
 	 */
 	Tag CalPhaseRow::getCalDataId() const {
	
  		return calDataId;
 	}

 	/**
 	 * Set calDataId with the specified Tag.
 	 * @param calDataId The Tag value to which calDataId is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void CalPhaseRow::setCalDataId (Tag calDataId)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("calDataId", "CalPhase");
		
  		}
  	
 		this->calDataId = calDataId;
	
 	}
	
	

	

	
 	/**
 	 * Get calReductionId.
 	 * @return calReductionId as Tag
 	 */
 	Tag CalPhaseRow::getCalReductionId() const {
	
  		return calReductionId;
 	}

 	/**
 	 * Set calReductionId with the specified Tag.
 	 * @param calReductionId The Tag value to which calReductionId is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void CalPhaseRow::setCalReductionId (Tag calReductionId)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("calReductionId", "CalPhase");
		
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
	 CalDataRow* CalPhaseRow::getCalDataUsingCalDataId() {
	 
	 	return table.getContainer().getCalData().getRowByKey(calDataId);
	 }
	 

	

	
	
	
		

	/**
	 * Returns the pointer to the row in the CalReduction table having CalReduction.calReductionId == calReductionId
	 * @return a CalReductionRow*
	 * 
	 
	 */
	 CalReductionRow* CalPhaseRow::getCalReductionUsingCalReductionId() {
	 
	 	return table.getContainer().getCalReduction().getRowByKey(calReductionId);
	 }
	 

	

	
	/**
	 * Create a CalPhaseRow.
	 * <p>
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 */ 
	CalPhaseRow::CalPhaseRow (CalPhaseTable &t) : table(t) {
		hasBeenAdded = false;
		
	
	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	
		correctionValidityExists = false;
	

	
		numAntennaExists = false;
	

	
		singleAntennaNameExists = false;
	

	
		refAntennaNameExists = false;
	

	
		phaseAntExists = false;
	

	
		phaseAntRMSExists = false;
	

	
	

	

	
	
	
	
// This attribute is scalar and has an enumeration type. Let's initialize it to some valid value (the 1st of the enumeration).		
basebandName = CBasebandName::from_int(0);
	

	
// This attribute is scalar and has an enumeration type. Let's initialize it to some valid value (the 1st of the enumeration).		
receiverBand = CReceiverBand::from_int(0);
	

	
// This attribute is scalar and has an enumeration type. Let's initialize it to some valid value (the 1st of the enumeration).		
atmPhaseCorrection = CAtmPhaseCorrection::from_int(0);
	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	
	
	 fromBinMethods["basebandName"] = &CalPhaseRow::basebandNameFromBin; 
	 fromBinMethods["receiverBand"] = &CalPhaseRow::receiverBandFromBin; 
	 fromBinMethods["atmPhaseCorrection"] = &CalPhaseRow::atmPhaseCorrectionFromBin; 
	 fromBinMethods["calDataId"] = &CalPhaseRow::calDataIdFromBin; 
	 fromBinMethods["calReductionId"] = &CalPhaseRow::calReductionIdFromBin; 
	 fromBinMethods["startValidTime"] = &CalPhaseRow::startValidTimeFromBin; 
	 fromBinMethods["endValidTime"] = &CalPhaseRow::endValidTimeFromBin; 
	 fromBinMethods["numBaseline"] = &CalPhaseRow::numBaselineFromBin; 
	 fromBinMethods["numReceptor"] = &CalPhaseRow::numReceptorFromBin; 
	 fromBinMethods["ampli"] = &CalPhaseRow::ampliFromBin; 
	 fromBinMethods["antennaNames"] = &CalPhaseRow::antennaNamesFromBin; 
	 fromBinMethods["baselineLengths"] = &CalPhaseRow::baselineLengthsFromBin; 
	 fromBinMethods["decorrelationFactor"] = &CalPhaseRow::decorrelationFactorFromBin; 
	 fromBinMethods["direction"] = &CalPhaseRow::directionFromBin; 
	 fromBinMethods["frequencyRange"] = &CalPhaseRow::frequencyRangeFromBin; 
	 fromBinMethods["integrationTime"] = &CalPhaseRow::integrationTimeFromBin; 
	 fromBinMethods["phase"] = &CalPhaseRow::phaseFromBin; 
	 fromBinMethods["polarizationTypes"] = &CalPhaseRow::polarizationTypesFromBin; 
	 fromBinMethods["phaseRMS"] = &CalPhaseRow::phaseRMSFromBin; 
	 fromBinMethods["statPhaseRMS"] = &CalPhaseRow::statPhaseRMSFromBin; 
		
	
	 fromBinMethods["correctionValidity"] = &CalPhaseRow::correctionValidityFromBin; 
	 fromBinMethods["numAntenna"] = &CalPhaseRow::numAntennaFromBin; 
	 fromBinMethods["singleAntennaName"] = &CalPhaseRow::singleAntennaNameFromBin; 
	 fromBinMethods["refAntennaName"] = &CalPhaseRow::refAntennaNameFromBin; 
	 fromBinMethods["phaseAnt"] = &CalPhaseRow::phaseAntFromBin; 
	 fromBinMethods["phaseAntRMS"] = &CalPhaseRow::phaseAntRMSFromBin; 
	
	
	
	
				 
	fromTextMethods["basebandName"] = &CalPhaseRow::basebandNameFromText;
		 
	
				 
	fromTextMethods["receiverBand"] = &CalPhaseRow::receiverBandFromText;
		 
	
				 
	fromTextMethods["atmPhaseCorrection"] = &CalPhaseRow::atmPhaseCorrectionFromText;
		 
	
				 
	fromTextMethods["calDataId"] = &CalPhaseRow::calDataIdFromText;
		 
	
				 
	fromTextMethods["calReductionId"] = &CalPhaseRow::calReductionIdFromText;
		 
	
				 
	fromTextMethods["startValidTime"] = &CalPhaseRow::startValidTimeFromText;
		 
	
				 
	fromTextMethods["endValidTime"] = &CalPhaseRow::endValidTimeFromText;
		 
	
				 
	fromTextMethods["numBaseline"] = &CalPhaseRow::numBaselineFromText;
		 
	
				 
	fromTextMethods["numReceptor"] = &CalPhaseRow::numReceptorFromText;
		 
	
				 
	fromTextMethods["ampli"] = &CalPhaseRow::ampliFromText;
		 
	
				 
	fromTextMethods["antennaNames"] = &CalPhaseRow::antennaNamesFromText;
		 
	
				 
	fromTextMethods["baselineLengths"] = &CalPhaseRow::baselineLengthsFromText;
		 
	
				 
	fromTextMethods["decorrelationFactor"] = &CalPhaseRow::decorrelationFactorFromText;
		 
	
				 
	fromTextMethods["direction"] = &CalPhaseRow::directionFromText;
		 
	
				 
	fromTextMethods["frequencyRange"] = &CalPhaseRow::frequencyRangeFromText;
		 
	
				 
	fromTextMethods["integrationTime"] = &CalPhaseRow::integrationTimeFromText;
		 
	
				 
	fromTextMethods["phase"] = &CalPhaseRow::phaseFromText;
		 
	
				 
	fromTextMethods["polarizationTypes"] = &CalPhaseRow::polarizationTypesFromText;
		 
	
				 
	fromTextMethods["phaseRMS"] = &CalPhaseRow::phaseRMSFromText;
		 
	
				 
	fromTextMethods["statPhaseRMS"] = &CalPhaseRow::statPhaseRMSFromText;
		 
	

	 
				
	fromTextMethods["correctionValidity"] = &CalPhaseRow::correctionValidityFromText;
		 	
	 
				
	fromTextMethods["numAntenna"] = &CalPhaseRow::numAntennaFromText;
		 	
	 
				
	fromTextMethods["singleAntennaName"] = &CalPhaseRow::singleAntennaNameFromText;
		 	
	 
				
	fromTextMethods["refAntennaName"] = &CalPhaseRow::refAntennaNameFromText;
		 	
	 
				
	fromTextMethods["phaseAnt"] = &CalPhaseRow::phaseAntFromText;
		 	
	 
				
	fromTextMethods["phaseAntRMS"] = &CalPhaseRow::phaseAntRMSFromText;
		 	
		
	}
	
	CalPhaseRow::CalPhaseRow (CalPhaseTable &t, CalPhaseRow *row) : table(t) {
		hasBeenAdded = false;
		
		if (row == 0) {
	
	
	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	
		correctionValidityExists = false;
	

	
		numAntennaExists = false;
	

	
		singleAntennaNameExists = false;
	

	
		refAntennaNameExists = false;
	

	
		phaseAntExists = false;
	

	
		phaseAntRMSExists = false;
	

	
	

	
		
		}
		else {
	
		
			basebandName = row->basebandName;
		
			receiverBand = row->receiverBand;
		
			atmPhaseCorrection = row->atmPhaseCorrection;
		
			calDataId = row->calDataId;
		
			calReductionId = row->calReductionId;
		
		
		
		
			startValidTime = row->startValidTime;
		
			endValidTime = row->endValidTime;
		
			numBaseline = row->numBaseline;
		
			numReceptor = row->numReceptor;
		
			ampli = row->ampli;
		
			antennaNames = row->antennaNames;
		
			baselineLengths = row->baselineLengths;
		
			decorrelationFactor = row->decorrelationFactor;
		
			direction = row->direction;
		
			frequencyRange = row->frequencyRange;
		
			integrationTime = row->integrationTime;
		
			phase = row->phase;
		
			polarizationTypes = row->polarizationTypes;
		
			phaseRMS = row->phaseRMS;
		
			statPhaseRMS = row->statPhaseRMS;
		
		
		
		
		if (row->correctionValidityExists) {
			correctionValidity = row->correctionValidity;		
			correctionValidityExists = true;
		}
		else
			correctionValidityExists = false;
		
		if (row->numAntennaExists) {
			numAntenna = row->numAntenna;		
			numAntennaExists = true;
		}
		else
			numAntennaExists = false;
		
		if (row->singleAntennaNameExists) {
			singleAntennaName = row->singleAntennaName;		
			singleAntennaNameExists = true;
		}
		else
			singleAntennaNameExists = false;
		
		if (row->refAntennaNameExists) {
			refAntennaName = row->refAntennaName;		
			refAntennaNameExists = true;
		}
		else
			refAntennaNameExists = false;
		
		if (row->phaseAntExists) {
			phaseAnt = row->phaseAnt;		
			phaseAntExists = true;
		}
		else
			phaseAntExists = false;
		
		if (row->phaseAntRMSExists) {
			phaseAntRMS = row->phaseAntRMS;		
			phaseAntRMSExists = true;
		}
		else
			phaseAntRMSExists = false;
		
		}
		
		 fromBinMethods["basebandName"] = &CalPhaseRow::basebandNameFromBin; 
		 fromBinMethods["receiverBand"] = &CalPhaseRow::receiverBandFromBin; 
		 fromBinMethods["atmPhaseCorrection"] = &CalPhaseRow::atmPhaseCorrectionFromBin; 
		 fromBinMethods["calDataId"] = &CalPhaseRow::calDataIdFromBin; 
		 fromBinMethods["calReductionId"] = &CalPhaseRow::calReductionIdFromBin; 
		 fromBinMethods["startValidTime"] = &CalPhaseRow::startValidTimeFromBin; 
		 fromBinMethods["endValidTime"] = &CalPhaseRow::endValidTimeFromBin; 
		 fromBinMethods["numBaseline"] = &CalPhaseRow::numBaselineFromBin; 
		 fromBinMethods["numReceptor"] = &CalPhaseRow::numReceptorFromBin; 
		 fromBinMethods["ampli"] = &CalPhaseRow::ampliFromBin; 
		 fromBinMethods["antennaNames"] = &CalPhaseRow::antennaNamesFromBin; 
		 fromBinMethods["baselineLengths"] = &CalPhaseRow::baselineLengthsFromBin; 
		 fromBinMethods["decorrelationFactor"] = &CalPhaseRow::decorrelationFactorFromBin; 
		 fromBinMethods["direction"] = &CalPhaseRow::directionFromBin; 
		 fromBinMethods["frequencyRange"] = &CalPhaseRow::frequencyRangeFromBin; 
		 fromBinMethods["integrationTime"] = &CalPhaseRow::integrationTimeFromBin; 
		 fromBinMethods["phase"] = &CalPhaseRow::phaseFromBin; 
		 fromBinMethods["polarizationTypes"] = &CalPhaseRow::polarizationTypesFromBin; 
		 fromBinMethods["phaseRMS"] = &CalPhaseRow::phaseRMSFromBin; 
		 fromBinMethods["statPhaseRMS"] = &CalPhaseRow::statPhaseRMSFromBin; 
			
	
		 fromBinMethods["correctionValidity"] = &CalPhaseRow::correctionValidityFromBin; 
		 fromBinMethods["numAntenna"] = &CalPhaseRow::numAntennaFromBin; 
		 fromBinMethods["singleAntennaName"] = &CalPhaseRow::singleAntennaNameFromBin; 
		 fromBinMethods["refAntennaName"] = &CalPhaseRow::refAntennaNameFromBin; 
		 fromBinMethods["phaseAnt"] = &CalPhaseRow::phaseAntFromBin; 
		 fromBinMethods["phaseAntRMS"] = &CalPhaseRow::phaseAntRMSFromBin; 
			
	}

	
	bool CalPhaseRow::compareNoAutoInc(BasebandNameMod::BasebandName basebandName, ReceiverBandMod::ReceiverBand receiverBand, AtmPhaseCorrectionMod::AtmPhaseCorrection atmPhaseCorrection, Tag calDataId, Tag calReductionId, ArrayTime startValidTime, ArrayTime endValidTime, int numBaseline, int numReceptor, std::vector<std::vector<float > > ampli, std::vector<std::vector<std::string > > antennaNames, std::vector<Length > baselineLengths, std::vector<std::vector<float > > decorrelationFactor, std::vector<Angle > direction, std::vector<Frequency > frequencyRange, Interval integrationTime, std::vector<std::vector<float > > phase, std::vector<PolarizationTypeMod::PolarizationType > polarizationTypes, std::vector<std::vector<float > > phaseRMS, std::vector<std::vector<float > > statPhaseRMS) {
		bool result;
		result = true;
		
	
		
		result = result && (this->basebandName == basebandName);
		
		if (!result) return false;
	

	
		
		result = result && (this->receiverBand == receiverBand);
		
		if (!result) return false;
	

	
		
		result = result && (this->atmPhaseCorrection == atmPhaseCorrection);
		
		if (!result) return false;
	

	
		
		result = result && (this->calDataId == calDataId);
		
		if (!result) return false;
	

	
		
		result = result && (this->calReductionId == calReductionId);
		
		if (!result) return false;
	

	
		
		result = result && (this->startValidTime == startValidTime);
		
		if (!result) return false;
	

	
		
		result = result && (this->endValidTime == endValidTime);
		
		if (!result) return false;
	

	
		
		result = result && (this->numBaseline == numBaseline);
		
		if (!result) return false;
	

	
		
		result = result && (this->numReceptor == numReceptor);
		
		if (!result) return false;
	

	
		
		result = result && (this->ampli == ampli);
		
		if (!result) return false;
	

	
		
		result = result && (this->antennaNames == antennaNames);
		
		if (!result) return false;
	

	
		
		result = result && (this->baselineLengths == baselineLengths);
		
		if (!result) return false;
	

	
		
		result = result && (this->decorrelationFactor == decorrelationFactor);
		
		if (!result) return false;
	

	
		
		result = result && (this->direction == direction);
		
		if (!result) return false;
	

	
		
		result = result && (this->frequencyRange == frequencyRange);
		
		if (!result) return false;
	

	
		
		result = result && (this->integrationTime == integrationTime);
		
		if (!result) return false;
	

	
		
		result = result && (this->phase == phase);
		
		if (!result) return false;
	

	
		
		result = result && (this->polarizationTypes == polarizationTypes);
		
		if (!result) return false;
	

	
		
		result = result && (this->phaseRMS == phaseRMS);
		
		if (!result) return false;
	

	
		
		result = result && (this->statPhaseRMS == statPhaseRMS);
		
		if (!result) return false;
	

		return result;
	}	
	
	
	
	bool CalPhaseRow::compareRequiredValue(ArrayTime startValidTime, ArrayTime endValidTime, int numBaseline, int numReceptor, std::vector<std::vector<float > > ampli, std::vector<std::vector<std::string > > antennaNames, std::vector<Length > baselineLengths, std::vector<std::vector<float > > decorrelationFactor, std::vector<Angle > direction, std::vector<Frequency > frequencyRange, Interval integrationTime, std::vector<std::vector<float > > phase, std::vector<PolarizationTypeMod::PolarizationType > polarizationTypes, std::vector<std::vector<float > > phaseRMS, std::vector<std::vector<float > > statPhaseRMS) {
		bool result;
		result = true;
		
	
		if (!(this->startValidTime == startValidTime)) return false;
	

	
		if (!(this->endValidTime == endValidTime)) return false;
	

	
		if (!(this->numBaseline == numBaseline)) return false;
	

	
		if (!(this->numReceptor == numReceptor)) return false;
	

	
		if (!(this->ampli == ampli)) return false;
	

	
		if (!(this->antennaNames == antennaNames)) return false;
	

	
		if (!(this->baselineLengths == baselineLengths)) return false;
	

	
		if (!(this->decorrelationFactor == decorrelationFactor)) return false;
	

	
		if (!(this->direction == direction)) return false;
	

	
		if (!(this->frequencyRange == frequencyRange)) return false;
	

	
		if (!(this->integrationTime == integrationTime)) return false;
	

	
		if (!(this->phase == phase)) return false;
	

	
		if (!(this->polarizationTypes == polarizationTypes)) return false;
	

	
		if (!(this->phaseRMS == phaseRMS)) return false;
	

	
		if (!(this->statPhaseRMS == statPhaseRMS)) return false;
	

		return result;
	}
	
	
	/**
	 * Return true if all required attributes of the value part are equal to their homologues
	 * in x and false otherwise.
	 *

	 * @param x a pointer on the CalPhaseRow whose required attributes of the value part 

	 * will be compared with those of this.
	 * @return a boolean.
	 */
	bool CalPhaseRow::equalByRequiredValue(CalPhaseRow*  x ) {
		
			
		if (this->startValidTime != x->startValidTime) return false;
			
		if (this->endValidTime != x->endValidTime) return false;
			
		if (this->numBaseline != x->numBaseline) return false;
			
		if (this->numReceptor != x->numReceptor) return false;
			
		if (this->ampli != x->ampli) return false;
			
		if (this->antennaNames != x->antennaNames) return false;
			
		if (this->baselineLengths != x->baselineLengths) return false;
			
		if (this->decorrelationFactor != x->decorrelationFactor) return false;
			
		if (this->direction != x->direction) return false;
			
		if (this->frequencyRange != x->frequencyRange) return false;
			
		if (this->integrationTime != x->integrationTime) return false;
			
		if (this->phase != x->phase) return false;
			
		if (this->polarizationTypes != x->polarizationTypes) return false;
			
		if (this->phaseRMS != x->phaseRMS) return false;
			
		if (this->statPhaseRMS != x->statPhaseRMS) return false;
			
		
		return true;
	}	
	
/*
	 map<string, CalPhaseAttributeFromBin> CalPhaseRow::initFromBinMethods() {
		map<string, CalPhaseAttributeFromBin> result;
		
		result["basebandName"] = &CalPhaseRow::basebandNameFromBin;
		result["receiverBand"] = &CalPhaseRow::receiverBandFromBin;
		result["atmPhaseCorrection"] = &CalPhaseRow::atmPhaseCorrectionFromBin;
		result["calDataId"] = &CalPhaseRow::calDataIdFromBin;
		result["calReductionId"] = &CalPhaseRow::calReductionIdFromBin;
		result["startValidTime"] = &CalPhaseRow::startValidTimeFromBin;
		result["endValidTime"] = &CalPhaseRow::endValidTimeFromBin;
		result["numBaseline"] = &CalPhaseRow::numBaselineFromBin;
		result["numReceptor"] = &CalPhaseRow::numReceptorFromBin;
		result["ampli"] = &CalPhaseRow::ampliFromBin;
		result["antennaNames"] = &CalPhaseRow::antennaNamesFromBin;
		result["baselineLengths"] = &CalPhaseRow::baselineLengthsFromBin;
		result["decorrelationFactor"] = &CalPhaseRow::decorrelationFactorFromBin;
		result["direction"] = &CalPhaseRow::directionFromBin;
		result["frequencyRange"] = &CalPhaseRow::frequencyRangeFromBin;
		result["integrationTime"] = &CalPhaseRow::integrationTimeFromBin;
		result["phase"] = &CalPhaseRow::phaseFromBin;
		result["polarizationTypes"] = &CalPhaseRow::polarizationTypesFromBin;
		result["phaseRMS"] = &CalPhaseRow::phaseRMSFromBin;
		result["statPhaseRMS"] = &CalPhaseRow::statPhaseRMSFromBin;
		
		
		result["correctionValidity"] = &CalPhaseRow::correctionValidityFromBin;
		result["numAntenna"] = &CalPhaseRow::numAntennaFromBin;
		result["singleAntennaName"] = &CalPhaseRow::singleAntennaNameFromBin;
		result["refAntennaName"] = &CalPhaseRow::refAntennaNameFromBin;
		result["phaseAnt"] = &CalPhaseRow::phaseAntFromBin;
		result["phaseAntRMS"] = &CalPhaseRow::phaseAntRMSFromBin;
			
		
		return result;	
	}
*/	
} // End namespace asdm
 
