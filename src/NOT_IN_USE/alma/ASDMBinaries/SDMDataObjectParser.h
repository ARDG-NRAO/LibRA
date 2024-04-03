#ifndef SDMDataObjectParser_CLASS
#define SDMDataObjectParser_CLASS

#include <iostream>
#include <string>
#include <vector>
#include <libxml/parser.h>
#include <libxml/tree.h>



#ifdef REG_BASIC
#undef REG_BASIC
#endif

#ifdef REG_EXTENDED 
#undef REG_EXTENDED
#endif 

#ifdef REG_ICASE
#undef REG_ICASE
#endif

#ifdef REG_NOSUB
#undef REG_NOSUB
#endif

#ifdef REG_NEWLINE
#undef REG_NEWLINE
#endif

#ifdef REG_NOTBOL
#undef REG_NOTBOL
#endif

#ifdef REG_NOTEOL
#undef REG_NOTEOL
#endif

#ifdef REG_STARTEND
#undef REG_STARTEND
#endif

#ifndef WITHOUT_BOOST
#include <boost/regex.hpp>
#else
#include <regex>
#endif

#include <alma/ASDMBinaries/SDMDataObject.h>


#ifndef WITHOUT_ACS
#include "almaEnumerations_IFC.h"
#endif
#include <alma/Enumerations/CAtmPhaseCorrection.h>

#include <alma/Enumerations/CPrimitiveDataType.h>

#include <alma/Enumerations/CCorrelatorType.h>

namespace asdmbinaries {

  /**
   * A class to represent an exception thrown during the parsing of an XML header 
   * in a MIME message containing ALMA binary data.
   */ 
  class SDMDataObjectParserException {
    
  public:
    /**
     * An empty contructor.
     */
    SDMDataObjectParserException();

    /**
     * A constructor with a message associated with the exception.
     * @param m a string containing the message.
     */
    SDMDataObjectParserException(std::string m);
    
    /**
     * The destructor.
     */
    virtual ~SDMDataObjectParserException();

    /**
     * Returns the message associated to this exception.
     * @return a string.
     */
    std::string getMessage() const;
    
  protected:
    std::string message;
    
  };
  
  inline SDMDataObjectParserException::SDMDataObjectParserException() : message ("SDMDataObjectParserException") {}
  inline SDMDataObjectParserException::SDMDataObjectParserException(std::string m) : message(m) {}
  inline SDMDataObjectParserException::~SDMDataObjectParserException() {}
  inline std::string SDMDataObjectParserException::getMessage() const {
    return "SDMDataObjectParserException : " + message;
  }


  // class HeaderParser
  class HeaderParser {
    friend class SDMDataObjectParser;
    friend class SDMDataObject;
    
  public:
    HeaderParser();
    virtual ~HeaderParser();
    
    void parseFile(const std::string& filename, SDMDataObject& sdmDataObject);
    void parseMemory(const std::string& buffer, SDMDataObject& sdmDataObject);
    
    void reset();
    
    
    
  private:
    void parseSDMDataHeader(xmlNode* a_node, SDMDataObject& sdmDataObject);
    
    void parseProjectPath(xmlNode* a_node, SDMDataObject& sdmDataObject);
    long long parseStartTime(xmlNode* a_node);
    std::string parseDataOID(xmlNode* a_node);
    int  parseDimensionality(xmlNode* a_node);
    int  parseNumTime(xmlNode* a_node);
    void parseExecBlock(xmlNode* a_node, SDMDataObject& sdmDataObject);
    int  parseExecBlockNum(xmlNode* a_node);
    int  parseScanNum(xmlNode* a_node);
    int  parseSubscanNum(xmlNode* a_node);
    
    int  parseNumAntenna(xmlNode* a_node);
    
    void parseCorrelationMode(xmlNode* a_node, SDMDataObject& sdmDataObject);
    void parseSpectralResolution(xmlNode* a_node, SDMDataObject& sdmDataObject);
    void parseProcessorType(xmlNode* a_node, SDMDataObject& sdmDataObject);
    void parseDataStruct (xmlNode* a_node, SDMDataObject& sdmDataObject);
    
