
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
 * File CalPositionRow.cpp
 */
 
#include <vector>
#include <set>

#include <alma/ASDM/ASDM.h>
#include <alma/ASDM/CalPositionRow.h>
#include <alma/ASDM/CalPositionTable.h>

#include <alma/ASDM/CalDataTable.h>
#include <alma/ASDM/CalDataRow.h>

#include <alma/ASDM/CalReductionTable.h>
#include <alma/ASDM/CalReductionRow.h>
	

using asdm::ASDM;
using asdm::CalPositionRow;
using asdm::CalPositionTable;

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
	CalPositionRow::~CalPositionRow() {
	}

	/**
	 * Return the table to which this row belongs.
	 */
	CalPositionTable &CalPositionRow::getTable() const {
		return table;
	}

	bool CalPositionRow::isAdded() const {
		return hasBeenAdded;
	}	

	void CalPositionRow::isAdded(bool added) {
		hasBeenAdded = added;
	}
	
#ifndef WITHOUT_ACS
	using asdmIDL::CalPositionRowIDL;
#endif
	
#ifndef WITHOUT_ACS
	/**
	 * Return this row in the form of an IDL struct.
	 * @return The values of this row as a CalPositionRowIDL struct.
	 */
	CalPositionRowIDL *CalPositionRow::toIDL() const {
		CalPositionRowIDL *x = new CalPositionRowIDL ();
		
		// Fill the IDL structure.
	
		
	
  		
		
		
			
				
		x->antennaName = CORBA::string_dup(antennaName.c_str());
				
 			
		
	

	
  		
		
		
			
				
		x->atmPhaseCorrection = atmPhaseCorrection;
 				
 			
		
	

	
  		
		
		
			
		x->startValidTime = startValidTime.toIDLArrayTime();
			
		
	

	
  		
		
		
			
		x->endValidTime = endValidTime.toIDLArrayTime();
			
		
	

	
  		
		
		
			
		x->antennaPosition.length(antennaPosition.size());
		for (unsigned int i = 0; i < antennaPosition.size(); ++i) {
			
			x->antennaPosition[i] = antennaPosition.at(i).toIDLLength();
			
	 	}
			
		
	

	
  		
		
		
			
				
		x->stationName = CORBA::string_dup(stationName.c_str());
				
 			
		
	

	
  		
		
		
			
		x->stationPosition.length(stationPosition.size());
		for (unsigned int i = 0; i < stationPosition.size(); ++i) {
			
			x->stationPosition[i] = stationPosition.at(i).toIDLLength();
			
	 	}
			
		
	

	
  		
		
		
			
				
		x->positionMethod = positionMethod;
 				
 			
		
	

	
  		
		
		
			
				
		x->receiverBand = receiverBand;
 				
 			
		
	

	
  		
		
		
			
				
		x->numAntenna = numAntenna;
 				
 			
		
	

	
  		
		
		
			
		x->refAntennaNames.length(refAntennaNames.size());
		for (unsigned int i = 0; i < refAntennaNames.size(); ++i) {
			
				
			x->refAntennaNames[i] = CORBA::string_dup(refAntennaNames.at(i).c_str());
				
	 		
	 	}
			
		
	

	
  		
		
		
			
		x->axesOffset = axesOffset.toIDLLength();
			
		
	

	
  		
		
		
			
		x->axesOffsetErr = axesOffsetErr.toIDLLength();
			
		
	

	
  		
		
		
			
				
		x->axesOffsetFixed = axesOffsetFixed;
 				
 			
		
	

	
  		
		
		
			
		x->positionOffset.length(positionOffset.size());
		for (unsigned int i = 0; i < positionOffset.size(); ++i) {
			
			x->positionOffset[i] = positionOffset.at(i).toIDLLength();
			
	 	}
			
		
	

	
  		
		
		
			
		x->positionErr.length(positionErr.size());
		for (unsigned int i = 0; i < positionErr.size(); ++i) {
			
			x->positionErr[i] = positionErr.at(i).toIDLLength();
			
	 	}
			
		
	

	
  		
		
		
			
				
		x->reducedChiSquared = reducedChiSquared;
 				
 			
		
	

	
  		
		
		x->delayRmsExists = delayRmsExists;
		
		
			
				
		x->delayRms = delayRms;
 				
 			
		
	

	
  		
		
		x->phaseRmsExists = phaseRmsExists;
		
		
			
		x->phaseRms = phaseRms.toIDLAngle();
			
		
	

	
	
		
	
  	
 		
		
	 	
			
		x->calDataId = calDataId.toIDLTag();
			
	 	 		
  	

	
  	
 		
		
	 	
			
		x->calReductionId = calReductionId.toIDLTag();
			
	 	 		
  	

	
		
	

	

		
		return x;
	
	}
	
	void CalPositionRow::toIDL(asdmIDL::CalPositionRowIDL& x) const {
		// Set the x's fields.
	
		
	
  		
		
		
			
				
		x.antennaName = CORBA::string_dup(antennaName.c_str());
				
 			
		
	

	
  		
		
		
			
				
		x.atmPhaseCorrection = atmPhaseCorrection;
 				
 			
		
	

	
  		
		
		
			
		x.startValidTime = startValidTime.toIDLArrayTime();
			
		
	

	
  		
		
		
			
		x.endValidTime = endValidTime.toIDLArrayTime();
			
		
	

	
  		
		
		
			
		x.antennaPosition.length(antennaPosition.size());
		for (unsigned int i = 0; i < antennaPosition.size(); ++i) {
			
			x.antennaPosition[i] = antennaPosition.at(i).toIDLLength();
			
	 	}
			
		
	

	
  		
		
		
			
				
		x.stationName = CORBA::string_dup(stationName.c_str());
				
 			
		
	

	
  		
		
		
			
		x.stationPosition.length(stationPosition.size());
		for (unsigned int i = 0; i < stationPosition.size(); ++i) {
			
			x.stationPosition[i] = stationPosition.at(i).toIDLLength();
			
	 	}
			
		
	

	
  		
		
		
			
				
		x.positionMethod = positionMethod;
 				
 			
		
	

	
  		
		
		
			
				
		x.receiverBand = receiverBand;
 				
 			
		
	

	
  		
		
		
			
				
		x.numAntenna = numAntenna;
 				
 			
		
	

	
  		
		
		
			
		x.refAntennaNames.length(refAntennaNames.size());
		for (unsigned int i = 0; i < refAntennaNames.size(); ++i) {
			
				
			x.refAntennaNames[i] = CORBA::string_dup(refAntennaNames.at(i).c_str());
				
	 		
	 	}
			
		
	

	
  		
		
		
			
		x.axesOffset = axesOffset.toIDLLength();
			
		
	

	
  		
		
		
			
		x.axesOffsetErr = axesOffsetErr.toIDLLength();
			
		
	

	
  		
		
		
			
				
		x.axesOffsetFixed = axesOffsetFixed;
 				
 			
		
	

	
  		
		
		
			
		x.positionOffset.length(positionOffset.size());
		for (unsigned int i = 0; i < positionOffset.size(); ++i) {
			
			x.positionOffset[i] = positionOffset.at(i).toIDLLength();
			
	 	}
			
		
	

	
  		
		
		
			
		x.positionErr.length(positionErr.size());
		for (unsigned int i = 0; i < positionErr.size(); ++i) {
			
			x.positionErr[i] = positionErr.at(i).toIDLLength();
			
	 	}
			
		
	

	
  		
		
		
			
				
		x.reducedChiSquared = reducedChiSquared;
 				
 			
		
	

	
  		
		
		x.delayRmsExists = delayRmsExists;
		
		
			
				
		x.delayRms = delayRms;
 				
 			
		
	

	
  		
		
		x.phaseRmsExists = phaseRmsExists;
		
		
			
		x.phaseRms = phaseRms.toIDLAngle();
			
		
	

	
	
		
	
  	
 		
		
	 	
			
		x.calDataId = calDataId.toIDLTag();
			
	 	 		
  	

	
  	
 		
		
	 	
			
		x.calReductionId = calReductionId.toIDLTag();
			
	 	 		
  	

	
		
	

	

	
	}
