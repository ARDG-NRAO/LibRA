//# RFAUVBinner.h: this defines RFAUVBinner
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
#ifndef FLAGGING_RFAUVBINNER_H
#define FLAGGING_RFAUVBINNER_H

#include <flagging/Flagging/RFAFlagCubeBase.h> 
#include <flagging/Flagging/RFDataMapper.h> 
#include <flagging/Flagging/RFFloatLattice.h>
#include <flagging/Flagging/RFFlagCube.h> 
#include <flagging/Flagging/RFRowClipper.h> 
#include <casacore/scimath/Mathematics/RigidVector.h>
    
namespace casa { //# NAMESPACE CASA - BEGIN

// <summary>
// RFAUVBinner: flagging via UV binning
// </summary>

// <use visibility=local>

// <reviewed reviewer="" date="" tests="" demos="">
// </reviewed>

// <prerequisite>
//   <li> RFDataMapper
//   <li> RFFlagCubeBase
//   <li> RFCubeLattice
// </prerequisite>
//
// <synopsis>
// </synopsis>
//
// <todo asof="2001/04/16">
//   <li> make UV-distance matrix static, to share between multiple instances
//   <li> collect population statistics across all channels
//   <li> 3D bins (ampl-UVdist-channel)?
//   <li> think how to solve "encroaching" problem to achieve a better
//        probability distribution. Perhaps two sets of staggered bins,
//        and for each point use the count of the bigger bin?
// </todo>

class RFAUVBinner : public RFAFlagCubeBase, public RFDataMapper
{
public:
  RFAUVBinner  ( RFChunkStats &ch,const casacore::RecordInterface &parm ); 
  virtual ~RFAUVBinner () {};
  
  virtual casacore::uInt estimateMemoryUse ();
  virtual casacore::Bool newChunk (casacore::Int &maxmem);
  virtual void endChunk ();
  virtual void startData (bool verbose);
  virtual void startDry (bool verbose);
  virtual IterMode iterTime (casacore::uInt it);
  virtual IterMode iterRow  (casacore::uInt ir);
  virtual IterMode iterDry  (casacore::uInt it);
  virtual IterMode endData  ();
  virtual IterMode endDry  ();

  virtual casacore::String getDesc ();
  static const casacore::RecordInterface & getDefaults ();
  
protected:
  casacore::IPosition computeBin( casacore::Float uv,casacore::Float y,casacore::uInt ich );

  casacore::Double  threshold;
  casacore::uInt    min_population;
  casacore::uInt    nbin_y,nbin_uv;
  casacore::Bool    binned;
  
// current UVW column
  casacore::Vector< casacore::RigidVector<casacore::Double,3> > *puvw;

// lattice of yvalues [NCH,NIFR,NTIME]
  RFFloatLattice yvalue;
// matrix of UV distances [NIFR,NTIME]
  casacore::Matrix<casacore::Float> uvdist;

// ranges and bin sizes
  casacore::Vector<casacore::Float> ymin,ymax,ybinsize,
// for UV, we have individual ranges/bins per each channel
               uvmin,uvmax,uvbinsize;
// bin counts
  casacore::Cube<casacore::Int> bincounts;
  casacore::Vector<casacore::Int> totcounts;
};


} //# NAMESPACE CASA - END

#endif
