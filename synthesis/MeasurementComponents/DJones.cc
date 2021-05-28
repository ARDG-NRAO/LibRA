//# DJones.cc: Implementation of polarization-related calibration types
//# Copyright (C) 1996,1997,1998,1999,2000,2001,2002,2003,2011
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

#include <synthesis/MeasurementComponents/DJones.h>
#include <synthesis/MeasurementComponents/CalCorruptor.h>
#include <synthesis/MeasurementComponents/MSMetaInfoForCal.h>
#include <synthesis/MeasurementComponents/SolveDataBuffer.h>
#include <msvis/MSVis/VisBuffer.h>
#include <msvis/MSVis/VisBuffAccumulator.h>
#include <synthesis/CalTables/CTIter.h>
#include <synthesis/MeasurementEquations/VisEquation.h>
#include <scimath/Fitting/LSQFit.h>
#include <scimath/Fitting/LinearFit.h>
#include <scimath/Functionals/CompiledFunction.h>
#include <scimath/Functionals/Polynomial.h>
#include <scimath/Mathematics/AutoDiff.h>
#include <casa/BasicMath/Math.h>
#include <tables/TaQL/ExprNode.h>

#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/MaskArrMath.h>
#include <casa/Arrays/MatrixMath.h>
#include <casa/BasicSL/String.h>
#include <casa/Utilities/Assert.h>
#include <casa/Utilities/GenSort.h>
#include <casa/Exceptions/Error.h>
#include <casa/OS/Memory.h>
#include <casa/System/Aipsrc.h>

#include <casa/sstream.h>

#include <measures/Measures/MCBaseline.h>
#include <measures/Measures/MDirection.h>
#include <measures/Measures/MEpoch.h>
#include <measures/Measures/MeasTable.h>

#include <casa/Logging/LogMessage.h>
#include <casa/Logging/LogSink.h>
// math.h ?

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN


// **********************************************************
//  DJones Implementations
//

DJones::DJones(VisSet& vs) :
  VisCal(vs),             // virtual base
  VisMueller(vs),         // virtual base
  SolvableVisJones(vs),   // immediate parent
  solvePol_(0)
{
  if (prtlev()>2) cout << "D::D(vs)" << endl;

}


DJones::DJones(String msname,Int MSnAnt,Int MSnSpw) :
  VisCal(msname,MSnAnt,MSnSpw),             // virtual base
  VisMueller(msname,MSnAnt,MSnSpw),         // virtual base
  SolvableVisJones(msname,MSnAnt,MSnSpw),   // immediate parent
  solvePol_(0)
{
  if (prtlev()>2) cout << "D::D(msname,MSnAnt,MSnSpw)" << endl;
}

DJones::DJones(const MSMetaInfoForCal& msmc) :
  VisCal(msmc),             // virtual base
  VisMueller(msmc),         // virtual base
  SolvableVisJones(msmc),   // immediate parent
  solvePol_(0)
{
  if (prtlev()>2) cout << "D::D(msmc)" << endl;
}


DJones::DJones(const Int& nAnt) :
  VisCal(nAnt), 
  VisMueller(nAnt),
  SolvableVisJones(nAnt),
  solvePol_(0)
{
  if (prtlev()>2) cout << "D::D(nAnt)" << endl;

}

DJones::~DJones() {
  if (prtlev()>2) cout << "D::~D()" << endl;
}

void DJones::setApply(const Record& apply) {

  SolvableVisJones::setApply(apply);

  // Force calwt to false for now
  calWt()=false;

}


void DJones::setSolve(const Record& solvepar) {

  // Call parent
  SolvableVisJones::setSolve(solvepar);

  // Determine if we are solving for source pol or not
  if (solvepar.isDefined("type")) {
    String type = solvepar.asString("type");
    if (type.contains("QU")) {
      solvePol_=2;
      logSink() << "Will solve for source polarization (Q,U)" << LogIO::POST;
    }
    else if (type.contains("X")) {
      solvePol_=1;
      logSink() << "Will solve for source polarization position angle correction" << LogIO::POST;
    }
    else
      solvePol_=0;
  }

  logSink() << "Using only cross-hand data for instrumental polarization solution." << LogIO::POST;
  
  // For D insist preavg is meaningful (5 minutes or user-supplied)
  if (preavg()<0.0)
    preavg()=300.0;

  /*
  cout << boolalpha;
  cout << endl << endl;
  cout << "useGenericGatherForSolve = " << useGenericGatherForSolve() << endl;
  cout << "useGenericSolveOne       = " << useGenericSolveOne() << endl;
  */

}


void DJones::calcOneJones(Vector<Complex>& mat, Vector<Bool>& mOk,
			  const Vector<Complex>& par, const Vector<Bool>& pOk) {

  if (prtlev()>10) cout << "   D::calcOneJones(vb)" << endl;

  // Only if both pars are good, form matrix
  if (allEQ(pOk,true)) {

    // On-diag = 1
    mat(0)=mat(3)=Complex(1.0);
    // Off-diag = par
    mat(1)=par(0);
    mat(2)=par(1);

    mOk=true;

  }
  else {
    mat=Complex(0.0);
    mOk=false;
  }
}

void DJones::guessPar(VisBuffer& vb) {

  if (prtlev()>4) cout << "   D::guessPar(vb)" << endl;

  // TBD: Should we use a common wt for the X-hands??

  // First guess is zero D-terms
  solveCPar()=0.0;
  solveParOK()=true;

  if (jonesType()==Jones::GenLinear) {
    vb.weightMat().row(0)=0.0;
    vb.weightMat().row(3)=0.0;
  }

  if (solvePol()) {

    // solvePol() tells us how many source pol parameters
    srcPolPar().resize(solvePol());

    // The following assumes the MODEL_DATA has been
    //  corrupted by P 

    LSQFit fit(2,LSQComplex());
    Vector<Complex> ce(2);
    ce(0)=Complex(1.0);
    Complex d,md;
    Float wt,a(0.0);
    for (Int irow=0;irow<vb.nRow();++irow) {
      if (!vb.flagRow()(irow)  &&
	  vb.antenna1()(irow)!=vb.antenna2()(irow)) {
	for (Int ich=0;ich<vb.nChannel();++ich) {
	  if (!vb.flag()(ich,irow)) {
	    for (Int icorr=1;icorr<3;++icorr) {
	      md=vb.modelVisCube()(icorr,ich,irow);
	      if (icorr==2) md=conj(md);
	      a=abs(md);
	      if (a>0.0) {
		wt=Double(vb.weightMat()(icorr,irow));
		if (wt>0.0) {
		  d=vb.visCube()(icorr,ich,irow);
		  if (icorr==2) d=conj(d);
		  if (abs(d)>0.0) {

		    ce(1)=md;
		    fit.makeNorm(ce.data(),wt,d,LSQFit::COMPLEX);

		  } // abs(d)>0
		} // wt>0
	      } // a>0
	    } // icorr
	  } // !flag
	} // ich
      } // !flagRow
    } // row

    uInt rank;
    Bool ok = fit.invert(rank);

    Complex sol[2];
    if (ok)
      fit.solve(sol);
    else
      throw(AipsError("Source polarization solution is singular; try solving for D-terms only."));

    if (solvePol()==1 && a>0.0) 
      srcPolPar()(0)=Complex(arg(sol[1]));
    else if (solvePol()==2) {
      srcPolPar()(0)=Complex(real(sol[1]));
      srcPolPar()(1)=Complex(imag(sol[1]));
    }

    // Log source polarization solution
    reportSolvedQU();


  } // solvePol()?

}


void DJones::setUpForPolSolve(vi::VisBuffer2& vb) {

  // Divide model and data by (scalar) stokes I model (which may be resolved!), 
  //  and set model cross-hands to (1,0) so we can solve for fractional
  //  pol factors.

  // NB: this is circ-basis-specific!!
  
  // NB: this method assumes vCC.set(vC)  (corrected data already set)

  // Only if solving for Q an U
  //  (leave cross-hands alone if just solving for X)
  if (solvePol()>1) {

    Int nCorr(vb.nCorrelations());
    Vector<Float> ampCorr(nCorr);
    Vector<Int> n(nCorr,0);
    Complex sI(0.0);
    for (rownr_t irow=0;irow<vb.nRows();++irow) {
      if (!vb.flagRow()(irow)) {
	ampCorr=0.0f;
	n=0;
	for (Int ich=0;ich<vb.nChannels();++ich) {

	  Vector<Bool> fl(vb.flagCube()(Slice(),Slice(ich,1,1),Slice(irow,1,1)));
	  Vector<Float> wt(vb.weightSpectrum()(Slice(),Slice(ich,1,1),Slice(irow,1,1)));
	  Vector<Complex> vCM(vb.visCubeModel()(Slice(),Slice(ich,1,1),Slice(irow,1,1)));
	  Vector<Complex> vCC(vb.visCubeCorrected()(Slice(),Slice(ich,1,1),Slice(irow,1,1)));

	  if (sum(fl)>0) {
	    // one or more correlations is flagged; cannot use
	    fl.set(true);
	    wt.set(0.0f);
	    vCC.set(0.0f);
	  }
	  else {
	    // all corrs ok, so do the calculation
	    sI=(vCM(0)+vCM(3))/Complex(2.0);
	    if (abs(sI)>0.0) {
	      vCC/=sI;
	      wt*=square(abs(sI));
	    }
	    else {
	      // model stokes I is zero, cannot use
	      fl.set(true);
	      wt.set(0.0f);
	      vCC.set(0.0f);
	    }
	  }
	  // Make model unity (so parang corruption works)
	  // NB: this is circ-basis specific!
	  // Q: Is this correct for p-hands?  (E.g., lin basis, or V!=0 in circ basis?)
	  vCM.set(Complex(1.0));

	} // ich
      } // !flagRow
    } // irow
  }

}

void DJones::guessPar(SDBList& sdbs,const casacore::Bool& corrDepFlags) {

  if (prtlev()>4) cout << "   D::guessPar(sdbs,corrDepFlags)" << endl;

  if (corrDepFlags)
    throw(AipsError("Hmmm, corrDepFlags=True in DJones::guessPar(sdbs,corrDepFlags"));

  // Call (old) corrDepFlags-agnostic version
  DJones::guessPar(sdbs);

}


