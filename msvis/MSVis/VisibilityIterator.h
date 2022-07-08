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

#ifndef MSVIS_VISIBILITYITERATOR_H
#define MSVIS_VISIBILITYITERATOR_H

#include <casacore/casa/Arrays/Cube.h>
#include <casacore/casa/Arrays/Matrix.h>
#include <casacore/casa/Arrays/Slicer.h>
#include <casacore/casa/BasicSL/String.h>
#include <casacore/casa/Containers/RecordInterface.h>
#include <casacore/casa/Quanta/MVDoppler.h>
#include <casacore/casa/aips.h>
#include <casacore/measures/Measures/MCDoppler.h>
#include <casacore/measures/Measures/MDoppler.h>
#include <casacore/measures/Measures/MeasConvert.h>
#include <casacore/measures/Measures/Stokes.h>
#include <casacore/ms/MSOper/MSDerivedValues.h>
#include <casacore/ms/MeasurementSets/MSIter.h>
#include <casacore/ms/MeasurementSets/MeasurementSet.h>
#include <casacore/scimath/Mathematics/RigidVector.h>
#include <casacore/scimath/Mathematics/SquareMatrix.h>
#include <msvis/MSVis/StokesVector.h>
#include <msvis/MSVis/VisBufferComponents.h>
#include <msvis/MSVis/VisImagingWeight.h>
#include <casacore/tables/Tables/ArrayColumn.h>
#include <casacore/tables/Tables/ScalarColumn.h>

#include <map>
#include <set>
#include <vector>

namespace casa { //# NAMESPACE CASA - BEGIN

//# forward decl

namespace asyncio {

class VLAT;

// <summary>
// The PrefetchColumns class is used to specify a set of columns that
// can be prefetched when the (RO)VisibilityIterator is using asynchronous
// I/O.
// </summary>
//
// <synopsis>
// When creating an ROVisibilityIterator or VisibilityIterator that can
// potentially operate using asynchronous I/O it is necessary to specify
// which columns of the main table are to be prefetched by the lookahead
// thread.  This class provides the way to specify those columns.  The
// class is an STL set object of enum values defined in VisBufferComponents.h.
// These roughly correspond to the different components that can be access
// via a VisBuffer.
//
// </synopsis>
//
// <example>
// This is a simple example showing the construction of a PrefetchColumns
// and its use in a VisibilityIterator constructor.  For more complex cases
// (e.g., where the columns to be prefetched depend on other factors) the
// class provides some additional utility methods.
//
// Usually the file that creates the VI will include VisBuffer which will
// then include VisBufferComponents.h; if not then the user will also need
// to add "#include <msvis/MSVis/VisBufferComponents.h>" to their file.
//
// <code>
//    asyncio::PrefetchColumns prefetchColumns =
//            PrefetchColumns::prefetchColumns (VisBufferComponents::Ant1,
//                                              VisBufferComponents::Ant2,
//                                              VisBufferComponents::ArrayId,
//                                              VisBufferComponents::Direction1,
//                                              VisBufferComponents::Direction2,
//                                              VisBufferComponents::Feed1,
//                                              VisBufferComponents::Feed1_pa,
//                                              VisBufferComponents::Feed2,
//                                              VisBufferComponents::Feed2_pa,
//                                              VisBufferComponents::FieldId,
//                                              VisBufferComponents::FlagCube,
//                                              VisBufferComponents::Flag,
//                                              VisBufferComponents::FlagRow,
//                                              VisBufferComponents::Freq,
//                                              VisBufferComponents::NChannel,
//                                              VisBufferComponents::NCorr,
//                                              VisBufferComponents::NRow,
//                                              VisBufferComponents::ObservedCube,
//                                              VisBufferComponents::PhaseCenter,
//                                              VisBufferComponents::PolFrame,
//                                              VisBufferComponents::SpW,
//                                              VisBufferComponents::Time,
//                                              VisBufferComponents::Uvw,
//                                              VisBufferComponents::UvwMat,
//                                              VisBufferComponents::Weight,
//                                              -1);
//
//     wvi_p = new VisibilityIterator (& prefetchColumns, * wvi_p);
//
//
// </code>
//
// </example
class PrefetchColumns : public std::set<VisBufferComponents::EnumType>{

public:

    PrefetchColumns operator+ (const PrefetchColumns & other);

    static casacore::String columnName (casacore::Int id);
    static PrefetchColumns prefetchColumnsAll ();
    static PrefetchColumns prefetchAllColumnsExcept (casacore::Int firstColumn, ...);
    static PrefetchColumns prefetchColumns (casacore::Int firstColumn, ...);
};

} // end namespace asyncio

class VisBuffer;
class VisibilityIteratorReadImpl;
class VisibilityIteratorWriteImpl;

class SubChunkPair : public std::pair<casacore::Int, casacore::Int>{

public:

    // First component is Chunk and second is subchunk

    SubChunkPair () { resetToOrigin ();}
    SubChunkPair (casacore::Int a , casacore::Int b) : pair<casacore::Int,casacore::Int> (a,b) {}

    casacore::Bool operator== (const SubChunkPair & other){
        return first == other.first && second == other.second;
    }

    casacore::Bool operator< (const SubChunkPair & other){
        return first < other.first ||
               (first == other.first && second < other.second);
    }

    casacore::Bool atOrigin () const { return * this == SubChunkPair ();}
    casacore::Int chunk () const { return first;}
    void incrementSubChunk () { second ++;}
    void incrementChunk () { first ++; second = 0; }

    void resetSubChunk () { second = 0;} // position to start of chunk

    void resetToOrigin () { first = 0; second = 0; }
       // Make a subchunk pair that is positioned to the first subchunk of the
       // first chunk (i.e., 0,0)

    casacore::Int subchunk () const { return second;}
    casacore::String toString () const;

