//# CalIntpMatch.h: A class to hold calibration interpolation index maps
//# Copyright (C) 1996,1997,1998,1999,2001,2002,2003
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
//# Correspondence concerning AIPS++ should be adressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//#
//# $Id$

#ifndef CALIBRATION_CALINTPMATCH_H
#define CALIBRATION_CALINTPMATCH_H

#include <casa/aips.h>
#include <casa/Arrays/Vector.h>
#include <casa/Arrays/LogiArray.h>
#include <casa/Containers/Record.h>
#include <ms/MSSel/MSSelection.h>
#include <msvis/MSVis/VisBuffer.h>
#include <synthesis/CalTables/CalMainBuffer.h>

namespace casacore{

// by class MSSelection). The CalIntpMatch class has a match methods,
}

namespace casa { //# NAMESPACE CASA - BEGIN

// <summary> 
// CalIntpMatch: a class to hold calibration interpolation index maps
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//   <li> <linkto class="MSCalEnums">MSCalEnums</linkto> module
//   <li> <linkto class="VisBuffer">VisBuffer</linkto> module
//   <li> <linkto class="CalMainBuffer">CalMainBuffer</linkto> module
//   <li> <linkto class="casacore::MSSelection">casacore::MSSelection</linkto> module
// </prerequisite>
//
// <etymology>
// From "calibration", "interpolation index" and "match".
// </etymology>
//
// <synopsis>
// The CalIntpMatch class holds calibration interpolation index maps.
// These define which subset of calibration table rows are associated
// with any given visibility row, i.e. which data and calibration
// indices need to match in selecting calibration for a given visibility
// record. The map type is defined in terms of an enumerated set of
// possibilities, including for example, requiring an exact match between
// the antenna and spectral window id.'s. The most general interpolation
// map is defined by a matching set of synthesis selections (as defined
// matchIndex() and calRows() which determine, for associated calibration 
// and visibility buffers, the calibration row mapping for a given visibility.
// </synopsis>
//
// <example>
// <srcblock>
// </srcblock>
// </example>
//
// <motivation>
// Encapsulate match operations for calibration and visibility data indices.
// </motivation>
//
// <todo asof="2002/05/01">
// (i) Expand the range of enumerated mappings.
// </todo>

class CalIntpMatch
{
 public:

  // Basic interpolation index match type
  enum MatchType {
    // ANTENNA_ID and SPECTRAL_WINDOW_ID to match identically
    MATCHING_ANT_SPW = 0,

    // Custom mapping between arbitrary visibility and calibration selections
    SELECTION_MATCH = 1
  };

  // Construct from an index match type and an index dimension. The
  // index dimension is one for baseline-based calibration and two 
  // for antenna-based calibration.
  CalIntpMatch (const MatchType& matchType = MATCHING_ANT_SPW,
		const casacore::uInt& indexDim = 2);

  // Destructor
  virtual ~CalIntpMatch();

  // Copy constructor and assignment operator
  CalIntpMatch (const CalIntpMatch& other);
  virtual CalIntpMatch& operator= (const CalIntpMatch& other);

  // Reset the mapping type
  void setType (const MatchType& matchType, const casacore::uInt& indexDim);

  // Add a selection mapping (NYI)
  //  void add (const casacore::MSSelection& msIndex, const casacore::MSSelection& calIndex);

  // Set the current visibility buffer to be used in interpolation matches
  void setVisBuffer (VisBuffer& vb);

  // Set the current calibration buffer to be used in interpolation matches
  void setCalBuffer (CalMainBuffer& cb);

  // Return the match index for a given visibility buffer 
  // row and offset position (0,1)
  casacore::Int matchIndex (const casacore::Int& row, const casacore::uInt& pos) 
    {return vbIndex_p(row,pos);};

  // Return the calibration buffer rows (and equivalent row mask) 
  // associated with a given match index
  casacore::Vector<casacore::Int> calRows (const casacore::Int& matchIndex, casacore::LogicalArray& calRowMask);

 protected:

 private:
  // Common code for the assignment operator and the copy constructor
  void copy (const CalIntpMatch& other);
  void free();

  // Reset functions for the local index mappings
  void reset();
  void resetVisIndex();
  void resetCalIndex();

  // Match index keys
  casacore::Int matchOrAddKey (const casacore::Record& key);

  // Index map type
  MatchType type_p;

  // Index dimension (one for baseline-based calibration and
  // two for antenna-based calibration).
  casacore::Int indexDim_p;

  // casacore::Map index assignment per visibility buffer row
  casacore::Matrix<casacore::Int> vbIndex_p;

  // casacore::Map index assignment per calibration buffer row
  casacore::Block<casacore::Vector<casacore::Int> > cbIndex_p;

  // casacore::List of visibility and calibration match keys
  casacore::PtrBlock<casacore::Record* > matchKeys_p;

  // Pointer to the current VisBuffer
  VisBuffer* vb_p;

  // Pointer to the current CalMainBuffer
  CalMainBuffer* cb_p;
};


} //# NAMESPACE CASA - END

#endif
   
