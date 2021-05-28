//# VisBuffer.h: buffer for iterating through casacore::MS in large blocks
//# Copyright (C) 1996,1997,1998,1999,2000,2002,2003
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
//# $Id: VisBuffer.h,v 19.14 2006/02/28 04:48:58 mvoronko Exp $

#ifndef VISBUFFER_ASYNC_WRAPPER_H
#define VISBUFFER_ASYNC_WRAPPER_H

#include <casa/aips.h>
#include <casa/Arrays/Cube.h>
#include <casa/Arrays/Vector.h>
#include <casa/Arrays/Matrix.h>
#include <casa/BasicSL/Complex.h>
#include <measures/Measures/MDirection.h>
#include <measures/Measures/MEpoch.h>
#include <msvis/MSVis/StokesVector.h>
#include <msvis/MSVis/VisBuffer.h>
#include <msvis/MSVis/VisBufferAsync.h>
#include <msvis/MSVis/VisibilityIterator.h>
#include <msvis/MSVis/MSCalEnums.h>

namespace casa { //# NAMESPACE CASA - BEGIN

//#forward

//<summary>VisBuffers encapulsate one chunk of visibility data for processing.</summary>
//
// <use visibility=export>
//
// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//   <li> <linkto class="VisSet">VisSet</linkto>
//   <li> <linkto class="VisibilityIterator">VisibilityIterator</linkto>
// </prerequisite>
//
// <etymology>
// VisBuffer is a buffer for visibility data
// </etymology>
//
//<synopsis>
// This class contains 'one iteration' of the
// <linkto class="VisibilityIterator">VisibilityIterator</linkto>
// It is a modifiable
// buffer of values to which calibration and averaging can be applied.
// This allows processing of the data in larger blocks, avoiding some
// overheads for processing per visibility point or spectrum.
//
// See <linkto module="MeasurementEquations">MeasurementEquations</linkto>
// for more details on how the VisBuffer is to be used.
//</synopsis>

//<todo>
// <li> reconcile vis/visCube usage: visCube, flagCube and weightMatrix
// are currently only correct when this VisBuffer got them from a
// VisIter, operations like -=, freqAverage() are only done for
// visibility() and flag().
//</todo>
class VisBufferAsyncWrapper : public VisBufferAsync {

    friend class ViReadImplAsync;

public:
    // Create empty VisBuffer you can assign to or attach.
    VisBufferAsyncWrapper();
//    // Construct VisBuffer for a particular VisibilityIterator
//    // The buffer will remain synchronized with the iterator.
    VisBufferAsyncWrapper (ROVisibilityIterator & iter);
//
//    // Copy construct, looses synchronization with iterator: only use buffer for
//    // current iteration (or reattach).
    VisBufferAsyncWrapper (const VisBufferAsync & vb);

    // Destructor (detaches from VisIter)
    virtual ~VisBufferAsyncWrapper();

    // Assignment, loses synchronization with iterator: only use buffer for
    // current iteration (or reattach)

    virtual VisBufferAsyncWrapper & operator=(const VisBufferAsyncWrapper & vb);

    // Assignment, optionally without copying the data across; with copy=true
    // this is identical to normal assignment operator

    virtual VisBufferAsyncWrapper & assign(const VisBuffer & vb, casacore::Bool copy = true);

    // subtraction: return the difference of the visibilities, flags of
    // this and other are or-ed. An exception is thrown if the number of
    // rows or channels differs, but no further checks are done.
    VisBufferAsyncWrapper & operator-=(const VisBuffer & vb);

    // Attach to a VisIter. Detaches itself first if already attached
    // to a VisIter. Will remain synchronized with iterator.
    virtual void attachToVisIter(ROVisibilityIterator & iter);
    void attachToVisIterAsync (ROVisibilityIterator & iter);
    virtual VisBuffer * clone ();
    virtual void detachFromVisIter ();

    // Invalidate the cache
    virtual void invalidate();

    // <group>
    // Access functions
    //
    virtual casacore::Int & nCorr();
    casacore::Int nCorr() const;

    casacore::Int & nChannel();
    casacore::Int nChannel() const;

    casacore::Vector<casacore::Int>& channel();
    const casacore::Vector<casacore::Int>& channel() const;

