//# RFABase.h: this defines RFABase
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
#ifndef FLAGGING_RFABASE_H
#define FLAGGING_RFABASE_H

#include <casacore/casa/Arrays/Cube.h> 
#include <flagging/Flagging/RFChunkStats.h>
#include <casacore/casa/Logging/LogIO.h>
#include <casacore/casa/Containers/Record.h>

namespace casa { //# NAMESPACE CASA - BEGIN

// <summary>
// Abstract RedFlagger Agent base class
// </summary>

// <use visibility=local>

// <reviewed reviewer="" date="" tests="" demos="">
// </reviewed>

// <synopsis>
// RFABase defines the interface for a flagging agent
// </synopsis>
//
// <motivation>
// RedFlagger works with objetcs called flagging agents. This class
// defines the complete interface between RedFlagger and agents.
// </motivation>
//
// <todo asof="2001/04/16">
//   <li> add this feature
// </todo>

class RFABase : public FlaggerEnums
{
public:
// iteration modes
  enum IterMode { 
      STOP = 0,
      DRY  = 1,
      DATA = 2,
          
      CONT = 3
  };
  
// An agent is always constructed from a chunk stats accessor, and a 
// record of parameters. No other constructors are defined, and no others
// may be used.
  RFABase ( RFChunkStats &ch,const casacore::RecordInterface &parm );
// Destructor 
  virtual ~RFABase () {};

// This method is called after constructing the agent.
  virtual void init ();

// This method is called before iterating over a chunk, to inquire the 
// expected memory use. Should return the max desired memory footprint, in MB.
// Available physical memory is divided between agents in proportion to their 
// requests.
  virtual casacore::uInt estimateMemoryUse () { return 1; }  

// Called before iterating over a chunk. Returns true if agent will
// process this chunk, or false if this the agent is unable to process it.
// (this can happen if, e.g., the requisite correlations are not present).
// The casacore::Int & maxmem argument is the number of MB memory which is still 
// available in the memory pool. The agent class should plan its memory 
// use accordingly, and subtract its expected memory use from maxmem. In effect, 
// the agent "reserves" some amount of memory. This is used by RedFlagger to 
// contain the total memory footprint. Note that only a rough reckoning
// is sufficient, so only bother estimating the biggest data structures.
// See implementations in RFADiffBase and RFATimeMedian for good examples.
// nAgent is the total number of agents.
  virtual casacore::Bool newChunk (casacore::Int &) 
         { return active=false; };
// Called once finished with a chunk
  virtual void endChunk () {}
  
// Called before starting a data pass on a chunk. 
  virtual void startData (bool /* verbose */) {};

// Called before starting a dry pass on a chunk. 
  virtual void startDry  (bool /* verbose */) {};

// Called before starting the fetch-flags pass.
  virtual void startFlag (bool /* verbose */) {};

// Called after a pass is completed successfully (i.e., not stopped
// by start or iter methods). Return value: STOP to stop, DATA for 
// another data pass, DRY for another dry pass.
  virtual IterMode endData   () { return STOP; };

// Called after a dry pass is complete
  virtual IterMode endDry    () { return STOP; };

// Called after a flag pass is complete
  virtual void endFlag () {};

// Called at end of time chunk
virtual void endRows(casacore::uInt /* itime */) {};
  
// Iteration methods for a data pass. Either or both may be implemented.
// iterTime() is called once for each new VisBuffer (= new time slot)
// Return value: STOP to finish iterating, CONT/DATA to continue, or DRY
// to cancel the data pass and request a dry pass.
  virtual IterMode iterTime ( casacore::uInt /* itime */ ) { return CONT; };

// iterRow() is called once per each row in the VisBuffer.
// Iterating over rows is perhaps preferrable in terms of performance,
// at least for data iterations.
  virtual IterMode iterRow  ( casacore::uInt /* irow */ ) { return CONT; };

// Iteration method for a dry pass. Called once per each time slot.
// Return value: STOP to finish iterating, CONT/DRY to continue, or DATA
// to cancel the dry pass and request another data pass.
  virtual IterMode iterDry  ( casacore::uInt /* itime */ ) { return CONT; };

// Iteration method for a flag pass. Called once per each VisBuffer.
  virtual void iterFlag ( casacore::uInt /* itime */ ) {}

// called to obtain a short description of this RFA
  virtual casacore::String getDesc () { return ""; }
// called (before endChunk()) to obtain a statistics report 
  virtual casacore::String getStats () { return ""; }

  virtual void printFlaggingReport ( ) {};

  virtual casacore::String getID() {return casacore::String("");};
  
// returns the name of this RFA (set in myname)
  const casacore::String & name ();
// returns the active status
  casacore::Bool isActive ()   { return active; }
// accessor to a casacore::LogIO for this agent
  casacore::LogIO & logSink ();

// static method for setting the indexing base for agent arguments
  static void setIndexingBase ( casacore::uInt base );

  virtual casacore::Record getResult( ) { return casacore::Record(); };

  virtual void finalize() {};
  // Initialize chunk
  virtual void initialize() {};
  virtual void initializeIter(casacore::uInt /* iter */) {};
  virtual void finalizeIter(casacore::uInt /* iter */) {};

  virtual void setNAgent(casacore::uInt n) { nAgent = n; };
  virtual void setOnlySelector(bool only_sel) { only_selector = only_sel; };

protected:
  casacore::uInt nAgent;
  RFChunkStats &chunk;
  casacore::Record params;
  casacore::String myname;
  bool only_selector;  //Do only RFASelector agents exist?

  casacore::uInt num (StatEnums which) { return chunk.num(which); };

// Bit mask of correlations which are used & flagged by this RFA. This mask is
// used to (a) interpret the pre-flags of the FLAG column, and (b) apply the
// resulting flags to the FLAG column
  RFlagWord corrmask;
  RFlagWord corrMask()  { return corrmask; }

// flag: agent is active for this chunk (set in newChunk)  
  casacore::Bool active;

  casacore::LogIO os;
  
// global flag indicates that Glish (1-based) indexing is in use
// for agent arguments
  static casacore::uInt indexingBase (); 
  
private:
    
  static casacore::uInt indexing_base;
};

inline casacore::uInt RFABase::indexingBase () 
{ 
  return indexing_base; 
}

inline void RFABase::setIndexingBase ( casacore::uInt base ) 
{ 
  indexing_base = base; 
}
  
inline casacore::LogIO & RFABase::logSink ()
{
  return os;
}
inline const casacore::String & RFABase::name ()
{
  return myname;
}


} //# NAMESPACE CASA - END

#endif
