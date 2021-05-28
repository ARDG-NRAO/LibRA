#if     !defined(_INTEGRATION_H)

#include <vector>
#include <alma/Enumtcl/CorrelationMode.h>
#include <alma/Enumtcl/AxisName.h>
#include <alma/Enumtcl/NetSideband.h>

namespace sdmbin {

  /** A class which gathers the attributes required to define data structures. A 
      structure is abstract; it may have the properties of a multi-dimensional
      structure or of a tree-hierarchy, the dimensionality being reflected by the 
      number of axes in one case or levels in the other case.

      A structure may have up to 7 levels:
      - 1 pol (polarization product axis) or hct HolographyChannelType
      - 2 sp  (spectral point axis)
      - 3 apc (atmospheric correction axis)
      - 4 bin (bin axis used eg when using a switching modes such as frequency switch)
      - 5 spw (spectral window)
      - 6 bb  (baseband)
      - 7 bl  (baseline)
      - 8 an  (antenna, ie zero-baseline)

      The choice between the axis 7 and 8 depends on the correlation mode. Note that
      for meta-data 7 and 8 can be considered implicitly at the same level. For the
      actual data (visibilities or auto-correlations) 7 and 8 are each other exclusive.

      @note The header of a SDM BLOB has all the informations required to define a
      data structure. Note also that there are two attributes based on enumerations,
      sideband and correlationMode. Strictly speacking these do not impact of the 
      data structure. They merely qualify the data contents. 

      The attribute 'sideband' provides purely semantic information. When there 
      are sidebands it is not mandatory to group substructures by pairs of sidebands 
      because it is not required to store the image sideband.

      The attribute 'correlationMode' provides semantic information for the context
      which constrains the nature of the data, each of these having their own data 
      structure. 
  */ 
  class DataStructure{
  public:
    /** Empty constructor */
    DataStructure();

    /** Constructor for a multi-dimensional structure to use e.g. for total power data
        produced by the baseband processor with correlationMode=1.  Determine the 
	property of the structure.
        @note For simulated data which do not account for sideband effects the attribute
	e_sideband may have no NetSideband enumerator set. For true data, the hardware 
	used being in principle always known, the appropriated enumerator should be set. 
	The attribute 'sideband' being mandatory in the SDM-BDF, when there is no 
        enumerator set, the value stored for that format will be NOSB.
    */
    DataStructure(  uint32_t      numPolProduct,     //!< Number of polarization products
		    uint32_t      numBin,            //!< Number of step in a switching cycle
		    Enum<NetSidebandMod::NetSideband> e_sideband,        //!< Sideband qualifier (not set or NOSB | LSB | USB | DSB )
		    uint32_t      numBaseband,       //!< Number of basebands
		    uint32_t      numAnt,            //!< Number of antennas
		    CorrelationModeMod::CorrelationMode   correlationMode);  //!< Correlation mode (CROSS_ONLY | AUTO_ONLY | CROSS_AND_AUTO )

    /** An other constructor  for a multi-dimensional structure use-case.
	Determine the property of the structure.

	@param numPolProduct Number of polarization cross-products
	@param numSpectralPoint Number of spectral points in the spectral window
	@param numBin       Number of bins in the switch cycle 
	@param numBaseband  Number of basebands
	@param numAnt       Number of antenna (used to determine the number
                            of baselines if correlationMode=CROSS_ONLY or CROSS_AND_AUTO
	@param e_sideband   Sideband NOSB | LSB | USB | DSB
	@param 	correlationMode
          - CROSS_ONLY     only non-zero antenna baselines
	  - AUTO_ONLY      only zero baselines (single-dish)
	  - CROSS_AND_AUTO both zero and non-zero.

        @note For simulated data which do not account for sideband effects the attribute
	e_sideband may have no NetSideband enumerator set. For true data, the hardware 
	used being in principle always known, the appropriated enumerator should be set. 
	The attribute 'sideband' being mandatory in the SDM-BDF, when there is no 
        enumerator set, the value stored for that format will be NOSB.
     */
    DataStructure(  uint32_t      numPolProduct,
		    uint32_t      numSpectralPoint,
		    uint32_t      numBin,
		    Enum<NetSidebandMod::NetSideband> e_sideband,
		    uint32_t      numBaseband,
		    uint32_t      numAnt,
		    CorrelationModeMod::CorrelationMode   correlationMode);

    /** Constructor in the general case, the data structure being eventualy
	a tree-hierarchy. Determine the property of the structure.
	@param 	vv_numPolProduct Number of polarization product in every
                                 spectral window for every baseband.
				 This vector has a size equal to numBaseband.  
	@param 	 vv_numSpectralPoint Number of spectral points in every
                                     spectral window for every baseband. 
				     This vector has a size equal to 
                                     numBaseband. 
	@param 	vv_numBin Number of bins (steps in a switch cycle) in every 
                                 spectral window for every baseband.
				 This vector has a size equal to numBaseband. 
	@param  vv_e_sideband Sideband qualifying every spectral window of
                              every baseband.        
	@param 	numApc Size o the atmospheric correction axis
	@param 	v_numSpectralWindow Number of spectral window in for every
                                    baseband. This vector has a size equal
                                    to numBaseband.
	@param 	numBaseband         Number of baseband
	@param 	numAnt              Number of antenna (used to determine the number
                                    of baselines if correlationMode=0 or 2
	@param 	correlationMode  An enumerator:    
             - CROSS_ONLY      only non-zero antenna baselines
	     - AUTO_ONLY       only zero baselines (single-dish)
	     - CROSS_AND_AUTO  both zero and non-zero.
    */
    DataStructure( std::vector<std::vector<uint32_t> >       vv_numCrossPolProduct,// /bb/spw
		   std::vector<std::vector<uint32_t> >       vv_numAutoPolProduct, // /bb/spw
		   std::vector<std::vector<uint32_t> >       vv_numSpectralPoint,  // /bb/spw
		   std::vector<std::vector<uint32_t> >       vv_numBin,            // /bb/spw
		   std::vector<std::vector<Enum<NetSidebandMod::NetSideband> > > vv_e_sideband,        // /bb/spw
		   uint32_t                        numApc,
		   std::vector<uint32_t>                v_numSpectralWindow,  // /bb
		   uint32_t                        numBaseband,
		   uint32_t                        numAnt,
		   CorrelationModeMod::CorrelationMode                     correlationMode);

