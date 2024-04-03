
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
 * File CalFocusModelRow.cpp
 */
 
#include <vector>
#include <set>

#include <alma/ASDM/ASDM.h>
#include <alma/ASDM/CalFocusModelRow.h>
#include <alma/ASDM/CalFocusModelTable.h>

#include <alma/ASDM/CalReductionTable.h>
#include <alma/ASDM/CalReductionRow.h>

#include <alma/ASDM/CalDataTable.h>
#include <alma/ASDM/CalDataRow.h>
	

using asdm::ASDM;
using asdm::CalFocusModelRow;
using asdm::CalFocusModelTable;

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
	CalFocusModelRow::~CalFocusModelRow() {
	}

	/**
	 * Return the table to which this row belongs.
	 */
	CalFocusModelTable &CalFocusModelRow::getTable() const {
		return table;
	}

	bool CalFocusModelRow::isAdded() const {
		return hasBeenAdded;
	}	

	void CalFocusModelRow::isAdded(bool added) {
		hasBeenAdded = added;
	}
	
#ifndef WITHOUT_ACS
	using asdmIDL::CalFocusModelRowIDL;
#endif
	
#ifndef WITHOUT_ACS
	/**
	 * Return this row in the form of an IDL struct.
	 * @return The values of this row as a CalFocusModelRowIDL struct.
	 */
	CalFocusModelRowIDL *CalFocusModelRow::toIDL() const {
		CalFocusModelRowIDL *x = new CalFocusModelRowIDL ();
		
		// Fill the IDL structure.
	
		
	
  		
		
		
			
				
		x->antennaName = CORBA::string_dup(antennaName.c_str());
				
 			
		
	

	
  		
		
		
			
				
		x->receiverBand = receiverBand;
 				
 			
		
	

	
  		
		
		
			
				
		x->polarizationType = polarizationType;
 				
 			
		
	

	
  		
		
		
			
		x->startValidTime = startValidTime.toIDLArrayTime();
			
		
	

	
  		
		
		
			
		x->endValidTime = endValidTime.toIDLArrayTime();
			
		
	

	
  		
		
		
			
				
		x->antennaMake = antennaMake;
 				
 			
		
	

	
  		
		
		
			
				
		x->numCoeff = numCoeff;
 				
 			
		
	

	
  		
		
		
			
				
		x->numSourceObs = numSourceObs;
 				
 			
		
	

	
  		
		
		
			
		x->coeffName.length(coeffName.size());
		for (unsigned int i = 0; i < coeffName.size(); ++i) {
			
				
			x->coeffName[i] = CORBA::string_dup(coeffName.at(i).c_str());
				
	 		
	 	}
			
		
	

	
  		
		
		
			
		x->coeffFormula.length(coeffFormula.size());
		for (unsigned int i = 0; i < coeffFormula.size(); ++i) {
			
				
			x->coeffFormula[i] = CORBA::string_dup(coeffFormula.at(i).c_str());
				
	 		
	 	}
			
		
	

	
  		
		
		
			
		x->coeffValue.length(coeffValue.size());
		for (unsigned int i = 0; i < coeffValue.size(); ++i) {
			
				
			x->coeffValue[i] = coeffValue.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		
			
		x->coeffError.length(coeffError.size());
		for (unsigned int i = 0; i < coeffError.size(); ++i) {
			
				
			x->coeffError[i] = coeffError.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		
			
		x->coeffFixed.length(coeffFixed.size());
		for (unsigned int i = 0; i < coeffFixed.size(); ++i) {
			
				
			x->coeffFixed[i] = coeffFixed.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		
			
				
		x->focusModel = CORBA::string_dup(focusModel.c_str());
				
 			
		
	

	
  		
		
		
			
		x->focusRMS.length(focusRMS.size());
		for (unsigned int i = 0; i < focusRMS.size(); ++i) {
			
			x->focusRMS[i] = focusRMS.at(i).toIDLLength();
			
	 	}
			
		
	

	
  		
		
		
			
				
		x->reducedChiSquared = reducedChiSquared;
 				
 			
		
	

	
	
		
	
  	
 		
		
	 	
			
		x->calDataId = calDataId.toIDLTag();
			
	 	 		
  	

	
  	
 		
		
	 	
			
		x->calReductionId = calReductionId.toIDLTag();
			
	 	 		
  	

	
		
	

	

		
		return x;
	
	}
	
	void CalFocusModelRow::toIDL(asdmIDL::CalFocusModelRowIDL& x) const {
		// Set the x's fields.
	
		
	
  		
		
		
			
				
		x.antennaName = CORBA::string_dup(antennaName.c_str());
				
 			
		
	

	
  		
		
		
			
				
		x.receiverBand = receiverBand;
 				
 			
		
	

	
  		
		
		
			
				
		x.polarizationType = polarizationType;
 				
 			
		
	

	
  		
		
		
			
		x.startValidTime = startValidTime.toIDLArrayTime();
			
		
	

	
  		
		
		
			
		x.endValidTime = endValidTime.toIDLArrayTime();
			
		
	

	
  		
		
		
			
				
		x.antennaMake = antennaMake;
 				
 			
		
	

	
  		
		
		
			
				
		x.numCoeff = numCoeff;
 				
 			
		
	

	
  		
		
		
			
				
		x.numSourceObs = numSourceObs;
 				
 			
		
	

	
  		
		
		
			
		x.coeffName.length(coeffName.size());
		for (unsigned int i = 0; i < coeffName.size(); ++i) {
			
				
			x.coeffName[i] = CORBA::string_dup(coeffName.at(i).c_str());
				
	 		
	 	}
			
		
	

	
  		
		
		
			
		x.coeffFormula.length(coeffFormula.size());
		for (unsigned int i = 0; i < coeffFormula.size(); ++i) {
			
				
			x.coeffFormula[i] = CORBA::string_dup(coeffFormula.at(i).c_str());
				
	 		
	 	}
			
		
	

	
  		
		
		
			
		x.coeffValue.length(coeffValue.size());
		for (unsigned int i = 0; i < coeffValue.size(); ++i) {
			
				
			x.coeffValue[i] = coeffValue.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		
			
		x.coeffError.length(coeffError.size());
		for (unsigned int i = 0; i < coeffError.size(); ++i) {
			
				
			x.coeffError[i] = coeffError.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		
			
		x.coeffFixed.length(coeffFixed.size());
		for (unsigned int i = 0; i < coeffFixed.size(); ++i) {
			
				
			x.coeffFixed[i] = coeffFixed.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		
			
				
		x.focusModel = CORBA::string_dup(focusModel.c_str());
				
 			
		
	

	
  		
		
		
			
		x.focusRMS.length(focusRMS.size());
		for (unsigned int i = 0; i < focusRMS.size(); ++i) {
			
			x.focusRMS[i] = focusRMS.at(i).toIDLLength();
			
	 	}
			
		
	

	
  		
		
		
			
				
		x.reducedChiSquared = reducedChiSquared;
 				
 			
		
	

	
	
		
	
  	
 		
		
	 	
			
		x.calDataId = calDataId.toIDLTag();
			
	 	 		
  	

	
  	
 		
		
	 	
			
		x.calReductionId = calReductionId.toIDLTag();
			
	 	 		
  	

	
		
	

	

	
	}
#endif
	

#ifndef WITHOUT_ACS
	/**
	 * Fill the values of this row from the IDL struct CalFocusModelRowIDL.
	 * @param x The IDL struct containing the values used to fill this row.
	 */
	void CalFocusModelRow::setFromIDL (CalFocusModelRowIDL x){
		try {
		// Fill the values from x.
	
		
	
		
		
			
		setAntennaName(string (x.antennaName));
			
 		
		
	

	
		
		
			
		setReceiverBand(x.receiverBand);
  			
 		
		
	

	
		
		
			
		setPolarizationType(x.polarizationType);
  			
 		
		
	

	
		
		
			
		setStartValidTime(ArrayTime (x.startValidTime));
			
 		
		
	

	
		
		
			
		setEndValidTime(ArrayTime (x.endValidTime));
			
 		
		
	

	
		
		
			
		setAntennaMake(x.antennaMake);
  			
 		
		
	

	
		
		
			
		setNumCoeff(x.numCoeff);
  			
 		
		
	

	
		
		
			
		setNumSourceObs(x.numSourceObs);
  			
 		
		
	

	
		
		
			
		coeffName .clear();
		for (unsigned int i = 0; i <x.coeffName.length(); ++i) {
			
			coeffName.push_back(string (x.coeffName[i]));
			
		}
			
  		
		
	

	
		
		
			
		coeffFormula .clear();
		for (unsigned int i = 0; i <x.coeffFormula.length(); ++i) {
			
			coeffFormula.push_back(string (x.coeffFormula[i]));
			
		}
			
  		
		
	

	
		
		
			
		coeffValue .clear();
		for (unsigned int i = 0; i <x.coeffValue.length(); ++i) {
			
			coeffValue.push_back(x.coeffValue[i]);
  			
		}
			
  		
		
	

	
		
		
			
		coeffError .clear();
		for (unsigned int i = 0; i <x.coeffError.length(); ++i) {
			
			coeffError.push_back(x.coeffError[i]);
  			
		}
			
  		
		
	

	
		
		
			
		coeffFixed .clear();
		for (unsigned int i = 0; i <x.coeffFixed.length(); ++i) {
			
			coeffFixed.push_back(x.coeffFixed[i]);
  			
		}
			
  		
		
	

	
		
		
			
		setFocusModel(string (x.focusModel));
			
 		
		
	

	
		
		
			
		focusRMS .clear();
		for (unsigned int i = 0; i <x.focusRMS.length(); ++i) {
			
			focusRMS.push_back(Length (x.focusRMS[i]));
			
		}
			
  		
		
	

	
		
		
			
		setReducedChiSquared(x.reducedChiSquared);
  			
 		
		
	

	
	
		
	
		
		
			
		setCalDataId(Tag (x.calDataId));
			
 		
		
	

	
		
		
			
		setCalReductionId(Tag (x.calReductionId));
			
 		
		
	

	
		
	

	

		} catch (const IllegalAccessException &err) {
			throw ConversionException (err.getMessage(),"CalFocusModel");
		}
	}
#endif
	
	/**
	 * Return this row in the form of an XML string.
	 * @return The values of this row as an XML string.
	 */
	string CalFocusModelRow::toXML() const {
		string buf;
		buf.append("<row> \n");
		
	
		
  	
 		
		
		Parser::toXML(antennaName, "antennaName", buf);
		
		
	

  	
 		
		
			buf.append(EnumerationParser::toXML("receiverBand", receiverBand));
		
		
	

  	
 		
		
			buf.append(EnumerationParser::toXML("polarizationType", polarizationType));
		
		
	

  	
 		
		
		Parser::toXML(startValidTime, "startValidTime", buf);
		
		
	

  	
 		
		
		Parser::toXML(endValidTime, "endValidTime", buf);
		
		
	

  	
 		
		
			buf.append(EnumerationParser::toXML("antennaMake", antennaMake));
		
		
	

  	
 		
		
		Parser::toXML(numCoeff, "numCoeff", buf);
		
		
	

  	
 		
		
		Parser::toXML(numSourceObs, "numSourceObs", buf);
		
		
	

  	
 		
		
		Parser::toXML(coeffName, "coeffName", buf);
		
		
	

  	
 		
		
		Parser::toXML(coeffFormula, "coeffFormula", buf);
		
		
	

  	
 		
		
		Parser::toXML(coeffValue, "coeffValue", buf);
		
		
	

  	
 		
		
		Parser::toXML(coeffError, "coeffError", buf);
		
		
	

  	
 		
		
		Parser::toXML(coeffFixed, "coeffFixed", buf);
		
		
	

  	
 		
		
		Parser::toXML(focusModel, "focusModel", buf);
		
		
	

  	
 		
		
		Parser::toXML(focusRMS, "focusRMS", buf);
		
		
	

  	
 		
		
		Parser::toXML(reducedChiSquared, "reducedChiSquared", buf);
		
		
	

	
	
		
  	
 		
		
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
	void CalFocusModelRow::setFromXML (string rowDoc) {
		Parser row(rowDoc);
		string s = "";
		try {
	
		
	
  		
			
	  	setAntennaName(Parser::getString("antennaName","CalFocusModel",rowDoc));
			
		
	

	
		
		
		
		receiverBand = EnumerationParser::getReceiverBand("receiverBand","CalFocusModel",rowDoc);
		
		
		
	

	
		
		
		
		polarizationType = EnumerationParser::getPolarizationType("polarizationType","CalFocusModel",rowDoc);
		
		
		
	

	
  		
			
	  	setStartValidTime(Parser::getArrayTime("startValidTime","CalFocusModel",rowDoc));
			
		
	

	
  		
			
	  	setEndValidTime(Parser::getArrayTime("endValidTime","CalFocusModel",rowDoc));
			
		
	

	
		
		
		
		antennaMake = EnumerationParser::getAntennaMake("antennaMake","CalFocusModel",rowDoc);
		
		
		
	

	
  		
			
	  	setNumCoeff(Parser::getInteger("numCoeff","CalFocusModel",rowDoc));
			
		
	

	
  		
			
	  	setNumSourceObs(Parser::getInteger("numSourceObs","CalFocusModel",rowDoc));
			
		
	

	
  		
			
					
	  	setCoeffName(Parser::get1DString("coeffName","CalFocusModel",rowDoc));
	  			
	  		
		
	

	
  		
			
					
	  	setCoeffFormula(Parser::get1DString("coeffFormula","CalFocusModel",rowDoc));
	  			
	  		
		
	

	
  		
			
					
	  	setCoeffValue(Parser::get1DFloat("coeffValue","CalFocusModel",rowDoc));
	  			
	  		
		
	

	
  		
			
					
	  	setCoeffError(Parser::get1DFloat("coeffError","CalFocusModel",rowDoc));
	  			
	  		
		
	

	
  		
			
					
	  	setCoeffFixed(Parser::get1DBoolean("coeffFixed","CalFocusModel",rowDoc));
	  			
	  		
		
	

	
  		
			
	  	setFocusModel(Parser::getString("focusModel","CalFocusModel",rowDoc));
			
		
	

	
  		
			
					
	  	setFocusRMS(Parser::get1DLength("focusRMS","CalFocusModel",rowDoc));
	  			
	  		
		
	

	
  		
			
	  	setReducedChiSquared(Parser::getDouble("reducedChiSquared","CalFocusModel",rowDoc));
			
		
	

	
	
		
	
  		
			
	  	setCalDataId(Parser::getTag("calDataId","CalData",rowDoc));
			
		
	

	
  		
			
	  	setCalReductionId(Parser::getTag("calReductionId","CalReduction",rowDoc));
			
		
	

	
		
	

	

		} catch (const IllegalAccessException &err) {
			throw ConversionException (err.getMessage(),"CalFocusModel");
		}
	}
	
	void CalFocusModelRow::toBin(EndianOSStream& eoss) {
	
	
	
	
		
						
			eoss.writeString(antennaName);
				
		
	

	
	
		
					
		eoss.writeString(CReceiverBand::name(receiverBand));
			/* eoss.writeInt(receiverBand); */
				
		
	

	
	
		
					
		eoss.writeString(CPolarizationType::name(polarizationType));
			/* eoss.writeInt(polarizationType); */
				
		
	

	
	
		
	calDataId.toBin(eoss);
		
	

	
	
		
	calReductionId.toBin(eoss);
		
	

	
	
		
	startValidTime.toBin(eoss);
		
	

	
	
		
	endValidTime.toBin(eoss);
		
	

	
	
		
					
		eoss.writeString(CAntennaMake::name(antennaMake));
			/* eoss.writeInt(antennaMake); */
				
		
	

	
	
		
						
			eoss.writeInt(numCoeff);
				
		
	

	
	
		
						
			eoss.writeInt(numSourceObs);
				
		
	

	
	
		
		
			
		eoss.writeInt((int) coeffName.size());
		for (unsigned int i = 0; i < coeffName.size(); i++)
				
			eoss.writeString(coeffName.at(i));
				
				
						
		
	

	
	
		
		
			
		eoss.writeInt((int) coeffFormula.size());
		for (unsigned int i = 0; i < coeffFormula.size(); i++)
				
			eoss.writeString(coeffFormula.at(i));
				
				
						
		
	

	
	
		
		
			
		eoss.writeInt((int) coeffValue.size());
		for (unsigned int i = 0; i < coeffValue.size(); i++)
				
			eoss.writeFloat(coeffValue.at(i));
				
				
						
		
	

	
	
		
		
			
		eoss.writeInt((int) coeffError.size());
		for (unsigned int i = 0; i < coeffError.size(); i++)
				
			eoss.writeFloat(coeffError.at(i));
				
				
						
		
	

	
	
		
		
			
		eoss.writeInt((int) coeffFixed.size());
		for (unsigned int i = 0; i < coeffFixed.size(); i++)
				
			eoss.writeBoolean(coeffFixed.at(i));
				
				
						
		
	

	
	
		
						
			eoss.writeString(focusModel);
				
		
	

	
	
		
	Length::toBin(focusRMS, eoss);
		
	

	
	
		
						
			eoss.writeDouble(reducedChiSquared);
				
		
	


	
	
	}
	
void CalFocusModelRow::antennaNameFromBin(EndianIStream& eis) {
		
	
	
		
			
		antennaName =  eis.readString();
			
		
	
	
}
void CalFocusModelRow::receiverBandFromBin(EndianIStream& eis) {
		
	
	
		
			
		receiverBand = CReceiverBand::literal(eis.readString());
			
		
	
	
}
void CalFocusModelRow::polarizationTypeFromBin(EndianIStream& eis) {
		
	
	
		
			
		polarizationType = CPolarizationType::literal(eis.readString());
			
		
	
	
}
void CalFocusModelRow::calDataIdFromBin(EndianIStream& eis) {
		
	
		
		
		calDataId =  Tag::fromBin(eis);
		
	
	
}
void CalFocusModelRow::calReductionIdFromBin(EndianIStream& eis) {
		
	
		
		
		calReductionId =  Tag::fromBin(eis);
		
	
	
}
void CalFocusModelRow::startValidTimeFromBin(EndianIStream& eis) {
		
	
		
		
		startValidTime =  ArrayTime::fromBin(eis);
		
	
	
}
void CalFocusModelRow::endValidTimeFromBin(EndianIStream& eis) {
		
	
		
		
		endValidTime =  ArrayTime::fromBin(eis);
		
	
	
}
void CalFocusModelRow::antennaMakeFromBin(EndianIStream& eis) {
		
	
	
		
			
		antennaMake = CAntennaMake::literal(eis.readString());
			
		
	
	
}
void CalFocusModelRow::numCoeffFromBin(EndianIStream& eis) {
		
	
	
		
			
		numCoeff =  eis.readInt();
			
		
	
	
}
void CalFocusModelRow::numSourceObsFromBin(EndianIStream& eis) {
		
	
	
		
			
		numSourceObs =  eis.readInt();
			
		
	
	
}
void CalFocusModelRow::coeffNameFromBin(EndianIStream& eis) {
		
	
	
		
			
	
		coeffName.clear();
		
		unsigned int coeffNameDim1 = eis.readInt();
		for (unsigned int  i = 0 ; i < coeffNameDim1; i++)
			
			coeffName.push_back(eis.readString());
			
	

		
	
	
}
void CalFocusModelRow::coeffFormulaFromBin(EndianIStream& eis) {
		
	
	
		
			
	
		coeffFormula.clear();
		
		unsigned int coeffFormulaDim1 = eis.readInt();
		for (unsigned int  i = 0 ; i < coeffFormulaDim1; i++)
			
			coeffFormula.push_back(eis.readString());
			
	

		
	
	
}
void CalFocusModelRow::coeffValueFromBin(EndianIStream& eis) {
		
	
	
		
			
	
		coeffValue.clear();
		
		unsigned int coeffValueDim1 = eis.readInt();
		for (unsigned int  i = 0 ; i < coeffValueDim1; i++)
			
			coeffValue.push_back(eis.readFloat());
			
	

		
	
	
}
void CalFocusModelRow::coeffErrorFromBin(EndianIStream& eis) {
		
	
	
		
			
	
		coeffError.clear();
		
		unsigned int coeffErrorDim1 = eis.readInt();
		for (unsigned int  i = 0 ; i < coeffErrorDim1; i++)
			
			coeffError.push_back(eis.readFloat());
			
	

		
	
	
}
void CalFocusModelRow::coeffFixedFromBin(EndianIStream& eis) {
		
	
	
		
			
	
		coeffFixed.clear();
		
		unsigned int coeffFixedDim1 = eis.readInt();
		for (unsigned int  i = 0 ; i < coeffFixedDim1; i++)
			
			coeffFixed.push_back(eis.readBoolean());
			
	

		
	
	
}
void CalFocusModelRow::focusModelFromBin(EndianIStream& eis) {
		
	
	
		
			
		focusModel =  eis.readString();
			
		
	
	
}
void CalFocusModelRow::focusRMSFromBin(EndianIStream& eis) {
		
	
		
		
			
	
	focusRMS = Length::from1DBin(eis);	
	

		
	
	
}
void CalFocusModelRow::reducedChiSquaredFromBin(EndianIStream& eis) {
		
	
	
		
			
		reducedChiSquared =  eis.readDouble();
			
		
	
	
}

		
	
	CalFocusModelRow* CalFocusModelRow::fromBin(EndianIStream& eis, CalFocusModelTable& table, const vector<string>& attributesSeq) {
		CalFocusModelRow* row = new  CalFocusModelRow(table);
		
		map<string, CalFocusModelAttributeFromBin>::iterator iter ;
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
					throw ConversionException("There is not method to read an attribute '"+attributesSeq.at(i)+"'.", "CalFocusModelTable");
			}
				
		}				
		return row;
	}

	//
	// A collection of methods to set the value of the attributes from their textual value in the XML representation
	// of one row.
	//
	
	// Convert a string into an String 
	void CalFocusModelRow::antennaNameFromText(const string & s) {
		 
          
		antennaName = ASDMValuesParser::parse<string>(s);
          
		
	}
	
	
	// Convert a string into an ReceiverBand 
	void CalFocusModelRow::receiverBandFromText(const string & s) {
		 
          
		receiverBand = ASDMValuesParser::parse<ReceiverBandMod::ReceiverBand>(s);
          
		
	}
	
	
	// Convert a string into an PolarizationType 
	void CalFocusModelRow::polarizationTypeFromText(const string & s) {
		 
          
		polarizationType = ASDMValuesParser::parse<PolarizationTypeMod::PolarizationType>(s);
          
		
	}
	
	
	// Convert a string into an Tag 
	void CalFocusModelRow::calDataIdFromText(const string & s) {
		 
          
		calDataId = ASDMValuesParser::parse<Tag>(s);
          
		
	}
	
	
	// Convert a string into an Tag 
	void CalFocusModelRow::calReductionIdFromText(const string & s) {
		 
          
		calReductionId = ASDMValuesParser::parse<Tag>(s);
          
		
	}
	
	
	// Convert a string into an ArrayTime 
	void CalFocusModelRow::startValidTimeFromText(const string & s) {
		 
          
		startValidTime = ASDMValuesParser::parse<ArrayTime>(s);
          
		
	}
	
	
	// Convert a string into an ArrayTime 
	void CalFocusModelRow::endValidTimeFromText(const string & s) {
		 
          
		endValidTime = ASDMValuesParser::parse<ArrayTime>(s);
          
		
	}
	
	
	// Convert a string into an AntennaMake 
	void CalFocusModelRow::antennaMakeFromText(const string & s) {
		 
          
		antennaMake = ASDMValuesParser::parse<AntennaMakeMod::AntennaMake>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void CalFocusModelRow::numCoeffFromText(const string & s) {
		 
          
		numCoeff = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void CalFocusModelRow::numSourceObsFromText(const string & s) {
		 
          
		numSourceObs = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	// Convert a string into an String 
	void CalFocusModelRow::coeffNameFromText(const string & s) {
		 
          
		coeffName = ASDMValuesParser::parse1D<string>(s);
          
		
	}
	
	
	// Convert a string into an String 
	void CalFocusModelRow::coeffFormulaFromText(const string & s) {
		 
          
		coeffFormula = ASDMValuesParser::parse1D<string>(s);
          
		
	}
	
	
	// Convert a string into an float 
	void CalFocusModelRow::coeffValueFromText(const string & s) {
		 
          
		coeffValue = ASDMValuesParser::parse1D<float>(s);
          
		
	}
	
	
	// Convert a string into an float 
	void CalFocusModelRow::coeffErrorFromText(const string & s) {
		 
          
		coeffError = ASDMValuesParser::parse1D<float>(s);
          
		
	}
	
	
	// Convert a string into an boolean 
	void CalFocusModelRow::coeffFixedFromText(const string & s) {
		 
          
		coeffFixed = ASDMValuesParser::parse1D<bool>(s);
          
		
	}
	
	
	// Convert a string into an String 
	void CalFocusModelRow::focusModelFromText(const string & s) {
		 
          
		focusModel = ASDMValuesParser::parse<string>(s);
          
		
	}
	
	
	// Convert a string into an Length 
	void CalFocusModelRow::focusRMSFromText(const string & s) {
		 
          
		focusRMS = ASDMValuesParser::parse1D<Length>(s);
          
		
	}
	
	
	// Convert a string into an double 
	void CalFocusModelRow::reducedChiSquaredFromText(const string & s) {
		 
          
		reducedChiSquared = ASDMValuesParser::parse<double>(s);
          
		
	}
	

		
	
	void CalFocusModelRow::fromText(const std::string& attributeName, const std::string&  t) {
		map<string, CalFocusModelAttributeFromText>::iterator iter;
		if ((iter = fromTextMethods.find(attributeName)) == fromTextMethods.end())
			throw ConversionException("I do not know what to do with '"+attributeName+"' and its content '"+t+"' (while parsing an XML document)", "CalFocusModelTable");
		(this->*(iter->second))(t);
	}
			
	////////////////////////////////////////////////
	// Intrinsic Table Attributes getters/setters //
	////////////////////////////////////////////////
	
	

	
 	/**
 	 * Get antennaName.
 	 * @return antennaName as std::string
 	 */
 	std::string CalFocusModelRow::getAntennaName() const {
	
  		return antennaName;
 	}

 	/**
 	 * Set antennaName with the specified std::string.
 	 * @param antennaName The std::string value to which antennaName is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void CalFocusModelRow::setAntennaName (std::string antennaName)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("antennaName", "CalFocusModel");
		
  		}
  	
 		this->antennaName = antennaName;
	
 	}
	
	

	

	
 	/**
 	 * Get receiverBand.
 	 * @return receiverBand as ReceiverBandMod::ReceiverBand
 	 */
 	ReceiverBandMod::ReceiverBand CalFocusModelRow::getReceiverBand() const {
	
  		return receiverBand;
 	}

 	/**
 	 * Set receiverBand with the specified ReceiverBandMod::ReceiverBand.
 	 * @param receiverBand The ReceiverBandMod::ReceiverBand value to which receiverBand is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void CalFocusModelRow::setReceiverBand (ReceiverBandMod::ReceiverBand receiverBand)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("receiverBand", "CalFocusModel");
		
  		}
  	
 		this->receiverBand = receiverBand;
	
 	}
	
	

	

	
 	/**
 	 * Get polarizationType.
 	 * @return polarizationType as PolarizationTypeMod::PolarizationType
 	 */
 	PolarizationTypeMod::PolarizationType CalFocusModelRow::getPolarizationType() const {
	
  		return polarizationType;
 	}

 	/**
 	 * Set polarizationType with the specified PolarizationTypeMod::PolarizationType.
 	 * @param polarizationType The PolarizationTypeMod::PolarizationType value to which polarizationType is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void CalFocusModelRow::setPolarizationType (PolarizationTypeMod::PolarizationType polarizationType)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("polarizationType", "CalFocusModel");
		
  		}
  	
 		this->polarizationType = polarizationType;
	
 	}
	
	

	

	
 	/**
 	 * Get startValidTime.
 	 * @return startValidTime as ArrayTime
 	 */
 	ArrayTime CalFocusModelRow::getStartValidTime() const {
	
  		return startValidTime;
 	}

 	/**
 	 * Set startValidTime with the specified ArrayTime.
 	 * @param startValidTime The ArrayTime value to which startValidTime is to be set.
 	 
 	
 		
 	 */
 	void CalFocusModelRow::setStartValidTime (ArrayTime startValidTime)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->startValidTime = startValidTime;
	
 	}
	
	

	

	
 	/**
 	 * Get endValidTime.
 	 * @return endValidTime as ArrayTime
 	 */
 	ArrayTime CalFocusModelRow::getEndValidTime() const {
	
  		return endValidTime;
 	}

 	/**
 	 * Set endValidTime with the specified ArrayTime.
 	 * @param endValidTime The ArrayTime value to which endValidTime is to be set.
 	 
 	
 		
 	 */
 	void CalFocusModelRow::setEndValidTime (ArrayTime endValidTime)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->endValidTime = endValidTime;
	
 	}
	
	

	

	
 	/**
 	 * Get antennaMake.
 	 * @return antennaMake as AntennaMakeMod::AntennaMake
 	 */
 	AntennaMakeMod::AntennaMake CalFocusModelRow::getAntennaMake() const {
	
  		return antennaMake;
 	}

 	/**
 	 * Set antennaMake with the specified AntennaMakeMod::AntennaMake.
 	 * @param antennaMake The AntennaMakeMod::AntennaMake value to which antennaMake is to be set.
 	 
 	
 		
 	 */
 	void CalFocusModelRow::setAntennaMake (AntennaMakeMod::AntennaMake antennaMake)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->antennaMake = antennaMake;
	
 	}
	
	

	

	
 	/**
 	 * Get numCoeff.
 	 * @return numCoeff as int
 	 */
 	int CalFocusModelRow::getNumCoeff() const {
	
  		return numCoeff;
 	}

 	/**
 	 * Set numCoeff with the specified int.
 	 * @param numCoeff The int value to which numCoeff is to be set.
 	 
 	
 		
 	 */
 	void CalFocusModelRow::setNumCoeff (int numCoeff)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->numCoeff = numCoeff;
	
 	}
	
	

	

	
 	/**
 	 * Get numSourceObs.
 	 * @return numSourceObs as int
 	 */
 	int CalFocusModelRow::getNumSourceObs() const {
	
  		return numSourceObs;
 	}

 	/**
 	 * Set numSourceObs with the specified int.
 	 * @param numSourceObs The int value to which numSourceObs is to be set.
 	 
 	
 		
 	 */
 	void CalFocusModelRow::setNumSourceObs (int numSourceObs)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->numSourceObs = numSourceObs;
	
 	}
	
	

	

	
 	/**
 	 * Get coeffName.
 	 * @return coeffName as std::vector<std::string >
 	 */
 	std::vector<std::string > CalFocusModelRow::getCoeffName() const {
	
  		return coeffName;
 	}

 	/**
 	 * Set coeffName with the specified std::vector<std::string >.
 	 * @param coeffName The std::vector<std::string > value to which coeffName is to be set.
 	 
 	
 		
 	 */
 	void CalFocusModelRow::setCoeffName (std::vector<std::string > coeffName)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->coeffName = coeffName;
	
 	}
	
	

	

	
 	/**
 	 * Get coeffFormula.
 	 * @return coeffFormula as std::vector<std::string >
 	 */
 	std::vector<std::string > CalFocusModelRow::getCoeffFormula() const {
	
  		return coeffFormula;
 	}

 	/**
 	 * Set coeffFormula with the specified std::vector<std::string >.
 	 * @param coeffFormula The std::vector<std::string > value to which coeffFormula is to be set.
 	 
 	
 		
 	 */
 	void CalFocusModelRow::setCoeffFormula (std::vector<std::string > coeffFormula)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->coeffFormula = coeffFormula;
	
 	}
	
	

	

	
 	/**
 	 * Get coeffValue.
 	 * @return coeffValue as std::vector<float >
 	 */
 	std::vector<float > CalFocusModelRow::getCoeffValue() const {
	
  		return coeffValue;
 	}

 	/**
 	 * Set coeffValue with the specified std::vector<float >.
 	 * @param coeffValue The std::vector<float > value to which coeffValue is to be set.
 	 
 	
 		
 	 */
 	void CalFocusModelRow::setCoeffValue (std::vector<float > coeffValue)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->coeffValue = coeffValue;
	
 	}
	
	

	

	
 	/**
 	 * Get coeffError.
 	 * @return coeffError as std::vector<float >
 	 */
 	std::vector<float > CalFocusModelRow::getCoeffError() const {
	
  		return coeffError;
 	}

 	/**
 	 * Set coeffError with the specified std::vector<float >.
 	 * @param coeffError The std::vector<float > value to which coeffError is to be set.
 	 
 	
 		
 	 */
 	void CalFocusModelRow::setCoeffError (std::vector<float > coeffError)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->coeffError = coeffError;
	
 	}
	
	

	

	
 	/**
 	 * Get coeffFixed.
 	 * @return coeffFixed as std::vector<bool >
 	 */
 	std::vector<bool > CalFocusModelRow::getCoeffFixed() const {
	
  		return coeffFixed;
 	}

 	/**
 	 * Set coeffFixed with the specified std::vector<bool >.
 	 * @param coeffFixed The std::vector<bool > value to which coeffFixed is to be set.
 	 
 	
 		
 	 */
 	void CalFocusModelRow::setCoeffFixed (std::vector<bool > coeffFixed)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->coeffFixed = coeffFixed;
	
 	}
	
	

	

	
 	/**
 	 * Get focusModel.
 	 * @return focusModel as std::string
 	 */
 	std::string CalFocusModelRow::getFocusModel() const {
	
  		return focusModel;
 	}

 	/**
 	 * Set focusModel with the specified std::string.
 	 * @param focusModel The std::string value to which focusModel is to be set.
 	 
 	
 		
 	 */
 	void CalFocusModelRow::setFocusModel (std::string focusModel)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->focusModel = focusModel;
	
 	}
	
	

	

	
 	/**
 	 * Get focusRMS.
 	 * @return focusRMS as std::vector<Length >
 	 */
 	std::vector<Length > CalFocusModelRow::getFocusRMS() const {
	
  		return focusRMS;
 	}

 	/**
 	 * Set focusRMS with the specified std::vector<Length >.
 	 * @param focusRMS The std::vector<Length > value to which focusRMS is to be set.
 	 
 	
 		
 	 */
 	void CalFocusModelRow::setFocusRMS (std::vector<Length > focusRMS)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->focusRMS = focusRMS;
	
 	}
	
	

	

	
 	/**
 	 * Get reducedChiSquared.
 	 * @return reducedChiSquared as double
 	 */
 	double CalFocusModelRow::getReducedChiSquared() const {
	
  		return reducedChiSquared;
 	}

 	/**
 	 * Set reducedChiSquared with the specified double.
 	 * @param reducedChiSquared The double value to which reducedChiSquared is to be set.
 	 
 	
 		
 	 */
 	void CalFocusModelRow::setReducedChiSquared (double reducedChiSquared)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->reducedChiSquared = reducedChiSquared;
	
 	}
	
	

	
	///////////////////////////////////////////////
	// Extrinsic Table Attributes getters/setters//
	///////////////////////////////////////////////
	
	

	
 	/**
 	 * Get calDataId.
 	 * @return calDataId as Tag
 	 */
 	Tag CalFocusModelRow::getCalDataId() const {
	
  		return calDataId;
 	}

 	/**
 	 * Set calDataId with the specified Tag.
 	 * @param calDataId The Tag value to which calDataId is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void CalFocusModelRow::setCalDataId (Tag calDataId)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("calDataId", "CalFocusModel");
		
  		}
  	
 		this->calDataId = calDataId;
	
 	}
	
	

	

	
 	/**
 	 * Get calReductionId.
 	 * @return calReductionId as Tag
 	 */
 	Tag CalFocusModelRow::getCalReductionId() const {
	
  		return calReductionId;
 	}

 	/**
 	 * Set calReductionId with the specified Tag.
 	 * @param calReductionId The Tag value to which calReductionId is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void CalFocusModelRow::setCalReductionId (Tag calReductionId)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("calReductionId", "CalFocusModel");
		
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
	 CalReductionRow* CalFocusModelRow::getCalReductionUsingCalReductionId() {
	 
	 	return table.getContainer().getCalReduction().getRowByKey(calReductionId);
	 }
	 

	

	
	
	
		

	/**
	 * Returns the pointer to the row in the CalData table having CalData.calDataId == calDataId
	 * @return a CalDataRow*
	 * 
	 
	 */
	 CalDataRow* CalFocusModelRow::getCalDataUsingCalDataId() {
	 
	 	return table.getContainer().getCalData().getRowByKey(calDataId);
	 }
	 

	

	
	/**
	 * Create a CalFocusModelRow.
	 * <p>
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 */ 
	CalFocusModelRow::CalFocusModelRow (CalFocusModelTable &t) : table(t) {
		hasBeenAdded = false;
		
	
	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	
	

	

	
	
	
	

	
// This attribute is scalar and has an enumeration type. Let's initialize it to some valid value (the 1st of the enumeration).		
receiverBand = CReceiverBand::from_int(0);
	

	
// This attribute is scalar and has an enumeration type. Let's initialize it to some valid value (the 1st of the enumeration).		
polarizationType = CPolarizationType::from_int(0);
	

	

	

	
// This attribute is scalar and has an enumeration type. Let's initialize it to some valid value (the 1st of the enumeration).		
antennaMake = CAntennaMake::from_int(0);
	

	

	

	

	

	

	

	

	

	

	

	
	
	 fromBinMethods["antennaName"] = &CalFocusModelRow::antennaNameFromBin; 
	 fromBinMethods["receiverBand"] = &CalFocusModelRow::receiverBandFromBin; 
	 fromBinMethods["polarizationType"] = &CalFocusModelRow::polarizationTypeFromBin; 
	 fromBinMethods["calDataId"] = &CalFocusModelRow::calDataIdFromBin; 
	 fromBinMethods["calReductionId"] = &CalFocusModelRow::calReductionIdFromBin; 
	 fromBinMethods["startValidTime"] = &CalFocusModelRow::startValidTimeFromBin; 
	 fromBinMethods["endValidTime"] = &CalFocusModelRow::endValidTimeFromBin; 
	 fromBinMethods["antennaMake"] = &CalFocusModelRow::antennaMakeFromBin; 
	 fromBinMethods["numCoeff"] = &CalFocusModelRow::numCoeffFromBin; 
	 fromBinMethods["numSourceObs"] = &CalFocusModelRow::numSourceObsFromBin; 
	 fromBinMethods["coeffName"] = &CalFocusModelRow::coeffNameFromBin; 
	 fromBinMethods["coeffFormula"] = &CalFocusModelRow::coeffFormulaFromBin; 
	 fromBinMethods["coeffValue"] = &CalFocusModelRow::coeffValueFromBin; 
	 fromBinMethods["coeffError"] = &CalFocusModelRow::coeffErrorFromBin; 
	 fromBinMethods["coeffFixed"] = &CalFocusModelRow::coeffFixedFromBin; 
	 fromBinMethods["focusModel"] = &CalFocusModelRow::focusModelFromBin; 
	 fromBinMethods["focusRMS"] = &CalFocusModelRow::focusRMSFromBin; 
	 fromBinMethods["reducedChiSquared"] = &CalFocusModelRow::reducedChiSquaredFromBin; 
		
	
	
	
	
	
				 
	fromTextMethods["antennaName"] = &CalFocusModelRow::antennaNameFromText;
		 
	
				 
	fromTextMethods["receiverBand"] = &CalFocusModelRow::receiverBandFromText;
		 
	
				 
	fromTextMethods["polarizationType"] = &CalFocusModelRow::polarizationTypeFromText;
		 
	
				 
	fromTextMethods["calDataId"] = &CalFocusModelRow::calDataIdFromText;
		 
	
				 
	fromTextMethods["calReductionId"] = &CalFocusModelRow::calReductionIdFromText;
		 
	
				 
	fromTextMethods["startValidTime"] = &CalFocusModelRow::startValidTimeFromText;
		 
	
				 
	fromTextMethods["endValidTime"] = &CalFocusModelRow::endValidTimeFromText;
		 
	
				 
	fromTextMethods["antennaMake"] = &CalFocusModelRow::antennaMakeFromText;
		 
	
				 
	fromTextMethods["numCoeff"] = &CalFocusModelRow::numCoeffFromText;
		 
	
				 
	fromTextMethods["numSourceObs"] = &CalFocusModelRow::numSourceObsFromText;
		 
	
				 
	fromTextMethods["coeffName"] = &CalFocusModelRow::coeffNameFromText;
		 
	
				 
	fromTextMethods["coeffFormula"] = &CalFocusModelRow::coeffFormulaFromText;
		 
	
				 
	fromTextMethods["coeffValue"] = &CalFocusModelRow::coeffValueFromText;
		 
	
				 
	fromTextMethods["coeffError"] = &CalFocusModelRow::coeffErrorFromText;
		 
	
				 
	fromTextMethods["coeffFixed"] = &CalFocusModelRow::coeffFixedFromText;
		 
	
				 
	fromTextMethods["focusModel"] = &CalFocusModelRow::focusModelFromText;
		 
	
				 
	fromTextMethods["focusRMS"] = &CalFocusModelRow::focusRMSFromText;
		 
	
				 
	fromTextMethods["reducedChiSquared"] = &CalFocusModelRow::reducedChiSquaredFromText;
		 
	

		
	}
	
	CalFocusModelRow::CalFocusModelRow (CalFocusModelTable &t, CalFocusModelRow *row) : table(t) {
		hasBeenAdded = false;
		
		if (row == 0) {
	
	
	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	
	

	
		
		}
		else {
	
		
			antennaName = row->antennaName;
		
			receiverBand = row->receiverBand;
		
			polarizationType = row->polarizationType;
		
			calDataId = row->calDataId;
		
			calReductionId = row->calReductionId;
		
		
		
		
			startValidTime = row->startValidTime;
		
			endValidTime = row->endValidTime;
		
			antennaMake = row->antennaMake;
		
			numCoeff = row->numCoeff;
		
			numSourceObs = row->numSourceObs;
		
			coeffName = row->coeffName;
		
			coeffFormula = row->coeffFormula;
		
			coeffValue = row->coeffValue;
		
			coeffError = row->coeffError;
		
			coeffFixed = row->coeffFixed;
		
			focusModel = row->focusModel;
		
			focusRMS = row->focusRMS;
		
			reducedChiSquared = row->reducedChiSquared;
		
		
		
		
		}
		
		 fromBinMethods["antennaName"] = &CalFocusModelRow::antennaNameFromBin; 
		 fromBinMethods["receiverBand"] = &CalFocusModelRow::receiverBandFromBin; 
		 fromBinMethods["polarizationType"] = &CalFocusModelRow::polarizationTypeFromBin; 
		 fromBinMethods["calDataId"] = &CalFocusModelRow::calDataIdFromBin; 
		 fromBinMethods["calReductionId"] = &CalFocusModelRow::calReductionIdFromBin; 
		 fromBinMethods["startValidTime"] = &CalFocusModelRow::startValidTimeFromBin; 
		 fromBinMethods["endValidTime"] = &CalFocusModelRow::endValidTimeFromBin; 
		 fromBinMethods["antennaMake"] = &CalFocusModelRow::antennaMakeFromBin; 
		 fromBinMethods["numCoeff"] = &CalFocusModelRow::numCoeffFromBin; 
		 fromBinMethods["numSourceObs"] = &CalFocusModelRow::numSourceObsFromBin; 
		 fromBinMethods["coeffName"] = &CalFocusModelRow::coeffNameFromBin; 
		 fromBinMethods["coeffFormula"] = &CalFocusModelRow::coeffFormulaFromBin; 
		 fromBinMethods["coeffValue"] = &CalFocusModelRow::coeffValueFromBin; 
		 fromBinMethods["coeffError"] = &CalFocusModelRow::coeffErrorFromBin; 
		 fromBinMethods["coeffFixed"] = &CalFocusModelRow::coeffFixedFromBin; 
		 fromBinMethods["focusModel"] = &CalFocusModelRow::focusModelFromBin; 
		 fromBinMethods["focusRMS"] = &CalFocusModelRow::focusRMSFromBin; 
		 fromBinMethods["reducedChiSquared"] = &CalFocusModelRow::reducedChiSquaredFromBin; 
			
	
			
	}

	
	bool CalFocusModelRow::compareNoAutoInc(std::string antennaName, ReceiverBandMod::ReceiverBand receiverBand, PolarizationTypeMod::PolarizationType polarizationType, Tag calDataId, Tag calReductionId, ArrayTime startValidTime, ArrayTime endValidTime, AntennaMakeMod::AntennaMake antennaMake, int numCoeff, int numSourceObs, std::vector<std::string > coeffName, std::vector<std::string > coeffFormula, std::vector<float > coeffValue, std::vector<float > coeffError, std::vector<bool > coeffFixed, std::string focusModel, std::vector<Length > focusRMS, double reducedChiSquared) {
		bool result;
		result = true;
		
	
		
		result = result && (this->antennaName == antennaName);
		
		if (!result) return false;
	

	
		
		result = result && (this->receiverBand == receiverBand);
		
		if (!result) return false;
	

	
		
		result = result && (this->polarizationType == polarizationType);
		
		if (!result) return false;
	

	
		
		result = result && (this->calDataId == calDataId);
		
		if (!result) return false;
	

	
		
		result = result && (this->calReductionId == calReductionId);
		
		if (!result) return false;
	

	
		
		result = result && (this->startValidTime == startValidTime);
		
		if (!result) return false;
	

	
		
		result = result && (this->endValidTime == endValidTime);
		
		if (!result) return false;
	

	
		
		result = result && (this->antennaMake == antennaMake);
		
		if (!result) return false;
	

	
		
		result = result && (this->numCoeff == numCoeff);
		
		if (!result) return false;
	

	
		
		result = result && (this->numSourceObs == numSourceObs);
		
		if (!result) return false;
	

	
		
		result = result && (this->coeffName == coeffName);
		
		if (!result) return false;
	

	
		
		result = result && (this->coeffFormula == coeffFormula);
		
		if (!result) return false;
	

	
		
		result = result && (this->coeffValue == coeffValue);
		
		if (!result) return false;
	

	
		
		result = result && (this->coeffError == coeffError);
		
		if (!result) return false;
	

	
		
		result = result && (this->coeffFixed == coeffFixed);
		
		if (!result) return false;
	

	
		
		result = result && (this->focusModel == focusModel);
		
		if (!result) return false;
	

	
		
		result = result && (this->focusRMS == focusRMS);
		
		if (!result) return false;
	

	
		
		result = result && (this->reducedChiSquared == reducedChiSquared);
		
		if (!result) return false;
	

		return result;
	}	
	
	
	
	bool CalFocusModelRow::compareRequiredValue(ArrayTime startValidTime, ArrayTime endValidTime, AntennaMakeMod::AntennaMake antennaMake, int numCoeff, int numSourceObs, std::vector<std::string > coeffName, std::vector<std::string > coeffFormula, std::vector<float > coeffValue, std::vector<float > coeffError, std::vector<bool > coeffFixed, std::string focusModel, std::vector<Length > focusRMS, double reducedChiSquared) {
		bool result;
		result = true;
		
	
		if (!(this->startValidTime == startValidTime)) return false;
	

	
		if (!(this->endValidTime == endValidTime)) return false;
	

	
		if (!(this->antennaMake == antennaMake)) return false;
	

	
		if (!(this->numCoeff == numCoeff)) return false;
	

	
		if (!(this->numSourceObs == numSourceObs)) return false;
	

	
		if (!(this->coeffName == coeffName)) return false;
	

	
		if (!(this->coeffFormula == coeffFormula)) return false;
	

	
		if (!(this->coeffValue == coeffValue)) return false;
	

	
		if (!(this->coeffError == coeffError)) return false;
	

	
		if (!(this->coeffFixed == coeffFixed)) return false;
	

	
		if (!(this->focusModel == focusModel)) return false;
	

	
		if (!(this->focusRMS == focusRMS)) return false;
	

	
		if (!(this->reducedChiSquared == reducedChiSquared)) return false;
	

		return result;
	}
	
	
	/**
	 * Return true if all required attributes of the value part are equal to their homologues
	 * in x and false otherwise.
	 *

	 * @param x a pointer on the CalFocusModelRow whose required attributes of the value part 

	 * will be compared with those of this.
	 * @return a boolean.
	 */
	bool CalFocusModelRow::equalByRequiredValue(CalFocusModelRow*  x ) {
		
			
		if (this->startValidTime != x->startValidTime) return false;
			
		if (this->endValidTime != x->endValidTime) return false;
			
		if (this->antennaMake != x->antennaMake) return false;
			
		if (this->numCoeff != x->numCoeff) return false;
			
		if (this->numSourceObs != x->numSourceObs) return false;
			
		if (this->coeffName != x->coeffName) return false;
			
		if (this->coeffFormula != x->coeffFormula) return false;
			
		if (this->coeffValue != x->coeffValue) return false;
			
		if (this->coeffError != x->coeffError) return false;
			
		if (this->coeffFixed != x->coeffFixed) return false;
			
		if (this->focusModel != x->focusModel) return false;
			
		if (this->focusRMS != x->focusRMS) return false;
			
		if (this->reducedChiSquared != x->reducedChiSquared) return false;
			
		
		return true;
	}	
	
