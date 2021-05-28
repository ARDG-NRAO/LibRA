//# VisCal.h: Definitions of interface for VisCal 
//# Copyright (C) 1996,1997,2000,2001,2002,2003
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
//# $Id: VisCal.h,v 1.10 2006/02/06 19:23:11 gmoellen Exp $

#ifndef SYNTHESIS_VISCAL_H
#define SYNTHESIS_VISCAL_H

#include <casa/aips.h>
#include <casa/Containers/Record.h>
#include <casa/BasicSL/Complex.h>
#include <casa/BasicSL/Constants.h>
#include <synthesis/MeasurementComponents/Mueller.h>
#include <synthesis/MeasurementComponents/Jones.h>
#include <synthesis/CalTables/VisCalEnum.h>
#include <synthesis/MeasurementComponents/VisVector.h>
#include <msvis/MSVis/VisSet.h>
#include <msvis/MSVis/VisBuffer2.h>

#include <msvis/MSVis/VisBuffGroupAcc.h>

namespace casa { //# NAMESPACE CASA - BEGIN

class MSMetaInfoForCal;

// **********************************************************
//  VisCal
//

class VisCal {

  friend class SolvableVisJones;

public:
 
  // Allowed types of VisCal matrices - 'correct' order
  //  enum Type{UVMOD,Mf,M,K,B,G,D,C,E,P,T,EP,F};
  //  enum Type{Test=0,ANoise,M,KAntPos,K,B,G,J,D,X,C,P,E,T,F,A,ALL};
  enum Type{Test=0,ANoise,M,KAntPos,B,K,G,J,D,X,C,P,E,T,F,A,ALL};

  // Enumeration of parameter types (casacore::Complex, Real, or Both)
  enum ParType{Co,Re,CoRe};

  static casacore::String nameOfType(Type type) {
    switch (type) {
    case ANoise: return "ANoise";
    case M: return "M";
    case K: return "K";
    case B: return "B";
    case J: return "J";
    case D: return "D";
    case X: return "X";
    case C: return "C";
    case P: return "P";
    case E: return "E";
    case T: return "T";
    case F: return "F";
    case A: return "A";
    default: return "0";
    }
  }

  VisCal(VisSet& vs);
  
  VisCal(casacore::String msname,casacore::Int MSnAnt,casacore::Int MSnSpw);

  VisCal(const MSMetaInfoForCal& msmc);
  
  VisCal(const casacore::Int& nAnt);
  
  virtual ~VisCal();

  // Return the type of this calibration matrix (actual type of derived class).
  //    (Must be implemented in specializations!)
  virtual Type type()=0;

  // Return type name as string
  virtual casacore::String typeName()     { return "Unknown VisCal"; };
  virtual casacore::String longTypeName() { return "Unknown VisCal"; };

  // Return casacore::Matrix type
  virtual VisCalEnum::MatrixType matrixType() { return VisCalEnum::GLOBAL; };

  // Return the parameter type (nominally complex)
  virtual VisCalEnum::VCParType parType() { return VisCalEnum::COMPLEX; };

  // Number of pars per ant/bln
  //    (Must be implemented in specializations!)
  virtual casacore::Int nPar()=0;

  // Report calibration availability per spw
  //  (always true for non-tabular?)
  virtual casacore::Vector<casacore::Bool> spwOK() { return casacore::Vector<casacore::Bool>(nSpw(),true); };
  virtual casacore::Bool spwOK(casacore::Int) { return true; };

  // Calibration available?
  //  (always true for non-tabular)
  virtual casacore::Bool calAvailable(vi::VisBuffer2&) { return true;};

  // Calibration expected AND available? 
  //  (always assume true for non-tabular)
  virtual casacore::Bool VBOKforCalApply(vi::VisBuffer2&) { return true;};

  // Frequency-dependent Parameters?  Nominally not.
  virtual casacore::Bool freqDepPar() { return false; };

  // Number of par channels in current spw
  inline const casacore::Int& nChanPar() const { return nChanPar_[currSpw_]; };
  
  // Frequency-dependent Matrices?  Nominally same as freqDepPar.
  virtual casacore::Bool freqDepMat() { return freqDepPar(); };

  // Freq-dep Weight scaling?  // almost always false
  virtual casacore::Bool freqDepCalWt() { return false; };

