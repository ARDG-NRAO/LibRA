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
 *
 * File Misc.cpp
 */

#include <alma/ASDM/Misc.h>
 
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

#include <algorithm> //required for std::swap
#include <iostream>
#include <sstream>

// string.h provides strcpy, strtok and strcat
#include <string.h>

#include <libxml/xmlmemory.h>
#include <libxml/debugXML.h>
#include <libxml/HTMLtree.h>
#include <libxml/xmlIO.h>
#include <libxml/parser.h>
#include <libxml/xinclude.h>
#include <libxml/catalog.h>

#include <libxslt/xslt.h>
#include <libxslt/xsltInternals.h>
#include <libxslt/transform.h>
#include <libxslt/xsltutils.h>

using namespace std;

extern int xmlLoadExtDtdDefaultValue;

#include <alma/ASDM/ASDMValuesParser.h>

namespace asdm {
  bool directoryExists(const char* dir) {
    DIR* dhandle = opendir(dir);

    if (dhandle != NULL) {
      closedir(dhandle);
      return true;
    }
    else {
      return false;
    }
  }

  bool createDirectory(const char* dir) { 
    return mkdir(dir, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH) == 0;
  }
	
  bool createPath(const char* path) {
    char localpath[256];
    strcpy(localpath, path);

    char directory[256];
    if (path[0] == '/') {
      strcpy(directory, "/");
    }
    else {
      strcpy(directory, "");
    }
    	
    char* pch = strtok(localpath, "/");
    while (pch != NULL) {
      strcat(directory, pch);
      strcat(directory, "/");
      if (!directoryExists(directory) && !createDirectory(directory)) {
	return false;
      }
      pch = strtok(NULL, "/");
    }
    return true;
  }
    
  void ByteSwap(unsigned char * b, int n) {
    int i = 0;
    int j = n-1;
    while (i<j) {
      std::swap(b[i], b[j]);
      i++, j--;
    }
  }

  const ByteOrder* ByteOrder::Little_Endian = new ByteOrder("Little_Endian");
  const ByteOrder* ByteOrder::Big_Endian = new ByteOrder("Big_Endian");
  const ByteOrder* ByteOrder::Machine_Endianity = ByteOrder::machineEndianity();

  ByteOrder::ByteOrder(const string& name):
    name_(name) {;}

  ByteOrder::~ByteOrder() {;}

  const ByteOrder* ByteOrder::machineEndianity() {
#if defined(__APPLE__)
    if (__DARWIN_BYTE_ORDER == __DARWIN_LITTLE_ENDIAN)
#else 
      if (__BYTE_ORDER == __LITTLE_ENDIAN)
#endif
	return Little_Endian;
      else
	return Big_Endian;
  }
  
  string ByteOrder::toString() const {
    return name_;
  }

  const ByteOrder* ByteOrder::fromString(const string &s) {
    if (s == "Little_Endian") return Little_Endian;
    if (s == "Big_Endian") return Big_Endian;
    return 0;
  }

  string uniqSlashes(const string & s) {
    string result;
    char c;
    bool inslash = false;
    size_t indexi=0;

    while (indexi < s.size()) {
      if ((c = s.at(indexi)) != '/') {
	inslash = false;
	result.push_back(c);
      }
      else
	if (inslash == false) {
	  result.push_back(c);
	  inslash = true;
	}
      indexi++;
    }
    return result;
  }

  ASDMUtilsException::ASDMUtilsException():message("ASDMUtilsException:") {;}

  ASDMUtilsException::ASDMUtilsException(const string & message): message("ASDMUtilsException:" + message) {;}   

  const string& ASDMUtilsException::getMessage() const { return message; }

#ifndef WITHOUT_BOOST
  ASDMUtils::DotXMLFilter::DotXMLFilter(vector<string>& filenames) {this->filenames = &filenames;}

  void ASDMUtils::DotXMLFilter::operator()(boost::filesystem::directory_entry& p) {
    if (!extension(p).compare(".xml")) {
      filenames->push_back(p.path().string());
    }
  }
#endif
 
  set<string>				ASDMUtils::evlaValidNames;
  set<string>				ASDMUtils::almaValidNames;
  map<ASDMUtils::Origin, string>	ASDMUtils::filenameOfV2V3xslTransform;
  map<string, string>			ASDMUtils::rootSubdir ;
  bool					ASDMUtils::initialized = ASDMUtils::initialize();

