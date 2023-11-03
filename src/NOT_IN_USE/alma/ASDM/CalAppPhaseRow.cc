
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
 * File CalAppPhaseRow.cpp
 */
 
#include <vector>
#include <set>

#include <alma/ASDM/ASDM.h>
#include <alma/ASDM/CalAppPhaseRow.h>
#include <alma/ASDM/CalAppPhaseTable.h>

#include <alma/ASDM/CalDataTable.h>
#include <alma/ASDM/CalDataRow.h>

#include <alma/ASDM/CalReductionTable.h>
#include <alma/ASDM/CalReductionRow.h>
	

using asdm::ASDM;
using asdm::CalAppPhaseRow;
using asdm::CalAppPhaseTable;

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
	CalAppPhaseRow::~CalAppPhaseRow() {
	}

	/**
	 * Return the table to which this row belongs.
	 */
	CalAppPhaseTable &CalAppPhaseRow::getTable() const {
		return table;
	}

	bool CalAppPhaseRow::isAdded() const {
		return hasBeenAdded;
	}	

	void CalAppPhaseRow::isAdded(bool added) {
		hasBeenAdded = added;
	}
	
#ifndef WITHOUT_ACS
	using asdmIDL::CalAppPhaseRowIDL;
#endif
	
#ifndef WITHOUT_ACS
	/**
	 * Return this row in the form of an IDL struct.
	 * @return The values of this row as a CalAppPhaseRowIDL struct.
	 */
	CalAppPhaseRowIDL *CalAppPhaseRow::toIDL() const {
		CalAppPhaseRowIDL *x = new CalAppPhaseRowIDL ();
		
		// Fill the IDL structure.
	
		
	
  		
		
		
			
				
		x->basebandName = basebandName;
 				
 			
		
	

	
  		
		
		
			
				
		x->scanNumber = scanNumber;
 				
 			
		
	

	
  		
		
		
			
		x->startValidTime = startValidTime.toIDLArrayTime();
			
		
	

	
  		
		
		
			
		x->endValidTime = endValidTime.toIDLArrayTime();
			
		
	

	
  		
		
		
			
		x->adjustTime = adjustTime.toIDLArrayTime();
			
		
	

	
  		
		
		
			
				
		x->adjustToken = CORBA::string_dup(adjustToken.c_str());
				
 			
		
	

	
  		
		
		
			
				
		x->phasingMode = CORBA::string_dup(phasingMode.c_str());
				
 			
		
	

	
  		
		
		
			
				
		x->numPhasedAntennas = numPhasedAntennas;
 				
 			
		
	

	
  		
		
		
			
		x->phasedAntennas.length(phasedAntennas.size());
		for (unsigned int i = 0; i < phasedAntennas.size(); ++i) {
			
				
			x->phasedAntennas[i] = CORBA::string_dup(phasedAntennas.at(i).c_str());
				
	 		
	 	}
			
		
	

	
  		
		
		
			
				
		x->refAntennaIndex = refAntennaIndex;
 				
 			
		
	

	
  		
		
		
			
				
		x->candRefAntennaIndex = candRefAntennaIndex;
 				
 			
		
	

	
  		
		
		
			
				
		x->phasePacking = CORBA::string_dup(phasePacking.c_str());
				
 			
		
	

	
  		
		
		
			
				
		x->numReceptors = numReceptors;
 				
 			
		
	

	
  		
		
		
			
				
		x->numChannels = numChannels;
 				
 			
		
	

	
  		
		
		
			
				
		x->numPhaseValues = numPhaseValues;
 				
 			
		
	

	
  		
		
		
			
		x->phaseValues.length(phaseValues.size());
		for (unsigned int i = 0; i < phaseValues.size(); ++i) {
			
				
			x->phaseValues[i] = phaseValues.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		
			
				
		x->numCompare = numCompare;
 				
 			
		
	

	
  		
		
		
			
				
		x->numEfficiencies = numEfficiencies;
 				
 			
		
	

	
  		
		
		
			
		x->compareArray.length(compareArray.size());
		for (unsigned int i = 0; i < compareArray.size(); ++i) {
			
				
			x->compareArray[i] = CORBA::string_dup(compareArray.at(i).c_str());
				
	 		
	 	}
			
		
	

	
  		
		
		
			
		x->efficiencyIndices.length(efficiencyIndices.size());
		for (unsigned int i = 0; i < efficiencyIndices.size(); ++i) {
			
				
			x->efficiencyIndices[i] = efficiencyIndices.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		
			
		x->efficiencies.length(efficiencies.size());
		for (unsigned int i = 0; i < efficiencies.size(); i++) {
			x->efficiencies[i].length(efficiencies.at(i).size());			 		
		}
		
		for (unsigned int i = 0; i < efficiencies.size() ; i++)
			for (unsigned int j = 0; j < efficiencies.at(i).size(); j++)
					
						
				x->efficiencies[i][j] = efficiencies.at(i).at(j);
		 				
			 						
		
			
		
	

	
  		
		
		
			
		x->quality.length(quality.size());
		for (unsigned int i = 0; i < quality.size(); ++i) {
			
				
			x->quality[i] = quality.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		
			
				
		x->phasedSumAntenna = CORBA::string_dup(phasedSumAntenna.c_str());
				
 			
		
	

	
  		
		
		x->typeSupportsExists = typeSupportsExists;
		
		
			
				
		x->typeSupports = CORBA::string_dup(typeSupports.c_str());
				
 			
		
	

	
  		
		
		x->numSupportsExists = numSupportsExists;
		
		
			
				
		x->numSupports = numSupports;
 				
 			
		
	

	
  		
		
		x->phaseSupportsExists = phaseSupportsExists;
		
		
			
		x->phaseSupports.length(phaseSupports.size());
		for (unsigned int i = 0; i < phaseSupports.size(); ++i) {
			
				
			x->phaseSupports[i] = phaseSupports.at(i);
	 			
	 		
	 	}
			
		
	

	
	
		
	
  	
 		
		
	 	
			
		x->calDataId = calDataId.toIDLTag();
			
	 	 		
  	

	
  	
 		
		
	 	
			
		x->calReductionId = calReductionId.toIDLTag();
			
	 	 		
  	

	
		
	

	

		
		return x;
	
	}
	
	void CalAppPhaseRow::toIDL(asdmIDL::CalAppPhaseRowIDL& x) const {
		// Set the x's fields.
	
		
	
  		
		
		
			
				
		x.basebandName = basebandName;
 				
 			
		
	

	
  		
		
		
			
				
		x.scanNumber = scanNumber;
 				
 			
		
	

	
  		
		
		
			
		x.startValidTime = startValidTime.toIDLArrayTime();
			
		
	

	
  		
		
		
			
		x.endValidTime = endValidTime.toIDLArrayTime();
			
		
	

	
  		
		
		
			
		x.adjustTime = adjustTime.toIDLArrayTime();
			
		
	

	
  		
		
		
			
				
		x.adjustToken = CORBA::string_dup(adjustToken.c_str());
				
 			
		
	

	
  		
		
		
			
				
		x.phasingMode = CORBA::string_dup(phasingMode.c_str());
				
 			
		
	

	
  		
		
		
			
				
		x.numPhasedAntennas = numPhasedAntennas;
 				
 			
		
	

	
  		
		
		
			
		x.phasedAntennas.length(phasedAntennas.size());
		for (unsigned int i = 0; i < phasedAntennas.size(); ++i) {
			
				
			x.phasedAntennas[i] = CORBA::string_dup(phasedAntennas.at(i).c_str());
				
	 		
	 	}
			
		
	

	
  		
		
		
			
				
		x.refAntennaIndex = refAntennaIndex;
 				
 			
		
	

	
  		
		
		
			
				
		x.candRefAntennaIndex = candRefAntennaIndex;
 				
 			
		
	

	
  		
		
		
			
				
		x.phasePacking = CORBA::string_dup(phasePacking.c_str());
				
 			
		
	

	
  		
		
		
			
				
		x.numReceptors = numReceptors;
 				
 			
		
	

	
  		
		
		
			
				
		x.numChannels = numChannels;
 				
 			
		
	

	
  		
		
		
			
				
		x.numPhaseValues = numPhaseValues;
 				
 			
		
	

	
  		
		
		
			
		x.phaseValues.length(phaseValues.size());
		for (unsigned int i = 0; i < phaseValues.size(); ++i) {
			
				
			x.phaseValues[i] = phaseValues.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		
			
				
		x.numCompare = numCompare;
 				
 			
		
	

	
  		
		
		
			
				
		x.numEfficiencies = numEfficiencies;
 				
 			
		
	

	
  		
		
		
			
		x.compareArray.length(compareArray.size());
		for (unsigned int i = 0; i < compareArray.size(); ++i) {
			
				
			x.compareArray[i] = CORBA::string_dup(compareArray.at(i).c_str());
				
	 		
	 	}
			
		
	

	
  		
		
		
			
		x.efficiencyIndices.length(efficiencyIndices.size());
		for (unsigned int i = 0; i < efficiencyIndices.size(); ++i) {
			
				
			x.efficiencyIndices[i] = efficiencyIndices.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		
			
		x.efficiencies.length(efficiencies.size());
		for (unsigned int i = 0; i < efficiencies.size(); i++) {
			x.efficiencies[i].length(efficiencies.at(i).size());			 		
		}
		
		for (unsigned int i = 0; i < efficiencies.size() ; i++)
			for (unsigned int j = 0; j < efficiencies.at(i).size(); j++)
					
						
				x.efficiencies[i][j] = efficiencies.at(i).at(j);
		 				
			 						
		
			
		
	

	
  		
		
		
			
		x.quality.length(quality.size());
		for (unsigned int i = 0; i < quality.size(); ++i) {
			
				
			x.quality[i] = quality.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		
			
				
		x.phasedSumAntenna = CORBA::string_dup(phasedSumAntenna.c_str());
				
 			
		
	

	
  		
		
		x.typeSupportsExists = typeSupportsExists;
		
		
			
				
		x.typeSupports = CORBA::string_dup(typeSupports.c_str());
				
 			
		
	

	
  		
		
		x.numSupportsExists = numSupportsExists;
		
		
			
				
		x.numSupports = numSupports;
 				
 			
		
	

	
  		
		
		x.phaseSupportsExists = phaseSupportsExists;
		
		
			
		x.phaseSupports.length(phaseSupports.size());
		for (unsigned int i = 0; i < phaseSupports.size(); ++i) {
			
				
			x.phaseSupports[i] = phaseSupports.at(i);
	 			
	 		
	 	}
			
		
	

	
	
		
	
  	
 		
		
	 	
			
		x.calDataId = calDataId.toIDLTag();
			
	 	 		
  	

	
  	
 		
		
	 	
			
		x.calReductionId = calReductionId.toIDLTag();
			
	 	 		
  	

	
		
	

	

	
	}
#endif
	

#ifndef WITHOUT_ACS
	/**
	 * Fill the values of this row from the IDL struct CalAppPhaseRowIDL.
	 * @param x The IDL struct containing the values used to fill this row.
	 */
	void CalAppPhaseRow::setFromIDL (CalAppPhaseRowIDL x){
		try {
		// Fill the values from x.
	
		
	
		
		
			
		setBasebandName(x.basebandName);
  			
 		
		
	

	
		
		
			
		setScanNumber(x.scanNumber);
  			
 		
		
	

	
		
		
			
		setStartValidTime(ArrayTime (x.startValidTime));
			
 		
		
	

	
		
		
			
		setEndValidTime(ArrayTime (x.endValidTime));
			
 		
		
	

	
		
		
			
		setAdjustTime(ArrayTime (x.adjustTime));
			
 		
		
	

	
		
		
			
		setAdjustToken(string (x.adjustToken));
			
 		
		
	

	
		
		
			
		setPhasingMode(string (x.phasingMode));
			
 		
		
	

	
		
		
			
		setNumPhasedAntennas(x.numPhasedAntennas);
  			
 		
		
	

	
		
		
			
		phasedAntennas .clear();
		for (unsigned int i = 0; i <x.phasedAntennas.length(); ++i) {
			
			phasedAntennas.push_back(string (x.phasedAntennas[i]));
			
		}
			
  		
		
	

	
		
		
			
		setRefAntennaIndex(x.refAntennaIndex);
  			
 		
		
	

	
		
		
			
		setCandRefAntennaIndex(x.candRefAntennaIndex);
  			
 		
		
	

	
		
		
			
		setPhasePacking(string (x.phasePacking));
			
 		
		
	

	
		
		
			
		setNumReceptors(x.numReceptors);
  			
 		
		
	

	
		
		
			
		setNumChannels(x.numChannels);
  			
 		
		
	

	
		
		
			
		setNumPhaseValues(x.numPhaseValues);
  			
 		
		
	

	
		
		
			
		phaseValues .clear();
		for (unsigned int i = 0; i <x.phaseValues.length(); ++i) {
			
			phaseValues.push_back(x.phaseValues[i]);
  			
		}
			
  		
		
	

	
		
		
			
		setNumCompare(x.numCompare);
  			
 		
		
	

	
		
		
			
		setNumEfficiencies(x.numEfficiencies);
  			
 		
		
	

	
		
		
			
		compareArray .clear();
		for (unsigned int i = 0; i <x.compareArray.length(); ++i) {
			
			compareArray.push_back(string (x.compareArray[i]));
			
		}
			
  		
		
	

	
		
		
			
		efficiencyIndices .clear();
		for (unsigned int i = 0; i <x.efficiencyIndices.length(); ++i) {
			
			efficiencyIndices.push_back(x.efficiencyIndices[i]);
  			
		}
			
  		
		
	

	
		
		
			
		efficiencies .clear();
        
        vector<float> v_aux_efficiencies;
        
		for (unsigned int i = 0; i < x.efficiencies.length(); ++i) {
			v_aux_efficiencies.clear();
			for (unsigned int j = 0; j < x.efficiencies[0].length(); ++j) {
				
				v_aux_efficiencies.push_back(x.efficiencies[i][j]);
	  			
  			}
  			efficiencies.push_back(v_aux_efficiencies);			
		}
			
  		
		
	

	
		
		
			
		quality .clear();
		for (unsigned int i = 0; i <x.quality.length(); ++i) {
			
			quality.push_back(x.quality[i]);
  			
		}
			
  		
		
	

	
		
		
			
		setPhasedSumAntenna(string (x.phasedSumAntenna));
			
 		
		
	

	
		
		typeSupportsExists = x.typeSupportsExists;
		if (x.typeSupportsExists) {
		
		
			
		setTypeSupports(string (x.typeSupports));
			
 		
		
		}
		
	

	
		
		numSupportsExists = x.numSupportsExists;
		if (x.numSupportsExists) {
		
		
			
		setNumSupports(x.numSupports);
  			
 		
		
		}
		
	

	
		
		phaseSupportsExists = x.phaseSupportsExists;
		if (x.phaseSupportsExists) {
		
		
			
		phaseSupports .clear();
		for (unsigned int i = 0; i <x.phaseSupports.length(); ++i) {
			
			phaseSupports.push_back(x.phaseSupports[i]);
  			
		}
			
  		
		
		}
		
	

	
	
		
	
		
		
			
		setCalDataId(Tag (x.calDataId));
			
 		
		
	

	
		
		
			
		setCalReductionId(Tag (x.calReductionId));
			
 		
		
	

	
		
	

	

		} catch (const IllegalAccessException &err) {
			throw ConversionException (err.getMessage(),"CalAppPhase");
		}
	}
#endif
	
	/**
	 * Return this row in the form of an XML string.
	 * @return The values of this row as an XML string.
	 */
	string CalAppPhaseRow::toXML() const {
		string buf;
		buf.append("<row> \n");
		
	
		
  	
 		
		
			buf.append(EnumerationParser::toXML("basebandName", basebandName));
		
		
	

  	
 		
		
		Parser::toXML(scanNumber, "scanNumber", buf);
		
		
	

  	
 		
		
		Parser::toXML(startValidTime, "startValidTime", buf);
		
		
	

  	
 		
		
		Parser::toXML(endValidTime, "endValidTime", buf);
		
		
	

  	
 		
		
		Parser::toXML(adjustTime, "adjustTime", buf);
		
		
	

  	
 		
		
		Parser::toXML(adjustToken, "adjustToken", buf);
		
		
	

  	
 		
		
		Parser::toXML(phasingMode, "phasingMode", buf);
		
		
	

  	
 		
		
		Parser::toXML(numPhasedAntennas, "numPhasedAntennas", buf);
		
		
	

  	
 		
		
		Parser::toXML(phasedAntennas, "phasedAntennas", buf);
		
		
	

  	
 		
		
		Parser::toXML(refAntennaIndex, "refAntennaIndex", buf);
		
		
	

  	
 		
		
		Parser::toXML(candRefAntennaIndex, "candRefAntennaIndex", buf);
		
		
	

  	
 		
		
		Parser::toXML(phasePacking, "phasePacking", buf);
		
		
	

  	
 		
		
		Parser::toXML(numReceptors, "numReceptors", buf);
		
		
	

  	
 		
		
		Parser::toXML(numChannels, "numChannels", buf);
		
		
	

  	
 		
		
		Parser::toXML(numPhaseValues, "numPhaseValues", buf);
		
		
	

  	
 		
		
		Parser::toXML(phaseValues, "phaseValues", buf);
		
		
	

  	
 		
		
		Parser::toXML(numCompare, "numCompare", buf);
		
		
	

  	
 		
		
		Parser::toXML(numEfficiencies, "numEfficiencies", buf);
		
		
	

  	
 		
		
		Parser::toXML(compareArray, "compareArray", buf);
		
		
	

  	
 		
		
		Parser::toXML(efficiencyIndices, "efficiencyIndices", buf);
		
		
	

  	
 		
		
		Parser::toXML(efficiencies, "efficiencies", buf);
		
		
	

  	
 		
		
		Parser::toXML(quality, "quality", buf);
		
		
	

  	
 		
		
		Parser::toXML(phasedSumAntenna, "phasedSumAntenna", buf);
		
		
	

  	
 		
		if (typeSupportsExists) {
		
		
		Parser::toXML(typeSupports, "typeSupports", buf);
		
		
		}
		
	

  	
 		
		if (numSupportsExists) {
		
		
		Parser::toXML(numSupports, "numSupports", buf);
		
		
		}
		
	

  	
 		
		if (phaseSupportsExists) {
		
		
		Parser::toXML(phaseSupports, "phaseSupports", buf);
		
		
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
	void CalAppPhaseRow::setFromXML (string rowDoc) {
		Parser row(rowDoc);
		string s = "";
		try {
	
		
	
		
		
		
		basebandName = EnumerationParser::getBasebandName("basebandName","CalAppPhase",rowDoc);
		
		
		
	

	
  		
			
	  	setScanNumber(Parser::getInteger("scanNumber","CalAppPhase",rowDoc));
			
		
	

	
  		
			
	  	setStartValidTime(Parser::getArrayTime("startValidTime","CalAppPhase",rowDoc));
			
		
	

	
  		
			
	  	setEndValidTime(Parser::getArrayTime("endValidTime","CalAppPhase",rowDoc));
			
		
	

	
  		
			
	  	setAdjustTime(Parser::getArrayTime("adjustTime","CalAppPhase",rowDoc));
			
		
	

	
  		
			
	  	setAdjustToken(Parser::getString("adjustToken","CalAppPhase",rowDoc));
			
		
	

	
  		
			
	  	setPhasingMode(Parser::getString("phasingMode","CalAppPhase",rowDoc));
			
		
	

	
  		
			
	  	setNumPhasedAntennas(Parser::getInteger("numPhasedAntennas","CalAppPhase",rowDoc));
			
		
	

	
  		
			
					
	  	setPhasedAntennas(Parser::get1DString("phasedAntennas","CalAppPhase",rowDoc));
	  			
	  		
		
	

	
  		
			
	  	setRefAntennaIndex(Parser::getInteger("refAntennaIndex","CalAppPhase",rowDoc));
			
		
	

	
  		
			
	  	setCandRefAntennaIndex(Parser::getInteger("candRefAntennaIndex","CalAppPhase",rowDoc));
			
		
	

	
  		
			
	  	setPhasePacking(Parser::getString("phasePacking","CalAppPhase",rowDoc));
			
		
	

	
  		
			
	  	setNumReceptors(Parser::getInteger("numReceptors","CalAppPhase",rowDoc));
			
		
	

	
  		
			
	  	setNumChannels(Parser::getInteger("numChannels","CalAppPhase",rowDoc));
			
		
	

	
  		
			
	  	setNumPhaseValues(Parser::getInteger("numPhaseValues","CalAppPhase",rowDoc));
			
		
	

	
  		
			
					
	  	setPhaseValues(Parser::get1DFloat("phaseValues","CalAppPhase",rowDoc));
	  			
	  		
		
	

	
  		
			
	  	setNumCompare(Parser::getInteger("numCompare","CalAppPhase",rowDoc));
			
		
	

	
  		
			
	  	setNumEfficiencies(Parser::getInteger("numEfficiencies","CalAppPhase",rowDoc));
			
		
	

	
  		
			
					
	  	setCompareArray(Parser::get1DString("compareArray","CalAppPhase",rowDoc));
	  			
	  		
		
	

	
  		
			
					
	  	setEfficiencyIndices(Parser::get1DInteger("efficiencyIndices","CalAppPhase",rowDoc));
	  			
	  		
		
	

	
  		
			
					
	  	setEfficiencies(Parser::get2DFloat("efficiencies","CalAppPhase",rowDoc));
	  			
	  		
		
	

	
  		
			
					
	  	setQuality(Parser::get1DFloat("quality","CalAppPhase",rowDoc));
	  			
	  		
		
	

	
  		
			
	  	setPhasedSumAntenna(Parser::getString("phasedSumAntenna","CalAppPhase",rowDoc));
			
		
	

	
  		
        if (row.isStr("<typeSupports>")) {
			
	  		setTypeSupports(Parser::getString("typeSupports","CalAppPhase",rowDoc));
			
		}
 		
	

	
  		
        if (row.isStr("<numSupports>")) {
			
	  		setNumSupports(Parser::getInteger("numSupports","CalAppPhase",rowDoc));
			
		}
 		
	

	
  		
        if (row.isStr("<phaseSupports>")) {
			
								
	  		setPhaseSupports(Parser::get1DFloat("phaseSupports","CalAppPhase",rowDoc));
	  			
	  		
		}
 		
	

	
	
		
	
  		
			
	  	setCalDataId(Parser::getTag("calDataId","CalData",rowDoc));
			
		
	

	
  		
			
	  	setCalReductionId(Parser::getTag("calReductionId","CalReduction",rowDoc));
			
		
	

	
		
	

	

		} catch (const IllegalAccessException &err) {
			throw ConversionException (err.getMessage(),"CalAppPhase");
		}
	}
	
	void CalAppPhaseRow::toBin(EndianOSStream& eoss) {
	
	
	
	
		
					
		eoss.writeString(CBasebandName::name(basebandName));
			/* eoss.writeInt(basebandName); */
				
		
	

	
	
		
						
			eoss.writeInt(scanNumber);
				
		
	

	
	
		
	calDataId.toBin(eoss);
		
	

	
	
		
	calReductionId.toBin(eoss);
		
	

	
	
		
	startValidTime.toBin(eoss);
		
	

	
	
		
	endValidTime.toBin(eoss);
		
	

	
	
		
	adjustTime.toBin(eoss);
		
	

	
	
		
						
			eoss.writeString(adjustToken);
				
		
	

	
	
		
						
			eoss.writeString(phasingMode);
				
		
	

	
	
		
						
			eoss.writeInt(numPhasedAntennas);
				
		
	

	
	
		
		
			
		eoss.writeInt((int) phasedAntennas.size());
		for (unsigned int i = 0; i < phasedAntennas.size(); i++)
				
			eoss.writeString(phasedAntennas.at(i));
				
				
						
		
	

	
	
		
						
			eoss.writeInt(refAntennaIndex);
				
		
	

	
	
		
						
			eoss.writeInt(candRefAntennaIndex);
				
		
	

	
	
		
						
			eoss.writeString(phasePacking);
				
		
	

	
	
		
						
			eoss.writeInt(numReceptors);
				
		
	

	
	
		
						
			eoss.writeInt(numChannels);
				
		
	

	
	
		
						
			eoss.writeInt(numPhaseValues);
				
		
	

	
	
		
		
			
		eoss.writeInt((int) phaseValues.size());
		for (unsigned int i = 0; i < phaseValues.size(); i++)
				
			eoss.writeFloat(phaseValues.at(i));
				
				
						
		
	

	
	
		
						
			eoss.writeInt(numCompare);
				
		
	

	
	
		
						
			eoss.writeInt(numEfficiencies);
				
		
	

	
	
		
		
			
		eoss.writeInt((int) compareArray.size());
		for (unsigned int i = 0; i < compareArray.size(); i++)
				
			eoss.writeString(compareArray.at(i));
				
				
						
		
	

	
	
		
		
			
		eoss.writeInt((int) efficiencyIndices.size());
		for (unsigned int i = 0; i < efficiencyIndices.size(); i++)
				
			eoss.writeInt(efficiencyIndices.at(i));
				
				
						
		
	

	
	
		
		
			
		eoss.writeInt((int) efficiencies.size());
		eoss.writeInt((int) efficiencies.at(0).size());
		for (unsigned int i = 0; i < efficiencies.size(); i++) 
			for (unsigned int j = 0;  j < efficiencies.at(0).size(); j++) 
							 
				eoss.writeFloat(efficiencies.at(i).at(j));
				
	
						
		
	

	
	
		
		
			
		eoss.writeInt((int) quality.size());
		for (unsigned int i = 0; i < quality.size(); i++)
				
			eoss.writeFloat(quality.at(i));
				
				
						
		
	

	
	
		
						
			eoss.writeString(phasedSumAntenna);
				
		
	


	
	
	eoss.writeBoolean(typeSupportsExists);
	if (typeSupportsExists) {
	
	
	
		
						
			eoss.writeString(typeSupports);
				
		
	

	}

	eoss.writeBoolean(numSupportsExists);
	if (numSupportsExists) {
	
	
	
		
						
			eoss.writeInt(numSupports);
				
		
	

	}

	eoss.writeBoolean(phaseSupportsExists);
	if (phaseSupportsExists) {
	
	
	
		
		
			
		eoss.writeInt((int) phaseSupports.size());
		for (unsigned int i = 0; i < phaseSupports.size(); i++)
				
			eoss.writeFloat(phaseSupports.at(i));
				
				
						
		
	

	}

	}
	
void CalAppPhaseRow::basebandNameFromBin(EndianIStream& eis) {
		
	
	
		
			
		basebandName = CBasebandName::literal(eis.readString());
			
		
	
	
}
void CalAppPhaseRow::scanNumberFromBin(EndianIStream& eis) {
		
	
	
		
			
		scanNumber =  eis.readInt();
			
		
	
	
}
void CalAppPhaseRow::calDataIdFromBin(EndianIStream& eis) {
		
	
		
		
		calDataId =  Tag::fromBin(eis);
		
	
	
}
void CalAppPhaseRow::calReductionIdFromBin(EndianIStream& eis) {
		
	
		
		
		calReductionId =  Tag::fromBin(eis);
		
	
	
}
void CalAppPhaseRow::startValidTimeFromBin(EndianIStream& eis) {
		
	
		
		
		startValidTime =  ArrayTime::fromBin(eis);
		
	
	
}
void CalAppPhaseRow::endValidTimeFromBin(EndianIStream& eis) {
		
	
		
		
		endValidTime =  ArrayTime::fromBin(eis);
		
	
	
}
void CalAppPhaseRow::adjustTimeFromBin(EndianIStream& eis) {
		
	
		
		
		adjustTime =  ArrayTime::fromBin(eis);
		
	
	
}
void CalAppPhaseRow::adjustTokenFromBin(EndianIStream& eis) {
		
	
	
		
			
		adjustToken =  eis.readString();
			
		
	
	
}
void CalAppPhaseRow::phasingModeFromBin(EndianIStream& eis) {
		
	
	
		
			
		phasingMode =  eis.readString();
			
		
	
	
}
void CalAppPhaseRow::numPhasedAntennasFromBin(EndianIStream& eis) {
		
	
	
		
			
		numPhasedAntennas =  eis.readInt();
			
		
	
	
}
void CalAppPhaseRow::phasedAntennasFromBin(EndianIStream& eis) {
		
	
	
		
			
	
		phasedAntennas.clear();
		
		unsigned int phasedAntennasDim1 = eis.readInt();
		for (unsigned int  i = 0 ; i < phasedAntennasDim1; i++)
			
			phasedAntennas.push_back(eis.readString());
			
	

		
	
	
}
void CalAppPhaseRow::refAntennaIndexFromBin(EndianIStream& eis) {
		
	
	
		
			
		refAntennaIndex =  eis.readInt();
			
		
	
	
}
void CalAppPhaseRow::candRefAntennaIndexFromBin(EndianIStream& eis) {
		
	
	
		
			
		candRefAntennaIndex =  eis.readInt();
			
		
	
	
}
void CalAppPhaseRow::phasePackingFromBin(EndianIStream& eis) {
		
	
	
		
			
		phasePacking =  eis.readString();
			
		
	
	
}
void CalAppPhaseRow::numReceptorsFromBin(EndianIStream& eis) {
		
	
	
		
			
		numReceptors =  eis.readInt();
			
		
	
	
}
void CalAppPhaseRow::numChannelsFromBin(EndianIStream& eis) {
		
	
	
		
			
		numChannels =  eis.readInt();
			
		
	
	
}
void CalAppPhaseRow::numPhaseValuesFromBin(EndianIStream& eis) {
		
	
	
		
			
		numPhaseValues =  eis.readInt();
			
		
	
	
}
void CalAppPhaseRow::phaseValuesFromBin(EndianIStream& eis) {
		
	
	
		
			
	
		phaseValues.clear();
		
		unsigned int phaseValuesDim1 = eis.readInt();
		for (unsigned int  i = 0 ; i < phaseValuesDim1; i++)
			
			phaseValues.push_back(eis.readFloat());
			
	

		
	
	
}
void CalAppPhaseRow::numCompareFromBin(EndianIStream& eis) {
		
	
	
		
			
		numCompare =  eis.readInt();
			
		
	
	
}
void CalAppPhaseRow::numEfficienciesFromBin(EndianIStream& eis) {
		
	
	
		
			
		numEfficiencies =  eis.readInt();
			
		
	
	
}
void CalAppPhaseRow::compareArrayFromBin(EndianIStream& eis) {
		
	
	
		
			
	
		compareArray.clear();
		
		unsigned int compareArrayDim1 = eis.readInt();
		for (unsigned int  i = 0 ; i < compareArrayDim1; i++)
			
			compareArray.push_back(eis.readString());
			
	

		
	
	
}
void CalAppPhaseRow::efficiencyIndicesFromBin(EndianIStream& eis) {
		
	
	
		
			
	
		efficiencyIndices.clear();
		
		unsigned int efficiencyIndicesDim1 = eis.readInt();
		for (unsigned int  i = 0 ; i < efficiencyIndicesDim1; i++)
			
			efficiencyIndices.push_back(eis.readInt());
			
	

		
	
	
}
void CalAppPhaseRow::efficienciesFromBin(EndianIStream& eis) {
		
	
	
		
			
	
		efficiencies.clear();
		
		unsigned int efficienciesDim1 = eis.readInt();
		unsigned int efficienciesDim2 = eis.readInt();
        
		vector <float> efficienciesAux1;
        
		for (unsigned int i = 0; i < efficienciesDim1; i++) {
			efficienciesAux1.clear();
			for (unsigned int j = 0; j < efficienciesDim2 ; j++)			
			
			efficienciesAux1.push_back(eis.readFloat());
			
			efficiencies.push_back(efficienciesAux1);
		}
	
	

		
	
	
}
void CalAppPhaseRow::qualityFromBin(EndianIStream& eis) {
		
	
	
		
			
	
		quality.clear();
		
		unsigned int qualityDim1 = eis.readInt();
		for (unsigned int  i = 0 ; i < qualityDim1; i++)
			
			quality.push_back(eis.readFloat());
			
	

		
	
	
}
void CalAppPhaseRow::phasedSumAntennaFromBin(EndianIStream& eis) {
		
	
	
		
			
		phasedSumAntenna =  eis.readString();
			
		
	
	
}

void CalAppPhaseRow::typeSupportsFromBin(EndianIStream& eis) {
		
	typeSupportsExists = eis.readBoolean();
	if (typeSupportsExists) {
		
	
	
		
			
		typeSupports =  eis.readString();
			
		
	

	}
	
}
void CalAppPhaseRow::numSupportsFromBin(EndianIStream& eis) {
		
	numSupportsExists = eis.readBoolean();
	if (numSupportsExists) {
		
	
	
		
			
		numSupports =  eis.readInt();
			
		
	

	}
	
}
void CalAppPhaseRow::phaseSupportsFromBin(EndianIStream& eis) {
		
	phaseSupportsExists = eis.readBoolean();
	if (phaseSupportsExists) {
		
	
	
		
			
	
		phaseSupports.clear();
		
		unsigned int phaseSupportsDim1 = eis.readInt();
		for (unsigned int  i = 0 ; i < phaseSupportsDim1; i++)
			
			phaseSupports.push_back(eis.readFloat());
			
	

		
	

	}
	
}
	
	
	CalAppPhaseRow* CalAppPhaseRow::fromBin(EndianIStream& eis, CalAppPhaseTable& table, const vector<string>& attributesSeq) {
		CalAppPhaseRow* row = new  CalAppPhaseRow(table);
		
		map<string, CalAppPhaseAttributeFromBin>::iterator iter ;
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
					throw ConversionException("There is not method to read an attribute '"+attributesSeq.at(i)+"'.", "CalAppPhaseTable");
			}
				
		}				
		return row;
	}

	//
	// A collection of methods to set the value of the attributes from their textual value in the XML representation
	// of one row.
	//
	
	// Convert a string into an BasebandName 
	void CalAppPhaseRow::basebandNameFromText(const string & s) {
		 
          
		basebandName = ASDMValuesParser::parse<BasebandNameMod::BasebandName>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void CalAppPhaseRow::scanNumberFromText(const string & s) {
		 
          
		scanNumber = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	// Convert a string into an Tag 
	void CalAppPhaseRow::calDataIdFromText(const string & s) {
		 
          
		calDataId = ASDMValuesParser::parse<Tag>(s);
          
		
	}
	
	
	// Convert a string into an Tag 
	void CalAppPhaseRow::calReductionIdFromText(const string & s) {
		 
          
		calReductionId = ASDMValuesParser::parse<Tag>(s);
          
		
	}
	
	
	// Convert a string into an ArrayTime 
	void CalAppPhaseRow::startValidTimeFromText(const string & s) {
		 
          
		startValidTime = ASDMValuesParser::parse<ArrayTime>(s);
          
		
	}
	
	
	// Convert a string into an ArrayTime 
	void CalAppPhaseRow::endValidTimeFromText(const string & s) {
		 
          
		endValidTime = ASDMValuesParser::parse<ArrayTime>(s);
          
		
	}
	
	
	// Convert a string into an ArrayTime 
	void CalAppPhaseRow::adjustTimeFromText(const string & s) {
		 
          
		adjustTime = ASDMValuesParser::parse<ArrayTime>(s);
          
		
	}
	
	
	// Convert a string into an String 
	void CalAppPhaseRow::adjustTokenFromText(const string & s) {
		 
          
		adjustToken = ASDMValuesParser::parse<string>(s);
          
		
	}
	
	
	// Convert a string into an String 
	void CalAppPhaseRow::phasingModeFromText(const string & s) {
		 
          
		phasingMode = ASDMValuesParser::parse<string>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void CalAppPhaseRow::numPhasedAntennasFromText(const string & s) {
		 
          
		numPhasedAntennas = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	// Convert a string into an String 
	void CalAppPhaseRow::phasedAntennasFromText(const string & s) {
		 
          
		phasedAntennas = ASDMValuesParser::parse1D<string>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void CalAppPhaseRow::refAntennaIndexFromText(const string & s) {
		 
          
		refAntennaIndex = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void CalAppPhaseRow::candRefAntennaIndexFromText(const string & s) {
		 
          
		candRefAntennaIndex = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	// Convert a string into an String 
	void CalAppPhaseRow::phasePackingFromText(const string & s) {
		 
          
		phasePacking = ASDMValuesParser::parse<string>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void CalAppPhaseRow::numReceptorsFromText(const string & s) {
		 
          
		numReceptors = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void CalAppPhaseRow::numChannelsFromText(const string & s) {
		 
          
		numChannels = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void CalAppPhaseRow::numPhaseValuesFromText(const string & s) {
		 
          
		numPhaseValues = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	// Convert a string into an float 
	void CalAppPhaseRow::phaseValuesFromText(const string & s) {
		 
          
		phaseValues = ASDMValuesParser::parse1D<float>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void CalAppPhaseRow::numCompareFromText(const string & s) {
		 
          
		numCompare = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void CalAppPhaseRow::numEfficienciesFromText(const string & s) {
		 
          
		numEfficiencies = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	// Convert a string into an String 
	void CalAppPhaseRow::compareArrayFromText(const string & s) {
		 
          
		compareArray = ASDMValuesParser::parse1D<string>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void CalAppPhaseRow::efficiencyIndicesFromText(const string & s) {
		 
          
		efficiencyIndices = ASDMValuesParser::parse1D<int>(s);
          
		
	}
	
	
	// Convert a string into an float 
	void CalAppPhaseRow::efficienciesFromText(const string & s) {
		 
          
		efficiencies = ASDMValuesParser::parse2D<float>(s);
          
		
	}
	
	
	// Convert a string into an float 
	void CalAppPhaseRow::qualityFromText(const string & s) {
		 
          
		quality = ASDMValuesParser::parse1D<float>(s);
          
		
	}
	
	
	// Convert a string into an String 
	void CalAppPhaseRow::phasedSumAntennaFromText(const string & s) {
		 
          
		phasedSumAntenna = ASDMValuesParser::parse<string>(s);
          
		
	}
	

	
	// Convert a string into an String 
	void CalAppPhaseRow::typeSupportsFromText(const string & s) {
		typeSupportsExists = true;
		 
          
		typeSupports = ASDMValuesParser::parse<string>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void CalAppPhaseRow::numSupportsFromText(const string & s) {
		numSupportsExists = true;
		 
          
		numSupports = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	// Convert a string into an float 
	void CalAppPhaseRow::phaseSupportsFromText(const string & s) {
		phaseSupportsExists = true;
		 
          
		phaseSupports = ASDMValuesParser::parse1D<float>(s);
          
		
	}
	
	
	
	void CalAppPhaseRow::fromText(const std::string& attributeName, const std::string&  t) {
		map<string, CalAppPhaseAttributeFromText>::iterator iter;
		if ((iter = fromTextMethods.find(attributeName)) == fromTextMethods.end())
			throw ConversionException("I do not know what to do with '"+attributeName+"' and its content '"+t+"' (while parsing an XML document)", "CalAppPhaseTable");
		(this->*(iter->second))(t);
	}
			
	////////////////////////////////////////////////
	// Intrinsic Table Attributes getters/setters //
	////////////////////////////////////////////////
	
	

	
 	/**
 	 * Get basebandName.
 	 * @return basebandName as BasebandNameMod::BasebandName
 	 */
 	BasebandNameMod::BasebandName CalAppPhaseRow::getBasebandName() const {
	
  		return basebandName;
 	}

 	/**
 	 * Set basebandName with the specified BasebandNameMod::BasebandName.
 	 * @param basebandName The BasebandNameMod::BasebandName value to which basebandName is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void CalAppPhaseRow::setBasebandName (BasebandNameMod::BasebandName basebandName)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("basebandName", "CalAppPhase");
		
  		}
  	
 		this->basebandName = basebandName;
	
 	}
	
	

	

	
 	/**
 	 * Get scanNumber.
 	 * @return scanNumber as int
 	 */
 	int CalAppPhaseRow::getScanNumber() const {
	
  		return scanNumber;
 	}

 	/**
 	 * Set scanNumber with the specified int.
 	 * @param scanNumber The int value to which scanNumber is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void CalAppPhaseRow::setScanNumber (int scanNumber)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("scanNumber", "CalAppPhase");
		
  		}
  	
 		this->scanNumber = scanNumber;
	
 	}
	
	

	

	
 	/**
 	 * Get startValidTime.
 	 * @return startValidTime as ArrayTime
 	 */
 	ArrayTime CalAppPhaseRow::getStartValidTime() const {
	
  		return startValidTime;
 	}

 	/**
 	 * Set startValidTime with the specified ArrayTime.
 	 * @param startValidTime The ArrayTime value to which startValidTime is to be set.
 	 
 	
 		
 	 */
 	void CalAppPhaseRow::setStartValidTime (ArrayTime startValidTime)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->startValidTime = startValidTime;
	
 	}
	
	

	

	
 	/**
 	 * Get endValidTime.
 	 * @return endValidTime as ArrayTime
 	 */
 	ArrayTime CalAppPhaseRow::getEndValidTime() const {
	
  		return endValidTime;
 	}

 	/**
 	 * Set endValidTime with the specified ArrayTime.
 	 * @param endValidTime The ArrayTime value to which endValidTime is to be set.
 	 
 	
 		
 	 */
 	void CalAppPhaseRow::setEndValidTime (ArrayTime endValidTime)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->endValidTime = endValidTime;
	
 	}
	
	

	

	
 	/**
 	 * Get adjustTime.
 	 * @return adjustTime as ArrayTime
 	 */
 	ArrayTime CalAppPhaseRow::getAdjustTime() const {
	
  		return adjustTime;
 	}

 	/**
 	 * Set adjustTime with the specified ArrayTime.
 	 * @param adjustTime The ArrayTime value to which adjustTime is to be set.
 	 
 	
 		
 	 */
 	void CalAppPhaseRow::setAdjustTime (ArrayTime adjustTime)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->adjustTime = adjustTime;
	
 	}
	
	

	

	
 	/**
 	 * Get adjustToken.
 	 * @return adjustToken as std::string
 	 */
 	std::string CalAppPhaseRow::getAdjustToken() const {
	
  		return adjustToken;
 	}

 	/**
 	 * Set adjustToken with the specified std::string.
 	 * @param adjustToken The std::string value to which adjustToken is to be set.
 	 
 	
 		
 	 */
 	void CalAppPhaseRow::setAdjustToken (std::string adjustToken)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->adjustToken = adjustToken;
	
 	}
	
	

	

	
 	/**
 	 * Get phasingMode.
 	 * @return phasingMode as std::string
 	 */
 	std::string CalAppPhaseRow::getPhasingMode() const {
	
  		return phasingMode;
 	}

 	/**
 	 * Set phasingMode with the specified std::string.
 	 * @param phasingMode The std::string value to which phasingMode is to be set.
 	 
 	
 		
 	 */
 	void CalAppPhaseRow::setPhasingMode (std::string phasingMode)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->phasingMode = phasingMode;
	
 	}
	
	

	

	
 	/**
 	 * Get numPhasedAntennas.
 	 * @return numPhasedAntennas as int
 	 */
 	int CalAppPhaseRow::getNumPhasedAntennas() const {
	
  		return numPhasedAntennas;
 	}

 	/**
 	 * Set numPhasedAntennas with the specified int.
 	 * @param numPhasedAntennas The int value to which numPhasedAntennas is to be set.
 	 
 	
 		
 	 */
 	void CalAppPhaseRow::setNumPhasedAntennas (int numPhasedAntennas)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->numPhasedAntennas = numPhasedAntennas;
	
 	}
	
	

	

	
 	/**
 	 * Get phasedAntennas.
 	 * @return phasedAntennas as std::vector<std::string >
 	 */
 	std::vector<std::string > CalAppPhaseRow::getPhasedAntennas() const {
	
  		return phasedAntennas;
 	}

 	/**
 	 * Set phasedAntennas with the specified std::vector<std::string >.
 	 * @param phasedAntennas The std::vector<std::string > value to which phasedAntennas is to be set.
 	 
 	
 		
 	 */
 	void CalAppPhaseRow::setPhasedAntennas (std::vector<std::string > phasedAntennas)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->phasedAntennas = phasedAntennas;
	
 	}
	
	

	

	
 	/**
 	 * Get refAntennaIndex.
 	 * @return refAntennaIndex as int
 	 */
 	int CalAppPhaseRow::getRefAntennaIndex() const {
	
  		return refAntennaIndex;
 	}

 	/**
 	 * Set refAntennaIndex with the specified int.
 	 * @param refAntennaIndex The int value to which refAntennaIndex is to be set.
 	 
 	
 		
 	 */
 	void CalAppPhaseRow::setRefAntennaIndex (int refAntennaIndex)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->refAntennaIndex = refAntennaIndex;
	
 	}
	
	

	

	
 	/**
 	 * Get candRefAntennaIndex.
 	 * @return candRefAntennaIndex as int
 	 */
 	int CalAppPhaseRow::getCandRefAntennaIndex() const {
	
  		return candRefAntennaIndex;
 	}

 	/**
 	 * Set candRefAntennaIndex with the specified int.
 	 * @param candRefAntennaIndex The int value to which candRefAntennaIndex is to be set.
 	 
 	
 		
 	 */
 	void CalAppPhaseRow::setCandRefAntennaIndex (int candRefAntennaIndex)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->candRefAntennaIndex = candRefAntennaIndex;
	
 	}
	
	

	

	
 	/**
 	 * Get phasePacking.
 	 * @return phasePacking as std::string
 	 */
 	std::string CalAppPhaseRow::getPhasePacking() const {
	
  		return phasePacking;
 	}

 	/**
 	 * Set phasePacking with the specified std::string.
 	 * @param phasePacking The std::string value to which phasePacking is to be set.
 	 
 	
 		
 	 */
 	void CalAppPhaseRow::setPhasePacking (std::string phasePacking)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->phasePacking = phasePacking;
	
 	}
	
	

	

	
 	/**
 	 * Get numReceptors.
 	 * @return numReceptors as int
 	 */
 	int CalAppPhaseRow::getNumReceptors() const {
	
  		return numReceptors;
 	}

 	/**
 	 * Set numReceptors with the specified int.
 	 * @param numReceptors The int value to which numReceptors is to be set.
 	 
 	
 		
 	 */
 	void CalAppPhaseRow::setNumReceptors (int numReceptors)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->numReceptors = numReceptors;
	
 	}
	
	

	

	
 	/**
 	 * Get numChannels.
 	 * @return numChannels as int
 	 */
 	int CalAppPhaseRow::getNumChannels() const {
	
  		return numChannels;
 	}

 	/**
 	 * Set numChannels with the specified int.
 	 * @param numChannels The int value to which numChannels is to be set.
 	 
 	
 		
 	 */
 	void CalAppPhaseRow::setNumChannels (int numChannels)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->numChannels = numChannels;
	
 	}
	
	

	

	
 	/**
 	 * Get numPhaseValues.
 	 * @return numPhaseValues as int
 	 */
 	int CalAppPhaseRow::getNumPhaseValues() const {
	
  		return numPhaseValues;
 	}

 	/**
 	 * Set numPhaseValues with the specified int.
 	 * @param numPhaseValues The int value to which numPhaseValues is to be set.
 	 
 	
 		
 	 */
 	void CalAppPhaseRow::setNumPhaseValues (int numPhaseValues)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->numPhaseValues = numPhaseValues;
	
 	}
	
	

	

	
 	/**
 	 * Get phaseValues.
 	 * @return phaseValues as std::vector<float >
 	 */
 	std::vector<float > CalAppPhaseRow::getPhaseValues() const {
	
  		return phaseValues;
 	}

 	/**
 	 * Set phaseValues with the specified std::vector<float >.
 	 * @param phaseValues The std::vector<float > value to which phaseValues is to be set.
 	 
 	
 		
 	 */
 	void CalAppPhaseRow::setPhaseValues (std::vector<float > phaseValues)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->phaseValues = phaseValues;
	
 	}
	
	

	

	
 	/**
 	 * Get numCompare.
 	 * @return numCompare as int
 	 */
 	int CalAppPhaseRow::getNumCompare() const {
	
  		return numCompare;
 	}

 	/**
 	 * Set numCompare with the specified int.
 	 * @param numCompare The int value to which numCompare is to be set.
 	 
 	
 		
 	 */
 	void CalAppPhaseRow::setNumCompare (int numCompare)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->numCompare = numCompare;
	
 	}
	
	

	

	
 	/**
 	 * Get numEfficiencies.
 	 * @return numEfficiencies as int
 	 */
 	int CalAppPhaseRow::getNumEfficiencies() const {
	
  		return numEfficiencies;
 	}

 	/**
 	 * Set numEfficiencies with the specified int.
 	 * @param numEfficiencies The int value to which numEfficiencies is to be set.
 	 
 	
 		
 	 */
 	void CalAppPhaseRow::setNumEfficiencies (int numEfficiencies)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->numEfficiencies = numEfficiencies;
	
 	}
	
	

	

	
 	/**
 	 * Get compareArray.
 	 * @return compareArray as std::vector<std::string >
 	 */
 	std::vector<std::string > CalAppPhaseRow::getCompareArray() const {
	
  		return compareArray;
 	}

 	/**
 	 * Set compareArray with the specified std::vector<std::string >.
 	 * @param compareArray The std::vector<std::string > value to which compareArray is to be set.
 	 
 	
 		
 	 */
 	void CalAppPhaseRow::setCompareArray (std::vector<std::string > compareArray)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->compareArray = compareArray;
	
 	}
	
	

	

	
 	/**
 	 * Get efficiencyIndices.
 	 * @return efficiencyIndices as std::vector<int >
 	 */
 	std::vector<int > CalAppPhaseRow::getEfficiencyIndices() const {
	
  		return efficiencyIndices;
 	}

 	/**
 	 * Set efficiencyIndices with the specified std::vector<int >.
 	 * @param efficiencyIndices The std::vector<int > value to which efficiencyIndices is to be set.
 	 
 	
 		
 	 */
 	void CalAppPhaseRow::setEfficiencyIndices (std::vector<int > efficiencyIndices)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->efficiencyIndices = efficiencyIndices;
	
 	}
	
	

	

	
 	/**
 	 * Get efficiencies.
 	 * @return efficiencies as std::vector<std::vector<float > >
 	 */
 	std::vector<std::vector<float > > CalAppPhaseRow::getEfficiencies() const {
	
  		return efficiencies;
 	}

 	/**
 	 * Set efficiencies with the specified std::vector<std::vector<float > >.
 	 * @param efficiencies The std::vector<std::vector<float > > value to which efficiencies is to be set.
 	 
 	
 		
 	 */
 	void CalAppPhaseRow::setEfficiencies (std::vector<std::vector<float > > efficiencies)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->efficiencies = efficiencies;
	
 	}
	
	

	

	
 	/**
 	 * Get quality.
 	 * @return quality as std::vector<float >
 	 */
 	std::vector<float > CalAppPhaseRow::getQuality() const {
	
  		return quality;
 	}

 	/**
 	 * Set quality with the specified std::vector<float >.
 	 * @param quality The std::vector<float > value to which quality is to be set.
 	 
 	
 		
 	 */
 	void CalAppPhaseRow::setQuality (std::vector<float > quality)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->quality = quality;
	
 	}
	
	

	

	
 	/**
 	 * Get phasedSumAntenna.
 	 * @return phasedSumAntenna as std::string
 	 */
 	std::string CalAppPhaseRow::getPhasedSumAntenna() const {
	
  		return phasedSumAntenna;
 	}

 	/**
 	 * Set phasedSumAntenna with the specified std::string.
 	 * @param phasedSumAntenna The std::string value to which phasedSumAntenna is to be set.
 	 
 	
 		
 	 */
 	void CalAppPhaseRow::setPhasedSumAntenna (std::string phasedSumAntenna)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->phasedSumAntenna = phasedSumAntenna;
	
 	}
	
	

	
	/**
	 * The attribute typeSupports is optional. Return true if this attribute exists.
	 * @return true if and only if the typeSupports attribute exists. 
	 */
	bool CalAppPhaseRow::isTypeSupportsExists() const {
		return typeSupportsExists;
	}
	

	
 	/**
 	 * Get typeSupports, which is optional.
 	 * @return typeSupports as std::string
 	 * @throw IllegalAccessException If typeSupports does not exist.
 	 */
 	std::string CalAppPhaseRow::getTypeSupports() const  {
		if (!typeSupportsExists) {
			throw IllegalAccessException("typeSupports", "CalAppPhase");
		}
	
  		return typeSupports;
 	}

 	/**
 	 * Set typeSupports with the specified std::string.
 	 * @param typeSupports The std::string value to which typeSupports is to be set.
 	 
 	
 	 */
 	void CalAppPhaseRow::setTypeSupports (std::string typeSupports) {
	
 		this->typeSupports = typeSupports;
	
		typeSupportsExists = true;
	
 	}
	
	
	/**
	 * Mark typeSupports, which is an optional field, as non-existent.
	 */
	void CalAppPhaseRow::clearTypeSupports () {
		typeSupportsExists = false;
	}
	

	
	/**
	 * The attribute numSupports is optional. Return true if this attribute exists.
	 * @return true if and only if the numSupports attribute exists. 
	 */
	bool CalAppPhaseRow::isNumSupportsExists() const {
		return numSupportsExists;
	}
	

	
 	/**
 	 * Get numSupports, which is optional.
 	 * @return numSupports as int
 	 * @throw IllegalAccessException If numSupports does not exist.
 	 */
 	int CalAppPhaseRow::getNumSupports() const  {
		if (!numSupportsExists) {
			throw IllegalAccessException("numSupports", "CalAppPhase");
		}
	
  		return numSupports;
 	}

 	/**
 	 * Set numSupports with the specified int.
 	 * @param numSupports The int value to which numSupports is to be set.
 	 
 	
 	 */
 	void CalAppPhaseRow::setNumSupports (int numSupports) {
	
 		this->numSupports = numSupports;
	
		numSupportsExists = true;
	
 	}
	
	
	/**
	 * Mark numSupports, which is an optional field, as non-existent.
	 */
	void CalAppPhaseRow::clearNumSupports () {
		numSupportsExists = false;
	}
	

	
	/**
	 * The attribute phaseSupports is optional. Return true if this attribute exists.
	 * @return true if and only if the phaseSupports attribute exists. 
	 */
	bool CalAppPhaseRow::isPhaseSupportsExists() const {
		return phaseSupportsExists;
	}
	

	
 	/**
 	 * Get phaseSupports, which is optional.
 	 * @return phaseSupports as std::vector<float >
 	 * @throw IllegalAccessException If phaseSupports does not exist.
 	 */
 	std::vector<float > CalAppPhaseRow::getPhaseSupports() const  {
		if (!phaseSupportsExists) {
			throw IllegalAccessException("phaseSupports", "CalAppPhase");
		}
	
  		return phaseSupports;
 	}

 	/**
 	 * Set phaseSupports with the specified std::vector<float >.
 	 * @param phaseSupports The std::vector<float > value to which phaseSupports is to be set.
 	 
 	
 	 */
 	void CalAppPhaseRow::setPhaseSupports (std::vector<float > phaseSupports) {
	
 		this->phaseSupports = phaseSupports;
	
		phaseSupportsExists = true;
	
 	}
	
	
	/**
	 * Mark phaseSupports, which is an optional field, as non-existent.
	 */
	void CalAppPhaseRow::clearPhaseSupports () {
		phaseSupportsExists = false;
	}
	

	
	///////////////////////////////////////////////
	// Extrinsic Table Attributes getters/setters//
	///////////////////////////////////////////////
	
	

	
 	/**
 	 * Get calDataId.
 	 * @return calDataId as Tag
 	 */
 	Tag CalAppPhaseRow::getCalDataId() const {
	
  		return calDataId;
 	}

 	/**
 	 * Set calDataId with the specified Tag.
 	 * @param calDataId The Tag value to which calDataId is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void CalAppPhaseRow::setCalDataId (Tag calDataId)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("calDataId", "CalAppPhase");
		
  		}
  	
 		this->calDataId = calDataId;
	
 	}
	
	

	

	
 	/**
 	 * Get calReductionId.
 	 * @return calReductionId as Tag
 	 */
 	Tag CalAppPhaseRow::getCalReductionId() const {
	
  		return calReductionId;
 	}

 	/**
 	 * Set calReductionId with the specified Tag.
 	 * @param calReductionId The Tag value to which calReductionId is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void CalAppPhaseRow::setCalReductionId (Tag calReductionId)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("calReductionId", "CalAppPhase");
		
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
	 CalDataRow* CalAppPhaseRow::getCalDataUsingCalDataId() {
	 
	 	return table.getContainer().getCalData().getRowByKey(calDataId);
	 }
	 

	

	
	
	
		

	/**
	 * Returns the pointer to the row in the CalReduction table having CalReduction.calReductionId == calReductionId
	 * @return a CalReductionRow*
	 * 
	 
	 */
	 CalReductionRow* CalAppPhaseRow::getCalReductionUsingCalReductionId() {
	 
	 	return table.getContainer().getCalReduction().getRowByKey(calReductionId);
	 }
	 

	

	
	/**
	 * Create a CalAppPhaseRow.
	 * <p>
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 */ 
	CalAppPhaseRow::CalAppPhaseRow (CalAppPhaseTable &t) : table(t) {
		hasBeenAdded = false;
		
	
	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	
		typeSupportsExists = false;
	

	
		numSupportsExists = false;
	

	
		phaseSupportsExists = false;
	

	
	

	

	
	
	
	
// This attribute is scalar and has an enumeration type. Let's initialize it to some valid value (the 1st of the enumeration).		
basebandName = CBasebandName::from_int(0);
	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	
	
	 fromBinMethods["basebandName"] = &CalAppPhaseRow::basebandNameFromBin; 
	 fromBinMethods["scanNumber"] = &CalAppPhaseRow::scanNumberFromBin; 
	 fromBinMethods["calDataId"] = &CalAppPhaseRow::calDataIdFromBin; 
	 fromBinMethods["calReductionId"] = &CalAppPhaseRow::calReductionIdFromBin; 
	 fromBinMethods["startValidTime"] = &CalAppPhaseRow::startValidTimeFromBin; 
	 fromBinMethods["endValidTime"] = &CalAppPhaseRow::endValidTimeFromBin; 
	 fromBinMethods["adjustTime"] = &CalAppPhaseRow::adjustTimeFromBin; 
	 fromBinMethods["adjustToken"] = &CalAppPhaseRow::adjustTokenFromBin; 
	 fromBinMethods["phasingMode"] = &CalAppPhaseRow::phasingModeFromBin; 
	 fromBinMethods["numPhasedAntennas"] = &CalAppPhaseRow::numPhasedAntennasFromBin; 
	 fromBinMethods["phasedAntennas"] = &CalAppPhaseRow::phasedAntennasFromBin; 
	 fromBinMethods["refAntennaIndex"] = &CalAppPhaseRow::refAntennaIndexFromBin; 
	 fromBinMethods["candRefAntennaIndex"] = &CalAppPhaseRow::candRefAntennaIndexFromBin; 
	 fromBinMethods["phasePacking"] = &CalAppPhaseRow::phasePackingFromBin; 
	 fromBinMethods["numReceptors"] = &CalAppPhaseRow::numReceptorsFromBin; 
	 fromBinMethods["numChannels"] = &CalAppPhaseRow::numChannelsFromBin; 
	 fromBinMethods["numPhaseValues"] = &CalAppPhaseRow::numPhaseValuesFromBin; 
	 fromBinMethods["phaseValues"] = &CalAppPhaseRow::phaseValuesFromBin; 
	 fromBinMethods["numCompare"] = &CalAppPhaseRow::numCompareFromBin; 
	 fromBinMethods["numEfficiencies"] = &CalAppPhaseRow::numEfficienciesFromBin; 
	 fromBinMethods["compareArray"] = &CalAppPhaseRow::compareArrayFromBin; 
	 fromBinMethods["efficiencyIndices"] = &CalAppPhaseRow::efficiencyIndicesFromBin; 
	 fromBinMethods["efficiencies"] = &CalAppPhaseRow::efficienciesFromBin; 
	 fromBinMethods["quality"] = &CalAppPhaseRow::qualityFromBin; 
	 fromBinMethods["phasedSumAntenna"] = &CalAppPhaseRow::phasedSumAntennaFromBin; 
		
	
	 fromBinMethods["typeSupports"] = &CalAppPhaseRow::typeSupportsFromBin; 
	 fromBinMethods["numSupports"] = &CalAppPhaseRow::numSupportsFromBin; 
	 fromBinMethods["phaseSupports"] = &CalAppPhaseRow::phaseSupportsFromBin; 
	
	
	
	
				 
	fromTextMethods["basebandName"] = &CalAppPhaseRow::basebandNameFromText;
		 
	
				 
	fromTextMethods["scanNumber"] = &CalAppPhaseRow::scanNumberFromText;
		 
	
				 
	fromTextMethods["calDataId"] = &CalAppPhaseRow::calDataIdFromText;
		 
	
				 
	fromTextMethods["calReductionId"] = &CalAppPhaseRow::calReductionIdFromText;
		 
	
				 
	fromTextMethods["startValidTime"] = &CalAppPhaseRow::startValidTimeFromText;
		 
	
				 
	fromTextMethods["endValidTime"] = &CalAppPhaseRow::endValidTimeFromText;
		 
	
				 
	fromTextMethods["adjustTime"] = &CalAppPhaseRow::adjustTimeFromText;
		 
	
				 
	fromTextMethods["adjustToken"] = &CalAppPhaseRow::adjustTokenFromText;
		 
	
				 
	fromTextMethods["phasingMode"] = &CalAppPhaseRow::phasingModeFromText;
		 
	
				 
	fromTextMethods["numPhasedAntennas"] = &CalAppPhaseRow::numPhasedAntennasFromText;
		 
	
				 
	fromTextMethods["phasedAntennas"] = &CalAppPhaseRow::phasedAntennasFromText;
		 
	
				 
	fromTextMethods["refAntennaIndex"] = &CalAppPhaseRow::refAntennaIndexFromText;
		 
	
				 
	fromTextMethods["candRefAntennaIndex"] = &CalAppPhaseRow::candRefAntennaIndexFromText;
		 
	
				 
	fromTextMethods["phasePacking"] = &CalAppPhaseRow::phasePackingFromText;
		 
	
				 
	fromTextMethods["numReceptors"] = &CalAppPhaseRow::numReceptorsFromText;
		 
	
				 
	fromTextMethods["numChannels"] = &CalAppPhaseRow::numChannelsFromText;
		 
	
				 
	fromTextMethods["numPhaseValues"] = &CalAppPhaseRow::numPhaseValuesFromText;
		 
	
				 
	fromTextMethods["phaseValues"] = &CalAppPhaseRow::phaseValuesFromText;
		 
	
				 
	fromTextMethods["numCompare"] = &CalAppPhaseRow::numCompareFromText;
		 
	
				 
	fromTextMethods["numEfficiencies"] = &CalAppPhaseRow::numEfficienciesFromText;
		 
	
				 
	fromTextMethods["compareArray"] = &CalAppPhaseRow::compareArrayFromText;
		 
	
				 
	fromTextMethods["efficiencyIndices"] = &CalAppPhaseRow::efficiencyIndicesFromText;
		 
	
				 
	fromTextMethods["efficiencies"] = &CalAppPhaseRow::efficienciesFromText;
		 
	
				 
	fromTextMethods["quality"] = &CalAppPhaseRow::qualityFromText;
		 
	
				 
	fromTextMethods["phasedSumAntenna"] = &CalAppPhaseRow::phasedSumAntennaFromText;
		 
	

	 
				
	fromTextMethods["typeSupports"] = &CalAppPhaseRow::typeSupportsFromText;
		 	
	 
				
	fromTextMethods["numSupports"] = &CalAppPhaseRow::numSupportsFromText;
		 	
	 
				
	fromTextMethods["phaseSupports"] = &CalAppPhaseRow::phaseSupportsFromText;
		 	
		
	}
	
	CalAppPhaseRow::CalAppPhaseRow (CalAppPhaseTable &t, CalAppPhaseRow *row) : table(t) {
		hasBeenAdded = false;
		
		if (row == 0) {
	
	
	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	
		typeSupportsExists = false;
	

	
		numSupportsExists = false;
	

	
		phaseSupportsExists = false;
	

	
	

	
		
		}
		else {
	
		
			basebandName = row->basebandName;
		
			scanNumber = row->scanNumber;
		
			calDataId = row->calDataId;
		
			calReductionId = row->calReductionId;
		
		
		
		
			startValidTime = row->startValidTime;
		
			endValidTime = row->endValidTime;
		
			adjustTime = row->adjustTime;
		
			adjustToken = row->adjustToken;
		
			phasingMode = row->phasingMode;
		
			numPhasedAntennas = row->numPhasedAntennas;
		
			phasedAntennas = row->phasedAntennas;
		
			refAntennaIndex = row->refAntennaIndex;
		
			candRefAntennaIndex = row->candRefAntennaIndex;
		
			phasePacking = row->phasePacking;
		
			numReceptors = row->numReceptors;
		
			numChannels = row->numChannels;
		
			numPhaseValues = row->numPhaseValues;
		
			phaseValues = row->phaseValues;
		
			numCompare = row->numCompare;
		
			numEfficiencies = row->numEfficiencies;
		
			compareArray = row->compareArray;
		
			efficiencyIndices = row->efficiencyIndices;
		
			efficiencies = row->efficiencies;
		
			quality = row->quality;
		
			phasedSumAntenna = row->phasedSumAntenna;
		
		
		
		
		if (row->typeSupportsExists) {
			typeSupports = row->typeSupports;		
			typeSupportsExists = true;
		}
		else
			typeSupportsExists = false;
		
		if (row->numSupportsExists) {
			numSupports = row->numSupports;		
			numSupportsExists = true;
		}
		else
			numSupportsExists = false;
		
		if (row->phaseSupportsExists) {
			phaseSupports = row->phaseSupports;		
			phaseSupportsExists = true;
		}
		else
			phaseSupportsExists = false;
		
		}
		
		 fromBinMethods["basebandName"] = &CalAppPhaseRow::basebandNameFromBin; 
		 fromBinMethods["scanNumber"] = &CalAppPhaseRow::scanNumberFromBin; 
		 fromBinMethods["calDataId"] = &CalAppPhaseRow::calDataIdFromBin; 
		 fromBinMethods["calReductionId"] = &CalAppPhaseRow::calReductionIdFromBin; 
		 fromBinMethods["startValidTime"] = &CalAppPhaseRow::startValidTimeFromBin; 
		 fromBinMethods["endValidTime"] = &CalAppPhaseRow::endValidTimeFromBin; 
		 fromBinMethods["adjustTime"] = &CalAppPhaseRow::adjustTimeFromBin; 
		 fromBinMethods["adjustToken"] = &CalAppPhaseRow::adjustTokenFromBin; 
		 fromBinMethods["phasingMode"] = &CalAppPhaseRow::phasingModeFromBin; 
		 fromBinMethods["numPhasedAntennas"] = &CalAppPhaseRow::numPhasedAntennasFromBin; 
		 fromBinMethods["phasedAntennas"] = &CalAppPhaseRow::phasedAntennasFromBin; 
		 fromBinMethods["refAntennaIndex"] = &CalAppPhaseRow::refAntennaIndexFromBin; 
		 fromBinMethods["candRefAntennaIndex"] = &CalAppPhaseRow::candRefAntennaIndexFromBin; 
		 fromBinMethods["phasePacking"] = &CalAppPhaseRow::phasePackingFromBin; 
		 fromBinMethods["numReceptors"] = &CalAppPhaseRow::numReceptorsFromBin; 
		 fromBinMethods["numChannels"] = &CalAppPhaseRow::numChannelsFromBin; 
		 fromBinMethods["numPhaseValues"] = &CalAppPhaseRow::numPhaseValuesFromBin; 
		 fromBinMethods["phaseValues"] = &CalAppPhaseRow::phaseValuesFromBin; 
		 fromBinMethods["numCompare"] = &CalAppPhaseRow::numCompareFromBin; 
		 fromBinMethods["numEfficiencies"] = &CalAppPhaseRow::numEfficienciesFromBin; 
		 fromBinMethods["compareArray"] = &CalAppPhaseRow::compareArrayFromBin; 
		 fromBinMethods["efficiencyIndices"] = &CalAppPhaseRow::efficiencyIndicesFromBin; 
		 fromBinMethods["efficiencies"] = &CalAppPhaseRow::efficienciesFromBin; 
		 fromBinMethods["quality"] = &CalAppPhaseRow::qualityFromBin; 
		 fromBinMethods["phasedSumAntenna"] = &CalAppPhaseRow::phasedSumAntennaFromBin; 
			
	
		 fromBinMethods["typeSupports"] = &CalAppPhaseRow::typeSupportsFromBin; 
		 fromBinMethods["numSupports"] = &CalAppPhaseRow::numSupportsFromBin; 
		 fromBinMethods["phaseSupports"] = &CalAppPhaseRow::phaseSupportsFromBin; 
			
	}

	
	bool CalAppPhaseRow::compareNoAutoInc(BasebandNameMod::BasebandName basebandName, int scanNumber, Tag calDataId, Tag calReductionId, ArrayTime startValidTime, ArrayTime endValidTime, ArrayTime adjustTime, std::string adjustToken, std::string phasingMode, int numPhasedAntennas, std::vector<std::string > phasedAntennas, int refAntennaIndex, int candRefAntennaIndex, std::string phasePacking, int numReceptors, int numChannels, int numPhaseValues, std::vector<float > phaseValues, int numCompare, int numEfficiencies, std::vector<std::string > compareArray, std::vector<int > efficiencyIndices, std::vector<std::vector<float > > efficiencies, std::vector<float > quality, std::string phasedSumAntenna) {
		bool result;
		result = true;
		
	
		
		result = result && (this->basebandName == basebandName);
		
		if (!result) return false;
	

	
		
		result = result && (this->scanNumber == scanNumber);
		
		if (!result) return false;
	

	
		
		result = result && (this->calDataId == calDataId);
		
		if (!result) return false;
	

	
		
		result = result && (this->calReductionId == calReductionId);
		
		if (!result) return false;
	

	
		
		result = result && (this->startValidTime == startValidTime);
		
		if (!result) return false;
	

	
		
		result = result && (this->endValidTime == endValidTime);
		
		if (!result) return false;
	

	
		
		result = result && (this->adjustTime == adjustTime);
		
		if (!result) return false;
	

	
		
		result = result && (this->adjustToken == adjustToken);
		
		if (!result) return false;
	

	
		
		result = result && (this->phasingMode == phasingMode);
		
		if (!result) return false;
	

	
		
		result = result && (this->numPhasedAntennas == numPhasedAntennas);
		
		if (!result) return false;
	

	
		
		result = result && (this->phasedAntennas == phasedAntennas);
		
		if (!result) return false;
	

	
		
		result = result && (this->refAntennaIndex == refAntennaIndex);
		
		if (!result) return false;
	

	
		
		result = result && (this->candRefAntennaIndex == candRefAntennaIndex);
		
		if (!result) return false;
	

	
		
		result = result && (this->phasePacking == phasePacking);
		
		if (!result) return false;
	

	
		
		result = result && (this->numReceptors == numReceptors);
		
		if (!result) return false;
	

	
		
		result = result && (this->numChannels == numChannels);
		
		if (!result) return false;
	

	
		
		result = result && (this->numPhaseValues == numPhaseValues);
		
		if (!result) return false;
	

	
		
		result = result && (this->phaseValues == phaseValues);
		
		if (!result) return false;
	

	
		
		result = result && (this->numCompare == numCompare);
		
		if (!result) return false;
	

	
		
		result = result && (this->numEfficiencies == numEfficiencies);
		
		if (!result) return false;
	

	
		
		result = result && (this->compareArray == compareArray);
		
		if (!result) return false;
	

	
		
		result = result && (this->efficiencyIndices == efficiencyIndices);
		
		if (!result) return false;
	

	
		
		result = result && (this->efficiencies == efficiencies);
		
		if (!result) return false;
	

	
		
		result = result && (this->quality == quality);
		
		if (!result) return false;
	

	
		
		result = result && (this->phasedSumAntenna == phasedSumAntenna);
		
		if (!result) return false;
	

		return result;
	}	
	
	
	
	bool CalAppPhaseRow::compareRequiredValue(ArrayTime startValidTime, ArrayTime endValidTime, ArrayTime adjustTime, std::string adjustToken, std::string phasingMode, int numPhasedAntennas, std::vector<std::string > phasedAntennas, int refAntennaIndex, int candRefAntennaIndex, std::string phasePacking, int numReceptors, int numChannels, int numPhaseValues, std::vector<float > phaseValues, int numCompare, int numEfficiencies, std::vector<std::string > compareArray, std::vector<int > efficiencyIndices, std::vector<std::vector<float > > efficiencies, std::vector<float > quality, std::string phasedSumAntenna) {
		bool result;
		result = true;
		
	
		if (!(this->startValidTime == startValidTime)) return false;
	

	
		if (!(this->endValidTime == endValidTime)) return false;
	

	
		if (!(this->adjustTime == adjustTime)) return false;
	

	
		if (!(this->adjustToken == adjustToken)) return false;
	

	
		if (!(this->phasingMode == phasingMode)) return false;
	

	
		if (!(this->numPhasedAntennas == numPhasedAntennas)) return false;
	

	
		if (!(this->phasedAntennas == phasedAntennas)) return false;
	

	
		if (!(this->refAntennaIndex == refAntennaIndex)) return false;
	

	
		if (!(this->candRefAntennaIndex == candRefAntennaIndex)) return false;
	

	
		if (!(this->phasePacking == phasePacking)) return false;
	

	
		if (!(this->numReceptors == numReceptors)) return false;
	

	
		if (!(this->numChannels == numChannels)) return false;
	

	
		if (!(this->numPhaseValues == numPhaseValues)) return false;
	

	
		if (!(this->phaseValues == phaseValues)) return false;
	

	
		if (!(this->numCompare == numCompare)) return false;
	

	
		if (!(this->numEfficiencies == numEfficiencies)) return false;
	

	
		if (!(this->compareArray == compareArray)) return false;
	

	
		if (!(this->efficiencyIndices == efficiencyIndices)) return false;
	

	
		if (!(this->efficiencies == efficiencies)) return false;
	

	
		if (!(this->quality == quality)) return false;
	

	
		if (!(this->phasedSumAntenna == phasedSumAntenna)) return false;
	

		return result;
	}
	
	
	/**
	 * Return true if all required attributes of the value part are equal to their homologues
	 * in x and false otherwise.
	 *

	 * @param x a pointer on the CalAppPhaseRow whose required attributes of the value part 

	 * will be compared with those of this.
	 * @return a boolean.
	 */
	bool CalAppPhaseRow::equalByRequiredValue(CalAppPhaseRow*  x ) {
		
			
		if (this->startValidTime != x->startValidTime) return false;
			
		if (this->endValidTime != x->endValidTime) return false;
			
		if (this->adjustTime != x->adjustTime) return false;
			
		if (this->adjustToken != x->adjustToken) return false;
			
		if (this->phasingMode != x->phasingMode) return false;
			
		if (this->numPhasedAntennas != x->numPhasedAntennas) return false;
			
		if (this->phasedAntennas != x->phasedAntennas) return false;
			
		if (this->refAntennaIndex != x->refAntennaIndex) return false;
			
		if (this->candRefAntennaIndex != x->candRefAntennaIndex) return false;
			
		if (this->phasePacking != x->phasePacking) return false;
			
		if (this->numReceptors != x->numReceptors) return false;
			
		if (this->numChannels != x->numChannels) return false;
			
		if (this->numPhaseValues != x->numPhaseValues) return false;
			
		if (this->phaseValues != x->phaseValues) return false;
			
		if (this->numCompare != x->numCompare) return false;
			
		if (this->numEfficiencies != x->numEfficiencies) return false;
			
		if (this->compareArray != x->compareArray) return false;
			
		if (this->efficiencyIndices != x->efficiencyIndices) return false;
			
		if (this->efficiencies != x->efficiencies) return false;
			
		if (this->quality != x->quality) return false;
			
		if (this->phasedSumAntenna != x->phasedSumAntenna) return false;
			
		
		return true;
	}	
	
/*
	 map<string, CalAppPhaseAttributeFromBin> CalAppPhaseRow::initFromBinMethods() {
		map<string, CalAppPhaseAttributeFromBin> result;
		
		result["basebandName"] = &CalAppPhaseRow::basebandNameFromBin;
		result["scanNumber"] = &CalAppPhaseRow::scanNumberFromBin;
		result["calDataId"] = &CalAppPhaseRow::calDataIdFromBin;
		result["calReductionId"] = &CalAppPhaseRow::calReductionIdFromBin;
		result["startValidTime"] = &CalAppPhaseRow::startValidTimeFromBin;
		result["endValidTime"] = &CalAppPhaseRow::endValidTimeFromBin;
		result["adjustTime"] = &CalAppPhaseRow::adjustTimeFromBin;
		result["adjustToken"] = &CalAppPhaseRow::adjustTokenFromBin;
		result["phasingMode"] = &CalAppPhaseRow::phasingModeFromBin;
		result["numPhasedAntennas"] = &CalAppPhaseRow::numPhasedAntennasFromBin;
		result["phasedAntennas"] = &CalAppPhaseRow::phasedAntennasFromBin;
		result["refAntennaIndex"] = &CalAppPhaseRow::refAntennaIndexFromBin;
		result["candRefAntennaIndex"] = &CalAppPhaseRow::candRefAntennaIndexFromBin;
		result["phasePacking"] = &CalAppPhaseRow::phasePackingFromBin;
		result["numReceptors"] = &CalAppPhaseRow::numReceptorsFromBin;
		result["numChannels"] = &CalAppPhaseRow::numChannelsFromBin;
		result["numPhaseValues"] = &CalAppPhaseRow::numPhaseValuesFromBin;
		result["phaseValues"] = &CalAppPhaseRow::phaseValuesFromBin;
		result["numCompare"] = &CalAppPhaseRow::numCompareFromBin;
		result["numEfficiencies"] = &CalAppPhaseRow::numEfficienciesFromBin;
		result["compareArray"] = &CalAppPhaseRow::compareArrayFromBin;
		result["efficiencyIndices"] = &CalAppPhaseRow::efficiencyIndicesFromBin;
		result["efficiencies"] = &CalAppPhaseRow::efficienciesFromBin;
		result["quality"] = &CalAppPhaseRow::qualityFromBin;
		result["phasedSumAntenna"] = &CalAppPhaseRow::phasedSumAntennaFromBin;
		
		
		result["typeSupports"] = &CalAppPhaseRow::typeSupportsFromBin;
		result["numSupports"] = &CalAppPhaseRow::numSupportsFromBin;
		result["phaseSupports"] = &CalAppPhaseRow::phaseSupportsFromBin;
			
		
		return result;	
	}
*/	
} // End namespace asdm
 
