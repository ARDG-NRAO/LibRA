//# RIorAParray.cc: Implementation of RI/AP on-demand converter
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

#include <synthesis/CalTables/RIorAParray.h>

#include <casa/aips.h>
#include <casa/BasicSL/Constants.h>
#include <casa/Arrays/Array.h>
#include <casacore/casa/IO/ArrayIO.h>
#include <casa/Arrays/ArrayIter.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Logging/LogMessage.h>
#include <casa/Logging/LogSink.h>

#define RIORAPVERB false

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN                                                   


// Construct empty
RIorAPArray::RIorAPArray() :
  c_ok_(false),
  f_ok_(false),
  phaseTracked_(false),
  c_(),
  f_()
{}

// Construct from external Complex Array
RIorAPArray::RIorAPArray(const Array<Complex>& c) :
  c_ok_(false),
  f_ok_(false),
  phaseTracked_(false),
  c_(),
  f_()
{
  if (RIORAPVERB) cout << "ctor(A<Complex>))" << endl;

  // Delegate to setData
  this->setData(c);
}

// Construct from external Float Array
RIorAPArray::RIorAPArray(const Array<Float>& f) :
  c_ok_(false),
  f_ok_(false),
  phaseTracked_(false),
  c_(),
  f_()
{
  if (RIORAPVERB) cout << "ctor(A<Float>))" << endl;

  // Delegate to setData
  setData(f);
}

// Destructor
RIorAPArray::~RIorAPArray() {};


void RIorAPArray::setData(const Array<Complex>& c) {

  // Discard any existing float part; will be created on-demand, if nec.
  f_.resize();
  f_ok_=false;
  // Reference incoming data array
  if (c.ndim()==1) {
    IPosition ip=c.shape();
    ip.prepend(IPosition(1,1));
    c_.reference(c.reform(ip));
  }
  else
    c_.reference(c);
  // Complex version now ok
  c_ok_=true;
}

void RIorAPArray::setData(const Array<Float>& f) {

  // Discard any existing complex part; will be created on-demand, if nec.
  c_.resize();
  c_ok_=false;

  // Insist that incoming Float Array has ndim>1
  if (f.ndim()<2)
    throw(AipsError("RIorAPArray: Input Float Array must be at least 2D."));

  // Reference incoming data array
  f_.reference(f);
  f_ok_=true;
}

// State
void RIorAPArray::state(Bool verbose) {

  cout << boolalpha;
  cout << "--state--" << endl;
  cout << "f_: ok=" << f_ok_ << " &=" << f_.data() << " sh=" << f_.shape() << " nrefs=" << f_.nrefs() << endl;
  cout << "c_: ok=" << c_ok_ << " &=" << c_.data() << " sh=" << c_.shape() << " nrefs=" << c_.nrefs() << endl;

  if (verbose) {
    cout.precision(10);
    cout << "f_ = " << f_ << endl;
    cout << "c_ = " << c_ << endl;
  }
  cout << "---------" << endl;

}


// Render Complex version (calc from Float, if necessary)
Array<Complex> RIorAPArray::c() {
  if (!c_ok_) { // not already calculated
    resizec_();
    calc_c();    // calc internally
  }
  return c_;  // return the array
}


// Render Float version (calc from Complex, if necessary)
Array<Float> RIorAPArray::f(Bool trackphase) {
  // form it, if not already calculated or phase needs tracking
  //  TBD optimize already-calc'd/needs phasetracked case
  if (!f_ok_ || (trackphase && !phaseTracked_)) { 
    resizef_();
    calc_f(trackphase);
  }
  return f_;                 // return the array
}

void RIorAPArray::resizec_() {
  if (RIORAPVERB) cout << "resizec_()" << endl;
  IPosition cip(f_.shape());
  cip(0)/=2;  // First axis float->complex (half as long)
  c_.resize(cip);
}
void RIorAPArray::resizef_() {
  if (RIORAPVERB) cout << "resizef_()" << endl;
  IPosition fip(c_.shape());
  fip(0)*=2;  // First axis complex->float (twice as long)
  f_.resize(fip);

  // Ensure that at least 2 axes...
  if (fip.size()<2)
    throw(AipsError("RIorAPArray: Internal Float array, f_, must have ndim>1"));
}

