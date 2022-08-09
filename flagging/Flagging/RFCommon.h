//# RFCommon.h: this defines RFCommon
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
#ifndef FLAGGING_RFCOMMON_H
#define FLAGGING_RFCOMMON_H
    
#include <casacore/casa/Arrays/Vector.h>
#include <casacore/casa/Arrays/Matrix.h>
#include <casacore/casa/Containers/RecordInterface.h>
#include <casacore/casa/Containers/RecordInterface.h>
#include <casacore/casa/Logging/LogIO.h>
#include <algorithm>
#include <limits>

namespace casa { //# NAMESPACE CASA - BEGIN

// RFAs use bitwise flags
typedef casacore::uInt RFlagWord;
typedef casacore::Vector<RFlagWord> FlagVector;
typedef casacore::Matrix<RFlagWord> FlagMatrix;

// character constants for common agent parameters
const char 
    RF_GLISHINDEX[] = "_glishindex", // this is set automatically to indicate 1-based indexing
    
    RF_NAME[]    = "name",
    
    RF_DEBUG[]   = "debug",
    RF_PLOT[]    = "plot",
    RF_PLOTSCR[] = "plotscr",
    RF_GLOBAL[]  = "global",
    RF_TRIAL[]   = "trial",

    RF_RESET[]   = "reset",
    RF_FIGNORE[] = "fignore",
    RF_UNFLAG[]  = "unflag",
    RF_SHADOW[]  = "shadow",
    RF_ELEVATION[] = "elevation",
    
    RF_THR[]     = "thr",
    RF_ROW_THR[] = "rowthr",
    
    RF_HW[]      = "hw",
    RF_ROW_HW[]  = "rowhw",
    RF_ROW_DISABLE[] = "norow",

    RF_COLUMN[]  = "column",
    RF_EXPR[]    = "expr",
    
    RF_CLIP[]      = "clip",
    RF_FLAGRANGE[] = "flagrange",
    RF_MIN[]       = "min",
    RF_MAX[]       = "max",
    RF_CHANAVG[]   = "chanavg",
    
    RF_NDEG[]    = "ndeg",
    RF_SPWID[]   = "spwid", 
    RF_FREQS[]   = "fq",
    RF_CHANS[]   = "chan",
    RF_REGION[]  = "region",
    
    RF_NBINS[]   = "nbins",
    RF_MINPOP[]  = "minpop",
    
    RF_CORR[]    = "corr",
    RF_ANT[]     = "ant",
    RF_FIELD[]   = "field",
    RF_BASELINE[] = "baseline",
    RF_AUTOCORR[] = "autocorr",

    RF_INTENT[] = "intent",
    RF_SCAN[]    = "scan",
    RF_ARRAY[]   = "array",
    RF_OBSERVATION[] = "observation",
    RF_FEED[]    = "feed",
    RF_UVRANGE[] = "uvrange",
    RF_DIAMETER[] = "diameter",
    RF_LOWERLIMIT[]  = "lowerlimit",
    RF_UPPERLIMIT[]  = "upperlimit",
    
    RF_TIMERANGE[] = "timerng",
    RF_CENTERTIME[] = "timeslot",
    RF_TIMEDELTA[] = "dtime",
    
    RF_QUACK[]     = "quack",
    RF_QUACKMODE[] = "quackmode",
    RF_QUACKINC[]  = "quackinc",

    RF_NCHAN[] = "nchan",
    RF_START[] = "start",
    RF_STEP[] = "step",
    RF_MODE[] = "mode",
    RF_MSSELECT[] = "msselect";

// <summary>
// FlaggerEnums: collection of enums for various flagger classes
// </summary>

// <use visibility=local>

// <reviewed reviewer="" date="" tests="" demos="">
// </reviewed>
class FlaggerEnums 
{
public:
  typedef enum 
    { POLZN = 0,POL=POLZN,CORR=POLZN,
      CHAN  = 1,
      IFR   = 2,
      TIME  = 3,
      ROW   = 4,
      ANT   = 5,
      FEED  = 6,
      FEEDCORR  = 7,
      NONE  = -1,
      Num_StatEnums=8 
    } StatEnums;

  typedef enum 
  {
    FL_HONOR  = 0,
    FL_IGNORE = 1,
    FL_RESET  = 2
  } PreFlagPolicy;
};
   

// short inline function for checking the type of a record field
inline casacore::Bool fieldType ( const casacore::RecordInterface &parm,
                                  const casacore::String &id,casacore::DataType type,
                                  casacore::DataType type2 = casacore::TpNumberOfTypes )
{
  if( !parm.isDefined(id) || !parm.shape(id).product() )
    return false;
  casacore::DataType t = parm.dataType(id);
  return t==type || t==type2;
}
    
// short inline function for checking a field's data type against some function
inline casacore::Bool isField ( const casacore::RecordInterface &parm,const casacore::String &id,casacore::Bool (*func)(casacore::DataType) )
{
  if( !parm.isDefined(id) )
    return false;
  casacore::DataType type = parm.dataType(id);
  return (*func)(type);
}

// short inline function for checking that a field is a non-empty record
inline casacore::Bool isValidRecord ( const casacore::RecordInterface &parm,const casacore::String &id)
{
  if( !parm.isDefined(id) || parm.dataType(id) != casacore::TpRecord )
    return false;
  return parm.asRecord(id).nfields() > 0;
}
    
// casacore::Short inline function for checking if a record field is "set",
// i.e. exists, and is not an empty array or a boolean false.
inline casacore::Bool isFieldSet ( const casacore::RecordInterface &parm,const casacore::String &id )
{
  return parm.isDefined(id) && parm.shape(id).product() &&
          ( parm.dataType(id) != casacore::TpBool || parm.asBool(id) );
}
    
// casacore::Short inline function for returning the number of elements in a field
// (0 for no field, 1 for scalar, >1 for arrays)
inline casacore::uInt fieldSize ( const casacore::RecordInterface &parm,const casacore::String &id )
{
  return parm.isDefined(id) ? parm.shape(id).product() : 0;
}

// Basically just upcase(), but in a form suitable for casacore::Array::apply
inline casacore::String stringUpper ( const casacore::String &in )
{
  return upcase(in);
}

// a debug-printf function, for printf-ing debug messages
int dprintf( casacore::LogIO &os, const char *format, ... );

inline std::vector<bool> bitvec_from_ulong( unsigned long val, size_t len = std::numeric_limits<unsigned long>::digits ) {
    std::vector<bool> result(len,false);
    unsigned long mask = 1;
    for( size_t i=0; i < std::min((size_t)result.size( ),(size_t)std::numeric_limits<unsigned long>::digits); ++i ) {
        result[i] = mask & val ? true : false;
        mask = mask << 1;
    }
    return result;
}

inline unsigned long bitvec_to_ulong( const std::vector<bool> &val) {
    unsigned long result = 0;
    unsigned long mask = 1;
    for( size_t i=0; i < std::min((size_t)std::numeric_limits<unsigned long>::digits,(size_t)val.size()); ++i ) {
        result |= (val[i] ? mask : 0);
        mask = mask << 1;
    }
    return result;
}

} //# NAMESPACE CASA - END

#endif
