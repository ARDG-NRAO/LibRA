
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
 * File HistoryRow.cpp
 */
 
#include <vector>
#include <set>

#include <alma/ASDM/ASDM.h>
#include <alma/ASDM/HistoryRow.h>
#include <alma/ASDM/HistoryTable.h>

#include <alma/ASDM/ExecBlockTable.h>
#include <alma/ASDM/ExecBlockRow.h>
	

using asdm::ASDM;
using asdm::HistoryRow;
using asdm::HistoryTable;

using asdm::ExecBlockTable;
using asdm::ExecBlockRow;


#include <alma/ASDM/Parser.h>

#include <alma/ASDM/EnumerationParser.h>
#include <alma/ASDM/ASDMValuesParser.h>
 
#include <alma/ASDM/InvalidArgumentException.h>

using namespace std;

namespace asdm {
	HistoryRow::~HistoryRow() {
	}

	/**
	 * Return the table to which this row belongs.
	 */
	HistoryTable &HistoryRow::getTable() const {
		return table;
	}

	bool HistoryRow::isAdded() const {
		return hasBeenAdded;
	}	

	void HistoryRow::isAdded(bool added) {
		hasBeenAdded = added;
	}
	
#ifndef WITHOUT_ACS
	using asdmIDL::HistoryRowIDL;
#endif
	
#ifndef WITHOUT_ACS
	/**
	 * Return this row in the form of an IDL struct.
	 * @return The values of this row as a HistoryRowIDL struct.
	 */
	HistoryRowIDL *HistoryRow::toIDL() const {
		HistoryRowIDL *x = new HistoryRowIDL ();
		
		// Fill the IDL structure.
	
		
	
  		
		
		
			
		x->time = time.toIDLArrayTime();
			
		
	

	
  		
		
		
			
				
		x->message = CORBA::string_dup(message.c_str());
				
 			
		
	

	
  		
		
		
			
				
		x->priority = CORBA::string_dup(priority.c_str());
				
 			
		
	

	
  		
		
		
			
				
		x->origin = CORBA::string_dup(origin.c_str());
				
 			
		
	

	
  		
		
		
			
				
		x->objectId = CORBA::string_dup(objectId.c_str());
				
 			
		
	

	
  		
		
		
			
				
		x->application = CORBA::string_dup(application.c_str());
				
 			
		
	

	
  		
		
		
			
				
		x->cliCommand = CORBA::string_dup(cliCommand.c_str());
				
 			
		
	

	
  		
		
		
			
				
		x->appParms = CORBA::string_dup(appParms.c_str());
				
 			
		
	

	
	
		
	
  	
 		
		
	 	
			
		x->execBlockId = execBlockId.toIDLTag();
			
	 	 		
  	

	
		
	

		
		return x;
	
	}
	
