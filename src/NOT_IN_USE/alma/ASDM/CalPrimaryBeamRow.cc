
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
 * File CalPrimaryBeamRow.cpp
 */
 
#include <vector>
#include <set>

#include <alma/ASDM/ASDM.h>
#include <alma/ASDM/CalPrimaryBeamRow.h>
#include <alma/ASDM/CalPrimaryBeamTable.h>

#include <alma/ASDM/CalReductionTable.h>
#include <alma/ASDM/CalReductionRow.h>

#include <alma/ASDM/CalDataTable.h>
#include <alma/ASDM/CalDataRow.h>
	

using asdm::ASDM;
using asdm::CalPrimaryBeamRow;
using asdm::CalPrimaryBeamTable;

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
	CalPrimaryBeamRow::~CalPrimaryBeamRow() {
	}

	/**
	 * Return the table to which this row belongs.
	 */
	CalPrimaryBeamTable &CalPrimaryBeamRow::getTable() const {
		return table;
	}

	bool CalPrimaryBeamRow::isAdded() const {
		return hasBeenAdded;
	}	

	void CalPrimaryBeamRow::isAdded(bool added) {
		hasBeenAdded = added;
	}
	
#ifndef WITHOUT_ACS
	using asdmIDL::CalPrimaryBeamRowIDL;
#endif
	
