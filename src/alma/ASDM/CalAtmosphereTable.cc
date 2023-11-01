
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
 * File CalAtmosphereTable.cpp
 */
#include <alma/ASDM/ConversionException.h>
#include <alma/ASDM/DuplicateKey.h>
#include <alma/ASDM/OutOfBoundsException.h>

using asdm::ConversionException;
using asdm::DuplicateKey;
using asdm::OutOfBoundsException;

#include <alma/ASDM/ASDM.h>
#include <alma/ASDM/CalAtmosphereTable.h>
#include <alma/ASDM/CalAtmosphereRow.h>
#include <alma/ASDM/Parser.h>

using asdm::ASDM;
using asdm::CalAtmosphereTable;
using asdm::CalAtmosphereRow;
using asdm::Parser;

#include <iostream>
#include <fstream>
#include <iterator>
#include <sstream>
#include <set>
#include <algorithm>
using namespace std;

#include <alma/ASDM/Misc.h>
using namespace asdm;

#include <libxml/parser.h>
#include <libxml/tree.h>

#ifndef WITHOUT_BOOST
#include "boost/filesystem/operations.hpp"
#include <boost/algorithm/string.hpp>
#else
#include <sys/stat.h>
#endif

namespace asdm {
	// The name of the entity we will store in this table.
	static string entityNameOfCalAtmosphere = "CalAtmosphere";
	
	// An array of string containing the names of the columns of this table.
	// The array is filled in the order : key, required value, optional value.
	//
	static string attributesNamesOfCalAtmosphere_a[] = {
		
			"antennaName"
		,
			"receiverBand"
		,
			"basebandName"
		,
			"calDataId"
		,
			"calReductionId"
		
		
			, "startValidTime"
		
			, "endValidTime"
		
			, "numFreq"
		
			, "numLoad"
		
			, "numReceptor"
		
			, "forwardEffSpectrum"
		
			, "frequencyRange"
		
			, "groundPressure"
		
			, "groundRelHumidity"
		
			, "frequencySpectrum"
		
			, "groundTemperature"
		
			, "polarizationTypes"
		
			, "powerSkySpectrum"
		
			, "powerLoadSpectrum"
		
			, "syscalType"
		
			, "tAtmSpectrum"
		
			, "tRecSpectrum"
		
			, "tSysSpectrum"
		
			, "tauSpectrum"
		
			, "tAtm"
		
			, "tRec"
		
			, "tSys"
		
			, "tau"
		
			, "water"
		
			, "waterError"
				
		
			, "alphaSpectrum"
		
			, "forwardEfficiency"
		
			, "forwardEfficiencyError"
		
			, "sbGain"
		
			, "sbGainError"
		
			, "sbGainSpectrum"
				
	};
	
	// A vector of string whose content is a copy of the strings in the array above.
	//
	static vector<string> attributesNamesOfCalAtmosphere_v (attributesNamesOfCalAtmosphere_a, attributesNamesOfCalAtmosphere_a + sizeof(attributesNamesOfCalAtmosphere_a) / sizeof(attributesNamesOfCalAtmosphere_a[0]));

	// An array of string containing the names of the columns of this table.
	// The array is filled in the order where the names would be read by default in the XML header of a file containing
	// the table exported in binary mode.
	//	
	static string attributesNamesInBinOfCalAtmosphere_a[] = {
    
    	 "antennaName" , "receiverBand" , "basebandName" , "calDataId" , "calReductionId" , "startValidTime" , "endValidTime" , "numFreq" , "numLoad" , "numReceptor" , "forwardEffSpectrum" , "frequencyRange" , "groundPressure" , "groundRelHumidity" , "frequencySpectrum" , "groundTemperature" , "polarizationTypes" , "powerSkySpectrum" , "powerLoadSpectrum" , "syscalType" , "tAtmSpectrum" , "tRecSpectrum" , "tSysSpectrum" , "tauSpectrum" , "tAtm" , "tRec" , "tSys" , "tau" , "water" , "waterError" 
    	,
    	 "alphaSpectrum" , "forwardEfficiency" , "forwardEfficiencyError" , "sbGain" , "sbGainError" , "sbGainSpectrum" 
    
	};
	        			
	// A vector of string whose content is a copy of the strings in the array above.
	//
	static vector<string> attributesNamesInBinOfCalAtmosphere_v(attributesNamesInBinOfCalAtmosphere_a, attributesNamesInBinOfCalAtmosphere_a + sizeof(attributesNamesInBinOfCalAtmosphere_a) / sizeof(attributesNamesInBinOfCalAtmosphere_a[0]));		
	

	// The array of attributes (or column) names that make up key key.
	//
	string keyOfCalAtmosphere_a[] = {
	
		"antennaName"
	,
		"receiverBand"
	,
		"basebandName"
	,
		"calDataId"
	,
		"calReductionId"
		 
	};
	 
	// A vector of strings which are copies of those stored in the array above.
	vector<string> keyOfCalAtmosphere_v(keyOfCalAtmosphere_a, keyOfCalAtmosphere_a + sizeof(keyOfCalAtmosphere_a) / sizeof(keyOfCalAtmosphere_a[0]));

	/**
	 * Return the list of field names that make up key key
	 * as a const reference to a vector of strings.
	 */	
	const vector<string>& CalAtmosphereTable::getKeyName() {
		return keyOfCalAtmosphere_v;
	}


	CalAtmosphereTable::CalAtmosphereTable(ASDM &c) : container(c) {

		// Define a default entity.
		entity.setEntityId(EntityId("uid://X0/X0/X0"));
		entity.setEntityIdEncrypted("na");
		entity.setEntityTypeName("CalAtmosphereTable");
		entity.setEntityVersion("1");
		entity.setInstanceVersion("1");
		
		// Archive XML
		archiveAsBin = true;
		
		// File XML
		fileAsBin = true;
		
		// By default the table is considered as present in memory
		presentInMemory = true;
		
		// By default there is no load in progress
		loadInProgress = false;
	}
	
/**
 * A destructor for CalAtmosphereTable.
 */
	CalAtmosphereTable::~CalAtmosphereTable() {
		for (unsigned int i = 0; i < privateRows.size(); i++) 
			delete(privateRows.at(i));
	}

	/**
	 * Container to which this table belongs.
	 */
	ASDM &CalAtmosphereTable::getContainer() const {
		return container;
	}

	/**
	 * Return the number of rows in the table.
	 */
	unsigned int CalAtmosphereTable::size() const {
		if (presentInMemory) 
			return privateRows.size();
		else
			return declaredSize;
	}
	
	/**
	 * Return the name of this table.
	 */
	string CalAtmosphereTable::getName() const {
		return entityNameOfCalAtmosphere;
	}
	
