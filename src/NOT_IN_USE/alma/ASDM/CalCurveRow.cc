
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
 * File CalCurveRow.cpp
 */
 
#include <vector>
#include <set>

#include <alma/ASDM/ASDM.h>
#include <alma/ASDM/CalCurveRow.h>
#include <alma/ASDM/CalCurveTable.h>

#include <alma/ASDM/CalDataTable.h>
#include <alma/ASDM/CalDataRow.h>

#include <alma/ASDM/CalReductionTable.h>
#include <alma/ASDM/CalReductionRow.h>
	

using asdm::ASDM;
using asdm::CalCurveRow;
using asdm::CalCurveTable;

using asdm::CalDataTable;
using asdm::CalDataRow;

using asdm::CalReductionTable;
using asdm::CalReductionRow;


#include <alma/ASDM/Parser.h>

#include <alma/ASDM/EnumerationParser.h>
#include <alma/ASDM/ASDMValuesParser.h>
 
#include <alma/ASDM/InvalidArgumentException.h>

using namespace std;

namespace asdm {
	CalCurveRow::~CalCurveRow() {
	}

	/**
	 * Return the table to which this row belongs.
	 */
	CalCurveTable &CalCurveRow::getTable() const {
		return table;
	}

	bool CalCurveRow::isAdded() const {
		return hasBeenAdded;
	}	

