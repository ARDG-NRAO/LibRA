
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
 * File DelayModelFixedParametersRow.cpp
 */
 
#include <vector>
#include <set>

#include <alma/ASDM/ASDM.h>
#include <alma/ASDM/DelayModelFixedParametersRow.h>
#include <alma/ASDM/DelayModelFixedParametersTable.h>

#include <alma/ASDM/ExecBlockTable.h>
#include <alma/ASDM/ExecBlockRow.h>
	

using asdm::ASDM;
using asdm::DelayModelFixedParametersRow;
using asdm::DelayModelFixedParametersTable;

using asdm::ExecBlockTable;
using asdm::ExecBlockRow;


#include <alma/ASDM/Parser.h>

#include <alma/ASDM/EnumerationParser.h>
#include <alma/ASDM/ASDMValuesParser.h>
 
#include <alma/ASDM/InvalidArgumentException.h>

using namespace std;

namespace asdm {
	DelayModelFixedParametersRow::~DelayModelFixedParametersRow() {
	}

	/**
	 * Return the table to which this row belongs.
	 */
	DelayModelFixedParametersTable &DelayModelFixedParametersRow::getTable() const {
		return table;
	}

	bool DelayModelFixedParametersRow::isAdded() const {
		return hasBeenAdded;
	}	

	void DelayModelFixedParametersRow::isAdded(bool added) {
		hasBeenAdded = added;
	}
	
#ifndef WITHOUT_ACS
	using asdmIDL::DelayModelFixedParametersRowIDL;
#endif
	
#ifndef WITHOUT_ACS
	/**
	 * Return this row in the form of an IDL struct.
	 * @return The values of this row as a DelayModelFixedParametersRowIDL struct.
	 */
	DelayModelFixedParametersRowIDL *DelayModelFixedParametersRow::toIDL() const {
		DelayModelFixedParametersRowIDL *x = new DelayModelFixedParametersRowIDL ();
		
		// Fill the IDL structure.
	
		
	
  		
		
		
			
		x->delayModelFixedParametersId = delayModelFixedParametersId.toIDLTag();
			
		
	

	
  		
		
		
			
				
		x->delayModelVersion = CORBA::string_dup(delayModelVersion.c_str());
				
 			
		
	

	
  		
		
		x->gaussConstantExists = gaussConstantExists;
		
		
			
		x->gaussConstant = gaussConstant.toIDLAngularRate();
			
		
	

	
  		
		
		x->newtonianConstantExists = newtonianConstantExists;
		
		
			
				
		x->newtonianConstant = newtonianConstant;
 				
 			
		
	

	
  		
		
		x->gravityExists = gravityExists;
		
		
			
				
		x->gravity = gravity;
 				
 			
		
	

	
  		
		
		x->earthFlatteningExists = earthFlatteningExists;
		
		
			
				
		x->earthFlattening = earthFlattening;
 				
 			
		
	

	
  		
		
		x->earthRadiusExists = earthRadiusExists;
		
		
			
		x->earthRadius = earthRadius.toIDLLength();
			
		
	

	
  		
		
		x->moonEarthMassRatioExists = moonEarthMassRatioExists;
		
		
			
				
		x->moonEarthMassRatio = moonEarthMassRatio;
 				
 			
		
	

	
  		
		
		x->ephemerisEpochExists = ephemerisEpochExists;
		
		
			
				
		x->ephemerisEpoch = CORBA::string_dup(ephemerisEpoch.c_str());
				
 			
		
	

	
  		
		
		x->earthTideLagExists = earthTideLagExists;
		
		
			
				
		x->earthTideLag = earthTideLag;
 				
 			
		
	

	
  		
		
		x->earthGMExists = earthGMExists;
		
		
			
				
		x->earthGM = earthGM;
 				
 			
		
	

	
  		
		
		x->moonGMExists = moonGMExists;
		
		
			
				
		x->moonGM = moonGM;
 				
 			
		
	

	
  		
		
		x->sunGMExists = sunGMExists;
		
		
			
				
		x->sunGM = sunGM;
 				
 			
		
	

	
  		
		
		x->loveNumberHExists = loveNumberHExists;
		
		
			
				
		x->loveNumberH = loveNumberH;
 				
 			
		
	

	
  		
		
		x->loveNumberLExists = loveNumberLExists;
		
		
			
				
		x->loveNumberL = loveNumberL;
 				
 			
		
	

	
  		
		
		x->precessionConstantExists = precessionConstantExists;
		
		
			
		x->precessionConstant = precessionConstant.toIDLAngularRate();
			
		
	

	
  		
		
		x->lightTime1AUExists = lightTime1AUExists;
		
		
			
				
		x->lightTime1AU = lightTime1AU;
 				
 			
		
	

	
  		
		
		x->speedOfLightExists = speedOfLightExists;
		
		
			
		x->speedOfLight = speedOfLight.toIDLSpeed();
			
		
	

	
  		
		
		x->delayModelFlagsExists = delayModelFlagsExists;
		
		
			
				
		x->delayModelFlags = CORBA::string_dup(delayModelFlags.c_str());
				
 			
		
	

	
	
		
	
  	
 		
		
	 	
			
		x->execBlockId = execBlockId.toIDLTag();
			
	 	 		
  	

	
		
	

		
		return x;
	
	}
	
	void DelayModelFixedParametersRow::toIDL(asdmIDL::DelayModelFixedParametersRowIDL& x) const {
		// Set the x's fields.
	
		
	
  		
		
		
			
		x.delayModelFixedParametersId = delayModelFixedParametersId.toIDLTag();
			
		
	

	
  		
		
		
			
				
		x.delayModelVersion = CORBA::string_dup(delayModelVersion.c_str());
				
 			
		
	

	
  		
		
		x.gaussConstantExists = gaussConstantExists;
		
		
			
		x.gaussConstant = gaussConstant.toIDLAngularRate();
			
		
	

	
  		
		
		x.newtonianConstantExists = newtonianConstantExists;
		
		
			
				
		x.newtonianConstant = newtonianConstant;
 				
 			
		
	

	
  		
		
		x.gravityExists = gravityExists;
		
		
			
				
		x.gravity = gravity;
 				
 			
		
	

	
  		
		
		x.earthFlatteningExists = earthFlatteningExists;
		
		
			
				
		x.earthFlattening = earthFlattening;
 				
 			
		
	

	
  		
		
		x.earthRadiusExists = earthRadiusExists;
		
		
			
		x.earthRadius = earthRadius.toIDLLength();
			
		
	

	
  		
		
		x.moonEarthMassRatioExists = moonEarthMassRatioExists;
		
		
			
				
		x.moonEarthMassRatio = moonEarthMassRatio;
 				
 			
		
	

	
  		
		
		x.ephemerisEpochExists = ephemerisEpochExists;
		
		
			
				
		x.ephemerisEpoch = CORBA::string_dup(ephemerisEpoch.c_str());
				
 			
		
	

	
  		
		
		x.earthTideLagExists = earthTideLagExists;
		
		
			
				
		x.earthTideLag = earthTideLag;
 				
 			
		
	

	
  		
		
		x.earthGMExists = earthGMExists;
		
		
			
				
		x.earthGM = earthGM;
 				
 			
		
	

	
  		
		
		x.moonGMExists = moonGMExists;
		
		
			
				
		x.moonGM = moonGM;
 				
 			
		
	

	
  		
		
		x.sunGMExists = sunGMExists;
		
		
			
				
		x.sunGM = sunGM;
 				
 			
		
	

	
  		
		
		x.loveNumberHExists = loveNumberHExists;
		
		
			
				
		x.loveNumberH = loveNumberH;
 				
 			
		
	

	
  		
		
		x.loveNumberLExists = loveNumberLExists;
		
		
			
				
		x.loveNumberL = loveNumberL;
 				
 			
		
	

	
  		
		
		x.precessionConstantExists = precessionConstantExists;
		
		
			
		x.precessionConstant = precessionConstant.toIDLAngularRate();
			
		
	

	
  		
		
		x.lightTime1AUExists = lightTime1AUExists;
		
		
			
				
		x.lightTime1AU = lightTime1AU;
 				
 			
		
	

	
  		
		
		x.speedOfLightExists = speedOfLightExists;
		
		
			
		x.speedOfLight = speedOfLight.toIDLSpeed();
			
		
	

	
  		
		
		x.delayModelFlagsExists = delayModelFlagsExists;
		
		
			
				
		x.delayModelFlags = CORBA::string_dup(delayModelFlags.c_str());
				
 			
		
	

	
	
		
	
  	
 		
		
	 	
			
		x.execBlockId = execBlockId.toIDLTag();
			
	 	 		
  	

	
		
	

	
	}
#endif
	

#ifndef WITHOUT_ACS
	/**
	 * Fill the values of this row from the IDL struct DelayModelFixedParametersRowIDL.
	 * @param x The IDL struct containing the values used to fill this row.
	 */
	void DelayModelFixedParametersRow::setFromIDL (DelayModelFixedParametersRowIDL x){
		try {
		// Fill the values from x.
	
		
	
		
		
			
		setDelayModelFixedParametersId(Tag (x.delayModelFixedParametersId));
			
 		
		
	

	
		
		
			
		setDelayModelVersion(string (x.delayModelVersion));
			
 		
		
	

	
		
		gaussConstantExists = x.gaussConstantExists;
		if (x.gaussConstantExists) {
		
		
			
		setGaussConstant(AngularRate (x.gaussConstant));
			
 		
		
		}
		
	

	
		
		newtonianConstantExists = x.newtonianConstantExists;
		if (x.newtonianConstantExists) {
		
		
			
		setNewtonianConstant(x.newtonianConstant);
  			
 		
		
		}
		
	

	
		
		gravityExists = x.gravityExists;
		if (x.gravityExists) {
		
		
			
		setGravity(x.gravity);
  			
 		
		
		}
		
	

	
		
		earthFlatteningExists = x.earthFlatteningExists;
		if (x.earthFlatteningExists) {
		
		
			
		setEarthFlattening(x.earthFlattening);
  			
 		
		
		}
		
	

	
		
		earthRadiusExists = x.earthRadiusExists;
		if (x.earthRadiusExists) {
		
		
			
		setEarthRadius(Length (x.earthRadius));
			
 		
		
		}
		
	

	
		
		moonEarthMassRatioExists = x.moonEarthMassRatioExists;
		if (x.moonEarthMassRatioExists) {
		
		
			
		setMoonEarthMassRatio(x.moonEarthMassRatio);
  			
 		
		
		}
		
	

	
		
		ephemerisEpochExists = x.ephemerisEpochExists;
		if (x.ephemerisEpochExists) {
		
		
			
		setEphemerisEpoch(string (x.ephemerisEpoch));
			
 		
		
		}
		
	

	
		
		earthTideLagExists = x.earthTideLagExists;
		if (x.earthTideLagExists) {
		
		
			
		setEarthTideLag(x.earthTideLag);
  			
 		
		
		}
		
	

	
		
		earthGMExists = x.earthGMExists;
		if (x.earthGMExists) {
		
		
			
		setEarthGM(x.earthGM);
  			
 		
		
		}
		
	

	
		
		moonGMExists = x.moonGMExists;
		if (x.moonGMExists) {
		
		
			
		setMoonGM(x.moonGM);
  			
 		
		
		}
		
	

	
		
		sunGMExists = x.sunGMExists;
		if (x.sunGMExists) {
		
		
			
		setSunGM(x.sunGM);
  			
 		
		
		}
		
	

	
		
		loveNumberHExists = x.loveNumberHExists;
		if (x.loveNumberHExists) {
		
		
			
		setLoveNumberH(x.loveNumberH);
  			
 		
		
		}
		
	

	
		
		loveNumberLExists = x.loveNumberLExists;
		if (x.loveNumberLExists) {
		
		
			
		setLoveNumberL(x.loveNumberL);
  			
 		
		
		}
		
	

	
		
		precessionConstantExists = x.precessionConstantExists;
		if (x.precessionConstantExists) {
		
		
			
		setPrecessionConstant(AngularRate (x.precessionConstant));
			
 		
		
		}
		
	

	
		
		lightTime1AUExists = x.lightTime1AUExists;
		if (x.lightTime1AUExists) {
		
		
			
		setLightTime1AU(x.lightTime1AU);
  			
 		
		
		}
		
	

	
		
		speedOfLightExists = x.speedOfLightExists;
		if (x.speedOfLightExists) {
		
		
			
		setSpeedOfLight(Speed (x.speedOfLight));
			
 		
		
		}
		
	

	
		
		delayModelFlagsExists = x.delayModelFlagsExists;
		if (x.delayModelFlagsExists) {
		
		
			
		setDelayModelFlags(string (x.delayModelFlags));
			
 		
		
		}
		
	

	
	
		
	
		
		
			
		setExecBlockId(Tag (x.execBlockId));
			
 		
		
	

	
		
	

		} catch (const IllegalAccessException &err) {
			throw ConversionException (err.getMessage(),"DelayModelFixedParameters");
		}
	}
#endif
	
