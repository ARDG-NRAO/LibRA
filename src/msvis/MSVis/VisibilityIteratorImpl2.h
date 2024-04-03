// -*- mode: c++ -*-
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
//# ANY WARRANTY; without even the Implied warranty of MERCHANTABILITY or
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
//# $Id: VisibilityIterator2.h,v 19.14 2006/02/28 04:48:58 mvoronko Exp $

#ifndef MSVIS_VISIBILITYITERATORIMPL2_H
#define MSVIS_VISIBILITYITERATORIMPL2_H

#include <casacore/casa/aips.h>
#include <casacore/ms/MSOper/MSDerivedValues.h>
#include <msvis/MSVis/ViImplementation2.h>
#include <msvis/MSVis/ViColumns2.h>
#include <casacore/tables/Tables/ArrayColumn.h>
#include <casacore/tables/Tables/ScalarColumn.h>
#include <msvis/MSVis/VisImagingWeight.h>
#include <msvis/MSVis/VisBuffer2.h>

#include <tuple>
#include <map>
#include <vector>
#include <mutex>
#include <memory>

namespace casacore{

template <typename T> class ArrayColumn;
template <typename T> class CountedPtr;
class VisModelDataI;
template <typename T> class ArrayColumn;
template <typename T, Int N> class RigidVector;
template <typename T, Int N> class SquareMatrix;
}

namespace casa { //# NAMESPACE CASA - BEGIN

class MsIter;
class VisModelDataI;

namespace vi {

//# forward decl

class VisBuffer2;

class ChannelSelector;
class ChannelSelectorCache;
class PointingDirectionCache;
class PointingSource;
class SpectralWindowChannelsCache;
class SpectralWindowChannels;
class SubtableColumns;

class VisibilityIteratorImpl2 : public ViImplementation2 {

	friend class AsyncEnabler;
	friend class ViImplAsync2;
	friend class VisibilityIterator2;
	friend class VLAT; // allow VI lookahead thread class to access protected functions
	friend class VisBuffer2Adapter;
	// VLAT should not access private parts, especially variables

public:

	using DataColumn = VisibilityIterator2::DataColumn;
	using ChannelInfo = std::tuple<
	    casacore::Vector<casacore::Int>, casacore::Vector<casacore::Int>,
	    casacore::Vector<casacore::Int>, casacore::Vector<casacore::Int>
	>;

	// Default constructor - useful only to assign another iterator later
	////VisibilityIteratorImpl2 ();

	// Construct from an casacore::MS and a casacore::Block of casacore::MS
	// column enums specifying the iteration order.  If no order is specified,
	// it uses the default sort order of MSIter, which is not necessarily the
	// raw order of ms!  The default ordering is ARRAY_ID, FIELD_ID,
	// DATA_DESC_ID, and TIME, but check MSIter.h to be sure.  These columns
	// will be added first if they are not specified.
	//
	// An optional timeInterval (in seconds) can be given to iterate through
	// chunks of time.  The default interval of 0 groups all times together.
	// Every 'chunk' of data contains all data within a certain time interval
	// and with identical values of the other iteration columns (e.g.
	// DATA_DESC_ID and FIELD_ID).  Using selectChannel (), a number of groups
	// of channels can be requested.  At present the channel group iteration
	// will always occur before the interval iteration.
//    VisibilityIteratorImpl (const casacore::MeasurementSet & ms,
//                                const casacore::Block<casacore::Int> & sortColumns,
//                                casacore::Double timeInterval = 0);

	// Same as above, but with the option of using the raw order of ms
	// (addDefaultSortCols=false).
//    VisibilityIteratorImpl (const casacore::MeasurementSet & ms,
//                                const casacore::Block<casacore::Int> & sortColumns, const casacore::Bool addDefaultSortCols,
//                                casacore::Double timeInterval = 0);

	// Same as previous constructors, but with multiple MSs to iterate over.
//    VisibilityIteratorImpl (const casacore::Block<casacore::MeasurementSet> & mss,
//                                const casacore::Block<casacore::Int> & sortColumns,
//                                casacore::Double timeInterval = 0);

    VisibilityIteratorImpl2(
        const casacore::Block<const casacore::MeasurementSet *> & mss,
        const SortColumns & sortColumns,
        casacore::Double timeInterval,
        casacore::Bool isWritable,
        casacore::Bool useMSIter2=false);

    // This constructor is similar to previous one but it allows to explicitely
    // define the sorting criteria used for chunk iteration and for subchunk
    // iteration. Also the criteria can be generic functions
    VisibilityIteratorImpl2(
        const casacore::Block<const casacore::MeasurementSet *> & mss,
        const SortColumns & chunkSortColumns,
        const SortColumns & subchunkSortColumns,
        bool isWritable);

	VisibilityIteratorImpl2(const VisibilityIteratorImpl2& vii);

	VisibilityIteratorImpl2 & operator=(const VisibilityIteratorImpl2& vii);

	VisibilityIteratorImpl2(VisibilityIteratorImpl2&& vii);

	VisibilityIteratorImpl2 & operator=(VisibilityIteratorImpl2&& vii);

	// Destructor

	virtual ~VisibilityIteratorImpl2();

	// Members

	std::unique_ptr<VisibilityIteratorImpl2> clone() const;

    // Set the scope of metadata, i.e., how long a given metadata is valid.
    // For instance, if ddIDScope = SubchunkScope, the ddId will be unique
    // within a subchunk.
    void setMetadataScope();

	// Report the the ViImplementation type
	//  TBD:  indicate writable?
	virtual casacore::String
	ViiType() const  override {
		return casacore::String("DiskIO()");
	};

	virtual casacore::Bool
	isWritable() const override;

	// Reset iterator to origin/start of data (of current chunk)
	virtual void
	origin() override;

	// Reset iterator to true start of data (first chunk)
	virtual void
	originChunks();

