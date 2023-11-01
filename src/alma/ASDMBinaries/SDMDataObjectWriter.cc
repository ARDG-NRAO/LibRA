#include <alma/ASDMBinaries/SDMDataObjectWriter.h>
#include <algorithm>

#include <alma/ASDMBinaries/CommonDefines.h>

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

  const string SDMDataObjectWriter::MIMEBOUNDARY_1  = $MIMEBOUNDARY1;
  const string SDMDataObjectWriter::MIMEBOUNDARY_2  = $MIMEBOUNDARY2;

  vector<AxisName> SDMDataObjectWriter::WVRDATAAXES;
  vector<AxisName> SDMDataObjectWriter::WVRDATAFLAGSAXES;

  const bool SDMDataObjectWriter::initClass_ = SDMDataObjectWriter::initClass();
  
  SDMDataObjectWriter::SDMDataObjectWriter(const string& uid, const string& title) {
    currentState_ = START;
    otype_ = STDOUT;
    uid_ = uid;
    title_ = title;
    preamble();
    done_ = false;
    sdmDataSubsetNum_ = 0;
    numBytes_ = 0;

    sdmDataObject_.schemaVersion_ = SCHEMAVERSION;
    sdmDataObject_.byteOrder_ = ByteOrder::Machine_Endianity;
  }
  
  SDMDataObjectWriter::SDMDataObjectWriter(ostringstream* oss, const string& uid,  const string& title) {
    currentState_ = START;
    otype_ = MEMORY;
    ofs_ = 0;
    oss_ = oss;
    uid_ = uid;
    title_ = title;
    preamble();
    done_ = false;
    sdmDataSubsetNum_ = 0;
    numBytes_ = 0;

    sdmDataObject_.schemaVersion_ = SCHEMAVERSION;
    sdmDataObject_.byteOrder_ = ByteOrder::Machine_Endianity;
  }
  
  SDMDataObjectWriter::SDMDataObjectWriter(ofstream* ofs, const string& uid,  const string& title) {
    currentState_ = START;
    otype_ = FILE;
    ofs_ = ofs;
    oss_ = 0;
    uid_ = uid;
    title_ = title;
    preamble();
    done_ = false;
    sdmDataSubsetNum_ = 0;
    numBytes_ = 0;

    sdmDataObject_.schemaVersion_ = SCHEMAVERSION;
    sdmDataObject_.byteOrder_ = ByteOrder::Machine_Endianity;
  }

  SDMDataObjectWriter::~SDMDataObjectWriter() {
    // Do nothing actually !
    // if (!done_) done();
  }

  void SDMDataObjectWriter::done() {
    checkState(T_DONE, "done");

    // Write MIME postamble.
    postamble();

    // And do some I/O management.    
    switch (otype_) {
    case STDOUT:
      // Do Nothing special
      break;
    case MEMORY:
      // Do Nothing special
      break;
    case FILE:
      // Do nothing special
      break;
    }
    numBytes_ = 0;
    done_ = true;
  }
  
  void SDMDataObjectWriter::output(const string& s) {
    numBytes_ += (uint64_t) s.size();
    switch (otype_) {
    case STDOUT:
      cout << s;
      break;

    case MEMORY:
      *oss_ << s;
      break;

    case FILE:
      *ofs_ << s;
      break;
    }
  }

  void SDMDataObjectWriter::outputln(const string& s) {
    output(s);
    outputln();
  }

  void SDMDataObjectWriter::output(const float* data, uint32_t numData) {
    switch (otype_) {

    case STDOUT:
      cout.write((const char*)data, numData*sizeof(float));
      break;

    case MEMORY:
      oss_->write((const char*)data, numData*sizeof(float));
      break;
      
    case FILE:
      ofs_->write((const char*)data, numData*sizeof(float));
      break;
    } 
  }

  void SDMDataObjectWriter::outputln(const float* data, uint32_t numData) {
    output(data, numData);
    outputln();
  } 

  void SDMDataObjectWriter::outputln() {
    output("\n");
  }

  void SDMDataObjectWriter::outputlnLocation(const string& name, const SDMDataSubset& sdmDataSubset) {
    outputln("Content-Location: " + sdmDataSubset.projectPath() +  name + ".bin");
  }

  void SDMDataObjectWriter::preamble() {
    outputln("MIME-Version: 1.0");
    outputln("Content-Type: multipart/mixed; boundary=\""+MIMEBOUNDARY_1+"\"; type=\"text/xml\"");
    outputln("Content-Description: " + title_ );
    outputln("Content-Location: uid:" + uid_.substr(4));
    outputln();
  }


  void SDMDataObjectWriter::postamble() {
    outputln("--"+MIMEBOUNDARY_1+"--");
  }



  // void SDMDataObjectWriter::addTPSubscan(uint64_t time,
  // 					 uint64_t interval,
  // 					 const vector<FLAGSTYPE>& flags,
  // 					 const vector<ACTUALTIMESTYPE>& actualTimes,
  // 					 const vector<ACTUALDURATIONSTYPE>& actualDurations,
  // 					 const vector<AUTODATATYPE>& autoData){
  //   checkState(T_ADDTPSUBSCAN, "addTPSubscan");
  //   outputln("Content-Type: Multipart/Related; boundary=\""+MIMEBOUNDARY_2+"\";type=\"text/xml\"; start=\"<DataSubset.xml>\"");
  //   outputln("Content-Description: Data and metadata subset");
  //   outputln("--"+MIMEBOUNDARY_2);
  //   outputln("Content-Type: text/xml; charset=\"UTF-8\"");
  //   outputln("Content-Location: " + sdmDataObject_.projectPath() + "desc.xml");
  //   outputln();

  //   SDMDataSubset tpDataSubset(&sdmDataObject_,
  // 			       time,
  // 			       interval,
  // 			       autoData);

  //   tpDataSubset.flags_ = (tpDataSubset.nFlags_ = flags.size()) ? &tpDataSubset.flags_[0] : 0;
  //   tpDataSubset.actualTimes_ = (tpDataSubset.nActualTimes_ = actualTimes.size()) ? &tpDataSubset.actualTimes_[0] : 0;    
  //   tpDataSubset.actualDurations_ = (tpDataSubset.nActualDurations_ = actualDurations.size()) ? &tpDataSubset.actualDurations_[0] : 0;    
  //   outputln(tpDataSubset.toXML());
  //   //outputln();

  //   if (flags.size() != 0) {
  //     uint32_t numFlags = sdmDataObject_.dataStruct_.flags_.size();
  //     if (numFlags!=0 && numFlags != flags.size()) {
  // 	ostringstream oss;
  // 	oss << "The number of values provided for 'flags' ("
  // 	    << flags.size()
  // 	    << "), is not equal to the number declared in the global header ("
  // 	    << numFlags << ").";
  // 	throw SDMDataObjectWriterException(oss.str());
  //     }
  //     outputln("--"+MIMEBOUNDARY_2);
  //     outputln("Content-Type: binary/octet-stream");
  //     outputln("Content-Location: " + tpDataSubset.projectPath() + "flags.bin");
  //     outputln();
  //     outputln<uint32_t>(flags);
  //   }
    
  //   if (actualTimes.size() != 0) {
  //     uint32_t numActualTimes = sdmDataObject_.dataStruct_.actualTimes_.size();
  //     if (numActualTimes != 0 && numActualTimes != actualTimes.size()) {
  // 	ostringstream oss;
  // 	oss << "The number of values provided for 'actualTimes' ("
  // 	    << actualTimes.size()
  // 	    << "), is not equal to the number declared in the global header ("
  // 	    << numActualTimes << ").";
  // 	throw SDMDataObjectWriterException(oss.str());
  //     }
  //     outputln("--"+MIMEBOUNDARY_2);
  //     outputln("Content-Type: binary/octet-stream");
  //     outputln("Content-Location: " + tpDataSubset.projectPath() + "actualTimes.bin");

  //     outputln();
  //     outputln<ACTUALTIMESTYPE>(actualTimes);
  //   }

  //   if (actualDurations.size() != 0) {
  //     uint32_t numActualDurations = sdmDataObject_.dataStruct_.actualDurations_.size();
  //     if (numActualDurations != 0 && numActualDurations != actualDurations.size()) {
  // 	ostringstream oss;
  // 	oss << "The number of values provided for 'actualDurations' ("
  // 	    << actualDurations.size()
  // 	    << "), is not equal to the number declared in the global header ("
  // 	    << numActualDurations << ").";
  // 	throw SDMDataObjectWriterException(oss.str());
  //     }
  //     outputln("--"+MIMEBOUNDARY_2);
  //     outputln("Content-Type: binary/octet-stream");
  //     outputln("Content-Location: " + tpDataSubset.projectPath() + "actualDurations.bin");
  //     outputln();
  //     outputln<ACTUALDURATIONSTYPE>(actualDurations);
  //   }

  //   uint32_t numAutoData = sdmDataObject_.dataStruct_.autoData_.size();
  //   if (numAutoData != 0 && numAutoData != autoData.size()) {
  //     ostringstream oss;
  //     oss << "The number of values provided for 'autoData' ("
  // 	  << autoData.size()
  // 	  << "), is not equal to the number declared in the global header ("
  // 	  << numAutoData << ").";
  //     throw SDMDataObjectWriterException(oss.str());
  //   }
  //   outputln("--"+MIMEBOUNDARY_2);
  //   outputln("Content-Type: binary/octet-stream");
  //   outputln("Content-Location: " + tpDataSubset.projectPath() + "autoData.bin");
  //   outputln();
  //   outputln<float>(autoData);
  //   outputln("--"+MIMEBOUNDARY_2+"--");
  // }

  void SDMDataObjectWriter::tpData(uint64_t startTime,
				   const string& execBlockUID,
				   uint32_t execBlockNum,
				   uint32_t scanNum,
				   uint32_t subscanNum,
				   uint32_t numberOfIntegrations,
				   uint32_t numAntenna,
				   const vector<SDMDataObject::Baseband>& basebands,
				   uint64_t time,
				   uint64_t interval,
				   const vector<AxisName>& autoDataAxes,
				   const vector<AUTODATATYPE>& autoData) {
    checkState(T_TPDATA, "tpData");

    SDMDataObject::DataStruct dataStruct;
    dataStruct.basebands_ = basebands;
    dataStruct.autoData_  = SDMDataObject::AutoDataBinaryPart(autoData.size(), autoDataAxes, false);
    
    sdmDataObject_.valid_ = true;

    sdmDataObject_.startTime_ = startTime;
    sdmDataObject_.dataOID_ = uid_;
    sdmDataObject_.title_ = title_;
    sdmDataObject_.dimensionality_ = 0;
    sdmDataObject_.numTime_ = numberOfIntegrations;
    sdmDataObject_.execBlockUID_ = execBlockUID;
    sdmDataObject_.execBlockNum_ = execBlockNum;
    sdmDataObject_.scanNum_ = scanNum;
    sdmDataObject_.subscanNum_ = subscanNum;
    sdmDataObject_.numAntenna_ = numAntenna;
    sdmDataObject_.correlationMode_ = AUTO_ONLY;
    sdmDataObject_.spectralResolutionType_ = BASEBAND_WIDE;
    sdmDataObject_.processorType_ = RADIOMETER;
    sdmDataObject_.dataStruct_ = dataStruct;

    outputln("--"+MIMEBOUNDARY_1);
    outputln("Content-Type: text/xml; charset=\"UTF-8\"");
    outputln("Content-Transfer-Encoding: 8bit");
    outputln("Content-Location: " + sdmDataObject_.projectPath() + "desc.xml");
    outputln();
        
    outputln(sdmDataObject_.toXML());
    outputln("--"+MIMEBOUNDARY_1);
    outputln("Content-Type: Multipart/Related; boundary=\""+MIMEBOUNDARY_2+"\";type=\"text/xml\"; start=\"<DataSubset.xml>\"");
    outputln("Content-Description: Data and metadata subset");
    outputln("--"+MIMEBOUNDARY_2);
    outputln("Content-Type: text/xml; charset=\"UTF-8\"");
    outputln("Content-Location: " + sdmDataObject_.projectPath() + "desc.xml");
    outputln();

    SDMDataSubset tpDataSubset(&sdmDataObject_,
			       time,
			       interval,
			       autoData);
    outputln(tpDataSubset.toXML());
    //outputln();

    outputln("--"+MIMEBOUNDARY_2);
    outputln("Content-Type: binary/octet-stream");
    outputln("Content-Location: " + tpDataSubset.projectPath() + "autoData.bin");
    outputln();
    
    outputln<float>(autoData);
    outputln("--"+MIMEBOUNDARY_2+"--");
  }

  void SDMDataObjectWriter::tpData(uint64_t startTime,
				   const string& execBlockUID,
				   uint32_t execBlockNum,
				   uint32_t scanNum,
				   uint32_t subscanNum,
				   uint32_t numberOfIntegrations,
				   uint32_t numAntenna,

				   const vector<SDMDataObject::Baseband>& basebands,

				   uint64_t time,
				   uint64_t interval,

				   const vector<AxisName>& flagsAxes,
				   const vector<FLAGSTYPE>& flags,
				   const vector<AxisName>& actualTimesAxes,
				   const vector<ACTUALTIMESTYPE>& actualTimes,
				   const vector<AxisName>& actualDurationsAxes,
				   const vector<ACTUALDURATIONSTYPE>& actualDurations,
				   const vector<AxisName>& autoDataAxes,
				   const vector<AUTODATATYPE>& autoData) {
    checkState(T_TPDATA, "tpData");
    
    SDMDataObject::DataStruct dataStruct;
    dataStruct.basebands_ = basebands;
    if (flags.size()) dataStruct.flags_ = SDMDataObject::BinaryPart(flags.size(), flagsAxes);
    if (actualTimes.size()) dataStruct.actualTimes_ = SDMDataObject::BinaryPart(actualTimes.size(), actualTimesAxes);
    if (actualDurations.size()) dataStruct.actualDurations_ = SDMDataObject::BinaryPart(actualDurations.size(), actualDurationsAxes);
    dataStruct.autoData_  = SDMDataObject::AutoDataBinaryPart(autoData.size(), autoDataAxes, false);
    
    sdmDataObject_.valid_ = true;

    sdmDataObject_.startTime_ = startTime;
    sdmDataObject_.dataOID_ = uid_;
    sdmDataObject_.title_ = title_;
    sdmDataObject_.dimensionality_ = 0;
    sdmDataObject_.numTime_ = numberOfIntegrations;
    sdmDataObject_.execBlockUID_ = execBlockUID;
    sdmDataObject_.execBlockNum_ = execBlockNum;
    sdmDataObject_.scanNum_ = scanNum;
    sdmDataObject_.subscanNum_ = subscanNum;
    sdmDataObject_.numAntenna_ = numAntenna;
    sdmDataObject_.correlationMode_ = AUTO_ONLY;
    sdmDataObject_.spectralResolutionType_ = BASEBAND_WIDE;
    sdmDataObject_.processorType_ = RADIOMETER;
    sdmDataObject_.dataStruct_ = dataStruct;

    outputln("--"+MIMEBOUNDARY_1);
    outputln("Content-Type: text/xml; charset=\"UTF-8\"");
    outputln("Content-Transfer-Encoding: 8bit");
    outputln("Content-Location: " + sdmDataObject_.projectPath() + "desc.xml");
    outputln();
    
    outputln(sdmDataObject_.toXML());
    outputln("--"+MIMEBOUNDARY_1);
    outputln("Content-Type: Multipart/Related; boundary=\""+MIMEBOUNDARY_2+"\";type=\"text/xml\"; start=\"<DataSubset.xml>\"");
    outputln("Content-Description: Data and metadata subset");
    outputln("--"+MIMEBOUNDARY_2);
    outputln("Content-Type: text/xml; charset=\"UTF-8\"");
    outputln("Content-Location: " + sdmDataObject_.projectPath() + "desc.xml");
    outputln();

    SDMDataSubset tpDataSubset(&sdmDataObject_,
			       time,
			       interval,
			       autoData);

    tpDataSubset.flags_ = (tpDataSubset.nFlags_ = flags.size()) ? &tpDataSubset.flags_[0] : 0;
    tpDataSubset.actualTimes_ = (tpDataSubset.nActualTimes_ = actualTimes.size()) ? &tpDataSubset.actualTimes_[0] : 0;    
    tpDataSubset.actualDurations_ = (tpDataSubset.nActualDurations_ = actualDurations.size()) ? &tpDataSubset.actualDurations_[0] : 0;    
    outputln(tpDataSubset.toXML());
    //outputln();



    if (flags.size() != 0) {
      uint32_t numFlags = sdmDataObject_.dataStruct_.flags_.size();
      if (numFlags!=0 && numFlags != flags.size()) {
	ostringstream oss;
	oss << "The number of values provided for 'flags' ("
	    << flags.size()
	    << "), is not equal to the number declared in the global header ("
	    << numFlags << ").";
	throw SDMDataObjectWriterException(oss.str());
      }
      outputln("--"+MIMEBOUNDARY_2);
      outputln("Content-Type: binary/octet-stream");
      outputln("Content-Location: " + tpDataSubset.projectPath() + "flags.bin");
      outputln();
      outputln<uint32_t>(flags);
    }
    
    if (actualTimes.size() != 0) {
      uint32_t numActualTimes = sdmDataObject_.dataStruct_.actualTimes_.size();
      if (numActualTimes != 0 && numActualTimes != actualTimes.size()) {
	ostringstream oss;
	oss << "The number of values provided for 'actualTimes' ("
	    << actualTimes.size()
	    << "), is not equal to the number declared in the global header ("
	    << numActualTimes << ").";
	throw SDMDataObjectWriterException(oss.str());
      }
      outputln("--"+MIMEBOUNDARY_2);
      outputln("Content-Type: binary/octet-stream");
      outputln("Content-Location: " + tpDataSubset.projectPath() + "actualTimes.bin");

      outputln();
      outputln<ACTUALTIMESTYPE>(actualTimes);
    }

    if (actualDurations.size() != 0) {
      uint32_t numActualDurations = sdmDataObject_.dataStruct_.actualDurations_.size();
      if (numActualDurations != 0 && numActualDurations != actualDurations.size()) {
	ostringstream oss;
	oss << "The number of values provided for 'actualDurations' ("
	    << actualDurations.size()
	    << "), is not equal to the number declared in the global header ("
	    << numActualDurations << ").";
	throw SDMDataObjectWriterException(oss.str());
      }
      outputln("--"+MIMEBOUNDARY_2);
      outputln("Content-Type: binary/octet-stream");
      outputln("Content-Location: " + tpDataSubset.projectPath() + "actualDurations.bin");
      outputln();
      outputln<ACTUALDURATIONSTYPE>(actualDurations);
    }

    uint32_t numAutoData = sdmDataObject_.dataStruct_.autoData_.size();
    if (numAutoData != 0 && numAutoData != autoData.size()) {
      ostringstream oss;
      oss << "The number of values provided for 'autoData' ("
	  << autoData.size()
	  << "), is not equal to the number declared in the global header ("
	  << numAutoData << ").";
      throw SDMDataObjectWriterException(oss.str());
    }
    outputln("--"+MIMEBOUNDARY_2);
    outputln("Content-Type: binary/octet-stream");
    outputln("Content-Location: " + tpDataSubset.projectPath() + "autoData.bin");
    outputln();
    outputln<float>(autoData);
    outputln("--"+MIMEBOUNDARY_2+"--");
  }

  void SDMDataObjectWriter::tpDataHeader(uint64_t startTime,
					 const string& execBlockUID,
					 uint32_t execBlockNum,
					 uint32_t scanNum,
					 uint32_t subscanNum,
					 uint32_t numAntenna,
					 SDMDataObject::DataStruct& dataStruct) {
    checkState(T_TPDATAHEADER, "tpDataHeader");

    sdmDataObject_.valid_ = true;

    sdmDataObject_.startTime_ = startTime;
    sdmDataObject_.dataOID_ = uid_;
    sdmDataObject_.title_ = title_;
    sdmDataObject_.dimensionality_ = 1;
    sdmDataObject_.execBlockUID_ = execBlockUID;
    sdmDataObject_.execBlockNum_ = execBlockNum;
    sdmDataObject_.scanNum_ = scanNum;
    sdmDataObject_.subscanNum_ = subscanNum;
    sdmDataObject_.numAntenna_ = numAntenna;
    sdmDataObject_.correlationMode_ = AUTO_ONLY;
    sdmDataObject_.spectralResolutionType_ = BASEBAND_WIDE;
    sdmDataObject_.processorType_ = RADIOMETER;
    sdmDataObject_.dataStruct_ = dataStruct;

    outputln("--"+MIMEBOUNDARY_1);
    outputln("Content-Type: text/xml; charset=\"UTF-8\"");
    outputln("Content-Transfer-Encoding: 8bit");    //outputln("Content-Location: " + sdmDataObject_.projectPath() + "desc.xml");
    outputln("Content-Location: sdmDataHeader.xml");
    outputln();

    outputln(sdmDataObject_.toXML());
  }

  void SDMDataObjectWriter::tpAddIntegration(uint32_t integrationNum,
                                             uint64_t time,
                                             uint64_t interval,
                                             const vector<FLAGSTYPE>& flags,
                                             const vector<ACTUALTIMESTYPE>& actualTimes,
                                             const vector<ACTUALDURATIONSTYPE>& actualDurations,
                                             const vector<AUTODATATYPE>& autoData) {

    checkState(T_ADDTPINTEGRATION, "tpAddIntegration");

    SDMDataSubset sdmDataSubset(&sdmDataObject_);
    sdmDataObject_.numTime_++;
    sdmDataSubsetNum_++;

    // integrationNum and subintegrationNum.
    sdmDataSubset.integrationNum_    = integrationNum;
    sdmDataSubset.subintegrationNum_ = 0;

    // The time.
    sdmDataSubset.time_ = time;

    // The interval.
    sdmDataSubset.interval_ = interval;

    // Attachments size;
    sdmDataSubset.nActualTimes_     = actualTimes.size();
    sdmDataSubset.nActualDurations_ = actualDurations.size();
    sdmDataSubset.nZeroLags_ = 0;
    sdmDataSubset.nCrossData_ = 0;

    sdmDataSubset.nFlags_   = flags.size();

    sdmDataSubset.nAutoData_ = autoData.size();

    outputln("--"+MIMEBOUNDARY_1);
    outputln("Content-Type: Multipart/Related; boundary=\""+MIMEBOUNDARY_2+"\";type=\"text/xml\"");
    outputln("Content-Description: Data and metadata subset");
    outputln("--"+MIMEBOUNDARY_2);
    outputln("Content-Type: text/xml; charset=\"UTF-8\"");
    outputln("Content-Location: " + sdmDataSubset.projectPath() + "desc.xml");
    outputln();
    outputln(sdmDataSubset.toXML());

    // Write flags, if any
    if (flags.size() != 0) {
      uint32_t numFlags = sdmDataObject_.dataStruct_.flags_.size();
      if (numFlags !=0 && numFlags != flags.size()) {
        ostringstream oss;
        oss << "The number of values provided for 'flags' ("
            << flags.size()
            << "), is not equal to the number declared in the global header ("
            << numFlags << ").";
        throw SDMDataObjectWriterException(oss.str());
      }
      outputln("--"+MIMEBOUNDARY_2);
      outputln("Content-Type: binary/octet-stream");
      outputlnLocation("flags", sdmDataSubset);
      outputln();
      outputln<uint32_t>(flags);
    }

    // Write actual times, if any
    if (actualTimes.size() != 0) {
      uint32_t numActualTimes = sdmDataObject_.dataStruct_.actualTimes_.size();
      if (numActualTimes != 0 && numActualTimes != actualTimes.size()) {
        ostringstream oss;
        oss << "The number of values provided for 'actualTimes' ("
            << actualTimes.size()
            << "), is not equal to the number declared in the global header ("
            << numActualTimes << ").";
        throw SDMDataObjectWriterException(oss.str());
      }
      outputln("--"+MIMEBOUNDARY_2);
      outputln("Content-Type: binary/octet-stream");
      outputlnLocation("actualTimes", sdmDataSubset);
      outputln();
      outputln<ACTUALTIMESTYPE>(actualTimes);
    }

    // Write actual durations, if any
    if (actualDurations.size() != 0) {
      uint32_t numActualDurations = sdmDataObject_.dataStruct_.actualDurations_.size();
      if (numActualDurations != 0 && numActualDurations != actualDurations.size()) {
        ostringstream oss;
        oss << "The number of values provided for 'actualDurations' ("
            << actualDurations.size()
            << "), is not equal to the number declared in the global header ("
            << numActualDurations << ").";
        throw SDMDataObjectWriterException(oss.str());
      }
      outputln("--"+MIMEBOUNDARY_2);
      outputln("Content-Type: binary/octet-stream");
      outputlnLocation("actualDurations", sdmDataSubset);
      outputln();
      outputln<ACTUALDURATIONSTYPE>(actualDurations);
    }

    // Write auto data
    uint32_t numAutoData = sdmDataObject_.dataStruct_.autoData_.size();
    if (numAutoData != autoData.size()) {
      ostringstream oss;
      oss << "The number of values provided for 'autoData' ("
          << autoData.size()
          << "), is not equal to the number declared in the global header ("
          << numAutoData << ").";
      throw SDMDataObjectWriterException(oss.str());
    }
    outputln("--"+MIMEBOUNDARY_2);
    outputln("Content-Type: binary/octet-stream");
    outputlnLocation("autoData", sdmDataSubset);
    outputln();
    outputln<float>(autoData);

    outputln("--"+MIMEBOUNDARY_2+"--");
  }


  void SDMDataObjectWriter::wvrData (const string & 		execBlockUID,
				     uint32_t 		execBlockNum,
				     uint32_t 		scanNum,
				     uint32_t 		subscanNum,
				     uint32_t 		numTimes,
				     uint32_t 		numAntennas,
				     uint32_t 		numChannels,
				     NetSideband  		netSideband,
				     uint64_t 	time,
				     uint64_t 	interval,
				     const vector<float>& 	wvrData,
				     const vector<uint32_t>& flags) {
    checkState(T_WVRDATA, "wvrData");

    //
    //
    // Check situations which raise exceptions.
    //
    ostringstream oss;
    if (numTimes == 0 || numAntennas == 0 || numChannels == 0) {
      oss << "At least one of these arguments is nul : numTimes (" << numTimes << "), "
	  << "numAntennas (" << numAntennas << "'), "
	  << "numChannels (" << numChannels << "')."
	  << endl;
      throw SDMDataObjectWriterException(oss.str());
    }

    if (wvrData.size() != numTimes * numAntennas * numChannels) {
      oss << "The size of 'wvrData' is " << wvrData.size()
	  << ". It is expected to be " << numTimes * numAntennas * numChannels
	  << "." << endl;
      throw SDMDataObjectWriterException(oss.str());
    }

    if (flags.size() !=0 && flags.size() != numTimes * numAntennas) {
      oss << "The size of 'flags' is " << flags.size() 
	  << ". It is expected to be wether null or equal to " 
	  << numTimes * numAntennas << endl;
      throw SDMDataObjectWriterException(oss.str());
    }

    //
    // Prepare the embedded SDMDataObject Main header
    // 
    vector<StokesParameter> 			sdPolProducts(1, I);
    vector<SDMDataObject::SpectralWindow> 	spectralWindows(1, SDMDataObject::SpectralWindow(sdPolProducts, numChannels, 1, netSideband));
    vector<SDMDataObject::Baseband> 		baseBands(1, SDMDataObject::Baseband(NOBB, spectralWindows));

    sdmDataObject_.valid_                  = true;
    sdmDataObject_.startTime_      	   = time - interval / 2;
    sdmDataObject_.dataOID_        	   = uid_;
    sdmDataObject_.title_          	   = title_;
    sdmDataObject_.dimensionality_ 	   = 0;
    sdmDataObject_.numTime_        	   = numTimes;
    sdmDataObject_.execBlockUID_   	   = execBlockUID;
    sdmDataObject_.execBlockNum_           = execBlockNum;
    sdmDataObject_.scanNum_                = scanNum;
    sdmDataObject_.subscanNum_             = subscanNum;
    sdmDataObject_.numAntenna_             = numAntennas;
    sdmDataObject_.correlationMode_        = AUTO_ONLY;
    sdmDataObject_.spectralResolutionType_ = FULL_RESOLUTION;
    sdmDataObject_.processorType_          = RADIOMETER;
    sdmDataObject_.dataStruct_.basebands_  = baseBands;
    sdmDataObject_.dataStruct_.autoData_   = SDMDataObject::AutoDataBinaryPart(wvrData.size(), WVRDATAAXES, true);
    if (flags.size()) sdmDataObject_.dataStruct_.flags_ = SDMDataObject::BinaryPart(flags.size(), WVRDATAFLAGSAXES); 
    
    //
    // And output it.
    outputln("--"+MIMEBOUNDARY_1);
    outputln("Content-Type: text/xml; charset=\"UTF-8\"");
    outputln("Content-Transfer-Encoding: 8bit");
    outputln("Content-Location: " + sdmDataObject_.projectPath() + "desc.xml");
    outputln();

    outputln(sdmDataObject_.toXML());
    outputln("--"+MIMEBOUNDARY_1);
    outputln("Content-Type: Multipart/Related; boundary=\""+MIMEBOUNDARY_2+"\";type=\"text/xml\"; start=\"<DataSubset.xml>\"");
    outputln("Content-Description: Data and metadata subset");
    outputln("--"+MIMEBOUNDARY_2);
    outputln("Content-Type: text/xml; charset=\"UTF-8\"");
    outputln("Content-Location: " + sdmDataObject_.projectPath() + "desc.xml");
    outputln();

    //
    //
    // Prepare the unique SDMDataSubset
    //
    SDMDataSubset wvrDataSubset(&sdmDataObject_,
			       time,
			       interval,
			       wvrData);
    wvrDataSubset.flags_ = (wvrDataSubset.nFlags_ = flags.size()) ? &wvrDataSubset.flags_[0] : 0;
    
    //
    // And output it.
    // 
    outputln(wvrDataSubset.toXML());

    if (flags.size() != 0) {
      outputln("--"+MIMEBOUNDARY_2);
      outputln("Content-Type: binary/octet-stream");
      outputln("Content-Location: " + wvrDataSubset.projectPath() + "flags.bin");
      outputln();
      outputln<uint32_t>(flags);
    }
    
    outputln("--"+MIMEBOUNDARY_2);
    outputln("Content-Type: binary/octet-stream");
    outputln("Content-Location: " + wvrDataSubset.projectPath() + "autoData.bin");
    outputln();
    
    outputln<float>(wvrData);
    outputln("--"+MIMEBOUNDARY_2+"--");
  }
  
  
  /**
   * Writes the XML global header into its attachment on the MIME message stream.
   */
  void SDMDataObjectWriter::corrDataHeader(uint64_t startTime,
					   const string& execBlockUID,
					   uint32_t execBlockNum,
					   uint32_t scanNum,
					   uint32_t subscanNum,
					   uint32_t numAntenna,
					   CorrelationMode correlationMode,
					   const OptionalSpectralResolutionType& spectralResolutionType,
					   SDMDataObject::DataStruct& dataStruct) {
    checkState(T_CORRDATAHEADER, "corrDataHeader");
    
    ostringstream oss;
    oss << "/" << execBlockNum << "/" << scanNum << "/" << subscanNum;
    subscanPath_ = oss.str();
    
    sdmDataObject_.valid_ = true;

    sdmDataObject_.title_ = title_;
    sdmDataObject_.startTime_ = startTime;
    sdmDataObject_.dataOID_ = uid_;
    sdmDataObject_.dimensionality_ = 1;
    sdmDataObject_.numTime_ = 0;
    sdmDataObject_.execBlockUID_ = execBlockUID;
    sdmDataObject_.execBlockNum_ = execBlockNum;
    sdmDataObject_.scanNum_ = scanNum;
    sdmDataObject_.subscanNum_ = subscanNum;
    sdmDataObject_.numAntenna_ = numAntenna;
    sdmDataObject_.correlationMode_ = correlationMode;
    sdmDataObject_.spectralResolutionType_ = spectralResolutionType;
    sdmDataObject_.processorType_ = CORRELATOR;
    sdmDataObject_.dataStruct_ = dataStruct;

    outputln("--"+MIMEBOUNDARY_1);
    outputln("Content-Type: text/xml; charset=\"UTF-8\"");
    outputln("Content-Transfer-Encoding: 8bit");
    outputln("Content-Location: " + sdmDataObject_.projectPath() + "desc.xml");
    outputln();

    outputln(sdmDataObject_.toXML());
  }
  

  void SDMDataObjectWriter::addData(uint32_t integrationNum,
				    uint32_t subintegrationNum,
				    uint64_t time,
				    uint64_t interval,
				    const vector<FLAGSTYPE>& flags,
				    const vector<ACTUALTIMESTYPE>& actualTimes,
				    const vector<ACTUALDURATIONSTYPE>& actualDurations,
				    const vector<ZEROLAGSTYPE>& zeroLags,
				    const vector<INTCROSSDATATYPE>& intCrossData,
				    const vector<SHORTCROSSDATATYPE>& shortCrossData,
				    const vector<FLOATCROSSDATATYPE>& floatCrossData,
				    const vector<AUTODATATYPE>& autoData) {
    SDMDataSubset sdmDataSubset(&sdmDataObject_);
    sdmDataObject_.numTime_++;
    sdmDataSubsetNum_++;
    
    // integrationNum and subintegrationNum.
    sdmDataSubset.integrationNum_    = integrationNum;
    sdmDataSubset.subintegrationNum_ = subintegrationNum?subintegrationNum:0;

    // The time.
    sdmDataSubset.time_ = time;

    // The interval.
    sdmDataSubset.interval_ = interval;

    // The crossDataType.
    if (intCrossData.size() != 0) 
      sdmDataSubset.crossDataType_ = INT32_TYPE;

    else if (shortCrossData.size() != 0)
      sdmDataSubset.crossDataType_ = INT16_TYPE;

    else if (floatCrossData.size() != 0) 
      sdmDataSubset.crossDataType_ = FLOAT32_TYPE;

    // Attachments size;
    sdmDataSubset.nActualTimes_     = actualTimes.size();
    sdmDataSubset.nActualDurations_ = actualDurations.size();
    
    // Ignore unconditionally zeroLags for FXF ~ ACA)
    if (sdmDataObject_.correlatorType() != FX) {
      sdmDataSubset.nZeroLags_        = zeroLags.size();
    }
    else
      sdmDataSubset.nZeroLags_ = 0;

    sdmDataSubset.nFlags_   = flags.size();
    sdmDataSubset.nFlags_   = flags.size();
    switch (sdmDataSubset.crossDataType_) {
    case INT32_TYPE:
      sdmDataSubset.nCrossData_ = intCrossData.size();
      break;
    case INT16_TYPE:
      sdmDataSubset.nCrossData_ = shortCrossData.size();
      break;
    case FLOAT32_TYPE:
      sdmDataSubset.nCrossData_ = floatCrossData.size();
      break;
    default:
      sdmDataSubset.nCrossData_ = 0;
    }

    //sdmDataSubset.nCrossData_       = shortCrossData.size() ? shortCrossData.size():intCrossData.size();

    sdmDataSubset.nAutoData_        = autoData.size();

    outputln("--"+MIMEBOUNDARY_1);
    outputln("Content-Type: Multipart/Related; boundary=\""+MIMEBOUNDARY_2+"\";type=\"text/xml\"");
    outputln("Content-Description: Data and metadata subset");
    outputln("--"+MIMEBOUNDARY_2);
    outputln("Content-Type: text/xml; charset=\"UTF-8\"");
    outputln("Content-Location: " + sdmDataSubset.projectPath() + "desc.xml");
    outputln();
    outputln(sdmDataSubset.toXML());
    

    if (flags.size() != 0) {
      uint32_t numFlags = sdmDataObject_.dataStruct_.flags_.size();
      if (numFlags !=0 && numFlags != flags.size()) {
	ostringstream oss;
	oss << "The number of values provided for 'flags' ("
	    << flags.size()
	    << "), is not equal to the number declared in the global header ("
	    << numFlags << ").";
	throw SDMDataObjectWriterException(oss.str());
      }
      outputln("--"+MIMEBOUNDARY_2);
      outputln("Content-Type: binary/octet-stream");
      outputlnLocation("flags", sdmDataSubset);
      outputln();
      outputln<uint32_t>(flags);
    }
    

    if (actualTimes.size() != 0) {
      uint32_t numActualTimes = sdmDataObject_.dataStruct_.actualTimes_.size();
      if (numActualTimes != 0 && numActualTimes != actualTimes.size()) {
	ostringstream oss;
	oss << "The number of values provided for 'actualTimes' ("
	    << actualTimes.size()
	    << "), is not equal to the number declared in the global header ("
	    << numActualTimes << ").";
	throw SDMDataObjectWriterException(oss.str());
      }
      outputln("--"+MIMEBOUNDARY_2);
      outputln("Content-Type: binary/octet-stream");
      outputlnLocation("actualTimes", sdmDataSubset);
      outputln();
      outputln<ACTUALTIMESTYPE>(actualTimes);
    }

    if (actualDurations.size() != 0) {
      uint32_t numActualDurations = sdmDataObject_.dataStruct_.actualDurations_.size();
      if (numActualDurations != 0 && numActualDurations != actualDurations.size()) {
	ostringstream oss;
	oss << "The number of values provided for 'actualDurations' ("
	    << actualDurations.size()
	    << "), is not equal to the number declared in the global header ("
	    << numActualDurations << ").";
	throw SDMDataObjectWriterException(oss.str());
      }
      outputln("--"+MIMEBOUNDARY_2);
      outputln("Content-Type: binary/octet-stream");
      outputlnLocation("actualDurations", sdmDataSubset);
      outputln();
      outputln<ACTUALDURATIONSTYPE>(actualDurations);
    }
    
    
    if (sdmDataObject_.correlationMode_ != AUTO_ONLY) {
      int numCrossData = sdmDataObject_.dataStruct_.crossData_.size();
      int numCrossDataV = 0; //= intCrossData.size() ? intCrossData.size():shortCrossData.size();
      switch(sdmDataSubset.crossDataType_) {
      case INT32_TYPE:
	numCrossDataV = intCrossData.size();
	break;
      case INT16_TYPE:
	numCrossDataV = shortCrossData.size();
	break;
      case FLOAT32_TYPE:
	numCrossDataV = floatCrossData.size();
	break;
      default:
	break;
      }
      if (numCrossData != numCrossDataV) {
	ostringstream oss;
	oss << "The number of values provided for 'crossData' ("
	    << numCrossDataV
	    << "), is not equal to the number declared in the global header ("
	    << numCrossData << ").";
	throw SDMDataObjectWriterException(oss.str());
      }
      outputln("--"+MIMEBOUNDARY_2);
      outputln("Content-Type: binary/octet-stream");
      outputlnLocation("crossData", sdmDataSubset);
      outputln();
      switch (sdmDataSubset.crossDataType_) {
      case INT32_TYPE:
	outputln<int>(intCrossData);
	break;
      case INT16_TYPE:
	outputln<short>(shortCrossData);
	break;
      case FLOAT32_TYPE:
	outputln<float>(floatCrossData);
	break;
      default: 
	throw SDMDataObjectWriterException("'" + CPrimitiveDataType::name(sdmDataSubset.crossDataType_)+"' data are not processed here."); 
      }
    }

    if (sdmDataObject_.correlationMode_ !=  CROSS_ONLY) {
      uint32_t numAutoData = sdmDataObject_.dataStruct_.autoData_.size();
      if (numAutoData != autoData.size()) {
	ostringstream oss;
	oss << "The number of values provided for 'autoData' ("
	    << autoData.size()
	    << "), is not equal to the number declared in the global header ("
	    << numAutoData << ").";
	throw SDMDataObjectWriterException(oss.str());
      }
      outputln("--"+MIMEBOUNDARY_2);
      outputln("Content-Type: binary/octet-stream");
      outputlnLocation("autoData", sdmDataSubset);
      outputln();
      outputln<float>(autoData);
    }
    
    // if (sdmDataObject_.spectralResolutionType_ != CHANNEL_AVERAGE) {  
    // Now the zeroLags are optionally allowed in CHANNEL_AVERAGE
    // zeroLags are now optional in any case - Michel Caillat - 24 Jul 2008
    uint32_t numZeroLags = sdmDataObject_.dataStruct_.zeroLags_.size();
    if (numZeroLags > 0) {  
      if (numZeroLags != zeroLags.size()) {
	ostringstream oss;
	oss << "The number of values provided for 'zeroLags' ("
	    << zeroLags.size()
	    << "), is not equal to the number declared in the global header ("
	    << numZeroLags << ").";
	throw SDMDataObjectWriterException(oss.str());
      }
      outputln("--"+MIMEBOUNDARY_2);
      outputln("Content-Type: binary/octet-stream");
      outputlnLocation("zeroLags", sdmDataSubset);
      outputln();
      outputln<float>(zeroLags);
    }

    outputln("--"+MIMEBOUNDARY_2+"--");
  }

  void SDMDataObjectWriter::addIntegration(uint32_t integrationNum,
					   uint64_t time,
					   uint64_t interval,
					   const vector<FLAGSTYPE>& flags,
					   const vector<ACTUALTIMESTYPE>& actualTimes,
					   const vector<ACTUALDURATIONSTYPE>& actualDurations,
					   const vector<ZEROLAGSTYPE>& zeroLags,
					   const vector<INTCROSSDATATYPE>& crossData,
					   const vector<AUTODATATYPE>& autoData) {

    checkState(T_ADDINTEGRATION, "addIntegration");

    vector<short> emptyShort;
    vector<float> emptyFloat;
    addData(integrationNum,
	    0,
	    time,
	    interval,
	    flags,
	    actualTimes,
	    actualDurations,
	    zeroLags,
	    crossData,
	    emptyShort,
	    emptyFloat,
	    autoData);		
  }
  

  void SDMDataObjectWriter::addIntegration(uint32_t integrationNum,
					   uint64_t time,
					   uint64_t interval,
					   const vector<FLAGSTYPE>& flags,
					   const vector<ACTUALTIMESTYPE>& actualTimes,
					   const vector<ACTUALDURATIONSTYPE>& actualDurations,
					   const vector<ZEROLAGSTYPE>& zeroLags,
					   const vector<SHORTCROSSDATATYPE>& crossData,
					   const vector<AUTODATATYPE>& autoData) {
    checkState(T_ADDINTEGRATION, "addIntegration");

    vector<int> emptyLong;
    vector<float> emptyFloat;
    addData(integrationNum,
	    0,
	    time,
	    interval,
	    flags,
	    actualTimes,
	    actualDurations,
	    zeroLags,
	    emptyLong,
	    crossData,
	    emptyFloat,
	    autoData);		
  }

  void SDMDataObjectWriter::addIntegration(uint32_t integrationNum,
					   uint64_t time,
					   uint64_t interval,
					   const vector<FLAGSTYPE>& flags,
					   const vector<ACTUALTIMESTYPE>& actualTimes,
					   const vector<ACTUALDURATIONSTYPE>& actualDurations,
					   const vector<ZEROLAGSTYPE>& zeroLags,
					   const vector<FLOATCROSSDATATYPE>& crossData,
					   const vector<AUTODATATYPE>& autoData) {
    checkState(T_ADDINTEGRATION, "addIntegration");

    vector<int> emptyLong;
    vector<short> emptyShort;
    addData(integrationNum,
	    0,
	    time,
	    interval,
	    flags,
	    actualTimes,
	    actualDurations,
	    zeroLags,
	    emptyLong,
	    emptyShort,
	    crossData,
	    autoData);		
  }

  void SDMDataObjectWriter::addSubintegration(uint32_t integrationNum,
					      uint32_t subIntegrationNum,
					      uint64_t time,
					      uint64_t interval,
					      const vector<FLAGSTYPE>& flags,
					      const vector<ACTUALTIMESTYPE>& actualTimes,
					      const vector<ACTUALDURATIONSTYPE>& actualDurations,
					      const vector<ZEROLAGSTYPE>& zeroLags,
					      const vector<SHORTCROSSDATATYPE>& crossData,
					      const vector<AUTODATATYPE>& autoData) {
    checkState(T_ADDSUBINTEGRATION, "addSubintegration");

    vector<int> emptyLong;
    vector<float> emptyFloat;
    addData(integrationNum,
	    subIntegrationNum,
	    time,
	    interval,
	    flags,
	    actualTimes,
	    actualDurations,
	    zeroLags,
	    emptyLong,
	    crossData,
	    emptyFloat,
	    autoData);		
  }

  void SDMDataObjectWriter::addSubintegration(uint32_t integrationNum,
					      uint32_t subIntegrationNum,
					      uint64_t time,
					      uint64_t interval,
					      const vector<FLAGSTYPE>& flags,
					      const vector<ACTUALTIMESTYPE>& actualTimes,
					      const vector<ACTUALDURATIONSTYPE>& actualDurations,
					      const vector<ZEROLAGSTYPE>& zeroLags,
					      const vector<INTCROSSDATATYPE>& crossData,
					      const vector<AUTODATATYPE>& autoData) {
    checkState(T_ADDSUBINTEGRATION, "addSubIntegration");

    vector<short> emptyShort;
    vector<float> emptyFloat;
    addData(integrationNum,
	    subIntegrationNum,
	    time,
	    interval,
	    flags,
	    actualTimes,
	    actualDurations,
	    zeroLags,
	    crossData,
	    emptyShort,
	    emptyFloat,
	    autoData);		
  }

  void SDMDataObjectWriter::addSubintegration(uint32_t integrationNum,
					      uint32_t subIntegrationNum,
					      uint64_t time,
					      uint64_t interval,
					      const vector<FLAGSTYPE>& flags,
					      const vector<ACTUALTIMESTYPE>& actualTimes,
					      const vector<ACTUALDURATIONSTYPE>& actualDurations,
					      const vector<ZEROLAGSTYPE>& zeroLags,
					      const vector<FLOATCROSSDATATYPE>& crossData,
					      const vector<AUTODATATYPE>& autoData) {
    checkState(T_ADDSUBINTEGRATION, "addSubIntegration");

    vector<int> emptyLong;
    vector<short> emptyShort;
    addData(integrationNum,
	    subIntegrationNum,
	    time,
	    interval,
	    flags,
	    actualTimes,
	    actualDurations,
	    zeroLags,
	    emptyLong,
	    emptyShort,
	    crossData,
	    autoData);		
  }

  uint64_t SDMDataObjectWriter::numBytes() { return numBytes_; }
  
  
  bool SDMDataObjectWriter::initClass() {
    //
    // WVR data are stored in the following order : TIM, ANT, SPP  (SPP varying faster then ANT varying itself faster than TIM)
    //
    WVRDATAAXES.push_back(TIM); 
    WVRDATAAXES.push_back(ANT); 
    WVRDATAAXES.push_back(SPP);

    //
    // WVR data flags are stored in the following order : TIM, ANT (ANT varying itself faster than TIM)
    //
    WVRDATAFLAGSAXES.push_back(TIM); 
    WVRDATAFLAGSAXES.push_back(ANT); 
 
    return true;
  }

