
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
 * File CalHolographyRow.cpp
 */
 
#include <vector>
#include <set>

#include <alma/ASDM/ASDM.h>
#include <alma/ASDM/CalHolographyRow.h>
#include <alma/ASDM/CalHolographyTable.h>

#include <alma/ASDM/CalReductionTable.h>
#include <alma/ASDM/CalReductionRow.h>

#include <alma/ASDM/CalDataTable.h>
#include <alma/ASDM/CalDataRow.h>
	

using asdm::ASDM;
using asdm::CalHolographyRow;
using asdm::CalHolographyTable;

using asdm::CalReductionTable;
using asdm::CalReductionRow;

using asdm::CalDataTable;
using asdm::CalDataRow;


#include <alma/ASDM/Parser.h>

#include <alma/ASDM/EnumerationParser.h>
#include <alma/ASDM/ASDMValuesParser.h>
 
#include <alma/ASDM/InvalidArgumentException.h>

using namespace std;

namespace asdm {
	CalHolographyRow::~CalHolographyRow() {
	}

	/**
	 * Return the table to which this row belongs.
	 */
	CalHolographyTable &CalHolographyRow::getTable() const {
		return table;
	}

	bool CalHolographyRow::isAdded() const {
		return hasBeenAdded;
	}	

	void CalHolographyRow::isAdded(bool added) {
		hasBeenAdded = added;
	}
	
#ifndef WITHOUT_ACS
	using asdmIDL::CalHolographyRowIDL;
#endif
	
#ifndef WITHOUT_ACS
	/**
	 * Return this row in the form of an IDL struct.
	 * @return The values of this row as a CalHolographyRowIDL struct.
	 */
	CalHolographyRowIDL *CalHolographyRow::toIDL() const {
		CalHolographyRowIDL *x = new CalHolographyRowIDL ();
		
		// Fill the IDL structure.
	
		
	
  		
		
		
			
				
		x->antennaName = CORBA::string_dup(antennaName.c_str());
				
 			
		
	

	
  		
		
		
			
				
		x->antennaMake = antennaMake;
 				
 			
		
	

	
  		
		
		
			
		x->startValidTime = startValidTime.toIDLArrayTime();
			
		
	

	
  		
		
		
			
		x->endValidTime = endValidTime.toIDLArrayTime();
			
		
	

	
  		
		
		
			
		x->ambientTemperature = ambientTemperature.toIDLTemperature();
			
		
	

	
  		
		
		
			
		x->focusPosition.length(focusPosition.size());
		for (unsigned int i = 0; i < focusPosition.size(); ++i) {
			
			x->focusPosition[i] = focusPosition.at(i).toIDLLength();
			
	 	}
			
		
	

	
  		
		
		
			
		x->frequencyRange.length(frequencyRange.size());
		for (unsigned int i = 0; i < frequencyRange.size(); ++i) {
			
			x->frequencyRange[i] = frequencyRange.at(i).toIDLFrequency();
			
	 	}
			
		
	

	
  		
		
		
			
				
		x->illuminationTaper = illuminationTaper;
 				
 			
		
	

	
  		
		
		
			
				
		x->numReceptor = numReceptor;
 				
 			
		
	

	
  		
		
		
			
		x->polarizationTypes.length(polarizationTypes.size());
		for (unsigned int i = 0; i < polarizationTypes.size(); ++i) {
			
				
			x->polarizationTypes[i] = polarizationTypes.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		
			
				
		x->numPanelModes = numPanelModes;
 				
 			
		
	

	
  		
		
		
			
				
		x->receiverBand = receiverBand;
 				
 			
		
	

	
  		
		
		
			
		x->beamMapUID = beamMapUID.toIDLEntityRef();
			
		
	

	
  		
		
		
			
		x->rawRMS = rawRMS.toIDLLength();
			
		
	

	
  		
		
		
			
		x->weightedRMS = weightedRMS.toIDLLength();
			
		
	

	
  		
		
		
			
		x->surfaceMapUID = surfaceMapUID.toIDLEntityRef();
			
		
	

	
  		
		
		
			
		x->direction.length(direction.size());
		for (unsigned int i = 0; i < direction.size(); ++i) {
			
			x->direction[i] = direction.at(i).toIDLAngle();
			
	 	}
			
		
	

	
  		
		
		x->numScrewExists = numScrewExists;
		
		
			
				
		x->numScrew = numScrew;
 				
 			
		
	

	
  		
		
		x->screwNameExists = screwNameExists;
		
		
			
		x->screwName.length(screwName.size());
		for (unsigned int i = 0; i < screwName.size(); ++i) {
			
				
			x->screwName[i] = CORBA::string_dup(screwName.at(i).c_str());
				
	 		
	 	}
			
		
	

	
  		
		
		x->screwMotionExists = screwMotionExists;
		
		
			
		x->screwMotion.length(screwMotion.size());
		for (unsigned int i = 0; i < screwMotion.size(); ++i) {
			
			x->screwMotion[i] = screwMotion.at(i).toIDLLength();
			
	 	}
			
		
	

	
  		
		
		x->screwMotionErrorExists = screwMotionErrorExists;
		
		
			
		x->screwMotionError.length(screwMotionError.size());
		for (unsigned int i = 0; i < screwMotionError.size(); ++i) {
			
			x->screwMotionError[i] = screwMotionError.at(i).toIDLLength();
			
	 	}
			
		
	

	
  		
		
		x->gravCorrectionExists = gravCorrectionExists;
		
		
			
				
		x->gravCorrection = gravCorrection;
 				
 			
		
	

	
  		
		
		x->gravOptRangeExists = gravOptRangeExists;
		
		
			
		x->gravOptRange.length(gravOptRange.size());
		for (unsigned int i = 0; i < gravOptRange.size(); ++i) {
			
			x->gravOptRange[i] = gravOptRange.at(i).toIDLAngle();
			
	 	}
			
		
	

	
  		
		
		x->tempCorrectionExists = tempCorrectionExists;
		
		
			
				
		x->tempCorrection = tempCorrection;
 				
 			
		
	

	
  		
		
		x->tempOptRangeExists = tempOptRangeExists;
		
		
			
		x->tempOptRange.length(tempOptRange.size());
		for (unsigned int i = 0; i < tempOptRange.size(); ++i) {
			
			x->tempOptRange[i] = tempOptRange.at(i).toIDLTemperature();
			
	 	}
			
		
	

	
	
		
	
  	
 		
		
	 	
			
		x->calDataId = calDataId.toIDLTag();
			
	 	 		
  	

	
  	
 		
		
	 	
			
		x->calReductionId = calReductionId.toIDLTag();
			
	 	 		
  	

	
		
	

	

		
		return x;
	
	}
	
	void CalHolographyRow::toIDL(asdmIDL::CalHolographyRowIDL& x) const {
		// Set the x's fields.
	
		
	
  		
		
		
			
				
		x.antennaName = CORBA::string_dup(antennaName.c_str());
				
 			
		
	

	
  		
		
		
			
				
		x.antennaMake = antennaMake;
 				
 			
		
	

	
  		
		
		
			
		x.startValidTime = startValidTime.toIDLArrayTime();
			
		
	

	
  		
		
		
			
		x.endValidTime = endValidTime.toIDLArrayTime();
			
		
	

	
  		
		
		
			
		x.ambientTemperature = ambientTemperature.toIDLTemperature();
			
		
	

	
  		
		
		
			
		x.focusPosition.length(focusPosition.size());
		for (unsigned int i = 0; i < focusPosition.size(); ++i) {
			
			x.focusPosition[i] = focusPosition.at(i).toIDLLength();
			
	 	}
			
		
	

	
  		
		
		
			
		x.frequencyRange.length(frequencyRange.size());
		for (unsigned int i = 0; i < frequencyRange.size(); ++i) {
			
			x.frequencyRange[i] = frequencyRange.at(i).toIDLFrequency();
			
	 	}
			
		
	

	
  		
		
		
			
				
		x.illuminationTaper = illuminationTaper;
 				
 			
		
	

	
  		
		
		
			
				
		x.numReceptor = numReceptor;
 				
 			
		
	

	
  		
		
		
			
		x.polarizationTypes.length(polarizationTypes.size());
		for (unsigned int i = 0; i < polarizationTypes.size(); ++i) {
			
				
			x.polarizationTypes[i] = polarizationTypes.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		
			
				
		x.numPanelModes = numPanelModes;
 				
 			
		
	

	
  		
		
		
			
				
		x.receiverBand = receiverBand;
 				
 			
		
	

	
  		
		
		
			
		x.beamMapUID = beamMapUID.toIDLEntityRef();
			
		
	

	
  		
		
		
			
		x.rawRMS = rawRMS.toIDLLength();
			
		
	

	
  		
		
		
			
		x.weightedRMS = weightedRMS.toIDLLength();
			
		
	

	
  		
		
		
			
		x.surfaceMapUID = surfaceMapUID.toIDLEntityRef();
			
		
	

	
  		
		
		
			
		x.direction.length(direction.size());
		for (unsigned int i = 0; i < direction.size(); ++i) {
			
			x.direction[i] = direction.at(i).toIDLAngle();
			
	 	}
			
		
	

	
  		
		
		x.numScrewExists = numScrewExists;
		
		
			
				
		x.numScrew = numScrew;
 				
 			
		
	

	
  		
		
		x.screwNameExists = screwNameExists;
		
		
			
		x.screwName.length(screwName.size());
		for (unsigned int i = 0; i < screwName.size(); ++i) {
			
				
			x.screwName[i] = CORBA::string_dup(screwName.at(i).c_str());
				
	 		
	 	}
			
		
	

	
  		
		
		x.screwMotionExists = screwMotionExists;
		
		
			
		x.screwMotion.length(screwMotion.size());
		for (unsigned int i = 0; i < screwMotion.size(); ++i) {
			
			x.screwMotion[i] = screwMotion.at(i).toIDLLength();
			
	 	}
			
		
	

	
  		
		
		x.screwMotionErrorExists = screwMotionErrorExists;
		
		
			
		x.screwMotionError.length(screwMotionError.size());
		for (unsigned int i = 0; i < screwMotionError.size(); ++i) {
			
			x.screwMotionError[i] = screwMotionError.at(i).toIDLLength();
			
	 	}
			
		
	

	
  		
		
		x.gravCorrectionExists = gravCorrectionExists;
		
		
			
				
		x.gravCorrection = gravCorrection;
 				
 			
		
	

	
  		
		
		x.gravOptRangeExists = gravOptRangeExists;
		
		
			
		x.gravOptRange.length(gravOptRange.size());
		for (unsigned int i = 0; i < gravOptRange.size(); ++i) {
			
			x.gravOptRange[i] = gravOptRange.at(i).toIDLAngle();
			
	 	}
			
		
	

	
  		
		
		x.tempCorrectionExists = tempCorrectionExists;
		
		
			
				
		x.tempCorrection = tempCorrection;
 				
 			
		
	

	
  		
		
		x.tempOptRangeExists = tempOptRangeExists;
		
		
			
		x.tempOptRange.length(tempOptRange.size());
		for (unsigned int i = 0; i < tempOptRange.size(); ++i) {
			
			x.tempOptRange[i] = tempOptRange.at(i).toIDLTemperature();
			
	 	}
			
		
	

	
	
		
	
  	
 		
		
	 	
			
		x.calDataId = calDataId.toIDLTag();
			
	 	 		
  	

	
  	
 		
		
	 	
			
		x.calReductionId = calReductionId.toIDLTag();
			
	 	 		
  	

	
		
	

	

	
	}
#endif
	

#ifndef WITHOUT_ACS
	/**
	 * Fill the values of this row from the IDL struct CalHolographyRowIDL.
	 * @param x The IDL struct containing the values used to fill this row.
	 */
	void CalHolographyRow::setFromIDL (CalHolographyRowIDL x){
		try {
		// Fill the values from x.
	
		
	
		
		
			
		setAntennaName(string (x.antennaName));
			
 		
		
	

	
		
		
			
		setAntennaMake(x.antennaMake);
  			
 		
		
	

	
		
		
			
		setStartValidTime(ArrayTime (x.startValidTime));
			
 		
		
	

	
		
		
			
		setEndValidTime(ArrayTime (x.endValidTime));
			
 		
		
	

	
		
		
			
		setAmbientTemperature(Temperature (x.ambientTemperature));
			
 		
		
	

	
		
		
			
		focusPosition .clear();
		for (unsigned int i = 0; i <x.focusPosition.length(); ++i) {
			
			focusPosition.push_back(Length (x.focusPosition[i]));
			
		}
			
  		
		
	

	
		
		
			
		frequencyRange .clear();
		for (unsigned int i = 0; i <x.frequencyRange.length(); ++i) {
			
			frequencyRange.push_back(Frequency (x.frequencyRange[i]));
			
		}
			
  		
		
	

	
		
		
			
		setIlluminationTaper(x.illuminationTaper);
  			
 		
		
	

	
		
		
			
		setNumReceptor(x.numReceptor);
  			
 		
		
	

	
		
		
			
		polarizationTypes .clear();
		for (unsigned int i = 0; i <x.polarizationTypes.length(); ++i) {
			
			polarizationTypes.push_back(x.polarizationTypes[i]);
  			
		}
			
  		
		
	

	
		
		
			
		setNumPanelModes(x.numPanelModes);
  			
 		
		
	

	
		
		
			
		setReceiverBand(x.receiverBand);
  			
 		
		
	

	
		
		
			
		setBeamMapUID(EntityRef (x.beamMapUID));
			
 		
		
	

	
		
		
			
		setRawRMS(Length (x.rawRMS));
			
 		
		
	

	
		
		
			
		setWeightedRMS(Length (x.weightedRMS));
			
 		
		
	

	
		
		
			
		setSurfaceMapUID(EntityRef (x.surfaceMapUID));
			
 		
		
	

	
		
		
			
		direction .clear();
		for (unsigned int i = 0; i <x.direction.length(); ++i) {
			
			direction.push_back(Angle (x.direction[i]));
			
		}
			
  		
		
	

	
		
		numScrewExists = x.numScrewExists;
		if (x.numScrewExists) {
		
		
			
		setNumScrew(x.numScrew);
  			
 		
		
		}
		
	

	
		
		screwNameExists = x.screwNameExists;
		if (x.screwNameExists) {
		
		
			
		screwName .clear();
		for (unsigned int i = 0; i <x.screwName.length(); ++i) {
			
			screwName.push_back(string (x.screwName[i]));
			
		}
			
  		
		
		}
		
	

	
		
		screwMotionExists = x.screwMotionExists;
		if (x.screwMotionExists) {
		
		
			
		screwMotion .clear();
		for (unsigned int i = 0; i <x.screwMotion.length(); ++i) {
			
			screwMotion.push_back(Length (x.screwMotion[i]));
			
		}
			
  		
		
		}
		
	

	
		
		screwMotionErrorExists = x.screwMotionErrorExists;
		if (x.screwMotionErrorExists) {
		
		
			
		screwMotionError .clear();
		for (unsigned int i = 0; i <x.screwMotionError.length(); ++i) {
			
			screwMotionError.push_back(Length (x.screwMotionError[i]));
			
		}
			
  		
		
		}
		
	

	
		
		gravCorrectionExists = x.gravCorrectionExists;
		if (x.gravCorrectionExists) {
		
		
			
		setGravCorrection(x.gravCorrection);
  			
 		
		
		}
		
	

	
		
		gravOptRangeExists = x.gravOptRangeExists;
		if (x.gravOptRangeExists) {
		
		
			
		gravOptRange .clear();
		for (unsigned int i = 0; i <x.gravOptRange.length(); ++i) {
			
			gravOptRange.push_back(Angle (x.gravOptRange[i]));
			
		}
			
  		
		
		}
		
	

	
		
		tempCorrectionExists = x.tempCorrectionExists;
		if (x.tempCorrectionExists) {
		
		
			
		setTempCorrection(x.tempCorrection);
  			
 		
		
		}
		
	

	
		
		tempOptRangeExists = x.tempOptRangeExists;
		if (x.tempOptRangeExists) {
		
		
			
		tempOptRange .clear();
		for (unsigned int i = 0; i <x.tempOptRange.length(); ++i) {
			
			tempOptRange.push_back(Temperature (x.tempOptRange[i]));
			
		}
			
  		
		
		}
		
	

	
	
		
	
		
		
			
		setCalDataId(Tag (x.calDataId));
			
 		
		
	

	
		
		
			
		setCalReductionId(Tag (x.calReductionId));
			
 		
		
	

	
		
	

	

		} catch (const IllegalAccessException &err) {
			throw ConversionException (err.getMessage(),"CalHolography");
		}
	}
#endif
	
	/**
	 * Return this row in the form of an XML string.
	 * @return The values of this row as an XML string.
	 */
	string CalHolographyRow::toXML() const {
		string buf;
		buf.append("<row> \n");
		
	
		
  	
 		
		
		Parser::toXML(antennaName, "antennaName", buf);
		
		
	

  	
 		
		
			buf.append(EnumerationParser::toXML("antennaMake", antennaMake));
		
		
	

  	
 		
		
		Parser::toXML(startValidTime, "startValidTime", buf);
		
		
	

  	
 		
		
		Parser::toXML(endValidTime, "endValidTime", buf);
		
		
	

  	
 		
		
		Parser::toXML(ambientTemperature, "ambientTemperature", buf);
		
		
	

  	
 		
		
		Parser::toXML(focusPosition, "focusPosition", buf);
		
		
	

  	
 		
		
		Parser::toXML(frequencyRange, "frequencyRange", buf);
		
		
	

  	
 		
		
		Parser::toXML(illuminationTaper, "illuminationTaper", buf);
		
		
	

  	
 		
		
		Parser::toXML(numReceptor, "numReceptor", buf);
		
		
	

  	
 		
		
			buf.append(EnumerationParser::toXML("polarizationTypes", polarizationTypes));
		
		
	

  	
 		
		
		Parser::toXML(numPanelModes, "numPanelModes", buf);
		
		
	

  	
 		
		
			buf.append(EnumerationParser::toXML("receiverBand", receiverBand));
		
		
	

  	
 		
		
		Parser::toXML(beamMapUID, "beamMapUID", buf);
		
		
	

  	
 		
		
		Parser::toXML(rawRMS, "rawRMS", buf);
		
		
	

  	
 		
		
		Parser::toXML(weightedRMS, "weightedRMS", buf);
		
		
	

  	
 		
		
		Parser::toXML(surfaceMapUID, "surfaceMapUID", buf);
		
		
	

  	
 		
		
		Parser::toXML(direction, "direction", buf);
		
		
	

  	
 		
		if (numScrewExists) {
		
		
		Parser::toXML(numScrew, "numScrew", buf);
		
		
		}
		
	

  	
 		
		if (screwNameExists) {
		
		
		Parser::toXML(screwName, "screwName", buf);
		
		
		}
		
	

  	
 		
		if (screwMotionExists) {
		
		
		Parser::toXML(screwMotion, "screwMotion", buf);
		
		
		}
		
	

  	
 		
		if (screwMotionErrorExists) {
		
		
		Parser::toXML(screwMotionError, "screwMotionError", buf);
		
		
		}
		
	

  	
 		
		if (gravCorrectionExists) {
		
		
		Parser::toXML(gravCorrection, "gravCorrection", buf);
		
		
		}
		
	

  	
 		
		if (gravOptRangeExists) {
		
		
		Parser::toXML(gravOptRange, "gravOptRange", buf);
		
		
		}
		
	

  	
 		
		if (tempCorrectionExists) {
		
		
		Parser::toXML(tempCorrection, "tempCorrection", buf);
		
		
		}
		
	

  	
 		
		if (tempOptRangeExists) {
		
		
		Parser::toXML(tempOptRange, "tempOptRange", buf);
		
		
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
	void CalHolographyRow::setFromXML (string rowDoc) {
		Parser row(rowDoc);
		string s = "";
		try {
	
		
	
  		
			
	  	setAntennaName(Parser::getString("antennaName","CalHolography",rowDoc));
			
		
	

	
		
		
		
		antennaMake = EnumerationParser::getAntennaMake("antennaMake","CalHolography",rowDoc);
		
		
		
	

	
  		
			
	  	setStartValidTime(Parser::getArrayTime("startValidTime","CalHolography",rowDoc));
			
		
	

	
  		
			
	  	setEndValidTime(Parser::getArrayTime("endValidTime","CalHolography",rowDoc));
			
		
	

	
  		
			
	  	setAmbientTemperature(Parser::getTemperature("ambientTemperature","CalHolography",rowDoc));
			
		
	

	
  		
			
					
	  	setFocusPosition(Parser::get1DLength("focusPosition","CalHolography",rowDoc));
	  			
	  		
		
	

	
  		
			
					
	  	setFrequencyRange(Parser::get1DFrequency("frequencyRange","CalHolography",rowDoc));
	  			
	  		
		
	

	
  		
			
	  	setIlluminationTaper(Parser::getDouble("illuminationTaper","CalHolography",rowDoc));
			
		
	

	
  		
			
	  	setNumReceptor(Parser::getInteger("numReceptor","CalHolography",rowDoc));
			
		
	

	
		
		
		
		polarizationTypes = EnumerationParser::getPolarizationType1D("polarizationTypes","CalHolography",rowDoc);			
		
		
		
	

	
  		
			
	  	setNumPanelModes(Parser::getInteger("numPanelModes","CalHolography",rowDoc));
			
		
	

	
		
		
		
		receiverBand = EnumerationParser::getReceiverBand("receiverBand","CalHolography",rowDoc);
		
		
		
	

	
  		
			
	  	setBeamMapUID(Parser::getEntityRef("beamMapUID","CalHolography",rowDoc));
			
		
	

	
  		
			
	  	setRawRMS(Parser::getLength("rawRMS","CalHolography",rowDoc));
			
		
	

	
  		
			
	  	setWeightedRMS(Parser::getLength("weightedRMS","CalHolography",rowDoc));
			
		
	

	
  		
			
	  	setSurfaceMapUID(Parser::getEntityRef("surfaceMapUID","CalHolography",rowDoc));
			
		
	

	
  		
			
					
	  	setDirection(Parser::get1DAngle("direction","CalHolography",rowDoc));
	  			
	  		
		
	

	
  		
        if (row.isStr("<numScrew>")) {
			
	  		setNumScrew(Parser::getInteger("numScrew","CalHolography",rowDoc));
			
		}
 		
	

	
  		
        if (row.isStr("<screwName>")) {
			
								
	  		setScrewName(Parser::get1DString("screwName","CalHolography",rowDoc));
	  			
	  		
		}
 		
	

	
  		
        if (row.isStr("<screwMotion>")) {
			
								
	  		setScrewMotion(Parser::get1DLength("screwMotion","CalHolography",rowDoc));
	  			
	  		
		}
 		
	

	
  		
        if (row.isStr("<screwMotionError>")) {
			
								
	  		setScrewMotionError(Parser::get1DLength("screwMotionError","CalHolography",rowDoc));
	  			
	  		
		}
 		
	

	
  		
        if (row.isStr("<gravCorrection>")) {
			
	  		setGravCorrection(Parser::getBoolean("gravCorrection","CalHolography",rowDoc));
			
		}
 		
	

	
  		
        if (row.isStr("<gravOptRange>")) {
			
								
	  		setGravOptRange(Parser::get1DAngle("gravOptRange","CalHolography",rowDoc));
	  			
	  		
		}
 		
	

	
  		
        if (row.isStr("<tempCorrection>")) {
			
	  		setTempCorrection(Parser::getBoolean("tempCorrection","CalHolography",rowDoc));
			
		}
 		
	

	
  		
        if (row.isStr("<tempOptRange>")) {
			
								
	  		setTempOptRange(Parser::get1DTemperature("tempOptRange","CalHolography",rowDoc));
	  			
	  		
		}
 		
	

	
	
		
	
  		
			
	  	setCalDataId(Parser::getTag("calDataId","CalData",rowDoc));
			
		
	

	
  		
			
	  	setCalReductionId(Parser::getTag("calReductionId","CalReduction",rowDoc));
			
		
	

	
		
	

	

		} catch (const IllegalAccessException &err) {
			throw ConversionException (err.getMessage(),"CalHolography");
		}
	}
	
	void CalHolographyRow::toBin(EndianOSStream& eoss) {
	
	
	
	
		
						
			eoss.writeString(antennaName);
				
		
	

	
	
		
	calDataId.toBin(eoss);
		
	

	
	
		
	calReductionId.toBin(eoss);
		
	

	
	
		
					
		eoss.writeString(CAntennaMake::name(antennaMake));
			/* eoss.writeInt(antennaMake); */
				
		
	

	
	
		
	startValidTime.toBin(eoss);
		
	

	
	
		
	endValidTime.toBin(eoss);
		
	

	
	
		
	ambientTemperature.toBin(eoss);
		
	

	
	
		
	Length::toBin(focusPosition, eoss);
		
	

	
	
		
	Frequency::toBin(frequencyRange, eoss);
		
	

	
	
		
						
			eoss.writeDouble(illuminationTaper);
				
		
	

	
	
		
						
			eoss.writeInt(numReceptor);
				
		
	

	
	
		
		
			
		eoss.writeInt((int) polarizationTypes.size());
		for (unsigned int i = 0; i < polarizationTypes.size(); i++)
				
			eoss.writeString(CPolarizationType::name(polarizationTypes.at(i)));
			/* eoss.writeInt(polarizationTypes.at(i)); */
				
				
						
		
	

	
	
		
						
			eoss.writeInt(numPanelModes);
				
		
	

	
	
		
					
		eoss.writeString(CReceiverBand::name(receiverBand));
			/* eoss.writeInt(receiverBand); */
				
		
	

	
	
		
	beamMapUID.toBin(eoss);
		
	

	
	
		
	rawRMS.toBin(eoss);
		
	

	
	
		
	weightedRMS.toBin(eoss);
		
	

	
	
		
	surfaceMapUID.toBin(eoss);
		
	

	
	
		
	Angle::toBin(direction, eoss);
		
	


	
	
	eoss.writeBoolean(numScrewExists);
	if (numScrewExists) {
	
	
	
		
						
			eoss.writeInt(numScrew);
				
		
	

	}

	eoss.writeBoolean(screwNameExists);
	if (screwNameExists) {
	
	
	
		
		
			
		eoss.writeInt((int) screwName.size());
		for (unsigned int i = 0; i < screwName.size(); i++)
				
			eoss.writeString(screwName.at(i));
				
				
						
		
	

	}

	eoss.writeBoolean(screwMotionExists);
	if (screwMotionExists) {
	
	
	
		
	Length::toBin(screwMotion, eoss);
		
	

	}

	eoss.writeBoolean(screwMotionErrorExists);
	if (screwMotionErrorExists) {
	
	
	
		
	Length::toBin(screwMotionError, eoss);
		
	

	}

	eoss.writeBoolean(gravCorrectionExists);
	if (gravCorrectionExists) {
	
	
	
		
						
			eoss.writeBoolean(gravCorrection);
				
		
	

	}

	eoss.writeBoolean(gravOptRangeExists);
	if (gravOptRangeExists) {
	
	
	
		
	Angle::toBin(gravOptRange, eoss);
		
	

	}

	eoss.writeBoolean(tempCorrectionExists);
	if (tempCorrectionExists) {
	
	
	
		
						
			eoss.writeBoolean(tempCorrection);
				
		
	

	}

	eoss.writeBoolean(tempOptRangeExists);
	if (tempOptRangeExists) {
	
	
	
		
	Temperature::toBin(tempOptRange, eoss);
		
	

	}

	}
	
void CalHolographyRow::antennaNameFromBin(EndianIStream& eis) {
		
	
	
		
			
		antennaName =  eis.readString();
			
		
	
	
}
void CalHolographyRow::calDataIdFromBin(EndianIStream& eis) {
		
	
		
		
		calDataId =  Tag::fromBin(eis);
		
	
	
}
void CalHolographyRow::calReductionIdFromBin(EndianIStream& eis) {
		
	
		
		
		calReductionId =  Tag::fromBin(eis);
		
	
	
}
void CalHolographyRow::antennaMakeFromBin(EndianIStream& eis) {
		
	
	
		
			
		antennaMake = CAntennaMake::literal(eis.readString());
			
		
	
	
}
void CalHolographyRow::startValidTimeFromBin(EndianIStream& eis) {
		
	
		
		
		startValidTime =  ArrayTime::fromBin(eis);
		
	
	
}
void CalHolographyRow::endValidTimeFromBin(EndianIStream& eis) {
		
	
		
		
		endValidTime =  ArrayTime::fromBin(eis);
		
	
	
}
void CalHolographyRow::ambientTemperatureFromBin(EndianIStream& eis) {
		
	
		
		
		ambientTemperature =  Temperature::fromBin(eis);
		
	
	
}
void CalHolographyRow::focusPositionFromBin(EndianIStream& eis) {
		
	
		
		
			
	
	focusPosition = Length::from1DBin(eis);	
	

		
	
	
}
void CalHolographyRow::frequencyRangeFromBin(EndianIStream& eis) {
		
	
		
		
			
	
	frequencyRange = Frequency::from1DBin(eis);	
	

		
	
	
}
void CalHolographyRow::illuminationTaperFromBin(EndianIStream& eis) {
		
	
	
		
			
		illuminationTaper =  eis.readDouble();
			
		
	
	
}
void CalHolographyRow::numReceptorFromBin(EndianIStream& eis) {
		
	
	
		
			
		numReceptor =  eis.readInt();
			
		
	
	
}
void CalHolographyRow::polarizationTypesFromBin(EndianIStream& eis) {
		
	
	
		
			
	
		polarizationTypes.clear();
		
		unsigned int polarizationTypesDim1 = eis.readInt();
		for (unsigned int  i = 0 ; i < polarizationTypesDim1; i++)
			
			polarizationTypes.push_back(CPolarizationType::literal(eis.readString()));
			
	

		
	
	
}
void CalHolographyRow::numPanelModesFromBin(EndianIStream& eis) {
		
	
	
		
			
		numPanelModes =  eis.readInt();
			
		
	
	
}
void CalHolographyRow::receiverBandFromBin(EndianIStream& eis) {
		
	
	
		
			
		receiverBand = CReceiverBand::literal(eis.readString());
			
		
	
	
}
void CalHolographyRow::beamMapUIDFromBin(EndianIStream& eis) {
		
	
		
		
		beamMapUID =  EntityRef::fromBin(eis);
		
	
	
}
void CalHolographyRow::rawRMSFromBin(EndianIStream& eis) {
		
	
		
		
		rawRMS =  Length::fromBin(eis);
		
	
	
}
void CalHolographyRow::weightedRMSFromBin(EndianIStream& eis) {
		
	
		
		
		weightedRMS =  Length::fromBin(eis);
		
	
	
}
void CalHolographyRow::surfaceMapUIDFromBin(EndianIStream& eis) {
		
	
		
		
		surfaceMapUID =  EntityRef::fromBin(eis);
		
	
	
}
void CalHolographyRow::directionFromBin(EndianIStream& eis) {
		
	
		
		
			
	
	direction = Angle::from1DBin(eis);	
	

		
	
	
}

void CalHolographyRow::numScrewFromBin(EndianIStream& eis) {
		
	numScrewExists = eis.readBoolean();
	if (numScrewExists) {
		
	
	
		
			
		numScrew =  eis.readInt();
			
		
	

	}
	
}
void CalHolographyRow::screwNameFromBin(EndianIStream& eis) {
		
	screwNameExists = eis.readBoolean();
	if (screwNameExists) {
		
	
	
		
			
	
		screwName.clear();
		
		unsigned int screwNameDim1 = eis.readInt();
		for (unsigned int  i = 0 ; i < screwNameDim1; i++)
			
			screwName.push_back(eis.readString());
			
	

		
	

	}
	
}
void CalHolographyRow::screwMotionFromBin(EndianIStream& eis) {
		
	screwMotionExists = eis.readBoolean();
	if (screwMotionExists) {
		
	
		
		
			
	
	screwMotion = Length::from1DBin(eis);	
	

		
	

	}
	
}
void CalHolographyRow::screwMotionErrorFromBin(EndianIStream& eis) {
		
	screwMotionErrorExists = eis.readBoolean();
	if (screwMotionErrorExists) {
		
	
		
		
			
	
	screwMotionError = Length::from1DBin(eis);	
	

		
	

	}
	
}
void CalHolographyRow::gravCorrectionFromBin(EndianIStream& eis) {
		
	gravCorrectionExists = eis.readBoolean();
	if (gravCorrectionExists) {
		
	
	
		
			
		gravCorrection =  eis.readBoolean();
			
		
	

	}
	
}
void CalHolographyRow::gravOptRangeFromBin(EndianIStream& eis) {
		
	gravOptRangeExists = eis.readBoolean();
	if (gravOptRangeExists) {
		
	
		
		
			
	
	gravOptRange = Angle::from1DBin(eis);	
	

		
	

	}
	
}
void CalHolographyRow::tempCorrectionFromBin(EndianIStream& eis) {
		
	tempCorrectionExists = eis.readBoolean();
	if (tempCorrectionExists) {
		
	
	
		
			
		tempCorrection =  eis.readBoolean();
			
		
	

	}
	
}
void CalHolographyRow::tempOptRangeFromBin(EndianIStream& eis) {
		
	tempOptRangeExists = eis.readBoolean();
	if (tempOptRangeExists) {
		
	
		
		
			
	
	tempOptRange = Temperature::from1DBin(eis);	
	

		
	

	}
	
}
	
	
	CalHolographyRow* CalHolographyRow::fromBin(EndianIStream& eis, CalHolographyTable& table, const vector<string>& attributesSeq) {
		CalHolographyRow* row = new  CalHolographyRow(table);
		
		map<string, CalHolographyAttributeFromBin>::iterator iter ;
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
					throw ConversionException("There is not method to read an attribute '"+attributesSeq.at(i)+"'.", "CalHolographyTable");
			}
				
		}				
		return row;
	}

	//
	// A collection of methods to set the value of the attributes from their textual value in the XML representation
	// of one row.
	//
	
	// Convert a string into an String 
	void CalHolographyRow::antennaNameFromText(const string & s) {
		 
          
		antennaName = ASDMValuesParser::parse<string>(s);
          
		
	}
	
	
	// Convert a string into an Tag 
	void CalHolographyRow::calDataIdFromText(const string & s) {
		 
          
		calDataId = ASDMValuesParser::parse<Tag>(s);
          
		
	}
	
	
	// Convert a string into an Tag 
	void CalHolographyRow::calReductionIdFromText(const string & s) {
		 
          
		calReductionId = ASDMValuesParser::parse<Tag>(s);
          
		
	}
	
	
	// Convert a string into an AntennaMake 
	void CalHolographyRow::antennaMakeFromText(const string & s) {
		 
          
		antennaMake = ASDMValuesParser::parse<AntennaMakeMod::AntennaMake>(s);
          
		
	}
	
	
	// Convert a string into an ArrayTime 
	void CalHolographyRow::startValidTimeFromText(const string & s) {
		 
          
		startValidTime = ASDMValuesParser::parse<ArrayTime>(s);
          
		
	}
	
	
	// Convert a string into an ArrayTime 
	void CalHolographyRow::endValidTimeFromText(const string & s) {
		 
          
		endValidTime = ASDMValuesParser::parse<ArrayTime>(s);
          
		
	}
	
	
	// Convert a string into an Temperature 
	void CalHolographyRow::ambientTemperatureFromText(const string & s) {
		 
          
		ambientTemperature = ASDMValuesParser::parse<Temperature>(s);
          
		
	}
	
	
	// Convert a string into an Length 
	void CalHolographyRow::focusPositionFromText(const string & s) {
		 
          
		focusPosition = ASDMValuesParser::parse1D<Length>(s);
          
		
	}
	
	
	// Convert a string into an Frequency 
	void CalHolographyRow::frequencyRangeFromText(const string & s) {
		 
          
		frequencyRange = ASDMValuesParser::parse1D<Frequency>(s);
          
		
	}
	
	
	// Convert a string into an double 
	void CalHolographyRow::illuminationTaperFromText(const string & s) {
		 
          
		illuminationTaper = ASDMValuesParser::parse<double>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void CalHolographyRow::numReceptorFromText(const string & s) {
		 
          
		numReceptor = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	// Convert a string into an PolarizationType 
	void CalHolographyRow::polarizationTypesFromText(const string & s) {
		 
          
		polarizationTypes = ASDMValuesParser::parse1D<PolarizationTypeMod::PolarizationType>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void CalHolographyRow::numPanelModesFromText(const string & s) {
		 
          
		numPanelModes = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	// Convert a string into an ReceiverBand 
	void CalHolographyRow::receiverBandFromText(const string & s) {
		 
          
		receiverBand = ASDMValuesParser::parse<ReceiverBandMod::ReceiverBand>(s);
          
		
	}
	
	
	
	// Convert a string into an Length 
	void CalHolographyRow::rawRMSFromText(const string & s) {
		 
          
		rawRMS = ASDMValuesParser::parse<Length>(s);
          
		
	}
	
	
	// Convert a string into an Length 
	void CalHolographyRow::weightedRMSFromText(const string & s) {
		 
          
		weightedRMS = ASDMValuesParser::parse<Length>(s);
          
		
	}
	
	
	
	// Convert a string into an Angle 
	void CalHolographyRow::directionFromText(const string & s) {
		 
          
		direction = ASDMValuesParser::parse1D<Angle>(s);
          
		
	}
	

	
	// Convert a string into an int 
	void CalHolographyRow::numScrewFromText(const string & s) {
		numScrewExists = true;
		 
          
		numScrew = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	// Convert a string into an String 
	void CalHolographyRow::screwNameFromText(const string & s) {
		screwNameExists = true;
		 
          
		screwName = ASDMValuesParser::parse1D<string>(s);
          
		
	}
	
	
	// Convert a string into an Length 
	void CalHolographyRow::screwMotionFromText(const string & s) {
		screwMotionExists = true;
		 
          
		screwMotion = ASDMValuesParser::parse1D<Length>(s);
          
		
	}
	
	
	// Convert a string into an Length 
	void CalHolographyRow::screwMotionErrorFromText(const string & s) {
		screwMotionErrorExists = true;
		 
          
		screwMotionError = ASDMValuesParser::parse1D<Length>(s);
          
		
	}
	
	
	// Convert a string into an boolean 
	void CalHolographyRow::gravCorrectionFromText(const string & s) {
		gravCorrectionExists = true;
		 
          
		gravCorrection = ASDMValuesParser::parse<bool>(s);
          
		
	}
	
	
	// Convert a string into an Angle 
	void CalHolographyRow::gravOptRangeFromText(const string & s) {
		gravOptRangeExists = true;
		 
          
		gravOptRange = ASDMValuesParser::parse1D<Angle>(s);
          
		
	}
	
	
	// Convert a string into an boolean 
	void CalHolographyRow::tempCorrectionFromText(const string & s) {
		tempCorrectionExists = true;
		 
          
		tempCorrection = ASDMValuesParser::parse<bool>(s);
          
		
	}
	
	
	// Convert a string into an Temperature 
	void CalHolographyRow::tempOptRangeFromText(const string & s) {
		tempOptRangeExists = true;
		 
          
		tempOptRange = ASDMValuesParser::parse1D<Temperature>(s);
          
		
	}
	
	
	
	void CalHolographyRow::fromText(const std::string& attributeName, const std::string&  t) {
		map<string, CalHolographyAttributeFromText>::iterator iter;
		if ((iter = fromTextMethods.find(attributeName)) == fromTextMethods.end())
			throw ConversionException("I do not know what to do with '"+attributeName+"' and its content '"+t+"' (while parsing an XML document)", "CalHolographyTable");
		(this->*(iter->second))(t);
	}
			
	////////////////////////////////////////////////
	// Intrinsic Table Attributes getters/setters //
	////////////////////////////////////////////////
	
	

	
 	/**
 	 * Get antennaName.
 	 * @return antennaName as std::string
 	 */
 	std::string CalHolographyRow::getAntennaName() const {
	
  		return antennaName;
 	}

 	/**
 	 * Set antennaName with the specified std::string.
 	 * @param antennaName The std::string value to which antennaName is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void CalHolographyRow::setAntennaName (std::string antennaName)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("antennaName", "CalHolography");
		
  		}
  	
 		this->antennaName = antennaName;
	
 	}
	
	

	

	
 	/**
 	 * Get antennaMake.
 	 * @return antennaMake as AntennaMakeMod::AntennaMake
 	 */
 	AntennaMakeMod::AntennaMake CalHolographyRow::getAntennaMake() const {
	
  		return antennaMake;
 	}

 	/**
 	 * Set antennaMake with the specified AntennaMakeMod::AntennaMake.
 	 * @param antennaMake The AntennaMakeMod::AntennaMake value to which antennaMake is to be set.
 	 
 	
 		
 	 */
 	void CalHolographyRow::setAntennaMake (AntennaMakeMod::AntennaMake antennaMake)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->antennaMake = antennaMake;
	
 	}
	
	

	

	
 	/**
 	 * Get startValidTime.
 	 * @return startValidTime as ArrayTime
 	 */
 	ArrayTime CalHolographyRow::getStartValidTime() const {
	
  		return startValidTime;
 	}

 	/**
 	 * Set startValidTime with the specified ArrayTime.
 	 * @param startValidTime The ArrayTime value to which startValidTime is to be set.
 	 
 	
 		
 	 */
 	void CalHolographyRow::setStartValidTime (ArrayTime startValidTime)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->startValidTime = startValidTime;
	
 	}
	
	

	

	
 	/**
 	 * Get endValidTime.
 	 * @return endValidTime as ArrayTime
 	 */
 	ArrayTime CalHolographyRow::getEndValidTime() const {
	
  		return endValidTime;
 	}

 	/**
 	 * Set endValidTime with the specified ArrayTime.
 	 * @param endValidTime The ArrayTime value to which endValidTime is to be set.
 	 
 	
 		
 	 */
 	void CalHolographyRow::setEndValidTime (ArrayTime endValidTime)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->endValidTime = endValidTime;
	
 	}
	
	

	

	
 	/**
 	 * Get ambientTemperature.
 	 * @return ambientTemperature as Temperature
 	 */
 	Temperature CalHolographyRow::getAmbientTemperature() const {
	
  		return ambientTemperature;
 	}

 	/**
 	 * Set ambientTemperature with the specified Temperature.
 	 * @param ambientTemperature The Temperature value to which ambientTemperature is to be set.
 	 
 	
 		
 	 */
 	void CalHolographyRow::setAmbientTemperature (Temperature ambientTemperature)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->ambientTemperature = ambientTemperature;
	
 	}
	
	

	

	
 	/**
 	 * Get focusPosition.
 	 * @return focusPosition as std::vector<Length >
 	 */
 	std::vector<Length > CalHolographyRow::getFocusPosition() const {
	
  		return focusPosition;
 	}

 	/**
 	 * Set focusPosition with the specified std::vector<Length >.
 	 * @param focusPosition The std::vector<Length > value to which focusPosition is to be set.
 	 
 	
 		
 	 */
 	void CalHolographyRow::setFocusPosition (std::vector<Length > focusPosition)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->focusPosition = focusPosition;
	
 	}
	
	

	

	
 	/**
 	 * Get frequencyRange.
 	 * @return frequencyRange as std::vector<Frequency >
 	 */
 	std::vector<Frequency > CalHolographyRow::getFrequencyRange() const {
	
  		return frequencyRange;
 	}

 	/**
 	 * Set frequencyRange with the specified std::vector<Frequency >.
 	 * @param frequencyRange The std::vector<Frequency > value to which frequencyRange is to be set.
 	 
 	
 		
 	 */
 	void CalHolographyRow::setFrequencyRange (std::vector<Frequency > frequencyRange)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->frequencyRange = frequencyRange;
	
 	}
	
	

	

	
 	/**
 	 * Get illuminationTaper.
 	 * @return illuminationTaper as double
 	 */
 	double CalHolographyRow::getIlluminationTaper() const {
	
  		return illuminationTaper;
 	}

 	/**
 	 * Set illuminationTaper with the specified double.
 	 * @param illuminationTaper The double value to which illuminationTaper is to be set.
 	 
 	
 		
 	 */
 	void CalHolographyRow::setIlluminationTaper (double illuminationTaper)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->illuminationTaper = illuminationTaper;
	
 	}
	
	

	

	
 	/**
 	 * Get numReceptor.
 	 * @return numReceptor as int
 	 */
 	int CalHolographyRow::getNumReceptor() const {
	
  		return numReceptor;
 	}

