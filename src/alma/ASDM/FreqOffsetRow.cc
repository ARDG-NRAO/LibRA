
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
 * File FreqOffsetRow.cpp
 */
 
#include <vector>
#include <set>

#include <alma/ASDM/ASDM.h>
#include <alma/ASDM/FreqOffsetRow.h>
#include <alma/ASDM/FreqOffsetTable.h>

#include <alma/ASDM/AntennaTable.h>
#include <alma/ASDM/AntennaRow.h>

#include <alma/ASDM/SpectralWindowTable.h>
#include <alma/ASDM/SpectralWindowRow.h>

#include <alma/ASDM/FeedTable.h>
#include <alma/ASDM/FeedRow.h>
	

using asdm::ASDM;
using asdm::FreqOffsetRow;
using asdm::FreqOffsetTable;

using asdm::AntennaTable;
using asdm::AntennaRow;

using asdm::SpectralWindowTable;
using asdm::SpectralWindowRow;

using asdm::FeedTable;
using asdm::FeedRow;


#include <alma/ASDM/Parser.h>

#include <alma/ASDM/EnumerationParser.h>
#include <alma/ASDM/ASDMValuesParser.h>
 
#include <alma/ASDM/InvalidArgumentException.h>

using namespace std;

namespace asdm {
	FreqOffsetRow::~FreqOffsetRow() {
	}

	/**
	 * Return the table to which this row belongs.
	 */
	FreqOffsetTable &FreqOffsetRow::getTable() const {
		return table;
	}

	bool FreqOffsetRow::isAdded() const {
		return hasBeenAdded;
	}	

	void FreqOffsetRow::isAdded(bool added) {
		hasBeenAdded = added;
	}
	
#ifndef WITHOUT_ACS
	using asdmIDL::FreqOffsetRowIDL;
#endif
	
#ifndef WITHOUT_ACS
	/**
	 * Return this row in the form of an IDL struct.
	 * @return The values of this row as a FreqOffsetRowIDL struct.
	 */
	FreqOffsetRowIDL *FreqOffsetRow::toIDL() const {
		FreqOffsetRowIDL *x = new FreqOffsetRowIDL ();
		
		// Fill the IDL structure.
	
		
	
  		
		
		
			
		x->timeInterval = timeInterval.toIDLArrayTimeInterval();
			
		
	

	
  		
		
		
			
		x->offset = offset.toIDLFrequency();
			
		
	

	
	
		
	
  	
 		
		
	 	
			
		x->antennaId = antennaId.toIDLTag();
			
	 	 		
  	

	
  	
 		
		
	 	
			
				
		x->feedId = feedId;
 				
 			
	 	 		
  	

	
  	
 		
		
	 	
			
		x->spectralWindowId = spectralWindowId.toIDLTag();
			
	 	 		
  	

	
		
	

	

	

		
		return x;
	
	}
	
	void FreqOffsetRow::toIDL(asdmIDL::FreqOffsetRowIDL& x) const {
		// Set the x's fields.
	
		
	
  		
		
		
			
		x.timeInterval = timeInterval.toIDLArrayTimeInterval();
			
		
	

	
  		
		
		
			
		x.offset = offset.toIDLFrequency();
			
		
	

	
	
		
	
  	
 		
		
	 	
			
		x.antennaId = antennaId.toIDLTag();
			
	 	 		
  	

	
  	
 		
		
	 	
			
				
		x.feedId = feedId;
 				
 			
	 	 		
  	

	
  	
 		
		
	 	
			
		x.spectralWindowId = spectralWindowId.toIDLTag();
			
	 	 		
  	

	
		
	

	

	

	
	}
#endif
	

#ifndef WITHOUT_ACS
	/**
	 * Fill the values of this row from the IDL struct FreqOffsetRowIDL.
	 * @param x The IDL struct containing the values used to fill this row.
	 */
	void FreqOffsetRow::setFromIDL (FreqOffsetRowIDL x){
		try {
		// Fill the values from x.
	
		
	
		
		
			
		setTimeInterval(ArrayTimeInterval (x.timeInterval));
			
 		
		
	

	
		
		
			
		setOffset(Frequency (x.offset));
			
 		
		
	

	
	
		
	
		
		
			
		setAntennaId(Tag (x.antennaId));
			
 		
		
	

	
		
		
			
		setFeedId(x.feedId);
  			
 		
		
	

	
		
		
			
		setSpectralWindowId(Tag (x.spectralWindowId));
			
 		
		
	

	
		
	

	

	

		} catch (const IllegalAccessException &err) {
			throw ConversionException (err.getMessage(),"FreqOffset");
		}
	}
#endif
	
