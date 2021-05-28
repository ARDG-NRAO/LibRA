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

#ifndef MSVIS_VISBUFFER2_H
#define MSVIS_VISBUFFER2_H

#include <casa/aips.h>

//#include <casa/Arrays/Cube.h>
//#include <casa/Arrays/Vector.h>
//#include <casa/Arrays/Matrix.h>
//#include <casa/BasicSL/Complex.h>
#include <casacore/casa/BasicSL/Complexfwd.h>
#include <msvis/MSVis/VisBufferComponents2.h>
//#include <msvis/MSVis/VisibilityIterator2.h>
#include <casacore/measures/Measures/Stokes.h>
#include <casacore/casa/Arrays/ArrayFwd.h>
#include <casacore/casa/Exceptions/Error.h>
#include <casacore/tables/Tables/RowNumbers.h>

using casa::vi::VisBufferComponent2;
using casa::vi::VisBufferComponents2;

namespace casacore{

template <typename T> class CountedPtr;
class MDirection;
template <typename T, Int N> class SquareMatrix;
class MeasurementSet;
}

namespace casa { //# NAMESPACE CASA - BEGIN

//#forward

class CStokesVector;

namespace ms {

    class MsRow;
}

namespace vi {

class Subchunk;
class ViImplementation2;
class VisibilityIterator2;
class WeightScaling;
class SubtableColumns;

// These are options to be applied to a VisBuffer, usually when it's created.
// The intent is that these form a bit mask so that they can be used as
// VbWritable | VbRekeyable, etc.  So add the next one in as 2 * theLastOne.

enum VisBufferOptions : int {VbNoOptions, VbWritable = 1, VbRekeyable = 2};

//<summary>VisBuffer2s encapsulate one chunk of visibility data for processing.</summary>
//
// <use visibility=export>
//
// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//   <li> <linkto class="VisibilityIterator">VisibilityIterator</linkto>
//   <li> <linkto class="VbDirtyComponents">VbDirtyComponents</linkto>
// </prerequisite>
//
// <etymology>
// VisBuffer2 is a buffer for visibility data
// </etymology>
//
//<synopsis>
// The VisBuffer is designed to contain a small amount of visibility-related
// data.  The VisBuffer can be used in two somewhat distinct ways.  The first
// is as an integral (or attached) part of the VisibilityIterator and the second
// is as a free or unattached object.
//
// Attached VisBuffer -- Each VisibilityIterator has a exactly one VisBuffer
// attached to it.  This VisBuffer is created and destroyed by the
// VisibilityIterator.  The role of an attached VB is to contain the data
// currently "pointed to" by the VI.  Because of this tight coupling between
// an attached VI and its VB the operations that can be applied to a VisBuffer
// are somewhat restricted in order to maintain the relationship between the
// casacore::MeasurementSet's data and the data i the VisBuffer (failure to do so allows
// obscure bugs to be created).  As such the functions for averaging in either
// the time or frequency axes is not permitted on an attached VB.
//
// Free VisBuffer -- A free VisBuffer is used to contain data that might not
// correspond to the data in the MeasurementSet.  The meaning of the data in
// a free VisBuffer will depend on the user.  Some obvious examples might be:
// a VisBuffer used to resample or average frequencies together; creation of
// "virtual" spectral windows which might reconnect frequencies that were
// split into separate spectral windows becasue of hardware limitation; and
// performing a time average of visibility data.  Because the free VB is not
// tightly coupled to a VI, it is the responsibility of the user to assign
// meaningful values to some of the fields in the VB.  For example, the user
// averaging across time will need to decide what values ought to be reported
// for the VisBuffer's timestamp, pointing angle, etc.
//
// Another possible attribute of a VisBuffer is "rekeyable".  This is for when
// the VisBuffer is being filled by client code rather than from an input MS.
// Because of this, the enforcement of VisBuffer invariants is relaxed.  Users
// will be able to change row key fields (e.g., antenna1, data description id,
// etc.) as well as change the shape of the data.  The method validateShapes is
// provided which will check that all modified shapes are consistent.  This method
// is automatically called by writeChangesBack and should also be called whenever
// the construction process is complete and the VisBuffer is about to be released
// for use; it's better to catch the error rather than letting an inconsistent
// VisBuffer escape into consuming code.
//
//</synopsis>
//
//<todo>
//</todo>

class VisBuffer2 {

