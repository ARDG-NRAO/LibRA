//# StandardVisCal.h: Declaration of standard (Solvable)VisCal types
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

#ifndef SYNTHESIS_STANDARDVISCAL_H
#define SYNTHESIS_STANDARDVISCAL_H

#include <casa/aips.h>
#include <casa/Containers/Record.h>
#include <casa/BasicSL/Complex.h>
#include <synthesis/MeasurementComponents/VisCal.h>
#include <synthesis/MeasurementComponents/SolvableVisCal.h>
#include <synthesis/MeasurementComponents/CalCorruptor.h>

namespace casa { //# NAMESPACE CASA - BEGIN

// Forward declarations
class VisEquation;
class TJonesCorruptor;


// **********************************************************
//  PJones
//

class PJones : public VisJones {
public:

  // Constructor
  PJones(VisSet& vs);
  PJones(casacore::String msname,casacore::Int MSnAnt,casacore::Int MSnSpw);
  PJones(const MSMetaInfoForCal& msmc);

  virtual ~PJones();

  // Return the type enum
  virtual Type type() { return VisCal::P; };

  // Return type name as string
  virtual casacore::String typeName()     { return "P Jones"; };
  virtual casacore::String longTypeName() { return "P Jones (parallactic angle phase)"; };

  // Type of Jones matrix according to nPar()
  Jones::JonesType jonesType() { return pjonestype_; };

protected:

  // P has one trivial casacore::Complex parameter
  virtual casacore::Int nPar() { return 1; };

  virtual casacore::Vector<casacore::Float>& pa() { return pa_; };

  // Jones matrix elements are NOT trivial
  virtual casacore::Bool trivialJonesElem() { return false; };

  // Calculate parallactic angle
  virtual void calcPar();

  // Detect basis and feed_pa for this vb
  virtual void syncMeta(const VisBuffer& vb);
  virtual void syncMeta2(const vi::VisBuffer2& vb);

  // Calculate a single PJones matrix 
  virtual void calcOneJones(casacore::Vector<casacore::Complex>& mat, casacore::Vector<casacore::Bool>& mOk,
			    const casacore::Vector<casacore::Complex>& par, const casacore::Vector<casacore::Bool>& pOk );

private:

  Jones::JonesType pjonestype_;

  casacore::Vector<casacore::Float> pa_;
  
};











// **********************************************************
//  TJones
//

class TJones : public SolvableVisJones {
public:

  // Constructor
  TJones(VisSet& vs);
  TJones(casacore::String msname,casacore::Int MSnAnt,casacore::Int MSnSpw);
  TJones(const MSMetaInfoForCal& msmc);
  TJones(const casacore::Int& nAnt);


  virtual ~TJones();

  // Return the type enum
  virtual Type type() { return VisCal::T; };

  // Return type name as string
  virtual casacore::String typeName()     { return "T Jones"; };
  virtual casacore::String longTypeName() { return "T Jones (polarization-independent troposphere"; };

  // Type of Jones matrix according to nPar()
  virtual Jones::JonesType jonesType() { return Jones::Scalar; };

  // This type is accumulatable
  virtual casacore::Bool accumulatable() { return true; };

  // This type is smoothable
  virtual casacore::Bool smoothable() { return true; };

  // Hazard a guess at parameters
  virtual void guessPar(VisBuffer& vb);
  virtual void guessPar(SDBList& sdbs,const casacore::Bool& corrDepFlags=false);  //  VI2

  // Set up corruptor
  virtual void createCorruptor(const VisIter& vi, const casacore::Record& simpar, const casacore::Int nSim);

protected:

  // T has one trivial complex parameter
  virtual casacore::Int nPar() { return 1; };

  // Jones matrix elements are trivial
  virtual casacore::Bool trivialJonesElem() { return true; };

  // Are diff'd Js trivial?
  virtual casacore::Bool trivialDJ() { return true; };

  // Initialize trivial dJs
  virtual void initTrivDJ();

private:

