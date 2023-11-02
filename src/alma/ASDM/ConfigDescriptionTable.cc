
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
 * File ConfigDescriptionTable.cpp
 */
#include <alma/ASDM/ConversionException.h>
#include <alma/ASDM/DuplicateKey.h>
#include <alma/ASDM/OutOfBoundsException.h>

using asdm::ConversionException;
using asdm::DuplicateKey;
using asdm::OutOfBoundsException;

#include <alma/ASDM/ASDM.h>
#include <alma/ASDM/ConfigDescriptionTable.h>
#include <alma/ASDM/ConfigDescriptionRow.h>
#include <alma/ASDM/Parser.h>

using asdm::ASDM;
using asdm::ConfigDescriptionTable;
using asdm::ConfigDescriptionRow;
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
	static string entityNameOfConfigDescription = "ConfigDescription";
	
	// An array of string containing the names of the columns of this table.
	// The array is filled in the order : key, required value, optional value.
	//
	static string attributesNamesOfConfigDescription_a[] = {
		
			"configDescriptionId"
		
		
			, "numAntenna"
		
			, "numDataDescription"
		
			, "numFeed"
		
			, "correlationMode"
		
			, "numAtmPhaseCorrection"
		
			, "atmPhaseCorrection"
		
			, "processorType"
		
			, "spectralType"
		
			, "antennaId"
		
			, "feedId"
		
			, "switchCycleId"
		
			, "dataDescriptionId"
		
			, "processorId"
				
		
			, "phasedArrayList"
		
			, "numAssocValues"
		
			, "assocNature"
		
			, "assocConfigDescriptionId"
				
	};
	
	// A vector of string whose content is a copy of the strings in the array above.
	//
	static vector<string> attributesNamesOfConfigDescription_v (attributesNamesOfConfigDescription_a, attributesNamesOfConfigDescription_a + sizeof(attributesNamesOfConfigDescription_a) / sizeof(attributesNamesOfConfigDescription_a[0]));

	// An array of string containing the names of the columns of this table.
	// The array is filled in the order where the names would be read by default in the XML header of a file containing
	// the table exported in binary mode.
	//	
	static string attributesNamesInBinOfConfigDescription_a[] = {
    
    	 "configDescriptionId" , "numAntenna" , "numDataDescription" , "numFeed" , "correlationMode" , "numAtmPhaseCorrection" , "atmPhaseCorrection" , "processorType" , "spectralType" , "antennaId" , "feedId" , "switchCycleId" , "dataDescriptionId" , "processorId" 
    	,
    	 "phasedArrayList" , "numAssocValues" , "assocNature" , "assocConfigDescriptionId" 
    
	};
	        			
	// A vector of string whose content is a copy of the strings in the array above.
	//
	static vector<string> attributesNamesInBinOfConfigDescription_v(attributesNamesInBinOfConfigDescription_a, attributesNamesInBinOfConfigDescription_a + sizeof(attributesNamesInBinOfConfigDescription_a) / sizeof(attributesNamesInBinOfConfigDescription_a[0]));		
	

	// The array of attributes (or column) names that make up key key.
	//
	string keyOfConfigDescription_a[] = {
	
		"configDescriptionId"
		 
	};
	 
	// A vector of strings which are copies of those stored in the array above.
	vector<string> keyOfConfigDescription_v(keyOfConfigDescription_a, keyOfConfigDescription_a + sizeof(keyOfConfigDescription_a) / sizeof(keyOfConfigDescription_a[0]));

	/**
	 * Return the list of field names that make up key key
	 * as a const reference to a vector of strings.
	 */	
	const vector<string>& ConfigDescriptionTable::getKeyName() {
		return keyOfConfigDescription_v;
	}


	ConfigDescriptionTable::ConfigDescriptionTable(ASDM &c) : container(c) {

		// Define a default entity.
		entity.setEntityId(EntityId("uid://X0/X0/X0"));
		entity.setEntityIdEncrypted("na");
		entity.setEntityTypeName("ConfigDescriptionTable");
		entity.setEntityVersion("1");
		entity.setInstanceVersion("1");
		
		// Archive XML
		archiveAsBin = false;
		
		// File XML
		fileAsBin = false;
		
		// By default the table is considered as present in memory
		presentInMemory = true;
		
		// By default there is no load in progress
		loadInProgress = false;
	}
	
