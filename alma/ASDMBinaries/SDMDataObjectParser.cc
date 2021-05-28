#include <alma/ASDMBinaries/SDMDataObjectParser.h>
#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>
#include <vector>
#include <iterator>

using namespace AtmPhaseCorrectionMod;
using namespace AxisNameMod;
using namespace BasebandNameMod;
using namespace CorrelationModeMod;
using namespace CorrelatorTypeMod;
using namespace NetSidebandMod;
using namespace PrimitiveDataTypeMod;
using namespace ProcessorTypeMod;
using namespace SpectralResolutionTypeMod;
using namespace StokesParameterMod;

using namespace std;

namespace asdmbinaries {

  // Names of XML elements/attributes in an sdmDataHeader.

#ifndef WITHOUT_BOOST
  const boost::regex  HeaderParser::PROJECTPATH3("([0-9]+)/([0-9]+)/([0-9]+)/");
#else
  const std::regex  HeaderParser::PROJECTPATH3("([0-9]+)/([0-9]+)/([0-9]+)/");
#endif
  const string HeaderParser::SDMDATAHEADER      = "sdmDataHeader";
  const string HeaderParser::SCHEMAVERSION      = "schemaVersion";
  const string HeaderParser::BYTEORDER          = "byteOrder";
  const string HeaderParser::PROJECTPATH        = "projectPath";
  const string HeaderParser::STARTTIME          = "startTime";
  const string HeaderParser::DATAOID            = "dataOID";
  const string HeaderParser::XLINKHREF          = "href";
  const string HeaderParser::XLINKTITLE         = "title";
  const string HeaderParser::DIMENSIONALITY     = "dimensionality";
  const string HeaderParser::NUMTIME            = "numTime";
  const string HeaderParser::EXECBLOCK          = "execBlock";
  const string HeaderParser::EXECBLOCKNUM       = "execBlockNum";
  const string HeaderParser::SCANNUM            = "scanNum";
  const string HeaderParser::SUBSCANNUM         = "subscanNum"; 
  const string HeaderParser::NUMANTENNA         = "numAntenna";
  const string HeaderParser::CORRELATIONMODE    = "correlationMode";
  const string HeaderParser::SPECTRALRESOLUTION = "spectralResolution";
  const string HeaderParser::PROCESSORTYPE      = "processorType";
  const string HeaderParser::DATASTRUCT         = "dataStruct";
  const string HeaderParser::APC                = "apc";
  const string HeaderParser::REF                = "ref";  
  const string HeaderParser::BASEBAND           = "baseband";
  const string HeaderParser::NAME               = "name";
  const string HeaderParser::SPECTRALWINDOW     = "spectralWindow";
  const string HeaderParser::SW                 = "sw";
  const string HeaderParser::SWBB               = "swbb";
  const string HeaderParser::CROSSPOLPRODUCTS   = "crossPolProducts";
  const string HeaderParser::SDPOLPRODUCTS      = "sdPolProducts"; 
  const string HeaderParser::SCALEFACTOR        = "scaleFactor"; 
  const string HeaderParser::NUMSPECTRALPOINT   = "numSpectralPoint";
  const string HeaderParser::NUMBIN             = "numBin";
  const string HeaderParser::SIDEBAND           = "sideband";
  const string HeaderParser::IMAGE              = "image";
  const string HeaderParser::FLAGS              = "flags";
  const string HeaderParser::ACTUALTIMES        = "actualTimes";
  const string HeaderParser::ACTUALDURATIONS    = "actualDurations";
  const string HeaderParser::ZEROLAGS           = "zeroLags";
  const string HeaderParser::CORRELATORTYPE     = "correlatorType";
  const string HeaderParser::CROSSDATA          = "crossData";
  const string HeaderParser::AUTODATA           = "autoData";
  const string HeaderParser::NORMALIZED         = "normalized";
  const string HeaderParser::SIZE               = "size";
  const string HeaderParser::AXES               = "axes";
  const string HeaderParser::TYPE               = "type";


  // Names of XML elements/attributes in an sdmSubsetDataHeader with dimensionality==1 (Correlator)

#ifndef WITHOUT_BOOST
  const boost::regex  SDMDataObjectParser::PROJECTPATH3("([0-9]+)/([0-9]+)/([0-9]+)/"); 
  const boost::regex  SDMDataObjectParser::PROJECTPATH4("([0-9]+)/([0-9]+)/([0-9]+)/([0-9]+)/");
  const boost::regex  SDMDataObjectParser::PROJECTPATH5("([0-9]+)/([0-9]+)/([0-9]+)/([0-9]+)/([0-9]+)/");
#else
  const std::regex  SDMDataObjectParser::PROJECTPATH3("([0-9]+)/([0-9]+)/([0-9]+)/"); 
  const std::regex  SDMDataObjectParser::PROJECTPATH4("([0-9]+)/([0-9]+)/([0-9]+)/([0-9]+)/");
  const std::regex  SDMDataObjectParser::PROJECTPATH5("([0-9]+)/([0-9]+)/([0-9]+)/([0-9]+)/([0-9]+)/");
#endif


  const string CorrSubsetHeaderParser::SDMDATASUBSETHEADER = "sdmDataSubsetHeader";
  const string CorrSubsetHeaderParser::PROJECTPATH         = "projectPath";
  const string CorrSubsetHeaderParser::SCHEDULEPERIODTIME  = "schedulePeriodTime";
  const string CorrSubsetHeaderParser::TIME                = "time";
  const string CorrSubsetHeaderParser::INTERVAL            = "interval";
  const string CorrSubsetHeaderParser::DATASTRUCT          = "dataStruct";
  const string CorrSubsetHeaderParser::REF                 = "ref";
  const string CorrSubsetHeaderParser::ABORTOBSERVATION    = "abortObservation";
  const string CorrSubsetHeaderParser::ABORTTIME           = "stopTime";
  const string CorrSubsetHeaderParser::ABORTREASON         = "abortReason";
  const string CorrSubsetHeaderParser::XLINKHREF           = "href";
  const string CorrSubsetHeaderParser::DATAREF             = "dataRef";
  const string CorrSubsetHeaderParser::FLAGSREF            = "flags";
  const string CorrSubsetHeaderParser::ACTUALTIMESREF      = "actualTimes";
  const string CorrSubsetHeaderParser::ACTUALDURATIONSREF  = "actualDurations";
  const string CorrSubsetHeaderParser::ZEROLAGSREF         = "zeroLags";
  const string CorrSubsetHeaderParser::CROSSDATAREF        = "crossData";
  const string CorrSubsetHeaderParser::AUTODATAREF         = "autoData";
  const string CorrSubsetHeaderParser::TYPE                = "type";

  // Names of XML elements/attributes in an sdmSubsetDataHeader with dimensionality==0 (TP)
#ifndef WITHOUT_BOOST
  const boost::regex  TPSubsetHeaderParser::PROJECTPATH3("([0-9]+)/([0-9]+)/([0-9]+)/"); 
#else
  const std::regex  TPSubsetHeaderParser::PROJECTPATH3("([0-9]+)/([0-9]+)/([0-9]+)/");
#endif
  const string TPSubsetHeaderParser::SDMDATASUBSETHEADER = "sdmDataSubsetHeader";
  const string TPSubsetHeaderParser::PROJECTPATH         = "projectPath";
  const string TPSubsetHeaderParser::SCHEDULEPERIODTIME  = "schedulePeriodTime";
  const string TPSubsetHeaderParser::TIME                = "time";
  const string TPSubsetHeaderParser::INTERVAL            = "interval";
  const string TPSubsetHeaderParser::DATASTRUCT          = "dataStruct";
  const string TPSubsetHeaderParser::REF                 = "ref";
  const string TPSubsetHeaderParser::DATAREF             = "dataRef";  
  const string TPSubsetHeaderParser::XLINKHREF           = "href";
  const string TPSubsetHeaderParser::FLAGSREF            = "flags";
  const string TPSubsetHeaderParser::ACTUALTIMESREF      = "actualTimes";
  const string TPSubsetHeaderParser::ACTUALDURATIONSREF  = "actualDurations";
  const string TPSubsetHeaderParser::AUTODATAREF         = "autoData";

