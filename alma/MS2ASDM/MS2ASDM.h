//# MS2ASDM.h 
//#
//#  ALMA - Atacama Large Millimeter Array
//#  (c) European Southern Observatory, 2002
//#  (c) Associated Universities Inc., 2002
//#  Copyright by ESO (in the framework of the ALMA collaboration),
//#  Copyright by AUI (in the framework of the ALMA collaboration),
//#  All rights reserved.
//#  
//#  This library is free software; you can redistribute it and/or
//#  modify it under the terms of the GNU Lesser General Public
//#  License as published by the Free software Foundation; either
//#  version 2.1 of the License, or (at your option) any later version.
//#  
//#  This library is distributed in the hope that it will be useful,
//#  but WITHOUT ANY WARRANTY, without even the implied warranty of
//#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//#  Lesser General Public License for more details.
//#  
//#  You should have received a copy of the GNU Lesser General Public
//#  License along with this library; if not, write to the Free Software
//#  Foundation, Inc., 59 Temple Place, Suite 330, Boston,
//#  MA 02111-1307  USA
//# $Id: $
#include <ms/MeasurementSets/MeasurementSet.h>
#include <ms/MeasurementSets/MSColumns.h>
#include <casa/aips.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/Vector.h>
#include <map>
#include <vector>
#include <casa/OS/Directory.h>

#include <alma/ASDM/ASDM.h>
#include <alma/ASDM/Tag.h>
#include <alma/ASDM/ComplexWrapper.h>
#include <alma/ASDM/Frequency.h>
#include <alma/ASDM/Angle.h>
#include <alma/ASDM/AngularRate.h>
#include <alma/ASDM/ArrayTime.h>
#include <alma/ASDM/Length.h>
#include <alma/ASDM/Temperature.h>
#include <alma/ASDM/ArrayTimeInterval.h>
#include <alma/ASDM/EntityRef.h>
#include <alma/Enumerations/CStokesParameter.h>
#include <alma/Enumerations/CAntennaType.h>
#include <alma/Enumerations/CBasebandName.h>
#include <alma/Enumerations/CNetSideband.h>
#include <alma/Enumerations/CFrequencyReferenceCode.h>
#include <alma/Enumerations/CReceiverBand.h>
#include <alma/Enumerations/CReceiverSideband.h>
#include <alma/MS2ASDM/MapWithDefault.h>


#ifndef MS2ASDM_MS2ASDM_H
#define MS2ASDM_MS2ASDM_H

namespace casa { //# NAMESPACE CASA - BEGIN

// <summary>
// MS2ASDM provides functionalities to create an ASDM (ALMA science data model)
// from an existing MS
// </summary>

// <visibility=export>

// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>

// <prerequisite>
//   <li> MeasurementSet
// </prerequisite>
//
// <etymology>
// </etymology>
//
// <synopsis>
// </synopsis>

  class MS2ASDM : public casacore::MSColumns
{

 public:
  
  // construct from an MS
  MS2ASDM(casacore::MeasurementSet& ms);
  
  ~MS2ASDM();
  
  const casacore::String& showversion();

  // set verbosity of the write methods
  void setVerbosity(const casacore::uInt verbosity = 2){ // 0 = only warnings, 1 = most, 2 = everything
    verbosity_p = verbosity; }
  
  void setBaseUid(const casacore::String& baseuid);
  
  const casacore::String& getBaseUid();
  
  const std::string& getCurrentUid();
  
  // return currentUid_p with all ":" and "/" characters replaced by "_"
  const std::string& getCurrentUidAsFileName();

  // set maximum duration of a subscan in seconds, 0. == no time limit
  void setSubScanDuration(const casacore::Double subscanDuration = 24.*3600.){
    subscanDuration_p = subscanDuration; }

  // get maximum duration of a subscan in seconds
  casacore::Double getSubScanDuration(){ return subscanDuration_p; }

  // set maximum duration of a Scheduling casacore::Block in seconds
  void setSBDuration(const casacore::Double sBDuration = 2700.){ // 45 minutes
    schedBlockDuration_p = sBDuration; }

  // get maximum duration of a Scheduling casacore::Block in seconds
  casacore::Double getSBDuration(){ return schedBlockDuration_p; }

  void setDataAPCorrected(const casacore::Bool isCorrected = true){
    dataIsAPCorrected_p = isCorrected; }

  casacore::Bool dataIsAPCorrected(){ return dataIsAPCorrected_p; }

  void setObservatoryName(const casacore::String& telName){
    telName_p = telName; }

  void getObservatoryName( casacore::String& telName ){
    telName = telName_p; }

  // convert CASA casacore::Stokes to ASDM Stokes
  StokesParameterMod::StokesParameter ASDMStokesParameter( casacore::Stokes::StokesTypes s);

  // convert CASA antenna type string to ASDM antenna type enum
  AntennaTypeMod::AntennaType ASDMAntennaType( const casacore::String& type ); 