    /** Constructor for the single-dish case, the data structure being eventually
	a tree-hierarchy. Determine the property of the structure.
	@param 	vv_numPolProduct Number of polarization product in every
                                 spectral window for every baseband.
				 This vector has a size equal to numBaseband.  
	@param 	 vv_numSpectralPoint Number of spectral points in every
                                     spectral window for every baseband. 
				     This vector has a size equal to 
                                     numBaseband. 
	@param 	vv_numBin Number of bins (steps in a switch cycle) in every 
                                 spectral window for every baseband.
				 This vector has a size equal to numBaseband. 
	@param  vv_e_sideband Sideband qualifying every spectral window of
                              every baseband.        
	@param 	numApc Size o the atmospheric correction axis
	@param 	v_numSpectralWindow Number of spectral window in for every
                                    baseband. This vector has a size equal
                                    to numBaseband.
	@param 	numBaseband         Number of baseband
	@param 	numAnt              Number of antenna (used to determine the number
                                    of baselines if correlationMode=0 or 2
	@param 	correlationMode An enumerator:
             - CROSS_ONLY      only non-zero antenna baselines
	     - AUTO_ONLY       only zero baselines (single-dish)
	     - CROSS_AND_AUTO  both zero and non-zero.
    */
    DataStructure( std::vector<std::vector<uint32_t> >       vv_numAutoPolProduct, // /bb/spw
		   std::vector<std::vector<uint32_t> >       vv_numSpectralPoint,  // /bb/spw
		   std::vector<std::vector<uint32_t> >       vv_numBin,            // /bb/spw
		   std::vector<std::vector<Enum<NetSidebandMod::NetSideband> > > vv_e_sideband,        // /bb/spw
		   uint32_t                        numApc,
		   std::vector<uint32_t>                v_numSpectralWindow,  // /bb
		   uint32_t                        numBaseband,
		   uint32_t                        numAnt,
		   CorrelationModeMod::CorrelationMode                     correlationMode);

    /** Copy constructor */
    DataStructure( const DataStructure &);

    /** Destructor (use the default implementation) */
    ~DataStructure();


    /** Accessor to the number of polarization cross product for non-zero baselines
	in every spectral window for every baseband
    */
    std::vector<std::vector<uint32_t> > numCrossPolProducts();

    /** Accessor to the number of polarization cross product for zero-baselines
	in every spectral window for every baseband
    */
    std::vector<std::vector<uint32_t> > numAutoPolProducts();

    /** Accessor to the number of spectral points in every spectral window
	for every baseband
    */
    std::vector<std::vector<uint32_t> > numSpectralPoints();


    /** Accessor to the number of bins in every spectral window
	for every baseband
    */
    std::vector<std::vector<uint32_t> > numBins();

    /** Accessor to the NetSideband qualifiers in every spectral window
	for every baseband
    */
    std::vector<std::vector<Enum<NetSidebandMod::NetSideband> > > sidebands();

    /** Accessor to the size of the APC axis */
    uint32_t                  numApc();

    /** Accessor to the number of spectral windows in the different basebands */
    std::vector<uint32_t>          numSpectralWindows();

    /** Accessor to the number of basebands */
    uint32_t                  numBaseband();

    /** Accessor to the number of antennas */
    uint32_t                  numAnt();

    /** Accessor to the correlationMode 
     - 0 implies only interferometric 
     - 1 implies only single-dish
     - 2 implies both interferometric and single-dish
    */
    CorrelationModeMod::CorrelationMode                 correlationMode();

    /** Method to get the axis sizes of the leaves when represented
	as multi-dimensional structures. A multi-dimensional leaf is a
	an array with at most 4 axes:
	- pol (polarization product axis)
	- sp  (spectral point axis)
	- apc (atmospheric phase correction axis)
	- bin (bin axis)
	@return the axis sizes of the leaf, the size of the vector 
	corresponding to the dimensionality of the leaf. 
    */
    std::vector<uint32_t> leafAxisSizes();

    /** Axis sizes of the leave of a data structure which may not
        be necessarily multi-dimensional. A multi-dimensional leaf 
	is an array with at most 4 axes:
	- pol (polarization product axis)
	- sp  (spectral point axis)
	- apc (atmospheric phase correction axis)
	- bin (bin axis)
	@param basebandIndex the baseband index (zero-based)
	@param spectralWindowIndex the spectral index (zero-based) in the selected baseband 
	@return the axis sizes of the leaf, the size of the vector 
	corresponding to the dimensionality of the leaf.
    */
    std::vector<uint32_t> leafAxisSizes(uint32_t basebandIndex, uint32_t spectralWindowIndex);

    /** Axis sizes of the leave of a data structure which may not
        be necessarily multi-dimensional. A multi-dimensional leaf 
	is an array with at most 4 axes:
	- pol (polarization product axis)
	- sp  (spectral point axis)
	- apc (atmospheric phase correction axis)
	- bin (bin axis)
	@pre   The spectral windows are grouped per baseband (this must
               be always the case)
	@param dataDescriptionIndex (zero-based)
	@param spectralWindowIndex the spectral index (zero-based) in the selected baseband 
	@return the axis sizes of the leaf, the size of the vector 
	corresponding to the dimensionality of the leaf.
    */
    std::vector<uint32_t> leafAxisSizes(uint32_t dataDescriptionIndex);


    /** A method to know if the whole data structure can be considered as a
	multi-dimensional structure or not.
	@return the dimensionality (return 0 in case of 
                a tree structure).
	@post when the structure is multi-dimensional the
              access to the data may be retrieved in the
              form of an array with this dimensionality.
    */
    uint32_t         isIndexible() const;


    /** A method to retrieve the size of all the different axes
       in case of a multi-dimensional structure.
       @return vector<uint32_t> a vector of axis sizes
              This vector has a size of 0 in case of a tree 
              structure else it is the number of axes (including
              those with a size of 1)
    */
    std::vector<uint32_t> eAxisSizes() const;

    /** A method to retrieve the size of the axes for a
        multi-dimensional structure. This is the compact
        form where all the axes of size 1 do not participate
        to set the dimension of the structure. This vector 
	has a size of 0 in case of a tree structure.
    */
    std::vector<uint32_t> axisSizes() const;

