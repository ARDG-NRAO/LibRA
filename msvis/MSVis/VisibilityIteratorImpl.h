//# VisibilityIterator.h: Step through the MeasurementEquation by visibility
//# Copyright (C) 1996,1997,1998,1999,2000,2001,2002,2003
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
//# $Id: VisibilityIterator.h,v 19.14 2006/02/28 04:48:58 mvoronko Exp $

#ifndef MSVIS_VISIBILITYITERATORIMPL_H
#define MSVIS_VISIBILITYITERATORIMPL_H

#include <stack>
#include <casa/aips.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/Cube.h>
#include <casa/Arrays/Slicer.h>
#include <ms/MeasurementSets/MeasurementSet.h>
#include <measures/Measures/Stokes.h>
#include <measures/Measures/MeasConvert.h>
#include <casa/Quanta/MVDoppler.h>
#include <measures/Measures/MCDoppler.h>
#include <measures/Measures/MDoppler.h>
#include <tables/Tables/ArrayColumn.h>
#include <tables/Tables/ScalarColumn.h>
#include <casa/BasicSL/String.h>
#include <scimath/Mathematics/SquareMatrix.h>
#include <scimath/Mathematics/RigidVector.h>

#include <ms/MSOper/MSDerivedValues.h>
#include <msvis/MSVis/StokesVector.h>
#include <msvis/MSVis/VisImagingWeight.h>
#include <msvis/MSVis/VisibilityIterator.h>
#include <msvis/MSVis/VisBufferComponents.h>
#include <ms/MeasurementSets/MSIter.h>

#include <map>
#include <vector>

namespace casa { //# NAMESPACE CASA - BEGIN

//# forward decl
class VisBuffer;



// <summary>
// ROVisibilityIterator iterates through one or more readonly MeasurementSets
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>

// <prerequisite>
//   <li> <linkto class="MSIter">MSIter</linkto>
//   <li> <linkto class="casacore::MeasurementSet">casacore::MeasurementSet</linkto>
//   <li> <linkto class="VisSet">VisSet</linkto>
// </prerequisite>
//
// <etymology>
// The ROVisibilityIterator is a readonly iterator returning visibilities
// </etymology>
//
// <synopsis>
// ROVisibilityIterator provides iteration with various sort orders
// for one or more MSs. It has member functions to retrieve the fields
// commonly needed in synthesis calibration and imaging.
//
// One should use <linkto class="VisBuffer">VisBuffer</linkto>
// to access chunks of data.
// </synopsis>
//
// <example>
// <code>
// //
// </code>
// </example>
//
// <motivation>
// For imaging and calibration you need to access an casacore::MS in some consistent
// order (by field, spectralwindow, time interval etc.). This class provides
// that access.
// </motivation>
//
// <thrown>
//    <li>
//    <li>
// </thrown>
//
// <todo asof="1997/05/30">
//   <li> cleanup the currently dual interface for visibilities and flags
//   <li> sort out what to do with weights when interpolating
// </todo>

class VisibilityIteratorReadImpl {

    friend class AsyncEnabler;
    friend class ViReadImplAsync;
    friend class VisibilityIteratorWriteImpl;
    friend class ROVisibilityIterator;
    friend class ROVisIterator;
    friend class VisIterator;
    friend class VLAT; // allow VI lookahead thread class to access protected functions
    // VLAT should not access private parts, especially variables

public:

    //# the following is a copy of the enum in MSIter
    //# can't think of a way to get one that known to the outside world from here
    //  enum PolFrame {
    //    // Circular polarization
    //    Circular=0,
    //    // Linear polarization
    //    Linear=1
    //  };
    //
    //  enum DataColumn {
    //    // Observed data
    //    Observed=0,
    //    // Model data
    //    Model,
    //    // Corrected data
    //    Corrected
    //  };

    typedef ROVisibilityIterator::DataColumn DataColumn;

    //    class AsyncEnabler {
    //    public:
    //        AsyncEnabler (ROVisibilityIterator &);
    //        AsyncEnabler (ROVisibilityIterator *);
    //        ~AsyncEnabler ();
    //
    //        void release ();
    //
    //    private:
    //
    //        casacore::Bool oldEnabledState_p;
    //        ROVisibilityIterator * roVisibilityIterator_p;
    //    };

    // Default constructor - useful only to assign another iterator later
    VisibilityIteratorReadImpl ();

    // Construct from an casacore::MS and a casacore::Block of casacore::MS column enums specifying the
    // iteration order.  If no order is specified, it uses the default sort
    // order of MSIter, which is not necessarily the raw order of ms!
    // The default ordering is ARRAY_ID, FIELD_ID, DATA_DESC_ID,
    // and TIME, but check MSIter.h to be sure.
    // These columns will be added first if they are not specified.
    //
    // An optional timeInterval (in seconds) can be given to iterate through
    // chunks of time.  The default interval of 0 groups all times together.
    // Every 'chunk' of data contains all data within a certain time interval and
    // with identical values of the other iteration columns (e.g.  DATA_DESC_ID
    // and FIELD_ID).  Using selectChannel (), a number of groups of channels can
    // be requested.  At present the channel group iteration will always occur
    // before the interval iteration.
//    VisibilityIteratorReadImpl (const casacore::MeasurementSet & ms,
//                                const casacore::Block<casacore::Int> & sortColumns,
//                                casacore::Double timeInterval = 0);

    // Same as above, but with the option of using the raw order of ms
    // (addDefaultSortCols=false).
//    VisibilityIteratorReadImpl (const casacore::MeasurementSet & ms,
//                                const casacore::Block<casacore::Int> & sortColumns, const casacore::Bool addDefaultSortCols,
//                                casacore::Double timeInterval = 0);

    // Same as previous constructors, but with multiple MSs to iterate over.
//    VisibilityIteratorReadImpl (const casacore::Block<casacore::MeasurementSet> & mss,
//                                const casacore::Block<casacore::Int> & sortColumns,
//                                casacore::Double timeInterval = 0);

    VisibilityIteratorReadImpl (ROVisibilityIterator * rovi,
                                const casacore::Block<casacore::MeasurementSet> & mss,
                                const casacore::Block<casacore::Int> & sortColumns,
                                const casacore::Bool addDefaultSortCols,
                                casacore::Double timeInterval);

    // Copy construct. This calls the assigment operator.
    VisibilityIteratorReadImpl (const VisibilityIteratorReadImpl & other,
                                ROVisibilityIterator * rovi);

    // Assigment. Any attached VisBuffers are lost in the assign.
    VisibilityIteratorReadImpl & operator= (const VisibilityIteratorReadImpl & other);

    // Destructor
    virtual ~VisibilityIteratorReadImpl ();

    virtual VisibilityIteratorReadImpl * clone (ROVisibilityIterator * rovi) const;

    // Members

    casacore::Bool isAsyncEnabled () const;

    virtual casacore::Bool isWritable () const {
        return false;
    }

    // Reset iterator to origin/start of data (of current chunk)
    virtual void origin ();
    // Reset iterator to true start of data (first chunk)
    virtual void originChunks ();

