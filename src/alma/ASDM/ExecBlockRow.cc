
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
 * File ExecBlockRow.cpp
 */
 
#include <vector>
#include <set>

#include <alma/ASDM/ASDM.h>
#include <alma/ASDM/ExecBlockRow.h>
#include <alma/ASDM/ExecBlockTable.h>

#include <alma/ASDM/AntennaTable.h>
#include <alma/ASDM/AntennaRow.h>

#include <alma/ASDM/SBSummaryTable.h>
#include <alma/ASDM/SBSummaryRow.h>

#include <alma/ASDM/ScaleTable.h>
#include <alma/ASDM/ScaleRow.h>
	

using asdm::ASDM;
using asdm::ExecBlockRow;
using asdm::ExecBlockTable;

using asdm::AntennaTable;
using asdm::AntennaRow;

using asdm::SBSummaryTable;
using asdm::SBSummaryRow;

using asdm::ScaleTable;
using asdm::ScaleRow;


#include <alma/ASDM/Parser.h>

#include <alma/ASDM/EnumerationParser.h>
#include <alma/ASDM/ASDMValuesParser.h>
 
#include <alma/ASDM/InvalidArgumentException.h>

using namespace std;

namespace asdm {
	ExecBlockRow::~ExecBlockRow() {
	}

	/**
	 * Return the table to which this row belongs.
	 */
	ExecBlockTable &ExecBlockRow::getTable() const {
		return table;
	}

	bool ExecBlockRow::isAdded() const {
		return hasBeenAdded;
	}	

	void ExecBlockRow::isAdded(bool added) {
		hasBeenAdded = added;
	}
	
#ifndef WITHOUT_ACS
	using asdmIDL::ExecBlockRowIDL;
#endif
	
#ifndef WITHOUT_ACS
	/**
	 * Return this row in the form of an IDL struct.
	 * @return The values of this row as a ExecBlockRowIDL struct.
	 */
	ExecBlockRowIDL *ExecBlockRow::toIDL() const {
		ExecBlockRowIDL *x = new ExecBlockRowIDL ();
		
		// Fill the IDL structure.
	
		
	
  		
		
		
			
		x->execBlockId = execBlockId.toIDLTag();
			
		
	

	
  		
		
		
			
		x->startTime = startTime.toIDLArrayTime();
			
		
	

	
  		
		
		
			
		x->endTime = endTime.toIDLArrayTime();
			
		
	

	
  		
		
		
			
				
		x->execBlockNum = execBlockNum;
 				
 			
		
	

	
  		
		
		
			
		x->execBlockUID = execBlockUID.toIDLEntityRef();
			
		
	

	
  		
		
		
			
		x->projectUID = projectUID.toIDLEntityRef();
			
		
	

	
  		
		
		
			
				
		x->configName = CORBA::string_dup(configName.c_str());
				
 			
		
	

	
  		
		
		
			
				
		x->telescopeName = CORBA::string_dup(telescopeName.c_str());
				
 			
		
	

	
  		
		
		
			
				
		x->observerName = CORBA::string_dup(observerName.c_str());
				
 			
		
	

	
  		
		
		
			
				
		x->numObservingLog = numObservingLog;
 				
 			
		
	

	
  		
		
		
			
		x->observingLog.length(observingLog.size());
		for (unsigned int i = 0; i < observingLog.size(); ++i) {
			
				
			x->observingLog[i] = CORBA::string_dup(observingLog.at(i).c_str());
				
	 		
	 	}
			
		
	

	
  		
		
		
			
		x->sessionReference = sessionReference.toIDLEntityRef();
			
		
	

	
  		
		
		
			
		x->baseRangeMin = baseRangeMin.toIDLLength();
			
		
	

	
  		
		
		
			
		x->baseRangeMax = baseRangeMax.toIDLLength();
			
		
	

	
  		
		
		
			
		x->baseRmsMinor = baseRmsMinor.toIDLLength();
			
		
	

	
  		
		
		
			
		x->baseRmsMajor = baseRmsMajor.toIDLLength();
			
		
	

	
  		
		
		
			
		x->basePa = basePa.toIDLAngle();
			
		
	

	
  		
		
		
			
				
		x->aborted = aborted;
 				
 			
		
	

	
  		
		
		
			
				
		x->numAntenna = numAntenna;
 				
 			
		
	

	
  		
		
		x->releaseDateExists = releaseDateExists;
		
		
			
		x->releaseDate = releaseDate.toIDLArrayTime();
			
		
	

	
  		
		
		x->schedulerModeExists = schedulerModeExists;
		
		
			
				
		x->schedulerMode = CORBA::string_dup(schedulerMode.c_str());
				
 			
		
	

	
  		
		
		x->siteAltitudeExists = siteAltitudeExists;
		
		
			
		x->siteAltitude = siteAltitude.toIDLLength();
			
		
	

	
  		
		
		x->siteLongitudeExists = siteLongitudeExists;
		
		
			
		x->siteLongitude = siteLongitude.toIDLAngle();
			
		
	

	
  		
		
		x->siteLatitudeExists = siteLatitudeExists;
		
		
			
		x->siteLatitude = siteLatitude.toIDLAngle();
			
		
	

	
  		
		
		x->observingScriptExists = observingScriptExists;
		
		
			
				
		x->observingScript = CORBA::string_dup(observingScript.c_str());
				
 			
		
	

	
  		
		
		x->observingScriptUIDExists = observingScriptUIDExists;
		
		
			
		x->observingScriptUID = observingScriptUID.toIDLEntityRef();
			
		
	

	
	
		
	
  	
 		
		
		
		x->antennaId.length(antennaId.size());
		for (unsigned int i = 0; i < antennaId.size(); ++i) {
			
			x->antennaId[i] = antennaId.at(i).toIDLTag();
			
	 	}
	 	 		
  	

	
  	
 		
		
	 	
			
		x->sBSummaryId = sBSummaryId.toIDLTag();
			
	 	 		
  	

	
  	
 		
 		
		x->scaleIdExists = scaleIdExists;
		
		
	 	
			
		x->scaleId = scaleId.toIDLTag();
			
	 	 		
  	

	
		
	

	

	

		
		return x;
	
	}
	
	void ExecBlockRow::toIDL(asdmIDL::ExecBlockRowIDL& x) const {
		// Set the x's fields.
	
		
	
  		
		
		
			
		x.execBlockId = execBlockId.toIDLTag();
			
		
	

	
  		
		
		
			
		x.startTime = startTime.toIDLArrayTime();
			
		
	

	
  		
		
		
			
		x.endTime = endTime.toIDLArrayTime();
			
		
	

	
  		
		
		
			
				
		x.execBlockNum = execBlockNum;
 				
 			
		
	

	
  		
		
		
			
		x.execBlockUID = execBlockUID.toIDLEntityRef();
			
		
	

	
  		
		
		
			
		x.projectUID = projectUID.toIDLEntityRef();
			
		
	

	
  		
		
		
			
				
		x.configName = CORBA::string_dup(configName.c_str());
				
 			
		
	

	
  		
		
		
			
				
		x.telescopeName = CORBA::string_dup(telescopeName.c_str());
				
 			
		
	

	
  		
		
		
			
				
		x.observerName = CORBA::string_dup(observerName.c_str());
				
 			
		
	

	
  		
		
		
			
				
		x.numObservingLog = numObservingLog;
 				
 			
		
	

	
  		
		
		
			
		x.observingLog.length(observingLog.size());
		for (unsigned int i = 0; i < observingLog.size(); ++i) {
			
				
			x.observingLog[i] = CORBA::string_dup(observingLog.at(i).c_str());
				
	 		
	 	}
			
		
	

	
  		
		
		
			
		x.sessionReference = sessionReference.toIDLEntityRef();
			
		
	

	
  		
		
		
			
		x.baseRangeMin = baseRangeMin.toIDLLength();
			
		
	

	
  		
		
		
			
		x.baseRangeMax = baseRangeMax.toIDLLength();
			
		
	

	
  		
		
		
			
		x.baseRmsMinor = baseRmsMinor.toIDLLength();
			
		
	

	
  		
		
		
			
		x.baseRmsMajor = baseRmsMajor.toIDLLength();
			
		
	

	
  		
		
		
			
		x.basePa = basePa.toIDLAngle();
			
		
	

	
  		
		
		
			
				
		x.aborted = aborted;
 				
 			
		
	

	
  		
		
		
			
				
		x.numAntenna = numAntenna;
 				
 			
		
	

	
  		
		
		x.releaseDateExists = releaseDateExists;
		
		
			
		x.releaseDate = releaseDate.toIDLArrayTime();
			
		
	

	
  		
		
		x.schedulerModeExists = schedulerModeExists;
		
		
			
				
		x.schedulerMode = CORBA::string_dup(schedulerMode.c_str());
				
 			
		
	

	
  		
		
		x.siteAltitudeExists = siteAltitudeExists;
		
		
			
		x.siteAltitude = siteAltitude.toIDLLength();
			
		
	

	
  		
		
		x.siteLongitudeExists = siteLongitudeExists;
		
		
			
		x.siteLongitude = siteLongitude.toIDLAngle();
			
		
	

	
  		
		
		x.siteLatitudeExists = siteLatitudeExists;
		
		
			
		x.siteLatitude = siteLatitude.toIDLAngle();
			
		
	

	
  		
		
		x.observingScriptExists = observingScriptExists;
		
		
			
				
		x.observingScript = CORBA::string_dup(observingScript.c_str());
				
 			
		
	

	
  		
		
		x.observingScriptUIDExists = observingScriptUIDExists;
		
		
			
		x.observingScriptUID = observingScriptUID.toIDLEntityRef();
			
		
	

	
	
		
	
  	
 		
		
		
		x.antennaId.length(antennaId.size());
		for (unsigned int i = 0; i < antennaId.size(); ++i) {
			
			x.antennaId[i] = antennaId.at(i).toIDLTag();
			
	 	}
	 	 		
  	

	
  	
 		
		
	 	
			
		x.sBSummaryId = sBSummaryId.toIDLTag();
			
	 	 		
  	

	
  	
 		
 		
		x.scaleIdExists = scaleIdExists;
		
		
	 	
			
		x.scaleId = scaleId.toIDLTag();
			
	 	 		
  	

	
		
	

	

	

	
	}
#endif
	

#ifndef WITHOUT_ACS
	/**
	 * Fill the values of this row from the IDL struct ExecBlockRowIDL.
	 * @param x The IDL struct containing the values used to fill this row.
	 */
	void ExecBlockRow::setFromIDL (ExecBlockRowIDL x){
		try {
		// Fill the values from x.
	
		
	
		
		
			
		setExecBlockId(Tag (x.execBlockId));
			
 		
		
	

	
		
		
			
		setStartTime(ArrayTime (x.startTime));
			
 		
		
	

	
		
		
			
		setEndTime(ArrayTime (x.endTime));
			
 		
		
	

	
		
		
			
		setExecBlockNum(x.execBlockNum);
  			
 		
		
	

	
		
		
			
		setExecBlockUID(EntityRef (x.execBlockUID));
			
 		
		
	

	
		
		
			
		setProjectUID(EntityRef (x.projectUID));
			
 		
		
	

	
		
		
			
		setConfigName(string (x.configName));
			
 		
		
	

	
		
		
			
		setTelescopeName(string (x.telescopeName));
			
 		
		
	

	
		
		
			
		setObserverName(string (x.observerName));
			
 		
		
	

	
		
		
			
		setNumObservingLog(x.numObservingLog);
  			
 		
		
	

	
		
		
			
		observingLog .clear();
		for (unsigned int i = 0; i <x.observingLog.length(); ++i) {
			
			observingLog.push_back(string (x.observingLog[i]));
			
		}
			
  		
		
	

	
		
		
			
		setSessionReference(EntityRef (x.sessionReference));
			
 		
		
	

	
		
		
			
		setBaseRangeMin(Length (x.baseRangeMin));
			
 		
		
	

	
		
		
			
		setBaseRangeMax(Length (x.baseRangeMax));
			
 		
		
	

	
		
		
			
		setBaseRmsMinor(Length (x.baseRmsMinor));
			
 		
		
	

	
		
		
			
		setBaseRmsMajor(Length (x.baseRmsMajor));
			
 		
		
	

	
		
		
			
		setBasePa(Angle (x.basePa));
			
 		
		
	

	
		
		
			
		setAborted(x.aborted);
  			
 		
		
	

	
		
		
			
		setNumAntenna(x.numAntenna);
  			
 		
		
	

	
		
		releaseDateExists = x.releaseDateExists;
		if (x.releaseDateExists) {
		
		
			
		setReleaseDate(ArrayTime (x.releaseDate));
			
 		
		
		}
		
	

	
		
		schedulerModeExists = x.schedulerModeExists;
		if (x.schedulerModeExists) {
		
		
			
		setSchedulerMode(string (x.schedulerMode));
			
 		
		
		}
		
	

	
		
		siteAltitudeExists = x.siteAltitudeExists;
		if (x.siteAltitudeExists) {
		
		
			
		setSiteAltitude(Length (x.siteAltitude));
			
 		
		
		}
		
	

	
		
		siteLongitudeExists = x.siteLongitudeExists;
		if (x.siteLongitudeExists) {
		
		
			
		setSiteLongitude(Angle (x.siteLongitude));
			
 		
		
		}
		
	

	
		
		siteLatitudeExists = x.siteLatitudeExists;
		if (x.siteLatitudeExists) {
		
		
			
		setSiteLatitude(Angle (x.siteLatitude));
			
 		
		
		}
		
	

	
		
		observingScriptExists = x.observingScriptExists;
		if (x.observingScriptExists) {
		
		
			
		setObservingScript(string (x.observingScript));
			
 		
		
		}
		
	

	
		
		observingScriptUIDExists = x.observingScriptUIDExists;
		if (x.observingScriptUIDExists) {
		
		
			
		setObservingScriptUID(EntityRef (x.observingScriptUID));
			
 		
		
		}
		
	

	
	
		
	
		
		antennaId .clear();
		for (unsigned int i = 0; i <x.antennaId.length(); ++i) {
			
			antennaId.push_back(Tag (x.antennaId[i]));
			
		}
		
  	

	
		
		
			
		setSBSummaryId(Tag (x.sBSummaryId));
			
 		
		
	

	
		
		scaleIdExists = x.scaleIdExists;
		if (x.scaleIdExists) {
		
		
			
		setScaleId(Tag (x.scaleId));
			
 		
		
		}
		
	

	
		
	

	

	

		} catch (const IllegalAccessException &err) {
			throw ConversionException (err.getMessage(),"ExecBlock");
		}
	}
#endif
	
