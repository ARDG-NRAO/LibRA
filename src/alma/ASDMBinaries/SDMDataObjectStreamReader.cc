#include <alma/ASDMBinaries/SDMDataObjectStreamReader.h>

#ifdef WITHOUT_BOOST
#include <regex>
#include <alma/ASDM/Misc.h>
#endif

using namespace CorrelationModeMod;
using namespace CorrelatorTypeMod;
using namespace PrimitiveDataTypeMod;
using namespace ProcessorTypeMod;

using namespace std;

namespace asdmbinaries {

  SDMDataObjectStreamReaderException::SDMDataObjectStreamReaderException():message("SDMDataObjectStreamReaderException:") {;}

  SDMDataObjectStreamReaderException::SDMDataObjectStreamReaderException(const string & message): message("SDMDataObjectStreamReaderException:" + message) {;}   

  const string& SDMDataObjectStreamReaderException::getMessage() { return message; }

  SDMDataObjectStreamReader::SDMDataObjectStreamReader()  {
  // cout << "SDMDataObjectStreamReader::SDMDataObjectStreamReader() : entering" << endl;
    const char *partNames[] = {"actualDurations", "actualTimes", "autoData", "crossData", "zeroLags", "flags"};
    set<string> dummy(partNames, partNames+6);
    s_partNames = dummy;
    sdmDataSubset = SDMDataSubset(&sdmDataObject);

    boundary_1 = "" ;
    currentState = S_NO_BDF;
  // cout << "SDMDataObjectStreamReader::SDMDataObjectStreamReader() : exiting" << endl;
  }

  SDMDataObjectStreamReader::~SDMDataObjectStreamReader() {
    //cout << "SDMDataObjectStreamReader::~SDMDataObjectStreamReader : entering" << endl;
    this->close();
    //cout << "SDMDataObjectStreamReader::~SDMDataObjectStreamReader : exiting" << endl;
  }

  void SDMDataObjectStreamReader::open(const string& path) {
    checkState(T_OPEN, "open");
    this->path = path;
    f.open(path.c_str(), ifstream::in);
    if (f.fail())
      throw SDMDataObjectStreamReaderException("could not open '" + path + "'.");

    boundary_1 = requireMIMEHeader();
    // cout << "Boundary = " << boundary_1 << endl;
    sdmDataObject.valid_ = true;
    requireSDMDataHeaderMIMEPart();
    sdmDataObject.owns();
    integrationIndex = -1; // We have not yet read any integration.

    currentState = S_AT_BEGINNING;
  }

  int64_t SDMDataObjectStreamReader::position() {
    return (int64_t) f.tellg();
  }

  void SDMDataObjectStreamReader::position(int64_t p) {
    f.seekg(p);
  }

  void SDMDataObjectStreamReader::close() {
    // cout << "SDMDataObjectStreamReader::close -- Entering" << endl;
    if (f.is_open()) {
      releaseMemory(sdmDataSubset);
      for (unsigned int i = 0; i < remainingSubsets.size(); i++)
	releaseMemory(remainingSubsets[i]);
      for (unsigned int i = 0; i < someSubsets.size(); i++)
	releaseMemory(someSubsets[i]);
      f.close();
      currentState = S_NO_BDF;
    }
    // cout << "SDMDataObjectStreamReader::close -- Exiting" << endl;
  }