/**
 * A destructor for ConfigDescriptionTable.
 */
	ConfigDescriptionTable::~ConfigDescriptionTable() {
		for (unsigned int i = 0; i < privateRows.size(); i++) 
			delete(privateRows.at(i));
	}

	/**
	 * Container to which this table belongs.
	 */
	ASDM &ConfigDescriptionTable::getContainer() const {
		return container;
	}

	/**
	 * Return the number of rows in the table.
	 */
	unsigned int ConfigDescriptionTable::size() const {
		if (presentInMemory) 
			return privateRows.size();
		else
			return declaredSize;
	}
	
	/**
	 * Return the name of this table.
	 */
	string ConfigDescriptionTable::getName() const {
		return entityNameOfConfigDescription;
	}
	
	/**
	 * Return the name of this table.
	 */
	string ConfigDescriptionTable::name() {
		return entityNameOfConfigDescription;
	}
	
	/**
	 * Return the the names of the attributes (or columns) of this table.
	 */
	const vector<string>& ConfigDescriptionTable::getAttributesNames() { return attributesNamesOfConfigDescription_v; }
	
	/**
	 * Return the the names of the attributes (or columns) of this table as they appear by default
	 * in an binary export of this table.
	 */
	const vector<string>& ConfigDescriptionTable::defaultAttributesNamesInBin() { return attributesNamesInBinOfConfigDescription_v; }

	/**
	 * Return this table's Entity.
	 */
	Entity ConfigDescriptionTable::getEntity() const {
		return entity;
	}

	/**
	 * Set this table's Entity.
	 */
	void ConfigDescriptionTable::setEntity(Entity e) {
		this->entity = e; 
	}
	
	//
	// ====> Row creation.
	//
	
	/**
	 * Create a new row.
	 */
	ConfigDescriptionRow *ConfigDescriptionTable::newRow() {
		return new ConfigDescriptionRow (*this);
	}
	

	/**
	 * Create a new row initialized to the specified values.
	 * @return a pointer on the created and initialized row.
	
 	 * @param numAntenna 
	
 	 * @param numDataDescription 
	
 	 * @param numFeed 
	
 	 * @param correlationMode 
	
 	 * @param numAtmPhaseCorrection 
	
 	 * @param atmPhaseCorrection 
	
 	 * @param processorType 
	
 	 * @param spectralType 
	
 	 * @param antennaId 
	
 	 * @param feedId 
	
 	 * @param switchCycleId 
	
 	 * @param dataDescriptionId 
	
 	 * @param processorId 
	
     */
	ConfigDescriptionRow* ConfigDescriptionTable::newRow(int numAntenna, int numDataDescription, int numFeed, CorrelationModeMod::CorrelationMode correlationMode, int numAtmPhaseCorrection, std::vector<AtmPhaseCorrectionMod::AtmPhaseCorrection > atmPhaseCorrection, ProcessorTypeMod::ProcessorType processorType, SpectralResolutionTypeMod::SpectralResolutionType spectralType, std::vector<Tag>  antennaId, std::vector<int>  feedId, std::vector<Tag>  switchCycleId, std::vector<Tag>  dataDescriptionId, Tag processorId){
		ConfigDescriptionRow *row = new ConfigDescriptionRow(*this);
			
		row->setNumAntenna(numAntenna);
			
		row->setNumDataDescription(numDataDescription);
			
		row->setNumFeed(numFeed);
			
		row->setCorrelationMode(correlationMode);
			
		row->setNumAtmPhaseCorrection(numAtmPhaseCorrection);
			
		row->setAtmPhaseCorrection(atmPhaseCorrection);
			
		row->setProcessorType(processorType);
			
		row->setSpectralType(spectralType);
			
		row->setAntennaId(antennaId);
			
		row->setFeedId(feedId);
			
		row->setSwitchCycleId(switchCycleId);
			
		row->setDataDescriptionId(dataDescriptionId);
			
		row->setProcessorId(processorId);
	
		return row;		
	}	
	


