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

#if ! defined (MSVIS_TransformingVi2_H_121115_0950)
#define MSVIS_TransformingVi2_H_121115_0950

#include <casacore/casa/aips.h>
#include <casacore/casa/Arrays/ArrayFwd.h>
#include <msvis/MSVis/ViImplementation2.h>

#include <map>
#include <vector>

#include <casacore/measures/Measures/Stokes.h>

namespace casa { //# NAMESPACE CASA - BEGIN


namespace vi {

//# forward decl

class VisBuffer2;

class ChannelSelector;
class ChannelSelectorCache;
typedef casacore::Vector<casacore::Vector <casacore::Slice> > ChannelSlicer;
class SpectralWindowChannelsCache;
class SpectralWindowChannels;
class SubtableColumns;


// <summary>
// VisibilityIterator2 iterates through one or more readonly MeasurementSets
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
// The VisibilityIterator2 is a readonly iterator returning visibilities
// </etymology>
//
// <synopsis>
// VisibilityIterator2 provides iteration with various sort orders
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

class TransformingVi2 : public ViImplementation2 {

public:

    // Destructor

    virtual ~TransformingVi2 ();

    // Report the the ViImplementation type
    //  (should be specialized in child classes)
    virtual casacore::String ViiType() const override { return casacore::String("UnknownTrans( ")+getVii()->ViiType()+" )"; };

    //   +==================================+
    //   |                                  |
    //   | Iteration Control and Monitoring |
    //   |                                  |
    //   +==================================+


    // Methods to control and monitor subchunk iteration

    virtual void origin () override;
    virtual casacore::Bool more () const override;
    virtual void next () override;
    virtual Subchunk getSubchunkId () const override;

    // Methods to control chunk iterator

    virtual void originChunks (casacore::Bool forceRewind = false) override;
    virtual casacore::Bool moreChunks () const override;
    virtual void nextChunk () override;

    // Pass results accumulated throughout iteration
    void result(casacore::Record& res) const;

    // Report Name of slowest column that changes at end of current iteration
    virtual casacore::String keyChange() const override { return getVii()->keyChange(); };

    virtual casacore::Bool isWritable () const override;

    // Return the time interval (in seconds) used for iteration.
    // This is not the same as the INTERVAL column.  Setting the
    // the interval requires calling origin chunks before performing
    // further iterator.

    virtual double getInterval() const override;
    virtual void setInterval (double timeInterval) override;

    // Select the channels to be returned.  Requires calling originChunks before
    // performing additional iteration.

    virtual void setFrequencySelections (const FrequencySelections & selection) override;

    // Set the 'blocking' size for returning data.
    // With the default (0) only a single integration is returned at a time, this
    // is what is currently required for the calibration software. With blocking
    // set, up to nRows can be returned in one go. The chunk
    // size determines the actual maximum.

    virtual void setRowBlocking (casacore::rownr_t nRows) override;

    virtual casacore::Bool existsColumn (VisBufferComponent2 id) const override;

    virtual const SortColumns & getSortColumns() const override;

    virtual casacore::Bool isNewArrayId () const override;
    virtual casacore::Bool isNewFieldId () const override;
    virtual casacore::Bool isNewMs () const override;
    virtual casacore::Bool isNewSpectralWindow () const override;

    // Return the number of rows in the current iteration
    virtual casacore::rownr_t nRows () const override;

    // Return the number of distinct array/cube shapes in the current iteration
    virtual casacore::rownr_t nShapes () const override;

    // Return the number of rows for each distinct array/cube shapes in the current iteration
    virtual const casacore::Vector<casacore::rownr_t>& nRowsPerShape () const override;

    // Return the number of channels for each distinct array/cube shapes in the current iteration
    virtual const casacore::Vector<casacore::Int>& nChannelsPerShape () const override;

    // Return the number of correlations for each distinct array/cube shapes in the current iteration
    virtual const casacore::Vector<casacore::Int>& nCorrelationsPerShape () const override;

    // Return the row ids as from the original root table. This is useful
    // to find correspondance between a given row in this iteration to the
    // original ms row

    virtual void getRowIds (casacore::Vector<casacore::rownr_t> & rowids) const override;

	virtual VisBuffer2 * getVisBuffer () const override {return vb_p;}


    //   +=========================+
    //   |                         |
    //   | Subchunk casacore::Data Accessors |
    //   |                         |
    //   +=========================+

    // Return antenna1

    virtual void antenna1 (casacore::Vector<casacore::Int> & ant1) const override;

