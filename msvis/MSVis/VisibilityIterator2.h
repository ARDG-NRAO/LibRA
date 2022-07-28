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

#ifndef MSVIS_VISIBILITYITERATOR2_H
#define MSVIS_VISIBILITYITERATOR2_H

#include <casacore/casa/aips.h>

#include <casacore/ms/MeasurementSets/MSIter.h>
#include <casacore/ms/MeasurementSets/MeasurementSet.h>
#include <msvis/MSVis/VisBuffer2.h>
#include <msvis/MSVis/VisBufferComponents2.h>
#include <msvis/MSVis/ViiLayerFactory.h>

#include <map>
#include <set>
#include <utility>
#include <vector>

namespace casacore{

class MDirection;
class MSDerivedValues;
class MSIter;
class RecordInterface;
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
template <typename T, Int N> class RigidVector;
template <typename T, Int N> class SquareMatrix;
}

namespace casa { //# NAMESPACE CASA - BEGIN

//# forward decl

//class CStokesVector;
class VisBufferType;
class VisBufferOptions;
class VisImagingWeight;

namespace asyncio {

class VLAT;

} // end namespace asyncio

// Forward declarations outside namespace vi
class MSTransformIteratorFactory;

namespace vi {

class FrequencySelection;
class FrequencySelections;
class VisBuffer2;
class VisBufferComponents2;
class VisBufferImpl2;
class ViImplementation2;

class SubtableColumns {

public:

    // Simple wrapper class to limit access to only the columns associated with the
    // current casacore::MS's subtables.  This prevents misuse of the main table data columns
    // which are provided as part of the casacore::MSColumns object returned by
    // MSIter::msColumns.

    SubtableColumns (casacore::CountedPtr <casacore::MSIter> msIter);

    const casacore::MSAntennaColumns& antenna() const;
    const casacore::MSDataDescColumns& dataDescription() const;
    const casacore::MSFeedColumns& feed() const;
    const casacore::MSFieldColumns& field() const;
    const casacore::MSFlagCmdColumns& flagCmd() const;
    const casacore::MSHistoryColumns& history() const;
    const casacore::MSObservationColumns& observation() const;
    const casacore::MSPointingColumns& pointing() const;
    const casacore::MSPolarizationColumns& polarization() const;
    const casacore::MSProcessorColumns& processor() const;
    const casacore::MSSpWindowColumns& spectralWindow() const;
    const casacore::MSStateColumns& state() const;
    const casacore::MSDopplerColumns& doppler() const;
    const casacore::MSFreqOffsetColumns& freqOffset() const;
    const casacore::MSSourceColumns& source() const;
    const casacore::MSSysCalColumns& sysCal() const;
    const casacore::MSWeatherColumns& weather() const;

private:

    casacore::CountedPtr <casacore::MSIter> msIter_p;
};

///////////////////////////////////////////////////////////////////
//
//    Subchunk class
//
//    Provides the ordered pair identifying the current subchunk.  The first
//    component is the chunk (goes from 0 to nChunks-1 during the outer loop
//    iteration).  The second component is the subchunk number which is
//    zero after VisibilityIterator::origin is called and is increment
//    until ROVI::more returns false.

class Subchunk : public std::pair<casacore::Int, casacore::Int>{

public:

    // First component is Chunk and second is Subchunk

    Subchunk () { resetToOrigin ();}
    Subchunk (casacore::Int a , casacore::Int b) : std::pair<casacore::Int,casacore::Int> (a,b) {}

    casacore::Bool operator== (const Subchunk & other){
        return first == other.first && second == other.second;
    }

    casacore::Bool operator< (const Subchunk & other){
        return first < other.first ||
               (first == other.first && second < other.second);
    }

    casacore::Bool atOrigin () const { return * this == Subchunk ();}
    casacore::Int chunk () const { return first;}
    void incrementSubChunk () { second ++;}
    void incrementChunk () { first ++; second = 0; }

    void resetSubChunk () { second = 0;} // position to start of chunk

