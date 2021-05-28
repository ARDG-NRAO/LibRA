//# RFChunkStats.h: this defines RFChunkStats
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
#ifndef FLAGGING_RFCHUNKSTATS_H
#define FLAGGING_RFCHUNKSTATS_H

#include <measures/Measures/Stokes.h> 
#include <casa/Arrays/Vector.h> 
#include <casa/Arrays/Matrix.h> 
#include <casa/Arrays/Cube.h> 
#include <casa/Containers/Block.h> 
#include <lattices/Lattices/LatticeIterator.h> 
#include <flagging/Flagging/RFCommon.h>

namespace casacore{

class MeasurementSet;
}

namespace casa { //# NAMESPACE CASA - BEGIN

class Flagger;
class VisibilityIterator;
class VisBuffer;

class RFABase;
typedef RFABase RFA;

// <summary>
// RFChunkStats: vital information and flagging stats for a visibility chunk
// </summary>

// <use visibility=local>

// <reviewed reviewer="" date="" tests="" demos="">
// </reviewed>

// <prerequisite>
//   <li> VisibilityIterator
//   <li> VisBuffer
//   <li> MeasurementSet
//   <li> Flagger
// </prerequisite>
//
// <etymology>
// </etymology>
//
// <synopsis>
// RFChunkStats maintains various stats, derived values, and flagging
// counts for a single visibility chunk. Also serves as an interface to
// VisIter and VisBuffer.
// </synopsis>
//
// <motivation>
// Vital information about an casacore::MS or a visibility chunk is spread all over 
// three classes (casacore::MS, VisIter, VisBuffer). RFChunkStats provides a central
// point for flagging agents to look this info up. 
// </motivation>
//
// <todo asof="2001/04/16">
//   <li> add this feature
//   <li> fix this bug
//   <li> start discussion of this possible extension
// </todo>

class RFChunkStats : public FlaggerEnums
{
protected:
  VisibilityIterator &visiter;
  VisBuffer          &visbuf;
  Flagger            &flagger;
  
  casacore::IPosition visshape;  
  casacore::uInt counts[Num_StatEnums];
  casacore::Matrix<casacore::uInt> nf_ifr_time,nf_chan_ifr;
  casacore::Vector<casacore::uInt> rows_per_ifr;
  casacore::Vector<casacore::uInt> nrf_ifr,nrf_time;
  casacore::Vector<casacore::Int>  ifr_nums;
  casacore::Vector<casacore::Int>  feed_nums;
  casacore::Vector<casacore::Int>  corrtypes;
  casacore::Vector<casacore::Double> freq;
  casacore::String       corr_string;
  casacore::Double       start_time,end_time,current_time;
  casacore::uInt chunk_no,pass_no;
  casacore::Int itime;

//  casacore::Matrix<casacore::uInt> nf_corr_ifr, nf_chan_corr, nf_chan_time, nf_corr_time;
  
  std::vector<double> scan_start;      /* first time stamp in scan */
  std::vector<double> scan_start_flag; /* first time stamp with any 
                                          unflagged data in scan*/
  std::vector<double> scan_end, scan_end_flag; /* as above */
  


public:
// constructor
  RFChunkStats( VisibilityIterator &vi,VisBuffer &vb,Flagger &rf );

// accessors to VisIter
  const VisibilityIterator & visIter () const { return visiter; }
  VisibilityIterator & visIter () { return visiter; }
// accessor to VisBuffer
  VisBuffer &visBuf () { return visbuf; }
// accessor to MS
  const casacore::MeasurementSet & measSet () const;
// accessor to MS
  const casacore::String msName () const;
// returns antenna names
  const casacore::Vector<casacore::String>  & antNames () const;

  // scan start/end times
  double get_scan_start(unsigned scan) const
    { return scan_start[scan]; }

  double get_scan_end(unsigned scan) const
    { return scan_end[scan]; }

  // scan start/end times for unflagged data
  //
  // returns: time stamps of first/last unflagged
  // data in the given scan, or a negative number 
  // if there's no unflagged data in the scan.
  double get_scan_start_unflagged(unsigned scan) const
    { return scan_start_flag[scan]; }

