#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/ArrayPartMath.h>
#include <casa/BasicMath/Functors.h>
#include <msvis/MSVis/AveragingTvi2.h>
#include <msvis/MSVis/AveragingVi2Factory.h>
#include <msvis/MSVis/MsRows.h>
#include <msvis/MSVis/VisBufferComponents2.h>
#include <msvis/MSVis/VisBuffer2.h>
#include <msvis/MSVis/VisBufferImpl2.h>
#include <msvis/MSVis/Vbi2MsRow.h>
#include <msvis/MSVis/UtilJ.h>
#include <msvis/MSVis/VisibilityIterator2.h>
#include <tuple>
#include <set>

using std::set;

using namespace casacore;
namespace casa {

namespace vi {

namespace avg {

using casa::ms::MsRow;


///////////////////////////////////////////////////////////
//
// VbAvg: Averaging VisBuffer
//
/*

Definition: A baseline sample (baseline for short) is a table row
with a particular (antenna1, antenna2) pair at a particular time.


Averaging does not cross chunk boundaries of the input VI so the
input VI determines what values are averaged together.  For example,
if the input VI is allows data from multiple scans into the same chunk
then some averaging across scans can occur; in this case the scan number
of the result will be the scan number of the first baseline instance
seen.

Row-level value treatment
=========================

The average is approximately computed on a per baseline basis:

averaged_baseline (antenna1, antenna2) =
    sumOverAveragingInterval (baseline (antenna1, antenna2)) /
    nBaselinesFoundInInterval

How row-level values are computed
---------------------------------

Time - Set to time of first baseline making up the average plus
       half of the averaging interval.
Antenna1 - Copied from instance of baseline
Antenna2 - Copied from instance of baseline
Feed1 - Copied from instance of baseline
Feed2 - Copied from instance of baseline
Flag_Row - The flag row is the logical "and" of the flag rows
           averaged together.
Data_Desc_Id - Copied from instance of baseline
Processor_Id - Copied from instance of baseline
Field_Id - Copied from instance of baseline
Interval - Set to averaging interval
Exposure - Minimum of the interval and the sum of the exposures
           from unflagged rows
Time_Centroid - sum (timeCentroid[i] * exposure[i]) / sum (exposure[i])
Scan_Number - Copied from instance of baseline
Sigma - ???
Array_Id - Copied from instance of baseline
Observation_Id - Copied from instance of baseline
State_Id - Copied from instance of baseline
Uvw - Weighted average of the UVW values for the baseline
Weight - ???

Cube-level value treatment
--------------------------

For each baseline (i.e., antenna1, antenna2 pair) the average is
computed for each correlation (co) and channel (ch) of the data cube.

Data - sum (f(weightSpectrum (co, ch)) * data (co, ch)) /
       sum (f(weightSpectrum (co, ch)))
       f :== optional function applied to the weights; default is unity function.
Corrected_Data - Same was for Data however, VI setup can disable producing
                 averaged Corrected_Data
Model_Data - Same was for Data however, VI setup can disable producing
             averaged Model_Data
Weight_Spectrum - sum (weightSpectrum (co, ch))
Flag - Each averaged flag (correlation, channel) is the logical "and"
       of the flag (correlation, channel) making up the average.

*/


class BaselineIndex {

public:

    // make noncopyable...
    BaselineIndex( const BaselineIndex& ) = delete;
    BaselineIndex& operator=( const BaselineIndex& ) = delete;

    BaselineIndex ();
    ~BaselineIndex ();

    Int operator () (Int antenna1, Int antenna2, Int spectralWindow);
    void configure (Int nAntennas, Int nSpw, const VisBuffer2 * vb);


private:

    enum {Empty = -1};

    class SpwIndex : public Matrix<Int>{

    public:

        SpwIndex (Int n) : Matrix<Int> (n, n, Empty) {}

        Int
        getBaselineNumber (Int antenna1, Int antenna2, Int & nBaselines)
        {
            Int i = (* this) (antenna1, antenna2);

            if (i == Empty){

                i = nBaselines ++;
                (* this) (antenna1, antenna2) = i;
            }

            return i;
        }

    private:

    };

    typedef vector<SpwIndex *> Index;

    SpwIndex * addSpwIndex (Int spw);
    Matrix<Int> * createMatrix ();
    void destroy();
    SpwIndex * getSpwIndex (Int spw);

    Index index_p;
    Int nAntennas_p;
    Int nBaselines_p;
    Int nSpw_p;

};

BaselineIndex::BaselineIndex ()
: nAntennas_p (0),
  nBaselines_p (0),
  nSpw_p (0)
{}

BaselineIndex::~BaselineIndex ()
{
    destroy();
}

Int
BaselineIndex::operator () (Int antenna1, Int antenna2, Int spectralWindow)
{
    SpwIndex * spwIndex = getSpwIndex (spectralWindow);
    if (spwIndex == 0){
        addSpwIndex (spectralWindow);
    }

    Int i = spwIndex->getBaselineNumber (antenna1, antenna2, nBaselines_p);

    return i;
}



BaselineIndex::SpwIndex *
BaselineIndex::addSpwIndex (Int i)
{
    // Delete an existing SpwIndex so that we start fresh

    delete index_p [i];

    // Create a new SpwIndex and insert it into the main index.

    index_p [i] = new SpwIndex (nAntennas_p);

    return index_p [i];
}

void
BaselineIndex::configure (Int nAntennas, Int nSpw, const VisBuffer2 * vb)
{
    // Capture the shape parameters

    nAntennas_p = nAntennas;
    nSpw_p = nSpw;
    nBaselines_p = 0;

    // Get rid of the existing index

    destroy ();
    index_p = Index (nSpw_p, (SpwIndex *) 0);

    // Fill out the index based on the contents of the first VB.
    // Usually this will determine the pattern for all of the VBs to be
    // averaged together so that is the ordering the index should
    // capture.

    for (rownr_t i = 0; i < vb->nRows(); i++){

        // Eagerly flesh out the Spw's index

        Int spw = vb->spectralWindows() (i);
        Int antenna1 = vb->antenna1()(i);
        Int antenna2 = vb->antenna2()(i);

        (* this) (antenna1, antenna2, spw);
    }
}

void
BaselineIndex::destroy ()
{
    // Delete all the dynamically allocated spectral window indices.
    // The vector destructor will take care of index_p itself.

    for (Index::iterator i = index_p.begin();
         i != index_p.end();
         i++){

        delete (* i);
    }
}

BaselineIndex::SpwIndex *
BaselineIndex::getSpwIndex (Int spw)
{
    AssertCc (spw < (int) index_p.size());

    SpwIndex * spwIndex = index_p [spw];

    if (spwIndex == 0){
        spwIndex = addSpwIndex (spw);
    }

    return spwIndex;
}

template <typename T>
class PrefilledMatrix {

public:

    PrefilledMatrix () : matrix_p (0, 0, 0), nChannels_p (0), nCorrelations_p (0) {}

    const Matrix<T> &
    getMatrix (Int nCorrelations, Int nChannels, const T & value)
    {
        if (nCorrelations != nCorrelations_p || nChannels != nChannels_p ||
            value != value_p){

            nCorrelations_p = nCorrelations;
            nChannels_p = nChannels;
            value_p = value;

            matrix_p.assign (Matrix<T> (nCorrelations_p, nChannels_p, value_p));
        }

        return matrix_p;
    }

private:

    Matrix<T> matrix_p;
    Int nChannels_p;
    Int nCorrelations_p;
    T value_p;

};

template <typename T>
class CachedPlaneAvg : public ms::CachedArrayBase {

public:

typedef const Cube<T> & (casa::vi::avg::VbAvg::* Accessor) () const;

CachedPlaneAvg (Accessor accessor) : accessor_p (accessor) {}

Matrix<T> &
getCachedPlane (casa::vi::avg::VbAvg * vb, Int row)
{
    if (! isCached()){

        //cache_p.reference ((vb ->* accessor_p)().xyPlane (row)); // replace with something more efficient
        referenceMatrix (cache_p, (vb ->* accessor_p)(), row);
        setCached ();
    }

    return cache_p;
}

private:

    static void
    referenceMatrix (Matrix<T> & cache, const Cube<T> & src, Int row)
    {
        IPosition shape = src.shape ();
        shape.resize (2);

        // This is a bit sleazy but it seems to be helpful to performance.
        // Assumes contiguously stored cube.

        T * storage = const_cast <T *> (& src (IPosition (3, 0, 0, row)));

        cache.takeStorage (shape, storage, casacore::SHARE);
    }

    Accessor accessor_p;
    Matrix<T> cache_p;
};


class VbAvg;

/**
 * Holds multiple rows in a buffer. changeRow() alternates between rows. The details
 * of how the rows are handled need to be traced to its parent class, Vbi2MsRow, and its
 * parent MsRow.
 */
class MsRowAvg : public ms::Vbi2MsRow {

public:

    MsRowAvg (rownr_t row, const VbAvg * vb);

    // Constructor for read/write access

    MsRowAvg (rownr_t row, VbAvg * vb);

    virtual ~MsRowAvg () {}

    Bool baselinePresent () const;
    /// For how many of the rows reachable by changeRow() does baselinePresent(() hold?
    /// That's equivalent to asking how many baselines are being
    Int nBaselinesPresent () const;

    Vector<Bool> correlationFlagsMutable ();
    const Matrix<Int> & counts () const;
    Int countsBaseline () const;
    Matrix<Bool> & flagsMutable () { return Vbi2MsRow::flagsMutable();}
    Double intervalLast () const;
    Double timeFirst () const;
    Double timeLast () const;
    Vector<Double> uvwFirst ();

    void setBaselinePresent (Bool isPresent);
    void setCounts (const Matrix<Int> &);
    void setCountsBaseline (Int);
    void setIntervalLast (Double);
    void setTimeFirst (Double);
    void setTimeLast (Double);

    Double getNormalizationFactor();
    void setNormalizationFactor(Double normalizationFactor);
    void accumulateNormalizationFactor(Double normalizationFactor);

    /**
     * For bookkeeping. Input row indices that have been added so far in the current row
     * (set with changeRow). This is a list of input rows attached to this averaged row,
     * needed to build the map of input->output row indices when this row is transferred to
     * the output/averaged buffer.
     */
    std::vector<Int> inputRowIdxs() { return inputRowIdxs_p[row()]; }
    /**
     * Adds into the 'inputRowIdxs' list the index of an input row from the buffer being
     * averaged.
     * @param idx the index of the input row in the input buffer
     */
    void addInputRowIdx(Int idx) {
        inputRowIdxs_p[row()].push_back(idx);
    }
    /**
     * Clear the list of input rows attached to this row. This is used once the row is
     * transferred to the output/averaged buffer (typically after every average interval).
     */
    void clearRowIdxs() { inputRowIdxs_p[row()].clear(); }

private:

    void configureCountsCache ();

    mutable CachedPlaneAvg<Int> countsCache_p;
    Vector<Double> normalizationFactor_p;
    VbAvg * vbAvg_p; // [use]
    // map: input buffer row index -> output buffer row index
    std::map<Int, std::vector<Int>> inputRowIdxs_p;
};

/**
 * It looks like the intended usage of this VbAvg class (from AveragingTvi2::
 * produceSubchunk()) is as follows:
 *
 * // Use a "VbAvg vbAvg":
 * VisBufferImpl2 * vbToFill = // get/create output (averaged) buffer
 * vbToFill->setFillable(true);
 * vbAvg.setBufferToFill(vbToFill);
 * // we have the input buffer (to be averaged) in "VisibilityIteratorImpl2 vii;
 * while (vii->more()) {
 *    ...
 *    vbAvg.accumulate(vb, subhunk);
 * }
 * vbAvg.finalizeAverages();
 * vbAvg.finalizeBufferFillnig();
 */
class VbAvg : public VisBufferImpl2 {

public:

    friend class MsRowAvg;

    VbAvg (const AveragingParameters & averagingParameters, ViImplementation2 * vi);