    // Return the time interval (in seconds) used for iteration.
    // This is not the same as the INTERVAL column.
    virtual casacore::Double getInterval() const { return timeInterval_p; }

    // Set or reset the time interval (in seconds) to use for iteration.
    // You should call originChunks () to reset the iteration after
    // calling this.
    virtual void setInterval (casacore::Double timeInterval) {
        msIter_p.setInterval (timeInterval);
        timeInterval_p = timeInterval;
    }

    // Set the 'blocking' size for returning data.
    // With the default (0) only a single integration is returned at a time, this
    // is what is currently required for the calibration software. With blocking
    // set, up to nRows can be returned in one go. The chunk
    // size determines the actual maximum.
    virtual void setRowBlocking (casacore::Int nRows = 0);

    virtual casacore::Bool existsColumn (VisBufferComponents::EnumType id) const;

    // Return false if no more data (in current chunk)
    virtual casacore::Bool more () const;

    virtual SubChunkPair getSubchunkId () const;

    virtual const casacore::Block<casacore::Int>& getSortColumns() const;

    // Return false if no more 'Chunks' of data left
    virtual casacore::Bool moreChunks () const {
        return msIter_p.more ();
    }

    // Check if ms has change since last iteration
    virtual casacore::Bool newMS () const {
        return msIter_p.newMS ();
    }

    virtual casacore::Int msId () const {
        return msIter_p.msId ();
    }

    virtual VisBuffer * getVisBuffer ();

    //reference to actual ms in interator
    virtual const casacore::MeasurementSet & ms () const {
        return msIter_p.ms ();
    }
    // Advance to the next Chunk of data
    virtual VisibilityIteratorReadImpl & nextChunk ();

    // Return antenna1
    virtual casacore::Vector<casacore::Int> & antenna1 (casacore::Vector<casacore::Int> & ant1) const;

    // Return antenna2
    virtual casacore::Vector<casacore::Int> & antenna2 (casacore::Vector<casacore::Int> & ant2) const;

    // Return feed1
    virtual casacore::Vector<casacore::Int> & feed1 (casacore::Vector<casacore::Int> & fd1) const;

    // Return feed2
    virtual casacore::Vector<casacore::Int> & feed2 (casacore::Vector<casacore::Int> & fd2) const;

    // Return channel numbers in selected VisSet spectrum
    // (i.e. disregarding possible selection on the iterator, but
    //  including the selection set when creating the VisSet)
    virtual casacore::Vector<casacore::Int> & channel (casacore::Vector<casacore::Int> & chan) const;

    // Return feed configuration matrix for specified antenna
    virtual casacore::Vector<casacore::SquareMatrix<casacore::Complex, 2> > &
    CJones (casacore::Vector<casacore::SquareMatrix<casacore::Complex, 2> > & cjones) const;

    // Return receptor angles for all antennae and feeds
    // First axis of the cube is a receptor number,
    // 2nd is antennaId, 3rd is feedId
    // Note: the method is intended to provide an access to MSIter::receptorAngles
    // for VisBuffer in the multi-feed case. It may be worth to change the
    // interface of feed_pa to return the information for all feeds.
    virtual const casacore::Cube<casacore::Double> & receptorAngles () const;

    // return a string mount identifier for each antenna
    virtual const casacore::Vector<casacore::String> & antennaMounts () const;

    // Return a cube containing pairs of coordinate offsets for each
    // receptor of each feed (values are in radians, coordinate system is fixed
    // with antenna and is the same one as used to define the BEAM_OFFSET
    // parameter in the feed table). The cube axes are receptor, antenna, feed.
    virtual const casacore::Cube<casacore::RigidVector<casacore::Double, 2> > & getBeamOffsets () const;

    // true if all elements of the cube returned by getBeamOffsets are zero
    virtual casacore::Bool allBeamOffsetsZero () const;

    // Return feed parallactic angles casacore::Vector (nant) (1 feed/ant)
    virtual casacore::Vector<casacore::Float> feed_pa (casacore::Double time) const;
    static casacore::Vector<casacore::Float> feed_paCalculate (casacore::Double time, casacore::MSDerivedValues & msd,
                                           casacore::Int nAntennas, const casacore::MEpoch & mEpoch0,
                                           const casacore::Vector<casacore::Float> & receptor0Angle);

    // Return nominal parallactic angle at specified time
    // (does not include feed position angle offset--see feed_pa)
    // A global value for all antennas (e.g., small array)
    virtual const casacore::Float & parang0 (casacore::Double time) const;
    static casacore::Float parang0Calculate (casacore::Double time, casacore::MSDerivedValues & msd, const casacore::MEpoch & epoch0);

    // Per antenna:
    virtual casacore::Vector<casacore::Float> parang (casacore::Double time) const;
    static casacore::Vector<casacore::Float> parangCalculate (casacore::Double time, casacore::MSDerivedValues & msd,
                                          int nAntennas, const casacore::MEpoch mEpoch0);

    // Return the antenna AZ/EL casacore::Vector (nant)
    virtual casacore::MDirection azel0 (casacore::Double time) const;
    static void azel0Calculate (casacore::Double time, casacore::MSDerivedValues & msd,
                                casacore::MDirection & azel0, const casacore::MEpoch & mEpoch0);

    virtual casacore::Vector<casacore::MDirection> azel (casacore::Double time) const;
    static void azelCalculate (casacore::Double time, casacore::MSDerivedValues & msd, casacore::Vector<casacore::MDirection> & azel,
                               casacore::Int nAnt, const casacore::MEpoch & mEpoch0);


    // Return the hour angle for the specified time
    virtual casacore::Double hourang (casacore::Double time) const;
    static casacore::Double hourangCalculate (casacore::Double time, casacore::MSDerivedValues & msd, const casacore::MEpoch & mEpoch0);

    // Return the current FieldId
    virtual casacore::Int fieldId () const {
        return msIter_p.fieldId ();
    }

    // Return the current ArrayId
    virtual casacore::Int arrayId () const {
        return msIter_p.arrayId ();
    }

    // Return the current Field Name
    virtual casacore::String fieldName () const {
        return msIter_p.fieldName ();
    }

    // Return the current Source Name
    virtual casacore::String sourceName () const {
        return msIter_p.sourceName ();
    }

    // Return flag for each polarization, channel and row
    virtual casacore::Cube<casacore::Bool> & flag (casacore::Cube<casacore::Bool> & flags) const;

    // Return flag for each channel & row
    virtual casacore::Matrix<casacore::Bool> & flag (casacore::Matrix<casacore::Bool> & flags) const;

    // Determine whether FLAG_CATEGORY is valid.
    casacore::Bool existsFlagCategory() const;

    // Return flags for each polarization, channel, category, and row.
    virtual casacore::Array<casacore::Bool> & flagCategory (casacore::Array<casacore::Bool> & flagCategories) const;

    // Return row flag
    virtual casacore::Vector<casacore::Bool> & flagRow (casacore::Vector<casacore::Bool> & rowflags) const;