#endif
	

#ifndef WITHOUT_ACS
	/**
	 * Fill the values of this row from the IDL struct CalPositionRowIDL.
	 * @param x The IDL struct containing the values used to fill this row.
	 */
	void CalPositionRow::setFromIDL (CalPositionRowIDL x){
		try {
		// Fill the values from x.
	
		
	
		
		
			
		setAntennaName(string (x.antennaName));
			
 		
		
	

	
		
		
			
		setAtmPhaseCorrection(x.atmPhaseCorrection);
  			
 		
		
	

	
		
		
			
		setStartValidTime(ArrayTime (x.startValidTime));
			
 		
		
	

	
		
		
			
		setEndValidTime(ArrayTime (x.endValidTime));
			
 		
		
	

	
		
		
			
		antennaPosition .clear();
		for (unsigned int i = 0; i <x.antennaPosition.length(); ++i) {
			
			antennaPosition.push_back(Length (x.antennaPosition[i]));
			
		}
			
  		
		
	

	
		
		
			
		setStationName(string (x.stationName));
			
 		
		
	

	
		
		
			
		stationPosition .clear();
		for (unsigned int i = 0; i <x.stationPosition.length(); ++i) {
			
			stationPosition.push_back(Length (x.stationPosition[i]));
			
		}
			
  		
		
	

	
		
		
			
		setPositionMethod(x.positionMethod);
  			
 		
		
	

	
		
		
			
		setReceiverBand(x.receiverBand);
  			
 		
		
	

	
		
		
			
		setNumAntenna(x.numAntenna);
  			
 		
		
	

	
		
		
			
		refAntennaNames .clear();
		for (unsigned int i = 0; i <x.refAntennaNames.length(); ++i) {
			
			refAntennaNames.push_back(string (x.refAntennaNames[i]));
			
		}
			
  		
		
	

	
		
		
			
		setAxesOffset(Length (x.axesOffset));
			
 		
		
	

	
		
		
			
		setAxesOffsetErr(Length (x.axesOffsetErr));
			
 		
		
	

	
		
		
			
		setAxesOffsetFixed(x.axesOffsetFixed);
  			
 		
		
	

	
		
		
			
		positionOffset .clear();
		for (unsigned int i = 0; i <x.positionOffset.length(); ++i) {
			
			positionOffset.push_back(Length (x.positionOffset[i]));
			
		}
			
  		
		
	

	
		
		
			
		positionErr .clear();
		for (unsigned int i = 0; i <x.positionErr.length(); ++i) {
			
			positionErr.push_back(Length (x.positionErr[i]));
			
		}
			
  		
		
	

	
		
		
			
		setReducedChiSquared(x.reducedChiSquared);
  			
 		
		
	

	
		
		delayRmsExists = x.delayRmsExists;
		if (x.delayRmsExists) {
		
		
			
		setDelayRms(x.delayRms);
  			
 		
		
		}
		
	

	
		
		phaseRmsExists = x.phaseRmsExists;
		if (x.phaseRmsExists) {
		
		
			
		setPhaseRms(Angle (x.phaseRms));
			
 		
		
		}
		
	

	
	
		
	
		
		
			
		setCalDataId(Tag (x.calDataId));
			
 		
		
	

	
		
		
			
		setCalReductionId(Tag (x.calReductionId));
			
 		
		
	

	
		
	

	

		} catch (const IllegalAccessException &err) {
			throw ConversionException (err.getMessage(),"CalPosition");
		}
	}
