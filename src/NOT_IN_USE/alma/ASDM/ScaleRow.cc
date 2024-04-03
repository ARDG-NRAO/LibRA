
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
 * File ScaleRow.cpp
 */
 
#include <vector>
#include <set>

#include <alma/ASDM/ASDM.h>
#include <alma/ASDM/ScaleRow.h>
#include <alma/ASDM/ScaleTable.h>
	

using asdm::ASDM;
using asdm::ScaleRow;
using asdm::ScaleTable;


#include <alma/ASDM/Parser.h>

#include <alma/ASDM/EnumerationParser.h>
#include <alma/ASDM/ASDMValuesParser.h>
 
#include <alma/ASDM/InvalidArgumentException.h>

using namespace std;

namespace asdm {
	ScaleRow::~ScaleRow() {
	}

	/**
	 * Return the table to which this row belongs.
	 */
	ScaleTable &ScaleRow::getTable() const {
		return table;
	}

	bool ScaleRow::isAdded() const {
		return hasBeenAdded;
	}	

	void ScaleRow::isAdded(bool added) {
		hasBeenAdded = added;
	}
	
#ifndef WITHOUT_ACS
	using asdmIDL::ScaleRowIDL;
#endif
	
#ifndef WITHOUT_ACS
	/**
	 * Return this row in the form of an IDL struct.
	 * @return The values of this row as a ScaleRowIDL struct.
	 */
	ScaleRowIDL *ScaleRow::toIDL() const {
		ScaleRowIDL *x = new ScaleRowIDL ();
		
		// Fill the IDL structure.
	
		
	
  		
		
		
			
		x->scaleId = scaleId.toIDLTag();
			
		
	

	
  		
		
		
			
				
		x->timeScale = timeScale;
 				
 			
		
	

	
  		
		
		
			
				
		x->crossDataScale = crossDataScale;
 				
 			
		
	

	
  		
		
		
			
				
		x->autoDataScale = autoDataScale;
 				
 			
		
	

	
  		
		
		
			
				
		x->weightType = weightType;
 				
 			
		
	

	
	
		
		
		return x;
	
	}
	
	void ScaleRow::toIDL(asdmIDL::ScaleRowIDL& x) const {
		// Set the x's fields.
	
		
	
  		
		
		
			
		x.scaleId = scaleId.toIDLTag();
			
		
	

	
  		
		
		
			
				
		x.timeScale = timeScale;
 				
 			
		
	

	
  		
		
		
			
				
		x.crossDataScale = crossDataScale;
 				
 			
		
	

	
  		
		
		
			
				
		x.autoDataScale = autoDataScale;
 				
 			
		
	

	
  		
		
		
			
				
		x.weightType = weightType;
 				
 			
		
	

	
	
		
	
	}
#endif
	

#ifndef WITHOUT_ACS
	/**
	 * Fill the values of this row from the IDL struct ScaleRowIDL.
	 * @param x The IDL struct containing the values used to fill this row.
	 */
	void ScaleRow::setFromIDL (ScaleRowIDL x){
		try {
		// Fill the values from x.
	
		
	
		
		
			
		setScaleId(Tag (x.scaleId));
			
 		
		
	

	
		
		
			
		setTimeScale(x.timeScale);
  			
 		
		
	

	
		
		
			
		setCrossDataScale(x.crossDataScale);
  			
 		
		
	

	
		
		
			
		setAutoDataScale(x.autoDataScale);
  			
 		
		
	

	
		
		
			
		setWeightType(x.weightType);
  			
 		
		
	

	
	
		
		} catch (const IllegalAccessException &err) {
			throw ConversionException (err.getMessage(),"Scale");
		}
	}
#endif
	
	/**
	 * Return this row in the form of an XML string.
	 * @return The values of this row as an XML string.
	 */
	string ScaleRow::toXML() const {
		string buf;
		buf.append("<row> \n");
		
	
		
  	
 		
		
		Parser::toXML(scaleId, "scaleId", buf);
		
		
	

  	
 		
		
			buf.append(EnumerationParser::toXML("timeScale", timeScale));
		
		
	

  	
 		
		
			buf.append(EnumerationParser::toXML("crossDataScale", crossDataScale));
		
		
	

  	
 		
		
			buf.append(EnumerationParser::toXML("autoDataScale", autoDataScale));
		
		
	

  	
 		
		
			buf.append(EnumerationParser::toXML("weightType", weightType));
		
		
	

	
	
		
		
		buf.append("</row>\n");
		return buf;
	}

