//# GroupProcessor.cc: Step through the Measurement Set by groups of VisBuffers.
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

#include <msvis/MSVis/GroupProcessor.h>
#include <msvis/MSVis/VisBuffGroup.h>
#include <casa/Exceptions/Error.h>
#include <casa/Logging/LogIO.h>
#include <casa/System/ProgressMeter.h>

using namespace casacore;
namespace casa {

GroupProcessor::GroupProcessor(ROVisibilityIterator& vi, GroupWorkerBase *gw,
                               Double groupInterval) :
  vi_p(vi),
  gw_p(gw),
  groupInterval_p(groupInterval),
  tvi_debug(False)
{
}

// GroupProcessor& GroupProcessor::operator=(const GroupProcessor &other)
// {
//   // trivial so far.
//   vi_p = other.vi_p;
//   return *this;
// }

void GroupProcessor::setGroupOrigin()
{
  // cerr << "Starting new group" << endl;
  vi_p.origin();
  vi_p.time(timev_p);
  groupStart_p = timev_p[0];
}

Bool GroupProcessor::groupHasMore()
{
  Bool answer = false;
  if(vi_p.moreChunks()){
    vi_p.time(timev_p);
    answer = abs(timev_p[0] - groupStart_p) <= groupInterval_p;
  }
  return answer;
}

Bool GroupProcessor::go()
{
  VisBuffer vb(vi_p);

  // Process the MS(es) chunk by chunk.
  uInt ninrows = vi_p.numberCoh();      // Apparently this accounts for selection.
  ProgressMeter meter(0.0, ninrows * 1.0, "GroupProcessor", "rows processed", "", "",
                      true, 1);
  uInt inrowsdone = 0;  // only for the meter.
  vi_p.originChunks();
  while(vi_p.moreChunks()){
    VisBuffGroup vbg;

    for(setGroupOrigin(); groupHasMore(); vi_p.nextChunk()){
      for(vi_p.origin(); vi_p.more(); ++vi_p){
        vb.fetch(gw_p->prefetchColumns());
        // cerr << "     " << vb.rowIds()[0] << "          " << (vb.time()[0] - 4.65136e9)
        //     << "         " << vb.spectralWindow() << endl;

        // jagonzal: Without this call the baselines corresponding to the
        // first time stamp of each chunk are skipped.
        // This does not happen with VI/VB2 which is used by the TVI framework
        // Also VisibilityIteratorReadImpl::update_rowIds seems to be extremely slow (see CAS-8882)
        if (tvi_debug) vb.rowIds();

        vbg.store(vb);
      }
      vbg.endChunk();                   // Record the end of a chunk.
      inrowsdone += vi_p.nRowChunk();
    }
    if(!gw_p->process(vbg))
      return false;
    meter.update(inrowsdone);
  }
  return true;
}

using namespace casacore;
} // end namespace casa