void SDMDataObjectWriter:: checkState(Transitions t, const string& methodName) {
    switch(currentState_) {
    case START:
        if (t == T_TPDATA ) { 
            currentState_ = S_TPDATA;
            return;
        }
        else if (t == T_TPDATAHEADER ) {
            currentState_ = S_TPDATAHEADER;
            return;
        }
        else if (t == T_CORRDATAHEADER) {
            currentState_ = S_CORRDATAHEADER;
            return;
        }
        else if (t == T_WVRDATA) {
            currentState_ = S_WVRDATA;
            return;
        }
        break;

    case S_TPDATA:
        if (t == T_DONE) {
            currentState_ = END;
            return;
        }
        break;

      
    case S_TPDATAHEADER:
        if (t == T_ADDTPSUBSCAN) {
            currentState_ = S_ADDTPSUBSCAN;
            return;
        }
        else if (t == T_ADDTPINTEGRATION) {
            currentState_ = S_ADDTPINTEGRATION;
            return;
        }
        break;

    case S_ADDTPINTEGRATION:
        if (t == T_ADDTPINTEGRATION) {
            return;
        }
        else if ( t == T_DONE ) {
            currentState_ = END;
            return;
        }
        break;

    case S_ADDTPSUBSCAN:
        if ( t == T_DONE ) {
            currentState_ = END;
            return;
        }
        break;

    case S_WVRDATA:
        if (t == T_DONE) {
            currentState_ = END;
            return;
        }
        break;

    case S_CORRDATAHEADER:
        if (t == T_ADDINTEGRATION) {
            currentState_ = S_ADDINTEGRATION;
            return;
        }
        else if (t == T_ADDSUBINTEGRATION) {
            currentState_ = S_ADDSUBINTEGRATION;
            return;
        }
        break;
      
    case S_ADDINTEGRATION:
        if (t == T_ADDINTEGRATION)
            return;
        else if (t == T_DONE) {
            currentState_ = END;
            return;
        }
        break;

      
    case S_ADDSUBINTEGRATION:
        if (t == T_ADDSUBINTEGRATION)
            return;
        else if (t == T_DONE) {
            currentState_ = END;
            return;
        }
        break;

	
    case END:
        break;
    }
    throw SDMDataObjectWriterException("Invalid call of method '" + methodName + "'");
}
}