    friend class ViImplementation2;
    friend class VisibilityIteratorImpl2;
    friend class FinalTvi2;
    friend class TransformingVi2;
    friend class SimpleSimVi2;

public:

    // make noncopyable...
    VisBuffer2( const VisBuffer2& ) = delete;
    VisBuffer2& operator=( const VisBuffer2& ) = delete;

    enum {FrameNotSpecified = -2};

    VisBuffer2 () : associatedVi_p (nullptr) {}

    static VisBuffer2 * factory (VisBufferOptions vbOptions = VbNoOptions);

        // Used by framework

    // Destructor (detaches from VisIter)

    virtual ~VisBuffer2() {}

    virtual ms::MsRow * getRow (casacore::Int row) const;
    virtual ms::MsRow * getRowMutable (casacore::Int row);

    //---------------------------------------------------------------------
    //
    // Copying methods
    //
    // These methods allow copying portions of the data between two
    // VisBuffers.  Because of the complicated semantics of a VisBuffer the
    // assignment and copy-construction methods are not used as they are likely
    // to confuse VisBuffer users.


    // Copies all of the components from the specified VisBuffer into this one.
    // Uncached values will be cleared in this VB.

    virtual void copy (const VisBuffer2 & other, casacore::Bool fetchIfNeeded) = 0;

    // Copies the specified components (or just the ones in the cache if
    // fetchIfNeeded is false) from the specified VisBuffer into this one.
    // If this VB is not empty it will must have the same shape as the other
    // VB unless allowShapeChange is true; in that case this VB will change
    // changes to match the other VB flushing the cached data.

    virtual void copyComponents (const VisBuffer2 & other,
				 const VisBufferComponents2 & components,
				 casacore::Bool allowShapeChange = false,
				 casacore::Bool fetchIfNeeded = true) = 0;

    // Copies the coordinate components (or just the ones in the cache if
    // fetchIfNeeded is false) from the specified VisBuffer into this one.
    // If this VB is not empty it will must have the same shape as the other
    // VB unless allowShapeChange is true; in that case this VB will change
    // changes to match the other VB flushing the cached data.
    // The basic coordinate components are:
    //  Antenna1, Antenna2, ArrayId, DataDescriptionIds, FieldId, SpectralWindows,
    //  casacore::Time, NRows, Feed1, Feed2
    // The directional coordinates (copied if includeDirections is true):
    //   Direction1, Direction2, FeedPa1, FeedPa2

    virtual void copyCoordinateInfo(const VisBuffer2 * other,
                                    casacore::Bool includeDirections,
                                    casacore::Bool allowShapeChange = false,
                                    casacore::Bool fetchIfNeeded = true) = 0;

    virtual void setShape (casacore::Int nCorrelations, casacore::Int nChannels, casacore::rownr_t nRows, casacore::Bool clearCache = false) = 0;
    virtual void validateShapes () const = 0;

    // For attached VBs this returns the VI the VB is attached to.  For free VBs
    // this method returns false. N.B.: Use of this method is deprecated.

    virtual const VisibilityIterator2 * getVi () const;

    virtual const casacore::MeasurementSet& ms() const { ThrowCc ("Should be overridden"); } // Kluge
    virtual const vi::SubtableColumns & subtableColumns () const { ThrowCc ("Should be overridden"); } // Kluge
	virtual casacore::Bool existsColumn (VisBufferComponent2 id) const = 0;
    
    virtual casacore::Bool isAttached () const = 0;
    virtual casacore::Bool isFillable () const = 0;

    //---------------------------------------------------------------------
    //
    //    Dirty component methods
    //
    //    The dirtyComponent methods support the data-flow approach to using
    //    VisBuffers (the Visibility Processing Framework).  In this approach
    //    a VisBuffer is passed to successive processing nodes (e.g., applycal,
    //    flagging, etc.) which operate  on it and pass it on to the next node
    //    in the algorithm. The dirtyComponents mechanism allows a processing
    //    node to mark portions of the VisBuffer as modified.  If the VisBuffer
    //    reaches an write-to-disk node then the modified portions of the
    //    VisBuffer will be written out.  The user can also explicitly direct
    //    that the changes be written out using the writeChangesBack method.
    //
    //    Using a setter on a VisBuffer component will also set the dirty flag for
    //    that component.  Normally the user should not need to use these methods;
    //    however, they are available in case unexpected situations should arise
    //    in the future.

