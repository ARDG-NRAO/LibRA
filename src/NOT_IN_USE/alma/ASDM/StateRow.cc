
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
 * File StateRow.cpp
 */
 
#include <vector>
#include <set>

#include <alma/ASDM/ASDM.h>
#include <alma/ASDM/StateRow.h>
#include <alma/ASDM/StateTable.h>
	

using asdm::ASDM;
using asdm::StateRow;
using asdm::StateTable;


#include <alma/ASDM/Parser.h>

#include <alma/ASDM/EnumerationParser.h>
#include <alma/ASDM/ASDMValuesParser.h>
 
#include <alma/ASDM/InvalidArgumentException.h>

using namespace std;

namespace asdm {
	StateRow::~StateRow() {
	}

	/**
	 * Return the table to which this row belongs.
	 */
	StateTable &StateRow::getTable() const {
		return table;
	}

	bool StateRow::isAdded() const {
		return hasBeenAdded;
	}	

	void StateRow::isAdded(bool added) {
		hasBeenAdded = added;
	}
	
#ifndef WITHOUT_ACS
	using asdmIDL::StateRowIDL;
#endif
	
#ifndef WITHOUT_ACS
	/**
	 * Return this row in the form of an IDL struct.
	 * @return The values of this row as a StateRowIDL struct.
	 */
	StateRowIDL *StateRow::toIDL() const {
		StateRowIDL *x = new StateRowIDL ();
		
		// Fill the IDL structure.
	
		
	
  		
		
		
			
		x->stateId = stateId.toIDLTag();
			
		
	

	
  		
		
		
			
				
		x->calDeviceName = calDeviceName;
 				
 			
		
	

	
  		
		
		
			
				
		x->sig = sig;
 				
 			
		
	

	
  		
		
		
			
				
		x->ref = ref;
 				
 			
		
	

	
  		
		
		
			
				
		x->onSky = onSky;
 				
 			
		
	

	
  		
		
		x->weightExists = weightExists;
		
		
			
				
		x->weight = weight;
 				
 			
		
	

	
	
		
		
		return x;
	
	}
	
	void StateRow::toIDL(asdmIDL::StateRowIDL& x) const {
		// Set the x's fields.
	
		
	
  		
		
		
			
		x.stateId = stateId.toIDLTag();
			
		
	

	
  		
		
		
			
				
		x.calDeviceName = calDeviceName;
 				
 			
		
	

	
  		
		
		
			
				
		x.sig = sig;
 				
 			
		
	

	
  		
		
		
			
				
		x.ref = ref;
 				
 			
		
	

	
  		
		
		
			
				
		x.onSky = onSky;
 				
 			
		
	

	
  		
		
		x.weightExists = weightExists;
		
		
			
				
		x.weight = weight;
 				
 			
		
	

	
	
		
	
	}
#endif
	

#ifndef WITHOUT_ACS
	/**
	 * Fill the values of this row from the IDL struct StateRowIDL.
	 * @param x The IDL struct containing the values used to fill this row.
	 */
	void StateRow::setFromIDL (StateRowIDL x){
		try {
		// Fill the values from x.
	
		
	
		
		
			
		setStateId(Tag (x.stateId));
			
 		
		
	

	
		
		
			
		setCalDeviceName(x.calDeviceName);
  			
 		
		
	

	
		
		
			
		setSig(x.sig);
  			
 		
		
	

	
		
		
			
		setRef(x.ref);
  			
 		
		
	

	
		
		
			
		setOnSky(x.onSky);
  			
 		
		
	

	
		
		weightExists = x.weightExists;
		if (x.weightExists) {
		
		
			
		setWeight(x.weight);
  			
 		
		
		}
		
	

	
	
		
		} catch (const IllegalAccessException &err) {
			throw ConversionException (err.getMessage(),"State");
		}
	}
#endif
	
	/**
	 * Return this row in the form of an XML string.
	 * @return The values of this row as an XML string.
	 */
	string StateRow::toXML() const {
		string buf;
		buf.append("<row> \n");
		
	
		
  	
 		
		
		Parser::toXML(stateId, "stateId", buf);
		
		
	

  	
 		
		
			buf.append(EnumerationParser::toXML("calDeviceName", calDeviceName));
		
		
	

  	
 		
		
		Parser::toXML(sig, "sig", buf);
		
		
	

  	
 		
		
		Parser::toXML(ref, "ref", buf);
		
		
	

  	
 		
		
		Parser::toXML(onSky, "onSky", buf);
		
		
	

  	
 		
		if (weightExists) {
		
		
		Parser::toXML(weight, "weight", buf);
		
		
		}
		
	

	
	
		
		
		buf.append("</row>\n");
		return buf;
	}

