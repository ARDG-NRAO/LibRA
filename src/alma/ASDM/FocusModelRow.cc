
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
 * File FocusModelRow.cpp
 */
 
#include <vector>
#include <set>

#include <alma/ASDM/ASDM.h>
#include <alma/ASDM/FocusModelRow.h>
#include <alma/ASDM/FocusModelTable.h>

#include <alma/ASDM/AntennaTable.h>
#include <alma/ASDM/AntennaRow.h>

#include <alma/ASDM/FocusModelTable.h>
#include <alma/ASDM/FocusModelRow.h>
	

using asdm::ASDM;
using asdm::FocusModelRow;
using asdm::FocusModelTable;

using asdm::AntennaTable;
using asdm::AntennaRow;

using asdm::FocusModelTable;
using asdm::FocusModelRow;


#include <alma/ASDM/Parser.h>

#include <alma/ASDM/EnumerationParser.h>
#include <alma/ASDM/ASDMValuesParser.h>
 
#include <alma/ASDM/InvalidArgumentException.h>

using namespace std;

namespace asdm {
	FocusModelRow::~FocusModelRow() {
	}

	/**
	 * Return the table to which this row belongs.
	 */
	FocusModelTable &FocusModelRow::getTable() const {
		return table;
	}

	bool FocusModelRow::isAdded() const {
		return hasBeenAdded;
	}	