    // Return scan number
    virtual casacore::Vector<casacore::Int> & scan (casacore::Vector<casacore::Int> & scans) const;

    // Return the OBSERVATION_IDs
    virtual casacore::Vector<casacore::Int> & observationId (casacore::Vector<casacore::Int> & obsids) const;

    // Return the PROCESSOR_IDs
    virtual casacore::Vector<casacore::Int> & processorId (casacore::Vector<casacore::Int> & procids) const;

    // Return the STATE_IDs
    virtual casacore::Vector<casacore::Int> & stateId (casacore::Vector<casacore::Int> & stateids) const;

    // Return current frequencies (in Hz, acc. to the casacore::MS def'n v.2)
    virtual casacore::Vector<casacore::Double> & frequency (casacore::Vector<casacore::Double> & freq) const;

    // Return frequencies  (in Hz, acc. to the casacore::MS def'n v.2) in selected velocity frame,
    // returns the same as frequency () if there is no vel selection active.
    virtual casacore::Vector<casacore::Double> & lsrFrequency (casacore::Vector<casacore::Double> & freq) const;

    // Return the current phase center as an MDirection
    virtual const casacore::MDirection & phaseCenter () const {
        return msIter_p.phaseCenter ();
    }
    
    virtual const casacore::MDirection  phaseCenter (const casacore::Int fieldId, const casacore::Double time=-1.0) const {
      return time >0.0 ? msIter_p.phaseCenter (fieldId, time) : msIter_p.phaseCenter () ;
    }
    // Return frame for polarization (returns PolFrame enum)
    virtual casacore::Int polFrame () const {
        return msIter_p.polFrame ();
    }

    // Return the correlation type (returns casacore::Stokes enums)
    virtual casacore::Vector<casacore::Int> & corrType (casacore::Vector<casacore::Int> & corrTypes) const;

    // Return sigma
    virtual casacore::Vector<casacore::Float> & sigma (casacore::Vector<casacore::Float> & sig) const;

    // Return sigma matrix (pol-dep)
    virtual casacore::Matrix<casacore::Float> & sigmaMat (casacore::Matrix<casacore::Float> & sigmat) const;

    // Return current SpectralWindow
    virtual casacore::Int spectralWindow () const {
        return msIter_p.spectralWindowId ();
    }

    // Return current Polarization Id
    virtual casacore::Int polarizationId () const {
        return msIter_p.polarizationId ();
    }

    // Return current DataDescription Id
    virtual casacore::Int dataDescriptionId () const {
        return msIter_p.dataDescriptionId ();
    }

    // Return MJD midpoint of interval.
    virtual casacore::Vector<casacore::Double> & time (casacore::Vector<casacore::Double> & t) const;

    // Return MJD centroid of interval.
    virtual casacore::Vector<casacore::Double> & timeCentroid (casacore::Vector<casacore::Double> & t) const;

    // Return nominal time interval
    virtual casacore::Vector<casacore::Double> & timeInterval (casacore::Vector<casacore::Double> & ti) const;

    // Return actual time interval
    virtual casacore::Vector<casacore::Double> & exposure (casacore::Vector<casacore::Double> & expo) const;

    // Return the visibilities as found in the casacore::MS, casacore::Cube (npol,nchan,nrow).
    virtual casacore::Cube<casacore::Complex> & visibility (casacore::Cube<casacore::Complex> & vis,
                                        DataColumn whichOne) const;

    // Return FLOAT_DATA as a casacore::Cube (npol, nchan, nrow) if found in the MS.
    virtual casacore::Cube<casacore::Float> & floatData (casacore::Cube<casacore::Float> & fcube) const;

    // Return the visibility 4-vector of polarizations for each channel.
    // If the casacore::MS doesn't contain all polarizations, it is assumed it
    // contains one or two parallel hand polarizations.
    virtual casacore::Matrix<CStokesVector> & visibility (casacore::Matrix<CStokesVector> & vis,
                                                DataColumn whichOne) const;

    // Return the shape of the visibility Cube
    virtual casacore::IPosition visibilityShape () const;

    // Return u,v and w (in meters)
    virtual casacore::Vector<casacore::RigidVector<casacore::Double, 3> > & uvw (casacore::Vector<casacore::RigidVector<casacore::Double, 3> > & uvwvec) const;
    virtual casacore::Matrix<casacore::Double> & uvwMat (casacore::Matrix<casacore::Double> & uvwmat) const;

    // Return weight
    virtual casacore::Vector<casacore::Float> & weight (casacore::Vector<casacore::Float> & wt) const;

    // Returns the nPol_p x curNumRow_p weight matrix
    virtual casacore::Matrix<casacore::Float> & weightMat (casacore::Matrix<casacore::Float> & wtmat) const;

    // Determine whether WEIGHT_SPECTRUM exists.
    casacore::Bool existsWeightSpectrum () const;

    // Return weightspectrum (a weight for each channel)
    virtual casacore::Cube<casacore::Float> & weightSpectrum (casacore::Cube<casacore::Float> & wtsp) const;

    // Return imaging weight (a weight for each channel)
    //virtual casacore::Matrix<casacore::Float> & imagingWeight (casacore::Matrix<casacore::Float> & wt) const;
    const VisImagingWeight & getImagingWeightGenerator () const;

    // Return true if FieldId/Source has changed since last iteration
    virtual casacore::Bool newFieldId () const {
        return (curStartRow_p == 0 && msIter_p.newField ());
    }

    // Return true if arrayID has changed since last iteration
    virtual casacore::Bool newArrayId () const {
        return (curStartRow_p == 0 && msIter_p.newArray ());
    }

    // Return true if SpectralWindow has changed since last iteration
    virtual casacore::Bool newSpectralWindow () const {
        return (curStartRow_p == 0 && msIter_p.newSpectralWindow ());
    }

    // Return the index of the first channel of the current channel group
    // in the total (selected) spectrum.
    virtual casacore::Int channelIndex () const;

    // Return the width of the current group of channels, i.e.,
    // the number of channels returned by visibility () and frequency ().
    virtual casacore::Int channelGroupSize () const;

    // Return the number of correlations in the current iteration
    virtual casacore::Int nCorr () const {
        return nPol_p;
    };

    // Return the number of rows in the current iteration
    virtual casacore::Int nRow () const;

    // Return the row ids as from the original root table. This is useful
    // to find correspondance between a given row in this iteration to the
    // original ms row
    virtual casacore::Vector<casacore::rownr_t> & rowIds (casacore::Vector<casacore::rownr_t> & rowids) const;

    // Return the numbers of rows in the current chunk
    virtual casacore::Int nRowChunk () const;

    // Return the number of sub-intervals in the current chunk
    virtual casacore::Int nSubInterval () const;

    // Call to use the slurp i/o method for all scalar columns. This
    // will set the casacore::BucketCache cache size to the full column length
    // and cause the full column to be cached in memory, if
    // any value of the column is used. In case of out-of-memory,
    // it will automatically fall-back on the smaller cache size.
    // Slurping the column is to be considered as a work-around for the
    // casacore::Table i/o code, which uses casacore::BucketCache and performs extremely bad
    // for random access. Slurping is useful when iterating non-sequentially
    // an casacore::MS or parts of an casacore::MS, it is not tested with multiple MSs.
    virtual void slurp () const;