  // Matrices time-dependent per parameter set (nominally no)
  virtual casacore::Bool timeDepMat() { return false; };

  // Is this calibration matrix to be applied?
  inline casacore::Bool isApplied() {return applied_;};

  // Is this solveable? (never via this interface)
  virtual casacore::Bool isSolvable() {return false;};

  // Return the time interval over which this calibration is constant
  inline virtual casacore::Double& interval() {return interval_;}

  // Set the application parameters
  virtual void setApply();
  virtual void setApply(const casacore::Record& apply);
  virtual void setCallib(const casacore::Record& callib,const casacore::MeasurementSet& selms);

  // Apply info/params, suitable for logging
  virtual casacore::String applyinfo();

  // simulation params - for a VC, sim is apply; for a SVC this will get 
  // overriden
  inline virtual casacore::String siminfo() { return applyinfo(); };

  // Trigger calibration of weights
  inline casacore::Bool& calWt() { return calWt_; };

  // Apply calibration to data in VisBuffer (correct casacore::Data or corrupt Model)
  //  (in-place versions)
  virtual void correct(VisBuffer& vb, casacore::Bool trial=false);
  virtual void correct2(vi::VisBuffer2& vb, casacore::Bool trial=false, 
			casacore::Bool doWtSp=false, casacore::Bool dosync=true);
  virtual void corrupt(VisBuffer& vb);
  virtual void corrupt2(vi::VisBuffer2& vb);

  // Apply calibration to data in VisBuffer; 
  //  (alternate output versions)
  virtual void correct(VisBuffer& vb, casacore::Cube<casacore::Complex>& Vout,casacore::Bool trial=false);
  virtual void corrupt(VisBuffer& vb, casacore::Cube<casacore::Complex>& Mout);
  virtual void corrupt2(vi::VisBuffer2& vb, casacore::Cube<casacore::Complex>& Mout);

  // Flag counting
  virtual void initCalFlagCount();
  virtual casacore::Record actionRec();

  // Report the state
  virtual void state();

  virtual void currMetaNote();

  // Set the print level
  inline void setPrtlev(const casacore::Int& prtlev) { prtlev_=prtlev; };

  // Baseline index from antenna indices: (assumes a1<=a2 !!)
  inline casacore::Int blnidx(const casacore::Int& a1, 
		    const casacore::Int& a2) { return  a1*nAnt() - a1*(a1+1)/2 + a2; };

  inline casacore::String& extraTag() { return extratag_; };

 
  // VI2-related refactor--------------------------------------

  // Set "current" meta info, so internals can be registered
  virtual void setMeta(int obs, int scan, double time,
		       int spw, const casacore::Vector<double>& freq,
		       int fld);

  // Reshape solvePar* arrays for the currSpw()  
  //  (sensitive to freqDepPar())
  virtual void sizeApplyParCurrSpw(int nVisChan);

  // Set parameters to def values in the currSpw(), 
  //   and optionally sync everything
  virtual void setDefApplyParCurrSpw(bool sync=false, bool doInv=false);

  // Set parameters to specified values in the currSpw(),
  //   and optionally sync matrices
  virtual void setApplyParCurrSpw(const casacore::Cube<casacore::Complex> cpar,
				  bool sync=false, bool doInv=false);

  virtual void setApplyParCurrSpw(const casacore::Cube<float> rpar,
				  bool sync=false, bool doInv=false);

  // Access (public) to current solution parameters and matrices
  inline virtual casacore::Cube<casacore::Complex>& currCPar()  {return (*currCPar_[currSpw()]);};
  inline virtual casacore::Cube<casacore::Float>&   currRPar()  {return (*currRPar_[currSpw()]);};
  inline virtual casacore::Cube<casacore::Bool>&    currParOK() {return (*currParOK_[currSpw()]);};


protected:

  // Set applied state flag
  inline void setApplied(const casacore::Bool& flag) {applied_=flag;};

  inline casacore::String& msName() { return msName_; };

  // General Shape Info 
  inline casacore::Int& nSpw() { return nSpw_; };
  inline casacore::Int& nAnt() { return nAnt_; };
  inline casacore::Int& nBln() { return nBln_; };

  // The number of sets of parameters under consideration
  virtual casacore::Int& nElem()=0;

