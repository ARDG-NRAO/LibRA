//# SimpleSimVi2.cc: Rudimentary data simulator--implementation
//# Copyright (C) 1996,1997,1998,1999,2000,2001,2002,2003
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This library is free software; you can redistribute it and/or modify it
//# under the terms of the GNU Library General Public License as published by
//# the Free Software Foundation; either version 2 of the License, or (at your
//# option) any later version.
//#
//# This library is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the Implied warranty of MERCHANTABILITY or
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
//# $Id: VisibilityIterator2.h,v 19.14 2006/02/28 04:48:58 mvoronko Exp $

#include <msvis/MSVis/SimpleSimVi2.h>
#include <casacore/measures/Measures/MFrequency.h>
#include <casacore/measures/Measures/MEpoch.h>
#include <casacore/casa/Arrays.h>
#include <casacore/casa/BasicMath/Random.h>
#include <casacore/casa/Quanta/MVTime.h>
#include <casacore/ms/MeasurementSets/MSAntennaColumns.h>
#include <casacore/tables/Tables/SetupNewTab.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

namespace vi {

SimpleSimVi2Parameters::SimpleSimVi2Parameters() :
  nField_(1),
  nScan_(1),
  nSpw_(1),
  nAnt_(4),
  nCorr_(4),
  nTimePerField_(Vector<Int>(1,1)),
  nChan_(Vector<Int>(1,1)),
  date0_("2016/01/06/00:00:00."),
  dt_(1.0),
  refFreq_(Vector<Double>(1,100.0e9)),
  df_(Vector<Double>(1,1.0e6)),  // 1 MHz chans
  doNoise_(false),
  stokes_(Matrix<Float>(1,1,1.0)),
  gain_(Matrix<Float>(1,1,1.0)),
  tsys_(Matrix<Float>(1,1,1.0)),
  doNorm_(false),
  polBasis_("circ"),
  doAC_(false),
  c0_(Complex(0.0)),
  autoPol_(false),
  doParang_(false),
  spwScope_(ChunkScope),
  antennaScope_(RowScope)
{
  
  Vector<Int> nTimePerField(nTimePerField_);
  Vector<Int> nChan(nChan_);
  Vector<Double> refFreq(refFreq_);
  Vector<Double> df(df_);
  Matrix<Float> stokes(stokes_);
  Matrix<Float> gain(gain_);
  Matrix<Float> tsys(tsys_);


  // Generic initialization
  this->initialize(nTimePerField,nChan,refFreq,df,stokes,gain,tsys);

}

SimpleSimVi2Parameters::SimpleSimVi2Parameters(Int nField,Int nScan,Int nSpw,Int nAnt,Int nCorr,
					       const Vector<Int>& nTimePerField,const Vector<Int>& nChan,
					       Complex c0,
					       String polBasis,
					       Bool autoPol,Bool doParang,
					       Bool doAC) :
  nField_(nField),
  nScan_(nScan),
  nSpw_(nSpw),
  nAnt_(nAnt),
  nCorr_(nCorr),
  nTimePerField_(),
  nChan_(),
  date0_("2016/01/06/00:00:00."),   //  the rest are defaulted
  dt_(1.0),
  refFreq_(Vector<Double>(1,100.0e9)),
  df_(Vector<Double>(nSpw,1.0e6)),  // 1 MHz chans
  doNoise_(False),
  stokes_(Matrix<Float>(1,1,1.0)),
  gain_(Matrix<Float>(1,1,1.0)),
  tsys_(Matrix<Float>(1,1,1.0)),
  doNorm_(False),
  polBasis_(polBasis),
  doAC_(doAC),
  c0_(c0),
  autoPol_(autoPol),
  doParang_(doParang),
  spwScope_(ChunkScope),
  antennaScope_(RowScope)
{

  Vector<Double> refFreq(nSpw_,100.0e9);
  // Make multiple spws adjacent (not identical) in frequency
  Int uNChan=nChan.nelements();
  for (Int i=1;i<nSpw_;i++) 
    refFreq[i]=refFreq[i-1]+Double(nChan[(i-1)%uNChan]*df_[i-1]);

  // cout << "SSV::refFreq=" << refFreq << endl;

  Vector<Double> df(df_);
  Matrix<Float> stokes(stokes_);
  Matrix<Float> gain(gain_);
  Matrix<Float> tsys(tsys_);

  //  cout << "SSVP(simple): stokes = " << stokes << " " << " doParang_=" << boolalpha << doParang_ << endl;

  // Generic initialization
  this->initialize(nTimePerField,nChan,refFreq,df,stokes,gain,tsys);

}



SimpleSimVi2Parameters::SimpleSimVi2Parameters(Int nField,Int nScan,Int nSpw,Int nAnt,Int nCorr,
        const Vector<Int>& nTimePerField,const Vector<Int>& nChan,
        String date0, Double dt,
        const Vector<Double>& refFreq, const Vector<Double>& df,
        const Matrix<Float>& stokes,
        Bool doNoise,
        const Matrix<Float>& gain, const Matrix<Float>& tsys,
        Bool doNorm,
        String polBasis, Bool doAC,
        Complex c0, Bool doParang,
        MetadataScope spwScope,
        MetadataScope antennaScope) :
  nField_(nField),
  nScan_(nScan),
  nSpw_(nSpw),
  nAnt_(nAnt),
  nCorr_(nCorr),
  nTimePerField_(),
  nChan_(),
  date0_(date0),
  dt_(dt),
  refFreq_(),
  df_(),
  doNoise_(doNoise),
  stokes_(),
  gain_(),
  tsys_(),
  doNorm_(doNorm),
  polBasis_(polBasis),
  doAC_(doAC),
  c0_(c0),
  autoPol_(false),
  doParang_(doParang),
  spwScope_(spwScope),
  antennaScope_(antennaScope)
{

  // Generic initialization
  this->initialize(nTimePerField,nChan,refFreq,df,stokes,gain,tsys);

}

SimpleSimVi2Parameters::SimpleSimVi2Parameters(const SimpleSimVi2Parameters& other) {
  *this=other;
}

SimpleSimVi2Parameters& SimpleSimVi2Parameters::operator=(const SimpleSimVi2Parameters& other) {

  if (this != &other) {
    nField_=other.nField_;
    nScan_=other.nScan_;
    nSpw_=other.nSpw_;
    nAnt_=other.nAnt_;
    nCorr_=other.nCorr_;
    nTimePerField_.assign(other.nTimePerField_);      // NB: Array::assign() forces reshape
    nChan_.assign(other.nChan_);
    date0_=other.date0_;
    dt_=other.dt_;
    refFreq_.assign(other.refFreq_);
    df_.assign(other.df_);
    doNoise_=other.doNoise_;
    stokes_.assign(other.stokes_);
    gain_.assign(other.gain_);
    tsys_.assign(other.tsys_);
    doNorm_=other.doNorm_;
    polBasis_=other.polBasis_;
    doAC_=other.doAC_;
    c0_=other.c0_;
    autoPol_=other.autoPol_;
    doParang_=other.doParang_;
    spwScope_=other.spwScope_;
    antennaScope_=other.antennaScope_;
  }
  return *this;

}


SimpleSimVi2Parameters::~SimpleSimVi2Parameters() {}



void SimpleSimVi2Parameters::summary() const {

  cout << endl << "***SimpleSimVi2Parameters Summary******************" << endl;
  cout << boolalpha;
  cout << "*  nField = " << nField_ << endl;
  cout << "*  nScan  = " << nScan_ << endl;
  cout << "*  nSpw   = " << nSpw_ << endl;
  cout << "*  nAnt   = " << nAnt_ << endl;
  cout << "*  nCorr  = " << nCorr_ << endl;

  cout << "*  nTimePerField = " << nTimePerField_ << endl;
  cout << "*  nChan         = " << nChan_ << endl;

  cout << "*  date0 = " << date0_ << endl;
  cout << "*  dt    = " << dt_ << endl;

  cout << "*  refFreq = " << refFreq_ << endl;
  cout << "*  df      = " << df_ << endl;

  cout << "*  stokes = " << stokes_ << endl;

  cout << "*  doNoise = " << doNoise_ << endl;

  cout << "*  gain   = " << gain_ << endl;
  cout << "*  tsys   = " << tsys_ << endl;

  cout << "*  doNorm = " << doNorm_ << endl;

  cout << "*  polBasis = " << polBasis_ << endl;
  cout << "*  doAC     = " << doAC_ << endl;
  cout << "*  c0       = " << c0_ << endl;
  cout << "***************************************************" << endl << endl;
}