	/**
	 * Return the name of this table.
	 */
	string CalAtmosphereTable::name() {
		return entityNameOfCalAtmosphere;
	}
	
	/**
	 * Return the the names of the attributes (or columns) of this table.
	 */
	const vector<string>& CalAtmosphereTable::getAttributesNames() { return attributesNamesOfCalAtmosphere_v; }
	
	/**
	 * Return the the names of the attributes (or columns) of this table as they appear by default
	 * in an binary export of this table.
	 */
	const vector<string>& CalAtmosphereTable::defaultAttributesNamesInBin() { return attributesNamesInBinOfCalAtmosphere_v; }

	/**
	 * Return this table's Entity.
	 */
	Entity CalAtmosphereTable::getEntity() const {
		return entity;
	}

	/**
	 * Set this table's Entity.
	 */
	void CalAtmosphereTable::setEntity(Entity e) {
		this->entity = e; 
	}
	
	//
	// ====> Row creation.
	//
	
	/**
	 * Create a new row.
	 */
	CalAtmosphereRow *CalAtmosphereTable::newRow() {
		return new CalAtmosphereRow (*this);
	}
	

	/**
	 * Create a new row initialized to the specified values.
	 * @return a pointer on the created and initialized row.
	
 	 * @param antennaName 
	
 	 * @param receiverBand 
	
 	 * @param basebandName 
	
 	 * @param calDataId 
	
 	 * @param calReductionId 
	
 	 * @param startValidTime 
	
 	 * @param endValidTime 
	
 	 * @param numFreq 
	
 	 * @param numLoad 
	
 	 * @param numReceptor 
	
 	 * @param forwardEffSpectrum 
	
 	 * @param frequencyRange 
	
 	 * @param groundPressure 
	
 	 * @param groundRelHumidity 
	
 	 * @param frequencySpectrum 
	
 	 * @param groundTemperature 
	
 	 * @param polarizationTypes 
	
 	 * @param powerSkySpectrum 
	
 	 * @param powerLoadSpectrum 
	
 	 * @param syscalType 
	
 	 * @param tAtmSpectrum 
	
 	 * @param tRecSpectrum 
	
 	 * @param tSysSpectrum 
	
 	 * @param tauSpectrum 
	
 	 * @param tAtm 
	
 	 * @param tRec 
	
 	 * @param tSys 
	
 	 * @param tau 
	
 	 * @param water 
	
 	 * @param waterError 
	
     */
	CalAtmosphereRow* CalAtmosphereTable::newRow(std::string antennaName, ReceiverBandMod::ReceiverBand receiverBand, BasebandNameMod::BasebandName basebandName, Tag calDataId, Tag calReductionId, ArrayTime startValidTime, ArrayTime endValidTime, int numFreq, int numLoad, int numReceptor, std::vector<std::vector<float > > forwardEffSpectrum, std::vector<Frequency > frequencyRange, Pressure groundPressure, Humidity groundRelHumidity, std::vector<Frequency > frequencySpectrum, Temperature groundTemperature, std::vector<PolarizationTypeMod::PolarizationType > polarizationTypes, std::vector<std::vector<float > > powerSkySpectrum, std::vector<std::vector<std::vector<float > > > powerLoadSpectrum, SyscalMethodMod::SyscalMethod syscalType, std::vector<std::vector<Temperature > > tAtmSpectrum, std::vector<std::vector<Temperature > > tRecSpectrum, std::vector<std::vector<Temperature > > tSysSpectrum, std::vector<std::vector<float > > tauSpectrum, std::vector<Temperature > tAtm, std::vector<Temperature > tRec, std::vector<Temperature > tSys, std::vector<float > tau, std::vector<Length > water, std::vector<Length > waterError){
		CalAtmosphereRow *row = new CalAtmosphereRow(*this);
			
		row->setAntennaName(antennaName);
			
		row->setReceiverBand(receiverBand);
			
		row->setBasebandName(basebandName);
			
		row->setCalDataId(calDataId);
			
		row->setCalReductionId(calReductionId);
			
		row->setStartValidTime(startValidTime);
			
		row->setEndValidTime(endValidTime);
			
		row->setNumFreq(numFreq);
			
		row->setNumLoad(numLoad);
			
		row->setNumReceptor(numReceptor);
			
		row->setForwardEffSpectrum(forwardEffSpectrum);
			
		row->setFrequencyRange(frequencyRange);
			
		row->setGroundPressure(groundPressure);
			
		row->setGroundRelHumidity(groundRelHumidity);
			
		row->setFrequencySpectrum(frequencySpectrum);
			
		row->setGroundTemperature(groundTemperature);
			
		row->setPolarizationTypes(polarizationTypes);
			
		row->setPowerSkySpectrum(powerSkySpectrum);
			
		row->setPowerLoadSpectrum(powerLoadSpectrum);
			
		row->setSyscalType(syscalType);
			
		row->setTAtmSpectrum(tAtmSpectrum);
			
		row->setTRecSpectrum(tRecSpectrum);
			
		row->setTSysSpectrum(tSysSpectrum);
			
		row->setTauSpectrum(tauSpectrum);
			
		row->setTAtm(tAtm);
			
		row->setTRec(tRec);
			
		row->setTSys(tSys);
			
		row->setTau(tau);
			
		row->setWater(water);
			
		row->setWaterError(waterError);
	
		return row;		
	}	
	


CalAtmosphereRow* CalAtmosphereTable::newRow(CalAtmosphereRow* row) {
	return new CalAtmosphereRow(*this, row);
}

	//
	// Append a row to its table.
	//

	
	 
	/**
	 * Add a row.
	 * @throws DuplicateKey Thrown if the new row has a key that is already in the table.
	 * @param x A pointer to the row to be added.
	 * @return x
	 */
	CalAtmosphereRow* CalAtmosphereTable::add(CalAtmosphereRow* x) {
		
		if (getRowByKey(
						x->getAntennaName()
						,
						x->getReceiverBand()
						,
						x->getBasebandName()
						,
						x->getCalDataId()
						,
						x->getCalReductionId()
						))
			//throw DuplicateKey(x.getAntennaName() + "|" + x.getReceiverBand() + "|" + x.getBasebandName() + "|" + x.getCalDataId() + "|" + x.getCalReductionId(),"CalAtmosphere");
			throw DuplicateKey("Duplicate key exception in ","CalAtmosphereTable");
		
		row.push_back(x);
		privateRows.push_back(x);
		x->isAdded(true);
		return x;
	}

	
		
	void CalAtmosphereTable::addWithoutCheckingUnique(CalAtmosphereRow * x) {
		if (getRowByKey(
						x->getAntennaName()
						,
						x->getReceiverBand()
						,
						x->getBasebandName()
						,
						x->getCalDataId()
						,
						x->getCalReductionId()
						) != (CalAtmosphereRow *) 0) 
			throw DuplicateKey("Dupicate key exception in ", "CalAtmosphereTable");
		row.push_back(x);
		privateRows.push_back(x);
		x->isAdded(true);
	}




