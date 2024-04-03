//# VisBuffGroupAcc.h: class to group separately averaged VisBuffers
//# Copyright (C) 2008
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
//# $Id: VisBuffAccumulator.h,v 19.6 2004/11/30 17:50:38 ddebonis Exp $

#ifndef MSVIS_VISBUFFGROUPACC_H
#define MSVIS_VISBUFFGROUPACC_H

#include <casacore/casa/aips.h>
#include <msvis/MSVis/VisBuffAccumulator.h>
#include <map>

namespace casa { //# NAMESPACE CASA - BEGIN

// <summary>
// A class to group separately averaged VisBuffers 
// </summary>
//
// <use visibility=export>
//
// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>

// <prerequisite>
//   <li> VisBufferAccumulator
// </prerequisite>
//
// <etymology>
// 
// </etymology>
//
// <synopsis>
// This class groups separately averaged VisBuffers
// </synopsis>
//
// <example>
// </example>
//
// <motivation>
// To encapsulate in a single object multiple (averaged) VisBuffers
// for the solver.
//
// Used by calibration to generate multiple accumulations, e.g., per spw, when
// using combine='spw' and the data cannot be averaged over spw.
// </motivation>
//
// <thrown>
//    <li>
//    <li>
// </thrown>
//
// <todo asof="2008/08/04">
//   <li> ??
// </todo>

class VisBuffGroupAcc
{
public:
  // Construct from the number of antennas, spws, fields, the averaging interval and the pre-normalization flag
  VisBuffGroupAcc (const casacore::Int& nAnt, 
		   const casacore::Int& nSpw, 
		   const casacore::Int& nFld,
		   const casacore::Double& subinterval,
                   const casacore::Bool fillModel=true);

  // Null destructor
  ~VisBuffGroupAcc();

  // Accumulate a VisBuffer
  void accumulate (const VisBuffer& vb);

  // Finalize averaging, and return the result
  void finalizeAverage();

  // Make data amp- or phase-only
  void enforceAPonData(const casacore::String& apmode);

  // Optionally set cross-hands weights to zero, so they are 
  //   not used (e.g., for solving) subsequently, but remain
  //   present in case a general calibration (like P for linears)
  //   is applied that will mix them with the parallel hands
  void enforceSolveCorrWeights(const casacore::Bool phandonly=false);

  // How many separate VisBuffers are contained herein?
  casacore::Int nBuf() const {return nBuf_p;}

  casacore::Int nAnt() const {return nAnt_p;}
  casacore::Int nSpw() const {return nSpw_p;}
  casacore::Int nFld() const {return nFld_p;}

  // The global timestamp
  casacore::Double& globalTimeStamp() { return globalTimeStamp_p; };

  // Return reference to the indexed VisBuffer
  CalVisBuffer& operator()(const casacore::Int& buf);
  CalVisBuffer& operator()(const casacore::Int& spw, const casacore::Int& fld);

  // Return a map from row numbers in the VisBuffer returned by the above
  // operator()s to row numbers in the corresponding input VisBuffer.  Only
  // useful if there is exactly one corresponding input VisBuffer or you are
  // sure that the last corresponding input VisBuffer will meet your
  // needs (i.e. all the corresponding input VisBuffers had same set of
  // antennas and the metadata you want also matches).  hurl controls whether
  // an exception will be thrown if the number of VisBuffers that went into the
  // output of operator() != 1.  Unfilled rows point to -1.
  const casacore::Vector<casacore::Int>& outToInRow(const casacore::Int buf, const casacore::Bool hurl=true) const;
  const casacore::Vector<casacore::Int>& outToInRow(const casacore::Int spw, const casacore::Int fld,
                                const casacore::Bool hurl=true) const;

  // Setup chanmask from a spw:chan selection string and an MS.
  // static so a chanmask can be made once and provided to multiple VBGAs.
  static casacore::Bool fillChanMask(std::map<casacore::Int, casacore::Vector<casacore::Bool>*>& chanmask,
                           const casacore::String& spwstr,
                           const casacore::MeasurementSet& ms);

  // Select channels in the accumulated buffers by flagging with the chanmask.
  // Returns the number of VisBuffers that the chanmask operated on.
  casacore::uInt applyChanMask(std::map<casacore::Int, casacore::Vector<casacore::Bool>*>& chanmask);

  // Empties chanmask (which may have some newed elements).
  static void clearChanMask(std::map<casacore::Int, casacore::Vector<casacore::Bool>*>& chanmask);

  void setTVIDebug(bool debug) {tvi_debug = debug;}

  void reportData();

private:

  // Prohibit in-public null constructor, copy constructor and assignment
  VisBuffGroupAcc();
  VisBuffGroupAcc& operator= (const VisBuffGroupAcc&);
  VisBuffGroupAcc (const VisBuffGroupAcc&);

  // Number of antennas, spw, fld, buffers
  casacore::Int nAnt_p, nSpw_p, nFld_p, nBuf_p;

  // Averaging interval
  casacore::Double subinterval_p;

  casacore::Bool fillModel_p;     // Whether or not to accumulate MODEL_DATA  

  // Pre-normalization flag
  casacore::Bool prenorm_p;

  // Per-interval timestamp averaging
  casacore::Double globalTimeStamp_p;

  // Averaging buffer  (length = nSpw_p x nFld_p)
  casacore::PtrBlock<VisBuffAccumulator*> VBA_p;
  
  // casacore::Map spw,fld to the buffer id
  casacore::Matrix<casacore::Int> spwfldids_p;

  bool tvi_debug;
};


} //# NAMESPACE CASA - END

#endif