	void CalCurveRow::isAdded(bool added) {
		hasBeenAdded = added;
	}
	
#ifndef WITHOUT_ACS
	using asdmIDL::CalCurveRowIDL;
#endif
	
#ifndef WITHOUT_ACS
	/**
	 * Return this row in the form of an IDL struct.
	 * @return The values of this row as a CalCurveRowIDL struct.
	 */
	CalCurveRowIDL *CalCurveRow::toIDL() const {
		CalCurveRowIDL *x = new CalCurveRowIDL ();
		
		// Fill the IDL structure.
	
		
	
  		
		
		
			
				
		x->atmPhaseCorrection = atmPhaseCorrection;
 				
 			
		
	

	
  		
		
		
			
				
		x->typeCurve = typeCurve;
 				
 			
		
	

	
  		
		
		
			
				
		x->receiverBand = receiverBand;
 				
 			
		
	

	
  		
		
		
			
		x->startValidTime = startValidTime.toIDLArrayTime();
			
		
	

	
  		
		
		
			
		x->endValidTime = endValidTime.toIDLArrayTime();
			
		
	

	
  		
		
		
			
		x->frequencyRange.length(frequencyRange.size());
		for (unsigned int i = 0; i < frequencyRange.size(); ++i) {
			
			x->frequencyRange[i] = frequencyRange.at(i).toIDLFrequency();
			
	 	}
			
		
	

	
  		
		
		
			
				
		x->numAntenna = numAntenna;
 				
 			
		
	

	
  		
		
		
			
				
		x->numPoly = numPoly;
 				
 			
		
	

	
  		
		
		
			
				
		x->numReceptor = numReceptor;
 				
 			
		
	

	
  		
		
		
			
		x->antennaNames.length(antennaNames.size());
		for (unsigned int i = 0; i < antennaNames.size(); ++i) {
			
				
			x->antennaNames[i] = CORBA::string_dup(antennaNames.at(i).c_str());
				
	 		
	 	}
			
		
	

	
  		
		
		
			
				
		x->refAntennaName = CORBA::string_dup(refAntennaName.c_str());
				
 			
		
	

	
  		
		
		
			
		x->polarizationTypes.length(polarizationTypes.size());
		for (unsigned int i = 0; i < polarizationTypes.size(); ++i) {
			
				
			x->polarizationTypes[i] = polarizationTypes.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		
			
		x->curve.length(curve.size());
		for (unsigned int i = 0; i < curve.size(); i++) {
			x->curve[i].length(curve.at(i).size());
			for (unsigned int j = 0; j < curve.at(i).size(); j++) {
				x->curve[i][j].length(curve.at(i).at(j).size());
			}					 		
		}
		
		for (unsigned int i = 0; i < curve.size() ; i++)
			for (unsigned int j = 0; j < curve.at(i).size(); j++)
				for (unsigned int k = 0; k < curve.at(i).at(j).size(); k++)
					
						
					x->curve[i][j][k] = curve.at(i).at(j).at(k);
		 				
			 									
			
		
	

	
  		
		
		
			
		x->reducedChiSquared.length(reducedChiSquared.size());
		for (unsigned int i = 0; i < reducedChiSquared.size(); ++i) {
			
				
			x->reducedChiSquared[i] = reducedChiSquared.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		x->numBaselineExists = numBaselineExists;
		
		
			
				
		x->numBaseline = numBaseline;
 				
 			
		
	

	
  		
		
		x->rmsExists = rmsExists;
		
		
			
		x->rms.length(rms.size());
		for (unsigned int i = 0; i < rms.size(); i++) {
			x->rms[i].length(rms.at(i).size());			 		
		}
		
		for (unsigned int i = 0; i < rms.size() ; i++)
			for (unsigned int j = 0; j < rms.at(i).size(); j++)
					
						
				x->rms[i][j] = rms.at(i).at(j);
		 				
			 						
		
			
		
	

	
	
		
	
  	
 		
		
	 	
			
		x->calDataId = calDataId.toIDLTag();
			
	 	 		
  	

	
  	
 		
		
	 	
			
		x->calReductionId = calReductionId.toIDLTag();
			
	 	 		
  	

	
		
	

	

		
		return x;
	
	}
	
	void CalCurveRow::toIDL(asdmIDL::CalCurveRowIDL& x) const {
		// Set the x's fields.
	
		
	
  		
		
		
			
				
		x.atmPhaseCorrection = atmPhaseCorrection;
 				
 			
		
	

	
  		
		
		
			
				
		x.typeCurve = typeCurve;
 				
 			
		
	

	
  		
		
		
			
				
		x.receiverBand = receiverBand;
 				
 			
		
	

	
  		
		
		
			
		x.startValidTime = startValidTime.toIDLArrayTime();
			
		
	

	
  		
		
		
			
		x.endValidTime = endValidTime.toIDLArrayTime();
			
		
	

	
  		
		
		
			
		x.frequencyRange.length(frequencyRange.size());
		for (unsigned int i = 0; i < frequencyRange.size(); ++i) {
			
			x.frequencyRange[i] = frequencyRange.at(i).toIDLFrequency();
			
	 	}
			
		
	

	
  		
		
		
			
				
		x.numAntenna = numAntenna;
 				
 			
		
	

	
  		
		
		
			
				
		x.numPoly = numPoly;
 				
 			
		
	

	
  		
		
		
			
				
		x.numReceptor = numReceptor;
 				
 			
		
	

	
  		
		
		
			
		x.antennaNames.length(antennaNames.size());
		for (unsigned int i = 0; i < antennaNames.size(); ++i) {
			
				
			x.antennaNames[i] = CORBA::string_dup(antennaNames.at(i).c_str());
				
	 		
	 	}
			
		
	

	
  		
		
		
			
				
		x.refAntennaName = CORBA::string_dup(refAntennaName.c_str());
				
 			
		
	

	
  		
		
		
			
		x.polarizationTypes.length(polarizationTypes.size());
		for (unsigned int i = 0; i < polarizationTypes.size(); ++i) {
			
				
			x.polarizationTypes[i] = polarizationTypes.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		
			
		x.curve.length(curve.size());
		for (unsigned int i = 0; i < curve.size(); i++) {
			x.curve[i].length(curve.at(i).size());
			for (unsigned int j = 0; j < curve.at(i).size(); j++) {
				x.curve[i][j].length(curve.at(i).at(j).size());
			}					 		
		}
		
		for (unsigned int i = 0; i < curve.size() ; i++)
			for (unsigned int j = 0; j < curve.at(i).size(); j++)
				for (unsigned int k = 0; k < curve.at(i).at(j).size(); k++)
					
						
					x.curve[i][j][k] = curve.at(i).at(j).at(k);
		 				
			 									
			
		
	

	
  		
		
		
			
		x.reducedChiSquared.length(reducedChiSquared.size());
		for (unsigned int i = 0; i < reducedChiSquared.size(); ++i) {
			
				
			x.reducedChiSquared[i] = reducedChiSquared.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		x.numBaselineExists = numBaselineExists;
		
		
			
				
		x.numBaseline = numBaseline;
 				
 			
		
	

	
  		
		
		x.rmsExists = rmsExists;
		
		
			
		x.rms.length(rms.size());
		for (unsigned int i = 0; i < rms.size(); i++) {
			x.rms[i].length(rms.at(i).size());			 		
		}
		
		for (unsigned int i = 0; i < rms.size() ; i++)
			for (unsigned int j = 0; j < rms.at(i).size(); j++)
					
						
				x.rms[i][j] = rms.at(i).at(j);
		 				
			 						
		
			
		
	

	
	
		
	
  	
 		
		
	 	
			
		x.calDataId = calDataId.toIDLTag();
			
	 	 		
  	

	
  	
 		
		
	 	
			
		x.calReductionId = calReductionId.toIDLTag();
			
	 	 		
  	

	
		
	

	

	
	}
#endif
	

#ifndef WITHOUT_ACS
	/**
	 * Fill the values of this row from the IDL struct CalCurveRowIDL.
	 * @param x The IDL struct containing the values used to fill this row.
	 */
	void CalCurveRow::setFromIDL (CalCurveRowIDL x){
		try {
		// Fill the values from x.
	
		
	
		
		
			
		setAtmPhaseCorrection(x.atmPhaseCorrection);
  			
 		
		
	

	
		
		
			
		setTypeCurve(x.typeCurve);
  			
 		
		
	

	
		
		
			
		setReceiverBand(x.receiverBand);
  			
 		
		
	

	
		
		
			
		setStartValidTime(ArrayTime (x.startValidTime));
			
 		
		
	

	
		
		
			
		setEndValidTime(ArrayTime (x.endValidTime));
			
 		
		
	

	
		
		
			
		frequencyRange .clear();
		for (unsigned int i = 0; i <x.frequencyRange.length(); ++i) {
			
			frequencyRange.push_back(Frequency (x.frequencyRange[i]));
			
		}
			
  		
		
	

	
		
		
			
		setNumAntenna(x.numAntenna);
  			
 		
		
	

	
		
		
			
		setNumPoly(x.numPoly);
  			
 		
		
	

	
		
		
			
		setNumReceptor(x.numReceptor);
  			
 		
		
	

	
		
		
			
		antennaNames .clear();
		for (unsigned int i = 0; i <x.antennaNames.length(); ++i) {
			
			antennaNames.push_back(string (x.antennaNames[i]));
			
		}
			
  		
		
	

	
		
		
			
		setRefAntennaName(string (x.refAntennaName));
			
 		
		
	

	
		
		
			
		polarizationTypes .clear();
		for (unsigned int i = 0; i <x.polarizationTypes.length(); ++i) {
			
			polarizationTypes.push_back(x.polarizationTypes[i]);
  			
		}
			
  		
		
	

	
		
		
			
		curve .clear();
		vector< vector<float> > vv_aux_curve;
		vector<float> v_aux_curve;
		
		for (unsigned int i = 0; i < x.curve.length(); ++i) {
			vv_aux_curve.clear();
			for (unsigned int j = 0; j < x.curve[0].length(); ++j) {
				v_aux_curve.clear();
				for (unsigned int k = 0; k < x.curve[0][0].length(); ++k) {
					
					v_aux_curve.push_back(x.curve[i][j][k]);
		  			
		  		}
		  		vv_aux_curve.push_back(v_aux_curve);
  			}
  			curve.push_back(vv_aux_curve);
		}
			
  		
		
	

	
		
		
			
		reducedChiSquared .clear();
		for (unsigned int i = 0; i <x.reducedChiSquared.length(); ++i) {
			
			reducedChiSquared.push_back(x.reducedChiSquared[i]);
  			
		}
			
  		
		
	

	
		
		numBaselineExists = x.numBaselineExists;
		if (x.numBaselineExists) {
		
		
			
		setNumBaseline(x.numBaseline);
  			
 		
		
		}
		
	

	
		
		rmsExists = x.rmsExists;
		if (x.rmsExists) {
		
		
			
		rms .clear();
        
        vector<float> v_aux_rms;
        
		for (unsigned int i = 0; i < x.rms.length(); ++i) {
			v_aux_rms.clear();
			for (unsigned int j = 0; j < x.rms[0].length(); ++j) {
				
				v_aux_rms.push_back(x.rms[i][j]);
	  			
  			}
  			rms.push_back(v_aux_rms);			
		}
			
  		
		
		}
		
	

	
	
		
	
		
		
			
		setCalDataId(Tag (x.calDataId));
			
 		
		
	

	
		
		
			
		setCalReductionId(Tag (x.calReductionId));
			
 		
		
	

	
		
	

	

		} catch (const IllegalAccessException &err) {
			throw ConversionException (err.getMessage(),"CalCurve");
		}
	}
#endif
	
	/**
	 * Return this row in the form of an XML string.
	 * @return The values of this row as an XML string.
	 */
	string CalCurveRow::toXML() const {
		string buf;
		buf.append("<row> \n");
		
	
		
  	
 		
		
			buf.append(EnumerationParser::toXML("atmPhaseCorrection", atmPhaseCorrection));
		
		
	

  	
 		
		
			buf.append(EnumerationParser::toXML("typeCurve", typeCurve));
		
		
	

  	
 		
		
			buf.append(EnumerationParser::toXML("receiverBand", receiverBand));
		
		
	

  	
 		
		
		Parser::toXML(startValidTime, "startValidTime", buf);
		
		
	

  	
 		
		
		Parser::toXML(endValidTime, "endValidTime", buf);
		
		
	

  	
 		
		
		Parser::toXML(frequencyRange, "frequencyRange", buf);
		
		
	

  	
 		
		
		Parser::toXML(numAntenna, "numAntenna", buf);
		
		
	

  	
 		
		
		Parser::toXML(numPoly, "numPoly", buf);
		
		
	

  	
 		
		
		Parser::toXML(numReceptor, "numReceptor", buf);
		
		
	

  	
 		
		
		Parser::toXML(antennaNames, "antennaNames", buf);
		
		
	

  	
 		
		
		Parser::toXML(refAntennaName, "refAntennaName", buf);
		
		
	

  	
 		
		
			buf.append(EnumerationParser::toXML("polarizationTypes", polarizationTypes));
		
		
	

  	
 		
		
		Parser::toXML(curve, "curve", buf);
		
		
	

  	
 		
		
		Parser::toXML(reducedChiSquared, "reducedChiSquared", buf);
		
		
	

  	
 		
		if (numBaselineExists) {
		
		
		Parser::toXML(numBaseline, "numBaseline", buf);
		
		
		}
		
	

  	
 		
		if (rmsExists) {
		
		
		Parser::toXML(rms, "rms", buf);
		
		
		}
		
	

	
	
		
  	
 		
		
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
	void CalCurveRow::setFromXML (string rowDoc) {
		Parser row(rowDoc);
		string s = "";
		try {
	
		
	
		
		
		
		atmPhaseCorrection = EnumerationParser::getAtmPhaseCorrection("atmPhaseCorrection","CalCurve",rowDoc);
		
		
		
	

	
		
		
		
		typeCurve = EnumerationParser::getCalCurveType("typeCurve","CalCurve",rowDoc);
		
		
		
	

	
		
		
		
		receiverBand = EnumerationParser::getReceiverBand("receiverBand","CalCurve",rowDoc);
		
		
		
	

	
  		
			
	  	setStartValidTime(Parser::getArrayTime("startValidTime","CalCurve",rowDoc));
			
		
	

	
  		
			
	  	setEndValidTime(Parser::getArrayTime("endValidTime","CalCurve",rowDoc));
			
		
	

	
  		
			
					
	  	setFrequencyRange(Parser::get1DFrequency("frequencyRange","CalCurve",rowDoc));
	  			
	  		
		
	

	
  		
			
	  	setNumAntenna(Parser::getInteger("numAntenna","CalCurve",rowDoc));
			
		
	

	
  		
			
	  	setNumPoly(Parser::getInteger("numPoly","CalCurve",rowDoc));
			
		
	

	
  		
			
	  	setNumReceptor(Parser::getInteger("numReceptor","CalCurve",rowDoc));
			
		
	

	
  		
			
					
	  	setAntennaNames(Parser::get1DString("antennaNames","CalCurve",rowDoc));
	  			
	  		
		
	

	
  		
			
	  	setRefAntennaName(Parser::getString("refAntennaName","CalCurve",rowDoc));
			
		
	

	
		
		
		
		polarizationTypes = EnumerationParser::getPolarizationType1D("polarizationTypes","CalCurve",rowDoc);			
		
		
		
	

	
  		
			
					
	  	setCurve(Parser::get3DFloat("curve","CalCurve",rowDoc));
	  			
	  		
		
	

	
  		
			
					
	  	setReducedChiSquared(Parser::get1DDouble("reducedChiSquared","CalCurve",rowDoc));
	  			
	  		
		
	

	
  		
        if (row.isStr("<numBaseline>")) {
			
	  		setNumBaseline(Parser::getInteger("numBaseline","CalCurve",rowDoc));
			
		}
 		
	

	
  		
        if (row.isStr("<rms>")) {
			
								
	  		setRms(Parser::get2DFloat("rms","CalCurve",rowDoc));
	  			
	  		
		}
 		
	

	
	
		
	
  		
			
	  	setCalDataId(Parser::getTag("calDataId","CalData",rowDoc));
			
		
	

	
  		
			
	  	setCalReductionId(Parser::getTag("calReductionId","CalReduction",rowDoc));
			
		
	

	
		
	

	

		} catch (const IllegalAccessException &err) {
			throw ConversionException (err.getMessage(),"CalCurve");
		}
	}
	
	void CalCurveRow::toBin(EndianOSStream& eoss) {
	
	
	
	
		
					
		eoss.writeString(CAtmPhaseCorrection::name(atmPhaseCorrection));
			/* eoss.writeInt(atmPhaseCorrection); */
				
		
	

	
	
		
					
		eoss.writeString(CCalCurveType::name(typeCurve));
			/* eoss.writeInt(typeCurve); */
				
		
	

	
	
		
					
		eoss.writeString(CReceiverBand::name(receiverBand));
			/* eoss.writeInt(receiverBand); */
				
		
	

	
	
		
	calDataId.toBin(eoss);
		
	

	
	
		
	calReductionId.toBin(eoss);
		
	

	
	
		
	startValidTime.toBin(eoss);
		
	

	
	
		
	endValidTime.toBin(eoss);
		
	

	
	
		
	Frequency::toBin(frequencyRange, eoss);
		
	

	
	
		
						
			eoss.writeInt(numAntenna);
				
		
	

	
	
		
						
			eoss.writeInt(numPoly);
				
		
	

	
	
		
						
			eoss.writeInt(numReceptor);
				
		
	

	
	
		
		
			
		eoss.writeInt((int) antennaNames.size());
		for (unsigned int i = 0; i < antennaNames.size(); i++)
				
			eoss.writeString(antennaNames.at(i));
				
				
						
		
	

	
	
		
						
			eoss.writeString(refAntennaName);
				
		
	

	
	
		
		
			
		eoss.writeInt((int) polarizationTypes.size());
		for (unsigned int i = 0; i < polarizationTypes.size(); i++)
				
			eoss.writeString(CPolarizationType::name(polarizationTypes.at(i)));
			/* eoss.writeInt(polarizationTypes.at(i)); */
				
				
						
		
	

	
	
		
		
			
		eoss.writeInt((int) curve.size());
		eoss.writeInt((int) curve.at(0).size());		
		eoss.writeInt((int) curve.at(0).at(0).size());
		for (unsigned int i = 0; i < curve.size(); i++) 
			for (unsigned int j = 0;  j < curve.at(0).size(); j++)
				for (unsigned int k = 0; k <  curve.at(0).at(0).size(); k++)	
							 
					eoss.writeFloat(curve.at(i).at(j).at(k));
						
						
		
	

	
	
		
		
			
		eoss.writeInt((int) reducedChiSquared.size());
		for (unsigned int i = 0; i < reducedChiSquared.size(); i++)
				
			eoss.writeDouble(reducedChiSquared.at(i));
				
				
						
		
	


	
	
	eoss.writeBoolean(numBaselineExists);
	if (numBaselineExists) {
	
	
	
		
						
			eoss.writeInt(numBaseline);
				
		
	

	}

	eoss.writeBoolean(rmsExists);
	if (rmsExists) {
	
	
	
		
		
			
		eoss.writeInt((int) rms.size());
		eoss.writeInt((int) rms.at(0).size());
		for (unsigned int i = 0; i < rms.size(); i++) 
			for (unsigned int j = 0;  j < rms.at(0).size(); j++) 
							 
				eoss.writeFloat(rms.at(i).at(j));
				
	
						
		
	

	}

	}
	
void CalCurveRow::atmPhaseCorrectionFromBin(EndianIStream& eis) {
		
	
	
		
			
		atmPhaseCorrection = CAtmPhaseCorrection::literal(eis.readString());
			
		
	
	
}
void CalCurveRow::typeCurveFromBin(EndianIStream& eis) {
		
	
	
		
			
		typeCurve = CCalCurveType::literal(eis.readString());
			
		
	
	
}
void CalCurveRow::receiverBandFromBin(EndianIStream& eis) {
		
	
	
		
			
		receiverBand = CReceiverBand::literal(eis.readString());
			
		
	
	
}
void CalCurveRow::calDataIdFromBin(EndianIStream& eis) {
		
	
		
		
		calDataId =  Tag::fromBin(eis);
		
	
	
}
void CalCurveRow::calReductionIdFromBin(EndianIStream& eis) {
		
	
		
		
		calReductionId =  Tag::fromBin(eis);
		
	
	
}
void CalCurveRow::startValidTimeFromBin(EndianIStream& eis) {
		
	
		
		
		startValidTime =  ArrayTime::fromBin(eis);
		
	
	
}
void CalCurveRow::endValidTimeFromBin(EndianIStream& eis) {
		
	
		
		
		endValidTime =  ArrayTime::fromBin(eis);
		
	
	
}
void CalCurveRow::frequencyRangeFromBin(EndianIStream& eis) {
		
	
		
		
			
	
	frequencyRange = Frequency::from1DBin(eis);	
	

		
	
	
}
void CalCurveRow::numAntennaFromBin(EndianIStream& eis) {
		
	
	
		
			
		numAntenna =  eis.readInt();
			
		
	
	
}
void CalCurveRow::numPolyFromBin(EndianIStream& eis) {
		
	
	
		
			
		numPoly =  eis.readInt();
			
		
	
	
}
void CalCurveRow::numReceptorFromBin(EndianIStream& eis) {
		
	
	
		
			
		numReceptor =  eis.readInt();
			
		
	
	
}
void CalCurveRow::antennaNamesFromBin(EndianIStream& eis) {
		
	
	
		
			
	
		antennaNames.clear();
		
		unsigned int antennaNamesDim1 = eis.readInt();
		for (unsigned int  i = 0 ; i < antennaNamesDim1; i++)
			
			antennaNames.push_back(eis.readString());
			
	

		
	
	
}
void CalCurveRow::refAntennaNameFromBin(EndianIStream& eis) {
		
	
	
		
			
		refAntennaName =  eis.readString();
			
		
	
	
}
void CalCurveRow::polarizationTypesFromBin(EndianIStream& eis) {
		
	
	
		
			
	
		polarizationTypes.clear();
		
		unsigned int polarizationTypesDim1 = eis.readInt();
		for (unsigned int  i = 0 ; i < polarizationTypesDim1; i++)
			
			polarizationTypes.push_back(CPolarizationType::literal(eis.readString()));
			
	

		
	
	
}
void CalCurveRow::curveFromBin(EndianIStream& eis) {
		
	
	
		
			
	
		curve.clear();
			
		unsigned int curveDim1 = eis.readInt();
		unsigned int curveDim2 = eis.readInt();
		unsigned int curveDim3 = eis.readInt();
		vector <vector<float> > curveAux2;
		vector <float> curveAux1;
		for (unsigned int i = 0; i < curveDim1; i++) {
			curveAux2.clear();
			for (unsigned int j = 0; j < curveDim2 ; j++) {
				curveAux1.clear();
				for (unsigned int k = 0; k < curveDim3; k++) {
			
					curveAux1.push_back(eis.readFloat());
			
				}
				curveAux2.push_back(curveAux1);
			}
			curve.push_back(curveAux2);
		}	
	

		
	
	
}
void CalCurveRow::reducedChiSquaredFromBin(EndianIStream& eis) {
		
	
	
		
			
	
		reducedChiSquared.clear();
		
		unsigned int reducedChiSquaredDim1 = eis.readInt();
		for (unsigned int  i = 0 ; i < reducedChiSquaredDim1; i++)
			
			reducedChiSquared.push_back(eis.readDouble());
			
	

		
	
	
}

void CalCurveRow::numBaselineFromBin(EndianIStream& eis) {
		
	numBaselineExists = eis.readBoolean();
	if (numBaselineExists) {
		
	
	
		
			
		numBaseline =  eis.readInt();
			
		
	

	}
	
}
void CalCurveRow::rmsFromBin(EndianIStream& eis) {
		
	rmsExists = eis.readBoolean();
	if (rmsExists) {
		
	
	
		
			
	
		rms.clear();
		
		unsigned int rmsDim1 = eis.readInt();
		unsigned int rmsDim2 = eis.readInt();
        
		vector <float> rmsAux1;
        
		for (unsigned int i = 0; i < rmsDim1; i++) {
			rmsAux1.clear();
			for (unsigned int j = 0; j < rmsDim2 ; j++)			
			
			rmsAux1.push_back(eis.readFloat());
			
			rms.push_back(rmsAux1);
		}
	
	

		
	

	}
	
}
	
	
	CalCurveRow* CalCurveRow::fromBin(EndianIStream& eis, CalCurveTable& table, const vector<string>& attributesSeq) {
		CalCurveRow* row = new  CalCurveRow(table);
		
		map<string, CalCurveAttributeFromBin>::iterator iter ;
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
					throw ConversionException("There is not method to read an attribute '"+attributesSeq.at(i)+"'.", "CalCurveTable");
			}
				
		}				
		return row;
	}

	//
	// A collection of methods to set the value of the attributes from their textual value in the XML representation
	// of one row.
	//
	
	// Convert a string into an AtmPhaseCorrection 
	void CalCurveRow::atmPhaseCorrectionFromText(const string & s) {
		 
          
		atmPhaseCorrection = ASDMValuesParser::parse<AtmPhaseCorrectionMod::AtmPhaseCorrection>(s);
          
		
	}
	
	
	// Convert a string into an CalCurveType 
	void CalCurveRow::typeCurveFromText(const string & s) {
		 
          
		typeCurve = ASDMValuesParser::parse<CalCurveTypeMod::CalCurveType>(s);
          
		
	}
	
	
	// Convert a string into an ReceiverBand 
	void CalCurveRow::receiverBandFromText(const string & s) {
		 
          
		receiverBand = ASDMValuesParser::parse<ReceiverBandMod::ReceiverBand>(s);
          
		
	}
	
	
	// Convert a string into an Tag 
	void CalCurveRow::calDataIdFromText(const string & s) {
		 
          
		calDataId = ASDMValuesParser::parse<Tag>(s);
          
		
	}
	
	
	// Convert a string into an Tag 
	void CalCurveRow::calReductionIdFromText(const string & s) {
		 
          
		calReductionId = ASDMValuesParser::parse<Tag>(s);
          
		
	}
	
	
	// Convert a string into an ArrayTime 
	void CalCurveRow::startValidTimeFromText(const string & s) {
		 
          
		startValidTime = ASDMValuesParser::parse<ArrayTime>(s);
          
		
	}
	
	
	// Convert a string into an ArrayTime 
	void CalCurveRow::endValidTimeFromText(const string & s) {
		 
          
		endValidTime = ASDMValuesParser::parse<ArrayTime>(s);
          
		
	}
	
	
	// Convert a string into an Frequency 
	void CalCurveRow::frequencyRangeFromText(const string & s) {
		 
          
		frequencyRange = ASDMValuesParser::parse1D<Frequency>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void CalCurveRow::numAntennaFromText(const string & s) {
		 
          
		numAntenna = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void CalCurveRow::numPolyFromText(const string & s) {
		 
          
		numPoly = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void CalCurveRow::numReceptorFromText(const string & s) {
		 
          
		numReceptor = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	// Convert a string into an String 
	void CalCurveRow::antennaNamesFromText(const string & s) {
		 
          
		antennaNames = ASDMValuesParser::parse1D<string>(s);
          
		
	}
	
	
	// Convert a string into an String 
	void CalCurveRow::refAntennaNameFromText(const string & s) {
		 
          
		refAntennaName = ASDMValuesParser::parse<string>(s);
          
		
	}
	
	
	// Convert a string into an PolarizationType 
	void CalCurveRow::polarizationTypesFromText(const string & s) {
		 
          
		polarizationTypes = ASDMValuesParser::parse1D<PolarizationTypeMod::PolarizationType>(s);
          
		
	}
	
	
	// Convert a string into an float 
	void CalCurveRow::curveFromText(const string & s) {
		 
          
		curve = ASDMValuesParser::parse3D<float>(s);
          
		
	}
	
	
	// Convert a string into an double 
	void CalCurveRow::reducedChiSquaredFromText(const string & s) {
		 
          
		reducedChiSquared = ASDMValuesParser::parse1D<double>(s);
          
		
	}
	

	
	// Convert a string into an int 
	void CalCurveRow::numBaselineFromText(const string & s) {
		numBaselineExists = true;
		 
          
		numBaseline = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	// Convert a string into an float 
	void CalCurveRow::rmsFromText(const string & s) {
		rmsExists = true;
		 
          
		rms = ASDMValuesParser::parse2D<float>(s);
          
		
	}
	
	
	
	void CalCurveRow::fromText(const std::string& attributeName, const std::string&  t) {
		map<string, CalCurveAttributeFromText>::iterator iter;
		if ((iter = fromTextMethods.find(attributeName)) == fromTextMethods.end())
			throw ConversionException("I do not know what to do with '"+attributeName+"' and its content '"+t+"' (while parsing an XML document)", "CalCurveTable");
		(this->*(iter->second))(t);
	}
			
	////////////////////////////////////////////////
	// Intrinsic Table Attributes getters/setters //
	////////////////////////////////////////////////
	
	

	
 	/**
 	 * Get atmPhaseCorrection.
 	 * @return atmPhaseCorrection as AtmPhaseCorrectionMod::AtmPhaseCorrection
 	 */
 	AtmPhaseCorrectionMod::AtmPhaseCorrection CalCurveRow::getAtmPhaseCorrection() const {
	
  		return atmPhaseCorrection;
 	}

 	/**
 	 * Set atmPhaseCorrection with the specified AtmPhaseCorrectionMod::AtmPhaseCorrection.
 	 * @param atmPhaseCorrection The AtmPhaseCorrectionMod::AtmPhaseCorrection value to which atmPhaseCorrection is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void CalCurveRow::setAtmPhaseCorrection (AtmPhaseCorrectionMod::AtmPhaseCorrection atmPhaseCorrection)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("atmPhaseCorrection", "CalCurve");
		
  		}
  	
 		this->atmPhaseCorrection = atmPhaseCorrection;
	
 	}
	
	

	

	
 	/**
 	 * Get typeCurve.
 	 * @return typeCurve as CalCurveTypeMod::CalCurveType
 	 */
 	CalCurveTypeMod::CalCurveType CalCurveRow::getTypeCurve() const {
	
  		return typeCurve;
 	}

 	/**
 	 * Set typeCurve with the specified CalCurveTypeMod::CalCurveType.
 	 * @param typeCurve The CalCurveTypeMod::CalCurveType value to which typeCurve is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void CalCurveRow::setTypeCurve (CalCurveTypeMod::CalCurveType typeCurve)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("typeCurve", "CalCurve");
		
  		}
  	
 		this->typeCurve = typeCurve;
	
 	}
	
	

	

	
 	/**
 	 * Get receiverBand.
 	 * @return receiverBand as ReceiverBandMod::ReceiverBand
 	 */
 	ReceiverBandMod::ReceiverBand CalCurveRow::getReceiverBand() const {
	
  		return receiverBand;
 	}

 	/**
 	 * Set receiverBand with the specified ReceiverBandMod::ReceiverBand.
 	 * @param receiverBand The ReceiverBandMod::ReceiverBand value to which receiverBand is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void CalCurveRow::setReceiverBand (ReceiverBandMod::ReceiverBand receiverBand)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("receiverBand", "CalCurve");
		
  		}
  	
 		this->receiverBand = receiverBand;
	
 	}
	
	

	

	
 	/**
 	 * Get startValidTime.
 	 * @return startValidTime as ArrayTime
 	 */
 	ArrayTime CalCurveRow::getStartValidTime() const {
	
  		return startValidTime;
 	}

 	/**
 	 * Set startValidTime with the specified ArrayTime.
 	 * @param startValidTime The ArrayTime value to which startValidTime is to be set.
 	 
 	
 		
 	 */
 	void CalCurveRow::setStartValidTime (ArrayTime startValidTime)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->startValidTime = startValidTime;
	
 	}
	
	

	

	
 	/**
 	 * Get endValidTime.
 	 * @return endValidTime as ArrayTime
 	 */
 	ArrayTime CalCurveRow::getEndValidTime() const {
	
  		return endValidTime;
 	}

 	/**
 	 * Set endValidTime with the specified ArrayTime.
 	 * @param endValidTime The ArrayTime value to which endValidTime is to be set.
 	 
 	
 		
 	 */
 	void CalCurveRow::setEndValidTime (ArrayTime endValidTime)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->endValidTime = endValidTime;
	
 	}
	
	

	

	
 	/**
 	 * Get frequencyRange.
 	 * @return frequencyRange as std::vector<Frequency >
 	 */
 	std::vector<Frequency > CalCurveRow::getFrequencyRange() const {
	
  		return frequencyRange;
 	}

 	/**
 	 * Set frequencyRange with the specified std::vector<Frequency >.
 	 * @param frequencyRange The std::vector<Frequency > value to which frequencyRange is to be set.
 	 
 	
 		
 	 */
 	void CalCurveRow::setFrequencyRange (std::vector<Frequency > frequencyRange)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->frequencyRange = frequencyRange;
	
 	}
	
	

	

	
 	/**
 	 * Get numAntenna.
 	 * @return numAntenna as int
 	 */
 	int CalCurveRow::getNumAntenna() const {
	
  		return numAntenna;
 	}

 	/**
 	 * Set numAntenna with the specified int.
 	 * @param numAntenna The int value to which numAntenna is to be set.
 	 
 	
 		
 	 */
 	void CalCurveRow::setNumAntenna (int numAntenna)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->numAntenna = numAntenna;
	
 	}
	
	

	

	
 	/**
 	 * Get numPoly.
 	 * @return numPoly as int
 	 */
 	int CalCurveRow::getNumPoly() const {
	
  		return numPoly;
 	}

 	/**
 	 * Set numPoly with the specified int.
 	 * @param numPoly The int value to which numPoly is to be set.
 	 
 	
 		
 	 */
 	void CalCurveRow::setNumPoly (int numPoly)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->numPoly = numPoly;
	
 	}
	
	

	

	
 	/**
 	 * Get numReceptor.
 	 * @return numReceptor as int
 	 */
 	int CalCurveRow::getNumReceptor() const {
	
  		return numReceptor;
 	}

 	/**
 	 * Set numReceptor with the specified int.
 	 * @param numReceptor The int value to which numReceptor is to be set.
 	 
 	
 		
 	 */
 	void CalCurveRow::setNumReceptor (int numReceptor)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->numReceptor = numReceptor;
	
 	}
	
	

	

	
 	/**
 	 * Get antennaNames.
 	 * @return antennaNames as std::vector<std::string >
 	 */
 	std::vector<std::string > CalCurveRow::getAntennaNames() const {
	
  		return antennaNames;
 	}

 	/**
 	 * Set antennaNames with the specified std::vector<std::string >.
 	 * @param antennaNames The std::vector<std::string > value to which antennaNames is to be set.
 	 
 	
 		
 	 */
 	void CalCurveRow::setAntennaNames (std::vector<std::string > antennaNames)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->antennaNames = antennaNames;
	
 	}
	
	

	

	
 	/**
 	 * Get refAntennaName.
 	 * @return refAntennaName as std::string
 	 */
 	std::string CalCurveRow::getRefAntennaName() const {
	
  		return refAntennaName;
 	}

 	/**
 	 * Set refAntennaName with the specified std::string.
 	 * @param refAntennaName The std::string value to which refAntennaName is to be set.
 	 
 	
 		
 	 */
 	void CalCurveRow::setRefAntennaName (std::string refAntennaName)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->refAntennaName = refAntennaName;
	
 	}
	
	

	

	
 	/**
 	 * Get polarizationTypes.
 	 * @return polarizationTypes as std::vector<PolarizationTypeMod::PolarizationType >
 	 */
 	std::vector<PolarizationTypeMod::PolarizationType > CalCurveRow::getPolarizationTypes() const {
	
  		return polarizationTypes;
 	}