	/**
	 * Return this row in the form of an XML string.
	 * @return The values of this row as an XML string.
	 */
	string ExecBlockRow::toXML() const {
		string buf;
		buf.append("<row> \n");
		
	
		
  	
 		
		
		Parser::toXML(execBlockId, "execBlockId", buf);
		
		
	

  	
 		
		
		Parser::toXML(startTime, "startTime", buf);
		
		
	

  	
 		
		
		Parser::toXML(endTime, "endTime", buf);
		
		
	

  	
 		
		
		Parser::toXML(execBlockNum, "execBlockNum", buf);
		
		
	

  	
 		
		
		Parser::toXML(execBlockUID, "execBlockUID", buf);
		
		
	

  	
 		
		
		Parser::toXML(projectUID, "projectUID", buf);
		
		
	

  	
 		
		
		Parser::toXML(configName, "configName", buf);
		
		
	

  	
 		
		
		Parser::toXML(telescopeName, "telescopeName", buf);
		
		
	

  	
 		
		
		Parser::toXML(observerName, "observerName", buf);
		
		
	

  	
 		
		
		Parser::toXML(numObservingLog, "numObservingLog", buf);
		
		
	

  	
 		
		
		Parser::toXML(observingLog, "observingLog", buf);
		
		
	

  	
 		
		
		Parser::toXML(sessionReference, "sessionReference", buf);
		
		
	

  	
 		
		
		Parser::toXML(baseRangeMin, "baseRangeMin", buf);
		
		
	

  	
 		
		
		Parser::toXML(baseRangeMax, "baseRangeMax", buf);
		
		
	

  	
 		
		
		Parser::toXML(baseRmsMinor, "baseRmsMinor", buf);
		
		
	

  	
 		
		
		Parser::toXML(baseRmsMajor, "baseRmsMajor", buf);
		
		
	

  	
 		
		
		Parser::toXML(basePa, "basePa", buf);
		
		
	

  	
 		
		
		Parser::toXML(aborted, "aborted", buf);
		
		
	

  	
 		
		
		Parser::toXML(numAntenna, "numAntenna", buf);
		
		
	

  	
 		
		if (releaseDateExists) {
		
		
		Parser::toXML(releaseDate, "releaseDate", buf);
		
		
		}
		
	

  	
 		
		if (schedulerModeExists) {
		
		
		Parser::toXML(schedulerMode, "schedulerMode", buf);
		
		
		}
		
	

  	
 		
		if (siteAltitudeExists) {
		
		
		Parser::toXML(siteAltitude, "siteAltitude", buf);
		
		
		}
		
	

  	
 		
		if (siteLongitudeExists) {
		
		
		Parser::toXML(siteLongitude, "siteLongitude", buf);
		
		
		}
		
	

  	
 		
		if (siteLatitudeExists) {
		
		
		Parser::toXML(siteLatitude, "siteLatitude", buf);
		
		
		}
		
	

  	
 		
		if (observingScriptExists) {
		
		
		Parser::toXML(observingScript, "observingScript", buf);
		
		
		}
		
	

  	
 		
		if (observingScriptUIDExists) {
		
		
		Parser::toXML(observingScriptUID, "observingScriptUID", buf);
		
		
		}
		
	

	
	
		
  	
 		
		
		Parser::toXML(antennaId, "antennaId", buf);
		
		
	

  	
 		
		
		Parser::toXML(sBSummaryId, "sBSummaryId", buf);
		
		
	

  	
 		
		if (scaleIdExists) {
		
		
		Parser::toXML(scaleId, "scaleId", buf);
		
		
		}
		
	

	
		
	

	

	

		
		buf.append("</row>\n");
		return buf;
	}

	/**
	 * Fill the values of this row from an XML string 
	 * that was produced by the toXML() method.
	 * @param x The XML string being used to set the values of this row.
	 */
	void ExecBlockRow::setFromXML (string rowDoc) {
		Parser row(rowDoc);
		string s = "";
		try {
	
		
	
  		
			
	  	setExecBlockId(Parser::getTag("execBlockId","ExecBlock",rowDoc));
			
		
	

	
  		
			
	  	setStartTime(Parser::getArrayTime("startTime","ExecBlock",rowDoc));
			
		
	

	
  		
			
	  	setEndTime(Parser::getArrayTime("endTime","ExecBlock",rowDoc));
			
		
	

	
  		
			
	  	setExecBlockNum(Parser::getInteger("execBlockNum","ExecBlock",rowDoc));
			
		
	

	
  		
			
	  	setExecBlockUID(Parser::getEntityRef("execBlockUID","ExecBlock",rowDoc));
			
		
	

	
  		
			
	  	setProjectUID(Parser::getEntityRef("projectUID","ExecBlock",rowDoc));
			
		
	

	
  		
			
	  	setConfigName(Parser::getString("configName","ExecBlock",rowDoc));
			
		
	

	
  		
			
	  	setTelescopeName(Parser::getString("telescopeName","ExecBlock",rowDoc));
			
		
	

	
  		
			
	  	setObserverName(Parser::getString("observerName","ExecBlock",rowDoc));
			
		
	

	
  		
			
	  	setNumObservingLog(Parser::getInteger("numObservingLog","ExecBlock",rowDoc));
			
		
	

	
  		
			
					
	  	setObservingLog(Parser::get1DString("observingLog","ExecBlock",rowDoc));
	  			
	  		
		
	

	
  		
			
	  	setSessionReference(Parser::getEntityRef("sessionReference","ExecBlock",rowDoc));
			
		
	

	
  		
			
	  	setBaseRangeMin(Parser::getLength("baseRangeMin","ExecBlock",rowDoc));
			
		
	

	
  		
			
	  	setBaseRangeMax(Parser::getLength("baseRangeMax","ExecBlock",rowDoc));
			
		
	

	
  		
			
	  	setBaseRmsMinor(Parser::getLength("baseRmsMinor","ExecBlock",rowDoc));
			
		
	

	
  		
			
	  	setBaseRmsMajor(Parser::getLength("baseRmsMajor","ExecBlock",rowDoc));
			
		
	

	
  		
			
	  	setBasePa(Parser::getAngle("basePa","ExecBlock",rowDoc));
			
		
	

	
  		
			
	  	setAborted(Parser::getBoolean("aborted","ExecBlock",rowDoc));
			
		
	

	
  		
			
	  	setNumAntenna(Parser::getInteger("numAntenna","ExecBlock",rowDoc));
			
		
	

	
  		
        if (row.isStr("<releaseDate>")) {
			
	  		setReleaseDate(Parser::getArrayTime("releaseDate","ExecBlock",rowDoc));
			
		}
 		
	

	
  		
        if (row.isStr("<schedulerMode>")) {
			
	  		setSchedulerMode(Parser::getString("schedulerMode","ExecBlock",rowDoc));
			
		}
 		
	

	
  		
        if (row.isStr("<siteAltitude>")) {
			
	  		setSiteAltitude(Parser::getLength("siteAltitude","ExecBlock",rowDoc));
			
		}
 		
	

	
  		
        if (row.isStr("<siteLongitude>")) {
			
	  		setSiteLongitude(Parser::getAngle("siteLongitude","ExecBlock",rowDoc));
			
		}
 		
	

	
  		
        if (row.isStr("<siteLatitude>")) {
			
	  		setSiteLatitude(Parser::getAngle("siteLatitude","ExecBlock",rowDoc));
			
		}
 		
	

	
  		
        if (row.isStr("<observingScript>")) {
			
	  		setObservingScript(Parser::getString("observingScript","ExecBlock",rowDoc));
			
		}
 		
	

	
  		
        if (row.isStr("<observingScriptUID>")) {
			
	  		setObservingScriptUID(Parser::getEntityRef("observingScriptUID","ExecBlock",rowDoc));
			
		}
 		
	

	
	
		
	
  		 
  		setAntennaId(Parser::get1DTag("antennaId","ExecBlock",rowDoc));
		
  	

	
  		
			
	  	setSBSummaryId(Parser::getTag("sBSummaryId","ExecBlock",rowDoc));
			
		
	

	
  		
        if (row.isStr("<scaleId>")) {
			
	  		setScaleId(Parser::getTag("scaleId","ExecBlock",rowDoc));
			
		}
 		
	

	
		
	

	

	

		} catch (const IllegalAccessException &err) {
			throw ConversionException (err.getMessage(),"ExecBlock");
		}
	}
	
