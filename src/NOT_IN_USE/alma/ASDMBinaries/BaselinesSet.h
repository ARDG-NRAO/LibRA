#if     !defined(_BASELINESSET_H)
 
#include <alma/ASDM/ASDMEntities.h>
#include <alma/ASDMBinaries/Error.h>
#include <alma/ASDMBinaries/BaselineMetadata.h>
#include <alma/ASDMBinaries/SwitchCyclesList.h>
#include <alma/ASDMBinaries/DataDescriptionsSet.h>

#include <alma/Enumtcl/AxisName.h>
#include <alma/Enumtcl/DataContent.h>


namespace sdmbin {
  
  /** Definition of the second level (the top level) in the tree hierarchy: include the antenna baseline
   *  configuration */ 
  class BaselinesSet : public DataDescriptionsSet
  {
  public:
    BaselinesSet();

    /** Definition of the second level of the tree hierarchy 
     * @pre the lower levels of the tree hierarchy are already known
     * @param v_antennaIdArray  Array of antenna identifiers
     * @param v_feedIdArray     Array of feed identifiers 
     *   \note                    Because with ALMA there is only one feed used at any time this vector has 
     *                            the same size as v_antennaIdArray. With Focal Plane Array this size is
     *                            multiplied by the number of feeds in the FPA
     * @param v_phasedArrayList Place-holder, not yet implemented
     * @param v_antennaUsedArray Array to tell in the antennaIdArray which antenna have been actually used
     *                           for the data being stored. v_antennaUsedArray and v_antennaIdArray must have
     *                           the same size.
     * @param dataDescriptionsSet a dataDescriptionsSet object (<i>i.e.</i> the lower levels of the hierarchy 
     * @post the whole tree hierarchy is now known, transfer identifier available for read and write methods
     * @note v_feedIdArray may be typed vector<vector<int> > in the future to better describe the case of
     *       of focal plane arrays when used with interferometers
     */     
    BaselinesSet( std::vector<asdm::Tag>  v_antennaIdArray, 
		  std::vector<int>  v_feedIdArray,
		  std::vector<int>  v_phasedArrayList,
		  std::vector<bool> v_antennaUsedArray, 
		  DataDescriptionsSet& dataDescriptionsSet
		  );

    /** Copy constructor */
    BaselinesSet(const BaselinesSet &);

    /** Destructor (use the default implementation) */
    ~BaselinesSet();

    /** Transfer identifier for a given node in the tree hierarchy for auto-correlation data
     *  Transfer identifier for a given node in the tree hierarchy for auto-correlation data
     *  @pre WARNING: if the use-case is CROSS_AND_AUTO and the current index ndd corresponds
     *       to the case when crossDataDescriptionId is not associated to an autoDataDescriptionId,
     *       i.e. when v_pairDataDescriptionId[ndd]==false, it is an error to invoke this method. 
     *       This situation is met with DSB front-ends, the sideband separation being possible only 
     *       for the cross data. 
     *  @param  na   Index of the antenna
     *  @param  ndd  Index of the dataDescription
     *  @param  nbin Index for the step in a switchCycle (nbin=0 if no switchCycle mode used)
     *  @return A position (counted in number of PDT values) in the container of auto-correlation 
     *          data where the data starts for the selection
     *  @note Note that the APC axis has a size of 1 for the auto-correlations, even when, in the 
     *        CROSS_AND_AUTO mode, the cross data are stored with an APC axis size of 2. Would TelCal
     *        not like this feature will have to be modified in the method size() of the base class
     *        DataDescriptionsSet and this will have an additional napc input parameter.  
     */ 
    unsigned int transferId(unsigned int na,  unsigned int ndd, unsigned int nbin);