    void accumulate (const VisBuffer2 * vb, const Subchunk & subchunk);
    const Cube<Int> & counts () const;
    Bool empty () const;
    void finalizeBufferFilling ();
    void finalizeAverages ();
    MsRowAvg * getRow (Int row) const;
    MsRowAvg * getRowMutable (Int row);
    Bool isComplete () const;
    Bool isUsingUvwDistance () const;
    void markEmpty ();
    int nSpectralWindowsInBuffer () const;
    void setBufferToFill (VisBufferImpl2 *);
    void startChunk (ViImplementation2 *);
    Int getBaselineIndex (Int antenna1, Int antenna2, Int spw) const;
    // Vector with row idx in the averaged/ooutput buffers
    const std::vector<size_t> & row2AvgRow() const { return row2AvgRow_p; };

protected:

    class Doing {
    public:

        Doing ()
        : correctedData_p (false),
          modelData_p (false),
          observedData_p (false),
          floatData_p(false),
          onlymetadata_p(true),
          weightSpectrumIn_p (false),
          sigmaSpectrumIn_p (false),
          weightSpectrumOut_p (false),
          sigmaSpectrumOut_p (false)
        {}

        Bool correctedData_p;
        Bool modelData_p;
        Bool observedData_p;
        Bool floatData_p;
        Bool onlymetadata_p;
        Bool weightSpectrumIn_p;
        Bool sigmaSpectrumIn_p;
        Bool weightSpectrumOut_p;
        Bool sigmaSpectrumOut_p;
    };

    class AccumulationParameters {

    public:

        AccumulationParameters (MsRow * rowInput, MsRowAvg * rowAveraged, const Doing & doing)
        : correctedIn_p (doing.correctedData_p ? rowInput->corrected().data() : 0),
          correctedOut_p (doing.correctedData_p ? rowAveraged->correctedMutable ().data(): 0),
          flagCubeIn_p (rowInput->flags().data()),
          flagCubeOut_p (rowAveraged->flagsMutable().data()),
          floatDataIn_p (doing.floatData_p ? rowInput->singleDishData().data() : 0),
          floatDataOut_p (doing.floatData_p ? rowAveraged->singleDishDataMutable().data() : 0),
          modelIn_p (doing.modelData_p ? rowInput->model().data(): 0),
          modelOut_p (doing.modelData_p ? rowAveraged->modelMutable().data() : 0),
          observedIn_p (doing.observedData_p ? rowInput->observed().data() : 0),
          observedOut_p (doing.observedData_p ? rowAveraged->observedMutable().data() : 0),
          sigmaIn_p (& rowInput->sigma()),
          sigmaOut_p (& rowAveraged->sigmaMutable()),
          sigmaSpectrumIn_p (doing.sigmaSpectrumIn_p ? rowInput->sigmaSpectrum().data() : 0),
          sigmaSpectrumOut_p (doing.sigmaSpectrumOut_p ? rowAveraged->sigmaSpectrumMutable().data() : 0),
          weightIn_p (& rowInput->weight()),
          weightOut_p (& rowAveraged->weightMutable()),
          weightSpectrumIn_p (doing.weightSpectrumIn_p ? rowInput->weightSpectrum().data() : 0),
          weightSpectrumOut_p (doing.weightSpectrumOut_p ? rowAveraged->weightSpectrumMutable().data() : 0)
        {}

        void incrementCubePointers()
        {
            // For improved performance this class is designed to sweep the cube data elements in this row
            // in the order (correlation0, channel0), (correlation1, channel1), etc.

            correctedIn_p && correctedIn_p ++;
            correctedOut_p && correctedOut_p ++;
            flagCubeIn_p && flagCubeIn_p ++;
            flagCubeOut_p && flagCubeOut_p ++;
            floatDataIn_p && floatDataIn_p ++;
            floatDataOut_p && floatDataOut_p ++;
            modelIn_p && modelIn_p ++;
            modelOut_p && modelOut_p ++;
            observedIn_p && observedIn_p ++;
            observedOut_p && observedOut_p ++;
            sigmaSpectrumIn_p && sigmaSpectrumIn_p ++;
            sigmaSpectrumOut_p && sigmaSpectrumOut_p ++;
            weightSpectrumIn_p && weightSpectrumIn_p ++;
            weightSpectrumOut_p && weightSpectrumOut_p ++;
        }

        inline const Complex *
        correctedIn ()
        {
            assert (correctedIn_p != 0);
            return correctedIn_p;
        }

        inline Complex *
        correctedOut ()
        {
            assert (correctedOut_p != 0);
            return correctedOut_p;
        }

        inline const Float *
        floatDataIn ()
        {
            return floatDataIn_p;
        }

        inline Float *
        floatDataOut ()
        {
            return floatDataOut_p;
        }

        inline const Complex *
        modelIn ()
        {
            assert (modelIn_p != 0);
            return modelIn_p;
        }

        inline Complex *
        modelOut ()
        {
            assert (modelOut_p != 0);
            return modelOut_p;
        }

        inline const Complex *
        observedIn ()
        {
            assert (observedIn_p != 0);
            return observedIn_p;
        }

        inline Complex *
        observedOut ()
        {
            assert (observedOut_p != 0);
            return observedOut_p;
        }

        inline const Bool *
        flagCubeIn ()
        {
            assert (flagCubeIn_p != 0);
            return flagCubeIn_p;
        }

        inline Bool *
        flagCubeOut ()
        {
            assert (flagCubeOut_p != 0);
            return flagCubeOut_p;
        }

        inline const Float *
        sigmaSpectrumIn ()
        {
            return sigmaSpectrumIn_p;
        }

        inline Float *
        sigmaSpectrumOut ()
        {
            assert (sigmaSpectrumOut_p != 0);
            return sigmaSpectrumOut_p;
        }

        inline const Float *
        weightSpectrumIn ()
        {
            assert (weightSpectrumIn_p != 0);
            return weightSpectrumIn_p;
        }

        inline Float *
        weightSpectrumOut ()
        {
            assert (weightSpectrumOut_p != 0);
            return weightSpectrumOut_p;
        }

        inline const Vector<Float> &
        weightIn ()
        {
            assert (weightIn_p != 0);
            return *weightIn_p;
        }

        inline Vector<Float> &
        weightOut ()
        {
            assert (weightOut_p != 0);
            return *weightOut_p;
        }

        inline const Vector<Float> &
        sigmaIn ()
        {
            assert (sigmaIn_p != 0);
            return *sigmaIn_p;
        }

        inline Vector<Float> &
        sigmaOut ()
        {
            assert (sigmaOut_p != 0);
            return *sigmaOut_p;
        }



    private:

        const Complex * correctedIn_p;
        Complex * correctedOut_p;
        const Bool * flagCubeIn_p;
        Bool * flagCubeOut_p;
        const Float * floatDataIn_p;
        Float * floatDataOut_p;
        const Complex * modelIn_p;
        Complex * modelOut_p;
        const Complex * observedIn_p;
        Complex * observedOut_p;
        const Vector<Float> * sigmaIn_p;
        Vector<Float> * sigmaOut_p;
        const Float * sigmaSpectrumIn_p;
        Float * sigmaSpectrumOut_p;
        const Vector<Float> * weightIn_p;
        Vector<Float> * weightOut_p;
        const Float * weightSpectrumIn_p;
        Float * weightSpectrumOut_p;


    };

    std::pair<Bool, Vector<Double> > accumulateCubeData (MsRow * rowInput, MsRowAvg * rowAveraged);
    void accumulateElementForCubes (AccumulationParameters & accumulationParameters,
                                    Bool zeroAccumulation);
    template<typename T>
    void
    accumulateElementForCube (const T * unweightedValue,
                              Float weight,
                              Bool zeroAccumulation,
                              T * accumulator);

    template <typename T>
    T accumulateRowDatum (const T & averagedValue, const T & inputValue,
                          Bool resetAverage);



    void accumulateExposure (const VisBuffer2 *);
    /*
     * Called once per row in the input buffer
     * @param rowInput row from the input buffer being averaged
     * @param rowAveraged changing "accumulator" row for the output buffer
     * @param subchunk - hard to understand
     * @param iidx index of the input row in the input buffer
     */
    void accumulateOneRow (MsRow * rowInput, MsRowAvg * rowAveraged,
                           const Subchunk & subchunk, Int iidx);
    void accumulateRowData (MsRow * rowInput, MsRowAvg * rowAveraged, Double adjustedWeight,
                            Bool rowFlagged);
    void accumulateTimeCentroid (const VisBuffer2 * input);
    void captureIterationInfo (VisBufferImpl2 * dstVb, const VisBuffer2 * srcVb,
                               const Subchunk & subchunk);
    void copyBaseline (Int sourceBaseline, Int targetBaseline);
    template<typename T>
    void copyBaselineScalar (Int sourceBaseline, Int targetBaseline,
                             Vector<T> & columnVector);
    template<typename T>
    void copyCubePlaneIf (Bool condition, Int sourceBaseline,
                          Int targetBaseline, Cube<T> & cube);
    void copyIdValues (MsRow * rowInput, MsRowAvg * rowAveraged);
    void copyIdValue (Int inputId, Int & averagedId);
    void finalizeBaseline (MsRowAvg *);
    void finalizeBaselineIfNeeded (MsRow * rowInput, MsRowAvg * rowAveraged,
                                   const Subchunk & subchunk);
    void finalizeCubeData (MsRowAvg *);
    void finalizeRowData (MsRowAvg *);
    AccumulationParameters * getAccumulationParameters (MsRow * rowInput,
                                                        MsRowAvg * rowAveraged);
    Int getBaselineIndex (const MsRow *) const;
    void initializeBaseline (MsRow * rowInput, MsRowAvg * rowAveraged,
                             const Subchunk & subchunk);
    Int nBaselines () const;
    void prepareIds (const VisBuffer2 * vb);
    void removeMissingBaselines ();

private:

    void setupVbAvg (const VisBuffer2 *);
    void setupArrays (Int nCorrelations, Int nChannels, Int nBaselines);
    void setupBaselineIndices (Int nAntennas, const VisBuffer2 * vb);

    /// A baseline being averaged into a MSRowAvg is put into the output/averaged buffer and
    /// set as not present
    void transferBaseline (MsRowAvg *);

    template <typename T>
    static T
    distanceSquared (const Vector<T> & p1, const Vector<T> & p2)
    {
        assert (p1.size() == 3 && p2.size() == 3);

        T distanceSquared = 0;

        for (Int i = 0; i < 3; i++){
            T delta = p1[i] - p2[i];
            distanceSquared += delta * delta;
        }

        return distanceSquared;
    }

    Double averagingInterval_p;
    AveragingOptions averagingOptions_p;
    const ViImplementation2 * averagingVii_p;
    mutable BaselineIndex baselineIndex_p; // map of antenna1,antenna2 to row number in this VB.
    Vector<Bool> baselinePresent_p; // indicates whether baseline has any data
    Vector<Double> normalizationFactor_p; // indicates whether baseline has any data
    VisBufferImpl2 * bufferToFill_p;
    Bool complete_p; // average is complete
    Matrix<Bool> correlationFlags_p; // used for weight accumulation
    Cube<Int> counts_p; // number of items summed together for each correlation, channel, baseline item.
    Vector<Int> countsBaseline_p; // number of items summed together for each baseline.
    Doing doing_p;
    Bool empty_p; // true when buffer hasn't seen any data
    Vector<Double> intervalLast_p;
    Double maxTimeDistance_p;
    Double maxUvwDistanceSquared_p;
    Bool needIterationInfo_p;
    VisBufferComponents2 optionalComponentsToCopy_p;
    Int rowIdGenerator_p;
    Double sampleInterval_p;
    Double startTime_p; // time of the first sample in average
    Vector<Double> timeFirst_p;
    Vector<Double> timeLast_p;
    mutable PrefilledMatrix<Bool> trueBool_p;
    Matrix<Double> uvwFirst_p;
    Bool usingUvwDistance_p;
    mutable PrefilledMatrix<Int> zeroInt_p;
    mutable PrefilledMatrix<Float> zeroFloat_p;

    std::vector<size_t> row2AvgRow_p;

