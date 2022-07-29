/*
 * VisBufferImpl.cc
 *
 *  Created on: Jul 3, 2012
 *      Author: jjacobs
 */


//#include <casa/Arrays/ArrayLogical.h>
//#include <casa/Arrays/ArrayMath.h>
//#include <casa/Arrays/ArrayUtil.h>
//#include <casa/Arrays/MaskArrMath.h>
//#include <casa/Arrays/MaskedArray.h>
#include <casacore/casa/OS/Path.h>
#include <casacore/casa/OS/Timer.h>
#include <casacore/casa/Utilities.h>
#include <casacore/casa/aipstype.h>
#include <typeinfo>
#include <components/ComponentModels/ComponentList.h>
#include <casacore/ms/MeasurementSets/MSColumns.h>
#include <stdcasa/UtilJ.h>
#include <msvis/MSVis/VisBufferAsyncWrapper2.h>
#include <msvis/MSVis/VisBufferComponents2.h>
#include <msvis/MSVis/VisBufferComponents2.h>
#include <msvis/MSVis/VisBufferImpl2.h>
#include <msvis/MSVis/VisBuffer2Adapter.h>
#include <msvis/MSVis/VisibilityIterator2.h>
#include <msvis/MSVis/Vbi2MsRow.h>
#include <msvis/MSVis/VisModelDataI.h>
#include <msvis/MSVis/VisBufferImpl2Internal.h>


#define CheckVisIter() checkVisIter (__func__, __FILE__, __LINE__)
#define CheckVisIter1(s) checkVisIter (__func__, __FILE__, __LINE__,s)
#define CheckVisIterBase() checkVisIterBase (__func__, __FILE__, __LINE__)

#include <functional>

using namespace std;

