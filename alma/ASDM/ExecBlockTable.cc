
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
 * File ExecBlockTable.cpp
 */
#include <alma/ASDM/ConversionException.h>
#include <alma/ASDM/DuplicateKey.h>
#include <alma/ASDM/OutOfBoundsException.h>

using asdm::ConversionException;
using asdm::DuplicateKey;
using asdm::OutOfBoundsException;

#include <alma/ASDM/ASDM.h>
#include <alma/ASDM/ExecBlockTable.h>
#include <alma/ASDM/ExecBlockRow.h>
#include <alma/ASDM/Parser.h>

using asdm::ASDM;
using asdm::ExecBlockTable;
using asdm::ExecBlockRow;
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
	static string entityNameOfExecBlock = "ExecBlock";
	
	// An array of string containing the names of the columns of this table.
	// The array is filled in the order : key, required value, optional value.
	//
	static string attributesNamesOfExecBlock_a[] = {
		
			"execBlockId"
		
		
			, "startTime"
		
			, "endTime"
		
			, "execBlockNum"
		
			, "execBlockUID"
		
			, "projectUID"
		
			, "configName"
		
			, "telescopeName"
		
			, "observerName"
		
			, "numObservingLog"
		
			, "observingLog"
		
			, "sessionReference"
		
			, "baseRangeMin"
		
			, "baseRangeMax"
		
			, "baseRmsMinor"
		
			, "baseRmsMajor"
		
			, "basePa"
		
			, "aborted"
		
			, "numAntenna"
		
			, "antennaId"
		
			, "sBSummaryId"
				
		
			, "releaseDate"
		
			, "schedulerMode"
		
			, "siteAltitude"
		
			, "siteLongitude"
		
			, "siteLatitude"
		
			, "observingScript"
		
			, "observingScriptUID"
		
			, "scaleId"
				
	};
	
	// A vector of string whose content is a copy of the strings in the array above.
	//
	static vector<string> attributesNamesOfExecBlock_v (attributesNamesOfExecBlock_a, attributesNamesOfExecBlock_a + sizeof(attributesNamesOfExecBlock_a) / sizeof(attributesNamesOfExecBlock_a[0]));

	// An array of string containing the names of the columns of this table.
	// The array is filled in the order where the names would be read by default in the XML header of a file containing
	// the table exported in binary mode.
	//	
	static string attributesNamesInBinOfExecBlock_a[] = {
    
    	 "execBlockId" , "startTime" , "endTime" , "execBlockNum" , "execBlockUID" , "projectUID" , "configName" , "telescopeName" , "observerName" , "numObservingLog" , "observingLog" , "sessionReference" , "baseRangeMin" , "baseRangeMax" , "baseRmsMinor" , "baseRmsMajor" , "basePa" , "aborted" , "numAntenna" , "antennaId" , "sBSummaryId" 
    	,
    	 "releaseDate" , "schedulerMode" , "siteAltitude" , "siteLongitude" , "siteLatitude" , "observingScript" , "observingScriptUID" , "scaleId" 
    
	};
	        			
	// A vector of string whose content is a copy of the strings in the array above.
	//
	static vector<string> attributesNamesInBinOfExecBlock_v(attributesNamesInBinOfExecBlock_a, attributesNamesInBinOfExecBlock_a + sizeof(attributesNamesInBinOfExecBlock_a) / sizeof(attributesNamesInBinOfExecBlock_a[0]));		
	

	// The array of attributes (or column) names that make up key key.
	//
	string keyOfExecBlock_a[] = {
	
		"execBlockId"
		 
	};
	 
	// A vector of strings which are copies of those stored in the array above.
	vector<string> keyOfExecBlock_v(keyOfExecBlock_a, keyOfExecBlock_a + sizeof(keyOfExecBlock_a) / sizeof(keyOfExecBlock_a[0]));

	/**
	 * Return the list of field names that make up key key
	 * as a const reference to a vector of strings.
	 */	
	const vector<string>& ExecBlockTable::getKeyName() {
		return keyOfExecBlock_v;
	}


	ExecBlockTable::ExecBlockTable(ASDM &c) : container(c) {

		// Define a default entity.
		entity.setEntityId(EntityId("uid://X0/X0/X0"));
		entity.setEntityIdEncrypted("na");
		entity.setEntityTypeName("ExecBlockTable");
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
 * A destructor for ExecBlockTable.
 */
	ExecBlockTable::~ExecBlockTable() {
		for (unsigned int i = 0; i < privateRows.size(); i++) 
			delete(privateRows.at(i));
	}

	/**
	 * Container to which this table belongs.
	 */
	ASDM &ExecBlockTable::getContainer() const {
		return container;
	}

	/**
	 * Return the number of rows in the table.
	 */
	unsigned int ExecBlockTable::size() const {
		if (presentInMemory) 
			return privateRows.size();
		else
			return declaredSize;
	}
	
	/**
	 * Return the name of this table.
	 */
	string ExecBlockTable::getName() const {
		return entityNameOfExecBlock;
	}
	
	/**
	 * Return the name of this table.
	 */
	string ExecBlockTable::name() {
		return entityNameOfExecBlock;
	}
	
	/**
	 * Return the the names of the attributes (or columns) of this table.
	 */
	const vector<string>& ExecBlockTable::getAttributesNames() { return attributesNamesOfExecBlock_v; }
	
	/**
	 * Return the the names of the attributes (or columns) of this table as they appear by default
	 * in an binary export of this table.
	 */
	const vector<string>& ExecBlockTable::defaultAttributesNamesInBin() { return attributesNamesInBinOfExecBlock_v; }

	/**
	 * Return this table's Entity.
	 */
	Entity ExecBlockTable::getEntity() const {
		return entity;
	}

	/**
	 * Set this table's Entity.
	 */
	void ExecBlockTable::setEntity(Entity e) {
		this->entity = e; 
	}
	
	//
	// ====> Row creation.
	//
	
	/**
	 * Create a new row.
	 */
	ExecBlockRow *ExecBlockTable::newRow() {
		return new ExecBlockRow (*this);
	}
	

	/**
	 * Create a new row initialized to the specified values.
	 * @return a pointer on the created and initialized row.
	
 	 * @param startTime 
	
 	 * @param endTime 
	
 	 * @param execBlockNum 
	
 	 * @param execBlockUID 
	
 	 * @param projectUID 
	
 	 * @param configName 
	
 	 * @param telescopeName 
	
 	 * @param observerName 
	
 	 * @param numObservingLog 
	
 	 * @param observingLog 
	
 	 * @param sessionReference 
	
 	 * @param baseRangeMin 
	
 	 * @param baseRangeMax 
	
 	 * @param baseRmsMinor 
	
 	 * @param baseRmsMajor 
	
 	 * @param basePa 
	
 	 * @param aborted 
	
 	 * @param numAntenna 
	
 	 * @param antennaId 
	
 	 * @param sBSummaryId 
	
     */
	ExecBlockRow* ExecBlockTable::newRow(ArrayTime startTime, ArrayTime endTime, int execBlockNum, EntityRef execBlockUID, EntityRef projectUID, std::string configName, std::string telescopeName, std::string observerName, int numObservingLog, std::vector<std::string > observingLog, EntityRef sessionReference, Length baseRangeMin, Length baseRangeMax, Length baseRmsMinor, Length baseRmsMajor, Angle basePa, bool aborted, int numAntenna, std::vector<Tag>  antennaId, Tag sBSummaryId){
		ExecBlockRow *row = new ExecBlockRow(*this);
			
		row->setStartTime(startTime);
			
		row->setEndTime(endTime);
			
		row->setExecBlockNum(execBlockNum);
			
		row->setExecBlockUID(execBlockUID);
			
		row->setProjectUID(projectUID);
			
		row->setConfigName(configName);
			
		row->setTelescopeName(telescopeName);
			
		row->setObserverName(observerName);
			
		row->setNumObservingLog(numObservingLog);
			
		row->setObservingLog(observingLog);
			
		row->setSessionReference(sessionReference);
			
		row->setBaseRangeMin(baseRangeMin);
			
		row->setBaseRangeMax(baseRangeMax);
			
		row->setBaseRmsMinor(baseRmsMinor);
			
		row->setBaseRmsMajor(baseRmsMajor);
			
		row->setBasePa(basePa);
			
		row->setAborted(aborted);
			
		row->setNumAntenna(numAntenna);
			
		row->setAntennaId(antennaId);
			
		row->setSBSummaryId(sBSummaryId);
	
		return row;		
	}	
	


ExecBlockRow* ExecBlockTable::newRow(ExecBlockRow* row) {
	return new ExecBlockRow(*this, row);
}

	//
	// Append a row to its table.
	//

	
	 
	
	/** 
 	 * Look up the table for a row whose noautoincrementable attributes are matching their
 	 * homologues in *x.  If a row is found  this row else autoincrement  *x.execBlockId, 
 	 * add x to its table and returns x.
 	 *  
 	 * @returns a pointer on a ExecBlockRow.
 	 * @param x. A pointer on the row to be added.
 	 */ 
 		
			
	ExecBlockRow* ExecBlockTable::add(ExecBlockRow* x) {
			 
		ExecBlockRow* aRow = lookup(
				
		x->getStartTime()
				,
		x->getEndTime()
				,
		x->getExecBlockNum()
				,
		x->getExecBlockUID()
				,
		x->getProjectUID()
				,
		x->getConfigName()
				,
		x->getTelescopeName()
				,
		x->getObserverName()
				,
		x->getNumObservingLog()
				,
		x->getObservingLog()
				,
		x->getSessionReference()
				,
		x->getBaseRangeMin()
				,
		x->getBaseRangeMax()
				,
		x->getBaseRmsMinor()
				,
		x->getBaseRmsMajor()
				,
		x->getBasePa()
				,
		x->getAborted()
				,
		x->getNumAntenna()
				,
		x->getAntennaId()
				,
		x->getSBSummaryId()
				
		);
		if (aRow) return aRow;
			

			
		// Autoincrement execBlockId
		x->setExecBlockId(Tag(size(), TagType::ExecBlock));
						
		row.push_back(x);
		privateRows.push_back(x);
		x->isAdded(true);
		return x;
	}
		
	
		
	void ExecBlockTable::addWithoutCheckingUnique(ExecBlockRow * x) {
		if (getRowByKey(
						x->getExecBlockId()
						) != (ExecBlockRow *) 0) 
			throw DuplicateKey("Dupicate key exception in ", "ExecBlockTable");
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
	ExecBlockRow*  ExecBlockTable::checkAndAdd(ExecBlockRow* x, bool skipCheckUniqueness)  {
		if (!skipCheckUniqueness) { 
	 
		 
			if (lookup(
			
				x->getStartTime()
		,
				x->getEndTime()
		,
				x->getExecBlockNum()
		,
				x->getExecBlockUID()
		,
				x->getProjectUID()
		,
				x->getConfigName()
		,
				x->getTelescopeName()
		,
				x->getObserverName()
		,
				x->getNumObservingLog()
		,
				x->getObservingLog()
		,
				x->getSessionReference()
		,
				x->getBaseRangeMin()
		,
				x->getBaseRangeMax()
		,
				x->getBaseRmsMinor()
		,
				x->getBaseRmsMajor()
		,
				x->getBasePa()
		,
				x->getAborted()
		,
				x->getNumAntenna()
		,
				x->getAntennaId()
		,
				x->getSBSummaryId()
		
			)) throw UniquenessViolationException();
		
		
		}
		
		if (getRowByKey(
	
			x->getExecBlockId()
			
		)) throw DuplicateKey("Duplicate key exception in ", "ExecBlockTable");
		
		row.push_back(x);
		privateRows.push_back(x);
		x->isAdded(true);
		return x;	
	}	



	//
	// A private method to brutally append a row to its table, without checking for row uniqueness.
	//

	void ExecBlockTable::append(ExecBlockRow *x) {
		privateRows.push_back(x);
		x->isAdded(true);
	}





	 vector<ExecBlockRow *> ExecBlockTable::get() {
	 	checkPresenceInMemory();
	    return privateRows;
	 }
	 
	 const vector<ExecBlockRow *>& ExecBlockTable::get() const {
	 	const_cast<ExecBlockTable&>(*this).checkPresenceInMemory();	
	    return privateRows;
	 }	 
	 	




	

	
/*
 ** Returns a ExecBlockRow* given a key.
 ** @return a pointer to the row having the key whose values are passed as parameters, or 0 if
 ** no row exists for that key.
 **
 */
 	ExecBlockRow* ExecBlockTable::getRowByKey(Tag execBlockId)  {
 	checkPresenceInMemory();
	ExecBlockRow* aRow = 0;
	for (unsigned int i = 0; i < privateRows.size(); i++) {
		aRow = row.at(i);
		
			
				if (aRow->execBlockId != execBlockId) continue;
			
		
		return aRow;
	}
	return 0;		
}
	

	
/**
 * Look up the table for a row whose all attributes  except the autoincrementable one 
 * are equal to the corresponding parameters of the method.
 * @return a pointer on this row if any, 0 otherwise.
 *
			
 * @param startTime.
 	 		
 * @param endTime.
 	 		
 * @param execBlockNum.
 	 		
 * @param execBlockUID.
 	 		
 * @param projectUID.
 	 		
 * @param configName.
 	 		
 * @param telescopeName.
 	 		
 * @param observerName.
 	 		
 * @param numObservingLog.
 	 		
 * @param observingLog.
 	 		
 * @param sessionReference.
 	 		
 * @param baseRangeMin.
 	 		
 * @param baseRangeMax.
 	 		
 * @param baseRmsMinor.
 	 		
 * @param baseRmsMajor.
 	 		
 * @param basePa.
 	 		
 * @param aborted.
 	 		
 * @param numAntenna.
 	 		
 * @param antennaId.
 	 		
 * @param sBSummaryId.
 	 		 
 */
ExecBlockRow* ExecBlockTable::lookup(ArrayTime startTime, ArrayTime endTime, int execBlockNum, EntityRef execBlockUID, EntityRef projectUID, std::string configName, std::string telescopeName, std::string observerName, int numObservingLog, std::vector<std::string > observingLog, EntityRef sessionReference, Length baseRangeMin, Length baseRangeMax, Length baseRmsMinor, Length baseRmsMajor, Angle basePa, bool aborted, int numAntenna, std::vector<Tag>  antennaId, Tag sBSummaryId) {
		ExecBlockRow* aRow;
		for (unsigned int i = 0; i < privateRows.size(); i++) {
			aRow = privateRows.at(i); 
			if (aRow->compareNoAutoInc(startTime, endTime, execBlockNum, execBlockUID, projectUID, configName, telescopeName, observerName, numObservingLog, observingLog, sessionReference, baseRangeMin, baseRangeMax, baseRmsMinor, baseRmsMajor, basePa, aborted, numAntenna, antennaId, sBSummaryId)) return aRow;
		}			
		return 0;	
} 
	
 	 	

	



#ifndef WITHOUT_ACS
	using asdmIDL::ExecBlockTableIDL;
#endif

#ifndef WITHOUT_ACS
	// Conversion Methods

	ExecBlockTableIDL *ExecBlockTable::toIDL() {
		ExecBlockTableIDL *x = new ExecBlockTableIDL ();
		unsigned int nrow = size();
		x->row.length(nrow);
		vector<ExecBlockRow*> v = get();
		for (unsigned int i = 0; i < nrow; ++i) {
			//x->row[i] = *(v[i]->toIDL());
			v[i]->toIDL(x->row[i]);
		}
		return x;
	}
	
	void ExecBlockTable::toIDL(asdmIDL::ExecBlockTableIDL& x) const {
		unsigned int nrow = size();
		x.row.length(nrow);
		vector<ExecBlockRow*> v = get();
		for (unsigned int i = 0; i < nrow; ++i) {
			v[i]->toIDL(x.row[i]);
		}
	}	
#endif
	
#ifndef WITHOUT_ACS
	void ExecBlockTable::fromIDL(ExecBlockTableIDL x) {
		unsigned int nrow = x.row.length();
		for (unsigned int i = 0; i < nrow; ++i) {
			ExecBlockRow *tmp = newRow();
			tmp->setFromIDL(x.row[i]);
			// checkAndAdd(tmp);
			add(tmp);
		}
	}	
#endif

	
	string ExecBlockTable::toXML()  {
		string buf;

		buf.append("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?> ");
		buf.append("<ExecBlockTable xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:excblk=\"http://Alma/XASDM/ExecBlockTable\" xsi:schemaLocation=\"http://Alma/XASDM/ExecBlockTable http://almaobservatory.org/XML/XASDM/4/ExecBlockTable.xsd\" schemaVersion=\"4\" schemaRevision=\"-1\">\n");
	
		buf.append(entity.toXML());
		string s = container.getEntity().toXML();
		// Change the "Entity" tag to "ContainerEntity".
		buf.append("<Container" + s.substr(1,s.length() - 1)+" ");
		vector<ExecBlockRow*> v = get();
		for (unsigned int i = 0; i < v.size(); ++i) {
			try {
				buf.append(v[i]->toXML());
			} catch (const NoSuchRow &e) {
			}
			buf.append("  ");
		}		
		buf.append("</ExecBlockTable> ");
		return buf;
	}

	
	string ExecBlockTable::getVersion() const {
		return version;
	}
	

	void ExecBlockTable::fromXML(string& tableInXML)  {
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
			throw ConversionException("Failed to parse the xmlHeader into a DOM structure.", "ExecBlock");
		
		xmlNode* root_element = xmlDocGetRootElement(doc);
   		if ( root_element == NULL || root_element->type != XML_ELEMENT_NODE )
      		throw ConversionException("Failed to retrieve the root element in the DOM structure.", "ExecBlock");
      		
      	xmlChar * propValue = xmlGetProp(root_element, (const xmlChar *) "schemaVersion");
      	if ( propValue != 0 ) {
      		version = string( (const char*) propValue);
      		xmlFree(propValue);   		
      	}
      		     							
		Parser xml(tableInXML);
		if (!xml.isStr("<ExecBlockTable")) 
			error();
		// cout << "Parsing a ExecBlockTable" << endl;
		string s = xml.getElement("<Entity","/>");
		if (s.length() == 0) 
			error();
		Entity e;
		e.setFromXML(s);
		if (e.getEntityTypeName() != "ExecBlockTable")
			error();
		setEntity(e);
		// Skip the container's entity; but, it has to be there.
		s = xml.getElement("<ContainerEntity","/>");
		if (s.length() == 0) 
			error();

		// Get each row in the table.
		s = xml.getElementContent("<row>","</row>");
		ExecBlockRow *row;
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
				throw ConversionException(e1.getMessage(),"ExecBlockTable");
			} 
			catch (const UniquenessViolationException &e1) {
				throw ConversionException(e1.getMessage(),"ExecBlockTable");	
			}
			catch (...) {
				// cout << "Unexpected error in ExecBlockTable::checkAndAdd called from ExecBlockTable::fromXML " << endl;
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
				throw ConversionException(e1.getMessage(),"ExecBlockTable");
			} 
			catch (...) {
				// cout << "Unexpected error in ExecBlockTable::addWithoutCheckingUnique called from ExecBlockTable::fromXML " << endl;
			}
		}				
				
				
		if (!xml.isStr("</ExecBlockTable>")) 
		error();
		
		//Does not change the convention defined in the model.	
		//archiveAsBin = false;
		//fileAsBin = false;

                // clean up the xmlDoc pointer
		if ( doc != NULL ) xmlFreeDoc(doc);
		
	}

	
	void ExecBlockTable::error()  {
		throw ConversionException("Invalid xml document","ExecBlock");
	}
	
	
	string ExecBlockTable::MIMEXMLPart(const asdm::ByteOrder* byteOrder) {
		string UID = getEntity().getEntityId().toString();
		string withoutUID = UID.substr(6);
		string containerUID = getContainer().getEntity().getEntityId().toString();
		ostringstream oss;
		oss << "<?xml version='1.0'  encoding='ISO-8859-1'?>";
		oss << "\n";
		oss << "<ExecBlockTable xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:excblk=\"http://Alma/XASDM/ExecBlockTable\" xsi:schemaLocation=\"http://Alma/XASDM/ExecBlockTable http://almaobservatory.org/XML/XASDM/4/ExecBlockTable.xsd\" schemaVersion=\"4\" schemaRevision=\"-1\">\n";
		oss<< "<Entity entityId='"<<UID<<"' entityIdEncrypted='na' entityTypeName='ExecBlockTable' schemaVersion='1' documentVersion='1'/>\n";
		oss<< "<ContainerEntity entityId='"<<containerUID<<"' entityIdEncrypted='na' entityTypeName='ASDM' schemaVersion='1' documentVersion='1'/>\n";
		oss << "<BulkStoreRef file_id='"<<withoutUID<<"' byteOrder='"<<byteOrder->toString()<<"' />\n";
		oss << "<Attributes>\n";

		oss << "<execBlockId/>\n"; 
		oss << "<startTime/>\n"; 
		oss << "<endTime/>\n"; 
		oss << "<execBlockNum/>\n"; 
		oss << "<execBlockUID/>\n"; 
		oss << "<projectUID/>\n"; 
		oss << "<configName/>\n"; 
		oss << "<telescopeName/>\n"; 
		oss << "<observerName/>\n"; 
		oss << "<numObservingLog/>\n"; 
		oss << "<observingLog/>\n"; 
		oss << "<sessionReference/>\n"; 
		oss << "<baseRangeMin/>\n"; 
		oss << "<baseRangeMax/>\n"; 
		oss << "<baseRmsMinor/>\n"; 
		oss << "<baseRmsMajor/>\n"; 
		oss << "<basePa/>\n"; 
		oss << "<aborted/>\n"; 
		oss << "<numAntenna/>\n"; 
		oss << "<antennaId/>\n"; 
		oss << "<sBSummaryId/>\n"; 

		oss << "<releaseDate/>\n"; 
		oss << "<schedulerMode/>\n"; 
		oss << "<siteAltitude/>\n"; 
		oss << "<siteLongitude/>\n"; 
		oss << "<siteLatitude/>\n"; 
		oss << "<observingScript/>\n"; 
		oss << "<observingScriptUID/>\n"; 
		oss << "<scaleId/>\n"; 
		oss << "</Attributes>\n";		
		oss << "</ExecBlockTable>\n";

		return oss.str();				
	}
	
	string ExecBlockTable::toMIME(const asdm::ByteOrder* byteOrder) {
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

	
	void ExecBlockTable::setFromMIME(const string & mimeMsg) {
    string xmlPartMIMEHeader = "Content-ID: <header.xml>\n\n";
    
    string binPartMIMEHeader = "--MIME_boundary\nContent-Type: binary/octet-stream\nContent-ID: <content.bin>\n\n";
    
    // Detect the XML header.
    string::size_type loc0 = mimeMsg.find(xmlPartMIMEHeader, 0);
    if ( loc0 == string::npos) {
      // let's try with CRLFs
      xmlPartMIMEHeader = "Content-ID: <header.xml>\r\n\r\n";
      loc0 = mimeMsg.find(xmlPartMIMEHeader, 0);
      if  ( loc0 == string::npos ) 
	      throw ConversionException("Failed to detect the beginning of the XML header", "ExecBlock");
    }

    loc0 += xmlPartMIMEHeader.size();
    
    // Look for the string announcing the binary part.
    string::size_type loc1 = mimeMsg.find( binPartMIMEHeader, loc0 );
    
    if ( loc1 == string::npos ) {
      throw ConversionException("Failed to detect the beginning of the binary part", "ExecBlock");
    }
    
    //
    // Extract the xmlHeader and analyze it to find out what is the byte order and the sequence
    // of attribute names.
    //
    string xmlHeader = mimeMsg.substr(loc0, loc1-loc0);
    xmlDoc *doc;
    doc = xmlReadMemory(xmlHeader.data(), xmlHeader.size(), "BinaryTableHeader.xml", NULL, XML_PARSE_NOBLANKS);
    if ( doc == NULL ) 
      throw ConversionException("Failed to parse the xmlHeader into a DOM structure.", "ExecBlock");
    
   // This vector will be filled by the names of  all the attributes of the table
   // in the order in which they are expected to be found in the binary representation.
   //
    vector<string> attributesSeq;
      
    xmlNode* root_element = xmlDocGetRootElement(doc);
    if ( root_element == NULL || root_element->type != XML_ELEMENT_NODE )
      throw ConversionException("Failed to parse the xmlHeader into a DOM structure.", "ExecBlock");
    
    const ByteOrder* byteOrder=0;
    if ( string("ASDMBinaryTable").compare((const char*) root_element->name) == 0) {
      // Then it's an "old fashioned" MIME file for tables.
      // Just try to deserialize it with Big_Endian for the bytes ordering.
      byteOrder = asdm::ByteOrder::Big_Endian;
      
 	 //
    // Let's consider a  default order for the sequence of attributes.
    //
    
    	 
    attributesSeq.push_back("execBlockId") ; 
    	 
    attributesSeq.push_back("startTime") ; 
    	 
    attributesSeq.push_back("endTime") ; 
    	 
    attributesSeq.push_back("execBlockNum") ; 
    	 
    attributesSeq.push_back("execBlockUID") ; 
    	 
    attributesSeq.push_back("projectUID") ; 
    	 
    attributesSeq.push_back("configName") ; 
    	 
    attributesSeq.push_back("telescopeName") ; 
    	 
    attributesSeq.push_back("observerName") ; 
    	 
    attributesSeq.push_back("numObservingLog") ; 
    	 
    attributesSeq.push_back("observingLog") ; 
    	 
    attributesSeq.push_back("sessionReference") ; 
    	 
    attributesSeq.push_back("baseRangeMin") ; 
    	 
    attributesSeq.push_back("baseRangeMax") ; 
    	 
    attributesSeq.push_back("baseRmsMinor") ; 
    	 
    attributesSeq.push_back("baseRmsMajor") ; 
    	 
    attributesSeq.push_back("basePa") ; 
    	 
    attributesSeq.push_back("aborted") ; 
    	 
    attributesSeq.push_back("numAntenna") ; 
    	 
    attributesSeq.push_back("antennaId") ; 
    	 
    attributesSeq.push_back("sBSummaryId") ; 
    	
    	 
    attributesSeq.push_back("releaseDate") ; 
    	 
    attributesSeq.push_back("schedulerMode") ; 
    	 
    attributesSeq.push_back("siteAltitude") ; 
    	 
    attributesSeq.push_back("siteLongitude") ; 
    	 
    attributesSeq.push_back("siteLatitude") ; 
    	 
    attributesSeq.push_back("observingScript") ; 
    	 
    attributesSeq.push_back("observingScriptUID") ; 
    	 
    attributesSeq.push_back("scaleId") ; 
    	
     
    
    
    // And decide that it has version == "2"
    version = "2";         
     }
    else if (string("ExecBlockTable").compare((const char*) root_element->name) == 0) {
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
      	throw ConversionException ("Could not find the element '/ExecBlockTable/BulkStoreRef'. Invalid XML header '"+ xmlHeader + "'.", "ExecBlock");
      	
      // We found BulkStoreRef, now look for its attribute byteOrder.
      _xmlAttr* byteOrderAttr = 0;
      for (struct _xmlAttr* attr = bulkStoreRef->properties; attr; attr = attr->next) 
	  if (string("byteOrder").compare((const char*) attr->name) == 0) {
	   byteOrderAttr = attr;
	   break;
	 }
      
      if (byteOrderAttr == 0) 
	     throw ConversionException("Could not find the element '/ExecBlockTable/BulkStoreRef/@byteOrder'. Invalid XML header '" + xmlHeader +"'.", "ExecBlock");
      
      string byteOrderValue = string((const char*) byteOrderAttr->children->content);
      if (!(byteOrder = asdm::ByteOrder::fromString(byteOrderValue)))
		throw ConversionException("No valid value retrieved for the element '/ExecBlockTable/BulkStoreRef/@byteOrder'. Invalid XML header '" + xmlHeader + "'.", "ExecBlock");
		
	 //
	 // 2nd) Look for the Attributes element and grab the names of the elements it contains.
	 //
	 xmlNode* attributes = bulkStoreRef->next;
     if ( attributes == 0 || (attributes->type != XML_ELEMENT_NODE)  || (string("Attributes").compare((const char*) attributes->name) != 0))	 
       	throw ConversionException ("Could not find the element '/ExecBlockTable/Attributes'. Invalid XML header '"+ xmlHeader + "'.", "ExecBlock");
 
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
				ExecBlockRow* aRow = ExecBlockRow::fromBin((EndianIStream&) eiss, *this, attributesSeq);
				checkAndAdd(aRow);
      		}
    	}
    	catch (const DuplicateKey &e) {
      		throw ConversionException("Error while writing binary data , the message was "
				+ e.getMessage(), "ExecBlock");
    	}
    	catch (const TagFormatException &e) {
     		 throw ConversionException("Error while reading binary data , the message was "
				+ e.getMessage(), "ExecBlock");
    	}
    }
    else {
 		for (uint32_t i = 0; i < this->declaredSize; i++) {
			ExecBlockRow* aRow = ExecBlockRow::fromBin((EndianIStream&) eiss, *this, attributesSeq);
			append(aRow);
      	}   	
    }
    //Does not change the convention defined in the model.	
    //archiveAsBin = true;
    //fileAsBin = true;
    if ( doc != NULL ) xmlFreeDoc(doc);

	}
	
	void ExecBlockTable::setUnknownAttributeBinaryReader(const string& attributeName, BinaryAttributeReaderFunctor* barFctr) {
		//
		// Is this attribute really unknown ?
		//
		for (vector<string>::const_iterator iter = attributesNamesOfExecBlock_v.begin(); iter != attributesNamesOfExecBlock_v.end(); iter++) {
			if ((*iter).compare(attributeName) == 0) 
				throw ConversionException("the attribute '"+attributeName+"' is known you can't override the way it's read in the MIME binary file containing the table.", "ExecBlock"); 
		}
		
		// Ok then register the functor to activate when an unknown attribute is met during the reading of a binary table?
		unknownAttributes2Functors[attributeName] = barFctr;
	}
	
	BinaryAttributeReaderFunctor* ExecBlockTable::getUnknownAttributeBinaryReader(const string& attributeName) const {
		map<string, BinaryAttributeReaderFunctor*>::const_iterator iter = unknownAttributes2Functors.find(attributeName);
		return (iter == unknownAttributes2Functors.end()) ? 0 : iter->second;
	}

	
	void ExecBlockTable::toFile(string directory) {
		if (!directoryExists(directory.c_str()) &&
			!createPath(directory.c_str())) {
			throw ConversionException("Could not create directory " , directory);
		}

		string fileName = directory + "/ExecBlock.xml";
		ofstream tableout(fileName.c_str(),ios::out|ios::trunc);
		if (tableout.rdstate() == ostream::failbit)
			throw ConversionException("Could not open file " + fileName + " to write ", "ExecBlock");
		if (fileAsBin) 
			tableout << MIMEXMLPart();
		else
			tableout << toXML() << endl;
		tableout.close();
		if (tableout.rdstate() == ostream::failbit)
			throw ConversionException("Could not close file " + fileName, "ExecBlock");

		if (fileAsBin) {
			// write the bin serialized
			string fileName = directory + "/ExecBlock.bin";
			ofstream tableout(fileName.c_str(),ios::out|ios::trunc);
			if (tableout.rdstate() == ostream::failbit)
				throw ConversionException("Could not open file " + fileName + " to write ", "ExecBlock");
			tableout << toMIME() << endl;
			tableout.close();
			if (tableout.rdstate() == ostream::failbit)
				throw ConversionException("Could not close file " + fileName, "ExecBlock");
		}
	}

	
	void ExecBlockTable::setFromFile(const string& directory) {
#ifndef WITHOUT_BOOST
    if (boost::filesystem::exists(boost::filesystem::path(uniqSlashes(directory + "/ExecBlock.xml"))))
      setFromXMLFile(directory);
    else if (boost::filesystem::exists(boost::filesystem::path(uniqSlashes(directory + "/ExecBlock.bin"))))
      setFromMIMEFile(directory);
#else 
    // alternative in Misc.h
    if (file_exists(uniqSlashes(directory + "/ExecBlock.xml")))
      setFromXMLFile(directory);
    else if (file_exists(uniqSlashes(directory + "/ExecBlock.bin")))
      setFromMIMEFile(directory);
#endif
    else
      throw ConversionException("No file found for the ExecBlock table", "ExecBlock");
	}			

	
  void ExecBlockTable::setFromMIMEFile(const string& directory) {
    string tablePath ;
    
    tablePath = directory + "/ExecBlock.bin";
    ifstream tablefile(tablePath.c_str(), ios::in|ios::binary);
    if (!tablefile.is_open()) { 
      throw ConversionException("Could not open file " + tablePath, "ExecBlock");
    }
    // Read in a stringstream.
    stringstream ss; ss << tablefile.rdbuf();
    
    if (tablefile.rdstate() == istream::failbit || tablefile.rdstate() == istream::badbit) {
      throw ConversionException("Error reading file " + tablePath,"ExecBlock");
    }
    
    // And close.
    tablefile.close();
    if (tablefile.rdstate() == istream::failbit)
      throw ConversionException("Could not close file " + tablePath,"ExecBlock");
    
    setFromMIME(ss.str());
  }	
