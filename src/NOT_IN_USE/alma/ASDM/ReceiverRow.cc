
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
 * File ReceiverRow.cpp
 */
 
#include <vector>
#include <set>

#include <alma/ASDM/ASDM.h>
#include <alma/ASDM/ReceiverRow.h>
#include <alma/ASDM/ReceiverTable.h>

#include <alma/ASDM/SpectralWindowTable.h>
#include <alma/ASDM/SpectralWindowRow.h>
	

using asdm::ASDM;
using asdm::ReceiverRow;
using asdm::ReceiverTable;

using asdm::SpectralWindowTable;
using asdm::SpectralWindowRow;


#include <alma/ASDM/Parser.h>

#include <alma/ASDM/EnumerationParser.h>
#include <alma/ASDM/ASDMValuesParser.h>
 
#include <alma/ASDM/InvalidArgumentException.h>

using namespace std;

namespace asdm {
	ReceiverRow::~ReceiverRow() {
	}

	/**
	 * Return the table to which this row belongs.
	 */
	ReceiverTable &ReceiverRow::getTable() const {
		return table;
	}

	bool ReceiverRow::isAdded() const {
		return hasBeenAdded;
	}	

	void ReceiverRow::isAdded(bool added) {
		hasBeenAdded = added;
	}
	
#ifndef WITHOUT_ACS
	using asdmIDL::ReceiverRowIDL;
#endif
	
#ifndef WITHOUT_ACS
	/**
	 * Return this row in the form of an IDL struct.
	 * @return The values of this row as a ReceiverRowIDL struct.
	 */
	ReceiverRowIDL *ReceiverRow::toIDL() const {
		ReceiverRowIDL *x = new ReceiverRowIDL ();
		
		// Fill the IDL structure.
	
		
	
  		
		
		
			
				
		x->receiverId = receiverId;
 				
 			
		
	

	
  		
		
		
			
		x->timeInterval = timeInterval.toIDLArrayTimeInterval();
			
		
	

	
  		
		
		
			
				
		x->name = CORBA::string_dup(name.c_str());
				
 			
		
	

	
  		
		
		
			
				
		x->numLO = numLO;
 				
 			
		
	

	
  		
		
		
			
				
		x->frequencyBand = frequencyBand;
 				
 			
		
	

	
  		
		
		
			
		x->freqLO.length(freqLO.size());
		for (unsigned int i = 0; i < freqLO.size(); ++i) {
			
			x->freqLO[i] = freqLO.at(i).toIDLFrequency();
			
	 	}
			
		
	

	
  		
		
		
			
				
		x->receiverSideband = receiverSideband;
 				
 			
		
	

	
  		
		
		
			
		x->sidebandLO.length(sidebandLO.size());
		for (unsigned int i = 0; i < sidebandLO.size(); ++i) {
			
				
			x->sidebandLO[i] = sidebandLO.at(i);
	 			
	 		
	 	}
			
		
	

	
	
		
	
  	
 		
		
	 	
			
		x->spectralWindowId = spectralWindowId.toIDLTag();
			
	 	 		
  	

	
		
	

		
		return x;
	
	}
	