 	/**
 	 * Set numReceptor with the specified int.
 	 * @param numReceptor The int value to which numReceptor is to be set.
 	 
 	
 		
 	 */
 	void CalHolographyRow::setNumReceptor (int numReceptor)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->numReceptor = numReceptor;
	
 	}
	
	

	

	
 	/**
 	 * Get polarizationTypes.
 	 * @return polarizationTypes as std::vector<PolarizationTypeMod::PolarizationType >
 	 */
 	std::vector<PolarizationTypeMod::PolarizationType > CalHolographyRow::getPolarizationTypes() const {
	
  		return polarizationTypes;
 	}

 	/**
 	 * Set polarizationTypes with the specified std::vector<PolarizationTypeMod::PolarizationType >.
 	 * @param polarizationTypes The std::vector<PolarizationTypeMod::PolarizationType > value to which polarizationTypes is to be set.
 	 
 	
 		
 	 */
 	void CalHolographyRow::setPolarizationTypes (std::vector<PolarizationTypeMod::PolarizationType > polarizationTypes)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->polarizationTypes = polarizationTypes;
	
 	}
	
	

	

	
 	/**
 	 * Get numPanelModes.
 	 * @return numPanelModes as int
 	 */
 	int CalHolographyRow::getNumPanelModes() const {
	
  		return numPanelModes;
 	}

 	/**
 	 * Set numPanelModes with the specified int.
 	 * @param numPanelModes The int value to which numPanelModes is to be set.
 	 
 	
 		
 	 */
 	void CalHolographyRow::setNumPanelModes (int numPanelModes)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->numPanelModes = numPanelModes;
	
 	}
	
	

	

	
 	/**
 	 * Get receiverBand.
 	 * @return receiverBand as ReceiverBandMod::ReceiverBand
 	 */
 	ReceiverBandMod::ReceiverBand CalHolographyRow::getReceiverBand() const {
	
  		return receiverBand;
 	}

 	/**
 	 * Set receiverBand with the specified ReceiverBandMod::ReceiverBand.
 	 * @param receiverBand The ReceiverBandMod::ReceiverBand value to which receiverBand is to be set.
 	 
 	
 		
 	 */
 	void CalHolographyRow::setReceiverBand (ReceiverBandMod::ReceiverBand receiverBand)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->receiverBand = receiverBand;
	
 	}
	
	

	

	
 	/**
 	 * Get beamMapUID.
 	 * @return beamMapUID as EntityRef
 	 */
 	EntityRef CalHolographyRow::getBeamMapUID() const {
	
  		return beamMapUID;
 	}

 	/**
 	 * Set beamMapUID with the specified EntityRef.
 	 * @param beamMapUID The EntityRef value to which beamMapUID is to be set.
 	 
 	
 		
 	 */
 	void CalHolographyRow::setBeamMapUID (EntityRef beamMapUID)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->beamMapUID = beamMapUID;
	
 	}
	
	

	

	
 	/**
 	 * Get rawRMS.
 	 * @return rawRMS as Length
 	 */
 	Length CalHolographyRow::getRawRMS() const {
	
  		return rawRMS;
 	}

 	/**
 	 * Set rawRMS with the specified Length.
 	 * @param rawRMS The Length value to which rawRMS is to be set.
 	 
 	
 		
 	 */
 	void CalHolographyRow::setRawRMS (Length rawRMS)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->rawRMS = rawRMS;
	
 	}
	
	

	

	
 	/**
 	 * Get weightedRMS.
 	 * @return weightedRMS as Length
 	 */
 	Length CalHolographyRow::getWeightedRMS() const {
	
  		return weightedRMS;
 	}

 	/**
 	 * Set weightedRMS with the specified Length.
 	 * @param weightedRMS The Length value to which weightedRMS is to be set.
 	 
 	
 		
 	 */
 	void CalHolographyRow::setWeightedRMS (Length weightedRMS)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->weightedRMS = weightedRMS;
	
 	}
	
	

	

	
 	/**
 	 * Get surfaceMapUID.
 	 * @return surfaceMapUID as EntityRef
 	 */
 	EntityRef CalHolographyRow::getSurfaceMapUID() const {
	
  		return surfaceMapUID;
 	}

 	/**
 	 * Set surfaceMapUID with the specified EntityRef.
 	 * @param surfaceMapUID The EntityRef value to which surfaceMapUID is to be set.
 	 
 	
 		
 	 */
 	void CalHolographyRow::setSurfaceMapUID (EntityRef surfaceMapUID)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->surfaceMapUID = surfaceMapUID;
	
 	}
	
	

	

	
 	/**
 	 * Get direction.
 	 * @return direction as std::vector<Angle >
 	 */
 	std::vector<Angle > CalHolographyRow::getDirection() const {
	
  		return direction;
 	}

 	/**
 	 * Set direction with the specified std::vector<Angle >.
 	 * @param direction The std::vector<Angle > value to which direction is to be set.
 	 
 	
 		
 	 */
 	void CalHolographyRow::setDirection (std::vector<Angle > direction)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->direction = direction;
	
 	}
	
	

	
	/**
	 * The attribute numScrew is optional. Return true if this attribute exists.
	 * @return true if and only if the numScrew attribute exists. 
	 */
	bool CalHolographyRow::isNumScrewExists() const {
		return numScrewExists;
	}
	

	
 	/**
 	 * Get numScrew, which is optional.
 	 * @return numScrew as int
 	 * @throw IllegalAccessException If numScrew does not exist.
 	 */
 	int CalHolographyRow::getNumScrew() const  {
		if (!numScrewExists) {
			throw IllegalAccessException("numScrew", "CalHolography");
		}
	
  		return numScrew;
 	}

 	/**
 	 * Set numScrew with the specified int.
 	 * @param numScrew The int value to which numScrew is to be set.
 	 
 	
 	 */
 	void CalHolographyRow::setNumScrew (int numScrew) {
	
 		this->numScrew = numScrew;
	
		numScrewExists = true;
	
 	}
	
	
	/**
	 * Mark numScrew, which is an optional field, as non-existent.
	 */
	void CalHolographyRow::clearNumScrew () {
		numScrewExists = false;
	}
	

	
	/**
	 * The attribute screwName is optional. Return true if this attribute exists.
	 * @return true if and only if the screwName attribute exists. 
	 */
	bool CalHolographyRow::isScrewNameExists() const {
		return screwNameExists;
	}
	

	
 	/**
 	 * Get screwName, which is optional.
 	 * @return screwName as std::vector<std::string >
 	 * @throw IllegalAccessException If screwName does not exist.
 	 */
 	std::vector<std::string > CalHolographyRow::getScrewName() const  {
		if (!screwNameExists) {
			throw IllegalAccessException("screwName", "CalHolography");
		}
	
  		return screwName;
 	}

 	/**
 	 * Set screwName with the specified std::vector<std::string >.
 	 * @param screwName The std::vector<std::string > value to which screwName is to be set.
 	 
 	
 	 */
 	void CalHolographyRow::setScrewName (std::vector<std::string > screwName) {
	
 		this->screwName = screwName;
	
		screwNameExists = true;
	
 	}
	
	
	/**
	 * Mark screwName, which is an optional field, as non-existent.
	 */
	void CalHolographyRow::clearScrewName () {
		screwNameExists = false;
	}
	

	
	/**
	 * The attribute screwMotion is optional. Return true if this attribute exists.
	 * @return true if and only if the screwMotion attribute exists. 
	 */
	bool CalHolographyRow::isScrewMotionExists() const {
		return screwMotionExists;
	}
	

	
 	/**
 	 * Get screwMotion, which is optional.
 	 * @return screwMotion as std::vector<Length >
 	 * @throw IllegalAccessException If screwMotion does not exist.
 	 */
 	std::vector<Length > CalHolographyRow::getScrewMotion() const  {
		if (!screwMotionExists) {
			throw IllegalAccessException("screwMotion", "CalHolography");
		}
	
  		return screwMotion;
 	}

 	/**
 	 * Set screwMotion with the specified std::vector<Length >.
 	 * @param screwMotion The std::vector<Length > value to which screwMotion is to be set.
 	 
 	
 	 */
 	void CalHolographyRow::setScrewMotion (std::vector<Length > screwMotion) {
	
 		this->screwMotion = screwMotion;
	
		screwMotionExists = true;
	
 	}
	
	
	/**
	 * Mark screwMotion, which is an optional field, as non-existent.
	 */
	void CalHolographyRow::clearScrewMotion () {
		screwMotionExists = false;
	}
	

	
	/**
	 * The attribute screwMotionError is optional. Return true if this attribute exists.
	 * @return true if and only if the screwMotionError attribute exists. 
	 */
	bool CalHolographyRow::isScrewMotionErrorExists() const {
		return screwMotionErrorExists;
	}
	

	
 	/**
 	 * Get screwMotionError, which is optional.
 	 * @return screwMotionError as std::vector<Length >
 	 * @throw IllegalAccessException If screwMotionError does not exist.
 	 */
 	std::vector<Length > CalHolographyRow::getScrewMotionError() const  {
		if (!screwMotionErrorExists) {
			throw IllegalAccessException("screwMotionError", "CalHolography");
		}
	
  		return screwMotionError;
 	}

 	/**
 	 * Set screwMotionError with the specified std::vector<Length >.
 	 * @param screwMotionError The std::vector<Length > value to which screwMotionError is to be set.
 	 
 	
 	 */
 	void CalHolographyRow::setScrewMotionError (std::vector<Length > screwMotionError) {
	
 		this->screwMotionError = screwMotionError;
	
		screwMotionErrorExists = true;
	
 	}
	
	
	/**
	 * Mark screwMotionError, which is an optional field, as non-existent.
	 */
	void CalHolographyRow::clearScrewMotionError () {
		screwMotionErrorExists = false;
	}
	

	
	/**
	 * The attribute gravCorrection is optional. Return true if this attribute exists.
	 * @return true if and only if the gravCorrection attribute exists. 
	 */
	bool CalHolographyRow::isGravCorrectionExists() const {
		return gravCorrectionExists;
	}
	

	
 	/**
 	 * Get gravCorrection, which is optional.
 	 * @return gravCorrection as bool
 	 * @throw IllegalAccessException If gravCorrection does not exist.
 	 */
 	bool CalHolographyRow::getGravCorrection() const  {
		if (!gravCorrectionExists) {
			throw IllegalAccessException("gravCorrection", "CalHolography");
		}
	
  		return gravCorrection;
 	}

 	/**
 	 * Set gravCorrection with the specified bool.
 	 * @param gravCorrection The bool value to which gravCorrection is to be set.
 	 
 	
 	 */
 	void CalHolographyRow::setGravCorrection (bool gravCorrection) {
	
 		this->gravCorrection = gravCorrection;
	
		gravCorrectionExists = true;
	
 	}
	
	
	/**
	 * Mark gravCorrection, which is an optional field, as non-existent.
	 */
	void CalHolographyRow::clearGravCorrection () {
		gravCorrectionExists = false;
	}
	

	
	/**
	 * The attribute gravOptRange is optional. Return true if this attribute exists.
	 * @return true if and only if the gravOptRange attribute exists. 
	 */
	bool CalHolographyRow::isGravOptRangeExists() const {
		return gravOptRangeExists;
	}
	

	
 	/**
 	 * Get gravOptRange, which is optional.
 	 * @return gravOptRange as std::vector<Angle >
 	 * @throw IllegalAccessException If gravOptRange does not exist.
 	 */
 	std::vector<Angle > CalHolographyRow::getGravOptRange() const  {
		if (!gravOptRangeExists) {
			throw IllegalAccessException("gravOptRange", "CalHolography");
		}
	
  		return gravOptRange;
 	}

 	/**
 	 * Set gravOptRange with the specified std::vector<Angle >.
 	 * @param gravOptRange The std::vector<Angle > value to which gravOptRange is to be set.
 	 
 	
 	 */
 	void CalHolographyRow::setGravOptRange (std::vector<Angle > gravOptRange) {
	
 		this->gravOptRange = gravOptRange;
	
		gravOptRangeExists = true;
	
 	}
	
	
	/**
	 * Mark gravOptRange, which is an optional field, as non-existent.
	 */
	void CalHolographyRow::clearGravOptRange () {
		gravOptRangeExists = false;
	}
	

	
	/**
	 * The attribute tempCorrection is optional. Return true if this attribute exists.
	 * @return true if and only if the tempCorrection attribute exists. 
	 */
	bool CalHolographyRow::isTempCorrectionExists() const {
		return tempCorrectionExists;
	}
	

	
 	/**
 	 * Get tempCorrection, which is optional.
 	 * @return tempCorrection as bool
 	 * @throw IllegalAccessException If tempCorrection does not exist.
 	 */
 	bool CalHolographyRow::getTempCorrection() const  {
		if (!tempCorrectionExists) {
			throw IllegalAccessException("tempCorrection", "CalHolography");
		}
	
  		return tempCorrection;
 	}

 	/**
 	 * Set tempCorrection with the specified bool.
 	 * @param tempCorrection The bool value to which tempCorrection is to be set.
 	 
 	
 	 */
 	void CalHolographyRow::setTempCorrection (bool tempCorrection) {
	
 		this->tempCorrection = tempCorrection;
	
		tempCorrectionExists = true;
	
 	}
	
	
	/**
	 * Mark tempCorrection, which is an optional field, as non-existent.
	 */
	void CalHolographyRow::clearTempCorrection () {
		tempCorrectionExists = false;
	}
	

	
	/**
	 * The attribute tempOptRange is optional. Return true if this attribute exists.
	 * @return true if and only if the tempOptRange attribute exists. 
	 */
	bool CalHolographyRow::isTempOptRangeExists() const {
		return tempOptRangeExists;
	}
	

	
 	/**
 	 * Get tempOptRange, which is optional.
 	 * @return tempOptRange as std::vector<Temperature >
 	 * @throw IllegalAccessException If tempOptRange does not exist.
 	 */
 	std::vector<Temperature > CalHolographyRow::getTempOptRange() const  {
		if (!tempOptRangeExists) {
			throw IllegalAccessException("tempOptRange", "CalHolography");
		}
	
  		return tempOptRange;
 	}

 	/**
 	 * Set tempOptRange with the specified std::vector<Temperature >.
 	 * @param tempOptRange The std::vector<Temperature > value to which tempOptRange is to be set.
 	 
 	
 	 */
 	void CalHolographyRow::setTempOptRange (std::vector<Temperature > tempOptRange) {
	
 		this->tempOptRange = tempOptRange;
	
		tempOptRangeExists = true;
	
 	}
	
	
	/**
	 * Mark tempOptRange, which is an optional field, as non-existent.
	 */
	void CalHolographyRow::clearTempOptRange () {
		tempOptRangeExists = false;
	}
	

	
	///////////////////////////////////////////////
	// Extrinsic Table Attributes getters/setters//
	///////////////////////////////////////////////
	
	

	
 	/**
 	 * Get calDataId.
 	 * @return calDataId as Tag
 	 */
 	Tag CalHolographyRow::getCalDataId() const {
	
  		return calDataId;
 	}

 	/**
 	 * Set calDataId with the specified Tag.
 	 * @param calDataId The Tag value to which calDataId is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void CalHolographyRow::setCalDataId (Tag calDataId)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("calDataId", "CalHolography");
		
  		}
  	
 		this->calDataId = calDataId;
	
 	}
	
	

	

	
 	/**
 	 * Get calReductionId.
 	 * @return calReductionId as Tag
 	 */
 	Tag CalHolographyRow::getCalReductionId() const {
	
  		return calReductionId;
 	}

 	/**
 	 * Set calReductionId with the specified Tag.
 	 * @param calReductionId The Tag value to which calReductionId is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void CalHolographyRow::setCalReductionId (Tag calReductionId)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("calReductionId", "CalHolography");
		
  		}
  	
 		this->calReductionId = calReductionId;
	
 	}
	
	


	//////////////////////////////////////
	// Links Attributes getters/setters //
	//////////////////////////////////////
	
	
	
	
		

	/**
	 * Returns the pointer to the row in the CalReduction table having CalReduction.calReductionId == calReductionId
	 * @return a CalReductionRow*
	 * 
	 
	 */
	 CalReductionRow* CalHolographyRow::getCalReductionUsingCalReductionId() {
	 
	 	return table.getContainer().getCalReduction().getRowByKey(calReductionId);
	 }
	 

	

	
	
	
		

	/**
	 * Returns the pointer to the row in the CalData table having CalData.calDataId == calDataId
	 * @return a CalDataRow*
	 * 
	 
	 */
	 CalDataRow* CalHolographyRow::getCalDataUsingCalDataId() {
	 
	 	return table.getContainer().getCalData().getRowByKey(calDataId);
	 }
	 

	

	
	/**
	 * Create a CalHolographyRow.
	 * <p>
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 */ 
	CalHolographyRow::CalHolographyRow (CalHolographyTable &t) : table(t) {
		hasBeenAdded = false;
		
	
	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	
		numScrewExists = false;
	

	
		screwNameExists = false;
	

	
		screwMotionExists = false;
	

	
		screwMotionErrorExists = false;
	

	
		gravCorrectionExists = false;
	

	
		gravOptRangeExists = false;
	

	
		tempCorrectionExists = false;
	

	
		tempOptRangeExists = false;
	

	
	

	

	
	
	
	

	
// This attribute is scalar and has an enumeration type. Let's initialize it to some valid value (the 1st of the enumeration).		
antennaMake = CAntennaMake::from_int(0);
	

	

	

	

	

	

	

	

	

	

	
// This attribute is scalar and has an enumeration type. Let's initialize it to some valid value (the 1st of the enumeration).		
receiverBand = CReceiverBand::from_int(0);
	

	

	

	

	

	

	

	

	

	

	

	

	

	

	
	
	 fromBinMethods["antennaName"] = &CalHolographyRow::antennaNameFromBin; 
	 fromBinMethods["calDataId"] = &CalHolographyRow::calDataIdFromBin; 
	 fromBinMethods["calReductionId"] = &CalHolographyRow::calReductionIdFromBin; 
	 fromBinMethods["antennaMake"] = &CalHolographyRow::antennaMakeFromBin; 
	 fromBinMethods["startValidTime"] = &CalHolographyRow::startValidTimeFromBin; 
	 fromBinMethods["endValidTime"] = &CalHolographyRow::endValidTimeFromBin; 
	 fromBinMethods["ambientTemperature"] = &CalHolographyRow::ambientTemperatureFromBin; 
	 fromBinMethods["focusPosition"] = &CalHolographyRow::focusPositionFromBin; 
	 fromBinMethods["frequencyRange"] = &CalHolographyRow::frequencyRangeFromBin; 
	 fromBinMethods["illuminationTaper"] = &CalHolographyRow::illuminationTaperFromBin; 
	 fromBinMethods["numReceptor"] = &CalHolographyRow::numReceptorFromBin; 
	 fromBinMethods["polarizationTypes"] = &CalHolographyRow::polarizationTypesFromBin; 
	 fromBinMethods["numPanelModes"] = &CalHolographyRow::numPanelModesFromBin; 
	 fromBinMethods["receiverBand"] = &CalHolographyRow::receiverBandFromBin; 
	 fromBinMethods["beamMapUID"] = &CalHolographyRow::beamMapUIDFromBin; 
	 fromBinMethods["rawRMS"] = &CalHolographyRow::rawRMSFromBin; 
	 fromBinMethods["weightedRMS"] = &CalHolographyRow::weightedRMSFromBin; 
	 fromBinMethods["surfaceMapUID"] = &CalHolographyRow::surfaceMapUIDFromBin; 
	 fromBinMethods["direction"] = &CalHolographyRow::directionFromBin; 
		
	
	 fromBinMethods["numScrew"] = &CalHolographyRow::numScrewFromBin; 
	 fromBinMethods["screwName"] = &CalHolographyRow::screwNameFromBin; 
	 fromBinMethods["screwMotion"] = &CalHolographyRow::screwMotionFromBin; 
	 fromBinMethods["screwMotionError"] = &CalHolographyRow::screwMotionErrorFromBin; 
	 fromBinMethods["gravCorrection"] = &CalHolographyRow::gravCorrectionFromBin; 
	 fromBinMethods["gravOptRange"] = &CalHolographyRow::gravOptRangeFromBin; 
	 fromBinMethods["tempCorrection"] = &CalHolographyRow::tempCorrectionFromBin; 
	 fromBinMethods["tempOptRange"] = &CalHolographyRow::tempOptRangeFromBin; 
	
	
	
	
				 
	fromTextMethods["antennaName"] = &CalHolographyRow::antennaNameFromText;
		 
	
				 
	fromTextMethods["calDataId"] = &CalHolographyRow::calDataIdFromText;
		 
	
				 
	fromTextMethods["calReductionId"] = &CalHolographyRow::calReductionIdFromText;
		 
	
				 
	fromTextMethods["antennaMake"] = &CalHolographyRow::antennaMakeFromText;
		 
	
				 
	fromTextMethods["startValidTime"] = &CalHolographyRow::startValidTimeFromText;
		 
	
				 
	fromTextMethods["endValidTime"] = &CalHolographyRow::endValidTimeFromText;
		 
	
				 
	fromTextMethods["ambientTemperature"] = &CalHolographyRow::ambientTemperatureFromText;
		 
	
				 
	fromTextMethods["focusPosition"] = &CalHolographyRow::focusPositionFromText;
		 
	
				 
	fromTextMethods["frequencyRange"] = &CalHolographyRow::frequencyRangeFromText;
		 
	
				 
	fromTextMethods["illuminationTaper"] = &CalHolographyRow::illuminationTaperFromText;
		 
	
				 
	fromTextMethods["numReceptor"] = &CalHolographyRow::numReceptorFromText;
		 
	
				 
	fromTextMethods["polarizationTypes"] = &CalHolographyRow::polarizationTypesFromText;
		 
	
				 
	fromTextMethods["numPanelModes"] = &CalHolographyRow::numPanelModesFromText;
		 
	
				 
	fromTextMethods["receiverBand"] = &CalHolographyRow::receiverBandFromText;
		 
	
		 
	
				 
	fromTextMethods["rawRMS"] = &CalHolographyRow::rawRMSFromText;
		 
	
				 
	fromTextMethods["weightedRMS"] = &CalHolographyRow::weightedRMSFromText;
		 
	
		 
	
				 
	fromTextMethods["direction"] = &CalHolographyRow::directionFromText;
		 
	

	 
				
	fromTextMethods["numScrew"] = &CalHolographyRow::numScrewFromText;
		 	
	 
				
	fromTextMethods["screwName"] = &CalHolographyRow::screwNameFromText;
		 	
	 
				
	fromTextMethods["screwMotion"] = &CalHolographyRow::screwMotionFromText;
		 	
	 
				
	fromTextMethods["screwMotionError"] = &CalHolographyRow::screwMotionErrorFromText;
		 	
	 
				
	fromTextMethods["gravCorrection"] = &CalHolographyRow::gravCorrectionFromText;
		 	
	 
				
	fromTextMethods["gravOptRange"] = &CalHolographyRow::gravOptRangeFromText;
		 	
	 
				
	fromTextMethods["tempCorrection"] = &CalHolographyRow::tempCorrectionFromText;
		 	
	 
				
	fromTextMethods["tempOptRange"] = &CalHolographyRow::tempOptRangeFromText;
		 	
		
	}
	
	CalHolographyRow::CalHolographyRow (CalHolographyTable &t, CalHolographyRow *row) : table(t) {
		hasBeenAdded = false;
		
		if (row == 0) {
	
	
	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	
		numScrewExists = false;
	

	
		screwNameExists = false;
	

	
		screwMotionExists = false;
	

	
		screwMotionErrorExists = false;
	

	
		gravCorrectionExists = false;
	

	
		gravOptRangeExists = false;
	

	
		tempCorrectionExists = false;
	

	
		tempOptRangeExists = false;
	

	
	

	
		
		}
		else {
	
		
			antennaName = row->antennaName;
		
			calDataId = row->calDataId;
		
			calReductionId = row->calReductionId;
		
		
		
		
			antennaMake = row->antennaMake;
		
			startValidTime = row->startValidTime;
		
			endValidTime = row->endValidTime;
		
			ambientTemperature = row->ambientTemperature;
		
			focusPosition = row->focusPosition;
		
			frequencyRange = row->frequencyRange;
		
			illuminationTaper = row->illuminationTaper;
		
			numReceptor = row->numReceptor;
		
			polarizationTypes = row->polarizationTypes;
		
			numPanelModes = row->numPanelModes;
		
			receiverBand = row->receiverBand;
		
			beamMapUID = row->beamMapUID;
		
			rawRMS = row->rawRMS;
		
			weightedRMS = row->weightedRMS;
		
			surfaceMapUID = row->surfaceMapUID;
		
			direction = row->direction;
		
		
		
		
		if (row->numScrewExists) {
			numScrew = row->numScrew;		
			numScrewExists = true;
		}
		else
			numScrewExists = false;
		
		if (row->screwNameExists) {
			screwName = row->screwName;		
			screwNameExists = true;
		}
		else
			screwNameExists = false;
		
		if (row->screwMotionExists) {
			screwMotion = row->screwMotion;		
			screwMotionExists = true;
		}
		else
			screwMotionExists = false;
		
		if (row->screwMotionErrorExists) {
			screwMotionError = row->screwMotionError;		
			screwMotionErrorExists = true;
		}
		else
			screwMotionErrorExists = false;
		
		if (row->gravCorrectionExists) {
			gravCorrection = row->gravCorrection;		
			gravCorrectionExists = true;
		}
		else
			gravCorrectionExists = false;
		
		if (row->gravOptRangeExists) {
			gravOptRange = row->gravOptRange;		
			gravOptRangeExists = true;
		}
		else
			gravOptRangeExists = false;
		
		if (row->tempCorrectionExists) {
			tempCorrection = row->tempCorrection;		
			tempCorrectionExists = true;
		}
		else
			tempCorrectionExists = false;
		
		if (row->tempOptRangeExists) {
			tempOptRange = row->tempOptRange;		
			tempOptRangeExists = true;
		}
		else
			tempOptRangeExists = false;
		
		}
		
		 fromBinMethods["antennaName"] = &CalHolographyRow::antennaNameFromBin; 
		 fromBinMethods["calDataId"] = &CalHolographyRow::calDataIdFromBin; 
		 fromBinMethods["calReductionId"] = &CalHolographyRow::calReductionIdFromBin; 
		 fromBinMethods["antennaMake"] = &CalHolographyRow::antennaMakeFromBin; 
		 fromBinMethods["startValidTime"] = &CalHolographyRow::startValidTimeFromBin; 
		 fromBinMethods["endValidTime"] = &CalHolographyRow::endValidTimeFromBin; 
		 fromBinMethods["ambientTemperature"] = &CalHolographyRow::ambientTemperatureFromBin; 
		 fromBinMethods["focusPosition"] = &CalHolographyRow::focusPositionFromBin; 
		 fromBinMethods["frequencyRange"] = &CalHolographyRow::frequencyRangeFromBin; 
		 fromBinMethods["illuminationTaper"] = &CalHolographyRow::illuminationTaperFromBin; 
		 fromBinMethods["numReceptor"] = &CalHolographyRow::numReceptorFromBin; 
		 fromBinMethods["polarizationTypes"] = &CalHolographyRow::polarizationTypesFromBin; 
		 fromBinMethods["numPanelModes"] = &CalHolographyRow::numPanelModesFromBin; 
		 fromBinMethods["receiverBand"] = &CalHolographyRow::receiverBandFromBin; 
		 fromBinMethods["beamMapUID"] = &CalHolographyRow::beamMapUIDFromBin; 
		 fromBinMethods["rawRMS"] = &CalHolographyRow::rawRMSFromBin; 
		 fromBinMethods["weightedRMS"] = &CalHolographyRow::weightedRMSFromBin; 
		 fromBinMethods["surfaceMapUID"] = &CalHolographyRow::surfaceMapUIDFromBin; 
		 fromBinMethods["direction"] = &CalHolographyRow::directionFromBin; 
			
	
		 fromBinMethods["numScrew"] = &CalHolographyRow::numScrewFromBin; 
		 fromBinMethods["screwName"] = &CalHolographyRow::screwNameFromBin; 
		 fromBinMethods["screwMotion"] = &CalHolographyRow::screwMotionFromBin; 
		 fromBinMethods["screwMotionError"] = &CalHolographyRow::screwMotionErrorFromBin; 
		 fromBinMethods["gravCorrection"] = &CalHolographyRow::gravCorrectionFromBin; 
		 fromBinMethods["gravOptRange"] = &CalHolographyRow::gravOptRangeFromBin; 
		 fromBinMethods["tempCorrection"] = &CalHolographyRow::tempCorrectionFromBin; 
		 fromBinMethods["tempOptRange"] = &CalHolographyRow::tempOptRangeFromBin; 
			
	}

	
	bool CalHolographyRow::compareNoAutoInc(std::string antennaName, Tag calDataId, Tag calReductionId, AntennaMakeMod::AntennaMake antennaMake, ArrayTime startValidTime, ArrayTime endValidTime, Temperature ambientTemperature, std::vector<Length > focusPosition, std::vector<Frequency > frequencyRange, double illuminationTaper, int numReceptor, std::vector<PolarizationTypeMod::PolarizationType > polarizationTypes, int numPanelModes, ReceiverBandMod::ReceiverBand receiverBand, EntityRef beamMapUID, Length rawRMS, Length weightedRMS, EntityRef surfaceMapUID, std::vector<Angle > direction) {
		bool result;
		result = true;
		
	
		
		result = result && (this->antennaName == antennaName);
		
		if (!result) return false;
	

	
		
		result = result && (this->calDataId == calDataId);
		
		if (!result) return false;
	

	
		
		result = result && (this->calReductionId == calReductionId);
		
		if (!result) return false;
	

	
		
		result = result && (this->antennaMake == antennaMake);
		
		if (!result) return false;
	

	
		
		result = result && (this->startValidTime == startValidTime);
		
		if (!result) return false;
	

	
		
		result = result && (this->endValidTime == endValidTime);
		
		if (!result) return false;
	

	
		
		result = result && (this->ambientTemperature == ambientTemperature);
		
		if (!result) return false;
	

	
		
		result = result && (this->focusPosition == focusPosition);
		
		if (!result) return false;
	

	
		
		result = result && (this->frequencyRange == frequencyRange);
		
		if (!result) return false;
	

	
		
		result = result && (this->illuminationTaper == illuminationTaper);
		
		if (!result) return false;
	

	
		
		result = result && (this->numReceptor == numReceptor);
		
		if (!result) return false;
	

	
		
		result = result && (this->polarizationTypes == polarizationTypes);
		
		if (!result) return false;
	

	
		
		result = result && (this->numPanelModes == numPanelModes);
		
		if (!result) return false;
	

	
		
		result = result && (this->receiverBand == receiverBand);
		
		if (!result) return false;
	

	
		
		result = result && (this->beamMapUID == beamMapUID);
		
		if (!result) return false;
	

	
		
		result = result && (this->rawRMS == rawRMS);
		
		if (!result) return false;
	

	
		
		result = result && (this->weightedRMS == weightedRMS);
		
		if (!result) return false;
	

	
		
		result = result && (this->surfaceMapUID == surfaceMapUID);
		
		if (!result) return false;
	

	
		
		result = result && (this->direction == direction);
		
		if (!result) return false;
	

		return result;
	}	
	
	
	
	bool CalHolographyRow::compareRequiredValue(AntennaMakeMod::AntennaMake antennaMake, ArrayTime startValidTime, ArrayTime endValidTime, Temperature ambientTemperature, std::vector<Length > focusPosition, std::vector<Frequency > frequencyRange, double illuminationTaper, int numReceptor, std::vector<PolarizationTypeMod::PolarizationType > polarizationTypes, int numPanelModes, ReceiverBandMod::ReceiverBand receiverBand, EntityRef beamMapUID, Length rawRMS, Length weightedRMS, EntityRef surfaceMapUID, std::vector<Angle > direction) {
		bool result;
		result = true;
		
	
		if (!(this->antennaMake == antennaMake)) return false;
	

	
		if (!(this->startValidTime == startValidTime)) return false;
	

	
		if (!(this->endValidTime == endValidTime)) return false;
	

	
		if (!(this->ambientTemperature == ambientTemperature)) return false;
	

	
		if (!(this->focusPosition == focusPosition)) return false;
	

	
		if (!(this->frequencyRange == frequencyRange)) return false;
	

	
		if (!(this->illuminationTaper == illuminationTaper)) return false;
	

	
		if (!(this->numReceptor == numReceptor)) return false;
	

	
		if (!(this->polarizationTypes == polarizationTypes)) return false;
	

	
		if (!(this->numPanelModes == numPanelModes)) return false;
	

	
		if (!(this->receiverBand == receiverBand)) return false;
	

	
		if (!(this->beamMapUID == beamMapUID)) return false;
	

	
		if (!(this->rawRMS == rawRMS)) return false;
	

	
		if (!(this->weightedRMS == weightedRMS)) return false;
	

	
		if (!(this->surfaceMapUID == surfaceMapUID)) return false;
	

	
		if (!(this->direction == direction)) return false;
	

		return result;
	}
	
	
	/**
	 * Return true if all required attributes of the value part are equal to their homologues
	 * in x and false otherwise.
	 *

	 * @param x a pointer on the CalHolographyRow whose required attributes of the value part 

	 * will be compared with those of this.
	 * @return a boolean.
	 */
	bool CalHolographyRow::equalByRequiredValue(CalHolographyRow*  x ) {
		
			
		if (this->antennaMake != x->antennaMake) return false;
			
		if (this->startValidTime != x->startValidTime) return false;
			
		if (this->endValidTime != x->endValidTime) return false;
			
		if (this->ambientTemperature != x->ambientTemperature) return false;
			
		if (this->focusPosition != x->focusPosition) return false;
			
		if (this->frequencyRange != x->frequencyRange) return false;
			
		if (this->illuminationTaper != x->illuminationTaper) return false;
			
		if (this->numReceptor != x->numReceptor) return false;
			
		if (this->polarizationTypes != x->polarizationTypes) return false;
			
		if (this->numPanelModes != x->numPanelModes) return false;
			
		if (this->receiverBand != x->receiverBand) return false;
			
		if (this->beamMapUID != x->beamMapUID) return false;
			
		if (this->rawRMS != x->rawRMS) return false;
			
		if (this->weightedRMS != x->weightedRMS) return false;
			
		if (this->surfaceMapUID != x->surfaceMapUID) return false;
			
		if (this->direction != x->direction) return false;
			
		
		return true;
	}	
	
