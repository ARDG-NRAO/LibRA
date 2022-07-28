//# RFADiffBase.h: this defines RFADiffBase and RFADiffMapbase
//# Copyright (C) 2000,2001,2002
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
#ifndef FLAGGING_RFADIFFBASE_H
#define FLAGGING_RFADIFFBASE_H

#include <flagging/Flagging/RFAFlagCubeBase.h> 
#include <flagging/Flagging/RFDataMapper.h> 
#include <flagging/Flagging/RFFloatLattice.h>
#include <flagging/Flagging/RFRowClipper.h>
#include <casacore/scimath/Mathematics/MedianSlider.h> 
#include <casacore/casa/Arrays/LogiVector.h>
#include <casacore/casa/Containers/RecordInterface.h>


namespace casa { //# NAMESPACE CASA - BEGIN

// min number of deviations for which average is considered valid
const int   RFA_MIN_NAD = 20;
// significant change in accumulated average
const casacore::Float RFA_AAD_CHANGE = 0.05;

// <summary>
// RFADiffBase: abstract class for deviation-based flagging
// </summary>

// <use visibility=local>

// <reviewed reviewer="" date="" tests="" demos="">
// </reviewed>

// <prerequisite>
//   <li> RFCubeLattice
//   <li> RFRowClipper
// </prerequisite>
//
// <etymology>
// Diff = Deviation. Well, almost...
// </etymology>
//
// <synopsis>
// Several flagging algorithms flag by analyzing the deviation w/respect
// to something at each point. RFADiffBase provides common functions for
// these classes. It will maintain a lattice of deviations, compute the 
// noise level estimates, and flag points. It will also flag rows with 
// excessive noise level (using RFRowClipper). Derived classes are
// responsible for computing the deviation.
// </synopsis>
//
// <todo asof="2001/04/16">
//   <li> add this feature
//   <li> fix this bug
//   <li> start discussion of this possible extension
// </todo>

class RFADiffBase : public RFAFlagCubeBase
{
public:
  RFADiffBase  ( RFChunkStats &ch,const casacore::RecordInterface &parm );
  virtual ~RFADiffBase ();
  
  virtual casacore::uInt estimateMemoryUse ();  
  virtual casacore::Bool newChunk ( casacore::Int &maxmem );
  virtual void endChunk ();
  virtual void startData (bool verbose);
  virtual void startDry (bool verbose);
  virtual IterMode iterTime (casacore::uInt it);
  virtual IterMode iterDry  (casacore::uInt it);
  virtual IterMode endData  ();
  virtual IterMode endDry   ();

  virtual casacore::String getDesc ();
  static const casacore::RecordInterface & getDefaults ();

protected:
  static casacore::Bool dummy_Bool;

// prepares for a pass over one data row
  void startDataRow (casacore::uInt ifr);
// updates the diff lattice with a value, and performs clipping
  casacore::Float setDiff (casacore::uInt ich,casacore::uInt ifr,casacore::Float d,casacore::Bool &flagged = dummy_Bool );
// ends pass over single data row  
  void endDataRow   (casacore::uInt ifr);
  
// updates noise estimates (sih0), returns the max change
  casacore::Float updateSigma ();     
  
// computes a correlations mask. Called once for each chunk (since correlations
// can change from chunk to chunk)
  virtual RFlagWord newCorrMask () =0;

  casacore::Double clip_level;      // clipping level, in AADs
  casacore::Double row_clip_level;  // clipping level for rows (based on noise estimates), <0 for disable
  casacore::Bool   disable_row_clip; // flag: row clipping _disabled_ globally
  casacore::Bool   clipping_rows;    // flag: row clipping active for this chunk
  
  RFFloatLattice diff;   // (Nchan,Nifr,Nt) cube of deviations
  FlagCubeIterator *     pflagiter; // flag iterator used by setDiff()
  RFRowClipper          rowclipper;
  
  casacore::Vector<casacore::Float> diffrow;   // one row of deviations, for noise computations
  int idiffrow;

  casacore::Matrix<casacore::Float> sig;       // current noise estimate for (it,ifr)
  casacore::Matrix<casacore::Float> sig0;      // reference estimate (boxcar average from previous pass)
  casacore::LogicalVector sigupdated;
};

// <summary>
// Abstract base class for deviation-based flagging with a data mapper.
// </summary>

// <use visibility=local>

// <reviewed reviewer="" date="" tests="" demos="">
// </reviewed>

// <prerequisite>
//   <li> RFDataMapper
// </prerequisite>
//
// <synopsis>
// This is another abstract class on top of DiffBase. It is also inherited from
// RFDataMapper, so it includes functions for mapping visibilities to a single
// casacore::Float value.
// </synopsis>
//
// <todo asof="2001/04/16">
//   <li> add this feature
//   <li> fix this bug
//   <li> start discussion of this possible extension
// </todo>

class RFADiffMapBase : public RFADiffBase, protected RFDataMapper
{
public:
  RFADiffMapBase  ( RFChunkStats &ch,const casacore::RecordInterface &parm );
  virtual ~RFADiffMapBase ();

  virtual IterMode iterTime (casacore::uInt it);
  
  virtual casacore::String getDesc ();
  
// returns a casacore::Record of all available parameters and their default values
  static const casacore::RecordInterface & getDefaults ();
  
protected:
  virtual RFlagWord newCorrMask () 
    { return RFDataMapper::corrMask(chunk.visIter()); } 
  
  void setupMapper () 
    { RFDataMapper::setVisBuffer(chunk.visBuf()); }
};

} //# NAMESPACE CASA - END

#endif