    static SubChunkPair noMoreData ();

private:


};


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
//   <li> <linkto class="PrefetchColumns">PrefetchColumns</linkto>
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
//
// ROVisibilityIterators can be either synchronous or asynchronous, depending
// on the constructor used to create them as well as the current value of
// a CASARC file setting.  A synchronous instance is works the same as
// this class ever worked; an asynchronous instance uses a second thread
// (the Visibility Lookahead Thread or VLAT) to fill the VisBuffers in
// advance of their use by the original thread.
//
// To create an asynchronous instance of ROVI you must use one of the two
// constructors which have a pointer to a PrefetchColumns object as the
// first argument.  This object specifies which table columns should be
// prefetched by the VLAT; accessing columns not specified in the PrefetchColumns
// object will result in an exception containing an error message indicating
// that a the VisBuffer does not contain the requested column.  In addition
// to using the appropriate constructor, the CASARC file setting
// VisibilityIterator.
//
// +-------------------+
// |                   |
// | *** Nota Bene *** |
// |                   |
// +-------------------+
//
// Because of the multithreaded nature of asynchronous I/O, the user
// needs to be a bit more careful in the use of the VI and it's attached VisBuffer.
// casacore::Data access operations need to be directed to the VisBuffer.  Additionally
// the user must not attempt to access the data using a separate VI since
// the underlying casacore objects are not threadsafe and bizarre errors
// will likely occur.
//
// CASARC Settings
// ===============
//
// casacore::Normal settings
// ---------------
//
// VisibilityIterator.async.enabled - Boolean value that enables or disables
//     async I/O.  The default value is currently false (i.e., disabled).
// VisibilityIterator.async.nBuffers - The number of lookahead buffers.  This
//     defaults to 2.
//
//
// Debug settings
// --------------
//
// VisibilityIterator.async.doStats: true
// VisibilityIterator.async.debug.logFile: stderr
// VisibilityIterator.async.debug.logLevel: 1
//

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
class ROVisibilityIterator
{
    friend class AsyncEnabler;
    friend class VisibilityIteratorReadImpl;
    friend class ViReadImplAsync;
    friend class asyncio::VLAT; // allow VI lookahead thread class to access protected functions
                                // VLAT should not access private parts, especially variables
public:

  class Factory {

  public:

      virtual ~Factory () {}

      virtual VisibilityIteratorReadImpl *
      operator() (const asyncio::PrefetchColumns * /*prefetchColumns*/,
                  const casacore::Block<casacore::MeasurementSet>& /*mss*/,
                  const casacore::Block<casacore::Int>& /*sortColumns*/,
                  const casacore::Bool /*addDefaultSortCols*/,
                  casacore::Double /*timeInterval*/) const
      {
          return NULL;
      }
  };

  //# the following is a copy of the enum in MSIter
  //# can't think of a way to get one that known to the outside world from here
  enum PolFrame {
    // Circular polarization
    Circular=0,
    // Linear polarization
    Linear=1
  };

  typedef enum DataColumn {
      Observed=0,  // Observed data
      Model,       // Model data
      Corrected    // Corrected data
  } DataColumn;

  class AsyncEnabler {
  public:
      AsyncEnabler (ROVisibilityIterator &);
      AsyncEnabler (ROVisibilityIterator *);
      ~AsyncEnabler ();
      void release ();
  private:
      casacore::Bool oldEnabledState_p;
      ROVisibilityIterator * roVisibilityIterator_p;
  };

  // Default constructor - useful only to assign another iterator later
  ROVisibilityIterator();
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
  // and FIELD_ID).  Using selectChannel(), a number of groups of channels can
  // be requested.  At present the channel group iteration will always occur
  // before the interval iteration.
  ROVisibilityIterator(const casacore::MeasurementSet& ms, 
		       const casacore::Block<casacore::Int>& sortColumns,
		       casacore::Double timeInterval=0,
		       const Factory & factory = Factory());
  // Same as above, but with the option of using the raw order of ms
  // (addDefaultSortCols=false).
  ROVisibilityIterator(const casacore::MeasurementSet& ms, 
		       const casacore::Block<casacore::Int>& sortColumns,
		       const casacore::Bool addDefaultSortCols,
		       casacore::Double timeInterval=0);
 
  // Same as previous constructors, but with multiple MSs to iterate over.
  ROVisibilityIterator(const casacore::Block<casacore::MeasurementSet>& mss,
		       const casacore::Block<casacore::Int>& sortColumns, 
		       casacore::Double timeInterval=0);

  ROVisibilityIterator(const casacore::Block<casacore::MeasurementSet>& mss,
                       const casacore::Block<casacore::Int>& sortColumns,
                       const casacore::Bool addDefaultSortCols,
                       casacore::Double timeInterval=0);

  ROVisibilityIterator(const asyncio::PrefetchColumns * prefetchColumns,
                       const casacore::MeasurementSet& ms,
		       const casacore::Block<casacore::Int>& sortColumns,
		       const casacore::Bool addDefaultSortCols = true,
		       casacore::Double timeInterval = 0);

  ROVisibilityIterator (const asyncio::PrefetchColumns * prefetchColumns,
                        const casacore::Block<casacore::MeasurementSet>& mss,
                        const casacore::Block<casacore::Int>& sortColumns,
                        const casacore::Bool addDefaultSortCols = true,
                        casacore::Double timeInterval = 0);

