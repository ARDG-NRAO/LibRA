
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
 * File StationRow.cpp
 */
 
#include <vector>
#include <set>

#include <alma/ASDM/ASDM.h>
#include <alma/ASDM/StationRow.h>
#include <alma/ASDM/StationTable.h>
	

using asdm::ASDM;
using asdm::StationRow;
using asdm::StationTable;


#include <alma/ASDM/Parser.h>

#include <alma/ASDM/EnumerationParser.h>
#include <alma/ASDM/ASDMValuesParser.h>
 
#include <alma/ASDM/InvalidArgumentException.h>

using namespace std;

namespace asdm {
	StationRow::~StationRow() {
	}

	/**
	 * Return the table to which this row belongs.
	 */
	StationTable &StationRow::getTable() const {
		return table;
	}

	bool StationRow::isAdded() const {
		return hasBeenAdded;
	}	

	void StationRow::isAdded(bool added) {
		hasBeenAdded = added;
	}
	
#ifndef WITHOUT_ACS
	using asdmIDL::StationRowIDL;
#endif
	
#ifndef WITHOUT_ACS
	/**
	 * Return this row in the form of an IDL struct.
	 * @return The values of this row as a StationRowIDL struct.
	 */
	StationRowIDL *StationRow::toIDL() const {
		StationRowIDL *x = new StationRowIDL ();
		
		// Fill the IDL structure.
	
		
	
  		
		
		
			
		x->stationId = stationId.toIDLTag();
			
		
	

	
  		
		
		
			
				
		x->name = CORBA::string_dup(name.c_str());
				
 			
		
	

	
  		
		
		
			
		x->position.length(position.size());
		for (unsigned int i = 0; i < position.size(); ++i) {
			
			x->position[i] = position.at(i).toIDLLength();
			
	 	}
			
		
	

	
  		
		
		
			
				
		x->type = type;
 				
 			
		
	

	
  		
		
		x->timeExists = timeExists;
		
		
			
		x->time = time.toIDLArrayTime();
			
		
	

	
	
		
		
		return x;
	
	}
	
	void StationRow::toIDL(asdmIDL::StationRowIDL& x) const {
		// Set the x's fields.
	
		
	
  		
		
		
			
		x.stationId = stationId.toIDLTag();
			
		
	

	
  		
		
		
			
				
		x.name = CORBA::string_dup(name.c_str());
				
 			
		
	

	
  		
		
		
			
		x.position.length(position.size());
		for (unsigned int i = 0; i < position.size(); ++i) {
			
			x.position[i] = position.at(i).toIDLLength();
			
	 	}
			
		
	

	
  		
		
		
			
				
		x.type = type;
 				
 			
		
	

	
  		
		
		x.timeExists = timeExists;
		
		
			
		x.time = time.toIDLArrayTime();
			
		
	

	
	
		
	
	}
#endif
	

#ifndef WITHOUT_ACS
	/**
	 * Fill the values of this row from the IDL struct StationRowIDL.
	 * @param x The IDL struct containing the values used to fill this row.
	 */
	void StationRow::setFromIDL (StationRowIDL x){
		try {
		// Fill the values from x.
	
		
	
		
		
			
		setStationId(Tag (x.stationId));
			
 		
		
	

	
		
		
			
		setName(string (x.name));
			
 		
		
	

	
		
		
			
		position .clear();
		for (unsigned int i = 0; i <x.position.length(); ++i) {
			
			position.push_back(Length (x.position[i]));
			
		}
			
  		
		
	

	
		
		
			
		setType(x.type);
  			
 		
		
	

	
		
		timeExists = x.timeExists;
		if (x.timeExists) {
		
		
			
		setTime(ArrayTime (x.time));
			
 		
		
		}
		
	

	
	
		
		} catch (const IllegalAccessException &err) {
			throw ConversionException (err.getMessage(),"Station");
		}
	}
#endif
	