    virtual void writeChangesBack () = 0;
    virtual void initWeightSpectrum (const casacore::Cube<casacore::Float>& wtspec) = 0;
    virtual void initSigmaSpectrum (const casacore::Cube<casacore::Float>& sigspec) = 0;

    virtual void dirtyComponentsAdd (const VisBufferComponents2 & additionalDirtyComponents) = 0;
    virtual void dirtyComponentsAdd (VisBufferComponent2 component) = 0;
    virtual void dirtyComponentsClear () = 0;
    virtual VisBufferComponents2 dirtyComponentsGet () const = 0;
    virtual void dirtyComponentsSet (const VisBufferComponents2 & dirtyComponents) = 0;
    virtual void dirtyComponentsSet (VisBufferComponent2 component) = 0;

    // This method returns the imagin g
    // If an imaging weight generator has not been supplied to the associated
    // VisibilityIterator then this method will throw an exception.

    virtual const casacore::Matrix<casacore::Float> & imagingWeight() const = 0;
    virtual void setImagingWeight (const casacore::Matrix<casacore::Float> & newImagingWeights) = 0;

    //---------------------------------------------------------------------------
    //
    // Frequency reporting methods.
    //
    // These methods provide information about the frequencies returned in the
    // visibility cubes.  The information can returned as the channel numbers
    // (numbered as in the underlying casacore::MS data) or in a frame-based frequency.
    // This information reported need not be in the same frame of reference
    // used to select the data.  If the frame of reference is specified in the
    // call, then that is the frame that is used to calculate the frequencies.
    // If it is not specified, then the VisibilityIterator will be queried for
    // virtual the reportingFrame = 0; if the user has specified a reporting frame to the
    // virtual VI then that frame will be used = 0; otherwise the frame used to select
    // the frequencies will be used.  If the user provides no frequency selection
    // to the VI then the selection frame will TOPO.
    //
    // Both the channelNumber and frequency reporting methods come in two versions:
    // one returns a single frequency for the specified frequency index and row
    // while the other provides all of the frequencies for the specified row.
    // The frequency index is the zero-based index along the frequency axis of
    // a visibility cube.

    virtual casacore::Vector<casacore::Stokes::StokesTypes> getCorrelationTypesDefined () const = 0;
    virtual casacore::Vector<casacore::Stokes::StokesTypes> getCorrelationTypesSelected () const = 0;
    virtual casacore::Vector<casacore::Int> getCorrelationTypes () const = 0;
    virtual casacore::Double getFrequency (casacore::Int rowInBuffer, casacore::Int frequencyIndex,
                                 casacore::Int frame = FrameNotSpecified) const = 0;
    virtual const casacore::Vector<casacore::Double> & getFrequencies (casacore::Int rowInBuffer,
                                                   casacore::Int frame = FrameNotSpecified) const = 0;
    virtual casacore::Int getChannelNumber (casacore::Int rowInBuffer, casacore::Int frequencyIndex) const = 0;
    virtual const casacore::Vector<casacore::Int> & getChannelNumbers (casacore::Int rowInBuffer) const = 0;
    virtual casacore::Vector<casacore::Int> getChannelNumbersSelected (casacore::Int outputChannelIndex) const = 0;

    // casacore::Sort/unsort the correlations, if necessary
    //  (Rudimentary handling of non-canonically sorted correlations--use with care!)
    //
    // The sorting functionality is a horrible kluge that puts the VisBuffer into a
    // somewhat incoherent state (e.g., after sorting the correlation types array
    // does not correspond to the data) and appears to serve the needs
    // of a tiny piece of code.  As such, this refactor is initially not going to
    // support this functionality since it is probably better implemented in the one
    // place that actually needs it. (jjacobs 10/3/12)
    //
    //virtual void sortCorr () = 0;
    //virtual void unSortCorr() = 0;

    // Normalize the visCube by the modelVisCube.

    virtual void normalize() = 0;