	void ReceiverRow::toIDL(asdmIDL::ReceiverRowIDL& x) const {
		// Set the x's fields.
	
		
	
  		
		
		
			
				
		x.receiverId = receiverId;
 				
 			
		
	

	
  		
		
		
			
		x.timeInterval = timeInterval.toIDLArrayTimeInterval();
			
		
	

	
  		
		
		
			
				
		x.name = CORBA::string_dup(name.c_str());
				
 			
		
	

	
  		
		
		
			
				
		x.numLO = numLO;
 				
 			
		
	

	
  		
		
		
			
				
		x.frequencyBand = frequencyBand;
 				
 			
		
	

	
  		
		
		
			
		x.freqLO.length(freqLO.size());
		for (unsigned int i = 0; i < freqLO.size(); ++i) {
			
			x.freqLO[i] = freqLO.at(i).toIDLFrequency();
			
	 	}
			
		
	

	
  		
		
		
			
				
		x.receiverSideband = receiverSideband;
 				
 			
		
	

	
  		
		
		
			
		x.sidebandLO.length(sidebandLO.size());
		for (unsigned int i = 0; i < sidebandLO.size(); ++i) {
			
				
			x.sidebandLO[i] = sidebandLO.at(i);
	 			
	 		
	 	}
			
		
	

	
	
		
	
  	
 		
		
	 	
			
		x.spectralWindowId = spectralWindowId.toIDLTag();
			
	 	 		
  	

	
		
	

	
	}
#endif
	

#ifndef WITHOUT_ACS
	/**
	 * Fill the values of this row from the IDL struct ReceiverRowIDL.
	 * @param x The IDL struct containing the values used to fill this row.
	 */
	void ReceiverRow::setFromIDL (ReceiverRowIDL x){
		try {
		// Fill the values from x.
	
		
	
		
		
			
		setReceiverId(x.receiverId);
  			
 		
		
	

	
		
		
			
		setTimeInterval(ArrayTimeInterval (x.timeInterval));
			
 		
		
	

	
		
		
			
		setName(string (x.name));
			
 		
		
	

	
		
		
			
		setNumLO(x.numLO);
  			
 		
		
	

	
		
		
			
		setFrequencyBand(x.frequencyBand);
  			
 		
		
	

	
		
		
			
		freqLO .clear();
		for (unsigned int i = 0; i <x.freqLO.length(); ++i) {
			
			freqLO.push_back(Frequency (x.freqLO[i]));
			
		}
			
  		
		
	

	
		
		
			
		setReceiverSideband(x.receiverSideband);
  			
 		
		
	

	
		
		
			
		sidebandLO .clear();
		for (unsigned int i = 0; i <x.sidebandLO.length(); ++i) {
			
			sidebandLO.push_back(x.sidebandLO[i]);
  			
		}
			
  		
		
	

	
	
		
	
		
		
			
		setSpectralWindowId(Tag (x.spectralWindowId));
			
 		
		
	

	
		
	

		} catch (const IllegalAccessException &err) {
			throw ConversionException (err.getMessage(),"Receiver");
		}
	}
#endif
	
	/**
	 * Return this row in the form of an XML string.
	 * @return The values of this row as an XML string.
	 */
	string ReceiverRow::toXML() const {
		string buf;
		buf.append("<row> \n");
		
	
		
  	
 		
		
		Parser::toXML(receiverId, "receiverId", buf);
		
		
	

  	
 		
		
		Parser::toXML(timeInterval, "timeInterval", buf);
		
		
	

  	
 		
		
		Parser::toXML(name, "name", buf);
		
		
	

  	
 		
		
		Parser::toXML(numLO, "numLO", buf);
		
		
	

  	
 		
		
			buf.append(EnumerationParser::toXML("frequencyBand", frequencyBand));
		
		
	

  	
 		
		
		Parser::toXML(freqLO, "freqLO", buf);
		
		
	

  	
 		
		
			buf.append(EnumerationParser::toXML("receiverSideband", receiverSideband));
		
		
	

  	
 		
		
			buf.append(EnumerationParser::toXML("sidebandLO", sidebandLO));
		
		
	

	
	
		
  	
 		
		
		Parser::toXML(spectralWindowId, "spectralWindowId", buf);
		
		
	

	
		
	

		
		buf.append("</row>\n");
		return buf;
	}

	/**
	 * Fill the values of this row from an XML string 
	 * that was produced by the toXML() method.
	 * @param x The XML string being used to set the values of this row.
	 */
	void ReceiverRow::setFromXML (string rowDoc) {
		Parser row(rowDoc);
		string s = "";
		try {
	
		
	
  		
			
	  	setReceiverId(Parser::getInteger("receiverId","Receiver",rowDoc));
			
		
	

	
  		
			
	  	setTimeInterval(Parser::getArrayTimeInterval("timeInterval","Receiver",rowDoc));
			
		
	

	
  		
			
	  	setName(Parser::getString("name","Receiver",rowDoc));
			
		
	

	
  		
			
	  	setNumLO(Parser::getInteger("numLO","Receiver",rowDoc));
			
		
	

	
		
		
		
		frequencyBand = EnumerationParser::getReceiverBand("frequencyBand","Receiver",rowDoc);
		
		
		
	

	
  		
			
					
	  	setFreqLO(Parser::get1DFrequency("freqLO","Receiver",rowDoc));
	  			
	  		
		
	

	
		
		
		
		receiverSideband = EnumerationParser::getReceiverSideband("receiverSideband","Receiver",rowDoc);
		
		
		
	

	
		
		
		
		sidebandLO = EnumerationParser::getNetSideband1D("sidebandLO","Receiver",rowDoc);			
		
		
		
	

	
	
		
	
  		
			
	  	setSpectralWindowId(Parser::getTag("spectralWindowId","SpectralWindow",rowDoc));
			
		
	

	
		
	

		} catch (const IllegalAccessException &err) {
			throw ConversionException (err.getMessage(),"Receiver");
		}
	}
	