  // object that can simulate the corruption terms - internal to T;
  // public access is only to the CalCorruptor parts
  AtmosCorruptor *tcorruptor_p;

};










// **********************************************************
//  TfJones (freq-dep T)
//

class TfJones : public TJones {
public:

  // Constructor
  TfJones(VisSet& vs);
  TfJones(casacore::String msname,casacore::Int MSnAnt,casacore::Int MSnSpw);
  TfJones(const MSMetaInfoForCal& msmc);
  TfJones(const casacore::Int& nAnt);

  virtual ~TfJones();

  // Return the type enum
  virtual Type type() { return VisCal::T; };

  // Return type name as string
  virtual casacore::String typeName()     { return "Tf Jones"; };
  virtual casacore::String longTypeName() { return "Tf Jones (frequency-dependent atmospheric complex gain"; };

  // This is the freq-dep version of T
  //   (this is the ONLY fundamental difference from T)
  virtual casacore::Bool freqDepPar() { return true; };

protected:

  // <nothing>

private:

  // <nothing>
  
};





// **********************************************************
//  GJones
//

class GJones : public SolvableVisJones {
public:

  // Constructor
  GJones(VisSet& vs);
  GJones(casacore::String msname,casacore::Int MSnAnt,casacore::Int MSnSpw);
  GJones(const MSMetaInfoForCal& msmc);
  GJones(const casacore::Int& nAnt);

  virtual ~GJones();

  // Return the type enum
  virtual Type type() { return VisCal::G; };

  // Return type name as string
  virtual casacore::String typeName()     { return "G Jones"; };
  virtual casacore::String longTypeName() { return "G Jones (electronic Gain)"; };

  // Local setSolve
  //  (parses solmode)
  using SolvableVisJones::setSolve;
  virtual void setSolve(const casacore::Record& solve);

  // Type of Jones matrix according to nPar()
  virtual Jones::JonesType jonesType() { return Jones::Diagonal; };

  // This type is accumulatable
  virtual casacore::Bool accumulatable() { return true; };

  // This type is smoothable
  virtual casacore::Bool smoothable() { return true; };

  // Nominally, we will only use parallel hands for now
  virtual casacore::Bool phandonly() { return true; };

  // Hazard a guess at parameters
  virtual void guessPar(VisBuffer& vb);
  virtual void guessPar(SDBList& sdbs,const casacore::Bool& corrDepFlags=false);  //  VI2

  virtual void createCorruptor(const VisIter& vi, const casacore::Record& simpar, const casacore::Int nSim);

protected:

  // G has two trivial casacore::Complex parameters
  virtual casacore::Int nPar() { return 2; };

  // Jones matrix elements are trivial
  virtual casacore::Bool trivialJonesElem() { return true; };

  // dG/dp are trivial
  virtual casacore::Bool trivialDJ() { return true; };

  // Initialize trivial dJs
  virtual void initTrivDJ();

private:

  GJonesCorruptor *gcorruptor_p;
  
};



// **********************************************************
//  BJones  (freq-dep GJones)
//

class BJones : public GJones {
public:

  // Constructor
  BJones(VisSet& vs);
  BJones(casacore::String msname,casacore::Int MSnAnt,casacore::Int MSnSpw);
  BJones(const MSMetaInfoForCal& msmc);
  BJones(const casacore::Int& nAnt);

  virtual ~BJones();

  // Return the type enum
  virtual Type type() { return VisCal::B; };

  // Return type name as string
  virtual casacore::String typeName()     { return "B Jones"; };
  virtual casacore::String longTypeName() { return "B Jones (bandpass)"; };

  // Local setSolve
  using GJones::setSolve;
  virtual void setSolve(const casacore::Record& solve);

  // This is the freq-dep version of G 
  //   (this is the ONLY fundamental difference from G)
  virtual casacore::Bool freqDepPar() { return true; };

  virtual void normalize();

  // Specialize post solve operations (adds chan gap fill)
  virtual void globalPostSolveTinker();
  
