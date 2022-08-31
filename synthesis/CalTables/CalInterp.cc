//# CalInterp.cc: Implementation of Calibration Interpolation
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
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#

#include <synthesis/CalTables/CalInterp.h>

#include <casacore/casa/Arrays.h>
#include <casacore/scimath/Mathematics/MatrixMathLA.h>
#include <casacore/casa/BasicSL/String.h>
#include <casacore/casa/Utilities/Assert.h>
#include <casacore/casa/Utilities/BinarySearch.h>
#include <casacore/casa/Exceptions/Error.h>

#include <sstream>

#include <casacore/casa/Logging/LogMessage.h>
#include <casacore/casa/Logging/LogSink.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

CalInterp::CalInterp(CalSet<Complex>& cs,
		     const String& timetype,
		     const String& freqtype) :
  cs_(&cs),
  timeType_(timetype),
  freqType_(freqtype),
  spwMap_(cs.nSpw(),-1),
  spwOK_(cs.nSpw(),false),
  lastTime_(cs.nSpw(),-1.0e99),
  finit_(cs.nSpw(),false),
  nFreq_(cs.nSpw(),1),
  solFreq_(cs.nSpw(),NULL),
  datFreq_(cs.nSpw(),NULL),
  currSpw_(-1),
  currSlot_(cs.nSpw(),-1),
  exactTime_(false),

  ip4d_(cs.nSpw(),NULL),
  ip3d_(cs.nSpw(),NULL),
  ip2d_(cs.nSpw(),NULL),

  t0_(cs.nSpw(),0.0),
  tS_(cs.nSpw(),0.0),
  lastlo_(cs.nSpw(),-1),

  tAC_(cs.nSpw(),NULL),
  tPC_(cs.nSpw(),NULL),
  tCC_(cs.nSpw(),NULL),
  tOk_(cs.nSpw(),NULL),

  ch0_(cs.nSpw(),NULL),
  ef_(cs.nSpw(),NULL),
  df_(cs.nSpw(),NULL),
  fdf_(cs.nSpw(),NULL),

  fAC_(cs.nSpw(),NULL),
  fPC_(cs.nSpw(),NULL),
  fCC_(cs.nSpw(),NULL),
  fOk_(cs.nSpw(),NULL),

  verbose_(false)

{

  if (verbose_) cout << "CalInterp::constructor" << endl;

  if (verbose_) cout << " timeType_ = " << timeType_ << endl;
  if (verbose_) cout << " freqType_ = " << freqType_ << endl;

  // Nominally, spwOK_ follows CalSet
  spwOK_ = cs_->spwOK();

  // Allocate (zero-size) working arrays and shapes
  //  (will resize non-trivially as needed)
  for (currSpw_=0;currSpw_<cs.nSpw();currSpw_++) {

    // nFreq()=1 here, will set this in initFreqInterp
    ip4d_[currSpw()] = new IPosition(4,2,nPar(),nFreq(),nElem());
    ip3d_[currSpw()] = new IPosition(3,nPar(),nFreq(),nElem());
    ip2d_[currSpw()] = new IPosition(2,nFreq(),nElem());

    tAC_[currSpw()] = new Array<Float>();
    tOk_[currSpw()] = new Cube<Bool>();
    tPC_[currSpw()] = new Array<Float>();
    tCC_[currSpw()] = new Array<Complex>();
    
    fAC_[currSpw()] = new Array<Float>();
    fOk_[currSpw()] = new Cube<Bool>();
    fPC_[currSpw()] = new Array<Float>();
    fCC_[currSpw()] = new Array<Complex>();
    
    df_[currSpw()]  = new Vector<Double>();
    fdf_[currSpw()] = new Vector<Double>();
    ch0_[currSpw()] = new Vector<Int>();
    ef_[currSpw()]  = new Vector<Bool>();
  }

  currSpw_=-1;

};


