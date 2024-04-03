
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
 * File SwitchCycleRow.cpp
 */
 
#include <vector>
#include <set>

#include <alma/ASDM/ASDM.h>
#include <alma/ASDM/SwitchCycleRow.h>
#include <alma/ASDM/SwitchCycleTable.h>
	

using asdm::ASDM;
using asdm::SwitchCycleRow;
using asdm::SwitchCycleTable;


#include <alma/ASDM/Parser.h>

#include <alma/ASDM/EnumerationParser.h>
#include <alma/ASDM/ASDMValuesParser.h>
 
#include <alma/ASDM/InvalidArgumentException.h>

using namespace std;

namespace asdm {
	SwitchCycleRow::~SwitchCycleRow() {
	}

	/**
	 * Return the table to which this row belongs.
	 */
	SwitchCycleTable &SwitchCycleRow::getTable() const {
		return table;
	}

	bool SwitchCycleRow::isAdded() const {
		return hasBeenAdded;
	}	

	void SwitchCycleRow::isAdded(bool added) {
		hasBeenAdded = added;
	}
	
#ifndef WITHOUT_ACS
	using asdmIDL::SwitchCycleRowIDL;
#endif
	
#ifndef WITHOUT_ACS
	/**
	 * Return this row in the form of an IDL struct.
	 * @return The values of this row as a SwitchCycleRowIDL struct.
	 */
	SwitchCycleRowIDL *SwitchCycleRow::toIDL() const {
		SwitchCycleRowIDL *x = new SwitchCycleRowIDL ();
		
		// Fill the IDL structure.
	
		
	
  		
		
		
			
		x->switchCycleId = switchCycleId.toIDLTag();
			
		
	

	
  		
		
		
			
				
		x->numStep = numStep;
 				
 			
		
	

	
  		
		
		
			
		x->weightArray.length(weightArray.size());
		for (unsigned int i = 0; i < weightArray.size(); ++i) {
			
				
			x->weightArray[i] = weightArray.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		
			
		x->dirOffsetArray.length(dirOffsetArray.size());
		for (unsigned int i = 0; i < dirOffsetArray.size(); i++) {
			x->dirOffsetArray[i].length(dirOffsetArray.at(i).size());			 		
		}
		
		for (unsigned int i = 0; i < dirOffsetArray.size() ; i++)
			for (unsigned int j = 0; j < dirOffsetArray.at(i).size(); j++)
					
				x->dirOffsetArray[i][j]= dirOffsetArray.at(i).at(j).toIDLAngle();
									
		
			
		
	

	
  		
		
		
			
		x->freqOffsetArray.length(freqOffsetArray.size());
		for (unsigned int i = 0; i < freqOffsetArray.size(); ++i) {
			
			x->freqOffsetArray[i] = freqOffsetArray.at(i).toIDLFrequency();
			
	 	}
			
		
	

	
  		
		
		
			
		x->stepDurationArray.length(stepDurationArray.size());
		for (unsigned int i = 0; i < stepDurationArray.size(); ++i) {
			
			x->stepDurationArray[i] = stepDurationArray.at(i).toIDLInterval();
			
	 	}
			
		
	

	
  		
		
		x->directionCodeExists = directionCodeExists;
		
		
			
				
		x->directionCode = directionCode;
 				
 			
		
	

	
  		
		
		x->directionEquinoxExists = directionEquinoxExists;
		
		
			
		x->directionEquinox = directionEquinox.toIDLArrayTime();
			
		
	

	
	
		
		
		return x;
	
	}
	
	void SwitchCycleRow::toIDL(asdmIDL::SwitchCycleRowIDL& x) const {
		// Set the x's fields.
	
		
	
  		
		
		
			
		x.switchCycleId = switchCycleId.toIDLTag();
			
		
	

	
  		
		
		
			
				
		x.numStep = numStep;
 				
 			
		
	

	
  		
		
		
			
		x.weightArray.length(weightArray.size());
		for (unsigned int i = 0; i < weightArray.size(); ++i) {
			
				
			x.weightArray[i] = weightArray.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		
			
		x.dirOffsetArray.length(dirOffsetArray.size());
		for (unsigned int i = 0; i < dirOffsetArray.size(); i++) {
			x.dirOffsetArray[i].length(dirOffsetArray.at(i).size());			 		
		}
		
		for (unsigned int i = 0; i < dirOffsetArray.size() ; i++)
			for (unsigned int j = 0; j < dirOffsetArray.at(i).size(); j++)
					
				x.dirOffsetArray[i][j]= dirOffsetArray.at(i).at(j).toIDLAngle();
									
		
			
		
	

	
  		
		
		
			
		x.freqOffsetArray.length(freqOffsetArray.size());
		for (unsigned int i = 0; i < freqOffsetArray.size(); ++i) {
			
			x.freqOffsetArray[i] = freqOffsetArray.at(i).toIDLFrequency();
			
	 	}
			
		
	

	
  		
		
		
			
		x.stepDurationArray.length(stepDurationArray.size());
		for (unsigned int i = 0; i < stepDurationArray.size(); ++i) {
			
			x.stepDurationArray[i] = stepDurationArray.at(i).toIDLInterval();
			
	 	}
			
		
	

	
  		
		
		x.directionCodeExists = directionCodeExists;
		
		
			
				
		x.directionCode = directionCode;
 				
 			
		
	

	
  		
		
		x.directionEquinoxExists = directionEquinoxExists;
		
		
			
		x.directionEquinox = directionEquinox.toIDLArrayTime();
			
		
	

	
	
		
	
	}
#endif
	

#ifndef WITHOUT_ACS
	/**
	 * Fill the values of this row from the IDL struct SwitchCycleRowIDL.
	 * @param x The IDL struct containing the values used to fill this row.
	 */
	void SwitchCycleRow::setFromIDL (SwitchCycleRowIDL x){
		try {
		// Fill the values from x.
	
		
	
		
		
			
		setSwitchCycleId(Tag (x.switchCycleId));
			
 		
		
	

	
		
		
			
		setNumStep(x.numStep);
  			
 		
		
	

	
		
		
			
		weightArray .clear();
		for (unsigned int i = 0; i <x.weightArray.length(); ++i) {
			
			weightArray.push_back(x.weightArray[i]);
  			
		}
			
  		
		
	

	
		
		
			
		dirOffsetArray .clear();
        
        vector<Angle> v_aux_dirOffsetArray;
        
		for (unsigned int i = 0; i < x.dirOffsetArray.length(); ++i) {
			v_aux_dirOffsetArray.clear();
			for (unsigned int j = 0; j < x.dirOffsetArray[0].length(); ++j) {
				
				v_aux_dirOffsetArray.push_back(Angle (x.dirOffsetArray[i][j]));
				
  			}
  			dirOffsetArray.push_back(v_aux_dirOffsetArray);			
		}
			
  		
		
	

	
		
		
			
		freqOffsetArray .clear();
		for (unsigned int i = 0; i <x.freqOffsetArray.length(); ++i) {
			
			freqOffsetArray.push_back(Frequency (x.freqOffsetArray[i]));
			
		}
			
  		
		
	

	
		
		
			
		stepDurationArray .clear();
		for (unsigned int i = 0; i <x.stepDurationArray.length(); ++i) {
			
			stepDurationArray.push_back(Interval (x.stepDurationArray[i]));
			
		}
			
  		
		
	

	
		
		directionCodeExists = x.directionCodeExists;
		if (x.directionCodeExists) {
		
		
			
		setDirectionCode(x.directionCode);
  			
 		
		
		}
		
	

	
		
		directionEquinoxExists = x.directionEquinoxExists;
		if (x.directionEquinoxExists) {
		
		
			
		setDirectionEquinox(ArrayTime (x.directionEquinox));
			
 		
		
		}
		
	

	
	
		
		} catch (const IllegalAccessException &err) {
			throw ConversionException (err.getMessage(),"SwitchCycle");
		}
	}
#endif
	
	/**
	 * Return this row in the form of an XML string.
	 * @return The values of this row as an XML string.
	 */
	string SwitchCycleRow::toXML() const {
		string buf;
		buf.append("<row> \n");
		
	
		
  	
 		
		
		Parser::toXML(switchCycleId, "switchCycleId", buf);
		
		
	

  	
 		
		
		Parser::toXML(numStep, "numStep", buf);
		
		
	

  	
 		
		
		Parser::toXML(weightArray, "weightArray", buf);
		
		
	

  	
 		
		
		Parser::toXML(dirOffsetArray, "dirOffsetArray", buf);
		
		
	

  	
 		
		
		Parser::toXML(freqOffsetArray, "freqOffsetArray", buf);
		
		
	

  	
 		
		
		Parser::toXML(stepDurationArray, "stepDurationArray", buf);
		
		
	

  	
 		
		if (directionCodeExists) {
		
		
			buf.append(EnumerationParser::toXML("directionCode", directionCode));
		
		
		}
		
	

  	
 		
		if (directionEquinoxExists) {
		
		
		Parser::toXML(directionEquinox, "directionEquinox", buf);
		
		
		}
		
	

	
	
		
		
		buf.append("</row>\n");
		return buf;
	}

	/**
	 * Fill the values of this row from an XML string 
	 * that was produced by the toXML() method.
	 * @param x The XML string being used to set the values of this row.
	 */
	void SwitchCycleRow::setFromXML (string rowDoc) {
		Parser row(rowDoc);
		string s = "";
		try {
	
		
	
  		
			
	  	setSwitchCycleId(Parser::getTag("switchCycleId","SwitchCycle",rowDoc));
			
		
	

	
  		
			
	  	setNumStep(Parser::getInteger("numStep","SwitchCycle",rowDoc));
			
		
	

	
  		
			
					
	  	setWeightArray(Parser::get1DFloat("weightArray","SwitchCycle",rowDoc));
	  			
	  		
		
	

	
  		
			
					
	  	setDirOffsetArray(Parser::get2DAngle("dirOffsetArray","SwitchCycle",rowDoc));
	  			
	  		
		
	

	
  		
			
					
	  	setFreqOffsetArray(Parser::get1DFrequency("freqOffsetArray","SwitchCycle",rowDoc));
	  			
	  		
		
	

	
  		
			
					
	  	setStepDurationArray(Parser::get1DInterval("stepDurationArray","SwitchCycle",rowDoc));
	  			
	  		
		
	

	
		
	if (row.isStr("<directionCode>")) {
		
		
		
		directionCode = EnumerationParser::getDirectionReferenceCode("directionCode","SwitchCycle",rowDoc);
		
		
		
		directionCodeExists = true;
	}
		
	

	
  		
        if (row.isStr("<directionEquinox>")) {
			
	  		setDirectionEquinox(Parser::getArrayTime("directionEquinox","SwitchCycle",rowDoc));
			
		}
 		
	

	
	
		
		} catch (const IllegalAccessException &err) {
			throw ConversionException (err.getMessage(),"SwitchCycle");
		}
	}
	
	void SwitchCycleRow::toBin(EndianOSStream& eoss) {
	
	
	
	
		
	switchCycleId.toBin(eoss);
		
	

	
	
		
						
			eoss.writeInt(numStep);
				
		
	

	
	
		
		
			
		eoss.writeInt((int) weightArray.size());
		for (unsigned int i = 0; i < weightArray.size(); i++)
				
			eoss.writeFloat(weightArray.at(i));
				
				
						
		
	

	
	
		
	Angle::toBin(dirOffsetArray, eoss);
		
	

	
	
		
	Frequency::toBin(freqOffsetArray, eoss);
		
	

	
	
		
	Interval::toBin(stepDurationArray, eoss);
		
	


	
	
	eoss.writeBoolean(directionCodeExists);
	if (directionCodeExists) {
	
	
	
		
					
		eoss.writeString(CDirectionReferenceCode::name(directionCode));
			/* eoss.writeInt(directionCode); */
				
		
	

	}

	eoss.writeBoolean(directionEquinoxExists);
	if (directionEquinoxExists) {
	
	
	
		
	directionEquinox.toBin(eoss);
		
	

	}

	}
	
void SwitchCycleRow::switchCycleIdFromBin(EndianIStream& eis) {
		
	
		
		
		switchCycleId =  Tag::fromBin(eis);
		
	
	
}
void SwitchCycleRow::numStepFromBin(EndianIStream& eis) {
		
	
	
		
			
		numStep =  eis.readInt();
			
		
	
	
}
void SwitchCycleRow::weightArrayFromBin(EndianIStream& eis) {
		
	
	
		
			
	
		weightArray.clear();
		
		unsigned int weightArrayDim1 = eis.readInt();
		for (unsigned int  i = 0 ; i < weightArrayDim1; i++)
			
			weightArray.push_back(eis.readFloat());
			
	

		
	
	
}
void SwitchCycleRow::dirOffsetArrayFromBin(EndianIStream& eis) {
		
	
		
		
			
	
	dirOffsetArray = Angle::from2DBin(eis);		
	

		
	
	
}
void SwitchCycleRow::freqOffsetArrayFromBin(EndianIStream& eis) {
		
	
		
		
			
	
	freqOffsetArray = Frequency::from1DBin(eis);	
	

		
	
	
}
void SwitchCycleRow::stepDurationArrayFromBin(EndianIStream& eis) {
		
	
		
		
			
	
	stepDurationArray = Interval::from1DBin(eis);	
	

		
	
	
}

void SwitchCycleRow::directionCodeFromBin(EndianIStream& eis) {
		
	directionCodeExists = eis.readBoolean();
	if (directionCodeExists) {
		
	
	
		
			
		directionCode = CDirectionReferenceCode::literal(eis.readString());
			
		
	

	}
	
}
void SwitchCycleRow::directionEquinoxFromBin(EndianIStream& eis) {
		
	directionEquinoxExists = eis.readBoolean();
	if (directionEquinoxExists) {
		
	
		
		
		directionEquinox =  ArrayTime::fromBin(eis);
		
	

	}
	
}
	
	
	SwitchCycleRow* SwitchCycleRow::fromBin(EndianIStream& eis, SwitchCycleTable& table, const vector<string>& attributesSeq) {
		SwitchCycleRow* row = new  SwitchCycleRow(table);
		
		map<string, SwitchCycleAttributeFromBin>::iterator iter ;
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
					throw ConversionException("There is not method to read an attribute '"+attributesSeq.at(i)+"'.", "SwitchCycleTable");
			}
				
		}				
		return row;
	}