    // Rotate visibility phase for given vector (dim = nrow of vb) of phases (metres)
    virtual void phaseCenterShift(const casacore::Vector<casacore::Double>& phase) = 0;
    // Rotate visibility phase for phase center offsets (arcsecs)
    virtual void phaseCenterShift(casacore::Double dx, casacore::Double dy) = 0;

    // Set the weight cube using the sigma cube.  Each weight will be
    // the reciprocal of the square of the corresponding element in the model
    // VisCube multiplied by the number of channels in the spectral window.
    // If an element in sigma is zero then the corresponding weight element
    // will also be set to zero.

    virtual void resetWeightsUsingSigma () = 0;//void resetWeightMat() = 0;

    //----------------------------------------------------------------------
    //
    //  Subhchunk information methods
    //
    //  These methods provide information related to the current subchunk.
    //  The isNewXXX methods return true if the XXX property of the subchunk
    //  differs from the previous subchunk.
    //
    //  The methods msId and msName provide information about the MS
    //  related to the current subchunk.  The msID is the zero-based index
    //  of the casacore::MS in the sequence of MSs being iterated over.
    //
    //  The isWritable method is true when the attached iterator is writable
    //  and false otherwise.
    //
    //  The isRekeyable method is true when the VisBuffer is writable and also
    //  allows the modification of non-data fields (e.g., antenna1, fieldId, etc.)
    //  A rekeyable VB is one that could be used to create data for a brand new
    //  MS.

    virtual casacore::Bool isNewArrayId () const = 0;
    virtual casacore::Bool isNewFieldId () const = 0;
    virtual casacore::Bool isNewMs() const = 0;
    virtual casacore::Bool isNewSpectralWindow () const = 0;
    virtual casacore::Bool isWritable () const = 0;
    virtual casacore::Int msId() const /*__attribute__((deprecated))*/ = 0;
    virtual casacore::String msName (casacore::Bool stripPath = false) const /*__attribute__((deprecated))*/ = 0;
    virtual Subchunk getSubchunk () const = 0;

    //////////////////////////////////////////////////////////////////////
    //
    //  casacore::Data accessors and setters (where appropriate)
    //
    //  There are the methods that allows access to the items cached in the
    //  VisBuffer.  The straight accessors provide read-only access to the
    //  item.  Where the item is allowed to be modified, one or more set
    //  methods are provided.
    //
    //  The dimensionality of the major object in in accessor/setter is
    //  shown in a trailing comment using the following abbreviations:
    //
    //  nA :== number of antennas
    //  nF :== number of frequencies (or channels)
    //  nC :== number of correlations
    //  nR :== number of table rows contained in the buffer

    //--------------------------------------------------------
    //
    // Accessors for data contained in the main casacore::MeasurementSet main table
    // The actual visibility data are at the end.
    //
    //  *** N.B.: the VB usually caches the information
    //  in the representation requested so that using a setter to modify
    //  one value type (e.g., weight or visibility) will not modify the
    //  cached value in a different representation (e.g., weightMat or
    //  visCube).  This should not be a problem in normal usage.

