//# RFANewMedianClip.h: this defines RFANewMedianClip
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
#ifndef FLAGGING_RFANEWMEDIANCLIP_H
#define FLAGGING_RFANEWMEDIANCLIP_H

#include <flagging/Flagging/RFAFlagCubeBase.h> 
#include <flagging/Flagging/RFDataMapper.h> 
#include <flagging/Flagging/RFFlagCube.h> 
#include <flagging/Flagging/RFFloatLattice.h> 
#include <flagging/Flagging/RFRowClipper.h> 
#include <casacore/scimath/Mathematics/MedianSlider.h> 

namespace casa { //# NAMESPACE CASA - BEGIN

// <summary>
// RFANewMedianClip:RedFlagger Agent;clips relative to median over time slots
// </summary>

// <use visibility=local>

// <reviewed reviewer="" date="" tests="" demos="">
// </reviewed>

// <prerequisite>
//   <li> MedianSlider
//   <li> RFAFlagCubeBase
// </prerequisite>
//
// <synopsis>
// RFANewMedianClip computes a median of some quantity over time slots, 
// per each channel. Deviation w/respect to the median is computed for 
// the actual flagging.
// </synopsis>
//
// <todo asof="2004/04/21">
//   <li> add this feature
//   <li> fix this bug
//   <li> start discussion of this possible extension
// </todo>

class RFANewMedianClip : public RFAFlagCubeBase, public RFDataMapper
{
public:
  RFANewMedianClip  ( RFChunkStats &ch, const casacore::RecordInterface &parm );
  virtual ~RFANewMedianClip ();

  virtual casacore::uInt estimateMemoryUse ();
  virtual casacore::Bool newChunk (casacore::Int &maxmem);
  virtual void endChunk ();
  virtual void startData (bool verbose);
  virtual void startDry (bool verbose); // add
  virtual IterMode iterTime (casacore::uInt itime);
  virtual IterMode iterRow  (casacore::uInt irow);
  virtual IterMode iterDry  (casacore::uInt it);
  virtual IterMode endData  ();
  virtual IterMode endDry  ();
 
  virtual casacore::String getDesc ();
  static const casacore::RecordInterface & getDefaults ();

protected:
  casacore::MedianSlider & slider (casacore::uInt ich,casacore::uInt ifr);
  casacore::MedianSlider globalmed;

  FlagCubeIterator * pflagiter; 
  FlagCubeIterator flag_iter;
  casacore::Double  threshold;  

  casacore::MedianSlider * msl;

  // lattice of evaluated values [NCH,NIFR,NTIME]
  RFFloatLattice evalue;
  // matrix of standard deviation [NCH,NIFR]
  casacore::Matrix<casacore::Float> stdev;
  casacore::Bool stdeved;
  casacore::Double globalsigma;
};


inline casacore::MedianSlider & RFANewMedianClip::slider (casacore::uInt ich,casacore::uInt ifr)
{
  return msl[ ifr*num(CHAN) + ich ];
}


} //# NAMESPACE CASA - END

#endif
