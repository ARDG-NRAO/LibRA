#include <alma/ASDMBinaries/SDMDataObjectReader.h>

#include <iostream>
#include <iomanip>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <exception>
#include <sstream>

#include <alma/ASDMBinaries/CommonDefines.h>

#if defined( __APPLE__ )
   #include <AvailabilityMacros.h>
// 10.5 defines stat64 but Tiger does NOT
   #if defined(MAC_OS_X_VERSION_10_4) && ! defined(MAC_OS_X_VERSION_10_5)
      #define  stat64  stat
      #define fstat64 fstat
   #endif
#endif

using namespace CorrelationModeMod;
using namespace CorrelatorTypeMod;
using namespace PrimitiveDataTypeMod;
using namespace ProcessorTypeMod;

using namespace std;

namespace asdmbinaries {

  const string SDMDataObjectReader::MIMEBOUNDARY_1  = $MIMEBOUNDARY1;
  const string SDMDataObjectReader::MIMEBOUNDARY_2  = $MIMEBOUNDARY2;

#ifndef WITHOUT_BOOST
  const boost::regex SDMDataObjectReader::CONTENTIDBINREGEXP(" <([a-zA-Z]+)_([_0-9]+)\\.bin>");
  const boost::regex SDMDataObjectReader::CONTENTIDBINREGEXP1(" <([a-zA-Z]+)_([0-9]+)\\.bin>");
  const boost::regex SDMDataObjectReader::CONTENTIDBINREGEXP2(" <([a-zA-Z]+)_([0-9]+)_([0-9]+)\\.bin>");
  //  const boost::regex SDMDataObjectReader::CONTENTIDSUBSETREGEXP(" <(Subset)_([0-9]+)\\.xml>");
  const boost::regex SDMDataObjectReader::CONTENTIDSUBSETREGEXP(" <(Subset)_([0-9]+)(_[0-9]+)?\\.xml>");
  const boost::regex SDMDataObjectReader::CONTENTIDDATASTRUCTUREREGEXP(" <DataStructure.xml>");
#else
  const std::regex SDMDataObjectReader::CONTENTIDBINREGEXP(" <([a-zA-Z]+)_([_0-9]+)\\.bin>");
  const std::regex SDMDataObjectReader::CONTENTIDBINREGEXP1(" <([a-zA-Z]+)_([0-9]+)\\.bin>");
  const std::regex SDMDataObjectReader::CONTENTIDBINREGEXP2(" <([a-zA-Z]+)_([0-9]+)_([0-9]+)\\.bin>");
  //  const std::regex SDMDataObjectReader::CONTENTIDSUBSETREGEXP(" <(Subset)_([0-9]+)\\.xml>");
  const std::regex SDMDataObjectReader::CONTENTIDSUBSETREGEXP(" <(Subset)_([0-9]+)(_[0-9]+)?\\.xml>");
  const std::regex SDMDataObjectReader::CONTENTIDDATASTRUCTUREREGEXP(" <DataStructure.xml>");
#endif
  set<string> SDMDataObjectReader::BINATTACHNAMES;
  map<string, SDMDataObjectReader::BINATTACHCODES> SDMDataObjectReader::name2code;
  const bool SDMDataObjectReader::initClass_ = SDMDataObjectReader::initClass();

  // SDMDataObjectReader:: methods
  //
  SDMDataObjectReader::SDMDataObjectReader() {
    init();
  }

  SDMDataObjectReader::~SDMDataObjectReader() {
    done();
  }

  bool SDMDataObjectReader::initClass() {
    char* binAttachNamesC[] = {(char *) "actualDurations", (char *) "actualTimes", (char *) "autoData", (char *) "flags", (char *) "crossData", (char *) "zeroLags", 0};
    for (int i = 0; binAttachNamesC[i]; i++) {
      BINATTACHNAMES.insert(string(binAttachNamesC[i]));
    }

    name2code["actualDurations"] = ACTUALDURATIONS;
    name2code["actualTimes"]     = ACTUALTIMES;
    name2code["autoData"]        = AUTODATA;
    name2code["flags"]           = FLAGS;
    name2code["crossData"]       = CROSSDATA;
    name2code["zeroLags"]        = ZEROLAGS;

    return true;
  }

  void SDMDataObjectReader::init() {
    filedes_ = 0;
    data_ = (char *) 0;
    position_ = 0;
    lastPosition_ = 0;
    endPosition_ = 0;
    
    read_ = UNKNOWN_;

    integrationNum_ = 0;
  }

  string::size_type SDMDataObjectReader::find(const string& s) {

    while (true) {
      while ((position_ < endPosition_) && (data_[position_++] != s.at(0))) {;}
      
      if (position_ == endPosition_) return string::npos;

      string::size_type lastPosition_ = position_;      
      unsigned long long int i = 1;

      while ((position_ < endPosition_) && (i < s.length())) {
	if (s.at(i) != data_[position_]) break;
	i++;
	position_++;
      }
      
      if (i == s.length()) return (position_ - s.length());
      
      if (position_ == endPosition_) return string::npos;
      
      position_ = lastPosition_; //+1;
    }
  }
    