  unsigned int SDMDataObjectStreamReader::currentIntegrationIndex() const { checkState(T_QUERY, "currentIntegrationIndex"); return integrationIndex; }
  unsigned long long SDMDataObjectStreamReader::currentIntegrationStartsAt() const { checkState(T_QUERY, "currentIntegrationStartAt"); return integrationStartsAt; }
  string				SDMDataObjectStreamReader::title() const { checkState(T_QUERY, "title"); return sdmDataObject.title(); }
  const ByteOrder*                      SDMDataObjectStreamReader::byteOrder() const { checkState(T_QUERY, "byteOrder"); return sdmDataObject.byteOrder(); }
  unsigned long long			SDMDataObjectStreamReader::startTime() const { checkState(T_QUERY, "startTime"); return sdmDataObject.startTime(); }
  unsigned int				SDMDataObjectStreamReader::numTime() const { checkState(T_QUERY, "numTime"); return sdmDataObject.numTime(); }
  string				SDMDataObjectStreamReader::dataOID() const { checkState(T_QUERY, "dataOID"); return sdmDataObject.dataOID(); }
  string				SDMDataObjectStreamReader::execBlockUID() const { checkState(T_QUERY, "execBlockUID"); return sdmDataObject.execBlockUID(); }
  unsigned int				SDMDataObjectStreamReader::execBlockNum() const { checkState(T_QUERY, "execBlockNum"); return sdmDataObject.execBlockNum(); }
  unsigned int				SDMDataObjectStreamReader::scanNum() const { checkState(T_QUERY, "scanNum"); return sdmDataObject.scanNum(); }
  unsigned int				SDMDataObjectStreamReader::subscanNum() const { checkState(T_QUERY, "subscanNum"); return sdmDataObject.subscanNum(); }
  unsigned int				SDMDataObjectStreamReader::numAntenna() const { checkState(T_QUERY, "numAntenna"); return sdmDataObject.numAntenna(); }
  CorrelationMode			SDMDataObjectStreamReader::correlationMode() const { checkState(T_QUERY, "correlationMode"); return sdmDataObject.correlationMode(); }
  OptionalSpectralResolutionType	SDMDataObjectStreamReader::spectralResolutionType() const { checkState(T_QUERY, "spectralResolutionType"); return sdmDataObject.spectralResolutionType(); }
  ProcessorType				SDMDataObjectStreamReader::processorType() const { checkState(T_QUERY, "processorType"); return sdmDataObject.processorType(); }
  CorrelatorType			SDMDataObjectStreamReader::correlatorType() const { checkState(T_QUERY, "correlatorType"); return sdmDataObject.correlatorType(); }
  bool                                  SDMDataObjectStreamReader::hasPackedData() const { checkState(T_QUERY, "isTP"); return sdmDataObject.hasPackedData();} 
  bool					SDMDataObjectStreamReader::isTP() const { checkState(T_QUERY, "isTP"); return sdmDataObject.isTP();} 
  bool					SDMDataObjectStreamReader::isWVR() const {checkState(T_QUERY, "isCorrelation"); return sdmDataObject.isWVR();} 
  bool					SDMDataObjectStreamReader::isCorrelation() const {checkState(T_QUERY, "isCorrelation"); return sdmDataObject.isCorrelation();} 
  const SDMDataObject::DataStruct&	SDMDataObjectStreamReader::dataStruct() const { checkState(T_QUERY, "dataStruct"); return sdmDataObject.dataStruct();}
  bool					SDMDataObjectStreamReader::aborted() const {checkState(T_QUERY, "aborted"); return sdmDataObject.aborted();} 
  unsigned long long			SDMDataObjectStreamReader::abortTime() const { checkState(T_QUERY, "abortTime"); return sdmDataObject.abortTime(); }
  string				SDMDataObjectStreamReader::abortReason() const { checkState(T_QUERY, "abortReason"); return sdmDataObject.abortReason(); }
  string				SDMDataObjectStreamReader::toString() const { checkState(T_QUERY, "toString"); return sdmDataObject.toString(); }

  void SDMDataObjectStreamReader::checkState(Transitions t, const string& methodName) const {
    // cout << "Entering checkState with currentState = " << currentState << " and transition = " << t << " for method " << methodName << endl;
    switch (currentState) {
    case S_NO_BDF:
      if ( t == T_OPEN ) {
	return;
      }
      break;
      
    case S_AT_BEGINNING :
      switch (t) {
      case T_QUERY:
      case T_TEST_END:
      case T_READ:
      case T_READ_NEXT:
      case T_READ_ALL:
      case T_CLOSE:
	return;
      default :
	break;
      }
      break;
      
    case S_READING:
      switch (t) {
      case T_TEST_END:
      case T_READ:
      case T_READ_NEXT:
      case T_READ_ALL:
      case T_QUERY:
      case T_CLOSE:
	return;
      default :
	break;
      }
      break;

    case S_AT_END:
      switch(t) {
      case  T_TEST_END:
      case T_QUERY: 
      case T_READ_NEXT:
      case T_READ_ALL:
      case T_CLOSE:
	return;
      default:
	break;
      }
      break;
    } // end switch on currentState

    // Any other pair (transition, currentState) will throw an exception.
    throw SDMDataObjectStreamReaderException("Invalid call of method '" + methodName + "' in the current context.");
  }

  bool SDMDataObjectStreamReader::hasSubset() {
    checkState(T_TEST_END, "hasSubset");
    bool atEnd = currentLine.compare("--"+boundary_1+"--") == 0;
    if (atEnd) currentState = S_AT_END;
    return !atEnd;
  }
 
  const vector<SDMDataSubset>& SDMDataObjectStreamReader::nextSubsets(unsigned int nSubsets) {
    checkState(T_READ_NEXT, "nextSubsets");
    
    // Deep empty of the vector nextSubsets
    // Firstly free all memory dynamically allocated in every element of the vector.
    for (unsigned int i = 0; i < someSubsets.size(); i++)
      releaseMemory(someSubsets.at(i));
    
    // Then clear the vector.
    someSubsets.clear();

    // Then populate the vector nextSubsets with as many SDMDataSubsets as possible up to a limit
    // of nSubsets read from the current position.
    unsigned int nRead = 0;
    while ((nRead < nSubsets) && hasSubset()) {
      someSubsets.push_back(SDMDataSubset(&sdmDataObject));
      integrationIndex++; nRead++;
      requireSDMDataSubsetMIMEPart(someSubsets.back());
      string line = nextLine();      
    }

    return someSubsets;
  }

