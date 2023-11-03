
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
 * File FlagRow.cpp
 */
 
#include <vector>
#include <set>

#include <alma/ASDM/ASDM.h>
#include <alma/ASDM/FlagRow.h>
#include <alma/ASDM/FlagTable.h>

#include <alma/ASDM/AntennaTable.h>
#include <alma/ASDM/AntennaRow.h>

#include <alma/ASDM/AntennaTable.h>
#include <alma/ASDM/AntennaRow.h>

#include <alma/ASDM/SpectralWindowTable.h>
#include <alma/ASDM/SpectralWindowRow.h>
	

using asdm::ASDM;
using asdm::FlagRow;
using asdm::FlagTable;

using asdm::AntennaTable;
using asdm::AntennaRow;

using asdm::AntennaTable;
using asdm::AntennaRow;

using asdm::SpectralWindowTable;
using asdm::SpectralWindowRow;


#include <alma/ASDM/Parser.h>

#include <alma/ASDM/EnumerationParser.h>
#include <alma/ASDM/ASDMValuesParser.h>
 
#include <alma/ASDM/InvalidArgumentException.h>

using namespace std;

namespace asdm {
	FlagRow::~FlagRow() {
	}

	/**
	 * Return the table to which this row belongs.
	 */
	FlagTable &FlagRow::getTable() const {
		return table;
	}

	bool FlagRow::isAdded() const {
		return hasBeenAdded;
	}	

