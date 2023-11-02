
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
 * File CalReductionRow.cpp
 */
 
#include <vector>
#include <set>

#include <alma/ASDM/ASDM.h>
#include <alma/ASDM/CalReductionRow.h>
#include <alma/ASDM/CalReductionTable.h>
	

using asdm::ASDM;
using asdm::CalReductionRow;
using asdm::CalReductionTable;


#include <alma/ASDM/Parser.h>

#include <alma/ASDM/EnumerationParser.h>
#include <alma/ASDM/ASDMValuesParser.h>
 
#include <alma/ASDM/InvalidArgumentException.h>

using namespace std;

namespace asdm {
	CalReductionRow::~CalReductionRow() {
	}

	/**
	 * Return the table to which this row belongs.
	 */
	CalReductionTable &CalReductionRow::getTable() const {
		return table;
	}

	bool CalReductionRow::isAdded() const {
		return hasBeenAdded;
	}	

	void CalReductionRow::isAdded(bool added) {
		hasBeenAdded = added;
	}
	
#ifndef WITHOUT_ACS
	using asdmIDL::CalReductionRowIDL;
#endif
	
#ifndef WITHOUT_ACS
	/**
	 * Return this row in the form of an IDL struct.
	 * @return The values of this row as a CalReductionRowIDL struct.
	 */
	CalReductionRowIDL *CalReductionRow::toIDL() const {
		CalReductionRowIDL *x = new CalReductionRowIDL ();
		
		// Fill the IDL structure.
	
		
	
  		
		
		
			
		x->calReductionId = calReductionId.toIDLTag();
			
		
	

	
  		
		
		
			
				
		x->numApplied = numApplied;
 				
 			
		
	

	
  		
		
		
			
		x->appliedCalibrations.length(appliedCalibrations.size());
		for (unsigned int i = 0; i < appliedCalibrations.size(); ++i) {
			
				
			x->appliedCalibrations[i] = CORBA::string_dup(appliedCalibrations.at(i).c_str());
				
	 		
	 	}
			
		
	

	
  		
		
		
			
				
		x->numParam = numParam;
 				
 			
		
	

	
  		
		
		
			
		x->paramSet.length(paramSet.size());
		for (unsigned int i = 0; i < paramSet.size(); ++i) {
			
				
			x->paramSet[i] = CORBA::string_dup(paramSet.at(i).c_str());
				
	 		
	 	}
			
		
	

	
  		
		
		
			
				
		x->numInvalidConditions = numInvalidConditions;
 				
 			
		
	

	
  		
		
		
			
		x->invalidConditions.length(invalidConditions.size());
		for (unsigned int i = 0; i < invalidConditions.size(); ++i) {
			
				
			x->invalidConditions[i] = invalidConditions.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		
			
		x->timeReduced = timeReduced.toIDLArrayTime();
			
		
	

	
  		
		
		
			
				
		x->messages = CORBA::string_dup(messages.c_str());
				
 			
		
	

	
  		
		
		
			
				
		x->software = CORBA::string_dup(software.c_str());
				
 			
		
	

	
  		
		
		
			
				
		x->softwareVersion = CORBA::string_dup(softwareVersion.c_str());
				
 			
		
	

	
	
		
		
		return x;
	
	}
	
	void CalReductionRow::toIDL(asdmIDL::CalReductionRowIDL& x) const {
		// Set the x's fields.
	
		
	
  		
		
		
			
		x.calReductionId = calReductionId.toIDLTag();
			
		
	

	
  		
		
		
			
				
		x.numApplied = numApplied;
 				
 			
		
	

	
  		
		
		
			
		x.appliedCalibrations.length(appliedCalibrations.size());
		for (unsigned int i = 0; i < appliedCalibrations.size(); ++i) {
			
				
			x.appliedCalibrations[i] = CORBA::string_dup(appliedCalibrations.at(i).c_str());
				
	 		
	 	}
			
		
	

	
  		
		
		
			
				
		x.numParam = numParam;
 				
 			
		
	

	
  		
		
		
			
		x.paramSet.length(paramSet.size());
		for (unsigned int i = 0; i < paramSet.size(); ++i) {
			
				
			x.paramSet[i] = CORBA::string_dup(paramSet.at(i).c_str());
				
	 		
	 	}
			
		
	

	
  		
		
		
			
				
		x.numInvalidConditions = numInvalidConditions;
 				
 			
		
	

	
  		
		
		
			
		x.invalidConditions.length(invalidConditions.size());
		for (unsigned int i = 0; i < invalidConditions.size(); ++i) {
			
				
			x.invalidConditions[i] = invalidConditions.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		
			
		x.timeReduced = timeReduced.toIDLArrayTime();
			
		
	

	
  		
		
		
			
				
		x.messages = CORBA::string_dup(messages.c_str());
				
 			
		
	

	
  		
		
		
			
				
		x.software = CORBA::string_dup(software.c_str());
				
 			
		
	

	
  		
		
		
			
				
		x.softwareVersion = CORBA::string_dup(softwareVersion.c_str());
				
 			
		
	

	
	
		
	
	}
#endif
	

#ifndef WITHOUT_ACS
	/**
	 * Fill the values of this row from the IDL struct CalReductionRowIDL.
	 * @param x The IDL struct containing the values used to fill this row.
	 */
	void CalReductionRow::setFromIDL (CalReductionRowIDL x){
		try {
		// Fill the values from x.
	
		
	
		
		
			
		setCalReductionId(Tag (x.calReductionId));
			
 		
		
	

	
		
		
			
		setNumApplied(x.numApplied);
  			
 		
		
	

	
		
		
			
		appliedCalibrations .clear();
		for (unsigned int i = 0; i <x.appliedCalibrations.length(); ++i) {
			
			appliedCalibrations.push_back(string (x.appliedCalibrations[i]));
			
		}
			
  		
		
	

	
		
		
			
		setNumParam(x.numParam);
  			
 		
		
	

	
		
		
			
		paramSet .clear();
		for (unsigned int i = 0; i <x.paramSet.length(); ++i) {
			
			paramSet.push_back(string (x.paramSet[i]));
			
		}
			
  		
		
	

	
		
		
			
		setNumInvalidConditions(x.numInvalidConditions);
  			
 		
		
	

	
		
		
			
		invalidConditions .clear();
		for (unsigned int i = 0; i <x.invalidConditions.length(); ++i) {
			
			invalidConditions.push_back(x.invalidConditions[i]);
  			
		}
			
  		
		
	

	
		
		
			
		setTimeReduced(ArrayTime (x.timeReduced));
			
 		
		
	

	
		
		
			
		setMessages(string (x.messages));
			
 		
		
	

	
		
		
			
		setSoftware(string (x.software));
			
 		
		
	

	
		
		
			
		setSoftwareVersion(string (x.softwareVersion));
			
 		
		
	

	
	
		
		} catch (const IllegalAccessException &err) {
			throw ConversionException (err.getMessage(),"CalReduction");
		}
	}
#endif
	
	/**
	 * Return this row in the form of an XML string.
	 * @return The values of this row as an XML string.
	 */
	string CalReductionRow::toXML() const {
		string buf;
		buf.append("<row> \n");
		
	
		
  	
 		
		
		Parser::toXML(calReductionId, "calReductionId", buf);
		
		
	

  	
 		
		
		Parser::toXML(numApplied, "numApplied", buf);
		
		
	

  	
 		
		
		Parser::toXML(appliedCalibrations, "appliedCalibrations", buf);
		
		
	

  	
 		
		
		Parser::toXML(numParam, "numParam", buf);
		
		
	

  	
 		
		
		Parser::toXML(paramSet, "paramSet", buf);
		
		
	

  	
 		
		
		Parser::toXML(numInvalidConditions, "numInvalidConditions", buf);
		
		
	

  	
 		
		
			buf.append(EnumerationParser::toXML("invalidConditions", invalidConditions));
		
		
	

  	
 		
		
		Parser::toXML(timeReduced, "timeReduced", buf);
		
		
	

  	
 		
		
		Parser::toXML(messages, "messages", buf);
		
		
	

  	
 		
		
		Parser::toXML(software, "software", buf);
		
		
	

  	
 		
		
		Parser::toXML(softwareVersion, "softwareVersion", buf);
		
		
	

	
	
		
		
		buf.append("</row>\n");
		return buf;
	}

	/**
	 * Fill the values of this row from an XML string 
	 * that was produced by the toXML() method.
	 * @param x The XML string being used to set the values of this row.
	 */
	void CalReductionRow::setFromXML (string rowDoc) {
		Parser row(rowDoc);
		string s = "";
		try {
	
		
	
  		
			
	  	setCalReductionId(Parser::getTag("calReductionId","CalReduction",rowDoc));
			
		
	

	
  		
			
	  	setNumApplied(Parser::getInteger("numApplied","CalReduction",rowDoc));
			
		
	

	
  		
			
					
	  	setAppliedCalibrations(Parser::get1DString("appliedCalibrations","CalReduction",rowDoc));
	  			
	  		
		
	

	
  		
			
	  	setNumParam(Parser::getInteger("numParam","CalReduction",rowDoc));
			
		
	

	
  		
			
					
	  	setParamSet(Parser::get1DString("paramSet","CalReduction",rowDoc));
	  			
	  		
		
	

	
  		
			
	  	setNumInvalidConditions(Parser::getInteger("numInvalidConditions","CalReduction",rowDoc));
			
		
	

	
		
		
		
		invalidConditions = EnumerationParser::getInvalidatingCondition1D("invalidConditions","CalReduction",rowDoc);			
		
		
		
	

	
  		
			
	  	setTimeReduced(Parser::getArrayTime("timeReduced","CalReduction",rowDoc));
			
		
	

	
  		
			
	  	setMessages(Parser::getString("messages","CalReduction",rowDoc));
			
		
	

	
  		
			
	  	setSoftware(Parser::getString("software","CalReduction",rowDoc));
			
		
	

	
  		
			
	  	setSoftwareVersion(Parser::getString("softwareVersion","CalReduction",rowDoc));
			
		
	

	
	
		
		} catch (const IllegalAccessException &err) {
			throw ConversionException (err.getMessage(),"CalReduction");
		}
	}
	
	void CalReductionRow::toBin(EndianOSStream& eoss) {
	
	
	
	
		
	calReductionId.toBin(eoss);
		
	

	
	
		
						
			eoss.writeInt(numApplied);
				
		
	

	
	
		
		
			
		eoss.writeInt((int) appliedCalibrations.size());
		for (unsigned int i = 0; i < appliedCalibrations.size(); i++)
				
			eoss.writeString(appliedCalibrations.at(i));
				
				
						
		
	

	
	
		
						
			eoss.writeInt(numParam);
				
		
	

	
	
		
		
			
		eoss.writeInt((int) paramSet.size());
		for (unsigned int i = 0; i < paramSet.size(); i++)
				
			eoss.writeString(paramSet.at(i));
				
				
						
		
	

	
	
		
						
			eoss.writeInt(numInvalidConditions);
				
		
	

	
	
		
		
			
		eoss.writeInt((int) invalidConditions.size());
		for (unsigned int i = 0; i < invalidConditions.size(); i++)
				
			eoss.writeString(CInvalidatingCondition::name(invalidConditions.at(i)));
			/* eoss.writeInt(invalidConditions.at(i)); */
				
				
						
		
	

	
	
		
	timeReduced.toBin(eoss);
		
	

	
	
		
						
			eoss.writeString(messages);
				
		
	

	
	
		
						
			eoss.writeString(software);
				
		
	

	
	
		
						
			eoss.writeString(softwareVersion);
				
		
	


	
	
	}
	
void CalReductionRow::calReductionIdFromBin(EndianIStream& eis) {
		
	
		
		
		calReductionId =  Tag::fromBin(eis);
		
	
	
}
void CalReductionRow::numAppliedFromBin(EndianIStream& eis) {
		
	
	
		
			
		numApplied =  eis.readInt();
			
		
	
	
}
void CalReductionRow::appliedCalibrationsFromBin(EndianIStream& eis) {
		
	
	
		
			
	
		appliedCalibrations.clear();
		
		unsigned int appliedCalibrationsDim1 = eis.readInt();
		for (unsigned int  i = 0 ; i < appliedCalibrationsDim1; i++)
			
			appliedCalibrations.push_back(eis.readString());
			
	

		
	
	
}
void CalReductionRow::numParamFromBin(EndianIStream& eis) {
		
	
	
		
			
		numParam =  eis.readInt();
			
		
	
	
}
void CalReductionRow::paramSetFromBin(EndianIStream& eis) {
		
	
	
		
			
	
		paramSet.clear();
		
		unsigned int paramSetDim1 = eis.readInt();
		for (unsigned int  i = 0 ; i < paramSetDim1; i++)
			
			paramSet.push_back(eis.readString());
			
	

		
	
	
}
void CalReductionRow::numInvalidConditionsFromBin(EndianIStream& eis) {
		
	
	
		
			
		numInvalidConditions =  eis.readInt();
			
		
	
	
}
void CalReductionRow::invalidConditionsFromBin(EndianIStream& eis) {
		
	
	
		
			
	
		invalidConditions.clear();
		
		unsigned int invalidConditionsDim1 = eis.readInt();
		for (unsigned int  i = 0 ; i < invalidConditionsDim1; i++)
			
			invalidConditions.push_back(CInvalidatingCondition::literal(eis.readString()));
			
	

		
	
	
}
void CalReductionRow::timeReducedFromBin(EndianIStream& eis) {
		
	
		
		
		timeReduced =  ArrayTime::fromBin(eis);
		
	
	
}
void CalReductionRow::messagesFromBin(EndianIStream& eis) {
		
	
	
		
			
		messages =  eis.readString();
			
		
	
	
}
void CalReductionRow::softwareFromBin(EndianIStream& eis) {
		
	
	
		
			
		software =  eis.readString();
			
		
	
	
}
void CalReductionRow::softwareVersionFromBin(EndianIStream& eis) {
		
	
	
		
			
		softwareVersion =  eis.readString();
			
		
	
	
}

		
	
	CalReductionRow* CalReductionRow::fromBin(EndianIStream& eis, CalReductionTable& table, const vector<string>& attributesSeq) {
		CalReductionRow* row = new  CalReductionRow(table);
		
		map<string, CalReductionAttributeFromBin>::iterator iter ;
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
					throw ConversionException("There is not method to read an attribute '"+attributesSeq.at(i)+"'.", "CalReductionTable");
			}
				
		}				
		return row;
	}

