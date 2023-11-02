//# VisBuffer.h: buffer for iterating through casacore::MS in large blocks
//# Copyright (C) 1996-2014
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

#ifndef MSVIS_VISBUFFER_H
#define MSVIS_VISBUFFER_H

#include <casacore/casa/aips.h>
#include <casacore/casa/Arrays/Cube.h>
#include <casacore/casa/Arrays/Vector.h>
#include <casacore/casa/Arrays/Matrix.h>
#include <casacore/casa/BasicSL/Complex.h>
#include <casacore/measures/Measures/MDirection.h>
#include <casacore/measures/Measures/MEpoch.h>
#include <msvis/MSVis/VisModelDataI.h>
#include <msvis/MSVis/StokesVector.h>
#include <msvis/MSVis/VisibilityIterator.h>
#include <msvis/MSVis/VisBufferComponents.h>
#include <msvis/MSVis/MSCalEnums.h>
#include <set>

namespace casa { //# NAMESPACE CASA - BEGIN


//#forward
  class SIMapperCollection;

// <summary>
// VbDirtyComponents allows marking portions of a VisBuffer as
// modified (aka dirty).  This feature is needed for the Visibility
// Processing Framework (VPF) which allows a sequence of data processing
// nodes to work as a bucket brigade operating sequentially on a
// VisBuffer.  A downstream output node needs to know what data,
// if any, needs to be written out.
//
// <prerequisite>
//   #<li><linkto class="VisBuffer">VisBuffer</linkto>
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
class VbDirtyComponents {

public:

    typedef std::set<VisBufferComponents::EnumType> Set;
    typedef Set::const_iterator const_iterator;

    VbDirtyComponents operator+ (const VbDirtyComponents & other) const;

    const_iterator begin () const;
    casacore::Bool contains (VisBufferComponents::EnumType component) const;
    const_iterator end () const;

    static VbDirtyComponents all ();
    static VbDirtyComponents exceptThese (VisBufferComponents::EnumType component, ...);
    static VbDirtyComponents none ();
    static VbDirtyComponents singleton (VisBufferComponents::EnumType component);
    static VbDirtyComponents these (VisBufferComponents::EnumType component, ...);

protected:

private:

    Set set_p;

    static const VbDirtyComponents all_p;

    static VbDirtyComponents initializeAll ();

};

namespace asyncio {
    class VLAT;
} // end namespace asyncio

//<summary>VisBuffers encapsulate one chunk of visibility data for processing.</summary>
//
// <use visibility=export>
//
// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//   <li> <linkto class="VisSet">VisSet</linkto>
//   <li> <linkto class="VisibilityIterator">VisibilityIterator</linkto>
//   <li> <linkto class="VisBufferAutoPtr">VisBufferAutoPtr</linkto>
//   <li> <linkto class="VbDirtyComponents">VbDirtyComponents</linkto>
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
//
// When the user intends to allow asynchronous I/O they will need to use
// the VisBufferAsync and VisBufferAsyncWrapper classes; these are
// publicly derived from VisBuffer.  Normally, the user should not
// explicitly use these classes but should use the helper class
// VisBufferAutoPtr which will ensure that the appropriate class is
// used.
//</synopsis>

//<todo>
// <li> reconcile vis/visCube usage: visCube, flagCube and weightMatrix
// are currently only correct when this VisBuffer got them from a
// VisIter, operations like -=, freqAverage() are only done for
// visibility() and flag().
//</todo>

class VisBuffer {

    friend class asyncio::VLAT; // for async i/o
    friend class VisBufferAsync; // for async i/o
    friend class VisBufferAsyncWrapper; // for async i/o
    friend class ViReadImplAsync; // for async I/O
    friend class SIMapperCollection; //for SIimager as we need access to casacore::MS object
    friend class VisModelData;  // need access to ms to recover the model rec

public:
    // Create empty VisBuffer you can assign to or attach.
    VisBuffer();
    // Construct VisBuffer for a particular VisibilityIterator
    // The buffer will remain synchronized with the iterator.
    VisBuffer(ROVisibilityIterator & iter);

    // Copy construct, looses synchronization with iterator: only use buffer for
    // current iteration (or reattach).
    VisBuffer(const VisBuffer & vb);

    // Destructor (detaches from VisIter)
    virtual ~VisBuffer();

    // Assignment, loses synchronization with iterator: only use buffer for
    // current iteration (or reattach)

    virtual VisBuffer & operator=(const VisBuffer & vb);

    // Assignment, optionally without copying the data across; with copy=true
    // this is identical to normal assignment operator

    virtual VisBuffer & assign(const VisBuffer & vb, casacore::Bool copy = true);

    virtual VisBuffer * clone () const;

    // subtraction: return the difference of the visibilities, flags of
    // this and other are or-ed. An exception is thrown if the number of
    // rows or channels differs, but no further checks are done.
    VisBuffer & operator-=(const VisBuffer & vb);

    // Attach to a VisIter. Detaches itself first if already attached
    // to a VisIter. Will remain synchronized with iterator.
    virtual void attachToVisIter(ROVisibilityIterator & iter);
    virtual void detachFromVisIter ();

    // Invalidate the cache
    virtual void invalidate();

    // <group>
    // Access functions
    //
    virtual casacore::Int & nCorr() {
        return nCorrOK_p ? nCorr_p : fillnCorr();
    }
    virtual casacore::Int nCorr() const {
        return This->nCorr();
    }

    virtual casacore::Int & nChannel() {
        return nChannelOK_p ? nChannel_p : fillnChannel();
    }
    virtual casacore::Int nChannel() const {
        return This->nChannel();
    }

