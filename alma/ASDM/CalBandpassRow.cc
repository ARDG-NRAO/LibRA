
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
 * File CalBandpassRow.cpp
 */
 
#include <vector>
#include <set>

#include <alma/ASDM/ASDM.h>
#include <alma/ASDM/CalBandpassRow.h>
#include <alma/ASDM/CalBandpassTable.h>

#include <alma/ASDM/CalReductionTable.h>
#include <alma/ASDM/CalReductionRow.h>

#include <alma/ASDM/CalDataTable.h>
#include <alma/ASDM/CalDataRow.h>
	

using asdm::ASDM;
using asdm::CalBandpassRow;
using asdm::CalBandpassTable;

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
	CalBandpassRow::~CalBandpassRow() {
	}

	/**
	 * Return the table to which this row belongs.
	 */
	CalBandpassTable &CalBandpassRow::getTable() const {
		return table;
	}

	bool CalBandpassRow::isAdded() const {
		return hasBeenAdded;
	}	

	void CalBandpassRow::isAdded(bool added) {
		hasBeenAdded = added;
	}
	
#ifndef WITHOUT_ACS
	using asdmIDL::CalBandpassRowIDL;
#endif
	
#ifndef WITHOUT_ACS
	/**
	 * Return this row in the form of an IDL struct.
	 * @return The values of this row as a CalBandpassRowIDL struct.
	 */
	CalBandpassRowIDL *CalBandpassRow::toIDL() const {
		CalBandpassRowIDL *x = new CalBandpassRowIDL ();
		
		// Fill the IDL structure.
	
		
	
  		
		
		
			
				
		x->basebandName = basebandName;
 				
 			
		
	

	
  		
		
		
			
				
		x->sideband = sideband;
 				
 			
		
	

	
  		
		
		
			
				
		x->atmPhaseCorrection = atmPhaseCorrection;
 				
 			
		
	

	
  		
		
		
			
				
		x->typeCurve = typeCurve;
 				
 			
		
	

	
  		
		
		
			
				
		x->receiverBand = receiverBand;
 				
 			
		
	

	
  		
		
		
			
		x->startValidTime = startValidTime.toIDLArrayTime();
			
		
	

	
  		
		
		
			
		x->endValidTime = endValidTime.toIDLArrayTime();
			
		
	

	
  		
		
		
			
				
		x->numAntenna = numAntenna;
 				
 			
		
	

	
  		
		
		
			
				
		x->numPoly = numPoly;
 				
 			
		
	

	
  		
		
		
			
				
		x->numReceptor = numReceptor;
 				
 			
		
	

	
  		
		
		
			
		x->antennaNames.length(antennaNames.size());
		for (unsigned int i = 0; i < antennaNames.size(); ++i) {
			
				
			x->antennaNames[i] = CORBA::string_dup(antennaNames.at(i).c_str());
				
	 		
	 	}
			
		
	

	
  		
		
		
			
				
		x->refAntennaName = CORBA::string_dup(refAntennaName.c_str());
				
 			
		
	

	
  		
		
		
			
		x->freqLimits.length(freqLimits.size());
		for (unsigned int i = 0; i < freqLimits.size(); ++i) {
			
			x->freqLimits[i] = freqLimits.at(i).toIDLFrequency();
			
	 	}
			
		
	

	
  		
		
		
			
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
 				
 			
		
	

	
  		
		
		x->numFreqExists = numFreqExists;
		
		
			
				
		x->numFreq = numFreq;
 				
 			
		
	

	
  		
		
		x->rmsExists = rmsExists;
		
		
			
		x->rms.length(rms.size());
		for (unsigned int i = 0; i < rms.size(); i++) {
			x->rms[i].length(rms.at(i).size());			 		
		}
		
		for (unsigned int i = 0; i < rms.size() ; i++)
			for (unsigned int j = 0; j < rms.at(i).size(); j++)
					
						
				x->rms[i][j] = rms.at(i).at(j);
		 				
			 						
		
			
		
	

	
  		
		
		x->frequencyRangeExists = frequencyRangeExists;
		
		
			
		x->frequencyRange.length(frequencyRange.size());
		for (unsigned int i = 0; i < frequencyRange.size(); ++i) {
			
			x->frequencyRange[i] = frequencyRange.at(i).toIDLFrequency();
			
	 	}
			
		
	

	
  		
		
		x->numSpectralWindowExists = numSpectralWindowExists;
		
		
			
				
		x->numSpectralWindow = numSpectralWindow;
 				
 			
		
	

	
  		
		
		x->chanFreqStartExists = chanFreqStartExists;
		
		
			
		x->chanFreqStart.length(chanFreqStart.size());
		for (unsigned int i = 0; i < chanFreqStart.size(); ++i) {
			
			x->chanFreqStart[i] = chanFreqStart.at(i).toIDLFrequency();
			
	 	}
			
		
	

	
  		
		
		x->chanFreqStepExists = chanFreqStepExists;
		
		
			
		x->chanFreqStep.length(chanFreqStep.size());
		for (unsigned int i = 0; i < chanFreqStep.size(); ++i) {
			
			x->chanFreqStep[i] = chanFreqStep.at(i).toIDLFrequency();
			
	 	}
			
		
	

	
  		
		
		x->numSpectralWindowChanExists = numSpectralWindowChanExists;
		
		
			
		x->numSpectralWindowChan.length(numSpectralWindowChan.size());
		for (unsigned int i = 0; i < numSpectralWindowChan.size(); ++i) {
			
				
			x->numSpectralWindowChan[i] = numSpectralWindowChan.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		x->spectrumExists = spectrumExists;
		
		
			
		x->spectrum.length(spectrum.size());
		for (unsigned int i = 0; i < spectrum.size(); i++) {
			x->spectrum[i].length(spectrum.at(i).size());
			for (unsigned int j = 0; j < spectrum.at(i).size(); j++) {
				x->spectrum[i][j].length(spectrum.at(i).at(j).size());
			}					 		
		}
		
		for (unsigned int i = 0; i < spectrum.size() ; i++)
			for (unsigned int j = 0; j < spectrum.at(i).size(); j++)
				for (unsigned int k = 0; k < spectrum.at(i).at(j).size(); k++)
					
						
					x->spectrum[i][j][k] = spectrum.at(i).at(j).at(k);
		 				
			 									
			
		
	

	
	
		
	
  	
 		
		
	 	
			
		x->calDataId = calDataId.toIDLTag();
			
	 	 		
  	

	
  	
 		
		
	 	
			
		x->calReductionId = calReductionId.toIDLTag();
			
	 	 		
  	

	
		
	

	

		
		return x;
	
	}
	
	void CalBandpassRow::toIDL(asdmIDL::CalBandpassRowIDL& x) const {
		// Set the x's fields.
	
		
	
  		
		
		
			
				
		x.basebandName = basebandName;
 				
 			
		
	

	
  		
		
		
			
				
		x.sideband = sideband;
 				
 			
		
	

	
  		
		
		
			
				
		x.atmPhaseCorrection = atmPhaseCorrection;
 				
 			
		
	

	
  		
		
		
			
				
		x.typeCurve = typeCurve;
 				
 			
		
	

	
  		
		
		
			
				
		x.receiverBand = receiverBand;
 				
 			
		
	

	
  		
		
		
			
		x.startValidTime = startValidTime.toIDLArrayTime();
			
		
	

	
  		
		
		
			
		x.endValidTime = endValidTime.toIDLArrayTime();
			
		
	

	
  		
		
		
			
				
		x.numAntenna = numAntenna;
 				
 			
		
	

	
  		
		
		
			
				
		x.numPoly = numPoly;
 				
 			
		
	

	
  		
		
		
			
				
		x.numReceptor = numReceptor;
 				
 			
		
	

	
  		
		
		
			
		x.antennaNames.length(antennaNames.size());
		for (unsigned int i = 0; i < antennaNames.size(); ++i) {
			
				
			x.antennaNames[i] = CORBA::string_dup(antennaNames.at(i).c_str());
				
	 		
	 	}
			
		
	

	
  		
		
		
			
				
		x.refAntennaName = CORBA::string_dup(refAntennaName.c_str());
				
 			
		
	

	
  		
		
		
			
		x.freqLimits.length(freqLimits.size());
		for (unsigned int i = 0; i < freqLimits.size(); ++i) {
			
			x.freqLimits[i] = freqLimits.at(i).toIDLFrequency();
			
	 	}
			
		
	

	
  		
		
		
			
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
 				
 			
		
	

	
  		
		
		x.numFreqExists = numFreqExists;
		
		
			
				
		x.numFreq = numFreq;
 				
 			
		
	

	
  		
		
		x.rmsExists = rmsExists;
		
		
			
		x.rms.length(rms.size());
		for (unsigned int i = 0; i < rms.size(); i++) {
			x.rms[i].length(rms.at(i).size());			 		
		}
		
		for (unsigned int i = 0; i < rms.size() ; i++)
			for (unsigned int j = 0; j < rms.at(i).size(); j++)
					
						
				x.rms[i][j] = rms.at(i).at(j);
		 				
			 						
		
			
		
	

	
  		
		
		x.frequencyRangeExists = frequencyRangeExists;
		
		
			
		x.frequencyRange.length(frequencyRange.size());
		for (unsigned int i = 0; i < frequencyRange.size(); ++i) {
			
			x.frequencyRange[i] = frequencyRange.at(i).toIDLFrequency();
			
	 	}
			
		
	

	
  		
		
		x.numSpectralWindowExists = numSpectralWindowExists;
		
		
			
				
		x.numSpectralWindow = numSpectralWindow;
 				
 			
		
	

	
  		
		
		x.chanFreqStartExists = chanFreqStartExists;
		
		
			
		x.chanFreqStart.length(chanFreqStart.size());
		for (unsigned int i = 0; i < chanFreqStart.size(); ++i) {
			
			x.chanFreqStart[i] = chanFreqStart.at(i).toIDLFrequency();
			
	 	}
			
		
	

	
  		
		
		x.chanFreqStepExists = chanFreqStepExists;
		
		
			
		x.chanFreqStep.length(chanFreqStep.size());
		for (unsigned int i = 0; i < chanFreqStep.size(); ++i) {
			
			x.chanFreqStep[i] = chanFreqStep.at(i).toIDLFrequency();
			
	 	}
			
		
	

	
  		
		
		x.numSpectralWindowChanExists = numSpectralWindowChanExists;
		
		
			
		x.numSpectralWindowChan.length(numSpectralWindowChan.size());
		for (unsigned int i = 0; i < numSpectralWindowChan.size(); ++i) {
			
				
			x.numSpectralWindowChan[i] = numSpectralWindowChan.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		x.spectrumExists = spectrumExists;
		
		
			
		x.spectrum.length(spectrum.size());
		for (unsigned int i = 0; i < spectrum.size(); i++) {
			x.spectrum[i].length(spectrum.at(i).size());
			for (unsigned int j = 0; j < spectrum.at(i).size(); j++) {
				x.spectrum[i][j].length(spectrum.at(i).at(j).size());
			}					 		
		}
		
		for (unsigned int i = 0; i < spectrum.size() ; i++)
			for (unsigned int j = 0; j < spectrum.at(i).size(); j++)
				for (unsigned int k = 0; k < spectrum.at(i).at(j).size(); k++)
					
						
					x.spectrum[i][j][k] = spectrum.at(i).at(j).at(k);
		 				
			 									
			
		
	

	
	
		
	
  	
 		
		
	 	
			
		x.calDataId = calDataId.toIDLTag();
			
	 	 		
  	

	
  	
 		
		
	 	
			
		x.calReductionId = calReductionId.toIDLTag();
			
	 	 		
  	

	
		
	

	

	
	}
#endif
	

#ifndef WITHOUT_ACS
	/**
	 * Fill the values of this row from the IDL struct CalBandpassRowIDL.
	 * @param x The IDL struct containing the values used to fill this row.
	 */
	void CalBandpassRow::setFromIDL (CalBandpassRowIDL x){
		try {
		// Fill the values from x.
	
		
	
		
		
			
		setBasebandName(x.basebandName);
  			
 		
		
	

	
		
		
			
		setSideband(x.sideband);
  			
 		
		
	

	
		
		
			
		setAtmPhaseCorrection(x.atmPhaseCorrection);
  			
 		
		
	

	
		
		
			
		setTypeCurve(x.typeCurve);
  			
 		
		
	

	
		
		
			
		setReceiverBand(x.receiverBand);
  			
 		
		
	

	
		
		
			
		setStartValidTime(ArrayTime (x.startValidTime));
			
 		
		
	

	
		
		
			
		setEndValidTime(ArrayTime (x.endValidTime));
			
 		
		
	

	
		
		
			
		setNumAntenna(x.numAntenna);
  			
 		
		
	

	
		
		
			
		setNumPoly(x.numPoly);
  			
 		
		
	

	
		
		
			
		setNumReceptor(x.numReceptor);
  			
 		
		
	

	
		
		
			
		antennaNames .clear();
		for (unsigned int i = 0; i <x.antennaNames.length(); ++i) {
			
			antennaNames.push_back(string (x.antennaNames[i]));
			
		}
			
  		
		
	

	
		
		
			
		setRefAntennaName(string (x.refAntennaName));
			
 		
		
	

	
		
		
			
		freqLimits .clear();
		for (unsigned int i = 0; i <x.freqLimits.length(); ++i) {
			
			freqLimits.push_back(Frequency (x.freqLimits[i]));
			
		}
			
  		
		
	

	
		
		
			
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
		
	

	
		
		numFreqExists = x.numFreqExists;
		if (x.numFreqExists) {
		
		
			
		setNumFreq(x.numFreq);
  			
 		
		
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
		
	

	
		
		frequencyRangeExists = x.frequencyRangeExists;
		if (x.frequencyRangeExists) {
		
		
			
		frequencyRange .clear();
		for (unsigned int i = 0; i <x.frequencyRange.length(); ++i) {
			
			frequencyRange.push_back(Frequency (x.frequencyRange[i]));
			
		}
			
  		
		
		}
		
	

	
		
		numSpectralWindowExists = x.numSpectralWindowExists;
		if (x.numSpectralWindowExists) {
		
		
			
		setNumSpectralWindow(x.numSpectralWindow);
  			
 		
		
		}
		
	

	
		
		chanFreqStartExists = x.chanFreqStartExists;
		if (x.chanFreqStartExists) {
		
		
			
		chanFreqStart .clear();
		for (unsigned int i = 0; i <x.chanFreqStart.length(); ++i) {
			
			chanFreqStart.push_back(Frequency (x.chanFreqStart[i]));
			
		}
			
  		
		
		}
		
	

	
		
		chanFreqStepExists = x.chanFreqStepExists;
		if (x.chanFreqStepExists) {
		
		
			
		chanFreqStep .clear();
		for (unsigned int i = 0; i <x.chanFreqStep.length(); ++i) {
			
			chanFreqStep.push_back(Frequency (x.chanFreqStep[i]));
			
		}
			
  		
		
		}
		
	

	
		
		numSpectralWindowChanExists = x.numSpectralWindowChanExists;
		if (x.numSpectralWindowChanExists) {
		
		
			
		numSpectralWindowChan .clear();
		for (unsigned int i = 0; i <x.numSpectralWindowChan.length(); ++i) {
			
			numSpectralWindowChan.push_back(x.numSpectralWindowChan[i]);
  			
		}
			
  		
		
		}
		
	

	
		
		spectrumExists = x.spectrumExists;
		if (x.spectrumExists) {
		
		
			
		spectrum .clear();
		vector< vector<float> > vv_aux_spectrum;
		vector<float> v_aux_spectrum;
		
		for (unsigned int i = 0; i < x.spectrum.length(); ++i) {
			vv_aux_spectrum.clear();
			for (unsigned int j = 0; j < x.spectrum[0].length(); ++j) {
				v_aux_spectrum.clear();
				for (unsigned int k = 0; k < x.spectrum[0][0].length(); ++k) {
					
					v_aux_spectrum.push_back(x.spectrum[i][j][k]);
		  			
		  		}
		  		vv_aux_spectrum.push_back(v_aux_spectrum);
  			}
  			spectrum.push_back(vv_aux_spectrum);
		}
			
  		
		
		}
		
	

	
	
		
	
		
		
			
		setCalDataId(Tag (x.calDataId));
			
 		
		
	

	
		
		
			
		setCalReductionId(Tag (x.calReductionId));
			
 		
		
	

	
		
	

	

		} catch (const IllegalAccessException &err) {
			throw ConversionException (err.getMessage(),"CalBandpass");
		}
	}
#endif
	
	/**
	 * Return this row in the form of an XML string.
	 * @return The values of this row as an XML string.
	 */
	string CalBandpassRow::toXML() const {
		string buf;
		buf.append("<row> \n");
		
	
		
  	
 		
		
			buf.append(EnumerationParser::toXML("basebandName", basebandName));
		
		
	

  	
 		
		
			buf.append(EnumerationParser::toXML("sideband", sideband));
		
		
	

  	
 		
		
			buf.append(EnumerationParser::toXML("atmPhaseCorrection", atmPhaseCorrection));
		
		
	

  	
 		
		
			buf.append(EnumerationParser::toXML("typeCurve", typeCurve));
		
		
	

  	
 		
		
			buf.append(EnumerationParser::toXML("receiverBand", receiverBand));
		
		
	

  	
 		
		
		Parser::toXML(startValidTime, "startValidTime", buf);
		
		
	

  	
 		
		
		Parser::toXML(endValidTime, "endValidTime", buf);
		
		
	

  	
 		
		
		Parser::toXML(numAntenna, "numAntenna", buf);
		
		
	

  	
 		
		
		Parser::toXML(numPoly, "numPoly", buf);
		
		
	

  	
 		
		
		Parser::toXML(numReceptor, "numReceptor", buf);
		
		
	

  	
 		
		
		Parser::toXML(antennaNames, "antennaNames", buf);
		
		
	

  	
 		
		
		Parser::toXML(refAntennaName, "refAntennaName", buf);
		
		
	

  	
 		
		
		Parser::toXML(freqLimits, "freqLimits", buf);
		
		
	

  	
 		
		
			buf.append(EnumerationParser::toXML("polarizationTypes", polarizationTypes));
		
		
	

  	
 		
		
		Parser::toXML(curve, "curve", buf);
		
		
	

  	
 		
		
		Parser::toXML(reducedChiSquared, "reducedChiSquared", buf);
		
		
	

  	
 		
		if (numBaselineExists) {
		
		
		Parser::toXML(numBaseline, "numBaseline", buf);
		
		
		}
		
	

  	
 		
		if (numFreqExists) {
		
		
		Parser::toXML(numFreq, "numFreq", buf);
		
		
		}
		
	

  	
 		
		if (rmsExists) {
		
		
		Parser::toXML(rms, "rms", buf);
		
		
		}
		
	

  	
 		
		if (frequencyRangeExists) {
		
		
		Parser::toXML(frequencyRange, "frequencyRange", buf);
		
		
		}
		
	

  	
 		
		if (numSpectralWindowExists) {
		
		
		Parser::toXML(numSpectralWindow, "numSpectralWindow", buf);
		
		
		}
		
	

  	
 		
		if (chanFreqStartExists) {
		
		
		Parser::toXML(chanFreqStart, "chanFreqStart", buf);
		
		
		}
		
	

  	
 		
		if (chanFreqStepExists) {
		
		
		Parser::toXML(chanFreqStep, "chanFreqStep", buf);
		
		
		}
		
	

  	
 		
		if (numSpectralWindowChanExists) {
		
		
		Parser::toXML(numSpectralWindowChan, "numSpectralWindowChan", buf);
		
		
		}
		
	

  	
 		
		if (spectrumExists) {
		
		
		Parser::toXML(spectrum, "spectrum", buf);
		
		
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
	void CalBandpassRow::setFromXML (string rowDoc) {
		Parser row(rowDoc);
		string s = "";
		try {
	
		
	
		
		
		
		basebandName = EnumerationParser::getBasebandName("basebandName","CalBandpass",rowDoc);
		
		
		
	

	
		
		
		
		sideband = EnumerationParser::getNetSideband("sideband","CalBandpass",rowDoc);
		
		
		
	

	
		
		
		
		atmPhaseCorrection = EnumerationParser::getAtmPhaseCorrection("atmPhaseCorrection","CalBandpass",rowDoc);
		
		
		
	

	
		
		
		
		typeCurve = EnumerationParser::getCalCurveType("typeCurve","CalBandpass",rowDoc);
		
		
		
	

	
		
		
		
		receiverBand = EnumerationParser::getReceiverBand("receiverBand","CalBandpass",rowDoc);
		
		
		
	

	
  		
			
	  	setStartValidTime(Parser::getArrayTime("startValidTime","CalBandpass",rowDoc));
			
		
	

	
  		
			
	  	setEndValidTime(Parser::getArrayTime("endValidTime","CalBandpass",rowDoc));
			
		
	

	
  		
			
	  	setNumAntenna(Parser::getInteger("numAntenna","CalBandpass",rowDoc));
			
		
	

	
  		
			
	  	setNumPoly(Parser::getInteger("numPoly","CalBandpass",rowDoc));
			
		
	

	
  		
			
	  	setNumReceptor(Parser::getInteger("numReceptor","CalBandpass",rowDoc));
			
		
	

	
  		
			
					
	  	setAntennaNames(Parser::get1DString("antennaNames","CalBandpass",rowDoc));
	  			
	  		
		
	

	
  		
			
	  	setRefAntennaName(Parser::getString("refAntennaName","CalBandpass",rowDoc));
			
		
	

	
  		
			
					
	  	setFreqLimits(Parser::get1DFrequency("freqLimits","CalBandpass",rowDoc));
	  			
	  		
		
	

	
		
		
		
		polarizationTypes = EnumerationParser::getPolarizationType1D("polarizationTypes","CalBandpass",rowDoc);			
		
		
		
	

	
  		
			
					
	  	setCurve(Parser::get3DFloat("curve","CalBandpass",rowDoc));
	  			
	  		
		
	

	
  		
			
					
	  	setReducedChiSquared(Parser::get1DDouble("reducedChiSquared","CalBandpass",rowDoc));
	  			
	  		
		
	

	
  		
        if (row.isStr("<numBaseline>")) {
			
	  		setNumBaseline(Parser::getInteger("numBaseline","CalBandpass",rowDoc));
			
		}
 		
	

	
  		
        if (row.isStr("<numFreq>")) {
			
	  		setNumFreq(Parser::getInteger("numFreq","CalBandpass",rowDoc));
			
		}
 		
	

	
  		
        if (row.isStr("<rms>")) {
			
								
	  		setRms(Parser::get2DFloat("rms","CalBandpass",rowDoc));
	  			
	  		
		}
 		
	

	
  		
        if (row.isStr("<frequencyRange>")) {
			
								
	  		setFrequencyRange(Parser::get1DFrequency("frequencyRange","CalBandpass",rowDoc));
	  			
	  		
		}
 		
	

	
  		
        if (row.isStr("<numSpectralWindow>")) {
			
	  		setNumSpectralWindow(Parser::getInteger("numSpectralWindow","CalBandpass",rowDoc));
			
		}
 		
	

	
  		
        if (row.isStr("<chanFreqStart>")) {
			
								
	  		setChanFreqStart(Parser::get1DFrequency("chanFreqStart","CalBandpass",rowDoc));
	  			
	  		
		}
 		
	

	
  		
        if (row.isStr("<chanFreqStep>")) {
			
								
	  		setChanFreqStep(Parser::get1DFrequency("chanFreqStep","CalBandpass",rowDoc));
	  			
	  		
		}
 		
	

	
  		
        if (row.isStr("<numSpectralWindowChan>")) {
			
								
	  		setNumSpectralWindowChan(Parser::get1DInteger("numSpectralWindowChan","CalBandpass",rowDoc));
	  			
	  		
		}
 		
	

	
  		
        if (row.isStr("<spectrum>")) {
			
								
	  		setSpectrum(Parser::get3DFloat("spectrum","CalBandpass",rowDoc));
	  			
	  		
		}
 		
	

	
	
		
	
  		
			
	  	setCalDataId(Parser::getTag("calDataId","CalData",rowDoc));
			
		
	

	
  		
			
	  	setCalReductionId(Parser::getTag("calReductionId","CalReduction",rowDoc));
			
		
	

	
		
	

	

		} catch (const IllegalAccessException &err) {
			throw ConversionException (err.getMessage(),"CalBandpass");
		}
	}
	
	void CalBandpassRow::toBin(EndianOSStream& eoss) {
	
	
	
	
		
					
		eoss.writeString(CBasebandName::name(basebandName));
			/* eoss.writeInt(basebandName); */
				
		
	

	
	
		
					
		eoss.writeString(CNetSideband::name(sideband));
			/* eoss.writeInt(sideband); */
				
		
	

	
	
		
					
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
		
	

	
	
		
						
			eoss.writeInt(numAntenna);
				
		
	

	
	
		
						
			eoss.writeInt(numPoly);
				
		
	

	
	
		
						
			eoss.writeInt(numReceptor);
				
		
	

	
	
		
		
			
		eoss.writeInt((int) antennaNames.size());
		for (unsigned int i = 0; i < antennaNames.size(); i++)
				
			eoss.writeString(antennaNames.at(i));
				
				
						
		
	

	
	
		
						
			eoss.writeString(refAntennaName);
				
		
	

	
	
		
	Frequency::toBin(freqLimits, eoss);
		
	

	
	
		
		
			
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

	eoss.writeBoolean(numFreqExists);
	if (numFreqExists) {
	
	
	
		
						
			eoss.writeInt(numFreq);
				
		
	

	}

	eoss.writeBoolean(rmsExists);
	if (rmsExists) {
	
	
	
		
		
			
		eoss.writeInt((int) rms.size());
		eoss.writeInt((int) rms.at(0).size());
		for (unsigned int i = 0; i < rms.size(); i++) 
			for (unsigned int j = 0;  j < rms.at(0).size(); j++) 
							 
				eoss.writeFloat(rms.at(i).at(j));
				
	
						
		
	

	}

	eoss.writeBoolean(frequencyRangeExists);
	if (frequencyRangeExists) {
	
	
	
		
	Frequency::toBin(frequencyRange, eoss);
		
	

	}

	eoss.writeBoolean(numSpectralWindowExists);
	if (numSpectralWindowExists) {
	
	
	
		
						
			eoss.writeInt(numSpectralWindow);
				
		
	

	}

	eoss.writeBoolean(chanFreqStartExists);
	if (chanFreqStartExists) {
	
	
	
		
	Frequency::toBin(chanFreqStart, eoss);
		
	

	}

	eoss.writeBoolean(chanFreqStepExists);
	if (chanFreqStepExists) {
	
	
	
		
	Frequency::toBin(chanFreqStep, eoss);
		
	

	}

	eoss.writeBoolean(numSpectralWindowChanExists);
	if (numSpectralWindowChanExists) {
	
	
	
		
		
			
		eoss.writeInt((int) numSpectralWindowChan.size());
		for (unsigned int i = 0; i < numSpectralWindowChan.size(); i++)
				
			eoss.writeInt(numSpectralWindowChan.at(i));
				
				
						
		
	

	}

	eoss.writeBoolean(spectrumExists);
	if (spectrumExists) {
	
	
	
		
		
			
		eoss.writeInt((int) spectrum.size());
		eoss.writeInt((int) spectrum.at(0).size());		
		eoss.writeInt((int) spectrum.at(0).at(0).size());
		for (unsigned int i = 0; i < spectrum.size(); i++) 
			for (unsigned int j = 0;  j < spectrum.at(0).size(); j++)
				for (unsigned int k = 0; k <  spectrum.at(0).at(0).size(); k++)	
							 
					eoss.writeFloat(spectrum.at(i).at(j).at(k));
						
						
		
	

	}

	}
	
void CalBandpassRow::basebandNameFromBin(EndianIStream& eis) {
		
	
	
		
			
		basebandName = CBasebandName::literal(eis.readString());
			
		
	
	
}
void CalBandpassRow::sidebandFromBin(EndianIStream& eis) {
		
	
	
		
			
		sideband = CNetSideband::literal(eis.readString());
			
		
	
	
}
void CalBandpassRow::atmPhaseCorrectionFromBin(EndianIStream& eis) {
		
	
	
		
			
		atmPhaseCorrection = CAtmPhaseCorrection::literal(eis.readString());
			
		
	
	
}
void CalBandpassRow::typeCurveFromBin(EndianIStream& eis) {
		
	
	
		
			
		typeCurve = CCalCurveType::literal(eis.readString());
			
		
	
	
}
void CalBandpassRow::receiverBandFromBin(EndianIStream& eis) {
		
	
	
		
			
		receiverBand = CReceiverBand::literal(eis.readString());
			
		
	
	
}
void CalBandpassRow::calDataIdFromBin(EndianIStream& eis) {
		
	
		
		
		calDataId =  Tag::fromBin(eis);
		
	
	
}
void CalBandpassRow::calReductionIdFromBin(EndianIStream& eis) {
		
	
		
		
		calReductionId =  Tag::fromBin(eis);
		
	
	
}
void CalBandpassRow::startValidTimeFromBin(EndianIStream& eis) {
		
	
		
		
		startValidTime =  ArrayTime::fromBin(eis);
		
	
	
}
void CalBandpassRow::endValidTimeFromBin(EndianIStream& eis) {
		
	
		
		
		endValidTime =  ArrayTime::fromBin(eis);
		
	
	
}
void CalBandpassRow::numAntennaFromBin(EndianIStream& eis) {
		
	
	
		
			
		numAntenna =  eis.readInt();
			
		
	
	
}
void CalBandpassRow::numPolyFromBin(EndianIStream& eis) {
		
	
	
		
			
		numPoly =  eis.readInt();
			
		
	
	
}
void CalBandpassRow::numReceptorFromBin(EndianIStream& eis) {
		
	
	
		
			
		numReceptor =  eis.readInt();
			
		
	
	
}
void CalBandpassRow::antennaNamesFromBin(EndianIStream& eis) {
		
	
	
		
			
	
		antennaNames.clear();
		
		unsigned int antennaNamesDim1 = eis.readInt();
		for (unsigned int  i = 0 ; i < antennaNamesDim1; i++)
			
			antennaNames.push_back(eis.readString());
			
	

		
	
	
}
void CalBandpassRow::refAntennaNameFromBin(EndianIStream& eis) {
		
	
	
		
			
		refAntennaName =  eis.readString();
			
		
	
	
}
void CalBandpassRow::freqLimitsFromBin(EndianIStream& eis) {
		
	
		
		
			
	
	freqLimits = Frequency::from1DBin(eis);	
	

		
	
	
}
void CalBandpassRow::polarizationTypesFromBin(EndianIStream& eis) {
		
	
	
		
			
	
		polarizationTypes.clear();
		
		unsigned int polarizationTypesDim1 = eis.readInt();
		for (unsigned int  i = 0 ; i < polarizationTypesDim1; i++)
			
			polarizationTypes.push_back(CPolarizationType::literal(eis.readString()));
			
	

		
	
	
}
void CalBandpassRow::curveFromBin(EndianIStream& eis) {
		
	
	
		
			
	
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
void CalBandpassRow::reducedChiSquaredFromBin(EndianIStream& eis) {
		
	
	
		
			
	
		reducedChiSquared.clear();
		
		unsigned int reducedChiSquaredDim1 = eis.readInt();
		for (unsigned int  i = 0 ; i < reducedChiSquaredDim1; i++)
			
			reducedChiSquared.push_back(eis.readDouble());
			
	

		
	
	
}

void CalBandpassRow::numBaselineFromBin(EndianIStream& eis) {
		
	numBaselineExists = eis.readBoolean();
	if (numBaselineExists) {
		
	
	
		
			
		numBaseline =  eis.readInt();
			
		
	

	}
	
}
void CalBandpassRow::numFreqFromBin(EndianIStream& eis) {
		
	numFreqExists = eis.readBoolean();
	if (numFreqExists) {
		
	
	
		
			
		numFreq =  eis.readInt();
			
		
	

	}
	
}
void CalBandpassRow::rmsFromBin(EndianIStream& eis) {
		
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
void CalBandpassRow::frequencyRangeFromBin(EndianIStream& eis) {
		
	frequencyRangeExists = eis.readBoolean();
	if (frequencyRangeExists) {
		
	
		
		
			
	
	frequencyRange = Frequency::from1DBin(eis);	
	

		
	

	}
	
}
void CalBandpassRow::numSpectralWindowFromBin(EndianIStream& eis) {
		
	numSpectralWindowExists = eis.readBoolean();
	if (numSpectralWindowExists) {
		
	
	
		
			
		numSpectralWindow =  eis.readInt();
			
		
	

	}
	
}
void CalBandpassRow::chanFreqStartFromBin(EndianIStream& eis) {
		
	chanFreqStartExists = eis.readBoolean();
	if (chanFreqStartExists) {
		
	
		
		
			
	
	chanFreqStart = Frequency::from1DBin(eis);	
	

		
	

	}
	
}
void CalBandpassRow::chanFreqStepFromBin(EndianIStream& eis) {
		
	chanFreqStepExists = eis.readBoolean();
	if (chanFreqStepExists) {
		
	
		
		
			
	
	chanFreqStep = Frequency::from1DBin(eis);	
	

		
	

	}
	
}
void CalBandpassRow::numSpectralWindowChanFromBin(EndianIStream& eis) {
		
	numSpectralWindowChanExists = eis.readBoolean();
	if (numSpectralWindowChanExists) {
		
	
	
		
			
	
		numSpectralWindowChan.clear();
		
		unsigned int numSpectralWindowChanDim1 = eis.readInt();
		for (unsigned int  i = 0 ; i < numSpectralWindowChanDim1; i++)
			
			numSpectralWindowChan.push_back(eis.readInt());
			
	

		
	

	}
	
}
void CalBandpassRow::spectrumFromBin(EndianIStream& eis) {
		
	spectrumExists = eis.readBoolean();
	if (spectrumExists) {
		
	
	
		
			
	
		spectrum.clear();
			
		unsigned int spectrumDim1 = eis.readInt();
		unsigned int spectrumDim2 = eis.readInt();
		unsigned int spectrumDim3 = eis.readInt();
		vector <vector<float> > spectrumAux2;
		vector <float> spectrumAux1;
		for (unsigned int i = 0; i < spectrumDim1; i++) {
			spectrumAux2.clear();
			for (unsigned int j = 0; j < spectrumDim2 ; j++) {
				spectrumAux1.clear();
				for (unsigned int k = 0; k < spectrumDim3; k++) {
			
					spectrumAux1.push_back(eis.readFloat());
			
				}
				spectrumAux2.push_back(spectrumAux1);
			}
			spectrum.push_back(spectrumAux2);
		}	
	

		
	

	}
	
}
	
	
	CalBandpassRow* CalBandpassRow::fromBin(EndianIStream& eis, CalBandpassTable& table, const vector<string>& attributesSeq) {
		CalBandpassRow* row = new  CalBandpassRow(table);
		
		map<string, CalBandpassAttributeFromBin>::iterator iter ;
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
					throw ConversionException("There is not method to read an attribute '"+attributesSeq.at(i)+"'.", "CalBandpassTable");
			}
				
		}				
		return row;
	}

	//
	// A collection of methods to set the value of the attributes from their textual value in the XML representation
	// of one row.
	//
	
	// Convert a string into an BasebandName 
	void CalBandpassRow::basebandNameFromText(const string & s) {
		 
          
		basebandName = ASDMValuesParser::parse<BasebandNameMod::BasebandName>(s);
          
		
	}
	
	
	// Convert a string into an NetSideband 
	void CalBandpassRow::sidebandFromText(const string & s) {
		 
          
		sideband = ASDMValuesParser::parse<NetSidebandMod::NetSideband>(s);
          
		
	}
	
	
	// Convert a string into an AtmPhaseCorrection 
	void CalBandpassRow::atmPhaseCorrectionFromText(const string & s) {
		 
          
		atmPhaseCorrection = ASDMValuesParser::parse<AtmPhaseCorrectionMod::AtmPhaseCorrection>(s);
          
		
	}
	
	
	// Convert a string into an CalCurveType 
	void CalBandpassRow::typeCurveFromText(const string & s) {
		 
          
		typeCurve = ASDMValuesParser::parse<CalCurveTypeMod::CalCurveType>(s);
          
		
	}
	
	
	// Convert a string into an ReceiverBand 
	void CalBandpassRow::receiverBandFromText(const string & s) {
		 
          
		receiverBand = ASDMValuesParser::parse<ReceiverBandMod::ReceiverBand>(s);
          
		
	}
	
	
	// Convert a string into an Tag 
	void CalBandpassRow::calDataIdFromText(const string & s) {
		 
          
		calDataId = ASDMValuesParser::parse<Tag>(s);
          
		
	}
	
	
	// Convert a string into an Tag 
	void CalBandpassRow::calReductionIdFromText(const string & s) {
		 
          
		calReductionId = ASDMValuesParser::parse<Tag>(s);
          
		
	}
	
	
	// Convert a string into an ArrayTime 
	void CalBandpassRow::startValidTimeFromText(const string & s) {
		 
          
		startValidTime = ASDMValuesParser::parse<ArrayTime>(s);
          
		
	}
	
	
	// Convert a string into an ArrayTime 
	void CalBandpassRow::endValidTimeFromText(const string & s) {
		 
          
		endValidTime = ASDMValuesParser::parse<ArrayTime>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void CalBandpassRow::numAntennaFromText(const string & s) {
		 
          
		numAntenna = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void CalBandpassRow::numPolyFromText(const string & s) {
		 
          
		numPoly = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void CalBandpassRow::numReceptorFromText(const string & s) {
		 
          
		numReceptor = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	// Convert a string into an String 
	void CalBandpassRow::antennaNamesFromText(const string & s) {
		 
          
		antennaNames = ASDMValuesParser::parse1D<string>(s);
          
		
	}
	
	
	// Convert a string into an String 
	void CalBandpassRow::refAntennaNameFromText(const string & s) {
		 
          
		refAntennaName = ASDMValuesParser::parse<string>(s);
          
		
	}
	
	
	// Convert a string into an Frequency 
	void CalBandpassRow::freqLimitsFromText(const string & s) {
		 
          
		freqLimits = ASDMValuesParser::parse1D<Frequency>(s);
          
		
	}
	
	
	// Convert a string into an PolarizationType 
	void CalBandpassRow::polarizationTypesFromText(const string & s) {
		 
          
		polarizationTypes = ASDMValuesParser::parse1D<PolarizationTypeMod::PolarizationType>(s);
          
		
	}
	
	
	// Convert a string into an float 
	void CalBandpassRow::curveFromText(const string & s) {
		 
          
		curve = ASDMValuesParser::parse3D<float>(s);
          
		
	}
	
	
	// Convert a string into an double 
	void CalBandpassRow::reducedChiSquaredFromText(const string & s) {
		 
          
		reducedChiSquared = ASDMValuesParser::parse1D<double>(s);
          
		
	}
	

	
	// Convert a string into an int 
	void CalBandpassRow::numBaselineFromText(const string & s) {
		numBaselineExists = true;
		 
          
		numBaseline = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void CalBandpassRow::numFreqFromText(const string & s) {
		numFreqExists = true;
		 
          
		numFreq = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	// Convert a string into an float 
	void CalBandpassRow::rmsFromText(const string & s) {
		rmsExists = true;
		 
          
		rms = ASDMValuesParser::parse2D<float>(s);
          
		
	}
	
	
	// Convert a string into an Frequency 
	void CalBandpassRow::frequencyRangeFromText(const string & s) {
		frequencyRangeExists = true;
		 
          
		frequencyRange = ASDMValuesParser::parse1D<Frequency>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void CalBandpassRow::numSpectralWindowFromText(const string & s) {
		numSpectralWindowExists = true;
		 
          
		numSpectralWindow = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	// Convert a string into an Frequency 
	void CalBandpassRow::chanFreqStartFromText(const string & s) {
		chanFreqStartExists = true;
		 
          
		chanFreqStart = ASDMValuesParser::parse1D<Frequency>(s);
          
		
	}
	
	
	// Convert a string into an Frequency 
	void CalBandpassRow::chanFreqStepFromText(const string & s) {
		chanFreqStepExists = true;
		 
          
		chanFreqStep = ASDMValuesParser::parse1D<Frequency>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void CalBandpassRow::numSpectralWindowChanFromText(const string & s) {
		numSpectralWindowChanExists = true;
		 
          
		numSpectralWindowChan = ASDMValuesParser::parse1D<int>(s);
          
		
	}
	
	
	// Convert a string into an float 
	void CalBandpassRow::spectrumFromText(const string & s) {
		spectrumExists = true;
		 
          
		spectrum = ASDMValuesParser::parse3D<float>(s);
          
		
	}
	
	
	
	void CalBandpassRow::fromText(const std::string& attributeName, const std::string&  t) {
		map<string, CalBandpassAttributeFromText>::iterator iter;
		if ((iter = fromTextMethods.find(attributeName)) == fromTextMethods.end())
			throw ConversionException("I do not know what to do with '"+attributeName+"' and its content '"+t+"' (while parsing an XML document)", "CalBandpassTable");
		(this->*(iter->second))(t);
	}
			
	////////////////////////////////////////////////
	// Intrinsic Table Attributes getters/setters //
	////////////////////////////////////////////////
	
	

	
 	/**
 	 * Get basebandName.
 	 * @return basebandName as BasebandNameMod::BasebandName
 	 */
 	BasebandNameMod::BasebandName CalBandpassRow::getBasebandName() const {
	
  		return basebandName;
 	}

 	/**
 	 * Set basebandName with the specified BasebandNameMod::BasebandName.
 	 * @param basebandName The BasebandNameMod::BasebandName value to which basebandName is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void CalBandpassRow::setBasebandName (BasebandNameMod::BasebandName basebandName)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("basebandName", "CalBandpass");
		
  		}
  	
 		this->basebandName = basebandName;
	
 	}
	
	

	

	
 	/**
 	 * Get sideband.
 	 * @return sideband as NetSidebandMod::NetSideband
 	 */
 	NetSidebandMod::NetSideband CalBandpassRow::getSideband() const {
	
  		return sideband;
 	}

 	/**
 	 * Set sideband with the specified NetSidebandMod::NetSideband.
 	 * @param sideband The NetSidebandMod::NetSideband value to which sideband is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void CalBandpassRow::setSideband (NetSidebandMod::NetSideband sideband)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("sideband", "CalBandpass");
		
  		}
  	
 		this->sideband = sideband;
	
 	}
	
	

	

	
 	/**
 	 * Get atmPhaseCorrection.
 	 * @return atmPhaseCorrection as AtmPhaseCorrectionMod::AtmPhaseCorrection
 	 */
 	AtmPhaseCorrectionMod::AtmPhaseCorrection CalBandpassRow::getAtmPhaseCorrection() const {
	
  		return atmPhaseCorrection;
 	}

 	/**
 	 * Set atmPhaseCorrection with the specified AtmPhaseCorrectionMod::AtmPhaseCorrection.
 	 * @param atmPhaseCorrection The AtmPhaseCorrectionMod::AtmPhaseCorrection value to which atmPhaseCorrection is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void CalBandpassRow::setAtmPhaseCorrection (AtmPhaseCorrectionMod::AtmPhaseCorrection atmPhaseCorrection)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("atmPhaseCorrection", "CalBandpass");
		
  		}
  	
 		this->atmPhaseCorrection = atmPhaseCorrection;
	
 	}
	
	

	

	
 	/**
 	 * Get typeCurve.
 	 * @return typeCurve as CalCurveTypeMod::CalCurveType
 	 */
 	CalCurveTypeMod::CalCurveType CalBandpassRow::getTypeCurve() const {
	
  		return typeCurve;
 	}

 	/**
 	 * Set typeCurve with the specified CalCurveTypeMod::CalCurveType.
 	 * @param typeCurve The CalCurveTypeMod::CalCurveType value to which typeCurve is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void CalBandpassRow::setTypeCurve (CalCurveTypeMod::CalCurveType typeCurve)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("typeCurve", "CalBandpass");
		
  		}
  	
 		this->typeCurve = typeCurve;
	
 	}
	
	

	

	
 	/**
 	 * Get receiverBand.
 	 * @return receiverBand as ReceiverBandMod::ReceiverBand
 	 */
 	ReceiverBandMod::ReceiverBand CalBandpassRow::getReceiverBand() const {
	
  		return receiverBand;
 	}

 	/**
 	 * Set receiverBand with the specified ReceiverBandMod::ReceiverBand.
 	 * @param receiverBand The ReceiverBandMod::ReceiverBand value to which receiverBand is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void CalBandpassRow::setReceiverBand (ReceiverBandMod::ReceiverBand receiverBand)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("receiverBand", "CalBandpass");
		
  		}
  	
 		this->receiverBand = receiverBand;
	
 	}
	
	

	

	
 	/**
 	 * Get startValidTime.
 	 * @return startValidTime as ArrayTime
 	 */
 	ArrayTime CalBandpassRow::getStartValidTime() const {
	
  		return startValidTime;
 	}

 	/**
 	 * Set startValidTime with the specified ArrayTime.
 	 * @param startValidTime The ArrayTime value to which startValidTime is to be set.
 	 
 	
 		
 	 */
 	void CalBandpassRow::setStartValidTime (ArrayTime startValidTime)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->startValidTime = startValidTime;
	
 	}
	
	

	

	
 	/**
 	 * Get endValidTime.
 	 * @return endValidTime as ArrayTime
 	 */
 	ArrayTime CalBandpassRow::getEndValidTime() const {
	
  		return endValidTime;
 	}

 	/**
 	 * Set endValidTime with the specified ArrayTime.
 	 * @param endValidTime The ArrayTime value to which endValidTime is to be set.
 	 
 	
 		
 	 */
 	void CalBandpassRow::setEndValidTime (ArrayTime endValidTime)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->endValidTime = endValidTime;
	
 	}
	
	

	

	
 	/**
 	 * Get numAntenna.
 	 * @return numAntenna as int
 	 */
 	int CalBandpassRow::getNumAntenna() const {
	
  		return numAntenna;
 	}

 	/**
 	 * Set numAntenna with the specified int.
 	 * @param numAntenna The int value to which numAntenna is to be set.
 	 
 	
 		
 	 */
 	void CalBandpassRow::setNumAntenna (int numAntenna)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->numAntenna = numAntenna;
	
 	}
	
	

	

	
 	/**
 	 * Get numPoly.
 	 * @return numPoly as int
 	 */
 	int CalBandpassRow::getNumPoly() const {
	
  		return numPoly;
 	}

 	/**
 	 * Set numPoly with the specified int.
 	 * @param numPoly The int value to which numPoly is to be set.
 	 
 	
 		
 	 */
 	void CalBandpassRow::setNumPoly (int numPoly)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->numPoly = numPoly;
	
 	}
	
	

	

	
 	/**
 	 * Get numReceptor.
 	 * @return numReceptor as int
 	 */
 	int CalBandpassRow::getNumReceptor() const {
	
  		return numReceptor;
 	}

 	/**
 	 * Set numReceptor with the specified int.
 	 * @param numReceptor The int value to which numReceptor is to be set.
 	 
 	
 		
 	 */
 	void CalBandpassRow::setNumReceptor (int numReceptor)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->numReceptor = numReceptor;
	
 	}
	
	

	

	
 	/**
 	 * Get antennaNames.
 	 * @return antennaNames as std::vector<std::string >
 	 */
 	std::vector<std::string > CalBandpassRow::getAntennaNames() const {
	
  		return antennaNames;
 	}

 	/**
 	 * Set antennaNames with the specified std::vector<std::string >.
 	 * @param antennaNames The std::vector<std::string > value to which antennaNames is to be set.
 	 
 	
 		
 	 */
 	void CalBandpassRow::setAntennaNames (std::vector<std::string > antennaNames)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->antennaNames = antennaNames;
	
 	}
	
	

	

	
 	/**
 	 * Get refAntennaName.
 	 * @return refAntennaName as std::string
 	 */
 	std::string CalBandpassRow::getRefAntennaName() const {
	
  		return refAntennaName;
 	}

 	/**
 	 * Set refAntennaName with the specified std::string.
 	 * @param refAntennaName The std::string value to which refAntennaName is to be set.
 	 
 	
 		
 	 */
 	void CalBandpassRow::setRefAntennaName (std::string refAntennaName)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->refAntennaName = refAntennaName;
	
 	}
	
	

	

	
 	/**
 	 * Get freqLimits.
 	 * @return freqLimits as std::vector<Frequency >
 	 */
 	std::vector<Frequency > CalBandpassRow::getFreqLimits() const {
	
  		return freqLimits;
 	}

 	/**
 	 * Set freqLimits with the specified std::vector<Frequency >.
 	 * @param freqLimits The std::vector<Frequency > value to which freqLimits is to be set.
 	 
 	
 		
 	 */
 	void CalBandpassRow::setFreqLimits (std::vector<Frequency > freqLimits)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->freqLimits = freqLimits;
	
 	}
	
	

	

	
 	/**
 	 * Get polarizationTypes.
 	 * @return polarizationTypes as std::vector<PolarizationTypeMod::PolarizationType >
 	 */
 	std::vector<PolarizationTypeMod::PolarizationType > CalBandpassRow::getPolarizationTypes() const {
	
  		return polarizationTypes;
 	}

 	/**
 	 * Set polarizationTypes with the specified std::vector<PolarizationTypeMod::PolarizationType >.
 	 * @param polarizationTypes The std::vector<PolarizationTypeMod::PolarizationType > value to which polarizationTypes is to be set.
 	 
 	
 		
 	 */
 	void CalBandpassRow::setPolarizationTypes (std::vector<PolarizationTypeMod::PolarizationType > polarizationTypes)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->polarizationTypes = polarizationTypes;
	
 	}
	
	

	

	
 	/**
 	 * Get curve.
 	 * @return curve as std::vector<std::vector<std::vector<float > > >
 	 */
 	std::vector<std::vector<std::vector<float > > > CalBandpassRow::getCurve() const {
	
  		return curve;
 	}

 	/**
 	 * Set curve with the specified std::vector<std::vector<std::vector<float > > >.
 	 * @param curve The std::vector<std::vector<std::vector<float > > > value to which curve is to be set.
 	 
 	
 		
 	 */
 	void CalBandpassRow::setCurve (std::vector<std::vector<std::vector<float > > > curve)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->curve = curve;
	
 	}
	
	

	

	
 	/**
 	 * Get reducedChiSquared.
 	 * @return reducedChiSquared as std::vector<double >
 	 */
 	std::vector<double > CalBandpassRow::getReducedChiSquared() const {
	
  		return reducedChiSquared;
 	}

 	/**
 	 * Set reducedChiSquared with the specified std::vector<double >.
 	 * @param reducedChiSquared The std::vector<double > value to which reducedChiSquared is to be set.
 	 
 	
 		
 	 */
 	void CalBandpassRow::setReducedChiSquared (std::vector<double > reducedChiSquared)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->reducedChiSquared = reducedChiSquared;
	
 	}
	
	

	
	/**
	 * The attribute numBaseline is optional. Return true if this attribute exists.
	 * @return true if and only if the numBaseline attribute exists. 
	 */
	bool CalBandpassRow::isNumBaselineExists() const {
		return numBaselineExists;
	}
	

	
 	/**
 	 * Get numBaseline, which is optional.
 	 * @return numBaseline as int
 	 * @throw IllegalAccessException If numBaseline does not exist.
 	 */
 	int CalBandpassRow::getNumBaseline() const  {
		if (!numBaselineExists) {
			throw IllegalAccessException("numBaseline", "CalBandpass");
		}
	
  		return numBaseline;
 	}

 	/**
 	 * Set numBaseline with the specified int.
 	 * @param numBaseline The int value to which numBaseline is to be set.
 	 
 	
 	 */
 	void CalBandpassRow::setNumBaseline (int numBaseline) {
	
 		this->numBaseline = numBaseline;
	
		numBaselineExists = true;
	
 	}
	
	
	/**
	 * Mark numBaseline, which is an optional field, as non-existent.
	 */
	void CalBandpassRow::clearNumBaseline () {
		numBaselineExists = false;
	}
	

	
	/**
	 * The attribute numFreq is optional. Return true if this attribute exists.
	 * @return true if and only if the numFreq attribute exists. 
	 */
	bool CalBandpassRow::isNumFreqExists() const {
		return numFreqExists;
	}
	

	
 	/**
 	 * Get numFreq, which is optional.
 	 * @return numFreq as int
 	 * @throw IllegalAccessException If numFreq does not exist.
 	 */
 	int CalBandpassRow::getNumFreq() const  {
		if (!numFreqExists) {
			throw IllegalAccessException("numFreq", "CalBandpass");
		}
	
  		return numFreq;
 	}

 	/**
 	 * Set numFreq with the specified int.
 	 * @param numFreq The int value to which numFreq is to be set.
 	 
 	
 	 */
 	void CalBandpassRow::setNumFreq (int numFreq) {
	
 		this->numFreq = numFreq;
	
		numFreqExists = true;
	
 	}
	
	
	/**
	 * Mark numFreq, which is an optional field, as non-existent.
	 */
	void CalBandpassRow::clearNumFreq () {
		numFreqExists = false;
	}
	

	
	/**
	 * The attribute rms is optional. Return true if this attribute exists.
	 * @return true if and only if the rms attribute exists. 
	 */
	bool CalBandpassRow::isRmsExists() const {
		return rmsExists;
	}
	

	
 	/**
 	 * Get rms, which is optional.
 	 * @return rms as std::vector<std::vector<float > >
 	 * @throw IllegalAccessException If rms does not exist.
 	 */
 	std::vector<std::vector<float > > CalBandpassRow::getRms() const  {
		if (!rmsExists) {
			throw IllegalAccessException("rms", "CalBandpass");
		}
	
  		return rms;
 	}

 	/**
 	 * Set rms with the specified std::vector<std::vector<float > >.
 	 * @param rms The std::vector<std::vector<float > > value to which rms is to be set.
 	 
 	
 	 */
 	void CalBandpassRow::setRms (std::vector<std::vector<float > > rms) {
	
 		this->rms = rms;
	
		rmsExists = true;
	
 	}
	
	
	/**
	 * Mark rms, which is an optional field, as non-existent.
	 */
	void CalBandpassRow::clearRms () {
		rmsExists = false;
	}
	

	
	/**
	 * The attribute frequencyRange is optional. Return true if this attribute exists.
	 * @return true if and only if the frequencyRange attribute exists. 
	 */
	bool CalBandpassRow::isFrequencyRangeExists() const {
		return frequencyRangeExists;
	}
	

	
 	/**
 	 * Get frequencyRange, which is optional.
 	 * @return frequencyRange as std::vector<Frequency >
 	 * @throw IllegalAccessException If frequencyRange does not exist.
 	 */
 	std::vector<Frequency > CalBandpassRow::getFrequencyRange() const  {
		if (!frequencyRangeExists) {
			throw IllegalAccessException("frequencyRange", "CalBandpass");
		}
	
  		return frequencyRange;
 	}

 	/**
 	 * Set frequencyRange with the specified std::vector<Frequency >.
 	 * @param frequencyRange The std::vector<Frequency > value to which frequencyRange is to be set.
 	 
 	
 	 */
 	void CalBandpassRow::setFrequencyRange (std::vector<Frequency > frequencyRange) {
	
 		this->frequencyRange = frequencyRange;
	
		frequencyRangeExists = true;
	
 	}
	
	
	/**
	 * Mark frequencyRange, which is an optional field, as non-existent.
	 */
	void CalBandpassRow::clearFrequencyRange () {
		frequencyRangeExists = false;
	}
	

	
	/**
	 * The attribute numSpectralWindow is optional. Return true if this attribute exists.
	 * @return true if and only if the numSpectralWindow attribute exists. 
	 */
	bool CalBandpassRow::isNumSpectralWindowExists() const {
		return numSpectralWindowExists;
	}
	

	
 	/**
 	 * Get numSpectralWindow, which is optional.
 	 * @return numSpectralWindow as int
 	 * @throw IllegalAccessException If numSpectralWindow does not exist.
 	 */
 	int CalBandpassRow::getNumSpectralWindow() const  {
		if (!numSpectralWindowExists) {
			throw IllegalAccessException("numSpectralWindow", "CalBandpass");
		}
	
  		return numSpectralWindow;
 	}

 	/**
 	 * Set numSpectralWindow with the specified int.
 	 * @param numSpectralWindow The int value to which numSpectralWindow is to be set.
 	 
 	
 	 */
 	void CalBandpassRow::setNumSpectralWindow (int numSpectralWindow) {
	
 		this->numSpectralWindow = numSpectralWindow;
	
		numSpectralWindowExists = true;
	
 	}
	
	
	/**
	 * Mark numSpectralWindow, which is an optional field, as non-existent.
	 */
	void CalBandpassRow::clearNumSpectralWindow () {
		numSpectralWindowExists = false;
	}
	

	
	/**
	 * The attribute chanFreqStart is optional. Return true if this attribute exists.
	 * @return true if and only if the chanFreqStart attribute exists. 
	 */
	bool CalBandpassRow::isChanFreqStartExists() const {
		return chanFreqStartExists;
	}
	

	
 	/**
 	 * Get chanFreqStart, which is optional.
 	 * @return chanFreqStart as std::vector<Frequency >
 	 * @throw IllegalAccessException If chanFreqStart does not exist.
 	 */
 	std::vector<Frequency > CalBandpassRow::getChanFreqStart() const  {
		if (!chanFreqStartExists) {
			throw IllegalAccessException("chanFreqStart", "CalBandpass");
		}
	
  		return chanFreqStart;
 	}

 	/**
 	 * Set chanFreqStart with the specified std::vector<Frequency >.
 	 * @param chanFreqStart The std::vector<Frequency > value to which chanFreqStart is to be set.
 	 
 	
 	 */
 	void CalBandpassRow::setChanFreqStart (std::vector<Frequency > chanFreqStart) {
	
 		this->chanFreqStart = chanFreqStart;
	
		chanFreqStartExists = true;
	
 	}
	
	
	/**
	 * Mark chanFreqStart, which is an optional field, as non-existent.
	 */
	void CalBandpassRow::clearChanFreqStart () {
		chanFreqStartExists = false;
	}
	

	
	/**
	 * The attribute chanFreqStep is optional. Return true if this attribute exists.
	 * @return true if and only if the chanFreqStep attribute exists. 
	 */
	bool CalBandpassRow::isChanFreqStepExists() const {
		return chanFreqStepExists;
	}
	

	
 	/**
 	 * Get chanFreqStep, which is optional.
 	 * @return chanFreqStep as std::vector<Frequency >
 	 * @throw IllegalAccessException If chanFreqStep does not exist.
 	 */
 	std::vector<Frequency > CalBandpassRow::getChanFreqStep() const  {
		if (!chanFreqStepExists) {
			throw IllegalAccessException("chanFreqStep", "CalBandpass");
		}
	
  		return chanFreqStep;
 	}

 	/**
 	 * Set chanFreqStep with the specified std::vector<Frequency >.
 	 * @param chanFreqStep The std::vector<Frequency > value to which chanFreqStep is to be set.
 	 
 	
 	 */
 	void CalBandpassRow::setChanFreqStep (std::vector<Frequency > chanFreqStep) {
	
 		this->chanFreqStep = chanFreqStep;
	
		chanFreqStepExists = true;
	
 	}
	
	
	/**
	 * Mark chanFreqStep, which is an optional field, as non-existent.
	 */
	void CalBandpassRow::clearChanFreqStep () {
		chanFreqStepExists = false;
	}
	

	
	/**
	 * The attribute numSpectralWindowChan is optional. Return true if this attribute exists.
	 * @return true if and only if the numSpectralWindowChan attribute exists. 
	 */
	bool CalBandpassRow::isNumSpectralWindowChanExists() const {
		return numSpectralWindowChanExists;
	}
	

	
 	/**
 	 * Get numSpectralWindowChan, which is optional.
 	 * @return numSpectralWindowChan as std::vector<int >
 	 * @throw IllegalAccessException If numSpectralWindowChan does not exist.
 	 */
 	std::vector<int > CalBandpassRow::getNumSpectralWindowChan() const  {
		if (!numSpectralWindowChanExists) {
			throw IllegalAccessException("numSpectralWindowChan", "CalBandpass");
		}
	
  		return numSpectralWindowChan;
 	}

 	/**
 	 * Set numSpectralWindowChan with the specified std::vector<int >.
 	 * @param numSpectralWindowChan The std::vector<int > value to which numSpectralWindowChan is to be set.
 	 
 	
 	 */
 	void CalBandpassRow::setNumSpectralWindowChan (std::vector<int > numSpectralWindowChan) {
	
 		this->numSpectralWindowChan = numSpectralWindowChan;
	
		numSpectralWindowChanExists = true;
	
 	}
	
	
	/**
	 * Mark numSpectralWindowChan, which is an optional field, as non-existent.
	 */
	void CalBandpassRow::clearNumSpectralWindowChan () {
		numSpectralWindowChanExists = false;
	}
	

	
	/**
	 * The attribute spectrum is optional. Return true if this attribute exists.
	 * @return true if and only if the spectrum attribute exists. 
	 */
	bool CalBandpassRow::isSpectrumExists() const {
		return spectrumExists;
	}
	

	
 	/**
 	 * Get spectrum, which is optional.
 	 * @return spectrum as std::vector<std::vector<std::vector<float > > >
 	 * @throw IllegalAccessException If spectrum does not exist.
 	 */
 	std::vector<std::vector<std::vector<float > > > CalBandpassRow::getSpectrum() const  {
		if (!spectrumExists) {
			throw IllegalAccessException("spectrum", "CalBandpass");
		}
	
  		return spectrum;
 	}

 	/**
 	 * Set spectrum with the specified std::vector<std::vector<std::vector<float > > >.
 	 * @param spectrum The std::vector<std::vector<std::vector<float > > > value to which spectrum is to be set.
 	 
 	
 	 */
 	void CalBandpassRow::setSpectrum (std::vector<std::vector<std::vector<float > > > spectrum) {
	
 		this->spectrum = spectrum;
	
		spectrumExists = true;
	
 	}
	
	
	/**
	 * Mark spectrum, which is an optional field, as non-existent.
	 */
	void CalBandpassRow::clearSpectrum () {
		spectrumExists = false;
	}
	

	
	///////////////////////////////////////////////
	// Extrinsic Table Attributes getters/setters//
	///////////////////////////////////////////////
	
	

	
 	/**
 	 * Get calDataId.
 	 * @return calDataId as Tag
 	 */
 	Tag CalBandpassRow::getCalDataId() const {
	
  		return calDataId;
 	}

 	/**
 	 * Set calDataId with the specified Tag.
 	 * @param calDataId The Tag value to which calDataId is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void CalBandpassRow::setCalDataId (Tag calDataId)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("calDataId", "CalBandpass");
		
  		}
  	
 		this->calDataId = calDataId;
	
 	}
	
	

	

	
 	/**
 	 * Get calReductionId.
 	 * @return calReductionId as Tag
 	 */
 	Tag CalBandpassRow::getCalReductionId() const {
	
  		return calReductionId;
 	}

 	/**
 	 * Set calReductionId with the specified Tag.
 	 * @param calReductionId The Tag value to which calReductionId is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void CalBandpassRow::setCalReductionId (Tag calReductionId)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("calReductionId", "CalBandpass");
		
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
	 CalReductionRow* CalBandpassRow::getCalReductionUsingCalReductionId() {
	 
	 	return table.getContainer().getCalReduction().getRowByKey(calReductionId);
	 }
	 

	

	
	
	
		

	/**
	 * Returns the pointer to the row in the CalData table having CalData.calDataId == calDataId
	 * @return a CalDataRow*
	 * 
	 
	 */
	 CalDataRow* CalBandpassRow::getCalDataUsingCalDataId() {
	 
	 	return table.getContainer().getCalData().getRowByKey(calDataId);
	 }
	 

	

	
	/**
	 * Create a CalBandpassRow.
	 * <p>
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 */ 
	CalBandpassRow::CalBandpassRow (CalBandpassTable &t) : table(t) {
		hasBeenAdded = false;
		
	
	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	
		numBaselineExists = false;
	

	
		numFreqExists = false;
	

	
		rmsExists = false;
	

	
		frequencyRangeExists = false;
	

	
		numSpectralWindowExists = false;
	

	
		chanFreqStartExists = false;
	

	
		chanFreqStepExists = false;
	

	
		numSpectralWindowChanExists = false;
	

	
		spectrumExists = false;
	

	
	

	

	
	
	
	
// This attribute is scalar and has an enumeration type. Let's initialize it to some valid value (the 1st of the enumeration).		
basebandName = CBasebandName::from_int(0);
	

	
// This attribute is scalar and has an enumeration type. Let's initialize it to some valid value (the 1st of the enumeration).		
sideband = CNetSideband::from_int(0);
	

	
// This attribute is scalar and has an enumeration type. Let's initialize it to some valid value (the 1st of the enumeration).		
atmPhaseCorrection = CAtmPhaseCorrection::from_int(0);
	

	
// This attribute is scalar and has an enumeration type. Let's initialize it to some valid value (the 1st of the enumeration).		
typeCurve = CCalCurveType::from_int(0);
	

	
// This attribute is scalar and has an enumeration type. Let's initialize it to some valid value (the 1st of the enumeration).		
receiverBand = CReceiverBand::from_int(0);
	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	
	
	 fromBinMethods["basebandName"] = &CalBandpassRow::basebandNameFromBin; 
	 fromBinMethods["sideband"] = &CalBandpassRow::sidebandFromBin; 
	 fromBinMethods["atmPhaseCorrection"] = &CalBandpassRow::atmPhaseCorrectionFromBin; 
	 fromBinMethods["typeCurve"] = &CalBandpassRow::typeCurveFromBin; 
	 fromBinMethods["receiverBand"] = &CalBandpassRow::receiverBandFromBin; 
	 fromBinMethods["calDataId"] = &CalBandpassRow::calDataIdFromBin; 
	 fromBinMethods["calReductionId"] = &CalBandpassRow::calReductionIdFromBin; 
	 fromBinMethods["startValidTime"] = &CalBandpassRow::startValidTimeFromBin; 
	 fromBinMethods["endValidTime"] = &CalBandpassRow::endValidTimeFromBin; 
	 fromBinMethods["numAntenna"] = &CalBandpassRow::numAntennaFromBin; 
	 fromBinMethods["numPoly"] = &CalBandpassRow::numPolyFromBin; 
	 fromBinMethods["numReceptor"] = &CalBandpassRow::numReceptorFromBin; 
	 fromBinMethods["antennaNames"] = &CalBandpassRow::antennaNamesFromBin; 
	 fromBinMethods["refAntennaName"] = &CalBandpassRow::refAntennaNameFromBin; 
	 fromBinMethods["freqLimits"] = &CalBandpassRow::freqLimitsFromBin; 
	 fromBinMethods["polarizationTypes"] = &CalBandpassRow::polarizationTypesFromBin; 
	 fromBinMethods["curve"] = &CalBandpassRow::curveFromBin; 
	 fromBinMethods["reducedChiSquared"] = &CalBandpassRow::reducedChiSquaredFromBin; 
		
	
	 fromBinMethods["numBaseline"] = &CalBandpassRow::numBaselineFromBin; 
	 fromBinMethods["numFreq"] = &CalBandpassRow::numFreqFromBin; 
	 fromBinMethods["rms"] = &CalBandpassRow::rmsFromBin; 
	 fromBinMethods["frequencyRange"] = &CalBandpassRow::frequencyRangeFromBin; 
	 fromBinMethods["numSpectralWindow"] = &CalBandpassRow::numSpectralWindowFromBin; 
	 fromBinMethods["chanFreqStart"] = &CalBandpassRow::chanFreqStartFromBin; 
	 fromBinMethods["chanFreqStep"] = &CalBandpassRow::chanFreqStepFromBin; 
	 fromBinMethods["numSpectralWindowChan"] = &CalBandpassRow::numSpectralWindowChanFromBin; 
	 fromBinMethods["spectrum"] = &CalBandpassRow::spectrumFromBin; 
	
	
	
	
				 
	fromTextMethods["basebandName"] = &CalBandpassRow::basebandNameFromText;
		 
	
				 
	fromTextMethods["sideband"] = &CalBandpassRow::sidebandFromText;
		 
	
				 
	fromTextMethods["atmPhaseCorrection"] = &CalBandpassRow::atmPhaseCorrectionFromText;
		 
	
				 
	fromTextMethods["typeCurve"] = &CalBandpassRow::typeCurveFromText;
		 
	
				 
	fromTextMethods["receiverBand"] = &CalBandpassRow::receiverBandFromText;
		 
	
				 
	fromTextMethods["calDataId"] = &CalBandpassRow::calDataIdFromText;
		 
	
				 
	fromTextMethods["calReductionId"] = &CalBandpassRow::calReductionIdFromText;
		 
	
				 
	fromTextMethods["startValidTime"] = &CalBandpassRow::startValidTimeFromText;
		 
	
				 
	fromTextMethods["endValidTime"] = &CalBandpassRow::endValidTimeFromText;
		 
	
				 
	fromTextMethods["numAntenna"] = &CalBandpassRow::numAntennaFromText;
		 
	
				 
	fromTextMethods["numPoly"] = &CalBandpassRow::numPolyFromText;
		 
	
				 
	fromTextMethods["numReceptor"] = &CalBandpassRow::numReceptorFromText;
		 
	
				 
	fromTextMethods["antennaNames"] = &CalBandpassRow::antennaNamesFromText;
		 
	
				 
	fromTextMethods["refAntennaName"] = &CalBandpassRow::refAntennaNameFromText;
		 
	
				 
	fromTextMethods["freqLimits"] = &CalBandpassRow::freqLimitsFromText;
		 
	
				 
	fromTextMethods["polarizationTypes"] = &CalBandpassRow::polarizationTypesFromText;
		 
	
				 
	fromTextMethods["curve"] = &CalBandpassRow::curveFromText;
		 
	
				 
	fromTextMethods["reducedChiSquared"] = &CalBandpassRow::reducedChiSquaredFromText;
		 
	

	 
				
	fromTextMethods["numBaseline"] = &CalBandpassRow::numBaselineFromText;
		 	
	 
				
	fromTextMethods["numFreq"] = &CalBandpassRow::numFreqFromText;
		 	
	 
				
	fromTextMethods["rms"] = &CalBandpassRow::rmsFromText;
		 	
	 
				
	fromTextMethods["frequencyRange"] = &CalBandpassRow::frequencyRangeFromText;
		 	
	 
				
	fromTextMethods["numSpectralWindow"] = &CalBandpassRow::numSpectralWindowFromText;
		 	
	 
				
	fromTextMethods["chanFreqStart"] = &CalBandpassRow::chanFreqStartFromText;
		 	
	 
				
	fromTextMethods["chanFreqStep"] = &CalBandpassRow::chanFreqStepFromText;
		 	
	 
				
	fromTextMethods["numSpectralWindowChan"] = &CalBandpassRow::numSpectralWindowChanFromText;
		 	
	 
				
	fromTextMethods["spectrum"] = &CalBandpassRow::spectrumFromText;
		 	
		
	}
	
	CalBandpassRow::CalBandpassRow (CalBandpassTable &t, CalBandpassRow *row) : table(t) {
		hasBeenAdded = false;
		
		if (row == 0) {
	
	
	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	

	
		numBaselineExists = false;
	

	
		numFreqExists = false;
	

	
		rmsExists = false;
	

	
		frequencyRangeExists = false;
	

	
		numSpectralWindowExists = false;
	

	
		chanFreqStartExists = false;
	

	
		chanFreqStepExists = false;
	

	
		numSpectralWindowChanExists = false;
	

	
		spectrumExists = false;
	

	
	

	
		
		}
		else {
	
		
			basebandName = row->basebandName;
		
			sideband = row->sideband;
		
			atmPhaseCorrection = row->atmPhaseCorrection;
		
			typeCurve = row->typeCurve;
		
			receiverBand = row->receiverBand;
		
			calDataId = row->calDataId;
		
			calReductionId = row->calReductionId;
		
		
		
		
			startValidTime = row->startValidTime;
		
			endValidTime = row->endValidTime;
		
			numAntenna = row->numAntenna;
		
			numPoly = row->numPoly;
		
			numReceptor = row->numReceptor;
		
			antennaNames = row->antennaNames;
		
			refAntennaName = row->refAntennaName;
		
			freqLimits = row->freqLimits;
		
			polarizationTypes = row->polarizationTypes;
		
			curve = row->curve;
		
			reducedChiSquared = row->reducedChiSquared;
		
		
		
		
		if (row->numBaselineExists) {
			numBaseline = row->numBaseline;		
			numBaselineExists = true;
		}
		else
			numBaselineExists = false;
		
		if (row->numFreqExists) {
			numFreq = row->numFreq;		
			numFreqExists = true;
		}
		else
			numFreqExists = false;
		
		if (row->rmsExists) {
			rms = row->rms;		
			rmsExists = true;
		}
		else
			rmsExists = false;
		
		if (row->frequencyRangeExists) {
			frequencyRange = row->frequencyRange;		
			frequencyRangeExists = true;
		}
		else
			frequencyRangeExists = false;
		
		if (row->numSpectralWindowExists) {
			numSpectralWindow = row->numSpectralWindow;		
			numSpectralWindowExists = true;
		}
		else
			numSpectralWindowExists = false;
		
		if (row->chanFreqStartExists) {
			chanFreqStart = row->chanFreqStart;		
			chanFreqStartExists = true;
		}
		else
			chanFreqStartExists = false;
		
		if (row->chanFreqStepExists) {
			chanFreqStep = row->chanFreqStep;		
			chanFreqStepExists = true;
		}
		else
			chanFreqStepExists = false;
		
		if (row->numSpectralWindowChanExists) {
			numSpectralWindowChan = row->numSpectralWindowChan;		
			numSpectralWindowChanExists = true;
		}
		else
			numSpectralWindowChanExists = false;
		
		if (row->spectrumExists) {
			spectrum = row->spectrum;		
			spectrumExists = true;
		}
		else
			spectrumExists = false;
		
		}
		
		 fromBinMethods["basebandName"] = &CalBandpassRow::basebandNameFromBin; 
		 fromBinMethods["sideband"] = &CalBandpassRow::sidebandFromBin; 
		 fromBinMethods["atmPhaseCorrection"] = &CalBandpassRow::atmPhaseCorrectionFromBin; 
		 fromBinMethods["typeCurve"] = &CalBandpassRow::typeCurveFromBin; 
		 fromBinMethods["receiverBand"] = &CalBandpassRow::receiverBandFromBin; 
		 fromBinMethods["calDataId"] = &CalBandpassRow::calDataIdFromBin; 
		 fromBinMethods["calReductionId"] = &CalBandpassRow::calReductionIdFromBin; 
		 fromBinMethods["startValidTime"] = &CalBandpassRow::startValidTimeFromBin; 
		 fromBinMethods["endValidTime"] = &CalBandpassRow::endValidTimeFromBin; 
		 fromBinMethods["numAntenna"] = &CalBandpassRow::numAntennaFromBin; 
		 fromBinMethods["numPoly"] = &CalBandpassRow::numPolyFromBin; 
		 fromBinMethods["numReceptor"] = &CalBandpassRow::numReceptorFromBin; 
		 fromBinMethods["antennaNames"] = &CalBandpassRow::antennaNamesFromBin; 
		 fromBinMethods["refAntennaName"] = &CalBandpassRow::refAntennaNameFromBin; 
		 fromBinMethods["freqLimits"] = &CalBandpassRow::freqLimitsFromBin; 
		 fromBinMethods["polarizationTypes"] = &CalBandpassRow::polarizationTypesFromBin; 
		 fromBinMethods["curve"] = &CalBandpassRow::curveFromBin; 
		 fromBinMethods["reducedChiSquared"] = &CalBandpassRow::reducedChiSquaredFromBin; 
			
	
		 fromBinMethods["numBaseline"] = &CalBandpassRow::numBaselineFromBin; 
		 fromBinMethods["numFreq"] = &CalBandpassRow::numFreqFromBin; 
		 fromBinMethods["rms"] = &CalBandpassRow::rmsFromBin; 
		 fromBinMethods["frequencyRange"] = &CalBandpassRow::frequencyRangeFromBin; 
		 fromBinMethods["numSpectralWindow"] = &CalBandpassRow::numSpectralWindowFromBin; 
		 fromBinMethods["chanFreqStart"] = &CalBandpassRow::chanFreqStartFromBin; 
		 fromBinMethods["chanFreqStep"] = &CalBandpassRow::chanFreqStepFromBin; 
		 fromBinMethods["numSpectralWindowChan"] = &CalBandpassRow::numSpectralWindowChanFromBin; 
		 fromBinMethods["spectrum"] = &CalBandpassRow::spectrumFromBin; 
			
	}

	
	bool CalBandpassRow::compareNoAutoInc(BasebandNameMod::BasebandName basebandName, NetSidebandMod::NetSideband sideband, AtmPhaseCorrectionMod::AtmPhaseCorrection atmPhaseCorrection, CalCurveTypeMod::CalCurveType typeCurve, ReceiverBandMod::ReceiverBand receiverBand, Tag calDataId, Tag calReductionId, ArrayTime startValidTime, ArrayTime endValidTime, int numAntenna, int numPoly, int numReceptor, std::vector<std::string > antennaNames, std::string refAntennaName, std::vector<Frequency > freqLimits, std::vector<PolarizationTypeMod::PolarizationType > polarizationTypes, std::vector<std::vector<std::vector<float > > > curve, std::vector<double > reducedChiSquared) {
		bool result;
		result = true;
		
	
		
		result = result && (this->basebandName == basebandName);
		
		if (!result) return false;
	

	
		
		result = result && (this->sideband == sideband);
		
		if (!result) return false;
	

	
		
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
	

	
		
		result = result && (this->freqLimits == freqLimits);
		
		if (!result) return false;
	

	
		
		result = result && (this->polarizationTypes == polarizationTypes);
		
		if (!result) return false;
	

	
		
		result = result && (this->curve == curve);
		
		if (!result) return false;
	

	
		
		result = result && (this->reducedChiSquared == reducedChiSquared);
		
		if (!result) return false;
	

		return result;
	}	
	
	
	
	bool CalBandpassRow::compareRequiredValue(ArrayTime startValidTime, ArrayTime endValidTime, int numAntenna, int numPoly, int numReceptor, std::vector<std::string > antennaNames, std::string refAntennaName, std::vector<Frequency > freqLimits, std::vector<PolarizationTypeMod::PolarizationType > polarizationTypes, std::vector<std::vector<std::vector<float > > > curve, std::vector<double > reducedChiSquared) {
		bool result;
		result = true;
		
	
		if (!(this->startValidTime == startValidTime)) return false;
	

	
		if (!(this->endValidTime == endValidTime)) return false;
	

	
		if (!(this->numAntenna == numAntenna)) return false;
	

	
		if (!(this->numPoly == numPoly)) return false;
	

	
		if (!(this->numReceptor == numReceptor)) return false;
	

	
		if (!(this->antennaNames == antennaNames)) return false;
	

	
		if (!(this->refAntennaName == refAntennaName)) return false;
	

	
		if (!(this->freqLimits == freqLimits)) return false;
	

	
		if (!(this->polarizationTypes == polarizationTypes)) return false;
	

	
		if (!(this->curve == curve)) return false;
	

	
		if (!(this->reducedChiSquared == reducedChiSquared)) return false;
	

		return result;
	}
	
	
	/**
	 * Return true if all required attributes of the value part are equal to their homologues
	 * in x and false otherwise.
	 *

	 * @param x a pointer on the CalBandpassRow whose required attributes of the value part 

	 * will be compared with those of this.
	 * @return a boolean.
	 */
	bool CalBandpassRow::equalByRequiredValue(CalBandpassRow*  x ) {
		
			
		if (this->startValidTime != x->startValidTime) return false;
			
		if (this->endValidTime != x->endValidTime) return false;
			
		if (this->numAntenna != x->numAntenna) return false;
			
		if (this->numPoly != x->numPoly) return false;
			
		if (this->numReceptor != x->numReceptor) return false;
			
		if (this->antennaNames != x->antennaNames) return false;
			
		if (this->refAntennaName != x->refAntennaName) return false;
			
		if (this->freqLimits != x->freqLimits) return false;
			
		if (this->polarizationTypes != x->polarizationTypes) return false;
			
		if (this->curve != x->curve) return false;
			
		if (this->reducedChiSquared != x->reducedChiSquared) return false;
			
		
		return true;
	}	
	
/*
	 map<string, CalBandpassAttributeFromBin> CalBandpassRow::initFromBinMethods() {
		map<string, CalBandpassAttributeFromBin> result;
		
		result["basebandName"] = &CalBandpassRow::basebandNameFromBin;
		result["sideband"] = &CalBandpassRow::sidebandFromBin;
		result["atmPhaseCorrection"] = &CalBandpassRow::atmPhaseCorrectionFromBin;
		result["typeCurve"] = &CalBandpassRow::typeCurveFromBin;
		result["receiverBand"] = &CalBandpassRow::receiverBandFromBin;
		result["calDataId"] = &CalBandpassRow::calDataIdFromBin;
		result["calReductionId"] = &CalBandpassRow::calReductionIdFromBin;
		result["startValidTime"] = &CalBandpassRow::startValidTimeFromBin;
		result["endValidTime"] = &CalBandpassRow::endValidTimeFromBin;
		result["numAntenna"] = &CalBandpassRow::numAntennaFromBin;
		result["numPoly"] = &CalBandpassRow::numPolyFromBin;
		result["numReceptor"] = &CalBandpassRow::numReceptorFromBin;
		result["antennaNames"] = &CalBandpassRow::antennaNamesFromBin;
		result["refAntennaName"] = &CalBandpassRow::refAntennaNameFromBin;
		result["freqLimits"] = &CalBandpassRow::freqLimitsFromBin;
		result["polarizationTypes"] = &CalBandpassRow::polarizationTypesFromBin;
		result["curve"] = &CalBandpassRow::curveFromBin;
		result["reducedChiSquared"] = &CalBandpassRow::reducedChiSquaredFromBin;
		
		
		result["numBaseline"] = &CalBandpassRow::numBaselineFromBin;
		result["numFreq"] = &CalBandpassRow::numFreqFromBin;
		result["rms"] = &CalBandpassRow::rmsFromBin;
		result["frequencyRange"] = &CalBandpassRow::frequencyRangeFromBin;
		result["numSpectralWindow"] = &CalBandpassRow::numSpectralWindowFromBin;
		result["chanFreqStart"] = &CalBandpassRow::chanFreqStartFromBin;
		result["chanFreqStep"] = &CalBandpassRow::chanFreqStepFromBin;
		result["numSpectralWindowChan"] = &CalBandpassRow::numSpectralWindowChanFromBin;
		result["spectrum"] = &CalBandpassRow::spectrumFromBin;
			
		
		return result;	
	}
*/	
} // End namespace asdm
 