  // Copy construct. This calls the assigment operator.
  ROVisibilityIterator(const ROVisibilityIterator & other);
  ROVisibilityIterator(const asyncio::PrefetchColumns * prefetchColumns, const ROVisibilityIterator & other);
  // Assigment. Any attached VisBuffers are lost in the assign.
  ROVisibilityIterator & operator=(const ROVisibilityIterator &other);
  // Destructor
  virtual ~ROVisibilityIterator();
  
//  static ROVisibilityIterator * create (const casacore::MeasurementSet & ms,
//                                        const casacore::Block<casacore::Int>& sortColumns,
//                                        const asyncio::PrefetchColumns * prefetchColumns = NULL,
//                                        const casacore::Bool addDefaultSortCols = true,
//                                        casacore::Double timeInterval=0);
//
//  static ROVisibilityIterator * create (const casacore::Block<casacore::MeasurementSet>& mss,
//                                        const casacore::Block<casacore::Int>& sortColumns,
//                                        const asyncio::PrefetchColumns * prefetchColumns = NULL,
//                                        const casacore::Bool addDefaultSortCols = true,
//                                        casacore::Double timeInterval=0);
//
//  static ROVisibilityIterator * create (const ROVisibilityIterator & rovi,
//                                        const asyncio::PrefetchColumns * prefetchColumns,
//                                        casacore::Int nReadAheadBuffers = 2);

  // Members
  
  casacore::Bool isAsynchronous () const;
  static casacore::Bool isAsynchronousIoEnabled();

  casacore::Bool isAsyncEnabled () const;
  casacore::Bool isWritable () const;
  // Reset iterator to origin/start of data (of current chunk)
  void origin();
  // Reset iterator to true start of data (first chunk)
  void originChunks();
 
  // Return the time interval (in seconds) used for iteration.
  // This is not the same as the INTERVAL column.
  casacore::Double getInterval() const;
  // Set or reset the time interval (in seconds) to use for iteration.
  // You should call originChunks() to reset the iteration after 
  // calling this.
  void setInterval(casacore::Double timeInterval);

  // Set the 'blocking' size for returning data.
  // With the default (0) only a single integration is returned at a time, this
  // is what is currently required for the calibration software. With blocking
  // set, up to nRows can be returned in one go. The chunk 
  // size determines the actual maximum.
  void setRowBlocking(casacore::Int nRows=0);
  // Return false if no more data (in current chunk)
  casacore::Bool more() const;
  SubChunkPair getSubchunkId () const;
  // Advance iterator through data
  ROVisibilityIterator & operator++(int);
  ROVisibilityIterator & operator++();
  // Return false if no more 'Chunks' of data left
  casacore::Bool moreChunks() const;
  // Check if ms has change since last iteration
  casacore::Bool newMS() const;
  casacore::Int msId() const;
  VisBuffer * getVisBuffer ();
  //reference to actual ms in interator 
  const casacore::MeasurementSet& ms() const;
 // Advance to the next Chunk of data
  ROVisibilityIterator& nextChunk();
  // Return antenna1
  casacore::Vector<casacore::Int>& antenna1(casacore::Vector<casacore::Int>& ant1) const;
  // Return antenna2
  casacore::Vector<casacore::Int>& antenna2(casacore::Vector<casacore::Int>& ant2) const;
  // Return feed1
  casacore::Vector<casacore::Int>& feed1(casacore::Vector<casacore::Int>& fd1) const;
  // Return feed2
  casacore::Vector<casacore::Int>& feed2(casacore::Vector<casacore::Int>& fd2) const;
  // Return channel numbers in selected VisSet spectrum
  // (i.e. disregarding possible selection on the iterator, but
  //  including the selection set when creating the VisSet)
  virtual casacore::Vector<casacore::Int>& channel(casacore::Vector<casacore::Int>& chan) const;
  // Return feed configuration matrix for specified antenna
  casacore::Vector<casacore::SquareMatrix<casacore::Complex,2> >& 
  CJones(casacore::Vector<casacore::SquareMatrix<casacore::Complex,2> >& cjones) const;
  // Return receptor angles for all antennae and feeds
  // First axis of the cube is a receptor number,
  // 2nd is antennaId, 3rd is feedId
  // Note: the method is intended to provide an access to MSIter::receptorAngles
  // for VisBuffer in the multi-feed case. It may be worth to change the
  // interface of feed_pa to return the information for all feeds.
  const casacore::Cube<casacore::Double>& receptorAngles() const;
  // return a string mount identifier for each antenna
  const casacore::Vector<casacore::String>& antennaMounts() const;
  // Return a cube containing pairs of coordinate offsets for each
  // receptor of each feed (values are in radians, coordinate system is fixed
  // with antenna and is the same one as used to define the BEAM_OFFSET 
  // parameter in the feed table). The cube axes are receptor, antenna, feed.
  const casacore::Cube<casacore::RigidVector<casacore::Double, 2> >& getBeamOffsets() const;
  // true if all elements of the cube returned by getBeamOffsets are zero
  casacore::Bool allBeamOffsetsZero() const;
  // Return feed parallactic angles casacore::Vector(nant) (1 feed/ant)
  casacore::Vector<casacore::Float> feed_pa(casacore::Double time) const;
  static casacore::Vector<casacore::Float> feed_paCalculate(casacore::Double time, casacore::MSDerivedValues & msd,
  									    casacore::Int nAntennas, const casacore::MEpoch & mEpoch0,
									    const casacore::Vector<casacore::Float> & receptor0Angle);
  // Return nominal parallactic angle at specified time
  // (does not include feed position angle offset--see feed_pa)
  // A global value for all antennas (e.g., small array)
  const casacore::Float& parang0(casacore::Double time) const;
  static casacore::Float parang0Calculate (casacore::Double time, casacore::MSDerivedValues & msd, const casacore::MEpoch & epoch0);
  // Per antenna:
  casacore::Vector<casacore::Float> parang(casacore::Double time) const;
  static casacore::Vector<casacore::Float> parangCalculate (casacore::Double time, casacore::MSDerivedValues & msd,
		                                int nAntennas, const casacore::MEpoch mEpoch0);
  // Return the antenna AZ/EL casacore::Vector(nant) 
  casacore::MDirection azel0(casacore::Double time) const;
  static void azel0Calculate (casacore::Double time, casacore::MSDerivedValues & msd,
		                      casacore::MDirection & azel0, const casacore::MEpoch & mEpoch0);
  casacore::Vector<casacore::MDirection> azel(casacore::Double time) const;
  static void azelCalculate (casacore::Double time, casacore::MSDerivedValues & msd, casacore::Vector<casacore::MDirection> & azel,
		                     casacore::Int nAnt, const casacore::MEpoch & mEpoch0);
  // Return the hour angle for the specified time
  casacore::Double hourang(casacore::Double time) const;
  static casacore::Double hourangCalculate (casacore::Double time, casacore::MSDerivedValues & msd, const casacore::MEpoch & mEpoch0);
  // Return the current FieldId
  casacore::Int fieldId() const;
  // Return the current ArrayId
  casacore::Int arrayId() const;
  // Return the current Field Name
  casacore::String fieldName() const;
  // Return the current Source Name
  casacore::String sourceName() const;
  // Return flag for each polarization, channel and row
  virtual casacore::Cube<casacore::Bool>& flag(casacore::Cube<casacore::Bool>& flags) const;
  // Return flag for each channel & row
  casacore::Matrix<casacore::Bool>& flag(casacore::Matrix<casacore::Bool>& flags) const;

