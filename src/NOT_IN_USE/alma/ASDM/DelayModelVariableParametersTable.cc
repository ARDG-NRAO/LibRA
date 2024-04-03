
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
 * File DelayModelVariableParametersTable.cpp
 */
#include <alma/ASDM/ConversionException.h>
#include <alma/ASDM/DuplicateKey.h>
#include <alma/ASDM/OutOfBoundsException.h>

using asdm::ConversionException;
using asdm::DuplicateKey;
using asdm::OutOfBoundsException;

#include <alma/ASDM/ASDM.h>
#include <alma/ASDM/DelayModelVariableParametersTable.h>
#include <alma/ASDM/DelayModelVariableParametersRow.h>
#include <alma/ASDM/Parser.h>

using asdm::ASDM;
using asdm::DelayModelVariableParametersTable;
using asdm::DelayModelVariableParametersRow;
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
	static string entityNameOfDelayModelVariableParameters = "DelayModelVariableParameters";
	
	// An array of string containing the names of the columns of this table.
	// The array is filled in the order : key, required value, optional value.
	//
	static string attributesNamesOfDelayModelVariableParameters_a[] = {
		
			"delayModelVariableParametersId"
		
		
			, "time"
		
			, "ut1_utc"
		
			, "iat_utc"
		
			, "timeType"
		
			, "gstAtUt0"
		
			, "earthRotationRate"
		
			, "polarOffsets"
		
			, "polarOffsetsType"
		
			, "delayModelFixedParametersId"
				
		
			, "nutationInLongitude"
		
			, "nutationInLongitudeRate"
		
			, "nutationInObliquity"
		
			, "nutationInObliquityRate"
				
	};
	
	// A vector of string whose content is a copy of the strings in the array above.
	//
	static vector<string> attributesNamesOfDelayModelVariableParameters_v (attributesNamesOfDelayModelVariableParameters_a, attributesNamesOfDelayModelVariableParameters_a + sizeof(attributesNamesOfDelayModelVariableParameters_a) / sizeof(attributesNamesOfDelayModelVariableParameters_a[0]));

	// An array of string containing the names of the columns of this table.
	// The array is filled in the order where the names would be read by default in the XML header of a file containing
	// the table exported in binary mode.
	//	
	static string attributesNamesInBinOfDelayModelVariableParameters_a[] = {
    
    	 "delayModelVariableParametersId" , "time" , "ut1_utc" , "iat_utc" , "timeType" , "gstAtUt0" , "earthRotationRate" , "polarOffsets" , "polarOffsetsType" , "delayModelFixedParametersId" 
    	,
    	 "nutationInLongitude" , "nutationInLongitudeRate" , "nutationInObliquity" , "nutationInObliquityRate" 
    
	};
	        			
	// A vector of string whose content is a copy of the strings in the array above.
	//
	static vector<string> attributesNamesInBinOfDelayModelVariableParameters_v(attributesNamesInBinOfDelayModelVariableParameters_a, attributesNamesInBinOfDelayModelVariableParameters_a + sizeof(attributesNamesInBinOfDelayModelVariableParameters_a) / sizeof(attributesNamesInBinOfDelayModelVariableParameters_a[0]));		
	

	// The array of attributes (or column) names that make up key key.
	//
	string keyOfDelayModelVariableParameters_a[] = {
	
		"delayModelVariableParametersId"
		 
	};
	 
	// A vector of strings which are copies of those stored in the array above.
	vector<string> keyOfDelayModelVariableParameters_v(keyOfDelayModelVariableParameters_a, keyOfDelayModelVariableParameters_a + sizeof(keyOfDelayModelVariableParameters_a) / sizeof(keyOfDelayModelVariableParameters_a[0]));

	/**
	 * Return the list of field names that make up key key
	 * as a const reference to a vector of strings.
	 */	
	const vector<string>& DelayModelVariableParametersTable::getKeyName() {
		return keyOfDelayModelVariableParameters_v;
	}


	DelayModelVariableParametersTable::DelayModelVariableParametersTable(ASDM &c) : container(c) {

		// Define a default entity.
		entity.setEntityId(EntityId("uid://X0/X0/X0"));
		entity.setEntityIdEncrypted("na");
		entity.setEntityTypeName("DelayModelVariableParametersTable");
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
 * A destructor for DelayModelVariableParametersTable.
 */
	DelayModelVariableParametersTable::~DelayModelVariableParametersTable() {
		for (unsigned int i = 0; i < privateRows.size(); i++) 
			delete(privateRows.at(i));
	}

	/**
	 * Container to which this table belongs.
	 */
	ASDM &DelayModelVariableParametersTable::getContainer() const {
		return container;
	}

	/**
	 * Return the number of rows in the table.
	 */
	unsigned int DelayModelVariableParametersTable::size() const {
		if (presentInMemory) 
			return privateRows.size();
		else
			return declaredSize;
	}
	
	/**
	 * Return the name of this table.
	 */
	string DelayModelVariableParametersTable::getName() const {
		return entityNameOfDelayModelVariableParameters;
	}
	
	/**
	 * Return the name of this table.
	 */
	string DelayModelVariableParametersTable::name() {
		return entityNameOfDelayModelVariableParameters;
	}
	
	/**
	 * Return the the names of the attributes (or columns) of this table.
	 */
	const vector<string>& DelayModelVariableParametersTable::getAttributesNames() { return attributesNamesOfDelayModelVariableParameters_v; }
	
	/**
	 * Return the the names of the attributes (or columns) of this table as they appear by default
	 * in an binary export of this table.
	 */
	const vector<string>& DelayModelVariableParametersTable::defaultAttributesNamesInBin() { return attributesNamesInBinOfDelayModelVariableParameters_v; }

	/**
	 * Return this table's Entity.
	 */
	Entity DelayModelVariableParametersTable::getEntity() const {
		return entity;
	}

	/**
	 * Set this table's Entity.
	 */
	void DelayModelVariableParametersTable::setEntity(Entity e) {
		this->entity = e; 
	}
	
	//
	// ====> Row creation.
	//
	
	/**
	 * Create a new row.
	 */
	DelayModelVariableParametersRow *DelayModelVariableParametersTable::newRow() {
		return new DelayModelVariableParametersRow (*this);
	}
	

	/**
	 * Create a new row initialized to the specified values.
	 * @return a pointer on the created and initialized row.
	
 	 * @param time 
	
 	 * @param ut1_utc 
	
 	 * @param iat_utc 
	
 	 * @param timeType 
	
 	 * @param gstAtUt0 
	
 	 * @param earthRotationRate 
	
 	 * @param polarOffsets 
	
 	 * @param polarOffsetsType 
	
 	 * @param delayModelFixedParametersId 
	
     */
	DelayModelVariableParametersRow* DelayModelVariableParametersTable::newRow(ArrayTime time, double ut1_utc, double iat_utc, DifferenceTypeMod::DifferenceType timeType, Angle gstAtUt0, AngularRate earthRotationRate, std::vector<double > polarOffsets, DifferenceTypeMod::DifferenceType polarOffsetsType, Tag delayModelFixedParametersId){
		DelayModelVariableParametersRow *row = new DelayModelVariableParametersRow(*this);
			
		row->setTime(time);
			
		row->setUt1_utc(ut1_utc);
			
		row->setIat_utc(iat_utc);
			
		row->setTimeType(timeType);
			
		row->setGstAtUt0(gstAtUt0);
			
		row->setEarthRotationRate(earthRotationRate);
			
		row->setPolarOffsets(polarOffsets);
			
		row->setPolarOffsetsType(polarOffsetsType);
			
		row->setDelayModelFixedParametersId(delayModelFixedParametersId);
	
		return row;		
	}	
	


DelayModelVariableParametersRow* DelayModelVariableParametersTable::newRow(DelayModelVariableParametersRow* row) {
	return new DelayModelVariableParametersRow(*this, row);
}

	//
	// Append a row to its table.
	//

	
	 
	
	/** 
 	 * Look up the table for a row whose noautoincrementable attributes are matching their
 	 * homologues in *x.  If a row is found  this row else autoincrement  *x.delayModelVariableParametersId, 
 	 * add x to its table and returns x.
 	 *  
 	 * @returns a pointer on a DelayModelVariableParametersRow.
 	 * @param x. A pointer on the row to be added.
 	 */ 
 		
			
	DelayModelVariableParametersRow* DelayModelVariableParametersTable::add(DelayModelVariableParametersRow* x) {
			 
		DelayModelVariableParametersRow* aRow = lookup(
				
		x->getTime()
				,
		x->getUt1_utc()
				,
		x->getIat_utc()
				,
		x->getTimeType()
				,
		x->getGstAtUt0()
				,
		x->getEarthRotationRate()
				,
		x->getPolarOffsets()
				,
		x->getPolarOffsetsType()
				,
		x->getDelayModelFixedParametersId()
				
		);
		if (aRow) return aRow;
			

			
		// Autoincrement delayModelVariableParametersId
		x->setDelayModelVariableParametersId(Tag(size(), TagType::DelayModelVariableParameters));
						
		row.push_back(x);
		privateRows.push_back(x);
		x->isAdded(true);
		return x;
	}
		
	
		
	void DelayModelVariableParametersTable::addWithoutCheckingUnique(DelayModelVariableParametersRow * x) {
		if (getRowByKey(
						x->getDelayModelVariableParametersId()
						) != (DelayModelVariableParametersRow *) 0) 
			throw DuplicateKey("Dupicate key exception in ", "DelayModelVariableParametersTable");
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
	DelayModelVariableParametersRow*  DelayModelVariableParametersTable::checkAndAdd(DelayModelVariableParametersRow* x, bool skipCheckUniqueness)  {
		if (!skipCheckUniqueness) { 
	 
		 
			if (lookup(
			
				x->getTime()
		,
				x->getUt1_utc()
		,
				x->getIat_utc()
		,
				x->getTimeType()
		,
				x->getGstAtUt0()
		,
				x->getEarthRotationRate()
		,
				x->getPolarOffsets()
		,
				x->getPolarOffsetsType()
		,
				x->getDelayModelFixedParametersId()
		
			)) throw UniquenessViolationException();
		
		
		}
		
		if (getRowByKey(
	
			x->getDelayModelVariableParametersId()
			
		)) throw DuplicateKey("Duplicate key exception in ", "DelayModelVariableParametersTable");
		
		row.push_back(x);
		privateRows.push_back(x);
		x->isAdded(true);
		return x;	
	}	



	//
	// A private method to brutally append a row to its table, without checking for row uniqueness.
	//

	void DelayModelVariableParametersTable::append(DelayModelVariableParametersRow *x) {
		privateRows.push_back(x);
		x->isAdded(true);
	}





	 vector<DelayModelVariableParametersRow *> DelayModelVariableParametersTable::get() {
	 	checkPresenceInMemory();
	    return privateRows;
	 }
	 
	 const vector<DelayModelVariableParametersRow *>& DelayModelVariableParametersTable::get() const {
	 	const_cast<DelayModelVariableParametersTable&>(*this).checkPresenceInMemory();	
	    return privateRows;
	 }	 
	 	




	

	
/*
 ** Returns a DelayModelVariableParametersRow* given a key.
 ** @return a pointer to the row having the key whose values are passed as parameters, or 0 if
 ** no row exists for that key.
 **
 */
 	DelayModelVariableParametersRow* DelayModelVariableParametersTable::getRowByKey(Tag delayModelVariableParametersId)  {
 	checkPresenceInMemory();
	DelayModelVariableParametersRow* aRow = 0;
	for (unsigned int i = 0; i < privateRows.size(); i++) {
		aRow = row.at(i);
		
			
				if (aRow->delayModelVariableParametersId != delayModelVariableParametersId) continue;
			
		
		return aRow;
	}
	return 0;		
}
	

	
/**
 * Look up the table for a row whose all attributes  except the autoincrementable one 
 * are equal to the corresponding parameters of the method.
 * @return a pointer on this row if any, 0 otherwise.
 *
			
 * @param time.
 	 		
 * @param ut1_utc.
 	 		
 * @param iat_utc.
 	 		
 * @param timeType.
 	 		
 * @param gstAtUt0.
 	 		
 * @param earthRotationRate.
 	 		
 * @param polarOffsets.
 	 		
 * @param polarOffsetsType.
 	 		
 * @param delayModelFixedParametersId.
 	 		 
 */
DelayModelVariableParametersRow* DelayModelVariableParametersTable::lookup(ArrayTime time, double ut1_utc, double iat_utc, DifferenceTypeMod::DifferenceType timeType, Angle gstAtUt0, AngularRate earthRotationRate, std::vector<double > polarOffsets, DifferenceTypeMod::DifferenceType polarOffsetsType, Tag delayModelFixedParametersId) {
		DelayModelVariableParametersRow* aRow;
		for (unsigned int i = 0; i < privateRows.size(); i++) {
			aRow = privateRows.at(i); 
			if (aRow->compareNoAutoInc(time, ut1_utc, iat_utc, timeType, gstAtUt0, earthRotationRate, polarOffsets, polarOffsetsType, delayModelFixedParametersId)) return aRow;
		}			
		return 0;	
} 
	
 	 	

	



#ifndef WITHOUT_ACS
	using asdmIDL::DelayModelVariableParametersTableIDL;
#endif

#ifndef WITHOUT_ACS
	// Conversion Methods

	DelayModelVariableParametersTableIDL *DelayModelVariableParametersTable::toIDL() {
		DelayModelVariableParametersTableIDL *x = new DelayModelVariableParametersTableIDL ();
		unsigned int nrow = size();
		x->row.length(nrow);
		vector<DelayModelVariableParametersRow*> v = get();
		for (unsigned int i = 0; i < nrow; ++i) {
			//x->row[i] = *(v[i]->toIDL());
			v[i]->toIDL(x->row[i]);
		}
		return x;
	}
	
	void DelayModelVariableParametersTable::toIDL(asdmIDL::DelayModelVariableParametersTableIDL& x) const {
		unsigned int nrow = size();
		x.row.length(nrow);
		vector<DelayModelVariableParametersRow*> v = get();
		for (unsigned int i = 0; i < nrow; ++i) {
			v[i]->toIDL(x.row[i]);
		}
	}	
#endif
	
#ifndef WITHOUT_ACS
	void DelayModelVariableParametersTable::fromIDL(DelayModelVariableParametersTableIDL x) {
		unsigned int nrow = x.row.length();
		for (unsigned int i = 0; i < nrow; ++i) {
			DelayModelVariableParametersRow *tmp = newRow();
			tmp->setFromIDL(x.row[i]);
			// checkAndAdd(tmp);
			add(tmp);
		}
	}	
#endif

	
	string DelayModelVariableParametersTable::toXML()  {
		string buf;

		buf.append("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?> ");
		buf.append("<DelayModelVariableParametersTable xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:dmvp=\"http://Alma/XASDM/DelayModelVariableParametersTable\" xsi:schemaLocation=\"http://Alma/XASDM/DelayModelVariableParametersTable http://almaobservatory.org/XML/XASDM/4/DelayModelVariableParametersTable.xsd\" schemaVersion=\"4\" schemaRevision=\"-1\">\n");
	
		buf.append(entity.toXML());
		string s = container.getEntity().toXML();
		// Change the "Entity" tag to "ContainerEntity".
		buf.append("<Container" + s.substr(1,s.length() - 1)+" ");
		vector<DelayModelVariableParametersRow*> v = get();
		for (unsigned int i = 0; i < v.size(); ++i) {
			try {
				buf.append(v[i]->toXML());
			} catch (const NoSuchRow &e) {
			}
			buf.append("  ");
		}		
		buf.append("</DelayModelVariableParametersTable> ");
		return buf;
	}

	
	string DelayModelVariableParametersTable::getVersion() const {
		return version;
	}
	

	void DelayModelVariableParametersTable::fromXML(string& tableInXML)  {
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
			throw ConversionException("Failed to parse the xmlHeader into a DOM structure.", "DelayModelVariableParameters");
		
		xmlNode* root_element = xmlDocGetRootElement(doc);
   		if ( root_element == NULL || root_element->type != XML_ELEMENT_NODE )
      		throw ConversionException("Failed to retrieve the root element in the DOM structure.", "DelayModelVariableParameters");
      		
      	xmlChar * propValue = xmlGetProp(root_element, (const xmlChar *) "schemaVersion");
      	if ( propValue != 0 ) {
      		version = string( (const char*) propValue);
      		xmlFree(propValue);   		
      	}
      		     							
		Parser xml(tableInXML);
		if (!xml.isStr("<DelayModelVariableParametersTable")) 
			error();
		// cout << "Parsing a DelayModelVariableParametersTable" << endl;
		string s = xml.getElement("<Entity","/>");
		if (s.length() == 0) 
			error();
		Entity e;
		e.setFromXML(s);
		if (e.getEntityTypeName() != "DelayModelVariableParametersTable")
			error();
		setEntity(e);
		// Skip the container's entity; but, it has to be there.
		s = xml.getElement("<ContainerEntity","/>");
		if (s.length() == 0) 
			error();

		// Get each row in the table.
		s = xml.getElementContent("<row>","</row>");
		DelayModelVariableParametersRow *row;
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
				throw ConversionException(e1.getMessage(),"DelayModelVariableParametersTable");
			} 
			catch (const UniquenessViolationException &e1) {
				throw ConversionException(e1.getMessage(),"DelayModelVariableParametersTable");	
			}
			catch (...) {
				// cout << "Unexpected error in DelayModelVariableParametersTable::checkAndAdd called from DelayModelVariableParametersTable::fromXML " << endl;
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
				throw ConversionException(e1.getMessage(),"DelayModelVariableParametersTable");
			} 
			catch (...) {
				// cout << "Unexpected error in DelayModelVariableParametersTable::addWithoutCheckingUnique called from DelayModelVariableParametersTable::fromXML " << endl;
			}
		}				
				
				
		if (!xml.isStr("</DelayModelVariableParametersTable>")) 
		error();
		
		//Does not change the convention defined in the model.	
		//archiveAsBin = false;
		//fileAsBin = false;

                // clean up the xmlDoc pointer
		if ( doc != NULL ) xmlFreeDoc(doc);
		
	}

	
	void DelayModelVariableParametersTable::error()  {
		throw ConversionException("Invalid xml document","DelayModelVariableParameters");
	}
	
	
	string DelayModelVariableParametersTable::MIMEXMLPart(const asdm::ByteOrder* byteOrder) {
		string UID = getEntity().getEntityId().toString();
		string withoutUID = UID.substr(6);
		string containerUID = getContainer().getEntity().getEntityId().toString();
		ostringstream oss;
		oss << "<?xml version='1.0'  encoding='ISO-8859-1'?>";
		oss << "\n";
		oss << "<DelayModelVariableParametersTable xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:dmvp=\"http://Alma/XASDM/DelayModelVariableParametersTable\" xsi:schemaLocation=\"http://Alma/XASDM/DelayModelVariableParametersTable http://almaobservatory.org/XML/XASDM/4/DelayModelVariableParametersTable.xsd\" schemaVersion=\"4\" schemaRevision=\"-1\">\n";
		oss<< "<Entity entityId='"<<UID<<"' entityIdEncrypted='na' entityTypeName='DelayModelVariableParametersTable' schemaVersion='1' documentVersion='1'/>\n";
		oss<< "<ContainerEntity entityId='"<<containerUID<<"' entityIdEncrypted='na' entityTypeName='ASDM' schemaVersion='1' documentVersion='1'/>\n";
		oss << "<BulkStoreRef file_id='"<<withoutUID<<"' byteOrder='"<<byteOrder->toString()<<"' />\n";
		oss << "<Attributes>\n";

		oss << "<delayModelVariableParametersId/>\n"; 
		oss << "<time/>\n"; 
		oss << "<ut1_utc/>\n"; 
		oss << "<iat_utc/>\n"; 
		oss << "<timeType/>\n"; 
		oss << "<gstAtUt0/>\n"; 
		oss << "<earthRotationRate/>\n"; 
		oss << "<polarOffsets/>\n"; 
		oss << "<polarOffsetsType/>\n"; 
		oss << "<delayModelFixedParametersId/>\n"; 

		oss << "<nutationInLongitude/>\n"; 
		oss << "<nutationInLongitudeRate/>\n"; 
		oss << "<nutationInObliquity/>\n"; 
		oss << "<nutationInObliquityRate/>\n"; 
		oss << "</Attributes>\n";		
		oss << "</DelayModelVariableParametersTable>\n";

		return oss.str();				
	}
	
	string DelayModelVariableParametersTable::toMIME(const asdm::ByteOrder* byteOrder) {
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

	
	void DelayModelVariableParametersTable::setFromMIME(const string & mimeMsg) {
    string xmlPartMIMEHeader = "Content-ID: <header.xml>\n\n";
    
    string binPartMIMEHeader = "--MIME_boundary\nContent-Type: binary/octet-stream\nContent-ID: <content.bin>\n\n";
    
    // Detect the XML header.
    string::size_type loc0 = mimeMsg.find(xmlPartMIMEHeader, 0);
    if ( loc0 == string::npos) {
      // let's try with CRLFs
      xmlPartMIMEHeader = "Content-ID: <header.xml>\r\n\r\n";
      loc0 = mimeMsg.find(xmlPartMIMEHeader, 0);
      if  ( loc0 == string::npos ) 
	      throw ConversionException("Failed to detect the beginning of the XML header", "DelayModelVariableParameters");
    }

    loc0 += xmlPartMIMEHeader.size();
    
    // Look for the string announcing the binary part.
    string::size_type loc1 = mimeMsg.find( binPartMIMEHeader, loc0 );
    
    if ( loc1 == string::npos ) {
      throw ConversionException("Failed to detect the beginning of the binary part", "DelayModelVariableParameters");
    }
    
    //
    // Extract the xmlHeader and analyze it to find out what is the byte order and the sequence
    // of attribute names.
    //
    string xmlHeader = mimeMsg.substr(loc0, loc1-loc0);
    xmlDoc *doc;
    doc = xmlReadMemory(xmlHeader.data(), xmlHeader.size(), "BinaryTableHeader.xml", NULL, XML_PARSE_NOBLANKS);
    if ( doc == NULL ) 
      throw ConversionException("Failed to parse the xmlHeader into a DOM structure.", "DelayModelVariableParameters");
    
   // This vector will be filled by the names of  all the attributes of the table
   // in the order in which they are expected to be found in the binary representation.
   //
    vector<string> attributesSeq;
      
    xmlNode* root_element = xmlDocGetRootElement(doc);
    if ( root_element == NULL || root_element->type != XML_ELEMENT_NODE )
      throw ConversionException("Failed to parse the xmlHeader into a DOM structure.", "DelayModelVariableParameters");
    
    const ByteOrder* byteOrder=0;
    if ( string("ASDMBinaryTable").compare((const char*) root_element->name) == 0) {
      // Then it's an "old fashioned" MIME file for tables.
      // Just try to deserialize it with Big_Endian for the bytes ordering.
      byteOrder = asdm::ByteOrder::Big_Endian;
      
 	 //
    // Let's consider a  default order for the sequence of attributes.
    //
    
    	 
    attributesSeq.push_back("delayModelVariableParametersId") ; 
    	 
    attributesSeq.push_back("time") ; 
    	 
    attributesSeq.push_back("ut1_utc") ; 
    	 
    attributesSeq.push_back("iat_utc") ; 
    	 
    attributesSeq.push_back("timeType") ; 
    	 
    attributesSeq.push_back("gstAtUt0") ; 
    	 
    attributesSeq.push_back("earthRotationRate") ; 
    	 
    attributesSeq.push_back("polarOffsets") ; 
    	 
    attributesSeq.push_back("polarOffsetsType") ; 
    	 
    attributesSeq.push_back("delayModelFixedParametersId") ; 
    	
    	 
    attributesSeq.push_back("nutationInLongitude") ; 
    	 
    attributesSeq.push_back("nutationInLongitudeRate") ; 
    	 
    attributesSeq.push_back("nutationInObliquity") ; 
    	 
    attributesSeq.push_back("nutationInObliquityRate") ; 
    	
     
    
    
    // And decide that it has version == "2"
    version = "2";         
     }
    else if (string("DelayModelVariableParametersTable").compare((const char*) root_element->name) == 0) {
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
      	throw ConversionException ("Could not find the element '/DelayModelVariableParametersTable/BulkStoreRef'. Invalid XML header '"+ xmlHeader + "'.", "DelayModelVariableParameters");
      	
      // We found BulkStoreRef, now look for its attribute byteOrder.
      _xmlAttr* byteOrderAttr = 0;
      for (struct _xmlAttr* attr = bulkStoreRef->properties; attr; attr = attr->next) 
	  if (string("byteOrder").compare((const char*) attr->name) == 0) {
	   byteOrderAttr = attr;
	   break;
	 }
      
      if (byteOrderAttr == 0) 
	     throw ConversionException("Could not find the element '/DelayModelVariableParametersTable/BulkStoreRef/@byteOrder'. Invalid XML header '" + xmlHeader +"'.", "DelayModelVariableParameters");
      
      string byteOrderValue = string((const char*) byteOrderAttr->children->content);
      if (!(byteOrder = asdm::ByteOrder::fromString(byteOrderValue)))
		throw ConversionException("No valid value retrieved for the element '/DelayModelVariableParametersTable/BulkStoreRef/@byteOrder'. Invalid XML header '" + xmlHeader + "'.", "DelayModelVariableParameters");
		
	 //
	 // 2nd) Look for the Attributes element and grab the names of the elements it contains.
	 //
	 xmlNode* attributes = bulkStoreRef->next;
     if ( attributes == 0 || (attributes->type != XML_ELEMENT_NODE)  || (string("Attributes").compare((const char*) attributes->name) != 0))	 
       	throw ConversionException ("Could not find the element '/DelayModelVariableParametersTable/Attributes'. Invalid XML header '"+ xmlHeader + "'.", "DelayModelVariableParameters");
 
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
				DelayModelVariableParametersRow* aRow = DelayModelVariableParametersRow::fromBin((EndianIStream&) eiss, *this, attributesSeq);
				checkAndAdd(aRow);
      		}
    	}
    	catch (const DuplicateKey &e) {
      		throw ConversionException("Error while writing binary data , the message was "
				+ e.getMessage(), "DelayModelVariableParameters");
    	}
    	catch (const TagFormatException &e) {
     		 throw ConversionException("Error while reading binary data , the message was "
				+ e.getMessage(), "DelayModelVariableParameters");
    	}
    }
    else {
 		for (uint32_t i = 0; i < this->declaredSize; i++) {
			DelayModelVariableParametersRow* aRow = DelayModelVariableParametersRow::fromBin((EndianIStream&) eiss, *this, attributesSeq);
			append(aRow);
      	}   	
    }
    //Does not change the convention defined in the model.	
    //archiveAsBin = true;
    //fileAsBin = true;
    if ( doc != NULL ) xmlFreeDoc(doc);

	}
	
	void DelayModelVariableParametersTable::setUnknownAttributeBinaryReader(const string& attributeName, BinaryAttributeReaderFunctor* barFctr) {
		//
		// Is this attribute really unknown ?
		//
		for (vector<string>::const_iterator iter = attributesNamesOfDelayModelVariableParameters_v.begin(); iter != attributesNamesOfDelayModelVariableParameters_v.end(); iter++) {
			if ((*iter).compare(attributeName) == 0) 
				throw ConversionException("the attribute '"+attributeName+"' is known you can't override the way it's read in the MIME binary file containing the table.", "DelayModelVariableParameters"); 
		}
		
		// Ok then register the functor to activate when an unknown attribute is met during the reading of a binary table?
		unknownAttributes2Functors[attributeName] = barFctr;
	}
	
	BinaryAttributeReaderFunctor* DelayModelVariableParametersTable::getUnknownAttributeBinaryReader(const string& attributeName) const {
		map<string, BinaryAttributeReaderFunctor*>::const_iterator iter = unknownAttributes2Functors.find(attributeName);
		return (iter == unknownAttributes2Functors.end()) ? 0 : iter->second;
	}

	
	void DelayModelVariableParametersTable::toFile(string directory) {
		if (!directoryExists(directory.c_str()) &&
			!createPath(directory.c_str())) {
			throw ConversionException("Could not create directory " , directory);
		}

		string fileName = directory + "/DelayModelVariableParameters.xml";
		ofstream tableout(fileName.c_str(),ios::out|ios::trunc);
		if (tableout.rdstate() == ostream::failbit)
			throw ConversionException("Could not open file " + fileName + " to write ", "DelayModelVariableParameters");
		if (fileAsBin) 
			tableout << MIMEXMLPart();
		else
			tableout << toXML() << endl;
		tableout.close();
		if (tableout.rdstate() == ostream::failbit)
			throw ConversionException("Could not close file " + fileName, "DelayModelVariableParameters");

		if (fileAsBin) {
			// write the bin serialized
			string fileName = directory + "/DelayModelVariableParameters.bin";
			ofstream tableout(fileName.c_str(),ios::out|ios::trunc);
			if (tableout.rdstate() == ostream::failbit)
				throw ConversionException("Could not open file " + fileName + " to write ", "DelayModelVariableParameters");
			tableout << toMIME() << endl;
			tableout.close();
			if (tableout.rdstate() == ostream::failbit)
				throw ConversionException("Could not close file " + fileName, "DelayModelVariableParameters");
		}
	}

	
	void DelayModelVariableParametersTable::setFromFile(const string& directory) {
#ifndef WITHOUT_BOOST
    if (boost::filesystem::exists(boost::filesystem::path(uniqSlashes(directory + "/DelayModelVariableParameters.xml"))))
      setFromXMLFile(directory);
    else if (boost::filesystem::exists(boost::filesystem::path(uniqSlashes(directory + "/DelayModelVariableParameters.bin"))))
      setFromMIMEFile(directory);
#else 
    // alternative in Misc.h
    if (file_exists(uniqSlashes(directory + "/DelayModelVariableParameters.xml")))
      setFromXMLFile(directory);
    else if (file_exists(uniqSlashes(directory + "/DelayModelVariableParameters.bin")))
      setFromMIMEFile(directory);
#endif
    else
      throw ConversionException("No file found for the DelayModelVariableParameters table", "DelayModelVariableParameters");
	}			

	
  void DelayModelVariableParametersTable::setFromMIMEFile(const string& directory) {
    string tablePath ;
    
    tablePath = directory + "/DelayModelVariableParameters.bin";
    ifstream tablefile(tablePath.c_str(), ios::in|ios::binary);
    if (!tablefile.is_open()) { 
      throw ConversionException("Could not open file " + tablePath, "DelayModelVariableParameters");
    }
    // Read in a stringstream.
    stringstream ss; ss << tablefile.rdbuf();
    
    if (tablefile.rdstate() == istream::failbit || tablefile.rdstate() == istream::badbit) {
      throw ConversionException("Error reading file " + tablePath,"DelayModelVariableParameters");
    }
    
    // And close.
    tablefile.close();
    if (tablefile.rdstate() == istream::failbit)
      throw ConversionException("Could not close file " + tablePath,"DelayModelVariableParameters");
    
    setFromMIME(ss.str());
  }	
