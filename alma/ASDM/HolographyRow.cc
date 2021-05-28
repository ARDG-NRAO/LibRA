
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
 * File HolographyRow.cpp
 */
 
#include <vector>
#include <set>

#include <alma/ASDM/ASDM.h>
#include <alma/ASDM/HolographyRow.h>
#include <alma/ASDM/HolographyTable.h>
	

using asdm::ASDM;
using asdm::HolographyRow;
using asdm::HolographyTable;


#include <alma/ASDM/Parser.h>

#include <alma/ASDM/EnumerationParser.h>
#include <alma/ASDM/ASDMValuesParser.h>
 
#include <alma/ASDM/InvalidArgumentException.h>

using namespace std;

namespace asdm {
	HolographyRow::~HolographyRow() {
	}

	/**
	 * Return the table to which this row belongs.
	 */
	HolographyTable &HolographyRow::getTable() const {
		return table;
	}

	bool HolographyRow::isAdded() const {
		return hasBeenAdded;
	}	

	void HolographyRow::isAdded(bool added) {
		hasBeenAdded = added;
	}
	
#ifndef WITHOUT_ACS
	using asdmIDL::HolographyRowIDL;
#endif
	
#ifndef WITHOUT_ACS
	/**
	 * Return this row in the form of an IDL struct.
	 * @return The values of this row as a HolographyRowIDL struct.
	 */
	HolographyRowIDL *HolographyRow::toIDL() const {
		HolographyRowIDL *x = new HolographyRowIDL ();
		
		// Fill the IDL structure.
	
		
	
  		
		
		
			
		x->holographyId = holographyId.toIDLTag();
			
		
	

	
  		
		
		
			
		x->distance = distance.toIDLLength();
			
		
	

	
  		
		
		
			
		x->focus = focus.toIDLLength();
			
		
	

	
  		
		
		
			
				
		x->numCorr = numCorr;
 				
 			
		
	

	
  		
		
		
			
		x->type.length(type.size());
		for (unsigned int i = 0; i < type.size(); ++i) {
			
				
			x->type[i] = type.at(i);
	 			
	 		
	 	}
			
		
	

	
	
		
		
		return x;
	
	}
	
	void HolographyRow::toIDL(asdmIDL::HolographyRowIDL& x) const {
		// Set the x's fields.
	
		
	
  		
		
		
			
		x.holographyId = holographyId.toIDLTag();
			
		
	

	
  		
		
		
			
		x.distance = distance.toIDLLength();
			
		
	

	
  		
		
		
			
		x.focus = focus.toIDLLength();
			
		
	

	
  		
		
		
			
				
		x.numCorr = numCorr;
 				
 			
		
	

	
  		
		
		
			
		x.type.length(type.size());
		for (unsigned int i = 0; i < type.size(); ++i) {
			
				
			x.type[i] = type.at(i);
	 			
	 		
	 	}
			
		
	

	
	
		
	
	}
#endif
	

#ifndef WITHOUT_ACS
	/**
	 * Fill the values of this row from the IDL struct HolographyRowIDL.
	 * @param x The IDL struct containing the values used to fill this row.
	 */
	void HolographyRow::setFromIDL (HolographyRowIDL x){
		try {
		// Fill the values from x.
	
		
	
		
		
			
		setHolographyId(Tag (x.holographyId));
			
 		
		
	

	
		
		
			
		setDistance(Length (x.distance));
			
 		
		
	

	
		
		
			
		setFocus(Length (x.focus));
			
 		
		
	

	
		
		
			
		setNumCorr(x.numCorr);
  			
 		
		
	

	
		
		
			
		type .clear();
		for (unsigned int i = 0; i <x.type.length(); ++i) {
			
			type.push_back(x.type[i]);
  			
		}
			
  		
		
	

	
	
		
		} catch (const IllegalAccessException &err) {
			throw ConversionException (err.getMessage(),"Holography");
		}
	}
#endif
	
