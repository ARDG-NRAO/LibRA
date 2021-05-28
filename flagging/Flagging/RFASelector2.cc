
//# RFASelector.cc: this defines RFASelector
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
#include <flagging/Flagging/RFASelector.h>
#include <casa/stdio.h>

#include <iomanip>
#include <ostream>
#include <cassert>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

Bool dbg2 = false;
Bool verbose2 = false;
        
// -----------------------------------------------------------------------
// reformRange
// Reforms an array of 2N elements into a [2,N] matrix
// -----------------------------------------------------------------------

template<> Array<Int> fieldToArray<Int>( const RecordInterface &parm,const String &id )
{ return parm.toArrayInt(id); }
template<> Array<Double> fieldToArray<Double>( const RecordInterface &parm,const String &id )
{ return parm.toArrayDouble(id); }
template<> Array<String> fieldToArray<String>( const RecordInterface &parm,const String &id )
{ return parm.toArrayString(id); }


template Bool RFASelector::reformRange<Int>( Matrix<Int>&,const Array<Int>& );
template Bool RFASelector::reformRange<Double>( Matrix<Double>&,const Array<Double>& );
template Bool RFASelector::reformRange<String>( Matrix<String>&,const Array<String>& );
template Bool RFASelector::parseRange<Int>( Matrix<Int>&,const RecordInterface&,const String&);
template Bool RFASelector::parseRange<Double>( Matrix<Double>&,const RecordInterface&,const String&);
template Bool RFASelector::parseRange<String>( Matrix<String>&,const RecordInterface&,const String&);

// -----------------------------------------------------------------------
// RFA_selector::find
// Helper templated method to find an object in an array
// -----------------------------------------------------------------------

template Bool RFASelector::find<uInt>(uInt&,const uInt&,const Vector<uInt>&);
template Bool RFASelector::find<Int>(uInt&,const Int&,const Vector<Int>&);
template Bool RFASelector::find<String>(uInt&,const String&,const Vector<String>&);

// parseTimes
// Converts a field that is an array of Int/Double or Strings into 
// an array of Doubles. Numeric values are converted as is. Strings 
// are fed through MVTime::read to convert into MJDs (or MJSs, if 
// secs is true).
// -----------------------------------------------------------------------
Bool RFASelector::parseTimes ( Array<Double> &times,const RecordInterface &parm,const String &id,Bool secs )
{
  LogIO os(LogOrigin("RFASelector", "parseTimes()", WHERE));
  if( !isFieldSet(parm,id) )
    return false;
  if( fieldType(parm,id,TpString,TpArrayString) ) // String date/times
  {
    Array<String> tt( parm.asArrayString(id) );
    times.resize(tt.shape());
    Bool deltt,deltimes;
    const String *ptt = tt.getStorage(deltt);
    Double *ptimes = times.getStorage(deltimes);
    Int scale = secs ? 24*3600 : 1;
    for( uInt i=0; i<tt.nelements(); i++ )
    {
      Quantity q;
      if( !MVTime::read(q,ptt[i]) ) {
        os<<"bad "<<id<<" specified: "<<ptt[i]<<endl<<LogIO::EXCEPTION;
      }
      ptimes[i] = scale*(Double)MVTime(q);
    }
    tt.freeStorage(ptt,deltt);
    times.putStorage(ptimes,deltimes);
  }
    else if( isField(parm,id,isReal) ) // if not strings, try numeric MJDs
  {
    times = parm.toArrayDouble(id);
  }
  else                              // else can't parse
    return false;
  return true;
}


// -----------------------------------------------------------------------
// addString
// Helper method to build up description strings
// -----------------------------------------------------------------------
void RFASelector::addString( String &str,const String &s1,const char *sep )
{
  if( str.length() )
    str += sep;
  str += s1;
}

// -----------------------------------------------------------------------
// parseMinMax
// Helper function to parse a range specification
// -----------------------------------------------------------------------
Bool RFASelector::parseMinMax( Float &vmin,Float &vmax,const RecordInterface &spec,uInt f0 )
{
    
  vmin = -C::flt_max; vmax=C::flt_max;
// Option 1: fields named min/max exist... so use them
  Bool named = false;
  if( spec.isDefined(RF_MIN) )
    { vmin = spec.asFloat(RF_MIN); named = true; }
  if( spec.isDefined(RF_MAX) )
    { vmax = spec.asFloat(RF_MAX); named = true; }
  if( named )
    return true;
// Else look at first available field, if a 2-element array, assume
// [min,max] has been specified
  if (spec.nfields() < f0) {
      return false;
  }
  if( spec.shape(f0).nelements()==1 && spec.shape(f0)(0) == 2 )
  {
    Vector<Double> mm = spec.toArrayDouble(f0);
    vmin=mm(0); vmax=mm(1);
    return true;
  }
// Else assume next two record fields are {min,max}
  if( spec.nfields()-f0 > 2 )
  {
    vmin = spec.asFloat(f0);
    vmax = spec.asFloat(f0+1);
  }
  else
    vmax = spec.asFloat(f0);
  return true;
}

Bool RFASelector::fortestingonly_parseMinMax( Float &vmin,Float &vmax,const RecordInterface &spec,uInt f0 )
{
    return parseMinMax( vmin, vmax, spec, f0 );
}


// -----------------------------------------------------------------------
// normalize
// Helper function to shift a cyclic value (i.e. angle) into
// the interval [base,base+cycle) by adding/subtarcting an integer
// number of cycles
// -----------------------------------------------------------------------
static Double normalize( Double value,Double base,Double cycle )
{
  if( value < base )
    value += (floor((base-value)/cycle)+1)*cycle;
  else if( value >= base+cycle )
    value -= floor((value-base)/cycle)*cycle;
  return value;  
}
    

// -----------------------------------------------------------------------
// addClipInfo
// -----------------------------------------------------------------------
void RFASelector::addClipInfo( const Vector<String> &expr,
                               Float vmin,Float vmax,
                               Bool clip,
                               Bool channel_average)
{
    // create mapper and clipinfo block
    RFDataMapper *mapper = new RFDataMapper(expr,sel_column);

    ClipInfo clipinfo = { mapper,
                          vmin,vmax, 
                          channel_average,
                          clip,0.0 };

    // if dealing with cyclic values, normalize min/max accordingly
    Double cycle = mapper->getValueCycle();  // e.g. 360 for angles

    if (cycle > 0)  {
        Double base = mapper->getValueBase();  // e.g. -180 for angles

        // normalize min/max angle into [base,base+cycle)
        clipinfo.vmin = normalize(clipinfo.vmin, base, cycle);
        clipinfo.vmax = normalize(clipinfo.vmax, base, cycle);

        // if order is reversed, then we're spanning a cycle boundary (i.e. 355->5)
        if( clipinfo.vmin > clipinfo.vmax ) {
          clipinfo.vmax += cycle;   // ...so add a cycle
        }

        // use vmin as offset
        clipinfo.offset = clipinfo.vmin;
        clipinfo.vmin = 0;
        clipinfo.vmax -= clipinfo.offset;
    }
    // add block to appropriate list  
    Block<ClipInfo> & block( mapper->type()==RFDataMapper::MAPROW ? sel_clip_row : sel_clip );
    uInt ncl = block.nelements();
    block.resize(ncl+1,false,true);
    block[ncl] = clipinfo;
}

