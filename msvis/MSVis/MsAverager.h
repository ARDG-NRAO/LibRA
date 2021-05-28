//# MsAverager.h: Measurement Sets (Visiablity Averager)
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

#if !defined MSAVERAGER_H
#define MSAVERAGER_H


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
//#include <msvis/MSVis/SubMS.h>
#include <msvis/MSVis/SelectAverageSpw.h>



///////////////////////////////////////////////////////////////////////////////
//# Start of documentation.    
//
// <summary>
// A class to average a visibilty data set in time and/or channel
// </summary>
//
// <use visibility=export>
//
// <reviewed reviewer="Me" date="2007/12/25" tests="" demos="">
// </reviewed>

// <prerequisite>
//   <li> VisBuffer
//   <li> MeasurementSet
// </prerequisite>
//
// <etymology>
// From "visibility", "time", "channel", "polarization" and "averaging".
// </etymology>
//
// <synopsis>
// This class averages VisBuffers in time, channel.
// </synopsis>
//
// <example>
//   casacore::MS ms("ngc5921.ms" casacore::Table::Update);
//   MsAverager msa(ms);
//   //chanlist is matrix of int its rows [spwid, start, end, step] 
//   msa.setAverager(chanlist, 90, 32, "data", "SCALAR");
//   if (msa.ok()) { 
//      casacore::MS avems;
//      msa.getMS(avems);   
//   }
//   
// </example>
//
// <motivation>
// Provide time/channel averaging capabilities for plotxy. 
// </motivation>
//
// <thrown>
//    <li>
//    <li>
// </thrown>
//
// <todo asof="2007/12/25">
//   <li> everything, and
//   <li> everything else
// </todo>

//# End of documentation.    
///////////////////////////////////////////////////////////////////////////////

namespace casa {


class MsAverager
{

public:
   enum OutputMode {
     TableMS = 1,   
     ListBuffer
   };

   MsAverager(casacore::MS*, OutputMode = MsAverager::TableMS);
   ~MsAverager();

   void reset(casacore::MS*, OutputMode = MsAverager::TableMS);
   void getMS(casacore::MS& ms);
   void getXY(casacore::Vector<casacore::Double>& x, casacore::Vector<casacore::Double>& y, 
              casacore::Vector<casacore::Int>& f, casacore::Int pol);
   void getMap(casacore::Matrix<casacore::Int>& rowMap, casacore::Matrix<casacore::Int>& chanMap);
    
   //average engine and its status 
   void setAverager(
              const casacore::Matrix<casacore::Int>& chanList,
              const casacore::Matrix<casacore::Int>& baselines,
              casacore::Double aveTime, casacore::Int aveChan,
              const casacore::String& column = "DATA",
              const casacore::String& aveMode = "VECTOR",
              const casacore::Bool& aveFlag = false,
              const casacore::Bool& aveScan = false,
              const casacore::Bool& aveBline = false,
              const casacore::Bool& aveArray = false,
              const casacore::Bool& aveVelo = false,
              const casacore::String& restfreq = "",
              const casacore::String& frame = "",
              const casacore::String& doppler = "");
   casacore::Bool ok() {return aveOK;}
   casacore::Int outputFormat() {return outputMode;}

   void showColumnNames();
   casacore::Bool isDataColumn(const casacore::String& colNmae);
   casacore::Bool hasColumn(const casacore::String& colName);
   static casacore::Int baselineRow(const casacore::Int& nAnt, const casacore::Int& a, const casacore::Int& b); 
   casacore::Int baselineRow(const casacore::Int& a = -1, const casacore::Int& b = -1); 

   void putAveBuffer(casacore::Double bufTime, casacore::Int bufField, casacore::Int bufScan,
                     casacore::Int bufArray, VisBuffer& p, casacore::Int nTime);
   void initAveBuffer(casacore::Double bufTime, VisBuffer& p, casacore::Int nAnt, casacore::Int nChan);
   void putAveTable(casacore::Double bufTime, casacore::Int bufField, casacore::Int bufScan,
                    casacore::Int bufArray, casacore::Int bufSpw, VisBuffer& p, casacore::Int nTime, 
                    casacore::Double timeShift);
   void showVisRow(casacore::Cube<casacore::Complex>& vc, casacore::Int row);
   void showMsRow(casacore::MSMainColumns* msc, casacore::Int row);
   void showAveMap(casacore::Matrix<casacore::Int>& rmap, casacore::Matrix<casacore::Int>& cmap);

   const static casacore::String DataColumn[4];
   casacore::Int npol() {return nAvePol;}
   casacore::Int nchan() {return nAveChan;}
   casacore::Int nAnt() {return nAntenna;}

private:
   //to be averaged
   void cleanup();
   casacore::MS* pMS;
   VisSet* vs;
   casacore::String msName;
   //casacore::MSDerivedValues *msdv;

   //averaged, store as MS
   casacore::MS aMS;
   casacore::Int msRow;
   casacore::MSMainColumns *msc;
   //casacore::MSColumns *msc;
   
   //averaged, store as VBs 
   VisBuffer* pAveBuff;
   std::list<VisBuffer*> aveList;

   //averaging inputs, ideally, calculate only needed column
   casacore::String column;

   //selected channels for each piece for spw expression
   //each row contains [spwid, start, end, stride]
   //may contains multiple rows of same spwid
   casacore::Matrix<casacore::Int> chanList;

   casacore::Vector<SAS> spw;

   //map between averaged and original 
   casacore::Matrix<casacore::Int> aveChanMap;
   //casacore::Matrix<casacore::Double> aveTimeMap;
   casacore::Matrix<casacore::Int> aveRowMap;

   //averaging requirements
   casacore::String aveMode;
   casacore::Double aveTime;
   casacore::Int aveChan;
   casacore::Bool aveFlag;
   casacore::Bool crossScans;
   casacore::Bool crossBlines;
   casacore::Bool crossArrays;
   casacore::Bool crossSpws;
   casacore::Bool aveVel;
   casacore::Bool sorryVel;
   casacore::Matrix<casacore::Int> baselines;

   //size of the averaged
   casacore::Int nAveChan;
   casacore::Int nAveTime;
   casacore::Int nAveRow;
   casacore::Int nAvePol;
   casacore::Int nAntenna;

   casacore::String restfreq;
   casacore::String frame;
   casacore::String doppler;

   casacore::Bool aveOK;

   //utility
   static const casacore::String clname;
   //SLog *log;

   int outputMode;

  
};


} 
#endif


