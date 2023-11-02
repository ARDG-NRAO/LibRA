#if ! defined (MSVIS_VisBufferComponents_H)
#define MSVIS_VisBufferComponents_H

namespace casa {

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
class VisBufferComponents {

public:

    typedef enum {
        Unknown=-1,
        AllBeamOffsetsZero,  // VisBufferAsync use only
        AntennaMounts,       // VisBufferAsync use only
        Ant1,
        Ant2,
        ArrayId,
        BeamOffsets,         // VisBufferAsync use only
        Channel,
        Cjones,
        CorrType,
        Corrected,
        CorrectedCube,
        DataDescriptionId,
        Direction1,
        Direction2,
        Exposure,
        Feed1,
        Feed1_pa,
        Feed2,
        Feed2_pa,
        FieldId,
        Flag,
        FlagCategory,
        FlagCube,
        FlagRow,
        Freq,
        ImagingWeight,
        Model,
        ModelCube,
        NChannel,
        NCorr,
        NRow,
        ObservationId,
        Observed,
        ObservedCube,
        PhaseCenter,
        PolFrame,
        ProcessorId,
        ReceptorAngles,       // VisBufferAsync use only
        Scan,
        Sigma,
        SigmaMat,
        SpW,
        StateId,
        Time,
        TimeCentroid,
        TimeInterval,
        Weight,
        WeightMat,
        WeightSpectrum,
        Uvw,
        UvwMat,
        N_VisBufferComponents // must be last
    } EnumType;


};

}
#endif // ! defined (MSVIS_VisBufferComponents_H)