// -----------------------------------------------------------------------
// parseClipField
// Helper function to parse a clip specification
// -----------------------------------------------------------------------
void RFASelector::parseClipField( const RecordInterface &spec,Bool clip )
{
    //cerr << "spec: " << spec << endl;
    //cerr << "clip: " << clip << endl;
    //cerr << "spec.name(0): " << spec.name(0)<< endl;
    //cerr << "RF_EXPR: " << RF_EXPR << endl;

  try {

    bool ca = spec.asBool(RF_CHANAVG);
    
// Syntax one - we have a record of {expr,min,max} or {expr,[min,max]}
// or {expr,max}
    if( spec.name(0)== RF_EXPR )
    {
      Vector<String> expr = spec.asArrayString(0);

      Float vmin,vmax;
      if( !parseMinMax(vmin,vmax,spec,1))
        throw(AipsError(""));
      addClipInfo(expr, vmin, vmax, clip, ca);
    }
// Syntax two: we have a record of { expr1=[min,max],expr2=[min,max],.. }
    else
    {
      for( uInt i=0; i<spec.nfields(); i++ )
      {
        Vector<String> expr(1,spec.name(i));
        Float vmin=-C::flt_max,vmax=C::flt_max;
        if( spec.dataType(i) == TpRecord )
        {
          uInt f0=0;
          if( spec.asRecord(i).name(0) == RF_EXPR )
          {
            expr = spec.asRecord(i).asArrayString(0);
            f0++;
          }
          if( !parseMinMax(vmin,vmax,spec.asRecord(i),f0))
            throw(AipsError(""));
        } 
        else 
        {
          if( isArray( spec.type(i)))
          {
            Vector<Double> vec = spec.toArrayDouble(i);
            if( vec.nelements() == 1 )
              vmax=vec(0);
            else if( vec.nelements() == 2 )
              { vmin=vec(0); vmax=vec(1); }
            else
              throw(AipsError(""));
          }
          else
          {
            vmax = spec.asFloat(i);
          }
        }
        addClipInfo(expr,vmin,vmax,clip, ca);
      }
    }
  }
  catch( AipsError x ) {
      os<<"Illegal \""<<(clip?RF_CLIP:RF_FLAGRANGE) <<
	  "\" record: " << x.what() << "\n" <<
	  LogIO::EXCEPTION;
  }
}

void RFASelector::fortestingonly_parseClipField( const RecordInterface &spec,Bool clip )
{
    parseClipField(spec, clip);
}

void RFASelector::addClipInfoDesc ( const Block<ClipInfo> &clip)
{
  for( uInt i=0; i<clip.nelements(); i++ )
  {
    String ss;
    char s1[32]="",s2[32]="";

    if (clip[i].channel_average) {
      ss = "average=1 ";
    }
    else {
      ss = "average=0 ";
    }

    if( clip[i].vmin != -C::flt_max )
      sprintf(s1,"%g",clip[i].vmin + clip[i].offset);

    if( clip[i].vmax != C::flt_max )
      sprintf(s2,"%g",clip[i].vmax + clip[i].offset);

    if( clip[i].clip )
    {
      ss += clip[i].mapper->description();
      if( s1[0] )
      { 
        ss += String("<") +s1;
        if( s2[0] ) ss += ",";
      }
      if( s2[0] )
        ss += String(">")+s2;
    }
    else
    {
      if( s1[0] )
        ss += String(s1)+"<=";
      ss += clip[i].mapper->description();
      if( s2[0] )
        ss += String("<=")+s2;
    }

    addString(desc_str,ss);
  }
}

