//# CTTimeInterp1.cc: Implementation of CTTimeInterp1.h
//# Copyright (C) 2012                                     
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

#include <synthesis/CalTables/CTTimeInterp1.h>
#include <synthesis/CalTables/CTMainColumns.h>
#include <synthesis/CalTables/RIorAParray.h>
#include <casacore/casa/aips.h>
#include <casacore/casa/BasicSL/Constants.h>
#include <casacore/casa/Arrays/Array.h>
#include <casacore/casa/IO/ArrayIO.h>
#include <casacore/casa/Arrays/ArrayMath.h>
#include <casacore/casa/Logging/LogMessage.h>
#include <casacore/casa/Logging/LogSink.h>
#include <casacore/scimath/Functionals/Interpolate1D.h>
#include <casacore/scimath/Functionals/ScalarSampledFunctional.h>
#include <casacore/scimath/Functionals/ArraySampledFunctional.h>

#define CTTIMEINTERPVERB1 false

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN                                                   

// Null ctor 
//CTTimeInterp1::CTTimeInterp1() {};  

// From NewCalTable
CTTimeInterp1::CTTimeInterp1(NewCalTable& ct,
			     const String& timetype,
			     Array<Float>& result,
			     Array<Bool>& rflag) :
  ct_(ct),
  mcols_p(NULL),
  timeType_(timetype),
  currTime_(-999.0),
  currIdx_(-1),
  lastWasExact_(false),
  timeRef_(0.0),
  timelist_(),
  domain_(2,0.0),
  flaglist_(),
  tInterpolator_p(NULL),
  cfreq_(-1.0),
  cycles_(),
  result_(),
  rflag_()
{

  if (CTTIMEINTERPVERB1) cout << "CTTimeInterp1::ctor()" << endl;

  // Access to main columns  (move to NewCalTable?)
  mcols_p = new ROCTMainColumns(ct_);

  // Flags
  mcols_p->flag().getColumn(flaglist_);

  // Record referenced timelist_
  //  TBD: handle flagged times
  Vector<Double> dtime;
  mcols_p->time().getColumn(dtime);
  domain_(0)=min(dtime);
  domain_(1)=max(dtime);

  // NB: time is rendered as Vector<Float> for processing
  //     because Interpolate1D doesn't work if it is V<Double>...
  //timeRef_=86400.0*floor(dtime(0)/86400.0);
  // 2016Aug22 (gmoellen): use ~current time (not prior midnight)
  //   as timeRef_ to ~minimize loss of precision for faster sampling
  timeRef_=floor(dtime(0)-1.0f);
  dtime-=timeRef_;  // Relative to reference time
  timelist_.resize(dtime.shape());
  convertArray(timelist_,dtime);  // store referenced times as Floats

  // Create the _time_ interpolator
  //  TBD:  f(par) (because flags may be different for each par...)
  tInterpolator_p = new Interpolate1D<Float,Array<Float> >();
  setInterpType(timeType_);

  // Get fiducial frequency

  // Get cycles, if nec.
  if (timetype.contains("PD")) {
    Int spw=mcols_p->spwId()(0);
    MSSpWindowColumns spwcol(ct_.spectralWindow());
    Int nChan=spwcol.numChan()(spw);
    cfreq_=Vector<Double>(spwcol.chanFreq()(spw))(nChan/2);
    //cout << "cfreq_ = " << cfreq_ << endl;
    mcols_p->cycles(cycles_);
    //cout << "ant = " << mcols_p->antenna1()(0) << ":  cycles_ = " << cycles_ << endl;
  }

  // Reference supplied arrays for results
  //  Elsewhere, must always use strict (non-shape-changing) assignment for these!
  //  TBD: verify shapes..
  result_.reference(result);
  rflag_.reference(rflag);

}

// Destructor
CTTimeInterp1::~CTTimeInterp1() {
  if (tInterpolator_p)
    delete tInterpolator_p;
  if (mcols_p)
    delete mcols_p;
};