#ifndef WITHOUT_ACS
	/**
	 * Return this row in the form of an IDL struct.
	 * @return The values of this row as a CalPrimaryBeamRowIDL struct.
	 */
	CalPrimaryBeamRowIDL *CalPrimaryBeamRow::toIDL() const {
		CalPrimaryBeamRowIDL *x = new CalPrimaryBeamRowIDL ();
		
		// Fill the IDL structure.
	
		
	
  		
		
		
			
				
		x->antennaName = CORBA::string_dup(antennaName.c_str());
				
 			
		
	

	
  		
		
		
			
				
		x->receiverBand = receiverBand;
 				
 			
		
	

	
  		
		
		
			
		x->startValidTime = startValidTime.toIDLArrayTime();
			
		
	

	
  		
		
		
			
		x->endValidTime = endValidTime.toIDLArrayTime();
			
		
	

	
  		
		
		
			
				
		x->antennaMake = antennaMake;
 				
 			
		
	

	
  		
		
		
			
				
		x->numSubband = numSubband;
 				
 			
		
	

	
  		
		
		
			
		x->frequencyRange.length(frequencyRange.size());
		for (unsigned int i = 0; i < frequencyRange.size(); i++) {
			x->frequencyRange[i].length(frequencyRange.at(i).size());			 		
		}
		
		for (unsigned int i = 0; i < frequencyRange.size() ; i++)
			for (unsigned int j = 0; j < frequencyRange.at(i).size(); j++)
					
				x->frequencyRange[i][j]= frequencyRange.at(i).at(j).toIDLFrequency();
									
		
			
		
	

	
  		
		
		
			
				
		x->numReceptor = numReceptor;
 				
 			
		
	

	
  		
		
		
			
		x->polarizationTypes.length(polarizationTypes.size());
		for (unsigned int i = 0; i < polarizationTypes.size(); ++i) {
			
				
			x->polarizationTypes[i] = polarizationTypes.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		
			
		x->mainBeamEfficiency.length(mainBeamEfficiency.size());
		for (unsigned int i = 0; i < mainBeamEfficiency.size(); ++i) {
			
				
			x->mainBeamEfficiency[i] = mainBeamEfficiency.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		
			
		x->beamDescriptionUID = beamDescriptionUID.toIDLEntityRef();
			
		
	

	
  		
		
		
			
				
		x->relativeAmplitudeRms = relativeAmplitudeRms;
 				
 			
		
	

	
  		
		
		
			
		x->direction.length(direction.size());
		for (unsigned int i = 0; i < direction.size(); ++i) {
			
			x->direction[i] = direction.at(i).toIDLAngle();
			
	 	}
			
		
	

	
  		
		
		
			
		x->minValidDirection.length(minValidDirection.size());
		for (unsigned int i = 0; i < minValidDirection.size(); ++i) {
			
			x->minValidDirection[i] = minValidDirection.at(i).toIDLAngle();
			
	 	}
			
		
	

	
  		
		
		
			
		x->maxValidDirection.length(maxValidDirection.size());
		for (unsigned int i = 0; i < maxValidDirection.size(); ++i) {
			
			x->maxValidDirection[i] = maxValidDirection.at(i).toIDLAngle();
			
	 	}
			
		
	

	
  		
		
		
			
				
		x->descriptionType = descriptionType;
 				
 			
		
	

	
  		
		
		
			
		x->imageChannelNumber.length(imageChannelNumber.size());
		for (unsigned int i = 0; i < imageChannelNumber.size(); ++i) {
			
				
			x->imageChannelNumber[i] = imageChannelNumber.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		
			
		x->imageNominalFrequency.length(imageNominalFrequency.size());
		for (unsigned int i = 0; i < imageNominalFrequency.size(); ++i) {
			
			x->imageNominalFrequency[i] = imageNominalFrequency.at(i).toIDLFrequency();
			
	 	}
			
		
	

	
	
		
	
  	
 		
		
	 	
			
		x->calDataId = calDataId.toIDLTag();
			
	 	 		
  	

	
  	
 		
		
	 	
			
		x->calReductionId = calReductionId.toIDLTag();
			
	 	 		
  	

	
		
	

	

		
		return x;
	
	}
	
	void CalPrimaryBeamRow::toIDL(asdmIDL::CalPrimaryBeamRowIDL& x) const {
		// Set the x's fields.
	
		
	
  		
		
		
			
				
		x.antennaName = CORBA::string_dup(antennaName.c_str());
				
 			
		
	

	
  		
		
		
			
				
		x.receiverBand = receiverBand;
 				
 			
		
	

	
  		
		
		
			
		x.startValidTime = startValidTime.toIDLArrayTime();
			
		
	

	
  		
		
		
			
		x.endValidTime = endValidTime.toIDLArrayTime();
			
		
	

	
  		
		
		
			
				
		x.antennaMake = antennaMake;
 				
 			
		
	

	
  		
		
		
			
				
		x.numSubband = numSubband;
 				
 			
		
	

	
  		
		
		
			
		x.frequencyRange.length(frequencyRange.size());
		for (unsigned int i = 0; i < frequencyRange.size(); i++) {
			x.frequencyRange[i].length(frequencyRange.at(i).size());			 		
		}
		
		for (unsigned int i = 0; i < frequencyRange.size() ; i++)
			for (unsigned int j = 0; j < frequencyRange.at(i).size(); j++)
					
				x.frequencyRange[i][j]= frequencyRange.at(i).at(j).toIDLFrequency();
									
		
			
		
	

	
  		
		
		
			
				
		x.numReceptor = numReceptor;
 				
 			
		
	

	
  		
		
		
			
		x.polarizationTypes.length(polarizationTypes.size());
		for (unsigned int i = 0; i < polarizationTypes.size(); ++i) {
			
				
			x.polarizationTypes[i] = polarizationTypes.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		
			
		x.mainBeamEfficiency.length(mainBeamEfficiency.size());
		for (unsigned int i = 0; i < mainBeamEfficiency.size(); ++i) {
			
				
			x.mainBeamEfficiency[i] = mainBeamEfficiency.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		
			
		x.beamDescriptionUID = beamDescriptionUID.toIDLEntityRef();
			
		
	

	
  		
		
		
			
				
		x.relativeAmplitudeRms = relativeAmplitudeRms;
 				
 			
		
	

	
  		
		
		
			
		x.direction.length(direction.size());
		for (unsigned int i = 0; i < direction.size(); ++i) {
			
			x.direction[i] = direction.at(i).toIDLAngle();
			
	 	}
			
		
	

	
  		
		
		
			
		x.minValidDirection.length(minValidDirection.size());
		for (unsigned int i = 0; i < minValidDirection.size(); ++i) {
			
			x.minValidDirection[i] = minValidDirection.at(i).toIDLAngle();
			
	 	}
			
		
	

	
  		
		
		
			
		x.maxValidDirection.length(maxValidDirection.size());
		for (unsigned int i = 0; i < maxValidDirection.size(); ++i) {
			
			x.maxValidDirection[i] = maxValidDirection.at(i).toIDLAngle();
			
	 	}
			
		
	

	
  		
		
		
			
				
		x.descriptionType = descriptionType;
 				
 			
		
	

	
  		
		
		
			
		x.imageChannelNumber.length(imageChannelNumber.size());
		for (unsigned int i = 0; i < imageChannelNumber.size(); ++i) {
			
				
			x.imageChannelNumber[i] = imageChannelNumber.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		
			
		x.imageNominalFrequency.length(imageNominalFrequency.size());
		for (unsigned int i = 0; i < imageNominalFrequency.size(); ++i) {
			
			x.imageNominalFrequency[i] = imageNominalFrequency.at(i).toIDLFrequency();
			
	 	}
			
		
	

	
	
		
	
  	
 		
		
	 	
			
		x.calDataId = calDataId.toIDLTag();
			
	 	 		
  	

	
  	
 		
		
	 	
			
		x.calReductionId = calReductionId.toIDLTag();
			
	 	 		
  	

	
		
	

	

	
	}
#endif
	

#ifndef WITHOUT_ACS
	/**
	 * Fill the values of this row from the IDL struct CalPrimaryBeamRowIDL.
	 * @param x The IDL struct containing the values used to fill this row.
	 */
	void CalPrimaryBeamRow::setFromIDL (CalPrimaryBeamRowIDL x){
		try {
		// Fill the values from x.
	
		
	
		
		
			
		setAntennaName(string (x.antennaName));
			
 		
		
	

	
		
		
			
		setReceiverBand(x.receiverBand);
  			
 		
		
	

	
		
		
			
		setStartValidTime(ArrayTime (x.startValidTime));
			
 		
		
	

	
		
		
			
		setEndValidTime(ArrayTime (x.endValidTime));
			
 		
		
	

	
		
		
			
		setAntennaMake(x.antennaMake);
  			
 		
		
	

	
		
		
			
		setNumSubband(x.numSubband);
  			
 		
		
	

	
		
		
			
		frequencyRange .clear();
        
        vector<Frequency> v_aux_frequencyRange;
        
		for (unsigned int i = 0; i < x.frequencyRange.length(); ++i) {
			v_aux_frequencyRange.clear();
			for (unsigned int j = 0; j < x.frequencyRange[0].length(); ++j) {
				
				v_aux_frequencyRange.push_back(Frequency (x.frequencyRange[i][j]));
				
  			}
  			frequencyRange.push_back(v_aux_frequencyRange);			
		}
			
  		
		
	

	
		
		
			
		setNumReceptor(x.numReceptor);
  			
 		
		
	

	
		
		
			
		polarizationTypes .clear();
		for (unsigned int i = 0; i <x.polarizationTypes.length(); ++i) {
			
			polarizationTypes.push_back(x.polarizationTypes[i]);
  			
		}
			
  		
		
	

	
		
		
			
		mainBeamEfficiency .clear();
		for (unsigned int i = 0; i <x.mainBeamEfficiency.length(); ++i) {
			
			mainBeamEfficiency.push_back(x.mainBeamEfficiency[i]);
  			
		}
			
  		
		
	

	
		
		
			
		setBeamDescriptionUID(EntityRef (x.beamDescriptionUID));
			
 		
		
	

	
		
		
			
		setRelativeAmplitudeRms(x.relativeAmplitudeRms);
  			
 		
		
	

	
		
		
			
		direction .clear();
		for (unsigned int i = 0; i <x.direction.length(); ++i) {
			
			direction.push_back(Angle (x.direction[i]));
			
		}
			
  		
		
	

	
		
		
			
		minValidDirection .clear();
		for (unsigned int i = 0; i <x.minValidDirection.length(); ++i) {
			
			minValidDirection.push_back(Angle (x.minValidDirection[i]));
			
		}
			
  		
		
	

	
		
		
			
		maxValidDirection .clear();
		for (unsigned int i = 0; i <x.maxValidDirection.length(); ++i) {
			
			maxValidDirection.push_back(Angle (x.maxValidDirection[i]));
			
		}
			
  		
		
	

	
		
		
			
		setDescriptionType(x.descriptionType);
  			
 		
		
	

	
		
		
			
		imageChannelNumber .clear();
		for (unsigned int i = 0; i <x.imageChannelNumber.length(); ++i) {
			
			imageChannelNumber.push_back(x.imageChannelNumber[i]);
  			
		}
			
  		
		
	

	
		
		
			
		imageNominalFrequency .clear();
		for (unsigned int i = 0; i <x.imageNominalFrequency.length(); ++i) {
			
			imageNominalFrequency.push_back(Frequency (x.imageNominalFrequency[i]));
			
		}
			
  		
		
	

	
	
		
	
		
		
			
		setCalDataId(Tag (x.calDataId));
			
 		
		
	

	
		
		
			
		setCalReductionId(Tag (x.calReductionId));
			
 		
		
	

	
		
	

	

		} catch (const IllegalAccessException &err) {
			throw ConversionException (err.getMessage(),"CalPrimaryBeam");
		}
	}
#endif
	
	/**
	 * Return this row in the form of an XML string.
	 * @return The values of this row as an XML string.
	 */
	string CalPrimaryBeamRow::toXML() const {
		string buf;
		buf.append("<row> \n");
		
	
		
  	
 		
		
		Parser::toXML(antennaName, "antennaName", buf);
		
		
	

  	
 		
		
			buf.append(EnumerationParser::toXML("receiverBand", receiverBand));
		
		
	

  	
 		
		
		Parser::toXML(startValidTime, "startValidTime", buf);
		
		
	

  	
 		
		
		Parser::toXML(endValidTime, "endValidTime", buf);
		
		
	

  	
 		
		
			buf.append(EnumerationParser::toXML("antennaMake", antennaMake));
		
		
	

  	
 		
		
		Parser::toXML(numSubband, "numSubband", buf);
		
		
	

  	
 		
		
		Parser::toXML(frequencyRange, "frequencyRange", buf);
		
		
	

  	
 		
		
		Parser::toXML(numReceptor, "numReceptor", buf);
		
		
	

  	
 		
		
			buf.append(EnumerationParser::toXML("polarizationTypes", polarizationTypes));
		
		
	

  	
 		
		
		Parser::toXML(mainBeamEfficiency, "mainBeamEfficiency", buf);
		
		
	

  	
 		
		
		Parser::toXML(beamDescriptionUID, "beamDescriptionUID", buf);
		
		
	

  	
 		
		
		Parser::toXML(relativeAmplitudeRms, "relativeAmplitudeRms", buf);
		
		
	

  	
 		
		
		Parser::toXML(direction, "direction", buf);
		
		
	

  	
 		
		
		Parser::toXML(minValidDirection, "minValidDirection", buf);
		
		
	

  	
 		
		
		Parser::toXML(maxValidDirection, "maxValidDirection", buf);
		
		
	

  	
 		
		
			buf.append(EnumerationParser::toXML("descriptionType", descriptionType));
		
		
	

  	
 		
		
		Parser::toXML(imageChannelNumber, "imageChannelNumber", buf);
		
		
	

  	
 		
		
		Parser::toXML(imageNominalFrequency, "imageNominalFrequency", buf);
		
		
	

	
	
		
  	
 		
		
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
	void CalPrimaryBeamRow::setFromXML (string rowDoc) {
		Parser row(rowDoc);
		string s = "";
		try {
	
		
	
  		
			
	  	setAntennaName(Parser::getString("antennaName","CalPrimaryBeam",rowDoc));
			
		
	

	
		
		
		
		receiverBand = EnumerationParser::getReceiverBand("receiverBand","CalPrimaryBeam",rowDoc);
		
		
		
	

	
  		
			
	  	setStartValidTime(Parser::getArrayTime("startValidTime","CalPrimaryBeam",rowDoc));
			
		
	

	
  		
			
	  	setEndValidTime(Parser::getArrayTime("endValidTime","CalPrimaryBeam",rowDoc));
			
		
	

	
		
		
		
		antennaMake = EnumerationParser::getAntennaMake("antennaMake","CalPrimaryBeam",rowDoc);
		
		
		
	

	
  		
			
	  	setNumSubband(Parser::getInteger("numSubband","CalPrimaryBeam",rowDoc));
			
		
	

	
  		
			
					
	  	setFrequencyRange(Parser::get2DFrequency("frequencyRange","CalPrimaryBeam",rowDoc));
	  			
	  		
		
	

	
  		
			
	  	setNumReceptor(Parser::getInteger("numReceptor","CalPrimaryBeam",rowDoc));
			
		
	

	
		
		
		
		polarizationTypes = EnumerationParser::getPolarizationType1D("polarizationTypes","CalPrimaryBeam",rowDoc);			
		
		
		
	

	
  		
			
					
	  	setMainBeamEfficiency(Parser::get1DDouble("mainBeamEfficiency","CalPrimaryBeam",rowDoc));
	  			
	  		
		
	

	
  		
			
	  	setBeamDescriptionUID(Parser::getEntityRef("beamDescriptionUID","CalPrimaryBeam",rowDoc));
			
		
	

	
  		
			
	  	setRelativeAmplitudeRms(Parser::getFloat("relativeAmplitudeRms","CalPrimaryBeam",rowDoc));
			
		
	

	
  		
			
					
	  	setDirection(Parser::get1DAngle("direction","CalPrimaryBeam",rowDoc));
	  			
	  		
		
	

	
  		
			
					
	  	setMinValidDirection(Parser::get1DAngle("minValidDirection","CalPrimaryBeam",rowDoc));
	  			
	  		
		
	

	
  		
			
					
	  	setMaxValidDirection(Parser::get1DAngle("maxValidDirection","CalPrimaryBeam",rowDoc));
	  			
	  		
		
	

	
		
		
		
		descriptionType = EnumerationParser::getPrimaryBeamDescription("descriptionType","CalPrimaryBeam",rowDoc);
		
		
		
	

	
  		
			
					
	  	setImageChannelNumber(Parser::get1DInteger("imageChannelNumber","CalPrimaryBeam",rowDoc));
	  			
	  		
		
	

	
  		
			
					
	  	setImageNominalFrequency(Parser::get1DFrequency("imageNominalFrequency","CalPrimaryBeam",rowDoc));
	  			
	  		
		
	

	
	
		
	
  		
			
	  	setCalDataId(Parser::getTag("calDataId","CalData",rowDoc));
			
		
	

	
  		
			
	  	setCalReductionId(Parser::getTag("calReductionId","CalReduction",rowDoc));
			
		
	

	
		
	

	

		} catch (const IllegalAccessException &err) {
			throw ConversionException (err.getMessage(),"CalPrimaryBeam");
		}
	}
	
	void CalPrimaryBeamRow::toBin(EndianOSStream& eoss) {
	
	
	
	
		
						
			eoss.writeString(antennaName);
				
		
	

	
	
		
					
		eoss.writeString(CReceiverBand::name(receiverBand));
			/* eoss.writeInt(receiverBand); */
				
		
	

	
	
		
	calDataId.toBin(eoss);
		
	

	
	
		
	calReductionId.toBin(eoss);
		
	

	
	
		
	startValidTime.toBin(eoss);
		
	

	
	
		
	endValidTime.toBin(eoss);
		
	

	
	
		
					
		eoss.writeString(CAntennaMake::name(antennaMake));
			/* eoss.writeInt(antennaMake); */
				
		
	

	
	
		
						
			eoss.writeInt(numSubband);
				
		
	

	
	
		
	Frequency::toBin(frequencyRange, eoss);
		
	

	
	
		
						
			eoss.writeInt(numReceptor);
				
		
	

	
	
		
		
			
		eoss.writeInt((int) polarizationTypes.size());
		for (unsigned int i = 0; i < polarizationTypes.size(); i++)
				
			eoss.writeString(CPolarizationType::name(polarizationTypes.at(i)));
			/* eoss.writeInt(polarizationTypes.at(i)); */
				
				
						
		
	

	
	
		
		
			
		eoss.writeInt((int) mainBeamEfficiency.size());
		for (unsigned int i = 0; i < mainBeamEfficiency.size(); i++)
				
			eoss.writeDouble(mainBeamEfficiency.at(i));
				
				
						
		
	

	
	
		
	beamDescriptionUID.toBin(eoss);
		
	

	
	
		
						
			eoss.writeFloat(relativeAmplitudeRms);
				
		
	

	
	
		
	Angle::toBin(direction, eoss);
		
	

	
	
		
	Angle::toBin(minValidDirection, eoss);
		
	

	
	
		
	Angle::toBin(maxValidDirection, eoss);
		
	

	
	
		
					
		eoss.writeString(CPrimaryBeamDescription::name(descriptionType));
			/* eoss.writeInt(descriptionType); */
				
		
	

	
	
		
		
			
		eoss.writeInt((int) imageChannelNumber.size());
		for (unsigned int i = 0; i < imageChannelNumber.size(); i++)
				
			eoss.writeInt(imageChannelNumber.at(i));
				
				
						
		
	

	
	
		
	Frequency::toBin(imageNominalFrequency, eoss);
		
	


	
	
	}
	
void CalPrimaryBeamRow::antennaNameFromBin(EndianIStream& eis) {
		
	
	
		
			
		antennaName =  eis.readString();
			
		
	
	
}
void CalPrimaryBeamRow::receiverBandFromBin(EndianIStream& eis) {
		
	
	
		
			
		receiverBand = CReceiverBand::literal(eis.readString());
			
		
	
	
}
void CalPrimaryBeamRow::calDataIdFromBin(EndianIStream& eis) {
		
	
		
		
		calDataId =  Tag::fromBin(eis);
		
	
	
}
void CalPrimaryBeamRow::calReductionIdFromBin(EndianIStream& eis) {
		
	
		
		
		calReductionId =  Tag::fromBin(eis);
		
	
	
}
void CalPrimaryBeamRow::startValidTimeFromBin(EndianIStream& eis) {
		
	
		
		
		startValidTime =  ArrayTime::fromBin(eis);
		
	
	
}
void CalPrimaryBeamRow::endValidTimeFromBin(EndianIStream& eis) {
		
	
		
		
		endValidTime =  ArrayTime::fromBin(eis);
		
	
	
}
void CalPrimaryBeamRow::antennaMakeFromBin(EndianIStream& eis) {
		
	
	
		
			
		antennaMake = CAntennaMake::literal(eis.readString());
			
		
	
	
}
void CalPrimaryBeamRow::numSubbandFromBin(EndianIStream& eis) {
		
	
	
		
			
		numSubband =  eis.readInt();
			
		
	
	
}
void CalPrimaryBeamRow::frequencyRangeFromBin(EndianIStream& eis) {
		
	
		
		
			
	
	frequencyRange = Frequency::from2DBin(eis);		
	

		
	
	
}
void CalPrimaryBeamRow::numReceptorFromBin(EndianIStream& eis) {
		
	
	
		
			
		numReceptor =  eis.readInt();
			
		
	
	
}
void CalPrimaryBeamRow::polarizationTypesFromBin(EndianIStream& eis) {
		
	
	
		
			
	
		polarizationTypes.clear();
		
		unsigned int polarizationTypesDim1 = eis.readInt();
		for (unsigned int  i = 0 ; i < polarizationTypesDim1; i++)
			
			polarizationTypes.push_back(CPolarizationType::literal(eis.readString()));
			
	

		
	
	
}
void CalPrimaryBeamRow::mainBeamEfficiencyFromBin(EndianIStream& eis) {
		
	
	
		
			
	
		mainBeamEfficiency.clear();
		
		unsigned int mainBeamEfficiencyDim1 = eis.readInt();
		for (unsigned int  i = 0 ; i < mainBeamEfficiencyDim1; i++)
			
			mainBeamEfficiency.push_back(eis.readDouble());
			
	

		
	
	
}
void CalPrimaryBeamRow::beamDescriptionUIDFromBin(EndianIStream& eis) {
		
	
		
		
		beamDescriptionUID =  EntityRef::fromBin(eis);
		
	
	
}
void CalPrimaryBeamRow::relativeAmplitudeRmsFromBin(EndianIStream& eis) {
		
	
	
		
			
		relativeAmplitudeRms =  eis.readFloat();
			
		
	
	
}
void CalPrimaryBeamRow::directionFromBin(EndianIStream& eis) {
		
	
		
		
			
	
	direction = Angle::from1DBin(eis);	
	

		
	
	
}
void CalPrimaryBeamRow::minValidDirectionFromBin(EndianIStream& eis) {
		
	
		
		
			
	
	minValidDirection = Angle::from1DBin(eis);	
	

		
	
	
}
void CalPrimaryBeamRow::maxValidDirectionFromBin(EndianIStream& eis) {
		
	
		
		
			
	
	maxValidDirection = Angle::from1DBin(eis);	
	

		
	
	
}
void CalPrimaryBeamRow::descriptionTypeFromBin(EndianIStream& eis) {
		
	
	
		
			
		descriptionType = CPrimaryBeamDescription::literal(eis.readString());
			
		
	
	
}
void CalPrimaryBeamRow::imageChannelNumberFromBin(EndianIStream& eis) {
		
	
	
		
			
	
		imageChannelNumber.clear();
		
		unsigned int imageChannelNumberDim1 = eis.readInt();
		for (unsigned int  i = 0 ; i < imageChannelNumberDim1; i++)
			
			imageChannelNumber.push_back(eis.readInt());
			
	

		
	
	
}
void CalPrimaryBeamRow::imageNominalFrequencyFromBin(EndianIStream& eis) {
		
	
		
		
			
	
	imageNominalFrequency = Frequency::from1DBin(eis);	
	

		
	
	
}

		
	
	CalPrimaryBeamRow* CalPrimaryBeamRow::fromBin(EndianIStream& eis, CalPrimaryBeamTable& table, const vector<string>& attributesSeq) {
		CalPrimaryBeamRow* row = new  CalPrimaryBeamRow(table);
		
		map<string, CalPrimaryBeamAttributeFromBin>::iterator iter ;
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
					throw ConversionException("There is not method to read an attribute '"+attributesSeq.at(i)+"'.", "CalPrimaryBeamTable");
			}
				
		}				
		return row;
	}

	//
	// A collection of methods to set the value of the attributes from their textual value in the XML representation
	// of one row.
	//
	
	// Convert a string into an String 
	void CalPrimaryBeamRow::antennaNameFromText(const string & s) {
		 
          
		antennaName = ASDMValuesParser::parse<string>(s);
          
		
	}
	
	
	// Convert a string into an ReceiverBand 
	void CalPrimaryBeamRow::receiverBandFromText(const string & s) {
		 
          
		receiverBand = ASDMValuesParser::parse<ReceiverBandMod::ReceiverBand>(s);
          
		
	}
	
	
	// Convert a string into an Tag 
	void CalPrimaryBeamRow::calDataIdFromText(const string & s) {
		 
          
		calDataId = ASDMValuesParser::parse<Tag>(s);
          
		
	}
	
	
	// Convert a string into an Tag 
	void CalPrimaryBeamRow::calReductionIdFromText(const string & s) {
		 
          
		calReductionId = ASDMValuesParser::parse<Tag>(s);
          
		
	}
	
	
	// Convert a string into an ArrayTime 
	void CalPrimaryBeamRow::startValidTimeFromText(const string & s) {
		 
          
		startValidTime = ASDMValuesParser::parse<ArrayTime>(s);
          
		
	}
	
	
	// Convert a string into an ArrayTime 
	void CalPrimaryBeamRow::endValidTimeFromText(const string & s) {
		 
          
		endValidTime = ASDMValuesParser::parse<ArrayTime>(s);
          
		
	}
	
	
	// Convert a string into an AntennaMake 
	void CalPrimaryBeamRow::antennaMakeFromText(const string & s) {
		 
          
		antennaMake = ASDMValuesParser::parse<AntennaMakeMod::AntennaMake>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void CalPrimaryBeamRow::numSubbandFromText(const string & s) {
		 
          
		numSubband = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	// Convert a string into an Frequency 
	void CalPrimaryBeamRow::frequencyRangeFromText(const string & s) {
		 
          
		frequencyRange = ASDMValuesParser::parse2D<Frequency>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void CalPrimaryBeamRow::numReceptorFromText(const string & s) {
		 
          
		numReceptor = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	// Convert a string into an PolarizationType 
	void CalPrimaryBeamRow::polarizationTypesFromText(const string & s) {
		 
          
		polarizationTypes = ASDMValuesParser::parse1D<PolarizationTypeMod::PolarizationType>(s);
          
		
	}
	
	
	// Convert a string into an double 
	void CalPrimaryBeamRow::mainBeamEfficiencyFromText(const string & s) {
		 
          
		mainBeamEfficiency = ASDMValuesParser::parse1D<double>(s);
          
		
	}
	
	
	
	// Convert a string into an float 
	void CalPrimaryBeamRow::relativeAmplitudeRmsFromText(const string & s) {
		 
          
		relativeAmplitudeRms = ASDMValuesParser::parse<float>(s);
          
		
	}
	
	
	// Convert a string into an Angle 
	void CalPrimaryBeamRow::directionFromText(const string & s) {
		 
          
		direction = ASDMValuesParser::parse1D<Angle>(s);
          
		
	}
	
	
	// Convert a string into an Angle 
	void CalPrimaryBeamRow::minValidDirectionFromText(const string & s) {
		 
          
		minValidDirection = ASDMValuesParser::parse1D<Angle>(s);
          
		
	}
	
	
	// Convert a string into an Angle 
	void CalPrimaryBeamRow::maxValidDirectionFromText(const string & s) {
		 
          
		maxValidDirection = ASDMValuesParser::parse1D<Angle>(s);
          
		
	}
	
	
	// Convert a string into an PrimaryBeamDescription 
	void CalPrimaryBeamRow::descriptionTypeFromText(const string & s) {
		 
          
		descriptionType = ASDMValuesParser::parse<PrimaryBeamDescriptionMod::PrimaryBeamDescription>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void CalPrimaryBeamRow::imageChannelNumberFromText(const string & s) {
		 
          
		imageChannelNumber = ASDMValuesParser::parse1D<int>(s);
          
		
	}
	
	
	// Convert a string into an Frequency 
	void CalPrimaryBeamRow::imageNominalFrequencyFromText(const string & s) {
		 
          
		imageNominalFrequency = ASDMValuesParser::parse1D<Frequency>(s);
          
		
	}
	

		
	
	void CalPrimaryBeamRow::fromText(const std::string& attributeName, const std::string&  t) {
		map<string, CalPrimaryBeamAttributeFromText>::iterator iter;
		if ((iter = fromTextMethods.find(attributeName)) == fromTextMethods.end())
			throw ConversionException("I do not know what to do with '"+attributeName+"' and its content '"+t+"' (while parsing an XML document)", "CalPrimaryBeamTable");
		(this->*(iter->second))(t);
	}
			
	////////////////////////////////////////////////
	// Intrinsic Table Attributes getters/setters //
	////////////////////////////////////////////////
	
	

	
 	/**
 	 * Get antennaName.
 	 * @return antennaName as std::string
 	 */
 	std::string CalPrimaryBeamRow::getAntennaName() const {
	
  		return antennaName;
 	}

 	/**
 	 * Set antennaName with the specified std::string.
 	 * @param antennaName The std::string value to which antennaName is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void CalPrimaryBeamRow::setAntennaName (std::string antennaName)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("antennaName", "CalPrimaryBeam");
		
  		}
  	
 		this->antennaName = antennaName;
	
 	}
	
	

	

	
 	/**
 	 * Get receiverBand.
 	 * @return receiverBand as ReceiverBandMod::ReceiverBand
 	 */
 	ReceiverBandMod::ReceiverBand CalPrimaryBeamRow::getReceiverBand() const {
	
  		return receiverBand;
 	}

 	/**
 	 * Set receiverBand with the specified ReceiverBandMod::ReceiverBand.
 	 * @param receiverBand The ReceiverBandMod::ReceiverBand value to which receiverBand is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void CalPrimaryBeamRow::setReceiverBand (ReceiverBandMod::ReceiverBand receiverBand)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("receiverBand", "CalPrimaryBeam");
		
  		}
  	
 		this->receiverBand = receiverBand;
	
 	}
	
	

	

	
 	/**
 	 * Get startValidTime.
 	 * @return startValidTime as ArrayTime
 	 */
 	ArrayTime CalPrimaryBeamRow::getStartValidTime() const {
	
  		return startValidTime;
 	}

 	/**
 	 * Set startValidTime with the specified ArrayTime.
 	 * @param startValidTime The ArrayTime value to which startValidTime is to be set.
 	 
 	
 		
 	 */
 	void CalPrimaryBeamRow::setStartValidTime (ArrayTime startValidTime)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->startValidTime = startValidTime;
	
 	}
	
	

	

	
 	/**
 	 * Get endValidTime.
 	 * @return endValidTime as ArrayTime
 	 */
 	ArrayTime CalPrimaryBeamRow::getEndValidTime() const {
	
  		return endValidTime;
 	}

 	/**
 	 * Set endValidTime with the specified ArrayTime.
 	 * @param endValidTime The ArrayTime value to which endValidTime is to be set.
 	 
 	
 		
 	 */
 	void CalPrimaryBeamRow::setEndValidTime (ArrayTime endValidTime)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->endValidTime = endValidTime;
	
 	}
	
	

	

	
 	/**
 	 * Get antennaMake.
 	 * @return antennaMake as AntennaMakeMod::AntennaMake
 	 */
 	AntennaMakeMod::AntennaMake CalPrimaryBeamRow::getAntennaMake() const {
	
  		return antennaMake;
 	}

 	/**
 	 * Set antennaMake with the specified AntennaMakeMod::AntennaMake.
 	 * @param antennaMake The AntennaMakeMod::AntennaMake value to which antennaMake is to be set.
 	 
 	
 		
 	 */
 	void CalPrimaryBeamRow::setAntennaMake (AntennaMakeMod::AntennaMake antennaMake)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->antennaMake = antennaMake;
	
 	}
	
	

	

	
 	/**
 	 * Get numSubband.
 	 * @return numSubband as int
 	 */
 	int CalPrimaryBeamRow::getNumSubband() const {
	
  		return numSubband;
 	}

 	/**
 	 * Set numSubband with the specified int.
 	 * @param numSubband The int value to which numSubband is to be set.
 	 
 	
 		
 	 */
 	void CalPrimaryBeamRow::setNumSubband (int numSubband)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->numSubband = numSubband;
	
 	}
	
	

	

	
 	/**
 	 * Get frequencyRange.
 	 * @return frequencyRange as std::vector<std::vector<Frequency > >
 	 */
 	std::vector<std::vector<Frequency > > CalPrimaryBeamRow::getFrequencyRange() const {
	
  		return frequencyRange;
 	}

 	/**
 	 * Set frequencyRange with the specified std::vector<std::vector<Frequency > >.
 	 * @param frequencyRange The std::vector<std::vector<Frequency > > value to which frequencyRange is to be set.
 	 
 	
 		
 	 */
 	void CalPrimaryBeamRow::setFrequencyRange (std::vector<std::vector<Frequency > > frequencyRange)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->frequencyRange = frequencyRange;
	
 	}
	
	

	

	
 	/**
 	 * Get numReceptor.
 	 * @return numReceptor as int
 	 */
 	int CalPrimaryBeamRow::getNumReceptor() const {
	
  		return numReceptor;
 	}

 	/**
 	 * Set numReceptor with the specified int.
 	 * @param numReceptor The int value to which numReceptor is to be set.
 	 
 	
 		
 	 */
 	void CalPrimaryBeamRow::setNumReceptor (int numReceptor)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->numReceptor = numReceptor;
	
 	}
	
	

	

	
 	/**
 	 * Get polarizationTypes.
 	 * @return polarizationTypes as std::vector<PolarizationTypeMod::PolarizationType >
 	 */
 	std::vector<PolarizationTypeMod::PolarizationType > CalPrimaryBeamRow::getPolarizationTypes() const {
	
  		return polarizationTypes;
 	}

 	/**
 	 * Set polarizationTypes with the specified std::vector<PolarizationTypeMod::PolarizationType >.
 	 * @param polarizationTypes The std::vector<PolarizationTypeMod::PolarizationType > value to which polarizationTypes is to be set.
 	 
 	
 		
 	 */
 	void CalPrimaryBeamRow::setPolarizationTypes (std::vector<PolarizationTypeMod::PolarizationType > polarizationTypes)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->polarizationTypes = polarizationTypes;
	
 	}
	
	

	

	
 	/**
 	 * Get mainBeamEfficiency.
 	 * @return mainBeamEfficiency as std::vector<double >
 	 */
 	std::vector<double > CalPrimaryBeamRow::getMainBeamEfficiency() const {
	
  		return mainBeamEfficiency;
 	}

 	/**
 	 * Set mainBeamEfficiency with the specified std::vector<double >.
 	 * @param mainBeamEfficiency The std::vector<double > value to which mainBeamEfficiency is to be set.
 	 
 	
 		
 	 */
 	void CalPrimaryBeamRow::setMainBeamEfficiency (std::vector<double > mainBeamEfficiency)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->mainBeamEfficiency = mainBeamEfficiency;
	
 	}
	
	

	

	
 	/**
 	 * Get beamDescriptionUID.
 	 * @return beamDescriptionUID as EntityRef
 	 */
 	EntityRef CalPrimaryBeamRow::getBeamDescriptionUID() const {
	
  		return beamDescriptionUID;
 	}

 	/**
 	 * Set beamDescriptionUID with the specified EntityRef.
 	 * @param beamDescriptionUID The EntityRef value to which beamDescriptionUID is to be set.
 	 
 	
 		
 	 */
 	void CalPrimaryBeamRow::setBeamDescriptionUID (EntityRef beamDescriptionUID)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->beamDescriptionUID = beamDescriptionUID;
	
 	}
	
	

	

	
 	/**
 	 * Get relativeAmplitudeRms.
 	 * @return relativeAmplitudeRms as float
 	 */
 	float CalPrimaryBeamRow::getRelativeAmplitudeRms() const {
	
  		return relativeAmplitudeRms;
 	}

 	/**
 	 * Set relativeAmplitudeRms with the specified float.
 	 * @param relativeAmplitudeRms The float value to which relativeAmplitudeRms is to be set.
 	 
 	
 		
 	 */
 	void CalPrimaryBeamRow::setRelativeAmplitudeRms (float relativeAmplitudeRms)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->relativeAmplitudeRms = relativeAmplitudeRms;
	
 	}
	
	

	

	
 	/**
 	 * Get direction.
 	 * @return direction as std::vector<Angle >
 	 */
 	std::vector<Angle > CalPrimaryBeamRow::getDirection() const {
	
  		return direction;
 	}

 	/**
 	 * Set direction with the specified std::vector<Angle >.
 	 * @param direction The std::vector<Angle > value to which direction is to be set.
 	 
 	
 		
 	 */
 	void CalPrimaryBeamRow::setDirection (std::vector<Angle > direction)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->direction = direction;
	
 	}
	
	

	

	
 	/**
 	 * Get minValidDirection.
 	 * @return minValidDirection as std::vector<Angle >
 	 */
 	std::vector<Angle > CalPrimaryBeamRow::getMinValidDirection() const {
	
  		return minValidDirection;
 	}

 	/**
 	 * Set minValidDirection with the specified std::vector<Angle >.
 	 * @param minValidDirection The std::vector<Angle > value to which minValidDirection is to be set.
 	 
 	
 		
 	 */
 	void CalPrimaryBeamRow::setMinValidDirection (std::vector<Angle > minValidDirection)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->minValidDirection = minValidDirection;
	
 	}
	
	

	

	
 	/**
 	 * Get maxValidDirection.
 	 * @return maxValidDirection as std::vector<Angle >
 	 */
 	std::vector<Angle > CalPrimaryBeamRow::getMaxValidDirection() const {
	
  		return maxValidDirection;
 	}

 	/**
 	 * Set maxValidDirection with the specified std::vector<Angle >.
 	 * @param maxValidDirection The std::vector<Angle > value to which maxValidDirection is to be set.
 	 
 	
 		
 	 */
 	void CalPrimaryBeamRow::setMaxValidDirection (std::vector<Angle > maxValidDirection)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->maxValidDirection = maxValidDirection;
	
 	}
	
	

	

	
 	/**
 	 * Get descriptionType.
 	 * @return descriptionType as PrimaryBeamDescriptionMod::PrimaryBeamDescription
 	 */
 	PrimaryBeamDescriptionMod::PrimaryBeamDescription CalPrimaryBeamRow::getDescriptionType() const {
	
  		return descriptionType;
 	}

 	/**
 	 * Set descriptionType with the specified PrimaryBeamDescriptionMod::PrimaryBeamDescription.
 	 * @param descriptionType The PrimaryBeamDescriptionMod::PrimaryBeamDescription value to which descriptionType is to be set.
 	 
 	
 		
 	 */
 	void CalPrimaryBeamRow::setDescriptionType (PrimaryBeamDescriptionMod::PrimaryBeamDescription descriptionType)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->descriptionType = descriptionType;
	
 	}
	
	

	

	
 	/**
 	 * Get imageChannelNumber.
 	 * @return imageChannelNumber as std::vector<int >
 	 */
 	std::vector<int > CalPrimaryBeamRow::getImageChannelNumber() const {
	
  		return imageChannelNumber;
 	}

 	/**
 	 * Set imageChannelNumber with the specified std::vector<int >.
 	 * @param imageChannelNumber The std::vector<int > value to which imageChannelNumber is to be set.
 	 
 	
 		
 	 */
 	void CalPrimaryBeamRow::setImageChannelNumber (std::vector<int > imageChannelNumber)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->imageChannelNumber = imageChannelNumber;
	
 	}
	
	

	

	
 	/**
 	 * Get imageNominalFrequency.
 	 * @return imageNominalFrequency as std::vector<Frequency >
 	 */
 	std::vector<Frequency > CalPrimaryBeamRow::getImageNominalFrequency() const {
	
  		return imageNominalFrequency;
 	}

 	/**
 	 * Set imageNominalFrequency with the specified std::vector<Frequency >.
 	 * @param imageNominalFrequency The std::vector<Frequency > value to which imageNominalFrequency is to be set.
 	 
 	
 		
 	 */
 	void CalPrimaryBeamRow::setImageNominalFrequency (std::vector<Frequency > imageNominalFrequency)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->imageNominalFrequency = imageNominalFrequency;
	
 	}
	
	

	
	///////////////////////////////////////////////
	// Extrinsic Table Attributes getters/setters//
	///////////////////////////////////////////////
	
	

	
 	/**
 	 * Get calDataId.
 	 * @return calDataId as Tag
 	 */
 	Tag CalPrimaryBeamRow::getCalDataId() const {
	
  		return calDataId;
 	}

 	/**
 	 * Set calDataId with the specified Tag.
 	 * @param calDataId The Tag value to which calDataId is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void CalPrimaryBeamRow::setCalDataId (Tag calDataId)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("calDataId", "CalPrimaryBeam");
		
  		}
  	
 		this->calDataId = calDataId;
	
 	}
	
	

	

	
 	/**
 	 * Get calReductionId.
 	 * @return calReductionId as Tag
 	 */
 	Tag CalPrimaryBeamRow::getCalReductionId() const {
	
  		return calReductionId;
 	}

 	/**
 	 * Set calReductionId with the specified Tag.
 	 * @param calReductionId The Tag value to which calReductionId is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void CalPrimaryBeamRow::setCalReductionId (Tag calReductionId)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("calReductionId", "CalPrimaryBeam");
		
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
	 CalReductionRow* CalPrimaryBeamRow::getCalReductionUsingCalReductionId() {
	 
	 	return table.getContainer().getCalReduction().getRowByKey(calReductionId);
	 }
	 

	

	
	
	
		

	/**
	 * Returns the pointer to the row in the CalData table having CalData.calDataId == calDataId
	 * @return a CalDataRow*
	 * 
	 
	 */
	 CalDataRow* CalPrimaryBeamRow::getCalDataUsingCalDataId() {
	 
	 	return table.getContainer().getCalData().getRowByKey(calDataId);
	 }
	 

	

	
	/**
	 * Create a CalPrimaryBeamRow.
	 * <p>
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 */ 
	CalPrimaryBeamRow::CalPrimaryBeamRow (CalPrimaryBeamTable &t) : table(t) {
		hasBeenAdded = false;
		
	
	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	
	

	

	
	
	
	

	
// This attribute is scalar and has an enumeration type. Let's initialize it to some valid value (the 1st of the enumeration).		
receiverBand = CReceiverBand::from_int(0);
	

	

	

	
// This attribute is scalar and has an enumeration type. Let's initialize it to some valid value (the 1st of the enumeration).		
antennaMake = CAntennaMake::from_int(0);
	

	

	

	

	

	

	

	

	

	

	

	
// This attribute is scalar and has an enumeration type. Let's initialize it to some valid value (the 1st of the enumeration).		
descriptionType = CPrimaryBeamDescription::from_int(0);
	

	

	

	
	
	 fromBinMethods["antennaName"] = &CalPrimaryBeamRow::antennaNameFromBin; 
	 fromBinMethods["receiverBand"] = &CalPrimaryBeamRow::receiverBandFromBin; 
	 fromBinMethods["calDataId"] = &CalPrimaryBeamRow::calDataIdFromBin; 
	 fromBinMethods["calReductionId"] = &CalPrimaryBeamRow::calReductionIdFromBin; 
	 fromBinMethods["startValidTime"] = &CalPrimaryBeamRow::startValidTimeFromBin; 
	 fromBinMethods["endValidTime"] = &CalPrimaryBeamRow::endValidTimeFromBin; 
	 fromBinMethods["antennaMake"] = &CalPrimaryBeamRow::antennaMakeFromBin; 
	 fromBinMethods["numSubband"] = &CalPrimaryBeamRow::numSubbandFromBin; 
	 fromBinMethods["frequencyRange"] = &CalPrimaryBeamRow::frequencyRangeFromBin; 
	 fromBinMethods["numReceptor"] = &CalPrimaryBeamRow::numReceptorFromBin; 
	 fromBinMethods["polarizationTypes"] = &CalPrimaryBeamRow::polarizationTypesFromBin; 
	 fromBinMethods["mainBeamEfficiency"] = &CalPrimaryBeamRow::mainBeamEfficiencyFromBin; 
	 fromBinMethods["beamDescriptionUID"] = &CalPrimaryBeamRow::beamDescriptionUIDFromBin; 
	 fromBinMethods["relativeAmplitudeRms"] = &CalPrimaryBeamRow::relativeAmplitudeRmsFromBin; 
	 fromBinMethods["direction"] = &CalPrimaryBeamRow::directionFromBin; 
	 fromBinMethods["minValidDirection"] = &CalPrimaryBeamRow::minValidDirectionFromBin; 
	 fromBinMethods["maxValidDirection"] = &CalPrimaryBeamRow::maxValidDirectionFromBin; 
	 fromBinMethods["descriptionType"] = &CalPrimaryBeamRow::descriptionTypeFromBin; 
	 fromBinMethods["imageChannelNumber"] = &CalPrimaryBeamRow::imageChannelNumberFromBin; 
	 fromBinMethods["imageNominalFrequency"] = &CalPrimaryBeamRow::imageNominalFrequencyFromBin; 
		
	
	
	
	
	
				 
	fromTextMethods["antennaName"] = &CalPrimaryBeamRow::antennaNameFromText;
		 
	
				 
	fromTextMethods["receiverBand"] = &CalPrimaryBeamRow::receiverBandFromText;
		 
	
				 
	fromTextMethods["calDataId"] = &CalPrimaryBeamRow::calDataIdFromText;
		 
	
				 
	fromTextMethods["calReductionId"] = &CalPrimaryBeamRow::calReductionIdFromText;
		 
	
				 
	fromTextMethods["startValidTime"] = &CalPrimaryBeamRow::startValidTimeFromText;
		 
	
				 
	fromTextMethods["endValidTime"] = &CalPrimaryBeamRow::endValidTimeFromText;
		 
	
				 
	fromTextMethods["antennaMake"] = &CalPrimaryBeamRow::antennaMakeFromText;
		 
	
				 
	fromTextMethods["numSubband"] = &CalPrimaryBeamRow::numSubbandFromText;
		 
	
				 
	fromTextMethods["frequencyRange"] = &CalPrimaryBeamRow::frequencyRangeFromText;
		 
	
				 
	fromTextMethods["numReceptor"] = &CalPrimaryBeamRow::numReceptorFromText;
		 
	
				 
	fromTextMethods["polarizationTypes"] = &CalPrimaryBeamRow::polarizationTypesFromText;
		 
	
				 
	fromTextMethods["mainBeamEfficiency"] = &CalPrimaryBeamRow::mainBeamEfficiencyFromText;
		 
	
		 
	
				 
	fromTextMethods["relativeAmplitudeRms"] = &CalPrimaryBeamRow::relativeAmplitudeRmsFromText;
		 
	
				 
	fromTextMethods["direction"] = &CalPrimaryBeamRow::directionFromText;
		 
	
				 
	fromTextMethods["minValidDirection"] = &CalPrimaryBeamRow::minValidDirectionFromText;
		 
	
				 
	fromTextMethods["maxValidDirection"] = &CalPrimaryBeamRow::maxValidDirectionFromText;
		 
	
				 
	fromTextMethods["descriptionType"] = &CalPrimaryBeamRow::descriptionTypeFromText;
		 
	
				 
	fromTextMethods["imageChannelNumber"] = &CalPrimaryBeamRow::imageChannelNumberFromText;
		 
	
				 
	fromTextMethods["imageNominalFrequency"] = &CalPrimaryBeamRow::imageNominalFrequencyFromText;
		 
	

		
	}
	
	CalPrimaryBeamRow::CalPrimaryBeamRow (CalPrimaryBeamTable &t, CalPrimaryBeamRow *row) : table(t) {
		hasBeenAdded = false;
		
		if (row == 0) {
	
	
	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	
	

	
		
		}
		else {
	
		
			antennaName = row->antennaName;
		
			receiverBand = row->receiverBand;
		
			calDataId = row->calDataId;
		
			calReductionId = row->calReductionId;
		
		
		
		
			startValidTime = row->startValidTime;
		
			endValidTime = row->endValidTime;
		
			antennaMake = row->antennaMake;
		
			numSubband = row->numSubband;
		
			frequencyRange = row->frequencyRange;
		
			numReceptor = row->numReceptor;
		
			polarizationTypes = row->polarizationTypes;
		
			mainBeamEfficiency = row->mainBeamEfficiency;
		
			beamDescriptionUID = row->beamDescriptionUID;
		
			relativeAmplitudeRms = row->relativeAmplitudeRms;
		
			direction = row->direction;
		
			minValidDirection = row->minValidDirection;
		
			maxValidDirection = row->maxValidDirection;
		
			descriptionType = row->descriptionType;
		
			imageChannelNumber = row->imageChannelNumber;
		
			imageNominalFrequency = row->imageNominalFrequency;
		
		
		
		
		}
		
		 fromBinMethods["antennaName"] = &CalPrimaryBeamRow::antennaNameFromBin; 
		 fromBinMethods["receiverBand"] = &CalPrimaryBeamRow::receiverBandFromBin; 
		 fromBinMethods["calDataId"] = &CalPrimaryBeamRow::calDataIdFromBin; 
		 fromBinMethods["calReductionId"] = &CalPrimaryBeamRow::calReductionIdFromBin; 
		 fromBinMethods["startValidTime"] = &CalPrimaryBeamRow::startValidTimeFromBin; 
		 fromBinMethods["endValidTime"] = &CalPrimaryBeamRow::endValidTimeFromBin; 
		 fromBinMethods["antennaMake"] = &CalPrimaryBeamRow::antennaMakeFromBin; 
		 fromBinMethods["numSubband"] = &CalPrimaryBeamRow::numSubbandFromBin; 
		 fromBinMethods["frequencyRange"] = &CalPrimaryBeamRow::frequencyRangeFromBin; 
		 fromBinMethods["numReceptor"] = &CalPrimaryBeamRow::numReceptorFromBin; 
		 fromBinMethods["polarizationTypes"] = &CalPrimaryBeamRow::polarizationTypesFromBin; 
		 fromBinMethods["mainBeamEfficiency"] = &CalPrimaryBeamRow::mainBeamEfficiencyFromBin; 
		 fromBinMethods["beamDescriptionUID"] = &CalPrimaryBeamRow::beamDescriptionUIDFromBin; 
		 fromBinMethods["relativeAmplitudeRms"] = &CalPrimaryBeamRow::relativeAmplitudeRmsFromBin; 
		 fromBinMethods["direction"] = &CalPrimaryBeamRow::directionFromBin; 
		 fromBinMethods["minValidDirection"] = &CalPrimaryBeamRow::minValidDirectionFromBin; 
		 fromBinMethods["maxValidDirection"] = &CalPrimaryBeamRow::maxValidDirectionFromBin; 
		 fromBinMethods["descriptionType"] = &CalPrimaryBeamRow::descriptionTypeFromBin; 
		 fromBinMethods["imageChannelNumber"] = &CalPrimaryBeamRow::imageChannelNumberFromBin; 
		 fromBinMethods["imageNominalFrequency"] = &CalPrimaryBeamRow::imageNominalFrequencyFromBin; 
			
	
			
	}

	
	bool CalPrimaryBeamRow::compareNoAutoInc(std::string antennaName, ReceiverBandMod::ReceiverBand receiverBand, Tag calDataId, Tag calReductionId, ArrayTime startValidTime, ArrayTime endValidTime, AntennaMakeMod::AntennaMake antennaMake, int numSubband, std::vector<std::vector<Frequency > > frequencyRange, int numReceptor, std::vector<PolarizationTypeMod::PolarizationType > polarizationTypes, std::vector<double > mainBeamEfficiency, EntityRef beamDescriptionUID, float relativeAmplitudeRms, std::vector<Angle > direction, std::vector<Angle > minValidDirection, std::vector<Angle > maxValidDirection, PrimaryBeamDescriptionMod::PrimaryBeamDescription descriptionType, std::vector<int > imageChannelNumber, std::vector<Frequency > imageNominalFrequency) {
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
	

	
		
		result = result && (this->antennaMake == antennaMake);
		
		if (!result) return false;
	

	
		
		result = result && (this->numSubband == numSubband);
		
		if (!result) return false;
	

	
		
		result = result && (this->frequencyRange == frequencyRange);
		
		if (!result) return false;
	

	
		
		result = result && (this->numReceptor == numReceptor);
		
		if (!result) return false;
	

	
		
		result = result && (this->polarizationTypes == polarizationTypes);
		
		if (!result) return false;
	

	
		
		result = result && (this->mainBeamEfficiency == mainBeamEfficiency);
		
		if (!result) return false;
	

	
		
		result = result && (this->beamDescriptionUID == beamDescriptionUID);
		
		if (!result) return false;
	

	
		
		result = result && (this->relativeAmplitudeRms == relativeAmplitudeRms);
		
		if (!result) return false;
	

	
		
		result = result && (this->direction == direction);
		
		if (!result) return false;
	

	
		
		result = result && (this->minValidDirection == minValidDirection);
		
		if (!result) return false;
	

	
		
		result = result && (this->maxValidDirection == maxValidDirection);
		
		if (!result) return false;
	

	
		
		result = result && (this->descriptionType == descriptionType);
		
		if (!result) return false;
	

	
		
		result = result && (this->imageChannelNumber == imageChannelNumber);
		
		if (!result) return false;
	

	
		
		result = result && (this->imageNominalFrequency == imageNominalFrequency);
		
		if (!result) return false;
	

		return result;
	}	
	
	
	
	bool CalPrimaryBeamRow::compareRequiredValue(ArrayTime startValidTime, ArrayTime endValidTime, AntennaMakeMod::AntennaMake antennaMake, int numSubband, std::vector<std::vector<Frequency > > frequencyRange, int numReceptor, std::vector<PolarizationTypeMod::PolarizationType > polarizationTypes, std::vector<double > mainBeamEfficiency, EntityRef beamDescriptionUID, float relativeAmplitudeRms, std::vector<Angle > direction, std::vector<Angle > minValidDirection, std::vector<Angle > maxValidDirection, PrimaryBeamDescriptionMod::PrimaryBeamDescription descriptionType, std::vector<int > imageChannelNumber, std::vector<Frequency > imageNominalFrequency) {
		bool result;
		result = true;
		
	
		if (!(this->startValidTime == startValidTime)) return false;
	

	
		if (!(this->endValidTime == endValidTime)) return false;
	

	
		if (!(this->antennaMake == antennaMake)) return false;
	

	
		if (!(this->numSubband == numSubband)) return false;
	

	
		if (!(this->frequencyRange == frequencyRange)) return false;
	

	
		if (!(this->numReceptor == numReceptor)) return false;
	

	
		if (!(this->polarizationTypes == polarizationTypes)) return false;
	

	
		if (!(this->mainBeamEfficiency == mainBeamEfficiency)) return false;
	

	
		if (!(this->beamDescriptionUID == beamDescriptionUID)) return false;
	

	
		if (!(this->relativeAmplitudeRms == relativeAmplitudeRms)) return false;
	

	
		if (!(this->direction == direction)) return false;
	

	
		if (!(this->minValidDirection == minValidDirection)) return false;
	

	
		if (!(this->maxValidDirection == maxValidDirection)) return false;
	

	
		if (!(this->descriptionType == descriptionType)) return false;
	

	
		if (!(this->imageChannelNumber == imageChannelNumber)) return false;
	

	
		if (!(this->imageNominalFrequency == imageNominalFrequency)) return false;
	

		return result;
	}
	
	
	/**
	 * Return true if all required attributes of the value part are equal to their homologues
	 * in x and false otherwise.
	 *

	 * @param x a pointer on the CalPrimaryBeamRow whose required attributes of the value part 

	 * will be compared with those of this.
	 * @return a boolean.
	 */
	bool CalPrimaryBeamRow::equalByRequiredValue(CalPrimaryBeamRow*  x ) {
		
			
		if (this->startValidTime != x->startValidTime) return false;
			
		if (this->endValidTime != x->endValidTime) return false;
			
		if (this->antennaMake != x->antennaMake) return false;
			
		if (this->numSubband != x->numSubband) return false;
			
		if (this->frequencyRange != x->frequencyRange) return false;
			
		if (this->numReceptor != x->numReceptor) return false;
			
		if (this->polarizationTypes != x->polarizationTypes) return false;
			
		if (this->mainBeamEfficiency != x->mainBeamEfficiency) return false;
			
		if (this->beamDescriptionUID != x->beamDescriptionUID) return false;
			
		if (this->relativeAmplitudeRms != x->relativeAmplitudeRms) return false;
			
		if (this->direction != x->direction) return false;
			
		if (this->minValidDirection != x->minValidDirection) return false;
			
		if (this->maxValidDirection != x->maxValidDirection) return false;
			
		if (this->descriptionType != x->descriptionType) return false;
			
		if (this->imageChannelNumber != x->imageChannelNumber) return false;
			
		if (this->imageNominalFrequency != x->imageNominalFrequency) return false;
			
		
		return true;
	}	
	
