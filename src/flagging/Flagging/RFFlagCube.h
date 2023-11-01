
//# RFFlagCube.h: this defines RFFlagCube
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
#ifndef FLAGGING_RFFLAGCUBE_H
#define FLAGGING_RFFLAGCUBE_H
    
//#include <flagging/Flagging/RedFlagger.h>
#include <flagging/Flagging/RFCubeLattice.h>
#include <flagging/Flagging/RFChunkStats.h>
#include <casacore/casa/Arrays/ArrayLogical.h>
#include <casacore/casa/Arrays/LogiMatrix.h>
#include <casacore/casa/Arrays/LogiVector.h>
#include <casacore/casa/Logging/LogIO.h>
#include <stdexcept>

namespace casa { //# NAMESPACE CASA - BEGIN

typedef RFCubeLatticeIterator<RFlagWord> FlagCubeIterator;

// special row flag masks. RowFlagged for flagged rows, 
// RowAbsent for absent rows
const RFlagWord RowFlagged=1,RowAbsent=2;

// casacore::Function for working with bitmasks. Does a bitwise-AND
// on every element, returns true if !=0 or false if ==0
template<class T> casacore::Array<T> operator & ( const casacore::Array<T> &,const T &);
// returns a casacore::LogicalArray corresponding to (ARR&MASK)!=0
template<class T> casacore::LogicalArray  maskBits  ( const casacore::Array<T> &,const T &);

// <summary>
// RFFlagCube: a cube of flags
// </summary>

// <use visibility=local>

// <reviewed reviewer="" date="" tests="" demos="">
// </reviewed>

// <prerequisite>
//   <li> RFCubeLattice
// </prerequisite>
//
// <synopsis>
// RFFlagCube implements an [NCHAN,NIFR,NTIME] cube of flags, stored in
// a casacore::TempLattice that is iterated alog the TIME axis.  One static
// (i.e. global) cube is used to hold the actual flags. Individual
// instances (instantiated by flagging agents) have individual unique
// bitmasks and, possibly, individual iterators.
//
// It was/is a design mistake to use a global/static buffer to hold the
// shared flags. Instead, every agent should point to the unique dynamically
// allocated buffer. 
// </synopsis>
//
// <example>
// </example>
//
// <motivation>
// </motivation>
//
// <todo asof="2001/04/16">
//   <li> add this feature
//   <li> fix this bug
//   <li> start discussion of this possible extension
// </todo>

class RFFlagCube : public FlaggerEnums
{
public:
  // default log sink
  static casacore::LogIO default_sink;
    
  // constructor
  RFFlagCube ( RFChunkStats &ch,casacore::Bool ignore=false,casacore::Bool reset=false,casacore::LogIO &os=default_sink );
  ~RFFlagCube ();

  // returns reference to logsink
  casacore::LogIO & logSink ();

  // returns estimated size of flag cube for a given chunk.
  static casacore::uInt estimateMemoryUse ( const RFChunkStats &ch );

  // creates flag cube for current chunk. name is name of agent.
  // nAgent is total number of agents
  void init ( RFlagWord polmsk, casacore::uInt nAgent, bool is_selector, const casacore::String &name = "" );

  // cleans up at end of chunk
  void cleanup ();

  // returns summary of stats in text form
  casacore::String getSummary ();

  // prints flagging stats to stderr
  void printStats ();

  // resets at start of pass
  void reset ();

  // advances global flag iterator to time slot it (if required), sets
  // the flag cursor from the iterator (see below). If getflags is true,
  // also calls getDataFlags().
  void advance   ( casacore::uInt it,casacore::Bool getFlags=false );

  // fills global flag lattice with apriori flags from a VisBuffer (if required)
  void getMSFlags  (casacore::uInt it);

  // transfers all flags from lattice into VisBuffer
  void setMSFlags  (casacore::uInt itime);

  // creates a custom iterator
  FlagCubeIterator newCustomIter ();

  // Returns full flag matrix (i.e. cursor of global iterator)
  const FlagMatrix & flagMatrix ();
  
  // sets or clears a flag at the given flag cursor
  casacore::Bool setFlag      ( casacore::uInt ich,casacore::uInt ifr,FlagCubeIterator &iter );
  casacore::Bool clearFlag    ( casacore::uInt ich,casacore::uInt ifr,FlagCubeIterator &iter );

  // Gets full flag word at the given flag cursor.
  RFlagWord getFlag ( casacore::uInt ich,casacore::uInt ifr,FlagCubeIterator &iter );

