//# VisibilityIterator.cc: Step through MeasurementEquation by visibility
//# Copyright (C) 1996-2012
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This library is free software; you can redistribute it and/or modify it
//# under the terms of the GNU Library General Public License as published by
//# the Free Software Foundation; either version 2 of the License, or (at your
//# option) any later version.
//#
//# This library is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
//# License for more details.
//#
//# You should have received a copy of the GNU Library General Public License
//# along with this library; if not, write to the Free Software Foundation,
//# Inc., 675 Massachusetts Ave, Cambridge, MA 02139, USA.
//#
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//# $Id: VisibilityIterator.cc,v 19.15 2006/02/01 01:25:14 kgolap Exp $

#include <msvis/MSVis/VisibilityIteratorImpl.h>
#include <msvis/MSVis/VisibilityIterator.h>
#include <msvis/MSVis/VisBuffer.h>
#include <msvis/MSVis/UtilJ.h>
#include <msvis/MSVis/MSUtil.h>
////#include <synthesis/TransformMachines/VisModelData.h>
#include <scimath/Mathematics/InterpolateArray1D.h>
#include <ms/MeasurementSets/MSColumns.h>
#include <ms/MSSel/MSSpwIndex.h>
#include <tables/Tables/TableDesc.h>
#include <tables/Tables/ColDescSet.h>
#include <tables/Tables/TableRecord.h>
#include <tables/DataMan/TiledStManAccessor.h>
#include <tables/DataMan/StandardStManAccessor.h>
#include <tables/DataMan/IncrStManAccessor.h>
#include <casa/Arrays/ArrayLogical.h>
#include <casa/System/AipsrcValue.h>
#include <casa/BasicSL/Constants.h>
#include <casa/Quanta/MVTime.h>
#include <casa/Containers/Record.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/MaskedArray.h>
#include <casa/Exceptions/Error.h>
#include <casa/Utilities/Assert.h>
#include <casa/Utilities/Sort.h>
#include <casacore/casa/OS/EnvVar.h>

#include <cassert>
#include <limits>
#include <memory>

using std::make_pair;

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

SubChunkPair
SubChunkPair::noMoreData ()
{
    Int maxInt = std::numeric_limits<Int>::max ();
    return SubChunkPair (maxInt, maxInt);
}

String
SubChunkPair::toString () const
{
    return String::format ("(%d,%d)", first, second);
}

VisibilityIteratorReadImpl::VisibilityIteratorReadImpl ()
: rovi_p (NULL),
  selRows_p (0, 0),
  tileCacheIsSet_p (0)
{}

VisibilityIteratorReadImpl::VisibilityIteratorReadImpl (ROVisibilityIterator * rovi,
                                                        const Block<MeasurementSet> &mss,
                                                        const Block<Int> & sortColumns,
                                                        const Bool addDefaultSort,
                                                        Double timeInterval)
: addDefaultSort_p (addDefaultSort),
  curChanGroup_p (0),
  floatDataFound_p (false),
  initialized_p (false),
  msIterAtOrigin_p (false),
  msIter_p (mss, sortColumns, timeInterval, addDefaultSort, false),
  nChan_p (0),
  nRowBlocking_p (0),
  rovi_p (NULL),
  selRows_p (0, 0),
  sortColumns_p (sortColumns),
  stateOk_p (false),
  tileCacheIsSet_p (0),
  timeInterval_p (timeInterval)
{
    // Make sure the pointer to the containing ROVI (rovi_p) is NULL when calling initialize
    // otherwise the call back to the VI can result in it trying to use an uninitialized pointer
    // to this object (since it is in the process or being constructed).

    initialize (mss);

    rovi_p = rovi;

}

void
VisibilityIteratorReadImpl::initialize (const Block<MeasurementSet> &mss)
{

    AipsrcValue<Bool>::find (autoTileCacheSizing_p, VisibilityIterator::getAipsRcBase () + ".AutoTileCacheSizing", false);

    asyncEnabled_p = false;
    cache_p.lastazelUT_p = -1;
    cache_p.lastazel0UT_p = -1;
    cache_p.lasthourangUT_p = -1;
    cache_p.lastfeedpaUT_p = -1;
    cache_p.lastParangUT_p = -1;
    cache_p.lastParang0UT_p = -1;

    msCounter_p = 0;

    Int numMS = mss.nelements ();
    isMultiMS_p = numMS > 1;

    Block<Vector<Int> > blockNGroup (numMS);
    Block<Vector<Int> > blockStart (numMS);
    Block<Vector<Int> > blockWidth (numMS);
    Block<Vector<Int> > blockIncr (numMS);
    Block<Vector<Int> > blockSpw (numMS);

    measurementSets_p.clear ();

    for (Int k = 0; k < numMS; ++k) {

        MSSpWindowColumns msSpW (mss[k].spectralWindow ());

        Int nspw = msSpW.nrow ();

        blockNGroup[k].resize (nspw);
        blockNGroup[k].set (1);
        blockStart[k].resize (nspw);
        blockStart[k].set (0);
        blockWidth[k].resize (nspw);
        blockWidth[k] = msSpW.numChan ().getColumn ();
        blockIncr[k].resize (nspw);
        blockIncr[k].set (1);
        blockSpw[k].resize (nspw);
        indgen (blockSpw[k]);

        measurementSets_p.push_back (mss [k]);
    }

    selectChannel (blockNGroup, blockStart, blockWidth, blockIncr,
                  blockSpw);
}


VisibilityIteratorReadImpl::VisibilityIteratorReadImpl (const VisibilityIteratorReadImpl & other,
                                                        ROVisibilityIterator * rovi)
: rovi_p (rovi),
  selRows_p (other.selRows_p) // no default constructor so init it here
{
    operator=(other);
}

VisibilityIteratorReadImpl::~VisibilityIteratorReadImpl ()
{
}

VisibilityIteratorReadImpl &
VisibilityIteratorReadImpl::operator=(const VisibilityIteratorReadImpl & other)
{
    if (this == &other) {
        return *this;
    }

    cache_p = other.cache_p;
    channels_p = other.channels_p;
    columns_p = other.columns_p;
    msChannels_p = other.msChannels_p;
    velocity_p = other.velocity_p;

    addDefaultSort_p = other.addDefaultSort_p;
    channelGroupSize_p = other.channelGroupSize_p;
    curChanGroup_p = other.curChanGroup_p;
    curEndRow_p = other.curEndRow_p;
    curChanGroup_p = other.curChanGroup_p;
    curNumRow_p = other.curNumRow_p;
    curStartRow_p = other.curStartRow_p;
    curTableNumRow_p = other.curTableNumRow_p;
    floatDataFound_p = other.floatDataFound_p;
    imwgt_p = other.imwgt_p;
    initialized_p = other.initialized_p;
    isMultiMS_p = other.isMultiMS_p;
    measurementSets_p = other.measurementSets_p;
    more_p = other.more_p;
    msCounter_p = other.msCounter_p;
    msIterAtOrigin_p = other.msIterAtOrigin_p;
    msIter_p = other.msIter_p;
    msIter_p.origin();
    msd_p = other.msd_p;
    nAnt_p = other.nAnt_p;
    nChan_p = other.nChan_p;
    nPol_p = other.nPol_p;
    nRowBlocking_p = other.nRowBlocking_p;
    newChanGroup_p = other.newChanGroup_p;
    selRows_p = other.selRows_p;
    slicer_p = other.slicer_p;
    sortColumns_p = other.sortColumns_p;
    stateOk_p = other.stateOk_p;
    tileCacheIsSet_p.resize ();
    timeInterval_p = other.timeInterval_p;
    time_p.assign (other.time_p);
    useSlicer_p = other.useSlicer_p;
    weightSlicer_p = other.weightSlicer_p;

    return *this;
}

VisibilityIteratorReadImpl::Velocity &
VisibilityIteratorReadImpl::Velocity::operator= (const VisibilityIteratorReadImpl::Velocity & other)
{
    cFromBETA_p = other.cFromBETA_p;
    lsrFreq_p.assign (other.lsrFreq_p);
    nVelChan_p = other.nVelChan_p;
    selFreq_p = other.selFreq_p;
    vDef_p = other.vDef_p;
    vInc_p = other.vInc_p;
    vInterpolation_p = other.vInterpolation_p;
    vPrecise_p = other.vPrecise_p;
    vStart_p = other.vStart_p;
    velSelection_p = other.velSelection_p;

    return * this;
}

VisibilityIteratorReadImpl::Cache::Cache()
: flagOK_p (false),
  floatDataCubeOK_p (false),
  freqCacheOK_p (false),
  hourang_p (0),
  lastParang0UT_p (-1), // set last cache update as invalid
  lastParangUT_p (-1), // set last cache update as invalid
  lastazelUT_p (-1), // set last cache update as invalid
  lastazel0UT_p (-1), // set last cache update as invalid
  lasthourangUT_p (-1), // set last cache update as invalid
  lastfeedpaUT_p (-1), // set last cache update as invalid
  msHasFC_p(false),
  msHasWtSp_p (false),
  parang0_p (0),
  weightSpOK_p (false)
{}

VisibilityIteratorReadImpl::Cache &
VisibilityIteratorReadImpl::Cache::operator= (const VisibilityIteratorReadImpl::Cache & other)
{
    azel0_p = other.azel0_p;
    azel_p.assign (other.azel_p);
    feedpa_p.assign (other.feedpa_p);
    flagCube_p.assign (other.flagCube_p);
    flagOK_p = other.flagOK_p;
    floatDataCubeOK_p = other.floatDataCubeOK_p;
    floatDataCube_p.assign (other.floatDataCube_p);
    freqCacheOK_p = other.freqCacheOK_p;
    frequency_p.assign (frequency_p);
    hourang_p = other.hourang_p;
    lastazelUT_p = other.lastazelUT_p;
    lastazel0UT_p = other.lastazel0UT_p;
    lasthourangUT_p = other.lasthourangUT_p;
    lastfeedpaUT_p = other.lastfeedpaUT_p;
    lastParangUT_p = other.lastParangUT_p;
    lastParang0UT_p = other.lastParang0UT_p;
    msHasFC_p = other.msHasFC_p;
    msHasWtSp_p = other.msHasWtSp_p;
    parang0_p = other.parang0_p;
    parang_p.assign (other.parang_p);
    rowIds_p = other.rowIds_p;
    uvwMat_p.assign (other.uvwMat_p);
    visCube_p.assign (other.visCube_p);
    visOK_p = other.visOK_p;
    weightSpOK_p = other.weightSpOK_p;
    wtSp_p.assign (other.wtSp_p);

    return * this;
}

VisibilityIteratorReadImpl::Columns &
VisibilityIteratorReadImpl::Columns::operator= (const VisibilityIteratorReadImpl::Columns & other)
{
    antenna1_p.reference (other.antenna1_p);
    antenna2_p.reference (other.antenna2_p);
    corrVis_p.reference (other.corrVis_p);
    exposure_p.reference (other.exposure_p);
    feed1_p.reference (other.feed1_p);
    feed2_p.reference (other.feed2_p);
    flag_p.reference (other.flag_p);
    flagCategory_p.reference (other.flagCategory_p);
    flagRow_p.reference (other.flagRow_p);
    floatVis_p.reference (other.floatVis_p);
    modelVis_p.reference (other.modelVis_p);
    observation_p.reference (other.observation_p);
    processor_p.reference (other.processor_p);
    scan_p.reference (other.scan_p);
    sigma_p.reference (other.sigma_p);
    state_p.reference (other.state_p);
    time_p.reference (other.time_p);
    timeCentroid_p.reference (other.timeCentroid_p);
    timeInterval_p.reference (other.timeInterval_p);
    uvw_p.reference (other.uvw_p);
    vis_p.reference (other.vis_p);
    weight_p.reference (other.weight_p);
    weightSpectrum_p.reference (other.weightSpectrum_p);

    return * this;
}

VisibilityIteratorReadImpl::Velocity::Velocity ()
: nVelChan_p (0),
  velSelection_p (false),
  vPrecise_p (false)
{}


VisibilityIteratorReadImpl *
VisibilityIteratorReadImpl::clone (ROVisibilityIterator * rovi) const
{
    return new VisibilityIteratorReadImpl (* this, rovi);
}

void
VisibilityIteratorReadImpl::setAsyncEnabled (Bool enabled)
{
    asyncEnabled_p = enabled;
}

Bool
VisibilityIteratorReadImpl::isAsyncEnabled () const
{
    return asyncEnabled_p;
}


void
VisibilityIteratorReadImpl::setRowBlocking (Int nRow)
{
    nRowBlocking_p = nRow;
}

Bool
VisibilityIteratorReadImpl::existsColumn (VisBufferComponents::EnumType id) const
{
    Bool result;
    switch (id){

    case VisBufferComponents::Corrected:
    case VisBufferComponents::CorrectedCube:

        result = ! columns_p.corrVis_p.isNull();
        break;

    case VisBufferComponents::Model:
    case VisBufferComponents::ModelCube:

        result = ! columns_p.modelVis_p.isNull();
        break;

    case VisBufferComponents::Observed:
    case VisBufferComponents::ObservedCube:

        result = ! (columns_p.vis_p.isNull() && columns_p.floatVis_p.isNull());
        break;

    // RR: I can't tell if the other columns should checked for here or not.
    //     It's not true that all the other columns are required.
    //     existsFlagCategory uses caching anyway.
    // case VisBufferComponents::FlagCategory:
    //   result = false;
    //   if(!columns_p.flagCategory().isNull() &&
    //      columns_p.flagCategory().isDefined(0)){
    //     IPosition fcshape(columns_p.flagCategory().shape(0));
    //     IPosition fshape(columns_p.flag().shape(0));

    //     result = fcshape(0) == fshape(0) && fcshape(1) == fshape(1);
    //   }

    default:
        result = true; // required columns
    }

    return result;
}


void
VisibilityIteratorReadImpl::useImagingWeight (const VisImagingWeight & imWgt)
{
    imwgt_p = imWgt;
}
void
VisibilityIteratorReadImpl::origin ()
{

    if (!initialized_p) {
        originChunks ();
    } else {
        curChanGroup_p = 0;
        newChanGroup_p = true;
        curStartRow_p = 0;
        cache_p.freqCacheOK_p = false;
        cache_p.flagOK_p = false;
        cache_p.weightSpOK_p = false;
        cache_p.visOK_p.resize (3);
        cache_p.visOK_p = false;
        cache_p.floatDataCubeOK_p = false;
        setSelTable ();
        attachColumnsSafe (attachTable ());
        getTopoFreqs ();
        updateSlicer ();
        more_p = curChanGroup_p < curNGroups_p;
        // invalidate any attached VisBuffer
        if (!vbStack_p.empty ()) {
            vbStack_p.top ()->invalidate ();
        }

        subchunk_p.resetSubChunk ();
    }
}

void
VisibilityIteratorReadImpl::originChunks ()
{
    originChunks (false);
}


void
VisibilityIteratorReadImpl::originChunks (Bool forceRewind)
{
    initialized_p = true;
    subchunk_p.resetToOrigin();

    if (forceRewind) {
        msIterAtOrigin_p = false;
    }

    if (!msIterAtOrigin_p) {

        msIter_p.origin ();
        msIterAtOrigin_p = true;

        while ((! isInSelectedSPW (msIter_p.spectralWindowId ())) &&
                msIter_p.more ()) {

            msIter_p++;
        }

        stateOk_p = false;
        msCounter_p = msId ();

    }

    setState ();
    origin ();
    setTileCache ();
}