// -----------------------------------------------------------------------
// RFASelector constructor
// -----------------------------------------------------------------------
RFASelector::RFASelector ( RFChunkStats &ch,const RecordInterface &parm) : 
  RFAFlagCubeBase(ch,parm)
{
  if ( chunk.measSet().isNull() ) {
    throw AipsError("Received chunk referring to NULL MS!");
  }

  char s[256];

  if( fieldType(parm,RF_FREQS,TpArrayString)) // frequency range[s], as measures
  {
    Matrix<String> sfq;
    parseRange(sfq,parm,RF_FREQS);
    sel_freq.resize( sfq.shape()) ;
    // parse array of String frequency quanta
    for( uInt i=0; i<sfq.nrow(); i++)  
      for( uInt j=0; i<sfq.ncolumn(); j++)  
      {
        Float q; char unit[32];
        if( sscanf(sfq(i,j).chars(),"%f%s",&q,unit)<2) 
          os<<"Illegal \""<<RF_FREQS<<"\" array\n"<<LogIO::EXCEPTION;
        Quantity qq(q,unit);
        sel_freq(i,j) = qq.getValue("Hz");
      }
  } 
  else // freq. specified as MHz
  {
    parseRange(sel_freq,parm,RF_FREQS);
    sel_freq *= 1e+6;
  }
  if( sel_freq.nelements()) 
  {
    String fq;
    for( uInt i=0; i<sel_freq.ncolumn(); i++) 
    {
      sprintf(s,"%.2f-%.2f",sel_freq(0,i)*1e-6,sel_freq(1,i)*1e-6);
      addString(fq,s,",");
    }
    addString(desc_str,String(RF_FREQS)+"="+fq+"MHz");
  }

  // parse input arguments: channels
  if( parseRange(sel_chan,parm,RF_CHANS)) 
  {
    String sch;
    for( uInt i=0; i<sel_chan.ncolumn(); i++) 
    {
      sprintf(s,"%d:%d",sel_chan(0,i),sel_chan(1,i));
      addString(sch,s,",");
    }
    addString(desc_str,String(RF_CHANS)+"="+sch);
    sel_chan(sel_chan>=0) += -(Int)indexingBase();
  }
  
  // parse input arguments: correlations
  if( fieldType(parm,RF_CORR,TpString,TpArrayString))
  {
    String ss;
    Vector<String> scorr( parm.asArrayString(RF_CORR)) ;
    sel_corr.resize( scorr.nelements()) ;
    for( uInt i=0; i<scorr.nelements(); i++) 
    {
      sel_corr(i) = Stokes::type( scorr(i)) ;
      if( sel_corr(i) == Stokes::Undefined) 
        os<<"Illegal correlation "<<scorr(i)<<endl<<LogIO::EXCEPTION;
      addString(ss,scorr(i),",");
    }
    addString(desc_str,String(RF_CORR)+"="+ss);
  }
  // read clip column
  if( fieldType(parm,RF_COLUMN,TpString)) 
  {
    parm.get(RF_COLUMN,sel_column);
    addString(desc_str, String(RF_COLUMN)+"="+sel_column);
  }
  
  // parse input arguments: Spw ID(s)
  if( fieldType(parm,RF_SPWID,TpInt,TpArrayInt)) 
  {
    parm.get(RF_SPWID,sel_spwid);
    String ss;
    for( uInt i=0; i<sel_spwid.nelements(); i++) 
      addString(ss,String::toString(sel_spwid(i)),",");
    addString(desc_str,String(RF_SPWID)+"="+ss);
    sel_spwid -= (Int)indexingBase();
  }

  // parse input arguments: Field names or ID(s)
  if( fieldType(parm,RF_FIELD,TpString,TpArrayString)) 
  {
    parm.get(RF_FIELD,sel_fieldnames);
    sel_fieldnames.apply(stringUpper);
    String ss;
    for( uInt i=0; i<sel_fieldnames.nelements(); i++) 
      addString(ss,sel_fieldnames(i),",");
    addString(desc_str,String(RF_FIELD)+"="+ss);
  }
  else if( fieldType(parm,RF_FIELD,TpInt,TpArrayInt)) 
  {
    parm.get(RF_FIELD,sel_fieldid);
    String ss;
    for( uInt i=0; i<sel_fieldid.nelements(); i++) 
      addString(ss,String::toString(sel_fieldid(i)),",");
    addString(desc_str,String(RF_FIELD)+"="+ss);
    sel_fieldid -= (Int)indexingBase();
  }
// parse input arguments: Scan Number(s)
  if( fieldType(parm,RF_SCAN,TpInt,TpArrayInt)) 
  {
    parm.get(RF_SCAN,sel_scannumber);
    String ss;
    for( uInt i=0; i<sel_scannumber.nelements(); i++) 
      addString(ss,String::toString(sel_scannumber(i)),",");
    addString(desc_str,String(RF_SCAN)+"="+ss);
    sel_scannumber -= (Int)indexingBase();
  }
// parse input arguments: Scan intent
  if ( fieldType(parm,RF_INTENT,TpInt,TpArrayInt))
  {  
    parm.get(RF_INTENT,sel_stateid);
    String ss;
    for( uInt i=0; i<sel_stateid.nelements(); i++) 
      addString(ss,String::toString(sel_stateid(i)),",");
    addString(desc_str,String(RF_INTENT)+"="+ss);
    sel_stateid -= (Int)indexingBase();
  }
// parse input arguments: Array ID(s)
  if( fieldType(parm,RF_ARRAY,TpInt,TpArrayInt)) 
  {
    parm.get(RF_ARRAY,sel_arrayid);
    String ss;
    for( uInt i=0; i<sel_arrayid.nelements(); i++) 
      addString(ss,String::toString(sel_arrayid(i)),",");
    addString(desc_str,String(RF_ARRAY)+"="+ss);
    sel_arrayid -= (Int)indexingBase();
  }
  // parse input arguments: Observation ID(s)
    if( fieldType(parm,RF_OBSERVATION,TpInt,TpArrayInt))
    {
      parm.get(RF_OBSERVATION,sel_observation);
      String ss;
      for( uInt i=0; i<sel_observation.nelements(); i++)
        addString(ss,String::toString(sel_observation(i)),",");
      addString(desc_str,String(RF_OBSERVATION)+"="+ss);
      sel_observation -= (Int)indexingBase();
    }
// parse input: specific time ranges 
  Array<Double> rng;
  Matrix<Double> timerng;
  if( parseTimes(rng,parm,RF_TIMERANGE)) 
  {
      if( !reformRange(timerng,rng)) 
	  os << "Illegal \"" << RF_TIMERANGE << "\" array\n" << LogIO::EXCEPTION;
      sel_timerng = timerng*(Double)(24*3600);

      String s(String(RF_TIMERANGE) + "("); // + String::toString(timerng.ncolumn()));

      Bool del;
      Double *ptimes = rng.getStorage(del);
      for (unsigned i = 0; i < rng.nelements(); i++) {
	  s += String::toString(ptimes[i]);
	  if (i < rng.nelements()-1) {
	      s += ' ';
	  }
      }
      rng.putStorage(ptimes, del);
      s += ")";

      addString(desc_str, s);
  }

  // parse input: specific UV ranges 
  Array<Double> uvrng;
  Matrix<Double> uvrange;
  if( parseTimes(uvrng,parm,RF_UVRANGE)) 
  {
    if( !reformRange(uvrange,uvrng)) 
      os<<"Illegal \""<<RF_UVRANGE<<"\" array\n"<<LogIO::EXCEPTION;
    sel_uvrange = uvrange;
    addString(desc_str,String(RF_UVRANGE)+"("+String::toString(uvrange.ncolumn())+")");
  }

// parse input arguments: ANT specified by string ID
  LogicalVector sel_ant(num(ANT),false); 
  if( fieldType(parm,RF_ANT,TpString,TpArrayString)) 
  {
    Vector<String> sant( parm.asArrayString(RF_ANT)) ;
    sant.apply(stringUpper);
    const Vector<String> &names( chunk.antNames()) ;
    for( uInt i=0; i<sant.nelements(); i++) 
    {
      uInt iant;
      if( !find( iant,sant(i),names)) 
        os<<"Illegal antenna ID "<<sant(i)<<endl<<LogIO::EXCEPTION;
      sel_ant(iant)=true;
    }
  }
// else ANT specified directly by indexes
  else if( fieldType(parm,RF_ANT,TpInt,TpArrayInt)) 
  {
    Vector<Int> sant = parm.asArrayInt(RF_ANT);
    for( uInt i=0; i<sant.nelements(); i++) 
      sel_ant( sant(i) - (Int)indexingBase())  = true;
  }
  if( sum(sel_ant)) 
  {
    String sant;
    for( uInt i=0; i<num(ANT); i++) 
      if( sel_ant(i)) 
        addString(sant, chunk.antNames()(i),",");
    addString(desc_str, String(RF_ANT)+"="+sant);
  }

// parse input: baselines as "X-Y"
  sel_ifr = LogicalVector(num(IFR),false);
  String ifrdesc;
  const Vector<String> &names( chunk.antNames()) ;
  if( fieldType(parm, RF_BASELINE, TpString, TpArrayString)) 
  {
    Vector<String> ss(parm.asArrayString(RF_BASELINE));
    ss.apply(stringUpper);
    for( uInt i=0; i<ss.nelements(); i++) 
    {
      uInt ant1,ant2;
      String ants[2];
      Int res = split(ss(i),ants,2,"-");
      Bool wild1 = (ants[0]=="*" || ants[0]=="") ;  // is it a wildcard instead of ID?
      Bool wild2 = (ants[1]=="*" || ants[1]=="") ;
      if( res<2 || ( wild1 && wild2)) 
        os<<"Illegal baseline specification "<<ss(i)<<endl<<LogIO::EXCEPTION;
      Bool val1 = wild1 || find(ant1,ants[0],names);
      Bool val2 = wild2 || find(ant2,ants[1],names);
      // if both antenna IDs are valid, use them
      if( val1 && val2) 
      {
        if( wild1) 
        {
          addString(ifrdesc,ants[1]+"-*",",");
          for( uInt a=0; a<num(ANT); a++)  
            sel_ifr( chunk.antToIfr(a,ant2))  = true;
        }
        else if( wild2) 
        {
          addString(ifrdesc,ants[0]+"-*",",");
          for( uInt a=0; a<num(ANT); a++)  
            sel_ifr( chunk.antToIfr(ant1,a))  = true;
        }
        else
        {
          addString(ifrdesc,ants[0]+"-"+ants[1],",");
          sel_ifr( chunk.antToIfr(ant1,ant2))  = true;
        }
      }
      else // try to interpret them as numbers instead
      {
        if( sscanf(ss(i).chars(),"%d-%d",&ant1,&ant2)<2 ||
            ant1>=num(ANT) || ant2>=num(ANT)) 
          os<<"Illegal baseline specification "<<ss(i)<<endl<<LogIO::EXCEPTION;
        sel_ifr( chunk.antToIfr(ant1-(Int)indexingBase(),ant2-(Int)indexingBase()))  = true;
        addString(ifrdesc,ss(i),",");
      }
    }
  }
// parse input: baselines as [[x1,y1],[x2,y2],... etc.
  else if( fieldType(parm,RF_BASELINE,TpInt,TpArrayInt)) 
  {
    Matrix<Int> ant;
    if( parseRange(ant,parm,RF_BASELINE))
    {
      ant -= (Int)indexingBase();
      for( uInt i=0; i<ant.ncolumn(); i++)
      {
        if( ant(0,i)==-1)
        {
          if( ant(1,i)==-1)
            os<<"Illegal baseline specification [-1,-1]"<<LogIO::EXCEPTION<<endl;
          for( uInt a=0; a<num(ANT); a++) 
            sel_ifr( chunk.antToIfr(a,ant(1,i))) = true;
          addString(ifrdesc,names(ant(1,i))+"-*",",");
        }
        else if( ant(1,i)==-1)
        {
          for( uInt a=0; a<num(ANT); a++) 
            sel_ifr( chunk.antToIfr(ant(0,i),a)) = true;
          addString(ifrdesc,names(ant(0,i))+"-*",",");
        }
        else
        {
          unsigned indx = chunk.antToIfr(ant(0,i),ant(1,i));

          assert( indx < sel_ifr.nelements() );

          sel_ifr(indx) = true;
          addString(ifrdesc,names(ant(0,i))+"-"+names(ant(1,i)),",");
        }
      }
    }
  }

  if( sum(sel_ifr))
  {
    String ss;
    addString(desc_str,String(RF_BASELINE)+"="+ifrdesc);
  }
  else // no IFRs were specified
  {
    if( sum(sel_ant)) // antennas specified? flag only their baselines
    {
      for( uInt a1=0; a1<num(ANT); a1++)
        if( sel_ant(a1))
          for( uInt a2=0; a2<num(ANT); a2++)
            sel_ifr(chunk.antToIfr(a1,a2)) = true;
    }
    else // no antennas either? flag everything
      sel_ifr.resize();
  }

  // parse input: feeds as [[x1,y1],[x2,y2],... etc.
  if( fieldType(parm,RF_FEED,TpInt,TpArrayInt)) 
  {
    sel_feed = LogicalVector(num(FEEDCORR),false);
    String feeddesc;
    Matrix<Int> feed;
    if( parseRange(feed,parm,RF_FEED))
    {
      feed -= (Int)indexingBase();
      for( uInt i=0; i<feed.ncolumn(); i++)
      {
        if( feed(0,i)==-1)
        {
          if( feed(1,i)==-1)
            os<<"Illegal feed specification [-1,-1]"<<LogIO::EXCEPTION<<endl;
          for( uInt a=0; a<num(FEED); a++) 
            sel_feed( chunk.antToIfr(a,feed(1,i))) = true;
          addString(feeddesc,names(feed(1,i))+"-*",",");
        }
        else if( feed(1,i)==-1)
        {
          for( uInt a=0; a<num(FEED); a++) 
            sel_feed( chunk.antToIfr(feed(0,i),a)) = true;
          addString(feeddesc,names(feed(0,i))+"-*",",");
        }
        else
        {
          sel_feed(chunk.antToIfr(feed(0,i),feed(1,i))) = true;
          addString(feeddesc,names(feed(0,i))+"-"+names(feed(1,i)),",");
        }
      }
    }
  }

// now, all selection-related arguments are accounted for.
// set flag if some subset has been selected
  Bool have_subset = ( desc_str.length());

  if (have_subset)
    desc_str+=";";
// unflag specified?
  unflag =  (fieldType(parm, RF_UNFLAG,TpBool) && parm.asBool(RF_UNFLAG));
  //addString(desc_str,unflag?RF_UNFLAG:"flag");

  ac = new MSAntennaColumns(chunk.measSet().antenna());
  diameters = ac->dishDiameter().getColumn();
  
  shadow = fieldType(parm, RF_SHADOW, TpBool) && parm.asBool(RF_SHADOW);
  if (shadow) {
    diameter = parm.asDouble(RF_DIAMETER);
  }

  elevation = fieldType(parm, RF_ELEVATION, TpBool) && parm.asBool(RF_ELEVATION);
  if (elevation) {
    lowerlimit = parm.asDouble(RF_LOWERLIMIT);
    upperlimit = parm.asDouble(RF_UPPERLIMIT);
  }
 
// now, scan arguments for what to flag within the selection
// specific times (specified by center times)
  Vector<Double> ctimes;
  Double timedelta = 10;
  if (parseTimes(ctimes,parm,RF_CENTERTIME))
  {
    ctimes *= (Double)(24*3600);
    String ss (String(RF_CENTERTIME)+"("+String::toString(ctimes.nelements())+")");
    Vector<Double> dt;
    if (parseTimes(dt,parm,RF_TIMEDELTA,true))
    {
      timedelta = dt(0);
      sprintf(s,",dtime=%.1fs",timedelta);
      ss += s;
    }
    addString(desc_str,ss);
    uInt n = ctimes.nelements();
    sel_time.resize(2,n);
    sel_time.row(0) = ctimes - timedelta;
    sel_time.row(1) = ctimes + timedelta;
  }

// flag autocorrelations too?
  sel_autocorr =  (fieldType(parm,RF_AUTOCORR,TpBool) && parm.asBool(RF_AUTOCORR));
  if (sel_autocorr)
    addString(desc_str,RF_AUTOCORR);

  // parse input: quack mode (for VLA)
  if (isFieldSet(parm, RF_QUACK)) {
    
      //quack_si = 30.0; // scan interval
      quack_dt = 10.0; // dt to flag at start of scan
 
      // are specific values given? 
      Vector<Double> qt;
      if (parseTimes(qt, parm, RF_QUACK, true)) {

          if (qt.nelements() > 3) {
              os << RF_QUACK << " must be specified as T, <scaninterval> or [scaninterval,dt]"
                 << endl << LogIO::EXCEPTION;
          }
          
          quack_si = qt(0);
          if (qt.nelements() > 2) {
              quack_dt = qt(1);
          }

          assert(parm.isDefined(RF_QUACKMODE));
          assert(parm.isDefined(RF_QUACKINC));

          quack_mode = parm.asString(RF_QUACKMODE);
          quack_increment = parm.asBool(RF_QUACKINC);
          
      }
      sprintf(s, "%s=%ds", // %s=%s; %s=%s", 
              RF_QUACK, (Int)quack_si);
              //RF_QUACKMODE, quack_mode,
              //RF_QUACKINC, quack_increment ? "true" : "false");
      addString(desc_str, s);
      //    quack_si /= (24*3600);
      //    quack_dt /= (24*3600);
  }
  else {
    quack_si = 0;
  }

  // flag a specific range or clip outside of range?

  if (isValidRecord(parm,RF_CLIP)) {
    parseClipField(parm.asRecord(RF_CLIP),true);
  }

  if (isValidRecord(parm,RF_FLAGRANGE))
    parseClipField(parm.asRecord(RF_FLAGRANGE),false);

  // add to description strings, if something was parsed
  if (sel_clip.nelements())
  {
    addClipInfoDesc(sel_clip);
    sel_clip_active.resize(sel_clip.nelements());
  }
  if (sel_clip_row.nelements())
    addClipInfoDesc(sel_clip_row);

// if nothing has been specified to flag, flag everything within selection
  flag_everything = 
    (quack_si == 0 && 
     !sel_time.nelements() && 
     !sel_autocorr && 
     !sel_clip.nelements() && 
     !sel_clip_row.nelements() && 
     !shadow && 
     !elevation &&
     !sel_stateid.nelements());
     //!elevation);
  /*
  if (flag_everything)
  {
    if (!have_subset && !unflag)
      os<<"FLAG ALL requested, but no MS subset specified.\n"
          "Refusing to flag the whole measurement set!\n"<<LogIO::EXCEPTION;
//    addString(desc_str,"all");
  }
  */
//  cout<<"Selector: "<<desc_str<<endl;

}


