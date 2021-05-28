
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
 * Note, DataDescriptionRow.cpp is no longer being generated.
 *
 * File DataDescriptionRow.cpp
 */
 
#include <vector>
#include <set>

#include <alma/ASDM/ASDM.h>
#include <alma/ASDM/DataDescriptionRow.h>
#include <alma/ASDM/DataDescriptionTable.h>

#include <alma/ASDM/PolarizationTable.h>
#include <alma/ASDM/PolarizationRow.h>

#include <alma/ASDM/HolographyTable.h>
#include <alma/ASDM/HolographyRow.h>

#include <alma/ASDM/SpectralWindowTable.h>
#include <alma/ASDM/SpectralWindowRow.h>

#include <alma/ASDM/PulsarTable.h>
#include <alma/ASDM/PulsarRow.h>

using asdm::ASDM;
using asdm::DataDescriptionRow;
using asdm::DataDescriptionTable;

using asdm::PolarizationTable;
using asdm::PolarizationRow;

using asdm::HolographyTable;
using asdm::HolographyRow;

using asdm::SpectralWindowTable;
using asdm::SpectralWindowRow;

using asdm::PulsarTable;
using asdm::PulsarRow;

#include <alma/ASDM/Parser.h>
#include <alma/ASDM/EnumerationParser.h>
#include <alma/ASDM/ASDMValuesParser.h>
#include <alma/ASDM/InvalidArgumentException.h>

using namespace std;

namespace asdm {
	DataDescriptionRow::~DataDescriptionRow() {
	}

	/**
	 * Return the table to which this row belongs.
	 */
	DataDescriptionTable &DataDescriptionRow::getTable() const {
		return table;
	}

	bool DataDescriptionRow::isAdded() const {
		return hasBeenAdded;
	}	

	void DataDescriptionRow::isAdded(bool added) {
		hasBeenAdded = added;
	}
	
#ifndef WITHOUT_ACS
	using asdmIDL::DataDescriptionRowIDL;
#endif
	
#ifndef WITHOUT_ACS
	/**
	 * Return this row in the form of an IDL struct.
	 * @return The values of this row as a DataDescriptionRowIDL struct.
	 */
	DataDescriptionRowIDL *DataDescriptionRow::toIDL() const {
		DataDescriptionRowIDL *x = new DataDescriptionRowIDL ();
		
		// Set the x's fields.
	
		
	
  		
		
		
			
		x->dataDescriptionId = dataDescriptionId.toIDLTag();
			
		
	

	
	
		
	
  	
 		
		
	 	
			
		x->polOrHoloId = polOrHoloId.toIDLTag();
			
	 	 		
  	

	
  	
 		
		
	 	
			
		x->spectralWindowId = spectralWindowId.toIDLTag();
			
	 	 		
  	

	
		x->pulsarIdExists = pulsarIdExists;





		x->pulsarId = pulsarId.toIDLTag();

	

		
		return x;
	
	}

  void DataDescriptionRow::toIDL(asdmIDL::DataDescriptionRowIDL& x) const {
		
		// Fill the IDL structure.			
		x.dataDescriptionId = dataDescriptionId.toIDLTag();
		x.polOrHoloId = polOrHoloId.toIDLTag();
		x.spectralWindowId = spectralWindowId.toIDLTag();	
		x.pulsarIdExists = pulsarIdExists;
		x.pulsarId = pulsarId.toIDLTag();
	}
#endif
	

#ifndef WITHOUT_ACS
	/**
	 * Fill the values of this row from the IDL struct DataDescriptionRowIDL.
	 * @param x The IDL struct containing the values used to fill this row.
	 */
	void DataDescriptionRow::setFromIDL (DataDescriptionRowIDL x){
		try {
		// Fill the values from x.
	
		
	
		
		
			
		setDataDescriptionId(Tag (x.dataDescriptionId));
			
 		
		
	

	
	
		
	
		
		
			
		setPolOrHoloId(Tag (x.polOrHoloId));
			
 		
		
	

	
		
		
			
		setSpectralWindowId(Tag (x.spectralWindowId));
			
 		
		
	

	
		pulsarIdExists = x.pulsarIdExists;
		if (x.pulsarIdExists) {


		setPulsarId(Tag (x.pulsarId));

		
		}
	

	

		} catch (const IllegalAccessException &err) {
			throw ConversionException (err.getMessage(),"DataDescription");
		}
	}
#endif
	
	/**
	 * Return this row in the form of an XML string.
	 * @return The values of this row as an XML string.
	 */
	string DataDescriptionRow::toXML() const {
		string buf;
		buf.append("<row> \n");
		
	
		
  	
 		
		
		Parser::toXML(dataDescriptionId, "dataDescriptionId", buf);
		
		
	

	
	
		
  	
 		
		
		Parser::toXML(polOrHoloId, "polOrHoloId", buf);
		
		
	

  	
 		
		
		Parser::toXML(spectralWindowId, "spectralWindowId", buf);
		
		
	

	
		
		if (pulsarIdExists) {

		
		Parser::toXML(pulsarId, "pulsarId", buf);


		}

	

		
		buf.append("</row>\n");
		return buf;
	}

