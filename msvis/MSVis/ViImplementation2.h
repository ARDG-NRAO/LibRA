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

#if ! defined (MSVIS_ViImplementation2_H_121115_0950)
#define MSVIS_ViImplementation2_H_121115_0950

#include <casa/aips.h>
#include <casa/BasicSL.h>
#include <msvis/MSVis/VisBufferComponents2.h>
#include <measures/Measures/MFrequency.h>
#include <measures/Measures/Stokes.h>
#include <measures/Measures/Stokes.h>
#include <casacore/casa/Arrays/ArrayFwd.h>
#include <casacore/scimath/Mathematics/RigidVector.h>
#include <casacore/tables/Tables/RowNumbers.h>

#include <map>
#include <vector>

//Forward declarations
namespace casacore{

class MDirection;
class MeasurementSet;
class MEpoch;
class MPosition;
class MSDerivedValues;
class RecordInterface;

class Slice;
class String;
template <typename T, Int n> class SquareMatrix;

class MSAntennaColumns;
class MSDataDescColumns;
class MSFeedColumns;
class MSFieldColumns;
class MSFlagCmdColumns;
class MSHistoryColumns;
class MSObservationColumns;
class MSPointingColumns;
class MSPolarizationColumns;
class MSProcessorColumns;
class MSSpWindowColumns;
class MSStateColumns;
class MSDopplerColumns;
class MSFreqOffsetColumns;
class MSSourceColumns;
class MSSysCalColumns;
class MSWeatherColumns;
}

namespace casa { //# NAMESPACE CASA - BEGIN

//class CStokesVector;
class VisImagingWeight;

namespace vi {


//# forward decl

class ChannelSelector;
class ChannelSelectorCache;
class FrequencySelections;
class SortColumns;
class SpectralWindowChannels;
class SpectralWindowChannelsCache;
class Subchunk;
class SubtableColumns;
class VisBuffer2;
class VisibilityIterator2;
class WeightScaling;
enum VisBufferType : int;
enum VisBufferOptions : int;

typedef enum {UnknownScope = 0, ChunkScope = 1, SubchunkScope = 2, RowScope = 3} MetadataScope;
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

class ViImplementation2 {

    friend class VisibilityIterator2;

public:

    // make noncopyable...
    ViImplementation2( const ViImplementation2& ) = delete;
    ViImplementation2& operator=( const ViImplementation2& ) = delete;
    ViImplementation2 () {}

    // Destructor

    virtual ~ViImplementation2 () {}

    // Report the the ViImplementation type
    virtual casacore::String ViiType() const = 0;

    //   +==================================+
    //   |                                  |
    //   | Iteration Control and Monitoring |
    //   |                                  |
    //   +==================================+

    // Methods to control and monitor subchunk iteration

    virtual void origin () = 0;
    virtual casacore::Bool more () const = 0;
    virtual void next () = 0;
    virtual Subchunk getSubchunkId () const = 0;

    // Methods to control chunk iterator

    virtual void originChunks (casacore::Bool forceRewind = false) = 0;
    virtual casacore::Bool moreChunks () const = 0;
    virtual void nextChunk () = 0;

    // Get results accumulated throughout iteration
    virtual void result(casacore::Record& res) const = 0;

    // Report Name of slowest column that changes at end of current iteration
    virtual casacore::String keyChange() const = 0;

    virtual casacore::Bool isWritable () const = 0;

    // Return the time interval (in seconds) used for iteration.
    // This is not the same as the INTERVAL column.  Setting the
    // the interval requires calling origin chunks before performing
    // further iterator.

    virtual casacore::Double getInterval() const = 0;
    virtual void setInterval (casacore::Double timeInterval) = 0;

    // Select the channels to be returned.  Requires calling originChunks before
    // performing additional iteration.

    virtual void setFrequencySelections (const FrequencySelections & selection) = 0;

    // Set the 'blocking' size for returning data.
    // With the default (0) only a single integration is returned at a time, this
    // is what is currently required for the calibration software. With blocking
    // set, up to nRows can be returned in one go. The chunk
    // size determines the actual maximum.

    virtual casacore::rownr_t getRowBlocking() const = 0;
    virtual void setRowBlocking (casacore::rownr_t nRows = 0) = 0;

    virtual casacore::Bool existsColumn (VisBufferComponent2 id) const = 0;

    virtual const SortColumns & getSortColumns() const = 0;

