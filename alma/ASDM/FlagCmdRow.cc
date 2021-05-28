
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
 * File FlagCmdRow.cpp
 */
 
#include <vector>
#include <set>

#include <alma/ASDM/ASDM.h>
#include <alma/ASDM/FlagCmdRow.h>
#include <alma/ASDM/FlagCmdTable.h>
	

using asdm::ASDM;
using asdm::FlagCmdRow;
using asdm::FlagCmdTable;


#include <alma/ASDM/Parser.h>

#include <alma/ASDM/EnumerationParser.h>
#include <alma/ASDM/ASDMValuesParser.h>
 
#include <alma/ASDM/InvalidArgumentException.h>

using namespace std;

namespace asdm {
	FlagCmdRow::~FlagCmdRow() {
	}

	/**
	 * Return the table to which this row belongs.
	 */
	FlagCmdTable &FlagCmdRow::getTable() const {
		return table;
	}

	bool FlagCmdRow::isAdded() const {
		return hasBeenAdded;
	}	

	void FlagCmdRow::isAdded(bool added) {
		hasBeenAdded = added;
	}
	
#ifndef WITHOUT_ACS
	using asdmIDL::FlagCmdRowIDL;
#endif
	
#ifndef WITHOUT_ACS
	/**
	 * Return this row in the form of an IDL struct.
	 * @return The values of this row as a FlagCmdRowIDL struct.
	 */
	FlagCmdRowIDL *FlagCmdRow::toIDL() const {
		FlagCmdRowIDL *x = new FlagCmdRowIDL ();
		
		// Fill the IDL structure.
	
		
	
  		
		
		
			
		x->timeInterval = timeInterval.toIDLArrayTimeInterval();
			
		
	

	
  		
		
		
			
				
		x->type = CORBA::string_dup(type.c_str());
				
 			
		
	

	
  		
		
		
			
				
		x->reason = CORBA::string_dup(reason.c_str());
				
 			
		
	

	
  		
		
		
			
				
		x->level = level;
 				
 			
		
	

	
  		
		
		
			
				
		x->severity = severity;
 				
 			
		
	

	
  		
		
		
			
				
		x->applied = applied;
 				
 			
		
	

	
  		
		
		
			
				
		x->command = CORBA::string_dup(command.c_str());
				
 			
		
	

	
	
		
		
		return x;
	
	}
	
	void FlagCmdRow::toIDL(asdmIDL::FlagCmdRowIDL& x) const {
		// Set the x's fields.
	
		
	
  		
		
		
			
		x.timeInterval = timeInterval.toIDLArrayTimeInterval();
			
		
	

	
  		
		
		
			
				
		x.type = CORBA::string_dup(type.c_str());
				
 			
		
	

	
  		
		
		
			
				
		x.reason = CORBA::string_dup(reason.c_str());
				
 			
		
	

	
  		
		
		
			
				
		x.level = level;
 				
 			
		
	

	
  		
		
		
			
				
		x.severity = severity;
 				
 			
		
	

	
  		
		
		
			
				
		x.applied = applied;
 				
 			
		
	

	
  		
		
		
			
				
		x.command = CORBA::string_dup(command.c_str());
				
 			
		
	

	
	
		
	
	}
#endif
	

#ifndef WITHOUT_ACS
	/**
	 * Fill the values of this row from the IDL struct FlagCmdRowIDL.
	 * @param x The IDL struct containing the values used to fill this row.
	 */
	void FlagCmdRow::setFromIDL (FlagCmdRowIDL x){
		try {
		// Fill the values from x.
	
		
	
		
		
			
		setTimeInterval(ArrayTimeInterval (x.timeInterval));
			
 		
		
	

	
		
		
			
		setType(string (x.type));
			
 		
		
	

	
		
		
			
		setReason(string (x.reason));
			
 		
		
	

	
		
		
			
		setLevel(x.level);
  			
 		
		
	

	
		
		
			
		setSeverity(x.severity);
  			
 		
		
	

	
		
		
			
		setApplied(x.applied);
  			
 		
		
	

	
		
		
			
		setCommand(string (x.command));
			
 		
		
	

	
	
		
		} catch (const IllegalAccessException &err) {
			throw ConversionException (err.getMessage(),"FlagCmd");
		}
	}
#endif
	
