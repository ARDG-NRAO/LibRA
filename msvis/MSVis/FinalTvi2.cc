#include <casacore/casa/aips.h>
#include <casacore/casa/Arrays.h>
#include <msvis/MSVis/VisibilityIterator2.h>
#include <msvis/MSVis/FinalTvi2.h>
#include <casacore/ms/MeasurementSets/MeasurementSet.h>
#include <casacore/tables/Tables/RefRows.h>
#include <msvis/MSVis/UtilJ.h>
#include <msvis/MSVis/VisBuffer2.h>

using namespace casacore;
using namespace casa::utilj;

using namespace casacore;
namespace casa {

namespace vi {

FinalTvi2::FinalTvi2 (ViImplementation2 * inputVi,
                      MeasurementSet & finalMs, Bool isWritable)
: TransformingVi2 (inputVi),
  columns_p (),
  columnsAttached_p (false),
  ms_p (finalMs)
{
    VisBufferOptions options = isWritable ? VbWritable : VbNoOptions;
    setVisBuffer (createAttachedVisBuffer (options));
}

FinalTvi2::~FinalTvi2 ()
{
}

//void
//FinalTvi2::configureNewSubchunk ()
//{
//    // Configure the VisBuffer for the new subchunk.  Most information comes from
//    // the Transforming VI2 superclass which in turn gets it from its VI implementation
//    // object.  The main addition is the need to provide the name of the MS output and
//    // the MS index which is always zero since we only support a single output MS.
//
//    Vector<Int> channels = getChannels (0, 0); // args are ignored
//    Int nChannels = channels.nelements();
//
//    getVisBuffer()->configureNewSubchunk (0, // always the first MS
//                                          ms_p.tableName(),
//                                          false,
//                                          isNewArrayId (),
//                                          isNewFieldId (),
//                                          isNewSpectralWindow (),
//                                          getSubchunkId (),
//                                          nRows(),
//                                          nChannels,
//                                          getVii()->nPolarizationIds(),
//                                          getVii()->getCorrelations(),
//                                          getVii()->getWeightScaling());
//}


void
FinalTvi2::origin ()
{
    TransformingVi2::origin ();

    configureNewSubchunk ();
}

void
FinalTvi2::next ()
{
    TransformingVi2::next();

    configureNewSubchunk ();
}

void
FinalTvi2::writeBackChanges (VisBuffer2 * vb)
{
    // Extend the measurement set by the number of rows contained
    // in the VB

    if (! columnsAttached_p){

        columns_p.attachColumns (ms_p, true);
        columnsAttached_p = true;
    }

    // Remember the current fillability state of the VB and then set it
    // to fillable.

    Bool wasFillable = vb->isFillable();
    vb->setFillable (true);

    Int firstRowAdded = ms_p.nrow();
    ms_p.addRow (vb->nRows());
    RefRows rows (firstRowAdded, ms_p.nrow() - 1);

    // Write out the data contained in the VB

    // First the key column values

    writeKeyValues (ms_p, rows);

    // Next the data column values

    writeDataValues (ms_p, rows);

    // Finally everything else

    writeMiscellaneousValues (ms_p, rows);

    // Put the fillability attribute back the way it was.

    vb->setFillable (wasFillable);
}

void
FinalTvi2::writeDataValues (MeasurementSet & /*ms*/, const RefRows & rows)
{
    // For each of the data columns, check to see if the output MS has the columns.
    // If so, write out to the MS the data in the VB for that column.
    //------------------------------------------------------------------------------

    // Write out the visibility data either complex or float

    if (columns_p.isFloatDataPresent()){
        columns_p.floatVis_p.putColumnCells (rows, getVisBuffer()->visCubeFloat());
    }
    else{
        columns_p.vis_p.putColumnCells (rows, getVisBuffer()->visCube());
    }

    // Write out the corrected and/or model visibilities if they are present

    if (! columns_p.corrVis_p.isNull()){
        columns_p.corrVis_p.putColumnCells (rows, getVisBuffer()->visCubeCorrected());
    }

    if (! columns_p.modelVis_p.isNull()){
        columns_p.modelVis_p.putColumnCells (rows, getVisBuffer()->visCubeModel());
    }

    columns_p.flag_p.putColumnCells (rows, getVisBuffer()->flagCube());
    columns_p.flagRow_p.putColumnCells (rows, getVisBuffer()->flagRow());

    columns_p.sigma_p.putColumnCells (rows, getVisBuffer()->sigma());
    columns_p.weight_p.putColumnCells (rows, getVisBuffer()->weight());

    if (! columns_p.weightSpectrum_p.isNull()){
        columns_p.weightSpectrum_p.putColumnCells (rows, getVisBuffer()->weightSpectrum());
    }
}

void
FinalTvi2::writeKeyValues (MeasurementSet & /*ms*/, const RefRows & rows)
{
    // Write out the values that are the database key for the VB rows.

    columns_p.antenna1_p.putColumnCells (rows, getVisBuffer()->antenna1());

    columns_p.antenna2_p.putColumnCells (rows, getVisBuffer()->antenna2());

    columns_p.feed1_p.putColumnCells (rows, getVisBuffer()->feed1());

    columns_p.feed2_p.putColumnCells (rows, getVisBuffer()->feed2());

    columns_p.dataDescription_p.putColumnCells (rows, getVisBuffer()->dataDescriptionIds());

    columns_p.processor_p.putColumnCells (rows, getVisBuffer()->processorId());

    columns_p.time_p.putColumnCells (rows, getVisBuffer()->time());

    columns_p.field_p.putColumnCells (rows, getVisBuffer()->fieldId());
}

void
FinalTvi2::writeMiscellaneousValues (MeasurementSet & /*ms*/, const RefRows & rows)
{
    // WRite out the non-key values for the rows contained in the VB.

    columns_p.timeInterval_p.putColumnCells (rows, getVisBuffer()->timeInterval());

    columns_p.exposure_p.putColumnCells (rows, getVisBuffer()->exposure());

    columns_p.timeCentroid_p.putColumnCells (rows, getVisBuffer()->timeCentroid());

    columns_p.scan_p.putColumnCells (rows, getVisBuffer()->scan());

    columns_p.array_p.putColumnCells (rows, getVisBuffer()->arrayId ());

    columns_p.observation_p.putColumnCells (rows, getVisBuffer()->observationId());

    columns_p.state_p.putColumnCells (rows, getVisBuffer()->stateId());

    columns_p.uvw_p.putColumnCells (rows, getVisBuffer()->uvw ());
}

void
FinalTvi2::writeFlag (const Matrix<Bool> & /*flag*/)
{
    Throw ("Not Implemented");
}

void
FinalTvi2::writeFlag (const Cube<Bool> & /*flag*/)
{
    Throw ("Not Implemented");
}

void
FinalTvi2::writeFlagRow (const Vector<Bool> & /*rowflags*/)
{
    Throw ("Not Implemented");
}

void
FinalTvi2::writeFlagCategory(const Array<Bool>& /*fc*/)
{
    Throw ("Not Implemented");
}


void
FinalTvi2::writeVisCorrected (const Cube<Complex> & /*vis*/)
{
    Throw ("Not Implemented");
}
void
FinalTvi2::writeVisModel (const Cube<Complex> & /*vis*/)
{
    Throw ("Not Implemented");
}
void
FinalTvi2::writeVisObserved (const Cube<Complex> & /*vis*/)
{
    Throw ("Not Implemented");
}

void
FinalTvi2::writeWeight (const Matrix<Float> & /*wt*/)
{
    Throw ("Not Implemented");
}

void
FinalTvi2::writeWeightSpectrum (const Cube<Float> & /*wtsp*/)
{
    Throw ("Not Implemented");
}

void
FinalTvi2::writeSigma (const Matrix<Float> & /*sig*/)
{
    Throw ("Not Implemented");
}

void
FinalTvi2::writeModel(const RecordInterface& /*rec*/, Bool /*iscomponentlist*/,
                      Bool /*incremental*/)
{
    Throw ("Not Implemented");
}

} // end namespace vi

} //# NAMESPACE CASA - END