    virtual casacore::Bool isNewArrayId () const = 0;
    virtual casacore::Bool isNewFieldId () const = 0;
    virtual casacore::Bool isNewMs () const = 0;
    virtual casacore::Bool isNewSpectralWindow () const = 0;

    // Return the number of rows in the current iteration
    virtual casacore::rownr_t nRows () const = 0;

    // Return the number of distinct array/cube shapes in the current iteration
    virtual casacore::rownr_t nShapes () const = 0;

    // Return the number of rows for each distinct array/cube shapes in the current iteration
    virtual const casacore::Vector<casacore::rownr_t>& nRowsPerShape () const = 0;

    // Return the number of channels for each distinct array/cube shapes in the current iteration
    virtual const casacore::Vector<casacore::Int>& nChannelsPerShape () const = 0;

    // Return the number of correlations for each distinct array/cube shapes in the current iteration
    virtual const casacore::Vector<casacore::Int>& nCorrelationsPerShape () const = 0;

    // Return the row ids as from the original root table. This is useful
    // to find correspondance between a given row in this iteration to the
    // original ms row

    virtual void getRowIds (casacore::Vector<casacore::rownr_t> & rowids) const = 0;

    virtual VisBuffer2 * getVisBuffer () const = 0;


    //   +=========================+
    //   |                         |
    //   | Subchunk Data Accessors |
    //   |                         |
    //   +=========================+

    // Return antenna1

    virtual void antenna1 (casacore::Vector<casacore::Int> & ant1) const = 0;

    // Return antenna2

    virtual void antenna2 (casacore::Vector<casacore::Int> & ant2) const = 0;

    // Return the correlation type (returns casacore::Stokes enums)

    virtual void corrType (casacore::Vector<casacore::Int> & corrTypes) const = 0;

    // Return current DataDescription Id

    virtual casacore::Int dataDescriptionId () const = 0;

    virtual void dataDescriptionIds (casacore::Vector<casacore::Int> & ddis) const = 0;

    // Return actual time interval

    virtual void  exposure (casacore::Vector<casacore::Double> & expo) const = 0;

    // Return feed1

    virtual void feed1 (casacore::Vector<casacore::Int> & fd1) const = 0;

    // Return feed2

    virtual void feed2 (casacore::Vector<casacore::Int> & fd2) const = 0;

    // Return the current FieldId

    virtual void fieldIds (casacore::Vector<casacore::Int>&) const = 0;

    // Return the current ArrayId

    virtual void arrayIds (casacore::Vector<casacore::Int>&) const = 0;

    // Return the current Field Name

    virtual casacore::String fieldName () const = 0;

    // Return flag for each polarization, channel and row

    virtual void flag (casacore::Cube<casacore::Bool> & flags) const = 0;
    virtual void flag (casacore::Vector<casacore::Cube<casacore::Bool>> & flags) const = 0;

    // Return flag for each channel & row

    virtual void flag (casacore::Matrix<casacore::Bool> & flags) const = 0;

    // Determine whether FLAG_CATEGORY is valid.

    virtual casacore::Bool flagCategoryExists () const = 0;

    // Return flags for each polarization, channel, category, and row.

    virtual void flagCategory (casacore::Array<casacore::Bool> & flagCategories) const = 0;

    // Return row flag

    virtual void flagRow (casacore::Vector<casacore::Bool> & rowflags) const = 0;

    // Return the OBSERVATION_IDs

    virtual void observationId (casacore::Vector<casacore::Int> & obsids) const = 0;

    // Return current Polarization Id

    virtual casacore::Int polarizationId () const = 0;

    // Return the PROCESSOR_IDs

    virtual void processorId (casacore::Vector<casacore::Int> & procids) const = 0;

    // Return scan number

    virtual void scan (casacore::Vector<casacore::Int> & scans) const = 0;

    // Return the current Source Name

    virtual casacore::String sourceName () const = 0;

    // Return the STATE_IDs

    virtual void stateId (casacore::Vector<casacore::Int> & stateids) const = 0;


    // Return feed configuration matrix for specified antenna

    virtual void jonesC (casacore::Vector<casacore::SquareMatrix<casacore::Complex, 2> > & cjones) const = 0;

    // Return frame for polarization (returns PolFrame enum)

    virtual casacore::Int polFrame () const = 0;

    // Return sigma

    virtual void sigma (casacore::Matrix<casacore::Float> & sigmat) const = 0;
    virtual void sigma (casacore::Vector<casacore::Matrix<casacore::Float>> & sigmat) const = 0;

