//# CalIntpMatch.cc: Implementation of CalIntpMatch.h
//# Copyright (C) 1996,1997,1998,2001,2002,2003
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
//# $Id$
//----------------------------------------------------------------------------

#include <synthesis/CalTables/CalIntpMatch.h>
#include <msvis/MSVis/MSCalEnums.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

//----------------------------------------------------------------------------

CalIntpMatch::CalIntpMatch (const MatchType& matchType, const uInt& indexDim):
  type_p(matchType),
  indexDim_p(indexDim),
  vbIndex_p(0,0),
  cbIndex_p(0),
  matchKeys_p(0),
  vb_p(NULL),
  cb_p(NULL)
{
// Construct from an interpolation match and index dimentsion
// Input:
//    matchType          const MatchType&         Interpolation match type
//    indexDim           const uInt&              Index dimension 
//                                                (1=baseline-based,
//                                                 2=antenna-based)
// Output to private data:
//    type_p             MatchType                Interpolation match type
//    indexDim_p         Int                      Index dimension
//    vbIndex_p          Matrix<Int>              Vis. buffer index    
//    cbIndex_p          Block<Vector<Int>>       Cal. buffer index
//    matchKeys_p        PtrBlock<Record*>        Match keys
//    vb_p               VisBuffer*               Ptr. to current vis. buffer
//    cb_p               CalMainBuffer*           Ptr. to current cal. buffer
};

//----------------------------------------------------------------------------

void CalIntpMatch::copy (const CalIntpMatch& other)
{
// Private utility function to copy member variables
// Input:
//    other              const CalIntpMatch&      Other CalIntpMatch object
// Output to private data:
//    type_p             MatchType                Interpolation match type
//    indexDim_p         Int                      Index dimension
//    vbIndex_p          Matrix<Int>              Vis. buffer index    
//    cbIndex_p          Block<Vector<Int>>       Cal. buffer index
//    matchKeys_p        PtrBlock<Record*>        Match keys
//    vb_p               VisBuffer*               Ptr. to current vis. buffer
//    cb_p               CalMainBuffer*           Ptr. to current cal. buffer

  // Member-wise copy 
  type_p = other.type_p;
  indexDim_p = other.indexDim_p;
  vbIndex_p = other.vbIndex_p;
  cbIndex_p = other.cbIndex_p;
  matchKeys_p = other.matchKeys_p;
  vb_p = other.vb_p;
  cb_p = other.cb_p;
};

//----------------------------------------------------------------------------

void CalIntpMatch::free()
{
// Private function to free the lvalue in assignment or copy construction
// A null operation for this class as pointers are to shared objects.
};

//----------------------------------------------------------------------------

CalIntpMatch::~CalIntpMatch()
{
// Destructor (null)
//
  free();
};

//----------------------------------------------------------------------------

CalIntpMatch::CalIntpMatch (const CalIntpMatch& other)
{
// Copy constructor
// Input:
//    other    const CalIntpMatch&       rvalue CalIntpMatch object
// Output to private data:
//    type_p             MatchType                Interpolation match type
//    indexDim_p         Int                      Index dimension
//    vbIndex_p          Matrix<Int>              Vis. buffer index    
//    cbIndex_p          Block<Vector<Int>>       Cal. buffer index
//    matchKeys_p        PtrBlock<Record*>        Match keys
//    vb_p               VisBuffer*               Ptr. to current vis. buffer
//    cb_p               CalMainBuffer*           Ptr. to current cal. buffer
//
  copy (other);
};

//----------------------------------------------------------------------------

CalIntpMatch& CalIntpMatch::operator= (const CalIntpMatch& other)
{
// Assignment operator
// Input:
//    other              const CalIntpMatch&      rvalue CalIntpMatch object
// Output to private data:
//    type_p             MatchType                Interpolation match type
//    indexDim_p         Int                      Index dimension
//    vbIndex_p          Matrix<Int>              Vis. buffer index    
//    cbIndex_p          Block<Vector<Int>>       Cal. buffer index
//    matchKeys_p        PtrBlock<Record*>        Match keys
//    vb_p               VisBuffer*               Ptr. to current vis. buffer
//    cb_p               CalMainBuffer*           Ptr. to current cal. buffer
//
  // Check for identity
  if (this != &other) {
    // Free the lvalue and assign the rvalue
    free();
    copy (other);
  };
  return *this;
};

