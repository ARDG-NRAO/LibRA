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

#ifndef MSVIS_VISBUFFERIMPL2_H
#define MSVIS_VISBUFFERIMPL2_H

#include <casacore/casa/aips.h>

//#warning "Remove next line later"
//#    include <msvis/MSVis/VisBuffer.h>

#include <casacore/casa/Arrays/Cube.h>
#include <casacore/casa/Arrays/Vector.h>
#include <casacore/casa/Arrays/Matrix.h>
#include <casacore/casa/BasicSL/Complex.h>
#include <msvis/MSVis/VisBufferComponents2.h>
#include <msvis/MSVis/VisBuffer2.h>

using casa::vi::VisBufferComponent2;
using casa::vi::VisBufferComponents2;

namespace casa { //# NAMESPACE CASA - BEGIN

class VisModelDataI;

namespace asyncio {
    class VLAT;
} // end namespace asyncio

namespace ms {
    class Vbi2MsRow;
}

namespace vi {

//#forward

class VbCacheItemBase;
class VisBufferCache;
class VisBufferState;
class VisBufferImpl2;
class ViImplementation2;

namespace avg {

    class VbAvg;
}

namespace tavg {

    class VbAvg;
}



//<summary>VisBufferImpls encapsulate one chunk of visibility data for processing.</summary>
//
// <use visibility=export>
//
// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//   <li> <linkto class="VisSet">VisSet</linkto>
//   <li> <linkto class="ViImplementation2">ViImplementation2</linkto>
//   <li> <linkto class="VisBufferImplAutoPtr">VisBufferImplAutoPtr</linkto>
//   <li> <linkto class="VbDirtyComponents">VbDirtyComponents</linkto>
// </prerequisite>
//
// <etymology>
// VisBufferImpl is a buffer for visibility data
// </etymology>
//
//<synopsis>
// This class contains 'one iteration' of the
// <linkto class="ViImplementation2">ViImplementation2</linkto>
// It is a modifiable
// buffer of values to which calibration and averaging can be applied.
// This allows processing of the data in larger blocks, avoiding some
// overheads for processing per visibility point or spectrum.
//
// See <linkto module="MeasurementEquations">MeasurementEquations</linkto>
// for more details on how the VisBufferImpl is to be used.
//
// When the user intends to allow asynchronous I/O they will need to use
// the VisBufferImplAsync and VisBufferImplAsyncWrapper classes; these are
// publicly derived from VisBufferImpl.  Normally, the user should not
// explicitly use these classes but should use the helper class
// VisBufferImplAutoPtr which will ensure that the appropriate class is
// used.
//
//</synopsis>

//<todo>
//</todo>

class VisBufferImpl2 : public VisBuffer2 {

    friend class VLAT; // for async i/o
    friend class casa::vi::avg::VbAvg;
    friend class casa::vi::tavg::VbAvg;
    friend class AveragingTvi2;
    friend class VbCacheItemBase;
    friend class VisBufferCache;
    friend class VisBufferState;
    friend class casa::vi::VisBuffer2;
	friend class VisBuffer2Adapter;
    friend class VisBufferImpl2Async; // for async i/o
    friend class VisBufferImpl2AsyncWrapper; // for async i/o
    friend class ViReadImpl;
    friend class ViReadImplAsync; // for async I/O
    friend class casa::ms::Vbi2MsRow;

public:

	// Create empty VisBufferImpl2 you can assign to or attach.
	VisBufferImpl2 (VisBufferOptions options = VbNoOptions);
	// Construct VisBufferImpl2 for a particular ViImplementation2
	// The buffer will remain synchronized with the iterator.

	VisBufferImpl2(ViImplementation2 * iter, VisBufferOptions options);

    // Destructor (detaches from VisIter)

    virtual ~VisBufferImpl2();

    void appendRow (ms::Vbi2MsRow * rowSrc, casacore::Int initialCapacity,
                    const VisBufferComponents2 & optionalComponentsToCopy,
                    bool doCopy = true);
    void appendRowsComplete ();
    casacore::Int appendSize () const;

    // Copies all of the components (or just the one in the cache) from
    // the specified VisBuffer into this one.