	/**
	 * Return this row in the form of an XML string.
	 * @return The values of this row as an XML string.
	 */
	string DelayModelFixedParametersRow::toXML() const {
		string buf;
		buf.append("<row> \n");
		
	
		
  	
 		
		
		Parser::toXML(delayModelFixedParametersId, "delayModelFixedParametersId", buf);
		
		
	

  	
 		
		
		Parser::toXML(delayModelVersion, "delayModelVersion", buf);
		
		
	

  	
 		
		if (gaussConstantExists) {
		
		
		Parser::toXML(gaussConstant, "gaussConstant", buf);
		
		
		}
		
	

  	
 		
		if (newtonianConstantExists) {
		
		
		Parser::toXML(newtonianConstant, "newtonianConstant", buf);
		
		
		}
		
	

  	
 		
		if (gravityExists) {
		
		
		Parser::toXML(gravity, "gravity", buf);
		
		
		}
		
	

  	
 		
		if (earthFlatteningExists) {
		
		
		Parser::toXML(earthFlattening, "earthFlattening", buf);
		
		
		}
		
	

  	
 		
		if (earthRadiusExists) {
		
		
		Parser::toXML(earthRadius, "earthRadius", buf);
		
		
		}
		
	

  	
 		
		if (moonEarthMassRatioExists) {
		
		
		Parser::toXML(moonEarthMassRatio, "moonEarthMassRatio", buf);
		
		
		}
		
	

  	
 		
		if (ephemerisEpochExists) {
		
		
		Parser::toXML(ephemerisEpoch, "ephemerisEpoch", buf);
		
		
		}
		
	

  	
 		
		if (earthTideLagExists) {
		
		
		Parser::toXML(earthTideLag, "earthTideLag", buf);
		
		
		}
		
	

  	
 		
		if (earthGMExists) {
		
		
		Parser::toXML(earthGM, "earthGM", buf);
		
		
		}
		
	

  	
 		
		if (moonGMExists) {
		
		
		Parser::toXML(moonGM, "moonGM", buf);
		
		
		}
		
	

  	
 		
		if (sunGMExists) {
		
		
		Parser::toXML(sunGM, "sunGM", buf);
		
		
		}
		
	

  	
 		
		if (loveNumberHExists) {
		
		
		Parser::toXML(loveNumberH, "loveNumberH", buf);
		
		
		}
		
	

  	
 		
		if (loveNumberLExists) {
		
		
		Parser::toXML(loveNumberL, "loveNumberL", buf);
		
		
		}
		
	

  	
 		
		if (precessionConstantExists) {
		
		
		Parser::toXML(precessionConstant, "precessionConstant", buf);
		
		
		}
		
	

  	
 		
		if (lightTime1AUExists) {
		
		
		Parser::toXML(lightTime1AU, "lightTime1AU", buf);
		
		
		}
		
	

  	
 		
		if (speedOfLightExists) {
		
		
		Parser::toXML(speedOfLight, "speedOfLight", buf);
		
		
		}
		
	

  	
 		
		if (delayModelFlagsExists) {
		
		
		Parser::toXML(delayModelFlags, "delayModelFlags", buf);
		
		
		}
		
	

	
	
		
  	
 		
		
		Parser::toXML(execBlockId, "execBlockId", buf);
		
		
	

	
		
	

		
		buf.append("</row>\n");
		return buf;
	}

	/**
	 * Fill the values of this row from an XML string 
	 * that was produced by the toXML() method.
	 * @param x The XML string being used to set the values of this row.
	 */
	void DelayModelFixedParametersRow::setFromXML (string rowDoc) {
		Parser row(rowDoc);
		string s = "";
		try {
	
		
	
  		
			
	  	setDelayModelFixedParametersId(Parser::getTag("delayModelFixedParametersId","DelayModelFixedParameters",rowDoc));
			
		
	

	
  		
			
	  	setDelayModelVersion(Parser::getString("delayModelVersion","DelayModelFixedParameters",rowDoc));
			
		
	

	
  		
        if (row.isStr("<gaussConstant>")) {
			
	  		setGaussConstant(Parser::getAngularRate("gaussConstant","DelayModelFixedParameters",rowDoc));
			
		}
 		
	

	
  		
        if (row.isStr("<newtonianConstant>")) {
			
	  		setNewtonianConstant(Parser::getDouble("newtonianConstant","DelayModelFixedParameters",rowDoc));
			
		}
 		
	

	
  		
        if (row.isStr("<gravity>")) {
			
	  		setGravity(Parser::getDouble("gravity","DelayModelFixedParameters",rowDoc));
			
		}
 		
	

	
  		
        if (row.isStr("<earthFlattening>")) {
			
	  		setEarthFlattening(Parser::getDouble("earthFlattening","DelayModelFixedParameters",rowDoc));
			
		}
 		
	

	
  		
        if (row.isStr("<earthRadius>")) {
			
	  		setEarthRadius(Parser::getLength("earthRadius","DelayModelFixedParameters",rowDoc));
			
		}
 		
	

	
  		
        if (row.isStr("<moonEarthMassRatio>")) {
			
	  		setMoonEarthMassRatio(Parser::getDouble("moonEarthMassRatio","DelayModelFixedParameters",rowDoc));
			
		}
 		
	

	
  		
        if (row.isStr("<ephemerisEpoch>")) {
			
	  		setEphemerisEpoch(Parser::getString("ephemerisEpoch","DelayModelFixedParameters",rowDoc));
			
		}
 		
	

	
  		
        if (row.isStr("<earthTideLag>")) {
			
	  		setEarthTideLag(Parser::getDouble("earthTideLag","DelayModelFixedParameters",rowDoc));
			
		}
 		
	

	
  		
        if (row.isStr("<earthGM>")) {
			
	  		setEarthGM(Parser::getDouble("earthGM","DelayModelFixedParameters",rowDoc));
			
		}
 		
	

	
  		
        if (row.isStr("<moonGM>")) {
			
	  		setMoonGM(Parser::getDouble("moonGM","DelayModelFixedParameters",rowDoc));
			
		}
 		
	

	
  		
        if (row.isStr("<sunGM>")) {
			
	  		setSunGM(Parser::getDouble("sunGM","DelayModelFixedParameters",rowDoc));
			
		}
 		
	

	
  		
        if (row.isStr("<loveNumberH>")) {
			
	  		setLoveNumberH(Parser::getDouble("loveNumberH","DelayModelFixedParameters",rowDoc));
			
		}
 		
	

	
  		
        if (row.isStr("<loveNumberL>")) {
			
	  		setLoveNumberL(Parser::getDouble("loveNumberL","DelayModelFixedParameters",rowDoc));
			
		}
 		
	

	
  		
        if (row.isStr("<precessionConstant>")) {
			
	  		setPrecessionConstant(Parser::getAngularRate("precessionConstant","DelayModelFixedParameters",rowDoc));
			
		}
 		
	

	
  		
        if (row.isStr("<lightTime1AU>")) {
			
	  		setLightTime1AU(Parser::getDouble("lightTime1AU","DelayModelFixedParameters",rowDoc));
			
		}
 		
	

	
  		
        if (row.isStr("<speedOfLight>")) {
			
	  		setSpeedOfLight(Parser::getSpeed("speedOfLight","DelayModelFixedParameters",rowDoc));
			
		}
 		
	

	
  		
        if (row.isStr("<delayModelFlags>")) {
			
	  		setDelayModelFlags(Parser::getString("delayModelFlags","DelayModelFixedParameters",rowDoc));
			
		}
 		
	

	
	
		
	
  		
			
	  	setExecBlockId(Parser::getTag("execBlockId","DelayModelFixedParameters",rowDoc));
			
		
	

	
		
	

		} catch (const IllegalAccessException &err) {
			throw ConversionException (err.getMessage(),"DelayModelFixedParameters");
		}
	}
	
