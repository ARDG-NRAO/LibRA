//# CTIter.cc: Implementation of CTIter.h
//# Copyright (C) 2011 
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
//# $Id: NewCalTable.cc 15602 2011-07-14 00:03:34Z tak.tsutsumi $
//----------------------------------------------------------------------------

#include <synthesis/CalTables/CTIter.h>
#include <tables/Tables/ScalarColumn.h>
#include <casa/Arrays.h>
#include <casa/OS/Timer.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

//----------------------------------------------------------------------------

ROCTIter::ROCTIter(NewCalTable tab, const Block<String>& sortcol) :
  sortCols_(sortcol.begin( ),sortcol.end( )),
  singleSpw_(false),
  parentNCT_(tab),
  calCol_(tab),
  ti_(NULL),
  inct_(NULL),
  iROCTMainCols_(NULL)
{

  ti_=new TableIterator(tab,sortcol);

  // Attach initial accessors:
  attach();

  //  cout << "CTIter sort columns (" << sortCols_.nelements() << "): " << sortCols_ << endl;

  // If SPW a sort column, then 
  singleSpw_=anyEQ(sortCols_,String("SPECTRAL_WINDOW_ID"));
  
  /*
  cout << "singleSpw_ = " << boolalpha << singleSpw_ << endl;
  cout << "inct_->nrow() = " << inct_->nrow() << endl;
  cout << "this->nrow() = " << this->nrow() << endl;
  cout << "iROCTMainCols_->spwId() = " << iROCTMainCols_->spwId().getColumn() << endl;
  cout << "iROCTMainCols_->spwId()(0) = " << iROCTMainCols_->spwId()(0) << endl;
  cout << "thisSpw() = " << this->thisSpw() << endl;

  cout << "done." << endl;
  */
};

//----------------------------------------------------------------------------

ROCTIter::~ROCTIter()
{
  if (ti_!=NULL) delete ti_;
  if (iROCTMainCols_!=NULL) delete iROCTMainCols_;
  if (inct_!=NULL) delete inct_;
};

void ROCTIter::next() { 
  // Advance the TableIterator
  ti_->next();

  // attach accessors to new iteration
  this->attach();

};

void ROCTIter::next0() { 
  // Advance the TableIterator
  ti_->next();
};

Double ROCTIter::thisTime() const { return iROCTMainCols_->time()(0); };
Vector<Double> ROCTIter::time() const { return iROCTMainCols_->time().getColumn(); };
void ROCTIter::time(Vector<Double>& v) const { iROCTMainCols_->time().getColumn(v); };

Int ROCTIter::thisField() const { return iROCTMainCols_->fieldId()(0); };
Vector<Int> ROCTIter::field() const { return iROCTMainCols_->fieldId().getColumn(); };
void ROCTIter::field(Vector<Int>& v) const { iROCTMainCols_->fieldId().getColumn(v); };

Int ROCTIter::thisSpw() const { return iROCTMainCols_->spwId()(0); };
Vector<Int> ROCTIter::spw() const { return iROCTMainCols_->spwId().getColumn(); };
void ROCTIter::spw(Vector<Int>& v) const { iROCTMainCols_->spwId().getColumn(v); };

Int ROCTIter::thisScan() const { return iROCTMainCols_->scanNo()(0); };
Vector<Int> ROCTIter::scan() const { return iROCTMainCols_->scanNo().getColumn(); };
void ROCTIter::scan(Vector<Int>& v) const { iROCTMainCols_->scanNo().getColumn(v); };

Int ROCTIter::thisObs() const { return iROCTMainCols_->obsId()(0); };
Vector<Int> ROCTIter::obs() const { return iROCTMainCols_->obsId().getColumn(); };
void ROCTIter::obs(Vector<Int>& v) const { iROCTMainCols_->obsId().getColumn(v); };

Int ROCTIter::thisAntenna1() const { return iROCTMainCols_->antenna1()(0); };
Vector<Int> ROCTIter::antenna1() const { return iROCTMainCols_->antenna1().getColumn(); };
void ROCTIter::antenna1(Vector<Int>& v) const { iROCTMainCols_->antenna1().getColumn(v); };
Int ROCTIter::thisAntenna2() const { return iROCTMainCols_->antenna2()(0); };
Vector<Int> ROCTIter::antenna2() const { return iROCTMainCols_->antenna2().getColumn(); };
void ROCTIter::antenna2(Vector<Int>& v) const { iROCTMainCols_->antenna2().getColumn(v); };

Double ROCTIter::thisInterval() const { return iROCTMainCols_->interval()(0); };
Vector<Double> ROCTIter::interval() const { return iROCTMainCols_->interval().getColumn(); };
void ROCTIter::interval(Vector<Double>& v) const { iROCTMainCols_->interval().getColumn(v); };