  // HeaderParser methods.
  HeaderParser::HeaderParser(){
    doc = NULL;
  }

  HeaderParser::~HeaderParser() {
    if (doc != NULL) xmlFreeDoc(doc);
  }

  void HeaderParser::parseFile(const string& filename, SDMDataObject& sdmDataObject){
    if (doc != NULL) xmlFreeDoc(doc);
    doc = xmlReadFile(filename.c_str(), NULL, XML_PARSE_NOBLANKS);
    if (doc == NULL) {
      throw SDMDataObjectParserException("The file '"+filename+"' could not be transformed into a DOM structure");
    }

    xmlNode* root_element = xmlDocGetRootElement(doc);
    parseSDMDataHeader(root_element, sdmDataObject);
  }

  void HeaderParser::parseMemory(const string& buffer, SDMDataObject& sdmDataObject) {
    if (doc != NULL) xmlFreeDoc(doc);
    doc = xmlReadMemory(buffer.data(), buffer.size(), "SDMDataHeader.xml", NULL, XML_PARSE_NOBLANKS );
    if (doc == NULL) {
      throw SDMDataObjectParserException("The buffer containing the XML document could not be transformed into a DOM structure");
    }    

    xmlNode* root_element = xmlDocGetRootElement(doc);
    parseSDMDataHeader(root_element, sdmDataObject);
  }

  void HeaderParser::reset() {
    if (doc) 
      xmlFreeDoc(doc);

    doc = NULL;
  }

  void HeaderParser::parseSDMDataHeader(xmlNode* a_node,  SDMDataObject& sdmDataObject){
    //cout << "Entering parseSDMDataHeader" << endl;
    // Look up for the <sdmDataHeader ... element.
    SDMDataObjectParser::isElement(a_node, HeaderParser::SDMDATAHEADER);

    // And parse some of its attributes.
    sdmDataObject.schemaVersion_ = SDMDataObjectParser::parseIntAttr(a_node, HeaderParser::SCHEMAVERSION);
    sdmDataObject.byteOrder_ = SDMDataObjectParser::parseByteOrderAttr(a_node, HeaderParser::BYTEORDER);


    // Look up for its projectPath attribute
    // and determine execBlockNum, scanNum and subscanNum from its projectPath attribute.
    vector<unsigned int> v = SDMDataObjectParser::parseProjectPath(a_node, 3);
    sdmDataObject.execBlockNum_ = v.at(0);
    sdmDataObject.scanNum_      = v.at(1);
    sdmDataObject.subscanNum_   = v.at(2);

    // Traverse the children.
    xmlNode* child = a_node->children;
    
    // Look up for the <startTime... child
    sdmDataObject.startTime(parseStartTime(child));

    // Look up for the <dataOID... child
    child = child->next;
    sdmDataObject.dataOID(parseDataOID(child));

    // ...and its title attribute
    sdmDataObject.title(SDMDataObjectParser::parseStringAttr(child, HeaderParser::XLINKTITLE));

    // Look up for the <dimensionality...
    child = child->next;
    unsigned int dimensionality = parseDimensionality(child);
    sdmDataObject.dimensionality(dimensionality);

    if ( dimensionality == 0 ) {
      // Look up for numTime... only if dimensionality == 0
      sdmDataObject.numTime((unsigned int) parseNumTime(child) );
    }
    
    // Look up for the <execBlock... child
    child = child->next;
    parseExecBlock(child, sdmDataObject);
    
    // Look up for the <numAntenna... child
    child = child->next;
    int numAntenna = parseNumAntenna(child); //cout << child->name << "=" << numAntenna << endl;
    sdmDataObject.numAntenna((unsigned int) numAntenna);

    // Look up for the <correlationMode> ... child
    child = child->next;
    parseCorrelationMode(child, sdmDataObject);    

    // Look up for the <spectralResolution> ... child
    child = child->next;
    parseSpectralResolution(child, sdmDataObject);    

    // Look up for child <processorType> ... child
    child = child->next;
    parseProcessorType(child, sdmDataObject);

    // Look up for the <dataStruct> ... child
    child = child->next;
    parseDataStruct(child, sdmDataObject);

    //cout << "Exiting parseSDMDataHeader" << endl;    
  }

//   void HeaderParser::parseProjectPath(xmlNode* a_node, SDMDataObject& sdmDataObject) {
//     string projectPath = SDMDataObjectParser::parseStringAttr(a_node, HeaderParser::PROJECTPATH);
    
//     boost::cmatch what;
    
//     if (boost::regex_match(projectPath.c_str(), what,PROJECTPATH3) && what[0].matched) {
//       sdmDataObject.execBlockNum(::atoi(what[1].first));
//       sdmDataObject.scanNum(::atoi(what[2].first));
//       sdmDataObject.subscanNum(::atoi(what[3].first));
//     }
//     else
//       throw SDMDataObjectParserException("HeaderParser::parseProjectPath: Invalid string for projectPath '" + projectPath + "'");
//   }
  
  long long HeaderParser::parseStartTime(xmlNode* a_node){
    SDMDataObjectParser::isElement(a_node, HeaderParser::STARTTIME);
    return SDMDataObjectParser::parseLongLong(a_node->children);
  }
  
  string HeaderParser::parseDataOID(xmlNode* a_node) {
    SDMDataObjectParser::isElement(a_node, HeaderParser::DATAOID);

    // Look for attribute
    string dataOID = SDMDataObjectParser::parseStringAttr(a_node, HeaderParser::XLINKHREF);
    return dataOID;
  } 

  int HeaderParser::parseDimensionality(xmlNode* a_node) {
    //cout << "Entering parseDimensionality with " << a_node->name << endl;
    if (SDMDataObjectParser::testElement(a_node, HeaderParser::DIMENSIONALITY)) {
      return SDMDataObjectParser::parseInt(a_node->children);
    }
    else
      return 0;
  } 

  int HeaderParser::parseNumTime(xmlNode* a_node) {
    SDMDataObjectParser::isElement(a_node, HeaderParser::NUMTIME);
    return SDMDataObjectParser::parseInt(a_node->children);
  } 
  
  void HeaderParser::parseExecBlock(xmlNode* a_node, SDMDataObject& sdmDataObject){
    //cout << "Entering parseExecBlock with " << a_node << endl;

    SDMDataObjectParser::isElement(a_node, HeaderParser::EXECBLOCK);

    // Look up for the execBlockUID attribute.
    sdmDataObject.execBlockUID(SDMDataObjectParser::parseStringAttr(a_node, HeaderParser::XLINKHREF));
  }

  int HeaderParser::parseExecBlockNum(xmlNode* a_node){
    SDMDataObjectParser::isElement(a_node, HeaderParser::EXECBLOCKNUM);
    return SDMDataObjectParser::parseInt(a_node->children);
  }

  int HeaderParser::parseScanNum(xmlNode* a_node){
    SDMDataObjectParser::isElement(a_node, HeaderParser::SCANNUM);
    return SDMDataObjectParser::parseInt(a_node->children);
  }

  int HeaderParser::parseSubscanNum(xmlNode* a_node){
    SDMDataObjectParser::isElement(a_node, HeaderParser::SUBSCANNUM);
    return SDMDataObjectParser::parseInt(a_node->children);
  }

  int HeaderParser::parseNumAntenna(xmlNode* a_node){
    SDMDataObjectParser::isElement(a_node, HeaderParser::NUMANTENNA);
    return SDMDataObjectParser::parseInt(a_node->children);
  }

  void HeaderParser::parseCorrelationMode(xmlNode* a_node, SDMDataObject& sdmDataObject) {
    SDMDataObjectParser::isElement(a_node, HeaderParser::CORRELATIONMODE);
    sdmDataObject.correlationMode_ = SDMDataObjectParser::parseLiteral<CorrelationMode, CCorrelationMode>(a_node->children);
  }