Bool
VisibilityIteratorReadImpl::isInSelectedSPW (const Int & spw)
{

    for (uInt k = 0; k < msChannels_p.spw_p[msId ()].nelements () ; ++k) {
        if (spw == msChannels_p.spw_p[msId ()][k]) {
            return true;
        }
    }
    return false;
}

void
VisibilityIteratorReadImpl::advance ()
{
    newChanGroup_p = false;
    cache_p.flagOK_p = false;
    cache_p.visOK_p = false;
    cache_p.floatDataCubeOK_p = false;
    cache_p.weightSpOK_p = false;
    curStartRow_p = curEndRow_p + 1;
    if (curStartRow_p >= curTableNumRow_p) {
        if (++ curChanGroup_p >= curNGroups_p) {
            curChanGroup_p--;
            more_p = false;
        } else {
            curStartRow_p = 0;
            newChanGroup_p = true;
            cache_p.freqCacheOK_p = false;
            updateSlicer ();
        }
    }
    if (more_p) {
        subchunk_p.incrementSubChunk();
        setSelTable ();
        getTopoFreqs ();
        // invalidate any attached VisBuffer
        if (!vbStack_p.empty ()) {
            vbStack_p.top ()->invalidate ();
        }
    }
}

SubChunkPair
VisibilityIteratorReadImpl::getSubchunkId () const
{
    return subchunk_p;
}

const Block<Int>& VisibilityIteratorReadImpl::getSortColumns() const
{
  return sortColumns_p;
}

VisibilityIteratorReadImpl &
VisibilityIteratorReadImpl::nextChunk ()
{

    if (msIter_p.more ()) {
        msIter_p++;
        if ((!isInSelectedSPW (msIter_p.spectralWindowId ()))) {
            while ( (!isInSelectedSPW (msIter_p.spectralWindowId ()))
                    && (msIter_p.more ())) {
                msIter_p++;
            }
            stateOk_p = false;
        }

        if (msIter_p.newMS ()) {
            msCounter_p = msId ();
            doChannelSelection ();
        }
        msIterAtOrigin_p = false;
        stateOk_p = false;
    }
    if (msIter_p.more ()) {
        subchunk_p.incrementChunk();
        setState ();
        getTopoFreqs ();
        if (!vbStack_p.empty ()) {
            vbStack_p.top ()->invalidate ();
        }
    }
    more_p = msIter_p.more ();
    return *this;
}

void
VisibilityIteratorReadImpl::setSelTable ()
{
    // work out how many rows to return
    // for the moment we return all rows with the same value for time
    // unless row blocking is set, in which case we return more rows at once.
    if (nRowBlocking_p > 0) {
        curEndRow_p = curStartRow_p + nRowBlocking_p;
        if (curEndRow_p >= curTableNumRow_p) {
            curEndRow_p = curTableNumRow_p - 1;
        }
    } else {
        for (curEndRow_p = curStartRow_p + 1; curEndRow_p < curTableNumRow_p &&
                time_p (curEndRow_p) == time_p (curEndRow_p - 1);
                curEndRow_p++) {
            ;
        }
        curEndRow_p--;
    }

    curNumRow_p = curEndRow_p - curStartRow_p + 1;
    selRows_p = RefRows (curStartRow_p, curEndRow_p);
    cache_p.rowIds_p.resize (0);
}

void
VisibilityIteratorReadImpl::getTopoFreqs ()
{
    if (velocity_p.velSelection_p) {

        // Convert selected velocities to TOPO frequencies.
        // First convert observatory vel to correct frame (for this time).

        msd_p.setEpoch (msIter_p.msColumns ().timeMeas ()(curStartRow_p));
        if (msIter_p.newMS ()) {
            msd_p.setObservatoryPosition (msIter_p.telescopePosition ());
        }

        MRadialVelocity obsRV = msd_p.obsVel (); // get obs velocity in required frame

        Double obsVel = velocity_p.cFromBETA_p (obsRV.toDoppler ()).getValue ().get ().getValue ();
        // convert to doppler in required definition and get out in m/s

        // Now compute corresponding TOPO freqs

        velocity_p.selFreq_p.resize (velocity_p.nVelChan_p);
        velocity_p.lsrFreq_p.resize (velocity_p.nVelChan_p);
        Double v0 = velocity_p.vStart_p.getValue ();
        Double dv = velocity_p.vInc_p.getValue ();

        if (aips_debug) {
            cout << "obsVel=" << obsVel << endl;
        }

        for (Int i = 0; i < velocity_p.nVelChan_p; i++) {

            Double vTopo = v0 + i * dv - obsVel;
            MDoppler dTopo (Quantity (vTopo, "m/s"), velocity_p.vDef_p);
            velocity_p.selFreq_p (i) = MFrequency::fromDoppler
                           (dTopo, msIter_p.restFrequency ().getValue ()).getValue ().getValue ();

            // also calculate the frequencies in the requested frame for matching
            // up with the image planes
            // (they are called lsr here, but don't need to be in that frame)

            MDoppler dLSR (Quantity (v0 + i * dv, "m/s"), velocity_p.vDef_p);
            const MFrequency & restFrequency = msIter_p.restFrequency ();
            velocity_p.lsrFreq_p (i) = MFrequency::fromDoppler (dLSR, restFrequency.getValue ()).getValue ().getValue ();
        }
    }
}

void
VisibilityIteratorReadImpl::getTopoFreqs (Vector<Double> & lsrFreq, Vector<Double> & selFreq)
{
    getTopoFreqs ();
    lsrFreq.assign (velocity_p.lsrFreq_p);
    selFreq.assign (velocity_p.selFreq_p);
}



void
VisibilityIteratorReadImpl::setState ()
{
    if (stateOk_p) {
        return;
    }

    curTableNumRow_p = msIter_p.table ().nrow ();
    // get the times for this (major) iteration, so we can do (minor)
    // iteration by constant time (needed for VisBuffer averaging).
    ScalarColumn<Double> lcolTime (msIter_p.table (), MS::columnName (MS::TIME));
    time_p.resize (curTableNumRow_p);
    lcolTime.getColumn (time_p);
    ScalarColumn<Double> lcolTimeInterval (msIter_p.table (),
                                            MS::columnName (MS::INTERVAL));
    ///////////timeInterval_p.resize (curTableNumRow_p);
    ///////////lcolTimeInterval.getColumn (timeInterval_p);
    curStartRow_p = 0;
    setSelTable ();
    attachColumnsSafe (attachTable ());
    // If this is a new MeasurementSet then set up the antenna locations
    if (msIter_p.newMS ()) {
        nAnt_p = msd_p.setAntennas (msIter_p.msColumns ().antenna ());
        cache_p.feedpa_p.resize (nAnt_p);
        cache_p.feedpa_p.set (0);
        cache_p.lastfeedpaUT_p = -1;
        cache_p.parang_p.resize (nAnt_p);
        cache_p.parang_p.set (0);
        cache_p.lastParangUT_p = -1;
        cache_p.parang0_p = 0;
        cache_p.lastParang0UT_p = -1;
        cache_p.azel_p.resize (nAnt_p);
        cache_p.lastazelUT_p = -1;
        cache_p.lastazel0UT_p = -1;
        cache_p.lasthourangUT_p = -1;

    }
    if (msIter_p.newField () || msIterAtOrigin_p) {
        msd_p.setFieldCenter (msIter_p.phaseCenter ());
    }
    if ( msIter_p.newDataDescriptionId () || msIterAtOrigin_p) {
        Int spw = msIter_p.spectralWindowId ();
        nChan_p = msColumns ().spectralWindow ().numChan ()(spw);
        nPol_p = msColumns ().polarization ().numCorr ()(msIter_p.polarizationId ());

        if (Int (channels_p.nGroups_p.nelements ()) <= spw ||
                channels_p.nGroups_p[spw] == 0) {
            // no selection set yet, set default = all
            // for a reference MS this will normally be set appropriately in VisSet
            selectChannel (1, 0, nChan_p);
        }
        channelGroupSize_p = channels_p.width_p[spw];
        curNGroups_p = channels_p.nGroups_p[spw];
        cache_p.freqCacheOK_p = false;
    }

    stateOk_p = true;
}

const MSDerivedValues &
VisibilityIteratorReadImpl::getMSD () const
{
    return msd_p;
}


void
VisibilityIteratorReadImpl::updateSlicer ()
{

    if (msIter_p.newMS ()) {
        channels_p.nGroups_p.resize (0, true, false);
        doChannelSelection ();
    }

    // set the Slicer to get the selected part of spectrum out of the table
    Int spw = msIter_p.spectralWindowId ();
    //Fixed what i think was a confusion between chanWidth and chanInc
    // 2007/11/12
    Int start = channels_p.start_p[spw] + curChanGroup_p * channels_p.width_p[spw];
    AlwaysAssert (start >= 0 && start + channelGroupSize_p <= nChan_p, AipsError);
    //  slicer_p=Slicer (Slice (),Slice (start,channelGroupSize_p));
    // above is slow, use IPositions instead.
    slicer_p = Slicer (IPosition (2, 0, start),
                      IPosition (2, nPol_p, channelGroupSize_p),
                      IPosition (2, 1, (channels_p.inc_p[spw] <= 0) ? 1 : channels_p.inc_p[spw] ));
    weightSlicer_p = Slicer (IPosition (1, start), IPosition (1, channelGroupSize_p),
                            IPosition (1, (channels_p.inc_p[spw] <= 0) ? 1 : channels_p.inc_p[spw]));
    useSlicer_p = channelGroupSize_p < nChan_p;

    //if (msIter_p.newDataDescriptionId ()){
    setTileCache ();
    //}
}



void
VisibilityIteratorReadImpl::setTileCache ()
{
    // This function sets the tile cache because of a feature in
    // sliced data access that grows memory dramatically in some cases
    //  if (useSlicer_p){

    if (autoTileCacheSizing_p){
        return; // rest of method does manual sizing so skip it
    }

    if (! (msIter_p.newDataDescriptionId () || msIter_p.newMS ()) ) {
        return;
    }

    const MeasurementSet & theMs = msIter_p.ms ();
    if (theMs.tableType () == Table::Memory) {
        return;
    }

    const ColumnDescSet & cds = theMs.tableDesc ().columnDescSet ();

    uInt startrow = msIter_p.table ().rowNumbers ()(0); // Get the first row number for this DDID.

    if (tileCacheIsSet_p.nelements () != 8) {
        tileCacheIsSet_p.resize (8);
        tileCacheIsSet_p.set (false);
    }

    Vector<String> columns (8);
    columns (0) = MS::columnName (MS::DATA);            // complex
    columns (1) = MS::columnName (MS::CORRECTED_DATA);  // complex
    columns (2) = MS::columnName (MS::MODEL_DATA);      // complex
    columns (3) = MS::columnName (MS::FLAG);            // boolean
    columns (4) = MS::columnName (MS::WEIGHT_SPECTRUM); // float
    columns (5) = MS::columnName (MS::WEIGHT);          // float
    columns (6) = MS::columnName (MS::SIGMA);           // float
    columns (7) = MS::columnName (MS::UVW);             // double

    for (uInt k = 0; k < columns.nelements (); ++k) {

        if (! cds.isDefined (columns (k)) || ! usesTiledDataManager (columns[k], theMs)){
            continue;
        }

        try {
            //////////////////
            //////Temporary fix for virtual ms of multiple real ms's ...miracle of statics
            //////setting the cache size of hypercube at row 0 of each ms.
            ///will not work if each subms of a virtual ms has multi hypecube being
            ///accessed.
            if (theMs.tableInfo ().subType () == "CONCATENATED" &&
                msIterAtOrigin_p &&
                ! tileCacheIsSet_p[k]) {

                Block<String> refTables = theMs.getPartNames (true);

                for (uInt kk = 0; kk < refTables.nelements (); ++kk) {

                    Table elms (refTables[kk]);
                    ROTiledStManAccessor tacc (elms, columns[k], true);

		    // Cleverly sense full-row cache size (in tiles)
		    uInt cacheSizeInTiles(1);

		    // Is startrow correct for each subMS?
		    //uInt rRow=startrow;
		    uInt rRow=0;    // This preserves previous "single-hypercube" assumption
		    		    
		    IPosition hypercubeShape=tacc.hypercubeShape(rRow);
		    IPosition tileShape=tacc.tileShape(rRow);
		    uInt nax=hypercubeShape.size();  // how many axes
		    
		    // Accumulate axis factors up to--but NOT including--row (last) axis
		    //  "ceil" catches partially filled tiles...
		    for (uInt iax=0;iax<nax-1;++iax)    
		      cacheSizeInTiles*= (uInt) ceil(hypercubeShape[iax]/ (Float)(tileShape[iax]));
		    
		    // Double for ALMA data
		    //   (this satisfies cases where required rows require >1 non-contiguous tiles)
		    cacheSizeInTiles*=2;

		    // Now set it
                    tacc.setCacheSize (rRow, cacheSizeInTiles);  
                    tileCacheIsSet_p[k] = true;
                    //cerr << "set cache on kk " << kk << " vol " << columns[k] << "  " << refTables[kk] << endl;
                }
            }
            else {

                ROTiledStManAccessor tacc (theMs, columns[k], true);


		// Cleverly sense full-row cache size (in tiles)
		uInt cacheSizeInTiles(1);

		IPosition hypercubeShape=tacc.hypercubeShape(startrow);
		IPosition tileShape=tacc.tileShape(startrow);
		uInt nax=hypercubeShape.size();  // how many axes

		// Accumulate axis factors up to--but NOT including--row (last) axis
		//  "ceil" catches partially filled tiles...
		for (uInt iax=0;iax<nax-1;++iax)    
		  cacheSizeInTiles*= (uInt) ceil(hypercubeShape[iax]/ (Float)(tileShape[iax]));

		// Double for ALMA data
		//   (this satisfies cases where required rows require >1 non-contiguous tiles)
		cacheSizeInTiles*=2;

	        Bool setCache = true;
                if (! useSlicer_p){        // always set cache if slicer in use
                    for (uInt jj = 0 ; jj <  tacc.nhypercubes (); ++jj) {
                        if (tacc.getBucketSize (jj) == 0) {
                            setCache = false;
                        }
                    }
                }


		/// If some bucketSize is 0...there is trouble in setting cache
                /// but if slicer is used it gushes anyways if one does not set cache
		/// need to fix the 0 bucket size in the filler anyways...then this is not needed
                if (setCache) {
		  /*
		  cout << columns[k] 
		       << " bucketSize=" << tacc.bucketSize(startrow)
		       << " hcShape=" << hypercubeShape
		       << " tShape=" << tileShape
		       << " cacheSizeInTiles = " << cacheSizeInTiles << "  (nhypercubes=" << tacc.nhypercubes() <<")"<< endl;
		  */
                    if (tacc.nhypercubes () == 1) {
                        tacc.setCacheSize (0, cacheSizeInTiles);
                    } else {
                        tacc.setCacheSize (startrow, cacheSizeInTiles);
                    }
                }



	    }
        }
        catch (AipsError x) {
            //  cerr << "Data man type " << dataManType << "  " << dataManType.contains ("Tiled") << "  && " << (!String (cdesc.dataManagerGroup ()).empty ()) << endl;
            //  cerr << "Failed to set settilecache due to " << x.getMesg () << " column " << columns[k]  <<endl;
            //It failed so leave the caching as is
            continue;
        }
    }
}