using namespace casacore;
namespace casa {

using namespace casacore;
using namespace casa::ms;

namespace vi {



VisBufferCache::VisBufferCache (VisBufferImpl2 * vb)
: imagingWeight_p (true)
{
    ThrowIf (vb == NULL, "VisBufferCacheImpl not connected to VisBufferImpl2");
}

void
VisBufferCache::initialize (VisBufferImpl2 * vb)
{

    // Initialize the cache items.  This will also register them
    // with the vb object to allow it to iterate over the cached
    // values.

    antenna1_p.initialize (this, vb, & VisBufferImpl2::fillAntenna1,
                           VisBufferComponent2::Antenna1, Nr, true);
    antenna2_p.initialize (this, vb, &VisBufferImpl2::fillAntenna2,
                           VisBufferComponent2::Antenna2, Nr, true);
    arrayId_p.initialize (this, vb, &VisBufferImpl2::fillArrayId,
                          VisBufferComponent2::ArrayId, Nr, true);
    cjones_p.initialize (this, vb, &VisBufferImpl2::fillJonesC,
                         VisBufferComponent2::JonesC, NoCheck, false);
    correctedVisCube_p.initialize (this, vb, &VisBufferImpl2::fillCubeCorrected,
                                   VisBufferComponent2::VisibilityCubeCorrected,
                                   NcNfNr, false);
    correctedVisCubes_p.initialize (this, vb, &VisBufferImpl2::fillCubesCorrected,
                                    VisBufferComponent2::VisibilityCubesCorrected,
                                    NsNcNfNr, false);
    corrType_p.initialize (this, vb, &VisBufferImpl2::fillCorrType,
                           VisBufferComponent2::CorrType, NoCheck, false);
    dataDescriptionIds_p.initialize (this, vb, &VisBufferImpl2::fillDataDescriptionIds,
                                     VisBufferComponent2::DataDescriptionIds, Nr, true);
    direction1_p.initialize (this, vb, &VisBufferImpl2::fillDirection1,
                             VisBufferComponent2::Direction1, NoCheck, false);
    direction2_p.initialize (this, vb, &VisBufferImpl2::fillDirection2,
                             VisBufferComponent2::Direction2, NoCheck, false);
    exposure_p.initialize (this, vb, &VisBufferImpl2::fillExposure,
                           VisBufferComponent2::Exposure, Nr, false);
    feed1_p.initialize (this, vb, &VisBufferImpl2::fillFeed1,
                        VisBufferComponent2::Feed1, Nr, false);
    feed1Pa_p.initialize (this, vb, &VisBufferImpl2::fillFeedPa1,
                          VisBufferComponent2::FeedPa1, NoCheck, false);
    feed2_p.initialize (this, vb, &VisBufferImpl2::fillFeed2,
                        VisBufferComponent2::Feed2, Nr, false);
    feed2Pa_p.initialize (this, vb, &VisBufferImpl2::fillFeedPa2,
                          VisBufferComponent2::FeedPa2, NoCheck, false);
    fieldId_p.initialize (this, vb, &VisBufferImpl2::fillFieldId,
                          VisBufferComponent2::FieldId, Nr, true);
    flagCategory_p.initialize (this, vb, &VisBufferImpl2::fillFlagCategory,
                               VisBufferComponent2::FlagCategory, NoCheck, false);
    // required column but not used in casa, make it a nocheck for shape validation
    flagCube_p.initialize (this, vb, &VisBufferImpl2::fillFlagCube,
                           VisBufferComponent2::FlagCube, NcNfNr, false);
    flagCubes_p.initialize (this, vb, &VisBufferImpl2::fillFlagCubes,
                           VisBufferComponent2::FlagCubes, NsNcNfNr, false);
    flagRow_p.initialize (this, vb, &VisBufferImpl2::fillFlagRow,
                          VisBufferComponent2::FlagRow, Nr, false);
    floatDataCube_p.initialize (this, vb, &VisBufferImpl2::fillFloatData,
                                VisBufferComponent2::VisibilityCubeFloat, NcNfNr, false);
    floatDataCubes_p.initialize (this, vb, &VisBufferImpl2::fillFloatCubes,
                                 VisBufferComponent2::VisibilityCubesFloat, NsNcNfNr, false);
    imagingWeight_p.initialize (this, vb, &VisBufferImpl2::fillImagingWeight,
                                VisBufferComponent2::ImagingWeight, NoCheck, false);
    modelVisCube_p.initialize (this, vb, &VisBufferImpl2::fillCubeModel,
                               VisBufferComponent2::VisibilityCubeModel, NcNfNr, false);
    modelVisCubes_p.initialize (this, vb, &VisBufferImpl2::fillCubesModel,
                               VisBufferComponent2::VisibilityCubesModel, NsNcNfNr, false);
    nAntennas_p.initialize (this, vb, &VisBufferImpl2::fillNAntennas,
                            VisBufferComponent2::NAntennas, false);
    nChannels_p.initialize (this, vb, &VisBufferImpl2::fillNChannel,
                            VisBufferComponent2::NChannels, false);
    nCorrelations_p.initialize (this, vb, &VisBufferImpl2::fillNCorr,
                                VisBufferComponent2::NCorrelations);
    nRows_p.initialize (this, vb, &VisBufferImpl2::fillNRow,
                        VisBufferComponent2::NRows, false);
    nShapes_p.initialize (this, vb, &VisBufferImpl2::fillNShapes,
                          VisBufferComponent2::NShapes, false);
    nRowsPerShape_p.initialize (this, vb, &VisBufferImpl2::fillNRowPerShape,
                                VisBufferComponent2::NRowsPerShape, Ns, false);
    nChannelsPerShape_p.initialize (this, vb, &VisBufferImpl2::fillNChannelPerShape,
                                    VisBufferComponent2::NChannelsPerShape, Ns, false);
    nCorrelationsPerShape_p.initialize (this, vb, &VisBufferImpl2::fillNCorrPerShape,
                                        VisBufferComponent2::NCorrelationsPerShape, Ns, false);
    observationId_p.initialize (this, vb, &VisBufferImpl2::fillObservationId,
                                VisBufferComponent2::ObservationId, Nr, true);
    phaseCenter_p.initialize (this, vb, &VisBufferImpl2::fillPhaseCenter,
                              VisBufferComponent2::PhaseCenter, false);
    polFrame_p.initialize (this, vb, &VisBufferImpl2::fillPolFrame,
                           VisBufferComponent2::PolFrame, false);
    polarizationId_p.initialize (this, vb, &VisBufferImpl2::fillPolarizationId,
                                 VisBufferComponent2::PolarizationId, false);
    processorId_p.initialize (this, vb, &VisBufferImpl2::fillProcessorId,
                              VisBufferComponent2::ProcessorId, Nr, true);
    rowIds_p.initialize (this, vb, &VisBufferImpl2::fillRowIds,
                         VisBufferComponent2::RowIds, Nr, false);
    scan_p.initialize (this, vb, &VisBufferImpl2::fillScan,
                       VisBufferComponent2::Scan, Nr, true);
    sigma_p.initialize (this, vb, &VisBufferImpl2::fillSigma,
                        VisBufferComponent2::Sigma, NcNr, false);
    sigmas_p.initialize (this, vb, &VisBufferImpl2::fillSigmas,
                         VisBufferComponent2::Sigma, NsNcNr, false);
    spectralWindows_p.initialize (this, vb, &VisBufferImpl2::fillSpectralWindows,
                                  VisBufferComponent2::SpectralWindows, Nr, false);
    stateId_p.initialize (this, vb, &VisBufferImpl2::fillStateId,
                          VisBufferComponent2::StateId, Nr, true);
    time_p.initialize (this, vb, &VisBufferImpl2::fillTime,
                       VisBufferComponent2::Time, Nr, true);
    timeCentroid_p.initialize (this, vb, &VisBufferImpl2::fillTimeCentroid,
                               VisBufferComponent2::TimeCentroid, Nr, false);
    timeInterval_p.initialize (this, vb, &VisBufferImpl2::fillTimeInterval,
                               VisBufferComponent2::TimeInterval, Nr, false);
    uvw_p.initialize (this, vb, &VisBufferImpl2::fillUvw,
                      VisBufferComponent2::Uvw, I3Nr, false);
    visCube_p.initialize (this, vb, &VisBufferImpl2::fillCubeObserved,
                          VisBufferComponent2::VisibilityCubeObserved, NcNfNr, false);
    visCubes_p.initialize (this, vb, &VisBufferImpl2::fillCubesObserved,
                           VisBufferComponent2::VisibilityCubesObserved, NsNcNfNr, false);
    weight_p.initialize (this, vb, &VisBufferImpl2::fillWeight,
                         VisBufferComponent2::Weight, NcNr, false);
    weights_p.initialize (this, vb, &VisBufferImpl2::fillWeights,
                          VisBufferComponent2::Weights, NsNcNr, false);
    weightSpectrum_p.initialize (this, vb, &VisBufferImpl2::fillWeightSpectrum,
                                 VisBufferComponent2::WeightSpectrum,
                                 NcNfNr, false);
    weightSpectra_p.initialize (this, vb, &VisBufferImpl2::fillWeightSpectra,
                                 VisBufferComponent2::WeightSpectra,
                                 NsNcNfNr, false);
    sigmaSpectrum_p.initialize (this, vb, &VisBufferImpl2::fillSigmaSpectrum,
                                VisBufferComponent2::SigmaSpectrum,
                                NcNfNr, false);
    sigmaSpectra_p.initialize (this, vb, &VisBufferImpl2::fillSigmaSpectra,
                                VisBufferComponent2::SigmaSpectra,
                                NsNcNfNr, false);
}

void
VisBufferCache::registerItem (VbCacheItemBase * item)
{
    registry_p.push_back (item);
}

void
VbCacheItemBase::initialize (VisBufferCache * cache, VisBufferImpl2 * vb, VisBufferComponent2 component, Bool isKey)
{
    vbComponent_p = component;
    vb_p = vb;
    cache->registerItem (this);
    setIsKey (isKey);
}


using namespace vi;

////////////////////////////////////////////////////////////
//
// Basic VisBufferImpl2 Methods
// ========================
//
// Other sections contain the accessor and filler methods

VisBufferImpl2::VisBufferImpl2 (VisBufferOptions options)
: cache_p (0), msRow_p (0), state_p (0)
{
    construct (0, options);
}

VisBufferImpl2::VisBufferImpl2(ViImplementation2 * vii, VisBufferOptions options)
: cache_p (0),
  msRow_p (0),
  state_p (0)
{
  construct (vii, options);
}

VisBufferImpl2::~VisBufferImpl2 ()
{
    delete cache_p;
    delete msRow_p;
    delete state_p;
}

void
VisBufferImpl2::appendRow (Vbi2MsRow * rowSrc,
                           Int initialCapacity,
                           const VisBufferComponents2 & optionalComponentsToCopy,
                           bool doCopy)
{
    // First check to see if the buffer needs to be reconfigured before another
    // row can be appended.

//    if (state_p->appendSize_p == 0 ||
//        state_p->appendSize_p == state_p->appendCapacity_p){
//
//        if (msRow_p == 0){
//            msRow_p = new Vbi2MsRow (0, this);
//        }
//
//        // The buffer is either empty or completely full; resize it so
//        // that there is room to add more rows.
//
//        Int nRowsToAdd;
//
//        if (state_p->appendSize_p == 0){
//
//            // If appendSize is zero then the buffer is completely empty.
//            // If capacity is nonzero then reuse that capacity.
//
//            nRowsToAdd = state_p->appendCapacity_p; // nonzero if buffer is being reused
//
//            if (nRowsToAdd == 0){
//                nRowsToAdd = initialCapacity; // new buffer: use provided initial capacity
//            }
//
//            state_p->appendCapacity_p = nRowsToAdd;
//        }
//        else{
//
//            // All rows in use so add 40% more rows
//
//            nRowsToAdd = (state_p->appendCapacity_p * 4) / 10;
//            state_p->appendCapacity_p += nRowsToAdd;
//        }
//
//        for (CacheRegistry::iterator i = cache_p->registry_p.begin();
//             i != cache_p->registry_p.end();
//             i++){
//
//            // Resize the various arrays so that they have the appropriate
//            // row dimension.
//
//            (*i)->appendRows (state_p->appendCapacity_p);
//        }
//    }

    // Now append the row.

//    Assert (state_p->appendSize_p <= cache_p->time_p.getItem().shape()(0));

    if (msRow_p == 0){
        msRow_p = new Vbi2MsRow (0, this);
    }


    state_p->appendSize_p ++;

    if (state_p->appendSize_p >= cache_p->time_p.getItem().shape().last()){

        Int desiredNRows = max (state_p->appendSize_p, initialCapacity);

        for (CacheRegistry::iterator i = cache_p->registry_p.begin();
                i != cache_p->registry_p.end();
                i++){

            // Resize the various arrays so that they have the appropriate
            // row dimension.

            (*i)->appendRows (desiredNRows);
        }
    }

    // Now copy the provided row into the next one in this buffer.

    if (doCopy){

        msRow_p->changeRow (state_p->appendSize_p - 1);

        msRow_p->copy (rowSrc, optionalComponentsToCopy);
    }
}

void
VisBufferImpl2::appendRowsComplete ()
{
    // Close out the append operation by trimming off the
    // excess baselines in the buffer.

    for (CacheRegistry::iterator i = cache_p->registry_p.begin();
            i != cache_p->registry_p.end();
            i++){

        (*i)->appendRows (state_p->appendSize_p, true);
    }

    cache_p->nRows_p.setSpecial (state_p->appendSize_p);
    //Assume that the append happens on the first shape only
    Vector<rownr_t> nRowsPerShape = cache_p->nRowsPerShape_p.get();
    nRowsPerShape[0] = state_p->appendSize_p;
    cache_p->nRowsPerShape_p.setSpecial (nRowsPerShape);

    state_p->appendSize_p = 0; //
}

Int
VisBufferImpl2::appendSize () const
{
    return state_p->appendSize_p;
}



Bool
VisBufferImpl2::areCorrelationsInCanonicalOrder () const
{
  Vector<Int> corrs(correlationTypes());

  // Only a meaningful question is all 4 corrs present

  Bool result = corrs.nelements () == 4 &&
                (corrs (1) == Stokes::LL || corrs (1) == Stokes::YY);

  return result;
}

Bool
VisBufferImpl2::areCorrelationsSorted() const
{
    return state_p->areCorrelationsSorted_p = false;
}

void
VisBufferImpl2::checkVisIter (const char * func, const char * file, int line, const char * extra) const
{
  checkVisIterBase (func, file, line, extra);
}

void
VisBufferImpl2::checkVisIterBase (const char * func, const char * file, int line, const char * extra) const
{
  if (! state_p->isAttached_p) {
    throw AipsError (String ("VisBuffer not attached to a ViImplementation2 while filling this field in (") +
                     func + extra + ")", file, line);
  }

  if (getViiP() == NULL){
    throw AipsError (String ("VisBuffer's ViImplementation2 is NULL while filling this field in (") +
                     func + extra + ")", file, line);
  }
}

void
VisBufferImpl2::cacheClear (Bool markAsCached)
{
    for (CacheRegistry::iterator i = cache_p->registry_p.begin();
         i != cache_p->registry_p.end();
         i++){

        (*i)->clear (false);

        if (markAsCached){
            (*i)->setAsPresent ();
        }
    }
}

void
VisBufferImpl2::cacheResizeAndZero (const VisBufferComponents2 & components)
{
    Bool doAll = components.empty();

    for (CacheRegistry::iterator i = cache_p->registry_p.begin();
         i != cache_p->registry_p.end();
         i++){

        if (! doAll && ! components.contains((*i)->getComponent())){
            continue; // this one is excluded
        }

       (*i)->resize (false);

       (*i)->setAsPresent();
    }
}



void
VisBufferImpl2::construct (ViImplementation2 * vi, VisBufferOptions options)
{
    state_p = new VisBufferState ();

    // Initialize all non-object member variables

    state_p->areCorrelationsSorted_p = false; // Have correlations been sorted by sortCorr?

    state_p->isAttached_p = vi != 0;
    state_p->pointingTableLastRow_p = -1;
    state_p->newMs_p = true;
    state_p->vi_p = vi;

    // Handle the options

    state_p->isWritable_p = false;
    state_p->isRekeyable_p = false;

    if (options & VbRekeyable){

        state_p->isWritable_p = true; // rekeyable implies writable
        state_p->isRekeyable_p = true;
    }
    else if (options & VbWritable){
        state_p->isWritable_p = true;
    }

    cache_p = new VisBufferCache (this);
    cache_p->initialize (this);
}

void
VisBufferImpl2::copy (const VisBuffer2 & otherRaw, Bool fetchIfNeeded)
{
    copyComponents (otherRaw, VisBufferComponents2::exceptThese({VisBufferComponent2::FlagCategory}),
                    true, fetchIfNeeded);
}

void
VisBufferImpl2::copyComponents (const VisBuffer2 & otherRaw,
                                const VisBufferComponents2 & components,
                                Bool allowShapeChange,
                                Bool fetchIfNeeded)
{
    const VisBufferImpl2 * other = dynamic_cast<const VisBufferImpl2 *> (& otherRaw);

    ThrowIf (other == 0,
             String::format ("Copy between %s and VisBufferImpl2 not implemented.",
                             typeid (otherRaw).name()));

    if (! hasShape()){

        // If this VB is shapeless, then assume the shape of the source VB.

        setShape (otherRaw.nCorrelations(), otherRaw.nChannels(), otherRaw.nRows(), true);
    }
    else if (allowShapeChange && getShape() != otherRaw.getShape()){

        setShape (otherRaw.nCorrelations(), otherRaw.nChannels(), otherRaw.nRows(), true);
    }

    setIterationInfo (other->msId(),
                      other->msName (),
                      other->isNewMs (),
                      other->isNewArrayId (),
                      other->isNewFieldId (),
                      other->isNewSpectralWindow (),
                      other->getSubchunk (),
                      other->getCorrelationTypes(),
                      other->getCorrelationTypesDefined(),
                      other->getCorrelationTypesSelected(),
                      other->getWeightScaling());

    Bool wasFillable = isFillable();
    setFillable (true);

    for (CacheRegistry::iterator dst = cache_p->registry_p.begin(),
                                 src = other->cache_p->registry_p.begin();
         dst != cache_p->registry_p.end();
         dst ++, src ++){

        if (components.contains ((* src)->getComponent())){

            try {

                (* dst)->copy (* src, fetchIfNeeded);
            }
            catch (AipsError & e){

                String m =  String::format ("While copying %s",
                                            VisBufferComponents2::name ((* src)->getComponent()).c_str());
                Rethrow (e, m);
            }
        }
    }

    setFillable (wasFillable);
}

void
VisBufferImpl2::copyCoordinateInfo (const VisBuffer2 * vb, Bool dirDependent,
                                    Bool allowShapeChange, Bool fetchIfNeeded)
{

    VisBufferComponents2 components =
        VisBufferComponents2::these ({VisBufferComponent2::Antenna1,
                                      VisBufferComponent2::Antenna2,
                                      VisBufferComponent2::ArrayId,
                                      VisBufferComponent2::DataDescriptionIds,
                                      VisBufferComponent2::FieldId,
                                      VisBufferComponent2::SpectralWindows,
                                      VisBufferComponent2::Time,
                                      VisBufferComponent2::NRows,
                                      VisBufferComponent2::Feed1,
                                      VisBufferComponent2::Feed2});

    copyComponents (* vb, components, allowShapeChange, fetchIfNeeded);

    if(dirDependent){

        VisBufferComponents2 components =
	    VisBufferComponents2::these ({VisBufferComponent2::Direction1,
                                      VisBufferComponent2::Direction2,
                                      VisBufferComponent2::FeedPa1,
                                      VisBufferComponent2::FeedPa2});

        copyComponents (* vb, components, allowShapeChange, fetchIfNeeded);
    }
}

void
VisBufferImpl2::copyRow (Int sourceRow, Int destinationRow)
{
    for (CacheRegistry::iterator i = cache_p->registry_p.begin();
         i != cache_p->registry_p.end();
         i++){

        if (! (* i)->isPresent()){
            continue;
        }

        (* i)->copyRowElement (sourceRow, destinationRow);

    }

}

void
VisBufferImpl2::dirtyComponentsAdd (const VisBufferComponents2 & additionalDirtyComponents)
{
    // Loop through all of the cached VB components and mark them dirty if
    // they're in the set <of addition dirty components

    for (CacheRegistry::iterator i = cache_p->registry_p.begin();
         i != cache_p->registry_p.end();
         i++){

        if (additionalDirtyComponents.contains ((* i)->getComponent())){

            (* i)->setDirty ();
        }
    }
}

void
VisBufferImpl2::dirtyComponentsAdd (VisBufferComponent2 component)
{
    dirtyComponentsAdd (VisBufferComponents2::singleton (component));
}


void
VisBufferImpl2::dirtyComponentsClear ()
{
    for (CacheRegistry::iterator i = cache_p->registry_p.begin();
         i != cache_p->registry_p.end();
         i++){

        (* i)->clearDirty ();
    }
}

VisBufferComponents2
VisBufferImpl2::dirtyComponentsGet () const
{
    VisBufferComponents2 dirtyComponents;

    for (CacheRegistry::iterator i = cache_p->registry_p.begin();
         i != cache_p->registry_p.end();
         i++){

        if ((*i)->isDirty ()){

            VisBufferComponent2 component = (* i)->getComponent ();

            if (component != VisBufferComponent2::Unknown){
                dirtyComponents = dirtyComponents +
                                  VisBufferComponents2::singleton (component);
            }
        }
    }

    return dirtyComponents;
}

void
VisBufferImpl2::dirtyComponentsSet (const VisBufferComponents2 & dirtyComponents)
{
    // Clear the dirty state for the cache

    cacheClear ();

    // Add in the specified components to the newly cleared cache.

    dirtyComponentsAdd (dirtyComponents);
}

void
VisBufferImpl2::dirtyComponentsSet (VisBufferComponent2 component)
{
    dirtyComponentsSet (VisBufferComponents2::singleton (component));
}


Bool
VisBufferImpl2::isAttached () const
{
    return state_p->isAttached_p;
}

Bool
VisBufferImpl2::isFillable () const
{
    return state_p->isFillable_p;
}


Int
VisBufferImpl2::msId () const
{
    return state_p->msId_p;
}

String
VisBufferImpl2::msName (Bool stripPath) const
{
    String result;

    if(stripPath){

      Path path(state_p->msName_p);
      result = path.baseName();
    }
    else{
        result = state_p->msName_p;
    }

    return result;
}

Double
VisBufferImpl2::getFrequency (Int rowInBuffer, Int frequencyIndex, Int frame) const
{
    if (frame == FrameNotSpecified){
        frame = getViiP()->getReportingFrameOfReference();
    }

    state_p->frequencies_p.updateCacheIfNeeded (getViiP(), rowInBuffer, frame, this);

    return state_p->frequencies_p.values_p (frequencyIndex);
}

const Vector<Double> &
VisBufferImpl2::getFrequencies (Int rowInBuffer, Int frame) const
{
    if (frame == FrameNotSpecified){
        frame = getViiP()->getReportingFrameOfReference();
    }

    state_p->frequencies_p.updateCacheIfNeeded (getViiP(), rowInBuffer, frame, this);

    return state_p->frequencies_p.values_p;
}

Int
VisBufferImpl2::getChannelNumber (Int rowInBuffer, Int frequencyIndex) const
{
    state_p->channelNumbers_p.updateCacheIfNeeded (getViiP(), rowInBuffer, 0, this);

    return state_p->channelNumbers_p.values_p (frequencyIndex);
}

const Vector<Int> &
VisBufferImpl2::getChannelNumbers (Int rowInBuffer) const
{
    state_p->channelNumbers_p.updateCacheIfNeeded (getViiP(), rowInBuffer, 0, this);

    return state_p->channelNumbers_p.values_p;
}

Vector<Int> VisBufferImpl2::getChannelNumbersSelected (Int outputChannelIndex) const
{
    return Vector<Int>(1,outputChannelIndex);
}

Vector<Stokes::StokesTypes>
VisBufferImpl2::getCorrelationTypesDefined () const
{
    return state_p->correlationsDefined_p.copy();
}

Vector<Stokes::StokesTypes>
VisBufferImpl2::getCorrelationTypesSelected () const
{
    return state_p->correlationsSelected_p.copy();
}


Vector<Int>
VisBufferImpl2::getCorrelationTypes () const
{
	return state_p->correlations_p.copy();
}


String
VisBufferImpl2::getFillErrorMessage () const
{
    return "VisBuffer not positioned to a subchunk";
}


Subchunk
VisBufferImpl2::getSubchunk () const
{
    return state_p->subchunk_p;
}

IPosition
VisBufferImpl2::getValidShape (Int i) const
{
    ThrowIf (i < 0 || i >= (int) state_p->validShapes_p.nelements(),
             String::format ("Invalid shape requested: %d", i));

    return state_p->validShapes_p (i);
}

Vector<IPosition>
VisBufferImpl2::getValidVectorShapes (Int i) const
{
    ThrowIf (i < 0 || i >= (int) state_p->validVectorShapes_p.nelements(),
             String::format ("Invalid shape requested: %d", i));

    return state_p->validVectorShapes_p (i);
}

ViImplementation2 *
VisBufferImpl2::getViiP () const
{
    ThrowIf (state_p->vi_p == nullptr, "Null pointer dereference!");

    return state_p->vi_p;
}

VisModelDataI *
VisBufferImpl2::getVisModelData () const
{
    return state_p->visModelData_p;
}

IPosition
VisBufferImpl2::getShape () const
{
    IPosition shape;

    if (hasShape()){
        shape = IPosition (3, cache_p->nCorrelations_p.get(), cache_p->nChannels_p.get(),
                           cache_p->nRows_p.get());
    }
    else{
        shape = IPosition ();
    }

    return shape;
}


Bool
VisBufferImpl2::hasShape () const
{
    Bool hasShape = cache_p->nCorrelations_p.isPresent() && cache_p->nCorrelations_p.get() > 0;
    hasShape = hasShape && cache_p->nChannels_p.isPresent() && cache_p->nChannels_p.get() > 0;
    hasShape = hasShape && cache_p->nRows_p.isPresent() && cache_p->nRows_p.get() > 0;

    return hasShape;
}

void
VisBufferImpl2::invalidate ()
{
    cacheClear (false); // empty cached values
    setFillable (false); // buffer is in limbo
}

Bool
VisBufferImpl2::isNewArrayId () const
{
    return state_p->isNewArrayId_p;
}

Bool
VisBufferImpl2::isNewFieldId () const
{
    return state_p->isNewFieldId_p;
}

Bool
VisBufferImpl2::isNewMs() const
{
    return state_p->isNewMs_p;
}

Bool
VisBufferImpl2::isNewSpectralWindow () const
{
    return state_p->isNewSpectralWindow_p;
}

Bool
VisBufferImpl2::isRekeyable () const
{
    return state_p->isRekeyable_p;
}


Bool
VisBufferImpl2::isWritable () const
{
    return state_p->isWritable_p;
}

void
VisBufferImpl2::normalize()
{
    // NB: Handles pol-dep weights in chan-indep way

    // Check for missing data

    ThrowIf (! cache_p->visCube_p.isPresent(),
             "Cannot normalize; visCube is missing.");
    ThrowIf (! cache_p->modelVisCube_p.isPresent (),
             "Cannot normalize; modelVisCube is missing.");
    ThrowIf (! cache_p->weight_p.isPresent(),
             "Cannot normalize; weightMap is missing.");

    // Get references to the cached values to be used in the
    // normalization.

    Int nCorrelations = this->nCorrelations ();

    const Vector<Bool> & rowFlagged = cache_p->flagRow_p.get ();
    const Cube<Bool> & flagged = cache_p->flagCube_p.get ();

    Cube<Complex> & visCube = cache_p->visCube_p.getRef();
    Cube<Complex> & modelCube = cache_p->modelVisCube_p.getRef();
    Matrix<Float> & weight = cache_p->weight_p.getRef();

    // Normalize each row.

    for (rownr_t row = 0; row < nRows (); row++) {

        if (rowFlagged (row)){

            weight.column(row) = 0.0f; // Zero weight on this flagged row
            continue;
        }

        normalizeRow (row, nCorrelations, flagged, visCube,
                      modelCube, weight);

    }
}

// Rotate visibility phase for given vector (dim = nrow of vb) of phases (metres)
void
VisBufferImpl2::phaseCenterShift(const Vector<Double>& phase)
{
	// Use the frequencies corresponding to the SPW of the first row in the buffer
	Vector<Double> freq(getFrequencies(0));

	// Set vis cube references
	Cube<Complex> visCubeRef;
	Cube<Complex> visCubeModelRef;
	Cube<Complex> visCubeCorrectedRef;
	if (cache_p->visCube_p.isPresent())
	{
		visCubeRef.reference(visCube());
	}
	if (cache_p->modelVisCube_p.isPresent())
	{
		visCubeModelRef.reference(visCubeModel());
	}
	if (cache_p->correctedVisCube_p.isPresent())
	{
		visCubeCorrectedRef.reference(visCubeCorrected());
	}


	Complex cph;
	Double ph, udx;
	for (rownr_t row_idx = 0; row_idx < nRows(); ++row_idx)
	{
		udx = phase(row_idx) * -2.0 * C::pi / C::c; // in radian/Hz

		for (Int chan_idx = 0; chan_idx < nChannels(); ++chan_idx)
		{
			// Calculate the Complex factor for this row and channel
			ph = udx * freq(chan_idx);

			if (ph != 0.)
			{
				cph = Complex(cos(ph), sin(ph));

				// Shift each correlation:
				for (Int corr_idx = 0; corr_idx < nCorrelations(); ++corr_idx)
				{
					if (cache_p->visCube_p.isPresent())
					{
						visCubeRef(corr_idx, chan_idx, row_idx) *= cph;
					}

					if (cache_p->modelVisCube_p.isPresent())
					{
						visCubeModelRef(corr_idx, chan_idx, row_idx) *= cph;
					}

					if (cache_p->correctedVisCube_p.isPresent())
					{
						visCubeCorrectedRef(corr_idx, chan_idx, row_idx) *= cph;
					}
				}
			}
		}
	}
}

// Rotate visibility phase for phase center offsets
void
VisBufferImpl2::phaseCenterShift(Double dx, Double dy)
{
	// Offsets in radians (input is arcsec)
	dx *= (C::pi / 180.0 / 3600.0);
	dy *= (C::pi / 180.0 / 3600.0);

	// Extra path as fraction of U and V
	Vector<Double> udx;
	udx = uvw().row(0);
	Vector<Double> vdy;
	vdy = uvw().row(1);
	udx *= dx; // in m
	vdy *= dy;

	// Combine axes
	udx += vdy;

	phaseCenterShift(udx);
}

void
VisBufferImpl2::normalizeRow (Int row, Int nCorrelations, const Cube<Bool> & flagged,
                              Cube<Complex> & visCube, Cube<Complex> & modelCube,
                              Matrix<Float> & weightMat)
{
    Vector<Float> amplitudeSum = Vector<Float> (nCorrelations, 0.0f);
    Vector<Int> count = Vector<Int> (nCorrelations, 0);

    for (Int channel = 0; channel < nChannels (); channel ++) {

        for (Int correlation = 0; correlation < nCorrelations; correlation ++) {

            if (flagged (correlation, channel, row)){
                continue;  // data is flagged so skip over it
            }

            // If the model amplitude is zero, zero out the observed value.
            // Otherwise divide the observed value by the model value and
            // set the model value to 1.0.

            Float amplitude = abs (modelCube(correlation, channel, row));

            if (amplitude <= 0.0f) { // zero data if model is zero

                visCube (correlation, channel, row) = 0.0;
                continue;
            }

            // Normalize visibility datum by corresponding model data point.

            DComplex vis = visCube(correlation, channel, row);
            DComplex mod = modelCube(correlation, channel, row);

            visCube (correlation, channel, row) = Complex (vis / mod);
            modelCube (correlation, channel, row) = Complex(1.0);

            // Count and sum up the nonzero model amplitudes for this correlation.

            amplitudeSum (correlation) += amplitude;
            count (correlation) ++;
        }
    }

    // Adjust the weight matrix entries for this row appropriately

    for (Int correlation = 0; correlation < nCorrelations; correlation++) {

        if (count (correlation) > 0) {

            weightMat (correlation, row) *= square (amplitudeSum (correlation) /
                                                    count (correlation));
        }
        else {

            weightMat (correlation, row) = 0.0f;
        }
    }
}

void
VisBufferImpl2::registerCacheItem (VbCacheItemBase * item)
{
    cache_p->registry_p.push_back (item);
}

void
VisBufferImpl2::resetWeightsUsingSigma ()
{

  if (getViiP()->weightSpectrumExists()) {   // Exists and contains actual values
    // We are doing spectral weights

    if (getViiP()->sigmaSpectrumExists()) {
      // Init WS from SS for calibration
      Cube<Float> wtsp(nCorrelations(),nChannels(),nRows(),0.0);
      const Cube <Float> & sigmaSpec = this->sigmaSpectrum ();

      for (rownr_t irow=0;irow<nRows();++irow) {
	for (Int ichan=0;ichan<nChannels();++ichan) {
	  for (Int icorr=0;icorr<nCorrelations();++icorr) {
	    const Float &s=sigmaSpec(icorr,ichan,irow);
	    wtsp(icorr,ichan,irow) = (s>0.0 ? 1.0f/square(s) : 0.0);
	  }
	}
      }
      // Set it in the VB
      setWeightSpectrum(wtsp);

    }
    else {
      // Init WS from S, broadcast on chan axis
      Cube<Float> wtsp(nCorrelations(),nChannels(),nRows(),0.0);
      const Matrix <Float> & sigma = this->sigma ();

      for (rownr_t irow=0;irow<nRows();++irow) {
	for (Int icorr=0;icorr<nCorrelations();++icorr) {
	  const Float &s=sigma(icorr,irow);
	  wtsp(Slice(icorr,1,1),Slice(),Slice(irow,1,1)) =
	    (s>0.0 ? 1.0f/square(s) : 0.0);
	}
      }
      // Set it in the VB
      setWeightSpectrum(wtsp);
    }
  }
  else {
    // Not using WS, fill W from S
    Matrix<Float> wtm(nCorrelations(),nRows(),0.0);
    const Matrix <Float> & sigma = this->sigma ();
    
    for (rownr_t irow=0;irow<nRows();++irow) {
      for (Int icorr=0;icorr<nCorrelations();++icorr) {
	const Float &s=sigma(icorr,irow);
	wtm(icorr,irow) = (s>0.0 ? 1.0f/square(s) : 0.0);
      }
    }
    // Set it in the VB
    setWeight(wtm);
  }

}

void
VisBufferImpl2::setIterationInfo (Int msId,
                                  const String & msName,
                                  Bool isNewMs,
                                  Bool isNewArrayId,
                                  Bool isNewFieldId,
                                  Bool isNewSpectralWindow,
                                  const Subchunk & subchunk,
                                  const Vector<Int> & correlations,
                                  const Vector<Stokes::StokesTypes> & correlationsDefined,
                                  const Vector<Stokes::StokesTypes> & correlationsSelected,
                                  CountedPtr <WeightScaling> weightScaling)
{
    // Set the iteration attributes into this VisBuffer

    state_p->msId_p = msId;
    state_p->msName_p = msName;
    state_p->isNewMs_p = isNewMs;
    state_p->isNewMs_p = isNewMs;
    state_p->isNewArrayId_p = isNewArrayId;
    state_p->isNewFieldId_p = isNewFieldId;
    state_p->isNewSpectralWindow_p = isNewSpectralWindow;
    state_p->subchunk_p = subchunk;
    state_p->correlations_p.assign (correlations);
    state_p->correlationsDefined_p.assign (correlationsDefined);
    state_p->correlationsSelected_p.assign (correlationsSelected);
    state_p->weightScaling_p = weightScaling;
}

void
VisBufferImpl2::setFillable (Bool isFillable)
{
    state_p->isFillable_p = isFillable;
}



void
VisBufferImpl2::configureNewSubchunk (Int msId,
                                      const String & msName,
                                      Bool isNewMs,
                                      Bool isNewArrayId,
                                      Bool isNewFieldId,
                                      Bool isNewSpectralWindow,
                                      const Subchunk & subchunk,
                                      const casacore::Vector<casacore::rownr_t>& nRowsPerShape, 
                                      const casacore::Vector<casacore::Int>& nChannelsPerShape,
                                      const casacore::Vector<casacore::Int>& nCorrelationsPerShape,
                                      const Vector<Int> & correlations,
                                      const Vector<Stokes::StokesTypes> & correlationsDefined,
                                      const Vector<Stokes::StokesTypes> & correlationsSelected,
                                      CountedPtr<WeightScaling> weightScaling)
{
    // Prepare this VisBuffer for the new subchunk

    cacheClear();

    setIterationInfo (msId, msName, isNewMs, isNewArrayId, isNewFieldId,
                      isNewSpectralWindow, subchunk, correlations,
                      correlationsDefined, correlationsSelected, weightScaling);

    setFillable (true); // New subchunk, so it's fillable

    state_p->frequencies_p.flush();
    state_p->channelNumbers_p.flush();

    // Initialize the API methods that do not support several shapes with the first shape
    // Clients which use this "old" API should work as expected as long as there is one
    // single shape in the VisBuffer.
    cache_p->nRows_p.setSpecial (std::accumulate(nRowsPerShape.begin(), nRowsPerShape.end(), 0));
    cache_p->nChannels_p.setSpecial (nChannelsPerShape[0]);
    cache_p->nCorrelations_p.setSpecial (nCorrelationsPerShape[0]);

    cache_p->nShapes_p.setSpecial (nRowsPerShape.size());
    cache_p->nRowsPerShape_p.setSpecial (nRowsPerShape);
    cache_p->nChannelsPerShape_p.setSpecial (nChannelsPerShape);
    cache_p->nCorrelationsPerShape_p.setSpecial (nCorrelationsPerShape);

    setupValidShapes ();
}

void
VisBufferImpl2::setRekeyable (Bool isRekeyable)
{
    state_p->isRekeyable_p = isRekeyable;
}

void
VisBufferImpl2::setShape (Int nCorrelations, Int nChannels, rownr_t nRows,
                          Bool clearTheCache)
{
    ThrowIf (hasShape() && ! isRekeyable(),
             "Operation setShape is illegal on nonrekeyable VisBuffer");

    if (clearTheCache){
        cacheClear (false); // leave values alone so that array could be reused
    }

    cache_p->nCorrelations_p.setSpecial(nCorrelations);
    cache_p->nChannels_p.setSpecial(nChannels);
    cache_p->nRows_p.setSpecial(nRows);

    Vector<rownr_t> nRowsPerShape(1);
    Vector<Int> nChannelsPerShape(1);
    Vector<Int> nCorrelationsPerShape(1);
    nRowsPerShape[0] = nRows;
    nChannelsPerShape[0] = nChannels;
    nCorrelationsPerShape[0] = nCorrelations;

    cache_p->nShapes_p.setSpecial (1);
    cache_p->nRowsPerShape_p.setSpecial (nRowsPerShape);
    cache_p->nChannelsPerShape_p.setSpecial (nChannelsPerShape);
    cache_p->nCorrelationsPerShape_p.setSpecial (nCorrelationsPerShape);

    setupValidShapes ();
}

void
VisBufferImpl2::setupValidShapes ()
{
    state_p->validShapes_p [Nr] = IPosition (1, nRows());
    state_p->validShapes_p [NfNr] = IPosition (2, nChannels(), nRows());
    state_p->validShapes_p [NcNr] = IPosition (2, nCorrelations(), nRows());
    state_p->validShapes_p [NcNfNr] = IPosition (3, nCorrelations(), nChannels(), nRows());
    state_p->validShapes_p [I3Nr] = IPosition (2, 3, nRows());
    //state_p->validShapes [NcNfNcatNr] = IPosition (4, nCorrelations(), nChannels(), nCategories(), nRows());
    //   flag_category is not used in CASA, so no need to implement checking.

    state_p->validVectorShapes_p [Ns] = IPosition (1, nShapes());
    state_p->validVectorShapes_p [NsNcNr].resize(nShapes());
    state_p->validVectorShapes_p [NsNcNfNr].resize(nShapes());
    for(size_t iShape = 0; iShape < nShapes(); iShape++)
    {
        state_p->validVectorShapes_p [NsNcNr][iShape] = IPosition (2, nCorrelationsPerShape()[iShape], nRowsPerShape()[iShape]);
        state_p->validVectorShapes_p [NsNcNfNr][iShape] = IPosition (3, nCorrelationsPerShape()[iShape], nChannelsPerShape()[iShape], nRowsPerShape()[iShape]);
    }
}

bool
VisBufferImpl2::setWritability (Bool newWritability)
{
    bool oldWritability = state_p->isWritable_p;
    state_p->isWritable_p = newWritability;
    return oldWritability;
}



// Sort correlations: (PP,QQ,PQ,QP) -> (PP,PQ,QP,QQ)
void
VisBufferImpl2::sortCorrelationsAux (bool makeSorted)
{

    // This method is for temporarily sorting the correlations
    //  into canonical order if the MS data is out-of-order
    // NB: Always sorts the weightMat()
    // NB: Only works on the visCube-style data
    // NB: It only sorts the data columns which are already present
    //     (so make sure the ones you need are already read!)
    // NB: It is the user's responsibility to run unSortCorr
    //     after using the sorted data to put it back in order
    // NB: corrType_p is NOT changed to match the sorted
    //     correlations (it is expected that this sort is
    //     temporary, and that we will run unSortCorr
    // NB: This method does nothing if no sort required

    // Sort if nominal order is non-canonical (only for nCorr=4).
    // Also do not sort or unsort if that is the current sort state.

    if (! areCorrelationsInCanonicalOrder() &&
        (state_p->areCorrelationsSorted_p != makeSorted)) {

        // First sort the weights

        weight();    // (ensures it is filled)

        Vector<Float> wtmp(nRows ());
        Vector<Float> w1, w2, w3;
        IPosition wblc (1, 0, 0);
        IPosition wtrc (2, 0, nRows () - 1);
        IPosition vec (1, nRows ());

        VisBufferCache::sortCorrelationItem (cache_p->weight_p, wblc, wtrc, vec, wtmp, makeSorted);

        // Now sort the complex data cubes

        Matrix<Complex> tmp(nChannels (), nRows ());
        Matrix<Complex> p1, p2, p3;
        IPosition blc(3, 0, 0, 0);
        IPosition trc(3, 0, nChannels () - 1, nRows () - 1);
        IPosition mat(2, nChannels (), nRows ());

        // Sort the various visCubes, if present

        VisBufferCache::sortCorrelationItem (cache_p->visCube_p, blc, trc, mat, tmp, makeSorted);

        VisBufferCache::sortCorrelationItem (cache_p->modelVisCube_p, blc, trc, mat, tmp, makeSorted);

        VisBufferCache::sortCorrelationItem (cache_p->correctedVisCube_p, blc, trc, mat, tmp, makeSorted);

        // Finally sort the float data cube

        Matrix<Float> tmp2 (nChannels (), nRows ());

        VisBufferCache::sortCorrelationItem (cache_p->floatDataCube_p, blc, trc, mat, tmp2, makeSorted);

        // Record the sort state

        state_p->areCorrelationsSorted_p = makeSorted;
    }

}

// Sort correlations: (PP,QQ,PQ,QP) -> (PP,PQ,QP,QQ)
void
VisBufferImpl2::sortCorr()
{
    sortCorrelationsAux (true);
}


void
VisBufferImpl2::stateCopy (const VisBufferImpl2 & other)
{
    // Copy state from the other buffer

    state_p->areCorrelationsSorted_p = other.areCorrelationsSorted ();
    state_p->dirtyComponents_p = other.dirtyComponentsGet ();
    state_p->isAttached_p = false;  // attachment isn't copyabled
    state_p->isNewArrayId_p = other.isNewArrayId ();
    state_p->isNewFieldId_p = other.isNewFieldId ();
    state_p->isNewMs_p = other.isNewMs ();
    state_p->isNewSpectralWindow_p = other.isNewSpectralWindow ();
    state_p->pointingTableLastRow_p = -1; // This will slow pointing table lookup
                                          // but probably not important in cases
                                          // where a vb is being copied (?).
    state_p->newMs_p = other.isNewMs ();
    state_p->vi_p = 0; // just to be safe
    state_p->visModelData_p = other.getVisModelData ()->clone();
}


// Unsort correlations: (PP,PQ,QP,QQ) -> (PP,QQ,PQ,QP)
void
VisBufferImpl2::unSortCorr()
{
    sortCorrelationsAux (false);
}

void
VisBufferImpl2::validate ()
{
    cacheClear (true); // empty values but mark as cached.
}

void
VisBufferImpl2::validateShapes () const
{
    // Check that all of the modified array columns have the proper shape.
    // Throw an exception if any columns have improper shape.  This is
    // intended to be a last chance sanity-check before the data is written to disk
    // or made available for use.

    String message;

    for (CacheRegistry::iterator i = cache_p->registry_p.begin();
         i != cache_p->registry_p.end();
         i++){

        if ((*i)->isDirty() && ! (*i)->isShapeOk ()){

            message += (*i)->shapeErrorMessage () + "\n";
        }
    }

    ThrowIf (! message.empty(), message);
}

void
VisBufferImpl2::writeChangesBack ()
{
    ThrowIf (! state_p->isAttached_p,
             "Call to writeChangesBack on unattached VisBuffer.");

    ViImplementation2 * rwvi = dynamic_cast <ViImplementation2 *> (getViiP());

    ThrowIf (rwvi == 0, "Can't write to a read-only VisibilityIterator.");

    if (isRekeyable()){

        validateShapes ();
    }

    rwvi->writeBackChanges (this);
}

void
VisBufferImpl2::initWeightSpectrum(const Cube<Float>& wtspec) 
{
    ThrowIf (! state_p->isAttached_p,
             "Call to writeChangesBack on unattached VisBuffer.");

    ViImplementation2 * rwvi = dynamic_cast <ViImplementation2 *> (getViiP());

    ThrowIf (rwvi == 0, "Can't write to a read-only VisibilityIterator.");

    // TBD:  Should verify that shape is correct!

    rwvi->initWeightSpectrum(wtspec);
}

void
VisBufferImpl2::initSigmaSpectrum(const Cube<Float>& wtspec) 
{
    ThrowIf (! state_p->isAttached_p,
             "Call to writeChangesBack on unattached VisBuffer.");

    ViImplementation2 * rwvi = dynamic_cast <ViImplementation2 *> (getViiP());

    ThrowIf (rwvi == 0, "Can't write to a read-only VisibilityIterator.");

    // TBD:  Should verify that shape is correct!

    rwvi->initSigmaSpectrum(wtspec);
}


const casacore::MeasurementSet& 
VisBufferImpl2::ms() const {
  return getViiP()->ms();
}

const vi::SubtableColumns& 
VisBufferImpl2::subtableColumns () const {
  return getViiP()->subtableColumns();
}  

casacore::Bool
VisBufferImpl2::existsColumn(VisBufferComponent2 id) const {
	return getViiP()->existsColumn(id);
}

//      +---------------+
//      |               |
//      |  Calculators  |
//      |               |
//      +---------------+


MDirection
VisBufferImpl2::azel0(Double time) const
{
  return getViiP()->azel0(time);
}

const Vector<MDirection> &
VisBufferImpl2::azel(Double time) const
{
  return getViiP()->azel(time);
}

const Vector<Float> &
VisBufferImpl2::feedPa(Double time) const
{
  return getViiP()->feed_pa(time);
}

Double
VisBufferImpl2::hourang(Double time) const
{
  return getViiP()->hourang(time);
}

Float
VisBufferImpl2::parang0(Double time) const
{
  return getViiP()->parang0(time);
}

const Vector<Float> &
VisBufferImpl2::parang(Double time) const
{
  return getViiP()->parang(time);
}

//      +-------------+
//      |             |
//      |  Accessors  |
//      |             |
//      +-------------+

const Vector<Int> &
VisBufferImpl2::antenna1 () const
{
    return cache_p->antenna1_p.get ();
}

void
VisBufferImpl2::setAntenna1 (const Vector<Int> & value)
{
    cache_p->antenna1_p.set (value);
}

const Vector<Int> &
VisBufferImpl2::antenna2 () const
{
    return cache_p->antenna2_p.get ();
}

void
VisBufferImpl2::setAntenna2 (const Vector<Int> & value)
{
    cache_p->antenna2_p.set (value);
}

const Vector<Int> &
VisBufferImpl2::arrayId () const
{
    return cache_p->arrayId_p.get ();
}

void
VisBufferImpl2::setArrayId (const Vector<Int> &value)
{
    cache_p->arrayId_p.set (value);
}

const Vector<SquareMatrix<Complex, 2> > &
VisBufferImpl2::cjones () const
{
    return cache_p->cjones_p.get ();
}

const Vector<Int> &
VisBufferImpl2::correlationTypes () const
{
    return cache_p->corrType_p.get ();
}

//Int
//VisBufferImpl2::dataDescriptionId () const
//{
//    return cache_p->dataDescriptionId_p.get ();
//}

const Vector<Int> &
VisBufferImpl2::dataDescriptionIds () const
{
	return cache_p->dataDescriptionIds_p.get ();
}

//void
//VisBufferImpl2::setDataDescriptionId (Int value)
//{
//    cache_p->dataDescriptionId_p.set (value);
//}

void
VisBufferImpl2::setDataDescriptionIds (const Vector<Int> & value)
{
    cache_p->dataDescriptionIds_p.set (value);
}


const Vector<MDirection> &
VisBufferImpl2::direction1 () const
{
    return cache_p->direction1_p.get ();
}

const Vector<MDirection> &
VisBufferImpl2::direction2 () const
{
    return cache_p->direction2_p.get ();
}

const Vector<Double> &
VisBufferImpl2::exposure () const
{
    return cache_p->exposure_p.get ();
}

void
VisBufferImpl2::setExposure (const Vector<Double> & value)
{
    cache_p->exposure_p.set (value);
}

const Vector<Int> &
VisBufferImpl2::feed1 () const
{
    return cache_p->feed1_p.get ();
}

void
VisBufferImpl2::setFeed1 (const Vector<Int> & value)
{
    cache_p->feed1_p.set (value);
}

const Vector<Float> &
VisBufferImpl2::feedPa1 () const
{
    return cache_p->feed1Pa_p.get ();
}

const Vector<Int> &
VisBufferImpl2::feed2 () const
{
    return cache_p->feed2_p.get ();
}

void
VisBufferImpl2::setFeed2 (const Vector<Int> & value)
{
    cache_p->feed2_p.set (value);
}

const Vector<Float> &
VisBufferImpl2::feedPa2 () const
{
    return cache_p->feed2Pa_p.get ();
}

const Vector<Int> &
VisBufferImpl2::fieldId () const
{
    return cache_p->fieldId_p.get ();
}

void
VisBufferImpl2::setFieldId (const Vector<Int> & value)
{
    cache_p->fieldId_p.set (value);
}

//const Matrix<Bool> &
//VisBufferImpl2::flag () const
//{
//    return cache_p->flag_p.get ();
//}

//void
//VisBufferImpl2::setFlag (const Matrix<Bool>& value)
//{
//    cache_p->flag_p.set (value);
//}

const Array<Bool> &
VisBufferImpl2::flagCategory () const
{
    return cache_p->flagCategory_p.get();
}

void
VisBufferImpl2::setFlagCategory (const Array<Bool>& value)
{
    cache_p->flagCategory_p.set (value);
}

const casacore::Vector<casacore::Array<casacore::Bool>> &
VisBufferImpl2::flagCategories () const
{
    return cache_p->flagCategories_p.get();
}

void
VisBufferImpl2::setFlagCategories (const casacore::Vector<casacore::Array<casacore::Bool>>& value)
{
    cache_p->flagCategories_p.set (value);
}

const Cube<Bool> &
VisBufferImpl2::flagCube () const
{
    return cache_p->flagCube_p.get ();
}

const Vector<Cube<Bool>> &
VisBufferImpl2::flagCubes () const
{
    return cache_p->flagCubes_p.get ();
}

Cube<Bool> &
VisBufferImpl2::flagCubeRef ()
{
    return cache_p->flagCube_p.getRef ();
}

Vector<Cube<Bool>> &
VisBufferImpl2::flagCubesRef ()
{
    return cache_p->flagCubes_p.getRef ();
}

void
VisBufferImpl2::setFlagCube (const Cube<Bool>& value)
{
    cache_p->flagCube_p.set (value);
}

void
VisBufferImpl2::setFlagCubes (const Vector<Cube<Bool>>& value)
{
    cache_p->flagCubes_p.set (value);
}

const Vector<Bool> &
VisBufferImpl2::flagRow () const
{
    return cache_p->flagRow_p.get ();
}

Vector<Bool> &
VisBufferImpl2::flagRowRef ()
{
    return cache_p->flagRow_p.getRef ();
}

void
VisBufferImpl2::setFlagRow (const Vector<Bool>& value)
{
    cache_p->flagRow_p.set (value);
}

const Matrix<Float> &
VisBufferImpl2::imagingWeight () const
{
    return cache_p->imagingWeight_p.get ();
}

void
VisBufferImpl2::setImagingWeight (const Matrix<Float> & newImagingWeight)
{
    cache_p->imagingWeight_p.set (newImagingWeight);
}


//const Vector<Double> &
//VisBufferImpl2::lsrFrequency () const
//{
//    return cache_p->lsrFrequency_p.get ();
//}

Int
VisBufferImpl2::nAntennas () const
{
    return cache_p->nAntennas_p.get ();
}

Int
VisBufferImpl2::nChannels () const
{
    return cache_p->nChannels_p.get ();
}

Int
VisBufferImpl2::nCorrelations () const
{
    return cache_p->nCorrelations_p.get();
}

rownr_t
VisBufferImpl2::nRows () const
{
    return cache_p->nRows_p.get ();
}

rownr_t
VisBufferImpl2::nShapes () const
{
    return cache_p->nShapes_p.get ();
}

const Vector<rownr_t>&
VisBufferImpl2::nRowsPerShape () const
{
    return cache_p->nRowsPerShape_p.get ();
}

const Vector<Int> &
VisBufferImpl2::nChannelsPerShape () const
{
    return cache_p->nChannelsPerShape_p.get ();
}

const Vector<Int> &
VisBufferImpl2::nCorrelationsPerShape () const
{
    return cache_p->nCorrelationsPerShape_p.get ();
}

const Vector<Int> &
VisBufferImpl2::observationId () const
{
    return cache_p->observationId_p.get ();
}

void
VisBufferImpl2::setObservationId (const Vector<Int> & value)
{
    cache_p->observationId_p.set (value);
}

const MDirection&
VisBufferImpl2::phaseCenter () const
{
    return cache_p->phaseCenter_p.get ();
}

Int
VisBufferImpl2::polarizationFrame () const
{
    return cache_p->polFrame_p.get ();
}

Int
VisBufferImpl2::polarizationId () const
{
    return cache_p->polarizationId_p.get ();
}

const Vector<Int> &
VisBufferImpl2::processorId () const
{
    return cache_p->processorId_p.get ();
}

void
VisBufferImpl2::setProcessorId (const Vector<Int> & value)
{
    cache_p->processorId_p.set (value);
}

const Vector<rownr_t> &
VisBufferImpl2::rowIds () const
{
    return cache_p->rowIds_p.get ();
}

const Vector<Int> &
VisBufferImpl2::scan () const
{
    return cache_p->scan_p.get ();
}

void
VisBufferImpl2::setScan (const Vector<Int> & value)
{
    cache_p->scan_p.set (value);
}

const Matrix<Float> &
VisBufferImpl2::sigma () const
{
    return cache_p->sigma_p.get ();
}

const Vector<Matrix<Float>> &
VisBufferImpl2::sigmas () const
{
    return cache_p->sigmas_p.get ();
}

void
VisBufferImpl2::setSigma (const Matrix<Float> & sigmas)
{
    cache_p->sigma_p.set (sigmas);
}

void
VisBufferImpl2::setSigmas (const Vector<Matrix<Float>> & sigmas)
{
    cache_p->sigmas_p.set (sigmas);
}

//const Matrix<Float> &
//VisBufferImpl2::sigmaMat () const
//{
//    return cache_p->sigmaMat_p.get ();
//}

const Vector<Int> &
VisBufferImpl2::spectralWindows () const
{
    return cache_p->spectralWindows_p.get ();
}

void
VisBufferImpl2::setSpectralWindows (const Vector<Int> & spectralWindows)
{
    cache_p->spectralWindows_p.set (spectralWindows);
}


const Vector<Int> &
VisBufferImpl2::stateId () const
{
    return cache_p->stateId_p.get ();
}

void
VisBufferImpl2::setStateId (const Vector<Int> & value)
{
    cache_p->stateId_p.set (value);
}

const Vector<Double> &
VisBufferImpl2::time () const
{
    return cache_p->time_p.get ();
}

void
VisBufferImpl2::setTime (const Vector<Double> & value)
{
    cache_p->time_p.set (value);
}

const Vector<Double> &
VisBufferImpl2::timeCentroid () const
{
    return cache_p->timeCentroid_p.get ();
}

void
VisBufferImpl2::setTimeCentroid (const Vector<Double> & value)
{
    cache_p->timeCentroid_p.set (value);
}

const Vector<Double> &
VisBufferImpl2::timeInterval () const
{
    return cache_p->timeInterval_p.get ();
}

void
VisBufferImpl2::setTimeInterval (const Vector<Double> & value)
{
    cache_p->timeInterval_p.set (value);
}

const Matrix<Double> &
VisBufferImpl2::uvw () const
{
    return cache_p->uvw_p.get ();
}

void
VisBufferImpl2::setUvw (const Matrix<Double> & value)
{
    cache_p->uvw_p.set (value);
}

const Cube<Complex> &
VisBufferImpl2::visCubeCorrected () const
{
    return cache_p->correctedVisCube_p.get ();
}

Cube<Complex> &
VisBufferImpl2::visCubeCorrectedRef ()
{
    return cache_p->correctedVisCube_p.getRef ();
}

void
VisBufferImpl2::setVisCubeCorrected (const Cube<Complex> & value)
{
    cache_p->correctedVisCube_p.set (value);
}

const Vector<Cube<Complex>> &
VisBufferImpl2::visCubesCorrected () const
{
    return cache_p->correctedVisCubes_p.get ();
}

Vector<Cube<Complex>> &
VisBufferImpl2::visCubesCorrectedRef ()
{
    return cache_p->correctedVisCubes_p.getRef ();
}

void
VisBufferImpl2::setVisCubesCorrected (const Vector<Cube<Complex>> & value)
{
    cache_p->correctedVisCubes_p.set (value);
}

//const Matrix<CStokesVector> &
//VisBufferImpl2::visCorrected () const
//{
//    return cache_p->correctedVisibility_p.get ();
//}

//void
//VisBufferImpl2::setVisCorrected (const Matrix<CStokesVector> & value)
//{
//    cache_p->correctedVisibility_p.set (value);
//}

const Cube<Float> &
VisBufferImpl2::visCubeFloat () const
{
    return cache_p->floatDataCube_p.get ();
}

void
VisBufferImpl2::setVisCubeFloat (const Cube<Float> & value)
{
    cache_p->floatDataCube_p.set (value);
}

const Vector<Cube<Float>> &
VisBufferImpl2::visCubesFloat () const
{
    return cache_p->floatDataCubes_p.get ();
}

void
VisBufferImpl2::setVisCubesFloat (const Vector<Cube<Float>> & value)
{
    cache_p->floatDataCubes_p.set (value);
}

const Cube<Complex> &
VisBufferImpl2::visCubeModel () const
{
    return cache_p->modelVisCube_p.get ();
}

Cube<Complex> &
VisBufferImpl2::visCubeModelRef ()
{
    return cache_p->modelVisCube_p.getRef ();
}

void
VisBufferImpl2::setVisCubeModel (const Complex & value)
{
    cache_p->modelVisCube_p.set (value);
}

void
VisBufferImpl2::setVisCubeModel (const Cube<Complex> & value)
{
    cache_p->modelVisCube_p.set (value);
}

const Vector<Cube<Complex>> &
VisBufferImpl2::visCubesModel () const
{
    return cache_p->modelVisCubes_p.get ();
}

Vector<Cube<Complex>> &
VisBufferImpl2::visCubesModelRef ()
{
    return cache_p->modelVisCubes_p.getRef ();
}

void
VisBufferImpl2::setVisCubesModel (const Vector<Cube<Complex>> & value)
{
    cache_p->modelVisCubes_p.set (value);
}

//void
//VisBufferImpl2::setVisCubesModel (const Complex & value)
//{
//    cache_p->modelVisCubes_p.set (value);
//}

ms::MsRow *
VisBufferImpl2::getRow (Int row) const
{
    ms::MsRow * msRow = new ms::Vbi2MsRow (row, this);

    return msRow;
}

ms::MsRow *
VisBufferImpl2::getRowMutable (Int row)
{
    ms::MsRow * msRow  = new ms::Vbi2MsRow (row, this);

    return msRow;
}


//const Matrix<CStokesVector> &
//VisBufferImpl2::visModel () const
//{
//    return cache_p->modelVisibility_p.get ();
//}

//void
//VisBufferImpl2::setVisModel (Matrix<CStokesVector> & value)
//{
//    cache_p->modelVisibility_p.set (value);
//}

void
VisBufferImpl2::setVisCubeModel(const Vector<Float>& stokesIn)
{

  enum {I, Q, U, V};

  Vector<Float> stokes (4, 0.0);

  stokes [I] = 1.0;  // Stokes parameters, nominally unpolarized, unit I

  for (uInt i = 0; i < stokesIn.nelements(); ++i){
      stokes [i] = stokesIn [i];
  }

  // Convert to correlations, according to basis
  Vector<Complex> stokesFinal (4, Complex(0.0)); // initially all zero

  if (polarizationFrame() == MSIter::Circular){
    stokesFinal(0) = Complex(stokes [I] + stokes [V]);
    stokesFinal(1) = Complex(stokes [Q], stokes [U]);
    stokesFinal(2) = Complex(stokes [Q], -stokes [U]);
    stokesFinal(3) = Complex(stokes [I] - stokes [V]);
  }
  else if (polarizationFrame() == MSIter::Linear) {
    stokesFinal(0) = Complex(stokes [I] + stokes [Q]);
    stokesFinal(1) = Complex(stokes [U], stokes [V]);
    stokesFinal(2) = Complex(stokes [U], -stokes [V]);
    stokesFinal(3) = Complex(stokes [I] - stokes [Q]);
  }
  else {
    throw(AipsError("Model-setting only works for CIRCULAR and LINEAR bases, for now."));
  }

  // A map onto the actual correlations in the VisBuffer  (which may be a subset)
  Vector<Int> corrmap;
  corrmap.assign(correlationTypes());  // actual copy, to avoid changing correlationTypes()!
  corrmap -= corrmap(0);

  ThrowIf (max(corrmap) >= 4,  "HELP! The correlations in the data are not normal!");

  // Set the modelVisCube accordingly
  Cube<Complex> visCube (getViiP()->visibilityShape(), 0.0);

  for (Int icorr = 0; icorr < nCorrelations (); ++icorr){
    if (abs(stokesFinal(corrmap(icorr))) > 0.0) {
      visCube (Slice (icorr, 1, 1), Slice(), Slice()).set(stokesFinal (corrmap (icorr)));
    }
  }

  cache_p->modelVisCube_p.set (visCube);

}


const Cube<Complex> &
VisBufferImpl2::visCube () const
{
    return cache_p->visCube_p.get ();
}

Cube<Complex> &
VisBufferImpl2::visCubeRef ()
{
    return cache_p->visCube_p.getRef ();
}

void
VisBufferImpl2::setVisCube (const Complex & value)
{
    cache_p->visCube_p.set (value);
}

void
VisBufferImpl2::setVisCube (const Cube<Complex> & value)
{
    cache_p->visCube_p.set (value);
}

const Vector<Cube<Complex>> &
VisBufferImpl2::visCubes () const
{
    return cache_p->visCubes_p.get ();
}

Vector<Cube<Complex>> &
VisBufferImpl2::visCubesRef ()
{
    return cache_p->visCubes_p.getRef ();
}

//void
//VisBufferImpl2::setVisCubes (const Complex & value)
//{
 //   cache_p->visCube_p.set (value);
//}

void
VisBufferImpl2::setVisCubes (const Vector<Cube<Complex>> & value)
{
    cache_p->visCubes_p.set (value);
}

//const Matrix<CStokesVector> &
//VisBufferImpl2::vis () const
//{
//    return cache_p->visibility_p.get ();
//}

//void
//VisBufferImpl2::setVis (Matrix<CStokesVector> & value)
//{
//    cache_p->visibility_p.set (value);
//}

const Matrix<Float> &
VisBufferImpl2::weight () const
{
    return cache_p->weight_p.get ();
}

void
VisBufferImpl2::setWeight (const Matrix<Float>& value)
{
    cache_p->weight_p.set (value);
}

const Vector<Matrix<Float>> &
VisBufferImpl2::weights () const
{
    return cache_p->weights_p.get ();
}

void
VisBufferImpl2::setWeights (const Vector<Matrix<Float>>& value)
{
    cache_p->weights_p.set (value);
}

//const Matrix<Float> &
//VisBufferImpl2::weightMat () const
//{
//    return cache_p->weightMat_p.get ();
//}

//void
//VisBufferImpl2::setWeightMat (const Matrix<Float>& value)
//{
//    cache_p->weightMat_p.set (value);
//}

const Cube<Float> &
VisBufferImpl2::weightSpectrum () const
{
    return cache_p->weightSpectrum_p.get ();
}

Cube<Float> &
VisBufferImpl2::weightSpectrumRef ()
{
    return cache_p->weightSpectrum_p.getRef();
}


void
VisBufferImpl2::setWeightSpectrum (const Cube<Float>& value)
{
    cache_p->weightSpectrum_p.set (value);
}

const Vector<Cube<Float>> &
VisBufferImpl2::weightSpectra () const
{
    return cache_p->weightSpectra_p.get ();
}

Vector<Cube<Float>> &
VisBufferImpl2::weightSpectraRef ()
{
    return cache_p->weightSpectra_p.getRef();
}

void
VisBufferImpl2::setWeightSpectra (const Vector<Cube<Float>>& value)
{
    cache_p->weightSpectra_p.set (value);
}

const Cube<Float> &
VisBufferImpl2::sigmaSpectrum () const
{
    return cache_p->sigmaSpectrum_p.get ();
}

Cube<Float> &
VisBufferImpl2::sigmaSpectrumRef ()
{
    return cache_p->sigmaSpectrum_p.getRef();
}

void
VisBufferImpl2::setSigmaSpectrum (const Cube<Float>& value)
{
    cache_p->sigmaSpectrum_p.set (value);
}

const Vector<Cube<Float>> &
VisBufferImpl2::sigmaSpectra () const
{
    return cache_p->sigmaSpectra_p.get ();
}

Vector<Cube<Float>> &
VisBufferImpl2::sigmaSpectraRef ()
{
    return cache_p->sigmaSpectra_p.getRef();
}

void
VisBufferImpl2::setSigmaSpectra (const Vector<Cube<Float>>& value)
{
    cache_p->sigmaSpectra_p.set (value);
}



//      +-----------+
//      |           |
//      |  Fillers  |
//      |           |
//      +-----------+


void
VisBufferImpl2::fillAntenna1 (Vector<Int>& value) const
{
  CheckVisIter ();

  getViiP()->antenna1 (value);
}


void
VisBufferImpl2::fillAntenna2 (Vector<Int>& value) const
{
  CheckVisIter ();

  getViiP()->antenna2 (value);
}

void
VisBufferImpl2::fillArrayId (Vector<Int>& value) const
{
  CheckVisIter ();

  getViiP()->arrayIds (value);
}

void
VisBufferImpl2::fillCorrType (Vector<Int>& value) const
{
  CheckVisIter ();

  getViiP()->corrType (value);
}

void
VisBufferImpl2::fillCubeCorrected (Cube <Complex> & value) const
{
    CheckVisIter ();

    getViiP()->visibilityCorrected (value);
}

void
VisBufferImpl2::fillCubesCorrected (Vector<Cube<Complex>> & value) const
{
    CheckVisIter ();

    getViiP()->visibilityCorrected (value);
}

void
VisBufferImpl2::fillCubeModel (Cube <Complex> & value) const
{
    CheckVisIter ();

    getViiP()->visibilityModel(value);
}

void
VisBufferImpl2::fillCubesModel (Vector<Cube <Complex>> & value) const
{
    CheckVisIter ();

    getViiP()->visibilityModel(value);
}

void
VisBufferImpl2::fillCubeObserved (Cube <Complex> & value) const
{
    CheckVisIter ();

    getViiP()->visibilityObserved (value);
}

void
VisBufferImpl2::fillCubesObserved (Vector<Cube <Complex>> & value) const
{
    CheckVisIter ();

    getViiP()->visibilityObserved (value);
}

void
VisBufferImpl2::fillDataDescriptionId  (Int& value) const
{
  CheckVisIter ();

  value = getViiP()->dataDescriptionId ();
}

void
VisBufferImpl2::fillDataDescriptionIds  (Vector<Int>& value) const
{
  CheckVisIter ();

  getViiP()->dataDescriptionIds (value);
}


void
VisBufferImpl2::fillDirection1 (Vector<MDirection>& value) const
{
  CheckVisIterBase ();
  // fill state_p->feed1_pa cache, antenna, feed and time will be filled automatically

  feedPa1 ();

  fillDirectionAux (value, antenna1 (), feed1 (), feedPa1 ());

  value.resize(nRows());
}

void
VisBufferImpl2::fillDirection2 (Vector<MDirection>& value) const
{
  CheckVisIterBase ();
  // fill state_p->feed1_pa cache, antenna, feed and time will be filled automatically

  feedPa2 ();

  fillDirectionAux (value, antenna2 (), feed2 (), feedPa2 ());

  value.resize(nRows());
}

void
VisBufferImpl2::fillDirectionAux (Vector<MDirection>& value,
                                  const Vector<Int> & antenna,
                                  const Vector<Int> &feed,
                                  const Vector<Float> & feedPa) const
{
    value.resize (nRows());

//    const MSPointingColumns & mspc = getViiP()->subtableColumns ().pointing();
//    state_p->pointingTableLastRow_p = mspc.pointingIndex (antenna (0),
//                                                          time()(0), state_p->pointingTableLastRow_p);

    if (getViiP()->allBeamOffsetsZero() && state_p->pointingTableLastRow_p < 0) {

        // No true pointing information found; use phase center from the field table

        value.set(phaseCenter());
        state_p->pointingTableLastRow_p = 0;
        return;
    }


    for (uInt row = 0; row < antenna.nelements(); ++row) {

        DebugAssert(antenna (row) >= 0 && feed (row) >= 0, AipsError);

        bool ok;
        std::tie (ok, value (row)) = getViiP()->getPointingAngle (antenna (row), time() (row));

        if (! ok) {

            value(row) = phaseCenter(); // nothing found, use phase center
        }

        if (!getViiP()->allBeamOffsetsZero()) {

            RigidVector<Double, 2> beamOffset = getViiP()->getBeamOffsets()(0, antenna (row),
                                                                           feed (row));

            if (downcase (getViiP()->antennaMounts()(antenna (row))) == "alt-az") {

                SquareMatrix<Double, 2> xform(SquareMatrix<Double, 2>::General);

                Double cpa = cos(feedPa(row));
                Double spa = sin(feedPa(row));

                xform(0, 0) = cpa;
                xform(1, 1) = cpa;
                xform(0, 1) = -spa;
                xform(1, 0) = spa;

                beamOffset *= xform; // parallactic angle rotation
            }

            value(row).shift(-beamOffset(0), beamOffset(1), true);
                // x direction is flipped to convert az-el type frame to ra-dec
        }
    }
}

void
VisBufferImpl2::fillExposure (Vector<Double>& value) const
{
  CheckVisIter ();

  getViiP()->exposure (value);
}

void
VisBufferImpl2::fillFeed1 (Vector<Int>& value) const
{
  CheckVisIter ();

  getViiP()->feed1 (value);
}

void
VisBufferImpl2::fillFeed2 (Vector<Int>& value) const
{
  CheckVisIter ();

  getViiP()->feed2 (value);
}

void
VisBufferImpl2::fillFeedPa1 (Vector <Float> & feedPa) const
{
  CheckVisIterBase ();

  // fill feed, antenna and time caches, if not filled before

  feed1 ();
  antenna1 ();
  time ();

  feedPa.resize(nRows());

  fillFeedPaAux (feedPa, antenna1 (), feed1 ());
}


void
VisBufferImpl2::fillFeedPa2 (Vector <Float> & feedPa) const
{
  CheckVisIterBase ();

  // Fill feed, antenna and time caches, if not filled before.

  feed2();
  antenna2();
  time();

  feedPa.resize(nRows());

  fillFeedPaAux (feedPa, antenna2(), feed2 ());
}

void
VisBufferImpl2::fillFeedPaAux (Vector <Float> & feedPa,
                              const Vector <Int> & antenna,
                              const Vector <Int> & feed) const
{
  for (uInt row = 0; row < feedPa.nelements(); ++row) {

    const Vector<Float>& antennaPointingAngle = this->feedPa (time ()(row));

    Assert(antenna (row) >= 0 && antenna (row) < (int) antennaPointingAngle.nelements());

    feedPa (row) = antennaPointingAngle (antenna (row));

    if (feed (row) != 0){  // Skip when feed(row) is zero

      // feedPa returns only the first feed position angle so
      // we need to add an offset if this row correspods to a
      // different feed

      float feedsAngle = getViiP()->receptorAngles()(0, antenna (row), feed (row));
      float feed0Angle = getViiP()->receptorAngles()(0, antenna (row), 0);

      feedPa (row) += feedsAngle - feed0Angle;
    }
  }
}

void
VisBufferImpl2::fillFieldId (Vector<Int>& value) const
{
  CheckVisIter ();

  getViiP()->fieldIds (value);
}

void
VisBufferImpl2::fillFlag (Matrix<Bool>& value) const
{
  CheckVisIter ();

  getViiP()->flag (value);
}

void
VisBufferImpl2::fillFlagCategory (Array<Bool>& value) const
{
  CheckVisIter();

  getViiP()->flagCategory (value);
}

void
VisBufferImpl2::fillFlagCube (Cube<Bool>& value) const
{
  CheckVisIter ();

  getViiP()->flag (value);
}

void
VisBufferImpl2::fillFlagCubes (Vector<Cube<Bool>>& value) const
{
  CheckVisIter ();

  getViiP()->flag (value);
}

void
VisBufferImpl2::fillFlagRow (Vector<Bool>& value) const
{
  CheckVisIter ();

  getViiP()->flagRow (value);
}

void
VisBufferImpl2::fillFloatData (Cube<Float>& value) const
{
  CheckVisIter ();

  getViiP()->floatData (value);
}

void
VisBufferImpl2::fillFloatCubes (Vector<Cube<Float>>& value) const
{
  CheckVisIter ();

  getViiP()->floatData (value);
}

void
VisBufferImpl2::fillImagingWeight (Matrix<Float> & value) const
{
    const VisImagingWeight & weightGenerator = getViiP()->getImagingWeightGenerator ();

    ThrowIf (weightGenerator.getType () == "none",
             "Bug check: Imaging weight generator not set");

    //#warning "Rework logic so that called code is not expecting a flag matrix."

    value.resize (IPosition (2, nChannels(), nRows()));

    Matrix<Bool> flagMat;
    flagMat.assign(flagCube().yzPlane(0));
    std::logical_and<Bool> andOp;

    /*
    Vector<Float> wts (nRows (), 0);
    wts = weight().row(0);
    wts += weight().row(nCorrelations() - 1);
    wts *= 0.5f;
    */

    // Extract weights correctly
    Matrix<Float> wtm;  // [nchan,nrow]
    Cube<Float> wtc;  //  [ncorr,nchan,nrow]
    if (getViiP()->weightSpectrumExists())
      wtc.reference(weightSpectrum());
    else 
      wtc.reference(weight().reform(IPosition(3,nCorrelations(),1,nRows())));

    // Collapse on correlation axis
    weightGenerator.unPolChanWeight(wtm,wtc);


    for (Int i = 1; i < nCorrelations(); ++ i){

        Matrix<Bool> flagPlane = flagCube().yzPlane(i);
        arrayTransform<Bool,Bool,Bool,std::logical_and<Bool> > (flagMat, flagPlane, flagMat, andOp);
    }

    if (weightGenerator.getType () == "uniform") {

        weightGenerator.weightUniform (value, flagMat, uvw (), getFrequencies (0), wtm, msId (), fieldId ()(0));

    } else if (weightGenerator.getType () == "radial") {

        weightGenerator.weightRadial (value, flagMat, uvw (), getFrequencies (0), wtm);

    } else {

        weightGenerator.weightNatural (value, flagMat, wtm);
    }

    if (weightGenerator.doFilter ()) {

        weightGenerator.filter (value, flagMat, uvw (), getFrequencies (0), wtm);
    }
}

void
VisBufferImpl2::fillJonesC (Vector<SquareMatrix<Complex, 2> >& value) const
{
  CheckVisIter ();

  getViiP()->jonesC (value);
}

void
VisBufferImpl2::fillNAntennas (Int & value) const
{
  CheckVisIter ();

  value = getViiP()->nAntennas();
}

void
VisBufferImpl2::fillNChannel (Int &) const
{
  CheckVisIter ();

  // This value enters the VB from a route that doesn't involve
  // filling; however the framework requires that this method exist
  // so it's implemented as a no-op.
}

void
VisBufferImpl2::fillNCorr (Int &) const
{
  CheckVisIter ();

  // This value enters the VB from a route that doesn't involve
  // filling; however the framework requires that this method exist
  // so it's implemented as a no-op.
}

void
VisBufferImpl2::fillNRow (Int&) const
{
  CheckVisIter ();

  // This value enters the VB from a route that doesn't involve
  // filling; however the framework requires that this method exist
  // so it's implemented as a no-op.
}

void
VisBufferImpl2::fillNShapes (Int&) const
{
  CheckVisIter ();

  // This value enters the VB from a route that doesn't involve
  // filling; however the framework requires that this method exist
  // so it's implemented as a no-op.
}

void
VisBufferImpl2::fillNRowPerShape (Vector<rownr_t> &) const
{
  CheckVisIter ();

  // This value enters the VB from a route that doesn't involve
  // filling; however the framework requires that this method exist
  // so it's implemented as a no-op.
}

void
VisBufferImpl2::fillNChannelPerShape (Vector<Int> &) const
{
  CheckVisIter ();

  // This value enters the VB from a route that doesn't involve
  // filling; however the framework requires that this method exist
  // so it's implemented as a no-op.
}

void
VisBufferImpl2::fillNCorrPerShape (Vector<Int> &) const
{
  CheckVisIter ();

  // This value enters the VB from a route that doesn't involve
  // filling; however the framework requires that this method exist
  // so it's implemented as a no-op.
}

void
VisBufferImpl2::fillObservationId (Vector<Int>& value) const
{
  CheckVisIter();

  getViiP()->observationId (value);
}

void
VisBufferImpl2::fillPhaseCenter (MDirection& value) const
{
  CheckVisIter ();

  value = getViiP()->phaseCenter ();
}

void
VisBufferImpl2::fillPolFrame (Int& value) const
{
  CheckVisIter ();

  value = getViiP()->polFrame ();
}

void
VisBufferImpl2::fillPolarizationId (Int& value) const
{
  CheckVisIter ();

  value = getViiP()->polarizationId ();
}

void
VisBufferImpl2::fillProcessorId (Vector<Int>& value) const
{
  CheckVisIter();

  getViiP()->processorId (value);
}

void
VisBufferImpl2::fillRowIds (Vector<rownr_t>& value) const
{
  CheckVisIter ();

  getViiP()->getRowIds(value);
}


void
VisBufferImpl2::fillScan (Vector<Int>& value) const
{
  CheckVisIter ();

  getViiP()->scan (value);
}

void
VisBufferImpl2::fillSigma (Matrix<Float>& value) const
{
  CheckVisIter ();
  getViiP()->sigma (value);
}

void
VisBufferImpl2::fillSigmas (Vector<Matrix<Float>>& value) const
{
  CheckVisIter ();

  getViiP()->sigma (value);
}

//void
//VisBufferImpl2::fillSigmaMat (Matrix<Float>& value) const
//{
//  CheckVisIter ();
//
//  getViiP()->sigmaMat (value);
//}

void
VisBufferImpl2::fillSpectralWindows (Vector<Int>& value) const
{
  CheckVisIter ();

  getViiP()->spectralWindows (value);
}

void
VisBufferImpl2::fillStateId (Vector<Int>& value) const
{
  CheckVisIter();

  getViiP()->stateId (value);
}


void
VisBufferImpl2::fillTime (Vector<Double>& value) const
{
  CheckVisIter ();

  getViiP()->time (value);
}

void
VisBufferImpl2::fillTimeCentroid (Vector<Double>& value) const
{
  CheckVisIter ();

  getViiP()->timeCentroid (value);
}

void
VisBufferImpl2::fillTimeInterval (Vector<Double>& value) const
{
  CheckVisIter ();

  getViiP()->timeInterval (value);
}

void
VisBufferImpl2::fillUvw (Matrix<Double>& value) const
{
  CheckVisIter ();

  getViiP()->uvw (value);
}

//void
//VisBufferImpl2::fillVisibilityCorrected (Matrix<CStokesVector>& value) const
//{
//    CheckVisIter ();
//
//    getViiP()->visibilityCorrected (value);
//}

//void
//VisBufferImpl2::fillVisibilityModel (Matrix<CStokesVector>& value) const
//{
//    CheckVisIter ();
//
//    getViiP()->visibilityModel (value);
//}

//void
//VisBufferImpl2::fillVisibilityObserved (Matrix<CStokesVector>& value) const
//{
//    CheckVisIter ();
//
//    getViiP()->visibilityObserved (value);
//}


void
VisBufferImpl2::fillWeight (Matrix<Float>& value) const
{
  CheckVisIter ();

  getViiP()->weight (value);
}

void
VisBufferImpl2::fillWeights (Vector<Matrix<Float>>& value) const
{
  CheckVisIter ();

  getViiP()->weight (value);
}

void
VisBufferImpl2::fillWeightSpectrum (Cube<Float>& value) const
{
    CheckVisIter ();

    if (getViiP()->weightSpectrumExists()){

        getViiP()->weightSpectrum (value);
    }
    else{

        // Weight spectrum doesn't exist so create on using the weight column.

        Matrix<Float> theWeights;
        theWeights = weight();  // need a mutable copy so ensure no sharing.

        // The weight is the sum of the weight across all channels
        // so divide it evenly between the channels.

        // jagonzal (new WEIGHT/SIGMA convention in CASA 4.2.2)
        // theWeights = theWeights / nChannels();

        value.resize (IPosition (3, nCorrelations (), nChannels (), nRows()));

        // Copy the apportioned weight value into the weight spectrum

        // jagonzal (TODO): Review this filling code (it should be row/channel/correlation)
        //                  Or even better direct array assignment
        for (rownr_t row = 0; row < nRows(); row ++){

            for (Int correlation = 0; correlation < nCorrelations (); correlation ++){

                Int nChan = nChannels();
                float theWeight = theWeights (correlation, row);

                for (Int channel = 0; channel < nChan; channel ++){

                    value (correlation, channel, row) = theWeight;
                }

            }
        }
    }
}

void
VisBufferImpl2::fillWeightSpectra (Vector<Cube<Float>>& value) const
{
    CheckVisIter ();

    if (getViiP()->weightSpectrumExists()){

        getViiP()->weightSpectrum (value);
    }
    else{
        value.resize(this->nShapes());
        
        auto nRowsPerShape = this->nRowsPerShape();
        auto nCorrelationsPerShape = this->nCorrelationsPerShape();
        auto nChannelsPerShape = this->nChannelsPerShape();

        // Weight spectrum doesn't exist so create on using the weight column.
        auto theWeights = weights();

        for (rownr_t ishape = 0; ishape < nShapes(); ishape ++){

            auto nRows = nRowsPerShape[ishape];
            auto nCorrelations = nCorrelationsPerShape[ishape];
            auto nChannels = nChannelsPerShape[ishape];

            value[ishape].resize (IPosition (3, nCorrelations, nChannels, nRows));

            for (rownr_t row = 0; row < nRows; row ++){

                for (Int correlation = 0; correlation < nCorrelations; correlation ++){

                    float theWeight = theWeights[ishape] (correlation, row);

                    for (Int channel = 0; channel < nChannels; channel ++){
                    
                        value[ishape] (correlation, channel, row) = theWeight;
                    }
            
                }
            }
        }
    }
}

void
VisBufferImpl2::fillSigmaSpectrum (Cube<Float>& value) const
{
    CheckVisIter ();

    if (getViiP()->sigmaSpectrumExists()){

        getViiP()->sigmaSpectrum (value);
    }
    else{

        auto nRows = this->nRows();
        auto nCorrelations = this->nCorrelations ();
        auto nChannels = this->nChannels();

        // Sigma spectrum doesn't exist so create on using the sigma column.

        const Matrix<Float> & theSigmas = sigma();
        /////theSigmas = sigma();  // need a mutable copy so ensure no sharing.

        value.resize (IPosition (3, nCorrelations, nChannels, nRows));

        // Copy the apportioned weight value into the sigma spectrum

        // jagonzal (TODO): Review this filling code (it should be row/channel/correlation)
        //                  Or even better direct array assignment

        for (rownr_t row = 0; row < nRows; row ++){

            for (Int correlation = 0; correlation < nCorrelations; correlation ++){

                float theSigma = theSigmas (correlation, row);

                for (Int channel = 0; channel < nChannels; channel ++){

                    value (correlation, channel, row) = theSigma;
                }

            }
        }
    }
}

void
VisBufferImpl2::fillSigmaSpectra (Vector<Cube<Float>>& value) const
{
    CheckVisIter ();

    if (getViiP()->sigmaSpectrumExists()){

        getViiP()->sigmaSpectrum (value);
    }
    else{
        value.resize(this->nShapes());
        
        auto nRowsPerShape = this->nRowsPerShape();
        auto nCorrelationsPerShape = this->nCorrelationsPerShape();
        auto nChannelsPerShape = this->nChannelsPerShape();

        // Sigma spectrum doesn't exist so create on using the sigma column.
        auto theSigmas = sigmas();

        for (rownr_t ishape = 0; ishape < nShapes(); ishape ++){

            auto nRows = nRowsPerShape[ishape];
            auto nCorrelations = nCorrelationsPerShape[ishape];
            auto nChannels = nChannelsPerShape[ishape];

            value[ishape].resize (IPosition (3, nCorrelations, nChannels, nRows));

            for (rownr_t row = 0; row < nRows; row ++){

                for (Int correlation = 0; correlation < nCorrelations; correlation ++){

                    float theSigma = theSigmas[ishape] (correlation, row);

                    for (Int channel = 0; channel < nChannels; channel ++){
                    
                        value[ishape] (correlation, channel, row) = theSigma;
                    }
            
                }
            }
        }
    }
}

Float
VisBufferImpl2::getWeightScaled (Int row) const
{
    Float sum = 0;
    Int n = nCorrelations();

    for (Int correlation = 0; correlation < n; ++ correlation){

        sum += getWeightScaled (correlation, row);
    }

    return sum / n;
}

Float
VisBufferImpl2::getWeightScaled (Int correlation, Int row) const
{
    if (flagRow () (row)){
        return 0;
    }

    if (weightSpectrumPresent()){

        Float sum = 0;
        Int n = nChannels ();

        for (Int channel = 0; channel < n; ++ channel){

            sum += getWeightScaled (correlation, channel, row);

        }

        return sum / n;
    }
    else {

        Float theWeight = weight () (correlation, row);

        if (! state_p->weightScaling_p.null()){
            theWeight = (* state_p->weightScaling_p) (theWeight);
        }

        return theWeight;
    }
}

Float
VisBufferImpl2::getWeightScaled (Int correlation, Int channel, Int row) const
{
    // Get the weight from the weightSpectrum if it is present (either it was
    // read from the MS or it was set by the user); otherwise get the weight
    // from the weight column.

    Float theWeight = 0;

    if (weightSpectrumPresent ()){

        theWeight = weightSpectrum () (correlation, channel, row);
    }
    else{
        theWeight = weight () (correlation, row);
    }

    // If there is a scaling function, the apply that to the weight

    if (! state_p->weightScaling_p.null()){
        theWeight = (* state_p->weightScaling_p) (theWeight);
    }

    return theWeight;
}

Float
VisBufferImpl2::getSigmaScaled (Int row) const
{
    Float sum = 0;
    Int n = nCorrelations();

    for (Int correlation = 0; correlation < n; ++ correlation){

        sum += getWeightScaled (correlation, row);
    }

    return sum / n;
}

Float
VisBufferImpl2::getSigmaScaled (Int correlation, Int row) const
{
    if (flagRow () (row)){
        return 0;
    }

    if (weightSpectrumPresent()){

        Float sum = 0;
        Int n = nChannels ();

        for (Int channel = 0; channel < n; ++ channel){

            sum += getWeightScaled (correlation, channel, row);

        }

        return sum / n;
    }
    else {

        Float theWeight = weight () (correlation, row);

        if (! state_p->weightScaling_p.null()){
            theWeight = (* state_p->weightScaling_p) (theWeight);
        }

        return theWeight;
    }
}

Float
VisBufferImpl2::getSigmaScaled (Int correlation, Int channel, Int row) const
{
    // Get the weight from the weightSpectrum if it is present (either it was
    // read from the MS or it was set by the user); otherwise get the weight
    // from the weight column.

    Float theWeight = 0;

    if (weightSpectrumPresent ()){

        theWeight = weightSpectrum () (correlation, channel, row);
    }
    else{
        theWeight = weight () (correlation, row);
    }

    // If there is a scaling function, the apply that to the weight

    if (! state_p->weightScaling_p.null()){
        theWeight = (* state_p->weightScaling_p) (theWeight);
    }

    return theWeight;
}

CountedPtr<WeightScaling>
VisBufferImpl2::getWeightScaling () const
{
    return state_p->weightScaling_p;
}

Bool
VisBufferImpl2::weightSpectrumPresent () const
{
    Bool present = cache_p->weightSpectrum_p.isPresent() ||
                   (isAttached() && getViiP()->weightSpectrumExists());

    return present;
}

Bool
VisBufferImpl2::sigmaSpectrumPresent () const
{
    Bool present = cache_p->sigmaSpectrum_p.isPresent() ||
                   (isAttached() && getViiP()->sigmaSpectrumExists());

    return present;
}


} // end namespace vi

using namespace casacore;
} // end namespace casa
