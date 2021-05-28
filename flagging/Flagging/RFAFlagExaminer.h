//# RFAFlagExaminer.h: this defines RFAFlagExaminer
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
#ifndef FLAGGING_RFAFLAGEXAMINER_H
#define FLAGGING_RFAFLAGEXAMINER_H

#include <flagging/Flagging/RFAFlagCubeBase.h> 
#include <flagging/Flagging/RFASelector.h> 
#include <flagging/Flagging/RFDataMapper.h>
#include <casa/Arrays/LogiVector.h>
    
namespace casa { //# NAMESPACE CASA - BEGIN

// <summary>
// RFAFlagExaminer: Examine the flags and get statistics. perform extensions too.
// </summary>

// <use visibility=local>

// <reviewed reviewer="" date="" tests="" demos="">
// </reviewed>

// <prerequisite>
//   <li> RFASelector
// </prerequisite>
//
// <etymology>
// RedFlaggerAgent Selector
// </etymology>
//
// <synopsis>
// RFAFlagExaminer accepts a whole bunch of options to select a subset of the
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

class RFAFlagExaminer : public RFASelector
{
public:
// constructor. 
  RFAFlagExaminer ( RFChunkStats &ch,const casacore::RecordInterface &parm ); 
  virtual ~RFAFlagExaminer ();
  
  virtual void iterFlag( casacore::uInt it );
  virtual IterMode iterRow( casacore::uInt irow );
  virtual casacore::Bool newChunk(casacore::Int &maxmem);
  virtual void endChunk();


  virtual void startData(bool verbose){RFAFlagCubeBase::startData(verbose);return;};
  virtual void startFlag(bool verbose);
  virtual void endFlag();
  virtual void finalize();
  virtual void initialize();
  virtual void initializeIter(casacore::uInt it);
  virtual void finalizeIter(casacore::uInt it);
  virtual casacore::String getID() {return casacore::String("FlagExaminer");};

  virtual casacore::Record getResult();

//  virtual casacore::String getDesc ();
//  static const casacore::RecordInterface & getDefaults ();

private:
    void processRow  ( casacore::uInt ifr,casacore::uInt it ) ;
    casacore::uInt64 totalflags,totalcount;
    casacore::uInt64 totalrowflags,totalrowcount;

    // accumulated over all chunks
    casacore::uInt64 
      accumTotalFlags, accumTotalCount, /* accumRowFlags, */
      accumTotalRowCount, accumTotalRowFlags;

    // per chunk
    casacore::uInt64 inTotalFlags, inTotalCount, inTotalRowFlags, inTotalRowCount;
    casacore::uInt64 outTotalFlags, outTotalCount, outTotalRowFlags, outTotalRowCount;
    
    // Statistics per antenna, baseline, spw, etc.
    // These maps of maps is used e.g. like:
    //
    //        accumflags["baseline"]["2&&7"] == 42
    //        accumflags["spw"     ]["0"   ] == 17
    //
    // which means that there were 42 flags on baseline 2 - 7, etc.
    std::map<std::string, std::map<std::string, casacore::uInt64> > accumflags;
    std::map<std::string, std::map<std::string, casacore::uInt64> > accumtotal;
    
    std::vector<casacore::uInt64> accumflags_channel;
    std::vector<casacore::uInt64> accumtotal_channel;
    std::vector<casacore::uInt64> accumflags_correlation;
    std::vector<casacore::uInt64> accumtotal_correlation;

};

} //# NAMESPACE CASA - END

#ifndef AIPS_NO_TEMPLATE_SRC
#include <flagging/Flagging/RFAFlagExaminer.tcc>
#endif //# AIPS_NO_TEMPLATE_SRC
#endif