	void FocusModelRow::isAdded(bool added) {
		hasBeenAdded = added;
	}
	
#ifndef WITHOUT_ACS
	using asdmIDL::FocusModelRowIDL;
#endif
	
#ifndef WITHOUT_ACS
	/**
	 * Return this row in the form of an IDL struct.
	 * @return The values of this row as a FocusModelRowIDL struct.
	 */
	FocusModelRowIDL *FocusModelRow::toIDL() const {
		FocusModelRowIDL *x = new FocusModelRowIDL ();
		
		// Fill the IDL structure.
	
		
	
  		
		
		
			
				
		x->focusModelId = focusModelId;
 				
 			
		
	

	
  		
		
		
			
				
		x->polarizationType = polarizationType;
 				
 			
		
	

	
  		
		
		
			
				
		x->receiverBand = receiverBand;
 				
 			
		
	

	
  		
		
		
			
				
		x->numCoeff = numCoeff;
 				
 			
		
	

	
  		
		
		
			
		x->coeffName.length(coeffName.size());
		for (unsigned int i = 0; i < coeffName.size(); ++i) {
			
				
			x->coeffName[i] = CORBA::string_dup(coeffName.at(i).c_str());
				
	 		
	 	}
			
		
	

	
  		
		
		
			
		x->coeffFormula.length(coeffFormula.size());
		for (unsigned int i = 0; i < coeffFormula.size(); ++i) {
			
				
			x->coeffFormula[i] = CORBA::string_dup(coeffFormula.at(i).c_str());
				
	 		
	 	}
			
		
	

	
  		
		
		
			
		x->coeffVal.length(coeffVal.size());
		for (unsigned int i = 0; i < coeffVal.size(); ++i) {
			
				
			x->coeffVal[i] = coeffVal.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		
			
				
		x->assocNature = CORBA::string_dup(assocNature.c_str());
				
 			
		
	

	
	
		
	
  	
 		
		
	 	
			
		x->antennaId = antennaId.toIDLTag();
			
	 	 		
  	

	
  	
 		
		
	 	
			
				
		x->assocFocusModelId = assocFocusModelId;
 				
 			
	 	 		
  	

	
		
	

	

		
		return x;
	
	}
	
	void FocusModelRow::toIDL(asdmIDL::FocusModelRowIDL& x) const {
		// Set the x's fields.
	
		
	
  		
		
		
			
				
		x.focusModelId = focusModelId;
 				
 			
		
	

	
  		
		
		
			
				
		x.polarizationType = polarizationType;
 				
 			
		
	

	
  		
		
		
			
				
		x.receiverBand = receiverBand;
 				
 			
		
	

	
  		
		
		
			
				
		x.numCoeff = numCoeff;
 				
 			
		
	

	
  		
		
		
			
		x.coeffName.length(coeffName.size());
		for (unsigned int i = 0; i < coeffName.size(); ++i) {
			
				
			x.coeffName[i] = CORBA::string_dup(coeffName.at(i).c_str());
				
	 		
	 	}
			
		
	

	
  		
		
		
			
		x.coeffFormula.length(coeffFormula.size());
		for (unsigned int i = 0; i < coeffFormula.size(); ++i) {
			
				
			x.coeffFormula[i] = CORBA::string_dup(coeffFormula.at(i).c_str());
				
	 		
	 	}
			
		
	

	
  		
		
		
			
		x.coeffVal.length(coeffVal.size());
		for (unsigned int i = 0; i < coeffVal.size(); ++i) {
			
				
			x.coeffVal[i] = coeffVal.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		
			
				
		x.assocNature = CORBA::string_dup(assocNature.c_str());
				
 			
		
	

	
	
		
	
  	
 		
		
	 	
			
		x.antennaId = antennaId.toIDLTag();
			
	 	 		
  	

	
  	
 		
		
	 	
			
				
		x.assocFocusModelId = assocFocusModelId;
 				
 			
	 	 		
  	

	
		
	

	

	
	}
#endif
	

#ifndef WITHOUT_ACS
	/**
	 * Fill the values of this row from the IDL struct FocusModelRowIDL.
	 * @param x The IDL struct containing the values used to fill this row.
	 */
	void FocusModelRow::setFromIDL (FocusModelRowIDL x){
		try {
		// Fill the values from x.
	
		
	
		
		
			
		setFocusModelId(x.focusModelId);
  			
 		
		
	

	
		
		
			
		setPolarizationType(x.polarizationType);
  			
 		
		
	

	
		
		
			
		setReceiverBand(x.receiverBand);
  			
 		
		
	

	
		
		
			
		setNumCoeff(x.numCoeff);
  			
 		
		
	

	
		
		
			
		coeffName .clear();
		for (unsigned int i = 0; i <x.coeffName.length(); ++i) {
			
			coeffName.push_back(string (x.coeffName[i]));
			
		}
			
  		
		
	

	
		
		
			
		coeffFormula .clear();
		for (unsigned int i = 0; i <x.coeffFormula.length(); ++i) {
			
			coeffFormula.push_back(string (x.coeffFormula[i]));
			
		}
			
  		
		
	

	
		
		
			
		coeffVal .clear();
		for (unsigned int i = 0; i <x.coeffVal.length(); ++i) {
			
			coeffVal.push_back(x.coeffVal[i]);
  			
		}
			
  		
		
	

	
		
		
			
		setAssocNature(string (x.assocNature));
			
 		
		
	

	
	
		
	
		
		
			
		setAntennaId(Tag (x.antennaId));
			
 		
		
	

	
		
		
			
		setAssocFocusModelId(x.assocFocusModelId);
  			
 		
		
	

	
		
	

	

		} catch (const IllegalAccessException &err) {
			throw ConversionException (err.getMessage(),"FocusModel");
		}
	}
#endif
	
	/**
	 * Return this row in the form of an XML string.
	 * @return The values of this row as an XML string.
	 */
	string FocusModelRow::toXML() const {
		string buf;
		buf.append("<row> \n");
		
	
		
  	
 		
		
		Parser::toXML(focusModelId, "focusModelId", buf);
		
		
	

  	
 		
		
			buf.append(EnumerationParser::toXML("polarizationType", polarizationType));
		
		
	

  	
 		
		
			buf.append(EnumerationParser::toXML("receiverBand", receiverBand));
		
		
	

  	
 		
		
		Parser::toXML(numCoeff, "numCoeff", buf);
		
		
	

  	
 		
		
		Parser::toXML(coeffName, "coeffName", buf);
		
		
	

  	
 		
		
		Parser::toXML(coeffFormula, "coeffFormula", buf);
		
		
	

  	
 		
		
		Parser::toXML(coeffVal, "coeffVal", buf);
		
		
	

  	
 		
		
		Parser::toXML(assocNature, "assocNature", buf);
		
		
	

	
	
		
  	
 		
		
		Parser::toXML(antennaId, "antennaId", buf);
		
		
	

  	
 		
		
		Parser::toXML(assocFocusModelId, "assocFocusModelId", buf);
		
		
	

	
		
	

	

		
		buf.append("</row>\n");
		return buf;
	}

	/**
	 * Fill the values of this row from an XML string 
	 * that was produced by the toXML() method.
	 * @param x The XML string being used to set the values of this row.
	 */
	void FocusModelRow::setFromXML (string rowDoc) {
		Parser row(rowDoc);
		string s = "";
		try {
	
		
	
  		
			
	  	setFocusModelId(Parser::getInteger("focusModelId","FocusModel",rowDoc));
			
		
	

	
		
		
		
		polarizationType = EnumerationParser::getPolarizationType("polarizationType","FocusModel",rowDoc);
		
		
		
	

	
		
		
		
		receiverBand = EnumerationParser::getReceiverBand("receiverBand","FocusModel",rowDoc);
		
		
		
	

	
  		
			
	  	setNumCoeff(Parser::getInteger("numCoeff","FocusModel",rowDoc));
			
		
	

	
  		
			
					
	  	setCoeffName(Parser::get1DString("coeffName","FocusModel",rowDoc));
	  			
	  		
		
	

	
  		
			
					
	  	setCoeffFormula(Parser::get1DString("coeffFormula","FocusModel",rowDoc));
	  			
	  		
		
	

	
  		
			
					
	  	setCoeffVal(Parser::get1DFloat("coeffVal","FocusModel",rowDoc));
	  			
	  		
		
	

	
  		
			
	  	setAssocNature(Parser::getString("assocNature","FocusModel",rowDoc));
			
		
	

	
	
		
	
  		
			
	  	setAntennaId(Parser::getTag("antennaId","Antenna",rowDoc));
			
		
	

	
  		
			
	  	setAssocFocusModelId(Parser::getInteger("assocFocusModelId","FocusModel",rowDoc));
			
		
	

	
		
	

	

		} catch (const IllegalAccessException &err) {
			throw ConversionException (err.getMessage(),"FocusModel");
		}
	}
	
	void FocusModelRow::toBin(EndianOSStream& eoss) {
	
	
	
	
		
	antennaId.toBin(eoss);
		
	

	
	
		
						
			eoss.writeInt(focusModelId);
				
		
	

	
	
		
					
		eoss.writeString(CPolarizationType::name(polarizationType));
			/* eoss.writeInt(polarizationType); */
				
		
	

	
	
		
					
		eoss.writeString(CReceiverBand::name(receiverBand));
			/* eoss.writeInt(receiverBand); */
				
		
	

	
	
		
						
			eoss.writeInt(numCoeff);
				
		
	

	
	
		
		
			
		eoss.writeInt((int) coeffName.size());
		for (unsigned int i = 0; i < coeffName.size(); i++)
				
			eoss.writeString(coeffName.at(i));
				
				
						
		
	

	
	
		
		
			
		eoss.writeInt((int) coeffFormula.size());
		for (unsigned int i = 0; i < coeffFormula.size(); i++)
				
			eoss.writeString(coeffFormula.at(i));
				
				
						
		
	

	
	
		
		
			
		eoss.writeInt((int) coeffVal.size());
		for (unsigned int i = 0; i < coeffVal.size(); i++)
				
			eoss.writeFloat(coeffVal.at(i));
				
				
						
		
	

	
	
		
						
			eoss.writeString(assocNature);
				
		
	

	
	
		
						
			eoss.writeInt(assocFocusModelId);
				
		
	


	
	
	}
	
void FocusModelRow::antennaIdFromBin(EndianIStream& eis) {
		
	
		
		
		antennaId =  Tag::fromBin(eis);
		
	
	
}
void FocusModelRow::focusModelIdFromBin(EndianIStream& eis) {
		
	
	
		
			
		focusModelId =  eis.readInt();
			
		
	
	
}
void FocusModelRow::polarizationTypeFromBin(EndianIStream& eis) {
		
	
	
		
			
		polarizationType = CPolarizationType::literal(eis.readString());
			
		
	
	
}
void FocusModelRow::receiverBandFromBin(EndianIStream& eis) {
		
	
	
		
			
		receiverBand = CReceiverBand::literal(eis.readString());
			
		
	
	
}
void FocusModelRow::numCoeffFromBin(EndianIStream& eis) {
		
	
	
		
			
		numCoeff =  eis.readInt();
			
		
	
	
}
void FocusModelRow::coeffNameFromBin(EndianIStream& eis) {
		
	
	
		
			
	
		coeffName.clear();
		
		unsigned int coeffNameDim1 = eis.readInt();
		for (unsigned int  i = 0 ; i < coeffNameDim1; i++)
			
			coeffName.push_back(eis.readString());
			
	

		
	
	
}
void FocusModelRow::coeffFormulaFromBin(EndianIStream& eis) {
		
	
	
		
			
	
		coeffFormula.clear();
		
		unsigned int coeffFormulaDim1 = eis.readInt();
		for (unsigned int  i = 0 ; i < coeffFormulaDim1; i++)
			
			coeffFormula.push_back(eis.readString());
			
	

		
	
	
}
void FocusModelRow::coeffValFromBin(EndianIStream& eis) {
		
	
	
		
			
	
		coeffVal.clear();
		
		unsigned int coeffValDim1 = eis.readInt();
		for (unsigned int  i = 0 ; i < coeffValDim1; i++)
			
			coeffVal.push_back(eis.readFloat());
			
	

		
	
	
}
void FocusModelRow::assocNatureFromBin(EndianIStream& eis) {
		
	
	
		
			
		assocNature =  eis.readString();
			
		
	
	
}
void FocusModelRow::assocFocusModelIdFromBin(EndianIStream& eis) {
		
	
	
		
			
		assocFocusModelId =  eis.readInt();
			
		
	
	
}

		
	
	FocusModelRow* FocusModelRow::fromBin(EndianIStream& eis, FocusModelTable& table, const vector<string>& attributesSeq) {
		FocusModelRow* row = new  FocusModelRow(table);
		
		map<string, FocusModelAttributeFromBin>::iterator iter ;
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
					throw ConversionException("There is not method to read an attribute '"+attributesSeq.at(i)+"'.", "FocusModelTable");
			}
				
		}				
		return row;
	}

