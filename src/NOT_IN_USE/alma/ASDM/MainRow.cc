
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
 * File MainRow.cpp
 */
 
#include <vector>
#include <set>

#include <alma/ASDM/ASDM.h>
#include <alma/ASDM/MainRow.h>
#include <alma/ASDM/MainTable.h>

#include <alma/ASDM/ConfigDescriptionTable.h>
#include <alma/ASDM/ConfigDescriptionRow.h>

#include <alma/ASDM/FieldTable.h>
#include <alma/ASDM/FieldRow.h>

#include <alma/ASDM/StateTable.h>
#include <alma/ASDM/StateRow.h>

#include <alma/ASDM/ExecBlockTable.h>
#include <alma/ASDM/ExecBlockRow.h>
	

using asdm::ASDM;
using asdm::MainRow;
using asdm::MainTable;

using asdm::ConfigDescriptionTable;
using asdm::ConfigDescriptionRow;

using asdm::FieldTable;
using asdm::FieldRow;

using asdm::StateTable;
using asdm::StateRow;

using asdm::ExecBlockTable;
using asdm::ExecBlockRow;


#include <alma/ASDM/Parser.h>

#include <alma/ASDM/EnumerationParser.h>
#include <alma/ASDM/ASDMValuesParser.h>
 
#include <alma/ASDM/InvalidArgumentException.h>

using namespace std;

namespace asdm {
	MainRow::~MainRow() {
	}

	/**
	 * Return the table to which this row belongs.
	 */
	MainTable &MainRow::getTable() const {
		return table;
	}

	bool MainRow::isAdded() const {
		return hasBeenAdded;
	}	

	void MainRow::isAdded(bool added) {
		hasBeenAdded = added;
	}
	
#ifndef WITHOUT_ACS
	using asdmIDL::MainRowIDL;
#endif
	
#ifndef WITHOUT_ACS
	/**
	 * Return this row in the form of an IDL struct.
	 * @return The values of this row as a MainRowIDL struct.
	 */
	MainRowIDL *MainRow::toIDL() const {
		MainRowIDL *x = new MainRowIDL ();
		
		// Fill the IDL structure.
	
		
	
  		
		
		
			
		x->time = time.toIDLArrayTime();
			
		
	

	
  		
		
		
			
				
		x->numAntenna = numAntenna;
 				
 			
		
	

	
  		
		
		
			
				
		x->timeSampling = timeSampling;
 				
 			
		
	

	
  		
		
		
			
		x->interval = interval.toIDLInterval();
			
		
	

	
  		
		
		
			
				
		x->numIntegration = numIntegration;
 				
 			
		
	

	
  		
		
		
			
				
		x->scanNumber = scanNumber;
 				
 			
		
	

	
  		
		
		
			
				
		x->subscanNumber = subscanNumber;
 				
 			
		
	

	
  		
		
		
			
				
		x->dataSize = dataSize;
 				
 			
		
	

	
  		
		
		
			
		x->dataUID = dataUID.toIDLEntityRef();
			
		
	

	
	
		
	
  	
 		
		
	 	
			
		x->configDescriptionId = configDescriptionId.toIDLTag();
			
	 	 		
  	

	
  	
 		
		
	 	
			
		x->execBlockId = execBlockId.toIDLTag();
			
	 	 		
  	

	
  	
 		
		
	 	
			
		x->fieldId = fieldId.toIDLTag();
			
	 	 		
  	

	
  	
 		
		
		
		x->stateId.length(stateId.size());
		for (unsigned int i = 0; i < stateId.size(); ++i) {
			
			x->stateId[i] = stateId.at(i).toIDLTag();
			
	 	}
	 	 		
  	

	
		
	

	

	

	

		
		return x;
	
	}
	