  double get_scan_end_unflagged(unsigned scan) const
    { return scan_end_flag[scan]; }

// loads data for new chunk, resets all flag stat counters
  void newChunk (bool init_quack);
// loads data for new pass
  void newPass (casacore::uInt npass);
// loads data for new iteration
  void newTime ();

// returns current chunk number
  casacore::uInt nchunk() const { return chunk_no; };
// returns current pass number
  casacore::uInt npass()  const { return pass_no; };
// returns current time slot
  casacore::Int  iTime()  const { return itime; };

// returns a data dimension (POL, CHAN, IFR, etc.)
  casacore::uInt num ( StatEnums which ) const { return counts[which]; }
// returns vector of frequencies (one per channel)
  const casacore::Vector<casacore::Double> & frequency ();
  
// returns time of currently iterated time slot
  casacore::Double currentMJD () const     
    { return current_time; }
// return first time slot in chunk
  casacore::Double startMJD () const     
    { return start_time; }
// return last time slot in chunk
  casacore::Double endMJD () const       
    { return end_time; }

// returns corr mask corresponding to specified casacore::Stokes types
// (templated, but only casacore::String and casacore::Int will actually work)
  template<class T> RFlagWord getCorrMask ( const casacore::Vector<T> &corrspec );
  
// returns mask with all correlations
  RFlagWord fullCorrMask () { return (1<<num(CORR))-1; };
// returns string of correlations
  const casacore::String & getCorrString () { return corr_string; }

// returns IFR index corresponding to current VisBuffer rows
  casacore::uInt ifrNum( casacore::uInt nr )  { return ifr_nums(nr); };
// returns IFR index corresponding to current VisBuffer rows
  const casacore::Vector<casacore::Int> & ifrNums ()  { return ifr_nums; };

// returns FEED index corresponding to current VisBuffer rows
  casacore::uInt feedNum( casacore::uInt nr )  { return feed_nums(nr); };
// returns FEED index corresponding to current VisBuffer rows
  const casacore::Vector<casacore::Int> & feedNums ()  { return feed_nums; };

// converts antenna indices into IFR index
  casacore::uInt antToIfr ( casacore::uInt ant1,casacore::uInt ant2 );
// converts IFR index back to antenna numbers
  void ifrToAnt ( casacore::uInt &ant1,casacore::uInt &ant2,casacore::uInt ifr );
// converts IFR index to standard ID string 
  casacore::String ifrString ( casacore::uInt ifr ); 
  
// data availability stats, per IFR
  casacore::uInt nrowPerIfr ( casacore::uInt ifr )            { return rows_per_ifr(ifr); }
  const casacore::Vector<casacore::uInt> & nrowPerIfr () const { return rows_per_ifr; }

// accessors to various flagging stats
  casacore::uInt & nfIfrTime ( casacore::uInt ifr,casacore::uInt itime )  
                                          { return nf_ifr_time(ifr,itime); }
  const casacore::Matrix<casacore::uInt> & nfIfrTime () const    
                                          { return nf_ifr_time; }
  casacore::uInt & nfChanIfr ( casacore::uInt ich,casacore::uInt ifr   )  // flags per channel and ifr
                                          { return nf_chan_ifr(ich,ifr); }
  const casacore::Matrix<casacore::uInt> & nfChanIfr () const   
                                          { return nf_chan_ifr; }        
  casacore::Matrix<casacore::uInt> & nfChanIfr ()
                                          { return nf_chan_ifr; }        
  casacore::uInt & nrfIfr  (casacore::uInt i)                  
                                          { return nrf_ifr(i); }
  casacore::uInt & nrfTime (casacore::uInt i)                  
                                          { return nrf_time(i); }
  const casacore::Vector<casacore::uInt> & nrfIfr  () const     
                                          { return nrf_ifr; };
  const casacore::Vector<casacore::uInt> & nrfTime () const     
                                          { return nrf_time; };

  //casacore::Matrix<casacore::uInt> nf_corr_ifr, nf_chan_corr, nf_chan_time, nf_corr_time;
  //casacore::uInt & nfCorrIfr( casacore::uInt icorr, casacore::uInt ifr ) { return nf_corr_ifr(icorr,ifr); }
  //const casacore::Matrix<casacore::uInt> & nfCorrIfr () const { return nf_corr_ifr; }
  //casacore::uInt & nfChanCorr( casacore::uInt ich, casacore::uInt icorr ) { return nf_chan_corr(ich,icorr); }
  //const casacore::Matrix<casacore::uInt> & nfChanCorr () const { return nf_chan_corr; }
  //casacore::uInt & nfChanTime( casacore::uInt ich, casacore::uInt itime ) { return nf_chan_time(ich,itime); }
  //const casacore::Matrix<casacore::uInt> & nfChanTime () const { return nf_chan_time; }
  //casacore::uInt & nfCorrTime( casacore::uInt icorr, casacore::uInt itime ) { return nf_corr_time(icorr,itime); }
  //const casacore::Matrix<casacore::uInt> & nfCorrTime () const { return nf_corr_time; }
  
// prints stats to stderr
  void printStats ();
};

// enums for which stats are actually collected
const RFChunkStats::StatEnums active_stats[] = { RFChunkStats::CHAN,RFChunkStats::IFR,RFChunkStats::TIME };
const casacore::uInt num_active_stats = 3;

// global function for finding polarization by index
casacore::Int findCorrType( casacore::Stokes::StokesTypes type,const casacore::Vector<casacore::Int> &corr );


} //# NAMESPACE CASA - END

#ifndef AIPS_NO_TEMPLATE_SRC
#include <flagging/Flagging/RFChunkStats.tcc>
#endif //# AIPS_NO_TEMPLATE_SRC
#endif
