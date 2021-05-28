//# VisChunkAverager.h: class to time average all columns of a VisBuffer
//# Copyright (C) 2010
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

#ifndef MSVIS_VISCHUNKAVERAGER_H
#define MSVIS_VISCHUNKAVERAGER_H

#include <casa/aips.h>
#include <msvis/MSVis/CalVisBuffer.h>
#include <map>
#include <vector>

namespace casa { //# NAMESPACE CASA - BEGIN

class CalVisBuffer;
class VisBuffer;

// <summary>
// A class to time average all columns of a VisBuffer.
// </summary>
//
// <use visibility=export>
//
// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>

// <prerequisite>
//   <li> VisBuffer
// </prerequisite>
//
// <etymology>
// From "VisBuffer", "Chunk", and "averaging".  "Chunk" comes from
// VisibilityIterator.
// </etymology>
//
// <synopsis>
// This class time averages complete rows of a VisBuffer.
// </synopsis>
//
// <example>
// See SubMS::doTimeAver().
// </example>
//
// <motivation>
// VisBuffAccumulator also time averages VisBuffers, but only for a few
// columns, as needed by calibration and plotms.  casacore::Time averaging in split
// requires (in principle) that all of the columns in the input casacore::MS be written
// to the output MS.  This is more work than required by calibration and
// plotms, and split also has some differences in the details of the averaging.
// </motivation>
//
// <note>
// For many of the columns averaging is trivial, i.e. the VisBuffer should only
// contain one ARRAY_ID, FIELD_ID, and DATA_DESC_ID, so they will all only have
// one value each.
//
// TIME and INTERVAL will also be univalued in the output, for a different
// reason.  For most (all?) uses this is a feature.  (See CAS-2422 + 2439 for
// why.)
// </note>
//
// <todo asof="2010/11/12">
//   <li> averaging over other indices.
// </todo>

typedef std::map<casacore::uInt, std::vector<casacore::Int> > mapuIvIType;

class VisChunkAverager //: public VisBuffAccumulator
{
public:
  // Construct from which *DATA column(s) to read and whether or not to use
  // WEIGHT_SPECTRUM.
  VisChunkAverager(const casacore::Vector<casacore::MS::PredefinedColumns>& dataCols,
                   const casacore::Bool doSpWeight,
                   const casacore::Vector<casacore::Matrix<casacore::Int> >& chBounds=casacore::Vector<casacore::Matrix<casacore::Int> >());

  // Null destructor
  ~VisChunkAverager();

  // Reset the averager
  void reset();

  // casacore::Time average vi's current chunk, AND advance vi to the end of that chunk.
  // casacore::Input:
  //    vi              ROVisibilityIterator  vi.setRowBlocking(0) will be
  //                                          called to ensure that ++vi
  //                                          advances by 1 integration at a
  //                                          time.
  // Output from private data:
  //    avBuf_p         CalVisBuffer          Output buffer for the averaged 
  //                                          "integration".
  VisBuffer& average(ROVisibilityIterator& vi);

  // Checks whether the interval of vi needs to be truncated in order to
  // prevent collisions where two rows have the same casacore::MS key (ignoring TIME) but
  // different SCAN_NUMBER, STATE_ID, and/or OBSERVATION_ID.  ARRAY_ID is
  // already separated by the chunking.
  //
  // time_to_break is set to the TIME of the earliest collision, or the end of
  // the buffer if none are found.
  //
  static casacore::Bool check_chunk(ROVisibilityIterator& vi, casacore::Double& time_to_break,
                          const casacore::Bool watch_obs, const casacore::Bool watch_scan,
                          const casacore::Bool watch_state);


  // max(a, b) in Math.h is so overloaded, and (u)casacore::Int is so promotable, that
  // they're unusable together.
  casacore::uInt uIntMax(const casacore::uInt a, const casacore::uInt b) const { return a > b ? a : b; }

private:
  // Prohibit null constructor, copy constructor and assignment for now
  VisChunkAverager();
  VisChunkAverager& operator= (const VisChunkAverager&);
  VisChunkAverager (const VisChunkAverager&);

  // Initialize the next accumulation interval
  void initialize(VisBuffer& vb);

