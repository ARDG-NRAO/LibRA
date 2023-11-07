//# RFAMedianClip.h: this defines RFATimeMedian and RFAFreqMedian
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
#ifndef FLAGGING_RFAMEDIANCLIP_H
#define FLAGGING_RFAMEDIANCLIP_H

#include <flagging/Flagging/RFADiffBase.h> 
#include <casacore/scimath/Mathematics/MedianSlider.h> 

namespace casa { //# NAMESPACE CASA - BEGIN

// <summary>
// RFATimeMedian: RedFlagger Agent for clipping relative to median over time
// </summary>

// <use visibility=local>

// <reviewed reviewer="" date="" tests="" demos="">
// </reviewed>

// <prerequisite>
//   <li> MedianSlider
//   <li> RFADiffMapBase
// </prerequisite>
//
// <synopsis>
// RFATimeMedian computes a sliding median of some quantity (as established
// in RFADiffMapbase) over time, per each channel. Deviation w/respect to
// the median is passed to RFADiffBase for the actual flagging.
// </synopsis>
//
// <todo asof="2001/04/16">
//   <li> add this feature
//   <li> fix this bug
//   <li> start discussion of this possible extension
// </todo>

class RFATimeMedian : public RFADiffMapBase
{
public:
  RFATimeMedian  ( RFChunkStats &ch,const casacore::RecordInterface &parm );
  virtual ~RFATimeMedian ();

  virtual casacore::Bool newChunk (casacore::Int &maxmem);
  virtual void endChunk ();
  virtual void startData (bool verbose);
  virtual IterMode iterTime (casacore::uInt itime);
  virtual IterMode iterRow  (casacore::uInt irow);
  virtual IterMode endData  ();
  virtual casacore::String getDesc ();
  static const casacore::RecordInterface & getDefaults ();

protected:
  casacore::uInt itime;  
  casacore::MedianSlider & slider (casacore::uInt ich,casacore::uInt ifr);

  FlagCubeIterator flag_iter;
  
  casacore::uInt halfwin;
  casacore::MedianSlider *msl;
  
};

inline casacore::MedianSlider & RFATimeMedian::slider (casacore::uInt ich,casacore::uInt ifr)
{
  return msl[ ifr*num(CHAN) + ich ];
}


// <summary>
// RFAFreqMedian: RedFlagger Agent for clipping relative to median over frequency
// </summary>

// <use visibility=local>

// <reviewed reviewer="" date="" tests="" demos="">
// </reviewed>

// <prerequisite>
//   <li> MedianSlider
//   <li> RFADiffMapBase
// </prerequisite>
//
// <synopsis>
// RFAFreqMedian computes a sliding median of some quantity (as established
// in RFADiffMapbase) over frequency, per each row. Deviation w/respect to
// the median is passed to RFADiffBase for the actual flagging.
// </synopsis>
//
// <todo asof="2001/04/16">
//   <li> add this feature
//   <li> fix this bug
//   <li> start discussion of this possible extension
// </todo>

class RFAFreqMedian : public RFADiffMapBase
{
public:
  RFAFreqMedian  ( RFChunkStats &ch,const casacore::RecordInterface &parm );
  virtual ~RFAFreqMedian () {};

  virtual casacore::Bool newChunk (casacore::Int &maxmem);
  virtual RFA::IterMode iterRow (casacore::uInt irow);
  virtual casacore::String getDesc ();
  static const casacore::RecordInterface & getDefaults ();

protected:
  casacore::uInt halfwin;

};


} //# NAMESPACE CASA - END

#endif
