#if ! defined (MSVIS_VisBufferComponents2_H)
#define MSVIS_VisBufferComponents2_H

#include <casacore/casa/BasicSL/String.h>
#include <initializer_list>
#include <set>

namespace casa {

namespace vi {

enum class VisBufferComponent2 {

    Unknown=-1,

    AllBeamOffsetsZero,  // VisBufferAsync use only
    AntennaMounts,       // VisBufferAsync use only
    Antenna1,
    Antenna2,
    ArrayId,
    BeamOffsets,         // VisBufferAsync use only
    CorrType,
    DataDescriptionIds,
    Direction1,
    Direction2,
    Exposure,
    Feed1,
    FeedPa1,
    Feed2,
    FeedPa2,
    FieldId,
    FlagCategory,
    FlagCube,
    FlagCubes,
    FlagRow,
    FloatData,
    Frequencies,
    ImagingWeight,
    JonesC,
    NAntennas,
    NChannels,
    NCorrelations,
    NRows,
    NShapes,
    NRowsPerShape,
    NChannelsPerShape,
    NCorrelationsPerShape,
    ObservationId,
    PhaseCenter,
    PolFrame,
    PolarizationId,
    ProcessorId,
    ReceptorAngles,       // VisBufferAsync use only
    RowIds,
    Scan,
    Sigma,
    Sigmas,
    SpectralWindows,
    StateId,
    Time,
    TimeCentroid,
    TimeInterval,
    Weight,
    Weights,
    WeightScaled,
    WeightSpectrum,
    WeightSpectra,
    SigmaSpectrum,
    SigmaSpectra,
    WeightSpectrumScaled,
    Uvw,
    VisibilityCorrected,
    VisibilityModel,
    VisibilityObserved,
    VisibilityCubeCorrected,
    VisibilityCubesCorrected,
    VisibilityCubeModel,
    VisibilityCubesModel,
    VisibilityCubeObserved,
    VisibilityCubesObserved,
    VisibilityCubeFloat,
    VisibilityCubesFloat,

    N_VisBufferComponents2 // must be last

};


// <summary>
//
// This class encapsulates an enum with values corresponding to the various
// data components accessible via a VisBuffer.  Its current usage is to
// specify the VisBuffer components to be prefetched when asynchronous I/O
// is in use or to specify the VisBuffer components that have been modified
// when a Visibility Processing Framework bucket-brigade is operating on
// a VisBuffer.  This implementation should be replaced by a C++11 when
// the standard matures a bit.
//
// <prerequisite>
//   <li> <linkto class="VisBuffer">VisBuffer</linkto>
// </prerequisite>
//
// </summary>
//
// <synopsis>
//
// </synopsis>
// <example>
//
// <code>
//
// </code>
// </example>
//
class VisBufferComponents2 {

public:


    typedef std::set<VisBufferComponent2> Set;
    typedef Set::const_iterator const_iterator;

    VisBufferComponents2 operator+ (const VisBufferComponents2 & other) const;
    VisBufferComponents2 & operator+= (const VisBufferComponents2 & other);
    VisBufferComponents2 & operator+= (VisBufferComponent2 componentToAdd);

    const_iterator begin () const;
    casacore::Bool contains (VisBufferComponent2 component) const;
    casacore::Bool empty () const;
    const_iterator end () const;
    casacore::String toString () const;

    static VisBufferComponents2 all ();

    // * exceptThese *: n.b., use the component "Unknown" as the last one in the list so that the variadic
    // framework knows when to stop.

    static VisBufferComponents2 exceptThese (std::initializer_list <VisBufferComponent2> components);

    static casacore::String name (VisBufferComponent2 id);
    static VisBufferComponents2 none ();
    static VisBufferComponents2 singleton (VisBufferComponent2 component);

    // * these * : n.b., use the component "Unknown" as the last one in the list so that the variadic
    // framework knows when to stop.

    static VisBufferComponents2 these (std::initializer_list<VisBufferComponent2> components);

protected:

private:

    Set set_p;

    static const VisBufferComponents2 all_p;

    static VisBufferComponents2 initializeAll ();

};

} // end namespace vi


} // end namespace casa
#endif // ! defined (MSVIS_VisBufferComponents2_H)

