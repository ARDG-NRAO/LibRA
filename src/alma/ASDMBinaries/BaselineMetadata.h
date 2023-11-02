#if     !defined(_BASELINEMETADATA_H)
#include <alma/ASDMBinaries/Error.h>

namespace sdmbin {

  /** Class for baseline-based metadata objects, one type of component in SDM data objects.\n
      There are three baseline-based  metadata in these objects, the flags, the actual times (the mid-point)
      and the actual durations. Sharing the same type of tree structure they derive from this BaselineMetadata 
      virtual class, the common base.\n
      An example of how to \ref testbaselinemetadata is given in a test program which uses methods 
      defined in this base but also other methods which are extensions defined in the BaselineFlagsMetada 
      and the ActualTimeDurations classes.
   */
  class BaselineMetadata
  { 
  public:
   
    /** Total (cross+auto) number of metadata values 
     * @returns The sum of the cross and auto baseline-based metadata values
     * @note
     * The size is determined in the method sizes() from the following formula:\n
     * \f$
         N_{bl}~ \sum_{i=1}^{N_{bb}} N_{bin}(i)~N_{pp}^{oo}(i) + 
         N_{ant}~\sum_{i=1}^{N_{bb}} N_{bin}(i)~N_{pp}^{o}(i) 
     * \f$\n
     * and the peculiar non-standard mode ({\it correlationMode}=0) by:\n
     * \f$
         N_{bl}~ \sum_{i=1}^{N_{bb}} N_{bin}(i)~N_{pp}^{oo}(i) + 
         N_{ant}~\sum_{i=1}^{N_{bb}} N_{bin}(i)~f(N_{pp})(i)~  
     * \f$\n
     * with in this specific case \n
     * \f$
        f(N_{pp}(i)) = \left\| \begin{array}
        {r@{\quad if\quad}l}
        1 & N_{pp}^{oo}(i)=1\\
        2 & N_{pp}^{oo}(i)>1\\
        \end{array} \right.
     * \f$\n
     * In this abstract class \f$N_{ant}\f$ must be considered as generic. Its actual value 
     * depends of the context in the derived classes.
     * @note The size of the containers for the baseline-based metadata may also be obtained,
     * in number of bytes, with methods of the asdmBinary::BinaryHeaderElement class. 
     */
    int                  numValue();

    /** Accessor to a generic number of antennas \f$ N_{ant} \f$
     * Specifities are defined in the derived classes 
     */
    virtual int          numAnt()=0;

    /** Accessor to an actual number of antennas \f$ N'_{ant} \f$
     * Specifities are defined in the derived classes
     */
    virtual int          actualNumAnt()=0;

    /** Accessor to the number of baselines
     * @return the number of baselines \f$ N_{bl} \f$
     */    
    int                  numBaseline();

    /** Accessor to the number of basebands
     * @return The number of basebands  \f$ N_{bb} \f$
     */
    int                  numBaseband();

    /** Accessor to the number of polarization products of a given baseband
     * @param basebandNum Baseband number (one-based)
     * @return The number of polarization products ( \f$ N_{pp}^{oo} \f$
     * @exception Error the method returns -1 in lax mode 
     * @note Use the method numAutoPolProduct(int) to get the antenna-based number of product
     * \f$ N_{pp}^{o}\f$ if correlationMode=1. 
     */
    int                  numPolProduct(int basebandNum) throw (Error);

    /** Accessor to the number of polarization products.
     * The constraint is that all spectral windows in a baseband are associated with a common 
     * polarization identifier. 
     * @return a vector of numbers of products, one number per baseband.
     * @note Use the method numAutoPolProduct() to get the antenna-based number of products
     * if correlationMode=1. 
     */
    std::vector<int>          numPolProduct();

    /** Accessor to the number of bins (steps or phases in scwitching cycles) for a given baseband
     * @param basebandNum The basbabnd number (one-based) 
     * @return The number of bins
     * @exception Error the method returns -1 in lax mode 
     */  
    int                  numBin(int basebandNum) throw (Error);

    /** Accessor to the number of bins (steps or phases in scwitching cycles) for each baseband
     * @return A vector of number of bins, one number per baseband.
     */  
    std::vector<int>          numBin();

