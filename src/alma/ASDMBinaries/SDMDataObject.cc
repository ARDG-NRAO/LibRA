#include <alma/ASDMBinaries/SDMDataObject.h>
#include <alma/ASDMBinaries/SDMDataObjectParser.h>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <iostream>
#include <sstream>

#ifndef WITHOUT_BOOST
#include <boost/range/iterator_range.hpp>
#include <boost/algorithm/string/find.hpp>
#endif

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

  // Initialization section.
  //
  vector<string> SDMDataObject::axes;
  vector<string> SDMDataObject::correlationModeRefs;
  vector<string> SDMDataObject::types;
  vector<string> SDMDataObject::apcs;

  const bool SDMDataObject::_init = SDMDataObject::init();
  bool SDMDataObject::init() {
    return true;
  }

  // A regular expression to define the syntax of a spectral window identifier.
#ifndef WITHOUT_BOOST
  const boost::regex SDMDataObject::SPWID("[0-9]+");
#else
  const std::regex SDMDataObject::SPWID("[0-9]+");
#endif
  
  // SDMDataObject::SpectralWindow:: methods
  //
  SDMDataObject::SpectralWindow::SpectralWindow():
    scaleFactor_(0.0),
    numSpectralPoint_(0),
    numBin_(0)
  {owner_ = 0; strSw_ = ""; strImage_ = "";}
  
  SDMDataObject::SpectralWindow::~SpectralWindow() {;}
  
  SDMDataObject::SpectralWindow::SpectralWindow(const vector<StokesParameter>& crossPolProducts,
						float scaleFactor, unsigned int numSpectralPoint,
						unsigned int numBin,
						NetSidebandMod::NetSideband sideband):
    crossPolProducts_(crossPolProducts),
    scaleFactor_(scaleFactor),
    numSpectralPoint_(numSpectralPoint),
    numBin_(numBin),
    sideband_(sideband)
  {owner_ = 0;  strSw_ = ""; strImage_ = "";}
  
  SDMDataObject::SpectralWindow::SpectralWindow::SpectralWindow(const vector<StokesParameter>& sdPolProducts,
								unsigned int numSpectralPoint,
								unsigned numBin,
								NetSidebandMod::NetSideband sideband):
    sdPolProducts_(sdPolProducts),
    numSpectralPoint_(numSpectralPoint),
    numBin_(numBin),
    sideband_(sideband)
  {owner_ = 0; strSw_ = ""; strImage_ = "";}
  
  SDMDataObject::SpectralWindow::SpectralWindow(const vector<StokesParameter>& crossPolProducts,
						const vector<StokesParameter>& sdPolProducts,
						float scaleFactor,
						unsigned int numSpectralPoint,
						unsigned int numBin,
						NetSidebandMod::NetSideband sideband):
    crossPolProducts_(crossPolProducts),
    sdPolProducts_(sdPolProducts),
    scaleFactor_(scaleFactor),
    numSpectralPoint_(numSpectralPoint),
    numBin_(numBin),
    sideband_(sideband)
  {owner_ = 0; strSw_ = ""; strImage_ = "";}
  
  const vector<StokesParameter>& SDMDataObject::SpectralWindow::crossPolProducts() const { 
    if (owner_ && (owner_->isTP() || owner_->correlationMode() == AUTO_ONLY)) Utils::invalidCall("SDMDataObject::SpectralWindow::crossPolProducts", owner_);
    return crossPolProducts_; 
  }

  //  void SDMDataObject::SpectralWindow::crossPolProducts(const vector<StokesParameter>& value) { crossPolProducts_ = value; }

  const vector<StokesParameter>& SDMDataObject::SpectralWindow::sdPolProducts() const { 
    if (owner_ && (owner_->correlationMode() == CROSS_ONLY)) Utils::invalidCall("SDMDataObject::SpectralWindow::sdPolProducts", owner_);
    return sdPolProducts_; 
  }

  //  void SDMDataObject::SpectralWindow::sdPolProducts(const vector<StokesParameter>& value) { sdPolProducts_ = value; }

  float SDMDataObject::SpectralWindow::scaleFactor() const { 
    if (owner_ && owner_->correlationMode() == AUTO_ONLY) Utils::invalidCall("SDMDataObject::SpectralWindow::scaleFactor", owner_);    
    return scaleFactor_; 
  }
  //  void SDMDataObject::SpectralWindow::scaleFactor(float value )  { scaleFactor_ = value; }
  
  unsigned int SDMDataObject::SpectralWindow::numSpectralPoint() const { return numSpectralPoint_; }
  //  void SDMDataObject::SpectralWindow::numSpectralPoint(unsigned int value)  { numSpectralPoint_ = value; }

  unsigned int SDMDataObject::SpectralWindow::numBin() const { return numBin_; }
  //  void SDMDataObject::SpectralWindow::numBin(unsigned int value) { numBin_ = value; }

  void SDMDataObject::SpectralWindow::owner(const SDMDataObject* o) { owner_ = o; }

  NetSidebandMod::NetSideband SDMDataObject::SpectralWindow::sideband() const { return sideband_; }

  void SDMDataObject::SpectralWindow::strImage(const string& s) {
#ifndef WITHOUT_BOOST
    boost::cmatch what;
#else
    std::cmatch what;
#endif
    if ((s.size() == 0) || regex_match(s.c_str(), what, SDMDataObject::SPWID)) {
      strImage_ = s;
    }
    else
      throw SDMDataObjectException("SpectralWindow::strImage : '" + s + "' is an invalid string to identify a spectral window.");
  }

  const string& SDMDataObject::SpectralWindow::strImage() const { return strImage_; }

  void SDMDataObject::SpectralWindow::strSw(const string& s) {
#ifndef WITHOUT_BOOST
    boost::cmatch what;
#else
    std::cmatch what;
#endif
    if ((s.size() == 0) || regex_match(s.c_str(), what, SDMDataObject::SPWID)) {
      strSw_ = s;
    }
    else
      throw SDMDataObjectException("SpectralWindow::strSw :  '" + s + "' is an invalid string to identify a spectral window.");
  }

  const string& SDMDataObject::SpectralWindow::strSw() const { return strSw_; }

  // SDMDataObject::SpectralWindow::


  // SDMDataObject::Baseband methods
  //
  SDMDataObject::Baseband::Baseband() {owner_ = 0;}
  
  SDMDataObject::Baseband::Baseband(BasebandName name,
				    const vector<SpectralWindow>& spectralWindows):
    name_(name),
    spectralWindows_(spectralWindows) {owner_ = 0;}


  SDMDataObject::Baseband::~Baseband() {;}
  
  BasebandName SDMDataObject::Baseband::name() const { 
    return name_; 
  }

  //  void SDMDataObject::Baseband::ref(BasebandName value) { ref_ = value; }
  
  const vector<SDMDataObject::SpectralWindow>& SDMDataObject::Baseband::spectralWindows() const { return spectralWindows_; }
  void SDMDataObject::Baseband::spectralWindows(const vector<SDMDataObject::SpectralWindow>& value) { spectralWindows_ = value; }
  void SDMDataObject::Baseband::owner(const SDMDataObject* o) {owner_ = o; }
  // SDMDataObject::Baseband::

  // SDMDataObject::BinaryPart:: methods
  //
  SDMDataObject::BinaryPart::BinaryPart() {owner_ = 0; size_ = 0;}
  SDMDataObject::BinaryPart::~BinaryPart() {;}
  
  
  SDMDataObject::BinaryPart::BinaryPart( unsigned int size,
					 const vector<AxisName>& axes):
    size_(size),
    axes_(axes){owner_ = 0;}
  
  
  unsigned int SDMDataObject::BinaryPart::size() const { return size_; }
  //  void SDMDataObject::BinaryPart::size (unsigned int value) { size_ = value; }
  
  const vector<AxisName>& SDMDataObject::BinaryPart::axes() const { return axes_; }
  //  void SDMDataObject::BinaryPart::axes (const vector<AxisName>& axes) { axes_ = axes; }

  void SDMDataObject::BinaryPart::owner(const SDMDataObject* o) { owner_ = o; }
  // SDMDataObject::BinaryPart
  
  // SDMDataObject::AutoDataBinaryPart:: methods
  SDMDataObject::AutoDataBinaryPart::AutoDataBinaryPart(): BinaryPart() { normalized_ = false; }
  SDMDataObject::AutoDataBinaryPart::~AutoDataBinaryPart() {;}

  SDMDataObject::AutoDataBinaryPart::AutoDataBinaryPart(unsigned int size,
							const vector<AxisName>& axes,
							bool normalized) : BinaryPart(size, axes) {
    normalized_ = normalized;
  }

  bool SDMDataObject::AutoDataBinaryPart::normalized() const { return normalized_; }

  // SDMDataObject::AutoDataBinaryPart::

  // SDMDataObject::ZeroLagsBinaryPart:: methods
  SDMDataObject::ZeroLagsBinaryPart::ZeroLagsBinaryPart(): BinaryPart() { correlatorType_ = XF; }
  SDMDataObject::ZeroLagsBinaryPart::~ZeroLagsBinaryPart() {;}

  SDMDataObject::ZeroLagsBinaryPart::ZeroLagsBinaryPart(unsigned int size,
							const vector<AxisName>& axes,
							CorrelatorType correlatorType) : BinaryPart(size, axes) {
    correlatorType_ = correlatorType;
  }

  CorrelatorType SDMDataObject::ZeroLagsBinaryPart::correlatorType() const { return correlatorType_; }

  // SDMDataObject::ZeroLagsBinaryPart::

  // SDMDataObject::DataStruct:: methods
  //

  /**
   * The empty constructor.
   *
   * @note This constructor should never be used.
   */
  SDMDataObject::DataStruct::DataStruct() {owner_ = 0;}

  /**
   * The destructor.
   */
  SDMDataObject::DataStruct::~DataStruct() {;}


  /** 
   * The full constructor.
   */
  SDMDataObject::DataStruct::DataStruct(const vector<AtmPhaseCorrection>& apc,
					const vector<Baseband>& basebands,
					const BinaryPart& flags,
					const BinaryPart& actualTimes,
					const BinaryPart& actualDurations,
					const ZeroLagsBinaryPart& zeroLags,
					const BinaryPart& crossData,
					const AutoDataBinaryPart& autoData
					):apc_(apc),
					  basebands_(basebands),
					  flags_(flags),
					  actualTimes_(actualTimes),
					  actualDurations_(actualDurations),
					  zeroLags_(zeroLags),
					  crossData_(crossData),
					  autoData_(autoData)
  {
    owner_ = 0;
  }      
  const vector<AtmPhaseCorrection>& SDMDataObject::DataStruct::apc() const { return apc_; }
  //  void SDMDataObject::DataStruct::apc(const vector<AtmPhaseCorrection>& value) { apc_ = value; }
  
  const vector<SDMDataObject::Baseband>& SDMDataObject::DataStruct::basebands() const { return basebands_; }
  //  void SDMDataObject::DataStruct::basebands(const vector<SDMDataObject::Baseband>& value) { basebands_ = value; }
  
  const SDMDataObject::BinaryPart& SDMDataObject::DataStruct::flags() const { return flags_;}
  //  void SDMDataObject::DataStruct::flags(const SDMDataObject::BinaryPart& value) { flags_ = value; }
  
  const SDMDataObject::BinaryPart& SDMDataObject::DataStruct::actualTimes() const { return actualTimes_;}
  //  void SDMDataObject::DataStruct::actualTimes(const SDMDataObject::BinaryPart& value) { actualTimes_ = value; }
 
  const SDMDataObject::BinaryPart& SDMDataObject::DataStruct::actualDurations() const { return actualDurations_;}
  //  void SDMDataObject::DataStruct::actualDurations(const SDMDataObject::BinaryPart& value) { actualDurations_ = value; }
  
  const SDMDataObject::ZeroLagsBinaryPart& SDMDataObject::DataStruct::zeroLags() const { 
    if (owner_ && owner_->isTP()) Utils::invalidCall("SDMDataObject::DataStruct::zeroLags", owner_);
    return zeroLags_;
  } 

  //  void SDMDataObject::DataStruct::zeroLags(const SDMDataObject::BinaryPart& value) { 
  //    if (owner_ && owner_->isTP()) Utils::invalidCall("SDMDataObject::DataStruct::zeroLags", owner_);
  //    zeroLags_ = value;
  //  }

  const SDMDataObject::BinaryPart& SDMDataObject::DataStruct::crossData() const { return crossData_;}
  //  void SDMDataObject::DataStruct::crossData(const SDMDataObject::BinaryPart& value) { crossData_ = value; }
  
  const SDMDataObject::AutoDataBinaryPart& SDMDataObject::DataStruct::autoData() const { return autoData_;} 
  //  void SDMDataObject::DataStruct::autoData(const SDMDataObject::BinaryPart& value) { autoData_ = value; }
  void SDMDataObject::DataStruct::owner(const SDMDataObject* o) { owner_ = o; }

  void SDMDataObject::DataStruct::checkCoordinate(unsigned int ibb, unsigned int ispw) const {
    ostringstream oss;

    if (ibb >= basebands_.size()) {
      oss << "In '("<< ibb << "," << ispw <<")' '" << ibb << "' is invalid as a baseband index, it should not exceed '" << basebands_.size()-1 << "'";
      throw SDMDataObjectException(oss.str());
    }
     
    if (ispw >= basebands_.at(ibb).spectralWindows().size()) {
      oss << "In '("<< ibb << "," << ispw <<")' '" << ispw << "' is invalid as a spectral window index, it should not exceed '" << basebands_.at(ibb).spectralWindows().size()-1 << "'";
    }    
  }

  bool SDMDataObject::DataStruct::associatedSPW(unsigned int ibb,
						unsigned int ispw1,
						unsigned int ispw2) {
    checkCoordinate(ibb, ispw1);
    checkCoordinate(ibb, ispw2);

    ostringstream oss;
    oss << ibb << " " << ispw1;
    string key = oss.str();

    oss.str("");
    
    oss << ibb << " " << ispw2;
    string value = oss.str();
    
    map<string, string>::iterator iter1 = imageSPW_.find(key);
    map<string, string>::iterator iter2 = imageOfSPW_.find(key);

    return ( ((iter1 != imageSPW_.end()) && ((iter1->second).compare(value) == 0))
	     ||
	     ((iter2 != imageOfSPW_.end()) && ((iter2->second).compare(value) == 0))
	     );
  }

  void SDMDataObject::DataStruct::divorceSPW(unsigned int ibb, unsigned int ispw) {
    checkCoordinate(ibb, ispw);

    ostringstream oss;
    oss << ibb << " " << ispw; 
    string key = oss.str();
    map<string, string>::iterator iter0, iter1;
    if ((iter0 = imageSPW_.find(key)) != imageSPW_.end()) {
      if ((iter1 = imageOfSPW_.find(iter0->second)) != imageOfSPW_.end()) {
	imageOfSPW_.erase(iter1);
      }
      imageSPW_.erase(iter0);
    }
  }

  void SDMDataObject::DataStruct::imageSPW(unsigned int ibb,
					   unsigned int ispw1,
					   unsigned int ispw2) {
    checkCoordinate(ibb, ispw1);
    checkCoordinate(ibb, ispw2);
    
    if (!associatedSPW(ibb, ispw1, ispw2)) {
      divorceSPW(ibb, ispw1);
      divorceSPW(ibb, ispw2);
    }

    ostringstream oss;

    oss << ibb << " " << ispw1;
    string key = oss.str();

    oss.str("");
    oss << ibb << " " << ispw2;
    string value = oss.str();
    imageSPW_[key] = value;
    imageOfSPW_[value] = key;
  }

  void SDMDataObject::DataStruct::imageOfSPW(unsigned int ibb,
					     unsigned int ispw1,
					     unsigned int ispw2) {
    imageSPW(ibb,ispw2,ispw1);
  }

  const SDMDataObject::SpectralWindow* SDMDataObject::DataStruct::imageSPW(unsigned int ibb,
									   unsigned int ispw) const {
    checkCoordinate(ibb, ispw);

    ostringstream oss;

    oss << ibb << " " << ispw;
    string key = oss.str();

    map<string, string>::const_iterator iter;
    if ((iter = imageSPW_.find(key)) == imageSPW_.end())
      return 0;

    unsigned int ibbImage;
    unsigned int ispwImage;

    istringstream iss(iter->second);
    iss >> ibbImage;
    iss >> ispwImage;
    checkCoordinate (ibbImage, ispwImage);

    return &(basebands_.at(ibbImage).spectralWindows().at(ispwImage));
  }


  const SDMDataObject::SpectralWindow* SDMDataObject::DataStruct::imageOfSPW(unsigned int ibb,
									     unsigned int ispw) const {
    checkCoordinate(ibb, ispw);

    ostringstream oss;

    oss << ibb << " " << ispw;
    string key = oss.str();

    map<string, string>::const_iterator iter;
    if ((iter = imageOfSPW_.find(key)) == imageOfSPW_.end())
      return 0;

    unsigned int ibbOf;
    unsigned int ispwOf;

    istringstream iss(iter->second);
    iss >> ibbOf;
    iss >> ispwOf;
    checkCoordinate (ibbOf, ispwOf);
    
    return &(basebands_.at(ibbOf).spectralWindows().at(ispwOf));
  }
  

  // SDMDataObject::DataStruct

  // SDMDataObject:: methods
  //