    // Return all the spectral windows ids for each row of the current buffer
    virtual void spectralWindows (casacore::Vector<casacore::Int> & spws) const = 0;

    // Return all the polarizations Ids for each row of the current buffer
    virtual void polarizationIds (casacore::Vector<casacore::Int> & polIds) const = 0;

    // Return MJD midpoint of interval.

    virtual void time (casacore::Vector<casacore::Double> & t) const = 0;

    // Return MJD centroid of interval.

    virtual void timeCentroid (casacore::Vector<casacore::Double> & t) const = 0;

    // Return nominal time interval

    virtual void timeInterval (casacore::Vector<casacore::Double> & ti) const = 0;

    // Return u,v and w (in meters)

    virtual void uvw (casacore::Matrix<casacore::Double> & uvwmat) const = 0;

    // Return the visibilities as found in the casacore::MS, casacore::Cube (npol,nchan,nrow).

    virtual void visibilityCorrected (casacore::Cube<casacore::Complex> & vis) const = 0;
    virtual void visibilityCorrected (casacore::Vector<casacore::Cube<casacore::Complex>> & vis) const = 0;
    virtual void visibilityModel (casacore::Cube<casacore::Complex> & vis) const = 0;
    virtual void visibilityModel (casacore::Vector<casacore::Cube<casacore::Complex>> & vis) const = 0;
    virtual void visibilityObserved (casacore::Cube<casacore::Complex> & vis) const = 0;
    virtual void visibilityObserved (casacore::Vector<casacore::Cube<casacore::Complex>> & vis) const = 0;

    // Return FLOAT_DATA as a casacore::Cube (npol, nchan, nrow) if found in the MS.

    virtual void floatData (casacore::Cube<casacore::Float> & fcube) const = 0;
    virtual void floatData (casacore::Vector<casacore::Cube<casacore::Float>> & fcube) const = 0;

    // Return the visibility 4-vector of polarizations for each channel.
    // If the casacore::MS doesn't contain all polarizations, it is assumed it
    // contains one or two parallel hand polarizations.

//    virtual void visibilityCorrected (casacore::Matrix<CStokesVector> & vis) const = 0;
//    virtual void visibilityModel (casacore::Matrix<CStokesVector> & vis) const = 0;
//    virtual void visibilityObserved (casacore::Matrix<CStokesVector> & vis) const = 0;

    // Return the shape of the visibility Cube

    virtual casacore::IPosition visibilityShape () const = 0;

    // Return weight

    virtual void weight (casacore::Matrix<casacore::Float> & wtmat) const = 0;
    virtual void weight (casacore::Vector<casacore::Matrix<casacore::Float>> & wtmat) const = 0;

    // Determine whether WEIGHT_SPECTRUM exists.

    virtual casacore::Bool weightSpectrumExists () const = 0;

    // Determine whether SIGMA_SPECTRUM exists.

    virtual casacore::Bool sigmaSpectrumExists () const = 0;

    // Return weightspectrum (a weight for each channel)

    virtual void weightSpectrum (casacore::Cube<casacore::Float> & wtsp) const = 0;
    virtual void weightSpectrum (casacore::Vector<casacore::Cube<casacore::Float>> & wtsp) const = 0;

    // Return sgimaspectrum (a sigma for each channel)

    virtual void sigmaSpectrum (casacore::Cube<casacore::Float> & wtsp) const = 0;
    virtual void sigmaSpectrum (casacore::Vector<casacore::Cube<casacore::Float>> & wtsp) const = 0;


    virtual void setWeightScaling (casacore::CountedPtr<WeightScaling> weightscaling) = 0;
    virtual casacore::Bool hasWeightScaling () const = 0;
    virtual casacore::CountedPtr<WeightScaling> getWeightScaling () const = 0;

    // Return the number of sub-intervals in the current chunk

    //   +------------------------+
    //   |                        |
    //   | Angular Data Providers |
    //   |                        |
    //   +------------------------+

    // true if all elements of the cube returned by getBeamOffsets are zero

    virtual casacore::Bool allBeamOffsetsZero () const = 0;

    // Return the antenna AZ/EL casacore::Vector (nant)

    virtual casacore::MDirection azel0 (casacore::Double time) const = 0;
    static void azel0Calculate (casacore::Double time, casacore::MSDerivedValues & msd,
                                casacore::MDirection & azel0, const casacore::MEpoch & mEpoch0);