Bool
VisibilityIteratorReadImpl::usesTiledDataManager (const String & columnName,
                                                  const MeasurementSet & theMs) const
{
    Bool noData = false;

    // Have to do something special about weight_spectrum as it tend to exist but
    // has no valid data.

    noData = noData ||
             (columnName == MS::columnName (MS::WEIGHT_SPECTRUM) && ! existsWeightSpectrum ());

    // Check to see if the column exist and have valid data

    noData = noData ||
             (columnName == MS::columnName (MS::DATA) &&
              (columns_p.vis_p.isNull () || ! columns_p.vis_p.isDefined (0)));

    noData = noData ||
             (columnName == MS::columnName (MS::MODEL_DATA) &&
              (columns_p.modelVis_p.isNull () || ! columns_p.modelVis_p.isDefined (0)));

    noData = noData ||
             (columnName == MS::columnName (MS::CORRECTED_DATA) &&
              (columns_p.corrVis_p.isNull () || ! columns_p.corrVis_p.isDefined (0)));

    noData = noData ||
             (columnName == MS::columnName (MS::FLAG) &&
              (columns_p.flag_p.isNull () || ! columns_p.flag_p.isDefined (0)));

    noData = noData ||
             (columnName == MS::columnName (MS::WEIGHT) &&
              (columns_p.weight_p.isNull () || ! columns_p.weight_p.isDefined (0)));

    noData = noData ||
             (columnName == MS::columnName (MS::SIGMA) &&
              (columns_p.sigma_p.isNull () || ! columns_p.sigma_p.isDefined (0)));

    noData = noData ||
             (columnName == MS::columnName (MS::UVW) &&
              (columns_p.uvw_p.isNull () || ! columns_p.uvw_p.isDefined (0)));

    Bool usesTiles = false;

    if (! noData){
        String dataManType = RODataManAccessor (theMs, columnName, true).dataManagerType ();

        usesTiles = dataManType.contains ("Tiled");
    }

    return usesTiles;
}


/*
void VisibilityIteratorReadImpl::setTileCache (){
  // This function sets the tile cache because of a feature in
  // sliced data access that grows memory dramatically in some cases
  //if (useSlicer_p){

  {
    const MeasurementSet& thems=msIter_p.ms ();
    const ColumnDescSet& cds=thems.tableDesc ().columnDescSet ();
    ArrayColumn<Complex> columns_p.vis_p;
    ArrayColumn<Float> colwgt;
    Vector<String> columns (3);
    columns (0)=MS::columnName (MS::DATA);
    columns (1)=MS::columnName (MS::CORRECTED_DATA);
    columns (2)=MS::columnName (MS::MODEL_DATA);
    //cout << "COL " << columns << endl;
    for (uInt k=0; k< 3; ++k){
      //cout << "IN loop k " << k << endl;
      if (thems.tableDesc ().isColumn (columns (k)) ) {

	columns_p.vis_p.attach (thems,columns (k));
	String dataManType;
	dataManType = columns_p.vis_p.columnDesc ().dataManagerType ();
	//cout << "dataManType " << dataManType << endl;
	if (dataManType.contains ("Tiled")){

	  ROTiledStManAccessor tacc (thems,
				    columns_p.vis_p.columnDesc ().dataManagerGroup ());
	  uInt nHyper = tacc.nhypercubes ();
	  // Find smallest tile shape
	  Int lowestProduct = 0;
	  Int lowestId = 0;
	  Bool firstFound = false;
	  for (uInt id=0; id < nHyper; id++) {
	    Int product = tacc.getTileShape (id).product ();
	    if (product > 0 && (!firstFound || product < lowestProduct)) {
	      lowestProduct = product;
	      lowestId = id;
	      if (!firstFound) firstFound = true;
	    }
	  }
	  Int nchantile;
	  IPosition tileshape=tacc.getTileShape (lowestId);
	  IPosition axisPath (3,2,0,1);
	  //nchantile=tileshape (1);
	  tileshape (1)=channelGroupSize_p;
	  tileshape (2)=curNumRow_p;
	  //cout << "cursorshape " << tileshape << endl;
	  nchantile=tacc.calcCacheSize (0, tileshape, axisPath);

	//  if (nchantile > 0)
	 //   nchantile=channelGroupSize_p/nchantile*10;
	 // if (nchantile<3)
	  //  nchantile=10;

	  ///////////////
	  //nchantile *=8;
	  nchantile=1;
	  //tileshape (2)=tileshape (2)*8;
	  //////////////
	  //cout << tacc.cacheSize (0) << " nchantile "<< nchantile << " max cache size " << tacc.maximumCacheSize () << endl;
	  tacc.clearCaches ();
	  tacc.setCacheSize (0, 1);
	  //tacc.setCacheSize (0, tileshape, axisPath);
	  //cout << k << "  " << columns (k) << " cache size  " <<  tacc.cacheSize (0) << endl;

	}
      }
    }
  }

}
 */

void
VisibilityIteratorReadImpl::attachColumnsSafe (const Table & t)
{
    // Normally, the call to attachColumns is redirected back to the ROVI class.
    // This allows writable VIs to attach columns in both the read and write impls.
    // However, this referral to the ROVI doesn't work during construction of this
    // class (VIRI) since there is as yet no pointer to the object under construction.
    // In that case, simply perform it locally.

    if (rovi_p == NULL){
        attachColumns (t);
    }
    else{
        rovi_p->attachColumns (t);
    }
}


void
VisibilityIteratorReadImpl::attachColumns (const Table & t)
{
    const ColumnDescSet & cds = t.tableDesc ().columnDescSet ();

    columns_p.antenna1_p.attach (t, MS::columnName (MS::ANTENNA1));
    columns_p.antenna2_p.attach (t, MS::columnName (MS::ANTENNA2));

    if (cds.isDefined ("CORRECTED_DATA")) {
        columns_p.corrVis_p.attach (t, "CORRECTED_DATA");
    }

    columns_p.exposure_p.attach (t, MS::columnName (MS::EXPOSURE));
    columns_p.feed1_p.attach (t, MS::columnName (MS::FEED1));
    columns_p.feed2_p.attach (t, MS::columnName (MS::FEED2));
    columns_p.flag_p.attach (t, MS::columnName (MS::FLAG));
    columns_p.flagCategory_p.attach (t, MS::columnName (MS::FLAG_CATEGORY));
    columns_p.flagRow_p.attach (t, MS::columnName (MS::FLAG_ROW));

    if (cds.isDefined (MS::columnName (MS::FLOAT_DATA))) {
        columns_p.floatVis_p.attach (t, MS::columnName (MS::FLOAT_DATA));
        floatDataFound_p = true;
    } else {
        floatDataFound_p = false;
    }

    if (cds.isDefined ("MODEL_DATA")) {
        columns_p.modelVis_p.attach (t, "MODEL_DATA");
    }

    columns_p.observation_p.attach (t, MS::columnName (MS::OBSERVATION_ID));
    columns_p.processor_p.attach (t, MS::columnName (MS::PROCESSOR_ID));
    columns_p.scan_p.attach (t, MS::columnName (MS::SCAN_NUMBER));
    columns_p.sigma_p.attach (t, MS::columnName (MS::SIGMA));
    columns_p.state_p.attach (t, MS::columnName (MS::STATE_ID));
    columns_p.time_p.attach (t, MS::columnName (MS::TIME));
    columns_p.timeCentroid_p.attach (t, MS::columnName (MS::TIME_CENTROID));
    columns_p.timeInterval_p.attach (t, MS::columnName (MS::INTERVAL));
    columns_p.uvw_p.attach (t, MS::columnName (MS::UVW));

    if (cds.isDefined (MS::columnName (MS::DATA))) {
        columns_p.vis_p.attach (t, MS::columnName (MS::DATA));
    }

    columns_p.weight_p.attach (t, MS::columnName (MS::WEIGHT));

    if (cds.isDefined ("WEIGHT_SPECTRUM")) {
        columns_p.weightSpectrum_p.attach (t, "WEIGHT_SPECTRUM");
    }
}

void
VisibilityIteratorReadImpl::update_rowIds () const
{
    if (cache_p.rowIds_p.nelements () == 0) {
        cache_p.rowIds_p = selRows_p.convert ();

        Vector<uInt> msIter_rowIds (msIter_p.table ().rowNumbers (msIter_p.ms ()));

        for (uInt i = 0; i < cache_p.rowIds_p.nelements (); i++) {
            cache_p.rowIds_p (i) = msIter_rowIds (cache_p.rowIds_p (i));
        }
    }
    return;
}


Int
VisibilityIteratorReadImpl::getDataDescriptionId () const
{
    return msIter_p.dataDescriptionId ();
}


const MeasurementSet &
VisibilityIteratorReadImpl::getMeasurementSet () const
{
    return msIter_p.ms ();
}

Int
VisibilityIteratorReadImpl::getMeasurementSetId () const
{
    return msIter_p.msId ();
}


Int
VisibilityIteratorReadImpl::getNAntennas () const
{
    Int nAntennas = msIter_p.receptorAngle ().shape ()(1);

    return nAntennas;
}

MEpoch
VisibilityIteratorReadImpl::getEpoch () const
{
    MEpoch mEpoch = msIter_p.msColumns ().timeMeas ()(0);

    return mEpoch;
}

Vector<Float>
VisibilityIteratorReadImpl::getReceptor0Angle ()
{
    Int nAntennas = getNAntennas ();

    Vector<Float> receptor0Angle (nAntennas);

    for (int i = 0; i < nAntennas; i++) {
        receptor0Angle [i] = msIter_p.receptorAngle ()(0, i);
    }

    return receptor0Angle;
}

Vector<rownr_t>
VisibilityIteratorReadImpl::getRowIds () const
{
    update_rowIds ();

    return cache_p.rowIds_p;
}


Vector<rownr_t> &
VisibilityIteratorReadImpl::rowIds (Vector<rownr_t> & rowids) const
{
    /* Calculate the row numbers in the original MS only when needed,
     i.e. when this function is called */
    update_rowIds ();
    rowids.resize (cache_p.rowIds_p.nelements ());
    rowids = cache_p.rowIds_p;
    return rowids;
}


Vector<Int> &
VisibilityIteratorReadImpl::antenna1(Vector<Int> & ant1) const
{
    ant1.resize (curNumRow_p);
    getCol (columns_p.antenna1_p, ant1);
    return ant1;
}

Vector<Int> &
VisibilityIteratorReadImpl::antenna2(Vector<Int> & ant2) const
{
    ant2.resize (curNumRow_p);
    getCol (columns_p.antenna2_p, ant2);
    return ant2;
}

Vector<Int> &
VisibilityIteratorReadImpl::feed1(Vector<Int> & fd1) const
{
    fd1.resize (curNumRow_p);
    getCol (columns_p.feed1_p, fd1);
    return fd1;
}

Vector<Int> &
VisibilityIteratorReadImpl::feed2(Vector<Int> & fd2) const
{
    fd2.resize (curNumRow_p);
    getCol (columns_p.feed2_p, fd2);
    return fd2;
}

Vector<Int> &
VisibilityIteratorReadImpl::channel (Vector<Int> & chan) const
{
    Int spw = msIter_p.spectralWindowId ();
    chan.resize (channelGroupSize_p);
    Int inc = channels_p.inc_p[spw] <= 0 ? 1 : channels_p.inc_p[spw];
    for (Int i = 0; i < channelGroupSize_p; i++) {
        chan (i) = channels_p.start_p[spw] + curChanGroup_p * channels_p.width_p[spw] + i * inc;
    }
    return chan;
}

Vector<Int> &
VisibilityIteratorReadImpl::corrType (Vector<Int> & corrTypes) const
{
    Int polId = msIter_p.polarizationId ();
    msIter_p.msColumns ().polarization ().corrType ().get (polId, corrTypes, true);
    return corrTypes;
}

Cube<Bool> &
VisibilityIteratorReadImpl::flag (Cube<Bool> & flags) const
{
    if (useSlicer_p) {
        getCol (columns_p.flag_p, slicer_p, flags, true);
    } else {
        getCol (columns_p.flag_p, flags, true);
    }
    return flags;
}

Matrix<Bool> &
VisibilityIteratorReadImpl::flag (Matrix<Bool> & flags) const
{
    if (useSlicer_p) {
        getCol (columns_p.flag_p, slicer_p, cache_p.flagCube_p, true);
    } else {
        getCol (columns_p.flag_p, cache_p.flagCube_p, true);
    }

    flags.resize (channelGroupSize_p, curNumRow_p);
    // need to optimize this...
    //for (Int row=0; row<curNumRow_p; row++) {
    //  for (Int chn=0; chn<channelGroupSize_p; chn++) {
    //    flags (chn,row)=flagCube (0,chn,row);
    //    for (Int pol=1; pol<nPol_p; pol++) {
    //	  flags (chn,row)|=flagCube (pol,chn,row);
    //    }
    //  }
    //}
    Bool deleteIt1;
    Bool deleteIt2;
    const Bool * pcube = cache_p.flagCube_p.getStorage (deleteIt1);
    Bool * pflags = flags.getStorage (deleteIt2);
    for (uInt row = 0; row < curNumRow_p; row++) {
        for (Int chn = 0; chn < channelGroupSize_p; chn++) {
            *pflags = *pcube++;
            for (Int pol = 1; pol < nPol_p; pol++, pcube++) {
                *pflags = *pcube ? *pcube : *pflags;
            }
            pflags++;
        }
    }
    cache_p.flagCube_p.freeStorage (pcube, deleteIt1);
    flags.putStorage (pflags, deleteIt2);
    return flags;
}

Bool VisibilityIteratorReadImpl::existsFlagCategory() const
{
  if(msIter_p.newMS()){ // Cache to avoid testing unnecessarily.
    try{
      cache_p.msHasFC_p = columns_p.flagCategory_p.hasContent();
    }
    catch (AipsError x){
      cache_p.msHasFC_p = false;
    }
  }
  return cache_p.msHasFC_p;
}

Array<Bool> &
VisibilityIteratorReadImpl::flagCategory (Array<Bool> & flagCategories) const
{
    if (columns_p.flagCategory_p.isNull () || !columns_p.flagCategory_p.isDefined (0)) { // It often is.
        flagCategories.resize ();    // Zap it.
    } else {
        if (velocity_p.velSelection_p) {
            throw (AipsError ("velocity selection not allowed in flagCategory ()."));
        } else {
            if (useSlicer_p) {
                getCol (columns_p.flagCategory_p, slicer_p, flagCategories, true);
            } else {
                getCol (columns_p.flagCategory_p, flagCategories, true);
            }
        }
    }
    return flagCategories;
}

Vector<Bool> &
VisibilityIteratorReadImpl::flagRow (Vector<Bool> & rowflags) const
{
    rowflags.resize (curNumRow_p);
    getCol (columns_p.flagRow_p, rowflags);
    return rowflags;
}

Vector<Int> &
VisibilityIteratorReadImpl::observationId (Vector<Int> & obsIDs) const
{
    obsIDs.resize (curNumRow_p);
    getCol (columns_p.observation_p, obsIDs);
    return obsIDs;
}

Vector<Int> &
VisibilityIteratorReadImpl::processorId (Vector<Int> & procIDs) const
{
    procIDs.resize (curNumRow_p);
    getCol (columns_p.processor_p, procIDs);
    return procIDs;
}

Vector<Int> &
VisibilityIteratorReadImpl::scan (Vector<Int> & scans) const
{
    scans.resize (curNumRow_p);
    getCol (columns_p.scan_p, scans);
    return scans;
}