  bool ASDMUtils::initialize() {
    string evlaValidNames_a[] = {"EVLA"};
    evlaValidNames = set<string>(evlaValidNames_a, evlaValidNames_a + 1);

    string almaValidNames_a[] = {"ALMA", "AOS", "OSF", "IRAM_PDB"};
    almaValidNames = set<string>(almaValidNames_a, almaValidNames_a + 4);

    filenameOfV2V3xslTransform[ASDMUtils::ALMA]    = "sdm-v2v3-alma.xsl";
    filenameOfV2V3xslTransform[ASDMUtils::EVLA]    = "sdm-v2v3-evla.xsl";
    filenameOfV2V3xslTransform[ASDMUtils::UNKNOWN] = "";

    rootSubdir["INTROOT"]  = "config/";
    rootSubdir["ACSROOT"]  = "config/";

    return true;
  }

  string ASDMUtils::version(const string& asdmPath) {

    string result;

#ifndef WITHOUT_BOOST
    string ASDMPath = boost::algorithm::trim_copy(asdmPath);
    if (!boost::algorithm::ends_with(ASDMPath, "/")) ASDMPath+="/";
#else
    string ASDMPath = trim_copy(asdmPath);
    if (ASDMPath.back()!='/') ASDMPath+="/";
#endif
    ASDMPath += "ASDM.xml";

    // Does ASDMPath exist ?
#ifndef WITHOUT_BOOST
    if (!boost::filesystem::exists(boost::filesystem::path(ASDMPath))) {
#else
    if (!file_exists(ASDMPath)) {
#endif
      throw ASDMUtilsException("File not found '"+ASDMPath+"'.");
    }

    // Read and parse ASDM.xml
    xmlDocPtr ASDMDoc = xmlParseFile(ASDMPath.c_str());
  
    if (ASDMDoc == NULL ) {
      throw ASDMUtilsException("Error while parsing '"+ASDMPath+"'.");
    }
  
    /*
     * Can we find an attribute schemaVersion in the top level element ?
     */
    xmlNodePtr cur = xmlDocGetRootElement(ASDMDoc);
    xmlChar* version_ = xmlGetProp(cur, (const xmlChar *) "schemaVersion");

    // Yes ? then return its value.
    if (version_ != NULL) {
      result = string((char *) version_);
      xmlFree(version_);
      xmlFreeDoc(ASDMDoc);
      return result;
    }

    // Let's do some housecleaning
    xmlFreeDoc(ASDMDoc);

    // No ? then try another approach ... Can we find a dataUID element in the row elements of the Main table and in such a case
    // make the assumption that it's a v3 ASDM.
#ifndef WITHOUT_BOOST
    string MainPath = boost::algorithm::trim_copy(asdmPath);
    if (!boost::algorithm::ends_with(MainPath, "/")) MainPath+="/";
#else
    string MainPath = trim_copy(asdmPath);
    if (MainPath.back()!='/') MainPath+="/";
#endif
    MainPath += "Main.xml";    

    result = "UNKNOWN";
    // Does MainPath exist ?
#ifndef WITHOUT_BOOST
    if (boost::filesystem::exists(boost::filesystem::path(MainPath))) {
#else
    if (file_exists(MainPath)) {
#endif
      xmlDocPtr MainDoc =  xmlParseFile(MainPath.c_str());
  
      if (MainDoc == NULL ) {
	throw ASDMUtilsException("Error while parsing '"+MainPath+"'.");
      }

      // Here we make the reasonable assumption that there will be
      // row elements (at least one).
      //
      // Send an alarm though if no row element is found.
      xmlNodePtr cur = xmlDocGetRootElement(MainDoc)->xmlChildrenNode;
      int nRow = 0;
      while (cur != NULL) {
	if (!xmlStrcmp(cur->name, (const xmlChar*) "row")) {
	  nRow++;
	  if (hasChild (MainDoc, cur, (const xmlChar *) "dataUID")) {
	    result = "3";
	    break;
	  }

	  if (hasChild (MainDoc, cur, (const xmlChar *) "dataOid")) {
	    result = "2";
	    break;
	  }
	}
	cur = cur -> next;
      }

      xmlFreeDoc(MainDoc);      
      if (nRow == 0) {
	throw ASDMUtilsException("No 'row' elements present in '"+MainPath+"'.");
      }
    }
    
    return result;
  }

  bool ASDMUtils::hasChild(xmlDocPtr, // doc
			   xmlNodePtr node, const xmlChar* childName) {
    node = node->xmlChildrenNode;

    while (node != NULL) {
      if (!xmlStrcmp(node->name , childName))
	break; 
      node = node->next;
    }
    return (node != NULL);      
  }

  string ASDMUtils::parseRow(xmlDocPtr doc, xmlNodePtr node, const xmlChar* childName) {
    //
    // Consider the children of node (i.e. expectedly of "<row>...</row>")
    node = node->xmlChildrenNode;

    //
    // And look for childName.
    while (node != NULL) {
      if (!xmlStrcmp(node->name , childName)) {
	xmlChar* content = xmlNodeListGetString(doc, node->xmlChildrenNode, 1);
	string result((char *) content);
	xmlFree(content);
	return result;
      }
      node = node->next;
    }
    // If we are here , it's because we have not found childName as an element in node.
    throw ASDMUtilsException("Element '"+string((char *)childName)+"' not found.");
  }
 
  vector<string> ASDMUtils::telescopeNames(const string& asdmPath) {
    vector<string> result;

#ifndef WITHOUT_BOOST
    string execBlockPath = boost::algorithm::trim_copy(asdmPath);
    if (!boost::algorithm::ends_with(execBlockPath, "/")) execBlockPath+="/";
#else
    string execBlockPath = trim_copy(asdmPath);
    if (execBlockPath.back()!='/') execBlockPath+="/";
#endif
    execBlockPath += "ExecBlock.xml";

    // Does execBlockPath exist ?
#ifndef WITHOUT_BOOST
    if (!boost::filesystem::exists(boost::filesystem::path(execBlockPath))) {
#else
    if (!file_exists(execBlockPath)) {
#endif
      throw ASDMUtilsException("File not found '"+execBlockPath+"'.");
    }

    // Read and parse ExecBlock.xml
    xmlDocPtr execBlockDoc;
    xmlNodePtr cur;

    execBlockDoc = xmlParseFile(execBlockPath.c_str());
    if (execBlockDoc == NULL) {
      throw ASDMUtilsException("Error while parsing '"+execBlockPath+"'.");
    }

    cur = xmlDocGetRootElement(execBlockDoc)->xmlChildrenNode;
    while (cur != NULL) {
      if (!xmlStrcmp(cur->name, (const xmlChar*) "row")) {
#ifndef WITHOUT_BOOST
	result.push_back(boost::algorithm::trim_copy(parseRow(execBlockDoc, cur, (const xmlChar *) "telescopeName")));
#else
	result.push_back(trim_copy(parseRow(execBlockDoc, cur, (const xmlChar *) "telescopeName")));
#endif
      }
      cur = cur -> next;
    }
    xmlFreeDoc(execBlockDoc);

    return result;
  }

  ASDMUtils::Origin ASDMUtils::origin(const vector<string>& telescopeNames) {
    unsigned int numEVLA = 0;
    unsigned int numALMA = 0;
    for ( vector<string>::const_iterator iter = telescopeNames.begin(); iter != telescopeNames.end(); iter++ ) {
      if (evlaValidNames.find(*iter) != evlaValidNames.end())
	numEVLA++;
      else if (almaValidNames.find(*iter) != almaValidNames.end())
	numALMA++;
    }

    Origin autoOrigin = UNKNOWN;
    if (numEVLA == telescopeNames.size())
      autoOrigin = EVLA;
    else if (numALMA == telescopeNames.size())
      autoOrigin = ALMA;
    
    return autoOrigin;
  }

  vector<string> ASDMUtils::xmlFilenames ( const string & asdmPath  ) {
    vector<string> result;

#ifndef WITHOUT_BOOST
    boost::filesystem::path p(asdmPath);
    DotXMLFilter dotXMLFilter(result);
    std::for_each(boost::filesystem::directory_iterator(p), boost::filesystem::directory_iterator(), dotXMLFilter);
#else
    DIR *dir;
    if ((dir = opendir(asdmPath.c_str())) != NULL) {
      struct dirent *ent;
      string dirSep="/";
      if (asdmPath.back()=='/') dirSep="";

      while ((ent = readdir(dir)) != NULL) {
	string thisFile = ent->d_name;
	if (thisFile.size() > 4) {
	  if (thisFile.compare((thisFile.size()-4),4,".xml")==0) {
	    result.push_back(asdmPath+dirSep+thisFile);
	  }
	}
      }
      closedir(dir);
    } else {
      throw ASDMUtilsException("Could not open ASDM directory to retrieve xml files: "+asdmPath);
    }
#endif
    return result;
  }

  string ASDMUtils::pathToV2V3ALMAxslTransform() {return pathToxslTransform(filenameOfV2V3xslTransform[ASDMUtils::ALMA]);}
  string ASDMUtils::pathToV2V3EVLAxslTransform() {return pathToxslTransform(filenameOfV2V3xslTransform[ASDMUtils::EVLA]);}
  string ASDMUtils::nameOfV2V3xslTransform(ASDMUtils::Origin origin) {
    return filenameOfV2V3xslTransform[origin];
  }

  ASDMParseOptions::ASDMParseOptions() {
    origin_		= ASDMUtils::UNKNOWN;
    detectOrigin_       = true;
    version_    	= "UNKNOWN";
    detectVersion_      = true;
    loadTablesOnDemand_ = false;
    checkRowUniqueness_ = true;
  }

  ASDMParseOptions::ASDMParseOptions(const ASDMParseOptions& x) {
    origin_				       = x.origin_;
    detectOrigin_			       = x.detectOrigin_;
    version_				       = x.version_;
    detectVersion_			       = x.detectVersion_;
    loadTablesOnDemand_			       = x.loadTablesOnDemand_;
    checkRowUniqueness_                        = x.checkRowUniqueness_;
  }

  ASDMParseOptions::~ASDMParseOptions() {;}

  ASDMParseOptions& ASDMParseOptions::operator = (const ASDMParseOptions& rhs) {
    origin_				       = rhs.origin_;
    detectOrigin_			       = rhs.detectOrigin_;
    version_				       = rhs.version_;
    detectVersion_			       = rhs.detectVersion_;
    loadTablesOnDemand_			       = rhs.loadTablesOnDemand_;
    checkRowUniqueness_                        = rhs.checkRowUniqueness_;
    return *this;
  }
  ASDMParseOptions& ASDMParseOptions::asALMA() { origin_ = ASDMUtils::ALMA; detectOrigin_ = false; return *this; }
  ASDMParseOptions& ASDMParseOptions::asIRAM_PDB() { origin_ = ASDMUtils::ALMA; detectOrigin_ = false; return *this; }
  ASDMParseOptions& ASDMParseOptions::asEVLA() { origin_ = ASDMUtils::EVLA; detectOrigin_ = false; return *this; }
  ASDMParseOptions& ASDMParseOptions::asV2() { version_ = "2"; detectVersion_ = false; return *this; }
  ASDMParseOptions& ASDMParseOptions::asV3() { version_ = "3"; detectVersion_ = false; return *this; }
  ASDMParseOptions& ASDMParseOptions::loadTablesOnDemand(bool b) { loadTablesOnDemand_ = b;  return *this; }
  ASDMParseOptions& ASDMParseOptions::checkRowUniqueness(bool b) { checkRowUniqueness_ = b;  return *this; }
  string ASDMParseOptions::toString() const {
    ostringstream oss;
    oss << *this;
    return oss.str();
  }

  XSLTransformer::XSLTransformer() : cur(NULL) {
    xmlSubstituteEntitiesDefault(1);
    xmlLoadExtDtdDefaultValue = 1;
  }

  XSLTransformer::XSLTransformer(const string& xsltPath) {
    if (getenv("ASDM_DEBUG")) {
	cout << "XSLTransformer::XSLTransformer(const string& xsltPath) called " << endl;
	cout << "About parse the style sheet contained in " << xsltPath << endl;
    }

    xmlSubstituteEntitiesDefault(1);
    xmlLoadExtDtdDefaultValue = 1;
    
    cur = xsltParseStylesheetFile((const xmlChar*) xsltPath.c_str());
    if (cur == NULL)
      throw XSLTransformerException("Failed to  parse the XSL stylecheet contained in '" + xsltPath + "'.");
  }

  void XSLTransformer::setTransformation(const string& xsltPath) {
    if (getenv("ASDM_DEBUG")) 
      cout << "XSLTransformer::setTransformation(const string& xsltPath) called on '" << xsltPath << "'." << endl;
    
    if (cur) {
      xsltFreeStylesheet(cur);
      cur = NULL;
    }

    // cout << "About parse the style sheet contained in " << xsltPath << endl;
      
    cur = xsltParseStylesheetFile((const xmlChar*) xsltPath.c_str());
    if (cur == NULL)
      throw XSLTransformerException("Failed to parse the XSL stylecheet contained in '" + xsltPath + "'." );
  }


  XSLTransformer::~XSLTransformer() {
    // cout << "XSLTransformer::~XSLTransformer() called" << endl;
    if (cur) {
      xsltFreeStylesheet(cur);
      cur = NULL;
    }
  } 

  string XSLTransformer::operator()(const string& xmlPath){
    xmlDocPtr doc = NULL, res = NULL;
    //xsltStylesheetPtr cur;

    xmlChar* docTxtPtr = NULL;
    int docTxtLen = 0;
    
    if (getenv("ASDM_DEBUG")) cout << "About to read and parse " << xmlPath << endl;
    doc = xmlParseFile(xmlPath.c_str());
    if (doc == NULL) {
      throw XSLTransformerException("Could not parse the XML file '" + xmlPath + "'." );
    }

    if (!cur) {
      xmlDocDumpFormatMemory(doc, &docTxtPtr, &docTxtLen, 1);
    }
    else {
      res = xsltApplyStylesheet(cur, doc, NULL);
      if ( res == NULL ) {
	throw XSLTransformerException("Failed to apply the XSLT tranformation to the XML document contained in '" + xmlPath + "'.");
      }
      int status = xsltSaveResultToString(&docTxtPtr,
					  &docTxtLen,
					  res,
					  cur);
      if (status == -1) 
	throw XSLTransformerException("Could not dump the result of the XSL transformation into memory.");
    }

    if (getenv("ASDM_DEBUG")) 
      cout << "Making a string from the result of the XSL transformation" << endl;
    string docXML((char *) docTxtPtr, docTxtLen);
    // cout << "docXML = " << docXML << endl;

    xmlFree(docTxtPtr);
    if (res) xmlFreeDoc(res);
    xmlFreeDoc(doc);
    
    // cout << "All resources unneeded freed" << endl;
    return docXML;
  } 

  std::ostream& operator<<(std::ostream& output, const ASDMParseOptions& p) {
    string s;
    switch (p.origin_) {
    case ASDMUtils::UNKNOWN:
      s = "UNKNOWN";
      break;
    case ASDMUtils::ALMA:
      s = "ALMA";
      break;
    case ASDMUtils::EVLA:
      s = "EVLA";
      break;
    }
    output << "Origin=" << s << ",Version=" << p.version_ << ",LoadTablesOnDemand=" << p.loadTablesOnDemand_ << ",CheckRowUniqueness=" << p.checkRowUniqueness_;
    return output;  // for multiple << operators.
  }

  CharComparator::CharComparator(std::ifstream * is_p, off_t limit):is_p(is_p), limit(limit){asdmDebug_p = getenv("ASDM_DEBUG");}

  bool CharComparator::operator() (char cl, char cr) {
    if (asdmDebug_p) cout << "Entering CharComparator::operator()" << endl;
    if (is_p && is_p->tellg() > limit) 
      return true;
    else 
      return toupper(cl) == cr;
    if (asdmDebug_p) cout << "Exiting CharComparator::operator()" << endl;
  }

  CharCompAccumulator::CharCompAccumulator(std::string* accumulator_p, std::ifstream * is_p, off_t limit): accumulator_p(accumulator_p),
													   is_p(is_p),
													   limit(limit) {nEqualChars = 0; asdmDebug_p = getenv("ASDM_DEBUG");}
  bool CharCompAccumulator::operator()(char cl, char cr) {
    if (asdmDebug_p) cout << "Entering CharCompAccumulator::operator()" << endl;
    bool result = false;
    // Are we beyond the limit ?
    if (is_p && is_p->tellg() > limit) 
      result = true;      // Yes
    else {                // No
      if (toupper(cl) == toupper(cr)) {
	result = true;
	nEqualChars++;
      }
      else {
	if (nEqualChars > 0) {
	  accumulator_p->erase(accumulator_p->end() - nEqualChars + 1, accumulator_p->end());
	  nEqualChars = 0;
	}
	result = false;
      }
      accumulator_p->push_back(cl);
    }
    if (asdmDebug_p) cout << "Exiting CharCompAccumulator::operator()" << endl;
    return result;
  }  
  
  istringstream ASDMValuesParser::iss;
  ostringstream ASDMValuesParser::oss;

} // end namespace asdm
 
 
