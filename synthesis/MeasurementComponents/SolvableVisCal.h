//# SolvableVisCal.h: Definitions of interface for SolvableVisCal 
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

#ifndef SYNTHESIS_SOLVABLEVISCAL_H
#define SYNTHESIS_SOLVABLEVISCAL_H

#include <casa/aips.h>
#include <casa/Containers/Record.h>
#include <casa/BasicSL/Complex.h>
#include <casa/BasicSL/Constants.h>
#include <synthesis/MeasurementComponents/VisCal.h>
#include <synthesis/MeasurementComponents/CalCorruptor.h>
#include <synthesis/MeasurementComponents/Mueller.h>
#include <synthesis/MeasurementComponents/Jones.h>
#include <synthesis/MeasurementComponents/VisVector.h>
#include <synthesis/TransformMachines/SynthesisError.h>
#include <synthesis/CalTables/NewCalTable.h>
#include <synthesis/CalTables/CLPatchPanel.h>
#include <synthesis/CalTables/CTPatchedInterp.h>
#include <synthesis/CalTables/CalSetMetaInfo.h>
#include <synthesis/CalTables/VisCalEnum.h>
#include <msvis/MSVis/VisSet.h>
#include <msvis/MSVis/CalVisBuffer.h>
#include <msvis/MSVis/VisBuffGroupAcc.h>

#include <casa/Logging/LogMessage.h>
#include <casa/Logging/LogSink.h>
#include <casa/Logging/LogIO.h>
#include <casa/OS/Timer.h>
#if ! defined(CASATOOLS)
#include <casadbus/plotserver/PlotServerProxy.h>
#include <casadbus/utilities/BusAccess.h>
#include <casadbus/session/DBusSession.h>
#endif

#include <iostream>
#include <fstream>

namespace casa { //# NAMESPACE CASA - BEGIN


// **********************************************************
//  SolvableVisCal
//

// Forward
class VisEquation;
class SolveDataBuffer;
class SDBList;

class SolNorm {

public:

  enum Type { MEAN, MEDIAN, UNKNOWN };

  SolNorm(casacore::Bool donorm=false, casacore::String normtype=casacore::String("mean"));
  SolNorm(const SolNorm& other);

  inline casacore::Bool donorm() const { return donorm_; };
  inline Type normtype() const { return normtype_; };
  inline casacore::String normtypeString() const { return normTypeAsString(normtype_); };

  void report();

private:

  // data 
  casacore::Bool donorm_;
  Type normtype_;

  static Type normTypeFromString(casacore::String name);
  static casacore::String normTypeAsString(Type type);

  
};
 
class SolvableVisCal : virtual public VisCal {
public:

  typedef struct fluxScaleStruct {
    casacore::Matrix<casacore::Double> fd;
    casacore::Matrix<casacore::Double> fderr;
    casacore::Matrix<casacore::Int> numSol;
    casacore::Vector<casacore::Double> freq;
    casacore::Matrix<casacore::Double> spidx;
    casacore::Matrix<casacore::Double> spidxerr;
    casacore::Vector<casacore::Double> fitfd;
    casacore::Vector<casacore::Double> fitfderr;
    casacore::Vector<casacore::Double> fitreffreq;
    //casacore::Matrix<casacore::Double> covarmat;
    //casacore::PtrBlock<casacore::Matrix<casacore::Double>* >  covarmat;
    casacore::Vector<casacore::Matrix<casacore::Double> >  covarmat;
  } fluxScaleStruct;


  SolvableVisCal(VisSet& vs);
  
  SolvableVisCal(casacore::String msname,casacore::Int MSnAnt,casacore::Int MSnSpw);

  SolvableVisCal(const MSMetaInfoForCal& msmc);

  SolvableVisCal(const casacore::Int& nAnt);

  virtual ~SolvableVisCal();

  // Access to user-supplied parameters
  inline casacore::String&      calTableName()   { return calTableName_; };
  inline casacore::String&      calTableSelect() { return calTableSelect_; };
  inline casacore::Bool&        append()         { return append_; };
  inline casacore::String&      tInterpType()    { return tInterpType_; };
  inline casacore::String&      fInterpType()    { return fInterpType_; };
  inline casacore::Vector<casacore::Int>& spwMap()         { return spwMap_; };
  inline casacore::String&      refantmode()     { return refantmode_; };
  inline casacore::Int&         refant()         { return refantlist()(0); };
  inline casacore::Vector<casacore::Int>& refantlist()     { return urefantlist_; };
  inline casacore::Int&         minblperant()    { return minblperant_; };
  inline casacore::String&      apmode()         { return apmode_; };
  inline casacore::String&      solmode()        { return solmode_; };
  inline casacore::Vector<casacore::Float>& rmsthresh()      { return rmsthresh_; };
  inline casacore::String&      solint()         { return solint_; };
  inline casacore::String&      fsolint()        { return fsolint_; };
  inline casacore::Double&      preavg()         { return preavg_; };
  inline const SolNorm&         solNorm()        { return solnorm_;};
  inline casacore::Bool         solnorm()        { return solnorm_.donorm();};
  inline casacore::Float&       minSNR()         { return minSNR_; };