	/**
	 * Fill the values of this row from an XML string 
	 * that was produced by the toXML() method.
	 * @param x The XML string being used to set the values of this row.
	 */
	void ScaleRow::setFromXML (string rowDoc) {
		Parser row(rowDoc);
		string s = "";
		try {
	
		
	
  		
			
	  	setScaleId(Parser::getTag("scaleId","Scale",rowDoc));
			
		
	

	
		
		
		
		timeScale = EnumerationParser::getTimeScale("timeScale","Scale",rowDoc);
		
		
		
	

	
		
		
		
		crossDataScale = EnumerationParser::getDataScale("crossDataScale","Scale",rowDoc);
		
		
		
	

	
		
		
		
		autoDataScale = EnumerationParser::getDataScale("autoDataScale","Scale",rowDoc);
		
		
		
	

	
		
		
		
		weightType = EnumerationParser::getWeightType("weightType","Scale",rowDoc);
		
		
		
	

	
	
		
		} catch (const IllegalAccessException &err) {
			throw ConversionException (err.getMessage(),"Scale");
		}
	}
	
	void ScaleRow::toBin(EndianOSStream& eoss) {
	
	
	
	
		
	scaleId.toBin(eoss);
		
	

	
	
		
					
		eoss.writeString(CTimeScale::name(timeScale));
			/* eoss.writeInt(timeScale); */
				
		
	

	
	
		
					
		eoss.writeString(CDataScale::name(crossDataScale));
			/* eoss.writeInt(crossDataScale); */
				
		
	

	
	
		
					
		eoss.writeString(CDataScale::name(autoDataScale));
			/* eoss.writeInt(autoDataScale); */
				
		
	

	
	
		
					
		eoss.writeString(CWeightType::name(weightType));
			/* eoss.writeInt(weightType); */
				
		
	


	
	
	}
	
void ScaleRow::scaleIdFromBin(EndianIStream& eis) {
		
	
		
		
		scaleId =  Tag::fromBin(eis);
		
	
	
}
void ScaleRow::timeScaleFromBin(EndianIStream& eis) {
		
	
	
		
			
		timeScale = CTimeScale::literal(eis.readString());
			
		
	
	
}
void ScaleRow::crossDataScaleFromBin(EndianIStream& eis) {
		
	
	
		
			
		crossDataScale = CDataScale::literal(eis.readString());
			
		
	
	
}
void ScaleRow::autoDataScaleFromBin(EndianIStream& eis) {
		
	
	
		
			
		autoDataScale = CDataScale::literal(eis.readString());
			
		
	
	
}
void ScaleRow::weightTypeFromBin(EndianIStream& eis) {
		
	
	
		
			
		weightType = CWeightType::literal(eis.readString());
			
		
	
	
}

		
	
