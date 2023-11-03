
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
 * File SpectralWindowRow.cpp
 */
 
#include <vector>
#include <set>

#include <alma/ASDM/ASDM.h>
#include <alma/ASDM/SpectralWindowRow.h>
#include <alma/ASDM/SpectralWindowTable.h>

#include <alma/ASDM/SpectralWindowTable.h>
#include <alma/ASDM/SpectralWindowRow.h>

#include <alma/ASDM/SpectralWindowTable.h>
#include <alma/ASDM/SpectralWindowRow.h>

#include <alma/ASDM/DopplerTable.h>
#include <alma/ASDM/DopplerRow.h>
	

using asdm::ASDM;
using asdm::SpectralWindowRow;
using asdm::SpectralWindowTable;

using asdm::SpectralWindowTable;
using asdm::SpectralWindowRow;

using asdm::SpectralWindowTable;
using asdm::SpectralWindowRow;

using asdm::DopplerTable;
using asdm::DopplerRow;


#include <alma/ASDM/Parser.h>

#include <alma/ASDM/EnumerationParser.h>
#include <alma/ASDM/ASDMValuesParser.h>
 
#include <alma/ASDM/InvalidArgumentException.h>

using namespace std;

namespace asdm {
	SpectralWindowRow::~SpectralWindowRow() {
	}

	/**
	 * Return the table to which this row belongs.
	 */
	SpectralWindowTable &SpectralWindowRow::getTable() const {
		return table;
	}

	bool SpectralWindowRow::isAdded() const {
		return hasBeenAdded;
	}	