	//
	// A collection of methods to set the value of the attributes from their textual value in the XML representation
	// of one row.
	//
	
	// Convert a string into an Tag 
	void SwitchCycleRow::switchCycleIdFromText(const string & s) {
		 
          
		switchCycleId = ASDMValuesParser::parse<Tag>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void SwitchCycleRow::numStepFromText(const string & s) {
		 
          
		numStep = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	// Convert a string into an float 
	void SwitchCycleRow::weightArrayFromText(const string & s) {
		 
          
		weightArray = ASDMValuesParser::parse1D<float>(s);
          
		
	}
	
	
	// Convert a string into an Angle 
	void SwitchCycleRow::dirOffsetArrayFromText(const string & s) {
		 
          
		dirOffsetArray = ASDMValuesParser::parse2D<Angle>(s);
          
		
	}
	
	
	// Convert a string into an Frequency 
	void SwitchCycleRow::freqOffsetArrayFromText(const string & s) {
		 
          
		freqOffsetArray = ASDMValuesParser::parse1D<Frequency>(s);
          
		
	}
	
	
	// Convert a string into an Interval 
	void SwitchCycleRow::stepDurationArrayFromText(const string & s) {
		 
          
		stepDurationArray = ASDMValuesParser::parse1D<Interval>(s);
          
		
	}
	

	
	// Convert a string into an DirectionReferenceCode 
	void SwitchCycleRow::directionCodeFromText(const string & s) {
		directionCodeExists = true;
		 
          
		directionCode = ASDMValuesParser::parse<DirectionReferenceCodeMod::DirectionReferenceCode>(s);
          
		
	}
	
	
	// Convert a string into an ArrayTime 
	void SwitchCycleRow::directionEquinoxFromText(const string & s) {
		directionEquinoxExists = true;
		 
          
		directionEquinox = ASDMValuesParser::parse<ArrayTime>(s);
          
		
	}
	
	
	
	void SwitchCycleRow::fromText(const std::string& attributeName, const std::string&  t) {
		map<string, SwitchCycleAttributeFromText>::iterator iter;
		if ((iter = fromTextMethods.find(attributeName)) == fromTextMethods.end())
			throw ConversionException("I do not know what to do with '"+attributeName+"' and its content '"+t+"' (while parsing an XML document)", "SwitchCycleTable");
		(this->*(iter->second))(t);
	}
			
	////////////////////////////////////////////////
	// Intrinsic Table Attributes getters/setters //
	////////////////////////////////////////////////
	
	

	
 	/**
 	 * Get switchCycleId.
 	 * @return switchCycleId as Tag
 	 */
 	Tag SwitchCycleRow::getSwitchCycleId() const {
	
  		return switchCycleId;
 	}

 	/**
 	 * Set switchCycleId with the specified Tag.
 	 * @param switchCycleId The Tag value to which switchCycleId is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void SwitchCycleRow::setSwitchCycleId (Tag switchCycleId)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("switchCycleId", "SwitchCycle");
		
  		}
  	
 		this->switchCycleId = switchCycleId;
	
 	}
	
	

	

	
 	/**
 	 * Get numStep.
 	 * @return numStep as int
 	 */
 	int SwitchCycleRow::getNumStep() const {
	
  		return numStep;
 	}

 	/**
 	 * Set numStep with the specified int.
 	 * @param numStep The int value to which numStep is to be set.
 	 
 	
 		
 	 */
 	void SwitchCycleRow::setNumStep (int numStep)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->numStep = numStep;
	
 	}
	
	

	

	
 	/**
 	 * Get weightArray.
 	 * @return weightArray as std::vector<float >
 	 */
 	std::vector<float > SwitchCycleRow::getWeightArray() const {
	
  		return weightArray;
 	}

 	/**
 	 * Set weightArray with the specified std::vector<float >.
 	 * @param weightArray The std::vector<float > value to which weightArray is to be set.
 	 
 	
 		
 	 */
 	void SwitchCycleRow::setWeightArray (std::vector<float > weightArray)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->weightArray = weightArray;
	
 	}
	
	

	

	
 	/**
 	 * Get dirOffsetArray.
 	 * @return dirOffsetArray as std::vector<std::vector<Angle > >
 	 */
 	std::vector<std::vector<Angle > > SwitchCycleRow::getDirOffsetArray() const {
	
  		return dirOffsetArray;
 	}

