
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
 * File GainTrackingRow.cpp
 */
 
#include <vector>
#include <set>

#include <alma/ASDM/ASDM.h>
#include <alma/ASDM/GainTrackingRow.h>
#include <alma/ASDM/GainTrackingTable.h>

#include <alma/ASDM/AntennaTable.h>
#include <alma/ASDM/AntennaRow.h>

#include <alma/ASDM/SpectralWindowTable.h>
#include <alma/ASDM/SpectralWindowRow.h>

#include <alma/ASDM/FeedTable.h>
#include <alma/ASDM/FeedRow.h>
	

using asdm::ASDM;
using asdm::GainTrackingRow;
using asdm::GainTrackingTable;

using asdm::AntennaTable;
using asdm::AntennaRow;

using asdm::SpectralWindowTable;
using asdm::SpectralWindowRow;

using asdm::FeedTable;
using asdm::FeedRow;


#include <alma/ASDM/Parser.h>

#include <alma/ASDM/EnumerationParser.h>
#include <alma/ASDM/ASDMValuesParser.h>
 
#include <alma/ASDM/InvalidArgumentException.h>

using namespace std;

namespace asdm {
	GainTrackingRow::~GainTrackingRow() {
	}

	/**
	 * Return the table to which this row belongs.
	 */
	GainTrackingTable &GainTrackingRow::getTable() const {
		return table;
	}

	bool GainTrackingRow::isAdded() const {
		return hasBeenAdded;
	}	

