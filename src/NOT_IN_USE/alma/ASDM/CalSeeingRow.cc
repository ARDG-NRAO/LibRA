
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
 * File CalSeeingRow.cpp
 */
 
#include <vector>
#include <set>

#include <alma/ASDM/ASDM.h>
#include <alma/ASDM/CalSeeingRow.h>
#include <alma/ASDM/CalSeeingTable.h>

#include <alma/ASDM/CalDataTable.h>
#include <alma/ASDM/CalDataRow.h>

#include <alma/ASDM/CalReductionTable.h>
#include <alma/ASDM/CalReductionRow.h>
	

using asdm::ASDM;
using asdm::CalSeeingRow;
using asdm::CalSeeingTable;

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
	CalSeeingRow::~CalSeeingRow() {
	}

	/**
	 * Return the table to which this row belongs.
	 */
	CalSeeingTable &CalSeeingRow::getTable() const {
		return table;
	}

	bool CalSeeingRow::isAdded() const {
		return hasBeenAdded;
	}	

	void CalSeeingRow::isAdded(bool added) {
		hasBeenAdded = added;
	}
	
#ifndef WITHOUT_ACS
	using asdmIDL::CalSeeingRowIDL;
#endif
	
#ifndef WITHOUT_ACS
	/**
	 * Return this row in the form of an IDL struct.
	 * @return The values of this row as a CalSeeingRowIDL struct.
	 */
	CalSeeingRowIDL *CalSeeingRow::toIDL() const {
		CalSeeingRowIDL *x = new CalSeeingRowIDL ();
		
		// Fill the IDL structure.
	
		
	
  		
		
		
			
				
		x->atmPhaseCorrection = atmPhaseCorrection;
 				
 			
		
	

	
  		
		
		
			
		x->startValidTime = startValidTime.toIDLArrayTime();
			
		
	

	
  		
		
		
			
		x->endValidTime = endValidTime.toIDLArrayTime();
			
		
	

	
  		
		
		
			
		x->frequencyRange.length(frequencyRange.size());
		for (unsigned int i = 0; i < frequencyRange.size(); ++i) {
			
			x->frequencyRange[i] = frequencyRange.at(i).toIDLFrequency();
			
	 	}
			
		
	

	
  		
		
		
			
		x->integrationTime = integrationTime.toIDLInterval();
			
		
	

	
  		
		
		
			
				
		x->numBaseLengths = numBaseLengths;
 				
 			
		
	

	
  		
		
		
			
		x->baselineLengths.length(baselineLengths.size());
		for (unsigned int i = 0; i < baselineLengths.size(); ++i) {
			
			x->baselineLengths[i] = baselineLengths.at(i).toIDLLength();
			
	 	}
			
		
	

	
  		
		
		
			
		x->phaseRMS.length(phaseRMS.size());
		for (unsigned int i = 0; i < phaseRMS.size(); ++i) {
			
			x->phaseRMS[i] = phaseRMS.at(i).toIDLAngle();
			
	 	}
			
		
	

	
  		
		
		
			
		x->seeing = seeing.toIDLAngle();
			
		
	

	
  		
		
		
			
		x->seeingError = seeingError.toIDLAngle();
			
		
	

	
  		
		
		x->exponentExists = exponentExists;
		
		
			
				
		x->exponent = exponent;
 				
 			
		
	

	
  		
		
		x->outerScaleExists = outerScaleExists;
		
		
			
		x->outerScale = outerScale.toIDLLength();
			
		
	

	
  		
		
		x->outerScaleRMSExists = outerScaleRMSExists;
		
		
			
		x->outerScaleRMS = outerScaleRMS.toIDLAngle();
			
		
	

	
	
		
	
  	
 		
		
	 	
			
		x->calDataId = calDataId.toIDLTag();
			
	 	 		
  	

	
  	
 		
		
	 	
			
		x->calReductionId = calReductionId.toIDLTag();
			
	 	 		
  	

	
		
	

	

		
		return x;
	
	}
	
	void CalSeeingRow::toIDL(asdmIDL::CalSeeingRowIDL& x) const {
		// Set the x's fields.
	
		
	
  		
		
		
			
				
		x.atmPhaseCorrection = atmPhaseCorrection;
 				
 			
		
	

	
  		
		
		
			
		x.startValidTime = startValidTime.toIDLArrayTime();
			
		
	

	
  		
		
		
			
		x.endValidTime = endValidTime.toIDLArrayTime();
			
		
	

	
  		
		
		
			
		x.frequencyRange.length(frequencyRange.size());
		for (unsigned int i = 0; i < frequencyRange.size(); ++i) {
			
			x.frequencyRange[i] = frequencyRange.at(i).toIDLFrequency();
			
	 	}
			
		
	

	
  		
		
		
			
		x.integrationTime = integrationTime.toIDLInterval();
			
		
	

	
  		
		
		
			
				
		x.numBaseLengths = numBaseLengths;
 				
 			
		
	

	
  		
		
		
			
		x.baselineLengths.length(baselineLengths.size());
		for (unsigned int i = 0; i < baselineLengths.size(); ++i) {
			
			x.baselineLengths[i] = baselineLengths.at(i).toIDLLength();
			
	 	}
			
		
	

	
  		
		
		
			
		x.phaseRMS.length(phaseRMS.size());
		for (unsigned int i = 0; i < phaseRMS.size(); ++i) {
			
			x.phaseRMS[i] = phaseRMS.at(i).toIDLAngle();
			
	 	}
			
		
	

	
  		
		
		
			
		x.seeing = seeing.toIDLAngle();
			
		
	

	
  		
		
		
			
		x.seeingError = seeingError.toIDLAngle();
			
		
	

	
  		
		
		x.exponentExists = exponentExists;
		
		
			
				
		x.exponent = exponent;
 				
 			
		
	

	
  		
		
		x.outerScaleExists = outerScaleExists;
		
		
			
		x.outerScale = outerScale.toIDLLength();
			
		
	

	
  		
		
		x.outerScaleRMSExists = outerScaleRMSExists;
		
		
			
		x.outerScaleRMS = outerScaleRMS.toIDLAngle();
			
		
	

	
	
		
	
  	
 		
		
	 	
			
		x.calDataId = calDataId.toIDLTag();
			
	 	 		
  	

	
  	
 		
		
	 	
			
		x.calReductionId = calReductionId.toIDLTag();
			
	 	 		
  	

	
		
	

	

	
	}