	void SpectralWindowRow::isAdded(bool added) {
		hasBeenAdded = added;
	}
	
#ifndef WITHOUT_ACS
	using asdmIDL::SpectralWindowRowIDL;
#endif
	
#ifndef WITHOUT_ACS
	/**
	 * Return this row in the form of an IDL struct.
	 * @return The values of this row as a SpectralWindowRowIDL struct.
	 */
	SpectralWindowRowIDL *SpectralWindowRow::toIDL() const {
		SpectralWindowRowIDL *x = new SpectralWindowRowIDL ();
		
		// Fill the IDL structure.
	
		
	
  		
		
		
			
		x->spectralWindowId = spectralWindowId.toIDLTag();
			
		
	

	
  		
		
		
			
				
		x->basebandName = basebandName;
 				
 			
		
	

	
  		
		
		
			
				
		x->netSideband = netSideband;
 				
 			
		
	

	
  		
		
		
			
				
		x->numChan = numChan;
 				
 			
		
	

	
  		
		
		x->numBinExists = numBinExists;
		
		
			
				
		x->numBin = numBin;
 				
 			
		
	

	
  		
		
		
			
		x->refFreq = refFreq.toIDLFrequency();
			
		
	

	
  		
		
		
			
				
		x->sidebandProcessingMode = sidebandProcessingMode;
 				
 			
		
	

	
  		
		
		
			
		x->totBandwidth = totBandwidth.toIDLFrequency();
			
		
	

	
  		
		
		
			
				
		x->windowFunction = windowFunction;
 				
 			
		
	

	
  		
		
		x->chanFreqStartExists = chanFreqStartExists;
		
		
			
		x->chanFreqStart = chanFreqStart.toIDLFrequency();
			
		
	

	
  		
		
		x->chanFreqStepExists = chanFreqStepExists;
		
		
			
		x->chanFreqStep = chanFreqStep.toIDLFrequency();
			
		
	

	
  		
		
		x->chanFreqArrayExists = chanFreqArrayExists;
		
		
			
		x->chanFreqArray.length(chanFreqArray.size());
		for (unsigned int i = 0; i < chanFreqArray.size(); ++i) {
			
			x->chanFreqArray[i] = chanFreqArray.at(i).toIDLFrequency();
			
	 	}
			
		
	

	
  		
		
		x->chanWidthExists = chanWidthExists;
		
		
			
		x->chanWidth = chanWidth.toIDLFrequency();
			
		
	

	
  		
		
		x->chanWidthArrayExists = chanWidthArrayExists;
		
		
			
		x->chanWidthArray.length(chanWidthArray.size());
		for (unsigned int i = 0; i < chanWidthArray.size(); ++i) {
			
			x->chanWidthArray[i] = chanWidthArray.at(i).toIDLFrequency();
			
	 	}
			
		
	

	
  		
		
		x->correlationBitExists = correlationBitExists;
		
		
			
				
		x->correlationBit = correlationBit;
 				
 			
		
	

	
  		
		
		x->effectiveBwExists = effectiveBwExists;
		
		
			
		x->effectiveBw = effectiveBw.toIDLFrequency();
			
		
	

	
  		
		
		x->effectiveBwArrayExists = effectiveBwArrayExists;
		
		
			
		x->effectiveBwArray.length(effectiveBwArray.size());
		for (unsigned int i = 0; i < effectiveBwArray.size(); ++i) {
			
			x->effectiveBwArray[i] = effectiveBwArray.at(i).toIDLFrequency();
			
	 	}
			
		
	

	
  		
		
		x->freqGroupExists = freqGroupExists;
		
		
			
				
		x->freqGroup = freqGroup;
 				
 			
		
	

	
  		
		
		x->freqGroupNameExists = freqGroupNameExists;
		
		
			
				
		x->freqGroupName = CORBA::string_dup(freqGroupName.c_str());
				
 			
		
	

	
  		
		
		x->lineArrayExists = lineArrayExists;
		
		
			
		x->lineArray.length(lineArray.size());
		for (unsigned int i = 0; i < lineArray.size(); ++i) {
			
				
			x->lineArray[i] = lineArray.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		x->measFreqRefExists = measFreqRefExists;
		
		
			
				
		x->measFreqRef = measFreqRef;
 				
 			
		
	

	
  		
		
		x->nameExists = nameExists;
		
		
			
				
		x->name = CORBA::string_dup(name.c_str());
				
 			
		
	

	
  		
		
		x->oversamplingExists = oversamplingExists;
		
		
			
				
		x->oversampling = oversampling;
 				
 			
		
	

	
  		
		
		x->quantizationExists = quantizationExists;
		
		
			
				
		x->quantization = quantization;
 				
 			
		
	

	
  		
		
		x->refChanExists = refChanExists;
		
		
			
				
		x->refChan = refChan;
 				
 			
		
	

	
  		
		
		x->resolutionExists = resolutionExists;
		
		
			
		x->resolution = resolution.toIDLFrequency();
			
		
	

	
  		
		
		x->resolutionArrayExists = resolutionArrayExists;
		
		
			
		x->resolutionArray.length(resolutionArray.size());
		for (unsigned int i = 0; i < resolutionArray.size(); ++i) {
			
			x->resolutionArray[i] = resolutionArray.at(i).toIDLFrequency();
			
	 	}
			
		
	

	
  		
		
		x->numAssocValuesExists = numAssocValuesExists;
		
		
			
				
		x->numAssocValues = numAssocValues;
 				
 			
		
	

	
  		
		
		x->assocNatureExists = assocNatureExists;
		
		
			
		x->assocNature.length(assocNature.size());
		for (unsigned int i = 0; i < assocNature.size(); ++i) {
			
				
			x->assocNature[i] = assocNature.at(i);
	 			
	 		
	 	}
			
		
	

	
	
		
	
  	
 		
 		
		x->assocSpectralWindowIdExists = assocSpectralWindowIdExists;
		
		
		
		x->assocSpectralWindowId.length(assocSpectralWindowId.size());
		for (unsigned int i = 0; i < assocSpectralWindowId.size(); ++i) {
			
			x->assocSpectralWindowId[i] = assocSpectralWindowId.at(i).toIDLTag();
			
	 	}
	 	 		
  	

	
  	
 		
 		
		x->dopplerIdExists = dopplerIdExists;
		
		
	 	
			
				
		x->dopplerId = dopplerId;
 				
 			
	 	 		
  	

	
  	
 		
 		
		x->imageSpectralWindowIdExists = imageSpectralWindowIdExists;
		
		
	 	
			
		x->imageSpectralWindowId = imageSpectralWindowId.toIDLTag();
			
	 	 		
  	

	
		
	

	

	

		
		return x;
	
	}
	
	void SpectralWindowRow::toIDL(asdmIDL::SpectralWindowRowIDL& x) const {
		// Set the x's fields.
	
		
	
  		
		
		
			
		x.spectralWindowId = spectralWindowId.toIDLTag();
			
		
	

	
  		
		
		
			
				
		x.basebandName = basebandName;
 				
 			
		
	

	
  		
		
		
			
				
		x.netSideband = netSideband;
 				
 			
		
	

	
  		
		
		
			
				
		x.numChan = numChan;
 				
 			
		
	

	
  		
		
		x.numBinExists = numBinExists;
		
		
			
				
		x.numBin = numBin;
 				
 			
		
	

	
  		
		
		
			
		x.refFreq = refFreq.toIDLFrequency();
			
		
	

	
  		
		
		
			
				
		x.sidebandProcessingMode = sidebandProcessingMode;
 				
 			
		
	

	
  		
		
		
			
		x.totBandwidth = totBandwidth.toIDLFrequency();
			
		
	

	
  		
		
		
			
				
		x.windowFunction = windowFunction;
 				
 			
		
	

	
  		
		
		x.chanFreqStartExists = chanFreqStartExists;
		
		
			
		x.chanFreqStart = chanFreqStart.toIDLFrequency();
			
		
	

	
  		
		
		x.chanFreqStepExists = chanFreqStepExists;
		
		
			
		x.chanFreqStep = chanFreqStep.toIDLFrequency();
			
		
	

	
  		
		
		x.chanFreqArrayExists = chanFreqArrayExists;
		
		
			
		x.chanFreqArray.length(chanFreqArray.size());
		for (unsigned int i = 0; i < chanFreqArray.size(); ++i) {
			
			x.chanFreqArray[i] = chanFreqArray.at(i).toIDLFrequency();
			
	 	}
			
		
	

	
  		
		
		x.chanWidthExists = chanWidthExists;
		
		
			
		x.chanWidth = chanWidth.toIDLFrequency();
			
		
	

	
  		
		
		x.chanWidthArrayExists = chanWidthArrayExists;
		
		
			
		x.chanWidthArray.length(chanWidthArray.size());
		for (unsigned int i = 0; i < chanWidthArray.size(); ++i) {
			
			x.chanWidthArray[i] = chanWidthArray.at(i).toIDLFrequency();
			
	 	}
			
		
	

	
  		
		
		x.correlationBitExists = correlationBitExists;
		
		
			
				
		x.correlationBit = correlationBit;
 				
 			
		
	

	
  		
		
		x.effectiveBwExists = effectiveBwExists;
		
		
			
		x.effectiveBw = effectiveBw.toIDLFrequency();
			
		
	

	
  		
		
		x.effectiveBwArrayExists = effectiveBwArrayExists;
		
		
			
		x.effectiveBwArray.length(effectiveBwArray.size());
		for (unsigned int i = 0; i < effectiveBwArray.size(); ++i) {
			
			x.effectiveBwArray[i] = effectiveBwArray.at(i).toIDLFrequency();
			
	 	}
			
		
	

	
  		
		
		x.freqGroupExists = freqGroupExists;
		
		
			
				
		x.freqGroup = freqGroup;
 				
 			
		
	

	
  		
		
		x.freqGroupNameExists = freqGroupNameExists;
		
		
			
				
		x.freqGroupName = CORBA::string_dup(freqGroupName.c_str());
				
 			
		
	

	
  		
		
		x.lineArrayExists = lineArrayExists;
		
		
			
		x.lineArray.length(lineArray.size());
		for (unsigned int i = 0; i < lineArray.size(); ++i) {
			
				
			x.lineArray[i] = lineArray.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		x.measFreqRefExists = measFreqRefExists;
		
		
			
				
		x.measFreqRef = measFreqRef;
 				
 			
		
	

	
  		
		
		x.nameExists = nameExists;
		
		
			
				
		x.name = CORBA::string_dup(name.c_str());
				
 			
		
	

	
  		
		
		x.oversamplingExists = oversamplingExists;
		
		
			
				
		x.oversampling = oversampling;
 				
 			
		
	

	
  		
		
		x.quantizationExists = quantizationExists;
		
		
			
				
		x.quantization = quantization;
 				
 			
		
	

	
  		
		
		x.refChanExists = refChanExists;
		
		
			
				
		x.refChan = refChan;
 				
 			
		
	

	
  		
		
		x.resolutionExists = resolutionExists;
		
		
			
		x.resolution = resolution.toIDLFrequency();
			
		
	

	
  		
		
		x.resolutionArrayExists = resolutionArrayExists;
		
		
			
		x.resolutionArray.length(resolutionArray.size());
		for (unsigned int i = 0; i < resolutionArray.size(); ++i) {
			
			x.resolutionArray[i] = resolutionArray.at(i).toIDLFrequency();
			
	 	}
			
		
	

	
  		
		
		x.numAssocValuesExists = numAssocValuesExists;
		
		
			
				
		x.numAssocValues = numAssocValues;
 				
 			
		
	

	
  		
		
		x.assocNatureExists = assocNatureExists;
		
		
			
		x.assocNature.length(assocNature.size());
		for (unsigned int i = 0; i < assocNature.size(); ++i) {
			
				
			x.assocNature[i] = assocNature.at(i);
	 			
	 		
	 	}
			
		
	

	
	
		
	
  	
 		
 		
		x.assocSpectralWindowIdExists = assocSpectralWindowIdExists;
		
		
		
		x.assocSpectralWindowId.length(assocSpectralWindowId.size());
		for (unsigned int i = 0; i < assocSpectralWindowId.size(); ++i) {
			
			x.assocSpectralWindowId[i] = assocSpectralWindowId.at(i).toIDLTag();
			
	 	}
	 	 		
  	

	
  	
 		
 		
		x.dopplerIdExists = dopplerIdExists;
		
		
	 	
			
				
		x.dopplerId = dopplerId;
 				
 			
	 	 		
  	

	
  	
 		
 		
		x.imageSpectralWindowIdExists = imageSpectralWindowIdExists;
		
		
	 	
			
		x.imageSpectralWindowId = imageSpectralWindowId.toIDLTag();
			
	 	 		
  	

	
		
	

	

	

	
	}
#endif
	

#ifndef WITHOUT_ACS
	/**
	 * Fill the values of this row from the IDL struct SpectralWindowRowIDL.
	 * @param x The IDL struct containing the values used to fill this row.
	 */
	void SpectralWindowRow::setFromIDL (SpectralWindowRowIDL x){
		try {
		// Fill the values from x.
	
		
	
		
		
			
		setSpectralWindowId(Tag (x.spectralWindowId));
			
 		
		
	

	
		
		
			
		setBasebandName(x.basebandName);
  			
 		
		
	

	
		
		
			
		setNetSideband(x.netSideband);
  			
 		
		
	

	
		
		
			
		setNumChan(x.numChan);
  			
 		
		
	

	
		
		numBinExists = x.numBinExists;
		if (x.numBinExists) {
		
		
			
		setNumBin(x.numBin);
  			
 		
		
		}
		
	

	
		
		
			
		setRefFreq(Frequency (x.refFreq));
			
 		
		
	

	
		
		
			
		setSidebandProcessingMode(x.sidebandProcessingMode);
  			
 		
		
	

	
		
		
			
		setTotBandwidth(Frequency (x.totBandwidth));
			
 		
		
	

	
		
		
			
		setWindowFunction(x.windowFunction);
  			
 		
		
	

	
		
		chanFreqStartExists = x.chanFreqStartExists;
		if (x.chanFreqStartExists) {
		
		
			
		setChanFreqStart(Frequency (x.chanFreqStart));
			
 		
		
		}
		
	

	
		
		chanFreqStepExists = x.chanFreqStepExists;
		if (x.chanFreqStepExists) {
		
		
			
		setChanFreqStep(Frequency (x.chanFreqStep));
			
 		
		
		}
		
	

	
		
		chanFreqArrayExists = x.chanFreqArrayExists;
		if (x.chanFreqArrayExists) {
		
		
			
		chanFreqArray .clear();
		for (unsigned int i = 0; i <x.chanFreqArray.length(); ++i) {
			
			chanFreqArray.push_back(Frequency (x.chanFreqArray[i]));
			
		}
			
  		
		
		}
		
	

	
		
		chanWidthExists = x.chanWidthExists;
		if (x.chanWidthExists) {
		
		
			
		setChanWidth(Frequency (x.chanWidth));
			
 		
		
		}
		
	

	
		
		chanWidthArrayExists = x.chanWidthArrayExists;
		if (x.chanWidthArrayExists) {
		
		
			
		chanWidthArray .clear();
		for (unsigned int i = 0; i <x.chanWidthArray.length(); ++i) {
			
			chanWidthArray.push_back(Frequency (x.chanWidthArray[i]));
			
		}
			
  		
		
		}
		
	

	
		
		correlationBitExists = x.correlationBitExists;
		if (x.correlationBitExists) {
		
		
			
		setCorrelationBit(x.correlationBit);
  			
 		
		
		}
		
	

	
		
		effectiveBwExists = x.effectiveBwExists;
		if (x.effectiveBwExists) {
		
		
			
		setEffectiveBw(Frequency (x.effectiveBw));
			
 		
		
		}
		
	

	
		
		effectiveBwArrayExists = x.effectiveBwArrayExists;
		if (x.effectiveBwArrayExists) {
		
		
			
		effectiveBwArray .clear();
		for (unsigned int i = 0; i <x.effectiveBwArray.length(); ++i) {
			
			effectiveBwArray.push_back(Frequency (x.effectiveBwArray[i]));
			
		}
			
  		
		
		}
		
	

	
		
		freqGroupExists = x.freqGroupExists;
		if (x.freqGroupExists) {
		
		
			
		setFreqGroup(x.freqGroup);
  			
 		
		
		}
		
	

	
		
		freqGroupNameExists = x.freqGroupNameExists;
		if (x.freqGroupNameExists) {
		
		
			
		setFreqGroupName(string (x.freqGroupName));
			
 		
		
		}
		
	

	
		
		lineArrayExists = x.lineArrayExists;
		if (x.lineArrayExists) {
		
		
			
		lineArray .clear();
		for (unsigned int i = 0; i <x.lineArray.length(); ++i) {
			
			lineArray.push_back(x.lineArray[i]);
  			
		}
			
  		
		
		}
		
	

	
		
		measFreqRefExists = x.measFreqRefExists;
		if (x.measFreqRefExists) {
		
		
			
		setMeasFreqRef(x.measFreqRef);
  			
 		
		
		}
		
	

	
		
		nameExists = x.nameExists;
		if (x.nameExists) {
		
		
			
		setName(string (x.name));
			
 		
		
		}
		
	

	
		
		oversamplingExists = x.oversamplingExists;
		if (x.oversamplingExists) {
		
		
			
		setOversampling(x.oversampling);
  			
 		
		
		}
		
	

	
		
		quantizationExists = x.quantizationExists;
		if (x.quantizationExists) {
		
		
			
		setQuantization(x.quantization);
  			
 		
		
		}
		
	

	
		
		refChanExists = x.refChanExists;
		if (x.refChanExists) {
		
		
			
		setRefChan(x.refChan);
  			
 		
		
		}
		
	

	
		
		resolutionExists = x.resolutionExists;
		if (x.resolutionExists) {
		
		
			
		setResolution(Frequency (x.resolution));
			
 		
		
		}
		
	

	
		
		resolutionArrayExists = x.resolutionArrayExists;
		if (x.resolutionArrayExists) {
		
		
			
		resolutionArray .clear();
		for (unsigned int i = 0; i <x.resolutionArray.length(); ++i) {
			
			resolutionArray.push_back(Frequency (x.resolutionArray[i]));
			
		}
			
  		
		
		}
		
	

	
		
		numAssocValuesExists = x.numAssocValuesExists;
		if (x.numAssocValuesExists) {
		
		
			
		setNumAssocValues(x.numAssocValues);
  			
 		
		
		}
		
	

	
		
		assocNatureExists = x.assocNatureExists;
		if (x.assocNatureExists) {
		
		
			
		assocNature .clear();
		for (unsigned int i = 0; i <x.assocNature.length(); ++i) {
			
			assocNature.push_back(x.assocNature[i]);
  			
		}
			
  		
		
		}
		
	

	
	
		
	
		
		assocSpectralWindowIdExists = x.assocSpectralWindowIdExists;
		if (x.assocSpectralWindowIdExists) {
		
		assocSpectralWindowId .clear();
		for (unsigned int i = 0; i <x.assocSpectralWindowId.length(); ++i) {
			
			assocSpectralWindowId.push_back(Tag (x.assocSpectralWindowId[i]));
			
		}
		
		}
		
  	

	
		
		dopplerIdExists = x.dopplerIdExists;
		if (x.dopplerIdExists) {
		
		
			
		setDopplerId(x.dopplerId);
  			
 		
		
		}
		
	

	
		
		imageSpectralWindowIdExists = x.imageSpectralWindowIdExists;
		if (x.imageSpectralWindowIdExists) {
		
		
			
		setImageSpectralWindowId(Tag (x.imageSpectralWindowId));
			
 		
		
		}
		
	

	
		
	

	

	

		} catch (const IllegalAccessException &err) {
			throw ConversionException (err.getMessage(),"SpectralWindow");
		}
	}
#endif
	
	/**
	 * Return this row in the form of an XML string.
	 * @return The values of this row as an XML string.
	 */
	string SpectralWindowRow::toXML() const {
		string buf;
		buf.append("<row> \n");
		
	
		
  	
 		
		
		Parser::toXML(spectralWindowId, "spectralWindowId", buf);
		
		
	

  	
 		
		
			buf.append(EnumerationParser::toXML("basebandName", basebandName));
		
		
	

  	
 		
		
			buf.append(EnumerationParser::toXML("netSideband", netSideband));
		
		
	

  	
 		
		
		Parser::toXML(numChan, "numChan", buf);
		
		
	

  	
 		
		if (numBinExists) {
		
		
		Parser::toXML(numBin, "numBin", buf);
		
		
		}
		
	

  	
 		
		
		Parser::toXML(refFreq, "refFreq", buf);
		
		
	

  	
 		
		
			buf.append(EnumerationParser::toXML("sidebandProcessingMode", sidebandProcessingMode));
		
		
	

  	
 		
		
		Parser::toXML(totBandwidth, "totBandwidth", buf);
		
		
	

  	
 		
		
			buf.append(EnumerationParser::toXML("windowFunction", windowFunction));
		
		
	

  	
 		
		if (chanFreqStartExists) {
		
		
		Parser::toXML(chanFreqStart, "chanFreqStart", buf);
		
		
		}
		
	

  	
 		
		if (chanFreqStepExists) {
		
		
		Parser::toXML(chanFreqStep, "chanFreqStep", buf);
		
		
		}
		
	

  	
 		
		if (chanFreqArrayExists) {
		
		
		Parser::toXML(chanFreqArray, "chanFreqArray", buf);
		
		
		}
		
	

  	
 		
		if (chanWidthExists) {
		
		
		Parser::toXML(chanWidth, "chanWidth", buf);
		
		
		}
		
	

  	
 		
		if (chanWidthArrayExists) {
		
		
		Parser::toXML(chanWidthArray, "chanWidthArray", buf);
		
		
		}
		
	

  	
 		
		if (correlationBitExists) {
		
		
			buf.append(EnumerationParser::toXML("correlationBit", correlationBit));
		
		
		}
		
	

  	
 		
		if (effectiveBwExists) {
		
		
		Parser::toXML(effectiveBw, "effectiveBw", buf);
		
		
		}
		
	

  	
 		
		if (effectiveBwArrayExists) {
		
		
		Parser::toXML(effectiveBwArray, "effectiveBwArray", buf);
		
		
		}
		
	

  	
 		
		if (freqGroupExists) {
		
		
		Parser::toXML(freqGroup, "freqGroup", buf);
		
		
		}
		
	

  	
 		
		if (freqGroupNameExists) {
		
		
		Parser::toXML(freqGroupName, "freqGroupName", buf);
		
		
		}
		
	

  	
 		
		if (lineArrayExists) {
		
		
		Parser::toXML(lineArray, "lineArray", buf);
		
		
		}
		
	

  	
 		
		if (measFreqRefExists) {
		
		
			buf.append(EnumerationParser::toXML("measFreqRef", measFreqRef));
		
		
		}
		
	

  	
 		
		if (nameExists) {
		
		
		Parser::toXML(name, "name", buf);
		
		
		}
		
	

  	
 		
		if (oversamplingExists) {
		
		
		Parser::toXML(oversampling, "oversampling", buf);
		
		
		}
		
	

  	
 		
		if (quantizationExists) {
		
		
		Parser::toXML(quantization, "quantization", buf);
		
		
		}
		
	

  	
 		
		if (refChanExists) {
		
		
		Parser::toXML(refChan, "refChan", buf);
		
		
		}
		
	

  	
 		
		if (resolutionExists) {
		
		
		Parser::toXML(resolution, "resolution", buf);
		
		
		}
		
	

  	
 		
		if (resolutionArrayExists) {
		
		
		Parser::toXML(resolutionArray, "resolutionArray", buf);
		
		
		}
		
	

  	
 		
		if (numAssocValuesExists) {
		
		
		Parser::toXML(numAssocValues, "numAssocValues", buf);
		
		
		}
		
	

  	
 		
		if (assocNatureExists) {
		
		
			buf.append(EnumerationParser::toXML("assocNature", assocNature));
		
		
		}
		
	

	
	
		
  	
 		
		if (assocSpectralWindowIdExists) {
		
		
		Parser::toXML(assocSpectralWindowId, "assocSpectralWindowId", buf);
		
		
		}
		
	

  	
 		
		if (dopplerIdExists) {
		
		
		Parser::toXML(dopplerId, "dopplerId", buf);
		
		
		}
		
	

  	
 		
		if (imageSpectralWindowIdExists) {
		
		
		Parser::toXML(imageSpectralWindowId, "imageSpectralWindowId", buf);
		
		
		}
		
	

	
		
	

	

	

		
		buf.append("</row>\n");
		return buf;
	}

	/**
	 * Fill the values of this row from an XML string 
	 * that was produced by the toXML() method.
	 * @param x The XML string being used to set the values of this row.
	 */
	void SpectralWindowRow::setFromXML (string rowDoc) {
		Parser row(rowDoc);
		string s = "";
		try {
	
		
	
  		
			
	  	setSpectralWindowId(Parser::getTag("spectralWindowId","SpectralWindow",rowDoc));
			
		
	

	
		
		
		
		basebandName = EnumerationParser::getBasebandName("basebandName","SpectralWindow",rowDoc);
		
		
		
	

	
		
		
		
		netSideband = EnumerationParser::getNetSideband("netSideband","SpectralWindow",rowDoc);
		
		
		
	

	
  		
			
	  	setNumChan(Parser::getInteger("numChan","SpectralWindow",rowDoc));
			
		
	

	
  		
        if (row.isStr("<numBin>")) {
			
	  		setNumBin(Parser::getInteger("numBin","SpectralWindow",rowDoc));
			
		}
 		
	

	
  		
			
	  	setRefFreq(Parser::getFrequency("refFreq","SpectralWindow",rowDoc));
			
		
	

	
		
		
		
		sidebandProcessingMode = EnumerationParser::getSidebandProcessingMode("sidebandProcessingMode","SpectralWindow",rowDoc);
		
		
		
	

	
  		
			
	  	setTotBandwidth(Parser::getFrequency("totBandwidth","SpectralWindow",rowDoc));
			
		
	

	
		
		
		
		windowFunction = EnumerationParser::getWindowFunction("windowFunction","SpectralWindow",rowDoc);
		
		
		
	

	
  		
        if (row.isStr("<chanFreqStart>")) {
			
	  		setChanFreqStart(Parser::getFrequency("chanFreqStart","SpectralWindow",rowDoc));
			
		}
 		
	

	
  		
        if (row.isStr("<chanFreqStep>")) {
			
	  		setChanFreqStep(Parser::getFrequency("chanFreqStep","SpectralWindow",rowDoc));
			
		}
 		
	

	
  		
        if (row.isStr("<chanFreqArray>")) {
			
								
	  		setChanFreqArray(Parser::get1DFrequency("chanFreqArray","SpectralWindow",rowDoc));
	  			
	  		
		}
 		
	

	
  		
        if (row.isStr("<chanWidth>")) {
			
	  		setChanWidth(Parser::getFrequency("chanWidth","SpectralWindow",rowDoc));
			
		}
 		
	

	
  		
        if (row.isStr("<chanWidthArray>")) {
			
								
	  		setChanWidthArray(Parser::get1DFrequency("chanWidthArray","SpectralWindow",rowDoc));
	  			
	  		
		}
 		
	

	
		
	if (row.isStr("<correlationBit>")) {
		
		
		
		correlationBit = EnumerationParser::getCorrelationBit("correlationBit","SpectralWindow",rowDoc);
		
		
		
		correlationBitExists = true;
	}
		
	

	
  		
        if (row.isStr("<effectiveBw>")) {
			
	  		setEffectiveBw(Parser::getFrequency("effectiveBw","SpectralWindow",rowDoc));
			
		}
 		
	

	
  		
        if (row.isStr("<effectiveBwArray>")) {
			
								
	  		setEffectiveBwArray(Parser::get1DFrequency("effectiveBwArray","SpectralWindow",rowDoc));
	  			
	  		
		}
 		
	

	
  		
        if (row.isStr("<freqGroup>")) {
			
	  		setFreqGroup(Parser::getInteger("freqGroup","SpectralWindow",rowDoc));
			
		}
 		
	

	
  		
        if (row.isStr("<freqGroupName>")) {
			
	  		setFreqGroupName(Parser::getString("freqGroupName","SpectralWindow",rowDoc));
			
		}
 		
	

	
  		
        if (row.isStr("<lineArray>")) {
			
								
	  		setLineArray(Parser::get1DBoolean("lineArray","SpectralWindow",rowDoc));
	  			
	  		
		}
 		
	

	
		
	if (row.isStr("<measFreqRef>")) {
		
		
		
		measFreqRef = EnumerationParser::getFrequencyReferenceCode("measFreqRef","SpectralWindow",rowDoc);
		
		
		
		measFreqRefExists = true;
	}
		
	

	
  		
        if (row.isStr("<name>")) {
			
	  		setName(Parser::getString("name","SpectralWindow",rowDoc));
			
		}
 		
	

	
  		
        if (row.isStr("<oversampling>")) {
			
	  		setOversampling(Parser::getBoolean("oversampling","SpectralWindow",rowDoc));
			
		}
 		
	

	
  		
        if (row.isStr("<quantization>")) {
			
	  		setQuantization(Parser::getBoolean("quantization","SpectralWindow",rowDoc));
			
		}
 		
	

	
  		
        if (row.isStr("<refChan>")) {
			
	  		setRefChan(Parser::getDouble("refChan","SpectralWindow",rowDoc));
			
		}
 		
	

	
  		
        if (row.isStr("<resolution>")) {
			
	  		setResolution(Parser::getFrequency("resolution","SpectralWindow",rowDoc));
			
		}
 		
	

	
  		
        if (row.isStr("<resolutionArray>")) {
			
								
	  		setResolutionArray(Parser::get1DFrequency("resolutionArray","SpectralWindow",rowDoc));
	  			
	  		
		}
 		
	

	
  		
        if (row.isStr("<numAssocValues>")) {
			
	  		setNumAssocValues(Parser::getInteger("numAssocValues","SpectralWindow",rowDoc));
			
		}
 		
	

	
		
	if (row.isStr("<assocNature>")) {
		
		
		
		assocNature = EnumerationParser::getSpectralResolutionType1D("assocNature","SpectralWindow",rowDoc);			
		
		
		
		assocNatureExists = true;
	}
		
	

	
	
		
	
  		
  		if (row.isStr("<assocSpectralWindowId>")) {
  			setAssocSpectralWindowId(Parser::get1DTag("assocSpectralWindowId","SpectralWindow",rowDoc));  		
  		}
  		
  	

	
  		
        if (row.isStr("<dopplerId>")) {
			
	  		setDopplerId(Parser::getInteger("dopplerId","SpectralWindow",rowDoc));
			
		}
 		
	

	
  		
        if (row.isStr("<imageSpectralWindowId>")) {
			
	  		setImageSpectralWindowId(Parser::getTag("imageSpectralWindowId","SpectralWindow",rowDoc));
			
		}
 		
	

	
		
	

	

	

		} catch (const IllegalAccessException &err) {
			throw ConversionException (err.getMessage(),"SpectralWindow");
		}
	}
	
	void SpectralWindowRow::toBin(EndianOSStream& eoss) {
	
	
	
	
		
	spectralWindowId.toBin(eoss);
		
	

	
	
		
					
		eoss.writeString(CBasebandName::name(basebandName));
			/* eoss.writeInt(basebandName); */
				
		
	

	
	
		
					
		eoss.writeString(CNetSideband::name(netSideband));
			/* eoss.writeInt(netSideband); */
				
		
	

	
	
		
						
			eoss.writeInt(numChan);
				
		
	

	
	
		
	refFreq.toBin(eoss);
		
	

	
	
		
					
		eoss.writeString(CSidebandProcessingMode::name(sidebandProcessingMode));
			/* eoss.writeInt(sidebandProcessingMode); */
				
		
	

	
	
		
	totBandwidth.toBin(eoss);
		
	

	
	
		
					
		eoss.writeString(CWindowFunction::name(windowFunction));
			/* eoss.writeInt(windowFunction); */
				
		
	


	
	
	eoss.writeBoolean(numBinExists);
	if (numBinExists) {
	
	
	
		
						
			eoss.writeInt(numBin);
				
		
	

	}

	eoss.writeBoolean(chanFreqStartExists);
	if (chanFreqStartExists) {
	
	
	
		
	chanFreqStart.toBin(eoss);
		
	

	}

	eoss.writeBoolean(chanFreqStepExists);
	if (chanFreqStepExists) {
	
	
	
		
	chanFreqStep.toBin(eoss);
		
	

	}

	eoss.writeBoolean(chanFreqArrayExists);
	if (chanFreqArrayExists) {
	
	
	
		
	Frequency::toBin(chanFreqArray, eoss);
		
	

	}

	eoss.writeBoolean(chanWidthExists);
	if (chanWidthExists) {
	
	
	
		
	chanWidth.toBin(eoss);
		
	

	}

	eoss.writeBoolean(chanWidthArrayExists);
	if (chanWidthArrayExists) {
	
	
	
		
	Frequency::toBin(chanWidthArray, eoss);
		
	

	}

	eoss.writeBoolean(correlationBitExists);
	if (correlationBitExists) {
	
	
	
		
					
		eoss.writeString(CCorrelationBit::name(correlationBit));
			/* eoss.writeInt(correlationBit); */
				
		
	

	}

	eoss.writeBoolean(effectiveBwExists);
	if (effectiveBwExists) {
	
	
	
		
	effectiveBw.toBin(eoss);
		
	

	}

	eoss.writeBoolean(effectiveBwArrayExists);
	if (effectiveBwArrayExists) {
	
	
	
		
	Frequency::toBin(effectiveBwArray, eoss);
		
	

	}

	eoss.writeBoolean(freqGroupExists);
	if (freqGroupExists) {
	
	
	
		
						
			eoss.writeInt(freqGroup);
				
		
	

	}

	eoss.writeBoolean(freqGroupNameExists);
	if (freqGroupNameExists) {
	
	
	
		
						
			eoss.writeString(freqGroupName);
				
		
	

	}

	eoss.writeBoolean(lineArrayExists);
	if (lineArrayExists) {
	
	
	
		
		
			
		eoss.writeInt((int) lineArray.size());
		for (unsigned int i = 0; i < lineArray.size(); i++)
				
			eoss.writeBoolean(lineArray.at(i));
				
				
						
		
	

	}

	eoss.writeBoolean(measFreqRefExists);
	if (measFreqRefExists) {
	
	
	
		
					
		eoss.writeString(CFrequencyReferenceCode::name(measFreqRef));
			/* eoss.writeInt(measFreqRef); */
				
		
	

	}

	eoss.writeBoolean(nameExists);
	if (nameExists) {
	
	
	
		
						
			eoss.writeString(name);
				
		
	

	}

	eoss.writeBoolean(oversamplingExists);
	if (oversamplingExists) {
	
	
	
		
						
			eoss.writeBoolean(oversampling);
				
		
	

	}

	eoss.writeBoolean(quantizationExists);
	if (quantizationExists) {
	
	
	
		
						
			eoss.writeBoolean(quantization);
				
		
	

	}

	eoss.writeBoolean(refChanExists);
	if (refChanExists) {
	
	
	
		
						
			eoss.writeDouble(refChan);
				
		
	

	}

	eoss.writeBoolean(resolutionExists);
	if (resolutionExists) {
	
	
	
		
	resolution.toBin(eoss);
		
	

	}

	eoss.writeBoolean(resolutionArrayExists);
	if (resolutionArrayExists) {
	
	
	
		
	Frequency::toBin(resolutionArray, eoss);
		
	

	}

	eoss.writeBoolean(numAssocValuesExists);
	if (numAssocValuesExists) {
	
	
	
		
						
			eoss.writeInt(numAssocValues);
				
		
	

	}

	eoss.writeBoolean(assocNatureExists);
	if (assocNatureExists) {
	
	
	
		
		
			
		eoss.writeInt((int) assocNature.size());
		for (unsigned int i = 0; i < assocNature.size(); i++)
				
			eoss.writeString(CSpectralResolutionType::name(assocNature.at(i)));
			/* eoss.writeInt(assocNature.at(i)); */
				
				
						
		
	

	}

	eoss.writeBoolean(assocSpectralWindowIdExists);
	if (assocSpectralWindowIdExists) {
	
	
	
		
	Tag::toBin(assocSpectralWindowId, eoss);
		
	

	}

	eoss.writeBoolean(imageSpectralWindowIdExists);
	if (imageSpectralWindowIdExists) {
	
	
	
		
	imageSpectralWindowId.toBin(eoss);
		
	

	}

	eoss.writeBoolean(dopplerIdExists);
	if (dopplerIdExists) {
	
	
	
		
						
			eoss.writeInt(dopplerId);
				
		
	

	}

	}
	
void SpectralWindowRow::spectralWindowIdFromBin(EndianIStream& eis) {
		
	
		
		
		spectralWindowId =  Tag::fromBin(eis);
		
	
	
}
void SpectralWindowRow::basebandNameFromBin(EndianIStream& eis) {
		
	
	
		
			
		basebandName = CBasebandName::literal(eis.readString());
			
		
	
	
}
void SpectralWindowRow::netSidebandFromBin(EndianIStream& eis) {
		
	
	
		
			
		netSideband = CNetSideband::literal(eis.readString());
			
		
	
	
}
void SpectralWindowRow::numChanFromBin(EndianIStream& eis) {
		
	
	
		
			
		numChan =  eis.readInt();
			
		
	
	
}
void SpectralWindowRow::refFreqFromBin(EndianIStream& eis) {
		
	
		
		
		refFreq =  Frequency::fromBin(eis);
		
	
	
}
void SpectralWindowRow::sidebandProcessingModeFromBin(EndianIStream& eis) {
		
	
	
		
			
		sidebandProcessingMode = CSidebandProcessingMode::literal(eis.readString());
			
		
	
	
}
void SpectralWindowRow::totBandwidthFromBin(EndianIStream& eis) {
		
	
		
		
		totBandwidth =  Frequency::fromBin(eis);
		
	
	
}
void SpectralWindowRow::windowFunctionFromBin(EndianIStream& eis) {
		
	
	
		
			
		windowFunction = CWindowFunction::literal(eis.readString());
			
		
	
	
}

void SpectralWindowRow::numBinFromBin(EndianIStream& eis) {
		
	numBinExists = eis.readBoolean();
	if (numBinExists) {
		
	
	
		
			
		numBin =  eis.readInt();
			
		
	

	}
	
}
void SpectralWindowRow::chanFreqStartFromBin(EndianIStream& eis) {
		
	chanFreqStartExists = eis.readBoolean();
	if (chanFreqStartExists) {
		
	
		
		
		chanFreqStart =  Frequency::fromBin(eis);
		
	

	}
	
}
void SpectralWindowRow::chanFreqStepFromBin(EndianIStream& eis) {
		
	chanFreqStepExists = eis.readBoolean();
	if (chanFreqStepExists) {
		
	
		
		
		chanFreqStep =  Frequency::fromBin(eis);
		
	

	}
	
}
void SpectralWindowRow::chanFreqArrayFromBin(EndianIStream& eis) {
		
	chanFreqArrayExists = eis.readBoolean();
	if (chanFreqArrayExists) {
		
	
		
		
			
	
	chanFreqArray = Frequency::from1DBin(eis);	
	

		
	

	}
	
}
void SpectralWindowRow::chanWidthFromBin(EndianIStream& eis) {
		
	chanWidthExists = eis.readBoolean();
	if (chanWidthExists) {
		
	
		
		
		chanWidth =  Frequency::fromBin(eis);
		
	

	}
	
}
void SpectralWindowRow::chanWidthArrayFromBin(EndianIStream& eis) {
		
	chanWidthArrayExists = eis.readBoolean();
	if (chanWidthArrayExists) {
		
	
		
		
			
	
	chanWidthArray = Frequency::from1DBin(eis);	
	

		
	

	}
	
}
void SpectralWindowRow::correlationBitFromBin(EndianIStream& eis) {
		
	correlationBitExists = eis.readBoolean();
	if (correlationBitExists) {
		
	
	
		
			
		correlationBit = CCorrelationBit::literal(eis.readString());
			
		
	

	}
	
}
void SpectralWindowRow::effectiveBwFromBin(EndianIStream& eis) {
		
	effectiveBwExists = eis.readBoolean();
	if (effectiveBwExists) {
		
	
		
		
		effectiveBw =  Frequency::fromBin(eis);
		
	

	}
	
}
void SpectralWindowRow::effectiveBwArrayFromBin(EndianIStream& eis) {
		
	effectiveBwArrayExists = eis.readBoolean();
	if (effectiveBwArrayExists) {
		
	
		
		
			
	
	effectiveBwArray = Frequency::from1DBin(eis);	
	

		
	

	}
	
}
void SpectralWindowRow::freqGroupFromBin(EndianIStream& eis) {
		
	freqGroupExists = eis.readBoolean();
	if (freqGroupExists) {
		
	
	
		
			
		freqGroup =  eis.readInt();
			
		
	

	}
	
}
void SpectralWindowRow::freqGroupNameFromBin(EndianIStream& eis) {
		
	freqGroupNameExists = eis.readBoolean();
	if (freqGroupNameExists) {
		
	
	
		
			
		freqGroupName =  eis.readString();
			
		
	

	}
	
}
void SpectralWindowRow::lineArrayFromBin(EndianIStream& eis) {
		
	lineArrayExists = eis.readBoolean();
	if (lineArrayExists) {
		
	
	
		
			
	
		lineArray.clear();
		
		unsigned int lineArrayDim1 = eis.readInt();
		for (unsigned int  i = 0 ; i < lineArrayDim1; i++)
			
			lineArray.push_back(eis.readBoolean());
			
	

		
	

	}
	
}
void SpectralWindowRow::measFreqRefFromBin(EndianIStream& eis) {
		
	measFreqRefExists = eis.readBoolean();
	if (measFreqRefExists) {
		
	
	
		
			
		measFreqRef = CFrequencyReferenceCode::literal(eis.readString());
			
		
	

	}
	
}
void SpectralWindowRow::nameFromBin(EndianIStream& eis) {
		
	nameExists = eis.readBoolean();
	if (nameExists) {
		
	
	
		
			
		name =  eis.readString();
			
		
	

	}
	
}
void SpectralWindowRow::oversamplingFromBin(EndianIStream& eis) {
		
	oversamplingExists = eis.readBoolean();
	if (oversamplingExists) {
		
	
	
		
			
		oversampling =  eis.readBoolean();
			
		
	

	}
	
}
void SpectralWindowRow::quantizationFromBin(EndianIStream& eis) {
		
	quantizationExists = eis.readBoolean();
	if (quantizationExists) {
		
	
	
		
			
		quantization =  eis.readBoolean();
			
		
	

	}
	
}
void SpectralWindowRow::refChanFromBin(EndianIStream& eis) {
		
	refChanExists = eis.readBoolean();
	if (refChanExists) {
		
	
	
		
			
		refChan =  eis.readDouble();
			
		
	

	}
	
}
void SpectralWindowRow::resolutionFromBin(EndianIStream& eis) {
		
	resolutionExists = eis.readBoolean();
	if (resolutionExists) {
		
	
		
		
		resolution =  Frequency::fromBin(eis);
		
	

	}
	
}
void SpectralWindowRow::resolutionArrayFromBin(EndianIStream& eis) {
		
	resolutionArrayExists = eis.readBoolean();
	if (resolutionArrayExists) {
		
	
		
		
			
	
	resolutionArray = Frequency::from1DBin(eis);	
	

		
	

	}
	
}
void SpectralWindowRow::numAssocValuesFromBin(EndianIStream& eis) {
		
	numAssocValuesExists = eis.readBoolean();
	if (numAssocValuesExists) {
		
	
	
		
			
		numAssocValues =  eis.readInt();
			
		
	

	}
	
}
void SpectralWindowRow::assocNatureFromBin(EndianIStream& eis) {
		
	assocNatureExists = eis.readBoolean();
	if (assocNatureExists) {
		
	
	
		
			
	
		assocNature.clear();
		
		unsigned int assocNatureDim1 = eis.readInt();
		for (unsigned int  i = 0 ; i < assocNatureDim1; i++)
			
			assocNature.push_back(CSpectralResolutionType::literal(eis.readString()));
			
	

		
	

	}
	
}
void SpectralWindowRow::assocSpectralWindowIdFromBin(EndianIStream& eis) {
		
	assocSpectralWindowIdExists = eis.readBoolean();
	if (assocSpectralWindowIdExists) {
		
	
		
		
			
	
	assocSpectralWindowId = Tag::from1DBin(eis);	
	

		
	

	}
	
}
void SpectralWindowRow::imageSpectralWindowIdFromBin(EndianIStream& eis) {
		
	imageSpectralWindowIdExists = eis.readBoolean();
	if (imageSpectralWindowIdExists) {
		
	
		
		
		imageSpectralWindowId =  Tag::fromBin(eis);
		
	

	}
	
}
void SpectralWindowRow::dopplerIdFromBin(EndianIStream& eis) {
		
	dopplerIdExists = eis.readBoolean();
	if (dopplerIdExists) {
		
	
	
		
			
		dopplerId =  eis.readInt();
			
		
	

	}
	
}
	
	
	SpectralWindowRow* SpectralWindowRow::fromBin(EndianIStream& eis, SpectralWindowTable& table, const vector<string>& attributesSeq) {
		SpectralWindowRow* row = new  SpectralWindowRow(table);
		
		map<string, SpectralWindowAttributeFromBin>::iterator iter ;
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
					throw ConversionException("There is not method to read an attribute '"+attributesSeq.at(i)+"'.", "SpectralWindowTable");
			}
				
		}				
		return row;
	}

