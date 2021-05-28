//# DJones.h: Declaration of standard Polarization Calibration types
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

#ifndef SYNTHESIS_DJONES_H
#define SYNTHESIS_DJONES_H

#include <casa/aips.h>
#include <casa/Containers/Record.h>
#include <casa/BasicSL/Complex.h>
#include <synthesis/MeasurementComponents/VisCal.h>
#include <synthesis/MeasurementComponents/SolvableVisCal.h>
#include <synthesis/MeasurementComponents/StandardVisCal.h>
#include <synthesis/MeasurementComponents/SolveDataBuffer.h>
#include <synthesis/MeasurementComponents/CalCorruptor.h>

namespace casa { //# NAMESPACE CASA - BEGIN

// Forward declarations
class VisEquation;

// **********************************************************
//  DJones
//

class DJones : public SolvableVisJones {
public:

  // Constructor
  DJones(VisSet& vs);
  DJones(casacore::String msname,casacore::Int MSnAnt,casacore::Int MSnSpw);
  DJones(const MSMetaInfoForCal& msmc);
  DJones(const casacore::Int& nAnt);

  virtual ~DJones();

  // Local setapply
  using SolvableVisJones::setApply;
  virtual void setApply(const casacore::Record& apply);

  // D-specific solve setup
  using SolvableVisJones::setSolve;
  void setSolve(const casacore::Record& solvepar);

  // Return the type enum
  virtual Type type() { return VisCal::D; };

  // Return type name as string
  virtual casacore::String typeName()     { return "Dgen Jones"; };
  virtual casacore::String longTypeName() { return "Dgen Jones (instrumental polarization"; };

  // Type of Jones matrix according to nPar()
  //   Do GENERAL matrix algebra
  virtual Jones::JonesType jonesType() { return Jones::General; };

  // We can solve for polarization with D
  virtual casacore::Int solvePol() { return solvePol_; };

  // Specialization that conditions raw data & model for OTF pol solving
  virtual void setUpForPolSolve(vi::VisBuffer2& vb);

  // Hazard a guess at parameters
  virtual void guessPar(VisBuffer& vb);
  virtual void guessPar(SDBList& sdbs);
  virtual void guessPar(SDBList&,const casacore::Bool&);  // VI2

  // Update the parameters from solving
  //  (in linear approx, we always set the source update to zero, for now!)
  virtual void updatePar(const casacore::Vector<casacore::Complex> dCalPar,
			 const casacore::Vector<casacore::Complex> dSrcPar);

  // SNR is 1/err for D-terms (?)
  virtual void formSolveSNR();

  // D-specific post-solve stuff
  virtual void globalPostSolveTinker();

  // D-specific reReference
  // TBD: non-triv impl
  virtual void reReference() { cout << "reReference!" << endl;};

  virtual void applyRefAnt();

  // Method to list the D results
  virtual void logResults();

  virtual void createCorruptor(const VisIter& vi, 
			       const casacore::Record& simpar, 
			       const casacore::Int nSim);
protected:

  // D has two casacore::Complex parameters
  virtual casacore::Int nPar() { return 2; };

  // Jones matrix elements are trivial?
  //  true if GenLinear, false if General
  virtual casacore::Bool trivialJonesElem() { return (jonesType()==Jones::GenLinear); };  
  // dD/dp are trivial
  virtual casacore::Bool trivialDJ() { return true; };

  // Non-trivial Jones matrix calculation
  virtual void calcOneJones(casacore::Vector<casacore::Complex>& mat, casacore::Vector<casacore::Bool>& mOk,
			    const casacore::Vector<casacore::Complex>& par, const casacore::Vector<casacore::Bool>& pOk);

  // Initialize trivial dJs
  virtual void initTrivDJ();

private:

  casacore::Int solvePol_;
  DJonesCorruptor *dcorruptor_p;
  

  
};

// **********************************************************
//  DfJones (freq-dep D)  (general)
//

class DfJones : public DJones {
public:

  // Constructor
  DfJones(VisSet& vs);
  DfJones(casacore::String msname,casacore::Int MSnAnt,casacore::Int MSnSpw);
  DfJones(const MSMetaInfoForCal& msmc);
  DfJones(const casacore::Int& nAnt);