    virtual void copy (const VisBuffer2 & other, casacore::Bool fetchIfNeeded = true);

    // Copies the specified components (or just the one in the cache) from
    // the specified VisBuffer into this one.

    virtual void copyComponents (const VisBuffer2 & other,
				                 const VisBufferComponents2 & components,
                                 casacore::Bool allowShapeChange = false,
				                 casacore::Bool fetchIfNeeded = true);

    virtual void copyCoordinateInfo (const VisBuffer2 * vb, casacore::Bool dirDependent,
                                     casacore::Bool allowShapeChange = false, casacore::Bool fetchIfNeeded = true);

    virtual casacore::Vector<casacore::Stokes::StokesTypes> getCorrelationTypesDefined () const;
    virtual casacore::Vector<casacore::Stokes::StokesTypes> getCorrelationTypesSelected () const;

    virtual casacore::Vector<casacore::Int> getCorrelationTypes () const;
    virtual casacore::String getFillErrorMessage () const;
    virtual casacore::Double getFrequency (casacore::Int rowInBuffer, casacore::Int frequencyIndex, casacore::Int frame = FrameNotSpecified) const;
    virtual const casacore::Vector<casacore::Double> & getFrequencies (casacore::Int rowInBuffer,
                                                   casacore::Int frame = FrameNotSpecified) const;
    virtual casacore::Int getChannelNumber (casacore::Int rowInBuffer, casacore::Int frequencyIndex) const;
    virtual const casacore::Vector<casacore::Int> & getChannelNumbers (casacore::Int rowInBuffer) const;
    virtual casacore::Vector<casacore::Int> getChannelNumbersSelected (casacore::Int outputChannelIndex) const;

    ms::MsRow * getRow (casacore::Int row) const;
    ms::MsRow * getRowMutable (casacore::Int row);

    virtual void invalidate();

    virtual casacore::Bool isAttached () const;
    virtual casacore::Bool isFillable () const;
    virtual void setShape (casacore::Int nCorrelations, casacore::Int nChannels, casacore::rownr_t nRows, casacore::Bool clearTheCache = true);
    virtual void validateShapes () const;

    virtual void writeChangesBack ();
    virtual void initWeightSpectrum (const casacore::Cube<casacore::Float>& wtspec);
    virtual void initSigmaSpectrum (const casacore::Cube<casacore::Float>& sigspec);

    // Reference to underlying MS, via the Vii
    virtual const casacore::MeasurementSet& ms() const;
    virtual const vi::SubtableColumns & subtableColumns () const;
	virtual casacore::Bool existsColumn (VisBufferComponent2 id) const;

    //--> This needs to be removed: virtual casacore::Bool fetch(const asyncio::PrefetchColumns *pfc);

    // feed1_pa() and feed2_pa() return an array of parallactic angles
    // (each corresponds to the first receptor of the feed) one for each
    // row in the current buffer. In contrast, feed_pa() calculates
    // the angles for each antenna. These methods are implemented for
    // VisBufferImpl2 only to benefit from caching of the feed and antenna IDs.

    // Note that feed_pa is a function instead of a cached value
    virtual const casacore::Vector<casacore::Float> & feedPa (casacore::Double time) const;

    // NOMINAL parallactic angle (feed p.a. offset NOT included)
    virtual casacore::Float parang0(casacore::Double time) const;
    virtual const casacore::Vector<casacore::Float> & parang(casacore::Double time) const;

    virtual casacore::MDirection azel0(casacore::Double time) const; // function rather than cached value
    virtual const casacore::Vector<casacore::MDirection> & azel(casacore::Double time) const;

    // Hour angle for specified time
    virtual casacore::Double hourang(casacore::Double time) const;

    // casacore::Sort/unsort the correlations, if necessary
    //  (Rudimentary handling of non-canonically sorted correlations--use with care!)
    virtual void sortCorr ();
    virtual void unSortCorr();

    // Normalize the visCube by the modelVisCube
    //   (and optionally also divide visCube_p by its normalized amp)

    virtual void normalize ();

    // Rotate visibility phase for given vector (dim = nrow of vb) of phases (metres)
    virtual void phaseCenterShift(const casacore::Vector<casacore::Double>& phase);
    // Rotate visibility phase for phase center offsets (arcsecs)
    virtual void phaseCenterShift(casacore::Double dx, casacore::Double dy);