	// Return the time interval (in seconds) used for iteration.  This is not
	// the same as the INTERVAL column.

	virtual casacore::Double
	getInterval() const override;

	// Set or reset the time interval (in seconds) to use for iteration.  You
	// should call originChunks() to reset the iteration after calling this.
	virtual void
	setInterval(casacore::Double timeInterval) override;

    // Set the 'blocking' size for returning data. If set to 0 (the default),
    // the chunk will be grouped in subchunks using the subchunk sorting functions
    // (which default to group by unique timestamp.
    // With blocking set, up to nRows can be returned in one go.
    // The chunk size determines the actual maximum.
    virtual void
    setRowBlocking(casacore::rownr_t nRows = 0) override;

	virtual casacore::rownr_t
	getRowBlocking() const override;

        virtual casacore::Bool
	existsColumn(VisBufferComponent2 id) const override;

	// Return false if no more data (in current chunk)
	virtual casacore::Bool
	more() const override;

	virtual Subchunk
	getSubchunkId() const override;

	virtual const SortColumns &
	getSortColumns() const override;

	virtual void
	setFrequencySelections(const FrequencySelections & selection) override;

	// Return false if no more 'Chunks' of data left
	virtual casacore::Bool
	moreChunks() const override;

	virtual void
	result(casacore::Record& res) const override;

	// Check if ms has change since last iteration

	virtual casacore::Bool
	isNewArrayId() const override;

	virtual casacore::Bool
	isNewFieldId() const override;

	virtual casacore::Bool
	isNewMs() const override;

	virtual casacore::Bool
	isNewSpectralWindow() const override;

	virtual casacore::Int
	msId() const override;

	virtual casacore::Int
	getNMs() const override;

	virtual VisBuffer2 *
	getVisBuffer() const override;

	//reference to actual ms in interator
	virtual const casacore::MeasurementSet &
	ms() const override;

	// Name of the MS in the interator
	virtual casacore::String
	msName() const override;

	// advance the iteration
	virtual void
	next() override;

	// Advance to the next Chunk of data
	virtual void
	nextChunk() override;

	// Report Name of slowest column that changes at end of current iteration
	virtual casacore::String
	keyChange() const  override {
		return msIter_p->keyChange();
	};

	// Return antenna1
	virtual void
	antenna1(casacore::Vector<casacore::Int> & ant1) const override;

	// Return antenna2
	virtual void
	antenna2(casacore::Vector<casacore::Int> & ant2) const override;

	// Return feed1
	virtual void
	feed1(casacore::Vector<casacore::Int> & fd1) const override;

	// Return feed2
	virtual void
	feed2(casacore::Vector<casacore::Int> & fd2) const override;

	// Return feed configuration matrix for specified antenna
	void
	jonesC(
		casacore::Vector<casacore::SquareMatrix<casacore::Complex, 2> > & cjones) const override;

	// Return receptor angles for all antennae and feeds First axis of the cube
	// is a receptor number, 2nd is antennaId, 3rd is feedId Note: the method is
	// intended to provide an access to MSIter::receptorAngles for VisBuffer in
	// the multi-feed case. It may be worth to change the interface of feed_pa
	// to return the information for all feeds.
	virtual const casacore::Cube<casacore::Double> &
	receptorAngles() const override;

	// return a string mount identifier for each antenna
	virtual const casacore::Vector<casacore::String> &
	antennaMounts() const override;

	// Return a cube containing pairs of coordinate offsets for each
	// receptor of each feed (values are in radians, coordinate system is fixed
	// with antenna and is the same one as used to define the BEAM_OFFSET
	// parameter in the feed table). The cube axes are receptor, antenna, feed.
	virtual const casacore::Cube<casacore::RigidVector<casacore::Double, 2> > &
	getBeamOffsets() const override;

	virtual std::pair<bool, casacore::MDirection>
	getPointingAngle(int antenna, double time) const override;

	// true if all elements of the cube returned by getBeamOffsets are zero
	virtual casacore::Bool
	allBeamOffsetsZero() const override;

	// Return feed parallactic angles casacore::Vector (nant) (1 feed/ant)
	virtual const casacore::Vector<casacore::Float> &
	feed_pa(casacore::Double time) const override;

	// Return nominal parallactic angle at specified time
	// (does not include feed position angle offset--see feed_pa)
	// A global value for all antennas (e.g., small array)
	virtual const casacore::Float &
	parang0(casacore::Double time) const override;

	// Per antenna:
	virtual const casacore::Vector<casacore::Float> &
	parang(casacore::Double time) const override;

	// Return the antenna AZ/EL casacore::Vector(nant)
	virtual casacore::MDirection
	azel0(casacore::Double time) const override;

	virtual const casacore::Vector<casacore::MDirection> &
	azel(casacore::Double time) const override;

	// Return the hour angle for the specified time
	virtual casacore::Double
	hourang(casacore::Double time) const override;

	// Return the current FieldId
	virtual void
	fieldIds(casacore::Vector<casacore::Int>&) const override;

	// Return the current ArrayId
	virtual void
	arrayIds(casacore::Vector<casacore::Int>&) const override;

	// Return the current Field Name
	virtual casacore::String
	fieldName() const override;

	// Return the current Source Name
	virtual casacore::String
	sourceName() const override;

    // Return flag for each polarization, channel and row
    virtual void
    flag(casacore::Cube<casacore::Bool> & flags) const override;

    // Return flag for each polarization, channel and row
    // Supports returning a vector of cubes.
    // If VisBuffer contains rows with different number of channels
    // each of the cubes will have a different shape
    virtual void
    flag(casacore::Vector<casacore::Cube<casacore::Bool>> & flags) const override;

	// Return flag for each channel & row
	virtual void
	flag(casacore::Matrix<casacore::Bool> & flags) const override;