    SDMDataObject::Baseband  parseBaseband(xmlNode* a_node, SDMDataObject& sdmDataObject);
    
    void  parseSpectralWindow(xmlNode* a_node, SDMDataObject& sdmDataObject, std::vector<SDMDataObject::SpectralWindow>& spectralWindow);
    
    SDMDataObject::BinaryPart parseBinaryPart(xmlNode* a_node, const std::string& attachmentName);
    SDMDataObject::AutoDataBinaryPart parseAutoDataBinaryPart(xmlNode* a_node, const std::string& attachmentName);
    SDMDataObject::ZeroLagsBinaryPart parseZeroLagsBinaryPart(xmlNode* a_node, const std::string& attachmentName);

    //      SDMDataObject::TypedBinaryPart  parseTypedBinaryPart(xmlNode* a_node, const std::string& attachmentName);
    
    xmlDoc *doc;

#ifndef WITHOUT_BOOST
    static const boost::regex  PROJECTPATH3;
#else
    static const std::regex  PROJECTPATH3;
#endif
    const static std::string SDMDATAHEADER;
    const static std::string SCHEMAVERSION;
    const static std::string BYTEORDER;
    const static std::string PROJECTPATH;
    const static std::string STARTTIME;
    const static std::string DATAOID;
    const static std::string XLINKHREF;
    const static std::string XLINKTITLE;
    
    const static std::string DIMENSIONALITY;
    const static std::string NUMTIME;
    
    const static std::string EXECBLOCK;
    const static std::string EXECBLOCKNUM;
    const static std::string SCANNUM;
    const static std::string SUBSCANNUM;
    
    const static std::string NUMANTENNA;
    
    const static std::string CORRELATIONMODE;
    const static std::string SPECTRALRESOLUTION;
    const static std::string PROCESSORTYPE;
    const static std::string DATASTRUCT;
    const static std::string APC;
    const static std::string REF;
    
    const static std::string BASEBAND;
    const static std::string NAME;
    
    const static std::string SPECTRALWINDOW;
    const static std::string SW;
    const static std::string SWBB;
    const static std::string CROSSPOLPRODUCTS;
    const static std::string SDPOLPRODUCTS;
    const static std::string SCALEFACTOR;
    const static std::string NUMSPECTRALPOINT;
    const static std::string NUMBIN;
    const static std::string SIDEBAND;
    const static std::string IMAGE;
    
    const static std::string FLAGS;
    const static std::string ACTUALTIMES;
    const static std::string ACTUALDURATIONS;
    const static std::string ZEROLAGS;
    const static std::string CORRELATORTYPE;
    const static std::string CROSSDATA;
    const static std::string AUTODATA;
    const static std::string NORMALIZED;
    
    const static std::string SIZE;
    const static std::string AXES;
    const static std::string TYPE;
  }; // class HeaderParser

  // class CorrSubsetHeaderParser
  class CorrSubsetHeaderParser {
    friend class SDMDataSubset;
    
  public:
    CorrSubsetHeaderParser();
    virtual ~CorrSubsetHeaderParser();
    void parseFile(const std::string& filename, SDMDataSubset& sdmCorrDataSubset);
    void parseMemory(const std::string& buffer, SDMDataSubset& sdmCorrDataSubset);
    void parseCrossDataType(xmlNode* a_node, SDMDataSubset& sdmCorrDataSubset);
    void reset();
    
  private:
    // Regular expressions used to decipher the content of projectPath attribute.
#ifndef WITHOUT_BOOST
    static const boost::regex PROJECTPATH4;
    static const boost::regex PROJECTPATH5;
#else
    static const std::regex PROJECTPATH4;
    static const std::regex PROJECTPATH5;
#endif
    
    void parseSDMDataSubsetHeader(xmlNode* a_node, SDMDataSubset& sdmCorrDataSubset);
    //    void parseProjectPath(xmlNode* a_node, SDMDataSubset& sdmCorrDataSubset);
    void parseSchedulePeriodTime(xmlNode* a_node, SDMDataSubset& sdmCorrDataSubset);
    long long parseTime(xmlNode* a_node);
    long long parseInterval(xmlNode* a_node);
    void parseAbortObservation(xmlNode* a_node, SDMDataSubset& sdmCorrDataSubset);
    