    /** Accessor to the dimensionality of the structure.
	If this is a tree-structure this is the number of
        levels in the hierarchy.
    */
    uint32_t         dimension() const;

    /** Minimum size for every axis (level) in the structure.
	The return size of the vector is equal to the dimension
        of the structure
    */
    std::vector<uint32_t> minAxSize() const;

    /** Maximum size for every axis (level) in the structure.
	The return size of the vector is equal to the dimension
        of the structure
    */
    std::vector<uint32_t> maxAxSize() const;

    /** Number of data value (leaves) that can host the structure
     */
    uint32_t         numAutoData() const;

    /** Number of data value (leaves) that can host the structure
     */
    uint32_t         numCrossData() const;

    /** Axis sequense: For example 128 means a structure with
                       3 axes, pol,sp,na
    */
    std::string               axisSequence() const;

    /** Utility which sumarizes the properties of the data structure.
     */
    void                 summary() const;

  protected:
    std::vector<std::vector<uint32_t> >       vv_numCrossPolProduct_;// /bb/spw
    std::vector<std::vector<uint32_t> >       vv_numAutoPolProduct_; // /bb/spw
    std::vector<std::vector<uint32_t> >       vv_numSpectralPoint_;  // /bb/spw
    std::vector<std::vector<uint32_t> >       vv_numBin_;            // /bb/spw
    std::vector<std::vector<Enum<NetSidebandMod::NetSideband> > > vv_e_sideband_;        // /bb/spw
    uint32_t                        numApc_;
    std::vector<uint32_t>                v_numSpectralWindow_;  // /bb
    uint32_t                        numBaseband_;
    uint32_t                        numAnt_;
    CorrelationModeMod::CorrelationMode                     correlationMode_;
    std::string                              axisSequence_;
    std::vector<uint32_t>                v_minSize_;
    std::vector<uint32_t>                v_maxSize_;
    
  private:
    std::string                              setStructureProperties();
  };

  /** The class DataDump extends the class DataStructure by adding time information
      to assign to the dump the observation date etc... It also get effectively 
      the data values with eventualy meta-data (flags, actualTimes, actualDurations), auxiliary
      data (zeroLags). These may be either attached or truly imported. 
      - Attach means retrieving read-only arrays, either passed by the client in the parameter 
      list of constructors or getting them via an attach method after creating the DataDump typed
      objects.
      - Import means deep copy of arrays via an import method. In that case the destructor delete
      these imported data when the objects are destroyed.
      
      A set of constructors is provided to avoid unecessary complexities when the context
      is simpler than the most general one (e.g. when using a simple filter bank spectrometer
      with a single dish vs multiple baseband/spectral windows with a multiplicity of antennas).   

      Behaviors: there are two manipulation categories:
      - axis-size invariant manipulations with arithmetic operators
      - manipulations which produce output objects with axis-sizes different those in the input objects.

      DataDump typed objects resulting from these manipulation are owner of their data, these being 
      deleted when these objects are destroyed.

      DataDump typed objects are by default anonymous objects in the context of the project data structure.
      To be identified in this context it is necessary to tag them by their integration number and eventually
      their sub-integration number. DataDump typed object resulting from the combination of DataDump typed 
      objects become anonymous. However they keep the information to know tagged object they derived from.
  */    
  class DataDump : public DataStructure {
  public:
    DataDump();
    
    /** A data dump is an object which can be saved at a persistent level. It has 
        a data structure and time informations to tell when this dump was obtained.

	The following constructor is convenient e.g. for the total power from the 
        baseband processor if all the data are valid. 
        With this constructor the data values, read-only, are attached.
	Would the data be averaged over the duration of a (sub)integration, they
	must be subsequently tagged by their integration (and sub-integration) number(s)
	using the method setIntegration (or setSubintegration).

        @note The arrival time of the data dumps may not be synchronized with the
        time stamp of an integration or sub-integration.

	@param numPolProduct Number of polarization cross-products
	@param numBin       Number of bins in the switch cycle 
	@param e_sideband   Sideband qualifier  (not set or NOSB | LSB | USB | DSB )
	@param numBaseband  Number of basebands
	@param numAnt       Number of antenna (used to determine the number
                            of baselines if correlationMode=0 or 2
	@param 	correlationMode  ( CROSS_ONLY | AUTO_ONLY | CROSS_AND_AUTO ) 
    */
    DataDump( uint32_t      numPolProduct,
	      uint32_t      numBin,
	      Enum<NetSidebandMod::NetSideband> e_sideband,
	      uint32_t      numBaseband,
	      uint32_t      numAnt,
	      CorrelationModeMod::CorrelationMode   correlationMode,
	      uint64_t         time,
	      uint64_t         timeCentroid,  
	      uint64_t         interval, 
	      uint64_t         exposure,
	      const float*      floatData);

    /** A constructor convenient for the total power from the baseband processor
	when some data have been flagged. Would the data averaged over the duration 
	of an (sub)integration, consider using subsequently the setIntegration 
	(or setSubintegration) method.
	@param numSdPolProduct  Number of polarization cross-products (zero-baselines)
	@param numSpectralPoint Number of spectral points in the spectral window
	@param numBin           Number of bins in the switch cycle 
	@param e_sideband       Sideband qualifier  (not set or NOSB | LSB | USB | DSB )
	@param numBaseband      Number of basebands
	@param numAnt           Number of antenna (used to determine the number
                                of baselines if correlationMode=0 or 2
	@param correlationMode   ( CROSS_ONLY | AUTO_ONLY | CROSS_AND_AUTO )
	  - CROSS_ONLY     only non-zero antenna baselines
	  - AUTO_ONLY      only zero baselines (single-dish)
	  - CROSS_AND_AUTO both zero and non-zero.
	@param time Mid-point (over the interval) epoch for the dump
        @param timeCentroid Centroid epoch associated to the exposure
                            This is different than the time value is
                            blanking occured during the observations.
	@param interval     Duration scheduled to produce the dump.
        @param exposure     Actual duration effectively spent (this
                            is the difference between interval and 
                            the time lost due to blanking.
        @param dataFlags    The data values.
    */
    DataDump( uint32_t      numSdPolProduct,
	      uint32_t      numSpectralPoint,
	      uint32_t      numBin,
	      Enum<NetSidebandMod::NetSideband> e_sideband,
	      uint32_t      numBaseband,
	      uint32_t      numAnt,
	      CorrelationModeMod::CorrelationMode   correlationMode,
	      uint64_t         time, 
	      uint64_t         timeCentroid, 
	      uint64_t         interval,
	      uint64_t         exposure,
	      const float*             floatData, 
	      const uint32_t* dataFlags); // TODO