  virtual ~DfJones();

  // Return type name as string
  virtual casacore::String typeName()     { return "Dfgen Jones"; };
  virtual casacore::String longTypeName() { return "Dfgen Jones (frequency-dependent instrumental polarization"; };

  // This is the freq-dep version of D 
  //   (this is the ONLY fundamental difference from D)
  virtual casacore::Bool freqDepPar() { return true; };
  
};



// **********************************************************
//  DlinJones   (linearized DJones)
//

class DlinJones : public DJones {
public:

  // Constructor
  DlinJones(VisSet& vs);
  DlinJones(casacore::String msname,casacore::Int MSnAnt,casacore::Int MSnSpw);
  DlinJones(const MSMetaInfoForCal& msmc);
  DlinJones(const casacore::Int& nAnt);

  virtual ~DlinJones();

  // Return type name as string
  virtual casacore::String typeName()     { return "D Jones"; };
  virtual casacore::String longTypeName() { return "D Jones (instrumental polarization"; };

  // Type of Jones matrix according to nPar()
  //  Do linearized matrix algebra
  virtual Jones::JonesType jonesType() { return Jones::GenLinear; };

};

// **********************************************************
//  DflinJones (freq-dep, linearized DJones)
//

class DflinJones : public DlinJones {
public:

  // Constructor
  DflinJones(VisSet& vs);
  DflinJones(casacore::String msname,casacore::Int MSnAnt,casacore::Int MSnSpw);
  DflinJones(const MSMetaInfoForCal& msmc);
  DflinJones(const casacore::Int& nAnt);

  virtual ~DflinJones();

  // Return type name as string
  virtual casacore::String typeName()     { return "Df Jones"; };
  virtual casacore::String longTypeName() { return "Df Jones (frequency-dependent instrumental polarization"; };

  // This is the freq-dep version of D 
  //   (this is the ONLY fundamental difference from D)
  virtual casacore::Bool freqDepPar() { return true; };

};

// **********************************************************
//  DllsJones   (LLS DJones solve, General apply)
//


class DllsJones : public DJones {
public:

  // Constructor
  DllsJones(VisSet& vs);
  DllsJones(casacore::String msname,casacore::Int MSnAnt,casacore::Int MSnSpw);
  DllsJones(const MSMetaInfoForCal& msmc);
  DllsJones(const casacore::Int& nAnt);


  virtual ~DllsJones();

  // Return type name as string
  virtual casacore::String typeName()     { return "DLLS Jones"; };
  virtual casacore::String longTypeName() { return "DLLS Jones (instrumental polarization)"; };

  // Type of Jones matrix 
  virtual Jones::JonesType jonesType() { return Jones::General; };

  // Dlin now uses generic gather, but solves for itself per solution
  virtual casacore::Bool useGenericGatherForSolve() { return true; };
  virtual casacore::Bool useGenericSolveOne() { return false; }

  // Local implementation of selfSolveOne (generalized signature)
  //   call solveOneVB with the first (and only?) VB
  virtual void selfSolveOne(VisBuffGroupAcc& vbga) { this->solveOneVB(vbga(0)); };
  virtual void selfSolveOne(SDBList& sdbs) { this->solveOne(sdbs); };


protected:

  // LSQ Solver for 1 VB
  virtual void solveOneVB(const VisBuffer& vb);
  virtual void solveOneSDB(SolveDataBuffer& sdb);

  // LSQ Solver for an SDBList (which may contain multiple SDBs over time/spw)
  virtual void solveOne(SDBList& sdbs);


};

// **********************************************************
//  DfllsJones (freq-dep, LLS DJones solve, General apply)
//

class DfllsJones : public DllsJones {
public:

  // Constructor
  DfllsJones(VisSet& vs);
  DfllsJones(casacore::String msname,casacore::Int MSnAnt,casacore::Int MSnSpw);
  DfllsJones(const MSMetaInfoForCal& msmc);
  DfllsJones(const casacore::Int& nAnt);

  virtual ~DfllsJones();