  // Fill in bracketted channel gaps in solutions
  virtual void fillChanGaps();
  virtual void fillChanGapArray(casacore::Array<casacore::Complex>& sol,
				casacore::Array<casacore::Bool>& solOK);

protected:

  // B-specific weight scaling factors
  virtual void calcWtScale(); // B-specific channel-collapsing weight scale calc

private:

  // widest channel gap to fill by interpolation
  casacore::Int maxchangap_p;


};



// **********************************************************
//  JJones
//

class JJones : public SolvableVisJones {
public:

  // Constructor
  JJones(VisSet& vs);
  JJones(casacore::String msname,casacore::Int MSnAnt,casacore::Int MSnSpw);
  JJones(const MSMetaInfoForCal& msmc);
  JJones(const casacore::Int& nAnt);

  virtual ~JJones();

  // J-specific solve setup
  using SolvableVisJones::setSolve;
  void setSolve(const casacore::Record& solvepar);

  // Return the type enum
  virtual Type type() { return VisCal::J; };

  // Return type name as string
  virtual casacore::String typeName()     { return "J Jones"; };
  virtual casacore::String longTypeName() { return "J Jones (generic polarized gain)"; };

  // Type of Jones matrix according to nPar()
  virtual Jones::JonesType jonesType() { return Jones::General; };

  // Hazard a guess at parameters
  virtual void guessPar(VisBuffer& vb);

  // J-specific reReference
  // TBD: non-triv impl
  virtual void reReference() {};

protected:

  // J has four trivial casacore::Complex parameters
  virtual casacore::Int nPar() { return 4; };

  // Jones matrix elements are trivial
  virtual casacore::Bool trivialJonesElem() { return true; };

  // dJ/dp are trivial
  virtual casacore::Bool trivialDJ() { return true; };

  // Initialize trivial dJs
  virtual void initTrivDJ();

private:

  // <nothing>
  
};


// **********************************************************
//  JfJones (freq-dep J)
//

class JfJones : public JJones {
public:

  // Constructor
  JfJones(VisSet& vs);
  JfJones(casacore::String msname,casacore::Int MSnAnt,casacore::Int MSnSpw);
  JfJones(const MSMetaInfoForCal& msmc);
  JfJones(const casacore::Int& nAnt);

  virtual ~JfJones();

  // Return the type enum
  virtual Type type() { return VisCal::J; };

  // Return type name as string
  virtual casacore::String typeName()     { return "Jf Jones"; };
  virtual casacore::String longTypeName() { return "Jf Jones (frequency-dependent generic polarized gain)"; };

  // This is the freq-dep version of J
  //   (this is the ONLY fundamental difference from J)
  virtual casacore::Bool freqDepPar() { return true; };

protected:

  // <nothing>

private:

