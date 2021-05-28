//# SelecteAverageSpwChan.h: 
//# Copyright (C) 2003
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
//#
//#
//# -------------------------------------------------------------------------


#if !defined SAS_H
#define SAS_H


#include <casa/aips.h>

#include <tables/Tables/SetupNewTab.h>
//#include <tables/Tables/Table.h>
#include <tables/Tables/MemoryTable.h>
#include <tables/Tables/TableDesc.h>
#include <tables/DataMan/StManAipsIO.h>

#include <ms/MeasurementSets/MSColumns.h>
#include <ms/MeasurementSets/MeasurementSet.h>

#include <list>

#include <msvis/MSVis/VisBuffer.h>
#include <msvis/MSVis/VisibilityIterator.h>
#include <msvis/MSVis/VisSet.h>



///////////////////////////////////////////////////////////////////////////////
//# Start of documentation.    
//
// <summary>
// A class to select/average spectral windows and associated channels
// </summary>
//
// <use visibility=export>
//
// <reviewed reviewer="Me" date="2008/07/07" tests="" demos="">
// </reviewed>

// <prerequisite>
//   <li> MeasurementSet
// </prerequisite>
//
// <etymology>
// </etymology>
//
// <synopsis>
// </synopsis>
//
// <example>
// </example>
//
// <motivation>
// </motivation>
//
// <thrown>
//    <li>
//    <li>
// </thrown>
//
// <todo asof="2008/07/06">
//   <li> everything, and
//   <li> everything else
// </todo>

//# End of documentation.    
///////////////////////////////////////////////////////////////////////////////

namespace casa {

class SelectAverageSpw;

typedef SelectAverageSpw SAS;

class SelectAverageSpw
{
public:
   casacore::Int spwid;
   casacore::Int desc;
   casacore::Double rFreq;
   casacore::Int measFreqRef;

   //selected channels
   casacore::Vector<casacore::Int> chans;

   //averaged channels
   casacore::Vector<casacore::Double> aveFreqs;
   casacore::Vector<casacore::Int> aveChans;
   casacore::Vector<casacore::Int> sxsChans;
   casacore::Vector<casacore::String> aveChanNames; 
   casacore::Matrix<casacore::Int> aveChanMaps; 

   //velocity depends on both freq and field and
   //can not be a member of this


   static casacore::Int nextSelected(casacore::Int spw, casacore::Int currId, casacore::Matrix<casacore::Int>& cList);
   static casacore::Int selectAverageChan(casacore::MS*, const casacore::Matrix<casacore::Int>& chanList,
                                casacore::Vector<SAS>& sp, const casacore::Int& aveChan = 1);
   static void averageVelocity(casacore::Bool &sorry,
                               casacore::MS*, casacore::Vector<SAS>& sp, casacore::Vector<casacore::Double>& velo,
                               const casacore::Int& spwidx, const casacore::Int& filed = 0,
                               const casacore::String& restfreq = "",
                               const casacore::String& frame = "",
                               const casacore::String& doppler = "");
   static void showSASC(const casacore::Vector<SAS>& sp);
   static void chanMap(casacore::Matrix<casacore::Int>& cmap, const casacore::Vector<SAS>& sp);
   static void showSASC();
   static casacore::Int descBySpw(const casacore::Int& spid, const casacore::Vector<SAS>& sp);
   static casacore::Int spwByDesc(const casacore::Int& desc, const casacore::Vector<SAS>& sp);
   static casacore::Int spwIndexByDesc(const casacore::Int& desc, const casacore::Vector<SAS>& sp);
   static casacore::Int spwIndexBySpw(const casacore::Int& spid, const casacore::Vector<SAS>& sp);

   static const casacore::Int maxChan;

  
};


} 
#endif