// Calc Complex version from Float info
//   (assumes c_ already correct size)
void RIorAPArray::calc_c() {

  if (RIORAPVERB) cout << "calc_c()" << endl;
  
  if (!f_ok_) 
    throw(AipsError("RIorAParray::f(): Can't calculate complex version from absent float version."));

  Int ndim=f_.ndim();
  Array<Float> amp;
  Array<Float> ph;
  IPosition blc(ndim,0),trc(f_.endPosition()),stp(ndim,1);
  stp(0)=2;  // by 2 in first axis
  amp=f_(blc,trc,stp);
  blc(0)=1;   // phase is 2nd value on first axis
  ph=f_(blc,trc,stp);

  // Form Float array with real/imag parts (not amp/phase)
  Array<Float> ftmp(f_.shape());
  blc(0)=0;
  ftmp(blc,trc,stp)=amp*cos(ph);
  blc(0)=1;
  ftmp(blc,trc,stp)=amp*sin(ph);

  // Convert Float R/I array to Complex array
  //  c_=RealToComplex(ftmp);
  RealToComplex(c_,ftmp);
  c_ok_=true;

}

// Calc Float version from Complex info
//   (assumes f_ already correct size)
void RIorAPArray::calc_f(Bool trackphase) {

  if (RIORAPVERB) cout << "calc_f(" << boolalpha << trackphase << ")" << endl;

  if (!c_ok_) 
    throw(AipsError("RIorAParray::f(): Can't calculate float version from absent complex version."));

  Int ndim=f_.ndim();
  IPosition blc(ndim,0),trc(f_.endPosition()),stp(ndim,1);
  stp(0)=2;  // by 2 in first axis
  Array<Float> amp(f_(blc,trc,stp));
  blc(0)=1;   // phase is 2nd value on first axis
  Array<Float> ph(f_(blc,trc,stp));

  amp=amplitude(c_).reform(amp.shape());
  ph=phase(c_).reform(amp.shape());
  f_ok_=true;

  phaseTracked_=false;

  if (trackphase) 
    trackPhase(ph);

}

// Track phase _on_last_axis_
void RIorAPArray::trackPhase(Array<Float>& ph) {

  if (RIORAPVERB) cout << "trackPhase()" << endl;

  IPosition phsh(ph.shape());
  ArrayIterator<Float> phiter(ph,IPosition(1,ph.ndim()-1));
  Vector<Float> ph1;
  while (!phiter.pastEnd()) {
    ph1.reference(phiter.array());
    for (uInt j=1;j<ph1.nelements();++j) {
      if (ph1(j)>ph1(j-1)) 
	while (ph1(j)>(ph1(j-1)+C::pi)) ph1(j)-=C::_2pi;
      if (ph1(j)<ph1(j-1)) 
	while (ph1(j)<(ph1(j-1)-C::pi)) ph1(j)+=C::_2pi;
    }
    phiter.next();
  }

  phaseTracked_=true;

}



/*

****TBD: Handle filling supplied arrays?  (to avoid copies)


// Render Complex version onto supplied Array (calc from Float, if necessary)
void RIorAPArray::c(Array<Complex>& c) {

  
  if (c_ok_) { // Already calculated internally
    if (c.conform(c_))
      c=c_;
  }
  else {       // Do the calculation

    // f_ *must* be ok if we are going to calculate c_
    if (!f_ok_) 
      throw(AipsError("RIorAParray::c(): Can't serve complex version from absent float version."));
    
    // Resize supplied storage
    IPosition cip(f_.shape());
    cip.getLast(cip.nelements()-1);
    c.resize(cip);   // the _supplied_ array (could be external)
    if (&c_!=&c)
      c_.reference(c); // no-op if c_ and c are same array?
    // Do the calculation    
    calc_c();
  }

}

// Render Float version onto supplied Array(calc from Complex, if necessary)
void RIorAPArray::f(Array<Float>& f,Bool trackphase) {
  // c_ must be ok if we are going to calculate f_
  if (!c_ok_) 
    throw(AipsError("RIorAParray::c(): Can't serve complex version from absent float version."));

  // Resize storage
  IPosition fip(c_.shape());
  fip.prepend(IPosition(1,2));
  f.resize(fip);    // the supplied array (could be external)
  f_.reference(f);  // no-op if f_ and f are same array?

  // Do the calculation
  calc_f(trackphase);
}
*/

} //# NAMESPACE CASA - END