  // <nothing>
  
};


// **********************************************************
//  M: baseline-based (closure) 
//

/*

class MMueller : public SolvableVisMueller {
public:

  // Constructor
  MMueller(VisSet& vs);
  MMueller(casacore::String msname,casacore::Int MSnAnt,casacore::Int MSnSpw);
  MMueller(const MSMetaInfoForCal& msmc);
  MMueller(const casacore::Int& nAnt);

  virtual ~MMueller();

  // Return the type enum
  virtual Type type() { return VisCal::M; };

  // Return type name as string
  virtual casacore::String typeName()     { return "M Mueller"; };
  virtual casacore::String longTypeName() { return "M Mueller (baseline-based)"; };

  // Type of Jones matrix according to nPar()
  virtual Mueller::MuellerType muellerType() { return Mueller::Diag2; };

  // Local setApply
  using SolvableVisCal::setApply;
  virtual void setApply(const casacore::Record& apply);

  // M gathers/solves for itself
  virtual casacore::Bool useGenericGatherForSolve() { return false; };

  // M solves for itself (by copying averaged data)
  virtual void selfGatherAndSolve(VisSet& vs, VisEquation& ve) { newselfSolve(vs,ve); };
  virtual void newselfSolve(VisSet& vs, VisEquation& ve);  // new supports combine

  // Local M version only supports normalization
  virtual void globalPostSolveTinker();

  virtual void createCorruptor(const VisIter& vi, const casacore::Record& simpar, const casacore::Int nSim);
protected:

  // M currently has just 2 complex parameters, i.e., both parallel hands
  virtual casacore::Int nPar() { return 2; };

  // Jones matrix elements are trivial
  virtual casacore::Bool trivialMuellerElem() { return true; };

private:
  AtmosCorruptor *atmcorruptor_p;

};






// **********************************************************
//  MfMueller (freq-dep MMueller)
//

class MfMueller : public MMueller {
public:

  // Constructor
  MfMueller(VisSet& vs);
  MfMueller(casacore::String msname,casacore::Int MSnAnt,casacore::Int MSnSpw);
  MfMueller(const MSMetaInfoForCal& msmc);
  MfMueller(const casacore::Int& nAnt);

  virtual ~MfMueller();

  // Return the type enum
  virtual Type type() { return VisCal::M; };

  // Return type name as string
  virtual casacore::String typeName()     { return "Mf Mueller"; };
  virtual casacore::String longTypeName() { return "Mf Mueller (closure bandpass)"; };

  // This is the freq-dep version of M
  //   (this is the ONLY fundamental difference from M)
  virtual casacore::Bool freqDepPar() { return true; };

  // Normalize baseline spectra (after B)
  virtual void normalize();

protected:

  // <nothing>

private:

  // <nothing>

};

*/


// **********************************************************
//  TOPac 
//

class TOpac : public TJones {
public:

  // Constructor
  TOpac(VisSet& vs);
  TOpac(casacore::String msname,casacore::Int MSnAnt,casacore::Int MSnSpw);
  TOpac(const MSMetaInfoForCal& msmc);

  virtual ~TOpac();

  // Return the type enum
  virtual Type type() { return VisCal::T; };

  // Return type name as string
  virtual casacore::String typeName()     { return "TOpac"; };
  virtual casacore::String longTypeName() { return "TOpac (Opacity corrections in amplitude)"; };

  // Type of Jones matrix according to nPar()
  Jones::JonesType jonesType() { return Jones::Scalar; };

  // We have casacore::Float parameters
  virtual VisCalEnum::VCParType parType() { return VisCalEnum::REAL; };

  // Local specialization of setApply to extract opacity
  void setApply(const casacore::Record& applypar);

  // Report apply-related info
  casacore::String applyinfo();

protected:

  // TOpac has one casacore::Float par per ant
  virtual casacore::Int nPar() { return 1; };

  // Jones matrix elements are NOT trivial
  virtual casacore::Bool trivialJonesElem() { return false; };

  // Access to z.a. data
  casacore::Vector<casacore::Double>& za() { return za_; };

  // Detect zenith angle for this vb
  virtual void syncMeta(const VisBuffer& vb);
  virtual void syncMeta2(const vi::VisBuffer2& vb);

  // Calculate parameters (in this case, the z.a.)
  virtual void calcPar();

  // Calculate the TOpac matrix for all ants
  virtual void calcAllJones();

private:

  // avoid compiler warnings
  using TJones::setApply;  

  // opacity per spw
  casacore::Vector<casacore::Double> opacity_;

  // zenith angle per antenna
  casacore::Vector<casacore::Double> za_;
  
};


// **********************************************************
//  TfOpac (freq-dep TOpac)
//

class TfOpac : public TOpac {
public:

  // Constructor
  TfOpac(VisSet& vs);
  TfOpac(casacore::String msname,casacore::Int MSnAnt,casacore::Int MSnSpw);
  TfOpac(const MSMetaInfoForCal& msmc);

  virtual ~TfOpac();

  // Return the type enum
  virtual Type type() { return VisCal::T; };

