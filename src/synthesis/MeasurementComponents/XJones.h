//# XJones.h: Cross-hand phase calibration
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

#ifndef SYNTHESIS_XJONES_H
#define SYNTHESIS_XJONES_H

#include <casacore/casa/aips.h>
#include <casacore/casa/Containers/Record.h>
#include <casacore/casa/BasicSL/Complex.h>
#include <synthesis/MeasurementComponents/VisCal.h>
#include <synthesis/MeasurementComponents/SolvableVisCal.h>
#include <synthesis/MeasurementComponents/StandardVisCal.h>
#include <synthesis/MeasurementComponents/SolveDataBuffer.h>
#include <synthesis/MeasurementComponents/CalCorruptor.h>

namespace casa { //# NAMESPACE CASA - BEGIN

// Forward declarations
class VisEquation;

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
//  X: Cross-hand phase (generic) 
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


// Cross-hand phase solved from data with parang coverage
class XparangJones : public XJones {
public:

  // Constructor
  XparangJones(VisSet& vs);
  XparangJones(casacore::String msname,casacore::Int MSnAnt,casacore::Int MSnSpw);
  XparangJones(const MSMetaInfoForCal& msmc);
  XparangJones(const casacore::Int& nAnt);

  virtual ~XparangJones();

  // Return the type enum  (position UPSTREAM of D, not downstream like G)
  virtual Type type() { return VisCal::X; };

  // Return type name as string
  virtual casacore::String typeName()     { return "Xparang Jones"; };
  virtual casacore::String longTypeName() { return "Xparang Jones (parang-dep X)"; };

  // Local setapply  (unsets calWt)
  using SolvableVisJones::setApply;
  virtual void setApply(const casacore::Record& apply);

  // NOT FreqDep
  virtual casacore::Bool freqDepPar() { return false; };

  // Requires cross-hands!
  virtual casacore::Bool phandonly() { return false; };

  // XparangJones specialization
  virtual casacore::Bool useGenericGatherForSolve() { return true; }; 
  virtual casacore::Bool useGenericSolveOne() { return false; }

  virtual void selfGatherAndSolve(VisSet& vs, VisEquation& ve);

  // Handle trivial vbga generated by generic gather-for-solve
  virtual void selfSolveOne(VisBuffGroupAcc& vbga);
  virtual void selfSolveOne(SDBList& sdbs);

  // Write QU info into table keywords
  virtual void globalPostSolveTinker();

  // Override for returning Q,U info via Record
  virtual casacore::Record solveActionRec();

  // Overide model division stuff...
  virtual casacore::Bool normalizable() { return false; };
  virtual casacore::Bool divideByStokesIModelForSolve() { return true; };

protected:

  // X has just 1 complex parameter, storing a phase
  virtual casacore::Int nPar() { return 1; };

  // Solver for one VB, that collapses baselines and cross-hands first,
  //  then solves for XY-phase and QU
  virtual void solveOneVB(const VisBuffer& vb);
  virtual void solveOne(SDBList& sdbs);

  // Derived QU_ info
  casacore::Matrix<casacore::Float> QU_;

  // Activity record
  casacore::Record QURec_;

};



// Freq-dep cross-hand phase
class XfparangJones : public XparangJones {
public:

  // Constructor
  XfparangJones(VisSet& vs);
  XfparangJones(casacore::String msname,casacore::Int MSnAnt,casacore::Int MSnSpw);
  XfparangJones(const MSMetaInfoForCal& msmc);
  XfparangJones(const casacore::Int& nAnt);


  virtual ~XfparangJones();
  // Return type name as string
  // Return type name as string
  virtual casacore::String typeName()     { return "Xfparang Jones"; };
  virtual casacore::String longTypeName() { return "Xfparang Jones (X-Y phase)"; };