    void resetToOrigin () { first = 0; second = 0; }
       // Make a subchunk pair that is positioned to the first subchunk of the
       // first chunk (i.e., 0,0)

    casacore::Int subchunk () const { return second;}
    casacore::String toString () const;

    static Subchunk noMoreData ();

private:

};



///////////////////////////////////////////////////////////
//
// Code to provide interface to weight function
//
// WeightScaling is essentially the interface to the function
// used for weight scaling while WeightScalingImpl is the class
// to actually use to create WeightScaling functors.  The use of
// a templated implementation allows the wrapping of both functors
// and functions while the base class allows polymorphic storage.

class WeightScaling {
public:

    virtual ~WeightScaling () {}
    casacore::Float operator() (casacore::Float x) { return apply (x);}

    static casacore::CountedPtr<WeightScaling> generateUnityWeightScaling ();
    static casacore::CountedPtr<WeightScaling> generateIdentityWeightScaling ();
    static casacore::CountedPtr<WeightScaling> generateSquareWeightScaling ();

protected:

    virtual casacore::Float apply (casacore::Float) = 0;
    static casacore::Float unity (casacore::Float);
    static casacore::Float identity (casacore::Float x);
    static casacore::Float square (casacore::Float x);
};

template<typename F>
class WeightScalingImpl : public WeightScaling {
public:

    // Provide either a unary function, casacore::Float (*) (casacore::Float), or
    // a functor class having a casacore::Float operator() (casacore::Float) method.

    WeightScalingImpl (F f) : function_p (f) {}

    casacore::Float apply (casacore::Float f) { return function_p (f);}

private:

    F function_p;
};


template<typename F>
casacore::CountedPtr <WeightScaling> generateWeightScaling (F f) { return new WeightScalingImpl<F> (f);}

class SortColumns {
public:

    explicit SortColumns (const casacore::Block<casacore::Int> & columnIds = casacore::Block<casacore::Int> (), casacore::Bool addDefaultColumns = true);

    explicit SortColumns (casacore::Bool usingDefaultSortingFunctions);

    // Constructor from a list of (column Ids, comparison function)
    // The column Ids are actually MSMainEnums enums.
    explicit SortColumns (const std::vector<std::pair<casacore::MS::PredefinedColumns, casacore::CountedPtr<casacore::BaseCompare>>> sortingDefinition);

    // Constructor from a list of (column names, comparison function)
    // The column names could in principle be any column name which must
    // of course exist in the MS.
    explicit SortColumns (const std::vector<std::pair<casacore::String, casacore::CountedPtr<casacore::BaseCompare>>> sortingDefinition);

    // Add a sorting column to the existing definitions.
    // It will be added at the end, i. e., it will be "running" faster
    // If sortingFunction = nullptr then a unique sorting function
    // for the type of the column will be used (ObjCompare<T>)
    void addSortingColumn(casacore::MS::PredefinedColumns colId,
        casacore::CountedPtr<casacore::BaseCompare> sortingFunction = nullptr);

    // Add a sorting column to the existing definitions.
    // It will be added at the end, i. e., it will be "running" faster
    // If sortingFunction = nullptr then a unique sorting function
    // for the type of the column will be used (ObjCompare<T>)
    void addSortingColumn(casacore::String colName,
        casacore::CountedPtr<casacore::BaseCompare> sortingFunction = nullptr);

    bool usingDefaultSortingFunctions () const;

    casacore::Bool shouldAddDefaultColumns () const;

    const casacore::Block<casacore::Int> & getColumnIds () const;

    // Get the sorting definitions, including the comparison functions
    const std::vector<std::pair<casacore::String, casacore::CountedPtr<casacore::BaseCompare>>> & sortingDefinition() const;

private:

    casacore::Bool addDefaultColumns_p;
    casacore::Block<casacore::Int> columnIds_p;
    std::vector<std::pair<casacore::String, casacore::CountedPtr<casacore::BaseCompare>>> sortingDefinition_p;
    bool usingDefaultSortingFunctions_p;
};

class VisibilityIterator2;

//////////////////////////////////////////////////////////////////////
//
// Class ViFactory
//
// The ViFactory is a class that can be used to initialize the implementation of
// a VisibilityIterator2.  It is passed into VI2's constructor where it creates
// the needed ViImplementation object used by the VI2.  The first example of this
// factory is the AveragingVi2Factory which is used to create a VI2 which will
// return time-averaged data.

class ViFactory {

public:

    virtual ~ViFactory () {}

protected:

    friend class VisibilityIterator2;

    virtual ViImplementation2 * createVi () const = 0;
};

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
//   <li> <linkto class="PrefetchColumns">PrefetchColumns</linkto>
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
//
// VisibilityIterator2s can be either synchronous or asynchronous, depending
// on the constructor used to create them as well as the current value of
// a CASARC file setting.  A synchronous instance is works the same as
// this class ever worked; an asynchronous instance uses a second thread
// (the Visibility Lookahead Thread or VLAT) to fill the VisBuffers in
// advance of their use by the original thread.
//
// To create an asynchronous instance of ROVI you must use one of the two
// constructors which have a pointer to a PrefetchColumns object as the
// first argument.  This object specifies which VisBuffer components should be
// prefetched by the VLAT; accessing components not specified in the PrefetchColumns
// object will result in an exception containing an error message indicating
// that the VisBuffer does not contain the requested column.  In addition
// to using the appropriate constructor, the CASARC file setting
// VisibilityIterator2.async.enabled can be used to turn asynchronous I/O
// off globally; if it's globally enabled then it is still possible for the
// user to choose to enable/disable it on a VI by VI basis.
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
// VisibilityIterator2.async.enabled - Boolean value that enables or disables
//     async I/O.  The default value is currently false (i.e., disabled).
// VisibilityIterator2.async.nBuffers - The number of lookahead buffers.  This
//     defaults to 2.
//
//
// Debug settings
// --------------
//
// VisibilityIterator2.async.doStats: true
// VisibilityIterator2.async.debug.logFile: stderr
// VisibilityIterator2.async.debug.logLevel: 1
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
class VisibilityIterator2
{
    // These classes are members of the VI/VB framework and need extra
    // access to the non-public methods of this class.  Any additions of
    // classes not in this framework is strongly discouraged.
    //
    // Friends of the class should limit themselves to accessing the protected
    // area of this class so that items in the private area can remeain de
    // facto private.

    //friend class VisibilityIteratorImpl2;
//    friend VisBuffer2 * VisBuffer2::factory (VisibilityIterator2 * vi, VisBufferType t,
//                                             VisBufferOptions options);
    friend class VisBuffer2Adapter;
    //friend class VisBufferImpl2;
    //friend class VisBufferState;
    friend class asyncio::VLAT; // allow VI lookahead thread class to access protected
                                // functions VLAT should not access private parts,
                                // especially variables
    friend class casa::MSTransformIteratorFactory;

public:

  VisibilityIterator2( const VisibilityIterator2& ) = delete;
  VisibilityIterator2& operator=( const VisibilityIterator2& ) = delete;
  class Factory { // Interface for implementation creation factory

  public:

      virtual ~Factory () {}

      virtual ViImplementation2 *
      operator() (const VisBufferComponents2 * /*prefetchColumns*/,
                  const casacore::Block<casacore::MeasurementSet>& /*mss*/,
                  const casacore::Block<casacore::Int>& /*sortColumns*/,
                  const casacore::Bool /*addDefaultSortCols*/,
                  casacore::Double /*timeInterval*/) const
      {
          return NULL;
      }
  };

  typedef enum casacore::MSIter::PolFrame PolFrame;

  typedef enum DataColumn {
      Observed=0,  // Observed data
      Model,       // Model data
      Corrected    // Corrected data
  } DataColumn;

