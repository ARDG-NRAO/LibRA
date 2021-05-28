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
#include <msvis/MSVis/VisBuffer2.h>

namespace casa { //# NAMESPACE CASA - BEGIN

namespace vi {

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
class VisBufferWrapperAsync2 : public VisBuffer2 {

    friend class ViReadImplAsync;

public:

    // Destructor (detaches from VisIter)

    virtual ~VisBufferWrapperAsync2();


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

    virtual void copy (const VisBuffer2 & other, casacore::Bool fetchIfNeeded);

    // Copies the specified components (or just the one in the cache) from
    // the specified VisBuffer into this one.

    virtual void copyComponents (const VisBuffer2 & other,
				 const VisBufferComponents2 & components,
				 casacore::Bool fetchIfNeeded = true);

    // Copies the coordinate components from the specified VisBuffer into this one.
    // Depending on includeDirections the direction related ones are copied or not.

    virtual void copyCoordinateInfo(const VisBuffer2 * other, casacore::Bool includeDirections,
                                    casacore::Bool fetchIfNeeded = true);

    virtual void setShape (casacore::Int nCorrelations, casacore::Int nChannels, casacore::rownr_t nRows);

    // For attached VBs this returns the VI the VB is attached to.  For free
    // VBs this method returns false.

    virtual const VisibilityIterator2 * getVi () const;

    virtual casacore::Bool isAttached () const;
    virtual casacore::Bool isFillable () const;

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

    virtual void writeChangesBack ();

    virtual void dirtyComponentsAdd (const VisBufferComponents2 & additionalDirtyComponents);
    virtual void dirtyComponentsAdd (VisBufferComponent2 component);
    virtual void dirtyComponentsClear ();
    virtual VisBufferComponents2 dirtyComponentsGet () const;
    virtual void dirtyComponentsSet (const VisBufferComponents2 & dirtyComponents);
    virtual void dirtyComponentsSet (VisBufferComponent2 component);

    // This method returns the imaging weights associated with the VisBuffer.
    // If an imaging weight generator has not been supplied to the associated
    // VisibilityIterator then this method will throw an exception.

    virtual const casacore::Matrix<casacore::Float> & imagingWeight() const;

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
    // virtual the reportingFrame; if the user has specified a reporting frame to the
    // virtual VI then that frame will be used; otherwise the frame used to select
    // the frequencies will be used.  If the user provides no frequency selection
    // to the VI then the selection frame will TOPO.
    //
    // Both the channelNumber and frequency reporting methods come in two versions:
    // one returns a single frequency for the specified frequency index and row
    // while the other provides all of the frequencies for the specified row.
    // The frequency index is the zero-based index along the frequency axis of
    // a visibility cube.

    virtual casacore::Double getFrequency (casacore::Int rowInBuffer, casacore::Int frequencyIndex,
                                 casacore::Int frame = FrameNotSpecified) const;
    virtual const casacore::Vector<casacore::Double> & getFrequencies (casacore::Int rowInBuffer,
                                                   casacore::Int frame = FrameNotSpecified) const;
    virtual casacore::Int getChannelNumber (casacore::Int rowInBuffer, casacore::Int frequencyIndex) const;
    virtual const casacore::Vector<casacore::Int> & getChannelNumbers (casacore::Int rowInBuffer) const;

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
    //virtual void sortCorr ();
    //virtual void unSortCorr();

    // Normalize the visCube by the modelVisCube.

    virtual void normalize();

    // Set the weight cube using the sigma cube.  Each weight will be
    // the reciprocal of the square of the corresponding element in the model
    // VisCube multiplied by the number of channels in the spectral window.
    // If an element in sigma is zero then the corresponding weight element
    // will also be set to zero.

    virtual void resetWeightsUsingSigma ();//void resetWeightMat();

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