Vector<Int> &
VisibilityIteratorReadImpl::stateId (Vector<Int> & stateIds) const
{
    stateIds.resize (curNumRow_p);
    getCol (columns_p.state_p, stateIds);
    return stateIds;
}

Vector<Double> &
VisibilityIteratorReadImpl::frequency (Vector<Double> & freq) const
{
    if (velocity_p.velSelection_p) {
        freq.resize (velocity_p.nVelChan_p);
        freq = velocity_p.selFreq_p;
    } else {
        if (! cache_p.freqCacheOK_p) {
            cache_p.freqCacheOK_p = true;
            Int spw = msIter_p.spectralWindowId ();
            cache_p.frequency_p.resize (channelGroupSize_p);
            const Vector<Double> & chanFreq = msIter_p.frequency ();
            Int start = channels_p.start_p[spw];
            Int inc = channels_p.inc_p[spw] <= 0 ? 1 : channels_p.inc_p[spw];
            for (Int i = 0; i < channelGroupSize_p; i++) {
                cache_p.frequency_p (i) = chanFreq (start + curChanGroup_p * channels_p.width_p[spw] + i * inc);
            }
        }
        freq.resize (channelGroupSize_p);
        freq = cache_p.frequency_p;
    }
    return freq;
}


Vector<Double> &
VisibilityIteratorReadImpl::time (Vector<Double> & t) const
{
    t.resize (curNumRow_p);

    getCol (columns_p.time_p, t);

    return t;
}

Vector<Double> &
VisibilityIteratorReadImpl::timeCentroid (Vector<Double> & t) const
{
    t.resize (curNumRow_p);
    getCol (columns_p.timeCentroid_p, t);
    return t;
}

Vector<Double> &
VisibilityIteratorReadImpl::timeInterval (Vector<Double> & t) const
{
    t.resize (curNumRow_p);
    getCol (columns_p.timeInterval_p, t);
    return t;
}

Vector<Double> &
VisibilityIteratorReadImpl::exposure (Vector<Double> & expo) const
{
    expo.resize (curNumRow_p);
    getCol (columns_p.exposure_p, expo);
    return expo;
}

Cube<Complex> &
VisibilityIteratorReadImpl::visibility (Cube<Complex> & vis, DataColumn whichOne) const
{

    if (useSlicer_p) {
        getDataColumn (whichOne, slicer_p, vis);
    } else {
        getDataColumn (whichOne, vis);
    }

    return vis;
}


// helper function to swap the y and z axes of a Cube
void
swapyz (Cube<Complex> & out, const Cube<Complex> & in)
{
    IPosition inShape = in.shape ();
    uInt nx = inShape (0), ny = inShape (2), nz = inShape (1);
    out.resize (nx, ny, nz);
    Bool deleteIn, deleteOut;
    const Complex * pin = in.getStorage (deleteIn);
    Complex * pout = out.getStorage (deleteOut);
    uInt i = 0, zOffset = 0;
    for (uInt iz = 0; iz < nz; iz++, zOffset += nx) {
        Int yOffset = zOffset;
        for (uInt iy = 0; iy < ny; iy++, yOffset += nx * nz) {
            for (uInt ix = 0; ix < nx; ix++) {
                pout[i++] = pin[ix + yOffset];
            }
        }
    }
    out.putStorage (pout, deleteOut);
    in.freeStorage (pin, deleteIn);
}

// helper function to swap the y and z axes of a Cube
void
swapyz (Cube<Bool> & out, const Cube<Bool> & in)
{
    IPosition inShape = in.shape ();
    uInt nx = inShape (0), ny = inShape (2), nz = inShape (1);
    out.resize (nx, ny, nz);
    Bool deleteIn, deleteOut;
    const Bool * pin = in.getStorage (deleteIn);
    Bool * pout = out.getStorage (deleteOut);
    uInt i = 0, zOffset = 0;
    for (uInt iz = 0; iz < nz; iz++, zOffset += nx) {
        Int yOffset = zOffset;
        for (uInt iy = 0; iy < ny; iy++, yOffset += nx * nz) {
            for (uInt ix = 0; ix < nx; ix++) {
                pout[i++] = pin[ix + yOffset];
            }
        }
    }
}

Cube<Float> &
VisibilityIteratorReadImpl::floatData (Cube<Float> & fcube) const
{
    if (useSlicer_p) {
        getFloatDataColumn (slicer_p, fcube);
    } else {
        getFloatDataColumn (fcube);
    }
    return fcube;
}

// transpose a matrix
void
transpose (Matrix<Float> & out, const Matrix<Float> & in)
{
    uInt ny = in.nrow (), nx = in.ncolumn ();
    out.resize (nx, ny);
    Bool deleteIn, deleteOut;
    const Float * pin = in.getStorage (deleteIn);
    Float * pout = out.getStorage (deleteOut);
    uInt i = 0;
    for (uInt iy = 0; iy < ny; iy++) {
        uInt yOffset = 0;
        for (uInt ix = 0; ix < nx; ix++, yOffset += ny) {
            pout[i++] = pin[iy + yOffset];
        }
    }
    out.putStorage (pout, deleteOut);
    in.freeStorage (pin, deleteIn);
}

void
VisibilityIteratorReadImpl::getDataColumn (DataColumn whichOne,
                                          const Slicer & slicer,
                                          Cube<Complex> & data) const
{


    // Return the visibility (observed, model or corrected);
    // deal with DATA and FLOAT_DATA seamlessly for observed data.
    switch (whichOne) {

    case ROVisibilityIterator::Observed:
        if (floatDataFound_p) {
            Cube<Float> dataFloat;
            getCol (columns_p.floatVis_p, slicer, dataFloat, true);
            data.resize (dataFloat.shape ());
            convertArray (data, dataFloat);
        } else {
            getCol (columns_p.vis_p, slicer, data, true);
        }
        break;

    case ROVisibilityIterator::Corrected:
        getCol (columns_p.corrVis_p, slicer, data, true);
        break;

    case ROVisibilityIterator::Model:
        getCol (columns_p.modelVis_p, slicer, data, true);
        break;

    default:
        Assert (false);
    }

}

void
VisibilityIteratorReadImpl::getDataColumn (DataColumn whichOne,
                                          Cube<Complex> & data) const
{
    // Return the visibility (observed, model or corrected);
    // deal with DATA and FLOAT_DATA seamlessly for observed data.

    switch (whichOne) {

    case ROVisibilityIterator::Observed:
        if (floatDataFound_p) {
            Cube<Float> dataFloat;
            getCol (columns_p.floatVis_p, dataFloat, true);
            data.resize (dataFloat.shape ());
            convertArray (data, dataFloat);
        } else {
            getCol (columns_p.vis_p, data, true);
        }
        break;

    case ROVisibilityIterator::Corrected:
        getCol (columns_p.corrVis_p, data, true);
        break;

    case ROVisibilityIterator::Model:
        getCol (columns_p.modelVis_p, data, true);
        break;

    default:
        Assert (false);
    }
}

void
VisibilityIteratorReadImpl::getFloatDataColumn (const Slicer & slicer,
                                               Cube<Float> & data) const
{
    // Return FLOAT_DATA as real Floats.
    if (floatDataFound_p) {
        getCol (columns_p.floatVis_p, slicer, data, true);
    }
}

void
VisibilityIteratorReadImpl::getFloatDataColumn (Cube<Float> & data) const
{
    // Return FLOAT_DATA as real Floats.
    if (floatDataFound_p) {
        getCol (columns_p.floatVis_p, data, true);
    }
}

Matrix<CStokesVector> &
VisibilityIteratorReadImpl::visibility (Matrix<CStokesVector> & vis,
                                       DataColumn whichOne) const
{
    if (useSlicer_p) {
        getDataColumn (whichOne, slicer_p, cache_p.visCube_p);
    } else {
        getDataColumn (whichOne, cache_p.visCube_p);
    }

    vis.resize (channelGroupSize_p, curNumRow_p);
    Bool deleteIt;
    Complex * pcube = cache_p.visCube_p.getStorage (deleteIt);
    if (deleteIt) {
        cerr << "Problem in ROVisIter::visibility - deleteIt true" << endl;
    }
    // Here we cope in a limited way with cases where not all 4
    // polarizations are present: if only 2, assume XX,YY or RR,LL
    // if only 1, assume it's an estimate of Stokes I (one of RR,LL,XX,YY)
    // The cross terms are zero filled in these cases.
    switch (nPol_p) {
    case 4: {
        for (uInt row = 0; row < curNumRow_p; row++) {
            for (Int chn = 0; chn < channelGroupSize_p; chn++, pcube += 4) {
                vis (chn, row) = pcube;
            }
        }
        break;
    }
    case 2: {
        vis.set (CStokesVector (Complex (0., 0.)));
        for (uInt row = 0; row < curNumRow_p; row++) {
            for (Int chn = 0; chn < channelGroupSize_p; chn++, pcube += 2) {
                CStokesVector & v = vis (chn, row);
                v (0) = *pcube;
                v (3) = *(pcube + 1);
            }
        }
        break;
    }
    case 1: {
        vis.set (CStokesVector (Complex (0., 0.)));
        for (uInt row = 0; row < curNumRow_p; row++) {
            for (Int chn = 0; chn < channelGroupSize_p; chn++, pcube++) {
                CStokesVector & v = vis (chn, row);
                v (0) = v (3) = *pcube;
            }
        }
    } //# case 1
    } //# switch
    return vis;
}

Vector<RigidVector<Double, 3> > &
VisibilityIteratorReadImpl::uvw (Vector<RigidVector<Double, 3> > & uvwvec) const
{
    uvwvec.resize (curNumRow_p);
    getColArray<Double>(columns_p.uvw_p, cache_p.uvwMat_p, true);
    // get a pointer to the raw storage for quick access
    Bool deleteIt;
    Double * pmat = cache_p.uvwMat_p.getStorage (deleteIt);
    for (uInt row = 0; row < curNumRow_p; row++, pmat += 3) {
        uvwvec (row) = pmat;
    }
    return uvwvec;
}

Matrix<Double> &
VisibilityIteratorReadImpl::uvwMat (Matrix<Double> & uvwmat) const
{
    getCol (columns_p.uvw_p, uvwmat, true);
    return uvwmat;
}

// Fill in parallactic angle.
Vector<Float>
VisibilityIteratorReadImpl::feed_pa (Double time) const
{
    //  LogMessage message (LogOrigin ("VisibilityIteratorReadImpl","feed_pa"));

    // Absolute UT
    Double ut = time;

    if (ut != cache_p.lastfeedpaUT_p) {

        // Set up the Epoch using the absolute MJD in seconds
        // get the Epoch reference from the column

        MEpoch mEpoch = getEpoch ();

        const Matrix<Double> & angles = receptorAngles ().xyPlane(0);
        Int nAnt = angles.shape ()(1);

        Vector<Float> receptor0Angle (nAnt, 0);

        for (int i = 0; i < nAnt; i++) {
            receptor0Angle [i] = angles (0, i);
        }

        cache_p.feedpa_p.assign (feed_paCalculate (time, msd_p, nAnt, mEpoch, receptor0Angle));

        cache_p.lastfeedpaUT_p = ut;
    }
    return cache_p.feedpa_p;
}

Vector<Float>
VisibilityIteratorReadImpl::feed_paCalculate (Double time, MSDerivedValues & msd,
                                              Int nAntennas, const MEpoch & mEpoch0,
                                              const Vector<Float> & receptor0Angle)
{
    MEpoch mEpoch = mEpoch0;

    mEpoch.set (MVEpoch (Quantity (time, "s")));

    msd.setEpoch (mEpoch);

    // Calculate pa for all antennas.

    Vector<Float> feedpa (nAntennas);

    for (Int iant = 0; iant < nAntennas; iant++) {

        msd.setAntenna (iant);
        feedpa (iant) = msd.parAngle ();

        // add angle for receptor 0

        feedpa (iant) += receptor0Angle (iant);

        if (aips_debug && iant == 0) {

            cout << "Antenna " << iant << " at time: " << MVTime (mEpoch.getValue ()) <<
                    " has PA = " << feedpa (iant) * 57.28 << endl;
        }
    }

    return feedpa;
}

// Fill in parallactic angle.
const Float &
VisibilityIteratorReadImpl::parang0(Double time) const
{
    //  LogMessage message (LogOrigin ("VisibilityIteratorReadImpl","parang0"));

    // Absolute UT
    Double ut = time;

    if (ut != cache_p.lastParang0UT_p) {

        cache_p.lastParang0UT_p = ut;

        // Set up the Epoch using the absolute MJD in seconds
        // get the Epoch reference from the column
        MEpoch mEpoch = msIter_p.msColumns ().timeMeas ()(0);
        cache_p.parang0_p = parang0Calculate (time, msd_p, mEpoch);
    }
    return cache_p.parang0_p;
}

Float
VisibilityIteratorReadImpl::parang0Calculate (Double time, MSDerivedValues & msd, const MEpoch & mEpoch0)
{
    MEpoch mEpoch = mEpoch0;

    mEpoch.set (MVEpoch (Quantity (time, "s")));
    msd.setEpoch (mEpoch);

    // Calculate pa for all antennas.
    msd.setAntenna (-1);
    Float parang0 = msd.parAngle ();

    if (aips_debug)
        cout << "At time: " << MVTime (mEpoch.getValue ()) <<
             " PA = " << parang0 * 57.28 << " deg" << endl;

    return parang0;
}


// Fill in parallactic angle (NO FEED PA offset!).
Vector<Float>
VisibilityIteratorReadImpl::parang (Double time) const
{
    //  LogMessage message (LogOrigin ("VisibilityIteratorReadImpl","parang"));

    // Absolute UT
    Double ut = time;

    if (ut != cache_p.lastParangUT_p) {

        cache_p.lastParangUT_p = ut;

        // Set up the Epoch using the absolute MJD in seconds
        // get the Epoch reference from the column

        MEpoch mEpoch = msIter_p.msColumns ().timeMeas ()(0);
        Int nAnt = msIter_p.receptorAngle ().shape ()(1);

        cache_p.parang_p = parangCalculate (time, msd_p, nAnt, mEpoch);

    }
    return cache_p.parang_p;
}

Vector<Float>
VisibilityIteratorReadImpl::parangCalculate (Double time, MSDerivedValues & msd, int nAntennas, const MEpoch mEpoch0)
{
    MEpoch mEpoch = mEpoch0;
    mEpoch.set (MVEpoch (Quantity (time, "s")));

    msd.setEpoch (mEpoch);

    // Calculate pa for all antennas.

    Vector<Float> parang (nAntennas);

    for (Int iant = 0; iant < nAntennas; iant++) {

        msd.setAntenna (iant);
        parang (iant) = msd.parAngle ();

        if (aips_debug && iant == 0) {
            cout << "Antenna " << iant << " at time: " << MVTime (mEpoch.getValue ()) <<
                 " has PA = " << parang (iant) * 57.28 << endl;
        }
    }

    return parang;
}

// Fill in azimuth/elevation of the antennas.
// Cloned from feed_pa, we need to check that this is all correct!
Vector<MDirection>
VisibilityIteratorReadImpl::azel (Double time) const
{
    //  LogMessage message (LogOrigin ("VisibilityIteratorReadImpl","azel"));

    // Absolute UT
    Double ut = time;

    if (ut != cache_p.lastazelUT_p) {

        cache_p.lastazelUT_p = ut;
        Int nAnt = msIter_p.receptorAngle ().shape ()(1);
        MEpoch mEpoch = msIter_p.msColumns ().timeMeas ()(0);

        azelCalculate (ut, msd_p, cache_p.azel_p, nAnt, mEpoch);

    }
    return cache_p.azel_p;
}