CalInterp::~CalInterp() {

  if (verbose_) cout << "CalInterp::destructor" << endl;
  deflTimeC();
  deflFreqC();
  deflFreqA();
}


Bool CalInterp::interpolate(const Double& time,
			    const Int& spw,
			    const Vector<Double>& /*freq*/) {

  if (verbose_) cout << endl << "CalInterp::interpolate()" << endl;

  // TODO:
  //  - catch case where requested spw has no solutions

  // Assume no change, for starters
  Bool newInterp(false);

  // If spw has changed, re-map spw
  currSpw()=spw;

  newInterp = interpTime(time);

  // Interpolate in Freq
  //  if (newInterp) interpFreq(freq);

  // Finalize interpolation
  finalize();

  return newInterp;

}


Bool CalInterp::interpTime(const Double& time) {

  // Interpolate in Time
  if (verbose_) cout << "CalInterp::interpTime()" << endl;

  Bool newTime=false;         // assume no new calculations needed

  Bool newSlot(false);
  if (time != lastTime() ) {
    lastTime()=time;

    // if not "nearest", must recalcuate time interp
    if ( !nearestT() ) newTime=true;

    // Find relevant time slot
    newSlot = findSlot(time);

    if (newSlot) 
      newTime = true;

    if (!exactTime() && !nearestT())
      updTimeCoeff();
    
  }

  if (verbose_) cout << "   " << boolalpha 
		     << "newTime = " << newTime << " " 
		     << "newSlot = " << newSlot << " " 
		     << "currSlot() = " << currSlot() << " "
		     << "fieldId = " << cs_->fieldId(currSpwMap())(currSlot()) << " "
		     << "exactTime() = " << exactTime()  << " "
		     << "nearestT() = " << nearestT()  << " "
		     << endl;

  // newTime=true here if we need to re-calc time interp (*any* mode)

  if (newTime) {
    
    if ( nearestT() || exactTime() ) {
      exactTime_=true;   // Behave as exact

      if (verbose_) cout << "   "
			 << "FOUND EXACT TIME!" << endl;
      // Just reference CalSet parameter
      IPosition blc(4,0,0,0,currSlot());
      IPosition trc(4,nPar()-1,nChan()-1,nElem()-1,currSlot());

      Cube<Complex> t;
      t.reference(csPar()(blc,trc).reform(IPosition(3,nPar(),nChan(),nElem())));
      r_.reference(t);
      tOk().reference(csParOK()(blc,trc).reform(IPosition(3,nPar(),nChan(),nElem())));

    } else {

      if (verbose_) cout << "   "
			 << "NON-EXACT TIME." << endl;

      // Do non-trivial interpolation
      //      r_.resize(0,0,0);


      interpTimeCalc(time);
    }
    
  if (verbose_) 
    cout << " CalInterp addr: " << r_.data()
	 << endl;



  } 

  return newTime;  // signals whether new interp calc required

}

void CalInterp::interpFreq(const Vector<Double>& freq) {

  if (verbose_) cout << "CalInterp::interpFreq()" << endl;

  // Only if more than one cal channel is freq interp (potentially) necessary 
  if (nChan() > 1) {

    // setup freq interp info (~no-op if already done)
    initFreqInterp(freq);

    // if all exact freqs, just reference time interp result:
    if ( allEQ(ef(),true) ) {

      // un-reference r
      r.resize();

    } else {
      // do non-trivial freq interpolation

      // ensure correct info available from time interpolation
      calcAPC();

      // update coeffs
      updFreqCoeff();

      // do freq interp calc
      interpFreqCalc();
       
    }

  }

}

