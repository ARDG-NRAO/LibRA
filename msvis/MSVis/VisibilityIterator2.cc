/*
 * VisibilityIterator2.cc
 *
 *  Created on: Jun 4, 2012
 *      Author: jjacobs
 */

#include <msvis/MSVis/VisibilityIterator2.h>

#include <casacore/casa/Arrays/Cube.h>
#include <casacore/casa/Arrays/Matrix.h>
#include <casacore/casa/Arrays/Slicer.h>
#include <casacore/casa/BasicSL/String.h>
#include <casacore/casa/Quanta/MVDoppler.h>
#include <casacore/casa/aips.h>
#include <casacore/casa/System/AipsrcValue.h>
#include <casacore/measures/Measures/MCDoppler.h>
#include <casacore/measures/Measures/MDoppler.h>
#include <casacore/measures/Measures/MeasConvert.h>
#include <casacore/measures/Measures/Stokes.h>
#include <casacore/ms/MSOper/MSDerivedValues.h>
#include <casacore/ms/MeasurementSets/MSIter.h>
#include <casacore/ms/MeasurementSets/MeasurementSet.h>
#include <casacore/scimath/Mathematics/RigidVector.h>
#include <casacore/scimath/Mathematics/SquareMatrix.h>
#include <msvis/MSVis/AveragingTvi2.h>
#include <msvis/MSVis/ViFrequencySelection.h>
#include <msvis/MSVis/StokesVector.h>
#include <msvis/MSVis/VisBuffer2.h>
#include <msvis/MSVis/VisBufferComponents2.h>
#include <msvis/MSVis/VisImagingWeight.h>
#include <msvis/MSVis/VisibilityIteratorImpl2.h>
#include <msvis/MSVis/VisibilityIteratorImplAsync2.h>
#include <msvis/MSVis/UtilJ.h>
#include <casacore/tables/Tables/ArrayColumn.h>
#include <casacore/tables/Tables/ScalarColumn.h>

#include <cstdarg>
#include <map>
#include <set>
#include <utility>
#include <vector>

using namespace std;

#define CheckImplementationPointer() Assert (impl_p != NULL);