    virtual casacore::Vector<casacore::Int>& channel() {
        return channelOK_p ? channel_p : fillChannel();
    }
    virtual const casacore::Vector<casacore::Int>& channel() const {
        return This->channel();
    }

    // virtual casacore::Int & nCat() {
    //     return nCatOK_p ? nCat_p : fillnCat();
    // }
    // virtual casacore::Int nCat() const {
    //     return This->nCat();
    // }

    ///Because of it being publicly exposed ...using nRow, nChannel, nCorr etc to 
    /// determine the size of the buffer can be totally WRONG
    ///They MAY NOT  represent the shape 
    /// of a buffer filled in this iteration.
    // Decide what is the right value (depending on where the vb is coming from) 
    // for you for the size of the buffer. i.e (nCorr(), nChannel(), nRow()) or vb.visCube().shape()
    // The latter comes from the VisIter state ...the former be careful...
    /// For example VisBuffAccumulator changes these in an unconnected fashion; 
    //without updating fillnrow 
    /// datacube shape  etc etc.
    /// You are warned nrow_p etc are public variables effectively (despite being 
    ///declared private) so it can be changed
    /// anywhere the code uses a vb, intentionally or accidentally.
    virtual casacore::Int & nRow() {
        return nRowOK_p ? nRow_p : fillnRow();
    }
    virtual casacore::Int nRow() const {
        return This->nRow();
    }

    virtual casacore::Int nRowChunk() const;

    virtual casacore::Vector<casacore::Int>& antenna1() {
        return antenna1OK_p ? antenna1_p : fillAnt1();
    }
    virtual const casacore::Vector<casacore::Int>& antenna1() const {
        return This->antenna1();
    }

    virtual casacore::Vector<casacore::Int>& antenna2() {
        return antenna2OK_p ? antenna2_p : fillAnt2();
    }
    virtual const casacore::Vector<casacore::Int>& antenna2() const {
        return This->antenna2();
    }

    virtual casacore::Vector<casacore::Int>& feed1() {
        return feed1OK_p ? feed1_p : fillFeed1();
    }
    virtual const casacore::Vector<casacore::Int>& feed1() const {
        return This->feed1();
    }

    virtual casacore::Vector<casacore::Int>& feed2() {
        return feed2OK_p ? feed2_p : fillFeed2();
    }
    virtual const casacore::Vector<casacore::Int>& feed2() const {
        return This->feed2();
    }

    virtual void dirtyComponentsAdd (const VbDirtyComponents & additionalDirtyComponents);
    virtual void dirtyComponentsAdd (VisBufferComponents::EnumType component);
    virtual void dirtyComponentsClear ();
    virtual VbDirtyComponents dirtyComponentsGet () const;
    virtual void dirtyComponentsSet (const VbDirtyComponents & dirtyComponents);
    virtual void dirtyComponentsSet (VisBufferComponents::EnumType component);

    virtual casacore::Bool fetch(const asyncio::PrefetchColumns *pfc);

    // feed1_pa() and feed2_pa() return an array of parallactic angles
    // (each corresponds to the first receptor of the feed) one for each
    // row in the current buffer. In contrast, feed_pa() calculates
    // the angles for each antenna. These methods are implemented for
    // VisBuffer only to benefit from caching of the feed and antenna IDs.
    virtual casacore::Vector<casacore::Float>& feed1_pa() {
        return feed1_paOK_p ? feed1_pa_p : fillFeed1_pa();
    }
    virtual const casacore::Vector<casacore::Float>& feed1_pa() const {
        return This->feed1_pa();
    }

    virtual casacore::Vector<casacore::Float>& feed2_pa() {
        return feed2_paOK_p ? feed2_pa_p : fillFeed2_pa();
    }
    virtual const casacore::Vector<casacore::Float>& feed2_pa() const {
        return This->feed2_pa();
    }

    virtual casacore::Vector<casacore::SquareMatrix<casacore::Complex, 2> >& CJones() {
        return cjonesOK_p ? cjones_p : fillCjones();
    }
    virtual const casacore::Vector<casacore::SquareMatrix<casacore::Complex, 2> >& CJones() const {
        return This->CJones();
    }

    // Note that feed_pa is a function instead of a cached value
    virtual casacore::Vector<casacore::Float> feed_pa(casacore::Double time) const;

    // direction1() and direction2() return arrays of directions where
    // the first and the second antenna/feed are pointed to. One value for
    // each row in the current buffer.
    virtual casacore::Vector<casacore::MDirection>& direction1() {
        return direction1OK_p ? direction1_p
               : fillDirection1();
    }
    virtual const casacore::Vector<casacore::MDirection>& direction1()  const {
        return This->direction1();
    }

    virtual casacore::Vector<casacore::MDirection>& direction2() {
        return direction2OK_p ? direction2_p :
               fillDirection2();
    }
    virtual const casacore::Vector<casacore::MDirection>& direction2()  const {
        return This->direction2();
    }
    virtual casacore::MDirection& firstDirection1() {
        return firstDirection1OK_p ? firstDirection1_p
               : fillFirstDirection1();
    }
    virtual const casacore::MDirection& firstDirection1()  const {
        return This->firstDirection1();
    }
    // NOMINAL parallactic angle (feed p.a. offset NOT included)
    virtual casacore::Float parang0(casacore::Double time) const;
    virtual casacore::Vector<casacore::Float> parang(casacore::Double time) const;

    // Note that azel is a function instead of a cached value
    virtual casacore::MDirection azel0(casacore::Double time) const;
    virtual casacore::Vector<casacore::Double>& azel0Vec(casacore::Double time, casacore::Vector<casacore::Double>& azelVec) const;
    virtual casacore::Vector<casacore::MDirection> azel(casacore::Double time) const;
    virtual casacore::Matrix<casacore::Double>& azelMat(casacore::Double time, casacore::Matrix<casacore::Double>& azelMat) const;

