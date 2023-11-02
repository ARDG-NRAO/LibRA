/*
 * VisibilityIteratorAsync.cc
 *
 *  Created on: Nov 2, 2010
 *      Author: jjacobs
 */

#include <msvis/MSVis/VisibilityIterator.h>
#include <msvis/MSVis/VisibilityIteratorImplAsync.h>
#include <msvis/MSVis/VisBufferAsync.h>
#include <msvis/MSVis/VisBufferAsyncWrapper.h>
#include <msvis/MSVis/VLAT.h>
#include <msvis/MSVis/AsynchronousInterface.h>
#include <stdcasa/UtilJ.h>
#include <casacore/casa/Utilities/GenSort.h>

#include <casacore/ms/MeasurementSets/MSColumns.h>
#include <casacore/casa/System/AipsrcValue.h>

#include <algorithm>
#include <cstdarg>
#include <ostream>

using namespace std;

using namespace casacore;
using namespace casa::utilj;

using namespace casacore;
using namespace casa::asyncio;

#define Log(level, ...) \
        {if (VlaData::loggingInitialized_p && level <= VlaData::logLevel_p) \
    Logger::get()->log (__VA_ARGS__);};

#define NotImplemented     Throw ("VisibilityIteratorReadImplAsync: Method not implemented!");

#define VIWIA_NotImplemented() \
        Throw ("ViWriteImplAsync --> Operation not permitted!\n" \
               "Modify VisBuffer, mark components as dirty and use VisibilityIterator::writeBack.");

#define ThrowIfNoVbaAttached() \
        ThrowIf (visBufferAsync_p == 0, "No VisBufferAsync attached to VI; try doing vi.origin() first.");