#endif
	
	/**
	 * Return this row in the form of an XML string.
	 * @return The values of this row as an XML string.
	 */
	string CalPositionRow::toXML() const {
		string buf;
		buf.append("<row> \n");
		
	
		
  	
 		
		
		Parser::toXML(antennaName, "antennaName", buf);
		
		
	

  	
 		
		
			buf.append(EnumerationParser::toXML("atmPhaseCorrection", atmPhaseCorrection));
		
		
	

  	
 		
		
		Parser::toXML(startValidTime, "startValidTime", buf);
		
		
	

  	
 		
		
		Parser::toXML(endValidTime, "endValidTime", buf);
		
		
	

  	
 		
		
		Parser::toXML(antennaPosition, "antennaPosition", buf);
		
		
	

  	
 		
		
		Parser::toXML(stationName, "stationName", buf);
		
		
	

  	
 		
		
		Parser::toXML(stationPosition, "stationPosition", buf);
		
		
	

  	
 		
		
			buf.append(EnumerationParser::toXML("positionMethod", positionMethod));
		
		
	

  	
 		
		
			buf.append(EnumerationParser::toXML("receiverBand", receiverBand));
		
		
	

  	
 		
		
		Parser::toXML(numAntenna, "numAntenna", buf);
		
		
	

  	
 		
		
		Parser::toXML(refAntennaNames, "refAntennaNames", buf);
		
		
	

  	
 		
		
		Parser::toXML(axesOffset, "axesOffset", buf);
		
		
	

  	
 		
		
		Parser::toXML(axesOffsetErr, "axesOffsetErr", buf);
		
		
	

  	
 		
		
		Parser::toXML(axesOffsetFixed, "axesOffsetFixed", buf);
		
		
	

  	
 		
		
		Parser::toXML(positionOffset, "positionOffset", buf);
		
		
	

  	
 		
		
		Parser::toXML(positionErr, "positionErr", buf);
		
		
	

  	
 		
		
		Parser::toXML(reducedChiSquared, "reducedChiSquared", buf);
		
		
	

  	
 		
		if (delayRmsExists) {
		
		
		Parser::toXML(delayRms, "delayRms", buf);
		
		
		}
		
	

  	
 		
		if (phaseRmsExists) {
		
		
		Parser::toXML(phaseRms, "phaseRms", buf);
		
		
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
	void CalPositionRow::setFromXML (string rowDoc) {
		Parser row(rowDoc);
		string s = "";
		try {
	
		
	
  		
			
	  	setAntennaName(Parser::getString("antennaName","CalPosition",rowDoc));
			
		
	

	
		
		
		
		atmPhaseCorrection = EnumerationParser::getAtmPhaseCorrection("atmPhaseCorrection","CalPosition",rowDoc);
		
		
		
	

	
  		
			
	  	setStartValidTime(Parser::getArrayTime("startValidTime","CalPosition",rowDoc));
			
		
	

	
  		
			
	  	setEndValidTime(Parser::getArrayTime("endValidTime","CalPosition",rowDoc));
			
		
	

	
  		
			
					
	  	setAntennaPosition(Parser::get1DLength("antennaPosition","CalPosition",rowDoc));
	  			
	  		
		
	

	
  		
			
	  	setStationName(Parser::getString("stationName","CalPosition",rowDoc));
			
		
	

	
  		
			
					
	  	setStationPosition(Parser::get1DLength("stationPosition","CalPosition",rowDoc));
	  			
	  		
		
	

	
		
		
		
		positionMethod = EnumerationParser::getPositionMethod("positionMethod","CalPosition",rowDoc);
		
		
		
	

	
		
		
		
		receiverBand = EnumerationParser::getReceiverBand("receiverBand","CalPosition",rowDoc);
		
		
		
	

	
  		
			
	  	setNumAntenna(Parser::getInteger("numAntenna","CalPosition",rowDoc));
			
		
	

	
  		
			
					
	  	setRefAntennaNames(Parser::get1DString("refAntennaNames","CalPosition",rowDoc));
	  			
	  		
		
	

	
  		
			
	  	setAxesOffset(Parser::getLength("axesOffset","CalPosition",rowDoc));
			
		
	

	
  		
			
	  	setAxesOffsetErr(Parser::getLength("axesOffsetErr","CalPosition",rowDoc));
			
		
	

	
  		
			
	  	setAxesOffsetFixed(Parser::getBoolean("axesOffsetFixed","CalPosition",rowDoc));
			
		
	

	
  		
			
					
	  	setPositionOffset(Parser::get1DLength("positionOffset","CalPosition",rowDoc));
	  			
	  		
		
	

	
  		
			
					
	  	setPositionErr(Parser::get1DLength("positionErr","CalPosition",rowDoc));
	  			
	  		
		
	

	
  		
			
	  	setReducedChiSquared(Parser::getDouble("reducedChiSquared","CalPosition",rowDoc));
			
		
	

	
  		
        if (row.isStr("<delayRms>")) {
			
	  		setDelayRms(Parser::getDouble("delayRms","CalPosition",rowDoc));
			
		}
 		
	

	
  		
        if (row.isStr("<phaseRms>")) {
			
	  		setPhaseRms(Parser::getAngle("phaseRms","CalPosition",rowDoc));
			
		}
 		
	

	
	
		
	
  		
			
	  	setCalDataId(Parser::getTag("calDataId","CalData",rowDoc));
			
		
	

	
  		
			
	  	setCalReductionId(Parser::getTag("calReductionId","CalReduction",rowDoc));
			
		
	

	
		
	

	

		} catch (const IllegalAccessException &err) {
			throw ConversionException (err.getMessage(),"CalPosition");
		}
	}
	
	void CalPositionRow::toBin(EndianOSStream& eoss) {
	
	
	
	
		
						
			eoss.writeString(antennaName);
				
		
	

	
	
		
					
		eoss.writeString(CAtmPhaseCorrection::name(atmPhaseCorrection));
			/* eoss.writeInt(atmPhaseCorrection); */
				
		
	

	
	
		
	calDataId.toBin(eoss);
		
	

	
	
		
	calReductionId.toBin(eoss);
		
	

	
	
		
	startValidTime.toBin(eoss);
		
	

	
	
		
	endValidTime.toBin(eoss);
		
	

	
	
		
	Length::toBin(antennaPosition, eoss);
		
	

	
	
		
						
			eoss.writeString(stationName);
				
		
	

	
	
		
	Length::toBin(stationPosition, eoss);
		
	

	
	
		
					
		eoss.writeString(CPositionMethod::name(positionMethod));
			/* eoss.writeInt(positionMethod); */
				
		
	

	
	
		
					
		eoss.writeString(CReceiverBand::name(receiverBand));
			/* eoss.writeInt(receiverBand); */
				
		
	

	
	
		
						
			eoss.writeInt(numAntenna);
				
		
	

	
	
		
		
			
		eoss.writeInt((int) refAntennaNames.size());
		for (unsigned int i = 0; i < refAntennaNames.size(); i++)
				
			eoss.writeString(refAntennaNames.at(i));
				
				
						
		
	

	
	
		
	axesOffset.toBin(eoss);
		
	

	
	
		
	axesOffsetErr.toBin(eoss);
		
	

	
	
		
						
			eoss.writeBoolean(axesOffsetFixed);
				
		
	

	
	
		
	Length::toBin(positionOffset, eoss);
		
	

	
	
		
	Length::toBin(positionErr, eoss);
		
	

	
	
		
						
			eoss.writeDouble(reducedChiSquared);
				
		
	


	
	
	eoss.writeBoolean(delayRmsExists);
	if (delayRmsExists) {
	
	
	
		
						
			eoss.writeDouble(delayRms);
				
		
	

	}

	eoss.writeBoolean(phaseRmsExists);
	if (phaseRmsExists) {
	
	
	
		
	phaseRms.toBin(eoss);
		
	

	}

	}
	
void CalPositionRow::antennaNameFromBin(EndianIStream& eis) {
		
	
	
		
			
		antennaName =  eis.readString();
			
		
	
	
}
void CalPositionRow::atmPhaseCorrectionFromBin(EndianIStream& eis) {
		
	
	
		
			
		atmPhaseCorrection = CAtmPhaseCorrection::literal(eis.readString());
			
		
	
	
}
void CalPositionRow::calDataIdFromBin(EndianIStream& eis) {
		
	
		
		
		calDataId =  Tag::fromBin(eis);
		
	
	
}
void CalPositionRow::calReductionIdFromBin(EndianIStream& eis) {
		
	
		
		
		calReductionId =  Tag::fromBin(eis);
		
	
	
}
void CalPositionRow::startValidTimeFromBin(EndianIStream& eis) {
		
	
		
		
		startValidTime =  ArrayTime::fromBin(eis);
		
	
	
}
void CalPositionRow::endValidTimeFromBin(EndianIStream& eis) {
		
	
		
		
		endValidTime =  ArrayTime::fromBin(eis);
		
	
	
}
void CalPositionRow::antennaPositionFromBin(EndianIStream& eis) {
		
	
		
		
			
	
	antennaPosition = Length::from1DBin(eis);	
	

		
	
	
}
void CalPositionRow::stationNameFromBin(EndianIStream& eis) {
		
	
	
		
			
		stationName =  eis.readString();
			
		
	
	
}
void CalPositionRow::stationPositionFromBin(EndianIStream& eis) {
		
	
		
		
			
	
	stationPosition = Length::from1DBin(eis);	
	

		
	
	
}
void CalPositionRow::positionMethodFromBin(EndianIStream& eis) {
		
	
	
		
			
		positionMethod = CPositionMethod::literal(eis.readString());
			
		
	
	
}
void CalPositionRow::receiverBandFromBin(EndianIStream& eis) {
		
	
	
		
			
		receiverBand = CReceiverBand::literal(eis.readString());
			
		
	
	
}
void CalPositionRow::numAntennaFromBin(EndianIStream& eis) {
		
	
	
		
			
		numAntenna =  eis.readInt();
			
		
	
	
}
void CalPositionRow::refAntennaNamesFromBin(EndianIStream& eis) {
		
	
	
		
			
	
		refAntennaNames.clear();
		
		unsigned int refAntennaNamesDim1 = eis.readInt();
		for (unsigned int  i = 0 ; i < refAntennaNamesDim1; i++)
			
			refAntennaNames.push_back(eis.readString());
			
	

		
	
	
}
void CalPositionRow::axesOffsetFromBin(EndianIStream& eis) {
		
	
		
		
		axesOffset =  Length::fromBin(eis);
		
	
	
}
void CalPositionRow::axesOffsetErrFromBin(EndianIStream& eis) {
		
	
		
		
		axesOffsetErr =  Length::fromBin(eis);
		
	
	
}
void CalPositionRow::axesOffsetFixedFromBin(EndianIStream& eis) {
		
	
	
		
			
		axesOffsetFixed =  eis.readBoolean();
			
		
	
	
}
void CalPositionRow::positionOffsetFromBin(EndianIStream& eis) {
		
	
		
		
			
	
	positionOffset = Length::from1DBin(eis);	
	

		
	
	
}
void CalPositionRow::positionErrFromBin(EndianIStream& eis) {
		
	
		
		
			
	
	positionErr = Length::from1DBin(eis);	
	

		
	
	
}
void CalPositionRow::reducedChiSquaredFromBin(EndianIStream& eis) {
		
	
	
		
			
		reducedChiSquared =  eis.readDouble();
			
		
	
	
}

void CalPositionRow::delayRmsFromBin(EndianIStream& eis) {
		
	delayRmsExists = eis.readBoolean();
	if (delayRmsExists) {
		
	
	
		
			
		delayRms =  eis.readDouble();
			
		
	

	}
	
}
void CalPositionRow::phaseRmsFromBin(EndianIStream& eis) {
		
	phaseRmsExists = eis.readBoolean();
	if (phaseRmsExists) {
		
	
		
		
		phaseRms =  Angle::fromBin(eis);
		
	

	}
	
}
	
	
	CalPositionRow* CalPositionRow::fromBin(EndianIStream& eis, CalPositionTable& table, const vector<string>& attributesSeq) {
		CalPositionRow* row = new  CalPositionRow(table);
		
		map<string, CalPositionAttributeFromBin>::iterator iter ;
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
					throw ConversionException("There is not method to read an attribute '"+attributesSeq.at(i)+"'.", "CalPositionTable");
			}
				
		}				
		return row;
	}

	//
	// A collection of methods to set the value of the attributes from their textual value in the XML representation
	// of one row.
	//
	
	// Convert a string into an String 
	void CalPositionRow::antennaNameFromText(const string & s) {
		 
          
		antennaName = ASDMValuesParser::parse<string>(s);
          
		
	}
	
	
	// Convert a string into an AtmPhaseCorrection 
	void CalPositionRow::atmPhaseCorrectionFromText(const string & s) {
		 
          
		atmPhaseCorrection = ASDMValuesParser::parse<AtmPhaseCorrectionMod::AtmPhaseCorrection>(s);
          
		
	}
	
	
	// Convert a string into an Tag 
	void CalPositionRow::calDataIdFromText(const string & s) {
		 
          
		calDataId = ASDMValuesParser::parse<Tag>(s);
          
		
	}
	
	
	// Convert a string into an Tag 
	void CalPositionRow::calReductionIdFromText(const string & s) {
		 
          
		calReductionId = ASDMValuesParser::parse<Tag>(s);
          
		
	}
	
	
	// Convert a string into an ArrayTime 
	void CalPositionRow::startValidTimeFromText(const string & s) {
		 
          
		startValidTime = ASDMValuesParser::parse<ArrayTime>(s);
          
		
	}
	
	
	// Convert a string into an ArrayTime 
	void CalPositionRow::endValidTimeFromText(const string & s) {
		 
          
		endValidTime = ASDMValuesParser::parse<ArrayTime>(s);
          
		
	}
	
	
	// Convert a string into an Length 
	void CalPositionRow::antennaPositionFromText(const string & s) {
		 
          
		antennaPosition = ASDMValuesParser::parse1D<Length>(s);
          
		
	}
	
	
	// Convert a string into an String 
	void CalPositionRow::stationNameFromText(const string & s) {
		 
          
		stationName = ASDMValuesParser::parse<string>(s);
          
		
	}
	
	
	// Convert a string into an Length 
	void CalPositionRow::stationPositionFromText(const string & s) {
		 
          
		stationPosition = ASDMValuesParser::parse1D<Length>(s);
          
		
	}
	
	
	// Convert a string into an PositionMethod 
	void CalPositionRow::positionMethodFromText(const string & s) {
		 
          
		positionMethod = ASDMValuesParser::parse<PositionMethodMod::PositionMethod>(s);
          
		
	}
	
	
	// Convert a string into an ReceiverBand 
	void CalPositionRow::receiverBandFromText(const string & s) {
		 
          
		receiverBand = ASDMValuesParser::parse<ReceiverBandMod::ReceiverBand>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void CalPositionRow::numAntennaFromText(const string & s) {
		 
          
		numAntenna = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	// Convert a string into an String 
	void CalPositionRow::refAntennaNamesFromText(const string & s) {
		 
          
		refAntennaNames = ASDMValuesParser::parse1D<string>(s);
          
		
	}
	
	
	// Convert a string into an Length 
	void CalPositionRow::axesOffsetFromText(const string & s) {
		 
          
		axesOffset = ASDMValuesParser::parse<Length>(s);
          
		
	}
	
	
	// Convert a string into an Length 
	void CalPositionRow::axesOffsetErrFromText(const string & s) {
		 
          
		axesOffsetErr = ASDMValuesParser::parse<Length>(s);
          
		
	}
	
	
	// Convert a string into an boolean 
	void CalPositionRow::axesOffsetFixedFromText(const string & s) {
		 
          
		axesOffsetFixed = ASDMValuesParser::parse<bool>(s);
          
		
	}
	
	
	// Convert a string into an Length 
	void CalPositionRow::positionOffsetFromText(const string & s) {
		 
          
		positionOffset = ASDMValuesParser::parse1D<Length>(s);
          
		
	}
	
	
	// Convert a string into an Length 
	void CalPositionRow::positionErrFromText(const string & s) {
		 
          
		positionErr = ASDMValuesParser::parse1D<Length>(s);
          
		
	}
	
	
	// Convert a string into an double 
	void CalPositionRow::reducedChiSquaredFromText(const string & s) {
		 
          
		reducedChiSquared = ASDMValuesParser::parse<double>(s);
          
		
	}
	

	
	// Convert a string into an double 
	void CalPositionRow::delayRmsFromText(const string & s) {
		delayRmsExists = true;
		 
          
		delayRms = ASDMValuesParser::parse<double>(s);
          
		
	}
	
	
	// Convert a string into an Angle 
	void CalPositionRow::phaseRmsFromText(const string & s) {
		phaseRmsExists = true;
		 
          
		phaseRms = ASDMValuesParser::parse<Angle>(s);
          
		
	}
	
	
	
	void CalPositionRow::fromText(const std::string& attributeName, const std::string&  t) {
		map<string, CalPositionAttributeFromText>::iterator iter;
		if ((iter = fromTextMethods.find(attributeName)) == fromTextMethods.end())
			throw ConversionException("I do not know what to do with '"+attributeName+"' and its content '"+t+"' (while parsing an XML document)", "CalPositionTable");
		(this->*(iter->second))(t);
	}
			
	////////////////////////////////////////////////
	// Intrinsic Table Attributes getters/setters //
	////////////////////////////////////////////////
	
	

	
 	/**
 	 * Get antennaName.
 	 * @return antennaName as std::string
 	 */
 	std::string CalPositionRow::getAntennaName() const {
	
  		return antennaName;
 	}

 	/**
 	 * Set antennaName with the specified std::string.
 	 * @param antennaName The std::string value to which antennaName is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void CalPositionRow::setAntennaName (std::string antennaName)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("antennaName", "CalPosition");
		
  		}
  	
 		this->antennaName = antennaName;
	
 	}
	
	

	

	
 	/**
 	 * Get atmPhaseCorrection.
 	 * @return atmPhaseCorrection as AtmPhaseCorrectionMod::AtmPhaseCorrection
 	 */
 	AtmPhaseCorrectionMod::AtmPhaseCorrection CalPositionRow::getAtmPhaseCorrection() const {
	
  		return atmPhaseCorrection;
 	}

 	/**
 	 * Set atmPhaseCorrection with the specified AtmPhaseCorrectionMod::AtmPhaseCorrection.
 	 * @param atmPhaseCorrection The AtmPhaseCorrectionMod::AtmPhaseCorrection value to which atmPhaseCorrection is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void CalPositionRow::setAtmPhaseCorrection (AtmPhaseCorrectionMod::AtmPhaseCorrection atmPhaseCorrection)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("atmPhaseCorrection", "CalPosition");
		
  		}
  	
 		this->atmPhaseCorrection = atmPhaseCorrection;
	
 	}
	
	

	

	
 	/**
 	 * Get startValidTime.
 	 * @return startValidTime as ArrayTime
 	 */
 	ArrayTime CalPositionRow::getStartValidTime() const {
	
  		return startValidTime;
 	}

 	/**
 	 * Set startValidTime with the specified ArrayTime.
 	 * @param startValidTime The ArrayTime value to which startValidTime is to be set.
 	 
 	
 		
 	 */
 	void CalPositionRow::setStartValidTime (ArrayTime startValidTime)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->startValidTime = startValidTime;
	
 	}
	
	

	

	
 	/**
 	 * Get endValidTime.
 	 * @return endValidTime as ArrayTime
 	 */
 	ArrayTime CalPositionRow::getEndValidTime() const {
	
  		return endValidTime;
 	}

 	/**
 	 * Set endValidTime with the specified ArrayTime.
 	 * @param endValidTime The ArrayTime value to which endValidTime is to be set.
 	 
 	
 		
 	 */
 	void CalPositionRow::setEndValidTime (ArrayTime endValidTime)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->endValidTime = endValidTime;
	
 	}
	
	

	

	
 	/**
 	 * Get antennaPosition.
 	 * @return antennaPosition as std::vector<Length >
 	 */
 	std::vector<Length > CalPositionRow::getAntennaPosition() const {
	
  		return antennaPosition;
 	}

 	/**
 	 * Set antennaPosition with the specified std::vector<Length >.
 	 * @param antennaPosition The std::vector<Length > value to which antennaPosition is to be set.
 	 
 	
 		
 	 */
 	void CalPositionRow::setAntennaPosition (std::vector<Length > antennaPosition)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->antennaPosition = antennaPosition;
	
 	}
	
	

	

	
 	/**
 	 * Get stationName.
 	 * @return stationName as std::string
 	 */
 	std::string CalPositionRow::getStationName() const {
	
  		return stationName;
 	}

 	/**
 	 * Set stationName with the specified std::string.
 	 * @param stationName The std::string value to which stationName is to be set.
 	 
 	
 		
 	 */
 	void CalPositionRow::setStationName (std::string stationName)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->stationName = stationName;
	
 	}
	
	

	

	
 	/**
 	 * Get stationPosition.
 	 * @return stationPosition as std::vector<Length >
 	 */
 	std::vector<Length > CalPositionRow::getStationPosition() const {
	
  		return stationPosition;
 	}

 	/**
 	 * Set stationPosition with the specified std::vector<Length >.
 	 * @param stationPosition The std::vector<Length > value to which stationPosition is to be set.
 	 
 	
 		
 	 */
 	void CalPositionRow::setStationPosition (std::vector<Length > stationPosition)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->stationPosition = stationPosition;
	
 	}
	
	

	

	
 	/**
 	 * Get positionMethod.
 	 * @return positionMethod as PositionMethodMod::PositionMethod
 	 */
 	PositionMethodMod::PositionMethod CalPositionRow::getPositionMethod() const {
	
  		return positionMethod;
 	}

 	/**
 	 * Set positionMethod with the specified PositionMethodMod::PositionMethod.
 	 * @param positionMethod The PositionMethodMod::PositionMethod value to which positionMethod is to be set.
 	 
 	
 		
 	 */
 	void CalPositionRow::setPositionMethod (PositionMethodMod::PositionMethod positionMethod)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->positionMethod = positionMethod;
	
 	}
	
	

	

	
 	/**
 	 * Get receiverBand.
 	 * @return receiverBand as ReceiverBandMod::ReceiverBand
 	 */
 	ReceiverBandMod::ReceiverBand CalPositionRow::getReceiverBand() const {
	
  		return receiverBand;
 	}

 	/**
 	 * Set receiverBand with the specified ReceiverBandMod::ReceiverBand.
 	 * @param receiverBand The ReceiverBandMod::ReceiverBand value to which receiverBand is to be set.
 	 
 	
 		
 	 */
 	void CalPositionRow::setReceiverBand (ReceiverBandMod::ReceiverBand receiverBand)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->receiverBand = receiverBand;
	
 	}
	
	

	

	
 	/**
 	 * Get numAntenna.
 	 * @return numAntenna as int
 	 */
 	int CalPositionRow::getNumAntenna() const {
	
  		return numAntenna;
 	}

 	/**
 	 * Set numAntenna with the specified int.
 	 * @param numAntenna The int value to which numAntenna is to be set.
 	 
 	
 		
 	 */
 	void CalPositionRow::setNumAntenna (int numAntenna)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->numAntenna = numAntenna;
	
 	}
	
	

	

	
 	/**
 	 * Get refAntennaNames.
 	 * @return refAntennaNames as std::vector<std::string >
 	 */
 	std::vector<std::string > CalPositionRow::getRefAntennaNames() const {
	
  		return refAntennaNames;
 	}

 	/**
 	 * Set refAntennaNames with the specified std::vector<std::string >.
 	 * @param refAntennaNames The std::vector<std::string > value to which refAntennaNames is to be set.
 	 
 	
 		
 	 */
 	void CalPositionRow::setRefAntennaNames (std::vector<std::string > refAntennaNames)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->refAntennaNames = refAntennaNames;
	
 	}
	
	

	

	
 	/**
 	 * Get axesOffset.
 	 * @return axesOffset as Length
 	 */
 	Length CalPositionRow::getAxesOffset() const {
	
  		return axesOffset;
 	}

 	/**
 	 * Set axesOffset with the specified Length.
 	 * @param axesOffset The Length value to which axesOffset is to be set.
 	 
 	
 		
 	 */
 	void CalPositionRow::setAxesOffset (Length axesOffset)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->axesOffset = axesOffset;
	
 	}
	
	

	

	
 	/**
 	 * Get axesOffsetErr.
 	 * @return axesOffsetErr as Length
 	 */
 	Length CalPositionRow::getAxesOffsetErr() const {
	
  		return axesOffsetErr;
 	}

 	/**
 	 * Set axesOffsetErr with the specified Length.
 	 * @param axesOffsetErr The Length value to which axesOffsetErr is to be set.
 	 
 	
 		
 	 */
 	void CalPositionRow::setAxesOffsetErr (Length axesOffsetErr)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->axesOffsetErr = axesOffsetErr;
	
 	}
	
	

	

	
 	/**
 	 * Get axesOffsetFixed.
 	 * @return axesOffsetFixed as bool
 	 */
 	bool CalPositionRow::getAxesOffsetFixed() const {
	
  		return axesOffsetFixed;
 	}

 	/**
 	 * Set axesOffsetFixed with the specified bool.
 	 * @param axesOffsetFixed The bool value to which axesOffsetFixed is to be set.
 	 
 	
 		
 	 */
 	void CalPositionRow::setAxesOffsetFixed (bool axesOffsetFixed)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->axesOffsetFixed = axesOffsetFixed;
	
 	}
	
	

	

	
 	/**
 	 * Get positionOffset.
 	 * @return positionOffset as std::vector<Length >
 	 */
 	std::vector<Length > CalPositionRow::getPositionOffset() const {
	
  		return positionOffset;
 	}

 	/**
 	 * Set positionOffset with the specified std::vector<Length >.
 	 * @param positionOffset The std::vector<Length > value to which positionOffset is to be set.
 	 
 	
 		
 	 */
 	void CalPositionRow::setPositionOffset (std::vector<Length > positionOffset)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->positionOffset = positionOffset;
	
 	}
	
	

	

	
 	/**
 	 * Get positionErr.
 	 * @return positionErr as std::vector<Length >
 	 */
 	std::vector<Length > CalPositionRow::getPositionErr() const {
	
  		return positionErr;
 	}

 	/**
 	 * Set positionErr with the specified std::vector<Length >.
 	 * @param positionErr The std::vector<Length > value to which positionErr is to be set.
 	 
 	
 		
 	 */
 	void CalPositionRow::setPositionErr (std::vector<Length > positionErr)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->positionErr = positionErr;
	
 	}
	
	

	

	
 	/**
 	 * Get reducedChiSquared.
 	 * @return reducedChiSquared as double
 	 */
 	double CalPositionRow::getReducedChiSquared() const {
	
  		return reducedChiSquared;
 	}

 	/**
 	 * Set reducedChiSquared with the specified double.
 	 * @param reducedChiSquared The double value to which reducedChiSquared is to be set.
 	 
 	
 		
 	 */
 	void CalPositionRow::setReducedChiSquared (double reducedChiSquared)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->reducedChiSquared = reducedChiSquared;
	
 	}
	
	

	
	/**
	 * The attribute delayRms is optional. Return true if this attribute exists.
	 * @return true if and only if the delayRms attribute exists. 
	 */
	bool CalPositionRow::isDelayRmsExists() const {
		return delayRmsExists;
	}
	

	
 	/**
 	 * Get delayRms, which is optional.
 	 * @return delayRms as double
 	 * @throw IllegalAccessException If delayRms does not exist.
 	 */
 	double CalPositionRow::getDelayRms() const  {
		if (!delayRmsExists) {
			throw IllegalAccessException("delayRms", "CalPosition");
		}
	
  		return delayRms;
 	}

 	/**
 	 * Set delayRms with the specified double.
 	 * @param delayRms The double value to which delayRms is to be set.
 	 
 	
 	 */
 	void CalPositionRow::setDelayRms (double delayRms) {
	
 		this->delayRms = delayRms;
	
		delayRmsExists = true;
	
 	}
	
	
	/**
	 * Mark delayRms, which is an optional field, as non-existent.
	 */
	void CalPositionRow::clearDelayRms () {
		delayRmsExists = false;
	}
	

	
	/**
	 * The attribute phaseRms is optional. Return true if this attribute exists.
	 * @return true if and only if the phaseRms attribute exists. 
	 */
	bool CalPositionRow::isPhaseRmsExists() const {
		return phaseRmsExists;
	}
	

	
 	/**
 	 * Get phaseRms, which is optional.
 	 * @return phaseRms as Angle
 	 * @throw IllegalAccessException If phaseRms does not exist.
 	 */
 	Angle CalPositionRow::getPhaseRms() const  {
		if (!phaseRmsExists) {
			throw IllegalAccessException("phaseRms", "CalPosition");
		}
	
  		return phaseRms;
 	}

 	/**
 	 * Set phaseRms with the specified Angle.
 	 * @param phaseRms The Angle value to which phaseRms is to be set.
 	 
 	
 	 */
 	void CalPositionRow::setPhaseRms (Angle phaseRms) {
	
 		this->phaseRms = phaseRms;
	
		phaseRmsExists = true;
	
 	}
	
	
	/**
	 * Mark phaseRms, which is an optional field, as non-existent.
	 */
	void CalPositionRow::clearPhaseRms () {
		phaseRmsExists = false;
	}
	

	
	///////////////////////////////////////////////
	// Extrinsic Table Attributes getters/setters//
	///////////////////////////////////////////////
	
	

	
 	/**
 	 * Get calDataId.
 	 * @return calDataId as Tag
 	 */
 	Tag CalPositionRow::getCalDataId() const {
	
  		return calDataId;
 	}

 	/**
 	 * Set calDataId with the specified Tag.
 	 * @param calDataId The Tag value to which calDataId is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void CalPositionRow::setCalDataId (Tag calDataId)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("calDataId", "CalPosition");
		
  		}
  	
 		this->calDataId = calDataId;
	
 	}
	
	

	

	
 	/**
 	 * Get calReductionId.
 	 * @return calReductionId as Tag
 	 */
 	Tag CalPositionRow::getCalReductionId() const {
	
  		return calReductionId;
 	}

 	/**
 	 * Set calReductionId with the specified Tag.
 	 * @param calReductionId The Tag value to which calReductionId is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void CalPositionRow::setCalReductionId (Tag calReductionId)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("calReductionId", "CalPosition");
		
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
	 CalDataRow* CalPositionRow::getCalDataUsingCalDataId() {
	 
	 	return table.getContainer().getCalData().getRowByKey(calDataId);
	 }
	 

	

	
	
	
		

	/**
	 * Returns the pointer to the row in the CalReduction table having CalReduction.calReductionId == calReductionId
	 * @return a CalReductionRow*
	 * 
	 
	 */
	 CalReductionRow* CalPositionRow::getCalReductionUsingCalReductionId() {
	 
	 	return table.getContainer().getCalReduction().getRowByKey(calReductionId);
	 }
	 

	

	
	/**
	 * Create a CalPositionRow.
	 * <p>
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 */ 
	CalPositionRow::CalPositionRow (CalPositionTable &t) : table(t) {
		hasBeenAdded = false;
		
	
	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	
		delayRmsExists = false;
	

	
		phaseRmsExists = false;
	

	
	

	

	
	
	
	

	
// This attribute is scalar and has an enumeration type. Let's initialize it to some valid value (the 1st of the enumeration).		
atmPhaseCorrection = CAtmPhaseCorrection::from_int(0);
	

	

	

	

	

	

	
// This attribute is scalar and has an enumeration type. Let's initialize it to some valid value (the 1st of the enumeration).		
positionMethod = CPositionMethod::from_int(0);
	

	
// This attribute is scalar and has an enumeration type. Let's initialize it to some valid value (the 1st of the enumeration).		
receiverBand = CReceiverBand::from_int(0);
	

	

	

	

	

	

	

	

	

	

	

	
	
	 fromBinMethods["antennaName"] = &CalPositionRow::antennaNameFromBin; 
	 fromBinMethods["atmPhaseCorrection"] = &CalPositionRow::atmPhaseCorrectionFromBin; 
	 fromBinMethods["calDataId"] = &CalPositionRow::calDataIdFromBin; 
	 fromBinMethods["calReductionId"] = &CalPositionRow::calReductionIdFromBin; 
	 fromBinMethods["startValidTime"] = &CalPositionRow::startValidTimeFromBin; 
	 fromBinMethods["endValidTime"] = &CalPositionRow::endValidTimeFromBin; 
	 fromBinMethods["antennaPosition"] = &CalPositionRow::antennaPositionFromBin; 
	 fromBinMethods["stationName"] = &CalPositionRow::stationNameFromBin; 
	 fromBinMethods["stationPosition"] = &CalPositionRow::stationPositionFromBin; 
	 fromBinMethods["positionMethod"] = &CalPositionRow::positionMethodFromBin; 
	 fromBinMethods["receiverBand"] = &CalPositionRow::receiverBandFromBin; 
	 fromBinMethods["numAntenna"] = &CalPositionRow::numAntennaFromBin; 
	 fromBinMethods["refAntennaNames"] = &CalPositionRow::refAntennaNamesFromBin; 
	 fromBinMethods["axesOffset"] = &CalPositionRow::axesOffsetFromBin; 
	 fromBinMethods["axesOffsetErr"] = &CalPositionRow::axesOffsetErrFromBin; 
	 fromBinMethods["axesOffsetFixed"] = &CalPositionRow::axesOffsetFixedFromBin; 
	 fromBinMethods["positionOffset"] = &CalPositionRow::positionOffsetFromBin; 
	 fromBinMethods["positionErr"] = &CalPositionRow::positionErrFromBin; 
	 fromBinMethods["reducedChiSquared"] = &CalPositionRow::reducedChiSquaredFromBin; 
		
	
	 fromBinMethods["delayRms"] = &CalPositionRow::delayRmsFromBin; 
	 fromBinMethods["phaseRms"] = &CalPositionRow::phaseRmsFromBin; 
	
	
	
	
				 
	fromTextMethods["antennaName"] = &CalPositionRow::antennaNameFromText;
		 
	
				 
	fromTextMethods["atmPhaseCorrection"] = &CalPositionRow::atmPhaseCorrectionFromText;
		 
	
				 
	fromTextMethods["calDataId"] = &CalPositionRow::calDataIdFromText;
		 
	
				 
	fromTextMethods["calReductionId"] = &CalPositionRow::calReductionIdFromText;
		 
	
				 
	fromTextMethods["startValidTime"] = &CalPositionRow::startValidTimeFromText;
		 
	
				 
	fromTextMethods["endValidTime"] = &CalPositionRow::endValidTimeFromText;
		 
	
				 
	fromTextMethods["antennaPosition"] = &CalPositionRow::antennaPositionFromText;
		 
	
				 
	fromTextMethods["stationName"] = &CalPositionRow::stationNameFromText;
		 
	
				 
	fromTextMethods["stationPosition"] = &CalPositionRow::stationPositionFromText;
		 
	
				 
	fromTextMethods["positionMethod"] = &CalPositionRow::positionMethodFromText;
		 
	
				 
	fromTextMethods["receiverBand"] = &CalPositionRow::receiverBandFromText;
		 
	
				 
	fromTextMethods["numAntenna"] = &CalPositionRow::numAntennaFromText;
		 
	
				 
	fromTextMethods["refAntennaNames"] = &CalPositionRow::refAntennaNamesFromText;
		 
	
				 
	fromTextMethods["axesOffset"] = &CalPositionRow::axesOffsetFromText;
		 
	
				 
	fromTextMethods["axesOffsetErr"] = &CalPositionRow::axesOffsetErrFromText;
		 
	
				 
	fromTextMethods["axesOffsetFixed"] = &CalPositionRow::axesOffsetFixedFromText;
		 
	
				 
	fromTextMethods["positionOffset"] = &CalPositionRow::positionOffsetFromText;
		 
	
				 
	fromTextMethods["positionErr"] = &CalPositionRow::positionErrFromText;
		 
	
				 
	fromTextMethods["reducedChiSquared"] = &CalPositionRow::reducedChiSquaredFromText;
		 
	

	 
				
	fromTextMethods["delayRms"] = &CalPositionRow::delayRmsFromText;
		 	
	 
				
	fromTextMethods["phaseRms"] = &CalPositionRow::phaseRmsFromText;
		 	
		
	}
	
	CalPositionRow::CalPositionRow (CalPositionTable &t, CalPositionRow *row) : table(t) {
		hasBeenAdded = false;
		
		if (row == 0) {
	
	
	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	
		delayRmsExists = false;
	

	
		phaseRmsExists = false;
	

	
	

	
		
		}
		else {
	
		
			antennaName = row->antennaName;
		
			atmPhaseCorrection = row->atmPhaseCorrection;
		
			calDataId = row->calDataId;
		
			calReductionId = row->calReductionId;
		
		
		
		
			startValidTime = row->startValidTime;
		
			endValidTime = row->endValidTime;
		
			antennaPosition = row->antennaPosition;
		
			stationName = row->stationName;
		
			stationPosition = row->stationPosition;
		
			positionMethod = row->positionMethod;
		
			receiverBand = row->receiverBand;
		
			numAntenna = row->numAntenna;
		
			refAntennaNames = row->refAntennaNames;
		
			axesOffset = row->axesOffset;
		
			axesOffsetErr = row->axesOffsetErr;
		
			axesOffsetFixed = row->axesOffsetFixed;
		
			positionOffset = row->positionOffset;
		
			positionErr = row->positionErr;
		
			reducedChiSquared = row->reducedChiSquared;
		
		
		
		
		if (row->delayRmsExists) {
			delayRms = row->delayRms;		
			delayRmsExists = true;
		}
		else
			delayRmsExists = false;
		
		if (row->phaseRmsExists) {
			phaseRms = row->phaseRms;		
			phaseRmsExists = true;
		}
		else
			phaseRmsExists = false;
		
		}
		
		 fromBinMethods["antennaName"] = &CalPositionRow::antennaNameFromBin; 
		 fromBinMethods["atmPhaseCorrection"] = &CalPositionRow::atmPhaseCorrectionFromBin; 
		 fromBinMethods["calDataId"] = &CalPositionRow::calDataIdFromBin; 
		 fromBinMethods["calReductionId"] = &CalPositionRow::calReductionIdFromBin; 
		 fromBinMethods["startValidTime"] = &CalPositionRow::startValidTimeFromBin; 
		 fromBinMethods["endValidTime"] = &CalPositionRow::endValidTimeFromBin; 
		 fromBinMethods["antennaPosition"] = &CalPositionRow::antennaPositionFromBin; 
		 fromBinMethods["stationName"] = &CalPositionRow::stationNameFromBin; 
		 fromBinMethods["stationPosition"] = &CalPositionRow::stationPositionFromBin; 
		 fromBinMethods["positionMethod"] = &CalPositionRow::positionMethodFromBin; 
		 fromBinMethods["receiverBand"] = &CalPositionRow::receiverBandFromBin; 
		 fromBinMethods["numAntenna"] = &CalPositionRow::numAntennaFromBin; 
		 fromBinMethods["refAntennaNames"] = &CalPositionRow::refAntennaNamesFromBin; 
		 fromBinMethods["axesOffset"] = &CalPositionRow::axesOffsetFromBin; 
		 fromBinMethods["axesOffsetErr"] = &CalPositionRow::axesOffsetErrFromBin; 
		 fromBinMethods["axesOffsetFixed"] = &CalPositionRow::axesOffsetFixedFromBin; 
		 fromBinMethods["positionOffset"] = &CalPositionRow::positionOffsetFromBin; 
		 fromBinMethods["positionErr"] = &CalPositionRow::positionErrFromBin; 
		 fromBinMethods["reducedChiSquared"] = &CalPositionRow::reducedChiSquaredFromBin; 
			
	
		 fromBinMethods["delayRms"] = &CalPositionRow::delayRmsFromBin; 
		 fromBinMethods["phaseRms"] = &CalPositionRow::phaseRmsFromBin; 
			
	}

	
	bool CalPositionRow::compareNoAutoInc(std::string antennaName, AtmPhaseCorrectionMod::AtmPhaseCorrection atmPhaseCorrection, Tag calDataId, Tag calReductionId, ArrayTime startValidTime, ArrayTime endValidTime, std::vector<Length > antennaPosition, std::string stationName, std::vector<Length > stationPosition, PositionMethodMod::PositionMethod positionMethod, ReceiverBandMod::ReceiverBand receiverBand, int numAntenna, std::vector<std::string > refAntennaNames, Length axesOffset, Length axesOffsetErr, bool axesOffsetFixed, std::vector<Length > positionOffset, std::vector<Length > positionErr, double reducedChiSquared) {
		bool result;
		result = true;
		
	
		
		result = result && (this->antennaName == antennaName);
		
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
	

	
		
		result = result && (this->antennaPosition == antennaPosition);
		
		if (!result) return false;
	

	
		
		result = result && (this->stationName == stationName);
		
		if (!result) return false;
	

	
		
		result = result && (this->stationPosition == stationPosition);
		
		if (!result) return false;
	

	
		
		result = result && (this->positionMethod == positionMethod);
		
		if (!result) return false;
	

	
		
		result = result && (this->receiverBand == receiverBand);
		
		if (!result) return false;
	

	
		
		result = result && (this->numAntenna == numAntenna);
		
		if (!result) return false;
	

	
		
		result = result && (this->refAntennaNames == refAntennaNames);
		
		if (!result) return false;
	

	
		
		result = result && (this->axesOffset == axesOffset);
		
		if (!result) return false;
	

	
		
		result = result && (this->axesOffsetErr == axesOffsetErr);
		
		if (!result) return false;
	

	
		
		result = result && (this->axesOffsetFixed == axesOffsetFixed);
		
		if (!result) return false;
	

	
		
		result = result && (this->positionOffset == positionOffset);
		
		if (!result) return false;
	

	
		
		result = result && (this->positionErr == positionErr);
		
		if (!result) return false;
	

	
		
		result = result && (this->reducedChiSquared == reducedChiSquared);
		
		if (!result) return false;
	

		return result;
	}	
	
	
	
	bool CalPositionRow::compareRequiredValue(ArrayTime startValidTime, ArrayTime endValidTime, std::vector<Length > antennaPosition, std::string stationName, std::vector<Length > stationPosition, PositionMethodMod::PositionMethod positionMethod, ReceiverBandMod::ReceiverBand receiverBand, int numAntenna, std::vector<std::string > refAntennaNames, Length axesOffset, Length axesOffsetErr, bool axesOffsetFixed, std::vector<Length > positionOffset, std::vector<Length > positionErr, double reducedChiSquared) {
		bool result;
		result = true;
		
	
		if (!(this->startValidTime == startValidTime)) return false;
	

	
		if (!(this->endValidTime == endValidTime)) return false;
	

	
		if (!(this->antennaPosition == antennaPosition)) return false;
	

	
		if (!(this->stationName == stationName)) return false;
	

	
		if (!(this->stationPosition == stationPosition)) return false;
	

	
		if (!(this->positionMethod == positionMethod)) return false;
	

	
		if (!(this->receiverBand == receiverBand)) return false;
	

	
		if (!(this->numAntenna == numAntenna)) return false;
	

	
		if (!(this->refAntennaNames == refAntennaNames)) return false;
	

	
		if (!(this->axesOffset == axesOffset)) return false;
	

	
		if (!(this->axesOffsetErr == axesOffsetErr)) return false;
	

	
		if (!(this->axesOffsetFixed == axesOffsetFixed)) return false;
	

	
		if (!(this->positionOffset == positionOffset)) return false;
	

	
		if (!(this->positionErr == positionErr)) return false;
	

	
		if (!(this->reducedChiSquared == reducedChiSquared)) return false;
	

		return result;
	}
	
	
	/**
	 * Return true if all required attributes of the value part are equal to their homologues
	 * in x and false otherwise.
	 *

	 * @param x a pointer on the CalPositionRow whose required attributes of the value part 

	 * will be compared with those of this.
	 * @return a boolean.
	 */
	bool CalPositionRow::equalByRequiredValue(CalPositionRow*  x ) {
		
			
		if (this->startValidTime != x->startValidTime) return false;
			
		if (this->endValidTime != x->endValidTime) return false;
			
		if (this->antennaPosition != x->antennaPosition) return false;
			
		if (this->stationName != x->stationName) return false;
			
		if (this->stationPosition != x->stationPosition) return false;
			
		if (this->positionMethod != x->positionMethod) return false;
			
		if (this->receiverBand != x->receiverBand) return false;
			
		if (this->numAntenna != x->numAntenna) return false;
			
		if (this->refAntennaNames != x->refAntennaNames) return false;
			
		if (this->axesOffset != x->axesOffset) return false;
			
		if (this->axesOffsetErr != x->axesOffsetErr) return false;
			
		if (this->axesOffsetFixed != x->axesOffsetFixed) return false;
			
		if (this->positionOffset != x->positionOffset) return false;
			
		if (this->positionErr != x->positionErr) return false;
			
		if (this->reducedChiSquared != x->reducedChiSquared) return false;
			
		
		return true;
	}	
	