    xmlDoc* doc;
    
    const static std::string SDMDATASUBSETHEADER;
    const static std::string PROJECTPATH;
    const static std::string SCHEDULEPERIODTIME;
    const static std::string TIME;
    const static std::string INTERVAL;
    const static std::string DATASTRUCT;
    const static std::string REF;
    const static std::string ABORTOBSERVATION;
    const static std::string ABORTTIME;
    const static std::string ABORTREASON;
    const static std::string XLINKHREF;
    const static std::string DATAREF;
    const static std::string FLAGSREF;
    const static std::string ACTUALTIMESREF;
    const static std::string ACTUALDURATIONSREF;
    const static std::string ZEROLAGSREF;
    const static std::string CROSSDATAREF;
    const static std::string TYPE;
    const static std::string AUTODATAREF;
  };  // class CorrSubsetHeaderParser


  // class TPSubsetHeaderParser
  class TPSubsetHeaderParser {
    friend class SDMDataSubset;
  public:
    TPSubsetHeaderParser();
    virtual ~TPSubsetHeaderParser();
    void parseFile(const std::string& filename,SDMDataSubset& sdmTPDataSubset );
    void parseMemory(const std::string& buffer,SDMDataSubset& sdmTPDataSubset );
    void reset();
    
  private:
    // Regular expressions used to decipher the content of projectPath attribute.
#ifndef WITHOUT_BOOST
    static const boost::regex PROJECTPATH3;
#else
    static const std::regex PROJECTPATH3;
#endif

    void parseSDMDataSubsetHeader(xmlNode* a_node,SDMDataSubset& sdmTPDataSubset);
    void parseProjectPath(xmlNode* a_node, SDMDataSubset& sdmTPDataSubset);
    void parseSchedulePeriodTime(xmlNode* a_node, SDMDataSubset& sdmCorrDataSubset);
    long long parseTime(xmlNode* a_node);
    long long parseInterval(xmlNode* a_node);
    std::string parseDataStructureDesc(xmlNode* a_node);
    void parseBinaryData(xmlNode* a_node, SDMDataSubset& sdmTPDataSubset);
    
    xmlDoc* doc;
    
    const static std::string SDMDATASUBSETHEADER;
    const static std::string PROJECTPATH;
    const static std::string SCHEDULEPERIODTIME;
    const static std::string TIME;
    const static std::string INTERVAL;
    const static std::string DATASTRUCT;
    const static std::string REF;
    const static std::string DATAREF;
    const static std::string XLINKHREF;
    const static std::string FLAGSREF;
    const static std::string ACTUALTIMESREF;
    const static std::string ACTUALDURATIONSREF;
    const static std::string AUTODATAREF;
  };  // class TPSubsetHeaderParser
  
  
  class SDMDataObjectParser {
    friend class SDMDataObject;
    friend class SDMDataSubset;
    friend class HeaderParser;

  public:
    SDMDataObjectParser();
    virtual ~SDMDataObjectParser();
    void parseFileHeader(const std::string& filename, SDMDataObject& sdmDataObject);
    void parseMemoryHeader(const std::string& buffer, SDMDataObject& sdmDataObject);

    void parseFileCorrSubsetHeader(const std::string& filename, SDMDataSubset& sdmCorrSubset);
    void parseMemoryCorrSubsetHeader(const std::string& buffer, SDMDataSubset& sdmCorrSubset);

    void parseFileTPSubsetHeader(const std::string& filename, SDMDataSubset& sdmCorrDataSubset);
    void parseMemoryTPSubsetHeader(const std::string& filename, SDMDataSubset& sdmCorrDataSubset);
    
    static void isElement(xmlNode* a_node, const std::string& elementName);
    static bool testElement(xmlNode* a_node, const std::string& elementName);
    static void inElements(xmlNode* a_node, const std::vector<std::string>& elementNames);

    static xmlAttr* hasAttr(xmlNode* a_node, const std::string& attrName);

