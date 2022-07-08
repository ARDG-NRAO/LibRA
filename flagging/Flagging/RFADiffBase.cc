//# RFADiffBase.cc: this defines RFADiffBase
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
#include <flagging/Flagging/RFADiffBase.h>
#include <msvis/MSVis/VisibilityIterator.h>
#include <msvis/MSVis/VisBuffer.h>
#include <casacore/casa/Arrays/ArrayMath.h>
#include <casacore/casa/Arrays/MaskArrMath.h>
#include <casacore/casa/Arrays/ArrayLogical.h>
#include <casacore/casa/Arrays/Slice.h>

#include <casa/stdio.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

Bool RFADiffBase::dummy_Bool;

// -----------------------------------------------------------------------
// RFADiffBase constructor
// Construct from a Record of parameters
// -----------------------------------------------------------------------
RFADiffBase::RFADiffBase (  RFChunkStats &ch,const RecordInterface &parm ) :
  RFAFlagCubeBase(ch,parm),
  clip_level(parm.asDouble(RF_THR)),
  row_clip_level(parm.asDouble(RF_ROW_THR)),
  disable_row_clip(parm.asBool(RF_ROW_DISABLE)),
  rowclipper(chunk,flag,row_clip_level,parm.asInt(RF_ROW_HW))
{
}

// -----------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------
RFADiffBase::~RFADiffBase () 
{
}

// -----------------------------------------------------------------------
// RFADiffBase::getDefaults
// Returns record of default parameters
// -----------------------------------------------------------------------
const RecordInterface & RFADiffBase::getDefaults ()
{
  static Record rec;
// create record description on first entry
  if( !rec.nfields() )
  {
    rec = RFAFlagCubeBase::getDefaults();
    rec.define(RF_THR,(Double)5);
    rec.define(RF_ROW_THR,(Double)10);
    rec.define(RF_ROW_HW,(Int)6);
    rec.define(RF_ROW_DISABLE,false);
    rec.setComment(RF_THR,"Pixel flagging threshold, in AADs");
    rec.setComment(RF_ROW_THR,"Row flagging threshold, in AADs");
    rec.setComment(RF_ROW_HW,"Row flagging, half-window of sliding median");
    rec.setComment(RF_ROW_DISABLE,"Disable row-based flagging");
  }
  return rec;
}

// -----------------------------------------------------------------------
// RFADiffBase::getDesc
// Returns description of parameters
// -----------------------------------------------------------------------
String RFADiffBase::getDesc ()
{
  char s[128];
  if( disable_row_clip )
    sprintf(s,"%s=%.1f",RF_THR,clip_level);
  else
    sprintf(s,"%s=%.1f %s=%.1f",RF_THR,clip_level,RF_ROW_THR,row_clip_level);
  String str(s);
  return str+" "+RFAFlagCubeBase::getDesc();
}

uInt RFADiffBase::estimateMemoryUse ()
{
  return diff.estimateMemoryUse(num(CHAN),num(IFR),num(TIME)) + 
        RFAFlagCubeBase::estimateMemoryUse() + 2;
}
  
// -----------------------------------------------------------------------
// RFADiffBase::newChunk
// Sets up for new chunk of data
// -----------------------------------------------------------------------
Bool RFADiffBase::newChunk (Int &maxmem)
{
// compute correlations mask
  corrmask = newCorrMask();
  if( !corrmask )
  {
    os<<LogIO::WARN<<"missing selected correlations, ignoring this chunk\n"<<LogIO::POST;
    return active=false;
  }
// memory management. Estimate the max memory use for the diff 
// lattice, plus a 5% margin
  Int mmdiff = (Int)(1.05*diff.estimateMemoryUse(num(CHAN),num(IFR),num(TIME)));
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
// create a temp lattice to hold nchan x num(IFR) x ntime diff-medians
  diff.init(num(CHAN),num(IFR),num(TIME),num(CORR), nAgent, mmdiff,2);
// init the row-clipper object
  rowclipper.init(num(IFR),num(TIME));
  diffrow.resize(num(CHAN));
  
// if rows are too short, there's no point in flagging them in toto 
// based on their noise level
  clipping_rows = !disable_row_clip;
  if( num(CHAN)<10 )
    clipping_rows = false;
  
  return active=true;
}

