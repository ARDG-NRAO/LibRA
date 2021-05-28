
//# RFFlagCube.cc: this defines RFFlagCube
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
#include <flagging/Flagging/RFFlagCube.h>
#include <casa/Exceptions/Error.h>
#include <msvis/MSVis/VisBuffer.h>
#include <casa/Arrays/ArrayLogical.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/LogiVector.h>
#include <casa/Utilities/Regex.h>
#include <casa/OS/Time.h>
#include <casa/Quanta/MVTime.h>
#include <memory>
#include <stdio.h>

using namespace std;
        
using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

const Bool dbg=false;
const Bool mdbg=false;
const Bool verbose=false;
        
RFCubeLattice<RFlagWord> RFFlagCube::flag; // global flag lattice
Cube<Bool> RFFlagCube::in_flags;  //global flag array (kiss mode)
int RFFlagCube::in_flags_time;
bool RFFlagCube::in_flags_flushed;

FlagMatrix RFFlagCube::flagrow;     // this data type supports only up to 32 agents (bad design)
Matrix<vector<bool> > RFFlagCube::flagrow_kiss;  // in kiss mode, support more agents
Int RFFlagCube::pos_get_flag=-1,RFFlagCube::pos_set_flag=-1;

RFlagWord RFFlagCube::base_flagmask=1,
          RFFlagCube::full_corrmask;
Int RFFlagCube::agent_count=0, RFFlagCube::num_inst=0;
Vector<RFlagWord> RFFlagCube::corr_flagmask;  
//This is a map from a set of correlations to a set of agents, i.e.
// which agents deal with any of the given correlations

Bool RFFlagCube::reset_preflags;
  LogIO RFFlagCube::default_sink(LogOrigin("Flagger","FlagCube"));

RFFlagCube::RFFlagCube ( RFChunkStats &ch,Bool ignore,Bool reset,LogIO &sink )
  : chunk(ch),os(sink)
{
  num_inst++;
  if( reset )
  {
    pfpolicy = FL_RESET;
    if(verbose) os<<"Existing flags will be reset\n"<<LogIO::POST;
  }
  else
  {
    if( ignore )
    {
      pfpolicy = FL_IGNORE;
      if(verbose) os<<"Existing flags will be ignored, but added to\n"<<LogIO::POST;
    }
    else
    {
      pfpolicy = FL_HONOR;
      if(verbose) os<<"Existing flags will be honored\n"<<LogIO::POST;
    }
  }
}

RFFlagCube::~RFFlagCube ()
{
    in_flags.resize(0, 0, 0);
    in_flags_time = -1;
    in_flags_flushed = false;
    num_inst--;
    if (num_inst == 0) {
        cleanup();
    }
}

uInt RFFlagCube::estimateMemoryUse ( const RFChunkStats &ch )
{
    return
        ch.num(CHAN) * ch.num(IFR) * ch.num(TIME) * 
        sizeof(RFlagWord) / (1024*1024) + 1;
    
}

