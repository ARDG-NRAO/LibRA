//# KJones.h: Declaration of delay-like (geometry) calibration types
//# Copyright (C) 1996,1997,2000,2001,2002,2003,2011
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

#ifndef SYNTHESIS_KJONES_H
#define SYNTHESIS_KJONES_H

#include <casa/aips.h>
#include <synthesis/MeasurementComponents/StandardVisCal.h>

namespace casa { //# NAMESPACE CASA - BEGIN

class DelayFFT {
public:
  // Construct from freq info and data-like casacore::Cube<casacore::Complex>
  //  (for generic testing w/out casacore::MS data)
  DelayFFT(casacore::Double f0, casacore::Double df, casacore::Double padBW, 
	   casacore::Cube<casacore::Complex> V, 
	   casacore::Cube<casacore::Float> wt=casacore::Cube<casacore::Float>());

  // Construct from freq info and shape, w/ initialization
  //   (for accumulation)
  DelayFFT(casacore::Double f0, casacore::Double df, casacore::Double padBW, 
	   casacore::Int nCorr, casacore::Int nElem, casacore::Int refant, 
	   casacore::Complex v0, casacore::Float wt=1.0);

  // Construct from a VB
  DelayFFT(const VisBuffer& vb,casacore::Double padBW,casacore::Int refant);

  // Construct from a VB2
  DelayFFT(SolveDataBuffer& sdb,casacore::Double padBW,casacore::Int refant,casacore::Int nElem);

  // Perform FFT
  void FFT();

  // Apply shift theorem
  void shift(casacore::Double f);

  // Accumulate another DelayFFT
  void add(const DelayFFT& other);

  // Accumulate another DelayFFT, with duplication (if nec) and shift
  void addWithDupAndShift(const DelayFFT& other);

  // Find peaks
  void searchPeak();

  const casacore::Cube<casacore::Complex>& Vpad() const { return Vpad_; };

  // Access to results
  const casacore::Matrix<casacore::Float>& delay() const { return delay_; };
  const casacore::Matrix<casacore::Bool>& flag() const { return flag_; };

  // Report some stateinfo
  void state();

protected:
  casacore::Double f0_, df_, padBW_;
  casacore::Int nCorr_, nPadChan_, nElem_;
  casacore::Int refant_;
  casacore::Cube<casacore::Complex> Vpad_;
  casacore::Matrix<casacore::Float> delay_;
  casacore::Matrix<casacore::Bool> flag_;

};


 class CrossDelayFFT : public DelayFFT {
public:
  // Construct from freq info and data-like casacore::Cube<casacore::Complex>
  //  (for generic testing w/out casacore::MS data)
  CrossDelayFFT(casacore::Double f0, casacore::Double df, casacore::Double padBW, 
	   casacore::Cube<casacore::Complex> V);

  // Construct from freq info and shape, w/ initialization
  //   (for accumulation)
  CrossDelayFFT(casacore::Double f0, casacore::Double df, casacore::Double padBW);

  // Construct from a SDB (VB2)
  CrossDelayFFT(SolveDataBuffer& sdb,casacore::Double padBW);

  // Report some stateinfo
  void state();

};






// Forward declarations


// K Jones provides support for SBD delays
class KJones : public GJones {


public:

  //  friend class KJonesTest;

  // Constructor
  KJones(VisSet& vs);
  KJones(casacore::String msname,casacore::Int MSnAnt,casacore::Int MSnSpw);
  KJones(const MSMetaInfoForCal& msmc);
  KJones(const casacore::Int& nAnt);

  virtual ~KJones();

  // Local setApply to enforce calWt=F for delays
  virtual void setApply(const casacore::Record& apply);
  virtual void setApply();
  using GJones::setApply;
  virtual void setCallib(const casacore::Record& callib,
			 const casacore::MeasurementSet& selms);