    // Hour angle for specified time
    virtual casacore::Double hourang(casacore::Double time) const;

    virtual casacore::Int fieldId() const {
        return fieldIdOK_p ? fieldId_p : This->fillFieldId();
    }

    virtual casacore::Int& fieldIdRef() {
        return fieldIdOK_p ? fieldId_p : This->fillFieldId();
    }

    virtual casacore::Int& arrayIdRef() {
        return arrayIdOK_p ? arrayId_p : This->fillArrayId();
    }

    virtual casacore::Int arrayId() const {
        return arrayIdOK_p ? arrayId_p : This->fillArrayId();
    }

    // Return flag for each channel & row
    virtual casacore::Matrix<casacore::Bool>& flag() {
        return flagOK_p ? flag_p : fillFlag();
    }
    virtual const casacore::Matrix<casacore::Bool>& flag() const {
        return This->flag();
    }

    // Return flag for each polarization, channel and row
    virtual casacore::Cube<casacore::Bool>& flagCube() {
        return flagCubeOK_p ? flagCube_p : fillFlagCube();
    }
    virtual const casacore::Cube<casacore::Bool>& flagCube() const {
        return This->flagCube();
    }

    virtual casacore::Vector<casacore::Bool>& flagRow() {
        return flagRowOK_p ? flagRow_p : fillFlagRow();
    }
    virtual const casacore::Vector<casacore::Bool>& flagRow() const {
        return This->flagRow();
    }

    // Return flags for each polarization, channel, category, and row.
    virtual casacore::Array<casacore::Bool>& flagCategory() {
        return flagCategoryOK_p ? flagCategory_p : fillFlagCategory();
    }
    virtual const casacore::Array<casacore::Bool>& flagCategory() const {
        return This->flagCategory();
    }

    virtual casacore::Vector<casacore::Int>& scan() {
        return scanOK_p ? scan_p : fillScan();
    }
    virtual const casacore::Vector<casacore::Int>& scan() const {
        return This->scan();
    }

    // scalar version for convenience, when scan known constant for
    // entire iteration/buffer.
    virtual casacore::Int scan0() {
        return scan()(0);
    }

    virtual casacore::Vector<casacore::Int>& processorId() {
        return processorIdOK_p ? processorId_p : fillProcessorId();
    }
    virtual const casacore::Vector<casacore::Int>& processorId() const {
        return This->processorId();
    }

    virtual casacore::Vector<casacore::Int>& observationId() {
        return observationIdOK_p ? observationId_p : fillObservationId();
    }
    virtual const casacore::Vector<casacore::Int>& observationId() const {
        return This->observationId();
    }

    virtual casacore::Vector<casacore::Int>& stateId() {
        return stateIdOK_p ? stateId_p : fillStateId();
    }
    virtual const casacore::Vector<casacore::Int>& stateId() const {
        return This->stateId();
    }

    // Gets SPECTRAL_WINDOW/CHAN_FREQ (in Hz, acc. to the casacore::MS def'n v.2).
    virtual casacore::Vector<casacore::Double>& frequency() {
        return frequencyOK_p ? frequency_p : fillFreq();
    }
    virtual const casacore::Vector<casacore::Double>& frequency() const {
        return This->frequency();
    }

//    virtual casacore::Vector<casacore::Double>& lsrFrequency() {
//        return lsrFrequencyOK_p ? lsrFrequency_p : fillLSRFreq();
//    }
//    virtual const casacore::Vector<casacore::Double>& lsrFrequency() const {
//        return This->lsrFrequency();
//    }


    //the following method is to convert the observed frequencies
    // This conversion may not be accurate for some frame
    // conversion like topo to lsr except if the spw is in the actual buffer

    //if ignoreconv=true..frequency is served as is in the data frame
    virtual void lsrFrequency(const casacore::Int & spw, casacore::Vector<casacore::Double>& freq, casacore::Bool & convert, const casacore::Bool ignoreconv=false) const;

    virtual casacore::Int numberCoh () const;

    virtual casacore::MDirection & phaseCenter() {
        return phaseCenterOK_p ? phaseCenter_p : fillPhaseCenter();
    }
    virtual casacore::MDirection phaseCenter() const {
        return This->phaseCenter();
    }
    virtual const casacore::MDirection phaseCenter(const casacore::Int fieldId, const casacore::Double time=-1.0) const;
    virtual const casacore::MDirection phaseCenter(const casacore::Double time) const;
    virtual casacore::Int polFrame() const {
        return polFrameOK_p ? polFrame_p : This->fillPolFrame();
    }

    virtual casacore::Vector<casacore::Int>& corrType() {
        return corrTypeOK_p ? corrType_p : fillCorrType();
    }
    virtual const casacore::Vector<casacore::Int>& corrType() const {
        return This->corrType();
    }

    virtual casacore::Vector<casacore::Float>& sigma() {
        return sigmaOK_p ? sigma_p : fillSigma();
    }
    virtual const casacore::Vector<casacore::Float>& sigma() const {
        return This->sigma();
    }

    virtual casacore::Matrix<casacore::Float>& sigmaMat() {
        return sigmaMatOK_p ? sigmaMat_p : fillSigmaMat();
    }
    virtual const casacore::Matrix<casacore::Float>& sigmaMat() const {
        return This->sigmaMat();
    }