	/**
	 * Return this row in the form of an XML string.
	 * @return The values of this row as an XML string.
	 */
	string FlagCmdRow::toXML() const {
		string buf;
		buf.append("<row> \n");
		
	
		
  	
 		
		
		Parser::toXML(timeInterval, "timeInterval", buf);
		
		
	

  	
 		
		
		Parser::toXML(type, "type", buf);
		
		
	

  	
 		
		
		Parser::toXML(reason, "reason", buf);
		
		
	

  	
 		
		
		Parser::toXML(level, "level", buf);
		
		
	

  	
 		
		
		Parser::toXML(severity, "severity", buf);
		
		
	

  	
 		
		
		Parser::toXML(applied, "applied", buf);
		
		
	

  	
 		
		
		Parser::toXML(command, "command", buf);
		
		
	

	
	
		
		
		buf.append("</row>\n");
		return buf;
	}

	/**
	 * Fill the values of this row from an XML string 
	 * that was produced by the toXML() method.
	 * @param x The XML string being used to set the values of this row.
	 */
	void FlagCmdRow::setFromXML (string rowDoc) {
		Parser row(rowDoc);
		string s = "";
		try {
	
		
	
  		
			
	  	setTimeInterval(Parser::getArrayTimeInterval("timeInterval","FlagCmd",rowDoc));
			
		
	

	
  		
			
	  	setType(Parser::getString("type","FlagCmd",rowDoc));
			
		
	

	
  		
			
	  	setReason(Parser::getString("reason","FlagCmd",rowDoc));
			
		
	

	
  		
			
	  	setLevel(Parser::getInteger("level","FlagCmd",rowDoc));
			
		
	

	
  		
			
	  	setSeverity(Parser::getInteger("severity","FlagCmd",rowDoc));
			
		
	

	
  		
			
	  	setApplied(Parser::getBoolean("applied","FlagCmd",rowDoc));
			
		
	

	
  		
			
	  	setCommand(Parser::getString("command","FlagCmd",rowDoc));
			
		
	

	
	
		
		} catch (const IllegalAccessException &err) {
			throw ConversionException (err.getMessage(),"FlagCmd");
		}
	}
	
	void FlagCmdRow::toBin(EndianOSStream& eoss) {
	
	
	
	
		
	timeInterval.toBin(eoss);
		
	

	
	
		
						
			eoss.writeString(type);
				
		
	

	
	
		
						
			eoss.writeString(reason);
				
		
	

	
	
		
						
			eoss.writeInt(level);
				
		
	

	
	
		
						
			eoss.writeInt(severity);
				
		
	

	
	
		
						
			eoss.writeBoolean(applied);
				
		
	

	
	
		
						
			eoss.writeString(command);
				
		
	


	
	
	}
	
void FlagCmdRow::timeIntervalFromBin(EndianIStream& eis) {
		
	
		
		
		timeInterval =  ArrayTimeInterval::fromBin(eis);
		
	
	
}
void FlagCmdRow::typeFromBin(EndianIStream& eis) {
		
	
	
		
			
		type =  eis.readString();
			
		
	
	
}
void FlagCmdRow::reasonFromBin(EndianIStream& eis) {
		
	
	
		
			
		reason =  eis.readString();
			
		
	
	
}
void FlagCmdRow::levelFromBin(EndianIStream& eis) {
		
	
	
		
			
		level =  eis.readInt();
			
		
	
	
}
void FlagCmdRow::severityFromBin(EndianIStream& eis) {
		
	
	
		
			
		severity =  eis.readInt();
			
		
	
	
}
void FlagCmdRow::appliedFromBin(EndianIStream& eis) {
		
	
	
		
			
		applied =  eis.readBoolean();
			
		
	
	
}
void FlagCmdRow::commandFromBin(EndianIStream& eis) {
		
	
	
		
			
		command =  eis.readString();
			
		
	
	
}

		
	