    /** A constructor to be used in more general case of total power data
	all the data being valid.
    */
    DataDump( std::vector<std::vector<uint32_t> >       vv_numPolProduct,     // /bb/spw
	      std::vector<std::vector<uint32_t> >       vv_numSpectralPoint,  // /bb/spw
	      std::vector<std::vector<uint32_t> >       vv_numBin,            // /bb/spw
	      std::vector<std::vector<Enum<NetSidebandMod::NetSideband> > > vv_e_sideband,        // /bb/spw
	      uint32_t                        numApc,
	      std::vector<uint32_t>                v_numSpectralWindow,  // /bb
	      uint32_t                        numBaseband,
	      uint32_t                        numAnt,
	      CorrelationModeMod::CorrelationMode                     correlationMode,
	      uint64_t                           time, 
	      uint64_t                           timeCentroid, 
	      uint64_t                           interval, 
	      uint64_t                           exposure,
	      const float*                        floatData);

    /** A constructor to be used in a more general case of total power data
	when some data have been flagged
    */
    DataDump( std::vector<std::vector<uint32_t> >       vv_numPolProduct,     // /bb/spw
	      std::vector<std::vector<uint32_t> >       vv_numSpectralPoint,  // /bb/spw
	      std::vector<std::vector<uint32_t> >       vv_numBin,            // /bb/spw
	      std::vector<std::vector<Enum<NetSidebandMod::NetSideband> > > vv_e_sideband,        // /bb/spw
	      uint32_t                        numApc,
	      std::vector<uint32_t>                v_numSpectralWindow,  // /bb
	      uint32_t                        numBaseband,
	      uint32_t                        numAnt,
	      CorrelationModeMod::CorrelationMode                     correlationMode,
	      uint64_t                           time,
	      uint64_t                           timeCentroid,  
	      uint64_t                           interval, 
	      uint64_t                           exposure,
	      const float*                        floatData, 
	      const uint32_t*                 dataFlags);

    /** A constructor with no importation nor attachment of binary meta.
    */
    DataDump( std::vector<std::vector<uint32_t> >       vv_numCrossPolProduct,// /bb/spw
	      std::vector<std::vector<uint32_t> >       vv_numAutoPolProduct, // /bb/spw
	      std::vector<std::vector<uint32_t> >       vv_numSpectralPoint,  // /bb/spw
	      std::vector<std::vector<uint32_t> >       vv_numBin,            // /bb/spw
	      std::vector<std::vector<Enum<NetSidebandMod::NetSideband> > > vv_e_sideband,        // /bb/spw
	      uint32_t                        numApc,
	      std::vector<uint32_t>                v_numSpectralWindow,  // /bb
	      uint32_t                        numBaseband,
	      uint32_t                        numAnt,
	      CorrelationModeMod::CorrelationMode                     correlationMode,
	      uint64_t                           time,
	      uint64_t                           timeCentroid,  
	      uint64_t                           interval, 
	      uint64_t                           exposure);

    /** Destructor 

    The pointer to the floatData being one of the parameters in the constructor,
    this DataDump class does not have the privilege to delete the allocated
    memory for these floatData; these are under the responsability of the client.
    More generally, any data attached by the client will never be deleted by this
    destructor.<br>
    However, if floatData has been created by a method in this class, e.g. when 
    using the operator + then the destructor delete this allocated memory. More
    generally, any data imported by the client will be deleted by this destructor. 

    List of methods which create DataDump objects by allocating memory to floatData:
    - copy constructor
    - assignement operator =
    - operators -, -, * and /

    */
    ~DataDump();

    /** Copy constructor 
	Perform a deep copy
     */
    DataDump(const DataDump& a);

    /** Assignement operatator */
    DataDump & operator = (const DataDump& a);

    void attachFlags( uint32_t declaredSize, EnumSet<AxisNameMod::AxisName> es_an, 
		      uint32_t numData, const uint32_t* flagsPtr);
    void importFlags( uint32_t declaredSize, EnumSet<AxisNameMod::AxisName> es_an, 
		      uint32_t numData, const uint32_t* flagsPtr);

    void attachActualTimes( uint32_t declaredSize, EnumSet<AxisNameMod::AxisName> es_an, 
			    uint32_t numData, const int64_t * actualTimesPtr);

    void importActualTimes( uint32_t declaredSize, EnumSet<AxisNameMod::AxisName> es_an, 
			    uint32_t numData, const  int64_t * actualTimesPtr);

    void attachActualDurations( uint32_t declaredSize, EnumSet<AxisNameMod::AxisName> es_an, 
				uint32_t numData, const int64_t * durationsPtr);

    void importActualDurations( uint32_t declaredSize, EnumSet<AxisNameMod::AxisName> es_an, 
				uint32_t numData, const int64_t * durationsPtr);


    void attachZeroLags ( uint32_t declaredSize, EnumSet<AxisNameMod::AxisName> es_an, 
			  uint32_t numData, const float* zeroLagsPtr);

    void importZeroLags ( uint32_t declaredSize, EnumSet<AxisNameMod::AxisName> es_an, 
			  uint32_t numData, const float* zeroLagsPtr);


    void attachAutoData ( uint32_t declaredSize, EnumSet<AxisNameMod::AxisName> es_an, 
			  uint32_t numData, const float * autoDataPtr);

    void importAutoData ( uint32_t declaredSize, EnumSet<AxisNameMod::AxisName> es_an, 
			  uint32_t numData, const float * autoDataPtr);

    void attachCrossData ( uint32_t declaredSize, EnumSet<AxisNameMod::AxisName> es_an, 
			   uint32_t numData, const short int* crossDataPtr);
    void importCrossData ( uint32_t declaredSize, EnumSet<AxisNameMod::AxisName> es_an, 
			   uint32_t numData, const short int* crossDataPtr);
    void attachCrossData ( uint32_t declaredSize, EnumSet<AxisNameMod::AxisName> es_an, 
			   uint32_t numData, const int * crossDataPtr);
    void importCrossData ( uint32_t declaredSize, EnumSet<AxisNameMod::AxisName> es_an, 
			   uint32_t numData, const int * crossDataPtr);
    void attachCrossData ( uint32_t declaredSize, EnumSet<AxisNameMod::AxisName> es_an, 
			   uint32_t numData, const float * crossDataPtr);
    void importCrossData ( uint32_t declaredSize, EnumSet<AxisNameMod::AxisName> es_an, 
			   uint32_t numData, const float * crossDataPtr);