/* 
  void ExecBlockTable::openMIMEFile (const string& directory) {
  		
  	// Open the file.
  	string tablePath ;
    tablePath = directory + "/ExecBlock.bin";
    ifstream tablefile(tablePath.c_str(), ios::in|ios::binary);
    if (!tablefile.is_open())
      throw ConversionException("Could not open file " + tablePath, "ExecBlock");
      
	// Locate the xmlPartMIMEHeader.
    string xmlPartMIMEHeader = "CONTENT-ID: <HEADER.XML>\n\n";
    CharComparator comparator;
    istreambuf_iterator<char> BEGIN(tablefile.rdbuf());
    istreambuf_iterator<char> END;
    istreambuf_iterator<char> it = search(BEGIN, END, xmlPartMIMEHeader.begin(), xmlPartMIMEHeader.end(), comparator);
    if (it == END) 
    	throw ConversionException("failed to detect the beginning of the XML header", "ExecBlock");
    
    // Locate the binaryPartMIMEHeader while accumulating the characters of the xml header.	
    string binPartMIMEHeader = "--MIME_BOUNDARY\nCONTENT-TYPE: BINARY/OCTET-STREAM\nCONTENT-ID: <CONTENT.BIN>\n\n";
    string xmlHeader;
   	CharCompAccumulator compaccumulator(&xmlHeader, 100000);
   	++it;
   	it = search(it, END, binPartMIMEHeader.begin(), binPartMIMEHeader.end(), compaccumulator);
   	if (it == END) 
   		throw ConversionException("failed to detect the beginning of the binary part", "ExecBlock");
   	
	cout << xmlHeader << endl;
	//
	// We have the xmlHeader , let's parse it.
	//
	xmlDoc *doc;
    doc = xmlReadMemory(xmlHeader.data(), xmlHeader.size(), "BinaryTableHeader.xml", NULL, XML_PARSE_NOBLANKS);
    if ( doc == NULL ) 
      throw ConversionException("Failed to parse the xmlHeader into a DOM structure.", "ExecBlock");
    
   // This vector will be filled by the names of  all the attributes of the table
   // in the order in which they are expected to be found in the binary representation.
   //
    vector<string> attributesSeq(attributesNamesInBinOfExecBlock_v);
      
    xmlNode* root_element = xmlDocGetRootElement(doc);
    if ( root_element == NULL || root_element->type != XML_ELEMENT_NODE )
      throw ConversionException("Failed to parse the xmlHeader into a DOM structure.", "ExecBlock");
    
    const ByteOrder* byteOrder=0;
    if ( string("ASDMBinaryTable").compare((const char*) root_element->name) == 0) {
      // Then it's an "old fashioned" MIME file for tables.
      // Just try to deserialize it with Big_Endian for the bytes ordering.
      byteOrder = asdm::ByteOrder::Big_Endian;
        
      // And decide that it has version == "2"
    version = "2";         
     }
    else if (string("ExecBlockTable").compare((const char*) root_element->name) == 0) {
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
      	throw ConversionException ("Could not find the element '/ExecBlockTable/BulkStoreRef'. Invalid XML header '"+ xmlHeader + "'.", "ExecBlock");
      	
      // We found BulkStoreRef, now look for its attribute byteOrder.
      _xmlAttr* byteOrderAttr = 0;
      for (struct _xmlAttr* attr = bulkStoreRef->properties; attr; attr = attr->next) 
	  if (string("byteOrder").compare((const char*) attr->name) == 0) {
	   byteOrderAttr = attr;
	   break;
	 }
      
      if (byteOrderAttr == 0) 
	     throw ConversionException("Could not find the element '/ExecBlockTable/BulkStoreRef/@byteOrder'. Invalid XML header '" + xmlHeader +"'.", "ExecBlock");
      
      string byteOrderValue = string((const char*) byteOrderAttr->children->content);
      if (!(byteOrder = asdm::ByteOrder::fromString(byteOrderValue)))
		throw ConversionException("No valid value retrieved for the element '/ExecBlockTable/BulkStoreRef/@byteOrder'. Invalid XML header '" + xmlHeader + "'.", "ExecBlock");
		
	 //
	 // 2nd) Look for the Attributes element and grab the names of the elements it contains.
	 //
	 xmlNode* attributes = bulkStoreRef->next;
     if ( attributes == 0 || (attributes->type != XML_ELEMENT_NODE)  || (string("Attributes").compare((const char*) attributes->name) != 0))	 
       	throw ConversionException ("Could not find the element '/ExecBlockTable/Attributes'. Invalid XML header '"+ xmlHeader + "'.", "ExecBlock");
 
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

	
void ExecBlockTable::setFromXMLFile(const string& directory) {
    string tablePath ;
    
    tablePath = directory + "/ExecBlock.xml";
    
    /*
    ifstream tablefile(tablePath.c_str(), ios::in|ios::binary);
    if (!tablefile.is_open()) { 
      throw ConversionException("Could not open file " + tablePath, "ExecBlock");
    }
      // Read in a stringstream.
    stringstream ss;
    ss << tablefile.rdbuf();
    
    if  (tablefile.rdstate() == istream::failbit || tablefile.rdstate() == istream::badbit) {
      throw ConversionException("Error reading file '" + tablePath + "'", "ExecBlock");
    }
    
    // And close
    tablefile.close();
    if (tablefile.rdstate() == istream::failbit)
      throw ConversionException("Could not close file '" + tablePath + "'", "ExecBlock");

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
    	throw ConversionException("Caugth an exception whose message is '" + e.getMessage() + "'.", "ExecBlock");
    }
    
    if (xmlDocument.find("<BulkStoreRef") != string::npos)
      setFromMIMEFile(directory);
    else
      fromXML(xmlDocument);
  }

	

	

			
	
	

	
	void ExecBlockTable::autoIncrement(string key, ExecBlockRow* x) {
		map<string, int>::iterator iter;
		if ((iter=noAutoIncIds.find(key)) == noAutoIncIds.end()) {
			// There is not yet a combination of the non autoinc attributes values in the hashtable
			
			// Initialize  execBlockId to Tag(0).
			x->setExecBlockId(Tag(0,  TagType::ExecBlock));
			
			// Record it in the map.		
			noAutoIncIds.insert(make_pair(key, 0));			
		} 
		else {
			// There is already a combination of the non autoinc attributes values in the hashtable
			// Increment its value.
			int n = iter->second + 1; 
			
			// Initialize  execBlockId to Tag(n).
			x->setExecBlockId(Tag(n, TagType::ExecBlock));
			
			// Record it in the map.		
			noAutoIncIds.insert(make_pair(key, n));				
		}		
	}
	
} // End namespace asdm
 
