//# FringeJones.h: Declaration of fringe-fitting VisCal
//# Copyright (C) 1996,1997,2000,2001,2002,2003,2011,2016
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

#ifndef SYNTHESIS_FRINGEJONES_H
#define SYNTHESIS_FRINGEJONES_H

#include <casacore/casa/aips.h>
#include <synthesis/MeasurementComponents/StandardVisCal.h>
#include <synthesis/MeasurementComponents/SolvableVisCal.h> 
#include <synthesis/CalTables/CTTimeInterp1.h>

namespace casa { //# NAMESPACE CASA - BEGIN

// Rate-aware time interpolation engine
class CTRateAwareTimeInterp1 : public CTTimeInterp1 {

public:

  // From NewCalTable
  CTRateAwareTimeInterp1(NewCalTable& ct,
			 const casacore::String& timetype,
			 casacore::Array<casacore::Float>& result,
			 casacore::Array<casacore::Bool>& rflag);

  // Destructor
  virtual ~CTRateAwareTimeInterp1();

  // Interpolate, given timestamp; returns T if new result
  virtual casacore::Bool interpolate(casacore::Double time);

  // static factory method to make CTRateAwareTimeInterp1
  // NB: returns pointer to a _generic_ CTTimeInterp1
  static CTTimeInterp1* factory(NewCalTable& ct,
				const casacore::String& timetype,
				casacore::Array<casacore::Float>& result,
				casacore::Array<casacore::Bool>& rflag) {
    return new casa::CTRateAwareTimeInterp1(ct,timetype,result,rflag); }

private:

  // Refine time-dep phase with rate info
  void applyPhaseRate(casacore::Bool single);

  // Default ctor is unusable
  //  CTRateAwareTimeInterp1(); 

};

// A utility class that provides an API that allows clients to find
// grid indices in time and frequency from a SBDList that can include
// multiple spectral windows.
class SDBListGridManager {
public:
     casacore::Double fmin, fmax, df;
     casacore::Double tmin, tmax, dt;
     casacore::Int nt, totalChans = 0, nSPWChan;
private:
    SDBList& sdbs;
    std::set< casacore::Int > spwins;
    std::set< casacore::Double > times;
    // You can't store references in a map.
    // C++ 11 has a reference_wrapper type, but for now:
    std::map< casacore::Int, casacore::Vector<casacore::Double> const * > spwIdToFreqMap;
public:
    SDBListGridManager(SDBList& sdbs_);
    size_t nSPW() { return spwins.size();   }
    casacore::Int bigFreqGridIndex(casacore::Double f) { return round( (f - fmin)/df ); }
    casacore::Int getTimeIndex(casacore::Double t) { return round( (t - tmin)/dt ); }
    casacore::Int nChannels() { return totalChans;  }
    casacore::Int swStartIndex(casacore::Int spw);
    void checkAllGridpoints();
};


// DelayRateFFT is responsible for the two-dimensional FFT of
// visibility phases to find an estimate for deay. rate and phase
// offset parameters during fringe-fitting.
class DelayRateFFT {
    // The idiom used in KJones solvers is:
    // DelayFFT delfft1(vbga(ibuf), ptbw, refant());
    // delfft1.FFT();
    // delfft1.shift(f0[0]);
    // delfft1.searchPeak();
    // This class is designed to follow that API (without the shift).
private:
    casacore::Int refant_;
    // SBDListGridManager handles all the sizing and interpolating of
    // multiple spectral windows onto a single frequency grid.
    SDBListGridManager gm_;
    casacore::Int nPadFactor_;
    casacore::Int nt_;
    casacore::Int nPadT_;
    casacore::Int nChan_;
    //casacore::Int nSPWChan_;
    casacore::Int nPadChan_;
    casacore::Int nElem_;
    casacore::Double dt_, f0_, df_, df_all_;
    //casacore::Double t0_, t1_;
    //casacore::Double padBW_;
    casacore::Array<casacore::Complex> Vpad_;
    casacore::Array<casacore::Int> xcount_;
    casacore::Array<casacore::Float> sumw_;
    casacore::Array<casacore::Float> sumww_;
    casacore::Int nCorr_;
    // 
    casacore::Matrix<casacore::Float> param_;
    casacore::Matrix<casacore::Bool> flag_; //?
    std::map< casacore::Int, std::set<casacore::Int> > activeAntennas_;
    std::set<casacore::Int> allActiveAntennas_;
    casacore::Array<casacore::Double>& delayWindow_;
    casacore::Array<casacore::Double>& rateWindow_;
    
public:
    DelayRateFFT(SDBList& sdbs, casacore::Int refant,
                 casacore::Double reffreq,
                 casacore::Array<casacore::Double>& delayWindow_,
                 casacore::Array<casacore::Double>& rateWindow_
         );
    DelayRateFFT(casacore::Array<casacore::Complex>& data, casacore::Int nPadFactor,
                 casacore::Float f0, casacore::Float df, casacore::Float dt, SDBList& s,
                 casacore::Array<casacore::Double>& delayWindow_,
                 casacore::Array<casacore::Double>& rateWindow_
         );
    // The following are copied from KJones.h definition of DelayFFT.
    const std::map<casacore::Int, std::set<casacore::Int> >& getActiveAntennas() const
    { return activeAntennas_; }
    const std::set<casacore::Int>& getActiveAntennasCorrelation(casacore::Int icor) const
    { return activeAntennas_.find(icor)->second; }
    void removeAntennasCorrelation(casacore::Int, std::set< casacore::Int >);
    const casacore::Array<casacore::Complex>& Vpad() const { return Vpad_; }
    const casacore::Matrix<casacore::Bool>& flag() const { return flag_; }
    const casacore::Matrix<casacore::Float>& param() const { return param_; }
    casacore::Matrix<casacore::Float> delay() const;
    casacore::Matrix<casacore::Float> rate() const;
    casacore::Int refant() const { return refant_; }
    casacore::Double get_df_all() { return df_all_; }
    