    virtual casacore::Bool isNewArrayId () const;
    virtual casacore::Bool isNewFieldId () const;
    virtual casacore::Bool isNewMs() const;
    virtual casacore::Bool isNewSpectralWindow () const;
    virtual casacore::Bool isWritable () const;
    virtual casacore::Int msId() const;
    virtual casacore::String msName (casacore::Bool stripPath = false) const;
    virtual Subchunk getSubchunk () const;

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
    virtual const casacore::Vector<casacore::Int>& arrayId () const = 0;
    virtual void setArrayId (const casacore::Vector<casacore::Int>&) = 0;
    virtual casacore::Int dataDescriptionId () const = 0;
    virtual void setDataDescriptionId (casacore::Int value) = 0;
    virtual const casacore::Vector<casacore::MDirection> & direction1 () const = 0; // [nR]
    virtual const casacore::Vector<casacore::MDirection> & direction2 () const = 0; // [nR]
    virtual const casacore::Vector<casacore::Double> & exposure () const = 0; // [nR]
    virtual void setExposure (const casacore::Vector<casacore::Double> & value) = 0; // [nR]
    virtual const casacore::Vector<casacore::Int> & feed1 () const = 0; // [nR]
    virtual void setFeed1 (const casacore::Vector<casacore::Int> & value) = 0; // [nR]
    virtual const casacore::Vector<casacore::Int> & feed2 () const = 0; // [nR]
    virtual void setFeed2 (const casacore::Vector<casacore::Int> & value) = 0; // [nR]
    virtual const casacore::Vector<casacore::Int>& fieldId () const = 0;
    virtual void setFieldId (const casacore::Vector<casacore::Int>&) = 0;
    virtual const casacore::Matrix<casacore::Bool> & flag () const = 0; // [nF,nR]
    virtual void setFlag (const casacore::Matrix<casacore::Bool>& value) = 0; // [nF,nR]
    virtual const casacore::Array<casacore::Bool> & flagCategory () const = 0; // [nC,nF,nCategories,nR]
    virtual void setFlagCategory (const casacore::Array<casacore::Bool>& value) = 0; // [nC,nF,nCategories,nR]
    virtual const casacore::Cube<casacore::Bool> & flagCube () const = 0; // [nC,nF,nR]
    virtual void setFlagCube (const casacore::Cube<casacore::Bool>& value) = 0; // [nC,nF,nR]
    virtual const casacore::Vector<casacore::Bool> & flagRow () const = 0; // [nR]
    virtual void setFlagRow (const casacore::Vector<casacore::Bool>& value) = 0; // [nR]
    virtual const casacore::Vector<casacore::Int> & observationId () const = 0; // [nR]
    virtual void setObservationId (const casacore::Vector<casacore::Int> & value) = 0; // [nR]
    virtual const casacore::Vector<casacore::Int> & processorId () const = 0; // [nR]
    virtual void setProcessorId (const casacore::Vector<casacore::Int> & value) = 0; // [nR]
    virtual const casacore::Vector<casacore::Int> & scan () const = 0; // [nR]
    virtual void setScan (const casacore::Vector<casacore::Int> & value) = 0; // [nR]
    virtual const casacore::Matrix<casacore::Float> & sigma () const = 0; // [nR]
    virtual void setSigma (const casacore::Matrix<casacore::Float> & value) = 0; // [nR]
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
    virtual const casacore::Matrix<casacore::Float> & weight () const = 0; // [nR]
    virtual void setWeight (const casacore::Matrix<casacore::Float>& value) = 0; // [nR]
//    virtual const casacore::Matrix<casacore::Float> & weightMat () const = 0; // [nC,nR]
//    virtual void setWeightMat (const casacore::Matrix<casacore::Float>& value) = 0; // [nC,nR]
    virtual const casacore::Cube<casacore::Float> & weightSpectrum () const = 0; // [nC,nF,nR]
    virtual void setWeightSpectrum (const casacore::Cube<casacore::Float>& value) = 0; // [nC,nF,nR]

    // --------------------------------------------------------------
    // Visibility data accessors in order of observed, corrected,
    // float, & model

    virtual const casacore::Cube<casacore::Complex> & visCube () const; // [nC,nF,nR]
    virtual void setVisCube(const casacore::Complex & c);
    virtual void setVisCube (const casacore::Cube<casacore::Complex> &); // [nC,nF,nR]
    virtual const casacore::Matrix<CStokesVector> & vis () const; // [nF,nR]
    virtual void setVis (casacore::Matrix<CStokesVector> &); // [nF,nR]

