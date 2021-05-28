#if     !defined(_SDMDATAVIEWS_H)

#include <alma/ASDM/Angle.h>
#include <alma/ASDM/ArrayTime.h>
#include <alma/ASDM/ArrayTimeInterval.h>
#include <alma/Enumerations/CAtmPhaseCorrection.h>

#ifndef WITHOUT_BOOST
#include <boost/shared_array.hpp>
#endif

namespace sdmbin{
   
  /**
   * A structure containing the data from a single SDM BLOB (origin: version for TP FBT, FV 12 Avril 2007)
   * with the view of one row in the SDM Main table
   */
  typedef struct {
    public:

    asdm::ArrayTime                       time;             //!< Epoch of obs requested epoch when obs. ordered

/*     int                             scanNum; */
/*     int                             subscanNum; */
    int                             integrationNum;
    int                             subintegrationNum;

    asdm::Interval                        interval;         //!< Requested exposure time when observations ordered

    std::vector<asdm::ArrayTime>               v_timeCentroid;   //!< Actual epoch for the observations; size=numAnt
    std::vector<asdm::Interval>                v_exposure;       //!< Actual duration for the observations; size=numAnt

    std::string                          axisStructure;    //!< Axis structure for the data
    std::vector<std::vector<unsigned int> >   vv_sizes;         //!< axis sizes of the multi-dimensional data leaves; size=numDataDesc (dim of leaf/spw)
    std::vector<std::vector<float*> >         vv_floatData;     //!< Actual data; size=numAnt (numBaseband per ant)

    std::vector<int>                     v_flagAnt;        //!< Antenna-based flags; size=numAnt
    std::vector<std::vector<int> >            vv_flagPol;       //!< Polarization-based flags; size=numAnt  (numBaseband per ant)
    std::vector<std::vector<std::vector<int> > >   vvv_flagBaseband; //!< Baseband-based flags; size=numAnt (numBaseband per ant and numpol per baseband)

  } SDMData;

  /** A structure containing state information following the MSv2.0 data model
   */
  typedef struct {
    public:
    bool         sig;         //!< True if the source signal is being observed
    bool         ref;         //!< True for a reference phase
    double       cal;         //!< Noise calibration temperature (zero if not added)
    double       load;        //!< Load temperature (zero if no load)
    unsigned int subscanNum;  //!< Subscan number
    std::string       obsMode;     //!< Observing mode defined by a set of reserved keywords characterizing the current observing mode. e.g. OFF_SPECTRUM. Used to define the schedule strategy
  } MSState;

  /**
   * A structure containing a block of data for a single MS Main table row.
   */
  typedef struct {
    public:   
    int                         processorId;          //!< processor identifier
    int                         antennaId1;           //!< identifier for antenna 1
    int                         antennaId2;           //!< identifier for antenna 2 (same as antennaId1 in case of auto-correlations)
    int                         feedId1;              //!< feed identifier used with antenna 1
    int                         feedId2;              //!< feed identifier used with antenna 2
    int                         dataDescId;           //!< dataDescription identifier
    double                      time;                 //!< time (MJD) (requested epoch for the observations)
    int                         fieldId;              //!< field identifier
    double                      interval;             //!< requested duration (sec) for the observations       
    double                      timeCentroid;         //!< actual epoch for the observations
    double                      exposure;             //!< actual duration for the observations
    std::vector<unsigned int>        projectPath;          //!< from BINARIES the relative project-path URI these data belongs to.
    std::vector<AtmPhaseCorrectionMod::AtmPhaseCorrection>  v_atmPhaseCorrection; //!< code: 0=uncorrected 1=corrected 2=uncorr. and corrected data
    int                         binNum;               //!< bin number in case of switch cyle obs. mode (1-based) 
    int                         numData;              //!< number of data (visibility or autocorrelations): nbin?*nbl*SUM(nsp*npp)
    std::vector<unsigned int>        v_dataShape;          //!< numPol, numChan and numApc=1 sizes
    std::vector<float*>              v_data;               //!< float complex data (size=numApc) or float data (size=1, not atm phase corrected)
    float*                      cData;                //!< actual corrected float complex data or NULL
    float*                      mData;                //!< actual mixed float complex data or NULL
    std::vector<std::vector<asdm::Angle> >      phaseDir;             //!< the reference phase direction to be used to determine the UVW
    int                         stateId;              //!< State identifier
    MSState                     msState;              //!< State information using the MS view
    unsigned int                flag;                 //!< flag
  } MSData;
  