  // convert time in seconds to an array time
  asdm::ArrayTime ASDMArrayTime( const casacore::Double seconds ){ 
      return asdm::ArrayTime((int64_t) (floor(seconds*asdm::ArrayTime::unitsInASecond))); }

  // convert array time to time in seconds
  casacore::Double MSTimeSecs( const asdm::ArrayTime atime ){ 
      return (casacore::Double) atime.get() / (casacore::Double) asdm::ArrayTime::unitsInASecond; }

  asdm::Interval ASDMInterval( const casacore::Double seconds ){ 
      return asdm::Interval((int64_t) (floor(seconds*asdm::ArrayTime::unitsInASecond))); }

  // convert casacore::MS style time interval to ASDM ArrayTimeInterval
  asdm::ArrayTimeInterval ASDMTimeInterval( const casacore::Quantity midpoint, const casacore::Quantity interval);

  // return start of casacore::MS main table timestamp (seconds)
  casacore::Double timestampStartSecs(const casacore::uInt mainTabRow){
    return timeQuant()(mainTabRow).getValue("s") - intervalQuant()(mainTabRow).getValue("s")/2.; }

  // return end of casacore::MS main table timestamp (seconds)
  casacore::Double timestampEndSecs(const casacore::uInt mainTabRow){
    return timeQuant()(mainTabRow).getValue("s") + intervalQuant()(mainTabRow).getValue("s")/2.; }

  // convert casacore::MDirection to a vector of Angles
  std::vector< asdm::Angle > ASDMAngleV(const casacore::MDirection mDir);

  // convert casacore::MDirection type to ASDM DirectionReferenceCode
  DirectionReferenceCodeMod::DirectionReferenceCode ASDMDirRefCode(const casacore::MDirection::Types type);

  // convert a base band converter number to an ASDM base band name
  BasebandNameMod::BasebandName ASDMBBName( const casacore::Int bbcNo );

  // convert a casacore::MS net sideband no. to an ASDM enum
  NetSidebandMod::NetSideband ASDMNetSideBand( const casacore::Int netSideband );

  // set a representative frequency, the receiver band and receiver sideband based on a frequency refFreq
  //   and the previously set observatory name telName_p, return the band id as an casacore::Int (1 to 10),
  //   -1 if refFreq is outside ALMA bands but observatory is ALMA, 0 if observatory not ALMA
  casacore::Int setRecBands( const asdm::Frequency refFreq,
		   casacore::Double& frequency,
		   ReceiverBandMod::ReceiverBand& frequencyBand,
		   ReceiverSidebandMod::ReceiverSideband& receiverSideband);

  FrequencyReferenceCodeMod::FrequencyReferenceCode ASDMFreqRefCode( const casacore::MFrequency::Types refFrame ); 

  casacore::Unit unitASDMFreq(){ return casacore::Unit(casacore::String(asdm::Frequency::unit())); }

  casacore::Unit unitASDMAngle(){ return casacore::Unit(casacore::String(asdm::Angle::unit())); }

  casacore::Unit unitASDMAngularRate(){ return casacore::Unit(casacore::String(asdm::AngularRate::unit())); }

  casacore::Unit unitASDMLength(){ return casacore::Unit(casacore::String(asdm::Length::unit())); }

  casacore::Unit unitASDMTemp(){ return casacore::Unit(casacore::String(asdm::Temperature::unit())); }

  asdm::Complex ASDMComplex( casacore::Complex x ){ return asdm::Complex(x.real(), x.imag()); }

  // write the entire ASDM from scratch
  casacore::Bool writeASDM(const casacore::String& asdmfile="", 
		 const casacore::String& datacolumn="data", 
		 const casacore::String& archiveid="S0", 
		 const casacore::String& rangeid="X1", 
		 const casacore::Bool verbose=true,
		 const casacore::Double maxSubscanDuration = 24.*3600.,
		 const casacore::Double maxSchedBlockDuration = 2700.,
		 const casacore::Bool msDataIsAPCorrected=true
		 );

 private:
  // *** Private member functions ***

  casacore::Bool incrementUid(); // returns true if successful

  casacore::Bool setDirectory(const casacore::String& asdmfile);


  casacore::Bool writeStation();

  casacore::Bool writeAntenna();

  casacore::Bool writeSpectralWindow();

  casacore::Bool writeSource();

  casacore::Bool writePolarization();

  casacore::Bool writeCorrelatorMode(); // not called directly but optionally called by writeProcessor()
  casacore::Bool writeAlmaRadiometer(); // optionally called by writeProcessor()
  casacore::Bool writeHolography(); // optionally called by writeProcessor()

  casacore::Bool writeProcessor();

  casacore::Bool writeField();

  casacore::Bool writeReceiver();

  casacore::Bool writeFeed();

  casacore::Bool writeDataDescription();

  casacore::Bool writeSwitchCycle(); // not yet fully implemented

  casacore::Bool writeState();

  casacore::Bool writeSysCal();

  casacore::Bool writeConfigDescription();

