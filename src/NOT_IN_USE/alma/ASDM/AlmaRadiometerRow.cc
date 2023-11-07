
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
 * File AlmaRadiometerRow.cpp
 */
 
#include <vector>
#include <set>

#include <alma/ASDM/ASDM.h>
#include <alma/ASDM/AlmaRadiometerRow.h>
#include <alma/ASDM/AlmaRadiometerTable.h>

#include <alma/ASDM/SpectralWindowTable.h>
#include <alma/ASDM/SpectralWindowRow.h>
	

using asdm::ASDM;
using asdm::AlmaRadiometerRow;
using asdm::AlmaRadiometerTable;

using asdm::SpectralWindowTable;
using asdm::SpectralWindowRow;


#include <alma/ASDM/Parser.h>

#include <alma/ASDM/EnumerationParser.h>
#include <alma/ASDM/ASDMValuesParser.h>
 
#include <alma/ASDM/InvalidArgumentException.h>

using namespace std;

namespace asdm {
	AlmaRadiometerRow::~AlmaRadiometerRow() {
	}

	/**
	 * Return the table to which this row belongs.
	 */
	AlmaRadiometerTable &AlmaRadiometerRow::getTable() const {
		return table;
	}

	bool AlmaRadiometerRow::isAdded() const {
		return hasBeenAdded;
	}	

	void AlmaRadiometerRow::isAdded(bool added) {
		hasBeenAdded = added;
	}
	
#ifndef WITHOUT_ACS
	using asdmIDL::AlmaRadiometerRowIDL;
#endif
	
#ifndef WITHOUT_ACS
	/**
	 * Return this row in the form of an IDL struct.
	 * @return The values of this row as a AlmaRadiometerRowIDL struct.
	 */
	AlmaRadiometerRowIDL *AlmaRadiometerRow::toIDL() const {
		AlmaRadiometerRowIDL *x = new AlmaRadiometerRowIDL ();
		
		// Fill the IDL structure.
	
		
	
  		
		
		
			
		x->almaRadiometerId = almaRadiometerId.toIDLTag();
			
		
	

	
  		
		
		x->numAntennaExists = numAntennaExists;
		
		
			
				
		x->numAntenna = numAntenna;
 				
 			
		
	

	
	
		
	
  	
 		
 		
		x->spectralWindowIdExists = spectralWindowIdExists;
		
		
		
		x->spectralWindowId.length(spectralWindowId.size());
		for (unsigned int i = 0; i < spectralWindowId.size(); ++i) {
			
			x->spectralWindowId[i] = spectralWindowId.at(i).toIDLTag();
			
	 	}
	 	 		
  	

	
		
	

		
		return x;
	
	}
	
	void AlmaRadiometerRow::toIDL(asdmIDL::AlmaRadiometerRowIDL& x) const {
		// Set the x's fields.
	
		
	
  		
		
		
			
		x.almaRadiometerId = almaRadiometerId.toIDLTag();
			
		
	

	
  		
		
		x.numAntennaExists = numAntennaExists;
		
		
			
				
		x.numAntenna = numAntenna;
 				
 			
		
	

	
	
		
	
  	
 		
 		
		x.spectralWindowIdExists = spectralWindowIdExists;
		
		
		
		x.spectralWindowId.length(spectralWindowId.size());
		for (unsigned int i = 0; i < spectralWindowId.size(); ++i) {
			
			x.spectralWindowId[i] = spectralWindowId.at(i).toIDLTag();
			
	 	}
	 	 		
  	

	
		
	

	
	}
#endif
	

#ifndef WITHOUT_ACS
	/**
	 * Fill the values of this row from the IDL struct AlmaRadiometerRowIDL.
	 * @param x The IDL struct containing the values used to fill this row.
	 */
	void AlmaRadiometerRow::setFromIDL (AlmaRadiometerRowIDL x){
		try {
		// Fill the values from x.
	
		
	
		
		
			
		setAlmaRadiometerId(Tag (x.almaRadiometerId));
			
 		
		
	

	
		
		numAntennaExists = x.numAntennaExists;
		if (x.numAntennaExists) {
		
		
			
		setNumAntenna(x.numAntenna);
  			
 		
		
		}
		
	

	
	
		
	
		
		spectralWindowIdExists = x.spectralWindowIdExists;
		if (x.spectralWindowIdExists) {
		
		spectralWindowId .clear();
		for (unsigned int i = 0; i <x.spectralWindowId.length(); ++i) {
			
			spectralWindowId.push_back(Tag (x.spectralWindowId[i]));
			
		}
		
		}
		
  	

	
		
	

		} catch (const IllegalAccessException &err) {
			throw ConversionException (err.getMessage(),"AlmaRadiometer");
		}
	}
#endif
	
