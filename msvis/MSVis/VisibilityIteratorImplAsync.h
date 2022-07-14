/*
 * VisibilityIteratorAsync.h
 *
 *  Created on: Nov 1, 2010
 *      Author: jjacobs
 */

#if ! defined (VisibilityIteratorAsync_H_)
#define VisibilityIteratorAsync_H_

#include <stack>
#include <set>

#include <msvis/MSVis/VisibilityIteratorImpl.h>
#include <msvis/MSVis/AsynchronousInterface.h>
#include <stdcasa/UtilJ.h>

#define NotImplementedROVIA throw utilj::AipsErrorTrace (casacore::String ("Method not legal in ROVIA: ") + __PRETTY_FUNCTION__, __FILE__, __LINE__)
#define NotPrefetched throw utilj::AipsErrorTrace (casacore::String ("Column not prefetched for async I/O: ") + __PRETTY_FUNCTION__, __FILE__, __LINE__)

namespace casa {


class VisBufferAsync;
class VisBufferAsyncWrapper;

namespace asyncio {

class AsynchronousInterface;
class VlaDatum;
class VlaData;
class VLAT;

} // end namespace asyncio

class ViReadImplAsync : public VisibilityIteratorReadImpl {
    // This needs to be changed back to ROVisibilityIterator at some point
    // after feasibility testing

    friend class Rovia_Test;
    friend class ROVisIterator;
    friend class VisIterator;
    friend class ViWriteImplAsync;

public:

    typedef casa::asyncio::PrefetchColumns PrefetchColumns;

//    static VisibilityIteratorReadImpl *
//    create (const ROVisibilityIterator &,
//            const PrefetchColumns & prefetchColumns,
//            casacore::Int nReadAheadBuffers = -1);

    ViReadImplAsync (const casacore::Block<casacore::MeasurementSet> & mss,
                     const PrefetchColumns & prefetchColumns,
                     const casacore::Block<casacore::Int> & sortColumns,
                     const casacore::Bool addDefaultSortCols,
                     casacore::Double timeInterval,
                     casacore::Bool writable);

    ViReadImplAsync (const PrefetchColumns & prefetchColumns,
                     const VisibilityIteratorReadImpl & other,
                     casacore::Bool writable);


    ~ViReadImplAsync ();

    VisibilityIteratorReadImpl * clone () const;

    void attachVisBuffer (VisBuffer & vb);
    void detachVisBuffer (VisBuffer & vb);
    void getChannelSelection(casacore::Block< casacore::Vector<casacore::Int> >& ,
                             casacore::Block< casacore::Vector<casacore::Int> >& ,
                             casacore::Block< casacore::Vector<casacore::Int> >& ,
                             casacore::Block< casacore::Vector<casacore::Int> >& ,
                             casacore::Block< casacore::Vector<casacore::Int> >& );
    PrefetchColumns getPrefetchColumns () const;
    VisBuffer * getVisBuffer ();


//    casacore::Int getDataDescriptionId () const;
//    const casacore::MeasurementSet & getMeasurementSet();
//    const casacore::Int getMeasurementSetId ();
//    casacore::Int getNAntennas () const;
    casacore::MEpoch getEpoch () const;
//    casacore::Vector<casacore::Float> getReceptor0Angle ();

    /////void linkWithRovi (VisibilityIteratorReadImpl * rovi);

    bool more () const;
    bool moreChunks () const;
    ViReadImplAsync & nextChunk ();
    void origin ();
    void originChunks ();
    virtual void advance ();

    void setPrefetchColumns (const PrefetchColumns & prefetchColumns);

    VisibilityIteratorReadImpl& selectChannel(casacore::Int nGroup=1,
                                        casacore::Int start=0,
                                        casacore::Int width=0,
                                        casacore::Int increment=1,
                                        casacore::Int spectralWindow=-1);