/*
	 map<string, CalPrimaryBeamAttributeFromBin> CalPrimaryBeamRow::initFromBinMethods() {
		map<string, CalPrimaryBeamAttributeFromBin> result;
		
		result["antennaName"] = &CalPrimaryBeamRow::antennaNameFromBin;
		result["receiverBand"] = &CalPrimaryBeamRow::receiverBandFromBin;
		result["calDataId"] = &CalPrimaryBeamRow::calDataIdFromBin;
		result["calReductionId"] = &CalPrimaryBeamRow::calReductionIdFromBin;
		result["startValidTime"] = &CalPrimaryBeamRow::startValidTimeFromBin;
		result["endValidTime"] = &CalPrimaryBeamRow::endValidTimeFromBin;
		result["antennaMake"] = &CalPrimaryBeamRow::antennaMakeFromBin;
		result["numSubband"] = &CalPrimaryBeamRow::numSubbandFromBin;
		result["frequencyRange"] = &CalPrimaryBeamRow::frequencyRangeFromBin;
		result["numReceptor"] = &CalPrimaryBeamRow::numReceptorFromBin;
		result["polarizationTypes"] = &CalPrimaryBeamRow::polarizationTypesFromBin;
		result["mainBeamEfficiency"] = &CalPrimaryBeamRow::mainBeamEfficiencyFromBin;
		result["beamDescriptionUID"] = &CalPrimaryBeamRow::beamDescriptionUIDFromBin;
		result["relativeAmplitudeRms"] = &CalPrimaryBeamRow::relativeAmplitudeRmsFromBin;
		result["direction"] = &CalPrimaryBeamRow::directionFromBin;
		result["minValidDirection"] = &CalPrimaryBeamRow::minValidDirectionFromBin;
		result["maxValidDirection"] = &CalPrimaryBeamRow::maxValidDirectionFromBin;
		result["descriptionType"] = &CalPrimaryBeamRow::descriptionTypeFromBin;
		result["imageChannelNumber"] = &CalPrimaryBeamRow::imageChannelNumberFromBin;
		result["imageNominalFrequency"] = &CalPrimaryBeamRow::imageNominalFrequencyFromBin;
		
		
			
		
		return result;	
	}
*/	
} // End namespace asdm
 