	void ReceiverRow::toBin(EndianOSStream& eoss) {
	
	
	
	
		
						
			eoss.writeInt(receiverId);
				
		
	

	
	
		
	spectralWindowId.toBin(eoss);
		
	

	
	
		
	timeInterval.toBin(eoss);
		
	

	
	
		
						
			eoss.writeString(name);
				
		
	

	
	
		
						
			eoss.writeInt(numLO);
				
		
	

	
	
		
					
		eoss.writeString(CReceiverBand::name(frequencyBand));
			/* eoss.writeInt(frequencyBand); */
				
		
	

	
	
		
	Frequency::toBin(freqLO, eoss);
		
	

	
	
		
					
		eoss.writeString(CReceiverSideband::name(receiverSideband));
			/* eoss.writeInt(receiverSideband); */
				
		
	

	
	
		
		
			
		eoss.writeInt((int) sidebandLO.size());
		for (unsigned int i = 0; i < sidebandLO.size(); i++)
				
			eoss.writeString(CNetSideband::name(sidebandLO.at(i)));
			/* eoss.writeInt(sidebandLO.at(i)); */
				
				
						
		
	


	
	
	}
	
void ReceiverRow::receiverIdFromBin(EndianIStream& eis) {
		
	
	
		
			
		receiverId =  eis.readInt();
			
		
	
	
}
void ReceiverRow::spectralWindowIdFromBin(EndianIStream& eis) {
		
	
		
		
		spectralWindowId =  Tag::fromBin(eis);
		
	
	
}
void ReceiverRow::timeIntervalFromBin(EndianIStream& eis) {
		
	
		
		
		timeInterval =  ArrayTimeInterval::fromBin(eis);
		
	
	
}
void ReceiverRow::nameFromBin(EndianIStream& eis) {
		
	
	
		
			
		name =  eis.readString();
			
		
	
	
}
void ReceiverRow::numLOFromBin(EndianIStream& eis) {
		
	
	
		
			
		numLO =  eis.readInt();
			
		
	
	
}
void ReceiverRow::frequencyBandFromBin(EndianIStream& eis) {
		
	
	
		
			
		frequencyBand = CReceiverBand::literal(eis.readString());
			
		
	
	
}
void ReceiverRow::freqLOFromBin(EndianIStream& eis) {
		
	
		
		
			
	
	freqLO = Frequency::from1DBin(eis);	
	

		
	
	
}
void ReceiverRow::receiverSidebandFromBin(EndianIStream& eis) {
		
	
	
		
			
		receiverSideband = CReceiverSideband::literal(eis.readString());
			
		
	
	
}
void ReceiverRow::sidebandLOFromBin(EndianIStream& eis) {
		
	
	
		
			
	
		sidebandLO.clear();
		
		unsigned int sidebandLODim1 = eis.readInt();
		for (unsigned int  i = 0 ; i < sidebandLODim1; i++)
			
			sidebandLO.push_back(CNetSideband::literal(eis.readString()));
			
	

		
	
	
}

		
	
