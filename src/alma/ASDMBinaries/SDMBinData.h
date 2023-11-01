#if     !defined(_SDMBINDATA_H)

#include <iostream>
#include <sstream>
#include <string>

#include <alma/ASDM/ASDMEntities.h>  
#include <alma/ASDM/Tag.h>
#include <alma/ASDM/ArrayTime.h>
#include <alma/ASDM/ArrayTimeInterval.h>

// Enumerations used to select data:
#include <alma/Enumtcl/AtmPhaseCorrection.h>
#include <alma/Enumtcl/CorrelationMode.h>
#include <alma/Enumtcl/ProcessorType.h>
#include <alma/Enumtcl/ScanIntent.h>
#include <alma/Enumtcl/SpectralResolutionType.h>
#include <alma/Enumtcl/TimeSampling.h>

#include <alma/Enumtcl/CorrelatorType.h>

// Views:
#include <alma/ASDMBinaries/SDMDataViews.h>

#include <alma/ASDMBinaries/BaselinesSet.h>
#include <alma/ASDMBinaries/Integration.h>


#include <alma/ASDMBinaries/SDMDataObjectReader.h>
#include <alma/ASDMBinaries/SDMDataObjectStreamReader.h>

// shared_ptr not needed by CASA, do not compile this for the WITHOUT_BOOST option
// It may be needed by ALMA code, so do not elimiate it yet.
#ifndef WITHOUT_BOOST
#include <boost/shared_ptr.hpp>
#endif

// #include "Singleton.hpp"
// #include "ExecBlockDir.hpp"

namespace sdmbin{

  /** Accessor to the SDM binary data */
class SDMBinData{
 public:
  /*   SDMBinData(); */
  /** Constructor:
      @pre The SDM dataset must be in memory, this dataset must have the binary data,
      files in a sub-directory named ASDMBinary. 
      @param datasetPtr Pointer to the SDM dataset
      @param execBlockDir SDM dataset directory (it must be readable as well as its 
                          sub-directory ASDMBinary.
      @post All the rows of the Main table will be accepted in the processing. 
      - Would that be inadequate use the methods 'select' to accept only a subset of the SDM
      main table rows. 
      - Use also, if desired, the method 'selectDataSubset' for further
      filtering at the level of the individual BLOBs.
      - After setting this filtering consider using the method 'isComplex()' before 
      scaning the SDM main table rows if the intent is to build a MeasurementSet 
      MAIN table.
  */ 
  SDMBinData( asdm::ASDM* const datasetPtr, std::string execBlockDir);

  ~SDMBinData();

  /** Method to select the main table rows to retrieve data for a subset of
      scan intents.
      @param es_si set of scan intents to be accepted
      @post All the rows in the Main table which do not satisfy this selection will be ignored.
      @note If the method isComplex() is used subsequently this selection can not be modified 
      after that.
  */
  void select( EnumSet<ScanIntentMod::ScanIntent> es_si);

  /** Method to select the main table rows to retrieve data for a subset of
      processor types.
      @param es_pt set of processor types to be accepted
      @post All the rows in the Main table which do not satisfy this selection will be ignored.
      @note If the method isComplex() is used subsequently this selection can not be modified 
      after that.
  */
  void select( EnumSet<ProcessorTypeMod::ProcessorType> es_pt);

  /** Method to select the main table rows to retrieve data for a subset of
      correlation modes.
      @param es_cm set of correlation modes to be accepted
      @post All the rows in the Main table which do not satisfy this selection will be ignored.
      @note If the method isComplex() is used subsequently this selection can not be modified 
      after that.
  */
  void select( EnumSet<CorrelationModeMod::CorrelationMode> es_cm);
  
  /** Method to select the main table rows to retrieve data for a subset of
      spectral resolution types.
      @param es_srt set of spectral resolution types to be accepted
      @post All the rows in the Main table which do not satisfy this selection will be ignored.
      @note If the method isComplex() is used subsequently this selection can not be modified 
      after that.
  */
  void select( EnumSet<SpectralResolutionTypeMod::SpectralResolutionType> es_srt);

  /** Method to select the main table rows to retrieve data for a subset of
      time samplings.
      @param es_ts set of time samplings to be accepted
      @post All the rows in the Main table which do not satisfy this selection will be ignored.
      @note If the method isComplex() is used subsequently this selection can not be modified 
      after that.
  */
  void select( EnumSet<TimeSamplingMod::TimeSampling> es_ts);


