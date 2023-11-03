
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
 * File PointingModelRow.cpp
 */
 
#include <vector>
#include <set>

#include <alma/ASDM/ASDM.h>
#include <alma/ASDM/PointingModelRow.h>
#include <alma/ASDM/PointingModelTable.h>

#include <alma/ASDM/PointingModelTable.h>
#include <alma/ASDM/PointingModelRow.h>

#include <alma/ASDM/AntennaTable.h>
#include <alma/ASDM/AntennaRow.h>
	

using asdm::ASDM;
using asdm::PointingModelRow;
using asdm::PointingModelTable;

using asdm::PointingModelTable;
using asdm::PointingModelRow;

using asdm::AntennaTable;
using asdm::AntennaRow;


#include <alma/ASDM/Parser.h>

#include <alma/ASDM/EnumerationParser.h>
#include <alma/ASDM/ASDMValuesParser.h>
 
#include <alma/ASDM/InvalidArgumentException.h>

using namespace std;

namespace asdm {
	PointingModelRow::~PointingModelRow() {
	}

	/**
	 * Return the table to which this row belongs.
	 */
	PointingModelTable &PointingModelRow::getTable() const {
		return table;
	}

	bool PointingModelRow::isAdded() const {
		return hasBeenAdded;
	}	

	void PointingModelRow::isAdded(bool added) {
		hasBeenAdded = added;
	}
	
#ifndef WITHOUT_ACS
	using asdmIDL::PointingModelRowIDL;
#endif
	
#ifndef WITHOUT_ACS
	/**
	 * Return this row in the form of an IDL struct.
	 * @return The values of this row as a PointingModelRowIDL struct.
	 */
	PointingModelRowIDL *PointingModelRow::toIDL() const {
		PointingModelRowIDL *x = new PointingModelRowIDL ();
		
		// Fill the IDL structure.
	
		
	
  		
		
		
			
				
		x->pointingModelId = pointingModelId;
 				
 			
		
	

	
  		
		
		
			
				
		x->numCoeff = numCoeff;
 				
 			
		
	

	
  		
		
		
			
		x->coeffName.length(coeffName.size());
		for (unsigned int i = 0; i < coeffName.size(); ++i) {
			
				
			x->coeffName[i] = CORBA::string_dup(coeffName.at(i).c_str());
				
	 		
	 	}
			
		
	

	
  		
		
		
			
		x->coeffVal.length(coeffVal.size());
		for (unsigned int i = 0; i < coeffVal.size(); ++i) {
			
				
			x->coeffVal[i] = coeffVal.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		
			
				
		x->polarizationType = polarizationType;
 				
 			
		
	

	
  		
		
		
			
				
		x->receiverBand = receiverBand;
 				
 			
		
	

	
  		
		
		
			
				
		x->assocNature = CORBA::string_dup(assocNature.c_str());
				
 			
		
	

	
  		
		
		x->coeffFormulaExists = coeffFormulaExists;
		
		
			
		x->coeffFormula.length(coeffFormula.size());
		for (unsigned int i = 0; i < coeffFormula.size(); ++i) {
			
				
			x->coeffFormula[i] = CORBA::string_dup(coeffFormula.at(i).c_str());
				
	 		
	 	}
			
		
	

	
	
		
	
  	
 		
		
	 	
			
		x->antennaId = antennaId.toIDLTag();
			
	 	 		
  	

	
  	
 		
		
	 	
			
				
		x->assocPointingModelId = assocPointingModelId;
 				
 			
	 	 		
  	

	
		
	

	

		
		return x;
	
	}
	
	void PointingModelRow::toIDL(asdmIDL::PointingModelRowIDL& x) const {
		// Set the x's fields.
	
		
	
  		
		
		
			
				
		x.pointingModelId = pointingModelId;
 				
 			
		
	

	
  		
		
		
			
				
		x.numCoeff = numCoeff;
 				
 			
		
	

	
  		
		
		
			
		x.coeffName.length(coeffName.size());
		for (unsigned int i = 0; i < coeffName.size(); ++i) {
			
				
			x.coeffName[i] = CORBA::string_dup(coeffName.at(i).c_str());
				
	 		
	 	}
			
		
	

	
  		
		
		
			
		x.coeffVal.length(coeffVal.size());
		for (unsigned int i = 0; i < coeffVal.size(); ++i) {
			
				
			x.coeffVal[i] = coeffVal.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		
			
				
		x.polarizationType = polarizationType;
 				
 			
		
	

	
  		
		
		
			
				
		x.receiverBand = receiverBand;
 				
 			
		
	

	
  		
		
		
			
				
		x.assocNature = CORBA::string_dup(assocNature.c_str());
				
 			
		
	

	
  		
		
		x.coeffFormulaExists = coeffFormulaExists;
		
		
			
		x.coeffFormula.length(coeffFormula.size());
		for (unsigned int i = 0; i < coeffFormula.size(); ++i) {
			
				
			x.coeffFormula[i] = CORBA::string_dup(coeffFormula.at(i).c_str());
				
	 		
	 	}
			
		
	

	
	
		
	
  	
 		
		
	 	
			
		x.antennaId = antennaId.toIDLTag();
			
	 	 		
  	

	
  	
 		
		
	 	
			
				
		x.assocPointingModelId = assocPointingModelId;
 				
 			
	 	 		
  	

	
		
	

	

	
	}
#endif
	

#ifndef WITHOUT_ACS
	/**
	 * Fill the values of this row from the IDL struct PointingModelRowIDL.
	 * @param x The IDL struct containing the values used to fill this row.
	 */
	void PointingModelRow::setFromIDL (PointingModelRowIDL x){
		try {
		// Fill the values from x.
	
		
	
		
		
			
		setPointingModelId(x.pointingModelId);
  			
 		
		
	

	
		
		
			
		setNumCoeff(x.numCoeff);
  			
 		
		
	

	
		
		
			
		coeffName .clear();
		for (unsigned int i = 0; i <x.coeffName.length(); ++i) {
			
			coeffName.push_back(string (x.coeffName[i]));
			
		}
			
  		
		
	

	
		
		
			
		coeffVal .clear();
		for (unsigned int i = 0; i <x.coeffVal.length(); ++i) {
			
			coeffVal.push_back(x.coeffVal[i]);
  			
		}
			
  		
		
	

	
		
		
			
		setPolarizationType(x.polarizationType);
  			
 		
		
	

	
		
		
			
		setReceiverBand(x.receiverBand);
  			
 		
		
	

	
		
		
			
		setAssocNature(string (x.assocNature));
			
 		
		
	

	
		
		coeffFormulaExists = x.coeffFormulaExists;
		if (x.coeffFormulaExists) {
		
		
			
		coeffFormula .clear();
		for (unsigned int i = 0; i <x.coeffFormula.length(); ++i) {
			
			coeffFormula.push_back(string (x.coeffFormula[i]));
			
		}
			
  		
		
		}
		
	

	
	
		
	
		
		
			
		setAntennaId(Tag (x.antennaId));
			
 		
		
	

	
		
		
			
		setAssocPointingModelId(x.assocPointingModelId);
  			
 		
		
	

	
		
	

	

		} catch (const IllegalAccessException &err) {
			throw ConversionException (err.getMessage(),"PointingModel");
		}
	}
#endif
	
	/**
	 * Return this row in the form of an XML string.
	 * @return The values of this row as an XML string.
	 */
	string PointingModelRow::toXML() const {
		string buf;
		buf.append("<row> \n");
		
	
		
  	
 		
		
		Parser::toXML(pointingModelId, "pointingModelId", buf);
		
		
	

  	
 		
		
		Parser::toXML(numCoeff, "numCoeff", buf);
		
		
	

  	
 		
		
		Parser::toXML(coeffName, "coeffName", buf);
		
		
	

  	
 		
		
		Parser::toXML(coeffVal, "coeffVal", buf);
		
		
	

  	
 		
		
			buf.append(EnumerationParser::toXML("polarizationType", polarizationType));
		
		
	

  	
 		
		
			buf.append(EnumerationParser::toXML("receiverBand", receiverBand));
		
		
	

  	
 		
		
		Parser::toXML(assocNature, "assocNature", buf);
		
		
	

  	
 		
		if (coeffFormulaExists) {
		
		
		Parser::toXML(coeffFormula, "coeffFormula", buf);
		
		
		}
		
	

	
	
		
  	
 		
		
		Parser::toXML(antennaId, "antennaId", buf);
		
		
	

  	
 		
		
		Parser::toXML(assocPointingModelId, "assocPointingModelId", buf);
		
		
	

	
		
	

	

		
		buf.append("</row>\n");
		return buf;
	}

	/**
	 * Fill the values of this row from an XML string 
	 * that was produced by the toXML() method.
	 * @param x The XML string being used to set the values of this row.
	 */
	void PointingModelRow::setFromXML (string rowDoc) {
		Parser row(rowDoc);
		string s = "";
		try {
	
		
	
  		
			
	  	setPointingModelId(Parser::getInteger("pointingModelId","PointingModel",rowDoc));
			
		
	

	
  		
			
	  	setNumCoeff(Parser::getInteger("numCoeff","PointingModel",rowDoc));
			
		
	

	
  		
			
					
	  	setCoeffName(Parser::get1DString("coeffName","PointingModel",rowDoc));
	  			
	  		
		
	

	
  		
			
					
	  	setCoeffVal(Parser::get1DFloat("coeffVal","PointingModel",rowDoc));
	  			
	  		
		
	

	
		
		
		
		polarizationType = EnumerationParser::getPolarizationType("polarizationType","PointingModel",rowDoc);
		
		
		
	

	
		
		
		
		receiverBand = EnumerationParser::getReceiverBand("receiverBand","PointingModel",rowDoc);
		
		
		
	

	
  		
			
	  	setAssocNature(Parser::getString("assocNature","PointingModel",rowDoc));
			
		
	

	
  		
        if (row.isStr("<coeffFormula>")) {
			
								
	  		setCoeffFormula(Parser::get1DString("coeffFormula","PointingModel",rowDoc));
	  			
	  		
		}
 		
	

	
	
		
	
  		
			
	  	setAntennaId(Parser::getTag("antennaId","Antenna",rowDoc));
			
		
	

	
  		
			
	  	setAssocPointingModelId(Parser::getInteger("assocPointingModelId","PointingModel",rowDoc));
			
		
	

	
		
	

	

		} catch (const IllegalAccessException &err) {
			throw ConversionException (err.getMessage(),"PointingModel");
		}
	}
	
	void PointingModelRow::toBin(EndianOSStream& eoss) {
	
	
	
	
		
	antennaId.toBin(eoss);
		
	

	
	
		
						
			eoss.writeInt(pointingModelId);
				
		
	

	
	
		
						
			eoss.writeInt(numCoeff);
				
		
	

	
	
		
		
			
		eoss.writeInt((int) coeffName.size());
		for (unsigned int i = 0; i < coeffName.size(); i++)
				
			eoss.writeString(coeffName.at(i));
				
				
						
		
	

	
	
		
		
			
		eoss.writeInt((int) coeffVal.size());
		for (unsigned int i = 0; i < coeffVal.size(); i++)
				
			eoss.writeFloat(coeffVal.at(i));
				
				
						
		
	

	
	
		
					
		eoss.writeString(CPolarizationType::name(polarizationType));
			/* eoss.writeInt(polarizationType); */
				
		
	

	
	
		
					
		eoss.writeString(CReceiverBand::name(receiverBand));
			/* eoss.writeInt(receiverBand); */
				
		
	

	
	
		
						
			eoss.writeString(assocNature);
				
		
	

	
	
		
						
			eoss.writeInt(assocPointingModelId);
				
		
	


	
	
	eoss.writeBoolean(coeffFormulaExists);
	if (coeffFormulaExists) {
	
	
	
		
		
			
		eoss.writeInt((int) coeffFormula.size());
		for (unsigned int i = 0; i < coeffFormula.size(); i++)
				
			eoss.writeString(coeffFormula.at(i));
				
				
						
		
	

	}

	}
	
void PointingModelRow::antennaIdFromBin(EndianIStream& eis) {
		
	
		
		
		antennaId =  Tag::fromBin(eis);
		
	
	
}
void PointingModelRow::pointingModelIdFromBin(EndianIStream& eis) {
		
	
	
		
			
		pointingModelId =  eis.readInt();
			
		
	
	
}
void PointingModelRow::numCoeffFromBin(EndianIStream& eis) {
		
	
	
		
			
		numCoeff =  eis.readInt();
			
		
	
	
}
void PointingModelRow::coeffNameFromBin(EndianIStream& eis) {
		
	
	
		
			
	
		coeffName.clear();
		
		unsigned int coeffNameDim1 = eis.readInt();
		for (unsigned int  i = 0 ; i < coeffNameDim1; i++)
			
			coeffName.push_back(eis.readString());
			
	

		
	
	
}
void PointingModelRow::coeffValFromBin(EndianIStream& eis) {
		
	
	
		
			
	
		coeffVal.clear();
		
		unsigned int coeffValDim1 = eis.readInt();
		for (unsigned int  i = 0 ; i < coeffValDim1; i++)
			
			coeffVal.push_back(eis.readFloat());
			
	

		
	
	
}
void PointingModelRow::polarizationTypeFromBin(EndianIStream& eis) {
		
	
	
		
			
		polarizationType = CPolarizationType::literal(eis.readString());
			
		
	
	
}
void PointingModelRow::receiverBandFromBin(EndianIStream& eis) {
		
	
	
		
			
		receiverBand = CReceiverBand::literal(eis.readString());
			
		
	
	
}
void PointingModelRow::assocNatureFromBin(EndianIStream& eis) {
		
	
	
		
			
		assocNature =  eis.readString();
			
		
	
	
}
void PointingModelRow::assocPointingModelIdFromBin(EndianIStream& eis) {
		
	
	
		
			
		assocPointingModelId =  eis.readInt();
			
		
	
	
}

void PointingModelRow::coeffFormulaFromBin(EndianIStream& eis) {
		
	coeffFormulaExists = eis.readBoolean();
	if (coeffFormulaExists) {
		
	
	
		
			
	
		coeffFormula.clear();
		
		unsigned int coeffFormulaDim1 = eis.readInt();
		for (unsigned int  i = 0 ; i < coeffFormulaDim1; i++)
			
			coeffFormula.push_back(eis.readString());
			
	

		
	

	}
	
}
	
	
	PointingModelRow* PointingModelRow::fromBin(EndianIStream& eis, PointingModelTable& table, const vector<string>& attributesSeq) {
		PointingModelRow* row = new  PointingModelRow(table);
		
		map<string, PointingModelAttributeFromBin>::iterator iter ;
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
					throw ConversionException("There is not method to read an attribute '"+attributesSeq.at(i)+"'.", "PointingModelTable");
			}
				
		}				
		return row;
	}