//----------------------------------------------------------------------------

void CalIntpMatch::setType (const MatchType& matchType, const uInt& indexDim)
{
// Reset the interpolation match type
// Input:
//    matchType          const MatchType&         Interpolation match type
//    indexDim           const uInt&              Index dimension 
//                                                (1=baseline-based,
//                                                 2=antenna-based)
// Output to private data:
//    type_p             MatchType                Interpolation match type
//    indexDim_p         Int                      Index dimension
//
  // Reset the interpolation match type and index dimension
  type_p = matchType;
  indexDim_p = indexDim;

  // Reset the index cache
  reset();
  return;
};

//----------------------------------------------------------------------------

void CalIntpMatch::reset()
{
// Reset all local index mappings
// Output to private data:
//    vbIndex_p          Matrix<Int>              Vis. buffer index    
//    cbIndex_p          Block<Vector<Int>>       Cal. buffer index
//    matchKeys_p        PtrBlock<Record*>        Match keys
//
  // Reset all index mappings
  resetCalIndex();
  resetVisIndex();
  matchKeys_p.resize(0);

  return;
};

//----------------------------------------------------------------------------

void CalIntpMatch::resetVisIndex()
{
// Reset the local visibility index mapping
// Output to private data:
//    vbIndex_p          Matrix<Int>              Vis. buffer index    
//
  // Reset the visibility index mapping
  if (vb_p) {
    vbIndex_p.resize(vb_p->nRow(), indexDim_p);
    vbIndex_p = 0;
  } else {
    vbIndex_p.resize();
  };

  return;
};

//----------------------------------------------------------------------------

void CalIntpMatch::resetCalIndex()
{
// Reset the local calibration index mapping
// Output to private data:
//    cbIndex_p          Block<Vector<Int>>       Cal. buffer index
//
  // Reset the calibration index mapping
  if (cb_p) {
    cbIndex_p.resize(cb_p->nRow());
    for (Int i=0; i < static_cast<Int>(cbIndex_p.nelements()); i++) {
      cbIndex_p[i] = 0;
    };
  } else {
    cbIndex_p.resize(0);
  };

  return;
};

//----------------------------------------------------------------------------

void CalIntpMatch::setVisBuffer (VisBuffer& vb)
{
// Set the current visibility buffer to be used in interpolation matches
// Input:
//    vb                 VisBuffer&               Visibility buffer
// Output to private data:
//    vbIndex_p          Matrix<Int>              Vis. buffer index    
//    vb_p               VisBuffer*               Ptr. to current vis. buffer
//
  // Update the visibility buffer pointer
  vb_p = &vb;

  // Reset the visibility buffer index
  resetVisIndex();

  // Refill the visibility buffer index; loop over all vis. buffer rows
  for (Int row=0; row < vb_p->nRow(); row++) {
    // Define the keys for this row
    PtrBlock<Record* > keys(indexDim_p);

    switch (type_p) {
    case MATCHING_ANT_SPW:
      keys[0] = new Record();
      keys[0]->define(MSC::fieldName(MSC::ANTENNA1), vb.antenna1()(row));
      keys[0]->define(MSC::fieldName(MSC::SPECTRAL_WINDOW_ID), 
				     vb.spectralWindow());
      keys[1] = new Record();
      keys[1]->define(MSC::fieldName(MSC::ANTENNA1), vb.antenna2()(row));
      keys[1]->define(MSC::fieldName(MSC::SPECTRAL_WINDOW_ID), 
				     vb.spectralWindow());
      break;

    default:
      break;
    };

    // Match the row keys in the key index and set the
    // visibility index accordingly
    for (Int i=0; i < indexDim_p; i++) {
      vbIndex_p(row,i) = matchOrAddKey (*keys[i]);
    };
  };

  return;
};