    virtual const casacore::Vector<casacore::MDirection> & azel (casacore::Double time) const = 0;
    static void azelCalculate (casacore::Double time, casacore::MSDerivedValues & msd, casacore::Vector<casacore::MDirection> & azel,
                               casacore::Int nAnt, const casacore::MEpoch & mEpoch0);

    // Return feed parallactic angles casacore::Vector (nant) (1 feed/ant)

    virtual const casacore::Vector<casacore::Float> & feed_pa (casacore::Double time) const = 0;
    static casacore::Vector<casacore::Float> feed_paCalculate (casacore::Double time, casacore::MSDerivedValues & msd,
                                           casacore::Int nAntennas, const casacore::MEpoch & mEpoch0,
                                           const casacore::Vector<casacore::Float> & receptor0Angle);

    // Return a cube containing pairs of coordinate offsets for each
    // receptor of each feed (values are in radians, coordinate system is fixed
    // with antenna and is the same one as used to define the BEAM_OFFSET
    // parameter in the feed table). The cube axes are receptor, antenna, feed.

    virtual const casacore::Cube<casacore::RigidVector<casacore::Double, 2> > & getBeamOffsets () const = 0;

    virtual std::pair<bool, casacore::MDirection> getPointingAngle (int antenna, double time) const = 0;

    // Return the hour angle for the specified time

    virtual casacore::Double hourang (casacore::Double time) const = 0;
    static casacore::Double hourangCalculate (casacore::Double time, casacore::MSDerivedValues & msd, const casacore::MEpoch & mEpoch0);

    // Return nominal parallactic angle at specified time
    // (does not include feed position angle offset--see feed_pa)
    // A global value for all antennas (e.g., small array)

    virtual const casacore::Float & parang0 (casacore::Double time) const = 0;
    static casacore::Float parang0Calculate (casacore::Double time, casacore::MSDerivedValues & msd, const casacore::MEpoch & epoch0);

    // Per antenna:

    virtual const casacore::Vector<casacore::Float> & parang (casacore::Double time) const = 0;
    static casacore::Vector<casacore::Float> parangCalculate (casacore::Double time, casacore::MSDerivedValues & msd,
                                          int nAntennas, const casacore::MEpoch mEpoch0);

    // Return the current phase center as an MDirection

    virtual const casacore::MDirection & phaseCenter () const = 0;

    // Return receptor angles for all antennae and feeds
    // First axis of the cube is a receptor number,
    // 2nd is antennaId, 3rd is feedId
    // Note: the method is intended to provide an access to MSIter::receptorAngles
    // for VisBuffer in the multi-feed case. It may be worth to change the
    // interface of feed_pa to return the information for all feeds.

    virtual const casacore::Cube<casacore::Double> & receptorAngles () const = 0;

    //   +=========================+
    //   |                         |
    //   | Chunk and MS Level Data |
    //   |                         |
    //   +=========================+

    // return a string mount identifier for each antenna

    virtual const casacore::Vector<casacore::String> & antennaMounts () const = 0;
    //    virtual casacore::Vector<casacore::MPosition> antennaPositions () const;
    //casacore::MSDerivedValues makeMsd ();

    virtual casacore::MEpoch getEpoch () const = 0;

    // Return imaging weight (a weight for each channel)
    //virtual casacore::Matrix<casacore::Float> & imagingWeight (casacore::Matrix<casacore::Float> & wt) const = 0;

    virtual const VisImagingWeight & getImagingWeightGenerator () const = 0;

    virtual casacore::MFrequency::Types getObservatoryFrequencyType () const = 0; //???
    virtual casacore::MPosition getObservatoryPosition () const = 0;
    virtual casacore::Vector<casacore::Float> getReceptor0Angle () = 0;

    virtual casacore::Int getReportingFrameOfReference () const = 0;
    virtual void setReportingFrameOfReference (casacore::Int frame) = 0;

    virtual casacore::Vector<casacore::Int> getCorrelations () const = 0;
    virtual casacore::Vector<casacore::Stokes::StokesTypes> getCorrelationTypesDefined () const = 0;
    virtual casacore::Vector<casacore::Stokes::StokesTypes> getCorrelationTypesSelected () const = 0;