    casacore::Bool newArrayId () const;
    casacore::Bool newFieldId () const;
    casacore::Bool newSpectralWindow () const;

    casacore::Int & nRow();
    casacore::Int nRow() const;

    casacore::Vector<casacore::Int>& antenna1();
    const casacore::Vector<casacore::Int>& antenna1() const;

    casacore::Vector<casacore::Int>& antenna2();
    const casacore::Vector<casacore::Int>& antenna2() const;

    casacore::Vector<casacore::Int>& feed1();
    const casacore::Vector<casacore::Int>& feed1() const;

    casacore::Vector<casacore::Int>& feed2();
    const casacore::Vector<casacore::Int>& feed2() const;

    // feed1_pa() and feed2_pa() return an array of parallactic angles
    // (each corresponds to the first receptor of the feed) one for each
    // row in the current buffer. In contrast, feed_pa() calculates
    // the angles for each antenna. These methods are implemented for
    // VisBuffer only to benefit from caching of the feed and antenna IDs.
    casacore::Vector<casacore::Float>& feed1_pa();
    const casacore::Vector<casacore::Float>& feed1_pa() const;

    casacore::Vector<casacore::Float>& feed2_pa();
    const casacore::Vector<casacore::Float>& feed2_pa() const;

    casacore::Vector<casacore::SquareMatrix<casacore::Complex, 2> >& CJones();
    const casacore::Vector<casacore::SquareMatrix<casacore::Complex, 2> >& CJones() const;

    // Note that feed_pa is a function instead of a cached value
    virtual casacore::Vector<casacore::Float> feed_pa(casacore::Double time) const;

    // direction1() and direction2() return arrays of directions where
    // the first and the second antenna/feed are pointed to. One value for
    // each row in the current buffer.
    casacore::Vector<casacore::MDirection>& direction1();
    const casacore::Vector<casacore::MDirection>& direction1()  const;

    casacore::Vector<casacore::MDirection>& direction2();
    const casacore::Vector<casacore::MDirection>& direction2()  const;


    // NOMINAL parallactic angle (feed p.a. offset NOT included)
    virtual casacore::Float parang0(casacore::Double time) const;
    virtual casacore::Vector<casacore::Float> parang(casacore::Double time) const;

    // Note that azel is a function instead of a cached value
    virtual casacore::MDirection azel0(casacore::Double time) const;
    casacore::Vector<casacore::Double>& azel0Vec(casacore::Double time, casacore::Vector<casacore::Double>& azelVec) const;
    virtual casacore::Vector<casacore::MDirection> azel(casacore::Double time) const;
    casacore::Matrix<casacore::Double>& azelMat(casacore::Double time, casacore::Matrix<casacore::Double>& azelMat) const;

    // Hour angle for specified time
    virtual casacore::Double hourang(casacore::Double time) const;

    casacore::Int fieldId() const;

    casacore::Int arrayId() const;

    casacore::Int polarizationId() const;

    // Return flag for each channel & row
    casacore::Matrix<casacore::Bool>& flag();
    const casacore::Matrix<casacore::Bool>& flag() const;

    // Return flag for each polarization, channel and row
    casacore::Cube<casacore::Bool>& flagCube();
    const casacore::Cube<casacore::Bool>& flagCube() const;

    casacore::Vector<casacore::Bool>& flagRow();
    const casacore::Vector<casacore::Bool>& flagRow() const;

    // Return flags for each polarization, channel, category, and row.
    casacore::Array<casacore::Bool>& flagCategory();
    const casacore::Array<casacore::Bool>& flagCategory() const;

    casacore::Vector<casacore::Int>& scan();
    const casacore::Vector<casacore::Int>& scan() const;

    // scalar version for convenience, when scan known constant for
    // entire iteration/buffer.
    casacore::Int scan0();

    casacore::Vector<casacore::Int>& processorId();
    const casacore::Vector<casacore::Int>& processorId() const;

    casacore::Vector<casacore::Int>& observationId();
    const casacore::Vector<casacore::Int>& observationId() const;

    casacore::Vector<casacore::Int>& stateId();
    const casacore::Vector<casacore::Int>& stateId() const;

    // Gets SPECTRAL_WINDOW/CHAN_FREQ (in Hz, acc. to the casacore::MS def'n v.2).
    casacore::Vector<casacore::Double>& frequency();
    const casacore::Vector<casacore::Double>& frequency() const;

//    casacore::Vector<casacore::Double>& lsrFrequency();
//    const casacore::Vector<casacore::Double>& lsrFrequency() const;