	/**
	 * Return this row in the form of an XML string.
	 * @return The values of this row as an XML string.
	 */
	string StationRow::toXML() const {
		string buf;
		buf.append("<row> \n");
		
	
		
  	
 		
		
		Parser::toXML(stationId, "stationId", buf);
		
		
	

  	
 		
		
		Parser::toXML(name, "name", buf);
		
		
	

  	
 		
		
		Parser::toXML(position, "position", buf);
		
		
	

  	
 		
		
			buf.append(EnumerationParser::toXML("type", type));
		
		
	

  	
 		
		if (timeExists) {
		
		
		Parser::toXML(time, "time", buf);
		
		
		}
		
	

	
	
		
		
		buf.append("</row>\n");
		return buf;
	}

	/**
	 * Fill the values of this row from an XML string 
	 * that was produced by the toXML() method.
	 * @param x The XML string being used to set the values of this row.
	 */
	void StationRow::setFromXML (string rowDoc) {
		Parser row(rowDoc);
		string s = "";
		try {
	
		
	
  		
			
	  	setStationId(Parser::getTag("stationId","Station",rowDoc));
			
		
	

	
  		
			
	  	setName(Parser::getString("name","Station",rowDoc));
			
		
	

	
  		
			
					
	  	setPosition(Parser::get1DLength("position","Station",rowDoc));
	  			
	  		
		
	

	
		
		
		
		type = EnumerationParser::getStationType("type","Station",rowDoc);
		
		
		
	

	
  		
        if (row.isStr("<time>")) {
			
	  		setTime(Parser::getArrayTime("time","Station",rowDoc));
			
		}
 		
	

	
	
		
		} catch (const IllegalAccessException &err) {
			throw ConversionException (err.getMessage(),"Station");
		}
	}
	