RFASelector::~RFASelector ()
{
  delete ac;

  for( uInt i=0; i<sel_clip.nelements(); i++ )
    if( sel_clip[i].mapper )
      delete sel_clip[i].mapper;
}

void RFASelector::startData(bool verbose)
{
    RFAFlagCubeBase::startData(verbose);
  
    String flagstring = unflag?String("unflag"):String("flag");

    os << LogIO::DEBUGGING << "Data flagged/unflagged : " << desc_str << " " << flagstring;
    if (flag_everything) os << " all" ;
    os << LogIO::POST;
    
    Bool have_subset = ( desc_str.length() );
    
    if( flag_everything && !shadow)
        {
          /* jmlarsen: This does not seem useful nor necessary */
          
            if (false) if( !have_subset && !unflag)
                os<<"FLAG ALL requested, but no MS subset specified.\n"
                    "Refusing to flag the whole measurement set!\n"<<LogIO::EXCEPTION;
        }
    
    return;
}

// -----------------------------------------------------------------------
// newChunk
// At each new chunk, figure out what goes where 
// -----------------------------------------------------------------------
Bool RFASelector::newChunk (Int &maxmem)
{
// check correlations and figure out the active correlations mask
  Vector<Int> corrtype;
  chunk.visIter().corrType(corrtype);
  corrmask = 0;
  if( sel_corr.nelements() )
  {
    corrmask = chunk.getCorrMask(sel_corr);
    if( !corrmask )
    {
      if(verbose2) os<<"No matching correlations in this chunk\n"<<LogIO::POST;
      return active=false;
    }
  }
  else // no correlations specified so flag everything
    corrmask = chunk.fullCorrMask();
  
// check field IDs and spectral window IDs
  uInt dum;
  if( sel_spwid.nelements() && !find(dum,chunk.visBuf().spectralWindow(),sel_spwid) )
  {
    if(verbose2) os<<"Spectral window does not match in this chunk\n"<<LogIO::POST;
    return active=false;
  }
  if( sel_fieldid.nelements() && !find(dum,chunk.visIter().fieldId(),sel_fieldid) )
  {
    if(verbose2) os<<"Field ID does not match in this chunk\n"<<LogIO::POST;
    return active=false;
  }
  if( sel_fieldnames.nelements() && !find(dum,chunk.visIter().fieldName(),sel_fieldnames) )
  {
    if(verbose2) os<<"Field name does not match in this chunk\n"<<LogIO::POST;
    return active=false;
  }
  if( sel_arrayid.nelements() && !find(dum,chunk.visIter().arrayId(),sel_arrayid) )
  {
    if(verbose2) os<<"Array ID does not match in this chunk\n"<<LogIO::POST;
    return active=false;
  }
  if( sel_observation.nelements() && !find(dum,chunk.visBuf().observationId()[0],sel_observation) )
  {
    if(verbose2) os<<"Observation ID does not match in this chunk\n"<<LogIO::POST;
    return active=false;
  }
  //Vector<Int> tempstateid(0);
  //tempstateid = chunk.visIter().stateId(tempstateid);
  //if( tempstateid.nelements() && sel_stateid.nelements() && !find(dum,tempstateid[0],sel_stateid) )
  //{
  //  if(verbose2) os<<"State ID does not match in this chunk\n"<<LogIO::POST;
  //  return active=false;
  //}  
  //
  /*
  Vector<Int> temp(0);
  temp = chunk.visIter().scan(temp);
  if( temp.nelements() &&  sel_scannumber.nelements() && !find(dum,temp[0],sel_scannumber) )
  {
    os<<"Scan Number does not match the FIRST scan number in this chunk\n"<<LogIO::POST;
    return active=false;
  }
  */
  
  // Get the times at the beginning and end of this scan.
  
  const Vector<Int> &scans(chunk.visBuf().scan());

  Int s0 = scans(0);

  if (!allEQ(scans, s0) && quack_si > 0) {
      os << "RFASelector: VisBuffer has given us different scans (in this chunk)." 
         << LogIO::EXCEPTION;
  }
  

  //cout << "scan range is = " <<
  //MVTime( scan_start/C::day).string( MVTime::DMY,7) << " - " << 
  //MVTime( scan_end  /C::day).string( MVTime::DMY,7) << endl;


// figure out active channels (i.e. within specified segments)
  flagchan.resize();
  if( sel_freq.ncolumn() || sel_chan.ncolumn() )
  {
    flagchan = LogicalVector(num(CHAN),false);
    const Vector<Double> & fq( chunk.frequency() );
    for( uInt i=0; i<sel_freq.ncolumn(); i++ )
      flagchan = flagchan || ( fq >= sel_freq(0,i) && fq <= sel_freq(1,i) );
    Vector<Int> ch( num(CHAN) );
    indgen(ch);
    Matrix<Int> schan = sel_chan;
    schan( sel_chan<0 ) += (Int)num(CHAN);
    for( uInt i=0; i<sel_chan.ncolumn(); i++ )
    {
      flagchan = flagchan || ( ch >= schan(0,i) && ch <= schan(1,i) );
    }
    if( !sum(flagchan) )
    {
     if(verbose2)  os<<"No matching frequencies/channels in this chunk\n"<<LogIO::POST;
      return active=false;
    }
    if( allEQ(flagchan,true) )
      flagchan.resize(); // null array = all true
  }
// init all clipping mappers, and check their correlation masks
  if( sel_clip.nelements() )
  {
    // see which mappers are active for this chunk, and accumulate their
    // masks in clip_corrmask
    RFlagWord clip_corrmask=0;
    for( uInt i=0; i<sel_clip.nelements(); i++ ) 
    {
      RFlagWord mask = sel_clip[i].mapper->corrMask(chunk.visIter());
      sel_clip_active(i) = (mask!=0);
      clip_corrmask |= mask;
    }
    sum_sel_clip_active = sum(sel_clip_active);
    // If no explicit correlations were selected by the user,  
    // then use clip_corrmask as the overall mask
    if( !sel_corr.nelements() )
    {
      corrmask = clip_corrmask;
      if( !corrmask )
      {
        if (verbose2) {
          os << "No matching correlations in this chunk\n" << LogIO::POST;
        }
        return active=false;
      }
    }
  }
// reserve a minimum of memory for ourselves
  maxmem -= 1;
// init flagging cube and off we go...
  RFAFlagCubeBase::newChunk(maxmem);
// see if full row is being flagged, i.e. no subset of channels was selected,
// and no explicit correlations (or all correlations).
  select_fullrow = (!flagchan.nelements() && 
    (!sel_corr.nelements() || corrmask==chunk.fullCorrMask()) );
  return active=true;
}