  inline casacore::String&      combine()        { return combine_; };
  inline casacore::Bool         combspw()        { return upcase(combine_).contains("SPW"); };
  inline casacore::Bool         combfld()        { return upcase(combine_).contains("FIELD"); };
  inline casacore::Bool         combscan()       { return upcase(combine_).contains("SCAN"); };
  inline casacore::Bool         combobs()        { return upcase(combine_).contains("OBS"); };

  // Total number of (complex) parameters per solve
  //  (specialize to jive with ant- or bln-basedness, etc.)
  virtual casacore::Int nTotalPar()=0;

  // Report if calibration available for specified spw
  //  (if no CalInterp available, assume true)

  // Use generic data gathering mechanism for solve
  virtual casacore::Bool useGenericGatherForSolve() { return true; };

  // Use generic solution engine for a single solve
  //  (usually inside the generic gathering mechanism)
  virtual casacore::Bool useGenericSolveOne() { return useGenericGatherForSolve(); };

  // Solve for point-source X or Q,U?
  //  nominally no (0)
  virtual casacore::Int solvePol() { return 0; };

  // Does normalization by MODEL_DATA commute with this VisCal?
  //   (if so, permits pre-solve time-averaging)
  virtual casacore::Bool normalizable()=0;

  // Should data and model be divided by Stokes I model  before average+solve?
  //   (Nominally false, for now)
  virtual casacore::Bool divideByStokesIModelForSolve() { return false; };

  // Is this type capable of accumulation?  (nominally no)
  virtual casacore::Bool accumulatable() { return false; };

  // Is this type capable of smoothing?  (nominally no)
  virtual casacore::Bool smoothable() { return false; };

  // Should only parallel-hands be used in solving?
  //  (generally no (default=false), but GJones and related
  //   will override, and eventually this will be a user-set-able
  //   parameter)
  virtual casacore::Bool phandonly() { return false; }

  // Access to focus channel
  inline casacore::Int&         focusChan()      { return focusChan_; };

  // Is this ready to solve?
  inline casacore::Bool isSolved() {return solved_;};

  // Is this solveable? (via this interface, nominally yes)
  virtual casacore::Bool isSolvable() {return true;};

  // Set the application parameters 
  virtual void setApply();
  virtual void setApply(const casacore::Record& apply);
  virtual void setCallib(const casacore::Record& callib,const casacore::MeasurementSet& selms);

  virtual void setModel(const casacore::String& )
  {throw(SynthesisError("Internal error: setModel() not yet supported for non EPJones type."));};

  // Report apply info/params, e.g. for logging
  virtual casacore::String applyinfo();

  // Set the solving parameters
  virtual void setSolve();
  virtual void setSolve(const casacore::Record& solve);

  // Report solve info/params, e.g., for logging
  virtual casacore::String solveinfo();

  // Arrange for accumulation
  virtual void setAccumulate(VisSet& vs,
			     const casacore::String& table,
			     const casacore::String& select,
			     const casacore::Double& t,
			     const casacore::Int& refAnt=-1);

  // Default value for parameters
  virtual casacore::Complex defaultPar() { return casacore::Complex(1.0); };
  virtual casacore::Float defaultRPar() { return casacore::Float(0.0); };
  virtual casacore::Complex defaultCPar() { return casacore::Complex(1.0); };

  // Arrange to build a cal table from specified values
  virtual void setSpecify(const casacore::Record& specify);

  // Fill a caltable with specified values
  virtual void specify(const casacore::Record& specify);

  // Size up the solving arrays, etc.  (supports combine)
  virtual casacore::Int sizeUpSolve(VisSet& vs, casacore::Vector<casacore::Int>& nChunkPerSol);

  // These inflate methods soon to deprecate (gmoellen, 20121212)
  // Inflate the pristine CalSet (from VisSet info)
  void inflate(VisSet& vs, const casacore::Bool& fillMeta=false);
  // Inflate the pristine CalSet (generically)
  virtual void inflate(const casacore::Vector<casacore::Int>& nChanDat,
		       const casacore::Vector<casacore::Int>& startChanDat,
		       const casacore::Vector<casacore::Int>& nSlot);


  // Hazard a guess at the parameters (solveCPar) given the data
  virtual void guessPar(VisBuffer& vb)=0;
  virtual void guessPar(SDBList&,const casacore::Bool&) { throw(casacore::AipsError("SVC::guessPar(SDBList&) NYI!!")); };  // VI2

  // Time-dep solution interval  (VI2)
  inline double solTimeInterval() const { return solTimeInterval_; };

  // Freq-dep solint values (const?!)
  inline casacore::Double& fintervalHz() { return fintervalHz_; };
  inline casacore::Double& fintervalCh() { return fintervalCh_(currSpw()); };  // for current Spw
  const inline casacore::Vector<casacore::Double>& fintervalChV() { return fintervalCh_; };  // all spws
  casacore::Matrix<casacore::Int> chanAveBounds()  { return chanAveBounds_(currSpw()); }; // for current Spw
  casacore::Matrix<casacore::Int> chanAveBounds(casacore::Int spw)  { return chanAveBounds_(spw); }; 

