#if     !defined(_DATADESCRIPTIONSSET_H) 

#include <vector>
#include <set>
#include <map>

#include <alma/Enumtcl/AtmPhaseCorrection.h>
#include <alma/Enumtcl/CorrelationMode.h>
#include <alma/Enumtcl/SidebandProcessingMode.h>

#include <alma/ASDM/ASDMEntities.h>
#include <alma/ASDMBinaries/SwitchCyclesList.h>
//#include <alma/ASDM/DataBlock.h>

namespace sdmbin {

  struct DataDescParams {
    unsigned int ddIdx;
    asdm::Tag    scId;
    asdm::Tag    spwId;
    asdm::Tag    polId;
    unsigned int numChan;
    unsigned int numCorr;
    unsigned int numBin;
  };


  /** Definition of the first level in the tree hierarchy: set of spectro-polarization descriptions, 
   *  basebands. 
   */
  class DataDescriptionsSet : public SwitchCyclesList
  {
  public:

    DataDescriptionsSet();

    /** Define the first level in the tree hierarchy.
     * @pre the pointer to the SDM dataset is already known
     * @param  v_switchCycleId a sequence of switchCycle identifiers, one per data description. It is not a set
     * @param  v_dataDescriptionId the sequence of DataDescription identifiers. it is an ordered set
     * @param  corrMode the correlation mode used:
     * - CROSS_ONLY if, in the data products, there are only visibilities (i.e. no auto-correlation data)
     * - AUTO-ONLY  if, in the data products, there are only auto-correlations (i.e. no visibilities data)
     * - CROSS_AND_AUTO if , in the data products, there are both visibilities and auto-correlations
     * @param atmPhaseCodes the vector of atmospheric phase correction enumerators used:
     * - AP_UNCORRECTED 
     * - AP_CORRECTED
     * - AP_MIXED
     * @post the first level of the tree hierarchy being defined 
     * this class can be derived to set the second level (class BaselinesSet).
     * @note Use the alternate constructor 
     * DataDescriptionsSet( SwitchCyclesList& , vector<int> , int ,  int , int);
     * if the data description identifiers are typed int. 
     * @note Use the alternate constructor if the data description identifiers are 
     *       typed int.
     */
    DataDescriptionsSet( asdm::ASDM* const datasetPtr, std::vector<asdm::Tag> v_switchCycleId,
			 std::vector<asdm::Tag> v_dataDescriptionId, 
			 CorrelationModeMod::CorrelationMode corrMode,  std::vector<AtmPhaseCorrectionMod::AtmPhaseCorrection> atmPhaseCodes);

    /** Define the first level in the tree hierarchy.
     * @pre the pointer to the SDM dataset is already known
     * @param  datasetPtr the SDM dataset
     * @param  switchCyclesList a switchCycle object
     * @param  v_dataDescriptionIdArray the set of DataDescription identifiers
     * @param  corrMode the correlation mode used:
     * - CROSS_ONLY
     * - AUTO-ONLY
     * - CROSS_AND_AUTO
     * @param atmPhaseCodes the vector of atmospheric phase correction enumerators used:
     * - AP_UNCORRECTED 
     * - AP_CORRECTED
     * - AP_MIXED
     * @param  bitSize (MAY BECOME OBSOLETE in the future since bitSize is now imposed to be the same for all spectral windows)
     * @post the first level of the tree hierarchy is now defined
     * @note Use the alternate constructor 
     * DataDescriptionsSet( SwitchCyclesList& , vector<Tag> , int ,  int , int);
     * if the data description identifiers are with the asdm type Tag.
     */
    DataDescriptionsSet( std::vector<int> v_switchCycleId,
			 std::vector<int> v_dataDescriptionId,
			 CorrelationModeMod::CorrelationMode corrMode,  std::vector<AtmPhaseCorrectionMod::AtmPhaseCorrection> atmPhaseCodes);

    /** Copy constructor */
    DataDescriptionsSet(const DataDescriptionsSet &);

    /** Destructor 
     * @note use the default implementation 
     */
    virtual ~DataDescriptionsSet();