    // Fill weightMat according to sigma column
    virtual void resetWeightsUsingSigma ();//virtual void resetWeightMat();

    // Rotate visibility phase for given vector (dim = nrow of vb) of phases (metres)

    // Update coordinate info - useful for copied VisBufferImpl2s that need
    // to retain some state for later reference.
    // Presently this fills antenna, array, field and spectralWindow ids, time,
    // frequency and number of rows.
    // if dirDependent is set to false the expensive direction dependent calculation of parallactic or direction of
    // antenna from pointing table is avoided
    //Add more as needed.

    virtual casacore::Bool isNewArrayId () const;
    virtual casacore::Bool isNewFieldId () const;
    virtual casacore::Bool isNewMs() const;
    virtual casacore::Bool isNewSpectralWindow () const;
    virtual casacore::Bool isRekeyable () const;
    virtual casacore::Bool isWritable () const;
    virtual casacore::IPosition getShape () const;
    virtual casacore::Int msId() const;
    virtual casacore::String msName (casacore::Bool stripPath = false) const;
    virtual Subchunk getSubchunk () const;

    virtual casacore::Bool areCorrelationsSorted() const;
    virtual casacore::IPosition getValidShape (casacore::Int) const;
    virtual casacore::Vector<casacore::IPosition> getValidVectorShapes (casacore::Int) const;

    virtual VisModelDataI * getVisModelData() const;


    //////////////////////////////////////////////////////////////////////
    //
    //  casacore::Data accessors and setters (where appropriate)
    //
    //  There are the methods that allows access to the items cached in the
    //  VisBuffer.  The straight accessors provide read-only access to the
    //  item.  Where the item is allowed to be modified, one or more set
    //  methods are provided.  N.B.: the VB usually caches the information
    //  in the representation requested so that using a setter to modify
    //  a value (e.g., weight vs. weightMat) will not modify the cached value
    //  in a different representation.

