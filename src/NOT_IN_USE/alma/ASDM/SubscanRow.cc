
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
 * File SubscanRow.cpp
 */
 
#include <vector>
#include <set>

#include <alma/ASDM/ASDM.h>
#include <alma/ASDM/SubscanRow.h>
#include <alma/ASDM/SubscanTable.h>

#include <alma/ASDM/ExecBlockTable.h>
#include <alma/ASDM/ExecBlockRow.h>
	

using asdm::ASDM;
using asdm::SubscanRow;
using asdm::SubscanTable;

using asdm::ExecBlockTable;
using asdm::ExecBlockRow;


#include <alma/ASDM/Parser.h>

#include <alma/ASDM/EnumerationParser.h>
#include <alma/ASDM/ASDMValuesParser.h>
 
#include <alma/ASDM/InvalidArgumentException.h>

using namespace std;

namespace asdm {
	SubscanRow::~SubscanRow() {
	}

	/**
	 * Return the table to which this row belongs.
	 */
	SubscanTable &SubscanRow::getTable() const {
		return table;
	}

	bool SubscanRow::isAdded() const {
		return hasBeenAdded;
	}	

	void SubscanRow::isAdded(bool added) {
		hasBeenAdded = added;
	}
	
#ifndef WITHOUT_ACS
	using asdmIDL::SubscanRowIDL;
#endif
	
#ifndef WITHOUT_ACS
	/**
	 * Return this row in the form of an IDL struct.
	 * @return The values of this row as a SubscanRowIDL struct.
	 */
	SubscanRowIDL *SubscanRow::toIDL() const {
		SubscanRowIDL *x = new SubscanRowIDL ();
		
		// Fill the IDL structure.
	
		
	
  		
		
		
			
				
		x->scanNumber = scanNumber;
 				
 			
		
	

	
  		
		
		
			
				
		x->subscanNumber = subscanNumber;
 				
 			
		
	

	
  		
		
		
			
		x->startTime = startTime.toIDLArrayTime();
			
		
	

	
  		
		
		
			
		x->endTime = endTime.toIDLArrayTime();
			
		
	

	
  		
		
		
			
				
		x->fieldName = CORBA::string_dup(fieldName.c_str());
				
 			
		
	

	
  		
		
		
			
				
		x->subscanIntent = subscanIntent;
 				
 			
		
	

	
  		
		
		x->subscanModeExists = subscanModeExists;
		
		
			
				
		x->subscanMode = subscanMode;
 				
 			
		
	

	
  		
		
		
			
				
		x->numIntegration = numIntegration;
 				
 			
		
	

	
  		
		
		
			
		x->numSubintegration.length(numSubintegration.size());
		for (unsigned int i = 0; i < numSubintegration.size(); ++i) {
			
				
			x->numSubintegration[i] = numSubintegration.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		x->correlatorCalibrationExists = correlatorCalibrationExists;
		
		
			
				
		x->correlatorCalibration = correlatorCalibration;
 				
 			
		
	

	
	
		
	
  	
 		
		
	 	
			
		x->execBlockId = execBlockId.toIDLTag();
			
	 	 		
  	

	
		
	

		
		return x;
	
	}
	
	void SubscanRow::toIDL(asdmIDL::SubscanRowIDL& x) const {
		// Set the x's fields.
	
		
	
  		
		
		
			
				
		x.scanNumber = scanNumber;
 				
 			
		
	

	
  		
		
		
			
				
		x.subscanNumber = subscanNumber;
 				
 			
		
	

	
  		
		
		
			
		x.startTime = startTime.toIDLArrayTime();
			
		
	

	
  		
		
		
			
		x.endTime = endTime.toIDLArrayTime();
			
		
	

	
  		
		
		
			
				
		x.fieldName = CORBA::string_dup(fieldName.c_str());
				
 			
		
	

	
  		
		
		
			
				
		x.subscanIntent = subscanIntent;
 				
 			
		
	

	
  		
		
		x.subscanModeExists = subscanModeExists;
		
		
			
				
		x.subscanMode = subscanMode;
 				
 			
		
	

	
  		
		
		
			
				
		x.numIntegration = numIntegration;
 				
 			
		
	

	
  		
		
		
			
		x.numSubintegration.length(numSubintegration.size());
		for (unsigned int i = 0; i < numSubintegration.size(); ++i) {
			
				
			x.numSubintegration[i] = numSubintegration.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		x.correlatorCalibrationExists = correlatorCalibrationExists;
		
		
			
				
		x.correlatorCalibration = correlatorCalibration;
 				
 			
		
	

	
	
		
	
  	
 		
		
	 	
			
		x.execBlockId = execBlockId.toIDLTag();
			
	 	 		
  	

	
		
	

	
	}
#endif
	

#ifndef WITHOUT_ACS
	/**
	 * Fill the values of this row from the IDL struct SubscanRowIDL.
	 * @param x The IDL struct containing the values used to fill this row.
	 */
	void SubscanRow::setFromIDL (SubscanRowIDL x){
		try {
		// Fill the values from x.
	
		
	
		
		
			
		setScanNumber(x.scanNumber);
  			
 		
		
	

	
		
		
			
		setSubscanNumber(x.subscanNumber);
  			
 		
		
	

	
		
		
			
		setStartTime(ArrayTime (x.startTime));
			
 		
		
	

	
		
		
			
		setEndTime(ArrayTime (x.endTime));
			
 		
		
	

	
		
		
			
		setFieldName(string (x.fieldName));
			
 		
		
	

	
		
		
			
		setSubscanIntent(x.subscanIntent);
  			
 		
		
	

	
		
		subscanModeExists = x.subscanModeExists;
		if (x.subscanModeExists) {
		
		
			
		setSubscanMode(x.subscanMode);
  			
 		
		
		}
		
	

	
		
		
			
		setNumIntegration(x.numIntegration);
  			
 		
		
	

	
		
		
			
		numSubintegration .clear();
		for (unsigned int i = 0; i <x.numSubintegration.length(); ++i) {
			
			numSubintegration.push_back(x.numSubintegration[i]);
  			
		}
			
  		
		
	

	
		
		correlatorCalibrationExists = x.correlatorCalibrationExists;
		if (x.correlatorCalibrationExists) {
		
		
			
		setCorrelatorCalibration(x.correlatorCalibration);
  			
 		
		
		}
		
	

	
	
		
	
		
		
			
		setExecBlockId(Tag (x.execBlockId));
			
 		
		
	

	
		
	

		} catch (const IllegalAccessException &err) {
			throw ConversionException (err.getMessage(),"Subscan");
		}
	}
#endif
	
	/**
	 * Return this row in the form of an XML string.
	 * @return The values of this row as an XML string.
	 */
	string SubscanRow::toXML() const {
		string buf;
		buf.append("<row> \n");
		
	
		
  	
 		
		
		Parser::toXML(scanNumber, "scanNumber", buf);
		
		
	

  	
 		
		
		Parser::toXML(subscanNumber, "subscanNumber", buf);
		
		
	

  	
 		
		
		Parser::toXML(startTime, "startTime", buf);
		
		
	

  	
 		
		
		Parser::toXML(endTime, "endTime", buf);
		
		
	

  	
 		
		
		Parser::toXML(fieldName, "fieldName", buf);
		
		
	

  	
 		
		
			buf.append(EnumerationParser::toXML("subscanIntent", subscanIntent));
		
		
	

  	
 		
		if (subscanModeExists) {
		
		
			buf.append(EnumerationParser::toXML("subscanMode", subscanMode));
		
		
		}
		
	

  	
 		
		
		Parser::toXML(numIntegration, "numIntegration", buf);
		
		
	

  	
 		
		
		Parser::toXML(numSubintegration, "numSubintegration", buf);
		
		
	

  	
 		
		if (correlatorCalibrationExists) {
		
		
			buf.append(EnumerationParser::toXML("correlatorCalibration", correlatorCalibration));
		
		
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
	void SubscanRow::setFromXML (string rowDoc) {
		Parser row(rowDoc);
		string s = "";
		try {
	
		
	
  		
			
	  	setScanNumber(Parser::getInteger("scanNumber","Subscan",rowDoc));
			
		
	

	
  		
			
	  	setSubscanNumber(Parser::getInteger("subscanNumber","Subscan",rowDoc));
			
		
	

	
  		
			
	  	setStartTime(Parser::getArrayTime("startTime","Subscan",rowDoc));
			
		
	

	
  		
			
	  	setEndTime(Parser::getArrayTime("endTime","Subscan",rowDoc));
			
		
	

	
  		
			
	  	setFieldName(Parser::getString("fieldName","Subscan",rowDoc));
			
		
	

	
		
		
		
		subscanIntent = EnumerationParser::getSubscanIntent("subscanIntent","Subscan",rowDoc);
		
		
		
	

	
		
	if (row.isStr("<subscanMode>")) {
		
		
		
		subscanMode = EnumerationParser::getSwitchingMode("subscanMode","Subscan",rowDoc);
		
		
		
		subscanModeExists = true;
	}
		
	

	
  		
			
	  	setNumIntegration(Parser::getInteger("numIntegration","Subscan",rowDoc));
			
		
	

	
  		
			
					
	  	setNumSubintegration(Parser::get1DInteger("numSubintegration","Subscan",rowDoc));
	  			
	  		
		
	

	
		
	if (row.isStr("<correlatorCalibration>")) {
		
		
		
		correlatorCalibration = EnumerationParser::getCorrelatorCalibration("correlatorCalibration","Subscan",rowDoc);
		
		
		
		correlatorCalibrationExists = true;
	}
		
	

	
	
		
	
  		
			
	  	setExecBlockId(Parser::getTag("execBlockId","ExecBlock",rowDoc));
			
		
	

	
		
	

		} catch (const IllegalAccessException &err) {
			throw ConversionException (err.getMessage(),"Subscan");
		}
	}
	
	void SubscanRow::toBin(EndianOSStream& eoss) {
	
	
	
	
		
	execBlockId.toBin(eoss);
		
	

	
	
		
						
			eoss.writeInt(scanNumber);
				
		
	

	
	
		
						
			eoss.writeInt(subscanNumber);
				
		
	

	
	
		
	startTime.toBin(eoss);
		
	

	
	
		
	endTime.toBin(eoss);
		
	

	
	
		
						
			eoss.writeString(fieldName);
				
		
	

	
	
		
					
		eoss.writeString(CSubscanIntent::name(subscanIntent));
			/* eoss.writeInt(subscanIntent); */
				
		
	

	
	
		
						
			eoss.writeInt(numIntegration);
				
		
	

	
	
		
		
			
		eoss.writeInt((int) numSubintegration.size());
		for (unsigned int i = 0; i < numSubintegration.size(); i++)
				
			eoss.writeInt(numSubintegration.at(i));
				
				
						
		
	


	
	
	eoss.writeBoolean(subscanModeExists);
	if (subscanModeExists) {
	
	
	
		
					
		eoss.writeString(CSwitchingMode::name(subscanMode));
			/* eoss.writeInt(subscanMode); */
				
		
	

	}

	eoss.writeBoolean(correlatorCalibrationExists);
	if (correlatorCalibrationExists) {
	
	
	
		
					
		eoss.writeString(CCorrelatorCalibration::name(correlatorCalibration));
			/* eoss.writeInt(correlatorCalibration); */
				
		
	

	}

	}
	
void SubscanRow::execBlockIdFromBin(EndianIStream& eis) {
		
	
		
		
		execBlockId =  Tag::fromBin(eis);
		
	
	
}
void SubscanRow::scanNumberFromBin(EndianIStream& eis) {
		
	
	
		
			
		scanNumber =  eis.readInt();
			
		
	
	
}
void SubscanRow::subscanNumberFromBin(EndianIStream& eis) {
		
	
	
		
			
		subscanNumber =  eis.readInt();
			
		
	
	
}
void SubscanRow::startTimeFromBin(EndianIStream& eis) {
		
	
		
		
		startTime =  ArrayTime::fromBin(eis);
		
	
	
}
void SubscanRow::endTimeFromBin(EndianIStream& eis) {
		
	
		
		
		endTime =  ArrayTime::fromBin(eis);
		
	
	
}
void SubscanRow::fieldNameFromBin(EndianIStream& eis) {
		
	
	
		
			
		fieldName =  eis.readString();
			
		
	
	
}
void SubscanRow::subscanIntentFromBin(EndianIStream& eis) {
		
	
	
		
			
		subscanIntent = CSubscanIntent::literal(eis.readString());
			
		
	
	
}
void SubscanRow::numIntegrationFromBin(EndianIStream& eis) {
		
	
	
		
			
		numIntegration =  eis.readInt();
			
		
	
	
}
void SubscanRow::numSubintegrationFromBin(EndianIStream& eis) {
		
	
	
		
			
	
		numSubintegration.clear();
		
		unsigned int numSubintegrationDim1 = eis.readInt();
		for (unsigned int  i = 0 ; i < numSubintegrationDim1; i++)
			
			numSubintegration.push_back(eis.readInt());
			
	

		
	
	
}

void SubscanRow::subscanModeFromBin(EndianIStream& eis) {
		
	subscanModeExists = eis.readBoolean();
	if (subscanModeExists) {
		
	
	
		
			
		subscanMode = CSwitchingMode::literal(eis.readString());
			
		
	

	}
	
}
void SubscanRow::correlatorCalibrationFromBin(EndianIStream& eis) {
		
	correlatorCalibrationExists = eis.readBoolean();
	if (correlatorCalibrationExists) {
		
	
	
		
			
		correlatorCalibration = CCorrelatorCalibration::literal(eis.readString());
			
		
	

	}
	
}
	
	
	SubscanRow* SubscanRow::fromBin(EndianIStream& eis, SubscanTable& table, const vector<string>& attributesSeq) {
		SubscanRow* row = new  SubscanRow(table);
		
		map<string, SubscanAttributeFromBin>::iterator iter ;
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
					throw ConversionException("There is not method to read an attribute '"+attributesSeq.at(i)+"'.", "SubscanTable");
			}
				
		}				
		return row;
	}

	//
	// A collection of methods to set the value of the attributes from their textual value in the XML representation
	// of one row.
	//
	
	// Convert a string into an Tag 
	void SubscanRow::execBlockIdFromText(const string & s) {
		 
          
		execBlockId = ASDMValuesParser::parse<Tag>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void SubscanRow::scanNumberFromText(const string & s) {
		 
          
		scanNumber = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void SubscanRow::subscanNumberFromText(const string & s) {
		 
          
		subscanNumber = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	// Convert a string into an ArrayTime 
	void SubscanRow::startTimeFromText(const string & s) {
		 
          
		startTime = ASDMValuesParser::parse<ArrayTime>(s);
          
		
	}
	
	
	// Convert a string into an ArrayTime 
	void SubscanRow::endTimeFromText(const string & s) {
		 
          
		endTime = ASDMValuesParser::parse<ArrayTime>(s);
          
		
	}
	
	
	// Convert a string into an String 
	void SubscanRow::fieldNameFromText(const string & s) {
		 
          
		fieldName = ASDMValuesParser::parse<string>(s);
          
		
	}
	
	
	// Convert a string into an SubscanIntent 
	void SubscanRow::subscanIntentFromText(const string & s) {
		 
          
		subscanIntent = ASDMValuesParser::parse<SubscanIntentMod::SubscanIntent>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void SubscanRow::numIntegrationFromText(const string & s) {
		 
          
		numIntegration = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void SubscanRow::numSubintegrationFromText(const string & s) {
		 
          
		numSubintegration = ASDMValuesParser::parse1D<int>(s);
          
		
	}
	

	
	// Convert a string into an SwitchingMode 
	void SubscanRow::subscanModeFromText(const string & s) {
		subscanModeExists = true;
		 
          
		subscanMode = ASDMValuesParser::parse<SwitchingModeMod::SwitchingMode>(s);
          
		
	}
	
	
	// Convert a string into an CorrelatorCalibration 
	void SubscanRow::correlatorCalibrationFromText(const string & s) {
		correlatorCalibrationExists = true;
		 
          
		correlatorCalibration = ASDMValuesParser::parse<CorrelatorCalibrationMod::CorrelatorCalibration>(s);
          
		
	}
	
	
	
	void SubscanRow::fromText(const std::string& attributeName, const std::string&  t) {
		map<string, SubscanAttributeFromText>::iterator iter;
		if ((iter = fromTextMethods.find(attributeName)) == fromTextMethods.end())
			throw ConversionException("I do not know what to do with '"+attributeName+"' and its content '"+t+"' (while parsing an XML document)", "SubscanTable");
		(this->*(iter->second))(t);
	}
			
	////////////////////////////////////////////////
	// Intrinsic Table Attributes getters/setters //
	////////////////////////////////////////////////
	
	

	
 	/**
 	 * Get scanNumber.
 	 * @return scanNumber as int
 	 */
 	int SubscanRow::getScanNumber() const {
	
  		return scanNumber;
 	}

 	/**
 	 * Set scanNumber with the specified int.
 	 * @param scanNumber The int value to which scanNumber is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void SubscanRow::setScanNumber (int scanNumber)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("scanNumber", "Subscan");
		
  		}
  	
 		this->scanNumber = scanNumber;
	
 	}
	
	

	

	
 	/**
 	 * Get subscanNumber.
 	 * @return subscanNumber as int
 	 */
 	int SubscanRow::getSubscanNumber() const {
	
  		return subscanNumber;
 	}

 	/**
 	 * Set subscanNumber with the specified int.
 	 * @param subscanNumber The int value to which subscanNumber is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void SubscanRow::setSubscanNumber (int subscanNumber)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("subscanNumber", "Subscan");
		
  		}
  	
 		this->subscanNumber = subscanNumber;
	
 	}
	
	

	

	
 	/**
 	 * Get startTime.
 	 * @return startTime as ArrayTime
 	 */
 	ArrayTime SubscanRow::getStartTime() const {
	
  		return startTime;
 	}

 	/**
 	 * Set startTime with the specified ArrayTime.
 	 * @param startTime The ArrayTime value to which startTime is to be set.
 	 
 	
 		
 	 */
 	void SubscanRow::setStartTime (ArrayTime startTime)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->startTime = startTime;
	
 	}
	
	

	

	
 	/**
 	 * Get endTime.
 	 * @return endTime as ArrayTime
 	 */
 	ArrayTime SubscanRow::getEndTime() const {
	
  		return endTime;
 	}

 	/**
 	 * Set endTime with the specified ArrayTime.
 	 * @param endTime The ArrayTime value to which endTime is to be set.
 	 
 	
 		
 	 */
 	void SubscanRow::setEndTime (ArrayTime endTime)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->endTime = endTime;
	
 	}
	
	

	

	
 	/**
 	 * Get fieldName.
 	 * @return fieldName as std::string
 	 */
 	std::string SubscanRow::getFieldName() const {
	
  		return fieldName;
 	}

 	/**
 	 * Set fieldName with the specified std::string.
 	 * @param fieldName The std::string value to which fieldName is to be set.
 	 
 	
 		
 	 */
 	void SubscanRow::setFieldName (std::string fieldName)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->fieldName = fieldName;
	
 	}
	
	

	

	
 	/**
 	 * Get subscanIntent.
 	 * @return subscanIntent as SubscanIntentMod::SubscanIntent
 	 */
 	SubscanIntentMod::SubscanIntent SubscanRow::getSubscanIntent() const {
	
  		return subscanIntent;
 	}

 	/**
 	 * Set subscanIntent with the specified SubscanIntentMod::SubscanIntent.
 	 * @param subscanIntent The SubscanIntentMod::SubscanIntent value to which subscanIntent is to be set.
 	 
 	
 		
 	 */
 	void SubscanRow::setSubscanIntent (SubscanIntentMod::SubscanIntent subscanIntent)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->subscanIntent = subscanIntent;
	
 	}
	
	

	
	/**
	 * The attribute subscanMode is optional. Return true if this attribute exists.
	 * @return true if and only if the subscanMode attribute exists. 
	 */
	bool SubscanRow::isSubscanModeExists() const {
		return subscanModeExists;
	}
	

	
 	/**
 	 * Get subscanMode, which is optional.
 	 * @return subscanMode as SwitchingModeMod::SwitchingMode
 	 * @throw IllegalAccessException If subscanMode does not exist.
 	 */
 	SwitchingModeMod::SwitchingMode SubscanRow::getSubscanMode() const  {
		if (!subscanModeExists) {
			throw IllegalAccessException("subscanMode", "Subscan");
		}
	
  		return subscanMode;
 	}

 	/**
 	 * Set subscanMode with the specified SwitchingModeMod::SwitchingMode.
 	 * @param subscanMode The SwitchingModeMod::SwitchingMode value to which subscanMode is to be set.
 	 
 	
 	 */
 	void SubscanRow::setSubscanMode (SwitchingModeMod::SwitchingMode subscanMode) {
	
 		this->subscanMode = subscanMode;
	
		subscanModeExists = true;
	
 	}
	
	
	/**
	 * Mark subscanMode, which is an optional field, as non-existent.
	 */
	void SubscanRow::clearSubscanMode () {
		subscanModeExists = false;
	}
	

	

	
 	/**
 	 * Get numIntegration.
 	 * @return numIntegration as int
 	 */
 	int SubscanRow::getNumIntegration() const {
	
  		return numIntegration;
 	}

 	/**
 	 * Set numIntegration with the specified int.
 	 * @param numIntegration The int value to which numIntegration is to be set.
 	 
 	
 		
 	 */
 	void SubscanRow::setNumIntegration (int numIntegration)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->numIntegration = numIntegration;
	
 	}
	
	

	

	
 	/**
 	 * Get numSubintegration.
 	 * @return numSubintegration as std::vector<int >
 	 */
 	std::vector<int > SubscanRow::getNumSubintegration() const {
	
  		return numSubintegration;
 	}

 	/**
 	 * Set numSubintegration with the specified std::vector<int >.
 	 * @param numSubintegration The std::vector<int > value to which numSubintegration is to be set.
 	 
 	
 		
 	 */
 	void SubscanRow::setNumSubintegration (std::vector<int > numSubintegration)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->numSubintegration = numSubintegration;
	
 	}
	
	

	
	/**
	 * The attribute correlatorCalibration is optional. Return true if this attribute exists.
	 * @return true if and only if the correlatorCalibration attribute exists. 
	 */
	bool SubscanRow::isCorrelatorCalibrationExists() const {
		return correlatorCalibrationExists;
	}
	

	
 	/**
 	 * Get correlatorCalibration, which is optional.
 	 * @return correlatorCalibration as CorrelatorCalibrationMod::CorrelatorCalibration
 	 * @throw IllegalAccessException If correlatorCalibration does not exist.
 	 */
 	CorrelatorCalibrationMod::CorrelatorCalibration SubscanRow::getCorrelatorCalibration() const  {
		if (!correlatorCalibrationExists) {
			throw IllegalAccessException("correlatorCalibration", "Subscan");
		}
	
  		return correlatorCalibration;
 	}

 	/**
 	 * Set correlatorCalibration with the specified CorrelatorCalibrationMod::CorrelatorCalibration.
 	 * @param correlatorCalibration The CorrelatorCalibrationMod::CorrelatorCalibration value to which correlatorCalibration is to be set.
 	 
 	
 	 */
 	void SubscanRow::setCorrelatorCalibration (CorrelatorCalibrationMod::CorrelatorCalibration correlatorCalibration) {
	
 		this->correlatorCalibration = correlatorCalibration;
	
		correlatorCalibrationExists = true;
	
 	}
	
	
	/**
	 * Mark correlatorCalibration, which is an optional field, as non-existent.
	 */
	void SubscanRow::clearCorrelatorCalibration () {
		correlatorCalibrationExists = false;
	}
	

	
	///////////////////////////////////////////////
	// Extrinsic Table Attributes getters/setters//
	///////////////////////////////////////////////
	
	

	
 	/**
 	 * Get execBlockId.
 	 * @return execBlockId as Tag
 	 */
 	Tag SubscanRow::getExecBlockId() const {
	
  		return execBlockId;
 	}

 	/**
 	 * Set execBlockId with the specified Tag.
 	 * @param execBlockId The Tag value to which execBlockId is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void SubscanRow::setExecBlockId (Tag execBlockId)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("execBlockId", "Subscan");
		
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
	 ExecBlockRow* SubscanRow::getExecBlockUsingExecBlockId() {
	 
	 	return table.getContainer().getExecBlock().getRowByKey(execBlockId);
	 }
	 

	

	
	/**
	 * Create a SubscanRow.
	 * <p>
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 */ 
	SubscanRow::SubscanRow (SubscanTable &t) : table(t) {
		hasBeenAdded = false;
		
	
	

	

	

	

	

	

	
		subscanModeExists = false;
	

	

	

	
		correlatorCalibrationExists = false;
	

	
	

	
	
	
	

	

	

	

	

	
// This attribute is scalar and has an enumeration type. Let's initialize it to some valid value (the 1st of the enumeration).		
subscanIntent = CSubscanIntent::from_int(0);
	

	
// This attribute is scalar and has an enumeration type. Let's initialize it to some valid value (the 1st of the enumeration).		
subscanMode = CSwitchingMode::from_int(0);
	

	

	

	
// This attribute is scalar and has an enumeration type. Let's initialize it to some valid value (the 1st of the enumeration).		
correlatorCalibration = CCorrelatorCalibration::from_int(0);
	

	
	
	 fromBinMethods["execBlockId"] = &SubscanRow::execBlockIdFromBin; 
	 fromBinMethods["scanNumber"] = &SubscanRow::scanNumberFromBin; 
	 fromBinMethods["subscanNumber"] = &SubscanRow::subscanNumberFromBin; 
	 fromBinMethods["startTime"] = &SubscanRow::startTimeFromBin; 
	 fromBinMethods["endTime"] = &SubscanRow::endTimeFromBin; 
	 fromBinMethods["fieldName"] = &SubscanRow::fieldNameFromBin; 
	 fromBinMethods["subscanIntent"] = &SubscanRow::subscanIntentFromBin; 
	 fromBinMethods["numIntegration"] = &SubscanRow::numIntegrationFromBin; 
	 fromBinMethods["numSubintegration"] = &SubscanRow::numSubintegrationFromBin; 
		
	
	 fromBinMethods["subscanMode"] = &SubscanRow::subscanModeFromBin; 
	 fromBinMethods["correlatorCalibration"] = &SubscanRow::correlatorCalibrationFromBin; 
	
	
	
	
				 
	fromTextMethods["execBlockId"] = &SubscanRow::execBlockIdFromText;
		 
	
				 
	fromTextMethods["scanNumber"] = &SubscanRow::scanNumberFromText;
		 
	
				 
	fromTextMethods["subscanNumber"] = &SubscanRow::subscanNumberFromText;
		 
	
				 
	fromTextMethods["startTime"] = &SubscanRow::startTimeFromText;
		 
	
				 
	fromTextMethods["endTime"] = &SubscanRow::endTimeFromText;
		 
	
				 
	fromTextMethods["fieldName"] = &SubscanRow::fieldNameFromText;
		 
	
				 
	fromTextMethods["subscanIntent"] = &SubscanRow::subscanIntentFromText;
		 
	
				 
	fromTextMethods["numIntegration"] = &SubscanRow::numIntegrationFromText;
		 
	
				 
	fromTextMethods["numSubintegration"] = &SubscanRow::numSubintegrationFromText;
		 
	

	 
				
	fromTextMethods["subscanMode"] = &SubscanRow::subscanModeFromText;
		 	
	 
				
	fromTextMethods["correlatorCalibration"] = &SubscanRow::correlatorCalibrationFromText;
		 	
		
	}
	
	SubscanRow::SubscanRow (SubscanTable &t, SubscanRow *row) : table(t) {
		hasBeenAdded = false;
		
		if (row == 0) {
	
	
	

	

	

	

	

	

	
		subscanModeExists = false;
	

	

	

	
		correlatorCalibrationExists = false;
	

	
	
		
		}
		else {
	
		
			execBlockId = row->execBlockId;
		
			scanNumber = row->scanNumber;
		
			subscanNumber = row->subscanNumber;
		
		
		
		
			startTime = row->startTime;
		
			endTime = row->endTime;
		
			fieldName = row->fieldName;
		
			subscanIntent = row->subscanIntent;
		
			numIntegration = row->numIntegration;
		
			numSubintegration = row->numSubintegration;
		
		
		
		
		if (row->subscanModeExists) {
			subscanMode = row->subscanMode;		
			subscanModeExists = true;
		}
		else
			subscanModeExists = false;
		
		if (row->correlatorCalibrationExists) {
			correlatorCalibration = row->correlatorCalibration;		
			correlatorCalibrationExists = true;
		}
		else
			correlatorCalibrationExists = false;
		
		}
		
		 fromBinMethods["execBlockId"] = &SubscanRow::execBlockIdFromBin; 
		 fromBinMethods["scanNumber"] = &SubscanRow::scanNumberFromBin; 
		 fromBinMethods["subscanNumber"] = &SubscanRow::subscanNumberFromBin; 
		 fromBinMethods["startTime"] = &SubscanRow::startTimeFromBin; 
		 fromBinMethods["endTime"] = &SubscanRow::endTimeFromBin; 
		 fromBinMethods["fieldName"] = &SubscanRow::fieldNameFromBin; 
		 fromBinMethods["subscanIntent"] = &SubscanRow::subscanIntentFromBin; 
		 fromBinMethods["numIntegration"] = &SubscanRow::numIntegrationFromBin; 
		 fromBinMethods["numSubintegration"] = &SubscanRow::numSubintegrationFromBin; 
			
	
		 fromBinMethods["subscanMode"] = &SubscanRow::subscanModeFromBin; 
		 fromBinMethods["correlatorCalibration"] = &SubscanRow::correlatorCalibrationFromBin; 
			
	}

	
	bool SubscanRow::compareNoAutoInc(Tag execBlockId, int scanNumber, int subscanNumber, ArrayTime startTime, ArrayTime endTime, std::string fieldName, SubscanIntentMod::SubscanIntent subscanIntent, int numIntegration, std::vector<int > numSubintegration) {
		bool result;
		result = true;
		
	
		
		result = result && (this->execBlockId == execBlockId);
		
		if (!result) return false;
	

	
		
		result = result && (this->scanNumber == scanNumber);
		
		if (!result) return false;
	

	
		
		result = result && (this->subscanNumber == subscanNumber);
		
		if (!result) return false;
	

	
		
		result = result && (this->startTime == startTime);
		
		if (!result) return false;
	

	
		
		result = result && (this->endTime == endTime);
		
		if (!result) return false;
	

	
		
		result = result && (this->fieldName == fieldName);
		
		if (!result) return false;
	

	
		
		result = result && (this->subscanIntent == subscanIntent);
		
		if (!result) return false;
	

	
		
		result = result && (this->numIntegration == numIntegration);
		
		if (!result) return false;
	

	
		
		result = result && (this->numSubintegration == numSubintegration);
		
		if (!result) return false;
	

		return result;
	}	
	
	
	
	bool SubscanRow::compareRequiredValue(ArrayTime startTime, ArrayTime endTime, std::string fieldName, SubscanIntentMod::SubscanIntent subscanIntent, int numIntegration, std::vector<int > numSubintegration) {
		bool result;
		result = true;
		
	
		if (!(this->startTime == startTime)) return false;
	

	
		if (!(this->endTime == endTime)) return false;
	

	
		if (!(this->fieldName == fieldName)) return false;
	

	
		if (!(this->subscanIntent == subscanIntent)) return false;
	

	
		if (!(this->numIntegration == numIntegration)) return false;
	

	
		if (!(this->numSubintegration == numSubintegration)) return false;
	

		return result;
	}
	
	
	/**
	 * Return true if all required attributes of the value part are equal to their homologues
	 * in x and false otherwise.
	 *

	 * @param x a pointer on the SubscanRow whose required attributes of the value part 

	 * will be compared with those of this.
	 * @return a boolean.
	 */
	bool SubscanRow::equalByRequiredValue(SubscanRow*  x ) {
		
			
		if (this->startTime != x->startTime) return false;
			
		if (this->endTime != x->endTime) return false;
			
		if (this->fieldName != x->fieldName) return false;
			
		if (this->subscanIntent != x->subscanIntent) return false;
			
		if (this->numIntegration != x->numIntegration) return false;
			
		if (this->numSubintegration != x->numSubintegration) return false;
			
		
		return true;
	}	
	
/*
	 map<string, SubscanAttributeFromBin> SubscanRow::initFromBinMethods() {
		map<string, SubscanAttributeFromBin> result;
		
		result["execBlockId"] = &SubscanRow::execBlockIdFromBin;
		result["scanNumber"] = &SubscanRow::scanNumberFromBin;
		result["subscanNumber"] = &SubscanRow::subscanNumberFromBin;
		result["startTime"] = &SubscanRow::startTimeFromBin;
		result["endTime"] = &SubscanRow::endTimeFromBin;
		result["fieldName"] = &SubscanRow::fieldNameFromBin;
		result["subscanIntent"] = &SubscanRow::subscanIntentFromBin;
		result["numIntegration"] = &SubscanRow::numIntegrationFromBin;
		result["numSubintegration"] = &SubscanRow::numSubintegrationFromBin;
		
		
		result["subscanMode"] = &SubscanRow::subscanModeFromBin;
		result["correlatorCalibration"] = &SubscanRow::correlatorCalibrationFromBin;
			
		
		return result;	
	}
*/	
} // End namespace asdm
 
