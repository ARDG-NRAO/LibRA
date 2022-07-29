//# ExtendFlagger.cc: 
//# Copyright (C) 1994,1995,1996,1997,1998,1999,2000,2001,2002,2003-2008
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


//# Includes

#include <cmath>

#include <casacore/casa/Exceptions.h>

#include <casacore/tables/TaQL/TableParse.h>
#include <casacore/tables/TaQL/TableGram.h>
#include <casacore/tables/Tables/TableDesc.h>
#include <casacore/tables/Tables/TableLock.h>
#include <casacore/tables/Tables/TableIter.h>

#include <casacore/casa/Arrays/ArrayMath.h>
#include <casacore/casa/Arrays/MatrixMath.h>
#include <casacore/casa/Arrays/ArrayError.h>
#include <casacore/casa/Arrays/Matrix.h>

#include <casacore/tables/TaQL/ExprMathNode.h>
#include <casacore/tables/TaQL/ExprMathNodeArray.h>
#include <casacore/tables/TaQL/ExprDerNode.h>
#include <casacore/tables/TaQL/ExprDerNodeArray.h>
#include <casacore/tables/TaQL/ExprFuncNode.h>
#include <casacore/tables/TaQL/ExprFuncNodeArray.h>
#include <casacore/tables/TaQL/ExprLogicNode.h>
#include <casacore/tables/TaQL/ExprLogicNodeArray.h>
#include <casacore/tables/TaQL/ExprNodeArray.h>
#include <casacore/tables/TaQL/ExprNodeSet.h>
#include <casacore/tables/TaQL/ExprNodeRep.h>
#include <casacore/tables/TaQL/ExprNodeRecord.h>
#include <casacore/tables/TaQL/ExprRange.h>
#include <casacore/tables/TaQL/RecordGram.h>

#include <casacore/casa/Utilities/DataType.h>
#include <casacore/casa/Utilities/Sort.h>
#include <casacore/casa/Quanta/MVTime.h>
#include <casacore/casa/System/ProgressMeter.h>

#include <flagging/Flagging/ExtendFlagger.h>

#include <iomanip>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

int FlagID::compare (const void* val1, const void* val2) {
        const FlagID& t1 = *(FlagID*)val1;
        const FlagID& t2 = *(FlagID*)val2;
        if (t1.time < t2.time) return -1;
        if (t1.time > t2.time) return 1;
        if (t1.ant1 < t2.ant1) return -1;
        if (t1.ant1 > t2.ant1) return 1;
        if (t1.ant2 < t2.ant2) return -1;
        if (t1.ant2 > t2.ant2) return 1;
        //if (t1.field < t2.field) return -1;
        //if (t1.field > t2.field) return 1;
        if (t1.spw < t2.spw) return -1;
        if (t1.spw > t2.spw) return 1;
        if (t1.corr < t2.corr) return -1;
        if (t1.corr > t2.corr) return 1;
        if (t1.chan < t2.chan) return -1;
        if (t1.chan > t2.chan) return 1;
        if (t1.polid < t2.polid) return -1;
        if (t1.polid > t2.polid) return 1;
        return 0;
}

int FlagIDS::compareID (const void* val1, const void* val2) {
        const FlagIDS& t1 = *(FlagIDS*)val1;
        const FlagIDS& t2 = *(FlagIDS*)val2;
    // this condition makes no sense and produces a clang compiler warning
    // warning: reference cannot be bound to dereferenced null pointer in well-defined C++ code;
    // comparison may be assumed to always evaluate to true [-Wtautological-undefined-compare]
    // if (&t1 != 0 && &t2 != 0) {  
        cout << "t1=" << t1.time << " " << t2.time << endl;
        if (t1.time < t2.time) return -1;
        if (t1.time > t2.time) return 1;
        if (t1.bl < t2.bl) return -1;
        if (t1.bl > t2.bl) return 1;
        //if (t1.field < t2.field) return -1;
        //if (t1.field > t2.field) return 1;
        if (t1.spw < t2.spw) return -1;
        if (t1.spw > t2.spw) return 1;
        if (t1.corr < t2.corr) return -1;
        if (t1.corr > t2.corr) return 1;
        if (t1.chan < t2.chan) return -1;
        if (t1.chan > t2.chan) return 1;
    // } 
        
    return 0;
}


// Default Constructor 
ExtendFlagger::ExtendFlagger()
{
   initdata();
   clipexpr = "";
   cliprange.resize(0);
   clipcolumn = "DATA";
   outside = false;
   channelavg = false;
   quackinterval = 0.0;
   opmode = "flag";
   extendCorr = "";
   extendChan = "";
   extendAnt = "";
   extendSpw = "";
   extendTime = "";
   ableCorr.resize(0, 0);
   chanNum.resize(0);
}

