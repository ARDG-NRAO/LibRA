#ifndef SDMDataObjectStreamReader_CLASS
#define SDMDataObjectStreamReader_CLASS
#include <utility>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <map>
#include <set>
#include <bitset>
#ifndef WITHOUT_BOOST
#include <boost/algorithm/string.hpp>
#endif
#include <libxml/parser.h>
#include <libxml/tree.h>

#ifndef WITHOUT_BOOST
#include "boost/filesystem/operations.hpp"
#include <boost/regex.hpp>
#endif

#include <alma/ASDMBinaries/SDMDataObjectParser.h>
#include <alma/ASDMBinaries/SDMDataObject.h>
#include <alma/Enumerations/CPrimitiveDataType.h>

namespace asdmbinaries {
  class SDMDataObjectStreamReaderException {
  public:
    SDMDataObjectStreamReaderException();
    SDMDataObjectStreamReaderException(const std::string& message);
  
    const std::string& getMessage();
  
  private:
    std::string message;
  };
 
  class SDMDataObjectStreamReader {
  public:
    SDMDataObjectStreamReader() ;
    virtual ~SDMDataObjectStreamReader() ;
    /**
     * Open a file expected to contain BDF data, read and parse the global header.
     * On exit the first block of data (subscan, integration or subintegration)  is ready to be read by the getData method.
     *
     * @throws SDMDataObjectStreamReaderException, SDMDataObjectException, SDMDataObjectParserException.
     */
    void	open(const std::string& path);

    /**
     * Returns the current position in the BDF file.
     *
     * or -1 if it fails to determine the position.
     *
     * @return an int64_t 
     */
    int64_t position();


    /**
     * Set the read position in the BDF file.
     *
     */
    void position(int64_t p);
    
    /**
     * Close the file containing the BDF data and releases all the memory dynamically allocated.
     */
    void close();

    /**
     * Returns the current position in bytes in the file of the current block of data (subscan, integration or subintegration) .
     *
     * @return a long long unsigned int.
     */
    unsigned long long currentIntegrationStartsAt() const;

    /**
     * Returns the index of the current block of data (subscan, integration or subintegration). 
     * The indexing is 0 based.
     * 
     * @return an unsigned int.
     */
    unsigned int currentIntegrationIndex() const;

    /**
     * Returns the title of the SDMDataObject.
     * @return a string.
     */
    std::string title() const;

    /**
     * Returns the byte order of the SDMDataObject.
     * @return a pointer to a ByteOrder instance.
     */
    const ByteOrder* byteOrder() const ;

    /**
     * Returns the start time.
     * @return a long long.
     */
    unsigned long long startTime() const;

    /**
     * Returns the number of (sub) integrations.
     * @return an unsigned int.
     *
     */
    unsigned int numTime() const;

    /**
     * Returns the dataOID.
     * @return a string.
     */
    std::string dataOID() const;

    /**
     * Returns the UID of the ExecBlock.
     * @return a string.
     */
    std::string execBlockUID() const;

    /**
     * Returns the number of the ExecBlock.
     * @return an unsigned int.
     */
    unsigned int execBlockNum() const;

    /**
     * Returns the number of the scan.
     * @return an unsigned int.
     */
    unsigned int scanNum() const;

    /**
     * Returns the number of the subscan.
     * @return an unsigned int.
     */
    unsigned int subscanNum() const;

    /**
     * Returns the project path.
     * The project path is a string of the form "/<s>execBlockNum</s>/<s>scanNum</s>/<s>subscanNum</s>"
     */
    std::string projectPath() const;

    /**
     * Returns the number of antenna.
     * @return an unsigned int.
     */
    unsigned int numAntenna() const;

    /**
     * Returns the correlation mode.
     * @return a value from enumeration CorrelationMode.
     */
    CorrelationModeMod::CorrelationMode correlationMode() const;


    /**
     * Returns the spectral resolution.
     * Due to this optional nature, the spectral resolution type is not returned directly as a literal of the enumeration
     * SpectralResolutionType, but as an instance of the class OptionalSpectralResolutionType. This instance can be queried to
     * check if the spectral resolution type information is present and if it is its value as an SpectralResolutionType literal.
     *
     * @return a value from enumeration SpectralResolutionType.
     */
    OptionalSpectralResolutionType spectralResolutionType() const;

    /**
     * Returns the processor type.
     * @return a value from the enumeration ProcessorType.
     */
    ProcessorTypeMod::ProcessorType processorType() const;


    /**
     * Returns the correlator type.
     *
     * @return a value from the enumeration CorrelatorType if processorType == CORRELATOR else an SDMDataObjectException is thrown.
     * @throw SDMDataException
     */
    CorrelatorTypeMod::CorrelatorType correlatorType() const;

    /**
     * Returns true if the binary data are all packed in one subset or distributed in a sequence 
     * of subsets (supposedly) along the time axis.
     *
     * @return a boolean
     */
    bool hasPackedData() const;

    /**
     * Returns true if the data are total power data and false otherwise.
     * @return a bool.
     *
     * @note data are considered as total power data if CorrelationMode == AUTO_ONLY && SpectralResolutionType == BASEBAND_WIDE.
     */
    bool isTP() const ;

    /**
     * Returns true if the data are WVR data and false otherwise.
     * @return a bool.
     *
     * @note data are considered as total power data if CorrelationMode == AUTO_ONLY && SpectralResolutionType == BASEBAND_WIDE.
     */
    bool isWVR() const ;