#endif
	

#ifndef WITHOUT_ACS
	/**
	 * Fill the values of this row from the IDL struct CalSeeingRowIDL.
	 * @param x The IDL struct containing the values used to fill this row.
	 */
	void CalSeeingRow::setFromIDL (CalSeeingRowIDL x){
		try {
		// Fill the values from x.
	
		
	
		
		
			
		setAtmPhaseCorrection(x.atmPhaseCorrection);
  			
 		
		
	

	
		
		
			
		setStartValidTime(ArrayTime (x.startValidTime));
			
 		
		
	

	
		
		
			
		setEndValidTime(ArrayTime (x.endValidTime));
			
 		
		
	

	
		
		
			
		frequencyRange .clear();
		for (unsigned int i = 0; i <x.frequencyRange.length(); ++i) {
			
			frequencyRange.push_back(Frequency (x.frequencyRange[i]));
			
		}
			
  		
		
	

	
		
		
			
		setIntegrationTime(Interval (x.integrationTime));
			
 		
		
	

	
		
		
			
		setNumBaseLengths(x.numBaseLengths);
  			
 		
		
	

	
		
		
			
		baselineLengths .clear();
		for (unsigned int i = 0; i <x.baselineLengths.length(); ++i) {
			
			baselineLengths.push_back(Length (x.baselineLengths[i]));
			
		}
			
  		
		
	

	
		
		
			
		phaseRMS .clear();
		for (unsigned int i = 0; i <x.phaseRMS.length(); ++i) {
			
			phaseRMS.push_back(Angle (x.phaseRMS[i]));
			
		}
			
  		
		
	

	
		
		
			
		setSeeing(Angle (x.seeing));
			
 		
		
	

	
		
		
			
		setSeeingError(Angle (x.seeingError));
			
 		
		
	

	
		
		exponentExists = x.exponentExists;
		if (x.exponentExists) {
		
		
			
		setExponent(x.exponent);
  			
 		
		
		}
		
	

	
		
		outerScaleExists = x.outerScaleExists;
		if (x.outerScaleExists) {
		
		
			
		setOuterScale(Length (x.outerScale));
			
 		
		
		}
		
	

	
		
		outerScaleRMSExists = x.outerScaleRMSExists;
		if (x.outerScaleRMSExists) {
		
		
			
		setOuterScaleRMS(Angle (x.outerScaleRMS));
			
 		
		
		}
		
	

	
	
		
	
		
		
			
		setCalDataId(Tag (x.calDataId));
			
 		
		
	

	
		
		
			
		setCalReductionId(Tag (x.calReductionId));
			
 		
		
	

	
		
	

	

		} catch (const IllegalAccessException &err) {
			throw ConversionException (err.getMessage(),"CalSeeing");
		}
	}
