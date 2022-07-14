//# VisCalSolver.cc: Implementation of generic visibility solving
//# Copyright (C) 1996,1997,1998,1999,2000,2001,2002,2003
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

#include <synthesis/MeasurementComponents/VisCalSolver.h>

#include <msvis/MSVis/VisBuffer.h>

#include <casacore/casa/Arrays/ArrayMath.h>
#include <casacore/casa/Arrays/MaskArrMath.h>
#include <casacore/casa/Arrays/ArrayLogical.h>
#include <casacore/casa/Arrays/ArrayIter.h>
//#include <scimath/Mathematics/MatrixMathLA.h>
#include <casacore/casa/BasicSL/String.h>
#include <casacore/casa/BasicMath/Math.h>
#include <casacore/casa/Utilities/Assert.h>
#include <casacore/casa/Exceptions/Error.h>
#include <casacore/casa/OS/Memory.h>
#include <casacore/casa/OS/Path.h>

#include <sstream>

#include <casacore/casa/Logging/LogMessage.h>
#include <casacore/casa/Logging/LogSink.h>

#define VCS_PRTLEV 0

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN


// **********************************************************
//  UVMod Implementations
//

VisCalSolver::VisCalSolver() :
  svb_(NULL),
  vbga_(NULL),
  ve_(NULL),
  svc_(NULL),
  nTotalPar_(0),
  R_(),dR_(),Rflg_(),
  maxIter_(50),
  chiSq_(0.0),
  chiSqV_(4,0.0),
  lastChiSq_(0.0),dChiSq_(0.0),
  sumWt_(0.0),nWt_(0),
  cvrgcount_(0),
  par_(), parOK_(), parErr_(), srcPar_(), lastCalPar_(), lastSrcPar_(),
  dpar_(), dcalpar_(), dsrcpar_(),
  grad_(),hess_(),
  lambda_(2.0),
  optstep_(true),
  prtlev_(VCS_PRTLEV)
{
  if (prtlev()>0) cout << "VCS::VCS()" << endl;
}

VisCalSolver::~VisCalSolver() 
{  
  if (prtlev()>0) cout << "VCS::~VCS()" << endl;
}

Bool VisCalSolver::solve(VisEquation& ve, SolvableVisCal& svc, VisBuffer& svb) {

  if (prtlev()>1) cout << "VCS::solve(,,)" << endl;

  /*
  LogSink logsink;
  {
    LogMessage message(LogOrigin("VisCalSolver", "solve"));
    ostringstream o; o<<"Beginning solve...";
    message.message(o);
    logsink.post(message);
  }
  */
  // Pointers to local ve,svc,svb
  ve_=&ve;
  svc_=&svc;
  svb_=&svb;
  vbga_=NULL;

  // Verify that VisEq has the correct svc:
  // TBD?

  // Initialize everything 
  initSolve();

  Vector<Float> steplist(maxIter_+2,0.0);
  Vector<Float> rsteplist(maxIter_+2,0.0);

  //  cout << "svb.modelVisCube() = " << phase(svb.modelVisCube())*180.0/C::pi << endl;
  //  cout << "svb.modelVisCube() = " << amplitude(svb.modelVisCube()) << endl;

  // Verify VisBuffer validity for solving
  //   (this sets parOK() on per-antenna basis (for focusChan)
  //    based on data weights and baseline participation)
  Bool oktosolve = svc_->verifyForSolve(*svb_);

  if (oktosolve) {
    
    if (prtlev()>1) cout << "First guess:" << endl
			 << "amp = " << amplitude(par()) << endl
			 << "pha = " << phase(par()) 
			 << endl;

    // Iterate solution
    Int iter(0);
    Bool done(false);
    while (!done) {
      
      if (prtlev()>2) cout << " Beginning iteration " << iter 
			   << "---------------------------------" << endl;
      
      // Differentiate the VB and get current Chi2
      differentiate();
      chiSquare();
      if (chiSq()==0.0) {
	//	cout << "CHI2 IS SPURIOUSLY ZERO!*************************************" << endl;
	//	cout << "R() = " << R() << endl;
	//	cout << "wtmat = " << svb.weightMat() << endl;
	//	cout << "flag = " << svb.flag() << endl;
	//	cout << "sum(wtmat) = " << sum(wtmat) << endl;
	return false;
      }

      dChiSq() = chiSq()-lastChiSq();

      //      cout << "chi2 = " << chiSq() << " " << dChiSq() << " " << dChiSq()/chiSq() << endl;
      
      // Continuue if we haven't converged
      if (!converged()) {
	
	if (dChiSq()<=0.0) {
	  // last step was good...
	  lastChiSq()=chiSq();
	  
	  // so accumulate new grad/hess...
	  accGradHess();
	  
	  //...and adjust lambda downward
	  //	lambda()/=2.0;
	  //	lambda()=0.8;
	  lambda()=1.0;
	}
	else {
	  //	  cout << "reverting..." << chiSq() << " " << dChiSq() << " (" << iter << ")" << endl;
	  // last step was bad, revert to previous 
	  revert();
	  //...with a larger lambda
	  //	lambda()*=4.0;
	  lambda()=1.0;
	}
	
	// Solve for the parameter step
	solveGradHess();
	
	// Remember curr pars
	lastCalPar()=par();
	if (svc_->solvePol())
	  lastSrcPar()=srcPar();
	
	// Refine the step size by exploring chi2 in the
	//  gradient direction
	if (optstep_) //  && cvrgcount_>=3)
	  optStepSize();
	
	// Update current parameters (saves a copy of them)
	updatePar();

	//	cout << "srcPar() = " << srcPar() << endl;

	steplist(iter)=max(amplitude(dCalPar()));
	rsteplist(iter)=max(amplitude(dCalPar())/amplitude(par()));

      }
      else {
	// Convergence means we're done!
	done=true;

	if (prtlev()>0) {
	  cout << "Iterations =" << iter << endl;
	  cout << "par()=" << par() << endl;
	  cout << "srcPar()=" << srcPar() << endl;
	}


	/*
	cout << " good pars=" << ntrue(parOK())
	     << " iterations=" << iter << endl
	     << " steps=" << steplist(IPosition(1,0),IPosition(1,iter)) 
	     << endl
	     << " rsteps=" << rsteplist(IPosition(1,0),IPosition(1,iter)) 
	     << endl;
	*/   

	// Get parameter errors:
	accGradHess();
	getErrors();

	// Return, signaling success if at least 1 good solution
	return (ntrue(parOK())>0);
	
      }
      
      // Escape iteration loop via iteration limit
      if (iter==maxIter()) {
	cout << "Reached iteration limit: " << iter << " iterations.  " << endl;
	//	cout << " good pars = " << ntrue(parOK())
	//	     << "  steps = " << steplist
	//	     << endl;
	done=true;
      }
      
      // Advance iteration counter
      iter++;
    }
    
  }
  else {
    cout << " Insufficient unflagged antennas to proceed with this solve." << endl;
  }

  return false;
    
}

