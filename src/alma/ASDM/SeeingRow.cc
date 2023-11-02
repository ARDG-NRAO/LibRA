
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
 * File SeeingRow.cpp
 */
 
#include <vector>
#include <set>

#include <alma/ASDM/ASDM.h>
#include <alma/ASDM/SeeingRow.h>
#include <alma/ASDM/SeeingTable.h>
	

using asdm::ASDM;
using asdm::SeeingRow;
using asdm::SeeingTable;


#include <alma/ASDM/Parser.h>

#include <alma/ASDM/EnumerationParser.h>
#include <alma/ASDM/ASDMValuesParser.h>
 
#include <alma/ASDM/InvalidArgumentException.h>

using namespace std;

namespace asdm {
	SeeingRow::~SeeingRow() {
	}

	/**
	 * Return the table to which this row belongs.
	 */
	SeeingTable &SeeingRow::getTable() const {
		return table;
	}

	bool SeeingRow::isAdded() const {
		return hasBeenAdded;
	}	

	void SeeingRow::isAdded(bool added) {
		hasBeenAdded = added;
	}
	
#ifndef WITHOUT_ACS
	using asdmIDL::SeeingRowIDL;
#endif
	
#ifndef WITHOUT_ACS
	/**
	 * Return this row in the form of an IDL struct.
	 * @return The values of this row as a SeeingRowIDL struct.
	 */
	SeeingRowIDL *SeeingRow::toIDL() const {
		SeeingRowIDL *x = new SeeingRowIDL ();
		
		// Fill the IDL structure.
	
		
	
  		
		
		
			
		x->timeInterval = timeInterval.toIDLArrayTimeInterval();
			
		
	

	
  		
		
		
			
				
		x->numBaseLength = numBaseLength;
 				
 			
		
	

	
  		
		
		
			
		x->baseLength.length(baseLength.size());
		for (unsigned int i = 0; i < baseLength.size(); ++i) {
			
			x->baseLength[i] = baseLength.at(i).toIDLLength();
			
	 	}
			
		
	

	
  		
		
		
			
		x->phaseRms.length(phaseRms.size());
		for (unsigned int i = 0; i < phaseRms.size(); ++i) {
			
			x->phaseRms[i] = phaseRms.at(i).toIDLAngle();
			
	 	}
			
		
	

	
  		
		
		
			
				
		x->seeing = seeing;
 				
 			
		
	

	
  		
		
		
			
				
		x->exponent = exponent;
 				
 			
		
	

	
	
		
		
		return x;
	
	}
	
	void SeeingRow::toIDL(asdmIDL::SeeingRowIDL& x) const {
		// Set the x's fields.
	
		
	
  		
		
		
			
		x.timeInterval = timeInterval.toIDLArrayTimeInterval();
			
		
	

	
  		
		
		
			
				
		x.numBaseLength = numBaseLength;
 				
 			
		
	

	
  		
		
		
			
		x.baseLength.length(baseLength.size());
		for (unsigned int i = 0; i < baseLength.size(); ++i) {
			
			x.baseLength[i] = baseLength.at(i).toIDLLength();
			
	 	}
			
		
	

	
  		
		
		
			
		x.phaseRms.length(phaseRms.size());
		for (unsigned int i = 0; i < phaseRms.size(); ++i) {
			
			x.phaseRms[i] = phaseRms.at(i).toIDLAngle();
			
	 	}
			
		
	

	
  		
		
		
			
				
		x.seeing = seeing;
 				
 			
		
	

	
  		
		
		
			
				
		x.exponent = exponent;
 				
 			
		
	

	
	
		
	
	}
#endif
	

#ifndef WITHOUT_ACS
	/**
	 * Fill the values of this row from the IDL struct SeeingRowIDL.
	 * @param x The IDL struct containing the values used to fill this row.
	 */
	void SeeingRow::setFromIDL (SeeingRowIDL x){
		try {
		// Fill the values from x.
	
		
	
		
		
			
		setTimeInterval(ArrayTimeInterval (x.timeInterval));
			
 		
		
	

	
		
		
			
		setNumBaseLength(x.numBaseLength);
  			
 		
		
	

	
		
		
			
		baseLength .clear();
		for (unsigned int i = 0; i <x.baseLength.length(); ++i) {
			
			baseLength.push_back(Length (x.baseLength[i]));
			
		}
			
  		
		
	

	
		
		
			
		phaseRms .clear();
		for (unsigned int i = 0; i <x.phaseRms.length(); ++i) {
			
			phaseRms.push_back(Angle (x.phaseRms[i]));
			
		}
			
  		
		
	

	
		
		
			
		setSeeing(x.seeing);
  			
 		
		
	

	
		
		
			
		setExponent(x.exponent);
  			
 		
		
	

	
	
		
		} catch (const IllegalAccessException &err) {
			throw ConversionException (err.getMessage(),"Seeing");
		}
	}
#endif
	