    /** Transfer identifier for a given node in the tree hierarchy for auto-correlation data
     *  @pre WARNING: if the use-case is CROSS_AND_AUTO and the current index ndd corresponds
     *       to the case when crossDataDescriptionId is not associated to a autoDataDescriptionId,
     *       i.e. when v_pairDataDescriptionId[ndd]==false, it is an error to invoke this method. 
     *       This situation is met with DSB front-ends, the sideband separation being possible only 
     *       for the cross data. 
     *  @param  na   Index of the antenna
     *  @param  nfe  Index of the feed (required when using FPAs)
     *  @param  ndd  Index of the dataDescription
     *  @param  nbin Index for the step in a switchCycle (nbin=0 if no switchCycle mode used)
     *  @return A position (counted in number of PDT values) in the container of auto-correlation 
     *          data where the data starts for the selection
     *  @note Note that the APC axis has a size of 1 for the auto-correlations, even when, in the 
     *        CROSS_AND_AUTO mode, the cross data are stored with an APC axis size of 2. Would TelCal
     *        not like this feature will have to be modified in the method size() of the base class
     *        DataDescriptionsSet and this will have an additional napc input parameter.  
     */ 
    unsigned int transferId(unsigned int na,  unsigned int nfe, unsigned int ndd, unsigned int nbin);
    
    /** Transfer identifier for a given node in the tree hierarchy for cross-correlation data
     *  @param  na1  Index of the antenna 1
     *  @param  na2  Index of the antenna 2
     *  @param  ndd  Index of the dataDescription
     *  @param  nbin index for the step in a switchCycle (nbin=0 if no switchCycle mode used)
     *  @return A position (counted in number of PDT values) in the container of cross-correlation 
     *          data where the data starts for the selection
     *  @note A baseline corresponds to a pair of indices. The convention is that the first index is 
     *        smaller than the second one (i.e. na1<na2). In case na2<na1, this method will consider 
     *        na2 as the first index and na1 as the second one.
     */ 
    unsigned int transferId(unsigned int na1, unsigned int na2, unsigned int ndd, unsigned int nbin, unsigned int napc);

    /** Transfer identifier for a given node in the tree hierarchy for cross-correlation data
     *  @param  na1  Index of the antenna 1
     *  @param  na2  Index of the antenna 2
     *  @param  nfe  Index of the feed (required when using FPAs)
     *  @param  ndd  Index of the dataDescription
     *  @param  nbin Index for the step in a switchCycle (0nbin=0 if no switchCycle mode used)
     *  @return A position (counted in number of PDT values) in the container of cross-correlation 
     *          data where the data starts for the selection
     *  @note A baseline corresponds to a pair of indices. The convention is that the first index is 
     *        smaller than the second one (i.e. na1<na2). In case na2<na1, this method will consider 
     *        na2 as the first index and na1 as the second one.
     */ 
    unsigned int transferId(unsigned int na1, unsigned int na2, unsigned int nfe, unsigned int ndd,  unsigned int nbin, unsigned int napc);

    /** Baseline number for a given antenna identifier. This number is equal to i+1 where i is the antenna index in the
     * list of antennas effectively producing data. In this context this is the 'zero-baseline' number. This number
     * never exceed the number of effectively used antennas (given by the method getNumEffAntennas()).
     * @param  antennaId Antenna identifier
     * @exception antennaId does not correspond to any of the antenna effectively producing data. 
     * @return The baseline number (one-based)
     * @exception Error if there is no antenna effectively 
     * producing data with this antennaId.
     */
    unsigned int baselineIndex( asdm::Tag antennaId)  ;

    /** Baseline number for a given pair of antenna identifiers
     * @param  antennaId1 Antenna identifier of one of the antenna in a pair of a non-zero baseline
     * @exception antennaId1 does not correspond to any of the antenna effectively producing data. 
     * @param  antennaId2 Antenna identifier of the other antenna in a pair to define the non-zero baseline
     * @exception antennaId2 does not correspond to any of the antenna effectively producing data. 
     * @return The baseline number (one-based)
     * @exception Error if antennaId1 or (and) antennaId2 is (are) not associated to an antenna producing 
     * effectively data. 
     * \note Order of the baselines:\n
     *      Example 1: Consider an antenna array with the list of identifiers (1,2,3,5) the pairs of identifier
     *      defining the baselines are in the order (1,2 1,3 2,3 1,5 2,5 3,5).  
     *      Hence, if antennaId1=2 and antennaId2=3 the returned value is 3. \n
     *      Example 2: Consider an antenna array with the list of identifiers (5,2,3,1) the pairs of identifier
     *      defining the baselines are in the order (5,2 5,3 2,3 5,1 2,1 3,1).  
     *      If antennaId1=2 and antennaId2=3 the returned value is 3. If antennaId1=5 and antennaId2=1 the returned 
     *      value is 4. Note that if antennaId1=1 and antennaId2=5 the returned value will be again 4 due to the
     *      convention that baselines are defined on the basis of a pair onf indices (see the note in the documentation
     *      for the transferId(int, int, int, int, int) method.
     */ 
    unsigned int baselineIndex( asdm::Tag antennaId1, asdm::Tag antennaId2) ;