/*
	 map<string, CalHolographyAttributeFromBin> CalHolographyRow::initFromBinMethods() {
		map<string, CalHolographyAttributeFromBin> result;
		
		result["antennaName"] = &CalHolographyRow::antennaNameFromBin;
		result["calDataId"] = &CalHolographyRow::calDataIdFromBin;
		result["calReductionId"] = &CalHolographyRow::calReductionIdFromBin;
		result["antennaMake"] = &CalHolographyRow::antennaMakeFromBin;
		result["startValidTime"] = &CalHolographyRow::startValidTimeFromBin;
		result["endValidTime"] = &CalHolographyRow::endValidTimeFromBin;
		result["ambientTemperature"] = &CalHolographyRow::ambientTemperatureFromBin;
		result["focusPosition"] = &CalHolographyRow::focusPositionFromBin;
		result["frequencyRange"] = &CalHolographyRow::frequencyRangeFromBin;
		result["illuminationTaper"] = &CalHolographyRow::illuminationTaperFromBin;
		result["numReceptor"] = &CalHolographyRow::numReceptorFromBin;
		result["polarizationTypes"] = &CalHolographyRow::polarizationTypesFromBin;
		result["numPanelModes"] = &CalHolographyRow::numPanelModesFromBin;
		result["receiverBand"] = &CalHolographyRow::receiverBandFromBin;
		result["beamMapUID"] = &CalHolographyRow::beamMapUIDFromBin;
		result["rawRMS"] = &CalHolographyRow::rawRMSFromBin;
		result["weightedRMS"] = &CalHolographyRow::weightedRMSFromBin;
		result["surfaceMapUID"] = &CalHolographyRow::surfaceMapUIDFromBin;
		result["direction"] = &CalHolographyRow::directionFromBin;
		
		
		result["numScrew"] = &CalHolographyRow::numScrewFromBin;
		result["screwName"] = &CalHolographyRow::screwNameFromBin;
		result["screwMotion"] = &CalHolographyRow::screwMotionFromBin;
		result["screwMotionError"] = &CalHolographyRow::screwMotionErrorFromBin;
		result["gravCorrection"] = &CalHolographyRow::gravCorrectionFromBin;
		result["gravOptRange"] = &CalHolographyRow::gravOptRangeFromBin;
		result["tempCorrection"] = &CalHolographyRow::tempCorrectionFromBin;
		result["tempOptRange"] = &CalHolographyRow::tempOptRangeFromBin;
			
		
		return result;	
	}
*/	
} // End namespace asdm
 