  // Determine whether FLAG_CATEGORY is valid.
  casacore::Bool existsFlagCategory() const;

  // Return flags for each polarization, channel, category, and row.
  casacore::Array<casacore::Bool>& flagCategory(casacore::Array<casacore::Bool>& flagCategories) const;
  // Return row flag
  casacore::Vector<casacore::Bool>& flagRow(casacore::Vector<casacore::Bool>& rowflags) const;
  // Return scan number
  casacore::Vector<casacore::Int>& scan(casacore::Vector<casacore::Int>& scans) const;
  // Return the OBSERVATION_IDs
  casacore::Vector<casacore::Int>& observationId(casacore::Vector<casacore::Int>& obsids) const;
  // Return the PROCESSOR_IDs
  casacore::Vector<casacore::Int>& processorId(casacore::Vector<casacore::Int>& procids) const;
  // Return the STATE_IDs
  casacore::Vector<casacore::Int>& stateId(casacore::Vector<casacore::Int>& stateids) const;
  // Return current frequencies (in Hz, acc. to the casacore::MS def'n v.2)
  virtual casacore::Vector<casacore::Double>& frequency(casacore::Vector<casacore::Double>& freq) const;
  // Return frequencies  (in Hz, acc. to the casacore::MS def'n v.2) in selected velocity frame,
  // returns the same as frequency() if there is no vel selection active.
  casacore::Vector<casacore::Double>& lsrFrequency(casacore::Vector<casacore::Double>& freq) const;
  // Return the current phase center as an MDirection
  const casacore::MDirection& phaseCenter() const;
  const casacore::MDirection phaseCenter(const casacore::Int fieldid, const casacore::Double time=-1.0) const;
  // Return frame for polarization (returns PolFrame enum)
  casacore::Int polFrame() const;
  // Return the correlation type (returns casacore::Stokes enums)
  virtual casacore::Vector<casacore::Int>& corrType(casacore::Vector<casacore::Int>& corrTypes) const;
  // Return sigma
  casacore::Vector<casacore::Float>& sigma(casacore::Vector<casacore::Float>& sig) const;
  // Return sigma matrix (pol-dep)
  virtual casacore::Matrix<casacore::Float>& sigmaMat(casacore::Matrix<casacore::Float>& sigmat) const;
  // Return current SpectralWindow
  casacore::Int spectralWindow() const;
  // Return current Polarization Id
  casacore::Int polarizationId() const;
  // Return current DataDescription Id
  casacore::Int dataDescriptionId() const;
  // Return MJD midpoint of interval.
  casacore::Vector<casacore::Double>& time(casacore::Vector<casacore::Double>& t) const;
  // Return MJD centroid of interval.
  casacore::Vector<casacore::Double>& timeCentroid(casacore::Vector<casacore::Double>& t) const;
  // Return nominal time interval
  casacore::Vector<casacore::Double>& timeInterval(casacore::Vector<casacore::Double>& ti) const;
  // Return actual time interval
  casacore::Vector<casacore::Double>& exposure(casacore::Vector<casacore::Double>& expo) const;
  // Return the visibilities as found in the casacore::MS, casacore::Cube(npol,nchan,nrow).
  virtual casacore::Cube<casacore::Complex>& visibility(casacore::Cube<casacore::Complex>& vis,
				    DataColumn whichOne) const;
  // Return FLOAT_DATA as a casacore::Cube(npol, nchan, nrow) if found in the MS.
  casacore::Cube<casacore::Float>& floatData(casacore::Cube<casacore::Float>& fcube) const;
  // Return the visibility 4-vector of polarizations for each channel.
  // If the casacore::MS doesn't contain all polarizations, it is assumed it
  // contains one or two parallel hand polarizations.
  casacore::Matrix<CStokesVector>& visibility(casacore::Matrix<CStokesVector>& vis, 
				    DataColumn whichOne) const;
  // Return the shape of the visibility Cube
  casacore::IPosition visibilityShape() const;
  // Return u,v and w (in meters)
  virtual casacore::Vector<casacore::RigidVector<casacore::Double,3> >& uvw(casacore::Vector<casacore::RigidVector<casacore::Double,3> >& uvwvec) const;
  casacore::Matrix<casacore::Double>& uvwMat(casacore::Matrix<casacore::Double>& uvwmat) const;
  // Return weight
  casacore::Vector<casacore::Float>& weight(casacore::Vector<casacore::Float>& wt) const;
  // Returns the nPol_p x curNumRow_p weight matrix
  virtual casacore::Matrix<casacore::Float>& weightMat(casacore::Matrix<casacore::Float>& wtmat) const;
  // Determine whether WEIGHT_SPECTRUM exists.
  casacore::Bool existsWeightSpectrum() const;
  // Return weightspectrum (a weight for each channel)
  casacore::Cube<casacore::Float>& weightSpectrum(casacore::Cube<casacore::Float>& wtsp) const;
  // Return imaging weight (a weight for each channel)
  //casacore::Matrix<casacore::Float>& imagingWeight(casacore::Matrix<casacore::Float>& wt) const;
  // Return true if FieldId/Source has changed since last iteration
  casacore::Bool newFieldId() const;
  // Return true if arrayID has changed since last iteration
  casacore::Bool newArrayId() const;
  // Return true if SpectralWindow has changed since last iteration
  casacore::Bool newSpectralWindow() const;
  // Return the index of the first channel of the current channel group 
  // in the total (selected) spectrum.
  casacore::Int channelIndex() const;
  // Return the width of the current group of channels, i.e.,
  // the number of channels returned by visibility() and frequency().
  casacore::Int channelGroupSize() const;
  
