//# RFASpectralRej.h: this defines RFASpectralRej
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
#ifndef FLAGGING_RFASPECTRALREJ_H
#define FLAGGING_RFASPECTRALREJ_H

#include <flagging/Flagging/RFAFlagCubeBase.h> 
#include <flagging/Flagging/RFDataMapper.h> 
#include <flagging/Flagging/RFFlagCube.h> 
#include <flagging/Flagging/RFRowClipper.h> 
#include <casacore/scimath/Functionals/Polynomial.h>
#include <casacore/scimath/Fitting/LinearFit.h>
#include <casacore/casa/Arrays/LogiVector.h>
    
namespace casa { //# NAMESPACE CASA - BEGIN

// <summary>
// RFASpectralRej: Spectral rejection agent
// </summary>

// <use visibility=local>

// <reviewed reviewer="" date="" tests="" demos="">
// </reviewed>

// <prerequisite>
//   <li> RFDataMapper
//   <li> RFFlagCubeBase
//   <li> RFRowClipper
// </prerequisite>
//
// <synopsis>
// RFASpectralRej fits a polynomial to a specified segment of the spectrum 
// (presumably, spectral line-free). The fit produces a noise estimate for
// each row. Rows with excessive noise levels are flagged using RFRowClipper.
// </synopsis>
//
// <todo asof="2001/04/16">
//   <li> add this feature
//   <li> fix this bug
//   <li> start discussion of this possible extension
// </todo>

class RFASpectralRej : public RFAFlagCubeBase, public RFDataMapper
{
public:
  RFASpectralRej  ( RFChunkStats &ch,const casacore::RecordInterface &parm ); 
  virtual ~RFASpectralRej () {};
  
  virtual casacore::uInt estimateMemoryUse () { return RFAFlagCubeBase::estimateMemoryUse()+2; }
  virtual casacore::Bool newChunk (casacore::Int &maxmem);
  virtual void endChunk ();
  virtual void startData (bool verbose);
  virtual IterMode iterTime (casacore::uInt it);
  virtual IterMode iterRow  (casacore::uInt ir);
  virtual IterMode endData  ();

  virtual casacore::String getDesc ();
  static const casacore::RecordInterface & getDefaults ();
  
protected:
  void addSegment  ( casacore::Int spwid,casacore::Double fq0,casacore::Double fq1,casacore::Int ch0,casacore::Int ch1 );
  void parseRegion ( const casacore::RecordInterface &parm );
    
  // spectral region specifications
  typedef struct Segment { casacore::Int spwid; casacore::Double fq0,fq1; casacore::Int ch0,ch1; } Segment;
  
  casacore::Block<Segment> segments;
  casacore::LogicalVector fitchan;
  casacore::uInt num_fitchan;

  casacore::uInt       ndeg,halfwin;
  casacore::Double     threshold;  
  
  RFRowClipper rowclipper; // row clipper object

  casacore::Polynomial<casacore::AutoDiff<casacore::Float> > poly; // fitted polynomial
  casacore::LinearFit<casacore::Float> fitter;     // fitter object
  
  casacore::Double     xnorm;
};


} //# NAMESPACE CASA - END

#endif
