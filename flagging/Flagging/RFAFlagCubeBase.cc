//# RFAFlagCubeBase.cc: this defines RFAFlagCubeBase
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
#include <flagging/Flagging/RFAFlagCubeBase.h>
   
using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

uInt RFABase::indexing_base = 0;
 
// -----------------------------------------------------------------------
// RFABase constructor
// -----------------------------------------------------------------------
RFABase::RFABase ( RFChunkStats &ch,const RecordInterface &parm )
  : chunk(ch),params(parm),
    myname(parm.isDefined(RF_NAME)?parm.asString(RF_NAME):String("RFABase")),
    only_selector(false),
    os(LogOrigin("Flagger",myname))
{
}

void RFABase::init ()
{
    os << LogIO::DEBUGGING << myname << ": " << getDesc() << endl << LogIO::POST;
}
        
// -----------------------------------------------------------------------
// RFAFlagCubeBase constructor
// Construct from a Record of parameters
// -----------------------------------------------------------------------
RFAFlagCubeBase::RFAFlagCubeBase (  RFChunkStats &ch,const RecordInterface &parm ) :
  RFABase(ch,parm),
  flag(ch,isFieldSet(parm,RF_FIGNORE),isFieldSet(parm,RF_RESET),os)
{
}

// -----------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------
RFAFlagCubeBase::~RFAFlagCubeBase () 
{
}

// -----------------------------------------------------------------------
// RFAFlagCubeBase::getDefaults
// Returns record of default parameters
// -----------------------------------------------------------------------
const RecordInterface & RFAFlagCubeBase::getDefaults ()
{
  static Record rec;
// create record description on first entry
  if( !rec.nfields() )
  {
    rec.define(RF_FIGNORE,false);
    rec.setComment(RF_FIGNORE,"Use T to ignore existing flags");
  }
  return rec;
}

// -----------------------------------------------------------------------
// RFAFlagCubeBase::getDesc
// Returns description of parameters
// -----------------------------------------------------------------------
String RFAFlagCubeBase::getDesc ()
{
//   if( fieldType(params,RF_POLICY,TpString) )
//   {
//     char s[128];
//     sprintf(s,"%s=%s",RF_POLICY,params.asString(RF_POLICY).chars());
//     return s;
//   }
  if( isFieldSet(params,RF_FIGNORE) )
    return RF_FIGNORE;
  return "";
}

// -----------------------------------------------------------------------
// RFAFlagCubeBase::getStats
// Returns flagging stats
// -----------------------------------------------------------------------
String RFAFlagCubeBase::getStats ()
{  
  return flag.getSummary();
}

// -----------------------------------------------------------------------
// RFAFlagCubeBase::estimateMemoryUse
// -----------------------------------------------------------------------
uInt RFAFlagCubeBase::estimateMemoryUse ()
{
  return 0;
// NB: RedFlagger check the flag cube memoery usage manually!
//  return flag.estimateMemoryUse();
}

// -----------------------------------------------------------------------
// RFAFlagCubeBase::newChunk
// Sets up for new chunk of data
// -----------------------------------------------------------------------
Bool RFAFlagCubeBase::newChunk (Int &)
{
  flag.init(corrmask, nAgent, only_selector, name());
  return active=true;
}

// -----------------------------------------------------------------------
// RFAFlagCubeBase::endChunk
// Resets at end of chunk
// -----------------------------------------------------------------------
void RFAFlagCubeBase::endChunk ()
{
  flag.printStats();
  flag.cleanup();
}

// -----------------------------------------------------------------------
// RFAFlagCubeBase::startData
// Prepares for an data pass over a VisIter chunk
// -----------------------------------------------------------------------
void RFAFlagCubeBase::startData (bool)
{
  flag.reset();
}

// -----------------------------------------------------------------------
// RFAFlagCubeBase::startDry
// Prepares for an dry pass 
// -----------------------------------------------------------------------
    void RFAFlagCubeBase::startDry (bool)
{
  flag.reset();
}

// -----------------------------------------------------------------------
// RFAFlagCubeBase::startFlag
// Prepares for a flag-copy pass
// -----------------------------------------------------------------------
void RFAFlagCubeBase::startFlag (bool)
{
  flag.reset();
}

// -----------------------------------------------------------------------
// RFAFlagCubeBase::iterTime
// Default version of iter time just keeps the diff and flag lattices
// in sync with the time slot.
// -----------------------------------------------------------------------
RFA::IterMode RFAFlagCubeBase::iterTime (uInt it)
{
  flag.advance(it,true);
  return RFA::CONT;
}

// -----------------------------------------------------------------------
// RFAFlagCubeBase::iterDry
// Dry run iterator: recomputes the AAD and does flagging
// -----------------------------------------------------------------------
RFA::IterMode RFAFlagCubeBase::iterDry ( uInt it )
{
  flag.advance(it);
  return RFA::CONT;
}

// -----------------------------------------------------------------------
// RFAFlagCubeBase::iterFlag
// Advance flag lattice and copy out flags
// -----------------------------------------------------------------------
void RFAFlagCubeBase::iterFlag (uInt it)
{
  flag.advance(it);
  flag.setMSFlags(it);
}


RFA::IterMode RFAFlagCubeBase::endData ()
{
  flag.printStats();
  return RFA::STOP;
}

RFA::IterMode RFAFlagCubeBase::endDry ()
{
  flag.printStats();
  return RFA::STOP;
}

} //# NAMESPACE CASA - END

