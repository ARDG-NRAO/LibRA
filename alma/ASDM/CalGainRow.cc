
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
 * File CalGainRow.cpp
 */
 
#include <vector>
#include <set>

#include <alma/ASDM/ASDM.h>
#include <alma/ASDM/CalGainRow.h>
#include <alma/ASDM/CalGainTable.h>

#include <alma/ASDM/CalReductionTable.h>
#include <alma/ASDM/CalReductionRow.h>

#include <alma/ASDM/CalDataTable.h>
#include <alma/ASDM/CalDataRow.h>
	

using asdm::ASDM;
using asdm::CalGainRow;
using asdm::CalGainTable;

using asdm::CalReductionTable;
using asdm::CalReductionRow;

using asdm::CalDataTable;
using asdm::CalDataRow;


#include <alma/ASDM/Parser.h>

#include <alma/ASDM/EnumerationParser.h>
#include <alma/ASDM/ASDMValuesParser.h>
 
#include <alma/ASDM/InvalidArgumentException.h>

using namespace std;

namespace asdm {
	CalGainRow::~CalGainRow() {
	}

	/**
	 * Return the table to which this row belongs.
	 */
	CalGainTable &CalGainRow::getTable() const {
		return table;
	}

	bool CalGainRow::isAdded() const {
		return hasBeenAdded;
	}	

	void CalGainRow::isAdded(bool added) {
		hasBeenAdded = added;
	}
	
#ifndef WITHOUT_ACS
	using asdmIDL::CalGainRowIDL;
#endif
	
#ifndef WITHOUT_ACS
	/**
	 * Return this row in the form of an IDL struct.
	 * @return The values of this row as a CalGainRowIDL struct.
	 */
	CalGainRowIDL *CalGainRow::toIDL() const {
		CalGainRowIDL *x = new CalGainRowIDL ();
		
		// Fill the IDL structure.
	
		
	
  		
		
		
			
		x->startValidTime = startValidTime.toIDLArrayTime();
			
		
	

	
  		
		
		
			
		x->endValidTime = endValidTime.toIDLArrayTime();
			
		
	

	
  		
		
		
			
				
		x->gain = gain;
 				
 			
		
	

	
  		
		
		
			
				
		x->gainValid = gainValid;
 				
 			
		
	

	
  		
		
		
			
				
		x->fit = fit;
 				
 			
		
	

	
  		
		
		
			
				
		x->fitWeight = fitWeight;
 				
 			
		
	

	
  		
		
		
			
				
		x->totalGainValid = totalGainValid;
 				
 			
		
	

	
  		
		
		
			
				
		x->totalFit = totalFit;
 				
 			
		
	

	
  		
		
		
			
				
		x->totalFitWeight = totalFitWeight;
 				
 			
		
	

	
	
		
	
  	
 		
		
	 	
			
		x->calDataId = calDataId.toIDLTag();
			
	 	 		
  	

	
  	
 		
		
	 	
			
		x->calReductionId = calReductionId.toIDLTag();
			
	 	 		
  	

	
		
	

	

		
		return x;
	
	}
	