 	/**
 	 * Set polarizationTypes with the specified std::vector<PolarizationTypeMod::PolarizationType >.
 	 * @param polarizationTypes The std::vector<PolarizationTypeMod::PolarizationType > value to which polarizationTypes is to be set.
 	 
 	
 		
 	 */
 	void CalCurveRow::setPolarizationTypes (std::vector<PolarizationTypeMod::PolarizationType > polarizationTypes)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->polarizationTypes = polarizationTypes;
	
 	}
	
	

	

	
 	/**
 	 * Get curve.
 	 * @return curve as std::vector<std::vector<std::vector<float > > >
 	 */
 	std::vector<std::vector<std::vector<float > > > CalCurveRow::getCurve() const {
	
  		return curve;
 	}

 	/**
 	 * Set curve with the specified std::vector<std::vector<std::vector<float > > >.
 	 * @param curve The std::vector<std::vector<std::vector<float > > > value to which curve is to be set.
 	 
 	
 		
 	 */
 	void CalCurveRow::setCurve (std::vector<std::vector<std::vector<float > > > curve)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->curve = curve;
	
 	}
	
	

	

	
 	/**
 	 * Get reducedChiSquared.
 	 * @return reducedChiSquared as std::vector<double >
 	 */
 	std::vector<double > CalCurveRow::getReducedChiSquared() const {
	
  		return reducedChiSquared;
 	}

 	/**
 	 * Set reducedChiSquared with the specified std::vector<double >.
 	 * @param reducedChiSquared The std::vector<double > value to which reducedChiSquared is to be set.
 	 
 	
 		
 	 */
 	void CalCurveRow::setReducedChiSquared (std::vector<double > reducedChiSquared)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->reducedChiSquared = reducedChiSquared;
	
 	}
	
	

	
	/**
	 * The attribute numBaseline is optional. Return true if this attribute exists.
	 * @return true if and only if the numBaseline attribute exists. 
	 */
	bool CalCurveRow::isNumBaselineExists() const {
		return numBaselineExists;
	}
	

	
 	/**
 	 * Get numBaseline, which is optional.
 	 * @return numBaseline as int
 	 * @throw IllegalAccessException If numBaseline does not exist.
 	 */
 	int CalCurveRow::getNumBaseline() const  {
		if (!numBaselineExists) {
			throw IllegalAccessException("numBaseline", "CalCurve");
		}
	
  		return numBaseline;
 	}

 	/**
 	 * Set numBaseline with the specified int.
 	 * @param numBaseline The int value to which numBaseline is to be set.
 	 
 	
 	 */
 	void CalCurveRow::setNumBaseline (int numBaseline) {
	
 		this->numBaseline = numBaseline;
	
		numBaselineExists = true;
	
 	}
	
	
	/**
	 * Mark numBaseline, which is an optional field, as non-existent.
	 */
	void CalCurveRow::clearNumBaseline () {
		numBaselineExists = false;
	}
	

	
	/**
	 * The attribute rms is optional. Return true if this attribute exists.
	 * @return true if and only if the rms attribute exists. 
	 */
	bool CalCurveRow::isRmsExists() const {
		return rmsExists;
	}
	

	
 	/**
 	 * Get rms, which is optional.
 	 * @return rms as std::vector<std::vector<float > >
 	 * @throw IllegalAccessException If rms does not exist.
 	 */
 	std::vector<std::vector<float > > CalCurveRow::getRms() const  {
		if (!rmsExists) {
			throw IllegalAccessException("rms", "CalCurve");
		}
	
  		return rms;
 	}

 	/**
 	 * Set rms with the specified std::vector<std::vector<float > >.
 	 * @param rms The std::vector<std::vector<float > > value to which rms is to be set.
 	 
 	
 	 */
 	void CalCurveRow::setRms (std::vector<std::vector<float > > rms) {
	
 		this->rms = rms;
	
		rmsExists = true;
	
 	}
	
	
	/**
	 * Mark rms, which is an optional field, as non-existent.
	 */
	void CalCurveRow::clearRms () {
		rmsExists = false;
	}
	

	
	///////////////////////////////////////////////
	// Extrinsic Table Attributes getters/setters//
	///////////////////////////////////////////////
	
	

	
 	/**
 	 * Get calDataId.
 	 * @return calDataId as Tag
 	 */
 	Tag CalCurveRow::getCalDataId() const {
	
  		return calDataId;
 	}

 	/**
 	 * Set calDataId with the specified Tag.
 	 * @param calDataId The Tag value to which calDataId is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void CalCurveRow::setCalDataId (Tag calDataId)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("calDataId", "CalCurve");
		
  		}
  	
 		this->calDataId = calDataId;
	
 	}
	
	

	

	
 	/**
 	 * Get calReductionId.
 	 * @return calReductionId as Tag
 	 */
 	Tag CalCurveRow::getCalReductionId() const {
	
  		return calReductionId;
 	}

 	/**
 	 * Set calReductionId with the specified Tag.
 	 * @param calReductionId The Tag value to which calReductionId is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void CalCurveRow::setCalReductionId (Tag calReductionId)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("calReductionId", "CalCurve");
		
  		}
  	
 		this->calReductionId = calReductionId;
	
 	}
	
	


	//////////////////////////////////////
	// Links Attributes getters/setters //
	//////////////////////////////////////
	
	
	
	
		

	/**
	 * Returns the pointer to the row in the CalData table having CalData.calDataId == calDataId
	 * @return a CalDataRow*
	 * 
	 
	 */
	 CalDataRow* CalCurveRow::getCalDataUsingCalDataId() {
	 
	 	return table.getContainer().getCalData().getRowByKey(calDataId);
	 }
	 

	

	
	
	
		

	/**
	 * Returns the pointer to the row in the CalReduction table having CalReduction.calReductionId == calReductionId
	 * @return a CalReductionRow*
	 * 
	 
	 */
	 CalReductionRow* CalCurveRow::getCalReductionUsingCalReductionId() {
	 
	 	return table.getContainer().getCalReduction().getRowByKey(calReductionId);
	 }
	 

	

	
	/**
	 * Create a CalCurveRow.
	 * <p>
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 */ 
	CalCurveRow::CalCurveRow (CalCurveTable &t) : table(t) {
		hasBeenAdded = false;
		
	
	

	

	

	

	

	

	

	

	

	

	

	

	

	

	
		numBaselineExists = false;
	

	
		rmsExists = false;
	

	
	

	

	
	
	
	
// This attribute is scalar and has an enumeration type. Let's initialize it to some valid value (the 1st of the enumeration).		
atmPhaseCorrection = CAtmPhaseCorrection::from_int(0);
	

	
// This attribute is scalar and has an enumeration type. Let's initialize it to some valid value (the 1st of the enumeration).		
typeCurve = CCalCurveType::from_int(0);
	

	
// This attribute is scalar and has an enumeration type. Let's initialize it to some valid value (the 1st of the enumeration).		
receiverBand = CReceiverBand::from_int(0);
	

	

	

	

	

	

	

	

	

	

	

	

	

	

	
	
	 fromBinMethods["atmPhaseCorrection"] = &CalCurveRow::atmPhaseCorrectionFromBin; 
	 fromBinMethods["typeCurve"] = &CalCurveRow::typeCurveFromBin; 
	 fromBinMethods["receiverBand"] = &CalCurveRow::receiverBandFromBin; 
	 fromBinMethods["calDataId"] = &CalCurveRow::calDataIdFromBin; 
	 fromBinMethods["calReductionId"] = &CalCurveRow::calReductionIdFromBin; 
	 fromBinMethods["startValidTime"] = &CalCurveRow::startValidTimeFromBin; 
	 fromBinMethods["endValidTime"] = &CalCurveRow::endValidTimeFromBin; 
	 fromBinMethods["frequencyRange"] = &CalCurveRow::frequencyRangeFromBin; 
	 fromBinMethods["numAntenna"] = &CalCurveRow::numAntennaFromBin; 
	 fromBinMethods["numPoly"] = &CalCurveRow::numPolyFromBin; 
	 fromBinMethods["numReceptor"] = &CalCurveRow::numReceptorFromBin; 
	 fromBinMethods["antennaNames"] = &CalCurveRow::antennaNamesFromBin; 
	 fromBinMethods["refAntennaName"] = &CalCurveRow::refAntennaNameFromBin; 
	 fromBinMethods["polarizationTypes"] = &CalCurveRow::polarizationTypesFromBin; 
	 fromBinMethods["curve"] = &CalCurveRow::curveFromBin; 
	 fromBinMethods["reducedChiSquared"] = &CalCurveRow::reducedChiSquaredFromBin; 
		
	
	 fromBinMethods["numBaseline"] = &CalCurveRow::numBaselineFromBin; 
	 fromBinMethods["rms"] = &CalCurveRow::rmsFromBin; 
	
	
	
	
				 
	fromTextMethods["atmPhaseCorrection"] = &CalCurveRow::atmPhaseCorrectionFromText;
		 
	
				 
	fromTextMethods["typeCurve"] = &CalCurveRow::typeCurveFromText;
		 
	
				 
	fromTextMethods["receiverBand"] = &CalCurveRow::receiverBandFromText;
		 
	
				 
	fromTextMethods["calDataId"] = &CalCurveRow::calDataIdFromText;
		 
	
				 
	fromTextMethods["calReductionId"] = &CalCurveRow::calReductionIdFromText;
		 
	
				 
	fromTextMethods["startValidTime"] = &CalCurveRow::startValidTimeFromText;
		 
	
				 
	fromTextMethods["endValidTime"] = &CalCurveRow::endValidTimeFromText;
		 
	
				 
	fromTextMethods["frequencyRange"] = &CalCurveRow::frequencyRangeFromText;
		 
	
				 
	fromTextMethods["numAntenna"] = &CalCurveRow::numAntennaFromText;
		 
	
				 
	fromTextMethods["numPoly"] = &CalCurveRow::numPolyFromText;
		 
	
				 
	fromTextMethods["numReceptor"] = &CalCurveRow::numReceptorFromText;
		 
	
				 
	fromTextMethods["antennaNames"] = &CalCurveRow::antennaNamesFromText;
		 
	
				 
	fromTextMethods["refAntennaName"] = &CalCurveRow::refAntennaNameFromText;
		 
	
				 
	fromTextMethods["polarizationTypes"] = &CalCurveRow::polarizationTypesFromText;
		 
	
				 
	fromTextMethods["curve"] = &CalCurveRow::curveFromText;
		 
	
				 
	fromTextMethods["reducedChiSquared"] = &CalCurveRow::reducedChiSquaredFromText;
		 
	

	 
				
	fromTextMethods["numBaseline"] = &CalCurveRow::numBaselineFromText;
		 	
	 
				
	fromTextMethods["rms"] = &CalCurveRow::rmsFromText;
		 	
		
	}
	
	CalCurveRow::CalCurveRow (CalCurveTable &t, CalCurveRow *row) : table(t) {
		hasBeenAdded = false;
		
		if (row == 0) {
	
	
	

	

	

	

	

	

	

	

	

	

	

	

	

	

	
		numBaselineExists = false;
	

	
		rmsExists = false;
	

	
	

	
		
		}
		else {
	
		
			atmPhaseCorrection = row->atmPhaseCorrection;
		
			typeCurve = row->typeCurve;
		
			receiverBand = row->receiverBand;
		
			calDataId = row->calDataId;
		
			calReductionId = row->calReductionId;
		
		
		
		
			startValidTime = row->startValidTime;
		
			endValidTime = row->endValidTime;
		
			frequencyRange = row->frequencyRange;
		
			numAntenna = row->numAntenna;
		
			numPoly = row->numPoly;
		
			numReceptor = row->numReceptor;
		
			antennaNames = row->antennaNames;
		
			refAntennaName = row->refAntennaName;
		
			polarizationTypes = row->polarizationTypes;
		
			curve = row->curve;
		
			reducedChiSquared = row->reducedChiSquared;
		
		
		
		
		if (row->numBaselineExists) {
			numBaseline = row->numBaseline;		
			numBaselineExists = true;
		}
		else
			numBaselineExists = false;
		
		if (row->rmsExists) {
			rms = row->rms;		
			rmsExists = true;
		}
		else
			rmsExists = false;
		
		}
		
		 fromBinMethods["atmPhaseCorrection"] = &CalCurveRow::atmPhaseCorrectionFromBin; 
		 fromBinMethods["typeCurve"] = &CalCurveRow::typeCurveFromBin; 
		 fromBinMethods["receiverBand"] = &CalCurveRow::receiverBandFromBin; 
		 fromBinMethods["calDataId"] = &CalCurveRow::calDataIdFromBin; 
		 fromBinMethods["calReductionId"] = &CalCurveRow::calReductionIdFromBin; 
		 fromBinMethods["startValidTime"] = &CalCurveRow::startValidTimeFromBin; 
		 fromBinMethods["endValidTime"] = &CalCurveRow::endValidTimeFromBin; 
		 fromBinMethods["frequencyRange"] = &CalCurveRow::frequencyRangeFromBin; 
		 fromBinMethods["numAntenna"] = &CalCurveRow::numAntennaFromBin; 
		 fromBinMethods["numPoly"] = &CalCurveRow::numPolyFromBin; 
		 fromBinMethods["numReceptor"] = &CalCurveRow::numReceptorFromBin; 
		 fromBinMethods["antennaNames"] = &CalCurveRow::antennaNamesFromBin; 
		 fromBinMethods["refAntennaName"] = &CalCurveRow::refAntennaNameFromBin; 
		 fromBinMethods["polarizationTypes"] = &CalCurveRow::polarizationTypesFromBin; 
		 fromBinMethods["curve"] = &CalCurveRow::curveFromBin; 
		 fromBinMethods["reducedChiSquared"] = &CalCurveRow::reducedChiSquaredFromBin; 
			
	
		 fromBinMethods["numBaseline"] = &CalCurveRow::numBaselineFromBin; 
		 fromBinMethods["rms"] = &CalCurveRow::rmsFromBin; 
			
	}

	
	bool CalCurveRow::compareNoAutoInc(AtmPhaseCorrectionMod::AtmPhaseCorrection atmPhaseCorrection, CalCurveTypeMod::CalCurveType typeCurve, ReceiverBandMod::ReceiverBand receiverBand, Tag calDataId, Tag calReductionId, ArrayTime startValidTime, ArrayTime endValidTime, std::vector<Frequency > frequencyRange, int numAntenna, int numPoly, int numReceptor, std::vector<std::string > antennaNames, std::string refAntennaName, std::vector<PolarizationTypeMod::PolarizationType > polarizationTypes, std::vector<std::vector<std::vector<float > > > curve, std::vector<double > reducedChiSquared) {
		bool result;
		result = true;
		
	
		
		result = result && (this->atmPhaseCorrection == atmPhaseCorrection);
		
		if (!result) return false;
	

	
		
		result = result && (this->typeCurve == typeCurve);
		
		if (!result) return false;
	

	
		
		result = result && (this->receiverBand == receiverBand);
		
		if (!result) return false;
	

	
		
		result = result && (this->calDataId == calDataId);
		
		if (!result) return false;
	

	
		
		result = result && (this->calReductionId == calReductionId);
		
		if (!result) return false;
	

	
		
		result = result && (this->startValidTime == startValidTime);
		
		if (!result) return false;
	

	
		
		result = result && (this->endValidTime == endValidTime);
		
		if (!result) return false;
	

	
		
		result = result && (this->frequencyRange == frequencyRange);
		
		if (!result) return false;
	

	
		
		result = result && (this->numAntenna == numAntenna);
		
		if (!result) return false;
	

	
		
		result = result && (this->numPoly == numPoly);
		
		if (!result) return false;
	

	
		
		result = result && (this->numReceptor == numReceptor);
		
		if (!result) return false;
	

	
		
		result = result && (this->antennaNames == antennaNames);
		
		if (!result) return false;
	

	
		
		result = result && (this->refAntennaName == refAntennaName);
		
		if (!result) return false;
	

	
		
		result = result && (this->polarizationTypes == polarizationTypes);
		
		if (!result) return false;
	

	
		
		result = result && (this->curve == curve);
		
		if (!result) return false;
	

	
		
		result = result && (this->reducedChiSquared == reducedChiSquared);
		
		if (!result) return false;
	

		return result;
	}	
	
	
	
	bool CalCurveRow::compareRequiredValue(ArrayTime startValidTime, ArrayTime endValidTime, std::vector<Frequency > frequencyRange, int numAntenna, int numPoly, int numReceptor, std::vector<std::string > antennaNames, std::string refAntennaName, std::vector<PolarizationTypeMod::PolarizationType > polarizationTypes, std::vector<std::vector<std::vector<float > > > curve, std::vector<double > reducedChiSquared) {
		bool result;
		result = true;
		
	
		if (!(this->startValidTime == startValidTime)) return false;
	

	
		if (!(this->endValidTime == endValidTime)) return false;
	

	
		if (!(this->frequencyRange == frequencyRange)) return false;
	

	
		if (!(this->numAntenna == numAntenna)) return false;
	

	
		if (!(this->numPoly == numPoly)) return false;
	

	
		if (!(this->numReceptor == numReceptor)) return false;
	

	
		if (!(this->antennaNames == antennaNames)) return false;
	

	
		if (!(this->refAntennaName == refAntennaName)) return false;
	

	
		if (!(this->polarizationTypes == polarizationTypes)) return false;
	

	
		if (!(this->curve == curve)) return false;
	

	
		if (!(this->reducedChiSquared == reducedChiSquared)) return false;
	

		return result;
	}
	
	
	/**
	 * Return true if all required attributes of the value part are equal to their homologues
	 * in x and false otherwise.
	 *

	 * @param x a pointer on the CalCurveRow whose required attributes of the value part 

	 * will be compared with those of this.
	 * @return a boolean.
	 */
	bool CalCurveRow::equalByRequiredValue(CalCurveRow*  x ) {
		
			
		if (this->startValidTime != x->startValidTime) return false;
			
		if (this->endValidTime != x->endValidTime) return false;
			
		if (this->frequencyRange != x->frequencyRange) return false;
			
		if (this->numAntenna != x->numAntenna) return false;
			
		if (this->numPoly != x->numPoly) return false;
			
		if (this->numReceptor != x->numReceptor) return false;
			
		if (this->antennaNames != x->antennaNames) return false;
			
		if (this->refAntennaName != x->refAntennaName) return false;
			
		if (this->polarizationTypes != x->polarizationTypes) return false;
			
		if (this->curve != x->curve) return false;
			
		if (this->reducedChiSquared != x->reducedChiSquared) return false;
			
		
		return true;
	}	
	