    /** Get the baseband name of a given dataDescription
     * @param ndd The index (zero-based) of the dataDescription identifier
     * @exception ndd has an illegal value or is too large for the number of dataDescriptions in the ConfigDescription 
     * @return The baseband number (one-based)
     */ 
    BasebandNameMod::BasebandName      getBasebandName(unsigned int ndd) ;

    /** Get the baseband number of a given dataDescription
     * @param ndd The index of the dataDescription identifier
     * @exception ndd has an illegal value or is too large for the number of dataDescriptions in the ConfigDescription 
     * @return The baseband number (one-based)
     */ 
    unsigned int      getBasebandIndex(unsigned int ndd) ;
				      
    /** Accessor to the number of basebands
     * @return the number of basebands \f$ N_{bb} \f$
     */
    unsigned int      numBaseband();

    /** Accessor to the number of steps for the baseband of a given dataDescription
     * @param ndd The index of the dataDescrition identifier in dataDescriptionIdArray
     *            (2nd param. in the constructors).
     * @exception ndd is too large for the number of dataDescriptions in the ConfigDescription 
     * @return Number of steps for the baseband of a given dataDescription
     */
    unsigned int      numBin(unsigned int ndd) ;

    /** Accessor to the number of polarization products of a given dataDescription
     * @param ndd The index of the dataDescrition identifier in dataDescriptionIdArray
     *            (2nd param. in the constructors).
     * @exception ndd is too large for the number of dataDescriptions in the ConfigDescription 
     * @return Number of polarization products depending on the correlation mode:
     * - if CROSS_ONLY     this is \f$ N_{pp}^{oo} \f$, the number of polarization cross products 
     *                    for the non-zero baselines.      
     * - if AUTO_ONLY      this number is \f$ N_{pp}^{o} \f$ for the case of "single-dish" data.
     * - if CROSS_AND_AUTO this is \f$ N_{pp}^{oo} \f$, the number of polarization cross products 
     *                    for the non-zero baselines;\n use the method numSdPol(int) to get the 
     *                    corresponding number \f$ N_{pp}^{o} \f$ for the zero baselines.
     * @note If it desired to get this number for a given baseband, use the method getNumPol(int bbNum).
     */
    unsigned int      numPol(unsigned int ndd) ;

    /** Accessor to the number of polarization cross-products of a given dataDescription
     * @param ndd The index (zero-based) of the dataDescription identifier in dataDescriptionIdArray
     *            (2nd param. in the constructors).
     * @exception ndd has an illegal value or is too large for the number of dataDescriptions in the ConfigDescription 
     * @return Number of products for the zero-baselines.\n
     *         If correlationMode=CROSS_AND_AUTO, use the accessor numPol(unsigned int) to get the corresponding number
     *          \f$ N_{pp}^{oo} \f$ of cross products for the non-zero baselines.
     * @note If it desired to get this number \f$ N_{pp}^{oo} \f$ for a given baseband, use the method 
     *       getNumSdPol(int bbNum).
     */
    unsigned int      numSdPol(unsigned int ndd) ;

    /** Accessor to the number of frequency channels of a given dataDescription
     * @param ndd The index of the dataDescription identifier in dataDescriptionIdArray
     *            (2nd param. in the constructors).
     * @exception ndd is too large for the number of dataDescriptions in the ConfigDescription 
     * @return Number of frequency channels (spectral points) for that dataDescription
     */
    unsigned int      numChan(unsigned int ndd) ;

    /** Accessor to get the spwtral window identifier of a given dataDescription
     * @param ndd The index of the dataDescription identifier in dataDescriptionIdArray
     * @exception ndd is too large for the number of dataDescriptions in the ConfigDescription 
     * @return The spectral window identifier
     */
    asdm::Tag               getSpwId(unsigned int ndd) ;

    /** Accessor to the total frequency bandwidth of a given dataDescription
     * @param ndd The index (zero-based) of the dataDescription identifier  dataDescriptionIdArray
     *            (2nd param. in the constructors).
     * @exception ndd has an illegal value or is too large for the number of dataDescriptions in the ConfigDescription 
     * @return Total spectral bandwidth (Hz)
     */
    asdm::Frequency         totBandwidth(unsigned int ndd) ;