  bool SDMDataObjectReader::compare(const string& s) {
    string::size_type index = position_;

    unsigned int i = 0;
    while ((index < endPosition_) && (i < s.length()) && (s.at(i++) == data_[index++])) {
      ;
    }
    return (i == s.length());
  }

  bool SDMDataObjectReader::EOD() {
    return position_ >= endPosition_;
  }


  string SDMDataObjectReader::extractXMLHeader(const string& boundary) {
    // cout << "Entering extractXMLHeader"  << endl;

    string boundary_ = "\n--"+boundary;

    unsigned long int positionBeg = 0;
    unsigned long int positionEnd = 0;

    // We assume that we are at the first character of the body.
    positionBeg = position_;

    // Look for the next boundary supposed to be found right after an the XML header. 
    if ( (positionEnd = find(boundary_)) == string::npos) {
      ostringstream eos;
      eos << "A second MIME boundary '" << boundary <<"' could not be found in the MIME message after position " << positionBeg << ".";      
      throw SDMDataObjectReaderException(eos.str());      
    }

    string xmlPart = string(data_+positionBeg, positionEnd-positionBeg);
  
    // cout << "Found XML Header : " << xmlPart << endl;
    // cout << "Exiting extractXMLHeader"  << endl;

    return xmlPart;
  }