    //the following method is to convert the observed frequencies
    // This conversion may not be accurate for some frame
    // conversion like topo to lsr except if the spw is in the actual buffer

    virtual void lsrFrequency(const casacore::Int & spw, casacore::Vector<casacore::Double>& freq, casacore::Bool & convert) const;

    virtual casacore::Int numberCoh () const;

    casacore::MDirection & phaseCenter();
    casacore::MDirection phaseCenter() const;

    casacore::Int polFrame() const;

    casacore::Vector<casacore::Int>& corrType();
    const casacore::Vector<casacore::Int>& corrType() const;

    casacore::Vector<casacore::Float>& sigma();
    const casacore::Vector<casacore::Float>& sigma() const;

    casacore::Matrix<casacore::Float>& sigmaMat();
    const casacore::Matrix<casacore::Float>& sigmaMat() const;

    casacore::Int & spectralWindow();
    casacore::Int spectralWindow() const;
    virtual casacore::Int dataDescriptionId() const;
    casacore::Vector<casacore::Double>& time();
    const casacore::Vector<casacore::Double>& time() const;

    casacore::Vector<casacore::Double>& timeCentroid();
    const casacore::Vector<casacore::Double>& timeCentroid() const;

    casacore::Vector<casacore::Double>& timeInterval();
    const casacore::Vector<casacore::Double>& timeInterval() const;

    casacore::Vector<casacore::Double>& exposure();
    const casacore::Vector<casacore::Double>& exposure() const;

    casacore::Vector<casacore::RigidVector<casacore::Double, 3> >& uvw();
    const casacore::Vector<casacore::RigidVector<casacore::Double, 3> >& uvw() const;

    casacore::Matrix<casacore::Double>& uvwMat();
    const casacore::Matrix<casacore::Double>& uvwMat() const;

    casacore::Matrix<CStokesVector>& visibility();
    const casacore::Matrix<CStokesVector>& visibility() const;

    casacore::Matrix<CStokesVector>& modelVisibility();
    const casacore::Matrix<CStokesVector>& modelVisibility() const;

    casacore::Matrix<CStokesVector>& correctedVisibility();
    const casacore::Matrix<CStokesVector>& correctedVisibility() const;

    casacore::Cube<casacore::Complex>& visCube();
    const casacore::Cube<casacore::Complex>& visCube() const;

    casacore::Cube<casacore::Complex>& modelVisCube();

    casacore::Cube<casacore::Complex>& modelVisCube(const casacore::Bool & matchVisCubeShape);

    const casacore::Cube<casacore::Complex>& modelVisCube() const;

    casacore::Cube<casacore::Complex>& correctedVisCube();
    const casacore::Cube<casacore::Complex>& correctedVisCube() const;

    // Return visCube(), modelVisCube(), or correctedVisCube(),
    // according to whichcol.  They throw an exception if whichcol is
    // unsupported, but note the encouraged default.
    // TODO: Optionally return DATA if whichcol is unavailable.
    casacore::Cube<casacore::Complex>& dataCube(const casacore::MS::PredefinedColumns whichcol=casacore::MS::DATA);
    const casacore::Cube<casacore::Complex>& dataCube(const casacore::MS::PredefinedColumns
                                  whichcol=casacore::MS::DATA) const;

    casacore::Cube<casacore::Float>& floatDataCube();
    const casacore::Cube<casacore::Float>& floatDataCube() const;

    // Returns the weights for each row averaged over the parallel hand correlations.
    casacore::Vector<casacore::Float>& weight();
    const casacore::Vector<casacore::Float>& weight() const;

    // Returns the nPol_p x curNumRow_p weight matrix.
    casacore::Matrix<casacore::Float>& weightMat();
    const casacore::Matrix<casacore::Float>& weightMat() const;

    // Is a valid WEIGHT_SPECTRUM available?
    casacore::Bool existsWeightSpectrum() const;

    casacore::Cube<casacore::Float>& weightSpectrum();
    const casacore::Cube<casacore::Float>& weightSpectrum() const;

    const casacore::Matrix<casacore::Float>& imagingWeight() const;
    casacore::Matrix<casacore::Float> & imagingWeight ();