// -----------------------------------------------------------------------
// processRow
// Raises/clears flags for a single row, depending on current selection
// -----------------------------------------------------------------------
void RFASelector::processRow(uInt ifr,uInt it)
{
   if(dbg2)   cout << ifr << ",";
   // does the selection include whole rows?
   if (select_fullrow) {
       
       unflag ? flag.clearRowFlag(ifr,it) : flag.setRowFlag(ifr,it);
       // apply this to the entire row...
       for( uInt ich=0; ich<num(CHAN); ich++ )
	   unflag ? flag.clearFlag(ich,ifr) : flag.setFlag(ich,ifr);
       
   }
   else {
       // else apply data flags to selection
       for( uInt ich=0; ich<num(CHAN); ich++ )
	   if( !flagchan.nelements() || flagchan(ich) )
	       unflag ? flag.clearFlag(ich,ifr) : flag.setFlag(ich,ifr);
   }
}

// -----------------------------------------------------------------------
// Processes 1 time slot in the MS
// There is one MS row per baseline
//  
// -----------------------------------------------------------------------
RFA::IterMode RFASelector::iterTime (uInt it)
{
  RFAFlagCubeBase::iterTime(it);

  // setup each correlation clip mapper
  for (uInt i=0; i<sel_clip.nelements(); i++) 
      if (sel_clip_active(i))
          sel_clip[i].mapper->setVisBuffer(chunk.visBuf());
  
  // extract time
  const Vector<Double> &times( chunk.visBuf().time() );
  const Vector<Double> &dtimes( chunk.visBuf().timeInterval() );
  Double t0 = times(0);
  Double dt0 = dtimes(0);
  if( !allEQ(times,t0) )
    os << "RFASelector: VisBuffer has given us different times." << LogIO::EXCEPTION;
  
  // extract scan number
  const Vector<Int> &scans( chunk.visBuf().scan() );

  Int s0 = scans(0);

  if (!allEQ(scans,s0) && quack_si > 0) {
      os << "RFASelector: crash&burn, VisBuffer's given us different scans."
         << LogIO::EXCEPTION;
  }

  // is current scan number within selected list of scan numbers?
  bool scanselect = true;
  if (sel_scannumber.nelements()) {
      bool sel = false;
      for (uInt i = 0;
           i < sel_scannumber.nelements();
           i++) {
          if( sel_scannumber(i) == s0 ) {
              sel = true;
          }
      }
    
      if( ! sel ) scanselect = false;
      //if( ! sel )  return RFA::CONT;
  }

  // flag if within specific timeslots
  bool timeselect = true;
  if (sel_timerng.ncolumn()) {
      timeselect = false;
      if( anyEQ(sel_timerng.row(0) <= t0 && sel_timerng.row(1) >= t0,
                true) ) {
          timeselect = true;
      }
  }
  
  if ( ! (timeselect && scanselect) ) {       
      return RFA::CONT;
  }
  
  const Vector<Int> &ifrs( chunk.ifrNums() );
  const Vector<Int> &feeds( chunk.feedNums() );
  const Vector<casacore::RigidVector<casacore::Double, 3> >&uvw( chunk.visBuf().uvw() );
  // Vector<Vector<Double> > &uvw=NULL;//( chunk.visIter.uvw(uvw) );
  //chunk.visIter().uvw(uvw);
  Double uvdist=0.0;

  if( ifrs.nelements() != feeds.nelements() || 
      ifrs.nelements() != uvw.nelements() ) 
          cout << "RFASelector::iterTime ->  nelements mismatch " << endl;

// do we flag the whole selection?  
  bool flagall = flag_everything;

  if (!flagall) {

      if (elevation) {
          const Vector<MDirection> &azimuth_elevation = chunk.visIter().azel(t0);

	  for (uInt i = 0; i < ifrs.nelements(); i++) {
	      
	      unsigned a1, a2;
	      chunk.ifrToAnt(a1, a2, chunk.ifrNum(i));

	      Bool inrange = false;
	      uvdist = sqrt( uvw(i)(0)*uvw(i)(0) + uvw(i)(1)*uvw(i)(1) );
	      for (uInt j = 0; j < sel_uvrange.ncolumn(); j++)
		  if (uvdist >= sel_uvrange(0, j) && uvdist <= sel_uvrange(1, j))
		      inrange |= true;
	      
	      if( (!sel_ifr.nelements() || sel_ifr(ifrs(i))) && 
		  (!sel_feed.nelements() || sel_feed(feeds(i))) &&
		  (!sel_uvrange.nelements() || inrange ) ) {
		  
                  //                  double antenna_elevation = azimuth_elevation[i].getAngle("deg").getValue()[1];
                  double antenna1_elevation = azimuth_elevation[a1].getAngle("deg").getValue()[1];
                  double antenna2_elevation = azimuth_elevation[a2].getAngle("deg").getValue()[1];

                  if ( antenna1_elevation < lowerlimit ||
                       antenna2_elevation < lowerlimit ||
                       antenna1_elevation > upperlimit ||
                       antenna2_elevation > upperlimit ) {
                      processRow(ifrs(i), it);
                  }
	      }
	  }

      }

      if (shadow) {
	  
	  /*
	    1st loop: Figure out which antennas are shadowed
	    2nd loop: Flag all data (incl self correlations)
	              involving shadowed antennas
	   */
	   
          std::vector<bool> shadowed(diameters.nelements(), false);

	  for (uInt i = 0; i < ifrs.nelements(); i++) {
	      
	      unsigned a1, a2;
	      chunk.ifrToAnt(a1, a2, chunk.ifrNum(i));

	      if (a1 != a2) {  /* Antennas don't shadow themselves. */
                  double d1, d2;
                  if (diameter < 0) {
                    d1 = diameters(a1);
                    d2 = diameters(a2);
                  }
                  else {
                    d1 = diameter;
                    d2 = diameter;
                  }
		  
		  Double uvdist2 = 
		      uvw(i)(0) * uvw(i)(0) + 
		      uvw(i)(1) * uvw(i)(1);
		  
		  /* The relevant threshold distance for shadowing is
		     (d1+d2)/2  */
		  if (uvdist2 < (d1+d2)*(d1+d2)/4.0) {
		      
		      if (0) cerr << "antenna is shadowed " << a1 << "-" << a2 << ": " <<
				 "(u, v, w) = (" << 
				 uvw(i)(0) << ", " <<
				 uvw(i)(1) << ", " <<
				 uvw(i)(2) << ")" << endl;
		      
		      if (uvw(i)(2) > 0) {
			  shadowed[a1] = true;
		      }
		      else {
			  shadowed[a2] = true;
		      }
		  }
	      }
	  }

	  if (0) {
	      std::copy(shadowed.begin(),
			shadowed.end(),
			std::ostream_iterator<bool>(std::cout, "] [" ));
	      cout << endl;
	  }

	  for (uInt i = 0; i < ifrs.nelements(); i++) {
	      
	      unsigned a1, a2;
	      chunk.ifrToAnt(a1, a2, chunk.ifrNum(i));

	      Bool inrange = false;
	      uvdist = sqrt( uvw(i)(0)*uvw(i)(0) + uvw(i)(1)*uvw(i)(1) );
	      for (uInt j = 0; j < sel_uvrange.ncolumn(); j++)
		  if (uvdist >= sel_uvrange(0, j) && uvdist <= sel_uvrange(1, j))
		      inrange |= true;
	      
	      if( (!sel_ifr.nelements() || sel_ifr(ifrs(i))) && 
		  (!sel_feed.nelements() || sel_feed(feeds(i))) &&
		  (!sel_uvrange.nelements() || inrange ) ) {
		  
		  if (shadowed[a1] || shadowed[a2]) {
		      processRow(ifrs(i),it);
		  }
	      }
	  }
      } /* end if shadow */
      
      // flag autocorrelations
      if (sel_autocorr) { 
	  for (uInt i=0; i < ifrs.nelements(); i++) {
	      Bool inrange=false;
	      uvdist = sqrt( uvw(i)(0)*uvw(i)(0) + uvw(i)(1)*uvw(i)(1) );
	      for( uInt j=0; j<sel_uvrange.ncolumn(); j++)
		  if( uvdist >= sel_uvrange(0,j) && uvdist <= sel_uvrange(1,j) ) inrange |= true;
	      //    if( inrange ) cout << "x selected : " << i << " : " << uvdist << endl;
	      if ((!sel_ifr.nelements() || sel_ifr(ifrs(i))) && 
		  (!sel_feed.nelements() || sel_feed(feeds(i))) &&
		  (!sel_uvrange.nelements() || inrange ))
		  {
		      uInt a1,a2;
		      chunk.ifrToAnt(a1,a2,ifrs(i));
		      if( a1==a2 )
			  processRow(ifrs(i),it);
		  }
	  }
      }

      // flag if quacked
      if (quack_si > 0) {
        
        double scan_start;
        double scan_end; 

        if (quack_increment) {
          scan_start = chunk.get_scan_start_unflagged(s0);
          scan_end   = chunk.get_scan_end_unflagged(s0);
          // returns negative if there's no unflagged data
        }
        else {
          scan_start = chunk.get_scan_start(s0);
          scan_end   = chunk.get_scan_end(s0);
        }
	  //cout << "Start time for scan  : " << MVTime( scan_start/C::day).string( MVTime::DMY,7) ;
	  //cout << "   :::  iterTime for " << MVTime( t0/C::day).string( MVTime::DMY,7) << " and scan " << s0;

        if (quack_mode == "beg") {
          if (scan_start > 0 &&
              t0 <= (scan_start + quack_si)) flagall = true;
        }
        else if (quack_mode == "endb") {
          if (scan_end > 0 &&
              t0 >= (scan_end - quack_si)) flagall = true;
        }
        else if (quack_mode == "end") {
          if (scan_end > 0 &&
              t0 < (scan_end - quack_si)) flagall = true;
        }
        else if (quack_mode == "tail") {
          if (scan_start > 0 &&
              t0 > (scan_start + quack_si)) flagall = true;
        }
        else {
          throw(AipsError("Illegal quack mode '" + quack_mode + "'"));
        }
	  
      }

      // flag for specific row-based clipping expressions
      if (sel_clip_row.nelements()) {

        for( uInt i=0; i < ifrs.nelements(); i++ ) {// loop over rows
          for( uInt j=0; j < sel_clip_row.nelements(); j++ ) {

              Float vmin = sel_clip_row[j].vmin;
              Float vmax = sel_clip_row[j].vmax;
              Float val  = sel_clip_row[j].mapper->mapValue(i) - sel_clip_row[j].offset;
              if( (sel_clip_row[j].clip  && ( val<vmin || val>vmax ) ) ||
                  (!sel_clip_row[j].clip && val>=vmin && val<=vmax ) )
                processRow(ifrs(i),it);
          }
        }
      }

      bool clip_based_on_tsys = false;
      if (clip_based_on_tsys) {
          /*
            for each row:
            
            find antenna1 and antenna2
            lookup temperature for 
            (antenna1,spwid,time) and
            (antenna2,spwid,time) in the SYSCAL subtable
            
          */
          cout << "Get sysCal table" << endl;
          // note, this is an optional subtable

          const MSSysCal syscal(chunk.measSet().sysCal());

          ScalarColumn<uInt> sc_antenna_id(syscal, "ANTENNA_ID");
          ScalarColumn<uInt> sc_spwid     (syscal, "SPECTRAL_WINDOW_ID");
          ScalarColumn<Double> sc_time   (syscal, "TIME");
          ArrayColumn<Float> sc_tsys     (syscal, "TSYS");
                    
          unsigned spwid = chunk.visBuf().spectralWindow();
          
          for (uInt i = 0; i < ifrs.nelements(); i++) {
          
              unsigned a1, a2;
              chunk.ifrToAnt(a1, a2, chunk.ifrNum(i));
              
              //for each SYSCAL table row
              // if antenna1 matches or antenna2 matches and
              //    spwid matches and time matches
              //     if (tsys out of allowed range)
              //             flag

              unsigned nrows_syscal = sc_antenna_id.getColumn().nelements();
              cout << nrows_syscal << " rows in SYSCAL table" << endl;
              for (unsigned row = 0; row < nrows_syscal; row++) {
                  cout << "a1, a2 = " << a1 << ", " << a2 << " ?= " << sc_antenna_id(row) << endl;
                  cout << "time   = " << t0 << " ?= " << sc_time(row) << endl;
                  cout << "spwid  = " << spwid << " ?= " << sc_spwid(row) << endl;
                  cout << "tsys   = " << sc_tsys(row) << endl;

                  if ((a1 == sc_antenna_id(row) || a2 == sc_antenna_id(row)) &&
                      spwid == sc_spwid(row) &&
                      t0-dt0/2 < sc_time(row) && sc_time(row) < t0+dt0/2) {
                    cout << "                         MATCH!" << endl;
                  }
              }
          }
      }
      // scan intent based flagging (check match per antenna/baseline) 
      if(sel_stateid.nelements()) {
          const Vector<Int> &stateids( chunk.visBuf().stateId() );
          //for (uInt i = 0; i < ifrs.nelements(); i++) {
          //    for (uInt j = 0; j < sel_stateid.nelements(); j++) {
          //        if( sel_stateid(j) == stateid0 )
          for (uInt i = 0; i < stateids.nelements(); i++) {

	      Bool inrange=false;
	      uvdist = sqrt( uvw(i)(0)*uvw(i)(0) + uvw(i)(1)*uvw(i)(1) );
	      for( uInt j=0; j<sel_uvrange.ncolumn(); j++) 
	           if( uvdist >= sel_uvrange(0,j) && uvdist <= sel_uvrange(1,j) ) inrange |= true;

              for (uInt j = 0; j < sel_stateid.nelements(); j++) {
                  if( stateids(i) == sel_stateid(j) && 
                      ifrs.nelements()==stateids.nelements() &&
                      (!sel_ifr.nelements()||sel_ifr(ifrs(i))) &&
                      (!sel_feed.nelements() || sel_feed(feeds(i))) &&
                      (!sel_uvrange.nelements() || inrange ) )
                      processRow(ifrs(i),it);
              }
          }
      }
  }

  // flag whole selection, if still needed
  if (flagall)
  {
    for (uInt i=0; i<ifrs.nelements(); i++) // loop over rows
    {
        Bool inrange=false;
        uvdist = sqrt( uvw(i)(0)*uvw(i)(0) + uvw(i)(1)*uvw(i)(1) );
        for( uInt j=0; j<sel_uvrange.ncolumn(); j++)
                if( uvdist >= sel_uvrange(0,j) && uvdist <= sel_uvrange(1,j) ) inrange |= true;
        if( (!sel_ifr.nelements() || sel_ifr(ifrs(i))) && 
            (!sel_feed.nelements() || sel_feed(feeds(i))) &&
            (!sel_uvrange.nelements() || inrange ) )
                processRow(ifrs(i),it);
    }
  }
  
  return RFA::CONT;
}