	void ExecBlockRow::toBin(EndianOSStream& eoss) {
	
	
	
	
		
	execBlockId.toBin(eoss);
		
	

	
	
		
	startTime.toBin(eoss);
		
	

	
	
		
	endTime.toBin(eoss);
		
	

	
	
		
						
			eoss.writeInt(execBlockNum);
				
		
	

	
	
		
	execBlockUID.toBin(eoss);
		
	

	
	
		
	projectUID.toBin(eoss);
		
	

	
	
		
						
			eoss.writeString(configName);
				
		
	

	
	
		
						
			eoss.writeString(telescopeName);
				
		
	

	
	
		
						
			eoss.writeString(observerName);
				
		
	

	
	
		
						
			eoss.writeInt(numObservingLog);
				
		
	

	
	
		
		
			
		eoss.writeInt((int) observingLog.size());
		for (unsigned int i = 0; i < observingLog.size(); i++)
				
			eoss.writeString(observingLog.at(i));
				
				
						
		
	

	
	
		
	sessionReference.toBin(eoss);
		
	

	
	
		
	baseRangeMin.toBin(eoss);
		
	

	
	
		
	baseRangeMax.toBin(eoss);
		
	

	
	
		
	baseRmsMinor.toBin(eoss);
		
	

	
	
		
	baseRmsMajor.toBin(eoss);
		
	

	
	
		
	basePa.toBin(eoss);
		
	

	
	
		
						
			eoss.writeBoolean(aborted);
				
		
	

	
	
		
						
			eoss.writeInt(numAntenna);
				
		
	

	
	
		
	Tag::toBin(antennaId, eoss);
		
	

	
	
		
	sBSummaryId.toBin(eoss);
		
	


	
	
	eoss.writeBoolean(releaseDateExists);
	if (releaseDateExists) {
	
	
	
		
	releaseDate.toBin(eoss);
		
	

	}

	eoss.writeBoolean(schedulerModeExists);
	if (schedulerModeExists) {
	
	
	
		
						
			eoss.writeString(schedulerMode);
				
		
	

	}

	eoss.writeBoolean(siteAltitudeExists);
	if (siteAltitudeExists) {
	
	
	
		
	siteAltitude.toBin(eoss);
		
	

	}

	eoss.writeBoolean(siteLongitudeExists);
	if (siteLongitudeExists) {
	
	
	
		
	siteLongitude.toBin(eoss);
		
	

	}

	eoss.writeBoolean(siteLatitudeExists);
	if (siteLatitudeExists) {
	
	
	
		
	siteLatitude.toBin(eoss);
		
	

	}

	eoss.writeBoolean(observingScriptExists);
	if (observingScriptExists) {
	
	
	
		
						
			eoss.writeString(observingScript);
				
		
	

	}

	eoss.writeBoolean(observingScriptUIDExists);
	if (observingScriptUIDExists) {
	
	
	
		
	observingScriptUID.toBin(eoss);
		
	

	}

	eoss.writeBoolean(scaleIdExists);
	if (scaleIdExists) {
	
	
	
		
	scaleId.toBin(eoss);
		
	

	}

	}
	
void ExecBlockRow::execBlockIdFromBin(EndianIStream& eis) {
		
	
		
		
		execBlockId =  Tag::fromBin(eis);
		
	
	
}
void ExecBlockRow::startTimeFromBin(EndianIStream& eis) {
		
	
		
		
		startTime =  ArrayTime::fromBin(eis);
		
	
	
}
void ExecBlockRow::endTimeFromBin(EndianIStream& eis) {
		
	
		
		
		endTime =  ArrayTime::fromBin(eis);
		
	
	
}
void ExecBlockRow::execBlockNumFromBin(EndianIStream& eis) {
		
	
	
		
			
		execBlockNum =  eis.readInt();
			
		
	
	
}
void ExecBlockRow::execBlockUIDFromBin(EndianIStream& eis) {
		
	
		
		
		execBlockUID =  EntityRef::fromBin(eis);
		
	
	
}
void ExecBlockRow::projectUIDFromBin(EndianIStream& eis) {
		
	
		
		
		projectUID =  EntityRef::fromBin(eis);
		
	
	
}
void ExecBlockRow::configNameFromBin(EndianIStream& eis) {
		
	
	
		
			
		configName =  eis.readString();
			
		
	
	
}
void ExecBlockRow::telescopeNameFromBin(EndianIStream& eis) {
		
	
	
		
			
		telescopeName =  eis.readString();
			
		
	
	
}
void ExecBlockRow::observerNameFromBin(EndianIStream& eis) {
		
	
	
		
			
		observerName =  eis.readString();
			
		
	
	
}
void ExecBlockRow::numObservingLogFromBin(EndianIStream& eis) {
		
	
	
		
			
		numObservingLog =  eis.readInt();
			
		
	
	
}
void ExecBlockRow::observingLogFromBin(EndianIStream& eis) {
		
	
	
		
			
	
		observingLog.clear();
		
		unsigned int observingLogDim1 = eis.readInt();
		for (unsigned int  i = 0 ; i < observingLogDim1; i++)
			
			observingLog.push_back(eis.readString());
			
	

		
	
	
}
void ExecBlockRow::sessionReferenceFromBin(EndianIStream& eis) {
		
	
		
		
		sessionReference =  EntityRef::fromBin(eis);
		
	
	
}
void ExecBlockRow::baseRangeMinFromBin(EndianIStream& eis) {
		
	
		
		
		baseRangeMin =  Length::fromBin(eis);
		
	
	
}
void ExecBlockRow::baseRangeMaxFromBin(EndianIStream& eis) {
		
	
		
		
		baseRangeMax =  Length::fromBin(eis);
		
	
	
}
void ExecBlockRow::baseRmsMinorFromBin(EndianIStream& eis) {
		
	
		
		
		baseRmsMinor =  Length::fromBin(eis);
		
	
	
}
void ExecBlockRow::baseRmsMajorFromBin(EndianIStream& eis) {
		
	
		
		
		baseRmsMajor =  Length::fromBin(eis);
		
	
	
}
void ExecBlockRow::basePaFromBin(EndianIStream& eis) {
		
	
		
		
		basePa =  Angle::fromBin(eis);
		
	
	
}
void ExecBlockRow::abortedFromBin(EndianIStream& eis) {
		
	
	
		
			
		aborted =  eis.readBoolean();
			
		
	
	
}
void ExecBlockRow::numAntennaFromBin(EndianIStream& eis) {
		
	
	
		
			
		numAntenna =  eis.readInt();
			
		
	
	
}
void ExecBlockRow::antennaIdFromBin(EndianIStream& eis) {
		
	
		
		
			
	
	antennaId = Tag::from1DBin(eis);	
	

		
	
	
}
void ExecBlockRow::sBSummaryIdFromBin(EndianIStream& eis) {
		
	
		
		
		sBSummaryId =  Tag::fromBin(eis);
		
	
	
}

void ExecBlockRow::releaseDateFromBin(EndianIStream& eis) {
		
	releaseDateExists = eis.readBoolean();
	if (releaseDateExists) {
		
	
		
		
		releaseDate =  ArrayTime::fromBin(eis);
		
	

	}
	
}
void ExecBlockRow::schedulerModeFromBin(EndianIStream& eis) {
		
	schedulerModeExists = eis.readBoolean();
	if (schedulerModeExists) {
		
	
	
		
			
		schedulerMode =  eis.readString();
			
		
	

	}
	
}
void ExecBlockRow::siteAltitudeFromBin(EndianIStream& eis) {
		
	siteAltitudeExists = eis.readBoolean();
	if (siteAltitudeExists) {
		
	
		
		
		siteAltitude =  Length::fromBin(eis);
		
	

	}
	
}
void ExecBlockRow::siteLongitudeFromBin(EndianIStream& eis) {
		
	siteLongitudeExists = eis.readBoolean();
	if (siteLongitudeExists) {
		
	
		
		
		siteLongitude =  Angle::fromBin(eis);
		
	

	}
	
}
void ExecBlockRow::siteLatitudeFromBin(EndianIStream& eis) {
		
	siteLatitudeExists = eis.readBoolean();
	if (siteLatitudeExists) {
		
	
		
		
		siteLatitude =  Angle::fromBin(eis);
		
	

	}
	
}
void ExecBlockRow::observingScriptFromBin(EndianIStream& eis) {
		
	observingScriptExists = eis.readBoolean();
	if (observingScriptExists) {
		
	
	
		
			
		observingScript =  eis.readString();
			
		
	

	}
	
}
void ExecBlockRow::observingScriptUIDFromBin(EndianIStream& eis) {
		
	observingScriptUIDExists = eis.readBoolean();
	if (observingScriptUIDExists) {
		
	
		
		
		observingScriptUID =  EntityRef::fromBin(eis);
		
	

	}
	
}
void ExecBlockRow::scaleIdFromBin(EndianIStream& eis) {
		
	scaleIdExists = eis.readBoolean();
	if (scaleIdExists) {
		
	
		
		
		scaleId =  Tag::fromBin(eis);
		
	

	}
	
}
	
	
	ExecBlockRow* ExecBlockRow::fromBin(EndianIStream& eis, ExecBlockTable& table, const vector<string>& attributesSeq) {
		ExecBlockRow* row = new  ExecBlockRow(table);
		
		map<string, ExecBlockAttributeFromBin>::iterator iter ;
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
					throw ConversionException("There is not method to read an attribute '"+attributesSeq.at(i)+"'.", "ExecBlockTable");
			}
				
		}				
		return row;
	}

	//
	// A collection of methods to set the value of the attributes from their textual value in the XML representation
	// of one row.
	//
	
	// Convert a string into an Tag 
	void ExecBlockRow::execBlockIdFromText(const string & s) {
		 
          
		execBlockId = ASDMValuesParser::parse<Tag>(s);
          
		
	}
	
	
	// Convert a string into an ArrayTime 
	void ExecBlockRow::startTimeFromText(const string & s) {
		 
          
		startTime = ASDMValuesParser::parse<ArrayTime>(s);
          
		
	}
	
	
	// Convert a string into an ArrayTime 
	void ExecBlockRow::endTimeFromText(const string & s) {
		 
          
		endTime = ASDMValuesParser::parse<ArrayTime>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void ExecBlockRow::execBlockNumFromText(const string & s) {
		 
          
		execBlockNum = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	
	
	// Convert a string into an String 
	void ExecBlockRow::configNameFromText(const string & s) {
		 
          
		configName = ASDMValuesParser::parse<string>(s);
          
		
	}
	
	
	// Convert a string into an String 
	void ExecBlockRow::telescopeNameFromText(const string & s) {
		 
          
		telescopeName = ASDMValuesParser::parse<string>(s);
          
		
	}
	
	
	// Convert a string into an String 
	void ExecBlockRow::observerNameFromText(const string & s) {
		 
          
		observerName = ASDMValuesParser::parse<string>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void ExecBlockRow::numObservingLogFromText(const string & s) {
		 
          
		numObservingLog = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	// Convert a string into an String 
	void ExecBlockRow::observingLogFromText(const string & s) {
		 
          
		observingLog = ASDMValuesParser::parse1D<string>(s);
          
		
	}
	
	
	
	// Convert a string into an Length 
	void ExecBlockRow::baseRangeMinFromText(const string & s) {
		 
          
		baseRangeMin = ASDMValuesParser::parse<Length>(s);
          
		
	}
	
	
	// Convert a string into an Length 
	void ExecBlockRow::baseRangeMaxFromText(const string & s) {
		 
          
		baseRangeMax = ASDMValuesParser::parse<Length>(s);
          
		
	}
	
	
	// Convert a string into an Length 
	void ExecBlockRow::baseRmsMinorFromText(const string & s) {
		 
          
		baseRmsMinor = ASDMValuesParser::parse<Length>(s);
          
		
	}
	
	
	// Convert a string into an Length 
	void ExecBlockRow::baseRmsMajorFromText(const string & s) {
		 
          
		baseRmsMajor = ASDMValuesParser::parse<Length>(s);
          
		
	}
	
	
	// Convert a string into an Angle 
	void ExecBlockRow::basePaFromText(const string & s) {
		 
          
		basePa = ASDMValuesParser::parse<Angle>(s);
          
		
	}
	
	
	// Convert a string into an boolean 
	void ExecBlockRow::abortedFromText(const string & s) {
		 
          
		aborted = ASDMValuesParser::parse<bool>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void ExecBlockRow::numAntennaFromText(const string & s) {
		 
          
		numAntenna = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	// Convert a string into an Tag 
	void ExecBlockRow::antennaIdFromText(const string & s) {
		 
          
		antennaId = ASDMValuesParser::parse1D<Tag>(s);
          
		
	}
	
	
	// Convert a string into an Tag 
	void ExecBlockRow::sBSummaryIdFromText(const string & s) {
		 
          
		sBSummaryId = ASDMValuesParser::parse<Tag>(s);
          
		
	}
	

	
	// Convert a string into an ArrayTime 
	void ExecBlockRow::releaseDateFromText(const string & s) {
		releaseDateExists = true;
		 
          
		releaseDate = ASDMValuesParser::parse<ArrayTime>(s);
          
		
	}
	
	
	// Convert a string into an String 
	void ExecBlockRow::schedulerModeFromText(const string & s) {
		schedulerModeExists = true;
		 
          
		schedulerMode = ASDMValuesParser::parse<string>(s);
          
		
	}
	
	
	// Convert a string into an Length 
	void ExecBlockRow::siteAltitudeFromText(const string & s) {
		siteAltitudeExists = true;
		 
          
		siteAltitude = ASDMValuesParser::parse<Length>(s);
          
		
	}
	
	
	// Convert a string into an Angle 
	void ExecBlockRow::siteLongitudeFromText(const string & s) {
		siteLongitudeExists = true;
		 
          
		siteLongitude = ASDMValuesParser::parse<Angle>(s);
          
		
	}
	
	
	// Convert a string into an Angle 
	void ExecBlockRow::siteLatitudeFromText(const string & s) {
		siteLatitudeExists = true;
		 
          
		siteLatitude = ASDMValuesParser::parse<Angle>(s);
          
		
	}
	
	
	// Convert a string into an String 
	void ExecBlockRow::observingScriptFromText(const string & s) {
		observingScriptExists = true;
		 
          
		observingScript = ASDMValuesParser::parse<string>(s);
          
		
	}
	
	
	
	// Convert a string into an Tag 
	void ExecBlockRow::scaleIdFromText(const string & s) {
		scaleIdExists = true;
		 
          
		scaleId = ASDMValuesParser::parse<Tag>(s);
          
		
	}
	
	
	
	void ExecBlockRow::fromText(const std::string& attributeName, const std::string&  t) {
		map<string, ExecBlockAttributeFromText>::iterator iter;
		if ((iter = fromTextMethods.find(attributeName)) == fromTextMethods.end())
			throw ConversionException("I do not know what to do with '"+attributeName+"' and its content '"+t+"' (while parsing an XML document)", "ExecBlockTable");
		(this->*(iter->second))(t);
	}
			
	////////////////////////////////////////////////
	// Intrinsic Table Attributes getters/setters //
	////////////////////////////////////////////////
	
	

	
 	/**
 	 * Get execBlockId.
 	 * @return execBlockId as Tag
 	 */
 	Tag ExecBlockRow::getExecBlockId() const {
	
  		return execBlockId;
 	}

 	/**
 	 * Set execBlockId with the specified Tag.
 	 * @param execBlockId The Tag value to which execBlockId is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void ExecBlockRow::setExecBlockId (Tag execBlockId)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("execBlockId", "ExecBlock");
		
  		}
  	
 		this->execBlockId = execBlockId;
	
 	}
	
	

	

	
 	/**
 	 * Get startTime.
 	 * @return startTime as ArrayTime
 	 */
 	ArrayTime ExecBlockRow::getStartTime() const {
	
  		return startTime;
 	}

 	/**
 	 * Set startTime with the specified ArrayTime.
 	 * @param startTime The ArrayTime value to which startTime is to be set.
 	 
 	
 		
 	 */
 	void ExecBlockRow::setStartTime (ArrayTime startTime)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->startTime = startTime;
	
 	}
	
	

	

	
 	/**
 	 * Get endTime.
 	 * @return endTime as ArrayTime
 	 */
 	ArrayTime ExecBlockRow::getEndTime() const {
	
  		return endTime;
 	}

 	/**
 	 * Set endTime with the specified ArrayTime.
 	 * @param endTime The ArrayTime value to which endTime is to be set.
 	 
 	
 		
 	 */
 	void ExecBlockRow::setEndTime (ArrayTime endTime)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->endTime = endTime;
	
 	}
	
	

	

	
 	/**
 	 * Get execBlockNum.
 	 * @return execBlockNum as int
 	 */
 	int ExecBlockRow::getExecBlockNum() const {
	
  		return execBlockNum;
 	}

 	/**
 	 * Set execBlockNum with the specified int.
 	 * @param execBlockNum The int value to which execBlockNum is to be set.
 	 
 	
 		
 	 */
 	void ExecBlockRow::setExecBlockNum (int execBlockNum)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->execBlockNum = execBlockNum;
	
 	}
	
	

	

	
 	/**
 	 * Get execBlockUID.
 	 * @return execBlockUID as EntityRef
 	 */
 	EntityRef ExecBlockRow::getExecBlockUID() const {
	
  		return execBlockUID;
 	}

 	/**
 	 * Set execBlockUID with the specified EntityRef.
 	 * @param execBlockUID The EntityRef value to which execBlockUID is to be set.
 	 
 	
 		
 	 */
 	void ExecBlockRow::setExecBlockUID (EntityRef execBlockUID)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->execBlockUID = execBlockUID;
	
 	}
	
	

	

	
 	/**
 	 * Get projectUID.
 	 * @return projectUID as EntityRef
 	 */
 	EntityRef ExecBlockRow::getProjectUID() const {
	
  		return projectUID;
 	}

 	/**
 	 * Set projectUID with the specified EntityRef.
 	 * @param projectUID The EntityRef value to which projectUID is to be set.
 	 
 	
 		
 	 */
 	void ExecBlockRow::setProjectUID (EntityRef projectUID)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->projectUID = projectUID;
	
 	}
	
	

	

	
 	/**
 	 * Get configName.
 	 * @return configName as std::string
 	 */
 	std::string ExecBlockRow::getConfigName() const {
	
  		return configName;
 	}

 	/**
 	 * Set configName with the specified std::string.
 	 * @param configName The std::string value to which configName is to be set.
 	 
 	
 		
 	 */
 	void ExecBlockRow::setConfigName (std::string configName)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->configName = configName;
	
 	}
	
	

	

	
 	/**
 	 * Get telescopeName.
 	 * @return telescopeName as std::string
 	 */
 	std::string ExecBlockRow::getTelescopeName() const {
	
  		return telescopeName;
 	}

 	/**
 	 * Set telescopeName with the specified std::string.
 	 * @param telescopeName The std::string value to which telescopeName is to be set.
 	 
 	
 		
 	 */
 	void ExecBlockRow::setTelescopeName (std::string telescopeName)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->telescopeName = telescopeName;
	
 	}
	
	

	

	
 	/**
 	 * Get observerName.
 	 * @return observerName as std::string
 	 */
 	std::string ExecBlockRow::getObserverName() const {
	
  		return observerName;
 	}

 	/**
 	 * Set observerName with the specified std::string.
 	 * @param observerName The std::string value to which observerName is to be set.
 	 
 	
 		
 	 */
 	void ExecBlockRow::setObserverName (std::string observerName)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->observerName = observerName;
	
 	}
	
	

	

	
 	/**
 	 * Get numObservingLog.
 	 * @return numObservingLog as int
 	 */
 	int ExecBlockRow::getNumObservingLog() const {
	
  		return numObservingLog;
 	}

 	/**
 	 * Set numObservingLog with the specified int.
 	 * @param numObservingLog The int value to which numObservingLog is to be set.
 	 
 	
 		
 	 */
 	void ExecBlockRow::setNumObservingLog (int numObservingLog)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->numObservingLog = numObservingLog;
	
 	}
	
	

	

	
 	/**
 	 * Get observingLog.
 	 * @return observingLog as std::vector<std::string >
 	 */
 	std::vector<std::string > ExecBlockRow::getObservingLog() const {
	
  		return observingLog;
 	}

 	/**
 	 * Set observingLog with the specified std::vector<std::string >.
 	 * @param observingLog The std::vector<std::string > value to which observingLog is to be set.
 	 
 	
 		
 	 */
 	void ExecBlockRow::setObservingLog (std::vector<std::string > observingLog)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->observingLog = observingLog;
	
 	}
	
	

	

	
 	/**
 	 * Get sessionReference.
 	 * @return sessionReference as EntityRef
 	 */
 	EntityRef ExecBlockRow::getSessionReference() const {
	
  		return sessionReference;
 	}

 	/**
 	 * Set sessionReference with the specified EntityRef.
 	 * @param sessionReference The EntityRef value to which sessionReference is to be set.
 	 
 	
 		
 	 */
 	void ExecBlockRow::setSessionReference (EntityRef sessionReference)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->sessionReference = sessionReference;
	
 	}
	
	

	

	
 	/**
 	 * Get baseRangeMin.
 	 * @return baseRangeMin as Length
 	 */
 	Length ExecBlockRow::getBaseRangeMin() const {
	
  		return baseRangeMin;
 	}

 	/**
 	 * Set baseRangeMin with the specified Length.
 	 * @param baseRangeMin The Length value to which baseRangeMin is to be set.
 	 
 	
 		
 	 */
 	void ExecBlockRow::setBaseRangeMin (Length baseRangeMin)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->baseRangeMin = baseRangeMin;
	
 	}
	
	

	

	
 	/**
 	 * Get baseRangeMax.
 	 * @return baseRangeMax as Length
 	 */
 	Length ExecBlockRow::getBaseRangeMax() const {
	
  		return baseRangeMax;
 	}

 	/**
 	 * Set baseRangeMax with the specified Length.
 	 * @param baseRangeMax The Length value to which baseRangeMax is to be set.
 	 
 	
 		
 	 */
 	void ExecBlockRow::setBaseRangeMax (Length baseRangeMax)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->baseRangeMax = baseRangeMax;
	
 	}
	
	

	

	
 	/**
 	 * Get baseRmsMinor.
 	 * @return baseRmsMinor as Length
 	 */
 	Length ExecBlockRow::getBaseRmsMinor() const {
	
  		return baseRmsMinor;
 	}

 	/**
 	 * Set baseRmsMinor with the specified Length.
 	 * @param baseRmsMinor The Length value to which baseRmsMinor is to be set.
 	 
 	
 		
 	 */
 	void ExecBlockRow::setBaseRmsMinor (Length baseRmsMinor)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->baseRmsMinor = baseRmsMinor;
	
 	}
	
	

	

	
 	/**
 	 * Get baseRmsMajor.
 	 * @return baseRmsMajor as Length
 	 */
 	Length ExecBlockRow::getBaseRmsMajor() const {
	
  		return baseRmsMajor;
 	}

 	/**
 	 * Set baseRmsMajor with the specified Length.
 	 * @param baseRmsMajor The Length value to which baseRmsMajor is to be set.
 	 
 	
 		
 	 */
 	void ExecBlockRow::setBaseRmsMajor (Length baseRmsMajor)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->baseRmsMajor = baseRmsMajor;
	
 	}
	
	

	

	
 	/**
 	 * Get basePa.
 	 * @return basePa as Angle
 	 */
 	Angle ExecBlockRow::getBasePa() const {
	
  		return basePa;
 	}

 	/**
 	 * Set basePa with the specified Angle.
 	 * @param basePa The Angle value to which basePa is to be set.
 	 
 	
 		
 	 */
 	void ExecBlockRow::setBasePa (Angle basePa)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->basePa = basePa;
	
 	}
	
	

	

	
 	/**
 	 * Get aborted.
 	 * @return aborted as bool
 	 */
 	bool ExecBlockRow::getAborted() const {
	
  		return aborted;
 	}

 	/**
 	 * Set aborted with the specified bool.
 	 * @param aborted The bool value to which aborted is to be set.
 	 
 	
 		
 	 */
 	void ExecBlockRow::setAborted (bool aborted)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->aborted = aborted;
	
 	}
	
	

	

	
 	/**
 	 * Get numAntenna.
 	 * @return numAntenna as int
 	 */
 	int ExecBlockRow::getNumAntenna() const {
	
  		return numAntenna;
 	}

 	/**
 	 * Set numAntenna with the specified int.
 	 * @param numAntenna The int value to which numAntenna is to be set.
 	 
 	
 		
 	 */
 	void ExecBlockRow::setNumAntenna (int numAntenna)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->numAntenna = numAntenna;
	
 	}
	
	

	
	/**
	 * The attribute releaseDate is optional. Return true if this attribute exists.
	 * @return true if and only if the releaseDate attribute exists. 
	 */
	bool ExecBlockRow::isReleaseDateExists() const {
		return releaseDateExists;
	}
	

	
 	/**
 	 * Get releaseDate, which is optional.
 	 * @return releaseDate as ArrayTime
 	 * @throw IllegalAccessException If releaseDate does not exist.
 	 */
 	ArrayTime ExecBlockRow::getReleaseDate() const  {
		if (!releaseDateExists) {
			throw IllegalAccessException("releaseDate", "ExecBlock");
		}
	
  		return releaseDate;
 	}

 	/**
 	 * Set releaseDate with the specified ArrayTime.
 	 * @param releaseDate The ArrayTime value to which releaseDate is to be set.
 	 
 	
 	 */
 	void ExecBlockRow::setReleaseDate (ArrayTime releaseDate) {
	
 		this->releaseDate = releaseDate;
	
		releaseDateExists = true;
	
 	}
	
	
	/**
	 * Mark releaseDate, which is an optional field, as non-existent.
	 */
	void ExecBlockRow::clearReleaseDate () {
		releaseDateExists = false;
	}
	

	
	/**
	 * The attribute schedulerMode is optional. Return true if this attribute exists.
	 * @return true if and only if the schedulerMode attribute exists. 
	 */
	bool ExecBlockRow::isSchedulerModeExists() const {
		return schedulerModeExists;
	}
	

	
 	/**
 	 * Get schedulerMode, which is optional.
 	 * @return schedulerMode as std::string
 	 * @throw IllegalAccessException If schedulerMode does not exist.
 	 */
 	std::string ExecBlockRow::getSchedulerMode() const  {
		if (!schedulerModeExists) {
			throw IllegalAccessException("schedulerMode", "ExecBlock");
		}
	
  		return schedulerMode;
 	}

 	/**
 	 * Set schedulerMode with the specified std::string.
 	 * @param schedulerMode The std::string value to which schedulerMode is to be set.
 	 
 	
 	 */
 	void ExecBlockRow::setSchedulerMode (std::string schedulerMode) {
	
 		this->schedulerMode = schedulerMode;
	
		schedulerModeExists = true;
	
 	}
	
	
	/**
	 * Mark schedulerMode, which is an optional field, as non-existent.
	 */
	void ExecBlockRow::clearSchedulerMode () {
		schedulerModeExists = false;
	}
	

	
	/**
	 * The attribute siteAltitude is optional. Return true if this attribute exists.
	 * @return true if and only if the siteAltitude attribute exists. 
	 */
	bool ExecBlockRow::isSiteAltitudeExists() const {
		return siteAltitudeExists;
	}
	

	
 	/**
 	 * Get siteAltitude, which is optional.
 	 * @return siteAltitude as Length
 	 * @throw IllegalAccessException If siteAltitude does not exist.
 	 */
 	Length ExecBlockRow::getSiteAltitude() const  {
		if (!siteAltitudeExists) {
			throw IllegalAccessException("siteAltitude", "ExecBlock");
		}
	
  		return siteAltitude;
 	}

 	/**
 	 * Set siteAltitude with the specified Length.
 	 * @param siteAltitude The Length value to which siteAltitude is to be set.
 	 
 	
 	 */
 	void ExecBlockRow::setSiteAltitude (Length siteAltitude) {
	
 		this->siteAltitude = siteAltitude;
	
		siteAltitudeExists = true;
	
 	}
	
	
	/**
	 * Mark siteAltitude, which is an optional field, as non-existent.
	 */
	void ExecBlockRow::clearSiteAltitude () {
		siteAltitudeExists = false;
	}
	

	
	/**
	 * The attribute siteLongitude is optional. Return true if this attribute exists.
	 * @return true if and only if the siteLongitude attribute exists. 
	 */
	bool ExecBlockRow::isSiteLongitudeExists() const {
		return siteLongitudeExists;
	}
	

	
 	/**
 	 * Get siteLongitude, which is optional.
 	 * @return siteLongitude as Angle
 	 * @throw IllegalAccessException If siteLongitude does not exist.
 	 */
 	Angle ExecBlockRow::getSiteLongitude() const  {
		if (!siteLongitudeExists) {
			throw IllegalAccessException("siteLongitude", "ExecBlock");
		}
	
  		return siteLongitude;
 	}

 	/**
 	 * Set siteLongitude with the specified Angle.
 	 * @param siteLongitude The Angle value to which siteLongitude is to be set.
 	 
 	
 	 */
 	void ExecBlockRow::setSiteLongitude (Angle siteLongitude) {
	
 		this->siteLongitude = siteLongitude;
	
		siteLongitudeExists = true;
	
 	}
	
	
	/**
	 * Mark siteLongitude, which is an optional field, as non-existent.
	 */
	void ExecBlockRow::clearSiteLongitude () {
		siteLongitudeExists = false;
	}
	

	
	/**
	 * The attribute siteLatitude is optional. Return true if this attribute exists.
	 * @return true if and only if the siteLatitude attribute exists. 
	 */
	bool ExecBlockRow::isSiteLatitudeExists() const {
		return siteLatitudeExists;
	}
	

	
 	/**
 	 * Get siteLatitude, which is optional.
 	 * @return siteLatitude as Angle
 	 * @throw IllegalAccessException If siteLatitude does not exist.
 	 */
 	Angle ExecBlockRow::getSiteLatitude() const  {
		if (!siteLatitudeExists) {
			throw IllegalAccessException("siteLatitude", "ExecBlock");
		}
	
  		return siteLatitude;
 	}

 	/**
 	 * Set siteLatitude with the specified Angle.
 	 * @param siteLatitude The Angle value to which siteLatitude is to be set.
 	 
 	
 	 */
 	void ExecBlockRow::setSiteLatitude (Angle siteLatitude) {
	
 		this->siteLatitude = siteLatitude;
	
		siteLatitudeExists = true;
	
 	}
	
	
	/**
	 * Mark siteLatitude, which is an optional field, as non-existent.
	 */
	void ExecBlockRow::clearSiteLatitude () {
		siteLatitudeExists = false;
	}
	

	
	/**
	 * The attribute observingScript is optional. Return true if this attribute exists.
	 * @return true if and only if the observingScript attribute exists. 
	 */
	bool ExecBlockRow::isObservingScriptExists() const {
		return observingScriptExists;
	}
	

	
 	/**
 	 * Get observingScript, which is optional.
 	 * @return observingScript as std::string
 	 * @throw IllegalAccessException If observingScript does not exist.
 	 */
 	std::string ExecBlockRow::getObservingScript() const  {
		if (!observingScriptExists) {
			throw IllegalAccessException("observingScript", "ExecBlock");
		}
	
  		return observingScript;
 	}

 	/**
 	 * Set observingScript with the specified std::string.
 	 * @param observingScript The std::string value to which observingScript is to be set.
 	 
 	
 	 */
 	void ExecBlockRow::setObservingScript (std::string observingScript) {
	
 		this->observingScript = observingScript;
	
		observingScriptExists = true;
	
 	}
	
	
	/**
	 * Mark observingScript, which is an optional field, as non-existent.
	 */
	void ExecBlockRow::clearObservingScript () {
		observingScriptExists = false;
	}
	

	
	/**
	 * The attribute observingScriptUID is optional. Return true if this attribute exists.
	 * @return true if and only if the observingScriptUID attribute exists. 
	 */
	bool ExecBlockRow::isObservingScriptUIDExists() const {
		return observingScriptUIDExists;
	}
	

	
 	/**
 	 * Get observingScriptUID, which is optional.
 	 * @return observingScriptUID as EntityRef
 	 * @throw IllegalAccessException If observingScriptUID does not exist.
 	 */
 	EntityRef ExecBlockRow::getObservingScriptUID() const  {
		if (!observingScriptUIDExists) {
			throw IllegalAccessException("observingScriptUID", "ExecBlock");
		}
	
  		return observingScriptUID;
 	}

 	/**
 	 * Set observingScriptUID with the specified EntityRef.
 	 * @param observingScriptUID The EntityRef value to which observingScriptUID is to be set.
 	 
 	
 	 */
 	void ExecBlockRow::setObservingScriptUID (EntityRef observingScriptUID) {
	
 		this->observingScriptUID = observingScriptUID;
	
		observingScriptUIDExists = true;
	
 	}
	
	
	/**
	 * Mark observingScriptUID, which is an optional field, as non-existent.
	 */
	void ExecBlockRow::clearObservingScriptUID () {
		observingScriptUIDExists = false;
	}
	

	
	///////////////////////////////////////////////
	// Extrinsic Table Attributes getters/setters//
	///////////////////////////////////////////////
	
	

	
 	/**
 	 * Get antennaId.
 	 * @return antennaId as std::vector<Tag> 
 	 */
 	std::vector<Tag>  ExecBlockRow::getAntennaId() const {
	
  		return antennaId;
 	}

 	/**
 	 * Set antennaId with the specified std::vector<Tag> .
 	 * @param antennaId The std::vector<Tag>  value to which antennaId is to be set.
 	 
 	
 		
 	 */
 	void ExecBlockRow::setAntennaId (std::vector<Tag>  antennaId)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->antennaId = antennaId;
	
 	}
	
	

	

	
 	/**
 	 * Get sBSummaryId.
 	 * @return sBSummaryId as Tag
 	 */
 	Tag ExecBlockRow::getSBSummaryId() const {
	
  		return sBSummaryId;
 	}

 	/**
 	 * Set sBSummaryId with the specified Tag.
 	 * @param sBSummaryId The Tag value to which sBSummaryId is to be set.
 	 
 	
 		
 	 */
 	void ExecBlockRow::setSBSummaryId (Tag sBSummaryId)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->sBSummaryId = sBSummaryId;
	
 	}
	
	

	
	/**
	 * The attribute scaleId is optional. Return true if this attribute exists.
	 * @return true if and only if the scaleId attribute exists. 
	 */
	bool ExecBlockRow::isScaleIdExists() const {
		return scaleIdExists;
	}
	

	
 	/**
 	 * Get scaleId, which is optional.
 	 * @return scaleId as Tag
 	 * @throw IllegalAccessException If scaleId does not exist.
 	 */
 	Tag ExecBlockRow::getScaleId() const  {
		if (!scaleIdExists) {
			throw IllegalAccessException("scaleId", "ExecBlock");
		}
	
  		return scaleId;
 	}

 	/**
 	 * Set scaleId with the specified Tag.
 	 * @param scaleId The Tag value to which scaleId is to be set.
 	 
 	
 	 */
 	void ExecBlockRow::setScaleId (Tag scaleId) {
	
 		this->scaleId = scaleId;
	
		scaleIdExists = true;
	
 	}
	
	
	/**
	 * Mark scaleId, which is an optional field, as non-existent.
	 */
	void ExecBlockRow::clearScaleId () {
		scaleIdExists = false;
	}
	


	//////////////////////////////////////
	// Links Attributes getters/setters //
	//////////////////////////////////////
	
	
 		
 	/**
 	 * Set antennaId[i] with the specified Tag.
 	 * @param i The index in antennaId where to set the Tag value.
 	 * @param antennaId The Tag value to which antennaId[i] is to be set. 
	 		
 	 * @throws IndexOutOfBoundsException
  	 */
  	void ExecBlockRow::setAntennaId (int i, Tag antennaId)  {
  	  	if (hasBeenAdded) {
  	  		
  		}
  		if ((i < 0) || (i > ((int) this->antennaId.size())))
  			throw OutOfBoundsException("Index out of bounds during a set operation on attribute antennaId in table ExecBlockTable");
  		std::vector<Tag> ::iterator iter = this->antennaId.begin();
  		int j = 0;
  		while (j < i) {
  			j++; iter++;
  		}
  		this->antennaId.insert(this->antennaId.erase(iter), antennaId); 
  	}	
 			
	
	
	
		