  // Return frequencies for specified spw
Vector<Double> SimpleSimVi2Parameters::freqs(Int spw) const {
  Vector<Double> f(nChan_(spw));
  indgen(f);
  f*=df_(spw);
  f+=(df_(spw)/2. + refFreq_(spw));
  return f;
}



void SimpleSimVi2Parameters::initialize(const Vector<Int>& nTimePerField,const Vector<Int>& nChan,
					const Vector<Double>& refFreq, const Vector<Double>& df,
					const Matrix<Float>& stokes, 
					const Matrix<Float>& gain, const Matrix<Float>& tsys) {

  nTimePerField_.resize(nField_);  // field-dep scan length
  if (nTimePerField.nelements()==1)
    nTimePerField_.set(nTimePerField(0));  // all to specified value
  else
    nTimePerField_=nTimePerField; // will throw if length mismatch

  nChan_.resize(nSpw_);
  if (nChan.nelements()==1)
    nChan_.set(nChan(0));  // all to specified value
  else
    nChan_=nChan; // will throw if length mismatch

  refFreq_.resize(nSpw_);
  if (refFreq.nelements()==1)
    refFreq_.set(refFreq(0));
  else
    refFreq_=refFreq;  // will throw if length mismatch

  df_.resize(nSpw_);
  if (df.nelements()==1)
    df_.set(df(0));  // all to specified value
  else
    df_=df;  // will throw if length mismatch

  stokes_.resize(4,nField_);
  if (stokes.nelements()==1) {
    stokes_.set(0.0f);                       // enforce unpolarized!
    stokes_(Slice(0),Slice())=stokes(0,0);   // only I specified

    // If requested, set Q=0.04*I, U=0.03*I 
    if (autoPol_) {
      stokes_(Slice(1),Slice())=Float(stokes(0,0)*0.04f);
      stokes_(Slice(2),Slice())=Float(stokes(0,0)*0.03f);
    }
  }
  else
    stokes_=stokes; // insist shapes match

  gain_.resize(2,nAnt_);
  if (gain.nelements()==1)
    gain_.set(gain(0,0));  // all to specified value
  else
    gain_=gain;  // will throw if shapes mismatch


  tsys_.resize(2,nAnt_);
  if (tsys.nelements()==1)
    tsys_.set(tsys(0,0));  // all to specified value
  else
    tsys_=tsys;  // will throw if shapes mismatch

  if(antennaScope_ == ChunkScope)
    SSVi2NotYetImplemented();
}


SimpleSimVi2::SimpleSimVi2 () {}

SimpleSimVi2::SimpleSimVi2 (const SimpleSimVi2Parameters& pars)