	void HistoryRow::toIDL(asdmIDL::HistoryRowIDL& x) const {
		// Set the x's fields.
	
		
	
  		
		
		
			
		x.time = time.toIDLArrayTime();
			
		
	

	
  		
		
		
			
				
		x.message = CORBA::string_dup(message.c_str());
				
 			
		
	

	
  		
		
		
			
				
		x.priority = CORBA::string_dup(priority.c_str());
				
 			
		
	

	
  		
		
		
			
				
		x.origin = CORBA::string_dup(origin.c_str());
				
 			
		
	

	
  		
		
		
			
				
		x.objectId = CORBA::string_dup(objectId.c_str());
				
 			
		
	

	
  		
		
		
			
				
		x.application = CORBA::string_dup(application.c_str());
				
 			
		
	

	
  		
		
		
			
				
		x.cliCommand = CORBA::string_dup(cliCommand.c_str());
				
 			
		
	

	
  		
		
		
			
				
		x.appParms = CORBA::string_dup(appParms.c_str());
				
 			
		
	

	
	
		
	
  	
 		
		
	 	
			
		x.execBlockId = execBlockId.toIDLTag();
			
	 	 		
  	

	
		
	

	
	}
#endif
	

#ifndef WITHOUT_ACS
	/**
	 * Fill the values of this row from the IDL struct HistoryRowIDL.
	 * @param x The IDL struct containing the values used to fill this row.
	 */
	void HistoryRow::setFromIDL (HistoryRowIDL x){
		try {
		// Fill the values from x.
	
		
	
		
		
			
		setTime(ArrayTime (x.time));
			
 		
		
	

	
		
		
			
		setMessage(string (x.message));
			
 		
		
	

	
		
		
			
		setPriority(string (x.priority));
			
 		
		
	

	
		
		
			
		setOrigin(string (x.origin));
			
 		
		
	

	
		
		
			
		setObjectId(string (x.objectId));
			
 		
		
	

	
		
		
			
		setApplication(string (x.application));
			
 		
		
	

	
		
		
			
		setCliCommand(string (x.cliCommand));
			
 		
		
	

	
		
		
			
		setAppParms(string (x.appParms));
			
 		
		
	

	
	
		
	
		
		
			
		setExecBlockId(Tag (x.execBlockId));
			
 		
		
	

	
		
	

		} catch (const IllegalAccessException &err) {
			throw ConversionException (err.getMessage(),"History");
		}
	}
#endif
	
	/**
	 * Return this row in the form of an XML string.
	 * @return The values of this row as an XML string.
	 */
	string HistoryRow::toXML() const {
		string buf;
		buf.append("<row> \n");
		
	
		
  	
 		
		
		Parser::toXML(time, "time", buf);
		
		
	

  	
 		
		
		Parser::toXML(message, "message", buf);
		
		
	

  	
 		
		
		Parser::toXML(priority, "priority", buf);
		
		
	

  	
 		
		
		Parser::toXML(origin, "origin", buf);
		
		
	

  	
 		
		
		Parser::toXML(objectId, "objectId", buf);
		
		
	

  	
 		
		
		Parser::toXML(application, "application", buf);
		
		
	

  	
 		
		
		Parser::toXML(cliCommand, "cliCommand", buf);
		
		
	

  	
 		
		
		Parser::toXML(appParms, "appParms", buf);
		
		
	

	
	
		
  	
 		
		
		Parser::toXML(execBlockId, "execBlockId", buf);
		
		
	

	
		
	

		
		buf.append("</row>\n");
		return buf;
	}

	/**
	 * Fill the values of this row from an XML string 
	 * that was produced by the toXML() method.
	 * @param x The XML string being used to set the values of this row.
	 */
	void HistoryRow::setFromXML (string rowDoc) {
		Parser row(rowDoc);
		string s = "";
		try {
	
		
	
  		
			
	  	setTime(Parser::getArrayTime("time","History",rowDoc));
			
		
	

	
  		
			
	  	setMessage(Parser::getString("message","History",rowDoc));
			
		
	

	
  		
			
	  	setPriority(Parser::getString("priority","History",rowDoc));
			
		
	

	
  		
			
	  	setOrigin(Parser::getString("origin","History",rowDoc));
			
		
	

	
  		
			
	  	setObjectId(Parser::getString("objectId","History",rowDoc));
			
		
	

	
  		
			
	  	setApplication(Parser::getString("application","History",rowDoc));
			
		
	

	
  		
			
	  	setCliCommand(Parser::getString("cliCommand","History",rowDoc));
			
		
	

	
  		
			
	  	setAppParms(Parser::getString("appParms","History",rowDoc));
			
		
	

	
	
		
	
  		
			
	  	setExecBlockId(Parser::getTag("execBlockId","ExecBlock",rowDoc));
			
		
	

	
		
	

		} catch (const IllegalAccessException &err) {
			throw ConversionException (err.getMessage(),"History");
		}
	}
	