  // Return the number of correlations in the current iteration
  casacore::Int nCorr() const;;
  // Return the number of rows in the current iteration
  casacore::Int nRow() const;
  // Return the row ids as from the original root table. This is useful 
  // to find correspondance between a given row in this iteration to the 
  // original ms row
  virtual casacore::Vector<casacore::rownr_t>& rowIds(casacore::Vector<casacore::rownr_t>& rowids) const;
  // Return the numbers of rows in the current chunk
  casacore::Int nRowChunk() const;
  // Return the number of sub-intervals in the current chunk
  casacore::Int nSubInterval() const;
  // Call to use the slurp i/o method for all scalar columns. This
  // will set the casacore::BucketCache cache size to the full column length
  // and cause the full column to be cached in memory, if
  // any value of the column is used. In case of out-of-memory,
  // it will automatically fall-back on the smaller cache size.
  // Slurping the column is to be considered as a work-around for the
  // casacore::Table i/o code, which uses casacore::BucketCache and performs extremely bad
  // for random access. Slurping is useful when iterating non-sequentially
  // an casacore::MS or parts of an casacore::MS, it is not tested with multiple MSs.
  void slurp() const;
  // Velocity selection - specify the output channels in velocity:
  // nChan - number of output channels, vStart - start velocity,
  // vInc - velocity increment. So channel i will have velocity 
  // vStart + i*vInc (i=0,nChan-1).
  // Specify velocities as in e.g., casacore::MVRadialVelocity(casacore::Quantity(2001.,"km/s")).
  // The reference type and velocity definition are specified separately.
  // Note that no averaging is performed, the visibilities will be interpolated
  // and sampled at the specified velocities, it's up to you to choose a vInc
  // appropriate to the channel width.
  // The REST_FREQUENCY column in the SPECTRAL_WINDOW subtable is used to
  // determine the velocity-frequency conversion.
  // By default calculations are done for a single velocity with offsets 
  // applied for the others (ok for non-rel velocities with RADIO defn), 
  // set precise to true to do a full conversion for each output channel.(NYI)
  ROVisibilityIterator& 
  selectVelocity(casacore::Int nChan, 
		 const casacore::MVRadialVelocity& vStart, const casacore::MVRadialVelocity& vInc,
		 casacore::MRadialVelocity::Types rvType = casacore::MRadialVelocity::LSR,
		 casacore::MDoppler::Types dType = casacore::MDoppler::RADIO, casacore::Bool precise=false);
  // Select the velocity interpolation scheme.
  // At present the choice is limited to : nearest and linear, linear
  // is the default. 
  // TODO: add cubic, spline and possibly FFT
  ROVisibilityIterator& velInterpolation(const casacore::String& type);
  // Channel selection - only the selected channels will be returned by the
  // access functions. The default spectralWindow is the current one (or 0)
  // This allows selection of the input channels, producing
  // nGroup groups of width output channels. Default is to return all channels
  // in a single group.
  ROVisibilityIterator& selectChannel(casacore::Int nGroup=1, casacore::Int start=0, casacore::Int width=0, 
				      casacore::Int increment=1, casacore::Int spectralWindow=-1);
  //Same as above except when multiple ms's are to be accessed
  ROVisibilityIterator& selectChannel(casacore::Block< casacore::Vector<casacore::Int> >& blockNGroup,
				      casacore::Block< casacore::Vector<casacore::Int> >& blockStart,
				      casacore::Block< casacore::Vector<casacore::Int> >& blockWidth,
				      casacore::Block< casacore::Vector<casacore::Int> >& blockIncr,
				      casacore::Block< casacore::Vector<casacore::Int> >& blockSpw);
  //get the channel selection ...the block over the number of ms's associated
  // with this iterator
  void getChannelSelection(casacore::Block< casacore::Vector<casacore::Int> >& blockNGroup,
			   casacore::Block< casacore::Vector<casacore::Int> >& blockStart,
			   casacore::Block< casacore::Vector<casacore::Int> >& blockWidth,
			   casacore::Block< casacore::Vector<casacore::Int> >& blockIncr,
			   casacore::Block< casacore::Vector<casacore::Int> >& blockSpw);
  // Translate slicesv from the form returned by casacore::MSSelection::getChanSlices()
  // to matv as used by setChanAveBounds().  widthsv is the channel averaging
  // width for each _selected_ spw.
  void slicesToMatrices(casacore::Vector<casacore::Matrix<casacore::Int> >& matv,
                        const casacore::Vector<casacore::Vector<casacore::Slice> >& slicesv,
                        const casacore::Vector<casacore::Int>& widthsv) const;
  // Get the spw, start  and nchan for all the ms's is this Visiter that 
  // match the frequecy "freqstart-freqStep" and "freqEnd+freqStep" range
  // Can help in doing channel selection above..
  // freqFrame is the frame the caller frequency values are in (freqStart and freqEnd)
  // These will be converted to the frame of the selected spw to match
  void getSpwInFreqRange(casacore::Block<casacore::Vector<casacore::Int> >& spw, 
				 casacore::Block<casacore::Vector<casacore::Int> >& start, 
				 casacore::Block<casacore::Vector<casacore::Int> >& nchan, 
				 casacore::Double freqStart, casacore::Double freqEnd, 
				 casacore::Double freqStep, casacore::MFrequency::Types freqFrame=casacore::MFrequency::LSRK);