    virtual const casacore::Vector<casacore::Int> & antenna1 () const = 0; // [nR]
    virtual void setAntenna1 (const casacore::Vector<casacore::Int> & value) = 0; // [nR]
    virtual const casacore::Vector<casacore::Int> & antenna2 () const = 0; // [nR]
    virtual void setAntenna2 (const casacore::Vector<casacore::Int> & value) = 0; // [nR]
    virtual const casacore::Vector<casacore::Int> & arrayId () const = 0;
    virtual void setArrayId (const casacore::Vector<casacore::Int> & ) = 0;
    //virtual casacore::Int dataDescriptionId () const = 0;
    //virtual void setDataDescriptionId (casacore::Int value) = 0;
    virtual const casacore::Vector<casacore::Int> & dataDescriptionIds () const = 0; // [nR]
    virtual void setDataDescriptionIds (const casacore::Vector<casacore::Int> & ) = 0; // [nR]
    virtual const casacore::Vector<casacore::MDirection> & direction1 () const = 0; // [nR]
    virtual const casacore::Vector<casacore::MDirection> & direction2 () const = 0; // [nR]
    virtual const casacore::Vector<casacore::Double> & exposure () const = 0; // [nR]
    virtual void setExposure (const casacore::Vector<casacore::Double> & value) = 0; // [nR]
    virtual const casacore::Vector<casacore::Int> & feed1 () const = 0; // [nR]
    virtual void setFeed1 (const casacore::Vector<casacore::Int> & value) = 0; // [nR]
    virtual const casacore::Vector<casacore::Int> & feed2 () const = 0; // [nR]
    virtual void setFeed2 (const casacore::Vector<casacore::Int> & value) = 0; // [nR]
    virtual const casacore::Vector<casacore::Int> & fieldId () const = 0;
    virtual void setFieldId (const casacore::Vector<casacore::Int> &) = 0;
    //virtual const casacore::Matrix<casacore::Bool> & flag () const = 0; // [nF,nR]
    //virtual void setFlag (const casacore::Matrix<casacore::Bool>& value) = 0; // [nF,nR]
    virtual const casacore::Array<casacore::Bool> & flagCategory () const = 0; // [nC,nF,nCategories,nR]
    virtual void setFlagCategory (const casacore::Array<casacore::Bool>& value) = 0; // [nC,nF,nCategories,nR]
    virtual const casacore::Vector<casacore::Array<casacore::Bool>> & flagCategories () const = 0; // [nC,nF,nCategories,nR]
    virtual void setFlagCategories (const casacore::Vector<casacore::Array<casacore::Bool>>& value) = 0; // [nC,nF,nCategories,nR]
    virtual const casacore::Cube<casacore::Bool> & flagCube () const = 0; // [nC,nF,nR]
    virtual void setFlagCube (const casacore::Cube<casacore::Bool>& value) = 0; // [nC,nF,nR]
    virtual const casacore::Vector<casacore::Cube<casacore::Bool>> & flagCubes () const = 0; // [nC,nF,nR]
    virtual void setFlagCubes (const casacore::Vector<casacore::Cube<casacore::Bool>>& value) = 0; // [nC,nF,nR]
    virtual const casacore::Vector<casacore::Bool> & flagRow () const = 0; // [nR]
    virtual void setFlagRow (const casacore::Vector<casacore::Bool>& value) = 0; // [nR]
    virtual const casacore::Vector<casacore::Int> & observationId () const = 0; // [nR]
    virtual void setObservationId (const casacore::Vector<casacore::Int> & value) = 0; // [nR]
    virtual const casacore::Vector<casacore::Int> & processorId () const = 0; // [nR]
    virtual void setProcessorId (const casacore::Vector<casacore::Int> & value) = 0; // [nR]
    virtual const casacore::Vector<casacore::Int> & scan () const = 0; // [nR]
    virtual void setScan (const casacore::Vector<casacore::Int> & value) = 0; // [nR]
    virtual const casacore::Matrix<casacore::Float> & sigma () const = 0; // [nC, nR]
    virtual void setSigma (const casacore::Matrix <casacore::Float> & value) = 0; // [nC, nR]
    virtual const casacore::Vector<casacore::Matrix<casacore::Float>> & sigmas () const = 0; // [nC, nR]
    virtual void setSigmas (const casacore::Vector<casacore::Matrix <casacore::Float>> & value) = 0; // [nC, nR]
    //virtual const casacore::Matrix<casacore::Float> & sigmaMat () const = 0; // [nC,nR]
    virtual const casacore::Vector<casacore::Int> & stateId () const = 0; // [nR]
    virtual void setStateId (const casacore::Vector<casacore::Int> & value) = 0; // [nR]
    virtual const casacore::Vector<casacore::Double> & time () const = 0; // [nR]
    virtual void setTime (const casacore::Vector<casacore::Double> & value) = 0; // [nR]
    virtual const casacore::Vector<casacore::Double> & timeCentroid () const = 0; // [nR]
    virtual void setTimeCentroid (const casacore::Vector<casacore::Double> & value) = 0; // [nR]
    virtual const casacore::Vector<casacore::Double> & timeInterval () const = 0; // [nR]
    virtual void setTimeInterval (const casacore::Vector<casacore::Double> & value) = 0; // [nR]
    virtual const casacore::Matrix<casacore::Double> & uvw () const = 0; // [3,nR]
    virtual void setUvw (const casacore::Matrix<casacore::Double> & value) = 0; // [3,nR]
    virtual const casacore::Matrix<casacore::Float> & weight () const = 0; // [nC, nR]
    virtual void setWeight (const casacore::Matrix <casacore::Float>& value) = 0; // [nC, nR]
    virtual const casacore::Vector<casacore::Matrix<casacore::Float>> & weights () const = 0; // [nC, nR]
    virtual void setWeights (const casacore::Vector<casacore::Matrix <casacore::Float>>& value) = 0; // [nC, nR]
    //virtual const casacore::Matrix<casacore::Float> & weightMat () const = 0; // [nC,nR]
    //virtual void setWeightMat (const casacore::Matrix<casacore::Float>& value) = 0; // [nC,nR]