  const vector<SDMDataSubset>& SDMDataObjectStreamReader::allRemainingSubsets() {
    // cout << "SDMDataObjectStreamReader::allRemainingSubsets: entering." << endl;
    checkState(T_READ_ALL, "allRemainingSubsets");

    // Deep empty of the vector remainingSubsets.
    // Firstly free all memory dynamically allocated in every element of the vector.
    for (unsigned int i = 0; i < remainingSubsets.size(); i++)
      releaseMemory(remainingSubsets.at(i));
    
    // Then clear the vector.
    remainingSubsets.clear();

    // Then populate the vector with a new collection.
    while (hasSubset()) {
      remainingSubsets.push_back(SDMDataSubset(&sdmDataObject));
      integrationIndex++;
      requireSDMDataSubsetMIMEPart(remainingSubsets.back());
      string line = nextLine();
    }

    // cout << "SDMDataObjectStreamReader::allRemainingSubsets: exiting." << endl;
    return remainingSubsets;
  }

  const SDMDataSubset & SDMDataObjectStreamReader::getSubset() {
    checkState(T_READ, "getSubset");
    integrationIndex++;
    requireSDMDataSubsetMIMEPart(sdmDataSubset);
    string line = nextLine();

    currentState = S_READING;
    return sdmDataSubset;
  }

  string SDMDataObjectStreamReader::nextLine() {
    unsigned long long whereAmI = f.tellg();
    getline(f, currentLine);
    if (f.fail()) {
      ostringstream oss ;
      oss << "SDMDataObjectStreamReader::nextLine() : I could not read a line in '" << path <<  "' at position " << whereAmI << ".";
      throw SDMDataObjectStreamReaderException(oss.str());
    }
    // cout << "nextLine has read '" << currentLine << "'" << endl;
    return currentLine;
  }

  pair<string, string> SDMDataObjectStreamReader::headerField2Pair(const string& hf){
    string name, value;
    size_t colonIndex = hf.find(":");
    if (colonIndex == string::npos)
      throw SDMDataObjectStreamReaderException(" could not detect a well formed MIME header field in '"+hf+"'");

    if (colonIndex > 0) {
      name = hf.substr(0, colonIndex);
#ifndef WITHOUT_BOOST
      boost::algorithm::trim(name);
#else
      asdm::trim(name);
#endif
    }

    if (colonIndex < hf.size()) {
      value = hf.substr(colonIndex+1);
#ifndef WITHOUT_BOOST
      boost::algorithm::trim(value);
#else
      asdm::trim(value);
#endif
    }

    return make_pair(name, value);
  }

  pair<string, string> SDMDataObjectStreamReader::requireHeaderField(const string & hf) {
    pair<string, string> hf2pair(headerField2Pair(nextLine()));
    // cout << hf2pair.first << ", " << hf2pair.second << endl;
#ifndef WITHOUT_BOOST
    if (boost::algorithm::to_upper_copy(hf2pair.first) != hf)
      throw SDMDataObjectStreamReaderException("read '" + currentLine + "'. Was expecting '" + hf + "'...");
#else
    if (asdm::str_toupper(hf2pair.first) != hf)
      throw SDMDataObjectStreamReaderException("read '" + currentLine + "'. Was expecting '" + hf + "'...");
#endif
    return hf2pair;
  }

  string unquote(const string& s, string& unquoted) {
    if (s.size() >= 2) 
      if (((s.at(0) == '"') && (s.at(s.size()-1) == '"')) || ((s.at(0) == '\'') && (s.at(s.size()-1) == '\''))) {
	if (s.size() == 2)
	  unquoted = "";
	else
	  unquoted = s.substr(1, s.size() - 2);
      }
      else
	unquoted = s;
    else
      unquoted = s;
    return unquoted;
  }
  


  string SDMDataObjectStreamReader::requireBoundaryInCT(const string& ctValue) {
    vector<string> cvValueItems;
 
#ifndef WITHOUT_BOOST
    boost::algorithm::split (cvValueItems, ctValue, boost::algorithm::is_any_of(";"));
    vector<string> cvValueItemsNameValue;
    for ( vector<string>::const_iterator iter = cvValueItems.begin(); iter != cvValueItems.end() ; iter++ ) {
      cvValueItemsNameValue.clear();
      boost::algorithm::split(cvValueItemsNameValue, *iter, boost::algorithm::is_any_of("="));
      string boundary;
      if ((cvValueItemsNameValue.size() > 1) && (boost::algorithm::to_upper_copy(boost::algorithm::trim_copy(cvValueItemsNameValue[0])) == "BOUNDARY") && (unquote(cvValueItemsNameValue[1], boundary).size() > 0))
	return boundary;
    }
#else
    asdm::strsplit (ctValue, ';', cvValueItems);
    vector<string> cvValueItemsNameValue;
    for ( vector<string>::const_iterator iter = cvValueItems.begin(); iter != cvValueItems.end() ; iter++ ) {
      cvValueItemsNameValue.clear();
      asdm::strsplit(*iter,'=',cvValueItemsNameValue);
      string boundary;
      if ((cvValueItemsNameValue.size() > 1) && (asdm::str_toupper(asdm::trim_copy(cvValueItemsNameValue[0])) == "BOUNDARY") && (unquote(cvValueItemsNameValue[1], boundary).size() > 0))
	return boundary;
    }
#endif
    throw SDMDataObjectStreamReaderException("could not find a boundary definition in '" + ctValue + "'.");
  }