	// Determine whether FLAG_CATEGORY is valid.
	casacore::Bool
	flagCategoryExists() const override;

	// Return flags for each polarization, channel, category, and row.
	virtual void
	flagCategory(casacore::Array<casacore::Bool> & flagCategories) const override;

	// Return row flag
	virtual void
	flagRow(casacore::Vector<casacore::Bool> & rowflags) const override;

	// Return scan number
	virtual void
	scan(casacore::Vector<casacore::Int> & scans) const override;

	// Return the OBSERVATION_IDs
	virtual void
	observationId(casacore::Vector<casacore::Int> & obsids) const override;

	// Return the PROCESSOR_IDs
	virtual void
	processorId(casacore::Vector<casacore::Int> & procids) const override;

	// Return the STATE_IDs
	virtual void
	stateId(casacore::Vector<casacore::Int> & stateids) const override;

	// Return the current phase center as an MDirection
	virtual const casacore::MDirection &
	phaseCenter() const override;

	// Return frame for polarization(returns PolFrame enum)
	virtual casacore::Int
	polFrame() const override;

	// Return the correlation type (returns casacore::Stokes enums)
	virtual void
	corrType(casacore::Vector<casacore::Int> & corrTypes) const override;

    // Return sigma
    virtual void
    sigma(casacore::Matrix<casacore::Float> & sig) const override;

    // Return sigma
    // Supports returning a vector of cubes.
    // If VisBuffer contains rows with different number of channels
    // each of the cubes will have a different shape
    virtual void
    sigma(casacore::Vector<casacore::Matrix<casacore::Float>> & sig) const override;

    // Return spw Ids for each row of the current iteration
    virtual void
    spectralWindows(casacore::Vector<casacore::Int> & spws) const override;

    // Return polarization Ids for each row of the current iteration
    virtual void
    polarizationIds(casacore::Vector<casacore::Int> & polIds) const override;

	// Return current Polarization Id
	virtual casacore::Int
	polarizationId() const override;

	// Return current DataDescription Id
	virtual casacore::Int
	dataDescriptionId() const override;

    // Return ddIds for each row of the current iteration
    virtual void
    dataDescriptionIds(casacore::Vector<casacore::Int> & ddis) const override;

	// Return MJD midpoint of interval.
	virtual void
	time(casacore::Vector<casacore::Double> & t) const override;

	// Return MJD centroid of interval.
	virtual void
	timeCentroid(casacore::Vector<casacore::Double> & t) const override;

	// Return nominal time interval
	virtual void
	timeInterval(casacore::Vector<casacore::Double> & ti) const override;

	// Return actual time interval
	virtual void
	exposure(casacore::Vector<casacore::Double> & expo) const override;

    // Return the visibilities as found in the casacore::MS,
    // casacore::Cube(npol,nchan,nrow).
    virtual void
    visibilityCorrected(casacore::Cube<casacore::Complex> & vis) const override;

    // Return the corrected visibilities
    // Supports returning a vector of cubes.
    // If VisBuffer contains rows with different number of channels
    // each of the cubes will have a different shape
    virtual void
    visibilityCorrected (casacore::Vector<casacore::Cube<casacore::Complex>> & vis) const override;

    // Return the observed visibilities
    virtual void
    visibilityObserved(casacore::Cube<casacore::Complex> & vis) const override;

    // Return the observed visibilities
    // Supports returning a vector of cubes.
    // If VisBuffer contains rows with different number of channels
    // each of the cubes will have a different shape
    virtual void
    visibilityObserved(casacore::Vector<casacore::Cube<casacore::Complex>> & vis) const override;

    // Return the model visibilities
    virtual void
    visibilityModel(casacore::Cube<casacore::Complex> & vis) const override;

    // Return the model visibilities
    // Supports returning a vector of cubes.
    // If VisBuffer contains rows with different number of channels
    // each of the cubes will have a different shape
    virtual void
    visibilityModel(casacore::Vector<casacore::Cube<casacore::Complex>> & vis) const override;

	// This will return all selected spwids for each ms attached with this iterator
	virtual casacore::Vector<casacore::Vector<casacore::Int> > getAllSelectedSpws() const;

    // Return FLOAT_DATA as a casacore::Cube(npol, nchan, nrow) if found in the
    // MS.
    virtual void
    floatData(casacore::Cube<casacore::Float> & fcube) const override;

    // Return FLOAT_DATA as a casacore::Cube(npol, nchan, nrow) if found in the
    // MS.
    // Supports returning a vector of cubes.
    // If VisBuffer contains rows with different number of channels
    // each of the cubes will have a different shape
    virtual void
    floatData(casacore::Vector<casacore::Cube<casacore::Float>> & fcubes) const override;

	// Return the visibility 4-vector of polarizations for each channel.  If the
	// casacore::MS doesn't contain all polarizations, it is assumed it contains
	// one or two parallel hand polarizations.

//    virtual void visibilityCorrected(casacore::Matrix<CStokesVector> & vis) const;
//    virtual void visibilityModel(casacore::Matrix<CStokesVector> & vis) const;
//    virtual void visibilityObserved(casacore::Matrix<CStokesVector> & vis) const;

	// Return the shape of the visibility Cube
	virtual casacore::IPosition
	visibilityShape() const override;

	// Return u,v and w (in meters)
	virtual void
	uvw(casacore::Matrix<casacore::Double> & uvwmat) const override;

    // Return weight
    virtual void
    weight(casacore::Matrix<casacore::Float> & wt) const override;

    // Return weight
    // Supports returning a vector of cubes.
    // If VisBuffer contains rows with different number of channels
    // each of the cubes will have a different shape
    virtual void
    weight(casacore::Vector<casacore::Matrix<casacore::Float>> & wt) const override;

	// Determine whether WEIGHT_SPECTRUM exists.
	casacore::Bool
	weightSpectrumExists() const override;