    LogIO logger_p;
};

///////////////////////////////////////////////////////////
//
// Set of Averaging VisBuffers, one per active DD ID


//class VbSet {
//
//public:
//
//    VbSet (const AveragingParameters & averagingParameters);
//    ~VbSet ();
//
//    void accumulate (const VisBuffer2 *, const Subchunk & subchunk);
//    void finalizeBufferFilling ();
//    void setBufferToFill (VisBuffer2 *);
//
//
//    VbAvg * add (Int ddId);
//    Bool anyAveragesReady(Int ddid = -1) const;
//    Bool contains (Int ddId) const;
////    void finalizeAverage (Int ddId);
//    void finalizeAverages ();
//    void finalizeRowIfNeeded (ms::MsRow * rowInput, avg::MsRowAvg * rowAveraged, const Subchunk & subchunk);
//    void flush (Bool okIfNonempty = false, ViImplementation2 * vi = 0); // delete all averagers
//    Int getFirstReadyDdid () const;
//    void transferAverage (Int ddId, VisBuffer2 * vb);
//    Bool vbPastAveragingInterval (const VisBuffer2 * vb) const;
//    void zero ();
//
//protected:
//
//    void seeIfCubeColumnsExist (ViImplementation2 * vi);
//
//private:
//
//    typedef map<Int, VbAvg *> Averagers;
//
//    const Double averagingInterval_p;
//    AveragingOptions averagingOptions_p;
//    const AveragingParameters averagingParameters_p;
//    Bool doingCorrectedData_p;
//    Bool doingModelData_p;
//    Bool doingObservedData_p;
//    Bool doingWeightSpectrum_p;
//    Bool doingsigmaSpectrum_p;
//    Averagers vbAveragers_p;
//};

MsRowAvg::MsRowAvg (rownr_t row, const VbAvg * vb)
: Vbi2MsRow (row, vb),
  countsCache_p (& VbAvg::counts),
  normalizationFactor_p(0.0),
  vbAvg_p (const_cast<VbAvg *> (vb))
{
    configureCountsCache();
}


// Constructor for read/write access

MsRowAvg::MsRowAvg (rownr_t row, VbAvg * vb)
: Vbi2MsRow (row, vb),
  countsCache_p (& VbAvg::counts),
  normalizationFactor_p(0.0),
  vbAvg_p (vb)
{
    configureCountsCache();
}

Bool
MsRowAvg::baselinePresent () const
{
    return vbAvg_p->baselinePresent_p (row ());
}

Int
MsRowAvg::nBaselinesPresent () const
{
    return std::count(vbAvg_p->baselinePresent_p.begin(), vbAvg_p->baselinePresent_p.end(),
                      true);
}

void
MsRowAvg::configureCountsCache ()
{
    addToCachedArrays (countsCache_p);
}

const Matrix<Int> &
MsRowAvg::counts () const
{
    return countsCache_p.getCachedPlane (dynamic_cast<VbAvg *> (getVbi()), row());
}

Vector<Bool>
MsRowAvg::correlationFlagsMutable ()
{
    return vbAvg_p->correlationFlags_p.column (row());
}

Int
MsRowAvg::countsBaseline () const
{
    return vbAvg_p->countsBaseline_p (row ());
}

void
MsRowAvg::setBaselinePresent (Bool value)
{
    vbAvg_p->baselinePresent_p (row ()) = value;
}


void
MsRowAvg::setCountsBaseline (Int value)
{
    vbAvg_p->countsBaseline_p (row ()) = value;
}

Double
MsRowAvg::intervalLast () const
{
    return vbAvg_p->intervalLast_p (row ());
}


Double
MsRowAvg::timeFirst () const
{
    return vbAvg_p->timeFirst_p (row ());
}

Double
MsRowAvg::timeLast () const
{
    return vbAvg_p->timeLast_p (row ());
}

Vector<Double>
MsRowAvg::uvwFirst ()
{
    return vbAvg_p->uvwFirst_p.column (row());
}


void
MsRowAvg::setCounts (const Matrix<Int> & value)
{
    Matrix<Int> & theCounts = countsCache_p.getCachedPlane (dynamic_cast<VbAvg *> (getVbi()), row());
    theCounts = value;
}

void
MsRowAvg::setIntervalLast (Double value)
{
    vbAvg_p->intervalLast_p (row ()) = value;
}


void
MsRowAvg::setTimeFirst (Double value)
{
    vbAvg_p->timeFirst_p (row ()) = value;
}

void
MsRowAvg::setTimeLast (Double value)
{
    vbAvg_p->timeLast_p (row ()) = value;
}

Double MsRowAvg::getNormalizationFactor()
{
	return vbAvg_p->normalizationFactor_p (row ());
}

void MsRowAvg::setNormalizationFactor(Double normalizationFactor)
{
	vbAvg_p->normalizationFactor_p (row ()) = normalizationFactor;
}

void MsRowAvg::accumulateNormalizationFactor(Double normalizationFactor)
{
	vbAvg_p->normalizationFactor_p (row ()) += normalizationFactor;
}

VbAvg::VbAvg (const AveragingParameters & averagingParameters, ViImplementation2 * vii)
: VisBufferImpl2 (vii, VbRekeyable),
  averagingInterval_p (averagingParameters.getAveragingInterval ()),
  averagingOptions_p (averagingParameters.getOptions()),
  averagingVii_p (vii),
  bufferToFill_p (0),
  complete_p (false),
  doing_p (), // all false until determined later on
  empty_p (true),
  maxTimeDistance_p (averagingParameters.getAveragingInterval() * (0.999)),
        // Shrink it just a bit for roundoff
  maxUvwDistanceSquared_p (pow(averagingParameters.getMaxUvwDistance(),2)),
  needIterationInfo_p (true),
  rowIdGenerator_p (0),
  sampleInterval_p (0),
  startTime_p (0),
  usingUvwDistance_p (averagingParameters.getOptions().contains (AveragingOptions::BaselineDependentAveraging))
{}

/**
 * Calculates the row index in the output buffer, given an averaged row, a baseline index
 * corresponding to this averaged row, and the magic "rowIdGenerator" of the VbAvg.
 *
 * @param nrows number of rows in the input buffer being averaged
 * @param baselineIndex index of the baseline being averaged into the rowAveraged
 * @param rowAveraged "accumulator" row being produced for the output buffer
 * @param rowIdGen the rowIdGenerator of the VbAvg, which increases (in a not so clean way)
 *        for every new baseline inside every chunk
 */
Int
calcOutRowIdx(Int nrows, Int baselineIndex, const MsRowAvg* rowAveraged, Int rowIdGen)
{
    auto nBasePresent = rowAveraged->nBaselinesPresent();
    // check whether multiple time steps are being averaged into the output buffer
    // (row blocking or similar feature is enabled)
    const bool multitime =  nrows > nBasePresent;

    if (!multitime) {
        // There is only one time step, so the index must be simply the baseline index.
        // with row blocking disabled, it doesn't seem to be possible to make sense out of
        // rowIdgenerator_p for the purpose of this calculation -> skip the more general
        // calculation from below and just use baseline index.
        return baselineIndex;
    }

    // the rowIdgenerator_p that we get in rowIdGen increases +1 for every new baseline.
    // It is not really a proper (input) row id. After all baselines have been seen for a
    // time step, the rows of the next time steps will get the same id.
    // And to turn it into a valid output id we need the following
    // "divide_with_roundup + multiply + baseline_index"
    Int rowIdG_div_baselines_roundup = 0;
    if (nBasePresent > 0)
        rowIdG_div_baselines_roundup = (rowIdGen + nBasePresent - 1)/ nBasePresent;
    const Int id = rowIdG_div_baselines_roundup * nBasePresent + baselineIndex;

    return id;
}

void
VbAvg::accumulate (const VisBuffer2 * vb, const Subchunk & subchunk)
{
    if (empty_p){
        setupVbAvg (vb);
    }

    if (needIterationInfo_p){
        captureIterationInfo (bufferToFill_p, vb, subchunk);
        needIterationInfo_p = false;
    }

    assert (bufferToFill_p != 0);

    MsRowAvg * rowAveraged = getRowMutable (0);
    MsRow * rowInput = vb->getRow (0);

    auto nrows = vb->nRows();
    row2AvgRow_p.resize(nrows);
    for (rownr_t row = 0; row < nrows; ++row){

        rowInput->changeRow (row);
        Int baselineIndex = getBaselineIndex (rowInput);

        rowAveraged->changeRow (baselineIndex);

        accumulateOneRow (rowInput, rowAveraged, subchunk, row);

        row2AvgRow_p[row] = calcOutRowIdx(nrows, baselineIndex, rowAveraged,
                                          rowIdGenerator_p);
    }

    delete rowAveraged;
    delete rowInput;

}

void
VbAvg::accumulateOneRow (MsRow * rowInput, MsRowAvg * rowAveraged, const Subchunk & subchunk,
                         Int iidx)
{
    finalizeBaselineIfNeeded (rowInput, rowAveraged, subchunk);

    if (! rowAveraged->baselinePresent())
    {

        initializeBaseline (rowInput, rowAveraged, subchunk);
    }

    // bookkeeping - save for later that this input row is being averaged into the output row
    rowAveraged->addInputRowIdx(iidx);

    // Accumulate data that is matrix-valued (e.g., vis, visModel, etc.).
    // The computation of time centroid requires the use of the weight column
    // adjusted for the flag cube.  Get the before and after weight accumulation
    // and the difference is the adjusted weight for this row.

    Vector<Double> adjustedWeights;
    Bool rowFlagged = false;

    std::tie (rowFlagged, adjustedWeights) = accumulateCubeData (rowInput, rowAveraged);

    Double adjustedWeight = 0;
    for (Int c = 0; c < nCorrelations(); c++){

        adjustedWeight += adjustedWeights (c);
    }

    // Accumulate the non matrix-valued data
    accumulateRowData (rowInput, rowAveraged, adjustedWeight, rowFlagged);
}

//void
//VbAvg::accumulate (const VisBuffer2 * vb, const Subchunk & subchunk)
//{
//    // "Add" the contents of this buffer to the accumulation.
//
//    if (empty_p){
//
//        // Initialize the buffer if this is the first time bit of data that it is
//        // being used after either creation or clearing.
//
//        prepareForFirstData (vb, subchunk);
//
//        empty_p = false;
//    }
//
//    // Averaging can be computed as flagged or unflagged.  If all the inputs to a
//    // datum are flagged, then the averaged datum (e.g., a visibility point)
//    // will also be flagged.  For an unflagged averaged datum, it will represent
//    // the average of all of the unflagged points for that datum.  This is done
//    // by assuming the accumulation is flagged and continuing to accumulate
//    // flagged points until the first unflagged point for a datum is encountered;
//    // when this happens the count is zeroed and the averaged datum's flag is cleared.
//
//    // Loop over all of the rows in the VB.  Map each one to a baseline and then accumulate
//    // the values for each correlation,channel cell.  Each row in the accumulating VB corresponds
//    // to one baseline (i.e., pair of (antenna1, antenna2) where antenna1 <= antenna2).
//
//    ThrowIf (vb->nRows() > nBaselines(),
//             String::format ("Expected %d baselines in VisBuffer but it contained %d rows",
//                             nBaselines(), nRows()));
//
//    for (Int row = 0; row < vb->nRows(); row ++){
//
//        // Accumulate data for fields that are scalars (and uvw) in each row
//
//        accumulateRowData (vb, row);
//
//        // Accumulate data that is matrix-valued (e.g., vis, visModel, etc.)
//
//        accumulateCubeData (vb, row);
//    }
//}

void
VbAvg::finalizeBufferFilling ()
{
    bufferToFill_p->appendRowsComplete();
    bufferToFill_p = 0; // decouple
}

template<typename T>
inline void
VbAvg::accumulateElementForCube (const T * unweightedValue,
                                 Float weight,
                                 Bool zeroAccumulation,
                                 T * accumulator)
{
    // Update the sum for this model visibility cube element.

    if (zeroAccumulation){
        * accumulator = (* unweightedValue) * weight;
    }
    else{
        * accumulator += (* unweightedValue) * weight;
    }
}


std::pair<Bool, Vector<Double> >
VbAvg::accumulateCubeData (MsRow * rowInput, MsRowAvg * rowAveraged)
{
    // Accumulate the sums needed for averaging of cube data (e.g., visibility).

    const Matrix<Bool> & inputFlags = rowInput->flags ();
    Matrix<Bool> & averagedFlags = rowAveraged->flagsMutable ();
    Matrix<Int>  counts = rowAveraged->counts ();
    Vector<Bool>  correlationFlagged = rowAveraged->correlationFlagsMutable ();

    AccumulationParameters accumulationParameters (rowInput, rowAveraged, doing_p);
        // is a member variable to reduce memory allocations (jhj)

    IPosition shape = inputFlags.shape();
    const Int nChannels = shape (1);
    const Int nCorrelations = shape (0);

    Bool rowFlagged = true;  // true if all correlations and all channels flagged

    for (Int channel = 0; channel < nChannels; channel ++){

        for (Int correlation = 0; correlation < nCorrelations; correlation ++){

            // Based on the current flag state of the accumulation and the current flag
            // state of the correlation,channel, accumulate the data (or not).  Accumulate
            // flagged data until the first unflagged datum appears.  Then restart the
            // accumulation with that datum.

            Bool inputFlagged = inputFlags (correlation, channel);
            if (rowFlagged && ! inputFlagged){
                rowFlagged = false;
            }
            //rowFlagged = rowFlagged && inputFlagged;
            Bool accumulatorFlagged = averagedFlags (correlation, channel);

            if (! accumulatorFlagged && inputFlagged){
                accumulationParameters.incrementCubePointers();
                continue;// good accumulation, bad data so toss it.
            }

            // If changing from flagged to unflagged for this cube element, reset the
            // accumulation count to 1; otherwise increment the count.

            Bool flagChange = (accumulatorFlagged && ! inputFlagged);
            Bool zeroAccumulation = flagChange || counts (correlation, channel) == 0;

            if (flagChange){
                averagedFlags (correlation, channel) = false;
            }

            if (zeroAccumulation){
                counts (correlation, channel) = 1;
            }
            else{
                counts (correlation, channel) += 1;
            }

            // Accumulate the sum for each cube element

            accumulateElementForCubes (accumulationParameters,
                                       zeroAccumulation); // zeroes out accumulation

            accumulationParameters.incrementCubePointers();

            // Update correlation Flag

            if (correlationFlagged (correlation) && ! inputFlagged){
                correlationFlagged (correlation) = false;
            }
        }
    }

    Vector<Double> adjustedWeight = Vector<Double> (nCorrelations, 1);
    if (doing_p.correctedData_p)
    {
        for (Int correlation = 0; correlation < nCorrelations; correlation ++)
        {
        	adjustedWeight(correlation) = rowInput->weight(correlation);
        }
    }
    else if (doing_p.observedData_p || doing_p.floatData_p)
    {
        for (Int correlation = 0; correlation < nCorrelations; correlation ++)
        {
        	adjustedWeight(correlation) = AveragingTvi2::sigmaToWeight(rowInput->sigma (correlation));
        }
    }

    return std::make_pair (rowFlagged, adjustedWeight);
}


inline void
VbAvg::accumulateElementForCubes (AccumulationParameters & accumulationParameters,
                                  Bool zeroAccumulation)
{

	// NOTE: The channelized flag check comes from the calling context (continue statement)
	float weightCorrected = 1.0f;
	float weightObserved = 1.0f;
	const float One = 1.0f;

	if (doing_p.correctedData_p)
	{
		// The weight corresponding to CORRECTED_DATA is that stored in WEIGHT
		weightCorrected = * accumulationParameters.weightSpectrumIn ();


		// Accumulate weighted average contribution (normalization will come at the end)
		accumulateElementForCube (	accumulationParameters.correctedIn (),
									weightCorrected, zeroAccumulation,
									accumulationParameters.correctedOut ());

		// The weight resulting from weighted average is the sum of the weights
		accumulateElementForCube (	& weightCorrected,
									One, zeroAccumulation,
									accumulationParameters.weightSpectrumOut ());
	}

	if (doing_p.observedData_p)
	{
		// The weight corresponding to DATA is that derived from the rms stored in SIGMA
		// This has to
		weightObserved = AveragingTvi2::sigmaToWeight(* accumulationParameters.sigmaSpectrumIn ());

		// Accumulate weighted average contribution (normalization will come at the end)

		accumulateElementForCube (	accumulationParameters.observedIn (),
									weightObserved, zeroAccumulation,
									accumulationParameters.observedOut ());

		if (not doing_p.correctedData_p)
		{
			// The weight resulting from weighted average is the sum of the weights
			accumulateElementForCube (	& weightObserved,
										One, zeroAccumulation,
										accumulationParameters.weightSpectrumOut ());
		}

        // This will always create a sigma spectrum column which is not empty.
        // This is useful in particular if not doing_p.correctedData_p but doing_p.modelData_p,
        // so that modelData can be properly divided by sigmaSpectrumOut in finalizeCubeData
        // We store the accumulated weight in sigmaSpectrumOut pending of
        // - normalization
        // - SIGMA = 1/sqrt(WEIGHT) in-place transformation
        accumulateElementForCube (&weightObserved,
                                  One, zeroAccumulation,
                                  accumulationParameters.sigmaSpectrumOut ());

	}

	// For model data is less clear what to do, what in order to convert to
	// split we use WEIGHT if averaging CORRECTED_DATA and SIGMA if avg. DATA.
	// Finally we use WEIGHT by default when averaging MODEL_DATA only
	if (doing_p.modelData_p)
	{
		if (doing_p.correctedData_p)
		{
			accumulateElementForCube (	accumulationParameters.modelIn (),
										weightCorrected, zeroAccumulation,
										accumulationParameters.modelOut ());
		}
		else if (doing_p.observedData_p)
		{
			accumulateElementForCube (	accumulationParameters.modelIn (),
										weightObserved, zeroAccumulation,
										accumulationParameters.modelOut ());
		}
		else
		{
			accumulateElementForCube (	accumulationParameters.modelIn (),
										One, zeroAccumulation,
										accumulationParameters.modelOut ());

			// When doing MODEL_DATA only the accumulated weight spectrum should just represent counts
			accumulateElementForCube (	& One,
										1.0f, zeroAccumulation,
										accumulationParameters.weightSpectrumOut ());
		}
	}

	if (doing_p.floatData_p)
	{

		// The weight corresponding to FLOAT_DATA is that derived from the rms stored in SIGMA
		weightObserved = AveragingTvi2::sigmaToWeight(* accumulationParameters.sigmaSpectrumIn ());

		// Accumulate weighted average contribution (normalization will come at the end)
		accumulateElementForCube (	accumulationParameters.floatDataIn (),
									weightObserved, zeroAccumulation,
									accumulationParameters.floatDataOut ());

		// The weight resulting from weighted average is the sum of the weights
		accumulateElementForCube (	& weightObserved,
									1.0f, zeroAccumulation,
									accumulationParameters.weightSpectrumOut ());
	}

	return;
}



template <typename T>
T
VbAvg::accumulateRowDatum (const T & averagedValue, const T & inputValue, Bool resetAverage)
{
    if (resetAverage){
        return inputValue;
    }
    else{
        return inputValue + averagedValue;
    }
}

void
VbAvg::accumulateRowData (MsRow * rowInput, MsRowAvg * rowAveraged,
                          Double adjustedWeight, Bool rowFlagged)
{

    // Grab working copies of the values to be accumulated.

    Bool accumulatorRowFlagged = rowAveraged->isRowFlagged();
    Bool flagChange = accumulatorRowFlagged != rowFlagged || // actual change
                      rowAveraged->countsBaseline() == 0; // first time

    if (! accumulatorRowFlagged && rowFlagged){
        // good accumulation, bad data --> skip it
    }
    else{

        // Update the row's accumulations; if the flagChanged then zero out the
        // previous (flagged) accumulation first.

        rowAveraged->setCountsBaseline (accumulateRowDatum (rowAveraged->countsBaseline(),
                                                            1,
                                                            flagChange));

        // The WEIGHT column is handled under accumulateCubeData because of the
        // interrelationship between weight and weightSpectrum.  The SIGMA column is
        // handled in finalizeBaseline for similar reasons.

        accumulatorRowFlagged = accumulatorRowFlagged && rowFlagged;
        rowAveraged->setRowFlag (accumulatorRowFlagged);

        rowAveraged->setExposure (accumulateRowDatum (rowAveraged->exposure(),
                                                      rowInput->exposure (),
                                                      flagChange));

        // While accumulating flagged values, the weights will be zero, so accumulate
        // an arithmetic average until the accumulator becomes unflagged.

        Double weightToUse;

        if (accumulatorRowFlagged){
            weightToUse = 1;
        }
        else{
            weightToUse = adjustedWeight;
        }

        if (flagChange){
            rowAveraged->setNormalizationFactor(0.0);
        }

        rowAveraged->accumulateNormalizationFactor(weightToUse);

        Double weightedTC = (rowInput->timeCentroid() - rowAveraged->timeFirst()) * weightToUse;
        rowAveraged->setTimeCentroid (accumulateRowDatum (rowAveraged->timeCentroid(),
                                                          weightedTC,
                                                          flagChange));

        Vector<Double> weightedUvw = rowInput->uvw() * weightToUse;

        rowAveraged->setUvw (accumulateRowDatum (rowAveraged->uvw (),
                                                 weightedUvw,
                                                 flagChange));

        // Capture a couple pieces of data

        rowAveraged->setTimeLast (rowInput->time());

        rowAveraged->setIntervalLast (rowInput->interval());
    }
}

//Vector<Float>
//VbAvg::adjustWeightForFlags (MsRow * rowInput)
//{
//    Matrix<Bool> flags = rowInput->flags();
//    Vector<Float> adjustedWeight = rowInput->weight();
//
//    for (Int correlation = 0; correlation < nCorrelations(); correlation++){
//
//        // Sum up the number of good channels in this correlation
//
//        Int sum = 0;
//
//        for (Int channel = 0; channel < nChannels(); channel ++){
//
//            if (! flags (correlation, channel)){
//
//                sum ++;
//            }
//        }
//
//        // Adjust the weight by multiplying by the fraction of good channels.
//
//        Float factor = ((float) sum) / nChannels();
//        adjustedWeight [correlation] *= factor;
//    }
//
//    return adjustedWeight;
//}

const Cube<Int> &
VbAvg::counts () const
{
    return counts_p;
}


void
VbAvg::copyIdValues (MsRow * rowInput, MsRowAvg * rowAveraged)
{
    rowAveraged->setAntenna1 (rowInput->antenna1());
    rowAveraged->setAntenna2 (rowInput->antenna2());
    rowAveraged->setArrayId (rowInput->arrayId());
    rowAveraged->setDataDescriptionId (rowInput->dataDescriptionId());
    rowAveraged->setFeed1 (rowInput->feed1());
    rowAveraged->setFeed2 (rowInput->feed2());
    rowAveraged->setFieldId (rowInput->fieldId());
    rowAveraged->setProcessorId (rowInput->processorId());
    rowAveraged->setScanNumber (rowInput->scanNumber());
    rowAveraged->setSpectralWindow (rowInput->spectralWindow());
    rowAveraged->setObservationId (rowInput->observationId());
    rowAveraged->setStateId (rowInput->stateId());
}

void
VbAvg::copyIdValue (Int inputId, Int & averagedId)
{
    if (averagedId < 0){
        averagedId = inputId;
    }
}

Bool
VbAvg::empty () const
{
    return empty_p;
}

Int
VbAvg::getBaselineIndex (const MsRow * msRow) const
{
    // Lookup the baseline index using the prebuilt lookup table.
    //
    // The baseline index is the index in the sequence
    // {(0,0),(1,0),(1,1),(2,0),(2,1),(2,2), ...} (i.e., the index in a
    // 1-d representation of the lower right half of the square matrix of size
    // nAntennas).
    //
    // This handles the case where the baseline ordering in the input VB might
    // shift from VB to VB.

    const Int antenna1 = msRow->antenna1 ();
    const Int antenna2 = msRow->antenna2 ();
    const Int spw = msRow->spectralWindow ();

    const Int index = baselineIndex_p (antenna1, antenna2, spw);

    return index;
}

Int
VbAvg::getBaselineIndex (Int antenna1, Int antenna2, Int spw) const
{
    const Int index = baselineIndex_p (antenna1, antenna2, spw);

    return index;
}

void
VbAvg::finalizeAverages ()
{
    if (empty_p){
        return; // nothing to finalize
    }

    MsRowAvg * msRowAvg = getRowMutable (0);

    for (Int baseline = 0; baseline < nBaselines(); baseline ++){

        msRowAvg->changeRow (baseline);

        if (msRowAvg->baselinePresent()){
            finalizeBaseline (msRowAvg);
        }

    }

    delete msRowAvg;

    empty_p = true;
}

void
VbAvg::finalizeBaseline (MsRowAvg * msRowAvg)
{
    // Software is no longer supposed to rely on the row flag.
    // Setting it to false insures that legacy software will
    // have to look at the flag cubes.

    msRowAvg->setRowFlag(false);

    finalizeCubeData (msRowAvg);

    finalizeRowData (msRowAvg);

    transferBaseline (msRowAvg);
}

// Functor to divide variables of possibly different types.
// This is unlike std::divides which requires equal types.
template <typename L, typename R=L, typename RES=L>
struct DividesNonZero : public std::binary_function<L,R,RES>
{
  RES operator() (const L& x, const R& y) const
  {
    { return y > 0? RES(x)/y : RES(x); }
  }
};


void
VbAvg::finalizeCubeData (MsRowAvg * msRowAvg)
{
    // Divide each of the data cubes in use by the sum of the appropriate weights.

    typedef DividesNonZero <Complex, Float, Complex> DivideOp;
    DivideOp op;

    if (doing_p.correctedData_p)
    {
        Matrix<Complex> corrected = msRowAvg->correctedMutable();
        arrayTransformInPlace<Complex, Float, DivideOp > (corrected,msRowAvg->weightSpectrum (), op);
    }

    if (doing_p.observedData_p)
    {
        Matrix<Complex> observed = msRowAvg->observedMutable();
        if (not doing_p.correctedData_p)
            arrayTransformInPlace<Complex, Float, DivideOp > (observed,msRowAvg->weightSpectrum (), op);
        else
            arrayTransformInPlace<Complex, Float, DivideOp > (observed,msRowAvg->sigmaSpectrum (), op);
    }

    if (doing_p.modelData_p)
    {
        Matrix<Complex> model = msRowAvg->modelMutable();

        if (doing_p.correctedData_p)
            arrayTransformInPlace<Complex, Float, DivideOp > (model,msRowAvg->weightSpectrum (), op);
        else if (doing_p.observedData_p)
            arrayTransformInPlace<Complex, Float, DivideOp > (model,msRowAvg->sigmaSpectrum (), op);
        else
            arrayTransformInPlace<Complex, Int, DivideOp > (model,msRowAvg->counts (), op);
    }

    if (doing_p.floatData_p)
    {
        typedef Divides <Float, Float, Float> DivideOpFloat;
        DivideOpFloat opFloat;

        Matrix<Float> visCubeFloat = msRowAvg->singleDishDataMutable();
        arrayTransformInPlace<Float, Float, DivideOpFloat > (visCubeFloat,msRowAvg->weightSpectrum (), opFloat);
    }


    return;
}

void
VbAvg::finalizeRowData (MsRowAvg * msRow)
{
    Int n = msRow->countsBaseline ();

    // Obtain row-level WEIGHT by calculating the mean of WEIGHT_SPECTRUM
    // msRow->setWeight(partialMedians(msRow->weightSpectrum(),IPosition(1,1),true));
    msRow->setWeight(AveragingTvi2::average(msRow->weightSpectrum(),msRow->flags()));

    // If doing both DATA and CORRECTED_DATA then SIGMA_SPECTRUM contains the weight
    // (not sigma) accumulation for DATA, and we have to derive SIGMA from it
    if (doing_p.correctedData_p and doing_p.observedData_p)
    {
    	// jagonzal: SIGMA is not derived from the mean of SIGMA_SPECTRUM but from the mean
    	// of the WEIGHT format of SIGMA_SPECTRUM turned into SIGMA by using 1/pow(weight,2)
    	// Vector<Float> weight = partialMedians(msRow->sigmaSpectrum(),IPosition(1,1),true);
    	Vector<Float> weight = AveragingTvi2::average(msRow->sigmaSpectrum(),msRow->flags());
    	arrayTransformInPlace (weight, AveragingTvi2::weightToSigma);
    	msRow->setSigma (weight);

    	// Now convert the DATA weight accumulation stored in sigmaSpectrum into the real SIGMA_SPECTRUM
    	// TODO: This should happen only if we are writing out SIGMA_SPECTRUM but
    	//       multiple column operation is rare and might be forbidden in the future
    	Matrix<Float> sigmaSpectrun = msRow->sigmaSpectrum(); // Reference copy
    	arrayTransformInPlace (sigmaSpectrun, AveragingTvi2::weightToSigma);
    }
    // Otherwise (doing only DATA/FLOAT_DATA or CORRECTED_DATA) we can derive SIGMA from WEIGHT directly
    else if ( not doing_p.onlymetadata_p)
    {
    	// jagonzal: SIGMA is not derived from the mean of SIGMA_SPECTRUM
    	// but from WEIGHT turned into SIGMA by using 1/pow(weight,2)
    	Vector<Float> sigma = msRow->sigma(); // Reference copy
    	sigma = msRow->weight(); // Normal copy (transfer Weight values to Sigma)
    	arrayTransformInPlace (sigma, AveragingTvi2::weightToSigma);

    	// Derive SIGMA_SPECTRUM from computed WEIGHT_SPECTRUM
    	Matrix<Float> sigmaSpectrun = msRow->sigmaSpectrum(); // Reference copy
    	sigmaSpectrun = msRow->weightSpectrum(); // Normal copy (transfer WeightSpectrum values to SigmaSpectrum)
    	arrayTransformInPlace (sigmaSpectrun, AveragingTvi2::weightToSigma);
    }

    // Get the normalization factor for this baseline, containing
    // the accumulation of row-level) weight contributions
    Double weight = msRow->getNormalizationFactor();

    if (n != 0){

        if (weight == 0){

            // The weights are all zero so compute an arithmetic average
            // so that a somewhat value can go into these columns (i.e. rather than NaN).

            weight = msRow->countsBaseline();
        }

        msRow->setTimeCentroid (msRow->timeCentroid() / weight + msRow->timeFirst());

        msRow->setUvw (msRow->uvw() / weight);

        // Exposure is a simple sum, not an average so it is already
        // done at this point.
    }

    // Fill in the time and the interval
    //
    // The time of a sample is centered over the integration time period.
    // Thus find the center of the averaged interval it is necessary to
    // slide it back by 1/2 an interval.

    Double dT = msRow->timeLast () - msRow->timeFirst();

    Double centerOfInterval = msRow->timeFirst () + dT / 2;

    msRow->setTime (centerOfInterval);

    if (dT != 0){

        // The interval is the center-to-center time + half of the intervals of
        // the first and the last sample (if dT == 0 then the interval is
        // already the interval of the first sample and is correct)

        Double interval = dT + msRow->interval() / 2 + msRow->intervalLast() / 2;
        msRow->setInterval (interval);
    }
}

void
VbAvg::finalizeBaselineIfNeeded (MsRow * rowInput, MsRowAvg * rowAveraged, const Subchunk & /*subchunk*/)
{
    if (! rowAveraged->baselinePresent()){
        return;
    }

    // Finalization is needed if either the uvw distance or the time distance between the input
    // baseline and the averaged baseline is above the maximum

    Bool needed = usingUvwDistance_p;

    if (needed) {
        Double deltaUvw = distanceSquared (rowInput->uvw(), rowAveraged->uvwFirst ());
        needed = deltaUvw > maxUvwDistanceSquared_p;
    }

    needed = needed || (rowInput->time() - rowAveraged->timeFirst()) > maxTimeDistance_p;

    if (needed){

        // Finalize the data so that the final averaging products and then move them to
        // output buffer.

        finalizeBaseline (rowAveraged);
    }
}

MsRowAvg *
VbAvg::getRow (Int row) const
{
    return new MsRowAvg (row, this);
}

MsRowAvg *
VbAvg::getRowMutable (Int row)
{
    return new MsRowAvg (row, this);
}

void
VbAvg::initializeBaseline (MsRow * rowInput, MsRowAvg * rowAveraged,
                           const Subchunk & /*subchunk*/)
{
    copyIdValues (rowInput, rowAveraged);

    // Size and zero out the counters

    rowAveraged->setInterval (rowInput->interval()); // capture first one
    rowAveraged->setTimeFirst (rowInput->time());
    rowAveraged->setTimeLast (rowInput->time());
    rowAveraged->uvwFirst () = rowInput->uvw ();

    rowAveraged->setCountsBaseline (0);

    IPosition shape = rowInput->flags().shape();
    Int nCorrelations = shape (0);
    Int nChannels = shape (1);

    rowAveraged->setCounts (zeroInt_p.getMatrix (nCorrelations, nChannels, 0));
    rowAveraged->setWeight (Vector<Float> (nCorrelations, 0));
    rowAveraged->setTimeCentroid (0.0);

    if (doing_p.weightSpectrumOut_p){
    	rowAveraged->setWeightSpectrum (zeroFloat_p.getMatrix (nCorrelations, nChannels, 0));
    }

    if (doing_p.sigmaSpectrumOut_p){
        rowAveraged->setSigmaSpectrum (zeroFloat_p.getMatrix (nCorrelations, nChannels, 0));
    }

//    VisBufferComponents2 exclusions =
//        VisBufferComponents2::these(VisibilityObserved, VisibilityCorrected,
//                                    VisibilityModel, CorrType, JonesC, Unknown);
//
//    cacheResizeAndZero(exclusions);

    // Flag everything to start with

    rowAveraged->setRowFlag (true); // only for use during row-value accumulation
    rowAveraged->setFlags(trueBool_p.getMatrix (nCorrelations, nChannels, true));
    rowAveraged->correlationFlagsMutable() = Vector<Bool> (nCorrelations, true);

    rowAveraged->setBaselinePresent(true);

    rowAveraged->setNormalizationFactor(0.0);
}


Bool
VbAvg::isComplete () const
{
    return complete_p;
}

Bool
VbAvg::isUsingUvwDistance () const
{
    return usingUvwDistance_p;
}


//void
//VbAvg::markEmpty ()
//{
//    empty_p = true;
//    complete_p = false;
//}

Int
VbAvg::nBaselines () const
{
    return countsBaseline_p.nelements();
}

Int
VbAvg::nSpectralWindowsInBuffer () const
{
    const Vector<Int> & windows = spectralWindows();
    set <Int> s;

    for (uInt i = 0; i < windows.nelements(); i ++){
        s.insert (windows(i));
    }

    return (Int) s.size();

}


void
VbAvg::captureIterationInfo (VisBufferImpl2 * dstVb, const VisBuffer2 * srcVb,
                             const Subchunk & subchunk)
{
    dstVb->setIterationInfo (srcVb->msId(),
                             srcVb->msName(),
                             srcVb->isNewMs(),
                             srcVb->isNewArrayId(),
                             srcVb->isNewFieldId(),
                             srcVb->isNewSpectralWindow(),
                             subchunk,
                             srcVb->getCorrelationTypes (),
                             srcVb->getCorrelationTypesDefined(),
                             srcVb->getCorrelationTypesSelected(),
                             CountedPtr <WeightScaling> ( ));

    // Request info from the VB which will cause it to be filled
    // into cache from the input VII at its current position.

    dstVb->setRekeyable(true);
    dstVb->setShape(srcVb->nCorrelations(), srcVb->nChannels(), nBaselines(), false);
        // Do not clear the cache since we're resuing the storage

    dstVb->phaseCenter();
    dstVb->nAntennas();
    dstVb->correlationTypes();
    dstVb->polarizationFrame();
    dstVb->polarizationId();
}

//void
//VbAvg::prepareForFirstData (const VisBuffer2 * vb, const Subchunk & subchunk)
//{
//    startTime_p = vb->time() (0);
//    sampleInterval_p = vb->timeInterval() (0);
//
//    Int nAntennas = vb->nAntennas();
//    Int nSpw = vb->getVi()->nSpectralWindows();
//    Int nBaselines = ((nAntennas * (nAntennas + 1)) / 2) * nSpw;
//
//    // Size and zero out the counters
//
//    timeFirst_p = Vector<Double> (nBaselines, vb->time() (0));
//    timeLast_p = Vector<Double> (nBaselines, vb->time() (0));
//    uvwFirst_p = Vector<Double> (nBaselines, vb->uvw().column(0));
//
//    countsBaseline_p = Vector<Int> (nBaselines, 0);
//    counts_p = Cube<Int> (vb->nCorrelations(), vb->nChannels(), nBaselines, 0);
//    weightSum_p = Cube<Float> (vb->nCorrelations(), vb->nChannels(), nBaselines, 0);
//    if (doing_p.sigmaSpectrum_p){
//        weightCorrectedSum_p = Cube<Float> (vb->nCorrelations(), vb->nChannels(), nBaselines, 0);
//    }
//
//    baselineIndex_p.configure (nAntennas, nSpw, vb);
//
//    // Reshape the inherited members from VisBuffer2
//
//    captureIterationInfo (vb, subchunk);
//
//    setShape (vb->nCorrelations(), vb->nChannels(), nBaselines, false);
//
//    VisBufferComponents2 exclusions =
//        VisBufferComponents2::these(VisibilityObserved, VisibilityCorrected,
//                                    VisibilityModel, CorrType, JonesC, Unknown);
//    cacheResizeAndZero(exclusions);
//
//    prepareIds (vb);
//
//    // Flag everything to start with
//
//    setFlagCube (Cube<Bool> (vb->nCorrelations(), vb->nChannels(), nBaselines, true));
//    setFlagRow (Vector<Bool> (nBaselines, true));
//
//    complete_p = false;
//}

void
VbAvg::prepareIds (const VisBuffer2 * vb)
{
    // Set these row ID values to indicate they are unknown

    Vector<Int> minusOne (nBaselines(), -1);

    setAntenna1 (minusOne);
    setAntenna2 (minusOne);
    setDataDescriptionIds (minusOne);
    setFeed1 (minusOne);
    setFeed2 (minusOne);
    setProcessorId (minusOne);
    setScan (minusOne);
    setObservationId (minusOne);
    setSpectralWindows (minusOne);
    setStateId (minusOne);

    // Copy the value from the input VB

    Vector<Int> tmp (nBaselines(), vb->arrayId()(0));

    setArrayId (tmp);

    tmp = vb->dataDescriptionIds()(0);
    setDataDescriptionIds (tmp);

    tmp = vb->fieldId()(0);
    setFieldId (tmp);
}

//void
//VbAvg::removeMissingBaselines ()
//{
//    // Some baselines may not be present in the portion of the input data
//    // that made up this average.  However, this is not known until after
//    // all of the data is seen.  Thus at finalization time these bogus
//    // baselines should be removed from the average so as not to pass
//    // flagged but zero-exposure baselines into the output.
//
//
//    Vector<Int> rowsToDelete (nBaselines());
//
//    Int nBaselinesDeleted = 0;
//
//    for (Int baseline = 0; baseline < nBaselines(); baseline ++){
//
//        if (countsBaseline_p (baseline) == 0){
//            rowsToDelete (nBaselinesDeleted) = baseline;
//            nBaselinesDeleted ++;
//        }
//    }
//
//    rowsToDelete.resize (nBaselinesDeleted, true);
//
//    deleteRows (rowsToDelete);
//}

void
VbAvg::setBufferToFill(VisBufferImpl2 * vb)
{
    bufferToFill_p = vb;

    // Set flag so that iteration information will be captured into
    // this VB from the first input VB.

    needIterationInfo_p = true;
}

void
VbAvg::setupVbAvg (const VisBuffer2 * vb)
{
    // Configure the index

    Int nAntennas = vb->nAntennas();

    // This is a kluge to allow multiple spectral windows (of the same shape)
    // to be combined into a single VB.  This really shouldn't be allowed!!!

    set<uInt> spwInVb;

    for (rownr_t i = 0; i < vb->nRows(); i++){
        spwInVb.insert (vb->dataDescriptionIds()(i));
    }

    uInt nSpwInVb = spwInVb.size();

    Int nSpw = averagingVii_p->nSpectralWindows();

    baselineIndex_p.configure (nAntennas, nSpw, vb);

    Int nBaselines = ((nAntennas * (nAntennas + 1)) / 2)* nSpwInVb;

    setShape (vb->nCorrelations(), vb->nChannels(), nBaselines);

    setupArrays (vb->nCorrelations(), vb->nChannels(), nBaselines);

    baselinePresent_p.resize(nBaselines);
    baselinePresent_p = False;

    normalizationFactor_p.resize(nBaselines);
    normalizationFactor_p = 0.0;

    empty_p = false;
}

void
VbAvg::setupArrays (Int nCorrelations, Int nChannels, Int nBaselines)
{

    setShape (nCorrelations, nChannels, nBaselines);

    // Start out with all of the array-valued components except the
    // optional ones.

    VisBufferComponents2 including =
	VisBufferComponents2::these ({VisBufferComponent2::Antenna1,
		    VisBufferComponent2::Antenna2,
		    VisBufferComponent2::ArrayId,
		    VisBufferComponent2::CorrType,
		    VisBufferComponent2::DataDescriptionIds,
		    VisBufferComponent2::Exposure,
		    VisBufferComponent2::Feed1,
		    VisBufferComponent2::Feed2,
		    VisBufferComponent2::FieldId,
		    VisBufferComponent2::FlagCategory,
		    VisBufferComponent2::FlagCube,
		    VisBufferComponent2::FlagRow,
		    VisBufferComponent2::ObservationId,
		    VisBufferComponent2::ProcessorId,
		    VisBufferComponent2::RowIds,
		    VisBufferComponent2::Scan,
		    VisBufferComponent2::Sigma,
		    VisBufferComponent2::SpectralWindows,
		    VisBufferComponent2::StateId,
		    VisBufferComponent2::Time,
		    VisBufferComponent2::TimeCentroid,
		    VisBufferComponent2::TimeInterval,
		    VisBufferComponent2::Weight,
		    VisBufferComponent2::Uvw});

    if (doing_p.modelData_p){
        including += VisBufferComponent2::VisibilityCubeModel;
    }

    if (doing_p.correctedData_p){
        including += VisBufferComponent2::VisibilityCubeCorrected;
    }

    if (doing_p.observedData_p){
        including += VisBufferComponent2::VisibilityCubeObserved;
    }

    if (doing_p.floatData_p){
        including += VisBufferComponent2::VisibilityCubeFloat;
    }

    if (doing_p.weightSpectrumOut_p){
        including += VisBufferComponent2::WeightSpectrum;
    }

    if (doing_p.sigmaSpectrumOut_p){
        including += VisBufferComponent2::SigmaSpectrum;
    }

    cacheResizeAndZero (including);

    correlationFlags_p.reference (Matrix<Bool> (IPosition (2, nCorrelations, nBaselines), true));
    counts_p.reference (Cube<Int> (IPosition (3, nCorrelations, nChannels, nBaselines), 0));
    countsBaseline_p.reference (Vector<Int> (nBaselines, 0)); // number of items summed together for each baseline.
    intervalLast_p.reference (Vector<Double> (nBaselines, 0));
    timeFirst_p.reference (Vector<Double> (nBaselines, 0));
    timeLast_p.reference (Vector<Double> (nBaselines, 0));
    uvwFirst_p.reference (Matrix<Double> (IPosition (2, 3, nBaselines), 0.0));
}

void
VbAvg::startChunk (ViImplementation2 * vi)
{
    empty_p = true;

    rowIdGenerator_p = 0;
    row2AvgRow_p.resize(0);
    
    // See if the new MS has corrected and/or model data columns

    doing_p.observedData_p = averagingOptions_p.contains (AveragingOptions::AverageObserved);
    doing_p.correctedData_p = vi->existsColumn (VisBufferComponent2::VisibilityCubeCorrected) &&
                              averagingOptions_p.contains (AveragingOptions::AverageCorrected);
    doing_p.modelData_p = vi->existsColumn (VisBufferComponent2::VisibilityCubeModel) &&
                          averagingOptions_p.contains (AveragingOptions::AverageModel);
    doing_p.floatData_p = vi->existsColumn (VisBufferComponent2::VisibilityCubeFloat) &&
                          averagingOptions_p.contains (AveragingOptions::AverageFloat);

    doing_p.weightSpectrumIn_p = doing_p.correctedData_p;
    doing_p.sigmaSpectrumIn_p = doing_p.observedData_p || doing_p.floatData_p;
    doing_p.weightSpectrumOut_p = true; // We always use the output WeightSpectrum
    doing_p.sigmaSpectrumOut_p = true; // We always use the output SigmaSpectrum

    if (doing_p.observedData_p or doing_p.correctedData_p or doing_p.modelData_p or doing_p.floatData_p)
    {
    	doing_p.onlymetadata_p = false;
    }

    // Set up the flags for row copying

    optionalComponentsToCopy_p = VisBufferComponents2::none();

    if (doing_p.observedData_p){
        optionalComponentsToCopy_p += VisBufferComponent2::VisibilityCubeObserved;
    }

    if (doing_p.correctedData_p){
        optionalComponentsToCopy_p += VisBufferComponent2::VisibilityCubeCorrected;
    }

    if (doing_p.modelData_p){
        optionalComponentsToCopy_p += VisBufferComponent2::VisibilityCubeModel;
    }

    if (doing_p.floatData_p){
        optionalComponentsToCopy_p += VisBufferComponent2::VisibilityCubeFloat;
    }

    if (doing_p.weightSpectrumOut_p){
        optionalComponentsToCopy_p += VisBufferComponent2::WeightSpectrum;
    }

    if (doing_p.sigmaSpectrumOut_p){
        optionalComponentsToCopy_p += VisBufferComponent2::SigmaSpectrum;
    }
}

void
VbAvg::transferBaseline (MsRowAvg * rowAveraged)
{
    rowAveraged->setRowId (rowIdGenerator_p ++);
    bufferToFill_p->appendRow (rowAveraged, nBaselines(), optionalComponentsToCopy_p);

    rowAveraged->setBaselinePresent(false);
}


//VbSet::VbSet (const AveragingParameters & averagingParameters)
//: averagingInterval_p (averagingParameters.getAveragingInterval ()),
//  averagingOptions_p (averagingParameters.getOptions()),
//  averagingParameters_p (averagingParameters),
//  doingCorrectedData_p (false),
//  doingModelData_p (false),
//  doingObservedData_p (false),
//  doingWeightSpectrum_p (false),
//  doingsigmaSpectrum_p (false),
//  vbAveragers_p ()
//{}

//VbSet::~VbSet ()
//{
//    flush (true); // allow killing nonempty buffers
//}
//
//void
//VbSet::accumulate (const VisBuffer2 * input, const Subchunk & subchunk)
//{
//    Int ddId = input->dataDescriptionIds()(0);
//
//    if (! utilj::containsKey (ddId, vbAveragers_p)){ // Need a new averager
//        add (ddId);
//    }
//
//    VbAvg * vba = vbAveragers_p [ddId];
//
//    vba->accumulate (input, subchunk);
//}
//
//VbAvg *
//VbSet::add (Int ddId)
//{
//    VbAvg::Doing doing;
//
//    doing.correctedData_p = doingCorrectedData_p;
//    doing.modelData_p = doingModelData_p;
//    doing.observedData_p = doingObservedData_p;
//    doing.weightSpectrum_p = doingWeightSpectrum_p;
//    doing.sigmaSpectrum_p = doingsigmaSpectrum_p;
//
//    VbAvg * newAverager =  new VbAvg (doing, averagingParameters_p);
//
//    vbAveragers_p [ddId] = newAverager;
//
//    return newAverager;
//}
//
//Bool
//VbSet::anyAveragesReady(Int ddid) const
//{
//    Bool any = false;
//
//    for (Averagers::const_iterator a = vbAveragers_p.begin();
//         a != vbAveragers_p.end();
//         a++){
//
//        if (a->second->isComplete() &&
//            (ddid < 0 || ddid == a->second->dataDescriptionIds()(0))){
//
//            any = true;
//            break;
//        }
//    }
//
//    return any;
//}
//
//Bool
//VbSet::contains (Int ddId) const
//{
//    return vbAveragers_p.find (ddId) != vbAveragers_p.end();
//}
//
////void
////VbSet::finalizeAverage (Int ddId)
////{
////    vbAveragers_p [ddId]->finalizeAverage();
////}
//
//void
//VbSet::finalizeAverages ()
//{
////    for (Averagers::iterator a = vbAveragers_p.begin();
////         a != vbAveragers_p.end();
////         a ++){
////
////        finalizeAverage (a->first);
////    }
//}
//
//void
//VbSet::flush (Bool okIfNonempty, ViImplementation2 * vi)
//{
//    // Get rid of all of the averagers.  This is done at
//    // destruction time and when a sweeping into a new MS.
//
//    for (Averagers::const_iterator a = vbAveragers_p.begin();
//         a != vbAveragers_p.end();
//         a ++){
//
//        Assert (okIfNonempty || (a->second)->empty());
//            // should have been emptied before calling this
//
//        delete a->second;
//    }
//
//    vbAveragers_p.clear();
//
//    seeIfCubeColumnsExist (vi);
//}
//
//void
//VbSet::seeIfCubeColumnsExist (ViImplementation2 * vi)
//{
//    if (vi != 0){
//
//        // See if the new MS has corrected and/or model data columns
//
//        doingObservedData_p = averagingOptions_p.contains (AveragingOptions::AverageObserved);
//        doingCorrectedData_p = vi->existsColumn (VisibilityCubeCorrected) &&
//                               averagingOptions_p.contains (AveragingOptions::AverageCorrected);
//        doingModelData_p = vi->existsColumn (VisibilityCubeModel) &&
//                               averagingOptions_p.contains (AveragingOptions::AverageModel);
//        doingWeightSpectrum_p = vi->existsColumn (WeightSpectrum);
//
//        // If the use of corrected weights were specified for one of the averages, it's an
//        // error if the column does not exist.  Also set the doing flag for corrected weights
//        // if it's being used in some way.
//
//        Bool needCorrectedWeights =
//            averagingOptions_p.contains (AveragingOptions::ModelUseCorrectedWeights) ||
//            averagingOptions_p.contains (AveragingOptions::CorrectedUseCorrectedWeights);
//
//        Bool correctedWeightsMissing = needCorrectedWeights &&
//                                       ! vi->existsColumn (sigmaSpectrum);
//
//        ThrowIf (correctedWeightsMissing,
//                 "Corrected_weight_spectrum not present but required by provided averaging options");
//
//        doingsigmaSpectrum_p = needCorrectedWeights;
//    }
//}
//
//Int
//VbSet::getFirstReadyDdid () const
//{
//    for (Averagers::const_iterator a = vbAveragers_p.begin();
//         a != vbAveragers_p.end();
//         a ++){
//
//        if (a->second->isComplete()){
//            return a->first;
//        }
//    }
//
//    Assert (false); // ought to have been one that's ready
//
//    return -1; // shouldn't be called
//}
//
////void
////VbSet::transferAverage (Int ddId, VisBuffer2 * vb)
////{
////    Assert (utilj::containsKey (ddId, vbAveragers_p));
////
////    VbAvg * vba = vbAveragers_p [ddId];
////
////    Assert (vba != 0 && ! vba->empty ());
////
////    // Copy< the completed average into the provided VisBuffer, but
////    // first reshape the VB if it's shape is different.
////
////    vba->transferAverage (vb);
////
////}
//
//void
//VbSet::zero ()
//{
//    for (Averagers::const_iterator a = vbAveragers_p.begin();
//         a != vbAveragers_p.end();
//         a ++){
//
//        a->second->markEmpty();
//    }
//}