    virtual const casacore::Vector<casacore::Int> & antenna1 () const;
    virtual void setAntenna1 (const casacore::Vector<casacore::Int> & value);
    virtual const casacore::Vector<casacore::Int> & antenna2 () const;
    virtual void setAntenna2 (const casacore::Vector<casacore::Int> & value);
    virtual const casacore::Vector<casacore::Int> & arrayId () const;
    virtual void setArrayId (const casacore::Vector<casacore::Int> &);
    virtual const casacore::Vector<casacore::SquareMatrix<casacore::Complex, 2> > & cjones () const;
    virtual const casacore::Vector<casacore::Int> & correlationTypes () const;
//    virtual casacore::Int dataDescriptionId () const;
//    virtual void setDataDescriptionId (casacore::Int value);
    virtual const casacore::Vector<casacore::Int> & dataDescriptionIds () const;
    virtual void setDataDescriptionIds (const casacore::Vector<casacore::Int> & ); // [nR]
    virtual const casacore::Vector<casacore::MDirection> & direction1 () const;
    virtual const casacore::Vector<casacore::MDirection> & direction2 () const;
    virtual const casacore::Vector<casacore::Double> & exposure () const;
    virtual void setExposure (const casacore::Vector<casacore::Double> & value);
    virtual const casacore::Vector<casacore::Int> & feed1 () const;
    virtual void setFeed1 (const casacore::Vector<casacore::Int> & value);
    virtual const casacore::Vector<casacore::Float> & feedPa1 () const;
    virtual const casacore::Vector<casacore::Int> & feed2 () const;
    virtual void setFeed2 (const casacore::Vector<casacore::Int> & value);
    virtual const casacore::Vector<casacore::Float> & feedPa2 () const;
    virtual const casacore::Vector<casacore::Int> & fieldId () const;
    virtual void setFieldId (const casacore::Vector<casacore::Int> &);
//    virtual const casacore::Matrix<casacore::Bool> & flag () const;
//    virtual void setFlag (const casacore::Matrix<casacore::Bool>&);
    virtual const casacore::Array<casacore::Bool> & flagCategory () const;
    virtual void setFlagCategory (const casacore::Array<casacore::Bool>&);
    virtual const casacore::Vector<casacore::Array<casacore::Bool>> & flagCategories () const;
    virtual void setFlagCategories (const casacore::Vector<casacore::Array<casacore::Bool>>& value);
    virtual const casacore::Cube<casacore::Bool> & flagCube () const;
    virtual void setFlagCube (const casacore::Cube<casacore::Bool>&);
    virtual const casacore::Vector<casacore::Cube<casacore::Bool>> & flagCubes () const;
    virtual void setFlagCubes (const casacore::Vector<casacore::Cube<casacore::Bool>>& value);
    virtual const casacore::Vector<casacore::Bool> & flagRow () const;
    virtual void setFlagRow (const casacore::Vector<casacore::Bool>&);
    virtual const casacore::Matrix<casacore::Float> & imagingWeight () const;
    void setImagingWeight (const casacore::Matrix<casacore::Float> & newImagingWeights);
    virtual casacore::Int nAntennas () const;
    virtual casacore::Int nChannels () const;
    virtual casacore::Int nCorrelations () const;
    virtual casacore::rownr_t nRows () const;
    virtual casacore::rownr_t nShapes () const;
    virtual const casacore::Vector<casacore::rownr_t>& nRowsPerShape () const;
    virtual const casacore::Vector<casacore::Int>& nChannelsPerShape () const;
    virtual const casacore::Vector<casacore::Int>& nCorrelationsPerShape () const;
    virtual const casacore::Vector<casacore::Int> & observationId () const;
    virtual void setObservationId (const casacore::Vector<casacore::Int> & value);
    virtual const casacore::MDirection& phaseCenter () const;
    virtual casacore::Int polarizationFrame () const;
    virtual casacore::Int polarizationId () const;
    virtual const casacore::Vector<casacore::Int> & processorId () const;
    virtual void setProcessorId (const casacore::Vector<casacore::Int> & value);
    virtual const casacore::Vector<casacore::rownr_t> & rowIds () const;
    virtual const casacore::Vector<casacore::Int> & scan () const;
    virtual void setScan (const casacore::Vector<casacore::Int> & value);
    virtual const casacore::Matrix<casacore::Float> & sigma () const;
    virtual void setSigma (const casacore::Matrix<casacore::Float> &);
    virtual const casacore::Vector<casacore::Matrix<casacore::Float>> & sigmas () const;
    virtual void setSigmas (const casacore::Vector<casacore::Matrix <casacore::Float>> & value);
    //virtual const casacore::Matrix<casacore::Float> & sigmaMat () const;
    virtual const casacore::Vector<casacore::Int> & spectralWindows () const;
    virtual void setSpectralWindows (const casacore::Vector<casacore::Int> & spectralWindows);
    virtual const casacore::Vector<casacore::Int> & stateId () const;
    virtual void setStateId (const casacore::Vector<casacore::Int> & value);
    virtual const casacore::Vector<casacore::Double> & time () const;
    virtual void setTime (const casacore::Vector<casacore::Double> & value);
    virtual const casacore::Vector<casacore::Double> & timeCentroid () const;
    virtual void setTimeCentroid (const casacore::Vector<casacore::Double> & value);
    virtual const casacore::Vector<casacore::Double> & timeInterval () const;
    virtual void setTimeInterval (const casacore::Vector<casacore::Double> & value);
    virtual const casacore::Matrix<casacore::Double> & uvw () const;
    virtual void setUvw (const casacore::Matrix<casacore::Double> & value);
    virtual const casacore::Cube<casacore::Complex> & visCubeCorrected () const;
    virtual void setVisCubeCorrected (const casacore::Cube<casacore::Complex> &);
//    virtual const casacore::Matrix<CStokesVector> & visCorrected () const;
//    virtual void setVisCorrected (const casacore::Matrix<CStokesVector> &);
    virtual const casacore::Vector<casacore::Cube<casacore::Complex>> & visCubesCorrected () const;
    virtual void setVisCubesCorrected (const casacore::Vector<casacore::Cube<casacore::Complex>> &);
    virtual const casacore::Cube<casacore::Float> & visCubeFloat () const;
    virtual void setVisCubeFloat (const casacore::Cube<casacore::Float> &);
    virtual const casacore::Vector<casacore::Cube<casacore::Float>> & visCubesFloat () const;
    virtual void setVisCubesFloat (const casacore::Vector<casacore::Cube<casacore::Float>> &);
    virtual const casacore::Cube<casacore::Complex> & visCubeModel () const;
    virtual void setVisCubeModel (const casacore::Complex & c);
    virtual void setVisCubeModel (const casacore::Cube<casacore::Complex>& vis);
//    virtual void setVisCubeModel(const casacore::Vector<casacore::Float>& stokes);
    virtual void setVisCubeModel(const casacore::Vector<casacore::Float>& stokes);  // by supplied Stokes vector
    virtual const casacore::Vector<casacore::Cube<casacore::Complex>> & visCubesModel () const;
    //virtual void setVisCubesModel (const casacore::Complex & c);
    virtual void setVisCubesModel(const casacore::Vector<casacore::Cube<casacore::Complex>>& vis);
//    virtual void setVisModel (casacore::Matrix<CStokesVector> &);
    virtual const casacore::Cube<casacore::Complex> & visCube () const;
    virtual void setVisCube(const casacore::Complex & c);
    virtual void setVisCube (const casacore::Cube<casacore::Complex> &);
    virtual const casacore::Vector<casacore::Cube<casacore::Complex>> & visCubes () const;
    virtual void setVisCubes (const casacore::Vector<casacore::Cube<casacore::Complex>> &);
//    virtual const casacore::Matrix<CStokesVector> & vis () const;
//    virtual void setVis (casacore::Matrix<CStokesVector> &);
    virtual const casacore::Matrix<casacore::Float> & weight () const;
    virtual void setWeight (const casacore::Matrix<casacore::Float>&);
    virtual const casacore::Vector<casacore::Matrix<casacore::Float>> & weights () const;
    virtual void setWeights (const casacore::Vector<casacore::Matrix <casacore::Float>>& value);
    virtual const casacore::Cube<casacore::Float> & weightSpectrum () const;
    virtual void setWeightSpectrum (const casacore::Cube<casacore::Float>&);
    virtual const casacore::Vector<casacore::Cube<casacore::Float>> & weightSpectra () const;
    virtual void setWeightSpectra (const casacore::Vector<casacore::Cube<casacore::Float>>& value);
    virtual const casacore::Cube<casacore::Float> & sigmaSpectrum () const;
    virtual void setSigmaSpectrum (const casacore::Cube<casacore::Float>& value);
    virtual const casacore::Vector<casacore::Cube<casacore::Float>> & sigmaSpectra () const;
    virtual void setSigmaSpectra (const casacore::Vector<casacore::Cube<casacore::Float>>& value);


protected:

    //virtual void cacheCopy (const VisBufferImpl2 & other, casacore::Bool markAsCached);

    void adjustWeightFactorsAndFlags (casacore::Matrix <casacore::Float> & rowWeightFactors,
                                      casacore::Bool useWeightSpectrum,
                                      casacore::rownr_t nRows,
                                      casacore::Int nCorrelations,
                                      casacore::Int nChannelsOut);
    void adjustWeightAndSigmaMatrices (casacore::Int nChannelsAveraged, casacore::Int nChannelsOut, casacore::rownr_t nRows,
                                       casacore::Int nCorrelations, casacore::Int nChannelsSelected,
                                       const casacore::Matrix <casacore::Float> & rowWeightFactors);

    casacore::Int averageChannelFrequencies (casacore::Int nChannelsOut,
                                   const casacore::Vector<casacore::Int> & channels,
                                   const casacore::Matrix<casacore::Int> & averagingBounds);

    void averageFlagInfoChannels (const casacore::Matrix<casacore::Int> & averagingBounds,
                                  casacore::Int nChannelsOut, casacore::Bool useWeightSpectrum);

    template<typename T>
    void averageVisCubeChannels (T & dataCache,
                                 casacore::Int nChanOut,
                                 const casacore::Matrix<casacore::Int>& averagingbounds);

    virtual void cacheClear (casacore::Bool markAsCached = false);
    virtual void cacheResizeAndZero (const VisBufferComponents2 & exclusions = VisBufferComponents2());