  /** An alternate method to select the main table rows to retrieve data for a subset of
      correlation modes, spectral resolution types and time sampling.
      @param es_cm set of correlation modes to be accepted
      @param es_srt set of spectral resolution types to be accepted
      @param es_ts set of time samplings to be accepted
      @post All the rows in the Main table which do not satisfy this selection will be ignored.
      @note If the method isComplex() is used subsequently this selection can not be modified 
      after that.
  */
  void select( EnumSet<CorrelationModeMod::CorrelationMode>        es_cm,
	       EnumSet<SpectralResolutionTypeMod::SpectralResolutionType> es_srt, 
	       EnumSet<TimeSamplingMod::TimeSampling>           es_ts);  


  /** Method to constrain the getData methods to return a (sub)set of the data which are in the BLOBs
      @param e_qcm   selected correlationMode enumerator;
      @param es_qapc set of AtmPhaseCorrection enumerators;
      @post The getData methods will return the data restricted to that selection. 
      For example if es_qapc is a set of 2 enumerators AP_CORRECTED and AP_UNCORRECTED
      then both type of data will be returned.
      @note Note that the MS filler accepts at most one AtmPhaseCorrection enumerator.
  */ 
  void selectDataSubset( Enum<CorrelationModeMod::CorrelationMode>       e_qcm,
			 EnumSet<AtmPhaseCorrectionMod::AtmPhaseCorrection> es_qapc );

  /** Method to set priority for DataDescription in the output order
      @post Output sequence forced to be DataDescriptionId/TimeInterval/Ant1/Ant2
      @note There is no unset method.
   */
  void setPriorityDataDescription();

  /** Accessor to the sequence of time centroids.
      @return An indexed sequence of time centroids
      @note The returned sequence has the required informations concerning transpositions to 
      minimize the number of measure conversions when computing the UVWs with the UvwCoords 
      state machine.
  */
  std::vector<std::pair<unsigned int,double> > timeSequence()const;

  /** Predicate to tell if the dataDescriptionId has priority over time in the out
      sequence of MSData objects.
  */
  bool dataDescriptionFirst()const;

  /** Apply system temperature calibration
      @pre  System temperature calibration not applied
      @post System temperature calibration will be applied
      @todo Handle the case when what is needed is missing in the syscal table
  */
  void applySysCal();

  /** Predicate to know if the system temperature calibration is applied
      when accessing data
  */
  bool sysCalApplied() const;

  /**
   * A utility method which returns the type of the processor which produced the data referred to by a row of the Main table.
   *
   * @param mainRowPtr a pointer to a row of the Main table of an ASDM dataset.
   * @return a ProcessorType value.
   */
  ProcessorTypeMod::ProcessorType processorType(asdm::MainRow* const mainRowPtr) const; 

  /** Accessor to the SDM Main table rows
      @param mainRowPtr Pointer to a row of the Main table
      @return true is the SDM row passed the selection criterion, else false
      @post If true is returned the state is adeqaute to respond to a 'getData' 
      method, else the state is adequate to respond to a new call of this
      acceptMainRow for an other SDM Main table row.
  */
  bool acceptMainRow( asdm::MainRow* const mainRowPtr);

  /** Accessor to one SDM Main table row.
   * @param mainrowPtr is a pointer to one row of the SDM Main table,
   * @return true if the SDM row passed the selection criterium , else false,
   * @post a true value returned means that the instance of SDMBinData is in 
   * a state such a call to the getData method can be done, else only another call to openRow (supposedly for another Main row) can be done.
   */
  bool openMainRow(asdm::MainRow* const mainRowPtr);

  /** Filter: rejection of a Main table row if it does not satisfy the criterion
      of selection. 
      @param mainRowPtr Pointer to a row of the Main table
      @return the reason if this row has been rejected, else an empty string
  */ 
  std::string reasonToReject(asdm::MainRow* const mainRowPtr);


  /** Method to know if the data to be retrieved will be of type complex or not.

      @pre 
      - case a: used in the context of the SDM main table as a whole
      - case b: used in the context of a given row
      @return
      - case a: true if there is in the table at least one row with references 
                to data with at least 3 polarization cross-products or if there 
                are data of type visibilities, else it returns false
      - case b: true if for the current row there are references to data with at 
                least 3 polarization cross-products or if there are data of type 
                visibilities, else it returns false
      @post data to be retrieve for the current row will be of type complex if this
            method returns true, else false.
  */
  bool isComplexData();

  std::vector<SDMData*> getData();

  /**
     @todo 
     - apply the polynomials for the field directions
     - apply syscal for the cross data
   */
  std::vector<MSData*>  getData( Enum<CorrelationModeMod::CorrelationMode> e_qcm, EnumSet<AtmPhaseCorrectionMod::AtmPhaseCorrection> es_qapc); 

  MSData*          getData( unsigned int na, unsigned int nfe, unsigned int ndd, unsigned int nbin) throw (Error); 