	// Determine whether SIGMA_SPECTRUM exists.
	casacore::Bool
	sigmaSpectrumExists() const override;

    // Return weightspectrum(a weight for each channel)
    virtual void
    weightSpectrum(casacore::Cube<casacore::Float> & wtsp) const override;

    // Return weightspectrum(a weight for each channel)
    // Supports returning a vector of cubes.
    // If VisBuffer contains rows with different number of channels
    // each of the cubes will have a different shape
    virtual void
    weightSpectrum(casacore::Vector<casacore::Cube<casacore::Float>> & wtsp) const override;

    // Return sigmaspectrum(a sigma for each channel)
    virtual void
    sigmaSpectrum(casacore::Cube<casacore::Float> & sigsp) const override;

    // Return sigmaspectrum(a sigma for each channel)
    // Supports returning a vector of cubes.
    // If VisBuffer contains rows with different number of channels
    // each of the cubes will have a different shape
    virtual void
    sigmaSpectrum(casacore::Vector<casacore::Cube<casacore::Float>> & sigsp) const override;

	virtual void
	setWeightScaling(casacore::CountedPtr<WeightScaling> weightscaling) override;

	virtual casacore::CountedPtr<WeightScaling>
	getWeightScaling() const override;

	virtual casacore::Bool
	hasWeightScaling() const override;

	// Return imaging weight (a weight for each channel)
	//virtual casacore::Matrix<casacore::Float> & imagingWeight(casacore::Matrix<casacore::Float> & wt) const;
	const VisImagingWeight &
	getImagingWeightGenerator() const override;

	// Return true if FieldId/Source has changed since last iteration
	virtual casacore::Bool
	newFieldId() const;

	// Return true if arrayID has changed since last iteration
	virtual casacore::Bool
	newArrayId() const;

	// Return true if SpectralWindow has changed since last iteration
	virtual casacore::Bool
	newSpectralWindow() const;

    // Return the number of rows in the current iteration
    virtual casacore::rownr_t
    nRows() const override;

    // Return the number of distinct array/cube shapes in the current iteration
    virtual casacore::rownr_t
    nShapes() const override;

    // Return the number of rows for each distinct array/cube shapes in the current iteration
    virtual const casacore::Vector<casacore::rownr_t>& 
    nRowsPerShape () const override;

    // Return the number of channels for each distinct array/cube shapes in the current iteration
    virtual const casacore::Vector<casacore::Int>& 
    nChannelsPerShape () const override;

    // Return the number of correlations for each distinct array/cube shapes in the current iteration
    virtual const casacore::Vector<casacore::Int>& 
    nCorrelationsPerShape () const override;

	// Return the row ids as from the original root table. This is useful
	// to find correspondance between a given row in this iteration to the
	// original ms row
	virtual void
	getRowIds(casacore::Vector<casacore::rownr_t> & rowids) const override;

	// Return the numbers of rows in the current chunk
	virtual casacore::rownr_t
	nRowsInChunk() const override;

    // number of unique time stamps in chunk
    virtual casacore::Int nTimes() const override;
 
	// Return the number of sub-intervals in the current chunk

	//virtual casacore::Int nSubInterval() const;

	// Call to use the slurp i/o method for all scalar columns. This will set
	// the casacore::BucketCache cache size to the full column length and cause
	// the full column to be cached in memory, if any value of the column is
	// used. In case of out-of-memory, it will automatically fall-back on the
	// smaller cache size.  Slurping the column is to be considered as a
	// work-around for the casacore::Table i/o code, which uses
	// casacore::BucketCache and performs extremely bad for random
	// access. Slurping is useful when iterating non-sequentially an
	// casacore::MS or parts of an casacore::MS, it is not tested with multiple
	// MSs.
	virtual void
	slurp() const override;

	// Get the spw, start and nchan for all the ms's is this Visiter that match
	// the frequecy "freqstart-freqStep" and "freqEnd+freqStep" range Can help
	// in doing channel selection above..  freqFrame is the frame the caller
	// frequency values are in(freqStart and freqEnd) These will be converted to
	// the frame of the selected spw to match

//    virtual void getSpwInFreqRange(casacore::Block<casacore::Vector<casacore::Int> > & spw,
//                                    casacore::Block<casacore::Vector<casacore::Int> > & start,
//                                    casacore::Block<casacore::Vector<casacore::Int> > & nchan,
//                                    casacore::Double freqStart, casacore::Double freqEnd,
//                                    casacore::Double freqStep, casacore::MFrequency::Types freqFrame = casacore::MFrequency::LSRK) const;

	// Get the range of frequency convered by the selected data in the frame
	// requested

//    virtual void getFreqInSpwRange(casacore::Double& freqStart, casacore::Double& freqEnd, casacore::MFrequency::Types freqframe = casacore::MFrequency::LSRK) const;

	// Access the current casacore::MSColumns object in MSIter
	virtual const vi::SubtableColumns &
	subtableColumns() const override;

	// get back the selected spectral windows and spectral channels for
	// current ms

	virtual const SpectralWindowChannels &
	getSpectralWindowChannels(
		casacore::Int msId,
		casacore::Int spectralWindowId) const override;

	//assign a VisImagingWeight object to this iterator
	virtual void
	useImagingWeight(const VisImagingWeight & imWgt) override;

	//return number  of Ant
	virtual casacore::Int
	nAntennas() const override;

	//Return number of rows in all selected ms's
	virtual casacore::rownr_t
	nRowsViWillSweep() const override;

	// Return number of spws, polids, ddids
	virtual casacore::Int
	nSpectralWindows() const override;

	virtual casacore::Int
	nPolarizationIds() const override;

	virtual casacore::Int
	nDataDescriptionIds() const override;

	virtual casacore::MEpoch
	getEpoch() const override;