void DJones::guessPar(SDBList& sdbs) {

  // TBD: Should we use a common wt for the X-hands??

  // First guess is zero D-terms
  solveCPar()=0.0;
  solveParOK()=true;

  if (jonesType()==Jones::GenLinear) {
    // Zero p-hand weights
    for (int isdb=0;isdb<sdbs.nSDB();++isdb) {
      SolveDataBuffer& sdb(sdbs(isdb));
      Cube<Float> wtS(sdb.weightSpectrum());
      wtS(Slice(0,2,3),Slice(),Slice())=0.0;
    }
  }

  if (solvePol()) {

    Int nSDB=sdbs.nSDB();

    Int nChan=sdbs.nChannels(); // insists on uniformity over SDBs

    // solvePol() tells us how many source pol parameters
    srcPolPar().resize(solvePol());

    // The following assumes the MODEL_DATA has been
    //  corrupted by P 

    LSQFit fit(2,LSQComplex());
    Vector<Complex> ce(2);
    ce(0)=Complex(1.0);
    Complex d,md;
    Float wt,a(0.0);
    for (int isdb=0;isdb<nSDB;++isdb) {
      SolveDataBuffer& sdb(sdbs(isdb));
    for (Int irow=0;irow<sdb.nRows();++irow) {
      if (!sdb.flagRow()(irow)  &&
	  sdb.antenna1()(irow)!=sdb.antenna2()(irow)) {
	for (Int ich=0;ich<nChan;++ich) {
	  for (Int icorr=1;icorr<3;++icorr) {
	    if (!sdb.flagCube()(icorr,ich,irow)) {
	      md=sdb.visCubeModel()(icorr,ich,irow);
	      if (icorr==2) md=conj(md);
	      a=abs(md);
	      if (a>0.0) {
		wt=Double(sdb.weightSpectrum()(icorr,ich,irow));
		if (wt>0.0) {
		  d=sdb.visCubeCorrected()(icorr,ich,irow);
		  if (icorr==2) d=conj(d);
		  if (abs(d)>0.0) {

		    ce(1)=md;
		    fit.makeNorm(ce.data(),wt,d,LSQFit::COMPLEX);

		  } // abs(d)>0
		} // wt>0
	      } // a>0
	    } // icorr
	  } // !flag
	} // ich
      } // !flagRow
    } // row
    } // isdb

    uInt rank;
    Bool ok = fit.invert(rank);

    Complex sol[2];
    if (ok)
      fit.solve(sol);
    else
      throw(AipsError("Source polarization solution is singular; try solving for D-terms only."));

    Complex Pol(1.0); // for updating the model
    if (solvePol()==1 && a>0.0)  {
      srcPolPar()(0)=Complex(arg(sol[1]));
      Pol=exp(Complex(0.0,real(srcPolPar()(0))));  // same as sol[1]?
    }
    else if (solvePol()==2) {
      srcPolPar()(0)=Complex(real(sol[1]));  // Q
      srcPolPar()(1)=Complex(imag(sol[1]));  // U
      Pol=Complex(real(srcPolPar()(0)),real(srcPolPar()(1)));  // same as real(sol[1]),imag(sol[1])?
    }

    // Log source polarization solution
    reportSolvedQU();

    // update model data with this source pol estimate
    Array<Complex> RL,LR;
    Complex cPol=conj(Pol); // for LR
    for (int isdb=0;isdb<nSDB;++isdb) {
      SolveDataBuffer& sdb(sdbs(isdb));
      RL.reference(sdb.visCubeModel()(Slice(1,1,1),Slice(),Slice())); // icorr=1
      RL*=Pol;
      LR.reference(sdb.visCubeModel()(Slice(2,1,1),Slice(),Slice())); // icorr=2
      LR*=cPol;
    }	
  } // solvePol()?

}


void DJones::updatePar(const Vector<Complex> dCalPar,
		       const Vector<Complex> dSrcPar) {

  // Enforce no change in source parameters 
  //  before calling generic version
  Vector<Complex> dsrcpar(dSrcPar.shape());
  dsrcpar=Complex(0.0);
  SolvableVisJones::updatePar(dCalPar,dsrcpar);

}

void DJones::formSolveSNR() {

  solveParSNR()=0.0;

  for (Int iant=0;iant<nAnt();++iant)
    for (Int ipar=0;ipar<nPar();++ipar) {
      if (solveParOK()(ipar,0,iant) &&
	  solveParErr()(ipar,0,iant)>0.0f) {
	solveParSNR()(ipar,0,iant)=1.0f/solveParErr()(ipar,0,iant);
      }
      else
	// Ensure F if Err<=0  (OK?)
	solveParOK()(ipar,0,iant)=false;
    }
}

void DJones::globalPostSolveTinker() {

  // call parent
  SolvableVisJones::globalPostSolveTinker();

  // if not freqdep, report results to the logger
  logResults();

}

						       
void DJones::applyRefAnt() {

  SolvableVisJones::applyRefAnt();
  return;

}

void DJones::logResults() {

  // Don't bother, if the Ds are channelized
  if (freqDepPar()) return;

  Vector<String> rl(2);
  rl(0)="R: ";
  rl(1)="L: ";

  MSAntennaColumns antcol(ct_->antenna());
  Vector<String> antNames(antcol.name().getColumn());

  logSink() << "The instrumental polarization solutions are: " << LogIO::POST;

  logSink().output().precision(4);

  Block<String> cols(3);
  cols[0]="SPECTRAL_WINDOW_ID";
  cols[1]="TIME";
  cols[2]="ANTENNA1";
  ROCTIter ctiter(*ct_,cols);

  Int lastspw(-1);
  Double lasttime(-1.0);
  while (!ctiter.pastEnd()) {
    Int ispw=ctiter.thisSpw();
    Double time=ctiter.thisTime();
    Int a1=ctiter.thisAntenna1();
    Vector<Complex> sol;
    sol=ctiter.cparam().xyPlane(0).column(0);
    Vector<Bool> fl;
    fl=ctiter.flag().xyPlane(0).column(0);

    if (ispw!=lastspw)    
      logSink() << " Spw " << ispw << ":" << endl;
    if (time !=lasttime)
      logSink() << " Time " << MVTime(time/C::day).string(MVTime::YMD,7) << ":" << endl;

    logSink().output().setf(ios::left, ios::adjustfield);

    logSink() << "  Ant=" << antNames(a1) << ": ";
    for (Int ipar=0;ipar<2;++ipar) {
      logSink() << rl(ipar);
      if (!fl(ipar)) {
	logSink() << "A="; 
	logSink().output().width(10);
	logSink() << abs(sol(ipar));
	logSink() << " P=";
	logSink().output().width(8);
	logSink() << arg(sol(ipar))*180.0/C::pi;
	if (ipar==0) logSink() << " ; ";
      }
      else {
	logSink().output().width(26);
	logSink() << "(flagged)" << " ";
      }
    } // ipol
    logSink() << endl;
    logSink() << LogIO::POST;

    lastspw=ispw;
    lasttime=time;
    ctiter.next();
  } // ctiter
}



// Fill the trivial DJ matrix elements
void DJones::initTrivDJ() {

  if (prtlev()>4) cout << "   D::initTrivDJ" << endl;

  // Must be trivial
  AlwaysAssert((trivialDJ()),AipsError);

  //  0 1     0 0
  //  0 0  &  1 0

  if (jonesType()==Jones::General) {
    diffJElem().resize(IPosition(4,4,2,1,1));
    diffJElem()=0.0;
    diffJElem()(IPosition(4,1,0,0,0))=Complex(1.0);
    diffJElem()(IPosition(4,2,1,0,0))=Complex(1.0);
  }
  else {
    diffJElem().resize(IPosition(4,2,2,1,1));
    diffJElem()=0.0;
    diffJElem()(IPosition(4,0,0,0,0))=Complex(1.0);
    diffJElem()(IPosition(4,1,1,0,0))=Complex(1.0);
  }

}



void DJones::createCorruptor(const VisIter& vi, const Record& simpar, const Int nSim)
{
  
  LogIO os(LogOrigin("D", "createCorruptor()", WHERE));  
  if (prtlev()>2) cout << "   D::createCorruptor()" << endl;
  
  // this may not be the best place for this:
  solvePol_=2;

  // no nSim since not time dependent (yet)
  dcorruptor_p = new DJonesCorruptor();
  corruptor_p = dcorruptor_p;

  // call generic parent to set corr,spw,etc info
  SolvableVisCal::createCorruptor(vi,simpar,nSim);
  
  Int Seed(1234);
  if (simpar.isDefined("seed")) {    
    Seed=simpar.asInt("seed");
  }

  Complex Scale(0.1,0.1); // scale of fluctuations 
  if (simpar.isDefined("camp")) {
    Scale=simpar.asComplex("camp");
  }

  Complex Offset(0.,0.); 
  if (simpar.isDefined("offset")) {
    Offset=simpar.asComplex("offset");
  }

  dcorruptor_p->initialize(Seed,Scale,Offset);
   
}



// **********************************************************
//  DfJones Implementations
//

DfJones::DfJones(VisSet& vs) :
  VisCal(vs),             // virtual base
  VisMueller(vs),         // virtual base
  DJones(vs)              // immediate parent
{
  if (prtlev()>2) cout << "Df::Df(vs)" << endl;
}

DfJones::DfJones(String msname,Int MSnAnt,Int MSnSpw) :
  VisCal(msname,MSnAnt,MSnSpw),             // virtual base
  VisMueller(msname,MSnAnt,MSnSpw),         // virtual base
  DJones(msname,MSnAnt,MSnSpw)              // immediate parent
{
  if (prtlev()>2) cout << "Df::Df(msname,MSnAnt,MSnSpw)" << endl;
}

DfJones::DfJones(const MSMetaInfoForCal& msmc) :
  VisCal(msmc),             // virtual base
  VisMueller(msmc),         // virtual base
  DJones(msmc)              // immediate parent
{
  if (prtlev()>2) cout << "Df::Df(msmc)" << endl;
}

DfJones::DfJones(const Int& nAnt) :
  VisCal(nAnt), 
  VisMueller(nAnt),
  DJones(nAnt)
{
  if (prtlev()>2) cout << "Df::Df(nAnt)" << endl;
}

DfJones::~DfJones() {
  if (prtlev()>2) cout << "Df::~Df()" << endl;
}



// **********************************************************
//  DlinJones Implementations
//

// Constructor
DlinJones::DlinJones(VisSet& vs)  :
  VisCal(vs),             // virtual base
  VisMueller(vs),         // virtual base
  DJones(vs)              // immediate parent
{
  if (prtlev()>2) cout << "Dlin::Dlin(vs)" << endl;
}

DlinJones::DlinJones(String msname,Int MSnAnt,Int MSnSpw) :
  VisCal(msname,MSnAnt,MSnSpw),             // virtual base
  VisMueller(msname,MSnAnt,MSnSpw),         // virtual base
  DJones(msname,MSnAnt,MSnSpw)              // immediate parent
{
  if (prtlev()>2) cout << "Dlin::Dlin(msname,MSnAnt,MSnSpw)" << endl;
}

DlinJones::DlinJones(const MSMetaInfoForCal& msmc) :
  VisCal(msmc),             // virtual base
  VisMueller(msmc),         // virtual base
  DJones(msmc)              // immediate parent
{
  if (prtlev()>2) cout << "Dlin::Dlin(msmc)" << endl;
}

DlinJones::DlinJones(const Int& nAnt) :
  VisCal(nAnt), 
  VisMueller(nAnt),
  DJones(nAnt)
{
  if (prtlev()>2) cout << "Dlin::Dlin(nAnt)" << endl;
}

DlinJones::~DlinJones() {
  if (prtlev()>2) cout << "Dlin::~Dlin()" << endl;
}


// **********************************************************
//  DflinJones Implementations
//

// Constructor
DflinJones::DflinJones(VisSet& vs)  :
  VisCal(vs),             // virtual base
  VisMueller(vs),         // virtual base
  DlinJones(vs)             // immediate parent
{
  if (prtlev()>2) cout << "Dflin::Dflin(vs)" << endl;
}

DflinJones::DflinJones(String msname,Int MSnAnt,Int MSnSpw) :
  VisCal(msname,MSnAnt,MSnSpw),             // virtual base
  VisMueller(msname,MSnAnt,MSnSpw),         // virtual base
  DlinJones(msname,MSnAnt,MSnSpw)             // immediate parent
{
  if (prtlev()>2) cout << "Dflin::Dflin(msname,MSnAnt,MSnSpw)" << endl;
}