	void MainRow::toIDL(asdmIDL::MainRowIDL& x) const {
		// Set the x's fields.
	
		
	
  		
		
		
			
		x.time = time.toIDLArrayTime();
			
		
	

	
  		
		
		
			
				
		x.numAntenna = numAntenna;
 				
 			
		
	

	
  		
		
		
			
				
		x.timeSampling = timeSampling;
 				
 			
		
	

	
  		
		
		
			
		x.interval = interval.toIDLInterval();
			
		
	

	
  		
		
		
			
				
		x.numIntegration = numIntegration;
 				
 			
		
	

	
  		
		
		
			
				
		x.scanNumber = scanNumber;
 				
 			
		
	

	
  		
		
		
			
				
		x.subscanNumber = subscanNumber;
 				
 			
		
	

	
  		
		
		
			
				
		x.dataSize = dataSize;
 				
 			
		
	

	
  		
		
		
			
		x.dataUID = dataUID.toIDLEntityRef();
			
		
	

	
	
		
	
  	
 		
		
	 	
			
		x.configDescriptionId = configDescriptionId.toIDLTag();
			
	 	 		
  	

	
  	
 		
		
	 	
			
		x.execBlockId = execBlockId.toIDLTag();
			
	 	 		
  	

	
  	
 		
		
	 	
			
		x.fieldId = fieldId.toIDLTag();
			
	 	 		
  	

	
  	
 		
		
		
		x.stateId.length(stateId.size());
		for (unsigned int i = 0; i < stateId.size(); ++i) {
			
			x.stateId[i] = stateId.at(i).toIDLTag();
			
	 	}
	 	 		
  	

	
		
	

	

	

	

	
	}
#endif
	

#ifndef WITHOUT_ACS
	/**
	 * Fill the values of this row from the IDL struct MainRowIDL.
	 * @param x The IDL struct containing the values used to fill this row.
	 */
	void MainRow::setFromIDL (MainRowIDL x){
		try {
		// Fill the values from x.
	
		
	
		
		
			
		setTime(ArrayTime (x.time));
			
 		
		
	

	
		
		
			
		setNumAntenna(x.numAntenna);
  			
 		
		
	

	
		
		
			
		setTimeSampling(x.timeSampling);
  			
 		
		
	

	
		
		
			
		setInterval(Interval (x.interval));
			
 		
		
	

	
		
		
			
		setNumIntegration(x.numIntegration);
  			
 		
		
	

	
		
		
			
		setScanNumber(x.scanNumber);
  			
 		
		
	

	
		
		
			
		setSubscanNumber(x.subscanNumber);
  			
 		
		
	

	
		
		
			
		setDataSize(x.dataSize);
  			
 		
		
	

	
		
		
			
		setDataUID(EntityRef (x.dataUID));
			
 		
		
	

	
	
		
	
		
		
			
		setConfigDescriptionId(Tag (x.configDescriptionId));
			
 		
		
	

	
		
		
			
		setExecBlockId(Tag (x.execBlockId));
			
 		
		
	

	
		
		
			
		setFieldId(Tag (x.fieldId));
			
 		
		
	

	
		
		stateId .clear();
		for (unsigned int i = 0; i <x.stateId.length(); ++i) {
			
			stateId.push_back(Tag (x.stateId[i]));
			
		}
		
  	

	
		
	

	

	

	

		} catch (const IllegalAccessException &err) {
			throw ConversionException (err.getMessage(),"Main");
		}
	}
#endif
	
	/**
	 * Return this row in the form of an XML string.
	 * @return The values of this row as an XML string.
	 */
	string MainRow::toXML() const {
		string buf;
		buf.append("<row> \n");
		
	
		
  	
 		
		
		Parser::toXML(time, "time", buf);
		
		
	

  	
 		
		
		Parser::toXML(numAntenna, "numAntenna", buf);
		
		
	

  	
 		
		
			buf.append(EnumerationParser::toXML("timeSampling", timeSampling));
		
		
	

  	
 		
		
		Parser::toXML(interval, "interval", buf);
		
		
	

  	
 		
		
		Parser::toXML(numIntegration, "numIntegration", buf);
		
		
	

  	
 		
		
		Parser::toXML(scanNumber, "scanNumber", buf);
		
		
	

  	
 		
		
		Parser::toXML(subscanNumber, "subscanNumber", buf);
		
		
	

  	
 		
		
		Parser::toXML(dataSize, "dataSize", buf);
		
		
	

  	
 		
		
		Parser::toXML(dataUID, "dataUID", buf);
		
		
	

	
	
		
  	
 		
		
		Parser::toXML(configDescriptionId, "configDescriptionId", buf);
		
		
	

  	
 		
		
		Parser::toXML(execBlockId, "execBlockId", buf);
		
		
	

  	
 		
		
		Parser::toXML(fieldId, "fieldId", buf);
		
		
	

  	
 		
		
		Parser::toXML(stateId, "stateId", buf);
		
		
	

	
		
	

	

	

	

		
		buf.append("</row>\n");
		return buf;
	}