  void SDMDataObjectStreamReader::skipAsLongAsLineStartsWith(const string& start) {
    int64_t curpos = 0;
    do {
      curpos = position();
      nextLine();
    }
    while (currentLine.find(start) == 0);
    position(curpos);
    return;
  }

  void SDMDataObjectStreamReader::skipUntilEmptyLine(int maxSkips) {
    // cout << "Entering skipUntilEmptyLine" << endl;
    int numSkips = 0;
    string line = nextLine();
    while ((line.size() != 0) && (numSkips <= maxSkips)) {
      line = nextLine();
      numSkips += 1;
    }

    if (numSkips > maxSkips) {
      ostringstream oss;
      oss << "could not find an empty line is less than " << maxSkips + 1 << " lines." << endl;
      throw SDMDataObjectStreamReaderException(oss.str());
    } 
    // cout << "Exiting skipUntilEmptyLine" << endl;
  }


  string SDMDataObjectStreamReader::requireMIMEHeader() {
    // MIME-Version
    pair<string, string>name_value(headerField2Pair(nextLine()));
    // cout << name_value.first << "=" << name_value.second << endl;
    // if (currentLine != "MIME-Version: 1.0") // a work around for the case when the very first character is not the expected "M" (happened with some corrupted data).
#ifndef WITHOUT_BOOST
    if (! boost::algorithm::iends_with(currentLine, "IME-Version: 1.0"))
#else
    std::string versionEnd = "IME-Version: 1.0";
    if ((currentLine.size()<=versionEnd.size()) || (currentLine.compare((currentLine.size()-versionEnd.size()),versionEnd.size(),versionEnd)!=0))
#endif
      throw SDMDataObjectStreamReaderException("'MIME-Version: 1.0' missing at the very beginning of the file '"+path+"'.");

    // Content-Type
    boundary_1 = requireBoundaryInCT(requireHeaderField("CONTENT-TYPE").second);

    // cout << "boundary_1 =" << boundary_1 << endl;

    // Content-Description
    name_value = requireHeaderField("CONTENT-DESCRIPTION");

    // Content-Location
    name_value = requireHeaderField("CONTENT-LOCATION");

    // Look for an empty line in the at most 10 subsequent lines.
    skipUntilEmptyLine(10);

    return boundary_1;
  }

  string SDMDataObjectStreamReader::accumulateUntilBoundary(const string& boundary, int maxLines) {
    // cout << "Entering accumulateUntilBoundary with maxLines = " << maxLines << endl;
    int numLines = 0;
    string line = nextLine();
    string result;
    while ((numLines <= maxLines) && (line.find("--"+boundary) == string::npos)) {
      result += line;
      numLines++;
      line = nextLine();
    }

    if (numLines > maxLines) {
      ostringstream oss;
      oss << "could not find the boundary string '"<< boundary << "' in less than " << maxLines + 1 << " lines." << endl;
      throw SDMDataObjectStreamReaderException(oss.str());    
    }

    return result;
  }

  void SDMDataObjectStreamReader::requireBoundary(const string& boundary, int maxLines) {
    // cout << "Entering require boundary with boundary == '" << boundary << "' and maxLines = " << maxLines << endl; 
    int numLines = 0;
    string dashdashBoundary = "--"+boundary;
    string line = nextLine();
    while ((numLines <= maxLines) && (line.compare(dashdashBoundary) != 0)) {
      numLines++;
      line = nextLine();
    }

    if (numLines > maxLines) {
      ostringstream oss;
      oss << "could not find the boundary string '"<< boundary << "' in less than " << maxLines + 1 << " lines." << endl;
      throw SDMDataObjectStreamReaderException(oss.str());
    }
  }

