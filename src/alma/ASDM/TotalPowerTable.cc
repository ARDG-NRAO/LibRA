
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
 * File TotalPowerTable.cpp
 */
#include <alma/ASDM/ConversionException.h>
#include <alma/ASDM/DuplicateKey.h>
#include <alma/ASDM/OutOfBoundsException.h>

using asdm::ConversionException;
using asdm::DuplicateKey;
using asdm::OutOfBoundsException;

#include <alma/ASDM/ASDM.h>
#include <alma/ASDM/TotalPowerTable.h>
#include <alma/ASDM/TotalPowerRow.h>
#include <alma/ASDM/Parser.h>

using asdm::ASDM;
using asdm::TotalPowerTable;
using asdm::TotalPowerRow;
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
	static string entityNameOfTotalPower = "TotalPower";
	
	// An array of string containing the names of the columns of this table.
	// The array is filled in the order : key, required value, optional value.
	//
	static string attributesNamesOfTotalPower_a[] = {
		
			"time"
		,
			"configDescriptionId"
		,
			"fieldId"
		
		
			, "scanNumber"
		
			, "subscanNumber"
		
			, "integrationNumber"
		
			, "uvw"
		
			, "exposure"
		
			, "timeCentroid"
		
			, "floatData"
		
			, "flagAnt"
		
			, "flagPol"
		
			, "interval"
		
			, "stateId"
		
			, "execBlockId"
				
		
			, "subintegrationNumber"
				
	};
	
	// A vector of string whose content is a copy of the strings in the array above.
	//
	static vector<string> attributesNamesOfTotalPower_v (attributesNamesOfTotalPower_a, attributesNamesOfTotalPower_a + sizeof(attributesNamesOfTotalPower_a) / sizeof(attributesNamesOfTotalPower_a[0]));

	// An array of string containing the names of the columns of this table.
	// The array is filled in the order where the names would be read by default in the XML header of a file containing
	// the table exported in binary mode.
	//	
	static string attributesNamesInBinOfTotalPower_a[] = {
    
    	 "time" , "configDescriptionId" , "fieldId" , "scanNumber" , "subscanNumber" , "integrationNumber" , "uvw" , "exposure" , "timeCentroid" , "floatData" , "flagAnt" , "flagPol" , "interval" , "stateId" , "execBlockId" 
    	,
    	 "subintegrationNumber" 
    
	};
	        			
	// A vector of string whose content is a copy of the strings in the array above.
	//
	static vector<string> attributesNamesInBinOfTotalPower_v(attributesNamesInBinOfTotalPower_a, attributesNamesInBinOfTotalPower_a + sizeof(attributesNamesInBinOfTotalPower_a) / sizeof(attributesNamesInBinOfTotalPower_a[0]));		
	

	// The array of attributes (or column) names that make up key key.
	//
	string keyOfTotalPower_a[] = {
	
		"time"
	,
		"configDescriptionId"
	,
		"fieldId"
		 
	};
	 
	// A vector of strings which are copies of those stored in the array above.
	vector<string> keyOfTotalPower_v(keyOfTotalPower_a, keyOfTotalPower_a + sizeof(keyOfTotalPower_a) / sizeof(keyOfTotalPower_a[0]));

	/**
	 * Return the list of field names that make up key key
	 * as a const reference to a vector of strings.
	 */	
	const vector<string>& TotalPowerTable::getKeyName() {
		return keyOfTotalPower_v;
	}


	TotalPowerTable::TotalPowerTable(ASDM &c) : container(c) {

		// Define a default entity.
		entity.setEntityId(EntityId("uid://X0/X0/X0"));
		entity.setEntityIdEncrypted("na");
		entity.setEntityTypeName("TotalPowerTable");
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
 * A destructor for TotalPowerTable.
 */
	TotalPowerTable::~TotalPowerTable() {
		for (unsigned int i = 0; i < privateRows.size(); i++) 
			delete(privateRows.at(i));
	}

	/**
	 * Container to which this table belongs.
	 */
	ASDM &TotalPowerTable::getContainer() const {
		return container;
	}

	/**
	 * Return the number of rows in the table.
	 */
	unsigned int TotalPowerTable::size() const {
		if (presentInMemory) 
			return privateRows.size();
		else
			return declaredSize;
	}
	
	/**
	 * Return the name of this table.
	 */
	string TotalPowerTable::getName() const {
		return entityNameOfTotalPower;
	}
	
	/**
	 * Return the name of this table.
	 */
	string TotalPowerTable::name() {
		return entityNameOfTotalPower;
	}
	
	/**
	 * Return the the names of the attributes (or columns) of this table.
	 */
	const vector<string>& TotalPowerTable::getAttributesNames() { return attributesNamesOfTotalPower_v; }
	
	/**
	 * Return the the names of the attributes (or columns) of this table as they appear by default
	 * in an binary export of this table.
	 */
	const vector<string>& TotalPowerTable::defaultAttributesNamesInBin() { return attributesNamesInBinOfTotalPower_v; }

	/**
	 * Return this table's Entity.
	 */
	Entity TotalPowerTable::getEntity() const {
		return entity;
	}

	/**
	 * Set this table's Entity.
	 */
	void TotalPowerTable::setEntity(Entity e) {
		this->entity = e; 
	}
	
	//
	// ====> Row creation.
	//
	
	/**
	 * Create a new row.
	 */
	TotalPowerRow *TotalPowerTable::newRow() {
		return new TotalPowerRow (*this);
	}
	

	/**
	 * Create a new row initialized to the specified values.
	 * @return a pointer on the created and initialized row.
	
 	 * @param time 
	
 	 * @param configDescriptionId 
	
 	 * @param fieldId 
	
 	 * @param scanNumber 
	
 	 * @param subscanNumber 
	
 	 * @param integrationNumber 
	
 	 * @param uvw 
	
 	 * @param exposure 
	
 	 * @param timeCentroid 
	
 	 * @param floatData 
	
 	 * @param flagAnt 
	
 	 * @param flagPol 
	
 	 * @param interval 
	
 	 * @param stateId 
	
 	 * @param execBlockId 
	
     */
	TotalPowerRow* TotalPowerTable::newRow(ArrayTime time, Tag configDescriptionId, Tag fieldId, int scanNumber, int subscanNumber, int integrationNumber, std::vector<std::vector<Length > > uvw, std::vector<std::vector<Interval > > exposure, std::vector<std::vector<ArrayTime > > timeCentroid, std::vector<std::vector<std::vector<float > > > floatData, std::vector<int > flagAnt, std::vector<std::vector<int > > flagPol, Interval interval, std::vector<Tag>  stateId, Tag execBlockId){
		TotalPowerRow *row = new TotalPowerRow(*this);
			
		row->setTime(time);
			
		row->setConfigDescriptionId(configDescriptionId);
			
		row->setFieldId(fieldId);
			
		row->setScanNumber(scanNumber);
			
		row->setSubscanNumber(subscanNumber);
			
		row->setIntegrationNumber(integrationNumber);
			
		row->setUvw(uvw);
			
		row->setExposure(exposure);
			
		row->setTimeCentroid(timeCentroid);
			
		row->setFloatData(floatData);
			
		row->setFlagAnt(flagAnt);
			
		row->setFlagPol(flagPol);
			
		row->setInterval(interval);
			
		row->setStateId(stateId);
			
		row->setExecBlockId(execBlockId);
	
		return row;		
	}	
	


TotalPowerRow* TotalPowerTable::newRow(TotalPowerRow* row) {
	return new TotalPowerRow(*this, row);
}

	//
	// Append a row to its table.
	//

	
	
		
		
	/** 
	 * Returns a string built by concatenating the ascii representation of the
	 * parameters values suffixed with a "_" character.
	 */
	 string TotalPowerTable::Key(Tag configDescriptionId, Tag fieldId) {
	 	ostringstream ostrstr;
	 		ostrstr  
			
				<<  configDescriptionId.toString()  << "_"
			
				<<  fieldId.toString()  << "_"
			
			;
		return ostrstr.str();	 	
	 }
	 
			
			
	TotalPowerRow* TotalPowerTable::add(TotalPowerRow* x) {
		string keystr = Key(
						x->getConfigDescriptionId()
					   ,
						x->getFieldId()
					   );
		if (context.find(keystr) == context.end()) {
			vector<TotalPowerRow *> v;
			context[keystr] = v;
		}
		return insertByTime(x, context[keystr]);					
	}
			
		
	
		
	void TotalPowerTable::addWithoutCheckingUnique(TotalPowerRow * x) {
		TotalPowerRow * dummy = checkAndAdd(x, true); // We require the check for uniqueness to be skipped.
		                                           // by passing true in the second parameter
		                                           // whose value by default is false.
		// this statement is never executed, but it hides the unused return value from the compiler to silence that warning.
                if (false) cout << (unsigned long long) dummy;
	}
	



	// 
	// A private method to append a row to its table, used by input conversion
	// methods, with row uniqueness.
	//

	
	
		
		
			
			
			
			
	TotalPowerRow*  TotalPowerTable::checkAndAdd(TotalPowerRow* x, bool /* skipCheckUniqueness */ ) {
		string keystr = Key( 
						x->getConfigDescriptionId() 
					   , 
						x->getFieldId() 
					   ); 
		if (context.find(keystr) == context.end()) {
			vector<TotalPowerRow *> v;
			context[keystr] = v;
		}
		
		vector<TotalPowerRow*>& found = context.find(keystr)->second;
		return insertByTime(x, found);	
	}				
			
					
		



	//
	// A private method to brutally append a row to its table, without checking for row uniqueness.
	//

	void TotalPowerTable::append(TotalPowerRow *x) {
		privateRows.push_back(x);
		x->isAdded(true);
	}





	 vector<TotalPowerRow *> TotalPowerTable::get() {
	 	checkPresenceInMemory();
	    return privateRows;
	 }
	 
	 const vector<TotalPowerRow *>& TotalPowerTable::get() const {
	 	const_cast<TotalPowerTable&>(*this).checkPresenceInMemory();	
	    return privateRows;
	 }	 
	 	




	

	
	
		
	 vector<TotalPowerRow *> *TotalPowerTable::getByContext(Tag configDescriptionId, Tag fieldId) {
	 	//if (getContainer().checkRowUniqueness() == false)
	 		//throw IllegalAccessException ("The method 'getByContext' can't be called because the dataset has been built without checking the row uniqueness.", "TotalPowerTable");

	 	checkPresenceInMemory();
	  	string k = Key(configDescriptionId, fieldId);
 
	    if (context.find(k) == context.end()) return 0;
 	   else return &(context[k]);		
	}		
		
	


	
		
		
			
			
			
/*
 ** Returns a TotalPowerRow* given a key.
 ** @return a pointer to the row having the key whose values are passed as parameters, or 0 if
 ** no row exists for that key.
 **
 */
 				
				
 	TotalPowerRow* TotalPowerTable::getRowByKey(ArrayTime time, Tag configDescriptionId, Tag fieldId)  {
 		checkPresenceInMemory();
		string keystr = Key(configDescriptionId, fieldId);
 		
 		if (context.find(keystr) == context.end()) return 0;
 		
 		vector<TotalPowerRow* > row = context[keystr];
 		
 		// Is the vector empty...impossible in principle !
		if (row.size() == 0) return 0;
		
		// Only one element in the vector
		if (row.size() == 1) {
			if (time.get() == row.at(0)->getTime().get())
				return row.at(0);
			else
				return 0;	
		}
		
		// Optimizations 
		TotalPowerRow* last = row.at(row.size()-1);		
		if (time.get() > last->getTime().get()) return 0;
		TotalPowerRow* first = row.at(0);
		if (time.get() < first->getTime().get()) return 0;
		
		// More than one row
		// let's use a dichotomy method for the general case..		
		int k0 = 0;
		int k1 = row.size() - 1;    	  
		while (k0 !=  k1 ) {
			if (time.get() == row.at(k0)->getTime().get()) {
				return row.at(k0);
			}
			else if (time.get() == row.at(k1)->getTime().get()) {
				return row.at(k1);
			}
			else {
				if (time.get() <= row.at((k0+k1)/2)->getTime().get())
					k1 = (k0 + k1) / 2;
				else
					k0 = (k0 + k1) / 2;				
			} 	
		}
		return 0; 			
	}								
							
			
		
		
		
	



#ifndef WITHOUT_ACS
	using asdmIDL::TotalPowerTableIDL;
#endif

#ifndef WITHOUT_ACS
	// Conversion Methods

	TotalPowerTableIDL *TotalPowerTable::toIDL() {
		TotalPowerTableIDL *x = new TotalPowerTableIDL ();
		unsigned int nrow = size();
		x->row.length(nrow);
		vector<TotalPowerRow*> v = get();
		for (unsigned int i = 0; i < nrow; ++i) {
			//x->row[i] = *(v[i]->toIDL());
			v[i]->toIDL(x->row[i]);
		}
		return x;
	}
	
	void TotalPowerTable::toIDL(asdmIDL::TotalPowerTableIDL& x) const {
		unsigned int nrow = size();
		x.row.length(nrow);
		vector<TotalPowerRow*> v = get();
		for (unsigned int i = 0; i < nrow; ++i) {
			v[i]->toIDL(x.row[i]);
		}
	}	
#endif
	
#ifndef WITHOUT_ACS
	void TotalPowerTable::fromIDL(TotalPowerTableIDL x) {
		unsigned int nrow = x.row.length();
		for (unsigned int i = 0; i < nrow; ++i) {
			TotalPowerRow *tmp = newRow();
			tmp->setFromIDL(x.row[i]);
			// checkAndAdd(tmp);
			add(tmp);
		}
	}	
#endif

	
	string TotalPowerTable::toXML()  {
		string buf;

		buf.append("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?> ");
		buf.append("<TotalPowerTable xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:ttlpwr=\"http://Alma/XASDM/TotalPowerTable\" xsi:schemaLocation=\"http://Alma/XASDM/TotalPowerTable http://almaobservatory.org/XML/XASDM/4/TotalPowerTable.xsd\" schemaVersion=\"4\" schemaRevision=\"-1\">\n");
	
		buf.append(entity.toXML());
		string s = container.getEntity().toXML();
		// Change the "Entity" tag to "ContainerEntity".
		buf.append("<Container" + s.substr(1,s.length() - 1)+" ");
		vector<TotalPowerRow*> v = get();
		for (unsigned int i = 0; i < v.size(); ++i) {
			try {
				buf.append(v[i]->toXML());
			} catch (const NoSuchRow &e) {
			}
			buf.append("  ");
		}		
		buf.append("</TotalPowerTable> ");
		return buf;
	}

	
	string TotalPowerTable::getVersion() const {
		return version;
	}
	

	void TotalPowerTable::fromXML(string& tableInXML)  {
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
			throw ConversionException("Failed to parse the xmlHeader into a DOM structure.", "TotalPower");
		
		xmlNode* root_element = xmlDocGetRootElement(doc);
   		if ( root_element == NULL || root_element->type != XML_ELEMENT_NODE )
      		throw ConversionException("Failed to retrieve the root element in the DOM structure.", "TotalPower");
      		
      	xmlChar * propValue = xmlGetProp(root_element, (const xmlChar *) "schemaVersion");
      	if ( propValue != 0 ) {
      		version = string( (const char*) propValue);
      		xmlFree(propValue);   		
      	}
      		     							
		Parser xml(tableInXML);
		if (!xml.isStr("<TotalPowerTable")) 
			error();
		// cout << "Parsing a TotalPowerTable" << endl;
		string s = xml.getElement("<Entity","/>");
		if (s.length() == 0) 
			error();
		Entity e;
		e.setFromXML(s);
		if (e.getEntityTypeName() != "TotalPowerTable")
			error();
		setEntity(e);
		// Skip the container's entity; but, it has to be there.
		s = xml.getElement("<ContainerEntity","/>");
		if (s.length() == 0) 
			error();

		// Get each row in the table.
		s = xml.getElementContent("<row>","</row>");
		TotalPowerRow *row;
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
				throw ConversionException(e1.getMessage(),"TotalPowerTable");
			} 
			catch (const UniquenessViolationException &e1) {
				throw ConversionException(e1.getMessage(),"TotalPowerTable");	
			}
			catch (...) {
				// cout << "Unexpected error in TotalPowerTable::checkAndAdd called from TotalPowerTable::fromXML " << endl;
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
				throw ConversionException(e1.getMessage(),"TotalPowerTable");
			} 
			catch (...) {
				// cout << "Unexpected error in TotalPowerTable::addWithoutCheckingUnique called from TotalPowerTable::fromXML " << endl;
			}
		}				
				
				
		if (!xml.isStr("</TotalPowerTable>")) 
		error();
		
		//Does not change the convention defined in the model.	
		//archiveAsBin = false;
		//fileAsBin = false;

                // clean up the xmlDoc pointer
		if ( doc != NULL ) xmlFreeDoc(doc);
		
	}

	
	void TotalPowerTable::error()  {
		throw ConversionException("Invalid xml document","TotalPower");
	}
	
	
	string TotalPowerTable::MIMEXMLPart(const asdm::ByteOrder* byteOrder) {
		string UID = getEntity().getEntityId().toString();
		string withoutUID = UID.substr(6);
		string containerUID = getContainer().getEntity().getEntityId().toString();
		ostringstream oss;
		oss << "<?xml version='1.0'  encoding='ISO-8859-1'?>";
		oss << "\n";
		oss << "<TotalPowerTable xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:ttlpwr=\"http://Alma/XASDM/TotalPowerTable\" xsi:schemaLocation=\"http://Alma/XASDM/TotalPowerTable http://almaobservatory.org/XML/XASDM/4/TotalPowerTable.xsd\" schemaVersion=\"4\" schemaRevision=\"-1\">\n";
		oss<< "<Entity entityId='"<<UID<<"' entityIdEncrypted='na' entityTypeName='TotalPowerTable' schemaVersion='1' documentVersion='1'/>\n";
		oss<< "<ContainerEntity entityId='"<<containerUID<<"' entityIdEncrypted='na' entityTypeName='ASDM' schemaVersion='1' documentVersion='1'/>\n";
		oss << "<BulkStoreRef file_id='"<<withoutUID<<"' byteOrder='"<<byteOrder->toString()<<"' />\n";
		oss << "<Attributes>\n";

		oss << "<time/>\n"; 
		oss << "<configDescriptionId/>\n"; 
		oss << "<fieldId/>\n"; 
		oss << "<scanNumber/>\n"; 
		oss << "<subscanNumber/>\n"; 
		oss << "<integrationNumber/>\n"; 
		oss << "<uvw/>\n"; 
		oss << "<exposure/>\n"; 
		oss << "<timeCentroid/>\n"; 
		oss << "<floatData/>\n"; 
		oss << "<flagAnt/>\n"; 
		oss << "<flagPol/>\n"; 
		oss << "<interval/>\n"; 
		oss << "<stateId/>\n"; 
		oss << "<execBlockId/>\n"; 

		oss << "<subintegrationNumber/>\n"; 
		oss << "</Attributes>\n";		
		oss << "</TotalPowerTable>\n";

		return oss.str();				
	}
	
	string TotalPowerTable::toMIME(const asdm::ByteOrder* byteOrder) {
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

	
	void TotalPowerTable::setFromMIME(const string & mimeMsg) {
    string xmlPartMIMEHeader = "Content-ID: <header.xml>\n\n";
    
    string binPartMIMEHeader = "--MIME_boundary\nContent-Type: binary/octet-stream\nContent-ID: <content.bin>\n\n";
    
    // Detect the XML header.
    string::size_type loc0 = mimeMsg.find(xmlPartMIMEHeader, 0);
    if ( loc0 == string::npos) {
      // let's try with CRLFs
      xmlPartMIMEHeader = "Content-ID: <header.xml>\r\n\r\n";
      loc0 = mimeMsg.find(xmlPartMIMEHeader, 0);
      if  ( loc0 == string::npos ) 
	      throw ConversionException("Failed to detect the beginning of the XML header", "TotalPower");
    }

    loc0 += xmlPartMIMEHeader.size();
    
    // Look for the string announcing the binary part.
    string::size_type loc1 = mimeMsg.find( binPartMIMEHeader, loc0 );
    
    if ( loc1 == string::npos ) {
      throw ConversionException("Failed to detect the beginning of the binary part", "TotalPower");
    }
    
    //
    // Extract the xmlHeader and analyze it to find out what is the byte order and the sequence
    // of attribute names.
    //
    string xmlHeader = mimeMsg.substr(loc0, loc1-loc0);
    xmlDoc *doc;
    doc = xmlReadMemory(xmlHeader.data(), xmlHeader.size(), "BinaryTableHeader.xml", NULL, XML_PARSE_NOBLANKS);
    if ( doc == NULL ) 
      throw ConversionException("Failed to parse the xmlHeader into a DOM structure.", "TotalPower");
    
   // This vector will be filled by the names of  all the attributes of the table
   // in the order in which they are expected to be found in the binary representation.
   //
    vector<string> attributesSeq;
      
    xmlNode* root_element = xmlDocGetRootElement(doc);
    if ( root_element == NULL || root_element->type != XML_ELEMENT_NODE )
      throw ConversionException("Failed to parse the xmlHeader into a DOM structure.", "TotalPower");
    
    const ByteOrder* byteOrder=0;
    if ( string("ASDMBinaryTable").compare((const char*) root_element->name) == 0) {
      // Then it's an "old fashioned" MIME file for tables.
      // Just try to deserialize it with Big_Endian for the bytes ordering.
      byteOrder = asdm::ByteOrder::Big_Endian;
      
 	 //
    // Let's consider a  default order for the sequence of attributes.
    //
    
    	 
    attributesSeq.push_back("time") ; 
    	 
    attributesSeq.push_back("configDescriptionId") ; 
    	 
    attributesSeq.push_back("fieldId") ; 
    	 
    attributesSeq.push_back("scanNumber") ; 
    	 
    attributesSeq.push_back("subscanNumber") ; 
    	 
    attributesSeq.push_back("integrationNumber") ; 
    	 
    attributesSeq.push_back("uvw") ; 
    	 
    attributesSeq.push_back("exposure") ; 
    	 
    attributesSeq.push_back("timeCentroid") ; 
    	 
    attributesSeq.push_back("floatData") ; 
    	 
    attributesSeq.push_back("flagAnt") ; 
    	 
    attributesSeq.push_back("flagPol") ; 
    	 
    attributesSeq.push_back("interval") ; 
    	 
    attributesSeq.push_back("stateId") ; 
    	 
    attributesSeq.push_back("execBlockId") ; 
    	
    	 
    attributesSeq.push_back("subintegrationNumber") ; 
    	
     
    
    
    // And decide that it has version == "2"
    version = "2";         
     }
    else if (string("TotalPowerTable").compare((const char*) root_element->name) == 0) {
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
      	throw ConversionException ("Could not find the element '/TotalPowerTable/BulkStoreRef'. Invalid XML header '"+ xmlHeader + "'.", "TotalPower");
      	
      // We found BulkStoreRef, now look for its attribute byteOrder.
      _xmlAttr* byteOrderAttr = 0;
      for (struct _xmlAttr* attr = bulkStoreRef->properties; attr; attr = attr->next) 
	  if (string("byteOrder").compare((const char*) attr->name) == 0) {
	   byteOrderAttr = attr;
	   break;
	 }
      
      if (byteOrderAttr == 0) 
	     throw ConversionException("Could not find the element '/TotalPowerTable/BulkStoreRef/@byteOrder'. Invalid XML header '" + xmlHeader +"'.", "TotalPower");
      
      string byteOrderValue = string((const char*) byteOrderAttr->children->content);
      if (!(byteOrder = asdm::ByteOrder::fromString(byteOrderValue)))
		throw ConversionException("No valid value retrieved for the element '/TotalPowerTable/BulkStoreRef/@byteOrder'. Invalid XML header '" + xmlHeader + "'.", "TotalPower");
		
	 //
	 // 2nd) Look for the Attributes element and grab the names of the elements it contains.
	 //
	 xmlNode* attributes = bulkStoreRef->next;
     if ( attributes == 0 || (attributes->type != XML_ELEMENT_NODE)  || (string("Attributes").compare((const char*) attributes->name) != 0))	 
       	throw ConversionException ("Could not find the element '/TotalPowerTable/Attributes'. Invalid XML header '"+ xmlHeader + "'.", "TotalPower");
 
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
				TotalPowerRow* aRow = TotalPowerRow::fromBin((EndianIStream&) eiss, *this, attributesSeq);
				checkAndAdd(aRow);
      		}
    	}
    	catch (const DuplicateKey &e) {
      		throw ConversionException("Error while writing binary data , the message was "
				+ e.getMessage(), "TotalPower");
    	}
    	catch (const TagFormatException &e) {
     		 throw ConversionException("Error while reading binary data , the message was "
				+ e.getMessage(), "TotalPower");
    	}
    }
    else {
 		for (uint32_t i = 0; i < this->declaredSize; i++) {
			TotalPowerRow* aRow = TotalPowerRow::fromBin((EndianIStream&) eiss, *this, attributesSeq);
			append(aRow);
      	}   	
    }
    //Does not change the convention defined in the model.	
    //archiveAsBin = true;
    //fileAsBin = true;
    if ( doc != NULL ) xmlFreeDoc(doc);

	}
	
	void TotalPowerTable::setUnknownAttributeBinaryReader(const string& attributeName, BinaryAttributeReaderFunctor* barFctr) {
		//
		// Is this attribute really unknown ?
		//
		for (vector<string>::const_iterator iter = attributesNamesOfTotalPower_v.begin(); iter != attributesNamesOfTotalPower_v.end(); iter++) {
			if ((*iter).compare(attributeName) == 0) 
				throw ConversionException("the attribute '"+attributeName+"' is known you can't override the way it's read in the MIME binary file containing the table.", "TotalPower"); 
		}
		
		// Ok then register the functor to activate when an unknown attribute is met during the reading of a binary table?
		unknownAttributes2Functors[attributeName] = barFctr;
	}
	
	BinaryAttributeReaderFunctor* TotalPowerTable::getUnknownAttributeBinaryReader(const string& attributeName) const {
		map<string, BinaryAttributeReaderFunctor*>::const_iterator iter = unknownAttributes2Functors.find(attributeName);
		return (iter == unknownAttributes2Functors.end()) ? 0 : iter->second;
	}

	
	void TotalPowerTable::toFile(string directory) {
		if (!directoryExists(directory.c_str()) &&
			!createPath(directory.c_str())) {
			throw ConversionException("Could not create directory " , directory);
		}

		string fileName = directory + "/TotalPower.xml";
		ofstream tableout(fileName.c_str(),ios::out|ios::trunc);
		if (tableout.rdstate() == ostream::failbit)
			throw ConversionException("Could not open file " + fileName + " to write ", "TotalPower");
		if (fileAsBin) 
			tableout << MIMEXMLPart();
		else
			tableout << toXML() << endl;
		tableout.close();
		if (tableout.rdstate() == ostream::failbit)
			throw ConversionException("Could not close file " + fileName, "TotalPower");

		if (fileAsBin) {
			// write the bin serialized
			string fileName = directory + "/TotalPower.bin";
			ofstream tableout(fileName.c_str(),ios::out|ios::trunc);
			if (tableout.rdstate() == ostream::failbit)
				throw ConversionException("Could not open file " + fileName + " to write ", "TotalPower");
			tableout << toMIME() << endl;
			tableout.close();
			if (tableout.rdstate() == ostream::failbit)
				throw ConversionException("Could not close file " + fileName, "TotalPower");
		}
	}

	
	void TotalPowerTable::setFromFile(const string& directory) {
#ifndef WITHOUT_BOOST
    if (boost::filesystem::exists(boost::filesystem::path(uniqSlashes(directory + "/TotalPower.xml"))))
      setFromXMLFile(directory);
    else if (boost::filesystem::exists(boost::filesystem::path(uniqSlashes(directory + "/TotalPower.bin"))))
      setFromMIMEFile(directory);
#else 
    // alternative in Misc.h
    if (file_exists(uniqSlashes(directory + "/TotalPower.xml")))
      setFromXMLFile(directory);
    else if (file_exists(uniqSlashes(directory + "/TotalPower.bin")))
      setFromMIMEFile(directory);
#endif
    else
      throw ConversionException("No file found for the TotalPower table", "TotalPower");
	}			

	
  void TotalPowerTable::setFromMIMEFile(const string& directory) {
    string tablePath ;
    
    tablePath = directory + "/TotalPower.bin";
    ifstream tablefile(tablePath.c_str(), ios::in|ios::binary);
    if (!tablefile.is_open()) { 
      throw ConversionException("Could not open file " + tablePath, "TotalPower");
    }
    // Read in a stringstream.
    stringstream ss; ss << tablefile.rdbuf();
    
    if (tablefile.rdstate() == istream::failbit || tablefile.rdstate() == istream::badbit) {
      throw ConversionException("Error reading file " + tablePath,"TotalPower");
    }
    
    // And close.
    tablefile.close();
    if (tablefile.rdstate() == istream::failbit)
      throw ConversionException("Could not close file " + tablePath,"TotalPower");
    
    setFromMIME(ss.str());
  }	