// -----------------------------------------------------------------------
// iterRow
// -----------------------------------------------------------------------
RFA::IterMode RFASelector::iterRow (uInt ir)
{
  uInt ifr = chunk.ifrNum(ir);

  if( sel_clip.nelements() && sum_sel_clip_active )
  {
      // apply data flags
      for( uInt j=0; j<sel_clip.nelements(); j++ ) {
      
          if (sel_clip[j].channel_average) {
              // Compute average
              Float average = 0;
              unsigned n_unflagged = 0;

              for (uInt ich=0; ich < num(CHAN); ich++ ) {
                
                  // if active channel, and not flagged...
                  if ((!flagchan.nelements() || flagchan(ich))
                      &&
                      !flag.preFlagged(ich, ifr)) {
                    
                    
                      average += sel_clip[j].mapper->mapValue(ich, ir);
                      n_unflagged += 1;
                  }
              }
          

              // Decide whether to flag row (or active channels), based on average
              //cout << "number of unflagged channels = " << n_unflagged << endl;
              if (n_unflagged > 0) {
                  average = average / n_unflagged;
                
                  //cout << " average = " << average << endl;
                  
                  Float vmin = sel_clip[j].vmin;
                  Float vmax = sel_clip[j].vmax;
                  Float val = average;

                  if( ( sel_clip[j].clip && ( val <  vmin || val > vmax ) ) ||
                      (!sel_clip[j].clip &&   vmin <= val && val <= vmax   ) )

                      /* No channel selections, flag all channels.
                         
                      jmlarsen: Unfortunately, clearRowFlag and setRowFlag
                      don't seem to work, therefore don't do like this.
                      (This could probably be brought to work by
                      fixing RFFlagCube::setMSFlags() to use the flagrow.)
                      
                      cout << " flag entire row " << endl;
                      unflag ? 
                      flag.clearRowFlag(ifr, it) :
                      flag.setRowFlag(ifr, it);
                      */
                      
                      for (uInt ich = 0; ich < num(CHAN); ich++) {
                        if (!flagchan.nelements() || flagchan(ich)) {
                            unflag ?
                              flag.clearFlag(ich, ifr) :
                              flag.setFlag(ich, ifr);
                        }
                    }
              }
              else {
              /* If all channels were already flagged, don't flag/unflag anything. */
              }
          }
          else {
            /* No averaging */
            for( uInt ich=0; ich<num(CHAN); ich++ ) {
              if( !flagchan.nelements() || flagchan(ich) ) {
                Float vmin = sel_clip[j].vmin;
                Float vmax = sel_clip[j].vmax;
                Float val  = sel_clip[j].mapper->mapValue(ich,ir);

                // jagonzal: Added ISO isnan check to catch extremely large values (CAS-3355)
                if( ( sel_clip[j].clip && (val<vmin || val>vmax || std::isnan(val)) ) ||
                    (!sel_clip[j].clip && val>=vmin && val<=vmax   ) )
                  unflag ? flag.clearFlag(ich,ifr) : flag.setFlag(ich,ifr);
              }
            }
          }
      }
  }

  return RFA::CONT;
}