	//
	// A collection of methods to set the value of the attributes from their textual value in the XML representation
	// of one row.
	//
	
	// Convert a string into an Tag 
	void FocusModelRow::antennaIdFromText(const string & s) {
		 
          
		antennaId = ASDMValuesParser::parse<Tag>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void FocusModelRow::focusModelIdFromText(const string & s) {
		 
          
		focusModelId = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	// Convert a string into an PolarizationType 
	void FocusModelRow::polarizationTypeFromText(const string & s) {
		 
          
		polarizationType = ASDMValuesParser::parse<PolarizationTypeMod::PolarizationType>(s);
          
		
	}
	
	
	// Convert a string into an ReceiverBand 
	void FocusModelRow::receiverBandFromText(const string & s) {
		 
          
		receiverBand = ASDMValuesParser::parse<ReceiverBandMod::ReceiverBand>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void FocusModelRow::numCoeffFromText(const string & s) {
		 
          
		numCoeff = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	// Convert a string into an String 
	void FocusModelRow::coeffNameFromText(const string & s) {
		 
          
		coeffName = ASDMValuesParser::parse1D<string>(s);
          
		
	}
	
	
	// Convert a string into an String 
	void FocusModelRow::coeffFormulaFromText(const string & s) {
		 
          
		coeffFormula = ASDMValuesParser::parse1D<string>(s);
          
		
	}
	
	
	// Convert a string into an float 
	void FocusModelRow::coeffValFromText(const string & s) {
		 
          
		coeffVal = ASDMValuesParser::parse1D<float>(s);
          
		
	}
	
	
	// Convert a string into an String 
	void FocusModelRow::assocNatureFromText(const string & s) {
		 
          
		assocNature = ASDMValuesParser::parse<string>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void FocusModelRow::assocFocusModelIdFromText(const string & s) {
		 
          
		assocFocusModelId = ASDMValuesParser::parse<int>(s);
          
		
	}
	

		
	
	void FocusModelRow::fromText(const std::string& attributeName, const std::string&  t) {
		map<string, FocusModelAttributeFromText>::iterator iter;
		if ((iter = fromTextMethods.find(attributeName)) == fromTextMethods.end())
			throw ConversionException("I do not know what to do with '"+attributeName+"' and its content '"+t+"' (while parsing an XML document)", "FocusModelTable");
		(this->*(iter->second))(t);
	}
			
	////////////////////////////////////////////////
	// Intrinsic Table Attributes getters/setters //
	////////////////////////////////////////////////
	
	

	
 	/**
 	 * Get focusModelId.
 	 * @return focusModelId as int
 	 */
 	int FocusModelRow::getFocusModelId() const {
	
  		return focusModelId;
 	}

 	/**
 	 * Set focusModelId with the specified int.
 	 * @param focusModelId The int value to which focusModelId is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void FocusModelRow::setFocusModelId (int focusModelId)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("focusModelId", "FocusModel");
		
  		}
  	
 		this->focusModelId = focusModelId;
	
 	}
	
	

	

	
 	/**
 	 * Get polarizationType.
 	 * @return polarizationType as PolarizationTypeMod::PolarizationType
 	 */
 	PolarizationTypeMod::PolarizationType FocusModelRow::getPolarizationType() const {
	
  		return polarizationType;
 	}

 	/**
 	 * Set polarizationType with the specified PolarizationTypeMod::PolarizationType.
 	 * @param polarizationType The PolarizationTypeMod::PolarizationType value to which polarizationType is to be set.
 	 
 	
 		
 	 */
 	void FocusModelRow::setPolarizationType (PolarizationTypeMod::PolarizationType polarizationType)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->polarizationType = polarizationType;
	
 	}
	
	

	

	
 	/**
 	 * Get receiverBand.
 	 * @return receiverBand as ReceiverBandMod::ReceiverBand
 	 */
 	ReceiverBandMod::ReceiverBand FocusModelRow::getReceiverBand() const {
	
  		return receiverBand;
 	}

 	/**
 	 * Set receiverBand with the specified ReceiverBandMod::ReceiverBand.
 	 * @param receiverBand The ReceiverBandMod::ReceiverBand value to which receiverBand is to be set.
 	 
 	
 		
 	 */
 	void FocusModelRow::setReceiverBand (ReceiverBandMod::ReceiverBand receiverBand)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->receiverBand = receiverBand;
	
 	}
	
	

	

	
 	/**
 	 * Get numCoeff.
 	 * @return numCoeff as int
 	 */
 	int FocusModelRow::getNumCoeff() const {
	
  		return numCoeff;
 	}

 	/**
 	 * Set numCoeff with the specified int.
 	 * @param numCoeff The int value to which numCoeff is to be set.
 	 
 	
 		
 	 */
 	void FocusModelRow::setNumCoeff (int numCoeff)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->numCoeff = numCoeff;
	
 	}
	
	

	

	
 	/**
 	 * Get coeffName.
 	 * @return coeffName as std::vector<std::string >
 	 */
 	std::vector<std::string > FocusModelRow::getCoeffName() const {
	
  		return coeffName;
 	}

 	/**
 	 * Set coeffName with the specified std::vector<std::string >.
 	 * @param coeffName The std::vector<std::string > value to which coeffName is to be set.
 	 
 	
 		
 	 */
 	void FocusModelRow::setCoeffName (std::vector<std::string > coeffName)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->coeffName = coeffName;
	
 	}
	
	

	

	
 	/**
 	 * Get coeffFormula.
 	 * @return coeffFormula as std::vector<std::string >
 	 */
 	std::vector<std::string > FocusModelRow::getCoeffFormula() const {
	
  		return coeffFormula;
 	}

 	/**
 	 * Set coeffFormula with the specified std::vector<std::string >.
 	 * @param coeffFormula The std::vector<std::string > value to which coeffFormula is to be set.
 	 
 	
 		
 	 */
 	void FocusModelRow::setCoeffFormula (std::vector<std::string > coeffFormula)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->coeffFormula = coeffFormula;
	
 	}
	
	

	

	
 	/**
 	 * Get coeffVal.
 	 * @return coeffVal as std::vector<float >
 	 */
 	std::vector<float > FocusModelRow::getCoeffVal() const {
	
  		return coeffVal;
 	}