    void FFT();
    std::pair<casacore::Bool, casacore::Float>  xinterp(casacore::Float alo, casacore::Float amax, casacore::Float ahi);
    void searchPeak();
    casacore::Float snr(casacore::Int icorr, casacore::Int ielem, casacore::Float delay, casacore::Float rate);

    void printActive();
}; // End of class DelayRateFFT.



// Fringe-fitting (parametrized phase) VisCal
class FringeJones : public GJones {
public:
  // Constructor
  //  TBD:  MSMetaInfoForCal-aware version; deprecate older ones
  FringeJones(VisSet& vs);
  FringeJones(casacore::String msname,casacore::Int MSnAnt,casacore::Int MSnSpw);
  FringeJones(const MSMetaInfoForCal& msmc);
  FringeJones(casacore::Int nAnt);

  virtual ~FringeJones();

  // We have casacore::Float parameters
  virtual VisCalEnum::VCParType parType() { return VisCalEnum::REAL; };

  // Return the type enum
  virtual Type type() { return VisCal::K; };

  // Return type name as string
  virtual casacore::String typeName()     { return "Fringe Jones"; };
  virtual casacore::String longTypeName() { return "Fringe Jones (parametrized phase)"; };

  // Type of Jones matrix according to nPar()
  virtual Jones::JonesType jonesType() { return Jones::Diagonal; };

  virtual bool timeDepMat() { return true; };

  // Freq dependence (delays)
  virtual casacore::Bool freqDepPar() { return false; };
  // If the following is false frequency spectrum is squashed to one point.
  virtual casacore::Bool freqDepMat() { return true; };

  // Local setApply to enforce calWt=F for delays
  virtual void setApply(const casacore::Record& apply);
  virtual void setApply();
  using GJones::setApply;
  virtual void setCallib(const casacore::Record& callib,
			 const casacore::MeasurementSet& selms);

  // Local setSolve (traps lack of refant)
  virtual void setSolve(const casacore::Record& solve);
  using GJones::setSolve;

  // Default parameter value
  virtual casacore::Complex defaultPar() { return casacore::Complex(0.0); };
  
  // FIXME: was omitted
  virtual void specify(const casacore::Record& specify);
  
  // This type is not yet accumulatable
  virtual casacore::Bool accumulatable() { return false; };

  // This type is smoothable
  virtual casacore::Bool smoothable() { return true; };

  // Delay to phase calculator
  virtual void calcAllJones();

  // Hazard a guess at parameters (unneeded here)
  //  TBD?  Needed?
  virtual void guessPar(VisBuffer& ) {};

  // FringeJones uses generic gather, but solves for itself per solution
  virtual casacore::Bool useGenericGatherForSolve() { return true; };
  virtual casacore::Bool useGenericSolveOne() { return false; }

  // Post solve tinkering
  virtual void globalPostSolveTinker();

  // Local implementation of selfSolveOne (generalized signature)
  // virtual void selfSolveOne(VisBuffGroupAcc& vbga);
  virtual void selfSolveOne(SDBList&);

  virtual void solveOneVB(const VisBuffer&);

  // SolveDataBuffer is being phased out; we no longer support it.
  // virtual void solveOneSDB(const SolveDataBuffer&);

  virtual casacore::Bool& zeroRates() { return zeroRates_; }
  virtual casacore::Bool& globalSolve() { return globalSolve_; } 
  virtual casacore::Int& maxits() { return maxits_; }
  virtual casacore::Array<casacore::Double>& delayWindow() { return delayWindow_; }
  virtual casacore::Array<casacore::Double>& rateWindow() { return rateWindow_; }
  virtual casacore::Array<casacore::Bool>& paramActive() { return paramActive_; }
  
  // Apply reference antenna
  virtual void applyRefAnt();

  virtual casacore::Int& refant() { return refant_; }
  
protected:

  // phase, delay, rate
  //  TBD:  Need to cater for parameter opt-out  (e.g., no rate solve, etc.)
  virtual casacore::Int nPar() { return 8; };

  // Jones matrix elements are NOT trivial
  virtual casacore::Bool trivialJonesElem() { return false; };

  // dJ/dp are trivial
  //  TBD: make this default in SVC?
  virtual casacore::Bool trivialDJ() { return false; };

  // Initialize trivial dJs
  //  TBD: make this default in SVC?
  virtual void initTrivDJ() {};

  // Reference frequencies
  casacore::Vector<casacore::Double> KrefFreqs_;

  
  
private:

  // Pointer to CTRateAwareTimeInterp1 factory method
  // This ensures the rates are incorporated into the time-dep interpolation
  virtual CTTIFactoryPtr cttifactoryptr() { return &CTRateAwareTimeInterp1::factory; };
  void calculateSNR(casacore::Int, DelayRateFFT);

  casacore::Int refant_; // Override
  casacore::Bool zeroRates_;
  casacore::Bool globalSolve_;
  casacore::Array<casacore::Double> delayWindow_;
  casacore::Array<casacore::Double> rateWindow_;
  casacore::Array<casacore::Bool> paramActive_;
  casacore::Int maxits_;
};


} //# NAMESPACE CASA - END

#endif