    /** Size of the apc axis (can be only 1 or 2)
     * @return size of this axis (the 3rd inner most in the axis hierarchy)
     * @note 
     * - atmospherePhaseCode=0 or 1 this size will be 1
     * - if atmospherePhaseCode=2 this size is 2
     * TODO tell in the SDM document that along this axis the first grid coordinate is for the uncorreceted data and the second
     *       for the corrected data. 
     */
    unsigned int      numApc();

    /** Accessor to the atmospherePhaseCorrection
     *  @param atmPhaseCorrectionIndex index (0-based) along the apc axis
     *  @return the atmPhaseCorrection enumerator for that index
     * @note
     *  return an empty enumeration if the index is outside the range along the apc axis
     * - AP_UNCORRECTED means the data product contain uncorrected data
     * - AP_CORRECTED   means the data product contain corrected data
     * - AP_MIXED       means in the data product contains some data having been corrected, the other not corrected, 
     *                  the criterion for correcting or not being being based to some algorithm
     */                     
      Enum<AtmPhaseCorrectionMod::AtmPhaseCorrection> atmPhaseCorrection(unsigned int atmPhaseCorrectionIndex);

     /** Get the index position along the apc axis
      * @param apc an AtmPhaseCorrection enumerator
      * @return  The index position (0-based) along the apc axis
      * @exception -1 returned if the input apc value is not present on the apc axis
      */ 
     unsigned int atmPhaseCorrectionIndex(AtmPhaseCorrectionMod::AtmPhaseCorrection apc) ;

    /** Provide the number of auto-correlations for a given dataDescription
     * \note would the dataDescription identifier for this index in ConfigDescription be for cross correlations, 
     * the corresponding dataDescription for auto-correlations would actually be used
     * @param ndd The index (zero-based) of the dataDescription identifier in dataDescriptionIdArray
     *            (2nd param. in the constructors).
     * @exception ndd has a value which is illegal or too large for the number of dataDescription used in the configuration
     * @return  Number of auto-correlations for that dataDescription
     * @exception 0 returned (lax mode) if correlationMode=0.
     */
    int               numAutoData(unsigned int ndd) ;

    /** Provide the number of cross-correlations for a given dataDescription
     * @param ndd The index (zero-based) of the dataDescription identifier in dataDescriptionIdArray
     * @exception ndd has an illegal value or is too large for the number of dataDescriptions in the ConfigDescription 
     * @return  the number of cross-correlations for that dataDescription
     * @exception returns 0 if correlationMode=1.
     */
    unsigned int      numCrossData(unsigned int ndd) ;

    /** Provide the number of switchCycle phases cumulated over all the basebands in the configuration.
     * @return the sum \f$ \sum_{i=1}^{N_{bb}} N_{bin}(i)  \f$
     * @note In the software of the ALMA correlator subsystem the following constraint has been
     * set: the number of phases in the switching cycles (e.g. when using the frequency switch observing
     * mode) must be common to all dataDescriptions within a baseband but may be different from
     * baseband to baseband.
     */ 
     unsigned int     sumMetaDataIndex();

    /** Provide the index for a given dataDescription in the meta-data tree   
     * @param ndd The index (zero-based) of the dataDescription identifier in dataDescriptionIdArray,
     * the second parameter in the signature of the constructors.
     * @exception ndd has an illegal value or is too large for the number of dataDescriptions in the ConfigDescription 
     * @return  The index in the metadata tree
     */
     unsigned int      metaDataIndex(unsigned int ndd)  ;

    /** Get the bitSize (actually the number of bytes, 2 or 4) used for the visibilities
     * @return The size (actually in number of bytes)
     * @note Since December 2005 only the cross data (the visibilities) may be represented
     * either with 2 bytes or 4 bytes words. The auto data are always represented using
     * 4 bytes. Furthermore the same representation must be used whatever the spectral
     * resolution or the baselines. Hence this getBitSize method no longer requires
     * input parameters.\n
     * Note that a visibility being a complex quantity it is represented using two words,
     * the first for the real part and the second for the imaginary part. Note also
     * that for single-dish (zero-baseline), the XY correlation product is also a complex
     * quantity while XX and YY are real quantities, these being represented using a 
     * single word.
     */
/*     int               getBitSize(); */

