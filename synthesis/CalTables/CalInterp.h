//# CalInterp.h: Definition for Calibration parameter Interpolation
//# Copyright (C) 1996,1997,1998,1999,2000,2001,2002,2003
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
//# Correspondence concerning AIPS++ should be adressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//#

#ifndef CALTABLES_CALINTERP_H
#define CALTABLES_CALINTERP_H
#include <casa/aips.h>
#include <casa/BasicSL/Constants.h>
#include <synthesis/CalTables/CalSet.h>
#include <casa/OS/File.h>
#include <casa/Logging/LogMessage.h>
#include <casa/Logging/LogSink.h>

namespace casa { //# NAMESPACE CASA - BEGIN


class CalInterp
{
public:

  // Null ctor does nothing
  CalInterp() {};

  // From CalSet 
  CalInterp(CalSet<casacore::Complex>& cs,
	    const casacore::String& timetype,
	    const casacore::String& freqtype);

  // Destructor
  virtual ~CalInterp();


  // Set non-trivial spw mapping
  void setSpwMap(const casacore::Vector<casacore::Int>& spwmap) {spwMap_ = spwmap; setSpwOK();};

  // Interpolate, given timestamp, spw, freq list; returns T if new result
  casacore::Bool interpolate(const casacore::Double& time,
		   const casacore::Int& spw,
		   const casacore::Vector<casacore::Double>& freq=casacore::Vector<casacore::Double>());

  // Do time-dep interpolation (called by interpolate)
  casacore::Bool interpTime(const casacore::Double& time);

  // Do freq-dep interpolation (called by interpolate)
  void interpFreq(const casacore::Vector<casacore::Double>& freq);

  // Timestamp of current Slot
  casacore::Double slotTime() { return csTimes()(currSlot()); };

  // Access to result (by reference)
  inline casacore::Cube<casacore::Complex>& result() { return r; };
  inline casacore::Cube<casacore::Bool>& resultOK() { return ok; };

  // Spwmap-sensitive spwOK()
  inline casacore::Vector<casacore::Bool> spwOK() { return spwOK_; };

  // Set spwOK_ according to spwmap  RI made public 200910
  void setSpwOK();

protected:

  // Find the reference timeslot for requested time
  casacore::Bool findSlot(const casacore::Double& time);

  void updTimeCoeff();

  void interpTimeCalc(const casacore::Double& time);

  void initFreqInterp(const casacore::Vector<casacore::Double> freq);

  void calcAPC();

  void updFreqCoeff();

  void interpFreqCalc();

  void finalize();

  void inflTimeC();
  void inflFreqC();
  void deflTimeC();
  void deflFreqC();
  void deflFreqA();

  // Access to private shape data (reflecting CalSet)
  inline casacore::Int& nSpw()  { return cs_->nSpw();  };
  inline casacore::Int& nPar()  { return cs_->nPar();  };
  inline casacore::Int& nElem() { return cs_->nElem(); };

  // Access to imutable private data
  inline casacore::String& timeType()  { return timeType_; };
  inline casacore::Bool   nearestT() { return (timeType()=="nearest"); };
  inline casacore::Bool   linearT()  { return (timeType()=="linear"); };
  inline casacore::Bool   aipslinT() { return (timeType()=="aipslin"); };
  inline casacore::String& freqType()  { return freqType_; };
  inline casacore::Bool   nearestF() { return (freqType()=="nearest"); };
  inline casacore::Bool   linearF()  { return (freqType()=="linear"); };
  inline casacore::Bool   aipslinF() { return (freqType()=="aipslin"); };

  inline casacore::Vector<casacore::Int>& spwMap()  { return spwMap_; };
  inline casacore::Int spwMap(const casacore::Int& spw) { return ((spwMap_(spw)>-1) ? spwMap_(spw) : spw); };