	void CalGainRow::toIDL(asdmIDL::CalGainRowIDL& x) const {
		// Set the x's fields.
	
		
	
  		
		
		
			
		x.startValidTime = startValidTime.toIDLArrayTime();
			
		
	

	
  		
		
		
			
		x.endValidTime = endValidTime.toIDLArrayTime();
			
		
	

	
  		
		
		
			
				
		x.gain = gain;
 				
 			
		
	

	
  		
		
		
			
				
		x.gainValid = gainValid;
 				
 			
		
	

	
  		
		
		
			
				
		x.fit = fit;
 				
 			
		
	

	
  		
		
		
			
				
		x.fitWeight = fitWeight;
 				
 			
		
	

	
  		
		
		
			
				
		x.totalGainValid = totalGainValid;
 				
 			
		
	

	
  		
		
		
			
				
		x.totalFit = totalFit;
 				
 			
		
	

	
  		
		
		
			
				
		x.totalFitWeight = totalFitWeight;
 				
 			
		
	

	
	
		
	
  	
 		
		
	 	
			
		x.calDataId = calDataId.toIDLTag();
			
	 	 		
  	

	
  	
 		
		
	 	
			
		x.calReductionId = calReductionId.toIDLTag();
			
	 	 		
  	

	
		
	

	

	
	}
#endif
	

#ifndef WITHOUT_ACS
	/**
	 * Fill the values of this row from the IDL struct CalGainRowIDL.
	 * @param x The IDL struct containing the values used to fill this row.
	 */
	void CalGainRow::setFromIDL (CalGainRowIDL x){
		try {
		// Fill the values from x.
	
		
	
		
		
			
		setStartValidTime(ArrayTime (x.startValidTime));
			
 		
		
	

	
		
		
			
		setEndValidTime(ArrayTime (x.endValidTime));
			
 		
		
	

	
		
		
			
		setGain(x.gain);
  			
 		
		
	

	
		
		
			
		setGainValid(x.gainValid);
  			
 		
		
	

	
		
		
			
		setFit(x.fit);
  			
 		
		
	

	
		
		
			
		setFitWeight(x.fitWeight);
  			
 		
		
	

	
		
		
			
		setTotalGainValid(x.totalGainValid);
  			
 		
		
	

	
		
		
			
		setTotalFit(x.totalFit);
  			
 		
		
	

	
		
		
			
		setTotalFitWeight(x.totalFitWeight);
  			
 		
		
	

	
	
		
	
		
		
			
		setCalDataId(Tag (x.calDataId));
			
 		
		
	

	
		
		
			
		setCalReductionId(Tag (x.calReductionId));
			
 		
		
	

	
		
	

	

		} catch (const IllegalAccessException &err) {
			throw ConversionException (err.getMessage(),"CalGain");
		}
	}
#endif
	
	/**
	 * Return this row in the form of an XML string.
	 * @return The values of this row as an XML string.
	 */
	string CalGainRow::toXML() const {
		string buf;
		buf.append("<row> \n");
		
	
		
  	
 		
		
		Parser::toXML(startValidTime, "startValidTime", buf);
		
		
	

  	
 		
		
		Parser::toXML(endValidTime, "endValidTime", buf);
		
		
	

  	
 		
		
		Parser::toXML(gain, "gain", buf);
		
		
	

  	
 		
		
		Parser::toXML(gainValid, "gainValid", buf);
		
		
	

  	
 		
		
		Parser::toXML(fit, "fit", buf);
		
		
	

  	
 		
		
		Parser::toXML(fitWeight, "fitWeight", buf);
		
		
	

  	
 		
		
		Parser::toXML(totalGainValid, "totalGainValid", buf);
		
		
	

  	
 		
		
		Parser::toXML(totalFit, "totalFit", buf);
		
		
	

  	
 		
		
		Parser::toXML(totalFitWeight, "totalFitWeight", buf);
		
		
	

	
	
		
  	
 		
		
		Parser::toXML(calDataId, "calDataId", buf);
		
		
	

  	
 		
		
		Parser::toXML(calReductionId, "calReductionId", buf);
		
		
	

	
		
	

	

		
		buf.append("</row>\n");
		return buf;
	}

	/**
	 * Fill the values of this row from an XML string 
	 * that was produced by the toXML() method.
	 * @param x The XML string being used to set the values of this row.
	 */
	void CalGainRow::setFromXML (string rowDoc) {
		Parser row(rowDoc);
		string s = "";
		try {
	
		
	
  		
			
	  	setStartValidTime(Parser::getArrayTime("startValidTime","CalGain",rowDoc));
			
		
	

	
  		
			
	  	setEndValidTime(Parser::getArrayTime("endValidTime","CalGain",rowDoc));
			
		
	

	
  		
			
	  	setGain(Parser::getFloat("gain","CalGain",rowDoc));
			
		
	

	
  		
			
	  	setGainValid(Parser::getBoolean("gainValid","CalGain",rowDoc));
			
		
	

	
  		
			
	  	setFit(Parser::getFloat("fit","CalGain",rowDoc));
			
		
	

	
  		
			
	  	setFitWeight(Parser::getFloat("fitWeight","CalGain",rowDoc));
			
		
	

	
  		
			
	  	setTotalGainValid(Parser::getBoolean("totalGainValid","CalGain",rowDoc));
			
		
	

	
  		
			
	  	setTotalFit(Parser::getFloat("totalFit","CalGain",rowDoc));
			
		
	

	
  		
			
	  	setTotalFitWeight(Parser::getFloat("totalFitWeight","CalGain",rowDoc));
			
		
	

	
	
		
	
  		
			
	  	setCalDataId(Parser::getTag("calDataId","CalData",rowDoc));
			
		
	

	
  		
			
	  	setCalReductionId(Parser::getTag("calReductionId","CalReduction",rowDoc));
			
		
	

	
		
	

	

		} catch (const IllegalAccessException &err) {
			throw ConversionException (err.getMessage(),"CalGain");
		}
	}
	