  // Construct from an casacore::MS and a casacore::Block of casacore::MS column enums specifying the
  // iteration order.  These can be specified as casacore::MS::ANTENNA1, casacore::MS::ARRAY_ID,
  // etc.; they are defined in MSMainEnums.h.
  // If no order is specified, it uses the default sort
  // order of MSIter, which is not necessarily the raw order of ms!
  // The default ordering is ARRAY_ID, FIELD_ID, DATA_DESC_ID,
  // and TIME, but check MSIter.h to be sure.
  // These columns will be added first if they are not specified.
  //
  // An optional timeInterval (in seconds) can be given to iterate through
  // chunks of time.  The default interval of 0 groups all times together.
  // Every 'chunk' of data contains all data within a certain time interval and
  // with identical values of the other iteration columns (e.g.  DATA_DESC_ID
  // and FIELD_ID).
  //
  // A set of prefetch columns can be specified for asynchronous I/O use.  These
  // roughly correspond to the fields accessible from the VisBuffer.  Specifying
  // prefetch columns will cause an asynchronous VI to be created if asynchronous
  // I/O is globally enabled.  If the user wishes to have application-specific
  // enable/disable of asynchronous I/O then they must implement logic that
  // either either provides prefetch columns (enables) or a null pointer (disables).

  VisibilityIterator2 (const casacore::MeasurementSet& ms,
                       const SortColumns & sortColumns = SortColumns (),
                       casacore::Bool isWritable = false,
                       const VisBufferComponents2 * prefetchColumns = 0,
                       casacore::Double timeInterval = 0);

  VisibilityIterator2 (const casacore::Block<const casacore::MeasurementSet *>& mss,
                       const SortColumns & sortColumns = SortColumns (),
                       casacore::Bool isWritable = false,
                       const VisBufferComponents2 * prefetchColumns = 0,
                       casacore::Double timeInterval = 0);

  VisibilityIterator2 (const ViFactory & factory);
  
  // Creates an iterator from a stack of VI factories
  // <thrown>
  //    <li>  AipsError if the last factory is NULL 
  // </thrown>
  VisibilityIterator2 (const casacore::Vector<ViiLayerFactory*> & factories);

  // Destructor

  virtual ~VisibilityIterator2();

  // Report the the ViImplementation type
  casacore::String ViiType() const;

  ///////////////////////////////////////////////////////////////////
  //
  // BEGIN Experimental Section
  //

  static VisibilityIterator2 * copyingViFactory (const casacore::MeasurementSet & srcMs,
                                                 casacore::MeasurementSet & dstMs);

  //
  // END Experimental Section
  //
  ///////////////////////////////////////////////////////////////////

  ///////////////////////////////////////////////////////////////////
  //
  // Iteration movement methods
  //
  // The typical use case for VisibilityIteration looks like
  //
  // for (vi.originChunks(); vi.moreChunks(); nextChunk()){
  //     for (vi.origin (); vi.more(); vi++){
  //     }
  // }
  //
  // The outer loop is the "chunk" loop and the inner loop is the "subchunk"
  // loop.  A chunk contains all the rows having identical values for the
  // sort columns values except time; time can have an optional interval
  // value specified either in the constructor or via the setInterval
  // method.  If an interval is specified the set of rows will all have
  // timestamps that are within that interval of the first row in the
  // chunk; if no interval is specified then the chunk will contain only
  // rows having the identical timestamp.  If multiple MSs are specified
  // in the constructor then changing from one casacore::MS to the next will occur
  // on a chunk boundary (i.e., a chunk will never contain data from more
  // than one casacore::MS).
  //
  // A subchunk typically contains all the rows in the chunk having identical
  // timestamps.  The only exception is when the user calls setRowBlocking(N)
  // with a positive value.  In this case, the subchunk *may* contain up to
  // N rows; however, this is only a suggestion to the VI.  If there are
  // less than N rows remaining in the chunk then fewer than N rows will be
  // contained in the chunk.  The number of rows in a subchunk will also be
  // less than N if a larger set of rows would create an inconsistently
  // shaped visibility cube; this could happen if the number of channels
  // changes from one timestamp to another or the framed frequency
  // selection selects a different number of channels from one timestamp to
  // the next.
  //
  // origin () - positions VI to the start of the current chunk
  // operator++ - advance VI to next subchunk if it exists
  // more - returns true if the VI is pointing to a valid subchunk
  //
  // originChunks - Move to the first chunk of data.
  // operator++ - advance VI to the next chunk if it exists
  // moreChunks - returns true if the VI is pointing to a valid chunk.