void
VisibilityIteratorReadImpl::azelCalculate (Double time, MSDerivedValues & msd, Vector<MDirection> & azel,
        Int nAnt, const MEpoch & mEpoch0)
{
    // Refactored into a static method to allow VisBufferAsync to use

    MEpoch mEpoch = mEpoch0;

    mEpoch.set (MVEpoch (Quantity (time, "s")));

    msd.setEpoch (mEpoch);

    // Calculate az/el for all antennas.

    azel.resize (nAnt);

    for (Int iant = 0; iant < nAnt; iant++) {
        msd.setAntenna (iant);
        azel (iant) = msd.azel ();
        if (aips_debug) {
            if (iant == 0)
                cout << "Antenna " << iant << " at time: " << MVTime (mEpoch.getValue ()) <<
                     " has AzEl = " << azel (iant).getAngle ("deg") << endl;
        }
    }
}

// Fill in azimuth/elevation of the antennas.
// Cloned from feed_pa, we need to check that this is all correct!
MDirection
VisibilityIteratorReadImpl::azel0(Double time) const
{
    //  LogMessage message (LogOrigin ("VisibilityIteratorReadImpl","azel0"));

    // Absolute UT
    Double ut = time;

    if (ut != cache_p.lastazel0UT_p) {

        cache_p.lastazel0UT_p = ut;

        MEpoch mEpoch = msIter_p.msColumns ().timeMeas ()(0);

        azel0Calculate (time, msd_p, cache_p.azel0_p, mEpoch);

    }
    return cache_p.azel0_p;
}

void
VisibilityIteratorReadImpl::azel0Calculate (Double time, MSDerivedValues & msd,
        MDirection & azel0, const MEpoch & mEpoch0)
{
    // Refactored into a static method to allow VisBufferAsync to use

    MEpoch mEpoch = mEpoch0;

    mEpoch.set (MVEpoch (Quantity (time, "s")));

    msd.setEpoch (mEpoch);

    msd.setAntenna (-1);

    azel0 = msd.azel ();

    if (aips_debug) {
        cout << "At time: " << MVTime (mEpoch.getValue ()) <<
             " AzEl = " << azel0.getAngle ("deg") << endl;
    }

}

// Hour angle at specified time.
Double
VisibilityIteratorReadImpl::hourang (Double time) const
{
    //  LogMessage message (LogOrigin ("VisibilityIteratorReadImpl","azel"));

    // Absolute UT
    Double ut = time;

    if (ut != cache_p.lasthourangUT_p) {

        cache_p.lasthourangUT_p = ut;

        // Set up the Epoch using the absolute MJD in seconds
        // get the Epoch reference from the column keyword

        MEpoch mEpoch = msIter_p.msColumns ().timeMeas ()(0);

        cache_p.hourang_p = hourangCalculate (time, msd_p, mEpoch);

    }
    return cache_p.hourang_p;
}

Double
VisibilityIteratorReadImpl::hourangCalculate (Double time, MSDerivedValues & msd, const MEpoch & mEpoch0)
{
    MEpoch mEpoch = mEpoch0;

    mEpoch.set (MVEpoch (Quantity (time, "s")));

    msd.setEpoch (mEpoch);

    msd.setAntenna (-1);

    Double hourang = msd.hourAngle ();

    return hourang;
}

Vector<Float> &
VisibilityIteratorReadImpl::sigma (Vector<Float> & sig) const
{
    Matrix<Float> sigmat;
    getCol (columns_p.sigma_p, sigmat);
    // Do a rough average of the parallel hand polarizations to get a single
    // sigma. Should do this properly someday, or return all values
    sig.resize (sigmat.ncolumn ());
    sig = sigmat.row (0);
    sig += sigmat.row (nPol_p - 1);
    sig /= 2.0f;
    return sig;
}

Matrix<Float> &
VisibilityIteratorReadImpl::sigmaMat (Matrix<Float> & sigmat) const
{
    sigmat.resize (nPol_p, curNumRow_p);
    getCol (columns_p.sigma_p, sigmat);
    return sigmat;
}

Vector<Float> &
VisibilityIteratorReadImpl::weight (Vector<Float> & wt) const
{
    // Take average of parallel hand polarizations for now.
    // Later convert weight () to return full polarization dependence
    Matrix<Float> polWeight;
    getCol (columns_p.weight_p, polWeight);
    wt.resize (polWeight.ncolumn ());
    wt = polWeight.row (0);
    wt += polWeight.row (nPol_p - 1);
    wt /= 2.0f;
    return wt;
}

Matrix<Float> &
VisibilityIteratorReadImpl::weightMat (Matrix<Float> & wtmat) const
{
    wtmat.resize (nPol_p, curNumRow_p);
    getCol (columns_p.weight_p, wtmat);
    return wtmat;
}


Bool
VisibilityIteratorReadImpl::existsWeightSpectrum () const
{
    if (msIter_p.newMS ()) { // Cache to avoid testing unnecessarily.
        try {
            cache_p.msHasWtSp_p = columns_p.weightSpectrum_p.hasContent ();
            // Comparing columns_p.weightSpectrum_p.shape (0) to
            // IPosition (2, nPol_p, channelGroupSize ()) is too strict
            // when channel averaging might have changed
            // channelGroupSize () or weightSpectrum () out of sync.  Unfortunately the
            // right answer might not get cached soon enough.
            //
            //       columns_p.weightSpectrum_p.shape (0).isEqual (IPosition (2, nPol_p,
            //                                                    channelGroupSize ())));
            // if (!msHasWtSp_p){
            //   cerr << "columns_p.weightSpectrum_p.shape (0): " << columns_p.weightSpectrum_p.shape (0) << endl;
            //   cerr << "(nPol_p, channelGroupSize ()): " << nPol_p
            //        << ", " << channelGroupSize () << endl;
            // }
        } catch (AipsError x) {
            cache_p.msHasWtSp_p = false;
        }
    }
    return cache_p.msHasWtSp_p;
}

Cube<Float> &
VisibilityIteratorReadImpl::weightSpectrum (Cube<Float> & wtsp) const
{
    if (existsWeightSpectrum ()) {
        if (useSlicer_p) {
            getCol (columns_p.weightSpectrum_p, slicer_p, wtsp, true);
        } else {
            getCol (columns_p.weightSpectrum_p, wtsp, true);
        }
    } else {
        wtsp.resize (0, 0, 0);
    }
    return wtsp;
}

const VisImagingWeight &
VisibilityIteratorReadImpl::getImagingWeightGenerator () const
{
    return imwgt_p;
}


//Matrix<Float> &
//VisibilityIteratorReadImpl::imagingWeight (Matrix<Float> & wt) const
//{
//    if (imwgt_p.getType () == "none") {
//        throw (AipsError ("Programmer Error... imaging weights not set"));
//    }
//    Vector<Float> weightvec;
//    weight (weightvec);
//    Matrix<Bool> flagmat;
//    flag (flagmat);
//    wt.resize (flagmat.shape ());
//    if (imwgt_p.getType () == "uniform") {
//        Vector<Double> fvec;
//        frequency (fvec);
//        Matrix<Double> uvwmat;
//        uvwMat (uvwmat);
//        imwgt_p.weightUniform (wt, flagmat, uvwmat, fvec, weightvec, msId (), fieldId ());
//        if (imwgt_p.doFilter ()) {
//            imwgt_p.filter (wt, flagmat, uvwmat, fvec, weightvec);
//        }
//    } else if (imwgt_p.getType () == "radial") {
//        Vector<Double> fvec;
//        frequency (fvec);
//        Matrix<Double> uvwmat;
//        uvwMat (uvwmat);
//        imwgt_p.weightRadial (wt, flagmat, uvwmat, fvec, weightvec);
//        if (imwgt_p.doFilter ()) {
//            imwgt_p.filter (wt, flagmat, uvwmat, fvec, weightvec);
//        }
//    } else {
//        imwgt_p.weightNatural (wt, flagmat, weightvec);
//        if (imwgt_p.doFilter ()) {
//            Matrix<Double> uvwmat;
//            uvwMat (uvwmat);
//            Vector<Double> fvec;
//            frequency (fvec);
//            imwgt_p.filter (wt, flagmat, uvwmat, fvec, weightvec);
//
//        }
//    }
//
//    return wt;
//}

Int
VisibilityIteratorReadImpl::nSubInterval () const
{
    // Return the number of sub-intervals in the current chunk,
    // i.e. the number of unique time stamps
    //
    // Find all unique times in time_p
    Int retval = 0;
    uInt nTimes = time_p.nelements ();
    if (nTimes > 0) {

        Vector<Double> times (time_p); /* Do not change time_p, make a copy */
        Bool deleteIt;
        Double * tp = times.getStorage (deleteIt);

        std::sort (tp, tp + nTimes);

        /* Count unique times */
        retval = 1;
        for (unsigned i = 0; i < nTimes - 1; i++) {
            if (tp[i] < tp[i + 1]) {
                retval += 1;
            }
        }
    }
    return retval;
}

VisibilityIteratorReadImpl &
VisibilityIteratorReadImpl::selectVelocity (Int /*nChan*/,
        const MVRadialVelocity & /*vStart*/,
        const MVRadialVelocity & /*vInc*/,
        MRadialVelocity::Types /*rvType*/,
        MDoppler::Types /*dType*/,
        Bool /*precise*/)
{
    ThrowIf (true, "Method not implemented");

//    if (!initialized_p) {
//        // initialize the base iterator only (avoid recursive call to originChunks)
//        if (!msIterAtOrigin_p) {
//            msIter_p.origin ();
//            msIterAtOrigin_p = true;
//            stateOk_p = false;
//        }
//    }
//    velSelection_p = true;
//    nVelChan_p = nChan;
//    vstart_p = vStart;
//    vinc_p = vInc;
//    msd_p.setVelocityFrame (rvType);
//    vDef_p = dType;
//    cFromBETA_p.set (MDoppler (MVDoppler (Quantity (0., "m/s")),
//                             MDoppler::BETA), vDef_p);
//    vPrecise_p = precise;
//    if (precise) {
//        // set up conversion engine for full conversion
//    }
//    // have to reset the iterator so all caches get filled
//    originChunks ();
    return *this;
}


VisibilityIteratorReadImpl &
VisibilityIteratorReadImpl::selectChannel (Int nGroup, Int start, Int width,
        Int increment, Int spectralWindow)
{

    if (!initialized_p) {
        // initialize the base iterator only (avoid recursive call to originChunks)
        if (!msIterAtOrigin_p) {
            msIter_p.origin ();
            msIterAtOrigin_p = true;
            stateOk_p = false;
        }
    }
    Int spw = spectralWindow;
    if (spw < 0) {
        spw = msIter_p.spectralWindowId ();
    }
    Int n = channels_p.nGroups_p.nelements ();
    if (n == 0) {
        msChannels_p.spw_p.resize (1, true, false);
        msChannels_p.spw_p[0].resize (1);
        msChannels_p.spw_p[0][0] = spw;
        msChannels_p.nGroups_p.resize (1, true, false);
        msChannels_p.nGroups_p[0].resize (1);
        msChannels_p.nGroups_p[0][0] = nGroup;
        msChannels_p.start_p.resize (1, true, false);
        msChannels_p.start_p[0].resize (1);
        msChannels_p.start_p[0][0] = start;
        msChannels_p.width_p.resize (1, true, false);
        msChannels_p.width_p[0].resize (1);
        msChannels_p.width_p[0][0] = width;
        msChannels_p.inc_p.resize (1, true, false);
        msChannels_p.inc_p[0].resize (1);
        msChannels_p.inc_p[0][0] = increment;
        msCounter_p = 0;

    } else {
        Bool hasSpw = false;
        Int spwIndex = -1;
        for (uInt k = 0; k < msChannels_p.spw_p[0].nelements (); ++k) {
            if (spw == msChannels_p.spw_p[0][k]) {
                hasSpw = true;
                spwIndex = k;
                break;
            }
        }
        if (!hasSpw) {
            Int nspw = msChannels_p.spw_p[0].nelements () + 1;
            msChannels_p.spw_p[0].resize (nspw, true);
            msChannels_p.spw_p[0][nspw - 1] = spw;
            msChannels_p.nGroups_p[0].resize (nspw, true);
            msChannels_p.nGroups_p[0][nspw - 1] = nGroup;
            msChannels_p.start_p[0].resize (nspw, true);
            msChannels_p.start_p[0][nspw - 1] = start;
            msChannels_p.width_p[0].resize (nspw, true);
            msChannels_p.width_p[0][nspw - 1] = width;
            msChannels_p.inc_p[0].resize (nspw, true);
            msChannels_p.inc_p[0][nspw - 1] = increment;
        } else {
            msChannels_p.spw_p[0][spwIndex] = spw;
            msChannels_p.nGroups_p[0][spwIndex] = nGroup;
            msChannels_p.start_p[0][spwIndex] = start;
            msChannels_p.width_p[0][spwIndex] = width;
            msChannels_p.inc_p[0][spwIndex] = increment;
        }


    }
    if (spw >= n) {
        // we need to resize the blocks
        Int newn = max (2, max (2 * n, spw + 1));
        channels_p.nGroups_p.resize (newn);
        channels_p.start_p.resize (newn);
        channels_p.width_p.resize (newn);
        channels_p.inc_p.resize (newn);
        for (Int i = n; i < newn; i++) {
            channels_p.nGroups_p[i] = 0;
        }
    }
    channels_p.start_p[spw] = start;
    channels_p.width_p[spw] = width;

    channels_p.inc_p[spw] = increment;
    channels_p.nGroups_p[spw] = nGroup;
    // have to reset the iterator so all caches get filled & slicer sizes
    // get updated
    //  originChunks ();
    //
    //    if (msIterAtOrigin_p){
    //        if (!isInSelectedSPW (msIter_p.spectralWindowId ())){
    //            while ((!isInSelectedSPW (msIter_p.spectralWindowId ()))
    //                    && (msIter_p.more ()))
    //                msIter_p++;
    //            stateOk_p=false;
    //            setState ();
    //        }
    //    }

    //leave the state where msiter is pointing
    channelGroupSize_p = channels_p.width_p[msIter_p.spectralWindowId ()];
    curNGroups_p = channels_p.nGroups_p[msIter_p.spectralWindowId ()];

    return *this;
}