	/**
	 * Fill the values of this row from an XML string 
	 * that was produced by the toXML() method.
	 * @param x The XML string being used to set the values of this row.
	 */
	void StateRow::setFromXML (string rowDoc) {
		Parser row(rowDoc);
		string s = "";
		try {
	
		
	
  		
			
	  	setStateId(Parser::getTag("stateId","State",rowDoc));
			
		
	

	
		
		
		
		calDeviceName = EnumerationParser::getCalibrationDevice("calDeviceName","State",rowDoc);
		
		
		
	

	
  		
			
	  	setSig(Parser::getBoolean("sig","State",rowDoc));
			
		
	

	
  		
			
	  	setRef(Parser::getBoolean("ref","State",rowDoc));
			
		
	

	
  		
			
	  	setOnSky(Parser::getBoolean("onSky","State",rowDoc));
			
		
	

	
  		
        if (row.isStr("<weight>")) {
			
	  		setWeight(Parser::getFloat("weight","State",rowDoc));
			
		}
 		
	

	
	
		
		} catch (const IllegalAccessException &err) {
			throw ConversionException (err.getMessage(),"State");
		}
	}
	
	void StateRow::toBin(EndianOSStream& eoss) {
	
	
	
	
		
	stateId.toBin(eoss);
		
	

	
	
		
					
		eoss.writeString(CCalibrationDevice::name(calDeviceName));
			/* eoss.writeInt(calDeviceName); */
				
		
	

	
	
		
						
			eoss.writeBoolean(sig);
				
		
	

	
	
		
						
			eoss.writeBoolean(ref);
				
		
	

	
	
		
						
			eoss.writeBoolean(onSky);
				
		
	


	
	
	eoss.writeBoolean(weightExists);
	if (weightExists) {
	
	
	
		
						
			eoss.writeFloat(weight);
				
		
	

	}

	}
	
void StateRow::stateIdFromBin(EndianIStream& eis) {
		
	
		
		
		stateId =  Tag::fromBin(eis);
		
	
	
}
void StateRow::calDeviceNameFromBin(EndianIStream& eis) {
		
	
	
		
			
		calDeviceName = CCalibrationDevice::literal(eis.readString());
			
		
	
	
}
void StateRow::sigFromBin(EndianIStream& eis) {
		
	
	
		
			
		sig =  eis.readBoolean();
			
		
	
	
}
void StateRow::refFromBin(EndianIStream& eis) {
		
	
	
		
			
		ref =  eis.readBoolean();
			
		
	
	
}
void StateRow::onSkyFromBin(EndianIStream& eis) {
		
	
	
		
			
		onSky =  eis.readBoolean();
			
		
	
	
}

void StateRow::weightFromBin(EndianIStream& eis) {
		
	weightExists = eis.readBoolean();
	if (weightExists) {
		
	
	
		
			
		weight =  eis.readFloat();
			
		
	

	}
	
}
	
	
	StateRow* StateRow::fromBin(EndianIStream& eis, StateTable& table, const vector<string>& attributesSeq) {
		StateRow* row = new  StateRow(table);
		
		map<string, StateAttributeFromBin>::iterator iter ;
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
					throw ConversionException("There is not method to read an attribute '"+attributesSeq.at(i)+"'.", "StateTable");
			}
				
		}				
		return row;
	}

	//
	// A collection of methods to set the value of the attributes from their textual value in the XML representation
	// of one row.
	//
	
	// Convert a string into an Tag 
	void StateRow::stateIdFromText(const string & s) {
		 
          
		stateId = ASDMValuesParser::parse<Tag>(s);
          
		
	}
	
	
	// Convert a string into an CalibrationDevice 
	void StateRow::calDeviceNameFromText(const string & s) {
		 
          
		calDeviceName = ASDMValuesParser::parse<CalibrationDeviceMod::CalibrationDevice>(s);
          
		
	}
	
	
	// Convert a string into an boolean 
	void StateRow::sigFromText(const string & s) {
		 
          
		sig = ASDMValuesParser::parse<bool>(s);
          
		
	}
	
	
	// Convert a string into an boolean 
	void StateRow::refFromText(const string & s) {
		 
          
		ref = ASDMValuesParser::parse<bool>(s);
          
		
	}
	
	
	// Convert a string into an boolean 
	void StateRow::onSkyFromText(const string & s) {
		 
          
		onSky = ASDMValuesParser::parse<bool>(s);
          
		
	}
	

	
	// Convert a string into an float 
	void StateRow::weightFromText(const string & s) {
		weightExists = true;
		 
          
		weight = ASDMValuesParser::parse<float>(s);
          
		
	}
	
	
	
	void StateRow::fromText(const std::string& attributeName, const std::string&  t) {
		map<string, StateAttributeFromText>::iterator iter;
		if ((iter = fromTextMethods.find(attributeName)) == fromTextMethods.end())
			throw ConversionException("I do not know what to do with '"+attributeName+"' and its content '"+t+"' (while parsing an XML document)", "StateTable");
		(this->*(iter->second))(t);
	}
			
	////////////////////////////////////////////////
	// Intrinsic Table Attributes getters/setters //
	////////////////////////////////////////////////
	
	

	
 	/**
 	 * Get stateId.
 	 * @return stateId as Tag
 	 */
 	Tag StateRow::getStateId() const {
	
  		return stateId;
 	}

 	/**
 	 * Set stateId with the specified Tag.
 	 * @param stateId The Tag value to which stateId is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void StateRow::setStateId (Tag stateId)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("stateId", "State");
		
  		}
  	
 		this->stateId = stateId;
	
 	}
	
	

	

	
 	/**
 	 * Get calDeviceName.
 	 * @return calDeviceName as CalibrationDeviceMod::CalibrationDevice
 	 */
 	CalibrationDeviceMod::CalibrationDevice StateRow::getCalDeviceName() const {
	
  		return calDeviceName;
 	}

 	/**
 	 * Set calDeviceName with the specified CalibrationDeviceMod::CalibrationDevice.
 	 * @param calDeviceName The CalibrationDeviceMod::CalibrationDevice value to which calDeviceName is to be set.
 	 
 	
 		
 	 */
 	void StateRow::setCalDeviceName (CalibrationDeviceMod::CalibrationDevice calDeviceName)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->calDeviceName = calDeviceName;
	
 	}
	
	

	

	
 	/**
 	 * Get sig.
 	 * @return sig as bool
 	 */
 	bool StateRow::getSig() const {
	
  		return sig;
 	}

 	/**
 	 * Set sig with the specified bool.
 	 * @param sig The bool value to which sig is to be set.
 	 
 	
 		
 	 */
 	void StateRow::setSig (bool sig)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->sig = sig;
	
 	}
	
	

	

	
 	/**
 	 * Get ref.
 	 * @return ref as bool
 	 */
 	bool StateRow::getRef() const {
	
  		return ref;
 	}

 	/**
 	 * Set ref with the specified bool.
 	 * @param ref The bool value to which ref is to be set.
 	 
 	
 		
 	 */
 	void StateRow::setRef (bool ref)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->ref = ref;
	
 	}
	
	

	

	
 	/**
 	 * Get onSky.
 	 * @return onSky as bool
 	 */
 	bool StateRow::getOnSky() const {
	
  		return onSky;
 	}

 	/**
 	 * Set onSky with the specified bool.
 	 * @param onSky The bool value to which onSky is to be set.
 	 
 	
 		
 	 */
 	void StateRow::setOnSky (bool onSky)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->onSky = onSky;
	
 	}
	
	

	
	/**
	 * The attribute weight is optional. Return true if this attribute exists.
	 * @return true if and only if the weight attribute exists. 
	 */
	bool StateRow::isWeightExists() const {
		return weightExists;
	}
	

	
 	/**
 	 * Get weight, which is optional.
 	 * @return weight as float
 	 * @throw IllegalAccessException If weight does not exist.
 	 */
 	float StateRow::getWeight() const  {
		if (!weightExists) {
			throw IllegalAccessException("weight", "State");
		}
	
  		return weight;
 	}

 	/**
 	 * Set weight with the specified float.
 	 * @param weight The float value to which weight is to be set.
 	 
 	
 	 */
 	void StateRow::setWeight (float weight) {
	
 		this->weight = weight;
	
		weightExists = true;
	
 	}
	
	
	/**
	 * Mark weight, which is an optional field, as non-existent.
	 */
	void StateRow::clearWeight () {
		weightExists = false;
	}
	

	
	///////////////////////////////////////////////
	// Extrinsic Table Attributes getters/setters//
	///////////////////////////////////////////////
	

	//////////////////////////////////////
	// Links Attributes getters/setters //
	//////////////////////////////////////
	
	
	/**
	 * Create a StateRow.
	 * <p>
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 */ 
	StateRow::StateRow (StateTable &t) : table(t) {
		hasBeenAdded = false;
		
	
	

	

	

	

	

	
		weightExists = false;
	

	
	
	
	
	

	
// This attribute is scalar and has an enumeration type. Let's initialize it to some valid value (the 1st of the enumeration).		
calDeviceName = CCalibrationDevice::from_int(0);
	

	

	

	

	

	
	
	 fromBinMethods["stateId"] = &StateRow::stateIdFromBin; 
	 fromBinMethods["calDeviceName"] = &StateRow::calDeviceNameFromBin; 
	 fromBinMethods["sig"] = &StateRow::sigFromBin; 
	 fromBinMethods["ref"] = &StateRow::refFromBin; 
	 fromBinMethods["onSky"] = &StateRow::onSkyFromBin; 
		
	
	 fromBinMethods["weight"] = &StateRow::weightFromBin; 
	
	
	
	
				 
	fromTextMethods["stateId"] = &StateRow::stateIdFromText;
		 
	
				 
	fromTextMethods["calDeviceName"] = &StateRow::calDeviceNameFromText;
		 
	
				 
	fromTextMethods["sig"] = &StateRow::sigFromText;
		 
	
				 
	fromTextMethods["ref"] = &StateRow::refFromText;
		 
	
				 
	fromTextMethods["onSky"] = &StateRow::onSkyFromText;
		 
	

	 
				
	fromTextMethods["weight"] = &StateRow::weightFromText;
		 	
		
	}
	
	StateRow::StateRow (StateTable &t, StateRow *row) : table(t) {
		hasBeenAdded = false;
		
		if (row == 0) {
	
	
	

	

	

	

	

	
		weightExists = false;
	

			
		}
		else {
	
		
			stateId = row->stateId;
		
		
		
		
			calDeviceName = row->calDeviceName;
		
			sig = row->sig;
		
			ref = row->ref;
		
			onSky = row->onSky;
		
		
		
		
		if (row->weightExists) {
			weight = row->weight;		
			weightExists = true;
		}
		else
			weightExists = false;
		
		}
		
		 fromBinMethods["stateId"] = &StateRow::stateIdFromBin; 
		 fromBinMethods["calDeviceName"] = &StateRow::calDeviceNameFromBin; 
		 fromBinMethods["sig"] = &StateRow::sigFromBin; 
		 fromBinMethods["ref"] = &StateRow::refFromBin; 
		 fromBinMethods["onSky"] = &StateRow::onSkyFromBin; 
			
	
		 fromBinMethods["weight"] = &StateRow::weightFromBin; 
			
	}

	
	bool StateRow::compareNoAutoInc(CalibrationDeviceMod::CalibrationDevice calDeviceName, bool sig, bool ref, bool onSky) {
		bool result;
		result = true;
		
	
		
		result = result && (this->calDeviceName == calDeviceName);
		
		if (!result) return false;
	

	
		
		result = result && (this->sig == sig);
		
		if (!result) return false;
	

	
		
		result = result && (this->ref == ref);
		
		if (!result) return false;
	

	
		
		result = result && (this->onSky == onSky);
		
		if (!result) return false;
	

		return result;
	}	
	
	
	
	bool StateRow::compareRequiredValue(CalibrationDeviceMod::CalibrationDevice calDeviceName, bool sig, bool ref, bool onSky) {
		bool result;
		result = true;
		
	
		if (!(this->calDeviceName == calDeviceName)) return false;
	

	
		if (!(this->sig == sig)) return false;
	

	
		if (!(this->ref == ref)) return false;
	

	
		if (!(this->onSky == onSky)) return false;
	

		return result;
	}
	
	
	/**
	 * Return true if all required attributes of the value part are equal to their homologues
	 * in x and false otherwise.
	 *

	 * @param x a pointer on the StateRow whose required attributes of the value part 

	 * will be compared with those of this.
	 * @return a boolean.
	 */
	bool StateRow::equalByRequiredValue(StateRow*  x ) {
		
			
		if (this->calDeviceName != x->calDeviceName) return false;
			
		if (this->sig != x->sig) return false;
			
		if (this->ref != x->ref) return false;
			
		if (this->onSky != x->onSky) return false;
			
		
		return true;
	}	
	
/*
	 map<string, StateAttributeFromBin> StateRow::initFromBinMethods() {
		map<string, StateAttributeFromBin> result;
		
		result["stateId"] = &StateRow::stateIdFromBin;
		result["calDeviceName"] = &StateRow::calDeviceNameFromBin;
		result["sig"] = &StateRow::sigFromBin;
		result["ref"] = &StateRow::refFromBin;
		result["onSky"] = &StateRow::onSkyFromBin;
		
		
		result["weight"] = &StateRow::weightFromBin;
			
		
		return result;	
	}
*/	
} // End namespace asdm
 