  : ViImplementation2(),
    pars_(pars),
    nChunk_(0),
    nBsln_(0),
    t0_(0.0),
    wt0_(),
    vis0_(),
    iChunk_(0),
    iSubChunk_(0),
    iRow0_(0),
    iScan_(0),
    iChunkTime0_(0),
    thisScan_(1),
    thisField_(0),
    thisSpw_(0),
    lastScan_(-1),
    lastField_(-1),
    lastSpw_(-1),
    thisTime_(0.0),
    corrdef_(4,Stokes::Undefined),
    vb_(nullptr),
    phaseCenter_(),
    feedpa_()
{
  // Derived stuff

  if(pars_.spwScope_ == ChunkScope)
    nChunk_=pars_.nScan_*pars_.nSpw_;
  else
    nChunk_=pars_.nScan_;

  nBsln_=pars_.nAnt_*(pars_.nAnt_+ (pars_.doAC_ ? 1 : -1))/2;

  // Time tbd
  //  cout << "Using 2016/01/06/00:00:00.0 as reference time." << endl;
  t0_=4958755200.0;

  // Fundamental weight value is pars_.df_*dt_
  wt0_.resize(pars_.nSpw_);
  if (pars_.doNoise_) {
    convertArray(wt0_,pars_.df_);  // Float <- Double
    wt0_*=Float(pars_.dt_);
  }
  else 
    wt0_.set(1.0);

  // Fundamental vis are just stokes combos (complex)
  const Int& nCor(pars_.nCorr_);
  vis0_.resize(nCor,pars_.nField_);
  vis0_.set(Complex(0.0));
  for (Int ifld=0;ifld<pars_.nField_;++ifld) {
    if (pars_.polBasis_=="circ") {
      vis0_(0,ifld)=Complex(pars_.stokes_(0,ifld)+pars_.stokes_(3,ifld),0.0);
      if (nCor>1) {
	vis0_(nCor-1,ifld)=Complex(pars_.stokes_(0,ifld)-pars_.stokes_(3,ifld),0.0);
	if (nCor>2) {
	  vis0_(1,ifld)=Complex(pars_.stokes_(1,ifld),      pars_.stokes_(2,ifld));
	  vis0_(2,ifld)=Complex(pars_.stokes_(1,ifld),-1.0f*pars_.stokes_(2,ifld));
	}
      }
    }
    else if (pars_.polBasis_=="lin") {
      vis0_(0,ifld)=Complex(pars_.stokes_(0,ifld)+pars_.stokes_(1,ifld),0.0);
      if (nCor>1) {
	vis0_(nCor-1,ifld)=Complex(pars_.stokes_(0,ifld)-pars_.stokes_(1,ifld),0.0);
	if (nCor>2) {
	  vis0_(1,ifld)=Complex(pars_.stokes_(2,ifld),      pars_.stokes_(3,ifld));
	  vis0_(2,ifld)=Complex(pars_.stokes_(2,ifld),-1.0f*pars_.stokes_(3,ifld));
	}
      }
    }
  }

  corrdef_.resize(nCor);
  if (pars_.polBasis_=="circ") {
    corrdef_(0)=Stokes::type("RR");
    if (nCor>1) {
      corrdef_(nCor-1)=Stokes::type("LL");
      if (nCor>2) {
	corrdef_(1)=Stokes::type("RL");
	corrdef_(2)=Stokes::type("LR");
      }
    }
  }
  else if (pars_.polBasis_=="lin") {
    corrdef_(0)=Stokes::type("XX");
    if (nCor>1) {
      corrdef_(3)=Stokes::type("YY");
      if (nCor>2) {
	corrdef_(1)=Stokes::type("XY");
	corrdef_(2)=Stokes::type("YX");
      }
    }
  }
    
  VisBufferOptions vbopt=VbWritable;
  vb_.reset(createAttachedVisBuffer(vbopt));

  generateSubtables();
}

void SimpleSimVi2::generateSubtables()
{
  // Generating Antenna Subtable
  TableDesc antennaTD = MSAntenna::requiredTableDesc();
  SetupNewTable antennaSetup("antennaSubtable", antennaTD, Table::New);
  antennaSubTable_p = Table(antennaSetup, Table::Memory, pars_.nAnt_, true);
  antennaSubTablecols_p.reset(new MSAntennaColumns(antennaSubTable_p));

  // Generating SPW Subtable
  TableDesc spwTD = MSSpectralWindow::requiredTableDesc();
  SetupNewTable spwSetup("spwSubtable", spwTD, Table::New);
  spwSubTable_p = Table(spwSetup, Table::Memory, pars_.nSpw_, true);
  spwSubTablecols_p.reset(new MSSpWindowColumns(spwSubTable_p));
  auto numChanCol = spwSubTablecols_p->numChan();
  numChanCol.putColumn(pars_.nChan_);
  auto refFreqCol = spwSubTablecols_p->refFrequency();
  refFreqCol.putColumn(pars_.refFreq_);
  auto chanFreqCol = spwSubTablecols_p->chanFreq();
  for (Int i=0; i < pars_.nSpw_; i++)
    chanFreqCol.put(i,  pars_.freqs(i));
  auto chanWidthCol = spwSubTablecols_p->chanWidth();
  for (Int i=0; i < pars_.nSpw_; i++)
    chanWidthCol.put(i, Vector<double>(pars_.nChan_(i), pars_.df_(i)));

  // Generating DD Subtable. There is only one polarizations,
  // therefore number of DDs = number of SPWs.
  TableDesc ddTD = MSDataDescription::requiredTableDesc();
  SetupNewTable ddSetup("ddSubtable", ddTD, Table::New);
  ddSubTable_p = Table(ddSetup, Table::Memory, pars_.nSpw_, true);
  ddSubTablecols_p.reset(new MSDataDescColumns(ddSubTable_p));
  auto spwCol = ddSubTablecols_p->spectralWindowId();
  for (int i=0; i < pars_.nSpw_; i++)
    spwCol.put(i,  i);

  // Generating polarization Subtable. There is only one polarizations,
  TableDesc polTD = MSPolarization::requiredTableDesc();
  SetupNewTable polSetup("polSubtable", polTD, Table::New);
  polSubTable_p = Table(polSetup, Table::Memory, 1, true);
  polSubTablecols_p.reset(new MSPolarizationColumns(polSubTable_p));

}

// Destructor
SimpleSimVi2::~SimpleSimVi2 () { /*cout << " ~SSVi2:        " << this << endl;*/ }


  //   +==================================+
  //   |                                  |
  //   | Iteration Control and Monitoring |
  //   |                                  |
  //   +==================================+

  // Methods to control chunk iterator

void SimpleSimVi2::originChunks (Bool)
{
  // Initialize global indices
  iChunk_=0;
  thisField_=0;
  if(pars_.spwScope_ == ChunkScope)
    thisSpw_=0;

  // First Scan
  thisScan_=1;

  // Initialize time
  iChunkTime0_=t0_+pars_.dt_/2.;
  thisTime_=iChunkTime0_;

  if(pars_.spwScope_ == ChunkScope)
    iRow0_=-nBsln_;
  else 
    iRow0_=-pars_.nSpw_ * nBsln_;

}


Bool SimpleSimVi2::moreChunks () const
{ 
  // if there are more chunks...
  return iChunk_<nChunk_; 
}

void SimpleSimVi2::nextChunk () 
{
  // Remember last chunk's indices
  lastScan_=thisScan_;
  lastField_=thisField_;
  if(pars_.spwScope_ == ChunkScope)
    lastSpw_=thisSpw_;

  // Increment chunk counter
  ++iChunk_;

  // If each chunk contains a unique SPW, then a new scan happens
  // each pars_.nSpw_ chunks
  // If eah chunk contains all SPWs, then a new scan happens each chunk
  if(pars_.spwScope_ == ChunkScope)
    iScan_ = iChunk_/pars_.nSpw_;
  else
    iScan_ = iChunk_;
  // 1-based
  thisScan_ = 1+ iScan_;

  // Each scan is new field
  thisField_ = iScan_%pars_.nField_;

  // Each chunk is new spw if each chunk contains a unique SPW
  if(pars_.spwScope_ == ChunkScope)
    thisSpw_ = iChunk_%pars_.nSpw_;

  // Increment chunk time if new scan 
  //  (spws have been exhausted on previous scan)
  if (thisScan_!=lastScan_) iChunkTime0_=thisTime_ + pars_.dt_;

  // Ensure subchunks initialized
  //  this->origin();

}