DflinJones::DflinJones(const MSMetaInfoForCal& msmc) :
  VisCal(msmc),             // virtual base
  VisMueller(msmc),         // virtual base
  DlinJones(msmc)             // immediate parent
{
  if (prtlev()>2) cout << "Dflin::Dflin(msmc)" << endl;
}

DflinJones::DflinJones(const Int& nAnt) :
  VisCal(nAnt), 
  VisMueller(nAnt),
  DlinJones(nAnt)
{
  if (prtlev()>2) cout << "Dflin::Dflin(nAnt)" << endl;
}

DflinJones::~DflinJones() {
  if (prtlev()>2) cout << "Dflin::~Dflin()" << endl;
}




// **********************************************************
//  DllsJones Implementations
//

// Constructor
DllsJones::DllsJones(VisSet& vs)  :
  VisCal(vs),             // virtual base
  VisMueller(vs),         // virtual base
  DJones(vs)              // immediate parent
{
  if (prtlev()>2) cout << "Dlls::Dlls(vs)" << endl;
}

DllsJones::DllsJones(String msname,Int MSnAnt,Int MSnSpw) :
  VisCal(msname,MSnAnt,MSnSpw),             // virtual base
  VisMueller(msname,MSnAnt,MSnSpw),         // virtual base
  DJones(msname,MSnAnt,MSnSpw)              // immediate parent
{
  if (prtlev()>2) cout << "Dlls::Dlls(msname,MSnAnt,MSnSpw)" << endl;
}

DllsJones::DllsJones(const MSMetaInfoForCal& msmc) :
  VisCal(msmc),             // virtual base
  VisMueller(msmc),         // virtual base
  DJones(msmc)              // immediate parent
{
  if (prtlev()>2) cout << "Dlls::Dlls(msmc)" << endl;
}

DllsJones::DllsJones(const Int& nAnt) :
  VisCal(nAnt), 
  VisMueller(nAnt),
  DJones(nAnt)
{
  if (prtlev()>2) cout << "Dlls::Dlls(nAnt)" << endl;
}

DllsJones::~DllsJones() {
  if (prtlev()>2) cout << "Dlls::~Dlls()" << endl;
}

void DllsJones::solveOneVB(const VisBuffer& vb) {

  Int nChan=vb.nChannel();

  solveAllCPar()=Complex(0.0);
  solveAllParOK()=false;

  // Solve per chan
  Vector<Complex> cEq(2);
  Vector<uInt> cEqIdx(2);
  Vector<Complex> obs(2);
  Vector<Float> wt(2);
  for (Int ich=0;ich<nChan;++ich) {
    //Int currCh=vb.channel()(ich);
    //cout << "Ch=" << currCh;

    solveCPar().reference(solveAllCPar()(Slice(),Slice(ich,1,1),Slice()));
    solveParOK().reference(solveAllParOK()(Slice(),Slice(ich,1,1),Slice()));
    solveParOK().set(false);

    LSQFit lsq(2*nAnt(),LSQComplex(),0);

    Int ng(0);
    for (Int irow=0;irow<vb.nRow();++irow) {
      if (!vb.flagRow()(irow)) {
	
	// Discern this row's antennas, avoid ACs
	Int a1=vb.antenna1()(irow);
	Int a2=vb.antenna2()(irow);
	if (a1!=a2) {
	  
	  // If not flagged...
	  if (!vb.flag()(ich,irow)) {
	    ++ng;

	    // simple-minded OK-setting, for now
	    solveParOK().xyPlane(a1).set(true);
	    solveParOK().xyPlane(a2).set(true);

	    // Discern D indices for this baseline
	    Int xi=2*a1;
	    Int yi=2*a1+1;
	    Int xj=2*a2;
	    Int yj=2*a2+1;
	    /*
	    cout << "currCh="<<currCh 
		 << " irow="<<irow 
		 << " a1="<<a1 << " a2="<<a2
		 << " XY: xi/yj*="<< xi<<"/"<<yj
		 << " YX: yi/xj*="<< yi<<"/"<<xj
		 << endl;
	    */

	    wt(0)=vb.weightMat()(1,irow);
	    wt(1)=vb.weightMat()(2,irow);

	    cEq(0)=vb.modelVisCube()(0,ich,irow);
	    cEq(1)=vb.modelVisCube()(3,ich,irow);
	    obs(0)=vb.visCube()(1,ich,irow)-vb.modelVisCube()(1,ich,irow);
	    obs(1)=vb.visCube()(2,ich,irow)-vb.modelVisCube()(2,ich,irow);

	    cEqIdx(0)=yj; cEqIdx(1)=xi;
	    lsq.makeNorm(2,cEqIdx.data(),cEq.data(),wt(0),obs(0),LSQFit::Complex());
	    cEqIdx(0)=yi; cEqIdx(1)=xj;
	    lsq.makeNorm(2,cEqIdx.data(),cEq.data(),wt(1),conj(obs(1)),LSQFit::Complex());

	  }
	
	} // !AC
      } // !flagRow
    } // irow

    if (ng>0) {

      // Turn the crank on the solver
      uInt rank;
      lsq.invert(rank,false);  // true);  // SVD?
      Cube<Complex> Dest(2,1,nAnt());

      lsq.solve(Dest.data());
      
      //if (Int(rank)<nAnt()) 
      //cout << "   Need to reference!!";
      
      // Conjugate Dy's because we solved for their conjugates
      Dest(Slice(1,1,1),Slice(),Slice())=conj(Dest(Slice(1,1,1),Slice(),Slice()));
      
      // Store the result
      solveCPar()=Dest;
      //solveParOK().set(true);  // we set this more 'carefully' above
    }
    //cout << endl;

  } // ich

}

void DllsJones::solveOneSDB(SolveDataBuffer& sdb) {

  Int nChan=sdb.nChannels();

  solveAllCPar()=Complex(0.0);
  solveAllParOK()=false;

  // Solve per chan
  Vector<Complex> cEq(2);
  Vector<uInt> cEqIdx(2);
  Vector<Complex> obs(2);
  Vector<Float> wt(2);
  for (Int ich=0;ich<nChan;++ich) {

    solveCPar().reference(solveAllCPar()(Slice(),Slice(ich,1,1),Slice()));
    solveParOK().reference(solveAllParOK()(Slice(),Slice(ich,1,1),Slice()));
    solveParOK().set(false);

    LSQFit lsq(2*nAnt(),LSQComplex(),0);

    Int ng(0);
    for (Int irow=0;irow<sdb.nRows();++irow) {
      if (!sdb.flagRow()(irow)) {
	
	// Discern this row's antennas, avoid ACs
	Int a1=sdb.antenna1()(irow);
	Int a2=sdb.antenna2()(irow);
	if (a1!=a2) {
	  
	  // If not flagged...
	  if (!sdb.flagCube()(0,ich,irow) &&
	      !sdb.flagCube()(1,ich,irow) &&
	      !sdb.flagCube()(2,ich,irow) &&
	      !sdb.flagCube()(3,ich,irow) ) {
	    ++ng;

	    // simple-minded OK-setting, for now
	    solveParOK().xyPlane(a1).set(true);
	    solveParOK().xyPlane(a2).set(true);

	    // Discern D indices for this baseline
	    Int xi=2*a1;
	    Int yi=2*a1+1;
	    Int xj=2*a2;
	    Int yj=2*a2+1;
	    /*
	    cout << "currCh="<<currCh 
		 << " irow="<<irow 
		 << " a1="<<a1 << " a2="<<a2
		 << " XY: xi/yj*="<< xi<<"/"<<yj
		 << " YX: yi/xj*="<< yi<<"/"<<xj
		 << endl;
	    */

	    wt(0)=sdb.weightSpectrum()(1,ich,irow);
	    wt(1)=sdb.weightSpectrum()(2,ich,irow);

	    cEq(0)=sdb.visCubeModel()(0,ich,irow);
	    cEq(1)=sdb.visCubeModel()(3,ich,irow);
	    obs(0)=sdb.visCubeCorrected()(1,ich,irow)-sdb.visCubeModel()(1,ich,irow);
	    obs(1)=sdb.visCubeCorrected()(2,ich,irow)-sdb.visCubeModel()(2,ich,irow);

	    cEqIdx(0)=yj; cEqIdx(1)=xi;
	    lsq.makeNorm(2,cEqIdx.data(),cEq.data(),wt(0),obs(0),LSQFit::Complex());
	    cEqIdx(0)=yi; cEqIdx(1)=xj;
	    lsq.makeNorm(2,cEqIdx.data(),cEq.data(),wt(1),conj(obs(1)),LSQFit::Complex());

	  }
	
	} // !AC
      } // !flagRow
    } // irow

    if (ng>0) {

      // Turn the crank on the solver
      uInt rank;
      lsq.invert(rank,false);  // true);  // SVD?
      Cube<Complex> Dest(2,1,nAnt());

      lsq.solve(Dest.data());
      
      //if (Int(rank)<nAnt()) 
      //cout << "   Need to reference!!";
      
      // Conjugate Dy's because we solved for their conjugates
      Dest(Slice(1,1,1),Slice(),Slice())=conj(Dest(Slice(1,1,1),Slice(),Slice()));
      
      // Store the result
      solveCPar()=Dest;
      //solveParOK().set(true);  // we set this more 'carefully' above
    }
    //cout << endl;

  } // ich

}