	//
	// A collection of methods to set the value of the attributes from their textual value in the XML representation
	// of one row.
	//
	
	// Convert a string into an Tag 
	void PointingModelRow::antennaIdFromText(const string & s) {
		 
          
		antennaId = ASDMValuesParser::parse<Tag>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void PointingModelRow::pointingModelIdFromText(const string & s) {
		 
          
		pointingModelId = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void PointingModelRow::numCoeffFromText(const string & s) {
		 
          
		numCoeff = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	// Convert a string into an String 
	void PointingModelRow::coeffNameFromText(const string & s) {
		 
          
		coeffName = ASDMValuesParser::parse1D<string>(s);
          
		
	}
	
	
	// Convert a string into an float 
	void PointingModelRow::coeffValFromText(const string & s) {
		 
          
		coeffVal = ASDMValuesParser::parse1D<float>(s);
          
		
	}
	
	
	// Convert a string into an PolarizationType 
	void PointingModelRow::polarizationTypeFromText(const string & s) {
		 
          
		polarizationType = ASDMValuesParser::parse<PolarizationTypeMod::PolarizationType>(s);
          
		
	}
	
	
	// Convert a string into an ReceiverBand 
	void PointingModelRow::receiverBandFromText(const string & s) {
		 
          
		receiverBand = ASDMValuesParser::parse<ReceiverBandMod::ReceiverBand>(s);
          
		
	}
	
	
	// Convert a string into an String 
	void PointingModelRow::assocNatureFromText(const string & s) {
		 
          
		assocNature = ASDMValuesParser::parse<string>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void PointingModelRow::assocPointingModelIdFromText(const string & s) {
		 
          
		assocPointingModelId = ASDMValuesParser::parse<int>(s);
          
		
	}
	

	
	// Convert a string into an String 
	void PointingModelRow::coeffFormulaFromText(const string & s) {
		coeffFormulaExists = true;
		 
          
		coeffFormula = ASDMValuesParser::parse1D<string>(s);
          
		
	}
	
	
	
	void PointingModelRow::fromText(const std::string& attributeName, const std::string&  t) {
		map<string, PointingModelAttributeFromText>::iterator iter;
		if ((iter = fromTextMethods.find(attributeName)) == fromTextMethods.end())
			throw ConversionException("I do not know what to do with '"+attributeName+"' and its content '"+t+"' (while parsing an XML document)", "PointingModelTable");
		(this->*(iter->second))(t);
	}
			
	////////////////////////////////////////////////
	// Intrinsic Table Attributes getters/setters //
	////////////////////////////////////////////////
	
	

	
 	/**
 	 * Get pointingModelId.
 	 * @return pointingModelId as int
 	 */
 	int PointingModelRow::getPointingModelId() const {
	
  		return pointingModelId;
 	}

 	/**
 	 * Set pointingModelId with the specified int.
 	 * @param pointingModelId The int value to which pointingModelId is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void PointingModelRow::setPointingModelId (int pointingModelId)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("pointingModelId", "PointingModel");
		
  		}
  	
 		this->pointingModelId = pointingModelId;
	
 	}
	
	

	

	
 	/**
 	 * Get numCoeff.
 	 * @return numCoeff as int
 	 */
 	int PointingModelRow::getNumCoeff() const {
	
  		return numCoeff;
 	}

 	/**
 	 * Set numCoeff with the specified int.
 	 * @param numCoeff The int value to which numCoeff is to be set.
 	 
 	
 		
 	 */
 	void PointingModelRow::setNumCoeff (int numCoeff)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->numCoeff = numCoeff;
	
 	}
	
	

	

	
 	/**
 	 * Get coeffName.
 	 * @return coeffName as std::vector<std::string >
 	 */
 	std::vector<std::string > PointingModelRow::getCoeffName() const {
	
  		return coeffName;
 	}

 	/**
 	 * Set coeffName with the specified std::vector<std::string >.
 	 * @param coeffName The std::vector<std::string > value to which coeffName is to be set.
 	 
 	
 		
 	 */
 	void PointingModelRow::setCoeffName (std::vector<std::string > coeffName)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->coeffName = coeffName;
	
 	}
	
	

	

	
 	/**
 	 * Get coeffVal.
 	 * @return coeffVal as std::vector<float >
 	 */
 	std::vector<float > PointingModelRow::getCoeffVal() const {
	
  		return coeffVal;
 	}

 	/**
 	 * Set coeffVal with the specified std::vector<float >.
 	 * @param coeffVal The std::vector<float > value to which coeffVal is to be set.
 	 
 	
 		
 	 */
 	void PointingModelRow::setCoeffVal (std::vector<float > coeffVal)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->coeffVal = coeffVal;
	
 	}
	
	

	

	
 	/**
 	 * Get polarizationType.
 	 * @return polarizationType as PolarizationTypeMod::PolarizationType
 	 */
 	PolarizationTypeMod::PolarizationType PointingModelRow::getPolarizationType() const {
	
  		return polarizationType;
 	}

 	/**
 	 * Set polarizationType with the specified PolarizationTypeMod::PolarizationType.
 	 * @param polarizationType The PolarizationTypeMod::PolarizationType value to which polarizationType is to be set.
 	 
 	
 		
 	 */
 	void PointingModelRow::setPolarizationType (PolarizationTypeMod::PolarizationType polarizationType)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->polarizationType = polarizationType;
	
 	}
	
	

	

	
 	/**
 	 * Get receiverBand.
 	 * @return receiverBand as ReceiverBandMod::ReceiverBand
 	 */
 	ReceiverBandMod::ReceiverBand PointingModelRow::getReceiverBand() const {
	
  		return receiverBand;
 	}