    virtual casacore::Int & spectralWindow() {
        return spectralWindowOK_p ? spectralWindow_p : This->fillSpW();
    }
    virtual casacore::Int spectralWindow() const {
        return spectralWindowOK_p ? spectralWindow_p : This->fillSpW();
    }
    virtual casacore::Int polarizationId() const {
      return visIter_p->polarizationId();
    } 
    virtual casacore::Int& dataDescriptionIdRef() {
      return dataDescriptionIdOK_p ? dataDescriptionId_p : This->fillDataDescriptionId ();
    }
    virtual casacore::Int dataDescriptionId() const {
      return dataDescriptionIdOK_p ? dataDescriptionId_p : This->fillDataDescriptionId ();
    }
    virtual casacore::Vector<casacore::Double>& time() {
        return timeOK_p ? time_p : fillTime();
    }
    virtual const casacore::Vector<casacore::Double>& time() const {
        return This->time();
    }

    virtual casacore::Vector<casacore::Double>& timeCentroid() {
        return timeCentroidOK_p ? timeCentroid_p : fillTimeCentroid();
    }
    virtual const casacore::Vector<casacore::Double>& timeCentroid() const {
        return This->timeCentroid();
    }

    virtual casacore::Vector<casacore::Double>& timeInterval() {
        return timeIntervalOK_p ? timeInterval_p : fillTimeInterval();
    }
    virtual const casacore::Vector<casacore::Double>& timeInterval() const {
        return This->timeInterval();
    }

    virtual casacore::Vector<casacore::Double>& exposure() {
        return exposureOK_p ? exposure_p : fillExposure();
    }
    virtual const casacore::Vector<casacore::Double>& exposure() const {
        return This->exposure();
    }

    virtual casacore::Vector<casacore::RigidVector<casacore::Double, 3> >& uvw() {
        return uvwOK_p ? uvw_p : filluvw();
    }
    virtual const casacore::Vector<casacore::RigidVector<casacore::Double, 3> >& uvw() const {
        return This->uvw();
    }

    virtual casacore::Matrix<casacore::Double>& uvwMat() {
        return uvwMatOK_p ? uvwMat_p : filluvwMat();
    }
    virtual const casacore::Matrix<casacore::Double>& uvwMat() const {
        return This->uvwMat();
    }

    virtual casacore::Matrix<CStokesVector>& visibility() {
        return visibilityOK_p ? visibility_p : fillVis(VisibilityIterator::Observed);
    }
    virtual const casacore::Matrix<CStokesVector>& visibility() const {
        return This->visibility();
    }

    virtual casacore::Matrix<CStokesVector>& modelVisibility() {
        return modelVisibilityOK_p ? modelVisibility_p :
               fillVis(VisibilityIterator::Model);
    }
    virtual const casacore::Matrix<CStokesVector>& modelVisibility() const {
        return This->modelVisibility();
    }

    virtual casacore::Matrix<CStokesVector>& correctedVisibility() {
        return correctedVisibilityOK_p ? correctedVisibility_p :
               fillVis(VisibilityIterator::Corrected);
    }
    virtual const casacore::Matrix<CStokesVector>& correctedVisibility() const {
        return This->correctedVisibility();
    }

    virtual casacore::Cube<casacore::Complex>& visCube() {
        return visCubeOK_p ? visCube_p : fillVisCube(VisibilityIterator::Observed);
    }
    virtual const casacore::Cube<casacore::Complex>& visCube() const {
        return This->visCube();
    }

    virtual casacore::Cube<casacore::Complex>& modelVisCube() {
        return modelVisCubeOK_p ? modelVisCube_p :
               fillVisCube(VisibilityIterator::Model);
    }

    virtual casacore::Cube<casacore::Complex>& modelVisCube(const casacore::Bool & matchVisCubeShape) {
        // Avoids call to fillVisCube(VisIter::Model)
        modelVisCubeOK_p = true;
        if (matchVisCubeShape) {
            // shape it in memory like data
            modelVisCube_p.resize(visCube().shape());
            modelVisCube_p = casacore::Complex(0.0);
        }
        // ... and return it in the usual way
        return modelVisCube();
    }

    virtual const casacore::Cube<casacore::Complex>& modelVisCube() const {
        return This->modelVisCube();
    }

    virtual casacore::Cube<casacore::Complex>& correctedVisCube() {
        return correctedVisCubeOK_p ? correctedVisCube_p :
               fillVisCube(VisibilityIterator::Corrected);
    }
    virtual const casacore::Cube<casacore::Complex>& correctedVisCube() const {
        return This->correctedVisCube();
    }

    // Return visCube(), modelVisCube(), or correctedVisCube(),
    // according to whichcol.  They throw an exception if whichcol is
    // unsupported, but note the encouraged default.
    // TODO: Optionally return DATA if whichcol is unavailable.
    casacore::Cube<casacore::Complex>& dataCube(const casacore::MS::PredefinedColumns whichcol=casacore::MS::DATA);
    const casacore::Cube<casacore::Complex>& dataCube(const casacore::MS::PredefinedColumns
                                  whichcol=casacore::MS::DATA) const;

    virtual casacore::Cube<casacore::Float>& floatDataCube() {
        return floatDataCubeOK_p ? floatDataCube_p : fillFloatDataCube();
    }
    virtual const casacore::Cube<casacore::Float>& floatDataCube() const {
        return This->floatDataCube();
    }

    // Returns the weights for each row averaged over the parallel hand correlations.
    virtual casacore::Vector<casacore::Float>& weight() {
        return weightOK_p ? weight_p : fillWeight();
    }
    virtual const casacore::Vector<casacore::Float>& weight() const {
        return This->weight();
    }

    // Returns the nPol_p x curNumRow_p weight matrix.
    virtual casacore::Matrix<casacore::Float>& weightMat() {
        return weightMatOK_p ? weightMat_p : fillWeightMat();
    }
    virtual const casacore::Matrix<casacore::Float>& weightMat() const {
        return This->weightMat();
    }