  // Methods to control and monitor subchunk iteration

void SimpleSimVi2::origin ()
{
  rownr_t spwSubchunkFactor = 1, antennaSubchunkFactor = 1;
  if(pars_.spwScope_ == SubchunkScope)
    spwSubchunkFactor = pars_.nSpw_;
  if(pars_.antennaScope_ == SubchunkScope)
    antennaSubchunkFactor = nBsln_;

  nSubchunk_ = spwSubchunkFactor * antennaSubchunkFactor * pars_.nTimePerField_(thisField_);

  // First subchunk this chunk
  iSubChunk_=0;

  // time is first time of the chunk
  thisTime_=iChunkTime0_;

  rownr_t spwRowFactor = 1, antennaRowFactor = 1;
  // row counter
  if(pars_.spwScope_ == RowScope)
    spwRowFactor = pars_.nSpw_;
  if(pars_.antennaScope_ == RowScope)
    antennaRowFactor = nBsln_;

  iRow0_ += spwRowFactor * antennaRowFactor;

  // Start with SPW=0 if scope is Subchunk. 
  if(pars_.spwScope_ == SubchunkScope || pars_.spwScope_ == RowScope)
    thisSpw_ = 0;

  if(pars_.antennaScope_ == SubchunkScope)
  {
      thisAntenna1_ = 0;
      thisAntenna2_ = (pars_.doAC_ ? 0 : 1);
  }

  // Keep VB sync'd
  this->configureNewSubchunk();
}

Bool SimpleSimVi2::more () const
{
  // true if still more subchunks for this scan's field
  return (iSubChunk_<nSubchunk_);
}

void SimpleSimVi2::next () {
  // Advance counter and time
  ++iSubChunk_;

  if(iSubChunk_ < nSubchunk_)
  {
    // Change SPW once all times and baselines have been served
    if(pars_.spwScope_ == SubchunkScope)
    {
      if(pars_.antennaScope_ == SubchunkScope)
        thisSpw_ = iSubChunk_ / (nBsln_ / pars_.nTimePerField_(thisField_));
      else 
        thisSpw_ = iSubChunk_ / pars_.nTimePerField_(thisField_);
    }

    if(pars_.antennaScope_ == SubchunkScope)
    {
      thisAntenna2_++;
      if(thisAntenna2_ == pars_.nAnt_)
      {
        thisAntenna1_++;
        thisAntenna2_ = (pars_.doAC_ ? thisAntenna1_ : thisAntenna1_ + 1);
      }
      // All baselines have been served, start again
      if(! (iSubChunk_ % nBsln_) )
      {
        thisAntenna1_ = 0;
        thisAntenna2_ = (pars_.doAC_ ? 0 : 1);
        thisTime_+=pars_.dt_;
      }
    }
    else
        thisTime_+=pars_.dt_;

    // Keep VB sync'd
    this->configureNewSubchunk();
  }
}

Subchunk SimpleSimVi2::getSubchunkId () const { return Subchunk(iChunk_,iSubChunk_);}
  
  // Return the time interval (in seconds) used for iteration.
  // This is not the same as the INTERVAL column.  Setting the
  // the interval requires calling origin chunks before performing
  // further iterator.
  
  // Select the channels to be returned.  Requires calling originChunks before
  // performing additional iteration.
  
void SimpleSimVi2::setFrequencySelections (const FrequencySelections &) 
{
  SSVi2NotYetImplemented()
}

Bool SimpleSimVi2::existsColumn (VisBufferComponent2 id) const 
{
  Bool result;
  switch (id){

  case VisBufferComponent2::VisibilityCubeFloat:
    result = False;
    break;

  case VisBufferComponent2::VisibilityCorrected:
  case VisBufferComponent2::VisibilityCubeCorrected:
  case VisBufferComponent2::VisibilityModel:
  case VisBufferComponent2::VisibilityCubeModel:
  case VisBufferComponent2::VisibilityObserved:
  case VisBufferComponent2::VisibilityCubeObserved:
  case VisBufferComponent2::WeightSpectrum:
  case VisBufferComponent2::SigmaSpectrum:
    result = True;
    break;

  default:
    result = True; // required columns
    break;
  }

  return result;


}

casacore::rownr_t SimpleSimVi2::nRows () const
{
  return nRows_;
};

casacore::rownr_t SimpleSimVi2::nShapes () const
{
  return nShapes_;
};

const casacore::Vector<casacore::rownr_t>& SimpleSimVi2::nRowsPerShape () const
{
  return nRowsPerShape_;
}

const casacore::Vector<casacore::Int>& SimpleSimVi2::nChannelsPerShape () const
{
  return nChannPerShape_;
}

const casacore::Vector<casacore::Int>& SimpleSimVi2::nCorrelationsPerShape () const
{
  return nCorrsPerShape_;
}

// Return the row ids as from the original root table. This is useful
// to find correspondance between a given row in this iteration to the
// original ms row
void SimpleSimVi2::getRowIds (Vector<rownr_t> &) const {
  SSVi2NotYetImplemented()
  /*
  rowids.resize(nRows());
  indgen(rowids);
  rowids+=iRow0;  // offset to current iteration
  */
}

  /*
VisBuffer2 * SimpleSimVi2::getVisBuffer (const VisibilityIterator2 * vi)
{
  ThrowIf (vb_ == nullptr, "VI Implementation has not VisBuffer.");
  vb_->associateWithVi2 (vi);
  return vb_;
}
  */

VisBuffer2 * SimpleSimVi2::getVisBuffer () const { return vb_.get(); }

  //   +=========================+
  //   |                         |
  //   | Subchunk Data Accessors |
  //   |                         |
  //   +=========================+
  