	void CalGainRow::toBin(EndianOSStream& eoss) {
	
	
	
	
		
	calDataId.toBin(eoss);
		
	

	
	
		
	calReductionId.toBin(eoss);
		
	

	
	
		
	startValidTime.toBin(eoss);
		
	

	
	
		
	endValidTime.toBin(eoss);
		
	

	
	
		
						
			eoss.writeFloat(gain);
				
		
	

	
	
		
						
			eoss.writeBoolean(gainValid);
				
		
	

	
	
		
						
			eoss.writeFloat(fit);
				
		
	

	
	
		
						
			eoss.writeFloat(fitWeight);
				
		
	

	
	
		
						
			eoss.writeBoolean(totalGainValid);
				
		
	

	
	
		
						
			eoss.writeFloat(totalFit);
				
		
	

	
	
		
						
			eoss.writeFloat(totalFitWeight);
				
		
	


	
	
	}
	
void CalGainRow::calDataIdFromBin(EndianIStream& eis) {
		
	
		
		
		calDataId =  Tag::fromBin(eis);
		
	
	
}
void CalGainRow::calReductionIdFromBin(EndianIStream& eis) {
		
	
		
		
		calReductionId =  Tag::fromBin(eis);
		
	
	
}
void CalGainRow::startValidTimeFromBin(EndianIStream& eis) {
		
	
		
		
		startValidTime =  ArrayTime::fromBin(eis);
		
	
	
}
void CalGainRow::endValidTimeFromBin(EndianIStream& eis) {
		
	
		
		
		endValidTime =  ArrayTime::fromBin(eis);
		
	
	
}
void CalGainRow::gainFromBin(EndianIStream& eis) {
		
	
	
		
			
		gain =  eis.readFloat();
			
		
	
	
}
void CalGainRow::gainValidFromBin(EndianIStream& eis) {
		
	
	
		
			
		gainValid =  eis.readBoolean();
			
		
	
	
}
void CalGainRow::fitFromBin(EndianIStream& eis) {
		
	
	
		
			
		fit =  eis.readFloat();
			
		
	
	
}
void CalGainRow::fitWeightFromBin(EndianIStream& eis) {
		
	
	
		
			
		fitWeight =  eis.readFloat();
			
		
	
	
}
void CalGainRow::totalGainValidFromBin(EndianIStream& eis) {
		
	
	
		
			
		totalGainValid =  eis.readBoolean();
			
		
	
	
}
void CalGainRow::totalFitFromBin(EndianIStream& eis) {
		
	
	
		
			
		totalFit =  eis.readFloat();
			
		
	
	
}
void CalGainRow::totalFitWeightFromBin(EndianIStream& eis) {
		
	
	
		
			
		totalFitWeight =  eis.readFloat();
			
		
	
	
}

		
	