Cube<Complex> ROCTIter::cparam() const { return iROCTMainCols_->cparam().getColumn(); };
void ROCTIter::cparam(Cube<Complex>& c) const { iROCTMainCols_->cparam().getColumn(c); };
Cube<Float> ROCTIter::fparam() const { return iROCTMainCols_->fparam().getColumn(); };
void ROCTIter::fparam(Cube<Float>& f) const { iROCTMainCols_->fparam().getColumn(f); };

// Complex as Float
Cube<Float> ROCTIter::casfparam(String what) const { return iROCTMainCols_->fparamArray(what); };
void ROCTIter::casfparam(Cube<Float>& f,String what) const { iROCTMainCols_->fparamArray(f,what); };

Cube<Float> ROCTIter::paramErr() const { return iROCTMainCols_->paramerr().getColumn(); };
void ROCTIter::paramErr(Cube<Float>& c) const { iROCTMainCols_->paramerr().getColumn(c); };

Cube<Float> ROCTIter::snr() const { return iROCTMainCols_->snr().getColumn(); };
void ROCTIter::snr(Cube<Float>& c) const { iROCTMainCols_->snr().getColumn(c); };
Cube<Float> ROCTIter::wt() const { return iROCTMainCols_->weight().getColumn(); };
void ROCTIter::wt(Cube<Float>& c) const { iROCTMainCols_->weight().getColumn(c); };

Cube<Bool> ROCTIter::flag() const { return iROCTMainCols_->flag().getColumn(); };
void ROCTIter::flag(Cube<Bool>& c) const { iROCTMainCols_->flag().getColumn(c); };

Vector<Int> ROCTIter::chan() const {
  Vector<Int> chans;
  this->chan(chans);
  return chans;
}

Int ROCTIter::nchan() const {
  if (singleSpw_)
    return calCol_.spectralWindow().numChan()(this->thisSpw());
  else
    // more than one spw per iteration...
    throw(AipsError("Please sort by spw."));
}

void ROCTIter::chan(Vector<Int>& v) const {
  if (singleSpw_) {
    v.resize(calCol_.spectralWindow().numChan()(this->thisSpw()));
    // TBD: observe channel selection here:
    indgen(v);
  }
  else
    // more than one spw per iteration...
    throw(AipsError("Please sort by spw."));
}

Vector<Double> ROCTIter::freq() const {
  Vector<Double> freqs;
  this->freq(freqs);
  return freqs;
}

void ROCTIter::freq(Vector<Double>& v) const {
  if (singleSpw_) {
    v.resize();
    calCol_.spectralWindow().chanFreq().get(this->thisSpw(),v);
  }
  else
    // more than one spw per iteration...
    throw(AipsError("Please sort by spw."));
}

int ROCTIter::freqFrame(int spwId) const {
  int frame = calCol_.spectralWindow().measFreqRef()(spwId);
  return frame;
}

void ROCTIter::attach() {
  // Attach accessors:
  if (iROCTMainCols_!=NULL) delete iROCTMainCols_;
  if (inct_!=NULL) delete inct_;
  inct_= new NewCalTable(ti_->table());
  iROCTMainCols_ = new ROCTMainColumns(*inct_);
}


CTIter::CTIter(NewCalTable tab, const Block<String>& sortcol) :
  ROCTIter(tab,sortcol),
  irwnct_(NULL),
  iRWCTMainCols_(NULL)
{
  // Attach first iteration
  //  TBD: this unnecessarily redoes the ROCTIter attach...
  attach();
}

CTIter::~CTIter() {
  if (iRWCTMainCols_!=NULL) delete iRWCTMainCols_;
  if (irwnct_!=NULL) delete irwnct_;
}


// Set fieldid
void CTIter::setfield(Int fieldid) {
  iRWCTMainCols_->fieldId().fillColumn(fieldid); 
}

// Set scan number
void CTIter::setscan(Int scan) {
  iRWCTMainCols_->scanNo().fillColumn(scan); 
}

// Set obsid
void CTIter::setobs(Int obs) {
  iRWCTMainCols_->obsId().fillColumn(obs); 
}

// Set antenna2 (e.g., used for setting refant)
void CTIter::setantenna2(const Vector<Int>& a2) {
  iRWCTMainCols_->antenna2().putColumn(a2); 
}

void CTIter::setflag(const Cube<Bool>& fl) {
  iRWCTMainCols_->flag().putColumn(fl); 
}

void CTIter::setfparam(const Cube<Float>& f) {
  iRWCTMainCols_->fparam().putColumn(f); 
};

void CTIter::setcparam(const Cube<Complex>& c) {
  iRWCTMainCols_->cparam().putColumn(c); 
};

void CTIter::attach() {

  // Attach readonly access
  ROCTIter::attach();

  // Attach writable access
  if (iRWCTMainCols_!=NULL) delete iRWCTMainCols_;
  if (irwnct_!=NULL) delete irwnct_;
  irwnct_= new NewCalTable(this->table());
  iRWCTMainCols_ = new CTMainColumns(*irwnct_);
}



} //# NAMESPACE CASA - END