  // Number of Calibration matrices on ant/bln axis
  virtual casacore::Int nCalMat()=0;

  // Current in-focus spw
  inline casacore::Int& currSpw() { return currSpw_; };

  // Current coords
  inline casacore::Double& lastTime()                { return lastTime_(currSpw()); };
  inline casacore::Double& currTime()                { return currTime_(currSpw()); };
  inline casacore::Int&    currScan()                { return currScan_(currSpw()); };
  inline casacore::Int&    currObs()                 { return currObs_(currSpw()); };
  inline casacore::Int&    currField()               { return currField_(currSpw()); };
  inline casacore::Int&    currIntent()              { return currIntent_(currSpw()); };
  inline casacore::Vector<casacore::Double>& currFreq()        { return currFreq_; };

  inline casacore::Double& refTime()                 { return refTime_; };
  inline casacore::Double& refFreq()                 { return refFreq_; };

  // Current spectral shapes
  inline casacore::Int& nChanPar()              { return nChanPar_[currSpw_]; };
  inline casacore::Int& nChanMat()              { return nChanMat_[currSpw_]; };
  inline casacore::Int& startChan()             { return startChan_[currSpw_];};
  inline casacore::Vector<casacore::Int>& nChanParList()  { return nChanPar_; };
  inline casacore::Vector<casacore::Int>& nChanMatList()  { return nChanMat_; };
  inline casacore::Vector<casacore::Int>& startChanList() { return startChan_;};

  // Access to matrix renderings of Visibilities
  inline VisVector& V() { return (*V_[currSpw()]); };

  // Validation of calibration parameters
  inline void invalidateP() {PValid_(currSpw())=false;};
  inline void validateP()   {PValid_(currSpw())=true;};
  inline casacore::Bool PValid()      {return PValid_(currSpw());};

  // Invalidate cal matrices generically 
  virtual void invalidateCalMat()=0;

  // Access to weight-scaling factors
  inline casacore::Cube<casacore::Float>& currWtScale() { return (*currWtScale_[currSpw()]); };

  // Flag counting
  virtual void countInFlag(const VisBuffer& vb);
  virtual void countInFlag2(const vi::VisBuffer2& vb);
  virtual void countOutFlag(const VisBuffer& vb);
  virtual void countOutFlag2(const vi::VisBuffer2& vb);

  // Row-by-row apply to a casacore::Cube<casacore::Complex> (generic)
  virtual void applyCal(VisBuffer& vb, casacore::Cube<casacore::Complex>& Vout,casacore::Bool trial=false)=0;
  virtual void applyCal2(vi::VisBuffer2& vb, 
			 casacore::Cube<casacore::Complex>& Vout,casacore::Cube<casacore::Float>& Wout,
			 casacore::Bool trial=false)=0;

  // Synchronize "gains" with a VisBuffer or another VisCal
  virtual void syncCal(const VisBuffer& vb,
		       const casacore::Bool& doInv=false);
  virtual void syncCal2(const vi::VisBuffer2& vb,
		       const casacore::Bool& doInv=false);
  virtual void syncCal(VisCal& vc);

  // Set internal meta data from a VisBuffer or another VisCal
  virtual void syncMeta(const VisBuffer& vb);
  virtual void syncMeta2(const vi::VisBuffer2& vb);
  void syncMeta(VisCal& vc);

  void syncMeta(const casacore::Int& spw,
		const casacore::Double& time,
		const casacore::Int& field,
		const casacore::Vector<casacore::Double>& freq,
		const casacore::Int& nchan);

  // Set the calibration matrix channelization
  void setCalChannelization(const casacore::Int& nChanDat);

  // Test for need of new calibration
  void checkCurrCal();

  // Synchronize "gains" with current meta-data
  virtual void syncCal(const casacore::Bool& doInv=false);

  // Sync parameters for current meta data
  virtual void syncPar();

  // Calculate Mueller parameters by some means 
  virtual void calcPar();

  // Sync matrices generically for current meta data 
  virtual void syncCalMat(const casacore::Bool& doInv=false)=0;

  // Return print (cout) level
  inline casacore::Int& prtlev() { return prtlev_; };

  // set current field index vector to given field id
  void setCurrField(const casacore::Int& ifld);