  void origin(); // Reset to start of the chunk
  void next ();
  casacore::Bool more() const;

  void originChunks();
  void nextChunk();
  casacore::Bool moreChunks() const;

  // Report Name of slowest column that changes at end of current chunk iteration
  virtual casacore::String keyChange() const;

  // Returns the pair (chunk,subchunk) for the current position of the VI.  Only
  // valid after origin has been called.

  Subchunk getSubchunkId () const;

  ///////////////////////////////////////////////////////////////////
  //
  // Iterator operation methods
  //
  // These methods alter the way that the VI operates.  After applying any of these
  // (except slurp which should be called after originChunks) originChunks must be
  // called before the VI is advanced again; otherwise an exception will be thrown
  // (changing these values in the middle of a sweep can lead to unexpected behavior).
  //
  // Selecting ranges of frequencies involves initializing a
  // FrequencySelection(s) object and providing it to the VI.  Once
  // properly applied the VI will only return data for the selected
  // frequencies.  No interpolation is performed; the data returned is
  // simply the channels that currently map to the selected frequencies for
  // the specified frame of reference.

  void setFrequencySelection (const FrequencySelection &);  // for single MS
  void setFrequencySelection (const FrequencySelections &); // when using multiple MSs

  // RowBlocking is a suggestion to the iterator that it try to use subchunks
  // having the suggested number of rows.  The VI will attempt to honor the
  // suggestion except when there are not enough rows remaining in the chunk
  // or when putting the suggested number of rows in the subchunk cannot
  // be contained in a cube (e.g., there are sets of row with different
  // numbers of frequencies, etc.).  The latter case will mainly occur
  // when the row visibility shape changes from one row to the next.

  casacore::Int getRowBlocking() const;
  void setRowBlocking(casacore::rownr_t nRows=0);

  // In this context the interval determines what rows will be contained
  // in a chunk.  A chunk is all the rows in an casacore::MS that have the same values
  // in the sort columns, except for time; all rows that otherwise have the
  // same values for the sort columns will be in the chunk if they are no
  // later than "interval" seconds after the first row in the chunk.
  //
  // *** This value is unrelated to the interval field of the casacore::MS main table.

  casacore::Double getInterval() const;
  void setInterval(casacore::Double timeInterval);

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

  ///////////////////////////////////////////////////////////////////
  //
  // Other methods
  //

  // Returns true if this is an asynchronous VI

  casacore::Bool isAsynchronous () const;

  // Returns true if async I/O is globally enabled.

  static casacore::Bool isAsynchronousIoEnabled();

  // Returns true if this VI is writable (always false for ROVI and
  // true for VI; see VisibilityIterator class).

  casacore::Bool isWritable () const;

  // Returns the columns that the VisibilityIterator2 is sorting by.  These are
  // defined in MSMainEnums.h.  These can be specified as casacore::MS::ANTENNA1,
  // casacore::MS::ARRAY_ID, etc.

  const SortColumns & getSortColumns() const;

  // Returns true if the specified column exists.

  bool existsColumn (VisBufferComponent2 id) const;

  // Returns the VisBuffer permanently attached to this VisibilityIterator.

  VisBuffer2 * getVisBuffer ();