using namespace casacore;
namespace casa {


ViReadImplAsync::ViReadImplAsync (const Block<MeasurementSet> & mss,
                                  const PrefetchColumns & prefetchColumns,
                                  const Block<Int> & sortColumns,
                                  const Bool addDefaultSortCols,
                                  Double timeInterval,
                                  Bool writable)
: visBufferAsync_p (NULL),
  vlaData_p (NULL),
  vlat_p (NULL)
{
    construct (mss, prefetchColumns, sortColumns, addDefaultSortCols,
               timeInterval, writable);
}

ViReadImplAsync::ViReadImplAsync (const PrefetchColumns & prefetchColumns,
                                  const VisibilityIteratorReadImpl & other,
                                  Bool writable)
: visBufferAsync_p (NULL),
  vlaData_p (NULL),
  vlat_p (NULL)
{
    Block<MeasurementSet> mss (other.measurementSets_p.size());

    for (int i = 0; i < (Int) other.measurementSets_p.size(); i++){
        mss [i] = other.measurementSets_p [i];
    }

    construct (mss, prefetchColumns, other.sortColumns_p,
               other.addDefaultSort_p, other.timeInterval_p, writable);

    imwgt_p = other.imwgt_p;

    // Pass over to the VLAT some of the VI modifiers if they are
    // not at the default value.

    Bool needViReset = false;

    if (other.timeInterval_p != 0){
        setInterval (other.timeInterval_p);
        needViReset = true;
    }

    if (other.nRowBlocking_p != 0){
        setRowBlocking (other.nRowBlocking_p);
        needViReset = true;
    }

    if (other.msChannels_p.nGroups_p.nelements() != 0){
        selectChannel (other.msChannels_p.nGroups_p,
                       other.msChannels_p.start_p,
                       other.msChannels_p.width_p,
                       other.msChannels_p.inc_p,
                       other.msChannels_p.spw_p);
        needViReset = true;
    }

    if (needViReset){
        originChunks ();
    }
}


ViReadImplAsync::~ViReadImplAsync ()
{
    if (! vbaWrapperStack_p.empty()){
        VisBufferAsync * vba =
                vbaWrapperStack_p.top()->releaseVba ();
        assert (vba == visBufferAsync_p);
        UnusedVariable (vba);
        vba = NULL; // prevent warning when in non425debug build
        delete visBufferAsync_p;
    }
    else if (visBufferAsync_p != NULL){
        delete visBufferAsync_p;
    }

    interface_p->terminate ();
    delete interface_p;
}

void
ViReadImplAsync::advance ()
{
    //readComplete (); // complete any pending read

    subchunk_p.incrementSubChunk ();

    fillVisBuffer ();
}

Bool
ViReadImplAsync::allBeamOffsetsZero () const
{
    ThrowIf (visBufferAsync_p == NULL, "No attached VisBufferAsync");

    return visBufferAsync_p -> getAllBeamOffsetsZero ();
}

const Vector<String> &
ViReadImplAsync::antennaMounts () const
{
    ThrowIf (visBufferAsync_p == NULL, "No attached VisBufferAsync");

    return visBufferAsync_p -> getAntennaMounts ();
}


void
ViReadImplAsync::attachVisBuffer (VisBuffer & vb0)
{
    VisBufferAsyncWrapper * vb = dynamic_cast<VisBufferAsyncWrapper *> (& vb0);
    ThrowIf (vb == NULL, "Attempt to attach other than VisBufferAsyncWrapper");

    if (! vbaWrapperStack_p.empty()){
        vbaWrapperStack_p.top () -> releaseVba ();
    }

    vbaWrapperStack_p.push (vb);

    if (visBufferAsync_p != NULL){
        vb->wrap (visBufferAsync_p);
    }
}

ViReadImplAsync::PrefetchColumns
ViReadImplAsync::augmentPrefetchColumns (const PrefetchColumns & prefetchColumnsBase)
{
    // Augment the list of prefetch columns with any that are implied
    // by the others.  N.B., be wary of reordering these.

    PrefetchColumns prefetchColumns = prefetchColumnsBase;

    if (contains (VisBufferComponents::Direction1, prefetchColumns)){
        prefetchColumns.insert (VisBufferComponents::AllBeamOffsetsZero);
        prefetchColumns.insert (VisBufferComponents::AntennaMounts);
        prefetchColumns.insert (VisBufferComponents::Feed1_pa);
        prefetchColumns.erase (VisBufferComponents::Direction1);
    }

    if (contains (VisBufferComponents::Direction2, prefetchColumns)){
        prefetchColumns.insert (VisBufferComponents::AllBeamOffsetsZero);
        prefetchColumns.insert (VisBufferComponents::AntennaMounts);
        prefetchColumns.insert (VisBufferComponents::Feed2_pa);
        prefetchColumns.erase (VisBufferComponents::Direction2);
    }

    if (contains (VisBufferComponents::Feed1_pa, prefetchColumns)){
        prefetchColumns.insert (VisBufferComponents::Ant1);
        prefetchColumns.insert (VisBufferComponents::Feed1);
        prefetchColumns.insert (VisBufferComponents::PhaseCenter);
        prefetchColumns.insert (VisBufferComponents::ReceptorAngles);
        prefetchColumns.insert (VisBufferComponents::Time);
        prefetchColumns.insert (VisBufferComponents::TimeInterval);
        prefetchColumns.erase (VisBufferComponents::Feed1_pa);
    }

    if (contains (VisBufferComponents::Feed2_pa, prefetchColumns)){
        prefetchColumns.insert (VisBufferComponents::Ant2);
        prefetchColumns.insert (VisBufferComponents::Feed2);
        prefetchColumns.insert (VisBufferComponents::PhaseCenter);
        prefetchColumns.insert (VisBufferComponents::ReceptorAngles);
        prefetchColumns.insert (VisBufferComponents::Time);
        prefetchColumns.insert (VisBufferComponents::TimeInterval);
        prefetchColumns.erase (VisBufferComponents::Feed2_pa);
    }

    return prefetchColumns;
}

const Cube<RigidVector<Double, 2> >&
ViReadImplAsync::getBeamOffsets () const
{
    ThrowIf (visBufferAsync_p == NULL, "No attached VisBufferAsync");

    return visBufferAsync_p -> getBeamOffsets ();
}



VisibilityIteratorReadImpl *
ViReadImplAsync::clone () const
{
    Throw ("ViReadImplAsync: cloning not permitted!!!");
}

void
ViReadImplAsync::construct(const Block<MeasurementSet> & mss,
                           const PrefetchColumns & prefetchColumns,
                           const Block<Int> & sortColumns,
                           const Bool addDefaultSortCols,
                           Double timeInterval,
                           Bool writable)
{
    AsynchronousInterface::initializeLogging();

    casa::async::Logger::get()->registerName ("Main");

    visBufferAsync_p = NULL;

    for (uint i = 0; i < mss.size(); i++){
        measurementSets_p.push_back (mss [i]);
    }

    msId_p = -1;

    // Create and initialize the Asynchronous Interface

    Int nReadAheadBuffers = getDefaultNBuffers ();

    interface_p = new AsynchronousInterface (nReadAheadBuffers);
    interface_p->initialize();

    vlaData_p = interface_p->getVlaData ();
    vlat_p = interface_p->getVlat ();

    // Augment the list of prefetch columns with any that are implied
    // by the others.  N.B., be wary of reordering these.

    prefetchColumns_p = augmentPrefetchColumns (prefetchColumns);

    // Get the VLAT going

    vlat_p->setPrefetchColumns (prefetchColumns_p);

    // If this is a writable VI then let the write implementation handle the
    // initialization of the VLAT.

    vlat_p->initialize (mss, sortColumns, addDefaultSortCols, timeInterval, writable);
    vlat_p->startThread ();
}

void
ViReadImplAsync::detachVisBuffer (VisBuffer & vb0)
{
    VisBufferAsyncWrapper * vb = dynamic_cast<VisBufferAsyncWrapper *> (& vb0);

    ThrowIf (vb == NULL, "Attempt to detach other than a VisBufferAsyncWrapper");

    if (vb == vbaWrapperStack_p.top()){

        // Get rid of the old buffer

        VisBufferAsync * vba = vb->releaseVba ();
        Assert (vba == visBufferAsync_p);
        UnusedVariable (vba); // prevent release build warning

        vbaWrapperStack_p.pop ();

        // If there is still a VB attached either fill it with the
        // current values for the VI position or clear it.

        if (! vbaWrapperStack_p.empty()){

            if (visBufferAsync_p != NULL){
                vbaWrapperStack_p.top() -> wrap (visBufferAsync_p);
            }
        }

    } else {
        Throw ("ViReadImplAsync::detachVisBuffer: VisBufferAsync not attached ");
    }
}

void
ViReadImplAsync::dumpPrefetchColumns () const
{
    int i = 0;
    for (PrefetchColumns::const_iterator c = prefetchColumns_p.begin();
            c != prefetchColumns_p.end();
            c ++){

        cerr << PrefetchColumns::columnName (*c) << ", ";
        i ++;
        if (i == 10){
            cerr << endl;
            i = 0;
        }

    }

    if (i != 0)
        cerr << endl;

}

void
ViReadImplAsync::fillVisBuffer()
{
    // Get the next buffer from the lookahead buffer ring.
    // This could block if the next buffer is not ready.
    // Before doing the fill check to see that there's more data.

    if (more()){

        readComplete ();

        visBufferAsync_p = vlaData_p->readStart (subchunk_p);

        Assert (visBufferAsync_p != NULL);

        msId_p = visBufferAsync_p->msId ();

        // If a VisBufferAsync is attached, then copy the prefetched VisBuffer into it.

        if (! vbaWrapperStack_p.empty ()){

            vbaWrapperStack_p.top() -> wrap (visBufferAsync_p);
        }
    }
}

//Vector<MDirection>
//ViReadImplAsync::fillAzel(Double time) const
//{
//    NotImplemented;
//}




//void
//ViReadImplAsync::fillVisBuffer ()
//{
//    vlaDatum_p = impl_p->getVlaData()->getDatum (subchunk_p);
//
//    ThrowIf (datum == NULL, format("Failed to get datum for subchunk (%d, %d)", subchunk_p));
//
//    if (visBuffer_p != NULL){
//        visBuffer_p->setRealBuffer (vlaDatum_p->getVisBuffer());
//    }
//}

void
ViReadImplAsync::getChannelSelection(Block< Vector<Int> >& blockNGroup,
                                     Block< Vector<Int> >& blockStart,
                                     Block< Vector<Int> >& blockWidth,
                                     Block< Vector<Int> >& blockIncr,
                                     Block< Vector<Int> >& blockSpw){

    asyncio::ChannelSelection channelSelection = vlaData_p->getChannelSelection ();
    channelSelection.get (blockNGroup, blockStart, blockWidth, blockIncr, blockSpw);
}

int
ViReadImplAsync::getDefaultNBuffers ()
{
    int nBuffers;
    casacore::AipsrcValue<Int>::find (nBuffers, ROVisibilityIterator::getAsyncRcBase () + ".nBuffers", 2);

    return nBuffers;
}

MEpoch
ViReadImplAsync::getEpoch () const
{
    ThrowIf (visBufferAsync_p == NULL, "No attached VisBufferAsync");

    return visBufferAsync_p -> mEpoch_p;
}

//ViReadImplAsync::PrefetchColumns
//ViReadImplAsync::getPrefetchColumns () const
//{
//    return prefetchColumns_p;
//}
const MeasurementSet &
ViReadImplAsync::getMs() const
{
    ThrowIfNoVbaAttached ();

    return visBufferAsync_p->getMs ();
}


VisBuffer *
ViReadImplAsync::getVisBuffer ()
{
    // Returns the currently attached VisBufferAsync or NULL if none attached

    VisBuffer * vb = (! vbaWrapperStack_p.empty()) ? vbaWrapperStack_p.top() : NULL;
    return vb;
}

//void
//ViReadImplAsync::linkWithRovi (VisibilityIteratorReadImpl * rovi)
//{
//    linkedVisibilityIterator_p = rovi;
//}


Bool
ViReadImplAsync::more () const
{
    // Returns true if the lookahead data structure has the next subchunk.

    Bool b = vlaData_p->isValidSubChunk (subchunk_p);

    return b;
}

Bool
ViReadImplAsync::moreChunks () const
{
    // Returns true if the looahead data structure has the first subchunk of the
    // next chunk.

    Bool b = vlaData_p->isValidChunk (subchunk_p.chunk());

    return b;
}

const MSColumns &
ViReadImplAsync::msColumns() const
{
    ThrowIfNoVbaAttached ();

    return visBufferAsync_p->msColumns();
}


Int
ViReadImplAsync::msId() const
{
    ThrowIf (msId_p < 0, "MS ID value not currently known.");

    return msId_p;
}


ViReadImplAsync &
ViReadImplAsync::nextChunk ()
{
    // Terminates the current read and advances the state of this
    // object to expect to access the first subchunk of the next
    // chunk

    subchunk_p.incrementChunk ();

    if (moreChunks()){

        readComplete (); // complete any pending read

    }

    return * this;
}

Int
ViReadImplAsync::nRowChunk() const
{
    ThrowIfNoVbaAttached ();

    return visBufferAsync_p->nRowChunk ();
}


Int
ViReadImplAsync::numberSpw()
{
    ThrowIfNoVbaAttached ();

    return visBufferAsync_p->getNSpw ();
}


void
ViReadImplAsync::origin ()
{
    // Terminates the current read and

    readComplete (); // complete any pending read

    subchunk_p.resetSubChunk();

    fillVisBuffer ();

    updateMsd ();
}

void
ViReadImplAsync::originChunks ()
{
    readComplete (); // complete any pending read

    subchunk_p.resetToOrigin ();

    interface_p->requestViReset ();
}

void
ViReadImplAsync::readComplete()
{
    if (visBufferAsync_p != NULL){

        // A buffer in the buffer ring was in use: clean up

        if (! vbaWrapperStack_p.empty()){

            // Break connection between our VisBufferAsync and
            // the shared data

            vbaWrapperStack_p.top()->releaseVba ();
        }

        // Clear the pointer to the shared buffer to indicate
        // internally that the read is complete

        delete visBufferAsync_p;
        visBufferAsync_p = NULL;

        // Inform the buffer ring that the read is complete.

        vlaData_p->readComplete (subchunk_p);
    }
}

void
ViReadImplAsync::updateMsd ()
{
    ThrowIf (visBufferAsync_p == NULL, "No attached VisBufferAsync");

    msd_p.setAntennas (visBufferAsync_p->msColumns().antenna());

    MDirection phaseCenter = visBufferAsync_p -> getPhaseCenter();
    msd_p.setFieldCenter (phaseCenter);
}


const Cube<Double>&
ViReadImplAsync::receptorAngles() const
{
    ThrowIf (visBufferAsync_p == NULL, "No attached VisBufferAsync");

    return visBufferAsync_p -> getReceptorAngles ();
}


void
ViReadImplAsync::saveMss (const Block<MeasurementSet> & mss)
{
    measurementSets_p.clear();
    for (uint i = 0; i < mss.size(); i++){
        measurementSets_p.push_back (mss [i]);
    }
}

void
ViReadImplAsync::saveMss (const MeasurementSet & ms)
{
    measurementSets_p.clear();
    measurementSets_p.push_back (ms);
}

VisibilityIteratorReadImpl&
ViReadImplAsync::selectVelocity (Int nChan,
                                 const MVRadialVelocity& vStart,
                                 const MVRadialVelocity& vInc,
                                 MRadialVelocity::Types rvType,
                                 MDoppler::Types dType,
                                 Bool precise)
{
    SelectVelocityModifier * svm = new SelectVelocityModifier (nChan, vStart, vInc, rvType, dType, precise);

    interface_p->addModifier (svm); // ownership is transferred by this call

    originChunks ();

    return * this;
}

VisibilityIteratorReadImpl&
ViReadImplAsync::selectChannel(Int nGroup,
                               Int start,
                               Int width,
                               Int increment,
                               Int spectralWindow)
{
    SelectChannelModifier * scm = new SelectChannelModifier (nGroup, start, width, increment, spectralWindow);

    interface_p->addModifier (scm);
    // ownership is transferred by this call

    return * this;
}

VisibilityIteratorReadImpl&
ViReadImplAsync::selectChannel(const Block< Vector<Int> >& blockNGroup,
                               const Block< Vector<Int> >& blockStart,
                               const Block< Vector<Int> >& blockWidth,
                               const Block< Vector<Int> >& blockIncr,
                               const Block< Vector<Int> >& blockSpw)
{
    SelectChannelModifier * scm = new SelectChannelModifier (blockNGroup, blockStart, blockWidth,
                                                             blockIncr, blockSpw);

    interface_p->addModifier (scm);
    // ownership is transferred by this call

    msChannels_p.nGroups_p = blockNGroup;
    msChannels_p.start_p = blockStart;
    msChannels_p.width_p = blockWidth;
    msChannels_p.inc_p = blockIncr;
    msChannels_p.spw_p = blockSpw;

    return * this;
}

void
ViReadImplAsync::setInterval (Double timeInterval)
{
    SetIntervalModifier * sim = new SetIntervalModifier (timeInterval);

    interface_p->addModifier (sim);
}


void
ViReadImplAsync::setRowBlocking(Int nRow)
{
    SetRowBlockingModifier * srbm = new SetRowBlockingModifier (nRow);

    interface_p->addModifier (srbm);
}

//void
//ViReadImplAsync::startVlat ()
//{
//    vlat_p->startThread ();
//}

namespace asyncio {

PrefetchColumns
PrefetchColumns::operator+ (const PrefetchColumns & other)
{
    // Form and return the union

            PrefetchColumns result;
    result.insert (begin(), end());

    for (const_iterator o = other.begin(); o != other.end(); o++){

        result.insert (* o);
    }

    return result;
}

} // end namespace asyncio

//void
//ViReadImplAsync::setPrefetchColumns (const PrefetchColumns & columns) const
//{
//
//}


//VisibilityIteratorReadImpl&
//ViReadImplAsync::selectChannel(Int nGroup=1,
//                                        Int start=0,
//                                        Int width=0,
//                                        Int increment=1,
//                                        Int spectralWindow=-1)
//{
//    VisibilityIteratorReadImpl & rovi = VisibilityIterator::selectChannel (nGroup, start, width, increment, spectralWindow);
//
//    ThrowIf (visBuffer_p == NULL, "No VisBufferAsync attached");
//
//    visBuffer_p->setBlockSpw (blockSpw_p);
//
//    return rovi;
//}

//VisibilityIteratorReadImpl&
//ViReadImplAsync::selectChannel(Block< Vector<Int> >& blockNGroup,
//                                        Block< Vector<Int> >& blockStart,
//                                        Block< Vector<Int> >& blockWidth,
//                                        Block< Vector<Int> >& blockIncr,
//                                        Block< Vector<Int> >& blockSpw)
//{
//    VisibilityIteratorReadImpl & rovi = VisibilityIterator::selectChannel (blockNGroup, blockStart, blockWidth, blockIncr, blockSpw);
//
//    ThrowIf (visBuffer_p == NULL, "No VisBufferAsync attached");
//
//    visBuffer_p->setBlockSpw (blockSpw_p);
//
//    return rovi;
//}

ViWriteImplAsync::ViWriteImplAsync (VisibilityIterator * vi)
: VisibilityIteratorWriteImpl (vi)
{
}

//ViWriteImplAsync::ViWriteImplAsync (VisibilityIterator * vi)
// : VisibilityIteratorWriteImpl (vi)
//{
//}

ViWriteImplAsync::ViWriteImplAsync (const PrefetchColumns & /*prefetchColumns*/,
                                    const VisibilityIteratorWriteImpl & /*other*/,
                                    VisibilityIterator * vi)
: VisibilityIteratorWriteImpl (vi)
{}


ViWriteImplAsync::~ViWriteImplAsync ()
{}

VisibilityIteratorWriteImpl *
ViWriteImplAsync::clone () const
{
    Throw ("ViWriteImplAsync: cloning not permitted!!!");
}

ViReadImplAsync *
ViWriteImplAsync::getReadImpl()
{
    return dynamic_cast <ViReadImplAsync *> (VisibilityIteratorWriteImpl::getReadImpl());
}

void
ViWriteImplAsync::putModel(const RecordInterface& rec, Bool iscomponentlist, Bool incremental)
{
//visBufferAsync_p;
    //Throw ("ViWriteImplAsync::putModel not implemented!!!");

  Vector<Int> fields = getReadImpl()->msColumns().fieldId().getColumn();
  const Int option = Sort::HeapSort | Sort::NoDuplicates;
  const Sort::Order order = Sort::Ascending;

  Int nfields = GenSort<Int>::sort (fields, order, option);

  // Make sure  we have the right size

  fields.resize(nfields, true);
  Int msid = getReadImpl()->msId();

  Vector<Int> spws =  getReadImpl()->msChannels_p.spw_p[msid];
  Vector<Int> starts = getReadImpl()->msChannels_p.start_p[msid];
  Vector<Int> nchan = getReadImpl()->msChannels_p.width_p[msid];
  Vector<Int> incr = getReadImpl()->msChannels_p.inc_p[msid];

  CountedPtr<VisModelDataI> visModelData = VisModelDataI::create();

  visModelData->putModelI (getReadImpl()->getMs (), rec, fields, spws, starts, nchan, incr,
                           iscomponentlist, incremental);

}



void
ViWriteImplAsync::setFlag(const Matrix<Bool>& flag)
{
    AsynchronousInterface * interface = getReadImpl()->interface_p;
    WriteQueue & writeQueue = interface->getWriteQueue();
    SubChunkPair subchunk = getReadImpl()->getSubchunkId ();

    writeQueue.enqueue (createWriteData (subchunk, flag, & VisibilityIterator::setFlag));
}

void
ViWriteImplAsync::setFlag(const Cube<Bool>& flag)
{
    AsynchronousInterface * interface = getReadImpl()->interface_p;
    WriteQueue & writeQueue = interface->getWriteQueue();

    writeQueue.enqueue (createWriteData (getReadImpl()->getSubchunkId (),
                                         flag,
                                         & VisibilityIterator::setFlag));
}

void
ViWriteImplAsync::setFlagCategory (const Array<Bool> & flagCategory)
{
    AsynchronousInterface * interface = getReadImpl()->interface_p;
    WriteQueue & writeQueue = interface->getWriteQueue();

    writeQueue.enqueue (createWriteData (getReadImpl()->getSubchunkId (),
                                         flagCategory,
                                         & VisibilityIterator::setFlagCategory));
}


void
ViWriteImplAsync::setFlagRow(const Vector<Bool>& rowflags)
{
    AsynchronousInterface * interface = getReadImpl()->interface_p;
    WriteQueue & writeQueue = interface->getWriteQueue();

    writeQueue.enqueue (createWriteData (getReadImpl()->getSubchunkId (),
                                         rowflags,
                                         & VisibilityIterator::setFlagRow));
}

void
ViWriteImplAsync::setVis(const Matrix<CStokesVector>& vis, DataColumn whichOne)
{
    AsynchronousInterface * interface = getReadImpl()->interface_p;
    WriteQueue & writeQueue = interface->getWriteQueue();

    writeQueue.enqueue (createWriteData (getReadImpl()->getSubchunkId (),
                                         vis,
                                         whichOne,
                                         & VisibilityIterator::setVis));
}

void
ViWriteImplAsync::setVis(const Cube<Complex>& vis, DataColumn whichOne)
{
    AsynchronousInterface * interface = getReadImpl()->interface_p;
    WriteQueue & writeQueue = interface->getWriteQueue();

    writeQueue.enqueue (createWriteData (getReadImpl()->getSubchunkId (),
                                         vis,
                                         whichOne,
                                         & VisibilityIterator::setVis));
}

void
ViWriteImplAsync::setVisAndFlag(const Cube<Complex>& vis, const Cube<Bool>& flag,
                                DataColumn whichOne)
{
    setVis (vis, whichOne);
    setFlag (flag);
}

void
ViWriteImplAsync::setWeight(const Vector<Float>& wt)
{
    AsynchronousInterface * interface = getReadImpl()->interface_p;
    WriteQueue & writeQueue = interface->getWriteQueue();
    SubChunkPair subchunk = getReadImpl()->getSubchunkId ();

    writeQueue.enqueue (createWriteData (subchunk, wt, & VisibilityIterator::setWeight));
}

void
ViWriteImplAsync::setWeightMat(const Matrix<Float>& wtmat)
{
    AsynchronousInterface * interface = getReadImpl()->interface_p;
    WriteQueue & writeQueue = interface->getWriteQueue();

    writeQueue.enqueue (createWriteData (getReadImpl()->getSubchunkId (),
                                         wtmat,
                                         & VisibilityIterator::setWeightMat));
}

void
ViWriteImplAsync::setWeightSpectrum(const Cube<Float>& wtsp)
{
    AsynchronousInterface * interface = getReadImpl()->interface_p;
    WriteQueue & writeQueue = interface->getWriteQueue();

    writeQueue.enqueue (createWriteData (getReadImpl()->getSubchunkId (),
                                         wtsp,
                                         & VisibilityIterator::setWeightSpectrum));
}

void
ViWriteImplAsync::setSigma(const Vector<Float>& sig)
{
    AsynchronousInterface * interface = getReadImpl()->interface_p;
    WriteQueue & writeQueue = interface->getWriteQueue();

    writeQueue.enqueue (createWriteData (getReadImpl()->getSubchunkId (),
                                         sig,
                                         & VisibilityIterator::setSigma));
}

void
ViWriteImplAsync::setSigmaMat(const Matrix<Float>& sigmat)
{
    AsynchronousInterface * interface = getReadImpl()->interface_p;
    WriteQueue & writeQueue = interface->getWriteQueue();

    writeQueue.enqueue (createWriteData (getReadImpl()->getSubchunkId (),
                                         sigmat,
                                         & VisibilityIterator::setSigmaMat));
}

using namespace casacore;
} // end namespace casa