/* 
  void TotalPowerTable::openMIMEFile (const string& directory) {
  		
  	// Open the file.
  	string tablePath ;
    tablePath = directory + "/TotalPower.bin";
    ifstream tablefile(tablePath.c_str(), ios::in|ios::binary);
    if (!tablefile.is_open())
      throw ConversionException("Could not open file " + tablePath, "TotalPower");
      
	// Locate the xmlPartMIMEHeader.
    string xmlPartMIMEHeader = "CONTENT-ID: <HEADER.XML>\n\n";
    CharComparator comparator;
    istreambuf_iterator<char> BEGIN(tablefile.rdbuf());
    istreambuf_iterator<char> END;
    istreambuf_iterator<char> it = search(BEGIN, END, xmlPartMIMEHeader.begin(), xmlPartMIMEHeader.end(), comparator);
    if (it == END) 
    	throw ConversionException("failed to detect the beginning of the XML header", "TotalPower");
    
    // Locate the binaryPartMIMEHeader while accumulating the characters of the xml header.	
    string binPartMIMEHeader = "--MIME_BOUNDARY\nCONTENT-TYPE: BINARY/OCTET-STREAM\nCONTENT-ID: <CONTENT.BIN>\n\n";
    string xmlHeader;
   	CharCompAccumulator compaccumulator(&xmlHeader, 100000);
   	++it;
   	it = search(it, END, binPartMIMEHeader.begin(), binPartMIMEHeader.end(), compaccumulator);
   	if (it == END) 
   		throw ConversionException("failed to detect the beginning of the binary part", "TotalPower");
   	
	cout << xmlHeader << endl;
	//
	// We have the xmlHeader , let's parse it.
	//
	xmlDoc *doc;
    doc = xmlReadMemory(xmlHeader.data(), xmlHeader.size(), "BinaryTableHeader.xml", NULL, XML_PARSE_NOBLANKS);
    if ( doc == NULL ) 
      throw ConversionException("Failed to parse the xmlHeader into a DOM structure.", "TotalPower");
    
   // This vector will be filled by the names of  all the attributes of the table
   // in the order in which they are expected to be found in the binary representation.
   //
    vector<string> attributesSeq(attributesNamesInBinOfTotalPower_v);
      
    xmlNode* root_element = xmlDocGetRootElement(doc);
    if ( root_element == NULL || root_element->type != XML_ELEMENT_NODE )
      throw ConversionException("Failed to parse the xmlHeader into a DOM structure.", "TotalPower");
    
    const ByteOrder* byteOrder=0;
    if ( string("ASDMBinaryTable").compare((const char*) root_element->name) == 0) {
      // Then it's an "old fashioned" MIME file for tables.
      // Just try to deserialize it with Big_Endian for the bytes ordering.
      byteOrder = asdm::ByteOrder::Big_Endian;
        
      // And decide that it has version == "2"
    version = "2";         
     }
    else if (string("TotalPowerTable").compare((const char*) root_element->name) == 0) {
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
      	throw ConversionException ("Could not find the element '/TotalPowerTable/BulkStoreRef'. Invalid XML header '"+ xmlHeader + "'.", "TotalPower");
      	
      // We found BulkStoreRef, now look for its attribute byteOrder.
      _xmlAttr* byteOrderAttr = 0;
      for (struct _xmlAttr* attr = bulkStoreRef->properties; attr; attr = attr->next) 
	  if (string("byteOrder").compare((const char*) attr->name) == 0) {
	   byteOrderAttr = attr;
	   break;
	 }
      
      if (byteOrderAttr == 0) 
	     throw ConversionException("Could not find the element '/TotalPowerTable/BulkStoreRef/@byteOrder'. Invalid XML header '" + xmlHeader +"'.", "TotalPower");
      
      string byteOrderValue = string((const char*) byteOrderAttr->children->content);
      if (!(byteOrder = asdm::ByteOrder::fromString(byteOrderValue)))
		throw ConversionException("No valid value retrieved for the element '/TotalPowerTable/BulkStoreRef/@byteOrder'. Invalid XML header '" + xmlHeader + "'.", "TotalPower");
		
	 //
	 // 2nd) Look for the Attributes element and grab the names of the elements it contains.
	 //
	 xmlNode* attributes = bulkStoreRef->next;
     if ( attributes == 0 || (attributes->type != XML_ELEMENT_NODE)  || (string("Attributes").compare((const char*) attributes->name) != 0))	 
       	throw ConversionException ("Could not find the element '/TotalPowerTable/Attributes'. Invalid XML header '"+ xmlHeader + "'.", "TotalPower");
 
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

	
void TotalPowerTable::setFromXMLFile(const string& directory) {
    string tablePath ;
    
    tablePath = directory + "/TotalPower.xml";
    
    /*
    ifstream tablefile(tablePath.c_str(), ios::in|ios::binary);
    if (!tablefile.is_open()) { 
      throw ConversionException("Could not open file " + tablePath, "TotalPower");
    }
      // Read in a stringstream.
    stringstream ss;
    ss << tablefile.rdbuf();
    
    if  (tablefile.rdstate() == istream::failbit || tablefile.rdstate() == istream::badbit) {
      throw ConversionException("Error reading file '" + tablePath + "'", "TotalPower");
    }
    
    // And close
    tablefile.close();
    if (tablefile.rdstate() == istream::failbit)
      throw ConversionException("Could not close file '" + tablePath + "'", "TotalPower");

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
    	throw ConversionException("Caugth an exception whose message is '" + e.getMessage() + "'.", "TotalPower");
    }
    
    if (xmlDocument.find("<BulkStoreRef") != string::npos)
      setFromMIMEFile(directory);
    else
      fromXML(xmlDocument);
  }

	

	

			
	
		
    	
 	TotalPowerRow * TotalPowerTable::insertByTime(TotalPowerRow* x, vector<TotalPowerRow *>&row ) {
		ArrayTime start = x->getTime();
		
		// Is the vector empty ?
		if (row.size() == 0) {
			row.push_back(x);
			privateRows.push_back(x);
			x->isAdded(true);
			return x;
		}
		
		// Optimization for the case of insertion by ascending time.
		TotalPowerRow* last = row.at(row.size()-1);
		
		if (start.get() > last->getTime().get()) {
			row.push_back(x);
			privateRows.push_back(x);
			x->isAdded(true);
			return x;
		}
		
		// Optimization for the case of insertion by descending time.
		TotalPowerRow* first = row.at(0);
		
		if (start.get() < first->getTime().get()) {
			row.insert(row.begin(), x);
			privateRows.push_back(x);
			x->isAdded(true);
			return x;
		}
		
		// Case where x has to be inserted inside row; let's use a dichotomy
		// method to find the insertion index.		
		int k0 = 0;
		int k1 = row.size() - 1;    	  
		while (k0 != (k1 - 1)) {
			if (start.get() == row.at(k0)->getTime().get()) {
				if (row.at(k0)->equalByRequiredValue(x))
					return row.at(k0);
				else
					throw DuplicateKey("DuplicateKey exception in ", "TotalPowerTable");	
			}
			else if (start.get() == row.at(k1)->getTime().get()) {
				if (row.at(k1)->equalByRequiredValue(x))
					return row.at(k1);
				else
					throw  DuplicateKey("DuplicateKey exception in ", "TotalPowerTable");	
			}
			else {
				if (start.get() <= row.at((k0+k1)/2)->getTime().get())
					k1 = (k0 + k1) / 2;
				else
					k0 = (k0 + k1) / 2;				
			} 	
		}
		
		if (start.get() == row.at(k0)->getTime().get()) {
			if (row.at(k0)->equalByRequiredValue(x))
				return row.at(k0);
			else
				throw DuplicateKey("DuplicateKey exception in ", "TotalPowerTable");	
		}
		else if (start.get() == row.at(k1)->getTime().get()) {
			if (row.at(k1)->equalByRequiredValue(x))
				return row.at(k1);
			else
				throw  DuplicateKey("DuplicateKey exception in ", "TotalPowerTable");	
		}		
		
		row.insert(row.begin()+(k0+1), x);
		privateRows.push_back(x);
		x->isAdded(true);
		return x; 						
	}   	
    	
	
	

	
} // End namespace asdm
 