  // Access to current solution parameters and matrices
  inline virtual casacore::Cube<casacore::Complex>& solveCPar()   {return (*solveCPar_[currSpw()]);};
  inline virtual casacore::Cube<casacore::Float>&   solveRPar()   {return (*solveRPar_[currSpw()]);};
  inline virtual casacore::Cube<casacore::Bool>&    solveParOK()  {return (*solveParOK_[currSpw()]);};
  inline virtual casacore::Cube<casacore::Float> &  solveParErr() {return (*solveParErr_[currSpw()]);};
  inline virtual casacore::Cube<casacore::Float> &  solveParSNR() {return (*solveParSNR_[currSpw()]);};

  inline virtual casacore::Cube<casacore::Complex>& solveAllCPar()   {return (*solveAllCPar_[currSpw()]);};
  inline virtual casacore::Cube<casacore::Float>&   solveAllRPar()   {return (*solveAllRPar_[currSpw()]);};
  inline virtual casacore::Cube<casacore::Bool>&    solveAllParOK()  {return (*solveAllParOK_[currSpw()]);};
  inline virtual casacore::Cube<casacore::Float> &  solveAllParErr() {return (*solveAllParErr_[currSpw()]);};
  inline virtual casacore::Cube<casacore::Float> &  solveAllParSNR() {return (*solveAllParSNR_[currSpw()]);};

  // Access to per-spw solution parameters and matrices
  inline virtual void solveAllCPar(casacore::Int spw, casacore::Cube<casacore::Complex>& cparSpw)
  	{ if (spw<nSpw()) cparSpw = (*solveAllCPar_[spw]);}
  inline virtual void solveAllRPar(casacore::Int spw, casacore::Cube<casacore::Float>& rparSpw)
    { if (spw<nSpw()) rparSpw = (*solveAllRPar_[spw]);}
  inline virtual void solveAllParOK(casacore::Int spw, casacore::Cube<casacore::Bool>& parokSpw)
    { if (spw<nSpw()) parokSpw = (*solveAllParOK_[spw]);}
  inline virtual void solveAllParErr(casacore::Int spw, casacore::Cube<casacore::Float>& parerrSpw) 
    { if (spw<nSpw()) parerrSpw = (*solveAllParErr_[spw]);}
  inline virtual void solveAllParSNR(casacore::Int spw, casacore::Cube<casacore::Float>& parsnrSpw) 
    { if (spw<nSpw()) parsnrSpw = (*solveAllParSNR_[spw]);}

  // Access to source pol parameters
  inline casacore::Vector<casacore::Complex>& srcPolPar() { return srcPolPar_; };

  // Synchronize the meta data with a solvable VisBuffer
  //   (returns false if VisBuffer has no valid data)
  casacore::Bool syncSolveMeta(VisBuffer& vb, const casacore::Int& fieldId);
  casacore::Bool syncSolveMeta(VisBuffGroupAcc& vbga);
  void syncSolveMeta(SDBList& sdbs);  // VI2   (valid data now checked elsewhere)
  // Provide for override of currScan and currObs
  void overrideObsScan(casacore::Int obs, casacore::Int scan);

  // If apmode() is "A", convert vb's visibilities to amp + 0i.
  // If it is "P", convert them to phase + 0i.
  // Otherwise (i.e. "AP"), leave them alone.
  virtual void enforceAPonData(VisBuffer& vb);

  // Verify VisBuffer data sufficient for solving (wts, etc.)
  virtual casacore::Bool verifyConstraints(VisBuffGroupAcc& vbag);
  virtual casacore::Bool verifyConstraints(SDBList& sdbs);  // VI2 
  virtual casacore::Bool verifyForSolve(VisBuffer& vb);
  
  // Self- gather and/or solve prototypes
  //  (triggered by useGenericGatherForSolve=F or useGenericSolveOne=F; 
  //   must be overridden in derived specializations)
  virtual void selfGatherAndSolve(VisSet& vs, VisEquation& ve);
  virtual void selfSolveOne(VisBuffGroupAcc& vs);
  virtual void selfSolveOne(SDBList&) { throw(casacore::AipsError("selfSolveOne for VI2/SDB usage NYI for "+typeName())); };

  // Set up data and model for pol solve
  void setUpForPolSolve(VisBuffer& vb);
  // A VB2-specific version which depends on local specialization
  //   (this may be further refined later)
  virtual void setUpForPolSolve(vi::VisBuffer2&) {
    throw(casacore::AipsError("setUpForPolSolve(VB2) not specialized in "+typeName()+" as required.")); };

  // Differentiate VB model w.r.t. Cal  parameters (no 2nd derivative yet)
  virtual void differentiate(CalVisBuffer& cvb)=0;
  virtual void differentiate(SolveDataBuffer&) { throw(casacore::AipsError("SVC::differentiate(SDB)  NYI!")); };  // VI2
  virtual void differentiate(VisBuffer& vb,        
			     casacore::Cube<casacore::Complex>& V,     
			     casacore::Array<casacore::Complex>& dV,
			     casacore::Matrix<casacore::Bool>& Vflg)=0;
  virtual void differentiate(VisBuffer& ,          // vb.visCube() has the obs. data.  vb.modelVisCube() will receive the residuals
                             VisBuffer&   ,       // 1st. Derivative w.r.t. first parameter
                             VisBuffer& ,         // 1st. Derivative w.r.t. second parameter
                             casacore::Matrix<casacore::Bool>& ){ throw(casacore::AipsError("Invalid use of differentiate(vb,dV0,dv1)")); };