    static void tokenize(const std::string& str,
			 std::vector<std::string>& tokens,
			 const std::string& delimiters = " ");

    static void tokenize(const std::string& str,
			 std::set<std::string>& tokens,
			 const std::string& delimiters = " ");
    static std::string substring(const std::string &s, int a, int b);
    static std::string trim(const std::string &s);

    static std::string parseString(xmlNode* a_node);
    static long long parseLongLong(xmlNode* a_node);
    static int   parseInt(xmlNode* a_node);
    static bool  parseBool(xmlNode* a_node);
    static float parseFloat(xmlNode* a_node);
    static int   parseIntAttr(xmlNode* a_node, const std::string& attrName);
    static bool  parseBoolAttr(xmlNode* a_node, const std::string& attrName);
    static float parseFloatAttr(xmlNode* a_node, const std::string& attrName);
    static std::string parseStringAttr(xmlNode* a_node, const std::string& attrName);
    static const ByteOrder* parseByteOrderAttr(xmlNode* a_node, const std::string& attrName);

    template<class Enum, class EnumHelper> static Enum parseStringAttr(xmlNode* a_node, const std::string& attrName) {
      xmlAttr* attr = 0;
      
      if ((attr = hasAttr(a_node, attrName))) {
	std::string s = std::string((const char*)attr->children->content);
	try {
	  Enum result = EnumHelper::literal(SDMDataObjectParser::trim(s));
	  return result;
	}
	catch(std::string m) {
	  throw  SDMDataObjectParserException(m);
	}
      }
      else 
	throw SDMDataObjectParserException("could not find attribute '" + attrName + "' in " + std::string((const char*)a_node->name));    
    }
    
    template<class Enum, class EnumHelper> static Enum parseLiteral(xmlNode* a_node) {
      if ((a_node != NULL) && (a_node->next == NULL)) {      
	try {
	  Enum result = EnumHelper::literal(SDMDataObjectParser::trim(std::string((const char*) a_node->content)));
	  return result;
	}
	catch (std::string m) {
	  throw SDMDataObjectParserException(m);
	}
      }
      else 
	throw SDMDataObjectParserException("The content of an element could not parsed into a literal");
    }


    static std::vector<std::string> parseStringsAttr(xmlNode* a_node, const std::string& attrName);
    static std::set<std::string> parseStringSetAttr(xmlNode* a_node, const std::string& attrName);

    template<class Enum, class EnumHelper> static std::vector<Enum> parseStringsAttr(xmlNode* a_node, const std::string& attrName) {
      xmlAttr* attr = 0;
      
      if ((attr = hasAttr(a_node, attrName))) {
	std::vector<std::string> v_s;
	tokenize((const char*)attr->children->content, v_s);

	std::vector<Enum> result; 
	unsigned int i = 0;
	try {
	  for (i = 0; i < v_s.size(); i++)
	    result.push_back(EnumHelper::literal(v_s.at(i)));
	  return result;
	}
	catch (std::string m) {
	  throw  SDMDataObjectParserException(m);
	}
      }
      else 
	throw SDMDataObjectParserException("could not find attribute '" + attrName + "' in " + std::string((const char*)a_node->name));
    }

    static std::vector<unsigned int> parseProjectPath(xmlNode* a_node, unsigned int len);

    static std::vector<unsigned int> parseProjectPath(xmlNode* a_node);

    private:    

#ifndef WITHOUT_BOOST
    static const boost::regex PROJECTPATH3;
    static const boost::regex PROJECTPATH4;
    static const boost::regex PROJECTPATH5;
    static const boost::regex PROJECTPATH4OR5;
#else
    static const std::regex PROJECTPATH3;
    static const std::regex PROJECTPATH4;
    static const std::regex PROJECTPATH5;
    static const std::regex PROJECTPATH4OR5;
#endif

    HeaderParser headerParser;
    CorrSubsetHeaderParser corrSubsetHeaderParser;
    TPSubsetHeaderParser tpSubsetHeaderParser;
    
  }; // class SDMDataObjectParser
  
  
} // namespace asdmbinaries
#endif // HeaderParser_CLASS