/**
 * Append a Tag to antennaId.
 * @param id the Tag to be appended to antennaId
 */
 void ExecBlockRow::addAntennaId(Tag id){
 	antennaId.push_back(id);
}

/**
 * Append an array of Tag to antennaId.
 * @param id an array of Tag to be appended to antennaId
 */
 void ExecBlockRow::addAntennaId(const std::vector<Tag> & id) {
 	for (unsigned int i=0; i < id.size(); i++)
 		antennaId.push_back(id.at(i));
 }
 

 /**
  * Returns the Tag stored in antennaId at position i.
  *
  */
 const Tag ExecBlockRow::getAntennaId(int i) {
 	return antennaId.at(i);
 }
 
 /**
  * Returns the AntennaRow linked to this row via the Tag stored in antennaId
  * at position i.
  */
 AntennaRow* ExecBlockRow::getAntennaUsingAntennaId(int i) {
 	return table.getContainer().getAntenna().getRowByKey(antennaId.at(i));
 } 
 
 /**
  * Returns the vector of AntennaRow* linked to this row via the Tags stored in antennaId
  *
  */
 vector<AntennaRow *> ExecBlockRow::getAntennasUsingAntennaId() {
 	vector<AntennaRow *> result;
 	for (unsigned int i = 0; i < antennaId.size(); i++)
 		result.push_back(table.getContainer().getAntenna().getRowByKey(antennaId.at(i)));
 		
 	return result;
 }
  

	

	
	
	
		

	/**
	 * Returns the pointer to the row in the SBSummary table having SBSummary.sBSummaryId == sBSummaryId
	 * @return a SBSummaryRow*
	 * 
	 
	 */
	 SBSummaryRow* ExecBlockRow::getSBSummaryUsingSBSummaryId() {
	 
	 	return table.getContainer().getSBSummary().getRowByKey(sBSummaryId);
	 }
	 

	

	
	
	
		

	/**
	 * Returns the pointer to the row in the Scale table having Scale.scaleId == scaleId
	 * @return a ScaleRow*
	 * 
	 
	 * throws IllegalAccessException
	 
	 */
	 ScaleRow* ExecBlockRow::getScaleUsingScaleId() {
	 
	 	if (!scaleIdExists)
	 		throw IllegalAccessException();	 		 
	 
	 	return table.getContainer().getScale().getRowByKey(scaleId);
	 }
	 

	

	
	/**
	 * Create a ExecBlockRow.
	 * <p>
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 */ 
	ExecBlockRow::ExecBlockRow (ExecBlockTable &t) : table(t) {
		hasBeenAdded = false;
		
	
	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	
		releaseDateExists = false;
	

	
		schedulerModeExists = false;
	

	
		siteAltitudeExists = false;
	

	
		siteLongitudeExists = false;
	

	
		siteLatitudeExists = false;
	

	
		observingScriptExists = false;
	

	
		observingScriptUIDExists = false;
	

	
	

	

	
		scaleIdExists = false;
	

	
	
	
	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	
	
	 fromBinMethods["execBlockId"] = &ExecBlockRow::execBlockIdFromBin; 
	 fromBinMethods["startTime"] = &ExecBlockRow::startTimeFromBin; 
	 fromBinMethods["endTime"] = &ExecBlockRow::endTimeFromBin; 
	 fromBinMethods["execBlockNum"] = &ExecBlockRow::execBlockNumFromBin; 
	 fromBinMethods["execBlockUID"] = &ExecBlockRow::execBlockUIDFromBin; 
	 fromBinMethods["projectUID"] = &ExecBlockRow::projectUIDFromBin; 
	 fromBinMethods["configName"] = &ExecBlockRow::configNameFromBin; 
	 fromBinMethods["telescopeName"] = &ExecBlockRow::telescopeNameFromBin; 
	 fromBinMethods["observerName"] = &ExecBlockRow::observerNameFromBin; 
	 fromBinMethods["numObservingLog"] = &ExecBlockRow::numObservingLogFromBin; 
	 fromBinMethods["observingLog"] = &ExecBlockRow::observingLogFromBin; 
	 fromBinMethods["sessionReference"] = &ExecBlockRow::sessionReferenceFromBin; 
	 fromBinMethods["baseRangeMin"] = &ExecBlockRow::baseRangeMinFromBin; 
	 fromBinMethods["baseRangeMax"] = &ExecBlockRow::baseRangeMaxFromBin; 
	 fromBinMethods["baseRmsMinor"] = &ExecBlockRow::baseRmsMinorFromBin; 
	 fromBinMethods["baseRmsMajor"] = &ExecBlockRow::baseRmsMajorFromBin; 
	 fromBinMethods["basePa"] = &ExecBlockRow::basePaFromBin; 
	 fromBinMethods["aborted"] = &ExecBlockRow::abortedFromBin; 
	 fromBinMethods["numAntenna"] = &ExecBlockRow::numAntennaFromBin; 
	 fromBinMethods["antennaId"] = &ExecBlockRow::antennaIdFromBin; 
	 fromBinMethods["sBSummaryId"] = &ExecBlockRow::sBSummaryIdFromBin; 
		
	
	 fromBinMethods["releaseDate"] = &ExecBlockRow::releaseDateFromBin; 
	 fromBinMethods["schedulerMode"] = &ExecBlockRow::schedulerModeFromBin; 
	 fromBinMethods["siteAltitude"] = &ExecBlockRow::siteAltitudeFromBin; 
	 fromBinMethods["siteLongitude"] = &ExecBlockRow::siteLongitudeFromBin; 
	 fromBinMethods["siteLatitude"] = &ExecBlockRow::siteLatitudeFromBin; 
	 fromBinMethods["observingScript"] = &ExecBlockRow::observingScriptFromBin; 
	 fromBinMethods["observingScriptUID"] = &ExecBlockRow::observingScriptUIDFromBin; 
	 fromBinMethods["scaleId"] = &ExecBlockRow::scaleIdFromBin; 
	
	
	
	
				 
	fromTextMethods["execBlockId"] = &ExecBlockRow::execBlockIdFromText;
		 
	
				 
	fromTextMethods["startTime"] = &ExecBlockRow::startTimeFromText;
		 
	
				 
	fromTextMethods["endTime"] = &ExecBlockRow::endTimeFromText;
		 
	
				 
	fromTextMethods["execBlockNum"] = &ExecBlockRow::execBlockNumFromText;
		 
	
		 
	
		 
	
				 
	fromTextMethods["configName"] = &ExecBlockRow::configNameFromText;
		 
	
				 
	fromTextMethods["telescopeName"] = &ExecBlockRow::telescopeNameFromText;
		 
	
				 
	fromTextMethods["observerName"] = &ExecBlockRow::observerNameFromText;
		 
	
				 
	fromTextMethods["numObservingLog"] = &ExecBlockRow::numObservingLogFromText;
		 
	
				 
	fromTextMethods["observingLog"] = &ExecBlockRow::observingLogFromText;
		 
	
		 
	
				 
	fromTextMethods["baseRangeMin"] = &ExecBlockRow::baseRangeMinFromText;
		 
	
				 
	fromTextMethods["baseRangeMax"] = &ExecBlockRow::baseRangeMaxFromText;
		 
	
				 
	fromTextMethods["baseRmsMinor"] = &ExecBlockRow::baseRmsMinorFromText;
		 
	
				 
	fromTextMethods["baseRmsMajor"] = &ExecBlockRow::baseRmsMajorFromText;
		 
	
				 
	fromTextMethods["basePa"] = &ExecBlockRow::basePaFromText;
		 
	
				 
	fromTextMethods["aborted"] = &ExecBlockRow::abortedFromText;
		 
	
				 
	fromTextMethods["numAntenna"] = &ExecBlockRow::numAntennaFromText;
		 
	
				 
	fromTextMethods["antennaId"] = &ExecBlockRow::antennaIdFromText;
		 
	
				 
	fromTextMethods["sBSummaryId"] = &ExecBlockRow::sBSummaryIdFromText;
		 
	

	 
				
	fromTextMethods["releaseDate"] = &ExecBlockRow::releaseDateFromText;
		 	
	 
				
	fromTextMethods["schedulerMode"] = &ExecBlockRow::schedulerModeFromText;
		 	
	 
				
	fromTextMethods["siteAltitude"] = &ExecBlockRow::siteAltitudeFromText;
		 	
	 
				
	fromTextMethods["siteLongitude"] = &ExecBlockRow::siteLongitudeFromText;
		 	
	 
				
	fromTextMethods["siteLatitude"] = &ExecBlockRow::siteLatitudeFromText;
		 	
	 
				
	fromTextMethods["observingScript"] = &ExecBlockRow::observingScriptFromText;
		 	
	 
		 	
	 
				
	fromTextMethods["scaleId"] = &ExecBlockRow::scaleIdFromText;
		 	
		
	}
	
	ExecBlockRow::ExecBlockRow (ExecBlockTable &t, ExecBlockRow *row) : table(t) {
		hasBeenAdded = false;
		
		if (row == 0) {
	
	
	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	
		releaseDateExists = false;
	

	
		schedulerModeExists = false;
	

	
		siteAltitudeExists = false;
	

	
		siteLongitudeExists = false;
	

	
		siteLatitudeExists = false;
	

	
		observingScriptExists = false;
	

	
		observingScriptUIDExists = false;
	

	
	

	

	
		scaleIdExists = false;
	
		
		}
		else {
	
		
			execBlockId = row->execBlockId;
		
		
		
		
			startTime = row->startTime;
		
			endTime = row->endTime;
		
			execBlockNum = row->execBlockNum;
		
			execBlockUID = row->execBlockUID;
		
			projectUID = row->projectUID;
		
			configName = row->configName;
		
			telescopeName = row->telescopeName;
		
			observerName = row->observerName;
		
			numObservingLog = row->numObservingLog;
		
			observingLog = row->observingLog;
		
			sessionReference = row->sessionReference;
		
			baseRangeMin = row->baseRangeMin;
		
			baseRangeMax = row->baseRangeMax;
		
			baseRmsMinor = row->baseRmsMinor;
		
			baseRmsMajor = row->baseRmsMajor;
		
			basePa = row->basePa;
		
			aborted = row->aborted;
		
			numAntenna = row->numAntenna;
		
			antennaId = row->antennaId;
		
			sBSummaryId = row->sBSummaryId;
		
		
		
		
		if (row->releaseDateExists) {
			releaseDate = row->releaseDate;		
			releaseDateExists = true;
		}
		else
			releaseDateExists = false;
		
		if (row->schedulerModeExists) {
			schedulerMode = row->schedulerMode;		
			schedulerModeExists = true;
		}
		else
			schedulerModeExists = false;
		
		if (row->siteAltitudeExists) {
			siteAltitude = row->siteAltitude;		
			siteAltitudeExists = true;
		}
		else
			siteAltitudeExists = false;
		
		if (row->siteLongitudeExists) {
			siteLongitude = row->siteLongitude;		
			siteLongitudeExists = true;
		}
		else
			siteLongitudeExists = false;
		
		if (row->siteLatitudeExists) {
			siteLatitude = row->siteLatitude;		
			siteLatitudeExists = true;
		}
		else
			siteLatitudeExists = false;
		
		if (row->observingScriptExists) {
			observingScript = row->observingScript;		
			observingScriptExists = true;
		}
		else
			observingScriptExists = false;
		
		if (row->observingScriptUIDExists) {
			observingScriptUID = row->observingScriptUID;		
			observingScriptUIDExists = true;
		}
		else
			observingScriptUIDExists = false;
		
		if (row->scaleIdExists) {
			scaleId = row->scaleId;		
			scaleIdExists = true;
		}
		else
			scaleIdExists = false;
		
		}
		
		 fromBinMethods["execBlockId"] = &ExecBlockRow::execBlockIdFromBin; 
		 fromBinMethods["startTime"] = &ExecBlockRow::startTimeFromBin; 
		 fromBinMethods["endTime"] = &ExecBlockRow::endTimeFromBin; 
		 fromBinMethods["execBlockNum"] = &ExecBlockRow::execBlockNumFromBin; 
		 fromBinMethods["execBlockUID"] = &ExecBlockRow::execBlockUIDFromBin; 
		 fromBinMethods["projectUID"] = &ExecBlockRow::projectUIDFromBin; 
		 fromBinMethods["configName"] = &ExecBlockRow::configNameFromBin; 
		 fromBinMethods["telescopeName"] = &ExecBlockRow::telescopeNameFromBin; 
		 fromBinMethods["observerName"] = &ExecBlockRow::observerNameFromBin; 
		 fromBinMethods["numObservingLog"] = &ExecBlockRow::numObservingLogFromBin; 
		 fromBinMethods["observingLog"] = &ExecBlockRow::observingLogFromBin; 
		 fromBinMethods["sessionReference"] = &ExecBlockRow::sessionReferenceFromBin; 
		 fromBinMethods["baseRangeMin"] = &ExecBlockRow::baseRangeMinFromBin; 
		 fromBinMethods["baseRangeMax"] = &ExecBlockRow::baseRangeMaxFromBin; 
		 fromBinMethods["baseRmsMinor"] = &ExecBlockRow::baseRmsMinorFromBin; 
		 fromBinMethods["baseRmsMajor"] = &ExecBlockRow::baseRmsMajorFromBin; 
		 fromBinMethods["basePa"] = &ExecBlockRow::basePaFromBin; 
		 fromBinMethods["aborted"] = &ExecBlockRow::abortedFromBin; 
		 fromBinMethods["numAntenna"] = &ExecBlockRow::numAntennaFromBin; 
		 fromBinMethods["antennaId"] = &ExecBlockRow::antennaIdFromBin; 
		 fromBinMethods["sBSummaryId"] = &ExecBlockRow::sBSummaryIdFromBin; 
			
	
		 fromBinMethods["releaseDate"] = &ExecBlockRow::releaseDateFromBin; 
		 fromBinMethods["schedulerMode"] = &ExecBlockRow::schedulerModeFromBin; 
		 fromBinMethods["siteAltitude"] = &ExecBlockRow::siteAltitudeFromBin; 
		 fromBinMethods["siteLongitude"] = &ExecBlockRow::siteLongitudeFromBin; 
		 fromBinMethods["siteLatitude"] = &ExecBlockRow::siteLatitudeFromBin; 
		 fromBinMethods["observingScript"] = &ExecBlockRow::observingScriptFromBin; 
		 fromBinMethods["observingScriptUID"] = &ExecBlockRow::observingScriptUIDFromBin; 
		 fromBinMethods["scaleId"] = &ExecBlockRow::scaleIdFromBin; 
			
	}

	
	bool ExecBlockRow::compareNoAutoInc(ArrayTime startTime, ArrayTime endTime, int execBlockNum, EntityRef execBlockUID, EntityRef projectUID, std::string configName, std::string telescopeName, std::string observerName, int numObservingLog, std::vector<std::string > observingLog, EntityRef sessionReference, Length baseRangeMin, Length baseRangeMax, Length baseRmsMinor, Length baseRmsMajor, Angle basePa, bool aborted, int numAntenna, std::vector<Tag>  antennaId, Tag sBSummaryId) {
		bool result;
		result = true;
		
	
		
		result = result && (this->startTime == startTime);
		
		if (!result) return false;
	

	
		
		result = result && (this->endTime == endTime);
		
		if (!result) return false;
	

	
		
		result = result && (this->execBlockNum == execBlockNum);
		
		if (!result) return false;
	

	
		
		result = result && (this->execBlockUID == execBlockUID);
		
		if (!result) return false;
	

	
		
		result = result && (this->projectUID == projectUID);
		
		if (!result) return false;
	

	
		
		result = result && (this->configName == configName);
		
		if (!result) return false;
	

	
		
		result = result && (this->telescopeName == telescopeName);
		
		if (!result) return false;
	

	
		
		result = result && (this->observerName == observerName);
		
		if (!result) return false;
	

	
		
		result = result && (this->numObservingLog == numObservingLog);
		
		if (!result) return false;
	

	
		
		result = result && (this->observingLog == observingLog);
		
		if (!result) return false;
	

	
		
		result = result && (this->sessionReference == sessionReference);
		
		if (!result) return false;
	

	
		
		result = result && (this->baseRangeMin == baseRangeMin);
		
		if (!result) return false;
	

	
		
		result = result && (this->baseRangeMax == baseRangeMax);
		
		if (!result) return false;
	

	
		
		result = result && (this->baseRmsMinor == baseRmsMinor);
		
		if (!result) return false;
	

	
		
		result = result && (this->baseRmsMajor == baseRmsMajor);
		
		if (!result) return false;
	

	
		
		result = result && (this->basePa == basePa);
		
		if (!result) return false;
	

	
		
		result = result && (this->aborted == aborted);
		
		if (!result) return false;
	

	
		
		result = result && (this->numAntenna == numAntenna);
		
		if (!result) return false;
	

	
		
		result = result && (this->antennaId == antennaId);
		
		if (!result) return false;
	

	
		
		result = result && (this->sBSummaryId == sBSummaryId);
		
		if (!result) return false;
	

		return result;
	}	
	
	
	
	bool ExecBlockRow::compareRequiredValue(ArrayTime startTime, ArrayTime endTime, int execBlockNum, EntityRef execBlockUID, EntityRef projectUID, std::string configName, std::string telescopeName, std::string observerName, int numObservingLog, std::vector<std::string > observingLog, EntityRef sessionReference, Length baseRangeMin, Length baseRangeMax, Length baseRmsMinor, Length baseRmsMajor, Angle basePa, bool aborted, int numAntenna, std::vector<Tag>  antennaId, Tag sBSummaryId) {
		bool result;
		result = true;
		
	
		if (!(this->startTime == startTime)) return false;
	

	
		if (!(this->endTime == endTime)) return false;
	

	
		if (!(this->execBlockNum == execBlockNum)) return false;
	

	
		if (!(this->execBlockUID == execBlockUID)) return false;
	

	
		if (!(this->projectUID == projectUID)) return false;
	

	
		if (!(this->configName == configName)) return false;
	

	
		if (!(this->telescopeName == telescopeName)) return false;
	

	
		if (!(this->observerName == observerName)) return false;
	

	
		if (!(this->numObservingLog == numObservingLog)) return false;
	

	
		if (!(this->observingLog == observingLog)) return false;
	

	
		if (!(this->sessionReference == sessionReference)) return false;
	

	
		if (!(this->baseRangeMin == baseRangeMin)) return false;
	

	
		if (!(this->baseRangeMax == baseRangeMax)) return false;
	

	
		if (!(this->baseRmsMinor == baseRmsMinor)) return false;
	

	
		if (!(this->baseRmsMajor == baseRmsMajor)) return false;
	

	
		if (!(this->basePa == basePa)) return false;
	

	
		if (!(this->aborted == aborted)) return false;
	

	
		if (!(this->numAntenna == numAntenna)) return false;
	

	
		if (!(this->antennaId == antennaId)) return false;
	

	
		if (!(this->sBSummaryId == sBSummaryId)) return false;
	

		return result;
	}
	
	
	/**
	 * Return true if all required attributes of the value part are equal to their homologues
	 * in x and false otherwise.
	 *

	 * @param x a pointer on the ExecBlockRow whose required attributes of the value part 

	 * will be compared with those of this.
	 * @return a boolean.
	 */
	bool ExecBlockRow::equalByRequiredValue(ExecBlockRow*  x ) {
		
			
		if (this->startTime != x->startTime) return false;
			
		if (this->endTime != x->endTime) return false;
			
		if (this->execBlockNum != x->execBlockNum) return false;
			
		if (this->execBlockUID != x->execBlockUID) return false;
			
		if (this->projectUID != x->projectUID) return false;
			
		if (this->configName != x->configName) return false;
			
		if (this->telescopeName != x->telescopeName) return false;
			
		if (this->observerName != x->observerName) return false;
			
		if (this->numObservingLog != x->numObservingLog) return false;
			
		if (this->observingLog != x->observingLog) return false;
			
		if (this->sessionReference != x->sessionReference) return false;
			
		if (this->baseRangeMin != x->baseRangeMin) return false;
			
		if (this->baseRangeMax != x->baseRangeMax) return false;
			
		if (this->baseRmsMinor != x->baseRmsMinor) return false;
			
		if (this->baseRmsMajor != x->baseRmsMajor) return false;
			
		if (this->basePa != x->basePa) return false;
			
		if (this->aborted != x->aborted) return false;
			
		if (this->numAntenna != x->numAntenna) return false;
			
		if (this->antennaId != x->antennaId) return false;
			
		if (this->sBSummaryId != x->sBSummaryId) return false;
			
		
		return true;
	}	
	