VisibilityIteratorReadImpl &
VisibilityIteratorReadImpl::selectChannel (const Block<Vector<Int> > & blockNGroup,
                                           const Block<Vector<Int> > & blockStart,
                                           const Block<Vector<Int> > & blockWidth,
                                           const Block<Vector<Int> > & blockIncr,
                                           const Block<Vector<Int> > & blockSpw)
{
    /*
    No longer needed
    if (!isMultiMS_p){
    //Programmer error ...so should not reach here
    cout << "Cannot use this function if Visiter was not constructed with multi-ms"
     << endl;
    }
     */

    msChannels_p.nGroups_p.resize (0, true, false);
    msChannels_p.nGroups_p = blockNGroup;
    msChannels_p.start_p.resize (0, true, false);
    msChannels_p.start_p = blockStart;
    msChannels_p.width_p.resize (0, true, false);
    msChannels_p.width_p = blockWidth;
    msChannels_p.inc_p.resize (0, true, false);
    msChannels_p.inc_p = blockIncr;
    msChannels_p.spw_p.resize (0, true, false);
    msChannels_p.spw_p = blockSpw;

    if (!initialized_p) {
        // initialize the base iterator only (avoid recursive call to originChunks)
        if (!msIterAtOrigin_p) {
            msIter_p.origin ();
            msIterAtOrigin_p = true;
            stateOk_p = false;
        }
    }

    channels_p.nGroups_p.resize (0);
    msCounter_p = 0;

    doChannelSelection ();
    // have to reset the iterator so all caches get filled & slicer sizes
    // get updated

    if (msIterAtOrigin_p) {
        if (!isInSelectedSPW (msIter_p.spectralWindowId ())) {
            while ((!isInSelectedSPW (msIter_p.spectralWindowId ()))
                    && (msIter_p.more ())) {
                msIter_p++;
            }
            stateOk_p = false;
        }

    }

    originChunks ();
    return *this;
}


void
VisibilityIteratorReadImpl::getChannelSelection (Block< Vector<Int> > & blockNGroup,
        Block< Vector<Int> > & blockStart,
        Block< Vector<Int> > & blockWidth,
        Block< Vector<Int> > & blockIncr,
        Block< Vector<Int> > & blockSpw)
{

    blockNGroup.resize (0, true, false);
    blockNGroup = msChannels_p.nGroups_p;
    blockStart.resize (0, true, false);
    blockStart = msChannels_p.start_p;
    blockWidth.resize (0, true, false);
    blockWidth = msChannels_p.width_p;
    blockIncr.resize (0, true, false);
    blockIncr = msChannels_p.inc_p;
    blockSpw.resize (0, true, false);
    blockSpw = msChannels_p.spw_p;
}
void
VisibilityIteratorReadImpl::doChannelSelection ()
{
    for (uInt k = 0; k < msChannels_p.spw_p[msCounter_p].nelements (); ++k) {
        Int spw = msChannels_p.spw_p[msCounter_p][k];
        if (spw < 0) {
            spw = msIter_p.spectralWindowId ();
        }
        Int n = channels_p.nGroups_p.nelements ();
        if (spw >= n) {
            // we need to resize the blocks
            Int newn = max (2, max (2 * n, spw + 1));
            channels_p.nGroups_p.resize (newn, true, true);
            channels_p.start_p.resize (newn, true, true);
            channels_p.width_p.resize (newn, true, true);
            channels_p.inc_p.resize (newn, true, true);
            for (Int i = n; i < newn; i++) {
                channels_p.nGroups_p[i] = 0;
            }
        }

        channels_p.start_p[spw] = msChannels_p.start_p[msCounter_p][k];
        channels_p.width_p[spw] = msChannels_p.width_p[msCounter_p][k];
        channelGroupSize_p = msChannels_p.width_p[msCounter_p][k];
        channels_p.inc_p[spw] = msChannels_p.inc_p[msCounter_p][k];
        channels_p.nGroups_p[spw] = msChannels_p.nGroups_p[msCounter_p][k];
        curNGroups_p = msChannels_p.nGroups_p[msCounter_p][k];

    }
    Int spw = msIter_p.spectralWindowId ();
    Int spIndex = -1;
    for (uInt k = 0; k < msChannels_p.spw_p[msCounter_p].nelements (); ++k) {
        if (spw == msChannels_p.spw_p[msCounter_p][k]) {
            spIndex = k;
            break;
        }
    }


    if (spIndex < 0) {
        spIndex = 0;
    }
    //leave this at the stage where msiter is pointing
    channelGroupSize_p = msChannels_p.width_p[msCounter_p][spIndex];
    curNGroups_p = msChannels_p.nGroups_p[msCounter_p][spIndex];



}

void
VisibilityIteratorReadImpl::slicesToMatrices (Vector<Matrix<Int> > & matv,
        const Vector<Vector<Slice> > & slicesv,
        const Vector<Int> & widthsv) const
{
    uInt nspw = slicesv.nelements ();

    matv.resize (nspw);
    uInt selspw = 0;
    for (uInt spw = 0; spw < nspw; ++spw) {
        uInt nSlices = slicesv[spw].nelements ();

        // Figure out how big to make matv[spw].
        uInt totOutChan = 0;

        Int width = (nSlices > 0) ? widthsv[selspw] : 1;
        if (width < 1) {
            throw (AipsError ("Cannot channel average with width < 1"));
        }

        for (uInt slicenum = 0; slicenum < nSlices; ++slicenum) {
            const Slice & sl = slicesv[spw][slicenum];
            Int firstchan = sl.start ();
            Int lastchan = sl.all () ? firstchan + channels_p.width_p[spw] - 1 : sl.end ();
            Int inc = sl.all () ? 1 : sl.inc ();

            // Even if negative increments are desirable, the for loop below has a <.
            if (inc < 1) {
                throw (AipsError ("The channel increment must be >= 1"));
            }

            // This formula is very dependent on integer division.  Don't rearrange it.
            totOutChan += 1 + ((lastchan - firstchan) / inc) / (1 + (width - 1) / inc);
        }
        matv[spw].resize (totOutChan, 4);

        // Index of input channel in SELECTED list, i.e.
        // mschan = vi.chanIds (chanids, spw)[selchanind].
        uInt selchanind = 0;

        // Fill matv with channel boundaries.
        uInt outChan = 0;
        for (uInt slicenum = 0; slicenum < nSlices; ++slicenum) {
            const Slice & sl = slicesv[spw][slicenum];
            Int firstchan = sl.start ();
            Int lastchan = sl.all () ? firstchan + channels_p.width_p[spw] - 1 : sl.end ();
            Int inc = sl.all () ? 1 : sl.inc (); // Default to no skipping

            // Again, these depend on integer division.  Don't rearrange them.
            Int selspan = 1 + (width - 1) / inc;
            Int span = inc * selspan;

            for (Int mschan = firstchan; mschan <= lastchan; mschan += span) {
                // The start and end in MS channel #s.
                matv[spw](outChan, 0) = mschan;
                matv[spw](outChan, 1) = mschan + width - 1;

                // The start and end in selected reckoning.
                matv[spw](outChan, 2) = selchanind;
                selchanind += selspan;
                matv[spw](outChan, 3) = selchanind - 1;
                ++outChan;
            }
        }
        if (nSlices > 0) {  // spw was selected
            ++selspw;
        }
    }
}


void VisibilityIteratorReadImpl::getFreqInSpwRange(Double& freqStart, Double& freqEnd, MFrequency::Types freqframe) const {
  Int nMS = msIter_p.numMS ();
  freqStart=C::dbl_max;
  freqEnd = 0.0;
            
  for (Int msId=0; msId < nMS; ++msId){

    Vector<Int> spws =  msChannels_p.spw_p[msId];
    Vector<Int> starts = msChannels_p.start_p[msId];
    Vector<Int> nchan;
    // careful here as we don't want to change width_p as it is multiplied by incr below
    nchan= msChannels_p.width_p[msId];
    Vector<Int> incr = msChannels_p.inc_p[msId];
    nchan=nchan*incr;
    Vector<uInt>  uniqIndx;
    Vector<Int> fldId;
    ScalarColumn<Int> (msIter_p.ms (msId), MS::columnName (MS::FIELD_ID)).getColumn (fldId);
    uInt nFields = GenSort<Int>::sort (fldId, Sort::Ascending, Sort::QuickSort | Sort::NoDuplicates);
    for (uInt indx=0; indx< nFields; ++indx){
      Int fieldid=fldId(indx);
      Double tmpFreqStart; 
      Double tmpFreqEnd;
      MSUtil::getFreqRangeInSpw(tmpFreqStart, tmpFreqEnd, spws, starts, nchan, msIter_p.ms(msId), freqframe, fieldid);
      if(freqStart > tmpFreqStart) freqStart=tmpFreqStart;
      if(freqEnd < tmpFreqEnd) freqEnd=tmpFreqEnd;
    }
  }

}



void
VisibilityIteratorReadImpl::getSpwInFreqRange (Block<Vector<Int> > & spw,
        Block<Vector<Int> > & start,
        Block<Vector<Int> > & nchan,
        Double freqStart,
        Double freqEnd,
        Double freqStep,
        MFrequency::Types freqframe) const
{
    // This functionality was relocated from MSIter in order to support this operation
    // within the VI to make the VisibilityIteratorReadImplAsync implementation feasible.

    Int nMS = msIter_p.numMS ();

    spw.resize (nMS, true, false);
    start.resize (nMS, true, false);
    nchan.resize (nMS, true, false);

    for (Int k = 0; k < nMS; ++k) {
        Vector<Double> t;
        ScalarColumn<Double> (msIter_p.ms (k), MS::columnName (MS::TIME)).getColumn (t);
        Vector<Int> ddId;
        Vector<Int> fldId;
        ScalarColumn<Int> (msIter_p.ms (k), MS::columnName (MS::DATA_DESC_ID)).getColumn (ddId);
        ScalarColumn<Int> (msIter_p.ms (k), MS::columnName (MS::FIELD_ID)).getColumn (fldId);
        MSFieldColumns fieldCol (msIter_p.ms (k).field ());
        MSDataDescColumns ddCol (msIter_p.ms (k).dataDescription ());
        MSSpWindowColumns spwCol (msIter_p.ms (k).spectralWindow ());
        ROScalarMeasColumn<MEpoch> timeCol (msIter_p.ms (k), MS::columnName (MS::TIME));
        Vector<uInt>  uniqIndx;
        uInt nTimes = GenSortIndirect<Double>::sort (uniqIndx, t, Sort::Ascending, Sort::QuickSort | Sort::NoDuplicates);
        //now need to do the conversion to data frame from requested frame
        //Get the epoch mesasures of the first row
        MEpoch ep;
        timeCol.get (0, ep);
        MPosition obsPos = msIter_p.telescopePosition ();
        Int oldDD = ddId[0];
        Int oldFLD = fldId[0];
        //For now we will assume that the field is not moving very far from polynome 0
        MDirection dir = fieldCol.phaseDirMeas (fldId[0]);
        MFrequency::Types obsMFreqType = (MFrequency::Types) (spwCol.measFreqRef ()(ddCol.spectralWindowId ()(ddId[0])));
        //cout << "nTimes " << nTimes << endl;
        //cout << " obsframe " << obsMFreqType << " reqFrame " << freqframe << endl;
        MeasFrame frame (ep, obsPos, dir);
        MFrequency::Convert toObs (freqframe,
                                  MFrequency::Ref (obsMFreqType, frame));

        Double freqEndMax = freqEnd;
        Double freqStartMin = freqStart;
        if (freqframe != obsMFreqType) {
            freqEndMax = 0.0;
            freqStartMin = C::dbl_max;
        }

        for (uInt j = 0; j < nTimes; ++j) {
            timeCol.get (uniqIndx[j], ep);
            if (oldDD != ddId[uniqIndx[j]]) {
                oldDD = ddId[uniqIndx[j]];
                if (spwCol.measFreqRef ()(ddCol.spectralWindowId ()(ddId[uniqIndx[j]])) != obsMFreqType) {
                    obsMFreqType = (MFrequency::Types) (spwCol.measFreqRef ()(ddCol.spectralWindowId ()(ddId[uniqIndx[j]])));
                    toObs.setOut (MFrequency::Ref (obsMFreqType, frame));
                }
            }
            if (obsMFreqType != freqframe) {
                frame.resetEpoch (ep);
                if (oldFLD != fldId[uniqIndx[j]]) {
                    oldFLD = fldId[uniqIndx[j]];
                    frame.resetDirection (fieldCol.phaseDirMeas (fldId[uniqIndx[j]]));
                }
                Double freqTmp = toObs (Quantity (freqStart, "Hz")).get ("Hz").getValue ();
                freqStartMin = (freqStartMin > freqTmp) ? freqTmp : freqStartMin;
                freqTmp = toObs (Quantity (freqEnd, "Hz")).get ("Hz").getValue ();
                freqEndMax = (freqEndMax < freqTmp) ? freqTmp : freqEndMax;
            }
        }

        //cout << "freqStartMin " << freqStartMin << " freqEndMax " << freqEndMax << endl;
        MSSpwIndex spwIn (msIter_p.ms (k).spectralWindow ());

        spwIn.matchFrequencyRange (freqStartMin - 0.5 * freqStep, freqEndMax + 0.5 * freqStep, spw[k], start[k], nchan[k]);
    }
}

vector<MeasurementSet>
VisibilityIteratorReadImpl::getMeasurementSets () const
{
    return measurementSets_p;
}


void
VisibilityIteratorReadImpl::allSelectedSpectralWindows (Vector<Int> & spws, Vector<Int> & nvischan)
{

    spws.resize ();
    spws = msChannels_p.spw_p[msId ()];
    nvischan.resize ();
    nvischan.resize (max (spws) + 1);
    nvischan.set (-1);
    for (uInt k = 0; k < spws.nelements (); ++k) {
        nvischan[spws[k]] = channels_p.width_p[spws[k]];
    }
}

Vector<Double> &
VisibilityIteratorReadImpl::lsrFrequency (Vector<Double> & freq) const
{
    if (velocity_p.velSelection_p) {
        freq.resize (velocity_p.nVelChan_p);
        freq = velocity_p.lsrFreq_p;
    } else {
        // if there is no vel selection, we just return the observing freqs
        frequency (freq);
    }
    return freq;
}


void
VisibilityIteratorReadImpl::lsrFrequency (const Int & spw, Vector<Double> & freq,
					  Bool & convert, const Bool ignoreconv)
{
    // This method is not good for conversion between frames which are extremely
    // time dependent over the course of the observation e.g topo to lsr unless
    // the epoch is in the actual buffer

    if (velocity_p.velSelection_p) {
        getTopoFreqs ();
        lsrFrequency (freq);
        return;
    }

    if (! cache_p.freqCacheOK_p) {
        frequency (freq);
    }

    //MFrequency::Types obsMFreqType=(MFrequency::Types)(msIter_p.msColumns ().spectralWindow ().measFreqRef ()(spw));

    //chanFreq=msIter_p.msColumns ().spectralWindow ().chanFreq ()(spw);

    const ArrayColumn <Double> & chanFreqs = msIter_p.msColumns ().spectralWindow ().chanFreq ();
    const ScalarColumn<Int> & obsMFreqTypes = msIter_p.msColumns ().spectralWindow ().measFreqRef ();
    MEpoch ep;
    ROScalarMeasColumn<MEpoch>(msIter_p.table (), MS::columnName (MS::TIME)).get (curStartRow_p, ep); // Setting epoch to iteration's first one
    MPosition obsPos = msIter_p.telescopePosition ();
    MDirection dir = msIter_p.phaseCenter ();

    lsrFrequency (spw, freq, convert, channels_p.start_p, channels_p.width_p, channels_p.inc_p,
                  channels_p.nGroups_p, chanFreqs, obsMFreqTypes, ep, obsPos, dir, ignoreconv);

}