/*
	 map<string, CalCurveAttributeFromBin> CalCurveRow::initFromBinMethods() {
		map<string, CalCurveAttributeFromBin> result;
		
		result["atmPhaseCorrection"] = &CalCurveRow::atmPhaseCorrectionFromBin;
		result["typeCurve"] = &CalCurveRow::typeCurveFromBin;
		result["receiverBand"] = &CalCurveRow::receiverBandFromBin;
		result["calDataId"] = &CalCurveRow::calDataIdFromBin;
		result["calReductionId"] = &CalCurveRow::calReductionIdFromBin;
		result["startValidTime"] = &CalCurveRow::startValidTimeFromBin;
		result["endValidTime"] = &CalCurveRow::endValidTimeFromBin;
		result["frequencyRange"] = &CalCurveRow::frequencyRangeFromBin;
		result["numAntenna"] = &CalCurveRow::numAntennaFromBin;
		result["numPoly"] = &CalCurveRow::numPolyFromBin;
		result["numReceptor"] = &CalCurveRow::numReceptorFromBin;
		result["antennaNames"] = &CalCurveRow::antennaNamesFromBin;
		result["refAntennaName"] = &CalCurveRow::refAntennaNameFromBin;
		result["polarizationTypes"] = &CalCurveRow::polarizationTypesFromBin;
		result["curve"] = &CalCurveRow::curveFromBin;
		result["reducedChiSquared"] = &CalCurveRow::reducedChiSquaredFromBin;
		
		
		result["numBaseline"] = &CalCurveRow::numBaselineFromBin;
		result["rms"] = &CalCurveRow::rmsFromBin;
			
		
		return result;	
	}
*/	
} // End namespace asdm
 
