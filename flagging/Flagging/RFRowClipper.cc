//# RFRowClipper.cc: this defines RFRowClipper
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
#include <flagging/Flagging/Flagger.h>
#include <flagging/Flagging/RFFlagCube.h>
#include <flagging/Flagging/RFChunkStats.h>
#include <flagging/Flagging/RFRowClipper.h>
#include <casacore/casa/Arrays/LogiVector.h>
#include <casacore/casa/Arrays/ArrayMath.h>
#include <casacore/casa/Arrays/MaskArrMath.h>
#include <casacore/casa/Arrays/ArrayLogical.h>
#include <casacore/casa/Arrays/Slice.h>
#include <casacore/scimath/Mathematics/MedianSlider.h>
    
using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

RFRowClipper::RFRowClipper( RFChunkStats &ch,RFFlagCube &fl,Float clip,uInt hw,uInt maxp ) :
  chunk(ch),flag(fl),clip_level(clip),halfwin(hw),maxpass(maxp),
  os(fl.logSink())
{}
    
void RFRowClipper::init( uInt ni,uInt nt ) 
{
  sig = Matrix<Float>(ntime=nt,nifr=ni,-1);
  sig0 = Matrix<Float>(nt,ni,-1);
  sigupdated = Vector<Bool>(ni,false);
}
        
void RFRowClipper::cleanup ()
{
  sig.resize();
  sig0.resize();
  sigupdated.resize();
}

void RFRowClipper::reset ()
{
  sigupdated = false;
}

Float RFRowClipper::updateSigma (uInt &ifrmax,uInt &itmax,Bool flag_rows, bool clear_flags )
{
  Vector<Float> medsigma(ntime);
  Vector<Float> diffsigma(ntime);
  Vector<Float> diffs(ntime);
  
  Float dmax=0;
  ifrmax=itmax=0;
  RFlagWord fm = flag.flagMask()|flag.fullCorrMask();

  for( uInt ifr=0; ifr<nifr; ifr++ ) 
  {
    if( sigupdated(ifr) )
    {
      Bool fl;
      Float d;
      Vector<Float> sigma( sig.column(ifr) );
      Bool recalc=true;
      for( uInt ipass=0; ipass<maxpass && recalc; ipass++ ) // loop while some rows are being flagged
      {
        uInt idiff=0;
        recalc=false;
        // Precompute mask of valid sigmas: existing and not flagged
        LogicalVector valid(ntime,true);
        for( uInt i=0; i<ntime; i++ )
          if( sigma(i)<=0 || flag.getRowFlag(ifr,i)&fm )
            valid(i) = false;
        
        // If we have a valid half-window specified, then compute diff WRT
        // to a sliding median. 
        if( halfwin>0 )
        {
          MedianSlider msl(halfwin);
          for( uInt it = 0; it<ntime; it++ ) 
          {
            msl.add( sigma(it), !valid(it) ); 
            if( it>=halfwin )
            {
              medsigma(it-halfwin) = msl.median();
              diffsigma(it-halfwin) = d = abs( msl.diff(fl) );
              if( !fl )
                diffs(idiff++) = d;
            }
          }
          for( uInt it=ntime-halfwin; it<ntime; it++ )
          {
            msl.next(); 
            medsigma(it) = msl.median();
            diffsigma(it) = d = abs(msl.diff(fl));
            if( !fl )
              diffs(idiff++) = d;
          }
        }
        else // No half-window, compute diff WRT global median
        {
          Vector<Float> s;
          s = sigma(valid);
          Float med = median(s);
          medsigma.set(med);
          diffsigma = abs( sigma - med );
          diffs.resize();
          diffs = diffsigma(valid);
          idiff = diffs.nelements();
        }
        if( !idiff ) // no data? go on
          continue;
  // compute threshold, using median of the good datums
        Float meddiff = idiff ? median( diffs( Slice(0,idiff) ) ) : 0;
        Float thr = clip_level*meddiff;
        uInt nbad=0;
        LogicalVector goodsigma( diffsigma<thr );
        for( uInt it=0; it<ntime; it++ )
        {
          Float s=sigma(it);
          if( s>0 )
          {
            // for good rows (or when not using row flagging at all)
            // update stats and clear flags, if needed
            if( !flag_rows || goodsigma(it) ) 
            {
              Bool res = false;
              if( flag_rows && clear_flags ) // clear row flag
              {
                recalc |= ( res = flag.clearRowFlag(ifr,it) );
                for( uInt ich=0; ich<chunk.num(CHAN); ich++ )
                        flag.clearFlag(ich,ifr);
              }
              Float s0 = sig0(it,ifr),
                    m = max(s,s0),
                    d= m!=0 ? abs(s-s0)/m : 0;
              if( d>dmax )  // compute new max difference in sigma
              {
                dmax=d;
                ifrmax=ifr; itmax=it;
              }
            }
            else   // set flags on apparently bad rows
            {
              Bool res = flag.setRowFlag(ifr,it);
              recalc |= res;
              for( uInt ich=0; ich<chunk.num(CHAN); ich++ )
                        flag.setFlag(ich,ifr);
              nbad++;
            }
          }
          else // ignore rows that are apriori bad/nonexistent
          {
          }
        }
        String ifrid( chunk.ifrString(ifr) );
//        dprintf(os,"IFR %d (%s): %d rows flagged, recalc=%d\n",ifr,ifrid.chars(),nbad,(Int)recalc);
      } // endwhile(recalc)
    } // endif( sigupated(ifr) )
  } // endfor( ifr )

  sig0 = sig;
      
//  dprintf(os,"Max diff (%f) at ifr %d (%s), it %d: new sigma is %f\n",
//      dmax,ifrmax,chunk.ifrString(ifrmax).chars(),itmax,sig0(itmax,ifrmax));

  return dmax;
}

} //# NAMESPACE CASA - END