    virtual casacore::Vector<casacore::Int> getChannels (casacore::Double time, casacore::Int frameOfReference, casacore::Int spectralWndow = -1,
                                     casacore::Int msId = -1) const = 0;
    virtual casacore::Vector<casacore::Double> getFrequencies (casacore::Double time, casacore::Int frameOfReference, casacore::Int spectralWndow = -1,
                                           casacore::Int msId = -1) const = 0;
    virtual casacore::Vector<casacore::Double> getChanWidths (casacore::Double time, casacore::Int frameOfReference, casacore::Int spectralWndow = -1,
                                           casacore::Int msId = -1) const = 0;


    //reference to actual ms in interator

    virtual casacore::Int msId () const = 0; // zero-based index of current casacore::MS in set of MSs
    virtual const casacore::MeasurementSet & ms () const = 0;
    virtual casacore::Int getNMs () const = 0;

    // Name of current nominal MS
    virtual casacore::String msName() const = 0;

    // Call to use the slurp i/o method for all scalar columns. This
    // will set the casacore::BucketCache cache size to the full column length
    // and cause the full column to be cached in memory, if
    // any value of the column is used. In case of out-of-memory,
    // it will automatically fall-back on the smaller cache size.
    // Slurping the column is to be considered as a work-around for the
    // casacore::Table i/o code, which uses casacore::BucketCache and performs extremely bad
    // for random access. Slurping is useful when iterating non-sequentially
    // an casacore::MS or parts of an casacore::MS, it is not tested with multiple MSs.

    virtual void slurp () const = 0;

    // Access the current casacore::MSColumns object in MSIter

    virtual const vi::SubtableColumns & subtableColumns () const = 0;

    // get back the selected spectral windows and spectral channels for
    // current ms

    virtual const SpectralWindowChannels & getSpectralWindowChannels (casacore::Int msId, casacore::Int spectralWindowId) const = 0;

    //assign a VisImagingWeight object to this iterator

    virtual void useImagingWeight (const VisImagingWeight & imWgt) = 0;

    // Return number of antennasm spws, polids, ddids

    virtual casacore::Int nAntennas () const = 0;
    virtual casacore::Int nDataDescriptionIds () const = 0;
    virtual casacore::Int nPolarizationIds () const = 0;
    virtual casacore::rownr_t nRowsInChunk () const = 0; // number rows in current chunk
    virtual casacore::rownr_t nRowsViWillSweep () const = 0; // number of rows in all selected ms's
    virtual casacore::Int nSpectralWindows () const = 0;
    // number of unique time stamps in chunk
    virtual casacore::Int nTimes() const = 0;

    //   +-------------------+
    //   |                   |
    //   | Writeback Methods |
    //   |                   |
    //   +-------------------+

    // This method writes back any changed (dirty) components of the provided
    // VisBuffer and is the preferred method for writing data out.

    virtual void writeBackChanges (VisBuffer2 * vb) = 0;

    // Write/modify the flags in the data.
    // This will flag all channels in the original data that contributed to
    // the output channel in the case of channel averaging.
    // All polarizations have the same flag value.
//    virtual void writeFlag (const casacore::Matrix<casacore::Bool> & flag) = 0;

    // Write/modify the flags in the data.
    // This writes the flags as found in the casacore::MS, casacore::Cube (npol,nchan,nrow),
    // where nrow is the number of rows in the current iteration (given by
    // nRow ()).
    virtual void writeFlag (const casacore::Cube<casacore::Bool> & flag) = 0;

    // Write/modify the flag row column = 0; dimension casacore::Vector (nrow)
    virtual void writeFlagRow (const casacore::Vector<casacore::Bool> & rowflags) = 0;

    virtual void writeFlagCategory(const casacore::Array<casacore::Bool>& fc) = 0;

    // Write/modify the visibilities.
    // This is possibly only for a 'reference' casacore::MS which has a new DATA column.
    // The first axis of the matrix should equal the selected number of channels
    // in the original MS.
    // If the casacore::MS does not contain all polarizations, only the parallel
    // hand polarizations are used.
//    virtual void writeVisCorrected (const casacore::Matrix<CStokesVector> & visibilityStokes) = 0;
//    virtual void writeVisModel (const casacore::Matrix<CStokesVector> & visibilityStokes) = 0;
//    virtual void writeVisObserved (const casacore::Matrix<CStokesVector> & visibilityStokes) = 0;

    // Write/modify the visibilities
    // This writes the data as found in the casacore::MS, casacore::Cube (npol,nchan,nrow).
    virtual void writeVisCorrected (const casacore::Cube<casacore::Complex> & vis) = 0;
    virtual void writeVisModel (const casacore::Cube<casacore::Complex> & vis) = 0;
    virtual void writeVisObserved (const casacore::Cube<casacore::Complex> & vis) = 0;