    VisibilityIteratorReadImpl& selectChannel(const casacore::Block< casacore::Vector<casacore::Int> >& blockNGroup,
                                        const casacore::Block< casacore::Vector<casacore::Int> >& blockStart,
                                        const casacore::Block< casacore::Vector<casacore::Int> >& blockWidth,
                                        const casacore::Block< casacore::Vector<casacore::Int> >& blockIncr,
                                        const casacore::Block< casacore::Vector<casacore::Int> >& blockSpw);


    VisibilityIteratorReadImpl&
    selectVelocity(casacore::Int ,
                   const casacore::MVRadialVelocity& ,
                   const casacore::MVRadialVelocity& ,
                   casacore::MRadialVelocity::Types ,
                   casacore::MDoppler::Types , casacore::Bool );
    void setInterval(casacore::Double timeInterval);
    void setRowBlocking(casacore::Int nRow);

    // These functions generate a list of the IDs (from PrefetchColumnIDs enum)
    // of the columns to prefetch.  For the variable arg calls, terminate with a
    // -1.

    static int getDefaultNBuffers ();

    // The functions below make no sense (at first glance) for asynchronous operation and are implemented
    // to throw an casacore::AipsError if called.  ROVIA is designed to have all the data accessed through the
    // associated VisBufferAsync.  Any method which tries to access data through the ROVIA makes no
    // sense.  Also anything that tries to change the characteristics of underlying ROVI is not currently
    // permitted.  During integration some methods may be found to be more important to the use of ROVIA
    // and a way may be found to implement them in ROVIA.