    // Return antenna2

    virtual void antenna2 (casacore::Vector<casacore::Int> & ant2) const override;

    // Return the correlation type (returns casacore::Stokes enums)

    virtual void corrType (casacore::Vector<casacore::Int> & corrTypes) const override;

    // Return current DataDescription Id

    virtual casacore::Int dataDescriptionId () const override;

    // Return actual time interval

    virtual void  exposure (casacore::Vector<double> & expo) const override;

    // Return feed1

    virtual void feed1 (casacore::Vector<casacore::Int> & fd1) const override;

    // Return feed2

    virtual void feed2 (casacore::Vector<casacore::Int> & fd2) const override;

    // Return the current FieldId

    virtual void fieldIds (casacore::Vector<casacore::Int>&) const override;


    // Return the current ArrayId

    virtual void arrayIds (casacore::Vector<casacore::Int>&) const override;

    // Return the current Field Name

    virtual casacore::String fieldName () const override;

    // Return flag for each polarization, channel and row

    virtual void flag (casacore::Cube<casacore::Bool> & flags) const override;

    // Return flag for each polarization, channel and row

    virtual void flag (casacore::Vector<casacore::Cube<casacore::Bool>> & flags) const override;

    // Return flag for each channel & row

    virtual void flag (casacore::Matrix<casacore::Bool> & flags) const override;

    // Determine whether FLAG_CATEGORY is valid.

    virtual casacore::Bool flagCategoryExists () const override;

    // Return flags for each polarization, channel, category, and row.

    virtual void flagCategory (casacore::Array<casacore::Bool> & flagCategories) const override;

    // Return row flag

    virtual void flagRow (casacore::Vector<casacore::Bool> & rowflags) const override;

    // Return the OBSERVATION_IDs

    virtual void observationId (casacore::Vector<casacore::Int> & obsids) const override;

    // Return current Polarization Id

    virtual casacore::Int polarizationId () const override;

    // Return the PROCESSOR_IDs

    virtual void processorId (casacore::Vector<casacore::Int> & procids) const override;

    // Return scan number

    virtual void scan (casacore::Vector<casacore::Int> & scans) const override;

    // Return the current Source Name

    virtual casacore::String sourceName () const override;

    // Return the STATE_IDs

    virtual void stateId (casacore::Vector<casacore::Int> & stateids) const override;


    // Return feed configuration matrix for specified antenna

    virtual void jonesC (casacore::Vector<casacore::SquareMatrix<casacore::Complex, 2> > & cjones) const override;

    // Return frame for polarization (returns PolFrame enum)

    virtual casacore::Int polFrame () const override;

    // Return sigma

    virtual void sigma (casacore::Matrix<casacore::Float> & sigmat) const override;
    virtual void sigma (casacore::Vector<casacore::Matrix<casacore::Float>> & sigmat) const override;

    // Return all the spectral windows ids for each row of the current buffer
    virtual void spectralWindows (casacore::Vector<casacore::Int> & spws) const override;

    // Return all the polarizations Ids for each row of the current buffer
    virtual void polarizationIds (casacore::Vector<casacore::Int> & polIds) const override;

    // Return MJD midpoint of interval.

    virtual void time (casacore::Vector<double> & t) const override;

    // Return MJD centroid of interval.

    virtual void timeCentroid (casacore::Vector<double> & t) const override;

    // Return nominal time interval

    virtual void timeInterval (casacore::Vector<double> & ti) const override;

    // Return u,v and w (in meters)

    virtual void uvw (casacore::Matrix<double> & uvwmat) const override;

    // Return the visibilities as found in the casacore::MS, casacore::Cube (npol,nchan,nrow).

    virtual void visibilityCorrected (casacore::Cube<casacore::Complex> & vis) const override;
    virtual void visibilityCorrected (casacore::Vector<casacore::Cube<casacore::Complex>> & vis) const override;
    virtual void visibilityModel (casacore::Cube<casacore::Complex> & vis) const override;
    virtual void visibilityModel (casacore::Vector<casacore::Cube<casacore::Complex>> & vis) const override;
    virtual void visibilityObserved (casacore::Cube<casacore::Complex> & vis) const override;
    virtual void visibilityObserved (casacore::Vector<casacore::Cube<casacore::Complex>> & vis) const override;

    // Return FLOAT_DATA as a casacore::Cube (npol, nchan, nrow) if found in the MS.

    virtual void floatData (casacore::Cube<casacore::Float> & fcube) const override;
    virtual void floatData (casacore::Vector<casacore::Cube<casacore::Float>> & fcube) const override;