Bool CTTimeInterp1::interpolate(Double newtime) {

  if (CTTIMEINTERPVERB1) {cout.precision(12);cout << "CTTimeInterp1::interpolate("<<newtime<<"):" << endl;}

  // Don't work unnecessarily
  if (newtime==currTime_)
    return false;  // no change

  if (CTTIMEINTERPVERB1) cout << " newtime is new..." << endl;

  // A new time is specified, so some work may be required

  // Convert supplied time value to Float (referenced to timeRef_)
  Float fnewtime(newtime-timeRef_);

  // Establish registration in time
  Bool exact(false);
  Int newIdx(currIdx_);
  Bool newReg=findTimeRegistration(newIdx,exact,fnewtime);

  if (CTTIMEINTERPVERB1) 
    cout <<boolalpha<< " newReg="<<newReg<< " newIdx="<<newIdx<< " exact="<<exact
	 << " lastWasExact_=" << lastWasExact_ << endl;

  // Update interpolator coeffs if new registr. and not nearest
  if (newReg || (!exact && lastWasExact_)) {
    // Only bother if not 'nearest' nor exact...
    if (!timeType().contains("nearest") && !exact) { 
      if (timeType().contains("linear")) {
	ScalarSampledFunctional<Float> xf(timelist_(Slice(newIdx,2)));
	Vector<uInt> rows(2); indgen(rows); rows+=uInt(newIdx);
	Array<Float> ya(mcols_p->fparamArray("",rows));
	ArraySampledFunctional<Array<Float> > yf(ya);
	tInterpolator_p->setData(xf,yf,true);
      } else if (timeType().contains("cubic")) { // Added for CAS-10787 (16/2/2018 WK)
	Int newIdxCubic(newIdx-1);
	if (newIdxCubic < 0) {
	  newIdxCubic = 0;
	} else if (newIdxCubic > (Int)timelist_.nelements()-4) {
	  newIdxCubic = timelist_.nelements()-4;
	}
	//cout << "{newIdxCubic = " << newIdxCubic << " / " << timelist_.nelements() << "}" << flush;
	ScalarSampledFunctional<Float> xf(timelist_(Slice(newIdxCubic,4)));
	Vector<uInt> rows(4); indgen(rows); rows+=uInt(newIdxCubic);
	Array<Float> ya(mcols_p->fparamArray("",rows));
	ArraySampledFunctional<Array<Float> > yf(ya);
	tInterpolator_p->setData(xf,yf,true);
      }
    }
  }
  else
    // Escape if registration unchanged and 'nearest' or exact
    if (timeType().contains("nearest") || exact) return false;  // no change

  // Now calculate the interpolation result

  if (timeType().contains("nearest") || exact) {
    if (CTTIMEINTERPVERB1) cout << " nearest or exact" << endl;
    Cube<Float> r(mcols_p->fparamArray("",Vector<uInt>(1,newIdx)));
    result_=r.xyPlane(0);
    rflag_=flaglist_.xyPlane(newIdx);
  }
  else {
    if (CTTIMEINTERPVERB1) cout << " non-trivial non-nearest" << endl;
    // Delegate to the interpolator
    // The next line is needed to restore the given interpolation type, which has been overwritten to linear in setData() above - CAS-10787 (16/2/2018 WK)
    setInterpType(timeType());
    result_=(*tInterpolator_p)(fnewtime);
    rflag_=(flaglist_.xyPlane(newIdx) || flaglist_.xyPlane(newIdx+1));
  }

  // Now remember for next round
  currTime_=newtime;
  currIdx_=newIdx;
  lastWasExact_=exact;

  return true;
 
}

Bool CTTimeInterp1::interpolate(Double newtime, Double freq) {

  Bool newcal=this->interpolate(newtime);

  if (newcal && timeType().contains("PD"))
    applyPhaseDelay(freq);

  return newcal;

}