  // Return type name as string
  virtual casacore::String typeName()     { return "DfLLS Jones"; };
  virtual casacore::String longTypeName() { return "DfLLS Jones (frequency-dependent instrumental polarization"; };

  // This is the freq-dep version of D 
  //   (this is the ONLY fundamental difference from D)
  virtual casacore::Bool freqDepPar() { return true; };

};

/*

// **********************************************************
//  X: position angle calibration (for circulars!)
//    (rendered as a Mueller for now)

class XMueller : public SolvableVisMueller {
public:

  // Constructor
  XMueller(VisSet& vs);
  XMueller(casacore::String msname,casacore::Int MSnAnt,casacore::Int MSnSpw);
  XMueller(const MSMetaInfoForCal& msmc);
  XMueller(const casacore::Int& nAnt);

  virtual ~XMueller();

  // Return the type enum
  virtual Type type() { return VisCal::X; };

  // Return type name as string
  virtual casacore::String typeName()     { return "X Mueller"; };
  virtual casacore::String longTypeName() { return "X Mueller (baseline-based)"; };

  // Type of Jones matrix according to nPar()
  virtual Mueller::MuellerType muellerType() { return Mueller::Diagonal; };

  // Local setApply
  using SolvableVisCal::setApply;
  virtual void setApply(const casacore::Record& apply);

  // Local setSolve
  using SolvableVisCal::setSolve;
  void setSolve(const casacore::Record& solvepar);

  // Turn off normalization by model....
  virtual casacore::Bool normalizable() { return false; };

  // X gathers/solves for itself
  virtual casacore::Bool useGenericGatherForSolve() { return false; };

  // X gathers/solves for itself 
  virtual void selfGatherAndSolve(VisSet& vs, VisEquation& ve) { newselfSolve(vs,ve); };
  virtual void newselfSolve(VisSet& vs, VisEquation& ve);  // new supports combine


protected:

  // X has just 1 complex parameter, storing a phase
  virtual casacore::Int nPar() { return 1; };

  // Jones matrix elements are trivial
  virtual casacore::Bool trivialMuellerElem() { return false; };

  // Calculate the X matrix for all ants
  virtual void calcAllMueller();

  // Solve in one VB for the position angle
  virtual void solveOneVB(const VisBuffer& vb);

private:

  // <nothing>

};



// **********************************************************
//  X: position angle calibration (for circulars!)
//
class XJones : public SolvableVisJones {
public:

  // Constructor
  XJones(VisSet& vs);
  XJones(casacore::String msname,casacore::Int MSnAnt,casacore::Int MSnSpw);
  XJones(const MSMetaInfoForCal& msmc);
  XJones(const casacore::Int& nAnt);


  virtual ~XJones();

  // Return the type enum
  virtual Type type() { return VisCal::X; };

  // Return type name as string
  virtual casacore::String typeName()     { return "X Jones"; };
  virtual casacore::String longTypeName() { return "X Jones (antenna-based)"; };

  // Type of Jones matrix according to nPar()
  virtual Jones::JonesType jonesType() { return Jones::Diagonal; };

  // Local setApply
  using SolvableVisCal::setApply;
  virtual void setApply(const casacore::Record& apply);

  // Local setSolve
  using SolvableVisCal::setSolve;
  void setSolve(const casacore::Record& solvepar);

  // X is normalizable by the model
  virtual casacore::Bool normalizable() { return true; };

  // X generically gathers, but solves for itself per solution
  virtual casacore::Bool useGenericGatherForSolve() { return true; };
  virtual casacore::Bool useGenericSolveOne() { return false; }

  // X gathers/solves for itself 
  virtual void selfGatherAndSolve(VisSet& vs, VisEquation& ve) { newselfSolve(vs,ve); };
  virtual void newselfSolve(VisSet& vs, VisEquation& ve);  // new supports combine

  // When genericall gathering, solve using first VB only in VBGA
  virtual void selfSolveOne(VisBuffGroupAcc& vbga) { this->solveOneVB(vbga(0)); };
  virtual void selfSolveOne(SDBList& sdbs) { this->solveOne(sdbs); };


protected:

  // X has just 1 complex parameter, storing a phase
  virtual casacore::Int nPar() { return 1; };

  // Jones matrix elements are trivial
  virtual casacore::Bool trivialJonesElem() { return false; };

  // Calculate the X matrix for all ants
  virtual void calcAllJones();

  // Solve in one VB for the position angle
  virtual void solveOneVB(const VisBuffer& vb);
  virtual void solveOneSDB(SolveDataBuffer& sdb);

  virtual void solveOne(SDBList& sdbs);

private:

  // <nothing>

};


// **********************************************************
//  Xf: position angle calibration (for circulars!)
//     (channel-dependent)
class XfJones : public XJones {
public:

  // Constructor
  XfJones(VisSet& vs);
  XfJones(casacore::String msname,casacore::Int MSnAnt,casacore::Int MSnSpw);
  XfJones(const MSMetaInfoForCal& msmc);
  XfJones(const casacore::Int& nAnt);

  virtual ~XfJones();

  // Return the type enum
  virtual Type type() { return VisCal::X; };

  // Return type name as string
  virtual casacore::String typeName()     { return "Xf Jones"; };
  virtual casacore::String longTypeName() { return "Xf Jones (antenna-based)"; };

  // This is the freq-dep version of X 
  //   (this is the ONLY fundamental difference from X)
  virtual casacore::Bool freqDepPar() { return true; };

protected:

  // Use nchan>=1 shaping
  //  (TBD: this should be generalized!)
  void initSolvePar();


};


// X-Y phase 
class GlinXphJones : public GJones {
public:

  // Constructor
  GlinXphJones(VisSet& vs);
  GlinXphJones(casacore::String msname,casacore::Int MSnAnt,casacore::Int MSnSpw);
  GlinXphJones(const MSMetaInfoForCal& msmc);
  GlinXphJones(const casacore::Int& nAnt);

  virtual ~GlinXphJones();

  // Return the type enum  (position UPSTREAM of D, not downstream like G)
  virtual Type type() { return VisCal::X; };

  // Return type name as string
  virtual casacore::String typeName()     { return "GlinXph Jones"; };
  virtual casacore::String longTypeName() { return "GlinXph Jones (X-Y phase)"; };

  // Local setapply  (unsets calWt)
  using SolvableVisJones::setApply;
  virtual void setApply(const casacore::Record& apply);

  // NOT FreqDep
  virtual casacore::Bool freqDepPar() { return false; };

  // Though derived from GJones, this type actually uses the cross-hands
  virtual casacore::Bool phandonly() { return false; };

  // GlinXphJones specialization
  virtual casacore::Bool useGenericGatherForSolve() { return true; }; 
  virtual casacore::Bool useGenericSolveOne() { return false; }

  virtual void selfGatherAndSolve(VisSet& vs, VisEquation& ve);

  // Handle trivial vbga generated by generic gather-for-solve
  virtual void selfSolveOne(VisBuffGroupAcc& vbga);
  virtual void selfSolveOne(SDBList& sdbs);

protected:


  // Solver for one VB, that collapses baselines and cross-hands first,
  //  then solves for XY-phase and QU
  virtual void solveOneVB(const VisBuffer& vb);
  virtual void solveOne(SDBList& sdbs);

  // Write QU info into table keywords
  virtual void globalPostSolveTinker();

  // Derived QU_ info
  casacore::Matrix<casacore::Float> QU_;


};


// Freq-dep XY-phase
class GlinXphfJones : public GlinXphJones {
public:

  // Constructor
  GlinXphfJones(VisSet& vs);
  GlinXphfJones(casacore::String msname,casacore::Int MSnAnt,casacore::Int MSnSpw);
  GlinXphfJones(const MSMetaInfoForCal& msmc);
  GlinXphfJones(const casacore::Int& nAnt);


  virtual ~GlinXphfJones();
  // Return type name as string
  // Return type name as string
  virtual casacore::String typeName()     { return "GlinXphf Jones"; };
  virtual casacore::String longTypeName() { return "GlinXphf Jones (X-Y phase)"; };

  // This is the freq-dep version of GlinXphJones
  //   (this is the ONLY fundamental difference from GlinXphJones)
  virtual casacore::Bool freqDepPar() { return true; };


};

*/


} //# NAMESPACE CASA - END

#endif