/* Flush flags after each time stamp, if in kiss mode */
void RFASelector::endRows(uInt it)
{
    if (only_selector) {
        flag.advance(it);
        flag.setMSFlags(it);
    }
}
    
void RFASelector::iterFlag(uInt it)
{
    if (!only_selector) {
        RFAFlagCubeBase::iterFlag(it);
    }
}

String RFASelector::getDesc ()
{
  return desc_str+" "+RFAFlagCubeBase::getDesc();
}

const RecordInterface & RFASelector::getDefaults ()
{
  static Record rec;
// create record description on first entry
  if( !rec.nfields() )
  {
    rec = RFAFlagCubeBase::getDefaults();
    rec.removeField(RF_FIGNORE); // fignore is meaningless
    rec.define(RF_NAME,"Selector");
    rec.define(RF_SPWID,false);
    rec.define(RF_FIELD,false);
    rec.define(RF_FREQS,false);
    rec.define(RF_CHANS,false);
    rec.define(RF_CORR,false);
    rec.define(RF_ANT,false);
    rec.define(RF_BASELINE,false);
    rec.define(RF_TIMERANGE,false);
    rec.define(RF_AUTOCORR,false);
    rec.define(RF_CENTERTIME,false);
    rec.define(RF_TIMEDELTA,10.0);
    rec.define(RF_QUACK,false);
    rec.define(RF_CLIP,false);
    rec.define(RF_CHANAVG, false);
    rec.define(RF_FLAGRANGE,false);
    rec.define(RF_UNFLAG,false);
    rec.define(RF_SHADOW,false);
    rec.define(RF_ELEVATION, false);
    rec.define(RF_SCAN,false);
    rec.define(RF_INTENT,false);
    rec.define(RF_ARRAY,false);
    rec.define(RF_FEED,false);
    rec.define(RF_UVRANGE,false);
    rec.define(RF_COLUMN,false);
    rec.define(RF_DIAMETER, false);
    rec.define(RF_LOWERLIMIT, false);
    rec.define(RF_UPPERLIMIT, false);
    
    rec.setComment(RF_SPWID,"Restrict flagging to specific spectral windows (integers)");
    rec.setComment(RF_FIELD,"Restrict flagging to specific field IDs or field names (integers/strings)");
    rec.setComment(RF_FREQS,"Restrict flagging to specific frequency ranges (2,N array of doubles:MHz)");
    rec.setComment(RF_CHANS,"Restrict flagging to specific channels (2,N array of integers)");
    rec.setComment(RF_CORR,"Restrict flagging to specific correlations (array of strings)");
    rec.setComment(RF_ANT,"Restrict flagging to specific antennas (array of strings/integers)");
    rec.setComment(RF_BASELINE,"Restrict flagging to specific baselines (array of strings, e.g., 'A1-A2','A1-*', or 2,N array of integers [[A1,A2],[B1,B2],...])");
    rec.setComment(RF_TIMERANGE,"Restrict flagging to specific time ranges (2,N array of strings or MJDs");
    rec.setComment(RF_AUTOCORR,"Flag autocorrelations (F/T)");
    rec.setComment(RF_CENTERTIME,"Flag specific timeslots (array of strings or MJDs)");
    rec.setComment(RF_TIMEDELTA,String("Time delta for ")+RF_CENTERTIME+", in seconds");
    rec.setComment(RF_QUACK,"Use [SI,DT] for VLA quack-flagging");
    rec.setComment(RF_CLIP,"Flag outside a specific range of values");
    rec.setComment(RF_CHANAVG, "Flag based on channel average");
    rec.setComment(RF_FLAGRANGE,"Flag inside a specific range of values");
    rec.setComment(RF_UNFLAG,"If T, specified flags are CLEARED");
    rec.setComment(RF_SHADOW, "If T, flag shadowed antennas");
    rec.setComment(RF_ELEVATION, "If T, flag based on elevation");
    rec.setComment(RF_SCAN,"Restrict flagging to specific scans (integers)");
    rec.setComment(RF_INTENT,"Restrict flagging to specific scan intent -corresponding state IDs (integers)");
    rec.setComment(RF_ARRAY,"Restrict flagging to specific array ids (integers)");
    rec.setComment(RF_FEED,"Restrict flagging to specific feeds (2,N array of integers)");
    rec.setComment(RF_UVRANGE,"Restrict flagging to specific uv-distance ranges in meters (2,N array of doubles )");
    rec.setComment(RF_COLUMN,"Data column to clip on.");
    rec.setComment(RF_DIAMETER, "Effective diameter to use. If negative, the true antenna diameters are used");
    rec.setComment(RF_LOWERLIMIT, "Limiting elevation");
    rec.setComment(RF_UPPERLIMIT, "Limiting elevation");
  }
  return rec;
}

} //# NAMESPACE CASA - END