    virtual const casacore::Cube<casacore::Float> & weightSpectrum () const = 0; // [nC,nF,nR]
    virtual void setWeightSpectrum (const casacore::Cube<casacore::Float>& value) = 0; // [nC,nF,nR]
    virtual const casacore::Vector<casacore::Cube<casacore::Float>> & weightSpectra () const = 0; // [nC,nF,nR]
    virtual void setWeightSpectra (const casacore::Vector<casacore::Cube<casacore::Float>>& value) = 0; // [nC,nF,nR]

    virtual const casacore::Cube<casacore::Float> & sigmaSpectrum () const = 0; // [nC,nF,nR]
    virtual void setSigmaSpectrum (const casacore::Cube<casacore::Float>& value) = 0; // [nC,nF,nR]
    virtual const casacore::Vector<casacore::Cube<casacore::Float>> & sigmaSpectra () const = 0; // [nC,nF,nR]
    virtual void setSigmaSpectra (const casacore::Vector<casacore::Cube<casacore::Float>>& value) = 0; // [nC,nF,nR]

    // --------------------------------------------------------------
    // Visibility data accessors in order of observed, corrected,
    // float, & model

    virtual const casacore::Cube<casacore::Complex> & visCube () const = 0; // [nC,nF,nR]
    virtual void setVisCube(const casacore::Complex & c) = 0;
    virtual void setVisCube (const casacore::Cube<casacore::Complex> &) = 0; // [nC,nF,nR]
    virtual const casacore::Vector<casacore::Cube<casacore::Complex>> & visCubes () const = 0; // [nC,nF,nR]
    virtual void setVisCubes (const casacore::Vector<casacore::Cube<casacore::Complex>> &) = 0; // [nC,nF,nR]
//    virtual const casacore::Matrix<CStokesVector> & vis () const = 0; // [nF,nR]
//    virtual void setVis (casacore::Matrix<CStokesVector> &) = 0; // [nF,nR]

    virtual const casacore::Cube<casacore::Complex> & visCubeCorrected () const = 0; // [nC,nF,nR]
    virtual void setVisCubeCorrected (const casacore::Cube<casacore::Complex> &) = 0; // [nC,nF,nR]
    virtual const casacore::Vector<casacore::Cube<casacore::Complex>> & visCubesCorrected () const = 0; // [nC,nF,nR]
    virtual void setVisCubesCorrected (const casacore::Vector<casacore::Cube<casacore::Complex>> &) = 0; // [nC,nF,nR]
//    virtual const casacore::Matrix<CStokesVector> & visCorrected () const = 0; // [nF,nR]
//    virtual void setVisCorrected (const casacore::Matrix<CStokesVector> &) = 0; // [nF,nR]

    virtual const casacore::Cube<casacore::Float> & visCubeFloat () const = 0; // [nC,nF,nR]
    virtual void setVisCubeFloat (const casacore::Cube<casacore::Float> &) = 0; // [nC,nF,nR]
    virtual const casacore::Vector<casacore::Cube<casacore::Float>> & visCubesFloat () const = 0; // [nC,nF,nR]
    virtual void setVisCubesFloat (const casacore::Vector<casacore::Cube<casacore::Float>> &) = 0; // [nC,nF,nR]