    /** Get the correlation mode used
     * @return The correlation mode:
     * - CROSS_ONLY
     * - AUTO_ONLY
     * - CROSS_AND_AUTO
     */ 
    CorrelationModeMod::CorrelationMode   getCorrelationMode();

    /** Get the number of dataDescription
     * @return  The number of dataDescription
     */    
    unsigned int      getNumDataDescription();

    /** Get the number of frequency channels for a given dataDescription
     * @param ndd The dataDescription number
     * @exception ndd equal or exeeding the number of dataDescription in the configuration setup
     * @return  The number of channels (spectral points) for that dataDescription number (1-based)
     */   
    unsigned int      getNumChan(unsigned int ndd)  ;

    /** Get the number of Polarization Products given a baseband.
     * @param  nbb The baseband index
     * @exception bbIndex is equal or exceeds number of basebands in the configuration or not baseband independent
     * @return The number of Polarization Products:
     * - if correlationMode=CROSS_ONLY number     \f$ N_{pp}^{oo} \f$ of products for the non-zero baselines
     * - if correlationMode=AUTO_ONLY number      \f$ N_{pp}^{o}  \f$ of products for the zero-baselines
     * - if correlationMode=CROSS_AND_AUTO number \f$ N_{pp}^{oo} \f$ of products for the non-zero baselines;
     *   \n 
     *   use the method getNumSdPol() to get \f$ N_{pp}^{o} \f$, the number of products for 
     *   the zero-baselines.
     * @note
     * To get number of products given a dataDescription use the method numPol().
     * To get number of products given a basebandName, would that be constant per baseband, use getNumPol(BasebandName bbName)
     */
    unsigned int       getNumPol(unsigned int bbIndex)  ;

    /** Get the number of Polarization Products given a baseband.
     * @param  bbName The baseband index (0-based)
     * @return The number of Polarization Products:
     * - if  CROSS_ONLY     number \f$ N_{pp}^{oo} \f$ of products for the non-zero baselines
     * - if  AUTO_ONLY      number \f$ N_{pp}^{o} \f$ of products for the zero-baselines
     * - if  CROSS_AND_AUTO number \f$ N_{pp}^{oo} \f$ of products for the non-zero baselines;
     *   \n 
     *   use the method getNumSdPol() to get \f$ N_{pp}^{o} \f$, the number of products for 
     *   the zero-baselines.
     * @note
     * To get number of products given a dataDescription use the method numPol().
     */
    unsigned int      getNumPol(BasebandNameMod::BasebandName bbName)  ;

    /** Get the number of Polarization Products in the case of zero-baselines given a baseband.
     * @param  bbIndex The baseband index
     * @exception not baseband independent
     * @return The number of Polarization Products \f$ N_{pp}^{o} \f$:
     * - if  CROSS_ONLY     the returned number is 0.
     * - if  AUTO_ONLY      this method is equivalent to the method getNumPol().
     * - if  CROSS_AND_AUTO use the method getNumPol() to get \f$ N_{pp}^{oo} \f$,
     *   the corresponding number of products for the non-zero baselines.
     * - if the input baseband name does not belong to the configuration return 0
     * @note To get number of products given a dataDescription use the method numPol().
     */
    unsigned int      getNumSdPol(unsigned int bbIndex)  ;

    /** Get the number of Polarization Products in the case of zero-baselines given a baseband.
     * @param  bbName The baseband index (0-based)
     * @return The number of Polarization Products \f$ N_{pp}^{o} \f$:
     * - if CROSS_ONLY     the returned number is 0.
     * - if AUTO_ONLY      this method is equivalent to the method getNumPol().
     * - if CROSS_AND_AUTO use the method getNumPol() to get \f$ N_{pp}^{oo} \f$,
     *   the corresponding number of products for the non-zero baselines.
     * - if the input baseband name does not belong to the configuration return 0
     * @note To get number of products given a dataDescription use the method numPol().
     */
    unsigned int      getNumSdPol(BasebandNameMod::BasebandName bbName) ;