    /** Accessor to the correlation mode
     * @return The correlation mode:
     * - 0 means that the data contains only interferometric data. This does not imply that the baseline-based
     *     metadata are exclusively for the non-zero baselines! See the documentation of the sizes() method.
     * - 1 means that the data are exclusively for the zero baselines. In this case the baseline-based metadata
     *     are also exclusively for the zero baselines.
     * - 2 means that both the non-zero and zero baselines are present.
     */
    int                  correlationMode();

    /** Accessor to the (derived) number of polarization products for the zero baselines for a given baseband
     * @param basebandNum The baseband number (one-based) 
     * @return The number of products for that baseband number
     * @exception Error the method returns -1 in lax mode 
     */  
    int                  numAutoPolProduct(int basebandNum) throw (Error);

    /** Accessor to the (derived) number of polarization products for the zero baselines for all the basebands
     * @return The number of products for each of the basebands in the configuration
     */  
    std::vector<int>          numAutoPolProduct();


    /** Method to get the list of the antenna indices restricted to those
     * which are not flagged above a certain critical limit in all baseband
     * @param valLimit critical flagging limit
     * @return The list of antenna indices fulfilling the criterion 
     */
    virtual std::vector<int>  antennaIndexUsed(unsigned long int valLimit)=0;

    /** Baseline number (one-based) given a pair of antenna indices
     * @param anti index (zero-based) for antenna i
     * @param antj index (zero-based) for antenna i
     * @return The baseline number
     * @exception Error the indices anti and/or antj exceed the limit for the number of baselines. 
     */
    int                  baselineNum(int anti, int antj) throw(Error);

    /** Index in a container of baseline-based metadata given a node specified by a full set of indices of the parameter space
     *  @param npp  polarization product index
     *  @param nbin bin index (always 0 when there is no switching mode used)
     *  @param nbb  baseband index
     *  @param i    antenna index i
     *  @param j    antenna index j (if i=j this means a zero-baseline)
     *  @return the index in the container of baseline-based metadata for this node.
     */
    int                           at(int npp, int nbin, int nbb, int i, int j);

    /** Set of indices, one per polarization product, in a container of baseline-based metadata given a set of nodes 
     *  specified by a subset of indices of the parameter space
     *  @param npp  polarization product index
     *  @param nbb  baseband index
     *  @param i    antenna index i
     *  @param j    antenna index j (if i=j this means a zero-baseline)
     *  @return A set of indices, one per bin, defining locations in the container of 
     *          baseline-based metadata for this set of nodes.
     */
    std::vector<int>                   at(         int npp, int nbb, int i, int j);  // numPolProduct elements

    /** Set of indices, one per non-zero basleine, in a container of baseline-based metadata given a set of nodes 
     *  specified by a subset of indices of the parameter space
     *  @param npp  polarization product index
     *  @param nbin bin index (always 0 when there is no switching mode used)
     *  @param nbb  baseband index
     *  @return A set of indices, one per non-zero baseline, defining locations in the container of 
     *          baseline-based metadata for this set of nodes.
     */
    std::vector<int>                   at(int npp, int nbin, int nbb              );  // numNonZeroBaseline elements         

    /** Set of indices in a container of baseline-based metadata given a set of nodes node specified by a subset 
     *  of indices of the parameter space
     *  @param nbin bin index (always 0 when there is no switching mode used)
     *  @param nbb  baseband index
     *  @return a two levels tree of indices (a set of subsets of indices)
     */
    std::vector<std::vector<int> >          at(         int nbin, int nbb              );  // numNonZeroBaseline vectors of numPolProduct elem.

    /** Set of indices in a container of baseline-based metadata given a set of nodes node specified by a subset 
     *  of indices of the parameter space
     *  @param cross  true to select the indices for the non-zero baselines, else false for the zero baselines
     *  @param i      antenna index i to select all baselines involving this antenna of index i
     *  @return a three levels tree (polarization product, baseband, baseline) of indices
     */
    std::vector<std::vector<std::vector<int> > > at(bool cross, int i                       );  // numBaseline vectors of numBasebands vectors of numPP elem. 

    static int           FLAGLIMIT;

  protected:
    /** Default constructor */
    BaselineMetadata();
    /** Virtual destructor */
    virtual ~BaselineMetadata();


    /** Method to instantiate the base of the hierarchy
     * @param v_npp           Number of polarization product for every baseband
     * @param v_nbin          Number of bins (phases in a switch cycle) for every baseband
     * @param nbb             Number of baseband (should be the size of v_npp and v_nbin)
     * @param nant            Number of antennas (\f$ N_{ant} \f$)
     * @param correlationMode The correlation mode (must be 0 or 1 or 2).
     */
    void mkBaselineMetadata( std::vector<int> v_npp, std::vector<int> v_nbin, int nbb, int nant, int correlationMode);