// New VisBuffGroupAcc version
Bool VisCalSolver::solve(VisEquation& ve, SolvableVisCal& svc, VisBuffGroupAcc& vbga) {

  if (prtlev()>1) cout << "VCS::solve(,,VBGA)" << endl;

  /*
  LogSink logsink;
  {
    LogMessage message(LogOrigin("VisCalSolver", "solve"));
    ostringstream o; o<<"Beginning solve...";
    message.message(o);
    logsink.post(message);
  }
  */
  // Pointers to local ve,svc,svb
  ve_=&ve;
  svc_=&svc;
  svb_=NULL;
  vbga_=&vbga;

  // Verify that VisEq has the correct svc:
  // TBD?

  // Initialize everything 
  initSolve();

  Vector<Float> steplist(maxIter_+2,0.0);
  Vector<Float> rsteplist(maxIter_+2,0.0);

  // Verify Data's validity for solve w.r.t. baselines available
  //   (this sets parOK() on per-antenna basis (for focusChan)
  //    based on data weights and baseline participation)
  Bool oktosolve = svc_->verifyConstraints(*vbga_);

  if (oktosolve) {
    
    if (prtlev()>1) cout << "First guess:" << endl
			 << "amp = " << amplitude(par()) << endl
			 << "pha = " << phase(par()) 
			 << endl;

    // Iterate solution
    Int iter(0);
    Bool done(false);
    while (!done) {
      
      if (prtlev()>2) cout << " Beginning iteration " << iter 
			   << "---------------------------------" << endl;
      
      // Differentiate the VB and get current Chi2
      differentiate2();
      chiSquare2();
      if (chiSq()==0.0) {
	//	cout << "CHI2 IS SPURIOUSLY ZERO!*************************************" << endl;
	//	cout << "R() = " << R() << endl;
	//	cout << "wtmat = " << svb.weightMat() << endl;
	//	cout << "flag = " << svb.flag() << endl;
	//	cout << "sum(wtmat) = " << sum(wtmat) << endl;
	return false;
      }

      dChiSq() = chiSq()-lastChiSq();

      //      cout << "chi2 = " << chiSq() << " " << dChiSq() << " " << dChiSq()/chiSq() << endl;
      
      // Continuue if we haven't converged
      if (!converged()) {
	
	if (dChiSq()<=0.0) {
	  // last step was good...
	  lastChiSq()=chiSq();
	  
	  // so accumulate new grad/hess...
	  accGradHess2();
	  
	  //...and adjust lambda downward
	  //	lambda()/=2.0;
	  //	lambda()=0.8;
	  lambda()=1.0;
	}
	else {
	  //	  cout << "reverting..." << chiSq() << " " << dChiSq() << " (" << iter << ")" << endl;
	  // last step was bad, revert to previous 
	  revert();
	  //...with a larger lambda
	  //	lambda()*=4.0;
	  lambda()=1.0;
	}
	
	// Solve for the parameter step
	solveGradHess();
	
	// Remember curr pars
	lastCalPar()=par();
	if (svc_->solvePol())
	  lastSrcPar()=srcPar();
	
	// Refine the step size by exploring chi2 in the
	//  gradient direction
	if (optstep_) //  && cvrgcount_>=3)
	  optStepSize2();
	
	// Update current parameters (saves a copy of them)
	updatePar();

	//	cout << "srcPar() = " << srcPar() << endl;

	steplist(iter)=max(amplitude(dCalPar()));
	rsteplist(iter)=max(amplitude(dCalPar())/amplitude(par()));

      }
      else {
	// Convergence means we're done!
	done=true;

	if (prtlev()>0) {
	  cout << "Iterations =" << iter << endl;
	  cout << "par()=" << par() << endl;
	  cout << "srcPar()=" << srcPar() << endl;
	}


	/*
	cout << " good pars=" << ntrue(parOK())
	     << " iterations=" << iter << endl
	     << " steps=" << steplist(IPosition(1,0),IPosition(1,iter)) 
	     << endl
	     << " rsteps=" << rsteplist(IPosition(1,0),IPosition(1,iter)) 
	     << endl;
	*/   

	// Get parameter errors:
	accGradHess2();
	getErrors();

	// Return, signaling success if at least 1 good solution
	return (ntrue(parOK())>0);
	
      }
      
      // Escape iteration loop via iteration limit
      if (iter==maxIter()) {
	cout << "Reached iteration limit: " << iter << " iterations.  " << endl;
	//	cout << " good pars = " << ntrue(parOK())
	//	     << "  steps = " << steplist
	//	     << endl;
	done=true;
      }
      
      // Advance iteration counter
      iter++;
    }
    
  }
  else {
    cout << " Insufficient unflagged antennas to proceed with this solve." << endl;
  }

  return false;
    
}
  