    // Velocity selection - specify the output channels in velocity:
    // nChan - number of output channels, vStart - start velocity,
    // vInc - velocity increment. So channel i will have velocity
    // vStart + i*vInc (i=0,nChan-1).
    // Specify velocities as in e.g., casacore::MVRadialVelocity (casacore::Quantity (2001.,"km/s")).
    // The reference type and velocity definition are specified separately.
    // Note that no averaging is performed, the visibilities will be interpolated
    // and sampled at the specified velocities, it's up to you to choose a vInc
    // appropriate to the channel width.
    // The REST_FREQUENCY column in the SPECTRAL_WINDOW subtable is used to
    // determine the velocity-frequency conversion.
    // By default calculations are done for a single velocity with offsets
    // applied for the others (ok for non-rel velocities with RADIO defn),
    // set precise to true to do a full conversion for each output channel. (NYI)
    virtual VisibilityIteratorReadImpl &
    selectVelocity (casacore::Int nChan,
                    const casacore::MVRadialVelocity & vStart, const casacore::MVRadialVelocity & vInc,
                    casacore::MRadialVelocity::Types rvType = casacore::MRadialVelocity::LSR,
                    casacore::MDoppler::Types dType = casacore::MDoppler::RADIO, casacore::Bool precise = false);

    // Select the velocity interpolation scheme.
    // At present the choice is limited to : nearest and linear, linear
    // is the default.
    // TODO: add cubic, spline and possibly FFT
    virtual VisibilityIteratorReadImpl & velInterpolation (const casacore::String & /*type*/) {return * this; }

    // Channel selection - only the selected channels will be returned by the
    // access functions. The default spectralWindow is the current one (or 0)
    // This allows selection of the input channels, producing
    // nGroup groups of width output channels. Default is to return all channels
    // in a single group.
    virtual VisibilityIteratorReadImpl & selectChannel (casacore::Int nGroup = 1, casacore::Int start = 0, casacore::Int width = 0,
                                                        casacore::Int increment = 1, casacore::Int spectralWindow = -1);

    //Same as above except when multiple ms's are to be accessed

    virtual VisibilityIteratorReadImpl & selectChannel (const casacore::Block< casacore::Vector<casacore::Int> > & blockNGroup,
                                                        const casacore::Block< casacore::Vector<casacore::Int> > & blockStart,
                                                        const casacore::Block< casacore::Vector<casacore::Int> > & blockWidth,
                                                        const casacore::Block< casacore::Vector<casacore::Int> > & blockIncr,
                                                        const casacore::Block< casacore::Vector<casacore::Int> > & blockSpw);


    //get the channel selection ...the block over the number of ms's associated
    // with this iterator
    virtual void getChannelSelection (casacore::Block< casacore::Vector<casacore::Int> > & blockNGroup,
                                      casacore::Block< casacore::Vector<casacore::Int> > & blockStart,
                                      casacore::Block< casacore::Vector<casacore::Int> > & blockWidth,
                                      casacore::Block< casacore::Vector<casacore::Int> > & blockIncr,
                                      casacore::Block< casacore::Vector<casacore::Int> > & blockSpw);

    // Translate slicesv from the form returned by casacore::MSSelection::getChanSlices ()
    // to matv as used by setChanAveBounds ().  widthsv is the channel averaging
    // width for each _selected_ spw.
    void slicesToMatrices (casacore::Vector<casacore::Matrix<casacore::Int> > & matv,
                           const casacore::Vector<casacore::Vector<casacore::Slice> > & slicesv,
                           const casacore::Vector<casacore::Int> & widthsv) const;

    // Get the spw, start  and nchan for all the ms's is this Visiter that
    // match the frequecy "freqstart-freqStep" and "freqEnd+freqStep" range
    // Can help in doing channel selection above..
    // freqFrame is the frame the caller frequency values are in (freqStart and freqEnd)
    // These will be converted to the frame of the selected spw to match

    virtual void getSpwInFreqRange (casacore::Block<casacore::Vector<casacore::Int> > & spw,
                                    casacore::Block<casacore::Vector<casacore::Int> > & start,
                                    casacore::Block<casacore::Vector<casacore::Int> > & nchan,
                                    casacore::Double freqStart, casacore::Double freqEnd,
                                    casacore::Double freqStep, casacore::MFrequency::Types freqFrame = casacore::MFrequency::LSRK) const;

    // Get the range of frequency convered by the selected data in the frame requested

    virtual void getFreqInSpwRange(casacore::Double& freqStart, casacore::Double& freqEnd, casacore::MFrequency::Types freqframe = casacore::MFrequency::LSRK) const;

    // Attach a VisBuffer object.
    // Note that while more than one VisBuffer may be attached, only the
    // last one is actively updated. A casacore::Stack is kept internally, so after
    // a detach, the previous VisBuffer becomes active again.
    virtual void attachVisBuffer (VisBuffer & vb);

    // Detach a VisBuffer object.
    // If the object detached is not the last one attached an exception
    // is thrown.
    virtual void detachVisBuffer (VisBuffer & vb);

    // Access the current casacore::MSColumns object in MSIter
    virtual const casacore::MSColumns & msColumns () const {
        return msIter_p.msColumns ();
    }

    // get back the selected spectral windows and spectral channels for
    // current ms

    virtual void allSelectedSpectralWindows (casacore::Vector<casacore::Int> & spws, casacore::Vector<casacore::Int> & nvischan);

    // Convert the frequency from the observe frame to lsr frame.
    // Returns true in convert if given spw was not observed
    // in the LSRK frame
    // if ignoreconv=true then conversion from frame in data 
    //is ignored by request
    virtual void lsrFrequency (const casacore::Int & spw, casacore::Vector<casacore::Double> & freq, casacore::Bool & convert, const casacore::Bool ignoreconv=false);
    //assign a VisImagingWeight object to this iterator
    virtual void useImagingWeight (const VisImagingWeight & imWgt);
    //return number  of Ant
    virtual casacore::Int numberAnt ();
    //Return number of rows in all selected ms's
    virtual casacore::Int numberCoh ();

    // Return number of spws, polids, ddids
    virtual casacore::Int numberSpw ();
    virtual casacore::Int numberPol ();
    virtual casacore::Int numberDDId ();

    casacore::ArrayColumn <casacore::Double> & getChannelFrequency () const;
    casacore::Block<casacore::Int> getChannelGroupNumber () const;
    casacore::Block<casacore::Int> getChannelIncrement () const;
    casacore::Block<casacore::Int> getChannelStart () const;
    casacore::Block<casacore::Int> getChannelWidth () const;
    casacore::Int getDataDescriptionId () const;
    const casacore::MeasurementSet & getMeasurementSet () const;;
    casacore::Int getMeasurementSetId () const;
    casacore::Int getNAntennas () const;
    virtual casacore::MEpoch getEpoch () const;
    casacore::MFrequency::Types getObservatoryFrequencyType () const; //???
    casacore::MPosition getObservatoryPosition () const;
    casacore::MDirection getPhaseCenter () const;
    casacore::Vector<casacore::Float> getReceptor0Angle ();
    casacore::Vector<casacore::rownr_t> getRowIds () const;

