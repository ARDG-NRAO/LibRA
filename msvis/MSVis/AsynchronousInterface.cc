#include "AsynchronousInterface.h"
#include "AsynchronousTools.h"
#include "VLAT.h"
#include "UtilJ.h"

#include <casacore/casa/System/AipsrcValue.h>
#include <msvis/MSVis/VisBufferAsync.h>
#include <msvis/MSVis/VisibilityIteratorImplAsync.h>

#include <ostream>
#include <utility>

using namespace casacore;
using namespace casa::async;
using namespace casacore;
using namespace casa::utilj;
using namespace std;

#define Log(level, ...) \
        {if (AsynchronousInterface::logThis (level)) \
    Logger::get()->log (__VA_ARGS__);};

using casa::async::Mutex;

using namespace casacore;
namespace casa {

namespace asyncio {

Bool AsynchronousInterface::loggingInitialized_p = false;
Int AsynchronousInterface::logLevel_p = -1;

AsynchronousInterface::AsynchronousInterface (int maxNBuffers)
: lookaheadTerminationRequested_p (false),
  sweepTerminationRequested_p (false),
  viResetComplete_p (false),
  viResetRequested_p (false),
  vlaData_p (maxNBuffers, mutex_p),
  vlat_p (NULL),
  writeQueue_p ()
{}

AsynchronousInterface::~AsynchronousInterface ()
{}

void
AsynchronousInterface::addModifier (RoviaModifier * modifier)
{
    Log (1, "AsynchronousInterface::addModifier: {%s}\n", string(*modifier).c_str());

    LockGuard lg (mutex_p);

    roviaModifiers_p.add (modifier);
}

async::Mutex &
AsynchronousInterface::getMutex () const
{
    return mutex_p;
}

VlaData *
AsynchronousInterface::getVlaData ()
{
    return & vlaData_p;
}

VLAT *
AsynchronousInterface::getVlat ()
{
    return vlat_p;
}

WriteQueue &
AsynchronousInterface::getWriteQueue ()
{
    return writeQueue_p;
}



void
AsynchronousInterface::initialize ()
{
    initializeLogging ();

    vlaData_p.initialize (this);

    writeQueue_p.initialize (this);

    vlat_p = new VLAT (this);
}

Bool
AsynchronousInterface::initializeLogging ()
{
    if (loggingInitialized_p){
        return true;
    }

    loggingInitialized_p = true;

    // If the log file variable is defined then start
    // up the logger

    const String logFileVariable = "Casa_VIA_LogFile";
    const String logLevelVariable = "Casa_VIA_LogLevel";

    String logFilename;
    Bool logFileFound = AipsrcValue<String>::find (logFilename,
                                                   ROVisibilityIterator::getAsyncRcBase () + ".debug.logFile",
                                                   "");

    if (logFileFound &&
        ! logFilename.empty() &&
        downcase (logFilename) != "null" &&
        downcase (logFilename) != "none"){

        Logger::get()->start (logFilename.c_str());
        AipsrcValue<Int>::find (logLevel_p, ROVisibilityIterator::getAsyncRcBase () + ".debug.logLevel", 1);
        Logger::get()->log ("VlaData log-level is %d; async I/O: %s; nBuffers=%d\n",
                            logLevel_p,
                            ROVisibilityIterator::isAsynchronousIoEnabled() ? "enabled" : "disabled",
                            ViReadImplAsync::getDefaultNBuffers() );

        return true;

    }

    return false;
}

Bool
AsynchronousInterface::isLookaheadTerminationRequested () const
{
    return lookaheadTerminationRequested_p;
}


Bool
AsynchronousInterface::isSweepTerminationRequested () const
{
    return sweepTerminationRequested_p;
}

Bool
AsynchronousInterface::logThis (Int level)
{
    return loggingInitialized_p && level <= logLevel_p;
}

void
AsynchronousInterface::notifyAllInterfaceChanged () const
{
    interfaceDataChanged_p.notify_all();
}

void
AsynchronousInterface::requestViReset ()
{
    // Called by main thread to request that the VI reset to the
    // start of the MS.

    UniqueLock uniqueLock (mutex_p); // enter critical section

    Log (1, "Requesting VI reset\n");

    viResetRequested_p = true; // officially request the reset
    viResetComplete_p = false; // clear any previous completions

    terminateSweep ();

    // Wait for the request to be completed.

    Log (1, "Waiting for requesting VI reset\n");

    while (! viResetComplete_p){
        interfaceDataChanged_p.wait (uniqueLock);
    }

    Log (1, "Notified that VI reset has completed\n");

    // The VI was reset
}




void
AsynchronousInterface::terminate ()
{
    // Destroy the VLAT

    vlat_p->terminate(); // request termination
    vlat_p->join();      // wait for it to terminate
    delete vlat_p;       // free its storage
}

void
AsynchronousInterface::terminateLookahead ()
{
    // Called by main thread to stop the VLAT, etc.

    LockGuard lg (& mutex_p);

    lookaheadTerminationRequested_p = true;

    terminateSweep();
}

void
AsynchronousInterface::terminateSweep ()
{
    // Called internally to terminate VI sweeping.

    sweepTerminationRequested_p = true;   // stop filling

    notifyAllInterfaceChanged();
}

RoviaModifiers
AsynchronousInterface::transferRoviaModifiers ()
{
    return roviaModifiers_p.transferModifiers();
}

void
AsynchronousInterface::viResetComplete ()
{
    ////Assert (mutex_p.isLockedByThisThread());

    viResetRequested_p = false;
    sweepTerminationRequested_p = false;
    viResetComplete_p = true;

    notifyAllInterfaceChanged();
}

Bool
AsynchronousInterface::viResetRequested ()
{
    ////Assert (mutex_p.isLockedByThisThread());

    return viResetRequested_p;
}

void
AsynchronousInterface::waitForInterfaceChange (async::UniqueLock & uniqueLock) const
{
    interfaceDataChanged_p.wait (uniqueLock);
}

ChannelSelection::ChannelSelection (const Block< Vector<Int> > & blockNGroup,
                                    const Block< Vector<Int> > & blockStart,
                                    const Block< Vector<Int> > & blockWidth,
                                    const Block< Vector<Int> > & blockIncr,
                                    const Block< Vector<Int> > & blockSpw)
{
    blockNGroup_p = blockNGroup;
    blockStart_p = blockStart;
    blockWidth_p = blockWidth;
    blockIncr_p = blockIncr;
    blockSpw_p = blockSpw;
}

ChannelSelection::ChannelSelection (const ChannelSelection & other)
{
    * this = other;
}

ChannelSelection &
ChannelSelection::operator= (const ChannelSelection & other)
{
    if (this != & other){

        copyBlock (other.blockNGroup_p, blockNGroup_p);
        copyBlock (other.blockStart_p, blockStart_p);
        copyBlock (other.blockWidth_p, blockWidth_p);
        copyBlock (other.blockIncr_p, blockIncr_p);
        copyBlock (other.blockSpw_p, blockSpw_p);

    }

    return * this;
}

void
ChannelSelection::copyBlock (const Block <Vector<Int> > & src,
                             Block <Vector<Int> > & to) const
{
    // Since this is a Block of Vector, we need to wipe out
    // the original contents of "to"; otherwise the semantics
    // of Vector::operator= will generate an exception if there
    // is a difference in length of any of the vector elements.

    to.resize (0, true);
    to = src;
}


void
ChannelSelection::get (Block< Vector<Int> > & blockNGroup,
                       Block< Vector<Int> > & blockStart,
                       Block< Vector<Int> > & blockWidth,
                       Block< Vector<Int> > & blockIncr,
                       Block< Vector<Int> > & blockSpw) const
{
    copyBlock (blockNGroup_p, blockNGroup);
    copyBlock (blockStart_p, blockStart);
    copyBlock (blockWidth_p, blockWidth);
    copyBlock (blockIncr_p, blockIncr);
    copyBlock (blockSpw_p, blockSpw);
}

std::ostream &
operator<< (std::ostream & o, const RoviaModifier & m)
{
    m.print (o);

    return o;
}

RoviaModifiers::~RoviaModifiers ()
{
//    // Free the objects owned by the vector
//
//    for (Data::iterator i = data_p.begin(); i != data_p.end(); i++){
//        delete (* i);
//    }
}

void
RoviaModifiers::add (RoviaModifier * modifier)
{
    data_p.push_back (modifier);
}

void
RoviaModifiers::apply (ROVisibilityIterator * rovi)
{
    // Free the objects owned by the vector

    for (Data::iterator i = data_p.begin(); i != data_p.end(); i++){
        Log (1, "Applying vi modifier: %s\n", string(** i).c_str());
        (* i) -> apply (rovi);
    }

}

void
RoviaModifiers::clearAndFree ()
{
    for (Data::iterator i = data_p.begin(); i != data_p.end(); i++){
        delete (* i);
    }

    data_p.clear();
}

RoviaModifiers
RoviaModifiers::transferModifiers ()
{
    RoviaModifiers result;

    result.data_p.assign (data_p.begin(), data_p.end());

    data_p.clear(); // remove them from the other object but do not destroy them

    return result;
}

SelectChannelModifier::SelectChannelModifier (Int nGroup, Int start, Int width, Int increment, Int spectralWindow)
: channelBlocks_p (false),
  increment_p (increment),
  nGroup_p (nGroup),
  spectralWindow_p (spectralWindow),
  start_p (start),
  width_p (width)
{}

SelectChannelModifier::SelectChannelModifier (const Block< Vector<Int> > & blockNGroup,
                                              const Block< Vector<Int> > & blockStart,
                                              const Block< Vector<Int> > & blockWidth,
                                              const Block< Vector<Int> > & blockIncr,
                                              const Block< Vector<Int> > & blockSpw)
: channelBlocks_p (true),
  channelSelection_p (blockNGroup, blockStart, blockWidth, blockIncr, blockSpw)
{}

void
SelectChannelModifier::apply (ROVisibilityIterator * rovi) const
{
    if (! channelBlocks_p){
        rovi->selectChannel (nGroup_p, start_p, width_p, increment_p, spectralWindow_p);
    }
    else{
        Block< Vector<Int> > blockNGroup;
        Block< Vector<Int> > blockStart;
        Block< Vector<Int> > blockWidth;
        Block< Vector<Int> > blockIncr;
        Block< Vector<Int> > blockSpw;

        channelSelection_p.get (blockNGroup, blockStart, blockWidth, blockIncr, blockSpw);
        rovi->selectChannel (blockNGroup, blockStart, blockWidth, blockIncr, blockSpw);
    }
}

void
SelectChannelModifier::print (ostream & os) const
{
    os << "SelectChannel::{";

    if (channelBlocks_p){
        Block< Vector<Int> > blockNGroup;
        Block< Vector<Int> > blockStart;
        Block< Vector<Int> > blockWidth;
        Block< Vector<Int> > blockIncr;
        Block< Vector<Int> > blockSpw;

        channelSelection_p.get (blockNGroup, blockStart, blockWidth, blockIncr, blockSpw);

        os << "nGroup=" << toCsv (blockNGroup)
           << ", start=" << toCsv (blockStart)
           << ", width=" << toCsv (blockWidth)
           << ", increment=" << toCsv (blockIncr)
           << ", spw=" << toCsv (blockSpw);
    }
    else {
        os << "nGroup=" << nGroup_p
           << ", start=" << start_p
           << ", width=" << width_p
           << ", increment=" << increment_p
           << ", spw=" << spectralWindow_p;
    }
    os << "}";
}

String
SelectChannelModifier::toCsv (const Block< Vector<Int> > & bv) const
{
    String result = "{";

    for (Block<Vector<Int> >::const_iterator v = bv.begin(); v != bv.end(); ++ v){
        if (result.size() != 1)
            result += ",";

        result += "{" + toCsv (* v) + "}";

    }

    result += "}";

    return result;

}

String
SelectChannelModifier::toCsv (const Vector<Int> & v) const
{
    String result = "";
    for (Vector<Int>::const_iterator i = v.begin(); i != v.end(); ++ i){
        if (! result.empty())
            result += ",";
        result +=  String::toString (* i);
    }

    return result;
}


SelectVelocityModifier::SelectVelocityModifier (Int nChan, const MVRadialVelocity& vStart, const MVRadialVelocity& vInc,
                                                MRadialVelocity::Types rvType, MDoppler::Types dType, Bool precise)

: dType_p (dType),
  nChan_p (nChan),
  precise_p (precise),
  rvType_p (rvType),
  vInc_p (vInc),
  vStart_p (vStart)
{}

void
SelectVelocityModifier::apply (ROVisibilityIterator * rovi) const
{
    rovi-> selectVelocity (nChan_p, vStart_p, vInc_p, rvType_p, dType_p, precise_p);
}

void
SelectVelocityModifier::print (std::ostream & os) const
{
    os << "SelectVelocity::{"

       << "dType=" << dType_p
       << ",nChan=" << nChan_p
       << ",precise=" << precise_p
       << ",rvType=" << rvType_p
       << ",vInc=" << vInc_p
       << ",vStart=" << vStart_p
       << "}";
}

SetIntervalModifier::SetIntervalModifier (Double timeInterval)
: timeInterval_p (timeInterval)
{}

void
SetIntervalModifier::apply (ROVisibilityIterator * rovi) const
{
    rovi -> setInterval (timeInterval_p);
}

void
SetIntervalModifier::print (std::ostream & os) const
{
    os << "SetInterval::{" << timeInterval_p << "}";
}



SetRowBlockingModifier::SetRowBlockingModifier (Int nRows)
: nRows_p (nRows)
{}

void
SetRowBlockingModifier::apply (ROVisibilityIterator * rovi) const
{
    rovi->setRowBlocking (nRows_p);
}

void
SetRowBlockingModifier::print (std::ostream & os) const
{
    os << "SetRowBlocking::{"
       << "nRows=" << nRows_p
       << ",nGroup=" << nGroup_p
       << ",spectralWindow=" << spectralWindow_p
       << ",start=" << start_p
       << ",width=" << width_p
       << "}";
}


//  **************************
//  *                        *
//  * VlaData Implementation *
//  *                        *
//  **************************

//Semaphore VlaData::debugBlockSemaphore_p (0); // used to block a thread for debugging

VlaData::VlaData (Int maxNBuffers, async::Mutex & mutex)
: MaxNBuffers_p (maxNBuffers),
  mutex_p (mutex)
{
    timing_p.fillCycle_p = DeltaThreadTimes (true);
    timing_p.fillOperate_p = DeltaThreadTimes (true);
    timing_p.fillWait_p = DeltaThreadTimes (true);
    timing_p.readCycle_p = DeltaThreadTimes (true);
    timing_p.readOperate_p = DeltaThreadTimes (true);
    timing_p.readWait_p = DeltaThreadTimes (true);
    timing_p.timeStart_p = ThreadTimes();
}

VlaData::~VlaData ()
{
    timing_p.timeStop_p = ThreadTimes();

    if (statsEnabled()){
        Log (1, "VlaData stats:\n%s", makeReport ().c_str());
    }

    resetBufferData ();
}


Int
VlaData::clock (Int arg, Int base)
{
    Int r = arg % base;

    if (r < 0){
        r += base;
    }

    return r;
}

//void
//VlaData::debugBlock ()
//{
//    //    Log (1, "VlaData::debugBlock(): Blocked\n");
//    //
//    //    debugBlockSemaphore_p.wait ();
//    //
//    //    Log (1, "VlaData::debugBlock(): Unblocked\n");
//}

//void
//VlaData::debugUnblock ()
//{
//    //    int v = debugBlockSemaphore_p.getValue();
//    //
//    //    if (v == 0){
//    //        Log (1, "VlaData::debugUnblock()\n");
//    //        debugBlockSemaphore_p.post ();
//    //    }
//    //    else
//    //        Log (1, "VlaData::debugUnblock(): already unblocked; v=%d\n", v);
//}


void
VlaData::fillComplete (VlaDatum * datum)
{
    LockGuard lg (mutex_p);

    if (statsEnabled()){
        timing_p.fill3_p = ThreadTimes();
        timing_p.fillWait_p += timing_p.fill2_p - timing_p.fill1_p;
        timing_p.fillOperate_p += timing_p.fill3_p - timing_p.fill2_p;
        timing_p.fillCycle_p += timing_p.fill3_p - timing_p.fill1_p;
    }

    data_p.push (datum);

    Log (2, "VlaData::fillComplete on %s\n", datum->getSubChunkPair ().toString().c_str());

    assert ((Int)data_p.size() <= MaxNBuffers_p);

    interface_p->notifyAllInterfaceChanged();
}

Bool
VlaData::fillCanStart () const
{
    // Caller must lock

    Bool canStart = (int) data_p.size() < MaxNBuffers_p;

    return canStart;
}


VlaDatum *
VlaData::fillStart (SubChunkPair subchunk, const ThreadTimes & fillStartTime)
{
    LockGuard lg (mutex_p);

    statsEnabled () && (timing_p.fill1_p = fillStartTime, true);

    Assert ((int) data_p.size() < MaxNBuffers_p);

    VlaDatum * datum = new VlaDatum (subchunk);

    Log (2, "VlaData::fillStart on %s\n", datum->getSubChunkPair().toString().c_str());

    if (validChunks_p.empty() || validChunks_p.back() != subchunk.chunk ())
        insertValidChunk (subchunk.chunk ());

    insertValidSubChunk (subchunk);

    statsEnabled () && (timing_p.fill2_p = ThreadTimes(), true);

    if (interface_p->isSweepTerminationRequested()){
        delete datum;
        datum = NULL; // datum may not be ready to fill and shouldn't be anyway
    }

    return datum;
}

asyncio::ChannelSelection
VlaData::getChannelSelection () const
{
    LockGuard lg (mutex_p);

    return channelSelection_p;
}

void
VlaData::initialize (const AsynchronousInterface * interface)
{
    interface_p = interface;

    LockGuard lg (mutex_p);

    resetBufferData ();
}


void
VlaData::insertValidChunk (Int chunkNumber)
{
    ////Assert (mutex_p.isLockedByThisThread ()); // Caller locks mutex.

    validChunks_p.push (chunkNumber);

    interface_p->notifyAllInterfaceChanged();
}

void
VlaData::insertValidSubChunk (SubChunkPair subchunk)
{
    ////Assert (mutex_p.isLockedByThisThread ()); // Caller locks mutex.

    validSubChunks_p.push (subchunk);

    interface_p->notifyAllInterfaceChanged();
}

//Bool
//VlaData::isSweepTerminationRequested () const
//{
//    return sweepTerminationRequested_p;
//}

Bool
VlaData::isValidChunk (Int chunkNumber) const
{
    bool validChunk = false;

    // Check to see if this is a valid chunk.  If the data structure is empty
    // then sleep for a tiny bit to allow the VLAT thread to either make more
    // chunks available for insert the sentinel value INT_MAX into the data
    // structure.

    UniqueLock uniqueLock (mutex_p);

    do {

        while (validChunks_p.empty()){
            interface_p->waitForInterfaceChange (uniqueLock);
        }

        while (! validChunks_p.empty() && validChunks_p.front() < chunkNumber){
            validChunks_p.pop();
        }

        if (! validChunks_p.empty())
            validChunk = validChunks_p.front() == chunkNumber;

    } while (validChunks_p.empty());

    Log (3, "isValidChunk (%d) --> %s\n", chunkNumber, validChunk ? "true" : "false");

    return validChunk;
}

Bool
VlaData::isValidSubChunk (SubChunkPair subchunk) const
{
    SubChunkPair s;

    bool validSubChunk = false;

    // Check to see if this is a valid subchunk.  If the data structure is empty
    // then sleep for a tiny bit to allow the VLAT thread to either make more
    // subchunks available for insert the sentinel value (INT_MAX, INT_MAX) into the data
    // structure.

    UniqueLock uniqueLock (mutex_p);

    do {

        while (validSubChunks_p.empty()){
            interface_p->waitForInterfaceChange (uniqueLock);
        }

        while (! validSubChunks_p.empty() && validSubChunks_p.front() < subchunk){
            validSubChunks_p.pop();
        }

        if (! validSubChunks_p.empty())
            validSubChunk = validSubChunks_p.front() == subchunk;

    } while (validSubChunks_p.empty());

    Log (3, "isValidSubChunk %s --> %s\n", subchunk.toString().c_str(), validSubChunk ? "true" : "false");

    return validSubChunk;
}

String
VlaData::makeReport ()
{
    String report;

    DeltaThreadTimes duration = (timing_p.timeStop_p - timing_p.timeStart_p); // seconds
    report += String::format ("\nLookahead Stats: nCycles=%d, duration=%.3f sec\n...\n",
                      timing_p.readWait_p.n(), duration.elapsed());
    report += "...ReadWait:    " + timing_p.readWait_p.formatAverage () + "\n";
    report += "...ReadOperate: " + timing_p.readOperate_p.formatAverage() + "\n";
    report += "...ReadCycle:   " + timing_p.readCycle_p.formatAverage() + "\n";

    report += "...FillWait:    " + timing_p.fillWait_p.formatAverage() + "\n";
    report += "...FillOperate: " + timing_p.fillOperate_p.formatAverage () + "\n";
    report += "...FillCycle:   " + timing_p.fillCycle_p.formatAverage () + "\n";

    Double syncCycle = timing_p.fillOperate_p.elapsedAvg() + timing_p.readOperate_p.elapsedAvg();
    Double asyncCycle = max (timing_p.fillCycle_p.elapsedAvg(), timing_p.readCycle_p.elapsedAvg());
    report += String::format ("...Sync cycle would be %6.1f ms\n", syncCycle * 1000);
    report += String::format ("...Speedup is %5.1f%%\n", (syncCycle / asyncCycle  - 1) * 100);
    report += String::format ("...Total time savings estimate is %7.3f seconds\n",
                      (syncCycle - asyncCycle) * timing_p.readWait_p.n());

    return report;

}


void
VlaData::readComplete (SubChunkPair subchunk)
{
    LockGuard lg (mutex_p);

    if (statsEnabled()){
        timing_p.read3_p = ThreadTimes();
        timing_p.readWait_p += timing_p.read2_p - timing_p.read1_p;
        timing_p.readOperate_p += timing_p.read3_p - timing_p.read2_p;
        timing_p.readCycle_p += timing_p.read3_p - timing_p.read1_p;
    }

    Log (2, "VlaData::readComplete on %s\n", subchunk.toString().c_str());
}

VisBufferAsync *
VlaData::readStart (SubChunkPair subchunk)
{
    // Called by main thread

    UniqueLock uniqueLock (mutex_p);

    statsEnabled () && (timing_p.read1_p = ThreadTimes(), true);

    // Wait for a subchunk's worth of data to be available.

    while (data_p.empty()){
        interface_p->waitForInterfaceChange (uniqueLock);
    }

    // Get the data off the queue and notify world of change in VlaData.

    VlaDatum * datum = data_p.front();
    data_p.pop ();
    interface_p->notifyAllInterfaceChanged();

    ThrowIf (! datum->isSubChunk (subchunk),
             String::format ("Reader wanted subchunk %s while next subchunk is %s",
                            subchunk.toString().c_str(), datum->getSubChunkPair().toString().c_str()));

    Log (2, "VlaData::readStart on %s\n", subchunk.toString().c_str());

    statsEnabled () && (timing_p.read2_p = ThreadTimes(), true);

    // Extract the VisBufferAsync enclosed in the datum for return to caller,
    // then destroy the rest of the datum object

    VisBufferAsync * vba = datum->releaseVisBufferAsync ();
    delete datum;
    return vba;
}

void
VlaData::resetBufferData ()
{
    ////Assert (mutex_p.isLockedByThisThread ()); // Caller locks mutex.

    // Flush any accumulated buffers

    while (! data_p.empty()){
        VlaDatum * datum = data_p.front();
        data_p.pop ();
        delete datum;
    }

    // Flush the chunk and subchunk indices

    while (! validChunks_p.empty())
        validChunks_p.pop();

    while (! validSubChunks_p.empty())
        validSubChunks_p.pop();
}

void
VlaData::setNoMoreData ()
{
    LockGuard lg (mutex_p);

    insertValidChunk (INT_MAX);
    insertValidSubChunk (SubChunkPair::noMoreData ());
}

Bool
VlaData::statsEnabled () const
{
    // Determines whether asynchronous I/O is enabled by looking for the
    // expected AipsRc value.  If not found then async i/o is disabled.

    Bool doStats;
    AipsrcValue<Bool>::find (doStats, ROVisibilityIterator::getAsyncRcBase () + ".doStats", false);

    return doStats;
}

void
VlaData::storeChannelSelection (const asyncio::ChannelSelection & channelSelection)
{
    LockGuard lg (mutex_p);

    channelSelection_p = channelSelection;
}



//  ***************************
//  *                         *
//  * VlaDatum Implementation *
//  *                         *
//  ***************************

VlaDatum::VlaDatum (SubChunkPair subchunk)
: subchunk_p (subchunk),
  visBuffer_p (new VisBufferAsync ())
{}

VlaDatum::~VlaDatum()
{
    delete visBuffer_p;
}

SubChunkPair
VlaDatum::getSubChunkPair () const
{
    return subchunk_p;
}

VisBufferAsync *
VlaDatum::getVisBuffer ()
{
    return visBuffer_p;
}

//const VisBufferAsync *
//VlaDatum::getVisBuffer () const
//{
//    assert (state_p == Filling || state_p == Reading);
//
//    return visBuffer_p;
//}

Bool
VlaDatum::isSubChunk (SubChunkPair subchunk) const
{
    return subchunk == subchunk_p;
}

VisBufferAsync *
VlaDatum::releaseVisBufferAsync ()
{
    VisBufferAsync * vba = visBuffer_p;
    visBuffer_p = NULL;

    return vba;
}

WriteQueue::WriteQueue ()
: interface_p (NULL)
{}

WriteQueue::~WriteQueue ()
{
    Assert (queue_p.empty());
}

WriteData *
WriteQueue::dequeue ()
{
    LockGuard lg (mutex_p);

    WriteData * result = NULL;

    if (! empty (true)){

        result = queue_p.front(); // get the first value
        queue_p.pop();            // remove it from the queue
    }

    return result;
}

Bool
WriteQueue::empty (Bool alreadyLocked)
{
    Bool isEmpty;

    if (alreadyLocked){
        isEmpty = queue_p.empty();
    }
    else {
        LockGuard lg (mutex_p);
        isEmpty = queue_p.empty();
    }

    return isEmpty;
}

void
WriteQueue::enqueue (WriteData * writeData)
{
    Assert (writeData != NULL);

    LockGuard lg (mutex_p);

    queue_p.push (writeData);

    interface_p->notifyAllInterfaceChanged ();
}

void
WriteQueue::initialize (const AsynchronousInterface * interface)
{
    interface_p = interface;
}

} // end namespace asyncio

using namespace casacore;
} // end namespace casa