  // Return type name as string
  virtual casacore::String typeName()     { return "TfOpac"; };
  virtual casacore::String longTypeName() { return "TfOpac (frequency-dependent opacity"; };

  // This is the freq-dep version of TOpac
  //   (this is the ONLY fundamental difference from TOpac)
  virtual casacore::Bool freqDepPar() { return true; };

protected:

  // Calculate the TfOpac matrix for all ants
  virtual void calcAllJones();

private:

  //  Frequency weight scaling factors
  virtual void calcWtScale();
  
};

// **********************************************************
//  M: baseline-based (closure) 
//


class MMueller : public SolvableVisMueller {
public:

  // Constructor
  MMueller(VisSet& vs);
  MMueller(casacore::String msname,casacore::Int MSnAnt,casacore::Int MSnSpw);
  MMueller(const MSMetaInfoForCal& msmc);
  MMueller(const casacore::Int& nAnt);

  virtual ~MMueller();

  // Return the type enum
  virtual Type type() { return VisCal::M; };

  // Return type name as string
  virtual casacore::String typeName()     { return "M Mueller"; };
  virtual casacore::String longTypeName() { return "M Mueller (baseline-based)"; };

  // Type of Jones matrix according to nPar()
  virtual Mueller::MuellerType muellerType() { return Mueller::Diag2; };

  // Local setApply
  using SolvableVisCal::setApply;
  virtual void setApply(const casacore::Record& apply);

  // M gathers/solves for itself under self-determined conditions
  //  Initialization:  VI1: False ; VI2: True
  virtual casacore::Bool useGenericGatherForSolve() { return useGenGathSolve_p; };

  // When generically gathering (VI2 only!), _don't_ use generic solveOne
  //   (use selfSolveOne below, which is just copy from averaged data)
  virtual casacore::Bool useGenericSolveOne() { return !useGenGathSolve_p; };

  // M solves for itself (by copying averaged data)
  virtual void selfGatherAndSolve(VisSet& vs, VisEquation& ve) { newselfSolve(vs,ve); };
  virtual void newselfSolve(VisSet& vs, VisEquation& ve);  // new supports combine

  // Local M version only supports normalization
  virtual void globalPostSolveTinker();

  // Modern solving
  virtual void selfSolveOne(SDBList& sdbs);
  using SolvableVisMueller::selfSolveOne;  // VS/VI version (trap)

  virtual void createCorruptor(const VisIter& vi, const casacore::Record& simpar, const casacore::Int nSim);
protected:

  // M currently has just 2 complex parameters, i.e., both parallel hands
  virtual casacore::Int nPar() { return 2; };

  // Jones matrix elements are trivial
  virtual casacore::Bool trivialMuellerElem() { return true; };

  // solve implementation
  virtual void solveOne(SDBList& sdbs);

private:
  AtmosCorruptor *atmcorruptor_p;

  const bool useGenGathSolve_p;  // true unless VisSet-driven ctor used


};


// **********************************************************
//  MfMueller (freq-dep MMueller)
//

class MfMueller : public MMueller {
public:

  // Constructor
  MfMueller(VisSet& vs);
  MfMueller(casacore::String msname,casacore::Int MSnAnt,casacore::Int MSnSpw);
  MfMueller(const MSMetaInfoForCal& msmc);
  MfMueller(const casacore::Int& nAnt);

  virtual ~MfMueller();

  // Return the type enum
  virtual Type type() { return VisCal::M; };

  // Return type name as string
  virtual casacore::String typeName()     { return "Mf Mueller"; };
  virtual casacore::String longTypeName() { return "Mf Mueller (closure bandpass)"; };

  // This is the freq-dep version of M
  //   (this is the ONLY fundamental difference from M)
  virtual casacore::Bool freqDepPar() { return true; };

  // Normalize baseline spectra (after B)
  virtual void normalize();

protected:

  // <nothing>

private:

  // <nothing>

};



} //# NAMESPACE CASA - END

#endif