    static void lsrFrequency (const casacore::Int & spw,
                              casacore::Vector<casacore::Double> & freq,
                              casacore::Bool & convert,
                              const casacore::Block<casacore::Int> & chanStart,
                              const casacore::Block<casacore::Int> & chanWidth,
                              const casacore::Block<casacore::Int> & chanInc,
                              const casacore::Block<casacore::Int> & numChanGroup,
                              const casacore::ArrayColumn <casacore::Double> & chanFreqs,
                              const casacore::ScalarColumn<casacore::Int> & obsMFreqTypes,
                              const casacore::MEpoch & ep,
                              const casacore::MPosition & obsPos,
                              const casacore::MDirection & dir,
			      const casacore::Bool ignoreconv=false);

protected:

    void attachColumnsSafe (const casacore::Table & t);

    // advance the iteration
    virtual void advance ();
    // set the currently selected table
    virtual void setSelTable ();
    // set the iteration state
    virtual void setState ();
    // get the TOPO frequencies from the selected velocities and the obs. vel.
    virtual void getTopoFreqs ();
    virtual void getTopoFreqs (casacore::Vector<casacore::Double> & lsrFreq, casacore::Vector<casacore::Double> & selFreq); // for async i/o

    virtual void getLsrInfo (casacore::Block<casacore::Int> & channelGroupNumber,
                             casacore::Block<casacore::Int> & channelIncrement,
                             casacore::Block<casacore::Int> & channelStart,
                             casacore::Block<casacore::Int> & channelWidth,
                             casacore::MPosition & observatoryPositon,
                             casacore::MDirection & phaseCenter,
                             casacore::Bool & velocitySelection) const;

    std::vector<casacore::MeasurementSet> getMeasurementSets () const;

    const casacore::MSDerivedValues & getMSD () const; // for use by Async I/O *ONLY*

    // update the DATA slicer
    virtual void updateSlicer ();
    // attach the column objects to the currently selected table
    virtual void attachColumns (const casacore::Table & t);
    // returns the table, to which columns are attached,
    // can be overridden in derived classes
    virtual const casacore::Table attachTable () const;

    // get the (velocity selected) interpolated visibilities, flags and weights.
    // It is not really const at all (it seems to use This-> trickery so callers
    // like flag () can be declared const).

//    virtual void getInterpolatedVisFlagWeight (DataColumn whichOne) const;

    // get the (velocity selected) interpolated FLOAT_DATA (as real Floats),
    // flags and weights.

//    void getInterpolatedFloatDataFlagWeight () const;

    casacore::Bool usesTiledDataManager (const casacore::String & columnName, const casacore::MeasurementSet & ms) const;

    // get the visibility data (observed, corrected or model);
    // deals with casacore::Float and casacore::Complex observed data (DATA or FLOAT_DATA)
    virtual void getDataColumn (ROVisibilityIterator::DataColumn whichOne, const casacore::Slicer & slicer,
                                casacore::Cube<casacore::Complex> & data) const;
    virtual void getDataColumn (DataColumn whichOne, casacore::Cube<casacore::Complex> & data) const;

    // get FLOAT_DATA as real Floats.
    virtual void getFloatDataColumn (const casacore::Slicer & slicer, casacore::Cube<casacore::Float> & data) const;
    virtual void getFloatDataColumn (casacore::Cube<casacore::Float> & data) const;

    //constructor helpers
    virtual void initialize (const casacore::Block<casacore::MeasurementSet> & mss);

    virtual void originChunks (casacore::Bool forceRewind);

    //Re-Do the channel selection in multi ms case
    virtual void doChannelSelection ();
    //Set the tile cache size....when using slice access if tile cache size is
    // not set memory usage can go wild.  Specifically, the caching scheme is
    // ephemeral and lives for that instance of setting the caching scheme.
    //
    // If you don't set any then the defaults come into play and caches a few
    // tiles along every axis at the tile you requested...which is a waste when
    // say you know you want to proceed along the row axis for example...and in
    // fact now VisIter just reads one tile (thus the commenting in setTileCache)
    // and lets the OS do the caching rather than than having the table system
    // cache extra tiles.
    virtual void setTileCache ();
    //Check if spw is in selected SPW for actual ms
    virtual casacore::Bool isInSelectedSPW (const casacore::Int & spw);

    // Updates, if necessary, rowIds_p member for the current chunk
    virtual void update_rowIds () const;

    void setAsyncEnabled (casacore::Bool enable);

    template<class T>
    void getColScalar (const casacore::ScalarColumn<T> & column, casacore::Vector<T> & array, casacore::Bool resize) const;

    template<class T>
    void getColArray (const casacore::ArrayColumn<T> & column, casacore::Array<T> & array, casacore::Bool resize) const;

    // column access functions, can be overridden in derived classes
    virtual void getCol (const casacore::ScalarColumn<casacore::Bool> & column, casacore::Vector<casacore::Bool> & array, casacore::Bool resize = false) const;
    virtual void getCol (const casacore::ScalarColumn<casacore::Int> & column, casacore::Vector<casacore::Int> & array, casacore::Bool resize = false) const;
    virtual void getCol (const casacore::ScalarColumn<casacore::Double> & column, casacore::Vector<casacore::Double> & array, casacore::Bool resize = false) const;

    virtual void getCol (const casacore::ArrayColumn<casacore::Bool> & column, casacore::Array<casacore::Bool> & array, casacore::Bool resize = false) const;
    virtual void getCol (const casacore::ArrayColumn<casacore::Float> & column, casacore::Array<casacore::Float> & array, casacore::Bool resize = false) const;
    virtual void getCol (const casacore::ArrayColumn<casacore::Double> & column, casacore::Array<casacore::Double> & array, casacore::Bool resize = false) const;
    virtual void getCol (const casacore::ArrayColumn<casacore::Complex> & column, casacore::Array<casacore::Complex> & array, casacore::Bool resize = false) const;

    virtual void getCol (const casacore::ArrayColumn<casacore::Bool> & column, const casacore::Slicer & slicer, casacore::Array<casacore::Bool> & array, casacore::Bool resize = false) const;
    virtual void getCol (const casacore::ArrayColumn<casacore::Float> & column, const casacore::Slicer & slicer, casacore::Array<casacore::Float> & array, casacore::Bool resize = false) const;
    virtual void getCol (const casacore::ArrayColumn<casacore::Complex> & column, const casacore::Slicer & slicer, casacore::Array<casacore::Complex> & array, casacore::Bool resize = false) const;

