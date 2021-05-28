//# RFAFlagExaminer.cc: this defines RFAFlagExaminer
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
#include <flagging/Flagging/RFAFlagExaminer.h>
#include <casa/Exceptions/Error.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/ArrayLogical.h>
#include <casa/Arrays/MaskedArray.h>
#include <casa/Arrays/MaskArrMath.h>
#include <casa/Quanta/Quantum.h>
#include <casa/Quanta/MVTime.h>
#include <casa/Logging/LogIO.h>
#include <msvis/MSVis/VisibilityIterator.h>
#include <msvis/MSVis/VisBuffer.h>
#include <casa/stdio.h>
#include <map>
#include <sstream>
#include <cassert>

namespace casa { //# NAMESPACE CASA - BEGIN

  const casacore::Bool dbg3 = false;
  
  // -----------------------------------------------------------------------
  // RFAFlagExaminer constructor
  // -----------------------------------------------------------------------
  RFAFlagExaminer::RFAFlagExaminer ( RFChunkStats &ch,const casacore::RecordInterface &parm ) : 
    RFASelector(ch, parm)//,RFDataMapper(parm.asArrayString(RF_EXPR),parm.asString(RF_COLUMN))
  {
    if(dbg3)  std::cout << __FILE__ << ":" << __func__ << "():" << __LINE__ << std::endl;
    //desc_str = casacore::String("flagexaminer");
    if(dbg3) std::cout<<"FlagExaminer constructor "<<std::endl;

    totalflags    = accumTotalFlags    = 0;
    totalcount    = accumTotalCount    = 0;
    totalrowflags = accumTotalRowFlags = 0;
    totalrowcount = accumTotalRowCount = 0;
    //parseParm(parm);

    os = casacore::LogIO(casacore::LogOrigin("RFAFlagExaminer", "RFAFlagExaminer", WHERE));

    accumflags.clear();
    accumtotal.clear();


    // Handle in-row selections, the following is a
    // copy-paste from RFASelector2

    char s[256];
    // parse input arguments: channels
    if( parseRange(sel_chan,parm,RF_CHANS)) 
      {
        casacore::String sch;
        for( casacore::uInt i=0; i<sel_chan.ncolumn(); i++) 
          {
            sprintf(s,"%d:%d",sel_chan(0,i),sel_chan(1,i));
            addString(sch,s,",");
          }
        addString(desc_str, casacore::String(RF_CHANS) + "=" +sch);
        sel_chan(sel_chan>=0) += -(casacore::Int)indexingBase();
      }

    // parse input arguments: correlations
    if( fieldType(parm,RF_CORR,casacore::TpString,casacore::TpArrayString))
      {
        casacore::String ss;
        casacore::Vector<casacore::String> scorr( parm.asArrayString(RF_CORR)) ;
        sel_corr.resize( scorr.nelements()) ;
        for( casacore::uInt i=0; i<scorr.nelements(); i++) 
          {
            sel_corr(i) = casacore::Stokes::type( scorr(i)) ;
            if( sel_corr(i) == casacore::Stokes::Undefined) 
              os<<"Illegal correlation "<<scorr(i)<<std::endl<<casacore::LogIO::EXCEPTION;
            addString(ss,scorr(i),",");
          }
        addString(desc_str,casacore::String(RF_CORR)+"="+ss);
      }
  }
  
 
  RFAFlagExaminer::~RFAFlagExaminer()
  {
    if(dbg3)  std::cout << "FlagExaminer destructor " << std::endl;    
  }

  casacore::Bool RFAFlagExaminer::newChunk(casacore::Int &maxmem)
    {
      /* For efficiency reasons, use arrays to collect 
         histogram data for in-row selections
      */
      accumflags_channel = std::vector<casacore::uInt64>(chunk.num(CHAN), 0);
      accumtotal_channel = std::vector<casacore::uInt64>(chunk.num(CHAN), 0);
      accumflags_correlation = std::vector<casacore::uInt64>(chunk.num(CORR), 0);
      accumtotal_correlation = std::vector<casacore::uInt64>(chunk.num(CORR), 0);
          
      return RFASelector::newChunk(maxmem);
    }

  
  void RFAFlagExaminer::initialize()
  {
    if(dbg3)  std::cout << __FILE__ << ":" << __func__ << "():" << __LINE__ << std::endl;

    totalflags    = 0;
    totalcount    = 0;
    totalrowflags = 0;
    totalrowcount = 0;
    inTotalFlags =
	inTotalCount = 
	inTotalRowCount = 
	outTotalFlags =
	outTotalCount = 
	outTotalRowCount = 
	outTotalRowFlags = 0;
  }