  // Access to the MSMetaInfoForCal (throws if none)
  const MSMetaInfoForCal& msmc() const
  {
      if (msmc_) return *msmc_;
      else throw(casacore::AipsError("VisCal::msmc(): No MSMetaInfoForCal object!"));
  };

private:

  // Defalt ctor is private
  VisCal();

  // Initialize pointers, etc.
  void initVisCal();

  // Delete pointers
  void deleteVisCal();

  // Associated casacore::MS name
  casacore::String msName_;

  // The MSMetaInfoForCal pointer
  const MSMetaInfoForCal* msmc_;
  const bool delmsmc_;  // must delete _only_ if locally formed

  // Number of Spectral windows
  casacore::Int nSpw_;

  // Number of antennas
  casacore::Int nAnt_;

  // Number of baselines
  casacore::Int nBln_;

  // Current synchronized spw
  casacore::Int currSpw_;

  // Current indices
  casacore::Vector<casacore::Double> currTime_;
  casacore::Vector<casacore::Int> currScan_;
  casacore::Vector<casacore::Int> currObs_;
  casacore::Vector<casacore::Int> currField_;
  casacore::Vector<casacore::Int> currIntent_;
  casacore::Vector<casacore::Double> currFreq_;
  casacore::Vector<casacore::Double> lastTime_;
  casacore::Double refTime_;
  casacore::Double refFreq_;

  // Channel counts
  casacore::Vector<casacore::Int> nChanPar_, nChanMat_;
  casacore::Vector<casacore::Int> startChan_;

  // Solution timescale (context-dependent)
  casacore::Double interval_;

  // Application flag
  casacore::Bool applied_;

  // In-focus channel for single-chan solves on multi-chan data
  casacore::Int focusChan_;

  // VisVector wrapper (per Spw)
  casacore::PtrBlock<VisVector*> V_;

  // Current parameters
  casacore::PtrBlock<casacore::Cube<casacore::Complex>*>  currCPar_;   // [nSpw](nPar,nChanPar,nElm)
  casacore::PtrBlock<casacore::Cube<casacore::Float>*>    currRPar_;   // [nSpw](nPar,nChanPar,nElm)
  casacore::PtrBlock<casacore::Cube<casacore::Bool>*>     currParOK_;  // [nSpw](nPar,nChanPar,nElm)

  // Paremeter validity
  casacore::Vector<casacore::Bool> PValid_;

  // Trigger calibration of weights
  casacore::Bool calWt_;

  // Weight scale factors
  casacore::PtrBlock<casacore::Cube<casacore::Float>*> currWtScale_;  // [nSpw](nPar,nChan,nElm)

  // Flag counting
  casacore::Int64 ndataIn_, nflagIn_, nflagOut_;

  // Print level
  casacore::Int prtlev_;

  casacore::String extratag_;  // e.g. to tag as noise scale


};


// **********************************************************
//  VisMueller
//

class VisMueller : virtual public VisCal {

public:

  // Constructor
  VisMueller(VisSet& vs);

  VisMueller(casacore::String msname,casacore::Int MSnAnt,casacore::Int MSnSpw);

  VisMueller(const MSMetaInfoForCal& msmc);

  VisMueller(const casacore::Int& nAnt);

  virtual ~VisMueller();

  // Return casacore::Matrix type
  virtual VisCalEnum::MatrixType matrixType() { return VisCalEnum::MUELLER; };

  // Mueller matrix type (must be implemented in Mueller specializations!)
  virtual Mueller::MuellerType muellerType()=0;  

  // Report the state
  virtual void state();

protected:

  // Total number of parameter sets required
  virtual casacore::Int& nElem() { return nBln(); };

  // Number of Cal Matrices to form on baseline axis
  //  (Mueller, apply context: nBln())
  virtual casacore::Int nCalMat() { return nBln(); };

  // Are the parameters the matrix elements? 
  //   (or is a non-trivial calculation required?)
  //    (Must be implemented in specializations!)
  virtual casacore::Bool trivialMuellerElem()=0;

  // Are we applying via Mueller multiplication?
  //   (necessarily true for native Muellers)
  virtual casacore::Bool applyByMueller() { return true; };

  // Access to matrix renderings of Muellers
  inline Mueller& M()   { return (*M_[currSpw()]); };

