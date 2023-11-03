
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
 * File VLAWVRRow.cpp
 */
 
#include <vector>
#include <set>

#include <alma/ASDM/ASDM.h>
#include <alma/ASDM/VLAWVRRow.h>
#include <alma/ASDM/VLAWVRTable.h>

#include <alma/ASDM/AntennaTable.h>
#include <alma/ASDM/AntennaRow.h>
	

using asdm::ASDM;
using asdm::VLAWVRRow;
using asdm::VLAWVRTable;

using asdm::AntennaTable;
using asdm::AntennaRow;


#include <alma/ASDM/Parser.h>

#include <alma/ASDM/EnumerationParser.h>
#include <alma/ASDM/ASDMValuesParser.h>
 
#include <alma/ASDM/InvalidArgumentException.h>

using namespace std;

namespace asdm {
	VLAWVRRow::~VLAWVRRow() {
	}

	/**
	 * Return the table to which this row belongs.
	 */
	VLAWVRTable &VLAWVRRow::getTable() const {
		return table;
	}

	bool VLAWVRRow::isAdded() const {
		return hasBeenAdded;
	}	

	void VLAWVRRow::isAdded(bool added) {
		hasBeenAdded = added;
	}
	
#ifndef WITHOUT_ACS
	using asdmIDL::VLAWVRRowIDL;
#endif
	
#ifndef WITHOUT_ACS
	/**
	 * Return this row in the form of an IDL struct.
	 * @return The values of this row as a VLAWVRRowIDL struct.
	 */
	VLAWVRRowIDL *VLAWVRRow::toIDL() const {
		VLAWVRRowIDL *x = new VLAWVRRowIDL ();
		
		// Fill the IDL structure.
	
		
	
  		
		
		
			
		x->timeInterval = timeInterval.toIDLArrayTimeInterval();
			
		
	

	
  		
		
		
			
				
		x->numChan = numChan;
 				
 			
		
	

	
  		
		
		
			
		x->hiValues.length(hiValues.size());
		for (unsigned int i = 0; i < hiValues.size(); ++i) {
			
				
			x->hiValues[i] = hiValues.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		
			
		x->loValues.length(loValues.size());
		for (unsigned int i = 0; i < loValues.size(); ++i) {
			
				
			x->loValues[i] = loValues.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		x->chanFreqCenterExists = chanFreqCenterExists;
		
		
			
		x->chanFreqCenter.length(chanFreqCenter.size());
		for (unsigned int i = 0; i < chanFreqCenter.size(); ++i) {
			
			x->chanFreqCenter[i] = chanFreqCenter.at(i).toIDLFrequency();
			
	 	}
			
		
	

	
  		
		
		x->chanWidthExists = chanWidthExists;
		
		
			
		x->chanWidth.length(chanWidth.size());
		for (unsigned int i = 0; i < chanWidth.size(); ++i) {
			
			x->chanWidth[i] = chanWidth.at(i).toIDLFrequency();
			
	 	}
			
		
	

	
  		
		
		x->wvrIdExists = wvrIdExists;
		
		
			
				
		x->wvrId = CORBA::string_dup(wvrId.c_str());
				
 			
		
	

	
	
		
	
  	
 		
		
	 	
			
		x->antennaId = antennaId.toIDLTag();
			
	 	 		
  	

	
		
	

		
		return x;
	
	}
	
	void VLAWVRRow::toIDL(asdmIDL::VLAWVRRowIDL& x) const {
		// Set the x's fields.
	
		
	
  		
		
		
			
		x.timeInterval = timeInterval.toIDLArrayTimeInterval();
			
		
	

	
  		
		
		
			
				
		x.numChan = numChan;
 				
 			
		
	

	
  		
		
		
			
		x.hiValues.length(hiValues.size());
		for (unsigned int i = 0; i < hiValues.size(); ++i) {
			
				
			x.hiValues[i] = hiValues.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		
			
		x.loValues.length(loValues.size());
		for (unsigned int i = 0; i < loValues.size(); ++i) {
			
				
			x.loValues[i] = loValues.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		x.chanFreqCenterExists = chanFreqCenterExists;
		
		
			
		x.chanFreqCenter.length(chanFreqCenter.size());
		for (unsigned int i = 0; i < chanFreqCenter.size(); ++i) {
			
			x.chanFreqCenter[i] = chanFreqCenter.at(i).toIDLFrequency();
			
	 	}
			
		
	

	
  		
		
		x.chanWidthExists = chanWidthExists;
		
		
			
		x.chanWidth.length(chanWidth.size());
		for (unsigned int i = 0; i < chanWidth.size(); ++i) {
			
			x.chanWidth[i] = chanWidth.at(i).toIDLFrequency();
			
	 	}
			
		
	

	
  		
		
		x.wvrIdExists = wvrIdExists;
		
		
			
				
		x.wvrId = CORBA::string_dup(wvrId.c_str());
				
 			
		
	

	
	
		
	
  	
 		
		
	 	
			
		x.antennaId = antennaId.toIDLTag();
			
	 	 		
  	

	
		
	

	
	}
#endif
	

#ifndef WITHOUT_ACS
	/**
	 * Fill the values of this row from the IDL struct VLAWVRRowIDL.
	 * @param x The IDL struct containing the values used to fill this row.
	 */
	void VLAWVRRow::setFromIDL (VLAWVRRowIDL x){
		try {
		// Fill the values from x.
	
		
	
		
		
			
		setTimeInterval(ArrayTimeInterval (x.timeInterval));
			
 		
		
	

	
		
		
			
		setNumChan(x.numChan);
  			
 		
		
	

	
		
		
			
		hiValues .clear();
		for (unsigned int i = 0; i <x.hiValues.length(); ++i) {
			
			hiValues.push_back(x.hiValues[i]);
  			
		}
			
  		
		
	

	
		
		
			
		loValues .clear();
		for (unsigned int i = 0; i <x.loValues.length(); ++i) {
			
			loValues.push_back(x.loValues[i]);
  			
		}
			
  		
		
	

	
		
		chanFreqCenterExists = x.chanFreqCenterExists;
		if (x.chanFreqCenterExists) {
		
		
			
		chanFreqCenter .clear();
		for (unsigned int i = 0; i <x.chanFreqCenter.length(); ++i) {
			
			chanFreqCenter.push_back(Frequency (x.chanFreqCenter[i]));
			
		}
			
  		
		
		}
		
	

	
		
		chanWidthExists = x.chanWidthExists;
		if (x.chanWidthExists) {
		
		
			
		chanWidth .clear();
		for (unsigned int i = 0; i <x.chanWidth.length(); ++i) {
			
			chanWidth.push_back(Frequency (x.chanWidth[i]));
			
		}
			
  		
		
		}
		
	

	
		
		wvrIdExists = x.wvrIdExists;
		if (x.wvrIdExists) {
		
		
			
		setWvrId(string (x.wvrId));
			
 		
		
		}
		
	

	
	
		
	
		
		
			
		setAntennaId(Tag (x.antennaId));
			
 		
		
	

	
		
	

		} catch (const IllegalAccessException &err) {
			throw ConversionException (err.getMessage(),"VLAWVR");
		}
	}
#endif
	
	/**
	 * Return this row in the form of an XML string.
	 * @return The values of this row as an XML string.
	 */
	string VLAWVRRow::toXML() const {
		string buf;
		buf.append("<row> \n");
		
	
		
  	
 		
		
		Parser::toXML(timeInterval, "timeInterval", buf);
		
		
	

  	
 		
		
		Parser::toXML(numChan, "numChan", buf);
		
		
	

  	
 		
		
		Parser::toXML(hiValues, "hiValues", buf);
		
		
	

  	
 		
		
		Parser::toXML(loValues, "loValues", buf);
		
		
	

  	
 		
		if (chanFreqCenterExists) {
		
		
		Parser::toXML(chanFreqCenter, "chanFreqCenter", buf);
		
		
		}
		
	

  	
 		
		if (chanWidthExists) {
		
		
		Parser::toXML(chanWidth, "chanWidth", buf);
		
		
		}
		
	

  	
 		
		if (wvrIdExists) {
		
		
		Parser::toXML(wvrId, "wvrId", buf);
		
		
		}
		
	

	
	
		
  	
 		
		
		Parser::toXML(antennaId, "antennaId", buf);
		
		
	

	
		
	

		
		buf.append("</row>\n");
		return buf;
	}

	/**
	 * Fill the values of this row from an XML string 
	 * that was produced by the toXML() method.
	 * @param x The XML string being used to set the values of this row.
	 */
	void VLAWVRRow::setFromXML (string rowDoc) {
		Parser row(rowDoc);
		string s = "";
		try {
	
		
	
  		
			
	  	setTimeInterval(Parser::getArrayTimeInterval("timeInterval","VLAWVR",rowDoc));
			
		
	

	
  		
			
	  	setNumChan(Parser::getInteger("numChan","VLAWVR",rowDoc));
			
		
	

	
  		
			
					
	  	setHiValues(Parser::get1DFloat("hiValues","VLAWVR",rowDoc));
	  			
	  		
		
	

	
  		
			
					
	  	setLoValues(Parser::get1DFloat("loValues","VLAWVR",rowDoc));
	  			
	  		
		
	

	
  		
        if (row.isStr("<chanFreqCenter>")) {
			
								
	  		setChanFreqCenter(Parser::get1DFrequency("chanFreqCenter","VLAWVR",rowDoc));
	  			
	  		
		}
 		
	

	
  		
        if (row.isStr("<chanWidth>")) {
			
								
	  		setChanWidth(Parser::get1DFrequency("chanWidth","VLAWVR",rowDoc));
	  			
	  		
		}
 		
	

	
  		
        if (row.isStr("<wvrId>")) {
			
	  		setWvrId(Parser::getString("wvrId","VLAWVR",rowDoc));
			
		}
 		
	

	
	
		
	
  		
			
	  	setAntennaId(Parser::getTag("antennaId","Antenna",rowDoc));
			
		
	

	
		
	

		} catch (const IllegalAccessException &err) {
			throw ConversionException (err.getMessage(),"VLAWVR");
		}
	}
	
	void VLAWVRRow::toBin(EndianOSStream& eoss) {
	
	
	
	
		
	antennaId.toBin(eoss);
		
	

	
	
		
	timeInterval.toBin(eoss);
		
	

	
	
		
						
			eoss.writeInt(numChan);
				
		
	

	
	
		
		
			
		eoss.writeInt((int) hiValues.size());
		for (unsigned int i = 0; i < hiValues.size(); i++)
				
			eoss.writeFloat(hiValues.at(i));
				
				
						
		
	

	
	
		
		
			
		eoss.writeInt((int) loValues.size());
		for (unsigned int i = 0; i < loValues.size(); i++)
				
			eoss.writeFloat(loValues.at(i));
				
				
						
		
	


	
	
	eoss.writeBoolean(chanFreqCenterExists);
	if (chanFreqCenterExists) {
	
	
	
		
	Frequency::toBin(chanFreqCenter, eoss);
		
	

	}

	eoss.writeBoolean(chanWidthExists);
	if (chanWidthExists) {
	
	
	
		
	Frequency::toBin(chanWidth, eoss);
		
	

	}

	eoss.writeBoolean(wvrIdExists);
	if (wvrIdExists) {
	
	
	
		
						
			eoss.writeString(wvrId);
				
		
	

	}

	}
	
void VLAWVRRow::antennaIdFromBin(EndianIStream& eis) {
		
	
		
		
		antennaId =  Tag::fromBin(eis);
		
	
	
}
void VLAWVRRow::timeIntervalFromBin(EndianIStream& eis) {
		
	
		
		
		timeInterval =  ArrayTimeInterval::fromBin(eis);
		
	
	
}
void VLAWVRRow::numChanFromBin(EndianIStream& eis) {
		
	
	
		
			
		numChan =  eis.readInt();
			
		
	
	
}
void VLAWVRRow::hiValuesFromBin(EndianIStream& eis) {
		
	
	
		
			
	
		hiValues.clear();
		
		unsigned int hiValuesDim1 = eis.readInt();
		for (unsigned int  i = 0 ; i < hiValuesDim1; i++)
			
			hiValues.push_back(eis.readFloat());
			
	

		
	
	
}
void VLAWVRRow::loValuesFromBin(EndianIStream& eis) {
		
	
	
		
			
	
		loValues.clear();
		
		unsigned int loValuesDim1 = eis.readInt();
		for (unsigned int  i = 0 ; i < loValuesDim1; i++)
			
			loValues.push_back(eis.readFloat());
			
	

		
	
	
}

void VLAWVRRow::chanFreqCenterFromBin(EndianIStream& eis) {
		
	chanFreqCenterExists = eis.readBoolean();
	if (chanFreqCenterExists) {
		
	
		
		
			
	
	chanFreqCenter = Frequency::from1DBin(eis);	
	

		
	

	}
	
}
void VLAWVRRow::chanWidthFromBin(EndianIStream& eis) {
		
	chanWidthExists = eis.readBoolean();
	if (chanWidthExists) {
		
	
		
		
			
	
	chanWidth = Frequency::from1DBin(eis);	
	

		
	

	}
	
}
void VLAWVRRow::wvrIdFromBin(EndianIStream& eis) {
		
	wvrIdExists = eis.readBoolean();
	if (wvrIdExists) {
		
	
	
		
			
		wvrId =  eis.readString();
			
		
	

	}
	
}
	
	
	VLAWVRRow* VLAWVRRow::fromBin(EndianIStream& eis, VLAWVRTable& table, const vector<string>& attributesSeq) {
		VLAWVRRow* row = new  VLAWVRRow(table);
		
		map<string, VLAWVRAttributeFromBin>::iterator iter ;
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
					throw ConversionException("There is not method to read an attribute '"+attributesSeq.at(i)+"'.", "VLAWVRTable");
			}
				
		}				
		return row;
	}

	//
	// A collection of methods to set the value of the attributes from their textual value in the XML representation
	// of one row.
	//
	
	// Convert a string into an Tag 
	void VLAWVRRow::antennaIdFromText(const string & s) {
		 
          
		antennaId = ASDMValuesParser::parse<Tag>(s);
          
		
	}
	
	
	// Convert a string into an ArrayTimeInterval 
	void VLAWVRRow::timeIntervalFromText(const string & s) {
		 
          
		timeInterval = ASDMValuesParser::parse<ArrayTimeInterval>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void VLAWVRRow::numChanFromText(const string & s) {
		 
          
		numChan = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	// Convert a string into an float 
	void VLAWVRRow::hiValuesFromText(const string & s) {
		 
          
		hiValues = ASDMValuesParser::parse1D<float>(s);
          
		
	}
	
	
	// Convert a string into an float 
	void VLAWVRRow::loValuesFromText(const string & s) {
		 
          
		loValues = ASDMValuesParser::parse1D<float>(s);
          
		
	}
	

	
	// Convert a string into an Frequency 
	void VLAWVRRow::chanFreqCenterFromText(const string & s) {
		chanFreqCenterExists = true;
		 
          
		chanFreqCenter = ASDMValuesParser::parse1D<Frequency>(s);
          
		
	}
	
	
	// Convert a string into an Frequency 
	void VLAWVRRow::chanWidthFromText(const string & s) {
		chanWidthExists = true;
		 
          
		chanWidth = ASDMValuesParser::parse1D<Frequency>(s);
          
		
	}
	
	
	// Convert a string into an String 
	void VLAWVRRow::wvrIdFromText(const string & s) {
		wvrIdExists = true;
		 
          
		wvrId = ASDMValuesParser::parse<string>(s);
          
		
	}
	
	
	
	void VLAWVRRow::fromText(const std::string& attributeName, const std::string&  t) {
		map<string, VLAWVRAttributeFromText>::iterator iter;
		if ((iter = fromTextMethods.find(attributeName)) == fromTextMethods.end())
			throw ConversionException("I do not know what to do with '"+attributeName+"' and its content '"+t+"' (while parsing an XML document)", "VLAWVRTable");
		(this->*(iter->second))(t);
	}
			
	////////////////////////////////////////////////
	// Intrinsic Table Attributes getters/setters //
	////////////////////////////////////////////////
	
	

	
 	/**
 	 * Get timeInterval.
 	 * @return timeInterval as ArrayTimeInterval
 	 */
 	ArrayTimeInterval VLAWVRRow::getTimeInterval() const {
	
  		return timeInterval;
 	}

 	/**
 	 * Set timeInterval with the specified ArrayTimeInterval.
 	 * @param timeInterval The ArrayTimeInterval value to which timeInterval is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void VLAWVRRow::setTimeInterval (ArrayTimeInterval timeInterval)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("timeInterval", "VLAWVR");
		
  		}
  	
 		this->timeInterval = timeInterval;
	
 	}
	
	

	

	
 	/**
 	 * Get numChan.
 	 * @return numChan as int
 	 */
 	int VLAWVRRow::getNumChan() const {
	
  		return numChan;
 	}

 	/**
 	 * Set numChan with the specified int.
 	 * @param numChan The int value to which numChan is to be set.
 	 
 	
 		
 	 */
 	void VLAWVRRow::setNumChan (int numChan)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->numChan = numChan;
	
 	}
	
	

	

	
 	/**
 	 * Get hiValues.
 	 * @return hiValues as std::vector<float >
 	 */
 	std::vector<float > VLAWVRRow::getHiValues() const {
	
  		return hiValues;
 	}

 	/**
 	 * Set hiValues with the specified std::vector<float >.
 	 * @param hiValues The std::vector<float > value to which hiValues is to be set.
 	 
 	
 		
 	 */
 	void VLAWVRRow::setHiValues (std::vector<float > hiValues)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->hiValues = hiValues;
	
 	}
	
	

	

	
 	/**
 	 * Get loValues.
 	 * @return loValues as std::vector<float >
 	 */
 	std::vector<float > VLAWVRRow::getLoValues() const {
	
  		return loValues;
 	}

 	/**
 	 * Set loValues with the specified std::vector<float >.
 	 * @param loValues The std::vector<float > value to which loValues is to be set.
 	 
 	
 		
 	 */
 	void VLAWVRRow::setLoValues (std::vector<float > loValues)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->loValues = loValues;
	
 	}
	
	

	
	/**
	 * The attribute chanFreqCenter is optional. Return true if this attribute exists.
	 * @return true if and only if the chanFreqCenter attribute exists. 
	 */
	bool VLAWVRRow::isChanFreqCenterExists() const {
		return chanFreqCenterExists;
	}
	

	
 	/**
 	 * Get chanFreqCenter, which is optional.
 	 * @return chanFreqCenter as std::vector<Frequency >
 	 * @throw IllegalAccessException If chanFreqCenter does not exist.
 	 */
 	std::vector<Frequency > VLAWVRRow::getChanFreqCenter() const  {
		if (!chanFreqCenterExists) {
			throw IllegalAccessException("chanFreqCenter", "VLAWVR");
		}
	
  		return chanFreqCenter;
 	}

 	/**
 	 * Set chanFreqCenter with the specified std::vector<Frequency >.
 	 * @param chanFreqCenter The std::vector<Frequency > value to which chanFreqCenter is to be set.
 	 
 	
 	 */
 	void VLAWVRRow::setChanFreqCenter (std::vector<Frequency > chanFreqCenter) {
	
 		this->chanFreqCenter = chanFreqCenter;
	
		chanFreqCenterExists = true;
	
 	}
	
	
	/**
	 * Mark chanFreqCenter, which is an optional field, as non-existent.
	 */
	void VLAWVRRow::clearChanFreqCenter () {
		chanFreqCenterExists = false;
	}
	

	
	/**
	 * The attribute chanWidth is optional. Return true if this attribute exists.
	 * @return true if and only if the chanWidth attribute exists. 
	 */
	bool VLAWVRRow::isChanWidthExists() const {
		return chanWidthExists;
	}
	

	
 	/**
 	 * Get chanWidth, which is optional.
 	 * @return chanWidth as std::vector<Frequency >
 	 * @throw IllegalAccessException If chanWidth does not exist.
 	 */
 	std::vector<Frequency > VLAWVRRow::getChanWidth() const  {
		if (!chanWidthExists) {
			throw IllegalAccessException("chanWidth", "VLAWVR");
		}
	
  		return chanWidth;
 	}

 	/**
 	 * Set chanWidth with the specified std::vector<Frequency >.
 	 * @param chanWidth The std::vector<Frequency > value to which chanWidth is to be set.
 	 
 	
 	 */
 	void VLAWVRRow::setChanWidth (std::vector<Frequency > chanWidth) {
	
 		this->chanWidth = chanWidth;
	
		chanWidthExists = true;
	
 	}
	
	
	/**
	 * Mark chanWidth, which is an optional field, as non-existent.
	 */
	void VLAWVRRow::clearChanWidth () {
		chanWidthExists = false;
	}
	

	
	/**
	 * The attribute wvrId is optional. Return true if this attribute exists.
	 * @return true if and only if the wvrId attribute exists. 
	 */
	bool VLAWVRRow::isWvrIdExists() const {
		return wvrIdExists;
	}
	

	
 	/**
 	 * Get wvrId, which is optional.
 	 * @return wvrId as std::string
 	 * @throw IllegalAccessException If wvrId does not exist.
 	 */
 	std::string VLAWVRRow::getWvrId() const  {
		if (!wvrIdExists) {
			throw IllegalAccessException("wvrId", "VLAWVR");
		}
	
  		return wvrId;
 	}

 	/**
 	 * Set wvrId with the specified std::string.
 	 * @param wvrId The std::string value to which wvrId is to be set.
 	 
 	
 	 */
 	void VLAWVRRow::setWvrId (std::string wvrId) {
	
 		this->wvrId = wvrId;
	
		wvrIdExists = true;
	
 	}
	
	
	/**
	 * Mark wvrId, which is an optional field, as non-existent.
	 */
	void VLAWVRRow::clearWvrId () {
		wvrIdExists = false;
	}
	

	
	///////////////////////////////////////////////
	// Extrinsic Table Attributes getters/setters//
	///////////////////////////////////////////////
	
	

	
 	/**
 	 * Get antennaId.
 	 * @return antennaId as Tag
 	 */
 	Tag VLAWVRRow::getAntennaId() const {
	
  		return antennaId;
 	}

 	/**
 	 * Set antennaId with the specified Tag.
 	 * @param antennaId The Tag value to which antennaId is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void VLAWVRRow::setAntennaId (Tag antennaId)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("antennaId", "VLAWVR");
		
  		}
  	
 		this->antennaId = antennaId;
	
 	}
	
	


	//////////////////////////////////////
	// Links Attributes getters/setters //
	//////////////////////////////////////
	
	
	
	
		

	/**
	 * Returns the pointer to the row in the Antenna table having Antenna.antennaId == antennaId
	 * @return a AntennaRow*
	 * 
	 
	 */
	 AntennaRow* VLAWVRRow::getAntennaUsingAntennaId() {
	 
	 	return table.getContainer().getAntenna().getRowByKey(antennaId);
	 }
	 

	

	
	/**
	 * Create a VLAWVRRow.
	 * <p>
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 */ 
	VLAWVRRow::VLAWVRRow (VLAWVRTable &t) : table(t) {
		hasBeenAdded = false;
		
	
	

	

	

	

	
		chanFreqCenterExists = false;
	

	
		chanWidthExists = false;
	

	
		wvrIdExists = false;
	

	
	

	
	
	
	

	

	

	

	

	

	

	
	
	 fromBinMethods["antennaId"] = &VLAWVRRow::antennaIdFromBin; 
	 fromBinMethods["timeInterval"] = &VLAWVRRow::timeIntervalFromBin; 
	 fromBinMethods["numChan"] = &VLAWVRRow::numChanFromBin; 
	 fromBinMethods["hiValues"] = &VLAWVRRow::hiValuesFromBin; 
	 fromBinMethods["loValues"] = &VLAWVRRow::loValuesFromBin; 
		
	
	 fromBinMethods["chanFreqCenter"] = &VLAWVRRow::chanFreqCenterFromBin; 
	 fromBinMethods["chanWidth"] = &VLAWVRRow::chanWidthFromBin; 
	 fromBinMethods["wvrId"] = &VLAWVRRow::wvrIdFromBin; 
	
	
	
	
				 
	fromTextMethods["antennaId"] = &VLAWVRRow::antennaIdFromText;
		 
	
				 
	fromTextMethods["timeInterval"] = &VLAWVRRow::timeIntervalFromText;
		 
	
				 
	fromTextMethods["numChan"] = &VLAWVRRow::numChanFromText;
		 
	
				 
	fromTextMethods["hiValues"] = &VLAWVRRow::hiValuesFromText;
		 
	
				 
	fromTextMethods["loValues"] = &VLAWVRRow::loValuesFromText;
		 
	

	 
				
	fromTextMethods["chanFreqCenter"] = &VLAWVRRow::chanFreqCenterFromText;
		 	
	 
				
	fromTextMethods["chanWidth"] = &VLAWVRRow::chanWidthFromText;
		 	
	 
				
	fromTextMethods["wvrId"] = &VLAWVRRow::wvrIdFromText;
		 	
		
	}
	
	VLAWVRRow::VLAWVRRow (VLAWVRTable &t, VLAWVRRow *row) : table(t) {
		hasBeenAdded = false;
		
		if (row == 0) {
	
	
	

	

	

	

	
		chanFreqCenterExists = false;
	

	
		chanWidthExists = false;
	

	
		wvrIdExists = false;
	

	
	
		
		}
		else {
	
		
			antennaId = row->antennaId;
		
			timeInterval = row->timeInterval;
		
		
		
		
			numChan = row->numChan;
		
			hiValues = row->hiValues;
		
			loValues = row->loValues;
		
		
		
		
		if (row->chanFreqCenterExists) {
			chanFreqCenter = row->chanFreqCenter;		
			chanFreqCenterExists = true;
		}
		else
			chanFreqCenterExists = false;
		
		if (row->chanWidthExists) {
			chanWidth = row->chanWidth;		
			chanWidthExists = true;
		}
		else
			chanWidthExists = false;
		
		if (row->wvrIdExists) {
			wvrId = row->wvrId;		
			wvrIdExists = true;
		}
		else
			wvrIdExists = false;
		
		}
		
		 fromBinMethods["antennaId"] = &VLAWVRRow::antennaIdFromBin; 
		 fromBinMethods["timeInterval"] = &VLAWVRRow::timeIntervalFromBin; 
		 fromBinMethods["numChan"] = &VLAWVRRow::numChanFromBin; 
		 fromBinMethods["hiValues"] = &VLAWVRRow::hiValuesFromBin; 
		 fromBinMethods["loValues"] = &VLAWVRRow::loValuesFromBin; 
			
	
		 fromBinMethods["chanFreqCenter"] = &VLAWVRRow::chanFreqCenterFromBin; 
		 fromBinMethods["chanWidth"] = &VLAWVRRow::chanWidthFromBin; 
		 fromBinMethods["wvrId"] = &VLAWVRRow::wvrIdFromBin; 
			
	}

	
	bool VLAWVRRow::compareNoAutoInc(Tag antennaId, ArrayTimeInterval timeInterval, int numChan, std::vector<float > hiValues, std::vector<float > loValues) {
		bool result;
		result = true;
		
	
		
		result = result && (this->antennaId == antennaId);
		
		if (!result) return false;
	

	
		
		result = result && (this->timeInterval.overlaps(timeInterval));
		
		if (!result) return false;
	

	
		
		result = result && (this->numChan == numChan);
		
		if (!result) return false;
	

	
		
		result = result && (this->hiValues == hiValues);
		
		if (!result) return false;
	

	
		
		result = result && (this->loValues == loValues);
		
		if (!result) return false;
	

		return result;
	}	
	
	
	
	bool VLAWVRRow::compareRequiredValue(int numChan, std::vector<float > hiValues, std::vector<float > loValues) {
		bool result;
		result = true;
		
	
		if (!(this->numChan == numChan)) return false;
	

	
		if (!(this->hiValues == hiValues)) return false;
	

	
		if (!(this->loValues == loValues)) return false;
	

		return result;
	}
	
	
	/**
	 * Return true if all required attributes of the value part are equal to their homologues
	 * in x and false otherwise.
	 *

	 * @param x a pointer on the VLAWVRRow whose required attributes of the value part 

	 * will be compared with those of this.
	 * @return a boolean.
	 */
	bool VLAWVRRow::equalByRequiredValue(VLAWVRRow*  x ) {
		
			
		if (this->numChan != x->numChan) return false;
			
		if (this->hiValues != x->hiValues) return false;
			
		if (this->loValues != x->loValues) return false;
			
		
		return true;
	}	
	
/*
	 map<string, VLAWVRAttributeFromBin> VLAWVRRow::initFromBinMethods() {
		map<string, VLAWVRAttributeFromBin> result;
		
		result["antennaId"] = &VLAWVRRow::antennaIdFromBin;
		result["timeInterval"] = &VLAWVRRow::timeIntervalFromBin;
		result["numChan"] = &VLAWVRRow::numChanFromBin;
		result["hiValues"] = &VLAWVRRow::hiValuesFromBin;
		result["loValues"] = &VLAWVRRow::loValuesFromBin;
		
		
		result["chanFreqCenter"] = &VLAWVRRow::chanFreqCenterFromBin;
		result["chanWidth"] = &VLAWVRRow::chanWidthFromBin;
		result["wvrId"] = &VLAWVRRow::wvrIdFromBin;
			
		
		return result;	
	}
*/	
} // End namespace asdm
 