/*
	 map<string, CalPositionAttributeFromBin> CalPositionRow::initFromBinMethods() {
		map<string, CalPositionAttributeFromBin> result;
		
		result["antennaName"] = &CalPositionRow::antennaNameFromBin;
		result["atmPhaseCorrection"] = &CalPositionRow::atmPhaseCorrectionFromBin;
		result["calDataId"] = &CalPositionRow::calDataIdFromBin;
		result["calReductionId"] = &CalPositionRow::calReductionIdFromBin;
		result["startValidTime"] = &CalPositionRow::startValidTimeFromBin;
		result["endValidTime"] = &CalPositionRow::endValidTimeFromBin;
		result["antennaPosition"] = &CalPositionRow::antennaPositionFromBin;
		result["stationName"] = &CalPositionRow::stationNameFromBin;
		result["stationPosition"] = &CalPositionRow::stationPositionFromBin;
		result["positionMethod"] = &CalPositionRow::positionMethodFromBin;
		result["receiverBand"] = &CalPositionRow::receiverBandFromBin;
		result["numAntenna"] = &CalPositionRow::numAntennaFromBin;
		result["refAntennaNames"] = &CalPositionRow::refAntennaNamesFromBin;
		result["axesOffset"] = &CalPositionRow::axesOffsetFromBin;
		result["axesOffsetErr"] = &CalPositionRow::axesOffsetErrFromBin;
		result["axesOffsetFixed"] = &CalPositionRow::axesOffsetFixedFromBin;
		result["positionOffset"] = &CalPositionRow::positionOffsetFromBin;
		result["positionErr"] = &CalPositionRow::positionErrFromBin;
		result["reducedChiSquared"] = &CalPositionRow::reducedChiSquaredFromBin;
		
		
		result["delayRms"] = &CalPositionRow::delayRmsFromBin;
		result["phaseRms"] = &CalPositionRow::phaseRmsFromBin;
			
		
		return result;	
	}
*/	
} // End namespace asdm
 