  // Versions of above that use global flag cursor
  casacore::Bool setFlag      ( casacore::uInt ich,casacore::uInt ifr );
  casacore::Bool clearFlag    ( casacore::uInt ich,casacore::uInt ifr );
  RFlagWord getFlag ( casacore::uInt ich,casacore::uInt ifr );
  
  // the preFlagged() function uses the corr-mask to tell if any of this
  // agent's correlations are pre-flagged. Uses internal cursor.
  casacore::Bool preFlagged   ( casacore::uInt ich,casacore::uInt ifr );

  // The anyFlagged() uses the corr-flagmask to tell if any of my
  // correlations are flagged either by any agent or pre-flagged
  // Uses internal cursor.
  casacore::Bool anyFlagged   ( casacore::uInt ich,casacore::uInt ifr );
  
  // Sets or clears a row flag
  casacore::Bool setRowFlag      ( casacore::uInt ifr,casacore::uInt itime );
  casacore::Bool clearRowFlag    ( casacore::uInt ifr,casacore::uInt itime );

  // Gets full row flag word
  RFlagWord getRowFlag ( casacore::uInt ifr,casacore::uInt itime );
  
  // tells if a row is pre-flagged in the casacore::MS (or does not exist)
  casacore::Bool rowPreFlagged   ( casacore::uInt ifr,casacore::uInt itime );  

  // tells if a row is flagged by any agent
  casacore::Bool rowAgentFlagged ( casacore::uInt ifr,casacore::uInt itime );  

  // preFlagged OR agentFlagged  
  casacore::Bool rowFlagged      ( casacore::uInt ifr,casacore::uInt itime );
  
  // returns reference to internal iterator
  FlagCubeIterator &  iterator ();
  
  // returns flag mask for this agent
  RFlagWord flagMask ();      

  // returns correlations mask for this agent
  RFlagWord corrMask ();

  // returns the checked-correlations mask for this agent
  // (=0 for RESET/IGNORE policies, or =corrMask() for HONOR policy).
  RFlagWord checkCorrMask ();

  // returns mask of all correlations
  static RFlagWord fullCorrMask ();

  // returns the number of instances of the flag cube
  static casacore::Int numInstances ();

  // sets the maximum memory usage for the flag cube  
  static void setMaxMem ( casacore::Int maxmem );
  // returns the current maximum memory usage
  static int  getMaxMem ();
      
 private:
  RFChunkStats &chunk;                  // chunk

  bool kiss;  // do things simpler (faster) if there is nothing but RFAselector agents
  bool kiss_flagrow;

  static casacore::Cube<casacore::Bool> in_flags;
  static int in_flags_time;  //time stamp that in_flags has reached
  static bool in_flags_flushed; // do we need to write the flags back for this time stamp?

  // shortcut to RFChunkStats::num
  casacore::uInt num ( StatEnums which ) { return chunk.num(which); }
      
  static RFCubeLattice<RFlagWord> flag; // global flag lattice
  static FlagMatrix flagrow;             // (nIfr,nTime) matrix of row flags
  static casacore::Matrix<std::vector<bool> > flagrow_kiss;
  static casacore::Int pos_get_flag,pos_set_flag; 

  static casacore::Bool reset_preflags; // flag: RESET policy specified for at least one instance
  
  static casacore::uInt npol,nchan;
  
  // Flag mask used by this instance. Each instance has a unique 1-bit mask.
  // This is assigned automatically in the constructor, by updating the 
  // instance count and the nextmask member.
  // Note that the low N bits of a mask are assigned to pre-flags (one per
  // each correlation in the casacore::MS); so the agents start at bit N+1.
  RFlagWord flagmask,       // flagmask of this instance
    corrmask,        // corrmask of this instance (corrs used/flagged by it)
    check_corrmask,  // mask checked by preFlagged() & co. Set to 0 for
    // RESET or IGNORE policy, or to corrmask for HONOR
    check_rowmask,   // same for row flags: 0 or RowFlagged
    my_corrflagmask; // see above
  unsigned long flagmask_kiss; // represents a bitmask with only bit number <n> set where 
                          // <n> is the value of this variable
  static casacore::Int agent_count;    // # of agents instantiated
  static RFlagWord base_flagmask, // flagmask of first agent instance
    full_corrmask;          // bitmask for all correlations in casacore::MS (low N bits)

  // corr_flagmask is a mapping from corrmasks into masks of agents that flag the
  // given corrmask
  static casacore::Vector<RFlagWord> corr_flagmask;
  
  // log sink
  casacore::LogIO os;

  // pre-flag policy (can be set on a per-instance basis)
  PreFlagPolicy pfpolicy;
  