    //  virtual void getCol (const casacore::String & colName, casacore::Array<casacore::Double> & array,
    //                      casacore::Array<casacore::Double> & all, casacore::Bool resize = false) const;
    //  virtual void getCol (const casacore::String & colName, casacore::Vector<casacore::Bool> & array,
    //                      casacore::Vector<casacore::Bool> & all, casacore::Bool resize = false) const;
    //  virtual void getCol (const casacore::String & colName, casacore::Vector<casacore::Int> & array,
    //                      casacore::Vector<casacore::Int> & all, casacore::Bool resize = false) const;
    //  virtual void getCol (const casacore::String & colName, casacore::Vector<casacore::Double> & array,
    //                      casacore::Vector<casacore::Double> & all, casacore::Bool resize = false) const;

    template<class T>
    void swapyz (casacore::Cube<T> & out, const casacore::Cube<T> & in) const;

    class Cache {

    public:

        Cache ();

        Cache & operator= (const Cache & other);

        casacore::MDirection         azel0_p;
        casacore::Vector<casacore::MDirection> azel_p;
        casacore::Vector<casacore::Float>      feedpa_p;
        casacore::Cube<casacore::Bool>         flagCube_p;
        casacore::Bool               flagOK_p;
        casacore::Bool               floatDataCubeOK_p;
        casacore::Cube<casacore::Float>        floatDataCube_p;
        casacore::Bool               freqCacheOK_p;
        casacore::Vector<casacore::Double>     frequency_p;
        casacore::Double             hourang_p;
        casacore::Matrix<casacore::Float>      imagingWeight_p;
        casacore::Double             lastParang0UT_p; // cache update timestamp
        casacore::Double             lastParangUT_p; // cache update timestamp
        casacore::Double             lastazelUT_p; // cache update timestamp
        casacore::Double             lastazel0UT_p; // cache update timestamp
        casacore::Double             lasthourangUT_p; // cache update timestamp
        casacore::Double             lastfeedpaUT_p; // cache update timestamp
        casacore::Bool               msHasFC_p;   // Does the current casacore::MS have a valid FLAG_CATEGORY?
        casacore::Bool               msHasWtSp_p; // Does the current casacore::MS have a valid WEIGHT_SPECTRUM?
        casacore::Float              parang0_p;
        casacore::Vector<casacore::Float>      parang_p;
        casacore::Vector<casacore::rownr_t>    rowIds_p;
        casacore::Matrix<casacore::Double>     uvwMat_p;
        casacore::Cube<casacore::Complex>      visCube_p;
        casacore::Block<casacore::Bool>        visOK_p;
        casacore::Bool               weightSpOK_p;
        casacore::Cube<casacore::Float>        wtSp_p;
    };

    class Channels { // channel selection

    public:

        casacore::Block<casacore::Int> inc_p;
        casacore::Block<casacore::Int> start_p;
        casacore::Block<casacore::Int> width_p;
        casacore::Block<casacore::Int> nGroups_p;
        casacore::Block<casacore::Int> preselectedChanStart_p;
        casacore::Block<casacore::Int> preselectednChan_p;

    };

    class Columns {

    public:

        Columns & operator= (const Columns & other);

        casacore::ScalarColumn<casacore::Int>    antenna1_p;
        casacore::ScalarColumn<casacore::Int>    antenna2_p;
        casacore::ArrayColumn<casacore::Complex> corrVis_p;
        casacore::ScalarColumn<casacore::Double> exposure_p;
        casacore::ScalarColumn<casacore::Int>    feed1_p;
        casacore::ScalarColumn<casacore::Int>    feed2_p;
        casacore::ArrayColumn<casacore::Bool>    flagCategory_p;
        casacore::ScalarColumn<casacore::Bool>   flagRow_p;
        casacore::ArrayColumn<casacore::Bool>    flag_p;
        casacore::ArrayColumn<casacore::Float>   floatVis_p;
        casacore::ArrayColumn<casacore::Complex> modelVis_p;
        casacore::ScalarColumn<casacore::Int>    observation_p;
        casacore::ScalarColumn<casacore::Int>    processor_p;
        casacore::ScalarColumn<casacore::Int>    scan_p;
        casacore::ArrayColumn<casacore::Float>   sigma_p;
        casacore::ScalarColumn<casacore::Int>    state_p;
        casacore::ScalarColumn<casacore::Double> timeCentroid_p;
        casacore::ScalarColumn<casacore::Double> timeInterval_p;
        casacore::ScalarColumn<casacore::Double> time_p;
        casacore::ArrayColumn<casacore::Double>  uvw_p;
        casacore::ArrayColumn<casacore::Complex> vis_p;
        casacore::ArrayColumn<casacore::Float>   weightSpectrum_p;
        casacore::ArrayColumn<casacore::Float>   weight_p;

    };

    class MeasurementSetChannels {

    public:

        casacore::Block < casacore::Vector<casacore::Int> > inc_p;
        casacore::Block < casacore::Vector<casacore::Int> > nGroups_p;
        casacore::Block < casacore::Vector<casacore::Int> > start_p;
        casacore::Block < casacore::Vector<casacore::Int> > width_p;
        casacore::Block < casacore::Vector<casacore::Int> > spw_p;

    };

    class Velocity { // for velocity selection and conversion

    public:

        Velocity ();
        Velocity & operator= (const Velocity & other);

        casacore::MDoppler::Convert cFromBETA_p;
        casacore::Vector<casacore::Double>    lsrFreq_p;
        casacore::Int               nVelChan_p;
        casacore::Vector<casacore::Double>    selFreq_p;
        casacore::MDoppler::Types   vDef_p;
        casacore::MVRadialVelocity  vInc_p;
        casacore::String            vInterpolation_p;
        casacore::Bool              velSelection_p;
        casacore::Bool              vPrecise_p;
        casacore::MVRadialVelocity  vStart_p;

    };


    typedef std::vector<casacore::MeasurementSet> MeasurementSets;