void VisCalSolver::initSolve() {
    
  if (prtlev()>2) cout << " VCS::initSolve()" << endl;

  // Get total number of cal parameters from svc info
  nCalPar()=svc().nTotalPar();

  // solvePol() tells us how many source pol params we are solving for
  nSrcPar()=svc().solvePol();

  // the total number of parameters
  nTotalPar()=nCalPar()+nSrcPar();

  if (prtlev()>2)
    cout << "  Total parameters in solve: " << nTotalPar() << endl;

  // Chi2 and weights
  chiSq()=0.0;
  lastChiSq()=DBL_MAX;
  dChiSq()=0.0;
    
  sumWt()=0.0;
  nWt()=0;

  // Link up svc's internal pars with local reference
  //   (only if shape is correct)

  if (svc().solveCPar().nelements()==uInt(nCalPar())) {
    par().reference(svc().solveCPar().reform(IPosition(1,nCalPar())));
    parOK().reference(svc().solveParOK().reform(IPosition(1,nCalPar())));
    parErr().reference(svc().solveParErr().reform(IPosition(1,nCalPar())));
    if (svc().solvePol()) 
      srcPar().reference(svc().srcPolPar());
  }
  else
    throw(AipsError("Solver and SVC cannot synchronize parameters."));

  // Pars

  dpar().resize(nTotalPar());
  dpar()=0.0;

  lastCalPar().resize(nCalPar());
  dCalPar().reference(dpar()(IPosition(1,0),IPosition(1,nCalPar()-1)));

  if (svc().solvePol()) {
    lastSrcPar().resize(nSrcPar());
    dSrcPar().reference(dpar()(IPosition(1,nCalPar()),IPosition(1,nTotalPar()-1)));
  }
  else {
    lastSrcPar().resize();
    dSrcPar().resize();
  }

  // Gradient and Hessian
  grad().resize(nTotalPar());
  grad()=0.0;

  hess().resize(nTotalPar());
  hess()=0.0;

  // Levenberg-Marquardt factor
  lambda()=2.0;

  // Convergence anticipation
  cvrgcount_=0;

}

void VisCalSolver::residualate() {

  if (prtlev()>2) cout << "  VCS::residualate()" << endl;

  // Delegate to VisEquation
  //  ve().residuals(svb(),R(),Rflag());

  // For now, just use ve.diffResid, until we have
  //  implemented focuschan-aware trial corrupt in SVC
  //  (this will hurt performance a bit)
  ve().diffResiduals(svb(),R(),dR(),Rflg());

}

void VisCalSolver::residualate2() {

  if (prtlev()>2) cout << "  VCS::residualate()" << endl;

  // Delegate to VisEquation
  //  ve().residuals(svb(),R(),Rflag());

  // For now, just use ve.diffResid, until we have
  //  implemented focuschan-aware trial corrupt in SVC
  //  (this will hurt performance a bit)
  
  for (Int ibuf=0;ibuf<vbga().nBuf();++ibuf) 
    ve().diffResiduals(vbga()(ibuf));
}

void VisCalSolver::differentiate() {

  if (prtlev()>2) cout << "  VCS::differentiate()" << endl;

  // Delegate to VisEquation
  ve().diffResiduals(svb(),R(),dR(),Rflg());


  if (svc().solvePol()) {

    // Differentiate w.r.t source
    svc().diffSrc(svb(),dSrc());
  }

  if (prtlev()>6) {  // R, dR
    cout << "   R= " << R() << endl;
    cout << "   dR=" << dR() << endl;
  }

}

void VisCalSolver::differentiate2() {

  if (prtlev()>2) cout << "  VCS::differentiate(VBGA version)" << endl;

  // Delegate to VisEquation
  for (Int ibuf=0;ibuf<vbga().nBuf();++ibuf)
    ve().diffResiduals(vbga()(ibuf));

  if (svc().solvePol()) {
    //    throw(AipsError("solvePol not yet sorted w.r.t. VBGA."));
    // Differentiate w.r.t source
    svc().diffSrc(vbga()(0),dSrc());
  }

}