    void         setScaleFactor(std::vector<std::vector<float> > vv_scaleFactor);
    uint32_t setIntegration(uint32_t integNum);
    uint32_t setSubintegration(uint32_t integNum, uint32_t subintegNum);
    uint32_t setContextUsingProjectPath(std::string projectPathUri);

    // Accessor
    uint32_t integrationNum();
    uint32_t subintegrationNum();

    float scaleFactor(uint32_t nbb, uint32_t nspw);

    float scaleFactor(uint32_t ndd);

    /** @name TimeMetadataAccessors
     * Accessors on time-related attributes 
     */
    //@{
    uint64_t      time() const;
    uint64_t      timeCentroid() const;
    uint64_t      interval() const;
    uint64_t      exposure() const;
    //@}

    /** @name DataAccessors 
     * Accessors to the actual time values
     */
    //@{
    const int64_t* actualTimes() const;
    const int64_t* actualDurations() const;
    /** Accessor to the flags values. Methods which can be used in case
	of multi-dimensional data structure. 
	@return pointer to the multidimensional array of flags.
    */
    const uint32_t*      flags()    const;
    const float*             zeroLags() const;


    /** Accessor to the data values. Methods which can be used in case
	of multi-dimensional data structure. Example of use-cases:
        - data produced by the baseband processors
        - data produced by one or more filter bank or an acousto-optic 
          spectrometer all with the same number of channels.
        - data produced by the water vapor radiometers
        - ...

	@return pointer to the multi-dimensional array.

    */
    const float*             autoData() const;

    const short*             crossDataShort() const;

    const int*              crossDataLong() const;

    const float*             crossDataFloat() const;

    /** Accessor to the data from a 2 levels tree-hierarchy.
	@note A multi-dimensional structure being a special case
              of a tree structure, this method can also be used
              for 2D multi-dimensional data structures! Example
              of a use-case: 
              - data produced by the sub-band processor with one
                single-dish when there is more than one baseband.
              - ...

    */
    uint32_t floatData(std::vector<std::vector<float>& >);


    /** Accessor to the data from a 3 levels tree-hierarchy with 
        muti-dimensional leaves.
	@return dim the dimensionality of the leaves. The reference to 
                a tree where the leaves are pointers
                to a multi-dimansional structure. 
	@note In general, within a spectral window, the number of
              polarization products is the same for every spectral point.
              This being true also for the bin axis, the leaves may 
              correspond to 2, 3 or more exceptionaly (usualy
              total powers have no apc axis) 4D multi-dimensional 
              structures. Example of use-cases:
              - one single-dish with multiple basebands each with
              their own set of specific spectral windows.
              - ...

    */
    uint32_t floatData(std::vector<std::vector<float*> >&);


    /** Accessor to the data from a 3 levels tree-hierarchy.
	@note A multi-dimensional structure being a special case
              of a tree structure, this method can also be used
              for 3D multi-dimensional data structures!
    */
    uint32_t floatData(std::vector<std::vector<std::vector<float> > >&);


    /** Accessor to the data from a 3 levels tree-hierarchy with 
        muti-dimensional leaves.
	@return dim the dimensionality of the leaves. The reference to 
                a tree where the leaves are pointers
                to a multi-dimansional structure. 
	@note In general, within a spectral window, the number of
              polarization products is the same for every spectral point.
              This being true also for the apc axis, the leaves may 
              correspond to 2 to 4D multi-dimensional structures.

	      Example of use-cases:
              - data produced by the 4 ACA single-dishes with multiple 
              correlator basebands each with their own set of specific 
              spectral windows.
              - ...

    */
    uint32_t floatData(std::vector<std::vector<std::vector<float*> > >&);


    /** Accessor to the data from a 4 levels tree-hierarchy.
	@note A multi-dimensional structure being a special case
              of a tree structure, this method can also be used
              for 4D multi-dimensional data structures!

	      Example of use-cases:
              - data produced by a set of antenna elements with multiple 
              correlator basebands each with their own set of specific 
              spectral windows when there is no bin axis (no switching 
              cycles). in that case the vector returned by reference
              has it size corresponding to the number of antenna
              elements
              - ...

    */
    uint32_t floatData(std::vector<std::vector<std::vector<std::vector<float> > > >&);



    /** Accessor to the data from a 4 levels tree-hierarchy.
	@note A multi-dimensional structure being a special case
              of a tree structure, this method can also be used
              for 4D multi-dimensional data structures!
    */
    uint32_t floatData(std::vector<std::vector<std::vector<std::vector<float*> > > >&);

    /** Accessor to the data from a 5 levels tree-hierarchy.
	@return the data tree by reference
	@note this is the general case, the five levels in the
              tree structure being an, bb, spw, bin, sp
              Hence the size of this returned vector corresponds
              to the number of antennas.
    */
    uint32_t floatData(std::vector<std::vector<std::vector<std::vector<std::vector<float> > > > >&);

    // /nant/nbb/nspw/nbin/napc/nsp/npol
    // |    |   |    |    |    |   | 
    // |    |   |    |    |    | corr or uncorr (enum)
    // |    |   |    |    | scId (global keys)
    // |    |   |    | spwNames (keys in baseband context)
    // |    |   | basebandNames (global keys)
    // |    | antNames (global keys)
    // | configDescId (global key)

    //@}