  // This is the freq-dep version of XparangJones
  //   (this is the ONLY fundamental difference from XparangJones)
  virtual casacore::Bool freqDepPar() { return true; };


};


// **********************************************************
//  PosAng: Basis-agnostic position angle
//
class PosAngJones : public XJones {
public:

  // Constructor
  PosAngJones(VisSet& vs);
  PosAngJones(casacore::String msname,casacore::Int MSnAnt,casacore::Int MSnSpw);
  PosAngJones(const MSMetaInfoForCal& msmc);
  PosAngJones(const casacore::Int& nAnt);

  virtual ~PosAngJones();

  // PosAng had casacore::Float parameter
  virtual VisCalEnum::VCParType parType() { return VisCalEnum::REAL; };

  // Return the type enum (this is its position in the ME)
  virtual Type type() { return VisCal::C; };

  // Return type name as string
  virtual casacore::String typeName()     { return "PosAng Jones"; };
  virtual casacore::String longTypeName() { return "PosAng Jones (antenna-based)"; };

  // Type of Jones matrix (basis-sensitive)
  virtual Jones::JonesType jonesType() { return jonestype_; };

  // FreqDep
  virtual casacore::Bool freqDepPar() { return true; };

  // Local setApply
  using XJones::setApply;
  virtual void setApply(const casacore::Record& apply);

  // Local setSolve
  using XJones::setSolve;
  void setSolve(const casacore::Record& solvepar);

  // PosAng is NOT normalizable by the model (per correlation)
  virtual casacore::Bool normalizable() { return false; };
  // ...but we can divide by the I model!
  virtual casacore::Bool divideByStokesIModelForSolve() { return true; };

  // X generically gathers, but solves for itself per solution
  virtual casacore::Bool useGenericGatherForSolve() { return true; };
  virtual casacore::Bool useGenericSolveOne() { return false; }

  // Actually do the solve on ths SDBs
  virtual void selfSolveOne(SDBList& sdbs) { this->solveOne(sdbs); };


protected:

  // PosAng has just 1 float parameter, storing an angle
  virtual casacore::Int nPar() { return 1; };

  // Jones matrix elements are NOT trivial
  virtual casacore::Bool trivialJonesElem() { return false; };

  // Detect basis for this vb
  virtual void syncMeta(const VisBuffer& vb);
  virtual void syncMeta2(const vi::VisBuffer2& vb);

  // Calculate the PosAng matrix for all ants
  //  Don't use XJones::calcAllJones!!
  virtual void calcAllJones(); //  { VisJones::calcAllJones(); };

  // Calculate a single PosAngJones matrix 
  virtual void calcOneJonesRPar(casacore::Vector<casacore::Complex>& mat, casacore::Vector<casacore::Bool>& mOk,
                            const casacore::Vector<casacore::Float>& par, const casacore::Vector<casacore::Bool>& pOk );

  virtual void solveOne(SDBList& sdbs);

private:

  // X gathers/solves for itself 
  virtual void selfGatherAndSolve(VisSet& vs, VisEquation& ve) { newselfSolve(vs,ve); };
  virtual void newselfSolve(VisSet& , VisEquation& ) { throw(casacore::AipsError("PosAngJones::newselfsolve(vs,ve) NYI")); };  // new supports combine

  // When genericall gathering, solve using first VB only in VBGA
  virtual void selfSolveOne(VisBuffGroupAcc& vbga) { this->solveOneVB(vbga(0)); };

  // Solve in one VB for the position angle
  virtual void solveOneVB(const VisBuffer& ) { throw(casacore::AipsError("PosAngJones::solveOneVB(vb) NYI")); };
  virtual void solveOneSDB(SolveDataBuffer& ) { throw(casacore::AipsError("PosAngJones::solveOneSDB(sdb) NYI")); };

  // We sense basis in setMeta, and this sets the matrix type
  Jones::JonesType jonestype_;

};









// X-Y phase for ALMA -- ORIGINAL ad hoc version as GJones specialization, but in Xf position
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




} //# NAMESPACE CASA - END

#endif

