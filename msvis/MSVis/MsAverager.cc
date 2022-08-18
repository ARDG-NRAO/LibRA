//# MsAverager.cc: Implementation of MsAverager.h
//# Copyright (C) 1996,1997,1998,1999,2000,2002,2003
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
//----------------------------------------------------------------------------


#include <msvis/MSVis/SelectAverageSpw.h>
#include <msvis/MSVis/MsAverager.h>
#include <casacore/casa/Exceptions/Error.h>
#include <casacore/casa/Arrays/ArrayMath.h>
#include <casacore/casa/Arrays/Slice.h>
#include <casacore/casa/OS/Timer.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <iostream>
#include <iomanip>
#include <cmath>

#define LOG2 0

#if LOG2
#define IfLog2(x) x
#else
#define IfLog2(x) /*x*/
#endif

using namespace casacore;
namespace casa { 

const String MsAverager::clname = "MsAverager";

const String MsAverager::DataColumn[] = {"DATA", "CORRECTEDDATA", 
                                         "MODELDATA", "RESIDUAL"};

MsAverager::MsAverager(MS* ms, OutputMode mode) {
   aveOK = false;

   //log = SLog::slog();
   reset(ms, mode);
}

void MsAverager::reset(MS* ms, OutputMode mode) {
   //cout << "MS=" << *ms << endl;
   aveOK = false;
   outputMode = mode;

   if (!ms) {
	   LogIO os(LogOrigin("MsAverager", "reset"));
	   os << LogIO::WARN << "Could not reset MsAverager: input MS is NULL" << LogIO::POST;
      //SLog::slog()->out("Could not reset MsAverager: input MS is NULL",
                //"MsAverager", clname, LogMessage::WARN, true);    
      return;
   }

   //SLog::slog()->out(String("Number of selected rows is ") + 
    //             String::toString(ms->nrow()),
     //           "MsAverager", clname, LogMessage::DEBUG1);    

   pMS = ms;
   vs = 0;
   pAveBuff = 0;

   spw.resize(0);

   //Block<int> sort(4);
   //sort[0] = MS::DATA_DESC_ID;
   //sort[1] = MS::FIELD_ID;
   //sort[2] = MS::TIME;
   //sort[3] = MS::ARRAY_ID;

   //Matrix<Int> allChannels;
   //Double intrvl = 2;
   //vs = new VisSet(*pMS, sort, allChannels, intrvl);

   TableDesc td = pMS->tableDesc();
   String aveTable=String("casapy.scratch-0.table");
   if (!Table::isOpened(aveTable)) {
      SetupNewTable aveSetup(aveTable, td, Table::Scratch);
      StManAipsIO stm;
      aveSetup.bindAll(stm);

      //if (mspointer!=NULL) delete mspointer;
      //mspointer=0;
      //MS* mspointer= SubMS::setupMS(aveTable, 10, 4, "VLA");
      //aMS=*mspointer;

   aMS = MS();
      aMS = MS(aveSetup, 0, false);
      msc = new MSColumns(aMS);
      //cout << "---aMS=" << aMS << endl;
   }

   msName = pMS->tableName(); 
   //msdv = new MSDerivedValues;
   //msdv->setMeasurementSet(*pMS);
   //msdv->setVelocityReference(MDoppler::RADIO);
   //msdv->setFrequencyReference(MFrequency::LSRK);
}

MsAverager::~MsAverager() {
   cleanup();
}

void MsAverager::cleanup() {
   if (pMS) {
      //delete pMS;
      pMS = 0;
   }
   if (vs) {
      //delete vs;
      vs = 0;
   }
   if (msc) {
      //delete msc;
      msc = 0;
   }
   if (pAveBuff) {
      //delete pAveBuff;
      pAveBuff = 0;
   }  
}


void MsAverager::setAverager(
                   const Matrix<Int>& cList,
                   const Matrix<Int>& bls,
                   Double aveT, Int aveC, 
                   const String& col,
                   const String& aveM,
                   const Bool& aveF,
                   const Bool& crossscan,
                   const Bool& crossbline,
                   const Bool& crossarray,
                   const Bool& aveVelo,
                   const String& restfre,
                   const String& fram,
                   const String& doppl) {
    String fnname = "average";
    //static uInt loop = 0;

    for (Int i = aMS.nrow() - 1; i >= 0; i--) {
       aMS.removeRow(i);
    }
    //cout << "aMS rows=" << aMS.nrow() << endl;
    //showColumnNames();
    LogIO os(LogOrigin("MsAverager", "setAverager"));
    if (upcase(col)=="MODEL")
    if (!hasColumn("MODEL_DATA")) {
       os << LogIO::WARN << "The MS does not have MODEL_DATA column" << LogIO::POST;
       return;
    }
    if (upcase(col)=="CORRECTED")
    if (!hasColumn("CORRECTED_DATA")) {
       os << LogIO::WARN << "The MS does not have CORRECTED_DATA column" << LogIO::POST;
       return;
    }
    if (upcase(col)=="RESIDUAL") {
    if (!hasColumn("CORRECTED_DATA")) {
       os << LogIO::WARN << "The MS does not have CORRECTED_DATA column" << LogIO::POST;
       return;
    }
    if (!hasColumn("MODEL_DATA")) {
       os << LogIO::WARN << "The MS does not have MODEL_DATA column" << LogIO::POST;
       return;
    }
    }
       
    column = col;
    if (column == "CORRECTED") {
       column = "CORRECTEDDATA";
    }
    if (column == "MODEL") {
       column = "MODELDATA";
    }
    if (column == "RESIDUAL") {
       column = "RESIDUAL";
    }

    crossSpws = false;
    if (aveC==1234567) {
        crossSpws = true;
    }

    Matrix<Int> allChannels;
    Double intrvl = 2;
    if (crossSpws) {
       //cout << "crossSpws=true sort by time" << endl;
       Block<int> sort(2);
       sort[0] = MS::TIME;
       //sort[1] = MS::FIELD_ID;
       sort[1] = MS::DATA_DESC_ID;
       //sort[3] = MS::ARRAY_ID;
       vs = new VisSet(*pMS, sort, allChannels, intrvl);
    }
    else {    
       //cout << "crossSpws=false sort by spw" << endl;
       Block<int> sort(4);
       sort[0] = MS::DATA_DESC_ID;
       sort[1] = MS::FIELD_ID;
       sort[2] = MS::TIME;
       sort[3] = MS::ARRAY_ID;
       vs = new VisSet(*pMS, sort, allChannels, intrvl);
    }
    
    aveFlag = aveF;
    aveMode = aveM;
    aveChan = aveC;
    aveTime = aveT;
    crossScans = crossscan;
    crossBlines = crossbline;
    crossArrays = crossarray;
    aveVel = aveVelo;
    sorryVel = true;

    baselines.resize(bls.shape()[0], bls.shape()[1]);
    baselines = bls;

    restfreq = restfre;
    frame = fram;
    doppler = doppl;

    //cout 
    //     << "aveFlag=" << aveFlag << " aveMode=" << aveMode
    //     << " aveChan=" << aveChan << " aveTime=" << aveTime 
    //     << " aveC=" << aveC << " crossScans=" << crossscan 
    //     << " crossBlines=" << crossbline
    //     << " crossArrays=" << crossarray 
    //     << " crossSpws=" << crossSpws
    //     << " aveVel=" << aveVel 
    //     << " baselines=" << baselines 
    //     << " restfreq=" << restfreq 
    //     << " frame=" << frame
    //     << " doppler=" << doppler
    //     << endl;

    nAvePol = -1;
    if (aveChan < 1)
       aveChan = 1;
    if (aveTime < 0)
       aveTime = 0;

    msRow = 0;

    //showColumnNames();
    //if (!hasColumn(upcase(col))) {
    //   cout << "The MS does not have '" + col + "' column" << endl;
    //   return;
    //}

    //if (aveMode == "SCALAR" &&
    //    !((isDataColumn(xcol) && xval == "AMP") || 
    //      (isDataColumn(ycol) && yval == "AMP"))) {
    // SLog::slog()->out("Scalar everage is for amplitude of visibility only",
    //                 fnname, clname, LogMessage::WARN); 
    //    return;
    //}

    if (aveTime == 0 && aveChan == 1) {
        //SLog::slog()->out("No averaging", 
                          //fnname, clname, LogMessage::NORMAL5); 
        aveOK = false;
        return;
    }

    try {
       VisIter& vi(vs->iter());
       VisBuffer vb(vi);

       //aveRow = 0;

       Int iChunk = 0;  //count the total number of chunks
       Int iIter = 0;   //count the total number of iterations
       Int iRow = 0;    //count the total number of input rows

       Int corrs = -1;
       Int chans = -1;
       Int corrChange = 0;
       Int chanChange = 0;
       //cout << "before checking the shape" << endl;
       vi.origin();
       for (vi.originChunks(); vi.moreChunks(); vi.nextChunk()) {
         for (vi.origin(); vi.more(); vi++) {
            //cout << "corrs=" <<  vi.nCorr()
            //   << " chans=" << vb.nChannel() << endl;
            if (corrs != vi.nCorr()) {
               corrs = vi.nCorr();
               corrChange++;
            }
            if (chans != vb.nChannel()) {
               chans = vb.nChannel();
               chanChange++; 
               //cout << "chanChange=" << chanChange << endl;
            }
         }
       }
       //cout << "after checking the shape" << endl;
       if (chanChange > 1 || corrChange > 1) {
	  LogIO os(LogOrigin("MsAverager", "setAverager"));
	  os << LogIO::WARN << "Average over variable shape of "
             << "channel/polarization is not supported" 
	     << LogIO::POST;
          aveOK = false;
          return;
       }

       Int nChan = vb.nChannel();
       aveChan = max(1, aveChan);
       aveChan = min(aveChan, nChan);

       nAveChan = 
           SAS::selectAverageChan(pMS, cList, spw, aveChan);
       //SAS::showSASC(spw);
       //cout << "nAveChan=" << nAveChan << endl;

       SAS::chanMap(aveChanMap, spw);
       //cout << "aveChanMap=" << aveChanMap ;
      
       aveRowMap.resize(pMS->nrow(), 3);
       //pMS->antenna().nrow() = vs->numberAnt()
       //due to the baseline/antenna selection, not all the 
       //antenna present in the pMS 
       //Int nAnt = (pMS->antenna().nrow());//vs->numberAnt();
       //cout << "nAnt=" << nAnt << endl;

       Int nAnt = baselines.shape()[0];
       nAntenna = nAnt;
       //cout << "nAnt=" << nAnt << endl;

       aveTime = max(0.01, aveTime);
       //cout << "aveTime=" << aveTime << endl;
       vi.setInterval(aveTime);

       nAveTime = 0;    //number of bins of time averaged buffer
       Int nTime = 0;   //number of integrations in the current cumulating bin 
       nAveRow = 0;

       Vector<Double> curTime;
       //Vector<Int> ant1;
       //Vector<Int> ant2;

       Double bufTime = -1;
       Bool newTime = true;

       Double sumTime = 0.;
       Double nextTime = -1;
       Int nTotalTime = 0;
      
       Int bufField = -1;
       Bool newField = true;

       Int bufScan = -1;
       Bool newScan = true;
 
       Int bufArray = -1;
       Bool newArray = true;

       Int bufDesc = -1;
       Bool newDesc = true;

       //timer for profile - cumulate time for patAveTable 
       //double usr = 0.;
       //double rea = 0.;
       //double sys = 0.;
       //Timer tmr2;
       //

       //timer for the total averaging time
       Timer tmr3;
       vi.origin();
       for (vi.originChunks(); vi.moreChunks(); vi.nextChunk()) {
#if LOG2
         cout << ">>>>>>>>>chunk=" << iChunk << endl;
#endif

         for (vi.origin(); vi.more(); vi++) {
            if (nAvePol == -1)
               nAvePol = vi.nCorr();

            if (nAvePol != vi.nCorr())
               cout << "nAvePol=" << nAvePol << endl;

            vi.time(curTime);
            //vi.antenna1(ant1);
            //vi.antenna2(ant2);

            Int nRow = curTime.nelements();
            //Int nRow = vi.nRow();
       	    //Int nRow = vb.nRow();

#if LOG2 
            cout << ">>>>>>iIter=" << iIter << " nRow=" << nRow << endl;
            cout << std::setprecision(12);
#endif

            Cube<Complex>& vc = vb.visCube();
            if (!upcase(column).compare("CORRECTEDDATA") ||
                !upcase(column).compare("CORRECTED_DATA")) {
               vc = vb.correctedVisCube();
            }
            if (!upcase(column).compare("MODELDATA") ||
                !upcase(column).compare("MODEL_DATA")) {
               vc = vb.modelVisCube(); 
            }
            if (!upcase(column).compare("RESIDUAL") ||
                !upcase(column).compare("CORRECTEDMODEL_DATA")) {
               vc = vb.correctedVisCube() - vb.modelVisCube();
            }
            Int i1, i2, i3;
            vc.shape(i1, i2, i3);
            //cout << "i1=" << i1 << " i2=" << i2 << " i3=" << i3 << endl;
            Cube<Complex> aveV(i1, nAveChan, 1); 
            Cube<Bool> aveF(i1, nAveChan, 1); 
            
            Int field = vb.fieldId();
            Int arry = vb.arrayId();

            Vector<rownr_t> rowNumber = vb.rowIds();
            //cout << "rowIds=" << rowNumber << endl;
            for (int row = 0; row < nRow; row++) {
               //skip flagged rows when selecting good data
               //this is done to make sure for good data average
               //each timebin start with good data
               while(row < nRow && vb.flagRow()(row) && aveFlag == 0) { 
                  row++;
               };
               if (row == nRow) 
                  break;

               //skip rows that are not in the selected spw
               Int dId = vi.dataDescriptionId();
               Int spwSelected = SAS::spwByDesc(dId, spw);
               while (row < nRow && spwSelected == -1) {
                     row++;
               } 
               //cout << "row=" << row 
               //     << " spwSelected=" << spwSelected << endl;
               if (row == nRow) 
                  break;

               //should not be, but careful
               if (spwSelected == -1)
                  spwSelected = 0;

               //cout << "row=" << row << " nRow=" << vb.nRow()
               //     << " dId=" << dId << endl;  

               //Double thisTime = curTime(row);
               Double thisTime = vb.time()(row);
               newTime = (thisTime - bufTime > aveTime - 0.005);
               //cout << "thisTime=" << std::setprecision(20) << thisTime 
               //     << " bufTime=" << bufTime << endl;


               newField = field - bufField;
               newArray = arry - bufArray;

               Int scan = vb.scan()(row);
               newScan = (scan - bufScan);

               newDesc = (dId - bufDesc); 

               //if (newDesc)
               //   cout << "newDesc=" << dId << endl;

               if (newField || (!crossScans && newScan) || 
                   newTime || (!crossArrays && newArray) || 
                              (!crossSpws && newDesc)) {

                     if (nTotalTime > 0)
                         sumTime /= nTotalTime;
                     //cout << "-nTotalTime=" << nTotalTime 
                     //     << " sumTime=" << sumTime << endl;

                     //tmr2.mark();
                     if (outputMode == MsAverager::TableMS)
                        putAveTable(bufTime, bufField, bufScan, bufArray, 
                                    bufDesc, *pAveBuff, nTime, sumTime);
                     else
                        putAveBuffer(bufTime, bufField, bufScan, bufArray,
                                     *pAveBuff, nTime); 
                     //usr += tmr2.user();
                     //rea += tmr2.real();
                     //sys += tmr2.system();
   
                     if (pAveBuff) {
                        delete pAveBuff;
                        pAveBuff = 0;
                     }
   
                     bufTime = thisTime;
                     bufField = field;
                     bufScan = scan;
                     bufArray = arry;
                     bufDesc = dId;
                     
                     pAveBuff = new VisBuffer;
                     initAveBuffer(bufTime, *pAveBuff, nAnt, nAveChan);
                     nAveTime++;
                     nTime = 0;
                     sumTime = 0;
                     nTotalTime = 0;
               }

               if (nextTime != thisTime) {
                  sumTime += (thisTime - bufTime);
                  nTotalTime++;
               }

               //cout << "row=" << row << " thisTime=" 
               //     << std::setprecision(12) << thisTime
               //     << " bufTime=" << bufTime 
               //     << " sumTime=" << sumTime 
               //     << " nTime=" << nTime << endl;
               
               Int ant1 = vb.antenna1()(row);
               Int ant2 = vb.antenna2()(row);

               Int orow = baselineRow(ant1, ant2);
               if (orow < 0)
                  continue;

               //Int orow = baselineRow(nAnt, ant1, ant2);
               //cout << "row=" << row << " orow=" << orow 
               //     << " nAnt=" << nAnt   
               //     << " ant1=" << ant1 << " ant2=" << ant2 << endl;
               //aveBuff.antenna1()(orow) = ant1;
               //aveBuff.antenna2()(orow) = ant2;

               pAveBuff->time()(orow) = bufTime;
               pAveBuff->feed1()(orow) = vb.feed1()(row);
               //pAveBuff->feed2()(orow) = vb.feed2()(row);

               Complex xxx = 0.;
               
               //cout << "flagrow=" << vb.flagRow()(row) << endl; 
               //showVisRow(vc, row);
               //cout << vb.flagCube();

               Int sw = SAS::spwIndexByDesc(dId, spw);
               if (sw < 0) sw = 0;
               pAveBuff->sigma()(orow) = spw(sw).rFreq;

               if (aveFlag) {
                  //select flagged

                  //init every cell unflagged
                  for (Int pol = 0; pol < nAvePol; pol++) {
                     for (Int chn = 0; chn < nAveChan; chn++) {
                        aveV(pol, chn, 0) = 0.;
                        aveF(pol, chn, 0) = 0;               
                     }
                  }

                  if (vb.flagRow()(row)) {
                     //row flagged = everyone is flagged, should select
                     //cout << "flagged row" << endl;
                   
                     for (Int pol = 0; pol < nAvePol; pol++) {

                        xxx = 0.;
                        Int p = 0;
                        Int chn = 0;
                           
                        Int sChan = 0; 
                        for (chn = 0; chn < Int(spw(sw).chans.size()); chn++) {

                           Int chNum = spw(sw).chans(chn);
                           p++;
                           if (aveMode == "SCALAR") {
                              xxx += Complex(fabs(vc(pol, chNum, row)),0.0);
                           }
                           else {
                              xxx += vc(pol, chNum, row);
                           }
                           if (p == aveChan) {
                              aveV(pol, sChan, 0) = xxx / p;
                              aveF(pol, sChan, 0) = 1;
                              xxx = 0;
                              p = 0;
                              sChan++;   
                           }
                        }
                        if (p > 0) {
                           aveV(pol, sChan, 0) = xxx / p;
                           aveF(pol, sChan, 0) = 1;
                        }
                        sChan++;
                     }
                  }
                  else {
                     //only select flagged ; 
                     //cout << " not row flagged" << endl;
                     for (Int pol = 0; pol < nAvePol; pol++) {

                        xxx = 0.;
                        Int p = 0;
                        Int sx = 0; 
                        Int chn = 0;

                        Int sChan = 0;
                        for (chn = 0; chn < Int(spw(sw).chans.size()); chn++) {
   
                           Int chNum = spw(sw).chans(chn);
                           p++;
                           if (vb.flagCube()(pol, chNum, row)) {
                              if (aveMode == "SCALAR") {
                                 xxx += Complex(fabs(vc(pol, chNum, row)),0.0);
                              }
                              else {
                                 xxx += vc(pol, chNum, row);
                              }
                              sx++;
                           }
                           if (p == aveChan) {
                              if (sx > 0) {
                                 aveV(pol, sChan, 0) = xxx / sx;
                                 aveF(pol, sChan, 0) = 1;
                              }
                              xxx = 0;
                              p = 0;
                              sx = 0;
                              sChan++;   
                           }
                        }
                        if (p > 0) {
                           if (sx > 0) {
                              aveV(pol, sChan, 0) = xxx / sx;
                              aveF(pol, sChan, 0) = 1;
                           }
                           sChan++;
                        }
                     }
                  }
                  //showVisRow(aveV, row);
                  //cout << aveF.xyPlane(0) << endl;
               }
               else {
                  //select non-flagged=good data
                  // cout << "row=" << rowNumber(row) 
                  //      << " flagrow=" << vb.flagRow()(row)
                  //      << " " << vb.flagCube();

                  //init every cell flagged
                  for (Int pol = 0; pol < nAvePol; pol++) {
                     for (Int chn = 0; chn < nAveChan; chn++) {
                        aveV(pol, chn, 0) = 0.;
                        aveF(pol, chn, 0) = 1;               
                     }
                  }

                  if (!vb.flagRow()(row)) {
                     //only select non-flagged
                     for (Int pol = 0; pol < nAvePol; pol++) {

                        xxx = 0.;
                        Int p = 0;
                        Int sx = 0; 

                        Int sChan = 0;
                        Int chn;
                        for (chn = 0; chn < Int(spw(sw).chans.size()); chn++){
   
                           Int chNum = spw(sw).chans(chn);
                           p++;
                           if (!(vb.flagCube()(pol, chNum, row))) {
                              if (aveMode == "SCALAR") {
                                 xxx += Complex(fabs(vc(pol, chNum, row)),0.0);
                              }
                              else {
                                 xxx += vc(pol, chNum, row);
                              }
                              sx++;
                           }
                           if (p == aveChan) {
                              if (sx > 0) {
                                 aveV(pol, sChan, 0) = xxx / sx;
                                 aveF(pol, sChan, 0) = 0;
                              }
                              xxx = 0;
                              p = 0;
                              sx = 0;
                              sChan++;   
                           }
                        }
                        if (p > 0) {
                           if (sx > 0) {
                              aveV(pol, sChan, 0) = xxx / sx;
                              aveF(pol, sChan, 0) = 0;
                           }
                           sChan++;
                        }
                     }
                  }
               }

               //cout << "orow=" << orow << endl;
               //weight is on polarization, should be a vector, but
               //defined in visBuffer as a scalar, good enugh for averaging
               Float wt = vb.weight()(row);
       
               pAveBuff->timeInterval()(orow) += 
                                            vb.timeInterval()(row) * wt;
               RigidVector<Double, 3> wtuvw = vb.uvw()(row) * Double(wt);
               pAveBuff->uvw()(orow) += wtuvw;
               pAveBuff->weight()(orow) += wt;

               for (Int pol = 0; pol < nAvePol; pol++) {
                  for (Int chn = 0; chn < nAveChan; chn++) {
                     //cout << "pol=" << pol << " chn=" << chn 
                     //     << " wt=" << wt
                     //     << " cellFlag=" << aveF(pol, chn, 0) 
                     //     << " aveFlag=" << aveFlag
                     //     << " Buff=" 
                     //     << pAveBuff->visCube()(pol, chn, orow);
                     if (aveF(pol, chn, 0) == aveFlag) {
                        pAveBuff->flagCube()(pol, chn, orow) = aveFlag;
                        // DComplex wtvis = 
                        //                   aveV(pol, chn, 0) * Double(wt);
                        //cout << " aveV=" << wt * aveV(pol, chn, 0) ;
                        pAveBuff->visCube()(pol, chn, orow) += 
                                          wt * aveV(pol, chn, 0);
                        pAveBuff->weightCube()(pol, chn, orow) += wt;
                     }
		     //cout << " ==>" 
		     //	     << pAveBuff->visCube()(pol, chn, orow);
                
                  }
               }
               //showVisRow(pAveBuff->visCube(), orow);
               //showVisRow(aveV, 0);
               //cout << pAveBuff->flagCube().xyPlane(orow); 

               if (crossBlines) 
                  aveRowMap(iRow, 0) = msRow;
               else
                  aveRowMap(iRow, 0) = msRow + orow;
               aveRowMap(iRow, 1) = rowNumber(row);
               aveRowMap(iRow, 2) = spwSelected;
               
               //cout << "row=" << row << " "
               //     << aveRowMap(iRow, 0) << " " 
               //     << aveRowMap(iRow, 1) << " " 
               //     << aveRowMap(iRow, 2) << endl;

               iRow++;
               //cout << "iRow=" << iRow << endl;
            }
            nTime++;
            //showVisRow(vc, 0);
            iIter++;  
            //cout << "nTime=" << nTime << " iIter=" << iIter << endl; 
         }
         iChunk++;  
      }
      tmr3.show("  Averaging:");

      if (nTotalTime > 0)
         sumTime /= nTotalTime;
      //cout << "nTotalTime=" << nTotalTime 
      //    << " sumTime=" << sumTime << endl;

      //tmr2.mark();
      if (outputMode == MsAverager::TableMS)
         putAveTable(bufTime, bufField, bufScan, bufArray, bufDesc, 
                     *pAveBuff, nTime, sumTime);
      else
         putAveBuffer(bufTime, bufField, bufScan, bufArray,
                     *pAveBuff, nTime);
      //usr += tmr2.user();
      //rea += tmr2.real();
      //sys += tmr2.system();

      if (pAveBuff) {
         delete pAveBuff;
         pAveBuff = 0;
      }
      //showAveMap(aveRowMap, aveChanMap); 

      {
         ostringstream os;
         os << "TOTAL " 
            << "Chunks=" << iChunk << " Iters=" << iIter 
            << " Rows=" << iRow << " nAveTime=" << nAveTime 
            << " nAveChan=" << nAveChan << " nAveRow=" << nAveRow 
            << " nAvePol=" << nAvePol
            << endl;
         //SLog::slog()->out(os, fnname, clname, LogMessage::NORMAL5);
      }

      //cout << "iRow=" << iRow << endl;
      //cout << "pMS->nrow()=" << pMS->nrow() << endl;
      aveRowMap.resize(iRow, 3, true);
      //showAveMap(aveRowMap, aveChanMap); 

      {
         ostringstream os;
         os << aveRowMap;
         //SLog::slog()->out(os, fnname, clname, LogMessage::DEBUG1);
      }

      //cout << "putAveTable:" << setw(11) << rea << " real "
      //   << setw(11) << usr << " user "
      //   << setw(11) << sys << " system" << endl;

   } 
   catch (const AipsError &x) {
      //SLog::slog()->out(String("Error: ") + x.getMesg(),
             //fnname, clname, LogMessage::WARN);
      LogIO os(LogOrigin("MsAverager", "setAverager"));
      os << LogIO::WARN << "Error: "
		   << x.getMesg()
		   << LogIO::POST;
      aveOK = false;
      return ;
   }
   //catch (...) {cout << "problem------" << endl;}
   aveOK = true;
   return;
}

void MsAverager::putAveTable(Double bufTime, Int bufField, Int bufScan,
                             Int bufArray, Int bufDesc, VisBuffer& aveBuff, 
                             Int nTime, Double timeShift) {
   if (nTime < 1)
      return;

   if (bufTime == -1 && bufField == -1 && bufScan == -1 && bufArray == -1)
      return;


#if LOG2
   cout << " putAveTable: bufTime=" << bufTime
        << " bufField=" << bufField
        << " bufScan=" << bufScan
        << " bufArray=" << bufArray
        << " bufDesc=" << bufDesc
        << " nRow=" << aveBuff.nRow()
        << " nTime=" << nTime
        << " nChannel=" << aveBuff.nChannel() 
        << endl;
#endif
   if (nAveRow < 1)
      nAveRow = aveBuff.nRow();
   //cout << "nAveRow=" << nAveRow << endl;
   //ScalarColumn<Double> timeCol(aMS, "TIME");
   
   //sigma stores refFreq, verify it containing only one nozero value
   Float sigVal = 0;
   Int sigValChange = 0;
   for (Int row = 0; row < nAveRow; row++) {
      Float sig = aveBuff.sigma()(row);  
      if (sig && sig != sigVal) {
         sigVal = sig;
         sigValChange++;
      }
      //cout << "sigma=" << aveBuff.sigma()(row) 
      //     << " row=" << row << endl;
   }
   //cout << "sigValChange=" << sigValChange << endl;
   //
   Vector<Float> refVal(1);
   refVal= sigVal;


   Vector<Complex> frqvel(nAveChan);
   Vector<Complex> sxsave(nAveChan);
   sxsave = 0;
   frqvel = 0;

   Int idx = SAS::spwIndexByDesc(bufDesc, spw);
   if (idx < 0) return;

   //calculate averaged velocities, store into velo 
   Vector<Double> velo(nAveChan);
   
   if (aveVel)
   SAS::averageVelocity(sorryVel, pMS, spw, velo, idx, bufField,
                        restfreq, frame, doppler);
   //cout << "velo=" << velo << " freq=" << spw(idx).aveFreqs << endl;

   for (uInt j = 0; j < spw(idx).aveFreqs.size(); j++) {
      frqvel(j) = Complex(velo(j), spw(idx).aveFreqs(j));
      sxsave(j) = Complex(spw(idx).aveChans(j), spw(idx).sxsChans(j));
   }


   if (crossBlines) { 
      //cout << " crossBlines=true" << endl; 
      aMS.addRow();

      Int tRow = msRow;
      msc->time().put(tRow, bufTime);
      msc->antenna1().put(tRow, aveBuff.antenna1()(0)); 
      msc->antenna2().put(tRow, aveBuff.antenna2()(0)); 

      msc->fieldId().put(tRow, bufField); 
      msc->scanNumber().put(tRow, bufScan); 
      msc->arrayId().put(tRow, bufArray);
      msc->feed1().put(tRow, aveBuff.feed1()(0)); 
      msc->dataDescId().put(tRow, bufDesc);
      //msc->feed2().put(tRow, aveBuff.feed2()(0)); 
      msc->sigma().put(tRow, refVal); 


      Vector<Float> w(nAvePol < 1 ? 1 : nAvePol);
      for (Int row = 0; row < nAveRow; row++) {
         for (Int pol = 0; pol < nAvePol; pol++) {
            for (Int chn = 0; chn < nAveChan; chn++) {
               Float wc  = aveBuff.weightCube()(pol, chn, row);
               if (wc > 0.) {
                  aveBuff.visCube()(pol, chn, row) /= wc;
               } 
               else {
                  aveBuff.visCube()(pol, chn, row) = 0.;
               }
            }
         }

         Float wt = aveBuff.weight()(row);
         if (wt == 0.0f) { 
            w = wt;
            wt = 1;
         }
         else {
            w = wt / nTime;
         }
         aveBuff.weight()(row) = wt;
         aveBuff.uvw()(row) = aveBuff.uvw()(row) * Double(1. / wt);
         aveBuff.timeInterval()(row) /= wt;
      }

      Cube<Complex> blV(nAvePol, nAveChan, 1);
      Cube<Complex> blU(nAvePol, nAveChan, 1);
      Cube<Bool> blF(nAvePol, nAveChan, 1);
      for (Int pol = 0; pol < nAvePol; pol++) {
         for (Int chn = 0; chn < nAveChan; chn++) {
            blV(pol, chn, 0) = 0.;
            blF(pol, chn, 0) = !aveFlag;
         }
      }

      for (Int pol = 0; pol < nAvePol; pol++) {
         for (Int chn = 0; chn < nAveChan; chn++) {
            Int blW = 0;
            for (Int row = 0; row < nAveRow; row++) {
               if (aveBuff.flagCube()(pol, chn, row) == aveFlag) {
                  blV(pol, chn, 0) += 
                     aveBuff.visCube()(pol, chn, row);
                  blW++;
               } 
            }
            if (blW > 0) {
               blV(pol, chn, 0) /= blW;
               blF(pol, chn, 0) = aveFlag;
            }
         }
      }
   
      Int nFlags = 0;
      for (Int pol = 0; pol < nAvePol; pol++) {
         for (Int chn = 0; chn < nAveChan; chn++) {
            if (!blF(pol, chn, 0)) {
               nFlags++;
            }
         }
      }
      if (nFlags)
         msc->flagRow().put(tRow, 0);
      else 
         msc->flagRow().put(tRow, 1);
         
      msc->data().put(tRow, blV.xyPlane(0));
      msc->flag().put(tRow, blF.xyPlane(0));  
      RigidVector<Double, 3> bluvw = aveBuff.uvw()(0) * Double(1.);
      msc->uvw().put(tRow, bluvw.vector());
   
      //showVisRow(blV, 0);
      //showMsRow(msc, tRow);
   
      msc->weight().put(tRow, w);

      for (Int pol = 0; pol < nAvePol; pol++) {
         for (Int chn = 0; chn < nAveChan; chn++) {
            blV(pol, chn, 0) = frqvel(chn);
            blU(pol, chn, 0) = sxsave(chn);
         }
      }
      //msc->modelData().put(tRow, blV.xyPlane(0));
      //msc->correctedData().put(tRow, blU.xyPlane(0));
   
      if (timeShift > 0.) {
         //cout << " bufTime=" << std::setprecision(12) 
         //     << bufTime << " timeShift=" << timeShift << endl;
         msc->time().put(tRow, bufTime + timeShift);
      }
      else {
         msc->time().put(tRow, 
            bufTime + 0.5 * aveBuff.timeInterval()(0) * (nTime - 1));
      }
   }
   else {
   
      //cout << " crossBlines=false" << endl; 
   
      for (Int row = 0; row < nAveRow; row++) {
         for (Int pol = 0; pol < nAvePol; pol++) {
            for (Int chn = 0; chn < nAveChan; chn++) {
               aveBuff.modelVisCube()(pol, chn, row) = frqvel(chn);
               aveBuff.correctedVisCube()(pol, chn, row) = sxsave(chn);
            }
         }
      }
   
      for (Int row = 0; row < nAveRow; row++) {
         
         Int nFlags = 0;
         //Int tFlags = nAvePol * nAveChan;
         for (Int pol = 0; pol < nAvePol; pol++) {
            for (Int chn = 0; chn < nAveChan; chn++) {
               Float wc  = aveBuff.weightCube()(pol, chn, row);
               //flags should be correct, no need to fiddle
               if (wc > 0.) {
                  aveBuff.visCube()(pol, chn, row) /= wc;
               } 
               else {
                  aveBuff.visCube()(pol, chn, row) = 0.;
               }
               if (!aveBuff.flagCube()(pol, chn, row))
                  nFlags++;
               //if (wc > 0)
               //   cout << " wc=" << wc << " row=" << row << " >>>" 
               //   << aveBuff.visCube()(pol, chn, row);
            }
         }

         Int tRow = msRow + row;
         aMS.addRow();
         
         msc->time().put(tRow, bufTime);
         msc->antenna1().put(tRow, aveBuff.antenna1()(row)); 
         msc->antenna2().put(tRow, aveBuff.antenna2()(row)); 
         //cout << "sigma=" << aveBuff.sigma()(row) << " row=" << row << endl;
         msc->sigma().put(tRow, refVal); 
     
   
         if (nFlags)
            msc->flagRow().put(tRow, 0);
         else 
            msc->flagRow().put(tRow, 1);
         
         
         Vector<Float> w(nAvePol < 1 ? 1 : nAvePol);
         Float wt = aveBuff.weight()(row);
         if (wt == 0.0f) { 
            w = wt;
            wt = 1;
         }
         else {
            w = wt / nTime;
         }
         
         msc->flag().put(tRow, aveBuff.flagCube().xyPlane(row));  
         msc->weight().put(tRow, w);
         msc->uvw().put(tRow, (aveBuff.uvw()(row) * Double(1. / wt)).vector());

         //msc->data().put(tRow, aveBuff.visCube().xyPlane(row) / wt);
         msc->data().put(tRow, aveBuff.visCube().xyPlane(row));
         msc->fieldId().put(tRow, bufField); 
         msc->scanNumber().put(tRow, bufScan); 
         msc->arrayId().put(tRow, bufArray);
         msc->feed1().put(tRow, aveBuff.feed1()(row)); 
         msc->dataDescId().put(tRow, bufDesc);
         //msc->feed2().put(tRow, aveBuff.feed2()(row)); 
         //msc->modelData().put(tRow, aveBuff.visCube().xyPlane(row));
   
         if (timeShift > 0.) {
            //cout << " bufTime=" << std::setprecision(12) 
            //     << bufTime << " timeShift=" << timeShift << endl;
            msc->time().put(tRow, bufTime + timeShift);
         }
         else {
            Float itvl = aveBuff.timeInterval()(row) / wt;
            msc->interval().put(tRow, itvl);
            //cout << "interval=" << itvl << endl;
            msc->time().put(tRow, bufTime + 0.5 * itvl * (nTime - 1));
         }
   
         //showVisRow(aveBuff.visCube(), row);
         //showMsRow(msc, tRow);
   
         // the flag and vis for the resulting row  
         //   cout << "row=" << row 
         //        << " flagCube=" << aveBuff.flagCube().xyPlane(row) 
         //        << " visCube=" << aveBuff.visCube().xyPlane(row)
         //        << endl; 
         //msc->modelData().put(tRow, aveBuff.modelVisCube().xyPlane(row));
         //msc->correctedData().put(tRow, aveBuff.correctedVisCube().xyPlane(row));
      }
      
   }

   //MSSpWindowColumns spwColumn(aMS.spectralWindow());
   //ScalarColumn<Int> numCol = spwColumn.numChan();
   //spwColumn.numChan().put(idx, nAveChan); 

   msRow = aMS.nrow();
   //cout << "msRow=" << msRow << endl;
   
   return;
}

void MsAverager::putAveBuffer(Double IfLog2 (bufTime), Int IfLog2 (bufField), Int IfLog2 (bufScan),
                              Int IfLog2 (bufArray), VisBuffer& aveBuff, Int nTime) {
#if LOG2
   cout << " putAveBuffer: bufTime=" << bufTime
        << " bufField=" << bufField
        << " bufScan=" << bufScan
        << " bufArray=" << bufArray
        << " nRow=" << aveBuff.nRow()
        << " nTime=" << nTime
        << " nChannel=" << aveBuff.nChannel() 
        << endl;
#endif

   if (nTime < 1)
      return;

   if (nAveRow < 1)
      nAveRow = aveBuff.nRow();

   for (Int row = 0; row < nAveRow; row++) {

      //aveBuff.time()(row) = bufTime;

      Float wt = aveBuff.weight()(row);
      if (wt == 0.0f) { 
         wt = 1.;
      }

      aveBuff.timeInterval()(row) /= wt;
      aveBuff.uvw()(row) *= 1.0f / wt;
      for (Int pol = 0; pol < nAvePol; pol++) {
         for (Int chn = 0; chn < nAveChan; chn++) {
            aveBuff.visCube()(pol, chn, row) *= 1.0f / wt;
         }
      }
   }


   aveList.push_back(pAveBuff); 

   return;
}

Int MsAverager::baselineRow(const Int& nAnt, const Int& a1, const Int& a2)
{
  Int index;
  index = nAnt * a1 - (a1 * (a1 - 1)) / 2 + a2 - a1;
  if (a1 > a2)
     index = nAnt * a2 - (a2 * (a2 - 1)) / 2 + a1 - a2;
  return index;
}

Int MsAverager::baselineRow(const Int& a1, const Int& a2)
{
  //cout << "bls=" << bls << " bls.shape=" << bls.shape()
  //     << " a1=" << a1 << " a2=" << a2 << endl;
  if (a1 < 0 && a2 < 0)
     return baselines.shape()[0];
  Int index = -1;
  for (Int i = 0; i < baselines.shape()[0]; i++) { 
     if (a1 == baselines(i, 0) && a2 == baselines(i, 1))
       index = i;
  }
  return index;
}

void MsAverager::showMsRow(MSMainColumns* msc, Int row) {
   cout << "row=" << row 
        << "\ntime=" << std::setprecision(12) << msc->time()(row) 
        << " rowflag=" << msc->flagRow()(row) 
        << std::setprecision(5)
        << "\nant1=" << msc->antenna1()(row)
        << " ant2=" << msc->antenna2()(row)
        << "\ndata=" << msc->data()(row) 
        << "\nflag=" << msc->flag()(row) 
        << "\nsigma=" << msc->sigma()(row) 
        << "\ndesc=" << msc->dataDescId()(row) 
        << endl; 
}

void MsAverager::showVisRow(Cube<Complex>& vc, Int row) {
   IPosition ipos = vc.shape();
   Int nPol = ipos(0);
   Int nChan = ipos(1); 
   cout << std::setprecision(8) 
        << " vis (" << nPol << " pol x " << nChan << " chan):\n";
   for (Int chn = 0; chn < nChan; chn++) {
      for (Int pol = 0; pol < nPol; pol++) {
         cout << " " << vc(pol, chn, row);
      }
      cout << endl;
   }
}

void MsAverager::showColumnNames() {
   cout << pMS->tableDesc().columnNames() << endl; 
   //UVW, FLAG, FLAG_CATEGORY, WEIGHT, SIGMA, ANTENNA1, ANTENNA2, 
   //ARRAY_ID, DATA_DESC_ID, EXPOSURE, FEED1, FEED2, FIELD_ID, 
   //FLAG_ROW, INTERVAL, OBSERVATION_ID, PROCESSOR_ID, SCAN_NUMBER, 
   //STATE_ID, TIME, TIME_CENTROID, DATA, WEIGHT_SPECTRUM, 
   //MODEL_DATA, CORRECTED_DATA
}

void MsAverager::showAveMap(Matrix<Int> &/*rMap*/, Matrix<Int> &/*cMap*/) {
   //cout << "aveRowMap=" << std::setprecision(8) << rMap;
   //cout << "aveChanMap=" << std::setprecision(12) << cMap;
}

Bool MsAverager::hasColumn(casacore::String const& col) {
    Vector<String> cols = pMS->tableDesc().columnNames();
    for (uInt i = 0; i < cols.nelements(); i++) {
       if (cols(i) == col)
          return true;
    } 
    LogIO os(LogOrigin("MsAverager", "hasColumn"));
    os << LogIO::WARN << String("No column '") + col + "' in the MS"
       << LogIO::POST;

    return false;
}

Bool MsAverager::isDataColumn(casacore::String const& col) {
    return col == "DATA" 
        || col == "CORRECTEDDATA"
        || col == "MODELDATA"
        ;
}

void MsAverager::getMap(Matrix<Int>& rowMap, Matrix<Int>& chanMap) {
    rowMap.resize(0, 0);
    rowMap = aveRowMap;

    chanMap.resize(0, 0);
    chanMap = aveChanMap;
} 

void MsAverager::getMS(MS& ms) {
    //cout << "getMS  aMS=" << aMS << endl;
    if (outputMode != MsAverager::TableMS) {
       //SLog::slog()->out(String("MS is not available in 'ListBuffer' mode"),
             //"getMS", clname, LogMessage::WARN);
       LogIO os(LogOrigin("MsAverager", "getMS"));
       os << LogIO::WARN << String("MS is not available in 'ListBuffer' mode")
		   << LogIO::POST;
       return;
    }
    //ms = aMS; 
    ms = MS(aMS);
} 

void MsAverager::getXY(Vector<Double>& x, casacore::Vector<Double>& y,
                       Vector<Int>& f, Int pol) {

  if (outputMode != MsAverager::ListBuffer) {
     //SLog::slog()->out(String("MS is not available in 'TableMS' mode"),
             //"getXY", clname, LogMessage::WARN);
     LogIO os(LogOrigin("MsAverager", "getXY"));
     os << LogIO::WARN << String("MS is not available in 'TableMS' mode")
		   << LogIO::POST;
     return;
  }

  VisBuffer* pAveBuff = aveList.front( );
  IPosition ip = pAveBuff->visCube().shape();

  Int nBuff = aveList.size( ); // nAveTime
  Int nRow = ip(2);
#if LOG2
  Int nChan = ip(1); // nAveChan
  Int nPol = ip(0); // nAvePol
 
  cout << "getXY nBuff=" << nBuff << " nPol=" << nPol 
       << " nChan=" << nChan << " nRow=" << nRow
       << " len=" << nChan * nRow << endl;
#endif
  Int len = nBuff * nRow * nAveChan;

  x.resize(len);
  y.resize(len);
  f.resize(len);

  int i = 0;
  for ( auto pb : aveList ) {
     if (pb != 0) {
        for (int row = 0; row < nRow; row++) {
           for (Int chn = 0; chn < nAveChan; chn++) {
              Complex xxx = pb->visCube()(pol, chn, row);  
              y(i) = xxx.real();
              x(i) = pb->time()(row);  
              f(i) = pb->flagCube()(pol, chn, row);
              i++;
           }
           //showVisRow(aveV, 0);
        }
     }
  }

}

void MsAverager::initAveBuffer(Double bufTime, 
                               VisBuffer& aveBuff, Int /*nAnt*/, Int nChan)
{

  
  Int nRowAdd = baselineRow ();
  //Int nRowAdd = baselineRow (nAnt, nAnt - 1, nAnt - 1) + 1;
  aveBuff.nRow();
  aveBuff.nRow() = nRowAdd;
  aveBuff.nChannel() = nChan;
  //cout << "nRowAdd=" << nRowAdd << endl;
#if LOG2
  cout << "initAveBuffer: bufTime=" << std::setprecision(12) << bufTime 
       << " nRow=" << aveBuff.nRow() 
       << " nChan=" << aveBuff.nChannel() 
       << endl;
#endif

  Int nRow = aveBuff.nRow();
  aveBuff.antenna1().resize(nRow);
  aveBuff.antenna2().resize(nRow);
  aveBuff.time().resize(nRow);
  aveBuff.timeInterval().resize(nRow);
  aveBuff.uvw().resize(nRow);
  //aveBuff.visibility().resize(nChan, nRow);
  //aveBuff.flag().resize(nChan, nRow);
  aveBuff.visCube().resize(nAvePol, nChan, nRow); 
  aveBuff.flagCube().resize(nAvePol, nChan, nRow);
  aveBuff.weightCube().resize(nAvePol, nChan, nRow);
  aveBuff.modelVisCube().resize(nAvePol, nChan, nRow);
  aveBuff.correctedVisCube().resize(nAvePol, nChan, nRow);
  aveBuff.weight().resize(nRow);
  aveBuff.flagRow().resize(nRow);
  aveBuff.feed1().resize(nRow);
  //aveBuff.feed2().resize(nRow);
  aveBuff.sigma().resize(nRow);

  Int row = 0;
  //for (Int ant1 = 0; ant1 < nAnt; ant1++) {
  //  for (Int ant2 = ant1; ant2 < nAnt; ant2++) {
  //    aveBuff.antenna1()(row) = ant1;
  //    aveBuff.antenna2()(row) = ant2;
  //    row++;
  //  }
  //}

  for (row = 0; row < nRow; row++) {
    aveBuff.antenna1()(row) = baselines(row, 0);
    aveBuff.antenna2()(row) = baselines(row, 1);
    aveBuff.time()(row) = bufTime;
    aveBuff.timeInterval()(row) = 0.0;
    aveBuff.uvw()(row) = 0.0;
    aveBuff.feed1()(row) = 0;
    aveBuff.sigma()(row) = 0;
    //aveBuff.feed2()(row) = 0;
    //for (Int chn = 0; chn < nChan; chn++) {
    //  aveBuff.visibility()(chn, row) = CStokesVector();
    //  aveBuff.flag()(chn, row) = true;
    //};
    for (Int pol = 0; pol < nAvePol; pol++) {
       for (Int chn = 0; chn < nChan; chn++) {
         aveBuff.flagCube()(pol, chn, row) = !aveFlag;
         aveBuff.visCube()(pol, chn, row) = 0;
         aveBuff.weightCube()(pol, chn, row) = 0.;
         aveBuff.modelVisCube()(pol, chn, row) = 0.;
         aveBuff.correctedVisCube()(pol, chn, row) = 0.;
       }
    };
    aveBuff.weight()(row) = 0.0f;
    aveBuff.flagRow()(row) = !aveFlag;
  };
  //cout << "initAve: nRow=" << aveBuff.nRow() 
  //     << " nChannel=" << aveBuff->nChannel() << endl;
  


}


}