	void GainTrackingRow::isAdded(bool added) {
		hasBeenAdded = added;
	}
	
#ifndef WITHOUT_ACS
	using asdmIDL::GainTrackingRowIDL;
#endif
	
#ifndef WITHOUT_ACS
	/**
	 * Return this row in the form of an IDL struct.
	 * @return The values of this row as a GainTrackingRowIDL struct.
	 */
	GainTrackingRowIDL *GainTrackingRow::toIDL() const {
		GainTrackingRowIDL *x = new GainTrackingRowIDL ();
		
		// Fill the IDL structure.
	
		
	
  		
		
		
			
		x->timeInterval = timeInterval.toIDLArrayTimeInterval();
			
		
	

	
  		
		
		
			
				
		x->numReceptor = numReceptor;
 				
 			
		
	

	
  		
		
		
			
		x->attenuator.length(attenuator.size());
		for (unsigned int i = 0; i < attenuator.size(); ++i) {
			
				
			x->attenuator[i] = attenuator.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		
			
		x->polarizationType.length(polarizationType.size());
		for (unsigned int i = 0; i < polarizationType.size(); ++i) {
			
				
			x->polarizationType[i] = polarizationType.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		x->samplingLevelExists = samplingLevelExists;
		
		
			
				
		x->samplingLevel = samplingLevel;
 				
 			
		
	

	
  		
		
		x->numAttFreqExists = numAttFreqExists;
		
		
			
				
		x->numAttFreq = numAttFreq;
 				
 			
		
	

	
  		
		
		x->attFreqExists = attFreqExists;
		
		
			
		x->attFreq.length(attFreq.size());
		for (unsigned int i = 0; i < attFreq.size(); ++i) {
			
				
			x->attFreq[i] = attFreq.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		x->attSpectrumExists = attSpectrumExists;
		
		
			
		x->attSpectrum.length(attSpectrum.size());
		for (unsigned int i = 0; i < attSpectrum.size(); ++i) {
			
			x->attSpectrum[i] = attSpectrum.at(i).toIDLComplex();
			
	 	}
			
		
	

	
	
		
	
  	
 		
		
	 	
			
		x->antennaId = antennaId.toIDLTag();
			
	 	 		
  	

	
  	
 		
		
	 	
			
				
		x->feedId = feedId;
 				
 			
	 	 		
  	

	
  	
 		
		
	 	
			
		x->spectralWindowId = spectralWindowId.toIDLTag();
			
	 	 		
  	

	
		
	

	

	

		
		return x;
	
	}
	
	void GainTrackingRow::toIDL(asdmIDL::GainTrackingRowIDL& x) const {
		// Set the x's fields.
	
		
	
  		
		
		
			
		x.timeInterval = timeInterval.toIDLArrayTimeInterval();
			
		
	

	
  		
		
		
			
				
		x.numReceptor = numReceptor;
 				
 			
		
	

	
  		
		
		
			
		x.attenuator.length(attenuator.size());
		for (unsigned int i = 0; i < attenuator.size(); ++i) {
			
				
			x.attenuator[i] = attenuator.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		
			
		x.polarizationType.length(polarizationType.size());
		for (unsigned int i = 0; i < polarizationType.size(); ++i) {
			
				
			x.polarizationType[i] = polarizationType.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		x.samplingLevelExists = samplingLevelExists;
		
		
			
				
		x.samplingLevel = samplingLevel;
 				
 			
		
	

	
  		
		
		x.numAttFreqExists = numAttFreqExists;
		
		
			
				
		x.numAttFreq = numAttFreq;
 				
 			
		
	

	
  		
		
		x.attFreqExists = attFreqExists;
		
		
			
		x.attFreq.length(attFreq.size());
		for (unsigned int i = 0; i < attFreq.size(); ++i) {
			
				
			x.attFreq[i] = attFreq.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		x.attSpectrumExists = attSpectrumExists;
		
		
			
		x.attSpectrum.length(attSpectrum.size());
		for (unsigned int i = 0; i < attSpectrum.size(); ++i) {
			
			x.attSpectrum[i] = attSpectrum.at(i).toIDLComplex();
			
	 	}
			
		
	

	
	
		
	
  	
 		
		
	 	
			
		x.antennaId = antennaId.toIDLTag();
			
	 	 		
  	

	
  	
 		
		
	 	
			
				
		x.feedId = feedId;
 				
 			
	 	 		
  	

	
  	
 		
		
	 	
			
		x.spectralWindowId = spectralWindowId.toIDLTag();
			
	 	 		
  	

	
		
	

	

	

	
	}
#endif
	

#ifndef WITHOUT_ACS
	/**
	 * Fill the values of this row from the IDL struct GainTrackingRowIDL.
	 * @param x The IDL struct containing the values used to fill this row.
	 */
	void GainTrackingRow::setFromIDL (GainTrackingRowIDL x){
		try {
		// Fill the values from x.
	
		
	
		
		
			
		setTimeInterval(ArrayTimeInterval (x.timeInterval));
			
 		
		
	

	
		
		
			
		setNumReceptor(x.numReceptor);
  			
 		
		
	

	
		
		
			
		attenuator .clear();
		for (unsigned int i = 0; i <x.attenuator.length(); ++i) {
			
			attenuator.push_back(x.attenuator[i]);
  			
		}
			
  		
		
	

	
		
		
			
		polarizationType .clear();
		for (unsigned int i = 0; i <x.polarizationType.length(); ++i) {
			
			polarizationType.push_back(x.polarizationType[i]);
  			
		}
			
  		
		
	

	
		
		samplingLevelExists = x.samplingLevelExists;
		if (x.samplingLevelExists) {
		
		
			
		setSamplingLevel(x.samplingLevel);
  			
 		
		
		}
		
	

	
		
		numAttFreqExists = x.numAttFreqExists;
		if (x.numAttFreqExists) {
		
		
			
		setNumAttFreq(x.numAttFreq);
  			
 		
		
		}
		
	

	
		
		attFreqExists = x.attFreqExists;
		if (x.attFreqExists) {
		
		
			
		attFreq .clear();
		for (unsigned int i = 0; i <x.attFreq.length(); ++i) {
			
			attFreq.push_back(x.attFreq[i]);
  			
		}
			
  		
		
		}
		
	

	
		
		attSpectrumExists = x.attSpectrumExists;
		if (x.attSpectrumExists) {
		
		
			
		attSpectrum .clear();
		for (unsigned int i = 0; i <x.attSpectrum.length(); ++i) {
			
			attSpectrum.push_back(Complex (x.attSpectrum[i]));
			
		}
			
  		
		
		}
		
	

	
	
		
	
		
		
			
		setAntennaId(Tag (x.antennaId));
			
 		
		
	

	
		
		
			
		setFeedId(x.feedId);
  			
 		
		
	

	
		
		
			
		setSpectralWindowId(Tag (x.spectralWindowId));
			
 		
		
	

	
		
	

	

	

		} catch (const IllegalAccessException &err) {
			throw ConversionException (err.getMessage(),"GainTracking");
		}
	}
#endif
	
	/**
	 * Return this row in the form of an XML string.
	 * @return The values of this row as an XML string.
	 */
	string GainTrackingRow::toXML() const {
		string buf;
		buf.append("<row> \n");
		
	
		
  	
 		
		
		Parser::toXML(timeInterval, "timeInterval", buf);
		
		
	

  	
 		
		
		Parser::toXML(numReceptor, "numReceptor", buf);
		
		
	

  	
 		
		
		Parser::toXML(attenuator, "attenuator", buf);
		
		
	

  	
 		
		
			buf.append(EnumerationParser::toXML("polarizationType", polarizationType));
		
		
	

  	
 		
		if (samplingLevelExists) {
		
		
		Parser::toXML(samplingLevel, "samplingLevel", buf);
		
		
		}
		
	

  	
 		
		if (numAttFreqExists) {
		
		
		Parser::toXML(numAttFreq, "numAttFreq", buf);
		
		
		}
		
	

  	
 		
		if (attFreqExists) {
		
		
		Parser::toXML(attFreq, "attFreq", buf);
		
		
		}
		
	

  	
 		
		if (attSpectrumExists) {
		
		
		Parser::toXML(attSpectrum, "attSpectrum", buf);
		
		
		}
		
	

	
	
		
  	
 		
		
		Parser::toXML(antennaId, "antennaId", buf);
		
		
	

  	
 		
		
		Parser::toXML(feedId, "feedId", buf);
		
		
	

  	
 		
		
		Parser::toXML(spectralWindowId, "spectralWindowId", buf);
		
		
	

	
		
	

	

	

		
		buf.append("</row>\n");
		return buf;
	}

	/**
	 * Fill the values of this row from an XML string 
	 * that was produced by the toXML() method.
	 * @param x The XML string being used to set the values of this row.
	 */
	void GainTrackingRow::setFromXML (string rowDoc) {
		Parser row(rowDoc);
		string s = "";
		try {
	
		
	
  		
			
	  	setTimeInterval(Parser::getArrayTimeInterval("timeInterval","GainTracking",rowDoc));
			
		
	

	
  		
			
	  	setNumReceptor(Parser::getInteger("numReceptor","GainTracking",rowDoc));
			
		
	

	
  		
			
					
	  	setAttenuator(Parser::get1DFloat("attenuator","GainTracking",rowDoc));
	  			
	  		
		
	

	
		
		
		
		polarizationType = EnumerationParser::getPolarizationType1D("polarizationType","GainTracking",rowDoc);			
		
		
		
	

	
  		
        if (row.isStr("<samplingLevel>")) {
			
	  		setSamplingLevel(Parser::getFloat("samplingLevel","GainTracking",rowDoc));
			
		}
 		
	

	
  		
        if (row.isStr("<numAttFreq>")) {
			
	  		setNumAttFreq(Parser::getInteger("numAttFreq","GainTracking",rowDoc));
			
		}
 		
	

	
  		
        if (row.isStr("<attFreq>")) {
			
								
	  		setAttFreq(Parser::get1DDouble("attFreq","GainTracking",rowDoc));
	  			
	  		
		}
 		
	

	
  		
        if (row.isStr("<attSpectrum>")) {
			
								
	  		setAttSpectrum(Parser::get1DComplex("attSpectrum","GainTracking",rowDoc));
	  			
	  		
		}
 		
	

	
	
		
	
  		
			
	  	setAntennaId(Parser::getTag("antennaId","Antenna",rowDoc));
			
		
	

	
  		
			
	  	setFeedId(Parser::getInteger("feedId","Feed",rowDoc));
			
		
	

	
  		
			
	  	setSpectralWindowId(Parser::getTag("spectralWindowId","SpectralWindow",rowDoc));
			
		
	

	
		
	

	

	

		} catch (const IllegalAccessException &err) {
			throw ConversionException (err.getMessage(),"GainTracking");
		}
	}
	
	void GainTrackingRow::toBin(EndianOSStream& eoss) {
	
	
	
	
		
	antennaId.toBin(eoss);
		
	

	
	
		
	spectralWindowId.toBin(eoss);
		
	

	
	
		
	timeInterval.toBin(eoss);
		
	

	
	
		
						
			eoss.writeInt(feedId);
				
		
	

	
	
		
						
			eoss.writeInt(numReceptor);
				
		
	

	
	
		
		
			
		eoss.writeInt((int) attenuator.size());
		for (unsigned int i = 0; i < attenuator.size(); i++)
				
			eoss.writeFloat(attenuator.at(i));
				
				
						
		
	

	
	
		
		
			
		eoss.writeInt((int) polarizationType.size());
		for (unsigned int i = 0; i < polarizationType.size(); i++)
				
			eoss.writeString(CPolarizationType::name(polarizationType.at(i)));
			/* eoss.writeInt(polarizationType.at(i)); */
				
				
						
		
	


	
	
	eoss.writeBoolean(samplingLevelExists);
	if (samplingLevelExists) {
	
	
	
		
						
			eoss.writeFloat(samplingLevel);
				
		
	

	}

	eoss.writeBoolean(numAttFreqExists);
	if (numAttFreqExists) {
	
	
	
		
						
			eoss.writeInt(numAttFreq);
				
		
	

	}

	eoss.writeBoolean(attFreqExists);
	if (attFreqExists) {
	
	
	
		
		
			
		eoss.writeInt((int) attFreq.size());
		for (unsigned int i = 0; i < attFreq.size(); i++)
				
			eoss.writeDouble(attFreq.at(i));
				
				
						
		
	

	}

	eoss.writeBoolean(attSpectrumExists);
	if (attSpectrumExists) {
	
	
	
		
	Complex::toBin(attSpectrum, eoss);
		
	

	}

	}
	
void GainTrackingRow::antennaIdFromBin(EndianIStream& eis) {
		
	
		
		
		antennaId =  Tag::fromBin(eis);
		
	
	
}
void GainTrackingRow::spectralWindowIdFromBin(EndianIStream& eis) {
		
	
		
		
		spectralWindowId =  Tag::fromBin(eis);
		
	
	
}
void GainTrackingRow::timeIntervalFromBin(EndianIStream& eis) {
		
	
		
		
		timeInterval =  ArrayTimeInterval::fromBin(eis);
		
	
	
}
void GainTrackingRow::feedIdFromBin(EndianIStream& eis) {
		
	
	
		
			
		feedId =  eis.readInt();
			
		
	
	
}
void GainTrackingRow::numReceptorFromBin(EndianIStream& eis) {
		
	
	
		
			
		numReceptor =  eis.readInt();
			
		
	
	
}
void GainTrackingRow::attenuatorFromBin(EndianIStream& eis) {
		
	
	
		
			
	
		attenuator.clear();
		
		unsigned int attenuatorDim1 = eis.readInt();
		for (unsigned int  i = 0 ; i < attenuatorDim1; i++)
			
			attenuator.push_back(eis.readFloat());
			
	

		
	
	
}
void GainTrackingRow::polarizationTypeFromBin(EndianIStream& eis) {
		
	
	
		
			
	
		polarizationType.clear();
		
		unsigned int polarizationTypeDim1 = eis.readInt();
		for (unsigned int  i = 0 ; i < polarizationTypeDim1; i++)
			
			polarizationType.push_back(CPolarizationType::literal(eis.readString()));
			
	

		
	
	
}

void GainTrackingRow::samplingLevelFromBin(EndianIStream& eis) {
		
	samplingLevelExists = eis.readBoolean();
	if (samplingLevelExists) {
		
	
	
		
			
		samplingLevel =  eis.readFloat();
			
		
	

	}
	
}
void GainTrackingRow::numAttFreqFromBin(EndianIStream& eis) {
		
	numAttFreqExists = eis.readBoolean();
	if (numAttFreqExists) {
		
	
	
		
			
		numAttFreq =  eis.readInt();
			
		
	

	}
	
}
void GainTrackingRow::attFreqFromBin(EndianIStream& eis) {
		
	attFreqExists = eis.readBoolean();
	if (attFreqExists) {
		
	
	
		
			
	
		attFreq.clear();
		
		unsigned int attFreqDim1 = eis.readInt();
		for (unsigned int  i = 0 ; i < attFreqDim1; i++)
			
			attFreq.push_back(eis.readDouble());
			
	

		
	

	}
	
}
void GainTrackingRow::attSpectrumFromBin(EndianIStream& eis) {
		
	attSpectrumExists = eis.readBoolean();
	if (attSpectrumExists) {
		
	
		
		
			
	
	attSpectrum = Complex::from1DBin(eis);	
	

		
	

	}
	
}
	
	
	GainTrackingRow* GainTrackingRow::fromBin(EndianIStream& eis, GainTrackingTable& table, const vector<string>& attributesSeq) {
		GainTrackingRow* row = new  GainTrackingRow(table);
		
		map<string, GainTrackingAttributeFromBin>::iterator iter ;
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
					throw ConversionException("There is not method to read an attribute '"+attributesSeq.at(i)+"'.", "GainTrackingTable");
			}
				
		}				
		return row;
	}

	//
	// A collection of methods to set the value of the attributes from their textual value in the XML representation
	// of one row.
	//
	
	// Convert a string into an Tag 
	void GainTrackingRow::antennaIdFromText(const string & s) {
		 
          
		antennaId = ASDMValuesParser::parse<Tag>(s);
          
		
	}
	
	
	// Convert a string into an Tag 
	void GainTrackingRow::spectralWindowIdFromText(const string & s) {
		 
          
		spectralWindowId = ASDMValuesParser::parse<Tag>(s);
          
		
	}
	
	
	// Convert a string into an ArrayTimeInterval 
	void GainTrackingRow::timeIntervalFromText(const string & s) {
		 
          
		timeInterval = ASDMValuesParser::parse<ArrayTimeInterval>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void GainTrackingRow::feedIdFromText(const string & s) {
		 
          
		feedId = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void GainTrackingRow::numReceptorFromText(const string & s) {
		 
          
		numReceptor = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	// Convert a string into an float 
	void GainTrackingRow::attenuatorFromText(const string & s) {
		 
          
		attenuator = ASDMValuesParser::parse1D<float>(s);
          
		
	}
	
	
	// Convert a string into an PolarizationType 
	void GainTrackingRow::polarizationTypeFromText(const string & s) {
		 
          
		polarizationType = ASDMValuesParser::parse1D<PolarizationTypeMod::PolarizationType>(s);
          
		
	}
	

	
	// Convert a string into an float 
	void GainTrackingRow::samplingLevelFromText(const string & s) {
		samplingLevelExists = true;
		 
          
		samplingLevel = ASDMValuesParser::parse<float>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void GainTrackingRow::numAttFreqFromText(const string & s) {
		numAttFreqExists = true;
		 
          
		numAttFreq = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	// Convert a string into an double 
	void GainTrackingRow::attFreqFromText(const string & s) {
		attFreqExists = true;
		 
          
		attFreq = ASDMValuesParser::parse1D<double>(s);
          
		
	}
	
	
	// Convert a string into an Complex 
	void GainTrackingRow::attSpectrumFromText(const string & s) {
		attSpectrumExists = true;
		 
          
		attSpectrum = ASDMValuesParser::parse1D<Complex>(s);
          
		
	}
	
	
	
	void GainTrackingRow::fromText(const std::string& attributeName, const std::string&  t) {
		map<string, GainTrackingAttributeFromText>::iterator iter;
		if ((iter = fromTextMethods.find(attributeName)) == fromTextMethods.end())
			throw ConversionException("I do not know what to do with '"+attributeName+"' and its content '"+t+"' (while parsing an XML document)", "GainTrackingTable");
		(this->*(iter->second))(t);
	}
			
	////////////////////////////////////////////////
	// Intrinsic Table Attributes getters/setters //
	////////////////////////////////////////////////
	
	

	
 	/**
 	 * Get timeInterval.
 	 * @return timeInterval as ArrayTimeInterval
 	 */
 	ArrayTimeInterval GainTrackingRow::getTimeInterval() const {
	
  		return timeInterval;
 	}

 	/**
 	 * Set timeInterval with the specified ArrayTimeInterval.
 	 * @param timeInterval The ArrayTimeInterval value to which timeInterval is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void GainTrackingRow::setTimeInterval (ArrayTimeInterval timeInterval)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("timeInterval", "GainTracking");
		
  		}
  	
 		this->timeInterval = timeInterval;
	
 	}
	
	

	

	
 	/**
 	 * Get numReceptor.
 	 * @return numReceptor as int
 	 */
 	int GainTrackingRow::getNumReceptor() const {
	
  		return numReceptor;
 	}

 	/**
 	 * Set numReceptor with the specified int.
 	 * @param numReceptor The int value to which numReceptor is to be set.
 	 
 	
 		
 	 */
 	void GainTrackingRow::setNumReceptor (int numReceptor)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->numReceptor = numReceptor;
	
 	}
	
	

	

	
 	/**
 	 * Get attenuator.
 	 * @return attenuator as std::vector<float >
 	 */
 	std::vector<float > GainTrackingRow::getAttenuator() const {
	
  		return attenuator;
 	}

 	/**
 	 * Set attenuator with the specified std::vector<float >.
 	 * @param attenuator The std::vector<float > value to which attenuator is to be set.
 	 
 	
 		
 	 */
 	void GainTrackingRow::setAttenuator (std::vector<float > attenuator)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->attenuator = attenuator;
	
 	}
	
	

	

	
 	/**
 	 * Get polarizationType.
 	 * @return polarizationType as std::vector<PolarizationTypeMod::PolarizationType >
 	 */
 	std::vector<PolarizationTypeMod::PolarizationType > GainTrackingRow::getPolarizationType() const {
	
  		return polarizationType;
 	}

 	/**
 	 * Set polarizationType with the specified std::vector<PolarizationTypeMod::PolarizationType >.
 	 * @param polarizationType The std::vector<PolarizationTypeMod::PolarizationType > value to which polarizationType is to be set.
 	 
 	
 		
 	 */
 	void GainTrackingRow::setPolarizationType (std::vector<PolarizationTypeMod::PolarizationType > polarizationType)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->polarizationType = polarizationType;
	
 	}
	
	

	
	/**
	 * The attribute samplingLevel is optional. Return true if this attribute exists.
	 * @return true if and only if the samplingLevel attribute exists. 
	 */
	bool GainTrackingRow::isSamplingLevelExists() const {
		return samplingLevelExists;
	}
	

	
 	/**
 	 * Get samplingLevel, which is optional.
 	 * @return samplingLevel as float
 	 * @throw IllegalAccessException If samplingLevel does not exist.
 	 */
 	float GainTrackingRow::getSamplingLevel() const  {
		if (!samplingLevelExists) {
			throw IllegalAccessException("samplingLevel", "GainTracking");
		}
	
  		return samplingLevel;
 	}

 	/**
 	 * Set samplingLevel with the specified float.
 	 * @param samplingLevel The float value to which samplingLevel is to be set.
 	 
 	
 	 */
 	void GainTrackingRow::setSamplingLevel (float samplingLevel) {
	
 		this->samplingLevel = samplingLevel;
	
		samplingLevelExists = true;
	
 	}
	
	
	/**
	 * Mark samplingLevel, which is an optional field, as non-existent.
	 */
	void GainTrackingRow::clearSamplingLevel () {
		samplingLevelExists = false;
	}
	

	
	/**
	 * The attribute numAttFreq is optional. Return true if this attribute exists.
	 * @return true if and only if the numAttFreq attribute exists. 
	 */
	bool GainTrackingRow::isNumAttFreqExists() const {
		return numAttFreqExists;
	}
	

	
 	/**
 	 * Get numAttFreq, which is optional.
 	 * @return numAttFreq as int
 	 * @throw IllegalAccessException If numAttFreq does not exist.
 	 */
 	int GainTrackingRow::getNumAttFreq() const  {
		if (!numAttFreqExists) {
			throw IllegalAccessException("numAttFreq", "GainTracking");
		}
	
  		return numAttFreq;
 	}

 	/**
 	 * Set numAttFreq with the specified int.
 	 * @param numAttFreq The int value to which numAttFreq is to be set.
 	 
 	
 	 */
 	void GainTrackingRow::setNumAttFreq (int numAttFreq) {
	
 		this->numAttFreq = numAttFreq;
	
		numAttFreqExists = true;
	
 	}
	
	
	/**
	 * Mark numAttFreq, which is an optional field, as non-existent.
	 */
	void GainTrackingRow::clearNumAttFreq () {
		numAttFreqExists = false;
	}
	

	
	/**
	 * The attribute attFreq is optional. Return true if this attribute exists.
	 * @return true if and only if the attFreq attribute exists. 
	 */
	bool GainTrackingRow::isAttFreqExists() const {
		return attFreqExists;
	}
	

	
 	/**
 	 * Get attFreq, which is optional.
 	 * @return attFreq as std::vector<double >
 	 * @throw IllegalAccessException If attFreq does not exist.
 	 */
 	std::vector<double > GainTrackingRow::getAttFreq() const  {
		if (!attFreqExists) {
			throw IllegalAccessException("attFreq", "GainTracking");
		}
	
  		return attFreq;
 	}

 	/**
 	 * Set attFreq with the specified std::vector<double >.
 	 * @param attFreq The std::vector<double > value to which attFreq is to be set.
 	 
 	
 	 */
 	void GainTrackingRow::setAttFreq (std::vector<double > attFreq) {
	
 		this->attFreq = attFreq;
	
		attFreqExists = true;
	
 	}
	
	
	/**
	 * Mark attFreq, which is an optional field, as non-existent.
	 */
	void GainTrackingRow::clearAttFreq () {
		attFreqExists = false;
	}
	

	
	/**
	 * The attribute attSpectrum is optional. Return true if this attribute exists.
	 * @return true if and only if the attSpectrum attribute exists. 
	 */
	bool GainTrackingRow::isAttSpectrumExists() const {
		return attSpectrumExists;
	}
	

	
 	/**
 	 * Get attSpectrum, which is optional.
 	 * @return attSpectrum as std::vector<Complex >
 	 * @throw IllegalAccessException If attSpectrum does not exist.
 	 */
 	std::vector<Complex > GainTrackingRow::getAttSpectrum() const  {
		if (!attSpectrumExists) {
			throw IllegalAccessException("attSpectrum", "GainTracking");
		}
	
  		return attSpectrum;
 	}

 	/**
 	 * Set attSpectrum with the specified std::vector<Complex >.
 	 * @param attSpectrum The std::vector<Complex > value to which attSpectrum is to be set.
 	 
 	
 	 */
 	void GainTrackingRow::setAttSpectrum (std::vector<Complex > attSpectrum) {
	
 		this->attSpectrum = attSpectrum;
	
		attSpectrumExists = true;
	
 	}
	
	
	/**
	 * Mark attSpectrum, which is an optional field, as non-existent.
	 */
	void GainTrackingRow::clearAttSpectrum () {
		attSpectrumExists = false;
	}
	

	
	///////////////////////////////////////////////
	// Extrinsic Table Attributes getters/setters//
	///////////////////////////////////////////////
	
	

	
 	/**
 	 * Get antennaId.
 	 * @return antennaId as Tag
 	 */
 	Tag GainTrackingRow::getAntennaId() const {
	
  		return antennaId;
 	}

 	/**
 	 * Set antennaId with the specified Tag.
 	 * @param antennaId The Tag value to which antennaId is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void GainTrackingRow::setAntennaId (Tag antennaId)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("antennaId", "GainTracking");
		
  		}
  	
 		this->antennaId = antennaId;
	
 	}
	
	

	

	
 	/**
 	 * Get feedId.
 	 * @return feedId as int
 	 */
 	int GainTrackingRow::getFeedId() const {
	
  		return feedId;
 	}

 	/**
 	 * Set feedId with the specified int.
 	 * @param feedId The int value to which feedId is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void GainTrackingRow::setFeedId (int feedId)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("feedId", "GainTracking");
		
  		}
  	
 		this->feedId = feedId;
	
 	}
	
	

	

	
 	/**
 	 * Get spectralWindowId.
 	 * @return spectralWindowId as Tag
 	 */
 	Tag GainTrackingRow::getSpectralWindowId() const {
	
  		return spectralWindowId;
 	}

 	/**
 	 * Set spectralWindowId with the specified Tag.
 	 * @param spectralWindowId The Tag value to which spectralWindowId is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void GainTrackingRow::setSpectralWindowId (Tag spectralWindowId)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("spectralWindowId", "GainTracking");
		
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
	 AntennaRow* GainTrackingRow::getAntennaUsingAntennaId() {
	 
	 	return table.getContainer().getAntenna().getRowByKey(antennaId);
	 }
	 

	

	
	
	
		

	/**
	 * Returns the pointer to the row in the SpectralWindow table having SpectralWindow.spectralWindowId == spectralWindowId
	 * @return a SpectralWindowRow*
	 * 
	 
	 */
	 SpectralWindowRow* GainTrackingRow::getSpectralWindowUsingSpectralWindowId() {
	 
	 	return table.getContainer().getSpectralWindow().getRowByKey(spectralWindowId);
	 }
	 

	

	
	
	
		

	// ===> Slice link from a row of GainTracking table to a collection of row of Feed table.
	
	/**
	 * Get the collection of row in the Feed table having their attribut feedId == this->feedId
	 */
	vector <FeedRow *> GainTrackingRow::getFeeds() {
		
			return table.getContainer().getFeed().getRowByFeedId(feedId);
		
	}
	

	

	
	/**
	 * Create a GainTrackingRow.
	 * <p>
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 */ 
	GainTrackingRow::GainTrackingRow (GainTrackingTable &t) : table(t) {
		hasBeenAdded = false;
		
	
	

	

	

	

	
		samplingLevelExists = false;
	

	
		numAttFreqExists = false;
	

	
		attFreqExists = false;
	

	
		attSpectrumExists = false;
	

	
	

	

	

	
	
	
	

	

	

	

	

	

	

	

	
	
	 fromBinMethods["antennaId"] = &GainTrackingRow::antennaIdFromBin; 
	 fromBinMethods["spectralWindowId"] = &GainTrackingRow::spectralWindowIdFromBin; 
	 fromBinMethods["timeInterval"] = &GainTrackingRow::timeIntervalFromBin; 
	 fromBinMethods["feedId"] = &GainTrackingRow::feedIdFromBin; 
	 fromBinMethods["numReceptor"] = &GainTrackingRow::numReceptorFromBin; 
	 fromBinMethods["attenuator"] = &GainTrackingRow::attenuatorFromBin; 
	 fromBinMethods["polarizationType"] = &GainTrackingRow::polarizationTypeFromBin; 
		
	
	 fromBinMethods["samplingLevel"] = &GainTrackingRow::samplingLevelFromBin; 
	 fromBinMethods["numAttFreq"] = &GainTrackingRow::numAttFreqFromBin; 
	 fromBinMethods["attFreq"] = &GainTrackingRow::attFreqFromBin; 
	 fromBinMethods["attSpectrum"] = &GainTrackingRow::attSpectrumFromBin; 
	
	
	
	
				 
	fromTextMethods["antennaId"] = &GainTrackingRow::antennaIdFromText;
		 
	
				 
	fromTextMethods["spectralWindowId"] = &GainTrackingRow::spectralWindowIdFromText;
		 
	
				 
	fromTextMethods["timeInterval"] = &GainTrackingRow::timeIntervalFromText;
		 
	
				 
	fromTextMethods["feedId"] = &GainTrackingRow::feedIdFromText;
		 
	
				 
	fromTextMethods["numReceptor"] = &GainTrackingRow::numReceptorFromText;
		 
	
				 
	fromTextMethods["attenuator"] = &GainTrackingRow::attenuatorFromText;
		 
	
				 
	fromTextMethods["polarizationType"] = &GainTrackingRow::polarizationTypeFromText;
		 
	

	 
				
	fromTextMethods["samplingLevel"] = &GainTrackingRow::samplingLevelFromText;
		 	
	 
				
	fromTextMethods["numAttFreq"] = &GainTrackingRow::numAttFreqFromText;
		 	
	 
				
	fromTextMethods["attFreq"] = &GainTrackingRow::attFreqFromText;
		 	
	 
				
	fromTextMethods["attSpectrum"] = &GainTrackingRow::attSpectrumFromText;
		 	
		
	}
	
	GainTrackingRow::GainTrackingRow (GainTrackingTable &t, GainTrackingRow *row) : table(t) {
		hasBeenAdded = false;
		
		if (row == 0) {
	
	
	

	

	

	

	
		samplingLevelExists = false;
	

	
		numAttFreqExists = false;
	

	
		attFreqExists = false;
	

	
		attSpectrumExists = false;
	

	
	

	

	
		
		}
		else {
	
		
			antennaId = row->antennaId;
		
			spectralWindowId = row->spectralWindowId;
		
			timeInterval = row->timeInterval;
		
			feedId = row->feedId;
		
		
		
		
			numReceptor = row->numReceptor;
		
			attenuator = row->attenuator;
		
			polarizationType = row->polarizationType;
		
		
		
		
		if (row->samplingLevelExists) {
			samplingLevel = row->samplingLevel;		
			samplingLevelExists = true;
		}
		else
			samplingLevelExists = false;
		
		if (row->numAttFreqExists) {
			numAttFreq = row->numAttFreq;		
			numAttFreqExists = true;
		}
		else
			numAttFreqExists = false;
		
		if (row->attFreqExists) {
			attFreq = row->attFreq;		
			attFreqExists = true;
		}
		else
			attFreqExists = false;
		
		if (row->attSpectrumExists) {
			attSpectrum = row->attSpectrum;		
			attSpectrumExists = true;
		}
		else
			attSpectrumExists = false;
		
		}
		
		 fromBinMethods["antennaId"] = &GainTrackingRow::antennaIdFromBin; 
		 fromBinMethods["spectralWindowId"] = &GainTrackingRow::spectralWindowIdFromBin; 
		 fromBinMethods["timeInterval"] = &GainTrackingRow::timeIntervalFromBin; 
		 fromBinMethods["feedId"] = &GainTrackingRow::feedIdFromBin; 
		 fromBinMethods["numReceptor"] = &GainTrackingRow::numReceptorFromBin; 
		 fromBinMethods["attenuator"] = &GainTrackingRow::attenuatorFromBin; 
		 fromBinMethods["polarizationType"] = &GainTrackingRow::polarizationTypeFromBin; 
			
	
		 fromBinMethods["samplingLevel"] = &GainTrackingRow::samplingLevelFromBin; 
		 fromBinMethods["numAttFreq"] = &GainTrackingRow::numAttFreqFromBin; 
		 fromBinMethods["attFreq"] = &GainTrackingRow::attFreqFromBin; 
		 fromBinMethods["attSpectrum"] = &GainTrackingRow::attSpectrumFromBin; 
			
	}

	
	bool GainTrackingRow::compareNoAutoInc(Tag antennaId, Tag spectralWindowId, ArrayTimeInterval timeInterval, int feedId, int numReceptor, std::vector<float > attenuator, std::vector<PolarizationTypeMod::PolarizationType > polarizationType) {
		bool result;
		result = true;
		
	
		
		result = result && (this->antennaId == antennaId);
		
		if (!result) return false;
	

	
		
		result = result && (this->spectralWindowId == spectralWindowId);
		
		if (!result) return false;
	

	
		
		result = result && (this->timeInterval.overlaps(timeInterval));
		
		if (!result) return false;
	

	
		
		result = result && (this->feedId == feedId);
		
		if (!result) return false;
	

	
		
		result = result && (this->numReceptor == numReceptor);
		
		if (!result) return false;
	

	
		
		result = result && (this->attenuator == attenuator);
		
		if (!result) return false;
	

	
		
		result = result && (this->polarizationType == polarizationType);
		
		if (!result) return false;
	

		return result;
	}	
	
	
	
	bool GainTrackingRow::compareRequiredValue(int numReceptor, std::vector<float > attenuator, std::vector<PolarizationTypeMod::PolarizationType > polarizationType) {
		bool result;
		result = true;
		
	
		if (!(this->numReceptor == numReceptor)) return false;
	

	
		if (!(this->attenuator == attenuator)) return false;
	

	
		if (!(this->polarizationType == polarizationType)) return false;
	

		return result;
	}
	
	
	/**
	 * Return true if all required attributes of the value part are equal to their homologues
	 * in x and false otherwise.
	 *

	 * @param x a pointer on the GainTrackingRow whose required attributes of the value part 

	 * will be compared with those of this.
	 * @return a boolean.
	 */
	bool GainTrackingRow::equalByRequiredValue(GainTrackingRow*  x ) {
		
			
		if (this->numReceptor != x->numReceptor) return false;
			
		if (this->attenuator != x->attenuator) return false;
			
		if (this->polarizationType != x->polarizationType) return false;
			
		
		return true;
	}	
	
/*
	 map<string, GainTrackingAttributeFromBin> GainTrackingRow::initFromBinMethods() {
		map<string, GainTrackingAttributeFromBin> result;
		
		result["antennaId"] = &GainTrackingRow::antennaIdFromBin;
		result["spectralWindowId"] = &GainTrackingRow::spectralWindowIdFromBin;
		result["timeInterval"] = &GainTrackingRow::timeIntervalFromBin;
		result["feedId"] = &GainTrackingRow::feedIdFromBin;
		result["numReceptor"] = &GainTrackingRow::numReceptorFromBin;
		result["attenuator"] = &GainTrackingRow::attenuatorFromBin;
		result["polarizationType"] = &GainTrackingRow::polarizationTypeFromBin;
		
		
		result["samplingLevel"] = &GainTrackingRow::samplingLevelFromBin;
		result["numAttFreq"] = &GainTrackingRow::numAttFreqFromBin;
		result["attFreq"] = &GainTrackingRow::attFreqFromBin;
		result["attSpectrum"] = &GainTrackingRow::attSpectrumFromBin;
			
		
		return result;	
	}
*/	
} // End namespace asdm
 