    casacore::Cube<casacore::Float>& weightCube();
    //</group>

    //<group>
    // Utility functions to provide coordinate or column ranges of the
    // data in the VisBuffer. Flagging is applied before computing the ranges.
    //
    // Generic accessor to column ranges of integer type, as specified by
    // enumerations defined in class MSCalEnums. Throws an exception
    // if the enum is not for a recognized integer column.
    casacore::Vector<casacore::Int> vecIntRange(const MSCalEnums::colDef & calEnum) const;

    // Antenna id. range (includes both ANTENNA1 and ANTENNA2 columns)
    casacore::Vector<casacore::Int> antIdRange() const;

    // casacore::Time range
    casacore::Bool timeRange(casacore::MEpoch & rTime, casacore::MVEpoch & rTimeEP, casacore::MVEpoch & rInterval) const;

    // Return the row Ids from the original ms. If the ms used is a subset of
    // another ms then rowIds() return the row ids of the original ms.
    virtual casacore::Vector<casacore::rownr_t>& rowIds();

    virtual const casacore::Vector<casacore::rownr_t>& rowIds() const;;

    //</group>

    // Frequency average the buffer (visibility() column only)
    void freqAverage();

    // Frequency average the buffer (visCube and [if present] modelVisCube)
    void freqAveCubes();

    // Average channel axis according to chanavebounds, for whichever of DATA,
    // MODEL_DATA, CORRECTED_DATA, FLOAT_DATA, FLAG, and WEIGHT_SPECTRUM are
    // present.  It will only treat the first 5 as present if they have already
    // been loaded into the buffer!
    void channelAve(const casacore::Matrix<casacore::Int>& chanavebounds);

    // Average channel axis by factor.
    template<class T> void chanAveVisCube(casacore::Cube<T>& data, casacore::Int nChanOut);

    // Accumulate channel axis by factor, without applying WEIGHT_SPECTRUM even
    // if it is present.
    // It is primarily intended for averaging WEIGHT_SPECTRUM itself.
    template<class T> void chanAccCube(casacore::Cube<T>& data, casacore::Int nChanOut);

    // This defaults to no conceptual side effects, but usually it is more
    // efficient to let it leave weightSpectrum() in a channel averaged state.
    // restoreWeightSpectrum has no effect if !existsWeightSpectrum().
    void chanAveFlagCube(casacore::Cube<casacore::Bool>& flagcube, const casacore::Int nChanOut,
                         const casacore::Bool restoreWeightSpectrum = true);

    // Form casacore::Stokes parameters from correlations
    //  (these are preliminary versions)
    void formStokes();
    void formStokesWeightandFlag();
    void formStokes(casacore::Cube<casacore::Complex>& vis);
    void formStokes(casacore::Cube<casacore::Float>& fcube);    // Will throw up if asked to do all 4.

    // casacore::Sort/unsort the correlations, if necessary
    //  (Rudimentary handling of non-canonically sorted correlations--use with care!)
    void sortCorr();
    void unSortCorr();

    // Normalize the visCube by the modelVisCube
    //   (and optionally also divide visCube_p by its normalized amp)
    void normalize(const casacore::Bool & phaseOnly = false);

    // Fill weightMat according to sigma column
    void resetWeightMat();

    // Rotate visibility phase for phase center offsets
    void phaseCenterShift(casacore::Double dx, casacore::Double dy);

    // Update coordinate info - useful for copied VisBuffers that need
    // to retain some state for later reference.
    // Presently this fills antenna, array, field and spectralWindow ids, time,
    // frequency and number of rows. Add more as needed.
    virtual void updateCoordInfo(const VisBuffer * vb = NULL);

    // Set the visibility to a constant, note that this only changes the buffer,
    // no values are written back to tables from here.
    virtual void setVisCube(casacore::Complex c);
    virtual void setModelVisCube(casacore::Complex c);
    virtual void setCorrectedVisCube(casacore::Complex c);

    // Set the visibility, note that this only changes the buffer,
    // no values are written back to tables from here.
    void setVisCube(const casacore::Cube<casacore::Complex>& vis);
    void setModelVisCube(const casacore::Cube<casacore::Complex>& vis);
    void setCorrectedVisCube(const casacore::Cube<casacore::Complex>& vis);