    // Return the visibility 4-vector of polarizations for each channel.
    // If the casacore::MS doesn't contain all polarizations, it is assumed it
    // contains one or two parallel hand polarizations.

//    virtual void visibilityCorrected (casacore::Matrix<CStokesVector> & vis) const;
//    virtual void visibilityModel (casacore::Matrix<CStokesVector> & vis) const;
//    virtual void visibilityObserved (casacore::Matrix<CStokesVector> & vis) const;

    // Return the shape of the visibility Cube

    virtual casacore::IPosition visibilityShape () const override;

    // Return weight

    virtual void weight (casacore::Matrix<casacore::Float> & wtmat) const override;
    virtual void weight (casacore::Vector<casacore::Matrix<casacore::Float>> & wtmat) const override;

    // Determine whether WEIGHT_SPECTRUM exists.

    virtual casacore::Bool weightSpectrumExists () const override;
    virtual casacore::Bool sigmaSpectrumExists () const override;

    // Return weightspectrum (a weight for each channel)

    virtual void weightSpectrum (casacore::Cube<casacore::Float> & wtsp) const override;
    virtual void weightSpectrum (casacore::Vector<casacore::Cube<casacore::Float>> & wtsp) const override;
    virtual void sigmaSpectrum (casacore::Cube<casacore::Float> & sigsp) const override;
    virtual void sigmaSpectrum (casacore::Vector<casacore::Cube<casacore::Float>> & sigsp) const override;

    // Return the number of sub-intervals in the current chunk

    //   +------------------------+
    //   |                        |
    //   | Angular casacore::Data Providers |
    //   |                        |
    //   +------------------------+

    // true if all elements of the cube returned by getBeamOffsets are zero

    virtual casacore::Bool allBeamOffsetsZero () const override;

    virtual std::pair<bool, casacore::MDirection> getPointingAngle (int antenna, double time) const override;

    // Return the antenna AZ/EL casacore::Vector (nant)

    virtual casacore::MDirection azel0 (double time) const override;
//    static void azel0Calculate (double time, MSDerivedValues & msd,
//                                MDirection & azel0, const MEpoch & mEpoch0);

    virtual const casacore::Vector<casacore::MDirection> & azel (double time) const override;
//    static void azelCalculate (double time, MSDerivedValues & msd, Vector<MDirection> & azel,
//                               Int nAnt, const MEpoch & mEpoch0);

    // Return feed parallactic angles casacore::Vector (nant) (1 feed/ant)

    virtual const casacore::Vector<casacore::Float> & feed_pa (double time) const override;
//    static Vector<Float> feed_paCalculate (double time, MSDerivedValues & msd,
//                                           Int nAntennas, const MEpoch & mEpoch0,
//                                           const Vector<Float> & receptor0Angle);

    // Return a cube containing pairs of coordinate offsets for each
    // receptor of each feed (values are in radians, coordinate system is fixed
    // with antenna and is the same one as used to define the BEAM_OFFSET
    // parameter in the feed table). The cube axes are receptor, antenna, feed.

    virtual const casacore::Cube<casacore::RigidVector<double, 2> > & getBeamOffsets () const override;

    // Return the hour angle for the specified time

    virtual double hourang (double time) const override;
    static double hourangCalculate (double time, casacore::MSDerivedValues & msd, const casacore::MEpoch & mEpoch0);

    // Return nominal parallactic angle at specified time
    // (does not include feed position angle offset--see feed_pa)
    // A global value for all antennas (e.g., small array)

    virtual const casacore::Float & parang0 (double time) const override;
    static casacore::Float parang0Calculate (double time, casacore::MSDerivedValues & msd, const casacore::MEpoch & epoch0);

    // Per antenna:

    virtual const casacore::Vector<casacore::Float> & parang (double time) const override;
    static casacore::Vector<casacore::Float> parangCalculate (double time, casacore::MSDerivedValues & msd,
                                          int nAntennas, const casacore::MEpoch mEpoch0);

    // Return the current phase center as an MDirection

    virtual const casacore::MDirection & phaseCenter () const override;

    // Return receptor angles for all antennae and feeds
    // First axis of the cube is a receptor number,
    // 2nd is antennaId, 3rd is feedId
    // Note: the method is intended to provide an access to MSIter::receptorAngles
    // for VisBuffer in the multi-feed case. It may be worth to change the
    // interface of feed_pa to return the information for all feeds.

    virtual const casacore::Cube<double> & receptorAngles () const override;