	ScaleRow* ScaleRow::fromBin(EndianIStream& eis, ScaleTable& table, const vector<string>& attributesSeq) {
		ScaleRow* row = new  ScaleRow(table);
		
		map<string, ScaleAttributeFromBin>::iterator iter ;
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
					throw ConversionException("There is not method to read an attribute '"+attributesSeq.at(i)+"'.", "ScaleTable");
			}
				
		}				
		return row;
	}

	//
	// A collection of methods to set the value of the attributes from their textual value in the XML representation
	// of one row.
	//
	
	// Convert a string into an Tag 
	void ScaleRow::scaleIdFromText(const string & s) {
		 
          
		scaleId = ASDMValuesParser::parse<Tag>(s);
          
		
	}
	
	
	// Convert a string into an TimeScale 
	void ScaleRow::timeScaleFromText(const string & s) {
		 
          
		timeScale = ASDMValuesParser::parse<TimeScaleMod::TimeScale>(s);
          
		
	}
	
	
	// Convert a string into an DataScale 
	void ScaleRow::crossDataScaleFromText(const string & s) {
		 
          
		crossDataScale = ASDMValuesParser::parse<DataScaleMod::DataScale>(s);
          
		
	}
	
	
	// Convert a string into an DataScale 
	void ScaleRow::autoDataScaleFromText(const string & s) {
		 
          
		autoDataScale = ASDMValuesParser::parse<DataScaleMod::DataScale>(s);
          
		
	}
	
	
	// Convert a string into an WeightType 
	void ScaleRow::weightTypeFromText(const string & s) {
		 
          
		weightType = ASDMValuesParser::parse<WeightTypeMod::WeightType>(s);
          
		
	}
	

		
	
	void ScaleRow::fromText(const std::string& attributeName, const std::string&  t) {
		map<string, ScaleAttributeFromText>::iterator iter;
		if ((iter = fromTextMethods.find(attributeName)) == fromTextMethods.end())
			throw ConversionException("I do not know what to do with '"+attributeName+"' and its content '"+t+"' (while parsing an XML document)", "ScaleTable");
		(this->*(iter->second))(t);
	}
			
	////////////////////////////////////////////////
	// Intrinsic Table Attributes getters/setters //
	////////////////////////////////////////////////
	
	

	
 	/**
 	 * Get scaleId.
 	 * @return scaleId as Tag
 	 */
 	Tag ScaleRow::getScaleId() const {
	
  		return scaleId;
 	}

 	/**
 	 * Set scaleId with the specified Tag.
 	 * @param scaleId The Tag value to which scaleId is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void ScaleRow::setScaleId (Tag scaleId)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("scaleId", "Scale");
		
  		}
  	
 		this->scaleId = scaleId;
	
 	}
	
	

	

	
 	/**
 	 * Get timeScale.
 	 * @return timeScale as TimeScaleMod::TimeScale
 	 */
 	TimeScaleMod::TimeScale ScaleRow::getTimeScale() const {
	
  		return timeScale;
 	}

 	/**
 	 * Set timeScale with the specified TimeScaleMod::TimeScale.
 	 * @param timeScale The TimeScaleMod::TimeScale value to which timeScale is to be set.
 	 
 	
 		
 	 */
 	void ScaleRow::setTimeScale (TimeScaleMod::TimeScale timeScale)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->timeScale = timeScale;
	
 	}
	
	

	

	
 	/**
 	 * Get crossDataScale.
 	 * @return crossDataScale as DataScaleMod::DataScale
 	 */
 	DataScaleMod::DataScale ScaleRow::getCrossDataScale() const {
	
  		return crossDataScale;
 	}

 	/**
 	 * Set crossDataScale with the specified DataScaleMod::DataScale.
 	 * @param crossDataScale The DataScaleMod::DataScale value to which crossDataScale is to be set.
 	 
 	
 		
 	 */
 	void ScaleRow::setCrossDataScale (DataScaleMod::DataScale crossDataScale)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->crossDataScale = crossDataScale;
	
 	}
	
	

	

	
 	/**
 	 * Get autoDataScale.
 	 * @return autoDataScale as DataScaleMod::DataScale
 	 */
 	DataScaleMod::DataScale ScaleRow::getAutoDataScale() const {
	
  		return autoDataScale;
 	}

 	/**
 	 * Set autoDataScale with the specified DataScaleMod::DataScale.
 	 * @param autoDataScale The DataScaleMod::DataScale value to which autoDataScale is to be set.
 	 
 	
 		
 	 */
 	void ScaleRow::setAutoDataScale (DataScaleMod::DataScale autoDataScale)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->autoDataScale = autoDataScale;
	
 	}
	
	

	

	
 	/**
 	 * Get weightType.
 	 * @return weightType as WeightTypeMod::WeightType
 	 */
 	WeightTypeMod::WeightType ScaleRow::getWeightType() const {
	
  		return weightType;
 	}

 	/**
 	 * Set weightType with the specified WeightTypeMod::WeightType.
 	 * @param weightType The WeightTypeMod::WeightType value to which weightType is to be set.
 	 
 	
 		
 	 */
 	void ScaleRow::setWeightType (WeightTypeMod::WeightType weightType)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->weightType = weightType;
	
 	}
	
	

	
	///////////////////////////////////////////////
	// Extrinsic Table Attributes getters/setters//
	///////////////////////////////////////////////
	

	//////////////////////////////////////
	// Links Attributes getters/setters //
	//////////////////////////////////////
	
	
	/**
	 * Create a ScaleRow.
	 * <p>
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 */ 
	ScaleRow::ScaleRow (ScaleTable &t) : table(t) {
		hasBeenAdded = false;
		
	
	

	

	

	

	

	
	
	
	
	

	
// This attribute is scalar and has an enumeration type. Let's initialize it to some valid value (the 1st of the enumeration).		
timeScale = CTimeScale::from_int(0);
	

	
// This attribute is scalar and has an enumeration type. Let's initialize it to some valid value (the 1st of the enumeration).		
crossDataScale = CDataScale::from_int(0);
	

	
// This attribute is scalar and has an enumeration type. Let's initialize it to some valid value (the 1st of the enumeration).		
autoDataScale = CDataScale::from_int(0);
	

	
// This attribute is scalar and has an enumeration type. Let's initialize it to some valid value (the 1st of the enumeration).		
weightType = CWeightType::from_int(0);
	

	
	
	 fromBinMethods["scaleId"] = &ScaleRow::scaleIdFromBin; 
	 fromBinMethods["timeScale"] = &ScaleRow::timeScaleFromBin; 
	 fromBinMethods["crossDataScale"] = &ScaleRow::crossDataScaleFromBin; 
	 fromBinMethods["autoDataScale"] = &ScaleRow::autoDataScaleFromBin; 
	 fromBinMethods["weightType"] = &ScaleRow::weightTypeFromBin; 
		
	
	
	
	
	
				 
	fromTextMethods["scaleId"] = &ScaleRow::scaleIdFromText;
		 
	
				 
	fromTextMethods["timeScale"] = &ScaleRow::timeScaleFromText;
		 
	
				 
	fromTextMethods["crossDataScale"] = &ScaleRow::crossDataScaleFromText;
		 
	
				 
	fromTextMethods["autoDataScale"] = &ScaleRow::autoDataScaleFromText;
		 
	
				 
	fromTextMethods["weightType"] = &ScaleRow::weightTypeFromText;
		 
	

		
	}
	
	ScaleRow::ScaleRow (ScaleTable &t, ScaleRow *row) : table(t) {
		hasBeenAdded = false;
		
		if (row == 0) {
	
	
	

	

	

	

	

			
		}
		else {
	
		
			scaleId = row->scaleId;
		
		
		
		
			timeScale = row->timeScale;
		
			crossDataScale = row->crossDataScale;
		
			autoDataScale = row->autoDataScale;
		
			weightType = row->weightType;
		
		
		
		
		}
		
		 fromBinMethods["scaleId"] = &ScaleRow::scaleIdFromBin; 
		 fromBinMethods["timeScale"] = &ScaleRow::timeScaleFromBin; 
		 fromBinMethods["crossDataScale"] = &ScaleRow::crossDataScaleFromBin; 
		 fromBinMethods["autoDataScale"] = &ScaleRow::autoDataScaleFromBin; 
		 fromBinMethods["weightType"] = &ScaleRow::weightTypeFromBin; 
			
	
			
	}

	
	bool ScaleRow::compareNoAutoInc(TimeScaleMod::TimeScale timeScale, DataScaleMod::DataScale crossDataScale, DataScaleMod::DataScale autoDataScale, WeightTypeMod::WeightType weightType) {
		bool result;
		result = true;
		
	
		
		result = result && (this->timeScale == timeScale);
		
		if (!result) return false;
	

	
		
		result = result && (this->crossDataScale == crossDataScale);
		
		if (!result) return false;
	

	
		
		result = result && (this->autoDataScale == autoDataScale);
		
		if (!result) return false;
	

	
		
		result = result && (this->weightType == weightType);
		
		if (!result) return false;
	

		return result;
	}	
	
	
	
	bool ScaleRow::compareRequiredValue(TimeScaleMod::TimeScale timeScale, DataScaleMod::DataScale crossDataScale, DataScaleMod::DataScale autoDataScale, WeightTypeMod::WeightType weightType) {
		bool result;
		result = true;
		
	
		if (!(this->timeScale == timeScale)) return false;
	

	
		if (!(this->crossDataScale == crossDataScale)) return false;
	

	
		if (!(this->autoDataScale == autoDataScale)) return false;
	

	
		if (!(this->weightType == weightType)) return false;
	

		return result;
	}
	
	
	/**
	 * Return true if all required attributes of the value part are equal to their homologues
	 * in x and false otherwise.
	 *

	 * @param x a pointer on the ScaleRow whose required attributes of the value part 

	 * will be compared with those of this.
	 * @return a boolean.
	 */
	bool ScaleRow::equalByRequiredValue(ScaleRow*  x ) {
		
			
		if (this->timeScale != x->timeScale) return false;
			
		if (this->crossDataScale != x->crossDataScale) return false;
			
		if (this->autoDataScale != x->autoDataScale) return false;
			
		if (this->weightType != x->weightType) return false;
			
		
		return true;
	}	
	
/*
	 map<string, ScaleAttributeFromBin> ScaleRow::initFromBinMethods() {
		map<string, ScaleAttributeFromBin> result;
		
		result["scaleId"] = &ScaleRow::scaleIdFromBin;
		result["timeScale"] = &ScaleRow::timeScaleFromBin;
		result["crossDataScale"] = &ScaleRow::crossDataScaleFromBin;
		result["autoDataScale"] = &ScaleRow::autoDataScaleFromBin;
		result["weightType"] = &ScaleRow::weightTypeFromBin;
		
		
			
		
		return result;	
	}
*/	
} // End namespace asdm
 