	// 
	// A private method to append a row to its table, used by input conversion
	// methods, with row uniqueness.
	//

	
	/**
	 * If this table has an autoincrementable attribute then check if *x verifies the rule of uniqueness and throw exception if not.
	 * Check if *x verifies the key uniqueness rule and throw an exception if not.
	 * Append x to its table.
	 * @param x a pointer on the row to be appended.
	 * @returns a pointer on x.
	 * @throws DuplicateKey
	 
	 */
	CalAtmosphereRow*  CalAtmosphereTable::checkAndAdd(CalAtmosphereRow* x, bool skipCheckUniqueness)  {
		if (!skipCheckUniqueness) { 
		
		}
		
		if (getRowByKey(
	
			x->getAntennaName()
	,
			x->getReceiverBand()
	,
			x->getBasebandName()
	,
			x->getCalDataId()
	,
			x->getCalReductionId()
			
		)) throw DuplicateKey("Duplicate key exception in ", "CalAtmosphereTable");
		
		row.push_back(x);
		privateRows.push_back(x);
		x->isAdded(true);
		return x;	
	}	



	//
	// A private method to brutally append a row to its table, without checking for row uniqueness.
	//

	void CalAtmosphereTable::append(CalAtmosphereRow *x) {
		privateRows.push_back(x);
		x->isAdded(true);
	}





	 vector<CalAtmosphereRow *> CalAtmosphereTable::get() {
	 	checkPresenceInMemory();
	    return privateRows;
	 }
	 
	 const vector<CalAtmosphereRow *>& CalAtmosphereTable::get() const {
	 	const_cast<CalAtmosphereTable&>(*this).checkPresenceInMemory();	
	    return privateRows;
	 }	 
	 	




	

	
/*
 ** Returns a CalAtmosphereRow* given a key.
 ** @return a pointer to the row having the key whose values are passed as parameters, or 0 if
 ** no row exists for that key.
 **
 */
 	CalAtmosphereRow* CalAtmosphereTable::getRowByKey(std::string antennaName, ReceiverBandMod::ReceiverBand receiverBand, BasebandNameMod::BasebandName basebandName, Tag calDataId, Tag calReductionId)  {
 	checkPresenceInMemory();
	CalAtmosphereRow* aRow = 0;
	for (unsigned int i = 0; i < privateRows.size(); i++) {
		aRow = row.at(i);
		
			
				if (aRow->antennaName != antennaName) continue;
			
		
			
				if (aRow->receiverBand != receiverBand) continue;
			
		
			
				if (aRow->basebandName != basebandName) continue;
			
		
			
				if (aRow->calDataId != calDataId) continue;
			
		
			
				if (aRow->calReductionId != calReductionId) continue;
			
		
		return aRow;
	}
	return 0;		
}
	

	
/**
 * Look up the table for a row whose all attributes 
 * are equal to the corresponding parameters of the method.
 * @return a pointer on this row if any, 0 otherwise.
 *
			
 * @param antennaName.
 	 		
 * @param receiverBand.
 	 		
 * @param basebandName.
 	 		
 * @param calDataId.
 	 		
 * @param calReductionId.
 	 		
 * @param startValidTime.
 	 		
 * @param endValidTime.
 	 		
 * @param numFreq.
 	 		
 * @param numLoad.
 	 		
 * @param numReceptor.
 	 		
 * @param forwardEffSpectrum.
 	 		
 * @param frequencyRange.
 	 		
 * @param groundPressure.
 	 		
 * @param groundRelHumidity.
 	 		
 * @param frequencySpectrum.
 	 		
 * @param groundTemperature.
 	 		
 * @param polarizationTypes.
 	 		
 * @param powerSkySpectrum.
 	 		
 * @param powerLoadSpectrum.
 	 		
 * @param syscalType.
 	 		
 * @param tAtmSpectrum.
 	 		
 * @param tRecSpectrum.
 	 		
 * @param tSysSpectrum.
 	 		
 * @param tauSpectrum.
 	 		
 * @param tAtm.
 	 		
 * @param tRec.
 	 		
 * @param tSys.
 	 		
 * @param tau.
 	 		
 * @param water.
 	 		
 * @param waterError.
 	 		 
 */
CalAtmosphereRow* CalAtmosphereTable::lookup(std::string antennaName, ReceiverBandMod::ReceiverBand receiverBand, BasebandNameMod::BasebandName basebandName, Tag calDataId, Tag calReductionId, ArrayTime startValidTime, ArrayTime endValidTime, int numFreq, int numLoad, int numReceptor, std::vector<std::vector<float > > forwardEffSpectrum, std::vector<Frequency > frequencyRange, Pressure groundPressure, Humidity groundRelHumidity, std::vector<Frequency > frequencySpectrum, Temperature groundTemperature, std::vector<PolarizationTypeMod::PolarizationType > polarizationTypes, std::vector<std::vector<float > > powerSkySpectrum, std::vector<std::vector<std::vector<float > > > powerLoadSpectrum, SyscalMethodMod::SyscalMethod syscalType, std::vector<std::vector<Temperature > > tAtmSpectrum, std::vector<std::vector<Temperature > > tRecSpectrum, std::vector<std::vector<Temperature > > tSysSpectrum, std::vector<std::vector<float > > tauSpectrum, std::vector<Temperature > tAtm, std::vector<Temperature > tRec, std::vector<Temperature > tSys, std::vector<float > tau, std::vector<Length > water, std::vector<Length > waterError) {
		CalAtmosphereRow* aRow;
		for (unsigned int i = 0; i < privateRows.size(); i++) {
			aRow = privateRows.at(i); 
			if (aRow->compareNoAutoInc(antennaName, receiverBand, basebandName, calDataId, calReductionId, startValidTime, endValidTime, numFreq, numLoad, numReceptor, forwardEffSpectrum, frequencyRange, groundPressure, groundRelHumidity, frequencySpectrum, groundTemperature, polarizationTypes, powerSkySpectrum, powerLoadSpectrum, syscalType, tAtmSpectrum, tRecSpectrum, tSysSpectrum, tauSpectrum, tAtm, tRec, tSys, tau, water, waterError)) return aRow;
		}			
		return 0;	
} 
	
 	 	

	



#ifndef WITHOUT_ACS
	using asdmIDL::CalAtmosphereTableIDL;
#endif

#ifndef WITHOUT_ACS
	// Conversion Methods