  // Manages the weight function that can be used to process the weights
  // produced by the "scaled" variants of the weight accessors.  Use
  // generateWeightscaling to create a WeightScaling object.  This allow you
  // to use either a function (FLoat (casacore::Float)) or a functor (object having
  // method casacore::Float operator () (casacore::Float)).
  //
  // To revert to having no scaling function, call setWeightScaling with
  // 0 as the argument.  Any call to setWeightScaling needs to be followed
  // by an originChunks call before any further data access is performed
  // using the VI.
  //
  // The method hasWeightScaling will return false if either no weightScaling
  // object has installed or setWeightScaling (0) was called.  There is not
  // way for VI to know if the user has passed in the identity function;
  // doing so will still cause hasWeightScaling to return true.

  virtual void setWeightScaling (casacore::CountedPtr<WeightScaling> weightscaling);
  virtual casacore::Bool hasWeightScaling () const;

  // Return number of spws, polids, ddids for the current MS

  casacore::Int nSpectralWindows () const;
  casacore::Int nPolarizationIds () const; // number of different polarization configurations
                                 // (i.e., length of polarization subtable)
  casacore::Int nDataDescriptionIds () const;

  // Determine whether WEIGHT_SPECTRUM exists.

  casacore::Bool weightSpectrumExists() const;

  // Determine whether WEIGHT_SPECTRUM exists.

  casacore::Bool sigmaSpectrumExists() const;

  //reference to actual ms in interator

  const casacore::MeasurementSet& ms() const /*__attribute__((deprecated))*/;

  const vi::SubtableColumns & subtableColumns () const /*__attribute__((deprecated))*/;

  static casacore::String getAipsRcBase () { return "VisibilityIterator2";}

  // The reporting frame of reference is the default frame of reference to be
  // used when the user requests the frequencies of the current data selection
  // in a VisBuffer.  This is useful when the user wishes to select the data
  // using one frame of reference but use the frequencies from another frame
  // of reference.  If not specified then the default will be to use the
  // frame of reference used to select the data.
  //
  // These *do not* change the selection in any way.

  casacore::Int getReportingFrameOfReference () const;
  void setReportingFrameOfReference (casacore::Int);

  // Return the numbers of rows in the current chunk

  casacore::rownr_t nRowsInChunk() const;

  // Assign a VisImagingWeight object to this iterator.  This object is used
  // to generate imaging weights.

  void useImagingWeight(const VisImagingWeight& imWgt);
  const VisImagingWeight & getImagingWeightGenerator () const;

  // Write/modify the flags in the data.
  // This will flag all channels in the original data that contributed to
  // the output channel in the case of channel averaging.
  // All polarizations have the same flag value.

  // Write/modify the flags in the data.
  // This writes the flags as found in the casacore::MS, casacore::Cube(npol,nchan,nrow),
  // where nrow is the number of rows in the current iteration (given by
  // nRow()).

  virtual void writeFlag(const casacore::Cube<casacore::Bool>& flag);

  // Write/modify the flag row column; dimension casacore::Vector(nrow)

  virtual void writeFlagRow(const casacore::Vector<casacore::Bool>& rowflags);

  void writeFlagCategory(const casacore::Array<casacore::Bool>& fc);

  // Write/modify the visibilities.
  // This is possibly only for a 'reference' casacore::MS which has a new DATA column.
  // The first axis of the matrix should equal the selected number of channels
  // in the original MS.
  // If the casacore::MS does not contain all polarizations, only the parallel
  // hand polarizations are used.

//  void writeVisCorrected (const casacore::Matrix<CStokesVector>& vis);
//  void writeVisModel (const casacore::Matrix<CStokesVector>& vis);
//  void writeVisObserved (const casacore::Matrix<CStokesVector>& vis);

  // Write/modify the visibilities
  // This writes the data as found in the casacore::MS, casacore::Cube(npol,nchan,nrow).

  void writeVisCorrected (const casacore::Cube <casacore::Complex> & vis);
  void writeVisModel (const casacore::Cube <casacore::Complex> & vis);
  void writeVisObserved (const casacore::Cube <casacore::Complex> & vis);

  // Write/modify the weights

  void writeWeight(const casacore::Matrix<casacore::Float>& wt);

  // Write/modify the weightMat