 	/**
 	 * Set receiverBand with the specified ReceiverBandMod::ReceiverBand.
 	 * @param receiverBand The ReceiverBandMod::ReceiverBand value to which receiverBand is to be set.
 	 
 	
 		
 	 */
 	void PointingModelRow::setReceiverBand (ReceiverBandMod::ReceiverBand receiverBand)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->receiverBand = receiverBand;
	
 	}
	
	

	

	
 	/**
 	 * Get assocNature.
 	 * @return assocNature as std::string
 	 */
 	std::string PointingModelRow::getAssocNature() const {
	
  		return assocNature;
 	}

 	/**
 	 * Set assocNature with the specified std::string.
 	 * @param assocNature The std::string value to which assocNature is to be set.
 	 
 	
 		
 	 */
 	void PointingModelRow::setAssocNature (std::string assocNature)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->assocNature = assocNature;
	
 	}
	
	

	
	/**
	 * The attribute coeffFormula is optional. Return true if this attribute exists.
	 * @return true if and only if the coeffFormula attribute exists. 
	 */
	bool PointingModelRow::isCoeffFormulaExists() const {
		return coeffFormulaExists;
	}
	

	
 	/**
 	 * Get coeffFormula, which is optional.
 	 * @return coeffFormula as std::vector<std::string >
 	 * @throw IllegalAccessException If coeffFormula does not exist.
 	 */
 	std::vector<std::string > PointingModelRow::getCoeffFormula() const  {
		if (!coeffFormulaExists) {
			throw IllegalAccessException("coeffFormula", "PointingModel");
		}
	
  		return coeffFormula;
 	}

 	/**
 	 * Set coeffFormula with the specified std::vector<std::string >.
 	 * @param coeffFormula The std::vector<std::string > value to which coeffFormula is to be set.
 	 
 	
 	 */
 	void PointingModelRow::setCoeffFormula (std::vector<std::string > coeffFormula) {
	
 		this->coeffFormula = coeffFormula;
	
		coeffFormulaExists = true;
	
 	}
	
	
	/**
	 * Mark coeffFormula, which is an optional field, as non-existent.
	 */
	void PointingModelRow::clearCoeffFormula () {
		coeffFormulaExists = false;
	}
	

	
	///////////////////////////////////////////////
	// Extrinsic Table Attributes getters/setters//
	///////////////////////////////////////////////
	
	

	
 	/**
 	 * Get antennaId.
 	 * @return antennaId as Tag
 	 */
 	Tag PointingModelRow::getAntennaId() const {
	
  		return antennaId;
 	}

 	/**
 	 * Set antennaId with the specified Tag.
 	 * @param antennaId The Tag value to which antennaId is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void PointingModelRow::setAntennaId (Tag antennaId)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("antennaId", "PointingModel");
		
  		}
  	
 		this->antennaId = antennaId;
	
 	}
	
	

	

	
 	/**
 	 * Get assocPointingModelId.
 	 * @return assocPointingModelId as int
 	 */
 	int PointingModelRow::getAssocPointingModelId() const {
	
  		return assocPointingModelId;
 	}

 	/**
 	 * Set assocPointingModelId with the specified int.
 	 * @param assocPointingModelId The int value to which assocPointingModelId is to be set.
 	 
 	
 		
 	 */
 	void PointingModelRow::setAssocPointingModelId (int assocPointingModelId)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->assocPointingModelId = assocPointingModelId;
	
 	}
	
	


	//////////////////////////////////////
	// Links Attributes getters/setters //
	//////////////////////////////////////
	
	
	
	
		

	// ===> Slice link from a row of PointingModel table to a collection of row of PointingModel table.
	
	/**
	 * Get the collection of row in the PointingModel table having their attribut pointingModelId == this->pointingModelId
	 */
	vector <PointingModelRow *> PointingModelRow::getPointingModels() {
		
			return table.getContainer().getPointingModel().getRowByPointingModelId(pointingModelId);
		
	}
	

	

	
	
	
		

	/**
	 * Returns the pointer to the row in the Antenna table having Antenna.antennaId == antennaId
	 * @return a AntennaRow*
	 * 
	 
	 */
	 AntennaRow* PointingModelRow::getAntennaUsingAntennaId() {
	 
	 	return table.getContainer().getAntenna().getRowByKey(antennaId);
	 }
	 

	

	
	/**
	 * Create a PointingModelRow.
	 * <p>
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 */ 
	PointingModelRow::PointingModelRow (PointingModelTable &t) : table(t) {
		hasBeenAdded = false;
		
	
	

	

	

	

	

	

	

	
		coeffFormulaExists = false;
	

	
	

	

	
	
	
	

	

	

	

	
// This attribute is scalar and has an enumeration type. Let's initialize it to some valid value (the 1st of the enumeration).		
polarizationType = CPolarizationType::from_int(0);
	

	
// This attribute is scalar and has an enumeration type. Let's initialize it to some valid value (the 1st of the enumeration).		
receiverBand = CReceiverBand::from_int(0);
	

	

	

	
	
	 fromBinMethods["antennaId"] = &PointingModelRow::antennaIdFromBin; 
	 fromBinMethods["pointingModelId"] = &PointingModelRow::pointingModelIdFromBin; 
	 fromBinMethods["numCoeff"] = &PointingModelRow::numCoeffFromBin; 
	 fromBinMethods["coeffName"] = &PointingModelRow::coeffNameFromBin; 
	 fromBinMethods["coeffVal"] = &PointingModelRow::coeffValFromBin; 
	 fromBinMethods["polarizationType"] = &PointingModelRow::polarizationTypeFromBin; 
	 fromBinMethods["receiverBand"] = &PointingModelRow::receiverBandFromBin; 
	 fromBinMethods["assocNature"] = &PointingModelRow::assocNatureFromBin; 
	 fromBinMethods["assocPointingModelId"] = &PointingModelRow::assocPointingModelIdFromBin; 
		
	
	 fromBinMethods["coeffFormula"] = &PointingModelRow::coeffFormulaFromBin; 
	
	
	
	
				 
	fromTextMethods["antennaId"] = &PointingModelRow::antennaIdFromText;
		 
	
				 
	fromTextMethods["pointingModelId"] = &PointingModelRow::pointingModelIdFromText;
		 
	
				 
	fromTextMethods["numCoeff"] = &PointingModelRow::numCoeffFromText;
		 
	
				 
	fromTextMethods["coeffName"] = &PointingModelRow::coeffNameFromText;
		 
	
				 
	fromTextMethods["coeffVal"] = &PointingModelRow::coeffValFromText;
		 
	
				 
	fromTextMethods["polarizationType"] = &PointingModelRow::polarizationTypeFromText;
		 
	
				 
	fromTextMethods["receiverBand"] = &PointingModelRow::receiverBandFromText;
		 
	
				 
	fromTextMethods["assocNature"] = &PointingModelRow::assocNatureFromText;
		 
	
				 
	fromTextMethods["assocPointingModelId"] = &PointingModelRow::assocPointingModelIdFromText;
		 
	

	 
				
	fromTextMethods["coeffFormula"] = &PointingModelRow::coeffFormulaFromText;
		 	
		
	}
	
	PointingModelRow::PointingModelRow (PointingModelTable &t, PointingModelRow *row) : table(t) {
		hasBeenAdded = false;
		
		if (row == 0) {
	
	
	

	

	

	

	

	

	

	
		coeffFormulaExists = false;
	

	
	

	
		
		}
		else {
	
		
			antennaId = row->antennaId;
		
			pointingModelId = row->pointingModelId;
		
		
		
		
			numCoeff = row->numCoeff;
		
			coeffName = row->coeffName;
		
			coeffVal = row->coeffVal;
		
			polarizationType = row->polarizationType;
		
			receiverBand = row->receiverBand;
		
			assocNature = row->assocNature;
		
			assocPointingModelId = row->assocPointingModelId;
		
		
		
		
		if (row->coeffFormulaExists) {
			coeffFormula = row->coeffFormula;		
			coeffFormulaExists = true;
		}
		else
			coeffFormulaExists = false;
		
		}
		
		 fromBinMethods["antennaId"] = &PointingModelRow::antennaIdFromBin; 
		 fromBinMethods["pointingModelId"] = &PointingModelRow::pointingModelIdFromBin; 
		 fromBinMethods["numCoeff"] = &PointingModelRow::numCoeffFromBin; 
		 fromBinMethods["coeffName"] = &PointingModelRow::coeffNameFromBin; 
		 fromBinMethods["coeffVal"] = &PointingModelRow::coeffValFromBin; 
		 fromBinMethods["polarizationType"] = &PointingModelRow::polarizationTypeFromBin; 
		 fromBinMethods["receiverBand"] = &PointingModelRow::receiverBandFromBin; 
		 fromBinMethods["assocNature"] = &PointingModelRow::assocNatureFromBin; 
		 fromBinMethods["assocPointingModelId"] = &PointingModelRow::assocPointingModelIdFromBin; 
			
	
		 fromBinMethods["coeffFormula"] = &PointingModelRow::coeffFormulaFromBin; 
			
	}

	
	bool PointingModelRow::compareNoAutoInc(Tag antennaId, int numCoeff, std::vector<std::string > coeffName, std::vector<float > coeffVal, PolarizationTypeMod::PolarizationType polarizationType, ReceiverBandMod::ReceiverBand receiverBand, std::string assocNature, int assocPointingModelId) {
		bool result;
		result = true;
		
	
		
		result = result && (this->antennaId == antennaId);
		
		if (!result) return false;
	

	
		
		result = result && (this->numCoeff == numCoeff);
		
		if (!result) return false;
	

	
		
		result = result && (this->coeffName == coeffName);
		
		if (!result) return false;
	

	
		
		result = result && (this->coeffVal == coeffVal);
		
		if (!result) return false;
	

	
		
		result = result && (this->polarizationType == polarizationType);
		
		if (!result) return false;
	

	
		
		result = result && (this->receiverBand == receiverBand);
		
		if (!result) return false;
	

	
		
		result = result && (this->assocNature == assocNature);
		
		if (!result) return false;
	

	
		
		result = result && (this->assocPointingModelId == assocPointingModelId);
		
		if (!result) return false;
	

		return result;
	}	
	
	
	
	bool PointingModelRow::compareRequiredValue(int numCoeff, std::vector<std::string > coeffName, std::vector<float > coeffVal, PolarizationTypeMod::PolarizationType polarizationType, ReceiverBandMod::ReceiverBand receiverBand, std::string assocNature, int assocPointingModelId) {
		bool result;
		result = true;
		
	
		if (!(this->numCoeff == numCoeff)) return false;
	

	
		if (!(this->coeffName == coeffName)) return false;
	

	
		if (!(this->coeffVal == coeffVal)) return false;
	

	
		if (!(this->polarizationType == polarizationType)) return false;
	

	
		if (!(this->receiverBand == receiverBand)) return false;
	

	
		if (!(this->assocNature == assocNature)) return false;
	

	
		if (!(this->assocPointingModelId == assocPointingModelId)) return false;
	

		return result;
	}
	
	
	/**
	 * Return true if all required attributes of the value part are equal to their homologues
	 * in x and false otherwise.
	 *

	 * @param x a pointer on the PointingModelRow whose required attributes of the value part 

	 * will be compared with those of this.
	 * @return a boolean.
	 */
	bool PointingModelRow::equalByRequiredValue(PointingModelRow*  x ) {
		
			
		if (this->numCoeff != x->numCoeff) return false;
			
		if (this->coeffName != x->coeffName) return false;
			
		if (this->coeffVal != x->coeffVal) return false;
			
		if (this->polarizationType != x->polarizationType) return false;
			
		if (this->receiverBand != x->receiverBand) return false;
			
		if (this->assocNature != x->assocNature) return false;
			
		if (this->assocPointingModelId != x->assocPointingModelId) return false;
			
		
		return true;
	}	
	
/*
	 map<string, PointingModelAttributeFromBin> PointingModelRow::initFromBinMethods() {
		map<string, PointingModelAttributeFromBin> result;
		
		result["antennaId"] = &PointingModelRow::antennaIdFromBin;
		result["pointingModelId"] = &PointingModelRow::pointingModelIdFromBin;
		result["numCoeff"] = &PointingModelRow::numCoeffFromBin;
		result["coeffName"] = &PointingModelRow::coeffNameFromBin;
		result["coeffVal"] = &PointingModelRow::coeffValFromBin;
		result["polarizationType"] = &PointingModelRow::polarizationTypeFromBin;
		result["receiverBand"] = &PointingModelRow::receiverBandFromBin;
		result["assocNature"] = &PointingModelRow::assocNatureFromBin;
		result["assocPointingModelId"] = &PointingModelRow::assocPointingModelIdFromBin;
		
		
		result["coeffFormula"] = &PointingModelRow::coeffFormulaFromBin;
			
		
		return result;	
	}
*/	
} // End namespace asdm
 