ConfigDescriptionRow* ConfigDescriptionTable::newRow(ConfigDescriptionRow* row) {
	return new ConfigDescriptionRow(*this, row);
}

	//
	// Append a row to its table.
	//

	
	 
	
	/** 
 	 * Look up the table for a row whose noautoincrementable attributes are matching their
 	 * homologues in *x.  If a row is found  this row else autoincrement  *x.configDescriptionId, 
 	 * add x to its table and returns x.
 	 *  
 	 * @returns a pointer on a ConfigDescriptionRow.
 	 * @param x. A pointer on the row to be added.
 	 */ 
 		
			
	ConfigDescriptionRow* ConfigDescriptionTable::add(ConfigDescriptionRow* x) {
			 
		ConfigDescriptionRow* aRow = lookup(
				
		x->getNumAntenna()
				,
		x->getNumDataDescription()
				,
		x->getNumFeed()
				,
		x->getCorrelationMode()
				,
		x->getNumAtmPhaseCorrection()
				,
		x->getAtmPhaseCorrection()
				,
		x->getProcessorType()
				,
		x->getSpectralType()
				,
		x->getAntennaId()
				,
		x->getFeedId()
				,
		x->getSwitchCycleId()
				,
		x->getDataDescriptionId()
				,
		x->getProcessorId()
				
		);
		if (aRow) return aRow;
			

			
		// Autoincrement configDescriptionId
		x->setConfigDescriptionId(Tag(size(), TagType::ConfigDescription));
						
		row.push_back(x);
		privateRows.push_back(x);
		x->isAdded(true);
		return x;
	}
		
	
		
	void ConfigDescriptionTable::addWithoutCheckingUnique(ConfigDescriptionRow * x) {
		if (getRowByKey(
						x->getConfigDescriptionId()
						) != (ConfigDescriptionRow *) 0) 
			throw DuplicateKey("Dupicate key exception in ", "ConfigDescriptionTable");
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
	 
	 * @throws UniquenessViolationException
	 
	 */
	ConfigDescriptionRow*  ConfigDescriptionTable::checkAndAdd(ConfigDescriptionRow* x, bool skipCheckUniqueness)  {
		if (!skipCheckUniqueness) { 
	 
		 
			if (lookup(
			
				x->getNumAntenna()
		,
				x->getNumDataDescription()
		,
				x->getNumFeed()
		,
				x->getCorrelationMode()
		,
				x->getNumAtmPhaseCorrection()
		,
				x->getAtmPhaseCorrection()
		,
				x->getProcessorType()
		,
				x->getSpectralType()
		,
				x->getAntennaId()
		,
				x->getFeedId()
		,
				x->getSwitchCycleId()
		,
				x->getDataDescriptionId()
		,
				x->getProcessorId()
		
			)) throw UniquenessViolationException();
		
		
		}
		
		if (getRowByKey(
	
			x->getConfigDescriptionId()
			
		)) throw DuplicateKey("Duplicate key exception in ", "ConfigDescriptionTable");
		
		row.push_back(x);
		privateRows.push_back(x);
		x->isAdded(true);
		return x;	
	}	



	//
	// A private method to brutally append a row to its table, without checking for row uniqueness.
	//

	void ConfigDescriptionTable::append(ConfigDescriptionRow *x) {
		privateRows.push_back(x);
		x->isAdded(true);
	}





	 vector<ConfigDescriptionRow *> ConfigDescriptionTable::get() {
	 	checkPresenceInMemory();
	    return privateRows;
	 }
	 
	 const vector<ConfigDescriptionRow *>& ConfigDescriptionTable::get() const {
	 	const_cast<ConfigDescriptionTable&>(*this).checkPresenceInMemory();	
	    return privateRows;
	 }	 
	 	




	

	
/*
 ** Returns a ConfigDescriptionRow* given a key.
 ** @return a pointer to the row having the key whose values are passed as parameters, or 0 if
 ** no row exists for that key.
 **
 */
 	ConfigDescriptionRow* ConfigDescriptionTable::getRowByKey(Tag configDescriptionId)  {
 	checkPresenceInMemory();
	ConfigDescriptionRow* aRow = 0;
	for (unsigned int i = 0; i < privateRows.size(); i++) {
		aRow = row.at(i);
		
			
				if (aRow->configDescriptionId != configDescriptionId) continue;
			
		
		return aRow;
	}
	return 0;		
}
	

	
/**
 * Look up the table for a row whose all attributes  except the autoincrementable one 
 * are equal to the corresponding parameters of the method.
 * @return a pointer on this row if any, 0 otherwise.
 *
			
 * @param numAntenna.
 	 		
 * @param numDataDescription.
 	 		
 * @param numFeed.
 	 		
 * @param correlationMode.
 	 		
 * @param numAtmPhaseCorrection.
 	 		
 * @param atmPhaseCorrection.
 	 		
 * @param processorType.
 	 		
 * @param spectralType.
 	 		
 * @param antennaId.
 	 		
 * @param feedId.
 	 		
 * @param switchCycleId.
 	 		
 * @param dataDescriptionId.
 	 		
 * @param processorId.
 	 		 
 */
ConfigDescriptionRow* ConfigDescriptionTable::lookup(int numAntenna, int numDataDescription, int numFeed, CorrelationModeMod::CorrelationMode correlationMode, int numAtmPhaseCorrection, std::vector<AtmPhaseCorrectionMod::AtmPhaseCorrection > atmPhaseCorrection, ProcessorTypeMod::ProcessorType processorType, SpectralResolutionTypeMod::SpectralResolutionType spectralType, std::vector<Tag>  antennaId, std::vector<int>  feedId, std::vector<Tag>  switchCycleId, std::vector<Tag>  dataDescriptionId, Tag processorId) {
		ConfigDescriptionRow* aRow;
		for (unsigned int i = 0; i < privateRows.size(); i++) {
			aRow = privateRows.at(i); 
			if (aRow->compareNoAutoInc(numAntenna, numDataDescription, numFeed, correlationMode, numAtmPhaseCorrection, atmPhaseCorrection, processorType, spectralType, antennaId, feedId, switchCycleId, dataDescriptionId, processorId)) return aRow;
		}			
		return 0;	
} 
	
 	 	

	



#ifndef WITHOUT_ACS
	using asdmIDL::ConfigDescriptionTableIDL;
#endif

#ifndef WITHOUT_ACS
	// Conversion Methods

	ConfigDescriptionTableIDL *ConfigDescriptionTable::toIDL() {
		ConfigDescriptionTableIDL *x = new ConfigDescriptionTableIDL ();
		unsigned int nrow = size();
		x->row.length(nrow);
		vector<ConfigDescriptionRow*> v = get();
		for (unsigned int i = 0; i < nrow; ++i) {
			//x->row[i] = *(v[i]->toIDL());
			v[i]->toIDL(x->row[i]);
		}
		return x;
	}
	
	void ConfigDescriptionTable::toIDL(asdmIDL::ConfigDescriptionTableIDL& x) const {
		unsigned int nrow = size();
		x.row.length(nrow);
		vector<ConfigDescriptionRow*> v = get();
		for (unsigned int i = 0; i < nrow; ++i) {
			v[i]->toIDL(x.row[i]);
		}
	}	
#endif
	
#ifndef WITHOUT_ACS
	void ConfigDescriptionTable::fromIDL(ConfigDescriptionTableIDL x) {
		unsigned int nrow = x.row.length();
		for (unsigned int i = 0; i < nrow; ++i) {
			ConfigDescriptionRow *tmp = newRow();
			tmp->setFromIDL(x.row[i]);
			// checkAndAdd(tmp);
			add(tmp);
		}
	}	
#endif

	
	string ConfigDescriptionTable::toXML()  {
		string buf;

		buf.append("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?> ");
		buf.append("<ConfigDescriptionTable xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:cnfdsc=\"http://Alma/XASDM/ConfigDescriptionTable\" xsi:schemaLocation=\"http://Alma/XASDM/ConfigDescriptionTable http://almaobservatory.org/XML/XASDM/4/ConfigDescriptionTable.xsd\" schemaVersion=\"4\" schemaRevision=\"-1\">\n");
	
		buf.append(entity.toXML());
		string s = container.getEntity().toXML();
		// Change the "Entity" tag to "ContainerEntity".
		buf.append("<Container" + s.substr(1,s.length() - 1)+" ");
		vector<ConfigDescriptionRow*> v = get();
		for (unsigned int i = 0; i < v.size(); ++i) {
			try {
				buf.append(v[i]->toXML());
			} catch (const NoSuchRow &e) {
			}
			buf.append("  ");
		}		
		buf.append("</ConfigDescriptionTable> ");
		return buf;
	}

	
	string ConfigDescriptionTable::getVersion() const {
		return version;
	}
	

	void ConfigDescriptionTable::fromXML(string& tableInXML)  {
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
			throw ConversionException("Failed to parse the xmlHeader into a DOM structure.", "ConfigDescription");
		
		xmlNode* root_element = xmlDocGetRootElement(doc);
   		if ( root_element == NULL || root_element->type != XML_ELEMENT_NODE )
      		throw ConversionException("Failed to retrieve the root element in the DOM structure.", "ConfigDescription");
      		
      	xmlChar * propValue = xmlGetProp(root_element, (const xmlChar *) "schemaVersion");
      	if ( propValue != 0 ) {
      		version = string( (const char*) propValue);
      		xmlFree(propValue);   		
      	}
      		     							
		Parser xml(tableInXML);
		if (!xml.isStr("<ConfigDescriptionTable")) 
			error();
		// cout << "Parsing a ConfigDescriptionTable" << endl;
		string s = xml.getElement("<Entity","/>");
		if (s.length() == 0) 
			error();
		Entity e;
		e.setFromXML(s);
		if (e.getEntityTypeName() != "ConfigDescriptionTable")
			error();
		setEntity(e);
		// Skip the container's entity; but, it has to be there.
		s = xml.getElement("<ContainerEntity","/>");
		if (s.length() == 0) 
			error();

		// Get each row in the table.
		s = xml.getElementContent("<row>","</row>");
		ConfigDescriptionRow *row;
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
				throw ConversionException(e1.getMessage(),"ConfigDescriptionTable");
			} 
			catch (const UniquenessViolationException &e1) {
				throw ConversionException(e1.getMessage(),"ConfigDescriptionTable");	
			}
			catch (...) {
				// cout << "Unexpected error in ConfigDescriptionTable::checkAndAdd called from ConfigDescriptionTable::fromXML " << endl;
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
				throw ConversionException(e1.getMessage(),"ConfigDescriptionTable");
			} 
			catch (...) {
				// cout << "Unexpected error in ConfigDescriptionTable::addWithoutCheckingUnique called from ConfigDescriptionTable::fromXML " << endl;
			}
		}				
				
				
		if (!xml.isStr("</ConfigDescriptionTable>")) 
		error();
		
		//Does not change the convention defined in the model.	
		//archiveAsBin = false;
		//fileAsBin = false;

                // clean up the xmlDoc pointer
		if ( doc != NULL ) xmlFreeDoc(doc);
		
	}

	
	void ConfigDescriptionTable::error()  {
		throw ConversionException("Invalid xml document","ConfigDescription");
	}
	
	
	string ConfigDescriptionTable::MIMEXMLPart(const asdm::ByteOrder* byteOrder) {
		string UID = getEntity().getEntityId().toString();
		string withoutUID = UID.substr(6);
		string containerUID = getContainer().getEntity().getEntityId().toString();
		ostringstream oss;
		oss << "<?xml version='1.0'  encoding='ISO-8859-1'?>";
		oss << "\n";
		oss << "<ConfigDescriptionTable xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:cnfdsc=\"http://Alma/XASDM/ConfigDescriptionTable\" xsi:schemaLocation=\"http://Alma/XASDM/ConfigDescriptionTable http://almaobservatory.org/XML/XASDM/4/ConfigDescriptionTable.xsd\" schemaVersion=\"4\" schemaRevision=\"-1\">\n";
		oss<< "<Entity entityId='"<<UID<<"' entityIdEncrypted='na' entityTypeName='ConfigDescriptionTable' schemaVersion='1' documentVersion='1'/>\n";
		oss<< "<ContainerEntity entityId='"<<containerUID<<"' entityIdEncrypted='na' entityTypeName='ASDM' schemaVersion='1' documentVersion='1'/>\n";
		oss << "<BulkStoreRef file_id='"<<withoutUID<<"' byteOrder='"<<byteOrder->toString()<<"' />\n";
		oss << "<Attributes>\n";

		oss << "<configDescriptionId/>\n"; 
		oss << "<numAntenna/>\n"; 
		oss << "<numDataDescription/>\n"; 
		oss << "<numFeed/>\n"; 
		oss << "<correlationMode/>\n"; 
		oss << "<numAtmPhaseCorrection/>\n"; 
		oss << "<atmPhaseCorrection/>\n"; 
		oss << "<processorType/>\n"; 
		oss << "<spectralType/>\n"; 
		oss << "<antennaId/>\n"; 
		oss << "<feedId/>\n"; 
		oss << "<switchCycleId/>\n"; 
		oss << "<dataDescriptionId/>\n"; 
		oss << "<processorId/>\n"; 

		oss << "<phasedArrayList/>\n"; 
		oss << "<numAssocValues/>\n"; 
		oss << "<assocNature/>\n"; 
		oss << "<assocConfigDescriptionId/>\n"; 
		oss << "</Attributes>\n";		
		oss << "</ConfigDescriptionTable>\n";

		return oss.str();				
	}
	
	string ConfigDescriptionTable::toMIME(const asdm::ByteOrder* byteOrder) {
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

	
	void ConfigDescriptionTable::setFromMIME(const string & mimeMsg) {
    string xmlPartMIMEHeader = "Content-ID: <header.xml>\n\n";
    
    string binPartMIMEHeader = "--MIME_boundary\nContent-Type: binary/octet-stream\nContent-ID: <content.bin>\n\n";
    
    // Detect the XML header.
    string::size_type loc0 = mimeMsg.find(xmlPartMIMEHeader, 0);
    if ( loc0 == string::npos) {
      // let's try with CRLFs
      xmlPartMIMEHeader = "Content-ID: <header.xml>\r\n\r\n";
      loc0 = mimeMsg.find(xmlPartMIMEHeader, 0);
      if  ( loc0 == string::npos ) 
	      throw ConversionException("Failed to detect the beginning of the XML header", "ConfigDescription");
    }

    loc0 += xmlPartMIMEHeader.size();
    
    // Look for the string announcing the binary part.
    string::size_type loc1 = mimeMsg.find( binPartMIMEHeader, loc0 );
    
    if ( loc1 == string::npos ) {
      throw ConversionException("Failed to detect the beginning of the binary part", "ConfigDescription");
    }
    
    //
    // Extract the xmlHeader and analyze it to find out what is the byte order and the sequence
    // of attribute names.
    //
    string xmlHeader = mimeMsg.substr(loc0, loc1-loc0);
    xmlDoc *doc;
    doc = xmlReadMemory(xmlHeader.data(), xmlHeader.size(), "BinaryTableHeader.xml", NULL, XML_PARSE_NOBLANKS);
    if ( doc == NULL ) 
      throw ConversionException("Failed to parse the xmlHeader into a DOM structure.", "ConfigDescription");
    
   // This vector will be filled by the names of  all the attributes of the table
   // in the order in which they are expected to be found in the binary representation.
   //
    vector<string> attributesSeq;
      
    xmlNode* root_element = xmlDocGetRootElement(doc);
    if ( root_element == NULL || root_element->type != XML_ELEMENT_NODE )
      throw ConversionException("Failed to parse the xmlHeader into a DOM structure.", "ConfigDescription");
    
    const ByteOrder* byteOrder=0;
    if ( string("ASDMBinaryTable").compare((const char*) root_element->name) == 0) {
      // Then it's an "old fashioned" MIME file for tables.
      // Just try to deserialize it with Big_Endian for the bytes ordering.
      byteOrder = asdm::ByteOrder::Big_Endian;
      
 	 //
    // Let's consider a  default order for the sequence of attributes.
    //
    
    	 
    attributesSeq.push_back("configDescriptionId") ; 
    	 
    attributesSeq.push_back("numAntenna") ; 
    	 
    attributesSeq.push_back("numDataDescription") ; 
    	 
    attributesSeq.push_back("numFeed") ; 
    	 
    attributesSeq.push_back("correlationMode") ; 
    	 
    attributesSeq.push_back("numAtmPhaseCorrection") ; 
    	 
    attributesSeq.push_back("atmPhaseCorrection") ; 
    	 
    attributesSeq.push_back("processorType") ; 
    	 
    attributesSeq.push_back("spectralType") ; 
    	 
    attributesSeq.push_back("antennaId") ; 
    	 
    attributesSeq.push_back("feedId") ; 
    	 
    attributesSeq.push_back("switchCycleId") ; 
    	 
    attributesSeq.push_back("dataDescriptionId") ; 
    	 
    attributesSeq.push_back("processorId") ; 
    	
    	 
    attributesSeq.push_back("phasedArrayList") ; 
    	 
    attributesSeq.push_back("numAssocValues") ; 
    	 
    attributesSeq.push_back("assocNature") ; 
    	 
    attributesSeq.push_back("assocConfigDescriptionId") ; 
    	
     
    
    
    // And decide that it has version == "2"
    version = "2";         
     }
    else if (string("ConfigDescriptionTable").compare((const char*) root_element->name) == 0) {
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
      	throw ConversionException ("Could not find the element '/ConfigDescriptionTable/BulkStoreRef'. Invalid XML header '"+ xmlHeader + "'.", "ConfigDescription");
      	
      // We found BulkStoreRef, now look for its attribute byteOrder.
      _xmlAttr* byteOrderAttr = 0;
      for (struct _xmlAttr* attr = bulkStoreRef->properties; attr; attr = attr->next) 
	  if (string("byteOrder").compare((const char*) attr->name) == 0) {
	   byteOrderAttr = attr;
	   break;
	 }
      
      if (byteOrderAttr == 0) 
	     throw ConversionException("Could not find the element '/ConfigDescriptionTable/BulkStoreRef/@byteOrder'. Invalid XML header '" + xmlHeader +"'.", "ConfigDescription");
      
      string byteOrderValue = string((const char*) byteOrderAttr->children->content);
      if (!(byteOrder = asdm::ByteOrder::fromString(byteOrderValue)))
		throw ConversionException("No valid value retrieved for the element '/ConfigDescriptionTable/BulkStoreRef/@byteOrder'. Invalid XML header '" + xmlHeader + "'.", "ConfigDescription");
		
	 //
	 // 2nd) Look for the Attributes element and grab the names of the elements it contains.
	 //
	 xmlNode* attributes = bulkStoreRef->next;
     if ( attributes == 0 || (attributes->type != XML_ELEMENT_NODE)  || (string("Attributes").compare((const char*) attributes->name) != 0))	 
       	throw ConversionException ("Could not find the element '/ConfigDescriptionTable/Attributes'. Invalid XML header '"+ xmlHeader + "'.", "ConfigDescription");
 
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
				ConfigDescriptionRow* aRow = ConfigDescriptionRow::fromBin((EndianIStream&) eiss, *this, attributesSeq);
				checkAndAdd(aRow);
      		}
    	}
    	catch (const DuplicateKey &e) {
      		throw ConversionException("Error while writing binary data , the message was "
				+ e.getMessage(), "ConfigDescription");
    	}
    	catch (const TagFormatException &e) {
     		 throw ConversionException("Error while reading binary data , the message was "
				+ e.getMessage(), "ConfigDescription");
    	}
    }
    else {
 		for (uint32_t i = 0; i < this->declaredSize; i++) {
			ConfigDescriptionRow* aRow = ConfigDescriptionRow::fromBin((EndianIStream&) eiss, *this, attributesSeq);
			append(aRow);
      	}   	
    }
    //Does not change the convention defined in the model.	
    //archiveAsBin = true;
    //fileAsBin = true;
    if ( doc != NULL ) xmlFreeDoc(doc);

	}
	
	void ConfigDescriptionTable::setUnknownAttributeBinaryReader(const string& attributeName, BinaryAttributeReaderFunctor* barFctr) {
		//
		// Is this attribute really unknown ?
		//
		for (vector<string>::const_iterator iter = attributesNamesOfConfigDescription_v.begin(); iter != attributesNamesOfConfigDescription_v.end(); iter++) {
			if ((*iter).compare(attributeName) == 0) 
				throw ConversionException("the attribute '"+attributeName+"' is known you can't override the way it's read in the MIME binary file containing the table.", "ConfigDescription"); 
		}
		
		// Ok then register the functor to activate when an unknown attribute is met during the reading of a binary table?
		unknownAttributes2Functors[attributeName] = barFctr;
	}
	
	BinaryAttributeReaderFunctor* ConfigDescriptionTable::getUnknownAttributeBinaryReader(const string& attributeName) const {
		map<string, BinaryAttributeReaderFunctor*>::const_iterator iter = unknownAttributes2Functors.find(attributeName);
		return (iter == unknownAttributes2Functors.end()) ? 0 : iter->second;
	}

	
	void ConfigDescriptionTable::toFile(string directory) {
		if (!directoryExists(directory.c_str()) &&
			!createPath(directory.c_str())) {
			throw ConversionException("Could not create directory " , directory);
		}

		string fileName = directory + "/ConfigDescription.xml";
		ofstream tableout(fileName.c_str(),ios::out|ios::trunc);
		if (tableout.rdstate() == ostream::failbit)
			throw ConversionException("Could not open file " + fileName + " to write ", "ConfigDescription");
		if (fileAsBin) 
			tableout << MIMEXMLPart();
		else
			tableout << toXML() << endl;
		tableout.close();
		if (tableout.rdstate() == ostream::failbit)
			throw ConversionException("Could not close file " + fileName, "ConfigDescription");

		if (fileAsBin) {
			// write the bin serialized
			string fileName = directory + "/ConfigDescription.bin";
			ofstream tableout(fileName.c_str(),ios::out|ios::trunc);
			if (tableout.rdstate() == ostream::failbit)
				throw ConversionException("Could not open file " + fileName + " to write ", "ConfigDescription");
			tableout << toMIME() << endl;
			tableout.close();
			if (tableout.rdstate() == ostream::failbit)
				throw ConversionException("Could not close file " + fileName, "ConfigDescription");
		}
	}

	
	void ConfigDescriptionTable::setFromFile(const string& directory) {
#ifndef WITHOUT_BOOST
    if (boost::filesystem::exists(boost::filesystem::path(uniqSlashes(directory + "/ConfigDescription.xml"))))
      setFromXMLFile(directory);
    else if (boost::filesystem::exists(boost::filesystem::path(uniqSlashes(directory + "/ConfigDescription.bin"))))
      setFromMIMEFile(directory);
#else 
    // alternative in Misc.h
    if (file_exists(uniqSlashes(directory + "/ConfigDescription.xml")))
      setFromXMLFile(directory);
    else if (file_exists(uniqSlashes(directory + "/ConfigDescription.bin")))
      setFromMIMEFile(directory);
#endif
    else
      throw ConversionException("No file found for the ConfigDescription table", "ConfigDescription");
	}			

	
  void ConfigDescriptionTable::setFromMIMEFile(const string& directory) {
    string tablePath ;
    
    tablePath = directory + "/ConfigDescription.bin";
    ifstream tablefile(tablePath.c_str(), ios::in|ios::binary);
    if (!tablefile.is_open()) { 
      throw ConversionException("Could not open file " + tablePath, "ConfigDescription");
    }
    // Read in a stringstream.
    stringstream ss; ss << tablefile.rdbuf();
    
    if (tablefile.rdstate() == istream::failbit || tablefile.rdstate() == istream::badbit) {
      throw ConversionException("Error reading file " + tablePath,"ConfigDescription");
    }
    
    // And close.
    tablefile.close();
    if (tablefile.rdstate() == istream::failbit)
      throw ConversionException("Could not close file " + tablePath,"ConfigDescription");
    
    setFromMIME(ss.str());
  }	