	void DelayModelFixedParametersRow::toBin(EndianOSStream& eoss) {
	
	
	
	
		
	delayModelFixedParametersId.toBin(eoss);
		
	

	
	
		
						
			eoss.writeString(delayModelVersion);
				
		
	

	
	
		
	execBlockId.toBin(eoss);
		
	


	
	
	eoss.writeBoolean(gaussConstantExists);
	if (gaussConstantExists) {
	
	
	
		
	gaussConstant.toBin(eoss);
		
	

	}

	eoss.writeBoolean(newtonianConstantExists);
	if (newtonianConstantExists) {
	
	
	
		
						
			eoss.writeDouble(newtonianConstant);
				
		
	

	}

	eoss.writeBoolean(gravityExists);
	if (gravityExists) {
	
	
	
		
						
			eoss.writeDouble(gravity);
				
		
	

	}

	eoss.writeBoolean(earthFlatteningExists);
	if (earthFlatteningExists) {
	
	
	
		
						
			eoss.writeDouble(earthFlattening);
				
		
	

	}

	eoss.writeBoolean(earthRadiusExists);
	if (earthRadiusExists) {
	
	
	
		
	earthRadius.toBin(eoss);
		
	

	}

	eoss.writeBoolean(moonEarthMassRatioExists);
	if (moonEarthMassRatioExists) {
	
	
	
		
						
			eoss.writeDouble(moonEarthMassRatio);
				
		
	

	}

	eoss.writeBoolean(ephemerisEpochExists);
	if (ephemerisEpochExists) {
	
	
	
		
						
			eoss.writeString(ephemerisEpoch);
				
		
	

	}

	eoss.writeBoolean(earthTideLagExists);
	if (earthTideLagExists) {
	
	
	
		
						
			eoss.writeDouble(earthTideLag);
				
		
	

	}

	eoss.writeBoolean(earthGMExists);
	if (earthGMExists) {
	
	
	
		
						
			eoss.writeDouble(earthGM);
				
		
	

	}

	eoss.writeBoolean(moonGMExists);
	if (moonGMExists) {
	
	
	
		
						
			eoss.writeDouble(moonGM);
				
		
	

	}

	eoss.writeBoolean(sunGMExists);
	if (sunGMExists) {
	
	
	
		
						
			eoss.writeDouble(sunGM);
				
		
	

	}

	eoss.writeBoolean(loveNumberHExists);
	if (loveNumberHExists) {
	
	
	
		
						
			eoss.writeDouble(loveNumberH);
				
		
	

	}

	eoss.writeBoolean(loveNumberLExists);
	if (loveNumberLExists) {
	
	
	
		
						
			eoss.writeDouble(loveNumberL);
				
		
	

	}

	eoss.writeBoolean(precessionConstantExists);
	if (precessionConstantExists) {
	
	
	
		
	precessionConstant.toBin(eoss);
		
	

	}

	eoss.writeBoolean(lightTime1AUExists);
	if (lightTime1AUExists) {
	
	
	
		
						
			eoss.writeDouble(lightTime1AU);
				
		
	

	}

	eoss.writeBoolean(speedOfLightExists);
	if (speedOfLightExists) {
	
	
	
		
	speedOfLight.toBin(eoss);
		
	

	}

	eoss.writeBoolean(delayModelFlagsExists);
	if (delayModelFlagsExists) {
	
	
	
		
						
			eoss.writeString(delayModelFlags);
				
		
	

	}

	}
	
void DelayModelFixedParametersRow::delayModelFixedParametersIdFromBin(EndianIStream& eis) {
		
	
		
		
		delayModelFixedParametersId =  Tag::fromBin(eis);
		
	
	
}
void DelayModelFixedParametersRow::delayModelVersionFromBin(EndianIStream& eis) {
		
	
	
		
			
		delayModelVersion =  eis.readString();
			
		
	
	
}
void DelayModelFixedParametersRow::execBlockIdFromBin(EndianIStream& eis) {
		
	
		
		
		execBlockId =  Tag::fromBin(eis);
		
	
	
}

void DelayModelFixedParametersRow::gaussConstantFromBin(EndianIStream& eis) {
		
	gaussConstantExists = eis.readBoolean();
	if (gaussConstantExists) {
		
	
		
		
		gaussConstant =  AngularRate::fromBin(eis);
		
	

	}
	
}
void DelayModelFixedParametersRow::newtonianConstantFromBin(EndianIStream& eis) {
		
	newtonianConstantExists = eis.readBoolean();
	if (newtonianConstantExists) {
		
	
	
		
			
		newtonianConstant =  eis.readDouble();
			
		
	

	}
	
}
void DelayModelFixedParametersRow::gravityFromBin(EndianIStream& eis) {
		
	gravityExists = eis.readBoolean();
	if (gravityExists) {
		
	
	
		
			
		gravity =  eis.readDouble();
			
		
	

	}
	
}
void DelayModelFixedParametersRow::earthFlatteningFromBin(EndianIStream& eis) {
		
	earthFlatteningExists = eis.readBoolean();
	if (earthFlatteningExists) {
		
	
	
		
			
		earthFlattening =  eis.readDouble();
			
		
	

	}
	
}
void DelayModelFixedParametersRow::earthRadiusFromBin(EndianIStream& eis) {
		
	earthRadiusExists = eis.readBoolean();
	if (earthRadiusExists) {
		
	
		
		
		earthRadius =  Length::fromBin(eis);
		
	

	}
	
}
void DelayModelFixedParametersRow::moonEarthMassRatioFromBin(EndianIStream& eis) {
		
	moonEarthMassRatioExists = eis.readBoolean();
	if (moonEarthMassRatioExists) {
		
	
	
		
			
		moonEarthMassRatio =  eis.readDouble();
			
		
	

	}
	
}
void DelayModelFixedParametersRow::ephemerisEpochFromBin(EndianIStream& eis) {
		
	ephemerisEpochExists = eis.readBoolean();
	if (ephemerisEpochExists) {
		
	
	
		
			
		ephemerisEpoch =  eis.readString();
			
		
	

	}
	
}
void DelayModelFixedParametersRow::earthTideLagFromBin(EndianIStream& eis) {
		
	earthTideLagExists = eis.readBoolean();
	if (earthTideLagExists) {
		
	
	
		
			
		earthTideLag =  eis.readDouble();
			
		
	

	}
	
}
void DelayModelFixedParametersRow::earthGMFromBin(EndianIStream& eis) {
		
	earthGMExists = eis.readBoolean();
	if (earthGMExists) {
		
	
	
		
			
		earthGM =  eis.readDouble();
			
		
	

	}
	
}
void DelayModelFixedParametersRow::moonGMFromBin(EndianIStream& eis) {
		
	moonGMExists = eis.readBoolean();
	if (moonGMExists) {
		
	
	
		
			
		moonGM =  eis.readDouble();
			
		
	

	}
	
}
void DelayModelFixedParametersRow::sunGMFromBin(EndianIStream& eis) {
		
	sunGMExists = eis.readBoolean();
	if (sunGMExists) {
		
	
	
		
			
		sunGM =  eis.readDouble();
			
		
	

	}
	
}
void DelayModelFixedParametersRow::loveNumberHFromBin(EndianIStream& eis) {
		
	loveNumberHExists = eis.readBoolean();
	if (loveNumberHExists) {
		
	
	
		
			
		loveNumberH =  eis.readDouble();
			
		
	

	}
	
}
void DelayModelFixedParametersRow::loveNumberLFromBin(EndianIStream& eis) {
		
	loveNumberLExists = eis.readBoolean();
	if (loveNumberLExists) {
		
	
	
		
			
		loveNumberL =  eis.readDouble();
			
		
	

	}
	
}
void DelayModelFixedParametersRow::precessionConstantFromBin(EndianIStream& eis) {
		
	precessionConstantExists = eis.readBoolean();
	if (precessionConstantExists) {
		
	
		
		
		precessionConstant =  AngularRate::fromBin(eis);
		
	

	}
	
}
void DelayModelFixedParametersRow::lightTime1AUFromBin(EndianIStream& eis) {
		
	lightTime1AUExists = eis.readBoolean();
	if (lightTime1AUExists) {
		
	
	
		
			
		lightTime1AU =  eis.readDouble();
			
		
	

	}
	
}
void DelayModelFixedParametersRow::speedOfLightFromBin(EndianIStream& eis) {
		
	speedOfLightExists = eis.readBoolean();
	if (speedOfLightExists) {
		
	
		
		
		speedOfLight =  Speed::fromBin(eis);
		
	

	}
	
}
void DelayModelFixedParametersRow::delayModelFlagsFromBin(EndianIStream& eis) {
		
	delayModelFlagsExists = eis.readBoolean();
	if (delayModelFlagsExists) {
		
	
	
		
			
		delayModelFlags =  eis.readString();
			
		
	

	}
	
}
	
	
	DelayModelFixedParametersRow* DelayModelFixedParametersRow::fromBin(EndianIStream& eis, DelayModelFixedParametersTable& table, const vector<string>& attributesSeq) {
		DelayModelFixedParametersRow* row = new  DelayModelFixedParametersRow(table);
		
		map<string, DelayModelFixedParametersAttributeFromBin>::iterator iter ;
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
					throw ConversionException("There is not method to read an attribute '"+attributesSeq.at(i)+"'.", "DelayModelFixedParametersTable");
			}
				
		}				
		return row;
	}

	//
	// A collection of methods to set the value of the attributes from their textual value in the XML representation
	// of one row.
	//
	
	// Convert a string into an Tag 
	void DelayModelFixedParametersRow::delayModelFixedParametersIdFromText(const string & s) {
		 
          
		delayModelFixedParametersId = ASDMValuesParser::parse<Tag>(s);
          
		
	}
	
	
	// Convert a string into an String 
	void DelayModelFixedParametersRow::delayModelVersionFromText(const string & s) {
		 
          
		delayModelVersion = ASDMValuesParser::parse<string>(s);
          
		
	}
	
	
	// Convert a string into an Tag 
	void DelayModelFixedParametersRow::execBlockIdFromText(const string & s) {
		 
          
		execBlockId = ASDMValuesParser::parse<Tag>(s);
          
		
	}
	

	
	// Convert a string into an AngularRate 
	void DelayModelFixedParametersRow::gaussConstantFromText(const string & s) {
		gaussConstantExists = true;
		 
          
		gaussConstant = ASDMValuesParser::parse<AngularRate>(s);
          
		
	}
	
	
	// Convert a string into an double 
	void DelayModelFixedParametersRow::newtonianConstantFromText(const string & s) {
		newtonianConstantExists = true;
		 
          
		newtonianConstant = ASDMValuesParser::parse<double>(s);
          
		
	}
	
	
	// Convert a string into an double 
	void DelayModelFixedParametersRow::gravityFromText(const string & s) {
		gravityExists = true;
		 
          
		gravity = ASDMValuesParser::parse<double>(s);
          
		
	}
	
	
	// Convert a string into an double 
	void DelayModelFixedParametersRow::earthFlatteningFromText(const string & s) {
		earthFlatteningExists = true;
		 
          
		earthFlattening = ASDMValuesParser::parse<double>(s);
          
		
	}
	
	
	// Convert a string into an Length 
	void DelayModelFixedParametersRow::earthRadiusFromText(const string & s) {
		earthRadiusExists = true;
		 
          
		earthRadius = ASDMValuesParser::parse<Length>(s);
          
		
	}
	
	
	// Convert a string into an double 
	void DelayModelFixedParametersRow::moonEarthMassRatioFromText(const string & s) {
		moonEarthMassRatioExists = true;
		 
          
		moonEarthMassRatio = ASDMValuesParser::parse<double>(s);
          
		
	}
	
	
	// Convert a string into an String 
	void DelayModelFixedParametersRow::ephemerisEpochFromText(const string & s) {
		ephemerisEpochExists = true;
		 
          
		ephemerisEpoch = ASDMValuesParser::parse<string>(s);
          
		
	}
	
	
	// Convert a string into an double 
	void DelayModelFixedParametersRow::earthTideLagFromText(const string & s) {
		earthTideLagExists = true;
		 
          
		earthTideLag = ASDMValuesParser::parse<double>(s);
          
		
	}
	
	
	// Convert a string into an double 
	void DelayModelFixedParametersRow::earthGMFromText(const string & s) {
		earthGMExists = true;
		 
          
		earthGM = ASDMValuesParser::parse<double>(s);
          
		
	}
	
	
	// Convert a string into an double 
	void DelayModelFixedParametersRow::moonGMFromText(const string & s) {
		moonGMExists = true;
		 
          
		moonGM = ASDMValuesParser::parse<double>(s);
          
		
	}
	
	
	// Convert a string into an double 
	void DelayModelFixedParametersRow::sunGMFromText(const string & s) {
		sunGMExists = true;
		 
          
		sunGM = ASDMValuesParser::parse<double>(s);
          
		
	}
	
	
	// Convert a string into an double 
	void DelayModelFixedParametersRow::loveNumberHFromText(const string & s) {
		loveNumberHExists = true;
		 
          
		loveNumberH = ASDMValuesParser::parse<double>(s);
          
		
	}
	
	
	// Convert a string into an double 
	void DelayModelFixedParametersRow::loveNumberLFromText(const string & s) {
		loveNumberLExists = true;
		 
          
		loveNumberL = ASDMValuesParser::parse<double>(s);
          
		
	}
	
	
	// Convert a string into an AngularRate 
	void DelayModelFixedParametersRow::precessionConstantFromText(const string & s) {
		precessionConstantExists = true;
		 
          
		precessionConstant = ASDMValuesParser::parse<AngularRate>(s);
          
		
	}
	
	
	// Convert a string into an double 
	void DelayModelFixedParametersRow::lightTime1AUFromText(const string & s) {
		lightTime1AUExists = true;
		 
          
		lightTime1AU = ASDMValuesParser::parse<double>(s);
          
		
	}
	
	
	// Convert a string into an Speed 
	void DelayModelFixedParametersRow::speedOfLightFromText(const string & s) {
		speedOfLightExists = true;
		 
          
		speedOfLight = ASDMValuesParser::parse<Speed>(s);
          
		
	}
	
	
	// Convert a string into an String 
	void DelayModelFixedParametersRow::delayModelFlagsFromText(const string & s) {
		delayModelFlagsExists = true;
		 
          
		delayModelFlags = ASDMValuesParser::parse<string>(s);
          
		
	}
	
	
	
	void DelayModelFixedParametersRow::fromText(const std::string& attributeName, const std::string&  t) {
		map<string, DelayModelFixedParametersAttributeFromText>::iterator iter;
		if ((iter = fromTextMethods.find(attributeName)) == fromTextMethods.end())
			throw ConversionException("I do not know what to do with '"+attributeName+"' and its content '"+t+"' (while parsing an XML document)", "DelayModelFixedParametersTable");
		(this->*(iter->second))(t);
	}
			
	////////////////////////////////////////////////
	// Intrinsic Table Attributes getters/setters //
	////////////////////////////////////////////////
	
	

	
 	/**
 	 * Get delayModelFixedParametersId.
 	 * @return delayModelFixedParametersId as Tag
 	 */
 	Tag DelayModelFixedParametersRow::getDelayModelFixedParametersId() const {
	
  		return delayModelFixedParametersId;
 	}

 	/**
 	 * Set delayModelFixedParametersId with the specified Tag.
 	 * @param delayModelFixedParametersId The Tag value to which delayModelFixedParametersId is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void DelayModelFixedParametersRow::setDelayModelFixedParametersId (Tag delayModelFixedParametersId)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("delayModelFixedParametersId", "DelayModelFixedParameters");
		
  		}
  	
 		this->delayModelFixedParametersId = delayModelFixedParametersId;
	
 	}
	
	

	

	
 	/**
 	 * Get delayModelVersion.
 	 * @return delayModelVersion as std::string
 	 */
 	std::string DelayModelFixedParametersRow::getDelayModelVersion() const {
	
  		return delayModelVersion;
 	}

 	/**
 	 * Set delayModelVersion with the specified std::string.
 	 * @param delayModelVersion The std::string value to which delayModelVersion is to be set.
 	 
 	
 		
 	 */
 	void DelayModelFixedParametersRow::setDelayModelVersion (std::string delayModelVersion)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->delayModelVersion = delayModelVersion;
	
 	}
	
	

	
	/**
	 * The attribute gaussConstant is optional. Return true if this attribute exists.
	 * @return true if and only if the gaussConstant attribute exists. 
	 */
	bool DelayModelFixedParametersRow::isGaussConstantExists() const {
		return gaussConstantExists;
	}
	

	
 	/**
 	 * Get gaussConstant, which is optional.
 	 * @return gaussConstant as AngularRate
 	 * @throw IllegalAccessException If gaussConstant does not exist.
 	 */
 	AngularRate DelayModelFixedParametersRow::getGaussConstant() const  {
		if (!gaussConstantExists) {
			throw IllegalAccessException("gaussConstant", "DelayModelFixedParameters");
		}
	
  		return gaussConstant;
 	}

 	/**
 	 * Set gaussConstant with the specified AngularRate.
 	 * @param gaussConstant The AngularRate value to which gaussConstant is to be set.
 	 
 	
 	 */
 	void DelayModelFixedParametersRow::setGaussConstant (AngularRate gaussConstant) {
	
 		this->gaussConstant = gaussConstant;
	
		gaussConstantExists = true;
	
 	}
	
	
	/**
	 * Mark gaussConstant, which is an optional field, as non-existent.
	 */
	void DelayModelFixedParametersRow::clearGaussConstant () {
		gaussConstantExists = false;
	}
	

	
	/**
	 * The attribute newtonianConstant is optional. Return true if this attribute exists.
	 * @return true if and only if the newtonianConstant attribute exists. 
	 */
	bool DelayModelFixedParametersRow::isNewtonianConstantExists() const {
		return newtonianConstantExists;
	}
	

	
 	/**
 	 * Get newtonianConstant, which is optional.
 	 * @return newtonianConstant as double
 	 * @throw IllegalAccessException If newtonianConstant does not exist.
 	 */
 	double DelayModelFixedParametersRow::getNewtonianConstant() const  {
		if (!newtonianConstantExists) {
			throw IllegalAccessException("newtonianConstant", "DelayModelFixedParameters");
		}
	
  		return newtonianConstant;
 	}

 	/**
 	 * Set newtonianConstant with the specified double.
 	 * @param newtonianConstant The double value to which newtonianConstant is to be set.
 	 
 	
 	 */
 	void DelayModelFixedParametersRow::setNewtonianConstant (double newtonianConstant) {
	
 		this->newtonianConstant = newtonianConstant;
	
		newtonianConstantExists = true;
	
 	}
	
	
	/**
	 * Mark newtonianConstant, which is an optional field, as non-existent.
	 */
	void DelayModelFixedParametersRow::clearNewtonianConstant () {
		newtonianConstantExists = false;
	}
	

	
	/**
	 * The attribute gravity is optional. Return true if this attribute exists.
	 * @return true if and only if the gravity attribute exists. 
	 */
	bool DelayModelFixedParametersRow::isGravityExists() const {
		return gravityExists;
	}
	

	
 	/**
 	 * Get gravity, which is optional.
 	 * @return gravity as double
 	 * @throw IllegalAccessException If gravity does not exist.
 	 */
 	double DelayModelFixedParametersRow::getGravity() const  {
		if (!gravityExists) {
			throw IllegalAccessException("gravity", "DelayModelFixedParameters");
		}
	
  		return gravity;
 	}

 	/**
 	 * Set gravity with the specified double.
 	 * @param gravity The double value to which gravity is to be set.
 	 
 	
 	 */
 	void DelayModelFixedParametersRow::setGravity (double gravity) {
	
 		this->gravity = gravity;
	
		gravityExists = true;
	
 	}
	
	
	/**
	 * Mark gravity, which is an optional field, as non-existent.
	 */
	void DelayModelFixedParametersRow::clearGravity () {
		gravityExists = false;
	}
	

	
	/**
	 * The attribute earthFlattening is optional. Return true if this attribute exists.
	 * @return true if and only if the earthFlattening attribute exists. 
	 */
	bool DelayModelFixedParametersRow::isEarthFlatteningExists() const {
		return earthFlatteningExists;
	}
	

	
 	/**
 	 * Get earthFlattening, which is optional.
 	 * @return earthFlattening as double
 	 * @throw IllegalAccessException If earthFlattening does not exist.
 	 */
 	double DelayModelFixedParametersRow::getEarthFlattening() const  {
		if (!earthFlatteningExists) {
			throw IllegalAccessException("earthFlattening", "DelayModelFixedParameters");
		}
	
  		return earthFlattening;
 	}

 	/**
 	 * Set earthFlattening with the specified double.
 	 * @param earthFlattening The double value to which earthFlattening is to be set.
 	 
 	
 	 */
 	void DelayModelFixedParametersRow::setEarthFlattening (double earthFlattening) {
	
 		this->earthFlattening = earthFlattening;
	
		earthFlatteningExists = true;
	
 	}
	
	
	/**
	 * Mark earthFlattening, which is an optional field, as non-existent.
	 */
	void DelayModelFixedParametersRow::clearEarthFlattening () {
		earthFlatteningExists = false;
	}
	

	
	/**
	 * The attribute earthRadius is optional. Return true if this attribute exists.
	 * @return true if and only if the earthRadius attribute exists. 
	 */
	bool DelayModelFixedParametersRow::isEarthRadiusExists() const {
		return earthRadiusExists;
	}
	

	
 	/**
 	 * Get earthRadius, which is optional.
 	 * @return earthRadius as Length
 	 * @throw IllegalAccessException If earthRadius does not exist.
 	 */
 	Length DelayModelFixedParametersRow::getEarthRadius() const  {
		if (!earthRadiusExists) {
			throw IllegalAccessException("earthRadius", "DelayModelFixedParameters");
		}
	
  		return earthRadius;
 	}

 	/**
 	 * Set earthRadius with the specified Length.
 	 * @param earthRadius The Length value to which earthRadius is to be set.
 	 
 	
 	 */
 	void DelayModelFixedParametersRow::setEarthRadius (Length earthRadius) {
	
 		this->earthRadius = earthRadius;
	
		earthRadiusExists = true;
	
 	}
	
	
	/**
	 * Mark earthRadius, which is an optional field, as non-existent.
	 */
	void DelayModelFixedParametersRow::clearEarthRadius () {
		earthRadiusExists = false;
	}
	

	
	/**
	 * The attribute moonEarthMassRatio is optional. Return true if this attribute exists.
	 * @return true if and only if the moonEarthMassRatio attribute exists. 
	 */
	bool DelayModelFixedParametersRow::isMoonEarthMassRatioExists() const {
		return moonEarthMassRatioExists;
	}
	

	
 	/**
 	 * Get moonEarthMassRatio, which is optional.
 	 * @return moonEarthMassRatio as double
 	 * @throw IllegalAccessException If moonEarthMassRatio does not exist.
 	 */
 	double DelayModelFixedParametersRow::getMoonEarthMassRatio() const  {
		if (!moonEarthMassRatioExists) {
			throw IllegalAccessException("moonEarthMassRatio", "DelayModelFixedParameters");
		}
	
  		return moonEarthMassRatio;
 	}

 	/**
 	 * Set moonEarthMassRatio with the specified double.
 	 * @param moonEarthMassRatio The double value to which moonEarthMassRatio is to be set.
 	 
 	
 	 */
 	void DelayModelFixedParametersRow::setMoonEarthMassRatio (double moonEarthMassRatio) {
	
 		this->moonEarthMassRatio = moonEarthMassRatio;
	
		moonEarthMassRatioExists = true;
	
 	}
	
	
	/**
	 * Mark moonEarthMassRatio, which is an optional field, as non-existent.
	 */
	void DelayModelFixedParametersRow::clearMoonEarthMassRatio () {
		moonEarthMassRatioExists = false;
	}
	

	
	/**
	 * The attribute ephemerisEpoch is optional. Return true if this attribute exists.
	 * @return true if and only if the ephemerisEpoch attribute exists. 
	 */
	bool DelayModelFixedParametersRow::isEphemerisEpochExists() const {
		return ephemerisEpochExists;
	}
	

	
 	/**
 	 * Get ephemerisEpoch, which is optional.
 	 * @return ephemerisEpoch as std::string
 	 * @throw IllegalAccessException If ephemerisEpoch does not exist.
 	 */
 	std::string DelayModelFixedParametersRow::getEphemerisEpoch() const  {
		if (!ephemerisEpochExists) {
			throw IllegalAccessException("ephemerisEpoch", "DelayModelFixedParameters");
		}
	
  		return ephemerisEpoch;
 	}

 	/**
 	 * Set ephemerisEpoch with the specified std::string.
 	 * @param ephemerisEpoch The std::string value to which ephemerisEpoch is to be set.
 	 
 	
 	 */
 	void DelayModelFixedParametersRow::setEphemerisEpoch (std::string ephemerisEpoch) {
	
 		this->ephemerisEpoch = ephemerisEpoch;
	
		ephemerisEpochExists = true;
	
 	}
	
	
	/**
	 * Mark ephemerisEpoch, which is an optional field, as non-existent.
	 */
	void DelayModelFixedParametersRow::clearEphemerisEpoch () {
		ephemerisEpochExists = false;
	}
	

	
	/**
	 * The attribute earthTideLag is optional. Return true if this attribute exists.
	 * @return true if and only if the earthTideLag attribute exists. 
	 */
	bool DelayModelFixedParametersRow::isEarthTideLagExists() const {
		return earthTideLagExists;
	}
	

	
 	/**
 	 * Get earthTideLag, which is optional.
 	 * @return earthTideLag as double
 	 * @throw IllegalAccessException If earthTideLag does not exist.
 	 */
 	double DelayModelFixedParametersRow::getEarthTideLag() const  {
		if (!earthTideLagExists) {
			throw IllegalAccessException("earthTideLag", "DelayModelFixedParameters");
		}
	
  		return earthTideLag;
 	}

 	/**
 	 * Set earthTideLag with the specified double.
 	 * @param earthTideLag The double value to which earthTideLag is to be set.
 	 
 	
 	 */
 	void DelayModelFixedParametersRow::setEarthTideLag (double earthTideLag) {
	
 		this->earthTideLag = earthTideLag;
	
		earthTideLagExists = true;
	
 	}
	
	
	/**
	 * Mark earthTideLag, which is an optional field, as non-existent.
	 */
	void DelayModelFixedParametersRow::clearEarthTideLag () {
		earthTideLagExists = false;
	}
	

	
	/**
	 * The attribute earthGM is optional. Return true if this attribute exists.
	 * @return true if and only if the earthGM attribute exists. 
	 */
	bool DelayModelFixedParametersRow::isEarthGMExists() const {
		return earthGMExists;
	}
	

	
 	/**
 	 * Get earthGM, which is optional.
 	 * @return earthGM as double
 	 * @throw IllegalAccessException If earthGM does not exist.
 	 */
 	double DelayModelFixedParametersRow::getEarthGM() const  {
		if (!earthGMExists) {
			throw IllegalAccessException("earthGM", "DelayModelFixedParameters");
		}
	
  		return earthGM;
 	}

 	/**
 	 * Set earthGM with the specified double.
 	 * @param earthGM The double value to which earthGM is to be set.
 	 
 	
 	 */
 	void DelayModelFixedParametersRow::setEarthGM (double earthGM) {
	
 		this->earthGM = earthGM;
	
		earthGMExists = true;
	
 	}
	
	
	/**
	 * Mark earthGM, which is an optional field, as non-existent.
	 */
	void DelayModelFixedParametersRow::clearEarthGM () {
		earthGMExists = false;
	}
	

	
	/**
	 * The attribute moonGM is optional. Return true if this attribute exists.
	 * @return true if and only if the moonGM attribute exists. 
	 */
	bool DelayModelFixedParametersRow::isMoonGMExists() const {
		return moonGMExists;
	}
	

	
 	/**
 	 * Get moonGM, which is optional.
 	 * @return moonGM as double
 	 * @throw IllegalAccessException If moonGM does not exist.
 	 */
 	double DelayModelFixedParametersRow::getMoonGM() const  {
		if (!moonGMExists) {
			throw IllegalAccessException("moonGM", "DelayModelFixedParameters");
		}
	
  		return moonGM;
 	}

 	/**
 	 * Set moonGM with the specified double.
 	 * @param moonGM The double value to which moonGM is to be set.
 	 
 	
 	 */
 	void DelayModelFixedParametersRow::setMoonGM (double moonGM) {
	
 		this->moonGM = moonGM;
	
		moonGMExists = true;
	
 	}
	
	
	/**
	 * Mark moonGM, which is an optional field, as non-existent.
	 */
	void DelayModelFixedParametersRow::clearMoonGM () {
		moonGMExists = false;
	}
	

	
	/**
	 * The attribute sunGM is optional. Return true if this attribute exists.
	 * @return true if and only if the sunGM attribute exists. 
	 */
	bool DelayModelFixedParametersRow::isSunGMExists() const {
		return sunGMExists;
	}
	

	
 	/**
 	 * Get sunGM, which is optional.
 	 * @return sunGM as double
 	 * @throw IllegalAccessException If sunGM does not exist.
 	 */
 	double DelayModelFixedParametersRow::getSunGM() const  {
		if (!sunGMExists) {
			throw IllegalAccessException("sunGM", "DelayModelFixedParameters");
		}
	
  		return sunGM;
 	}

 	/**
 	 * Set sunGM with the specified double.
 	 * @param sunGM The double value to which sunGM is to be set.
 	 
 	
 	 */
 	void DelayModelFixedParametersRow::setSunGM (double sunGM) {
	
 		this->sunGM = sunGM;
	
		sunGMExists = true;
	
 	}
	
	
	/**
	 * Mark sunGM, which is an optional field, as non-existent.
	 */
	void DelayModelFixedParametersRow::clearSunGM () {
		sunGMExists = false;
	}
	

	
	/**
	 * The attribute loveNumberH is optional. Return true if this attribute exists.
	 * @return true if and only if the loveNumberH attribute exists. 
	 */
	bool DelayModelFixedParametersRow::isLoveNumberHExists() const {
		return loveNumberHExists;
	}
	

	
 	/**
 	 * Get loveNumberH, which is optional.
 	 * @return loveNumberH as double
 	 * @throw IllegalAccessException If loveNumberH does not exist.
 	 */
 	double DelayModelFixedParametersRow::getLoveNumberH() const  {
		if (!loveNumberHExists) {
			throw IllegalAccessException("loveNumberH", "DelayModelFixedParameters");
		}
	
  		return loveNumberH;
 	}

 	/**
 	 * Set loveNumberH with the specified double.
 	 * @param loveNumberH The double value to which loveNumberH is to be set.
 	 
 	
 	 */
 	void DelayModelFixedParametersRow::setLoveNumberH (double loveNumberH) {
	
 		this->loveNumberH = loveNumberH;
	
		loveNumberHExists = true;
	
 	}
	
	
	/**
	 * Mark loveNumberH, which is an optional field, as non-existent.
	 */
	void DelayModelFixedParametersRow::clearLoveNumberH () {
		loveNumberHExists = false;
	}
	

	
	/**
	 * The attribute loveNumberL is optional. Return true if this attribute exists.
	 * @return true if and only if the loveNumberL attribute exists. 
	 */
	bool DelayModelFixedParametersRow::isLoveNumberLExists() const {
		return loveNumberLExists;
	}
	

	
 	/**
 	 * Get loveNumberL, which is optional.
 	 * @return loveNumberL as double
 	 * @throw IllegalAccessException If loveNumberL does not exist.
 	 */
 	double DelayModelFixedParametersRow::getLoveNumberL() const  {
		if (!loveNumberLExists) {
			throw IllegalAccessException("loveNumberL", "DelayModelFixedParameters");
		}
	
  		return loveNumberL;
 	}

 	/**
 	 * Set loveNumberL with the specified double.
 	 * @param loveNumberL The double value to which loveNumberL is to be set.
 	 
 	
 	 */
 	void DelayModelFixedParametersRow::setLoveNumberL (double loveNumberL) {
	
 		this->loveNumberL = loveNumberL;
	
		loveNumberLExists = true;
	
 	}
	
	
	/**
	 * Mark loveNumberL, which is an optional field, as non-existent.
	 */
	void DelayModelFixedParametersRow::clearLoveNumberL () {
		loveNumberLExists = false;
	}
	

	
	/**
	 * The attribute precessionConstant is optional. Return true if this attribute exists.
	 * @return true if and only if the precessionConstant attribute exists. 
	 */
	bool DelayModelFixedParametersRow::isPrecessionConstantExists() const {
		return precessionConstantExists;
	}
	

	
 	/**
 	 * Get precessionConstant, which is optional.
 	 * @return precessionConstant as AngularRate
 	 * @throw IllegalAccessException If precessionConstant does not exist.
 	 */
 	AngularRate DelayModelFixedParametersRow::getPrecessionConstant() const  {
		if (!precessionConstantExists) {
			throw IllegalAccessException("precessionConstant", "DelayModelFixedParameters");
		}
	
  		return precessionConstant;
 	}

 	/**
 	 * Set precessionConstant with the specified AngularRate.
 	 * @param precessionConstant The AngularRate value to which precessionConstant is to be set.
 	 
 	
 	 */
 	void DelayModelFixedParametersRow::setPrecessionConstant (AngularRate precessionConstant) {
	
 		this->precessionConstant = precessionConstant;
	
		precessionConstantExists = true;
	
 	}
	
	
	/**
	 * Mark precessionConstant, which is an optional field, as non-existent.
	 */
	void DelayModelFixedParametersRow::clearPrecessionConstant () {
		precessionConstantExists = false;
	}
	

	
	/**
	 * The attribute lightTime1AU is optional. Return true if this attribute exists.
	 * @return true if and only if the lightTime1AU attribute exists. 
	 */
	bool DelayModelFixedParametersRow::isLightTime1AUExists() const {
		return lightTime1AUExists;
	}
	

	
 	/**
 	 * Get lightTime1AU, which is optional.
 	 * @return lightTime1AU as double
 	 * @throw IllegalAccessException If lightTime1AU does not exist.
 	 */
 	double DelayModelFixedParametersRow::getLightTime1AU() const  {
		if (!lightTime1AUExists) {
			throw IllegalAccessException("lightTime1AU", "DelayModelFixedParameters");
		}
	
  		return lightTime1AU;
 	}

 	/**
 	 * Set lightTime1AU with the specified double.
 	 * @param lightTime1AU The double value to which lightTime1AU is to be set.
 	 
 	
 	 */
 	void DelayModelFixedParametersRow::setLightTime1AU (double lightTime1AU) {
	
 		this->lightTime1AU = lightTime1AU;
	
		lightTime1AUExists = true;
	
 	}
	
	
	/**
	 * Mark lightTime1AU, which is an optional field, as non-existent.
	 */
	void DelayModelFixedParametersRow::clearLightTime1AU () {
		lightTime1AUExists = false;
	}
	

	
	/**
	 * The attribute speedOfLight is optional. Return true if this attribute exists.
	 * @return true if and only if the speedOfLight attribute exists. 
	 */
	bool DelayModelFixedParametersRow::isSpeedOfLightExists() const {
		return speedOfLightExists;
	}
	

	
 	/**
 	 * Get speedOfLight, which is optional.
 	 * @return speedOfLight as Speed
 	 * @throw IllegalAccessException If speedOfLight does not exist.
 	 */
 	Speed DelayModelFixedParametersRow::getSpeedOfLight() const  {
		if (!speedOfLightExists) {
			throw IllegalAccessException("speedOfLight", "DelayModelFixedParameters");
		}
	
  		return speedOfLight;
 	}

 	/**
 	 * Set speedOfLight with the specified Speed.
 	 * @param speedOfLight The Speed value to which speedOfLight is to be set.
 	 
 	
 	 */
 	void DelayModelFixedParametersRow::setSpeedOfLight (Speed speedOfLight) {
	
 		this->speedOfLight = speedOfLight;
	
		speedOfLightExists = true;
	
 	}
	
	
	/**
	 * Mark speedOfLight, which is an optional field, as non-existent.
	 */
	void DelayModelFixedParametersRow::clearSpeedOfLight () {
		speedOfLightExists = false;
	}
	

	
	/**
	 * The attribute delayModelFlags is optional. Return true if this attribute exists.
	 * @return true if and only if the delayModelFlags attribute exists. 
	 */
	bool DelayModelFixedParametersRow::isDelayModelFlagsExists() const {
		return delayModelFlagsExists;
	}
	

	
 	/**
 	 * Get delayModelFlags, which is optional.
 	 * @return delayModelFlags as std::string
 	 * @throw IllegalAccessException If delayModelFlags does not exist.
 	 */
 	std::string DelayModelFixedParametersRow::getDelayModelFlags() const  {
		if (!delayModelFlagsExists) {
			throw IllegalAccessException("delayModelFlags", "DelayModelFixedParameters");
		}
	
  		return delayModelFlags;
 	}

 	/**
 	 * Set delayModelFlags with the specified std::string.
 	 * @param delayModelFlags The std::string value to which delayModelFlags is to be set.
 	 
 	
 	 */
 	void DelayModelFixedParametersRow::setDelayModelFlags (std::string delayModelFlags) {
	
 		this->delayModelFlags = delayModelFlags;
	
		delayModelFlagsExists = true;
	
 	}
	
	
	/**
	 * Mark delayModelFlags, which is an optional field, as non-existent.
	 */
	void DelayModelFixedParametersRow::clearDelayModelFlags () {
		delayModelFlagsExists = false;
	}
	

	
	///////////////////////////////////////////////
	// Extrinsic Table Attributes getters/setters//
	///////////////////////////////////////////////
	
	

	
 	/**
 	 * Get execBlockId.
 	 * @return execBlockId as Tag
 	 */
 	Tag DelayModelFixedParametersRow::getExecBlockId() const {
	
  		return execBlockId;
 	}

 	/**
 	 * Set execBlockId with the specified Tag.
 	 * @param execBlockId The Tag value to which execBlockId is to be set.
 	 
 	
 		
 	 */
 	void DelayModelFixedParametersRow::setExecBlockId (Tag execBlockId)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->execBlockId = execBlockId;
	
 	}
	
	


	//////////////////////////////////////
	// Links Attributes getters/setters //
	//////////////////////////////////////
	
	
	
	
		

	/**
	 * Returns the pointer to the row in the ExecBlock table having ExecBlock.execBlockId == execBlockId
	 * @return a ExecBlockRow*
	 * 
	 
	 */
	 ExecBlockRow* DelayModelFixedParametersRow::getExecBlockUsingExecBlockId() {
	 
	 	return table.getContainer().getExecBlock().getRowByKey(execBlockId);
	 }
	 

	

	
	/**
	 * Create a DelayModelFixedParametersRow.
	 * <p>
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 */ 
	DelayModelFixedParametersRow::DelayModelFixedParametersRow (DelayModelFixedParametersTable &t) : table(t) {
		hasBeenAdded = false;
		
	
	

	

	
		gaussConstantExists = false;
	

	
		newtonianConstantExists = false;
	

	
		gravityExists = false;
	

	
		earthFlatteningExists = false;
	

	
		earthRadiusExists = false;
	

	
		moonEarthMassRatioExists = false;
	

	
		ephemerisEpochExists = false;
	

	
		earthTideLagExists = false;
	

	
		earthGMExists = false;
	

	
		moonGMExists = false;
	

	
		sunGMExists = false;
	

	
		loveNumberHExists = false;
	

	
		loveNumberLExists = false;
	

	
		precessionConstantExists = false;
	

	
		lightTime1AUExists = false;
	

	
		speedOfLightExists = false;
	

	
		delayModelFlagsExists = false;
	

	
	

	
	
	
	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	
	
	 fromBinMethods["delayModelFixedParametersId"] = &DelayModelFixedParametersRow::delayModelFixedParametersIdFromBin; 
	 fromBinMethods["delayModelVersion"] = &DelayModelFixedParametersRow::delayModelVersionFromBin; 
	 fromBinMethods["execBlockId"] = &DelayModelFixedParametersRow::execBlockIdFromBin; 
		
	
	 fromBinMethods["gaussConstant"] = &DelayModelFixedParametersRow::gaussConstantFromBin; 
	 fromBinMethods["newtonianConstant"] = &DelayModelFixedParametersRow::newtonianConstantFromBin; 
	 fromBinMethods["gravity"] = &DelayModelFixedParametersRow::gravityFromBin; 
	 fromBinMethods["earthFlattening"] = &DelayModelFixedParametersRow::earthFlatteningFromBin; 
	 fromBinMethods["earthRadius"] = &DelayModelFixedParametersRow::earthRadiusFromBin; 
	 fromBinMethods["moonEarthMassRatio"] = &DelayModelFixedParametersRow::moonEarthMassRatioFromBin; 
	 fromBinMethods["ephemerisEpoch"] = &DelayModelFixedParametersRow::ephemerisEpochFromBin; 
	 fromBinMethods["earthTideLag"] = &DelayModelFixedParametersRow::earthTideLagFromBin; 
	 fromBinMethods["earthGM"] = &DelayModelFixedParametersRow::earthGMFromBin; 
	 fromBinMethods["moonGM"] = &DelayModelFixedParametersRow::moonGMFromBin; 
	 fromBinMethods["sunGM"] = &DelayModelFixedParametersRow::sunGMFromBin; 
	 fromBinMethods["loveNumberH"] = &DelayModelFixedParametersRow::loveNumberHFromBin; 
	 fromBinMethods["loveNumberL"] = &DelayModelFixedParametersRow::loveNumberLFromBin; 
	 fromBinMethods["precessionConstant"] = &DelayModelFixedParametersRow::precessionConstantFromBin; 
	 fromBinMethods["lightTime1AU"] = &DelayModelFixedParametersRow::lightTime1AUFromBin; 
	 fromBinMethods["speedOfLight"] = &DelayModelFixedParametersRow::speedOfLightFromBin; 
	 fromBinMethods["delayModelFlags"] = &DelayModelFixedParametersRow::delayModelFlagsFromBin; 
	
	
	
	
				 
	fromTextMethods["delayModelFixedParametersId"] = &DelayModelFixedParametersRow::delayModelFixedParametersIdFromText;
		 
	
				 
	fromTextMethods["delayModelVersion"] = &DelayModelFixedParametersRow::delayModelVersionFromText;
		 
	
				 
	fromTextMethods["execBlockId"] = &DelayModelFixedParametersRow::execBlockIdFromText;
		 
	

	 
				
	fromTextMethods["gaussConstant"] = &DelayModelFixedParametersRow::gaussConstantFromText;
		 	
	 
				
	fromTextMethods["newtonianConstant"] = &DelayModelFixedParametersRow::newtonianConstantFromText;
		 	
	 
				
	fromTextMethods["gravity"] = &DelayModelFixedParametersRow::gravityFromText;
		 	
	 
				
	fromTextMethods["earthFlattening"] = &DelayModelFixedParametersRow::earthFlatteningFromText;
		 	
	 
				
	fromTextMethods["earthRadius"] = &DelayModelFixedParametersRow::earthRadiusFromText;
		 	
	 
				
	fromTextMethods["moonEarthMassRatio"] = &DelayModelFixedParametersRow::moonEarthMassRatioFromText;
		 	
	 
				
	fromTextMethods["ephemerisEpoch"] = &DelayModelFixedParametersRow::ephemerisEpochFromText;
		 	
	 
				
	fromTextMethods["earthTideLag"] = &DelayModelFixedParametersRow::earthTideLagFromText;
		 	
	 
				
	fromTextMethods["earthGM"] = &DelayModelFixedParametersRow::earthGMFromText;
		 	
	 
				
	fromTextMethods["moonGM"] = &DelayModelFixedParametersRow::moonGMFromText;
		 	
	 
				
	fromTextMethods["sunGM"] = &DelayModelFixedParametersRow::sunGMFromText;
		 	
	 
				
	fromTextMethods["loveNumberH"] = &DelayModelFixedParametersRow::loveNumberHFromText;
		 	
	 
				
	fromTextMethods["loveNumberL"] = &DelayModelFixedParametersRow::loveNumberLFromText;
		 	
	 
				
	fromTextMethods["precessionConstant"] = &DelayModelFixedParametersRow::precessionConstantFromText;
		 	
	 
				
	fromTextMethods["lightTime1AU"] = &DelayModelFixedParametersRow::lightTime1AUFromText;
		 	
	 
				
	fromTextMethods["speedOfLight"] = &DelayModelFixedParametersRow::speedOfLightFromText;
		 	
	 
				
	fromTextMethods["delayModelFlags"] = &DelayModelFixedParametersRow::delayModelFlagsFromText;
		 	
		
	}
	
	DelayModelFixedParametersRow::DelayModelFixedParametersRow (DelayModelFixedParametersTable &t, DelayModelFixedParametersRow *row) : table(t) {
		hasBeenAdded = false;
		
		if (row == 0) {
	
	
	

	

	
		gaussConstantExists = false;
	

	
		newtonianConstantExists = false;
	

	
		gravityExists = false;
	

	
		earthFlatteningExists = false;
	

	
		earthRadiusExists = false;
	

	
		moonEarthMassRatioExists = false;
	

	
		ephemerisEpochExists = false;
	

	
		earthTideLagExists = false;
	

	
		earthGMExists = false;
	

	
		moonGMExists = false;
	

	
		sunGMExists = false;
	

	
		loveNumberHExists = false;
	

	
		loveNumberLExists = false;
	

	
		precessionConstantExists = false;
	

	
		lightTime1AUExists = false;
	

	
		speedOfLightExists = false;
	

	
		delayModelFlagsExists = false;
	

	
	
		
		}
		else {
	
		
			delayModelFixedParametersId = row->delayModelFixedParametersId;
		
		
		
		
			delayModelVersion = row->delayModelVersion;
		
			execBlockId = row->execBlockId;
		
		
		
		
		if (row->gaussConstantExists) {
			gaussConstant = row->gaussConstant;		
			gaussConstantExists = true;
		}
		else
			gaussConstantExists = false;
		
		if (row->newtonianConstantExists) {
			newtonianConstant = row->newtonianConstant;		
			newtonianConstantExists = true;
		}
		else
			newtonianConstantExists = false;
		
		if (row->gravityExists) {
			gravity = row->gravity;		
			gravityExists = true;
		}
		else
			gravityExists = false;
		
		if (row->earthFlatteningExists) {
			earthFlattening = row->earthFlattening;		
			earthFlatteningExists = true;
		}
		else
			earthFlatteningExists = false;
		
		if (row->earthRadiusExists) {
			earthRadius = row->earthRadius;		
			earthRadiusExists = true;
		}
		else
			earthRadiusExists = false;
		
		if (row->moonEarthMassRatioExists) {
			moonEarthMassRatio = row->moonEarthMassRatio;		
			moonEarthMassRatioExists = true;
		}
		else
			moonEarthMassRatioExists = false;
		
		if (row->ephemerisEpochExists) {
			ephemerisEpoch = row->ephemerisEpoch;		
			ephemerisEpochExists = true;
		}
		else
			ephemerisEpochExists = false;
		
		if (row->earthTideLagExists) {
			earthTideLag = row->earthTideLag;		
			earthTideLagExists = true;
		}
		else
			earthTideLagExists = false;
		
		if (row->earthGMExists) {
			earthGM = row->earthGM;		
			earthGMExists = true;
		}
		else
			earthGMExists = false;
		
		if (row->moonGMExists) {
			moonGM = row->moonGM;		
			moonGMExists = true;
		}
		else
			moonGMExists = false;
		
		if (row->sunGMExists) {
			sunGM = row->sunGM;		
			sunGMExists = true;
		}
		else
			sunGMExists = false;
		
		if (row->loveNumberHExists) {
			loveNumberH = row->loveNumberH;		
			loveNumberHExists = true;
		}
		else
			loveNumberHExists = false;
		
		if (row->loveNumberLExists) {
			loveNumberL = row->loveNumberL;		
			loveNumberLExists = true;
		}
		else
			loveNumberLExists = false;
		
		if (row->precessionConstantExists) {
			precessionConstant = row->precessionConstant;		
			precessionConstantExists = true;
		}
		else
			precessionConstantExists = false;
		
		if (row->lightTime1AUExists) {
			lightTime1AU = row->lightTime1AU;		
			lightTime1AUExists = true;
		}
		else
			lightTime1AUExists = false;
		
		if (row->speedOfLightExists) {
			speedOfLight = row->speedOfLight;		
			speedOfLightExists = true;
		}
		else
			speedOfLightExists = false;
		
		if (row->delayModelFlagsExists) {
			delayModelFlags = row->delayModelFlags;		
			delayModelFlagsExists = true;
		}
		else
			delayModelFlagsExists = false;
		
		}
		
		 fromBinMethods["delayModelFixedParametersId"] = &DelayModelFixedParametersRow::delayModelFixedParametersIdFromBin; 
		 fromBinMethods["delayModelVersion"] = &DelayModelFixedParametersRow::delayModelVersionFromBin; 
		 fromBinMethods["execBlockId"] = &DelayModelFixedParametersRow::execBlockIdFromBin; 
			
	
		 fromBinMethods["gaussConstant"] = &DelayModelFixedParametersRow::gaussConstantFromBin; 
		 fromBinMethods["newtonianConstant"] = &DelayModelFixedParametersRow::newtonianConstantFromBin; 
		 fromBinMethods["gravity"] = &DelayModelFixedParametersRow::gravityFromBin; 
		 fromBinMethods["earthFlattening"] = &DelayModelFixedParametersRow::earthFlatteningFromBin; 
		 fromBinMethods["earthRadius"] = &DelayModelFixedParametersRow::earthRadiusFromBin; 
		 fromBinMethods["moonEarthMassRatio"] = &DelayModelFixedParametersRow::moonEarthMassRatioFromBin; 
		 fromBinMethods["ephemerisEpoch"] = &DelayModelFixedParametersRow::ephemerisEpochFromBin; 
		 fromBinMethods["earthTideLag"] = &DelayModelFixedParametersRow::earthTideLagFromBin; 
		 fromBinMethods["earthGM"] = &DelayModelFixedParametersRow::earthGMFromBin; 
		 fromBinMethods["moonGM"] = &DelayModelFixedParametersRow::moonGMFromBin; 
		 fromBinMethods["sunGM"] = &DelayModelFixedParametersRow::sunGMFromBin; 
		 fromBinMethods["loveNumberH"] = &DelayModelFixedParametersRow::loveNumberHFromBin; 
		 fromBinMethods["loveNumberL"] = &DelayModelFixedParametersRow::loveNumberLFromBin; 
		 fromBinMethods["precessionConstant"] = &DelayModelFixedParametersRow::precessionConstantFromBin; 
		 fromBinMethods["lightTime1AU"] = &DelayModelFixedParametersRow::lightTime1AUFromBin; 
		 fromBinMethods["speedOfLight"] = &DelayModelFixedParametersRow::speedOfLightFromBin; 
		 fromBinMethods["delayModelFlags"] = &DelayModelFixedParametersRow::delayModelFlagsFromBin; 
			
	}

	
	bool DelayModelFixedParametersRow::compareNoAutoInc(std::string delayModelVersion, Tag execBlockId) {
		bool result;
		result = true;
		
	
		
		result = result && (this->delayModelVersion == delayModelVersion);
		
		if (!result) return false;
	

	
		
		result = result && (this->execBlockId == execBlockId);
		
		if (!result) return false;
	

		return result;
	}	
	
	
	
	bool DelayModelFixedParametersRow::compareRequiredValue(std::string delayModelVersion, Tag execBlockId) {
		bool result;
		result = true;
		
	
		if (!(this->delayModelVersion == delayModelVersion)) return false;
	

	
		if (!(this->execBlockId == execBlockId)) return false;
	

		return result;
	}
	
	
	/**
	 * Return true if all required attributes of the value part are equal to their homologues
	 * in x and false otherwise.
	 *

	 * @param x a pointer on the DelayModelFixedParametersRow whose required attributes of the value part 

	 * will be compared with those of this.
	 * @return a boolean.
	 */
	bool DelayModelFixedParametersRow::equalByRequiredValue(DelayModelFixedParametersRow*  x ) {
		
			
		if (this->delayModelVersion != x->delayModelVersion) return false;
			
		if (this->execBlockId != x->execBlockId) return false;
			
		
		return true;
	}	
	