	/**
	 * Return this row in the form of an XML string.
	 * @return The values of this row as an XML string.
	 */
	string HolographyRow::toXML() const {
		string buf;
		buf.append("<row> \n");
		
	
		
  	
 		
		
		Parser::toXML(holographyId, "holographyId", buf);
		
		
	

  	
 		
		
		Parser::toXML(distance, "distance", buf);
		
		
	

  	
 		
		
		Parser::toXML(focus, "focus", buf);
		
		
	

  	
 		
		
		Parser::toXML(numCorr, "numCorr", buf);
		
		
	

  	
 		
		
			buf.append(EnumerationParser::toXML("type", type));
		
		
	

	
	
		
		
		buf.append("</row>\n");
		return buf;
	}

	/**
	 * Fill the values of this row from an XML string 
	 * that was produced by the toXML() method.
	 * @param x The XML string being used to set the values of this row.
	 */
	void HolographyRow::setFromXML (string rowDoc) {
		Parser row(rowDoc);
		string s = "";
		try {
	
		
	
  		
			
	  	setHolographyId(Parser::getTag("holographyId","Holography",rowDoc));
			
		
	

	
  		
			
	  	setDistance(Parser::getLength("distance","Holography",rowDoc));
			
		
	

	
  		
			
	  	setFocus(Parser::getLength("focus","Holography",rowDoc));
			
		
	

	
  		
			
	  	setNumCorr(Parser::getInteger("numCorr","Holography",rowDoc));
			
		
	

	
		
		
		
		type = EnumerationParser::getHolographyChannelType1D("type","Holography",rowDoc);			
		
		
		
	

	
	
		
		} catch (const IllegalAccessException &err) {
			throw ConversionException (err.getMessage(),"Holography");
		}
	}
	
	void HolographyRow::toBin(EndianOSStream& eoss) {
	
	
	
	
		
	holographyId.toBin(eoss);
		
	

	
	
		
	distance.toBin(eoss);
		
	

	
	
		
	focus.toBin(eoss);
		
	

	
	
		
						
			eoss.writeInt(numCorr);
				
		
	

	
	
		
		
			
		eoss.writeInt((int) type.size());
		for (unsigned int i = 0; i < type.size(); i++)
				
			eoss.writeString(CHolographyChannelType::name(type.at(i)));
			/* eoss.writeInt(type.at(i)); */
				
				
						
		
	


	
	
	}
	
void HolographyRow::holographyIdFromBin(EndianIStream& eis) {
		
	
		
		
		holographyId =  Tag::fromBin(eis);
		
	
	
}
void HolographyRow::distanceFromBin(EndianIStream& eis) {
		
	
		
		
		distance =  Length::fromBin(eis);
		
	
	
}
void HolographyRow::focusFromBin(EndianIStream& eis) {
		
	
		
		
		focus =  Length::fromBin(eis);
		
	
	
}
void HolographyRow::numCorrFromBin(EndianIStream& eis) {
		
	
	
		
			
		numCorr =  eis.readInt();
			
		
	
	
}
void HolographyRow::typeFromBin(EndianIStream& eis) {
		
	
	
		
			
	
		type.clear();
		
		unsigned int typeDim1 = eis.readInt();
		for (unsigned int  i = 0 ; i < typeDim1; i++)
			
			type.push_back(CHolographyChannelType::literal(eis.readString()));
			
	

		
	
	
}

		
	