//----------------------------------------------------------------------------

void CalIntpMatch::setCalBuffer (CalMainBuffer& cb)
{
// Set the current calibration buffer to be used in interpolation matches
// Input:
//    cb                 CalMainBuffer&           Calibration buffer
// Output to private data:
//    cbIndex_p          Block<Vector<Int>>       Cal. buffer index
//    matchKeys_p        PtrBlock<Record*>        Match keys
//    cb_p               CalMainBuffer*           Ptr. to current cal. buffer
//
  // Update the calibration buffer pointer
  cb_p = &cb;

  // Reset the calibration buffer index
  resetCalIndex();

  // Refill the calibration buffer index; loop over all cal. buffer rows
  for (Int row=0; row < cb_p->nRow(); row++) {
    // Define the keys for this row
    PtrBlock<Record* > keys;

    // Extract cal_desc buffer reference and cal_desc id.
    CalDescBuffer& cdb = cb.calDescBuffer();
    Int calDescId = cb.calDescId()(row);

    switch (type_p) {
    case MATCHING_ANT_SPW: {
      Int numSpw = cdb.numSpw()(calDescId);
      keys.resize(numSpw, true, true);
      for (Int iSpw=0; iSpw < numSpw; iSpw++) {
	keys[iSpw] = new Record();
	keys[iSpw]->define(MSC::fieldName(MSC::ANTENNA1), cb.antenna1()(row));
	keys[iSpw]->define(MSC::fieldName(MSC::SPECTRAL_WINDOW_ID), 
					  cdb.spwId()(calDescId,iSpw));
      };
      break;
    }

    default: {
      break;
    };
    };

    // Match the row keys in the key index and set the
    // calibration index accordingly
    Int nKeys = keys.nelements();
    cbIndex_p[row].resize(nKeys, true);
    for (Int i=0; i < nKeys; i++) {
      cbIndex_p[row](i) = matchOrAddKey (*keys[i]);
    };
  };
  return;
};

//----------------------------------------------------------------------------

Int CalIntpMatch::matchOrAddKey (const Record& key)
{
// Match or add index keys
// Input:
//    key              const Record&            Index key
// Output:
//    matchOrAddKey    Int                      Index key id.
//
  // Search existing keys
  Int nKeys = matchKeys_p.nelements();

  for (Int i=0; i < nKeys; i++) {
    // Match the key entry
    Bool match = true;
    for (uInt j=0; j < key.nfields(); j++) {
      if (matchKeys_p[i]->asInt(j) != key.asInt(j)) {
	match = false;
      };
    };
    if (match) return i;
  };

  // Add a new key entry
  matchKeys_p.resize(nKeys+1, true, true);
  matchKeys_p[nKeys] = new Record(key);
  
  return nKeys;
};

//----------------------------------------------------------------------------

Vector<Int> CalIntpMatch::calRows (const Int& matchIndex, 
				   LogicalArray& calRowMask)
{
// Return the calibration buffer rows (and equivalent row mask)
// for a given match index value
// Input:
//    matchIndex       const Int&               Key index value to match
// Output:
//    calRowMask       LogicalArray&            Matching calibration buffer
//                                              rows (as Boolean mask)
//    calRows          Vector<Int>              Matching calibration buffer
//                                              row numbers
//
  // Initialization
  calRowMask.resize(IPosition(1,cb_p->nRow()));
  calRowMask = false;
  Vector<Int> retval(cb_p->nRow());
  Int nMatch = 0;

  // Iterate through the calibration buffer key index
  for (uInt row=0; row < cbIndex_p.nelements(); row++) {
    for (uInt col=0; col < cbIndex_p[row].nelements(); col++) {
      if (cbIndex_p[row](col) == matchIndex) {
	retval(nMatch) = row;
	calRowMask(IPosition(1,nMatch)) = true;
	nMatch++;
      };
    };
  };
  
  retval.resize(nMatch, true);
  return retval;
};

//----------------------------------------------------------------------------




} //# NAMESPACE CASA - END