	/**
	 * Fill the values of this row from an XML string 
	 * that was produced by the toXML() method.
	 * @param x The XML string being used to set the values of this row.
	 */
	void DataDescriptionRow::setFromXML (string rowDoc) {
		Parser row(rowDoc);
		string s = "";
		try {
	
		
	
  		
			
	  	setDataDescriptionId(Parser::getTag("dataDescriptionId","DataDescription",rowDoc));
			
		
	

	
	
		
	
  		
			
	  	setPolOrHoloId(Parser::getTag("polOrHoloId","DataDescription",rowDoc));
			
		
	

	
  		
			
	  	setSpectralWindowId(Parser::getTag("spectralWindowId","DataDescription",rowDoc));
			
		
	

		if (row.isStr("<pulsarId>")) {

			setPulsarId(Parser::getTag("pulsarId","Pulsar",rowDoc));

		} else {
                    clearPulsarId();
                }
		
	

	

		} catch (const IllegalAccessException &err) {
			throw ConversionException (err.getMessage(),"DataDescription");
		}
	}
	
	void DataDescriptionRow::toBin(EndianOSStream& eoss) {
	
	
	
	
		
	dataDescriptionId.toBin(eoss);
		
	

	
	
		
	polOrHoloId.toBin(eoss);
		
	

	
	
		
	spectralWindowId.toBin(eoss);
		
	


	eoss.writeBoolean(pulsarIdExists);
	if (pulsarIdExists) {

	pulsarId.toBin(eoss);

	
	}
	
	}
	
void DataDescriptionRow::dataDescriptionIdFromBin(EndianIStream& eis) {
		
	
		
		
		dataDescriptionId =  Tag::fromBin(eis);
		
	
	
}
void DataDescriptionRow::polOrHoloIdFromBin(EndianIStream& eis) {
		
	
		
		
		polOrHoloId =  Tag::fromBin(eis);
		
	
	
}
void DataDescriptionRow::spectralWindowIdFromBin(EndianIStream& eis) {
		
	
		
		
		spectralWindowId =  Tag::fromBin(eis);
		
	
	
}
void DataDescriptionRow::pulsarIdFromBin(EndianIStream& eis) {


		pulsarIdExists = eis.readBoolean();
		if (pulsarIdExists) {


			pulsarId = Tag::fromBin(eis);


		}
}
	