// -----------------------------------------------------------------------
// RFADiffBase::endChunk
// Resets at end of chunk
// -----------------------------------------------------------------------
void RFADiffBase::endChunk ()
{
  RFAFlagCubeBase::endChunk();
  diff.cleanup();
  rowclipper.cleanup();
  diffrow.resize();
}

// -----------------------------------------------------------------------
// RFADiffBase::startData
// Prepares for an data pass over a VisIter chunk
// -----------------------------------------------------------------------
void RFADiffBase::startData (bool verbose)
{
  RFAFlagCubeBase::startData(verbose);
  diff.reset(chunk.npass()>0,true);
  rowclipper.reset();

  pflagiter = &flag.iterator();
}

// -----------------------------------------------------------------------
// RFADiffBase::startDry
// Prepares for an dry pass 
// -----------------------------------------------------------------------
void RFADiffBase::startDry (bool verbose)
{
  RFAFlagCubeBase::startDry(verbose);
  diff.reset(chunk.npass()>0,false);
  rowclipper.reset();
  pflagiter = &flag.iterator();
}


// -----------------------------------------------------------------------
// RFADiffBase::iterTime
// Default version of iter time just keeps the diff and flag lattices
// in sync with the time slot.
// -----------------------------------------------------------------------
RFA::IterMode RFADiffBase::iterTime (uInt it)
{
  RFAFlagCubeBase::iterTime(it);
  diff.advance(it);
  return RFA::CONT;
}

// -----------------------------------------------------------------------
// RFADiffBase::iterDry
// Dry run iterator: recomputes the AAD and does flagging
// -----------------------------------------------------------------------
RFA::IterMode RFADiffBase::iterDry ( uInt it )
{
  RFAFlagCubeBase::iterDry(it);
  diff.advance(it);
  
  for( uInt ifr=0; ifr<num(IFR); ifr++ ) // outer loop over IFRs
  {
    if( flag.rowFlagged(ifr,it) ) // skip if whole row is flagged
    {
      continue;
    }
    Float thr = clip_level*rowclipper.sigma0(ifr,it);
    idiffrow=0;
    Bool updated=false;
    for( uInt ich=0; ich<num(CHAN); ich++ ) // loop over channels
    {
      if( flag.preFlagged(ich,ifr) ) // skip pixel if pre-flagged
        continue;

      Float d = diff(ich,ifr);
      if( d > thr )   // should be clipped?
      {
        Bool res = flag.setFlag(ich,ifr);
        updated |= res;
      }
      else
      {
        diffrow(idiffrow++) = d;
      }
    } // for(ich)
    // update the noise level, if any changes in flags
    if( updated ) 
      rowclipper.setSigma(ifr,it,idiffrow ? median( diffrow(Slice(0,idiffrow)) ) : -1 );
  } // for(ifr)
  return CONT;
}
      
// -----------------------------------------------------------------------
// RFADiffBase::endData
// After a data pass, we always request one more dry pass
// -----------------------------------------------------------------------
RFA::IterMode RFADiffBase::endData ()
{
  RFAFlagCubeBase::endData();
  uInt dum;
  rowclipper.updateSigma(dum, dum, true, false);
  
  return RFA::DRY;
}

// after a dry pass - see if AAD has managed to update itself 
// significantly
RFA::IterMode RFADiffBase::endDry ()
{
  RFAFlagCubeBase::endDry();
// update the reference AAD
  uInt ifrmax,itmax;
  Float dmax =   rowclipper.updateSigma(ifrmax,itmax, true, false);
  
  dprintf(os,"Max diff (%f) at ifr %d (%s), it %d: new sigma is %f\n",
      dmax,ifrmax,chunk.ifrString(ifrmax).chars(),itmax,rowclipper.sigma0(ifrmax,itmax));

// no significant change this pass? Then we're really through with it.
  if( dmax <= RFA_AAD_CHANGE )
    return RFA::STOP;
// else try another dry pass
// NB: perhaps request a data pass, if too many flags?
  return RFA::DRY;
}