    // Like the above, but for FLOAT_DATA, keeping it as real floats.
    void setFloatDataCube(const casacore::Cube<casacore::Float>& fcube);

    // Set model according to a casacore::Stokes vector
    void setModelVisCube(const casacore::Vector<casacore::Float>& stokes);

    // Reference external model visibilities
    void refModelVis(const casacore::Matrix<CStokesVector>& mvis);

    // Remove scratch cols data from vb
    void removeScratchCols();

    // Access the current casacore::MSColumns object via VisIter
    virtual const casacore::MSColumns & msColumns() const;

    virtual  casacore::Int numberAnt () const;



    // Get all selected spectral windows not just the one in the actual buffer
    virtual void allSelectedSpectralWindows(casacore::Vector<casacore::Int>& spws, casacore::Vector<casacore::Int>& nvischan);

    void allSelectedSpectralWindows(casacore::Vector<casacore::Int>& spws, casacore::Vector<casacore::Int>& nvischan) const;

    // Return the actual msid, useful if using multiple ms to monitor which
    // ms in the  list is being dealt with
    virtual casacore::Int msId() const;

    //checked if the ms has changed since the last chunk processed
    virtual casacore::Bool newMS() const;

    void wrap (VisBufferAsync * vba);

protected:

    virtual casacore::Bool checkMSId();
    virtual void checkVisIter (const char * func, const char * file, int line) const;
    void copyCache (const VisBuffer & other, casacore::Bool force);
    const VisImagingWeight & getImagingWeightGenerator () const;
    casacore::Int getOldMsId () const;
    ROVisibilityIterator * getVisibilityIterator () const;
    VisBufferAsync * releaseVba ();

    // Create cache status accessors which relay the request to the wrapped
    // VBA.

#undef CacheStatus
#define CacheStatus(item)\
bool item ## OK () const\
{\
    if (wrappedVba_p == NULL){\
        throw casacore::AipsError ("VisBufferAsyncWrapper: No attached VBA", __FILE__, __LINE__);\
    }\
    return wrappedVba_p->item ## OK_p;\
}

    CacheStatus (antenna1);
    CacheStatus (antenna2);
    CacheStatus (arrayId);
    CacheStatus (channel);
    CacheStatus (cjones);
    CacheStatus (correctedVisCube);
    CacheStatus (correctedVisibility);
    CacheStatus (corrType);
    CacheStatus (dataDescriptionId);
    CacheStatus (direction1);
    CacheStatus (direction2);
    CacheStatus (exposure);
    CacheStatus (feed1_pa);
    CacheStatus (feed1);
    CacheStatus (feed2_pa);
    CacheStatus (feed2);
    CacheStatus (fieldId);
    CacheStatus (flagCategory);
    CacheStatus (flagCube);
    CacheStatus (flag);
    CacheStatus (flagRow);
    CacheStatus (floatDataCube);
    CacheStatus (frequency);
    CacheStatus (imagingWeight);
    CacheStatus (modelVisCube);
    CacheStatus (modelVisibility);
    CacheStatus (ms);
    CacheStatus (nChannel);
    CacheStatus (nCorr);
    CacheStatus (nRow);
    CacheStatus (observationId);
    CacheStatus (phaseCenter);
    CacheStatus (polFrame);
    CacheStatus (processorId);
    CacheStatus (rowIds);
    CacheStatus (scan);
    CacheStatus (sigmaMat);
    CacheStatus (sigma);
    CacheStatus (spectralWindow);
    CacheStatus (stateId);
    CacheStatus (timeCentroid);
    CacheStatus (timeInterval);
    CacheStatus (time);
    CacheStatus (uvwMat);
    CacheStatus (uvw);
    CacheStatus (visCube);
    CacheStatus (visibility);
    CacheStatus (weightMat);
    CacheStatus (weight);
    CacheStatus (weightSpectrum);

private:

    VisBufferAsync * wrappedVba_p;
    ROVisibilityIterator * wrappedVisIterAsync_p;

    // validate the cache
    void validate();
    void attachWrappedVb (ROVisibilityIterator * iter);

    void setAllCacheStatuses (bool status);


    casacore::Bool nonCanonCorr(); // Are correlations in non-canonical order?

    // Filter index arrays for unique elements
    casacore::Vector<casacore::Int> unique(const casacore::Vector<casacore::Int>& indices) const;