    // Write/modify the weights
    virtual void writeWeight (const casacore::Matrix<casacore::Float> & wt) = 0;

    // Write/modify the weightMat
    //virtual void writeWeightMat (const casacore::Matrix<casacore::Float> & wtmat) = 0;

    // Write/modify the weightSpectrum
    virtual void writeWeightSpectrum (const casacore::Cube<casacore::Float> & wtsp) = 0;

    // Initialize the weightSpectrum
    virtual void initWeightSpectrum (const casacore::Cube<casacore::Float> & /* wtsp */) {};

    // Write/modify the sigmaSpectrum
    virtual void writeSigmaSpectrum (const casacore::Cube<casacore::Float> & sigsp) = 0;

    // Initialize the sigmaSpectrum
    virtual void initSigmaSpectrum (const casacore::Cube<casacore::Float> & /* sigsp */) {};

    // Write/modify the Sigma
    virtual void writeSigma (const casacore::Matrix<casacore::Float> & sig) = 0;

    // Write/modify the ncorr x nrow SigmaMat.
    //virtual void writeSigmaMat (const casacore::Matrix<casacore::Float> & sigmat) = 0;

    // Write the information needed to generate on-the-fly model visibilities.

    virtual void writeModel(const casacore::RecordInterface& rec, casacore::Bool iscomponentlist=true,
                            casacore::Bool incremental=false) = 0;

    //**********************************************************************
    // Methods to access the subtables.
    // Note that for some subclasses, like TransformingTVI, those tables
    // might be created on the fly, rather than read from the MS.
    //**********************************************************************

    // Access to antenna subtable
    virtual const casacore::MSAntennaColumns& antennaSubtablecols() const = 0;

    // Access to dataDescription subtable
    virtual const casacore::MSDataDescColumns& dataDescriptionSubtablecols() const = 0;

    // Access to feed subtable
    virtual const casacore::MSFeedColumns& feedSubtablecols() const = 0;

    // Access to field subtable
    virtual const casacore::MSFieldColumns& fieldSubtablecols() const = 0;

    // Access to flagCmd subtable
    virtual const casacore::MSFlagCmdColumns& flagCmdSubtablecols() const = 0;

    // Access to history subtable
    virtual const casacore::MSHistoryColumns& historySubtablecols() const = 0;

    // Access to observation subtable
    virtual const casacore::MSObservationColumns& observationSubtablecols() const = 0;

    // Access to pointing subtable
    virtual const casacore::MSPointingColumns& pointingSubtablecols() const = 0;

    // Access to polarization subtable
    virtual const casacore::MSPolarizationColumns& polarizationSubtablecols() const = 0;

    // Access to processor subtable
    virtual const casacore::MSProcessorColumns& processorSubtablecols() const = 0;

    // Access to spectralWindow subtable
    virtual const casacore::MSSpWindowColumns& spectralWindowSubtablecols() const = 0;

    // Access to state subtable
    virtual const casacore::MSStateColumns& stateSubtablecols() const = 0;

    // Access to doppler subtable
    virtual const casacore::MSDopplerColumns& dopplerSubtablecols() const = 0;

    // Access to freqOffset subtable
    virtual const casacore::MSFreqOffsetColumns& freqOffsetSubtablecols() const = 0;

    // Access to source subtable
    virtual const casacore::MSSourceColumns& sourceSubtablecols() const = 0;

    // Access to sysCal subtable
    virtual const casacore::MSSysCalColumns& sysCalSubtablecols() const = 0;

    // Access to weather subtable
    virtual const casacore::MSWeatherColumns& weatherSubtablecols() const = 0;


protected:

	virtual VisBuffer2 *createAttachedVisBuffer(VisBufferOptions options);

	VisBuffer2 * getVisBuffer(const VisibilityIterator2 *) const;

    static void doWeightScaling (casacore::Bool hasWeightScaling,
                                 WeightScaling * scaling,
                                 const casacore::Array<casacore::Float>& unscaled,
                                 casacore::Array<casacore::Float>& scaled);
    //static void setVisBufferFillable (VisBuffer2 * vb, bool fillable);
};

} // end namespace vi

} //# NAMESPACE CASA - END

#endif // ! defined (MSVIS_ViImplementation2_H_121115_0950)