ExtendFlagger::ExtendFlagger(MeasurementSet& ms, const String& exchan, 
                const String& excorr, const String& exant, 
                const String& exspw, const String& time,
                const Vector<Vector<String> >& corrs, const Vector<Int>& nchan) {
   initdata();
   clipexpr = "";
   cliprange.resize(0);
   clipcolumn = "DATA";
   outside = false;
   channelavg = false;
   quackinterval = 0.0;
   opmode = "flag";
   extendCorr = excorr;
   extendChan = exchan;
   extendAnt = exant;
   extendSpw = exspw;
   extendTime = time;
   ableCorr = corrs;
   chanNum = nchan;
   
   flagger.attach(ms);
}

// Destructor 
ExtendFlagger::~ExtendFlagger()
{
}

// attach the ms to be flaggered
Bool ExtendFlagger::attach(MeasurementSet &ms) {
   return flagger.attach(ms);
}

// detach the ms 
void ExtendFlagger::detach() {
   flagger.detach();
}

// initialize flagging selection string 
Bool ExtendFlagger::initdata(const String& field, const String& spw, 
            const String& array, const String& feed, 
            const String& scan, const String& baseline,
            const String& uvrange, const String& time, 
            const String& correlation, const String& intent) {

   this->field = field;
   this->spw = spw;
   this->array = array;
   this->feed = feed;
   this->scan = scan;
   this->baseline = baseline;
   this->uvrange = uvrange;
   this->time = time;
   this->correlation = correlation;
   this->intent = intent;
   return true;

}

// set flagging selection string 
Bool ExtendFlagger::setdata() {
   return flagger.setdata(field, spw, array,
         feed, scan, baseline, uvrange, time, correlation, intent); 
}

// make data selection 
Bool ExtendFlagger::selectdata(Bool useoriginalms) {
   //cout << "useoriginalms=" << useoriginalms
   //     << " field=" << field << " spw=" << spw
   //     << " array=" << array << " feed=" << feed
   //     << " scan=" << scan << " baseline=" << baseline
   //     << " uvrange=" << uvrange << " time=" << time
   //     << " correlation=" << correlation << endl;
   //useoriginalms = false;
   return flagger.selectdata(useoriginalms, field, spw, array,
         feed, scan, baseline, uvrange, time, correlation, intent); 
}

// 
Bool ExtendFlagger::setmanualflags(Bool unflag, Bool autocorr) {
   //cout << "autocorr=" << autocorr << " rowflag=" << rowflag
   //     << " unflag=" << unflag 
   //     << " clipexpr=" << clipexpr << " cliprange=" << cliprange
   //     << " clipcolumn=" << clipcolumn << " outside=" << outside
   //     << " quackinterval=" << quackinterval << " opmode=" << opmode
   //     << endl;
   return flagger.setmanualflags(autocorr, unflag, clipexpr, 
        cliprange, clipcolumn, outside, channelavg, quackinterval, "beg", false, opmode);
}

Bool ExtendFlagger::run(Bool trial, Bool reset) {
  flagger.run(trial, reset);
  return true;
}