    /** Get the index of a dataDescription given its index in its parent baseband
     * @param  bbIndex The baseband index
     * @exception bbIndex eqaul or exceeding the number of basebands used in the configuration
     * @param  j    The dataDescription index (zero-based) within that baseband
     * @exception j too large for the number of dataDescriptions in the given baseband
     * @return The index of the dataDescrition in the sequence of dataDescription in the
     *         configDescription
     */ 
    unsigned int      getNdd(unsigned int bbIndex, unsigned int j)  ;

    /** Get the index of a dataDescription given its index in its parent baseband
     * @param  bbName The baseband name
     * @param  j    The dataDescription index (zero-based) within that baseband
     * @exception no such babseband name in the configuration or j too large for the number of dataDescriptions given that baseband
     * @return The index of the dataDescrition in the sequence of dataDescription in the
     *         configDescription
     */ 
    unsigned int      getNdd(BasebandNameMod::BasebandName bbName, unsigned int j)  ;

    /** Get the number of Spectral Windows in a given baseband identified by its index.
     * @param  bbNum The baseband index (zero-based)
     * @exception bbNum illegal or exceeding the number of basebands used in the configuration
     * @return The number \f$ N_{sw} \f$ of spectral windows in the baseband
     */
    unsigned int      getNumSpw(unsigned int bbIndex)  ;

    /** Get the number of Spectral Windows in a given baseband identified by its name.
     * @param  bbName The baseband name
     * @return The number \f$ N_{sw} \f$ of spectral windows in the baseband 
     * or 0 if baseband does not belong to the configuration
     */
    unsigned int      getNumSpw(BasebandNameMod::BasebandName bbName);

    /** Get the dataDescription index given a dataDescriptionId identifier
     * @param dataDescriptionId The dataDescriptionId identifier
     * @exception dataDescriptionId does not exist in the set of dataDescription identifiers in the configuration
     * @return  The dataDescription index (0-based)
     */
    unsigned int      getDataDescriptionIndex( asdm::Tag dataDescriptionId)  ;

    /** Get the size of the block of data for the auto-correlations originating from one antenna. \n
     * This size is determined according to the formula \n
     *
     * \f$ sizeof(float) \times \sum_{i=1}^{N_{bb}} \left( N_{bin}(i)~f(N_{pp}(i))~ \sum_{j=1}^{N_{sw}(i)} N_{sp}(j) \right)  \f$
     * \n
     * where, for pure single-dish, (i.e. correlationMode=1), \n
       \f$
             f(N_{pp}(i)) = \left\| \begin{array}
             {r@{\quad if\quad}l}
             N_{pp}^{o}(i) & N_{pp}^{o}(i) \le 2   \\
             4 & N_{pp}^{o}(i)=3 \\
             \end{array} \right.
          
       \f$ 
       * \n
       * and for correlationMode=2 \n
       \f$
               f(N_{pp}(i)) = \left\| \begin{array}
               {r@{\quad if\quad}l}
                N_{pp}^{oo}(i) & N_{pp}^{oo}(i) \le 2~~ (i.e.~1~re~if~N_{pp}^{oo}(i)=1, ~2~re~if~N_{pp}^{oo}(i)=2)\\
                N_{pp}^{oo}(i) & N_{pp}^{oo}(i) = 4~~  (i.e.~3~re + 1~im,~standard~mode) \\
            \end{array} \right.
       \f$  
     * 
     */
    unsigned long     getAutoSize();