  /**
   * A structure containing the data from a single SDM BLOB.
   * \note 
   * - The query (param correlMode, binNum and atmospherePhaseCode) may have filtered out some data!
   * - The typical size of the  member vectors is nt*ndd*(na*(na+1))/2 where ndd is the number of data-descriptions,
   * na the number of antennas and nt is the number of data dumps found in the BLOB.
   * - The implementation does not support multiple configDescId within a BLOB
   * - In general interval has the same value for every dump; time and interval are derived from the BLOB
   */
  typedef struct {
    public:

    int                             processorId;          //!< from SDM      identifier of the processor used for these data
    std::vector<double>                  v_time;               //!< from BINARIES Epoch of obs requested epoch when obs. ordered
    std::vector<int>                     v_fieldId;            //!< from SDM      Identifier of the field every of these data dumps

    std::vector<double>                  v_interval;           //!< from BINARIES Requested exposure time when observations ordered
    std::vector<AtmPhaseCorrectionMod::AtmPhaseCorrection>      v_atmPhaseCorrection; //!< from QUERY    data filtering
    int                             binNum;               //!< from QUERY    data filtering (a MS row cannot have data for more than one step)

    std::vector<unsigned int>            v_projectPath;        //!< from BINARIES the rel. project-path URI for evry fragment of data it belongs to 
    std::vector<int>                     v_antennaId1;         //!< from SDM      antenna 1 identifiers 
    std::vector<int>                     v_antennaId2;         //!< from SDM      antenna 2 identifiers (=v_antennaId1 if auto-correlations)
    std::vector<int>                     v_feedId1;            //!< from SDM      feed identifiers used with antenna 1
    std::vector<int>                     v_feedId2;            //!< from SDM      feed identifiers used with antenna 1
    std::vector<int>                     v_dataDescId;         //!< from SDM      auto-find for the auto-correlation if correlMode=2
    std::vector<double>                  v_timeCentroid;       //!< from BINARIES actual epoch for the observations 
    std::vector<double>                  v_exposure;           //!< from BINARIES actual duration for the observations
    std::vector<int>                     v_numData;            //!< from SDM      number of auto-correlations or cross-correlation
    std::vector<std::vector<unsigned int> >   vv_dataShape;         //!< from SDM      numPol,numChan,numApc=1
    std::vector<std::map<AtmPhaseCorrectionMod::AtmPhaseCorrection,float*> > v_m_data;     //!< from BINARIES vector of maps (size num MS rows), map size=v_atmPhaseCorrection.size()
    std::vector<std::vector<std::vector<asdm::Angle> > > v_phaseDir;           //!< from SDM      the ref phase directions at the epochs (v_timeCentroid) to use for uvw 
    std::vector<int>                     v_stateId;            //!< from SDM      need to be redefined when numBin>1 & checked when baseline>0
    std::vector<MSState>                 v_msState;            //!< from SDM      MS state tuples
    std::vector<unsigned int>            v_flag;               //!< from BINARIES  
  } VMSData;
  

  // This struct, using boost::shared_array, is not used by CASA. It is not available when WITHOUT_BOOST is set.
  // It may be used by ALMA code and so has not been eliminated. 
#ifndef WITHOUT_BOOST
  /**
   * A structure containing the data from a single SDM BLOB.
   * The *big* difference with the previous definition (VMSData) is that the visibilities are referred to by a boost::shared_array instead
   * of a plain old pointer.
   *
   * \note 
   * - The query (param correlMode, binNum and atmospherePhaseCode) may have filtered out some data!
   * - The typical size of the  member vectors is nt*ndd*(na*(na+1))/2 where ndd is the number of data-descriptions,
   * na the number of antennas and nt is the number of data dumps found in the BLOB.
   * - The implementation does not support multiple configDescId within a BLOB
   * - In general interval has the same value for every dump; time and interval are derived from the BLOB
   */
  typedef struct {
    public:

    int                             processorId;          //!< from SDM      identifier of the processor used for these data
    std::vector<double>                  v_time;               //!< from BINARIES Epoch of obs requested epoch when obs. ordered
    std::vector<int>                     v_fieldId;            //!< from SDM      Identifier of the field every of these data dumps

    std::vector<double>                  v_interval;           //!< from BINARIES Requested exposure time when observations ordered
    std::vector<AtmPhaseCorrectionMod::AtmPhaseCorrection>      v_atmPhaseCorrection; //!< from QUERY    data filtering
    int                             binNum;               //!< from QUERY    data filtering (a MS row cannot have data for more than one step)

    std::vector<unsigned int>            v_projectPath;        //!< from BINARIES the rel. project-path URI for evry fragment of data it belongs to 
    std::vector<int>                     v_antennaId1;         //!< from SDM      antenna 1 identifiers 
    std::vector<int>                     v_antennaId2;         //!< from SDM      antenna 2 identifiers (=v_antennaId1 if auto-correlations)
    std::vector<int>                     v_feedId1;            //!< from SDM      feed identifiers used with antenna 1
    std::vector<int>                     v_feedId2;            //!< from SDM      feed identifiers used with antenna 1
    std::vector<int>                     v_dataDescId;         //!< from SDM      auto-find for the auto-correlation if correlMode=2
    std::vector<double>                  v_timeCentroid;       //!< from BINARIES actual epoch for the observations 
    std::vector<double>                  v_exposure;           //!< from BINARIES actual duration for the observations
    std::vector<int>                     v_numData;            //!< from SDM      number of auto-correlations or cross-correlation
    std::vector<std::vector<unsigned int> >   vv_dataShape;         //!< from SDM      numPol,numChan,numApc=1
    std::vector<std::map<AtmPhaseCorrectionMod::AtmPhaseCorrection,boost::shared_array<float> > > v_m_data;     //!< from BINARIES vector of maps (size num MS rows), map size=v_atmPhaseCorrection.size()
    std::vector<std::vector<std::vector<asdm::Angle> > > v_phaseDir;           //!< from SDM      the ref phase directions at the epochs (v_timeCentroid) to use for uvw 
    std::vector<int>                     v_stateId;            //!< from SDM      need to be redefined when numBin>1 & checked when baseline>0
    std::vector<MSState>                 v_msState;            //!< from SDM      MS state tuples
    std::vector<unsigned int>            v_flag;               //!< from BINARIES  
  } VMSDataWithSharedPtr;
#endif
}

#define _SDMDATAVIEWS_H
#endif
