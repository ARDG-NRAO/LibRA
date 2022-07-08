//# RFAUVBinner.cc: this defines RFAUVBinner
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
#include <flagging/Flagging/RFAUVBinner.h> 
#include <casacore/casa/BasicMath/Math.h>
#include <casacore/casa/BasicSL/Constants.h>
#include <casacore/casa/Arrays/ArrayMath.h>
#include <casacore/casa/Arrays/MaskArrMath.h>
#include <casacore/casa/Arrays/ArrayLogical.h>
#include <casacore/casa/Arrays/Slice.h>
#include <msvis/MSVis/VisBuffer.h>
#include <casacore/casa/System/PGPlotterInterface.h>
    
#include <casa/stdio.h>
#include <casa/stdlib.h>
    
using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

RFAUVBinner::RFAUVBinner  ( RFChunkStats &ch,const RecordInterface &parm ) : 
    RFAFlagCubeBase(ch,parm),
    RFDataMapper(parm.asArrayString(RF_EXPR),parm.asString(RF_COLUMN)),
    threshold( parm.asDouble(RF_THR) ),
    min_population( parm.asInt(RF_MINPOP) )
//    rowclipper(chunk,flag,threshold,halfwin)
{
// get bin size arguments
  if( isFieldSet(parm,RF_NBINS) ) 
  {
    if( fieldType(parm,RF_NBINS,TpArrayInt) )
    {
      Vector<Int> binsize( parm.asArrayInt(RF_NBINS) );
      nbin_uv = binsize(0);
      nbin_y  = binsize(1);
    } 
    else if( fieldType(parm,RF_NBINS,TpInt) )
    {
      nbin_uv = nbin_y = parm.asInt(RF_NBINS);
    }
  }
// check threshold for validity
  if( threshold >= 1 )
    os<<String("RFAUVBinner: ")+RF_THR+" must be below 1"<<endl<<LogIO::EXCEPTION;
  if( threshold==0 && !min_population )
    os<<String("RFAUVBinner: ")+RF_THR+" and/or "+RF_MINPOP+" must be specified"<<endl<<LogIO::EXCEPTION;
// check if a report is requested for a specific channel
}

uInt RFAUVBinner::estimateMemoryUse () 
{
  return RFAFlagCubeBase::estimateMemoryUse() +
        yvalue.estimateMemoryUse(num(CHAN),num(IFR),num(TIME)) +
        num(IFR)*num(TIME)*sizeof(Float)/(1024*1024) +
        nbin_uv*nbin_y*num(CHAN)*sizeof(Int)/(1024*1024);
}

Bool RFAUVBinner::newChunk (Int &maxmem)
{
// compute correlations mask, return false if fails
  corrmask = RFDataMapper::corrMask(chunk.visIter());
  if( !corrmask )
  {
    os<<LogIO::WARN<<"missing selected correlations, ignoring this chunk\n"<<LogIO::POST;
    return active=false;
  }
// memory management. 
// bin counts are always in memory
  maxmem -= nbin_uv*nbin_y*num(CHAN)*sizeof(Int)/(1024*1024) +
            num(IFR)*num(TIME)*sizeof(Float)/(1024*1024);
// Estimate the max memory use for the lattices, plus a 5% margin  
  Int mmdiff = (Int)(1.05*yvalue.estimateMemoryUse(num(CHAN),num(IFR),num(TIME)));
  // sufficient memory? reserve it
  if( maxmem>mmdiff ) 
    maxmem -= mmdiff;  
  else // insufficient memory: use disk-based lattice
  {
    mmdiff = 0;
    maxmem -= 2; // reserve 2Mb for the iterator anyway
  }
// init flag cube
  RFAFlagCubeBase::newChunk(maxmem);
// create temp lattice for yvalues 
  yvalue.init(num(CHAN),num(IFR),num(TIME),num(CORR),nAgent,mmdiff,2);
// init uvdist matrix
  uvdist.resize(num(IFR),num(TIME));
  uvdist.set(-1);
// init min/max estimates
  ymin.resize(num(CHAN));
  ymax.resize(num(CHAN));
  ymin.set(C::flt_max);
  ymax.set(C::flt_min);
  uvmin.resize(num(CHAN));
  uvmax.resize(num(CHAN));
  uvmin.set(C::flt_max);
  uvmax.set(0);
  binned = false;
// finish with init  
  RFAFlagCubeBase::newChunk(maxmem-=1);
  
  return active=true;
}