	//
	// A collection of methods to set the value of the attributes from their textual value in the XML representation
	// of one row.
	//
	
	// Convert a string into an Tag 
	void SpectralWindowRow::spectralWindowIdFromText(const string & s) {
		 
          
		spectralWindowId = ASDMValuesParser::parse<Tag>(s);
          
		
	}
	
	
	// Convert a string into an BasebandName 
	void SpectralWindowRow::basebandNameFromText(const string & s) {
		 
          
		basebandName = ASDMValuesParser::parse<BasebandNameMod::BasebandName>(s);
          
		
	}
	
	
	// Convert a string into an NetSideband 
	void SpectralWindowRow::netSidebandFromText(const string & s) {
		 
          
		netSideband = ASDMValuesParser::parse<NetSidebandMod::NetSideband>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void SpectralWindowRow::numChanFromText(const string & s) {
		 
          
		numChan = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	// Convert a string into an Frequency 
	void SpectralWindowRow::refFreqFromText(const string & s) {
		 
          
		refFreq = ASDMValuesParser::parse<Frequency>(s);
          
		
	}
	
	
	// Convert a string into an SidebandProcessingMode 
	void SpectralWindowRow::sidebandProcessingModeFromText(const string & s) {
		 
          
		sidebandProcessingMode = ASDMValuesParser::parse<SidebandProcessingModeMod::SidebandProcessingMode>(s);
          
		
	}
	
	
	// Convert a string into an Frequency 
	void SpectralWindowRow::totBandwidthFromText(const string & s) {
		 
          
		totBandwidth = ASDMValuesParser::parse<Frequency>(s);
          
		
	}
	
	
	// Convert a string into an WindowFunction 
	void SpectralWindowRow::windowFunctionFromText(const string & s) {
		 
          
		windowFunction = ASDMValuesParser::parse<WindowFunctionMod::WindowFunction>(s);
          
		
	}
	

	
	// Convert a string into an int 
	void SpectralWindowRow::numBinFromText(const string & s) {
		numBinExists = true;
		 
          
		numBin = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	// Convert a string into an Frequency 
	void SpectralWindowRow::chanFreqStartFromText(const string & s) {
		chanFreqStartExists = true;
		 
          
		chanFreqStart = ASDMValuesParser::parse<Frequency>(s);
          
		
	}
	
	
	// Convert a string into an Frequency 
	void SpectralWindowRow::chanFreqStepFromText(const string & s) {
		chanFreqStepExists = true;
		 
          
		chanFreqStep = ASDMValuesParser::parse<Frequency>(s);
          
		
	}
	
	
	// Convert a string into an Frequency 
	void SpectralWindowRow::chanFreqArrayFromText(const string & s) {
		chanFreqArrayExists = true;
		 
          
		chanFreqArray = ASDMValuesParser::parse1D<Frequency>(s);
          
		
	}
	
	
	// Convert a string into an Frequency 
	void SpectralWindowRow::chanWidthFromText(const string & s) {
		chanWidthExists = true;
		 
          
		chanWidth = ASDMValuesParser::parse<Frequency>(s);
          
		
	}
	
	
	// Convert a string into an Frequency 
	void SpectralWindowRow::chanWidthArrayFromText(const string & s) {
		chanWidthArrayExists = true;
		 
          
		chanWidthArray = ASDMValuesParser::parse1D<Frequency>(s);
          
		
	}
	
	
	// Convert a string into an CorrelationBit 
	void SpectralWindowRow::correlationBitFromText(const string & s) {
		correlationBitExists = true;
		 
          
		correlationBit = ASDMValuesParser::parse<CorrelationBitMod::CorrelationBit>(s);
          
		
	}
	
	
	// Convert a string into an Frequency 
	void SpectralWindowRow::effectiveBwFromText(const string & s) {
		effectiveBwExists = true;
		 
          
		effectiveBw = ASDMValuesParser::parse<Frequency>(s);
          
		
	}
	
	
	// Convert a string into an Frequency 
	void SpectralWindowRow::effectiveBwArrayFromText(const string & s) {
		effectiveBwArrayExists = true;
		 
          
		effectiveBwArray = ASDMValuesParser::parse1D<Frequency>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void SpectralWindowRow::freqGroupFromText(const string & s) {
		freqGroupExists = true;
		 
          
		freqGroup = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	// Convert a string into an String 
	void SpectralWindowRow::freqGroupNameFromText(const string & s) {
		freqGroupNameExists = true;
		 
          
		freqGroupName = ASDMValuesParser::parse<string>(s);
          
		
	}
	
	
	// Convert a string into an boolean 
	void SpectralWindowRow::lineArrayFromText(const string & s) {
		lineArrayExists = true;
		 
          
		lineArray = ASDMValuesParser::parse1D<bool>(s);
          
		
	}
	
	
	// Convert a string into an FrequencyReferenceCode 
	void SpectralWindowRow::measFreqRefFromText(const string & s) {
		measFreqRefExists = true;
		 
          
		measFreqRef = ASDMValuesParser::parse<FrequencyReferenceCodeMod::FrequencyReferenceCode>(s);
          
		
	}
	
	
	// Convert a string into an String 
	void SpectralWindowRow::nameFromText(const string & s) {
		nameExists = true;
		 
          
		name = ASDMValuesParser::parse<string>(s);
          
		
	}
	
	
	// Convert a string into an boolean 
	void SpectralWindowRow::oversamplingFromText(const string & s) {
		oversamplingExists = true;
		 
          
		oversampling = ASDMValuesParser::parse<bool>(s);
          
		
	}
	
	
	// Convert a string into an boolean 
	void SpectralWindowRow::quantizationFromText(const string & s) {
		quantizationExists = true;
		 
          
		quantization = ASDMValuesParser::parse<bool>(s);
          
		
	}
	
	
	// Convert a string into an double 
	void SpectralWindowRow::refChanFromText(const string & s) {
		refChanExists = true;
		 
          
		refChan = ASDMValuesParser::parse<double>(s);
          
		
	}
	
	
	// Convert a string into an Frequency 
	void SpectralWindowRow::resolutionFromText(const string & s) {
		resolutionExists = true;
		 
          
		resolution = ASDMValuesParser::parse<Frequency>(s);
          
		
	}
	
	
	// Convert a string into an Frequency 
	void SpectralWindowRow::resolutionArrayFromText(const string & s) {
		resolutionArrayExists = true;
		 
          
		resolutionArray = ASDMValuesParser::parse1D<Frequency>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void SpectralWindowRow::numAssocValuesFromText(const string & s) {
		numAssocValuesExists = true;
		 
          
		numAssocValues = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	// Convert a string into an SpectralResolutionType 
	void SpectralWindowRow::assocNatureFromText(const string & s) {
		assocNatureExists = true;
		 
          
		assocNature = ASDMValuesParser::parse1D<SpectralResolutionTypeMod::SpectralResolutionType>(s);
          
		
	}
	
	
	// Convert a string into an Tag 
	void SpectralWindowRow::assocSpectralWindowIdFromText(const string & s) {
		assocSpectralWindowIdExists = true;
		 
          
		assocSpectralWindowId = ASDMValuesParser::parse1D<Tag>(s);
          
		
	}
	
	
	// Convert a string into an Tag 
	void SpectralWindowRow::imageSpectralWindowIdFromText(const string & s) {
		imageSpectralWindowIdExists = true;
		 
          
		imageSpectralWindowId = ASDMValuesParser::parse<Tag>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void SpectralWindowRow::dopplerIdFromText(const string & s) {
		dopplerIdExists = true;
		 
          
		dopplerId = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	
	void SpectralWindowRow::fromText(const std::string& attributeName, const std::string&  t) {
		map<string, SpectralWindowAttributeFromText>::iterator iter;
		if ((iter = fromTextMethods.find(attributeName)) == fromTextMethods.end())
			throw ConversionException("I do not know what to do with '"+attributeName+"' and its content '"+t+"' (while parsing an XML document)", "SpectralWindowTable");
		(this->*(iter->second))(t);
	}
			
	////////////////////////////////////////////////
	// Intrinsic Table Attributes getters/setters //
	////////////////////////////////////////////////
	
	

	
 	/**
 	 * Get spectralWindowId.
 	 * @return spectralWindowId as Tag
 	 */
 	Tag SpectralWindowRow::getSpectralWindowId() const {
	
  		return spectralWindowId;
 	}

 	/**
 	 * Set spectralWindowId with the specified Tag.
 	 * @param spectralWindowId The Tag value to which spectralWindowId is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void SpectralWindowRow::setSpectralWindowId (Tag spectralWindowId)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("spectralWindowId", "SpectralWindow");
		
  		}
  	
 		this->spectralWindowId = spectralWindowId;
	
 	}
	
	

	

	
 	/**
 	 * Get basebandName.
 	 * @return basebandName as BasebandNameMod::BasebandName
 	 */
 	BasebandNameMod::BasebandName SpectralWindowRow::getBasebandName() const {
	
  		return basebandName;
 	}

 	/**
 	 * Set basebandName with the specified BasebandNameMod::BasebandName.
 	 * @param basebandName The BasebandNameMod::BasebandName value to which basebandName is to be set.
 	 
 	
 		
 	 */
 	void SpectralWindowRow::setBasebandName (BasebandNameMod::BasebandName basebandName)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->basebandName = basebandName;
	
 	}
	
	

	

	
 	/**
 	 * Get netSideband.
 	 * @return netSideband as NetSidebandMod::NetSideband
 	 */
 	NetSidebandMod::NetSideband SpectralWindowRow::getNetSideband() const {
	
  		return netSideband;
 	}

 	/**
 	 * Set netSideband with the specified NetSidebandMod::NetSideband.
 	 * @param netSideband The NetSidebandMod::NetSideband value to which netSideband is to be set.
 	 
 	
 		
 	 */
 	void SpectralWindowRow::setNetSideband (NetSidebandMod::NetSideband netSideband)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->netSideband = netSideband;
	
 	}
	
	

	

	
 	/**
 	 * Get numChan.
 	 * @return numChan as int
 	 */
 	int SpectralWindowRow::getNumChan() const {
	
  		return numChan;
 	}

 	/**
 	 * Set numChan with the specified int.
 	 * @param numChan The int value to which numChan is to be set.
 	 
 	
 		
 	 */
 	void SpectralWindowRow::setNumChan (int numChan)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->numChan = numChan;
	
 	}
	
	

	
	/**
	 * The attribute numBin is optional. Return true if this attribute exists.
	 * @return true if and only if the numBin attribute exists. 
	 */
	bool SpectralWindowRow::isNumBinExists() const {
		return numBinExists;
	}
	

	
 	/**
 	 * Get numBin, which is optional.
 	 * @return numBin as int
 	 * @throw IllegalAccessException If numBin does not exist.
 	 */
 	int SpectralWindowRow::getNumBin() const  {
		if (!numBinExists) {
			throw IllegalAccessException("numBin", "SpectralWindow");
		}
	
  		return numBin;
 	}

 	/**
 	 * Set numBin with the specified int.
 	 * @param numBin The int value to which numBin is to be set.
 	 
 	
 	 */
 	void SpectralWindowRow::setNumBin (int numBin) {
	
 		this->numBin = numBin;
	
		numBinExists = true;
	
 	}
	
	
	/**
	 * Mark numBin, which is an optional field, as non-existent.
	 */
	void SpectralWindowRow::clearNumBin () {
		numBinExists = false;
	}
	

	

	
 	/**
 	 * Get refFreq.
 	 * @return refFreq as Frequency
 	 */
 	Frequency SpectralWindowRow::getRefFreq() const {
	
  		return refFreq;
 	}

 	/**
 	 * Set refFreq with the specified Frequency.
 	 * @param refFreq The Frequency value to which refFreq is to be set.
 	 
 	
 		
 	 */
 	void SpectralWindowRow::setRefFreq (Frequency refFreq)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->refFreq = refFreq;
	
 	}
	
	

	

	
 	/**
 	 * Get sidebandProcessingMode.
 	 * @return sidebandProcessingMode as SidebandProcessingModeMod::SidebandProcessingMode
 	 */
 	SidebandProcessingModeMod::SidebandProcessingMode SpectralWindowRow::getSidebandProcessingMode() const {
	
  		return sidebandProcessingMode;
 	}

 	/**
 	 * Set sidebandProcessingMode with the specified SidebandProcessingModeMod::SidebandProcessingMode.
 	 * @param sidebandProcessingMode The SidebandProcessingModeMod::SidebandProcessingMode value to which sidebandProcessingMode is to be set.
 	 
 	
 		
 	 */
 	void SpectralWindowRow::setSidebandProcessingMode (SidebandProcessingModeMod::SidebandProcessingMode sidebandProcessingMode)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->sidebandProcessingMode = sidebandProcessingMode;
	
 	}
	
	

	

	
 	/**
 	 * Get totBandwidth.
 	 * @return totBandwidth as Frequency
 	 */
 	Frequency SpectralWindowRow::getTotBandwidth() const {
	
  		return totBandwidth;
 	}

 	/**
 	 * Set totBandwidth with the specified Frequency.
 	 * @param totBandwidth The Frequency value to which totBandwidth is to be set.
 	 
 	
 		
 	 */
 	void SpectralWindowRow::setTotBandwidth (Frequency totBandwidth)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->totBandwidth = totBandwidth;
	
 	}
	
	

	

	
 	/**
 	 * Get windowFunction.
 	 * @return windowFunction as WindowFunctionMod::WindowFunction
 	 */
 	WindowFunctionMod::WindowFunction SpectralWindowRow::getWindowFunction() const {
	
  		return windowFunction;
 	}

 	/**
 	 * Set windowFunction with the specified WindowFunctionMod::WindowFunction.
 	 * @param windowFunction The WindowFunctionMod::WindowFunction value to which windowFunction is to be set.
 	 
 	
 		
 	 */
 	void SpectralWindowRow::setWindowFunction (WindowFunctionMod::WindowFunction windowFunction)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->windowFunction = windowFunction;
	
 	}
	
	

	
	/**
	 * The attribute chanFreqStart is optional. Return true if this attribute exists.
	 * @return true if and only if the chanFreqStart attribute exists. 
	 */
	bool SpectralWindowRow::isChanFreqStartExists() const {
		return chanFreqStartExists;
	}
	

	
 	/**
 	 * Get chanFreqStart, which is optional.
 	 * @return chanFreqStart as Frequency
 	 * @throw IllegalAccessException If chanFreqStart does not exist.
 	 */
 	Frequency SpectralWindowRow::getChanFreqStart() const  {
		if (!chanFreqStartExists) {
			throw IllegalAccessException("chanFreqStart", "SpectralWindow");
		}
	
  		return chanFreqStart;
 	}

 	/**
 	 * Set chanFreqStart with the specified Frequency.
 	 * @param chanFreqStart The Frequency value to which chanFreqStart is to be set.
 	 
 	
 	 */
 	void SpectralWindowRow::setChanFreqStart (Frequency chanFreqStart) {
	
 		this->chanFreqStart = chanFreqStart;
	
		chanFreqStartExists = true;
	
 	}
	
	
	/**
	 * Mark chanFreqStart, which is an optional field, as non-existent.
	 */
	void SpectralWindowRow::clearChanFreqStart () {
		chanFreqStartExists = false;
	}
	

	
	/**
	 * The attribute chanFreqStep is optional. Return true if this attribute exists.
	 * @return true if and only if the chanFreqStep attribute exists. 
	 */
	bool SpectralWindowRow::isChanFreqStepExists() const {
		return chanFreqStepExists;
	}
	

	
 	/**
 	 * Get chanFreqStep, which is optional.
 	 * @return chanFreqStep as Frequency
 	 * @throw IllegalAccessException If chanFreqStep does not exist.
 	 */
 	Frequency SpectralWindowRow::getChanFreqStep() const  {
		if (!chanFreqStepExists) {
			throw IllegalAccessException("chanFreqStep", "SpectralWindow");
		}
	
  		return chanFreqStep;
 	}

 	/**
 	 * Set chanFreqStep with the specified Frequency.
 	 * @param chanFreqStep The Frequency value to which chanFreqStep is to be set.
 	 
 	
 	 */
 	void SpectralWindowRow::setChanFreqStep (Frequency chanFreqStep) {
	
 		this->chanFreqStep = chanFreqStep;
	
		chanFreqStepExists = true;
	
 	}
	
	
	/**
	 * Mark chanFreqStep, which is an optional field, as non-existent.
	 */
	void SpectralWindowRow::clearChanFreqStep () {
		chanFreqStepExists = false;
	}
	

	
	/**
	 * The attribute chanFreqArray is optional. Return true if this attribute exists.
	 * @return true if and only if the chanFreqArray attribute exists. 
	 */
	bool SpectralWindowRow::isChanFreqArrayExists() const {
		return chanFreqArrayExists;
	}
	

	
 	/**
 	 * Get chanFreqArray, which is optional.
 	 * @return chanFreqArray as std::vector<Frequency >
 	 * @throw IllegalAccessException If chanFreqArray does not exist.
 	 */
 	std::vector<Frequency > SpectralWindowRow::getChanFreqArray() const  {
		if (!chanFreqArrayExists) {
			throw IllegalAccessException("chanFreqArray", "SpectralWindow");
		}
	
  		return chanFreqArray;
 	}

 	/**
 	 * Set chanFreqArray with the specified std::vector<Frequency >.
 	 * @param chanFreqArray The std::vector<Frequency > value to which chanFreqArray is to be set.
 	 
 	
 	 */
 	void SpectralWindowRow::setChanFreqArray (std::vector<Frequency > chanFreqArray) {
	
 		this->chanFreqArray = chanFreqArray;
	
		chanFreqArrayExists = true;
	
 	}
	
	
	/**
	 * Mark chanFreqArray, which is an optional field, as non-existent.
	 */
	void SpectralWindowRow::clearChanFreqArray () {
		chanFreqArrayExists = false;
	}
	

	
	/**
	 * The attribute chanWidth is optional. Return true if this attribute exists.
	 * @return true if and only if the chanWidth attribute exists. 
	 */
	bool SpectralWindowRow::isChanWidthExists() const {
		return chanWidthExists;
	}
	

	
 	/**
 	 * Get chanWidth, which is optional.
 	 * @return chanWidth as Frequency
 	 * @throw IllegalAccessException If chanWidth does not exist.
 	 */
 	Frequency SpectralWindowRow::getChanWidth() const  {
		if (!chanWidthExists) {
			throw IllegalAccessException("chanWidth", "SpectralWindow");
		}
	
  		return chanWidth;
 	}

 	/**
 	 * Set chanWidth with the specified Frequency.
 	 * @param chanWidth The Frequency value to which chanWidth is to be set.
 	 
 	
 	 */
 	void SpectralWindowRow::setChanWidth (Frequency chanWidth) {
	
 		this->chanWidth = chanWidth;
	
		chanWidthExists = true;
	
 	}
	
	
	/**
	 * Mark chanWidth, which is an optional field, as non-existent.
	 */
	void SpectralWindowRow::clearChanWidth () {
		chanWidthExists = false;
	}
	

	
	/**
	 * The attribute chanWidthArray is optional. Return true if this attribute exists.
	 * @return true if and only if the chanWidthArray attribute exists. 
	 */
	bool SpectralWindowRow::isChanWidthArrayExists() const {
		return chanWidthArrayExists;
	}
	

	
 	/**
 	 * Get chanWidthArray, which is optional.
 	 * @return chanWidthArray as std::vector<Frequency >
 	 * @throw IllegalAccessException If chanWidthArray does not exist.
 	 */
 	std::vector<Frequency > SpectralWindowRow::getChanWidthArray() const  {
		if (!chanWidthArrayExists) {
			throw IllegalAccessException("chanWidthArray", "SpectralWindow");
		}
	
  		return chanWidthArray;
 	}

 	/**
 	 * Set chanWidthArray with the specified std::vector<Frequency >.
 	 * @param chanWidthArray The std::vector<Frequency > value to which chanWidthArray is to be set.
 	 
 	
 	 */
 	void SpectralWindowRow::setChanWidthArray (std::vector<Frequency > chanWidthArray) {
	
 		this->chanWidthArray = chanWidthArray;
	
		chanWidthArrayExists = true;
	
 	}
	
	
	/**
	 * Mark chanWidthArray, which is an optional field, as non-existent.
	 */
	void SpectralWindowRow::clearChanWidthArray () {
		chanWidthArrayExists = false;
	}
	

	
	/**
	 * The attribute correlationBit is optional. Return true if this attribute exists.
	 * @return true if and only if the correlationBit attribute exists. 
	 */
	bool SpectralWindowRow::isCorrelationBitExists() const {
		return correlationBitExists;
	}
	

	
 	/**
 	 * Get correlationBit, which is optional.
 	 * @return correlationBit as CorrelationBitMod::CorrelationBit
 	 * @throw IllegalAccessException If correlationBit does not exist.
 	 */
 	CorrelationBitMod::CorrelationBit SpectralWindowRow::getCorrelationBit() const  {
		if (!correlationBitExists) {
			throw IllegalAccessException("correlationBit", "SpectralWindow");
		}
	
  		return correlationBit;
 	}

 	/**
 	 * Set correlationBit with the specified CorrelationBitMod::CorrelationBit.
 	 * @param correlationBit The CorrelationBitMod::CorrelationBit value to which correlationBit is to be set.
 	 
 	
 	 */
 	void SpectralWindowRow::setCorrelationBit (CorrelationBitMod::CorrelationBit correlationBit) {
	
 		this->correlationBit = correlationBit;
	
		correlationBitExists = true;
	
 	}
	
	
	/**
	 * Mark correlationBit, which is an optional field, as non-existent.
	 */
	void SpectralWindowRow::clearCorrelationBit () {
		correlationBitExists = false;
	}
	

	
	/**
	 * The attribute effectiveBw is optional. Return true if this attribute exists.
	 * @return true if and only if the effectiveBw attribute exists. 
	 */
	bool SpectralWindowRow::isEffectiveBwExists() const {
		return effectiveBwExists;
	}
	

	
 	/**
 	 * Get effectiveBw, which is optional.
 	 * @return effectiveBw as Frequency
 	 * @throw IllegalAccessException If effectiveBw does not exist.
 	 */
 	Frequency SpectralWindowRow::getEffectiveBw() const  {
		if (!effectiveBwExists) {
			throw IllegalAccessException("effectiveBw", "SpectralWindow");
		}
	
  		return effectiveBw;
 	}

 	/**
 	 * Set effectiveBw with the specified Frequency.
 	 * @param effectiveBw The Frequency value to which effectiveBw is to be set.
 	 
 	
 	 */
 	void SpectralWindowRow::setEffectiveBw (Frequency effectiveBw) {
	
 		this->effectiveBw = effectiveBw;
	
		effectiveBwExists = true;
	
 	}
	
	
	/**
	 * Mark effectiveBw, which is an optional field, as non-existent.
	 */
	void SpectralWindowRow::clearEffectiveBw () {
		effectiveBwExists = false;
	}
	

	
	/**
	 * The attribute effectiveBwArray is optional. Return true if this attribute exists.
	 * @return true if and only if the effectiveBwArray attribute exists. 
	 */
	bool SpectralWindowRow::isEffectiveBwArrayExists() const {
		return effectiveBwArrayExists;
	}
	

	
 	/**
 	 * Get effectiveBwArray, which is optional.
 	 * @return effectiveBwArray as std::vector<Frequency >
 	 * @throw IllegalAccessException If effectiveBwArray does not exist.
 	 */
 	std::vector<Frequency > SpectralWindowRow::getEffectiveBwArray() const  {
		if (!effectiveBwArrayExists) {
			throw IllegalAccessException("effectiveBwArray", "SpectralWindow");
		}
	
  		return effectiveBwArray;
 	}

 	/**
 	 * Set effectiveBwArray with the specified std::vector<Frequency >.
 	 * @param effectiveBwArray The std::vector<Frequency > value to which effectiveBwArray is to be set.
 	 
 	
 	 */
 	void SpectralWindowRow::setEffectiveBwArray (std::vector<Frequency > effectiveBwArray) {
	
 		this->effectiveBwArray = effectiveBwArray;
	
		effectiveBwArrayExists = true;
	
 	}
	
	
	/**
	 * Mark effectiveBwArray, which is an optional field, as non-existent.
	 */
	void SpectralWindowRow::clearEffectiveBwArray () {
		effectiveBwArrayExists = false;
	}
	

	
	/**
	 * The attribute freqGroup is optional. Return true if this attribute exists.
	 * @return true if and only if the freqGroup attribute exists. 
	 */
	bool SpectralWindowRow::isFreqGroupExists() const {
		return freqGroupExists;
	}
	

	
 	/**
 	 * Get freqGroup, which is optional.
 	 * @return freqGroup as int
 	 * @throw IllegalAccessException If freqGroup does not exist.
 	 */
 	int SpectralWindowRow::getFreqGroup() const  {
		if (!freqGroupExists) {
			throw IllegalAccessException("freqGroup", "SpectralWindow");
		}
	
  		return freqGroup;
 	}

 	/**
 	 * Set freqGroup with the specified int.
 	 * @param freqGroup The int value to which freqGroup is to be set.
 	 
 	
 	 */
 	void SpectralWindowRow::setFreqGroup (int freqGroup) {
	
 		this->freqGroup = freqGroup;
	
		freqGroupExists = true;
	
 	}
	
	
	/**
	 * Mark freqGroup, which is an optional field, as non-existent.
	 */
	void SpectralWindowRow::clearFreqGroup () {
		freqGroupExists = false;
	}
	

	
	/**
	 * The attribute freqGroupName is optional. Return true if this attribute exists.
	 * @return true if and only if the freqGroupName attribute exists. 
	 */
	bool SpectralWindowRow::isFreqGroupNameExists() const {
		return freqGroupNameExists;
	}
	

	
 	/**
 	 * Get freqGroupName, which is optional.
 	 * @return freqGroupName as std::string
 	 * @throw IllegalAccessException If freqGroupName does not exist.
 	 */
 	std::string SpectralWindowRow::getFreqGroupName() const  {
		if (!freqGroupNameExists) {
			throw IllegalAccessException("freqGroupName", "SpectralWindow");
		}
	
  		return freqGroupName;
 	}

 	/**
 	 * Set freqGroupName with the specified std::string.
 	 * @param freqGroupName The std::string value to which freqGroupName is to be set.
 	 
 	
 	 */
 	void SpectralWindowRow::setFreqGroupName (std::string freqGroupName) {
	
 		this->freqGroupName = freqGroupName;
	
		freqGroupNameExists = true;
	
 	}
	
	
	/**
	 * Mark freqGroupName, which is an optional field, as non-existent.
	 */
	void SpectralWindowRow::clearFreqGroupName () {
		freqGroupNameExists = false;
	}
	

	
	/**
	 * The attribute lineArray is optional. Return true if this attribute exists.
	 * @return true if and only if the lineArray attribute exists. 
	 */
	bool SpectralWindowRow::isLineArrayExists() const {
		return lineArrayExists;
	}
	

	
 	/**
 	 * Get lineArray, which is optional.
 	 * @return lineArray as std::vector<bool >
 	 * @throw IllegalAccessException If lineArray does not exist.
 	 */
 	std::vector<bool > SpectralWindowRow::getLineArray() const  {
		if (!lineArrayExists) {
			throw IllegalAccessException("lineArray", "SpectralWindow");
		}
	
  		return lineArray;
 	}

 	/**
 	 * Set lineArray with the specified std::vector<bool >.
 	 * @param lineArray The std::vector<bool > value to which lineArray is to be set.
 	 
 	
 	 */
 	void SpectralWindowRow::setLineArray (std::vector<bool > lineArray) {
	
 		this->lineArray = lineArray;
	
		lineArrayExists = true;
	
 	}
	
	
	/**
	 * Mark lineArray, which is an optional field, as non-existent.
	 */
	void SpectralWindowRow::clearLineArray () {
		lineArrayExists = false;
	}
	

	
	/**
	 * The attribute measFreqRef is optional. Return true if this attribute exists.
	 * @return true if and only if the measFreqRef attribute exists. 
	 */
	bool SpectralWindowRow::isMeasFreqRefExists() const {
		return measFreqRefExists;
	}
	

	
 	/**
 	 * Get measFreqRef, which is optional.
 	 * @return measFreqRef as FrequencyReferenceCodeMod::FrequencyReferenceCode
 	 * @throw IllegalAccessException If measFreqRef does not exist.
 	 */
 	FrequencyReferenceCodeMod::FrequencyReferenceCode SpectralWindowRow::getMeasFreqRef() const  {
		if (!measFreqRefExists) {
			throw IllegalAccessException("measFreqRef", "SpectralWindow");
		}
	
  		return measFreqRef;
 	}

 	/**
 	 * Set measFreqRef with the specified FrequencyReferenceCodeMod::FrequencyReferenceCode.
 	 * @param measFreqRef The FrequencyReferenceCodeMod::FrequencyReferenceCode value to which measFreqRef is to be set.
 	 
 	
 	 */
 	void SpectralWindowRow::setMeasFreqRef (FrequencyReferenceCodeMod::FrequencyReferenceCode measFreqRef) {
	
 		this->measFreqRef = measFreqRef;
	
		measFreqRefExists = true;
	
 	}
	
	
	/**
	 * Mark measFreqRef, which is an optional field, as non-existent.
	 */
	void SpectralWindowRow::clearMeasFreqRef () {
		measFreqRefExists = false;
	}
	

	
	/**
	 * The attribute name is optional. Return true if this attribute exists.
	 * @return true if and only if the name attribute exists. 
	 */
	bool SpectralWindowRow::isNameExists() const {
		return nameExists;
	}
	

	
 	/**
 	 * Get name, which is optional.
 	 * @return name as std::string
 	 * @throw IllegalAccessException If name does not exist.
 	 */
 	std::string SpectralWindowRow::getName() const  {
		if (!nameExists) {
			throw IllegalAccessException("name", "SpectralWindow");
		}
	
  		return name;
 	}

 	/**
 	 * Set name with the specified std::string.
 	 * @param name The std::string value to which name is to be set.
 	 
 	
 	 */
 	void SpectralWindowRow::setName (std::string name) {
	
 		this->name = name;
	
		nameExists = true;
	
 	}
	
	
	/**
	 * Mark name, which is an optional field, as non-existent.
	 */
	void SpectralWindowRow::clearName () {
		nameExists = false;
	}
	

	
	/**
	 * The attribute oversampling is optional. Return true if this attribute exists.
	 * @return true if and only if the oversampling attribute exists. 
	 */
	bool SpectralWindowRow::isOversamplingExists() const {
		return oversamplingExists;
	}
	

	
 	/**
 	 * Get oversampling, which is optional.
 	 * @return oversampling as bool
 	 * @throw IllegalAccessException If oversampling does not exist.
 	 */
 	bool SpectralWindowRow::getOversampling() const  {
		if (!oversamplingExists) {
			throw IllegalAccessException("oversampling", "SpectralWindow");
		}
	
  		return oversampling;
 	}

 	/**
 	 * Set oversampling with the specified bool.
 	 * @param oversampling The bool value to which oversampling is to be set.
 	 
 	
 	 */
 	void SpectralWindowRow::setOversampling (bool oversampling) {
	
 		this->oversampling = oversampling;
	
		oversamplingExists = true;
	
 	}
	
	
	/**
	 * Mark oversampling, which is an optional field, as non-existent.
	 */
	void SpectralWindowRow::clearOversampling () {
		oversamplingExists = false;
	}
	

	
	/**
	 * The attribute quantization is optional. Return true if this attribute exists.
	 * @return true if and only if the quantization attribute exists. 
	 */
	bool SpectralWindowRow::isQuantizationExists() const {
		return quantizationExists;
	}
	

	
 	/**
 	 * Get quantization, which is optional.
 	 * @return quantization as bool
 	 * @throw IllegalAccessException If quantization does not exist.
 	 */
 	bool SpectralWindowRow::getQuantization() const  {
		if (!quantizationExists) {
			throw IllegalAccessException("quantization", "SpectralWindow");
		}
	
  		return quantization;
 	}

 	/**
 	 * Set quantization with the specified bool.
 	 * @param quantization The bool value to which quantization is to be set.
 	 
 	
 	 */
 	void SpectralWindowRow::setQuantization (bool quantization) {
	
 		this->quantization = quantization;
	
		quantizationExists = true;
	
 	}
	
	
	/**
	 * Mark quantization, which is an optional field, as non-existent.
	 */
	void SpectralWindowRow::clearQuantization () {
		quantizationExists = false;
	}
	

	
	/**
	 * The attribute refChan is optional. Return true if this attribute exists.
	 * @return true if and only if the refChan attribute exists. 
	 */
	bool SpectralWindowRow::isRefChanExists() const {
		return refChanExists;
	}
	

	
 	/**
 	 * Get refChan, which is optional.
 	 * @return refChan as double
 	 * @throw IllegalAccessException If refChan does not exist.
 	 */
 	double SpectralWindowRow::getRefChan() const  {
		if (!refChanExists) {
			throw IllegalAccessException("refChan", "SpectralWindow");
		}
	
  		return refChan;
 	}

 	/**
 	 * Set refChan with the specified double.
 	 * @param refChan The double value to which refChan is to be set.
 	 
 	
 	 */
 	void SpectralWindowRow::setRefChan (double refChan) {
	
 		this->refChan = refChan;
	
		refChanExists = true;
	
 	}
	
	
	/**
	 * Mark refChan, which is an optional field, as non-existent.
	 */
	void SpectralWindowRow::clearRefChan () {
		refChanExists = false;
	}
	

	
	/**
	 * The attribute resolution is optional. Return true if this attribute exists.
	 * @return true if and only if the resolution attribute exists. 
	 */
	bool SpectralWindowRow::isResolutionExists() const {
		return resolutionExists;
	}
	

	
 	/**
 	 * Get resolution, which is optional.
 	 * @return resolution as Frequency
 	 * @throw IllegalAccessException If resolution does not exist.
 	 */
 	Frequency SpectralWindowRow::getResolution() const  {
		if (!resolutionExists) {
			throw IllegalAccessException("resolution", "SpectralWindow");
		}
	
  		return resolution;
 	}

 	/**
 	 * Set resolution with the specified Frequency.
 	 * @param resolution The Frequency value to which resolution is to be set.
 	 
 	
 	 */
 	void SpectralWindowRow::setResolution (Frequency resolution) {
	
 		this->resolution = resolution;
	
		resolutionExists = true;
	
 	}
	
	
	/**
	 * Mark resolution, which is an optional field, as non-existent.
	 */
	void SpectralWindowRow::clearResolution () {
		resolutionExists = false;
	}
	

	
	/**
	 * The attribute resolutionArray is optional. Return true if this attribute exists.
	 * @return true if and only if the resolutionArray attribute exists. 
	 */
	bool SpectralWindowRow::isResolutionArrayExists() const {
		return resolutionArrayExists;
	}
	

	
 	/**
 	 * Get resolutionArray, which is optional.
 	 * @return resolutionArray as std::vector<Frequency >
 	 * @throw IllegalAccessException If resolutionArray does not exist.
 	 */
 	std::vector<Frequency > SpectralWindowRow::getResolutionArray() const  {
		if (!resolutionArrayExists) {
			throw IllegalAccessException("resolutionArray", "SpectralWindow");
		}
	
  		return resolutionArray;
 	}

 	/**
 	 * Set resolutionArray with the specified std::vector<Frequency >.
 	 * @param resolutionArray The std::vector<Frequency > value to which resolutionArray is to be set.
 	 
 	
 	 */
 	void SpectralWindowRow::setResolutionArray (std::vector<Frequency > resolutionArray) {
	
 		this->resolutionArray = resolutionArray;
	
		resolutionArrayExists = true;
	
 	}
	
	
	/**
	 * Mark resolutionArray, which is an optional field, as non-existent.
	 */
	void SpectralWindowRow::clearResolutionArray () {
		resolutionArrayExists = false;
	}
	

	
	/**
	 * The attribute numAssocValues is optional. Return true if this attribute exists.
	 * @return true if and only if the numAssocValues attribute exists. 
	 */
	bool SpectralWindowRow::isNumAssocValuesExists() const {
		return numAssocValuesExists;
	}
	

	
 	/**
 	 * Get numAssocValues, which is optional.
 	 * @return numAssocValues as int
 	 * @throw IllegalAccessException If numAssocValues does not exist.
 	 */
 	int SpectralWindowRow::getNumAssocValues() const  {
		if (!numAssocValuesExists) {
			throw IllegalAccessException("numAssocValues", "SpectralWindow");
		}
	
  		return numAssocValues;
 	}

 	/**
 	 * Set numAssocValues with the specified int.
 	 * @param numAssocValues The int value to which numAssocValues is to be set.
 	 
 	
 	 */
 	void SpectralWindowRow::setNumAssocValues (int numAssocValues) {
	
 		this->numAssocValues = numAssocValues;
	
		numAssocValuesExists = true;
	
 	}
	
	
	/**
	 * Mark numAssocValues, which is an optional field, as non-existent.
	 */
	void SpectralWindowRow::clearNumAssocValues () {
		numAssocValuesExists = false;
	}
	

	
	/**
	 * The attribute assocNature is optional. Return true if this attribute exists.
	 * @return true if and only if the assocNature attribute exists. 
	 */
	bool SpectralWindowRow::isAssocNatureExists() const {
		return assocNatureExists;
	}
	

	
 	/**
 	 * Get assocNature, which is optional.
 	 * @return assocNature as std::vector<SpectralResolutionTypeMod::SpectralResolutionType >
 	 * @throw IllegalAccessException If assocNature does not exist.
 	 */
 	std::vector<SpectralResolutionTypeMod::SpectralResolutionType > SpectralWindowRow::getAssocNature() const  {
		if (!assocNatureExists) {
			throw IllegalAccessException("assocNature", "SpectralWindow");
		}
	
  		return assocNature;
 	}

 	/**
 	 * Set assocNature with the specified std::vector<SpectralResolutionTypeMod::SpectralResolutionType >.
 	 * @param assocNature The std::vector<SpectralResolutionTypeMod::SpectralResolutionType > value to which assocNature is to be set.
 	 
 	
 	 */
 	void SpectralWindowRow::setAssocNature (std::vector<SpectralResolutionTypeMod::SpectralResolutionType > assocNature) {
	
 		this->assocNature = assocNature;
	
		assocNatureExists = true;
	
 	}
	
	
	/**
	 * Mark assocNature, which is an optional field, as non-existent.
	 */
	void SpectralWindowRow::clearAssocNature () {
		assocNatureExists = false;
	}
	

	
	///////////////////////////////////////////////
	// Extrinsic Table Attributes getters/setters//
	///////////////////////////////////////////////
	
	
	/**
	 * The attribute assocSpectralWindowId is optional. Return true if this attribute exists.
	 * @return true if and only if the assocSpectralWindowId attribute exists. 
	 */
	bool SpectralWindowRow::isAssocSpectralWindowIdExists() const {
		return assocSpectralWindowIdExists;
	}
	

	
 	/**
 	 * Get assocSpectralWindowId, which is optional.
 	 * @return assocSpectralWindowId as std::vector<Tag> 
 	 * @throw IllegalAccessException If assocSpectralWindowId does not exist.
 	 */
 	std::vector<Tag>  SpectralWindowRow::getAssocSpectralWindowId() const  {
		if (!assocSpectralWindowIdExists) {
			throw IllegalAccessException("assocSpectralWindowId", "SpectralWindow");
		}
	
  		return assocSpectralWindowId;
 	}

 	/**
 	 * Set assocSpectralWindowId with the specified std::vector<Tag> .
 	 * @param assocSpectralWindowId The std::vector<Tag>  value to which assocSpectralWindowId is to be set.
 	 
 	
 	 */
 	void SpectralWindowRow::setAssocSpectralWindowId (std::vector<Tag>  assocSpectralWindowId) {
	
 		this->assocSpectralWindowId = assocSpectralWindowId;
	
		assocSpectralWindowIdExists = true;
	
 	}
	
	
	/**
	 * Mark assocSpectralWindowId, which is an optional field, as non-existent.
	 */
	void SpectralWindowRow::clearAssocSpectralWindowId () {
		assocSpectralWindowIdExists = false;
	}
	

	
	/**
	 * The attribute dopplerId is optional. Return true if this attribute exists.
	 * @return true if and only if the dopplerId attribute exists. 
	 */
	bool SpectralWindowRow::isDopplerIdExists() const {
		return dopplerIdExists;
	}
	

	
 	/**
 	 * Get dopplerId, which is optional.
 	 * @return dopplerId as int
 	 * @throw IllegalAccessException If dopplerId does not exist.
 	 */
 	int SpectralWindowRow::getDopplerId() const  {
		if (!dopplerIdExists) {
			throw IllegalAccessException("dopplerId", "SpectralWindow");
		}
	
  		return dopplerId;
 	}

 	/**
 	 * Set dopplerId with the specified int.
 	 * @param dopplerId The int value to which dopplerId is to be set.
 	 
 	
 	 */
 	void SpectralWindowRow::setDopplerId (int dopplerId) {
	
 		this->dopplerId = dopplerId;
	
		dopplerIdExists = true;
	
 	}
	
	
	/**
	 * Mark dopplerId, which is an optional field, as non-existent.
	 */
	void SpectralWindowRow::clearDopplerId () {
		dopplerIdExists = false;
	}
	

	
	/**
	 * The attribute imageSpectralWindowId is optional. Return true if this attribute exists.
	 * @return true if and only if the imageSpectralWindowId attribute exists. 
	 */
	bool SpectralWindowRow::isImageSpectralWindowIdExists() const {
		return imageSpectralWindowIdExists;
	}
	

	
 	/**
 	 * Get imageSpectralWindowId, which is optional.
 	 * @return imageSpectralWindowId as Tag
 	 * @throw IllegalAccessException If imageSpectralWindowId does not exist.
 	 */
 	Tag SpectralWindowRow::getImageSpectralWindowId() const  {
		if (!imageSpectralWindowIdExists) {
			throw IllegalAccessException("imageSpectralWindowId", "SpectralWindow");
		}
	
  		return imageSpectralWindowId;
 	}

 	/**
 	 * Set imageSpectralWindowId with the specified Tag.
 	 * @param imageSpectralWindowId The Tag value to which imageSpectralWindowId is to be set.
 	 
 	
 	 */
 	void SpectralWindowRow::setImageSpectralWindowId (Tag imageSpectralWindowId) {
	
 		this->imageSpectralWindowId = imageSpectralWindowId;
	
		imageSpectralWindowIdExists = true;
	
 	}
	
	
	/**
	 * Mark imageSpectralWindowId, which is an optional field, as non-existent.
	 */
	void SpectralWindowRow::clearImageSpectralWindowId () {
		imageSpectralWindowIdExists = false;
	}
	


	//////////////////////////////////////
	// Links Attributes getters/setters //
	//////////////////////////////////////
	
	
 		
 	/**
 	 * Set assocSpectralWindowId[i] with the specified Tag.
 	 * @param i The index in assocSpectralWindowId where to set the Tag value.
 	 * @param assocSpectralWindowId The Tag value to which assocSpectralWindowId[i] is to be set. 
 	 * @throws OutOfBoundsException
  	 */
  	void SpectralWindowRow::setAssocSpectralWindowId (int i, Tag assocSpectralWindowId) {
  		if ((i < 0) || (i > ((int) this->assocSpectralWindowId.size())))
  			throw OutOfBoundsException("Index out of bounds during a set operation on attribute assocSpectralWindowId in table SpectralWindowTable");
  		std::vector<Tag> ::iterator iter = this->assocSpectralWindowId.begin();
  		int j = 0;
  		while (j < i) {
  			j++; iter++;
  		}
  		this->assocSpectralWindowId.insert(this->assocSpectralWindowId.erase(iter), assocSpectralWindowId); 	
  	}
 			
	
	
	
		
/**
 * Append a Tag to assocSpectralWindowId.
 * @param id the Tag to be appended to assocSpectralWindowId
 */
 void SpectralWindowRow::addAssocSpectralWindowId(Tag id){
 	assocSpectralWindowId.push_back(id);
}

/**
 * Append an array of Tag to assocSpectralWindowId.
 * @param id an array of Tag to be appended to assocSpectralWindowId
 */
 void SpectralWindowRow::addAssocSpectralWindowId(const std::vector<Tag> & id) {
 	for (unsigned int i=0; i < id.size(); i++)
 		assocSpectralWindowId.push_back(id.at(i));
 }
 

 /**
  * Returns the Tag stored in assocSpectralWindowId at position i.
  *
  */
 const Tag SpectralWindowRow::getAssocSpectralWindowId(int i) {
 	return assocSpectralWindowId.at(i);
 }
 
 /**
  * Returns the SpectralWindowRow linked to this row via the Tag stored in assocSpectralWindowId
  * at position i.
  */
 SpectralWindowRow* SpectralWindowRow::getSpectralWindowUsingAssocSpectralWindowId(int i) {
 	return table.getContainer().getSpectralWindow().getRowByKey(assocSpectralWindowId.at(i));
 } 
 
 /**
  * Returns the vector of SpectralWindowRow* linked to this row via the Tags stored in assocSpectralWindowId
  *
  */
 vector<SpectralWindowRow *> SpectralWindowRow::getSpectralWindowsUsingAssocSpectralWindowId() {
 	vector<SpectralWindowRow *> result;
 	for (unsigned int i = 0; i < assocSpectralWindowId.size(); i++)
 		result.push_back(table.getContainer().getSpectralWindow().getRowByKey(assocSpectralWindowId.at(i)));
 		
 	return result;
 }
  

	

	
	
	
		

	/**
	 * Returns the pointer to the row in the SpectralWindow table having SpectralWindow.imageSpectralWindowId == imageSpectralWindowId
	 * @return a SpectralWindowRow*
	 * 
	 
	 * throws IllegalAccessException
	 
	 */
	 SpectralWindowRow* SpectralWindowRow::getSpectralWindowUsingImageSpectralWindowId() {
	 
	 	if (!imageSpectralWindowIdExists)
	 		throw IllegalAccessException();	 		 
	 
	 	return table.getContainer().getSpectralWindow().getRowByKey(imageSpectralWindowId);
	 }
	 

	

	
	
	
		

	// ===> Slice link from a row of SpectralWindow table to a collection of row of Doppler table.
	
	/**
	 * Get the collection of row in the Doppler table having their attribut dopplerId == this->dopplerId
	 */
	vector <DopplerRow *> SpectralWindowRow::getDopplers() {
		
			if (dopplerIdExists) {
				return table.getContainer().getDoppler().getRowByDopplerId(dopplerId);
			}
			else 
				throw IllegalAccessException();
		
	}
	

	

	
	/**
	 * Create a SpectralWindowRow.
	 * <p>
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 */ 
	SpectralWindowRow::SpectralWindowRow (SpectralWindowTable &t) : table(t) {
		hasBeenAdded = false;
		
	
	

	

	

	

	
		numBinExists = false;
	

	

	

	

	

	
		chanFreqStartExists = false;
	

	
		chanFreqStepExists = false;
	

	
		chanFreqArrayExists = false;
	

	
		chanWidthExists = false;
	

	
		chanWidthArrayExists = false;
	

	
		correlationBitExists = false;
	

	
		effectiveBwExists = false;
	

	
		effectiveBwArrayExists = false;
	

	
		freqGroupExists = false;
	

	
		freqGroupNameExists = false;
	

	
		lineArrayExists = false;
	

	
		measFreqRefExists = false;
	

	
		nameExists = false;
	

	
		oversamplingExists = false;
	

	
		quantizationExists = false;
	

	
		refChanExists = false;
	

	
		resolutionExists = false;
	

	
		resolutionArrayExists = false;
	

	
		numAssocValuesExists = false;
	

	
		assocNatureExists = false;
	

	
	
		assocSpectralWindowIdExists = false;
	

	
		dopplerIdExists = false;
	

	
		imageSpectralWindowIdExists = false;
	

	
	
	
	

	
// This attribute is scalar and has an enumeration type. Let's initialize it to some valid value (the 1st of the enumeration).		
basebandName = CBasebandName::from_int(0);
	

	
// This attribute is scalar and has an enumeration type. Let's initialize it to some valid value (the 1st of the enumeration).		
netSideband = CNetSideband::from_int(0);
	

	

	

	

	
// This attribute is scalar and has an enumeration type. Let's initialize it to some valid value (the 1st of the enumeration).		
sidebandProcessingMode = CSidebandProcessingMode::from_int(0);
	

	

	
// This attribute is scalar and has an enumeration type. Let's initialize it to some valid value (the 1st of the enumeration).		
windowFunction = CWindowFunction::from_int(0);
	

	

	

	

	

	

	
// This attribute is scalar and has an enumeration type. Let's initialize it to some valid value (the 1st of the enumeration).		
correlationBit = CCorrelationBit::from_int(0);
	

	

	

	

	

	

	
// This attribute is scalar and has an enumeration type. Let's initialize it to some valid value (the 1st of the enumeration).		
measFreqRef = CFrequencyReferenceCode::from_int(0);
	

	

	

	

	

	

	

	

	

	
	
	 fromBinMethods["spectralWindowId"] = &SpectralWindowRow::spectralWindowIdFromBin; 
	 fromBinMethods["basebandName"] = &SpectralWindowRow::basebandNameFromBin; 
	 fromBinMethods["netSideband"] = &SpectralWindowRow::netSidebandFromBin; 
	 fromBinMethods["numChan"] = &SpectralWindowRow::numChanFromBin; 
	 fromBinMethods["refFreq"] = &SpectralWindowRow::refFreqFromBin; 
	 fromBinMethods["sidebandProcessingMode"] = &SpectralWindowRow::sidebandProcessingModeFromBin; 
	 fromBinMethods["totBandwidth"] = &SpectralWindowRow::totBandwidthFromBin; 
	 fromBinMethods["windowFunction"] = &SpectralWindowRow::windowFunctionFromBin; 
		
	
	 fromBinMethods["numBin"] = &SpectralWindowRow::numBinFromBin; 
	 fromBinMethods["chanFreqStart"] = &SpectralWindowRow::chanFreqStartFromBin; 
	 fromBinMethods["chanFreqStep"] = &SpectralWindowRow::chanFreqStepFromBin; 
	 fromBinMethods["chanFreqArray"] = &SpectralWindowRow::chanFreqArrayFromBin; 
	 fromBinMethods["chanWidth"] = &SpectralWindowRow::chanWidthFromBin; 
	 fromBinMethods["chanWidthArray"] = &SpectralWindowRow::chanWidthArrayFromBin; 
	 fromBinMethods["correlationBit"] = &SpectralWindowRow::correlationBitFromBin; 
	 fromBinMethods["effectiveBw"] = &SpectralWindowRow::effectiveBwFromBin; 
	 fromBinMethods["effectiveBwArray"] = &SpectralWindowRow::effectiveBwArrayFromBin; 
	 fromBinMethods["freqGroup"] = &SpectralWindowRow::freqGroupFromBin; 
	 fromBinMethods["freqGroupName"] = &SpectralWindowRow::freqGroupNameFromBin; 
	 fromBinMethods["lineArray"] = &SpectralWindowRow::lineArrayFromBin; 
	 fromBinMethods["measFreqRef"] = &SpectralWindowRow::measFreqRefFromBin; 
	 fromBinMethods["name"] = &SpectralWindowRow::nameFromBin; 
	 fromBinMethods["oversampling"] = &SpectralWindowRow::oversamplingFromBin; 
	 fromBinMethods["quantization"] = &SpectralWindowRow::quantizationFromBin; 
	 fromBinMethods["refChan"] = &SpectralWindowRow::refChanFromBin; 
	 fromBinMethods["resolution"] = &SpectralWindowRow::resolutionFromBin; 
	 fromBinMethods["resolutionArray"] = &SpectralWindowRow::resolutionArrayFromBin; 
	 fromBinMethods["numAssocValues"] = &SpectralWindowRow::numAssocValuesFromBin; 
	 fromBinMethods["assocNature"] = &SpectralWindowRow::assocNatureFromBin; 
	 fromBinMethods["assocSpectralWindowId"] = &SpectralWindowRow::assocSpectralWindowIdFromBin; 
	 fromBinMethods["imageSpectralWindowId"] = &SpectralWindowRow::imageSpectralWindowIdFromBin; 
	 fromBinMethods["dopplerId"] = &SpectralWindowRow::dopplerIdFromBin; 
	
	
	
	
				 
	fromTextMethods["spectralWindowId"] = &SpectralWindowRow::spectralWindowIdFromText;
		 
	
				 
	fromTextMethods["basebandName"] = &SpectralWindowRow::basebandNameFromText;
		 
	
				 
	fromTextMethods["netSideband"] = &SpectralWindowRow::netSidebandFromText;
		 
	
				 
	fromTextMethods["numChan"] = &SpectralWindowRow::numChanFromText;
		 
	
				 
	fromTextMethods["refFreq"] = &SpectralWindowRow::refFreqFromText;
		 
	
				 
	fromTextMethods["sidebandProcessingMode"] = &SpectralWindowRow::sidebandProcessingModeFromText;
		 
	
				 
	fromTextMethods["totBandwidth"] = &SpectralWindowRow::totBandwidthFromText;
		 
	
				 
	fromTextMethods["windowFunction"] = &SpectralWindowRow::windowFunctionFromText;
		 
	

	 
				
	fromTextMethods["numBin"] = &SpectralWindowRow::numBinFromText;
		 	
	 
				
	fromTextMethods["chanFreqStart"] = &SpectralWindowRow::chanFreqStartFromText;
		 	
	 
				
	fromTextMethods["chanFreqStep"] = &SpectralWindowRow::chanFreqStepFromText;
		 	
	 
				
	fromTextMethods["chanFreqArray"] = &SpectralWindowRow::chanFreqArrayFromText;
		 	
	 
				
	fromTextMethods["chanWidth"] = &SpectralWindowRow::chanWidthFromText;
		 	
	 
				
	fromTextMethods["chanWidthArray"] = &SpectralWindowRow::chanWidthArrayFromText;
		 	
	 
				
	fromTextMethods["correlationBit"] = &SpectralWindowRow::correlationBitFromText;
		 	
	 
				
	fromTextMethods["effectiveBw"] = &SpectralWindowRow::effectiveBwFromText;
		 	
	 
				
	fromTextMethods["effectiveBwArray"] = &SpectralWindowRow::effectiveBwArrayFromText;
		 	
	 
				
	fromTextMethods["freqGroup"] = &SpectralWindowRow::freqGroupFromText;
		 	
	 
				
	fromTextMethods["freqGroupName"] = &SpectralWindowRow::freqGroupNameFromText;
		 	
	 
				
	fromTextMethods["lineArray"] = &SpectralWindowRow::lineArrayFromText;
		 	
	 
				
	fromTextMethods["measFreqRef"] = &SpectralWindowRow::measFreqRefFromText;
		 	
	 
				
	fromTextMethods["name"] = &SpectralWindowRow::nameFromText;
		 	
	 
				
	fromTextMethods["oversampling"] = &SpectralWindowRow::oversamplingFromText;
		 	
	 
				
	fromTextMethods["quantization"] = &SpectralWindowRow::quantizationFromText;
		 	
	 
				
	fromTextMethods["refChan"] = &SpectralWindowRow::refChanFromText;
		 	
	 
				
	fromTextMethods["resolution"] = &SpectralWindowRow::resolutionFromText;
		 	
	 
				
	fromTextMethods["resolutionArray"] = &SpectralWindowRow::resolutionArrayFromText;
		 	
	 
				
	fromTextMethods["numAssocValues"] = &SpectralWindowRow::numAssocValuesFromText;
		 	
	 
				
	fromTextMethods["assocNature"] = &SpectralWindowRow::assocNatureFromText;
		 	
	 
				
	fromTextMethods["assocSpectralWindowId"] = &SpectralWindowRow::assocSpectralWindowIdFromText;
		 	
	 
				
	fromTextMethods["imageSpectralWindowId"] = &SpectralWindowRow::imageSpectralWindowIdFromText;
		 	
	 
				
	fromTextMethods["dopplerId"] = &SpectralWindowRow::dopplerIdFromText;
		 	
		
	}
	
	SpectralWindowRow::SpectralWindowRow (SpectralWindowTable &t, SpectralWindowRow *row) : table(t) {
		hasBeenAdded = false;
		
		if (row == 0) {
	
	
	

	

	

	

	
		numBinExists = false;
	

	

	

	

	

	
		chanFreqStartExists = false;
	

	
		chanFreqStepExists = false;
	

	
		chanFreqArrayExists = false;
	

	
		chanWidthExists = false;
	

	
		chanWidthArrayExists = false;
	

	
		correlationBitExists = false;
	

	
		effectiveBwExists = false;
	

	
		effectiveBwArrayExists = false;
	

	
		freqGroupExists = false;
	

	
		freqGroupNameExists = false;
	

	
		lineArrayExists = false;
	

	
		measFreqRefExists = false;
	

	
		nameExists = false;
	

	
		oversamplingExists = false;
	

	
		quantizationExists = false;
	

	
		refChanExists = false;
	

	
		resolutionExists = false;
	

	
		resolutionArrayExists = false;
	

	
		numAssocValuesExists = false;
	

	
		assocNatureExists = false;
	

	
	
		assocSpectralWindowIdExists = false;
	

	
		dopplerIdExists = false;
	

	
		imageSpectralWindowIdExists = false;
	
		
		}
		else {
	
		
			spectralWindowId = row->spectralWindowId;
		
		
		
		
			basebandName = row->basebandName;
		
			netSideband = row->netSideband;
		
			numChan = row->numChan;
		
			refFreq = row->refFreq;
		
			sidebandProcessingMode = row->sidebandProcessingMode;
		
			totBandwidth = row->totBandwidth;
		
			windowFunction = row->windowFunction;
		
		
		
		
		if (row->numBinExists) {
			numBin = row->numBin;		
			numBinExists = true;
		}
		else
			numBinExists = false;
		
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
		
		if (row->chanFreqArrayExists) {
			chanFreqArray = row->chanFreqArray;		
			chanFreqArrayExists = true;
		}
		else
			chanFreqArrayExists = false;
		
		if (row->chanWidthExists) {
			chanWidth = row->chanWidth;		
			chanWidthExists = true;
		}
		else
			chanWidthExists = false;
		
		if (row->chanWidthArrayExists) {
			chanWidthArray = row->chanWidthArray;		
			chanWidthArrayExists = true;
		}
		else
			chanWidthArrayExists = false;
		
		if (row->correlationBitExists) {
			correlationBit = row->correlationBit;		
			correlationBitExists = true;
		}
		else
			correlationBitExists = false;
		
		if (row->effectiveBwExists) {
			effectiveBw = row->effectiveBw;		
			effectiveBwExists = true;
		}
		else
			effectiveBwExists = false;
		
		if (row->effectiveBwArrayExists) {
			effectiveBwArray = row->effectiveBwArray;		
			effectiveBwArrayExists = true;
		}
		else
			effectiveBwArrayExists = false;
		
		if (row->freqGroupExists) {
			freqGroup = row->freqGroup;		
			freqGroupExists = true;
		}
		else
			freqGroupExists = false;
		
		if (row->freqGroupNameExists) {
			freqGroupName = row->freqGroupName;		
			freqGroupNameExists = true;
		}
		else
			freqGroupNameExists = false;
		
		if (row->lineArrayExists) {
			lineArray = row->lineArray;		
			lineArrayExists = true;
		}
		else
			lineArrayExists = false;
		
		if (row->measFreqRefExists) {
			measFreqRef = row->measFreqRef;		
			measFreqRefExists = true;
		}
		else
			measFreqRefExists = false;
		
		if (row->nameExists) {
			name = row->name;		
			nameExists = true;
		}
		else
			nameExists = false;
		
		if (row->oversamplingExists) {
			oversampling = row->oversampling;		
			oversamplingExists = true;
		}
		else
			oversamplingExists = false;
		
		if (row->quantizationExists) {
			quantization = row->quantization;		
			quantizationExists = true;
		}
		else
			quantizationExists = false;
		
		if (row->refChanExists) {
			refChan = row->refChan;		
			refChanExists = true;
		}
		else
			refChanExists = false;
		
		if (row->resolutionExists) {
			resolution = row->resolution;		
			resolutionExists = true;
		}
		else
			resolutionExists = false;
		
		if (row->resolutionArrayExists) {
			resolutionArray = row->resolutionArray;		
			resolutionArrayExists = true;
		}
		else
			resolutionArrayExists = false;
		
		if (row->numAssocValuesExists) {
			numAssocValues = row->numAssocValues;		
			numAssocValuesExists = true;
		}
		else
			numAssocValuesExists = false;
		
		if (row->assocNatureExists) {
			assocNature = row->assocNature;		
			assocNatureExists = true;
		}
		else
			assocNatureExists = false;
		
		if (row->assocSpectralWindowIdExists) {
			assocSpectralWindowId = row->assocSpectralWindowId;		
			assocSpectralWindowIdExists = true;
		}
		else
			assocSpectralWindowIdExists = false;
		
		if (row->imageSpectralWindowIdExists) {
			imageSpectralWindowId = row->imageSpectralWindowId;		
			imageSpectralWindowIdExists = true;
		}
		else
			imageSpectralWindowIdExists = false;
		
		if (row->dopplerIdExists) {
			dopplerId = row->dopplerId;		
			dopplerIdExists = true;
		}
		else
			dopplerIdExists = false;
		
		}
		
		 fromBinMethods["spectralWindowId"] = &SpectralWindowRow::spectralWindowIdFromBin; 
		 fromBinMethods["basebandName"] = &SpectralWindowRow::basebandNameFromBin; 
		 fromBinMethods["netSideband"] = &SpectralWindowRow::netSidebandFromBin; 
		 fromBinMethods["numChan"] = &SpectralWindowRow::numChanFromBin; 
		 fromBinMethods["refFreq"] = &SpectralWindowRow::refFreqFromBin; 
		 fromBinMethods["sidebandProcessingMode"] = &SpectralWindowRow::sidebandProcessingModeFromBin; 
		 fromBinMethods["totBandwidth"] = &SpectralWindowRow::totBandwidthFromBin; 
		 fromBinMethods["windowFunction"] = &SpectralWindowRow::windowFunctionFromBin; 
			
	
		 fromBinMethods["numBin"] = &SpectralWindowRow::numBinFromBin; 
		 fromBinMethods["chanFreqStart"] = &SpectralWindowRow::chanFreqStartFromBin; 
		 fromBinMethods["chanFreqStep"] = &SpectralWindowRow::chanFreqStepFromBin; 
		 fromBinMethods["chanFreqArray"] = &SpectralWindowRow::chanFreqArrayFromBin; 
		 fromBinMethods["chanWidth"] = &SpectralWindowRow::chanWidthFromBin; 
		 fromBinMethods["chanWidthArray"] = &SpectralWindowRow::chanWidthArrayFromBin; 
		 fromBinMethods["correlationBit"] = &SpectralWindowRow::correlationBitFromBin; 
		 fromBinMethods["effectiveBw"] = &SpectralWindowRow::effectiveBwFromBin; 
		 fromBinMethods["effectiveBwArray"] = &SpectralWindowRow::effectiveBwArrayFromBin; 
		 fromBinMethods["freqGroup"] = &SpectralWindowRow::freqGroupFromBin; 
		 fromBinMethods["freqGroupName"] = &SpectralWindowRow::freqGroupNameFromBin; 
		 fromBinMethods["lineArray"] = &SpectralWindowRow::lineArrayFromBin; 
		 fromBinMethods["measFreqRef"] = &SpectralWindowRow::measFreqRefFromBin; 
		 fromBinMethods["name"] = &SpectralWindowRow::nameFromBin; 
		 fromBinMethods["oversampling"] = &SpectralWindowRow::oversamplingFromBin; 
		 fromBinMethods["quantization"] = &SpectralWindowRow::quantizationFromBin; 
		 fromBinMethods["refChan"] = &SpectralWindowRow::refChanFromBin; 
		 fromBinMethods["resolution"] = &SpectralWindowRow::resolutionFromBin; 
		 fromBinMethods["resolutionArray"] = &SpectralWindowRow::resolutionArrayFromBin; 
		 fromBinMethods["numAssocValues"] = &SpectralWindowRow::numAssocValuesFromBin; 
		 fromBinMethods["assocNature"] = &SpectralWindowRow::assocNatureFromBin; 
		 fromBinMethods["assocSpectralWindowId"] = &SpectralWindowRow::assocSpectralWindowIdFromBin; 
		 fromBinMethods["imageSpectralWindowId"] = &SpectralWindowRow::imageSpectralWindowIdFromBin; 
		 fromBinMethods["dopplerId"] = &SpectralWindowRow::dopplerIdFromBin; 
			
	}

	
	bool SpectralWindowRow::compareNoAutoInc(BasebandNameMod::BasebandName basebandName, NetSidebandMod::NetSideband netSideband, int numChan, Frequency refFreq, SidebandProcessingModeMod::SidebandProcessingMode sidebandProcessingMode, Frequency totBandwidth, WindowFunctionMod::WindowFunction windowFunction) {
		bool result;
		result = true;
		
	
		
		result = result && (this->basebandName == basebandName);
		
		if (!result) return false;
	

	
		
		result = result && (this->netSideband == netSideband);
		
		if (!result) return false;
	

	
		
		result = result && (this->numChan == numChan);
		
		if (!result) return false;
	

	
		
		result = result && (this->refFreq == refFreq);
		
		if (!result) return false;
	

	
		
		result = result && (this->sidebandProcessingMode == sidebandProcessingMode);
		
		if (!result) return false;
	

	
		
		result = result && (this->totBandwidth == totBandwidth);
		
		if (!result) return false;
	

	
		
		result = result && (this->windowFunction == windowFunction);
		
		if (!result) return false;
	

		return result;
	}	
	
	
	
	bool SpectralWindowRow::compareRequiredValue(BasebandNameMod::BasebandName basebandName, NetSidebandMod::NetSideband netSideband, int numChan, Frequency refFreq, SidebandProcessingModeMod::SidebandProcessingMode sidebandProcessingMode, Frequency totBandwidth, WindowFunctionMod::WindowFunction windowFunction) {
		bool result;
		result = true;
		
	
		if (!(this->basebandName == basebandName)) return false;
	

	
		if (!(this->netSideband == netSideband)) return false;
	

	
		if (!(this->numChan == numChan)) return false;
	

	
		if (!(this->refFreq == refFreq)) return false;
	

	
		if (!(this->sidebandProcessingMode == sidebandProcessingMode)) return false;
	

	
		if (!(this->totBandwidth == totBandwidth)) return false;
	

	
		if (!(this->windowFunction == windowFunction)) return false;
	

		return result;
	}
	
	
	/**
	 * Return true if all required attributes of the value part are equal to their homologues
	 * in x and false otherwise.
	 *

	 * @param x a pointer on the SpectralWindowRow whose required attributes of the value part 

	 * will be compared with those of this.
	 * @return a boolean.
	 */
	bool SpectralWindowRow::equalByRequiredValue(SpectralWindowRow*  x ) {
		
			
		if (this->basebandName != x->basebandName) return false;
			
		if (this->netSideband != x->netSideband) return false;
			
		if (this->numChan != x->numChan) return false;
			
		if (this->refFreq != x->refFreq) return false;
			
		if (this->sidebandProcessingMode != x->sidebandProcessingMode) return false;
			
		if (this->totBandwidth != x->totBandwidth) return false;
			
		if (this->windowFunction != x->windowFunction) return false;
			
		
		return true;
	}	
	
/*
	 map<string, SpectralWindowAttributeFromBin> SpectralWindowRow::initFromBinMethods() {
		map<string, SpectralWindowAttributeFromBin> result;
		
		result["spectralWindowId"] = &SpectralWindowRow::spectralWindowIdFromBin;
		result["basebandName"] = &SpectralWindowRow::basebandNameFromBin;
		result["netSideband"] = &SpectralWindowRow::netSidebandFromBin;
		result["numChan"] = &SpectralWindowRow::numChanFromBin;
		result["refFreq"] = &SpectralWindowRow::refFreqFromBin;
		result["sidebandProcessingMode"] = &SpectralWindowRow::sidebandProcessingModeFromBin;
		result["totBandwidth"] = &SpectralWindowRow::totBandwidthFromBin;
		result["windowFunction"] = &SpectralWindowRow::windowFunctionFromBin;
		
		
		result["numBin"] = &SpectralWindowRow::numBinFromBin;
		result["chanFreqStart"] = &SpectralWindowRow::chanFreqStartFromBin;
		result["chanFreqStep"] = &SpectralWindowRow::chanFreqStepFromBin;
		result["chanFreqArray"] = &SpectralWindowRow::chanFreqArrayFromBin;
		result["chanWidth"] = &SpectralWindowRow::chanWidthFromBin;
		result["chanWidthArray"] = &SpectralWindowRow::chanWidthArrayFromBin;
		result["correlationBit"] = &SpectralWindowRow::correlationBitFromBin;
		result["effectiveBw"] = &SpectralWindowRow::effectiveBwFromBin;
		result["effectiveBwArray"] = &SpectralWindowRow::effectiveBwArrayFromBin;
		result["freqGroup"] = &SpectralWindowRow::freqGroupFromBin;
		result["freqGroupName"] = &SpectralWindowRow::freqGroupNameFromBin;
		result["lineArray"] = &SpectralWindowRow::lineArrayFromBin;
		result["measFreqRef"] = &SpectralWindowRow::measFreqRefFromBin;
		result["name"] = &SpectralWindowRow::nameFromBin;
		result["oversampling"] = &SpectralWindowRow::oversamplingFromBin;
		result["quantization"] = &SpectralWindowRow::quantizationFromBin;
		result["refChan"] = &SpectralWindowRow::refChanFromBin;
		result["resolution"] = &SpectralWindowRow::resolutionFromBin;
		result["resolutionArray"] = &SpectralWindowRow::resolutionArrayFromBin;
		result["numAssocValues"] = &SpectralWindowRow::numAssocValuesFromBin;
		result["assocNature"] = &SpectralWindowRow::assocNatureFromBin;
		result["assocSpectralWindowId"] = &SpectralWindowRow::assocSpectralWindowIdFromBin;
		result["imageSpectralWindowId"] = &SpectralWindowRow::imageSpectralWindowIdFromBin;
		result["dopplerId"] = &SpectralWindowRow::dopplerIdFromBin;
			
		
		return result;	
	}
*/	
} // End namespace asdm
 