  // flagging stats for this instance
  casacore::uInt tot_fl_raised,fl_raised,fl_cleared,
    tot_row_fl_raised,row_fl_raised,row_fl_cleared;
    
  // local flag cursor used by this instance (setFlag and clearFlag). 
  // Normally, set to flag.cursor() in advance(), but can be overridden
  // by setFlagCursor();
  FlagMatrix * flag_curs;
  casacore::uInt flag_itime;
  
  // number of instances in use
  static casacore::Int num_inst;
};

inline RFlagWord RFFlagCube::flagMask ()
  { 
     if (kiss) {
       throw std::logic_error("Cannot do this in kiss mode (program bug, please report)");
     }
     return flagmask; 
  }
 
inline RFlagWord RFFlagCube::corrMask ()
   { 
     return corrmask; 
   }

inline RFlagWord RFFlagCube::checkCorrMask ()
   { return check_corrmask; }

inline RFlagWord RFFlagCube::fullCorrMask ()
   { return full_corrmask; }

inline RFlagWord RFFlagCube::getFlag ( casacore::uInt ich,casacore::uInt ifr,FlagCubeIterator &iter )
   { 
     if (kiss) {
       /* Create the bitmap (integer) from the correlation flags
          relevant for this agent */
       RFlagWord f = 0;
       casacore::uInt c = 1;

       for (casacore::uInt icorr = 0; icorr < num(CORR); icorr++, c<<=1) {
         if ((c & corrmask) && 
             in_flags(icorr, ich, ifr)) {
           f |= c;
         }
       }
       return f;
     }
     else {
       return (iter)(ich,ifr); 
     }
   }

inline casacore::Bool RFFlagCube::setFlag ( casacore::uInt ich,casacore::uInt ifr ) 
   { return setFlag(ich,ifr,flag.iterator()); } 

inline casacore::Bool RFFlagCube::clearFlag ( casacore::uInt ich,casacore::uInt ifr ) 
   { return clearFlag(ich,ifr,flag.iterator()); } 

inline RFlagWord RFFlagCube::getFlag ( casacore::uInt ich,casacore::uInt ifr ) 
   { return getFlag(ich,ifr,flag.iterator()); } 

inline FlagCubeIterator RFFlagCube::newCustomIter ()
   { return flag.newIter(); }

inline const FlagMatrix & RFFlagCube::flagMatrix ()
   { return *flag_curs; }

inline casacore::Bool RFFlagCube::preFlagged ( casacore::uInt ich,casacore::uInt ifr )
   { return (getFlag(ich,ifr)&check_corrmask) != 0; }

inline casacore::Bool RFFlagCube::anyFlagged ( casacore::uInt ich,casacore::uInt ifr )
   { 
     if (kiss) {
       throw std::logic_error("Cannot do this in kiss mode (program bug, please report)");
     }
     return (getFlag(ich,ifr)&(check_corrmask|my_corrflagmask)) != 0;
   }

// Gets full row flag word
inline RFlagWord RFFlagCube::getRowFlag ( casacore::uInt ifr,casacore::uInt itime )
  {
    if (kiss) {
      throw std::logic_error("Cannot do this in kiss mode (program bug, please report)");
    }
    return flagrow(ifr,itime); 
  }

// tells if a row is pre-flagged in the casacore::MS (or does not exist)
inline casacore::Bool RFFlagCube::rowPreFlagged   ( casacore::uInt ifr,casacore::uInt itime )
   { return getRowFlag(ifr,itime)&check_rowmask; }

// tells if a row is flagged by any agent
inline casacore::Bool RFFlagCube::rowAgentFlagged ( casacore::uInt ifr,casacore::uInt itime )
   { return getRowFlag(ifr,itime)&~(RowFlagged|RowAbsent); }

// preFlagged OR agentFlagged  
inline casacore::Bool RFFlagCube::rowFlagged      ( casacore::uInt ifr,casacore::uInt itime )
   { return getRowFlag(ifr,itime)&(check_rowmask?~0:~RowFlagged); }

inline FlagCubeIterator & RFFlagCube::iterator ()
   { return flag.iterator(); }

inline int RFFlagCube::numInstances ()
   { return num_inst; }

inline casacore::LogIO & RFFlagCube::logSink ()
   { return os; }


} //# NAMESPACE CASA - END

#ifndef AIPS_NO_TEMPLATE_SRC
#include <flagging/Flagging/RFFlagCube.tcc>
#endif //# AIPS_NO_TEMPLATE_SRC
#endif