    /** Get the size of the block of cross-correlation data for the ensemble of spectral windows
     * and a single non-zero baseline. \n
     * This size is determined according to the formula \n
     * \f$
         sizeof(bitSize) \times 2 ~ N_{apc}~
               \sum_{i=1}^{N_{bb}} ~\left( N_{bin}(i)~N_{pp}^{oo}(i) \sum_{j=1}^{N_{sw}(i)} N_{sp}(j) \right) 
       \f$
     * \n
     * where there is a factor 2 because a visibility is a complex.\n
     * 
     * \f$ sizeof(bitSize)\f$=2 or 4 bytes (ref. note of Steeve).
     */
    unsigned long     getCrossSize();

    /** Number of auto-correlations produce for one antenna.
     *  \note If the number \f$N_{pp}^{o}\f$ of polarization products is 3 the cross product XY
     *  counts for one data quantity.\n 
     *  To get this number of polarization products \f$N_{pp}^{o}\f$ see the method numSdPol(int ndd); 
     *  to get it for a given dataDescription index or the method getNumSdPol(int bbNum) for a
     *  given baseband.
     */  
    unsigned long     getNumAutoData();

    /** Number of cross-correlations produced by one non-zero baseline for the set of dataDescriptions.
     *  \note This number must be understood as the number complex values, one data value being
     *   a complex quantity.
     */
    unsigned long     getNumCrossData();

    /** Number of auto-correlations produced by one antenna for one dataDescription.
     * \param autoDataDescriptionId dataDescription identifier of the subset of auto-correlations
     * \exception autoDataDescriptionId (eventualy as infered from crossDataDescriptionId) does not exist 
     * for the configuration setup
     *  \note If the number \f$N_{pp}^{o}\f$ of polarization products is 3 the cross product XY
     *  counts for one data quantity.\n 
     *  To get this number of polarization products \f$N_{pp}^{o}\f$ for a given dataDescription index
     *  see the method numSdPol(int ndd); 
     *  to get it for a given baseband, would it be baseband-based, see the method getNumSdPol(int bbNum).
     */  
    unsigned long     getNumAutoData(asdm::Tag autoDataDescriptionId) ;


    /** Number of cross-correlations produced by one non-zero baseline for a given dataDescription.
     * \param crossDataDescriptionId one of the dataDescription identifiers in the set used for the configuration setup
     * @exception crossDataDescriptionId does not exist in the set of dataDescription identifiers in the setup 
     * configDescription setup
     *  \note This number must be understood as the number complex values, one data value being
     *   a complex quantity.
     */
    unsigned long     getNumCrossData(asdm::Tag crossDataDescriptionId)  ;

    /** Get the array of dataDescriptionId identifiers for the auto-correlations
     * \param crossDataDescriptionId one of the dataDescription identifiers in the set used for the configuration setup
     * @exception crossDataDescriptionId does not exist in the set of dataDescription identifiers in the setup 
     * @return The array of dataDescriptionId identifier for the auto-correlations
     * \note In the case correlationMode=2 this array of identifiers is only implicit in configDescription. 
     * It is automaticly derived based on the input dataDescriptionId identifiers for the cross-correlations 
     * (the second parameter in the signature of the constructor of this class).
     */
    asdm::Tag               getAutoDataDescriptionId(asdm::Tag crossDataDescriptionId) ;

    /** Get the array of dataDescriptionId identifiers for the cross-correlations
     * @return The array of dataDescriptionId identifier for the auto-correlations (zero-baselines)
     * \exception Would correlationMode=0 the returned vector would have a size of 0
     */
    std::vector<asdm::Tag>       getAutoDataDescriptionId();

    /** Get the array of dataDescriptionId identifiers for the cross-correlations
     * @return The array of dataDescriptionId identifier for the cross-correlations
     * \exception would correlationMode=1 the returned vector would have a size of 0
     */
    std::vector<asdm::Tag>       getCrossDataDescriptionId();


  protected:
    std::vector<asdm::Tag>                 v_dataDescriptionIdArray_;//!< the input array of dataDescription identifiers

    Enum<CorrelationModeMod::CorrelationMode>       e_cm_;                    //!< Type-safe correlation mode
    CorrelationModeMod::CorrelationMode             correlationMode_;         //!< Correlation mode