    // Is a valid WEIGHT_SPECTRUM available?
    virtual casacore::Bool existsWeightSpectrum() const {
      return visIter_p ? visIter_p->existsWeightSpectrum() : false;
    }

    virtual casacore::Cube<casacore::Float>& weightSpectrum() {
        return weightSpectrumOK_p ? weightSpectrum_p : fillWeightSpectrum();
    }
    virtual const casacore::Cube<casacore::Float>& weightSpectrum() const {
        return This->weightSpectrum();
    }

    virtual casacore::Matrix<casacore::Float>& imagingWeight();
    virtual const casacore::Matrix<casacore::Float>& imagingWeight() const;


//    virtual casacore::Matrix<casacore::Float>& imagingWeight() {
//        return imagingWeightOK_p ? imagingWeight_p : fillImagingWeight();
//    }
//    virtual const casacore::Matrix<casacore::Float>& imagingWeight() const {
//        return This->imagingWeight();
//    }

    virtual casacore::Cube<casacore::Float>& weightCube() {
        return weightCube_p;
    }
    //</group>

    //<group>
    // Utility functions to provide coordinate or column ranges of the
    // data in the VisBuffer. Flagging is applied before computing the ranges.
    //
    // Generic accessor to column ranges of integer type, as specified by
    // enumerations defined in class MSCalEnums. Throws an exception
    // if the enum is not for a recognized integer column.
    virtual casacore::Vector<casacore::Int> vecIntRange(const MSCalEnums::colDef & calEnum) const;

    // Antenna id. range (includes both ANTENNA1 and ANTENNA2 columns)
    virtual casacore::Vector<casacore::Int> antIdRange() const;

    // casacore::Time range
    virtual casacore::Bool timeRange(casacore::MEpoch & rTime, casacore::MVEpoch & rTimeEP, casacore::MVEpoch & rInterval) const;

    // Return the row Ids from the original ms. If the ms used is a subset of
    // another ms then rowIds() return the row ids of the original ms.
    virtual casacore::Vector<casacore::rownr_t>& rowIds();

    virtual const casacore::Vector<casacore::rownr_t>& rowIds() const {
        return This->rowIds();
    };

    //</group>

    // Frequency average the buffer (visibility() column only)
    virtual void freqAverage();

    // Frequency average the buffer (visCube and [if present] modelVisCube)
    void freqAveCubes();

    // Average channel axis according to chanavebounds, for whichever of DATA,
    // MODEL_DATA, CORRECTED_DATA, FLOAT_DATA, FLAG, and WEIGHT_SPECTRUM are
    // present.  It will only treat the first 5 as present if they have already
    // been loaded into the buffer!
    void channelAve(const casacore::Matrix<casacore::Int>& chanavebounds,casacore::Bool calmode=true);

    // Average channel axis by factor.
    template<class T> void chanAveVisCube(casacore::Cube<T>& data, casacore::Int nChanOut);

    // Accumulate channel axis by factor, without applying WEIGHT_SPECTRUM even
    // if it is present.
    // It is primarily intended for averaging WEIGHT_SPECTRUM itself.
    template<class T> void chanAccCube(casacore::Cube<T>& data, casacore::Int nChanOut);

    // This defaults to no conceptual side effects, but usually it is more
    // efficient to let it leave weightSpectrum() in a channel averaged state.
    // restoreWeightSpectrum has no effect if !existsWeightSpectrum().
    virtual void chanAveFlagCube(casacore::Cube<casacore::Bool>& flagcube, const casacore::Int nChanOut,
                         const casacore::Bool restoreWeightSpectrum = true);

    // Doesn't do anything if flagcat is degenerate.
    void chanAveFlagCategory(casacore::Array<casacore::Bool>& flagcat, const casacore::Int nChanOut);

    // Form casacore::Stokes parameters from correlations
    //  (these are preliminary versions)
    virtual void formStokes();
    virtual void formStokesWeightandFlag();
    virtual void formStokes(casacore::Cube<casacore::Complex>& vis);
    virtual void formStokes(casacore::Cube<casacore::Float>& fcube);    // Will throw up if asked to do all 4.

    // casacore::Sort/unsort the correlations, if necessary
    //  (Rudimentary handling of non-canonically sorted correlations--use with care!)
    virtual void sortCorr();
    virtual void unSortCorr();

    // Normalize the visCube by the modelVisCube
    //   (and optionally also divide visCube_p by its normalized amp)
    virtual void normalize(const casacore::Bool & phaseOnly = false);

    // Fill weightMat according to sigma column
    virtual void resetWeightMat();

    // Rotate visibility phase for given vector (dim = nrow of vb) of phases (metres)
    virtual void phaseCenterShift(const casacore::Vector<casacore::Double>& phase);
    // Rotate visibility phase for phase center offsets (arcsecs)
    virtual void phaseCenterShift(casacore::Double dx, casacore::Double dy);

    // Update coordinate info - useful for copied VisBuffers that need
    // to retain some state for later reference.
    // Presently this fills antenna, array, field and spectralWindow ids, time,
    // frequency and number of rows. 
    // if dirDependent is set to false the expensive direction dependent calculation of parallactic or direction of
    // antenna from pointing table is avoided
    //Add more as needed.
    virtual void updateCoordInfo(const VisBuffer * vb = NULL, const casacore::Bool dirDependent=true);
    void copyCoordInfo(const VisBuffer& other, casacore::Bool force=false);

    // Set the visibility to a constant, note that this only changes the buffer,
    // no values are written back to tables from here.
    virtual void setVisCube(casacore::Complex c);
    virtual void setModelVisCube(casacore::Complex c);
    virtual void setCorrectedVisCube(casacore::Complex c);