	casacore::MFrequency::Types
	getObservatoryFrequencyType() const override; //???

	casacore::MPosition
	getObservatoryPosition() const override;

	casacore::Vector<casacore::Float>
	getReceptor0Angle() override;

	// Write/modify the flags in the data.  This will flag all channels in the
	// original data that contributed to the output channel in the case of
	// channel averaging.  All polarizations have the same flag value.

//    virtual void writeFlag(const casacore::Matrix<casacore::Bool> & flag);

	// Write/modify the flags in the data.

	// This writes the flags as found in the casacore::MS,
	// casacore::Cube(npol,nchan,nrow), where nrow is the number of rows in the
	// current iteration(given by nRow()).

	virtual void
	writeFlag(const casacore::Cube<casacore::Bool> & flag) override;

	// Write/modify the flag row column; dimension casacore::Vector(nrow)
	virtual void
	writeFlagRow(const casacore::Vector<casacore::Bool> & rowflags) override;

	void
	writeFlagCategory(const casacore::Array<casacore::Bool>& fc) override;

	// Write/modify the visibilities.  This is possibly only for a 'reference'
	// casacore::MS which has a new DATA column.  The first axis of the matrix
	// should equal the selected number of channels in the original MS.  If the
	// casacore::MS does not contain all polarizations, only the parallel hand
	// polarizations are used.

//    void writeVisCorrected(const casacore::Matrix<CStokesVector> & visibilityStokes);
//    void writeVisModel(const casacore::Matrix<CStokesVector> & visibilityStokes);
//    void writeVisObserved(const casacore::Matrix<CStokesVector> & visibilityStokes);

	// Write/modify the visibilities

	// This writes the data as found in the casacore::MS,
	// casacore::Cube(npol,nchan,nrow).
	virtual void
	writeVisCorrected(const casacore::Cube<casacore::Complex> & vis) override;

	virtual void
	writeVisModel(const casacore::Cube<casacore::Complex> & vis) override;

	virtual void
	writeVisObserved(const casacore::Cube<casacore::Complex> & vis) override;

	// Write/modify the weights
	virtual void
	writeWeight(const casacore::Matrix<casacore::Float> & wt) override;

	// Write/modify the weightMat
	//virtual void writeWeightMat(const casacore::Matrix<casacore::Float> & wtmat);

	// Write/modify the weightSpectrum
	virtual void
	writeWeightSpectrum(const casacore::Cube<casacore::Float> & wtsp) override;

	// Initialize the weightSpectrum column
	virtual void
	initWeightSpectrum(const casacore::Cube<casacore::Float> & weightSpectrum) override;

	// Write/modify the sigmaSpectrum
	virtual void
	writeSigmaSpectrum(const casacore::Cube<casacore::Float> & sigsp) override;

	// Initialize the sigmaSpectrum column
	virtual void
	initSigmaSpectrum(const casacore::Cube<casacore::Float> & sigmaSpectrum) override;

	// Write/modify the Sigma
	virtual void
	writeSigma(const casacore::Matrix<casacore::Float> & sig) override;

	// Write/modify the ncorr x nrow SigmaMat.
	//virtual void writeSigmaMat(const casacore::Matrix<casacore::Float> & sigmat);

	virtual void
	writeModel(
		const casacore::RecordInterface& rec,
		casacore::Bool iscomponentlist=true,
		casacore::Bool incremental=false) override;

	virtual void
	writeBackChanges(VisBuffer2 *) override;

    //**********************************************************************
    // Methods to access the subtables.
    //**********************************************************************

    // Access to antenna subtable
    const casacore::MSAntennaColumns& antennaSubtablecols() const override;

    // Access to dataDescription subtable
    const casacore::MSDataDescColumns& dataDescriptionSubtablecols() const override;

    // Access to feed subtable
    const casacore::MSFeedColumns& feedSubtablecols() const override;

    // Access to field subtable
    const casacore::MSFieldColumns& fieldSubtablecols() const override;

    // Access to flagCmd subtable
    const casacore::MSFlagCmdColumns& flagCmdSubtablecols() const override;

    // Access to history subtable
    const casacore::MSHistoryColumns& historySubtablecols() const override;

    // Access to observation subtable
    const casacore::MSObservationColumns& observationSubtablecols() const override;

    // Access to pointing subtable
    const casacore::MSPointingColumns& pointingSubtablecols() const override;

    // Access to polarization subtable
    const casacore::MSPolarizationColumns& polarizationSubtablecols() const override;

    // Access to processor subtable
    const casacore::MSProcessorColumns& processorSubtablecols() const override;

    // Access to spectralWindow subtable
    const casacore::MSSpWindowColumns& spectralWindowSubtablecols() const override;

    // Access to state subtable
    const casacore::MSStateColumns& stateSubtablecols() const override;

    // Access to doppler subtable
    const casacore::MSDopplerColumns& dopplerSubtablecols() const override;

    // Access to freqOffset subtable
    const casacore::MSFreqOffsetColumns& freqOffsetSubtablecols() const override;

    // Access to source subtable
    const casacore::MSSourceColumns& sourceSubtablecols() const override;

    // Access to sysCal subtable
    const casacore::MSSysCalColumns& sysCalSubtablecols() const override;

    // Access to weather subtable
    const casacore::MSWeatherColumns& weatherSubtablecols() const override;

protected:


	virtual void
	attachColumns(const casacore::Table & t);

	// returns the table, to which columns are attached,
	// can be overridden in derived classes

	virtual const casacore::Table
	attachTable() const;

	virtual void
	applyPendingChanges();

	virtual void
	allSpectralWindowsSelected(
		casacore::Vector<casacore::Int> & selectedWindows,
		casacore::Vector<casacore::Int> & nChannels) const;

	// set the iteration state

	virtual void
	configureNewChunk();

	// set the currently selected table