void RFAUVBinner::endChunk ()
{
  RFAFlagCubeBase::endChunk();
  yvalue.cleanup();
  uvdist.resize();
  bincounts.resize();
  ymin.resize();
  ymax.resize();
  ybinsize.resize();
  uvmin.resize();
  uvmax.resize();
  uvbinsize.resize();
  totcounts.resize();
//  rowclipper.cleanup();
}

void RFAUVBinner::startData (bool verbose)
{
// reset lattices to write-only
  yvalue.reset(false,true);
  RFAFlagCubeBase::startData(verbose);
//  rowclipper.reset();
}

RFA::IterMode RFAUVBinner::iterTime (uInt it)
{
  yvalue.advance(it);
  RFAFlagCubeBase::iterTime(it);
  RFDataMapper::setVisBuffer(chunk.visBuf());
// get UVW data from VisBuffer
  puvw = & chunk.visBuf().uvw();
  return RFA::CONT;
}

RFA::IterMode RFAUVBinner::iterRow ( uInt irow )
{
  uInt ant1,ant2,ifr;
  chunk.ifrToAnt(ant1,ant2,ifr=chunk.ifrNum(irow));
// skip auto-correlations
  if( ant1==ant2 )
    return RFA::CONT;
// compute UV distance for this row
  Float uv = sqrt(square((*puvw)(irow)(0))+square((*puvw)(irow)(1)));
  uvdist(ifr,yvalue.position()) = uv;
// compute yvalues for every unflagged pixel
  for( uInt ich=0; ich<num(CHAN); ich++ )
  {
    if( flag.preFlagged(ich,ifr) )
      continue;
    // update UV range for this channel
    if( uv < uvmin(ich) )
      uvmin = uv;
    if( uv > uvmax(ich) )
      uvmax = uv;
    // compute y value and update y ranges
    Float yval = mapValue(ich,irow);
    yvalue(ich,ifr) = yval;
    if( yval < ymin(ich) )
      ymin(ich) = yval;
    if( yval > ymax(ich) )
      ymax(ich) = yval;
  }
  return RFA::CONT;
}

RFA::IterMode RFAUVBinner::endData ()
{
// compute bin sizes
  uvbinsize.resize();
  uvbinsize = (uvmax-uvmin)/nbin_uv;
  ybinsize.resize();
  ybinsize = (ymax-ymin)/nbin_y;
  
  RFAFlagCubeBase::endData();
//  uInt dum;
//  rowclipper.updateSigma(dum,dum);
  return RFA::DRY;
}


void RFAUVBinner::startDry (bool verbose)
{
  RFAFlagCubeBase::startDry(verbose);
// reset lattices to read-only
  yvalue.reset(true,false);
// create bincounts cube, if necessary
  if( !binned )
  {
    bincounts.resize();
    bincounts = Cube<Int>(nbin_uv,nbin_y,num(CHAN),0);
    totcounts.resize();
    totcounts = Vector<Int>(num(CHAN),0);
  }
}

IPosition RFAUVBinner::computeBin( Float uv,Float y,uInt ich )
{
  uInt i = (uInt)((uv-uvmin(ich))/uvbinsize(ich));
  uInt j = (uInt)((y -ymin(ich))/ybinsize(ich));
// loss of precision near max values can sometimes put us into bin 
// N+1, so correct for this:
  if( i >= nbin_uv )
    i = nbin_uv-1;
  if( j >= nbin_y )
    j = nbin_y-1;
  return IPosition(3,i,j,ich);
}

RFA::IterMode RFAUVBinner::iterDry (uInt it)
{
  RFAFlagCubeBase::iterDry(it);
  yvalue.advance(it);
// already binned? Do flagging
  if( binned )
  {
    for( uInt ifr=0; ifr<num(IFR); ifr++ )
    {
      Float uv = uvdist(ifr,it);
      if( uv>0 )
      {
        for( uInt ich=0; ich<num(CHAN); ich++ )
        {
          if( !flag.preFlagged(ich,ifr) )
          {
            Int bc = bincounts(computeBin(uv,yvalue(ich,ifr),ich));
            if( bc<0 )
              flag.setFlag(ich,ifr);
            // add point to plot if in low-pop bin
          }
        }
      }
    }
  }
// else compute bins
  else
  {
    for( uInt ifr=0; ifr<num(IFR); ifr++ )
    {
      Float uv = uvdist(ifr,it);
      if( uv>0 )
        for( uInt ich=0; ich<num(CHAN); ich++ )
          if( !flag.preFlagged(ich,ifr) )
          {
            Float y = yvalue(ich,ifr);
            IPosition binpos( computeBin(uv,y,ich) );
            bincounts(binpos)++;
//            bincounts( computeBin(uv,yvalue(ich,ifr),ich) )++;
            totcounts(ich)++;
          }
    }
  }
  return RFA::CONT;
}