	CalAtmosphereTableIDL *CalAtmosphereTable::toIDL() {
		CalAtmosphereTableIDL *x = new CalAtmosphereTableIDL ();
		unsigned int nrow = size();
		x->row.length(nrow);
		vector<CalAtmosphereRow*> v = get();
		for (unsigned int i = 0; i < nrow; ++i) {
			//x->row[i] = *(v[i]->toIDL());
			v[i]->toIDL(x->row[i]);
		}
		return x;
	}
	
	void CalAtmosphereTable::toIDL(asdmIDL::CalAtmosphereTableIDL& x) const {
		unsigned int nrow = size();
		x.row.length(nrow);
		vector<CalAtmosphereRow*> v = get();
		for (unsigned int i = 0; i < nrow; ++i) {
			v[i]->toIDL(x.row[i]);
		}
	}	
#endif
	
#ifndef WITHOUT_ACS
	void CalAtmosphereTable::fromIDL(CalAtmosphereTableIDL x) {
		unsigned int nrow = x.row.length();
		for (unsigned int i = 0; i < nrow; ++i) {
			CalAtmosphereRow *tmp = newRow();
			tmp->setFromIDL(x.row[i]);
			// checkAndAdd(tmp);
			add(tmp);
		}
	}	
#endif

	
	string CalAtmosphereTable::toXML()  {
		string buf;

		buf.append("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?> ");
		buf.append("<CalAtmosphereTable xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:clatm=\"http://Alma/XASDM/CalAtmosphereTable\" xsi:schemaLocation=\"http://Alma/XASDM/CalAtmosphereTable http://almaobservatory.org/XML/XASDM/4/CalAtmosphereTable.xsd\" schemaVersion=\"4\" schemaRevision=\"-1\">\n");
	
		buf.append(entity.toXML());
		string s = container.getEntity().toXML();
		// Change the "Entity" tag to "ContainerEntity".
		buf.append("<Container" + s.substr(1,s.length() - 1)+" ");
		vector<CalAtmosphereRow*> v = get();
		for (unsigned int i = 0; i < v.size(); ++i) {
			try {
				buf.append(v[i]->toXML());
			} catch (const NoSuchRow &e) {
			}
			buf.append("  ");
		}		
		buf.append("</CalAtmosphereTable> ");
		return buf;
	}

	
	string CalAtmosphereTable::getVersion() const {
		return version;
	}
	

	void CalAtmosphereTable::fromXML(string& tableInXML)  {
		//
		// Look for a version information in the schemaVersion of the XML
		//
		xmlDoc *doc;
#if LIBXML_VERSION >= 20703
        doc = xmlReadMemory(tableInXML.data(), tableInXML.size(), "XMLTableHeader.xml", NULL, XML_PARSE_NOBLANKS|XML_PARSE_HUGE);
#else
		doc = xmlReadMemory(tableInXML.data(), tableInXML.size(), "XMLTableHeader.xml", NULL, XML_PARSE_NOBLANKS);
#endif
		if ( doc == NULL )
			throw ConversionException("Failed to parse the xmlHeader into a DOM structure.", "CalAtmosphere");
		
		xmlNode* root_element = xmlDocGetRootElement(doc);
   		if ( root_element == NULL || root_element->type != XML_ELEMENT_NODE )
      		throw ConversionException("Failed to retrieve the root element in the DOM structure.", "CalAtmosphere");
      		
      	xmlChar * propValue = xmlGetProp(root_element, (const xmlChar *) "schemaVersion");
      	if ( propValue != 0 ) {
      		version = string( (const char*) propValue);
      		xmlFree(propValue);   		
      	}
      		     							
		Parser xml(tableInXML);
		if (!xml.isStr("<CalAtmosphereTable")) 
			error();
		// cout << "Parsing a CalAtmosphereTable" << endl;
		string s = xml.getElement("<Entity","/>");
		if (s.length() == 0) 
			error();
		Entity e;
		e.setFromXML(s);
		if (e.getEntityTypeName() != "CalAtmosphereTable")
			error();
		setEntity(e);
		// Skip the container's entity; but, it has to be there.
		s = xml.getElement("<ContainerEntity","/>");
		if (s.length() == 0) 
			error();

		// Get each row in the table.
		s = xml.getElementContent("<row>","</row>");
		CalAtmosphereRow *row;
		if (getContainer().checkRowUniqueness()) {
			try {
				while (s.length() != 0) {
					row = newRow();
					row->setFromXML(s);
					checkAndAdd(row);
					s = xml.getElementContent("<row>","</row>");
				}
				
			}
			catch (const DuplicateKey &e1) {
				throw ConversionException(e1.getMessage(),"CalAtmosphereTable");
			} 
			catch (const UniquenessViolationException &e1) {
				throw ConversionException(e1.getMessage(),"CalAtmosphereTable");	
			}
			catch (...) {
				// cout << "Unexpected error in CalAtmosphereTable::checkAndAdd called from CalAtmosphereTable::fromXML " << endl;
			}
		}
		else {
			try {
				while (s.length() != 0) {
					row = newRow();
					row->setFromXML(s);
					addWithoutCheckingUnique(row);
					s = xml.getElementContent("<row>","</row>");
				}
			}
			catch (const DuplicateKey &e1) {
				throw ConversionException(e1.getMessage(),"CalAtmosphereTable");
			} 
			catch (...) {
				// cout << "Unexpected error in CalAtmosphereTable::addWithoutCheckingUnique called from CalAtmosphereTable::fromXML " << endl;
			}
		}				
				
				
		if (!xml.isStr("</CalAtmosphereTable>")) 
		error();
		
		//Does not change the convention defined in the model.	
		//archiveAsBin = false;
		//fileAsBin = false;

                // clean up the xmlDoc pointer
		if ( doc != NULL ) xmlFreeDoc(doc);
		
	}

	
	void CalAtmosphereTable::error()  {
		throw ConversionException("Invalid xml document","CalAtmosphere");
	}
	
	
	string CalAtmosphereTable::MIMEXMLPart(const asdm::ByteOrder* byteOrder) {
		string UID = getEntity().getEntityId().toString();
		string withoutUID = UID.substr(6);
		string containerUID = getContainer().getEntity().getEntityId().toString();
		ostringstream oss;
		oss << "<?xml version='1.0'  encoding='ISO-8859-1'?>";
		oss << "\n";
		oss << "<CalAtmosphereTable xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:clatm=\"http://Alma/XASDM/CalAtmosphereTable\" xsi:schemaLocation=\"http://Alma/XASDM/CalAtmosphereTable http://almaobservatory.org/XML/XASDM/4/CalAtmosphereTable.xsd\" schemaVersion=\"4\" schemaRevision=\"-1\">\n";
		oss<< "<Entity entityId='"<<UID<<"' entityIdEncrypted='na' entityTypeName='CalAtmosphereTable' schemaVersion='1' documentVersion='1'/>\n";
		oss<< "<ContainerEntity entityId='"<<containerUID<<"' entityIdEncrypted='na' entityTypeName='ASDM' schemaVersion='1' documentVersion='1'/>\n";
		oss << "<BulkStoreRef file_id='"<<withoutUID<<"' byteOrder='"<<byteOrder->toString()<<"' />\n";
		oss << "<Attributes>\n";

		oss << "<antennaName/>\n"; 
		oss << "<receiverBand/>\n"; 
		oss << "<basebandName/>\n"; 
		oss << "<calDataId/>\n"; 
		oss << "<calReductionId/>\n"; 
		oss << "<startValidTime/>\n"; 
		oss << "<endValidTime/>\n"; 
		oss << "<numFreq/>\n"; 
		oss << "<numLoad/>\n"; 
		oss << "<numReceptor/>\n"; 
		oss << "<forwardEffSpectrum/>\n"; 
		oss << "<frequencyRange/>\n"; 
		oss << "<groundPressure/>\n"; 
		oss << "<groundRelHumidity/>\n"; 
		oss << "<frequencySpectrum/>\n"; 
		oss << "<groundTemperature/>\n"; 
		oss << "<polarizationTypes/>\n"; 
		oss << "<powerSkySpectrum/>\n"; 
		oss << "<powerLoadSpectrum/>\n"; 
		oss << "<syscalType/>\n"; 
		oss << "<tAtmSpectrum/>\n"; 
		oss << "<tRecSpectrum/>\n"; 
		oss << "<tSysSpectrum/>\n"; 
		oss << "<tauSpectrum/>\n"; 
		oss << "<tAtm/>\n"; 
		oss << "<tRec/>\n"; 
		oss << "<tSys/>\n"; 
		oss << "<tau/>\n"; 
		oss << "<water/>\n"; 
		oss << "<waterError/>\n"; 

		oss << "<alphaSpectrum/>\n"; 
		oss << "<forwardEfficiency/>\n"; 
		oss << "<forwardEfficiencyError/>\n"; 
		oss << "<sbGain/>\n"; 
		oss << "<sbGainError/>\n"; 
		oss << "<sbGainSpectrum/>\n"; 
		oss << "</Attributes>\n";		
		oss << "</CalAtmosphereTable>\n";

		return oss.str();				
	}
	