void VisCalSolver::chiSquare() {

  if (prtlev()>2) cout << "   VCS::chiSquare()" << endl;

  // NB: Assumes R() is up-to-date

  //  TBD: Review correctness of summing weights 
  //     inside the channel loop?

  // TBD: per-ant/bln chiSq?

  //  cout << "VCS::chiSquare: svc().focusChan() = " << svc().focusChan() << endl;

  chiSq()=0.0;
  chiSqV()=0.0;
  sumWt()=0.0;
  nWt()=0;

  // Shapes for iteration
  IPosition shR(R().shape());
  Int nCorr=shR(0);
  Int nChan=shR(1);
  Int nRow=shR(2);

  ArrayIterator<Bool>  flag(svb().flag(),1);        // fl(chan) by (row)
  ArrayIterator<Float> wtMat(svb().weightMat(),1);  // wt(corr) by (row)
  ArrayIterator<Complex> Rit(R(),1);                // R(corr)  by (chan,row)

  Bool*  flR = svb().flagRow().data();
  Bool*  fl;
  Float* wt;
  Complex *Rp;

  for (Int irow=0;irow<nRow;++irow) { 
    if (!*flR) {
      // This row's wt(corr), flag(chan)
      wt = wtMat.array().data(); 
      //      wt[1]=wt[2]=0.0;
      fl = flag.array().data() + svc().focusChan();
      // Register R for this row, 0th channel
      for (Int ich=0;ich<nChan;++ich) {
	if (!*fl) { 

	  Rp = Rit.array().data();
	  for (Int icorr=0;icorr<nCorr;++icorr) {

   /*
	    if (svb().weightMat()(icorr,irow)>0.0) {
	      cout << irow << " " << icorr << " "
		   << svb().weightMat()(icorr,irow) << " "
		   << *wt << " "
		   << R()(icorr,0,irow) << " "
		   << *Rp << " "
		   << &R()(icorr,0,irow) << " "
		   << Rp << " "
		   << Rp-&R()(icorr,0,irow) << "    "
		   << &R()(0,0,irow)-&R()(3,0,irow) << " "
		   << Rit.array().shape()
		   << endl;
	    }
   */
	    chiSq()+=Double( (*wt)*real((*Rp)*conj(*Rp)) );
	    //	    chiSq()+=Double( real((*Rp)*conj(*Rp)) );
	    chiSqV()(icorr)+=Double( (*wt)*real((*Rp)*conj(*Rp)) );
	    sumWt()+=Double(*wt);   // for each channel?!

	    if (*wt>0.0) nWt()++;
	    
	    //Advance to next corr
	    ++wt;
	    ++Rp;
	  }
	  // Use same wt(corr) vectors for each channel
	  wt-=nCorr;
	}
	// Advance to next channel
	Rit.next();
	++fl;
      }
    }
    else 
      // Advance over flagged row!
      for (Int ich=0;ich<nChan;++ich) Rit.next();
    
    // Advance to next row
    ++flR;
    flag.next();
    wtMat.next();
  }


}

void VisCalSolver::chiSquare2() {

  if (prtlev()>2) cout << "   VCS::chiSquare(CVB version)" << endl;

  // NB: Assumes R() is up-to-date

  //  TBD: Review correctness of summing weights 
  //     inside the channel loop?

  // TBD: per-ant/bln chiSq?

  //  cout << "VCS::chiSquare: svc().focusChan() = " << svc().focusChan() << endl;

  chiSq()=0.0;
  chiSqV()=0.0;
  sumWt()=0.0;
  nWt()=0;

  // Loop over CVBs
  for (Int ibuf=0;ibuf<vbga().nBuf();++ibuf) {

    // Focus on one CVB at a time
    CalVisBuffer& cvb(vbga()(ibuf));

    R().reference(cvb.residuals());

    // Shapes for iteration
    IPosition shR(R().shape());
    Int nCorr=shR(0);
    Int nChan=shR(1);
    Int nRow=shR(2);
    
    ArrayIterator<Bool>  flag(cvb.residFlag(),1);        // fl(chan) by (row)
    ArrayIterator<Float> wtMat(cvb.weightMat(),1);  // wt(corr) by (row)
    ArrayIterator<Complex> Rit(R(),1);                // R(corr)  by (chan,row)
    
    Bool*  flR = cvb.flagRow().data();
    Bool*  fl;
    Float* wt;
    Complex *Rp;
    
    for (Int irow=0;irow<nRow;++irow) { 
      if (!*flR) {
	// This row's wt(corr), flag(chan)
	wt = wtMat.array().data(); 
	fl = flag.array().data();
	// Register R for this row, 0th channel
	for (Int ich=0;ich<nChan;++ich) {
	  if (!*fl) { 
	    
	    Rp = Rit.array().data();
	    for (Int icorr=0;icorr<nCorr;++icorr) {
	      
	      chiSq()+=Double( (*wt)*real((*Rp)*conj(*Rp)) );
	      chiSqV()(icorr)+=Double( (*wt)*real((*Rp)*conj(*Rp)) );
	      sumWt()+=Double(*wt);   // for each channel?!
	      
	      if (*wt>0.0) nWt()++;
	      
	      //Advance to next corr
	      ++wt;
	      ++Rp;
	    }
	    // Use same wt(corr) vectors for each channel
	    wt-=nCorr;
	  }
	  // Advance to next channel
	  Rit.next();
	  ++fl;
	}
      }
      else 
	// Advance over flagged row!
	for (Int ich=0;ich<nChan;++ich) Rit.next();
      
      // Advance to next row
      ++flR;
      flag.next();
      wtMat.next();
    }
  } // ibuf
}


Bool VisCalSolver::converged() {

  if (prtlev()>2) cout << "    VCS::converged()" << endl;

  // Change in chi2
  dChiSq() = chiSq()-lastChiSq();
  Float fChiSq(dChiSq()/chiSq());

  //  if (prtlev()>0)
  //    cout << "chiSq: " << chiSq() << " " << chiSqV() << " " << lambda() << endl;

  // Consider convergence if chi2 decreases...
  //  if (dChiSq()<=0.0) {
  if (fChiSq<=0.001) {

    // ...and the change is small:
    if (abs(dChiSq()) < 0.1*chiSq()) {
      ++cvrgcount_;

      //      if (cvrgcount_==2) lambda()=2.0;

      // Four such steps we believe we have converged!
      //      if (cvrgcount_>3)
      //if (cvrgcount_>10)
      if (cvrgcount_>5)
	return true;
    }

    if (prtlev()>0)
      cout << "     Good: chiSq=" << chiSq()
	   << " dChiSq=" << dChiSq()
	   << " fChiSq=" << dChiSq()/chiSq()
	   << " cvrgcnt=" << cvrgcount_
	   << " lambda=" << lambda()
	   << endl;
     
  }
  else {
    // (chi2 increased)

    // If a large increase, don't anticipate yet
    if (abs(dChiSq()) > 0.1*chiSq())
      cvrgcount_=0;
    else {
      // anticipate a little less if upward change is small
      //  TBD:  is this right?
      --cvrgcount_;
      cvrgcount_=max(cvrgcount_,0);  // never less than zero
    }

    if (prtlev()>0)
      cout << "     Bad:  chiSq=" << chiSq()
	   << " dChiSq=" << dChiSq()
	   << " fChiSq=" << dChiSq()/chiSq()
	   << " cvrgcnt=" << cvrgcount_
	   << " lambda=" << lambda()
	   << endl;


  }

  // Not yet converged
  return false;

}