    casacore::Bool allBeamOffsetsZero() const; // { NotPrefetched; }
    void allSelectedSpectralWindows(casacore::Vector<casacore::Int>& , casacore::Vector<casacore::Int>& ) { NotPrefetched; }
    casacore::Vector<casacore::Int>& antenna1(casacore::Vector<casacore::Int>& ) const { NotPrefetched; }
    casacore::Vector<casacore::Int>& antenna2(casacore::Vector<casacore::Int>& ) const { NotPrefetched; }
    const casacore::Vector<casacore::String>& antennaMounts() const; // { NotPrefetched; }
    casacore::Vector<casacore::MDirection> azel(casacore::Double ) const { NotImplementedROVIA; }
    casacore::MDirection azel0(casacore::Double ) const { NotImplementedROVIA; }
    casacore::Vector<casacore::Int>& channel(casacore::Vector<casacore::Int>& ) const { NotPrefetched; }
    casacore::Int channelGroupSize() const { NotImplementedROVIA; }
    casacore::Int channelIndex() const { NotImplementedROVIA; }
    casacore::Vector<casacore::SquareMatrix<casacore::Complex,2> >& CJones(casacore::Vector<casacore::SquareMatrix<casacore::Complex,2> >& ) const { NotPrefetched; }
    casacore::Vector<casacore::Int>& corrType(casacore::Vector<casacore::Int>& ) const { NotPrefetched; }
    casacore::Int dataDescriptionId() const { NotPrefetched; }
    casacore::Bool existsWeightSpectrum() const { NotImplementedROVIA; }
    casacore::Vector<casacore::Double>& exposure(casacore::Vector<casacore::Double>& /*expo*/) const { NotPrefetched; }
    casacore::Vector<casacore::Int>& feed1(casacore::Vector<casacore::Int>& ) const { NotPrefetched; }
    casacore::Vector<casacore::Int>& feed2(casacore::Vector<casacore::Int>& ) const { NotPrefetched; }
    //casacore::Vector<casacore::Float> feed_pa(casacore::Double ) const { NotImplementedROVIA; }
    casacore::Int fieldId() const { NotPrefetched; }
    casacore::Array<casacore::Bool>& flagCategory(casacore::Array<casacore::Bool>& /*flagCategories*/) const { NotPrefetched; }
    casacore::Cube<casacore::Float>& floatData(casacore::Cube<casacore::Float>& /*fcube*/) const { NotPrefetched; }
    void getFloatDataColumn (casacore::Cube<casacore::Float>& /*data*/) const { NotImplementedROVIA; }
    void getFloatDataColumn(const casacore::Slicer& /*slicer*/, casacore::Cube<casacore::Float>& /*data*/) const { NotImplementedROVIA; }
    void getInterpolatedFloatDataFlagWeight() const { NotImplementedROVIA; }
    void getInterpolatedVisFlagWeight(DataColumn /*whichOne*/) const { NotImplementedROVIA; }
    casacore::Int arrayId() const { NotPrefetched; }
    casacore::String fieldName() const { NotImplementedROVIA; }
    casacore::String sourceName() const { NotImplementedROVIA; }
    casacore::Cube<casacore::Bool>& flag(casacore::Cube<casacore::Bool>& ) const { NotPrefetched; }
    casacore::Matrix<casacore::Bool>& flag(casacore::Matrix<casacore::Bool>& ) const { NotPrefetched; }
    casacore::Vector<casacore::Bool>& flagRow(casacore::Vector<casacore::Bool>& ) const { NotPrefetched; }
    casacore::Vector<casacore::Double>& frequency(casacore::Vector<casacore::Double>& ) const { NotPrefetched; }
    const casacore::Cube<casacore::RigidVector<casacore::Double, 2> >& getBeamOffsets() const;// { NotImplementedROVIA; }
    casacore::Int getDataDescriptionId () const { NotPrefetched; }
    casacore::MEpoch getMEpoch () const { NotImplementedROVIA; }
    const casacore::MeasurementSet & getMeasurementSet() const { NotImplementedROVIA; }
    casacore::Int getMeasurementSetId() const { NotImplementedROVIA; }
    casacore::Int getNAntennas () const { NotImplementedROVIA; }
    casacore::Vector<casacore::Float> getReceptor0Angle () { NotImplementedROVIA; }
    casacore::Vector<casacore::uInt> getRowIds () const { NotImplementedROVIA; }
    casacore::Double hourang(casacore::Double ) const { NotImplementedROVIA; }
    casacore::Vector<casacore::Double>& lsrFrequency(casacore::Vector<casacore::Double>& ) const { NotImplementedROVIA; }
    void lsrFrequency(const casacore::Int& , casacore::Vector<casacore::Double>& , casacore::Bool& ) { NotImplementedROVIA; }
    const casacore::MeasurementSet& ms() const  { NotImplementedROVIA; }
    const casacore::MSColumns& msColumns() const;
    casacore::Int msId() const;
    casacore::Int nCorr() const { NotPrefetched; }
    casacore::Int nRow() const { NotPrefetched; }
    casacore::Int nRowChunk() const;
    casacore::Int nSubInterval() const { NotImplementedROVIA; }
    casacore::Bool newArrayId() const { NotImplementedROVIA; }
    casacore::Bool newFieldId() const { NotImplementedROVIA; }
    casacore::Bool newMS() const   { return msIter_p.more();}
    casacore::Bool newSpectralWindow() const { NotImplementedROVIA; }
    casacore::Int numberCoh() { NotPrefetched; }
    casacore::Int numberDDId() { NotPrefetched; }
    casacore::Int numberPol() { NotPrefetched; }
    casacore::Int numberSpw();
    casacore::Vector<casacore::Int>& observationId(casacore::Vector<casacore::Int>& /*obsIDs*/) const { NotPrefetched; }
    casacore::Vector<casacore::Float> parang(casacore::Double ) const { NotImplementedROVIA; }
    const casacore::Float& parang0(casacore::Double ) const { NotImplementedROVIA; }
    const casacore::MDirection& phaseCenter() const { NotPrefetched; }
    casacore::Int polFrame() const  { NotPrefetched; }
    casacore::Vector<casacore::Int>& processorId(casacore::Vector<casacore::Int>& /*procIDs*/) const { NotPrefetched; }
    casacore::Int polarizationId() const { NotPrefetched; }
    const casacore::Cube<casacore::Double>& receptorAngles() const; // { NotImplementedROVIA; }
    casacore::Vector<casacore::rownr_t>& rowIds(casacore::Vector<casacore::rownr_t>& ) const { NotImplementedROVIA; }
    casacore::Vector<casacore::Int>& scan(casacore::Vector<casacore::Int>& ) const { NotPrefetched; }
    casacore::Vector<casacore::Float>& sigma(casacore::Vector<casacore::Float>& ) const { NotPrefetched; }
    casacore::Matrix<casacore::Float>& sigmaMat(casacore::Matrix<casacore::Float>& ) const { NotPrefetched; }
    void slurp() const { NotImplementedROVIA; }
    casacore::Int spectralWindow() const { NotPrefetched; }
    casacore::Vector<casacore::Int>& stateId(casacore::Vector<casacore::Int>& /*stateIds*/) const { NotPrefetched; }
    casacore::Vector<casacore::Double>& time(casacore::Vector<casacore::Double>& ) const { NotPrefetched; }
    casacore::Vector<casacore::Double>& timeCentroid(casacore::Vector<casacore::Double>& /*t*/) const { NotPrefetched; }
    casacore::Vector<casacore::Double>& timeInterval(casacore::Vector<casacore::Double>& ) const { NotPrefetched; }
    casacore::Vector<casacore::RigidVector<casacore::Double,3> >& uvw(casacore::Vector<casacore::RigidVector<casacore::Double,3> >& ) const { NotPrefetched; }
    casacore::Matrix<casacore::Double>& uvwMat(casacore::Matrix<casacore::Double>& ) const { NotPrefetched; }
    VisibilityIteratorReadImpl& velInterpolation(const casacore::String& ) { NotImplementedROVIA; }
    casacore::Cube<casacore::Complex>& visibility(casacore::Cube<casacore::Complex>& , DataColumn ) const { NotPrefetched; }
    casacore::Matrix<CStokesVector>& visibility(casacore::Matrix<CStokesVector>& , DataColumn ) const { NotPrefetched; }
    casacore::IPosition visibilityShape() const { NotImplementedROVIA; }
    casacore::Vector<casacore::Float>& weight(casacore::Vector<casacore::Float>& ) const { NotPrefetched; }
    casacore::Matrix<casacore::Float>& weightMat(casacore::Matrix<casacore::Float>& ) const { NotPrefetched; }
    casacore::Cube<casacore::Float>& weightSpectrum(casacore::Cube<casacore::Float>& ) const { NotPrefetched; }