void DllsJones::solveOne(SDBList& sdbs) {

  // In general, there will be many SDBs in the SDBList, usually
  //  over time (parang).  (If combine='spw', then there will be
  //  multiple SDBs over spw; in this case, we have to be clearer
  //  about what we mean by nChan below....)

  // The number of SDBs in the SDBList
  Int nSDB=sdbs.nSDB();

  //cout << "nSDB=" << nSDB << endl;

  // This will insist on uniformity over SDBs (usually the same spw)
  Int nChan=sdbs.nChannels();

  solveAllCPar()=Complex(0.0);
  solveAllParOK()=false;

  // Solve per chan
  Vector<Complex> cEq(2);
  Vector<uInt> cEqIdx(2);
  Vector<Complex> obs(2);
  Vector<Float> wt(2);
  for (Int ich=0;ich<nChan;++ich) {

    solveCPar().reference(solveAllCPar()(Slice(),Slice(ich,1,1),Slice()));
    solveParOK().reference(solveAllParOK()(Slice(),Slice(ich,1,1),Slice()));
    solveParOK().set(false);

    LSQFit lsq(2*nAnt(),LSQComplex(),0);

    Int ng(0);
    Float wtsum(0.0);
    for (Int isdb=0;isdb<nSDB;++isdb) {
      SolveDataBuffer& sdb(sdbs(isdb));
    for (Int irow=0;irow<sdb.nRows();++irow) {
      if (!sdb.flagRow()(irow)) {
	
	// Discern this row's antennas, avoid ACs
	Int a1=sdb.antenna1()(irow);
	Int a2=sdb.antenna2()(irow);
	if (a1!=a2) {
	  
	  // If not flagged...
	  if (!sdb.flagCube()(0,ich,irow) &&
	      !sdb.flagCube()(1,ich,irow) &&
	      !sdb.flagCube()(2,ich,irow) &&
	      !sdb.flagCube()(3,ich,irow) ) {
	    ++ng;

	    // simple-minded OK-setting, for now
	    solveParOK().xyPlane(a1).set(true);
	    solveParOK().xyPlane(a2).set(true);

	    // Discern D indices for this baseline
	    Int xi=2*a1;
	    Int yi=2*a1+1;
	    Int xj=2*a2;
	    Int yj=2*a2+1;
	    /*
	    cout << "currCh="<<currCh 
		 << " irow="<<irow 
		 << " a1="<<a1 << " a2="<<a2
		 << " XY: xi/yj*="<< xi<<"/"<<yj
		 << " YX: yi/xj*="<< yi<<"/"<<xj
		 << endl;
	    */

	    wt(0)=sdb.weightSpectrum()(1,ich,irow);
	    wt(1)=sdb.weightSpectrum()(2,ich,irow);

	    cEq(0)=sdb.visCubeModel()(0,ich,irow);
	    cEq(1)=sdb.visCubeModel()(3,ich,irow);
	    obs(0)=sdb.visCubeCorrected()(1,ich,irow)-sdb.visCubeModel()(1,ich,irow);
	    obs(1)=sdb.visCubeCorrected()(2,ich,irow)-sdb.visCubeModel()(2,ich,irow);

	    cEqIdx(0)=yj; cEqIdx(1)=xi;
	    lsq.makeNorm(2,cEqIdx.data(),cEq.data(),wt(0),obs(0),LSQFit::Complex());
	    cEqIdx(0)=yi; cEqIdx(1)=xj;
	    lsq.makeNorm(2,cEqIdx.data(),cEq.data(),wt(1),conj(obs(1)),LSQFit::Complex());

	    wtsum+=(wt(0)+wt(1));

	  }
	
	} // !AC
      } // !flagRow
    } // irow
    } // isdb

    //cout << "wtsum=" << wtsum << endl;


    if (ng>0) {

      // Turn the crank on the solver
      uInt rank;
      lsq.invert(rank,false);  // true);  // SVD?
      Cube<Complex> Dest(2,1,nAnt());

      lsq.solve(Dest.data());
      
      //if (Int(rank)<nAnt()) 
      //cout << "   Need to reference!!";
      
      // Conjugate Dy's because we solved for their conjugates
      Dest(Slice(1,1,1),Slice(),Slice())=conj(Dest(Slice(1,1,1),Slice(),Slice()));
      
      // Store the result
      solveCPar()=Dest;
      //solveParOK().set(true);  // we set this more 'carefully' above
    }
    //cout << endl;

  } // ich

}


// **********************************************************
//  DfllsJones Implementations
//

// Constructor
DfllsJones::DfllsJones(VisSet& vs)  :
  VisCal(vs),             // virtual base
  VisMueller(vs),         // virtual base
  DllsJones(vs)             // immediate parent
{
  if (prtlev()>2) cout << "Dflls::Dflls(vs)" << endl;
}

DfllsJones::DfllsJones(String msname,Int MSnAnt,Int MSnSpw) :
  VisCal(msname,MSnAnt,MSnSpw),             // virtual base
  VisMueller(msname,MSnAnt,MSnSpw),         // virtual base
  DllsJones(msname,MSnAnt,MSnSpw)           // immediate parent
{
  if (prtlev()>2) cout << "Dflls::Dflls(msname,MSnAnt,MSnSpw)" << endl;
}

DfllsJones::DfllsJones(const MSMetaInfoForCal& msmc) :
  VisCal(msmc),             // virtual base
  VisMueller(msmc),         // virtual base
  DllsJones(msmc)           // immediate parent
{
  if (prtlev()>2) cout << "Dflls::Dflls(msmc)" << endl;
}

DfllsJones::DfllsJones(const Int& nAnt) :
  VisCal(nAnt), 
  VisMueller(nAnt),
  DllsJones(nAnt)
{
  if (prtlev()>2) cout << "Dflls::Dflls(nAnt)" << endl;
}

DfllsJones::~DfllsJones() {
  if (prtlev()>2) cout << "Dflls::~Dflls()" << endl;
}