/* 
  void ConfigDescriptionTable::openMIMEFile (const string& directory) {
  		
  	// Open the file.
  	string tablePath ;
    tablePath = directory + "/ConfigDescription.bin";
    ifstream tablefile(tablePath.c_str(), ios::in|ios::binary);
    if (!tablefile.is_open())
      throw ConversionException("Could not open file " + tablePath, "ConfigDescription");
      
	// Locate the xmlPartMIMEHeader.
    string xmlPartMIMEHeader = "CONTENT-ID: <HEADER.XML>\n\n";
    CharComparator comparator;
    istreambuf_iterator<char> BEGIN(tablefile.rdbuf());
    istreambuf_iterator<char> END;
    istreambuf_iterator<char> it = search(BEGIN, END, xmlPartMIMEHeader.begin(), xmlPartMIMEHeader.end(), comparator);
    if (it == END) 
    	throw ConversionException("failed to detect the beginning of the XML header", "ConfigDescription");
    
    // Locate the binaryPartMIMEHeader while accumulating the characters of the xml header.	
    string binPartMIMEHeader = "--MIME_BOUNDARY\nCONTENT-TYPE: BINARY/OCTET-STREAM\nCONTENT-ID: <CONTENT.BIN>\n\n";
    string xmlHeader;
   	CharCompAccumulator compaccumulator(&xmlHeader, 100000);
   	++it;
   	it = search(it, END, binPartMIMEHeader.begin(), binPartMIMEHeader.end(), compaccumulator);
   	if (it == END) 
   		throw ConversionException("failed to detect the beginning of the binary part", "ConfigDescription");
   	
	cout << xmlHeader << endl;
	//
	// We have the xmlHeader , let's parse it.
	//
	xmlDoc *doc;
    doc = xmlReadMemory(xmlHeader.data(), xmlHeader.size(), "BinaryTableHeader.xml", NULL, XML_PARSE_NOBLANKS);
    if ( doc == NULL ) 
      throw ConversionException("Failed to parse the xmlHeader into a DOM structure.", "ConfigDescription");
    
   // This vector will be filled by the names of  all the attributes of the table
   // in the order in which they are expected to be found in the binary representation.
   //
    vector<string> attributesSeq(attributesNamesInBinOfConfigDescription_v);
      
    xmlNode* root_element = xmlDocGetRootElement(doc);
    if ( root_element == NULL || root_element->type != XML_ELEMENT_NODE )
      throw ConversionException("Failed to parse the xmlHeader into a DOM structure.", "ConfigDescription");
    
    const ByteOrder* byteOrder=0;
    if ( string("ASDMBinaryTable").compare((const char*) root_element->name) == 0) {
      // Then it's an "old fashioned" MIME file for tables.
      // Just try to deserialize it with Big_Endian for the bytes ordering.
      byteOrder = asdm::ByteOrder::Big_Endian;
        
      // And decide that it has version == "2"
    version = "2";         
     }
    else if (string("ConfigDescriptionTable").compare((const char*) root_element->name) == 0) {
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
      	throw ConversionException ("Could not find the element '/ConfigDescriptionTable/BulkStoreRef'. Invalid XML header '"+ xmlHeader + "'.", "ConfigDescription");
      	
      // We found BulkStoreRef, now look for its attribute byteOrder.
      _xmlAttr* byteOrderAttr = 0;
      for (struct _xmlAttr* attr = bulkStoreRef->properties; attr; attr = attr->next) 
	  if (string("byteOrder").compare((const char*) attr->name) == 0) {
	   byteOrderAttr = attr;
	   break;
	 }
      
      if (byteOrderAttr == 0) 
	     throw ConversionException("Could not find the element '/ConfigDescriptionTable/BulkStoreRef/@byteOrder'. Invalid XML header '" + xmlHeader +"'.", "ConfigDescription");
      
      string byteOrderValue = string((const char*) byteOrderAttr->children->content);
      if (!(byteOrder = asdm::ByteOrder::fromString(byteOrderValue)))
		throw ConversionException("No valid value retrieved for the element '/ConfigDescriptionTable/BulkStoreRef/@byteOrder'. Invalid XML header '" + xmlHeader + "'.", "ConfigDescription");
		
	 //
	 // 2nd) Look for the Attributes element and grab the names of the elements it contains.
	 //
	 xmlNode* attributes = bulkStoreRef->next;
     if ( attributes == 0 || (attributes->type != XML_ELEMENT_NODE)  || (string("Attributes").compare((const char*) attributes->name) != 0))	 
       	throw ConversionException ("Could not find the element '/ConfigDescriptionTable/Attributes'. Invalid XML header '"+ xmlHeader + "'.", "ConfigDescription");
 
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

	
void ConfigDescriptionTable::setFromXMLFile(const string& directory) {
    string tablePath ;
    
    tablePath = directory + "/ConfigDescription.xml";
    
    /*
    ifstream tablefile(tablePath.c_str(), ios::in|ios::binary);
    if (!tablefile.is_open()) { 
      throw ConversionException("Could not open file " + tablePath, "ConfigDescription");
    }
      // Read in a stringstream.
    stringstream ss;
    ss << tablefile.rdbuf();
    
    if  (tablefile.rdstate() == istream::failbit || tablefile.rdstate() == istream::badbit) {
      throw ConversionException("Error reading file '" + tablePath + "'", "ConfigDescription");
    }
    
    // And close
    tablefile.close();
    if (tablefile.rdstate() == istream::failbit)
      throw ConversionException("Could not close file '" + tablePath + "'", "ConfigDescription");

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
    	throw ConversionException("Caugth an exception whose message is '" + e.getMessage() + "'.", "ConfigDescription");
    }
    
    if (xmlDocument.find("<BulkStoreRef") != string::npos)
      setFromMIMEFile(directory);
    else
      fromXML(xmlDocument);
  }

	

	

			
	
	

	
	void ConfigDescriptionTable::autoIncrement(string key, ConfigDescriptionRow* x) {
		map<string, int>::iterator iter;
		if ((iter=noAutoIncIds.find(key)) == noAutoIncIds.end()) {
			// There is not yet a combination of the non autoinc attributes values in the hashtable
			
			// Initialize  configDescriptionId to Tag(0).
			x->setConfigDescriptionId(Tag(0,  TagType::ConfigDescription));
			
			// Record it in the map.		
			noAutoIncIds.insert(make_pair(key, 0));			
		} 
		else {
			// There is already a combination of the non autoinc attributes values in the hashtable
			// Increment its value.
			int n = iter->second + 1; 
			
			// Initialize  configDescriptionId to Tag(n).
			x->setConfigDescriptionId(Tag(n, TagType::ConfigDescription));
			
			// Record it in the map.		
			noAutoIncIds.insert(make_pair(key, n));				
		}		
	}
	
} // End namespace asdm
 