	void StationRow::toBin(EndianOSStream& eoss) {
	
	
	
	
		
	stationId.toBin(eoss);
		
	

	
	
		
						
			eoss.writeString(name);
				
		
	

	
	
		
	Length::toBin(position, eoss);
		
	

	
	
		
					
		eoss.writeString(CStationType::name(type));
			/* eoss.writeInt(type); */
				
		
	


	
	
	eoss.writeBoolean(timeExists);
	if (timeExists) {
	
	
	
		
	time.toBin(eoss);
		
	

	}

	}
	
void StationRow::stationIdFromBin(EndianIStream& eis) {
		
	
		
		
		stationId =  Tag::fromBin(eis);
		
	
	
}
void StationRow::nameFromBin(EndianIStream& eis) {
		
	
	
		
			
		name =  eis.readString();
			
		
	
	
}
void StationRow::positionFromBin(EndianIStream& eis) {
		
	
		
		
			
	
	position = Length::from1DBin(eis);	
	

		
	
	
}
void StationRow::typeFromBin(EndianIStream& eis) {
		
	
	
		
			
		type = CStationType::literal(eis.readString());
			
		
	
	
}

void StationRow::timeFromBin(EndianIStream& eis) {
		
	timeExists = eis.readBoolean();
	if (timeExists) {
		
	
		
		
		time =  ArrayTime::fromBin(eis);
		
	

	}
	
}
	
	
	StationRow* StationRow::fromBin(EndianIStream& eis, StationTable& table, const vector<string>& attributesSeq) {
		StationRow* row = new  StationRow(table);
		
		map<string, StationAttributeFromBin>::iterator iter ;
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
					throw ConversionException("There is not method to read an attribute '"+attributesSeq.at(i)+"'.", "StationTable");
			}
				
		}				
		return row;
	}

	//
	// A collection of methods to set the value of the attributes from their textual value in the XML representation
	// of one row.
	//
	
	// Convert a string into an Tag 
	void StationRow::stationIdFromText(const string & s) {
		 
          
		stationId = ASDMValuesParser::parse<Tag>(s);
          
		
	}
	
	
	// Convert a string into an String 
	void StationRow::nameFromText(const string & s) {
		 
          
		name = ASDMValuesParser::parse<string>(s);
          
		
	}
	
	
	// Convert a string into an Length 
	void StationRow::positionFromText(const string & s) {
		 
          
		position = ASDMValuesParser::parse1D<Length>(s);
          
		
	}
	
	
	// Convert a string into an StationType 
	void StationRow::typeFromText(const string & s) {
		 
          
		type = ASDMValuesParser::parse<StationTypeMod::StationType>(s);
          
		
	}
	

	
	// Convert a string into an ArrayTime 
	void StationRow::timeFromText(const string & s) {
		timeExists = true;
		 
          
		time = ASDMValuesParser::parse<ArrayTime>(s);
          
		
	}
	
	
	
	void StationRow::fromText(const std::string& attributeName, const std::string&  t) {
		map<string, StationAttributeFromText>::iterator iter;
		if ((iter = fromTextMethods.find(attributeName)) == fromTextMethods.end())
			throw ConversionException("I do not know what to do with '"+attributeName+"' and its content '"+t+"' (while parsing an XML document)", "StationTable");
		(this->*(iter->second))(t);
	}
			
	////////////////////////////////////////////////
	// Intrinsic Table Attributes getters/setters //
	////////////////////////////////////////////////
	
	

	
 	/**
 	 * Get stationId.
 	 * @return stationId as Tag
 	 */
 	Tag StationRow::getStationId() const {
	
  		return stationId;
 	}

 	/**
 	 * Set stationId with the specified Tag.
 	 * @param stationId The Tag value to which stationId is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void StationRow::setStationId (Tag stationId)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("stationId", "Station");
		
  		}
  	
 		this->stationId = stationId;
	
 	}
	
	

	

	
 	/**
 	 * Get name.
 	 * @return name as std::string
 	 */
 	std::string StationRow::getName() const {
	
  		return name;
 	}

 	/**
 	 * Set name with the specified std::string.
 	 * @param name The std::string value to which name is to be set.
 	 
 	
 		
 	 */
 	void StationRow::setName (std::string name)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->name = name;
	
 	}
	
	

	

	
 	/**
 	 * Get position.
 	 * @return position as std::vector<Length >
 	 */
 	std::vector<Length > StationRow::getPosition() const {
	
  		return position;
 	}

 	/**
 	 * Set position with the specified std::vector<Length >.
 	 * @param position The std::vector<Length > value to which position is to be set.
 	 
 	
 		
 	 */
 	void StationRow::setPosition (std::vector<Length > position)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->position = position;
	
 	}
	
	

	

	
 	/**
 	 * Get type.
 	 * @return type as StationTypeMod::StationType
 	 */
 	StationTypeMod::StationType StationRow::getType() const {
	
  		return type;
 	}

 	/**
 	 * Set type with the specified StationTypeMod::StationType.
 	 * @param type The StationTypeMod::StationType value to which type is to be set.
 	 
 	
 		
 	 */
 	void StationRow::setType (StationTypeMod::StationType type)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->type = type;
	
 	}
	
	

	
	/**
	 * The attribute time is optional. Return true if this attribute exists.
	 * @return true if and only if the time attribute exists. 
	 */
	bool StationRow::isTimeExists() const {
		return timeExists;
	}
	

	
 	/**
 	 * Get time, which is optional.
 	 * @return time as ArrayTime
 	 * @throw IllegalAccessException If time does not exist.
 	 */
 	ArrayTime StationRow::getTime() const  {
		if (!timeExists) {
			throw IllegalAccessException("time", "Station");
		}
	
  		return time;
 	}

 	/**
 	 * Set time with the specified ArrayTime.
 	 * @param time The ArrayTime value to which time is to be set.
 	 
 	
 	 */
 	void StationRow::setTime (ArrayTime time) {
	
 		this->time = time;
	
		timeExists = true;
	
 	}
	
	
	/**
	 * Mark time, which is an optional field, as non-existent.
	 */
	void StationRow::clearTime () {
		timeExists = false;
	}
	

	
	///////////////////////////////////////////////
	// Extrinsic Table Attributes getters/setters//
	///////////////////////////////////////////////
	

	//////////////////////////////////////
	// Links Attributes getters/setters //
	//////////////////////////////////////
	
	
	/**
	 * Create a StationRow.
	 * <p>
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 */ 
	StationRow::StationRow (StationTable &t) : table(t) {
		hasBeenAdded = false;
		
	
	

	

	

	

	
		timeExists = false;
	

	
	
	
	
	

	

	

	
// This attribute is scalar and has an enumeration type. Let's initialize it to some valid value (the 1st of the enumeration).		
type = CStationType::from_int(0);
	

	

	
	
	 fromBinMethods["stationId"] = &StationRow::stationIdFromBin; 
	 fromBinMethods["name"] = &StationRow::nameFromBin; 
	 fromBinMethods["position"] = &StationRow::positionFromBin; 
	 fromBinMethods["type"] = &StationRow::typeFromBin; 
		
	
	 fromBinMethods["time"] = &StationRow::timeFromBin; 
	
	
	
	
				 
	fromTextMethods["stationId"] = &StationRow::stationIdFromText;
		 
	
				 
	fromTextMethods["name"] = &StationRow::nameFromText;
		 
	
				 
	fromTextMethods["position"] = &StationRow::positionFromText;
		 
	
				 
	fromTextMethods["type"] = &StationRow::typeFromText;
		 
	

	 
				
	fromTextMethods["time"] = &StationRow::timeFromText;
		 	
		
	}
	
	StationRow::StationRow (StationTable &t, StationRow *row) : table(t) {
		hasBeenAdded = false;
		
		if (row == 0) {
	
	
	

	

	

	

	
		timeExists = false;
	

			
		}
		else {
	
		
			stationId = row->stationId;
		
		
		
		
			name = row->name;
		
			position = row->position;
		
			type = row->type;
		
		
		
		
		if (row->timeExists) {
			time = row->time;		
			timeExists = true;
		}
		else
			timeExists = false;
		
		}
		
		 fromBinMethods["stationId"] = &StationRow::stationIdFromBin; 
		 fromBinMethods["name"] = &StationRow::nameFromBin; 
		 fromBinMethods["position"] = &StationRow::positionFromBin; 
		 fromBinMethods["type"] = &StationRow::typeFromBin; 
			
	
		 fromBinMethods["time"] = &StationRow::timeFromBin; 
			
	}

	
	bool StationRow::compareNoAutoInc(std::string name, std::vector<Length > position, StationTypeMod::StationType type) {
		bool result;
		result = true;
		
	
		
		result = result && (this->name == name);
		
		if (!result) return false;
	

	
		
		result = result && (this->position == position);
		
		if (!result) return false;
	

	
		
		result = result && (this->type == type);
		
		if (!result) return false;
	

		return result;
	}	
	
	
	
	bool StationRow::compareRequiredValue(std::string name, std::vector<Length > position, StationTypeMod::StationType type) {
		bool result;
		result = true;
		
	
		if (!(this->name == name)) return false;
	

	
		if (!(this->position == position)) return false;
	

	
		if (!(this->type == type)) return false;
	

		return result;
	}
	
	
	/**
	 * Return true if all required attributes of the value part are equal to their homologues
	 * in x and false otherwise.
	 *

	 * @param x a pointer on the StationRow whose required attributes of the value part 

	 * will be compared with those of this.
	 * @return a boolean.
	 */
	bool StationRow::equalByRequiredValue(StationRow*  x ) {
		
			
		if (this->name != x->name) return false;
			
		if (this->position != x->position) return false;
			
		if (this->type != x->type) return false;
			
		
		return true;
	}	
	
/*
	 map<string, StationAttributeFromBin> StationRow::initFromBinMethods() {
		map<string, StationAttributeFromBin> result;
		
		result["stationId"] = &StationRow::stationIdFromBin;
		result["name"] = &StationRow::nameFromBin;
		result["position"] = &StationRow::positionFromBin;
		result["type"] = &StationRow::typeFromBin;
		
		
		result["time"] = &StationRow::timeFromBin;
			
		
		return result;	
	}
*/	
} // End namespace asdm
 