    // Set the visibility, note that this only changes the buffer,
    // no values are written back to tables from here.
    virtual void setVisCube(const casacore::Cube<casacore::Complex>& vis);
    virtual void setModelVisCube(const casacore::Cube<casacore::Complex>& vis);
    virtual void setCorrectedVisCube(const casacore::Cube<casacore::Complex>& vis);

    // Like the above, but for FLOAT_DATA, keeping it as real floats.
    virtual void setFloatDataCube(const casacore::Cube<casacore::Float>& fcube);

    // Set model according to a casacore::Stokes vector
    virtual void setModelVisCube(const casacore::Vector<casacore::Float>& stokes);

    // Reference external model visibilities
    virtual void refModelVis(const casacore::Matrix<CStokesVector>& mvis);

    // Remove scratch cols data from vb
    virtual void removeScratchCols();

    // Access the current casacore::MSColumns object via VisIter
    virtual const casacore::MSColumns & msColumns() const {
        return visIter_p->msColumns();
    }

    virtual  casacore::Int numberAnt () const;



    // Get all selected spectral windows not just the one in the actual buffer
    virtual void allSelectedSpectralWindows(casacore::Vector<casacore::Int>& spws, casacore::Vector<casacore::Int>& nvischan) {
        visIter_p->allSelectedSpectralWindows(spws, nvischan);
    }

  virtual void getChannelSelection(casacore::Block< casacore::Vector<casacore::Int> >& blockNGroup,
				   casacore::Block< casacore::Vector<casacore::Int> >& blockStart,
				   casacore::Block< casacore::Vector<casacore::Int> >& blockWidth,
				   casacore::Block< casacore::Vector<casacore::Int> >& blockIncr,
				   casacore::Block< casacore::Vector<casacore::Int> >& blockSpw) const {
    visIter_p->getChannelSelection(blockNGroup, blockStart, blockWidth, blockIncr, blockSpw);
  };
    void allSelectedSpectralWindows(casacore::Vector<casacore::Int>& spws, casacore::Vector<casacore::Int>& nvischan) const {
        This->allSelectedSpectralWindows(spws, nvischan);
    }

    // Return the actual msid, useful if using multiple ms to monitor which
    // ms in the  list is being dealt with
    virtual casacore::Int msId() const {
        This->checkMSId();
        return oldMSId_p;
    }

    //checked if the ms has changed since the last chunk processed
    virtual casacore::Bool newMS() const {
        This->checkMSId();
        return newMS_p;
    }
    //get the name of the ms the buffer is at empty string if no visiter is attached
    virtual casacore::String msName(casacore::Bool stripPath=false) const;

    virtual casacore::Bool newArrayId () const;
    virtual casacore::Bool newFieldId () const;
    virtual casacore::Bool newSpectralWindow () const;

    //

protected:

    virtual casacore::Bool checkMSId();
    virtual void checkVisIter (const char * func, const char * file, int line, const char * extra = "") const;
    virtual void copyCache (const VisBuffer & other, casacore::Bool force);
    virtual void copyMsInfo (casacore::Int & msID, casacore::Bool & MsOk_p, casacore::Bool & newMs) const;
    virtual const casacore::Matrix<casacore::Float>& imagingWeight(const VisImagingWeight & weightGenerator) const;
    virtual casacore::Int getOldMsId () const;
    virtual ROVisibilityIterator * getVisibilityIterator () const;

    template <typename Coord>
    void updateCoord (const VisBuffer * other,
                      casacore::Bool otherOk,
                      const Coord & (VisBuffer::* getCoord) () const,
                      Coord & coord,
                      casacore::Bool & coordOk)
    {
        if (otherOk){
            coord.assign ((other ->* getCoord) ());
            coordOk = true;
        }
        else {
            ((this ->* getCoord) ());
        }
    }

    template <typename Scalar>
    void updateCoordS (const VisBuffer * other,
                       casacore::Bool otherOk,
                       Scalar (VisBuffer::* getCoord) () const,
                       Scalar & coord,
                       casacore::Bool & coordOk)
    {
        if (otherOk){
            coord = (other ->* getCoord) ();
            coordOk = true;
        }
        else {
            ((this ->* getCoord) ());
        }
    }


private:


    // validate the cache
    virtual void validate();

    void checkVisIterBase (const char * func, const char * file, int line, const char * extra = "") const;

    template<typename T>
    static void cacheCopyArray (casacore::Bool & newStatus,
                                casacore::Bool oldStatus,
                                T & newCache,
                                const VisBuffer & other,
                                const T &  (VisBuffer::* oldCache) () const,
                                casacore::Bool force) {

        // Leave things unchanged if the old status is false.  This will often
        // leave the value with an empty data structure and an OK status which
        // is needed in many cases.
        //
        // For copying casacore::Array<T> derived objects since the assign operator
        // doesn't work for these.

        newStatus = force || oldStatus;

        if (newStatus) {
            newCache.assign (((& other) ->* oldCache) ());
        }
    }

    template<typename T>
    static void cacheCopyNormal (casacore::Bool & newStatus,
                                 casacore::Bool oldStatus,
                                 T & newCache,
                                 const VisBuffer & other,
                                 T (VisBuffer::* oldCache) () const,
                                 casacore::Bool force) {

        // Leave things unchanged if the old status is false.  This will often
        // leave the value with an empty data structure and an OK status which
        // is needed in many cases.
        //
        // For copying "normal" cache status and values.  casacore::Normal means
        // the assign operator works (which it doesn't for classes based on casacore::Array<T>)

        newStatus = force || oldStatus;

        if (newStatus) {

            newCache = ((& other) ->* oldCache) ();
        }
    }

