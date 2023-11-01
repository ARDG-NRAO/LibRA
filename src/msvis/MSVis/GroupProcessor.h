//# GroupProcessor.h: Step through the Measurement Set by groups of VisBuffers.
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

#ifndef MSVIS_GROUPPROCESSOR_H
#define MSVIS_GROUPPROCESSOR_H

#include <casacore/casa/aips.h>
//#include <casa/Containers/Stack.h>
//#include <ms/MeasurementSets/MeasurementSet.h>
//#include <msvis/MSVis/VisBuffGroup.h>
#include <msvis/MSVis/VisibilityIterator.h>
#include <msvis/MSVis/GroupWorker.h>

namespace casacore{

//class Block;
//class MeasurementSet;
}

namespace casa { //# NAMESPACE CASA - BEGIN

//# forward decl

// <summary>
// GroupProcessor goes through one or more MeasurementSets, feeding VisBuffGroups
// to a GroupWorker.
// </summary>
// <use visibility=export>
// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>
// <prerequisite>
//   <li> <linkto class="VisibilityIterator">VisibilityIterator</linkto>
//   <li> <linkto class="casacore::MeasurementSet">casacore::MeasurementSet</linkto>
//   <li> <linkto class="VisBuffer">VisBuffer</linkto>
//   <li> <linkto class="VisBuffGroup">VisBuffGroup</linkto>
//   <li> <linkto class="GroupWorker">GroupWorker</linkto>
// </prerequisite>
//
// <etymology>
// The GroupProcessor processes one VisBuffGroup after another.
// </etymology>
//
// <synopsis>
// GroupProcessor handles iteration with by groups of chunklets
// for one or more MSs.
// </synopsis>
//
// <example>
// <code>
// // VBGContinuumSubtractor is a GroupWorker.
// VBGContinuumSubtractor vbgcs(msOut_p, fitorder_p);
//
// // sort is a casacore::Block<casacore::Int> with the right columns.
// VisibilityIterator viIn(mssel_p, sort, 0.0);
//
// GroupProcessor rogp(viIn, &vbgcs);
// casacore::Bool success = rogp.go();            // Encapsulates the nested for loops.
// </code>
// </example>
//
// <motivation>
// For imaging and calibration you need to access an casacore::MS in some consistent
// order (by field, spectralwindow, time interval etc.). This class provides
// that access.  Furthermore, some calculations or applications need more than
// one VisBuffer.  For example, one might want to estimate and subtract the
// continuum from the visibilities of an casacore::MS that has a broad line completely
// spanning spw 1, but spws 0 and 2 line-free, so the spws should be combined
// (combine='spw') for the continuum estimation.
//
// It is much more efficient if the group of necessary data can be read only
// once, worked on, and then written.  The CalTable approach is more flexible
// in that a CalTable can be applied to an casacore::MS with a different number or
// arrangement of rows from the input casacore::MS, but per chunk it requires two more
// reads (the CalTable and the _output_ casacore::MS) and an extra write (the CalTable).
//
// Another goal is to encapsulate the commonly repeated boilerplate of setting
// up VisIters and VisBuffers, and then looping over chunks and then chunklets.
//
// A separate GroupProcessor is needed so the input casacore::MS can be declared const.
// </motivation>
//
// <thrown>
//    <li>
//    <li>
// </thrown>
//
// <todo asof="2011/11/07">
// ProgressMeter
// </todo>
class GroupProcessor
{
public:
  // Construct from a ROVisibilityIterator (provides the data) and a
  // pointer to a GroupWorker (does something with the data).
  //
  // vi should be set up (selection, slurping, and sort order specified) before
  // calling this, but this will call vi.originChunks() and drive the iteration.
  //
  GroupProcessor(ROVisibilityIterator& vi, GroupWorkerBase *gw,
                   casacore::Double groupInterval=0.0);

  // // Copy construct. This calls the assigment operator.
  // GroupProcessor(const GroupProcessor & other){
  //   vi_p = other.vi_p ? other.vi_p->clone(&(this->vi_p)) : NULL;
  // }

  // Assigment. Any attached VisBuffers are lost in the assign.
  // GroupProcessor & operator=(const GroupProcessor &other);
  // // Destructor
  // //virtual ~GroupProcessor() {};
  // ~GroupProcessor() {};
  
  // Members

  // Return and set the "timebin" for each group, in s.
  casacore::Double groupInterval() {return groupInterval_p;}
  void setGroupInterval(casacore::Double gi) {groupInterval_p = abs(gi);}

  // Once it's all set up, you should just have to tell it to go!
  // Returns true/false depending on whether it thinks it went all the way.
  casacore::Bool go();
  
  static casacore::String getAipsRcBase() {return "GroupProcessor";}

  void setTVIDebug(bool debug) {tvi_debug = debug;}

private:
  void setGroupOrigin();        // casacore::Record the beginning of a new group.
  casacore::Bool groupHasMore();          // Returns whether or not more VisBuffers
                                // should be added to the group.

  // Initialized by c'tor.
  ROVisibilityIterator vi_p;
  GroupWorkerBase      *gw_p;
  casacore::Double               groupInterval_p;  // >= 0.0, in s.

  // Uninitialized by c'tor.
  casacore::Vector<casacore::Double> timev_p;
  casacore::Double groupStart_p;
  bool tvi_debug;
};

} //# NAMESPACE CASA - END

#endif