  // Return info
void SimpleSimVi2::antenna1 (Vector<Int> & ant1) const {
  ant1.resize(nRows());
  if(pars_.antennaScope_ == RowScope)
  {
    Int k=0;
    for (Int i=0;i<(pars_.doAC_ ? pars_.nAnt_ : pars_.nAnt_-1);++i) {
      for (Int j=(pars_.doAC_ ? i : i+1);j<pars_.nAnt_;++j) {
          ant1[k]=i;
          ++k;
      }
    }
  }
  else
    ant1 = thisAntenna1_;
}
void SimpleSimVi2::antenna2 (Vector<Int> & ant2) const {
  ant2.resize(nRows());
  if(pars_.antennaScope_ == RowScope)
  {
    Int k=0;
    for (Int i=0;i<(pars_.doAC_ ? pars_.nAnt_ : pars_.nAnt_-1);++i) {
      for (Int j=(pars_.doAC_ ? i : i+1);j<pars_.nAnt_;++j) {
        ant2[k]=j;
        ++k;
      }
    }
  }
  else
    ant2 = thisAntenna2_;
}

void SimpleSimVi2::corrType (Vector<Int> & corrs) const { 
  const Int& nCor(pars_.nCorr_);
  corrs.resize(nCor);
  for (Int icor=0;icor<nCor;++icor) 
    corrs[icor]=corrdef_[icor];
}

Int  SimpleSimVi2::dataDescriptionId () const { return thisSpw_; }
void SimpleSimVi2::dataDescriptionIds (Vector<Int> & ddis) const { ddis.resize(nRows()); ddis.set(thisSpw_); }
void SimpleSimVi2::exposure (Vector<Double> & expo) const { expo.resize(nRows()); expo.set(pars_.dt_); }
void SimpleSimVi2::feed1 (Vector<Int> & fd1) const { fd1.resize(nRows()); fd1.set(0); }
void SimpleSimVi2::feed2 (Vector<Int> & fd2) const { fd2.resize(nRows()); fd2.set(0); }
void SimpleSimVi2::fieldIds (Vector<Int>& fieldids) const { fieldids.resize(nRows()); fieldids.set(thisField_); }
void SimpleSimVi2::arrayIds (Vector<Int>& arrayids) const { arrayids.resize(nRows()); arrayids.set(0); }
String SimpleSimVi2::fieldName () const {return "Field"+String(thisField_); }

void SimpleSimVi2::flag (Cube<Bool> & flags) const {
  // unflagged
  Vector<Cube<Bool>> flagsVec;
  this->flag(flagsVec);
  flags = flagsVec[0];
}

void SimpleSimVi2::flag (Vector<Cube<Bool>> & flags) const {
  // unflagged
  flags.resize(nShapes());
  for (rownr_t ishape = 0 ; ishape < nShapes(); ++ishape)
  {
    flags[ishape].resize(pars_.nCorr_,nChannPerShape_[ishape], nRowsPerShape_[ishape]);
    flags[ishape].set(false);
  }
}

void SimpleSimVi2::flagRow (Vector<Bool> & rowflags) const { rowflags.resize(nRows()); rowflags.set(false); }
void SimpleSimVi2::observationId (Vector<Int> & obsids) const { obsids.resize(nRows()); obsids.set(0); }
Int SimpleSimVi2::polarizationId () const { return 0; }
void SimpleSimVi2::processorId (Vector<Int> & procids) const { procids.resize(nRows()); procids.set(0); }
void SimpleSimVi2::scan (Vector<Int> & scans) const { scans.resize(nRows()); scans.set(thisScan_); }
String SimpleSimVi2::sourceName () const { return "Source"+String(thisField_); }
void SimpleSimVi2::stateId (Vector<Int> & stateids) const { stateids.resize(nRows()); stateids.set(0); }