    //   +=========================+
    //   |                         |
    //   | Chunk and casacore::MS Level casacore::Data |
    //   |                         |
    //   +=========================+

    // return a string mount identifier for each antenna

    virtual const casacore::Vector<casacore::String> & antennaMounts () const override;

    virtual casacore::MEpoch getEpoch () const override;

    // Return imaging weight (a weight for each channel)
    // virtual casacore::Matrix<casacore::Float> & imagingWeight (casacore::Matrix<casacore::Float> & wt) const;

    virtual const VisImagingWeight & getImagingWeightGenerator () const override;

    virtual casacore::MFrequency::Types getObservatoryFrequencyType () const override; //???
    virtual casacore::MPosition getObservatoryPosition () const override;
    virtual casacore::Vector<casacore::Float> getReceptor0Angle () override;

    virtual casacore::Int getReportingFrameOfReference () const override;
    virtual void setReportingFrameOfReference (casacore::Int frame) override;

    virtual casacore::Vector<casacore::Int> getChannels (double time, casacore::Int frameOfReference,
                                     casacore::Int spectralWindowId, casacore::Int msId) const override;
    virtual casacore::Vector<casacore::Int> getCorrelations () const override;
    virtual casacore::Vector<casacore::Stokes::StokesTypes> getCorrelationTypesDefined () const override;
    virtual casacore::Vector<casacore::Stokes::StokesTypes> getCorrelationTypesSelected () const override;

    virtual casacore::Vector<double> getFrequencies (double time, casacore::Int frameOfReference,
                                           casacore::Int spectralWindowId, casacore::Int msId) const override;
    virtual casacore::Vector<double> getChanWidths (double time, casacore::Int frameOfReference,
                                           casacore::Int spectralWindowId, casacore::Int msId) const override;
    virtual void dataDescriptionIds(casacore::Vector<casacore::Int> &) const override;

    //reference to actual ms in interator

    virtual casacore::Int msId () const override; // zero-based index of current casacore::MS in set of MSs
    virtual const casacore::MeasurementSet & ms () const override;
    virtual casacore::Int getNMs () const override;

    // Name of nominal MS (will typically be beneath several layers)
    virtual casacore::String msName () const override;

    // Call to use the slurp i/o method for all scalar columns. This
    // will set the casacore::BucketCache cache size to the full column length
    // and cause the full column to be cached in memory, if
    // any value of the column is used. In case of out-of-memory,
    // it will automatically fall-back on the smaller cache size.
    // Slurping the column is to be considered as a work-around for the
    // casacore::Table i/o code, which uses casacore::BucketCache and performs extremely bad
    // for random access. Slurping is useful when iterating non-sequentially
    // an casacore::MS or parts of an casacore::MS, it is not tested with multiple MSs.

    virtual void slurp () const override;

    // Access the current casacore::MSColumns object in MSIter

    virtual const vi::SubtableColumns & subtableColumns () const override;

    // get back the selected spectral windows and spectral channels for
    // current ms

    virtual const SpectralWindowChannels & getSpectralWindowChannels (casacore::Int msId, casacore::Int spectralWindowId) const override;

    //assign a VisImagingWeight object to this iterator

    virtual void useImagingWeight (const VisImagingWeight & imWgt) override;

    // Return number of antennasm spws, polids, ddids

    virtual casacore::Int nAntennas () const override;
    virtual casacore::Int nDataDescriptionIds () const override;
    virtual casacore::Int nPolarizationIds () const override;
    virtual casacore::rownr_t nRowsInChunk () const override; // number rows in current chunk
    virtual casacore::rownr_t nRowsViWillSweep () const override; // number of rows in all selected ms's
    virtual casacore::Int nSpectralWindows () const override;
    virtual casacore::Int nTimes() const override;

    //   +-------------------+
    //   |                   |
    //   | Writeback Methods |
    //   |                   |
    //   +-------------------+

    // This method writes back any changed (dirty) components of the provided
    // VisBuffer and is the preferred method for writing data out.

    virtual void writeBackChanges (VisBuffer2 * vb) override;

    // Write/modify the flags in the data.
    // This will flag all channels in the original data that contributed to
    // the output channel in the case of channel averaging.
    // All polarizations have the same flag value.
    virtual void writeFlag (const casacore::Matrix<casacore::Bool> & flag);

    // Write/modify the flags in the data.
    // This writes the flags as found in the casacore::MS, casacore::Cube (npol,nchan,nrow),
    // where nrow is the number of rows in the current iteration (given by
    // nRow ()).
    virtual void writeFlag (const casacore::Cube<casacore::Bool> & flag) override;