  // Differentiate VB model w.r.t. Source parameters
  virtual void diffSrc(VisBuffer& vb,        
		       casacore::Array<casacore::Complex>& dV)=0;

  // Update solve parameters incrementally (additive)
  virtual void updatePar(const casacore::Vector<casacore::Complex> dCalPar,const casacore::Vector<casacore::Complex> dSrcPar);
  virtual void updatePar(const casacore::Vector<casacore::Complex> dCalPar);  //  (VI2)

  // Form solution SNR
  virtual void formSolveSNR();

  // Apply SNR threshold
  virtual void applySNRThreshold();

  // Apply refant (implemented in SVJ)
  virtual void reReference()=0;

  // Retrieve the cal flag info as a record
  virtual casacore::Record actionRec();

  // Retrieve solve-related info via Record
  virtual casacore::Record solveActionRec();

  // Accumulate another VisCal onto this one
  virtual void accumulate(SolvableVisCal* incr,
			  const casacore::Vector<casacore::Int>& fields)=0;

  virtual void smooth(casacore::Vector<casacore::Int>& fields,
		      const casacore::String& smtype,
		      const casacore::Double& smtime);

  // Report solved-for QU
  virtual void reportSolvedQU();


  // New CalTable handling
  virtual void createMemCalTable();
  virtual void keep1(casacore::Int ichan);
  virtual void keepNCT();
  virtual void storeNCT();
  void storeNCT(const casacore::String& tableName,const casacore::Bool& append);

  virtual void loadMemCalTable(casacore::String ctname,casacore::String field="");

  // New spwOK
  virtual casacore::Bool spwOK(casacore::Int ispw);

  //Is VB OK for calibration? 
  virtual casacore::Bool VBOKforCalApply(vi::VisBuffer2& vb);

  // Calibration available?
  virtual casacore::Bool calAvailable(vi::VisBuffer2&);

  // Post solve tinkering (generic version)
  virtual void globalPostSolveTinker();

  // Divide all solutions by their amplitudes
  virtual void enforceAPonSoln();

  // Normalize a solution (generic implementation)
  virtual void normalize();

  // Determine and apply flux density scaling
  virtual void fluxscale(const casacore::String& outfile,
                         const casacore::Vector<casacore::Int>& refFieldIn,
			 const casacore::Vector<casacore::Int>& tranFieldIn,
			 const casacore::Vector<casacore::Int>& inRefSpwMap,
			 const casacore::Vector<casacore::String>& fldNames,
                         const casacore::Float& inGainThres,
                         const casacore::String& antSel,
                         const casacore::String& timerangeSel,
                         const casacore::String& scanSel,
			 fluxScaleStruct& oFluxScaleStruct,
			 const casacore::String& oListFile,
                         const casacore::Bool& incremental,
                         const casacore::Int& fitorder,
                         const casacore::Bool& display)=0;

  // Report state:
  inline virtual void state() { stateSVC(true); };

  virtual VisCalEnum::VCParType setParType(VisCalEnum::VCParType type) 
  {parType_ = type;return (VisCalEnum::VCParType)parType_;};
  virtual void currMetaNote();

  virtual void listCal(const casacore::Vector<casacore::Int> ufldids, const casacore::Vector<casacore::Int> uantids,
		       const casacore::Matrix<casacore::Int> uchanids,  //const casacore::Int& spw, const casacore::Int& chan,
		       const casacore::String& listfile="",const casacore::Int& pagerows=50)=0;

  // Handle external channel mask
  inline void setChanMask(casacore::PtrBlock<casacore::Vector<casacore::Bool>*>& chanmask) { chanmask_=&chanmask; };
  inline void clearChanMask() { chanmask_=NULL; };
  void applyChanMask(VisBuffer& vb);
  // Log periodic solver activity
  virtual void printActivity(const casacore::Int nSlots, const casacore::Int slotNo, 
			     const casacore::Int fieldId, const casacore::Int spw, 
			     const casacore::Int nSolutions);
  virtual void markTimer() {timer_p.mark();};


  // -------------
  // Set the simulation parameters
  virtual void setSimulate(VisSet& vs, casacore::Record& simpar, casacore::Vector<casacore::Double>& solTimes);

  // make a corruptor in a VC-specific way
  virtual void createCorruptor(const VisIter& vi,const casacore::Record& simpar, const int nSim);

  // access to simulation variables that are general to all VisCals
  inline casacore::String& simint() { return simint_; };

  // Simulation info/params, suitable for logging
  virtual casacore::String siminfo();

  // Is this calibration simulated?
  inline casacore::Bool isSimulated() {return simulated_;};

  // object that can simulate the corruption terms
  CalCorruptor *corruptor_p;

  // calculate # required slots to simulate this SVC
  casacore::Int sizeUpSim(VisSet& vs, casacore::Vector<casacore::Int>& nChunkPerSol, casacore::Vector<casacore::Double>& solTimes);


  // VI2-related refactor--------------------------------------