  // Is not called if this is the only agent
  void RFAFlagExaminer::finalize()
  {
    //cout << __FILE__ << ":" << __func__ << "():" << __LINE__ << std::endl;

    return;
  }
  // -----------------------------------------------------------------------
  // processRow
  // Raises/clears flags for a single row, depending on current selection
  // -----------------------------------------------------------------------
  void RFAFlagExaminer::processRow(casacore::uInt, casacore::uInt)
  {
      // called often... if(dbg3)  std::cout << __FILE__ << ":" << __func__ << "():" << __LINE__ << std::endl;      
      return;
  }
  
  void RFAFlagExaminer::startFlag (bool verbose)
  {
    if(dbg3)  std::cout << __FILE__ << ":" << __func__ << "():" << __LINE__ << std::endl;

    totalflags    = 0;
    totalcount    = 0;
    totalrowflags = 0;
    totalrowcount = 0;
    
    inTotalFlags = 
	inTotalCount = 
	inTotalRowCount =
	outTotalFlags = 
	outTotalCount =
	outTotalRowCount = 
	outTotalRowFlags = 0;

    RFAFlagCubeBase::startFlag(verbose);

    return;
  }
  
  void RFAFlagExaminer::initializeIter (casacore::uInt) 
  {
      if(dbg3)  std::cout << __FILE__ << ":" << __func__ << "():" << __LINE__ << std::endl;

      for(unsigned ii=0;
	  ii<chunk.visBuf().flagRow().nelements();
	  ii++)
	  if (chunk.visBuf().flagRow()(ii)) {
	      inTotalRowFlags++;
	  }
      
      inTotalRowCount += chunk.visBuf().flagRow().nelements();
      
      for(casacore::Int ii=0;
	  ii<chunk.visBuf().flag().shape()(0);
	  ii++)
	  for(casacore::Int jj=0;
	      jj<chunk.visBuf().flag().shape()(1);
	      jj++)
	      if (chunk.visBuf().flag()(ii,jj)) inTotalFlags++;
  }

  // Is not called if this is the only agent
  void RFAFlagExaminer::finalizeIter (casacore::uInt) 
  {
    //cout << __FILE__ << ":" << __func__ << "():" << __LINE__ << std::endl;

    outTotalRowCount += chunk.visBuf().flagRow().nelements();

    for (unsigned ii = 0;
	 ii < chunk.visBuf().flagRow().nelements();
	 ii++)
	if (chunk.visBuf().flagRow()(ii)) {
	    outTotalRowFlags++;
	}

    for (casacore::Int ii=0;
	 ii<chunk.visBuf().flag().shape()(0);
	 ii++) {

	outTotalCount += chunk.visBuf().flag().shape()(1);
	
	for (casacore::Int jj=0;
	     jj<chunk.visBuf().flag().shape()(1);
	     jj++) {
	    if (chunk.visBuf().flag()(ii,jj)) outTotalFlags++;
	}
    }
  }

  //also need to call RFFlagCube::setMSFlags, which
  // updates some statistics   void RFAFlagExaminer::endRows(casacore::uInt it)

