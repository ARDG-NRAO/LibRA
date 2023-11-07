
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
 * File SourceTable.cpp
 */
#include <alma/ASDM/ConversionException.h>
#include <alma/ASDM/DuplicateKey.h>
#include <alma/ASDM/OutOfBoundsException.h>

using asdm::ConversionException;
using asdm::DuplicateKey;
using asdm::OutOfBoundsException;

#include <alma/ASDM/ASDM.h>
#include <alma/ASDM/SourceTable.h>
#include <alma/ASDM/SourceRow.h>
#include <alma/ASDM/Parser.h>

using asdm::ASDM;
using asdm::SourceTable;
using asdm::SourceRow;
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
	static string entityNameOfSource = "Source";
	
	// An array of string containing the names of the columns of this table.
	// The array is filled in the order : key, required value, optional value.
	//
	static string attributesNamesOfSource_a[] = {
		
			"sourceId"
		,
			"timeInterval"
		,
			"spectralWindowId"
		
		
			, "code"
		
			, "direction"
		
			, "properMotion"
		
			, "sourceName"
				
		
			, "directionCode"
		
			, "directionEquinox"
		
			, "calibrationGroup"
		
			, "catalog"
		
			, "deltaVel"
		
			, "position"
		
			, "numLines"
		
			, "transition"
		
			, "restFrequency"
		
			, "sysVel"
		
			, "rangeVel"
		
			, "sourceModel"
		
			, "frequencyRefCode"
		
			, "numFreq"
		
			, "numStokes"
		
			, "frequency"
		
			, "frequencyInterval"
		
			, "stokesParameter"
		
			, "flux"
		
			, "fluxErr"
		
			, "positionAngle"
		
			, "positionAngleErr"
		
			, "size"
		
			, "sizeErr"
		
			, "velRefCode"
		
			, "dopplerVelocity"
		
			, "dopplerReferenceSystem"
		
			, "dopplerCalcType"
		
			, "parallax"
				
	};
	
	// A vector of string whose content is a copy of the strings in the array above.
	//
	static vector<string> attributesNamesOfSource_v (attributesNamesOfSource_a, attributesNamesOfSource_a + sizeof(attributesNamesOfSource_a) / sizeof(attributesNamesOfSource_a[0]));

	// An array of string containing the names of the columns of this table.
	// The array is filled in the order where the names would be read by default in the XML header of a file containing
	// the table exported in binary mode.
	//	
	static string attributesNamesInBinOfSource_a[] = {
    
    	 "sourceId" , "timeInterval" , "spectralWindowId" , "code" , "direction" , "properMotion" , "sourceName" 
    	,
    	 "directionCode" , "directionEquinox" , "calibrationGroup" , "catalog" , "deltaVel" , "position" , "numLines" , "transition" , "restFrequency" , "sysVel" , "rangeVel" , "sourceModel" , "frequencyRefCode" , "numFreq" , "numStokes" , "frequency" , "frequencyInterval" , "stokesParameter" , "flux" , "fluxErr" , "positionAngle" , "positionAngleErr" , "size" , "sizeErr" , "velRefCode" , "dopplerVelocity" , "dopplerReferenceSystem" , "dopplerCalcType" , "parallax" 
    
	};
	        			
	// A vector of string whose content is a copy of the strings in the array above.
	//
	static vector<string> attributesNamesInBinOfSource_v(attributesNamesInBinOfSource_a, attributesNamesInBinOfSource_a + sizeof(attributesNamesInBinOfSource_a) / sizeof(attributesNamesInBinOfSource_a[0]));		
	

	// The array of attributes (or column) names that make up key key.
	//
	string keyOfSource_a[] = {
	
		"sourceId"
	,
		"timeInterval"
	,
		"spectralWindowId"
		 
	};
	 
	// A vector of strings which are copies of those stored in the array above.
	vector<string> keyOfSource_v(keyOfSource_a, keyOfSource_a + sizeof(keyOfSource_a) / sizeof(keyOfSource_a[0]));

	/**
	 * Return the list of field names that make up key key
	 * as a const reference to a vector of strings.
	 */	
	const vector<string>& SourceTable::getKeyName() {
		return keyOfSource_v;
	}


	SourceTable::SourceTable(ASDM &c) : container(c) {

		// Define a default entity.
		entity.setEntityId(EntityId("uid://X0/X0/X0"));
		entity.setEntityIdEncrypted("na");
		entity.setEntityTypeName("SourceTable");
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
 * A destructor for SourceTable.
 */
	SourceTable::~SourceTable() {
		for (unsigned int i = 0; i < privateRows.size(); i++) 
			delete(privateRows.at(i));
	}

	/**
	 * Container to which this table belongs.
	 */
	ASDM &SourceTable::getContainer() const {
		return container;
	}

	/**
	 * Return the number of rows in the table.
	 */
	unsigned int SourceTable::size() const {
		if (presentInMemory) 
			return privateRows.size();
		else
			return declaredSize;
	}
	
	/**
	 * Return the name of this table.
	 */
	string SourceTable::getName() const {
		return entityNameOfSource;
	}
	
	/**
	 * Return the name of this table.
	 */
	string SourceTable::name() {
		return entityNameOfSource;
	}
	
	/**
	 * Return the the names of the attributes (or columns) of this table.
	 */
	const vector<string>& SourceTable::getAttributesNames() { return attributesNamesOfSource_v; }
	
	/**
	 * Return the the names of the attributes (or columns) of this table as they appear by default
	 * in an binary export of this table.
	 */
	const vector<string>& SourceTable::defaultAttributesNamesInBin() { return attributesNamesInBinOfSource_v; }

	/**
	 * Return this table's Entity.
	 */
	Entity SourceTable::getEntity() const {
		return entity;
	}

	/**
	 * Set this table's Entity.
	 */
	void SourceTable::setEntity(Entity e) {
		this->entity = e; 
	}
	
	//
	// ====> Row creation.
	//
	
	/**
	 * Create a new row.
	 */
	SourceRow *SourceTable::newRow() {
		return new SourceRow (*this);
	}
	

	/**
	 * Create a new row initialized to the specified values.
	 * @return a pointer on the created and initialized row.
	
 	 * @param timeInterval 
	
 	 * @param spectralWindowId 
	
 	 * @param code 
	
 	 * @param direction 
	
 	 * @param properMotion 
	
 	 * @param sourceName 
	
     */
	SourceRow* SourceTable::newRow(ArrayTimeInterval timeInterval, Tag spectralWindowId, std::string code, std::vector<Angle > direction, std::vector<AngularRate > properMotion, std::string sourceName){
		SourceRow *row = new SourceRow(*this);
			
		row->setTimeInterval(timeInterval);
			
		row->setSpectralWindowId(spectralWindowId);
			
		row->setCode(code);
			
		row->setDirection(direction);
			
		row->setProperMotion(properMotion);
			
		row->setSourceName(sourceName);
	
		return row;		
	}	
	


SourceRow* SourceTable::newRow(SourceRow* row) {
	return new SourceRow(*this, row);
}

	//
	// Append a row to its table.
	//

	
	
	
		
			

	/** 
	 * Returns a string built by concatenating the ascii representation of the
	 * parameters values suffixed with a "_" character.
	 */
	 string SourceTable::Key(Tag spectralWindowId) {
	 	ostringstream ostrstr;
	 		ostrstr  
			
				<< spectralWindowId.toString()  << "_"
			
			;
		return ostrstr.str();	 	
	 }

	/**
	 * Append a row to a SourceTable which has simply 
	 * 1) an autoincrementable attribute  (sourceId) 
	 * 2) a temporal attribute (timeInterval) in its key section.
	 * 3) other attributes in the key section (defining a so called context).
	 * If there is already a row in the table whose key section non including is equal to x's one and
	 * whose value section is equal to x's one then return this row, otherwise add x to the collection
	 * of rows.
	 */
	SourceRow* SourceTable::add(SourceRow* x) {
		// Get the start time of the row to be inserted.
		ArrayTime startTime = x->getTimeInterval().getStart();
		// cout << "Trying to add a new row with start time = " << startTime << endl;
		int insertionId = 0;

		 
		// Determine the entry in the context map from the appropriates attributes.
		string k = Key(
						x->getSpectralWindowId()
					   );
					   

		// Determine the insertion index for the row x, possibly returning a pointer to a row identical to x. 					   
		if (context.find(k) != context.end()) {
			// cout << "The context " << k << " already exists " << endl;
			for (unsigned int i = 0; i < context[k].size(); i++) {
				//cout << "Looking for a same starttime in i = " << i << endl;
				for (unsigned int j=0; j<context[k][i].size(); j++) 
					if (context[k][i][j]->getTimeInterval().getStart().equals(startTime)) {
						if (
						
						 (context[k][i][j]->getCode() == x->getCode())
						 && 

						 (context[k][i][j]->getDirection() == x->getDirection())
						 && 

						 (context[k][i][j]->getProperMotion() == x->getProperMotion())
						 && 

						 (context[k][i][j]->getSourceName() == x->getSourceName())
						
						) {
							// cout << "A row equal to x has been found, I return it " << endl;
							return context[k][i][j];
						}
						
						// Otherwise we must autoincrement sourceId and
						// insert a new SourceRow with this autoincremented value.
						insertionId = i+1;

						
						name2id_m[x->getSourceName()] = insertionId;
						
						break;
						
						// And goto insertion
						// goto done;
					}
			}
			//cout << "No row with the same start time than x, it will be inserted in row with id = 0" << endl;
			// insertionId = 0;
		}
		else { // There is not yet a context ...
			   // Create and initialize an entry in the context map for this combination....
			// cout << "Starting a new context " << k << endl;
			ID_TIME_ROWS vv;
			context[k] = vv;

		
			if (name2id_m.find(x->getSourceName()) == name2id_m.end()) {
				name2id_m[x->getSourceName()] = 0;
			}
			insertionId = name2id_m[x->getSourceName()];
						
		}
		
		
			x->setSourceId(insertionId);
		
			if (insertionId >= (int) context[k].size()) context[k].resize(insertionId+1);
			return insertByStartTime(x, context[k][insertionId]);
	}
	
		
	
		
	void SourceTable::addWithoutCheckingUnique(SourceRow * x) {
		SourceRow * dummy = checkAndAdd(x, true); // We require the check for uniqueness to be skipped.
		                                           // by passing true in the second parameter
		                                           // whose value by default is false.
		// this statement is never executed, but it hides the unused return value from the compiler to silence that warning.
                if (false) cout << (unsigned long long) dummy;
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
	SourceRow*  SourceTable::checkAndAdd(SourceRow* x, bool skipCheckUniqueness) {
		ArrayTime startTime = x->getTimeInterval().getStart();		
		
		// Determine the entry in the context map from the appropriate attributes.
		string k = Key(
		                x->getSpectralWindowId()
		               );

		if (!skipCheckUniqueness) {
			// Uniqueness Rule Check
			if (context.find(k) != context.end()) {
				for (unsigned int i = 0;  i < context[k].size(); i++) 
					for (unsigned int j = 0; j < context[k][i].size(); j++)
						if (
							(context[k][i][j]->getTimeInterval().getStart().equals(startTime)) 
					
							 && (context[k][i][j]->getCode() == x->getCode())
					

							 && (context[k][i][j]->getDirection() == x->getDirection())
					

							 && (context[k][i][j]->getProperMotion() == x->getProperMotion())
					

							 && (context[k][i][j]->getSourceName() == x->getSourceName())
					
						)
							throw UniquenessViolationException("Uniqueness violation exception in table SourceTable");			
			}
		}


		// Good, now it's time to insert the row x, possibly triggering a DuplicateKey exception.	
		
		ID_TIME_ROWS dummyPlane;

		// Determine the integer representation of the identifier of the row (x) to be inserted. 
		int id = 
				x->getSourceId();
				
	
		if (context.find(k) != context.end()) {
			if (id >= (int) context[k].size()) 
				context[k].resize(id+1);
			else {
				// This sourceId 's value has already rows for this context.
				// Check that there is not yet a row with the same time. (simply check start time)
				// If there is such a row then trigger a Duplicate Key Exception.
				for (unsigned int j = 0; j < context[k][id].size(); j++)
					if (context[k][id][j]->getTimeInterval().getStart().equals(startTime))
						throw DuplicateKey("Duplicate key exception in ", "SourceTable"); 
			}					
		}
		else {
			context[k] = dummyPlane;
			context[k].resize(id+1);
		}
		return insertByStartTime(x, context[k][id]);
	}
		



	//
	// A private method to brutally append a row to its table, without checking for row uniqueness.
	//

	void SourceTable::append(SourceRow *x) {
		privateRows.push_back(x);
		x->isAdded(true);
	}





	 vector<SourceRow *> SourceTable::get() {
	 	checkPresenceInMemory();
	    return privateRows;
	 }
	 
	 const vector<SourceRow *>& SourceTable::get() const {
	 	const_cast<SourceTable&>(*this).checkPresenceInMemory();	
	    return privateRows;
	 }	 
	 	




	

	


	
		
		
			
			 
/*
 ** Returns a SourceRow* given a key.
 ** @return a pointer to the row having the key whose values are passed as parameters, or 0 if
 ** no row exists for that key.
 **
 */
 	SourceRow* SourceTable::getRowByKey(int sourceId, ArrayTimeInterval timeInterval, Tag spectralWindowId)  {
 		checkPresenceInMemory();	
		ArrayTime start = timeInterval.getStart();
		
		map<string, ID_TIME_ROWS >::iterator mapIter;
		if ((mapIter = context.find(Key(spectralWindowId))) != context.end()) {
			
			int id = sourceId;
			
			if (id < (int) ((*mapIter).second).size()) {
				vector <SourceRow*>::iterator rowIter;
				for (rowIter = ((*mapIter).second)[id].begin(); rowIter != ((*mapIter).second)[id].end(); rowIter++) {
					if ((*rowIter)->getTimeInterval().contains(timeInterval))
						return *rowIter; 
				}
			}
		}
		return 0;
	}
/*
 * Returns a vector of pointers on rows whose key element sourceId 
 * is equal to the parameter sourceId.
 * @return a vector of vector <SourceRow *>. A returned vector of size 0 means that no row has been found.
 * @param sourceId int contains the value of
 * the autoincrementable attribute that is looked up in the table.
 */
 vector <SourceRow *>  SourceTable::getRowBySourceId(int sourceId) {
	checkPresenceInMemory();
	vector<SourceRow *> list;
	map<string, ID_TIME_ROWS >::iterator mapIter;
	
	for (mapIter=context.begin(); mapIter!=context.end(); mapIter++) {
		int maxId = ((*mapIter).second).size();
		if (sourceId < maxId) {
			vector<SourceRow *>::iterator rowIter;
			for (rowIter=((*mapIter).second)[sourceId].begin(); 
			     rowIter!=((*mapIter).second)[sourceId].end(); rowIter++)
				list.push_back(*rowIter);
		}
	}
	return list;	
 }			
			
		
		
		
/**
 * Look up the table for a row whose all attributes  except the autoincrementable one 
 * are equal to the corresponding parameters of the method.
 * @return a pointer on this row if any, 0 otherwise.
 *
			
 * @param <<ASDMAttribute>> timeInterval.
 	 		
 * @param <<ExtrinsicAttribute>> spectralWindowId.
 	 		
 * @param <<ASDMAttribute>> code.
 	 		
 * @param <<ArrayAttribute>> direction.
 	 		
 * @param <<ArrayAttribute>> properMotion.
 	 		
 * @param <<ASDMAttribute>> sourceName.
 	 		 
 */
SourceRow* SourceTable::lookup(ArrayTimeInterval timeInterval, Tag spectralWindowId, std::string code, std::vector<Angle > direction, std::vector<AngularRate > properMotion, std::string sourceName) {	
		using asdm::ArrayTimeInterval;
		map<string, ID_TIME_ROWS >::iterator mapIter;
		string k = Key(spectralWindowId);
		if ((mapIter = context.find(k)) != context.end()) {
			ID_TIME_ROWS::iterator planeIter;
			for (planeIter = context[k].begin(); planeIter != context[k].end(); planeIter++)  {
				vector <SourceRow*>::iterator rowIter;
				for (rowIter = (*planeIter).begin(); rowIter != (*planeIter).end(); rowIter++) {
					if ((*rowIter)->getTimeInterval().contains(timeInterval)
					    && (*rowIter)->compareRequiredValue(code, direction, properMotion, sourceName)) {
						return *rowIter;
					} 
				}
			}
		}				
		return 0;	
} 
		
	



#ifndef WITHOUT_ACS
	using asdmIDL::SourceTableIDL;
#endif

#ifndef WITHOUT_ACS
	// Conversion Methods

	SourceTableIDL *SourceTable::toIDL() {
		SourceTableIDL *x = new SourceTableIDL ();
		unsigned int nrow = size();
		x->row.length(nrow);
		vector<SourceRow*> v = get();
		for (unsigned int i = 0; i < nrow; ++i) {
			//x->row[i] = *(v[i]->toIDL());
			v[i]->toIDL(x->row[i]);
		}
		return x;
	}
	
	void SourceTable::toIDL(asdmIDL::SourceTableIDL& x) const {
		unsigned int nrow = size();
		x.row.length(nrow);
		vector<SourceRow*> v = get();
		for (unsigned int i = 0; i < nrow; ++i) {
			v[i]->toIDL(x.row[i]);
		}
	}	
#endif
	
#ifndef WITHOUT_ACS
	void SourceTable::fromIDL(SourceTableIDL x) {
		unsigned int nrow = x.row.length();
		for (unsigned int i = 0; i < nrow; ++i) {
			SourceRow *tmp = newRow();
			tmp->setFromIDL(x.row[i]);
			// checkAndAdd(tmp);
			add(tmp);
		}
	}	
#endif

	
	string SourceTable::toXML()  {
		string buf;

		buf.append("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?> ");
		buf.append("<SourceTable xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:src=\"http://Alma/XASDM/SourceTable\" xsi:schemaLocation=\"http://Alma/XASDM/SourceTable http://almaobservatory.org/XML/XASDM/4/SourceTable.xsd\" schemaVersion=\"4\" schemaRevision=\"-1\">\n");
	
		buf.append(entity.toXML());
		string s = container.getEntity().toXML();
		// Change the "Entity" tag to "ContainerEntity".
		buf.append("<Container" + s.substr(1,s.length() - 1)+" ");
		vector<SourceRow*> v = get();
		for (unsigned int i = 0; i < v.size(); ++i) {
			try {
				buf.append(v[i]->toXML());
			} catch (const NoSuchRow &e) {
			}
			buf.append("  ");
		}		
		buf.append("</SourceTable> ");
		return buf;
	}

	
	string SourceTable::getVersion() const {
		return version;
	}
	

	void SourceTable::fromXML(string& tableInXML)  {
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
			throw ConversionException("Failed to parse the xmlHeader into a DOM structure.", "Source");
		
		xmlNode* root_element = xmlDocGetRootElement(doc);
   		if ( root_element == NULL || root_element->type != XML_ELEMENT_NODE )
      		throw ConversionException("Failed to retrieve the root element in the DOM structure.", "Source");
      		
      	xmlChar * propValue = xmlGetProp(root_element, (const xmlChar *) "schemaVersion");
      	if ( propValue != 0 ) {
      		version = string( (const char*) propValue);
      		xmlFree(propValue);   		
      	}
      		     							
		Parser xml(tableInXML);
		if (!xml.isStr("<SourceTable")) 
			error();
		// cout << "Parsing a SourceTable" << endl;
		string s = xml.getElement("<Entity","/>");
		if (s.length() == 0) 
			error();
		Entity e;
		e.setFromXML(s);
		if (e.getEntityTypeName() != "SourceTable")
			error();
		setEntity(e);
		// Skip the container's entity; but, it has to be there.
		s = xml.getElement("<ContainerEntity","/>");
		if (s.length() == 0) 
			error();

		// Get each row in the table.
		s = xml.getElementContent("<row>","</row>");
		SourceRow *row;
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
				throw ConversionException(e1.getMessage(),"SourceTable");
			} 
			catch (const UniquenessViolationException &e1) {
				throw ConversionException(e1.getMessage(),"SourceTable");	
			}
			catch (...) {
				// cout << "Unexpected error in SourceTable::checkAndAdd called from SourceTable::fromXML " << endl;
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
				throw ConversionException(e1.getMessage(),"SourceTable");
			} 
			catch (...) {
				// cout << "Unexpected error in SourceTable::addWithoutCheckingUnique called from SourceTable::fromXML " << endl;
			}
		}				
				
				
		if (!xml.isStr("</SourceTable>")) 
		error();
		
		//Does not change the convention defined in the model.	
		//archiveAsBin = false;
		//fileAsBin = false;

                // clean up the xmlDoc pointer
		if ( doc != NULL ) xmlFreeDoc(doc);
		
	}

	
	void SourceTable::error()  {
		throw ConversionException("Invalid xml document","Source");
	}
	
	
	string SourceTable::MIMEXMLPart(const asdm::ByteOrder* byteOrder) {
		string UID = getEntity().getEntityId().toString();
		string withoutUID = UID.substr(6);
		string containerUID = getContainer().getEntity().getEntityId().toString();
		ostringstream oss;
		oss << "<?xml version='1.0'  encoding='ISO-8859-1'?>";
		oss << "\n";
		oss << "<SourceTable xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:src=\"http://Alma/XASDM/SourceTable\" xsi:schemaLocation=\"http://Alma/XASDM/SourceTable http://almaobservatory.org/XML/XASDM/4/SourceTable.xsd\" schemaVersion=\"4\" schemaRevision=\"-1\">\n";
		oss<< "<Entity entityId='"<<UID<<"' entityIdEncrypted='na' entityTypeName='SourceTable' schemaVersion='1' documentVersion='1'/>\n";
		oss<< "<ContainerEntity entityId='"<<containerUID<<"' entityIdEncrypted='na' entityTypeName='ASDM' schemaVersion='1' documentVersion='1'/>\n";
		oss << "<BulkStoreRef file_id='"<<withoutUID<<"' byteOrder='"<<byteOrder->toString()<<"' />\n";
		oss << "<Attributes>\n";

		oss << "<sourceId/>\n"; 
		oss << "<timeInterval/>\n"; 
		oss << "<spectralWindowId/>\n"; 
		oss << "<code/>\n"; 
		oss << "<direction/>\n"; 
		oss << "<properMotion/>\n"; 
		oss << "<sourceName/>\n"; 

		oss << "<directionCode/>\n"; 
		oss << "<directionEquinox/>\n"; 
		oss << "<calibrationGroup/>\n"; 
		oss << "<catalog/>\n"; 
		oss << "<deltaVel/>\n"; 
		oss << "<position/>\n"; 
		oss << "<numLines/>\n"; 
		oss << "<transition/>\n"; 
		oss << "<restFrequency/>\n"; 
		oss << "<sysVel/>\n"; 
		oss << "<rangeVel/>\n"; 
		oss << "<sourceModel/>\n"; 
		oss << "<frequencyRefCode/>\n"; 
		oss << "<numFreq/>\n"; 
		oss << "<numStokes/>\n"; 
		oss << "<frequency/>\n"; 
		oss << "<frequencyInterval/>\n"; 
		oss << "<stokesParameter/>\n"; 
		oss << "<flux/>\n"; 
		oss << "<fluxErr/>\n"; 
		oss << "<positionAngle/>\n"; 
		oss << "<positionAngleErr/>\n"; 
		oss << "<size/>\n"; 
		oss << "<sizeErr/>\n"; 
		oss << "<velRefCode/>\n"; 
		oss << "<dopplerVelocity/>\n"; 
		oss << "<dopplerReferenceSystem/>\n"; 
		oss << "<dopplerCalcType/>\n"; 
		oss << "<parallax/>\n"; 
		oss << "</Attributes>\n";		
		oss << "</SourceTable>\n";

		return oss.str();				
	}
	
	string SourceTable::toMIME(const asdm::ByteOrder* byteOrder) {
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

	
	void SourceTable::setFromMIME(const string & mimeMsg) {
    string xmlPartMIMEHeader = "Content-ID: <header.xml>\n\n";
    
    string binPartMIMEHeader = "--MIME_boundary\nContent-Type: binary/octet-stream\nContent-ID: <content.bin>\n\n";
    
    // Detect the XML header.
    string::size_type loc0 = mimeMsg.find(xmlPartMIMEHeader, 0);
    if ( loc0 == string::npos) {
      // let's try with CRLFs
      xmlPartMIMEHeader = "Content-ID: <header.xml>\r\n\r\n";
      loc0 = mimeMsg.find(xmlPartMIMEHeader, 0);
      if  ( loc0 == string::npos ) 
	      throw ConversionException("Failed to detect the beginning of the XML header", "Source");
    }

    loc0 += xmlPartMIMEHeader.size();
    
    // Look for the string announcing the binary part.
    string::size_type loc1 = mimeMsg.find( binPartMIMEHeader, loc0 );
    
    if ( loc1 == string::npos ) {
      throw ConversionException("Failed to detect the beginning of the binary part", "Source");
    }
    
    //
    // Extract the xmlHeader and analyze it to find out what is the byte order and the sequence
    // of attribute names.
    //
    string xmlHeader = mimeMsg.substr(loc0, loc1-loc0);
    xmlDoc *doc;
    doc = xmlReadMemory(xmlHeader.data(), xmlHeader.size(), "BinaryTableHeader.xml", NULL, XML_PARSE_NOBLANKS);
    if ( doc == NULL ) 
      throw ConversionException("Failed to parse the xmlHeader into a DOM structure.", "Source");
    
   // This vector will be filled by the names of  all the attributes of the table
   // in the order in which they are expected to be found in the binary representation.
   //
    vector<string> attributesSeq;
      
    xmlNode* root_element = xmlDocGetRootElement(doc);
    if ( root_element == NULL || root_element->type != XML_ELEMENT_NODE )
      throw ConversionException("Failed to parse the xmlHeader into a DOM structure.", "Source");
    
    const ByteOrder* byteOrder=0;
    if ( string("ASDMBinaryTable").compare((const char*) root_element->name) == 0) {
      // Then it's an "old fashioned" MIME file for tables.
      // Just try to deserialize it with Big_Endian for the bytes ordering.
      byteOrder = asdm::ByteOrder::Big_Endian;
      
 	 //
    // Let's consider a  default order for the sequence of attributes.
    //
    
    	 
    attributesSeq.push_back("sourceId") ; 
    	 
    attributesSeq.push_back("timeInterval") ; 
    	 
    attributesSeq.push_back("spectralWindowId") ; 
    	 
    attributesSeq.push_back("code") ; 
    	 
    attributesSeq.push_back("direction") ; 
    	 
    attributesSeq.push_back("properMotion") ; 
    	 
    attributesSeq.push_back("sourceName") ; 
    	
    	 
    attributesSeq.push_back("directionCode") ; 
    	 
    attributesSeq.push_back("directionEquinox") ; 
    	 
    attributesSeq.push_back("calibrationGroup") ; 
    	 
    attributesSeq.push_back("catalog") ; 
    	 
    attributesSeq.push_back("deltaVel") ; 
    	 
    attributesSeq.push_back("position") ; 
    	 
    attributesSeq.push_back("numLines") ; 
    	 
    attributesSeq.push_back("transition") ; 
    	 
    attributesSeq.push_back("restFrequency") ; 
    	 
    attributesSeq.push_back("sysVel") ; 
    	 
    attributesSeq.push_back("rangeVel") ; 
    	 
    attributesSeq.push_back("sourceModel") ; 
    	 
    attributesSeq.push_back("frequencyRefCode") ; 
    	 
    attributesSeq.push_back("numFreq") ; 
    	 
    attributesSeq.push_back("numStokes") ; 
    	 
    attributesSeq.push_back("frequency") ; 
    	 
    attributesSeq.push_back("frequencyInterval") ; 
    	 
    attributesSeq.push_back("stokesParameter") ; 
    	 
    attributesSeq.push_back("flux") ; 
    	 
    attributesSeq.push_back("fluxErr") ; 
    	 
    attributesSeq.push_back("positionAngle") ; 
    	 
    attributesSeq.push_back("positionAngleErr") ; 
    	 
    attributesSeq.push_back("size") ; 
    	 
    attributesSeq.push_back("sizeErr") ; 
    	 
    attributesSeq.push_back("velRefCode") ; 
    	 
    attributesSeq.push_back("dopplerVelocity") ; 
    	 
    attributesSeq.push_back("dopplerReferenceSystem") ; 
    	 
    attributesSeq.push_back("dopplerCalcType") ; 
    	 
    attributesSeq.push_back("parallax") ; 
    	
     
    
    
    // And decide that it has version == "2"
    version = "2";         
     }
    else if (string("SourceTable").compare((const char*) root_element->name) == 0) {
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
      	throw ConversionException ("Could not find the element '/SourceTable/BulkStoreRef'. Invalid XML header '"+ xmlHeader + "'.", "Source");
      	
      // We found BulkStoreRef, now look for its attribute byteOrder.
      _xmlAttr* byteOrderAttr = 0;
      for (struct _xmlAttr* attr = bulkStoreRef->properties; attr; attr = attr->next) 
	  if (string("byteOrder").compare((const char*) attr->name) == 0) {
	   byteOrderAttr = attr;
	   break;
	 }
      
      if (byteOrderAttr == 0) 
	     throw ConversionException("Could not find the element '/SourceTable/BulkStoreRef/@byteOrder'. Invalid XML header '" + xmlHeader +"'.", "Source");
      
      string byteOrderValue = string((const char*) byteOrderAttr->children->content);
      if (!(byteOrder = asdm::ByteOrder::fromString(byteOrderValue)))
		throw ConversionException("No valid value retrieved for the element '/SourceTable/BulkStoreRef/@byteOrder'. Invalid XML header '" + xmlHeader + "'.", "Source");
		
	 //
	 // 2nd) Look for the Attributes element and grab the names of the elements it contains.
	 //
	 xmlNode* attributes = bulkStoreRef->next;
     if ( attributes == 0 || (attributes->type != XML_ELEMENT_NODE)  || (string("Attributes").compare((const char*) attributes->name) != 0))	 
       	throw ConversionException ("Could not find the element '/SourceTable/Attributes'. Invalid XML header '"+ xmlHeader + "'.", "Source");
 
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
				SourceRow* aRow = SourceRow::fromBin((EndianIStream&) eiss, *this, attributesSeq);
				checkAndAdd(aRow);
      		}
    	}
    	catch (const DuplicateKey &e) {
      		throw ConversionException("Error while writing binary data , the message was "
				+ e.getMessage(), "Source");
    	}
    	catch (const TagFormatException &e) {
     		 throw ConversionException("Error while reading binary data , the message was "
				+ e.getMessage(), "Source");
    	}
    }
    else {
 		for (uint32_t i = 0; i < this->declaredSize; i++) {
			SourceRow* aRow = SourceRow::fromBin((EndianIStream&) eiss, *this, attributesSeq);
			append(aRow);
      	}   	
    }
    //Does not change the convention defined in the model.	
    //archiveAsBin = true;
    //fileAsBin = true;
    if ( doc != NULL ) xmlFreeDoc(doc);

	}
	
	void SourceTable::setUnknownAttributeBinaryReader(const string& attributeName, BinaryAttributeReaderFunctor* barFctr) {
		//
		// Is this attribute really unknown ?
		//
		for (vector<string>::const_iterator iter = attributesNamesOfSource_v.begin(); iter != attributesNamesOfSource_v.end(); iter++) {
			if ((*iter).compare(attributeName) == 0) 
				throw ConversionException("the attribute '"+attributeName+"' is known you can't override the way it's read in the MIME binary file containing the table.", "Source"); 
		}
		
		// Ok then register the functor to activate when an unknown attribute is met during the reading of a binary table?
		unknownAttributes2Functors[attributeName] = barFctr;
	}
	
	BinaryAttributeReaderFunctor* SourceTable::getUnknownAttributeBinaryReader(const string& attributeName) const {
		map<string, BinaryAttributeReaderFunctor*>::const_iterator iter = unknownAttributes2Functors.find(attributeName);
		return (iter == unknownAttributes2Functors.end()) ? 0 : iter->second;
	}

	
	void SourceTable::toFile(string directory) {
		if (!directoryExists(directory.c_str()) &&
			!createPath(directory.c_str())) {
			throw ConversionException("Could not create directory " , directory);
		}

		string fileName = directory + "/Source.xml";
		ofstream tableout(fileName.c_str(),ios::out|ios::trunc);
		if (tableout.rdstate() == ostream::failbit)
			throw ConversionException("Could not open file " + fileName + " to write ", "Source");
		if (fileAsBin) 
			tableout << MIMEXMLPart();
		else
			tableout << toXML() << endl;
		tableout.close();
		if (tableout.rdstate() == ostream::failbit)
			throw ConversionException("Could not close file " + fileName, "Source");

		if (fileAsBin) {
			// write the bin serialized
			string fileName = directory + "/Source.bin";
			ofstream tableout(fileName.c_str(),ios::out|ios::trunc);
			if (tableout.rdstate() == ostream::failbit)
				throw ConversionException("Could not open file " + fileName + " to write ", "Source");
			tableout << toMIME() << endl;
			tableout.close();
			if (tableout.rdstate() == ostream::failbit)
				throw ConversionException("Could not close file " + fileName, "Source");
		}
	}

	
	void SourceTable::setFromFile(const string& directory) {
#ifndef WITHOUT_BOOST
    if (boost::filesystem::exists(boost::filesystem::path(uniqSlashes(directory + "/Source.xml"))))
      setFromXMLFile(directory);
    else if (boost::filesystem::exists(boost::filesystem::path(uniqSlashes(directory + "/Source.bin"))))
      setFromMIMEFile(directory);
#else 
    // alternative in Misc.h
    if (file_exists(uniqSlashes(directory + "/Source.xml")))
      setFromXMLFile(directory);
    else if (file_exists(uniqSlashes(directory + "/Source.bin")))
      setFromMIMEFile(directory);
#endif
    else
      throw ConversionException("No file found for the Source table", "Source");
	}			

	
  void SourceTable::setFromMIMEFile(const string& directory) {
    string tablePath ;
    
    tablePath = directory + "/Source.bin";
    ifstream tablefile(tablePath.c_str(), ios::in|ios::binary);
    if (!tablefile.is_open()) { 
      throw ConversionException("Could not open file " + tablePath, "Source");
    }
    // Read in a stringstream.
    stringstream ss; ss << tablefile.rdbuf();
    
    if (tablefile.rdstate() == istream::failbit || tablefile.rdstate() == istream::badbit) {
      throw ConversionException("Error reading file " + tablePath,"Source");
    }
    
    // And close.
    tablefile.close();
    if (tablefile.rdstate() == istream::failbit)
      throw ConversionException("Could not close file " + tablePath,"Source");
    
    setFromMIME(ss.str());
  }	