	string CalAtmosphereTable::toMIME(const asdm::ByteOrder* byteOrder) {
		EndianOSStream eoss(byteOrder);
		
		string UID = getEntity().getEntityId().toString();
		
		// The MIME Header
		eoss <<"MIME-Version: 1.0";
		eoss << "\n";
		eoss << "Content-Type: Multipart/Related; boundary='MIME_boundary'; type='text/xml'; start= '<header.xml>'";
		eoss <<"\n";
		eoss <<"Content-Description: Correlator";
		eoss <<"\n";
		eoss <<"alma-uid:" << UID;
		eoss <<"\n";
		eoss <<"\n";		
		
		// The MIME XML part header.
		eoss <<"--MIME_boundary";
		eoss <<"\n";
		eoss <<"Content-Type: text/xml; charset='ISO-8859-1'";
		eoss <<"\n";
		eoss <<"Content-Transfer-Encoding: 8bit";
		eoss <<"\n";
		eoss <<"Content-ID: <header.xml>";
		eoss <<"\n";
		eoss <<"\n";
		
		// The MIME XML part content.
		eoss << MIMEXMLPart(byteOrder);

		// The MIME binary part header
		eoss <<"--MIME_boundary";
		eoss <<"\n";
		eoss <<"Content-Type: binary/octet-stream";
		eoss <<"\n";
		eoss <<"Content-ID: <content.bin>";
		eoss <<"\n";
		eoss <<"\n";	
		
		// The MIME binary content
		entity.toBin(eoss);
		container.getEntity().toBin(eoss);
		eoss.writeInt((int) privateRows.size());
		for (unsigned int i = 0; i < privateRows.size(); i++) {
			privateRows.at(i)->toBin(eoss);	
		}
		
		// The closing MIME boundary
		eoss << "\n--MIME_boundary--";
		eoss << "\n";
		
		return eoss.str();	
	}

	
	void CalAtmosphereTable::setFromMIME(const string & mimeMsg) {
    string xmlPartMIMEHeader = "Content-ID: <header.xml>\n\n";
    
    string binPartMIMEHeader = "--MIME_boundary\nContent-Type: binary/octet-stream\nContent-ID: <content.bin>\n\n";
    
    // Detect the XML header.
    string::size_type loc0 = mimeMsg.find(xmlPartMIMEHeader, 0);
    if ( loc0 == string::npos) {
      // let's try with CRLFs
      xmlPartMIMEHeader = "Content-ID: <header.xml>\r\n\r\n";
      loc0 = mimeMsg.find(xmlPartMIMEHeader, 0);
      if  ( loc0 == string::npos ) 
	      throw ConversionException("Failed to detect the beginning of the XML header", "CalAtmosphere");
    }

    loc0 += xmlPartMIMEHeader.size();
    
    // Look for the string announcing the binary part.
    string::size_type loc1 = mimeMsg.find( binPartMIMEHeader, loc0 );
    
    if ( loc1 == string::npos ) {
      throw ConversionException("Failed to detect the beginning of the binary part", "CalAtmosphere");
    }
    
    //
    // Extract the xmlHeader and analyze it to find out what is the byte order and the sequence
    // of attribute names.
    //
    string xmlHeader = mimeMsg.substr(loc0, loc1-loc0);
    xmlDoc *doc;
    doc = xmlReadMemory(xmlHeader.data(), xmlHeader.size(), "BinaryTableHeader.xml", NULL, XML_PARSE_NOBLANKS);
    if ( doc == NULL ) 
      throw ConversionException("Failed to parse the xmlHeader into a DOM structure.", "CalAtmosphere");
    
   // This vector will be filled by the names of  all the attributes of the table
   // in the order in which they are expected to be found in the binary representation.
   //
    vector<string> attributesSeq;
      
    xmlNode* root_element = xmlDocGetRootElement(doc);
    if ( root_element == NULL || root_element->type != XML_ELEMENT_NODE )
      throw ConversionException("Failed to parse the xmlHeader into a DOM structure.", "CalAtmosphere");
    
    const ByteOrder* byteOrder=0;
    if ( string("ASDMBinaryTable").compare((const char*) root_element->name) == 0) {
      // Then it's an "old fashioned" MIME file for tables.
      // Just try to deserialize it with Big_Endian for the bytes ordering.
      byteOrder = asdm::ByteOrder::Big_Endian;
      
 	 //
    // Let's consider a  default order for the sequence of attributes.
    //
    
    	 
    attributesSeq.push_back("antennaName") ; 
    	 
    attributesSeq.push_back("receiverBand") ; 
    	 
    attributesSeq.push_back("basebandName") ; 
    	 
    attributesSeq.push_back("calDataId") ; 
    	 
    attributesSeq.push_back("calReductionId") ; 
    	 
    attributesSeq.push_back("startValidTime") ; 
    	 
    attributesSeq.push_back("endValidTime") ; 
    	 
    attributesSeq.push_back("numFreq") ; 
    	 
    attributesSeq.push_back("numLoad") ; 
    	 
    attributesSeq.push_back("numReceptor") ; 
    	 
    attributesSeq.push_back("forwardEffSpectrum") ; 
    	 
    attributesSeq.push_back("frequencyRange") ; 
    	 
    attributesSeq.push_back("groundPressure") ; 
    	 
    attributesSeq.push_back("groundRelHumidity") ; 
    	 
    attributesSeq.push_back("frequencySpectrum") ; 
    	 
    attributesSeq.push_back("groundTemperature") ; 
    	 
    attributesSeq.push_back("polarizationTypes") ; 
    	 
    attributesSeq.push_back("powerSkySpectrum") ; 
    	 
    attributesSeq.push_back("powerLoadSpectrum") ; 
    	 
    attributesSeq.push_back("syscalType") ; 
    	 
    attributesSeq.push_back("tAtmSpectrum") ; 
    	 
    attributesSeq.push_back("tRecSpectrum") ; 
    	 
    attributesSeq.push_back("tSysSpectrum") ; 
    	 
    attributesSeq.push_back("tauSpectrum") ; 
    	 
    attributesSeq.push_back("tAtm") ; 
    	 
    attributesSeq.push_back("tRec") ; 
    	 
    attributesSeq.push_back("tSys") ; 
    	 
    attributesSeq.push_back("tau") ; 
    	 
    attributesSeq.push_back("water") ; 
    	 
    attributesSeq.push_back("waterError") ; 
    	
    	 
    attributesSeq.push_back("alphaSpectrum") ; 
    	 
    attributesSeq.push_back("forwardEfficiency") ; 
    	 
    attributesSeq.push_back("forwardEfficiencyError") ; 
    	 
    attributesSeq.push_back("sbGain") ; 
    	 
    attributesSeq.push_back("sbGainError") ; 
    	 
    attributesSeq.push_back("sbGainSpectrum") ; 
    	
     
    
    
    // And decide that it has version == "2"
    version = "2";         
     }
    else if (string("CalAtmosphereTable").compare((const char*) root_element->name) == 0) {
      // It's a new (and correct) MIME file for tables.
      //
      // 1st )  Look for a BulkStoreRef element with an attribute byteOrder.
      //
      xmlNode* bulkStoreRef = 0;
      xmlNode* child = root_element->children;
      
      if (xmlHasProp(root_element, (const xmlChar*) "schemaVersion")) {
      	xmlChar * value = xmlGetProp(root_element, (const xmlChar *) "schemaVersion");
      	version = string ((const char *) value);
      	xmlFree(value);	
      }
      
      // Skip the two first children (Entity and ContainerEntity).
      bulkStoreRef = (child ==  0) ? 0 : ( (child->next) == 0 ? 0 : child->next->next );
      
      if ( bulkStoreRef == 0 || (bulkStoreRef->type != XML_ELEMENT_NODE)  || (string("BulkStoreRef").compare((const char*) bulkStoreRef->name) != 0))
      	throw ConversionException ("Could not find the element '/CalAtmosphereTable/BulkStoreRef'. Invalid XML header '"+ xmlHeader + "'.", "CalAtmosphere");
      	
      // We found BulkStoreRef, now look for its attribute byteOrder.
      _xmlAttr* byteOrderAttr = 0;
      for (struct _xmlAttr* attr = bulkStoreRef->properties; attr; attr = attr->next) 
	  if (string("byteOrder").compare((const char*) attr->name) == 0) {
	   byteOrderAttr = attr;
	   break;
	 }
      
      if (byteOrderAttr == 0) 
	     throw ConversionException("Could not find the element '/CalAtmosphereTable/BulkStoreRef/@byteOrder'. Invalid XML header '" + xmlHeader +"'.", "CalAtmosphere");
      
      string byteOrderValue = string((const char*) byteOrderAttr->children->content);
      if (!(byteOrder = asdm::ByteOrder::fromString(byteOrderValue)))
		throw ConversionException("No valid value retrieved for the element '/CalAtmosphereTable/BulkStoreRef/@byteOrder'. Invalid XML header '" + xmlHeader + "'.", "CalAtmosphere");
		
	 //
	 // 2nd) Look for the Attributes element and grab the names of the elements it contains.
	 //
	 xmlNode* attributes = bulkStoreRef->next;
     if ( attributes == 0 || (attributes->type != XML_ELEMENT_NODE)  || (string("Attributes").compare((const char*) attributes->name) != 0))	 
       	throw ConversionException ("Could not find the element '/CalAtmosphereTable/Attributes'. Invalid XML header '"+ xmlHeader + "'.", "CalAtmosphere");
 
 	xmlNode* childOfAttributes = attributes->children;
 	
 	while ( childOfAttributes != 0 && (childOfAttributes->type == XML_ELEMENT_NODE) ) {
 		attributesSeq.push_back(string((const char*) childOfAttributes->name));
 		childOfAttributes = childOfAttributes->next;
    }
    }
    // Create an EndianISStream from the substring containing the binary part.
    EndianISStream eiss(mimeMsg.substr(loc1+binPartMIMEHeader.size()), byteOrder);
    
    entity = Entity::fromBin((EndianIStream&) eiss);
    
    // We do nothing with that but we have to read it.
    Entity containerEntity = Entity::fromBin((EndianIStream&) eiss);

	// Let's read numRows but ignore it and rely on the value specified in the ASDM.xml file.    
    int numRows = ((EndianIStream&) eiss).readInt();
    if ((numRows != -1)                        // Then these are *not* data produced at the EVLA.
    	&& ((unsigned int) numRows != this->declaredSize )) { // Then the declared size (in ASDM.xml) is not equal to the one 
    	                                       // written into the binary representation of the table.
		cout << "The a number of rows ('" 
			 << numRows
			 << "') declared in the binary representation of the table is different from the one declared in ASDM.xml ('"
			 << this->declaredSize
			 << "'). I'll proceed with the value declared in ASDM.xml"
			 << endl;
    }                                           

	if (getContainer().checkRowUniqueness()) {
    	try {
      		for (uint32_t i = 0; i < this->declaredSize; i++) {
				CalAtmosphereRow* aRow = CalAtmosphereRow::fromBin((EndianIStream&) eiss, *this, attributesSeq);
				checkAndAdd(aRow);
      		}
    	}
    	catch (const DuplicateKey &e) {
      		throw ConversionException("Error while writing binary data , the message was "
				+ e.getMessage(), "CalAtmosphere");
    	}
    	catch (const TagFormatException &e) {
     		 throw ConversionException("Error while reading binary data , the message was "
				+ e.getMessage(), "CalAtmosphere");
    	}
    }
    else {
 		for (uint32_t i = 0; i < this->declaredSize; i++) {
			CalAtmosphereRow* aRow = CalAtmosphereRow::fromBin((EndianIStream&) eiss, *this, attributesSeq);
			append(aRow);
      	}   	
    }
    //Does not change the convention defined in the model.	
    //archiveAsBin = true;
    //fileAsBin = true;
    if ( doc != NULL ) xmlFreeDoc(doc);

	}
	