  // Access to info relevant to currSpw
  inline casacore::Int& currSpw()     { return currSpw_; };
  inline casacore::Int currSpwMap()  { return spwMap(currSpw()); };
  inline casacore::Int& currSlot()    { return currSlot_(currSpw()); };
  inline casacore::Int& nFreq()       { return nFreq_(currSpw()); };
  inline casacore::Vector<casacore::Double>& datFreq() { return (*datFreq_[currSpw()]); };
  inline casacore::Bool& finit()      { return finit_(currSpw()); };
  inline casacore::Double& lastTime() { return lastTime_(currSpw_); };
  inline casacore::Bool& exactTime()  { return exactTime_; };

  // Access to in-focus portion of CalSet (spwmap'd)
  inline casacore::Int& nTime()              { return cs_->nTime(currSpwMap()); };
  inline casacore::Int& nChan()              { return cs_->nChan(currSpwMap()); }
  inline casacore::Vector<casacore::Double>& csTimes() { return cs_->time(currSpwMap()); };
  inline casacore::Vector<casacore::Double>& csFreq()  { return cs_->frequencies(currSpwMap()); };
  inline casacore::Array<casacore::Complex>& csPar()   { return cs_->par(currSpwMap()); };
  inline casacore::Array<casacore::Bool>&    csParOK() { return cs_->parOK(currSpwMap()); };

  // Access to IPositions
  inline casacore::IPosition&  ip4d() { return (*ip4d_[currSpw_]); };
  inline casacore::IPosition&  ip3d() { return (*ip3d_[currSpw_]); };
  inline casacore::IPosition&  ip2d() { return (*ip2d_[currSpw_]); };

  // Access to in-focus interpolation abscissa data
  inline casacore::Double&         t0()  {return t0_(currSpw_);};
  inline casacore::Double&         tS()  {return tS_(currSpw_);};
  inline casacore::Int&        lastlo()  {return lastlo_(currSpw_); };

  inline casacore::Vector<casacore::Int>&    ch0() {return (*ch0_[currSpw_]); };
  inline casacore::Vector<casacore::Bool>&   ef()  {return (*ef_[currSpw_]); };
  inline casacore::Vector<casacore::Double>& df()  {return (*df_[currSpw_]); };
  inline casacore::Vector<casacore::Double>& fdf() {return (*df_[currSpw_]); };

  // In-focus casacore::Time Interpolation coefficients 
  inline casacore::Array<casacore::Float>&   tAC() {return (*tAC_[currSpw()]); };
  inline casacore::Array<casacore::Float>&   tPC() {return (*tPC_[currSpw()]); };
  inline casacore::Array<casacore::Complex>& tCC() {return (*tCC_[currSpw()]); };
  inline casacore::Cube<casacore::Bool>&     tOk() {return (*tOk_[currSpw()]); };

  // In-focus Freq Interpolation coefficients
  inline casacore::Array<casacore::Float>&   fAC() {return (*fAC_[currSpw()]); };
  inline casacore::Array<casacore::Float>&   fPC() {return (*fPC_[currSpw()]); };
  inline casacore::Array<casacore::Complex>& fCC() {return (*fCC_[currSpw()]); };
  inline casacore::Cube<casacore::Bool>&     fOk() {return (*fOk_[currSpw()]); };

  inline void rPart(casacore::Array<casacore::Complex>& c,casacore::Array<casacore::Float>& rp) { return part(c,0,rp); };
  inline void iPart(casacore::Array<casacore::Complex>& c,casacore::Array<casacore::Float>& ip) { return part(c,1,ip); };

  void asFloatArr(const casacore::Array<casacore::Complex>& in, casacore::Array<casacore::Float>& out);

  void part(const casacore::Array<casacore::Complex>& c, const casacore::Int& which, casacore::Array<casacore::Float>& f);

private:

  // The CalSet from which we interpolate
  CalSet<casacore::Complex>* cs_;

  // Interpolation modes
  casacore::String timeType_, freqType_;

  // Spw map
  casacore::Vector<casacore::Int> spwMap_;