 	/**
 	 * Set coeffVal with the specified std::vector<float >.
 	 * @param coeffVal The std::vector<float > value to which coeffVal is to be set.
 	 
 	
 		
 	 */
 	void FocusModelRow::setCoeffVal (std::vector<float > coeffVal)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->coeffVal = coeffVal;
	
 	}
	
	

	

	
 	/**
 	 * Get assocNature.
 	 * @return assocNature as std::string
 	 */
 	std::string FocusModelRow::getAssocNature() const {
	
  		return assocNature;
 	}

 	/**
 	 * Set assocNature with the specified std::string.
 	 * @param assocNature The std::string value to which assocNature is to be set.
 	 
 	
 		
 	 */
 	void FocusModelRow::setAssocNature (std::string assocNature)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->assocNature = assocNature;
	
 	}
	
	

	
	///////////////////////////////////////////////
	// Extrinsic Table Attributes getters/setters//
	///////////////////////////////////////////////
	
	

	
 	/**
 	 * Get antennaId.
 	 * @return antennaId as Tag
 	 */
 	Tag FocusModelRow::getAntennaId() const {
	
  		return antennaId;
 	}

 	/**
 	 * Set antennaId with the specified Tag.
 	 * @param antennaId The Tag value to which antennaId is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void FocusModelRow::setAntennaId (Tag antennaId)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("antennaId", "FocusModel");
		
  		}
  	
 		this->antennaId = antennaId;
	
 	}
	
	

	

	
 	/**
 	 * Get assocFocusModelId.
 	 * @return assocFocusModelId as int
 	 */
 	int FocusModelRow::getAssocFocusModelId() const {
	
  		return assocFocusModelId;
 	}

 	/**
 	 * Set assocFocusModelId with the specified int.
 	 * @param assocFocusModelId The int value to which assocFocusModelId is to be set.
 	 
 	
 		
 	 */
 	void FocusModelRow::setAssocFocusModelId (int assocFocusModelId)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->assocFocusModelId = assocFocusModelId;
	
 	}
	
	


	//////////////////////////////////////
	// Links Attributes getters/setters //
	//////////////////////////////////////
	
	
	
	
		

	/**
	 * Returns the pointer to the row in the Antenna table having Antenna.antennaId == antennaId
	 * @return a AntennaRow*
	 * 
	 
	 */
	 AntennaRow* FocusModelRow::getAntennaUsingAntennaId() {
	 
	 	return table.getContainer().getAntenna().getRowByKey(antennaId);
	 }
	 

	

	
	
	
		

	// ===> Slice link from a row of FocusModel table to a collection of row of FocusModel table.
	
	/**
	 * Get the collection of row in the FocusModel table having their attribut focusModelId == this->focusModelId
	 */
	vector <FocusModelRow *> FocusModelRow::getFocusModels() {
		
			return table.getContainer().getFocusModel().getRowByFocusModelId(focusModelId);
		
	}
	

	

	
	/**
	 * Create a FocusModelRow.
	 * <p>
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 */ 
	FocusModelRow::FocusModelRow (FocusModelTable &t) : table(t) {
		hasBeenAdded = false;
		
	
	

	

	

	

	

	

	

	

	
	

	

	
	
	
	

	
// This attribute is scalar and has an enumeration type. Let's initialize it to some valid value (the 1st of the enumeration).		
polarizationType = CPolarizationType::from_int(0);
	

	
// This attribute is scalar and has an enumeration type. Let's initialize it to some valid value (the 1st of the enumeration).		
receiverBand = CReceiverBand::from_int(0);
	

	

	

	

	

	

	
	
	 fromBinMethods["antennaId"] = &FocusModelRow::antennaIdFromBin; 
	 fromBinMethods["focusModelId"] = &FocusModelRow::focusModelIdFromBin; 
	 fromBinMethods["polarizationType"] = &FocusModelRow::polarizationTypeFromBin; 
	 fromBinMethods["receiverBand"] = &FocusModelRow::receiverBandFromBin; 
	 fromBinMethods["numCoeff"] = &FocusModelRow::numCoeffFromBin; 
	 fromBinMethods["coeffName"] = &FocusModelRow::coeffNameFromBin; 
	 fromBinMethods["coeffFormula"] = &FocusModelRow::coeffFormulaFromBin; 
	 fromBinMethods["coeffVal"] = &FocusModelRow::coeffValFromBin; 
	 fromBinMethods["assocNature"] = &FocusModelRow::assocNatureFromBin; 
	 fromBinMethods["assocFocusModelId"] = &FocusModelRow::assocFocusModelIdFromBin; 
		
	
	
	
	
	
				 
	fromTextMethods["antennaId"] = &FocusModelRow::antennaIdFromText;
		 
	
				 
	fromTextMethods["focusModelId"] = &FocusModelRow::focusModelIdFromText;
		 
	
				 
	fromTextMethods["polarizationType"] = &FocusModelRow::polarizationTypeFromText;
		 
	
				 
	fromTextMethods["receiverBand"] = &FocusModelRow::receiverBandFromText;
		 
	
				 
	fromTextMethods["numCoeff"] = &FocusModelRow::numCoeffFromText;
		 
	
				 
	fromTextMethods["coeffName"] = &FocusModelRow::coeffNameFromText;
		 
	
				 
	fromTextMethods["coeffFormula"] = &FocusModelRow::coeffFormulaFromText;
		 
	
				 
	fromTextMethods["coeffVal"] = &FocusModelRow::coeffValFromText;
		 
	
				 
	fromTextMethods["assocNature"] = &FocusModelRow::assocNatureFromText;
		 
	
				 
	fromTextMethods["assocFocusModelId"] = &FocusModelRow::assocFocusModelIdFromText;
		 
	

		
	}
	
	FocusModelRow::FocusModelRow (FocusModelTable &t, FocusModelRow *row) : table(t) {
		hasBeenAdded = false;
		
		if (row == 0) {
	
	
	

	

	

	

	

	

	

	

	
	

	
		
		}
		else {
	
		
			antennaId = row->antennaId;
		
			focusModelId = row->focusModelId;
		
		
		
		
			polarizationType = row->polarizationType;
		
			receiverBand = row->receiverBand;
		
			numCoeff = row->numCoeff;
		
			coeffName = row->coeffName;
		
			coeffFormula = row->coeffFormula;
		
			coeffVal = row->coeffVal;
		
			assocNature = row->assocNature;
		
			assocFocusModelId = row->assocFocusModelId;
		
		
		
		
		}
		
		 fromBinMethods["antennaId"] = &FocusModelRow::antennaIdFromBin; 
		 fromBinMethods["focusModelId"] = &FocusModelRow::focusModelIdFromBin; 
		 fromBinMethods["polarizationType"] = &FocusModelRow::polarizationTypeFromBin; 
		 fromBinMethods["receiverBand"] = &FocusModelRow::receiverBandFromBin; 
		 fromBinMethods["numCoeff"] = &FocusModelRow::numCoeffFromBin; 
		 fromBinMethods["coeffName"] = &FocusModelRow::coeffNameFromBin; 
		 fromBinMethods["coeffFormula"] = &FocusModelRow::coeffFormulaFromBin; 
		 fromBinMethods["coeffVal"] = &FocusModelRow::coeffValFromBin; 
		 fromBinMethods["assocNature"] = &FocusModelRow::assocNatureFromBin; 
		 fromBinMethods["assocFocusModelId"] = &FocusModelRow::assocFocusModelIdFromBin; 
			
	
			
	}

	
	bool FocusModelRow::compareNoAutoInc(Tag antennaId, PolarizationTypeMod::PolarizationType polarizationType, ReceiverBandMod::ReceiverBand receiverBand, int numCoeff, std::vector<std::string > coeffName, std::vector<std::string > coeffFormula, std::vector<float > coeffVal, std::string assocNature, int assocFocusModelId) {
		bool result;
		result = true;
		
	
		
		result = result && (this->antennaId == antennaId);
		
		if (!result) return false;
	

	
		
		result = result && (this->polarizationType == polarizationType);
		
		if (!result) return false;
	

	
		
		result = result && (this->receiverBand == receiverBand);
		
		if (!result) return false;
	

	
		
		result = result && (this->numCoeff == numCoeff);
		
		if (!result) return false;
	

	
		
		result = result && (this->coeffName == coeffName);
		
		if (!result) return false;
	

	
		
		result = result && (this->coeffFormula == coeffFormula);
		
		if (!result) return false;
	

	
		
		result = result && (this->coeffVal == coeffVal);
		
		if (!result) return false;
	

	
		
		result = result && (this->assocNature == assocNature);
		
		if (!result) return false;
	

	
		
		result = result && (this->assocFocusModelId == assocFocusModelId);
		
		if (!result) return false;
	

		return result;
	}	
	
	
	
	bool FocusModelRow::compareRequiredValue(PolarizationTypeMod::PolarizationType polarizationType, ReceiverBandMod::ReceiverBand receiverBand, int numCoeff, std::vector<std::string > coeffName, std::vector<std::string > coeffFormula, std::vector<float > coeffVal, std::string assocNature, int assocFocusModelId) {
		bool result;
		result = true;
		
	
		if (!(this->polarizationType == polarizationType)) return false;
	

	
		if (!(this->receiverBand == receiverBand)) return false;
	

	
		if (!(this->numCoeff == numCoeff)) return false;
	

	
		if (!(this->coeffName == coeffName)) return false;
	

	
		if (!(this->coeffFormula == coeffFormula)) return false;
	

	
		if (!(this->coeffVal == coeffVal)) return false;
	

	
		if (!(this->assocNature == assocNature)) return false;
	

	
		if (!(this->assocFocusModelId == assocFocusModelId)) return false;
	

		return result;
	}
	
	
	/**
	 * Return true if all required attributes of the value part are equal to their homologues
	 * in x and false otherwise.
	 *

	 * @param x a pointer on the FocusModelRow whose required attributes of the value part 

	 * will be compared with those of this.
	 * @return a boolean.
	 */
	bool FocusModelRow::equalByRequiredValue(FocusModelRow*  x ) {
		
			
		if (this->polarizationType != x->polarizationType) return false;
			
		if (this->receiverBand != x->receiverBand) return false;
			
		if (this->numCoeff != x->numCoeff) return false;
			
		if (this->coeffName != x->coeffName) return false;
			
		if (this->coeffFormula != x->coeffFormula) return false;
			
		if (this->coeffVal != x->coeffVal) return false;
			
		if (this->assocNature != x->assocNature) return false;
			
		if (this->assocFocusModelId != x->assocFocusModelId) return false;
			
		
		return true;
	}	
	
/*
	 map<string, FocusModelAttributeFromBin> FocusModelRow::initFromBinMethods() {
		map<string, FocusModelAttributeFromBin> result;
		
		result["antennaId"] = &FocusModelRow::antennaIdFromBin;
		result["focusModelId"] = &FocusModelRow::focusModelIdFromBin;
		result["polarizationType"] = &FocusModelRow::polarizationTypeFromBin;
		result["receiverBand"] = &FocusModelRow::receiverBandFromBin;
		result["numCoeff"] = &FocusModelRow::numCoeffFromBin;
		result["coeffName"] = &FocusModelRow::coeffNameFromBin;
		result["coeffFormula"] = &FocusModelRow::coeffFormulaFromBin;
		result["coeffVal"] = &FocusModelRow::coeffValFromBin;
		result["assocNature"] = &FocusModelRow::assocNatureFromBin;
		result["assocFocusModelId"] = &FocusModelRow::assocFocusModelIdFromBin;
		
		
			
		
		return result;	
	}
*/	
} // End namespace asdm
 