/*
	 map<string, CalFocusModelAttributeFromBin> CalFocusModelRow::initFromBinMethods() {
		map<string, CalFocusModelAttributeFromBin> result;
		
		result["antennaName"] = &CalFocusModelRow::antennaNameFromBin;
		result["receiverBand"] = &CalFocusModelRow::receiverBandFromBin;
		result["polarizationType"] = &CalFocusModelRow::polarizationTypeFromBin;
		result["calDataId"] = &CalFocusModelRow::calDataIdFromBin;
		result["calReductionId"] = &CalFocusModelRow::calReductionIdFromBin;
		result["startValidTime"] = &CalFocusModelRow::startValidTimeFromBin;
		result["endValidTime"] = &CalFocusModelRow::endValidTimeFromBin;
		result["antennaMake"] = &CalFocusModelRow::antennaMakeFromBin;
		result["numCoeff"] = &CalFocusModelRow::numCoeffFromBin;
		result["numSourceObs"] = &CalFocusModelRow::numSourceObsFromBin;
		result["coeffName"] = &CalFocusModelRow::coeffNameFromBin;
		result["coeffFormula"] = &CalFocusModelRow::coeffFormulaFromBin;
		result["coeffValue"] = &CalFocusModelRow::coeffValueFromBin;
		result["coeffError"] = &CalFocusModelRow::coeffErrorFromBin;
		result["coeffFixed"] = &CalFocusModelRow::coeffFixedFromBin;
		result["focusModel"] = &CalFocusModelRow::focusModelFromBin;
		result["focusRMS"] = &CalFocusModelRow::focusRMSFromBin;
		result["reducedChiSquared"] = &CalFocusModelRow::reducedChiSquaredFromBin;
		
		
			
		
		return result;	
	}
*/	
} // End namespace asdm
 