  void HeaderParser::parseSpectralResolution(xmlNode* a_node, SDMDataObject& sdmDataObject) {
    SDMDataObjectParser::isElement(a_node, HeaderParser::SPECTRALRESOLUTION);
    sdmDataObject.spectralResolutionType_ = SDMDataObjectParser::parseLiteral<SpectralResolutionType, CSpectralResolutionType>(a_node->children);
  }

  void HeaderParser::parseProcessorType(xmlNode* a_node, SDMDataObject& sdmDataObject) {
    SDMDataObjectParser::isElement(a_node, HeaderParser::PROCESSORTYPE);
    sdmDataObject.processorType_ = SDMDataObjectParser::parseLiteral<ProcessorType, CProcessorType>(a_node->children);
  }

  void HeaderParser::parseDataStruct(xmlNode* a_node, SDMDataObject& sdmDataObject){
    //cout << "Entering parseDataStruct with " << a_node->name << endl;
    SDMDataObjectParser::isElement(a_node, HeaderParser::DATASTRUCT);

    SDMDataObject::DataStruct dataStruct;
    
    switch (sdmDataObject.correlationMode()) {
    case AUTO_ONLY:
      break;
      
    case CROSS_ONLY:
    case CROSS_AND_AUTO:
      dataStruct.apc_ = SDMDataObjectParser::parseStringsAttr<AtmPhaseCorrection, CAtmPhaseCorrection>(a_node, HeaderParser::APC);
      break;      
    }
            
    // Traverse the children 

    // BaseBands...
    xmlNode* child = a_node->children;
    vector <SDMDataObject::Baseband> basebands;
    while (SDMDataObjectParser::testElement(child, HeaderParser::BASEBAND)) {
      basebands.push_back(parseBaseband(child, sdmDataObject ));
      child = child->next;
    }

    dataStruct.basebands_ = (basebands);

    // Now that all the spectral windows are read set the associations between spectral windows.
    // We have recorded the id and image attributes when they are present in private strings in each spectralWindow.
    //
    
    // I want a map to associate a string of the form "<int>_<int>" to a string of the form "<int>_<int>"
    // and I define an entry in this map for each spectralWindow with an strSw_ non empty, the value
    // associated is of the form <int>_<int> where the two integers are the coordinates 
    // (baseband index, spectralWindow index in the baseband)
    //
    map<string, string> id2int_int;
    ostringstream oss;

    string key, value;
    for (unsigned int ibb = 0; ibb < dataStruct.basebands_.size(); ibb++) {
      vector<SDMDataObject::SpectralWindow>& spws = dataStruct.basebands_.at(ibb).spectralWindows_;
      for (unsigned int ispw = 0; ispw < spws.size(); ispw++) {

	oss.str("");
	oss << ibb << " " << ispw;
	value = oss.str();

	oss.str("");
	oss << ibb << " " << spws.at(ispw).strSw();
	key = oss.str();

	id2int_int[key] = value;
	
      }
    }


    // Now re scan all the spectralWindows and look for the ones with an strImage_ non empty.
    // and define the associations.
    //
    istringstream iss;
    map<string, string>::iterator iter;
    int ibbImage, ispwImage;
    for (unsigned int ibb = 0; ibb < dataStruct.basebands_.size(); ibb++) {
      vector<SDMDataObject::SpectralWindow>& spws = dataStruct.basebands_.at(ibb).spectralWindows_;
      for (unsigned int ispw = 0; ispw < spws.size(); ispw++) {
	string image = spws.at(ispw).strImage();
	if (image.size() > 0) {
	  oss.str("");
	  oss << ibb << " " << image;
	  key = oss.str();
	  if ((iter = id2int_int.find(key)) != id2int_int.end()) {
	    iss.str(iter->second);
	    iss >> ibbImage;
	    iss >> ispwImage;
	    dataStruct.imageSPW(ibb, ispw, ispwImage);
	  }
	  else {
	    oss.str("");
	    oss << "In baseband #" << ibb << " the spectral window #" << ispw << " refers to non defined image ('" << image << "')";
	    throw SDMDataObjectParserException(oss.str());
	  }
	}
      }
    }

    // Metadata attachments.

    // flags (optional)
    if (SDMDataObjectParser::testElement(child, HeaderParser::FLAGS)) {
      dataStruct.flags_ = (parseBinaryPart(child, HeaderParser::FLAGS));
      child = child->next;
    }
    
    // actualTimes (optional)
    if (SDMDataObjectParser::testElement(child, HeaderParser::ACTUALTIMES)) {
      dataStruct.actualTimes_ = (parseBinaryPart(child, HeaderParser::ACTUALTIMES));
      child = child->next;
    }
    
    // actualDurations (optional)
    if (SDMDataObjectParser::testElement(child, HeaderParser::ACTUALDURATIONS)) {
      dataStruct.actualDurations_ = (parseBinaryPart(child, HeaderParser::ACTUALDURATIONS));
      child = child->next;
    }

    // Binary attachments elements...
    switch (sdmDataObject.correlationMode()) {
    case CROSS_ONLY :
      dataStruct.crossData_ = (parseBinaryPart(child, HeaderParser::CROSSDATA));
      child = child->next;
      break;

    case AUTO_ONLY : 
      dataStruct.autoData_ = (parseAutoDataBinaryPart(child, HeaderParser::AUTODATA));
      child = child->next;
      break;

    case CROSS_AND_AUTO :
      dataStruct.crossData_ = (parseBinaryPart(child, HeaderParser::CROSSDATA));
      child = child->next;
      dataStruct.autoData_ = (parseAutoDataBinaryPart(child, HeaderParser::AUTODATA));
      child = child->next;
      break;
    }

    /*
    // zeroLags (mandatory in FULL_RESOLUTION)
    if (sdmDataObject.spectralResolutionType() == FULL_RESOLUTION) {
      dataStruct.zeroLags_ = (parseBinaryPart(child, HeaderParser::ZEROLAGS));
      child = child->next;
    }
    // (and optional in CHANNEL_AVERAGE)
    else if (sdmDataObject.spectralResolutionType() == CHANNEL_AVERAGE) {
      if (SDMDataObjectParser::testElement(child, HeaderParser::ZEROLAGS)) {
	dataStruct.zeroLags_ = (parseBinaryPart(child, HeaderParser::ZEROLAGS));
	child = child->next;
      }
    }
    */
    
    // zeroLags are allowed only with a non FX correlator.
    if (SDMDataObjectParser::testElement(child, HeaderParser::ZEROLAGS)) {
      // Reject zeroLags if the context does not allow them	  
      if (sdmDataObject.processorType_ != CORRELATOR) 
	throw SDMDataObjectParserException("zeroLags are not expected with the declared processor type ('" +
					   CProcessorType::name(sdmDataObject.processorType_) + "')");
      
      
      dataStruct.zeroLags_ = (parseZeroLagsBinaryPart(child, HeaderParser::ZEROLAGS));
      
      // Reject zeroLags if the context does not allow them ... again

      if (dataStruct.zeroLags_.correlatorType_ == FX)
	throw SDMDataObjectParserException ("zeroLags are not expected with the declared correlator type ('" +
					    CCorrelatorType::name(dataStruct.zeroLags_.correlatorType_) + "')");
      
      child = child->next;
    }

    sdmDataObject.dataStruct_ = (dataStruct);
  }
  
  SDMDataObject::Baseband HeaderParser::parseBaseband(xmlNode* a_node, SDMDataObject& sdmDataObject){
    SDMDataObject::Baseband bb;
    SDMDataObjectParser::isElement(a_node,  HeaderParser::BASEBAND);
    
    bb.name_ = (SDMDataObjectParser::parseStringAttr<BasebandName, CBasebandName>(a_node, HeaderParser::NAME));

    // Traverse the children (spectralWindow).
    xmlNode* child = a_node->children;
    vector<SDMDataObject::SpectralWindow> spw;
    parseSpectralWindow(child, sdmDataObject, spw);
    bb.spectralWindows(spw);

    return bb;
  }