 	/**
 	 * Set dirOffsetArray with the specified std::vector<std::vector<Angle > >.
 	 * @param dirOffsetArray The std::vector<std::vector<Angle > > value to which dirOffsetArray is to be set.
 	 
 	
 		
 	 */
 	void SwitchCycleRow::setDirOffsetArray (std::vector<std::vector<Angle > > dirOffsetArray)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->dirOffsetArray = dirOffsetArray;
	
 	}
	
	

	

	
 	/**
 	 * Get freqOffsetArray.
 	 * @return freqOffsetArray as std::vector<Frequency >
 	 */
 	std::vector<Frequency > SwitchCycleRow::getFreqOffsetArray() const {
	
  		return freqOffsetArray;
 	}

 	/**
 	 * Set freqOffsetArray with the specified std::vector<Frequency >.
 	 * @param freqOffsetArray The std::vector<Frequency > value to which freqOffsetArray is to be set.
 	 
 	
 		
 	 */
 	void SwitchCycleRow::setFreqOffsetArray (std::vector<Frequency > freqOffsetArray)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->freqOffsetArray = freqOffsetArray;
	
 	}
	
	

	

	
 	/**
 	 * Get stepDurationArray.
 	 * @return stepDurationArray as std::vector<Interval >
 	 */
 	std::vector<Interval > SwitchCycleRow::getStepDurationArray() const {
	
  		return stepDurationArray;
 	}

 	/**
 	 * Set stepDurationArray with the specified std::vector<Interval >.
 	 * @param stepDurationArray The std::vector<Interval > value to which stepDurationArray is to be set.
 	 
 	
 		
 	 */
 	void SwitchCycleRow::setStepDurationArray (std::vector<Interval > stepDurationArray)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->stepDurationArray = stepDurationArray;
	
 	}
	
	

	
	/**
	 * The attribute directionCode is optional. Return true if this attribute exists.
	 * @return true if and only if the directionCode attribute exists. 
	 */
	bool SwitchCycleRow::isDirectionCodeExists() const {
		return directionCodeExists;
	}
	

	
 	/**
 	 * Get directionCode, which is optional.
 	 * @return directionCode as DirectionReferenceCodeMod::DirectionReferenceCode
 	 * @throw IllegalAccessException If directionCode does not exist.
 	 */
 	DirectionReferenceCodeMod::DirectionReferenceCode SwitchCycleRow::getDirectionCode() const  {
		if (!directionCodeExists) {
			throw IllegalAccessException("directionCode", "SwitchCycle");
		}
	
  		return directionCode;
 	}

 	/**
 	 * Set directionCode with the specified DirectionReferenceCodeMod::DirectionReferenceCode.
 	 * @param directionCode The DirectionReferenceCodeMod::DirectionReferenceCode value to which directionCode is to be set.
 	 
 	
 	 */
 	void SwitchCycleRow::setDirectionCode (DirectionReferenceCodeMod::DirectionReferenceCode directionCode) {
	
 		this->directionCode = directionCode;
	
		directionCodeExists = true;
	
 	}
	
	
	/**
	 * Mark directionCode, which is an optional field, as non-existent.
	 */
	void SwitchCycleRow::clearDirectionCode () {
		directionCodeExists = false;
	}
	

	
	/**
	 * The attribute directionEquinox is optional. Return true if this attribute exists.
	 * @return true if and only if the directionEquinox attribute exists. 
	 */
	bool SwitchCycleRow::isDirectionEquinoxExists() const {
		return directionEquinoxExists;
	}
	

	
 	/**
 	 * Get directionEquinox, which is optional.
 	 * @return directionEquinox as ArrayTime
 	 * @throw IllegalAccessException If directionEquinox does not exist.
 	 */
 	ArrayTime SwitchCycleRow::getDirectionEquinox() const  {
		if (!directionEquinoxExists) {
			throw IllegalAccessException("directionEquinox", "SwitchCycle");
		}
	
  		return directionEquinox;
 	}

 	/**
 	 * Set directionEquinox with the specified ArrayTime.
 	 * @param directionEquinox The ArrayTime value to which directionEquinox is to be set.
 	 
 	
 	 */
 	void SwitchCycleRow::setDirectionEquinox (ArrayTime directionEquinox) {
	
 		this->directionEquinox = directionEquinox;
	
		directionEquinoxExists = true;
	
 	}
	
	
	/**
	 * Mark directionEquinox, which is an optional field, as non-existent.
	 */
	void SwitchCycleRow::clearDirectionEquinox () {
		directionEquinoxExists = false;
	}
	

	
	///////////////////////////////////////////////
	// Extrinsic Table Attributes getters/setters//
	///////////////////////////////////////////////
	

	//////////////////////////////////////
	// Links Attributes getters/setters //
	//////////////////////////////////////
	
	
	/**
	 * Create a SwitchCycleRow.
	 * <p>
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 */ 
	SwitchCycleRow::SwitchCycleRow (SwitchCycleTable &t) : table(t) {
		hasBeenAdded = false;
		
	
	

	

	

	

	

	

	
		directionCodeExists = false;
	

	
		directionEquinoxExists = false;
	

	
	
	
	
	

	

	

	

	

	

	
// This attribute is scalar and has an enumeration type. Let's initialize it to some valid value (the 1st of the enumeration).		
directionCode = CDirectionReferenceCode::from_int(0);
	

	

	
	
	 fromBinMethods["switchCycleId"] = &SwitchCycleRow::switchCycleIdFromBin; 
	 fromBinMethods["numStep"] = &SwitchCycleRow::numStepFromBin; 
	 fromBinMethods["weightArray"] = &SwitchCycleRow::weightArrayFromBin; 
	 fromBinMethods["dirOffsetArray"] = &SwitchCycleRow::dirOffsetArrayFromBin; 
	 fromBinMethods["freqOffsetArray"] = &SwitchCycleRow::freqOffsetArrayFromBin; 
	 fromBinMethods["stepDurationArray"] = &SwitchCycleRow::stepDurationArrayFromBin; 
		
	
	 fromBinMethods["directionCode"] = &SwitchCycleRow::directionCodeFromBin; 
	 fromBinMethods["directionEquinox"] = &SwitchCycleRow::directionEquinoxFromBin; 
	
	
	
	
				 
	fromTextMethods["switchCycleId"] = &SwitchCycleRow::switchCycleIdFromText;
		 
	
				 
	fromTextMethods["numStep"] = &SwitchCycleRow::numStepFromText;
		 
	
				 
	fromTextMethods["weightArray"] = &SwitchCycleRow::weightArrayFromText;
		 
	
				 
	fromTextMethods["dirOffsetArray"] = &SwitchCycleRow::dirOffsetArrayFromText;
		 
	
				 
	fromTextMethods["freqOffsetArray"] = &SwitchCycleRow::freqOffsetArrayFromText;
		 
	
				 
	fromTextMethods["stepDurationArray"] = &SwitchCycleRow::stepDurationArrayFromText;
		 
	

	 
				
	fromTextMethods["directionCode"] = &SwitchCycleRow::directionCodeFromText;
		 	
	 
				
	fromTextMethods["directionEquinox"] = &SwitchCycleRow::directionEquinoxFromText;
		 	
		
	}
	
	SwitchCycleRow::SwitchCycleRow (SwitchCycleTable &t, SwitchCycleRow *row) : table(t) {
		hasBeenAdded = false;
		
		if (row == 0) {
	
	
	

	

	

	

	

	

	
		directionCodeExists = false;
	

	
		directionEquinoxExists = false;
	

			
		}
		else {
	
		
			switchCycleId = row->switchCycleId;
		
		
		
		
			numStep = row->numStep;
		
			weightArray = row->weightArray;
		
			dirOffsetArray = row->dirOffsetArray;
		
			freqOffsetArray = row->freqOffsetArray;
		
			stepDurationArray = row->stepDurationArray;
		
		
		
		
		if (row->directionCodeExists) {
			directionCode = row->directionCode;		
			directionCodeExists = true;
		}
		else
			directionCodeExists = false;
		
		if (row->directionEquinoxExists) {
			directionEquinox = row->directionEquinox;		
			directionEquinoxExists = true;
		}
		else
			directionEquinoxExists = false;
		
		}
		
		 fromBinMethods["switchCycleId"] = &SwitchCycleRow::switchCycleIdFromBin; 
		 fromBinMethods["numStep"] = &SwitchCycleRow::numStepFromBin; 
		 fromBinMethods["weightArray"] = &SwitchCycleRow::weightArrayFromBin; 
		 fromBinMethods["dirOffsetArray"] = &SwitchCycleRow::dirOffsetArrayFromBin; 
		 fromBinMethods["freqOffsetArray"] = &SwitchCycleRow::freqOffsetArrayFromBin; 
		 fromBinMethods["stepDurationArray"] = &SwitchCycleRow::stepDurationArrayFromBin; 
			
	
		 fromBinMethods["directionCode"] = &SwitchCycleRow::directionCodeFromBin; 
		 fromBinMethods["directionEquinox"] = &SwitchCycleRow::directionEquinoxFromBin; 
			
	}

	
	bool SwitchCycleRow::compareNoAutoInc(int numStep, std::vector<float > weightArray, std::vector<std::vector<Angle > > dirOffsetArray, std::vector<Frequency > freqOffsetArray, std::vector<Interval > stepDurationArray) {
		bool result;
		result = true;
		
	
		
		result = result && (this->numStep == numStep);
		
		if (!result) return false;
	

	
		
		result = result && (this->weightArray == weightArray);
		
		if (!result) return false;
	

	
		
		result = result && (this->dirOffsetArray == dirOffsetArray);
		
		if (!result) return false;
	

	
		
		result = result && (this->freqOffsetArray == freqOffsetArray);
		
		if (!result) return false;
	

	
		
		result = result && (this->stepDurationArray == stepDurationArray);
		
		if (!result) return false;
	

		return result;
	}	
	
	
	
	bool SwitchCycleRow::compareRequiredValue(int numStep, std::vector<float > weightArray, std::vector<std::vector<Angle > > dirOffsetArray, std::vector<Frequency > freqOffsetArray, std::vector<Interval > stepDurationArray) {
		bool result;
		result = true;
		
	
		if (!(this->numStep == numStep)) return false;
	

	
		if (!(this->weightArray == weightArray)) return false;
	

	
		if (!(this->dirOffsetArray == dirOffsetArray)) return false;
	

	
		if (!(this->freqOffsetArray == freqOffsetArray)) return false;
	

	
		if (!(this->stepDurationArray == stepDurationArray)) return false;
	

		return result;
	}
	
	
	/**
	 * Return true if all required attributes of the value part are equal to their homologues
	 * in x and false otherwise.
	 *

	 * @param x a pointer on the SwitchCycleRow whose required attributes of the value part 

	 * will be compared with those of this.
	 * @return a boolean.
	 */
	bool SwitchCycleRow::equalByRequiredValue(SwitchCycleRow*  x ) {
		
			
		if (this->numStep != x->numStep) return false;
			
		if (this->weightArray != x->weightArray) return false;
			
		if (this->dirOffsetArray != x->dirOffsetArray) return false;
			
		if (this->freqOffsetArray != x->freqOffsetArray) return false;
			
		if (this->stepDurationArray != x->stepDurationArray) return false;
			
		
		return true;
	}	
	
/*
	 map<string, SwitchCycleAttributeFromBin> SwitchCycleRow::initFromBinMethods() {
		map<string, SwitchCycleAttributeFromBin> result;
		
		result["switchCycleId"] = &SwitchCycleRow::switchCycleIdFromBin;
		result["numStep"] = &SwitchCycleRow::numStepFromBin;
		result["weightArray"] = &SwitchCycleRow::weightArrayFromBin;
		result["dirOffsetArray"] = &SwitchCycleRow::dirOffsetArrayFromBin;
		result["freqOffsetArray"] = &SwitchCycleRow::freqOffsetArrayFromBin;
		result["stepDurationArray"] = &SwitchCycleRow::stepDurationArrayFromBin;
		
		
		result["directionCode"] = &SwitchCycleRow::directionCodeFromBin;
		result["directionEquinox"] = &SwitchCycleRow::directionEquinoxFromBin;
			
		
		return result;	
	}
*/	
} // End namespace asdm
 