Bool ExtendFlagger::extend2(const Vector<FlagID>& flagids) {
   //cout << " extendCorr=" + extendCorr
   //     << " extendChan=" + extendChan
   //     << " extendSpw=" + extendSpw
   //     << " extendAnt=" + extendAnt
   //     << " extendTime=" + extendTime
   //     << endl;
   Int npts = flagids.size();
   if (npts == 0) {
      return false;
   } 

   Vector<FlagIDS> fids(npts);
   for (Int i = 0; i < npts; i++) {

      FlagIDS fid;

      //set chan
      if (!upcase(extendChan).compare("ALL")) {
          fid.chan = "";
      }
      else {
          fid.chan = flagids(i).chan;
      }

      //set spw
      if (!upcase(extendSpw).compare("ALL")) {
         fid.spw = ableSpw(flagids(i).spw);
      }
      else {
         fid.spw = String::toString(flagids(i).spw);
      }

      //set correlation
      if (!upcase(extendCorr).compare("ALL")) {
         fid.corr = "";
      }
      else if (!upcase(extendCorr).compare("HALF")) {
         fid.corr = halfExtendCorr(flagids(i).corr);
      }
      else {
         fid.corr = flagids(i).corr;
      }

      //set baseline
      if (!upcase(extendAnt).compare("ALL")) {
         fid.bl = "";
      }
      else {
         fid.bl = String::toString(flagids(i).ant1) +
                  String("&") +
                  String::toString(flagids(i).ant2);
      }

      //set time
      if (!upcase(extendTime).compare("ALL")) {
         fid.time = "";
      }
      else {
         fid.time = flagids(i).time;
      }

      fid.field = flagids(i).field;

      fids(i) = fid;
   }

   for (Int i = 0; i < npts; i++) { 
      fids[i].show();
   }

   Vector<uInt> indexV(npts); 
   Vector<uInt> uniqueV(npts); 
   Sort sort;
   //sort.sortKey (&fids, &(FlagIDS::compareID), sizeof(FlagIDS));
   sort.sortKey (&fids[0].spw, TpString, sizeof(FlagIDS));
   sort.sortKey (&fids[0].time, TpString, sizeof(FlagIDS));
   sort.sortKey (&fids[0].bl, TpString, sizeof(FlagIDS));
   sort.sortKey (&fids[0].corr, TpString, sizeof(FlagIDS));
   sort.sortKey (&fids[0].chan, TpString, sizeof(FlagIDS));

   uInt sr = 
     sort.sort (indexV, npts);

   uInt ur = sort.unique (uniqueV, indexV);
   cout 
          << "npts=" << npts
          << " sorted=" << sr 
          << " unique=" << ur
          << endl;

   Bool ret = true;
   for (uInt i = 0; i < ur; i++) {

      FlagIDS usFlag = fids[indexV(uniqueV(i))];
      // set spw:chan
      // need to be careful of the variable shape spws
      if (usFlag.chan.length() > 0) {
         setSpw(usFlag.spw + String(":") + usFlag.chan); 
      }
      else {
         setSpw(usFlag.spw);
      }

      //set correlation
      setCorrelation(usFlag.corr);

      //set baseline
      setBaseline(usFlag.bl);

      //set time
      setTime(usFlag.time);

      setField(String::toString(usFlag.field));

      selectdata();
      setmanualflags(unflag);
   }
   try {
     ret = run();
   }
   catch (AipsError ex) {
      LogIO log;
           log<<LogIO::WARN<< LogOrigin("Flagger")
           <<ex.getMesg()
           <<LogIO::POST;
   }

   return ret;
}

Bool ExtendFlagger::extend(const Vector<FlagID>& flagids) {
   //cout << " extendCorr=" + extendCorr
   //     << " extendChan=" + extendChan
   //     << " extendSpw=" + extendSpw
   //     << " extendAnt=" + extendAnt
   //     << " extendTime=" + extendTime
   //     << endl;
   Int npts = flagids.size();
   if (npts == 0) {
      return false;
   } 

   Bool ret = true;
   for (Int i = 0; i < npts; i++) {
      String chanstr = flagids(i).chan;
      if (!upcase(extendChan).compare("ALL")) {
          chanstr = "";
      }

      String spwstr = String::toString(flagids(i).spw);
      if (!upcase(extendSpw).compare("ALL")) {
         spwstr = ableSpw(flagids(i).spw);
      }

      // set spw:chan
      // need to be careful of the variable shape spws
      if (chanstr.length() > 0) {
         setSpw(spwstr + String(":") + chanstr); 
      }
      else {
         setSpw(spwstr);
      }

      //set correlation
      if (!upcase(extendCorr).compare("ALL")) {
         setCorrelation("");
      }
      else if (!upcase(extendCorr).compare("HALF")) {
         setCorrelation(halfExtendCorr(flagids(i).corr));
      }
      else {
         setCorrelation(flagids(i).corr);
      }

      //set baseline
      // flagids.ant1 is the ant id, we need ant name
      //String blstr = String::toString(flagids(i).ant1) +
      //                String("&") +
      //                String::toString(flagids(i).ant2);

      //const MSAntennaColumns & antColumn  = ms.antenna();
      //const ScalarColumn<String> antNames = antColumn.name();
      //String blstr = (antColumn.name())(flagids(i).ant1) +
      //                 String("&") +
      //               (antColumn.name())(flagids(i).ant2);
      
      String blstr = "";
      try {
         Vector<String> ants = flagger.antNames();
         blstr = ants(flagids(i).ant1) +
                 String("&") +
                 ants(flagids(i).ant2);
      }
      catch(...) {}
      
      if (!upcase(extendAnt).compare("ALL")) {
         setBaseline("");
      }
      else {
         setBaseline(blstr);
      }

      //set time
      String timestr = flagids(i).time;
      if (!upcase(extendTime).compare("ALL")) {
         setTime("");
      }
      else {
         setTime(timestr);
      }

      setField(String::toString(flagids(i).field));

      selectdata();
      setmanualflags(unflag);
      //ret = run();
      
      //show(); 
   }
   try {
     ret = run();
   }
   catch (AipsError ex) {
      LogIO log;
      log<<LogIO::WARN << LogOrigin("Flagger")
                <<ex.getMesg()
          <<LogIO::POST;
   }
   

   return ret;
}