void
VisibilityIteratorReadImpl::lsrFrequency (const Int & spw,
                                          Vector<Double> & freq,
                                          Bool & convert,
                                          const Block<Int> & chanStart,
                                          const Block<Int> & chanWidth,
                                          const Block<Int> & chanInc,
                                          const Block<Int> & numChanGroup,
                                          const ArrayColumn <Double> & chanFreqs,
                                          const ScalarColumn<Int> & obsMFreqTypes,
                                          const MEpoch & ep,
                                          const MPosition & obsPos,
                                          const MDirection & dir, const Bool ignoreconv)
{

    Vector<Double> chanFreq (0);
    chanFreq = chanFreqs (spw);

    //chanFreq=msIter_p.msColumns ().spectralWindow ().chanFreq ()(spw);
    //      Int start=channels_p.start_p[spw]-msIter_p.startChan ();
    //Assuming that the spectral windows selected is not a reference ms from
    //visset ...as this will have a start chan offseted may be.

    Int start = chanStart[spw];
    freq.resize (chanWidth[spw]);
    MFrequency::Types obsMFreqType = (MFrequency::Types) (obsMFreqTypes (spw));
    MeasFrame frame (ep, obsPos, dir);
    MFrequency::Convert tolsr (obsMFreqType,
                               MFrequency::Ref (MFrequency::LSRK, frame));

    //    if (obsMFreqType != MFrequency::LSRK){
    //        convert=true;
    //    }

    convert = obsMFreqType != MFrequency::LSRK; // make this parameter write-only
    // user requested no conversion
    if(ignoreconv) convert=false;

    for (Int i = 0; i < chanWidth[spw]; i++) {
        Int inc = chanInc[spw] <= 0 ? 1 : chanInc[spw] ;
        if (convert) {
            freq[i] = tolsr (chanFreq (start +
                                       (numChanGroup[spw] - 1) * chanWidth[spw] + i * inc)).
                                       getValue ().getValue ();
        } else {
            freq[i] = chanFreq (start +
                                (numChanGroup[spw] - 1) * chanWidth[spw] + i * inc);
        }
    }
}

void
VisibilityIteratorReadImpl::getLsrInfo (Block<Int> & channelGroupNumber,
                                        Block<Int> & channelIncrement,
                                        Block<Int> & channelStart,
                                        Block<Int> & channelWidth,
                                        MPosition & observatoryPositon,
                                        MDirection & phaseCenter,
                                        Bool & velocitySelection) const
{
    channelStart = channels_p.start_p;
    channelWidth = channels_p.width_p;
    channelIncrement = channels_p.inc_p;
    channelGroupNumber = channels_p.nGroups_p;
    observatoryPositon = msIter_p.telescopePosition ();
    phaseCenter = msIter_p.phaseCenter ();
    velocitySelection = velocity_p.velSelection_p;
}


void
VisibilityIteratorReadImpl::attachVisBuffer (VisBuffer & vb)
{
    vbStack_p.push (& vb);
    vb.invalidate ();
}

VisBuffer *
VisibilityIteratorReadImpl::getVisBuffer ()
{
    VisBuffer * result = NULL;

    if (! vbStack_p.empty ()) {
        result = vbStack_p.top ();
    }

    return result;
}

void
VisibilityIteratorReadImpl::detachVisBuffer (VisBuffer & vb)
{
    if (!vbStack_p.empty ()) {
        if (vbStack_p.top () == & vb) {
            vbStack_p.pop ();
            if (!vbStack_p.empty ()) {
                vbStack_p.top ()->invalidate ();
            }
        } else {
            throw (AipsError ("ROVisIter::detachVisBuffer - attempt to detach "
                            "buffer that is not the last one attached"));
        }
    }
}

Int
VisibilityIteratorReadImpl::numberAnt ()
{
    return msColumns ().antenna ().nrow (); // for single (sub)array only..
}

Int
VisibilityIteratorReadImpl::numberSpw ()
{
    return msColumns ().spectralWindow ().nrow ();
}

Int
VisibilityIteratorReadImpl::numberDDId ()
{
    return msColumns ().dataDescription ().nrow ();
}

Int
VisibilityIteratorReadImpl::numberPol ()
{
    return msColumns ().polarization ().nrow ();
}

Int
VisibilityIteratorReadImpl::numberCoh ()
{
    Int numcoh = 0;
    for (uInt k = 0; k < uInt (msIter_p.numMS ()) ; ++k) {
        numcoh += msIter_p.ms (k).nrow ();
    }
    return numcoh;

}

template<class T>
void
VisibilityIteratorReadImpl::getColScalar (const ScalarColumn<T> &column, Vector<T> &array, Bool resize) const
{
    column.getColumnCells (selRows_p, array, resize);
    return;
}

void
VisibilityIteratorReadImpl::getCol (const ScalarColumn<Bool> &column, Vector<Bool> &array, Bool resize) const
{
    getColScalar<Bool>(column, array, resize);
}

void
VisibilityIteratorReadImpl::getCol (const ScalarColumn<Int> &column, Vector<Int> &array, Bool resize) const
{
    getColScalar<Int>(column, array, resize);
}

void
VisibilityIteratorReadImpl::getCol (const ScalarColumn<Double> &column, Vector<Double> &array, Bool resize) const
{
    getColScalar<Double>(column, array, resize);
}

void
VisibilityIteratorReadImpl::getCol (const ArrayColumn<Bool> &column, Array<Bool> &array, Bool resize) const
{
    column.getColumnCells (selRows_p, array, resize);
}

void VisibilityIteratorReadImpl::getCol (const ArrayColumn<Float> &column, Array<Float> &array, Bool resize) const
{
    column.getColumnCells (selRows_p, array, resize);
}

template<class T>
void
VisibilityIteratorReadImpl::getColArray (const ArrayColumn<T> &column, Array<T> &array, Bool resize) const
{
    column.getColumnCells (selRows_p, array, resize);
    return;
}

void
VisibilityIteratorReadImpl::getCol (const ArrayColumn<Double> &column, Array<Double> &array, Bool resize) const
{
    column.getColumnCells (selRows_p, array, resize);
}

void
VisibilityIteratorReadImpl::getCol (const ArrayColumn<Complex> &column, Array<Complex> &array, Bool resize) const
{
    column.getColumnCells (selRows_p, array, resize);
}

void
VisibilityIteratorReadImpl::getCol (const ArrayColumn<Bool> &column, const Slicer & slicer, Array<Bool> &array, Bool resize) const
{
    column.getColumnCells (selRows_p, slicer, array, resize);
}

void
VisibilityIteratorReadImpl::getCol (const ArrayColumn<Float> &column, const Slicer & slicer, Array<Float> &array, Bool resize) const
{
    column.getColumnCells (selRows_p, slicer, array, resize);
}

void
VisibilityIteratorReadImpl::getCol (const ArrayColumn<Complex> &column, const Slicer & slicer, Array<Complex> &array, Bool resize) const
{
    column.getColumnCells (selRows_p, slicer, array, resize);
}

const Table
VisibilityIteratorReadImpl::attachTable () const
{
    return msIter_p.table ();
}

void
VisibilityIteratorReadImpl::slurp () const
{
    /* Set the table data manager (ISM and SSM) cache size to the full column size, for
       the columns ANTENNA1, ANTENNA2, FEED1, FEED2, TIME, INTERVAL, FLAG_ROW, SCAN_NUMBER and UVW
     */
    Record dmInfo (msIter_p.ms ().dataManagerInfo ());

    //  cout << "nfields = " << dmInfo.nfields () << endl;
    //  cout << "dminfo = " << dmInfo.description () << endl;
    RecordDesc desc = dmInfo.description ();
    for (unsigned i = 0; i < dmInfo.nfields (); i++) {
        //      cout << "field " << i << " isSubRecord = " << desc.isSubRecord (i) << endl;
        //      cout << "field " << i << " isArray = " << desc.isArray (i) << endl;
        if (desc.isSubRecord (i)) {

            Record sub = dmInfo.subRecord (i);

            //          cout << "sub = " << sub << endl;
            if (sub.fieldNumber ("NAME") >= 0 &&
                    sub.fieldNumber ("TYPE") >= 0 &&
                    sub.fieldNumber ("COLUMNS") >= 0 &&
                    sub.type (sub.fieldNumber ("NAME")) == TpString &&
                    sub.type (sub.fieldNumber ("TYPE")) == TpString &&
                    sub.type (sub.fieldNumber ("COLUMNS")) == TpArrayString) {

                Array<String> columns;
                dmInfo.subRecord (i).get ("COLUMNS", columns);

                bool match = false;
                for (unsigned j = 0; j < columns.nelements (); j++) {
                    String column = columns (IPosition (1, j));
                    match |= (column == MS::columnName (MS::ANTENNA1) ||
                              column == MS::columnName (MS::ANTENNA2) ||
                              column == MS::columnName (MS::FEED1) ||
                              column == MS::columnName (MS::FEED2) ||
                              column == MS::columnName (MS::TIME) ||
                              column == MS::columnName (MS::INTERVAL) ||
                              column == MS::columnName (MS::FLAG_ROW) ||
                              column == MS::columnName (MS::SCAN_NUMBER) ||
                              column == MS::columnName (MS::UVW));
                }
                //              cout << "columns = " << columns << endl;

                if (match) {

                    String dm_name;
                    dmInfo.subRecord (i).get ("NAME", dm_name);
                    // cout << "dm_name = " << dm_name << endl;

                    String dm_type;
                    dmInfo.subRecord (i).get ("TYPE", dm_type);
                    // cout << "dm_type = " << dm_type << endl;

                    Bool can_exceed_nr_buckets = false;
                    uInt num_buckets = msIter_p.ms ().nrow ();
                    // One bucket is at least one row, so this is enough

                    if (dm_type == "IncrementalStMan") {
                        ROIncrementalStManAccessor acc (msIter_p.ms (), dm_name);
                        acc.setCacheSize (num_buckets, can_exceed_nr_buckets);
                    } else if (dm_type == "StandardStMan") {
                        ROStandardStManAccessor acc (msIter_p.ms (), dm_name);
                        acc.setCacheSize (num_buckets, can_exceed_nr_buckets);
                    }
                    /* These are the only storage managers which use the BucketCache
                     (and therefore are slow for random access and small cache sizes)
                     */
                } else {
                    String dm_name;
                    dmInfo.subRecord (i).get ("NAME", dm_name);
                    //cout << "IGNORING...." << dm_name << endl;
                }
            } else {
                cerr << "Data manager info has unexpected shape! " << sub << endl;
            }
        }
    }
    return;
}

VisibilityIteratorWriteImpl::VisibilityIteratorWriteImpl (VisibilityIterator * vi)
: vi_p (vi)
{
	useCustomTileShape_p = useCustomTileShape();
}

VisibilityIteratorWriteImpl::VisibilityIteratorWriteImpl (const VisibilityIteratorWriteImpl & other)
{
    operator=(other);
    useCustomTileShape_p = useCustomTileShape();
}

VisibilityIteratorWriteImpl::~VisibilityIteratorWriteImpl ()
{
}

VisibilityIteratorWriteImpl *
VisibilityIteratorWriteImpl::clone (VisibilityIterator * vi) const
{
    // Return a clone of this object but ensure it's attached to the proper
    // VI container object.

    VisibilityIteratorWriteImpl * viwi = new VisibilityIteratorWriteImpl (* this);

    viwi->vi_p = vi;

    return viwi;
}

void
VisibilityIteratorWriteImpl::attachColumns (const Table & t)
{
    VisibilityIteratorReadImpl * readImpl = getReadImpl();

    readImpl->attachColumns (t); // Let the read impl handle it's tables

    const ColumnDescSet & cds = t.tableDesc ().columnDescSet ();

    if (cds.isDefined (MS::columnName (MS::DATA))) {
        columns_p.vis_p.attach (t, MS::columnName (MS::DATA));
    }

    if (cds.isDefined (MS::columnName (MS::FLOAT_DATA))) {
        readImpl->floatDataFound_p = true;
        columns_p.floatVis_p.attach (t, MS::columnName (MS::FLOAT_DATA));
    } else {
        readImpl->floatDataFound_p = false;
    }

    if (cds.isDefined ("MODEL_DATA")) {
        columns_p.modelVis_p.attach (t, "MODEL_DATA");
    }

    if (cds.isDefined ("CORRECTED_DATA")) {
        columns_p.corrVis_p.attach (t, "CORRECTED_DATA");
    }

    columns_p.weight_p.attach (t, MS::columnName (MS::WEIGHT));

    if (cds.isDefined ("WEIGHT_SPECTRUM")) {
        columns_p.weightSpectrum_p.attach (t, "WEIGHT_SPECTRUM");
    }

    columns_p.sigma_p.attach (t, MS::columnName (MS::SIGMA));

    columns_p.flag_p.attach (t, MS::columnName (MS::FLAG));

    columns_p.flagRow_p.attach (t, MS::columnName (MS::FLAG_ROW));

    if (cds.isDefined ("FLAG_CATEGORY")) {
        columns_p.flagCategory_p.attach (t, MS::columnName (MS::FLAG_CATEGORY));
    }
}

VisibilityIteratorReadImpl *
VisibilityIteratorWriteImpl::getReadImpl ()
{
    return vi_p->getReadImpl();
}

void
VisibilityIteratorWriteImpl::setFlag (const Matrix<Bool> & flag)
{
    // use same value for all polarizations

    VisibilityIteratorReadImpl * readImpl = getReadImpl();

    readImpl->cache_p.flagCube_p.resize (readImpl->nPol_p, readImpl->channelGroupSize_p,
                                         readImpl->curNumRow_p);

    Bool deleteIt;
    Bool * p = readImpl->cache_p.flagCube_p.getStorage (deleteIt);
    const Bool * pflag = flag.getStorage (deleteIt);
    if (Int (flag.nrow ()) != readImpl->channelGroupSize_p) {
        throw (AipsError ("VisIter::setFlag (flag) - inconsistent number of channels"));
    }

    for (uInt row = 0; row < readImpl->curNumRow_p; row++) {
        for (Int chn = 0; chn < readImpl->channelGroupSize_p; chn++) {
            for (Int pol = 0; pol < readImpl->nPol_p; pol++) {
                *p++ = *pflag;
            }
            pflag++;
        }
    }

    if (readImpl->useSlicer_p) {
        putCol (columns_p.flag_p, readImpl->slicer_p, readImpl->cache_p.flagCube_p);
    } else {
        putCol (columns_p.flag_p, readImpl->cache_p.flagCube_p);
    }
}

void
VisibilityIteratorWriteImpl::setFlag (const Cube<Bool> & flags)
{
    VisibilityIteratorReadImpl * readImpl = getReadImpl();

    if (readImpl->useSlicer_p) {
        putCol (columns_p.flag_p, readImpl->slicer_p, flags);
    } else {
        putCol (columns_p.flag_p, flags);
    }
}

void
VisibilityIteratorWriteImpl::setFlagCategory(const Array<Bool>& flagCategory)
{
    VisibilityIteratorReadImpl * readImpl = getReadImpl();

    if (readImpl->useSlicer_p){
        putCol(columns_p.flagCategory_p, readImpl->slicer_p, flagCategory);
    }
    else{
        putCol(columns_p.flagCategory_p, flagCategory);
    }
}


void
VisibilityIteratorWriteImpl::setFlagRow (const Vector<Bool> & rowflags)
{
    putCol (columns_p.flagRow_p, rowflags);
}