    /**
     * Returns true if the data are correlator data and false otherwise.
     * @return a bool.
     *
     * @note data are considered as correlator data if SpectralResolutionType != BASEBAND_WIDE.
     */
    bool isCorrelation() const;

    /**
     * Returns the structure of the data.
     * @returns a reference to a DataStruct.
     */
    const SDMDataObject::DataStruct& dataStruct() const;

    /**
     * Returns true if the observation has been aborted.
     * This method must be used on an SDMDataObject containing correlator data,otherwise a SDMDataObjectException is thrown.
     *
     * @return a bool.
     * 
     * @throw SDMDataObjectException.
     */
    bool aborted() const;

    /**
     * Returns the time, as an unsigned long long, at which the observation has been aborted.
     * The returned value is significant only if the observation has been aborted, therefore the method must always be used in 
     * conjuction with the aborted method. This method must be used on an SDMDataObject containing correlator data,
     * otherwise a SDMDataObjectException is thrown.
     *
     * @return an unsigned long long.
     *
     * @throw SDMDataObjectException
     */
    unsigned long long abortTime() const;

    /**
     * Returns the reason, as a string, why the observation has been aborted.
     * The returned value is significant only if the observation has been aborted, therefore the method must always be used in 
     * conjuction with the aborted method. This method must be used on an SDMDataObject containing correlator data,
     * otherwise a SDMDataObjectException is thrown.
     *
     * @return a string.
     *
     * @throw SDMDataObjectException
     */
    std::string abortReason() const;

    /**
     * Returns a string representation of the global header of this SDMDataObject.
     * @return a string.
     */
    std::string toString() const;

    /**
     * Returns true as long as the mark announcing the end of the sequence of (sub)integrations
     * is not read in the stream.
     */
    bool			hasSubset();

    /*
     * Returns a reference to an SDMDataSubset.
     * This SDMDataSubset represents the (sub)integration read and parsed at the current position (see currentIntegrationIndex()
     * and currentIntegrationStartsAt()) in the sequence stored in the stream.
     * O
     * 
     * @return a reference to an SDMDataSubset
     */
    const SDMDataSubset&	getSubset();

    /*
     * Returns binary data found in the BDF file from the current location and contained in the next 
     * nDataSubsets at maximum.
     *
     * The result is returned as a reference to a vector of SDMDataSubset. Each element of this vector
     * is an instance of an SDMDataSubset obtained by a sequential read of the file opened with the method open
     * from the current position in that file.
     *
     * The size of the resulting vector determines how many SDMDataSubsets have been actually read. 
     *
     * @return const vector<SDMDataSubset>& 
     */
    const std::vector<SDMDataSubset>& nextSubsets(unsigned int nSubsets);

    /*
     * Returns all binary data found in the BDF file from the current location.
     *
     * The result is returned as a reference to a vector of SDMDataSubset. Each element of this vector
     * is an instance of an SDMDataSubset obtained by a sequential read of the file opened with the method open
     * from the current position in that file. 
     *
     * @return const vector<SDMDataSubset>& 
     */
    const std::vector<SDMDataSubset>& allRemainingSubsets();

    enum BINATTACHCODES {ACTUALDURATIONS=0, ACTUALTIMES=1, AUTODATA=2, FLAGS=3, CROSSDATA=4, ZEROLAGS=5};
    std::bitset<6> attachmentFlags;

  private:
    // Enumerations to manage the state of an instance of SDMDataObjectStreamReader.
    //
    enum States {S_NO_BDF, S_AT_BEGINNING, S_READING, S_AT_END};
    enum Transitions {T_OPEN, T_QUERY, T_TEST_END, T_READ, T_READ_NEXT, T_READ_ALL, T_CLOSE};

    // Private variables
    unsigned long long integrationStartsAt;
    unsigned int       integrationIndex;
    std::string	path;
    std::ifstream	f;
    States      currentState;
    std::string	currentLine;
    std::string	boundary_1;
    std::string      boundary_2;
    bool        opened;

    std::map<std::string, int64_t>	binaryPartSize;
    std::set<std::string>			s_partNames;
    char*			actualTimesBuffer;
    char*			actualDurationsBuffer;
    char*			autoDataBuffer;
    char*			crossDataBuffer;
    char*			flagsBuffer;
    char*			zeroLagsBuffer;

    SDMDataObjectParser   parser;
    SDMDataObject         sdmDataObject;
    SDMDataSubset         sdmDataSubset;
    std::vector<SDMDataSubset> remainingSubsets;
    std::vector<SDMDataSubset> someSubsets;

    // Private methods
    void                        checkState(Transitions t, const std::string& methodName) const; 
    std::string			nextLine();
    std::pair<std::string, std::string>	headerField2Pair(const std::string& hf);
    std::pair<std::string, std::string>	requireHeaderField(const std::string& hf);
    std::string			requireMIMEHeader();
    std::string			requireBoundaryInCT(const std::string& ctValue);
    void                        skipAsLongAsLineStartsWith(const std::string& start);
    void			skipUntilEmptyLine(int maxSkips);  
    std::string			accumulateUntilBoundary(const std::string& boundary, int maxLines);
    void			requireBoundary(const std::string&, int maxLines);
    void			lookForBinaryPartSize(xmlNode*  aNode);
    std::string			requireCrossDataType(xmlNode* parent);

    void        printElementNames(xmlNode * a_node);
    void        requireSDMDataHeaderMIMEPart();
    void        requireSDMDataSubsetMIMEPart(SDMDataSubset& sdmDataSubset);

    void        releaseMemory(SDMDataSubset & sdmDataSubset);
  };
} // end namespace asdmbinaries

#endif 