  //Get the frequency range of the data selection
  void getFreqInSpwRange(casacore::Double& freqStart, casacore::Double& freqEnd, casacore::MFrequency::Types freqframe = casacore::MFrequency::LSRK);

  // Attach a VisBuffer object.
  // Note that while more than one VisBuffer may be attached, only the
  // last one is actively updated. A casacore::Stack is kept internally, so after 
  // a detach, the previous VisBuffer becomes active again.
  void attachVisBuffer(VisBuffer& vb);
  // Detach a VisBuffer object.
  // If the object detached is not the last one attached an exception
  // is thrown.
  void detachVisBuffer(VisBuffer& vb);
  // Access the current casacore::MSColumns object in MSIter
  const casacore::MSColumns& msColumns() const;
  // get back the selected spectral windows and spectral channels for
  // current ms 
  void allSelectedSpectralWindows(casacore::Vector<casacore::Int>& spws, casacore::Vector<casacore::Int>& nvischan);
  // Convert the frequency from the observe frame to lsr frame.
  // Returns true in convert if given spw was not observed 
  // in the LSRK frame
  //when ignoreconv=true...no conversion is done from data frame
  //frequency is served as is
  void lsrFrequency(const casacore::Int& spw, casacore::Vector<casacore::Double>& freq, casacore::Bool& convert, const  casacore::Bool ignoreconv=false);
  //assign a VisImagingWeight object to this iterator
  void useImagingWeight(const VisImagingWeight& imWgt);
  const VisImagingWeight & getImagingWeightGenerator () const;

  //return number  of Ant 
  casacore::Int numberAnt();
  //Return number of rows in all selected ms's
  casacore::Int numberCoh();
  // Return number of spws, polids, ddids
  casacore::Int numberSpw();
  casacore::Int numberPol();
  casacore::Int numberDDId();
//  casacore::ArrayColumn <casacore::Double> & getChannelFrequency () const;
//  casacore::Block<casacore::Int> getChannelGroupNumber () const;
//  casacore::Block<casacore::Int> getChannelIncrement () const;
//  casacore::Block<casacore::Int> getChannelStart () const;
//  casacore::Block<casacore::Int> getChannelWidth () const;
  casacore::Int getDataDescriptionId () const;
  const casacore::MeasurementSet & getMeasurementSet() const;;
  casacore::Int getMeasurementSetId() const;
  casacore::Int getNAntennas () const;
  casacore::MEpoch getEpoch () const;
  ///////////////casacore::MFrequency::Types getObservatoryFrequencyType () const; //???
  ///////////////casacore::MPosition getObservatoryPosition () const;
  ///////////////casacore::MDirection getPhaseCenter () const;
  casacore::Vector<casacore::Float> getReceptor0Angle ();
  casacore::Vector<casacore::rownr_t> getRowIds () const;

  // Returns the columns that the VisibilityIterator is sorting by.
  const casacore::Block<casacore::Int>& getSortColumns() const;

  static void lsrFrequency (const casacore::Int& spw,
                            casacore::Vector<casacore::Double>& freq,
                            casacore::Bool & convert,
                            const casacore::Block<casacore::Int> & chanStart,
                            const casacore::Block<casacore::Int> & chanWidth,
                            const casacore::Block<casacore::Int> & chanInc,
                            const casacore::Block<casacore::Int> & numChanGroup,
                            const casacore::ArrayColumn <casacore::Double> & chanFreqs,
                            const casacore::ScalarColumn<casacore::Int> & obsMFreqTypes,
                            const casacore::MEpoch & ep,
                            const casacore::MPosition & obsPos,
                            const casacore::MDirection & dir);

  static casacore::String getAipsRcBase () { return "VisibilityIterator";}
  static casacore::String getAsyncRcBase () { return getAipsRcBase() + ".async";}

protected:

  ROVisibilityIterator (const asyncio::PrefetchColumns * prefetchColumns,
                        const casacore::Block<casacore::MeasurementSet>& mss,
                        const casacore::Block<casacore::Int>& sortColumns,
                        const casacore::Bool addDefaultSortCols,
                        casacore::Double timeInterval,
                        casacore::Bool writable);

  void construct (const asyncio::PrefetchColumns * prefetchColumns,
                  const casacore::Block<casacore::MeasurementSet>& mss,
                  const casacore::Block<casacore::Int>& sortColumns,
                  const casacore::Bool addDefaultSortCols,
                  casacore::Double timeInterval,
                  casacore::Bool writable,
                  const Factory & factory);

  VisibilityIteratorReadImpl * getReadImpl() const;