  void SDMDataObjectStreamReader::lookForBinaryPartSize(xmlNode* aNode) {
#ifndef WITHOUT_BOOST
    const boost::regex UINT("[0-9]+");
#else
    const std::regex UINT("[0-9]+");
#endif
    xmlNode *curNode = NULL;

    for (curNode = aNode; curNode ; curNode = curNode->next) {
      if (curNode->type == XML_ELEMENT_NODE) {
	if (s_partNames.find(string((char *)curNode->name)) != s_partNames.end()){
	  if (xmlHasProp(curNode, (const xmlChar*) "size")) {
	    xmlChar * value = xmlGetProp(curNode, (const xmlChar *) "size");
#ifndef WITHOUT_BOOST
	    boost::cmatch what;
	    if (boost::regex_match((char*) value, what, UINT)) {
#else
	    std::cmatch what;
	    if (std::regex_match((char*) value, what, UINT)) {
#endif
	      int64_t result = ::atoi(what[0].first);
	      xmlFree(value);
	      binaryPartSize[string((char *) curNode->name)] = result;
	    }
	    else {
	      xmlFree(value);
	      throw SDMDataObjectStreamReaderException("In '" + string((const char*) curNode->name) + "' failed to parse the value of '"+string((const char*) value)+"' as an int.");
	    }    
	  }
	  else {
	    throw SDMDataObjectStreamReaderException("In '" + string((const char*) curNode->name) + "' could not find the attribute 'size'.");
	  }
	}
      }
      lookForBinaryPartSize(curNode->children);
    }
  }

  string SDMDataObjectStreamReader::requireCrossDataType(xmlNode* parent) {
    string result;

    string comparee("crossData");
    xmlNode * child = parent->children;

    while ((child != 0) && (comparee.compare((const char*) child->name) != 0))
      child = child->next;

    if ((child == 0) || (child->type != XML_ELEMENT_NODE)) {
      ostringstream oss;
      oss << "could not find the element 'crossData'." << endl;
      throw SDMDataObjectStreamReaderException(oss.str());
    }

    if (xmlHasProp(child, (const xmlChar*) "type")) {
      xmlChar * value = xmlGetProp(child, (const xmlChar *) "type");
      result = string((const char *) value);
      xmlFree(value);
      return result;
    }
    else
      throw SDMDataObjectStreamReaderException("In '" + string((const char*) child->name) + "' could not find the attribute 'type'.");
  }

  void SDMDataObjectStreamReader::printElementNames(xmlNode * a_node) {
    xmlNode *cur_node = NULL;

    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
      if (cur_node->type == XML_ELEMENT_NODE) {
	cout << "node type: Element, name: " << cur_node->name << endl;
      }
      printElementNames(cur_node->children);
    }
  }

  void SDMDataObjectStreamReader::requireSDMDataHeaderMIMEPart() {
    //
    // Requires the presense of boundary_1
    requireBoundary(boundary_1, 0);

    // Ignore header fields
    // requireHeaderField("CONTENT-TYPE")
    // requireHeaderField("CONTENT-TRANSFER-ENCODING")
    // requireHeaderField("CONTENT-LOCATION")

    //
    // Look for an empty line at most distant from 100 lines from here.
    skipUntilEmptyLine(10);
    string sdmDataHeader = accumulateUntilBoundary(boundary_1, 100);
 
    /*
    xmlDoc * doc = xmlReadMemory(sdmDataHeader.data(), sdmDataHeader.size(),  "SDMDataHeader.xml", NULL, XML_PARSE_NOBLANKS);
    xmlChar* xmlBuff;
    int bufferSize;
    xmlDocDumpFormatMemory(doc, &xmlBuff, &bufferSize, 1);
    cout << string((const char*) xmlBuff, bufferSize) << endl;
    xmlFree(xmlBuff);
    xmlNode* root_element = xmlDocGetRootElement(doc);
    if ( root_element == NULL || root_element->type != XML_ELEMENT_NODE )
      throw SDMDataObjectStreamReaderException("Failed to parse the SDMDataHeader into a DOM structure."); 
    */
 
    parser.parseMemoryHeader(sdmDataHeader, sdmDataObject);
    unsigned int bps;
    if ((bps = sdmDataObject.dataStruct().flags().size()))
      binaryPartSize["flags"] = bps;

    if ((bps = sdmDataObject.dataStruct().actualTimes().size()))
      binaryPartSize["actualTimes"] = bps;

    if ((bps = sdmDataObject.dataStruct().actualDurations().size()))
      binaryPartSize["actualDurations"] = bps;

    if ((bps = sdmDataObject.dataStruct().zeroLags().size()))
      binaryPartSize["zeroLags"] = bps;

    if ((bps = sdmDataObject.dataStruct().autoData().size()))
      binaryPartSize["autoData"] = bps;
    
    if ((bps = sdmDataObject.dataStruct().crossData().size()))
      binaryPartSize["crossData"] = bps;
  }