    static casacore::String prefetchColumnName (casacore::Int id); // for debug only

protected:

    // Use the factory method "create" instead of calling the constructor
    // directly.  This allows disabling the feature.

//    ViReadImplAsync (const casacore::MeasurementSet & ms,
//                     const PrefetchColumns & prefetchColumns,
//                     const casacore::Block<casacore::Int> & sortColumns,
//                     casacore::Double timeInterval=0,
//                     casacore::Int nReadAheadBuffers = 2);
//    ViReadImplAsync (const casacore::MeasurementSet & ms,
//                     const PrefetchColumns & prefetchColumns,
//                     const casacore::Block<casacore::Int> & sortColumns,
//                     const casacore::Bool addDefaultSortCols,
//                     casacore::Double timeInterval=0,
//                     casacore::Int nReadAheadBuffers = 2);
//
//    // Same as previous constructor, but with multiple MSs to iterate over.
//
//    ViReadImplAsync (const casacore::Block<casacore::MeasurementSet> & mss,
//                     const PrefetchColumns & prefetchColumns,
//                     const casacore::Block<casacore::Int> & sortColumns,
//                     casacore::Double timeInterval=0,
//                     casacore::Int nReadAheadBuffers = 2);


    ViReadImplAsync (const ROVisibilityIterator & rovi,
                     const PrefetchColumns & prefetchColumns,
                     casacore::Int nReadAheadBuffers = -1);

    PrefetchColumns augmentPrefetchColumns (const PrefetchColumns & prefetchColumnsBase);
    void construct(const casacore::Block<casacore::MeasurementSet> & mss,
                   const PrefetchColumns & prefetchColumns,
                   const casacore::Block<casacore::Int> & sortColumns,
                   const casacore::Bool addDefaultSortCols,
                   casacore::Double timeInterval,
                   casacore::Bool writable);