    casacore::Bool                    addDefaultSort_p;
    casacore::Bool                    asyncEnabled_p; // Allows lower-level code to make an async "copy" of this VI.
    casacore::Bool                    autoTileCacheSizing_p;
    mutable Cache           cache_p;
    casacore::Int                     channelGroupSize_p;
    Channels                channels_p;
    casacore::Int                     chunkNumber_p;
    Columns                 columns_p;
    casacore::Int                     curChanGroup_p;
    casacore::rownr_t                 curEndRow_p;
    casacore::Int                     curNGroups_p;
    casacore::rownr_t                 curNumRow_p;
    casacore::rownr_t                 curStartRow_p;
    casacore::rownr_t                 curTableNumRow_p;
    casacore::Bool                    floatDataFound_p;
    VisImagingWeight        imwgt_p;    // object to calculate imaging weight
    casacore::Bool                    initialized_p;
    casacore::Bool                    isMultiMS_p;
    MeasurementSets         measurementSets_p; // [use]
    casacore::Bool                    more_p;
    MeasurementSetChannels  msChannels_p;
    casacore::Int                     msCounter_p;
    casacore::Bool                    msIterAtOrigin_p;
    casacore::MSIter                  msIter_p;
    mutable casacore::MSDerivedValues msd_p;
    casacore::Int                     nAnt_p;
    casacore::Int                     nChan_p;
    casacore::Int                     nPol_p;
    casacore::Int                     nRowBlocking_p;
    casacore::Bool                    newChanGroup_p;
    ROVisibilityIterator *  rovi_p; // [use]
    casacore::RefRows                 selRows_p; // currently selected rows from msIter_p.table ()
    casacore::Slicer                  slicer_p;
    casacore::Block<casacore::Int>              sortColumns_p;
    casacore::Bool                    stateOk_p;
    SubChunkPair            subchunk_p;
    casacore::Vector<casacore::Bool>            tileCacheIsSet_p;
    casacore::Double                  timeInterval_p;
    casacore::Vector<casacore::Double>          time_p;
    casacore::Bool                    useSlicer_p;
    std::stack<VisBuffer *>      vbStack_p;   // casacore::Stack of VisBuffer objects
    Velocity                velocity_p;
    casacore::Slicer                  weightSlicer_p;


};

inline casacore::Bool VisibilityIteratorReadImpl::more () const
{
    return more_p;
}

inline casacore::Vector<casacore::SquareMatrix<casacore::Complex, 2> > &
VisibilityIteratorReadImpl::CJones (casacore::Vector<casacore::SquareMatrix<casacore::Complex, 2> > & cjones) const
{
    cjones.resize (msIter_p.CJones ().nelements ());
    return cjones = msIter_p.CJones ();
}

inline const casacore::Cube<casacore::Double> & VisibilityIteratorReadImpl::receptorAngles () const
{
    return msIter_p.receptorAngles ();
}

inline const casacore::Vector<casacore::String> & VisibilityIteratorReadImpl::antennaMounts () const
{
    return msIter_p.antennaMounts ();
}

inline const casacore::Cube<casacore::RigidVector<casacore::Double, 2> > &
VisibilityIteratorReadImpl::getBeamOffsets () const
{
    return msIter_p.getBeamOffsets ();
}

inline casacore::Bool VisibilityIteratorReadImpl::allBeamOffsetsZero () const
{
    return msIter_p.allBeamOffsetsZero ();
}

inline casacore::Int VisibilityIteratorReadImpl::channelGroupSize () const
{
    return channels_p.width_p[msIter_p.spectralWindowId ()];
}
inline casacore::Int VisibilityIteratorReadImpl::channelIndex () const
{
    return channels_p.inc_p[msIter_p.spectralWindowId ()] * curChanGroup_p;
}
inline casacore::Int VisibilityIteratorReadImpl::nRow () const
{
    return curNumRow_p;
}
inline casacore::Int VisibilityIteratorReadImpl::nRowChunk () const
{
    return msIter_p.table ().nrow ();
}
//inline VisibilityIteratorReadImpl &
//VisibilityIteratorReadImpl::velInterpolation (const casacore::String & type)
//{
//    vInterpolation_p = type;
//    return *this;
//}
inline casacore::IPosition VisibilityIteratorReadImpl::visibilityShape () const
{
    return casacore::IPosition (3, nPol_p, channelGroupSize (), curNumRow_p);
}

// <summary>
// VisibilityIterator iterates through one or more writable MeasurementSets
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>

// <prerequisite>
//   <li> <linkto class="VisibilityIteratorReadImpl">VisibilityIteratorReadImpl</linkto>
// </prerequisite>
//
// <etymology>
// The VisibilityIterator is a read/write iterator returning visibilities
// </etymology>
//
// <synopsis>
// VisibilityIterator provides iteration with various sort orders
// for one or more MSs. It has member functions to retrieve the fields
// commonly needed in synthesis calibration and imaging. It is
// derived from the read-only iterator
// <linkto class="VisibilityIteratorReadImpl">VisibilityIteratorReadImpl</linkto>.
//
// One should use <linkto class="VisBuffer">VisBuffer</linkto>
// to access chunks of data.
// </synopsis>
//
// <example>
// <code>
// //
// </code>
// </example>
//
// <motivation>
// For imaging and calibration you need to access an casacore::MS in some consistent
// order (by field, spectralwindow, time interval etc.). This class provides
// that access.
// </motivation>
//
// #<thrown>
//
// #</thrown>
//
// <todo asof="1997/05/30">
//   <li> cleanup the currently dual interface for visibilities and flags
//   <li> sort out what to do with weights when interpolating
// </todo>

class VisibilityIteratorWriteImpl {

    friend class VisibilityIterator;

public:

    typedef ROVisibilityIterator::DataColumn DataColumn;

    // Constructors.
    // Note: The VisibilityIterator is not initialized correctly by default, you
    // need to call origin () before using it to iterate.

    VisibilityIteratorWriteImpl (VisibilityIterator * vi);

    //VisibilityIteratorWriteImpl (VisibilityIterator * vi);

    VisibilityIteratorWriteImpl (const VisibilityIteratorWriteImpl & other);

    // Destructor

    virtual ~VisibilityIteratorWriteImpl ();

    // Members

    virtual VisibilityIteratorWriteImpl * clone (VisibilityIterator * vi) const;

    virtual casacore::Bool isWritable () const {
        return true;
    }

    // Set/modify the flags in the data.
    // This will flag all channels in the original data that contributed to
    // the output channel in the case of channel averaging.
    // All polarizations have the same flag value.
    virtual void setFlag (const casacore::Matrix<casacore::Bool> & flag);

    // Set/modify the flags in the data.
    // This sets the flags as found in the casacore::MS, casacore::Cube (npol,nchan,nrow),
    // where nrow is the number of rows in the current iteration (given by
    // nRow ()).
    virtual void setFlag (const casacore::Cube<casacore::Bool> & flag);

    // Set/modify the flag row column; dimension casacore::Vector (nrow)
    virtual void setFlagRow (const casacore::Vector<casacore::Bool> & rowflags);

    void setFlagCategory(const casacore::Array<casacore::Bool>& fc);

    // Set/modify the visibilities.
    // This is possibly only for a 'reference' casacore::MS which has a new DATA column.
    // The first axis of the matrix should equal the selected number of channels
    // in the original MS.
    // If the casacore::MS does not contain all polarizations, only the parallel
    // hand polarizations are used.
    virtual void setVis (const casacore::Matrix<CStokesVector> & vis, DataColumn whichOne);

    // Set/modify the visibilities
    // This sets the data as found in the casacore::MS, casacore::Cube (npol,nchan,nrow).
    virtual void setVis (const casacore::Cube<casacore::Complex> & vis, DataColumn whichOne);

    // Set the visibility and flags, and interpolate from velocities if needed
    virtual void setVisAndFlag (const casacore::Cube<casacore::Complex> & vis, const casacore::Cube<casacore::Bool> & flag,
                        DataColumn whichOne);

    // Set/modify the weights
    virtual void setWeight (const casacore::Vector<casacore::Float> & wt);

    // Set/modify the weightMat
    virtual void setWeightMat (const casacore::Matrix<casacore::Float> & wtmat);