    /** Baseline number for a given pair of antenna indices in the list of antennas effectively producing data
     * @param  na1 index (zero-based) of antenna 1
     * @exception na1 exceeds the maximum limit imposed by the number of antennas effectively producing data
     * @param  na2 index  (zero-based) of antenna 2
     * @exception na2 exceeds the maximum limit imposed by the number of antennas effectively producing data
     * @return the corresponding baseline number (one-based)
     * @exception Error this methods returns 0 in lax mode if na1 or na2 exceeds this maximum limit
     * @note 
     * -# Order of the baselines:\n
     *      Example for an antenna array consisting of 4 antennas the indices describing this array range
     *      from 0 to 3. The pairs of indices for the baselines are in the order (0,1 0,2 1,2 0,3 1,3 2,3). Hence, if
     *              na1=1 and na2=3 the returned value is 5.
     * -# By definition the first index in any pair being always smaller than the second index, would in the input na1>na2,
     * the method swaps these indices. 
     */ 
    unsigned int baselineIndex( unsigned int na1, unsigned int na2) ;

    /** Antenna index of the first antenna of a pair defining a baseline number 
     * @param baselineIndex A baseline number (zero-based)
     * @exception baselineIndex not zero-based
     * @return the index for the first antenna of the pair in the list of antennas effectively producing data
     * @exception Error if there is no such baselineIndex in the configuration.
     * @note See the note in the documentation of the baselineIndex() method to know
     *       how baselines are ordered and how baselines correspond to pairs of 
     *       antenna indices. 
     */
    unsigned int antenna1(unsigned int baselineIndex)  ;

    /** Antenna index of the second antenna of a pair defining a baseline number
     * @param baselineIndex A baseline number (one-based)
     * @exception baselineIndex not one-based
     * @return the index for the second antenna of the pair
     * @exception Error if there is no such baselineIndex in the configuration.
     * @note See the note in the documentation of the baselineIndex() method to know
     *       how baselines are ordered and how baselines correspond to pairs of 
     *       antenna indices. 
     */
    unsigned int antenna2(unsigned int baselineIndex)  ;

    /** Accessor to the feed index of a feed identifier given an antenna identifier
     * @param antennaId Antenna identifier of an antenna
     * @param feedId    Feed identifier
     * @return The feed index (zero-based)
     * @exception Error
     * - No antenna with this antenna identifier. In this case, in lax mode, this method return -1.
     * - No feed with this feed identifier given the antenna identifier. In this case, in lax mode, this method return -2.
     */
    unsigned int feedIndex(asdm::Tag antennaId, int feedId) ;

    /** Antenna identifier from its index in the sequence of antenna restricted to those effectively producing data
     * @param na The antenna index in the list of \f$ N'_{ant}\f$ antennas effectively producing data
     * @exception na exceeds \f$ N'_{ant}-1\f$
     * @return The antenna identifier
     */
    asdm::Tag         getEffAntennaId(unsigned int na) ;

    /** Number of antenna involved in the data if none would have been dropped
     * @return this number \f$ N_{ant} \f$ which is also the number of antenna scheduled for the observations
     */
    unsigned int getNumAntennas();


    /** Accessor to the feed identifier given a feed index and an antenna index
     * @param na The antenna index in the list of \f$ N'_{ant}\f$ antennas effectively producing data
     * @exception na exceeds \f$ N'_{ant}-1\f$
     * @param nfe The feed index
     * @exception nfe exceeds the upper limit considering the number of feeds used in the configuration.
     * @return a feed identifier
     * @note In general for telescopes with no focal plane arrays (FPA), this identifier should have always
     *       the value 0. Although ALMA does not have FPAs, a feed identifier may also have the value 1. That
     *       would happen in datasets with spectral windows located in the overlap region of two receiver bands,
     *       e.g. the ALMA bands 2 and 3, some of the data obtained with band 2 and the other
     *       data with band 3 for identical spectral windows.
     */   
    int         getFeedId(unsigned int na, unsigned int nfe) ;

