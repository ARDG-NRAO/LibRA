//# VBGContinuumSubtractor.h: Estimates the continuum of VisBuffGroups and
//# writes the difference to a new MS.
//# Copyright (C) 2011
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

#ifndef MSVIS_VBGCONTINUUMSUBTRACTOR_H
#define MSVIS_VBGCONTINUUMSUBTRACTOR_H

#include <casacore/casa/aips.h>
#include <casacore/ms/MeasurementSets/MeasurementSet.h>
#include <msvis/MSVis/GroupWorker.h>
#include <msvis/MSVis/VisibilityIterator.h>
#include <msvis/MSVis/VisBufferComponents.h>
#include <map>
#include <set>

namespace casa { //# NAMESPACE CASA - BEGIN

//# forward decl
class VisBuffGroup;

//<summary>VBGContinuumSubtractors subtract the continuum out of VisBuffGroups</summary>
//
// <use visibility=export>
//
// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//   <li> <linkto class="VisBuffGroup">VisBuffGroup</linkto>
//   <li> <linkto class="VisBuffGroup">VisibilityIterator</linkto>
//   <li> <linkto class="GroupProcessor">GroupProcessor</linkto>
//   <li> <linkto class="GroupWriteToNewMS">GroupWriteToNewMS</linkto>
// </prerequisite>
//
// <etymology>
// VBGContinuumSubtractors subtract the continuum out of VisBuffGroups.
// </etymology>
//
//<synopsis>
// This estimates the continuum by fitting a low order polynomial in frequency
// for each baseline in a VisBuffGroup, and then writes those visibilities,
// with the continuum subtracted, to a different MS.  The VisBuffers in the
// VisBuffGroup do not all have to have the same spw, set of baselines, or even
// set of correlations, but output visibilities for which the continuum cannot
// be estimated will be flagged.
//</synopsis>
//
//<todo>
// <li> 
//</todo>
class VBGContinuumSubtractor : public GroupWriteToNewMS
{
public:
  // Construct an object that will write to outms after subtracting a continuum
  // estimated by fitting a polynomial of order fitorder.  It will always
  // _write_ to DATA if datacols.nelements() == 1, and write to all 3
  // otherwise.  Thus datacols.nelements() should be either 1 or 3.  Anything
  // else will result in an AipsError.  4 and 2 are right out, and FLOAT_DATA
  // isn't handled by this yet.
  //
  // invi is the input VisibilityIterator, only used directly here to get the
  // sort columns and whether or not WEIGHT_SPECTRUM and/or FLAG_CATEGORY are
  // present.
  //
  VBGContinuumSubtractor(casacore::MeasurementSet& outms,
                         casacore::MSColumns *msc,
                         const VBRemapper& remapper,
                         const ROVisibilityIterator& invi,
                         const casacore::uInt fitorder=1,
                         const casacore::MS::PredefinedColumns datacols=casacore::MS::DATA,
                         const casacore::String& fitspw="*",
                         const casacore::String& outspw="*");

  //// Copy construct
  //VBGContinuumSubtractor(const VBGContinuumSubtractor& other) {}

  // Destructor
  virtual ~VBGContinuumSubtractor();

  //// Assignment
  //virtual VBGContinuumSubtractor& operator=(const VBGContinuumSubtractor& gw) {}

  // // Returns which columns need to be prefetched for process to work.
  // virtual asyncio::PrefetchColumns *prefetchColumns() const;

  // This is where all the work gets done!
  virtual casacore::Bool process(VisBuffGroup& vbg);

  void setTVIDebug(bool debug) {tvi_debug = debug;}
  void setWantCont(bool want_cont) {want_cont_p = want_cont;}

private:
  // Disable null c'tor.
  VBGContinuumSubtractor();

  // Initialized by c'tor:

  // Order of the fit polynomials.
  casacore::uInt fitorder_p;
  
  // Which of DATA, MODEL_DATA, or CORRECTED_DATA to fit.
  // It will always _write_ to DATA if datacols_p.nelements() == 1, and write
  // to all 3 otherwise.  Thus datacols_p.nelements() should be either 1 or 3.
  // 4 and 2 are right out, and FLOAT_DATA isn't handled by this yet.
  casacore::MS::PredefinedColumns datacol_p;

  casacore::String fitspw_p;      // Line-free channels used for the fit.  Can include ;
  casacore::String outspw_p;      // Channels to write out.  Does not yet support ;.
  casacore::uInt   rowsdone_p;    // How many rows have been written so far.
  std::set<casacore::Int> outspws_p;  // Spws to write out.
  
  casacore::Bool doWS_p;                               // Is WEIGHT_SPECTRUM present?
  casacore::Bool doFC_p;                               // Is FLAG_CATEGORY present?

  // Not initialized by c'tor:

  // These cubes could be made local to process(), but I want to avoid repeated
  // c'ting and d'ting.
  casacore::Cube<casacore::Complex> coeffs_p; // casacore::Polynomial coefficients for the continuum, indexed by (corr,
                          // order, hash(ant1, ant2).
  casacore::Cube<casacore::Bool> coeffsOK_p;  // Flags on the sol'ns, indexed by (corr,
                          // order, hash(ant1, ant2).

  std::set<casacore::Int> appliedSpWs_p;

  std::map<casacore::Int, casacore::Vector<casacore::Bool>*> fitmask_p;      // spw -> a list of flags by chan

  bool tvi_debug;
  bool want_cont_p;
};

} //# NAMESPACE CASA - END

#endif

