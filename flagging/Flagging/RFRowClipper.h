//# RFRowClipper.h: this defines RFRowClipper
//# Copyright (C) 2000,2001
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
#ifndef FLAGGING_RFROWCLIPPER_H
#define FLAGGING_RFROWCLIPPER_H

#include <flagging/Flagging/RFCommon.h>
#include <casa/Arrays/Vector.h>
#include <casa/Arrays/Matrix.h>
    
namespace casa { //# NAMESPACE CASA - BEGIN

class RFFlagCube;
class RFChunkStats;
    
// <summary>
// RFRowClipper: flags rows based on their noise level
// </summary>

// <use visibility=local>

// <reviewed reviewer="" date="" tests="" demos="">
// </reviewed>

// <synopsis>
// RFRowClipper accumulates per-row noise estimates in an [NIFR,NTIME] matrix.
// After each pass it performs flagging of rows with excessive noise (w/respect 
// to a sliding median per IFR, over time).
// </synopsis>
//
// <motivation>
// Several flagging agents produce per-row noise estimates and can flag based
// on them. Hence, a commmon implementation was desired.
// </motivation>
//
// <todo asof="2001/04/16">
//   <li> add this feature
//   <li> fix this bug
//   <li> start discussion of this possible extension
// </todo>

class RFRowClipper : public FlaggerEnums
{
public:
  // construct from a chunk accessor and flag cube. Clip is the clipping
  // level, HW is the sliding median window half-width, MAXP is maximum
  // iterative passes.
  RFRowClipper  ( RFChunkStats &chunk,RFFlagCube &flag,casacore::Float clip,casacore::uInt hw=6,casacore::uInt maxp=5 );
  // destructor
  ~RFRowClipper () {}; 
  
  // initialize for an [NI,NT] matrix
  void init     ( casacore::uInt ni,casacore::uInt nt );
  // deallocate matrices
  void cleanup  ();
  // reset at start of pass
  void reset    ();
  
  // returns the current noise estimate
  casacore::Float sigma0   ( casacore::uInt ifr,casacore::uInt it );
  // sets a new noise estimate
  void  setSigma ( casacore::uInt ifr,casacore::uInt it,casacore::Float level );
  // marks a noise estimate as updated without changing it
  void  markSigma ( casacore::uInt ifr );

  // recompute updated estimates and optionally do row flagging
  casacore::Float updateSigma (casacore::uInt &ifrmax,casacore::uInt &itmax,casacore::Bool flagrows = true, bool clear_flags = true );
        
private:
  RFChunkStats &chunk;
  RFFlagCube   &flag;
  casacore::Float clip_level;
  casacore::uInt halfwin,maxpass;
  
  casacore::uInt nifr,ntime;
  casacore::Matrix<casacore::Float> sig,sig0;
  casacore::Vector<casacore::Bool> sigupdated;
  
  casacore::LogIO &os;
};

inline casacore::Float RFRowClipper::sigma0 (casacore::uInt ifr,casacore::uInt it)
{
  return sig0(it,ifr);
}

inline void RFRowClipper::setSigma (casacore::uInt ifr,casacore::uInt it,casacore::Float level) 
{
  sig(it,ifr) = level;
  sigupdated(ifr) = true;
}

inline void RFRowClipper::markSigma (casacore::uInt ifr) 
{
  sigupdated(ifr) = true;
}


} //# NAMESPACE CASA - END

#endif