    /** @name DataManipulation1 
     * Arithmetics operators (these are axis-size invariant manipulations).
     */
    //@{
    /** Operator to take the difference between two data dumps.

        Let C be a data dump difference of two data dumps A and B.
	The result attributes of  \f$ C = A - B  \f$ are the following:
        - floatData d: 
            \f$  d_{C}(x,y,...) =  d_{A}(x,y,...) - d_{B}(x,y,...) \f$
        - start time:
            \f$ ^st_C = MIN(^st_A,^st_B) = MIN( t_A-i_A/2 , t_B-i_B/2 ) \f$
        - end time:
	    \f$ ^et_C = MAX(^et_A,^et_B) = MAX( t_A-i_A/2 , t_B-i_B/2 ) \f$
        - interval i:
            \f$ i_C = 0.5~\big(~^et_C~- ~^st_C \big) \f$
        - time t:
            \f$ t_C = 0.5~\big(~^et_C~+ ~^st_C \big) \f$
        - time centroid:
            \f$ ^ct_C = \big(~e_A~^ct_A~+~e_B~^ct_B\big)~/~\big(~e_A~+~e_B~\big) \f$ 
        - exposure:
            \f$ e_C = 0.5~\big(~e_A~+~e_B\big) \f$ 
        - flags:
            \f$ f_C(x,y,...) = 0 \f$ if \f$ f_A(x,y,...) = f_B(x,y,...) = 0 \f$  else ...
 
	Example of use-case: compute the difference to subtract a reference
        data dump to a non-reference data dump.
    */
    DataDump            operator - (const DataDump & B);  // shape unchanged

    /** Operator to take the difference between two data dumps.

	The result attributes of  A -= B  are the following:

        - floatData d: 
            \f$  d(x,y,...) =  d(x,y,...) - d_{B}(x,y,...) \f$
        - start time:
            \f$ ^st = MIN(^st,^st_B) = MIN( t-i/2 , t_B-i_B/2 ) \f$
        - end time:
	    \f$ ^et = MAX(^et,^et_B) = MAX( t-i/2 , t_B-i_B/2 ) \f$
        - interval i:
            \f$ i = 0.5~\big(~^et -~^st_C~\big) \f$
        - time t:
            \f$ t =~^st~+~i/2 \f$
        - time centroid:
            \f$ ^ct = \big(e~^ct~+~e_B~^ct_B \big)~/~\big(~e~+~e_B~\big) \f$ 
        - exposure:
            \f$ e = 0.5~\big(e~+~e_B\big) \f$ 
        - flags:
            \f$ f(x,y,...) = 0 \f$ if \f$ f(x,y,...) = f_B(x,y,...) = 0 \f$  else ...
 
	Example of use-case: compute the difference to subtract a reference
        data dump to a non-reference data dump.
     */
    DataDump           operator -= (const DataDump &);  // shape unchanged

    /** Operator to take the sum between two data dumps.

        Let C be a data dump "sum" of data dumps A and B. Effectively this
        "sum" is a weighted mean, the weights being equal to the values
        of the intervals for A and B. 
        
	The result attributes of  \f$ C = A + B  \f$ are the following:
        - floatData d: 
            \f$  d_C(x,y,...) =  \big(i_A d_A(x,y,...) + i_B d_B(x,y,...)\big)~/~\big(i_A+i_B\big) \f$
        - start time:
            \f$ ^st_C = MIN(^st_A,^st_B) = MIN( t_A-i_A/2 , t_B-i_B/2 ) \f$
        - end time:
	    \f$ ^et_C = MAX(^et_A,^et_B) = MAX( t_A-i_A/2 , t_B-i_B/2 ) \f$
        - interval i:
            \f$ i_C = ~^et_C~- ~^st_C \f$
        - time t:
            \f$ t_C =~ 0.5~\big(~^et_C~+~^st_C~ \big) \f$
        - time centroid:
            \f$ ^ct_C = \big(~e_A~^ct_A~+~e_B~^ct_B~\big)~/~\big(~e_A~+~e_B~\big) \f$ 
        - exposure:
            \f$ e_C =~e_A~+~e_B \f$ 

	Example of use-case: sum data dumps to create a new dump with increased
                             signal-to-noise ratio.

        @note To take the exposure for the weights use the operator +=. 
    */
    DataDump            operator + (const DataDump &);  // shape unchanged


    /** Method to accumulate dumps.

    Let A the instance of this data dump and B an other data dump. This method
    "accumulates" to this A the data dump B. Effectively this accumulation
    corresponds to a weigthed mean, the weigths being the exposures values.
    To use the interval values for the wieghts, use the operator +.

    The result attributes of the accumulation A += B are the following:

        - floatData d: 
            \f$  d(x,y,...) =  \big(e d(x,y,...) + i_B d_B(x,y,...)\big)~/~\big(~e~+~e_B~\big) \f$
        - start time:
            \f$ ^st = MIN(^st,^st_B) = MIN( t-i/2 , t_B-i_B/2 ) \f$
        - end time:
	    \f$ ^et = MAX(^et,^et_B) = MAX( t-i/2 , t_B-i_B/2 ) \f$
        - interval i:
            \f$ i =~^et~-~^st \f$
        - time t:
            \f$ t =~^st~+~i/2 ) \f$
        - time centroid:
            \f$ ^ct = \big(e~^ct~+~e_B~^ct_B~\big)~/~\big(e+e_B\big) \f$ 
        - exposure:
            \f$ e = ~e~+~e_B \f$ 

	Example of use-case: sum data dumps to create a new dump with increased
                             signal-to-noise ratio.
 
    */ 
    DataDump      operator += (const DataDump &);

    DataDump      operator *  (const float &);
    DataDump      operator /  (const float &);
/*     DataDump      operator *  (const DataProfile &); */
/*     DataDump      operator /  (const DataProfile &); */
/*     DataProfile   operator *= (const DataProfile &); */
/*     Dataprofile   operator /= (const DataProfile &); */



    //@}

    /** @name DataManipulation2
     * Manipulations which produce output objects with axis-sizes different to those of the input objects.
     *
     */
    //@{
    DataDump      spectralAverage();
    DataDump      spectralAverage(uint32_t startIndex, uint32_t endIndex);
    DataDump            decim(const DataDump &);
    DataDump      subtractRef(const DataDump &);
    //@}


    //DataDump     binSequence(vector<const DataDump &>); see Subscan

  protected:

    uint32_t              integrationNum_;
    uint32_t              subintegrationNum_;
    uint64_t                 time_;
    uint64_t                 timeCentroid_; 
    uint64_t                 interval_;
    uint64_t                 exposure_;

    EnumSet<AxisNameMod::AxisName>         es_flagsAxes_;
    EnumSet<AxisNameMod::AxisName>         es_actualTimesAxes_;
    EnumSet<AxisNameMod::AxisName>         es_actualDurationsAxes_;
    EnumSet<AxisNameMod::AxisName>         es_weightsAxes_;
    EnumSet<AxisNameMod::AxisName>         es_zeroLagsAxes_;
    EnumSet<AxisNameMod::AxisName>         es_autoDataAxes_;
    EnumSet<AxisNameMod::AxisName>         es_crossDataAxes_;