	FlagCmdRow* FlagCmdRow::fromBin(EndianIStream& eis, FlagCmdTable& table, const vector<string>& attributesSeq) {
		FlagCmdRow* row = new  FlagCmdRow(table);
		
		map<string, FlagCmdAttributeFromBin>::iterator iter ;
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
					throw ConversionException("There is not method to read an attribute '"+attributesSeq.at(i)+"'.", "FlagCmdTable");
			}
				
		}				
		return row;
	}

	//
	// A collection of methods to set the value of the attributes from their textual value in the XML representation
	// of one row.
	//
	
	// Convert a string into an ArrayTimeInterval 
	void FlagCmdRow::timeIntervalFromText(const string & s) {
		 
          
		timeInterval = ASDMValuesParser::parse<ArrayTimeInterval>(s);
          
		
	}
	
	
	// Convert a string into an String 
	void FlagCmdRow::typeFromText(const string & s) {
		 
          
		type = ASDMValuesParser::parse<string>(s);
          
		
	}
	
	
	// Convert a string into an String 
	void FlagCmdRow::reasonFromText(const string & s) {
		 
          
		reason = ASDMValuesParser::parse<string>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void FlagCmdRow::levelFromText(const string & s) {
		 
          
		level = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void FlagCmdRow::severityFromText(const string & s) {
		 
          
		severity = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	// Convert a string into an boolean 
	void FlagCmdRow::appliedFromText(const string & s) {
		 
          
		applied = ASDMValuesParser::parse<bool>(s);
          
		
	}
	
	
	// Convert a string into an String 
	void FlagCmdRow::commandFromText(const string & s) {
		 
          
		command = ASDMValuesParser::parse<string>(s);
          
		
	}
	

		
	
	void FlagCmdRow::fromText(const std::string& attributeName, const std::string&  t) {
		map<string, FlagCmdAttributeFromText>::iterator iter;
		if ((iter = fromTextMethods.find(attributeName)) == fromTextMethods.end())
			throw ConversionException("I do not know what to do with '"+attributeName+"' and its content '"+t+"' (while parsing an XML document)", "FlagCmdTable");
		(this->*(iter->second))(t);
	}
			
	////////////////////////////////////////////////
	// Intrinsic Table Attributes getters/setters //
	////////////////////////////////////////////////
	
	

	
 	/**
 	 * Get timeInterval.
 	 * @return timeInterval as ArrayTimeInterval
 	 */
 	ArrayTimeInterval FlagCmdRow::getTimeInterval() const {
	
  		return timeInterval;
 	}

 	/**
 	 * Set timeInterval with the specified ArrayTimeInterval.
 	 * @param timeInterval The ArrayTimeInterval value to which timeInterval is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void FlagCmdRow::setTimeInterval (ArrayTimeInterval timeInterval)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("timeInterval", "FlagCmd");
		
  		}
  	
 		this->timeInterval = timeInterval;
	
 	}
	
	

	

	
 	/**
 	 * Get type.
 	 * @return type as std::string
 	 */
 	std::string FlagCmdRow::getType() const {
	
  		return type;
 	}

 	/**
 	 * Set type with the specified std::string.
 	 * @param type The std::string value to which type is to be set.
 	 
 	
 		
 	 */
 	void FlagCmdRow::setType (std::string type)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->type = type;
	
 	}
	
	

	

	
 	/**
 	 * Get reason.
 	 * @return reason as std::string
 	 */
 	std::string FlagCmdRow::getReason() const {
	
  		return reason;
 	}

 	/**
 	 * Set reason with the specified std::string.
 	 * @param reason The std::string value to which reason is to be set.
 	 
 	
 		
 	 */
 	void FlagCmdRow::setReason (std::string reason)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->reason = reason;
	
 	}
	
	

	

	
 	/**
 	 * Get level.
 	 * @return level as int
 	 */
 	int FlagCmdRow::getLevel() const {
	
  		return level;
 	}

 	/**
 	 * Set level with the specified int.
 	 * @param level The int value to which level is to be set.
 	 
 	
 		
 	 */
 	void FlagCmdRow::setLevel (int level)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->level = level;
	
 	}
	
	

	

	
 	/**
 	 * Get severity.
 	 * @return severity as int
 	 */
 	int FlagCmdRow::getSeverity() const {
	
  		return severity;
 	}

 	/**
 	 * Set severity with the specified int.
 	 * @param severity The int value to which severity is to be set.
 	 
 	
 		
 	 */
 	void FlagCmdRow::setSeverity (int severity)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->severity = severity;
	
 	}
	
	

	

	
 	/**
 	 * Get applied.
 	 * @return applied as bool
 	 */
 	bool FlagCmdRow::getApplied() const {
	
  		return applied;
 	}

 	/**
 	 * Set applied with the specified bool.
 	 * @param applied The bool value to which applied is to be set.
 	 
 	
 		
 	 */
 	void FlagCmdRow::setApplied (bool applied)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->applied = applied;
	
 	}
	
	

	

	
 	/**
 	 * Get command.
 	 * @return command as std::string
 	 */
 	std::string FlagCmdRow::getCommand() const {
	
  		return command;
 	}

 	/**
 	 * Set command with the specified std::string.
 	 * @param command The std::string value to which command is to be set.
 	 
 	
 		
 	 */
 	void FlagCmdRow::setCommand (std::string command)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->command = command;
	
 	}
	
	

	
	///////////////////////////////////////////////
	// Extrinsic Table Attributes getters/setters//
	///////////////////////////////////////////////
	

	//////////////////////////////////////
	// Links Attributes getters/setters //
	//////////////////////////////////////
	
	
	/**
	 * Create a FlagCmdRow.
	 * <p>
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 */ 
	FlagCmdRow::FlagCmdRow (FlagCmdTable &t) : table(t) {
		hasBeenAdded = false;
		
	
	

	

	

	

	

	

	

	
	
	
	
	

	

	

	

	

	

	

	
	
	 fromBinMethods["timeInterval"] = &FlagCmdRow::timeIntervalFromBin; 
	 fromBinMethods["type"] = &FlagCmdRow::typeFromBin; 
	 fromBinMethods["reason"] = &FlagCmdRow::reasonFromBin; 
	 fromBinMethods["level"] = &FlagCmdRow::levelFromBin; 
	 fromBinMethods["severity"] = &FlagCmdRow::severityFromBin; 
	 fromBinMethods["applied"] = &FlagCmdRow::appliedFromBin; 
	 fromBinMethods["command"] = &FlagCmdRow::commandFromBin; 
		
	
	
	
	
	
				 
	fromTextMethods["timeInterval"] = &FlagCmdRow::timeIntervalFromText;
		 
	
				 
	fromTextMethods["type"] = &FlagCmdRow::typeFromText;
		 
	
				 
	fromTextMethods["reason"] = &FlagCmdRow::reasonFromText;
		 
	
				 
	fromTextMethods["level"] = &FlagCmdRow::levelFromText;
		 
	
				 
	fromTextMethods["severity"] = &FlagCmdRow::severityFromText;
		 
	
				 
	fromTextMethods["applied"] = &FlagCmdRow::appliedFromText;
		 
	
				 
	fromTextMethods["command"] = &FlagCmdRow::commandFromText;
		 
	

		
	}
	
	FlagCmdRow::FlagCmdRow (FlagCmdTable &t, FlagCmdRow *row) : table(t) {
		hasBeenAdded = false;
		
		if (row == 0) {
	
	
	

	

	

	

	

	

	

			
		}
		else {
	
		
			timeInterval = row->timeInterval;
		
		
		
		
			type = row->type;
		
			reason = row->reason;
		
			level = row->level;
		
			severity = row->severity;
		
			applied = row->applied;
		
			command = row->command;
		
		
		
		
		}
		
		 fromBinMethods["timeInterval"] = &FlagCmdRow::timeIntervalFromBin; 
		 fromBinMethods["type"] = &FlagCmdRow::typeFromBin; 
		 fromBinMethods["reason"] = &FlagCmdRow::reasonFromBin; 
		 fromBinMethods["level"] = &FlagCmdRow::levelFromBin; 
		 fromBinMethods["severity"] = &FlagCmdRow::severityFromBin; 
		 fromBinMethods["applied"] = &FlagCmdRow::appliedFromBin; 
		 fromBinMethods["command"] = &FlagCmdRow::commandFromBin; 
			
	
			
	}

	
	bool FlagCmdRow::compareNoAutoInc(ArrayTimeInterval timeInterval, std::string type, std::string reason, int level, int severity, bool applied, std::string command) {
		bool result;
		result = true;
		
	
		
		result = result && (this->timeInterval.overlaps(timeInterval));
		
		if (!result) return false;
	

	
		
		result = result && (this->type == type);
		
		if (!result) return false;
	

	
		
		result = result && (this->reason == reason);
		
		if (!result) return false;
	

	
		
		result = result && (this->level == level);
		
		if (!result) return false;
	

	
		
		result = result && (this->severity == severity);
		
		if (!result) return false;
	

	
		
		result = result && (this->applied == applied);
		
		if (!result) return false;
	

	
		
		result = result && (this->command == command);
		
		if (!result) return false;
	

		return result;
	}	
	
	
	
	bool FlagCmdRow::compareRequiredValue(std::string type, std::string reason, int level, int severity, bool applied, std::string command) {
		bool result;
		result = true;
		
	
		if (!(this->type == type)) return false;
	

	
		if (!(this->reason == reason)) return false;
	

	
		if (!(this->level == level)) return false;
	

	
		if (!(this->severity == severity)) return false;
	

	
		if (!(this->applied == applied)) return false;
	

	
		if (!(this->command == command)) return false;
	

		return result;
	}
	
	
	/**
	 * Return true if all required attributes of the value part are equal to their homologues
	 * in x and false otherwise.
	 *

	 * @param x a pointer on the FlagCmdRow whose required attributes of the value part 

	 * will be compared with those of this.
	 * @return a boolean.
	 */
	bool FlagCmdRow::equalByRequiredValue(FlagCmdRow*  x ) {
		
			
		if (this->type != x->type) return false;
			
		if (this->reason != x->reason) return false;
			
		if (this->level != x->level) return false;
			
		if (this->severity != x->severity) return false;
			
		if (this->applied != x->applied) return false;
			
		if (this->command != x->command) return false;
			
		
		return true;
	}	
	
/*
	 map<string, FlagCmdAttributeFromBin> FlagCmdRow::initFromBinMethods() {
		map<string, FlagCmdAttributeFromBin> result;
		
		result["timeInterval"] = &FlagCmdRow::timeIntervalFromBin;
		result["type"] = &FlagCmdRow::typeFromBin;
		result["reason"] = &FlagCmdRow::reasonFromBin;
		result["level"] = &FlagCmdRow::levelFromBin;
		result["severity"] = &FlagCmdRow::severityFromBin;
		result["applied"] = &FlagCmdRow::appliedFromBin;
		result["command"] = &FlagCmdRow::commandFromBin;
		
		
			
		
		return result;	
	}
*/	
} // End namespace asdm
 