	void FlagRow::isAdded(bool added) {
		hasBeenAdded = added;
	}
	
#ifndef WITHOUT_ACS
	using asdmIDL::FlagRowIDL;
#endif
	
#ifndef WITHOUT_ACS
	/**
	 * Return this row in the form of an IDL struct.
	 * @return The values of this row as a FlagRowIDL struct.
	 */
	FlagRowIDL *FlagRow::toIDL() const {
		FlagRowIDL *x = new FlagRowIDL ();
		
		// Fill the IDL structure.
	
		
	
  		
		
		
			
		x->flagId = flagId.toIDLTag();
			
		
	

	
  		
		
		
			
		x->startTime = startTime.toIDLArrayTime();
			
		
	

	
  		
		
		
			
		x->endTime = endTime.toIDLArrayTime();
			
		
	

	
  		
		
		
			
				
		x->reason = CORBA::string_dup(reason.c_str());
				
 			
		
	

	
  		
		
		
			
				
		x->numAntenna = numAntenna;
 				
 			
		
	

	
  		
		
		x->numPolarizationTypeExists = numPolarizationTypeExists;
		
		
			
				
		x->numPolarizationType = numPolarizationType;
 				
 			
		
	

	
  		
		
		x->numSpectralWindowExists = numSpectralWindowExists;
		
		
			
				
		x->numSpectralWindow = numSpectralWindow;
 				
 			
		
	

	
  		
		
		x->numPairedAntennaExists = numPairedAntennaExists;
		
		
			
				
		x->numPairedAntenna = numPairedAntenna;
 				
 			
		
	

	
  		
		
		x->numChanExists = numChanExists;
		
		
			
				
		x->numChan = numChan;
 				
 			
		
	

	
  		
		
		x->polarizationTypeExists = polarizationTypeExists;
		
		
			
		x->polarizationType.length(polarizationType.size());
		for (unsigned int i = 0; i < polarizationType.size(); ++i) {
			
				
			x->polarizationType[i] = polarizationType.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		x->channelExists = channelExists;
		
		
			
		x->channel.length(channel.size());
		for (unsigned int i = 0; i < channel.size(); i++) {
			x->channel[i].length(channel.at(i).size());			 		
		}
		
		for (unsigned int i = 0; i < channel.size() ; i++)
			for (unsigned int j = 0; j < channel.at(i).size(); j++)
					
						
				x->channel[i][j] = channel.at(i).at(j);
		 				
			 						
		
			
		
	

	
	
		
	
  	
 		
		
		
		x->antennaId.length(antennaId.size());
		for (unsigned int i = 0; i < antennaId.size(); ++i) {
			
			x->antennaId[i] = antennaId.at(i).toIDLTag();
			
	 	}
	 	 		
  	

	
  	
 		
 		
		x->pairedAntennaIdExists = pairedAntennaIdExists;
		
		
		
		x->pairedAntennaId.length(pairedAntennaId.size());
		for (unsigned int i = 0; i < pairedAntennaId.size(); ++i) {
			
			x->pairedAntennaId[i] = pairedAntennaId.at(i).toIDLTag();
			
	 	}
	 	 		
  	

	
  	
 		
 		
		x->spectralWindowIdExists = spectralWindowIdExists;
		
		
		
		x->spectralWindowId.length(spectralWindowId.size());
		for (unsigned int i = 0; i < spectralWindowId.size(); ++i) {
			
			x->spectralWindowId[i] = spectralWindowId.at(i).toIDLTag();
			
	 	}
	 	 		
  	

	
		
	

	

	

		
		return x;
	
	}
	
	void FlagRow::toIDL(asdmIDL::FlagRowIDL& x) const {
		// Set the x's fields.
	
		
	
  		
		
		
			
		x.flagId = flagId.toIDLTag();
			
		
	

	
  		
		
		
			
		x.startTime = startTime.toIDLArrayTime();
			
		
	

	
  		
		
		
			
		x.endTime = endTime.toIDLArrayTime();
			
		
	

	
  		
		
		
			
				
		x.reason = CORBA::string_dup(reason.c_str());
				
 			
		
	

	
  		
		
		
			
				
		x.numAntenna = numAntenna;
 				
 			
		
	

	
  		
		
		x.numPolarizationTypeExists = numPolarizationTypeExists;
		
		
			
				
		x.numPolarizationType = numPolarizationType;
 				
 			
		
	

	
  		
		
		x.numSpectralWindowExists = numSpectralWindowExists;
		
		
			
				
		x.numSpectralWindow = numSpectralWindow;
 				
 			
		
	

	
  		
		
		x.numPairedAntennaExists = numPairedAntennaExists;
		
		
			
				
		x.numPairedAntenna = numPairedAntenna;
 				
 			
		
	

	
  		
		
		x.numChanExists = numChanExists;
		
		
			
				
		x.numChan = numChan;
 				
 			
		
	

	
  		
		
		x.polarizationTypeExists = polarizationTypeExists;
		
		
			
		x.polarizationType.length(polarizationType.size());
		for (unsigned int i = 0; i < polarizationType.size(); ++i) {
			
				
			x.polarizationType[i] = polarizationType.at(i);
	 			
	 		
	 	}
			
		
	

	
  		
		
		x.channelExists = channelExists;
		
		
			
		x.channel.length(channel.size());
		for (unsigned int i = 0; i < channel.size(); i++) {
			x.channel[i].length(channel.at(i).size());			 		
		}
		
		for (unsigned int i = 0; i < channel.size() ; i++)
			for (unsigned int j = 0; j < channel.at(i).size(); j++)
					
						
				x.channel[i][j] = channel.at(i).at(j);
		 				
			 						
		
			
		
	

	
	
		
	
  	
 		
		
		
		x.antennaId.length(antennaId.size());
		for (unsigned int i = 0; i < antennaId.size(); ++i) {
			
			x.antennaId[i] = antennaId.at(i).toIDLTag();
			
	 	}
	 	 		
  	

	
  	
 		
 		
		x.pairedAntennaIdExists = pairedAntennaIdExists;
		
		
		
		x.pairedAntennaId.length(pairedAntennaId.size());
		for (unsigned int i = 0; i < pairedAntennaId.size(); ++i) {
			
			x.pairedAntennaId[i] = pairedAntennaId.at(i).toIDLTag();
			
	 	}
	 	 		
  	

	
  	
 		
 		
		x.spectralWindowIdExists = spectralWindowIdExists;
		
		
		
		x.spectralWindowId.length(spectralWindowId.size());
		for (unsigned int i = 0; i < spectralWindowId.size(); ++i) {
			
			x.spectralWindowId[i] = spectralWindowId.at(i).toIDLTag();
			
	 	}
	 	 		
  	

	
		
	

	

	

	
	}
#endif
	

#ifndef WITHOUT_ACS
	/**
	 * Fill the values of this row from the IDL struct FlagRowIDL.
	 * @param x The IDL struct containing the values used to fill this row.
	 */
	void FlagRow::setFromIDL (FlagRowIDL x){
		try {
		// Fill the values from x.
	
		
	
		
		
			
		setFlagId(Tag (x.flagId));
			
 		
		
	

	
		
		
			
		setStartTime(ArrayTime (x.startTime));
			
 		
		
	

	
		
		
			
		setEndTime(ArrayTime (x.endTime));
			
 		
		
	

	
		
		
			
		setReason(string (x.reason));
			
 		
		
	

	
		
		
			
		setNumAntenna(x.numAntenna);
  			
 		
		
	

	
		
		numPolarizationTypeExists = x.numPolarizationTypeExists;
		if (x.numPolarizationTypeExists) {
		
		
			
		setNumPolarizationType(x.numPolarizationType);
  			
 		
		
		}
		
	

	
		
		numSpectralWindowExists = x.numSpectralWindowExists;
		if (x.numSpectralWindowExists) {
		
		
			
		setNumSpectralWindow(x.numSpectralWindow);
  			
 		
		
		}
		
	

	
		
		numPairedAntennaExists = x.numPairedAntennaExists;
		if (x.numPairedAntennaExists) {
		
		
			
		setNumPairedAntenna(x.numPairedAntenna);
  			
 		
		
		}
		
	

	
		
		numChanExists = x.numChanExists;
		if (x.numChanExists) {
		
		
			
		setNumChan(x.numChan);
  			
 		
		
		}
		
	

	
		
		polarizationTypeExists = x.polarizationTypeExists;
		if (x.polarizationTypeExists) {
		
		
			
		polarizationType .clear();
		for (unsigned int i = 0; i <x.polarizationType.length(); ++i) {
			
			polarizationType.push_back(x.polarizationType[i]);
  			
		}
			
  		
		
		}
		
	

	
		
		channelExists = x.channelExists;
		if (x.channelExists) {
		
		
			
		channel .clear();
        
        vector<int> v_aux_channel;
        
		for (unsigned int i = 0; i < x.channel.length(); ++i) {
			v_aux_channel.clear();
			for (unsigned int j = 0; j < x.channel[0].length(); ++j) {
				
				v_aux_channel.push_back(x.channel[i][j]);
	  			
  			}
  			channel.push_back(v_aux_channel);			
		}
			
  		
		
		}
		
	

	
	
		
	
		
		antennaId .clear();
		for (unsigned int i = 0; i <x.antennaId.length(); ++i) {
			
			antennaId.push_back(Tag (x.antennaId[i]));
			
		}
		
  	

	
		
		pairedAntennaIdExists = x.pairedAntennaIdExists;
		if (x.pairedAntennaIdExists) {
		
		pairedAntennaId .clear();
		for (unsigned int i = 0; i <x.pairedAntennaId.length(); ++i) {
			
			pairedAntennaId.push_back(Tag (x.pairedAntennaId[i]));
			
		}
		
		}
		
  	

	
		
		spectralWindowIdExists = x.spectralWindowIdExists;
		if (x.spectralWindowIdExists) {
		
		spectralWindowId .clear();
		for (unsigned int i = 0; i <x.spectralWindowId.length(); ++i) {
			
			spectralWindowId.push_back(Tag (x.spectralWindowId[i]));
			
		}
		
		}
		
  	

	
		
	

	

	

		} catch (const IllegalAccessException &err) {
			throw ConversionException (err.getMessage(),"Flag");
		}
	}
#endif
	
	/**
	 * Return this row in the form of an XML string.
	 * @return The values of this row as an XML string.
	 */
	string FlagRow::toXML() const {
		string buf;
		buf.append("<row> \n");
		
	
		
  	
 		
		
		Parser::toXML(flagId, "flagId", buf);
		
		
	

  	
 		
		
		Parser::toXML(startTime, "startTime", buf);
		
		
	

  	
 		
		
		Parser::toXML(endTime, "endTime", buf);
		
		
	

  	
 		
		
		Parser::toXML(reason, "reason", buf);
		
		
	

  	
 		
		
		Parser::toXML(numAntenna, "numAntenna", buf);
		
		
	

  	
 		
		if (numPolarizationTypeExists) {
		
		
		Parser::toXML(numPolarizationType, "numPolarizationType", buf);
		
		
		}
		
	

  	
 		
		if (numSpectralWindowExists) {
		
		
		Parser::toXML(numSpectralWindow, "numSpectralWindow", buf);
		
		
		}
		
	

  	
 		
		if (numPairedAntennaExists) {
		
		
		Parser::toXML(numPairedAntenna, "numPairedAntenna", buf);
		
		
		}
		
	

  	
 		
		if (numChanExists) {
		
		
		Parser::toXML(numChan, "numChan", buf);
		
		
		}
		
	

  	
 		
		if (polarizationTypeExists) {
		
		
			buf.append(EnumerationParser::toXML("polarizationType", polarizationType));
		
		
		}
		
	

  	
 		
		if (channelExists) {
		
		
		Parser::toXML(channel, "channel", buf);
		
		
		}
		
	

	
	
		
  	
 		
		
		Parser::toXML(antennaId, "antennaId", buf);
		
		
	

  	
 		
		if (pairedAntennaIdExists) {
		
		
		Parser::toXML(pairedAntennaId, "pairedAntennaId", buf);
		
		
		}
		
	

  	
 		
		if (spectralWindowIdExists) {
		
		
		Parser::toXML(spectralWindowId, "spectralWindowId", buf);
		
		
		}
		
	

	
		
	

	

	

		
		buf.append("</row>\n");
		return buf;
	}

	/**
	 * Fill the values of this row from an XML string 
	 * that was produced by the toXML() method.
	 * @param x The XML string being used to set the values of this row.
	 */
	void FlagRow::setFromXML (string rowDoc) {
		Parser row(rowDoc);
		string s = "";
		try {
	
		
	
  		
			
	  	setFlagId(Parser::getTag("flagId","Flag",rowDoc));
			
		
	

	
  		
			
	  	setStartTime(Parser::getArrayTime("startTime","Flag",rowDoc));
			
		
	

	
  		
			
	  	setEndTime(Parser::getArrayTime("endTime","Flag",rowDoc));
			
		
	

	
  		
			
	  	setReason(Parser::getString("reason","Flag",rowDoc));
			
		
	

	
  		
			
	  	setNumAntenna(Parser::getInteger("numAntenna","Flag",rowDoc));
			
		
	

	
  		
        if (row.isStr("<numPolarizationType>")) {
			
	  		setNumPolarizationType(Parser::getInteger("numPolarizationType","Flag",rowDoc));
			
		}
 		
	

	
  		
        if (row.isStr("<numSpectralWindow>")) {
			
	  		setNumSpectralWindow(Parser::getInteger("numSpectralWindow","Flag",rowDoc));
			
		}
 		
	

	
  		
        if (row.isStr("<numPairedAntenna>")) {
			
	  		setNumPairedAntenna(Parser::getInteger("numPairedAntenna","Flag",rowDoc));
			
		}
 		
	

	
  		
        if (row.isStr("<numChan>")) {
			
	  		setNumChan(Parser::getInteger("numChan","Flag",rowDoc));
			
		}
 		
	

	
		
	if (row.isStr("<polarizationType>")) {
		
		
		
		polarizationType = EnumerationParser::getPolarizationType1D("polarizationType","Flag",rowDoc);			
		
		
		
		polarizationTypeExists = true;
	}
		
	

	
  		
        if (row.isStr("<channel>")) {
			
								
	  		setChannel(Parser::get2DInteger("channel","Flag",rowDoc));
	  			
	  		
		}
 		
	

	
	
		
	
  		 
  		setAntennaId(Parser::get1DTag("antennaId","Flag",rowDoc));
		
  	

	
  		
  		if (row.isStr("<pairedAntennaId>")) {
  			setPairedAntennaId(Parser::get1DTag("pairedAntennaId","Flag",rowDoc));  		
  		}
  		
  	

	
  		
  		if (row.isStr("<spectralWindowId>")) {
  			setSpectralWindowId(Parser::get1DTag("spectralWindowId","Flag",rowDoc));  		
  		}
  		
  	

	
		
	

	

	

		} catch (const IllegalAccessException &err) {
			throw ConversionException (err.getMessage(),"Flag");
		}
	}
	
	void FlagRow::toBin(EndianOSStream& eoss) {
	
	
	
	
		
	flagId.toBin(eoss);
		
	

	
	
		
	startTime.toBin(eoss);
		
	

	
	
		
	endTime.toBin(eoss);
		
	

	
	
		
						
			eoss.writeString(reason);
				
		
	

	
	
		
						
			eoss.writeInt(numAntenna);
				
		
	

	
	
		
	Tag::toBin(antennaId, eoss);
		
	


	
	
	eoss.writeBoolean(numPolarizationTypeExists);
	if (numPolarizationTypeExists) {
	
	
	
		
						
			eoss.writeInt(numPolarizationType);
				
		
	

	}

	eoss.writeBoolean(numSpectralWindowExists);
	if (numSpectralWindowExists) {
	
	
	
		
						
			eoss.writeInt(numSpectralWindow);
				
		
	

	}

	eoss.writeBoolean(numPairedAntennaExists);
	if (numPairedAntennaExists) {
	
	
	
		
						
			eoss.writeInt(numPairedAntenna);
				
		
	

	}

	eoss.writeBoolean(numChanExists);
	if (numChanExists) {
	
	
	
		
						
			eoss.writeInt(numChan);
				
		
	

	}

	eoss.writeBoolean(polarizationTypeExists);
	if (polarizationTypeExists) {
	
	
	
		
		
			
		eoss.writeInt((int) polarizationType.size());
		for (unsigned int i = 0; i < polarizationType.size(); i++)
				
			eoss.writeString(CPolarizationType::name(polarizationType.at(i)));
			/* eoss.writeInt(polarizationType.at(i)); */
				
				
						
		
	

	}

	eoss.writeBoolean(channelExists);
	if (channelExists) {
	
	
	
		
		
			
		eoss.writeInt((int) channel.size());
		eoss.writeInt((int) channel.at(0).size());
		for (unsigned int i = 0; i < channel.size(); i++) 
			for (unsigned int j = 0;  j < channel.at(0).size(); j++) 
							 
				eoss.writeInt(channel.at(i).at(j));
				
	
						
		
	

	}

	eoss.writeBoolean(pairedAntennaIdExists);
	if (pairedAntennaIdExists) {
	
	
	
		
	Tag::toBin(pairedAntennaId, eoss);
		
	

	}

	eoss.writeBoolean(spectralWindowIdExists);
	if (spectralWindowIdExists) {
	
	
	
		
	Tag::toBin(spectralWindowId, eoss);
		
	

	}

	}
	
void FlagRow::flagIdFromBin(EndianIStream& eis) {
		
	
		
		
		flagId =  Tag::fromBin(eis);
		
	
	
}
void FlagRow::startTimeFromBin(EndianIStream& eis) {
		
	
		
		
		startTime =  ArrayTime::fromBin(eis);
		
	
	
}
void FlagRow::endTimeFromBin(EndianIStream& eis) {
		
	
		
		
		endTime =  ArrayTime::fromBin(eis);
		
	
	
}
void FlagRow::reasonFromBin(EndianIStream& eis) {
		
	
	
		
			
		reason =  eis.readString();
			
		
	
	
}
void FlagRow::numAntennaFromBin(EndianIStream& eis) {
		
	
	
		
			
		numAntenna =  eis.readInt();
			
		
	
	
}
void FlagRow::antennaIdFromBin(EndianIStream& eis) {
		
	
		
		
			
	
	antennaId = Tag::from1DBin(eis);	
	

		
	
	
}

void FlagRow::numPolarizationTypeFromBin(EndianIStream& eis) {
		
	numPolarizationTypeExists = eis.readBoolean();
	if (numPolarizationTypeExists) {
		
	
	
		
			
		numPolarizationType =  eis.readInt();
			
		
	

	}
	
}
void FlagRow::numSpectralWindowFromBin(EndianIStream& eis) {
		
	numSpectralWindowExists = eis.readBoolean();
	if (numSpectralWindowExists) {
		
	
	
		
			
		numSpectralWindow =  eis.readInt();
			
		
	

	}
	
}
void FlagRow::numPairedAntennaFromBin(EndianIStream& eis) {
		
	numPairedAntennaExists = eis.readBoolean();
	if (numPairedAntennaExists) {
		
	
	
		
			
		numPairedAntenna =  eis.readInt();
			
		
	

	}
	
}
void FlagRow::numChanFromBin(EndianIStream& eis) {
		
	numChanExists = eis.readBoolean();
	if (numChanExists) {
		
	
	
		
			
		numChan =  eis.readInt();
			
		
	

	}
	
}
void FlagRow::polarizationTypeFromBin(EndianIStream& eis) {
		
	polarizationTypeExists = eis.readBoolean();
	if (polarizationTypeExists) {
		
	
	
		
			
	
		polarizationType.clear();
		
		unsigned int polarizationTypeDim1 = eis.readInt();
		for (unsigned int  i = 0 ; i < polarizationTypeDim1; i++)
			
			polarizationType.push_back(CPolarizationType::literal(eis.readString()));
			
	

		
	

	}
	
}
void FlagRow::channelFromBin(EndianIStream& eis) {
		
	channelExists = eis.readBoolean();
	if (channelExists) {
		
	
	
		
			
	
		channel.clear();
		
		unsigned int channelDim1 = eis.readInt();
		unsigned int channelDim2 = eis.readInt();
        
		vector <int> channelAux1;
        
		for (unsigned int i = 0; i < channelDim1; i++) {
			channelAux1.clear();
			for (unsigned int j = 0; j < channelDim2 ; j++)			
			
			channelAux1.push_back(eis.readInt());
			
			channel.push_back(channelAux1);
		}
	
	

		
	

	}
	
}
void FlagRow::pairedAntennaIdFromBin(EndianIStream& eis) {
		
	pairedAntennaIdExists = eis.readBoolean();
	if (pairedAntennaIdExists) {
		
	
		
		
			
	
	pairedAntennaId = Tag::from1DBin(eis);	
	

		
	

	}
	
}
void FlagRow::spectralWindowIdFromBin(EndianIStream& eis) {
		
	spectralWindowIdExists = eis.readBoolean();
	if (spectralWindowIdExists) {
		
	
		
		
			
	
	spectralWindowId = Tag::from1DBin(eis);	
	

		
	

	}
	
}
	
	
	FlagRow* FlagRow::fromBin(EndianIStream& eis, FlagTable& table, const vector<string>& attributesSeq) {
		FlagRow* row = new  FlagRow(table);
		
		map<string, FlagAttributeFromBin>::iterator iter ;
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
					throw ConversionException("There is not method to read an attribute '"+attributesSeq.at(i)+"'.", "FlagTable");
			}
				
		}				
		return row;
	}

	//
	// A collection of methods to set the value of the attributes from their textual value in the XML representation
	// of one row.
	//
	
	// Convert a string into an Tag 
	void FlagRow::flagIdFromText(const string & s) {
		 
          
		flagId = ASDMValuesParser::parse<Tag>(s);
          
		
	}
	
	
	// Convert a string into an ArrayTime 
	void FlagRow::startTimeFromText(const string & s) {
		 
          
		startTime = ASDMValuesParser::parse<ArrayTime>(s);
          
		
	}
	
	
	// Convert a string into an ArrayTime 
	void FlagRow::endTimeFromText(const string & s) {
		 
          
		endTime = ASDMValuesParser::parse<ArrayTime>(s);
          
		
	}
	
	
	// Convert a string into an String 
	void FlagRow::reasonFromText(const string & s) {
		 
          
		reason = ASDMValuesParser::parse<string>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void FlagRow::numAntennaFromText(const string & s) {
		 
          
		numAntenna = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	// Convert a string into an Tag 
	void FlagRow::antennaIdFromText(const string & s) {
		 
          
		antennaId = ASDMValuesParser::parse1D<Tag>(s);
          
		
	}
	

	
	// Convert a string into an int 
	void FlagRow::numPolarizationTypeFromText(const string & s) {
		numPolarizationTypeExists = true;
		 
          
		numPolarizationType = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void FlagRow::numSpectralWindowFromText(const string & s) {
		numSpectralWindowExists = true;
		 
          
		numSpectralWindow = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void FlagRow::numPairedAntennaFromText(const string & s) {
		numPairedAntennaExists = true;
		 
          
		numPairedAntenna = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void FlagRow::numChanFromText(const string & s) {
		numChanExists = true;
		 
          
		numChan = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	// Convert a string into an PolarizationType 
	void FlagRow::polarizationTypeFromText(const string & s) {
		polarizationTypeExists = true;
		 
          
		polarizationType = ASDMValuesParser::parse1D<PolarizationTypeMod::PolarizationType>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void FlagRow::channelFromText(const string & s) {
		channelExists = true;
		 
          
		channel = ASDMValuesParser::parse2D<int>(s);
          
		
	}
	
	
	// Convert a string into an Tag 
	void FlagRow::pairedAntennaIdFromText(const string & s) {
		pairedAntennaIdExists = true;
		 
          
		pairedAntennaId = ASDMValuesParser::parse1D<Tag>(s);
          
		
	}
	
	
	// Convert a string into an Tag 
	void FlagRow::spectralWindowIdFromText(const string & s) {
		spectralWindowIdExists = true;
		 
          
		spectralWindowId = ASDMValuesParser::parse1D<Tag>(s);
          
		
	}
	
	
	
	void FlagRow::fromText(const std::string& attributeName, const std::string&  t) {
		map<string, FlagAttributeFromText>::iterator iter;
		if ((iter = fromTextMethods.find(attributeName)) == fromTextMethods.end())
			throw ConversionException("I do not know what to do with '"+attributeName+"' and its content '"+t+"' (while parsing an XML document)", "FlagTable");
		(this->*(iter->second))(t);
	}
			
	////////////////////////////////////////////////
	// Intrinsic Table Attributes getters/setters //
	////////////////////////////////////////////////
	
	

	
 	/**
 	 * Get flagId.
 	 * @return flagId as Tag
 	 */
 	Tag FlagRow::getFlagId() const {
	
  		return flagId;
 	}

 	/**
 	 * Set flagId with the specified Tag.
 	 * @param flagId The Tag value to which flagId is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void FlagRow::setFlagId (Tag flagId)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("flagId", "Flag");
		
  		}
  	
 		this->flagId = flagId;
	
 	}
	
	

	

	
 	/**
 	 * Get startTime.
 	 * @return startTime as ArrayTime
 	 */
 	ArrayTime FlagRow::getStartTime() const {
	
  		return startTime;
 	}

 	/**
 	 * Set startTime with the specified ArrayTime.
 	 * @param startTime The ArrayTime value to which startTime is to be set.
 	 
 	
 		
 	 */
 	void FlagRow::setStartTime (ArrayTime startTime)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->startTime = startTime;
	
 	}
	
	

	

	
 	/**
 	 * Get endTime.
 	 * @return endTime as ArrayTime
 	 */
 	ArrayTime FlagRow::getEndTime() const {
	
  		return endTime;
 	}

 	/**
 	 * Set endTime with the specified ArrayTime.
 	 * @param endTime The ArrayTime value to which endTime is to be set.
 	 
 	
 		
 	 */
 	void FlagRow::setEndTime (ArrayTime endTime)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->endTime = endTime;
	
 	}
	
	

	

	
 	/**
 	 * Get reason.
 	 * @return reason as std::string
 	 */
 	std::string FlagRow::getReason() const {
	
  		return reason;
 	}

 	/**
 	 * Set reason with the specified std::string.
 	 * @param reason The std::string value to which reason is to be set.
 	 
 	
 		
 	 */
 	void FlagRow::setReason (std::string reason)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->reason = reason;
	
 	}
	
	

	

	
 	/**
 	 * Get numAntenna.
 	 * @return numAntenna as int
 	 */
 	int FlagRow::getNumAntenna() const {
	
  		return numAntenna;
 	}

 	/**
 	 * Set numAntenna with the specified int.
 	 * @param numAntenna The int value to which numAntenna is to be set.
 	 
 	
 		
 	 */
 	void FlagRow::setNumAntenna (int numAntenna)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->numAntenna = numAntenna;
	
 	}
	
	

	
	/**
	 * The attribute numPolarizationType is optional. Return true if this attribute exists.
	 * @return true if and only if the numPolarizationType attribute exists. 
	 */
	bool FlagRow::isNumPolarizationTypeExists() const {
		return numPolarizationTypeExists;
	}
	

	
 	/**
 	 * Get numPolarizationType, which is optional.
 	 * @return numPolarizationType as int
 	 * @throw IllegalAccessException If numPolarizationType does not exist.
 	 */
 	int FlagRow::getNumPolarizationType() const  {
		if (!numPolarizationTypeExists) {
			throw IllegalAccessException("numPolarizationType", "Flag");
		}
	
  		return numPolarizationType;
 	}

 	/**
 	 * Set numPolarizationType with the specified int.
 	 * @param numPolarizationType The int value to which numPolarizationType is to be set.
 	 
 	
 	 */
 	void FlagRow::setNumPolarizationType (int numPolarizationType) {
	
 		this->numPolarizationType = numPolarizationType;
	
		numPolarizationTypeExists = true;
	
 	}
	
	
	/**
	 * Mark numPolarizationType, which is an optional field, as non-existent.
	 */
	void FlagRow::clearNumPolarizationType () {
		numPolarizationTypeExists = false;
	}
	

	
	/**
	 * The attribute numSpectralWindow is optional. Return true if this attribute exists.
	 * @return true if and only if the numSpectralWindow attribute exists. 
	 */
	bool FlagRow::isNumSpectralWindowExists() const {
		return numSpectralWindowExists;
	}
	

	
 	/**
 	 * Get numSpectralWindow, which is optional.
 	 * @return numSpectralWindow as int
 	 * @throw IllegalAccessException If numSpectralWindow does not exist.
 	 */
 	int FlagRow::getNumSpectralWindow() const  {
		if (!numSpectralWindowExists) {
			throw IllegalAccessException("numSpectralWindow", "Flag");
		}
	
  		return numSpectralWindow;
 	}

 	/**
 	 * Set numSpectralWindow with the specified int.
 	 * @param numSpectralWindow The int value to which numSpectralWindow is to be set.
 	 
 	
 	 */
 	void FlagRow::setNumSpectralWindow (int numSpectralWindow) {
	
 		this->numSpectralWindow = numSpectralWindow;
	
		numSpectralWindowExists = true;
	
 	}
	
	
	/**
	 * Mark numSpectralWindow, which is an optional field, as non-existent.
	 */
	void FlagRow::clearNumSpectralWindow () {
		numSpectralWindowExists = false;
	}
	

	
	/**
	 * The attribute numPairedAntenna is optional. Return true if this attribute exists.
	 * @return true if and only if the numPairedAntenna attribute exists. 
	 */
	bool FlagRow::isNumPairedAntennaExists() const {
		return numPairedAntennaExists;
	}
	

	
 	/**
 	 * Get numPairedAntenna, which is optional.
 	 * @return numPairedAntenna as int
 	 * @throw IllegalAccessException If numPairedAntenna does not exist.
 	 */
 	int FlagRow::getNumPairedAntenna() const  {
		if (!numPairedAntennaExists) {
			throw IllegalAccessException("numPairedAntenna", "Flag");
		}
	
  		return numPairedAntenna;
 	}

 	/**
 	 * Set numPairedAntenna with the specified int.
 	 * @param numPairedAntenna The int value to which numPairedAntenna is to be set.
 	 
 	
 	 */
 	void FlagRow::setNumPairedAntenna (int numPairedAntenna) {
	
 		this->numPairedAntenna = numPairedAntenna;
	
		numPairedAntennaExists = true;
	
 	}
	
	
	/**
	 * Mark numPairedAntenna, which is an optional field, as non-existent.
	 */
	void FlagRow::clearNumPairedAntenna () {
		numPairedAntennaExists = false;
	}
	

	
	/**
	 * The attribute numChan is optional. Return true if this attribute exists.
	 * @return true if and only if the numChan attribute exists. 
	 */
	bool FlagRow::isNumChanExists() const {
		return numChanExists;
	}
	

	
 	/**
 	 * Get numChan, which is optional.
 	 * @return numChan as int
 	 * @throw IllegalAccessException If numChan does not exist.
 	 */
 	int FlagRow::getNumChan() const  {
		if (!numChanExists) {
			throw IllegalAccessException("numChan", "Flag");
		}
	
  		return numChan;
 	}

 	/**
 	 * Set numChan with the specified int.
 	 * @param numChan The int value to which numChan is to be set.
 	 
 	
 	 */
 	void FlagRow::setNumChan (int numChan) {
	
 		this->numChan = numChan;
	
		numChanExists = true;
	
 	}
	
	
	/**
	 * Mark numChan, which is an optional field, as non-existent.
	 */
	void FlagRow::clearNumChan () {
		numChanExists = false;
	}
	

	
	/**
	 * The attribute polarizationType is optional. Return true if this attribute exists.
	 * @return true if and only if the polarizationType attribute exists. 
	 */
	bool FlagRow::isPolarizationTypeExists() const {
		return polarizationTypeExists;
	}
	

	
 	/**
 	 * Get polarizationType, which is optional.
 	 * @return polarizationType as std::vector<PolarizationTypeMod::PolarizationType >
 	 * @throw IllegalAccessException If polarizationType does not exist.
 	 */
 	std::vector<PolarizationTypeMod::PolarizationType > FlagRow::getPolarizationType() const  {
		if (!polarizationTypeExists) {
			throw IllegalAccessException("polarizationType", "Flag");
		}
	
  		return polarizationType;
 	}

 	/**
 	 * Set polarizationType with the specified std::vector<PolarizationTypeMod::PolarizationType >.
 	 * @param polarizationType The std::vector<PolarizationTypeMod::PolarizationType > value to which polarizationType is to be set.
 	 
 	
 	 */
 	void FlagRow::setPolarizationType (std::vector<PolarizationTypeMod::PolarizationType > polarizationType) {
	
 		this->polarizationType = polarizationType;
	
		polarizationTypeExists = true;
	
 	}
	
	
	/**
	 * Mark polarizationType, which is an optional field, as non-existent.
	 */
	void FlagRow::clearPolarizationType () {
		polarizationTypeExists = false;
	}
	

	
	/**
	 * The attribute channel is optional. Return true if this attribute exists.
	 * @return true if and only if the channel attribute exists. 
	 */
	bool FlagRow::isChannelExists() const {
		return channelExists;
	}
	

	
 	/**
 	 * Get channel, which is optional.
 	 * @return channel as std::vector<std::vector<int > >
 	 * @throw IllegalAccessException If channel does not exist.
 	 */
 	std::vector<std::vector<int > > FlagRow::getChannel() const  {
		if (!channelExists) {
			throw IllegalAccessException("channel", "Flag");
		}
	
  		return channel;
 	}

 	/**
 	 * Set channel with the specified std::vector<std::vector<int > >.
 	 * @param channel The std::vector<std::vector<int > > value to which channel is to be set.
 	 
 	
 	 */
 	void FlagRow::setChannel (std::vector<std::vector<int > > channel) {
	
 		this->channel = channel;
	
		channelExists = true;
	
 	}
	
	
	/**
	 * Mark channel, which is an optional field, as non-existent.
	 */
	void FlagRow::clearChannel () {
		channelExists = false;
	}
	

	
	///////////////////////////////////////////////
	// Extrinsic Table Attributes getters/setters//
	///////////////////////////////////////////////
	
	

	
 	/**
 	 * Get antennaId.
 	 * @return antennaId as std::vector<Tag> 
 	 */
 	std::vector<Tag>  FlagRow::getAntennaId() const {
	
  		return antennaId;
 	}

 	/**
 	 * Set antennaId with the specified std::vector<Tag> .
 	 * @param antennaId The std::vector<Tag>  value to which antennaId is to be set.
 	 
 	
 		
 	 */
 	void FlagRow::setAntennaId (std::vector<Tag>  antennaId)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->antennaId = antennaId;
	
 	}
	
	

	
	/**
	 * The attribute pairedAntennaId is optional. Return true if this attribute exists.
	 * @return true if and only if the pairedAntennaId attribute exists. 
	 */
	bool FlagRow::isPairedAntennaIdExists() const {
		return pairedAntennaIdExists;
	}
	

	
 	/**
 	 * Get pairedAntennaId, which is optional.
 	 * @return pairedAntennaId as std::vector<Tag> 
 	 * @throw IllegalAccessException If pairedAntennaId does not exist.
 	 */
 	std::vector<Tag>  FlagRow::getPairedAntennaId() const  {
		if (!pairedAntennaIdExists) {
			throw IllegalAccessException("pairedAntennaId", "Flag");
		}
	
  		return pairedAntennaId;
 	}

 	/**
 	 * Set pairedAntennaId with the specified std::vector<Tag> .
 	 * @param pairedAntennaId The std::vector<Tag>  value to which pairedAntennaId is to be set.
 	 
 	
 	 */
 	void FlagRow::setPairedAntennaId (std::vector<Tag>  pairedAntennaId) {
	
 		this->pairedAntennaId = pairedAntennaId;
	
		pairedAntennaIdExists = true;
	
 	}
	
	
	/**
	 * Mark pairedAntennaId, which is an optional field, as non-existent.
	 */
	void FlagRow::clearPairedAntennaId () {
		pairedAntennaIdExists = false;
	}
	

	
	/**
	 * The attribute spectralWindowId is optional. Return true if this attribute exists.
	 * @return true if and only if the spectralWindowId attribute exists. 
	 */
	bool FlagRow::isSpectralWindowIdExists() const {
		return spectralWindowIdExists;
	}
	

	
 	/**
 	 * Get spectralWindowId, which is optional.
 	 * @return spectralWindowId as std::vector<Tag> 
 	 * @throw IllegalAccessException If spectralWindowId does not exist.
 	 */
 	std::vector<Tag>  FlagRow::getSpectralWindowId() const  {
		if (!spectralWindowIdExists) {
			throw IllegalAccessException("spectralWindowId", "Flag");
		}
	
  		return spectralWindowId;
 	}

 	/**
 	 * Set spectralWindowId with the specified std::vector<Tag> .
 	 * @param spectralWindowId The std::vector<Tag>  value to which spectralWindowId is to be set.
 	 
 	
 	 */
 	void FlagRow::setSpectralWindowId (std::vector<Tag>  spectralWindowId) {
	
 		this->spectralWindowId = spectralWindowId;
	
		spectralWindowIdExists = true;
	
 	}
	
	
	/**
	 * Mark spectralWindowId, which is an optional field, as non-existent.
	 */
	void FlagRow::clearSpectralWindowId () {
		spectralWindowIdExists = false;
	}
	


	//////////////////////////////////////
	// Links Attributes getters/setters //
	//////////////////////////////////////
	
	
 		
 	/**
 	 * Set antennaId[i] with the specified Tag.
 	 * @param i The index in antennaId where to set the Tag value.
 	 * @param antennaId The Tag value to which antennaId[i] is to be set. 
	 		
 	 * @throws IndexOutOfBoundsException
  	 */
  	void FlagRow::setAntennaId (int i, Tag antennaId)  {
  	  	if (hasBeenAdded) {
  	  		
  		}
  		if ((i < 0) || (i > ((int) this->antennaId.size())))
  			throw OutOfBoundsException("Index out of bounds during a set operation on attribute antennaId in table FlagTable");
  		std::vector<Tag> ::iterator iter = this->antennaId.begin();
  		int j = 0;
  		while (j < i) {
  			j++; iter++;
  		}
  		this->antennaId.insert(this->antennaId.erase(iter), antennaId); 
  	}	
 			
	
	
	
		
/**
 * Append a Tag to antennaId.
 * @param id the Tag to be appended to antennaId
 */
 void FlagRow::addAntennaId(Tag id){
 	antennaId.push_back(id);
}

/**
 * Append an array of Tag to antennaId.
 * @param id an array of Tag to be appended to antennaId
 */
 void FlagRow::addAntennaId(const std::vector<Tag> & id) {
 	for (unsigned int i=0; i < id.size(); i++)
 		antennaId.push_back(id.at(i));
 }
 

 /**
  * Returns the Tag stored in antennaId at position i.
  *
  */
 const Tag FlagRow::getAntennaId(int i) {
 	return antennaId.at(i);
 }
 
 /**
  * Returns the AntennaRow linked to this row via the Tag stored in antennaId
  * at position i.
  */
 AntennaRow* FlagRow::getAntennaUsingAntennaId(int i) {
 	return table.getContainer().getAntenna().getRowByKey(antennaId.at(i));
 } 
 
 /**
  * Returns the vector of AntennaRow* linked to this row via the Tags stored in antennaId
  *
  */
 vector<AntennaRow *> FlagRow::getAntennasUsingAntennaId() {
 	vector<AntennaRow *> result;
 	for (unsigned int i = 0; i < antennaId.size(); i++)
 		result.push_back(table.getContainer().getAntenna().getRowByKey(antennaId.at(i)));
 		
 	return result;
 }
  

	

	
 		
 	/**
 	 * Set pairedAntennaId[i] with the specified Tag.
 	 * @param i The index in pairedAntennaId where to set the Tag value.
 	 * @param pairedAntennaId The Tag value to which pairedAntennaId[i] is to be set. 
 	 * @throws OutOfBoundsException
  	 */
  	void FlagRow::setPairedAntennaId (int i, Tag pairedAntennaId) {
  		if ((i < 0) || (i > ((int) this->pairedAntennaId.size())))
  			throw OutOfBoundsException("Index out of bounds during a set operation on attribute pairedAntennaId in table FlagTable");
  		std::vector<Tag> ::iterator iter = this->pairedAntennaId.begin();
  		int j = 0;
  		while (j < i) {
  			j++; iter++;
  		}
  		this->pairedAntennaId.insert(this->pairedAntennaId.erase(iter), pairedAntennaId); 	
  	}
 			
	
	
	
		
/**
 * Append a Tag to pairedAntennaId.
 * @param id the Tag to be appended to pairedAntennaId
 */
 void FlagRow::addPairedAntennaId(Tag id){
 	pairedAntennaId.push_back(id);
}

/**
 * Append an array of Tag to pairedAntennaId.
 * @param id an array of Tag to be appended to pairedAntennaId
 */
 void FlagRow::addPairedAntennaId(const std::vector<Tag> & id) {
 	for (unsigned int i=0; i < id.size(); i++)
 		pairedAntennaId.push_back(id.at(i));
 }
 

 /**
  * Returns the Tag stored in pairedAntennaId at position i.
  *
  */
 const Tag FlagRow::getPairedAntennaId(int i) {
 	return pairedAntennaId.at(i);
 }
 
 /**
  * Returns the AntennaRow linked to this row via the Tag stored in pairedAntennaId
  * at position i.
  */
 AntennaRow* FlagRow::getAntennaUsingPairedAntennaId(int i) {
 	return table.getContainer().getAntenna().getRowByKey(pairedAntennaId.at(i));
 } 
 
 /**
  * Returns the vector of AntennaRow* linked to this row via the Tags stored in pairedAntennaId
  *
  */
 vector<AntennaRow *> FlagRow::getAntennasUsingPairedAntennaId() {
 	vector<AntennaRow *> result;
 	for (unsigned int i = 0; i < pairedAntennaId.size(); i++)
 		result.push_back(table.getContainer().getAntenna().getRowByKey(pairedAntennaId.at(i)));
 		
 	return result;
 }
  

	

	
 		
 	/**
 	 * Set spectralWindowId[i] with the specified Tag.
 	 * @param i The index in spectralWindowId where to set the Tag value.
 	 * @param spectralWindowId The Tag value to which spectralWindowId[i] is to be set. 
 	 * @throws OutOfBoundsException
  	 */
  	void FlagRow::setSpectralWindowId (int i, Tag spectralWindowId) {
  		if ((i < 0) || (i > ((int) this->spectralWindowId.size())))
  			throw OutOfBoundsException("Index out of bounds during a set operation on attribute spectralWindowId in table FlagTable");
  		std::vector<Tag> ::iterator iter = this->spectralWindowId.begin();
  		int j = 0;
  		while (j < i) {
  			j++; iter++;
  		}
  		this->spectralWindowId.insert(this->spectralWindowId.erase(iter), spectralWindowId); 	
  	}
 			
	
	
	
		
/**
 * Append a Tag to spectralWindowId.
 * @param id the Tag to be appended to spectralWindowId
 */
 void FlagRow::addSpectralWindowId(Tag id){
 	spectralWindowId.push_back(id);
}

/**
 * Append an array of Tag to spectralWindowId.
 * @param id an array of Tag to be appended to spectralWindowId
 */
 void FlagRow::addSpectralWindowId(const std::vector<Tag> & id) {
 	for (unsigned int i=0; i < id.size(); i++)
 		spectralWindowId.push_back(id.at(i));
 }
 

 /**
  * Returns the Tag stored in spectralWindowId at position i.
  *
  */
 const Tag FlagRow::getSpectralWindowId(int i) {
 	return spectralWindowId.at(i);
 }
 
 /**
  * Returns the SpectralWindowRow linked to this row via the Tag stored in spectralWindowId
  * at position i.
  */
 SpectralWindowRow* FlagRow::getSpectralWindowUsingSpectralWindowId(int i) {
 	return table.getContainer().getSpectralWindow().getRowByKey(spectralWindowId.at(i));
 } 
 
 /**
  * Returns the vector of SpectralWindowRow* linked to this row via the Tags stored in spectralWindowId
  *
  */
 vector<SpectralWindowRow *> FlagRow::getSpectralWindowsUsingSpectralWindowId() {
 	vector<SpectralWindowRow *> result;
 	for (unsigned int i = 0; i < spectralWindowId.size(); i++)
 		result.push_back(table.getContainer().getSpectralWindow().getRowByKey(spectralWindowId.at(i)));
 		
 	return result;
 }
  

	

	
	/**
	 * Create a FlagRow.
	 * <p>
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 */ 
	FlagRow::FlagRow (FlagTable &t) : table(t) {
		hasBeenAdded = false;
		
	
	

	

	

	

	

	
		numPolarizationTypeExists = false;
	

	
		numSpectralWindowExists = false;
	

	
		numPairedAntennaExists = false;
	

	
		numChanExists = false;
	

	
		polarizationTypeExists = false;
	

	
		channelExists = false;
	

	
	

	
		pairedAntennaIdExists = false;
	

	
		spectralWindowIdExists = false;
	

	
	
	
	

	

	

	

	

	

	

	

	

	

	

	
	
	 fromBinMethods["flagId"] = &FlagRow::flagIdFromBin; 
	 fromBinMethods["startTime"] = &FlagRow::startTimeFromBin; 
	 fromBinMethods["endTime"] = &FlagRow::endTimeFromBin; 
	 fromBinMethods["reason"] = &FlagRow::reasonFromBin; 
	 fromBinMethods["numAntenna"] = &FlagRow::numAntennaFromBin; 
	 fromBinMethods["antennaId"] = &FlagRow::antennaIdFromBin; 
		
	
	 fromBinMethods["numPolarizationType"] = &FlagRow::numPolarizationTypeFromBin; 
	 fromBinMethods["numSpectralWindow"] = &FlagRow::numSpectralWindowFromBin; 
	 fromBinMethods["numPairedAntenna"] = &FlagRow::numPairedAntennaFromBin; 
	 fromBinMethods["numChan"] = &FlagRow::numChanFromBin; 
	 fromBinMethods["polarizationType"] = &FlagRow::polarizationTypeFromBin; 
	 fromBinMethods["channel"] = &FlagRow::channelFromBin; 
	 fromBinMethods["pairedAntennaId"] = &FlagRow::pairedAntennaIdFromBin; 
	 fromBinMethods["spectralWindowId"] = &FlagRow::spectralWindowIdFromBin; 
	
	
	
	
				 
	fromTextMethods["flagId"] = &FlagRow::flagIdFromText;
		 
	
				 
	fromTextMethods["startTime"] = &FlagRow::startTimeFromText;
		 
	
				 
	fromTextMethods["endTime"] = &FlagRow::endTimeFromText;
		 
	
				 
	fromTextMethods["reason"] = &FlagRow::reasonFromText;
		 
	
				 
	fromTextMethods["numAntenna"] = &FlagRow::numAntennaFromText;
		 
	
				 
	fromTextMethods["antennaId"] = &FlagRow::antennaIdFromText;
		 
	

	 
				
	fromTextMethods["numPolarizationType"] = &FlagRow::numPolarizationTypeFromText;
		 	
	 
				
	fromTextMethods["numSpectralWindow"] = &FlagRow::numSpectralWindowFromText;
		 	
	 
				
	fromTextMethods["numPairedAntenna"] = &FlagRow::numPairedAntennaFromText;
		 	
	 
				
	fromTextMethods["numChan"] = &FlagRow::numChanFromText;
		 	
	 
				
	fromTextMethods["polarizationType"] = &FlagRow::polarizationTypeFromText;
		 	
	 
				
	fromTextMethods["channel"] = &FlagRow::channelFromText;
		 	
	 
				
	fromTextMethods["pairedAntennaId"] = &FlagRow::pairedAntennaIdFromText;
		 	
	 
				
	fromTextMethods["spectralWindowId"] = &FlagRow::spectralWindowIdFromText;
		 	
		
	}
	
	FlagRow::FlagRow (FlagTable &t, FlagRow *row) : table(t) {
		hasBeenAdded = false;
		
		if (row == 0) {
	
	
	

	

	

	

	

	
		numPolarizationTypeExists = false;
	

	
		numSpectralWindowExists = false;
	

	
		numPairedAntennaExists = false;
	

	
		numChanExists = false;
	

	
		polarizationTypeExists = false;
	

	
		channelExists = false;
	

	
	

	
		pairedAntennaIdExists = false;
	

	
		spectralWindowIdExists = false;
	
		
		}
		else {
	
		
			flagId = row->flagId;
		
		
		
		
			startTime = row->startTime;
		
			endTime = row->endTime;
		
			reason = row->reason;
		
			numAntenna = row->numAntenna;
		
			antennaId = row->antennaId;
		
		
		
		
		if (row->numPolarizationTypeExists) {
			numPolarizationType = row->numPolarizationType;		
			numPolarizationTypeExists = true;
		}
		else
			numPolarizationTypeExists = false;
		
		if (row->numSpectralWindowExists) {
			numSpectralWindow = row->numSpectralWindow;		
			numSpectralWindowExists = true;
		}
		else
			numSpectralWindowExists = false;
		
		if (row->numPairedAntennaExists) {
			numPairedAntenna = row->numPairedAntenna;		
			numPairedAntennaExists = true;
		}
		else
			numPairedAntennaExists = false;
		
		if (row->numChanExists) {
			numChan = row->numChan;		
			numChanExists = true;
		}
		else
			numChanExists = false;
		
		if (row->polarizationTypeExists) {
			polarizationType = row->polarizationType;		
			polarizationTypeExists = true;
		}
		else
			polarizationTypeExists = false;
		
		if (row->channelExists) {
			channel = row->channel;		
			channelExists = true;
		}
		else
			channelExists = false;
		
		if (row->pairedAntennaIdExists) {
			pairedAntennaId = row->pairedAntennaId;		
			pairedAntennaIdExists = true;
		}
		else
			pairedAntennaIdExists = false;
		
		if (row->spectralWindowIdExists) {
			spectralWindowId = row->spectralWindowId;		
			spectralWindowIdExists = true;
		}
		else
			spectralWindowIdExists = false;
		
		}
		
		 fromBinMethods["flagId"] = &FlagRow::flagIdFromBin; 
		 fromBinMethods["startTime"] = &FlagRow::startTimeFromBin; 
		 fromBinMethods["endTime"] = &FlagRow::endTimeFromBin; 
		 fromBinMethods["reason"] = &FlagRow::reasonFromBin; 
		 fromBinMethods["numAntenna"] = &FlagRow::numAntennaFromBin; 
		 fromBinMethods["antennaId"] = &FlagRow::antennaIdFromBin; 
			
	
		 fromBinMethods["numPolarizationType"] = &FlagRow::numPolarizationTypeFromBin; 
		 fromBinMethods["numSpectralWindow"] = &FlagRow::numSpectralWindowFromBin; 
		 fromBinMethods["numPairedAntenna"] = &FlagRow::numPairedAntennaFromBin; 
		 fromBinMethods["numChan"] = &FlagRow::numChanFromBin; 
		 fromBinMethods["polarizationType"] = &FlagRow::polarizationTypeFromBin; 
		 fromBinMethods["channel"] = &FlagRow::channelFromBin; 
		 fromBinMethods["pairedAntennaId"] = &FlagRow::pairedAntennaIdFromBin; 
		 fromBinMethods["spectralWindowId"] = &FlagRow::spectralWindowIdFromBin; 
			
	}

	
	bool FlagRow::compareNoAutoInc(ArrayTime startTime, ArrayTime endTime, std::string reason, int numAntenna, std::vector<Tag>  antennaId) {
		bool result;
		result = true;
		
	
		
		result = result && (this->startTime == startTime);
		
		if (!result) return false;
	

	
		
		result = result && (this->endTime == endTime);
		
		if (!result) return false;
	

	
		
		result = result && (this->reason == reason);
		
		if (!result) return false;
	

	
		
		result = result && (this->numAntenna == numAntenna);
		
		if (!result) return false;
	

	
		
		result = result && (this->antennaId == antennaId);
		
		if (!result) return false;
	

		return result;
	}	
	
	
	
	bool FlagRow::compareRequiredValue(ArrayTime startTime, ArrayTime endTime, std::string reason, int numAntenna, std::vector<Tag>  antennaId) {
		bool result;
		result = true;
		
	
		if (!(this->startTime == startTime)) return false;
	

	
		if (!(this->endTime == endTime)) return false;
	

	
		if (!(this->reason == reason)) return false;
	

	
		if (!(this->numAntenna == numAntenna)) return false;
	

	
		if (!(this->antennaId == antennaId)) return false;
	

		return result;
	}
	
	
	/**
	 * Return true if all required attributes of the value part are equal to their homologues
	 * in x and false otherwise.
	 *

	 * @param x a pointer on the FlagRow whose required attributes of the value part 

	 * will be compared with those of this.
	 * @return a boolean.
	 */
	bool FlagRow::equalByRequiredValue(FlagRow*  x ) {
		
			
		if (this->startTime != x->startTime) return false;
			
		if (this->endTime != x->endTime) return false;
			
		if (this->reason != x->reason) return false;
			
		if (this->numAntenna != x->numAntenna) return false;
			
		if (this->antennaId != x->antennaId) return false;
			
		
		return true;
	}	
	
/*
	 map<string, FlagAttributeFromBin> FlagRow::initFromBinMethods() {
		map<string, FlagAttributeFromBin> result;
		
		result["flagId"] = &FlagRow::flagIdFromBin;
		result["startTime"] = &FlagRow::startTimeFromBin;
		result["endTime"] = &FlagRow::endTimeFromBin;
		result["reason"] = &FlagRow::reasonFromBin;
		result["numAntenna"] = &FlagRow::numAntennaFromBin;
		result["antennaId"] = &FlagRow::antennaIdFromBin;
		
		
		result["numPolarizationType"] = &FlagRow::numPolarizationTypeFromBin;
		result["numSpectralWindow"] = &FlagRow::numSpectralWindowFromBin;
		result["numPairedAntenna"] = &FlagRow::numPairedAntennaFromBin;
		result["numChan"] = &FlagRow::numChanFromBin;
		result["polarizationType"] = &FlagRow::polarizationTypeFromBin;
		result["channel"] = &FlagRow::channelFromBin;
		result["pairedAntennaId"] = &FlagRow::pairedAntennaIdFromBin;
		result["spectralWindowId"] = &FlagRow::spectralWindowIdFromBin;
			
		
		return result;	
	}
*/	
} // End namespace asdm
 