    virtual const casacore::Cube<casacore::Complex> & visCubeModel () const = 0; // [nC,nF,nR]
    virtual void setVisCubeModel(const casacore::Complex & c) = 0;
    virtual void setVisCubeModel(const casacore::Cube<casacore::Complex>& vis) = 0; // [nC,nF,nR]
    virtual void setVisCubeModel(const casacore::Vector<casacore::Float>& stokes) = 0; // [1..4]
    virtual const casacore::Vector<casacore::Cube<casacore::Complex>> & visCubesModel () const = 0; // [nC,nF,nR]
    //virtual void setVisCubesModel(const casacore::Complex & c) = 0;
    virtual void setVisCubesModel(const casacore::Vector<casacore::Cube<casacore::Complex>>& vis) = 0; // [nC,nF,nR]
//    virtual const casacore::Matrix<CStokesVector> & visModel () const = 0; // [nF,nR]
//    virtual void setVisModel (casacore::Matrix<CStokesVector> &) = 0; // [nF,nR]

    virtual casacore::Float getWeightScaled (casacore::Int row) const = 0;
    virtual casacore::Float getWeightScaled (casacore::Int correlation, casacore::Int row) const = 0;
    virtual casacore::Float getWeightScaled (casacore::Int correlation, casacore::Int channel, casacore::Int row) const = 0;
    virtual casacore::Float getSigmaScaled (casacore::Int row) const = 0;
    virtual casacore::Float getSigmaScaled (casacore::Int correlation, casacore::Int row) const = 0;
    virtual casacore::Float getSigmaScaled (casacore::Int correlation, casacore::Int channel, casacore::Int row) const = 0;

    //--------------------------------------------------------
    //
    // Accessors for data derived from the casacore::MS main table data

    // Returns the pointing angle for the array as a whole at the
    // specified time.

    virtual casacore::MDirection azel0 (casacore::Double time) const = 0;

    // Returns the pointing angle for each antenna in the array
    // at the specified time.

    virtual const casacore::Vector<casacore::MDirection> & azel(casacore::Double time) const = 0; // [nA]

    // Returns the Jones C matrix for each antenna.

    virtual const casacore::Vector<casacore::SquareMatrix<casacore::Complex, 2> > & cjones () const = 0; // [nA]

    // Returns the correlation type of each correlation in the
    // VisCube.

    virtual const casacore::Vector<casacore::Int> & correlationTypes () const = 0; // [nC]

    // Calculates the parallactic angle for the first receptor of
    // each antenna at the specified time.

    virtual const casacore::Vector<casacore::Float> & feedPa(casacore::Double time) const = 0; // [nR]

    // Calculates the parallactic angle for feed 0 of the
    // row's Antenna1.

    virtual const casacore::Vector<casacore::Float> & feedPa1 () const = 0; // [nR]

    // Calculates the parallactic angle for feed 0 of the
    // row's Antenna2.

    virtual const casacore::Vector<casacore::Float> & feedPa2 () const = 0; // [nR]

    virtual casacore::IPosition getShape () const = 0;

    // Returns the hour angle of the array at the specified time.

    virtual casacore::Double hourang(casacore::Double time) const = 0;

    virtual casacore::Int nAntennas () const = 0;

    virtual casacore::Int nChannels () const = 0;

    // Returns the number of correlations along the visCube
    // correlation axis.  This comes from the "channel" selection and thus can
    // be anything positive integer (e.g., user could select the same
    // correlation more than once).

    virtual casacore::Int nCorrelations () const = 0;

    // Returns the number of rows in this VisBuffer
    virtual casacore::rownr_t nRows () const = 0;

    // Returns the number of distinct cube/array shapes in this VisBuffer
    virtual casacore::rownr_t nShapes () const = 0;

    // Returns the number of rows per distinct cube/array shapes in this VisBuffer
    virtual const casacore::Vector<casacore::rownr_t>& nRowsPerShape () const = 0;

    // Returns the number of channels per distinct cube/array shapes in this VisBuffer
    virtual const casacore::Vector<casacore::Int>& nChannelsPerShape () const = 0;

    // Returns the number of correlation per distinct cube/array shapes in this VisBuffer
    virtual const casacore::Vector<casacore::Int>& nCorrelationsPerShape () const = 0;

    // Calculates the parallactic angle of the array as a whole
    // at the specified time.

    virtual casacore::Float parang0(casacore::Double time) const = 0;

    // Calculates the parallactic angle of each antenna in the
    // array at the specified time.

    virtual const casacore::Vector<casacore::Float> & parang(casacore::Double time) const = 0; // [nA]

    // Returns the phase center of the array for the specified
    // row.