void VisCalSolver::accGradHess() {

  if (prtlev()>2) cout << "     VCS::accGradHess()" << endl;

  grad()=0.0;
  hess()=0.0;

  IPosition dRip(dR().shape());

  Int nRow(dRip(3));
  Int nChan(dRip(2));
  Int nPar(dRip(1));   // pars per antenna
  Int nCorr(dRip(0));

  ArrayIterator<Bool>  flag(svb().flag(),1);        // fl(chan) by (row)
  ArrayIterator<Float> wtMat(svb().weightMat(),1);  // wt(corr) by (row)

  ArrayIterator<Complex> Rit(R(),1);       // R(corr)       by (chan,row)
  
  Array<Complex> dR0, dR1;
  {
    ArrayIterator<Complex> dRit(dR(),4);  // dR(corr,par,chan,row) by (ant)
    dR0.reference(dRit.array());
    dRit.next();
    dR1.reference(dRit.array());
  }
  ArrayIterator<Complex> dR0it(dR0,2);   // dR0(corr,par) by (chan,row)
  ArrayIterator<Complex> dR1it(dR1,2);   // dR1(corr,par) by (chan,row)

  Bool*  flR = svb().flagRow().data();
  Bool*  fl;
  Float* wt;
  Int*   a1 = svb().antenna1().data();
  Int*   a2 = svb().antenna2().data();
  Complex *Rp;
  Complex *dR0p;
  Complex *dR1p;
  DComplex *G1;
  DComplex *G2;
  Double *H1;
  Double *H2;

  //  cout << "flag.array().data() = " << flag.array().data() << endl;
  //  cout << "VCS::accGradHess:  svc().focusChan()   = " << svc().focusChan() << endl;

  for (Int irow=0;irow<nRow;++irow) { 
    if (!*flR) {
      // Register grad, hess for ants in this baseline
      Int p1((*a1)*nPar);
      Int p2((*a2)*nPar);
      G1 = grad().data() + p1;
      G2 = grad().data() + p2;
      H1 = hess().data() + p1;
      H2 = hess().data() + p2;
      // This row's wt(corr), flag(chan)
      wt = wtMat.array().data(); 
      fl = flag.array().data() + svc().focusChan();
      for (Int ich=0;ich<nChan;++ich) {
	if (!*fl) { 

	  // Do source bits, if necessary
	  if (svc().solvePol()) {
	    for (Int icorr=0;icorr<nCorr;++icorr) {
	      Float swt=Vector<Float>(wtMat.array())(icorr);

	      for (Int ispar=0;ispar<nSrcPar();++ispar) {
		grad()(nCalPar()+ispar) += DComplex((swt)*2.0*real(dSrc()(IPosition(4,icorr,ich,irow,ispar))*
								   conj(R()(icorr,ich,irow))));
		hess()(nCalPar()+ispar) += Double((swt)*2.0*real(dSrc()(IPosition(4,icorr,ich,irow,ispar))*
								 conj(dSrc()(IPosition(4,icorr,ich,irow,ispar)))));
	      }
	    }
	  }

	  // Register R,dR for this channel, row
	  Rp = Rit.array().data();
	  dR0p = dR0it.array().data();
	  dR1p = dR1it.array().data();

   /*
	  cout << "irow=" << irow << endl;
	  cout << "R=" << Rit.array() << endl;
	  cout << "dR0=" << dR0it.array()
	       << "dR1=" << dR1it.array()
	       << endl;
   */

	  for (int ip=0;ip<nPar;++ip) {

	    for (Int icorr=0;icorr<nCorr;++icorr) {

	      (*G1) += DComplex( (*wt)*((*Rp)  *conj(*dR0p)) );
	      (*G2) += DComplex( (*wt)*((*dR1p)*conj(*Rp)) );
	      (*H1) +=   Double( (*wt)*real((*dR0p)*conj(*dR0p)) );
	      (*H2) +=   Double( (*wt)*real((*dR1p)*conj(*dR1p)) );
     /*
	      (*G1) += DComplex( (*Rp)  *conj(*dR0p) );
	      (*G2) += DComplex( (*dR1p)*conj(*Rp) );
	      (*H1) +=   Double( real((*dR0p)*conj(*dR0p)) );
	      (*H2) +=   Double( real((*dR1p)*conj(*dR1p)) );
     */
    

	      //Advance to next corr
	      ++wt;
	      ++Rp;
	      ++dR0p;++dR1p;
	    }
	    // Advance to next par
	    ++G1; ++G2;
	    ++H1; ++H2;
	    // Use same Rp(corr), wt(corr) vectors for each parameter
	    Rp-=nCorr;
	    wt-=nCorr;
	  }
	  // Accumulate to same grad(par) & hess(par) for each channel
	  G1-=nPar; G2-=nPar;
	  H1-=nPar; H2-=nPar;
	}
	// Advance to next channel
	++fl;
	Rit.next();
	dR0it.next();
	dR1it.next();
      }
    } // !*flgR
    else {
      // Advance over flagged row
      for (Int ich=0;ich<nChan;++ich) {
	Rit.next();
	dR0it.next();
	dR1it.next();
      }
    }
    // Advance to next row
    ++flR;
    ++a1;++a2;
    flag.next();
    wtMat.next();
  }

  if (prtlev()>4) {  // grad, hess
    cout << "      grad= " << grad() << endl;
    cout << "      hess= " << hess() << endl;
  }


/*  
  // This is the slower way with array indexing
  for (Int irow=0;irow<nRow;++irow,++flR,++a1,++a2) {
    if (!*flR) {
      par1=(*a1)*nPar;
      par2=(*a2)*nPar;
      for (Int ich=0;ich<nChan;++ich,++fl) {
	if (!*fl) {
	  for (int ip=0;ip<nPar;++ip) {
	    for (Int icorr=0;icorr<nCorr;++icorr) {
	      grad(par1+ip) += ( Double(wt(icorr,irow))*
				 Double( R()(icorr,ich,irow  )*
					 conj(dR()(icorr,ip,ich,irow,0)) ) );
	      grad(par2+ip) += ( Double(wt(icorr,irow))*
				 Double( dR()(icorr,ip,ich,irow,1)*
					 conj(R()(icorr,ich,irow  )) ) );
	      
	      hess(par1+ip) += ( Double(wt(icorr,irow))*
				 Double( dR()(icorr,ip,ich,irow,0)*
					 conj(dR()(icorr,ip,ich,irow,0)) ) );
	      hess(par2+ip) += ( Double(wt(icorr,irow))*
				 Double( dR()(icorr,ip,ich,irow,1)*
					 conj(dR()(icorr,ip,ich,irow,1)) ) );
	    }
	  }
	}
      }
    }
    else {
      // Advance over flagged row
      fl+=nChan;
    }

  }
*/


}