  void HeaderParser::parseSpectralWindow(xmlNode* a_node, SDMDataObject& sdmDataObject , vector<SDMDataObject::SpectralWindow>& spectralWindow){
    for (xmlNode* cur_node = a_node; cur_node; cur_node = cur_node->next) { 
      SDMDataObjectParser::isElement(a_node, HeaderParser::SPECTRALWINDOW);
 
     //Look for attributes
      vector<StokesParameter> crossPolProducts;
      vector<StokesParameter> sdPolProducts;
      float scaleFactor;
      int numSpectralPoint;
      int numBin;
      NetSideband sideband;

      SDMDataObject::SpectralWindow spw;

      
      string dummy = SDMDataObjectParser::parseStringAttr(cur_node, HeaderParser::SWBB);

      switch (sdmDataObject.correlationMode()) {

      case CROSS_ONLY:
	crossPolProducts = SDMDataObjectParser::parseStringsAttr<StokesParameter, CStokesParameter>(cur_node, HeaderParser::CROSSPOLPRODUCTS ); 
	scaleFactor = SDMDataObjectParser::parseFloatAttr(cur_node, HeaderParser::SCALEFACTOR ); 
	numSpectralPoint = SDMDataObjectParser::parseIntAttr(cur_node, HeaderParser::NUMSPECTRALPOINT ); 
	numBin = SDMDataObjectParser::parseIntAttr(cur_node, HeaderParser::NUMBIN ); 
	sideband = SDMDataObjectParser::parseStringAttr<NetSideband, CNetSideband>(cur_node, HeaderParser::SIDEBAND);
	spw = SDMDataObject::SpectralWindow(crossPolProducts,
					    scaleFactor,
					    (unsigned int)numSpectralPoint,
					    (unsigned int)numBin,
					    sideband);
	break;
	
      case AUTO_ONLY:
	sdPolProducts =  SDMDataObjectParser::parseStringsAttr<StokesParameter, CStokesParameter>(cur_node, HeaderParser::SDPOLPRODUCTS ); 
	numSpectralPoint = SDMDataObjectParser::parseIntAttr(cur_node, HeaderParser::NUMSPECTRALPOINT );
	numBin  = SDMDataObjectParser::parseIntAttr(cur_node, HeaderParser::NUMBIN ); 
	sideband = SDMDataObjectParser::parseStringAttr<NetSideband, CNetSideband>(cur_node, HeaderParser::SIDEBAND);
	spw = SDMDataObject::SpectralWindow(sdPolProducts,
					    (unsigned int)numSpectralPoint,
					    (unsigned int)numBin,
					    sideband);
	break;
	
      case CROSS_AND_AUTO:
	crossPolProducts = SDMDataObjectParser::parseStringsAttr<StokesParameter, CStokesParameter>(cur_node, HeaderParser::CROSSPOLPRODUCTS ); 
	sdPolProducts =  SDMDataObjectParser::parseStringsAttr<StokesParameter, CStokesParameter>(cur_node, HeaderParser::SDPOLPRODUCTS ); 
	scaleFactor = SDMDataObjectParser::parseFloatAttr(cur_node, HeaderParser::SCALEFACTOR ); 
	numSpectralPoint = SDMDataObjectParser::parseIntAttr(cur_node, HeaderParser::NUMSPECTRALPOINT ); 
	numBin = SDMDataObjectParser::parseIntAttr(cur_node, HeaderParser::NUMBIN );
	sideband = SDMDataObjectParser::parseStringAttr<NetSideband, CNetSideband>(cur_node, HeaderParser::SIDEBAND);
	spw = SDMDataObject::SpectralWindow(crossPolProducts,
					    sdPolProducts,
					    scaleFactor,
					    (unsigned int)numSpectralPoint,
					    (unsigned int)numBin,
					    sideband);
	break;
      }
	
      spw.strSw(SDMDataObjectParser::parseStringAttr(cur_node, HeaderParser::SW));
      if (SDMDataObjectParser::hasAttr(cur_node, HeaderParser::IMAGE))
	spw.strImage(SDMDataObjectParser::parseStringAttr(cur_node, HeaderParser::IMAGE));
      
      spectralWindow.push_back(spw);
      
    }
  }
  
  SDMDataObject::BinaryPart HeaderParser::parseBinaryPart(xmlNode* a_node, const string& attachmentName) {
    SDMDataObjectParser::isElement(a_node, attachmentName);

    return SDMDataObject::BinaryPart(SDMDataObjectParser::parseIntAttr(a_node, HeaderParser::SIZE),
				     SDMDataObjectParser::parseStringsAttr<AxisName, CAxisName>(a_node, HeaderParser::AXES));
  }

  SDMDataObject::AutoDataBinaryPart HeaderParser::parseAutoDataBinaryPart(xmlNode* a_node, const string& attachmentName) {
    SDMDataObjectParser::isElement(a_node, attachmentName);

    return SDMDataObject::AutoDataBinaryPart(SDMDataObjectParser::parseIntAttr(a_node, HeaderParser::SIZE),
					     SDMDataObjectParser::parseStringsAttr<AxisName, CAxisName>(a_node, HeaderParser::AXES),
					     SDMDataObjectParser::parseBoolAttr(a_node, HeaderParser::NORMALIZED));
  }
  
  SDMDataObject::ZeroLagsBinaryPart HeaderParser::parseZeroLagsBinaryPart(xmlNode* a_node, const string& attachmentName) {
    SDMDataObjectParser::isElement(a_node, attachmentName);
    
    return SDMDataObject::ZeroLagsBinaryPart(SDMDataObjectParser::parseIntAttr(a_node, HeaderParser::SIZE),
					     SDMDataObjectParser::parseStringsAttr<AxisName, CAxisName>(a_node, HeaderParser::AXES),
					     SDMDataObjectParser::parseStringAttr<CorrelatorType, CCorrelatorType>(a_node, HeaderParser::CORRELATORTYPE));
  }
  
  // CorrSubsetHeaderParser methods.

  CorrSubsetHeaderParser::CorrSubsetHeaderParser() {
    doc = NULL;
  }
  
  CorrSubsetHeaderParser::~CorrSubsetHeaderParser() {
    if (doc != NULL) xmlFreeDoc(doc);
  }


  void CorrSubsetHeaderParser::parseFile(const string& filename, SDMDataSubset& sdmCorrDataSubset){
    if (doc != NULL) xmlFreeDoc(doc);
    doc = xmlReadFile(filename.c_str(), NULL, XML_PARSE_NOBLANKS);
    if (doc == NULL) {
      throw SDMDataObjectParserException("The file '"+filename+"' could not be transformed into a DOM structure");
    }

    xmlNode* root_element = xmlDocGetRootElement(doc);
    parseSDMDataSubsetHeader(root_element, sdmCorrDataSubset);
  }

  void CorrSubsetHeaderParser::parseMemory(const string& buffer, SDMDataSubset& sdmCorrDataSubset) {
    if (doc != NULL) xmlFreeDoc(doc);
    doc = xmlReadMemory(buffer.data(), buffer.size(), "SDMDataHeader.xml", NULL,  XML_PARSE_NOBLANKS);
    if (doc == NULL) {
      throw SDMDataObjectParserException("The buffer containing the XML document could not be transformed into a DOM structure");
    }    
    
    xmlNode* root_element = xmlDocGetRootElement(doc);
    parseSDMDataSubsetHeader(root_element, sdmCorrDataSubset );
  };