  //virtual void writeWeightMat(const casacore::Matrix<casacore::Float>& wtmat);

  // Write/modify the weightSpectrum

  virtual void writeWeightSpectrum(const casacore::Cube<casacore::Float>& wtsp);


  // Initialize the weightSpectrum
  virtual void initWeightSpectrum(const casacore::Cube<casacore::Float>& wtsp);

  // Write/modify the Sigma

  void writeSigma(const casacore::Matrix<casacore::Float>& sig);

  // Write/modify the ncorr x nrow SigmaMat.

  //void writeSigmaMat(const casacore::Matrix<casacore::Float>& sigmat);

  // This puts a model into the descriptor of the current ms in the iterator.
  // Set isComponentList to true if the record represents a componentList;
  // if false then it is a FTMachine Record that holds the model image.
  // Note that the spw and fields selected are going to be associated with this model.
  // Setting addToExistingModel to true adds the model to the previous existent model
  // in the ms for the spw and fields; setting it to false means any existing
  // model will be replaced.

  void writeModel(const casacore::RecordInterface & record,
                  casacore::Bool isComponentList = true,
                  casacore::Bool addToExistingModel = false);

  // Requests that the modified VisBuffer2 be written back to the visibility
  // at the same spot that it came from.  The dirtyComponents feature of
  // VisBuffer is used to mark which portions of the VisBuffer actually need
  // to be written back out.

  void writeBackChanges (VisBuffer2 *);

  //**********************************************************************
  // Methods to access the subtables.
  //**********************************************************************

  // Access to antenna subtable
  const casacore::MSAntennaColumns& antennaSubtablecols() const;

  // Access to MS dataDescription subtable
  const casacore::MSDataDescColumns& dataDescriptionSubtablecols() const;

  // Access to MS feed subtable
  const casacore::MSFeedColumns& feedSubtablecols() const;

  // Access to MS field subtable
  const casacore::MSFieldColumns& fieldSubtablecols() const;

  // Access to MS flagCmd subtable
  const casacore::MSFlagCmdColumns& flagCmdSubtablecols() const;

  // Access to MS history subtable
  const casacore::MSHistoryColumns& historySubtablecols() const;

  // Access to MS observation subtable
  const casacore::MSObservationColumns& observationSubtablecols() const;

  // Access to MS pointing subtable
  const casacore::MSPointingColumns& pointingSubtablecols() const;

  // Access to MS polarization subtable
  const casacore::MSPolarizationColumns& polarizationSubtablecols() const;

  // Access to MS processor subtable
  const casacore::MSProcessorColumns& processorSubtablecols() const;

  // Access to MS spectralWindow subtable
  const casacore::MSSpWindowColumns& spectralWindowSubtablecols() const;

  // Access to MS state subtable
  const casacore::MSStateColumns& stateSubtablecols() const;

  // Access to MS doppler subtable
  const casacore::MSDopplerColumns& dopplerSubtablecols() const;

  // Access to MS freqOffset subtable
  const casacore::MSFreqOffsetColumns& freqOffsetSubtablecols() const;

  // Access to MS source subtable
  const casacore::MSSourceColumns& sourceSubtablecols() const;

  // Access to MS sysCal subtable
  const casacore::MSSysCalColumns& sysCalSubtablecols() const;

  // Access to MS weather subtable
  const casacore::MSWeatherColumns& weatherSubtablecols() const;


//**********************************************************************
// Internal methods below this line
//**********************************************************************

  ViImplementation2 * getImpl() const;

protected:


  VisibilityIterator2();

  void construct (const VisBufferComponents2 * prefetchColumns,
                  const casacore::Block<const casacore::MeasurementSet *>& mss,
                  const SortColumns & sortColumns,
                  casacore::Double timeInterval,
                  casacore::Bool writable);

// advance the iteration

  void originChunks(casacore::Bool forceRewind);


private:

  ViImplementation2 * impl_p;
};

} // end namespace vi

} //# NAMESPACE CASA - END

#endif