void ExtendFlagger::show() {
    cout << "time=" << time
      << " spw=" << spw
      << " field=" << field
      //String array;
      //String feed;
      //String scan;
      << " baseline=" << baseline
      //String uvrange;
      << " corr=" << correlation
      << endl;
}

void ExtendFlagger::setUnflag(Bool unflg) {
   this->unflag = unflg; 
}

void ExtendFlagger::setField(const String& field) {
   this->field = field; 
}

void ExtendFlagger::setSpw(const String& spw) {
   this->spw = spw;
}

void ExtendFlagger::setArray(const String& array) {
   this->array = array;
}

void ExtendFlagger::setFeed(const String& feed) {
   this->feed = feed;
}

void ExtendFlagger::setScan(const String& scan) {
   this->scan = scan;
}
void ExtendFlagger::setBaseline(const String& baseline) {
   this->baseline = baseline;
}

void ExtendFlagger::setUvrange(const String& uvrange) {
   this->uvrange = uvrange;
}

void ExtendFlagger::setTime(const String& time) {
   this->time = time;
}

void ExtendFlagger::setCorrelation(const String& correlation) {
   this->correlation = correlation;
}

void ExtendFlagger::setIntent(const String& intent) {
   this->intent = intent;
}

void ExtendFlagger::setExtendChan(const String& exchan) {
   this->extendChan = exchan;
}

void ExtendFlagger::setExtendCorr(const String& excorr) {
   this->extendCorr = excorr;
}
void ExtendFlagger::setExtendAnt(const String& exant) {
   this->extendAnt = exant;
}

void ExtendFlagger::setExtendSpw(const String& exspw) {
   this->extendSpw = exspw;
}

void ExtendFlagger::setExtendTime(const String& time) {
   this->extendTime = time;
}

void ExtendFlagger::setExtend(const String& exchan, const String& excorr,
                const String& exspw, const String& exant, const String& time,
                const Vector<Vector<String> >& cname, const Vector<Int>& nchan) {
   this->extendCorr = excorr;
   this->extendChan = exchan;
   this->extendAnt = exant;
   this->extendSpw = exspw;
   this->extendTime = time;
   this->ableCorr = cname;
   this->chanNum = nchan;
}

void ExtendFlagger::setAbleCorr(const Vector<Vector<String> >& corrName) {
   this->ableCorr = corrName;
}

void ExtendFlagger::setChanNum(const Vector<Int>& nchan) {
   this->chanNum = nchan;
}

String ExtendFlagger::halfExtendCorr(const String& corr, const uInt pid) {
   String ex = corr;
   if (ableCorr.size() > pid) {
      uInt corrCount = ableCorr[pid].nelements();
      if (corrCount == 2) {
         return corr;
      }
      //cout << "corr=" << corr << " corrCount=" << corrCount << endl;
      if (corrCount == 4) {
         uInt i = 0;
         for (i = 0; i < corrCount; i++) {
            if (corr.length() > 0 && !upcase(corr).compare(ableCorr[pid][i])) {
               break;
            }
         }
         if (i == 0) {
            return ableCorr[pid][0] + "," +
                   ableCorr[pid][1] + "," +
                   ableCorr[pid][2];
         }
         if (i == 1) {
            return ableCorr[pid][0] + "," +
                   ableCorr[pid][1] + "," +
                   ableCorr[pid][3];
         }
         if (i == 2) {
            return ableCorr[pid][0] + "," +
                   ableCorr[pid][2] + "," +
                   ableCorr[pid][3];
         }
         if (i == 3) {
            return ableCorr[pid][1] + "," +
                   ableCorr[pid][2] + "," +
                   ableCorr[pid][3];
         }
      }
   }
   return "";
}

String ExtendFlagger::ableSpw(const Int spw) {
   String ex = "";
   Int nspw = chanNum.nelements();
   if (nspw > spw) {
      Int nchan = chanNum[spw];
      for (Int i = 0; i < nspw; i++) {
         if (chanNum[i] == nchan) {
            ex += String(";") + String::toString(i);
         }
      }
      return ex.after(";");
   }
   return "";
}


} //# NAMESPACE CASA - END 