	/**
	 * Return this row in the form of an XML string.
	 * @return The values of this row as an XML string.
	 */
	string FreqOffsetRow::toXML() const {
		string buf;
		buf.append("<row> \n");
		
	
		
  	
 		
		
		Parser::toXML(timeInterval, "timeInterval", buf);
		
		
	

  	
 		
		
		Parser::toXML(offset, "offset", buf);
		
		
	

	
	
		
  	
 		
		
		Parser::toXML(antennaId, "antennaId", buf);
		
		
	

  	
 		
		
		Parser::toXML(feedId, "feedId", buf);
		
		
	

  	
 		
		
		Parser::toXML(spectralWindowId, "spectralWindowId", buf);
		
		
	

	
		
	

	

	

		
		buf.append("</row>\n");
		return buf;
	}

	/**
	 * Fill the values of this row from an XML string 
	 * that was produced by the toXML() method.
	 * @param x The XML string being used to set the values of this row.
	 */
	void FreqOffsetRow::setFromXML (string rowDoc) {
		Parser row(rowDoc);
		string s = "";
		try {
	
		
	
  		
			
	  	setTimeInterval(Parser::getArrayTimeInterval("timeInterval","FreqOffset",rowDoc));
			
		
	

	
  		
			
	  	setOffset(Parser::getFrequency("offset","FreqOffset",rowDoc));
			
		
	

	
	
		
	
  		
			
	  	setAntennaId(Parser::getTag("antennaId","Antenna",rowDoc));
			
		
	

	
  		
			
	  	setFeedId(Parser::getInteger("feedId","Feed",rowDoc));
			
		
	

	
  		
			
	  	setSpectralWindowId(Parser::getTag("spectralWindowId","SpectralWindow",rowDoc));
			
		
	

	
		
	

	

	

		} catch (const IllegalAccessException &err) {
			throw ConversionException (err.getMessage(),"FreqOffset");
		}
	}
	
	void FreqOffsetRow::toBin(EndianOSStream& eoss) {
	
	
	
	
		
	antennaId.toBin(eoss);
		
	

	
	
		
	spectralWindowId.toBin(eoss);
		
	

	
	
		
	timeInterval.toBin(eoss);
		
	

	
	
		
						
			eoss.writeInt(feedId);
				
		
	

	
	
		
	offset.toBin(eoss);
		
	


	
	
	}
	
void FreqOffsetRow::antennaIdFromBin(EndianIStream& eis) {
		
	
		
		
		antennaId =  Tag::fromBin(eis);
		
	
	
}
void FreqOffsetRow::spectralWindowIdFromBin(EndianIStream& eis) {
		
	
		
		
		spectralWindowId =  Tag::fromBin(eis);
		
	
	
}
void FreqOffsetRow::timeIntervalFromBin(EndianIStream& eis) {
		
	
		
		
		timeInterval =  ArrayTimeInterval::fromBin(eis);
		
	
	
}
void FreqOffsetRow::feedIdFromBin(EndianIStream& eis) {
		
	
	
		
			
		feedId =  eis.readInt();
			
		
	
	
}
void FreqOffsetRow::offsetFromBin(EndianIStream& eis) {
		
	
		
		
		offset =  Frequency::fromBin(eis);
		
	
	
}

		
	