	//
	// A collection of methods to set the value of the attributes from their textual value in the XML representation
	// of one row.
	//
	
	// Convert a string into an Tag 
	void CalReductionRow::calReductionIdFromText(const string & s) {
		 
          
		calReductionId = ASDMValuesParser::parse<Tag>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void CalReductionRow::numAppliedFromText(const string & s) {
		 
          
		numApplied = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	// Convert a string into an String 
	void CalReductionRow::appliedCalibrationsFromText(const string & s) {
		 
          
		appliedCalibrations = ASDMValuesParser::parse1D<string>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void CalReductionRow::numParamFromText(const string & s) {
		 
          
		numParam = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	// Convert a string into an String 
	void CalReductionRow::paramSetFromText(const string & s) {
		 
          
		paramSet = ASDMValuesParser::parse1D<string>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void CalReductionRow::numInvalidConditionsFromText(const string & s) {
		 
          
		numInvalidConditions = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	// Convert a string into an InvalidatingCondition 
	void CalReductionRow::invalidConditionsFromText(const string & s) {
		 
          
		invalidConditions = ASDMValuesParser::parse1D<InvalidatingConditionMod::InvalidatingCondition>(s);
          
		
	}
	
	
	// Convert a string into an ArrayTime 
	void CalReductionRow::timeReducedFromText(const string & s) {
		 
          
		timeReduced = ASDMValuesParser::parse<ArrayTime>(s);
          
		
	}
	
	
	// Convert a string into an String 
	void CalReductionRow::messagesFromText(const string & s) {
		 
          
		messages = ASDMValuesParser::parse<string>(s);
          
		
	}
	
	
	// Convert a string into an String 
	void CalReductionRow::softwareFromText(const string & s) {
		 
          
		software = ASDMValuesParser::parse<string>(s);
          
		
	}
	
	
	// Convert a string into an String 
	void CalReductionRow::softwareVersionFromText(const string & s) {
		 
          
		softwareVersion = ASDMValuesParser::parse<string>(s);
          
		
	}
	

		
	
	void CalReductionRow::fromText(const std::string& attributeName, const std::string&  t) {
		map<string, CalReductionAttributeFromText>::iterator iter;
		if ((iter = fromTextMethods.find(attributeName)) == fromTextMethods.end())
			throw ConversionException("I do not know what to do with '"+attributeName+"' and its content '"+t+"' (while parsing an XML document)", "CalReductionTable");
		(this->*(iter->second))(t);
	}
			
	////////////////////////////////////////////////
	// Intrinsic Table Attributes getters/setters //
	////////////////////////////////////////////////
	
	

	
 	/**
 	 * Get calReductionId.
 	 * @return calReductionId as Tag
 	 */
 	Tag CalReductionRow::getCalReductionId() const {
	
  		return calReductionId;
 	}

 	/**
 	 * Set calReductionId with the specified Tag.
 	 * @param calReductionId The Tag value to which calReductionId is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void CalReductionRow::setCalReductionId (Tag calReductionId)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("calReductionId", "CalReduction");
		
  		}
  	
 		this->calReductionId = calReductionId;
	
 	}
	
	

	

	
 	/**
 	 * Get numApplied.
 	 * @return numApplied as int
 	 */
 	int CalReductionRow::getNumApplied() const {
	
  		return numApplied;
 	}

 	/**
 	 * Set numApplied with the specified int.
 	 * @param numApplied The int value to which numApplied is to be set.
 	 
 	
 		
 	 */
 	void CalReductionRow::setNumApplied (int numApplied)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->numApplied = numApplied;
	
 	}
	
	

	

	
 	/**
 	 * Get appliedCalibrations.
 	 * @return appliedCalibrations as std::vector<std::string >
 	 */
 	std::vector<std::string > CalReductionRow::getAppliedCalibrations() const {
	
  		return appliedCalibrations;
 	}

 	/**
 	 * Set appliedCalibrations with the specified std::vector<std::string >.
 	 * @param appliedCalibrations The std::vector<std::string > value to which appliedCalibrations is to be set.
 	 
 	
 		
 	 */
 	void CalReductionRow::setAppliedCalibrations (std::vector<std::string > appliedCalibrations)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->appliedCalibrations = appliedCalibrations;
	
 	}
	
	

	

	
 	/**
 	 * Get numParam.
 	 * @return numParam as int
 	 */
 	int CalReductionRow::getNumParam() const {
	
  		return numParam;
 	}

 	/**
 	 * Set numParam with the specified int.
 	 * @param numParam The int value to which numParam is to be set.
 	 
 	
 		
 	 */
 	void CalReductionRow::setNumParam (int numParam)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->numParam = numParam;
	
 	}
	
	

	

	
 	/**
 	 * Get paramSet.
 	 * @return paramSet as std::vector<std::string >
 	 */
 	std::vector<std::string > CalReductionRow::getParamSet() const {
	
  		return paramSet;
 	}

 	/**
 	 * Set paramSet with the specified std::vector<std::string >.
 	 * @param paramSet The std::vector<std::string > value to which paramSet is to be set.
 	 
 	
 		
 	 */
 	void CalReductionRow::setParamSet (std::vector<std::string > paramSet)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->paramSet = paramSet;
	
 	}
	
	

	

	
 	/**
 	 * Get numInvalidConditions.
 	 * @return numInvalidConditions as int
 	 */
 	int CalReductionRow::getNumInvalidConditions() const {
	
  		return numInvalidConditions;
 	}

 	/**
 	 * Set numInvalidConditions with the specified int.
 	 * @param numInvalidConditions The int value to which numInvalidConditions is to be set.
 	 
 	
 		
 	 */
 	void CalReductionRow::setNumInvalidConditions (int numInvalidConditions)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->numInvalidConditions = numInvalidConditions;
	
 	}
	
	

	

	
 	/**
 	 * Get invalidConditions.
 	 * @return invalidConditions as std::vector<InvalidatingConditionMod::InvalidatingCondition >
 	 */
 	std::vector<InvalidatingConditionMod::InvalidatingCondition > CalReductionRow::getInvalidConditions() const {
	
  		return invalidConditions;
 	}

 	/**
 	 * Set invalidConditions with the specified std::vector<InvalidatingConditionMod::InvalidatingCondition >.
 	 * @param invalidConditions The std::vector<InvalidatingConditionMod::InvalidatingCondition > value to which invalidConditions is to be set.
 	 
 	
 		
 	 */
 	void CalReductionRow::setInvalidConditions (std::vector<InvalidatingConditionMod::InvalidatingCondition > invalidConditions)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->invalidConditions = invalidConditions;
	
 	}
	
	

	

	
 	/**
 	 * Get timeReduced.
 	 * @return timeReduced as ArrayTime
 	 */
 	ArrayTime CalReductionRow::getTimeReduced() const {
	
  		return timeReduced;
 	}