    uint32_t              numFlags_;
    uint32_t              numActualTimes_;
    uint32_t              numActualDurations_;
    uint32_t              numWeights_;
    uint32_t              numZeroLags_;
    uint32_t              numAutoData_;
    uint32_t              numCrossData_;

    std::vector<std::vector<float> >    vv_scaleFactor_;     // /bb/spw
    std::vector<float>             v_scaleFactor_;      // /ndd      linearized version of vv_scaleFactor_ for fast access

    const uint32_t*     cuintFlagsPtr_;
    const int64_t*      clonlonActualTimesPtr_;
    const int64_t*      clonlonActualDurationsPtr_;
    const float*        cfloatWeightsPtr_;
    const float*        cfloatZeroLagsPtr_;
    const float*        cfloatAutoDataPtr_;
    const short int*    cshortCrossDataPtr_;
    const int*          cintCrossDataPtr_;
    const float*        cfloatCrossDataPtr_;

    uint32_t*   uintFlagsPtr_;
    int64_t*    lonlonActualTimesPtr_;
    int64_t*    lonlonActualDurationsPtr_;
    float*      floatWeightsPtr_;
    float*      floatZeroLagsPtr_;
    float*      floatAutoDataPtr_;
    short int*  shortCrossDataPtr_;
    int*        intCrossDataPtr_;
    float*      floatCrossDataPtr_;

    /** A constructor used by the operators + and - */
    DataDump( std::vector<std::vector<uint32_t> >       vv_numCrossPolProduct,// /bb/spw
	      std::vector<std::vector<uint32_t> >       vv_numAutoPolProduct, // /bb/spw
	      std::vector<std::vector<uint32_t> >       vv_numSpectralPoint,  // /bb/spw
	      std::vector<std::vector<uint32_t> >       vv_numBin,            // /bb/spw
	      std::vector<std::vector<Enum<NetSidebandMod::NetSideband> > > vv_e_sideband,        // /bb/spw
	      uint32_t                        numApc,
	      std::vector<uint32_t>                v_numSpectralWindow,  // /bb
	      uint32_t                        numBaseband,
	      uint32_t                        numAnt,
	      CorrelationModeMod::CorrelationMode                     correlationMode,
	      uint64_t                           time,
	      uint64_t                           timeCentroid,  
	      uint64_t                           interval, 
	      uint64_t                           exposure,
	      uint32_t                    numCrossData,
	      const int*                      crossData,
	      uint32_t                    numAutoData,
	      const float*                    floatData,
	      uint32_t                    numFlags,
	      const uint32_t*            dataFlags);

  private:

    void                          getDataStructure();
  };


  /** The class Integration extends the DataDump class by adding an attribute to tag 
      the dump with an integration number. Integration numbers are one-based, a 
      value of 0 meaning undefined. Note that integrations object have always an 
      subintegration number which is set undefined. */
  class Integration : public DataDump
  {
  public:
    Integration();
    Integration( std::vector<std::vector<uint32_t> >       vv_numPolProduct,     // /bb/spw
		 std::vector<std::vector<uint32_t> >       vv_numSpectralPoint,  // /bb/spw
		 std::vector<std::vector<uint32_t> >       vv_numBin,            // /bb/spw
		 std::vector<std::vector<Enum<NetSidebandMod::NetSideband> > > vv_e_sideband,        // /bb/spw
		 uint32_t                        numApc,
		 std::vector<uint32_t>                v_numSpectralWindow,  // /bb
		 uint32_t                        numBaseband,
		 uint32_t                        numAnt,
		 CorrelationModeMod::CorrelationMode                     correlationMode,
		 uint64_t                           time,
		 uint64_t                           timeCentroid,  
		 uint64_t                           interval, 
		 uint64_t                           exposure,
		 float*                              floatData, 
		 uint32_t                        integNum);


    Integration( std::vector<std::vector<uint32_t> >       vv_numPolProduct,     // /bb/spw
		 std::vector<std::vector<uint32_t> >       vv_numSpectralPoint,  // /bb/spw
		 std::vector<std::vector<uint32_t> >       vv_numBin,            // /bb/spw
		 std::vector<std::vector<Enum<NetSidebandMod::NetSideband> > > vv_e_sideband,        // /bb/spw
		 uint32_t                        numApc,
		 std::vector<uint32_t>                v_numSpectralWindow,  // /bb
		 uint32_t                        numBaseband,
		 uint32_t                        numAnt,
		 CorrelationModeMod::CorrelationMode                     correlationMode,
		 uint64_t                           time, 
		 uint64_t                           timeCentroid, 
		 uint64_t                           interval,
		 uint64_t                           exposure, 
		 float*                              floatData, 
		 uint32_t*                       dataFlags,
		 uint32_t                        integNum);

    Integration( uint32_t      numPolProduct,
		 uint32_t      numSpectralPoint,
		 uint32_t      numBin,
		 Enum<NetSidebandMod::NetSideband> e_sideband,
		 uint32_t      numApc,
		 uint32_t      numSpectralWindow,
		 uint32_t      numBaseband,
		 uint32_t      numAnt, 
		 CorrelationModeMod::CorrelationMode   correlationMode,
		 uint64_t         time, 
		 uint64_t         timeCentroid,
		 uint64_t         interval,
		 uint64_t         exposure, 
		 float*            floatData);

    Integration( uint32_t      numPolProduct,
		 uint32_t      numSpectralPoint,
		 uint32_t      numBin,
		 Enum<NetSidebandMod::NetSideband> e_sideband,
		 uint32_t      numApc,
		 uint32_t      numSpectralWindow,
		 uint32_t      numBaseband,
		 uint32_t      numAnt,
		 CorrelationModeMod::CorrelationMode   correlationMode,
		 uint64_t         time, 
		 uint64_t         timeCentroid, 
		 uint64_t         interval,
		 uint64_t         exposure, 
		 float*            floatData, 
		 uint32_t*   dataFlags,  // TODO
		 uint32_t      integNum);
    ~Integration();

    Integration( const Integration & a);

  
  private:
/*     uint32_t integrationNum_; */
/*     uint32_t subintegrationNum_; */
  };

/*   uint32_t Integration::subintegrationNum_=0; */