  // Scheme
  // 1) We regard one casacore::MS Observation as a set of ASDM ExecBlocks modelled on 
  //    a single ASDM Scheduling Block
  // 2) ALMA ExecBlocks are at most 30 minutes long.
  //    If an casacore::MS Observation is more than 30 Minutes long, it is split up into 
  //    several ASDM ExecBlocks each referring to the same Scheduling Block.
  // 3) Each ASDM ExecBlock contains one or more ASDM Scans based on the casacore::MS scans 
  // 4) Each ASDM Scan contains one or more ASDM Subscans
  // 5) Each ASDM Subscan is at most subscanduration long. (external parameter)
  // 6) If an casacore::MS Scan is longer than subscanduration, it is split up into 
  //    several ASDM subscans.

  casacore::Bool writeSBSummaryAndExecBlockStubs(); // "stubs" because these tables will be completed later
                                          //  with information from the APDM
  casacore::Bool writeMainAndScanAndSubScan(const casacore::String& datacolumn);

  // write the Main binary data for one DataDescId/FieldId pair and one SubScan
  // (return number of integrations written and set the last three parameters in the list)
  casacore::Int writeMainBinSubScanForOneDDIdFIdPair(const casacore::Int theDDId, const casacore::Int theFieldId, 
					   const casacore::String& datacolumn, 
					   const casacore::uInt theScan, const casacore::uInt theSubScan,
					   const casacore::uInt startRow, const casacore::uInt endRow,
					   const asdm::Tag eBlockId,
					   int& datasize, asdm::EntityRef& dataOid, 
					   std::vector< asdm::Tag >& stateId);

  casacore::Bool writePointingModel(); // write dummy pointing models

  casacore::Bool writePointing();
    
  // *** Aux. methods ***

  // check if vector corrT already contains a stokes type equivalent to st
  casacore::Bool stokesTypePresent( const casacore::Vector< casacore::Int > corrT, const casacore::Stokes::StokesTypes st );

  // *** Member variables ***

  // Initialized* by ctors.  (Maintain order both here and in ctors.)
  casacore::MeasurementSet ms_p; // the measurement set from which the ASDM is filled

  asdm::ASDM* ASDM_p; // the new ASDM

  string asdmVersion_p; // the version of the new ASDM

  casacore::uInt verbosity_p; // verbosity of the write methods

  casacore::String baseUid_p;  // the part of the UID which is common to all elements of the ASDM,
                    // i.e. typically "uid://archiveid/rangeid/"

  casacore::uInt runningId_p; // counter for the tables written; starts at 1!
                    // used to construct the UIDs: uid = baseUid_p + (runningId_p converted to unpadded hex string)

  casacore::String currentUid_p; // the last used uid

  casacore::String telName_p; // the name of the observatory from first row of casacore::MS observation table

  casacore::Double subscanDuration_p; // maximum duration of a subscan in seconds

  casacore::Double schedBlockDuration_p; // maximum duration of a scheduling or exec block in seconds

  casacore::Bool dataIsAPCorrected_p; // true if the data in the selected casacore::MS data column is 
                            // AtmPhaseCorrectionMod::AP_CORRECTED, false if it is
                            // AtmPhaseCorrectionMod::AP_UNCORRECTED

  string asdmUID_p; // ASDM UID == container ID of all tables

  casacore::String asdmDir_p; // ASDM output directory name

  // The default value for the Tag keys is Tag(), so the std::map works as is
  std::map <casacore::String, asdm::Tag> asdmStationId_p;  
  std::map <casacore::Int, asdm::Tag> asdmAntennaId_p;
  std::map <casacore::Int, asdm::Tag> asdmSpectralWindowId_p;
  std::map <casacore::Int, asdm::Tag> asdmPolarizationId_p;
  std::map <casacore::Int, asdm::Tag> asdmProcessorId_p;
  std::map <casacore::Int, asdm::Tag> asdmFieldId_p;
  std::map <casacore::Int, asdm::Tag> asdmEphemerisId_p;
  std::map <casacore::Int, asdm::Tag> asdmDataDescriptionId_p;
  std::map <casacore::Int, asdm::Tag> asdmStateId_p;
  std::map <casacore::uInt, asdm::Tag> asdmConfigDescriptionId_p; // maps from casacore::MS Main rows
  std::map <casacore::Int, asdm::Tag> asdmSBSummaryId_p; // maps from casacore::MS Observation Id + 10000*SpwId
  std::map <casacore::Double, asdm::Tag> asdmExecBlockId_p; // maps from casacore::MS Main timestamps 
  // the default value for the int values is -1, needs MapWithDefault
  MapWithDefault <casacore::Int, int> asdmFeedId_p; // ASDM feed id is not a Tag
  MapWithDefault <casacore::Int, int> asdmSourceId_p; // neither is the source id
  MapWithDefault <asdm::Tag, int> asdmPointingModelId_p; // maps ASDM Antenna Id to dummy pointing model

  std::vector< std::vector< casacore::Bool > > skipCorr_p; // skipCorr_p[j][PolId] indicates that correlation 
                                       // product j for POLARIZATION_ID PolId should not 
                                       // be written in the ASDM

};


} //# NAMESPACE CASA - END

#endif