  // Local setSolve (traps lack of refant)
  virtual void setSolve(const casacore::Record& solve);
  using GJones::setSolve;

  // We have casacore::Float parameters
  virtual VisCalEnum::VCParType parType() { return VisCalEnum::REAL; };

  // Return the type enum
  virtual Type type() { return VisCal::K; };

  // Return type name as string
  virtual casacore::String typeName()     { return "K Jones"; };
  virtual casacore::String longTypeName() { return "K Jones (delay)"; };

  // Type of Jones matrix according to nPar()
  virtual Jones::JonesType jonesType() { return Jones::Diagonal; };

  // Freq dependence (delays)
  virtual casacore::Bool freqDepPar() { return false; };
  virtual casacore::Bool freqDepMat() { return true; };

  // Default parameter value
  virtual casacore::Complex defaultPar() { return casacore::Complex(0.0); };

  // Type-specific specify
  virtual void specify(const casacore::Record& specify);

  // This type is not yet accumulatable
  virtual casacore::Bool accumulatable() { return false; };

  // This type is smoothable
  virtual casacore::Bool smoothable() { return true; };

  // Calculate phase(chan) from delay
  virtual void calcAllJones();

  // Delay to phase calculator
  //  virtual void calcOneJones(casacore::Vector<casacore::Complex>& mat, casacore::Vector<casacore::Bool>& mOk, 
  //                            const casacore::Vector<casacore::Complex>& par, const casacore::Vector<casacore::Bool>& pOk );


  // Hazard a guess at parameters (unneeded here)
  virtual void guessPar(VisBuffer& ) {};

  // K now uses generic gather, but solves for itself per solution
  virtual casacore::Bool useGenericGatherForSolve() { return true; };
  virtual casacore::Bool useGenericSolveOne() { return false; }

  // Override G here; nothing to do for K, for now
  virtual void globalPostSolveTinker() {};

  // Local implementation of selfSolveOne (generalized signature)
  virtual void selfSolveOne(VisBuffGroupAcc& vbga);
  virtual void selfSolveOne(SDBList& sdbs);

protected:

  // K has two "real" parameters
  virtual casacore::Int nPar() { return 2; };

  // Jones matrix elements are trivial
  virtual casacore::Bool trivialJonesElem() { return false; };

  // dG/dp are trivial
  virtual casacore::Bool trivialDJ() { return false; };

  // Initialize trivial dJs
  virtual void initTrivDJ() {};

  // FFT solver for one VB
  virtual void solveOneVB(const VisBuffer& vb);
  virtual void solveOneSDB(SolveDataBuffer& sdb);

  // FFT solver for multi-VB (MBD)
  virtual void solveOneVBmbd(VisBuffGroupAcc& vbga);
  virtual void solveOneSDBmbd(SDBList& sdbs);

  // Reference frequencies
  casacore::Vector<casacore::Double> KrefFreqs_;

private:

  
};

// (sbd) K for cross-hand solve
class KcrossJones : public KJones {
public:

  // Constructor
  KcrossJones(VisSet& vs);
  KcrossJones(casacore::String msname,casacore::Int MSnAnt,casacore::Int MSnSpw);
  KcrossJones(const MSMetaInfoForCal& msmc);
  KcrossJones(const casacore::Int& nAnt);


  virtual ~KcrossJones();

  // Return type name as string
  virtual casacore::String typeName()     { return "Kcross Jones"; };
  virtual casacore::String longTypeName() { return "Kcross Jones (cross-hand delay)"; };

  // By definition, we consider cross-hands
  virtual casacore::Bool phandonly() { return false; };

  // Local implementation of selfSolveOne 
  //   This traps combine='spw', which isn't supported yet
  virtual void selfSolveOne(VisBuffGroupAcc& vbga);
  virtual void selfSolveOne(SDBList& sdbs);



protected:


  // FFT solver for on VB, that collapses baselines and cross-hands first
  virtual void solveOneVB(const VisBuffer& vb);
  virtual void solveOneSDB(SolveDataBuffer& sdb);

  // MBD support
  void solveOneSDBmbd(SDBList& sdbs);

};


// KMBD Jones provides support for multi-band delays
class KMBDJones : public KJones {
public:

  // Constructor
  KMBDJones(VisSet& vs);
  KMBDJones(casacore::String msname,casacore::Int MSnAnt,casacore::Int MSnSpw);
  KMBDJones(const MSMetaInfoForCal& msmc);
  KMBDJones(const casacore::Int& nAnt);

  virtual ~KMBDJones();

  // Return the type enum
  virtual Type type() { return VisCal::K; };

  // Return type name as string
  virtual casacore::String typeName()     { return "KMBD Jones"; };
  virtual casacore::String longTypeName() { return "KMBD Jones (multi-band delay)"; };

  // Local setApply (to enforce KrefFreq_=0.0)
  virtual void setApply(const casacore::Record& apply);
  using KJones::setApply;

 
};



class KAntPosJones : public KJones {
public:

  // Constructor
  KAntPosJones(VisSet& vs);
  KAntPosJones(casacore::String msname,casacore::Int MSnAnt,casacore::Int MSnSpw);
  KAntPosJones(const MSMetaInfoForCal& msmc);
  KAntPosJones(const casacore::Int& nAnt);


  virtual ~KAntPosJones();

  // Return the type enum
  virtual Type type() { return VisCal::KAntPos; };

  // Return type name as string
  virtual casacore::String typeName()     { return "KAntPos Jones"; };
  virtual casacore::String longTypeName() { return "KAntPos Jones (antenna position errors)"; };

  // This is a scalar Jones matrix
  virtual Jones::JonesType jonesType() { return Jones::Scalar; };

  virtual casacore::Bool timeDepMat() { return true; };

  // This type is not smoothable
  virtual casacore::Bool smoothable() { return false; };

  // Local setApply to enforce spwmap=0 for all spw
  virtual void setApply(const casacore::Record& apply);
  using KJones::setApply;
  virtual void setCallib(const casacore::Record& callib,const casacore::MeasurementSet& selms);

  // Type-specific specify
  virtual void specify(const casacore::Record& specify);

  // Calculate phase(chan) from delay
  virtual void calcAllJones();

protected:

  // Detect phase direction and antpos for the current VB
  virtual void syncMeta(const VisBuffer& vb);
  virtual void syncMeta2(const vi::VisBuffer2& vb);

  // AntPos has three "real" parameters (dBx, dBy, dBz)
  virtual casacore::Int nPar() { return 3; };

  // Jones matrix elements are not trivial
  virtual casacore::Bool trivialJonesElem() { return false; };

  // dG/dp are not trivial
  virtual casacore::Bool trivialDJ() { return false; };

  // Initialize trivial dJs
  virtual void initTrivDJ() {};

private:
  
  // casacore::Geometry info for internal calculations (updated per VB)
  casacore::String epochref_p;
  casacore::MDirection phasedir_p;
  casacore::MPosition antpos0_p;

  // utility methods/variables for Trop Delay Error correction
  bool vlaTrDelCorrApplicable(bool checkCalTable=false);
  void markCalTableForTrDelCorr();
  void initTrDelCorr();                        // init (at set apply)
  double calcTrDelError(int iant);              // calc (per ant,timestamp)
  bool doTrDelCorr_;                            // on or off
  double userEterm_;
  casacore::Vector<double> MJDlim_;             // applicable date ranges
  #define MJD0 String("2016/08/09/00:00:00.0")
  #define MJD1 String("2016/11/15/00:00:00.0") 
  double eterm_;                                // scale
  casacore::Vector<double> losDist_, armAz_;    // geo info
  casacore::Vector<casacore::MDirection> azel_;

};


} //# NAMESPACE CASA - END

#endif