  // it: time index
  void RFAFlagExaminer::iterFlag(casacore::uInt it)
  {
    if(dbg3)  std::cout << __FILE__ << ":" << __func__ << "():" << __LINE__ << std::endl;

    // Set the flags and count them up.
    RFASelector::iterFlag(it);
    
    // count if within specific timeslots
    const casacore::Vector<casacore::Double> &times( chunk.visBuf().time() );
    casacore::Double t0 = times(it);
    
    casacore::Bool within_time_slot = true;
    
    if (sel_time.ncolumn()) {

	if( anyEQ(sel_timerng.row(0) <= t0 && 
		  sel_timerng.row(1) >= t0, true) )
	    within_time_slot = true;
	else within_time_slot = false;
    }
    
    if (within_time_slot) {

	// More counting and fill up final display variables.
	const casacore::Vector<casacore::Int> &ifrs( chunk.ifrNums() );
	const casacore::Vector<casacore::Int> &feeds( chunk.feedNums() );
	const casacore::Vector<casacore::RigidVector<casacore::Double, 3> >&uvw( chunk.visBuf().uvw() );

        unsigned spw = chunk.visBuf().spectralWindow();
        unsigned field = chunk.visBuf().fieldId();
        const casacore::Vector<casacore::Int> &antenna1( chunk.visBuf().antenna1() );
        const casacore::Vector<casacore::Int> &antenna2( chunk.visBuf().antenna2() );
        const casacore::Vector<casacore::Int> &scan    ( chunk.visBuf().scan() );
        const casacore::Vector<casacore::Int> &observation    ( chunk.visBuf().observationId() );
        casacore::Int array    ( chunk.visBuf().arrayId() );

        const casacore::Vector<casacore::String> &antenna_names( chunk.antNames()) ;

	// casacore::Vector<Vector<casacore::Double> > &uvw=NULL;//( chunk.visIter.uvw(uvw) );
	//chunk.visIter().uvw(uvw);
	casacore::Double uvdist=0.0;
	
	// loop over rows
	for (casacore::uInt i=0; i < ifrs.nelements(); i++) {
	    casacore::Bool inrange=false;
	    
	    uvdist = sqrt( uvw(i)(0)*uvw(i)(0) + uvw(i)(1)*uvw(i)(1) );
	    
	    for( casacore::uInt j=0; j<sel_uvrange.ncolumn(); j++) {
		if( uvdist >= sel_uvrange(0, j) &&
		    uvdist <= sel_uvrange(1, j) ) 
		    
		    inrange |= true;
	    }

	    if( (!sel_ifr.nelements() || sel_ifr(ifrs(i))) && 
		(!sel_feed.nelements() || sel_feed(feeds(i))) &&
		(!sel_uvrange.nelements() || inrange ) )
	      {
		// Operate on the chosen row.
		// Collect counts.
		
		//cout << "selected row for " << ifrs(i) << "," << it << std::endl;
		
                if(chunk.nfIfrTime(ifrs(i),it) == chunk.num(CORR)*chunk.num(CHAN))
                    totalrowflags++;

                totalrowcount++;

                casacore::uInt64 f = chunk.nfIfrTime(ifrs(i), it);
                casacore::uInt64 c = chunk.num(CORR) * chunk.num(CHAN);

                // need nfIfrTimeCorr
                // need nfIfrTimeChan

                totalflags += f;
                totalcount += c;

                /* Update histograms */
                
                /* histogram baseline */
                {
                  string baseline = 
                    antenna_names(antenna1(i)) + "&&" +
                    antenna_names(antenna2(i));
                  
                  accumflags["baseline"][baseline] += f;
                  accumtotal["baseline"][baseline] += c;
                }
                
                /* histogram antenna */
                {
                  /* Careful here, update the counts for both
                     antenna1 and antenna2, unless they are the same.
                  */
                  accumflags["antenna"][antenna_names(antenna1(i))] += f;
                  accumtotal["antenna"][antenna_names(antenna1(i))] += c;
                  if (antenna1(i) != antenna2(i)) {
                    accumflags["antenna"][antenna_names(antenna2(i))] += f;
                    accumtotal["antenna"][antenna_names(antenna2(i))] += c;
                  }
                }

                /* histogram spw */
                {
                    std::stringstream spw_string;
                  spw_string << spw;
                  accumflags["spw"][spw_string.str()] += f;
                  accumtotal["spw"][spw_string.str()] += c;
                }

                /* histogram fieldID */
                {
                    std::stringstream fieldID_string;
                  fieldID_string << field;
                  accumflags["field"][fieldID_string.str()] += f;
                  accumtotal["field"][fieldID_string.str()] += c;
                }
                /* histogram scan */
                {
                    std::stringstream scan_string;
                  scan_string << scan(i);
                  accumflags["scan"][scan_string.str()] += f;
                  accumtotal["scan"][scan_string.str()] += c;
                }
                /* histogram observation */
                {
                    std::stringstream observation_string;
                  observation_string << observation(i);
                  accumflags["observation"][observation_string.str()] += f;
                  accumtotal["observation"][observation_string.str()] += c;
                }
                /* histogram array */
                {
                    std::stringstream array_string;
                  array_string << array;
                  accumflags["array"][array_string.str()] += f;
                  accumtotal["array"][array_string.str()] += c;
                }
              }
        }
    }
    
    return;
  }

  /* Update histogram for channel and correlation
     This cannot happen in iterFlag(), which is called once per chunk,
     because the "flag" cursor needs to be updated once per row.
  */
  RFA::IterMode RFAFlagExaminer::iterRow(casacore::uInt irow)
    {
      unsigned ifr = chunk.ifrNum(irow);
      
      for( casacore::uInt ich = 0;
           ich < chunk.num(CHAN); 
           ich++ ) {

        if( !flagchan.nelements() || flagchan(ich) ) {
          
          RFlagWord corrs = flag.getFlag(ich, ifr);
          unsigned n_flags = 0;

          for (casacore::uInt icorr = 0; icorr < chunk.num(CORR); icorr++) {
              if (corrs & 1) {
                  accumflags_correlation[icorr] += 1;
                  n_flags += 1;
              }
              accumtotal_correlation[icorr] += 1;
              corrs >>= 1;
          }

          accumflags_channel[ich] += n_flags;
          accumtotal_channel[ich] += chunk.num(CORR);
        }
      }
      
      return RFA::CONT;
    }