  MSData*          getCalibratedData( unsigned int na, unsigned int nfe, unsigned int ndd, unsigned int nbin,
				      std::pair<bool,std::vector<std::vector<float> > > p_tsys) throw (Error); 

  MSData*          getData( unsigned int na1, unsigned int nfe1, unsigned int na2, unsigned int nfe2, 
			    unsigned int ndd, unsigned int nbin, std::vector<unsigned int> v_napc,
			    float scleFactor);

  MSData*          getCalibratedData( unsigned int na1, unsigned int nfe1, unsigned int na2, unsigned int nfe2, 
				      unsigned int ndd, unsigned int nbin, std::vector<unsigned int> v_napc,
				      float scleFactor,
				      std::pair<bool,std::vector<std::vector<float> > > p_tsys);

  MSData*          getData( asdm::Tag antId, int feedId,
			    asdm::Tag dataDescId, 
			    AtmPhaseCorrectionMod::AtmPhaseCorrection apc, 
			    unsigned int  binNum); 

  MSData*          getData( asdm::Tag antId1, int feedId1,
			    asdm::Tag antId2, int feedId2,
			    asdm::Tag dataDescId, 
			    std::vector<AtmPhaseCorrectionMod::AtmPhaseCorrection> v_apc, 
			    unsigned int  binNum); 

  const VMSData* getDataCols();

  const VMSData* getDataCols( Enum<CorrelationModeMod::CorrelationMode> e_qcm, EnumSet<AtmPhaseCorrectionMod::AtmPhaseCorrection> es_qapc );

  /**
   * Returns a pointer to a VMSData structure containing the values require to populate the MS Main table columns
   * from what it has got from the reading of at most n SDMDataSubsets in the current BDF file. 
   * <ul>
   * <li> This method call be called iteratively as long as there are SDMDataSubsets.  </li>
   * <li> A returned VMSData having any of its array empty means that all the SDMDataSubsets have been read.</li>
   * </ul>
   * @param n gives the maximum number of SDMDataSubsets ([sub]integrations) that one wants to read in the current BDF.
   * @return a pointer to a VMSData containing the values to populate MS rows.
   */
  const VMSData* getNextMSMainCols(unsigned int n);
  const VMSData* getNextMSMainCols(Enum<CorrelationModeMod::CorrelationMode> e_qcm, EnumSet<AtmPhaseCorrectionMod::AtmPhaseCorrection> es_qapc, unsigned int n);

  // shared_ptr not needed by CASA, do not compile this for the WITHOUT_BOOST option
  // It may be needed by ALMA code, so do not elimiate it yet.
#ifndef WITHOUT_BOOST
  void getNextMSMainCols(unsigned int n, boost::shared_ptr<VMSDataWithSharedPtr>& vmsData_p_sp);
  void getNextMSMainCols(Enum<CorrelationModeMod::CorrelationMode> e_qcm, EnumSet<AtmPhaseCorrectionMod::AtmPhaseCorrection> es_qapc, unsigned int n, boost::shared_ptr<VMSDataWithSharedPtr>& vmsData_p_sp);
#endif

  std::vector<MSData*> getMSDataFromBDFData(Enum<CorrelationModeMod::CorrelationMode> e_qcm, EnumSet<AtmPhaseCorrectionMod::AtmPhaseCorrection> es_qapc, unsigned int n);

  /**
   * Populates the vector v_dataDump after having read at most n DataSubsets in the BDF.
   */
  void getNextDataDumps(unsigned int n);

  /** To know easily in which order the baselines are returned in a sequence of MSData objects or in a VMSData object.
      @note Let assume the sequence of antennas {1,2,3,4} in the ConfigDescription table. There are two cases: 
      - Return true if the resulting order is (1,2) (1,3) (2,3) (1,4) (2,4) and (3,4)
      - Return false if the resulting order is (1,2) (1,3) (1,4) (2,3) (2,4) and (3,4)

      Note that this order may be different compared to the order used in the BDF.  
  */
  static bool baselineReverse();

  /** To know easily the position of the auto data and metadata relative to the cross data and metadata in the
      sequence of MSData objects or in a VMSDATA object

      Note that this order may be different compared to the order used in the BDF.  
  */
  static bool autoTrailing();

  /** The two parameters defining the order in the output sequences, baslineReverse and autoTrailing */
  static std::pair<bool,bool> dataOrder();

 protected:

  /** Do the actual binary data binding
      @param dataOID The data object identifier
   */
  int  attachDataObject(std::string dataOID);

  /** Do the actual binary data binding.
      The BDF is traversed as a stream.
      @param dataOID The data object identifier
   */
  int  attachStreamDataObject(const std::string& dataOID);