    virtual void setAllCacheStatuses (bool status);

    virtual casacore::Bool nonCanonCorr(); // Are correlations in non-canonical order?

    // Filter index arrays for unique elements
    virtual casacore::Vector<casacore::Int> unique(const casacore::Vector<casacore::Int>& indices) const;

    casacore::Bool corrSorted_p; // Have correlations been sorted by sortCorr?
    VbDirtyComponents dirtyComponents_p;
    casacore::Int lastPointTableRow_p;
    casacore::Int oldMSId_p;
    VisBuffer * This;
    casacore::Bool twoWayConnection_p;
    ROVisibilityIterator * visIter_p;
    casacore::CountedPtr <VisModelDataI> visModelData_p;

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

    virtual casacore::Vector<casacore::Int>& fillAnt1();
    virtual casacore::Vector<casacore::Int>& fillAnt2();
    virtual casacore::Int & fillArrayId();
    //virtual casacore::Matrix<casacore::Int>& fillChanAveBounds();
    virtual casacore::Vector<casacore::Int>& fillChannel();
    virtual casacore::Vector<casacore::SquareMatrix<casacore::Complex, 2> >& fillCjones();
    virtual casacore::Vector<casacore::Int>& fillCorrType();
    virtual casacore::Int & fillDataDescriptionId ();
    virtual casacore::Vector<casacore::MDirection>& fillDirection1();
    virtual casacore::MDirection& fillFirstDirection1();
    virtual casacore::Vector<casacore::MDirection>& fillDirection2();
    virtual casacore::Vector<casacore::Double>& fillExposure();
    virtual casacore::Vector<casacore::Int>& fillFeed1();
    virtual casacore::Vector<casacore::Float>& fillFeed1_pa();
    virtual casacore::Vector<casacore::Int>& fillFeed2();
    virtual casacore::Vector<casacore::Float>& fillFeed2_pa();
    virtual casacore::Int & fillFieldId();
    virtual casacore::Matrix<casacore::Bool>& fillFlag();
    virtual casacore::Array<casacore::Bool>& fillFlagCategory();
    virtual casacore::Cube<casacore::Bool>& fillFlagCube();
    virtual casacore::Vector<casacore::Bool> & fillFlagRow();
    virtual casacore::Cube<casacore::Float>& fillFloatDataCube();
    virtual casacore::Vector<casacore::Double>& fillFreq();         // Puts SPECTRAL_WINDOW/CHAN_FREQ in frequency_p.
    //virtual casacore::Matrix<casacore::Float>& fillImagingWeight();
    //virtual casacore::Vector<casacore::Double>& fillLSRFreq();
    virtual casacore::Int & fillnChannel();
    virtual casacore::Int & fillnCorr();
  //    virtual casacore::Int & fillnCat();
    virtual casacore::Int & fillnRow();
    virtual casacore::Vector<casacore::Int> & fillObservationId();
    virtual casacore::MDirection & fillPhaseCenter();
    virtual casacore::Int & fillPolFrame();
    virtual casacore::Vector<casacore::Int> & fillProcessorId();
    virtual casacore::Vector<casacore::Int> & fillScan();
    virtual casacore::Vector<casacore::Float>& fillSigma();
    virtual casacore::Matrix<casacore::Float>& fillSigmaMat();
    virtual casacore::Int & fillSpW();
    virtual casacore::Vector<casacore::Int> & fillStateId();
    virtual casacore::Vector<casacore::Double>& fillTime();
    virtual casacore::Vector<casacore::Double>& fillTimeCentroid();
    virtual casacore::Vector<casacore::Double>& fillTimeInterval();
    virtual casacore::Vector<casacore::RigidVector<casacore::Double, 3> >& filluvw();
    virtual casacore::Matrix<casacore::Double>& filluvwMat();
    virtual casacore::Matrix<CStokesVector>& fillVis(VisibilityIterator::DataColumn whichOne);
    virtual casacore::Cube<casacore::Complex>& fillVisCube(VisibilityIterator::DataColumn whichOne);
    virtual casacore::Vector<casacore::Float>& fillWeight();
    virtual casacore::Matrix<casacore::Float>& fillWeightMat();
    virtual casacore::Cube<casacore::Float>& fillWeightSpectrum();

    casacore::Bool newMS_p;

    // Variables to track validity of cache (alphabetical order)

#define CacheStatus(item) \
virtual bool item ## OK () const\
{\
    return item ## OK_p;\
}\
bool item ## OK_p;

    // Define the cache statuses
    //
    // For example, CacheStatus (antenna1) defines:
    //
    // virtual casacore::Bool antenna1OK () const { return antenna1OK_p;}
    //
    // and
    //
    // casacore::Bool antenna1OK_p;

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
    CacheStatus (firstDirection1);
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

    // Cached values (alphabetical order)