	/**
	 * Fill the values of this row from an XML string 
	 * that was produced by the toXML() method.
	 * @param x The XML string being used to set the values of this row.
	 */
	void MainRow::setFromXML (string rowDoc) {
		Parser row(rowDoc);
		string s = "";
		try {
	
		
	
  		
			
	  	setTime(Parser::getArrayTime("time","Main",rowDoc));
			
		
	

	
  		
			
	  	setNumAntenna(Parser::getInteger("numAntenna","Main",rowDoc));
			
		
	

	
		
		
		
		timeSampling = EnumerationParser::getTimeSampling("timeSampling","Main",rowDoc);
		
		
		
	

	
  		
			
	  	setInterval(Parser::getInterval("interval","Main",rowDoc));
			
		
	

	
  		
			
	  	setNumIntegration(Parser::getInteger("numIntegration","Main",rowDoc));
			
		
	

	
  		
			
	  	setScanNumber(Parser::getInteger("scanNumber","Main",rowDoc));
			
		
	

	
  		
			
	  	setSubscanNumber(Parser::getInteger("subscanNumber","Main",rowDoc));
			
		
	

	
  		
			
	  	setDataSize(Parser::getLong("dataSize","Main",rowDoc));
			
		
	

	
  		
			
	  	setDataUID(Parser::getEntityRef("dataUID","Main",rowDoc));
			
		
	

	
	
		
	
  		
			
	  	setConfigDescriptionId(Parser::getTag("configDescriptionId","ConfigDescription",rowDoc));
			
		
	

	
  		
			
	  	setExecBlockId(Parser::getTag("execBlockId","Main",rowDoc));
			
		
	

	
  		
			
	  	setFieldId(Parser::getTag("fieldId","Field",rowDoc));
			
		
	

	
  		 
  		setStateId(Parser::get1DTag("stateId","Main",rowDoc));
		
  	

	
		
	

	

	

	

		} catch (const IllegalAccessException &err) {
			throw ConversionException (err.getMessage(),"Main");
		}
	}
	
	void MainRow::toBin(EndianOSStream& eoss) {
	
	
	
	
		
	time.toBin(eoss);
		
	

	
	
		
	configDescriptionId.toBin(eoss);
		
	

	
	
		
	fieldId.toBin(eoss);
		
	

	
	
		
						
			eoss.writeInt(numAntenna);
				
		
	

	
	
		
					
		eoss.writeString(CTimeSampling::name(timeSampling));
			/* eoss.writeInt(timeSampling); */
				
		
	

	
	
		
	interval.toBin(eoss);
		
	

	
	
		
						
			eoss.writeInt(numIntegration);
				
		
	

	
	
		
						
			eoss.writeInt(scanNumber);
				
		
	

	
	
		
						
			eoss.writeInt(subscanNumber);
				
		
	

	
	
		
						
			eoss.writeLong(dataSize);
				
		
	

	
	
		
	dataUID.toBin(eoss);
		
	

	
	
		
	Tag::toBin(stateId, eoss);
		
	

	
	
		
	execBlockId.toBin(eoss);
		
	


	
	
	}
	
void MainRow::timeFromBin(EndianIStream& eis) {
		
	
		
		
		time =  ArrayTime::fromBin(eis);
		
	
	
}
void MainRow::configDescriptionIdFromBin(EndianIStream& eis) {
		
	
		
		
		configDescriptionId =  Tag::fromBin(eis);
		
	
	
}
void MainRow::fieldIdFromBin(EndianIStream& eis) {
		
	
		
		
		fieldId =  Tag::fromBin(eis);
		
	
	
}
void MainRow::numAntennaFromBin(EndianIStream& eis) {
		
	
	
		
			
		numAntenna =  eis.readInt();
			
		
	
	
}
void MainRow::timeSamplingFromBin(EndianIStream& eis) {
		
	
	
		
			
		timeSampling = CTimeSampling::literal(eis.readString());
			
		
	
	
}
void MainRow::intervalFromBin(EndianIStream& eis) {
		
	
		
		
		interval =  Interval::fromBin(eis);
		
	
	
}
void MainRow::numIntegrationFromBin(EndianIStream& eis) {
		
	
	
		
			
		numIntegration =  eis.readInt();
			
		
	
	
}
void MainRow::scanNumberFromBin(EndianIStream& eis) {
		
	
	
		
			
		scanNumber =  eis.readInt();
			
		
	
	
}
void MainRow::subscanNumberFromBin(EndianIStream& eis) {
		
	
	
		
			
		subscanNumber =  eis.readInt();
			
		
	
	
}
void MainRow::dataSizeFromBin(EndianIStream& eis) {
		
	
	
		
			
		dataSize =  eis.readLong();
			
		
	
	
}
void MainRow::dataUIDFromBin(EndianIStream& eis) {
		
	
		
		
		dataUID =  EntityRef::fromBin(eis);
		
	
	
}
void MainRow::stateIdFromBin(EndianIStream& eis) {
		
	
		
		
			
	
	stateId = Tag::from1DBin(eis);	
	

		
	
	
}
void MainRow::execBlockIdFromBin(EndianIStream& eis) {
		
	
		
		
		execBlockId =  Tag::fromBin(eis);
		
	
	
}

		
	