using namespace casacore;
namespace casa {

namespace vi {

SortColumns::SortColumns (const Block<Int> & columnIds, Bool addDefaultColumns)
: addDefaultColumns_p (addDefaultColumns),
  columnIds_p (columnIds),
  usingDefaultSortingFunctions_p (true)
{
    for (auto colId : columnIds)
        sortingDefinition_p.push_back(
            std::make_pair(MS::columnName(casacore::MS::PredefinedColumns(colId)), nullptr));
}

SortColumns::SortColumns (Bool usingDefaultSortingFunctions)
: usingDefaultSortingFunctions_p (usingDefaultSortingFunctions)
{
}

SortColumns::SortColumns (const std::vector<std::pair<casacore::MS::PredefinedColumns, casacore::CountedPtr<casacore::BaseCompare>>> sortingDefinition)
: addDefaultColumns_p (false),
  usingDefaultSortingFunctions_p (false)
{
    for (auto pair : sortingDefinition)
        sortingDefinition_p.push_back(
            std::make_pair(MS::columnName(pair.first), pair.second));
}

SortColumns::SortColumns (const std::vector<std::pair<casacore::String, casacore::CountedPtr<casacore::BaseCompare>>> sortingDefinition)
: addDefaultColumns_p (false),
  sortingDefinition_p(sortingDefinition),
  usingDefaultSortingFunctions_p (false)
{
}

void SortColumns::addSortingColumn(casacore::MS::PredefinedColumns colId,
    casacore::CountedPtr<casacore::BaseCompare> sortingFunction)
{
    if (usingDefaultSortingFunctions_p)
        throw AipsError("SortColumns invalid construction. "
                        "Cannot add generic sorting functions.");
    sortingDefinition_p.push_back(
        std::make_pair(MS::columnName(colId), sortingFunction));
    usingDefaultSortingFunctions_p = false;
}

void SortColumns::addSortingColumn(casacore::String colName,
    casacore::CountedPtr<casacore::BaseCompare> sortingFunction)
{
    if (usingDefaultSortingFunctions_p)
        throw AipsError("SortColumns invalid construction. "
                        "Cannot add generic sorting functions.");
    sortingDefinition_p.push_back(
        std::make_pair(colName, sortingFunction));
    usingDefaultSortingFunctions_p = false;
}

Bool
SortColumns::shouldAddDefaultColumns () const
{
    return addDefaultColumns_p;
}

bool
SortColumns::usingDefaultSortingFunctions () const
{
    return usingDefaultSortingFunctions_p;
}

const Block<Int> &
SortColumns::getColumnIds () const
{
    return columnIds_p;
}

const std::vector<std::pair<casacore::String, casacore::CountedPtr<casacore::BaseCompare>>> &
SortColumns::sortingDefinition() const
{
    return sortingDefinition_p;
}

CountedPtr <WeightScaling>
WeightScaling::generateUnityWeightScaling ()
{
    return generateWeightScaling (WeightScaling::unity);
}

CountedPtr <WeightScaling>
WeightScaling::generateIdentityWeightScaling ()
{
    return  generateWeightScaling (WeightScaling::identity);
}

CountedPtr <WeightScaling>
WeightScaling::generateSquareWeightScaling ()
{
    return  generateWeightScaling (WeightScaling::square);
}

Float WeightScaling::unity (Float) { return 1.0;}
Float WeightScaling::identity (Float x) { return x;}
Float WeightScaling::square (Float x) { return x * x;}

VisibilityIterator2::VisibilityIterator2()
: impl_p (0)
{
}

VisibilityIterator2::VisibilityIterator2(const MeasurementSet& ms,
                                         const SortColumns & sortColumns,
                                         Bool isWritable,
                                         const VisBufferComponents2 * prefetchColumns,
                                         Double timeInterval)
{
    Block<const MeasurementSet *> mss (1, & ms);
    construct (prefetchColumns, mss, sortColumns,
               timeInterval, isWritable);
}

VisibilityIterator2::VisibilityIterator2 (const Block<const MeasurementSet *>& mss,
                                          const SortColumns & sortColumns,
                                          Bool isWritable,
                                          const VisBufferComponents2 * prefetchColumns,
                                          Double timeInterval)
{
    construct (prefetchColumns, mss, sortColumns, timeInterval, isWritable);
}

VisibilityIterator2::VisibilityIterator2 (const ViFactory & factory)
: impl_p (0)
{
    ViImplementation2 * newImpl = factory.createVi ();

    impl_p = newImpl;
}

VisibilityIterator2::VisibilityIterator2 (const Vector<ViiLayerFactory*> & factories)
: impl_p (0)
{

    Int nfactory=factories.nelements();
    
    if(factories(nfactory-1) == nullptr)
        throw(AipsError("ViiLayerFactory in factories is null"));

    ViImplementation2 * newImpl = factories(nfactory-1)->createViImpl2(factories(Slice(0,nfactory-1,1)));

    impl_p = newImpl;
}


void
VisibilityIterator2::construct (const VisBufferComponents2 * prefetchColumns,
                                const Block<const MeasurementSet *>& mss,
                                const SortColumns & sortColumns,
                                Double timeInterval,
                                Bool writable)
{

    // Factory didn't create the read implementation so decide whether to create a
    // synchronous or asynchronous read implementation.

    Bool createAsAsynchronous = prefetchColumns != NULL && isAsynchronousIoEnabled ();

    if (createAsAsynchronous){
        //            impl_p = new ViImplAsync2 (mss, * prefetchColumns, sortColumns,
        //                                               addDefaultSortCols, timeInterval, writable);
    }
    else{
        impl_p = new VisibilityIteratorImpl2 (mss, sortColumns, timeInterval, writable);
    }
}

VisibilityIterator2::~VisibilityIterator2 ()
{
	delete impl_p;
}

String 
VisibilityIterator2::ViiType() const
{ 
  return impl_p->ViiType (); 
}


void
VisibilityIterator2::next()
{
    CheckImplementationPointer ();
    impl_p->next ();
}


const MeasurementSet&
VisibilityIterator2::ms () const
{
    CheckImplementationPointer ();
    return impl_p->ms ();
}

Bool
VisibilityIterator2::existsColumn (VisBufferComponent2 id) const
{
    CheckImplementationPointer ();

    return impl_p->existsColumn (id);
}

Bool
VisibilityIterator2::weightSpectrumExists () const
{
    CheckImplementationPointer ();
    return impl_p->weightSpectrumExists();
}

ViImplementation2 *
VisibilityIterator2::getImpl () const
{
    return impl_p;
}

Int
VisibilityIterator2::getReportingFrameOfReference () const
{
    return impl_p->getReportingFrameOfReference ();
}

void
VisibilityIterator2::setReportingFrameOfReference (Int frame)
{
    impl_p->setReportingFrameOfReference (frame);
}


Subchunk
VisibilityIterator2::getSubchunkId () const
{
    CheckImplementationPointer ();
    return impl_p->getSubchunkId ();
}

VisBuffer2 *
VisibilityIterator2::getVisBuffer ()
{
    CheckImplementationPointer ();
    return impl_p->getVisBuffer(this);
}

Bool
VisibilityIterator2::isAsynchronous () const
{
//    Bool isAsync = impl_p != NULL && dynamic_cast<const ViImplAsync2 *> (impl_p) != NULL;
//
//    return isAsync;

    return false; // for now
}


Bool
VisibilityIterator2::isAsynchronousIoEnabled()
{
    // Determines whether asynchronous I/O is enabled by looking for the
    // expected AipsRc value.  If not found then async i/o is disabled.

    Bool isEnabled;
    AipsrcValue<Bool>::find (isEnabled, getAipsRcBase () + "async.enabled", false);

    return isEnabled;
}

Bool
VisibilityIterator2::more () const
{
    CheckImplementationPointer ();
    return impl_p->more ();
}

Bool
VisibilityIterator2::moreChunks () const
{
    CheckImplementationPointer ();
    return impl_p->moreChunks ();
}


void
VisibilityIterator2::nextChunk ()
{
    CheckImplementationPointer ();
    impl_p->nextChunk ();
}

void
VisibilityIterator2::result(casacore::Record& res) const
{
    impl_p->result(res);
}

// Report Name of slowest column that changes at end of current iteration
String VisibilityIterator2::keyChange() const 
{ 
  return impl_p->keyChange(); 
}





Int
VisibilityIterator2::nDataDescriptionIds () const
{
    CheckImplementationPointer ();
    return impl_p->nDataDescriptionIds ();
}
//
Int
VisibilityIterator2::nPolarizationIds () const
{
    CheckImplementationPointer ();
    return impl_p->nPolarizationIds ();
}
//
Int
VisibilityIterator2::nSpectralWindows () const
{
    CheckImplementationPointer ();
    return impl_p->nSpectralWindows ();
}

void
VisibilityIterator2::origin ()
{
    CheckImplementationPointer ();
    impl_p->origin ();
}

void
VisibilityIterator2::originChunks ()
{
    CheckImplementationPointer ();
    originChunks (false);
}

void
VisibilityIterator2::originChunks (Bool forceRewind)
{
    CheckImplementationPointer ();
    impl_p->originChunks (forceRewind);
}

void
VisibilityIterator2::setRowBlocking (rownr_t nRows) // for use by Async I/O *ONLY
{
    CheckImplementationPointer ();
    impl_p->setRowBlocking (nRows);
}

void
VisibilityIterator2::slurp () const
{
    CheckImplementationPointer ();
    impl_p->slurp ();
}

rownr_t
VisibilityIterator2::nRowsInChunk () const
{
    CheckImplementationPointer ();
    return impl_p->nRowsInChunk ();
}

void
VisibilityIterator2::setFrequencySelection (const FrequencySelection & selection)
{
    FrequencySelections selections;
    selections.add (selection);
    setFrequencySelection (selections);
}

void
VisibilityIterator2::setFrequencySelection (const FrequencySelections & selections)
{
    ThrowIf (selections.size () != impl_p->getNMs (),
             String::format ("Frequency selection size, %d, does not VisibilityIterator # of MSs, %d.",
                     impl_p->getNMs (), selections.size()));

    CheckImplementationPointer ();
    impl_p->setFrequencySelections (selections);
}



const vi::SubtableColumns &
VisibilityIterator2::subtableColumns () const
{
    CheckImplementationPointer ();
    return impl_p->subtableColumns ();
}

void
VisibilityIterator2::useImagingWeight (const VisImagingWeight & viw)
{
    CheckImplementationPointer ();
    impl_p->useImagingWeight(viw);
}

const VisImagingWeight & 
VisibilityIterator2::getImagingWeightGenerator () const
{
    CheckImplementationPointer ();
    return impl_p->getImagingWeightGenerator ();
}

void
VisibilityIterator2::writeFlag (const Cube<Bool>& flag)
{
    CheckImplementationPointer ();
    impl_p->writeFlag (flag);
}

void
VisibilityIterator2::writeFlagRow (const Vector<Bool>& rowflags)
{
    CheckImplementationPointer ();
    impl_p->writeFlagRow (rowflags);
}

void
VisibilityIterator2::writeWeightSpectrum (const Cube<Float>& wtsp)
{
    CheckImplementationPointer ();
    impl_p->writeWeightSpectrum (wtsp);
}

void
VisibilityIterator2::writeVisModel (const Cube<Complex>& modelCube)
{
    CheckImplementationPointer ();
    impl_p->writeVisModel (modelCube);
}

void
VisibilityIterator2::writeVisCorrected (const Cube<Complex>& CorrectedCube)
{
    CheckImplementationPointer ();
    impl_p->writeVisCorrected (CorrectedCube);
}

void
VisibilityIterator2::writeModel(const RecordInterface& record,
                                bool isComponentList,
                                bool addToExistingModel)
{
    CheckImplementationPointer ();
    impl_p->writeModel (record, isComponentList, addToExistingModel);
}

void
VisibilityIterator2::writeVisObserved (const Cube<Complex>& ObservedCube)
{
    CheckImplementationPointer ();
    impl_p->writeVisObserved (ObservedCube);
}

//
void
VisibilityIterator2::initWeightSpectrum (const Cube<Float>& wtsp)
{
    CheckImplementationPointer ();
    impl_p->initWeightSpectrum (wtsp);
}

void
VisibilityIterator2::setWeightScaling (CountedPtr<WeightScaling> weightScaling)
{
  CheckImplementationPointer ();

  impl_p->setWeightScaling (weightScaling);
}

Bool
VisibilityIterator2::hasWeightScaling () const
{
  CheckImplementationPointer ();

  return impl_p->hasWeightScaling ();
}

const casacore::MSAntennaColumns& VisibilityIterator2::antennaSubtablecols() const
{
  CheckImplementationPointer ();

  return impl_p->antennaSubtablecols ();
}

const casacore::MSDataDescColumns& VisibilityIterator2::dataDescriptionSubtablecols() const
{
  CheckImplementationPointer ();

  return impl_p->dataDescriptionSubtablecols ();
}

const casacore::MSFeedColumns& VisibilityIterator2::feedSubtablecols() const
{
  CheckImplementationPointer ();

  return impl_p->feedSubtablecols ();
}

const casacore::MSFieldColumns& VisibilityIterator2::fieldSubtablecols() const
{
  CheckImplementationPointer ();

  return impl_p->fieldSubtablecols ();
}

const casacore::MSFlagCmdColumns& VisibilityIterator2::flagCmdSubtablecols() const
{
  CheckImplementationPointer ();

  return impl_p->flagCmdSubtablecols ();
}

const casacore::MSHistoryColumns& VisibilityIterator2::historySubtablecols() const
{
  CheckImplementationPointer ();

  return impl_p->historySubtablecols ();
}

const casacore::MSObservationColumns& VisibilityIterator2::observationSubtablecols() const
{
  CheckImplementationPointer ();

  return impl_p->observationSubtablecols ();
}

const casacore::MSPointingColumns& VisibilityIterator2::pointingSubtablecols() const
{
  CheckImplementationPointer ();

  return impl_p->pointingSubtablecols ();
}

const casacore::MSPolarizationColumns& VisibilityIterator2::polarizationSubtablecols() const
{
  CheckImplementationPointer ();

  return impl_p->polarizationSubtablecols ();
}

const casacore::MSProcessorColumns& VisibilityIterator2::processorSubtablecols() const
{
  CheckImplementationPointer ();

  return impl_p->processorSubtablecols ();
}

const casacore::MSSpWindowColumns& VisibilityIterator2::spectralWindowSubtablecols() const
{
  CheckImplementationPointer ();

  return impl_p->spectralWindowSubtablecols ();
}

const casacore::MSStateColumns& VisibilityIterator2::stateSubtablecols() const
{
  CheckImplementationPointer ();

  return impl_p->stateSubtablecols ();
}

const casacore::MSDopplerColumns& VisibilityIterator2::dopplerSubtablecols() const
{
  CheckImplementationPointer ();

  return impl_p->dopplerSubtablecols ();
}

const casacore::MSFreqOffsetColumns& VisibilityIterator2::freqOffsetSubtablecols() const
{
  CheckImplementationPointer ();

  return impl_p->freqOffsetSubtablecols ();
}

const casacore::MSSourceColumns& VisibilityIterator2::sourceSubtablecols() const
{
  CheckImplementationPointer ();

  return impl_p->sourceSubtablecols ();
}

const casacore::MSSysCalColumns& VisibilityIterator2::sysCalSubtablecols() const
{
  CheckImplementationPointer ();

  return impl_p->sysCalSubtablecols ();
}

const casacore::MSWeatherColumns& VisibilityIterator2::weatherSubtablecols() const
{
  CheckImplementationPointer ();

  return impl_p->weatherSubtablecols ();
}

SubtableColumns::SubtableColumns (CountedPtr <MSIter> msIter)
: msIter_p (msIter)
{}

const MSAntennaColumns&
SubtableColumns::antenna() const
{
    return msIter_p->msColumns().antenna();
}

const MSDataDescColumns&
SubtableColumns::dataDescription() const
{
    return msIter_p->msColumns().dataDescription();
}

const MSFeedColumns&
SubtableColumns::feed() const
{
    return msIter_p->msColumns().feed();
}

const MSFieldColumns&
SubtableColumns::field() const
{
    return msIter_p->msColumns().field();
}

const MSFlagCmdColumns&
SubtableColumns::flagCmd() const
{
    return msIter_p->msColumns().flagCmd();
}

const MSHistoryColumns&
SubtableColumns::history() const
{
    return msIter_p->msColumns().history();
}

const MSObservationColumns&
SubtableColumns::observation() const
{
    return msIter_p->msColumns().observation();
}

const MSPointingColumns&
SubtableColumns::pointing() const
{
    return msIter_p->msColumns().pointing();
}

const MSPolarizationColumns&
SubtableColumns::polarization() const
{

    return msIter_p->msColumns().polarization();
}

const MSProcessorColumns&
SubtableColumns::processor() const
{
    return msIter_p->msColumns().processor();
}

const MSSpWindowColumns&
SubtableColumns::spectralWindow() const
{

    return msIter_p->msColumns().spectralWindow();
}

const MSStateColumns&
SubtableColumns::state() const
{
    return msIter_p->msColumns().state();
}

const MSDopplerColumns&
SubtableColumns::doppler() const
{
    return msIter_p->msColumns().doppler();
}

const MSFreqOffsetColumns&
SubtableColumns::freqOffset() const
{
    return msIter_p->msColumns().freqOffset();
}

const MSSourceColumns&
SubtableColumns::source() const
{
    return msIter_p->msColumns().source();
}

const MSSysCalColumns&
SubtableColumns::sysCal() const
{
    return msIter_p->msColumns().sysCal();
}

const MSWeatherColumns&
SubtableColumns::weather() const
{
    return msIter_p->msColumns().weather();
}


} // end namespace vi


using namespace casacore;
} // end namespace casa