	void CalAtmosphereTable::setUnknownAttributeBinaryReader(const string& attributeName, BinaryAttributeReaderFunctor* barFctr) {
		//
		// Is this attribute really unknown ?
		//
		for (vector<string>::const_iterator iter = attributesNamesOfCalAtmosphere_v.begin(); iter != attributesNamesOfCalAtmosphere_v.end(); iter++) {
			if ((*iter).compare(attributeName) == 0) 
				throw ConversionException("the attribute '"+attributeName+"' is known you can't override the way it's read in the MIME binary file containing the table.", "CalAtmosphere"); 
		}
		
		// Ok then register the functor to activate when an unknown attribute is met during the reading of a binary table?
		unknownAttributes2Functors[attributeName] = barFctr;
	}
	
	BinaryAttributeReaderFunctor* CalAtmosphereTable::getUnknownAttributeBinaryReader(const string& attributeName) const {
		map<string, BinaryAttributeReaderFunctor*>::const_iterator iter = unknownAttributes2Functors.find(attributeName);
		return (iter == unknownAttributes2Functors.end()) ? 0 : iter->second;
	}

	
	void CalAtmosphereTable::toFile(string directory) {
		if (!directoryExists(directory.c_str()) &&
			!createPath(directory.c_str())) {
			throw ConversionException("Could not create directory " , directory);
		}

		string fileName = directory + "/CalAtmosphere.xml";
		ofstream tableout(fileName.c_str(),ios::out|ios::trunc);
		if (tableout.rdstate() == ostream::failbit)
			throw ConversionException("Could not open file " + fileName + " to write ", "CalAtmosphere");
		if (fileAsBin) 
			tableout << MIMEXMLPart();
		else
			tableout << toXML() << endl;
		tableout.close();
		if (tableout.rdstate() == ostream::failbit)
			throw ConversionException("Could not close file " + fileName, "CalAtmosphere");

		if (fileAsBin) {
			// write the bin serialized
			string fileName = directory + "/CalAtmosphere.bin";
			ofstream tableout(fileName.c_str(),ios::out|ios::trunc);
			if (tableout.rdstate() == ostream::failbit)
				throw ConversionException("Could not open file " + fileName + " to write ", "CalAtmosphere");
			tableout << toMIME() << endl;
			tableout.close();
			if (tableout.rdstate() == ostream::failbit)
				throw ConversionException("Could not close file " + fileName, "CalAtmosphere");
		}
	}

	
	void CalAtmosphereTable::setFromFile(const string& directory) {
#ifndef WITHOUT_BOOST
    if (boost::filesystem::exists(boost::filesystem::path(uniqSlashes(directory + "/CalAtmosphere.xml"))))
      setFromXMLFile(directory);
    else if (boost::filesystem::exists(boost::filesystem::path(uniqSlashes(directory + "/CalAtmosphere.bin"))))
      setFromMIMEFile(directory);
#else 
    // alternative in Misc.h
    if (file_exists(uniqSlashes(directory + "/CalAtmosphere.xml")))
      setFromXMLFile(directory);
    else if (file_exists(uniqSlashes(directory + "/CalAtmosphere.bin")))
      setFromMIMEFile(directory);
#endif
    else
      throw ConversionException("No file found for the CalAtmosphere table", "CalAtmosphere");
	}			

	
  void CalAtmosphereTable::setFromMIMEFile(const string& directory) {
    string tablePath ;
    
    tablePath = directory + "/CalAtmosphere.bin";
    ifstream tablefile(tablePath.c_str(), ios::in|ios::binary);
    if (!tablefile.is_open()) { 
      throw ConversionException("Could not open file " + tablePath, "CalAtmosphere");
    }
    // Read in a stringstream.
    stringstream ss; ss << tablefile.rdbuf();
    
    if (tablefile.rdstate() == istream::failbit || tablefile.rdstate() == istream::badbit) {
      throw ConversionException("Error reading file " + tablePath,"CalAtmosphere");
    }
    
    // And close.
    tablefile.close();
    if (tablefile.rdstate() == istream::failbit)
      throw ConversionException("Could not close file " + tablePath,"CalAtmosphere");
    
    setFromMIME(ss.str());
  }	
/* 
  void CalAtmosphereTable::openMIMEFile (const string& directory) {
  		
  	// Open the file.
  	string tablePath ;
    tablePath = directory + "/CalAtmosphere.bin";
    ifstream tablefile(tablePath.c_str(), ios::in|ios::binary);
    if (!tablefile.is_open())
      throw ConversionException("Could not open file " + tablePath, "CalAtmosphere");
      
	// Locate the xmlPartMIMEHeader.
    string xmlPartMIMEHeader = "CONTENT-ID: <HEADER.XML>\n\n";
    CharComparator comparator;
    istreambuf_iterator<char> BEGIN(tablefile.rdbuf());
    istreambuf_iterator<char> END;
    istreambuf_iterator<char> it = search(BEGIN, END, xmlPartMIMEHeader.begin(), xmlPartMIMEHeader.end(), comparator);
    if (it == END) 
    	throw ConversionException("failed to detect the beginning of the XML header", "CalAtmosphere");
    
    // Locate the binaryPartMIMEHeader while accumulating the characters of the xml header.	
    string binPartMIMEHeader = "--MIME_BOUNDARY\nCONTENT-TYPE: BINARY/OCTET-STREAM\nCONTENT-ID: <CONTENT.BIN>\n\n";
    string xmlHeader;
   	CharCompAccumulator compaccumulator(&xmlHeader, 100000);
   	++it;
   	it = search(it, END, binPartMIMEHeader.begin(), binPartMIMEHeader.end(), compaccumulator);
   	if (it == END) 
   		throw ConversionException("failed to detect the beginning of the binary part", "CalAtmosphere");
   	
	cout << xmlHeader << endl;
	//
	// We have the xmlHeader , let's parse it.
	//
	xmlDoc *doc;
    doc = xmlReadMemory(xmlHeader.data(), xmlHeader.size(), "BinaryTableHeader.xml", NULL, XML_PARSE_NOBLANKS);
    if ( doc == NULL ) 
      throw ConversionException("Failed to parse the xmlHeader into a DOM structure.", "CalAtmosphere");
    
   // This vector will be filled by the names of  all the attributes of the table
   // in the order in which they are expected to be found in the binary representation.
   //
    vector<string> attributesSeq(attributesNamesInBinOfCalAtmosphere_v);
      
    xmlNode* root_element = xmlDocGetRootElement(doc);
    if ( root_element == NULL || root_element->type != XML_ELEMENT_NODE )
      throw ConversionException("Failed to parse the xmlHeader into a DOM structure.", "CalAtmosphere");
    
    const ByteOrder* byteOrder=0;
    if ( string("ASDMBinaryTable").compare((const char*) root_element->name) == 0) {
      // Then it's an "old fashioned" MIME file for tables.
      // Just try to deserialize it with Big_Endian for the bytes ordering.
      byteOrder = asdm::ByteOrder::Big_Endian;
        
      // And decide that it has version == "2"
    version = "2";         
     }
    else if (string("CalAtmosphereTable").compare((const char*) root_element->name) == 0) {
      // It's a new (and correct) MIME file for tables.
      //
      // 1st )  Look for a BulkStoreRef element with an attribute byteOrder.
      //
      xmlNode* bulkStoreRef = 0;
      xmlNode* child = root_element->children;
      
      if (xmlHasProp(root_element, (const xmlChar*) "schemaVersion")) {
      	xmlChar * value = xmlGetProp(root_element, (const xmlChar *) "schemaVersion");
      	version = string ((const char *) value);
      	xmlFree(value);	
      }
      
      // Skip the two first children (Entity and ContainerEntity).
      bulkStoreRef = (child ==  0) ? 0 : ( (child->next) == 0 ? 0 : child->next->next );
      
      if ( bulkStoreRef == 0 || (bulkStoreRef->type != XML_ELEMENT_NODE)  || (string("BulkStoreRef").compare((const char*) bulkStoreRef->name) != 0))
      	throw ConversionException ("Could not find the element '/CalAtmosphereTable/BulkStoreRef'. Invalid XML header '"+ xmlHeader + "'.", "CalAtmosphere");
      	
      // We found BulkStoreRef, now look for its attribute byteOrder.
      _xmlAttr* byteOrderAttr = 0;
      for (struct _xmlAttr* attr = bulkStoreRef->properties; attr; attr = attr->next) 
	  if (string("byteOrder").compare((const char*) attr->name) == 0) {
	   byteOrderAttr = attr;
	   break;
	 }
      
      if (byteOrderAttr == 0) 
	     throw ConversionException("Could not find the element '/CalAtmosphereTable/BulkStoreRef/@byteOrder'. Invalid XML header '" + xmlHeader +"'.", "CalAtmosphere");
      
      string byteOrderValue = string((const char*) byteOrderAttr->children->content);
      if (!(byteOrder = asdm::ByteOrder::fromString(byteOrderValue)))
		throw ConversionException("No valid value retrieved for the element '/CalAtmosphereTable/BulkStoreRef/@byteOrder'. Invalid XML header '" + xmlHeader + "'.", "CalAtmosphere");
		
	 //
	 // 2nd) Look for the Attributes element and grab the names of the elements it contains.
	 //
	 xmlNode* attributes = bulkStoreRef->next;
     if ( attributes == 0 || (attributes->type != XML_ELEMENT_NODE)  || (string("Attributes").compare((const char*) attributes->name) != 0))	 
       	throw ConversionException ("Could not find the element '/CalAtmosphereTable/Attributes'. Invalid XML header '"+ xmlHeader + "'.", "CalAtmosphere");
 
 	xmlNode* childOfAttributes = attributes->children;
 	
 	while ( childOfAttributes != 0 && (childOfAttributes->type == XML_ELEMENT_NODE) ) {
 		attributesSeq.push_back(string((const char*) childOfAttributes->name));
 		childOfAttributes = childOfAttributes->next;
    }
    }
    // Create an EndianISStream from the substring containing the binary part.
    EndianIFStream eifs(&tablefile, byteOrder);
    
    entity = Entity::fromBin((EndianIStream &) eifs);
    
    // We do nothing with that but we have to read it.
    Entity containerEntity = Entity::fromBin((EndianIStream &) eifs);

	// Let's read numRows but ignore it and rely on the value specified in the ASDM.xml file.    
    int numRows = eifs.readInt();
    if ((numRows != -1)                        // Then these are *not* data produced at the EVLA.
    	&& ((unsigned int) numRows != this->declaredSize )) { // Then the declared size (in ASDM.xml) is not equal to the one 
    	                                       // written into the binary representation of the table.
		cout << "The a number of rows ('" 
			 << numRows
			 << "') declared in the binary representation of the table is different from the one declared in ASDM.xml ('"
			 << this->declaredSize
			 << "'). I'll proceed with the value declared in ASDM.xml"
			 << endl;
    }
    // clean up xmlDoc pointer
    if ( doc != NULL ) xmlFreeDoc(doc);    
  } 
 */

	
void CalAtmosphereTable::setFromXMLFile(const string& directory) {
    string tablePath ;
    
    tablePath = directory + "/CalAtmosphere.xml";
    
    /*
    ifstream tablefile(tablePath.c_str(), ios::in|ios::binary);
    if (!tablefile.is_open()) { 
      throw ConversionException("Could not open file " + tablePath, "CalAtmosphere");
    }
      // Read in a stringstream.
    stringstream ss;
    ss << tablefile.rdbuf();
    
    if  (tablefile.rdstate() == istream::failbit || tablefile.rdstate() == istream::badbit) {
      throw ConversionException("Error reading file '" + tablePath + "'", "CalAtmosphere");
    }
    
    // And close
    tablefile.close();
    if (tablefile.rdstate() == istream::failbit)
      throw ConversionException("Could not close file '" + tablePath + "'", "CalAtmosphere");

    // Let's make a string out of the stringstream content and empty the stringstream.
    string xmlDocument = ss.str(); ss.str("");
	
    // Let's make a very primitive check to decide
    // whether the XML content represents the table
    // or refers to it via a <BulkStoreRef element.
    */
    
    string xmlDocument;
    try {
    	xmlDocument = getContainer().getXSLTransformer()(tablePath);
    	if (getenv("ASDM_DEBUG")) cout << "About to read " << tablePath << endl;
    }
    catch (const XSLTransformerException &e) {
    	throw ConversionException("Caugth an exception whose message is '" + e.getMessage() + "'.", "CalAtmosphere");
    }
    
    if (xmlDocument.find("<BulkStoreRef") != string::npos)
      setFromMIMEFile(directory);
    else
      fromXML(xmlDocument);
  }

	

	

			
	
	

	
} // End namespace asdm
 