	DataDescriptionRow* DataDescriptionRow::fromBin(EndianIStream& eis, DataDescriptionTable& table, const vector<string>& attributesSeq) {
		DataDescriptionRow* row = new  DataDescriptionRow(table);
		
		map<string, DataDescriptionAttributeFromBin>::iterator iter ;
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
					throw ConversionException("There is not method to read an attribute '"+attributesSeq.at(i)+"'.", "DataDescriptionTable");
			}
				
		}				
		return row;
	}

	//
	// A collection of methods to set the value of the attributes from their textual value in the XML representation
	// of one row.
	//
	
	// Convert a string into an Tag 
	void DataDescriptionRow::dataDescriptionIdFromText(const string & s) {
		 
		dataDescriptionId = ASDMValuesParser::parse<Tag>(s);
		
	}
	
	
	// Convert a string into an Tag 
	void DataDescriptionRow::polOrHoloIdFromText(const string & s) {
		 
		polOrHoloId = ASDMValuesParser::parse<Tag>(s);
		
	}
	
	
	// Convert a string into an Tag 
	void DataDescriptionRow::spectralWindowIdFromText(const string & s) {
		 
		spectralWindowId = ASDMValuesParser::parse<Tag>(s);
		
	}
	

	// Convert a string into an Tag
	void DataDescriptionRow::pulsarIdFromText(const string & s) {
		pulsarIdExists = true;

		pulsarId = ASDMValuesParser::parse<Tag>(s);

	}	
	
	void DataDescriptionRow::fromText(const std::string& attributeName, const std::string&  t) {
		map<string, DataDescriptionAttributeFromText>::iterator iter;
		if ((iter = fromTextMethods.find(attributeName)) == fromTextMethods.end())
			throw ConversionException("I do not know what to do with '"+attributeName+"' and its content '"+t+"' (while parsing an XML document)", "DataDescriptionTable");
		(this->*(iter->second))(t);
	}
			
	////////////////////////////////////////////////
	// Intrinsic Table Attributes getters/setters //
	////////////////////////////////////////////////
	
	

	
 	/**
 	 * Get dataDescriptionId.
 	 * @return dataDescriptionId as Tag
 	 */
 	Tag DataDescriptionRow::getDataDescriptionId() const {
	
  		return dataDescriptionId;
 	}

 	/**
 	 * Set dataDescriptionId with the specified Tag.
 	 * @param dataDescriptionId The Tag value to which dataDescriptionId is to be set.
 	 
 	
 		
 	 * @throw IllegalAccessException If an attempt is made to change this field after is has been added to the table.
 	 	
 	 */
 	void DataDescriptionRow::setDataDescriptionId (Tag dataDescriptionId)  {
  	
  	
  		if (hasBeenAdded) {
 		
			throw IllegalAccessException("dataDescriptionId", "DataDescription");
		
  		}
  	
 		this->dataDescriptionId = dataDescriptionId;
	
 	}
	
	

	
	///////////////////////////////////////////////
	// Extrinsic Table Attributes getters/setters//
	///////////////////////////////////////////////
	
	

	
 	/**
 	 * Get polOrHoloId.
 	 * @return polOrHoloId as Tag
 	 */
 	Tag DataDescriptionRow::getPolOrHoloId() const {
	
  		return polOrHoloId;
 	}

 	/**
 	 * Set polOrHoloId with the specified Tag.
 	 * @param polOrHoloId The Tag value to which polOrHoloId is to be set.
 	 
 	
 		
 	 */
 	void DataDescriptionRow::setPolOrHoloId (Tag polOrHoloId)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->polOrHoloId = polOrHoloId;
	
 	}
	
	

	

	
 	/**
 	 * Get spectralWindowId.
 	 * @return spectralWindowId as Tag
 	 */
 	Tag DataDescriptionRow::getSpectralWindowId() const {
	
  		return spectralWindowId;
 	}

 	/**
 	 * Set spectralWindowId with the specified Tag.
 	 * @param spectralWindowId The Tag value to which spectralWindowId is to be set.
 	 
 	
 		
 	 */
 	void DataDescriptionRow::setSpectralWindowId (Tag spectralWindowId)  {
  	
  	
  		if (hasBeenAdded) {
 		
  		}
  	
 		this->spectralWindowId = spectralWindowId;
	
 	}
	
	

        /**
         * The attribute pulsarId is optional. Return true if this attribute exists.
         * @return true if and only if the pulsarId attribute exists. 
         */
        bool DataDescriptionRow::isPulsarIdExists() const {
                return pulsarIdExists;
        }



        /**
         * Get pulsarId, which is optional.
         * @return pulsarId as Tag
         * @throw IllegalAccessException If pulsarId does not exist.
         */
        Tag DataDescriptionRow::getPulsarId() const  {
                if (!pulsarIdExists) {
                        throw IllegalAccessException("pulsarId", "DataDescription");
                }

                return pulsarId;
        }


        /**
         * Set pulsarId with the specified Tag.
         * @param pulsarId The Tag value to which pulsarId is to be set.
         
        
         */
        void DataDescriptionRow::setPulsarId (Tag pulsarId) {

                this->pulsarId = pulsarId;

                pulsarIdExists = true;

        }


        /**
         * Mark pulsarId, which is an optional field, as non-existent.
         */
        void DataDescriptionRow::clearPulsarId () {
                pulsarIdExists = false;
        }




	//////////////////////////////////////
	// Links Attributes getters/setters //
	//////////////////////////////////////
	
	
	
	
		

	/**
	 * Returns the pointer to the row in the Polarization table having Polarization.polarizationId == polOrHoloId
	 * @return a PolarizationRow*
	 * 
	 
	 */
	 PolarizationRow* DataDescriptionRow::getPolarizationUsingPolOrHoloId() {
	 
	 	return table.getContainer().getPolarization().getRowByKey(polOrHoloId);
	 }



	 
	/**
	 * Returns the pointer to the row in the Holography table having Holography.holographyId == polOrHoloId
	 * @return a HolographyRow*
	 * 
	 
	 */
	 HolographyRow* DataDescriptionRow::getHolographyUsingPolOrHoloId() {
	 
	 	return table.getContainer().getHolography().getRowByKey(polOrHoloId);
	 }
	 


	

	
	
	
		

	/**
	 * Returns the pointer to the row in the SpectralWindow table having SpectralWindow.spectralWindowId == spectralWindowId
	 * @return a SpectralWindowRow*
	 * 
	 
	 */
	 SpectralWindowRow* DataDescriptionRow::getSpectralWindowUsingSpectralWindowId() {
	 
	 	return table.getContainer().getSpectralWindow().getRowByKey(spectralWindowId);
	 }
	 

	
        /**
         * Returns the pointer to the row in the Pulsar table having Pulsar.pulsarId == pulsarId
         * @return a PulsarRow*
         * 
         
         * throws IllegalAccessException
         
         */
         PulsarRow* DataDescriptionRow::getPulsarUsingPulsarId() {

                if (!pulsarIdExists)
                        throw IllegalAccessException();

                return table.getContainer().getPulsar().getRowByKey(pulsarId);
         }

	
	/**
	 * Create a DataDescriptionRow.
	 * <p>
	 * This constructor is private because only the
	 * table can create rows.  All rows know the table
	 * to which they belong.
	 * @param table The table to which this row belongs.
	 */ 
	DataDescriptionRow::DataDescriptionRow (DataDescriptionTable &t) : table(t) {
		hasBeenAdded = false;
                pulsarIdExists = false;
		
	
	

	
	

	

	
	
	
	

	
	
	 fromBinMethods["dataDescriptionId"] = &DataDescriptionRow::dataDescriptionIdFromBin; 
	 fromBinMethods["polOrHoloId"] = &DataDescriptionRow::polOrHoloIdFromBin; 
	 fromBinMethods["spectralWindowId"] = &DataDescriptionRow::spectralWindowIdFromBin; 
	 fromBinMethods["pulsarId"] = &DataDescriptionRow::pulsarIdFromBin;	
	
	
	
	
	
				 
	fromTextMethods["dataDescriptionId"] = &DataDescriptionRow::dataDescriptionIdFromText;
		 
	
				 
	fromTextMethods["polOrHoloId"] = &DataDescriptionRow::polOrHoloIdFromText;
		 
	
				 
	fromTextMethods["spectralWindowId"] = &DataDescriptionRow::spectralWindowIdFromText;
		 
	
	fromTextMethods["pulsarId"] = &DataDescriptionRow::pulsarIdFromText;
		
	}
	
	DataDescriptionRow::DataDescriptionRow (DataDescriptionTable &t, DataDescriptionRow *row) : table(t) {
		hasBeenAdded = false;
                pulsarIdExists = false;
		
		if (row == 0) {
	
	
	

	
	

	
		
		}
		else {
	
		
			dataDescriptionId = row->dataDescriptionId;
		
		
		
		
			polOrHoloId = row->polOrHoloId;
		
			spectralWindowId = row->spectralWindowId;
		
			if (row->pulsarIdExists) {
				pulsarId = row->pulsarId;
				pulsarIdExists = true;
			}
			else
				pulsarIdExists = false;
			
		
		}
		
		 fromBinMethods["dataDescriptionId"] = &DataDescriptionRow::dataDescriptionIdFromBin; 
		 fromBinMethods["polOrHoloId"] = &DataDescriptionRow::polOrHoloIdFromBin; 
		 fromBinMethods["spectralWindowId"] = &DataDescriptionRow::spectralWindowIdFromBin; 
		 fromBinMethods["pulsarId"] = &DataDescriptionRow::pulsarIdFromBin;	
	
			
	}

	
	bool DataDescriptionRow::compareNoAutoInc(Tag polOrHoloId, Tag spectralWindowId) {
		bool result;
		result = true;
		
	
		
		result = result && (this->polOrHoloId == polOrHoloId);
		
		if (!result) return false;
	

	
		
		result = result && (this->spectralWindowId == spectralWindowId);
		
		if (!result) return false;
	

		return result;
	}	
	
	
	
	bool DataDescriptionRow::compareRequiredValue(Tag polOrHoloId, Tag spectralWindowId) {
		bool result;
		result = true;
		
	
		if (!(this->polOrHoloId == polOrHoloId)) return false;
	

	
		if (!(this->spectralWindowId == spectralWindowId)) return false;
	

		return result;
	}
	
	
	/**
	 * Return true if all required attributes of the value part are equal to their homologues
	 * in x and false otherwise.
	 *
	 * @param x a pointer on the DataDescriptionRow whose required attributes of the value part 
	 * will be compared with those of this.
	 * @return a boolean.
	 */
	bool DataDescriptionRow::equalByRequiredValue(DataDescriptionRow* x) {
		
			
		if (this->polOrHoloId != x->polOrHoloId) return false;
			
		if (this->spectralWindowId != x->spectralWindowId) return false;
			
		
		return true;
	}	
	
/*
	 map<string, DataDescriptionAttributeFromBin> DataDescriptionRow::initFromBinMethods() {
		map<string, DataDescriptionAttributeFromBin> result;
		
		result["dataDescriptionId"] = &DataDescriptionRow::dataDescriptionIdFromBin;
		result["polOrHoloId"] = &DataDescriptionRow::polOrHoloIdFromBin;
		result["spectralWindowId"] = &DataDescriptionRow::spectralWindowIdFromBin;
                result["pulsarId"] = &DataDescriptionRow::pulsarIdFromBin;
		
		
			
		
		return result;	
	}
*/	
} // End namespace asdm
 