	/**
	 * Return this row in the form of an XML string.
	 * @return The values of this row as an XML string.
	 */
	string AlmaRadiometerRow::toXML() const {
		string buf;
		buf.append("<row> \n");
		
	
		
  	
 		
		
		Parser::toXML(almaRadiometerId, "almaRadiometerId", buf);
		
		
	

  	
 		
		if (numAntennaExists) {
		
		
		Parser::toXML(numAntenna, "numAntenna", buf);
		
		
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
	void AlmaRadiometerRow::setFromXML (string rowDoc) {
		Parser row(rowDoc);
		string s = "";
		try {
	
		
	
  		
			
	  	setAlmaRadiometerId(Parser::getTag("almaRadiometerId","AlmaRadiometer",rowDoc));
			
		
	

	
  		
        if (row.isStr("<numAntenna>")) {
			
	  		setNumAntenna(Parser::getInteger("numAntenna","AlmaRadiometer",rowDoc));
			
		}
 		
	

	
	
		
	
  		
  		if (row.isStr("<spectralWindowId>")) {
  			setSpectralWindowId(Parser::get1DTag("spectralWindowId","AlmaRadiometer",rowDoc));  		
  		}
  		
  	

	
		
	

		} catch (const IllegalAccessException &err) {
			throw ConversionException (err.getMessage(),"AlmaRadiometer");
		}
	}
	
	void AlmaRadiometerRow::toBin(EndianOSStream& eoss) {
	
	
	
	
		
	almaRadiometerId.toBin(eoss);
		
	


	
	
	eoss.writeBoolean(numAntennaExists);
	if (numAntennaExists) {
	
	
	
		
						
			eoss.writeInt(numAntenna);
				
		
	

	}

	eoss.writeBoolean(spectralWindowIdExists);
	if (spectralWindowIdExists) {
	
	
	
		
	Tag::toBin(spectralWindowId, eoss);
		
	

	}

	}
	
void AlmaRadiometerRow::almaRadiometerIdFromBin(EndianIStream& eis) {
		
	
		
		
		almaRadiometerId =  Tag::fromBin(eis);
		
	
	
}

void AlmaRadiometerRow::numAntennaFromBin(EndianIStream& eis) {
		
	numAntennaExists = eis.readBoolean();
	if (numAntennaExists) {
		
	
	
		
			
		numAntenna =  eis.readInt();
			
		
	

	}
	
}
void AlmaRadiometerRow::spectralWindowIdFromBin(EndianIStream& eis) {
		
	spectralWindowIdExists = eis.readBoolean();
	if (spectralWindowIdExists) {
		
	
		
		
			
	
	spectralWindowId = Tag::from1DBin(eis);	
	

		
	

	}
	
}
	
	
	AlmaRadiometerRow* AlmaRadiometerRow::fromBin(EndianIStream& eis, AlmaRadiometerTable& table, const vector<string>& attributesSeq) {
		AlmaRadiometerRow* row = new  AlmaRadiometerRow(table);
		
		map<string, AlmaRadiometerAttributeFromBin>::iterator iter ;
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
					throw ConversionException("There is not method to read an attribute '"+attributesSeq.at(i)+"'.", "AlmaRadiometerTable");
			}
				
		}				
		return row;
	}

	//
	// A collection of methods to set the value of the attributes from their textual value in the XML representation
	// of one row.
	//
	
	// Convert a string into an Tag 
	void AlmaRadiometerRow::almaRadiometerIdFromText(const string & s) {
		 
          
		almaRadiometerId = ASDMValuesParser::parse<Tag>(s);
          
		
	}
	

	
	// Convert a string into an int 
	void AlmaRadiometerRow::numAntennaFromText(const string & s) {
		numAntennaExists = true;
		 
          
		numAntenna = ASDMValuesParser::parse<int>(s);
          
		
	}
	
	
	// Convert a string into an Tag 
	void AlmaRadiometerRow::spectralWindowIdFromText(const string & s) {
		spectralWindowIdExists = true;
		 
          
		spectralWindowId = ASDMValuesParser::parse1D<Tag>(s);
          
		
	}
	
	
	
	void AlmaRadiometerRow::fromText(const std::string& attributeName, const std::string&  t) {
		map<string, AlmaRadiometerAttributeFromText>::iterator iter;
		if ((iter = fromTextMethods.find(attributeName)) == fromTextMethods.end())
			throw ConversionException("I do not know what to do with '"+attributeName+"' and its content '"+t+"' (while parsing an XML document)", "AlmaRadiometerTable");
		(this->*(iter->second))(t);
	}
			