    virtual const casacore::MDirection& phaseCenter () const = 0;

    // Returns the polarization frame for the specified row.

    virtual casacore::Int polarizationFrame () const = 0;

    virtual casacore::Int polarizationId () const = 0;

    // The returned casacore::Vector serves as a map between the rows in
    // the VisBuffer and the row IDs in the underlying casacore::MS main
    // virtual table:  mainTableID [i] = rowIds () [ i] = 0;

    virtual const casacore::Vector<casacore::rownr_t> & rowIds () const = 0; // [nR]

    // Returns the spectral window ID for each row.
    virtual const casacore::Vector<casacore::Int> & spectralWindows () const = 0; // [nR]
    virtual void setSpectralWindows (const casacore::Vector<casacore::Int> & spectralWindows) = 0;

    virtual casacore::CountedPtr<WeightScaling> getWeightScaling () const = 0;

protected:

    virtual void associateWithVi2 (const VisibilityIterator2 *);
    virtual void configureNewSubchunk (casacore::Int msId, const casacore::String & msName, casacore::Bool isNewMs,
                                       casacore::Bool isNewArrayId, casacore::Bool isNewFieldId,
                                       casacore::Bool isNewSpectralWindow, const Subchunk & subchunk,
                                       const casacore::Vector<casacore::rownr_t>& nRowsPerShape,
                                       const casacore::Vector<casacore::Int>& nChannelsPerShape, 
                                       const casacore::Vector<casacore::Int>& nCorrelationsPerShape,
                                       const casacore::Vector<casacore::Int> & correlations,
                                       const casacore::Vector<casacore::Stokes::StokesTypes> & correlationsDefined,
                                       const casacore::Vector<casacore::Stokes::StokesTypes> & correlationsSelected,
                                       casacore::CountedPtr<WeightScaling> weightScaling) = 0;
    virtual void invalidate() = 0;
    virtual casacore::Bool isRekeyable () const = 0;
    virtual void setFillable (casacore::Bool isFillable) = 0;
    virtual void setRekeyable (casacore::Bool isRekeable) = 0;
    virtual bool setWritability (bool /*newWritability*/) { ThrowCc ("Should be overridden"); } // Kluge

    virtual casacore::Vector<casacore::Bool> & flagRowRef () = 0;  // [nR]
    virtual casacore::Cube<casacore::Bool> & flagCubeRef () = 0;  // [nC,nF,nR]
    virtual casacore::Vector<casacore::Cube<casacore::Bool>> & flagCubesRef () = 0;  // [nC,nF,nR]
    virtual casacore::Cube<casacore::Complex> & visCubeRef () = 0; // [nC,nF,nR]
    virtual casacore::Vector<casacore::Cube<casacore::Complex>> & visCubesRef () = 0; // [nC,nF,nR]
    virtual casacore::Cube<casacore::Complex> & visCubeCorrectedRef () = 0; // [nC,nF,nR]
    virtual casacore::Vector<casacore::Cube<casacore::Complex>> & visCubesCorrectedRef () = 0; // [nC,nF,nR]
    virtual casacore::Cube<casacore::Complex> & visCubeModelRef () = 0; // [nC,nF,nR]
    virtual casacore::Vector<casacore::Cube<casacore::Complex>> & visCubesModelRef () = 0; // [nC,nF,nR]
    virtual casacore::Cube<casacore::Float> & weightSpectrumRef () = 0; // [nC,nF,nR]
    virtual casacore::Vector<casacore::Cube<casacore::Float>> & weightSpectraRef () = 0; // [nC,nF,nR]
    virtual casacore::Cube<casacore::Float> & sigmaSpectrumRef () = 0; // [nC,nF,nR]
    virtual casacore::Vector<casacore::Cube<casacore::Float>> & sigmaSpectraRef () = 0; // [nC,nF,nR]

private:

    const VisibilityIterator2 * associatedVi_p;

};

class VisBufferUtil2 {

public:

  static void phaseCenterShift(VisBuffer2 & vb, const casacore::Vector<casacore::Double>& phase);
  static void phaseCenterShift(VisBuffer2 & vb, casacore::Double dx, casacore::Double dy);
};


} // end namespace vi


} //# NAMESPACE CASA - END


#endif