	/**
	 * Return this row in the form of an XML string.
	 * @return The values of this row as an XML string.
	 */
	string SeeingRow::toXML() const {
		string buf;
		buf.append("<row> \n");
		
	
		
  	
 		
		
		Parser::toXML(timeInterval, "timeInterval", buf);
		
		
	

  	
 		
		
		Parser::toXML(numBaseLength, "numBaseLength", buf);
		
		
	

  	
 		
		
		Parser::toXML(baseLength, "baseLength", buf);
		
		
	

  	
 		
		
		Parser::toXML(phaseRms, "phaseRms", buf);
		
		
	

  	
 		
		
		Parser::toXML(seeing, "seeing", buf);
		
		
	

  	
 		
		
		Parser::toXML(exponent, "exponent", buf);
		
		
	

	
	
		
		
		buf.append("</row>\n");
		return buf;
	}

	/**
	 * Fill the values of this row from an XML string 
	 * that was produced by the toXML() method.
	 * @param x The XML string being used to set the values of this row.
	 */
	void SeeingRow::setFromXML (string rowDoc) {
		Parser row(rowDoc);
		string s = "";
		try {
	
		
	
  		
			
	  	setTimeInterval(Parser::getArrayTimeInterval("timeInterval","Seeing",rowDoc));
			
		
	

	
  		
			
	  	setNumBaseLength(Parser::getInteger("numBaseLength","Seeing",rowDoc));
			
		
	

	
  		
			
					
	  	setBaseLength(Parser::get1DLength("baseLength","Seeing",rowDoc));
	  			
	  		
		
	

	
  		
			
					
	  	setPhaseRms(Parser::get1DAngle("phaseRms","Seeing",rowDoc));
	  			
	  		
		
	

	
  		
			
	  	setSeeing(Parser::getFloat("seeing","Seeing",rowDoc));
			
		
	

	
  		
			
	  	setExponent(Parser::getFloat("exponent","Seeing",rowDoc));
			
		
	

	
	
		
		} catch (const IllegalAccessException &err) {
			throw ConversionException (err.getMessage(),"Seeing");
		}
	}
	
	void SeeingRow::toBin(EndianOSStream& eoss) {
	
	
	
	
		
	timeInterval.toBin(eoss);
		
	

	
	
		
						
			eoss.writeInt(numBaseLength);
				
		
	

	
	
		
	Length::toBin(baseLength, eoss);
		
	

	
	
		
	Angle::toBin(phaseRms, eoss);
		
	

	
	
		
						
			eoss.writeFloat(seeing);
				
		
	

	
	
		
						
			eoss.writeFloat(exponent);
				
		
	


	
	
	}
	
void SeeingRow::timeIntervalFromBin(EndianIStream& eis) {
		
	
		
		
		timeInterval =  ArrayTimeInterval::fromBin(eis);
		
	
	
}
void SeeingRow::numBaseLengthFromBin(EndianIStream& eis) {
		
	
	
		
			
		numBaseLength =  eis.readInt();
			
		
	
	
}
void SeeingRow::baseLengthFromBin(EndianIStream& eis) {
		
	
		
		
			
	
	baseLength = Length::from1DBin(eis);	
	

		
	
	
}
void SeeingRow::phaseRmsFromBin(EndianIStream& eis) {
		
	
		
		
			
	
	phaseRms = Angle::from1DBin(eis);	
	

		
	
	
}
void SeeingRow::seeingFromBin(EndianIStream& eis) {
		
	
	
		
			
		seeing =  eis.readFloat();
			
		
	
	
}
void SeeingRow::exponentFromBin(EndianIStream& eis) {
		
	
	
		
			
		exponent =  eis.readFloat();
			
		
	
	
}

		
	