  // Normalize the accumulation (finish the average).
  void normalize(const casacore::Double minTime, const casacore::Double maxTime,
                 const casacore::Double firstinterval, const casacore::Double lastinterval);

  // Force vb to read all the columns (modified by colEnums_p and
  // doSpWeight_p).
  //
  // Sets readyToHash_p to false.
  //
  void fill_vb(VisBuffer& vb);

  // Hash function to return a unique (within the VisBuffer) key (as defined by
  // the casacore::MS def'n) for an interferometer (ant1, ant2, feed1, feed2,
  // processor_id).  Note that a VisBuffer only contains one ddid and fieldid,
  // and TIME is deliberately excluded this that is what will be averaged over.
  //
  // Sorting a set of the returned keys is equivalent to sorting by
  // (procid, ant1, ant2, feed1, feed2).
  //
  casacore::uInt hashFunction(const casacore::Int ant1, const casacore::Int ant2,
                    const casacore::Int feed1, const casacore::Int feed2,
                    const casacore::Int procid) const;

  // These return their success values.  (They could fail if any of ant1, ant2,
  // feed1, feed2, or procid are < 0, but that shouldn't happen and isn't
  // checked for.)
  casacore::Bool setupHashFunction(ROVisibilityIterator& vi);
  casacore::Bool makeHashMap(ROVisibilityIterator& vi);

  // Check whether any of the unflagged rows in vi's current chunk have the
  // same casacore::MS key (not counting TIME) but different values for the columns in
  // colsToWatch.
  //
  // time_to_break will be set to the time of the earliest collision if any are
  // found, or the last TIME in vi's current chunk otherwise (assumes vi's
  // current chunk is ascendingly sorted in TIME).
  //
  // colsToWatch should contain casacore::MS::SCAN_NUMBER, casacore::MS::STATE_ID,
  // casacore::MS::OBSERVATION_ID, and/or nothing.  Otherwise an casacore::AipsError will be
  // thrown.  casacore::MS::ARRAY_ID is already separated by the chunking.
  //
  casacore::Bool findCollision(ROVisibilityIterator& vi, casacore::Double& time_to_break,
                     const casacore::Bool watchObs, const casacore::Bool watchScan,
                     const casacore::Bool watchState);

  // Helper function for findCollision().
  casacore::Bool checkForBreak(casacore::Vector<casacore::Int>& firstVals, const casacore::Int i, const casacore::uInt slotnum,
                     const casacore::uInt chunkletNum,
                     const std::vector<casacore::Int>& inrows_for_slot) const;

  // Start of initialization list.
  // Which of DATA, MODEL_DATA, and/or CORRECTED_DATA to use.
  casacore::Vector<casacore::MS::PredefinedColumns> colEnums_p;

  // Use WEIGHT_SPECTRUM?
  casacore::Bool doSpWeight_p;

  // Used for both selecting and averaging channels.
  casacore::Vector<casacore::Matrix<casacore::Int> > chanAveBounds_p;
  // End of initialization list.

  // Is everything setup for hashFunction()?
  casacore::Bool readyToHash_p;

  // Is sphash_to_inprows_p OK?
  casacore::Bool haveHashMap_p;

  // Maxima for hashFunction().
  casacore::uInt maxant1p1_p;
  casacore::uInt maxant2p1_p;
  casacore::uInt maxfeed1p1_p;
  casacore::uInt maxfeed2p1_p;
  // casacore::uInt maxprocp1_p; Not needed

  // A map from a sparse hash of "baseline" to a vector of input row numbers
  // matching that hash.
  //
  // The hash is calculated by hashFunction().
  //
  // The vector has an entry, in order, for each chunklet (vb) in vi's current
  // chunk which is either -1 (the chunklet has no row matching the hash) or
  // the row number _relative_ to the starting row in that chunklet's vb.
  //
  mapuIvIType sphash_to_inprows_p;

  // Number of correlations and channels
  casacore::Int nCorr_p, nChan_p;

  // The number of flag categories.  0 if flagCategory() is invalid.
  casacore::Int nCat_p;

  // Averaging buffer
  CalVisBuffer avBuf_p;
};


} //# NAMESPACE CASA - END

#endif


