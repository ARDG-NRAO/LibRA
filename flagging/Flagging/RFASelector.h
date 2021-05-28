
//# RFASelector.h: this defines RFASelector
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
#ifndef FLAGGING_RFASELECTOR_H
#define FLAGGING_RFASELECTOR_H

#include <flagging/Flagging/RFAFlagCubeBase.h> 
#include <flagging/Flagging/RFDataMapper.h>
#include <ms/MeasurementSets/MSColumns.h>
#include <casa/Arrays/LogiVector.h>
    
namespace casa { //# NAMESPACE CASA - BEGIN

// <summary>
// RFASelector: flags pixels/rows based on a specified selection
// </summary>

// <use visibility=local>

// <reviewed reviewer="" date="" tests="" demos="">
// </reviewed>

// <prerequisite>
//   <li> RFAFlagCubebase
// </prerequisite>
//
// <etymology>
// RedFlaggerAgent Selector
// </etymology>
//
// <synopsis>
// RFASelector accepts a whole bunch of options to select a subset of the
// casacore::MS (by time, antenna, baseline, channel/frequency, etc.), and to flag/unflag 
// the whole selection, or specific parts of it (autocorrelations, specific 
// time slots, VLA quacks, etc.)
// </synopsis>
//
// <todo asof="2001/04/16">
//   <li> add this feature
//   <li> fix this bug
//   <li> start discussion of this possible extension
// </todo>

class RFASelector : public RFAFlagCubeBase
{
public:
// constructor. 
  RFASelector ( RFChunkStats &ch,const casacore::RecordInterface &parm ); 
  virtual ~RFASelector ();
  
  virtual casacore::uInt estimateMemoryUse () { return RFAFlagCubeBase::estimateMemoryUse()+2; }
  virtual casacore::Bool newChunk ( casacore::Int &maxmem );
  virtual IterMode iterTime ( casacore::uInt it );
  virtual void endRows(casacore::uInt itime);
  virtual IterMode iterRow  ( casacore::uInt ir );
  virtual void iterFlag(casacore::uInt itime);
  virtual void startData(bool verbose);

  virtual casacore::String getDesc ();
  static const casacore::RecordInterface & getDefaults ();

  casacore::Bool fortestingonly_parseMinMax( casacore::Float &vmin,casacore::Float &vmax,const casacore::RecordInterface &spec,casacore::uInt f0 );
  void fortestingonly_parseClipField( const casacore::RecordInterface &spec,casacore::Bool clip );

protected:
  typedef struct ClipInfo {
      RFDataMapper *mapper; 
      casacore::Float vmin, vmax; 
      casacore::Bool channel_average; // average data over channels?
      casacore::Bool clip;            // flag outside range if true (otherwise flag inside)
      casacore::Float offset;         // offset added to value (used for angles, etc.)
  } ClipInfo;
  
    
  template<class T> casacore::Bool reformRange( casacore::Matrix<T> &rng,const casacore::Array<T> &arr );
  template<class T> casacore::Bool parseRange( casacore::Matrix<T> &rng,const casacore::RecordInterface &parm,const casacore::String &id );
  template<class T> casacore::Bool find( casacore::uInt &index,const T &obj,const casacore::Vector<T> &arr );
  
  casacore::Bool parseTimes  ( casacore::Array<casacore::Double> &times,const casacore::RecordInterface &parm,const casacore::String &id,casacore::Bool secs=false );
  void addString   ( casacore::String &str,const casacore::String &s1,const char *sep=" " );
  virtual void processRow  ( casacore::uInt ifr,casacore::uInt it );
  casacore::Bool parseMinMax ( casacore::Float &vmin,casacore::Float &vmax,const casacore::RecordInterface &spec,casacore::uInt f0 );
  void addClipInfo ( const casacore::Vector<casacore::String> &expr,casacore::Float vmin,casacore::Float vmax,casacore::Bool clip, casacore::Bool channel_average );
  void parseClipField  ( const casacore::RecordInterface &spec,casacore::Bool clip );
  void addClipInfoDesc ( const casacore::Block<ClipInfo> &clip );

  // shadow mode
  casacore::Double diameter;   /* diameter to use. If negative use 
                        the diameters array (true antenna diameters)
                     */
  casacore::Vector< casacore::Double > diameters;
  casacore::MSAntennaColumns *ac;

  // elevation
  double lowerlimit;
  double upperlimit;

// description of agent
  casacore::String desc_str;
// selection arguments
  casacore::Matrix<casacore::Double> sel_freq,sel_time,sel_timerng,sel_uvrange;
  casacore::Matrix<casacore::Int>    sel_chan;
  casacore::Vector<casacore::Int>    sel_corr,sel_spwid,sel_fieldid, sel_stateid;
  casacore::Vector<casacore::String>  sel_fieldnames;
  casacore::LogicalVector  sel_ifr,flagchan,sel_feed;
  casacore::Bool          sel_autocorr,unflag;
  casacore::Block<ClipInfo> sel_clip,sel_clip_row;
  casacore::LogicalVector  sel_clip_active;
  casacore::Bool            sum_sel_clip_active;
  casacore::Double        quack_si, quack_dt;
  casacore::String        quack_mode;
  casacore::Bool          quack_increment;
  casacore::Vector<casacore::Int>   sel_scannumber,sel_arrayid,sel_observation;
  casacore::String        sel_column;

  casacore::Bool select_fullrow,flag_everything, shadow, elevation;

};

    
    

} //# NAMESPACE CASA - END

#ifndef AIPS_NO_TEMPLATE_SRC
#include <flagging/Flagging/RFASelector.tcc>
#endif //# AIPS_NO_TEMPLATE_SRC
#endif