	virtual void
	configureNewSubchunk();

	std::shared_ptr<vi::ChannelSelector>
	createDefaultChannelSelector(
		casacore::Double time,
		casacore::Int msId,
		casacore::Int spectralWindowId);

	virtual std::shared_ptr<vi::ChannelSelector>
	determineChannelSelection(
		casacore::Double time,
		casacore::Int spectralWindowId = -1,
		casacore::Int polarizationId = -1,
		casacore::Int msId = -1) const;

	bool
	fillFromVirtualModel(casacore::Cube <casacore::Complex> & value) const;

	casacore::Slice
	findChannelsInRange(
		casacore::Double lowerFrequency,
		casacore::Double upperFrequency,
		const vi::SpectralWindowChannels & spectralWindowChannels) const;

	ChannelInfo
	getChannelInformation() const;

	ChannelInfo
	getChannelInformationUsingFrequency() const;

	// Methods to get the data out of a table column according to whatever
	// selection criteria (e.g., slicing) is in effect.

	template <typename T>
	void
	getColumnRows(
		const casacore::ScalarColumn<T> & column,
		casacore::Vector<T> & array) const;

    template <typename T>
    void
    getColumnRowsMatrix(const casacore::ArrayColumn<T> & column,
                        casacore::Matrix<T> & array,
                        casacore::Bool correlationSlicing) const;

    template <typename T>
    void
    getColumnRowsMatrix(const casacore::ArrayColumn<T> & column,
                        casacore::Vector<casacore::Matrix<T>> & matrixVector) const;

    template <typename T>
    void
    getColumnRows(const casacore::ArrayColumn<T> & column,
                  casacore::Array<T> & array) const;

    template <typename T>
    void
    getColumnRows(const casacore::ArrayColumn<T> & column,
                  casacore::Vector<casacore::Cube<T>> & cubeVector) const;

	casacore::Vector<casacore::Double>
	getFrequencies(
		casacore::Double time,
		casacore::Int frameOfReference,
		casacore::Int spectralWindowId,
		casacore::Int msId) const override; // helper method

  	casacore::Vector<casacore::Double>
	getChanWidths(
		casacore::Double time,
		casacore::Int frameOfReference,
		casacore::Int spectralWindowId,
		casacore::Int msId) const override; // helper method

	casacore::Vector<casacore::Int>
	getChannels(
		casacore::Double time,
		casacore::Int frameOfReference,
		casacore::Int spectralWindowId,
		casacore::Int msId) const override;

	casacore::Vector<casacore::Int>
	getCorrelations() const override;

	casacore::Vector<casacore::Stokes::StokesTypes>
	getCorrelationTypesDefined() const override;

	casacore::Vector<casacore::Stokes::StokesTypes>
	getCorrelationTypesSelected() const override;

	int getMeasurementFrame(int spectralWindowId) const;

	casacore::Int
	getPolarizationId(
		casacore::Int spectralWindowId,
		casacore::Int msId) const;

	casacore::Int
	getReportingFrameOfReference() const override;

	// Returns the casacore::MS objects that this VI is iterating over.

	casacore::Block
	<casacore::MeasurementSet> getMeasurementSets() const;

	// Provides access to the casacore::MS-derived values object

	const casacore::MSDerivedValues &
	getMsd() const; // for use by Async I/O *ONLY*

	// Get privileged (non-const) access to the containing ROVI

	VisibilityIterator2 *
	getViP() const;

//    void getVisibilityAsStokes(casacore::Matrix<CStokesVector> & visibilityStokes,
//                                const casacore::ArrayColumn<casacore::Complex> & column) const;

	VisBuffer2 *
	getVisBuffer(const VisibilityIterator2 *) const;

	// Ctor auxiliary method

	virtual void
	addDataSelection(const casacore::MeasurementSet & ms);

	virtual void
	initialize(
		const casacore::Block<const casacore::MeasurementSet *> & mss,
		casacore::Bool useMSIter2);

    // Initialize using only the generic sorting criteria
    void
    initialize(const casacore::Block<const casacore::MeasurementSet *> &mss);

	// Returns true if casacore::MS Iterator is currently pointing to a selected
	// spectral window

	casacore::Bool
	isInASelectedSpectralWindow() const;

	// Creates a channel selection for the current subchunk based on the channel
	// or frequency selection made by the user.

	std::shared_ptr<vi::ChannelSelector>
	makeChannelSelectorC(
		const FrequencySelection & selection,
		casacore::Double time,
		casacore::Int msId,
		casacore::Int spectralWindowId,
		casacore::Int polarizationId) const;

	std::shared_ptr<vi::ChannelSelector>
	makeChannelSelectorF(
		const FrequencySelection & selection,
		casacore::Double time,
		casacore::Int msId,
		casacore::Int spectralWindowId,
		casacore::Int polarizationId) const;

	casacore::MFrequency::Convert
	makeFrequencyConverter(
		casacore::Double time,
		int spectralWindowId,
		casacore::Int otherFrameOfReference,
		casacore::Bool toObservedFrame,
		casacore::Unit) const;

	// Allow access to the casacore::MSColumns object; for use by
	// VisBuffer2Adapter *KLUGE*

	const casacore::MSColumns *
	msColumnsKluge() const;

	// Method to reset the VI back to the start.  Unlike the public version
	// there is a parameter to allow forcing the rewind even if the
	// casacore::MS Iter is already at the origin.

	virtual void
	originChunks(casacore::Bool forceRewind) override;

	// Advances the casacore::MS Iterator until it points at a spectral window
	// that is part of the frequency selection.

	void
	positionMsIterToASelectedSpectralWindow();

	// Sets the default frequency reporting frame of reference.  This
	// affects the default frame for obtaining the frequencies in a
	// VisBuffer.

	void
	setReportingFrameOfReference(casacore::Int) override;