  /** The class  Subintegration extends the  DataDump by adding two attributes
      the integration number and the subintegration number. Subintegration objects
      must have these two numbers with defined vaues (i.e. they must be greater 
      than zero).
  */
  class Subintegration : public DataDump
  {
  public:

    /** Constructor to generate a subintegration from an integration by
	averaging, in every spectral window, all the spectral points.
	To this resulting spectral-averaged dataset is assigned a 
	subintegration number.
	This constructor is intended in case the duration of the integration
	is shorter than the timescale for the atmospheric phase fluctautions,
	i.e. shorter that ~0.5s.
    */   
    Subintegration (const Integration&, uint32_t subintegNum );

    /** The constructor for a subintegration which should be the most commonly
	used for ALMA, the number of spectral points in each spectral window
	being reduced to 1 and the data having not been processed with the WV
	radiometric corrections.
	Case when no data flagged.
    */
    Subintegration( std::vector<std::vector<uint32_t> >       vv_numPolProduct,     // /bb/spw
		    std::vector<std::vector<uint32_t> >       vv_numBin,            // /bb/spw
		    std::vector<std::vector<Enum<NetSidebandMod::NetSideband> > > vv_e_sideband,        // /bb/spw
		    std::vector<uint32_t>                v_numSpectralWindow,  // /bb
		    uint32_t                        numBaseband,
		    uint32_t                        numAnt,
		    CorrelationModeMod::CorrelationMode                     correlationMode, 
		    uint64_t                           time,
		    uint64_t                           timeCentroid, 
		    uint64_t                           interval,
		    uint64_t                           exposure,
		    float*                              floatData,
		    uint32_t                        integNum,
		    uint32_t                        subintegNum);

    /** The constructor for a subintegration which should be the most commonly
	used for ALMA, the number of spectral points in each spectral window
	being reduced to 1 and the data having not been processed with the WV
	radiometric corrections.
	Case when some data may have been flagged.
    */
    Subintegration( std::vector<std::vector<uint32_t> >       vv_numPolProduct,     // /bb/spw
		    std::vector<std::vector<uint32_t> >       vv_numBin,            // /bb/spw
		    std::vector<std::vector<Enum<NetSidebandMod::NetSideband> > > vv_e_sideband,        // /bb/spw
		    std::vector<uint32_t>                v_numSpectralWindow,  // /bb
		    uint32_t                        numBaseband,
		    uint32_t                        numAnt,
		    CorrelationModeMod::CorrelationMode                     correlationMode, 
		    uint64_t                           time, 
		    uint64_t                           timeCentroid, 
		    uint64_t                           interval,
		    uint64_t                           exposure, 
		    float*                              floatData,
		    uint32_t*                 dataFlags,  // TODO
		    uint32_t                        integNum,
		    uint32_t                        subintegNum);

    /** A general constructor for subintegrations with all the data unflagged
	In general the subintegrations are not processed by the WVC radiometric
	corrections because the temporal sampling should be short enough to
	track the atm. phase fluctuations. 
	This constructor is intended in case the data would have been processed by 
	the WV radiometric corrections.
    */
    Subintegration( std::vector<std::vector<uint32_t> >       vv_numPolProduct,     // /bb/spw
		    std::vector<std::vector<uint32_t> >       vv_numBin,            // /bb/spw
		    std::vector<std::vector<Enum<NetSidebandMod::NetSideband> > > vv_e_sideband,        // /bb/spw
		    uint32_t                        numApc,
		    std::vector<uint32_t>                v_numSpectralWindow,  // /bb
		    uint32_t                        numBaseband,
		    uint32_t                        numAnt,
		    CorrelationModeMod::CorrelationMode                     correlationMode, 
		    uint64_t                           time,
		    uint64_t                           timeCentroid, 
		    uint64_t                           interval,
		    uint64_t                           exposure,
		    float*                              floatData,
		    uint32_t                        integNum,
		    uint32_t                        subintegNum);

    /** An even more general constructor. Using this constructor, the only difference
	between an integration and a subintegration is that the subintegration has
	a defined subintegration number.
	To be used when ther is no data flagged
    */
    Subintegration( std::vector<std::vector<uint32_t> >       vv_numPolProduct,     // /bb/spw
		    std::vector<std::vector<uint32_t> >       vv_numSpectralPoint,  // /bb/spw
		    std::vector<std::vector<uint32_t> >       vv_numBin,            // /bb/spw
		    std::vector<std::vector<Enum<NetSidebandMod::NetSideband> > > vv_e_sideband,        // /bb/spw
		    uint32_t                        numApc,
		    std::vector<uint32_t>                v_numSpectralWindow,  // /bb
		    uint32_t    numBaseband,
		    uint32_t    numAnt, 
		    CorrelationModeMod::CorrelationMode correlationMode,
		    uint64_t       time, 
		    uint64_t       timeCentroid, 
		    uint64_t       interval,
		    uint64_t       exposure, 
		    float*          floatData,
		    uint32_t    integNum,
		    uint32_t    subintegNum);

    /** The most general constructor. Using this constructor, the only difference
	between an integration and a subintegration is that the subintegration has
	a defined subintegration number.
	To be used if there are some flagged data
    */
    Subintegration( std::vector<std::vector<uint32_t> >       vv_numPolProduct,     // /bb/spw
		    std::vector<std::vector<uint32_t> >       vv_numSpectralPoint,  // /bb/spw
		    std::vector<std::vector<uint32_t> >       vv_numBin,            // /bb/spw
		    std::vector<std::vector<Enum<NetSidebandMod::NetSideband> > > vv_e_sideband,        // /bb/spw
		    uint32_t                        numApc,
		    std::vector<uint32_t>                v_numSpectralWindow,  // /bb
		    uint32_t    numBaseband,
		    uint32_t    numAnt,
		    CorrelationModeMod::CorrelationMode correlationMode,
		    uint64_t       time, 
		    uint64_t       timeCentroid, 
		    uint64_t       interval,
		    uint64_t       exposure, 
		    float*          floatData, 
		    uint32_t*   dataFlags, // TODO
		    uint32_t    integNum,
		    uint32_t    subintegNum);

  private:

/*     uint32_t integrationNum_; */
/*     uint32_t subintegrationNum_; */

  };

}

#define _INTEGRATION_H 
#endif