  // Set "current" meta info, so internals are registered
  //  (VI2: replaces meta-inf part of syncSolveMeta; 
  //   NB: does _NOT_ check sum(wt)>0 older syncSolveMeta!)
  virtual void setMeta(int obs, int scan, double time,
		       int spw, const casacore::Vector<double>& freq,
		       int fld);

  // Reshape solvePar* arrays for the currSpw()  
  //  (ensitive to freqDepPar())
  //  (VI2: replaces initSolvePar part of sizeUpSolve)
  virtual int sizeSolveParCurrSpw(int nVisChan);

  // Set parameters to def values in the currSpw(), 
  //   and optionally sync everything
  virtual void setDefSolveParCurrSpw(bool sync=false);

  // Parse solint in VI2 context
  void reParseSolintForVI2();

  // Generate the in-memory caltable (empty)
  //  NB: no subtable revisions
  virtual void createMemCalTable2();

  // Set (or verify) freq info in output cal table for specified spw
  virtual void setOrVerifyCTFrequencies(int spw);


protected:

  // Set to-be-solved-for flag
  inline void setSolved(const casacore::Bool& flag) {solved_=flag;};

  // Initialize solve parameters (shape)
  virtual void initSolvePar()=0;

  // Invalidate diff cal matrices generically 
  inline virtual void invalidateDiffCalMat()=0;

  // Explicitly synchronize pars with a CalSet slot
  using VisCal::syncPar;

  // Set matrix channelization according to a VisSet
  virtual void setSolveChannelization(VisSet& vs);

  // Convert Hz to Ch in fsolint
  virtual void convertHzToCh();

  // Calculate chan averaging bounds
  virtual void setFracChanAve();

  // Inflate an NCT w/ meta-data according to a VisSet 
  //   (for accum)
  void inflateNCTwithMetaData(VisSet& vs);

  // Synchronize calibration for solve context
  void syncSolveCal();

  // Synchronize parameters for solve context
  void syncSolvePar();

  // Calculate parameters by some means (e.g., interpolation from a CalSet)
  virtual void calcPar();
  virtual void calcParByCLPP();

  // Synchronize the differentiated calibration 
  virtual void syncDiffMat()=0;

  // Report the SVC-specific state, w/ option for VC::state()
  virtual void stateSVC(const casacore::Bool& doVC);

  // Normalize a (complex) solution array (generic)
  casacore::Complex normSolnArray(casacore::Array<casacore::Complex>& sol,
				  const casacore::Array<casacore::Bool>& solOK,
				  const casacore::Bool doPhase=false);

  virtual casacore::Float calcPowerNorm(casacore::Array<casacore::Float>& amp, const casacore::Array<casacore::Bool>& ok)=0;

  // Logger
  casacore::LogIO& logSink() { return logsink_p; };

  // Check if a cal table is appropriate
  void verifyCalTable(const casacore::String& caltablename);

  void sortVisSet(VisSet& vs, const casacore::Bool verbose=false);

  casacore::Int parType_;

  // New CalTable 
  NewCalTable *ct_;
  CTPatchedInterp *ci_;
  CLPatchPanel *cpp_;
  casacore::Vector<casacore::Bool> spwOK_;

  casacore::Double maxTimePerSolution_p, minTimePerSolution_p, avgTimePerSolution_p;
  casacore::Float userPrintActivityInterval_p, userPrintActivityFraction_p;
  casacore::uInt caiRC_p, cafRC_p;
  casacore::Timer timer_p;

  // Set state flag to simulate cal terms
  inline void setSimulated(const casacore::Bool& flag) {simulated_=flag;};

  // RI todo implement calcOneJones like calcAllMueller
  // calculate terms during apply, or up front during setSim?
  inline casacore::Bool& simOnTheFly() { 
    //    cout << "simOTF=" << onthefly_ << endl;
    return onthefly_; };
    



private:

  // Default ctor is private
  SolvableVisCal();

  // Initialize pointers, etc.
  void initSVC();

  // Delete pointers
  void deleteSVC();

  // Pointer to CTTimeInterp1 factory method (generic)
  // SVC specializations may choose to specialize CTTimeInterp1, as needed,
  //   and override this method accordingly (e.g., see FringeJones.h)
  virtual CTTIFactoryPtr cttifactoryptr() { return &CTTimeInterp1::factory; };

  // Cal table name
  casacore::String calTableName_;
  casacore::String calTableSelect_;
  casacore::Bool append_;

  // Interpolation types
  casacore::String tInterpType_;
  casacore::String fInterpType_;

  // Spw mapping
  casacore::Vector<casacore::Int> spwMap_;

  // Refant mode
  casacore::String refantmode_;

  // Refant
  casacore::Vector<casacore::Int> urefantlist_;

  // Min baselines per ant for solve
  casacore::Int minblperant_;

  // Solved-for flag
  casacore::Bool solved_;

  // Signal apply by callib
  casacore::Bool byCallib_;

  // Solving mode
  casacore::String apmode_;

  // Solver iteration mode
  casacore::String solmode_;
  casacore::Vector<casacore::Float> rmsthresh_;

  // User-specified full solint string
  casacore::String usolint_;

  // User-specified time-dep solint (string)
  casacore::String solint_;

  // Derived time-dep solution interval (s) (VI2)
  double solTimeInterval_;

  // User-specified freq-dep solint info
  casacore::String fsolint_;