    void fillVisBuffer();
    const casacore::MeasurementSet & getMs() const;
    void readComplete ();
    void saveMss (const casacore::Block<casacore::MeasurementSet> & mss);
    void saveMss (const casacore::MeasurementSet & ms);
    //void startVlat ();

private:

    asyncio::AsynchronousInterface * interface_p; // [own]
    casacore::Int                              msId_p;
    PrefetchColumns                  prefetchColumns_p;
    std::stack<VisBufferAsyncWrapper *>   vbaWrapperStack_p;
    VisBufferAsync *                 visBufferAsync_p;
    asyncio::VlaData *               vlaData_p; // [use]
    asyncio::VLAT *                  vlat_p; // [use]

    void dumpPrefetchColumns () const;
    void updateMsd ();

    ViReadImplAsync (const ViReadImplAsync & MSI);
    ViReadImplAsync & operator=(const ViReadImplAsync &MSI);

}; // end class ViReadImplAsync

namespace asyncio {
    class WriteData;
} // end namespace asyncio

class ViWriteImplAsync : public VisibilityIteratorWriteImpl {

public:

    typedef casa::asyncio::PrefetchColumns PrefetchColumns;

    ViWriteImplAsync (VisibilityIterator * vi);
    ViWriteImplAsync (const PrefetchColumns & prefetchColumns,
                      const VisibilityIteratorWriteImpl & other,
                      VisibilityIterator * vi);

    ~ViWriteImplAsync ();

    VisibilityIteratorWriteImpl * clone () const;

    void putModel(const casacore::RecordInterface& rec, casacore::Bool iscomponentlist=true, casacore::Bool incremental=false);


    // Set/modify the flags in the data.
    // This will flag all channels in the original data that contributed to
    // the output channel in the case of channel averaging.
    // All polarizations have the same flag value.
    void setFlag(const casacore::Matrix<casacore::Bool>& flag);
    // Set/modify the flags in the data.
    // This sets the flags as found in the casacore::MS, casacore::Cube(npol,nchan,nrow),
    // where nrow is the number of rows in the current iteration (given by
    // nRow()).
    void setFlag(const casacore::Cube<casacore::Bool>& flag);

    void setFlagCategory (const casacore::Array<casacore::Bool> & flagCategory);

    // Set/modify the flag row column; dimension casacore::Vector(nrow)
    void setFlagRow(const casacore::Vector<casacore::Bool>& rowflags);
    // Set/modify the visibilities.
    // This is possibly only for a 'reference' casacore::MS which has a new DATA column.
    // The first axis of the matrix should equal the selected number of channels
    // in the original MS.
    // If the casacore::MS does not contain all polarizations, only the parallel
    // hand polarizations are used.
    void setVis(const casacore::Matrix<CStokesVector>& vis, DataColumn whichOne);
    // Set/modify the visibilities
    // This sets the data as found in the casacore::MS, casacore::Cube(npol,nchan,nrow).
    void setVis(const casacore::Cube<casacore::Complex>& vis, DataColumn whichOne);
    // Set the visibility and flags, and interpolate from velocities if needed
    void setVisAndFlag(const casacore::Cube<casacore::Complex>& vis, const casacore::Cube<casacore::Bool>& flag,
                       DataColumn whichOne);
    // Set/modify the weights
    void setWeight(const casacore::Vector<casacore::Float>& wt);
    // Set/modify the weightMat
    void setWeightMat(const casacore::Matrix<casacore::Float>& wtmat);
    // Set/modify the weightSpectrum
    void setWeightSpectrum(const casacore::Cube<casacore::Float>& wtsp);
    // Set/modify the Sigma
    void setSigma(const casacore::Vector<casacore::Float>& sig);
    // Set/modify the ncorr x nrow SigmaMat.
    void setSigmaMat(const casacore::Matrix<casacore::Float>& sigmat);

protected:

    ViReadImplAsync * getReadImpl();
    void queueWriteData (const asyncio::WriteData & data);

private:


}; // end class ViWriteImplAsync


typedef ViReadImplAsync ROVIA;

} // end namespace casa

#endif // ViReadImplAsync