void CTTimeInterp1::state(Bool verbose) {

  cout << endl << "-state---------" << endl;
  cout.precision(12);
  cout << " timeType_= " << timeType_ << endl;
  cout << " ntime    = " << timelist_.nelements() << endl;
  cout << " currTime_= " << currTime_ << " (" << Float(currTime_-timeRef_) << ")" << endl;
  cout << " currIdx_ = " << currIdx_ << endl;
  cout << " timeRef_ = " << timeRef_ << endl;
  cout << " domain_  = " << domain_ << endl;
  if (verbose) {
    cout << " result_  = " << result_ << endl;
    cout << boolalpha;
    cout << " rflag_   = " << rflag_ << endl;
  }
  cout << "---------------" << endl;
}

void CTTimeInterp1::setInterpType(String strtype) {
  timeType_=strtype;
  currTime_=-999.0; // ensure force refreshed calculation
  if (strtype.contains("nearest")) {
    tInterpolator_p->setMethod(Interpolate1D<Float,Array<Float> >::nearestNeighbour);
    return;
  }
  if (strtype.contains("linear")) {
    tInterpolator_p->setMethod(Interpolate1D<Float,Array<Float> >::linear);
    return;
  }
  if (strtype.contains("cubic")) {
    tInterpolator_p->setMethod(Interpolate1D<Float,Array<Float> >::cubic);
    return;
  }
  if (strtype.contains("spline")) {
    tInterpolator_p->setMethod(Interpolate1D<Float,Array<Float> >::spline);
    return;
  }
  throw(AipsError("Unknown interp type: '"+strtype+"'!!"));
}

Bool CTTimeInterp1::findTimeRegistration(Int& idx,Bool& exact,Float newtime) {

  if (CTTIMEINTERPVERB1) cout << " CTTimeInterp1::findTimeRegistration()" << endl;

  Int ntime=timelist_.nelements();

  // If only one time in timelist, that's it, and its exact (behave as nearest)
  if (ntime==1) {
    idx=0;
    exact=true;
  } 
  else {
    // More than one element in timelist, find the right one:

    // Behave as nearest outside absolute range of available calibration   
    //   to avoid wild extrapolation, else search for the correct soln slot
    if (newtime<timelist_(0)) {
      // Before first timestamp, use first slot as nearest one
      idx=0;
      exact=true;
    }
    else if (newtime>timelist_(ntime-1)) {
      // After last timestamp, use last slot as nearest one
      idx=ntime-1;
      exact=true;
    }
    else
      // Find index in timelist where time would be:
      //  TBD: can we use last result to help this?
      idx=binarySearch(exact,timelist_,newtime,ntime,0);

    // If not (yet) an exact match...
    if ( !exact ) {

      // identify this time via index just prior
      if (idx>0) idx--;

      // If nearest, fine-tune slot to actual nearest:
      if ( timeType().contains("nearest") ) {
	//        exact=true;   // Nearest behaves like exact match
        if (idx!=(ntime-1) && 
            (timelist_(idx+1)-newtime) < (newtime-timelist_(idx)) )
          idx++;
      } else {
        // linear modes require one later slot
        if (idx==ntime-1) idx--;
      }
    }

  }
  // Return if new
  return (idx!=currIdx_);

}

void CTTimeInterp1::applyPhaseDelay(Double freq) {

  if (freq>0.0) {
    IPosition blc(2,1,0),trc(result_.shape()-1),stp(2,2,1);
    Matrix<Float> rph(result_(blc,trc,stp));
    if (cfreq_>0.0) {
      rph+=cycles_.xyPlane(currIdx_);
      rph*=Float(freq/cfreq_);
    }
  }
  else
    throw(AipsError("CTTimeInterp1::applyPhaseDelay: invalid frequency."));

}

} //# NAMESPACE CASA - END