/* 
  void DelayModelVariableParametersTable::openMIMEFile (const string& directory) {
  		
  	// Open the file.
  	string tablePath ;
    tablePath = directory + "/DelayModelVariableParameters.bin";
    ifstream tablefile(tablePath.c_str(), ios::in|ios::binary);
    if (!tablefile.is_open())
      throw ConversionException("Could not open file " + tablePath, "DelayModelVariableParameters");
      
	// Locate the xmlPartMIMEHeader.
    string xmlPartMIMEHeader = "CONTENT-ID: <HEADER.XML>\n\n";
    CharComparator comparator;
    istreambuf_iterator<char> BEGIN(tablefile.rdbuf());
    istreambuf_iterator<char> END;
    istreambuf_iterator<char> it = search(BEGIN, END, xmlPartMIMEHeader.begin(), xmlPartMIMEHeader.end(), comparator);
    if (it == END) 
    	throw ConversionException("failed to detect the beginning of the XML header", "DelayModelVariableParameters");
    
    // Locate the binaryPartMIMEHeader while accumulating the characters of the xml header.	
    string binPartMIMEHeader = "--MIME_BOUNDARY\nCONTENT-TYPE: BINARY/OCTET-STREAM\nCONTENT-ID: <CONTENT.BIN>\n\n";
    string xmlHeader;
   	CharCompAccumulator compaccumulator(&xmlHeader, 100000);
   	++it;
   	it = search(it, END, binPartMIMEHeader.begin(), binPartMIMEHeader.end(), compaccumulator);
   	if (it == END) 
   		throw ConversionException("failed to detect the beginning of the binary part", "DelayModelVariableParameters");
   	
	cout << xmlHeader << endl;
	//
	// We have the xmlHeader , let's parse it.
	//
	xmlDoc *doc;
    doc = xmlReadMemory(xmlHeader.data(), xmlHeader.size(), "BinaryTableHeader.xml", NULL, XML_PARSE_NOBLANKS);
    if ( doc == NULL ) 
      throw ConversionException("Failed to parse the xmlHeader into a DOM structure.", "DelayModelVariableParameters");
    
   // This vector will be filled by the names of  all the attributes of the table
   // in the order in which they are expected to be found in the binary representation.
   //
    vector<string> attributesSeq(attributesNamesInBinOfDelayModelVariableParameters_v);
      
    xmlNode* root_element = xmlDocGetRootElement(doc);
    if ( root_element == NULL || root_element->type != XML_ELEMENT_NODE )
      throw ConversionException("Failed to parse the xmlHeader into a DOM structure.", "DelayModelVariableParameters");
    
    const ByteOrder* byteOrder=0;
    if ( string("ASDMBinaryTable").compare((const char*) root_element->name) == 0) {
      // Then it's an "old fashioned" MIME file for tables.
      // Just try to deserialize it with Big_Endian for the bytes ordering.
      byteOrder = asdm::ByteOrder::Big_Endian;
        
      // And decide that it has version == "2"
    version = "2";         
     }
    else if (string("DelayModelVariableParametersTable").compare((const char*) root_element->name) == 0) {
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
      	throw ConversionException ("Could not find the element '/DelayModelVariableParametersTable/BulkStoreRef'. Invalid XML header '"+ xmlHeader + "'.", "DelayModelVariableParameters");
      	
      // We found BulkStoreRef, now look for its attribute byteOrder.
      _xmlAttr* byteOrderAttr = 0;
      for (struct _xmlAttr* attr = bulkStoreRef->properties; attr; attr = attr->next) 
	  if (string("byteOrder").compare((const char*) attr->name) == 0) {
	   byteOrderAttr = attr;
	   break;
	 }
      
      if (byteOrderAttr == 0) 
	     throw ConversionException("Could not find the element '/DelayModelVariableParametersTable/BulkStoreRef/@byteOrder'. Invalid XML header '" + xmlHeader +"'.", "DelayModelVariableParameters");
      
      string byteOrderValue = string((const char*) byteOrderAttr->children->content);
      if (!(byteOrder = asdm::ByteOrder::fromString(byteOrderValue)))
		throw ConversionException("No valid value retrieved for the element '/DelayModelVariableParametersTable/BulkStoreRef/@byteOrder'. Invalid XML header '" + xmlHeader + "'.", "DelayModelVariableParameters");
		
	 //
	 // 2nd) Look for the Attributes element and grab the names of the elements it contains.
	 //
	 xmlNode* attributes = bulkStoreRef->next;
     if ( attributes == 0 || (attributes->type != XML_ELEMENT_NODE)  || (string("Attributes").compare((const char*) attributes->name) != 0))	 
       	throw ConversionException ("Could not find the element '/DelayModelVariableParametersTable/Attributes'. Invalid XML header '"+ xmlHeader + "'.", "DelayModelVariableParameters");
 
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

	
void DelayModelVariableParametersTable::setFromXMLFile(const string& directory) {
    string tablePath ;
    
    tablePath = directory + "/DelayModelVariableParameters.xml";
    
    /*
    ifstream tablefile(tablePath.c_str(), ios::in|ios::binary);
    if (!tablefile.is_open()) { 
      throw ConversionException("Could not open file " + tablePath, "DelayModelVariableParameters");
    }
      // Read in a stringstream.
    stringstream ss;
    ss << tablefile.rdbuf();
    
    if  (tablefile.rdstate() == istream::failbit || tablefile.rdstate() == istream::badbit) {
      throw ConversionException("Error reading file '" + tablePath + "'", "DelayModelVariableParameters");
    }
    
    // And close
    tablefile.close();
    if (tablefile.rdstate() == istream::failbit)
      throw ConversionException("Could not close file '" + tablePath + "'", "DelayModelVariableParameters");

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
    	throw ConversionException("Caugth an exception whose message is '" + e.getMessage() + "'.", "DelayModelVariableParameters");
    }
    
    if (xmlDocument.find("<BulkStoreRef") != string::npos)
      setFromMIMEFile(directory);
    else
      fromXML(xmlDocument);
  }

	

	

			
	
	

	
	void DelayModelVariableParametersTable::autoIncrement(string key, DelayModelVariableParametersRow* x) {
		map<string, int>::iterator iter;
		if ((iter=noAutoIncIds.find(key)) == noAutoIncIds.end()) {
			// There is not yet a combination of the non autoinc attributes values in the hashtable
			
			// Initialize  delayModelVariableParametersId to Tag(0).
			x->setDelayModelVariableParametersId(Tag(0,  TagType::DelayModelVariableParameters));
			
			// Record it in the map.		
			noAutoIncIds.insert(make_pair(key, 0));			
		} 
		else {
			// There is already a combination of the non autoinc attributes values in the hashtable
			// Increment its value.
			int n = iter->second + 1; 
			
			// Initialize  delayModelVariableParametersId to Tag(n).
			x->setDelayModelVariableParametersId(Tag(n, TagType::DelayModelVariableParameters));
			
			// Record it in the map.		
			noAutoIncIds.insert(make_pair(key, n));				
		}		
	}
	
} // End namespace asdm
 