  // Access to current matrices
  inline casacore::Cube<casacore::Complex>& currMElem()   {return (*currMElem_[currSpw()]);};
  inline casacore::Cube<casacore::Bool>&    currMElemOK() {return (*currMElemOK_[currSpw()]);};

  // Invalidate cal matrices generically (at this level, just Mueller)
  inline virtual void invalidateCalMat() { invalidateM(); };

  // Validation of Mueller matrices (for currSpw)
  inline void invalidateM() {MValid_(currSpw())=false;};
  inline void validateM()   {MValid_(currSpw())=true;};
  inline casacore::Bool MValid()      {return MValid_(currSpw());};

  // Row-by-row apply to a casacore::Cube<casacore::Complex> (applyByMueller override)
  virtual void applyCal(VisBuffer& vb, casacore::Cube<casacore::Complex>& Vout,casacore::Bool trial=false);
  virtual void applyCal2(vi::VisBuffer2& vb, 
			 casacore::Cube<casacore::Complex>& Vout,casacore::Cube<casacore::Float>& Wout,
			 casacore::Bool trial=false);
  //  { throw(casacore::AipsError("VisMueller::applyCal2 NYI!!!!!!!!!!!!!")); };

  // Sync matrices for current meta data (Mueller override)
  virtual void syncCalMat(const casacore::Bool& doInv=false);

  // Sync Mueller matrix elements for current parameters
  virtual void syncMueller(const casacore::Bool& doInv=false);

  // Calculate an ensemble of Mueller matrices (all baselines, channels)
  virtual void calcAllMueller();

  // Calculate a single Mueller matrix by some means
  virtual void calcOneMueller(casacore::Vector<casacore::Complex>& mat, casacore::Vector<casacore::Bool>& mOk,
			      const casacore::Vector<casacore::Complex>& par, const casacore::Vector<casacore::Bool>& pOk);

  // Invert Mueller matrices
  virtual void invMueller();

  // Set matrix elements according to their ok flags
  //  (e.g., makes a unit matrix if everything flagged,
  //   so we don't have to make atomic ok checks in apply)
  virtual void setMatByOk();

  // Create Mueller matrix algebra interface
  void createMueller();
 
  // Synchronize weight scale factors
  virtual void syncWtScale();

  // Perform weight scale calculation (specializable)
  virtual void calcWtScale();

  // Update the wt vector for a baseline
  virtual void updateWt(casacore::Vector<casacore::Float>& wt,const casacore::Int& a1,const casacore::Int& a2);


private:

  // Default ctor is private
  VisMueller();

  // Init VisMueller pointers, etc.
  void initVisMueller();

  // Delete the this* caches
  void deleteVisMueller();

  // Mueller algebra wrapper (per Spw)
  casacore::PtrBlock<Mueller*> M_;                

  // Current Mueller matrix elements
  casacore::PtrBlock<casacore::Cube<casacore::Complex>*> currMElem_;    // [nSpw]([1,2,4,16],nChanMat,nBln)
  casacore::PtrBlock<casacore::Cube<casacore::Bool>*>    currMElemOK_;  // [nSpw]([1,2,4,16],nChanMat,nBln)

  // Mueller validity
  casacore::Vector<casacore::Bool> MValid_;

};


// **********************************************************
//  VisJones
//

class VisJones : virtual public VisMueller {

  friend class SolvableVisJones;

public:

  // Constructor
  VisJones(VisSet& vs);

  VisJones(casacore::String msname,casacore::Int MSnAnt,casacore::Int MSnSpw);

  VisJones(const MSMetaInfoForCal& msmc);

  VisJones(const casacore::Int& nAnt);

  virtual ~VisJones();

  // Return casacore::Matrix type
  virtual VisCalEnum::MatrixType matrixType() { return VisCalEnum::JONES; };

  // What kind of Mueller matrices should we use?  
  //   (A function of the jonesType and target data shape)
  virtual Mueller::MuellerType muellerType();

  // What kind of Jones matrix are we?  
  //    (Must be implemented in specializations!)
  virtual Jones::JonesType jonesType()=0; 

  // Report the state
  virtual void state();

protected:

  // Number of parameter sets is number of antennas
  inline virtual casacore::Int& nElem() { return nAnt(); };