// creates flag cube for a given visibility chunk
void RFFlagCube::init( RFlagWord corrmsk, uInt nAgent, bool only_selector, const String &name) 
{
    if (dbg) cout << "name=" << name << endl;

    kiss = only_selector; /* Use a Cube<Bool> instead of the
                             expensive flag lattice in this case */

    /* Using the 'flagrow_kiss' buffer instead of 'flagrow'
       allows to have more than 32 agents+correlations, but at a
       (small, maybe insignificant 1m38s vs 1m32s) runtime cost. 
       Therefore use it only when necessary.
     */
    kiss_flagrow = (kiss && nAgent + num(CORR) + 1 > sizeof(RFlagWord)*8);

    // In order to use flagrow_kiss whenever possible (for testing), define here kiss_flagrow = kiss;

    // setup some masks
    corrmask = corrmsk;
    check_corrmask = (pfpolicy == FL_HONOR) ? corrmsk : 0;
    check_rowmask  = (pfpolicy == FL_HONOR) ? RowFlagged : 0;

    // clear stats  
    tot_fl_raised=tot_row_fl_raised=fl_raised=fl_cleared=
	row_fl_raised=row_fl_cleared=0;
    
    // init flag cube if it is empty
    if ( !flag.shape().nelements() ) {
    
	reset_preflags=false;

	// setup correlation masks. The first NCORR bits of the flag word
	// are used to store the apriori flags. Basemask is the first bitmask
	// actually used for flagging
	base_flagmask = num(CORR) >= 2 ? 1 << num(CORR) : 4;  // is this a bug, should base_flagmask be 2 if num(CORR) is 1?

	// full_corrmask is the mask of all correlations flagged
	full_corrmask = (1<<num(CORR))-1;

        if (!kiss) {
            // init empty flag lattice
            // initial state is all pre-flags set; we'll clear them as we go along
            flag.init(num(CHAN),num(IFR),num(TIME),num(CORR), nAgent, full_corrmask);

        }
        else {
            /* Set shape to a dummy value, 
               only so that we can later use flag.shape().nelements()
               in the if conditions.
            */
            flag.shape().resize(1);
            in_flags_time = -1;

        }

        if (!kiss_flagrow) {
            // allocate cube of row flags
            flagrow.resize(num(IFR),num(TIME));
            flagrow = RowFlagged|RowAbsent; // 0000 0011

            corr_flagmask.resize(1<<num(CORR));
            corr_flagmask = 0;
        }
        else {
            flagrow_kiss.resize(num(IFR),num(TIME));
            unsigned long val = (RowFlagged|RowAbsent);
            flagrow_kiss = bitvec_from_ulong( val, (num(CORR) >= 2 ? num(CORR) : 2) + nAgent );
        }

	pos_get_flag = pos_set_flag = -1;

	// reset instance counters 
	agent_count = 0;
    }
    if (kiss_flagrow) {
        // basebit plus agent_count
        flagmask_kiss = (num(CORR) >= 2 ? num(CORR) : 2) + agent_count;

    }
    else {
        flagmask = base_flagmask << agent_count;
        if (dbg) cout << "agent_count=" << agent_count 
                      << " base_flagmask=" << base_flagmask 
                      << " flagmask=" << (flagmask > UINT_MAX) << endl;
        if( !flagmask  )
            throw(AipsError("Too many flagging agents instantiated"));
    }
    agent_count++;

    // raise flag if any one instance has a RESET pre-flag policy
    if ( pfpolicy==FL_RESET )
	reset_preflags=true;

    // set bits in corr_flagmask, not used in kiss mode
    if (!kiss_flagrow) {
        for ( uInt cm=0; cm < corr_flagmask.nelements(); cm++ )
            if ( cm & corrmask )
                corr_flagmask(cm) |= flagmask;
    }
  
    if(dbg) cout << "End of init. reset_preflags : " << reset_preflags << endl;
}

// deallocates flag cube
void RFFlagCube::cleanup ()
{
    if (flag.shape().nelements()) {
        flag.cleanup();
        if (!kiss_flagrow) {
            flagrow.resize(0,0);
            corr_flagmask.resize(0);
        }
        else {
            flagrow_kiss.resize(0, 0);
        }
        agent_count=0;
    }
}

void RFFlagCube::reset ()
{
    fl_raised=fl_cleared=row_fl_raised=row_fl_cleared=0;
    if (!kiss_flagrow) {
        my_corrflagmask = corr_flagmask(corrmask);
    }
    if (!kiss) {
        flag.reset();
    }

    return;
}

String RFFlagCube::getSummary ()
{
  char s[128];
  sprintf(s, "%d pixel flags, %d row flags",
	  tot_fl_raised,tot_row_fl_raised);
  return s;
}

// prints flagging statistics
void RFFlagCube::printStats ()
{
  if( tot_fl_raised )
    dprintf(os,"%d pixel flags; %d raised this pass, %d cleared\n",
        tot_fl_raised,fl_raised,fl_cleared);
  if( tot_row_fl_raised )
    dprintf(os,"%d row flags; %d raised this pass, %d cleared\n",
        tot_row_fl_raised,row_fl_raised,row_fl_cleared);
}

// Sets flag at (ich, ifr) to 1. Returns true if the flag has not been raised
// previously.
Bool RFFlagCube::setFlag ( uInt ich,uInt ifr, FlagCubeIterator &iter )
{
    if (kiss) {
        uInt c = 1;
        bool raised = false;
        for (uInt icorr = 0; icorr < num(CORR); icorr++, c<<=1) {
            if (c & corrmask) {
                if (!in_flags(icorr, ich, ifr)) {
                    raised = true;
                }
                in_flags(icorr, ich, ifr) = 1;
            }
        }
        if (raised) {
            tot_fl_raised++;
            fl_raised++;
        }
        return raised;
    }

    if (dbg) cerr << "flag for " << ich << "," << ifr << "corrmask = " << corrmask;

    RFlagWord oldfl = iter(ich,ifr);
    if (dbg) cerr << " : " << oldfl << "," << flagmask;
    if ( !(oldfl&flagmask) ) {
	tot_fl_raised++;
	fl_raised++;
	if (dbg) cerr << " setting " << oldfl << " | " << flagmask << endl;
	iter.set(ich, ifr, oldfl | flagmask);
	if (dbg) cerr << " -----> true --> " << iter(ich,ifr) << endl;
	return true;
    }
    if (dbg) cerr << " -----> false --> " << iter(ich,ifr) << endl;
    return false;
}