Bool CalInterp::findSlot(const Double& time) {

  if (verbose_) cout << "CalInterp::findSlot()" << endl;

  Int slot(-1);

  Bool newSlot(false);               // Assume no change

  // If only one slot, we've found it
  if (nTime()==1) {
    slot=0;
    exactTime_=true;

  // More than one slot, find the right one:
  } else {

    Vector<Double> timelist(csTimes());

    if (exactTime_) newSlot=true;

    // Behave as nearest outside absolute range of available calibration   
    //   to avoid wild extrapolation, else search for the correct soln slot
    if (time<timelist(0)) {
      // Before first timestamp, use first slot as nearest one
      slot=0;
      exactTime_=true;
    }
    else if (time>timelist(nTime()-1)) {
      // After last timestamp, use last slot as nearest one
      slot=nTime()-1;
      exactTime_=true;
    }
    else
      // Find index in timelist where time would be:
      slot=binarySearch(exactTime_,timelist,time,nTime(),0);

    //    cout << "time = " << time << "  slot = " << slot << " nTime() = " << nTime() << endl;
    // If not already an exact match...
    if ( !exactTime_ ) {

      // identify this time via index just prior
      if (slot>0) slot--;

      // If nearest, fine-tune slot to actual nearest:
      if ( timeType()=="nearest" ) {
	exactTime_=true;   // Nearest behaves like exact match
	if (slot!=nTime()-1 && 
	    (timelist(slot+1)-time) < (time-timelist(slot)) )
	  slot++;
      } else {
	// linear modes require one later slot
	if (slot==nTime()-1) slot--;
      }
    }

  }
  newSlot = (slot!=currSlot());
  if (newSlot) {
    currSlot_(currSpw_)=slot;
  }

  return newSlot;

}

void CalInterp::updTimeCoeff() {

  if (verbose_) cout << "CalInterp::updTimeCoeff()" << endl;

  if ( currSlot() != lastlo() ) {
    lastlo()=currSlot();

    // Resize Coefficient arrays 
    IPosition ip4s(4,2,nPar(),nChan(),nElem());
    IPosition ip3s(3,nPar(),nChan(),nElem());

    tAC().resize(ip4s);
    tOk().resize(); // ensure not referencing csParOK!
    tOk().resize(ip3s);

    if ( timeType()=="linear") 
      tPC().resize(ip4s);
    else if (timeType()=="aipslin") 
      tCC().resize(ip4s);
    
    // Time ref/step for this interval
    t0()=csTimes()(currSlot());
    tS()=csTimes()(currSlot()+1)-t0();
    
    // For indexing parameter cache
    IPosition lo(4,0,0,0,currSlot()), hi(4,0,0,0,currSlot()+1);
    IPosition ref(4,0,0,0,0), slope(4,1,0,0,0);

    for (Int ielem=0;ielem<nElem();ielem++) {
      lo(2)=hi(2)=ref(3)=slope(3)=ielem;
      for (Int ichan=0;ichan<nChan();ichan++) {
	lo(1)=hi(1)=ref(2)=slope(2)=ichan;
	for (Int ipar=0;ipar<nPar();ipar++) {
	  lo(0)=hi(0)=ref(1)=slope(1)=ipar;

	  tOk()(ipar,ichan,ielem) = (csParOK()(lo) && csParOK()(hi));
	  
	  if (tOk()(ipar,ichan,ielem)) {
	    // Intercept
	    tAC()(ref) = abs(csPar()(lo));
	    tAC()(slope) = abs(csPar()(hi)) - tAC()(ref);
	    
	    if (timeType()=="linear") {
	      tPC()(ref) = arg(csPar()(lo));
	      
	      // Slope
	      Float pslope = arg(csPar()(hi)) - tPC()(ref);
	      // Catch simple phase wraps
	      if (pslope > C::pi)
		pslope-=(2*C::pi);
	      else if (pslope < -C::pi)
		pslope+=(2*C::pi);
	      tPC()(slope) = pslope;
	      
	    } else if (timeType()=="aipslin") {
	      tCC()(ref) = csPar()(lo);
	      tCC()(slope) = csPar()(hi)-tCC()(ref);
	      
	    }
	    
	  } else {
	    tAC()(ref)=1.0;
	    tAC()(slope)=0.0;
	    if (timeType()=="linear") {
	      tPC()(ref)=0.0;
	      tPC()(slope)=0.0;
	    } else if (timeType()=="aipslin") {
	      tCC()(ref)=Complex(1.0,0.0);
	      tCC()(slope)=Complex(0.0,0.0);
	    }
	  }
	}
      }
    }
  }

}