	FreqOffsetRow* FreqOffsetRow::fromBin(EndianIStream& eis, FreqOffsetTable& table, const vector<string>& attributesSeq) {
		FreqOffsetRow* row = new  FreqOffsetRow(table);
		
		map<string, FreqOffsetAttributeFromBin>::iterator iter ;
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
					throw ConversionException("There is not method to read an attribute '"+attributesSeq.at(i)+"'.", "FreqOffsetTable");
			}
				
		}				
		return row;
	}

	//
	// A collection of methods to set the value of the attributes from their textual value in the XML representation
	// of one row.
	//
	
	// Convert a string into an Tag 
	void FreqOffsetRow::antennaIdFromText(const string & s) {
		 
          
		antennaId = ASDMValuesParser::parse<Tag>(s);
          
		
	}
	
	
	// Convert a string into an Tag 
	void FreqOffsetRow::spectralWindowIdFromText(const string & s) {
		 
          
		spectralWindowId = ASDMValuesParser::parse<Tag>(s);
          
		
	}
	
	
	// Convert a string into an ArrayTimeInterval 
	void FreqOffsetRow::timeIntervalFromText(const string & s) {
		 
          
		timeInterval = ASDMValuesParser::parse<ArrayTimeInterval>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void FreqOffsetRow::feedIdFromText(const string & s) {
		 
          
		feedId = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	// Convert a string into an Frequency 
	void FreqOffsetRow::offsetFromText(const string & s) {
		 
          
		offset = ASDMValuesParser::parse<Frequency>(s);
          
		
	}
	

		
	
	void FreqOffsetRow::fromText(const std::string& attributeName, const std::string&  t) {
		map<string, FreqOffsetAttributeFromText>::iterator iter;
		if ((iter = fromTextMethods.find(attributeName)) == fromTextMethods.end())
			throw ConversionException("I do not know what to do with '"+attributeName+"' and its content '"+t+"' (while parsing an XML document)", "FreqOffsetTable");
		(this->*(iter->second))(t);
	}
			
	////////////////////////////////////////////////
	// Intrinsic Table Attributes getters/setters //
	////////////////////////////////////////////////
	
	

	
 	/**
 	 * Get timeInterval.
 	 * @return timeInterval as ArrayTimeInterval
 	 */
 	ArrayTimeInterval FreqOffsetRow::getTimeInterval() const {
	
  		return timeInterval;
 	}

 	/**
 	 * Set timeInterval with the specified ArrayTimeInterval.
 	 * @param timeInterval The ArrayTimeInterval value to which timeInterval is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void FreqOffsetRow::setTimeInterval (ArrayTimeInterval timeInterval)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("timeInterval", "FreqOffset");
		
  		}
  	
 		this->timeInterval = timeInterval;
	
 	}
	
	

	

	
 	/**
 	 * Get offset.
 	 * @return offset as Frequency
 	 */
 	Frequency FreqOffsetRow::getOffset() const {
	
  		return offset;
 	}

 	/**
 	 * Set offset with the specified Frequency.
 	 * @param offset The Frequency value to which offset is to be set.
 	 
 	
 		
 	 */
 	void FreqOffsetRow::setOffset (Frequency offset)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->offset = offset;
	
 	}
	
	

	
	///////////////////////////////////////////////
	// Extrinsic Table Attributes getters/setters//
	///////////////////////////////////////////////
	
	

	
 	/**
 	 * Get antennaId.
 	 * @return antennaId as Tag
 	 */
 	Tag FreqOffsetRow::getAntennaId() const {
	
  		return antennaId;
 	}

 	/**
 	 * Set antennaId with the specified Tag.
 	 * @param antennaId The Tag value to which antennaId is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void FreqOffsetRow::setAntennaId (Tag antennaId)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("antennaId", "FreqOffset");
		
  		}
  	
 		this->antennaId = antennaId;
	
 	}
	
	

	

	
 	/**
 	 * Get feedId.
 	 * @return feedId as int
 	 */
 	int FreqOffsetRow::getFeedId() const {
	
  		return feedId;
 	}

 	/**
 	 * Set feedId with the specified int.
 	 * @param feedId The int value to which feedId is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void FreqOffsetRow::setFeedId (int feedId)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("feedId", "FreqOffset");
		
  		}
  	
 		this->feedId = feedId;
	
 	}
	
	

	

	
 	/**
 	 * Get spectralWindowId.
 	 * @return spectralWindowId as Tag
 	 */
 	Tag FreqOffsetRow::getSpectralWindowId() const {
	
  		return spectralWindowId;
 	}

 	/**
 	 * Set spectralWindowId with the specified Tag.
 	 * @param spectralWindowId The Tag value to which spectralWindowId is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void FreqOffsetRow::setSpectralWindowId (Tag spectralWindowId)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("spectralWindowId", "FreqOffset");
		
  		}
  	
 		this->spectralWindowId = spectralWindowId;
	
 	}
	
	


	//////////////////////////////////////
	// Links Attributes getters/setters //
	//////////////////////////////////////
	
	
	
	
		

	/**
	 * Returns the pointer to the row in the Antenna table having Antenna.antennaId == antennaId
	 * @return a AntennaRow*
	 * 
	 
	 */
	 AntennaRow* FreqOffsetRow::getAntennaUsingAntennaId() {
	 
	 	return table.getContainer().getAntenna().getRowByKey(antennaId);
	 }
	 

	

	
	
	
		

	/**
	 * Returns the pointer to the row in the SpectralWindow table having SpectralWindow.spectralWindowId == spectralWindowId
	 * @return a SpectralWindowRow*
	 * 
	 
	 */
	 SpectralWindowRow* FreqOffsetRow::getSpectralWindowUsingSpectralWindowId() {
	 
	 	return table.getContainer().getSpectralWindow().getRowByKey(spectralWindowId);
	 }
	 

	

	
	
	
		

	// ===> Slice link from a row of FreqOffset table to a collection of row of Feed table.
	
	/**
	 * Get the collection of row in the Feed table having their attribut feedId == this->feedId
	 */
	vector <FeedRow *> FreqOffsetRow::getFeeds() {
		
			return table.getContainer().getFeed().getRowByFeedId(feedId);
		
	}
	

	

	
	/**
	 * Create a FreqOffsetRow.
	 * <p>
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 */ 
	FreqOffsetRow::FreqOffsetRow (FreqOffsetTable &t) : table(t) {
		hasBeenAdded = false;
		
	
	

	

	
	

	

	

	
	
	
	

	

	
	
	 fromBinMethods["antennaId"] = &FreqOffsetRow::antennaIdFromBin; 
	 fromBinMethods["spectralWindowId"] = &FreqOffsetRow::spectralWindowIdFromBin; 
	 fromBinMethods["timeInterval"] = &FreqOffsetRow::timeIntervalFromBin; 
	 fromBinMethods["feedId"] = &FreqOffsetRow::feedIdFromBin; 
	 fromBinMethods["offset"] = &FreqOffsetRow::offsetFromBin; 
		
	
	
	
	
	
				 
	fromTextMethods["antennaId"] = &FreqOffsetRow::antennaIdFromText;
		 
	
				 
	fromTextMethods["spectralWindowId"] = &FreqOffsetRow::spectralWindowIdFromText;
		 
	
				 
	fromTextMethods["timeInterval"] = &FreqOffsetRow::timeIntervalFromText;
		 
	
				 
	fromTextMethods["feedId"] = &FreqOffsetRow::feedIdFromText;
		 
	
				 
	fromTextMethods["offset"] = &FreqOffsetRow::offsetFromText;
		 
	

		
	}
	
	FreqOffsetRow::FreqOffsetRow (FreqOffsetTable &t, FreqOffsetRow *row) : table(t) {
		hasBeenAdded = false;
		
		if (row == 0) {
	
	
	

	

	
	

	

	
		
		}
		else {
	
		
			antennaId = row->antennaId;
		
			spectralWindowId = row->spectralWindowId;
		
			timeInterval = row->timeInterval;
		
			feedId = row->feedId;
		
		
		
		
			offset = row->offset;
		
		
		
		
		}
		
		 fromBinMethods["antennaId"] = &FreqOffsetRow::antennaIdFromBin; 
		 fromBinMethods["spectralWindowId"] = &FreqOffsetRow::spectralWindowIdFromBin; 
		 fromBinMethods["timeInterval"] = &FreqOffsetRow::timeIntervalFromBin; 
		 fromBinMethods["feedId"] = &FreqOffsetRow::feedIdFromBin; 
		 fromBinMethods["offset"] = &FreqOffsetRow::offsetFromBin; 
			
	
			
	}

	
	bool FreqOffsetRow::compareNoAutoInc(Tag antennaId, Tag spectralWindowId, ArrayTimeInterval timeInterval, int feedId, Frequency offset) {
		bool result;
		result = true;
		
	
		
		result = result && (this->antennaId == antennaId);
		
		if (!result) return false;
	

	
		
		result = result && (this->spectralWindowId == spectralWindowId);
		
		if (!result) return false;
	

	
		
		result = result && (this->timeInterval.overlaps(timeInterval));
		
		if (!result) return false;
	

	
		
		result = result && (this->feedId == feedId);
		
		if (!result) return false;
	

	
		
		result = result && (this->offset == offset);
		
		if (!result) return false;
	

		return result;
	}	
	
	
	
	bool FreqOffsetRow::compareRequiredValue(Frequency offset) {
		bool result;
		result = true;
		
	
		if (!(this->offset == offset)) return false;
	

		return result;
	}
	
	
	/**
	 * Return true if all required attributes of the value part are equal to their homologues
	 * in x and false otherwise.
	 *

	 * @param x a pointer on the FreqOffsetRow whose required attributes of the value part 

	 * will be compared with those of this.
	 * @return a boolean.
	 */
	bool FreqOffsetRow::equalByRequiredValue(FreqOffsetRow*  x ) {
		
			
		if (this->offset != x->offset) return false;
			
		
		return true;
	}	
	
/*
	 map<string, FreqOffsetAttributeFromBin> FreqOffsetRow::initFromBinMethods() {
		map<string, FreqOffsetAttributeFromBin> result;
		
		result["antennaId"] = &FreqOffsetRow::antennaIdFromBin;
		result["spectralWindowId"] = &FreqOffsetRow::spectralWindowIdFromBin;
		result["timeInterval"] = &FreqOffsetRow::timeIntervalFromBin;
		result["feedId"] = &FreqOffsetRow::feedIdFromBin;
		result["offset"] = &FreqOffsetRow::offsetFromBin;
		
		
			
		
		return result;	
	}
*/	
} // End namespace asdm
 