    virtual void checkVisIter (const char * func, const char * file, int line, const char * extra = "") const;
    void computeRowWeightFactors (casacore::Matrix <casacore::Float> & rowWeightFactors, casacore::Bool useWeightSpectrum);
    virtual void configureNewSubchunk (casacore::Int msId, const casacore::String & msName, casacore::Bool isNewMs,
                                       casacore::Bool isNewArrayId, casacore::Bool isNewFieldId,
                                       casacore::Bool isNewSpectralWindow, const Subchunk & subchunk,
                                       const casacore::Vector<casacore::rownr_t>& nRowsPerShape,
                                       const casacore::Vector<casacore::Int>& nChannelsPerShape, 
                                       const casacore::Vector<casacore::Int>& nCorrelationsPerShape,
                                       const casacore::Vector<casacore::Int> & correlations,
                                       const casacore::Vector<casacore::Stokes::StokesTypes> & correlationsDefined,
                                       const casacore::Vector<casacore::Stokes::StokesTypes> & correlationsSelected,
                                       casacore::CountedPtr<WeightScaling> weightScaling);

    virtual void copyRow (casacore::Int sourceRow, casacore::Int destinationRow);
    virtual void dirtyComponentsAdd (const VisBufferComponents2 & additionalDirtyComponents);
    virtual void dirtyComponentsAdd (VisBufferComponent2 component);
    virtual void dirtyComponentsClear ();
    virtual VisBufferComponents2 dirtyComponentsGet () const;
    virtual void dirtyComponentsSet (const VisBufferComponents2 & dirtyComponents);
    virtual void dirtyComponentsSet (VisBufferComponent2 component);
    virtual casacore::Bool hasShape () const;
    void normalizeRow (casacore::Int row, casacore::Int nCorrelations, const casacore::Cube<casacore::Bool> & flagged,
                       casacore::Cube<casacore::Complex> & visCube, casacore::Cube<casacore::Complex> & modelCube,
                        casacore::Matrix<casacore::Float> & weightMat);

    virtual void sortCorrelationsAux (casacore::Bool makeSorted);
    virtual ViImplementation2 * getViiP () const; // protected, non-const access to VI
    void registerCacheItem (VbCacheItemBase *);
    virtual void stateCopy (const VisBufferImpl2 & other); // copy relevant noncached members
    virtual void setFillable (casacore::Bool isFillable);
    virtual void setRekeyable (casacore::Bool isRekeyable);
    virtual bool setWritability (bool newWritability); //  override;
    virtual void setupValidShapes ();
    template <typename Coord>
    void updateCoord (Coord & item, const Coord & otherItem);

    virtual casacore::Vector<casacore::Bool> & flagRowRef ();  // [nR]
    virtual casacore::Cube<casacore::Bool> & flagCubeRef ();  // [nC,nF,nR]
    virtual casacore::Vector<casacore::Cube<casacore::Bool>> & flagCubesRef ();  // [nC,nF,nR]
    virtual casacore::Cube<casacore::Complex> & visCubeRef (); // [nC,nF,nR]
    virtual casacore::Vector<casacore::Cube<casacore::Complex>> & visCubesRef (); // [nC,nF,nR]
    virtual casacore::Cube<casacore::Complex> & visCubeCorrectedRef (); // [nC,nF,nR]
    virtual casacore::Vector<casacore::Cube<casacore::Complex>> & visCubesCorrectedRef (); // [nC,nF,nR]
    virtual casacore::Cube<casacore::Complex> & visCubeModelRef (); // [nC,nF,nR]
    virtual casacore::Vector<casacore::Cube<casacore::Complex>> & visCubesModelRef (); // [nC,nF,nR]
    virtual casacore::Cube<casacore::Float> & weightSpectrumRef (); // [nC,nF,nR]
    virtual casacore::Vector<casacore::Cube<casacore::Float>> & weightSpectraRef (); // [nC,nF,nR]
    virtual casacore::Cube<casacore::Float> & sigmaSpectrumRef (); // [nC,nF,nR]
    virtual casacore::Vector<casacore::Cube<casacore::Float>> & sigmaSpectraRef (); // [nC,nF,nR]