void CalInterp::interpTimeCalc(const Double& time) {

  if (verbose_) cout << "CalInterp::interpTimeCalc()" << endl;

  // TODO:
  //  a. Use matrix math instead of loops?  (tOk() usage?)

  // Fractional time interval for this timestamp
  Float dt( Float( (time-t0())/tS() ) );

  // Ensure intermediate results cache is properly sized and ref'd
  if (tA_.nelements()==0) {
    tA_.resize(nPar(),nChan(),nElem());
    a.reference(tA_);
    ok.reference(tOk());
    if (timeType()=="linear") {
      tP_.resize(nPar(),nChan(),nElem());
      p.reference(tP_);
      c.resize(); 
    }
    else if (timeType()=="aipslin") {
      tC_.resize(nPar(),nChan(),nElem());
      c.reference(tC_);
      p.resize();
    }
  }
  
  IPosition ref(4,0,0,0,0),slope(4,1,0,0,0);
  for (Int ielem=ref(3)=slope(3)=0; 
       ielem<nElem();
       ielem++,ref(3)++,slope(3)++) {
    for (Int ichan=ref(2)=slope(2)=0;
	 ichan<nChan();
	 ichan++,ref(2)++,slope(2)++) {
      for (Int ipar=ref(1)=slope(1)=0;
	   ipar<nPar();
	   ipar++,ref(1)++,slope(1)++) {
	
	if (tOk()(ipar,ichan,ielem)) {

	  tA_(ipar,ichan,ielem) = tAC()(ref) + tAC()(slope)*dt;
	  if (timeType()=="linear") {
	    tP_(ipar,ichan,ielem) = tPC()(ref) + tPC()(slope)*dt;
	  } else if (timeType()=="aipslin") {
	    Complex tCtmp(tCC()(ref) + tCC()(slope)*dt);
	    Float Amp(abs(tCtmp));
	    if (Amp>0.0)
	      tC_(ipar,ichan,ielem) = tCtmp/abs(tCtmp);
	    else
	      tC_(ipar,ichan,ielem) = Complex(1.0);
	  }
	} 
	else {
	  tA_(ipar,ichan,ielem) = 1.0;
	  if (timeType()=="linear") {
	    tP_(ipar,ichan,ielem) = 0.0;
	  } else if (timeType()=="aipslin") {
	    tC_(ipar,ichan,ielem) = Complex(1.0,0.0);
	  }
	}	// tOk()
      } // ipar
    } // ichan
  } // ielem
  
  if (verbose_) {
    cout << "tA_ = " << tA_.nonDegenerate() << endl;
    if (timeType()=="linear") 
      cout << "tP_ = " << tP_.nonDegenerate() << endl;
    else if (timeType()=="aipslin")
      cout << "tC_ = " << tC_.nonDegenerate() << endl;
  }

}