void VisCalSolver::accGradHess2() {

  if (prtlev()>2) cout << "     VCS::accGradHess(CVB version)" << endl;

  grad()=0.0;
  hess()=0.0;

  for (Int ibuf=0;ibuf<vbga().nBuf();++ibuf) {

    CalVisBuffer& cvb(vbga()(ibuf));

    R().reference(cvb.residuals());
    dR().reference(cvb.diffResiduals());

    
    IPosition dRip(dR().shape());
    
    Int nRow(dRip(3));
    Int nChan(dRip(2));
    Int nPar(dRip(1));   // pars per antenna
    Int nCorr(dRip(0));
    
    ArrayIterator<Bool>  flag(cvb.residFlag(),1);        // fl(chan) by (row)
    ArrayIterator<Float> wtMat(cvb.weightMat(),1);  // wt(corr) by (row)
    
    ArrayIterator<Complex> Rit(R(),1);       // R(corr)       by (chan,row)
    
    Array<Complex> dR0, dR1;
    {
      ArrayIterator<Complex> dRit(dR(),4);  // dR(corr,par,chan,row) by (ant)
      dR0.reference(dRit.array());
      dRit.next();
      dR1.reference(dRit.array());
    }
    ArrayIterator<Complex> dR0it(dR0,2);   // dR0(corr,par) by (chan,row)
    ArrayIterator<Complex> dR1it(dR1,2);   // dR1(corr,par) by (chan,row)
    
    Bool*  flR = cvb.flagRow().data();
    Bool*  fl;
    Float* wt;
    Int*   a1 = cvb.antenna1().data();
    Int*   a2 = cvb.antenna2().data();
    Complex *Rp;
    Complex *dR0p;
    Complex *dR1p;
    DComplex *G1;
    DComplex *G2;
    Double *H1;
    Double *H2;

    for (Int irow=0;irow<nRow;++irow) { 
      if (!*flR) {
	// Register grad, hess for ants in this baseline
	Int p1((*a1)*nPar);
	Int p2((*a2)*nPar);
	G1 = grad().data() + p1;
	G2 = grad().data() + p2;
	H1 = hess().data() + p1;
	H2 = hess().data() + p2;
	// This row's wt(corr), flag(chan)
	wt = wtMat.array().data(); 
	fl = flag.array().data();
	for (Int ich=0;ich<nChan;++ich) {
	  if (!*fl) { 
	    
	    // Do source bits, if necessary
	    if (svc().solvePol()) {
	      for (Int icorr=0;icorr<nCorr;++icorr) {
		Float swt=Vector<Float>(wtMat.array())(icorr);
		
		for (Int ispar=0;ispar<nSrcPar();++ispar) {
		  grad()(nCalPar()+ispar) += DComplex((swt)*2.0*real(dSrc()(IPosition(4,icorr,ich,irow,ispar))*
								     conj(R()(icorr,ich,irow))));
		  hess()(nCalPar()+ispar) += Double((swt)*2.0*real(dSrc()(IPosition(4,icorr,ich,irow,ispar))*
								   conj(dSrc()(IPosition(4,icorr,ich,irow,ispar)))));
		}
	      }
	    }
	    
	    // Register R,dR for this channel, row
	    Rp = Rit.array().data();
	    dR0p = dR0it.array().data();
	    dR1p = dR1it.array().data();
	    
	    for (int ip=0;ip<nPar;++ip) {
	      
	      for (Int icorr=0;icorr<nCorr;++icorr) {
		
		(*G1) += DComplex( (*wt)*((*Rp)  *conj(*dR0p)) );
		(*G2) += DComplex( (*wt)*((*dR1p)*conj(*Rp)) );
		(*H1) +=   Double( (*wt)*real((*dR0p)*conj(*dR0p)) );
		(*H2) +=   Double( (*wt)*real((*dR1p)*conj(*dR1p)) );
		
		//Advance to next corr
		++wt;
		++Rp;
		++dR0p;++dR1p;
	      }
	      // Advance to next par
	      ++G1; ++G2;
	      ++H1; ++H2;
	      // Use same Rp(corr), wt(corr) vectors for each parameter
	      Rp-=nCorr;
	      wt-=nCorr;
	    }
	    // Accumulate to same grad(par) & hess(par) for each channel
	    G1-=nPar; G2-=nPar;
	    H1-=nPar; H2-=nPar;
	  }
	  // Advance to next channel
	  ++fl;
	  Rit.next();
	  dR0it.next();
	  dR1it.next();
	}
      } // !*flgR
      else {
	// Advance over flagged row
	for (Int ich=0;ich<nChan;++ich) {
	  Rit.next();
	  dR0it.next();
	  dR1it.next();
	}
      }
      // Advance to next row
      ++flR;
      ++a1;++a2;
      flag.next();
      wtMat.next();
    }
    
    if (prtlev()>4) {  // grad, hess
      cout << "      grad= " << grad() << endl;
      cout << "      hess= " << hess() << endl;
    }    

  } // ibuf
}