  bool existsColumn (VisBufferComponents::EnumType id) const;
  // advance the iteration
  virtual void advance();
  // set the currently selected table
  virtual void setSelTable();
  // set the iteration state
  void setState();
  // get the TOPO frequencies from the selected velocities and the obs. vel.
  void getTopoFreqs();
  void getTopoFreqs(casacore::Vector<casacore::Double> & lsrFreq, casacore::Vector<casacore::Double> & selFreq); // for async i/o
  void getLsrInfo (casacore::Block<casacore::Int> & channelGroupNumber,
                           casacore::Block<casacore::Int> & channelIncrement,
                           casacore::Block<casacore::Int> & channelStart,
                           casacore::Block<casacore::Int> & channelWidth,
                           casacore::MPosition & observatoryPositon,
                           casacore::MDirection & phaseCenter,
                           casacore::Bool & velocitySelection) const;
  std::vector<casacore::MeasurementSet> getMeasurementSets () const;
  const casacore::MSDerivedValues & getMSD () const; // for use by Async I/O *ONLY*
  // update the DATA slicer
  virtual void updateSlicer();
  // attach the column objects to the currently selected table
  virtual void attachColumns(const casacore::Table &t);
  // returns the table, to which columns are attached, 
  // can be overridden in derived classes
  virtual const casacore::Table attachTable() const;
  // get the (velocity selected) interpolated visibilities, flags and weights.
  // It is not really const at all (it seems to use This-> trickery so callers
  // like flag() can be declared const).
//  void getInterpolatedVisFlagWeight(DataColumn whichOne) const;
  // get the (velocity selected) interpolated FLOAT_DATA (as real Floats),
  // flags and weights.
//  void getInterpolatedFloatDataFlagWeight() const;
  // get the visibility data (observed, corrected or model);
  // deals with casacore::Float and casacore::Complex observed data (DATA or FLOAT_DATA)
  void getDataColumn(DataColumn whichOne, const casacore::Slicer& slicer, 
			     casacore::Cube<casacore::Complex>& data) const;
  void getDataColumn(DataColumn whichOne, casacore::Cube<casacore::Complex>& data) const;
  // get FLOAT_DATA as real Floats.
  void getFloatDataColumn(const casacore::Slicer& slicer, casacore::Cube<casacore::Float>& data) const;
  void getFloatDataColumn(casacore::Cube<casacore::Float>& data) const;

  void originChunks(casacore::Bool forceRewind);
  //Re-Do the channel selection in multi ms case 
  void doChannelSelection();
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
  virtual void setTileCache();
  //Check if spw is in selected SPW for actual ms
  casacore::Bool isInSelectedSPW(const casacore::Int& spw);
  // Updates, if necessary, rowIds_p member for the current chunk
  void update_rowIds() const;
  void setAsyncEnabled (casacore::Bool enable);
  template<class T>
    void getColScalar(const casacore::ScalarColumn<T> &column, casacore::Vector<T> &array, casacore::Bool resize) const;
  template<class T>
    void getColArray(const casacore::ArrayColumn<T> &column, casacore::Array<T> &array, casacore::Bool resize) const;
  // column access functions, can be overridden in derived classes
  virtual void getCol(const casacore::ScalarColumn<casacore::Bool> &column, casacore::Vector<casacore::Bool> &array, casacore::Bool resize = false) const;
  virtual void getCol(const casacore::ScalarColumn<casacore::Int> &column, casacore::Vector<casacore::Int> &array, casacore::Bool resize = false) const;
  virtual void getCol(const casacore::ScalarColumn<casacore::Double> &column, casacore::Vector<casacore::Double> &array, casacore::Bool resize = false) const;
  virtual void getCol(const casacore::ArrayColumn<casacore::Bool> &column, casacore::Array<casacore::Bool> &array, casacore::Bool resize = false) const;
  virtual void getCol(const casacore::ArrayColumn<casacore::Float> &column, casacore::Array<casacore::Float> &array, casacore::Bool resize = false) const;
  virtual void getCol(const casacore::ArrayColumn<casacore::Double> &column, casacore::Array<casacore::Double> &array, casacore::Bool resize = false) const;
  virtual void getCol(const casacore::ArrayColumn<casacore::Complex> &column, casacore::Array<casacore::Complex> &array, casacore::Bool resize = false) const;
  virtual void getCol(const casacore::ArrayColumn<casacore::Bool> &column, const casacore::Slicer &slicer, casacore::Array<casacore::Bool> &array, casacore::Bool resize = false) const;
  virtual void getCol(const casacore::ArrayColumn<casacore::Float> &column, const casacore::Slicer &slicer, casacore::Array<casacore::Float> &array, casacore::Bool resize = false) const;
  virtual void getCol(const casacore::ArrayColumn<casacore::Complex> &column, const casacore::Slicer &slicer, casacore::Array<casacore::Complex> &array, casacore::Bool resize = false) const;
  //  void getCol(const casacore::String &colName, casacore::Array<casacore::Double> &array,
  //                      casacore::Array<casacore::Double> &all, casacore::Bool resize = false) const;
  //  void getCol(const casacore::String &colName, casacore::Vector<casacore::Bool> &array,
  //                      casacore::Vector<casacore::Bool> &all, casacore::Bool resize = false) const;
  //  void getCol(const casacore::String &colName, casacore::Vector<casacore::Int> &array,
  //                      casacore::Vector<casacore::Int> &all, casacore::Bool resize = false) const;
  //  void getCol(const casacore::String &colName, casacore::Vector<casacore::Double> &array,
  //                      casacore::Vector<casacore::Double> &all, casacore::Bool resize = false) const;
  template<class T>
  void swapyz(casacore::Cube<T>& out, const casacore::Cube<T>& in) const;

  VisibilityIteratorReadImpl * readImpl_p;
};


// <summary>
// VisibilityIterator iterates through one or more writable MeasurementSets
// </summary>
// <use visibility=export>
// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>
// <prerequisite>
//   <li> <linkto class="ROVisibilityIterator">ROVisibilityIterator</linkto>
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
// <linkto class="ROVisibilityIterator">ROVisibilityIterator</linkto>.
//
// One should use <linkto class="VisBuffer">VisBuffer</linkto>
// to access chunks of data.
//
// As with an ROVI, a VI can be created to only utilize synchronous I/O
// (the "normal" way) or it can potentially use asynchronous I/O.  To create
// an asynchronous instance of VI requires using an appropriate constructor
// (i.e., one that has PrefetchColumns * as its first argument) and setting
// the CASARC setting "VisibilityIterator.async.disabled: false".
//
// When a write operation is performed using the various setX methods of an
// asynchronous instance of this class, the data is shipped over to the VLAT
// which writes out when it is not in the middle of performing a read ahead
// of a VisBuffer.
//
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
class VisibilityIterator : public ROVisibilityIterator
{