	MainRow* MainRow::fromBin(EndianIStream& eis, MainTable& table, const vector<string>& attributesSeq) {
		MainRow* row = new  MainRow(table);
		
		map<string, MainAttributeFromBin>::iterator iter ;
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
					throw ConversionException("There is not method to read an attribute '"+attributesSeq.at(i)+"'.", "MainTable");
			}
				
		}				
		return row;
	}

	//
	// A collection of methods to set the value of the attributes from their textual value in the XML representation
	// of one row.
	//
	
	// Convert a string into an ArrayTime 
	void MainRow::timeFromText(const string & s) {
		 
          
		time = ASDMValuesParser::parse<ArrayTime>(s);
          
		
	}
	
	
	// Convert a string into an Tag 
	void MainRow::configDescriptionIdFromText(const string & s) {
		 
          
		configDescriptionId = ASDMValuesParser::parse<Tag>(s);
          
		
	}
	
	
	// Convert a string into an Tag 
	void MainRow::fieldIdFromText(const string & s) {
		 
          
		fieldId = ASDMValuesParser::parse<Tag>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void MainRow::numAntennaFromText(const string & s) {
		 
          
		numAntenna = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	// Convert a string into an TimeSampling 
	void MainRow::timeSamplingFromText(const string & s) {
		 
          
		timeSampling = ASDMValuesParser::parse<TimeSamplingMod::TimeSampling>(s);
          
		
	}
	
	
	// Convert a string into an Interval 
	void MainRow::intervalFromText(const string & s) {
		 
          
		interval = ASDMValuesParser::parse<Interval>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void MainRow::numIntegrationFromText(const string & s) {
		 
          
		numIntegration = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void MainRow::scanNumberFromText(const string & s) {
		 
          
		scanNumber = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void MainRow::subscanNumberFromText(const string & s) {
		 
          
		subscanNumber = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	// Convert a string into an long 
	void MainRow::dataSizeFromText(const string & s) {
		 
          
		dataSize = ASDMValuesParser::parse<int64_t>(s);
          
		
	}
	
	
	
	// Convert a string into an Tag 
	void MainRow::stateIdFromText(const string & s) {
		 
          
		stateId = ASDMValuesParser::parse1D<Tag>(s);
          
		
	}
	
	
	// Convert a string into an Tag 
	void MainRow::execBlockIdFromText(const string & s) {
		 
          
		execBlockId = ASDMValuesParser::parse<Tag>(s);
          
		
	}
	

		
	
	void MainRow::fromText(const std::string& attributeName, const std::string&  t) {
		map<string, MainAttributeFromText>::iterator iter;
		if ((iter = fromTextMethods.find(attributeName)) == fromTextMethods.end())
			throw ConversionException("I do not know what to do with '"+attributeName+"' and its content '"+t+"' (while parsing an XML document)", "MainTable");
		(this->*(iter->second))(t);
	}
			
	////////////////////////////////////////////////
	// Intrinsic Table Attributes getters/setters //
	////////////////////////////////////////////////
	
	

	
 	/**
 	 * Get time.
 	 * @return time as ArrayTime
 	 */
 	ArrayTime MainRow::getTime() const {
	
  		return time;
 	}

 	/**
 	 * Set time with the specified ArrayTime.
 	 * @param time The ArrayTime value to which time is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void MainRow::setTime (ArrayTime time)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("time", "Main");
		
  		}
  	
 		this->time = time;
	
 	}
	
	

	

	
 	/**
 	 * Get numAntenna.
 	 * @return numAntenna as int
 	 */
 	int MainRow::getNumAntenna() const {
	
  		return numAntenna;
 	}

 	/**
 	 * Set numAntenna with the specified int.
 	 * @param numAntenna The int value to which numAntenna is to be set.
 	 
 	
 		
 	 */
 	void MainRow::setNumAntenna (int numAntenna)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->numAntenna = numAntenna;
	
 	}
	
	

	

	
 	/**
 	 * Get timeSampling.
 	 * @return timeSampling as TimeSamplingMod::TimeSampling
 	 */
 	TimeSamplingMod::TimeSampling MainRow::getTimeSampling() const {
	
  		return timeSampling;
 	}

 	/**
 	 * Set timeSampling with the specified TimeSamplingMod::TimeSampling.
 	 * @param timeSampling The TimeSamplingMod::TimeSampling value to which timeSampling is to be set.
 	 
 	
 		
 	 */
 	void MainRow::setTimeSampling (TimeSamplingMod::TimeSampling timeSampling)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->timeSampling = timeSampling;
	
 	}
	
	

	

	
 	/**
 	 * Get interval.
 	 * @return interval as Interval
 	 */
 	Interval MainRow::getInterval() const {
	
  		return interval;
 	}

 	/**
 	 * Set interval with the specified Interval.
 	 * @param interval The Interval value to which interval is to be set.
 	 
 	
 		
 	 */
 	void MainRow::setInterval (Interval interval)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->interval = interval;
	
 	}
	
	

	

	
 	/**
 	 * Get numIntegration.
 	 * @return numIntegration as int
 	 */
 	int MainRow::getNumIntegration() const {
	
  		return numIntegration;
 	}

 	/**
 	 * Set numIntegration with the specified int.
 	 * @param numIntegration The int value to which numIntegration is to be set.
 	 
 	
 		
 	 */
 	void MainRow::setNumIntegration (int numIntegration)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->numIntegration = numIntegration;
	
 	}
	
	

	

	
 	/**
 	 * Get scanNumber.
 	 * @return scanNumber as int
 	 */
 	int MainRow::getScanNumber() const {
	
  		return scanNumber;
 	}

 	/**
 	 * Set scanNumber with the specified int.
 	 * @param scanNumber The int value to which scanNumber is to be set.
 	 
 	
 		
 	 */
 	void MainRow::setScanNumber (int scanNumber)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->scanNumber = scanNumber;
	
 	}
	
	

	

	
 	/**
 	 * Get subscanNumber.
 	 * @return subscanNumber as int
 	 */
 	int MainRow::getSubscanNumber() const {
	
  		return subscanNumber;
 	}

 	/**
 	 * Set subscanNumber with the specified int.
 	 * @param subscanNumber The int value to which subscanNumber is to be set.
 	 
 	
 		
 	 */
 	void MainRow::setSubscanNumber (int subscanNumber)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->subscanNumber = subscanNumber;
	
 	}
	
	

	

	
 	/**
 	 * Get dataSize.
 	 * @return dataSize as int64_t
 	 */
 	int64_t MainRow::getDataSize() const {
	
  		return dataSize;
 	}

 	/**
 	 * Set dataSize with the specified int64_t.
 	 * @param dataSize The int64_t value to which dataSize is to be set.
 	 
 	
 		
 	 */
 	void MainRow::setDataSize (int64_t dataSize)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->dataSize = dataSize;
	
 	}
	
	

	

	
 	/**
 	 * Get dataUID.
 	 * @return dataUID as EntityRef
 	 */
 	EntityRef MainRow::getDataUID() const {
	
  		return dataUID;
 	}

 	/**
 	 * Set dataUID with the specified EntityRef.
 	 * @param dataUID The EntityRef value to which dataUID is to be set.
 	 
 	
 		
 	 */
 	void MainRow::setDataUID (EntityRef dataUID)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->dataUID = dataUID;
	
 	}
	
	

	
	///////////////////////////////////////////////
	// Extrinsic Table Attributes getters/setters//
	///////////////////////////////////////////////
	
	

	
 	/**
 	 * Get configDescriptionId.
 	 * @return configDescriptionId as Tag
 	 */
 	Tag MainRow::getConfigDescriptionId() const {
	
  		return configDescriptionId;
 	}

 	/**
 	 * Set configDescriptionId with the specified Tag.
 	 * @param configDescriptionId The Tag value to which configDescriptionId is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void MainRow::setConfigDescriptionId (Tag configDescriptionId)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("configDescriptionId", "Main");
		
  		}
  	
 		this->configDescriptionId = configDescriptionId;
	
 	}
	
	

	

	
 	/**
 	 * Get execBlockId.
 	 * @return execBlockId as Tag
 	 */
 	Tag MainRow::getExecBlockId() const {
	
  		return execBlockId;
 	}

 	/**
 	 * Set execBlockId with the specified Tag.
 	 * @param execBlockId The Tag value to which execBlockId is to be set.
 	 
 	
 		
 	 */
 	void MainRow::setExecBlockId (Tag execBlockId)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->execBlockId = execBlockId;
	
 	}
	
	

	

	
 	/**
 	 * Get fieldId.
 	 * @return fieldId as Tag
 	 */
 	Tag MainRow::getFieldId() const {
	
  		return fieldId;
 	}

 	/**
 	 * Set fieldId with the specified Tag.
 	 * @param fieldId The Tag value to which fieldId is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void MainRow::setFieldId (Tag fieldId)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("fieldId", "Main");
		
  		}
  	
 		this->fieldId = fieldId;
	
 	}
	
	

	

	
 	/**
 	 * Get stateId.
 	 * @return stateId as std::vector<Tag> 
 	 */
 	std::vector<Tag>  MainRow::getStateId() const {
	
  		return stateId;
 	}

 	/**
 	 * Set stateId with the specified std::vector<Tag> .
 	 * @param stateId The std::vector<Tag>  value to which stateId is to be set.
 	 
 	
 		
 	 */
 	void MainRow::setStateId (std::vector<Tag>  stateId)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->stateId = stateId;
	
 	}
	
	


	//////////////////////////////////////
	// Links Attributes getters/setters //
	//////////////////////////////////////
	
	
	
	
		

	/**
	 * Returns the pointer to the row in the ConfigDescription table having ConfigDescription.configDescriptionId == configDescriptionId
	 * @return a ConfigDescriptionRow*
	 * 
	 
	 */
	 ConfigDescriptionRow* MainRow::getConfigDescriptionUsingConfigDescriptionId() {
	 
	 	return table.getContainer().getConfigDescription().getRowByKey(configDescriptionId);
	 }
	 

	

	
	
	
		

	/**
	 * Returns the pointer to the row in the Field table having Field.fieldId == fieldId
	 * @return a FieldRow*
	 * 
	 
	 */
	 FieldRow* MainRow::getFieldUsingFieldId() {
	 
	 	return table.getContainer().getField().getRowByKey(fieldId);
	 }
	 

	

	
 		
 	/**
 	 * Set stateId[i] with the specified Tag.
 	 * @param i The index in stateId where to set the Tag value.
 	 * @param stateId The Tag value to which stateId[i] is to be set. 
	 		
 	 * @throws IndexOutOfBoundsException
  	 */
  	void MainRow::setStateId (int i, Tag stateId)  {
  	  	if (hasBeenAdded) {
  	  		
  		}
  		if ((i < 0) || (i > ((int) this->stateId.size())))
  			throw OutOfBoundsException("Index out of bounds during a set operation on attribute stateId in table MainTable");
  		std::vector<Tag> ::iterator iter = this->stateId.begin();
  		int j = 0;
  		while (j < i) {
  			j++; iter++;
  		}
  		this->stateId.insert(this->stateId.erase(iter), stateId); 
  	}	
 			
	
	
	
		