	void HistoryRow::toBin(EndianOSStream& eoss) {
	
	
	
	
		
	execBlockId.toBin(eoss);
		
	

	
	
		
	time.toBin(eoss);
		
	

	
	
		
						
			eoss.writeString(message);
				
		
	

	
	
		
						
			eoss.writeString(priority);
				
		
	

	
	
		
						
			eoss.writeString(origin);
				
		
	

	
	
		
						
			eoss.writeString(objectId);
				
		
	

	
	
		
						
			eoss.writeString(application);
				
		
	

	
	
		
						
			eoss.writeString(cliCommand);
				
		
	

	
	
		
						
			eoss.writeString(appParms);
				
		
	


	
	
	}
	
void HistoryRow::execBlockIdFromBin(EndianIStream& eis) {
		
	
		
		
		execBlockId =  Tag::fromBin(eis);
		
	
	
}
void HistoryRow::timeFromBin(EndianIStream& eis) {
		
	
		
		
		time =  ArrayTime::fromBin(eis);
		
	
	
}
void HistoryRow::messageFromBin(EndianIStream& eis) {
		
	
	
		
			
		message =  eis.readString();
			
		
	
	
}
void HistoryRow::priorityFromBin(EndianIStream& eis) {
		
	
	
		
			
		priority =  eis.readString();
			
		
	
	
}
void HistoryRow::originFromBin(EndianIStream& eis) {
		
	
	
		
			
		origin =  eis.readString();
			
		
	
	
}
void HistoryRow::objectIdFromBin(EndianIStream& eis) {
		
	
	
		
			
		objectId =  eis.readString();
			
		
	
	
}
void HistoryRow::applicationFromBin(EndianIStream& eis) {
		
	
	
		
			
		application =  eis.readString();
			
		
	
	
}
void HistoryRow::cliCommandFromBin(EndianIStream& eis) {
		
	
	
		
			
		cliCommand =  eis.readString();
			
		
	
	
}
void HistoryRow::appParmsFromBin(EndianIStream& eis) {
		
	
	
		
			
		appParms =  eis.readString();
			
		
	
	
}

		
	