    /** Number of antenna actualy involved in the data
     * @return the actual number \f$ N'_{ant} \f$ producing data
     * @note The constraint is that this number can not exceed the number of antenna scheduled
     *       for the observations: \f$ N'_{ant} \le N_{ant} \f$. \n
     *       This number \f$ N'_{ant}\f$ is equal to the number of items in the vector antennaUsedArray
     *       which have the value "true", this vector being a parameter required by the constructors.
     *
     * See the BaselineFlagsMetadata class for details to define or find when a given antenna produce
     * or not data. 
     */
    unsigned int getNumEffAntennas();

    /** Get the number of antenna pairs if all the antennas in  v_AntennaIdArray_ were used 
     * @return The number of baselines actually scheduled for the observations.
     * @note  This is simply given by \f$ N_{bl}=N_{ant}(N_{ant}-1)/2 \f$
     */
    unsigned int getNumBaselines();

    /** Get the effective number of antenna pairs producing data
     * @return The number of antenna actually producing data
     * @note  This is simply given by \f$ N'_{bl}=N'_{ant}(N'_{ant}-1)/2 \f$\n
     * where N'_{ant} is the value returned by the method getNumEffAntennas().
     */
    unsigned int getNumEffBaselines();
    
    /** Get the number of primitive data values (i.e. when a complex number counts for
     *  two values) for a block of data.
     * @param e_dc the nature of the data content
     * @param es_an the sequence of axes associated to the structure for this block of data
     * @param effective a boolean to tell if one consider only the antennas which actualy
     *        produce data or all those scheduled to do the observations. 
     * @note  Note that ALMA has dropped this concept of restricting to the antennas that 
     *        have effectively produced data. Hence in that context "effective" must be set
     *        to false and the data involving the antennas with problem will be considered
     *        as blanked on the basis of the FLAGS information.
     */
    unsigned int getNumPDTvalue(Enum<DataContentMod::DataContent> e_dc, EnumSet<AxisNameMod::AxisName> es_an, bool effective);

  private:
    std::vector<asdm::Tag>  v_AntennaIdArray_;   //!< Input array set of antenna identifiers (size numAntennas_)
    std::vector<int>  v_FeedIdArray_;      //!< Input array list of feed identifiers (size numFeed*numAntennas_)
    std::vector<int>  v_PhasedArrayList_;  //!< Input phasedArray list (not yet implemented) 
    unsigned int numFeeds_;           //!< Input number of feeds
    unsigned int numAntennas_;        //!< Input number of antennas
    unsigned int numEffAntennas_;     //!< Effective number of antennas with data
    unsigned int numBaselines_;       //!< number of antenna pairs
    unsigned int numEffBaselines_;    //!< Effective number of antenna pairs
    std::vector<asdm::Tag>  v_effAntennaIdArray_;//!< Effective array of antenna identifiers

  };


  inline unsigned int BaselinesSet::transferId(unsigned int na1,unsigned int na2,unsigned int nfe, unsigned int ndd,unsigned int nbin,unsigned int napc){

    unsigned int baselineidx = baselineIndex(na1,na2);                   //cout << "baselineidx=" << baselineidx << endl;

    unsigned int v_cumulCrossSize_ndd=0; if(v_cumulCrossSize_.size()!=0)v_cumulCrossSize_ndd = v_cumulCrossSize_[ndd];
    unsigned int v_crossSize_ndd=0;      if(v_crossSize_.size()!=0)     v_crossSize_ndd      = v_crossSize_[ndd];

    unsigned int n = 
      baselineidx*numFeeds_*sumCrossSize_ +                           // /baseline
      nfe*sumCrossSize_ +                                             // /baseline/feed     
      v_cumulCrossSize_ndd +                                          // /baseline/feed/datadesc
      nbin*(v_crossSize_ndd/numBin(ndd)) +                            // /baseline/feed/datadesc/bin
      napc*(v_crossSize_ndd/(numBin(ndd)*numApc_));                   // /baseline/feed/datadesc/bin/napc

    return n;
  }

  inline unsigned int BaselinesSet::baselineIndex( unsigned int na1, unsigned int na2) {
    unsigned int iIdx = std::min (na1, na2);
    unsigned int jIdx = std::max (na1, na2);
    return jIdx * (jIdx - 1) / 2 + iIdx;
  }
}
#define _BASELINESSET_H
#endif