  // Derived frequency intervals
  casacore::Double fintervalHz_;
  casacore::Vector<casacore::Double> fintervalCh_;   // (nSpw)


  // Channel averaging bounds
  casacore::Vector<casacore::Matrix<casacore::Int> > chanAveBounds_;  // (nSpw)(2,nOutChan)

  // Preavering interval
  casacore::Double preavg_;

  // Do solution normalization after a solve
  SolNorm solnorm_;

  // SNR threshold
  casacore::Float minSNR_;

  // axes to combine for solve
  casacore::String combine_;

  // In-focus channel for single-chan solves on multi-chan data
  casacore::Int focusChan_;

  // Solving meta-data
  casacore::Double dataInterval_;
  casacore::Double fitWt_;
  casacore::Double fit_;


  // Current parameters
  casacore::PtrBlock<casacore::Cube<casacore::Complex>*> solveCPar_;  // [nSpw](nPar,1,{1|nElem})
  casacore::PtrBlock<casacore::Cube<casacore::Float>*>   solveRPar_;  // [nSpw](nPar,1,{1|nElem})
  casacore::PtrBlock<casacore::Cube<casacore::Bool>*>    solveParOK_; // [nSpw](nPar,1,{1|nElm})
  casacore::PtrBlock<casacore::Cube<casacore::Float>*>   solveParErr_; // [nSpw](nPar,1,{1|nElm})
  casacore::PtrBlock<casacore::Cube<casacore::Float>*>   solveParSNR_; // [nSpw](nPar,1,{1|nElm})

  casacore::PtrBlock<casacore::Cube<casacore::Complex>*> solveAllCPar_;   // [nSpw](nPar,nChan,{1|nElem})
  casacore::PtrBlock<casacore::Cube<casacore::Float>*>   solveAllRPar_;   // [nSpw](nPar,nChan,{1|nElem})
  casacore::PtrBlock<casacore::Cube<casacore::Bool>*>    solveAllParOK_;  // [nSpw](nPar,nChan,{1|nElm})
  casacore::PtrBlock<casacore::Cube<casacore::Float>*>   solveAllParErr_; // [nSpw](nPar,nChan,{1|nElm})
  casacore::PtrBlock<casacore::Cube<casacore::Float>*>   solveAllParSNR_; // [nSpw](nPar,nChan,{1|nElm})

  casacore::Vector<casacore::Complex> srcPolPar_;

  // A _pointer_ to the external channel mask
  casacore::PtrBlock<casacore::Vector<casacore::Bool>*> *chanmask_;

  // LogIO
  casacore::LogIO logsink_p;

  // Simulation flag
  casacore::Bool simulated_;

  // simulation interval
  casacore::String simint_;

  casacore::Bool onthefly_;  

};



// **********************************************************
//  SolvableVisMueller
//

class SolvableVisMueller : public SolvableVisCal, virtual public VisMueller
{

public:

  SolvableVisMueller(VisSet& vs);

  SolvableVisMueller(casacore::String msname,casacore::Int MSnAnt,casacore::Int MSnSpw);

  SolvableVisMueller(const MSMetaInfoForCal& msmc);

  SolvableVisMueller(const casacore::Int& nAnt);

  virtual ~SolvableVisMueller();

  // Total number of (complex) parameters per solve
  //  Mueller version:  just return number of per-Bln parameters
  virtual casacore::Int nTotalPar() { return nPar(); };

  // Does normalization by MODEL_DATA commute with this VisCal?
  virtual casacore::Bool normalizable() { return (this->muellerType() < Mueller::General); };

  // Hazard a guess at the parameters (solvePar) given the data
  virtual void guessPar(VisBuffer& ) { throw(casacore::AipsError("NYI")); };

  // Differentiate VB model w.r.t. Mueller parameters (no 2nd derivative yet)
  virtual void differentiate(CalVisBuffer& ) {throw(casacore::AipsError("SVM::differentiate(CVB): NYI")); };
  virtual void differentiate(SolveDataBuffer& ) {throw(casacore::AipsError("SVM::differentiate(SDB): NYI")); };  // VI2
  virtual void differentiate(VisBuffer& ,          // input data
			     casacore::Cube<casacore::Complex>& ,       // trial apply (nCorr,nChan,nRow)
			     casacore::Array<casacore::Complex>& ,     // 1st deriv   (nCorr,nPar,nChan,nRow)
			     casacore::Matrix<casacore::Bool>& ) { throw(casacore::AipsError("NYI")); };
  using SolvableVisCal::differentiate;

  // Differentiate VB model w.r.t. Source parameters
  virtual void diffSrc(VisBuffer& ,
		       casacore::Array<casacore::Complex>& ) {throw(casacore::AipsError("NYI")); };

  // Apply refant (no-op for Muellers)
  virtual void reReference() {};

  // Accumulate another VisCal onto this one
  virtual void accumulate(SolvableVisCal* ,
			  const casacore::Vector<casacore::Int>& ) { throw(casacore::AipsError("NYI")); };