	CalGainRow* CalGainRow::fromBin(EndianIStream& eis, CalGainTable& table, const vector<string>& attributesSeq) {
		CalGainRow* row = new  CalGainRow(table);
		
		map<string, CalGainAttributeFromBin>::iterator iter ;
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
					throw ConversionException("There is not method to read an attribute '"+attributesSeq.at(i)+"'.", "CalGainTable");
			}
				
		}				
		return row;
	}

	//
	// A collection of methods to set the value of the attributes from their textual value in the XML representation
	// of one row.
	//
	
	// Convert a string into an Tag 
	void CalGainRow::calDataIdFromText(const string & s) {
		 
          
		calDataId = ASDMValuesParser::parse<Tag>(s);
          
		
	}
	
	
	// Convert a string into an Tag 
	void CalGainRow::calReductionIdFromText(const string & s) {
		 
          
		calReductionId = ASDMValuesParser::parse<Tag>(s);
          
		
	}
	
	
	// Convert a string into an ArrayTime 
	void CalGainRow::startValidTimeFromText(const string & s) {
		 
          
		startValidTime = ASDMValuesParser::parse<ArrayTime>(s);
          
		
	}
	
	
	// Convert a string into an ArrayTime 
	void CalGainRow::endValidTimeFromText(const string & s) {
		 
          
		endValidTime = ASDMValuesParser::parse<ArrayTime>(s);
          
		
	}
	
	
	// Convert a string into an float 
	void CalGainRow::gainFromText(const string & s) {
		 
          
		gain = ASDMValuesParser::parse<float>(s);
          
		
	}
	
	
	// Convert a string into an boolean 
	void CalGainRow::gainValidFromText(const string & s) {
		 
          
		gainValid = ASDMValuesParser::parse<bool>(s);
          
		
	}
	
	
	// Convert a string into an float 
	void CalGainRow::fitFromText(const string & s) {
		 
          
		fit = ASDMValuesParser::parse<float>(s);
          
		
	}
	
	
	// Convert a string into an float 
	void CalGainRow::fitWeightFromText(const string & s) {
		 
          
		fitWeight = ASDMValuesParser::parse<float>(s);
          
		
	}
	
	
	// Convert a string into an boolean 
	void CalGainRow::totalGainValidFromText(const string & s) {
		 
          
		totalGainValid = ASDMValuesParser::parse<bool>(s);
          
		
	}
	
	
	// Convert a string into an float 
	void CalGainRow::totalFitFromText(const string & s) {
		 
          
		totalFit = ASDMValuesParser::parse<float>(s);
          
		
	}
	
	
	// Convert a string into an float 
	void CalGainRow::totalFitWeightFromText(const string & s) {
		 
          
		totalFitWeight = ASDMValuesParser::parse<float>(s);
          
		
	}
	

		
	
	void CalGainRow::fromText(const std::string& attributeName, const std::string&  t) {
		map<string, CalGainAttributeFromText>::iterator iter;
		if ((iter = fromTextMethods.find(attributeName)) == fromTextMethods.end())
			throw ConversionException("I do not know what to do with '"+attributeName+"' and its content '"+t+"' (while parsing an XML document)", "CalGainTable");
		(this->*(iter->second))(t);
	}
			
	////////////////////////////////////////////////
	// Intrinsic Table Attributes getters/setters //
	////////////////////////////////////////////////
	
	

	
 	/**
 	 * Get startValidTime.
 	 * @return startValidTime as ArrayTime
 	 */
 	ArrayTime CalGainRow::getStartValidTime() const {
	
  		return startValidTime;
 	}

 	/**
 	 * Set startValidTime with the specified ArrayTime.
 	 * @param startValidTime The ArrayTime value to which startValidTime is to be set.
 	 
 	
 		
 	 */
 	void CalGainRow::setStartValidTime (ArrayTime startValidTime)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->startValidTime = startValidTime;
	
 	}
	
	

	

	
 	/**
 	 * Get endValidTime.
 	 * @return endValidTime as ArrayTime
 	 */
 	ArrayTime CalGainRow::getEndValidTime() const {
	
  		return endValidTime;
 	}

 	/**
 	 * Set endValidTime with the specified ArrayTime.
 	 * @param endValidTime The ArrayTime value to which endValidTime is to be set.
 	 
 	
 		
 	 */
 	void CalGainRow::setEndValidTime (ArrayTime endValidTime)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->endValidTime = endValidTime;
	
 	}
	
	

	

	
 	/**
 	 * Get gain.
 	 * @return gain as float
 	 */
 	float CalGainRow::getGain() const {
	
  		return gain;
 	}

 	/**
 	 * Set gain with the specified float.
 	 * @param gain The float value to which gain is to be set.
 	 
 	
 		
 	 */
 	void CalGainRow::setGain (float gain)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->gain = gain;
	
 	}
	
	

	

	
 	/**
 	 * Get gainValid.
 	 * @return gainValid as bool
 	 */
 	bool CalGainRow::getGainValid() const {
	
  		return gainValid;
 	}

 	/**
 	 * Set gainValid with the specified bool.
 	 * @param gainValid The bool value to which gainValid is to be set.
 	 
 	
 		
 	 */
 	void CalGainRow::setGainValid (bool gainValid)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->gainValid = gainValid;
	
 	}
	
	

	

	
 	/**
 	 * Get fit.
 	 * @return fit as float
 	 */
 	float CalGainRow::getFit() const {
	
  		return fit;
 	}

 	/**
 	 * Set fit with the specified float.
 	 * @param fit The float value to which fit is to be set.
 	 
 	
 		
 	 */
 	void CalGainRow::setFit (float fit)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->fit = fit;
	
 	}
	
	

	

	
 	/**
 	 * Get fitWeight.
 	 * @return fitWeight as float
 	 */
 	float CalGainRow::getFitWeight() const {
	
  		return fitWeight;
 	}

 	/**
 	 * Set fitWeight with the specified float.
 	 * @param fitWeight The float value to which fitWeight is to be set.
 	 
 	
 		
 	 */
 	void CalGainRow::setFitWeight (float fitWeight)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->fitWeight = fitWeight;
	
 	}
	
	

	

	
 	/**
 	 * Get totalGainValid.
 	 * @return totalGainValid as bool
 	 */
 	bool CalGainRow::getTotalGainValid() const {
	
  		return totalGainValid;
 	}

 	/**
 	 * Set totalGainValid with the specified bool.
 	 * @param totalGainValid The bool value to which totalGainValid is to be set.
 	 
 	
 		
 	 */
 	void CalGainRow::setTotalGainValid (bool totalGainValid)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->totalGainValid = totalGainValid;
	
 	}
	
	

	

	
 	/**
 	 * Get totalFit.
 	 * @return totalFit as float
 	 */
 	float CalGainRow::getTotalFit() const {
	
  		return totalFit;
 	}

 	/**
 	 * Set totalFit with the specified float.
 	 * @param totalFit The float value to which totalFit is to be set.
 	 
 	
 		
 	 */
 	void CalGainRow::setTotalFit (float totalFit)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->totalFit = totalFit;
	
 	}
	
	

	

	
 	/**
 	 * Get totalFitWeight.
 	 * @return totalFitWeight as float
 	 */
 	float CalGainRow::getTotalFitWeight() const {
	
  		return totalFitWeight;
 	}

 	/**
 	 * Set totalFitWeight with the specified float.
 	 * @param totalFitWeight The float value to which totalFitWeight is to be set.
 	 
 	
 		
 	 */
 	void CalGainRow::setTotalFitWeight (float totalFitWeight)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->totalFitWeight = totalFitWeight;
	
 	}
	
	

	
	///////////////////////////////////////////////
	// Extrinsic Table Attributes getters/setters//
	///////////////////////////////////////////////
	
	

	
 	/**
 	 * Get calDataId.
 	 * @return calDataId as Tag
 	 */
 	Tag CalGainRow::getCalDataId() const {
	
  		return calDataId;
 	}

 	/**
 	 * Set calDataId with the specified Tag.
 	 * @param calDataId The Tag value to which calDataId is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void CalGainRow::setCalDataId (Tag calDataId)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("calDataId", "CalGain");
		
  		}
  	
 		this->calDataId = calDataId;
	
 	}
	
	

	

	
 	/**
 	 * Get calReductionId.
 	 * @return calReductionId as Tag
 	 */
 	Tag CalGainRow::getCalReductionId() const {
	
  		return calReductionId;
 	}

 	/**
 	 * Set calReductionId with the specified Tag.
 	 * @param calReductionId The Tag value to which calReductionId is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void CalGainRow::setCalReductionId (Tag calReductionId)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("calReductionId", "CalGain");
		
  		}
  	
 		this->calReductionId = calReductionId;
	
 	}
	
	


	//////////////////////////////////////
	// Links Attributes getters/setters //
	//////////////////////////////////////
	
	
	
	
		

	/**
	 * Returns the pointer to the row in the CalReduction table having CalReduction.calReductionId == calReductionId
	 * @return a CalReductionRow*
	 * 
	 
	 */
	 CalReductionRow* CalGainRow::getCalReductionUsingCalReductionId() {
	 
	 	return table.getContainer().getCalReduction().getRowByKey(calReductionId);
	 }
	 

	

	
	
	
		

	/**
	 * Returns the pointer to the row in the CalData table having CalData.calDataId == calDataId
	 * @return a CalDataRow*
	 * 
	 
	 */
	 CalDataRow* CalGainRow::getCalDataUsingCalDataId() {
	 
	 	return table.getContainer().getCalData().getRowByKey(calDataId);
	 }
	 

	

	
	/**
	 * Create a CalGainRow.
	 * <p>
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 */ 
	CalGainRow::CalGainRow (CalGainTable &t) : table(t) {
		hasBeenAdded = false;
		
	
	

	

	

	

	

	

	

	

	

	
	

	

	
	
	
	

	

	

	

	

	

	

	

	

	
	
	 fromBinMethods["calDataId"] = &CalGainRow::calDataIdFromBin; 
	 fromBinMethods["calReductionId"] = &CalGainRow::calReductionIdFromBin; 
	 fromBinMethods["startValidTime"] = &CalGainRow::startValidTimeFromBin; 
	 fromBinMethods["endValidTime"] = &CalGainRow::endValidTimeFromBin; 
	 fromBinMethods["gain"] = &CalGainRow::gainFromBin; 
	 fromBinMethods["gainValid"] = &CalGainRow::gainValidFromBin; 
	 fromBinMethods["fit"] = &CalGainRow::fitFromBin; 
	 fromBinMethods["fitWeight"] = &CalGainRow::fitWeightFromBin; 
	 fromBinMethods["totalGainValid"] = &CalGainRow::totalGainValidFromBin; 
	 fromBinMethods["totalFit"] = &CalGainRow::totalFitFromBin; 
	 fromBinMethods["totalFitWeight"] = &CalGainRow::totalFitWeightFromBin; 
		
	
	
	
	
	
				 
	fromTextMethods["calDataId"] = &CalGainRow::calDataIdFromText;
		 
	
				 
	fromTextMethods["calReductionId"] = &CalGainRow::calReductionIdFromText;
		 
	
				 
	fromTextMethods["startValidTime"] = &CalGainRow::startValidTimeFromText;
		 
	
				 
	fromTextMethods["endValidTime"] = &CalGainRow::endValidTimeFromText;
		 
	
				 
	fromTextMethods["gain"] = &CalGainRow::gainFromText;
		 
	
				 
	fromTextMethods["gainValid"] = &CalGainRow::gainValidFromText;
		 
	
				 
	fromTextMethods["fit"] = &CalGainRow::fitFromText;
		 
	
				 
	fromTextMethods["fitWeight"] = &CalGainRow::fitWeightFromText;
		 
	
				 
	fromTextMethods["totalGainValid"] = &CalGainRow::totalGainValidFromText;
		 
	
				 
	fromTextMethods["totalFit"] = &CalGainRow::totalFitFromText;
		 
	
				 
	fromTextMethods["totalFitWeight"] = &CalGainRow::totalFitWeightFromText;
		 
	

		
	}
	
	CalGainRow::CalGainRow (CalGainTable &t, CalGainRow *row) : table(t) {
		hasBeenAdded = false;
		
		if (row == 0) {
	
	
	

	

	

	

	

	

	

	

	

	
	

	
		
		}
		else {
	
		
			calDataId = row->calDataId;
		
			calReductionId = row->calReductionId;
		
		
		
		
			startValidTime = row->startValidTime;
		
			endValidTime = row->endValidTime;
		
			gain = row->gain;
		
			gainValid = row->gainValid;
		
			fit = row->fit;
		
			fitWeight = row->fitWeight;
		
			totalGainValid = row->totalGainValid;
		
			totalFit = row->totalFit;
		
			totalFitWeight = row->totalFitWeight;
		
		
		
		
		}
		
		 fromBinMethods["calDataId"] = &CalGainRow::calDataIdFromBin; 
		 fromBinMethods["calReductionId"] = &CalGainRow::calReductionIdFromBin; 
		 fromBinMethods["startValidTime"] = &CalGainRow::startValidTimeFromBin; 
		 fromBinMethods["endValidTime"] = &CalGainRow::endValidTimeFromBin; 
		 fromBinMethods["gain"] = &CalGainRow::gainFromBin; 
		 fromBinMethods["gainValid"] = &CalGainRow::gainValidFromBin; 
		 fromBinMethods["fit"] = &CalGainRow::fitFromBin; 
		 fromBinMethods["fitWeight"] = &CalGainRow::fitWeightFromBin; 
		 fromBinMethods["totalGainValid"] = &CalGainRow::totalGainValidFromBin; 
		 fromBinMethods["totalFit"] = &CalGainRow::totalFitFromBin; 
		 fromBinMethods["totalFitWeight"] = &CalGainRow::totalFitWeightFromBin; 
			
	
			
	}

	
	bool CalGainRow::compareNoAutoInc(Tag calDataId, Tag calReductionId, ArrayTime startValidTime, ArrayTime endValidTime, float gain, bool gainValid, float fit, float fitWeight, bool totalGainValid, float totalFit, float totalFitWeight) {
		bool result;
		result = true;
		
	
		
		result = result && (this->calDataId == calDataId);
		
		if (!result) return false;
	

	
		
		result = result && (this->calReductionId == calReductionId);
		
		if (!result) return false;
	

	
		
		result = result && (this->startValidTime == startValidTime);
		
		if (!result) return false;
	

	
		
		result = result && (this->endValidTime == endValidTime);
		
		if (!result) return false;
	

	
		
		result = result && (this->gain == gain);
		
		if (!result) return false;
	

	
		
		result = result && (this->gainValid == gainValid);
		
		if (!result) return false;
	

	
		
		result = result && (this->fit == fit);
		
		if (!result) return false;
	

	
		
		result = result && (this->fitWeight == fitWeight);
		
		if (!result) return false;
	

	
		
		result = result && (this->totalGainValid == totalGainValid);
		
		if (!result) return false;
	

	
		
		result = result && (this->totalFit == totalFit);
		
		if (!result) return false;
	

	
		
		result = result && (this->totalFitWeight == totalFitWeight);
		
		if (!result) return false;
	

		return result;
	}	
	
	
	
	bool CalGainRow::compareRequiredValue(ArrayTime startValidTime, ArrayTime endValidTime, float gain, bool gainValid, float fit, float fitWeight, bool totalGainValid, float totalFit, float totalFitWeight) {
		bool result;
		result = true;
		
	
		if (!(this->startValidTime == startValidTime)) return false;
	

	
		if (!(this->endValidTime == endValidTime)) return false;
	

	
		if (!(this->gain == gain)) return false;
	

	
		if (!(this->gainValid == gainValid)) return false;
	

	
		if (!(this->fit == fit)) return false;
	

	
		if (!(this->fitWeight == fitWeight)) return false;
	

	
		if (!(this->totalGainValid == totalGainValid)) return false;
	

	
		if (!(this->totalFit == totalFit)) return false;
	

	
		if (!(this->totalFitWeight == totalFitWeight)) return false;
	

		return result;
	}
	
	
	/**
	 * Return true if all required attributes of the value part are equal to their homologues
	 * in x and false otherwise.
	 *

	 * @param x a pointer on the CalGainRow whose required attributes of the value part 

	 * will be compared with those of this.
	 * @return a boolean.
	 */
	bool CalGainRow::equalByRequiredValue(CalGainRow*  x ) {
		
			
		if (this->startValidTime != x->startValidTime) return false;
			
		if (this->endValidTime != x->endValidTime) return false;
			
		if (this->gain != x->gain) return false;
			
		if (this->gainValid != x->gainValid) return false;
			
		if (this->fit != x->fit) return false;
			
		if (this->fitWeight != x->fitWeight) return false;
			
		if (this->totalGainValid != x->totalGainValid) return false;
			
		if (this->totalFit != x->totalFit) return false;
			
		if (this->totalFitWeight != x->totalFitWeight) return false;
			
		
		return true;
	}	
	
/*
	 map<string, CalGainAttributeFromBin> CalGainRow::initFromBinMethods() {
		map<string, CalGainAttributeFromBin> result;
		
		result["calDataId"] = &CalGainRow::calDataIdFromBin;
		result["calReductionId"] = &CalGainRow::calReductionIdFromBin;
		result["startValidTime"] = &CalGainRow::startValidTimeFromBin;
		result["endValidTime"] = &CalGainRow::endValidTimeFromBin;
		result["gain"] = &CalGainRow::gainFromBin;
		result["gainValid"] = &CalGainRow::gainValidFromBin;
		result["fit"] = &CalGainRow::fitFromBin;
		result["fitWeight"] = &CalGainRow::fitWeightFromBin;
		result["totalGainValid"] = &CalGainRow::totalGainValidFromBin;
		result["totalFit"] = &CalGainRow::totalFitFromBin;
		result["totalFitWeight"] = &CalGainRow::totalFitWeightFromBin;
		
		
			
		
		return result;	
	}
*/	
} // End namespace asdm
 