	////////////////////////////////////////////////
	// Intrinsic Table Attributes getters/setters //
	////////////////////////////////////////////////
	
	

	
 	/**
 	 * Get almaRadiometerId.
 	 * @return almaRadiometerId as Tag
 	 */
 	Tag AlmaRadiometerRow::getAlmaRadiometerId() const {
	
  		return almaRadiometerId;
 	}

 	/**
 	 * Set almaRadiometerId with the specified Tag.
 	 * @param almaRadiometerId The Tag value to which almaRadiometerId is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void AlmaRadiometerRow::setAlmaRadiometerId (Tag almaRadiometerId)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("almaRadiometerId", "AlmaRadiometer");
		
  		}
  	
 		this->almaRadiometerId = almaRadiometerId;
	
 	}
	
	

	
	/**
	 * The attribute numAntenna is optional. Return true if this attribute exists.
	 * @return true if and only if the numAntenna attribute exists. 
	 */
	bool AlmaRadiometerRow::isNumAntennaExists() const {
		return numAntennaExists;
	}
	

	
 	/**
 	 * Get numAntenna, which is optional.
 	 * @return numAntenna as int
 	 * @throw IllegalAccessException If numAntenna does not exist.
 	 */
 	int AlmaRadiometerRow::getNumAntenna() const  {
		if (!numAntennaExists) {
			throw IllegalAccessException("numAntenna", "AlmaRadiometer");
		}
	
  		return numAntenna;
 	}

 	/**
 	 * Set numAntenna with the specified int.
 	 * @param numAntenna The int value to which numAntenna is to be set.
 	 
 	
 	 */
 	void AlmaRadiometerRow::setNumAntenna (int numAntenna) {
	
 		this->numAntenna = numAntenna;
	
		numAntennaExists = true;
	
 	}
	
	
	/**
	 * Mark numAntenna, which is an optional field, as non-existent.
	 */
	void AlmaRadiometerRow::clearNumAntenna () {
		numAntennaExists = false;
	}
	

	
	///////////////////////////////////////////////
	// Extrinsic Table Attributes getters/setters//
	///////////////////////////////////////////////
	
	
	/**
	 * The attribute spectralWindowId is optional. Return true if this attribute exists.
	 * @return true if and only if the spectralWindowId attribute exists. 
	 */
	bool AlmaRadiometerRow::isSpectralWindowIdExists() const {
		return spectralWindowIdExists;
	}
	

	
 	/**
 	 * Get spectralWindowId, which is optional.
 	 * @return spectralWindowId as std::vector<Tag> 
 	 * @throw IllegalAccessException If spectralWindowId does not exist.
 	 */
 	std::vector<Tag>  AlmaRadiometerRow::getSpectralWindowId() const  {
		if (!spectralWindowIdExists) {
			throw IllegalAccessException("spectralWindowId", "AlmaRadiometer");
		}
	
  		return spectralWindowId;
 	}

 	/**
 	 * Set spectralWindowId with the specified std::vector<Tag> .
 	 * @param spectralWindowId The std::vector<Tag>  value to which spectralWindowId is to be set.
 	 
 	
 	 */
 	void AlmaRadiometerRow::setSpectralWindowId (std::vector<Tag>  spectralWindowId) {
	
 		this->spectralWindowId = spectralWindowId;
	
		spectralWindowIdExists = true;
	
 	}
	
	
	/**
	 * Mark spectralWindowId, which is an optional field, as non-existent.
	 */
	void AlmaRadiometerRow::clearSpectralWindowId () {
		spectralWindowIdExists = false;
	}
	


	//////////////////////////////////////
	// Links Attributes getters/setters //
	//////////////////////////////////////
	
	
 		