    // Set/modify the weightSpectrum
    virtual void setWeightSpectrum (const casacore::Cube<casacore::Float> & wtsp);

    // Set/modify the Sigma
    virtual void setSigma (const casacore::Vector<casacore::Float> & sig);

    // Set/modify the ncorr x nrow SigmaMat.
    virtual void setSigmaMat (const casacore::Matrix<casacore::Float> & sigmat);

    virtual void writeBack (VisBuffer *);

protected:

    // A BackWriter is a functor that will extract a piece of information out of its VisBuffer
    // argument and write it out using a "set" method on the supplied VisibilityIterator.
    class BackWriter {

    public:

        virtual ~BackWriter () {}

        virtual void operator () (VisibilityIteratorWriteImpl * vi, VisBuffer * vb) = 0;

    };

    // A simple BackWriterImpl uses a nullary accessor on a VisBuffer.
    template <typename Setter, typename Getter>
    class BackWriterImpl : public BackWriter {
    public:

        BackWriterImpl (Setter setter, Getter getter) : getter_p (getter), setter_p (setter) {}
        void operator () (VisibilityIteratorWriteImpl * vi, VisBuffer * vb) {
            (vi ->* setter_p) ((vb ->* getter_p) ());
        }

    private:

        Getter getter_p;
        Setter setter_p;
    };

    // BackWriterImpl2 is slightly more complicated in that it uses a unary accessor.  The argument
    // to the unary accessor is a member of the ROVisibilityIterator DataColumn enumeration which
    // specifies which visibilty or visCube type is wanted (e.g., observed, model or corrected).
    template <typename Setter, typename Getter>
    class BackWriterImpl2 : public BackWriter {
    public:

        typedef VisibilityIteratorReadImpl::DataColumn DataColumn;

        BackWriterImpl2 (Setter setter, Getter getter, DataColumn dc)
        : dataColumn_p (dc), getter_p (getter), setter_p (setter)
        {}
        void operator () (VisibilityIteratorWriteImpl * vi, VisBuffer * vb) {
            (vi ->* setter_p) ((vb ->* getter_p) (), dataColumn_p);
        }

    private:

        DataColumn dataColumn_p;
        Getter getter_p;
        Setter setter_p;
    };

    // Backwriter(2) creation methods.  These methods make it fairly straightforward to create
    // a BackWriter object.

    template <typename Ret>
    static
    BackWriter *
    makeBackWriter (void (VisibilityIteratorWriteImpl::* setter) (Ret), Ret (VisBuffer::* getter) () const) {
        return new BackWriterImpl <void (VisibilityIteratorWriteImpl:: *) (Ret),
                                   Ret (VisBuffer:: *) () const >
        (setter, getter);
    }

    template <typename Ret>
    static
    BackWriter *
    makeBackWriter2 (void (VisibilityIteratorWriteImpl::* setter) (Ret, VisibilityIteratorReadImpl::DataColumn),
                     Ret (VisBuffer::* getter) () const,
                     ROVisibilityIterator::DataColumn dc) {

        // Define the Getter and Setter types

        typedef void (VisibilityIteratorWriteImpl::* Setter) (Ret, VisibilityIteratorReadImpl::DataColumn);
        typedef Ret (VisBuffer::* Getter) () const;

        return new BackWriterImpl2 < Setter, Getter> (setter, getter, dc);
    }

    VisibilityIteratorReadImpl * getReadImpl ();

    void initializeBackWriters ();

    virtual void attachColumns (const casacore::Table & t);

//    void setInterpolatedVisFlag (const casacore::Cube<casacore::Complex> & vis,
//                                 const casacore::Cube<casacore::Bool> & flag);
//    void setInterpolatedWeight (const casacore::Matrix<casacore::Float> & wt);

    // Write the data column (observed, model or corrected);
    // deals with casacore::Float or casacore::Complex observed data (DATA and FLOAT_DATA).

    void putDataColumn (DataColumn whichOne, const casacore::Slicer & slicer,
                        const casacore::Cube<casacore::Complex> & data);
    void putDataColumn (DataColumn whichOne, const casacore::Cube<casacore::Complex> & data);

    // column access functions, can be overridden in derived classes
    virtual void putCol (casacore::ScalarColumn<casacore::Bool> & column, const casacore::Vector<casacore::Bool> & array);
    virtual void putCol (casacore::ArrayColumn<casacore::Bool> & column, const casacore::Array<casacore::Bool> & array);
    virtual void putCol (casacore::ArrayColumn<casacore::Float> & column, const casacore::Array<casacore::Float> & array);
    virtual void putCol (casacore::ArrayColumn<casacore::Complex> & column, const casacore::Array<casacore::Complex> & array);

    virtual void putCol (casacore::ArrayColumn<casacore::Bool> & column, const casacore::Slicer & slicer, const casacore::Array<casacore::Bool> & array);
    virtual void putCol (casacore::ArrayColumn<casacore::Float> & column, const casacore::Slicer & slicer, const casacore::Array<casacore::Float> & array);
    virtual void putCol (casacore::ArrayColumn<casacore::Complex> & column, const casacore::Slicer & slicer, const casacore::Array<casacore::Complex> & array);

    template <class T> void setTileShape(casacore::RefRows &rowRef,casacore::ArrayColumn<T> &outputDataCol,const casacore::IPosition &arrayShape);
    casacore::Bool useCustomTileShape();

    // non-virtual, no reason to template this function because casacore::Bool is the only type needed
    void putColScalar (casacore::ScalarColumn<casacore::Bool> & column, const casacore::Vector<casacore::Bool> & array);

    //This puts a model into the descriptor of the actual ms
    //Set iscomponentlist to true if the record represent a componentlist
    //if false then it is a FTMachine casacore::Record that holds the model image
    // a [-1] vector in validfields mean the model applies to all fields of the active ms 
    virtual void putModel(const casacore::RecordInterface& rec, casacore::Bool iscomponentlist=true, casacore::Bool incremental=false);





private:

    class Columns {

    public:

        Columns & operator= (const Columns & other);

        casacore::ArrayColumn<casacore::Complex> corrVis_p;
        casacore::ScalarColumn<casacore::Bool>   flagRow_p;
        casacore::ArrayColumn<casacore::Bool>    flag_p;
        casacore::ArrayColumn<casacore::Bool>    flagCategory_p;
        casacore::ArrayColumn<casacore::Float>   floatVis_p;
        casacore::ArrayColumn<casacore::Complex> modelVis_p;
        casacore::ArrayColumn<casacore::Float>   sigma_p;
        casacore::ArrayColumn<casacore::Complex> vis_p;
        casacore::ArrayColumn<casacore::Float>   weightSpectrum_p;
        casacore::ArrayColumn<casacore::Float>   weight_p;

    };

    std::map <VisBufferComponents::EnumType, BackWriter *> backWriters_p;
    Columns columns_p;
    VisibilityIterator * vi_p; // [use]
    casacore::Bool useCustomTileShape_p;
};

} //# NAMESPACE CASA - END

#endif