  void CorrSubsetHeaderParser::parseSDMDataSubsetHeader(xmlNode* a_node, SDMDataSubset& sdmCorrDataSubset) {
    // Look up for <sdmSubsetDataHeader...
    SDMDataObjectParser::isElement(a_node, SDMDATASUBSETHEADER);

    // Project path.
    vector<unsigned int> v;
    v = SDMDataObjectParser::parseProjectPath(a_node);     // v should contain 4 (integration) or 5 (subintegration) elements.
    
    // Check conformity of execBlockNum, scanNum and subscanNum.

    if (v.at(0)      != sdmCorrDataSubset.owner()->execBlockNum()
	|| v.at(1)   != sdmCorrDataSubset.owner()->scanNum()
	|| v.at(2)   != sdmCorrDataSubset.owner()->subscanNum())
      throw SDMDataObjectParserException("The project path of this data subset '"
					 +SDMDataObjectParser::parseStringAttr(a_node, PROJECTPATH)
					 +"' is not compatible with the project path announced in the global header"
					 +" '"+sdmCorrDataSubset.owner()->projectPath()+"'"); 
    
    // Determine integrationNum [, subintegrationNum]
    sdmCorrDataSubset.integrationNum_ = v.at(3); 
    sdmCorrDataSubset.subintegrationNum_ = (v.size() == 5) ? v.at(4) : 0;
    
    // Traverse the children .
    xmlNode* child = a_node->children;

    // <schedulePeriodTime...
    parseSchedulePeriodTime(child, sdmCorrDataSubset);

    // <dataStruct...
    child = child->next;
    SDMDataObjectParser::isElement(child, CorrSubsetHeaderParser::DATASTRUCT);
    sdmCorrDataSubset.dataStruct_ = SDMDataObjectParser::parseStringAttr(child,  CorrSubsetHeaderParser::REF);

    child = child->next;
    if (SDMDataObjectParser::testElement(child, CorrSubsetHeaderParser::ABORTOBSERVATION)) {
      // Is it a cancelling [sub]integration ?
      sdmCorrDataSubset.aborted_ = true;
      parseAbortObservation(child, sdmCorrDataSubset);
    } 
    else {
      // ... or a sequence of attachments description.
      if (SDMDataObjectParser::testElement(child, FLAGSREF)) {
	sdmCorrDataSubset.flagsREF_ = SDMDataObjectParser::parseStringAttr(child, XLINKHREF);
	child = child->next;
      }

      
      if (SDMDataObjectParser::testElement(child, ACTUALTIMESREF)) {
	sdmCorrDataSubset.actualTimesREF_ = SDMDataObjectParser::parseStringAttr(child, XLINKHREF);
	child = child->next;
      }

      
      if (SDMDataObjectParser::testElement(child, ACTUALDURATIONSREF)) {
	sdmCorrDataSubset.actualDurationsREF_ = SDMDataObjectParser::parseStringAttr(child, XLINKHREF);
	child = child->next;
      }
            
      switch (sdmCorrDataSubset.owner()->correlationMode()) {
      case CROSS_ONLY:
	SDMDataObjectParser::isElement(child, CROSSDATAREF);
	sdmCorrDataSubset.crossDataREF_  = SDMDataObjectParser::parseStringAttr(child, XLINKHREF);
	sdmCorrDataSubset.crossDataType_ = SDMDataObjectParser::parseStringAttr<PrimitiveDataType, CPrimitiveDataType>(child, CorrSubsetHeaderParser::TYPE);
	child = child->next;
	break;
	
      case AUTO_ONLY:
	SDMDataObjectParser::isElement(child, AUTODATAREF);
	sdmCorrDataSubset.autoDataREF_  = SDMDataObjectParser::parseStringAttr(child, XLINKHREF);
	child = child->next;
	break;
	
      case CROSS_AND_AUTO:
	SDMDataObjectParser::isElement(child, CROSSDATAREF);
	sdmCorrDataSubset.crossDataREF_  = SDMDataObjectParser::parseStringAttr(child, XLINKHREF);
	sdmCorrDataSubset.crossDataType_ = SDMDataObjectParser::parseStringAttr<PrimitiveDataType, CPrimitiveDataType>(child, CorrSubsetHeaderParser::TYPE);
	child = child->next;
	
	SDMDataObjectParser::isElement(child, AUTODATAREF);
	sdmCorrDataSubset.autoDataREF_  = SDMDataObjectParser::parseStringAttr(child, XLINKHREF);
	child = child->next;
	break;
      }
      /*      
      if (sdmCorrDataSubset.owner()->spectralResolutionType()  != CHANNEL_AVERAGE) {
	SDMDataObjectParser::isElement(child, ZEROLAGSREF);
	sdmCorrDataSubset.zeroLagsREF_ = SDMDataObjectParser::parseStringAttr(child, XLINKHREF);
	child = child->next;
      } 
      */
      
      // zeroLags are optional in any case. Michel Caillat. 24 Jul 2008
      if (SDMDataObjectParser::testElement(child, ZEROLAGSREF)) {
	sdmCorrDataSubset.zeroLagsREF_ = SDMDataObjectParser::parseStringAttr(child, XLINKHREF);
	child = child->next;
      }
    }
  }

//   void CorrSubsetHeaderParser::parseProjectPath (xmlNode* a_node, SDMDataSubset& sdmCorrDataSubset) {
//     string projectPath = SDMDataObjectParser::parseStringAttr(a_node,CorrSubsetHeaderParser::PROJECTPATH);
    
//     boost::cmatch what;
//     unsigned int execBlockNum = 0;
//     unsigned int scanNum      = 0;
//     unsigned int subscanNum   = 0;
//     switch (sdmCorrDataSubset.owner()->spectralResolutionType()) {
//     case FULL_RESOLUTION:
//       if (boost::regex_match(projectPath.c_str(), what, SDMDataObjectParser::PROJECTPATH4) && what[0].matched) {
// 	execBlockNum = ::atoi(what[1].first);
// 	scanNum      = ::atoi(what[2].first);
// 	subscanNum   = ::atoi(what[3].first);
// 	sdmCorrDataSubset.integrationNum_  = ::atoi(what[4].first);
//       }
//       else
// 	throw SDMDataObjectParserException("Invalid string for projectPath '" + projectPath + "'");
//       break;

//     case  CHANNEL_AVERAGE:
//       if (boost::regex_match(projectPath.c_str(), what, SDMDataObjectParser::PROJECTPATH5) && what[0].matched) {
// 	execBlockNum = ::atoi(what[1].first);
// 	scanNum      = ::atoi(what[2].first);
// 	subscanNum   = ::atoi(what[3].first);
// 	sdmCorrDataSubset.integrationNum_    = ::atoi(what[4].first);
// 	sdmCorrDataSubset.subintegrationNum_ = ::atoi(what[5].first);
//       }
//       else
// 	throw SDMDataObjectParserException("Invalid string for projectPath '" + projectPath + "'");
//       break;

//     case BASEBAND_WIDE:
//       throw SDMDataObjectParserException("'"+CSpectralResolutionType::name(BASEBAND_WIDE)+"' cannot appear in this context.");
//       break;
//     }
    
//     if (execBlockNum    != sdmCorrDataSubset.owner()->execBlockNum()
// 	|| scanNum      != sdmCorrDataSubset.owner()->scanNum()
// 	|| subscanNum   != sdmCorrDataSubset.owner()->subscanNum())
//       throw SDMDataObjectParserException("The project path of this data subset '"
// 					 +projectPath
// 					 +"' is not compatible with the project path announced in the global header"
// 					 +" '"+sdmCorrDataSubset.owner()->projectPath()+"'"); 
//   }

  void CorrSubsetHeaderParser::parseSchedulePeriodTime(xmlNode* a_node, SDMDataSubset& sdmCorrDataSubset) {
    SDMDataObjectParser::isElement(a_node, CorrSubsetHeaderParser::SCHEDULEPERIODTIME);
    
    xmlNode* child = a_node->children;
    // <time...
    sdmCorrDataSubset.time_ = parseTime(child);

    // <interval...
    child = child->next;
    sdmCorrDataSubset.interval_ = parseInterval(child);
  }

  long long CorrSubsetHeaderParser::parseTime(xmlNode* a_node) {
    SDMDataObjectParser::isElement(a_node, CorrSubsetHeaderParser::TIME);
    return SDMDataObjectParser::parseLongLong(a_node->children);
  }


  long long CorrSubsetHeaderParser::parseInterval(xmlNode* a_node) {
    SDMDataObjectParser::isElement(a_node, CorrSubsetHeaderParser::INTERVAL);
    return SDMDataObjectParser::parseLongLong(a_node->children);
  }


