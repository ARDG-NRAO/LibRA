#ifndef SDMDataObjectReader_CLASS
#define SDMDataObjectReader_CLASS

#include <string>
#include <map>
#include <set>
#include <vector>
#include <bitset>

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

#include <alma/ASDMBinaries/SDMDataObjectParser.h>
#include <alma/ASDMBinaries/SDMDataObject.h>

/*
typedef long long          ACTUALDURATIONSTYPE;
typedef long long          ACTUALTIMESTYPE;     
typedef float              AUTODATATYPE;        
typedef int                LONGCROSSDATATYPE;   // mcaillat 
typedef short int          SHORTCROSSDATATYPE;
typedef float              FLOATCROSSDATATYPE;  
typedef unsigned int       FLAGSTYPE; // mcaillat
typedef float              ZEROLAGSTYPE;   
*/

namespace asdmbinaries {
  /**
   * A class to represent an exception thrown while reading a MIME message containing ALMA binary data.
   */ 
  class SDMDataObjectReaderException {
    
  public:
    /**
     * An empty contructor.
     */
    SDMDataObjectReaderException();

    /**
     * A constructor with a message associated with the exception.
     * @param m a string containing the message.
     */
    SDMDataObjectReaderException(std::string m);
    
    /**
     * The destructor.
     */
    virtual ~SDMDataObjectReaderException();

    /**
     * Returns the message associated to this exception.
     * @return a string.
     */
    std::string getMessage() const;
    
  protected:
    std::string message;
    
  };

  inline SDMDataObjectReaderException::SDMDataObjectReaderException() : message ("SDMDataObjectReaderException") {}
  inline SDMDataObjectReaderException::SDMDataObjectReaderException(std::string m) : message(m) {}
  inline SDMDataObjectReaderException::~SDMDataObjectReaderException() {}
  inline std::string SDMDataObjectReaderException::getMessage() const {
    return "SDMDataObjectReaderException : " + message;
  }

  /**
   * A class to read a MIME message containing ALMA binary data and provide a view on these binary data through an instance of SDMDataObject.
   * An instance of a SDMDataObjectReader can read MIME messages stored in a file or in a memory buffer, via two different versions of
   * the read() method.
   * The binary data read can be Total Power or Correlator (integration and subintegration) data.
   * Once the data obtained by a call to a read method are not needed any more the done() method must be called to release the resources allocated to 
   * access the data.
   */
  class SDMDataObjectReader {
  public:
    /**
     * An empty constructor.
     */
    SDMDataObjectReader();

    /**
     * The destructor.
     */
    virtual ~SDMDataObjectReader();

    /**
     * This method reads a MIME message contained in a disk file and returns a reference to an SDMDataObject.
     * @param filename the name of the file to be read.
     * @return a reference to an SDMDataObject built during the read operation.
     * @throw SDMDataObjectReaderException
     */

    const SDMDataObject & read(std::string filename);

    /**
     * This method reads a MIME message contained in a memory buffer and returns a reference to an SDMDataObject.
     * @param buffer the adress of the buffer containing the MIME message.
     * @param size the size in *bytes* of that buffer.
     * @param fromFile a bool to indicate if the buffer is actually the result of a mapping of a file to virtual memory (false by default).
     * @return a reference to an SDMDataObject built during the read operation.
     * @throw SDMDataObjectReaderException
     */
    const SDMDataObject & read(const char * buffer, unsigned long int size, bool fromFile=false);

    
    /**
     * This method returns a reference to the SDMDataObject instance built during the execution of 
     * the read method.
     * @return a reference to an SDMDataObject.
     * @throw SDMDataObjectReaderException.
     */
    const SDMDataObject & ref() const;
    
    /**
     * This method returns a pointer to the SDMDataObject instance built during the execution of 
     * the read method.
     * @return a reference to an SDMDataObject.
     * @throw SDMDataObjectReaderException.
     */
    const SDMDataObject * ptr() const;
    
    /**
     * A method to release the resources allocated by the read operation. 
     * This method must be called once the SDMDataObject built by the read method is no more needed.
     */
    void done();

  private:
    enum READ {UNKNOWN_, MEMORY_, FILE_};
    READ read_;

    int filedes_;
    unsigned int dataSize_;
    char* data_;
    std::string::size_type position_;
    std::string::size_type lastPosition_;
    std::string::size_type endPosition_;

    unsigned int integrationNum_;

    static const bool initClass_;
    static bool initClass();
    void init() ;
    std::string::size_type find(const std::string& s);
    bool compare(const std::string& s);
    bool EOD();

    // Two strings used as MIME boundaries
    static const std::string MIMEBOUNDARY_1;
    static const std::string MIMEBOUNDARY_2;
    
    // Regular expressions used to identify a Content-ID field in a MIME header
#ifndef WITHOUT_BOOST
    static const boost::regex CONTENTIDDATASTRUCTUREREGEXP;
    static const boost::regex CONTENTIDSUBSETREGEXP;
    static const boost::regex CONTENTIDBINREGEXP;
    static const boost::regex CONTENTIDBINREGEXP1;
    static const boost::regex CONTENTIDBINREGEXP2;
#else
    static const std::regex CONTENTIDDATASTRUCTUREREGEXP;
    static const std::regex CONTENTIDSUBSETREGEXP;
    static const std::regex CONTENTIDBINREGEXP;
    static const std::regex CONTENTIDBINREGEXP1;
    static const std::regex CONTENTIDBINREGEXP2;
#endif
    // Set of valid binary attachment names
    static std::set<std::string> BINATTACHNAMES;
    enum BINATTACHCODES {ACTUALDURATIONS=0, ACTUALTIMES=1, AUTODATA=2, FLAGS=3, CROSSDATA=4, ZEROLAGS=5};
    static std::map<std::string, BINATTACHCODES> name2code;

    std::string extractXMLHeader(const std::string& boundary);
    void tokenize(const std::string& str, std::vector<std::string>& tokens, const std::string& delimiters);
    void getFields(const std::string& header, std::map<std::string, std::string>& fields);
    std::string getContentID();
    std::string getContentLocation();

    void processMIME();
    void processMIMESDMDataHeader();
    void processMIMESDMDataSubsetHeader(SDMDataSubset& sdmDataSubset);
    void processMIMEIntegrations();
    void processMIMEIntegration();
    void processMIMESubscan();
    void processBinaryAttachment(const std::string& boundary, const SDMDataSubset& sdmDataSubset);


    // SDMDataObject related stuff
    SDMDataObjectParser parser_;
    SDMDataObject sdmDataObject_;
    const SDMDataObject& sdmDataObject();

    std::bitset<6> attachmentFlags;

    const ACTUALTIMESTYPE * actualTimes_;
    unsigned long int nActualTimes_;
    const ACTUALDURATIONSTYPE * actualDurations_;
    unsigned long int nActualDurations_;
    const ZEROLAGSTYPE * zeroLags_;
    unsigned long int nZeroLags_;
    const FLAGSTYPE * flags_;
    unsigned long int nFlags_;
    const INTCROSSDATATYPE  * longCrossData_;
    const SHORTCROSSDATATYPE * shortCrossData_;
    const FLOATCROSSDATATYPE * floatCrossData_;
    unsigned long int nCrossData_;
    const AUTODATATYPE * autoData_;
    unsigned long int nAutoData_;   

  };
} // namespace asdmbinaries
#endif // SDMDataObjectReader_CLASS