    casacore::Float getWeightScaled (casacore::Int row) const;
    casacore::Float getWeightScaled (casacore::Int correlation, casacore::Int row) const;
    casacore::Float getWeightScaled (casacore::Int correlation, casacore::Int channel, casacore::Int row) const;
    casacore::Float getSigmaScaled (casacore::Int row) const;
    casacore::Float getSigmaScaled (casacore::Int correlation, casacore::Int row) const;
    casacore::Float getSigmaScaled (casacore::Int correlation, casacore::Int channel, casacore::Int row) const;
    virtual casacore::CountedPtr<WeightScaling> getWeightScaling () const;
    void setIterationInfo (casacore::Int msId, const casacore::String & msName, casacore::Bool isNewMs,
                           casacore::Bool isNewArrayId, casacore::Bool isNewFieldId, casacore::Bool isNewSpectralWindow,
                           const Subchunk & subchunk, const casacore::Vector<casacore::Int> & correlations,
                           const casacore::Vector<casacore::Stokes::StokesTypes> & correlationsDefined,
                           const casacore::Vector<casacore::Stokes::StokesTypes> & correlationsSelected,
                           casacore::CountedPtr <WeightScaling> weightScaling);
    casacore::Bool weightSpectrumPresent () const;
    casacore::Bool sigmaSpectrumPresent () const;

private:

    virtual casacore::Bool areCorrelationsInCanonicalOrder () const;
    void checkVisIterBase (const char * func, const char * file, int line, const char * extra = "") const;
    void construct(ViImplementation2 * vi, VisBufferOptions options);
    void constructCache();
    virtual void validate();

    /////////////////////////////////////////
    //
    //  Fillers
    //
    // These methods fill the needed value into the cache.  This
    // usually means getting it from the attached ViImplementation2
    // but the needed value can be generated as well.
    //
    // The name of the filler should match up with the accessor method
    // (see above) as well as the member name in the cache (see
    // VisBufferCache class in VisBuffer.cc).


