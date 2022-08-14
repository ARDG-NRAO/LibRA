//# SelectAverageSpw.cc: Implementation of SelectAverageSpw.h
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
#include <casacore/casa/Exceptions/Error.h>
#include <casacore/casa/Arrays/ArrayMath.h>
#include <casacore/casa/Arrays/Slice.h>
#include <casacore/measures/Measures/MeasTable.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <iostream>
#include <iomanip>
#include <cmath>

#define LOG2 0

using namespace casacore;
namespace casa { 

const Int SelectAverageSpw::maxChan = 100000; 

Int SelectAverageSpw::nextSelected(Int sp, Int currId, Matrix<Int>& chanList) {
   //cout << "spw=" << sp << " currId=" << currId << endl;
   Int pick = SelectAverageSpw::maxChan;
   Int nWin = chanList.shape()(0);
   for (Int i = 0; i < nWin; i++) {
      if (chanList(i, 0) == sp) {
         Int tic = -1;
         for (Int j = chanList(i, 1); j <= chanList(i, 2);
              j += chanList(i, 3)) {
            if (j > currId) {
               tic = j;
               break;
            }
         }
         //cout << "tic=" << tic << " pick=" << pick << endl;
         if (tic > -1 && tic < pick)
            pick = tic;
      }
   }
   if (pick == maxChan)
      return -1;
   else
      return pick;
}

Int SelectAverageSpw::selectAverageChan(MS* pMS, const Matrix<Int>& cList, 
                               Vector<SAS>& spw, const Int& aveChan) {

   uInt nAveChan = 0;

   Matrix<Int> chanList;
   chanList.resize(0, 0);
   chanList = cList;

   Int nWin = chanList.nrow();
   //cout << "number of selected spw:chan =" << nWin << endl;

   MSSpWindowColumns spwColumn(pMS->spectralWindow());
   MSDataDescColumns descColumn(pMS->dataDescription());

   //when spw='', the chanList is empty, nWin=0, 
   //so make up chanList here
   if (nWin < 1) {
       Vector<Int> spwNumChan = spwColumn.numChan().getColumn();
       Int spwNum = spwNumChan.shape()(0);
       chanList.resize(spwNum, 4);
       for (Int w = 0; w < spwNum; w++) {
          chanList(w, 0) = w;
          chanList(w, 1) = 0;
          chanList(w, 2) = spwNumChan(w) - 1;
          chanList(w, 3) = 1;
       }
       nWin = spwNum;
   }
   //cout << "number of actual spw:chan =" << nWin << endl;

   //MSSelection produced chanList behaves as following:
   //the stride for spw='0:50' is 0
   //the stride for spw='0:50~50' is 1
   //so force the stride=chanList(i, 3)=1
   for (Int i = 0; i < nWin; i++) {
      if (chanList(i, 2) < chanList(i, 1)) {
         chanList(i, 2) = chanList(i, 1);
         chanList(i, 3) = 1;
      }
      if (chanList(i, 3) < 1)
         chanList(i, 3) = 1;
   }
   //cout << "validated chanList=" << chanList;

   //from spw:chan list extract non-repeat spw ids
   Vector<Int> spwids(nWin);
   spwids(0) = chanList(0, 0);
   Int sCnt = 1;
   for (Int w = 0; w < nWin; w++) {
      Bool listed = false;
      for (Int i = 0; i < sCnt; i++) {
         if (spwids(i) == chanList(w, 0)) {
            listed = true; 
            break;
         } 
      }
      if (!listed) {
         spwids(sCnt++) = chanList(w, 0);
      }
   }
   spwids.resize(sCnt, true);
   //cout << "actual spws=" << spwids << endl;

   Vector<Int> spwDesc = descColumn.spectralWindowId().getColumn();

   //fill attributes for each of the selected spws
   spw.resize(sCnt);
   for (Int s = 0; s < sCnt; s++) { 
      
      spw(s).spwid = spwids(s);

      for (uInt t = 0; t < spwDesc.nelements(); t++) {
         if (spwDesc(t) == spw(s).spwid) {
            spw(s).desc = t;
            break;
         }
      }

      spw(s).measFreqRef = spwColumn.measFreqRef()(spw(s).spwid);
      Double rfreq = spwColumn.refFrequency()(spw(s).spwid);
      spw(s).rFreq = rfreq / 1000.; 

      Vector<Int> chids(SelectAverageSpw::maxChan);
      Int cCnt = 0;
      Int ch = -1;
      while(
        (ch = SelectAverageSpw::nextSelected(spw(s).spwid, ch, chanList)) > -1 
            && cCnt < SelectAverageSpw::maxChan){
         chids(cCnt++) = ch;
      }
      chids.resize(cCnt, true);
      //cout << "chids=" << chids << endl;
      spw(s).chans.resize(cCnt);
      spw(s).chans.assign(chids);

   }

   //fill in the averaged attributes
   //cout << "aveChan=" << aveChan << endl;
   Int ssChan = 0;
   for (Int s = 0; s < sCnt; s++) { 
      
      Int cCnt = Int(ceil(Float(spw(s).chans.size()) / aveChan));
      //cout << "cCnt=" << cCnt << endl;

      spw(s).sxsChans.resize(cCnt);
      spw(s).aveChans.resize(cCnt);
      spw(s).aveFreqs.resize(cCnt);
      spw(s).aveChanNames.resize(cCnt);

      Int aCnt = spw(s).chans.size();
      for (Int i = 0; i < cCnt; i++) {
         Int sumChan = 0; 
         Int j;
         for (j = 0; j < aveChan && (i * aveChan + j) < aCnt; j++) {
            sumChan += spw(s).chans(i * aveChan + j);
         }
         spw(s).sxsChans(i) = ssChan++;
         spw(s).aveChans(i) = sumChan / j;
      }

      Vector<Double> chanFrequencies = 
              spwColumn.chanFreq()(spw(s).spwid);
      //cout << "chanfreq=" << chanFrequencies << endl; 

      for (Int i = 0; i < cCnt; i++) {
         Double freq = 0.;
         Int j;
         for (j = 0; j < aveChan && (i * aveChan + j) < aCnt; j++) {
             freq += chanFrequencies(spw(s).chans(i * aveChan + j));
         }
         spw(s).aveFreqs(i) = freq / j / 1.e9;
      }

      for (Int i = 0; i < cCnt; i++) {
         String nm("");
         Int j;
         Int start = spw(s).chans(i * aveChan);
         Int end = start;
         Int step = 1;
         Int k = 1;
         for (j = 1; j < aveChan && (i * aveChan + j) < aCnt; j++) {
            if (k++ == 1) {
               end = spw(s).chans(i * aveChan + j);
               step = end - start;
            }
            else {
               Int del = spw(s).chans(i * aveChan + j);
               if (del - end == step) {
                  end = del;
               }
               else {
                  nm += String("[") + String::toString(start) +
                        String(":") + String::toString(end) +
                        String(":") + String::toString(step) + String("]"); 
                  start = del;
                  end = start;
                  step = 1;
                  k = 1;
               }
            }
         }
         if (k > 0) {
            nm += String("[") + String::toString(start) +
                  String(":") + String::toString(end) +
                  String(":") + String::toString(step) + String("]"); 
         }
         spw(s).aveChanNames(i) = nm;
      }

      Matrix<Int> maps(aCnt, 5);
      Int rCnt = 0;
      for (Int i = 0; i < cCnt; i++) {
         //cout << "++++maps=" << maps;
         Int j;
         Int start = spw(s).chans(i * aveChan);
         Int end = start;
         Int step = 1;
         Int k = 1;
         for (j = 1; j < aveChan && (i * aveChan + j) < aCnt; j++) {
            if (k++ == 1) {
               end = spw(s).chans(i * aveChan + j);
               step = end - start;
            }
            else {
               Int del = spw(s).chans(i * aveChan + j);
               if (del - end == step) {
                  end = del;
               }
               else {
                  //cout << "rCnt=" << rCnt << " start=" << start 
                  //      << " end=" << end << " step=" << step << endl;
                  maps(rCnt, 0) = start;
                  maps(rCnt, 1) = end;
                  maps(rCnt, 2) = step;
                  maps(rCnt, 3) = i;
                  maps(rCnt, 4) = spw(s).spwid;
                  //cout << "----maps=" << maps;
                  start = del;
                  end = start;
                  step = 1;
                  k = 1;
                  rCnt++;
               }
            }
         }
         if (k > 0) {
            //cout << "rCnt=" << rCnt << " start=" << start 
            //     << " end=" << end << " step=" << step << endl;
            maps(rCnt, 0) = start;
            maps(rCnt, 1) = end;
            maps(rCnt, 2) = step;
            maps(rCnt, 3) = i;
            maps(rCnt, 4) = spw(s).spwid;
            //cout << "----maps=" << maps;
            rCnt++;
         }
      }
      spw(s).aveChanMaps.resize(0, 0);
      spw(s).aveChanMaps = maps(Slice(0, rCnt), Slice(0, 5));

      if (spw(s).aveChans.size() > nAveChan) {
         nAveChan = spw(s).aveChans.size();
      }

   }
   return nAveChan;

}

void SelectAverageSpw::chanMap(Matrix<Int>& cmap, const Vector<SAS>& spw) {

   Int sCnt = spw.size();
   Int cCnt = 0;
   for (Int s = 0; s < sCnt; s++) { 
      cCnt += spw(s).aveChanMaps.nrow(); 
   }

   cmap.resize(cCnt, 5);
   Int k = 0;
   for (Int s = 0; s < sCnt; s++) { 
      for (uInt t = 0; t < spw(s).aveChanMaps.nrow(); t++) {
         cmap(k, 0) = spw(s).aveChanMaps(t, 0); 
         cmap(k, 1) = spw(s).aveChanMaps(t, 1); 
         cmap(k, 2) = spw(s).aveChanMaps(t, 2); 
         cmap(k, 3) = spw(s).aveChanMaps(t, 3); 
         cmap(k, 4) = spw(s).aveChanMaps(t, 4); 
         k++; 
      }
   }
   return;
}

void SelectAverageSpw::averageVelocity(Bool &sorryVel,
            MS* pMS, Vector<SAS>& spw, 
            Vector<Double>& velo, const Int& spwidx, const Int& field,
            const String& restfreq, const String& frame,
            const String& doppler) {

   MSDerivedValues msdv;
   msdv.setMeasurementSet(*pMS);
   msdv.setVelocityReference(MDoppler::RADIO);
   //msdv.setFrequencyReference(MFrequency::LSRK);

   //fill in the averaged velocities
   //cout << "field=" << field << endl;

   Int sCnt = spw.size();
   if (sCnt < 1)
      return;

   Double cspeed = (QC::c( )).getValue() / 1000.;

   String itsRestFreq = restfreq;
   String itsFrame = frame;
   String itsDoppler = doppler;

   //cout << "itsRestFreq=" << itsRestFreq
   //      << " itsFrame=" << itsFrame
   //      << " itsDoppler=" << itsDoppler << endl;

   MFrequency::Types freqtp = MFrequency::LSRK;
   MDoppler::Types doptp = MDoppler::RADIO;
   MFrequency trans;
   Quantity qt;

   if (restfreq != "") {
      if (!MFrequency::getType(freqtp, itsFrame))
         freqtp = MFrequency::LSRK;
      if (!MDoppler::getType(doptp, itsDoppler))
         doptp = MDoppler::RADIO;
        
      msdv.setVelocityReference(doptp);
      msdv.setFrequencyReference(freqtp);

      if (MeasTable::Line(trans, itsRestFreq)) {
         qt = trans.get("GHz");
         msdv.setRestFrequency(qt);
	     // LogIO os(LogOrigin("SelectAverageSpw","averageVelocity"));
	     // os << LogIO::NORMAL << "setRestFrequency: "+ String::toString(qt.getValue()) + " " + qt.getUnit()
		     // << LogIO::POST;
         // SLog::slog()->out("setRestFrequency: " + String::toString(qt.getValue()) +
                   //" " + qt.getUnit(),
                    //"", "SAS", LogMessage::NORMAL5);
      }
      else {
    
          Double fr = 1;
          String unit = "";
          String dc = downcase(itsRestFreq);
          if (dc.contains("ghz")) {
             dc = dc.before("ghz");
             fr = atof(dc.chars());
             unit = "GHz";
          }
          else if (dc.contains("mhz")) {
             dc = dc.before("mhz");
             fr = atof(dc.chars());
             unit = "MHz";
          }
          //else if (dc.contains("khz")) {
          //   dc = dc.before("khz");
          //   fr = atof(dc.chars());
          //   unit = "KHz";
          //}
          else if (dc.contains("hz")) {
             dc = dc.before("hz");
             fr = atof(dc.chars());
             unit = "Hz";
          }

          //cout << "itsRestFreq=" << fr << " " << unit << endl;
          qt =  Quantity(fr, unit);
          msdv.setRestFrequency(qt);
          //SLog::slog()->out("setRestFrequency: " + String::toString(qt.getValue()) +
           //          " " + qt.getUnit(),
            //        "", "SAS", LogMessage::NORMAL5);
       }

       msdv.setFieldCenter(field);

       MSSpWindowColumns spwColumn(pMS->spectralWindow());
       //Int freqRef = spwColumn.measFreqRef()(spw(spwidx).spwid);
       Int cCnt = spw(spwidx).aveFreqs.size();
       for (Int k = 0; k < cCnt; k++) {
          Double tmp = msdv.toVelocity(
             Quantity(spw(spwidx).aveFreqs(k), "GHz")).get("km/s").getValue();
          velo(k) = (tmp < 0) ? max(tmp, -cspeed) : min(tmp, cspeed);
       }
   }
   else {
    
       msdv.setFieldCenter(field);
       Bool hasRestFreq = false;
       Int cCnt = spw(spwidx).aveFreqs.size();
       //cout << "spwidx=" << spwidx << " cCnt=" << cCnt << endl;
       velo = 0.;
    
       MSSpWindowColumns spwColumn(pMS->spectralWindow());
       Int freqRef = spwColumn.measFreqRef()(spw(spwidx).spwid);
    
       hasRestFreq = msdv.setRestFrequency(field, spw(spwidx).spwid);
       if (hasRestFreq) {
          msdv.setFrequencyReference(MFrequency::castType((uInt)freqRef));
       }
       else {
          //cout << "sorryVel=" << sorryVel << endl;
          msdv.setFrequencyReference(MFrequency::LSRK );
          msdv.setRestFrequency(Quantity(1.420, "GHz"));
          if (sorryVel) { 
	     LogIO os(LogOrigin("SelectAverageSpw","averageVelocity"));
	     os << LogIO::NORMAL << "No rest frequency found in the MS, can't caluclate velocity properly."
		     << LogIO::POST;
             //SLog::slog()->out(String("No rest frequency found in the MS,")
                     //+ String(" use LSRK 1.420 Ghz for velocity calculation"),
                     //+ String(" Can't calculate velocity properly."),
                      //"", "SAS", LogMessage::NORMAL, false);
             sorryVel = false;
          }
       }
    
       for (Int k = 0; k < cCnt; k++) {
          //velo(k) = 
          //   msdv.toVelocity(
          //   Quantity(spw(spwidx).aveFreqs(k), "GHz")
          // ).get("km/s").getValue();
          //cout << " k=" << k << " field=" << field
          //  << " reffreq=" << freqRef
          //  << " freq=" << spw(spwidx).aveFreqs(k)
          //  << endl;
          Double tmp = msdv.toVelocity(
             Quantity(spw(spwidx).aveFreqs(k), "GHz")).get("km/s").getValue();
          velo(k) = (tmp < 0) ? max(tmp, -cspeed) : min(tmp, cspeed);
       }
   }
   return;
}

void SelectAverageSpw::showSASC(const Vector<SAS>& spw) {
   Int sCnt = spw.size();
   for (Int i = 0; i < sCnt; i++) {
       cout << "spwid=" << spw(i).spwid 
            << " desc=" << spw(i).desc
            << " rFreq=" << std::setprecision(16) << spw(i).rFreq
            << " measFreqRef=" << spw(i).measFreqRef
            << " chans=" << spw(i).chans 
            << " aveChans" << spw(i).aveChans
            << " sxsChans" << spw(i).sxsChans
            << " aveFreqs" << spw(i).aveFreqs
            << " aveChanNames" << spw(i).aveChanNames
            << " aveChanMaps" << spw(i).aveChanMaps
            << endl;
   }
   return;
} 

Int SelectAverageSpw::descBySpw(const Int& spid, const Vector<SAS>& spw) {
    Int descid = -1;
    for (uInt i = 0; i < spw.size(); i++) {
       if (spw(i).spwid == spid) {
          descid = spw(i).desc; 
          break;
       }
    }
    return descid;
}

Int SelectAverageSpw::spwByDesc(const Int& desc, const Vector<SAS>& spw) {
    Int spid = -1;
    for (uInt i = 0; i < spw.size(); i++) {
       if (spw(i).desc == desc) {
          spid = spw(i).spwid; 
          break;
       }
    }
    return spid;
}

Int SelectAverageSpw::spwIndexByDesc(
        const Int& desc, const Vector<SAS>& spw) {
    Int idx = -1;
    for (uInt i = 0; i < spw.size(); i++) {
       if (spw(i).desc == desc) {
          idx = i; 
          break;
       }
    }
    return idx;
}

Int SelectAverageSpw::spwIndexBySpw(
        const Int& spid, const Vector<SAS>& spw) {
    Int idx = -1;
    for (uInt i = 0; i < spw.size(); i++) {
       if (spw(i).spwid == spid) {
          idx = i; 
          break;
       }
    }
    return idx;
}


}