	// Adjusts the tile cache for some columns so that the cache size is
	// optimized for the current input state (e.g., a new data description).

	virtual void
	setTileCache();

	void
	setMsCacheSizes(
		const casacore::MeasurementSet & ms,
		std::vector<casacore::MSMainEnums::PredefinedColumns> columnIds);

	void
	setMsColumnCacheSizes(const casacore::MeasurementSet&, const string &);

	// Throws exception if there is a pending (i.e., unapplied) change to
	// the VI's properties.  Called when the VI is advanced since the user
	// probably forgot to apply the changes.

	virtual void
	throwIfPendingChanges();

	// Returns true if the named column uses a tiled data manager in the
	// specified MS

	casacore::Bool
	usesTiledDataManager(
		const casacore::String & columnName,
		const casacore::MeasurementSet & ms) const;


//    +========================|
//    |                        |
//    | Output Related Methods |
//    |                        |
//    +========================|

	// A BackWriter is a functor that will extract a piece of information out of
	// its VisBuffer argument and write it out using a "set" method on the
	// supplied VisibilityIterator2.
	class BackWriter {

	public:

		virtual ~BackWriter() {}

		virtual void operator()(VisibilityIteratorImpl2 * vi, VisBuffer2 * vb) = 0;

	};

	// A simple BackWriterImpl2 uses a nullary accessor on a VisBuffer.
	template <typename Setter, typename Getter>
	class BackWriterImpl : public BackWriter {
	public:

		BackWriterImpl(Setter setter, Getter getter)
			: getter_p(getter), setter_p(setter) {}

		void
		operator() (VisibilityIteratorImpl2 * vi, VisBuffer2 * vb) {
			(vi ->* setter_p)((vb ->* getter_p)());
		}

	private:

		Getter getter_p;
		Setter setter_p;
	};

	// BackWriterImpl2 is slightly more complicated in that it uses a unary
	// accessor.  The argument to the unary accessor is a member of the
	// VisibilityIterator2 DataColumn enumeration which specifies which
	// visibilty or visCube type is wanted (e.g., observed, model or corrected).
	template <typename Setter, typename Getter>
	class BackWriterImpl2 : public BackWriter {
	public:

		typedef VisibilityIteratorImpl2::DataColumn DataColumn;

		BackWriterImpl2(Setter setter, Getter getter, DataColumn dc)
			: dataColumn_p(dc), getter_p(getter), setter_p(setter)
			{}

		void
		operator() (VisibilityIteratorImpl2 * vi, VisBuffer2 * vb) {
			(vi ->* setter_p)((vb ->* getter_p)(), dataColumn_p);
		}

	private:

		DataColumn dataColumn_p;
		Getter getter_p;
		Setter setter_p;
	};

	// Backwriter(2) creation methods.  These methods make it fairly
	// straightforward to create a BackWriter object.

	template <typename Ret>
	static
	BackWriter *
	makeBackWriter(
		void (VisibilityIteratorImpl2::* setter)(Ret),
		Ret (VisBuffer2::* getter)() const) {
		return new BackWriterImpl<void(VisibilityIteratorImpl2:: *)(Ret),
		                          Ret(VisBuffer2:: *)() const >(
			                          setter, getter);
	}

	template <typename Ret>
	static
	BackWriter *
	makeBackWriter2(
		void (VisibilityIteratorImpl2::* setter)(Ret, VisibilityIteratorImpl2::DataColumn),
		Ret (VisBuffer2::* getter)() const,
		VisibilityIterator2::DataColumn dc) {

		// Define the Getter and Setter types

		typedef void (VisibilityIteratorImpl2::* Setter)(Ret, VisibilityIteratorImpl2::DataColumn);
		typedef Ret (VisBuffer2::* Getter)() const;

		return new BackWriterImpl2 < Setter, Getter>(setter, getter, dc);
	}

	void
	initializeBackWriters();

	template <typename T>
	void
	putColumnRows(
		casacore::ArrayColumn<T> & column,
		const casacore::Array<T> & array);

	template <typename T>
	void
	putColumnRows(
		casacore::ArrayColumn<T> & column,
		const casacore::Matrix<T> & array);

	template <typename T>
	void
	putColumnRows(
		casacore::ScalarColumn<T> & column,
		const casacore::Vector <T> & array);

//    void convertVisFromStokes(const casacore::Matrix<CStokesVector> & visibilityStokes,
//                               casacore::Cube<casacore::Complex> & visCube);

	class Cache {

	public:

		Cache();

		void flush();

		casacore::MDirection azel0_p;
		casacore::Double azel0Time_p;
		casacore::Vector<casacore::MDirection> azel_p;
		casacore::Double azelTime_p;
		// Row numbers of underlying casacore::MS; used to map form chunk rows
		// to casacore::MS rows.  See rowIds method.
		casacore::Vector<casacore::rownr_t> chunkRowIds_p;
		casacore::Vector<casacore::Float> feedpa_p;
		casacore::Double feedpaTime_p;
		casacore::Double hourang_p;
		casacore::Double hourangTime_p;
		casacore::Matrix<casacore::Float> imagingWeight_p;
		// Does the current casacore::MS have a valid FLAG_CATEGORY?
		casacore::Bool msHasFlagCategory_p;
		// Does the current casacore::MS have a valid WEIGHT_SPECTRUM?
		casacore::Bool msHasWeightSpectrum_p;
		// Does the current casacore::MS have a valid SIGMA_SPECTRUM?
		casacore::Bool msHasSigmaSpectrum_p;
		casacore::Float parang0_p;
		casacore::Double parang0Time_p;
		casacore::Vector<casacore::Float> parang_p;
		casacore::Double parangTime_p;
	};

	class PendingChanges {

	public:

		PendingChanges();
		~PendingChanges();

		PendingChanges *
		clone() const;

		casacore::Bool
		empty() const;

