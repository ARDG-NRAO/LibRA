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

#include <casacore/measures/Measures/Muvw.h>
#include <casacore/measures/Measures/MCBaseline.h>
#include <msvis/MSVis/ViImplementation2.h>
#include <synthesis/CalTables/CTIter.h>
#include <casacore/tables/Tables/ScalarColumn.h>
#include <casacore/casa/Arrays.h>
#include <casacore/casa/OS/Timer.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

//----------------------------------------------------------------------------

ROCTIter::ROCTIter(NewCalTable tab, const Block<String>& sortcol) :
  sortCols_(sortcol.begin( ),sortcol.end( )),
  singleSpw_(false),
  parentNCT_(tab),
  calCol_(NULL),
  ti_(NULL),
  inct_(NULL),
  iROCTMainCols_(NULL),
  init_epoch_(false),
  init_uvw_(false)
{
  calCol_=new ROCTColumns(tab);
  ti_=new TableIterator(tab,sortcol);
  // Attach initial accessors:
  attach();

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
  if (calCol_!=NULL) delete calCol_;
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

void ROCTIter::setCTColumns(const NewCalTable& tab) {
  // Set subtable columns from another table
  delete calCol_;
  calCol_ = new ROCTColumns(tab);
}

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

int ROCTIter::freqFrame(int spwId) const {
  int frame = calCol_->spectralWindow().measFreqRef()(spwId);
  return frame;
}

void ROCTIter::attach() {
  // Attach accessors for current iteration:
  if (iROCTMainCols_!=NULL) delete iROCTMainCols_;
  if (inct_!=NULL) delete inct_;

  inct_= new NewCalTable(ti_->table());
  iROCTMainCols_ = new ROCTMainColumns(*inct_);
}

casacore::MDirection ROCTIter::azel0(casacore::Double time) {
  if (!init_epoch_) {
    initEpoch();
  }

  try {
    updatePhaseCenter();
  } catch (const casacore::AipsError& err) {
    throw(casacore::AipsError("azel: " + err.getMesg()));
  }

  casacore::MSDerivedValues msd;
  msd.setAntennas(calCol_->antenna());
  msd.setFieldCenter(phaseCenter_);

  casacore::MDirection azel;
  vi::ViImplementation2::azel0Calculate(time, msd, azel, epoch_);
  return azel;
}

casacore::Double ROCTIter::hourang(casacore::Double time) {
  if (!init_epoch_) {
    initEpoch();
  }

  try {
    updatePhaseCenter();
  } catch (const casacore::AipsError& err) {
    throw(casacore::AipsError("hourang: " + err.getMesg()));
  }

  casacore::MSDerivedValues msd;
  msd.setAntennas(calCol_->antenna());
  msd.setFieldCenter(phaseCenter_);

  return vi::ViImplementation2::hourangCalculate(time, msd, epoch_);
}

casacore::Float ROCTIter::parang0(casacore::Double time) {
  if (!init_epoch_) {
    initEpoch();
  }

  try {
    updatePhaseCenter();
  } catch (const casacore::AipsError& err) {
    throw(casacore::AipsError("parang: " + err.getMesg()));
  }

  casacore::MSDerivedValues msd;
  msd.setAntennas(calCol_->antenna());
  msd.setFieldCenter(phaseCenter_);

  return vi::ViImplementation2::parang0Calculate(time, msd, epoch_);
}

casacore::Matrix<casacore::Double> ROCTIter::uvw() {
  casacore::Vector<casacore::Int> ant2 = antenna2();
  if (ant2(0) == -1) {
    throw(AipsError("UVW axes cannot be calculated for antenna-based calibration tables."));  
  }

  updateAntennaUVW();

  casacore::Vector<casacore::Int> ant1 = antenna1();
  auto nbaseline = ant1.size();
  casacore::Matrix<casacore::Double> uvw(3, nbaseline);

  for (uInt i = 0; i < nbaseline; ++i) {
    uvw.column(i) = antennaUVW_[ant2(i)] - antennaUVW_[ant1(i)];
  }

  return uvw;
}

void ROCTIter::updatePhaseCenter() {
  // Set MSDerivedValues phase center for field
  if ((thisTime() == 0) || (thisField() < 0)) {
    throw(AipsError("cannot calculate this value with no valid timestamp or field ID."));
  }

  phaseCenter_ = calCol_->field().phaseDirMeas(thisField(), thisTime());
}

void ROCTIter::initEpoch() {
  epoch_ = iROCTMainCols_->timeMeas()(0);
  init_epoch_ = true;
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
  // Set antennaUVW_ for current iteration
  if (!init_uvw_) {
    initUVW();
  }

  updatePhaseCenter();

  MeasFrame measFrame(refAntPos_, epoch_, phaseCenter_);

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
}


// CTIter

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