#endif
	
	/**
	 * Return this row in the form of an XML string.
	 * @return The values of this row as an XML string.
	 */
	string CalSeeingRow::toXML() const {
		string buf;
		buf.append("<row> \n");
		
	
		
  	
 		
		
			buf.append(EnumerationParser::toXML("atmPhaseCorrection", atmPhaseCorrection));
		
		
	

  	
 		
		
		Parser::toXML(startValidTime, "startValidTime", buf);
		
		
	

  	
 		
		
		Parser::toXML(endValidTime, "endValidTime", buf);
		
		
	

  	
 		
		
		Parser::toXML(frequencyRange, "frequencyRange", buf);
		
		
	

  	
 		
		
		Parser::toXML(integrationTime, "integrationTime", buf);
		
		
	

  	
 		
		
		Parser::toXML(numBaseLengths, "numBaseLengths", buf);
		
		
	

  	
 		
		
		Parser::toXML(baselineLengths, "baselineLengths", buf);
		
		
	

  	
 		
		
		Parser::toXML(phaseRMS, "phaseRMS", buf);
		
		
	

  	
 		
		
		Parser::toXML(seeing, "seeing", buf);
		
		
	

  	
 		
		
		Parser::toXML(seeingError, "seeingError", buf);
		
		
	

  	
 		
		if (exponentExists) {
		
		
		Parser::toXML(exponent, "exponent", buf);
		
		
		}
		
	

  	
 		
		if (outerScaleExists) {
		
		
		Parser::toXML(outerScale, "outerScale", buf);
		
		
		}
		
	

  	
 		
		if (outerScaleRMSExists) {
		
		
		Parser::toXML(outerScaleRMS, "outerScaleRMS", buf);
		
		
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
	void CalSeeingRow::setFromXML (string rowDoc) {
		Parser row(rowDoc);
		string s = "";
		try {
	
		
	
		
		
		
		atmPhaseCorrection = EnumerationParser::getAtmPhaseCorrection("atmPhaseCorrection","CalSeeing",rowDoc);
		
		
		
	

	
  		
			
	  	setStartValidTime(Parser::getArrayTime("startValidTime","CalSeeing",rowDoc));
			
		
	

	
  		
			
	  	setEndValidTime(Parser::getArrayTime("endValidTime","CalSeeing",rowDoc));
			
		
	

	
  		
			
					
	  	setFrequencyRange(Parser::get1DFrequency("frequencyRange","CalSeeing",rowDoc));
	  			
	  		
		
	

	
  		
			
	  	setIntegrationTime(Parser::getInterval("integrationTime","CalSeeing",rowDoc));
			
		
	

	
  		
			
	  	setNumBaseLengths(Parser::getInteger("numBaseLengths","CalSeeing",rowDoc));
			
		
	

	
  		
			
					
	  	setBaselineLengths(Parser::get1DLength("baselineLengths","CalSeeing",rowDoc));
	  			
	  		
		
	

	
  		
			
					
	  	setPhaseRMS(Parser::get1DAngle("phaseRMS","CalSeeing",rowDoc));
	  			
	  		
		
	

	
  		
			
	  	setSeeing(Parser::getAngle("seeing","CalSeeing",rowDoc));
			
		
	

	
  		
			
	  	setSeeingError(Parser::getAngle("seeingError","CalSeeing",rowDoc));
			
		
	

	
  		
        if (row.isStr("<exponent>")) {
			
	  		setExponent(Parser::getFloat("exponent","CalSeeing",rowDoc));
			
		}
 		
	

	
  		
        if (row.isStr("<outerScale>")) {
			
	  		setOuterScale(Parser::getLength("outerScale","CalSeeing",rowDoc));
			
		}
 		
	

	
  		
        if (row.isStr("<outerScaleRMS>")) {
			
	  		setOuterScaleRMS(Parser::getAngle("outerScaleRMS","CalSeeing",rowDoc));
			
		}
 		
	

	
	
		
	
  		
			
	  	setCalDataId(Parser::getTag("calDataId","CalData",rowDoc));
			
		
	

	
  		
			
	  	setCalReductionId(Parser::getTag("calReductionId","CalReduction",rowDoc));
			
		
	

	
		
	

	

		} catch (const IllegalAccessException &err) {
			throw ConversionException (err.getMessage(),"CalSeeing");
		}
	}
	
	void CalSeeingRow::toBin(EndianOSStream& eoss) {
	
	
	
	
		
					
		eoss.writeString(CAtmPhaseCorrection::name(atmPhaseCorrection));
			/* eoss.writeInt(atmPhaseCorrection); */
				
		
	

	
	
		
	calDataId.toBin(eoss);
		
	

	
	
		
	calReductionId.toBin(eoss);
		
	

	
	
		
	startValidTime.toBin(eoss);
		
	

	
	
		
	endValidTime.toBin(eoss);
		
	

	
	
		
	Frequency::toBin(frequencyRange, eoss);
		
	

	
	
		
	integrationTime.toBin(eoss);
		
	

	
	
		
						
			eoss.writeInt(numBaseLengths);
				
		
	

	
	
		
	Length::toBin(baselineLengths, eoss);
		
	

	
	
		
	Angle::toBin(phaseRMS, eoss);
		
	

	
	
		
	seeing.toBin(eoss);
		
	

	
	
		
	seeingError.toBin(eoss);
		
	


	
	
	eoss.writeBoolean(exponentExists);
	if (exponentExists) {
	
	
	
		
						
			eoss.writeFloat(exponent);
				
		
	

	}

	eoss.writeBoolean(outerScaleExists);
	if (outerScaleExists) {
	
	
	
		
	outerScale.toBin(eoss);
		
	

	}

	eoss.writeBoolean(outerScaleRMSExists);
	if (outerScaleRMSExists) {
	
	
	
		
	outerScaleRMS.toBin(eoss);
		
	

	}

	}
	
void CalSeeingRow::atmPhaseCorrectionFromBin(EndianIStream& eis) {
		
	
	
		
			
		atmPhaseCorrection = CAtmPhaseCorrection::literal(eis.readString());
			
		
	
	
}
void CalSeeingRow::calDataIdFromBin(EndianIStream& eis) {
		
	
		
		
		calDataId =  Tag::fromBin(eis);
		
	
	
}
void CalSeeingRow::calReductionIdFromBin(EndianIStream& eis) {
		
	
		
		
		calReductionId =  Tag::fromBin(eis);
		
	
	
}
void CalSeeingRow::startValidTimeFromBin(EndianIStream& eis) {
		
	
		
		
		startValidTime =  ArrayTime::fromBin(eis);
		
	
	
}
void CalSeeingRow::endValidTimeFromBin(EndianIStream& eis) {
		
	
		
		
		endValidTime =  ArrayTime::fromBin(eis);
		
	
	
}
void CalSeeingRow::frequencyRangeFromBin(EndianIStream& eis) {
		
	
		
		
			
	
	frequencyRange = Frequency::from1DBin(eis);	
	

		
	
	
}
void CalSeeingRow::integrationTimeFromBin(EndianIStream& eis) {
		
	
		
		
		integrationTime =  Interval::fromBin(eis);
		
	
	
}
void CalSeeingRow::numBaseLengthsFromBin(EndianIStream& eis) {
		
	
	
		
			
		numBaseLengths =  eis.readInt();
			
		
	
	
}
void CalSeeingRow::baselineLengthsFromBin(EndianIStream& eis) {
		
	
		
		
			
	
	baselineLengths = Length::from1DBin(eis);	
	

		
	
	
}
void CalSeeingRow::phaseRMSFromBin(EndianIStream& eis) {
		
	
		
		
			
	
	phaseRMS = Angle::from1DBin(eis);	
	

		
	
	
}
void CalSeeingRow::seeingFromBin(EndianIStream& eis) {
		
	
		
		
		seeing =  Angle::fromBin(eis);
		
	
	
}
void CalSeeingRow::seeingErrorFromBin(EndianIStream& eis) {
		
	
		
		
		seeingError =  Angle::fromBin(eis);
		
	
	
}

void CalSeeingRow::exponentFromBin(EndianIStream& eis) {
		
	exponentExists = eis.readBoolean();
	if (exponentExists) {
		
	
	
		
			
		exponent =  eis.readFloat();
			
		
	

	}
	
}
void CalSeeingRow::outerScaleFromBin(EndianIStream& eis) {
		
	outerScaleExists = eis.readBoolean();
	if (outerScaleExists) {
		
	
		
		
		outerScale =  Length::fromBin(eis);
		
	

	}
	
}
void CalSeeingRow::outerScaleRMSFromBin(EndianIStream& eis) {
		
	outerScaleRMSExists = eis.readBoolean();
	if (outerScaleRMSExists) {
		
	
		
		
		outerScaleRMS =  Angle::fromBin(eis);
		
	

	}
	
}
	
	
	CalSeeingRow* CalSeeingRow::fromBin(EndianIStream& eis, CalSeeingTable& table, const vector<string>& attributesSeq) {
		CalSeeingRow* row = new  CalSeeingRow(table);
		
		map<string, CalSeeingAttributeFromBin>::iterator iter ;
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
					throw ConversionException("There is not method to read an attribute '"+attributesSeq.at(i)+"'.", "CalSeeingTable");
			}
				
		}				
		return row;
	}

	//
	// A collection of methods to set the value of the attributes from their textual value in the XML representation
	// of one row.
	//
	
	// Convert a string into an AtmPhaseCorrection 
	void CalSeeingRow::atmPhaseCorrectionFromText(const string & s) {
		 
          
		atmPhaseCorrection = ASDMValuesParser::parse<AtmPhaseCorrectionMod::AtmPhaseCorrection>(s);
          
		
	}
	
	
	// Convert a string into an Tag 
	void CalSeeingRow::calDataIdFromText(const string & s) {
		 
          
		calDataId = ASDMValuesParser::parse<Tag>(s);
          
		
	}
	
	
	// Convert a string into an Tag 
	void CalSeeingRow::calReductionIdFromText(const string & s) {
		 
          
		calReductionId = ASDMValuesParser::parse<Tag>(s);
          
		
	}
	
	
	// Convert a string into an ArrayTime 
	void CalSeeingRow::startValidTimeFromText(const string & s) {
		 
          
		startValidTime = ASDMValuesParser::parse<ArrayTime>(s);
          
		
	}
	
	
	// Convert a string into an ArrayTime 
	void CalSeeingRow::endValidTimeFromText(const string & s) {
		 
          
		endValidTime = ASDMValuesParser::parse<ArrayTime>(s);
          
		
	}
	
	
	// Convert a string into an Frequency 
	void CalSeeingRow::frequencyRangeFromText(const string & s) {
		 
          
		frequencyRange = ASDMValuesParser::parse1D<Frequency>(s);
          
		
	}
	
	
	// Convert a string into an Interval 
	void CalSeeingRow::integrationTimeFromText(const string & s) {
		 
          
		integrationTime = ASDMValuesParser::parse<Interval>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void CalSeeingRow::numBaseLengthsFromText(const string & s) {
		 
          
		numBaseLengths = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	// Convert a string into an Length 
	void CalSeeingRow::baselineLengthsFromText(const string & s) {
		 
          
		baselineLengths = ASDMValuesParser::parse1D<Length>(s);
          
		
	}
	
	
	// Convert a string into an Angle 
	void CalSeeingRow::phaseRMSFromText(const string & s) {
		 
          
		phaseRMS = ASDMValuesParser::parse1D<Angle>(s);
          
		
	}
	
	
	// Convert a string into an Angle 
	void CalSeeingRow::seeingFromText(const string & s) {
		 
          
		seeing = ASDMValuesParser::parse<Angle>(s);
          
		
	}
	
	
	// Convert a string into an Angle 
	void CalSeeingRow::seeingErrorFromText(const string & s) {
		 
          
		seeingError = ASDMValuesParser::parse<Angle>(s);
          
		
	}
	

	
	// Convert a string into an float 
	void CalSeeingRow::exponentFromText(const string & s) {
		exponentExists = true;
		 
          
		exponent = ASDMValuesParser::parse<float>(s);
          
		
	}
	
	
	// Convert a string into an Length 
	void CalSeeingRow::outerScaleFromText(const string & s) {
		outerScaleExists = true;
		 
          
		outerScale = ASDMValuesParser::parse<Length>(s);
          
		
	}
	
	
	// Convert a string into an Angle 
	void CalSeeingRow::outerScaleRMSFromText(const string & s) {
		outerScaleRMSExists = true;
		 
          
		outerScaleRMS = ASDMValuesParser::parse<Angle>(s);
          
		
	}
	
	
	
	void CalSeeingRow::fromText(const std::string& attributeName, const std::string&  t) {
		map<string, CalSeeingAttributeFromText>::iterator iter;
		if ((iter = fromTextMethods.find(attributeName)) == fromTextMethods.end())
			throw ConversionException("I do not know what to do with '"+attributeName+"' and its content '"+t+"' (while parsing an XML document)", "CalSeeingTable");
		(this->*(iter->second))(t);
	}
			
	////////////////////////////////////////////////
	// Intrinsic Table Attributes getters/setters //
	////////////////////////////////////////////////
	
	

	
 	/**
 	 * Get atmPhaseCorrection.
 	 * @return atmPhaseCorrection as AtmPhaseCorrectionMod::AtmPhaseCorrection
 	 */
 	AtmPhaseCorrectionMod::AtmPhaseCorrection CalSeeingRow::getAtmPhaseCorrection() const {
	
  		return atmPhaseCorrection;
 	}

 	/**
 	 * Set atmPhaseCorrection with the specified AtmPhaseCorrectionMod::AtmPhaseCorrection.
 	 * @param atmPhaseCorrection The AtmPhaseCorrectionMod::AtmPhaseCorrection value to which atmPhaseCorrection is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void CalSeeingRow::setAtmPhaseCorrection (AtmPhaseCorrectionMod::AtmPhaseCorrection atmPhaseCorrection)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("atmPhaseCorrection", "CalSeeing");
		
  		}
  	
 		this->atmPhaseCorrection = atmPhaseCorrection;
	
 	}
	
	

	

	
 	/**
 	 * Get startValidTime.
 	 * @return startValidTime as ArrayTime
 	 */
 	ArrayTime CalSeeingRow::getStartValidTime() const {
	
  		return startValidTime;
 	}

 	/**
 	 * Set startValidTime with the specified ArrayTime.
 	 * @param startValidTime The ArrayTime value to which startValidTime is to be set.
 	 
 	
 		
 	 */
 	void CalSeeingRow::setStartValidTime (ArrayTime startValidTime)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->startValidTime = startValidTime;
	
 	}
	
	

	

	
 	/**
 	 * Get endValidTime.
 	 * @return endValidTime as ArrayTime
 	 */
 	ArrayTime CalSeeingRow::getEndValidTime() const {
	
  		return endValidTime;
 	}

 	/**
 	 * Set endValidTime with the specified ArrayTime.
 	 * @param endValidTime The ArrayTime value to which endValidTime is to be set.
 	 
 	
 		
 	 */
 	void CalSeeingRow::setEndValidTime (ArrayTime endValidTime)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->endValidTime = endValidTime;
	
 	}
	
	

	

	
 	/**
 	 * Get frequencyRange.
 	 * @return frequencyRange as std::vector<Frequency >
 	 */
 	std::vector<Frequency > CalSeeingRow::getFrequencyRange() const {
	
  		return frequencyRange;
 	}

 	/**
 	 * Set frequencyRange with the specified std::vector<Frequency >.
 	 * @param frequencyRange The std::vector<Frequency > value to which frequencyRange is to be set.
 	 
 	
 		
 	 */
 	void CalSeeingRow::setFrequencyRange (std::vector<Frequency > frequencyRange)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->frequencyRange = frequencyRange;
	
 	}
	
	

	

	
 	/**
 	 * Get integrationTime.
 	 * @return integrationTime as Interval
 	 */
 	Interval CalSeeingRow::getIntegrationTime() const {
	
  		return integrationTime;
 	}

 	/**
 	 * Set integrationTime with the specified Interval.
 	 * @param integrationTime The Interval value to which integrationTime is to be set.
 	 
 	
 		
 	 */
 	void CalSeeingRow::setIntegrationTime (Interval integrationTime)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->integrationTime = integrationTime;
	
 	}
	
	

	

	
 	/**
 	 * Get numBaseLengths.
 	 * @return numBaseLengths as int
 	 */
 	int CalSeeingRow::getNumBaseLengths() const {
	
  		return numBaseLengths;
 	}

 	/**
 	 * Set numBaseLengths with the specified int.
 	 * @param numBaseLengths The int value to which numBaseLengths is to be set.
 	 
 	
 		
 	 */
 	void CalSeeingRow::setNumBaseLengths (int numBaseLengths)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->numBaseLengths = numBaseLengths;
	
 	}
	
	

	

	
 	/**
 	 * Get baselineLengths.
 	 * @return baselineLengths as std::vector<Length >
 	 */
 	std::vector<Length > CalSeeingRow::getBaselineLengths() const {
	
  		return baselineLengths;
 	}

 	/**
 	 * Set baselineLengths with the specified std::vector<Length >.
 	 * @param baselineLengths The std::vector<Length > value to which baselineLengths is to be set.
 	 
 	
 		
 	 */
 	void CalSeeingRow::setBaselineLengths (std::vector<Length > baselineLengths)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->baselineLengths = baselineLengths;
	
 	}
	
	

	

	
 	/**
 	 * Get phaseRMS.
 	 * @return phaseRMS as std::vector<Angle >
 	 */
 	std::vector<Angle > CalSeeingRow::getPhaseRMS() const {
	
  		return phaseRMS;
 	}

 	/**
 	 * Set phaseRMS with the specified std::vector<Angle >.
 	 * @param phaseRMS The std::vector<Angle > value to which phaseRMS is to be set.
 	 
 	
 		
 	 */
 	void CalSeeingRow::setPhaseRMS (std::vector<Angle > phaseRMS)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->phaseRMS = phaseRMS;
	
 	}
	
	

	

	
 	/**
 	 * Get seeing.
 	 * @return seeing as Angle
 	 */
 	Angle CalSeeingRow::getSeeing() const {
	
  		return seeing;
 	}

 	/**
 	 * Set seeing with the specified Angle.
 	 * @param seeing The Angle value to which seeing is to be set.
 	 
 	
 		
 	 */
 	void CalSeeingRow::setSeeing (Angle seeing)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->seeing = seeing;
	
 	}
	
	

	

	
 	/**
 	 * Get seeingError.
 	 * @return seeingError as Angle
 	 */
 	Angle CalSeeingRow::getSeeingError() const {
	
  		return seeingError;
 	}

 	/**
 	 * Set seeingError with the specified Angle.
 	 * @param seeingError The Angle value to which seeingError is to be set.
 	 
 	
 		
 	 */
 	void CalSeeingRow::setSeeingError (Angle seeingError)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->seeingError = seeingError;
	
 	}
	
	

	
	/**
	 * The attribute exponent is optional. Return true if this attribute exists.
	 * @return true if and only if the exponent attribute exists. 
	 */
	bool CalSeeingRow::isExponentExists() const {
		return exponentExists;
	}
	

	
 	/**
 	 * Get exponent, which is optional.
 	 * @return exponent as float
 	 * @throw IllegalAccessException If exponent does not exist.
 	 */
 	float CalSeeingRow::getExponent() const  {
		if (!exponentExists) {
			throw IllegalAccessException("exponent", "CalSeeing");
		}
	
  		return exponent;
 	}

 	/**
 	 * Set exponent with the specified float.
 	 * @param exponent The float value to which exponent is to be set.
 	 
 	
 	 */
 	void CalSeeingRow::setExponent (float exponent) {
	
 		this->exponent = exponent;
	
		exponentExists = true;
	
 	}
	
	
	/**
	 * Mark exponent, which is an optional field, as non-existent.
	 */
	void CalSeeingRow::clearExponent () {
		exponentExists = false;
	}
	

	
	/**
	 * The attribute outerScale is optional. Return true if this attribute exists.
	 * @return true if and only if the outerScale attribute exists. 
	 */
	bool CalSeeingRow::isOuterScaleExists() const {
		return outerScaleExists;
	}
	

	
 	/**
 	 * Get outerScale, which is optional.
 	 * @return outerScale as Length
 	 * @throw IllegalAccessException If outerScale does not exist.
 	 */
 	Length CalSeeingRow::getOuterScale() const  {
		if (!outerScaleExists) {
			throw IllegalAccessException("outerScale", "CalSeeing");
		}
	
  		return outerScale;
 	}

 	/**
 	 * Set outerScale with the specified Length.
 	 * @param outerScale The Length value to which outerScale is to be set.
 	 
 	
 	 */
 	void CalSeeingRow::setOuterScale (Length outerScale) {
	
 		this->outerScale = outerScale;
	
		outerScaleExists = true;
	
 	}
	
	
	/**
	 * Mark outerScale, which is an optional field, as non-existent.
	 */
	void CalSeeingRow::clearOuterScale () {
		outerScaleExists = false;
	}
	

	
	/**
	 * The attribute outerScaleRMS is optional. Return true if this attribute exists.
	 * @return true if and only if the outerScaleRMS attribute exists. 
	 */
	bool CalSeeingRow::isOuterScaleRMSExists() const {
		return outerScaleRMSExists;
	}
	

	
 	/**
 	 * Get outerScaleRMS, which is optional.
 	 * @return outerScaleRMS as Angle
 	 * @throw IllegalAccessException If outerScaleRMS does not exist.
 	 */
 	Angle CalSeeingRow::getOuterScaleRMS() const  {
		if (!outerScaleRMSExists) {
			throw IllegalAccessException("outerScaleRMS", "CalSeeing");
		}
	
  		return outerScaleRMS;
 	}

 	/**
 	 * Set outerScaleRMS with the specified Angle.
 	 * @param outerScaleRMS The Angle value to which outerScaleRMS is to be set.
 	 
 	
 	 */
 	void CalSeeingRow::setOuterScaleRMS (Angle outerScaleRMS) {
	
 		this->outerScaleRMS = outerScaleRMS;
	
		outerScaleRMSExists = true;
	
 	}
	
	
	/**
	 * Mark outerScaleRMS, which is an optional field, as non-existent.
	 */
	void CalSeeingRow::clearOuterScaleRMS () {
		outerScaleRMSExists = false;
	}
	

	
	///////////////////////////////////////////////
	// Extrinsic Table Attributes getters/setters//
	///////////////////////////////////////////////
	
	

	
 	/**
 	 * Get calDataId.
 	 * @return calDataId as Tag
 	 */
 	Tag CalSeeingRow::getCalDataId() const {
	
  		return calDataId;
 	}

 	/**
 	 * Set calDataId with the specified Tag.
 	 * @param calDataId The Tag value to which calDataId is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void CalSeeingRow::setCalDataId (Tag calDataId)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("calDataId", "CalSeeing");
		
  		}
  	
 		this->calDataId = calDataId;
	
 	}
	
	

	

	
 	/**
 	 * Get calReductionId.
 	 * @return calReductionId as Tag
 	 */
 	Tag CalSeeingRow::getCalReductionId() const {
	
  		return calReductionId;
 	}

 	/**
 	 * Set calReductionId with the specified Tag.
 	 * @param calReductionId The Tag value to which calReductionId is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void CalSeeingRow::setCalReductionId (Tag calReductionId)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("calReductionId", "CalSeeing");
		
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
	 CalDataRow* CalSeeingRow::getCalDataUsingCalDataId() {
	 
	 	return table.getContainer().getCalData().getRowByKey(calDataId);
	 }
	 

	

	
	
	
		

	/**
	 * Returns the pointer to the row in the CalReduction table having CalReduction.calReductionId == calReductionId
	 * @return a CalReductionRow*
	 * 
	 
	 */
	 CalReductionRow* CalSeeingRow::getCalReductionUsingCalReductionId() {
	 
	 	return table.getContainer().getCalReduction().getRowByKey(calReductionId);
	 }
	 

	

	
	/**
	 * Create a CalSeeingRow.
	 * <p>
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 */ 
	CalSeeingRow::CalSeeingRow (CalSeeingTable &t) : table(t) {
		hasBeenAdded = false;
		
	
	

	

	

	

	

	

	

	

	

	

	
		exponentExists = false;
	

	
		outerScaleExists = false;
	

	
		outerScaleRMSExists = false;
	

	
	

	

	
	
	
	
// This attribute is scalar and has an enumeration type. Let's initialize it to some valid value (the 1st of the enumeration).		
atmPhaseCorrection = CAtmPhaseCorrection::from_int(0);
	

	

	

	

	

	

	

	

	

	

	

	

	

	
	
	 fromBinMethods["atmPhaseCorrection"] = &CalSeeingRow::atmPhaseCorrectionFromBin; 
	 fromBinMethods["calDataId"] = &CalSeeingRow::calDataIdFromBin; 
	 fromBinMethods["calReductionId"] = &CalSeeingRow::calReductionIdFromBin; 
	 fromBinMethods["startValidTime"] = &CalSeeingRow::startValidTimeFromBin; 
	 fromBinMethods["endValidTime"] = &CalSeeingRow::endValidTimeFromBin; 
	 fromBinMethods["frequencyRange"] = &CalSeeingRow::frequencyRangeFromBin; 
	 fromBinMethods["integrationTime"] = &CalSeeingRow::integrationTimeFromBin; 
	 fromBinMethods["numBaseLengths"] = &CalSeeingRow::numBaseLengthsFromBin; 
	 fromBinMethods["baselineLengths"] = &CalSeeingRow::baselineLengthsFromBin; 
	 fromBinMethods["phaseRMS"] = &CalSeeingRow::phaseRMSFromBin; 
	 fromBinMethods["seeing"] = &CalSeeingRow::seeingFromBin; 
	 fromBinMethods["seeingError"] = &CalSeeingRow::seeingErrorFromBin; 
		
	
	 fromBinMethods["exponent"] = &CalSeeingRow::exponentFromBin; 
	 fromBinMethods["outerScale"] = &CalSeeingRow::outerScaleFromBin; 
	 fromBinMethods["outerScaleRMS"] = &CalSeeingRow::outerScaleRMSFromBin; 
	
	
	
	
				 
	fromTextMethods["atmPhaseCorrection"] = &CalSeeingRow::atmPhaseCorrectionFromText;
		 
	
				 
	fromTextMethods["calDataId"] = &CalSeeingRow::calDataIdFromText;
		 
	
				 
	fromTextMethods["calReductionId"] = &CalSeeingRow::calReductionIdFromText;
		 
	
				 
	fromTextMethods["startValidTime"] = &CalSeeingRow::startValidTimeFromText;
		 
	
				 
	fromTextMethods["endValidTime"] = &CalSeeingRow::endValidTimeFromText;
		 
	
				 
	fromTextMethods["frequencyRange"] = &CalSeeingRow::frequencyRangeFromText;
		 
	
				 
	fromTextMethods["integrationTime"] = &CalSeeingRow::integrationTimeFromText;
		 
	
				 
	fromTextMethods["numBaseLengths"] = &CalSeeingRow::numBaseLengthsFromText;
		 
	
				 
	fromTextMethods["baselineLengths"] = &CalSeeingRow::baselineLengthsFromText;
		 
	
				 
	fromTextMethods["phaseRMS"] = &CalSeeingRow::phaseRMSFromText;
		 
	
				 
	fromTextMethods["seeing"] = &CalSeeingRow::seeingFromText;
		 
	
				 
	fromTextMethods["seeingError"] = &CalSeeingRow::seeingErrorFromText;
		 
	

	 
				
	fromTextMethods["exponent"] = &CalSeeingRow::exponentFromText;
		 	
	 
				
	fromTextMethods["outerScale"] = &CalSeeingRow::outerScaleFromText;
		 	
	 
				
	fromTextMethods["outerScaleRMS"] = &CalSeeingRow::outerScaleRMSFromText;
		 	
		
	}
	
	CalSeeingRow::CalSeeingRow (CalSeeingTable &t, CalSeeingRow *row) : table(t) {
		hasBeenAdded = false;
		
		if (row == 0) {
	
	
	

	

	

	

	

	

	

	

	

	

	
		exponentExists = false;
	

	
		outerScaleExists = false;
	

	
		outerScaleRMSExists = false;
	

	
	

	
		
		}
		else {
	
		
			atmPhaseCorrection = row->atmPhaseCorrection;
		
			calDataId = row->calDataId;
		
			calReductionId = row->calReductionId;
		
		
		
		
			startValidTime = row->startValidTime;
		
			endValidTime = row->endValidTime;
		
			frequencyRange = row->frequencyRange;
		
			integrationTime = row->integrationTime;
		
			numBaseLengths = row->numBaseLengths;
		
			baselineLengths = row->baselineLengths;
		
			phaseRMS = row->phaseRMS;
		
			seeing = row->seeing;
		
			seeingError = row->seeingError;
		
		
		
		
		if (row->exponentExists) {
			exponent = row->exponent;		
			exponentExists = true;
		}
		else
			exponentExists = false;
		
		if (row->outerScaleExists) {
			outerScale = row->outerScale;		
			outerScaleExists = true;
		}
		else
			outerScaleExists = false;
		
		if (row->outerScaleRMSExists) {
			outerScaleRMS = row->outerScaleRMS;		
			outerScaleRMSExists = true;
		}
		else
			outerScaleRMSExists = false;
		
		}
		
		 fromBinMethods["atmPhaseCorrection"] = &CalSeeingRow::atmPhaseCorrectionFromBin; 
		 fromBinMethods["calDataId"] = &CalSeeingRow::calDataIdFromBin; 
		 fromBinMethods["calReductionId"] = &CalSeeingRow::calReductionIdFromBin; 
		 fromBinMethods["startValidTime"] = &CalSeeingRow::startValidTimeFromBin; 
		 fromBinMethods["endValidTime"] = &CalSeeingRow::endValidTimeFromBin; 
		 fromBinMethods["frequencyRange"] = &CalSeeingRow::frequencyRangeFromBin; 
		 fromBinMethods["integrationTime"] = &CalSeeingRow::integrationTimeFromBin; 
		 fromBinMethods["numBaseLengths"] = &CalSeeingRow::numBaseLengthsFromBin; 
		 fromBinMethods["baselineLengths"] = &CalSeeingRow::baselineLengthsFromBin; 
		 fromBinMethods["phaseRMS"] = &CalSeeingRow::phaseRMSFromBin; 
		 fromBinMethods["seeing"] = &CalSeeingRow::seeingFromBin; 
		 fromBinMethods["seeingError"] = &CalSeeingRow::seeingErrorFromBin; 
			
	
		 fromBinMethods["exponent"] = &CalSeeingRow::exponentFromBin; 
		 fromBinMethods["outerScale"] = &CalSeeingRow::outerScaleFromBin; 
		 fromBinMethods["outerScaleRMS"] = &CalSeeingRow::outerScaleRMSFromBin; 
			
	}

	
	bool CalSeeingRow::compareNoAutoInc(AtmPhaseCorrectionMod::AtmPhaseCorrection atmPhaseCorrection, Tag calDataId, Tag calReductionId, ArrayTime startValidTime, ArrayTime endValidTime, std::vector<Frequency > frequencyRange, Interval integrationTime, int numBaseLengths, std::vector<Length > baselineLengths, std::vector<Angle > phaseRMS, Angle seeing, Angle seeingError) {
		bool result;
		result = true;
		
	
		
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
	

	
		
		result = result && (this->frequencyRange == frequencyRange);
		
		if (!result) return false;
	

	
		
		result = result && (this->integrationTime == integrationTime);
		
		if (!result) return false;
	

	
		
		result = result && (this->numBaseLengths == numBaseLengths);
		
		if (!result) return false;
	

	
		
		result = result && (this->baselineLengths == baselineLengths);
		
		if (!result) return false;
	

	
		
		result = result && (this->phaseRMS == phaseRMS);
		
		if (!result) return false;
	

	
		
		result = result && (this->seeing == seeing);
		
		if (!result) return false;
	

	
		
		result = result && (this->seeingError == seeingError);
		
		if (!result) return false;
	

		return result;
	}	
	
	
	
	bool CalSeeingRow::compareRequiredValue(ArrayTime startValidTime, ArrayTime endValidTime, std::vector<Frequency > frequencyRange, Interval integrationTime, int numBaseLengths, std::vector<Length > baselineLengths, std::vector<Angle > phaseRMS, Angle seeing, Angle seeingError) {
		bool result;
		result = true;
		
	
		if (!(this->startValidTime == startValidTime)) return false;
	

	
		if (!(this->endValidTime == endValidTime)) return false;
	

	
		if (!(this->frequencyRange == frequencyRange)) return false;
	

	
		if (!(this->integrationTime == integrationTime)) return false;
	

	
		if (!(this->numBaseLengths == numBaseLengths)) return false;
	

	
		if (!(this->baselineLengths == baselineLengths)) return false;
	

	
		if (!(this->phaseRMS == phaseRMS)) return false;
	

	
		if (!(this->seeing == seeing)) return false;
	

	
		if (!(this->seeingError == seeingError)) return false;
	

		return result;
	}
	
	
	/**
	 * Return true if all required attributes of the value part are equal to their homologues
	 * in x and false otherwise.
	 *

	 * @param x a pointer on the CalSeeingRow whose required attributes of the value part 

	 * will be compared with those of this.
	 * @return a boolean.
	 */
	bool CalSeeingRow::equalByRequiredValue(CalSeeingRow*  x ) {
		
			
		if (this->startValidTime != x->startValidTime) return false;
			
		if (this->endValidTime != x->endValidTime) return false;
			
		if (this->frequencyRange != x->frequencyRange) return false;
			
		if (this->integrationTime != x->integrationTime) return false;
			
		if (this->numBaseLengths != x->numBaseLengths) return false;
			
		if (this->baselineLengths != x->baselineLengths) return false;
			
		if (this->phaseRMS != x->phaseRMS) return false;
			
		if (this->seeing != x->seeing) return false;
			
		if (this->seeingError != x->seeingError) return false;
			
		
		return true;
	}	
	