  void SDMDataObjectStreamReader::requireSDMDataSubsetMIMEPart(SDMDataSubset & sdmDataSubset) {
    integrationStartsAt = f.tellg();
    
    skipAsLongAsLineStartsWith("--"+boundary_1);  // <--- this was added to preserve compatiblity with SDMDataObjectReader, which
                                                  // was less strict on the respect of the BDF specifications. (cf CAS-8151)
                                                  // Here the problem was caused by two successive occurrences of --MIME_boundary_1 
                                                  // instead of only one as indicated in the specs at the very beginning of
                                                  // an SDMDataSubsetHeader. M.Caillat - 4 decembre 2015
    pair<string, string> name_value = requireHeaderField("CONTENT-TYPE");
    boundary_2 = requireBoundaryInCT(name_value.second);
    // cout << "boundary_2 = " << boundary_2 << endl;
    name_value = requireHeaderField("CONTENT-DESCRIPTION");
    requireBoundary(boundary_2, 10);
    skipUntilEmptyLine(10);

    //
    // We assume that the subset header can't be longer than 100 lines.
    //
    string sdmDataSubsetHeader = accumulateUntilBoundary(boundary_2, 100);

    //
    // Empty sdmDataSubset if necessary.
    //
    releaseMemory(sdmDataSubset);

    // We can start to acquire a new SDMDataSubset.
    if (sdmDataObject.isCorrelation())
      parser.parseMemoryCorrSubsetHeader(sdmDataSubsetHeader, sdmDataSubset);
    else
      parser.parseMemoryTPSubsetHeader(sdmDataSubsetHeader, sdmDataSubset);

    attachmentFlags.reset();
#ifndef WITHOUT_BOOST
    boost::regex BINARYPARTLOC("([0-9]+/)+(actualDurations|actualTimes|autoData|crossData|zeroLags|flags)\\.bin");
#else
    std::regex BINARYPARTLOC("([0-9]+/)+(actualDurations|actualTimes|autoData|crossData|zeroLags|flags)\\.bin");
#endif
    bool done = false;
    while (!done) {
      name_value = requireHeaderField("CONTENT-TYPE");
      name_value = requireHeaderField("CONTENT-LOCATION");
    
#ifndef WITHOUT_BOOST
      boost::smatch what;
      const string contentLocation = boost::algorithm::trim_copy(name_value.second);
      if (!boost::regex_search(contentLocation, what, BINARYPARTLOC)) {
	throw SDMDataObjectStreamReaderException("Invalid field : '" + name_value.first + ":" + name_value.second + "'.");
      }
#else
      std::smatch what;
      const string contentLocation = asdm::trim_copy(name_value.second);
      if (!std::regex_search(contentLocation, what, BINARYPARTLOC)) {
	throw SDMDataObjectStreamReaderException("Invalid field : '" + name_value.first + ":" + name_value.second + "'.");
      }
#endif
      // cout << "Binary part name = " << what[2] << "...";
      string binaryPartName = string(what[2]);
      if (binaryPartSize.find(binaryPartName) == binaryPartSize.end())
	throw SDMDataObjectStreamReaderException("The size of '"+binaryPartName+"' was not announced in the data header.!");

      if (binaryPartSize[binaryPartName] == 0)
	throw SDMDataObjectStreamReaderException("The size of '"+binaryPartName+"' was announced as null. I was not expecting a '"+binaryPartName+"' attachment here.");

      skipUntilEmptyLine(10);
      int numberOfCharsPerValue = 0;
      char** binaryPartPtrPtr = 0;
      if (binaryPartName == "actualDurations") {
	attachmentFlags.set(ACTUALDURATIONS);
	binaryPartPtrPtr = (char**) &sdmDataSubset.actualDurations_;
	sdmDataSubset.nActualDurations_ = binaryPartSize[binaryPartName];
	sdmDataSubset.actualDurationsPosition_ = f.tellg();
	numberOfCharsPerValue = 8;
      }
      else if (binaryPartName == "actualTimes") {
	attachmentFlags.set(ACTUALTIMES);
	binaryPartPtrPtr = (char **) &sdmDataSubset.actualTimes_;
	sdmDataSubset.nActualTimes_ = binaryPartSize[binaryPartName];
	sdmDataSubset.actualTimesPosition_ = f.tellg();
	numberOfCharsPerValue = 8;
      }
      else if (binaryPartName == "autoData") {
	attachmentFlags.set(AUTODATA);
	binaryPartPtrPtr = (char **) &sdmDataSubset.autoData_;
	sdmDataSubset.nAutoData_ = binaryPartSize[binaryPartName];
	sdmDataSubset.autoDataPosition_ = f.tellg();
	numberOfCharsPerValue = 4;
      }
      else if (binaryPartName == "crossData") {
	attachmentFlags.set(CROSSDATA);
	sdmDataSubset.shortCrossData_ = 0;
	sdmDataSubset.longCrossData_  = 0;
	sdmDataSubset.floatCrossData_ = 0;
	sdmDataSubset.nCrossData_ = binaryPartSize[binaryPartName];
	sdmDataSubset.crossDataPosition_ = f.tellg();
	PrimitiveDataType pdt = sdmDataSubset.crossDataType();
	switch (pdt) {
	case  INT16_TYPE:
	  binaryPartPtrPtr = (char **) &sdmDataSubset.shortCrossData_;
	  numberOfCharsPerValue = 2;
	  break;
	case INT32_TYPE:
	  binaryPartPtrPtr = (char **) &sdmDataSubset.longCrossData_;
	  numberOfCharsPerValue = 4;
	  break;
	case FLOAT32_TYPE:
	  binaryPartPtrPtr = (char **) &sdmDataSubset.floatCrossData_;
	  numberOfCharsPerValue = 4;
	    break;
	default:
	  throw SDMDataObjectStreamReaderException("Invalid data type for cross data '" + CPrimitiveDataType::name(pdt) + "'.");
	}
      }
      else if (binaryPartName == "flags") {
	attachmentFlags.set(FLAGS);
	binaryPartPtrPtr = (char **) &sdmDataSubset.flags_;
	sdmDataSubset.nFlags_ = binaryPartSize[binaryPartName];
	sdmDataSubset.flagsPosition_ = f.tellg();
	numberOfCharsPerValue = 4;
      }
      else if (binaryPartName == "zeroLags") {
	attachmentFlags.set(ZEROLAGS);
	binaryPartPtrPtr = (char **) &sdmDataSubset.zeroLags_;
	sdmDataSubset.nZeroLags_ = binaryPartSize[binaryPartName];
	sdmDataSubset.zeroLagsPosition_ = f.tellg();
	numberOfCharsPerValue = 4;
      }

      int64_t numberOfCharsToRead = numberOfCharsPerValue * binaryPartSize[binaryPartName];
      *binaryPartPtrPtr = new char[numberOfCharsToRead * sizeof(char)];
      if (*binaryPartPtrPtr == 0) {
	ostringstream oss;
	oss << "Processing integration # " << integrationIndex << ": I could not get memory to store '" << binaryPartName << "'." << endl;
	throw SDMDataObjectStreamReaderException(oss.str());
      }

      f.read(*binaryPartPtrPtr, numberOfCharsToRead * sizeof(char));
      if (f.fail()) {
	ostringstream oss;
	oss << "Processing integration # " << integrationIndex << ": a problem occurred while reading '" << binaryPartName << "'." << endl;
	throw SDMDataObjectStreamReaderException(oss.str());
      }
      if (f.eof()) {
	ostringstream oss;
	oss << "Processing integration # " << integrationIndex << ": a unexpected end of file occurred while reading '" << binaryPartName  << "'." << endl;
	throw SDMDataObjectStreamReaderException(oss.str());
      }

      string line = nextLine(); // Absorb the nl right after the last byte of the binary attachment
      line = nextLine();   // This should boundary_2				
   
      if (line.find("--" + boundary_2) != 0) {
 	ostringstream oss;
	oss << "Processing integration # " << integrationIndex << ": unexpected '" <<  line << "' after the binary part '" << binaryPartName << "'." << endl;
	throw SDMDataObjectStreamReaderException(oss.str());
      }

      done = line.compare("--" + boundary_2+"--") == 0;
    }

    // Now check if the binary attachments found are compatible with the correlation mode
    // and if their sizes are equal to what is announced in the global header.
    //
    // The presence of crossData and autoData depends on the correlation mode.
    
    switch (sdmDataObject.correlationMode()) {  
    case CROSS_ONLY:
      if (!attachmentFlags.test(CROSSDATA)) {
	ostringstream oss;
	oss << "Data subset '"<<sdmDataSubset.projectPath()<<"': ";
	oss << "a binary attachment 'crossData' was expected in integration #" << integrationIndex;
	throw SDMDataObjectStreamReaderException(oss.str());
      }
      
      if (attachmentFlags.test(AUTODATA)) {
	ostringstream oss;
	oss << "Data subset '"<<sdmDataSubset.projectPath()<<"': ";
	oss << "found an unexpected attachment 'autoData' in integration #" << integrationIndex << ".";
	throw SDMDataObjectStreamReaderException(oss.str());
      }
      break;
      
    case AUTO_ONLY:
      if (!attachmentFlags.test(AUTODATA)) {
	ostringstream oss;
	oss << "Data subset '"<<sdmDataSubset.projectPath()<<"': ";
	oss << "a binary attachment 'autoData' was expected.in integration #" << integrationIndex << ".";
	throw SDMDataObjectStreamReaderException(oss.str());
      }

      if (attachmentFlags.test(CROSSDATA)) {
	ostringstream oss;
	oss << "Data subset '"<<sdmDataSubset.projectPath()<<"': ";
	oss << "found an unexpected attachment 'crossData' in integration #" << integrationIndex << ".";
	throw SDMDataObjectStreamReaderException(oss.str());
      }
      break;
      
    case CROSS_AND_AUTO:
      if (!attachmentFlags.test(AUTODATA)) {
	ostringstream oss;
	oss << "Data subset '"<<sdmDataSubset.projectPath()<<"': ";
	oss << "a binary attachment 'autoData' was expected in integration #" << integrationIndex << ".";
	throw SDMDataObjectStreamReaderException(oss.str());
      }
      
      if (!attachmentFlags.test(CROSSDATA)) {
	ostringstream oss;
	oss << "Data subset '"<<sdmDataSubset.projectPath()<<"': ";
	oss << "a binary attachment 'crossData' was expected in integration #" << integrationIndex << ".";
	throw SDMDataObjectStreamReaderException(oss.str());
      }      
      break;

    default:
      throw SDMDataObjectStreamReaderException("Data subset '"+sdmDataSubset.projectPath()+"': unrecognized correlation mode");
      break;
    }
    
    
    if (attachmentFlags.test(ZEROLAGS)) {
      // Refuse the zeroLags attachment if it's not a Correlator or if the correlator is a CORRELATOR_FX (ACA).
      if ((sdmDataObject.processorType_ != CORRELATOR) || (sdmDataObject.correlatorType() == FX))
	throw SDMDataObjectStreamReaderException("zeroLags are not expected from a correlator CORRELATOR_FX");
    }
  }

