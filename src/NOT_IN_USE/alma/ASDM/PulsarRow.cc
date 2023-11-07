
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
 * File PulsarRow.cpp
 */
 
#include <vector>
#include <set>

#include <alma/ASDM/ASDM.h>
#include <alma/ASDM/PulsarRow.h>
#include <alma/ASDM/PulsarTable.h>
	

using asdm::ASDM;
using asdm::PulsarRow;
using asdm::PulsarTable;


#include <alma/ASDM/Parser.h>

#include <alma/ASDM/EnumerationParser.h>
#include <alma/ASDM/ASDMValuesParser.h>
 
#include <alma/ASDM/InvalidArgumentException.h>

using namespace std;

namespace asdm {
	PulsarRow::~PulsarRow() {
	}

	/**
	 * Return the table to which this row belongs.
	 */
	PulsarTable &PulsarRow::getTable() const {
		return table;
	}

	bool PulsarRow::isAdded() const {
		return hasBeenAdded;
	}	

	void PulsarRow::isAdded(bool added) {
		hasBeenAdded = added;
	}
	
#ifndef WITHOUT_ACS
	using asdmIDL::PulsarRowIDL;
#endif
	
#ifndef WITHOUT_ACS
	/**
	 * Return this row in the form of an IDL struct.
	 * @return The values of this row as a PulsarRowIDL struct.
	 */
	PulsarRowIDL *PulsarRow::toIDL() const {
		PulsarRowIDL *x = new PulsarRowIDL ();
		
		// Fill the IDL structure.
	
		
	
  		
		
		
			
		x->pulsarId = pulsarId.toIDLTag();
			
		
	

	
  		
		
		
			
		x->refTime = refTime.toIDLArrayTime();
			
		
	

	
  		
		
		
			
		x->refPulseFreq = refPulseFreq.toIDLFrequency();
			
		
	

	
  		
		
		
			
				
		x->refPhase = refPhase;
 				
 			
		
	

	
  		
		
		
			
				
		x->numBin = numBin;
 				
 			
		
	

	
  		
		
		x->numPolyExists = numPolyExists;
		
		
			
				
		x->numPoly = numPoly;
 				
 			
		
	

	
  		
		
		x->phasePolyExists = phasePolyExists;
		
		
			
		x->phasePoly.length(phasePoly.size());
		for (unsigned int i = 0; i < phasePoly.size(); ++i) {
			
				
			x->phasePoly[i] = phasePoly.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		x->timeSpanExists = timeSpanExists;
		
		
			
		x->timeSpan = timeSpan.toIDLInterval();
			
		
	

	
  		
		
		x->startPhaseBinExists = startPhaseBinExists;
		
		
			
		x->startPhaseBin.length(startPhaseBin.size());
		for (unsigned int i = 0; i < startPhaseBin.size(); ++i) {
			
				
			x->startPhaseBin[i] = startPhaseBin.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		x->endPhaseBinExists = endPhaseBinExists;
		
		
			
		x->endPhaseBin.length(endPhaseBin.size());
		for (unsigned int i = 0; i < endPhaseBin.size(); ++i) {
			
				
			x->endPhaseBin[i] = endPhaseBin.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		x->dispersionMeasureExists = dispersionMeasureExists;
		
		
			
				
		x->dispersionMeasure = dispersionMeasure;
 				
 			
		
	

	
  		
		
		x->refFrequencyExists = refFrequencyExists;
		
		
			
		x->refFrequency = refFrequency.toIDLFrequency();
			
		
	

	
	
		
		
		return x;
	
	}
	
	void PulsarRow::toIDL(asdmIDL::PulsarRowIDL& x) const {
		// Set the x's fields.
	
		
	
  		
		
		
			
		x.pulsarId = pulsarId.toIDLTag();
			
		
	

	
  		
		
		
			
		x.refTime = refTime.toIDLArrayTime();
			
		
	

	
  		
		
		
			
		x.refPulseFreq = refPulseFreq.toIDLFrequency();
			
		
	

	
  		
		
		
			
				
		x.refPhase = refPhase;
 				
 			
		
	

	
  		
		
		
			
				
		x.numBin = numBin;
 				
 			
		
	

	
  		
		
		x.numPolyExists = numPolyExists;
		
		
			
				
		x.numPoly = numPoly;
 				
 			
		
	

	
  		
		
		x.phasePolyExists = phasePolyExists;
		
		
			
		x.phasePoly.length(phasePoly.size());
		for (unsigned int i = 0; i < phasePoly.size(); ++i) {
			
				
			x.phasePoly[i] = phasePoly.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		x.timeSpanExists = timeSpanExists;
		
		
			
		x.timeSpan = timeSpan.toIDLInterval();
			
		
	

	
  		
		
		x.startPhaseBinExists = startPhaseBinExists;
		
		
			
		x.startPhaseBin.length(startPhaseBin.size());
		for (unsigned int i = 0; i < startPhaseBin.size(); ++i) {
			
				
			x.startPhaseBin[i] = startPhaseBin.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		x.endPhaseBinExists = endPhaseBinExists;
		
		
			
		x.endPhaseBin.length(endPhaseBin.size());
		for (unsigned int i = 0; i < endPhaseBin.size(); ++i) {
			
				
			x.endPhaseBin[i] = endPhaseBin.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		x.dispersionMeasureExists = dispersionMeasureExists;
		
		
			
				
		x.dispersionMeasure = dispersionMeasure;
 				
 			
		
	

	
  		
		
		x.refFrequencyExists = refFrequencyExists;
		
		
			
		x.refFrequency = refFrequency.toIDLFrequency();
			
		
	

	
	
		
	
	}
#endif
	

#ifndef WITHOUT_ACS
	/**
	 * Fill the values of this row from the IDL struct PulsarRowIDL.
	 * @param x The IDL struct containing the values used to fill this row.
	 */
	void PulsarRow::setFromIDL (PulsarRowIDL x){
		try {
		// Fill the values from x.
	
		
	
		
		
			
		setPulsarId(Tag (x.pulsarId));
			
 		
		
	

	
		
		
			
		setRefTime(ArrayTime (x.refTime));
			
 		
		
	

	
		
		
			
		setRefPulseFreq(Frequency (x.refPulseFreq));
			
 		
		
	

	
		
		
			
		setRefPhase(x.refPhase);
  			
 		
		
	

	
		
		
			
		setNumBin(x.numBin);
  			
 		
		
	

	
		
		numPolyExists = x.numPolyExists;
		if (x.numPolyExists) {
		
		
			
		setNumPoly(x.numPoly);
  			
 		
		
		}
		
	

	
		
		phasePolyExists = x.phasePolyExists;
		if (x.phasePolyExists) {
		
		
			
		phasePoly .clear();
		for (unsigned int i = 0; i <x.phasePoly.length(); ++i) {
			
			phasePoly.push_back(x.phasePoly[i]);
  			
		}
			
  		
		
		}
		
	

	
		
		timeSpanExists = x.timeSpanExists;
		if (x.timeSpanExists) {
		
		
			
		setTimeSpan(Interval (x.timeSpan));
			
 		
		
		}
		
	

	
		
		startPhaseBinExists = x.startPhaseBinExists;
		if (x.startPhaseBinExists) {
		
		
			
		startPhaseBin .clear();
		for (unsigned int i = 0; i <x.startPhaseBin.length(); ++i) {
			
			startPhaseBin.push_back(x.startPhaseBin[i]);
  			
		}
			
  		
		
		}
		
	

	
		
		endPhaseBinExists = x.endPhaseBinExists;
		if (x.endPhaseBinExists) {
		
		
			
		endPhaseBin .clear();
		for (unsigned int i = 0; i <x.endPhaseBin.length(); ++i) {
			
			endPhaseBin.push_back(x.endPhaseBin[i]);
  			
		}
			
  		
		
		}
		
	

	
		
		dispersionMeasureExists = x.dispersionMeasureExists;
		if (x.dispersionMeasureExists) {
		
		
			
		setDispersionMeasure(x.dispersionMeasure);
  			
 		
		
		}
		
	

	
		
		refFrequencyExists = x.refFrequencyExists;
		if (x.refFrequencyExists) {
		
		
			
		setRefFrequency(Frequency (x.refFrequency));
			
 		
		
		}
		
	

	
	
		
		} catch (const IllegalAccessException &err) {
			throw ConversionException (err.getMessage(),"Pulsar");
		}
	}
#endif
	
	/**
	 * Return this row in the form of an XML string.
	 * @return The values of this row as an XML string.
	 */
	string PulsarRow::toXML() const {
		string buf;
		buf.append("<row> \n");
		
	
		
  	
 		
		
		Parser::toXML(pulsarId, "pulsarId", buf);
		
		
	

  	
 		
		
		Parser::toXML(refTime, "refTime", buf);
		
		
	

  	
 		
		
		Parser::toXML(refPulseFreq, "refPulseFreq", buf);
		
		
	

  	
 		
		
		Parser::toXML(refPhase, "refPhase", buf);
		
		
	

  	
 		
		
		Parser::toXML(numBin, "numBin", buf);
		
		
	

  	
 		
		if (numPolyExists) {
		
		
		Parser::toXML(numPoly, "numPoly", buf);
		
		
		}
		
	

  	
 		
		if (phasePolyExists) {
		
		
		Parser::toXML(phasePoly, "phasePoly", buf);
		
		
		}
		
	

  	
 		
		if (timeSpanExists) {
		
		
		Parser::toXML(timeSpan, "timeSpan", buf);
		
		
		}
		
	

  	
 		
		if (startPhaseBinExists) {
		
		
		Parser::toXML(startPhaseBin, "startPhaseBin", buf);
		
		
		}
		
	

  	
 		
		if (endPhaseBinExists) {
		
		
		Parser::toXML(endPhaseBin, "endPhaseBin", buf);
		
		
		}
		
	

  	
 		
		if (dispersionMeasureExists) {
		
		
		Parser::toXML(dispersionMeasure, "dispersionMeasure", buf);
		
		
		}
		
	

  	
 		
		if (refFrequencyExists) {
		
		
		Parser::toXML(refFrequency, "refFrequency", buf);
		
		
		}
		
	

	
	
		
		
		buf.append("</row>\n");
		return buf;
	}

	/**
	 * Fill the values of this row from an XML string 
	 * that was produced by the toXML() method.
	 * @param x The XML string being used to set the values of this row.
	 */
	void PulsarRow::setFromXML (string rowDoc) {
		Parser row(rowDoc);
		string s = "";
		try {
	
		
	
  		
			
	  	setPulsarId(Parser::getTag("pulsarId","Pulsar",rowDoc));
			
		
	

	
  		
			
	  	setRefTime(Parser::getArrayTime("refTime","Pulsar",rowDoc));
			
		
	

	
  		
			
	  	setRefPulseFreq(Parser::getFrequency("refPulseFreq","Pulsar",rowDoc));
			
		
	

	
  		
			
	  	setRefPhase(Parser::getDouble("refPhase","Pulsar",rowDoc));
			
		
	

	
  		
			
	  	setNumBin(Parser::getInteger("numBin","Pulsar",rowDoc));
			
		
	

	
  		
        if (row.isStr("<numPoly>")) {
			
	  		setNumPoly(Parser::getInteger("numPoly","Pulsar",rowDoc));
			
		}
 		
	

	
  		
        if (row.isStr("<phasePoly>")) {
			
								
	  		setPhasePoly(Parser::get1DDouble("phasePoly","Pulsar",rowDoc));
	  			
	  		
		}
 		
	

	
  		
        if (row.isStr("<timeSpan>")) {
			
	  		setTimeSpan(Parser::getInterval("timeSpan","Pulsar",rowDoc));
			
		}
 		
	

	
  		
        if (row.isStr("<startPhaseBin>")) {
			
								
	  		setStartPhaseBin(Parser::get1DFloat("startPhaseBin","Pulsar",rowDoc));
	  			
	  		
		}
 		
	

	
  		
        if (row.isStr("<endPhaseBin>")) {
			
								
	  		setEndPhaseBin(Parser::get1DFloat("endPhaseBin","Pulsar",rowDoc));
	  			
	  		
		}
 		
	

	
  		
        if (row.isStr("<dispersionMeasure>")) {
			
	  		setDispersionMeasure(Parser::getDouble("dispersionMeasure","Pulsar",rowDoc));
			
		}
 		
	

	
  		
        if (row.isStr("<refFrequency>")) {
			
	  		setRefFrequency(Parser::getFrequency("refFrequency","Pulsar",rowDoc));
			
		}
 		
	

	
	
		
		} catch (const IllegalAccessException &err) {
			throw ConversionException (err.getMessage(),"Pulsar");
		}
	}
	
	void PulsarRow::toBin(EndianOSStream& eoss) {
	
	
	
	
		
	pulsarId.toBin(eoss);
		
	

	
	
		
	refTime.toBin(eoss);
		
	

	
	
		
	refPulseFreq.toBin(eoss);
		
	

	
	
		
						
			eoss.writeDouble(refPhase);
				
		
	

	
	
		
						
			eoss.writeInt(numBin);
				
		
	


	
	
	eoss.writeBoolean(numPolyExists);
	if (numPolyExists) {
	
	
	
		
						
			eoss.writeInt(numPoly);
				
		
	

	}

	eoss.writeBoolean(phasePolyExists);
	if (phasePolyExists) {
	
	
	
		
		
			
		eoss.writeInt((int) phasePoly.size());
		for (unsigned int i = 0; i < phasePoly.size(); i++)
				
			eoss.writeDouble(phasePoly.at(i));
				
				
						
		
	

	}

	eoss.writeBoolean(timeSpanExists);
	if (timeSpanExists) {
	
	
	
		
	timeSpan.toBin(eoss);
		
	

	}

	eoss.writeBoolean(startPhaseBinExists);
	if (startPhaseBinExists) {
	
	
	
		
		
			
		eoss.writeInt((int) startPhaseBin.size());
		for (unsigned int i = 0; i < startPhaseBin.size(); i++)
				
			eoss.writeFloat(startPhaseBin.at(i));
				
				
						
		
	

	}

	eoss.writeBoolean(endPhaseBinExists);
	if (endPhaseBinExists) {
	
	
	
		
		
			
		eoss.writeInt((int) endPhaseBin.size());
		for (unsigned int i = 0; i < endPhaseBin.size(); i++)
				
			eoss.writeFloat(endPhaseBin.at(i));
				
				
						
		
	

	}

	eoss.writeBoolean(dispersionMeasureExists);
	if (dispersionMeasureExists) {
	
	
	
		
						
			eoss.writeDouble(dispersionMeasure);
				
		
	

	}

	eoss.writeBoolean(refFrequencyExists);
	if (refFrequencyExists) {
	
	
	
		
	refFrequency.toBin(eoss);
		
	

	}

	}
	
void PulsarRow::pulsarIdFromBin(EndianIStream& eis) {
		
	
		
		
		pulsarId =  Tag::fromBin(eis);
		
	
	
}
void PulsarRow::refTimeFromBin(EndianIStream& eis) {
		
	
		
		
		refTime =  ArrayTime::fromBin(eis);
		
	
	
}
void PulsarRow::refPulseFreqFromBin(EndianIStream& eis) {
		
	
		
		
		refPulseFreq =  Frequency::fromBin(eis);
		
	
	
}
void PulsarRow::refPhaseFromBin(EndianIStream& eis) {
		
	
	
		
			
		refPhase =  eis.readDouble();
			
		
	
	
}
void PulsarRow::numBinFromBin(EndianIStream& eis) {
		
	
	
		
			
		numBin =  eis.readInt();
			
		
	
	
}

void PulsarRow::numPolyFromBin(EndianIStream& eis) {
		
	numPolyExists = eis.readBoolean();
	if (numPolyExists) {
		
	
	
		
			
		numPoly =  eis.readInt();
			
		
	

	}
	
}
void PulsarRow::phasePolyFromBin(EndianIStream& eis) {
		
	phasePolyExists = eis.readBoolean();
	if (phasePolyExists) {
		
	
	
		
			
	
		phasePoly.clear();
		
		unsigned int phasePolyDim1 = eis.readInt();
		for (unsigned int  i = 0 ; i < phasePolyDim1; i++)
			
			phasePoly.push_back(eis.readDouble());
			
	

		
	

	}
	
}
void PulsarRow::timeSpanFromBin(EndianIStream& eis) {
		
	timeSpanExists = eis.readBoolean();
	if (timeSpanExists) {
		
	
		
		
		timeSpan =  Interval::fromBin(eis);
		
	

	}
	
}
void PulsarRow::startPhaseBinFromBin(EndianIStream& eis) {
		
	startPhaseBinExists = eis.readBoolean();
	if (startPhaseBinExists) {
		
	
	
		
			
	
		startPhaseBin.clear();
		
		unsigned int startPhaseBinDim1 = eis.readInt();
		for (unsigned int  i = 0 ; i < startPhaseBinDim1; i++)
			
			startPhaseBin.push_back(eis.readFloat());
			
	

		
	

	}
	
}
void PulsarRow::endPhaseBinFromBin(EndianIStream& eis) {
		
	endPhaseBinExists = eis.readBoolean();
	if (endPhaseBinExists) {
		
	
	
		
			
	
		endPhaseBin.clear();
		
		unsigned int endPhaseBinDim1 = eis.readInt();
		for (unsigned int  i = 0 ; i < endPhaseBinDim1; i++)
			
			endPhaseBin.push_back(eis.readFloat());
			
	

		
	

	}
	
}
void PulsarRow::dispersionMeasureFromBin(EndianIStream& eis) {
		
	dispersionMeasureExists = eis.readBoolean();
	if (dispersionMeasureExists) {
		
	
	
		
			
		dispersionMeasure =  eis.readDouble();
			
		
	

	}
	
}
void PulsarRow::refFrequencyFromBin(EndianIStream& eis) {
		
	refFrequencyExists = eis.readBoolean();
	if (refFrequencyExists) {
		
	
		
		
		refFrequency =  Frequency::fromBin(eis);
		
	

	}
	
}
	
	
	PulsarRow* PulsarRow::fromBin(EndianIStream& eis, PulsarTable& table, const vector<string>& attributesSeq) {
		PulsarRow* row = new  PulsarRow(table);
		
		map<string, PulsarAttributeFromBin>::iterator iter ;
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
					throw ConversionException("There is not method to read an attribute '"+attributesSeq.at(i)+"'.", "PulsarTable");
			}
				
		}				
		return row;
	}

	//
	// A collection of methods to set the value of the attributes from their textual value in the XML representation
	// of one row.
	//
	
	// Convert a string into an Tag 
	void PulsarRow::pulsarIdFromText(const string & s) {
		 
          
		pulsarId = ASDMValuesParser::parse<Tag>(s);
          
		
	}
	
	
	// Convert a string into an ArrayTime 
	void PulsarRow::refTimeFromText(const string & s) {
		 
          
		refTime = ASDMValuesParser::parse<ArrayTime>(s);
          
		
	}
	
	
	// Convert a string into an Frequency 
	void PulsarRow::refPulseFreqFromText(const string & s) {
		 
          
		refPulseFreq = ASDMValuesParser::parse<Frequency>(s);
          
		
	}
	
	
	// Convert a string into an double 
	void PulsarRow::refPhaseFromText(const string & s) {
		 
          
		refPhase = ASDMValuesParser::parse<double>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void PulsarRow::numBinFromText(const string & s) {
		 
          
		numBin = ASDMValuesParser::parse<int>(s);
          
		
	}
	

	
	// Convert a string into an int 
	void PulsarRow::numPolyFromText(const string & s) {
		numPolyExists = true;
		 
          
		numPoly = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	// Convert a string into an double 
	void PulsarRow::phasePolyFromText(const string & s) {
		phasePolyExists = true;
		 
          
		phasePoly = ASDMValuesParser::parse1D<double>(s);
          
		
	}
	
	
	// Convert a string into an Interval 
	void PulsarRow::timeSpanFromText(const string & s) {
		timeSpanExists = true;
		 
          
		timeSpan = ASDMValuesParser::parse<Interval>(s);
          
		
	}
	
	
	// Convert a string into an float 
	void PulsarRow::startPhaseBinFromText(const string & s) {
		startPhaseBinExists = true;
		 
          
		startPhaseBin = ASDMValuesParser::parse1D<float>(s);
          
		
	}
	
	
	// Convert a string into an float 
	void PulsarRow::endPhaseBinFromText(const string & s) {
		endPhaseBinExists = true;
		 
          
		endPhaseBin = ASDMValuesParser::parse1D<float>(s);
          
		
	}
	
	
	// Convert a string into an double 
	void PulsarRow::dispersionMeasureFromText(const string & s) {
		dispersionMeasureExists = true;
		 
          
		dispersionMeasure = ASDMValuesParser::parse<double>(s);
          
		
	}
	
	
	// Convert a string into an Frequency 
	void PulsarRow::refFrequencyFromText(const string & s) {
		refFrequencyExists = true;
		 
          
		refFrequency = ASDMValuesParser::parse<Frequency>(s);
          
		
	}
	
	
	
	void PulsarRow::fromText(const std::string& attributeName, const std::string&  t) {
		map<string, PulsarAttributeFromText>::iterator iter;
		if ((iter = fromTextMethods.find(attributeName)) == fromTextMethods.end())
			throw ConversionException("I do not know what to do with '"+attributeName+"' and its content '"+t+"' (while parsing an XML document)", "PulsarTable");
		(this->*(iter->second))(t);
	}
			
	////////////////////////////////////////////////
	// Intrinsic Table Attributes getters/setters //
	////////////////////////////////////////////////
	
	

	
 	/**
 	 * Get pulsarId.
 	 * @return pulsarId as Tag
 	 */
 	Tag PulsarRow::getPulsarId() const {
	
  		return pulsarId;
 	}

 	/**
 	 * Set pulsarId with the specified Tag.
 	 * @param pulsarId The Tag value to which pulsarId is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void PulsarRow::setPulsarId (Tag pulsarId)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("pulsarId", "Pulsar");
		
  		}
  	
 		this->pulsarId = pulsarId;
	
 	}
	
	

	

	
 	/**
 	 * Get refTime.
 	 * @return refTime as ArrayTime
 	 */
 	ArrayTime PulsarRow::getRefTime() const {
	
  		return refTime;
 	}

 	/**
 	 * Set refTime with the specified ArrayTime.
 	 * @param refTime The ArrayTime value to which refTime is to be set.
 	 
 	
 		
 	 */
 	void PulsarRow::setRefTime (ArrayTime refTime)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->refTime = refTime;
	
 	}
	
	

	

	
 	/**
 	 * Get refPulseFreq.
 	 * @return refPulseFreq as Frequency
 	 */
 	Frequency PulsarRow::getRefPulseFreq() const {
	
  		return refPulseFreq;
 	}

 	/**
 	 * Set refPulseFreq with the specified Frequency.
 	 * @param refPulseFreq The Frequency value to which refPulseFreq is to be set.
 	 
 	
 		
 	 */
 	void PulsarRow::setRefPulseFreq (Frequency refPulseFreq)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->refPulseFreq = refPulseFreq;
	
 	}
	
	

	

	
 	/**
 	 * Get refPhase.
 	 * @return refPhase as double
 	 */
 	double PulsarRow::getRefPhase() const {
	
  		return refPhase;
 	}

 	/**
 	 * Set refPhase with the specified double.
 	 * @param refPhase The double value to which refPhase is to be set.
 	 
 	
 		
 	 */
 	void PulsarRow::setRefPhase (double refPhase)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->refPhase = refPhase;
	
 	}
	
	

	

	
 	/**
 	 * Get numBin.
 	 * @return numBin as int
 	 */
 	int PulsarRow::getNumBin() const {
	
  		return numBin;
 	}

 	/**
 	 * Set numBin with the specified int.
 	 * @param numBin The int value to which numBin is to be set.
 	 
 	
 		
 	 */
 	void PulsarRow::setNumBin (int numBin)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->numBin = numBin;
	
 	}
	
	

	
	/**
	 * The attribute numPoly is optional. Return true if this attribute exists.
	 * @return true if and only if the numPoly attribute exists. 
	 */
	bool PulsarRow::isNumPolyExists() const {
		return numPolyExists;
	}
	

	
 	/**
 	 * Get numPoly, which is optional.
 	 * @return numPoly as int
 	 * @throw IllegalAccessException If numPoly does not exist.
 	 */
 	int PulsarRow::getNumPoly() const  {
		if (!numPolyExists) {
			throw IllegalAccessException("numPoly", "Pulsar");
		}
	
  		return numPoly;
 	}

 	/**
 	 * Set numPoly with the specified int.
 	 * @param numPoly The int value to which numPoly is to be set.
 	 
 	
 	 */
 	void PulsarRow::setNumPoly (int numPoly) {
	
 		this->numPoly = numPoly;
	
		numPolyExists = true;
	
 	}
	
	
	/**
	 * Mark numPoly, which is an optional field, as non-existent.
	 */
	void PulsarRow::clearNumPoly () {
		numPolyExists = false;
	}
	

	
	/**
	 * The attribute phasePoly is optional. Return true if this attribute exists.
	 * @return true if and only if the phasePoly attribute exists. 
	 */
	bool PulsarRow::isPhasePolyExists() const {
		return phasePolyExists;
	}
	

	
 	/**
 	 * Get phasePoly, which is optional.
 	 * @return phasePoly as std::vector<double >
 	 * @throw IllegalAccessException If phasePoly does not exist.
 	 */
 	std::vector<double > PulsarRow::getPhasePoly() const  {
		if (!phasePolyExists) {
			throw IllegalAccessException("phasePoly", "Pulsar");
		}
	
  		return phasePoly;
 	}

 	/**
 	 * Set phasePoly with the specified std::vector<double >.
 	 * @param phasePoly The std::vector<double > value to which phasePoly is to be set.
 	 
 	
 	 */
 	void PulsarRow::setPhasePoly (std::vector<double > phasePoly) {
	
 		this->phasePoly = phasePoly;
	
		phasePolyExists = true;
	
 	}
	
	
	/**
	 * Mark phasePoly, which is an optional field, as non-existent.
	 */
	void PulsarRow::clearPhasePoly () {
		phasePolyExists = false;
	}
	

	
	/**
	 * The attribute timeSpan is optional. Return true if this attribute exists.
	 * @return true if and only if the timeSpan attribute exists. 
	 */
	bool PulsarRow::isTimeSpanExists() const {
		return timeSpanExists;
	}
	

	
 	/**
 	 * Get timeSpan, which is optional.
 	 * @return timeSpan as Interval
 	 * @throw IllegalAccessException If timeSpan does not exist.
 	 */
 	Interval PulsarRow::getTimeSpan() const  {
		if (!timeSpanExists) {
			throw IllegalAccessException("timeSpan", "Pulsar");
		}
	
  		return timeSpan;
 	}

 	/**
 	 * Set timeSpan with the specified Interval.
 	 * @param timeSpan The Interval value to which timeSpan is to be set.
 	 
 	
 	 */
 	void PulsarRow::setTimeSpan (Interval timeSpan) {
	
 		this->timeSpan = timeSpan;
	
		timeSpanExists = true;
	
 	}
	
	
	/**
	 * Mark timeSpan, which is an optional field, as non-existent.
	 */
	void PulsarRow::clearTimeSpan () {
		timeSpanExists = false;
	}
	

	
	/**
	 * The attribute startPhaseBin is optional. Return true if this attribute exists.
	 * @return true if and only if the startPhaseBin attribute exists. 
	 */
	bool PulsarRow::isStartPhaseBinExists() const {
		return startPhaseBinExists;
	}
	

	
 	/**
 	 * Get startPhaseBin, which is optional.
 	 * @return startPhaseBin as std::vector<float >
 	 * @throw IllegalAccessException If startPhaseBin does not exist.
 	 */
 	std::vector<float > PulsarRow::getStartPhaseBin() const  {
		if (!startPhaseBinExists) {
			throw IllegalAccessException("startPhaseBin", "Pulsar");
		}
	
  		return startPhaseBin;
 	}

 	/**
 	 * Set startPhaseBin with the specified std::vector<float >.
 	 * @param startPhaseBin The std::vector<float > value to which startPhaseBin is to be set.
 	 
 	
 	 */
 	void PulsarRow::setStartPhaseBin (std::vector<float > startPhaseBin) {
	
 		this->startPhaseBin = startPhaseBin;
	
		startPhaseBinExists = true;
	
 	}
	
	
	/**
	 * Mark startPhaseBin, which is an optional field, as non-existent.
	 */
	void PulsarRow::clearStartPhaseBin () {
		startPhaseBinExists = false;
	}
	

	
	/**
	 * The attribute endPhaseBin is optional. Return true if this attribute exists.
	 * @return true if and only if the endPhaseBin attribute exists. 
	 */
	bool PulsarRow::isEndPhaseBinExists() const {
		return endPhaseBinExists;
	}
	

	
 	/**
 	 * Get endPhaseBin, which is optional.
 	 * @return endPhaseBin as std::vector<float >
 	 * @throw IllegalAccessException If endPhaseBin does not exist.
 	 */
 	std::vector<float > PulsarRow::getEndPhaseBin() const  {
		if (!endPhaseBinExists) {
			throw IllegalAccessException("endPhaseBin", "Pulsar");
		}
	
  		return endPhaseBin;
 	}

 	/**
 	 * Set endPhaseBin with the specified std::vector<float >.
 	 * @param endPhaseBin The std::vector<float > value to which endPhaseBin is to be set.
 	 
 	
 	 */
 	void PulsarRow::setEndPhaseBin (std::vector<float > endPhaseBin) {
	
 		this->endPhaseBin = endPhaseBin;
	
		endPhaseBinExists = true;
	
 	}
	
	
	/**
	 * Mark endPhaseBin, which is an optional field, as non-existent.
	 */
	void PulsarRow::clearEndPhaseBin () {
		endPhaseBinExists = false;
	}
	

	
	/**
	 * The attribute dispersionMeasure is optional. Return true if this attribute exists.
	 * @return true if and only if the dispersionMeasure attribute exists. 
	 */
	bool PulsarRow::isDispersionMeasureExists() const {
		return dispersionMeasureExists;
	}
	

	
 	/**
 	 * Get dispersionMeasure, which is optional.
 	 * @return dispersionMeasure as double
 	 * @throw IllegalAccessException If dispersionMeasure does not exist.
 	 */
 	double PulsarRow::getDispersionMeasure() const  {
		if (!dispersionMeasureExists) {
			throw IllegalAccessException("dispersionMeasure", "Pulsar");
		}
	
  		return dispersionMeasure;
 	}

 	/**
 	 * Set dispersionMeasure with the specified double.
 	 * @param dispersionMeasure The double value to which dispersionMeasure is to be set.
 	 
 	
 	 */
 	void PulsarRow::setDispersionMeasure (double dispersionMeasure) {
	
 		this->dispersionMeasure = dispersionMeasure;
	
		dispersionMeasureExists = true;
	
 	}
	
	
	/**
	 * Mark dispersionMeasure, which is an optional field, as non-existent.
	 */
	void PulsarRow::clearDispersionMeasure () {
		dispersionMeasureExists = false;
	}
	

	
	/**
	 * The attribute refFrequency is optional. Return true if this attribute exists.
	 * @return true if and only if the refFrequency attribute exists. 
	 */
	bool PulsarRow::isRefFrequencyExists() const {
		return refFrequencyExists;
	}
	

	
 	/**
 	 * Get refFrequency, which is optional.
 	 * @return refFrequency as Frequency
 	 * @throw IllegalAccessException If refFrequency does not exist.
 	 */
 	Frequency PulsarRow::getRefFrequency() const  {
		if (!refFrequencyExists) {
			throw IllegalAccessException("refFrequency", "Pulsar");
		}
	
  		return refFrequency;
 	}

 	/**
 	 * Set refFrequency with the specified Frequency.
 	 * @param refFrequency The Frequency value to which refFrequency is to be set.
 	 
 	
 	 */
 	void PulsarRow::setRefFrequency (Frequency refFrequency) {
	
 		this->refFrequency = refFrequency;
	
		refFrequencyExists = true;
	
 	}
	
	
	/**
	 * Mark refFrequency, which is an optional field, as non-existent.
	 */
	void PulsarRow::clearRefFrequency () {
		refFrequencyExists = false;
	}
	

	
	///////////////////////////////////////////////
	// Extrinsic Table Attributes getters/setters//
	///////////////////////////////////////////////
	

	//////////////////////////////////////
	// Links Attributes getters/setters //
	//////////////////////////////////////
	
	
	/**
	 * Create a PulsarRow.
	 * <p>
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 */ 
	PulsarRow::PulsarRow (PulsarTable &t) : table(t) {
		hasBeenAdded = false;
		
	
	

	

	

	

	

	
		numPolyExists = false;
	

	
		phasePolyExists = false;
	

	
		timeSpanExists = false;
	

	
		startPhaseBinExists = false;
	

	
		endPhaseBinExists = false;
	

	
		dispersionMeasureExists = false;
	

	
		refFrequencyExists = false;
	

	
	
	
	
	

	

	

	

	

	

	

	

	

	

	

	

	
	
	 fromBinMethods["pulsarId"] = &PulsarRow::pulsarIdFromBin; 
	 fromBinMethods["refTime"] = &PulsarRow::refTimeFromBin; 
	 fromBinMethods["refPulseFreq"] = &PulsarRow::refPulseFreqFromBin; 
	 fromBinMethods["refPhase"] = &PulsarRow::refPhaseFromBin; 
	 fromBinMethods["numBin"] = &PulsarRow::numBinFromBin; 
		
	
	 fromBinMethods["numPoly"] = &PulsarRow::numPolyFromBin; 
	 fromBinMethods["phasePoly"] = &PulsarRow::phasePolyFromBin; 
	 fromBinMethods["timeSpan"] = &PulsarRow::timeSpanFromBin; 
	 fromBinMethods["startPhaseBin"] = &PulsarRow::startPhaseBinFromBin; 
	 fromBinMethods["endPhaseBin"] = &PulsarRow::endPhaseBinFromBin; 
	 fromBinMethods["dispersionMeasure"] = &PulsarRow::dispersionMeasureFromBin; 
	 fromBinMethods["refFrequency"] = &PulsarRow::refFrequencyFromBin; 
	
	
	
	
				 
	fromTextMethods["pulsarId"] = &PulsarRow::pulsarIdFromText;
		 
	
				 
	fromTextMethods["refTime"] = &PulsarRow::refTimeFromText;
		 
	
				 
	fromTextMethods["refPulseFreq"] = &PulsarRow::refPulseFreqFromText;
		 
	
				 
	fromTextMethods["refPhase"] = &PulsarRow::refPhaseFromText;
		 
	
				 
	fromTextMethods["numBin"] = &PulsarRow::numBinFromText;
		 
	

	 
				
	fromTextMethods["numPoly"] = &PulsarRow::numPolyFromText;
		 	
	 
				
	fromTextMethods["phasePoly"] = &PulsarRow::phasePolyFromText;
		 	
	 
				
	fromTextMethods["timeSpan"] = &PulsarRow::timeSpanFromText;
		 	
	 
				
	fromTextMethods["startPhaseBin"] = &PulsarRow::startPhaseBinFromText;
		 	
	 
				
	fromTextMethods["endPhaseBin"] = &PulsarRow::endPhaseBinFromText;
		 	
	 
				
	fromTextMethods["dispersionMeasure"] = &PulsarRow::dispersionMeasureFromText;
		 	
	 
				
	fromTextMethods["refFrequency"] = &PulsarRow::refFrequencyFromText;
		 	
		
	}
	
	PulsarRow::PulsarRow (PulsarTable &t, PulsarRow *row) : table(t) {
		hasBeenAdded = false;
		
		if (row == 0) {
	
	
	

	

	

	

	

	
		numPolyExists = false;
	

	
		phasePolyExists = false;
	

	
		timeSpanExists = false;
	

	
		startPhaseBinExists = false;
	

	
		endPhaseBinExists = false;
	

	
		dispersionMeasureExists = false;
	

	
		refFrequencyExists = false;
	

			
		}
		else {
	
		
			pulsarId = row->pulsarId;
		
		
		
		
			refTime = row->refTime;
		
			refPulseFreq = row->refPulseFreq;
		
			refPhase = row->refPhase;
		
			numBin = row->numBin;
		
		
		
		
		if (row->numPolyExists) {
			numPoly = row->numPoly;		
			numPolyExists = true;
		}
		else
			numPolyExists = false;
		
		if (row->phasePolyExists) {
			phasePoly = row->phasePoly;		
			phasePolyExists = true;
		}
		else
			phasePolyExists = false;
		
		if (row->timeSpanExists) {
			timeSpan = row->timeSpan;		
			timeSpanExists = true;
		}
		else
			timeSpanExists = false;
		
		if (row->startPhaseBinExists) {
			startPhaseBin = row->startPhaseBin;		
			startPhaseBinExists = true;
		}
		else
			startPhaseBinExists = false;
		
		if (row->endPhaseBinExists) {
			endPhaseBin = row->endPhaseBin;		
			endPhaseBinExists = true;
		}
		else
			endPhaseBinExists = false;
		
		if (row->dispersionMeasureExists) {
			dispersionMeasure = row->dispersionMeasure;		
			dispersionMeasureExists = true;
		}
		else
			dispersionMeasureExists = false;
		
		if (row->refFrequencyExists) {
			refFrequency = row->refFrequency;		
			refFrequencyExists = true;
		}
		else
			refFrequencyExists = false;
		
		}
		
		 fromBinMethods["pulsarId"] = &PulsarRow::pulsarIdFromBin; 
		 fromBinMethods["refTime"] = &PulsarRow::refTimeFromBin; 
		 fromBinMethods["refPulseFreq"] = &PulsarRow::refPulseFreqFromBin; 
		 fromBinMethods["refPhase"] = &PulsarRow::refPhaseFromBin; 
		 fromBinMethods["numBin"] = &PulsarRow::numBinFromBin; 
			
	
		 fromBinMethods["numPoly"] = &PulsarRow::numPolyFromBin; 
		 fromBinMethods["phasePoly"] = &PulsarRow::phasePolyFromBin; 
		 fromBinMethods["timeSpan"] = &PulsarRow::timeSpanFromBin; 
		 fromBinMethods["startPhaseBin"] = &PulsarRow::startPhaseBinFromBin; 
		 fromBinMethods["endPhaseBin"] = &PulsarRow::endPhaseBinFromBin; 
		 fromBinMethods["dispersionMeasure"] = &PulsarRow::dispersionMeasureFromBin; 
		 fromBinMethods["refFrequency"] = &PulsarRow::refFrequencyFromBin; 
			
	}

	
	bool PulsarRow::compareNoAutoInc(ArrayTime refTime, Frequency refPulseFreq, double refPhase, int numBin) {
		bool result;
		result = true;
		
	
		
		result = result && (this->refTime == refTime);
		
		if (!result) return false;
	

	
		
		result = result && (this->refPulseFreq == refPulseFreq);
		
		if (!result) return false;
	

	
		
		result = result && (this->refPhase == refPhase);
		
		if (!result) return false;
	

	
		
		result = result && (this->numBin == numBin);
		
		if (!result) return false;
	

		return result;
	}	
	
	
	
	bool PulsarRow::compareRequiredValue(ArrayTime refTime, Frequency refPulseFreq, double refPhase, int numBin) {
		bool result;
		result = true;
		
	
		if (!(this->refTime == refTime)) return false;
	

	
		if (!(this->refPulseFreq == refPulseFreq)) return false;
	

	
		if (!(this->refPhase == refPhase)) return false;
	

	
		if (!(this->numBin == numBin)) return false;
	

		return result;
	}
	
	
	/**
	 * Return true if all required attributes of the value part are equal to their homologues
	 * in x and false otherwise.
	 *

	 * @param x a pointer on the PulsarRow whose required attributes of the value part 

	 * will be compared with those of this.
	 * @return a boolean.
	 */
	bool PulsarRow::equalByRequiredValue(PulsarRow*  x ) {
		
			
		if (this->refTime != x->refTime) return false;
			
		if (this->refPulseFreq != x->refPulseFreq) return false;
			
		if (this->refPhase != x->refPhase) return false;
			
		if (this->numBin != x->numBin) return false;
			
		
		return true;
	}	
	
/*
	 map<string, PulsarAttributeFromBin> PulsarRow::initFromBinMethods() {
		map<string, PulsarAttributeFromBin> result;
		
		result["pulsarId"] = &PulsarRow::pulsarIdFromBin;
		result["refTime"] = &PulsarRow::refTimeFromBin;
		result["refPulseFreq"] = &PulsarRow::refPulseFreqFromBin;
		result["refPhase"] = &PulsarRow::refPhaseFromBin;
		result["numBin"] = &PulsarRow::numBinFromBin;
		
		
		result["numPoly"] = &PulsarRow::numPolyFromBin;
		result["phasePoly"] = &PulsarRow::phasePolyFromBin;
		result["timeSpan"] = &PulsarRow::timeSpanFromBin;
		result["startPhaseBin"] = &PulsarRow::startPhaseBinFromBin;
		result["endPhaseBin"] = &PulsarRow::endPhaseBinFromBin;
		result["dispersionMeasure"] = &PulsarRow::dispersionMeasureFromBin;
		result["refFrequency"] = &PulsarRow::refFrequencyFromBin;
			
		
		return result;	
	}
*/	
} // End namespace asdm
 