  void CorrSubsetHeaderParser::parseAbortObservation(xmlNode* a_node, SDMDataSubset& sdmCorrDataSubset) {
    xmlNode* child = a_node->children;

    // <abortTime...
    SDMDataObjectParser::isElement(child, CorrSubsetHeaderParser::ABORTTIME);
    sdmCorrDataSubset.abortTime_ = SDMDataObjectParser::parseLongLong(child->children);
    
    // <abortReason...
    child = child->next;
    SDMDataObjectParser::isElement(child, CorrSubsetHeaderParser::ABORTREASON);
    sdmCorrDataSubset.abortReason_ = SDMDataObjectParser::parseString(child->children);    
  }

  void CorrSubsetHeaderParser::parseCrossDataType(xmlNode* a_node, SDMDataSubset& sdmCorrDataSubset) {
    SDMDataObjectParser::isElement(a_node, CorrSubsetHeaderParser::TYPE);
    sdmCorrDataSubset.crossDataType(SDMDataObjectParser::parseLiteral<PrimitiveDataType, CPrimitiveDataType>(a_node->children));    
  }


  void CorrSubsetHeaderParser::reset() {
    if (doc) 
      xmlFreeDoc(doc);
    
    doc = NULL;
  }


  // TPSubsetHeaderParser methods.

  TPSubsetHeaderParser::TPSubsetHeaderParser() {
    doc = NULL;
  }
  
  TPSubsetHeaderParser::~TPSubsetHeaderParser() {
    if (doc != NULL) xmlFreeDoc(doc);
  }

  
  void TPSubsetHeaderParser::parseFile(const string& filename, SDMDataSubset& sdmTPDataSubset){
    if (doc != NULL) xmlFreeDoc(doc);
    doc = xmlReadFile(filename.c_str(), NULL, XML_PARSE_NOBLANKS);
    if (doc == NULL) {
      throw SDMDataObjectParserException("The file '"+filename+"' could not be transformed into a DOM structure");
    }

    xmlNode* root_element = xmlDocGetRootElement(doc);
    parseSDMDataSubsetHeader(root_element, sdmTPDataSubset);

  }

  void TPSubsetHeaderParser::parseMemory(const string& buffer, SDMDataSubset& sdmTPDataSubset) {
    if (doc != NULL) xmlFreeDoc(doc);
    doc = xmlReadMemory(buffer.data(), buffer.size(), "SDMDataHeader.xml", NULL,  XML_PARSE_NOBLANKS);
    if (doc == NULL) {
      throw SDMDataObjectParserException("The buffer containing the XML document could not be transformed into a DOM structure");
    }    
    
    xmlNode* root_element = xmlDocGetRootElement(doc);
    parseSDMDataSubsetHeader(root_element, sdmTPDataSubset);
  }

  void TPSubsetHeaderParser::parseSDMDataSubsetHeader(xmlNode* a_node, SDMDataSubset& sdmTPDataSubset) {
    // Look up for <sdmSubsetDataHeader...
    SDMDataObjectParser::isElement(a_node, TPSubsetHeaderParser::SDMDATASUBSETHEADER);

    // Project path.
    int pathLen = (sdmTPDataSubset.owner()->dimensionality() == 0) ? 3 : 4;
    vector<unsigned int> v = SDMDataObjectParser::parseProjectPath(a_node, pathLen); 
    
    // Check conformity of execBlockNum, scanNum and subscanNum.
    if (v.at(0)      != sdmTPDataSubset.owner()->execBlockNum()
	|| v.at(1)   != sdmTPDataSubset.owner()->scanNum()
	|| v.at(2)   != sdmTPDataSubset.owner()->subscanNum())
      throw SDMDataObjectParserException("The project path of this data subset '"
					 +SDMDataObjectParser::parseStringAttr(a_node, PROJECTPATH)
					 +"' is not compatible with the project path announced in the global header"
					 +" '"+sdmTPDataSubset.owner()->projectPath()+"'"); 

    if (pathLen == 4)
      sdmTPDataSubset.integrationNum_ = v.at(3);

    // Traverse the children...
    xmlNode* child = a_node->children;

    // <schedulePeriodTime...
    parseSchedulePeriodTime(child, sdmTPDataSubset);

    // <dataStruct...
    child = child->next;
    SDMDataObjectParser::isElement(child, TPSubsetHeaderParser::DATASTRUCT);
    sdmTPDataSubset.dataStruct_ = (SDMDataObjectParser::parseStringAttr(child,  TPSubsetHeaderParser::REF));

    child = child->next;
    // Optional flags attachments.
    if (SDMDataObjectParser::testElement(child, FLAGSREF)) {
	sdmTPDataSubset.flagsREF_ = SDMDataObjectParser::parseStringAttr(child, XLINKHREF);
	child = child->next;      
    }

    if (SDMDataObjectParser::testElement(child, ACTUALTIMESREF)) {
      sdmTPDataSubset.actualTimesREF_ = SDMDataObjectParser::parseStringAttr(child, XLINKHREF);
      child = child->next;
    }
   
    if (SDMDataObjectParser::testElement(child, ACTUALDURATIONSREF)) {
      sdmTPDataSubset.actualDurationsREF_ = SDMDataObjectParser::parseStringAttr(child, XLINKHREF);
      child = child->next;
    }
    
    // Look for mandatory autoData element.
    SDMDataObjectParser::isElement(child, AUTODATAREF);
    sdmTPDataSubset.autoDataREF_ = (SDMDataObjectParser::parseStringAttr(child, TPSubsetHeaderParser::XLINKHREF )); 
  }