  Int SimpleSimVi2::polFrame () const { return 0; } // SSVi2NotYetImplemented() }

void SimpleSimVi2::spectralWindows (Vector<Int> & spws) const 
{ 
  spws.resize(nRows()); 
  if(pars_.spwScope_ == ChunkScope || pars_.spwScope_ == SubchunkScope)
    spws.set(thisSpw_); 
  else
  {
    indgen(spws);
    if(pars_.antennaScope_ == RowScope)
      spws = spws / nBsln_;
  }
}

void SimpleSimVi2::polarizationIds (Vector<Int> & polIds) const { polIds.resize(nRows()); polIds.set(0);}
void SimpleSimVi2::time (Vector<Double> & t) const { t.resize(nRows()); t.set(thisTime_); }
void SimpleSimVi2::timeCentroid (Vector<Double> & t) const { t.resize(nRows()); t.set(thisTime_); }

void SimpleSimVi2::timeInterval (Vector<Double> & ti) const { ti.resize(nRows()); ti.set(pars_.dt_); }
void SimpleSimVi2::uvw (Matrix<Double> & uvwmat) const { uvwmat.resize(3,nRows()); uvwmat.set(0); }  // zero for now

void SimpleSimVi2::visibilityCorrected (Cube<Complex> & vis) const {
  // from DATA, for now
  this->visibilityObserved(vis);
}

void SimpleSimVi2::visibilityCorrected (Vector<Cube<Complex>> & vis) const {
  // from DATA, for now
  this->visibilityObserved(vis);
}

void SimpleSimVi2::visibilityModel (Cube<Complex> & vis) const {
  Vector<Cube<Complex>> visVec;
  this->visibilityModel(visVec);
  vis = visVec[0];
}

void SimpleSimVi2::visibilityModel (Vector<Cube<Complex>> & vis) const {
  vis.resize(nShapes());
  for (rownr_t ishape = 0 ; ishape < nShapes(); ++ishape)
  {
    vis[ishape].resize(pars_.nCorr_,nChannPerShape_[ishape], nRowsPerShape_[ishape]);
    for (int icor=0;icor<pars_.nCorr_;++icor)
    {
      vis[ishape](Slice(icor),Slice(),Slice()).set(vis0_(icor,thisField_));
    }
  }
}

void SimpleSimVi2::visibilityObserved (Cube<Complex> & vis) const {
  Vector<Cube<Complex>> visVec;
  this->visibilityObserved(visVec);
  vis = visVec[0];
}

void SimpleSimVi2::visibilityObserved (Vector<Cube<Complex>> & vis) const {
  vis.resize(nShapes());
  this->visibilityModel(vis);

  Vector<Int> a1;
  Vector<Int> a2;
  this->antenna1(a1);
  this->antenna2(a2);

  Array<Complex> specvis;
  Matrix<Float> G(pars_.gain_);
  Matrix<Float> Tsys(pars_.tsys_);

  rownr_t vbRowIdx = 0;
  for (rownr_t ishape = 0 ; ishape < nShapes(); ++ishape)
  {
    rownr_t thisShapeVbRowIdx = vbRowIdx;
    if (pars_.doParang_ && pars_.nCorr_==4)
      corruptByParang(vis[ishape], thisShapeVbRowIdx);

    if (abs(pars_.c0_)>0.0) {  // Global offset for systematic solve testing
      Cube<Complex> v(vis[ishape](Slice(0,1,1),Slice(),Slice()));
      v*=(pars_.c0_);
      if (pars_.nCorr_>1) {
        v.reference(vis[ishape](Slice(pars_.nCorr_-1,1,1),Slice(),Slice()));
        v*=(conj(pars_.c0_*pars_.c0_));  // twice the phase in the opposite direction
      }
    }

    for (rownr_t irow=0;irow<nRowsPerShape_[ishape];++irow, ++vbRowIdx) {
      for (int icorr=0;icorr<pars_.nCorr_;++icorr) {
        specvis.reference(vis[ishape](Slice(icorr),Slice(),Slice(irow)));
        specvis*=sqrt( G(icorr/2,a1(vbRowIdx)) * G(icorr%2,a2(vbRowIdx)) );
        if (pars_.doNorm_)
          specvis/=sqrt( Tsys(icorr/2,a1(vbRowIdx)) * Tsys(icorr%2,a2(vbRowIdx)) );
      }
    }

    // Now add noise
    if (pars_.doNoise_)
      this->addNoise(vis[ishape], thisShapeVbRowIdx);
  }
}

void SimpleSimVi2::floatData (Cube<Float> & fcube) const {
  Vector<Cube<Float>> fCubeVec;
  this->floatData(fCubeVec);
  fcube = fCubeVec[0];
}

void SimpleSimVi2::floatData (Vector<Cube<Float>> & fcubes) const {
  fcubes.resize(nShapes());
  for (rownr_t ishape = 0 ; ishape < nShapes(); ++ishape)
  {
    fcubes[ishape].resize(pars_.nCorr_,nChannPerShape_[ishape], nRowsPerShape_[ishape]);
    // set according to stokes
    // TBD
    fcubes[ishape] = 0.0;
    // add noise
    // TBD
  }
}

IPosition SimpleSimVi2::visibilityShape () const { return IPosition(3,pars_.nCorr_,pars_.nChan_(thisSpw_),nRows()); }

void SimpleSimVi2::sigma (Matrix<Float> & sigmat) const {
  Vector<Matrix<Float>> sigVec;
  this->sigma(sigVec);
  sigmat = sigVec[0];
}

void SimpleSimVi2::sigma (Vector<Matrix<Float>> & sigmat) const {
  Vector<Matrix<Float>> wtMatVec;
  this->weight(wtMatVec);
  sigmat.resize(nShapes());
  for (rownr_t ishape = 0 ; ishape < nShapes(); ++ishape)
  {
    sigmat[ishape].resize(pars_.nCorr_, nRowsPerShape_[ishape]);
    sigmat[ishape] = (1.f/sqrt(wtMatVec[ishape]));
  }
}

void SimpleSimVi2::weight (Matrix<Float> & wtmat) const {
  Vector<Matrix<Float>> wtVec;
  this->weight(wtVec);
  wtmat = wtVec[0];
}

void SimpleSimVi2::weight (Vector<Matrix<Float>> & wtMatVec) const {
  wtMatVec.resize(nShapes());

  Vector<Int> spws;
  Vector<Int> a1;
  Vector<Int> a2;
  this->spectralWindows(spws);
  this->antenna1(a1);
  this->antenna2(a2);

  rownr_t vbRowIdx = 0;
  for (rownr_t ishape = 0 ; ishape < nShapes(); ++ishape)
  {
    wtMatVec[ishape].resize(pars_.nCorr_, nRowsPerShape_[ishape]);
    for (rownr_t irow=0;irow<nRowsPerShape_[ishape];++irow, ++vbRowIdx) {
      wtMatVec[ishape](Slice(),Slice(irow)) = wt0_(spws(vbRowIdx)); // spw-specific
      // non-ACs have twice this weight
      if(a1[vbRowIdx] != a2[vbRowIdx])
      {
        Array<Float> thiswtmat(wtMatVec[ishape](Slice(),Slice(irow)));
        thiswtmat*=Float(2.0);
      }
    }
  }
}

Bool SimpleSimVi2::weightSpectrumExists () const { return true; }
Bool SimpleSimVi2::sigmaSpectrumExists () const { return true; } 

void SimpleSimVi2::weightSpectrum (Cube<Float> & wtsp) const {
  Vector<Cube<Float>> wtspVec;
  this->weightSpectrum(wtspVec);
  wtsp = wtspVec[0];
}

void SimpleSimVi2::weightSpectrum (Vector<Cube<Float>> & wtsp) const {
  wtsp.resize(nShapes());

  Vector<Int> spws;
  Vector<Int> a1;
  Vector<Int> a2;
  this->spectralWindows(spws);
  this->antenna1(a1);
  this->antenna2(a2);

  Matrix<Float> Tsys(pars_.tsys_);

  rownr_t vbRowIdx = 0;
  for (rownr_t ishape = 0 ; ishape < nShapes(); ++ishape)
  {
    wtsp[ishape].resize(pars_.nCorr_,nChannPerShape_[ishape], nRowsPerShape_[ishape]);

    for (rownr_t irow=0;irow<nRowsPerShape_[ishape];++irow, ++vbRowIdx) {
      wtsp[ishape](Slice(),Slice(),Slice(irow)) = wt0_(spws(vbRowIdx)); // spw-specific
      if (pars_.doNoise_)
      {
        // non-ACs have twice this weight
        if(a1[vbRowIdx] != a2[vbRowIdx])
        {
          Array<Float> thiswtsp(wtsp[ishape](Slice(),Slice(),Slice(irow)));
          thiswtsp*=Float(2.0);
        }
        if (!pars_.doNorm_) {
          for (Int icorr=0;icorr<pars_.nCorr_;++icorr) {
            Array<Float> thiswt(wtsp[ishape](Slice(icorr),Slice(),Slice(irow)));
            Float c= Tsys(icorr/2,a1(vbRowIdx))*Tsys(icorr%2,a2(vbRowIdx));
            thiswt/=c;
          }
        }
      }
    }
  }
}

void SimpleSimVi2::sigmaSpectrum (Cube<Float> & sigsp) const {
  Vector<Cube<Float>> sigspVec;
  this->sigmaSpectrum(sigspVec);
  sigsp = sigspVec[0];
}

void SimpleSimVi2::sigmaSpectrum (Vector<Cube<Float>> & sigsp) const {
  Vector<Cube<Float>> wtVec;
  this->weightSpectrum(wtVec);
  sigsp.resize(nShapes());
  for (rownr_t ishape = 0 ; ishape < nShapes(); ++ishape)
  {
    sigsp[ishape].resize(pars_.nCorr_,nChannPerShape_[ishape], nRowsPerShape_[ishape]);
    sigsp[ishape] = (1.f/sqrt(wtVec[ishape]));
  }
}

const casacore::Vector<casacore::Float>& SimpleSimVi2::feed_pa (casacore::Double t) const
{ 
  feedpa_.resize(nAntennas(),0.0f);
  if (pars_.doParang_)
    feedpa_.set(0.05f*Float(t-t0_));  // 0.05 rad/s
    
  return feedpa_;
}