    EnumSet<AtmPhaseCorrectionMod::AtmPhaseCorrection> es_apc_;                  //!< Atmospheric Phase Correction set
    std::vector<AtmPhaseCorrectionMod::AtmPhaseCorrection>  v_atmPhaseCorrection_;    //!< sequence of the Atmospheric Phase Correction values along the apc axis
    
    std::vector<unsigned int>        v_numPol_;                //!< number of cross-products (size numDataDesc_)
    std::vector<asdm::Tag>                 v_spwId_;                 //!< sequence of spectral window identifiers (size numDataDesc_)
    std::vector<unsigned int>        v_numChan_;               //!< sequence of nb of chan. i.e. spectral points (size numDataDesc_)
    std::vector<BasebandNameMod::BasebandName>        v_basebandName_;          //!< (size numDataDesc_), baseband name for every spectral window.
    std::vector<std::vector<int> >        vv_nsp_;                  //!< Nb of chan. per dataDescription (size numBaseband) vectors
    std::map<BasebandNameMod::BasebandName,std::vector<DataDescParams> > m_bn_v_ddp_; //!<  The sequence of dataDescParams in the different basebands
    std::map<asdm::Tag,BasebandNameMod::BasebandName>       m_ddid_bbn_;              //!< Association between the dataDescriptionId and the baseband name
    unsigned int                numApc_;                  //!< APC axis size (can take the value 1 or 2 only)
    unsigned int                numDataDescription_;      //!< Explicit number of dataDescriptionId 

    unsigned int                sumMetaDataIndex_;        //!< Sum of the elements of the vector v_metaDataIndex_
    unsigned long               sumAutoSize_;             //!< Sum of the elements of the vector v_autoSize_
    unsigned long               sumCrossSize_;            //!< Sum of the elements of the vector v_crossSize_

    std::vector<unsigned int>        v_metaDataIndex_;         //!< Indices for positions for every dataDesc (size numDataDesc_)
    std::vector<unsigned long>       v_cumulAutoSize_;         //!< Number of bytes to skip for every dataDesc (size numDataDesc_) 
    std::vector<unsigned long>       v_cumulCrossSize_;        //!< Number of bytes to skip for every dataDesc (size numDataDesc_) 
    std::vector<unsigned long>       v_numAutoData_;           //!< Number of auto-correlations per antenna (size numDataDesc_) 
    std::vector<unsigned long>       v_numCrossData_;          //!< Number of cross-correlations per antenna pair (size numDataDesc_) 
    std::vector<unsigned long>       v_autoSize_;              //!< Size, in nb PDT values, for every dataDesc (size numDataDesc_) 
    std::vector<unsigned long>       v_crossSize_;             //!< Size, in nb PDT values, for every dataDesc (size numDataDesc_) 


    std::vector<asdm::Tag>                 v_crossDataDescriptionId_;//!< Vector of dataDecsritionId for the cross-correlations (size numDataDesc_)
    std::vector<asdm::Tag>                 v_autoDataDescriptionId_; //!< Vector of dataDecsritionId for the auto-correlations (size <= numDataDesc_)
    std::vector<bool>                v_pairDataDescriptionId_; //!< true if crossDataDecsritionId assoc to autoDataDecsritionId (size numDataDesc_)


    std::vector<BasebandNameMod::BasebandName>        v_basebandSet_;           //!< Sequence of baseband names
    EnumSet<BasebandNameMod::BasebandName>       e_basebandSet_;           //!< Set of baseband names

    std::vector<unsigned int>        v_numSpwPerBb_;           //!< Number of spectral windows in every baseband

  private:
    
    void                        size();                   // Method used by the constructor
    void                        metaDataIndex();          // Method used by the constructor

    void mapScToBaseband();                               // Method to map vectors of switchCycles to the basebandName.
  };

  inline unsigned int DataDescriptionsSet::numBin(unsigned int ndd) { // throw(Error){
    // if(ndd>=v_basebandName_.size())
    //   Error(SERIOUS,"The dataDescription index must be smaller than %d",
    // 	    v_basebandName_.size());
    return v_numBin_[ndd];  
  }
  
}
#define _DATADESCRIPTIONSSET_H
#endif