void
VisibilityIteratorWriteImpl::setVis (const Matrix<CStokesVector> & vis,
        DataColumn whichOne)
{
    // two problems: 1. channel selection -> we can only write to reference
    // MS with 'processed' channels
    //               2. polarization: there could be 1, 2 or 4 in the
    // original data, predict () always gives us 4. We save what was there
    // originally.

    //  if (!preselected_p) {
    //    throw (AipsError ("VisIter::setVis (vis) - cannot change original data"));
    //  }

    VisibilityIteratorReadImpl * readImpl = getReadImpl();

    if (Int (vis.nrow ()) != readImpl->channelGroupSize_p) {
        throw (AipsError ("VisIter::setVis (vis) - inconsistent number of channels"));
    }
    // we need to reform the vis matrix to a cube before we can use
    // putColumn to a Matrix column
    readImpl->cache_p.visCube_p.resize (readImpl->nPol_p, readImpl->channelGroupSize_p, readImpl->curNumRow_p);
    Bool deleteIt;
    Complex * p = readImpl->cache_p.visCube_p.getStorage (deleteIt);
    for (uInt row = 0; row < readImpl->curNumRow_p; row++) {
        for (Int chn = 0; chn < readImpl->channelGroupSize_p; chn++) {
            const CStokesVector & v = vis (chn, row);
            switch (readImpl->nPol_p) {
            case 4:
                *p++ = v (0);
                *p++ = v (1);
                *p++ = v (2);
                *p++ = v (3);
                break;
            case 2:
                *p++ = v (0);
                *p++ = v (3);
                break;
            case 1:
                *p++ = (v (0) + v (3)) / 2;
                break;
            }
        }
    }
    if (readImpl->useSlicer_p) {
        putDataColumn (whichOne, readImpl->slicer_p, readImpl->cache_p.visCube_p);
    } else {
        putDataColumn (whichOne, readImpl->cache_p.visCube_p);
    }
}

void
VisibilityIteratorWriteImpl::setVisAndFlag (const Cube<Complex> & vis,
                                            const Cube<Bool> & flag,
                                            DataColumn whichOne)
{
    VisibilityIteratorReadImpl * readImpl = getReadImpl();

    if (readImpl->useSlicer_p) {
        putDataColumn (whichOne, readImpl->slicer_p, vis);
    } else {
        putDataColumn (whichOne, vis);
    }
    if (readImpl->useSlicer_p) {
        putCol (columns_p.flag_p, readImpl->slicer_p, flag);
    } else {
        putCol (columns_p.flag_p, flag);
    }

}

void
VisibilityIteratorWriteImpl::setVis (const Cube<Complex> & vis, DataColumn whichOne)
{
    VisibilityIteratorReadImpl * readImpl = getReadImpl();

    if (readImpl->useSlicer_p) {
        putDataColumn (whichOne, readImpl->slicer_p, vis);
    } else {
        putDataColumn (whichOne, vis);
    }
}

void
VisibilityIteratorWriteImpl::setWeight (const Vector<Float> & weight)
{
    VisibilityIteratorReadImpl * readImpl = getReadImpl();

    // No polarization dependence for now
    Matrix<Float> polWeight;
    readImpl->getCol (readImpl->columns_p.weight_p, polWeight);
    for (Int i = 0; i < readImpl->nPol_p; i++) {
        Vector<Float> r = polWeight.row (i);
        r = weight;
    }
    putCol (columns_p.weight_p, polWeight);
}

void
VisibilityIteratorWriteImpl::setWeightMat (const Matrix<Float> & weightMat)
{
    putCol (columns_p.weight_p, weightMat);
}

void
VisibilityIteratorWriteImpl::setWeightSpectrum (const Cube<Float> & weightSpectrum)
{
    VisibilityIteratorReadImpl * readImpl = getReadImpl();

    if (! readImpl->columns_p.weightSpectrum_p.isNull ()) {
        putCol (columns_p.weightSpectrum_p, weightSpectrum);
    }
}

void
VisibilityIteratorWriteImpl::setSigma (const Vector<Float> & sigma)
{
    VisibilityIteratorReadImpl * readImpl = getReadImpl();

    Matrix<Float> sigmat;
    readImpl->getCol (readImpl->columns_p.sigma_p, sigmat);
    for (Int i = 0; i < readImpl->nPol_p; i++) {
        Vector<Float> r = sigmat.row (i);
        r = sigma;
    }
    putCol (columns_p.sigma_p, sigmat);
}

void
VisibilityIteratorWriteImpl::setSigmaMat (const Matrix<Float> & sigMat)
{
    putCol (columns_p.sigma_p, sigMat);
}


void
VisibilityIteratorWriteImpl::putDataColumn (DataColumn whichOne,
                                           const Slicer & slicer,
                                           const Cube<Complex> & data)
{
    // Set the visibility (observed, model or corrected);
    // deal with DATA and FLOAT_DATA seamlessly for observed data.

    VisibilityIteratorReadImpl * readImpl = getReadImpl();

    switch (whichOne) {

    case ROVisibilityIterator::Observed:
        if (readImpl->floatDataFound_p) {
            Cube<Float> dataFloat = real (data);
            putCol (columns_p.floatVis_p, slicer, dataFloat);
        } else {
            putCol (columns_p.vis_p, slicer, data);
        }
        break;

    case ROVisibilityIterator::Corrected:
        putCol (columns_p.corrVis_p, slicer, data);
        break;

    case ROVisibilityIterator::Model:
        putCol (columns_p.modelVis_p, slicer, data);
        break;

    default:
        Assert (false);
    }
}

void
VisibilityIteratorWriteImpl::putDataColumn (DataColumn whichOne,
        const Cube<Complex> & data)
{
    // Set the visibility (observed, model or corrected);
    // deal with DATA and FLOAT_DATA seamlessly for observed data.

    VisibilityIteratorReadImpl * readImpl = getReadImpl();

    switch (whichOne) {

    case ROVisibilityIterator::Observed:
        if (readImpl->floatDataFound_p) {
            Cube<Float> dataFloat = real (data);
            if (useCustomTileShape_p) setTileShape(readImpl->selRows_p,columns_p.floatVis_p,dataFloat.shape());
            putCol (columns_p.floatVis_p, dataFloat);
        } else {
        	if (useCustomTileShape_p) setTileShape(readImpl->selRows_p,columns_p.vis_p,data.shape());
            putCol (columns_p.vis_p, data);
        }
        break;

    case ROVisibilityIterator::Corrected:
    	if (useCustomTileShape_p) setTileShape(readImpl->selRows_p,columns_p.corrVis_p,data.shape());
        putCol (columns_p.corrVis_p, data);
        break;

    case ROVisibilityIterator::Model:
    	if (useCustomTileShape_p) setTileShape(readImpl->selRows_p,columns_p.modelVis_p,data.shape());
        putCol (columns_p.modelVis_p, data);
        break;

    default:
        Assert (false);
    }
}

void
VisibilityIteratorWriteImpl::putColScalar (ScalarColumn<Bool> &column, const Vector<Bool> &array)
{
    VisibilityIteratorReadImpl * readImpl = getReadImpl();

    column.putColumnCells (readImpl->selRows_p, array);
    return;
}

void
VisibilityIteratorWriteImpl::putCol (ScalarColumn<Bool> &column, const Vector<Bool> &array)
{
    putColScalar (column, array);
}

void
VisibilityIteratorWriteImpl::putCol (ArrayColumn<Bool> &column, const Array<Bool> &array)
{
    VisibilityIteratorReadImpl * readImpl = getReadImpl();

    column.putColumnCells (readImpl->selRows_p, array);
}

void
VisibilityIteratorWriteImpl::putCol (ArrayColumn<Float> &column, const Array<Float> &array)
{
    VisibilityIteratorReadImpl * readImpl = getReadImpl();

    column.putColumnCells (readImpl->selRows_p, array);
}

void
VisibilityIteratorWriteImpl::putCol (ArrayColumn<Complex> &column, const Array<Complex> &array)
{
    VisibilityIteratorReadImpl * readImpl = getReadImpl();

    column.putColumnCells (readImpl->selRows_p, array);
}

void
VisibilityIteratorWriteImpl::putCol (ArrayColumn<Bool> &column,
                                    const Slicer & slicer,
                                    const Array<Bool> &array)
{
    VisibilityIteratorReadImpl * readImpl = getReadImpl();

    column.putColumnCells (readImpl->selRows_p, slicer, array);
}

void
VisibilityIteratorWriteImpl::putCol (ArrayColumn<Float> &column,
                                    const Slicer & slicer,
                                    const Array<Float> &array)
{
    VisibilityIteratorReadImpl * readImpl = getReadImpl();

    column.putColumnCells (readImpl->selRows_p, slicer, array);
}

void
VisibilityIteratorWriteImpl::putCol (ArrayColumn<Complex> &column,
                                    const Slicer & slicer,
                                    const Array<Complex> &array)
{
     VisibilityIteratorReadImpl * readImpl = getReadImpl();

   column.putColumnCells (readImpl->selRows_p, slicer, array);
}

template <class T> void VisibilityIteratorWriteImpl::setTileShape(	RefRows &rowRef,
																	ArrayColumn<T> &outputDataCol,
																	const IPosition &arrayShape)
{
	size_t nCorr = arrayShape(0);
	size_t nChan = arrayShape(1);
	ssize_t nRows = 1048576 / (sizeof(T)*nCorr*nChan);
	IPosition outputPlaneShape(2,nCorr,nChan);
	IPosition tileShape(3,nCorr,nChan,nRows);
	outputDataCol.setShape(rowRef.firstRow(),outputPlaneShape,tileShape);

	return;
}

Bool VisibilityIteratorWriteImpl::useCustomTileShape()
{
	Bool ret = false;
	String rank = EnvironmentVariable::get("OMPI_COMM_WORLD_RANK");
	if (!rank.empty())
	{
		ret = true;
	}

	return ret;
}

void
VisibilityIteratorWriteImpl::putModel(const RecordInterface& rec, Bool iscomponentlist, Bool incremental)
{

  /*
  Vector<Int> fields = getReadImpl()->msColumns().fieldId().getColumn();
  const Int option = Sort::HeapSort | Sort::NoDuplicates;
  const Sort::Order order = Sort::Ascending;

  Int nfields = GenSort<Int>::sort (fields, order, option);

  // Make sure  we have the right size

  fields.resize(nfields, true);
  */
  Matrix<Int>  combiIndex;
  MSUtil::getIndexCombination(getReadImpl()->msColumns(), combiIndex);
  Int msid = getReadImpl()->msId();

  Vector<Int> spws =  getReadImpl()->msChannels_p.spw_p[msid];
  Vector<Int> starts = getReadImpl()->msChannels_p.start_p[msid];
  Vector<Int> nchan = getReadImpl()->msChannels_p.width_p[msid];
  Vector<Int> incr = getReadImpl()->msChannels_p.inc_p[msid];
  Matrix<Int> chansel(spws.nelements(),4);
  chansel.column(0)=spws;
  chansel.column(1)=starts;
  chansel.column(2)=nchan;
  chansel.column(3)=incr;

  CountedPtr<VisModelDataI> visModelData = VisModelDataI::create();

  //visModelData->putModelI (getReadImpl()->ms (), rec, fields, spws, starts, nchan, incr,
  //                           iscomponentlist, incremental);

  //version 2.0 to keep track of scan and state_id selection 
  visModelData->putModelI (getReadImpl()->ms (), rec, combiIndex, chansel, iscomponentlist, incremental);
  
}


void
VisibilityIteratorWriteImpl::writeBack (VisBuffer * vb)
{
    if (backWriters_p.empty ()) {
        initializeBackWriters ();
    }

    VbDirtyComponents dirtyComponents = vb->dirtyComponentsGet ();

    for (VbDirtyComponents::const_iterator dirtyComponent = dirtyComponents.begin ();
         dirtyComponent != dirtyComponents.end ();
         dirtyComponent ++) {

        ThrowIf (backWriters_p.find (* dirtyComponent) == backWriters_p.end (),
                 String::format ("No writer defined for VisBuffer component %d", * dirtyComponent));
        BackWriter * backWriter = backWriters_p [ * dirtyComponent];

        try {
            (* backWriter) (this, vb);
        } catch (AipsError & e) {
            Rethrow (e, String::format ("Error while writing back VisBuffer component %d", * dirtyComponent));
        }
    }
}

void
VisibilityIteratorWriteImpl::initializeBackWriters ()
{
    backWriters_p [VisBufferComponents::Flag] =
        makeBackWriter (& VisibilityIteratorWriteImpl::setFlag, & VisBuffer::flag);
    backWriters_p [VisBufferComponents::FlagCube] =
        makeBackWriter (& VisibilityIteratorWriteImpl::setFlag, & VisBuffer::flagCube);
    backWriters_p [VisBufferComponents::FlagRow] =
        makeBackWriter (& VisibilityIteratorWriteImpl::setFlagRow, & VisBuffer::flagRow);
    backWriters_p [VisBufferComponents::FlagCategory] =
        makeBackWriter (& VisibilityIteratorWriteImpl::setFlagCategory, & VisBuffer::flagCategory);
    backWriters_p [VisBufferComponents::Sigma] =
        makeBackWriter (& VisibilityIteratorWriteImpl::setSigma, & VisBuffer::sigma);
    backWriters_p [VisBufferComponents::SigmaMat] =
        makeBackWriter (& VisibilityIteratorWriteImpl::setSigmaMat, & VisBuffer::sigmaMat);
    backWriters_p [VisBufferComponents::Weight] =
        makeBackWriter (& VisibilityIteratorWriteImpl::setWeight, & VisBuffer::weight);
    backWriters_p [VisBufferComponents::WeightMat] =
        makeBackWriter (& VisibilityIteratorWriteImpl::setWeightMat, & VisBuffer::weightMat);

    // Now do the visibilities.  These are slightly different since the setter requires an
    // enum value.

    backWriters_p [VisBufferComponents::Observed] =
        makeBackWriter2 (& VisibilityIteratorWriteImpl::setVis, & VisBuffer::visibility,
                         ROVisibilityIterator::Observed);
    backWriters_p [VisBufferComponents::Corrected] =
        makeBackWriter2 (& VisibilityIteratorWriteImpl::setVis, & VisBuffer::correctedVisibility,
                         ROVisibilityIterator::Corrected);
    backWriters_p [VisBufferComponents::Model] =
        makeBackWriter2 (& VisibilityIteratorWriteImpl::setVis, & VisBuffer::modelVisibility,
                         ROVisibilityIterator::Model);

    backWriters_p [VisBufferComponents::ObservedCube] =
        makeBackWriter2 (& VisibilityIteratorWriteImpl::setVis, & VisBuffer::visCube,
                         ROVisibilityIterator::Observed);
    backWriters_p [VisBufferComponents::CorrectedCube] =
        makeBackWriter2 (& VisibilityIteratorWriteImpl::setVis, & VisBuffer::correctedVisCube,
                         ROVisibilityIterator::Corrected);
    backWriters_p [VisBufferComponents::ModelCube] =
        makeBackWriter2 (& VisibilityIteratorWriteImpl::setVis, & VisBuffer::modelVisCube,
                         ROVisibilityIterator::Model);

}

VisibilityIteratorWriteImpl::Columns &
VisibilityIteratorWriteImpl::Columns::operator= (const VisibilityIteratorWriteImpl::Columns & other)
{
    flag_p.reference (other.flag_p);
    flagCategory_p.reference (other.flagCategory_p);
    flagRow_p.reference (other.flagRow_p);
    vis_p.reference (other.vis_p);
    floatVis_p.reference (other.floatVis_p);
    modelVis_p.reference (other.modelVis_p);
    corrVis_p.reference (other.corrVis_p);
    weight_p.reference (other.weight_p);
    weightSpectrum_p.reference (other.weightSpectrum_p);
    sigma_p.reference (other.sigma_p);

    return * this;
}


} //# NAMESPACE CASA - END