/*
	 map<string, CalSeeingAttributeFromBin> CalSeeingRow::initFromBinMethods() {
		map<string, CalSeeingAttributeFromBin> result;
		
		result["atmPhaseCorrection"] = &CalSeeingRow::atmPhaseCorrectionFromBin;
		result["calDataId"] = &CalSeeingRow::calDataIdFromBin;
		result["calReductionId"] = &CalSeeingRow::calReductionIdFromBin;
		result["startValidTime"] = &CalSeeingRow::startValidTimeFromBin;
		result["endValidTime"] = &CalSeeingRow::endValidTimeFromBin;
		result["frequencyRange"] = &CalSeeingRow::frequencyRangeFromBin;
		result["integrationTime"] = &CalSeeingRow::integrationTimeFromBin;
		result["numBaseLengths"] = &CalSeeingRow::numBaseLengthsFromBin;
		result["baselineLengths"] = &CalSeeingRow::baselineLengthsFromBin;
		result["phaseRMS"] = &CalSeeingRow::phaseRMSFromBin;
		result["seeing"] = &CalSeeingRow::seeingFromBin;
		result["seeingError"] = &CalSeeingRow::seeingErrorFromBin;
		
		
		result["exponent"] = &CalSeeingRow::exponentFromBin;
		result["outerScale"] = &CalSeeingRow::outerScaleFromBin;
		result["outerScaleRMS"] = &CalSeeingRow::outerScaleRMSFromBin;
			
		
		return result;	
	}
*/	
} // End namespace asdm
 