    virtual void fillAntenna1 (casacore::Vector<casacore::Int>& value) const;
    virtual void fillAntenna2 (casacore::Vector<casacore::Int>& value) const;
    virtual void fillArrayId (casacore::Vector<casacore::Int>& value) const;
    virtual void fillCorrType (casacore::Vector<casacore::Int>& value) const;
    virtual void fillCubeCorrected (casacore::Cube <casacore::Complex> & value) const;
    virtual void fillCubesCorrected (casacore::Vector<casacore::Cube <casacore::Complex>> & value) const;
    virtual void fillCubeModel (casacore::Cube <casacore::Complex> & value) const;
    virtual void fillCubesModel (casacore::Vector<casacore::Cube <casacore::Complex>> & value) const;
    virtual void fillCubeObserved (casacore::Cube <casacore::Complex> & value) const;
    virtual void fillCubesObserved (casacore::Vector<casacore::Cube <casacore::Complex>> & value) const;
    virtual void fillDataDescriptionId  (casacore::Int& value) const;
    virtual void fillDataDescriptionIds  (casacore::Vector<casacore::Int>& value) const;
    virtual void fillDirection1 (casacore::Vector<casacore::MDirection>& value) const;
    virtual void fillDirection2 (casacore::Vector<casacore::MDirection>& value) const;
    virtual void fillDirectionAux (casacore::Vector<casacore::MDirection>& value,
                           const casacore::Vector<casacore::Int> & antenna,
                           const casacore::Vector<casacore::Int> &feed,
                           const casacore::Vector<casacore::Float> & feedPa) const;
    virtual void fillExposure (casacore::Vector<casacore::Double>& value) const;
    virtual void fillFeed1 (casacore::Vector<casacore::Int>& value) const;
    virtual void fillFeed2 (casacore::Vector<casacore::Int>& value) const;
    virtual void fillFeedPa1 (casacore::Vector <casacore::Float> & feedPa) const;
    virtual void fillFeedPa2 (casacore::Vector <casacore::Float> & feedPa) const;
    virtual void fillFeedPaAux (casacore::Vector <casacore::Float> & feedPa,
                        const casacore::Vector <casacore::Int> & antenna,
                        const casacore::Vector <casacore::Int> & feed) const;
    virtual void fillFieldId (casacore::Vector<casacore::Int>& value) const;
    virtual void fillFlag (casacore::Matrix<casacore::Bool>& value) const;
    virtual void fillFlagCategory (casacore::Array<casacore::Bool>& value) const;
    virtual void fillFlagCube (casacore::Cube<casacore::Bool>& value) const;
    virtual void fillFlagCubes (casacore::Vector<casacore::Cube<casacore::Bool>>& value) const;
    virtual void fillFlagRow (casacore::Vector<casacore::Bool>& value) const;
    virtual void fillFloatData (casacore::Cube<casacore::Float>& value) const;
    virtual void fillFloatCubes (casacore::Vector<casacore::Cube<casacore::Float>>& value) const;
    virtual void fillImagingWeight (casacore::Matrix<casacore::Float> & value) const;
    virtual void fillJonesC (casacore::Vector<casacore::SquareMatrix<casacore::Complex, 2> >& value) const;
    virtual void fillNAntennas (casacore::Int& value) const;
    virtual void fillNChannel (casacore::Int& value) const;
    virtual void fillNCorr (casacore::Int& value) const;
    virtual void fillNRow (casacore::Int& value) const;
    virtual void fillNShapes (casacore::Int& value) const;
    virtual void fillNRowPerShape (casacore::Vector<casacore::rownr_t>& value) const;
    virtual void fillNChannelPerShape (casacore::Vector<casacore::Int>& value) const;
    virtual void fillNCorrPerShape (casacore::Vector<casacore::Int>& value) const;
    virtual void fillObservationId (casacore::Vector<casacore::Int>& value) const;
    virtual void fillPhaseCenter (casacore::MDirection& value) const;
    virtual void fillPolFrame (casacore::Int& value) const;
    virtual void fillPolarizationId (casacore::Int& value) const;
    virtual void fillProcessorId (casacore::Vector<casacore::Int>& value) const;
    virtual void fillRowIds (casacore::Vector<casacore::rownr_t>& value) const;
    virtual void fillScan (casacore::Vector<casacore::Int>& value) const;
    virtual void fillSigma (casacore::Matrix<casacore::Float>& value) const;
    virtual void fillSigmas (casacore::Vector<casacore::Matrix<casacore::Float>>& value) const;
    //virtual void fillSigmaMat (casacore::Matrix<casacore::Float>& value) const;
    //virtual void fillSpectralWindow (casacore::Int& value) const;
    virtual void fillSpectralWindows  (casacore::Vector<casacore::Int>& value) const;
    virtual void fillStateId (casacore::Vector<casacore::Int>& value) const;
    virtual void fillTime (casacore::Vector<casacore::Double>& value) const;
    virtual void fillTimeCentroid (casacore::Vector<casacore::Double>& value) const;
    virtual void fillTimeInterval (casacore::Vector<casacore::Double>& value) const;
    virtual void fillUvw (casacore::Matrix<casacore::Double>& value) const;
//    virtual void fillVisibilityCorrected (casacore::Matrix<CStokesVector>& value) const;
//    virtual void fillVisibilityModel (casacore::Matrix<CStokesVector>& value) const;
//    virtual void fillVisibilityObserved (casacore::Matrix<CStokesVector>& value) const;
    virtual void fillWeight (casacore::Matrix<casacore::Float>& value) const;
    virtual void fillWeights (casacore::Vector<casacore::Matrix<casacore::Float>>& value) const;
    //virtual void fillWeightMat (casacore::Matrix<casacore::Float>& value) const;
    virtual void fillWeightSpectrum (casacore::Cube<casacore::Float>& value) const;
    virtual void fillWeightSpectra (casacore::Vector<casacore::Cube<casacore::Float>>& value) const;
    virtual void fillSigmaSpectrum (casacore::Cube<casacore::Float>& value) const;
    virtual void fillSigmaSpectra (casacore::Vector<casacore::Cube<casacore::Float>>& value) const;

    VisBufferCache * cache_p;
    casa::ms::Vbi2MsRow * msRow_p;
    VisBufferState * state_p;
};

} // end namespace vi

} // end namespace casa


#endif