  void RFAFlagExaminer::endChunk ()
    {
        RFASelector::endChunk();

        for (unsigned ich = 0; ich < chunk.num(CHAN); ich++) {
            if (accumtotal_channel[ich] > 0) {
                std::stringstream ss;
                ss << chunk.visIter().spectralWindow() << ":" << ich;
                accumflags["channel"][ss.str()] = accumflags_channel[ich];
                accumtotal["channel"][ss.str()] = accumtotal_channel[ich];
            }
        }

        for (unsigned icorr = 0; icorr < chunk.num(CORR); icorr++) {
            if (accumtotal_correlation[icorr] > 0) {
                std::stringstream ss;
                ss << chunk.visIter().spectralWindow() << ":" << icorr;
                accumflags["correlation"][ss.str()] = accumflags_correlation[icorr];
                accumtotal["correlation"][ss.str()] = accumtotal_correlation[icorr];
            }
        }
    }


  void RFAFlagExaminer::endFlag ()
  {
    if(dbg3)  std::cout << __FILE__ << ":" << __func__ << "():" << __LINE__ << std::endl;
        
    char s[1024];
    sprintf(s,"Chunk %d (field %s, fieldID %d, spw %d)",
     	    chunk.nchunk(),
            chunk.visIter().fieldName().chars(),
            chunk.visIter().fieldId(),
            chunk.visIter().spectralWindow());
    os << "---------------------------------------------------------------------" << casacore::LogIO::POST;
    os<<s<<casacore::LogIO::POST;
    
    sprintf(s, "%s, %d channel%s, %d time slots, %d baselines, %d rows\n",
     	    chunk.getCorrString().chars(),
	    chunk.num(CHAN),
	    chunk.num(CHAN) == 1 ? "" : "s",
	    chunk.num(TIME),
     	    chunk.num(IFR),
	    chunk.num(ROW));
    os << s << casacore::LogIO::POST;
    
    os << "\n\n\nData Selection to examine : " << desc_str ;
    if(flag_everything) os << " all " ;
    os << casacore::LogIO::POST;
    
    casacore::Double ffrac=0.0,rffrac=0.0;
    if(totalcount) ffrac = totalflags*100.0/totalcount;
    if(totalrowcount) rffrac = totalrowflags*100.0/totalrowcount;

    // casacore::LogSink cannot handle uInt64...
    std::stringstream ss;
    ss << totalrowflags << " out of " << totalrowcount <<
	" (" << rffrac << "%) rows are flagged.";
    os << ss.str() << casacore::LogIO::POST;

    ss.str("");

    ss << totalflags << " out of " << totalcount <<
	" (" << ffrac << "%) data points are flagged.\n\n";

    os << ss.str() << casacore::LogIO::POST;
    
    os << "---------------------------------------------------------------------" << casacore::LogIO::POST;
    
    accumTotalFlags    += totalflags;
    accumTotalCount    += totalcount;
    accumTotalRowFlags += totalrowflags;
    accumTotalRowCount += totalrowcount;
    
    return;
  }

  /*
    Return results of this run over all chunks:
    How many flags were set / unset
   */
  casacore::Record RFAFlagExaminer::getResult()
    {
      casacore::Record r;

      r.define("flagged", (casacore::Double) accumTotalFlags);
      r.define("total"  , (casacore::Double) accumTotalCount);


      for (std::map<string, std::map<string, casacore::uInt64> >::iterator j = accumtotal.begin();
           j != accumtotal.end();
           j++) {
        /* Note here: loop over the keys of accumtotal, not accumflags,
           because accumflags may not have all channel keys */
        
          casacore::Record prop;
          for (std::map<string, casacore::uInt64>::const_iterator i = j->second.begin();
               i != j->second.end();
               i++) {
            
              casacore::Record t;

              t.define("flagged", (casacore::Double) accumflags[j->first][i->first]);
              t.define("total", (casacore::Double) i->second);
              
              prop.defineRecord(i->first, t);
          }
          
          r.defineRecord(j->first, prop);
      }
      
      return r;
    }

  
} //# NAMESPACE CASA - END