// Clears flag at (ich,iifr). Returns true if flag was up before.
Bool RFFlagCube::clearFlag ( uInt ich,uInt ifr,FlagCubeIterator &iter )
{
    if(dbg) cerr << "unflag for " << ich << "," << ifr;

    if (kiss) {
        
        uInt c = 1;
        bool cleared = false;
        for (uInt icorr = 0; icorr < num(CORR); icorr++, c<<=1) {
            if (c & corrmask) {
                if (in_flags(icorr, ich, ifr)) {
                    cleared = true;
                    in_flags(icorr, ich, ifr) = 0;
                }
            }
        }
        if (cleared) {
            tot_fl_raised--;
            fl_cleared++;
        }
        return cleared;
    }

    RFlagWord oldfl = iter(ich,ifr);
    if(dbg)cerr << " : " << oldfl << "," << flagmask;

    // all flags cleared for this point - update global stats
    
    if( !(oldfl&flagmask) ) {
        tot_fl_raised--;
        fl_cleared++;
        iter.set(ich, ifr, oldfl & flagmask);
        if(dbg)cerr << " -----> true --> " << iter(ich,ifr) << endl;
        return true;
    }
    if(dbg)cerr << " -----> false --> " << iter(ich,ifr) << endl;
    return false;
}

// Sets flag at (ifr, itime). Returns true if flag has not been raised
// previously.
Bool RFFlagCube::setRowFlag ( uInt ifr, uInt itime )
{
    if (kiss_flagrow){

        const std::vector<bool> &oldfl = flagrow_kiss(ifr, itime);

        if ( ! oldfl[flagmask_kiss] ) {
            tot_row_fl_raised++;
            row_fl_raised++;

            flagrow_kiss(ifr, itime)[flagmask_kiss] = true;

            return true;
        }
        return false;
    }
    else {
        RFlagWord oldfl = flagrow(ifr,itime);
        
        // first flag raised for this row - update global stats
        if ( !(oldfl&flagmask) )
            {
                tot_row_fl_raised++;
                row_fl_raised++;
                flagrow(ifr, itime) = oldfl | flagmask;
                return true;
            }
        return false;
    }
}

// Clears row flag for (ifr, itime). Returns true if flag was up before.
Bool RFFlagCube::clearRowFlag ( uInt ifr,uInt itime )
{
    if (kiss_flagrow){

        const std::vector<bool> &oldfl = flagrow_kiss(ifr, itime);

        if ( oldfl[flagmask_kiss] ) {
            tot_row_fl_raised--;
            row_fl_raised++;

            flagrow_kiss(ifr, itime)[flagmask_kiss] = false;

            return true;
        }
        return false;
    }   
    else {
        RFlagWord oldfl = flagrow(ifr, itime);
        
        if (oldfl & flagmask)  {
            tot_row_fl_raised--;
            row_fl_cleared++;
            flagrow(ifr,itime) = oldfl & (~flagmask);
            return true;
        }
        return false;
    }
}

// Advances the global flag lattice iterator to the specified time.
// If pfr and pfc are specified, fills in data
void RFFlagCube::advance( uInt it,Bool getFlags )
{
  if( !kiss && flag.position() != (Int)it )
    flag.advance(it);
  if( getFlags )
    getMSFlags(it);
  return;
}

