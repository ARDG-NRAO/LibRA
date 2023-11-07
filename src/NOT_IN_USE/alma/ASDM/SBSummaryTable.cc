
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
 * File SBSummaryTable.cpp
 */
#include <alma/ASDM/ConversionException.h>
#include <alma/ASDM/DuplicateKey.h>
#include <alma/ASDM/OutOfBoundsException.h>

using asdm::ConversionException;
using asdm::DuplicateKey;
using asdm::OutOfBoundsException;

#include <alma/ASDM/ASDM.h>
#include <alma/ASDM/SBSummaryTable.h>
#include <alma/ASDM/SBSummaryRow.h>
#include <alma/ASDM/Parser.h>

using asdm::ASDM;
using asdm::SBSummaryTable;
using asdm::SBSummaryRow;
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
	static string entityNameOfSBSummary = "SBSummary";
	
	// An array of string containing the names of the columns of this table.
	// The array is filled in the order : key, required value, optional value.
	//
	static string attributesNamesOfSBSummary_a[] = {
		
			"sBSummaryId"
		
		
			, "sbSummaryUID"
		
			, "projectUID"
		
			, "obsUnitSetUID"
		
			, "frequency"
		
			, "frequencyBand"
		
			, "sbType"
		
			, "sbDuration"
		
			, "numObservingMode"
		
			, "observingMode"
		
			, "numberRepeats"
		
			, "numScienceGoal"
		
			, "scienceGoal"
		
			, "numWeatherConstraint"
		
			, "weatherConstraint"
				
		
			, "centerDirection"
		
			, "centerDirectionCode"
		
			, "centerDirectionEquinox"
				
	};
	
	// A vector of string whose content is a copy of the strings in the array above.
	//
	static vector<string> attributesNamesOfSBSummary_v (attributesNamesOfSBSummary_a, attributesNamesOfSBSummary_a + sizeof(attributesNamesOfSBSummary_a) / sizeof(attributesNamesOfSBSummary_a[0]));

	// An array of string containing the names of the columns of this table.
	// The array is filled in the order where the names would be read by default in the XML header of a file containing
	// the table exported in binary mode.
	//	
	static string attributesNamesInBinOfSBSummary_a[] = {
    
    	 "sBSummaryId" , "sbSummaryUID" , "projectUID" , "obsUnitSetUID" , "frequency" , "frequencyBand" , "sbType" , "sbDuration" , "numObservingMode" , "observingMode" , "numberRepeats" , "numScienceGoal" , "scienceGoal" , "numWeatherConstraint" , "weatherConstraint" 
    	,
    	 "centerDirection" , "centerDirectionCode" , "centerDirectionEquinox" 
    
	};
	        			
	// A vector of string whose content is a copy of the strings in the array above.
	//
	static vector<string> attributesNamesInBinOfSBSummary_v(attributesNamesInBinOfSBSummary_a, attributesNamesInBinOfSBSummary_a + sizeof(attributesNamesInBinOfSBSummary_a) / sizeof(attributesNamesInBinOfSBSummary_a[0]));		
	

	// The array of attributes (or column) names that make up key key.
	//
	string keyOfSBSummary_a[] = {
	
		"sBSummaryId"
		 
	};
	 
	// A vector of strings which are copies of those stored in the array above.
	vector<string> keyOfSBSummary_v(keyOfSBSummary_a, keyOfSBSummary_a + sizeof(keyOfSBSummary_a) / sizeof(keyOfSBSummary_a[0]));

	/**
	 * Return the list of field names that make up key key
	 * as a const reference to a vector of strings.
	 */	
	const vector<string>& SBSummaryTable::getKeyName() {
		return keyOfSBSummary_v;
	}


	SBSummaryTable::SBSummaryTable(ASDM &c) : container(c) {

		// Define a default entity.
		entity.setEntityId(EntityId("uid://X0/X0/X0"));
		entity.setEntityIdEncrypted("na");
		entity.setEntityTypeName("SBSummaryTable");
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
 * A destructor for SBSummaryTable.
 */
	SBSummaryTable::~SBSummaryTable() {
		for (unsigned int i = 0; i < privateRows.size(); i++) 
			delete(privateRows.at(i));
	}

	/**
	 * Container to which this table belongs.
	 */
	ASDM &SBSummaryTable::getContainer() const {
		return container;
	}

	/**
	 * Return the number of rows in the table.
	 */
	unsigned int SBSummaryTable::size() const {
		if (presentInMemory) 
			return privateRows.size();
		else
			return declaredSize;
	}
	
	/**
	 * Return the name of this table.
	 */
	string SBSummaryTable::getName() const {
		return entityNameOfSBSummary;
	}
	
	/**
	 * Return the name of this table.
	 */
	string SBSummaryTable::name() {
		return entityNameOfSBSummary;
	}
	
	/**
	 * Return the the names of the attributes (or columns) of this table.
	 */
	const vector<string>& SBSummaryTable::getAttributesNames() { return attributesNamesOfSBSummary_v; }
	
	/**
	 * Return the the names of the attributes (or columns) of this table as they appear by default
	 * in an binary export of this table.
	 */
	const vector<string>& SBSummaryTable::defaultAttributesNamesInBin() { return attributesNamesInBinOfSBSummary_v; }

	/**
	 * Return this table's Entity.
	 */
	Entity SBSummaryTable::getEntity() const {
		return entity;
	}

	/**
	 * Set this table's Entity.
	 */
	void SBSummaryTable::setEntity(Entity e) {
		this->entity = e; 
	}
	
	//
	// ====> Row creation.
	//
	
	/**
	 * Create a new row.
	 */
	SBSummaryRow *SBSummaryTable::newRow() {
		return new SBSummaryRow (*this);
	}
	

	/**
	 * Create a new row initialized to the specified values.
	 * @return a pointer on the created and initialized row.
	
 	 * @param sbSummaryUID 
	
 	 * @param projectUID 
	
 	 * @param obsUnitSetUID 
	
 	 * @param frequency 
	
 	 * @param frequencyBand 
	
 	 * @param sbType 
	
 	 * @param sbDuration 
	
 	 * @param numObservingMode 
	
 	 * @param observingMode 
	
 	 * @param numberRepeats 
	
 	 * @param numScienceGoal 
	
 	 * @param scienceGoal 
	
 	 * @param numWeatherConstraint 
	
 	 * @param weatherConstraint 
	
     */
	SBSummaryRow* SBSummaryTable::newRow(EntityRef sbSummaryUID, EntityRef projectUID, EntityRef obsUnitSetUID, double frequency, ReceiverBandMod::ReceiverBand frequencyBand, SBTypeMod::SBType sbType, Interval sbDuration, int numObservingMode, std::vector<std::string > observingMode, int numberRepeats, int numScienceGoal, std::vector<std::string > scienceGoal, int numWeatherConstraint, std::vector<std::string > weatherConstraint){
		SBSummaryRow *row = new SBSummaryRow(*this);
			
		row->setSbSummaryUID(sbSummaryUID);
			
		row->setProjectUID(projectUID);
			
		row->setObsUnitSetUID(obsUnitSetUID);
			
		row->setFrequency(frequency);
			
		row->setFrequencyBand(frequencyBand);
			
		row->setSbType(sbType);
			
		row->setSbDuration(sbDuration);
			
		row->setNumObservingMode(numObservingMode);
			
		row->setObservingMode(observingMode);
			
		row->setNumberRepeats(numberRepeats);
			
		row->setNumScienceGoal(numScienceGoal);
			
		row->setScienceGoal(scienceGoal);
			
		row->setNumWeatherConstraint(numWeatherConstraint);
			
		row->setWeatherConstraint(weatherConstraint);
	
		return row;		
	}	
	


SBSummaryRow* SBSummaryTable::newRow(SBSummaryRow* row) {
	return new SBSummaryRow(*this, row);
}

	//
	// Append a row to its table.
	//

	
	 
	
	/** 
 	 * Look up the table for a row whose noautoincrementable attributes are matching their
 	 * homologues in *x.  If a row is found  this row else autoincrement  *x.sBSummaryId, 
 	 * add x to its table and returns x.
 	 *  
 	 * @returns a pointer on a SBSummaryRow.
 	 * @param x. A pointer on the row to be added.
 	 */ 
 		
			
	SBSummaryRow* SBSummaryTable::add(SBSummaryRow* x) {
			 
		SBSummaryRow* aRow = lookup(
				
		x->getSbSummaryUID()
				,
		x->getProjectUID()
				,
		x->getObsUnitSetUID()
				,
		x->getFrequency()
				,
		x->getFrequencyBand()
				,
		x->getSbType()
				,
		x->getSbDuration()
				,
		x->getNumObservingMode()
				,
		x->getObservingMode()
				,
		x->getNumberRepeats()
				,
		x->getNumScienceGoal()
				,
		x->getScienceGoal()
				,
		x->getNumWeatherConstraint()
				,
		x->getWeatherConstraint()
				
		);
		if (aRow) return aRow;
			

			
		// Autoincrement sBSummaryId
		x->setSBSummaryId(Tag(size(), TagType::SBSummary));
						
		row.push_back(x);
		privateRows.push_back(x);
		x->isAdded(true);
		return x;
	}
		
	
		
	void SBSummaryTable::addWithoutCheckingUnique(SBSummaryRow * x) {
		if (getRowByKey(
						x->getSBSummaryId()
						) != (SBSummaryRow *) 0) 
			throw DuplicateKey("Dupicate key exception in ", "SBSummaryTable");
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
	SBSummaryRow*  SBSummaryTable::checkAndAdd(SBSummaryRow* x, bool skipCheckUniqueness)  {
		if (!skipCheckUniqueness) { 
	 
		 
			if (lookup(
			
				x->getSbSummaryUID()
		,
				x->getProjectUID()
		,
				x->getObsUnitSetUID()
		,
				x->getFrequency()
		,
				x->getFrequencyBand()
		,
				x->getSbType()
		,
				x->getSbDuration()
		,
				x->getNumObservingMode()
		,
				x->getObservingMode()
		,
				x->getNumberRepeats()
		,
				x->getNumScienceGoal()
		,
				x->getScienceGoal()
		,
				x->getNumWeatherConstraint()
		,
				x->getWeatherConstraint()
		
			)) throw UniquenessViolationException();
		
		
		}
		
		if (getRowByKey(
	
			x->getSBSummaryId()
			
		)) throw DuplicateKey("Duplicate key exception in ", "SBSummaryTable");
		
		row.push_back(x);
		privateRows.push_back(x);
		x->isAdded(true);
		return x;	
	}	



	//
	// A private method to brutally append a row to its table, without checking for row uniqueness.
	//

	void SBSummaryTable::append(SBSummaryRow *x) {
		privateRows.push_back(x);
		x->isAdded(true);
	}





	 vector<SBSummaryRow *> SBSummaryTable::get() {
	 	checkPresenceInMemory();
	    return privateRows;
	 }
	 
	 const vector<SBSummaryRow *>& SBSummaryTable::get() const {
	 	const_cast<SBSummaryTable&>(*this).checkPresenceInMemory();	
	    return privateRows;
	 }	 
	 	




	

	
/*
 ** Returns a SBSummaryRow* given a key.
 ** @return a pointer to the row having the key whose values are passed as parameters, or 0 if
 ** no row exists for that key.
 **
 */
 	SBSummaryRow* SBSummaryTable::getRowByKey(Tag sBSummaryId)  {
 	checkPresenceInMemory();
	SBSummaryRow* aRow = 0;
	for (unsigned int i = 0; i < privateRows.size(); i++) {
		aRow = row.at(i);
		
			
				if (aRow->sBSummaryId != sBSummaryId) continue;
			
		
		return aRow;
	}
	return 0;		
}
	

	
/**
 * Look up the table for a row whose all attributes  except the autoincrementable one 
 * are equal to the corresponding parameters of the method.
 * @return a pointer on this row if any, 0 otherwise.
 *
			
 * @param sbSummaryUID.
 	 		
 * @param projectUID.
 	 		
 * @param obsUnitSetUID.
 	 		
 * @param frequency.
 	 		
 * @param frequencyBand.
 	 		
 * @param sbType.
 	 		
 * @param sbDuration.
 	 		
 * @param numObservingMode.
 	 		
 * @param observingMode.
 	 		
 * @param numberRepeats.
 	 		
 * @param numScienceGoal.
 	 		
 * @param scienceGoal.
 	 		
 * @param numWeatherConstraint.
 	 		
 * @param weatherConstraint.
 	 		 
 */
SBSummaryRow* SBSummaryTable::lookup(EntityRef sbSummaryUID, EntityRef projectUID, EntityRef obsUnitSetUID, double frequency, ReceiverBandMod::ReceiverBand frequencyBand, SBTypeMod::SBType sbType, Interval sbDuration, int numObservingMode, std::vector<std::string > observingMode, int numberRepeats, int numScienceGoal, std::vector<std::string > scienceGoal, int numWeatherConstraint, std::vector<std::string > weatherConstraint) {
		SBSummaryRow* aRow;
		for (unsigned int i = 0; i < privateRows.size(); i++) {
			aRow = privateRows.at(i); 
			if (aRow->compareNoAutoInc(sbSummaryUID, projectUID, obsUnitSetUID, frequency, frequencyBand, sbType, sbDuration, numObservingMode, observingMode, numberRepeats, numScienceGoal, scienceGoal, numWeatherConstraint, weatherConstraint)) return aRow;
		}			
		return 0;	
} 
	
 	 	

	



#ifndef WITHOUT_ACS
	using asdmIDL::SBSummaryTableIDL;
#endif

#ifndef WITHOUT_ACS
	// Conversion Methods

	SBSummaryTableIDL *SBSummaryTable::toIDL() {
		SBSummaryTableIDL *x = new SBSummaryTableIDL ();
		unsigned int nrow = size();
		x->row.length(nrow);
		vector<SBSummaryRow*> v = get();
		for (unsigned int i = 0; i < nrow; ++i) {
			//x->row[i] = *(v[i]->toIDL());
			v[i]->toIDL(x->row[i]);
		}
		return x;
	}
	
	void SBSummaryTable::toIDL(asdmIDL::SBSummaryTableIDL& x) const {
		unsigned int nrow = size();
		x.row.length(nrow);
		vector<SBSummaryRow*> v = get();
		for (unsigned int i = 0; i < nrow; ++i) {
			v[i]->toIDL(x.row[i]);
		}
	}	
#endif
	
#ifndef WITHOUT_ACS
	void SBSummaryTable::fromIDL(SBSummaryTableIDL x) {
		unsigned int nrow = x.row.length();
		for (unsigned int i = 0; i < nrow; ++i) {
			SBSummaryRow *tmp = newRow();
			tmp->setFromIDL(x.row[i]);
			// checkAndAdd(tmp);
			add(tmp);
		}
	}	
#endif

	
	string SBSummaryTable::toXML()  {
		string buf;

		buf.append("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?> ");
		buf.append("<SBSummaryTable xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:sbsmmr=\"http://Alma/XASDM/SBSummaryTable\" xsi:schemaLocation=\"http://Alma/XASDM/SBSummaryTable http://almaobservatory.org/XML/XASDM/4/SBSummaryTable.xsd\" schemaVersion=\"4\" schemaRevision=\"-1\">\n");
	
		buf.append(entity.toXML());
		string s = container.getEntity().toXML();
		// Change the "Entity" tag to "ContainerEntity".
		buf.append("<Container" + s.substr(1,s.length() - 1)+" ");
		vector<SBSummaryRow*> v = get();
		for (unsigned int i = 0; i < v.size(); ++i) {
			try {
				buf.append(v[i]->toXML());
			} catch (const NoSuchRow &e) {
			}
			buf.append("  ");
		}		
		buf.append("</SBSummaryTable> ");
		return buf;
	}

	
	string SBSummaryTable::getVersion() const {
		return version;
	}
	

	void SBSummaryTable::fromXML(string& tableInXML)  {
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
			throw ConversionException("Failed to parse the xmlHeader into a DOM structure.", "SBSummary");
		
		xmlNode* root_element = xmlDocGetRootElement(doc);
   		if ( root_element == NULL || root_element->type != XML_ELEMENT_NODE )
      		throw ConversionException("Failed to retrieve the root element in the DOM structure.", "SBSummary");
      		
      	xmlChar * propValue = xmlGetProp(root_element, (const xmlChar *) "schemaVersion");
      	if ( propValue != 0 ) {
      		version = string( (const char*) propValue);
      		xmlFree(propValue);   		
      	}
      		     							
		Parser xml(tableInXML);
		if (!xml.isStr("<SBSummaryTable")) 
			error();
		// cout << "Parsing a SBSummaryTable" << endl;
		string s = xml.getElement("<Entity","/>");
		if (s.length() == 0) 
			error();
		Entity e;
		e.setFromXML(s);
		if (e.getEntityTypeName() != "SBSummaryTable")
			error();
		setEntity(e);
		// Skip the container's entity; but, it has to be there.
		s = xml.getElement("<ContainerEntity","/>");
		if (s.length() == 0) 
			error();

		// Get each row in the table.
		s = xml.getElementContent("<row>","</row>");
		SBSummaryRow *row;
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
				throw ConversionException(e1.getMessage(),"SBSummaryTable");
			} 
			catch (const UniquenessViolationException &e1) {
				throw ConversionException(e1.getMessage(),"SBSummaryTable");	
			}
			catch (...) {
				// cout << "Unexpected error in SBSummaryTable::checkAndAdd called from SBSummaryTable::fromXML " << endl;
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
				throw ConversionException(e1.getMessage(),"SBSummaryTable");
			} 
			catch (...) {
				// cout << "Unexpected error in SBSummaryTable::addWithoutCheckingUnique called from SBSummaryTable::fromXML " << endl;
			}
		}				
				
				
		if (!xml.isStr("</SBSummaryTable>")) 
		error();
		
		//Does not change the convention defined in the model.	
		//archiveAsBin = false;
		//fileAsBin = false;

                // clean up the xmlDoc pointer
		if ( doc != NULL ) xmlFreeDoc(doc);
		
	}

	
	void SBSummaryTable::error()  {
		throw ConversionException("Invalid xml document","SBSummary");
	}
	
	
	string SBSummaryTable::MIMEXMLPart(const asdm::ByteOrder* byteOrder) {
		string UID = getEntity().getEntityId().toString();
		string withoutUID = UID.substr(6);
		string containerUID = getContainer().getEntity().getEntityId().toString();
		ostringstream oss;
		oss << "<?xml version='1.0'  encoding='ISO-8859-1'?>";
		oss << "\n";
		oss << "<SBSummaryTable xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:sbsmmr=\"http://Alma/XASDM/SBSummaryTable\" xsi:schemaLocation=\"http://Alma/XASDM/SBSummaryTable http://almaobservatory.org/XML/XASDM/4/SBSummaryTable.xsd\" schemaVersion=\"4\" schemaRevision=\"-1\">\n";
		oss<< "<Entity entityId='"<<UID<<"' entityIdEncrypted='na' entityTypeName='SBSummaryTable' schemaVersion='1' documentVersion='1'/>\n";
		oss<< "<ContainerEntity entityId='"<<containerUID<<"' entityIdEncrypted='na' entityTypeName='ASDM' schemaVersion='1' documentVersion='1'/>\n";
		oss << "<BulkStoreRef file_id='"<<withoutUID<<"' byteOrder='"<<byteOrder->toString()<<"' />\n";
		oss << "<Attributes>\n";

		oss << "<sBSummaryId/>\n"; 
		oss << "<sbSummaryUID/>\n"; 
		oss << "<projectUID/>\n"; 
		oss << "<obsUnitSetUID/>\n"; 
		oss << "<frequency/>\n"; 
		oss << "<frequencyBand/>\n"; 
		oss << "<sbType/>\n"; 
		oss << "<sbDuration/>\n"; 
		oss << "<numObservingMode/>\n"; 
		oss << "<observingMode/>\n"; 
		oss << "<numberRepeats/>\n"; 
		oss << "<numScienceGoal/>\n"; 
		oss << "<scienceGoal/>\n"; 
		oss << "<numWeatherConstraint/>\n"; 
		oss << "<weatherConstraint/>\n"; 

		oss << "<centerDirection/>\n"; 
		oss << "<centerDirectionCode/>\n"; 
		oss << "<centerDirectionEquinox/>\n"; 
		oss << "</Attributes>\n";		
		oss << "</SBSummaryTable>\n";

		return oss.str();				
	}
	
	string SBSummaryTable::toMIME(const asdm::ByteOrder* byteOrder) {
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

	
	void SBSummaryTable::setFromMIME(const string & mimeMsg) {
    string xmlPartMIMEHeader = "Content-ID: <header.xml>\n\n";
    
    string binPartMIMEHeader = "--MIME_boundary\nContent-Type: binary/octet-stream\nContent-ID: <content.bin>\n\n";
    
    // Detect the XML header.
    string::size_type loc0 = mimeMsg.find(xmlPartMIMEHeader, 0);
    if ( loc0 == string::npos) {
      // let's try with CRLFs
      xmlPartMIMEHeader = "Content-ID: <header.xml>\r\n\r\n";
      loc0 = mimeMsg.find(xmlPartMIMEHeader, 0);
      if  ( loc0 == string::npos ) 
	      throw ConversionException("Failed to detect the beginning of the XML header", "SBSummary");
    }

    loc0 += xmlPartMIMEHeader.size();
    
    // Look for the string announcing the binary part.
    string::size_type loc1 = mimeMsg.find( binPartMIMEHeader, loc0 );
    
    if ( loc1 == string::npos ) {
      throw ConversionException("Failed to detect the beginning of the binary part", "SBSummary");
    }
    
    //
    // Extract the xmlHeader and analyze it to find out what is the byte order and the sequence
    // of attribute names.
    //
    string xmlHeader = mimeMsg.substr(loc0, loc1-loc0);
    xmlDoc *doc;
    doc = xmlReadMemory(xmlHeader.data(), xmlHeader.size(), "BinaryTableHeader.xml", NULL, XML_PARSE_NOBLANKS);
    if ( doc == NULL ) 
      throw ConversionException("Failed to parse the xmlHeader into a DOM structure.", "SBSummary");
    
   // This vector will be filled by the names of  all the attributes of the table
   // in the order in which they are expected to be found in the binary representation.
   //
    vector<string> attributesSeq;
      
    xmlNode* root_element = xmlDocGetRootElement(doc);
    if ( root_element == NULL || root_element->type != XML_ELEMENT_NODE )
      throw ConversionException("Failed to parse the xmlHeader into a DOM structure.", "SBSummary");
    
    const ByteOrder* byteOrder=0;
    if ( string("ASDMBinaryTable").compare((const char*) root_element->name) == 0) {
      // Then it's an "old fashioned" MIME file for tables.
      // Just try to deserialize it with Big_Endian for the bytes ordering.
      byteOrder = asdm::ByteOrder::Big_Endian;
      
 	 //
    // Let's consider a  default order for the sequence of attributes.
    //
    
    	 
    attributesSeq.push_back("sBSummaryId") ; 
    	 
    attributesSeq.push_back("sbSummaryUID") ; 
    	 
    attributesSeq.push_back("projectUID") ; 
    	 
    attributesSeq.push_back("obsUnitSetUID") ; 
    	 
    attributesSeq.push_back("frequency") ; 
    	 
    attributesSeq.push_back("frequencyBand") ; 
    	 
    attributesSeq.push_back("sbType") ; 
    	 
    attributesSeq.push_back("sbDuration") ; 
    	 
    attributesSeq.push_back("numObservingMode") ; 
    	 
    attributesSeq.push_back("observingMode") ; 
    	 
    attributesSeq.push_back("numberRepeats") ; 
    	 
    attributesSeq.push_back("numScienceGoal") ; 
    	 
    attributesSeq.push_back("scienceGoal") ; 
    	 
    attributesSeq.push_back("numWeatherConstraint") ; 
    	 
    attributesSeq.push_back("weatherConstraint") ; 
    	
    	 
    attributesSeq.push_back("centerDirection") ; 
    	 
    attributesSeq.push_back("centerDirectionCode") ; 
    	 
    attributesSeq.push_back("centerDirectionEquinox") ; 
    	
     
    
    
    // And decide that it has version == "2"
    version = "2";         
     }
    else if (string("SBSummaryTable").compare((const char*) root_element->name) == 0) {
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
      	throw ConversionException ("Could not find the element '/SBSummaryTable/BulkStoreRef'. Invalid XML header '"+ xmlHeader + "'.", "SBSummary");
      	
      // We found BulkStoreRef, now look for its attribute byteOrder.
      _xmlAttr* byteOrderAttr = 0;
      for (struct _xmlAttr* attr = bulkStoreRef->properties; attr; attr = attr->next) 
	  if (string("byteOrder").compare((const char*) attr->name) == 0) {
	   byteOrderAttr = attr;
	   break;
	 }
      
      if (byteOrderAttr == 0) 
	     throw ConversionException("Could not find the element '/SBSummaryTable/BulkStoreRef/@byteOrder'. Invalid XML header '" + xmlHeader +"'.", "SBSummary");
      
      string byteOrderValue = string((const char*) byteOrderAttr->children->content);
      if (!(byteOrder = asdm::ByteOrder::fromString(byteOrderValue)))
		throw ConversionException("No valid value retrieved for the element '/SBSummaryTable/BulkStoreRef/@byteOrder'. Invalid XML header '" + xmlHeader + "'.", "SBSummary");
		
	 //
	 // 2nd) Look for the Attributes element and grab the names of the elements it contains.
	 //
	 xmlNode* attributes = bulkStoreRef->next;
     if ( attributes == 0 || (attributes->type != XML_ELEMENT_NODE)  || (string("Attributes").compare((const char*) attributes->name) != 0))	 
       	throw ConversionException ("Could not find the element '/SBSummaryTable/Attributes'. Invalid XML header '"+ xmlHeader + "'.", "SBSummary");
 
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
				SBSummaryRow* aRow = SBSummaryRow::fromBin((EndianIStream&) eiss, *this, attributesSeq);
				checkAndAdd(aRow);
      		}
    	}
    	catch (const DuplicateKey &e) {
      		throw ConversionException("Error while writing binary data , the message was "
				+ e.getMessage(), "SBSummary");
    	}
    	catch (const TagFormatException &e) {
     		 throw ConversionException("Error while reading binary data , the message was "
				+ e.getMessage(), "SBSummary");
    	}
    }
    else {
 		for (uint32_t i = 0; i < this->declaredSize; i++) {
			SBSummaryRow* aRow = SBSummaryRow::fromBin((EndianIStream&) eiss, *this, attributesSeq);
			append(aRow);
      	}   	
    }
    //Does not change the convention defined in the model.	
    //archiveAsBin = true;
    //fileAsBin = true;
    if ( doc != NULL ) xmlFreeDoc(doc);

	}
	
	void SBSummaryTable::setUnknownAttributeBinaryReader(const string& attributeName, BinaryAttributeReaderFunctor* barFctr) {
		//
		// Is this attribute really unknown ?
		//
		for (vector<string>::const_iterator iter = attributesNamesOfSBSummary_v.begin(); iter != attributesNamesOfSBSummary_v.end(); iter++) {
			if ((*iter).compare(attributeName) == 0) 
				throw ConversionException("the attribute '"+attributeName+"' is known you can't override the way it's read in the MIME binary file containing the table.", "SBSummary"); 
		}
		
		// Ok then register the functor to activate when an unknown attribute is met during the reading of a binary table?
		unknownAttributes2Functors[attributeName] = barFctr;
	}
	
	BinaryAttributeReaderFunctor* SBSummaryTable::getUnknownAttributeBinaryReader(const string& attributeName) const {
		map<string, BinaryAttributeReaderFunctor*>::const_iterator iter = unknownAttributes2Functors.find(attributeName);
		return (iter == unknownAttributes2Functors.end()) ? 0 : iter->second;
	}

	
	void SBSummaryTable::toFile(string directory) {
		if (!directoryExists(directory.c_str()) &&
			!createPath(directory.c_str())) {
			throw ConversionException("Could not create directory " , directory);
		}

		string fileName = directory + "/SBSummary.xml";
		ofstream tableout(fileName.c_str(),ios::out|ios::trunc);
		if (tableout.rdstate() == ostream::failbit)
			throw ConversionException("Could not open file " + fileName + " to write ", "SBSummary");
		if (fileAsBin) 
			tableout << MIMEXMLPart();
		else
			tableout << toXML() << endl;
		tableout.close();
		if (tableout.rdstate() == ostream::failbit)
			throw ConversionException("Could not close file " + fileName, "SBSummary");

		if (fileAsBin) {
			// write the bin serialized
			string fileName = directory + "/SBSummary.bin";
			ofstream tableout(fileName.c_str(),ios::out|ios::trunc);
			if (tableout.rdstate() == ostream::failbit)
				throw ConversionException("Could not open file " + fileName + " to write ", "SBSummary");
			tableout << toMIME() << endl;
			tableout.close();
			if (tableout.rdstate() == ostream::failbit)
				throw ConversionException("Could not close file " + fileName, "SBSummary");
		}
	}

	
	void SBSummaryTable::setFromFile(const string& directory) {
#ifndef WITHOUT_BOOST
    if (boost::filesystem::exists(boost::filesystem::path(uniqSlashes(directory + "/SBSummary.xml"))))
      setFromXMLFile(directory);
    else if (boost::filesystem::exists(boost::filesystem::path(uniqSlashes(directory + "/SBSummary.bin"))))
      setFromMIMEFile(directory);
#else 
    // alternative in Misc.h
    if (file_exists(uniqSlashes(directory + "/SBSummary.xml")))
      setFromXMLFile(directory);
    else if (file_exists(uniqSlashes(directory + "/SBSummary.bin")))
      setFromMIMEFile(directory);
#endif
    else
      throw ConversionException("No file found for the SBSummary table", "SBSummary");
	}			

	
  void SBSummaryTable::setFromMIMEFile(const string& directory) {
    string tablePath ;
    
    tablePath = directory + "/SBSummary.bin";
    ifstream tablefile(tablePath.c_str(), ios::in|ios::binary);
    if (!tablefile.is_open()) { 
      throw ConversionException("Could not open file " + tablePath, "SBSummary");
    }
    // Read in a stringstream.
    stringstream ss; ss << tablefile.rdbuf();
    
    if (tablefile.rdstate() == istream::failbit || tablefile.rdstate() == istream::badbit) {
      throw ConversionException("Error reading file " + tablePath,"SBSummary");
    }
    
    // And close.
    tablefile.close();
    if (tablefile.rdstate() == istream::failbit)
      throw ConversionException("Could not close file " + tablePath,"SBSummary");
    
    setFromMIME(ss.str());
  }	
