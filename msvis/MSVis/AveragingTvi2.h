#if ! defined (Msvis_AveragingTvi2_H_121211_1236)
#define Msvis_AveragingTvi2_H_121211_1236

#include <casa/aips.h>
#include <msvis/MSVis/TransformingVi2.h>
#include <msvis/MSVis/VisibilityIterator2.h>
#include <msvis/MSVis/AveragingVi2Factory.h>

namespace casa {

namespace ms {

class MsRow;

};

namespace vi {

class AveragingOptions;

namespace avg {

class MsRowAvg;
class VbAvg;

}

class AveragingTvi2 : public TransformingVi2 {

public:

    AveragingTvi2 (ViImplementation2 * inputVii,
                   const AveragingParameters & averagingParameters);
    ~AveragingTvi2 ();

    // Report the the ViImplementation type
    virtual casacore::String ViiType() const { return casacore::String("TimeAve( ")+getVii()->ViiType()+" )"; };

    /////////////////////////////////////////////////////////////////////////
    //
    // Chunk/Subchunk structure in the AveragingTvi2
    //
    // The averaging interval, in seconds, is specified at construction time.
    // The interval (i.e., the size of the chunk in time) is also specified
    // at creation time; the interval must be an integer multiple of the
    // averaging interval.
    //
    // The input VI's setting must be compatible with those of the
    // AveragingTvi2.  This means that the chunk size of the input VI must
    // have the same duration as the averaging VI.  Although the input VI
    // and the averaging VI have the same duration, the averaging VI will
    // have fewer subchunks since N input subchunks will be averaged down
    // to create a single output subchunk.
    //
    // The input VI will also define the averaging boundaries by its
    // definition of a chunk.  For example, if the input VI allows data with
    // different scans to be in the same chunk, then they will potentially
    // be averaged together.
    //
    // The input VI must use the data description ID as a sort column so that
    // a chunk will only contain data from a single DDID setting.

    void originChunks (casacore::Bool forceRewind = false);
    void nextChunk ();
    casacore::Bool moreChunks () const;

    void origin ();
    void next ();
    casacore::Bool more () const;

    void writeFlag (const casacore::Cube<casacore::Bool> & flag);
    void writeFlagRow (const casacore::Vector<casacore::Bool> & rowflags);

    virtual void visibilityCorrected(casacore::Cube<casacore::Complex>& vis) const;
    virtual void visibilityModel(casacore::Cube<casacore::Complex>& vis) const;
    virtual void visibilityObserved(casacore::Cube<casacore::Complex>& vis) const;
    virtual void floatData(casacore::Cube<casacore::Float>& fcube) const;
    virtual void flag(casacore::Cube<casacore::Bool>& flags) const;
    virtual void flagRow(casacore::Vector<casacore::Bool>& rowflags) const;
    virtual void weight(casacore::Matrix<casacore::Float>& wtmat) const;
    virtual void sigma(casacore::Matrix<casacore::Float>& sigmat) const;
    virtual void weightSpectrum (casacore::Cube<casacore::Float> & wtsp) const;
    virtual void sigmaSpectrum (casacore::Cube<casacore::Float> & sigsp) const;
    virtual casacore::Bool weightSpectrumExists () const;
    virtual casacore::Bool sigmaSpectrumExists () const;
    virtual void exposure (casacore::Vector<double> & expo) const;
    virtual void getRowIds (casacore::Vector<casacore::rownr_t> & rowids) const;
    virtual void time (casacore::Vector<double> & t) const;
    virtual void timeInterval (casacore::Vector<double> & ti) const;
    virtual void timeCentroid (casacore::Vector<double> & t) const;
    virtual void antenna1 (casacore::Vector<casacore::Int> & ant1) const;
    virtual void antenna2 (casacore::Vector<casacore::Int> & ant2) const;
    virtual void uvw (casacore::Matrix<double> & uvwmat) const;

    static casacore::Float weightToSigma (casacore::Float weight);
    static inline casacore::Float sigmaToWeight (casacore::Float sigma)
    {
        return sigma > FLT_MIN ? 1.0 / (sigma * sigma) : 0.0; // bad sample
    }


    static casacore::Vector<casacore::Float> average (const casacore::Matrix<casacore::Float> &data, const casacore::Matrix<casacore::Bool> &flags);
    static casacore::Matrix<casacore::Float> average (const casacore::Cube<casacore::Float> &data, const casacore::Cube<casacore::Bool> &flags);

protected:

    void advanceInputVii ();
    casacore::Int determineDdidToUse () const;
    casacore::Bool inputExceedsTimeDistance (ms::MsRow * rowInput, avg::MsRowAvg * rowAveraged);
    casacore::Bool inputExceedsUvwDistance (ms::MsRow * rowInput, avg::MsRowAvg * rowAveraged);
    void produceSubchunk ();
    void processInputSubchunk (const VisBuffer2 *);
    casacore::Bool reachedAveragingBoundary();
    void captureIterationInfo (const VisBuffer2 * vb2);
    bool subchunksReady () const;
    void validateInputVi (ViImplementation2 *);

private:

    const casacore::Double averagingInterval_p; // averaging interval in seconds
    AveragingOptions averagingOptions_p;
    AveragingParameters averagingParameters_p;
    casacore::Int ddidLastUsed_p; // ddId last used to produce a subchunk.
    casacore::Bool inputViiAdvanced_p; // true if input VII was advanced but data not used
    casacore::Bool more_p;
    Subchunk subchunk_p;
    casacore::Bool subchunkExists_p;
    avg::VbAvg * vbAvg_p;
    WeightScaling * weightScaling_p;
    casacore::Int startBuffer_p;
    casacore::Int endBuffer_p;
};

} // end namespace vi

} // end namespace casa

#endif // ! defined (Msvis_AveragingTvi2_H_121211_1236)