  // Number of Cal Matrices to form on antenna axis
  //  (Jones, apply context: nAnt())
  virtual casacore::Int nCalMat() { return nAnt(); };

  // Jones matrices can never be trivial Muellers!
  virtual casacore::Bool trivialMuellerElem() { return false; };

  // Are the parameters the Jones matrix elements?
  //   (or is a non-trivial calculation required?)
  //    (Must be implemented in specializations!)
  virtual casacore::Bool trivialJonesElem()=0;

  // Are we applying via Mueller or Jones multiplication?
  //   (probably by Jones for native Jones?)
  virtual casacore::Bool applyByMueller() { return false; };
  virtual casacore::Bool applyByJones()   { return true; };

  // Access to matrix renderings of Jones matrices
  inline Jones& J1() { return *J1_[currSpw()]; };
  inline Jones& J2() { return *J2_[currSpw()]; };

  // Access to Jones matrix element array
  inline casacore::Cube<casacore::Complex>& currJElem() {return (*currJElem_[currSpw()]);};
  inline casacore::Cube<casacore::Bool>& currJElemOK()  {return (*currJElemOK_[currSpw()]);};

  // Invalidate cal matrices generically (at this level, both Mueller and Jones)
  inline virtual void invalidateCalMat() { invalidateM(); invalidateJ(); };

  // Validation of Jones matrices
  inline void invalidateJ() {JValid_(currSpw())=false;};
  inline void validateJ()   {JValid_(currSpw())=true;};
  inline casacore::Bool JValid()      {return JValid_(currSpw());};

  // Row-by-row apply to a casacore::Cube<casacore::Complex> (applyByJones override)
  virtual void applyCal(VisBuffer& vb, casacore::Cube<casacore::Complex>& Vout,casacore::Bool trial=false);
  virtual void applyCal2(vi::VisBuffer2& vb, 
			 casacore::Cube<casacore::Complex>& Vout,casacore::Cube<casacore::Float>& Wout,
			 casacore::Bool trial=false);

  // Sync matrices for current meta data (VisJones override)
  virtual void syncCalMat(const casacore::Bool& doInv=false);

  // Calculate an ensemble of Mueller matrices (all baselines, channels)
  //  (only meaningful if applyByMueller()=T)
  virtual void calcAllMueller();

  // Synchronize current Jones matrices
  virtual void syncJones(const casacore::Bool& doInv=false);

  // Calculate an ensemble of Jones matrices (all antennas, channels)
  virtual void calcAllJones();

  // Calculate a single Jones matrix by some means from parameters
  virtual void calcOneJones(casacore::Vector<casacore::Complex>& mat, casacore::Vector<casacore::Bool>& mOk, 
			    const casacore::Vector<casacore::Complex>& par, const casacore::Vector<casacore::Bool>& pOk );

  // Invert Jones matrices
  virtual void invJones();

  // Set matrix elements according to their ok flags
  //  (e.g., makes a unit matrix if everything flagged,
  //   so we don't have to make atomic ok checks in apply)
  virtual void setMatByOk();

  // Create Jones interface
  void createJones();

  // Synchronize weight scale factors
  virtual void syncWtScale();

  // Perform weight scale calculation (specializable)
  virtual void calcWtScale();

  // Update the wt vector for a baseline
  virtual void updateWt(casacore::Vector<casacore::Float>& wt,const casacore::Int& a1,const casacore::Int& a2);
  virtual void updateWt2(casacore::Matrix<casacore::Float>& wt,const casacore::Int& a1,const casacore::Int& a2);

private:

  // Default ctor is private
  VisJones();

  // Init VisJones pointers
  void initVisJones();

  // Delete the this* caches
  void deleteVisJones();

  // Jones algebra wrapper (per Spw)
  casacore::PtrBlock<Jones*> J1_;
  casacore::PtrBlock<Jones*> J2_;

  // Current Jones matrix-element arrays
  casacore::PtrBlock<casacore::Cube<casacore::Complex>*> currJElem_;    // [nSpw](nJElem,nChanMat,nAnt)
  casacore::PtrBlock<casacore::Cube<casacore::Bool>*>    currJElemOK_;  // [nSpw](nJElem,nChanMat,nAnt)

  // Jones validity, per spw
  casacore::Vector<casacore::Bool> JValid_;


};

} //# NAMESPACE CASA - END

#endif