/* 
  void SBSummaryTable::openMIMEFile (const string& directory) {
  		
  	// Open the file.
  	string tablePath ;
    tablePath = directory + "/SBSummary.bin";
    ifstream tablefile(tablePath.c_str(), ios::in|ios::binary);
    if (!tablefile.is_open())
      throw ConversionException("Could not open file " + tablePath, "SBSummary");
      
	// Locate the xmlPartMIMEHeader.
    string xmlPartMIMEHeader = "CONTENT-ID: <HEADER.XML>\n\n";
    CharComparator comparator;
    istreambuf_iterator<char> BEGIN(tablefile.rdbuf());
    istreambuf_iterator<char> END;
    istreambuf_iterator<char> it = search(BEGIN, END, xmlPartMIMEHeader.begin(), xmlPartMIMEHeader.end(), comparator);
    if (it == END) 
    	throw ConversionException("failed to detect the beginning of the XML header", "SBSummary");
    
    // Locate the binaryPartMIMEHeader while accumulating the characters of the xml header.	
    string binPartMIMEHeader = "--MIME_BOUNDARY\nCONTENT-TYPE: BINARY/OCTET-STREAM\nCONTENT-ID: <CONTENT.BIN>\n\n";
    string xmlHeader;
   	CharCompAccumulator compaccumulator(&xmlHeader, 100000);
   	++it;
   	it = search(it, END, binPartMIMEHeader.begin(), binPartMIMEHeader.end(), compaccumulator);
   	if (it == END) 
   		throw ConversionException("failed to detect the beginning of the binary part", "SBSummary");
   	
	cout << xmlHeader << endl;
	//
	// We have the xmlHeader , let's parse it.
	//
	xmlDoc *doc;
    doc = xmlReadMemory(xmlHeader.data(), xmlHeader.size(), "BinaryTableHeader.xml", NULL, XML_PARSE_NOBLANKS);
    if ( doc == NULL ) 
      throw ConversionException("Failed to parse the xmlHeader into a DOM structure.", "SBSummary");
    
   // This vector will be filled by the names of  all the attributes of the table
   // in the order in which they are expected to be found in the binary representation.
   //
    vector<string> attributesSeq(attributesNamesInBinOfSBSummary_v);
      
    xmlNode* root_element = xmlDocGetRootElement(doc);
    if ( root_element == NULL || root_element->type != XML_ELEMENT_NODE )
      throw ConversionException("Failed to parse the xmlHeader into a DOM structure.", "SBSummary");
    
    const ByteOrder* byteOrder=0;
    if ( string("ASDMBinaryTable").compare((const char*) root_element->name) == 0) {
      // Then it's an "old fashioned" MIME file for tables.
      // Just try to deserialize it with Big_Endian for the bytes ordering.
      byteOrder = asdm::ByteOrder::Big_Endian;
        
      // And decide that it has version == "2"
    version = "2";         
     }
    else if (string("SBSummaryTable").compare((const char*) root_element->name) == 0) {
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
      	throw ConversionException ("Could not find the element '/SBSummaryTable/BulkStoreRef'. Invalid XML header '"+ xmlHeader + "'.", "SBSummary");
      	
      // We found BulkStoreRef, now look for its attribute byteOrder.
      _xmlAttr* byteOrderAttr = 0;
      for (struct _xmlAttr* attr = bulkStoreRef->properties; attr; attr = attr->next) 
	  if (string("byteOrder").compare((const char*) attr->name) == 0) {
	   byteOrderAttr = attr;
	   break;
	 }
      
      if (byteOrderAttr == 0) 
	     throw ConversionException("Could not find the element '/SBSummaryTable/BulkStoreRef/@byteOrder'. Invalid XML header '" + xmlHeader +"'.", "SBSummary");
      
      string byteOrderValue = string((const char*) byteOrderAttr->children->content);
      if (!(byteOrder = asdm::ByteOrder::fromString(byteOrderValue)))
		throw ConversionException("No valid value retrieved for the element '/SBSummaryTable/BulkStoreRef/@byteOrder'. Invalid XML header '" + xmlHeader + "'.", "SBSummary");
		
	 //
	 // 2nd) Look for the Attributes element and grab the names of the elements it contains.
	 //
	 xmlNode* attributes = bulkStoreRef->next;
     if ( attributes == 0 || (attributes->type != XML_ELEMENT_NODE)  || (string("Attributes").compare((const char*) attributes->name) != 0))	 
       	throw ConversionException ("Could not find the element '/SBSummaryTable/Attributes'. Invalid XML header '"+ xmlHeader + "'.", "SBSummary");
 
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

	
void SBSummaryTable::setFromXMLFile(const string& directory) {
    string tablePath ;
    
    tablePath = directory + "/SBSummary.xml";
    
    /*
    ifstream tablefile(tablePath.c_str(), ios::in|ios::binary);
    if (!tablefile.is_open()) { 
      throw ConversionException("Could not open file " + tablePath, "SBSummary");
    }
      // Read in a stringstream.
    stringstream ss;
    ss << tablefile.rdbuf();
    
    if  (tablefile.rdstate() == istream::failbit || tablefile.rdstate() == istream::badbit) {
      throw ConversionException("Error reading file '" + tablePath + "'", "SBSummary");
    }
    
    // And close
    tablefile.close();
    if (tablefile.rdstate() == istream::failbit)
      throw ConversionException("Could not close file '" + tablePath + "'", "SBSummary");

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
    	throw ConversionException("Caugth an exception whose message is '" + e.getMessage() + "'.", "SBSummary");
    }
    
    if (xmlDocument.find("<BulkStoreRef") != string::npos)
      setFromMIMEFile(directory);
    else
      fromXML(xmlDocument);
  }

	

	

			
	
	

	
	void SBSummaryTable::autoIncrement(string key, SBSummaryRow* x) {
		map<string, int>::iterator iter;
		if ((iter=noAutoIncIds.find(key)) == noAutoIncIds.end()) {
			// There is not yet a combination of the non autoinc attributes values in the hashtable
			
			// Initialize  sBSummaryId to Tag(0).
			x->setSBSummaryId(Tag(0,  TagType::SBSummary));
			
			// Record it in the map.		
			noAutoIncIds.insert(make_pair(key, 0));			
		} 
		else {
			// There is already a combination of the non autoinc attributes values in the hashtable
			// Increment its value.
			int n = iter->second + 1; 
			
			// Initialize  sBSummaryId to Tag(n).
			x->setSBSummaryId(Tag(n, TagType::SBSummary));
			
			// Record it in the map.		
			noAutoIncIds.insert(make_pair(key, n));				
		}		
	}
	
} // End namespace asdm
 