    ///////////////////////
    //                   //
    // End Namespace avg //
    //                   //
    ///////////////////////

} // end avg

using namespace avg;

AveragingTvi2::AveragingTvi2 (ViImplementation2 * inputVi,
                              const AveragingParameters & averagingParameters)
: TransformingVi2 (inputVi),
  averagingInterval_p (averagingParameters.getAveragingInterval()),
  averagingOptions_p (averagingParameters.getOptions()),
  averagingParameters_p (averagingParameters),
  ddidLastUsed_p (-1),
  inputViiAdvanced_p (false),
  vbAvg_p (new VbAvg (averagingParameters, this))
{
    validateInputVi (inputVi);

    // Position input Vi to the first subchunk

    getVii()->originChunks();
    getVii()->origin ();

    setVisBuffer (createAttachedVisBuffer (VbNoOptions));
}

AveragingTvi2::~AveragingTvi2 ()
{
    delete vbAvg_p;
}

//void
//AveragingTvi2::advanceInputVii ()
//{
//    Assert (false);
//
//    // Attempt to advance to the next subchunk
//
//    getVii()->next ();
//
//    if (! getVii()->more()){
//
//        // No more subchunks so advance to the next chunk
//
//        getVii()->nextChunk();
//
//        if (! getVii()->moreChunks()){
//            return; // no more chunks
//        }
//
//        // Position to the first subchunk
//
//        getVii()->origin();
//    }
//}

//Int
//AveragingTvi2::determineDdidToUse() const
//{
//    if (ddidLastUsed_p >= 0 && vbSet_p->anyAveragesReady (ddidLastUsed_p)){
//        return ddidLastUsed_p;
//    }
//
//    return vbSet_p->getFirstReadyDdid();
//}

Bool
AveragingTvi2::more () const
{
    return more_p;
}

Bool
AveragingTvi2::moreChunks () const
{
    return getVii()->moreChunks();
}

void
AveragingTvi2::next ()
{
    subchunkExists_p = false;

    startBuffer_p = endBuffer_p + 1;
    endBuffer_p = startBuffer_p - 1;

    if (getVii()->more()){
        getVii()->next();
        endBuffer_p++;
    }

    produceSubchunk ();

    subchunk_p.incrementSubChunk();
}

void
AveragingTvi2::nextChunk ()
{
    // New chunk, so toss all of the accumulators

    vbAvg_p->startChunk (getVii());

    // Advance the input to the next chunk as well.

    getVii()->nextChunk ();

    subchunk_p.incrementChunk();

    more_p = false;
}

void
AveragingTvi2::origin ()
{
    // Position input VI to the start of the chunk

    subchunk_p.resetSubChunk();

    getVii()->origin();

    startBuffer_p = 0;
    endBuffer_p = -1;

    // Get the first subchunk ready.

    produceSubchunk ();
}

void
AveragingTvi2::originChunks (Bool forceRewind)
{
    // Ensure that the underlying VI is in a state where some metadata
    // can be retrieved
    getVii()->originChunks(forceRewind);

    // Initialize the chunk
    vbAvg_p->startChunk (getVii());

    more_p = false;

    subchunk_p.resetToOrigin();
}

/**
 * Configure a VisBuffer with given averagingOptions related to phase shifting
 *
 * @param vb a VisBuffer to set up in terms of phase shifting
 * @param averagingOptions averaging options enabled
 * @param avgPars AveragingParmeters object to set into the buffer
 */
void
setPhaseShiftingOptions(VisBuffer2 * vb, const AveragingOptions &averagingOptions,
                        const AveragingParameters &avgPars)
{
    if (averagingOptions.contains (AveragingOptions::phaseShifting))
    {
        if (averagingOptions.contains (AveragingOptions::AverageObserved))
        {
            vb->visCube();
        }

        if (averagingOptions.contains (AveragingOptions::AverageCorrected))
        {
            vb->visCubeCorrected();
        }

        if (averagingOptions.contains (AveragingOptions::AverageModel))
        {
            vb->visCubeModel();
        }

        vb->phaseCenterShift(avgPars.getXpcOffset(),
                             avgPars.getYpcOffset());
    }
}

/**
 * The iteration of this method is different depending on whether "row blocking" is used or
 * not. The reason is that the while loop already had enough complexity embedded when fixes
 * were done to get flagdata+time-average+row-blocking working (CAS-11910). Hopefully in the
 * near future we can get rid of the hacky "row blocking" feature. For the time being, it is
 * not clear how it could possibly work together with the "uvwdistance" feature. So better
 * to keep those features separate.
 *
 * So the "if (block > 0)" separates iteration when using row blocking. That implies that
 * row blocking takes precedence over (and disables) other features like
 * "isUsingUvwDistance()".
 * An alternative would be to add comparisons between block and vbToFill->appendSize() in
 * the ifs below.  Something like:
 *         if (! vbAvg_p->isUsingUvwDistance()
 *            && (block == 0 && vbToFill->appendSize() > 0
 *                || (block > 0 && vbToFill->appendSize() >= block)
 *                )
 *            ){
 *          ...
 *         else if ((block > 0 && vbToFill->appendSize() < block) ||
 *                 vbToFill->appendSize() < nBaselines * nWindows){
 *         ...
 *         } else {
 *
 * But I prefer not adding more complexity to those ifs. The potential combinations would
 * be too many to handle in a handful of if branches, and they are not well understood let
 * alone well tested.
 */
void
AveragingTvi2::produceSubchunk ()
{
    VisBufferImpl2 * vbToFill = dynamic_cast<VisBufferImpl2 *> (getVisBuffer());
    assert (vbToFill != 0);

    vbToFill->setFillable (true); // New subchunk, so it's fillable

    vbAvg_p->setBufferToFill (vbToFill);

    Int nBaselines = nAntennas() * (nAntennas() -1) / 2;
        // This is just a heuristic to keep output VBs from being too small

    // jagonzal: Handle nBaselines for SD case
    if (nBaselines == 0) nBaselines = 1;

    auto block = getVii()->getRowBlocking();
    while (getVii()->more()){

        VisBuffer2 * vb = getVii()->getVisBuffer();

        setPhaseShiftingOptions(vb, averagingOptions_p, averagingParameters_p);

        bool rowBlocking = block > 0;
        vbAvg_p->accumulate (vb, subchunk_p);

        if (rowBlocking) {
            auto app = vbToFill->appendSize();
            if (app <= block) {
                getVii()->next();
                endBuffer_p++;
            } else {
                break;
            }
        } else {
            Int nWindows = vbAvg_p->nSpectralWindowsInBuffer ();
            if (! vbAvg_p->isUsingUvwDistance() && vbToFill->appendSize() > 0){
                // Doing straight average and some data has been produced so
                // output it to the user
                break;
            }
            else if (vbToFill->appendSize() < nBaselines * nWindows) {
                getVii()->next();
                endBuffer_p++;
            }
            else {
                break;
            }
        }
    };

    if (! getVii()->more()){
        vbAvg_p->finalizeAverages ();
    }

    vbAvg_p->finalizeBufferFilling ();

    more_p = getVii()->more() || // more to read
             vbToFill->nRows() > 0; // some to process
}

// Bool
// AveragingTvi2::reachedAveragingBoundary()
// {
//     // An average can be terminated for a variety of reasons:
//     // o the time interval has elapsed
//     // o the current MS is completed
//     // o no more input data
//     // o other (e.g, change of scan, etc.)

//     Bool reachedIt = false;
//     VisBuffer2 * vb = getVii()->getVisBuffer();

//     if (! getVii()->more()  && ! getVii ()->moreChunks()){

//         reachedIt = true; // no more input data
//     }
//     else if (vb->isNewMs()){
//         reachedIt = true; // new MS
//     }

//     return reachedIt;
// }

//Bool
//AveragingTvi2::subchunksReady() const
//{
//    Bool ready = vbSet_p->anyAveragesReady();
//
//    return ready;
//}

void
AveragingTvi2::validateInputVi (ViImplementation2 *)
{
    // Validate that the input VI is compatible with this VI.

    // No implmented right now
}

Float AveragingTvi2::weightToSigma (Float weight)
{
    return weight > FLT_MIN ? 1.0 / std::sqrt (weight) : -1.0; // bogosity indicator
}

Vector<Float>
AveragingTvi2::average (const Matrix<Float> &data, const Matrix<Bool> &flags)
{
    IPosition shape = data.shape();
    Vector<Float> result(shape(0),0);
    Vector<uInt> samples(shape(0),0);
    uInt nCorrelations = shape (0);
    uInt nChannels = shape (1);

    for (uInt correlation = 0; correlation < nCorrelations; correlation++)
    {
        int nSamples = 0;
        float sum = 0;
        bool accumulatorFlag = true;

        for (uInt channel=0; channel< nChannels; channel++)
        {
            Bool inputFlag = flags(correlation,channel);
            // true/true or false/false
            if (accumulatorFlag == inputFlag)
            {
                nSamples ++;
                sum += data (correlation, channel);
            }
            // true/false: Reset accumulation when accumulator switches from flagged to unflagged
            else if ( accumulatorFlag and ! inputFlag )
            {
                accumulatorFlag = false;
                nSamples = 1;
                sum = data (correlation, channel);
            }
            // else ignore case where accumulator is valid and data is not

        }

        result (correlation) = sum / (nSamples != 0 ? nSamples : 1);
    }

    return result;
}

Matrix<Float>
AveragingTvi2::average (const Cube<Float> &data, const Cube<Bool> &flags)
{
	IPosition shape = data.shape();
	uInt nRows = shape(2);
	uInt nCorrelations = shape (0);

	Matrix<Float> result(nCorrelations, nRows, 0);

	for (uInt row=0; row < nRows; row++)
	{
		result.column(row) = AveragingTvi2::average (data.xyPlane(row), flags.xyPlane(row));
	}

    return result;
}

/**
 * Strategy to support different ways of propagating flags from the 'transformed' cube to
 * the original ('propagated') cube. Iterates through rows, channels, correlations.
 *
 * This is meant to be used from propagateChanAvgFlags with at least two alternative
 * functors. One to propagate flags as required by flagdata (preserve pre-existing flags
 * in the original data cube), and a second one to propagate flags as required by plotms.
 * CAS-12737, CAS-9985, CAS-12205.
 *
 * @param flagrow per row FLAG_ROW value
 * @param flagMapped propagated FLAG_ROW
 * @param flagCubeMapped Cube of flags in which flags are to be propagated
 * @param row2AvgRow map of input_buffer_row_index->output_buffer_row_index (this is pre-
 *        calculated by the "VbAvg" when averaging rows and is needed here).
 */
template <typename Functor>
void cubePropagateFlags(const Vector<Bool> &flagRow,
                        Vector<Bool> &flagMapped,
                        Cube<Bool> &flagCubeMapped,
                        std::vector<size_t> row2AvgRow,
                        Functor propagate)
{
    Int nRowsMapped = flagCubeMapped.shape()[2];
    for(Int rowMapped=0; rowMapped < nRowsMapped; ++rowMapped) {
        size_t index = row2AvgRow[rowMapped];
        flagMapped(rowMapped) = flagRow(index);

        for (Int chan_i=0; chan_i < flagCubeMapped.shape()[1]; ++chan_i) {
            for (Int corr_i=0; corr_i<flagCubeMapped.shape()[0]; ++corr_i) {
                propagate(rowMapped, chan_i, corr_i, index);
            }
        }
    }
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
void AveragingTvi2::writeFlag (const Cube<Bool> & flag)
{
    // Calculate FLAG_ROW from flag
    Vector<Bool> flagRow;
    TransformingVi2::calculateFlagRowFromFlagCube(flag, flagRow);

    const auto flagdataFlagPropagation =
        averagingOptions_p.contains(AveragingOptions::flagdataFlagPropagation);

    // Propagate transformed flags
    getVii()->origin();
    Int currentBuffer = 0;
    while (getVii()->more())
    {
        if ((currentBuffer >= startBuffer_p) and (currentBuffer <= endBuffer_p))
        {
            // Allocated propagated flag vector/cube
            uInt nOriginalRows = getVii()->getVisBuffer()->nRows();
            Vector<Bool> flagMapped(nOriginalRows,false);
            Cube<Bool> flagCubeMapped;
            flagCubeMapped = getVii()->getVisBuffer()->flagCube();

            // Keeping two separate blocks for 'flagdataFlagPropagation' (CAS-12737,
            // CAS-12205, CAS-9985) until this issue is better settled.
            // Fill propagated flag vector/cube
            if (flagdataFlagPropagation)
            {
                cubePropagateFlags(flagRow, flagMapped, flagCubeMapped, vbAvg_p->row2AvgRow(),
                                   [&flag, &flagCubeMapped]
                                   (uInt rowMapped, uInt chan_i, uInt corr_i, uInt index) {
                                       if (flag(corr_i,chan_i,index))
                                           flagCubeMapped(corr_i,chan_i,rowMapped) = true;
                                   });
            }
            else
            {
                cubePropagateFlags(flagRow, flagMapped, flagCubeMapped, vbAvg_p->row2AvgRow(),
                                   [&flag, &flagCubeMapped]
                                   (uInt rowMapped, uInt chan_i, uInt corr_i, uInt index) {
                                       flagCubeMapped(corr_i, chan_i, rowMapped) =
                                           flag(corr_i, chan_i, index);
                                   });
            }

            // Write propagated flag vector/cube
            getVii()->writeFlag(flagCubeMapped);
            getVii()->writeFlagRow(flagMapped);
        }

        currentBuffer++;
        getVii()->next();
        if (currentBuffer > endBuffer_p) break;
    }
}

// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
void AveragingTvi2::writeFlagRow (const Vector<Bool> & flagRow)
{
	// Create index map for averaged data
	VisBuffer2 *avgVB = getVisBuffer();
	Vector<Int> avgAnt1 = avgVB->antenna1();
	Vector<Int> avgAnt2 = avgVB->antenna2();
	Vector<Int> avgSPW = avgVB->spectralWindows();

	std::map< Int, std::map <Int, std::map< Int, uInt> >  > spwAnt1Ant2IndexMap;
	for (uInt avgRow=0;avgRow<avgAnt1.size();avgRow++)
	{
		spwAnt1Ant2IndexMap[avgSPW(avgRow)][avgAnt1(avgRow)][avgAnt2(avgRow)] = avgRow;
	}

	// Propagate transformed flags
	getVii()->origin();
	Int currentBuffer = 0;
	while (getVii()->more())
	{
		if ((currentBuffer >= startBuffer_p) and (currentBuffer <= endBuffer_p))
		{
			// Allocated propagated flag vector/cube
			uInt nOriginalRows = getVii()->getVisBuffer()->nRows();
			Vector<Bool> flagMapped(nOriginalRows,false);

			// Get original ant1/ant2/spw cols. to determine the mapped index
			Vector<Int> orgAnt1 = getVii()->getVisBuffer()->antenna1();
			Vector<Int> orgAnt2 = getVii()->getVisBuffer()->antenna2();
			Vector<Int> orgSPW = getVii()->getVisBuffer()->spectralWindows();

			// Fill propagated flag vector/cube
			for (uInt row=0;row<nOriginalRows;row++)
			{
				uInt index = spwAnt1Ant2IndexMap[orgSPW(row)][orgAnt1(row)][orgAnt2(row)];
				flagMapped(row) = flagRow(index);
			}

			// Write propagated flag vector/cube
			getVii()->writeFlagRow(flagMapped);

		}

		currentBuffer += 1;
		getVii()->next();
		if (currentBuffer > endBuffer_p) break;
	}

	return;
}

void AveragingTvi2::visibilityObserved(casacore::Cube<casacore::Complex>& vis) const
{
  if(!averagingOptions_p.contains(AveragingOptions::AverageObserved))
      throw AipsError("Requesting visibilityCorrected but column has not been averaged");
    VisBuffer2* vb = getVisBuffer();
    vis = vb->visCube();
    return;
}

void AveragingTvi2::visibilityCorrected(casacore::Cube<casacore::Complex>& vis) const
{
    if(!getVii()->getVisBuffer()->existsColumn (VisBufferComponent2::VisibilityCubeCorrected) ||
        !averagingOptions_p.contains(AveragingOptions::AverageCorrected))
        throw AipsError("Requesting visibilityCorrected but column not "
            "provided by underlying VI or column has not been averaged");
    VisBuffer2* vb = getVisBuffer();
    vis = vb->visCubeCorrected();
    return;
}

void AveragingTvi2::visibilityModel(casacore::Cube<casacore::Complex>& vis) const
{
    if(!getVii()->getVisBuffer()->existsColumn (VisBufferComponent2::VisibilityCubeModel) ||
        !averagingOptions_p.contains(AveragingOptions::AverageModel))
        throw AipsError("Requesting visibilityModel but column not "
            "provided by underlying VI or column has not been averaged");
    VisBuffer2* vb = getVisBuffer();
    vis = vb->visCubeModel();
    return;
}

void AveragingTvi2::floatData(casacore::Cube<casacore::Float>& fcube) const
{
    if(!getVii()->getVisBuffer()->existsColumn (VisBufferComponent2::VisibilityCubeFloat) ||
        !averagingOptions_p.contains(AveragingOptions::AverageFloat))
        throw AipsError("Requesting floatData but column not "
            "provided by underlying VI or column has not been averaged");
    VisBuffer2* vb = getVisBuffer();
    fcube = vb->visCubeFloat();
    return;
}

void AveragingTvi2::flag(casacore::Cube<casacore::Bool>& flags) const
{
    VisBuffer2* vb = getVisBuffer();
    flags = vb->flagCube();
    return;
}

void AveragingTvi2::flagRow(casacore::Vector<casacore::Bool>& rowflags) const
{
    VisBuffer2* vb = getVisBuffer();
    rowflags = vb->flagRow();
    return;
}

void AveragingTvi2::sigma(casacore::Matrix<casacore::Float> & sigmat) const
{
    VisBuffer2* vb = getVisBuffer();
    sigmat = vb->sigma();
    return;
}

void AveragingTvi2::weight (casacore::Matrix<casacore::Float> & wtmat) const
{
    VisBuffer2* vb = getVisBuffer();
    wtmat = vb->weight();
    return;
}

void AveragingTvi2::weightSpectrum (casacore::Cube<casacore::Float> & wtsp) const
{
    VisBuffer2* vb = getVisBuffer();
    wtsp = vb->weightSpectrum();
    return;
}

void AveragingTvi2::sigmaSpectrum (casacore::Cube<casacore::Float> & sigsp) const
{
    VisBuffer2* vb = getVisBuffer();
    sigsp = vb->sigmaSpectrum();
    return;
}

void AveragingTvi2::exposure (casacore::Vector<double> & expo) const
{
    VisBuffer2* vb = getVisBuffer();
    expo = vb->exposure();
    return;
}

void AveragingTvi2::getRowIds (Vector<rownr_t> & rowids) const
{
    VisBuffer2* vb = getVisBuffer();
    rowids = vb->rowIds();
    return;
}

void AveragingTvi2::time (casacore::Vector<double> & t) const
{
    VisBuffer2* vb = getVisBuffer();
    t = vb->time();
    return;
}

void AveragingTvi2::timeInterval (casacore::Vector<double> & ti) const
{
    VisBuffer2* vb = getVisBuffer();
    ti = vb->timeInterval();
    return;
}

void AveragingTvi2::timeCentroid (casacore::Vector<double> & t) const
{
    VisBuffer2* vb = getVisBuffer();
    t = vb->timeCentroid();
    return;
}

void AveragingTvi2::uvw (casacore::Matrix<double> & uvwmat) const
{
    VisBuffer2* vb = getVisBuffer();
    uvwmat = vb->uvw();
    return;
}

void AveragingTvi2::antenna1 (casacore::Vector<casacore::Int> & ant1) const
{
    VisBuffer2* vb = getVisBuffer();
    ant1 = vb->antenna1();
    return;
}

void AveragingTvi2::antenna2 (casacore::Vector<casacore::Int> & ant2) const
{
    VisBuffer2* vb = getVisBuffer();
    ant2 = vb->antenna2();
    return;
}

casacore::Bool AveragingTvi2::weightSpectrumExists () const
{
  //According to VbAvg::startChunk code comments,
  //there is always an output weightSpectrum. See also CAS-11559.
  return true;
}

casacore::Bool AveragingTvi2::sigmaSpectrumExists () const
{
  //According to VbAvg::startChunk code comments,
  //there is always an output sigmaSpectrum. See also CAS-11559.
  return true;
}

} // end namespace vi

using namespace casacore;
} // end namespace casa