void CalInterp::initFreqInterp(const Vector<Double> freq) {

  if (verbose_) cout << "CalInterp::initFreqInterp()" << endl;

  // Initialize freq interpolation if
  //  a. not yet initialized (first time thru), or,
  //  b. # of requested frequencies not equal to previous, or,
  //  (c. # of requested freqs same, but freqs different --- NYI)


  if (nFreq() != Int(freq.nelements()) ||
      !allEQ(freq,datFreq()) )
    finit()=false;

  if (!finit()) {

    // Remember number of requested frequencies
    nFreq() = freq.nelements();
    
    // Remember the frequencies:
    datFreq() = freq;

    ch0().resize(nFreq()); ch0()=-1;
    ef().resize(nFreq());  ef()=false;

    if (nChan()==1) {
      // one-to-many case (e.g., G)
      //  need not support non-trivial freq interpolation
      ch0()=0;
      ef()=true;

    } else {
      // many-to-many case (e.g., B)
      //  support non-trivial freq interpolation if some non-exact freqs

      // Fill ref ch indices for each input freq:
      Int ichan(0);
      for (Int i=0;i<nFreq();i++) {

	ichan = binarySearch(ef()(i),csFreq(),freq(i),nChan(),0);
	
	// interp-type adjustments:
	if (!ef()(i)) {
	  if ( freqType()=="nearest" ) {
	    // refer to true nearest, behave as exact match
	    ef()(i)=true;
	    if ( ichan==nChan() || 
		 (ichan>0 && 
		  abs(freq(i)-csFreq()(ichan-1))<abs(csFreq()(ichan)-freq(i))) )
	      ichan--;
	  } else {
	    // refer to index at low and of pair that brackets this freq, within bounds
	    if (ichan>0) ichan--;
	    if (ichan==nChan()-1) ichan--;
	  }
	}
	ch0()(i)=ichan;

      }
      
      // Fill in frac freq info if some non-exact freqs
      if ( !allEQ(ef(),true) ) {
	
	df().resize(nFreq());  df()=0.0;
	fdf().resize(nFreq()); fdf()=0.0;

	// Fill in frac freq info
	for (Int i=0;i<nFreq()-1;i++) {
	  if (!ef()(i)) {
	    df()(i) = freq(i)-csFreq()(ch0()(i));
	    fdf()(i) = df()(i)/abs( csFreq()(ch0()(i)+1)-csFreq()(ch0()(i)) );
	  }
	}
      }
    }

  }

}


void CalInterp::calcAPC() {

  if (verbose_) cout << "CalInterp::calcAPC()" << endl;

  // re-package interpTimeCalc result for updFreqCoeff, as necessary

  if (freqType()=="linear") {
    if (timeType()=="nearest") {
      // need a/p from r
      tA_.resize(nPar(),nChan(),nElem());
      tA_ = amplitude(r);
      tP_.resize(nPar(),nChan(),nElem());
      tP_ = phase(r);
      r.resize(); // drop reference to nearest cached solution
    }
    else if (timeType()=="aipslin") {
      // a ok, need p
      tP_.resize(nPar(),nChan(),nElem());
      tP_ = phase(tC_);
    } 
  } else if (freqType()=="aipslin") {
    if (timeType()=="nearest") {
      // need a/c
      tA_.resize(nPar(),nChan(),nElem());
      tA_ = amplitude(r);
      tC_.resize(nPar(),nChan(),nElem());
      tC_ = r;
      Array<Float> rp,ip;
      rPart(tC_,rp);
      iPart(tC_,ip);

      //  NEED TO CHECK for tA_=0 here!      

      rp/=tA_;
      ip/=tA_;

      r.resize(); // drop reference to nearest cached solution
    }
    else if (timeType()=="linear") {
      // a ok, need c
      tC_.resize(nPar(),nChan(),nElem());
      Array<Float> rp,ip;
      rPart(tC_,rp);
      iPart(tC_,ip);
      rp = cos(tP_);
      ip = sin(tP_);
    } 
  }    

}