/*

// **********************************************************
//  XMueller: positiona angle for circulars
//

XMueller::XMueller(VisSet& vs) :
  VisCal(vs),             // virtual base
  VisMueller(vs),         // virtual base
  SolvableVisMueller(vs)    // immediate parent
{
  if (prtlev()>2) cout << "X::X(vs)" << endl;
}

XMueller::XMueller(String msname,Int MSnAnt,Int MSnSpw) :
  VisCal(msname,MSnAnt,MSnSpw),             // virtual base
  VisMueller(msname,MSnAnt,MSnSpw),         // virtual base
  SolvableVisMueller(msname,MSnAnt,MSnSpw)    // immediate parent
{
  if (prtlev()>2) cout << "X::X(msname,MSnAnt,MSnSpw)" << endl;
}

XMueller::XMueller(const MSMetaInfoForCal& msmc) :
  VisCal(msmc),             // virtual base
  VisMueller(msmc),         // virtual base
  SolvableVisMueller(msmc)    // immediate parent
{
  if (prtlev()>2) cout << "X::X(msmc)" << endl;
}

XMueller::XMueller(const Int& nAnt) :
  VisCal(nAnt), 
  VisMueller(nAnt),
  SolvableVisMueller(nAnt)
{
  if (prtlev()>2) cout << "X::X(nAnt)" << endl;
}

XMueller::~XMueller() {
  if (prtlev()>2) cout << "X::~X()" << endl;
}

void XMueller::setApply(const Record& apply) {

  SolvableVisCal::setApply(apply);

  // Force calwt to false 
  calWt()=false;

}


void XMueller::setSolve(const Record& solvepar) {

  cout << "XMueller: parType() = " << this->parType() << endl;

  SolvableVisCal::setSolve(solvepar);

  // Force calwt to false 
  calWt()=false;

  // For X insist preavg is meaningful (5 minutes or user-supplied)
  if (preavg()<0.0)
    preavg()=300.0;

  
  cout << "ct_ = " << ct_ << endl;


}

void XMueller::newselfSolve(VisSet& vs, VisEquation& ve) {

  if (prtlev()>4) cout << "   M::selfSolve(ve)" << endl;

  // Inform logger/history
  logSink() << "Solving for " << typeName()
            << LogIO::POST;

  // Initialize the svc according to current VisSet
  //  (this counts intervals, sizes CalSet)
  Vector<Int> nChunkPerSol;
  Int nSol = sizeUpSolve(vs,nChunkPerSol);
  
  // Create the Caltable
  createMemCalTable();

  // The iterator, VisBuffer
  VisIter& vi(vs.iter());
  VisBuffer vb(vi);

  //  cout << "nSol = " << nSol << endl;
  //  cout << "nChunkPerSol = " << nChunkPerSol << endl;

  Vector<Int> slotidx(vs.numberSpw(),-1);

  Int nGood(0);
  vi.originChunks();
  for (Int isol=0;isol<nSol && vi.moreChunks();++isol) {

    // Arrange to accumulate
    VisBuffAccumulator vba(nAnt(),preavg(),false);
    
    for (Int ichunk=0;ichunk<nChunkPerSol(isol);++ichunk) {

      // Current _chunk_'s spw
      Int spw(vi.spectralWindow());

      // Abort if we encounter a spw for which a priori cal not available
      if (!ve.spwOK(spw))
        throw(AipsError("Pre-applied calibration not available for at least 1 spw. Check spw selection carefully."));


      // Collapse each timestamp in this chunk according to VisEq
      //  with calibration and averaging
      for (vi.origin(); vi.more(); vi++) {

        // Force read of the field Id
        vb.fieldId();

        // This forces the data/model/wt I/O, and applies
        //   any prior calibrations
        ve.collapse(vb);

        // If permitted/required by solvable component, normalize
        //if (normalizable())
	//          vb.normalize();

	// If this solve not freqdep, and channels not averaged yet, do so
	if (!freqDepMat() && vb.nChannel()>1)
	  vb.freqAveCubes();

        // Accumulate collapsed vb in a time average
        vba.accumulate(vb);
      }
      // Advance the VisIter, if possible
      if (vi.moreChunks()) vi.nextChunk();

    }

    // Finalize the averged VisBuffer
    vba.finalizeAverage();

    // The VisBuffer to solve with
    VisBuffer& svb(vba.aveVisBuff());

    // Establish meta-data for this interval
    //  (some of this may be used _during_ solve)
    //  (this sets currSpw() in the SVC)
    Bool vbOk=syncSolveMeta(svb,-1);

    Int thisSpw=spwMap()(svb.spectralWindow());
    slotidx(thisSpw)++;

    // We are actually solving for all channels simultaneously
    solveCPar().reference(solveAllCPar());
    solveParOK().reference(solveAllParOK());
    solveParErr().reference(solveAllParErr());
    solveParSNR().reference(solveAllParSNR());

    // Fill solveCPar() with 1, nominally, and flagged
    solveCPar()=Complex(1.0);
    solveParOK()=false;
    
    if (vbOk && svb.nRow()>0) {

      // solve for the R-L phase term in the current VB
      solveOneVB(svb);

      if (solveParOK()(0,0,0))
	logSink() << "Position angle offset solution for " 
		  << msmc().fieldName(currField())
		  << " (spw = " << currSpw() << ") = "
		  << arg(solveCPar()(0,0,0))*180.0/C::pi/2.0
		  << " deg."
		  << LogIO::POST;
      else
	logSink() << "Position angle offset solution for " 
		  << msmc().fieldName(currField())
		  << " (spw = " << currSpw() << ") "
		  << " was not determined (insufficient data)."
		  << LogIO::POST;
	
      nGood++;
    }

    keepNCT();
    
  }
  
  logSink() << "  Found good "
            << typeName() << " solutions in "
            << nGood << " intervals."
            << LogIO::POST;

  // Store whole of result in a caltable
  if (nGood==0)
    logSink() << "No output calibration table written."
              << LogIO::POST;
  else {

    // Do global post-solve tinkering (e.g., phase-only, normalization, etc.)
    //  TBD
    // globalPostSolveTinker();

    // write the table
    storeNCT();

  }

}

void XMueller::calcAllMueller() {

  //  cout << "currMElem().shape() = " << currMElem().shape() << endl;

  // Put the phase factor into the cross-hand diagonals
  //  (1,0) for the para-hands  
  IPosition blc(3,0,0,0);
  IPosition trc(3,0,nChanMat()-1,nElem()-1);
  currMElem()(blc,trc)=Complex(1.0);

  blc(0)=trc(0)=1;
  currMElem()(blc,trc)=currCPar()(0,0,0);
  blc(0)=trc(0)=2;
  currMElem()(blc,trc)=conj(currCPar()(0,0,0));

  blc(0)=trc(0)=3;
  currMElem()(blc,trc)=Complex(1.0);

  currMElemOK()=true;

}


void XMueller::solveOneVB(const VisBuffer& vb) {

  // This just a simple average of the cross-hand
  //  visbilities...

  Complex d,md;
  Float wt,a;
  DComplex rl(0.0),lr(0.0);
  Double sumwt(0.0);
  for (Int irow=0;irow<vb.nRow();++irow) {
    if (!vb.flagRow()(irow) &&
	vb.antenna1()(irow)!=vb.antenna2()(irow)) {

      for (Int ich=0;ich<vb.nChannel();++ich) {
	if (!vb.flag()(ich,irow)) {
	  
	  // A common weight for both crosshands
	  // TBD: we should probably consider this carefully...
	  //  (also in D::guessPar...)
	  wt=Double(vb.weightMat()(1,irow)+
		    vb.weightMat()(2,irow))/2.0;

	  // correct weight for model normalization
	  a=abs(vb.modelVisCube()(1,ich,irow));
	  wt*=(a*a);
	  
	  if (wt>0.0) {
	    // Cross-hands only
	    for (Int icorr=1;icorr<3;++icorr) {
	      md=vb.modelVisCube()(icorr,ich,irow);
	      d=vb.visCube()(icorr,ich,irow);
	      
	      if (abs(d)>0.0) {
		
		if (icorr==1) 
		  rl+=DComplex(Complex(wt)*d/md);
		else
		  lr+=DComplex(Complex(wt)*d/md);
		
		sumwt+=Double(wt);
		
	      } // abs(d)>0
	    } // icorr
	  } // wt>0
	} // !flag
      } // ich
    } // !flagRow
  } // row
  

//  cout << "spw = " << currSpw() << endl;
//  cout << " rl = " << rl << " " << arg(rl)*180.0/C::pi << endl;
//  cout << " lr = " << lr << " " << arg(lr)*180.0/C::pi << endl;


    // combine lr with rl
  rl+=conj(lr);
  
  // Normalize to unit amplitude
  //  (note that the phase result is insensitive to sumwt)
  Double amp=abs(rl);
  if (sumwt>0 && amp>0.0) {
    rl/=DComplex(amp);
    
    solveCPar()=Complex(rl);
    solveParOK()=true;
  }
  
}



// **********************************************************
//  XJones: position angle for circulars (antenna-based
//

XJones::XJones(VisSet& vs) :
  VisCal(vs),             // virtual base
  VisMueller(vs),         // virtual base
  SolvableVisJones(vs)    // immediate parent
{
  if (prtlev()>2) cout << "X::X(vs)" << endl;
}

XJones::XJones(String msname,Int MSnAnt,Int MSnSpw) :
  VisCal(msname,MSnAnt,MSnSpw),             // virtual base
  VisMueller(msname,MSnAnt,MSnSpw),         // virtual base
  SolvableVisJones(msname,MSnAnt,MSnSpw)    // immediate parent
{
  if (prtlev()>2) cout << "X::X(msname,MSnAnt,MSnSpw)" << endl;
}

XJones::XJones(const MSMetaInfoForCal& msmc) :
  VisCal(msmc),             // virtual base
  VisMueller(msmc),         // virtual base
  SolvableVisJones(msmc)    // immediate parent
{
  if (prtlev()>2) cout << "X::X(msmc)" << endl;
}

XJones::XJones(const Int& nAnt) :
  VisCal(nAnt), 
  VisMueller(nAnt),
  SolvableVisJones(nAnt)
{
  if (prtlev()>2) cout << "X::X(nAnt)" << endl;
}

XJones::~XJones() {
  if (prtlev()>2) cout << "X::~X()" << endl;
}

void XJones::setApply(const Record& apply) {

  SolvableVisCal::setApply(apply);

  // Force calwt to false 
  calWt()=false;

}


void XJones::setSolve(const Record& solvepar) {

  SolvableVisCal::setSolve(solvepar);

  // Force calwt to false 
  calWt()=false;

  // For X insist preavg is meaningful (5 minutes or user-supplied)
  if (preavg()<0.0)
    preavg()=300.0;

  // Force refant to none (==-1), because it is meaningless to
  //  apply a refant to an X solution
  if (refant()>-1) {
    logSink() << ".   (Ignoring specified refant for " 
	      << typeName() << " solve.)"
	      << LogIO::POST;
    refantlist().resize(1);
    refantlist()(0)=-1;
  }

}

void XJones::newselfSolve(VisSet& vs, VisEquation& ve) {

  if (prtlev()>4) cout << "   Xj::newselfSolve(ve)" << endl;

  // Inform logger/history
  logSink() << "Solving for " << typeName()
            << LogIO::POST;

  // Initialize the svc according to current VisSet
  //  (this counts intervals, sizes CalSet)
  Vector<Int> nChunkPerSol;
  Int nSol = sizeUpSolve(vs,nChunkPerSol);

  // Create the Caltable
  createMemCalTable();

  // The iterator, VisBuffer
  VisIter& vi(vs.iter());
  VisBuffer vb(vi);

  //  cout << "nSol = " << nSol << endl;
  //  cout << "nChunkPerSol = " << nChunkPerSol << endl;

  Vector<Int> slotidx(vs.numberSpw(),-1);

  Int nGood(0);
  vi.originChunks();
  for (Int isol=0;isol<nSol && vi.moreChunks();++isol) {

    // Arrange to accumulate
    VisBuffAccumulator vba(nAnt(),preavg(),false);
    
    for (Int ichunk=0;ichunk<nChunkPerSol(isol);++ichunk) {

      // Current _chunk_'s spw
      Int spw(vi.spectralWindow());

      // Abort if we encounter a spw for which a priori cal not available
      if (!ve.spwOK(spw))
        throw(AipsError("Pre-applied calibration not available for at least 1 spw. Check spw selection carefully."));


      // Collapse each timestamp in this chunk according to VisEq
      //  with calibration and averaging
      for (vi.origin(); vi.more(); vi++) {

        // Force read of the field Id
        vb.fieldId();

        // This forces the data/model/wt I/O, and applies
        //   any prior calibrations
        ve.collapse(vb);

        // If permitted/required by solvable component, normalize
        if (normalizable())
	  vb.normalize();

	// If this solve not freqdep, and channels not averaged yet, do so
	if (!freqDepMat() && vb.nChannel()>1)
	  vb.freqAveCubes();

        // Accumulate collapsed vb in a time average
        vba.accumulate(vb);
      }
      // Advance the VisIter, if possible
      if (vi.moreChunks()) vi.nextChunk();

    }

    // Finalize the averged VisBuffer
    vba.finalizeAverage();

    // The VisBuffer to solve with
    VisBuffer& svb(vba.aveVisBuff());

    // Establish meta-data for this interval
    //  (some of this may be used _during_ solve)
    //  (this sets currSpw() in the SVC)
    Bool vbOk=syncSolveMeta(svb,-1);

    Int thisSpw=spwMap()(svb.spectralWindow());
    slotidx(thisSpw)++;

    // We are actually solving for all channels simultaneously
    solveCPar().reference(solveAllCPar());
    solveParOK().reference(solveAllParOK());
    solveParErr().reference(solveAllParErr());
    solveParSNR().reference(solveAllParSNR());

    // Fill solveCPar() with 1, nominally, and flagged
    solveCPar()=Complex(1.0);
    solveParOK()=false;
    
    if (vbOk && svb.nRow()>0) {

      // solve for the R-L phase term in the current VB
      solveOneVB(svb);

      if (ntrue(solveParOK())>0) {
	Float ang=arg(sum(solveCPar()(solveParOK()))/Float(ntrue(solveParOK())))*90.0/C::pi;


	logSink() << "Mean position angle offset solution for " 
		  << msmc().fieldName(currField())
		  << " (spw = " << currSpw() << ") = "
		  << ang
		  << " deg."
		  << LogIO::POST;
      }
      else
	logSink() << "Position angle offset solution for " 
		  << msmc().fieldName(currField())
		  << " (spw = " << currSpw() << ") "
		  << " was not determined (insufficient data)."
		  << LogIO::POST;
	
      nGood++;
    }

    keepNCT();
    
  }
  
  logSink() << "  Found good "
            << typeName() << " solutions in "
            << nGood << " intervals."
            << LogIO::POST;

  // Store whole of result in a caltable
  if (nGood==0)
    logSink() << "No output calibration table written."
              << LogIO::POST;
  else {

    // Do global post-solve tinkering (e.g., phase-only, normalization, etc.)
    //  TBD
    // globalPostSolveTinker();

    // write the table
    storeNCT();
  }

}


void XJones::calcAllJones() {

  //  cout << "currJElem().shape() = " << currJElem().shape() << endl;

  //  put the par in the first position on the diagonal
  //  [p 0]
  //  [0 1]
  

  // Set first element to the parameter
  IPosition blc(3,0,0,0);
  IPosition trc(3,0,nChanMat()-1,nElem()-1);
  currJElem()(blc,trc)=currCPar();
  currJElemOK()(blc,trc)=currParOK();
  
  // Set second diag element to one
  blc(0)=trc(0)=1;
  currJElem()(blc,trc)=Complex(1.0);
  currJElemOK()(blc,trc)=currParOK();

}


void XJones::solveOneVB(const VisBuffer& vb) {

  // This just a simple average of the cross-hand
  //  visbilities...

  // We are actually solving for all channels simultaneously
  solveCPar().reference(solveAllCPar());
  solveParOK().reference(solveAllParOK());
  solveParErr().reference(solveAllParErr());
  solveParSNR().reference(solveAllParSNR());
  
  // Fill solveCPar() with 1, nominally, and flagged
  solveCPar()=Complex(1.0);
  solveParOK()=false;

  Int nChan=vb.nChannel();

  Complex d;
  Float wt;
  Vector<DComplex> rl(nChan,0.0),lr(nChan,0.0);
  Double sumwt(0.0);
  for (Int irow=0;irow<vb.nRow();++irow) {
    if (!vb.flagRow()(irow) &&
	vb.antenna1()(irow)!=vb.antenna2()(irow)) {

      for (Int ich=0;ich<nChan;++ich) {
	if (!vb.flag()(ich,irow)) {
	  
	  // A common weight for both crosshands
	  // TBD: we should probably consider this carefully...
	  //  (also in D::guessPar...)
	  wt=Double(vb.weightMat()(1,irow)+
		    vb.weightMat()(2,irow))/2.0;

	  // correct weight for model normalization
	  //	  a=abs(vb.modelVisCube()(1,ich,irow));
	  //	  wt*=(a*a);
	  
	  if (wt>0.0) {
	    // Cross-hands only
	    for (Int icorr=1;icorr<3;++icorr) {
	      //	      md=vb.modelVisCube()(icorr,ich,irow);
	      d=vb.visCube()(icorr,ich,irow);
	      
	      if (abs(d)>0.0) {
		
		if (icorr==1) 
		  rl(ich)+=DComplex(Complex(wt)*d);
		//		  rl(ich)+=DComplex(Complex(wt)*d/md);
		else
		  lr(ich)+=DComplex(Complex(wt)*d);
		//		  lr(ich)+=DComplex(Complex(wt)*d/md);
		
		sumwt+=Double(wt);
		
	      } // abs(d)>0
	    } // icorr
	  } // wt>0
	} // !flag
      } // ich
    } // !flagRow
  } // row
  

  //  cout << "spw = " << currSpw() << endl;
  //  cout << " rl = " << rl << " " << phase(rl)*180.0/C::pi << endl;
  //  cout << " lr = " << lr << " " << phase(lr)*180.0/C::pi << endl;

  // Record results
  solveCPar()=Complex(1.0);
  solveParOK()=false;
  for (Int ich=0;ich<nChan;++ich) {
    // combine lr with rl
    rl(ich)+=conj(lr(ich));
  
    // Normalize to unit amplitude
    //  (note that the phase result is insensitive to sumwt)
    Double amp=abs(rl(ich));
    // For now, all antennas get the same solution
    IPosition blc(3,0,0,0);
    IPosition trc(3,0,0,nElem()-1);
    if (sumwt>0 && amp>0.0) {
      rl(ich)/=DComplex(amp);
      blc(1)=trc(1)=ich;
      solveCPar()(blc,trc)=Complex(rl(ich));
      solveParOK()(blc,trc)=true;
    }
  }

  
  if (ntrue(solveParOK())>0) {
    Float ang=arg(sum(solveCPar()(solveParOK()))/Float(ntrue(solveParOK())))*90.0/C::pi;
    
    
    logSink() << "Mean position angle offset solution for " 
	      << msmc().fieldName(currField())
	      << " (spw = " << currSpw() << ") = "
	      << ang
	      << " deg."
	      << LogIO::POST;
  }
  else
    logSink() << "Position angle offset solution for " 
	      << msmc().fieldName(currField())
	      << " (spw = " << currSpw() << ") "
	      << " was not determined (insufficient data)."
	      << LogIO::POST;
  
}

void XJones::solveOneSDB(SolveDataBuffer& sdb) {

  // This just a simple average of the cross-hand
  //  visbilities...

  // We are actually solving for all channels simultaneously
  solveCPar().reference(solveAllCPar());
  solveParOK().reference(solveAllParOK());
  solveParErr().reference(solveAllParErr());
  solveParSNR().reference(solveAllParSNR());
  
  // Fill solveCPar() with 1, nominally, and flagged
  solveCPar()=Complex(1.0);
  solveParOK()=false;

  Int nChan=sdb.nChannels();

  Complex d;
  Float wt;
  Vector<DComplex> rl(nChan,0.0),lr(nChan,0.0);
  Double sumwt(0.0);
  for (Int irow=0;irow<sdb.nRows();++irow) {
    if (!sdb.flagRow()(irow) &&
	sdb.antenna1()(irow)!=sdb.antenna2()(irow)) {

      for (Int ich=0;ich<nChan;++ich) {
	if (!sdb.flagCube()(1,ich,irow) &&
	    !sdb.flagCube()(2,ich,irow)) {
	  
	  // A common weight for both crosshands
	  // TBD: we should probably consider this carefully...
	  //  (also in D::guessPar...)
	  wt=Double(sdb.weightSpectrum()(1,ich,irow)+
		    sdb.weightSpectrum()(2,ich,irow))/2.0;

	  // correct weight for model normalization
	  //	  a=abs(vb.modelVisCube()(1,ich,irow));
	  //	  wt*=(a*a);
	  
	  if (wt>0.0) {
	    // Cross-hands only
	    for (Int icorr=1;icorr<3;++icorr) {
	      d=sdb.visCubeCorrected()(icorr,ich,irow);
	      
	      if (abs(d)>0.0) {
		
		if (icorr==1) 
		  rl(ich)+=DComplex(Complex(wt)*d);
		else
		  lr(ich)+=DComplex(Complex(wt)*d);
		
		sumwt+=Double(wt);
		
	      } // abs(d)>0
	    } // icorr
	  } // wt>0
	} // !flag
      } // ich
    } // !flagRow
  } // row
  

  //  cout << "spw = " << currSpw() << endl;
  //  cout << " rl = " << rl << " " << phase(rl)*180.0/C::pi << endl;
  //  cout << " lr = " << lr << " " << phase(lr)*180.0/C::pi << endl;

  // Record results
  solveCPar()=Complex(1.0);
  solveParOK()=false;
  for (Int ich=0;ich<nChan;++ich) {
    // combine lr with rl
    rl(ich)+=conj(lr(ich));
  
    // Normalize to unit amplitude
    //  (note that the phase result is insensitive to sumwt)
    Double amp=abs(rl(ich));
    // For now, all antennas get the same solution
    IPosition blc(3,0,0,0);
    IPosition trc(3,0,0,nElem()-1);
    if (sumwt>0 && amp>0.0) {
      rl(ich)/=DComplex(amp);
      blc(1)=trc(1)=ich;
      solveCPar()(blc,trc)=Complex(rl(ich));
      solveParOK()(blc,trc)=true;
    }
  }

  
  if (ntrue(solveParOK())>0) {
    Float ang=arg(sum(solveCPar()(solveParOK()))/Float(ntrue(solveParOK())))*90.0/C::pi;
    
    
    logSink() << "Mean position angle offset solution for " 
	      << msmc().fieldName(currField())
	      << " (spw = " << currSpw() << ") = "
	      << ang
	      << " deg."
	      << LogIO::POST;
  }
  else
    logSink() << "Position angle offset solution for " 
	      << msmc().fieldName(currField())
	      << " (spw = " << currSpw() << ") "
	      << " was not determined (insufficient data)."
	      << LogIO::POST;
  
}


void XJones::solveOne(SDBList& sdbs) {

  // This just a simple average of the cross-hand
  //  visbilities...

  Int nSDB=sdbs.nSDB();

  //cout << "nSDB=" << nSDB << endl;

  // We are actually solving for all channels simultaneously
  solveCPar().reference(solveAllCPar());
  solveParOK().reference(solveAllParOK());
  solveParErr().reference(solveAllParErr());
  solveParSNR().reference(solveAllParSNR());
  
  // Fill solveCPar() with 1, nominally, and flagged
  solveCPar()=Complex(1.0);
  solveParOK()=false;

  Int nChan=sdbs.nChannels();

  Complex d;
  Float wt;
  Vector<DComplex> rl(nChan,0.0),lr(nChan,0.0);
  Double sumwt(0.0);
  for (Int isdb=0;isdb<nSDB;++isdb) {
    SolveDataBuffer& sdb(sdbs(isdb));
  for (Int irow=0;irow<sdb.nRows();++irow) {
    if (!sdb.flagRow()(irow) &&
	sdb.antenna1()(irow)!=sdb.antenna2()(irow)) {

      for (Int ich=0;ich<nChan;++ich) {
	if (!sdb.flagCube()(1,ich,irow) &&
	    !sdb.flagCube()(2,ich,irow)) {
	  
	  // A common weight for both crosshands
	  // TBD: we should probably consider this carefully...
	  //  (also in D::guessPar...)
	  wt=Double(sdb.weightSpectrum()(1,ich,irow)+
		    sdb.weightSpectrum()(2,ich,irow))/2.0;

	  // correct weight for model normalization
	  //	  a=abs(vb.modelVisCube()(1,ich,irow));
	  //	  wt*=(a*a);
	  
	  if (wt>0.0) {
	    // Cross-hands only
	    for (Int icorr=1;icorr<3;++icorr) {
	      d=sdb.visCubeCorrected()(icorr,ich,irow);
	      
	      if (abs(d)>0.0) {
		
		if (icorr==1) 
		  rl(ich)+=DComplex(Complex(wt)*d);
		else
		  lr(ich)+=DComplex(Complex(wt)*d);
		
		sumwt+=Double(wt);
		
	      } // abs(d)>0
	    } // icorr
	  } // wt>0
	} // !flag
      } // ich
    } // !flagRow
  } // row
  } // isdb

  //  cout << "spw = " << currSpw() << endl;
  //  cout << " rl = " << rl << " " << phase(rl)*180.0/C::pi << endl;
  //  cout << " lr = " << lr << " " << phase(lr)*180.0/C::pi << endl;

  // Record results
  solveCPar()=Complex(1.0);
  solveParOK()=false;
  for (Int ich=0;ich<nChan;++ich) {
    // combine lr with rl
    rl(ich)+=conj(lr(ich));
  
    // Normalize to unit amplitude
    //  (note that the phase result is insensitive to sumwt)
    Double amp=abs(rl(ich));
    // For now, all antennas get the same solution
    IPosition blc(3,0,0,0);
    IPosition trc(3,0,0,nElem()-1);
    if (sumwt>0 && amp>0.0) {
      rl(ich)/=DComplex(amp);
      blc(1)=trc(1)=ich;
      solveCPar()(blc,trc)=Complex(rl(ich));
      solveParOK()(blc,trc)=true;
    }
  }

  
  if (ntrue(solveParOK())>0) {
    Float ang=arg(sum(solveCPar()(solveParOK()))/Float(ntrue(solveParOK())))*90.0/C::pi;
    
    
    logSink() << "Mean position angle offset solution for " 
	      << msmc().fieldName(currField())
	      << " (spw = " << currSpw() << ") = "
	      << ang
	      << " deg."
	      << LogIO::POST;
  }
  else
    logSink() << "Position angle offset solution for " 
	      << msmc().fieldName(currField())
	      << " (spw = " << currSpw() << ") "
	      << " was not determined (insufficient data)."
	      << LogIO::POST;
  
}

// **********************************************************
//  XfJones: CHANNELIZED position angle for circulars (antenna-based)
//

XfJones::XfJones(VisSet& vs) :
  VisCal(vs),             // virtual base
  VisMueller(vs),         // virtual base
  XJones(vs)              // immediate parent
{
  if (prtlev()>2) cout << "Xf::Xf(vs)" << endl;
}

XfJones::XfJones(String msname,Int MSnAnt,Int MSnSpw) :
  VisCal(msname,MSnAnt,MSnSpw),             // virtual base
  VisMueller(msname,MSnAnt,MSnSpw),         // virtual base
  XJones(msname,MSnAnt,MSnSpw)              // immediate parent
{
  if (prtlev()>2) cout << "Xf::Xf(msname,MSnAnt,MSnSpw)" << endl;
}

XfJones::XfJones(const MSMetaInfoForCal& msmc) :
  VisCal(msmc),             // virtual base
  VisMueller(msmc),         // virtual base
  XJones(msmc)              // immediate parent
{
  if (prtlev()>2) cout << "Xf::Xf(msmc)" << endl;
}

XfJones::XfJones(const Int& nAnt) :
  VisCal(nAnt), 
  VisMueller(nAnt),
  XJones(nAnt)
{
  if (prtlev()>2) cout << "Xf::Xf(nAnt)" << endl;
}

XfJones::~XfJones() {
  if (prtlev()>2) cout << "Xf::~Xf()" << endl;
}

void XfJones::initSolvePar() {

  SolvableVisJones::initSolvePar();
  return;

}




// **********************************************************
//  GlinXphJones Implementations
//

GlinXphJones::GlinXphJones(VisSet& vs) :
  VisCal(vs),             // virtual base
  VisMueller(vs),         // virtual base
  GJones(vs),             // immediate parent
  QU_()
{
  if (prtlev()>2) cout << "GlinXph::GlinXph(vs)" << endl;
}

GlinXphJones::GlinXphJones(String msname,Int MSnAnt,Int MSnSpw) :
  VisCal(msname,MSnAnt,MSnSpw),             // virtual base
  VisMueller(msname,MSnAnt,MSnSpw),         // virtual base
  GJones(msname,MSnAnt,MSnSpw),             // immediate parent
  QU_()
{
  if (prtlev()>2) cout << "GlinXph::GlinXph(msname,MSnAnt,MSnSpw)" << endl;
}

GlinXphJones::GlinXphJones(const MSMetaInfoForCal& msmc) :
  VisCal(msmc),             // virtual base
  VisMueller(msmc),         // virtual base
  GJones(msmc),             // immediate parent
  QU_()
{
  if (prtlev()>2) cout << "GlinXph::GlinXph(msmc)" << endl;
}

GlinXphJones::GlinXphJones(const Int& nAnt) :
  VisCal(nAnt), 
  VisMueller(nAnt),
  GJones(nAnt),
  QU_()
{
  if (prtlev()>2) cout << "GlinXph::GlinXph(nAnt)" << endl;
}

GlinXphJones::~GlinXphJones() {
  if (prtlev()>2) cout << "GlinXph::~GlinXph()" << endl;
}

void GlinXphJones::setApply(const Record& apply) {

  GJones::setApply(apply);

  // Force calwt to false 
  calWt()=false;

}



void GlinXphJones::selfGatherAndSolve(VisSet& vs, VisEquation& ve) {

  if (prtlev()>4) cout << "   GlnXph::selfGatherAndSolve(ve)" << endl;

  // Inform logger/history
  logSink() << "Solving for " << typeName()
            << LogIO::POST;

  // Initialize the svc according to current VisSet
  //  (this counts intervals, sizes CalSet)
  Vector<Int> nChunkPerSol;
  Int nSol = sizeUpSolve(vs,nChunkPerSol);

  // Create the Caltable
  createMemCalTable();

  // The iterator, VisBuffer
  VisIter& vi(vs.iter());
  VisBuffer vb(vi);

  //  cout << "nSol = " << nSol << endl;
  //  cout << "nChunkPerSol = " << nChunkPerSol << endl;

  Int nGood(0);
  vi.originChunks();
  for (Int isol=0;isol<nSol && vi.moreChunks();++isol) {

    // Arrange to accumulate
    VisBuffAccumulator vba(nAnt(),preavg(),false);
    
    for (Int ichunk=0;ichunk<nChunkPerSol(isol);++ichunk) {

      // Current _chunk_'s spw
      Int spw(vi.spectralWindow());

      // Abort if we encounter a spw for which a priori cal not available
      if (!ve.spwOK(spw))
        throw(AipsError("Pre-applied calibration not available for at least 1 spw. Check spw selection carefully."));


      // Collapse each timestamp in this chunk according to VisEq
      //  with calibration and averaging
      for (vi.origin(); vi.more(); vi++) {

        // Force read of the field Id
        vb.fieldId();

        // This forces the data/model/wt I/O, and applies
        //   any prior calibrations
        ve.collapse(vb);

        // If permitted/required by solvable component, normalize
        if (normalizable())
	  vb.normalize();

	// If this solve not freqdep, and channels not averaged yet, do so
	if (!freqDepMat() && vb.nChannel()>1)
	  vb.freqAveCubes();

        // Accumulate collapsed vb in a time average
        vba.accumulate(vb);
      }
      // Advance the VisIter, if possible
      if (vi.moreChunks()) vi.nextChunk();

    }

    // Finalize the averged VisBuffer
    vba.finalizeAverage();

    // The VisBuffer to solve with
    VisBuffer& svb(vba.aveVisBuff());

    // Establish meta-data for this interval
    //  (some of this may be used _during_ solve)
    //  (this sets currSpw() in the SVC)
    Bool vbOk=syncSolveMeta(svb,-1);

    if (vbOk && svb.nRow()>0) {

      // solve for the X-Y phase term in the current VB
      solveOneVB(svb);

      nGood++;
    }

    keepNCT();
    
  }
  
  logSink() << "  Found good "
            << typeName() << " solutions in "
            << nGood << " intervals."
            << LogIO::POST;

  // Store whole of result in a caltable
  if (nGood==0)
    logSink() << "No output calibration table written."
              << LogIO::POST;
  else {

    // Do global post-solve tinkering (e.g., phase-only, normalization, etc.)
    globalPostSolveTinker();

    // write the table
    storeNCT();
  }

}

// Handle trivial vbga
void GlinXphJones::selfSolveOne(VisBuffGroupAcc& vbga) {

  // Expecting only on VB in the vbga (with many times)
  if (vbga.nBuf()!=1)
    throw(AipsError("GlinXphJones can't process multi-vb vbga."));

  // Call single-VB specialized solver with the one vb
  this->solveOneVB(vbga(0));

}

// SDBList (VI2) version
void GlinXphJones::selfSolveOne(SDBList& sdbs) {

  // Expecting multiple SDBs (esp. in time)
  if (sdbs.nSDB()==1)
    throw(AipsError("GlinXphJones needs multiple SDBs"));

  // Call single-VB specialized solver with the one vb
  this->solveOne(sdbs);

}

// Solve for the X-Y phase from the cross-hand's slope in R/I
void GlinXphJones::solveOneVB(const VisBuffer& vb) {

  // ensure
  if (QU_.shape()!=IPosition(2,2,nSpw())) {
    QU_.resize(2,nSpw());
    QU_.set(0.0);
  }

  Int thisSpw=spwMap()(vb.spectralWindow());
  
  // We are actually solving for all channels simultaneously
  solveCPar().reference(solveAllCPar());
  solveParOK().reference(solveAllParOK());
  solveParErr().reference(solveAllParErr());
  solveParSNR().reference(solveAllParSNR());
  
  // Fill solveCPar() with 1, nominally, and flagged
  solveCPar()=Complex(1.0);
  solveParOK()=false;

  Int nChan=vb.nChannel();
  //  if (nChan>1)
  //    throw(AipsError("X-Y phase solution NYI for channelized data"));

  // Find number of timestamps in the VB
  Vector<uInt> ord;
  Int nTime=genSort(ord,vb.time(),Sort::Ascending,Sort::NoDuplicates);

  Matrix<Double> x(nTime,nChan,0.0),y(nTime,nChan,0.0),wt(nTime,nChan,0.0),sig(nTime,nChan,0.0);
  Matrix<Bool> mask(nTime,nChan,false);

  mask.set(false);
  Complex v(0.0);
  Float wt0(0.0);
  Int iTime(-1);
  Double currtime(-1.0);
  for (Int irow=0;irow<vb.nRow();++irow) {
    if (!vb.flagRow()(irow) &&
	vb.antenna1()(irow)!=vb.antenna2()(irow)) {

      // Advance time index when we see a new time
      if (vb.time()(irow)!=currtime) {
	++iTime;
	currtime=vb.time()(irow); // remember the new current time
      }

      // Weights not yet chan-dep
      wt0=(vb.weightMat()(1,irow)+vb.weightMat()(2,irow));
      if (wt0>0.0) {

	for (Int ich=0;ich<nChan;++ich) {
	  if (!vb.flag()(ich,irow)) {
	    v=vb.visCube()(1,ich,irow)+conj(vb.visCube()(2,ich,irow));
	    x(iTime,ich)+=Double(wt0*real(v));
	    y(iTime,ich)+=Double(wt0*imag(v));
	    wt(iTime,ich)+=Double(wt0);
	  }
	}
      }
    }
  }

  // Normalize data by accumulated weights
  for (Int itime=0;itime<nTime;++itime) {
    for (Int ich=0;ich<nChan;++ich) {
      if (wt(itime,ich)>0.0) {
	x(itime,ich)/=wt(itime,ich);
	y(itime,ich)/=wt(itime,ich);
	sig(itime,ich)=sqrt(1.0/wt(itime,ich));
	mask(itime,ich)=true;
      }
      else
	sig(itime,ich)=DBL_MAX;    // ~zero weight
    }
  }

  // Solve for each channel
  Vector<Complex> Cph(nChan);
  Cph.set(Complex(1.0,0.0));
  Float currAmb(1.0);
  Bool report(false);
  for (Int ich=0;ich<nChan;++ich) {

    if (sum(wt.column(ich))>0.0) {
      report=true;
      LinearFit<Double> phfitter;
      Polynomial<AutoDiff<Double> > line(1);
      phfitter.setFunction(line);
      Vector<Bool> m(mask.column(ich));

      // Fit shallow and steep, and always prefer shallow
      
      // Assumed shallow solve:
      Vector<Double> solnA;
      solnA.assign(phfitter.fit(x.column(ich),y.column(ich),sig.column(ich),&m));

      // Assumed steep solve:
      Vector<Double> solnB;
      solnB.assign(phfitter.fit(y.column(ich),x.column(ich),sig.column(ich),&m));

      Double Xph(0.0);
      if (abs(solnA(1))<abs(solnB(1))) {
	Xph=atan(solnA(1));
      }
      else {
	Xph=atan(1.0/solnB(1));
      }

      Cph(ich)=currAmb*Complex(DComplex(cos(Xph),sin(Xph)));

      // Watch for and remove ambiguity changes which can
      //  occur near Xph~= +/-90 deg (the atan above can jump)
      //  (NB: this does _not_ resolve the amb; it merely makes
      //   it consistent within the spw)
      if (ich>0) {
	// If Xph changes by more than pi/2, probably a ambig jump...
	Float dang=abs(arg(Cph(ich)/Cph(ich-1)));
	if (dang > (C::pi/2.)) {
	  Cph(ich)*=-1.0;   // fix this one
	  currAmb*=-1.0;    // reverse currAmb, so curr amb is carried forward
	  //	  cout << "  Found XY phase ambiguity jump at chan=" << ich << " in spw=" << currSpw();
	}
      }

      //      cout << " (" << currAmb << ")";
      //      cout << endl;


      // Set all antennas with this X-Y phase (as a complex number)
      solveCPar()(Slice(0,1,1),Slice(ich,1,1),Slice())=Cph(ich);
      solveParOK()(Slice(),Slice(ich,1,1),Slice())=true;
    }
    else {
      solveCPar()(Slice(0,1,1),Slice(ich,1,1),Slice())=Complex(1.0);
      solveParOK()(Slice(),Slice(ich,1,1),Slice())=false;
    }
  }

  if (report)
    cout << endl 
	 << "Spw = " << thisSpw
	 << " (ich=" << nChan/2 << "/" << nChan << "): " << endl
	 << " X-Y phase = " << arg(Cph[nChan/2])*180.0/C::pi << " deg." << endl;
      

  // Now fit for the source polarization
  {

    Vector<Double> wtf(nTime,0.0),sigf(nTime,0.0),xf(nTime,0.0),yf(nTime,0.0);
    Vector<Bool> maskf(nTime,false);
    Float wt0;
    Complex v;
    Double currtime(-1.0);
    Int iTime(-1);
    for (Int irow=0;irow<vb.nRow();++irow) {
      if (!vb.flagRow()(irow) &&
	  vb.antenna1()(irow)!=vb.antenna2()(irow)) {
	
	if (vb.time()(irow)!=currtime) {
	  // Advance time index when we see a new time
	  ++iTime;
	  currtime=vb.time()(irow); // remember the new current time
	}

	// Weights not yet chan-dep
	wt0=(vb.weightMat()(1,irow)+vb.weightMat()(2,irow));
	if (wt0>0.0) {
	  for (Int ich=0;ich<nChan;++ich) {
	    
	    if (!vb.flag()(ich,irow)) {
	      // Correct x-hands for xy-phase and add together
	      v=vb.visCube()(1,ich,irow)/Cph(ich)+vb.visCube()(2,ich,irow)/conj(Cph(ich));
	      xf(iTime)+=Double(wt0*2.0*(vb.feed_pa(vb.time()(irow))(0)));
	      yf(iTime)+=Double(wt0*real(v)/2.0);
	      wtf(iTime)+=Double(wt0);
	    }
	  }
	}
      }
    }
    
    // Normalize data by accumulated weights
    for (Int itime=0;itime<nTime;++itime) {
      if (wtf(itime)>0.0) {
	xf(itime)/=wtf(itime);
	yf(itime)/=wtf(itime);
	sigf(itime)=sqrt(1.0/wtf(itime));
	maskf(itime)=true;
      }
      else
	sigf(itime)=DBL_MAX;    // ~zero weight
    }
    
    // p0=Q, p1=U, p2 = real part of net instr pol offset
    //  x is TWICE the parallactic angle
    CompiledFunction<AutoDiff<Double> > fn;
    fn.setFunction("-p0*sin(x) + p1*cos(x) + p2");

    LinearFit<Double> fitter;
    fitter.setFunction(fn);
    
    Vector<Double> soln=fitter.fit(xf,yf,sigf,&maskf);
    
    QU_(0,thisSpw) = soln(0);
    QU_(1,thisSpw) = soln(1);

    cout << " Fractional Poln: "
	 << "Q = " << QU_(0,thisSpw) << ", "
	 << "U = " << QU_(1,thisSpw) << "; "
	 << "P = " << sqrt(soln(0)*soln(0)+soln(1)*soln(1)) << ", "
	 << "X = " << atan2(soln(1),soln(0))*90.0/C::pi << "deg."
	 << endl;
    cout << " Net (over baselines) instrumental polarization: " 
	 << soln(2) << endl;

  }	

}

// Solve for the X-Y phase from the cross-hand's slope in R/I
void GlinXphJones::solveOne(SDBList& sdbs) {

  // ensure
  if (QU_.shape()!=IPosition(2,2,nSpw())) {
    QU_.resize(2,nSpw());
    QU_.set(0.0);
  }

  Int thisSpw=sdbs.aggregateSpw();
  
  // We are actually solving for all channels simultaneously
  solveCPar().reference(solveAllCPar());
  solveParOK().reference(solveAllParOK());
  solveParErr().reference(solveAllParErr());
  solveParSNR().reference(solveAllParSNR());
  
  // Fill solveCPar() with 1, nominally, and flagged
  solveCPar()=Complex(1.0);
  solveParOK()=false;

  Int nChan=sdbs.nChannels();

  // Number of datapoints in fit is the number of SDBs
  Int nSDB=sdbs.nSDB();

  Matrix<Double> x(nSDB,nChan,0.0),y(nSDB,nChan,0.0),wt(nSDB,nChan,0.0),sig(nSDB,nChan,0.0);
  Matrix<Bool> mask(nSDB,nChan,false);

  mask.set(false);
  Complex v(0.0);
  Float wt0(0.0);
  
  for (Int isdb=0;isdb<nSDB;++isdb) {
    SolveDataBuffer& sdb(sdbs(isdb));

    for (Int irow=0;irow<sdb.nRows();++irow) {
      if (!sdb.flagRow()(irow) &&
	  sdb.antenna1()(irow)!=sdb.antenna2()(irow)) {
	
	for (Int ich=0;ich<nChan;++ich) {
	  if (!sdb.flagCube()(1,ich,irow)) {
	    wt0=sdb.weightSpectrum()(1,ich,irow);
	    v=sdb.visCubeCorrected()(1,ich,irow);
	    x(isdb,ich)+=Double(wt0*real(v));
	    y(isdb,ich)+=Double(wt0*imag(v));
	    wt(isdb,ich)+=Double(wt0);
	  }
	  if (!sdb.flagCube()(2,ich,irow)) {
	    wt0=sdb.weightSpectrum()(2,ich,irow);
	    v=conj(sdb.visCubeCorrected()(2,ich,irow));
	    x(isdb,ich)+=Double(wt0*real(v));
	    y(isdb,ich)+=Double(wt0*imag(v));
	    wt(isdb,ich)+=Double(wt0);
	  }
	}
      }
    }
  }

  // Normalize data by accumulated weights
  for (Int isdb=0;isdb<nSDB;++isdb) {
    for (Int ich=0;ich<nChan;++ich) {
      if (wt(isdb,ich)>0.0) {
	x(isdb,ich)/=wt(isdb,ich);
	y(isdb,ich)/=wt(isdb,ich);
	sig(isdb,ich)=sqrt(1.0/wt(isdb,ich));
	mask(isdb,ich)=true;
      }
      else
	sig(isdb,ich)=DBL_MAX;    // ~zero weight
    }
  }

  // Solve for each channel
  Vector<Complex> Cph(nChan);
  Cph.set(Complex(1.0,0.0));
  Float currAmb(1.0);
  Bool report(false);
  for (Int ich=0;ich<nChan;++ich) {

    if (sum(wt.column(ich))>0.0) {
      report=true;
      LinearFit<Double> phfitter;
      Polynomial<AutoDiff<Double> > line(1);
      phfitter.setFunction(line);
      Vector<Bool> m(mask.column(ich));

      // Fit shallow and steep, and always prefer shallow
      
      // Assumed shallow solve:
      Vector<Double> solnA;
      solnA.assign(phfitter.fit(x.column(ich),y.column(ich),sig.column(ich),&m));

      // Assumed steep solve:
      Vector<Double> solnB;
      solnB.assign(phfitter.fit(y.column(ich),x.column(ich),sig.column(ich),&m));

      Double Xph(0.0);
      if (abs(solnA(1))<abs(solnB(1))) {
	Xph=atan(solnA(1));
      }
      else {
	Xph=atan(1.0/solnB(1));
      }

      Cph(ich)=currAmb*Complex(DComplex(cos(Xph),sin(Xph)));

      // Watch for and remove ambiguity changes which can
      //  occur near Xph~= +/-90 deg (the atan above can jump)
      //  (NB: this does _not_ resolve the amb; it merely makes
      //   it consistent within the spw)
      if (ich>0) {
	// If Xph changes by more than pi/2, probably a ambig jump...
	Float dang=abs(arg(Cph(ich)/Cph(ich-1)));
	if (dang > (C::pi/2.)) {
	  Cph(ich)*=-1.0;   // fix this one
	  currAmb*=-1.0;    // reverse currAmb, so curr amb is carried forward
	  //	  cout << "  Found XY phase ambiguity jump at chan=" << ich << " in spw=" << currSpw();
	}
      }

      //      cout << " (" << currAmb << ")";
      //      cout << endl;


      // Set all antennas with this X-Y phase (as a complex number)
      solveCPar()(Slice(0,1,1),Slice(ich,1,1),Slice())=Cph(ich);
      solveParOK()(Slice(),Slice(ich,1,1),Slice())=true;
    }
    else {
      solveCPar()(Slice(0,1,1),Slice(ich,1,1),Slice())=Complex(1.0);
      solveParOK()(Slice(),Slice(ich,1,1),Slice())=false;
    }
  }

  if (report)
    cout << endl 
	 << "Spw = " << thisSpw
	 << " (ich=" << nChan/2 << "/" << nChan << "): " << endl
	 << " X-Y phase = " << arg(Cph[nChan/2])*180.0/C::pi << " deg." << endl;
      

  // Now fit for the source polarization
  {

    Vector<Double> wtf(nSDB,0.0),sigf(nSDB,0.0),xf(nSDB,0.0),yf(nSDB,0.0);
    Vector<Bool> maskf(nSDB,false);
    Float wt0;
    Complex v;
    for (Int isdb=0;isdb<nSDB;++isdb) {
      SolveDataBuffer& sdb(sdbs(isdb));

      for (Int irow=0;irow<sdb.nRows();++irow) {
	if (!sdb.flagRow()(irow) &&
	    sdb.antenna1()(irow)!=sdb.antenna2()(irow)) {
	  
	  Float fpa(sdb.feedPa()(0));  // assumes same for all antennas!
	  
	  for (Int ich=0;ich<nChan;++ich) {
	    
	    if (!sdb.flagCube()(1,ich,irow)) {
	      // Correct x-hands for xy-phase and add together
	      wt0=sdb.weightSpectrum()(1,ich,irow);
	      v=sdb.visCubeCorrected()(1,ich,irow)/Cph(ich);
	      xf(isdb)+=Double(wt0*2.0*(fpa));
	      yf(isdb)+=Double(wt0*real(v));
	      wtf(isdb)+=Double(wt0);
	    }
	    if (!sdb.flagCube()(2,ich,irow)) {
	      // Correct x-hands for xy-phase and add together
	      wt0=sdb.weightSpectrum()(2,ich,irow);
	      v=sdb.visCubeCorrected()(2,ich,irow)/conj(Cph(ich));
	      xf(isdb)+=Double(wt0*2.0*(fpa));
	      yf(isdb)+=Double(wt0*real(v));
	      wtf(isdb)+=Double(wt0);
	    }
	  }
	}
      }
    }
    
    // Normalize data by accumulated weights
    for (Int isdb=0;isdb<nSDB;++isdb) {
      if (wtf(isdb)>0.0) {
	xf(isdb)/=wtf(isdb);
	yf(isdb)/=wtf(isdb);
	sigf(isdb)=sqrt(1.0/wtf(isdb));
	maskf(isdb)=true;
      }
      else
	sigf(isdb)=DBL_MAX;    // ~zero weight
    }
    
    // p0=Q, p1=U, p2 = real part of net instr pol offset
    //  x is TWICE the parallactic angle
    CompiledFunction<AutoDiff<Double> > fn;
    fn.setFunction("-p0*sin(x) + p1*cos(x) + p2");

    LinearFit<Double> fitter;
    fitter.setFunction(fn);
    
    Vector<Double> soln=fitter.fit(xf,yf,sigf,&maskf);

    srcPolPar().resize(2);
    srcPolPar()(0)=soln(0);
    srcPolPar()(1)=soln(1);
        
    QU_(0,thisSpw) = soln(0);
    QU_(1,thisSpw) = soln(1);

    cout << " Fractional Poln: "
	 << "Q = " << QU_(0,thisSpw) << ", "
	 << "U = " << QU_(1,thisSpw) << "; "
	 << "P = " << sqrt(soln(0)*soln(0)+soln(1)*soln(1)) << ", "
	 << "X = " << atan2(soln(1),soln(0))*90.0/C::pi << "deg."
	 << endl;
    cout << " Net (over baselines) instrumental polarization: " 
	 << soln(2) << endl;

  }	

}

void GlinXphJones::globalPostSolveTinker() {

    // Add QU info the the keywords
    TableRecord& tr(ct_->rwKeywordSet());
    Record qu;
    qu.define("QU",QU_);
    tr.defineRecord("QU",qu);

}


// **********************************************************
//  GlinXphfJones Implementations
//

// Constructor
GlinXphfJones::GlinXphfJones(VisSet& vs)  :
  VisCal(vs),             // virtual base
  VisMueller(vs),         // virtual base
  GlinXphJones(vs)        // immediate parent
{
  if (prtlev()>2) cout << "GlinXphf::GlinXphf(vs)" << endl;
}

GlinXphfJones::GlinXphfJones(String msname,Int MSnAnt,Int MSnSpw) :
  VisCal(msname,MSnAnt,MSnSpw),             // virtual base
  VisMueller(msname,MSnAnt,MSnSpw),         // virtual base
  GlinXphJones(msname,MSnAnt,MSnSpw)        // immediate parent
{
  if (prtlev()>2) cout << "GlinXphf::GlinXphf(msname,MSnAnt,MSnSpw)" << endl;
}

GlinXphfJones::GlinXphfJones(const MSMetaInfoForCal& msmc) :
  VisCal(msmc),             // virtual base
  VisMueller(msmc),         // virtual base
  GlinXphJones(msmc)        // immediate parent
{
  if (prtlev()>2) cout << "GlinXphf::GlinXphf(msmc)" << endl;
}

GlinXphfJones::GlinXphfJones(const Int& nAnt) :
  VisCal(nAnt), 
  VisMueller(nAnt),
  GlinXphJones(nAnt)
{
  if (prtlev()>2) cout << "GlinXphf::GlinXphf(nAnt)" << endl;
}

GlinXphfJones::~GlinXphfJones() {
  if (prtlev()>2) cout << "GlinXphf::~GlinXphf()" << endl;
}

*/



} //# NAMESPACE CASA - END