    virtual const casacore::Cube<casacore::Complex> & visCubeCorrected () const; // [nC,nF,nR]
    virtual void setVisCubeCorrected (const casacore::Cube<casacore::Complex> &); // [nC,nF,nR]
    virtual const casacore::Matrix<CStokesVector> & visCorrected () const; // [nF,nR]
    virtual void setVisCorrected (const casacore::Matrix<CStokesVector> &); // [nF,nR]

    virtual const casacore::Cube<casacore::Float> & visCubeFloat () const; // [nC,nF,nR]
    virtual void setVisCubeFloat (const casacore::Cube<casacore::Float> &); // [nC,nF,nR]

    virtual const casacore::Cube<casacore::Complex> & visCubeModel () const; // [nC,nF,nR]
    virtual void setVisCubeModel(const casacore::Complex & c);
    virtual void setVisCubeModel(const casacore::Cube<casacore::Complex>& vis); // [nC,nF,nR]
    virtual void setVisCubeModel(const casacore::Vector<casacore::Float>& stokes); // [1..4]
    virtual const casacore::Matrix<CStokesVector> & visModel () const; // [nF,nR]
    virtual void setVisModel (casacore::Matrix<CStokesVector> &); // [nF,nR]

    //--------------------------------------------------------
    //
    // Accessors for data derived from the casacore::MS main table data

    // Returns the pointing angle for the array as a whole at the
    // specified time.

    virtual casacore::MDirection azel0 (casacore::Double time) const;

    // Returns the pointing angle for each antenna in the array
    // at the specified time.

    virtual const casacore::Vector<casacore::MDirection> & azel(casacore::Double time) const; // [nA]

    // Returns the Jones C matrix for each antenna.

    virtual const casacore::Vector<casacore::SquareMatrix<casacore::Complex, 2> > & cjones () const; // [nA]

    // Returns the correlation type of each correlation in the
    // VisCube.

    virtual const casacore::Vector<casacore::Int> & correlationTypes () const; // [nC]

    // Calculates the parallactic angle for the first receptor of
    // each antenna at the specified time.

    virtual const casacore::Vector<casacore::Float> & feedPa(casacore::Double time) const; // [nR]

    // Calculates the parallactic angle for feed 0 of the
    // row's Antenna1.

    virtual const casacore::Vector<casacore::Float> & feedPa1 () const; // [nR]

    // Calculates the parallactic angle for feed 0 of the
    // row's Antenna2.

    virtual const casacore::Vector<casacore::Float> & feedPa2 () const; // [nR]

    // Returns the hour angle of the array at the specified time.

    virtual casacore::Double hourang(casacore::Double time) const;

    virtual casacore::Int nAntennas () const;

    virtual casacore::Int nChannels () const;

    // Returns the number of correlations along the visCube
    // correlation axis.

    virtual casacore::Int nCorrelations () const;

    // Returns the number of rows in this VisBuffer

    virtual casacore::rownr_t nRows () const;

    // Calculates the parallactic angle of the array as a whole
    // at the specified time.

    virtual casacore::Float parang0(casacore::Double time) const;

    // Calculates the parallactic angle of each antenna in the
    // array at the specified time.

    virtual const casacore::Vector<casacore::Float> & parang(casacore::Double time) const; // [nA]

    // Returns the phase center of the array for the specified
    // row.

    virtual const casacore::MDirection& phaseCenter () const;

    // Returns the polarization frame for the specified row.

    virtual casacore::Int polarizationFrame () const;

    virtual casacore::Int polarizationId () const;

    // The returned casacore::Vector serves as a map between the rows in
    // the VisBuffer and the row IDs in the underlying casacore::MS main
    // virtual table:  mainTableID [i] = rowIds () [ i];

    virtual const casacore::Vector<casacore::rownr_t> & rowIds () const; // [nR]

    // Returns the spectral window ID for the specified row.

    virtual casacore::Int spectralWindow () const;


protected:

    VisBufferWrapperAsync2 (VisibilityIterator2 & iter);

    VisBuffer2 * getWrappedVisBuffer () const;
    void wrap (VisBuffer2 * vba);

private:

    VisBuffer2 * wrappedVb_p;
    VisibilityIterator2 * wrappedVisIterAsync_p;


};

} // end namespace vi

} //# NAMESPACE CASA - END

#endif