 	/**
 	 * Set spectralWindowId[i] with the specified Tag.
 	 * @param i The index in spectralWindowId where to set the Tag value.
 	 * @param spectralWindowId The Tag value to which spectralWindowId[i] is to be set. 
 	 * @throws OutOfBoundsException
  	 */
  	void AlmaRadiometerRow::setSpectralWindowId (int i, Tag spectralWindowId) {
  		if ((i < 0) || (i > ((int) this->spectralWindowId.size())))
  			throw OutOfBoundsException("Index out of bounds during a set operation on attribute spectralWindowId in table AlmaRadiometerTable");
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
 void AlmaRadiometerRow::addSpectralWindowId(Tag id){
 	spectralWindowId.push_back(id);
}

/**
 * Append an array of Tag to spectralWindowId.
 * @param id an array of Tag to be appended to spectralWindowId
 */
 void AlmaRadiometerRow::addSpectralWindowId(const std::vector<Tag> & id) {
 	for (unsigned int i=0; i < id.size(); i++)
 		spectralWindowId.push_back(id.at(i));
 }
 

 /**
  * Returns the Tag stored in spectralWindowId at position i.
  *
  */
 const Tag AlmaRadiometerRow::getSpectralWindowId(int i) {
 	return spectralWindowId.at(i);
 }
 
 /**
  * Returns the SpectralWindowRow linked to this row via the Tag stored in spectralWindowId
  * at position i.
  */
 SpectralWindowRow* AlmaRadiometerRow::getSpectralWindowUsingSpectralWindowId(int i) {
 	return table.getContainer().getSpectralWindow().getRowByKey(spectralWindowId.at(i));
 } 
 
 /**
  * Returns the vector of SpectralWindowRow* linked to this row via the Tags stored in spectralWindowId
  *
  */
 vector<SpectralWindowRow *> AlmaRadiometerRow::getSpectralWindowsUsingSpectralWindowId() {
 	vector<SpectralWindowRow *> result;
 	for (unsigned int i = 0; i < spectralWindowId.size(); i++)
 		result.push_back(table.getContainer().getSpectralWindow().getRowByKey(spectralWindowId.at(i)));
 		
 	return result;
 }
  

	

	
	/**
	 * Create a AlmaRadiometerRow.
	 * <p>
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 */ 
	AlmaRadiometerRow::AlmaRadiometerRow (AlmaRadiometerTable &t) : table(t) {
		hasBeenAdded = false;
		
	
	

	
		numAntennaExists = false;
	

	
	
		spectralWindowIdExists = false;
	

	
	
	
	

	

	
	
	 fromBinMethods["almaRadiometerId"] = &AlmaRadiometerRow::almaRadiometerIdFromBin; 
		
	
	 fromBinMethods["numAntenna"] = &AlmaRadiometerRow::numAntennaFromBin; 
	 fromBinMethods["spectralWindowId"] = &AlmaRadiometerRow::spectralWindowIdFromBin; 
	
	
	
	
				 
	fromTextMethods["almaRadiometerId"] = &AlmaRadiometerRow::almaRadiometerIdFromText;
		 
	

	 
				
	fromTextMethods["numAntenna"] = &AlmaRadiometerRow::numAntennaFromText;
		 	
	 
				
	fromTextMethods["spectralWindowId"] = &AlmaRadiometerRow::spectralWindowIdFromText;
		 	
		
	}
	
	AlmaRadiometerRow::AlmaRadiometerRow (AlmaRadiometerTable &t, AlmaRadiometerRow *row) : table(t) {
		hasBeenAdded = false;
		
		if (row == 0) {
	
	
	

	
		numAntennaExists = false;
	

	
	
		spectralWindowIdExists = false;
	
		
		}
		else {
	
		
			almaRadiometerId = row->almaRadiometerId;
		
		
		
		
		
		
		
		if (row->numAntennaExists) {
			numAntenna = row->numAntenna;		
			numAntennaExists = true;
		}
		else
			numAntennaExists = false;
		
		if (row->spectralWindowIdExists) {
			spectralWindowId = row->spectralWindowId;		
			spectralWindowIdExists = true;
		}
		else
			spectralWindowIdExists = false;
		
		}
		
		 fromBinMethods["almaRadiometerId"] = &AlmaRadiometerRow::almaRadiometerIdFromBin; 
			
	
		 fromBinMethods["numAntenna"] = &AlmaRadiometerRow::numAntennaFromBin; 
		 fromBinMethods["spectralWindowId"] = &AlmaRadiometerRow::spectralWindowIdFromBin; 
			
	}

	
	
	
	
	/**
	 * Return true if all required attributes of the value part are equal to their homologues
	 * in x and false otherwise.
	 *

	 * will be compared with those of this.
	 * @return a boolean.
	 */
	bool AlmaRadiometerRow::equalByRequiredValue(AlmaRadiometerRow* ) {
		
		return true;
	}	
	
/*
	 map<string, AlmaRadiometerAttributeFromBin> AlmaRadiometerRow::initFromBinMethods() {
		map<string, AlmaRadiometerAttributeFromBin> result;
		
		result["almaRadiometerId"] = &AlmaRadiometerRow::almaRadiometerIdFromBin;
		
		
		result["numAntenna"] = &AlmaRadiometerRow::numAntennaFromBin;
		result["spectralWindowId"] = &AlmaRadiometerRow::spectralWindowIdFromBin;
			
		
		return result;	
	}
*/	
} // End namespace asdm
 