    // Write/modify the flag row column; dimension casacore::Vector (nrow)
    virtual void writeFlagRow (const casacore::Vector<casacore::Bool> & rowflags) override;

    virtual void writeFlagCategory(const casacore::Array<casacore::Bool>& fc) override;

    // Write/modify the visibilities.
    // This is possibly only for a 'reference' casacore::MS which has a new DATA column.
    // The first axis of the matrix should equal the selected number of channels
    // in the original MS.
    // If the casacore::MS does not contain all polarizations, only the parallel
    // hand polarizations are used.
//    virtual void writeVisCorrected (const casacore::Matrix<CStokesVector> & visibilityStokes);
//    virtual void writeVisModel (const casacore::Matrix<CStokesVector> & visibilityStokes);
//    virtual void writeVisObserved (const casacore::Matrix<CStokesVector> & visibilityStokes);

    // Write/modify the visibilities
    // This writes the data as found in the casacore::MS, casacore::Cube (npol,nchan,nrow).
    virtual void writeVisCorrected (const casacore::Cube<casacore::Complex> & vis) override;
    virtual void writeVisModel (const casacore::Cube<casacore::Complex> & vis) override;
    virtual void writeVisObserved (const casacore::Cube<casacore::Complex> & vis) override;

    // Write/modify the weights
    virtual void writeWeight (const casacore::Matrix<casacore::Float> & wt) override;

    // Write/modify the weightMat
    //virtual void writeWeightMat (const casacore::Matrix<casacore::Float> & wtmat);

    // Write/modify the weightSpectrum
    virtual void writeWeightSpectrum (const casacore::Cube<casacore::Float> & wtsp) override;
    virtual void writeSigmaSpectrum (const casacore::Cube<casacore::Float> & sigsp) override;

    // Write/modify the Sigma
    virtual void writeSigma (const casacore::Matrix<casacore::Float> & sig) override;

    // Write/modify the ncorr x nrow SigmaMat.
    //virtual void writeSigmaMat (const casacore::Matrix<casacore::Float> & sigmat);

    // Write the information needed to generate on-the-fly model visibilities.

    virtual void writeModel(const casacore::RecordInterface& rec, casacore::Bool iscomponentlist=true,
                            casacore::Bool incremental=false) override;

    virtual void setWeightScaling (casacore::CountedPtr <WeightScaling> weightscaling) override;
    virtual casacore::Bool hasWeightScaling () const override;
    virtual casacore::CountedPtr<WeightScaling> getWeightScaling () const override;

    // Library of static transformations available for all TVIs
    static void calculateFlagRowFromFlagCube (const casacore::Cube<casacore::Bool> &flagCube, casacore::Vector<casacore::Bool> &flagRow);

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

    TransformingVi2 (ViImplementation2 * inputVi);

    void configureNewSubchunk ();
    void configureNewSubchunk (casacore::Int msId, const casacore::String & msName, casacore::Bool isNewMs,
                               casacore::Bool isNewArrayId, casacore::Bool isNewFieldId,
                               casacore::Bool isNewSpectralWindow, const Subchunk & subchunk,
                               const casacore::Vector<casacore::rownr_t>& nRowsPerShape,
                               const casacore::Vector<casacore::Int>& nChannelsPerShape, 
                               const casacore::Vector<casacore::Int>& nCorrelationsPerShape,
                               const casacore::Vector<casacore::Int> & correlations,
                               const casacore::Vector<casacore::Stokes::StokesTypes> & correlationsDefined,
                               const casacore::Vector<casacore::Stokes::StokesTypes> & correlationsSelected,
                               casacore::CountedPtr<WeightScaling> weightScaling);

    VisibilityIterator2 * getVi () const;
    ViImplementation2 * getVii () const;
    void setVisBuffer (VisBuffer2 * vb);

    // jagonzal (to be reviewed by jjacobs): I need to set inputVii_p to NULL from
    // MSTransformIterator destructor because MSTransformIteratorFactory is borrowing
    // the ViImplementation2 from the input iterator, which is deleted in the first
    // steps of the MSTransformIterator destructor.
    ViImplementation2 * inputVii_p;

private:

    VisBuffer2 * vb_p; // [own]
    casacore::CountedPtr<WeightScaling> weightScaling_p;
};

} // end namespace vi

} //# NAMESPACE CASA - END

#endif // ! defined (MSVIS_TransformingVi2_H_121115_0950)