	HistoryRow* HistoryRow::fromBin(EndianIStream& eis, HistoryTable& table, const vector<string>& attributesSeq) {
		HistoryRow* row = new  HistoryRow(table);
		
		map<string, HistoryAttributeFromBin>::iterator iter ;
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
					throw ConversionException("There is not method to read an attribute '"+attributesSeq.at(i)+"'.", "HistoryTable");
			}
				
		}				
		return row;
	}

	//
	// A collection of methods to set the value of the attributes from their textual value in the XML representation
	// of one row.
	//
	
	// Convert a string into an Tag 
	void HistoryRow::execBlockIdFromText(const string & s) {
		 
          
		execBlockId = ASDMValuesParser::parse<Tag>(s);
          
		
	}
	
	
	// Convert a string into an ArrayTime 
	void HistoryRow::timeFromText(const string & s) {
		 
          
		time = ASDMValuesParser::parse<ArrayTime>(s);
          
		
	}
	
	
	// Convert a string into an String 
	void HistoryRow::messageFromText(const string & s) {
		 
          
		message = ASDMValuesParser::parse<string>(s);
          
		
	}
	
	
	// Convert a string into an String 
	void HistoryRow::priorityFromText(const string & s) {
		 
          
		priority = ASDMValuesParser::parse<string>(s);
          
		
	}
	
	
	// Convert a string into an String 
	void HistoryRow::originFromText(const string & s) {
		 
          
		origin = ASDMValuesParser::parse<string>(s);
          
		
	}
	
	
	// Convert a string into an String 
	void HistoryRow::objectIdFromText(const string & s) {
		 
          
		objectId = ASDMValuesParser::parse<string>(s);
          
		
	}
	
	
	// Convert a string into an String 
	void HistoryRow::applicationFromText(const string & s) {
		 
          
		application = ASDMValuesParser::parse<string>(s);
          
		
	}
	
	
	// Convert a string into an String 
	void HistoryRow::cliCommandFromText(const string & s) {
		 
          
		cliCommand = ASDMValuesParser::parse<string>(s);
          
		
	}
	
	
	// Convert a string into an String 
	void HistoryRow::appParmsFromText(const string & s) {
		 
          
		appParms = ASDMValuesParser::parse<string>(s);
          
		
	}
	

		
	
	void HistoryRow::fromText(const std::string& attributeName, const std::string&  t) {
		map<string, HistoryAttributeFromText>::iterator iter;
		if ((iter = fromTextMethods.find(attributeName)) == fromTextMethods.end())
			throw ConversionException("I do not know what to do with '"+attributeName+"' and its content '"+t+"' (while parsing an XML document)", "HistoryTable");
		(this->*(iter->second))(t);
	}
			
	////////////////////////////////////////////////
	// Intrinsic Table Attributes getters/setters //
	////////////////////////////////////////////////
	
	

	
 	/**
 	 * Get time.
 	 * @return time as ArrayTime
 	 */
 	ArrayTime HistoryRow::getTime() const {
	
  		return time;
 	}

 	/**
 	 * Set time with the specified ArrayTime.
 	 * @param time The ArrayTime value to which time is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void HistoryRow::setTime (ArrayTime time)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("time", "History");
		
  		}
  	
 		this->time = time;
	
 	}
	
	

	

	
 	/**
 	 * Get message.
 	 * @return message as std::string
 	 */
 	std::string HistoryRow::getMessage() const {
	
  		return message;
 	}

 	/**
 	 * Set message with the specified std::string.
 	 * @param message The std::string value to which message is to be set.
 	 
 	
 		
 	 */
 	void HistoryRow::setMessage (std::string message)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->message = message;
	
 	}
	
	

	

	
 	/**
 	 * Get priority.
 	 * @return priority as std::string
 	 */
 	std::string HistoryRow::getPriority() const {
	
  		return priority;
 	}

 	/**
 	 * Set priority with the specified std::string.
 	 * @param priority The std::string value to which priority is to be set.
 	 
 	
 		
 	 */
 	void HistoryRow::setPriority (std::string priority)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->priority = priority;
	
 	}
	
	

	

	
 	/**
 	 * Get origin.
 	 * @return origin as std::string
 	 */
 	std::string HistoryRow::getOrigin() const {
	
  		return origin;
 	}

 	/**
 	 * Set origin with the specified std::string.
 	 * @param origin The std::string value to which origin is to be set.
 	 
 	
 		
 	 */
 	void HistoryRow::setOrigin (std::string origin)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->origin = origin;
	
 	}
	
	

	

	
 	/**
 	 * Get objectId.
 	 * @return objectId as std::string
 	 */
 	std::string HistoryRow::getObjectId() const {
	
  		return objectId;
 	}

 	/**
 	 * Set objectId with the specified std::string.
 	 * @param objectId The std::string value to which objectId is to be set.
 	 
 	
 		
 	 */
 	void HistoryRow::setObjectId (std::string objectId)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->objectId = objectId;
	
 	}
	
	

	

	
 	/**
 	 * Get application.
 	 * @return application as std::string
 	 */
 	std::string HistoryRow::getApplication() const {
	
  		return application;
 	}

 	/**
 	 * Set application with the specified std::string.
 	 * @param application The std::string value to which application is to be set.
 	 
 	
 		
 	 */
 	void HistoryRow::setApplication (std::string application)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->application = application;
	
 	}
	
	

	

	
 	/**
 	 * Get cliCommand.
 	 * @return cliCommand as std::string
 	 */
 	std::string HistoryRow::getCliCommand() const {
	
  		return cliCommand;
 	}

 	/**
 	 * Set cliCommand with the specified std::string.
 	 * @param cliCommand The std::string value to which cliCommand is to be set.
 	 
 	
 		
 	 */
 	void HistoryRow::setCliCommand (std::string cliCommand)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->cliCommand = cliCommand;
	
 	}
	
	

	

	
 	/**
 	 * Get appParms.
 	 * @return appParms as std::string
 	 */
 	std::string HistoryRow::getAppParms() const {
	
  		return appParms;
 	}

 	/**
 	 * Set appParms with the specified std::string.
 	 * @param appParms The std::string value to which appParms is to be set.
 	 
 	
 		
 	 */
 	void HistoryRow::setAppParms (std::string appParms)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->appParms = appParms;
	
 	}
	
	

	
	///////////////////////////////////////////////
	// Extrinsic Table Attributes getters/setters//
	///////////////////////////////////////////////
	
	

	
 	/**
 	 * Get execBlockId.
 	 * @return execBlockId as Tag
 	 */
 	Tag HistoryRow::getExecBlockId() const {
	
  		return execBlockId;
 	}

 	/**
 	 * Set execBlockId with the specified Tag.
 	 * @param execBlockId The Tag value to which execBlockId is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void HistoryRow::setExecBlockId (Tag execBlockId)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("execBlockId", "History");
		
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
	 ExecBlockRow* HistoryRow::getExecBlockUsingExecBlockId() {
	 
	 	return table.getContainer().getExecBlock().getRowByKey(execBlockId);
	 }
	 

	

	
	/**
	 * Create a HistoryRow.
	 * <p>
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 */ 
	HistoryRow::HistoryRow (HistoryTable &t) : table(t) {
		hasBeenAdded = false;
		
	
	

	

	

	

	

	

	

	

	
	

	
	
	
	

	

	

	

	

	

	

	

	
	
	 fromBinMethods["execBlockId"] = &HistoryRow::execBlockIdFromBin; 
	 fromBinMethods["time"] = &HistoryRow::timeFromBin; 
	 fromBinMethods["message"] = &HistoryRow::messageFromBin; 
	 fromBinMethods["priority"] = &HistoryRow::priorityFromBin; 
	 fromBinMethods["origin"] = &HistoryRow::originFromBin; 
	 fromBinMethods["objectId"] = &HistoryRow::objectIdFromBin; 
	 fromBinMethods["application"] = &HistoryRow::applicationFromBin; 
	 fromBinMethods["cliCommand"] = &HistoryRow::cliCommandFromBin; 
	 fromBinMethods["appParms"] = &HistoryRow::appParmsFromBin; 
		
	
	
	
	
	
				 
	fromTextMethods["execBlockId"] = &HistoryRow::execBlockIdFromText;
		 
	
				 
	fromTextMethods["time"] = &HistoryRow::timeFromText;
		 
	
				 
	fromTextMethods["message"] = &HistoryRow::messageFromText;
		 
	
				 
	fromTextMethods["priority"] = &HistoryRow::priorityFromText;
		 
	
				 
	fromTextMethods["origin"] = &HistoryRow::originFromText;
		 
	
				 
	fromTextMethods["objectId"] = &HistoryRow::objectIdFromText;
		 
	
				 
	fromTextMethods["application"] = &HistoryRow::applicationFromText;
		 
	
				 
	fromTextMethods["cliCommand"] = &HistoryRow::cliCommandFromText;
		 
	
				 
	fromTextMethods["appParms"] = &HistoryRow::appParmsFromText;
		 
	

		
	}
	
	HistoryRow::HistoryRow (HistoryTable &t, HistoryRow *row) : table(t) {
		hasBeenAdded = false;
		
		if (row == 0) {
	
	
	

	

	

	

	

	

	

	

	
	
		
		}
		else {
	
		
			execBlockId = row->execBlockId;
		
			time = row->time;
		
		
		
		
			message = row->message;
		
			priority = row->priority;
		
			origin = row->origin;
		
			objectId = row->objectId;
		
			application = row->application;
		
			cliCommand = row->cliCommand;
		
			appParms = row->appParms;
		
		
		
		
		}
		
		 fromBinMethods["execBlockId"] = &HistoryRow::execBlockIdFromBin; 
		 fromBinMethods["time"] = &HistoryRow::timeFromBin; 
		 fromBinMethods["message"] = &HistoryRow::messageFromBin; 
		 fromBinMethods["priority"] = &HistoryRow::priorityFromBin; 
		 fromBinMethods["origin"] = &HistoryRow::originFromBin; 
		 fromBinMethods["objectId"] = &HistoryRow::objectIdFromBin; 
		 fromBinMethods["application"] = &HistoryRow::applicationFromBin; 
		 fromBinMethods["cliCommand"] = &HistoryRow::cliCommandFromBin; 
		 fromBinMethods["appParms"] = &HistoryRow::appParmsFromBin; 
			
	
			
	}

	
	bool HistoryRow::compareNoAutoInc(Tag execBlockId, ArrayTime time, std::string message, std::string priority, std::string origin, std::string objectId, std::string application, std::string cliCommand, std::string appParms) {
		bool result;
		result = true;
		
	
		
		result = result && (this->execBlockId == execBlockId);
		
		if (!result) return false;
	

	
		
		result = result && (this->time == time);
		
		if (!result) return false;
	

	
		
		result = result && (this->message == message);
		
		if (!result) return false;
	

	
		
		result = result && (this->priority == priority);
		
		if (!result) return false;
	

	
		
		result = result && (this->origin == origin);
		
		if (!result) return false;
	

	
		
		result = result && (this->objectId == objectId);
		
		if (!result) return false;
	

	
		
		result = result && (this->application == application);
		
		if (!result) return false;
	

	
		
		result = result && (this->cliCommand == cliCommand);
		
		if (!result) return false;
	

	
		
		result = result && (this->appParms == appParms);
		
		if (!result) return false;
	

		return result;
	}	
	
	
	
	bool HistoryRow::compareRequiredValue(std::string message, std::string priority, std::string origin, std::string objectId, std::string application, std::string cliCommand, std::string appParms) {
		bool result;
		result = true;
		
	
		if (!(this->message == message)) return false;
	

	
		if (!(this->priority == priority)) return false;
	

	
		if (!(this->origin == origin)) return false;
	

	
		if (!(this->objectId == objectId)) return false;
	

	
		if (!(this->application == application)) return false;
	

	
		if (!(this->cliCommand == cliCommand)) return false;
	

	
		if (!(this->appParms == appParms)) return false;
	

		return result;
	}
	
	
	/**
	 * Return true if all required attributes of the value part are equal to their homologues
	 * in x and false otherwise.
	 *

	 * @param x a pointer on the HistoryRow whose required attributes of the value part 

	 * will be compared with those of this.
	 * @return a boolean.
	 */
	bool HistoryRow::equalByRequiredValue(HistoryRow*  x ) {
		
			
		if (this->message != x->message) return false;
			
		if (this->priority != x->priority) return false;
			
		if (this->origin != x->origin) return false;
			
		if (this->objectId != x->objectId) return false;
			
		if (this->application != x->application) return false;
			
		if (this->cliCommand != x->cliCommand) return false;
			
		if (this->appParms != x->appParms) return false;
			
		
		return true;
	}	
	
/*
	 map<string, HistoryAttributeFromBin> HistoryRow::initFromBinMethods() {
		map<string, HistoryAttributeFromBin> result;
		
		result["execBlockId"] = &HistoryRow::execBlockIdFromBin;
		result["time"] = &HistoryRow::timeFromBin;
		result["message"] = &HistoryRow::messageFromBin;
		result["priority"] = &HistoryRow::priorityFromBin;
		result["origin"] = &HistoryRow::originFromBin;
		result["objectId"] = &HistoryRow::objectIdFromBin;
		result["application"] = &HistoryRow::applicationFromBin;
		result["cliCommand"] = &HistoryRow::cliCommandFromBin;
		result["appParms"] = &HistoryRow::appParmsFromBin;
		
		
			
		
		return result;	
	}
*/	
} // End namespace asdm
 