void VisCalSolver::revert() {

  if (prtlev()>2) cout << "     VCS::revert()" << endl;

  // Recall the last decent parameter set
  //  TBD: the OK flag?
  par()=lastCalPar();
  if (svc().solvePol())
    srcPar()=lastSrcPar();
}

void VisCalSolver::solveGradHess() {

  if (prtlev()>2) cout << "      VCS::solveGradHess()" << endl;

  // TBD: explicit option to avoid lmfact?
  // TBD: pointer (or MaskedArray?) optimization?

  Double lmfact(1.0+lambda());

  lmfact=2.0;

  dpar()=Complex(0.0);
  for (Int ipar=0; ipar<nCalPar(); ipar++) {
    if ( parOK()(ipar) && hess()(ipar)!=0.0) {
      // good hess for this par:
      dpar()(ipar) = grad()(ipar)/hess()(ipar);
      dpar()(ipar)/=lmfact;
    }
    else {
      dpar()(ipar)=0.0; 
      parOK()(ipar)=false;
    }
  }
  
  // Turn off source pol update for the moment
  if (svc().solvePol() && false) 
    for (Int ipar=nCalPar(); ipar<nTotalPar(); ipar++) {
      if ( hess()(ipar)!=0.0) {
	// good hess for this par:
	dpar()(ipar) = grad()(ipar)/hess()(ipar);
	dpar()(ipar)/=lmfact;
      }
      else {
	dpar()(ipar)=0.0; 
      }
    }

  // Negate (so updatePar() can _add_)
  dpar()*=Complex(-1.0f);

}

void VisCalSolver::updatePar() {

  if (prtlev()>2) cout << "       VCS::updatePar()" << endl;

  //  if (prtlev()>4) cout << "        old =" << par() << endl;

  //  if (prtlev()>4) cout << "        dpar=" << dpar() << endl;


  //  cout << "dCalPar() = " << dCalPar() << endl;
  //  cout << "dSrcPar() = " << dSrcPar() << endl;

  // Tell svc to update the par 
  //   (permits svc() to condition the current solutions)
  svc().updatePar(dCalPar(),dSrcPar());

  if (prtlev()>4) 
    cout << "        new =" << endl
	 << "amp = " << amplitude(par()) << endl
	 << "pha = " << phase(par()) << endl;

}


void VisCalSolver::optStepSize() {

  Vector<Double> x2(3,-999.0);
  Float step(1.0);
  
  // Starting point is curr chiSq
  x2(0)=chiSq();

  // take nominal step
  par()+=dCalPar();  
  if (svc().solvePol()) srcPar()+=dSrcPar();  
  residualate();
  chiSquare();
  x2(1)=chiSq();

  // If nominal step is an improvement...
  if (x2(1)<x2(0)) {

    // ...double step size until x2 starts increasing
    par()=dCalPar(); par()*=Complex(2.0*step); par()+=lastCalPar();
    if (svc().solvePol()) { srcPar()=dSrcPar(); srcPar()*=Complex(2.0*step); srcPar()+=lastSrcPar(); };
    residualate();
    chiSquare();
    x2(2)=chiSq();
    //    cout <<   "  down:    " << step << " " << x2-x2(0) << LogicalArray(x2>=x2(0)) <<endl;
    while (x2(2)<x2(1)) {    //  && step<4.0) {
      step*=2.0;
      par()=dCalPar(); par()*=Complex(2.0*step); par()+=lastCalPar();
      if (svc().solvePol()) { srcPar()=dSrcPar(); srcPar()*=Complex(2.0*step); srcPar()+=lastSrcPar(); };
      residualate();
      chiSquare();
      x2(1)=x2(2);
      x2(2)=chiSq();
      //      cout << "  stretch: " << step << " " << x2-x2(0) << LogicalArray(x2>=x2(0)) <<endl;

    }
  }
  // else nominal step too big, so...
  else {

    // ... contract by halves until we bracket a minimum
    step*=0.5;
    par()=dCalPar(); par()*=Complex(step); par()+=lastCalPar();
    if (svc().solvePol()) {srcPar()=dSrcPar(); srcPar()*=Complex(step); srcPar()+=lastSrcPar();};
    residualate();
    chiSquare();
    x2(2)=x2(1);
    x2(1)=chiSq();
    //    cout <<   "  up:       " << step << " " << x2-x2(0) << LogicalArray(x2>=x2(0)) <<endl;
    while (x2(1)>x2(0)) { //  && step>0.125) {
      step*=0.5;
      par()=dCalPar(); par()*=Complex(step); par()+=lastCalPar();
      if (svc().solvePol()) {srcPar()=dSrcPar(); srcPar()*=Complex(step); srcPar()+=lastSrcPar();};
      residualate();
      chiSquare();
      x2(2)=x2(1);
      x2(1)=chiSq();
      //      cout << "  contract: " << step << " " << x2-x2(0) << LogicalArray(x2>=x2(0)) <<endl;
    }

  }

  // At this point   x2(0) > x2(1) < x2(2), so 
  //   calculate (quadratic) step optimization factor
  Double optfactor(0.0);
  Double optn(x2(2)-x2(1));
  Double optd(x2(0)-2*x2(1)+x2(2));
	      
  if (abs(optd)>0.0)
    optfactor=Double(step)*(1.5-optn/optd);
  
  /*  
    cout << "Optimization: " 
       << step << " " 
       << optfactor << " "
       << x2 << " "
       << "(" << min(amplitude(lastPar())) << ") "
       << max(amplitude(dpar())/amplitude(lastPar()))*180.0/C::pi << " ";
  */

  par()=lastCalPar();
  srcPar()=lastSrcPar();
  
  // Adjust step by the optfactor
  if (optfactor>0.0)
    dpar()*=Complex(optfactor);

  /*
  cout << max(amplitude(dpar())/amplitude(lastPar()))*180.0/C::pi
       << endl;
  */
}