void CalInterp::updFreqCoeff() {

  if (verbose_) cout << "CalInterp::updFreqCoeff()" << endl;

  // Resize Coefficient arrays (no-op if already correct size)
  ip4d()(2)=ip2d()(0)=nFreq();
  fAC().resize(ip4d());
  fOk().resize(ip3d());
  if ( timeType()=="linear") 
    fPC().resize(ip4d());
  else if (timeType()=="aipslin") 
    fCC().resize(ip4d());

  // For indexing parameter cache
  IPosition lo(3,0,0,0), hi(3,0,0,0);
  IPosition ref(4,0,0,0,0), slope(4,1,0,0,0);
  for (Int ielem=0;ielem<nElem();ielem++) {
    lo(2)=hi(2)=ref(3)=slope(3)=ielem;
    for (Int ichan=0;ichan<nChan()-1;ichan++) {
      lo(1)=hi(1)=ref(2)=slope(2)=ichan;
      
      if (!ef()(ichan)) {
	for (Int ipar=0;ipar<nPar();ipar++) {
	  lo(0)=hi(0)=ref(1)=slope(1)=ipar;

	  fOk()(ipar,ichan,ielem) = tOk()(ipar,ichan,ielem) && 
	                            tOk()(ipar,ichan+1,ielem);
	  if (fOk()(ipar,ichan,ielem)) {
	    
	    // Intercept
	    fAC()(ref) = tA_(lo);
	    fAC()(slope) = tA_(hi) - tA_(lo);
	    
	    if (freqType()=="linear") {
	      fPC()(ref) = tP_(lo);
	      
	      // Slope
	      Float pslope = tP_(hi) - tP_(lo);
	      // Catch/remove simple phase wraps
	      if (pslope > C::pi)
		pslope-=(2*C::pi);
	      else if (pslope < -C::pi)
		pslope+=(2*C::pi);
	      fPC()(slope) = pslope;
	      
	    } else if (freqType()=="aipslin") {
	      fCC()(ref) = tC_(lo);
	      fCC()(slope) = tC_(hi)-tC_(lo);
	      
	    }
	  } else {
	    fAC()(ref)=1.0;
	    fAC()(slope)=0.0;
	    if (timeType()=="linear") {
	      fPC()(ref)=0.0;
	      fPC()(slope)=0.0;
	    } else if (timeType()=="aipslin") {
	      fCC()(ref)=Complex(1.0,0.0);
	      fCC()(slope)=Complex(0.0,0.0);
	    }
	  }   // fOk()
	}   // ipar
      }   // !ef()(ichan)
    } // ichan
  } // ielem

}


void CalInterp::interpFreqCalc() {

  if (verbose_) cout << "CalInterp::interpFreqCalc()" << endl;

  // TODO:
  //  a. Use matrix math instead of loops?  (fOk(), ef() usage?)

  fA_.resize(nPar(),nFreq(),nElem());
  a.reference(fA_);
  ok.reference(fOk());
  if (freqType()=="linear") {
    fP_.resize(nPar(),nFreq(),nElem());
    p.reference(fP_);
    c.resize();
  }
  else if (freqType()=="aipslin") {
    fC_.resize(nPar(),nFreq(),nElem());
    c.reference(fC_);
    p.resize();
  }

  IPosition ref(4,0,0,0,0),slope(4,1,0,0,0);
  for (Int ielem=ref(3)=slope(3)=0;
       ielem<nElem();
       ielem++,ref(3)++,slope(3)++) {
    for (Int ichan=ref(2)=slope(2)=0;
	 ichan<nFreq();
	 ichan++,ref(2)++,slope(2)++) {
      // if non-exact channel and otherwise ok, calc interp per par
      if ( !ef()(ichan) ) {
	for (Int ipar=ref(1)=slope(1)=0;
	     ipar<nPar();
	     ipar++,ref(1)++,ref(2)++) {
	  if (fOk()(ipar,ichan,ielem) ) {
	    fA_(ipar,ichan,ielem) = fAC()(ref) + fAC()(slope)*fdf()(ichan);
	    if ( freqType()=="linear") {
	      fP_(ipar,ichan,ielem) = fPC()(ref) + fPC()(slope)*fdf()(ichan);
	    } else if ( freqType()=="aipslin") {
	      fC_(ipar,ichan,ielem) = fCC()(ref) + fCC()(slope)*fdf()(ichan);
	    }
	  } 
	  else {
	    // copy from time interp
	    fA_(ipar,ichan,ielem) = tA_(ipar,ch0()(ichan),ielem);
	    if ( freqType()=="linear") {
	      fP_(ipar,ichan,ielem) = tP_(ipar,ch0()(ichan),ielem);
	    } else if ( freqType()=="aipslin") {
	      fC_(ipar,ichan,ielem) = tC_(ipar,ch0()(ichan),ielem);
	    }
	  } // fOk()
	} // ipar
      } // !ef() 
    } // ichan
  } // ielem

}
  