    casacore::Vector<casacore::Int> antenna1_p;
    casacore::Vector<casacore::Int> antenna2_p;
    casacore::Int arrayId_p;
    casacore::Matrix<casacore::Int> chanAveBounds_p;
    casacore::Vector<casacore::Int> channel_p;
    casacore::Vector<casacore::SquareMatrix<casacore::Complex, 2> > cjones_p;
    casacore::Cube<casacore::Complex> correctedVisCube_p;
    casacore::Matrix<CStokesVector> correctedVisibility_p;
    casacore::Vector<casacore::Int> corrType_p;
    casacore::Int dataDescriptionId_p;
    casacore::Vector<casacore::MDirection> direction1_p; //where the first antenna/feed is pointed to
    casacore::Vector<casacore::MDirection> direction2_p; //where the second antenna/feed is pointed to
    casacore::MDirection firstDirection1_p;
    casacore::Vector<casacore::Double> exposure_p;
    casacore::Vector<casacore::Int> feed1_p;
    casacore::Vector<casacore::Float> feed1_pa_p;
    casacore::Vector<casacore::Int> feed2_p;
    casacore::Vector<casacore::Float> feed2_pa_p;
    casacore::Int fieldId_p;
    casacore::Matrix<casacore::Bool> flag_p;
    casacore::Array<casacore::Bool> flagCategory_p;
    casacore::Cube<casacore::Bool> flagCube_p;
    casacore::Vector<casacore::Bool> flagRow_p;
    casacore::Cube<casacore::Float> floatDataCube_p;
    casacore::Vector<casacore::Double> frequency_p;
    mutable casacore::Matrix<casacore::Float> imagingWeight_p;
    //casacore::Vector<casacore::Double> lsrFrequency_p;
    casacore::Cube<casacore::Complex> modelVisCube_p;
    casacore::Matrix<CStokesVector> modelVisibility_p;
    casacore::Int nChannel_p;
    casacore::Int nCorr_p;
  //    casacore::Int nCat_p;
    casacore::Int nRow_p;
    casacore::Vector<casacore::Int> observationId_p;
    casacore::MDirection phaseCenter_p;
    casacore::Int polFrame_p;
    casacore::Vector<casacore::Int> processorId_p;
    casacore::Vector<casacore::rownr_t> rowIds_p;
    casacore::Vector<casacore::Int> scan_p;
    casacore::Vector<casacore::Float> sigma_p;
    casacore::Matrix<casacore::Float> sigmaMat_p;
    casacore::Int spectralWindow_p;
    casacore::Vector<casacore::Int> stateId_p;
    casacore::Vector<casacore::Double> time_p;
    casacore::Vector<casacore::Double> timeCentroid_p;
    casacore::Vector<casacore::Double> timeInterval_p;
    casacore::Vector<casacore::RigidVector<casacore::Double, 3> > uvw_p;
    casacore::Matrix<casacore::Double> uvwMat_p;
    casacore::Cube<casacore::Complex> visCube_p;
    casacore::Matrix<CStokesVector> visibility_p;
    casacore::Vector<casacore::Float> weight_p;
    casacore::Cube<casacore::Float> weightCube_p;
    casacore::Matrix<casacore::Float> weightMat_p;
    casacore::Cube<casacore::Float> weightSpectrum_p;

};

// <summary>
// A convenience class to assist in migrating code to potentially use
// asynchronous I/O.
//
// <prerequisite>
//   <li> <linkto class="VisBuffer">VisBuffer</linkto>
//   <li> <linkto class="VisBufferAsync">VisBufferAsync</linkto>
//   <li> <linkto class="ROVisibilityIterator">ROVisibilityIterator</linkto>
// </prerequisite>
//
// </summary>
//
// <synopsis>
//
// When existing code is modified to potentially use asynchronous I/O the current
// VisBuffer usage is probably using automatic (stack) storage which will have to
// be replaced to allow VisBufferAsync objects (which derive from VisBuffer) to be
// used with asynchronous I/O.  The goal of this class is to make that transition
// easier.  The user will replace their existing declaration of a VisBuffer object
// with a declaration of a VisBufferAutoPtr object.  Depending on the attributes
// of the VisBuffer reference/pointer or the ROVisibilityIterator provided in the
// VisBufferAutoPtr constructor, the appropriate type of VisBuffer will be created
// dynamically.  The VisBufferAutoPtr will also function somewhat like an auto_ptr
// and delete the underlying object when the VisBufferAutoPtr object is destroyed.
//
// Once the straight VisBuffer declaration is replaced, then the code in its scope
// will need to be modified to dereference the VisBufferAutoPtr or to delete use
// of the address-of operator, "&", applied to the previous VisBuffer variable.
// See the example below.
//
// </synopsis>
// <example>
//
// <code>
// // Before adding asynchronous I/O support
//
// VisBuffer vb (vi);
//
// doSomething (vb);        // void doSomething (VisBuffer &);
// doSomethingElse (& vb);  // void doSomethingElse (VisBuffer *);
//
// // After adding asynchronous I/O support
//
// VisBufferAutoPtr vb (vi);
//
// doSomething (* vb);
// doSomethingElse (vb.get());
//
// </code>
// </example>
//
class VisBufferAutoPtr {

public:

    VisBufferAutoPtr ();
    VisBufferAutoPtr (VisBufferAutoPtr & other);
    explicit VisBufferAutoPtr (VisBuffer &);
    explicit VisBufferAutoPtr (VisBuffer *);
    explicit VisBufferAutoPtr (ROVisibilityIterator * rovi);
    explicit VisBufferAutoPtr (ROVisibilityIterator & rovi);
    ~VisBufferAutoPtr ();

    VisBufferAutoPtr & operator= (VisBufferAutoPtr & other);
    VisBuffer & operator* () const;
    VisBuffer * operator-> () const;

    VisBuffer * get () const;
    VisBuffer * release ();
    void set (VisBuffer &);
    void set (VisBuffer *);
    void set (ROVisibilityIterator * rovi, casacore::Bool attachIt = false);
    void set (ROVisibilityIterator & rovi, casacore::Bool attachIt = false);

protected:

    void construct (ROVisibilityIterator * rovi, casacore::Bool attachVi);
    void constructVb (VisBuffer * rovi);

private:

    VisBuffer * visBuffer_p;

};

} //# NAMESPACE CASA - END

#ifndef AIPS_NO_TEMPLATE_SRC
#include <msvis/MSVis/VisBuffer.tcc>
#endif //# AIPS_NO_TEMPLATE_SRC

#endif