// Fills lattice with apriori flags (from VisBuffer in ChunkStats)
void RFFlagCube::getMSFlags(uInt it)
{
  // return if already filled at this iterator position
  if( !kiss ) {
      if (pos_get_flag >= flag.position() )
          return;
      
      pos_get_flag = flag.position();
  }
  else {
      pos_get_flag = it;
      if (in_flags_time == (int)it) {
          return;
      }
      in_flags_time = it;
      in_flags_flushed = false;
  }
  
  unique_ptr<FlagVector> fl_row;
  FlagVector *flr = NULL;

  //  FlagVector fl_row;//(flagrow.column(pos_get_flag));
  if (!kiss) {
      fl_row = unique_ptr<FlagVector>(new FlagVector(flagrow.column(pos_get_flag)));
      flr = fl_row.get();
  }

  const Vector<Bool> & fr( chunk.visBuf().flagRow() );

  in_flags.resize(num(CORR), num(CHAN), num(IFR));
    
  if( reset_preflags ) // RESET pre-flag policy: reset the flags
  {
    for( uInt i=0; i<fr.nelements(); i++ )
    {
      uInt ifr = chunk.ifrNum(i);

      if (!kiss) {
          // clear row flag
          (*flr)(ifr) &= ~(RowAbsent|RowFlagged); // 0000 0011 & 1111 1100 = 0000 0000
          // clear pixel flags
          flag.set_column(ifr, 0); // 0000 0000
      }

      for (uInt ichan = 0; ichan < num(CHAN); ichan++) {
          for (uInt icorr = 0; icorr < num(CORR); icorr++) {
              in_flags(icorr, ichan, ifr) = 0;
          }
      }
    }
  }
  else // HONOR/IGNORE policy: faithfully copy flags from FLAG and FLAG_ROW
  {
    const Cube<Bool>   & fc( chunk.visBuf().flagCube() );

    if (fc.shape()(0) != (Int) num(CORR) ||
	fc.shape()(1) != (Int) num(CHAN))
      {
	stringstream ss;
	ss << "The shape of FLAGs has changed (from (" << num(CORR) << ", " << num(CHAN)
	   << ") to (" << fc.shape()(0) << ", " << fc.shape()(1) << ")) within the same chunk. "
	   << "Invalid MS.";
	throw AipsError(ss.str());
      }

    Bool deleteIn, deleteFc;
    Bool *inp = in_flags.getStorage(deleteIn);
    const Bool *fcp = fc.getStorage(deleteFc);

    for( uInt i=0; i < fr.nelements(); i++ )
    {
      uInt ifr = chunk.ifrNum(i);

      if (fr(i)) {
          unsigned n = num(CHAN)*num(CORR);
          for (unsigned j = 0; j < n; j++) {
              inp[j + ifr * n] = 1; 
          }
      }
      else {
          unsigned n = num(CORR) * num(CHAN);
          for (unsigned j = 0; j < n; j++) {
              inp[j + n * ifr] = fcp[j + n * i];
          }
      }

      if (!kiss) {
          (*flr)(ifr) &= ~RowAbsent; // 0000 0011 & 11111101 = 0000 0001
          // initial state of lattice is all correlations flagged, so we just
          // ignore flagged rows
          //if( !fr(i) )  // row not flagged, or we ignore/reset flags
          //{
          // clear row flag in internal matrix, if needed
          if( !fr(i) ) 
              (*flr)(ifr) &= ~RowFlagged; // 0000 0001 & 1111 1110 -> 0000 0000
          /* clear all row flags...so that only new flags are true at the end */
          
          ///... read in chan flags for all rows......
          ///...  because all may need to be written back.
          
          
          /* The lattice was initialized to all flags set,
             Now clear as appropriate (if not FLAG_ROW and not FLAG)
          */
          if (num(CORR) == 1) {
              for (uInt ich=0; ich<num(CHAN); ich++ ) {
                  if( !(*flr)(ifr) && !fc(0, ich, i) ) {
                      flag.set(ich, ifr, 0, 0);
                  }
              }
          }
          else {
              for (uInt ich=0; ich<num(CHAN); ich++ ) {
                  for (uInt icorr=0; icorr<num(CORR); icorr++ ) {
                      
                      if( !(*flr)(ifr) && !fc(icorr, ich, i) ) {
                          //(*flag.cursor())(ich,ifr) &= ~(1<<icorr); 
                          
                          flag.set(ich, ifr, icorr, 0);
                      }
                  }
              }
          }
      }
    }
    in_flags.putStorage(inp, deleteIn);
    fc.freeStorage(fcp, deleteFc);
  }
}