  // Scale solutions
  virtual void fluxscale(const casacore::String&,
                         const casacore::Vector<casacore::Int>& ,
			 const casacore::Vector<casacore::Int>& ,
			 const casacore::Vector<casacore::Int>& ,
			 const casacore::Vector<casacore::String>& ,
                         const casacore::Float& ,
                         const casacore::String& ,
                         const casacore::String& ,
                         const casacore::String& ,
			 SolvableVisCal::fluxScaleStruct&,
			 const casacore::String&,
                         const casacore::Bool&,
                         const casacore::Int&,
                         const casacore::Bool&)
	{ throw(casacore::AipsError("NYI")); };

  // SVM-specific write to caltable
  virtual void keepNCT();

  // Report state:
  inline virtual void state() { stateSVM(true); };

  // casacore::List calibration solutions in tabular form.
  virtual void listCal(const casacore::Vector<casacore::Int> ,
                       const casacore::Vector<casacore::Int> ,
                       const casacore::Matrix<casacore::Int> ,
   		               const casacore::String& ,
                       const casacore::Int& )
  { throw(casacore::AipsError(casacore::String("Calibration listing not supported for "+typeName()))); };

protected:

  // Number of Cal Matrices to form on baseline axis
  //  (Mueller, solve context: 1)
  virtual casacore::Int nCalMat() { return isSolved() ? 1 : nBln(); };

  // Are differentiated M matrices constant in chan & bln?
  virtual casacore::Bool trivialDM() { return false; };

  // Initialize solve parameter shape
  //  Mueller version:  (nPar(),1,1)   (one chan, one baseline)
  virtual void initSolvePar();

  // Access to matrix rendering of dM (per par)
  inline Mueller& dM() { return *dM_; };

  // Access to differentiated Mueller elements
  inline casacore::Array<casacore::Complex>& diffMElem() {return diffMElem_;};

  // Invalidate diff cal matrices generically (at this level, just M, dM)
  inline virtual void invalidateDiffCalMat() { invalidateM(); invalidateDM(); };

  // Validation of diffMueller matrices
  inline void invalidateDM() {DMValid_=false;};
  inline void validateDM()   {DMValid_=true;};
  inline casacore::Bool DMValid()      {return DMValid_;};

  // Synchronize the differentiated calibration (specialization for Mueller);
  virtual void syncDiffMat();

  // Synchronize the Muellers AND diffMuellers
  virtual void syncDiffMueller();

  // Calculate the ensemble of diff'd Mueller Elements
  virtual void calcAllDiffMueller();

  // Calculate one diffMElem 
  virtual void calcOneDiffMueller(casacore::Matrix<casacore::Complex>& mat, const casacore::Vector<casacore::Complex>& par);

  // Create matrix renderers for dMs
  void createDiffMueller();

  // Override VM::setMatByOk in solve context
  void setMatByOk() { if (!isSolved()) VisMueller::setMatByOk(); };

  // Initialize trivial diff'd Muellers
  virtual void initTrivDM();

  // SVM-specific state
  virtual void stateSVM(const casacore::Bool& doVC);

  // atomic power normalization calculation
  //   Mueller version assumes amp in power units
  virtual casacore::Float calcPowerNorm(casacore::Array<casacore::Float>& amp, const casacore::Array<casacore::Bool>& ok);

private:

  // Default ctor is private
  SolvableVisMueller();

  // Mueller wrapper for diffMElem_;
  Mueller *dM_;

  // Differentiated Mueller matrix elements
  casacore::Array<casacore::Complex> diffMElem_;    // (nDMEl,nPar,nChanMat,nBln)

  // diffMueller validation
  casacore::Bool DMValid_;


};

// **********************************************************
//  SolvableVisJones
//

class SolvableVisJones : public SolvableVisMueller, public VisJones {

public:

  SolvableVisJones(VisSet& vs);

  SolvableVisJones(casacore::String msname,casacore::Int MSnAnt,casacore::Int MSnSpw);

  SolvableVisJones(const casacore::Int& nAnt);

  SolvableVisJones(const MSMetaInfoForCal& msmc);

  virtual ~SolvableVisJones();

  // Total number of (complex) parameters per solve
  //  Jones version:  nPar()*nAnt()
  virtual casacore::Int nTotalPar() { return nPar()*nAnt(); };

  // Does normalization by MODEL_DATA commute with this VisCal?
  virtual casacore::Bool normalizable() { return (this->jonesType() < Jones::GenLinear); };

  // Differentiate VB model w.r.t. Jones parameters
  virtual void differentiate(CalVisBuffer& cvb);
  virtual void differentiate(SolveDataBuffer& sdb);  // VI2
  virtual void differentiate(VisBuffer& vb,          // input data
			     casacore::Cube<casacore::Complex>& V,       // trial apply (nCorr,nChan,nRow)
			     casacore::Array<casacore::Complex>& dV,     // 1st deriv   (nCorr,nPar,nChan,nRow,2)
			     casacore::Matrix<casacore::Bool>& Vflg);
  using SolvableVisMueller::differentiate;

  // Differentiate VB model w.r.t. Source parameters
  virtual void diffSrc(VisBuffer& vb,        
		       casacore::Array<casacore::Complex>& dV);

  // Apply refant
  virtual void reReference();

  // Accumulate another VisCal onto this one
  virtual void accumulate(SolvableVisCal* incr,
			  const casacore::Vector<casacore::Int>& fields);

  // Post solve tinkering (Jones version: includes refant application)
  virtual void globalPostSolveTinker();