 	/**
 	 * Set timeReduced with the specified ArrayTime.
 	 * @param timeReduced The ArrayTime value to which timeReduced is to be set.
 	 
 	
 		
 	 */
 	void CalReductionRow::setTimeReduced (ArrayTime timeReduced)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->timeReduced = timeReduced;
	
 	}
	
	

	

	
 	/**
 	 * Get messages.
 	 * @return messages as std::string
 	 */
 	std::string CalReductionRow::getMessages() const {
	
  		return messages;
 	}

 	/**
 	 * Set messages with the specified std::string.
 	 * @param messages The std::string value to which messages is to be set.
 	 
 	
 		
 	 */
 	void CalReductionRow::setMessages (std::string messages)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->messages = messages;
	
 	}
	
	

	

	
 	/**
 	 * Get software.
 	 * @return software as std::string
 	 */
 	std::string CalReductionRow::getSoftware() const {
	
  		return software;
 	}

 	/**
 	 * Set software with the specified std::string.
 	 * @param software The std::string value to which software is to be set.
 	 
 	
 		
 	 */
 	void CalReductionRow::setSoftware (std::string software)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->software = software;
	
 	}
	
	

	

	
 	/**
 	 * Get softwareVersion.
 	 * @return softwareVersion as std::string
 	 */
 	std::string CalReductionRow::getSoftwareVersion() const {
	
  		return softwareVersion;
 	}

 	/**
 	 * Set softwareVersion with the specified std::string.
 	 * @param softwareVersion The std::string value to which softwareVersion is to be set.
 	 
 	
 		
 	 */
 	void CalReductionRow::setSoftwareVersion (std::string softwareVersion)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->softwareVersion = softwareVersion;
	
 	}
	
	

	
	///////////////////////////////////////////////
	// Extrinsic Table Attributes getters/setters//
	///////////////////////////////////////////////
	

	//////////////////////////////////////
	// Links Attributes getters/setters //
	//////////////////////////////////////
	
	
	/**
	 * Create a CalReductionRow.
	 * <p>
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 */ 
	CalReductionRow::CalReductionRow (CalReductionTable &t) : table(t) {
		hasBeenAdded = false;
		
	
	

	

	

	

	

	

	

	

	

	

	

	
	
	
	
	

	

	

	

	

	

	

	

	

	

	

	
	
	 fromBinMethods["calReductionId"] = &CalReductionRow::calReductionIdFromBin; 
	 fromBinMethods["numApplied"] = &CalReductionRow::numAppliedFromBin; 
	 fromBinMethods["appliedCalibrations"] = &CalReductionRow::appliedCalibrationsFromBin; 
	 fromBinMethods["numParam"] = &CalReductionRow::numParamFromBin; 
	 fromBinMethods["paramSet"] = &CalReductionRow::paramSetFromBin; 
	 fromBinMethods["numInvalidConditions"] = &CalReductionRow::numInvalidConditionsFromBin; 
	 fromBinMethods["invalidConditions"] = &CalReductionRow::invalidConditionsFromBin; 
	 fromBinMethods["timeReduced"] = &CalReductionRow::timeReducedFromBin; 
	 fromBinMethods["messages"] = &CalReductionRow::messagesFromBin; 
	 fromBinMethods["software"] = &CalReductionRow::softwareFromBin; 
	 fromBinMethods["softwareVersion"] = &CalReductionRow::softwareVersionFromBin; 
		
	
	
	
	
	
				 
	fromTextMethods["calReductionId"] = &CalReductionRow::calReductionIdFromText;
		 
	
				 
	fromTextMethods["numApplied"] = &CalReductionRow::numAppliedFromText;
		 
	
				 
	fromTextMethods["appliedCalibrations"] = &CalReductionRow::appliedCalibrationsFromText;
		 
	
				 
	fromTextMethods["numParam"] = &CalReductionRow::numParamFromText;
		 
	
				 
	fromTextMethods["paramSet"] = &CalReductionRow::paramSetFromText;
		 
	
				 
	fromTextMethods["numInvalidConditions"] = &CalReductionRow::numInvalidConditionsFromText;
		 
	
				 
	fromTextMethods["invalidConditions"] = &CalReductionRow::invalidConditionsFromText;
		 
	
				 
	fromTextMethods["timeReduced"] = &CalReductionRow::timeReducedFromText;
		 
	
				 
	fromTextMethods["messages"] = &CalReductionRow::messagesFromText;
		 
	
				 
	fromTextMethods["software"] = &CalReductionRow::softwareFromText;
		 
	
				 
	fromTextMethods["softwareVersion"] = &CalReductionRow::softwareVersionFromText;
		 
	

		
	}
	
	CalReductionRow::CalReductionRow (CalReductionTable &t, CalReductionRow *row) : table(t) {
		hasBeenAdded = false;
		
		if (row == 0) {
	
	
	

	

	

	

	

	

	

	

	

	

	

			
		}
		else {
	
		
			calReductionId = row->calReductionId;
		
		
		
		
			numApplied = row->numApplied;
		
			appliedCalibrations = row->appliedCalibrations;
		
			numParam = row->numParam;
		
			paramSet = row->paramSet;
		
			numInvalidConditions = row->numInvalidConditions;
		
			invalidConditions = row->invalidConditions;
		
			timeReduced = row->timeReduced;
		
			messages = row->messages;
		
			software = row->software;
		
			softwareVersion = row->softwareVersion;
		
		
		
		
		}
		
		 fromBinMethods["calReductionId"] = &CalReductionRow::calReductionIdFromBin; 
		 fromBinMethods["numApplied"] = &CalReductionRow::numAppliedFromBin; 
		 fromBinMethods["appliedCalibrations"] = &CalReductionRow::appliedCalibrationsFromBin; 
		 fromBinMethods["numParam"] = &CalReductionRow::numParamFromBin; 
		 fromBinMethods["paramSet"] = &CalReductionRow::paramSetFromBin; 
		 fromBinMethods["numInvalidConditions"] = &CalReductionRow::numInvalidConditionsFromBin; 
		 fromBinMethods["invalidConditions"] = &CalReductionRow::invalidConditionsFromBin; 
		 fromBinMethods["timeReduced"] = &CalReductionRow::timeReducedFromBin; 
		 fromBinMethods["messages"] = &CalReductionRow::messagesFromBin; 
		 fromBinMethods["software"] = &CalReductionRow::softwareFromBin; 
		 fromBinMethods["softwareVersion"] = &CalReductionRow::softwareVersionFromBin; 
			
	
			
	}

	
	bool CalReductionRow::compareNoAutoInc(int numApplied, std::vector<std::string > appliedCalibrations, int numParam, std::vector<std::string > paramSet, int numInvalidConditions, std::vector<InvalidatingConditionMod::InvalidatingCondition > invalidConditions, ArrayTime timeReduced, std::string messages, std::string software, std::string softwareVersion) {
		bool result;
		result = true;
		
	
		
		result = result && (this->numApplied == numApplied);
		
		if (!result) return false;
	

	
		
		result = result && (this->appliedCalibrations == appliedCalibrations);
		
		if (!result) return false;
	

	
		
		result = result && (this->numParam == numParam);
		
		if (!result) return false;
	

	
		
		result = result && (this->paramSet == paramSet);
		
		if (!result) return false;
	

	
		
		result = result && (this->numInvalidConditions == numInvalidConditions);
		
		if (!result) return false;
	

	
		
		result = result && (this->invalidConditions == invalidConditions);
		
		if (!result) return false;
	

	
		
		result = result && (this->timeReduced == timeReduced);
		
		if (!result) return false;
	

	
		
		result = result && (this->messages == messages);
		
		if (!result) return false;
	

	
		
		result = result && (this->software == software);
		
		if (!result) return false;
	

	
		
		result = result && (this->softwareVersion == softwareVersion);
		
		if (!result) return false;
	

		return result;
	}	
	
	
	
	bool CalReductionRow::compareRequiredValue(int numApplied, std::vector<std::string > appliedCalibrations, int numParam, std::vector<std::string > paramSet, int numInvalidConditions, std::vector<InvalidatingConditionMod::InvalidatingCondition > invalidConditions, ArrayTime timeReduced, std::string messages, std::string software, std::string softwareVersion) {
		bool result;
		result = true;
		
	
		if (!(this->numApplied == numApplied)) return false;
	

	
		if (!(this->appliedCalibrations == appliedCalibrations)) return false;
	

	
		if (!(this->numParam == numParam)) return false;
	

	
		if (!(this->paramSet == paramSet)) return false;
	

	
		if (!(this->numInvalidConditions == numInvalidConditions)) return false;
	

	
		if (!(this->invalidConditions == invalidConditions)) return false;
	

	
		if (!(this->timeReduced == timeReduced)) return false;
	

	
		if (!(this->messages == messages)) return false;
	

	
		if (!(this->software == software)) return false;
	

	
		if (!(this->softwareVersion == softwareVersion)) return false;
	

		return result;
	}
	
	
	/**
	 * Return true if all required attributes of the value part are equal to their homologues
	 * in x and false otherwise.
	 *

	 * @param x a pointer on the CalReductionRow whose required attributes of the value part 

	 * will be compared with those of this.
	 * @return a boolean.
	 */
	bool CalReductionRow::equalByRequiredValue(CalReductionRow*  x ) {
		
			
		if (this->numApplied != x->numApplied) return false;
			
		if (this->appliedCalibrations != x->appliedCalibrations) return false;
			
		if (this->numParam != x->numParam) return false;
			
		if (this->paramSet != x->paramSet) return false;
			
		if (this->numInvalidConditions != x->numInvalidConditions) return false;
			
		if (this->invalidConditions != x->invalidConditions) return false;
			
		if (this->timeReduced != x->timeReduced) return false;
			
		if (this->messages != x->messages) return false;
			
		if (this->software != x->software) return false;
			
		if (this->softwareVersion != x->softwareVersion) return false;
			
		
		return true;
	}	
	
/*
	 map<string, CalReductionAttributeFromBin> CalReductionRow::initFromBinMethods() {
		map<string, CalReductionAttributeFromBin> result;
		
		result["calReductionId"] = &CalReductionRow::calReductionIdFromBin;
		result["numApplied"] = &CalReductionRow::numAppliedFromBin;
		result["appliedCalibrations"] = &CalReductionRow::appliedCalibrationsFromBin;
		result["numParam"] = &CalReductionRow::numParamFromBin;
		result["paramSet"] = &CalReductionRow::paramSetFromBin;
		result["numInvalidConditions"] = &CalReductionRow::numInvalidConditionsFromBin;
		result["invalidConditions"] = &CalReductionRow::invalidConditionsFromBin;
		result["timeReduced"] = &CalReductionRow::timeReducedFromBin;
		result["messages"] = &CalReductionRow::messagesFromBin;
		result["software"] = &CalReductionRow::softwareFromBin;
		result["softwareVersion"] = &CalReductionRow::softwareVersionFromBin;
		
		
			
		
		return result;	
	}
*/	
} // End namespace asdm
 