// Moves flags from lattice to VisBuffer
// ifrnums is a vector of IFR indices (derived from antenna indices)
void RFFlagCube::setMSFlags(uInt itime)
{
    //    cerr << itime << " write to visIter" << endl;
  //jmlarsen: This function should probably use the flagrow member variable.

    if(mdbg) 
    {
      cerr << "RFFlagCube :: setMSFlags for " ;
      cerr << "itime : " << itime << endl;
    }

  // return if already done at this iterator position
  if (!kiss) {
      if( flag.position() <= pos_set_flag )
          return;
      
      pos_set_flag = flag.position();
  }
  else {
      if (in_flags_flushed) {
          return;
      }
      else {
          in_flags_flushed = true;
      }
  }

  uInt nr = chunk.visBuf().nRow();
  Vector<Bool> out_flagrow( nr,false );
  Cube<Bool>   out_flagcube( num(CORR),num(CHAN),nr,false );

  chunk.nrfTime(itime) = 0;

  Bool deleteOut, deleteIn;
  Bool *outp = out_flagcube.getStorage(deleteOut);
  const Bool *inp = in_flags.getStorage(deleteIn);

  Bool deleteNfChanIfr;
  uInt *nfChanIfrp = chunk.nfChanIfr().getStorage(deleteNfChanIfr);

  unsigned ncorr = num(CORR);
  unsigned nchan = num(CHAN);

  for( uInt ir=0; ir<nr; ir++ )
  {
      uInt ifr = chunk.ifrNum(ir);

      chunk.nrfIfr(ifr) = 0;

      if (dbg) cerr << "  at " << __FILE__ << " " << __func__ << " " << __LINE__ << " " << __LINE__ << out_flagrow(ir) << endl;
      
      // Set data flags
      unsigned n = nchan * ncorr;
      unsigned iout = n*ir;
      unsigned iin = n*ifr;
      unsigned iChanIfr = nchan * ifr;
      uInt &iNfIfrTime = chunk.nfIfrTime(ifr, itime);
      iNfIfrTime = 0;
      for( uInt ich=0; ich < nchan; ich++, iChanIfr++) {
          nfChanIfrp[iChanIfr] = 0;
      }

      iChanIfr = nchan * ifr;
      for( uInt ich=0; ich < nchan; ich++, iChanIfr++) {

          if (kiss) {

              if (ncorr == 1) {
            	  /* TODO assignment correct? */
                  if ((outp[iout++] = inp[iin++])) {
                      nfChanIfrp[iChanIfr]++;
                      iNfIfrTime++;
                  }
              }
              else {
                  for( uInt icorr = 0; icorr < ncorr; icorr++, iout++, iin++) {
                	  /* TODO assignment correct? */
                      if ((outp[iout] = inp[iin])) {
                          nfChanIfrp[iChanIfr]++;
                          iNfIfrTime++;
                      }
                  }
              }
          } else {

              RFlagWord fw = flag(ich, ifr);
              
              if (fw) {
                  // if anything was raised for this channel
                  
                  // loop over correlations and see which are (a) preflagged
                  // (b) been flagged by agents. 
                  RFlagWord cmask = 1;
                  if (num(CORR) == 1) {
                      
                      if ((fw & 1) || (fw & corr_flagmask(1))) {
                          out_flagcube(0, ich, ir) = true;
                          chunk.nfChanIfr(ich,ifr)++;
                          chunk.nfIfrTime(ifr,itime)++;
                      }
                  }
                  else {
                      for( uInt  icorr=0; icorr<num(CORR); icorr++, cmask<<=1 ) {
                          
                          if( (fw & cmask)       // (a) if fw is set for this correlation
                              ||
                              (fw & corr_flagmask(cmask)) // (b) if agent flag in fw
                              // is set for any agent that 
                              // deals with this correlation
                              ) {
                              
                              out_flagcube(icorr,ich,ir) = true;
                              chunk.nfChanIfr(ich,ifr)++;
                              chunk.nfIfrTime(ifr,itime)++;
                          }
                      }
                  }
              }
          }
      }
    
      /* if any dataflags have been unflagged, they already are.
	 if any rowflags have been unflagged, this is already in the dataflags too */
      /* if any dataflags have been flagged - this info is there in dataflags.
	 if any rowflags have been flagged, this is also there in dataflags */
      // so make flag_row the AND of the dataflags.

      /* Set flagrow if everything was flagged */

      /* Fill in all the flag counts here */
      // chunk.nf*
      // nrfIfr(ifr), nrfTime(itime), nfIfrTime(ifr,itime), nfChanIfr(ich,ifr)

      if (chunk.nfIfrTime(ifr, itime) == nchan * ncorr) {

          out_flagrow(ir) = true;

          chunk.nrfIfr(ifr)++;
          chunk.nrfTime(itime)++;
      }
  }

  out_flagcube.putStorage(outp, deleteOut);
  in_flags.freeStorage(inp, deleteIn);
  chunk.nfChanIfr().putStorage(nfChanIfrp, deleteNfChanIfr);

  if(mdbg)
      {
          Int cnt1=0,cnt2=0;
          for( uInt ir=0; ir<nr; ir++ )
	      {
                  uInt ifr = chunk.ifrNum(ir);
                  cnt1 += chunk.nrfIfr(ifr);
                  cnt2 += chunk.nfIfrTime(ifr,itime);
                  cerr << "Sum of flagrow (ifr) : " << cnt1 << endl;
                  cerr << "Sum of flags (ifr,itime): " << cnt2 << endl;
	      }
      }

  chunk.visIter().setFlag(out_flagcube);
  chunk.visIter().setFlagRow(out_flagrow);
}

} //# NAMESPACE CASA - END