  void SDMDataObjectStreamReader::releaseMemory(SDMDataSubset & sdmDataSubset) {
    // cout << "SDMDataObjectStreamReader::releaseMemory : entering." << endl;
    if (sdmDataSubset.actualTimes_ != 0) {
    // cout << "actualTimes" << endl;
      delete[] sdmDataSubset.actualTimes_;
      sdmDataSubset.actualTimes_  = 0;    
    }
    sdmDataSubset.nActualTimes_ = 0;


    if (sdmDataSubset.actualDurations_ != 0) {
    // cout << "actualDurations" << endl;
      delete[] sdmDataSubset.actualDurations_;
      sdmDataSubset.actualDurations_  = 0;    
    }
    sdmDataSubset.nActualDurations_ = 0;

    if (sdmDataSubset.flags_ != 0) {
    // cout << "Flags" << endl;
      delete[] sdmDataSubset.flags_;
      sdmDataSubset.flags_  = 0;    
    }
    sdmDataSubset.nFlags_ = 0;

    if (sdmDataSubset.zeroLags_ != 0) {
    // cout << "zeroLags" << endl;
      delete[] sdmDataSubset.zeroLags_;
      sdmDataSubset.zeroLags_  = 0;    
    }
    sdmDataSubset.nZeroLags_ = 0;

    if (sdmDataSubset.autoData_ != 0) {
      //cout << "autoData_ = " << sdmDataSubset.autoData_ << " before deletion" <<  endl;
      delete[] sdmDataSubset.autoData_;
      sdmDataSubset.autoData_  = 0;    
    }
    sdmDataSubset.nAutoData_ = 0;

    if (sdmDataSubset.shortCrossData_ != 0) {
      //cout << "shortCrossData_ = " << sdmDataSubset.shortCrossData_ << " before deletion" <<  endl;
      delete[] sdmDataSubset.shortCrossData_;
      sdmDataSubset.shortCrossData_ = 0;
    }

    if (sdmDataSubset.longCrossData_ != 0) {
    // cout << "longCrossData" << endl;
      delete[] sdmDataSubset.longCrossData_;
      sdmDataSubset.longCrossData_ = 0;
    }

    if (sdmDataSubset.floatCrossData_ != 0) {
    // cout << "floatCrossData" << endl;
      delete[] sdmDataSubset.floatCrossData_;
      sdmDataSubset.floatCrossData_ = 0;
    }
    sdmDataSubset.nCrossData_ = 0;

  // cout << "SDMDataObjectStreamReader::releaseMemory : exiting." << endl;
    return;
  }
}  // end namespace asdmbinaries

using namespace asdmbinaries;

#ifdef TEST_CLASS
int main (int argC, char* argV[]) {
  if (argC < 2) {
    cout << "a.out filename" << endl;
    exit(1);
  }

  SDMDataObjectStreamReader ssr;
  try {
    ssr.open(argV[1]);
    cout << ssr.toString() << endl;
    while (ssr.hasSubset()) {
      const SDMDataSubset & sdmdss = ssr.getSubset();
      cout << "block of data #" << ssr.currentIntegrationIndex() << " starting at byte #" << ssr.currentIntegrationStartsAt() << endl; 
      cout << sdmdss.toString(128) << endl;
    }
  }
  catch (SDMDataObjectStreamReaderException& e) {
    cout << e.getMessage() << endl;
  }
  catch (SDMDataObjectParserException& e) {
    cout << e.getMessage() << endl;
  }
  catch (SDMDataObjectException& e) {
    cout << e.getMessage() << endl;
  }
}
#endif