void RFADiffBase::startDataRow (uInt)
{
  idiffrow=0;
}

void RFADiffBase::endDataRow (uInt ifr)
{
//  if( !idiffrow )
//    dprintf(os,"No data points at ifr %d\n",ifr);
  Float sigma = idiffrow ? median( diffrow( Slice(0,idiffrow) ) ) : -1;
  uInt it = diff.position();
  rowclipper.setSigma(ifr,it,sigma);
}

// -----------------------------------------------------------------------
// RFADiffBase::setDiff
// Meant to be called during a data pass. Updates the difference 
// lattice; flags things if appropriate.
// This assumes caller has already checked existing pre-flags,
// and that the current data point is NOT flagged.
// Returns the threshold used for flagging, or 0 if no threshold is
// yet available.
// -----------------------------------------------------------------------
Float RFADiffBase::setDiff ( uInt ich,uInt ifr,Float d,Bool &flagged )
{
  Float thr = 0;
  d = abs(d);
  uInt it = diff.position();
  
  // write diff to lattice
  diff(ich,ifr) = d;
  
  flagged=false; 
  if( chunk.npass() && rowclipper.sigma0(ifr,it)>0 )
  {
    thr = rowclipper.sigma0(ifr,it);
    
    if( d > thr )
    {
      if( flag.setFlag(ich,ifr,*pflagiter) )
        rowclipper.markSigma(ifr);
      flagged=true;
    }
  }
  if( !flagged )
    diffrow(idiffrow++) = d;
  
  return thr;
}

// -----------------------------------------------------------------------
// class RFADiffMapBase 
//
//  
//
// -----------------------------------------------------------------------

// -----------------------------------------------------------------------
// RFADiffMapBase constructor
// Construct from Record of parameters
// -----------------------------------------------------------------------
RFADiffMapBase::RFADiffMapBase (  RFChunkStats &ch,const RecordInterface &parm ) 
  : RFADiffBase(ch,parm),
    RFDataMapper(parm.asArrayString(RF_EXPR),parm.asString(RF_COLUMN))
{
}

// -----------------------------------------------------------------------
// RFADiffMapBase destructor
// -----------------------------------------------------------------------
RFADiffMapBase::~RFADiffMapBase () 
{ 
}

// -----------------------------------------------------------------------
// RFADiffMapBase::getDefaults
// Returns record of default paramaters
// -----------------------------------------------------------------------
const RecordInterface & RFADiffMapBase::getDefaults ()
{
  static Record rec;
// create record description on first entry
  if( !rec.nfields() )
  {
    rec = RFADiffBase::getDefaults();
    rec.define(RF_COLUMN,"DATA");
    rec.define(RF_EXPR,"+ ABS XX YY");
    rec.setComment(RF_COLUMN,"Use column: [DATA|MODEL|CORRected]");
    rec.setComment(RF_EXPR,"Expression for deriving value (e.g. \"ABS XX\", \"+ ABS XX YY\")");
  }
  return rec;
}

// -----------------------------------------------------------------------
// RFADiffMapBase::iterTime
// Sets up mapper
// -----------------------------------------------------------------------
RFA::IterMode RFADiffMapBase::iterTime (uInt it)
{
  setupMapper();
  return RFADiffBase::iterTime(it);
}

// -----------------------------------------------------------------------
// RFADiffMapBase::getDesc
// Returns short description of parameters
// -----------------------------------------------------------------------
String RFADiffMapBase::getDesc ()
{
  return RFDataMapper::description()+"; "+RFADiffBase::getDesc();
}

} //# NAMESPACE CASA - END