    std::vector<int> v_numPolProduct_;     //!< Number of cross polarization products for every baseband
    std::vector<int> v_numBin_;            //!< Number of phases in SwitchCycles for every baseband
    int         numBaseband_;         //!< Number of basebands
    int         numAnt_;              //!< Number of antennas
    int         numBaseline_;         //!< Number of baselines
  
    int         autoSize_;            //!< Number of metadata values for all the zero baselines (second part in size formula)
    int         crossSize_;           //!< Number of metadata values for all the  non-zero baselines (first part in size formula)
    int         correlationMode_;     //!< Correlation mode
    std::vector<int> v_numAutoPolProduct_; //!< Number of auto polarization products for every baseband

    /** Utility to access the validity of an input basebandNum
     * @param basebandNum Baseband number
     * @return True if not one-based or exceeds the number of basebands in the configuration, else false
     */
    bool wrongBasebandNum(int basebandNum);

    /** Utility method to check that a query is relevant or not for interferometric data
     *  @param interfero
     *  @return True or false depending on the correlationMode value
     */
    bool        notFor(bool interfero);

    /** Utility method to check that the input value of antenna index in a query is valid
     * or not.
     *  @param antIndex The antenna index
     *  @return True or false depending on the upper limit coming from the number of antennas
     */
    bool        wrongAntennaIndex(int antIndex);

    /** Utility method to check that the input baseband index value in a query is valid
     * or not.
     *  @param bbIndex The baseband index
     *  @return True or false depending on the upper limit coming from the number of basebands
     *  set in the configuartion
     */
    bool        wrongBasebandIndex(int bbIndex);

    /** Utility method to check that the input set of index values in a query is valid
     * or not.
     *  @param ppIndex The polarization product index
     *  @param bbIndex The baseband index
     *  @param antIndex_i The antenna index for antenna i
     *  @param antIndex_j The antenna index for antenna j
     *  @return True or false depending on the upper limit for each of these items.
     *  Note that ppIndex is a function of the baseband index.
     */
    bool        wrongPolProductIndex(int ppIndex,int bbIndex,int antIndex_i,int antIndex_j);

    /** Common part shared by the derived classes for summary information
     *  Display, for each baseband sizes (i.e. number of branches in the baseline-based metadata tree)
     */
    virtual void display()=0;


  private:
    int         numValPerBaseline_;      //!< Number of metadata values per non-zero baseline (first sum in size formula)
    int         numValPerZeroBaseline_;  //!< Number of metadata values per zero baseline  (second sum in size formula)

    /** Method to get the size (in number of typed values).
     * The size is determined from the following formula:\n
     * \f$
         N_{bl}~ \sum_{i=1}^{N_{bb}} N_{bin}(i)~N_{pp}^{oo}(i) + 
         N_{ant}~\sum_{i=1}^{N_{bb}} N_{bin}(i)~N_{pp}^{o}(i) 
     * \f$\n
     * and the peculiar non-standard mode (correlationMode=0) by:\n
     * \f$
         N_{bl}~ \sum_{i=1}^{N_{bb}} N_{bin}(i)~N_{pp}^{oo}(i) + 
         N_{ant}~\sum_{i=1}^{N_{bb}} N_{bin}(i)~f(N_{pp})(i)~  
     * \f$\n
     * with in this specific case \n
     * \f$
        f(N_{pp}(i)) = \left\| \begin{array}
        {r@{\quad if\quad}l}
        1 & N_{pp}^{oo}(i)=1\\
        2 & N_{pp}^{oo}(i)>1\\
        \end{array} \right.
     * \f$\n
     * In this abstract class \f$N_{ant}\f$ must be considered as generic. Its actual value 
     * depends of the context in the derived classes.
     * @note The size of the containers for the baseline-based metadata may also be obtained,
     * in number of bytes, with methods of the asdmBinary::BinaryHeaderElement class. 
     */
    void        sizes();                 
  };

  /** \page testbaselinemetadata Access to the baseline-based metadata

    \dontinclude testBaselineMetadata.cpp

    \pre None

    \skip include
    \until // End
  */

}
#define _BASELINEMETADATA_H
#endif