	SeeingRow* SeeingRow::fromBin(EndianIStream& eis, SeeingTable& table, const vector<string>& attributesSeq) {
		SeeingRow* row = new  SeeingRow(table);
		
		map<string, SeeingAttributeFromBin>::iterator iter ;
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
					throw ConversionException("There is not method to read an attribute '"+attributesSeq.at(i)+"'.", "SeeingTable");
			}
				
		}				
		return row;
	}

	//
	// A collection of methods to set the value of the attributes from their textual value in the XML representation
	// of one row.
	//
	
	// Convert a string into an ArrayTimeInterval 
	void SeeingRow::timeIntervalFromText(const string & s) {
		 
          
		timeInterval = ASDMValuesParser::parse<ArrayTimeInterval>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void SeeingRow::numBaseLengthFromText(const string & s) {
		 
          
		numBaseLength = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	// Convert a string into an Length 
	void SeeingRow::baseLengthFromText(const string & s) {
		 
          
		baseLength = ASDMValuesParser::parse1D<Length>(s);
          
		
	}
	
	
	// Convert a string into an Angle 
	void SeeingRow::phaseRmsFromText(const string & s) {
		 
          
		phaseRms = ASDMValuesParser::parse1D<Angle>(s);
          
		
	}
	
	
	// Convert a string into an float 
	void SeeingRow::seeingFromText(const string & s) {
		 
          
		seeing = ASDMValuesParser::parse<float>(s);
          
		
	}
	
	
	// Convert a string into an float 
	void SeeingRow::exponentFromText(const string & s) {
		 
          
		exponent = ASDMValuesParser::parse<float>(s);
          
		
	}
	

		
	
	void SeeingRow::fromText(const std::string& attributeName, const std::string&  t) {
		map<string, SeeingAttributeFromText>::iterator iter;
		if ((iter = fromTextMethods.find(attributeName)) == fromTextMethods.end())
			throw ConversionException("I do not know what to do with '"+attributeName+"' and its content '"+t+"' (while parsing an XML document)", "SeeingTable");
		(this->*(iter->second))(t);
	}
			
	////////////////////////////////////////////////
	// Intrinsic Table Attributes getters/setters //
	////////////////////////////////////////////////
	
	

	
 	/**
 	 * Get timeInterval.
 	 * @return timeInterval as ArrayTimeInterval
 	 */
 	ArrayTimeInterval SeeingRow::getTimeInterval() const {
	
  		return timeInterval;
 	}

 	/**
 	 * Set timeInterval with the specified ArrayTimeInterval.
 	 * @param timeInterval The ArrayTimeInterval value to which timeInterval is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void SeeingRow::setTimeInterval (ArrayTimeInterval timeInterval)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("timeInterval", "Seeing");
		
  		}
  	
 		this->timeInterval = timeInterval;
	
 	}
	
	

	

	
 	/**
 	 * Get numBaseLength.
 	 * @return numBaseLength as int
 	 */
 	int SeeingRow::getNumBaseLength() const {
	
  		return numBaseLength;
 	}

 	/**
 	 * Set numBaseLength with the specified int.
 	 * @param numBaseLength The int value to which numBaseLength is to be set.
 	 
 	
 		
 	 */
 	void SeeingRow::setNumBaseLength (int numBaseLength)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->numBaseLength = numBaseLength;
	
 	}
	
	

	

	
 	/**
 	 * Get baseLength.
 	 * @return baseLength as std::vector<Length >
 	 */
 	std::vector<Length > SeeingRow::getBaseLength() const {
	
  		return baseLength;
 	}

 	/**
 	 * Set baseLength with the specified std::vector<Length >.
 	 * @param baseLength The std::vector<Length > value to which baseLength is to be set.
 	 
 	
 		
 	 */
 	void SeeingRow::setBaseLength (std::vector<Length > baseLength)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->baseLength = baseLength;
	
 	}
	
	

	

	
 	/**
 	 * Get phaseRms.
 	 * @return phaseRms as std::vector<Angle >
 	 */
 	std::vector<Angle > SeeingRow::getPhaseRms() const {
	
  		return phaseRms;
 	}

 	/**
 	 * Set phaseRms with the specified std::vector<Angle >.
 	 * @param phaseRms The std::vector<Angle > value to which phaseRms is to be set.
 	 
 	
 		
 	 */
 	void SeeingRow::setPhaseRms (std::vector<Angle > phaseRms)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->phaseRms = phaseRms;
	
 	}
	
	

	

	
 	/**
 	 * Get seeing.
 	 * @return seeing as float
 	 */
 	float SeeingRow::getSeeing() const {
	
  		return seeing;
 	}

 	/**
 	 * Set seeing with the specified float.
 	 * @param seeing The float value to which seeing is to be set.
 	 
 	
 		
 	 */
 	void SeeingRow::setSeeing (float seeing)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->seeing = seeing;
	
 	}
	
	

	

	
 	/**
 	 * Get exponent.
 	 * @return exponent as float
 	 */
 	float SeeingRow::getExponent() const {
	
  		return exponent;
 	}

 	/**
 	 * Set exponent with the specified float.
 	 * @param exponent The float value to which exponent is to be set.
 	 
 	
 		
 	 */
 	void SeeingRow::setExponent (float exponent)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->exponent = exponent;
	
 	}
	
	

	
	///////////////////////////////////////////////
	// Extrinsic Table Attributes getters/setters//
	///////////////////////////////////////////////
	

	//////////////////////////////////////
	// Links Attributes getters/setters //
	//////////////////////////////////////
	
	
	/**
	 * Create a SeeingRow.
	 * <p>
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 */ 
	SeeingRow::SeeingRow (SeeingTable &t) : table(t) {
		hasBeenAdded = false;
		
	
	

	

	

	

	

	

	
	
	
	
	

	

	

	

	

	

	
	
	 fromBinMethods["timeInterval"] = &SeeingRow::timeIntervalFromBin; 
	 fromBinMethods["numBaseLength"] = &SeeingRow::numBaseLengthFromBin; 
	 fromBinMethods["baseLength"] = &SeeingRow::baseLengthFromBin; 
	 fromBinMethods["phaseRms"] = &SeeingRow::phaseRmsFromBin; 
	 fromBinMethods["seeing"] = &SeeingRow::seeingFromBin; 
	 fromBinMethods["exponent"] = &SeeingRow::exponentFromBin; 
		
	
	
	
	
	
				 
	fromTextMethods["timeInterval"] = &SeeingRow::timeIntervalFromText;
		 
	
				 
	fromTextMethods["numBaseLength"] = &SeeingRow::numBaseLengthFromText;
		 
	
				 
	fromTextMethods["baseLength"] = &SeeingRow::baseLengthFromText;
		 
	
				 
	fromTextMethods["phaseRms"] = &SeeingRow::phaseRmsFromText;
		 
	
				 
	fromTextMethods["seeing"] = &SeeingRow::seeingFromText;
		 
	
				 
	fromTextMethods["exponent"] = &SeeingRow::exponentFromText;
		 
	

		
	}
	
	SeeingRow::SeeingRow (SeeingTable &t, SeeingRow *row) : table(t) {
		hasBeenAdded = false;
		
		if (row == 0) {
	
	
	

	

	

	

	

	

			
		}
		else {
	
		
			timeInterval = row->timeInterval;
		
		
		
		
			numBaseLength = row->numBaseLength;
		
			baseLength = row->baseLength;
		
			phaseRms = row->phaseRms;
		
			seeing = row->seeing;
		
			exponent = row->exponent;
		
		
		
		
		}
		
		 fromBinMethods["timeInterval"] = &SeeingRow::timeIntervalFromBin; 
		 fromBinMethods["numBaseLength"] = &SeeingRow::numBaseLengthFromBin; 
		 fromBinMethods["baseLength"] = &SeeingRow::baseLengthFromBin; 
		 fromBinMethods["phaseRms"] = &SeeingRow::phaseRmsFromBin; 
		 fromBinMethods["seeing"] = &SeeingRow::seeingFromBin; 
		 fromBinMethods["exponent"] = &SeeingRow::exponentFromBin; 
			
	
			
	}

	
	bool SeeingRow::compareNoAutoInc(ArrayTimeInterval timeInterval, int numBaseLength, std::vector<Length > baseLength, std::vector<Angle > phaseRms, float seeing, float exponent) {
		bool result;
		result = true;
		
	
		
		result = result && (this->timeInterval.overlaps(timeInterval));
		
		if (!result) return false;
	

	
		
		result = result && (this->numBaseLength == numBaseLength);
		
		if (!result) return false;
	

	
		
		result = result && (this->baseLength == baseLength);
		
		if (!result) return false;
	

	
		
		result = result && (this->phaseRms == phaseRms);
		
		if (!result) return false;
	

	
		
		result = result && (this->seeing == seeing);
		
		if (!result) return false;
	

	
		
		result = result && (this->exponent == exponent);
		
		if (!result) return false;
	

		return result;
	}	
	
	
	
	bool SeeingRow::compareRequiredValue(int numBaseLength, std::vector<Length > baseLength, std::vector<Angle > phaseRms, float seeing, float exponent) {
		bool result;
		result = true;
		
	
		if (!(this->numBaseLength == numBaseLength)) return false;
	

	
		if (!(this->baseLength == baseLength)) return false;
	

	
		if (!(this->phaseRms == phaseRms)) return false;
	

	
		if (!(this->seeing == seeing)) return false;
	

	
		if (!(this->exponent == exponent)) return false;
	

		return result;
	}
	
	
	/**
	 * Return true if all required attributes of the value part are equal to their homologues
	 * in x and false otherwise.
	 *

	 * @param x a pointer on the SeeingRow whose required attributes of the value part 

	 * will be compared with those of this.
	 * @return a boolean.
	 */
	bool SeeingRow::equalByRequiredValue(SeeingRow*  x ) {
		
			
		if (this->numBaseLength != x->numBaseLength) return false;
			
		if (this->baseLength != x->baseLength) return false;
			
		if (this->phaseRms != x->phaseRms) return false;
			
		if (this->seeing != x->seeing) return false;
			
		if (this->exponent != x->exponent) return false;
			
		
		return true;
	}	
	
/*
	 map<string, SeeingAttributeFromBin> SeeingRow::initFromBinMethods() {
		map<string, SeeingAttributeFromBin> result;
		
		result["timeInterval"] = &SeeingRow::timeIntervalFromBin;
		result["numBaseLength"] = &SeeingRow::numBaseLengthFromBin;
		result["baseLength"] = &SeeingRow::baseLengthFromBin;
		result["phaseRms"] = &SeeingRow::phaseRmsFromBin;
		result["seeing"] = &SeeingRow::seeingFromBin;
		result["exponent"] = &SeeingRow::exponentFromBin;
		
		
			
		
		return result;	
	}
*/	
} // End namespace asdm
 