	HolographyRow* HolographyRow::fromBin(EndianIStream& eis, HolographyTable& table, const vector<string>& attributesSeq) {
		HolographyRow* row = new  HolographyRow(table);
		
		map<string, HolographyAttributeFromBin>::iterator iter ;
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
					throw ConversionException("There is not method to read an attribute '"+attributesSeq.at(i)+"'.", "HolographyTable");
			}
				
		}				
		return row;
	}

	//
	// A collection of methods to set the value of the attributes from their textual value in the XML representation
	// of one row.
	//
	
	// Convert a string into an Tag 
	void HolographyRow::holographyIdFromText(const string & s) {
		 
          
		holographyId = ASDMValuesParser::parse<Tag>(s);
          
		
	}
	
	
	// Convert a string into an Length 
	void HolographyRow::distanceFromText(const string & s) {
		 
          
		distance = ASDMValuesParser::parse<Length>(s);
          
		
	}
	
	
	// Convert a string into an Length 
	void HolographyRow::focusFromText(const string & s) {
		 
          
		focus = ASDMValuesParser::parse<Length>(s);
          
		
	}
	
	
	// Convert a string into an int 
	void HolographyRow::numCorrFromText(const string & s) {
		 
          
		numCorr = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	// Convert a string into an HolographyChannelType 
	void HolographyRow::typeFromText(const string & s) {
		 
          
		type = ASDMValuesParser::parse1D<HolographyChannelTypeMod::HolographyChannelType>(s);
          
		
	}
	

		
	
	void HolographyRow::fromText(const std::string& attributeName, const std::string&  t) {
		map<string, HolographyAttributeFromText>::iterator iter;
		if ((iter = fromTextMethods.find(attributeName)) == fromTextMethods.end())
			throw ConversionException("I do not know what to do with '"+attributeName+"' and its content '"+t+"' (while parsing an XML document)", "HolographyTable");
		(this->*(iter->second))(t);
	}
			
	////////////////////////////////////////////////
	// Intrinsic Table Attributes getters/setters //
	////////////////////////////////////////////////
	
	

	
 	/**
 	 * Get holographyId.
 	 * @return holographyId as Tag
 	 */
 	Tag HolographyRow::getHolographyId() const {
	
  		return holographyId;
 	}

 	/**
 	 * Set holographyId with the specified Tag.
 	 * @param holographyId The Tag value to which holographyId is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void HolographyRow::setHolographyId (Tag holographyId)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("holographyId", "Holography");
		
  		}
  	
 		this->holographyId = holographyId;
	
 	}
	
	

	

	
 	/**
 	 * Get distance.
 	 * @return distance as Length
 	 */
 	Length HolographyRow::getDistance() const {
	
  		return distance;
 	}

 	/**
 	 * Set distance with the specified Length.
 	 * @param distance The Length value to which distance is to be set.
 	 
 	
 		
 	 */
 	void HolographyRow::setDistance (Length distance)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->distance = distance;
	
 	}
	
	

	

	
 	/**
 	 * Get focus.
 	 * @return focus as Length
 	 */
 	Length HolographyRow::getFocus() const {
	
  		return focus;
 	}

 	/**
 	 * Set focus with the specified Length.
 	 * @param focus The Length value to which focus is to be set.
 	 
 	
 		
 	 */
 	void HolographyRow::setFocus (Length focus)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->focus = focus;
	
 	}
	
	

	

	
 	/**
 	 * Get numCorr.
 	 * @return numCorr as int
 	 */
 	int HolographyRow::getNumCorr() const {
	
  		return numCorr;
 	}

 	/**
 	 * Set numCorr with the specified int.
 	 * @param numCorr The int value to which numCorr is to be set.
 	 
 	
 		
 	 */
 	void HolographyRow::setNumCorr (int numCorr)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->numCorr = numCorr;
	
 	}
	
	

	

	
 	/**
 	 * Get type.
 	 * @return type as std::vector<HolographyChannelTypeMod::HolographyChannelType >
 	 */
 	std::vector<HolographyChannelTypeMod::HolographyChannelType > HolographyRow::getType() const {
	
  		return type;
 	}

 	/**
 	 * Set type with the specified std::vector<HolographyChannelTypeMod::HolographyChannelType >.
 	 * @param type The std::vector<HolographyChannelTypeMod::HolographyChannelType > value to which type is to be set.
 	 
 	
 		
 	 */
 	void HolographyRow::setType (std::vector<HolographyChannelTypeMod::HolographyChannelType > type)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->type = type;
	
 	}
	
	

	
	///////////////////////////////////////////////
	// Extrinsic Table Attributes getters/setters//
	///////////////////////////////////////////////
	

	//////////////////////////////////////
	// Links Attributes getters/setters //
	//////////////////////////////////////
	
	
	/**
	 * Create a HolographyRow.
	 * <p>
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 */ 
	HolographyRow::HolographyRow (HolographyTable &t) : table(t) {
		hasBeenAdded = false;
		
	
	

	

	

	

	

	
	
	
	
	

	

	

	

	

	
	
	 fromBinMethods["holographyId"] = &HolographyRow::holographyIdFromBin; 
	 fromBinMethods["distance"] = &HolographyRow::distanceFromBin; 
	 fromBinMethods["focus"] = &HolographyRow::focusFromBin; 
	 fromBinMethods["numCorr"] = &HolographyRow::numCorrFromBin; 
	 fromBinMethods["type"] = &HolographyRow::typeFromBin; 
		
	
	
	
	
	
				 
	fromTextMethods["holographyId"] = &HolographyRow::holographyIdFromText;
		 
	
				 
	fromTextMethods["distance"] = &HolographyRow::distanceFromText;
		 
	
				 
	fromTextMethods["focus"] = &HolographyRow::focusFromText;
		 
	
				 
	fromTextMethods["numCorr"] = &HolographyRow::numCorrFromText;
		 
	
				 
	fromTextMethods["type"] = &HolographyRow::typeFromText;
		 
	

		
	}
	
	HolographyRow::HolographyRow (HolographyTable &t, HolographyRow *row) : table(t) {
		hasBeenAdded = false;
		
		if (row == 0) {
	
	
	

	

	

	

	

			
		}
		else {
	
		
			holographyId = row->holographyId;
		
		
		
		
			distance = row->distance;
		
			focus = row->focus;
		
			numCorr = row->numCorr;
		
			type = row->type;
		
		
		
		
		}
		
		 fromBinMethods["holographyId"] = &HolographyRow::holographyIdFromBin; 
		 fromBinMethods["distance"] = &HolographyRow::distanceFromBin; 
		 fromBinMethods["focus"] = &HolographyRow::focusFromBin; 
		 fromBinMethods["numCorr"] = &HolographyRow::numCorrFromBin; 
		 fromBinMethods["type"] = &HolographyRow::typeFromBin; 
			
	
			
	}

	
	bool HolographyRow::compareNoAutoInc(Length distance, Length focus, int numCorr, std::vector<HolographyChannelTypeMod::HolographyChannelType > type) {
		bool result;
		result = true;
		
	
		
		result = result && (this->distance == distance);
		
		if (!result) return false;
	

	
		
		result = result && (this->focus == focus);
		
		if (!result) return false;
	

	
		
		result = result && (this->numCorr == numCorr);
		
		if (!result) return false;
	

	
		
		result = result && (this->type == type);
		
		if (!result) return false;
	

		return result;
	}	
	
	
	
	bool HolographyRow::compareRequiredValue(Length distance, Length focus, int numCorr, std::vector<HolographyChannelTypeMod::HolographyChannelType > type) {
		bool result;
		result = true;
		
	
		if (!(this->distance == distance)) return false;
	

	
		if (!(this->focus == focus)) return false;
	

	
		if (!(this->numCorr == numCorr)) return false;
	

	
		if (!(this->type == type)) return false;
	

		return result;
	}
	
	
	/**
	 * Return true if all required attributes of the value part are equal to their homologues
	 * in x and false otherwise.
	 *

	 * @param x a pointer on the HolographyRow whose required attributes of the value part 

	 * will be compared with those of this.
	 * @return a boolean.
	 */
	bool HolographyRow::equalByRequiredValue(HolographyRow*  x ) {
		
			
		if (this->distance != x->distance) return false;
			
		if (this->focus != x->focus) return false;
			
		if (this->numCorr != x->numCorr) return false;
			
		if (this->type != x->type) return false;
			
		
		return true;
	}	
	
/*
	 map<string, HolographyAttributeFromBin> HolographyRow::initFromBinMethods() {
		map<string, HolographyAttributeFromBin> result;
		
		result["holographyId"] = &HolographyRow::holographyIdFromBin;
		result["distance"] = &HolographyRow::distanceFromBin;
		result["focus"] = &HolographyRow::focusFromBin;
		result["numCorr"] = &HolographyRow::numCorrFromBin;
		result["type"] = &HolographyRow::typeFromBin;
		
		
			
		
		return result;	
	}
*/	
} // End namespace asdm
 