		// yields ownership
		std::pair<casacore::Bool, FrequencySelections *>
		popFrequencySelections();

		std::pair<casacore::Bool, casacore::Double>
		popInterval();

		std::pair<casacore::Bool, casacore::Int>
		popNRowBlocking();

		// takes ownership
		void
		setFrequencySelections(FrequencySelections *);

		void
		setInterval(casacore::Double);

		void
		setNRowBlocking(casacore::Int);

	private:

		enum {Empty = -1};

		FrequencySelections * frequencySelections_p;
		casacore::Bool frequencySelectionsPending_p;
		casacore::Double interval_p;
		casacore::Int nRowBlocking_p;
	};

	typedef casacore::Block <casacore::MeasurementSet> MeasurementSets;

    class RowBounds
    {
    public:

        RowBounds() :
            chunkNRows_p(-1), subchunkBegin_p(-1), subchunkEnd_p(-1),
            subchunkNRows_p(-1), subchunkRows_p(0, 0), timeMax_p(-1), timeMin_p(-1)
        {}

        // last row in current chunk
        ssize_t chunkNRows_p;
        // first row in current subchunk
        ssize_t subchunkBegin_p;
        // last row in current subchunk
        ssize_t subchunkEnd_p;
        // # rows in subchunk
        ssize_t subchunkNRows_p;
        // subchunk's table row numbers
        casacore::RefRows subchunkRows_p;
        // List of Row numbers for each subset of the subchunk with equal channel selector
        std::vector<casacore::RefRows> subchunkEqChanSelRows_p;
        // times for each row in the chunk
        casacore::Vector<casacore::Double> times_p;
        // max timestamp in the chunk
        casacore::Double timeMax_p;
        // min timechunk in the chunk
        casacore::Double timeMin_p;

    };


    casacore::Bool autoTileCacheSizing_p;
    std::map <VisBufferComponent2, BackWriter *> backWriters_p;
    // general collection of cached values
    mutable Cache cache_p;
    // [use] current channel selectors for this chunk 
    std::vector<std::shared_ptr<ChannelSelector>>  channelSelectors_p;
    // Number of rows in the VisBuffer for which each of the channel selector applies
    std::vector<size_t>  channelSelectorsNrows_p;
    // [own] cache of recently used channel selectors
    ChannelSelectorCache * channelSelectorCache_p;
    // The main columns for the current MS
    ViColumns2 columns_p;
    // true if a float data column was found
    casacore::Bool floatDataFound_p;
    // [own] Current frequency selection
    FrequencySelections * frequencySelections_p;
    // object to calculate imaging weight
    VisImagingWeight imwgt_p;
    // cached value of observatory type
    mutable casacore::Int measurementFrame_p;
    MeasurementSets measurementSets_p; // [use]
    VisModelDataI * modelDataGenerator_p; // [own]
    // true if more data in this chunk
    casacore::Bool more_p;
    // array index of current MS
    casacore::Int msIndex_p;
    // true if casacore::MS Iter is a start of first MS
    casacore::Bool msIterAtOrigin_p;
    // casacore::MS Iter that underlies the VI (sweeps in chunks)
    casacore::CountedPtr<casacore::MSIter> msIter_p;
    // Helper class holding casacore::MS derived values.
    mutable casacore::MSDerivedValues msd_p;
    casacore::Int nCorrelations_p;
    // suggested # of rows in a subchunk
    casacore::Int nRowBlocking_p;
    // holds pending changes to VI properties
    std::unique_ptr<PendingChanges> pendingChanges_p;
    mutable std::unique_ptr<PointingDirectionCache>  pointingDirectionCache_p;
    mutable std::unique_ptr<PointingSource>  pointingSource_p;
    // default frequency reporting (not selecting) frame of reference
    casacore::Int reportingFrame_p;
    // Subchunk row management object (see above)
    RowBounds rowBounds_p;
    // [own] Info about spectral windows
    mutable SpectralWindowChannelsCache * spectralWindowChannelsCache_p;
    // (chunkN #, subchunk #) pair
    Subchunk subchunk_p;
    // Number of rows for each distinct array/cube shapes in the current iteration
    casacore::Vector<casacore::rownr_t> nRowsPerShape_p;
    // Number of channels for each distinct array/cube shapes in the current iteration
    casacore::Vector<casacore::Int> nChannPerShape_p;
    // Number of correlations for each distinct array/cube shapes in the current iteration
    casacore::Vector<casacore::Int> nCorrsPerShape_p;
    // [own] Allows const access to casacore::MS's subtable columns
    SubtableColumns * subtableColumns_p;
    casacore::MeasRef<casacore::MEpoch> timeFrameOfReference_p;
    std::shared_ptr<std::mutex> tileCacheModMtx_p;
    std::shared_ptr<std::vector<bool> > tileCacheIsSet_p;
    casacore::Double timeInterval_p;
    VisBufferType vbType;
    // [own] VisBuffer attached to this VI
    VisBuffer2 * vb_p;
    casacore::CountedPtr<WeightScaling> weightScaling_p;
    casacore::Bool writable_p;
    // Determine several metadata uniqueness. For each metadata
    // the valus could be unique in each chunk or subchunk,
    // or in the worst case for each row.
    MetadataScope ddIdScope_p;
    MetadataScope timeScope_p;
    MetadataScope freqSelScope_p;
    MetadataScope antenna1Scope_p;
    MetadataScope antenna2Scope_p;

    // Variables for the handling of the subchunk  loop
    std::shared_ptr<casacore::MeasurementSet> msSubchunk_p;
    std::shared_ptr<casacore::MSIter> msIterSubchunk_p;
    // sort columns specified when creating VI
    SortColumns sortColumns_p;
    SortColumns subchunkSortColumns_p;
};

} // end namespace vi

} //# NAMESPACE CASA - END

#endif