#define TSTVALID() if (valid_ == false) throw SDMDataObjectException("no valid binary data in this SDMDataObject.");


  SDMDataObject::SDMDataObject() {numTime_ = 0; valid_  = false; aborted_ = false; }
  SDMDataObject::SDMDataObject(unsigned long long startTime,
			       const string& dataOID,
			       unsigned int dimensionality,
			       const string& execBlockUID, 
			       unsigned int execBlockNum,
			       unsigned int scanNum,
			       unsigned int subscanNum,
			       unsigned int numAntenna,
			       CorrelationMode correlationMode,
			       const SDMDataObject::DataStruct& dataStruct):
    startTime_(startTime),
    dataOID_(dataOID),
    dimensionality_(dimensionality),
    numTime_(0),
    execBlockUID_(execBlockUID),
    execBlockNum_(execBlockNum),
    scanNum_(scanNum),
    subscanNum_(subscanNum),
    numAntenna_(numAntenna),
    correlationMode_(correlationMode),
    dataStruct_(dataStruct){
    valid_ = true;
    aborted_ = false;
  }
  
  
  SDMDataObject::SDMDataObject(unsigned long long startTime,
			       const string& dataOID,
			       unsigned int dimensionality,
			       unsigned int numTime,
			       const string& execBlockUID,
			       unsigned int execBlockNum,
			       unsigned int scanNum,
			       unsigned int subscanNum,
			       unsigned int numAntenna,
			       const SDMDataObject::DataStruct& dataStruct):
    startTime_(startTime),
    dataOID_(dataOID),
    dimensionality_(dimensionality),
    numTime_(numTime),
    execBlockUID_(execBlockUID),
    execBlockNum_(execBlockNum),
    scanNum_(scanNum),
    subscanNum_(subscanNum),
    numAntenna_(numAntenna),
    dataStruct_(dataStruct)
  { valid_ = true; aborted_ = false;}
  
  string SDMDataObject::title() const { TSTVALID(); return title_; }
  void SDMDataObject::title(const string& value) { title_ = value; }

  const ByteOrder* SDMDataObject::byteOrder() const { TSTVALID(); return byteOrder_; }

  unsigned long long SDMDataObject::startTime() const {TSTVALID(); return startTime_; }
  void SDMDataObject::startTime(unsigned long long value) { startTime_ = value; }
  
  unsigned int SDMDataObject::numTime() const  {TSTVALID(); return numTime_; }
  void SDMDataObject::numTime(unsigned int value) { numTime_ = value; }
  
  string SDMDataObject::dataOID() const {TSTVALID(); return dataOID_; }
  void SDMDataObject::dataOID(const string& value) { dataOID_ = value; }
  
  unsigned int SDMDataObject::dimensionality() const  {TSTVALID(); return dimensionality_; }
  void SDMDataObject::dimensionality( unsigned int value ) { dimensionality_ = value; }

  string SDMDataObject::execBlockUID() const {TSTVALID(); return execBlockUID_; }
  void SDMDataObject::execBlockUID (const string& value) { execBlockUID_ = value; }
  
  unsigned int SDMDataObject::execBlockNum() const  {TSTVALID(); return execBlockNum_; }
  void SDMDataObject::execBlockNum (unsigned int value ) { execBlockNum_ = value; }
  
  unsigned int SDMDataObject::scanNum() const  {TSTVALID(); return scanNum_ ; }
  void SDMDataObject::scanNum( unsigned int value) { scanNum_ = value; }
  
  unsigned int SDMDataObject::subscanNum() const {TSTVALID(); return subscanNum_; }
  void SDMDataObject::subscanNum(int value) { subscanNum_ = value; }
  
  string SDMDataObject::projectPath() const {
    TSTVALID();
    ostringstream oss;
    oss << execBlockNum_ << "/" << scanNum_ << "/" << subscanNum_ << "/";
    return oss.str();
  }

  unsigned int SDMDataObject::numAntenna() const  {TSTVALID();  return numAntenna_; }
  void SDMDataObject::numAntenna (unsigned int value) { numAntenna_ = value; }
  
  const SDMDataObject::DataStruct& SDMDataObject::dataStruct() const {TSTVALID(); return dataStruct_; }
  void SDMDataObject::dataStruct(const SDMDataObject::DataStruct& value) {dataStruct_ = value; }

  CorrelationMode SDMDataObject::correlationMode() const  {TSTVALID(); return correlationMode_; }

  OptionalSpectralResolutionType SDMDataObject::spectralResolutionType() const {TSTVALID(); return spectralResolutionType_; }
  ProcessorType SDMDataObject::processorType() const {TSTVALID(); return processorType_; }
  CorrelatorType SDMDataObject::correlatorType() const {
    TSTVALID(); 
    if (processorType_ == CORRELATOR) 
      return dataStruct_.zeroLags_.correlatorType_ ;
    else
      throw SDMDataObjectException("no correlator type defined in the current context (processor type = '" +
				   CProcessorType::name(processorType_));
  }


  bool SDMDataObject::inTitle(const std::string&  what) const {TSTVALID();
    // string s(title_);
    // boost::iterator_range<std::string::iterator> range = boost::algorithm::ifind_first(s, what);
    // return !range.empty();
    return title_.find(what) != std::string::npos;
  }

  /*
   * Due to the fact that the value of processor type in the global header does not allow to really determine which processor has produced the data
   * (this is particularly true for ALMA_RADIOMETER and SQUARE_LAW_DETECTOR which fall under the same type PROCESSOR, I decided to use
   * the content of the title to make a clear distinction. Of course this is based on a non said conventions used to form the title...
   */ 
  bool SDMDataObject::isTP() const {TSTVALID(); return inTitle("Total Power");}
  bool SDMDataObject::isWVR() const {TSTVALID(); return inTitle("WVR");}
  bool SDMDataObject::isCorrelation() const {TSTVALID(); return processorType() == CORRELATOR;}


  /*
   * hasPackedData returns true if all the integrations are grouped in one subset for all the timestamps and conversely false if 
   * there is one subset per integration (i.e. per timestamp).
   */
  bool SDMDataObject::hasPackedData() const {TSTVALID(); return dimensionality() == 0;}
  
  const vector<SDMDataSubset>& SDMDataObject::sdmDataSubsets() const {
    TSTVALID();
    return dataSubsets_;
  }

  const vector<SDMDataSubset>& SDMDataObject::corrDataSubsets() const {
    TSTVALID();
    if (isTP()) Utils::invalidCall("corrDataSubsets", this);
    return dataSubsets_;
  }

  const SDMDataSubset& SDMDataObject::sdmDataSubset(const string& projectPath) const {
    TSTVALID();

    map<string, unsigned int>::const_iterator iter;
    iter = str2index_.find(projectPath);
    if (iter == str2index_.end())
      throw SDMDataObjectException("dataSubset with project path '" + projectPath + "' not found.");
    return dataSubsets_.at(iter->second);
  }



  bool SDMDataObject::aborted() const {
    if (isTP()) Utils::invalidCall("SDMDataObject::aborted", this);
    
    return aborted_;
  }

  unsigned long long SDMDataObject::abortTime() const {
    if (isTP()) Utils::invalidCall("SDMDataObject::abortTime", this);

    return abortTime_;

  }

  string SDMDataObject::abortReason() const {
    if (isTP()) Utils::invalidCall("SDMDataObject::abortReason", this);

    return abortReason_;

  }

  const SDMDataSubset& SDMDataObject::tpDataSubset() const {
    TSTVALID();
    if (isCorrelation()) Utils::invalidCall("tpDataSubset", this);
    return dataSubsets_.at(0);
    //return str2index_.begin()->second;
  }

  void SDMDataObject::tpDataSubset(const SDMDataSubset& value) {
    if (isCorrelation()) Utils::invalidCall("tpDataSubset", this);
    dataSubsets_.push_back(value);

    string projectPath = value.projectPath();
    if (SDMDataObjectParser::trim(projectPath).size() == 0)
      throw (SDMDataObjectException("can't accept a data subset without a project path"));

    str2index_[projectPath] = dataSubsets_.size() - 1;
  }
  
  vector<string> SDMDataObject::projectPaths() const {
    vector<string> result(str2index_.size());

    map<string, unsigned int>::const_iterator iter = str2index_.begin();
 
    for ( ; iter != str2index_.end(); iter++)
      result[iter->second] = iter->first;
    
    return result;
  }
 
  void SDMDataObject::done() {
    valid_ = false;
    
    str2index_.clear();
    dataSubsets_.clear();
  }

  void SDMDataObject::append(const SDMDataSubset& value) {
    TSTVALID();
    dataSubsets_.push_back(value);
    str2index_[value.projectPath()] = dataSubsets_.size()-1;
    numTime_ = dataSubsets_.size();
  }
  
  void SDMDataObject::owns() {
    // I own my correlationMode
    dataStruct_.owner_ = this;

    // I own my basebands and their spectral windows
    for (unsigned int i = 0; i < dataStruct_.basebands_.size(); i++) {
      Baseband& bb = dataStruct_.basebands_.at(i);
      bb.owner_ = this;
      
      for (unsigned int j = 0 ; j < bb.spectralWindows_.size(); j++) {
	bb.spectralWindows_.at(j).owner_ = this;
      }
    }

    // I own my binary part descriptions
    // (let's all of them, even those which are not relevant)
    dataStruct_.flags_.owner_   = this;
    dataStruct_.actualTimes_.owner_     = this;
    dataStruct_.actualDurations_.owner_ = this;
    dataStruct_.zeroLags_.owner_        = this;
    dataStruct_.crossData_.owner_       = this;
    dataStruct_.autoData_.owner_        = this;

    // I own my dataSubsets
    for (unsigned int i = 0; i < dataSubsets_.size(); i++)
      dataSubsets_.at(i).owner_ = this;
  }