  // SpwOK
  casacore::Vector<casacore::Bool> spwOK_;

  // Prior interp time, per spw
  casacore::Vector<casacore::Double> lastTime_;

  // Frequency initialization flag (true if init complete)
  casacore::Vector<casacore::Bool> finit_;

  // Number of channels
  casacore::Vector<casacore::Int> nFreq_;          // (nSpw_) number of _data_ channels per spw

  // Freq lists
  casacore::PtrBlock<casacore::Vector<casacore::Double>*> solFreq_;   // [nSpw](nChan)
  casacore::PtrBlock<casacore::Vector<casacore::Double>*> datFreq_;   // [nSpw](nFreq)

  casacore::Int currSpw_;
  casacore::Vector<casacore::Int> currSlot_;

  // Exact time match flag
  casacore::Bool exactTime_;

  casacore::Cube<casacore::Float> a,p;    // For referencing interp results
  casacore::Cube<casacore::Complex> c;
  casacore::Cube<casacore::Bool> ok;

  casacore::Cube<casacore::Complex> r, r_;

  // casacore::Array shapes
  casacore::PtrBlock<casacore::IPosition*> ip4d_;     // [nSpw]
  casacore::PtrBlock<casacore::IPosition*> ip3d_;     // [nSpw]
  casacore::PtrBlock<casacore::IPosition*> ip2d_;     // [nSpw]

  // casacore::Time Interpolation ordinate info (per spw)
  casacore::Vector<casacore::Double>            t0_;         // (nSpw)
  casacore::Vector<casacore::Double>            tS_;         // (nSpw)
  casacore::Vector<casacore::Int>               lastlo_;     // (nSpw)

  // casacore::Time Interpolation coefficients (per spw)
  casacore::PtrBlock<casacore::Array<casacore::Float>*>   tAC_, tPC_;  // [nSpw](2,nPar,nChan,nElem)
  casacore::PtrBlock<casacore::Array<casacore::Complex>*> tCC_;        // [nSpw](2,nPar,nChan,nElem)
  casacore::PtrBlock<casacore::Cube<casacore::Bool>*>     tOk_;        // [nSpw](nPar,nChan,nElem)

  // casacore::Time Interpolation results (currSpw)
  casacore::Cube<casacore::Float>    tA_, tP_;    // (nPar,nChan,nElem)
  casacore::Cube<casacore::Complex>  tC_;         // (nPar,nChan,nElem)

  // Freq Interpolation ordinate info (per spw)
  casacore::PtrBlock<casacore::Vector<casacore::Int>*>    ch0_;        // [nSpw](nFreq)
  casacore::PtrBlock<casacore::Vector<casacore::Bool>*>   ef_;         // [nSpw](nFreq)
  casacore::PtrBlock<casacore::Vector<casacore::Double>*> df_;         // [nSpw](nFreq)
  casacore::PtrBlock<casacore::Vector<casacore::Double>*> fdf_;        // [nSpw](nFreq)

  // Freq Interpolation coefficients (per spw)
  casacore::PtrBlock<casacore::Array<casacore::Float>*>   fAC_, fPC_;  // [nSpw](2,nPar,nFreq,nElem)
  casacore::PtrBlock<casacore::Array<casacore::Complex>*> fCC_;        // [nSpw](2,nPar,nFreq,nElem)
  casacore::PtrBlock<casacore::Cube<casacore::Bool>*>     fOk_;        // [nSpw](nPar,nFreq,nElem)

  // Freq Interpolation results (currSspw)
  casacore::Cube<casacore::Float>    fA_, fP_;               // (nPar,nFreq,nElem)
  casacore::Cube<casacore::Complex>  fC_;                    // (nPar,nFreq,nElem)

  casacore::LogSink logSink_p;
  casacore::LogSink& logSink() {return logSink_p;};

  casacore::Bool verbose_;

};


} //# NAMESPACE CASA - END

#endif