void CalInterp::finalize() {

  if (verbose_) cout << "CalInterp::finalize()" << endl;

  if (!exactTime()) {
    if (p.nelements() > 0 ) {
      // if we did phase interp, shape and set r/i parts of r_
      //      cout << "CalInterp::finalize(): pppppp" << endl;
      c.resize(p.shape());
      
      Array<Float> rp,ip;
      rPart(c,rp);
      iPart(c,ip);
      
      rp = a*cos(p);
      ip = a*sin(p);

    } 
    else if (c.nelements() > 0) {
      
      // just reference c
      Array<Float> rp,ip;
      rPart(c,rp);
      iPart(c,ip);
      rp*=a;
      ip*=a;
      
    }
    r_.reference(c);
  }

  // make public by reference
  r.reference(r_);

  ok.reference(tOk());

}


// Deflate in-focus time interpolation coeff cache
void CalInterp::deflTimeC() {

  for (Int ispw=0; ispw<nSpw(); ispw++) {
    if (tAC_[ispw]) delete tAC_[ispw];
    if (tOk_[ispw]) delete tOk_[ispw];
    if (tPC_[ispw]) delete tPC_[ispw];
    if (tCC_[ispw]) delete tCC_[ispw];
    
    tAC_[ispw]=tPC_[ispw]= NULL;
    tOk_[ispw]=NULL;
    tCC_[ispw]=NULL;
  }    
}

// Deflate in-focus freq interpolation coefficient cache
void CalInterp::deflFreqC() {

  for (Int ispw=0;ispw<nSpw();ispw++) {
    if (fAC_[ispw]) delete fAC_[ispw];
    if (fOk_[ispw]) delete fOk_[ispw];
    if (fPC_[ispw]) delete fPC_[ispw];
    if (fCC_[ispw]) delete fCC_[ispw];
    
    fAC_[ispw]=fPC_[ispw]=NULL;
    fOk_[ispw]=NULL;
    fCC_[ispw]=NULL;
  }
}

// Deflate in-focus freq interpolation abscissa data
void CalInterp::deflFreqA() {

  for (Int ispw=0;ispw<nSpw();ispw++) {
    if ( ch0_[ispw] ) delete ch0_[ispw];
    if ( ef_[ispw] )  delete ef_[ispw];
    if ( df_[ispw] )  delete df_[ispw];
    if ( fdf_[ispw] ) delete fdf_[ispw];
    
    ch0_[ispw] = NULL;
    ef_[ispw] = NULL;
    df_[ispw] = fdf_[ispw] = NULL;
  }
}

void CalInterp::asFloatArr(const Array<Complex>& in, Array<Float>& out) {
  IPosition ip1(in.shape());
  IPosition ip2(1,2);
  ip2.append(ip1);
  out.takeStorage(ip2,(Float*)(in.data()),SHARE);
}

void CalInterp::part(const Array<Complex>& c, 
		     const Int& which,
		     Array<Float>& f) {

  IPosition ip1(c.shape());
  Array<Float> asfl;
  asFloatArr(c,asfl);
  IPosition ip2(asfl.shape());
  IPosition blc(ip2), trc(ip2);
  blc=0; trc-=1;
  blc(0)=trc(0)=which;
  f.reference(asfl(blc,trc).reform(ip1));
}

void CalInterp::setSpwOK() {

  for (Int i=0;i<nSpw();i++)
    if (spwMap(i)>-1)
      spwOK_(i) = cs_->spwOK()(spwMap(i));
    else
      spwOK_(i) = cs_->spwOK()(i);

  //  cout << "CalInterp::spwOK() (spwmap) = " << boolalpha << spwOK() << endl;

}




} //# NAMESPACE CASA - END