  /** Initialize the sequential reading of the SDMDataSubsets (i.e. [sub]integrations) contained in the BDF referred 
   * to by dataOID.
   * <ul>
   * <li>The BDF file referred to by dataOID is opened and the informations global to the whole BDF are read </li>
   * <li>The sequential reading of the SDMDataSubsets can be started. </li>
   * </ul>
   *
   * @param dataOID the BDF identifier.
   * @return an int value. 0 means a problem, 1 means Ok.
   */
  int openStreamDataObject(const std::string& dataOID);

  /** Unset the current data binding
   */
  void detachDataObject();

  /** Determine the view of a MS STATE
    @param subscanNum Subscan number (the semantic of this attribute is assumed to be identical
                      in the SDM and the MS
    @param v_stateId  The list of states identifiers in the row of SDM Main table
    @param v_antennaId The corresponding list of antenna identifier
    @param v_feedId The corresponding list of collection of feed identifier
    @param v_ddId The sequence of data description identifiers for the current config description
    @param na Antenna index for a given antenna in this antenna list
    @param nfe Feed index
    @param nspw DataDescription index
    @param timeOfDump The epoch to be considered to determine this MS STATE view
  */
  MSState          getMSState( unsigned int subscanNum,
			       std::vector<asdm::Tag>  v_stateId, 
			       std::vector<asdm::Tag>  v_antennaId, std::vector<int> v_feedId, std::vector<asdm::Tag> v_ddId,
			       unsigned int na, unsigned int nfe, unsigned int nspw, asdm::ArrayTime timeOfDump);

 private:
  static asdm::ASDM*                     datasetPtr_;
  static std::string                  execBlockDir_;
  static bool                       canSelect_;
  static bool                    forceComplex_;  // true ==> autoData will be transformed into complex data
  EnumSet<ScanIntentMod::ScanIntent>                   es_si_;  // set of scan intents selected
  EnumSet<ProcessorTypeMod::ProcessorType>                es_pt_;  // set of processor type selected
  EnumSet<CorrelationModeMod::CorrelationMode>              es_cm_;  // set of correlation mode selected
  EnumSet<SpectralResolutionTypeMod::SpectralResolutionType>      es_srt_;  // set of spectral resolution types selected
  EnumSet<TimeSamplingMod::TimeSampling>                 es_ts_;  // set of time sampling selected

  Enum<CorrelationModeMod::CorrelationMode>                 e_qcm_;  // query to select a subset of data in a BLOB
  EnumSet<AtmPhaseCorrectionMod::AtmPhaseCorrection>         es_qapc_;  // query to select a subset of data in a BLOB

  bool                                ddfirst_;  // true ==> output sequences of time for every dataDescription

  asdm::MainRow*                         mainRowPtr_;
  std::string                              dataOID_;
  asdmbinaries::SDMDataObjectReader                  blob_r_;  // current read-only BLOB
  asdmbinaries::SDMDataObjectStreamReader           sdmdosr ;  // current SDMDataObjectStreamReader
  bool                         bdfMemoryMapped;  // will the BDF mapped in memory (true) or read sequentially (false) ?
  std::vector<DataDump*>                v_dataDump_;

  const  float*                  floatDataPtr_;  // mutable attribute; autocorrelation data of a single dump
  const  short*                  shortDataPtr_;  // mutable attribute; visiblity data of a single dump
  const  int*                     longDataPtr_;  // mutable attribute; visiblity data of a single dump
  static MSData*                    msDataPtr_;  // mutable attribute; one MS-MAIN row given
  static SDMData*                  sdmDataPtr_;  // mutable attribute; one SDM-Main (v2) row 
  static BaselinesSet*           baselinesSet_;  // mutable attribute
  static std::vector<MSData*>          v_msDataPtr_;  // mutable attribute
  static VMSData*                  vmsDataPtr_;
  static std::vector<SDMData*>        v_sdmDataPtr_;
  std::map<asdm::Tag,BaselinesSet*>  m_cdId_baselinesSet_;
  std::set<asdm::Tag>                             s_cdId_; // the keys present in  m_cdId_baselinesSet_ (used for optimization)
  bool                            complexData_;
  static bool                  coutDeleteInfo_; // utility for debugging
  static bool                 baselineReverse_; // order in which the data are in the returning VMSData structure.
  static bool                    autoTrailing_; // position of the auto data relative cross data in the returning VMSData structure.
  static bool                          syscal_; // true ==> apply system temperature calibration.

  const float*               floatDataDumpPtr_;

  std::vector<std::pair<unsigned int,double> >    v_tci_; // Indexed time centroid sequence

  void   deleteMsData(MSData* msDataPtr);

  bool verbose_ ;  // A variable to turn on/off the verbosity of the code, traditionnally based on the existence of env var "ASDM_DEBUG"
};

}

#define _SDMBINDATA_H
#endif