	ReceiverRow* ReceiverRow::fromBin(EndianIStream& eis, ReceiverTable& table, const vector<string>& attributesSeq) {
		ReceiverRow* row = new  ReceiverRow(table);
		
		map<string, ReceiverAttributeFromBin>::iterator iter ;
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
					throw ConversionException("There is not method to read an attribute '"+attributesSeq.at(i)+"'.", "ReceiverTable");
			}
				
		}				
		return row;
	}

	//
	// A collection of methods to set the value of the attributes from their textual value in the XML representation
	// of one row.
	//
	
	// Convert a string into an int 
	void ReceiverRow::receiverIdFromText(const string & s) {
		 
          
		receiverId = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	// Convert a string into an Tag 
	void ReceiverRow::spectralWindowIdFromText(const string & s) {
		 
          
		spectralWindowId = ASDMValuesParser::parse<Tag>(s);
          
		
	}
	
	
	// Convert a string into an ArrayTimeInterval 
	void ReceiverRow::timeIntervalFromText(const string & s) {
		 
          
		timeInterval = ASDMValuesParser::parse<ArrayTimeInterval>(s);
          
		
	}
	
	
	// Convert a string into an String 
	void ReceiverRow::nameFromText(const string & s) {
		 
          
		name = ASDMValuesParser::parse<string>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void ReceiverRow::numLOFromText(const string & s) {
		 
          
		numLO = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	// Convert a string into an ReceiverBand 
	void ReceiverRow::frequencyBandFromText(const string & s) {
		 
          
		frequencyBand = ASDMValuesParser::parse<ReceiverBandMod::ReceiverBand>(s);
          
		
	}
	
	
	// Convert a string into an Frequency 
	void ReceiverRow::freqLOFromText(const string & s) {
		 
          
		freqLO = ASDMValuesParser::parse1D<Frequency>(s);
          
		
	}
	
	
	// Convert a string into an ReceiverSideband 
	void ReceiverRow::receiverSidebandFromText(const string & s) {
		 
          
		receiverSideband = ASDMValuesParser::parse<ReceiverSidebandMod::ReceiverSideband>(s);
          
		
	}
	
	
	// Convert a string into an NetSideband 
	void ReceiverRow::sidebandLOFromText(const string & s) {
		 
          
		sidebandLO = ASDMValuesParser::parse1D<NetSidebandMod::NetSideband>(s);
          
		
	}
	

		
	
	void ReceiverRow::fromText(const std::string& attributeName, const std::string&  t) {
		map<string, ReceiverAttributeFromText>::iterator iter;
		if ((iter = fromTextMethods.find(attributeName)) == fromTextMethods.end())
			throw ConversionException("I do not know what to do with '"+attributeName+"' and its content '"+t+"' (while parsing an XML document)", "ReceiverTable");
		(this->*(iter->second))(t);
	}
			
	////////////////////////////////////////////////
	// Intrinsic Table Attributes getters/setters //
	////////////////////////////////////////////////
	
	

	
 	/**
 	 * Get receiverId.
 	 * @return receiverId as int
 	 */
 	int ReceiverRow::getReceiverId() const {
	
  		return receiverId;
 	}

 	/**
 	 * Set receiverId with the specified int.
 	 * @param receiverId The int value to which receiverId is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void ReceiverRow::setReceiverId (int receiverId)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("receiverId", "Receiver");
		
  		}
  	
 		this->receiverId = receiverId;
	
 	}
	
	

	

	
 	/**
 	 * Get timeInterval.
 	 * @return timeInterval as ArrayTimeInterval
 	 */
 	ArrayTimeInterval ReceiverRow::getTimeInterval() const {
	
  		return timeInterval;
 	}

 	/**
 	 * Set timeInterval with the specified ArrayTimeInterval.
 	 * @param timeInterval The ArrayTimeInterval value to which timeInterval is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void ReceiverRow::setTimeInterval (ArrayTimeInterval timeInterval)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("timeInterval", "Receiver");
		
  		}
  	
 		this->timeInterval = timeInterval;
	
 	}
	
	

	

	
 	/**
 	 * Get name.
 	 * @return name as std::string
 	 */
 	std::string ReceiverRow::getName() const {
	
  		return name;
 	}

 	/**
 	 * Set name with the specified std::string.
 	 * @param name The std::string value to which name is to be set.
 	 
 	
 		
 	 */
 	void ReceiverRow::setName (std::string name)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->name = name;
	
 	}
	
	

	

	
 	/**
 	 * Get numLO.
 	 * @return numLO as int
 	 */
 	int ReceiverRow::getNumLO() const {
	
  		return numLO;
 	}

 	/**
 	 * Set numLO with the specified int.
 	 * @param numLO The int value to which numLO is to be set.
 	 
 	
 		
 	 */
 	void ReceiverRow::setNumLO (int numLO)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->numLO = numLO;
	
 	}
	
	

	

	
 	/**
 	 * Get frequencyBand.
 	 * @return frequencyBand as ReceiverBandMod::ReceiverBand
 	 */
 	ReceiverBandMod::ReceiverBand ReceiverRow::getFrequencyBand() const {
	
  		return frequencyBand;
 	}

 	/**
 	 * Set frequencyBand with the specified ReceiverBandMod::ReceiverBand.
 	 * @param frequencyBand The ReceiverBandMod::ReceiverBand value to which frequencyBand is to be set.
 	 
 	
 		
 	 */
 	void ReceiverRow::setFrequencyBand (ReceiverBandMod::ReceiverBand frequencyBand)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->frequencyBand = frequencyBand;
	
 	}
	
	

	

	
 	/**
 	 * Get freqLO.
 	 * @return freqLO as std::vector<Frequency >
 	 */
 	std::vector<Frequency > ReceiverRow::getFreqLO() const {
	
  		return freqLO;
 	}

 	/**
 	 * Set freqLO with the specified std::vector<Frequency >.
 	 * @param freqLO The std::vector<Frequency > value to which freqLO is to be set.
 	 
 	
 		
 	 */
 	void ReceiverRow::setFreqLO (std::vector<Frequency > freqLO)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->freqLO = freqLO;
	
 	}
	
	

	

	
 	/**
 	 * Get receiverSideband.
 	 * @return receiverSideband as ReceiverSidebandMod::ReceiverSideband
 	 */
 	ReceiverSidebandMod::ReceiverSideband ReceiverRow::getReceiverSideband() const {
	
  		return receiverSideband;
 	}

 	/**
 	 * Set receiverSideband with the specified ReceiverSidebandMod::ReceiverSideband.
 	 * @param receiverSideband The ReceiverSidebandMod::ReceiverSideband value to which receiverSideband is to be set.
 	 
 	
 		
 	 */
 	void ReceiverRow::setReceiverSideband (ReceiverSidebandMod::ReceiverSideband receiverSideband)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->receiverSideband = receiverSideband;
	
 	}
	
	

	

	
 	/**
 	 * Get sidebandLO.
 	 * @return sidebandLO as std::vector<NetSidebandMod::NetSideband >
 	 */
 	std::vector<NetSidebandMod::NetSideband > ReceiverRow::getSidebandLO() const {
	
  		return sidebandLO;
 	}

 	/**
 	 * Set sidebandLO with the specified std::vector<NetSidebandMod::NetSideband >.
 	 * @param sidebandLO The std::vector<NetSidebandMod::NetSideband > value to which sidebandLO is to be set.
 	 
 	
 		
 	 */
 	void ReceiverRow::setSidebandLO (std::vector<NetSidebandMod::NetSideband > sidebandLO)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->sidebandLO = sidebandLO;
	
 	}
	
	

	
	///////////////////////////////////////////////
	// Extrinsic Table Attributes getters/setters//
	///////////////////////////////////////////////
	
	

	
 	/**
 	 * Get spectralWindowId.
 	 * @return spectralWindowId as Tag
 	 */
 	Tag ReceiverRow::getSpectralWindowId() const {
	
  		return spectralWindowId;
 	}

 	/**
 	 * Set spectralWindowId with the specified Tag.
 	 * @param spectralWindowId The Tag value to which spectralWindowId is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void ReceiverRow::setSpectralWindowId (Tag spectralWindowId)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("spectralWindowId", "Receiver");
		
  		}
  	
 		this->spectralWindowId = spectralWindowId;
	
 	}
	
	


	//////////////////////////////////////
	// Links Attributes getters/setters //
	//////////////////////////////////////
	
	
	
	
		

	/**
	 * Returns the pointer to the row in the SpectralWindow table having SpectralWindow.spectralWindowId == spectralWindowId
	 * @return a SpectralWindowRow*
	 * 
	 
	 */
	 SpectralWindowRow* ReceiverRow::getSpectralWindowUsingSpectralWindowId() {
	 
	 	return table.getContainer().getSpectralWindow().getRowByKey(spectralWindowId);
	 }
	 

	

	
	/**
	 * Create a ReceiverRow.
	 * <p>
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 */ 
	ReceiverRow::ReceiverRow (ReceiverTable &t) : table(t) {
		hasBeenAdded = false;
		
	
	

	

	

	

	

	

	

	

	
	

	
	
	
	

	

	

	

	
// This attribute is scalar and has an enumeration type. Let's initialize it to some valid value (the 1st of the enumeration).		
frequencyBand = CReceiverBand::from_int(0);
	

	

	
// This attribute is scalar and has an enumeration type. Let's initialize it to some valid value (the 1st of the enumeration).		
receiverSideband = CReceiverSideband::from_int(0);
	

	

	
	
	 fromBinMethods["receiverId"] = &ReceiverRow::receiverIdFromBin; 
	 fromBinMethods["spectralWindowId"] = &ReceiverRow::spectralWindowIdFromBin; 
	 fromBinMethods["timeInterval"] = &ReceiverRow::timeIntervalFromBin; 
	 fromBinMethods["name"] = &ReceiverRow::nameFromBin; 
	 fromBinMethods["numLO"] = &ReceiverRow::numLOFromBin; 
	 fromBinMethods["frequencyBand"] = &ReceiverRow::frequencyBandFromBin; 
	 fromBinMethods["freqLO"] = &ReceiverRow::freqLOFromBin; 
	 fromBinMethods["receiverSideband"] = &ReceiverRow::receiverSidebandFromBin; 
	 fromBinMethods["sidebandLO"] = &ReceiverRow::sidebandLOFromBin; 
		
	
	
	
	
	
				 
	fromTextMethods["receiverId"] = &ReceiverRow::receiverIdFromText;
		 
	
				 
	fromTextMethods["spectralWindowId"] = &ReceiverRow::spectralWindowIdFromText;
		 
	
				 
	fromTextMethods["timeInterval"] = &ReceiverRow::timeIntervalFromText;
		 
	
				 
	fromTextMethods["name"] = &ReceiverRow::nameFromText;
		 
	
				 
	fromTextMethods["numLO"] = &ReceiverRow::numLOFromText;
		 
	
				 
	fromTextMethods["frequencyBand"] = &ReceiverRow::frequencyBandFromText;
		 
	
				 
	fromTextMethods["freqLO"] = &ReceiverRow::freqLOFromText;
		 
	
				 
	fromTextMethods["receiverSideband"] = &ReceiverRow::receiverSidebandFromText;
		 
	
				 
	fromTextMethods["sidebandLO"] = &ReceiverRow::sidebandLOFromText;
		 
	

		
	}
	
	ReceiverRow::ReceiverRow (ReceiverTable &t, ReceiverRow *row) : table(t) {
		hasBeenAdded = false;
		
		if (row == 0) {
	
	
	

	

	

	

	

	

	

	

	
	
		
		}
		else {
	
		
			receiverId = row->receiverId;
		
			spectralWindowId = row->spectralWindowId;
		
			timeInterval = row->timeInterval;
		
		
		
		
			name = row->name;
		
			numLO = row->numLO;
		
			frequencyBand = row->frequencyBand;
		
			freqLO = row->freqLO;
		
			receiverSideband = row->receiverSideband;
		
			sidebandLO = row->sidebandLO;
		
		
		
		
		}
		
		 fromBinMethods["receiverId"] = &ReceiverRow::receiverIdFromBin; 
		 fromBinMethods["spectralWindowId"] = &ReceiverRow::spectralWindowIdFromBin; 
		 fromBinMethods["timeInterval"] = &ReceiverRow::timeIntervalFromBin; 
		 fromBinMethods["name"] = &ReceiverRow::nameFromBin; 
		 fromBinMethods["numLO"] = &ReceiverRow::numLOFromBin; 
		 fromBinMethods["frequencyBand"] = &ReceiverRow::frequencyBandFromBin; 
		 fromBinMethods["freqLO"] = &ReceiverRow::freqLOFromBin; 
		 fromBinMethods["receiverSideband"] = &ReceiverRow::receiverSidebandFromBin; 
		 fromBinMethods["sidebandLO"] = &ReceiverRow::sidebandLOFromBin; 
			
	
			
	}

	
	bool ReceiverRow::compareNoAutoInc(Tag spectralWindowId, ArrayTimeInterval timeInterval, std::string name, int numLO, ReceiverBandMod::ReceiverBand frequencyBand, std::vector<Frequency > freqLO, ReceiverSidebandMod::ReceiverSideband receiverSideband, std::vector<NetSidebandMod::NetSideband > sidebandLO) {
		bool result;
		result = true;
		
	
		
		result = result && (this->spectralWindowId == spectralWindowId);
		
		if (!result) return false;
	

	
		
		result = result && (this->timeInterval.overlaps(timeInterval));
		
		if (!result) return false;
	

	
		
		result = result && (this->name == name);
		
		if (!result) return false;
	

	
		
		result = result && (this->numLO == numLO);
		
		if (!result) return false;
	

	
		
		result = result && (this->frequencyBand == frequencyBand);
		
		if (!result) return false;
	

	
		
		result = result && (this->freqLO == freqLO);
		
		if (!result) return false;
	

	
		
		result = result && (this->receiverSideband == receiverSideband);
		
		if (!result) return false;
	

	
		
		result = result && (this->sidebandLO == sidebandLO);
		
		if (!result) return false;
	

		return result;
	}	
	
	
	
	bool ReceiverRow::compareRequiredValue(std::string name, int numLO, ReceiverBandMod::ReceiverBand frequencyBand, std::vector<Frequency > freqLO, ReceiverSidebandMod::ReceiverSideband receiverSideband, std::vector<NetSidebandMod::NetSideband > sidebandLO) {
		bool result;
		result = true;
		
	
		if (!(this->name == name)) return false;
	

	
		if (!(this->numLO == numLO)) return false;
	

	
		if (!(this->frequencyBand == frequencyBand)) return false;
	

	
		if (!(this->freqLO == freqLO)) return false;
	

	
		if (!(this->receiverSideband == receiverSideband)) return false;
	

	
		if (!(this->sidebandLO == sidebandLO)) return false;
	

		return result;
	}
	
	
	/**
	 * Return true if all required attributes of the value part are equal to their homologues
	 * in x and false otherwise.
	 *

	 * @param x a pointer on the ReceiverRow whose required attributes of the value part 

	 * will be compared with those of this.
	 * @return a boolean.
	 */
	bool ReceiverRow::equalByRequiredValue(ReceiverRow*  x ) {
		
			
		if (this->name != x->name) return false;
			
		if (this->numLO != x->numLO) return false;
			
		if (this->frequencyBand != x->frequencyBand) return false;
			
		if (this->freqLO != x->freqLO) return false;
			
		if (this->receiverSideband != x->receiverSideband) return false;
			
		if (this->sidebandLO != x->sidebandLO) return false;
			
		
		return true;
	}	
	
/*
	 map<string, ReceiverAttributeFromBin> ReceiverRow::initFromBinMethods() {
		map<string, ReceiverAttributeFromBin> result;
		
		result["receiverId"] = &ReceiverRow::receiverIdFromBin;
		result["spectralWindowId"] = &ReceiverRow::spectralWindowIdFromBin;
		result["timeInterval"] = &ReceiverRow::timeIntervalFromBin;
		result["name"] = &ReceiverRow::nameFromBin;
		result["numLO"] = &ReceiverRow::numLOFromBin;
		result["frequencyBand"] = &ReceiverRow::frequencyBandFromBin;
		result["freqLO"] = &ReceiverRow::freqLOFromBin;
		result["receiverSideband"] = &ReceiverRow::receiverSidebandFromBin;
		result["sidebandLO"] = &ReceiverRow::sidebandLOFromBin;
		
		
			
		
		return result;	
	}
*/	
} // End namespace asdm
 