#define FIRST_JAN_2007 4674326400000000000LL
  
  string SDMDataObject::toString() const {
    TSTVALID();
    ostringstream result;
    //ptime t1(date(2007, Jan, 1), time_duration(0, 0, 0,(startTime_ - FIRST_JAN_2007)/1000 ));
    result << "SDMDataObject at address '" << this << "' :" << endl; 
    result << "XML Schema version = " << schemaVersion_ << endl;
    result << "Byte order = " << byteOrder_->toString() << endl;
    //result << "startTime = " << startTime_ << " (" << to_simple_string(t1) << ")" << endl;
    result << "startTime = " << startTime_ << endl;
    result << "dataOID = " << dataOID_ << endl;
    result << "title = " << title_ << endl;
    if (dimensionality_==0) 
      result << "numTime = " << numTime_ << endl;
    else
      result << "dimensionality = " << dimensionality_ << endl;
    result << "execBlockUID = " << execBlockUID_ << endl;
    result << "execBlockNum = " << execBlockNum_ << endl;
    result << "scanNum = " << scanNum_ << endl;
    result << "subscanNum = " << subscanNum_ << endl;
    result << "numAntenna = " << numAntenna_ << endl;
    result << "correlationMode = " << CCorrelationMode::name(correlationMode_) << endl;
    if (spectralResolutionType_.present()) 
      result << "spectralResolutionType = " << CSpectralResolutionType::name(spectralResolutionType_.literal()) << endl;
    result << "processorType = " << CProcessorType::name(processorType_) << endl;
    if ( correlationMode_ != AUTO_ONLY ) {
      result << "atmospheric phase correction = " << TOSTRING<AtmPhaseCorrection, CAtmPhaseCorrection>(dataStruct_.apc_) << endl;
    } 
    const vector<Baseband>& bbs = dataStruct_.basebands();
    for (unsigned int i = 0; i < bbs.size(); i++) {
      result << "baseband #" << i << ":" << endl;
      result << "\tname = " << CBasebandName::name(bbs.at(i).name()) << endl;
      const vector<SpectralWindow>& spWs = bbs.at(i).spectralWindows();
      for (unsigned int j = 0; j < spWs.size(); j++) {
	result << "\tspectralWindow #" << j << ":" << endl;
	
	SpectralWindow spW = spWs.at(j);
	result << "\t\t" << "sw = " << spW.strSw() << endl;
	switch (correlationMode_) {
	case CROSS_ONLY :
	  result << "\t\t" << "crossPolProducts = " << TOSTRING<StokesParameter, CStokesParameter>(spW.crossPolProducts()) << endl;
	  result << "\t\t" << "scaleFactor = " << spW.scaleFactor() << endl;
	  result << "\t\t" << "numSpectralPoint = " << spW.numSpectralPoint() << endl;
	  result << "\t\t" << "numBin = " << spW.numBin() << endl;
	  break;
	  
	case AUTO_ONLY :
	  result << "\t\t" << "sPolProducts = " << TOSTRING<StokesParameter, CStokesParameter>(spW.sdPolProducts()) << endl;
	  result << "\t\t" << "numSpectralPoint = " << spW.numSpectralPoint() << endl;
	  result << "\t\t" << "numBin = " << spW.numBin() << endl;
	  break;

	case CROSS_AND_AUTO :
	  result << "\t\t" << "crossPolProducts = " << TOSTRING<StokesParameter, CStokesParameter>(spW.crossPolProducts()) << endl;
	  result << "\t\t" << "sdPolProducts = " << TOSTRING<StokesParameter, CStokesParameter>(spW.sdPolProducts()) << endl;
	  result << "\t\t" << "scaleFactor = " << spW.scaleFactor() << endl;
	  result << "\t\t" << "numSpectralPoint = " << spW.numSpectralPoint() << endl;
	  result << "\t\t" << "numBin = " << spW.numBin() << endl;
	  
	  break;
	  
	default:
	  break;
	}
	result << "\t\t" << "sideband = " << TOSTRING<NetSideband, CNetSideband>(spW.sideband()) << endl;
	if (spW.strSw().size() > 0)    
	if (spW.strImage().size() > 0) result << "\t\t" << "image = " << spW.strImage() << endl;
      }
    }
        
    result << "flags:" << endl;
    result << "\tsize = " << dataStruct_.flags().size() << endl;
    result << "\taxes = " << TOSTRING<AxisName, CAxisName>(dataStruct_.flags().axes()) << endl; 
    
    result << "actualTimes:" << endl;
    result << "\tsize = " << dataStruct_.actualTimes().size() << endl;
    result << "\taxes = " << TOSTRING<AxisName, CAxisName>(dataStruct_.actualTimes().axes()) << endl; 
    
    result << "actualDurations:" << endl;
    result << "\tsize = " << dataStruct_.actualDurations().size() << endl;
    result << "\taxes = " << TOSTRING<AxisName, CAxisName>(dataStruct_.actualDurations().axes()) << endl;
    
    if ((processorType_ == CORRELATOR)
	&& (dataStruct_.zeroLags_.correlatorType_ != FX)
	&& (dataStruct_.zeroLags_.size_ != 0)) {
      result << "zeroLags:" << endl;
      result << "\tsize = " << dataStruct_.zeroLags().size() << endl;
      result << "\taxes = " << TOSTRING<AxisName, CAxisName>(dataStruct_.zeroLags().axes()) << endl;
      result << "\tcorrelatorType = " << TOSTRING<CorrelatorType, CCorrelatorType>(dataStruct_.zeroLags_.correlatorType_) << endl;
    }

    switch (correlationMode_) {
    case CROSS_ONLY :
      result << "crossData:" << endl;
      result << "\tsize = " << dataStruct_.crossData().size() << endl;
      result << "\taxes = " << TOSTRING<AxisName, CAxisName> (dataStruct_.crossData().axes()) << endl;
      break;
      
    case AUTO_ONLY :
      result << "autoData:" << endl;
      result << "\tsize = " << dataStruct_.autoData().size() << endl;
      result << "\taxes = " << TOSTRING<AxisName, CAxisName>(dataStruct_.autoData().axes()) << endl;
      result << "\tnormalized = " << (dataStruct_.autoData_.normalized_ ? "true" : "false") << endl;
      break;
      
    case CROSS_AND_AUTO :
      result << "crossData:" << endl;
      result << "\tsize = " << dataStruct_.crossData().size() << endl;
      result << "\taxes = " << TOSTRING<AxisName, CAxisName>(dataStruct_.crossData().axes()) << endl;
      
      result << "autoData:" << endl;
      result << "\tsize = " << dataStruct_.autoData().size() << endl;
      result << "\taxes = " << TOSTRING<AxisName, CAxisName>(dataStruct_.autoData().axes()) << endl;
      result << "\tnormalized = " << (dataStruct_.autoData_.normalized_ ? "true" : "false") << endl;
      break;
	
    default:
      break;
    }
    return result.str();
  }


  void SDMDataObject::toXML(const BinaryPart& binaryPart, const string& elementName, ostringstream& oss) const {
    oss << "<" << elementName ;
    oss << " " << HeaderParser::SIZE << "=" << QUOTE(binaryPart.size())
	<< " " << HeaderParser::AXES << "=" << QUOTE<AxisName, CAxisName>(binaryPart.axes())
	<< "/>"
	<< endl;
  }

  void SDMDataObject::toXML(const AutoDataBinaryPart& autoDataBinaryPart, const string& elementName, ostringstream& oss) const {
    oss << "<" << elementName ;
    oss << " " << HeaderParser::SIZE << "=" << QUOTE(autoDataBinaryPart.size())
	<< " " << HeaderParser::AXES << "=" << QUOTE<AxisName, CAxisName>(autoDataBinaryPart.axes())
	<< " " << HeaderParser::NORMALIZED << "=" << QUOTE(autoDataBinaryPart.normalized())
	<< "/>"
	<< endl;
  }

  void SDMDataObject::toXML(const ZeroLagsBinaryPart& zeroLagsBinaryPart, const string& elementName, ostringstream& oss) const {
    oss << "<" << elementName ;
    oss << " " << HeaderParser::SIZE << "=" << QUOTE(zeroLagsBinaryPart.size())
	<< " " << HeaderParser::AXES << "=" << QUOTE<AxisName, CAxisName>(zeroLagsBinaryPart.axes())
	<< " " << HeaderParser::CORRELATORTYPE << "=" << QUOTE<CorrelatorType, CCorrelatorType>(zeroLagsBinaryPart.correlatorType())
	<< "/>"
	<< endl;
  }

  void SDMDataObject::spectralWindowsToXML(const vector<Baseband>& basebands,unsigned int ibb,  ostringstream& oss) const {
    const vector<SDMDataObject::SpectralWindow>& spectralWindows = basebands.at(ibb).spectralWindows();
    for (unsigned int i = 0; i < spectralWindows.size(); i++) {
      const SpectralWindow& spectralWindow = spectralWindows.at(i);
      oss << "<" << HeaderParser::SPECTRALWINDOW;
      oss << " " << HeaderParser::SW << "=" << QUOTE(i+1);
      oss << " " << HeaderParser::SWBB << "=" << QUOTE(CBasebandName::name(basebands.at(ibb).name()));
      
      switch (correlationMode_) {
      case CROSS_ONLY:
	oss << " " << HeaderParser::CROSSPOLPRODUCTS << "=" << QUOTE<StokesParameter, CStokesParameter>(spectralWindow.crossPolProducts()) ;
	oss << " " << HeaderParser::SCALEFACTOR << "=" << QUOTE(spectralWindow.scaleFactor()) ;
	oss << " " << HeaderParser::NUMSPECTRALPOINT << "=" << QUOTE(spectralWindow.numSpectralPoint()) ;
	oss << " " << HeaderParser::NUMBIN << "=" << QUOTE(spectralWindow.numBin()) ;
	break;
	
      case AUTO_ONLY:
	oss << " " << HeaderParser::SDPOLPRODUCTS << "=" << QUOTE<StokesParameter, CStokesParameter>(spectralWindow.sdPolProducts()) ;
	oss << " " << HeaderParser::NUMSPECTRALPOINT << "=" << QUOTE(spectralWindow.numSpectralPoint()) ;
	oss << " " << HeaderParser::NUMBIN << "=" << QUOTE(spectralWindow.numBin()) ;
	break;
	
      case CROSS_AND_AUTO:
	oss << " " << HeaderParser::CROSSPOLPRODUCTS << "=" << QUOTE<StokesParameter, CStokesParameter>(spectralWindow.crossPolProducts()) ;
	oss << " " << HeaderParser::SDPOLPRODUCTS << "=" << QUOTE<StokesParameter, CStokesParameter>(spectralWindow.sdPolProducts()) ;
	oss << " " << HeaderParser::SCALEFACTOR << "=" << QUOTE(spectralWindow.scaleFactor()) ;
	oss << " " << HeaderParser::NUMSPECTRALPOINT << "=" << QUOTE(spectralWindow.numSpectralPoint()) ;
	oss << " " << HeaderParser::NUMBIN << "=" << QUOTE(spectralWindow.numBin()) ;
	break;
      }

      oss << " " << HeaderParser::SIDEBAND << "=" << QUOTE<NetSideband, CNetSideband>(spectralWindow.sideband());

      if (spectralWindow.strImage().size() > 0) oss << " " << HeaderParser::IMAGE << "=" << QUOTE(::atoi(spectralWindow.strImage().c_str()));
      
      oss << "/>" << endl;
    }
  }

  void SDMDataObject::basebandsToXML(ostringstream& oss) const {
    for (unsigned int ibb = 0; ibb < dataStruct_.basebands_.size(); ibb++) {
      oss << "<" << HeaderParser::BASEBAND << " " << HeaderParser::NAME << "=" << QUOTE(CBasebandName::name(dataStruct_.basebands_.at(ibb).name())) << ">" << endl;
      spectralWindowsToXML(dataStruct_.basebands_, ibb, oss);
      oss << "</" << HeaderParser::BASEBAND << ">" << endl;
    }
  }

  void SDMDataObject::dataStructToXML(ostringstream& oss) {
    oss << "<" << HeaderParser::DATASTRUCT ;

    // The xsi:type attribute.
    oss << " xsi:type=" ;
    switch (correlationMode_) {
    case CROSS_ONLY:
      oss << QUOTE("CrossDataFullResolution");
      break;
    case AUTO_ONLY:
      oss << QUOTE("AutoDataFullResolution");
      break;
    case CROSS_AND_AUTO:
      oss << QUOTE("CrossAndAutoDataFullResolution");
      break;
    }
    
    // the apc attribute
    switch (correlationMode_) {
    case CROSS_ONLY:
    case CROSS_AND_AUTO:
      oss << " apc=" ;
      oss << QUOTE<AtmPhaseCorrection, CAtmPhaseCorrection>(dataStruct_.apc_);
      break;
    case AUTO_ONLY: 
      break;
    }
    
    oss << ">" << endl;
    
    // Before unrolling the basebands let's prepare the id_ and image_ private fields of the spectral windows.
    updateIdImageSPW();
    
    // Now unroll the basebands.    
    basebandsToXML(oss);
    
    if ( dataStruct_.flags_.size() )          toXML(dataStruct_.flags_, HeaderParser::FLAGS, oss);
    if ( dataStruct_.actualTimes_.size() )    toXML(dataStruct_.actualTimes_, HeaderParser::ACTUALTIMES, oss);
    if ( dataStruct_.actualDurations_.size() )toXML(dataStruct_.actualDurations_, HeaderParser::ACTUALDURATIONS, oss);
    
    switch (correlationMode_) {
    case CROSS_ONLY:
      toXML(dataStruct_.crossData_, HeaderParser::CROSSDATA, oss);
      break;
    case AUTO_ONLY:
      toXML(dataStruct_.autoData_, HeaderParser::AUTODATA, oss);
      break;
    case CROSS_AND_AUTO:
      toXML(dataStruct_.crossData_, HeaderParser::CROSSDATA, oss);
      toXML(dataStruct_.autoData_, HeaderParser::AUTODATA, oss);
      break;
    default:
      break;
    }
    
    /*
      if (spectralResolutionType_ == FULL_RESOLUTION ) {
      toXML(dataStruct_.zeroLags_, HeaderParser::ZEROLAGS, oss);
      }
    */
    // zeroLags are optional in any case - Michel Caillat - 24 Jul 2008
    if (dataStruct_.zeroLags().size() != 0) {
      toXML(dataStruct_.zeroLags_, HeaderParser::ZEROLAGS, oss);
    }
    
    oss << "</" << HeaderParser::DATASTRUCT << ">" << endl;
  }

  string SDMDataObject::toXML()  {
    TSTVALID();
    ostringstream result;
  
    result << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl;
  
    result << "<" << HeaderParser::SDMDATAHEADER 
	   << " " << HeaderParser::BYTEORDER << "=\"" << byteOrder_->toString() << "\""
	   << " " << HeaderParser::SCHEMAVERSION << "=\"" << schemaVersion_ << "\""
	   << " xsi:noNamespaceSchemaLocation=\"binDataHeader9.xsd\"" 
	   << " xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\""
	   << " xmlns:xs=\"http://www.w3.org/2001/XMLSchema\"" 
	   << " xmlns:xlink=\"http://www.w3.org/1999/xlink\""
	   << " xmlns:xhtml=\"http://www.w3.org/1999/xhtml\""
	   << " xmlns:xvers=\"http://aramis.obspm.fr/~alma/xvers\""
	   << " " << HeaderParser::PROJECTPATH << "=" << QUOTE(projectPath())
	   << " " << "xvers:schemaVersion=\"0\""
	   << " " << "xvers:revision=\"0.0.96\""
	   << ">" << endl;
  
    // startTime...
    TOXML(HeaderParser::STARTTIME, startTime_, result);   
  
    // dataOID...
    result << "<" << HeaderParser::DATAOID
	   << " xlink:type=\"locator\""
	   << " xlink:" << HeaderParser::XLINKHREF  << "=" << QUOTE(dataOID_)
	   << " xlink:" << HeaderParser::XLINKTITLE << "=" << QUOTE(title_)
	   << "/>" << endl;
  
    // dimensionality or numTime ...
    if (dimensionality_ == 0) {
      result << "<" << HeaderParser::NUMTIME
	     << ">"
	     << numTime_
	     << "</" << HeaderParser::NUMTIME << ">"
	     << endl;
    }
    else { 
      result << "<" << HeaderParser::DIMENSIONALITY
	     << " axes=\"TIM\""
	     << ">"
	     << dimensionality_
	     << "</" << HeaderParser::DIMENSIONALITY << ">"
	     << endl;
    }
  
    // execBlock...
    result << "<" << HeaderParser::EXECBLOCK << " " << HeaderParser::XLINKHREF << "=" << QUOTE(execBlockUID_) << "/>" << endl;
  
    // numAntenna
    TOXML(HeaderParser::NUMANTENNA, numAntenna_, result);
  
    //correlationMode
    TOXML<CorrelationMode, CCorrelationMode>(HeaderParser::CORRELATIONMODE,
					     correlationMode_,
					     result);

    // spectralResolutionType ... if present.
    if (spectralResolutionType_.present())
      TOXML<SpectralResolutionType, CSpectralResolutionType>(HeaderParser::SPECTRALRESOLUTION,
							     spectralResolutionType_.literal(),
							     result);
  
    // processorType
    TOXML<ProcessorType, CProcessorType>(HeaderParser::PROCESSORTYPE,
					 processorType_,
					 result);
  
    // dataStruct
    dataStructToXML(result);
  
    result << "</" << HeaderParser::SDMDATAHEADER << ">";
  
  
    return result.str();
  }

  void SDMDataObject::updateIdImageSPW() {
    ostringstream oss;
    for (unsigned int ibb = 0; ibb < dataStruct_.basebands_.size(); ibb++) {
      vector<SDMDataObject::SpectralWindow>& spws = dataStruct_.basebands_.at(ibb).spectralWindows_;
      for (unsigned int ispw = 0; ispw < spws.size(); ispw++) {
	oss.str("");
	oss << ispw+1;
	spws.at(ispw).strSw(oss.str());
	spws.at(ispw).strImage("");
      }
    }
    
    unsigned int ibbKey;
    unsigned int ispwKey;
    unsigned int ibbValue;
    unsigned int ispwValue;
    
    string key, value;
    map<string, string>::const_iterator iter;
    istringstream iss;

    
    for (iter = dataStruct_.imageSPW_.begin(); iter != dataStruct_.imageSPW_.end(); iter++) {
      key   = iter->first;
      iss.clear();
      iss.str(key);
      iss >> ibbKey;
      iss >> ispwKey;

      value = iter->second;
      iss.clear();
      iss.str(value);
      iss >> ibbValue;
      iss >> ispwValue;      

      oss.str("");
      oss << ispwKey;
      dataStruct_.basebands_.at(ibbKey).spectralWindows_.at(ispwKey).strImage_  = dataStruct_.basebands_.at(ibbKey).spectralWindows_.at(ispwValue).strSw_;
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

  // SDMDataObject::

  // SDMDataSubset:: methods
  //
  SDMDataSubset::SDMDataSubset(SDMDataObject* owner) {
    owner_	       = owner ;
    integrationNum_    = 0;
    subintegrationNum_ = 0;
    time_	       = 0;
    interval_	       = 0;
    actualTimes_       = 0;
    nActualTimes_      = 0;
    actualDurations_   = 0;
    nActualDurations_  = 0;
    zeroLags_	       = 0;
    nZeroLags_	       = 0;
    flags_	       = 0;
    nFlags_	       = 0;
    longCrossData_     = 0;
    shortCrossData_    = 0;
    floatCrossData_    = 0;
    nCrossData_	       = 0;
    autoData_	       = 0;
    nAutoData_	       = 0;
    aborted_	       = false;
  }


  SDMDataSubset::SDMDataSubset(SDMDataObject* owner,
			       unsigned long long time,
			       unsigned long long interval,
			       const vector<float>& autoData):
    owner_(owner),
    time_(time),
    interval_(interval) {
    integrationNum_    = 0;
    subintegrationNum_ = 0;
    actualTimes_       = 0;
    nActualTimes_      = 0;
    actualDurations_   = 0;
    nActualDurations_  = 0;
    zeroLags_	       = 0;
    nZeroLags_	       = 0;
    flags_	       = 0;
    nFlags_	       = 0;
    longCrossData_     = 0;
    shortCrossData_    = 0;
    floatCrossData_    = 0;
    nCrossData_	       = 0;
    if (autoData.size() != 0) {
      autoData_ = &autoData.at(0);
      nAutoData_ = autoData.size();
    }
    else {
      autoData_ = 0;
      nAutoData_ = 0;
    }
    aborted_ = false;
  }
  
  SDMDataSubset::SDMDataSubset(const SDMDataSubset& sdmDataSubset) {
    owner_	       = sdmDataSubset.owner_ ;
    integrationNum_    = sdmDataSubset.integrationNum_;
    subintegrationNum_ = sdmDataSubset.subintegrationNum_;
    ref_               = sdmDataSubset.ref_;
    time_	       = sdmDataSubset.time_;
    interval_	       = sdmDataSubset.interval_;
    dataStruct_        = sdmDataSubset.dataStruct_;
    flagsREF_          = sdmDataSubset.flagsREF_;
    actualTimesREF_    = sdmDataSubset.actualTimesREF_;
    actualDurationsREF_= sdmDataSubset.actualDurationsREF_;
    zeroLagsREF_       = sdmDataSubset.zeroLagsREF_;
    crossDataREF_      = sdmDataSubset.crossDataREF_;
    crossDataType_     = sdmDataSubset.crossDataType_;
    autoDataREF_       = sdmDataSubset.autoDataREF_;
    actualTimes_       = sdmDataSubset.actualTimes_;
    nActualTimes_      = sdmDataSubset.nActualTimes_;
    actualDurations_   = sdmDataSubset.actualDurations_;
    nActualDurations_  = sdmDataSubset.nActualDurations_;
    zeroLags_	       = sdmDataSubset.zeroLags_;
    nZeroLags_	       = sdmDataSubset.nZeroLags_;
    flags_	       = sdmDataSubset.flags_;
    nFlags_	       = sdmDataSubset.nFlags_;
    longCrossData_     = sdmDataSubset.longCrossData_;
    shortCrossData_    = sdmDataSubset.shortCrossData_;
    floatCrossData_    = sdmDataSubset.floatCrossData_;
    nCrossData_	       = sdmDataSubset.nCrossData_;
    autoData_	       = sdmDataSubset.autoData_;
    nAutoData_	       = sdmDataSubset.nAutoData_;
    aborted_	       = sdmDataSubset.aborted_;
    abortTime_         = sdmDataSubset.abortTime_;
    abortReason_       = sdmDataSubset.abortReason_;
  }

  SDMDataSubset& SDMDataSubset::operator=(const SDMDataSubset& sdmDataSubset) {
    owner_	       = sdmDataSubset.owner_ ;
    integrationNum_    = sdmDataSubset.integrationNum_;
    subintegrationNum_ = sdmDataSubset.subintegrationNum_;
    ref_               = sdmDataSubset.ref_;
    time_	       = sdmDataSubset.time_;
    interval_	       = sdmDataSubset.interval_;
    dataStruct_        = sdmDataSubset.dataStruct_;
    flagsREF_          = sdmDataSubset.flagsREF_;
    actualTimesREF_    = sdmDataSubset.actualTimesREF_;
    actualDurationsREF_= sdmDataSubset.actualDurationsREF_;
    zeroLagsREF_       = sdmDataSubset.zeroLagsREF_;
    crossDataREF_      = sdmDataSubset.crossDataREF_;
    crossDataType_     = sdmDataSubset.crossDataType_;
    autoDataREF_       = sdmDataSubset.autoDataREF_;
    actualTimes_       = sdmDataSubset.actualTimes_;
    nActualTimes_      = sdmDataSubset.nActualTimes_;
    actualDurations_   = sdmDataSubset.actualDurations_;
    nActualDurations_  = sdmDataSubset.nActualDurations_;
    zeroLags_	       = sdmDataSubset.zeroLags_;
    nZeroLags_	       = sdmDataSubset.nZeroLags_;
    flags_	       = sdmDataSubset.flags_;
    nFlags_	       = sdmDataSubset.nFlags_;
    longCrossData_     = sdmDataSubset.longCrossData_;
    shortCrossData_    = sdmDataSubset.shortCrossData_;
    floatCrossData_    = sdmDataSubset.floatCrossData_;
    nCrossData_	       = sdmDataSubset.nCrossData_;
    autoData_	       = sdmDataSubset.autoData_;
    nAutoData_	       = sdmDataSubset.nAutoData_;
    aborted_	       = sdmDataSubset.aborted_;
    abortTime_         = sdmDataSubset.abortTime_;
    abortReason_       = sdmDataSubset.abortReason_;
    return *this;
  }

  SDMDataSubset::~SDMDataSubset() {;}

  string SDMDataSubset::projectPath() const {
    if (owner_ == 0) return "";

    ostringstream oss;
    oss << owner_->execBlockNum_ << "/" << owner_->scanNum_ << "/" << owner_->subscanNum_ << "/"; 

    if (integrationNum_ > 0) oss << integrationNum_ << "/";
    if (subintegrationNum_ > 0) oss << subintegrationNum_ << "/";

    return oss.str();
  }
  

  unsigned int SDMDataSubset::integrationNum() const { return integrationNum_; }

  unsigned int SDMDataSubset::subintegrationNum() const { return subintegrationNum_; }

  unsigned long long SDMDataSubset::time() const { return time_; }
  
  unsigned long long SDMDataSubset::interval() const { 
    // if (owner_ && !owner_->isTP()) Utils::invalidCall("SDMDataSubset::interval", owner_);
    return interval_; 
  }

   
  unsigned long int SDMDataSubset::actualDurations(const ACTUALDURATIONSTYPE* & ptr) const {ptr = actualDurations_; return nActualDurations_;}
  uint64_t SDMDataSubset::actualDurationsPosition() const { return actualDurationsPosition_; }
  unsigned long int SDMDataSubset::actualTimes(const ACTUALTIMESTYPE* & ptr) const { ptr = actualTimes_; return nActualTimes_;}
  uint64_t SDMDataSubset::actualTimesPosition() const { return actualTimesPosition_; }
  unsigned long int SDMDataSubset::autoData(const AUTODATATYPE* & ptr)const  { ptr = autoData_; return nAutoData_; }
  uint64_t SDMDataSubset::autoDataPosition() const { return autoDataPosition_; }
  unsigned long int SDMDataSubset::crossData(const SHORTCROSSDATATYPE* & ptr) const { 
    if (owner_ && (owner_->isTP() || owner_->correlationMode() == AUTO_ONLY)) Utils::invalidCall("SDMDataSubset::crossData", owner_);
    ptr = shortCrossData_; return nCrossData_; 
  }
  PrimitiveDataType SDMDataSubset::crossDataType() const { return crossDataType_; }
  void SDMDataSubset::crossDataType(PrimitiveDataType value) { crossDataType_ = value;}

  unsigned long int SDMDataSubset::crossData(const INTCROSSDATATYPE* & ptr) const { 
    if (owner_ && (owner_->isTP() || owner_->correlationMode() == AUTO_ONLY)) Utils::invalidCall("SDMDataSubset::crossData", owner_);
    ptr = longCrossData_; return nCrossData_; 
  }


  unsigned long int SDMDataSubset::crossData(const FLOATCROSSDATATYPE* & ptr) const { 
    if (owner_ && (owner_->isTP() || owner_->correlationMode() == AUTO_ONLY)) Utils::invalidCall("SDMDataSubset::crossData", owner_);
    ptr = floatCrossData_; return nCrossData_; 
  }

  uint64_t SDMDataSubset::crossDataPosition() const { return crossDataPosition_; }
  unsigned long int SDMDataSubset::flags(const FLAGSTYPE* & ptr) const { ptr = flags_; return nFlags_; }
  uint64_t SDMDataSubset::flagsPosition() const { return flagsPosition_; }

  unsigned long int SDMDataSubset::zeroLags(const ZEROLAGSTYPE* & ptr) const { 
    if (owner_ && owner_->isTP()) Utils::invalidCall("SDMDataSubset::zeroLags", owner_);
    ptr = zeroLags_; return nZeroLags_;
  }
  uint64_t SDMDataSubset::zeroLagsPosition() const { return zeroLagsPosition_; }
  
  bool SDMDataSubset::aborted() const {return aborted_; }
  unsigned long long SDMDataSubset::abortTime() const {return abortTime_; }  
  string SDMDataSubset::abortReason() const {return abortReason_;}

#define MIN_(a,b) ((a<b)?a:b)
#define PRINT(_v_, _n_, _nmax_, _out_ ) for (unsigned int i = 0; i < MIN_(_n_, _nmax_); i++) _out_ << " " << _v_[i]; if (_n_ > _nmax_) _out_ << "...";
 
  string SDMDataSubset::toString(unsigned int N ) const {
    ostringstream result;
    //boost::gregorian::date d(2007, Jan, 1);
    //ptime t1(date(2007, Jan, 1), time_duration(0, 0, 0,(time_ - FIRST_JAN_2007)/1000 ));
    //result << "(owned by '" << owner_ << "')" << endl; 
  
    result << "projectPath = " << projectPath() << endl;
    //result << "time = " << time_ << " (" << to_simple_string(t1)  << ")" << endl;
    result << "time = " << time_ << endl;
    //result << "dataStructureDesc = " << dataStructureDesc_ << endl;
    result << "interval = " << interval_ <<endl;
  

    if ( aborted_ ) {
      // It"s an aborted [sub]integration.
      result << "Aborted at = " << abortTime_ << endl;
      result << "Reason = " << abortReason_ << endl;
      return result.str();
    }


    // It's not an aborted [sub]integration.
    switch (owner_->dimensionality_) {
    case 0:
      break;
      
    case 1:
      switch (owner_->correlationMode_) {
      case CROSS_ONLY:
	result << "crossDataType = " << CPrimitiveDataType::name(crossDataType_) << endl;
	break;
      case AUTO_ONLY:
	break;
      case CROSS_AND_AUTO:
	result << "crossDataType = " << CPrimitiveDataType::name(crossDataType_) << endl;
	break;
      }
      break;
    default:
      break;
    }

    result << "Binary attachments :" << endl;
    
    if (nActualTimes_) {
      result << "ActualTimes (" << nActualTimes_ << " values ) = "; PRINT(actualTimes_, nActualTimes_, N, result); result << endl;
    }

    if (nActualDurations_) {
      result << "ActualDurations (" << nActualDurations_ << " values ) = "; PRINT(actualDurations_, nActualDurations_, N, result); result << endl;
    }

    if (nFlags_) { 
      result << "Flags (" << nFlags_ << " values ) = "; PRINT(flags_, nFlags_, N, result); result << endl;
    }
    
    if (nCrossData_) { 
      result << "CrossData (" << nCrossData_ << " values ) = "; 
      switch (crossDataType_) {
      case INT32_TYPE :
	PRINT(longCrossData_, nCrossData_, N, result); result << endl;
	break;
      case INT16_TYPE :
	PRINT(shortCrossData_, nCrossData_, N, result); result << endl;
	break;
      case FLOAT32_TYPE:
	PRINT(floatCrossData_, nCrossData_, N, result); result << endl;
	break;

      default: 
	throw SDMDataObjectException("'" + CPrimitiveDataType::name(crossDataType_)+"' data are not processed here."); 
  
      }
    }

    if (nAutoData_) {
      result << "AutoData (" << nAutoData_ << " values ) = "; PRINT(autoData_, nAutoData_, N, result); result << endl;
    }
    
    if (nZeroLags_) { 
      result << "ZeroLags (" << nZeroLags_ << " values ) = = "; PRINT(zeroLags_, nZeroLags_, N, result); result << endl;
    }

    return result.str();
  }


  void SDMDataSubset::binAttachToXML(const string& name, ostringstream& oss) {
    oss << "<" << name << " xlink:href= \"" << projectPath() << "/" << name << ".bin\"";
    if (name == "crossData") 
      oss << " type=\"" <<  CPrimitiveDataType::name(crossDataType_) << "\"";
    oss << "/>" << endl;
  }

  void SDMDataSubset::tpBinAttachToXML(ostringstream& oss) {
    if (nFlags_)
      binAttachToXML(TPSubsetHeaderParser::FLAGSREF, oss);
    if (nActualTimes_)
      binAttachToXML(TPSubsetHeaderParser::ACTUALTIMESREF, oss);
    if (nActualDurations_)
      binAttachToXML(TPSubsetHeaderParser::ACTUALDURATIONSREF, oss);
    binAttachToXML(TPSubsetHeaderParser::AUTODATAREF, oss);
  }

  void SDMDataSubset::corrBinAttachToXML(ostringstream& oss) {    
    // The binaryAttachments elements

    if (nFlags_)
      binAttachToXML(CorrSubsetHeaderParser::FLAGSREF, oss);
    if (nActualTimes_)
      binAttachToXML(CorrSubsetHeaderParser::ACTUALTIMESREF, oss);
    if (nActualDurations_)
      binAttachToXML(CorrSubsetHeaderParser::ACTUALDURATIONSREF, oss);

    switch (owner_->correlationMode_) {
    case CROSS_ONLY:
      
      binAttachToXML(CorrSubsetHeaderParser::CROSSDATAREF, oss);
      break;
      
    case AUTO_ONLY:
      binAttachToXML(CorrSubsetHeaderParser::AUTODATAREF, oss);          
      break;

    case CROSS_AND_AUTO:
      binAttachToXML(CorrSubsetHeaderParser::CROSSDATAREF, oss);            
      binAttachToXML(CorrSubsetHeaderParser::AUTODATAREF, oss);          
      break;
    }

    if ((owner_->processorType_ == CORRELATOR) && (owner_->correlatorType() != FX))
	binAttachToXML(CorrSubsetHeaderParser::ZEROLAGSREF, oss);            
  }
 

  string SDMDataSubset::xsiType() const {
    string result;
    
    switch (owner_->correlationMode_) {
    case CROSS_ONLY:     result = "BinaryCrossData" ; break;
    case AUTO_ONLY:      result = "BinaryAutoData" ; break;
    case CROSS_AND_AUTO: result = "BinaryCrossAndAutoData" ; break;
    }
        
    return result;
  }


  string SDMDataSubset::toXML() {
    ostringstream oss;
    
    //oss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl;
    if (owner_->processorType_ == CORRELATOR) {
      oss << "<" << CorrSubsetHeaderParser::SDMDATASUBSETHEADER
	  << " xmlns:xlink=\"http://www.w3.org/1999/xlink\""
	  << " xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\""
	  << " xsi:type=\"" << xsiType() << "\""
	  << " projectPath="<< QUOTE(projectPath())
	  << ">" << endl;
      
      OXML(CorrSubsetHeaderParser::SCHEDULEPERIODTIME, oss);
      TOXML(CorrSubsetHeaderParser::TIME, time_, oss);
      TOXML(CorrSubsetHeaderParser::INTERVAL, interval_, oss);
      CXML(CorrSubsetHeaderParser::SCHEDULEPERIODTIME, oss);
      
      oss << "<" << CorrSubsetHeaderParser::DATASTRUCT << " ref =" << QUOTE("sdmDataHeader") << "/>" << endl;
      corrBinAttachToXML(oss);
      oss << "</" << CorrSubsetHeaderParser::SDMDATASUBSETHEADER << ">";
      return oss.str();
    }
    else if (owner_->processorType_ == RADIOMETER) {
      oss << "<" << TPSubsetHeaderParser::SDMDATASUBSETHEADER
	  << " xmlns:xlink=\"http://www.w3.org/1999/xlink\""
	  << " xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\""
	  << " xsi:type=\"" << xsiType() << "\""
	  << " projectPath="<< QUOTE(projectPath())
	  << ">" << endl;
    
      OXML(TPSubsetHeaderParser::SCHEDULEPERIODTIME, oss);
      TOXML(TPSubsetHeaderParser::TIME, time_, oss);
      TOXML(TPSubsetHeaderParser::INTERVAL, interval_, oss);
      CXML(TPSubsetHeaderParser::SCHEDULEPERIODTIME, oss);
      
      oss << "<" << TPSubsetHeaderParser::DATASTRUCT << " ref =" << QUOTE("sdmDataHeader") << "/>" << endl;
      tpBinAttachToXML(oss);
      oss << "</" << TPSubsetHeaderParser::SDMDATASUBSETHEADER << ">";
      return oss.str();
    }
    else
      throw SDMDataObjectException("no XML representation defined for an SDMDataSubset with a processor type defined as '"
				   + CProcessorType::name(owner_->processorType_));
  }
  

  const SDMDataObject* SDMDataSubset::owner() const { return owner_; }
  // SDMDataSubset::


  // Utils:: methods
  //
#define COUTCONTAINER(result, v) (copy(v.begin(), v.end(), ostream_iterator<string>(result, " ")))  
  string Utils::quote(const string& s) {
    return "\""+s+"\"";
  }


  void Utils::invalidCall(const string & methodName, const SDMDataObject* sdmDataObject) {
    ostringstream oss;
    oss << "Invalid call of method '" << methodName << "' in this context :";
    
    if (sdmDataObject->isTP()) 
      oss << "total power data ('" << CCorrelationMode::name(sdmDataObject->correlationMode()) << "')" << endl;
    else if (sdmDataObject->isCorrelation()) {
      oss << "correlator data ('" << CCorrelationMode::name(sdmDataObject->correlationMode()) << "')" << endl; 
    }
    throw SDMDataObjectException(oss.str());
  }

  string Utils::quote(bool b) {
    ostringstream oss;
    oss << "\"" << (b?"true":"false") << "\"";
    return oss.str();
  }

  string Utils::quote(int i) {
    ostringstream oss;
    oss << "\"" << i << "\"";
    return oss.str();
  }

  string Utils::quote(unsigned int ui) {
    ostringstream oss;
    oss << "\"" << ui << "\"";
    return oss.str();
  }


  string Utils::quote(long long l) {
    ostringstream oss;
    oss << "\"" << l << "\"";
    return oss.str();
  }

  string Utils::quote(float f) {
    ostringstream oss;
    oss << "\"" << f << "\"";
    return oss.str();
  }

  string Utils::quote(const set<string>& s) {
    ostringstream oss;
    oss << "\"";
    COUTCONTAINER(oss, s);
    oss << "\"";
    return oss.str();
  }


  string Utils::quote(const vector<string>& s) {
    ostringstream oss;
    oss << "\"";
    COUTCONTAINER(oss, s);
    oss << "\"";
    return oss.str();
  }


  void  Utils::toXML(const string& elementName, int value, ostringstream& oss){
    oss << "<" << elementName << ">" << value << "</" << elementName << ">" << endl;
  }

  void  Utils::toXML(const string& elementName, unsigned int value, ostringstream& oss){
    oss << "<" << elementName << ">" << value << "</" << elementName << ">" << endl;
  }

  void  Utils::toXML(const string& elementName, long long value, ostringstream& oss){
    oss << "<" << elementName << ">" << value << "</" << elementName << ">" << endl;
  }

  void  Utils::toXML(const string& elementName, unsigned long long value, ostringstream& oss){
    oss << "<" << elementName << ">" << value << "</" << elementName << ">" << endl;
  }

  void Utils::oXML(const string& elementName, ostringstream& oss) {
    oss << "<" << elementName << ">" << endl;
  }

  void Utils::cXML(const string& elementName, ostringstream& oss) {
    oss << "</" << elementName << ">" << endl;
  }

  // Utils::


} // asdmbinaries