  // Apply reference antenna (generic Jones version)
  virtual void applyRefAnt();

  // Fluxscale is implemented here
  void fluxscale(const casacore::String& outfile,
                 const casacore::Vector<casacore::Int>& refFieldIn,
		 const casacore::Vector<casacore::Int>& tranFieldIn,
		 const casacore::Vector<casacore::Int>& inRefSpwMap,
		 const casacore::Vector<casacore::String>& fldNames,
                 const casacore::Float& inGainThres,
                 const casacore::String& antSel,
                 const casacore::String& timerangeSel,
                 const casacore::String& scanSel,
		 SolvableVisCal::fluxScaleStruct& oFluxScaleStruct,
		 const casacore::String& oListFile,
                 const casacore::Bool& incremental=false,
                 const casacore::Int& fitorder=1,
                 const casacore::Bool& display=false);

  // SVJ-specific write to caltable
  virtual void keepNCT();

  // Report state:
  inline virtual void state() { stateSVJ(true); };

  // Write calibration solutions to the terminal
  virtual void listCal(const casacore::Vector<casacore::Int> ufldids, const casacore::Vector<casacore::Int> uantids,
                       const casacore::Matrix<casacore::Int> uchanids,
                       const casacore::String& listfile="",const casacore::Int& pagerows=50);

  // Write header for listCal output
  int writeHeader(const casacore::uInt numAntCols, 
                  const casacore::uInt numAnts,
                  const casacore::uInt iElem);

  virtual void nearest(const casacore::Double , casacore::Array<casacore::Float>& ) {};
  virtual void nearest(const casacore::Double , casacore::Array<casacore::Complex>& ) {};

  //plotting historgram
  void setupPlotter();
  void plotHistogram(const casacore::String& title, const casacore::Int index,
                     const casacore::Vector<casacore::Double>& data, const casacore::Int nbin);

protected:
 
  // Number of Cal Matrices to form on baseline axis
  //  (Jones, all contexts: nAnt())
  virtual casacore::Int nCalMat() { return nAnt(); };

  // DM never trivial for SVJ
  virtual casacore::Bool trivialDM() { return false; };

  // Are differentiated J matrices constant in chan & ant?
  inline virtual casacore::Bool trivialDJ() { return false; };

  // Initialize solve parameter shape
  //  Jones version:  (nPar(),1,nAnt())   (one chan, all antennas)
  virtual void initSolvePar();

  // Access to matrix rendering of dJ1, dJ2
  inline Jones& dJ1() { return *dJ1_; };
  inline Jones& dJ2() { return *dJ2_; };

  // Access to differentiated Joness
  casacore::Array<casacore::Complex>& diffJElem() { return diffJElem_; };

  // Invalidate diff cal matrices generically (at this level, M, dM, J, dJ)
  inline virtual void invalidateDiffCalMat() { 
    SolvableVisMueller::invalidateDiffCalMat(); invalidateJ(); invalidateDJ(); };

  // Validation of Jones matrix derivatives
  inline void invalidateDJ() {DJValid_=false;};
  inline void validateDJ()   {DJValid_=true;};
  inline casacore::Bool DJValid()      {return DJValid_;};

  // Synchronize the differentiated calibration (specialization for Jones)
  virtual void syncDiffMat();

  // Synchronize the Jones AND diffJones matrices
  virtual void syncDiffJones();

  // Calculate the ensemble of diff'd Jones Elements
  virtual void calcAllDiffJones();

  // Calculate one ant/chan's diffJElem w.r.t. each par
  virtual void calcOneDiffJones(casacore::Matrix<casacore::Complex>& mat, const casacore::Vector<casacore::Complex>& par);

  // Create matrix renderers for dJs
  void createDiffJones();

  // Override VJ::setMatByOk in solve context
  void setMatByOk() { if (!isSolved()) VisJones::setMatByOk(); };

  // Initialize trivial diff'd Jones
  virtual void initTrivDJ();

  virtual void stateSVJ(const casacore::Bool& doVC);

  // atomic power normalization calculation
  //   SVJ version assumes amp in voltage units
  virtual casacore::Float calcPowerNorm(casacore::Array<casacore::Float>& amp, const casacore::Array<casacore::Bool>& ok);
			     
private:

  // Default ctor is private
  SolvableVisJones();

  // Jones wrappers for diffJElem_;
  Jones *dJ1_;
  Jones *dJ2_;

  // Differentiated Jones matrix elements
  casacore::Array<casacore::Complex> diffJElem_;    // (nJME,nPar,nChanMat,nAnt,2)

  // Validity of Jones matrix derivatives
  casacore::Bool DJValid_;

  // Column widths for listing
  casacore::uInt wTime_p,  wField_p, wChan_p, wAmp_p, 
       wPhase_p, wFlag_p,  wPol_p,  wAntCol_p, 
       wTotal_p, wPreAnt_p;

  //for plotting
#if ! defined(CASATOOLS)
  PlotServerProxy* plotter_;
  casacore::Vector<dbus::variant> panels_id_;
#endif

};

// Global methods

// Discern cal table type from the table itself
casacore::String calTableType(const casacore::String& tablename);

} //# NAMESPACE CASA - END

#endif
