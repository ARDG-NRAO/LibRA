//# VLAT.cc: Implemenation of visibility lookahead thread related functionality.
//# Copyright (C) 2011
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
//# Correspondence concerning CASA should be addressed as follows:
//#        Internet email: CASA-request@nrao.edu.
//#        Postal address: CASA Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//#
//# $Id$

#include <assert.h>
#include <time.h>
#include <sys/time.h>

#include <casacore/casa/Logging/LogIO.h>
#include <casacore/casa/System/AipsrcValue.h>
#include <msvis/MSVis/VLAT.h>
#include <msvis/MSVis/VisBufferAsync.h>

#include <stdcasa/thread/AsynchronousTools.h>
using namespace casacore;
using namespace casa::async;

#include <algorithm>
#include <cstdarg>
#include <functional>

#include <stdcasa/UtilJ.h>

using namespace casacore;
using namespace casa::utilj;
using namespace std;
using namespace casacore;
using namespace casa::asyncio;

#define Log(level, ...) \
        {if (AsynchronousInterface::logThis (level)) \
    Logger::get()->log (__VA_ARGS__);};

using namespace casacore;
namespace casa {

namespace asyncio {

//  *****************************
//  *                           *
//  * VlatAndDataImplementation *
//  *                           *
//  *****************************

VlatAndData::VlatAndData ()
: vlaData_p (NULL),
  vlat_p (NULL)
{
}

//  ***********************
//  *                     *
//  * VLAT Implementation *
//  *                     *
//  ***********************

VLAT::VLAT (AsynchronousInterface * asynchronousInterface)
{
    interface_p = asynchronousInterface;
    vlaData_p = interface_p->getVlaData();
    visibilityIterator_p = NULL;
    writeIterator_p = NULL;
    threadTerminated_p = false;
}

VLAT::~VLAT ()
{

    // Free up storage

    for (FillerDictionary::iterator f = fillerDictionary_p.begin();
            f != fillerDictionary_p.end();
            f++){
        delete (f->second);
    }

    for (Fillers::iterator f = fillers_p.begin();
            f != fillers_p.end();
            f++){
        delete (* f);
    }

    delete visibilityIterator_p;
    delete writeIterator_p;
}

void
VLAT::alignWriteIterator (SubChunkPair subchunk)
{
    Assert (subchunk <= readSubchunk_p);

    Bool done = false;

    while (subchunk > writeSubchunk_p && ! done){

        ++ (* writeIterator_p); // advance to next subchunk in chunk

        if (writeIterator_p->more()){

            // Sucessfully moved on to next subchunk

            writeSubchunk_p.incrementSubChunk();
        }
        else{

            // End of subchunks to advance to start of next chunk

            writeIterator_p->nextChunk ();

            if (writeIterator_p->moreChunks ()){

                // Moved on to next chunk; position to first subchunk

                writeIterator_p->origin ();

                if (writeIterator_p->more()){
                    writeSubchunk_p.incrementChunk();
                }
                else{
                    done = true; // no more data
                }
            }
            else{
                done = true; // no more data
            }
        }
    }

    ThrowIf (subchunk != writeSubchunk_p,
             String::format ("Failed to advance write iterator to subchunk %s; last subchunk is %s",
                             subchunk.toString().c_str(), writeSubchunk_p.toString().c_str()));
}

void
VLAT::applyModifiers (ROVisibilityIterator * rovi, VisibilityIterator * vi)
{
    // Apply modifiers to read iterator and to write iterator (if it exists)

    roviaModifiers_p.apply (rovi);

    if (vi != NULL){
        roviaModifiers_p.apply (vi);
    }

    // Get the channel selection information from the modified read VI and provide it to the
    // data object

    roviaModifiers_p.clearAndFree ();

    Block< Vector<Int> > blockNGroup;
    Block< Vector<Int> > blockStart;
    Block< Vector<Int> > blockWidth;
    Block< Vector<Int> > blockIncr;
    Block< Vector<Int> > blockSpw;

    rovi->getChannelSelection (blockNGroup, blockStart, blockWidth, blockIncr, blockSpw);

    vlaData_p -> storeChannelSelection (asyncio::ChannelSelection (blockNGroup, blockStart,
                                                                   blockWidth, blockIncr, blockSpw));
}

void
VLAT::checkFiller (VisBufferComponents::EnumType fillerId)
{
    ThrowIf (! visibilityIterator_p -> existsColumn (fillerId),
             String::format ("VLAT: Column to be prefetched, %s, does not exist!",
                            PrefetchColumns::columnName (fillerId).c_str()));
}

void
VLAT::createFillerDictionary ()
{
    // Create a dictionary of all the possible fillers using the
    // ViReadImplAsync::PrefetchColumnIds as the keys

    fillerDictionary_p.clear();

    fillerDictionary_p.add (VisBufferComponents::AllBeamOffsetsZero,
                           vlatFunctor0 (& VisBufferAsync::fillAllBeamOffsetsZero));
    fillerDictionary_p.add (VisBufferComponents::AntennaMounts,
                           vlatFunctor0 (& VisBufferAsync::fillAntennaMounts));
    fillerDictionary_p.add (VisBufferComponents::Ant1,
                           vlatFunctor0 (& VisBuffer::fillAnt1));
    fillerDictionary_p.add (VisBufferComponents::Ant2,
                           vlatFunctor0 (& VisBuffer::fillAnt2));
    fillerDictionary_p.add (VisBufferComponents::ArrayId,
                           vlatFunctor0 (& VisBuffer::fillArrayId));
    fillerDictionary_p.add (VisBufferComponents::BeamOffsets,
                           vlatFunctor0 (& VisBufferAsync::fillBeamOffsets));
    fillerDictionary_p.add (VisBufferComponents::Channel,
                           vlatFunctor0 (& VisBuffer::fillChannel));
    fillerDictionary_p.add (VisBufferComponents::Cjones,
                           vlatFunctor0 (& VisBuffer::fillCjones));
    fillerDictionary_p.add (VisBufferComponents::CorrType,
                           vlatFunctor0 (& VisBuffer::fillCorrType));
    fillerDictionary_p.add (VisBufferComponents::Corrected,
                           vlatFunctor1(& VisBuffer::fillVis,
                                        VisibilityIterator::Corrected));
    fillerDictionary_p.add (VisBufferComponents::CorrectedCube,
                           vlatFunctor1(& VisBuffer::fillVisCube,
                                        VisibilityIterator::Corrected));
    fillerDictionary_p.add (VisBufferComponents::DataDescriptionId,
                           vlatFunctor0 (& VisBuffer::fillDataDescriptionId));
//    fillerDictionary_p.add (VisBufferComponents::Direction1,
//                           vlatFunctor0 (& VisBuffer::fillDirection1));
//    fillerDictionary_p.add (VisBufferComponents::Direction2,
//                           vlatFunctor0 (& VisBuffer::fillDirection2));
    fillerDictionary_p.add (VisBufferComponents::Exposure,
                           vlatFunctor0 (& VisBuffer::fillExposure));
    fillerDictionary_p.add (VisBufferComponents::Feed1,
                           vlatFunctor0 (& VisBuffer::fillFeed1));
//    fillerDictionary_p.add (VisBufferComponents::Feed1_pa,
//                           vlatFunctor0 (& VisBuffer::fillFeed1_pa));
    fillerDictionary_p.add (VisBufferComponents::Feed2,
                           vlatFunctor0 (& VisBuffer::fillFeed2));
//    fillerDictionary_p.add (VisBufferComponents::Feed2_pa,
//                           vlatFunctor0 (& VisBuffer::fillFeed2_pa));
    fillerDictionary_p.add (VisBufferComponents::FieldId,
                           vlatFunctor0 (& VisBuffer::fillFieldId));
    fillerDictionary_p.add (VisBufferComponents::Flag,
                           vlatFunctor0 (& VisBuffer::fillFlag));
    fillerDictionary_p.add (VisBufferComponents::FlagCategory,
                           vlatFunctor0 (& VisBuffer::fillFlagCategory));
    fillerDictionary_p.add (VisBufferComponents::FlagCube,
                           vlatFunctor0 (& VisBuffer::fillFlagCube));
    fillerDictionary_p.add (VisBufferComponents::FlagRow,
                           vlatFunctor0 (& VisBuffer::fillFlagRow));
    fillerDictionary_p.add (VisBufferComponents::Freq,
                           vlatFunctor0 (& VisBuffer::fillFreq));
    fillerDictionary_p.add (VisBufferComponents::ImagingWeight,
                           new VlatFunctor ("ImagingWeight")); // do not fill this one
    fillerDictionary_p.add (VisBufferComponents::Model,
                           vlatFunctor1(& VisBuffer::fillVis,
                                        VisibilityIterator::Model));
    fillerDictionary_p.add (VisBufferComponents::ModelCube,
                           vlatFunctor1(& VisBuffer::fillVisCube,
                                        VisibilityIterator::Model));
    fillerDictionary_p.add (VisBufferComponents::NChannel,
                           vlatFunctor0 (& VisBuffer::fillnChannel));
    fillerDictionary_p.add (VisBufferComponents::NCorr,
                           vlatFunctor0 (& VisBuffer::fillnCorr));
    fillerDictionary_p.add (VisBufferComponents::NRow,
                           vlatFunctor0 (& VisBuffer::fillnRow));
    fillerDictionary_p.add (VisBufferComponents::ObservationId,
                           vlatFunctor0 (& VisBuffer::fillObservationId));
    fillerDictionary_p.add (VisBufferComponents::Observed,
                           vlatFunctor1(& VisBuffer::fillVis,
                                        VisibilityIterator::Observed));
    fillerDictionary_p.add (VisBufferComponents::ObservedCube,
                           vlatFunctor1(& VisBuffer::fillVisCube,
                                        VisibilityIterator::Observed));
    fillerDictionary_p.add (VisBufferComponents::PhaseCenter,
                           vlatFunctor0 (& VisBuffer::fillPhaseCenter));
    fillerDictionary_p.add (VisBufferComponents::PolFrame,
                           vlatFunctor0 (& VisBuffer::fillPolFrame));
    fillerDictionary_p.add (VisBufferComponents::ProcessorId,
                           vlatFunctor0 (& VisBuffer::fillProcessorId));
    fillerDictionary_p.add (VisBufferComponents::ReceptorAngles,
                           vlatFunctor0 (& VisBufferAsync::fillReceptorAngles));
    fillerDictionary_p.add (VisBufferComponents::Scan,
                           vlatFunctor0 (& VisBuffer::fillScan));
    fillerDictionary_p.add (VisBufferComponents::Sigma,
                           vlatFunctor0 (& VisBuffer::fillSigma));
    fillerDictionary_p.add (VisBufferComponents::SigmaMat,
                           vlatFunctor0 (& VisBuffer::fillSigmaMat));
    fillerDictionary_p.add (VisBufferComponents::SpW,
                           vlatFunctor0 (& VisBuffer::fillSpW));
    fillerDictionary_p.add (VisBufferComponents::StateId,
                           vlatFunctor0 (& VisBuffer::fillStateId));
    fillerDictionary_p.add (VisBufferComponents::Time,
                           vlatFunctor0 (& VisBuffer::fillTime));
    fillerDictionary_p.add (VisBufferComponents::TimeCentroid,
                           vlatFunctor0 (& VisBuffer::fillTimeCentroid));
    fillerDictionary_p.add (VisBufferComponents::TimeInterval,
                           vlatFunctor0 (& VisBuffer::fillTimeInterval));
    fillerDictionary_p.add (VisBufferComponents::Uvw,
                           vlatFunctor0 (& VisBuffer::filluvw));
    fillerDictionary_p.add (VisBufferComponents::UvwMat,
                           vlatFunctor0 (& VisBuffer::filluvwMat));
    fillerDictionary_p.add (VisBufferComponents::Weight,
                           vlatFunctor0 (& VisBuffer::fillWeight));
    fillerDictionary_p.add (VisBufferComponents::WeightMat,
                           vlatFunctor0 (& VisBuffer::fillWeightMat));
    fillerDictionary_p.add (VisBufferComponents::WeightSpectrum,
                           vlatFunctor0 (& VisBuffer::fillWeightSpectrum));

    // assert (fillerDictionary_p.size() == VisBufferComponents::N_VisBufferComponents);
    // Every supported prefetch column needs a filler

    //fillerDependencies_p.add ();

    //fillerDependencies_p.performTransitiveClosure (fillerDictionary_p);
}

void
VLAT::fillDatum (VlaDatum * datum)
{

    VisBufferComponents::EnumType fillerId = VisBufferComponents::Unknown;
    try{
        VisBufferAsync * vb = datum->getVisBuffer();

        vb->clear();
        vb->setFilling (true);
        vb->attachToVisIter (* visibilityIterator_p); // invalidates vb's cache as well

        fillDatumMiscellanyBefore (datum);

        for (Fillers::iterator filler = fillers_p.begin(); filler != fillers_p.end(); filler ++){

            //Log (2, "Filler id=%d name=%s starting\n", (* filler)->getId(), ViReadImplAsync::prefetchColumnName((* filler)->getId()).c_str());

            fillerId = (* filler)->getId();
            checkFiller (fillerId);
            (** filler) (vb);
        }

        fillDatumMiscellanyAfter (datum);

        vb->detachFromVisIter ();
        vb->setFilling (false);
    }
    catch (...){

        if (fillerId == -1){
            Log (1, "VLAT: Error while filling datum at 0x%08x, vb at 0x%08x; rethrowing\n",
                 datum, datum->getVisBuffer());
        }
        else{
            Log (1, "VLAT: Error while filling datum at 0x%08x, vb at 0x%08x; "
                 "fillingColumn='%s'; rethrowing\n",
                 datum, datum->getVisBuffer(),
                 PrefetchColumns::columnName (fillerId).c_str());
        }

        throw;
    }
}

void
VLAT::fillDatumMiscellanyAfter (VlaDatum * datum)
{
    datum->getVisBuffer()->setVisibilityShape (visibilityIterator_p->visibilityShape ());

    //////datum->getVisBuffer()->setDataDescriptionId (visibilityIterator_p->getDataDescriptionId());

    datum->getVisBuffer()->setPolarizationId (visibilityIterator_p->polarizationId());

    datum->getVisBuffer()->setNCoh(visibilityIterator_p->numberCoh ());

    datum->getVisBuffer()->setNRowChunk (visibilityIterator_p->nRowChunk());

    Vector<Int> nvischan;
    Vector<Int> spw;

    visibilityIterator_p->allSelectedSpectralWindows(spw, nvischan);

    datum->getVisBuffer()->setSelectedNVisibilityChannels (nvischan);
    datum->getVisBuffer()->setSelectedSpectralWindows (spw);

    int nSpw = visibilityIterator_p->numberSpw();
    datum->getVisBuffer()->setNSpw (nSpw);

    int nRowChunk = visibilityIterator_p->nRowChunk();
    datum->getVisBuffer()->setNRowChunk (nRowChunk);

    datum->getVisBuffer()->setMSD (visibilityIterator_p->getMSD ()); // ought to be last
}

void
VLAT::fillDatumMiscellanyBefore (VlaDatum * datum)
{
    datum->getVisBuffer()->setMeasurementSet (visibilityIterator_p->getMeasurementSet());
    datum->getVisBuffer()->setMeasurementSetId (visibilityIterator_p->getMeasurementSetId(),
                                                datum->getSubChunkPair().second == 0);

    datum->getVisBuffer()->setNewEntityFlags (visibilityIterator_p->newArrayId(),
                                              visibilityIterator_p->newFieldId(),
                                              visibilityIterator_p->newSpectralWindow());
    datum->getVisBuffer()->setNAntennas (visibilityIterator_p->getNAntennas ());
    datum->getVisBuffer()->setMEpoch (visibilityIterator_p->getEpoch ());
    datum->getVisBuffer()->setReceptor0Angle (visibilityIterator_p->getReceptor0Angle());

    fillLsrInfo (datum);

    Vector<Double> lsrFreq, selFreq;
    visibilityIterator_p->getTopoFreqs (lsrFreq, selFreq);
    datum->getVisBuffer()->setTopoFreqs (lsrFreq, selFreq);
}


void
VLAT::fillLsrInfo (VlaDatum * datum)
{
    MPosition observatoryPositon;
    MDirection phaseCenter;
    Bool velocitySelection;


    Block<Int> channelGroupNumber;
    Block<Int> channelIncrement;
    Block<Int> channelStart;
    Block<Int> channelWidth;

    visibilityIterator_p->getLsrInfo (channelGroupNumber,
                                      channelIncrement,
                                      channelStart,
                                      channelWidth,
                                      observatoryPositon,
                                      phaseCenter,
                                      velocitySelection);

    datum->getVisBuffer()->setLsrInfo (channelGroupNumber,
                                       channelIncrement,
                                       channelStart,
                                       channelWidth,
                                       observatoryPositon,
                                       phaseCenter,
                                       velocitySelection);
}

void
VLAT::flushWrittenData ()
{
    for (int i = 0; i < (int) measurementSets_p.nelements() ; i++){
        measurementSets_p [i].flush();
    }
}

void
VLAT::handleWrite ()
{
    // While there is data to write out, write it out.

    Bool done = false;

    WriteQueue & writeQueue = interface_p->getWriteQueue ();

    do {

        WriteData * writeData = writeQueue.dequeue ();

        if (writeData != NULL){

            SubChunkPair subchunk = writeData->getSubChunkPair();

            alignWriteIterator (subchunk);

            writeData->write (writeIterator_p);
            delete writeData;
        }
        else{
            done = true;
        }

    } while (! done);
}


void
VLAT::initialize (const ROVisibilityIterator & rovi)
{
    ThrowIf (isStarted(), "VLAT::initialize: thread already started");

    visibilityIterator_p = new ROVisibilityIterator (rovi);

    visibilityIterator_p->originChunks (true);
    // force the MSIter, etc., to be rewound, reinitialized, etc.
}

void
VLAT::initialize (const Block<MeasurementSet> & mss,
                  const Block<Int> & sortColumns,
                  Bool addDefaultSortCols,
                  Double timeInterval,
                  Bool writable)
{
    ThrowIf (isStarted(), "VLAT::initialize: thread already started");

    visibilityIterator_p = new ROVisibilityIterator (mss, sortColumns, addDefaultSortCols, timeInterval);

    if (writable){
        writeIterator_p = new VisibilityIterator (mss, sortColumns, addDefaultSortCols, timeInterval);
        writeIterator_p->originChunks();
        writeIterator_p->origin ();

        measurementSets_p = mss;
    }

}

Bool
VLAT::isTerminated () const
{
    return threadTerminated_p;
}

void *
VLAT::run ()
{
    // Log thread initiation

    Logger::get()->registerName ("VLAT");
    String writable = writeIterator_p != NULL ? "writable" : "readonly";
    Log (1, "VLAT starting execution; tid=%d; VI is %s.\n", gettid(), writable.c_str());

    LogIO logIo (LogOrigin ("VLAT"));
    logIo << "starting execution; tid=" << gettid() << endl << LogIO::POST;

    // Enter run loop.  The run loop will only be exited when the main thread
    // explicitly asks for the termination of this thread (or an uncaught
    // exception comes to this level).


    try {
        do{
            Log (1, "VLAT starting VI sweep\n");

            // Start sweeping the real VI over its entire range
            // (subject to the number of free buffers).  The sweep
            // can be ended abruptly if

            sweepVi ();

            Bool startNewSweep = waitForViReset ();

            if (! startNewSweep){
                break; // Not resetting so it's time to quit
            }

        } while (true);

        handleWrite (); // service any pending writes

        flushWrittenData ();

        threadTerminated_p = true;
        Log (1, "VLAT stopping execution.\n");
        logIo << "stopping execution normally; tid=" << gettid() << endl << LogIO::POST;

        return NULL;

    }
    catch (std::exception & e){

        cerr << "VLAT thread caught exception: " << e.what() << endl;
        cerr.flush();

        Log (1, "VLAT caught exception: %s.\n", e.what());
        logIo << "caught exception; tid=" << gettid() << "-->" << e.what() << endl << LogIO::POST;

        threadTerminated_p = true;
        throw;
    }
    catch (...){

        cerr << "VLAT thread caught unknown exception: " << endl;
        cerr.flush();

        Log (1, "VLAT caught unknown exception:\n");
        logIo << "caught unknown exception; tid=" << gettid() << endl << LogIO::POST;

        threadTerminated_p = true;
        throw;
    }
}

void
VLAT::setPrefetchColumns (const ViReadImplAsync::PrefetchColumns & columns)
{
    ThrowIf (isStarted(), "VLAT::setColumns: cannot do this after thread started");
    ThrowIf (! fillers_p.empty(), "VLAT::setColumns:: has already been done");

    createFillerDictionary ();

    for (ViReadImplAsync::PrefetchColumns::const_iterator c = columns.begin();
            c != columns.end();
            c ++){

        ThrowIf (! containsKey (*c, fillerDictionary_p),
                 String::format ("Unknown prefetch column id (%d)", *c));

        fillers_p.push_back (fillerDictionary_p [*c]->clone());
    }

    //	sort (fillers_p.begin(),
    //	      fillers_p.end(),
    //	      VlatFunctor::byDecreasingPrecedence);
}

void
VLAT::sweepVi ()
{

    // Configure the iterator(s) to start a new sweep through the data.
    // Reset the subchunk counters, apply any queued modifiers and if
    // the write iterator exists reset it to the chunk origin (the
    // read iterator gets reset at the start of the sweep loop).

    readSubchunk_p.resetToOrigin ();
    writeSubchunk_p.resetToOrigin ();

    applyModifiers (visibilityIterator_p, writeIterator_p);

    if (writeIterator_p != NULL){
        writeIterator_p->originChunks (true);
    }

    // Start sweeping the data with the read only iterator.  If there
    // is a write iterator it will write behind the RO iterator; the RW
    // iterator is advanced to align with the appropriate subchunk when
    // a request is made to write a particular subchunk.

    try {

        for (visibilityIterator_p->originChunks(true);
             visibilityIterator_p->moreChunks();
             visibilityIterator_p->nextChunk(), readSubchunk_p.incrementChunk ()){

            for (visibilityIterator_p->origin();
                 visibilityIterator_p->more();
                 ++ (* visibilityIterator_p), readSubchunk_p.incrementSubChunk ()){

                ThreadTimes startTime = ThreadTimes ();

                waitUntilFillCanStart ();

                throwIfSweepTerminated ();

                VlaDatum * vlaDatum = vlaData_p -> fillStart (readSubchunk_p, startTime);

                throwIfSweepTerminated();

                fillDatum (vlaDatum);

                throwIfSweepTerminated ();

                vlaData_p -> fillComplete (vlaDatum);

                throwIfSweepTerminated ();

                handleWrite ();
            }
        }

        Log (1, "VLAT: no more data\n");

        vlaData_p -> setNoMoreData ();
    }
    catch (SweepTerminated &){
        Log (1, "VLAT: VI sweep termination requested.\n");
    }
    catch (AipsError e){
        Log (1, "AipsError during sweepVi; readSubchunk=%s, writeSubChunk=%s",
             readSubchunk_p.toString().c_str(), writeSubchunk_p.toString().c_str());
        throw;
    }
}

void
VLAT::terminate ()
{
    // Called by another thread to terminate the VLAT.

    Log (2, "Terminating VLAT\n");
    //printBacktrace (cerr, "VLAT termination");

    Thread::terminate(); // ask thread to terminate

    interface_p->terminateLookahead (); // stop lookahead
}

void
VLAT::throwIfSweepTerminated ()
{
    if (interface_p->isSweepTerminationRequested()){
        throw SweepTerminated ();
    }
}

Bool
VLAT::waitForViReset()
{
    UniqueLock uniqueLock (interface_p->getMutex());

    while (! interface_p->viResetRequested () &&
           ! interface_p->isLookaheadTerminationRequested ()){

        handleWrite (); // process any pending write requests

        // Wait for the interface to change:
        //
        //   o Buffer consumed by main thread
        //   o A write was requested
        //   o A sweep or thread termination was requested

        interface_p->waitForInterfaceChange (uniqueLock);
    }

    handleWrite (); // One more time to be sure that all writes are completed before
                    // we either quit or rewind the iterator.

    if (interface_p->isLookaheadTerminationRequested ()){
        return false;
    }
    else{

        vlaData_p->resetBufferData ();

        roviaModifiers_p = interface_p->transferRoviaModifiers ();

        interface_p->viResetComplete ();

        return true;
    }
}

void
VLAT::waitUntilFillCanStart ()
{
    UniqueLock uniqueLock (interface_p->getMutex());

    while ( ! vlaData_p->fillCanStart () &&
            ! interface_p->isSweepTerminationRequested ()){

        handleWrite (); // process any pending write requests

        // Wait for the interface to change:
        //
        //   o Buffer consumed by main thread
        //   o A write was requested
        //   o A sweep or thread termination was requested

        interface_p->waitForInterfaceChange (uniqueLock);
    }
}

void
VlatFunctor::operator() (VisBuffer *)
{
    ThrowIf (true, "No filler is defined for this VisBuffer component: " + name_p);
}


} // end namespace asyncio

using namespace casacore;
} // end namespace casa