  //   +=========================+
  //   |                         |
  //   | Chunk and MS Level Data |
  //   |                         |
  //   +=========================+


CountedPtr<WeightScaling> SimpleSimVi2::getWeightScaling () const {
  return WeightScaling::generateUnityWeightScaling();
}

MEpoch SimpleSimVi2::getEpoch () const { SSVi2NotYetImplemented() }
  
Vector<Int> SimpleSimVi2::getCorrelations () const { 
  // The correlation indices
  Vector<Int> corrs(pars_.nCorr_);
  indgen(corrs);
  return corrs;
}

Vector<Int> SimpleSimVi2::getChannels (Double, Int, Int spw, Int) const { 
  Vector<Int> chans(pars_.nChan_(spw));
  indgen(chans);
  return chans;
}

Vector<Double> SimpleSimVi2::getFrequencies (Double, Int, Int spw, Int) const { 
  return pars_.freqs(spw);
  /*
  Vector<Double> freqs(pars_.nChan_(spw));
  indgen(freqs);
  freqs*=pars_.df_(spw);
  freqs+=(pars_.df_(spw)/2. + pars_.refFreq_(spw));
  return freqs;
  */
}

Vector<Double> SimpleSimVi2::getChanWidths (Double, Int, Int spw, Int) const { 
  Vector<Double> widths(pars_.nChan_(spw),pars_.df_(spw));
  return widths;
}

  // get back the selected spectral windows and spectral channels for
  // current ms

const SpectralWindowChannels & SimpleSimVi2::getSpectralWindowChannels (Int /*msId*/, Int /*spectralWindowId*/) const { SSVi2NotYetImplemented() }

  // Return number of antennasm spws, polids, ddids
  
Int SimpleSimVi2::nAntennas () const { return pars_.nAnt_; }
Int SimpleSimVi2::nDataDescriptionIds () const { return pars_.nSpw_; }
Int SimpleSimVi2::nPolarizationIds () const { return Int(1); }

rownr_t SimpleSimVi2::nRowsInChunk () const { SSVi2NotYetImplemented() } // number rows in current chunk
rownr_t SimpleSimVi2::nRowsViWillSweep () const { SSVi2NotYetImplemented() } // number of rows in all selected ms's

Int SimpleSimVi2::nSpectralWindows () const { return pars_.nSpw_; }

Int SimpleSimVi2::nTimes() const {
    SSVi2NotYetImplemented();
}


void SimpleSimVi2::configureNewSubchunk() {
  nShapes_ = 1;
  rownr_t spwRowFactor = 1, antennaRowFactor = 1;

  if(pars_.spwScope_ == RowScope)
    spwRowFactor = pars_.nSpw_;
  if(pars_.antennaScope_ == RowScope)
    antennaRowFactor = nBsln_;

  if(pars_.spwScope_ == RowScope)
    nShapes_ = pars_.nSpw_;

  nRows_ = spwRowFactor * antennaRowFactor;

  nRowsPerShape_.resize(nShapes_);
  nRowsPerShape_ = nRows_ / nShapes_;

  Vector<Int> spws;
  spectralWindows(spws);
  nChannPerShape_.resize(nShapes_);
  if(pars_.spwScope_ == RowScope)
    nChannPerShape_ =  pars_.nChan_;
  else
    nChannPerShape_ = pars_.nChan_(spws(0));
  nCorrsPerShape_.resize(nShapes_);
  nCorrsPerShape_ = pars_.nCorr_;

  // Poke the vb to do this
  vb_->configureNewSubchunk(0,"faked",false,
                            isNewArrayId(),isNewFieldId(),
                            isNewSpectralWindow(),getSubchunkId(),
                            nRowsPerShape_,
                            nChannPerShape_,
                            nCorrsPerShape_,
                            getCorrelations(),
                            corrdef_,corrdef_,
                            WeightScaling::generateUnityWeightScaling());
}