/**
 * Append a Tag to stateId.
 * @param id the Tag to be appended to stateId
 */
 void MainRow::addStateId(Tag id){
 	stateId.push_back(id);
}

/**
 * Append an array of Tag to stateId.
 * @param id an array of Tag to be appended to stateId
 */
 void MainRow::addStateId(const std::vector<Tag> & id) {
 	for (unsigned int i=0; i < id.size(); i++)
 		stateId.push_back(id.at(i));
 }
 

 /**
  * Returns the Tag stored in stateId at position i.
  *
  */
 const Tag MainRow::getStateId(int i) {
 	return stateId.at(i);
 }
 
 /**
  * Returns the StateRow linked to this row via the Tag stored in stateId
  * at position i.
  */
 StateRow* MainRow::getStateUsingStateId(int i) {
 	return table.getContainer().getState().getRowByKey(stateId.at(i));
 } 
 
 /**
  * Returns the vector of StateRow* linked to this row via the Tags stored in stateId
  *
  */
 vector<StateRow *> MainRow::getStatesUsingStateId() {
 	vector<StateRow *> result;
 	for (unsigned int i = 0; i < stateId.size(); i++)
 		result.push_back(table.getContainer().getState().getRowByKey(stateId.at(i)));
 		
 	return result;
 }
  

	

	
	
	
		

	/**
	 * Returns the pointer to the row in the ExecBlock table having ExecBlock.execBlockId == execBlockId
	 * @return a ExecBlockRow*
	 * 
	 
	 */
	 ExecBlockRow* MainRow::getExecBlockUsingExecBlockId() {
	 
	 	return table.getContainer().getExecBlock().getRowByKey(execBlockId);
	 }
	 

	

	
	/**
	 * Create a MainRow.
	 * <p>
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 */ 
	MainRow::MainRow (MainTable &t) : table(t) {
		hasBeenAdded = false;
		
	
	

	

	

	

	

	

	

	

	

	
	

	

	

	

	
	
	
	

	

	
// This attribute is scalar and has an enumeration type. Let's initialize it to some valid value (the 1st of the enumeration).		
timeSampling = CTimeSampling::from_int(0);
	

	

	

	

	

	

	

	
	
	 fromBinMethods["time"] = &MainRow::timeFromBin; 
	 fromBinMethods["configDescriptionId"] = &MainRow::configDescriptionIdFromBin; 
	 fromBinMethods["fieldId"] = &MainRow::fieldIdFromBin; 
	 fromBinMethods["numAntenna"] = &MainRow::numAntennaFromBin; 
	 fromBinMethods["timeSampling"] = &MainRow::timeSamplingFromBin; 
	 fromBinMethods["interval"] = &MainRow::intervalFromBin; 
	 fromBinMethods["numIntegration"] = &MainRow::numIntegrationFromBin; 
	 fromBinMethods["scanNumber"] = &MainRow::scanNumberFromBin; 
	 fromBinMethods["subscanNumber"] = &MainRow::subscanNumberFromBin; 
	 fromBinMethods["dataSize"] = &MainRow::dataSizeFromBin; 
	 fromBinMethods["dataUID"] = &MainRow::dataUIDFromBin; 
	 fromBinMethods["stateId"] = &MainRow::stateIdFromBin; 
	 fromBinMethods["execBlockId"] = &MainRow::execBlockIdFromBin; 
		
	
	
	
	
	
				 
	fromTextMethods["time"] = &MainRow::timeFromText;
		 
	
				 
	fromTextMethods["configDescriptionId"] = &MainRow::configDescriptionIdFromText;
		 
	
				 
	fromTextMethods["fieldId"] = &MainRow::fieldIdFromText;
		 
	
				 
	fromTextMethods["numAntenna"] = &MainRow::numAntennaFromText;
		 
	
				 
	fromTextMethods["timeSampling"] = &MainRow::timeSamplingFromText;
		 
	
				 
	fromTextMethods["interval"] = &MainRow::intervalFromText;
		 
	
				 
	fromTextMethods["numIntegration"] = &MainRow::numIntegrationFromText;
		 
	
				 
	fromTextMethods["scanNumber"] = &MainRow::scanNumberFromText;
		 
	
				 
	fromTextMethods["subscanNumber"] = &MainRow::subscanNumberFromText;
		 
	
				 
	fromTextMethods["dataSize"] = &MainRow::dataSizeFromText;
		 
	
		 
	
				 
	fromTextMethods["stateId"] = &MainRow::stateIdFromText;
		 
	
				 
	fromTextMethods["execBlockId"] = &MainRow::execBlockIdFromText;
		 
	

		
	}
	
	MainRow::MainRow (MainTable &t, MainRow *row) : table(t) {
		hasBeenAdded = false;
		
		if (row == 0) {
	
	
	

	

	

	

	

	

	

	

	

	
	

	

	

	
		
		}
		else {
	
		
			time = row->time;
		
			configDescriptionId = row->configDescriptionId;
		
			fieldId = row->fieldId;
		
		
		
		
			numAntenna = row->numAntenna;
		
			timeSampling = row->timeSampling;
		
			interval = row->interval;
		
			numIntegration = row->numIntegration;
		
			scanNumber = row->scanNumber;
		
			subscanNumber = row->subscanNumber;
		
			dataSize = row->dataSize;
		
			dataUID = row->dataUID;
		
			stateId = row->stateId;
		
			execBlockId = row->execBlockId;
		
		
		
		
		}
		
		 fromBinMethods["time"] = &MainRow::timeFromBin; 
		 fromBinMethods["configDescriptionId"] = &MainRow::configDescriptionIdFromBin; 
		 fromBinMethods["fieldId"] = &MainRow::fieldIdFromBin; 
		 fromBinMethods["numAntenna"] = &MainRow::numAntennaFromBin; 
		 fromBinMethods["timeSampling"] = &MainRow::timeSamplingFromBin; 
		 fromBinMethods["interval"] = &MainRow::intervalFromBin; 
		 fromBinMethods["numIntegration"] = &MainRow::numIntegrationFromBin; 
		 fromBinMethods["scanNumber"] = &MainRow::scanNumberFromBin; 
		 fromBinMethods["subscanNumber"] = &MainRow::subscanNumberFromBin; 
		 fromBinMethods["dataSize"] = &MainRow::dataSizeFromBin; 
		 fromBinMethods["dataUID"] = &MainRow::dataUIDFromBin; 
		 fromBinMethods["stateId"] = &MainRow::stateIdFromBin; 
		 fromBinMethods["execBlockId"] = &MainRow::execBlockIdFromBin; 
			
	
			
	}

	
	bool MainRow::compareNoAutoInc(ArrayTime time, Tag configDescriptionId, Tag fieldId, int numAntenna, TimeSamplingMod::TimeSampling timeSampling, Interval interval, int numIntegration, int scanNumber, int subscanNumber, int64_t dataSize, EntityRef dataUID, std::vector<Tag>  stateId, Tag execBlockId) {
		bool result;
		result = true;
		
	
		
		result = result && (this->time == time);
		
		if (!result) return false;
	

	
		
		result = result && (this->configDescriptionId == configDescriptionId);
		
		if (!result) return false;
	

	
		
		result = result && (this->fieldId == fieldId);
		
		if (!result) return false;
	

	
		
		result = result && (this->numAntenna == numAntenna);
		
		if (!result) return false;
	

	
		
		result = result && (this->timeSampling == timeSampling);
		
		if (!result) return false;
	

	
		
		result = result && (this->interval == interval);
		
		if (!result) return false;
	

	
		
		result = result && (this->numIntegration == numIntegration);
		
		if (!result) return false;
	

	
		
		result = result && (this->scanNumber == scanNumber);
		
		if (!result) return false;
	

	
		
		result = result && (this->subscanNumber == subscanNumber);
		
		if (!result) return false;
	

	
		
		result = result && (this->dataSize == dataSize);
		
		if (!result) return false;
	

	
		
		result = result && (this->dataUID == dataUID);
		
		if (!result) return false;
	

	
		
		result = result && (this->stateId == stateId);
		
		if (!result) return false;
	

	
		
		result = result && (this->execBlockId == execBlockId);
		
		if (!result) return false;
	

		return result;
	}	
	
	
	
	bool MainRow::compareRequiredValue(int numAntenna, TimeSamplingMod::TimeSampling timeSampling, Interval interval, int numIntegration, int scanNumber, int subscanNumber, int64_t dataSize, EntityRef dataUID, std::vector<Tag>  stateId, Tag execBlockId) {
		bool result;
		result = true;
		
	
		if (!(this->numAntenna == numAntenna)) return false;
	

	
		if (!(this->timeSampling == timeSampling)) return false;
	

	
		if (!(this->interval == interval)) return false;
	

	
		if (!(this->numIntegration == numIntegration)) return false;
	

	
		if (!(this->scanNumber == scanNumber)) return false;
	

	
		if (!(this->subscanNumber == subscanNumber)) return false;
	

	
		if (!(this->dataSize == dataSize)) return false;
	

	
		if (!(this->dataUID == dataUID)) return false;
	

	
		if (!(this->stateId == stateId)) return false;
	

	
		if (!(this->execBlockId == execBlockId)) return false;
	

		return result;
	}
	
	
	/**
	 * Return true if all required attributes of the value part are equal to their homologues
	 * in x and false otherwise.
	 *

	 * @param x a pointer on the MainRow whose required attributes of the value part 

	 * will be compared with those of this.
	 * @return a boolean.
	 */
	bool MainRow::equalByRequiredValue(MainRow*  x ) {
		
			
		if (this->numAntenna != x->numAntenna) return false;
			
		if (this->timeSampling != x->timeSampling) return false;
			
		if (this->interval != x->interval) return false;
			
		if (this->numIntegration != x->numIntegration) return false;
			
		if (this->scanNumber != x->scanNumber) return false;
			
		if (this->subscanNumber != x->subscanNumber) return false;
			
		if (this->dataSize != x->dataSize) return false;
			
		if (this->dataUID != x->dataUID) return false;
			
		if (this->stateId != x->stateId) return false;
			
		if (this->execBlockId != x->execBlockId) return false;
			
		
		return true;
	}	
	
/*
	 map<string, MainAttributeFromBin> MainRow::initFromBinMethods() {
		map<string, MainAttributeFromBin> result;
		
		result["time"] = &MainRow::timeFromBin;
		result["configDescriptionId"] = &MainRow::configDescriptionIdFromBin;
		result["fieldId"] = &MainRow::fieldIdFromBin;
		result["numAntenna"] = &MainRow::numAntennaFromBin;
		result["timeSampling"] = &MainRow::timeSamplingFromBin;
		result["interval"] = &MainRow::intervalFromBin;
		result["numIntegration"] = &MainRow::numIntegrationFromBin;
		result["scanNumber"] = &MainRow::scanNumberFromBin;
		result["subscanNumber"] = &MainRow::subscanNumberFromBin;
		result["dataSize"] = &MainRow::dataSizeFromBin;
		result["dataUID"] = &MainRow::dataUIDFromBin;
		result["stateId"] = &MainRow::stateIdFromBin;
		result["execBlockId"] = &MainRow::execBlockIdFromBin;
		
		
			
		
		return result;	
	}
*/	
} // End namespace asdm
 