  void SDMDataObjectReader::tokenize(const string& str,
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


  void SDMDataObjectReader::getFields(const string& header, map<string, string>& fields) {
    // We are at the last character of the line preceding the 1st field declaration.
    istringstream iss;
    iss.str(header);
    string line;

    getline(iss, line); // skip an nl
    
    // Split each line into field-name, field-value
    while (getline(iss, line)) {
      vector<string> nv;
      tokenize(line, nv, ":");
      if (nv.size() < 2) {
	ostringstream oss;
	oss << "Invalid field in a MIME header '" << header << "', integration #" << integrationNum_;
	throw SDMDataObjectReaderException (oss.str());
      }
      fields[nv.at(0)] = nv.at(1);
    }
  }

  string SDMDataObjectReader::getContentID() {
    // We are at the beginning of an attachment right after its opening boundary.
    string::size_type lastPosition = position_;
    
    // Now let's extract the header (which is followed by two successive nl nl)
    string::size_type endHeader;
    if ( (endHeader = find ("\n\n")) == string::npos) {
      throw SDMDataObjectReaderException("Could not find the end of a MIME header");
    }

    string header(data_+lastPosition, (endHeader - lastPosition));
    
    map<string, string> fields;
    getFields(header, fields);

    // Look for the Content-ID field
    map<string, string>::iterator iter = fields.find("Content-ID");
    if (iter == fields.end()) {
      ostringstream oss;
      oss << "'Content-ID' field is missing the MIME header of an attachment (approx. char position = " << position_ << ").";
      throw SDMDataObjectReaderException(oss.str());
    }
    return iter->second;
  }

  string SDMDataObjectReader::getContentLocation() {
    // We are at the beginning of an attachment right after its opening boundary.
    string::size_type lastPosition = position_;
    
    // Now let's extract the header (which is followed by two successive nl nl)
    string::size_type endHeader;
    if ( (endHeader = find ("\n\n")) == string::npos) {
      throw SDMDataObjectReaderException("Could not find the end of a MIME header");
    }

    string header(data_+lastPosition, (endHeader - lastPosition));
    
    map<string, string> fields;
    getFields(header, fields);

    // Look for the Content-Location field
    map<string, string>::iterator iter = fields.find("Content-Location");
    if (iter == fields.end()) {
      ostringstream oss;
      oss << "'Content-Location' field is missing the MIME header of an attachment (approx. char position = " << position_ << ").";
      throw SDMDataObjectReaderException(oss.str());
    }
    return iter->second;    
  }

  void SDMDataObjectReader::processBinaryAttachment(const string& boundary, const SDMDataSubset& sdmDataSubset) {

    string contentLocation = getContentLocation();

    // Extract the attachment name and the integration number.
#ifndef WITHOUT_BOOST
    boost::cmatch what;
    boost::regex r(" *" + sdmDataSubset.projectPath()+"(actualDurations|actualTimes|autoData|flags|crossData|zeroLags)\\.bin");
    if (boost::regex_match(contentLocation.c_str(), what, r)) {
    }
#else
    std::cmatch what;
    std::regex r(" *" + sdmDataSubset.projectPath()+"(actualDurations|actualTimes|autoData|flags|crossData|zeroLags)\\.bin");
    if (std::regex_match(contentLocation.c_str(), what, r)) {
    }
#endif
    else {
      ostringstream oss;
      oss << "Invalid Content-Location field '" << contentLocation <<"' in MIME header of an attachment (approx. char position = " << position_ << ").";
      throw SDMDataObjectReaderException(oss.str());
    } 

    // We are at the first byte of the body of the binary attachment.
    string::size_type startAttachPosition = position_;  // mcaillat

    // Now look for its closing boundary (or opening boundary of the next one if any).
    string boundary_ = "\n--"+boundary;
    string::size_type endAttachPosition;
    if ( (endAttachPosition = find(boundary_)) == string::npos) { 
      ostringstream eos;
      eos << "A MIME boundary '" << boundary << "' terminating a binary attachment starting approx. at character " << startAttachPosition <<" could not be found.";
      throw SDMDataObjectReaderException(eos.str());
    }

    // Compute the length of the attachment.
    string::size_type length = endAttachPosition - startAttachPosition;
    


    switch (name2code[what[1]]) {
    case ACTUALDURATIONS:
      actualDurations_ = (ACTUALDURATIONSTYPE *) (data_ + startAttachPosition);
      nActualDurations_ = length / sizeof (ACTUALDURATIONSTYPE);
      attachmentFlags.set(ACTUALDURATIONS);
      //cout << "actualDurations =" << (unsigned long int) actualDurations_ << ", nActualDurations = " << nActualDurations_ << endl;
      break;
    case ACTUALTIMES:
      actualTimes_ = (ACTUALTIMESTYPE *) (data_ + startAttachPosition);
      nActualTimes_ = length / sizeof( ACTUALTIMESTYPE);
      attachmentFlags.set(ACTUALTIMES);
      //cout << "actualTimes =" << (unsigned long int) actualTimes_ << ", nActualTimes = " << nActualTimes_ << endl;
      break;
    case AUTODATA:
      autoData_ = (AUTODATATYPE *) (data_ + startAttachPosition);
      nAutoData_ = length / sizeof(AUTODATATYPE);
      attachmentFlags.set(AUTODATA);
      //cout << "autoData =" << (unsigned long int) autoData_ << ", nAutoData = " << nAutoData_ << endl;
      break;
    case FLAGS:
      flags_ = (const FLAGSTYPE *) (data_ + startAttachPosition);
      nFlags_ = length / sizeof(FLAGSTYPE);
      attachmentFlags.set(FLAGS);
      //cout << "flags =" << (unsigned long int) flags_ << ", nFlags = " << nFlags_ << endl;
      break;
    case CROSSDATA:
      shortCrossData_ = 0;
      longCrossData_  = 0;
      floatCrossData_ = 0;
      switch (sdmDataSubset.crossDataType()) {
      case INT16_TYPE:
	shortCrossData_  = (const SHORTCROSSDATATYPE*) (data_ + startAttachPosition);
	nCrossData_ = length / sizeof (SHORTCROSSDATATYPE);
	//cout << "shortCrossData = " << (unsigned long int) shortCrossData_ << ", nShortCrossData = " << nCrossData_ << endl;
	break;
	
      case INT32_TYPE:
	longCrossData_  = (const INTCROSSDATATYPE*) (data_ + startAttachPosition);
	nCrossData_ = length / sizeof (INTCROSSDATATYPE);
	//cout << "longCrossData = " << (unsigned long int) longCrossData_ << ", nLongCrossData = " << nCrossData_ << endl;
	break;

      case FLOAT32_TYPE:
	floatCrossData_ = (const FLOATCROSSDATATYPE*) (data_ + startAttachPosition);
	nCrossData_ = length / sizeof (FLOATCROSSDATATYPE);
	break;
       
      default:
	ostringstream eos;
	eos << "'" << CPrimitiveDataType::toString(sdmDataSubset.crossDataType())
	    << "' is not a valid primitive data type for CROSSDATA";
	throw SDMDataObjectReaderException(eos.str());	
	// cout << "Unrecognized type for cross data" << endl;
      }	
      attachmentFlags.set(CROSSDATA);
      break;
    case ZEROLAGS:
      zeroLags_ = (ZEROLAGSTYPE *) (data_ + startAttachPosition);
      nZeroLags_ = length / sizeof(ZEROLAGSTYPE) ;
      attachmentFlags.set(ZEROLAGS);
      //cout << "zeroLags =" << (unsigned long int) zeroLags_ << ", nZeroLags = " << nZeroLags_ << endl;
      break;

    default:
      //cout << "Unrecognized code" << endl;
      break;
    }
    // cout << "found a binary attachment" << endl;
  }
  
  void SDMDataObjectReader::processMIMESDMDataHeader() {
    // cout << "Entering processMIMESDMDataHeader" << endl;
    string sdmDataHeader = extractXMLHeader(MIMEBOUNDARY_1);
    parser_.parseMemoryHeader(sdmDataHeader, sdmDataObject_);
    // cout << "Exiting processMIMESDMDataHeader" << endl;
  }

  void SDMDataObjectReader::processMIMESDMDataSubsetHeader(SDMDataSubset& sdmDataSubset) {
    // cout << "Entering processMIMESDMDataSubsetHeader" << endl;
    string sdmDataSubsetHeader = extractXMLHeader(MIMEBOUNDARY_2);
    if (sdmDataSubset.owner_->isCorrelation())
      parser_.parseMemoryCorrSubsetHeader(sdmDataSubsetHeader, sdmDataSubset);
    else 
      parser_.parseMemoryTPSubsetHeader(sdmDataSubsetHeader, sdmDataSubset);
    // cout << "Exiting processMIMESDMDataSubsetHeader" << endl;
  }

  void SDMDataObjectReader::processMIMEIntegration() {
    // cout << "Entering processMIMEIntegration" << endl;

    // We are 1 character beyond the end of --<MIMEBOUNDARY_2>
    string contentLocation = getContentLocation();

#ifndef WITHOUT_BOOST
    boost::regex r(" *" + sdmDataObject_.projectPath() + "([[:digit:]]+/){1,2}" + "desc.xml");;
    ostringstream oss;
    
    // Extract the Subset name and the integration [, subintegration] number.
    boost::cmatch what;
    if (!boost::regex_match(contentLocation.c_str(), what, r)) {
      ostringstream oss;
      oss << "Invalid Content-Location field '" << contentLocation <<"' in MIME header of a Subset (approx. char position = " << position_ << ").";
      throw SDMDataObjectReaderException(oss.str());
    } 
#else
    std::regex r(" *" + sdmDataObject_.projectPath() + "([[:digit:]]+/){1,2}" + "desc.xml");;
    ostringstream oss;
    
    // Extract the Subset name and the integration [, subintegration] number.
    std::cmatch what;
    if (!std::regex_match(contentLocation.c_str(), what, r)) {
      ostringstream oss;
      oss << "Invalid Content-Location field '" << contentLocation <<"' in MIME header of a Subset (approx. char position = " << position_ << ").";
      throw SDMDataObjectReaderException(oss.str());
    } 
#endif
    
    SDMDataSubset integration(&sdmDataObject_);
    
    // The SDMDataSubset header.
    processMIMESDMDataSubsetHeader(integration);
    
    if (integration.aborted_) {
      // The [sub]integration has been aborted. Just append its header, without trying to get binary attachment.    
      sdmDataObject_.append(integration);
    }
    else {
      // This is regular [sub]integration, process its binary attachments.
      attachmentFlags.reset();
      while (!EOD() && !compare("--")) {
	processBinaryAttachment(MIMEBOUNDARY_2, integration);
      } 
    
      if (EOD()) 
	throw SDMDataObjectReaderException("Unexpected end of data");

      // Now check if the binary attachments found are compatible with the correlation mode
      // and if their sizes are equal to what is announced in the global header.
    
      if (!attachmentFlags.test(ACTUALDURATIONS)) {
	// No actualdurations attachment found, ok then set everything to 0.
	integration.actualDurations_  = 0;
	integration.nActualDurations_ = 0;
      }
      else {
	if (nActualDurations_ != sdmDataObject_.dataStruct().actualDurations().size()) {
	  ostringstream oss;
	  oss << "Data subset '"<<contentLocation<<"': ";
	  oss << "zize of 'actualDuration' attachment (" << nActualDurations_ << ") is not equal to the size announced in the header (" << sdmDataObject_.dataStruct().actualDurations().size() << ")";
	  throw SDMDataObjectReaderException(oss.str());
	}

	integration.actualDurations_  = actualDurations_;
	integration.nActualDurations_ = nActualDurations_; 
      }

      if (!attachmentFlags.test(ACTUALTIMES)) {
	// No actualtimes attachment found, ok then set everything to 0.
	integration.actualTimes_  = 0;
	integration.nActualTimes_ = 0;
      }
      else {
	if (nActualTimes_ != sdmDataObject_.dataStruct().actualTimes().size()) {
	  ostringstream oss;
	  oss << "Data subset '"<<contentLocation<<"': ";
	  oss << "size of 'actualTimes' attachment (" << nActualTimes_ << ") is not equal to the size announced in the header (" << sdmDataObject_.dataStruct().actualTimes().size() << ")";
	  throw SDMDataObjectReaderException(oss.str());
	} 
	integration.actualTimes_  = actualTimes_;
	integration.nActualTimes_ = nActualTimes_;
      } 

      // The flags are optional. They may be absent.
      if (!attachmentFlags.test(FLAGS)) {
	// No flags binary attachment found, ok then set everything to 0.
	integration.flags_  = 0;
	integration.nFlags_ = 0;
      }
      else {
	// flags found
	// Check size conformity
	if (nFlags_ != sdmDataObject_.dataStruct().flags().size()) {
	  ostringstream oss;
	  oss << "Data subset '"<<contentLocation<<"': ";
	  oss << "size of 'flags' attachment (" << nFlags_ << ") is not equal to the size announced in the header (" << sdmDataObject_.dataStruct().flags().size() << ")";
	  throw SDMDataObjectReaderException(oss.str());
	} 

	// Set pointer and size
	integration.flags_ = flags_;
	integration.nFlags_ = nFlags_;
      }


      //
      // The presence of crossData and autoData depends on the correlation mode.

      // crossDataTypeSize is not currently used
      // unsigned int crossDataTypeSize;    
      switch (sdmDataObject_.correlationMode()) {  
      case CROSS_ONLY:
	if (!attachmentFlags.test(CROSSDATA)) {
	  ostringstream oss;
	  oss << "Data subset '"<<contentLocation<<"': ";
	  oss << "a binary attachment 'crossData' was expected in integration #" << integrationNum_;
	  throw SDMDataObjectReaderException(oss.str());
	}

      
	if (nCrossData_ != sdmDataObject_.dataStruct().crossData().size()) {
	  ostringstream oss;
	  oss << "Data subset '"<<contentLocation<<"': ";
	  oss << "size of 'crossData' attachment (" << nCrossData_ << ") is not equal to the size announced in the header (" << sdmDataObject_.dataStruct().crossData().size() << ")";
	  throw SDMDataObjectReaderException(oss.str());
	}
      
      
	if (attachmentFlags.test(AUTODATA)) {
	  ostringstream oss;
	  oss << "Data subset '"<<contentLocation<<"': ";
	  oss << "found an unexpected attachment 'autoData' in integration #" << integrationNum_ << ".";
	  throw SDMDataObjectReaderException(oss.str());
	}

	integration.shortCrossData_ = 0;
	integration.longCrossData_  = 0;
	integration.floatCrossData_ = 0;
	integration.nCrossData_     = 0;
	switch (integration.crossDataType()) {
	case INT32_TYPE:
	  integration.longCrossData_    = longCrossData_;
	  integration.nCrossData_       = nCrossData_; 
	  break;

	case INT16_TYPE:
	  integration.shortCrossData_   = shortCrossData_;
	  integration.nCrossData_       = nCrossData_; 
	  break;

	case FLOAT32_TYPE:
	  integration.floatCrossData_   = floatCrossData_;
	  integration.nCrossData_       = nCrossData_;
	  break;

	default:
	  throw SDMDataObjectReaderException("'"+CPrimitiveDataType::name(integration.crossDataType())+"' unexpected here.");
	} 
	break;
      
      case AUTO_ONLY:
	if (!attachmentFlags.test(AUTODATA)) {
	  ostringstream oss;
	  oss << "Data subset '"<<contentLocation<<"': ";
	  oss << "a binary attachment 'autoData' was expected.";
	  throw SDMDataObjectReaderException(oss.str());
	}
	if (nAutoData_ != sdmDataObject_.dataStruct().autoData().size()) {
	  ostringstream oss;
	  oss << "Data subset '"<<contentLocation<<"': ";
	  oss << "size of 'autoData' attachment (" << nAutoData_ << ") is not equal to the size announced in the header (" << sdmDataObject_.dataStruct().autoData().size() << ")";
	  throw SDMDataObjectReaderException(oss.str());
	}
      
      
	if (attachmentFlags.test(CROSSDATA)) {
	  ostringstream oss;
	  oss << "Data subset '"<<contentLocation<<"': ";
	  oss << "found an unexpected attachment 'crossData'";
	  throw SDMDataObjectReaderException(oss.str());
	}

	integration.autoData_    = autoData_;
	integration.nAutoData_   = nAutoData_; 

	break;
      
      case CROSS_AND_AUTO:
	if (!attachmentFlags.test(AUTODATA)) {
	  ostringstream oss;
	  oss << "Data subset '"<<contentLocation<<"': ";
	  oss << "a binary attachment 'autoData' was expected.";
	  throw SDMDataObjectReaderException(oss.str());
	}
	if (nAutoData_ != sdmDataObject_.dataStruct().autoData().size()) {
	  ostringstream oss;
	  oss << "Data subset '"<<contentLocation<<"': ";
	  oss << "size of 'autoData' attachment (" << nAutoData_ << ") is not equal to the size announced in the header (" << sdmDataObject_.dataStruct().autoData().size() << ")";
	  throw SDMDataObjectReaderException(oss.str());
	}
      
	if (!attachmentFlags.test(CROSSDATA)) {
	  ostringstream oss;
	  oss << "Data subset '"<<contentLocation<<"': ";
	  oss << "a binary attachment 'crossData' was expected.";
	  throw SDMDataObjectReaderException(oss.str());
	}

	// crossDataTypeSize is not currently used, skip this block
	// switch (integration.crossDataType()) {
	// case INT32_TYPE:
	//   crossDataTypeSize = sizeof(INTCROSSDATATYPE);
	//   break;
	// case INT16_TYPE:
	//   crossDataTypeSize = sizeof(SHORTCROSSDATATYPE);
	//   break;
	// case FLOAT32_TYPE:
	//   crossDataTypeSize = sizeof(FLOATCROSSDATATYPE);
	//  break;
	// default:
	//   throw SDMDataObjectReaderException("'"+CPrimitiveDataType::name(integration.crossDataType())+"' unexpected here.");
        // }

	if (nCrossData_ != sdmDataObject_.dataStruct().crossData().size()) {
	  ostringstream oss;
	  oss << "Data subset '"<<contentLocation<<"': ";
	  oss << "size of 'crossData' attachment (" << nCrossData_ << ") is not equal to the size announced in the header (" << sdmDataObject_.dataStruct().crossData().size() << ")";
	  throw SDMDataObjectReaderException(oss.str());
	}
	

	integration.shortCrossData_ = 0;
	integration.longCrossData_  = 0;
	integration.floatCrossData_ = 0;
	integration.nCrossData_     = 0;
	switch (integration.crossDataType()) {
	case INT32_TYPE:
	  integration.longCrossData_    = longCrossData_;
	  integration.nCrossData_       = nCrossData_; 
	  break;
	
	case INT16_TYPE:
	  integration.shortCrossData_   = shortCrossData_;
	  integration.nCrossData_       = nCrossData_; 
	  break;

	case FLOAT32_TYPE:
	  integration.floatCrossData_   = floatCrossData_;
	  integration.nCrossData_       = nCrossData_;
	  break;

	default:
	  throw SDMDataObjectReaderException("Data subset '"+contentLocation+"': '"+CPrimitiveDataType::name(integration.crossDataType())+"' unexpected here.");
	} 
	
	integration.autoData_    = autoData_;
	integration.nAutoData_   = nAutoData_; 
	
	
	break;
      default:
	throw SDMDataObjectReaderException("Data subset '"+contentLocation+"': unrecognized correlation mode");
	break;
      }
      
      
      if (attachmentFlags.test(ZEROLAGS)) {
	// Refuse the zeroLags attachment if it's not a Correlator or if the correlator is a CORRELATOR_FX (ACA).
	if ((sdmDataObject_.processorType_ != CORRELATOR) || (sdmDataObject_.correlatorType() == FX))
	  throw SDMDataObjectReaderException("zeroLags are not expected from a correlator CORRELATOR_FX");

	if (nZeroLags_ != sdmDataObject_.dataStruct().zeroLags().size()) {
	  ostringstream oss;
	  oss << "Data subset '"<<contentLocation<<"': ";
	  oss << "size of 'zeroLags' attachment (" << nZeroLags_ << ") is not equal to the size announced in the header (" << sdmDataObject_.dataStruct().zeroLags().size() << ")";
	  throw SDMDataObjectReaderException(oss.str());
	}
	integration.zeroLags_  = zeroLags_;
	integration.nZeroLags_ = nZeroLags_;      
      }
    
    sdmDataObject_.append(integration);
  }

    if (!compare("--\n--"+MIMEBOUNDARY_1))
      throw SDMDataObjectReaderException("Data subset '"+contentLocation+"': expecting a '--"+MIMEBOUNDARY_1+"' at the end of a data subset");
    
    find("--\n--"+MIMEBOUNDARY_1);
    // cout << "Exiting processMIMEIntegration" << endl;    
  }

  void SDMDataObjectReader::processMIMEIntegrations() {
    // cout << "Entering processMIMEIntegrations" << endl;

    while (!compare("--")) {
      // We are one character beyond the last character of --<MIMEBOUNDARY_1>
      // Let's ignore the MIME Header right after the  --<MIMEBOUNDARY_1> 
      // and move to the next --<MIMEBOUNDARY_2>
      if (find("--"+MIMEBOUNDARY_2) == string::npos)
	throw SDMDataObjectReaderException("Expecting a boundary '--"+MIMEBOUNDARY_2+"' at this position");

      //Process the next integration.
      integrationNum_++;
      processMIMEIntegration() ;
    }

    // cout << "Exiting processMIMEIntegrations" << endl;
  }

  void SDMDataObjectReader::processMIMESubscan() {
    //    cout << "Entering processMIMESubscan" << endl;

    // We are one character beyond the last character of --<MIMEBOUNDARY_1>
    // Let's ignore the MIME Header right after the  --<MIMEBOUNDARY_1> 
    // and move to the next --<MIMEBOUNDARY_2>
    // We are 1 character beyond the end of --<MIMEBOUNDARY_2>
    if (find("--"+MIMEBOUNDARY_2) == string::npos)
      throw SDMDataObjectReaderException("Expecting a boundary '--"+MIMEBOUNDARY_2+"' at this position");


    string contentLocation = getContentLocation();
    // cout << contentLocation << endl;
#ifndef WITHOUT_BOOST
    boost::regex r(" *"+sdmDataObject_.projectPath() + "desc.xml");
    
    // Extract the Subset name and the suffix number (which must be equal to 1).
    boost::cmatch what;
    if (!boost::regex_match(contentLocation.c_str(), what, r)) {
      ostringstream oss;
      oss << "Invalid Content-Location field '" << contentLocation <<"' in MIME header of the subset";
      throw SDMDataObjectReaderException(oss.str());
    } 
#else
    std::regex r(" *"+sdmDataObject_.projectPath() + "desc.xml");
    
    // Extract the Subset name and the suffix number (which must be equal to 1).
    std::cmatch what;
    if (!std::regex_match(contentLocation.c_str(), what, r)) {
      ostringstream oss;
      oss << "Invalid Content-Location field '" << contentLocation <<"' in MIME header of the subset";
      throw SDMDataObjectReaderException(oss.str());
    } 
#endif

    SDMDataSubset subscan(&sdmDataObject_);

    // The SDMDataSubset header.
    processMIMESDMDataSubsetHeader(subscan);
    

    // Process the binary attachments.
    attachmentFlags.reset();
    integrationNum_++;
    while(!EOD() && !compare("--")) {
      processBinaryAttachment(MIMEBOUNDARY_2, subscan);
    }

    if (EOD()) throw SDMDataObjectReaderException("Unexpected end of data");
    
    // Now check if the binary attachments found are compatible with the correlation mode.
    // and if their sizes are equal to what is announced in the global header.

    // Start with the only mandatory attachment : autoData.
    if (!attachmentFlags.test(AUTODATA)) {
      ostringstream oss;
      oss << "Binary attachment 'autoData' was expected in integration #" << integrationNum_;
      throw SDMDataObjectReaderException(oss.str());
    }
    if (nAutoData_ != sdmDataObject_.dataStruct().autoData().size()) {
      ostringstream oss;
      oss << "Size of 'autoData' attachment (" << nAutoData_ << ") is not equal to the size announced in the header (" << sdmDataObject_.dataStruct().autoData().size() << ")";
      throw SDMDataObjectReaderException(oss.str());
    }
    
    subscan.autoData_    = autoData_;
    subscan.nAutoData_   = nAutoData_; 
    

    // And now consider the optional attachments.

    // The actualDurations are optional. They may be absent.
    if (!attachmentFlags.test(ACTUALDURATIONS)) {
      // No actualDurations binary attachment found, ok then set everything to 0.
      subscan.actualDurations_  = 0;
      subscan.nActualDurations_ = 0;
    }
    else {
      // actualDurations found
      // Check size conformity
      if (nActualDurations_ != sdmDataObject_.dataStruct().actualDurations().size()) {
	ostringstream oss;
	oss << "Size of 'actualDurations' attachment (" << nActualDurations_ << ") is not equal to the size announced in the header (" << sdmDataObject_.dataStruct().actualDurations().size() << ")";
	throw SDMDataObjectReaderException(oss.str());
      } 

      // Set pointer and size
      subscan.actualDurations_ = actualDurations_;
      subscan.nActualDurations_ = nActualDurations_;
    }


    // The actualTimes are optional. They may be absent.
    if (!attachmentFlags.test(ACTUALTIMES)) {
      // No actualTimes binary attachment found, ok then set everything to 0.
      subscan.actualTimes_  = 0;
      subscan.nActualTimes_ = 0;
    }
    else {
      // actualTimes found
      // Check size conformity
      if (nActualTimes_ != sdmDataObject_.dataStruct().actualTimes().size()) {
	ostringstream oss;
	oss << "Size of 'actualTimes' attachment (" << nActualTimes_ << ") is not equal to the size announced in the header (" << sdmDataObject_.dataStruct().actualTimes().size() << ")";
	throw SDMDataObjectReaderException(oss.str());
      } 

      // Set pointer and size
      subscan.actualTimes_ = actualTimes_;
      subscan.nActualTimes_ = nActualTimes_;
    }


    // The flags are optional. They may be absent.
    if (!attachmentFlags.test(FLAGS)) {
      // No flags binary attachment found, ok then set everything to 0.
      subscan.flags_  = 0;
      subscan.nFlags_ = 0;
    }
    else {
      // flags found
      // Check size conformity
      if (nFlags_ != sdmDataObject_.dataStruct().flags().size()) {
	ostringstream oss;
	oss << "Size of 'flags' attachment (" << nFlags_ << ") is not equal to the size announced in the header (" << sdmDataObject_.dataStruct().flags().size() << ")";
	throw SDMDataObjectReaderException(oss.str());
      } 

      // Set pointer and size
      subscan.flags_ = flags_;
      subscan.nFlags_ = nFlags_;
    }

    // And finally let's check that no unexpected binary attachment was found.
    if (attachmentFlags.test(CROSSDATA))  {
      ostringstream oss;
      oss << "Found an unexpected attachment 'crossData' in the binary attachments.";
      throw SDMDataObjectReaderException(oss.str());
    }

    
    if (attachmentFlags.test(ZEROLAGS))  {
      ostringstream oss;
      oss << "Found an unexpected attachment 'zeroLags' in the binary attachments.";
      throw SDMDataObjectReaderException(oss.str());
    }

    sdmDataObject_.tpDataSubset(subscan);


    if (!compare("--\n--"+MIMEBOUNDARY_1))
      throw SDMDataObjectReaderException("Expecting a '--"+MIMEBOUNDARY_1+"' at the end of a data subset");
    
    find("--\n--"+MIMEBOUNDARY_1);    
  }

  void SDMDataObjectReader::processMIME() {
    //    cout << "Entering processMIME" << endl;
    // Let's find the opening boundary
    if (find("--"+MIMEBOUNDARY_1) == string::npos)
      throw SDMDataObjectReaderException("Expecting a first boundary '--"+MIMEBOUNDARY_1+"'.");

   // Check the Content-Location of the MIME header ...but do nothing special with it...
    string contentLocation=getContentLocation();
   
    // Detect SDMDataHeader.
    processMIMESDMDataHeader();

    // Do we have packed data (i.e. only one SDMDataSubset grouping all the integrations) or one integration per SDMDataSubset (i.e. per timestamp) ?
    if (sdmDataObject_.hasPackedData()) {
      processMIMESubscan();
    }
    else {
      processMIMEIntegrations();
    }

    
    // if (sdmDataObject_.isCorrelation()) {
    //   // Process integrations.
    //   processMIMEIntegrations();
    // }
    // else if (sdmDataObject_.isTP()){
    //   // cout << "TP data" << endl;
    //   if (sdmDataObject_.dimensionality() == 0) {
    // 	processMIMESubscan();
    //   }
    //   else {
    // 	processMIMEIntegrations();
    //   }
    // }
    // else {
    //   // cout << "Unrecognized type of binary data." << endl;
    // }
    //cout << "Exiting processMIME" << endl;
  }

  const SDMDataObject & SDMDataObjectReader::read(const char * buffer, unsigned long int size, bool fromFile) {
    if (!fromFile) read_  = MEMORY_;

    // Set up all sensitive pointers and sizes.
    data_        = (char *)buffer;
    dataSize_    = size;
    position_    = 0;
    endPosition_ = size;

    // And process the MIME message
    sdmDataObject_.valid_ = true;
    processMIME();

    sdmDataObject_.owns();
    return sdmDataObject_;
  }

  const SDMDataObject& SDMDataObjectReader::ref() const {
    if (read_ == UNKNOWN_) 
      throw SDMDataObjectReaderException("a reference to an SDMDataObject cannot be obtained as long as the method 'read' has not been called.");
    return sdmDataObject_;
  }

  const SDMDataObject* SDMDataObjectReader::ptr() const {
    if (read_ == UNKNOWN_) 
      throw SDMDataObjectReaderException("a reference to an SDMDataObject cannot be obtained as long as the method 'read' has not been called.");
    return &sdmDataObject_;
  }

  void SDMDataObjectReader::done() {
    switch (read_) {
    case UNKNOWN_:
    case MEMORY_:
      break;
    case FILE_:
      munmap((caddr_t) data_, dataSize_);
      close(filedes_);
      break;
    }
    sdmDataObject_.done();
    read_ = UNKNOWN_;
  }

  const SDMDataObject& SDMDataObjectReader::sdmDataObject() {
    return sdmDataObject_;
  }


  const SDMDataObject & SDMDataObjectReader::read(string filename) {

    struct stat fattr; // stat64 fattr;

    unsigned long int filesize;
    char* data = 0;
    // Open the file.
    errno = 0;
#ifdef __APPLE__
    if ( (filedes_ = open(filename.c_str(), O_RDONLY )) == -1) {
#else
    if ( (filedes_ = open(filename.c_str(), O_RDONLY | O_LARGEFILE)) == -1) {
#endif
      string message(strerror(errno));
      throw SDMDataObjectReaderException("Could not open file '" + filename + "'. The message was '" + message + "'");
    }
    read_ = FILE_;
    
    // Get its size.
    errno = 0;
    int status = fstat(filedes_,&fattr); // fstat64(filedes_,&fattr);
    if (status == -1) {
      string message(strerror(errno));
      throw SDMDataObjectReaderException("Could not retrieve size of file '" + filename + "'. The message was '" + message + "'");
    }
    filesize = fattr.st_size;

    // Map it to virtual memory address space.
    errno = 0;

    data = (char *) mmap((caddr_t)0, filesize, PROT_READ, MAP_SHARED, filedes_, (off_t)0);
    if ( ((unsigned long) data) == 0xffffffff) {      
      string message(strerror(errno));
      throw SDMDataObjectReaderException("Could not map file '" + filename + "' to memory. The message was '" + message + "'");
    }
    
    // cout << "Successfully mapped file." << endl;
    // And delegate to the other read (memory buffer) method.
    return read(data, filesize, true);
  }

  // SDMDataObjectReader::

} // namespace asdmbinaries

