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
//----------------------------------------------------------------------------

#include <measures/Measures/Muvw.h>
#include <measures/Measures/MCBaseline.h>
#include <msvis/MSVis/ViImplementation2.h>
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
  calCol_(nullptr),
  ti_(nullptr),
  inct_(nullptr),
  iROCTMainCols_(nullptr),
  init_uvw_(false),
  thisfield_(-1),
  lastfield_(-1),
  thistime_(0.0),
  lasttime_(0.0)
{
  ti_=new TableIterator(tab,sortcol);
  // Attach initial accessors:
  attach();

  calCol_ = new ROCTColumns(tab);
  epoch_ = calCol_->timeMeas()(0);

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
  if (calCol_!=nullptr) delete calCol_;
  if (ti_!=nullptr) delete ti_;
  if (iROCTMainCols_!=nullptr) delete iROCTMainCols_;
  if (inct_!=nullptr) delete inct_;
};

void ROCTIter::setCTColumns(const NewCalTable& tab) {
  delete calCol_;
  calCol_ = new ROCTColumns(tab);
}

void ROCTIter::reset() {
  ti_->reset();
  this->attach();

  // Save then update values
  lastfield_ = thisfield_;
  lasttime_ = thistime_;
  thisfield_ = thisField();
  thistime_ = thisTime();

  // must be done before uvw
  updatePhaseCenter();

  if (!init_uvw_) {
    // Initialize MVBaselines from antenna positions
    initUVW();
  }

  updateAntennaUVW();
}

void ROCTIter::next() { 
  // Advance the TableIterator
  ti_->next();

  // attach accessors to new iteration
  this->attach();

  if (!pastEnd()) {
    // Update phase center and uvw if needed
    // Save then update values
    lastfield_ = thisfield_;
    lasttime_ = thistime_;
    thisfield_ = thisField();
    thistime_ = thisTime();

    if ((thisfield_ != lastfield_) || (thistime_ != lasttime_)) {
      updatePhaseCenter();
      updateAntennaUVW();
    }
  }
}

void ROCTIter::updatePhaseCenter() {
  // Set MSDerivedValues phase center for field
  if ((thisfield_ == -1) || (thistime_ == 0)) {
    return;
  }

  phaseCenter_ = calCol_->field().phaseDirMeas(thisfield_, thistime_);
}

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
    return calCol_->spectralWindow().numChan()(this->thisSpw());
  else
    // more than one spw per iteration...
    throw(AipsError("Please sort by spw."));
}

void ROCTIter::chan(Vector<Int>& v) const {
  if (singleSpw_) {
    v.resize(calCol_->spectralWindow().numChan()(this->thisSpw()));
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
    calCol_->spectralWindow().chanFreq().get(this->thisSpw(),v);
  }
  else
    // more than one spw per iteration...
    throw(AipsError("Please sort by spw."));
}

casacore::MDirection ROCTIter::azel0(casacore::Double time) const {
  if (thisfield_ == -1) {
    throw(AipsError("Cannot calculate azel with no field ID."));
  }

  casacore::MSDerivedValues msd;
  msd.setAntennas(calCol_->antenna());
  msd.setFieldCenter(phaseCenter_);

  casacore::MDirection azel;
  vi::ViImplementation2::azel0Calculate(time, msd, azel, epoch_);
  return azel;
}

casacore::Double ROCTIter::hourang(casacore::Double time) const {
  if (thisfield_ == -1) {
    throw(AipsError("Cannot calculate hourang with no field ID."));
  }

  casacore::MSDerivedValues msd;
  msd.setAntennas(calCol_->antenna());
  msd.setFieldCenter(phaseCenter_);
  return vi::ViImplementation2::hourangCalculate(time, msd, epoch_);
}

casacore::Float ROCTIter::parang0(casacore::Double time) const {
  if (thisfield_ == -1) {
    throw(AipsError("Cannot calculate parang with no field ID."));
  }

  casacore::MSDerivedValues msd;
  msd.setAntennas(calCol_->antenna());
  msd.setFieldCenter(phaseCenter_);
  return vi::ViImplementation2::parang0Calculate(time, msd, epoch_);
}

casacore::Matrix<casacore::Double> ROCTIter::uvw() const {
  if (thisfield_ == -1) {
    throw(AipsError("UVW axes cannot be calculated with no field phase center."));  
  }

  casacore::Vector<casacore::Int> ant2 = antenna2();
  if (ant2(0) == -1) {
    throw(AipsError("UVW axes cannot be calculated for antenna-based calibration tables."));  
  }

  casacore::Vector<casacore::Int> ant1 = antenna1();
  auto nbaseline = ant1.size();
  casacore::Matrix<casacore::Double> uvw(3, nbaseline);

  for (uInt i = 0; i < nbaseline; ++i) {
    uvw.column(i) = antennaUVW_[ant2(i)] - antennaUVW_[ant1(i)];
  }

  return uvw;
}

void ROCTIter::initUVW() {
  // Calculate relative positions of antennas
  nAnt_ = calCol_->antenna().nrow();
  auto antPosMeas = calCol_->antenna().positionMeas();
  refAntPos_ = antPosMeas(0); // use first antenna for reference
  auto refAntPosValue = refAntPos_.getValue();

  // Set up baseline and uvw types
  MBaseline::getType(baseline_type_, MPosition::showType(refAntPos_.getRef().getType()));

  mvbaselines_.resize(nAnt_);

  for (Int ant = 0; ant < nAnt_; ++ant) {
    // MVBaselines are basically xyz Vectors, not Measures
    mvbaselines_[ant] = MVBaseline(refAntPosValue, antPosMeas(ant).getValue());    
  }

  init_uvw_ = true;
}

void ROCTIter::updateAntennaUVW() {
  // Set antennaUVW_ for current iteration when field or time changes
  if (thisfield_ == -1) {
    return; // no phase center
  }

  MEpoch epoch = iROCTMainCols_->timeMeas()(0);
  MeasFrame measFrame(refAntPos_, epoch, phaseCenter_);

  // Antenna frame
  MBaseline::Ref baselineRef(baseline_type_);
  MVBaseline mvbaseline;
  MBaseline baselineMeas;
  baselineMeas.set(mvbaseline, baselineRef);
  baselineMeas.getRefPtr()->set(measFrame);

  // Conversion engine to phasedir type
  casacore::MBaseline::Types phasedir_type;
  MBaseline::getType(phasedir_type, MDirection::showType(phaseCenter_.getRef().getType()));
  MBaseline::Ref uvwRef(phasedir_type);
  MBaseline::Convert baselineConv(baselineMeas, uvwRef);

  // WSRT convention: phase opposite to VLA (l increases toward increasing RA)
  Int obsId = thisObs();
  if (obsId < 0) obsId = 0;
  casacore::String telescope = calCol_->observation().telescopeName()(obsId);
  bool wsrtConvention = (telescope == "WSRT");

  antennaUVW_.resize(nAnt_);

  for (int i = 0; i < nAnt_; ++i) {
    baselineMeas.set(mvbaselines_[i], baselineRef);
    MBaseline baselineOutFrame = baselineConv(baselineMeas);
    MVuvw uvwOutFrame(baselineOutFrame.getValue(), phaseCenter_.getValue());

    if (wsrtConvention) {
      antennaUVW_[i] = -uvwOutFrame.getValue();
    } else {
      antennaUVW_[i] = uvwOutFrame.getValue();
    }
  }

int ROCTIter::freqFrame(int spwId) const {
  int frame = calCol_.spectralWindow().measFreqRef()(spwId);
  return frame;
}

void ROCTIter::attach() {
  // Attach accessors for current iteration:
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