  void TPSubsetHeaderParser::parseProjectPath(xmlNode* a_node, SDMDataSubset& sdmTPDataSubset) {
    string projectPath = SDMDataObjectParser::parseStringAttr(a_node,TPSubsetHeaderParser::PROJECTPATH);
    
#ifndef WITHOUT_BOOST
    boost::cmatch what;
#else
    std::cmatch what;
#endif
    unsigned int execBlockNum = 0;
    unsigned int scanNum      = 0;
    unsigned int subscanNum   = 0;

#ifndef WITHOUT_BOOST
    if (boost::regex_match(projectPath.c_str(), what, PROJECTPATH3) && what[0].matched) {
#else
    if (std::regex_match(projectPath.c_str(), what, PROJECTPATH3) && what[0].matched) {
#endif
      execBlockNum = ::atoi(what[1].first);
      scanNum      = ::atoi(what[2].first);
      subscanNum   = ::atoi(what[3].first);
    }
    else 
      throw SDMDataObjectParserException("Invalid string for projectPath '" + projectPath + "'");
    
    if (execBlockNum    != sdmTPDataSubset.owner()->execBlockNum()
	|| scanNum      != sdmTPDataSubset.owner()->scanNum()
	|| subscanNum   != sdmTPDataSubset.owner()->subscanNum())
      throw SDMDataObjectParserException("The project path of this data subset '"
					 +projectPath
					 +"' is not compatible with the project path announced in the global header"
					 +" '"+sdmTPDataSubset.owner()->projectPath()+"'"); 
  }

  void TPSubsetHeaderParser::parseSchedulePeriodTime(xmlNode* a_node, SDMDataSubset& sdmTPDataSubset) {
    SDMDataObjectParser::isElement(a_node, TPSubsetHeaderParser::SCHEDULEPERIODTIME);
    
    xmlNode* child = a_node->children;
    // <time...
    sdmTPDataSubset.time_ = parseTime(child);

    // <interval...
    child = child->next;
    sdmTPDataSubset.interval_ = parseInterval(child);
  }

  long long TPSubsetHeaderParser::parseTime(xmlNode* a_node) {
    SDMDataObjectParser::isElement(a_node, TPSubsetHeaderParser::TIME);
    return SDMDataObjectParser::parseLongLong(a_node->children);
  }


  long long TPSubsetHeaderParser::parseInterval(xmlNode* a_node) {
    SDMDataObjectParser::isElement(a_node, TPSubsetHeaderParser::INTERVAL);
    return SDMDataObjectParser::parseLongLong(a_node->children);
  }
  // SDMDataObject::TPSubsetHeaderParser:: methods.


  // SDMDataObjectHeaderParser:: methods.
  void SDMDataObjectParser::isElement(xmlNode* a_node, const string& elementName) {
    //cout << "Entering isElement for " << a_node->name << endl;
    if ((a_node == NULL) ||
	(a_node->type != XML_ELEMENT_NODE) ||
	(elementName.compare((const char*)a_node->name) != 0)) {
      ostringstream oss;
      oss << "Could not find '<" << elementName << "...";
      if ((a_node != NULL) && (a_node->type == XML_ELEMENT_NODE))
	oss << " ( I was given '<" << a_node->name <<"...')";
      else 
	oss << " ( node is not an xml element ) " << endl;
								
      throw SDMDataObjectParserException(oss.str());
    }
    //cout << "Exiting isElement" << endl;
  }

  bool SDMDataObjectParser::testElement(xmlNode* a_node, const string& elementName) {
    //cout << "Entering testElement with " << elementName << " against " << a_node->name << endl;
    bool result = ((a_node != NULL) &&
		   (a_node->type == XML_ELEMENT_NODE) &&
		   (elementName.compare((const char*)a_node->name) == 0));
    return result;
  }

  void SDMDataObjectParser::inElements(xmlNode* a_node, const vector<string>& elementNames) {
    if (find(elementNames.begin(), elementNames.end(), string((char*) a_node->name)) == elementNames.end()) {
      ostringstream message;
      copy(elementNames.begin(), elementNames.end(), ostream_iterator<string>(message, " "));
      throw SDMDataObjectParserException("Could not find any of elements '" + message.str()+"' in " + string((const char*) a_node->name));
    }
  }

  xmlAttr*  SDMDataObjectParser::hasAttr(xmlNode* a_node, const string& attrName) {
    xmlAttr* result = 0;
    for (struct _xmlAttr* attr = a_node->properties; attr; attr = attr->next) {
      if (attrName.compare((const char*) attr->name) == 0) {
	result = attr;
	break;
      }
    }
    return result;  
  }

  void SDMDataObjectParser::tokenize(const string& str,
				     vector<string>& tokens,
				     const string& delimiters) {
    // Skip delimiters at beginning.
    string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    string::size_type pos     = str.find_first_of(delimiters, lastPos);
    
    while (string::npos != pos || string::npos != lastPos) {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }    
  }


  void SDMDataObjectParser::tokenize(const string& str,
				     set<string>& tokens,
				     const string& delimiters) {
    // Skip delimiters at beginning.
    string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    string::size_type pos     = str.find_first_of(delimiters, lastPos);
    
    while (string::npos != pos || string::npos != lastPos) {
        // Found a token, add it to the vector.
        tokens.insert(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }    
  }

  string SDMDataObjectParser::substring(const string &s, int a, int b) {
    return s.substr(a,(b - a));
  }

  string SDMDataObjectParser::trim(const string& s) {
    unsigned int i = 0;
    while (s.at(i) == ' ' && i < s.length())
      ++i;
    if (i == s.length())
      return "";
    unsigned int j = s.length() - 1;
    while (s.at(j) == ' ' && j > i)
      --j;
    return substring(s,i,j + 1);
  }

  string SDMDataObjectParser::parseString(xmlNode* a_node) {
    if ((a_node != NULL) && (a_node->next == NULL))
      return string((const char*) a_node->content);
    
    throw SDMDataObjectParserException("Invalid node , can't be parsed into a long long");

  }

  long long  SDMDataObjectParser::parseLongLong(xmlNode* a_node) {
    if ((a_node != NULL) && (a_node->next == NULL)) {
      istringstream in;
      in.str((const char*) a_node->content);
      long long x;
      in >> x;
      if (in.rdstate() == istream::failbit)
			throw SDMDataObjectParserException("failed to parse '"+string((const char*)a_node->content)+"' as a long long in" + string((const char*)a_node->parent->name));
      return x;
    }
    
    throw SDMDataObjectParserException("Invalid node , can't be parsed into a long long");
  }

  int SDMDataObjectParser::parseInt(xmlNode* a_node) {
    //cout << "Entering parseInt with " << a_node->content << endl;
    if ((a_node != NULL) && (a_node->next == NULL)) {
#ifndef WITHOUT_BOOST
      const boost::regex UINT("[0-9]+");
      boost::cmatch what;
      if (boost::regex_match((char*)a_node->content, what, UINT)) {
	return (::atoi(what[0].first));
      }
#else
      const std::regex UINT("[0-9]+");
      std::cmatch what;
      if (std::regex_match((char*)a_node->content, what, UINT)) {
	return (::atoi(what[0].first));
      }
#endif
      else
	throw SDMDataObjectParserException("failed to parse '"+string((const char*)a_node->content)+"' as an int in " + string((const char*)a_node->parent->name));
    }

    throw SDMDataObjectParserException("Invalid node , can't be parsed into an int");
  }

  bool SDMDataObjectParser::parseBool(xmlNode* a_node) {
    if ((a_node != NULL) && (a_node->next == NULL)) {
#ifndef WITHOUT_BOOST
      const boost::regex TORF("true|false");
      boost::cmatch what;
      if (boost::regex_match((char*)a_node->content, what, TORF)) {
	return ( *(what[0].first) == 't') ? true:false;
      }
#else
      const std::regex TORF("true|false");
      std::cmatch what;
      if (std::regex_match((char*)a_node->content, what, TORF)) {
	return ( *(what[0].first) == 't') ? true:false;
      }
#endif
      else
	throw SDMDataObjectParserException("failed to parse '"+string((const char*)a_node->content)+"' as an int in " + string((const char*)a_node->parent->name));
    }

    throw SDMDataObjectParserException("Invalid node , can't be parsed into an bool");    
  }

  float SDMDataObjectParser::parseFloat(xmlNode* a_node) {
    if ((a_node != NULL) && (a_node->next == NULL)) {
      istringstream in;
      in.str((const char*) a_node->content);
      float x;
      in >> x;
      if (in.rdstate() == istream::failbit)
			throw SDMDataObjectParserException("failed to parse '"+string((const char*)a_node->content)+"' as a float in " + string((const char*)a_node->parent->name));
      return x;
    }

    throw SDMDataObjectParserException("Invalid node , can't be parsed into an float");
  }

  int SDMDataObjectParser::parseIntAttr(xmlNode* a_node, const string& attrName) {
    xmlAttr* attr = 0;

    if ((attr = hasAttr(a_node, attrName))) {
      int result =parseInt(attr->children);
       return result;
    }
    else throw SDMDataObjectParserException("could not find attribute '" + attrName + "' in " + string((const char*)a_node->name));    
  }

  bool SDMDataObjectParser::parseBoolAttr(xmlNode* a_node, const string& attrName) {
    xmlAttr* attr = 0;

    if ((attr = hasAttr(a_node, attrName))) {
      bool result = parseBool(attr->children);
      return result;
    }
    else throw SDMDataObjectParserException("could not find attribute '" + attrName + "' in " + string((const char*)a_node->name));    
  }

  float SDMDataObjectParser::parseFloatAttr(xmlNode* a_node, const string& attrName) {
    xmlAttr* attr = 0;

    if ((attr = hasAttr(a_node, attrName))) {
      float result = parseFloat(attr->children);
      //cout << attr->name << " = " << result << endl;
      return result;
    }
    else throw SDMDataObjectParserException("could not find attribute '" + attrName + "' in " + string((const char*)a_node->name));    
  }


  string SDMDataObjectParser::parseStringAttr(xmlNode* a_node, const string& attrName) {
    //cout << "Entering parseStringAttr with " << attrName << " in " << a_node->name << endl;
    xmlAttr* attr = 0;

    if ((attr = hasAttr(a_node, attrName))) {
      string result = string((const char*)attr->children->content);
      //cout << attr->name << " = " << result << endl;
      return result;
    }
    else throw SDMDataObjectParserException("could not find attribute '" + attrName + "' in " + string((const char*)a_node->name));    
  }


  vector<string> SDMDataObjectParser::parseStringsAttr(xmlNode* a_node, const string& attrName) {
    xmlAttr* attr = 0;

    if ((attr = hasAttr(a_node, attrName))) {
      vector<string> result;
      tokenize((const char*)attr->children->content, result);
      //cout << attr->name << " = '"; copy(result.begin(), result.end(), ostream_iterator<string>(cout, " ")); cout << "'" << endl; 
      return result;
    }
    else throw SDMDataObjectParserException("could not find attribute '" + attrName + "' in " + string((const char*)a_node->name));    
  }

  set<string> SDMDataObjectParser::parseStringSetAttr(xmlNode* a_node, const string& attrName) {
    xmlAttr* attr = 0;

    if ((attr = hasAttr(a_node, attrName))) {
      set<string> result;
      tokenize((const char*)attr->children->content, result);
      //cout << attr->name << " = '"; copy(result.begin(), result.end(), ostream_iterator<string>(cout, " ")); cout << "'" << endl; 
      return result;
    }
    else throw SDMDataObjectParserException("could not find attribute '" + attrName + "' in " + string((const char*)a_node->name));    
  }


  vector<unsigned int> SDMDataObjectParser::parseProjectPath(xmlNode* a_node, unsigned int len) {
    string projectPath = SDMDataObjectParser::parseStringAttr(a_node, HeaderParser::PROJECTPATH);
    vector<unsigned int> result;
    bool matched = true;
#ifndef WITHOUT_BOOST
    boost::cmatch what;
    switch (len) {
    case 3: matched = boost::regex_match(projectPath.c_str(), what, PROJECTPATH3); break;
    case 4: matched = boost::regex_match(projectPath.c_str(), what, PROJECTPATH4); break;
    case 5: matched = boost::regex_match(projectPath.c_str(), what, PROJECTPATH5); break;
#else	
    std::cmatch what;
    switch (len) {
    case 3: matched = std::regex_match(projectPath.c_str(), what, PROJECTPATH3); break;
    case 4: matched = std::regex_match(projectPath.c_str(), what, PROJECTPATH4); break;
    case 5: matched = std::regex_match(projectPath.c_str(), what, PROJECTPATH5); break;
#endif	
    default: throw SDMDataObjectParserException ("internal error in method 'parseProjectPath'. The parameter 'len' has a value out of the range [3,5]");
    }
    
    if (!matched)
      throw SDMDataObjectException("'" + projectPath + "' is an invalid string for a 'projectPath' attribute");

    for (unsigned int i = 0; i < len; i++) {
      result.push_back(::atoi(what[i+1].first));
    }
    return result;
  }

#ifndef WITHOUT_BOOST
  const boost::regex  SDMDataObjectParser::PROJECTPATH4OR5("([0-9]+)/([0-9]+)/([0-9]+)/([0-9]+)/([0-9]+/)?");
#else
  const std::regex  SDMDataObjectParser::PROJECTPATH4OR5("([0-9]+)/([0-9]+)/([0-9]+)/([0-9]+)/([0-9]+/)?");
#endif
  vector<unsigned int> SDMDataObjectParser::parseProjectPath(xmlNode* a_node) {
    string projectPath = SDMDataObjectParser::parseStringAttr(a_node, HeaderParser::PROJECTPATH);
    vector<unsigned int> result;
    
    bool matched = true;

#ifndef WITHOUT_BOOST
    boost::cmatch what;
    matched = boost::regex_match(projectPath.c_str(), what, PROJECTPATH4OR5);
#else
    std::cmatch what;
    matched = std::regex_match(projectPath.c_str(), what, PROJECTPATH4OR5);
#endif
    
    if (!matched)
      throw SDMDataObjectException("'" + projectPath + "' is an invalid string for a 'projectPath' attribute.");

    // Let's retrieve the 4 first numbers.
    for (unsigned int i = 0; i < 4; i++)
      result.push_back(::atoi(what[i+1].first));
		       
    // and the fifth if it exists...
    if (what[5].matched) {
      result.push_back(::atoi(what[5].first));
    }
    
    return result;
  }

  const ByteOrder* SDMDataObjectParser::parseByteOrderAttr(xmlNode* a_node, const string& attrName) {
    string byteOrder = SDMDataObjectParser::parseStringAttr(a_node, attrName);

    if (byteOrder.compare("Little_Endian")==0) return ByteOrder::Little_Endian;
    if (byteOrder.compare("Big_Endian")==0) return ByteOrder::Big_Endian;

    throw SDMDataObjectParserException("'" + byteOrder + "' is an invalid string for a 'byteOrder' attribute.");
  }

  SDMDataObjectParser::SDMDataObjectParser() {;}
  SDMDataObjectParser::~SDMDataObjectParser() {;}

  void SDMDataObjectParser::parseFileHeader(const string& filename, SDMDataObject& sdmDataObject) {
    headerParser.parseFile(filename, sdmDataObject);
  }

  void SDMDataObjectParser::parseMemoryHeader(const string& buffer, SDMDataObject& sdmDataObject) {
    headerParser.parseMemory(buffer, sdmDataObject);
  }

  void SDMDataObjectParser::parseFileCorrSubsetHeader(const string& filename, SDMDataSubset& sdmCorrDataSubset) {
    corrSubsetHeaderParser.parseFile(filename, sdmCorrDataSubset);
  }

  void SDMDataObjectParser::parseMemoryCorrSubsetHeader(const string& buffer, SDMDataSubset& sdmCorrDataSubset) {
    corrSubsetHeaderParser.parseMemory(buffer, sdmCorrDataSubset);
  }

  void SDMDataObjectParser::parseFileTPSubsetHeader(const string& filename, SDMDataSubset& sdmCorrDataSubset) {
    tpSubsetHeaderParser.parseFile(filename, sdmCorrDataSubset);
  }

  void SDMDataObjectParser::parseMemoryTPSubsetHeader(const string& buffer, SDMDataSubset& sdmCorrDataSubset) {
    tpSubsetHeaderParser.parseMemory(buffer, sdmCorrDataSubset);
  }
  
}

#if 0
using namespace asdmbinaries;
int main (int argC, char* argV[]) {

  if (argC != 3) return (1);

  SDMDataObjectParser parser;
  SDMDataObject sdmDataObject;

  SDMDataSubset sdmDataSubset;  

  cout << "Trying to parse an SDMDataHeader in " << argV[1] << " and an SDMDataSubsetHeader in " << argV[2] <<endl;
  try {
    parser.parseFileHeader(argV[1], sdmDataObject);
    cout << "----- SDMDataObject ------" << endl;
    cout << "SDMDataHeader: " << endl;
    cout << endl;
    cout << sdmDataObject.toString() << endl;
    
    // Now process the sdmDataSubsetHeader passed in argV[2]
    switch (sdmDataObject.dimensionality()) {
    case 0: 
      {
	parser.parseFileTPSubsetHeader(argV[2], sdmDataSubset);
	cout << endl;
	cout << "SDMDataSubsetHeader: " << endl;
	cout << endl;
	cout << sdmDataSubset.toString(sdmDataObject.dimensionality()) << endl;
	break;
      }
      
    case 1: 
      {
	parser.parseFileCorrSubsetHeader(argV[2], sdmDataSubset);
	cout << endl;
	cout << "SDMDataSubsetHeader: " << endl;
	cout << endl;
	cout << sdmDataSubset.toString(sdmDataObject.dimensionality());
	break;
      }
      
    default:
      break;
    }
  }
  catch (SDMDataObjectParserException e) {
    cout << e.getMessage() << endl;
    exit(1);
  }

  cout << endl;
  cout << "----------------------" << endl;
  cout << "XML representation of the sdmDataHeader: " << endl;
  cout << endl;
  cout << sdmDataObject.toXML() << endl;

  cout << endl;
  cout << "----------------------" << endl;
  cout << "XML representation of the sdmDataSubsetHeader: " << endl;
  cout << endl;
  cout << sdmDataSubset.toXML(sdmDataObject.dimensionality()) << endl;
}
#endif