/*
	 map<string, DelayModelFixedParametersAttributeFromBin> DelayModelFixedParametersRow::initFromBinMethods() {
		map<string, DelayModelFixedParametersAttributeFromBin> result;
		
		result["delayModelFixedParametersId"] = &DelayModelFixedParametersRow::delayModelFixedParametersIdFromBin;
		result["delayModelVersion"] = &DelayModelFixedParametersRow::delayModelVersionFromBin;
		result["execBlockId"] = &DelayModelFixedParametersRow::execBlockIdFromBin;
		
		
		result["gaussConstant"] = &DelayModelFixedParametersRow::gaussConstantFromBin;
		result["newtonianConstant"] = &DelayModelFixedParametersRow::newtonianConstantFromBin;
		result["gravity"] = &DelayModelFixedParametersRow::gravityFromBin;
		result["earthFlattening"] = &DelayModelFixedParametersRow::earthFlatteningFromBin;
		result["earthRadius"] = &DelayModelFixedParametersRow::earthRadiusFromBin;
		result["moonEarthMassRatio"] = &DelayModelFixedParametersRow::moonEarthMassRatioFromBin;
		result["ephemerisEpoch"] = &DelayModelFixedParametersRow::ephemerisEpochFromBin;
		result["earthTideLag"] = &DelayModelFixedParametersRow::earthTideLagFromBin;
		result["earthGM"] = &DelayModelFixedParametersRow::earthGMFromBin;
		result["moonGM"] = &DelayModelFixedParametersRow::moonGMFromBin;
		result["sunGM"] = &DelayModelFixedParametersRow::sunGMFromBin;
		result["loveNumberH"] = &DelayModelFixedParametersRow::loveNumberHFromBin;
		result["loveNumberL"] = &DelayModelFixedParametersRow::loveNumberLFromBin;
		result["precessionConstant"] = &DelayModelFixedParametersRow::precessionConstantFromBin;
		result["lightTime1AU"] = &DelayModelFixedParametersRow::lightTime1AUFromBin;
		result["speedOfLight"] = &DelayModelFixedParametersRow::speedOfLightFromBin;
		result["delayModelFlags"] = &DelayModelFixedParametersRow::delayModelFlagsFromBin;
			
		
		return result;	
	}
*/	
} // End namespace asdm
 