/* 
  void SourceTable::openMIMEFile (const string& directory) {
  		
  	// Open the file.
  	string tablePath ;
    tablePath = directory + "/Source.bin";
    ifstream tablefile(tablePath.c_str(), ios::in|ios::binary);
    if (!tablefile.is_open())
      throw ConversionException("Could not open file " + tablePath, "Source");
      
	// Locate the xmlPartMIMEHeader.
    string xmlPartMIMEHeader = "CONTENT-ID: <HEADER.XML>\n\n";
    CharComparator comparator;
    istreambuf_iterator<char> BEGIN(tablefile.rdbuf());
    istreambuf_iterator<char> END;
    istreambuf_iterator<char> it = search(BEGIN, END, xmlPartMIMEHeader.begin(), xmlPartMIMEHeader.end(), comparator);
    if (it == END) 
    	throw ConversionException("failed to detect the beginning of the XML header", "Source");
    
    // Locate the binaryPartMIMEHeader while accumulating the characters of the xml header.	
    string binPartMIMEHeader = "--MIME_BOUNDARY\nCONTENT-TYPE: BINARY/OCTET-STREAM\nCONTENT-ID: <CONTENT.BIN>\n\n";
    string xmlHeader;
   	CharCompAccumulator compaccumulator(&xmlHeader, 100000);
   	++it;
   	it = search(it, END, binPartMIMEHeader.begin(), binPartMIMEHeader.end(), compaccumulator);
   	if (it == END) 
   		throw ConversionException("failed to detect the beginning of the binary part", "Source");
   	
	cout << xmlHeader << endl;
	//
	// We have the xmlHeader , let's parse it.
	//
	xmlDoc *doc;
    doc = xmlReadMemory(xmlHeader.data(), xmlHeader.size(), "BinaryTableHeader.xml", NULL, XML_PARSE_NOBLANKS);
    if ( doc == NULL ) 
      throw ConversionException("Failed to parse the xmlHeader into a DOM structure.", "Source");
    
   // This vector will be filled by the names of  all the attributes of the table
   // in the order in which they are expected to be found in the binary representation.
   //
    vector<string> attributesSeq(attributesNamesInBinOfSource_v);
      
    xmlNode* root_element = xmlDocGetRootElement(doc);
    if ( root_element == NULL || root_element->type != XML_ELEMENT_NODE )
      throw ConversionException("Failed to parse the xmlHeader into a DOM structure.", "Source");
    
    const ByteOrder* byteOrder=0;
    if ( string("ASDMBinaryTable").compare((const char*) root_element->name) == 0) {
      // Then it's an "old fashioned" MIME file for tables.
      // Just try to deserialize it with Big_Endian for the bytes ordering.
      byteOrder = asdm::ByteOrder::Big_Endian;
        
      // And decide that it has version == "2"
    version = "2";         
     }
    else if (string("SourceTable").compare((const char*) root_element->name) == 0) {
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
      	throw ConversionException ("Could not find the element '/SourceTable/BulkStoreRef'. Invalid XML header '"+ xmlHeader + "'.", "Source");
      	
      // We found BulkStoreRef, now look for its attribute byteOrder.
      _xmlAttr* byteOrderAttr = 0;
      for (struct _xmlAttr* attr = bulkStoreRef->properties; attr; attr = attr->next) 
	  if (string("byteOrder").compare((const char*) attr->name) == 0) {
	   byteOrderAttr = attr;
	   break;
	 }
      
      if (byteOrderAttr == 0) 
	     throw ConversionException("Could not find the element '/SourceTable/BulkStoreRef/@byteOrder'. Invalid XML header '" + xmlHeader +"'.", "Source");
      
      string byteOrderValue = string((const char*) byteOrderAttr->children->content);
      if (!(byteOrder = asdm::ByteOrder::fromString(byteOrderValue)))
		throw ConversionException("No valid value retrieved for the element '/SourceTable/BulkStoreRef/@byteOrder'. Invalid XML header '" + xmlHeader + "'.", "Source");
		
	 //
	 // 2nd) Look for the Attributes element and grab the names of the elements it contains.
	 //
	 xmlNode* attributes = bulkStoreRef->next;
     if ( attributes == 0 || (attributes->type != XML_ELEMENT_NODE)  || (string("Attributes").compare((const char*) attributes->name) != 0))	 
       	throw ConversionException ("Could not find the element '/SourceTable/Attributes'. Invalid XML header '"+ xmlHeader + "'.", "Source");
 
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

	
void SourceTable::setFromXMLFile(const string& directory) {
    string tablePath ;
    
    tablePath = directory + "/Source.xml";
    
    /*
    ifstream tablefile(tablePath.c_str(), ios::in|ios::binary);
    if (!tablefile.is_open()) { 
      throw ConversionException("Could not open file " + tablePath, "Source");
    }
      // Read in a stringstream.
    stringstream ss;
    ss << tablefile.rdbuf();
    
    if  (tablefile.rdstate() == istream::failbit || tablefile.rdstate() == istream::badbit) {
      throw ConversionException("Error reading file '" + tablePath + "'", "Source");
    }
    
    // And close
    tablefile.close();
    if (tablefile.rdstate() == istream::failbit)
      throw ConversionException("Could not close file '" + tablePath + "'", "Source");

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
    	throw ConversionException("Caugth an exception whose message is '" + e.getMessage() + "'.", "Source");
    }
    
    if (xmlDocument.find("<BulkStoreRef") != string::npos)
      setFromMIMEFile(directory);
    else
      fromXML(xmlDocument);
  }

	

	

			
	
		
		
	/**
	 * Insert a SourceRow* in a vector of SourceRow* so that it's ordered by ascending start time.
	 *
	 * @param SourceRow* x . The pointer to be inserted.
	 * @param vector <SourceRow*>& row. A reference to the vector where to insert x.
	 *
	 */
	 SourceRow* SourceTable::insertByStartTime(SourceRow* x, vector<SourceRow*>& row) {
				
		vector <SourceRow*>::iterator theIterator;
		
		ArrayTime start = x->timeInterval.getStart();

    	// Is the row vector empty ?
    	if (row.size() == 0) {
    		row.push_back(x);
    		privateRows.push_back(x);
    		x->isAdded(true);
    		return x;
    	}
    	
    	// Optimization for the case of insertion by ascending time.
    	SourceRow* last = *(row.end()-1);
        
    	if ( start > last->timeInterval.getStart() ) {
 	    	//
	    	// Modify the duration of last if and only if the start time of x
	    	// is located strictly before the end time of last.
	    	//
	  		if ( start < (last->timeInterval.getStart() + last->timeInterval.getDuration()))   		
    			last->timeInterval.setDuration(start - last->timeInterval.getStart());
    		row.push_back(x);
    		privateRows.push_back(x);
    		x->isAdded(true);
    		return x;
    	}
    	
    	// Optimization for the case of insertion by descending time.
    	SourceRow* first = *(row.begin());
        
    	if ( start < first->timeInterval.getStart() ) {
			//
	  		// Modify the duration of x if and only if the start time of first
	  		// is located strictly before the end time of x.
	  		//
	  		if ( first->timeInterval.getStart() < (start + x->timeInterval.getDuration()) )	  		
    			x->timeInterval.setDuration(first->timeInterval.getStart() - start);
    		row.insert(row.begin(), x);
    		privateRows.push_back(x);
    		x->isAdded(true);
    		return x;
    	}
    	
    	// Case where x has to be inserted inside row; let's use a dichotomy
    	// method to find the insertion index.
		unsigned int k0 = 0;
		unsigned int k1 = row.size() - 1;
	
		while (k0 != (k1 - 1)) {
			if (start == row[k0]->timeInterval.getStart()) {
				if (row[k0]->equalByRequiredValue(x))
					return row[k0];
				else
					throw DuplicateKey("DuplicateKey exception in ", "SourceTable");	
			}
			else if (start == row[k1]->timeInterval.getStart()) {
				if (row[k1]->equalByRequiredValue(x))
					return row[k1];
				else
					throw DuplicateKey("DuplicateKey exception in ", "SourceTable");	
			}
			else {
				if (start <= row[(k0+k1)/2]->timeInterval.getStart())
					k1 = (k0 + k1) / 2;
				else
					k0 = (k0 + k1) / 2;				
			} 	
		}

		if (start == row[k0]->timeInterval.getStart()) {
			if (row[k0]->equalByRequiredValue(x))
				return row[k0];
			else
				throw DuplicateKey("DuplicateKey exception in ", "SourceTable");	
		}
		else if (start == row[k1]->timeInterval.getStart()) {
			if (row[k1]->equalByRequiredValue(x))
				return row[k1];
			else
				throw DuplicateKey("DuplicateKey exception in ", "SourceTable");	
		}	

		row[k0]->timeInterval.setDuration(start-row[k0]->timeInterval.getStart());
		x->timeInterval.setDuration(row[k0+1]->timeInterval.getStart() - start);
		row.insert(row.begin()+(k0+1), x);
		privateRows.push_back(x);
   		x->isAdded(true);
		return x;   
    } 
    	
	
	

	
} // End namespace asdm
 