/*
	 map<string, ExecBlockAttributeFromBin> ExecBlockRow::initFromBinMethods() {
		map<string, ExecBlockAttributeFromBin> result;
		
		result["execBlockId"] = &ExecBlockRow::execBlockIdFromBin;
		result["startTime"] = &ExecBlockRow::startTimeFromBin;
		result["endTime"] = &ExecBlockRow::endTimeFromBin;
		result["execBlockNum"] = &ExecBlockRow::execBlockNumFromBin;
		result["execBlockUID"] = &ExecBlockRow::execBlockUIDFromBin;
		result["projectUID"] = &ExecBlockRow::projectUIDFromBin;
		result["configName"] = &ExecBlockRow::configNameFromBin;
		result["telescopeName"] = &ExecBlockRow::telescopeNameFromBin;
		result["observerName"] = &ExecBlockRow::observerNameFromBin;
		result["numObservingLog"] = &ExecBlockRow::numObservingLogFromBin;
		result["observingLog"] = &ExecBlockRow::observingLogFromBin;
		result["sessionReference"] = &ExecBlockRow::sessionReferenceFromBin;
		result["baseRangeMin"] = &ExecBlockRow::baseRangeMinFromBin;
		result["baseRangeMax"] = &ExecBlockRow::baseRangeMaxFromBin;
		result["baseRmsMinor"] = &ExecBlockRow::baseRmsMinorFromBin;
		result["baseRmsMajor"] = &ExecBlockRow::baseRmsMajorFromBin;
		result["basePa"] = &ExecBlockRow::basePaFromBin;
		result["aborted"] = &ExecBlockRow::abortedFromBin;
		result["numAntenna"] = &ExecBlockRow::numAntennaFromBin;
		result["antennaId"] = &ExecBlockRow::antennaIdFromBin;
		result["sBSummaryId"] = &ExecBlockRow::sBSummaryIdFromBin;
		
		
		result["releaseDate"] = &ExecBlockRow::releaseDateFromBin;
		result["schedulerMode"] = &ExecBlockRow::schedulerModeFromBin;
		result["siteAltitude"] = &ExecBlockRow::siteAltitudeFromBin;
		result["siteLongitude"] = &ExecBlockRow::siteLongitudeFromBin;
		result["siteLatitude"] = &ExecBlockRow::siteLatitudeFromBin;
		result["observingScript"] = &ExecBlockRow::observingScriptFromBin;
		result["observingScriptUID"] = &ExecBlockRow::observingScriptUIDFromBin;
		result["scaleId"] = &ExecBlockRow::scaleIdFromBin;
			
		
		return result;	
	}
*/	
} // End namespace asdm
 