    // +-------------------------------------------------+
    // | Cache Declarations (fillers, statuses and data) |
    // +-------------------------------------------------+
    //
    // Preferred naming convention:
    //
    // T thing () --- returns the value of thing (public method)
    // T fillThing () --- fills thing cache variable and sets thingOK_p to true (private)
    // casacore::Bool thingOK_p --- true if the value of thing is currently cached  (private)
    // T thing_p --- holds cached value of thing (if thingOK_p is true)  (private)
    //
    // Example: casacore::Vector<casacore::Int>& feed1(); casacore::Vector<casacore::Int>& fillFeed1();
    //          casacore::Bool feed1OK_P; casacore::Vector<casacore::Int> feed1_p;

    // Cache filling routines in alphabetical order
    //
    // The first line in a fill routine should be the macro CheckVisIter.  This
    // will cause an casacore::AipsError to be throw when there is no visibilty iterator
    // associated with the VisBuffer.  This is especially important because the
    // fillers may be called on a VisBufferAsync in two contexts: filling
    // where there will be a VI attached and in normal use where there is no VI
    // attached.  The filler must also set the cache status variable to true during
    // its execution.

    casacore::Vector<casacore::Int>& fillAnt1();
    casacore::Vector<casacore::Int>& fillAnt2();
    casacore::Int & fillArrayId();
    //casacore::Matrix<casacore::Int>& fillChanAveBounds();
    casacore::Vector<casacore::Int>& fillChannel();
    casacore::Vector<casacore::SquareMatrix<casacore::Complex, 2> >& fillCjones();
    casacore::Vector<casacore::Int>& fillCorrType();
    virtual casacore::Vector<casacore::MDirection>& fillDirection1();
    virtual casacore::Vector<casacore::MDirection>& fillDirection2();
    casacore::Vector<casacore::Double>& fillExposure();
    casacore::Vector<casacore::Int>& fillFeed1();
    casacore::Vector<casacore::Float>& fillFeed1_pa();
    casacore::Vector<casacore::Int>& fillFeed2();
    casacore::Vector<casacore::Float>& fillFeed2_pa();
    casacore::Int & fillFieldId();
    casacore::Matrix<casacore::Bool>& fillFlag();
    casacore::Array<casacore::Bool>& fillFlagCategory();
    casacore::Cube<casacore::Bool>& fillFlagCube();
    casacore::Vector<casacore::Bool> & fillFlagRow();
    casacore::Cube<casacore::Float>& fillFloatDataCube();
    casacore::Vector<casacore::Double>& fillFreq();         // Puts SPECTRAL_WINDOW/CHAN_FREQ in frequency_p.
    //casacore::Matrix<casacore::Float>& fillImagingWeight();
    //casacore::Vector<casacore::Double>& fillLSRFreq();
    casacore::Int & fillnChannel();
    casacore::Int & fillnCorr();
    casacore::Int & fillnRow();
    casacore::Vector<casacore::Int> & fillObservationId();
    virtual casacore::MDirection & fillPhaseCenter();
    casacore::Int & fillPolFrame();
    casacore::Vector<casacore::Int> & fillProcessorId();
    casacore::Vector<casacore::Int> & fillScan();
    casacore::Vector<casacore::Float>& fillSigma();
    casacore::Matrix<casacore::Float>& fillSigmaMat();
    casacore::Int & fillSpW();
    casacore::Vector<casacore::Int> & fillStateId();
    casacore::Vector<casacore::Double>& fillTime();
    casacore::Vector<casacore::Double>& fillTimeCentroid();
    casacore::Vector<casacore::Double>& fillTimeInterval();
    casacore::Vector<casacore::RigidVector<casacore::Double, 3> >& filluvw();
    casacore::Matrix<casacore::Double>& filluvwMat();
    casacore::Matrix<CStokesVector>& fillVis(VisibilityIterator::DataColumn whichOne);
    casacore::Cube<casacore::Complex>& fillVisCube(VisibilityIterator::DataColumn whichOne);
    casacore::Vector<casacore::Float>& fillWeight();
    casacore::Matrix<casacore::Float>& fillWeightMat();
    casacore::Cube<casacore::Float>& fillWeightSpectrum();

};

} //# NAMESPACE CASA - END

#endif