RFA::IterMode RFAUVBinner::endDry ()
{
// already binned? then it must have been flagged, so stop
  if( binned )
  {
    return RFA::STOP;
  }
// else compute bad bins
  binned = true;
  for( uInt ich=0; ich<num(CHAN); ich++ )
  {
    // bins for this channel
    Matrix<Int> bins( bincounts.xyPlane(ich) );
    Int maxcount = max(bins);
    Vector<Int> cumul(maxcount+1,0);
    // compute total population for each non-zero count
    // (what we compute is actually the total number of points
    // resident in a bin of size N, that is, N*numbins{population=N})
    for( uInt i=0; i<bins.ncolumn(); i++ )
      for( uInt j=0; j<bins.nrow(); j++ )
        if( bins(j,i) )
          cumul( bins(j,i) ) += bins(j,i);
    // convert to cumul(N): number of points residing in a bin of size<=N
    // (cumul(0) is 0 by definition)
    for( Int i=1; i<=maxcount; i++ )
      cumul(i) += cumul(i-1);
    Int thr_count=0;
    if( threshold>0 )
    {
      // compute threshold based on cumulative counts
      Float pop_cutoff = totcounts(ich)*threshold;
      // find the first bin count value where the cumulative bin population 
      // is higher than the threshold
      while( thr_count<=maxcount && cumul(thr_count)<=pop_cutoff )
        thr_count++; 
    }
    // if the explicit bin cut-off is higher, use it instead
    if( (Int)thr_count < (Int)min_population )
      thr_count = min_population;
    // thr_count is now the first population value exceeding the threshold
    // Bins with populations up to thr_count should be flagged
    LogicalMatrix wh( bins<thr_count );
    bins(wh) = - bins(wh);
  }
// request another dry pass to do the flags
  return RFA::DRY;
}

// -----------------------------------------------------------------------
// RFAUVBinner::getDesc
// Returns description of parameters
// -----------------------------------------------------------------------
String RFAUVBinner::getDesc ()
{
  String desc( RFDataMapper::description()+"; " );
  char s[256];
  if( threshold>0 ) 
  {
    sprintf(s,"%s=%g ",RF_THR,threshold);
    desc += s;
  }
  if( min_population ) 
  {
    sprintf(s,"%s=%d ",RF_MINPOP,min_population );
    desc += s;
  }
  sprintf(s,"%s=%d,%d ",RF_NBINS,nbin_uv,nbin_y);
  desc += s + RFAFlagCubeBase::getDesc();
  return desc;
}

// -----------------------------------------------------------------------
// RFAUVBinner::getDefaults
// Returns record of default parameters
// -----------------------------------------------------------------------
const RecordInterface & RFAUVBinner::getDefaults ()
{
  static Record rec;
// create record description on first entry
  if( !rec.nfields() )
  {
    rec = RFAFlagCubeBase::getDefaults();
    rec.define(RF_NAME,"UVBinner");
    rec.define(RF_COLUMN,"DATA");
    rec.define(RF_EXPR,"+ ABS XX YY");
    rec.define(RF_THR,.001);
    rec.define(RF_MINPOP,0);
    rec.define(RF_NBINS,50);
    
    rec.setComment(RF_COLUMN,"Use column: [DATA|MODEL|CORRected]");
    rec.setComment(RF_EXPR,"Expression for deriving value (e.g. \"ABS XX\", \"+ ABS XX YY\")");
    rec.setComment(RF_THR,"Probability cut-off");
    rec.setComment(RF_MINPOP,"Bin population cut-off");
    rec.setComment(RF_NBINS,"Number of bins (single value, or [NUV,NY])");
  }
  return rec;
}

} //# NAMESPACE CASA - END