    friend class VisibilityIteratorWriteImpl;

public:
  // Constructors.
  // Note: The VisibilityIterator is not initialized correctly by default, you
  // need to call origin() before using it to iterate.
  VisibilityIterator();
  VisibilityIterator(casacore::MeasurementSet & ms, const casacore::Block<casacore::Int>& sortColumns, 
       casacore::Double timeInterval=0);
  VisibilityIterator(casacore::MeasurementSet & ms, const casacore::Block<casacore::Int>& sortColumns, 
		     const casacore::Bool addDefaultSortCols,
		     casacore::Double timeInterval=0);
  // Same as previous constructor, but with multiple MSs to iterate over.
  VisibilityIterator(const casacore::Block<casacore::MeasurementSet>& mss,
		       const casacore::Block<casacore::Int>& sortColumns, 
		       casacore::Double timeInterval=0);
  VisibilityIterator(const casacore::Block<casacore::MeasurementSet>& mss,
		     const casacore::Block<casacore::Int>& sortColumns, const casacore::Bool addDefaultSortCols, 
		       casacore::Double timeInterval=0);
  VisibilityIterator (const asyncio::PrefetchColumns * prefetchColumns,
                      const casacore::Block<casacore::MeasurementSet>& mss,
                      const casacore::Block<casacore::Int>& sortColumns,
                      const casacore::Bool addDefaultSortCols = true,
                      casacore::Double timeInterval = 0);
  VisibilityIterator (const asyncio::PrefetchColumns * prefetchColumns,
                      casacore::MeasurementSet & mss,
                      const casacore::Block<casacore::Int>& sortColumns,
                      const casacore::Bool addDefaultSortCols = true,
                      casacore::Double timeInterval = 0);
  VisibilityIterator(const VisibilityIterator & MSI);
  VisibilityIterator(const asyncio::PrefetchColumns * prefetchColumns, const VisibilityIterator & other);

  
  virtual ~VisibilityIterator();

  VisibilityIterator & operator=(const VisibilityIterator &MSI);


  // Members
  
  casacore::Bool isWritable () const;

  // Advance iterator through data
  VisibilityIterator & operator++(int);
  VisibilityIterator & operator++();
  // Set/modify the flags in the data.
  // This will flag all channels in the original data that contributed to
  // the output channel in the case of channel averaging.
  // All polarizations have the same flag value.
  void setFlag(const casacore::Matrix<casacore::Bool>& flag);
  // Set/modify the flags in the data.
  // This sets the flags as found in the casacore::MS, casacore::Cube(npol,nchan,nrow),
  // where nrow is the number of rows in the current iteration (given by
  // nRow()).
  virtual void setFlag(const casacore::Cube<casacore::Bool>& flag);
  // Set/modify the flag row column; dimension casacore::Vector(nrow)
  virtual void setFlagRow(const casacore::Vector<casacore::Bool>& rowflags);

  void setFlagCategory(const casacore::Array<casacore::Bool>& fc);

  // Set/modify the visibilities.
  // This is possibly only for a 'reference' casacore::MS which has a new DATA column.
  // The first axis of the matrix should equal the selected number of channels
  // in the original MS.
  // If the casacore::MS does not contain all polarizations, only the parallel
  // hand polarizations are used.
  void setVis(const casacore::Matrix<CStokesVector>& vis, DataColumn whichOne);
  // Set/modify the visibilities
  // This sets the data as found in the casacore::MS, casacore::Cube(npol,nchan,nrow).
  virtual void setVis(const casacore::Cube<casacore::Complex>& vis, DataColumn whichOne);
  // Set the visibility and flags, and interpolate from velocities if needed
  virtual void setVisAndFlag(const casacore::Cube<casacore::Complex>& vis, const casacore::Cube<casacore::Bool>& flag,
	   	             DataColumn whichOne);
  // Set/modify the weights
  void setWeight(const casacore::Vector<casacore::Float>& wt);
  // Set/modify the weightMat
  virtual void setWeightMat(const casacore::Matrix<casacore::Float>& wtmat);
  // Set/modify the weightSpectrum
  virtual void setWeightSpectrum(const casacore::Cube<casacore::Float>& wtsp);
  // Set/modify the Sigma
  void setSigma(const casacore::Vector<casacore::Float>& sig);
  // Set/modify the ncorr x nrow SigmaMat.
  void setSigmaMat(const casacore::Matrix<casacore::Float>& sigmat);
  
  //This puts a model into the descriptor of the current ms in the iterator
  //Set iscomponentlist to true if the record represent a componentlist
  //if false then it is a FTMachine casacore::Record that holds the model image 
  //note the spw and fields selected are going to be associated with this model
  //incremetal =true implies add the model to previous any existant model 
  //in the ms for the spw and fields 
  //false means any existant model will be replaces.
  void putModel(const casacore::RecordInterface& rec, casacore::Bool iscomponentlist=true, casacore::Bool incremental=false);


  void writeBack (VisBuffer *);

protected:

  virtual void attachColumns(const casacore::Table &t);

  void construct ();


  VisibilityIteratorWriteImpl * getWriteImpl() const;

  void initializeBackWriters ();

  VisibilityIteratorWriteImpl * writeImpl_p;
};

} //# NAMESPACE CASA - END

#ifndef AIPS_NO_TEMPLATE_SRC
#include <msvis/MSVis/VisibilityIterator.tcc>
#endif //# AIPS_NO_TEMPLATE_SRC

#endif