  // Generate noise on data
void SimpleSimVi2::addNoise(Cube<Complex>& vis, rownr_t vbRowOffset) const {

  IPosition sh3(vis.shape());

  Int64 seed(thisScan_*1000000+thisField_*100000+thisSpw_*10000 + Int(thisTime_-t0_));
  ACG r(seed);
  Normal rn(&r,0.0,1.0/wt0_(thisSpw_));

  Vector<Int> a1;
  Vector<Int> a2;
  this->antenna1(a1);
  this->antenna2(a2);

  Matrix<Float> Tsys(pars_.tsys_);
  for (Int i=0;i<sh3[2];++i) {
    Float c(1.0);
    if (a1(i+vbRowOffset)!=a2(i+vbRowOffset))
      c=1./sqrt(2.0);
    for (Int k=0;k<sh3[0];++k) {
      Float d(c);
      if (!pars_.doNorm_)
	d*=sqrt(Tsys(k/2,a1(i+vbRowOffset))*Tsys(k%2,a2(i+vbRowOffset)));
      for (Int j=0;j<sh3[1];++j) {
	vis(k,j,i)+=Complex(d)*Complex(rn(),rn());
      }
    }
  }

}

void SimpleSimVi2::corruptByParang(Cube<Complex>& vis, rownr_t vbRowOffset) const {


  // Assumes constant time in this subchunk
  Vector<Float> pa(this->feed_pa(thisTime_));
  // Row-wise antenna Ids
  Vector<Int> a1;
  Vector<Int> a2;
  this->antenna1(a1);
  this->antenna2(a2);

  Cube<Complex> v;
  if (pars_.polBasis_=="circ") {
    Matrix<Complex> P(2,pars_.nAnt_,Complex(1.0f));
    for (Int iant=0;iant<pars_.nAnt_;++iant) {
      Float& a(pa(iant));
      P(1,iant)=Complex(cos(a),sin(a));
      P(0,iant)=conj(P(1,iant));
    }
    for (Int irow=0;irow<vis.shape()(2);++irow) {
      Int& i(a1(irow+vbRowOffset)), j(a2(irow+vbRowOffset));
      for (Int icor=0;icor<pars_.nCorr_;++icor) {
	v.reference(vis(Slice(icor,1,1),Slice(),Slice(irow,1,1)));
	v*=(P(icor/2,i)*conj(P(icor%2,j)));   // cf PJones
      }
    }
  }
  else if (pars_.polBasis_=="lin") {
    Matrix<Float> P(2,pars_.nAnt_,0.0f);
    for (Int iant=0;iant<pars_.nAnt_;++iant) {
      Float& a(pa(iant));
      P(0,iant)=cos(a);
      P(1,iant)=sin(a);
    }
    Vector<Complex> v0(pars_.nCorr_),v1;
    for (Int irow=0;irow<vis.shape()(2);++irow) {
      Int& i(a1(irow+vbRowOffset)), j(a2(irow+vbRowOffset));
      Float& C(P(0,i)),S(P(1,i)),c(P(0,j)),s(P(1,j));
      for (Int ich=0;ich<pars_.nChan_(thisSpw_);++ich) {
	v1.reference(vis.xyPlane(irow).column(ich));
	v0.assign(v1);  // _deep copy_ of this irow
	v1(0)=( C*v0[0]+S*v0[2])*c    + ( C*v0[1]+S*v0[3])*s;
	v1(1)=( C*v0[0]+S*v0[2])*(-s) + ( C*v0[1]+S*v0[3])*c;
	v1(2)=(-S*v0[0]+C*v0[2])*c    + (-S*v0[1]+C*v0[3])*s;
	v1(3)=(-S*v0[0]+C*v0[2])*(-s) + (-S*v0[1]+C*v0[3])*c;
      }
    }
  }
}

const casacore::MSAntennaColumns& SimpleSimVi2::antennaSubtablecols() const
{
    return *antennaSubTablecols_p;
}

// Access to dataDescription subtable
const casacore::MSDataDescColumns& SimpleSimVi2::dataDescriptionSubtablecols() const
{
    return *ddSubTablecols_p;
}

// Access to feed subtable
const casacore::MSFeedColumns& SimpleSimVi2::feedSubtablecols() const
{
    SSVi2NotYetImplemented();
}

// Access to field subtable
const casacore::MSFieldColumns& SimpleSimVi2::fieldSubtablecols() const
{
    SSVi2NotYetImplemented();
}

// Access to flagCmd subtable
const casacore::MSFlagCmdColumns& SimpleSimVi2::flagCmdSubtablecols() const
{
    SSVi2NotYetImplemented();
}

// Access to history subtable
const casacore::MSHistoryColumns& SimpleSimVi2::historySubtablecols() const
{
    SSVi2NotYetImplemented();
}

// Access to observation subtable
const casacore::MSObservationColumns& SimpleSimVi2::observationSubtablecols() const
{
    SSVi2NotYetImplemented();
}

// Access to pointing subtable
const casacore::MSPointingColumns& SimpleSimVi2::pointingSubtablecols() const
{
    SSVi2NotYetImplemented();
}

// Access to polarization subtable
const casacore::MSPolarizationColumns& SimpleSimVi2::polarizationSubtablecols() const
{
    return *polSubTablecols_p;
}

// Access to processor subtable
const casacore::MSProcessorColumns& SimpleSimVi2::processorSubtablecols() const
{
    SSVi2NotYetImplemented();
}

// Access to spectralWindow subtable
const casacore::MSSpWindowColumns& SimpleSimVi2::spectralWindowSubtablecols() const
{
    return *spwSubTablecols_p;
}

// Access to state subtable
const casacore::MSStateColumns& SimpleSimVi2::stateSubtablecols() const
{
    SSVi2NotYetImplemented();
}

// Access to doppler subtable
const casacore::MSDopplerColumns& SimpleSimVi2::dopplerSubtablecols() const
{
    SSVi2NotYetImplemented();
}

// Access to freqOffset subtable
const casacore::MSFreqOffsetColumns& SimpleSimVi2::freqOffsetSubtablecols() const
{
    SSVi2NotYetImplemented();
}

// Access to source subtable
const casacore::MSSourceColumns& SimpleSimVi2::sourceSubtablecols() const
{
    SSVi2NotYetImplemented();
}

// Access to sysCal subtable
const casacore::MSSysCalColumns& SimpleSimVi2::sysCalSubtablecols() const
{
    SSVi2NotYetImplemented();
}

// Access to weather subtable
const casacore::MSWeatherColumns& SimpleSimVi2::weatherSubtablecols() const
{
    SSVi2NotYetImplemented();
}

SimpleSimVi2Factory::SimpleSimVi2Factory(const SimpleSimVi2Parameters& pars)
  : pars_(pars)
{}
  
SimpleSimVi2Factory::~SimpleSimVi2Factory () {}

ViImplementation2 * SimpleSimVi2Factory::createVi () const {

  ViImplementation2* vii = new SimpleSimVi2(pars_);
  return vii;
  
}

SimpleSimVi2LayerFactory::SimpleSimVi2LayerFactory(const SimpleSimVi2Parameters& pars)
  : ViiLayerFactory(),
    pars_(pars)
{
  //  cout << "&pars  = " << &pars << endl;
  //  cout << "&pars_ = " << &pars_ << endl;
}


// SimpleSimVi2-specific layer-creater
ViImplementation2 * SimpleSimVi2LayerFactory::createInstance (ViImplementation2* /*vii0*/) const {

  // No deeper layers
  //  Assert(vii0==NULL);
  
  // Make it and return it
  ViImplementation2 *vii = new SimpleSimVi2(pars_);
  return vii;
}


} // end namespace vi

} //# NAMESPACE CASA - END