void VisCalSolver::optStepSize2() {

  Vector<Double> x2(3,-999.0);
  Float step(1.0);
  
  // Starting point is curr chiSq
  x2(0)=chiSq();

  // take nominal step
  par()+=dCalPar();  
  if (svc().solvePol()) srcPar()+=dSrcPar();  
  residualate2();
  chiSquare2();
  x2(1)=chiSq();

  // If nominal step is an improvement...
  if (x2(1)<x2(0)) {

    // ...double step size until x2 starts increasing
    par()=dCalPar(); par()*=Complex(2.0*step); par()+=lastCalPar();
    if (svc().solvePol()) { srcPar()=dSrcPar(); srcPar()*=Complex(2.0*step); srcPar()+=lastSrcPar(); };
    residualate2();
    chiSquare2();
    x2(2)=chiSq();
    //    cout <<   "  down:    " << step << " " << x2-x2(0) << LogicalArray(x2>=x2(0)) <<endl;
    while (x2(2)<x2(1)) {    //  && step<4.0) {
      step*=2.0;
      par()=dCalPar(); par()*=Complex(2.0*step); par()+=lastCalPar();
      if (svc().solvePol()) { srcPar()=dSrcPar(); srcPar()*=Complex(2.0*step); srcPar()+=lastSrcPar(); };
      residualate2();
      chiSquare2();
      x2(1)=x2(2);
      x2(2)=chiSq();
      //      cout << "  stretch: " << step << " " << x2-x2(0) << LogicalArray(x2>=x2(0)) <<endl;

    }
  }
  // else nominal step too big, so...
  else {

    // ... contract by halves until we bracket a minimum
    step*=0.5;
    par()=dCalPar(); par()*=Complex(step); par()+=lastCalPar();
    if (svc().solvePol()) {srcPar()=dSrcPar(); srcPar()*=Complex(step); srcPar()+=lastSrcPar();};
    residualate2();
    chiSquare2();
    x2(2)=x2(1);
    x2(1)=chiSq();
    //    cout <<   "  up:       " << step << " " << x2-x2(0) << LogicalArray(x2>=x2(0)) <<endl;
    while (x2(1)>x2(0)) { //  && step>0.125) {
      step*=0.5;
      par()=dCalPar(); par()*=Complex(step); par()+=lastCalPar();
      if (svc().solvePol()) {srcPar()=dSrcPar(); srcPar()*=Complex(step); srcPar()+=lastSrcPar();};
      residualate2();
      chiSquare2();
      x2(2)=x2(1);
      x2(1)=chiSq();
      //      cout << "  contract: " << step << " " << x2-x2(0) << LogicalArray(x2>=x2(0)) <<endl;
    }

  }

  // At this point   x2(0) > x2(1) < x2(2), so 
  //   calculate (quadratic) step optimization factor
  Double optfactor(0.0);
  Double optn(x2(2)-x2(1));
  Double optd(x2(0)-2*x2(1)+x2(2));
	      
  if (abs(optd)>0.0)
    optfactor=Double(step)*(1.5-optn/optd);
  
  /*  
    cout << "Optimization: " 
       << step << " " 
       << optfactor << " "
       << x2 << " "
       << "(" << min(amplitude(lastPar())) << ") "
       << max(amplitude(dpar())/amplitude(lastPar()))*180.0/C::pi << " ";
  */

  par()=lastCalPar();
  srcPar()=lastSrcPar();
  
  // Adjust step by the optfactor
  if (optfactor>0.0)
    dpar()*=Complex(optfactor);

  /*
  cout << max(amplitude(dpar())/amplitude(lastPar()))*180.0/C::pi
       << endl;
  */
}

void VisCalSolver::getErrors() {

  // Number of *REAL* dof
  //  Int nDOF=2*(nWt()-ntrue(parOK()));  // !!!! this is zero for 3 antennas!
  Int nDOF=max(2*(nWt()-ntrue(parOK())), 1u);

  Double k2=chiSq()/Double(nDOF);

  parErr()=0.0;
  //  Vector<Double> snr(nTotalPar(),0.0);
  for (Int i=0;i<nCalPar();++i) 
    if (hess()(i)>0.0) {
      parErr()(i)=1.0/sqrt(hess()(i)/k2/2.0);   // 2 is from def of Hess!
      //      snr(i)=Double(abs(par()(i)))/errs(i);
    }


  if (false) {

    cout << "ChiSq  = " << chiSq() << endl;
    cout << "ChiSqV = " << chiSqV() << endl;
    cout << "sumWt  = " << sumWt() << endl;
    cout << "nWt    = " << nWt()
	 << "; nTotalPar() = " << nTotalPar() 
	 << "; nParOK = " << ntrue(parOK())
	 << "; nDOF = " << nDOF 
	 << endl;
    
    cout << "rChiSq = " << k2 << endl;
    cout << "max(dpar) = " << max(amplitude(dpar())) << endl;
    cout << "Amplitudes = " << amplitude(par()) << endl;
    cout << "Errors = " << mean(parErr()(parOK())) << endl;
    
  }
}


} //# NAMESPACE CASA - END

