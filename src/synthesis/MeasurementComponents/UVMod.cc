//# UVMod.cc: Implementation of UV Modelling within calibrater
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
//# $Id$

#include <synthesis/MeasurementComponents/UVMod.h>

#include <msvis/MSVis/VisibilityIterator.h>
#include <msvis/MSVis/VisSet.h>
#include <msvis/MSVis/VisBuffer.h>
#include <casacore/casa/Quanta/Quantum.h>
#include <casacore/casa/Quanta/QuantumHolder.h>
#include <casacore/measures/Measures.h>
#include <casacore/measures/Measures/MDirection.h>
#include <casacore/measures/Measures/MEpoch.h>

#include <casacore/casa/Arrays/ArrayMath.h>
#include <casacore/casa/Arrays/ArrayLogical.h>
#include <casacore/casa/Arrays/ArrayIter.h>
#include <casacore/scimath/Mathematics/MatrixMathLA.h>
#include <casacore/casa/BasicSL/String.h>
#include <casacore/casa/BasicMath/Math.h>
#include <casacore/casa/Utilities/Assert.h>
#include <casacore/casa/Quanta/MVTime.h>
#include <casacore/casa/Exceptions/Error.h>
#include <casacore/casa/OS/Memory.h>
#include <casacore/casa/OS/Path.h>

#include <components/ComponentModels/ComponentType.h>
#include <components/ComponentModels/Flux.h>
#include <components/ComponentModels/ComponentShape.h>
#include <components/ComponentModels/TwoSidedShape.h>
#include <components/ComponentModels/PointShape.h>
#include <components/ComponentModels/GaussianShape.h>
#include <components/ComponentModels/DiskShape.h>
#include <components/ComponentModels/SpectralModel.h>
#include <components/ComponentModels/ConstantSpectrum.h>
#include <components/ComponentModels/SpectralIndex.h>
#include <components/ComponentModels/SkyCompBase.h>
#include <components/ComponentModels/SkyCompRep.h>
#include <components/ComponentModels/SkyComponent.h>
#include <components/ComponentModels/ComponentList.h>

#include <sstream>

#include <casacore/casa/Logging/LogMessage.h>
#include <casacore/casa/Logging/LogSink.h>


using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN


// **********************************************************
//  UVMod Implementations
//

UVMod::UVMod(VisSet& vs) :
  vs_(NULL),
  cl_(NULL),
  svb_(NULL),
  fitfld_(-1),
  pc_(),
  nPar_(0),
  R_(),
  dR_(),
  chiSq_(0.0),
  lastChiSq_(0.0),
  sumWt_(0.0),
  nWt_(0),
  polWt_(4,false),
  par_(), lastPar_(),
  lamb_(0.001),  grad_(), lastGrad_(),
  hess_(), lastHess_(),
  dpar_(),
  vary_(),
  nVary_(0)
{
  //  cout << "UVMod ctor " << endl;

  // Local copy of VisSet, with correct chunking
  Block<Int> columns;
  columns.resize(4);
  columns[0]=MS::ARRAY_ID;
  columns[1]=MS::FIELD_ID;
  columns[2]=MS::DATA_DESC_ID;
  columns[3]=MS::TIME;

  vs_ = new VisSet(vs,columns,3600.0);

  // The VisIter/VisBuffer
  VisIter& vi(vs_->iter());
  VisBuffer vb(vi);
  vi.originChunks();
  vi.origin();

  // Get phase center & direction ref:
  pc_=vb.phaseCenter();

  // Check for single field (for now)
  fitfld()=vb.fieldId();
  for (vi.originChunks();
       vi.moreChunks();
       vi.nextChunk()) {
    vi.origin();
    if (vb.fieldId()!=fitfld())
      throw(AipsError("More than one field Id in selected data"));
  }

}

UVMod::~UVMod() {
 
  // We are only responsible for the ComponentList and VisSet
  if (cl_) delete cl_;  cl_=NULL;
  if (vs_) delete vs_;  vs_=NULL;

}
void UVMod::setModel(const ComponentType::Shape type, 
		     const Vector<Double> inpar,
		     const Vector<Bool> invary) {

  //  cout << "UVM::setModel" << endl;
  //  cout << " type = " << type << endl;
  //  cout << " inpar  = " << inpar << endl;

  // Create empty componentlist
  cl_ = new ComponentList();

  par().resize(inpar.shape());
  par()=inpar;

  // Make comp
  SkyComponent comp(type);

  switch (type) {
  case ComponentType::POINT:
    {
      // A point has 3 pars
      nPar()=3;

      // Insist par().length()=3
      if (inpar.nelements()!=3)  
	throw(AipsError("Wrong number of parameters for Point; need 3."));

      break;
    };
  
  case ComponentType::GAUSSIAN:
  case ComponentType::DISK:
    {

      nPar()=6;

      // Insist par.length()=6
      if (inpar.nelements()!=6)  
	throw(AipsError("Wrong number of parameters for resolved component; need 6."));

      // Convert pa to radians
      par()(5)*=(C::pi/180.0);
      
      // Set shape pars (all in rad)
      Vector<Double> gpar(3,0.0);  //  a, b, pa
      gpar(0)=par()(3)*(C::pi/180.0/60.0/60.0);  
      gpar(1)=gpar(0)*par()(4);                  
      gpar(2)=par()(5);
      comp.shape().setParameters(gpar);

      break;
    };
  default:
    {
      throw(AipsError("Unrecognized component type in UVMod::setModel."));
      break;
    }

  }

  // Set I
  comp.flux().setValue(par()(0));


  // Add to list
  cl().add(comp);


  // Convert direction from arcsec to radians
  //  par()(1)*=(C::pi/180.0/60.0/60.0);
  //  par()(2)*=(C::pi/180.0/60.0/60.0);

  // Render direction as an MDirection; use phase-center's DirRef
  MVDirection mvdir(par()(1)*(C::pi/180.0/60.0/60.0),par()(2)*(C::pi/180.0/60.0/60.0));
  MDirection dir(mvdir,pc().getRef());

  // Set direction in component
  skycomp(0).shape().setRefDirection(dir);

  // Handle invary flags:
  vary().resize(par().shape());

  // Assume varying everything
  vary()=true;
  nVary()=nPar();

  // If invary specified, set vary() accordingly
  for (uInt i=0; i<invary.nelements();i++)
    if (!invary(i)) {
      vary()(i) = false;
      nVary()-=1;
    }
}

Bool UVMod::modelfit(const Int& maxiter, const String file) {

  //  cout << "UVMod::modelfit" << endl;

  //  cout << " maxiter = " << maxiter << endl;
  //  cout << " file    = " << file << endl;

  LogSink logsink;

  // Local ref to  VisIter/VisBuffer
  VisIter& vi(vs().iter());
  VisBuffer vb(vi);
  svb_=&vb;

  // Initialize grad, hess, etc.
  initSolve();

  {
    LogMessage message(LogOrigin("UVMod", "solve"));
    ostringstream o; o<<"Solving for single-component "
		      << skycomp(0).shape().ident();
    message.message(o);
    logsink.post(message);
  }

  Int iter(0);
  // Begin solution iterations
  //  Guarantee first pass, which gets initial chi2,
  //  then evaluate convergence by chi2 comparisons

  Bool parOK(true);
  for (Int validiter=0;validiter<=maxiter;iter++,validiter++) {
    
    //    cout << "iter =" << iter << endl;

    // If we have accumulated Hess/Grad to solve, do so
    if (iter>0) {

      // If we have done at least one real solve, invoke LM
      if (iter > 1) {
	if (parOK && (chiSq()-lastChiSq()) < DBL_EPSILON) {
	  // if last update succeeded
	  lamb()*=0.5;
	  //	  cout << "  Good iteration, decreasing lambda: " << lamb() << "  " << validiter << endl;
	  lastChiSq()=chiSq();
	  lastPar()=par();      // remember these par,hess,grad,
	  lastGrad()=grad();    //  in case we need to resolve
	  lastHess()=hess();    //  with new lamb
	} else {
	  // last update failed, so redo with new lamb
	  //	  validiter--;  // Don't count this iter as valid
	  lamb()*=10.0;

       /*
	  cout << "  Bad iteration,  increasing lambda: " << lamb();
	  if (!parOK) cout << "  (bad par update)";
	  else        cout << "  (chi2 increased)"; 
	  cout << "  " << validiter;
	  cout << endl;
       */
	  par()=lastPar();      // recover previous par,hess,grad
	  grad()=lastGrad();
	  hess()=lastHess();
	}
      } else {
	// Remember first par/grad/hess
	lastChiSq()=chiSq();
	lastPar()=par();      // remember these par,hess,grad,
	lastGrad()=grad();    //  in case we need to resolve
	lastHess()=hess();    //  with new lamb
      }

      // Do the solve 
      solveGradHess();
      parOK=updPar();
    }

    // If pars are ok, calc residuals, etc.
    if (parOK) {
      // zero current chiSq()
      chiSq()=0.0;
      sumWt()=0.0;
      hess()=0.0;
      grad()=0.0;
      nWt()=0;

      // Loop over data to accumulate Chi2, Grad, and Hess
      for (vi.originChunks();
	   vi.moreChunks();
	   vi.nextChunk()) {
	
	// This version does not pre-apply any calibration!!
	
	// Loop over VBs:
	for (vi.origin();
	     vi.more();
	     vi++) {
	  
	  // Accumulate chi2 calcuation 
	  //  (also accumulates residuals and differentiated residuals)
	  chiSquare();
	  
	  
	  // Accumulate gradients and hessian 
	  //  (uses resid and diff'd resid from above)
	  accGradHess();
	  
	}
      } // chunks
      
      
      
      
      // If sum of weights is positive, we can continue with solve
      if (sumWt()>0) {

	//	chiSq()/=sumWt();
	//	cout << "chiSq(), sumWt() = " << chiSq()<< " " << sumWt() << endl;

	// Report chi2 and pars if a good step
       	if ( (chiSq()-lastChiSq()) < DBL_EPSILON || iter==0) {
	  printPar(validiter);
	}
	else {
	  validiter--;
	  //	  printPar(validiter);
	}
      } else {
	// Escape and report no data!
	throw(AipsError("Found no data to fit!"));
      }
      
      // Don't get stuck
      if (iter>100) {
	cout << "Exceeded maximum iteration limit!" << endl;
	break;  
      }

    } 
    else
      validiter--;

  }

  //
  Double A; 
  A = chiSq()/Double(2*nWt()-nVary());
  //  grad()/=A;
  //  hess()/=A;

  lamb()=0.0;
  solveGradHess(true);

  if (false) {
  cout << "chiSq()   = " << chiSq() << endl;
  cout << "sumWt()   = " << sumWt() << endl;
  cout << "nWt()     = " << nWt() << endl;

  cout << "sumWt()/N = " << sumWt()/nWt() << endl;
  cout << "<chiSq()> = " << chiSq()/(sumWt()) << endl;

  cout << "A         = " << A  << endl;

  cout << "grad() = " << grad() << endl;
  //  cout << "hess() = " << hess() << endl;

  if (nPar()>3) {
    dpar()(5)*=(180.0/C::pi);
  }

  cout << "dpar() = " << dpar() << endl;

  cout << "cov()  = " << hess() << endl;

  Matrix<Double> corr;

  corr = hess();

  for (Int i=0; i<nPar(); i++) {
    for (Int j=i; j<nPar(); j++) {
      corr(i,j)/=sqrt(hess()(i,i)*hess()(j,j));
    }
  }

  cout << "corr    = " << corr << endl;
  }

  cout << "If data weights are arbitrarily scaled, the following formal errors" << endl
       << " will be underestimated by at least a factor sqrt(reduced chi2). If " << endl
       << " the fit is systematically poor, the errors are much worse."  << endl;

  Vector<Double> err(nPar(),0.0);
  for (Int ipar=0; ipar<nPar(); ipar++) 
    if (vary()(ipar))
      err(ipar)=sqrt(hess()(ipar,ipar));
  
  
  cout << "I = " << par()(0) << " +/- " << err(0) << endl;
  cout << "x = " << par()(1) << " +/- " << err(1) << " arcsec" << endl;
  cout << "y = " << par()(2) << " +/- " << err(2) << " arcsec" << endl;
  if (nPar()>3) {
    cout << "a = " << par()(3) << " +/- " << err(3) << " arcsec" << endl;
    cout << "r = " << par()(4) << " +/- " << err(4) << endl;
    cout << "p = " << par()(5)*180.0/C::pi << " +/- " << err(5)*180.0/C::pi << " deg" << endl;
  }

  //  }  // (false)

  // Shift pa back to deg
  if (par().nelements()>3) {
    par()(5)*=(180.0/C::pi);
  }

  // Shift model to phase center of selected field
  MDirection newdir;
  newdir=pc();
  newdir.shift(skycomp(0).shape().refDirection().getValue(),true);
  skycomp(0).shape().setRefDirection(newdir);

  // Export componentlist to file, if specified
  if (file.length()>0) {
    Path path(file);
    cout << "Writing componentlist to file: "
	 << path.absoluteName()
	 << endl;
    cl().rename(path);
  }

  return true;

}

void UVMod::initSolve() {

  //  cout << "UVM::initSolve" << endl;

  // Chi2 and weights
  chiSq()=0.0;
  sumWt()=0.0;
  nWt()=0;

  lastPar().resize(nPar());

  // Gradient and Hessian
  grad().resize(nPar());
  grad()=0.0;

  lastGrad().resize(nPar());
  lastGrad()=0.0;

  hess().resize(nPar(),nPar()); 
  hess()=0.0;

  lastHess().resize(nPar(),nPar()); 
  lastHess()=0.0;

  dpar().resize(nPar());
  dpar()=0.0;

  lamb()=0.001;

}

void UVMod::chiSquare() {

  //  cout << "UVM::chiSquare" << endl;

  // Get residuals w.r.t. current parameters
  residual();

  // Pointer access to vb elements
  const Bool*  flr=&svb().flagRow()(0);
  const Bool*  fl= &svb().flag()(0,0);
  const Int*   a1= &svb().antenna1()(0);
  const Int*   a2= &svb().antenna2()(0);
  const Float* wt= &svb().weight()(0);


  Int nCorr = R().shape()(0);

  DComplex* res;

  // Loop over row/channel
  Int irow(0),ich(0),icorr(0);
  for (irow=0;irow<svb().nRow();irow++,flr++,a1++,a2++,wt++) {
    if (!(*flr) && (*a1)!=(*a2)) { // not flagrow nor AC
      fl=&svb().flag()(0,irow);
      for (ich=0;ich<svb().nChannel();ich++,fl++) {
	if (!(*fl)) {

	  res=&R()(0,ich,irow);
	  for (icorr=0; icorr<nCorr; icorr++,res++) {

	    if ( polWt()(icorr) ) {
	      chiSq() += Double(*wt)*real((*res)*conj(*res));
	      sumWt() += Double(*wt);
	      nWt()   += 1;

	    /*
	      cout << irow << " " << *a1 << " " << *a2 << " ";
	      cout << "*res = " << *res << " ";
	      cout << innerProduct((*res),(*res)) << " ";
	      cout << chiSq()/sumWt();
	      cout << endl;
	    */

	  //	  chiSq() += real(innerProduct((*res),(*res)));
	  //	  sumWt() += 1.0;
	    }
	  }

	}
      }
    }
  }

  //  cout << "End of UVM::chiSquare" << chiSq() << endl;

}

  
//  Vobs - (M)*Vmod,  - (dM)*Vmod
void UVMod::residual() {  

  //  cout << "UVM::residual" << endl;

  // Shapes
  Int nRow= svb().nRow();
  Int nChan=svb().nChannel();

  // Data Access
  const Bool*    flagR= &svb().flagRow()(0);
  const Bool*    flag=  &svb().flag()(0,0);
  const Int*     a1= &svb().antenna1()(0);
  const Int*     a2= &svb().antenna2()(0);
  const RigidVector<Double,3>* uvw = &svb().uvw()(0);

  // Prepare residuals array
  Int nCorr = svb().correctedVisCube().shape()(0);
  R().resize(nCorr,nChan,nRow);
  convertArray(R(),svb().correctedVisCube());

  // Zero cross-hands...
  //  TBD

  // Prepare differentials array
  dR().resize(IPosition(4,nCorr,nPar(),nChan,nRow));
  dR()=DComplex(0.0);

  // Calculate wave number per frequency
  Vector<Double> freq = svb().frequency();
  Vector<Double> waveNum = C::_2pi*freq/C::c;

  //  cout << "waveNum = " << waveNum << endl;

  //  cout << "Polarizations = " << svb().corrType() << endl;


  Vector<Int> corridx = svb().corrType();

  // Ensure component is in correct pol
  ComponentType::Polarisation pol(ComponentType::CIRCULAR);
  if (svb().polFrame()==0) {
    pol=ComponentType::CIRCULAR;
    corridx-=5;
  }
  else if (svb().polFrame()==1) {
    pol=ComponentType::LINEAR;
    corridx-=9;
  }

  // Handle polarization selections (parallel hands only, for now)
  polWt().resize(nCorr);
  polWt()=false;
  polWt() = (corridx==0 || corridx==3);

  skycomp(0).flux().convertPol(pol);

  // The per-row model visibility
  Vector<DComplex> M(4);  

  // The comp flux
  Vector<DComplex> F(4);
  F = skycomp(0).flux().value();

  // The Direction visibility, derivatives
  DComplex D, dphdx, dphdy;

  // short-hand access to pars
  Vector<Double> p;
  p.reference(par());

  // Utility
  Vector<DComplex> dMdG;
  Double cospa;
  Double sinpa;
  Double dGda(0.0), G(0.0), g1(0.0),g2(0.0);
  Double pi2a2(0.0);
  if (nPar()>3)
    pi2a2=C::pi*C::pi*p(3)*p(3);
	    
  // Pointer access to R(), dR();
  DComplex  *res = &R()(0,0,0);
  DComplex *dres = &dR()(IPosition(4,0,0,0,0));

  // iterate rows
  for (Int row=0; row<nRow; row++,flagR++,uvw++,a1++,a2++) {

    if (!*flagR && (*a1)!=(*a2)) { // not flagrow nor AC

      flag = &svb().flag()(0,row);
      for (Int chn=0; chn<nChan; chn++,flag++) {
	
	// if this data channel unflagged
	if (!*flag) {

	  Vector<Double> uvw2(uvw->vector());
	  // The model visbility vector
	  M=skycomp(0).visibility(uvw2,freq(chn)).value();

	  // Scale uvw2 to 1/arcsec (dir pars are in arcsec)
	  uvw2/=(648000/C::pi);

	  // Direction phase factors (OPTIMIZED?)
	  Double phase=waveNum(chn)*( uvw2(0)*p(1)+uvw2(1)*p(2) );
	  D=DComplex(cos(phase),sin(phase));
	  dphdx=DComplex(0.0,uvw2(0)*waveNum(chn));
	  dphdy=DComplex(0.0,uvw2(1)*waveNum(chn));

	  // apply direction phase
	  M*=D;  

	  // Resolved component geometry derivatives
	  if (nPar()>3) {
	    
	    cospa=cos(p(5));
	    sinpa=sin(p(5));
	    g1=waveNum(chn)*(uvw2(0)*cospa - uvw2(1)*sinpa)/C::_2pi;
	    g2=waveNum(chn)*(uvw2(0)*sinpa + uvw2(1)*cospa)/C::_2pi;
	    dGda=C::pi*sqrt(g1*g1*p(4)*p(4) + g2*g2);
	    G=p(3)*dGda;
	    
	    // Gaussian-specific, for now
	    switch (skycomp(0).shape().type()) {
	    case ComponentType::GAUSSIAN:
	      {
		dMdG=M*DComplex(-G/2.0/C::ln2);
		break;
	      }
	    case ComponentType::DISK:
	      {
		dMdG=M*DComplex(-2.0*jn(2,G)/G);
		break;
	      }
	    default:
	      {
		break;
	      }
	    }
	  }

	  // Fill R, dR by element:
	  res  = &R()(0,chn,row);
	  dres = &dR()(IPosition(4,0,0,chn,row));
	  Int cidx(0);
	  for (Int icorr=0; icorr<nCorr; icorr++,res++,dres++) {

	    if (polWt()(icorr)) {

	      // Re-index correlations in model
	      //  (copes with partial or mis-ordered data correlations)
	      cidx=corridx(icorr);
	      
	      // The residual itself
	      *(res) -= M(cidx);
	      
	      // Flux derivative
	      *(dres) = -M(cidx)/F(cidx);
	      
	      // Direction derivatives
	      *(dres+1*nCorr) = -M(cidx)*dphdx;
	      *(dres+2*nCorr) = -M(cidx)*dphdy;
	      
	      // If fitting resolved shapes:
	      if (nPar()>3) {
		
		// 2D-comp-specific par derivatives
		
		// a
		*(dres+3*nCorr) = -dMdG(cidx)*dGda;
		
		// r = b/a
		*(dres+4*nCorr) = -dMdG(cidx)*(pi2a2*p(4)*g1*g1/G);
		
		// pa
		*(dres+5*nCorr) = -dMdG(cidx)*(pi2a2*(1.0-p(4)*p(4))*g1*g2/G);
	      }
	    }
	  }	    
	} // !*flag
      } // chn
    } // !*flagR
  } // row

  //  cout << "End of UVM::residual" << endl;


}

void UVMod::accGradHess() {

  // Assumes we've already calculated R() & dR()  (via chiSquare/residual)

  //  cout << "UVM::accGradHess" << endl;
  
  const Bool*  flagR= &svb().flagRow()(0);
  const Bool*  flag=  &svb().flag()(0,0);
  const Int*   a1=    &svb().antenna1()(0);
  const Int*   a2=    &svb().antenna2()(0);
  const Float* wt=    &svb().weight()(0);

  Int nCorr = R().shape()(0);

  DComplex *res;
  DComplex *dres1, *dres2;

  //  grad()=0.0;
  //  hess()=0.0;

  //  cout << endl;

  for (Int irow=0;irow<svb().nRow();irow++,a1++,a2++,flagR++,wt++) {
   
    if (!(*flagR)) {

      flag=  &svb().flag()(0,irow);
      for (Int ich=0;ich<svb().nChannel();ich++,flag++) {
	
	if (!(*flag)) {
	  
	  res = &R()(0,ich,irow);
	  for (Int icorr=0; icorr<nCorr; icorr++,res++) {
	    	
	    if ( polWt()(icorr) ) {
	      dres1 = &dR()(IPosition(4,icorr,0,ich,irow));
	      for (Int ipar0=0;ipar0<nPar();ipar0++,dres1+=nCorr) {
		
		// Only if this parameter varies, calc grad/hess
		if (vary()(ipar0)) {
		  grad()(ipar0)       += (Double(*wt)*real( (*res)   * conj(*dres1) ) );
		  hess()(ipar0,ipar0) += (Double(*wt)*real( (*dres1) * conj(*dres1) ) );

		  dres2 = dres1+nCorr;
		  for (Int ipar1=ipar0+1;ipar1<nPar();ipar1++,dres2+=nCorr) {
		    if (vary()(ipar1))
		      hess()(ipar0,ipar1) += (Double(*wt)*real( (*dres1) * conj(*dres2) ) );
		  }
		}
	      }
	    }
	  }
	}

      }
    }
  }

  //  cout << "grad() = " << grad().column(1) << endl;
  //  cout << "hess() = " << hess().xyPlane(1) << endl;



}

void UVMod::solveGradHess(const Bool& doCovar) {

  //  cout << "UVM::solveGradHess" << endl;

  // Treat diagonal
  for (Int ipar=0; ipar<nPar(); ipar++) {
    // Ensure non-zero diag
    if (hess()(ipar,ipar)==0.0)   // corresponds to vary()(ipar)=false
      hess()(ipar,ipar)=1.0;

    // apply lamb() to diag (if covar matrix not requested)
    if (!doCovar) hess()(ipar,ipar)*=(1.0+lamb());
  }

  //  cout << "grad() = " << grad() << endl;
  //  cout << "hess() = " << hess() << endl;


  char uplo('U');
  Int n(nPar());
  Int nrhs(1);
  Bool deleteaa, deletebb;
  Int lda(n);
  Int ldb(n);
  Int info;
  Double *aa;
  Double *bb;
    
  if (sumWt() > 0.0) {
    
    aa = hess().getStorage(deleteaa);
    bb = grad().getStorage(deletebb);
    
    posv(&uplo, &n, &nrhs, aa, &lda, bb, &ldb, &info);

    if (doCovar) potri(&uplo, &n, aa, &lda, &info);

    //      if (info!=0) cout << "info = " << info << endl;
    
    hess().putStorage(aa,deleteaa);
    grad().putStorage(bb,deletebb);
  }
  dpar()=grad();

  //  cout << "dpar() = " << dpar() << endl;

}

Bool UVMod::updPar() {

  //  cout << "UVM::updPar" << endl;

  //  cout << endl << "dpar() = " << dpar() << endl;


  // Handle "unphysical" updates:

  if (nPar() > 3 ) {
    if (dpar()(3) > par()(3))   // Keep size positive
      dpar()(3) = 0.9*par()(3);
    if (dpar()(4) > par()(4))   // Keep axial ratio positive
      dpar()(4) = 0.9*par()(4);
  }

  // Update pars 
  par()-=dpar();

  if (nPar()>3 && par()(4) > 1.0) par()(4)=1.0;  // Keep axial ration <= 1


  // Set current pars in the componentlist
  return setCompPar();

}

Bool UVMod::setCompPar() {

  // Update I flux:
  skycomp(0).flux().convertPol(ComponentType::STOKES);
  skycomp(0).flux().setValue(par()(0));

  Double rad2as(180.0*60.0*60.0/C::pi);

  // Update (relative) direction:
  MVDirection mvdir(par()(1)/rad2as,par()(2)/rad2as);
  MDirection dir(mvdir);
  skycomp(0).shape().setRefDirection(dir);

  // Set shape pars
  if (skycomp(0).shape().nParameters() > 0) {

    // Keep pa within a quarter cycle of zero
    while (par()(5) > C::pi/2.0)  par()(5)-=C::pi;
    while (par()(5) < -C::pi/2.0) par()(5)+=C::pi;
    
    Vector<Double> gpar(skycomp(0).shape().parameters());  //  a, b, pa
    //    cout << "gpar = " << gpar << endl;
    gpar(0)=par()(3)/rad2as;   // a (rad)
    gpar(1)=gpar(0)*par()(4);  // b = a*r (rad)
    gpar(2)=par()(5);          // pa (rad)


    // Now set new pars
    //    cout << "gpar = " << gpar << endl;
    try {
      skycomp(0).shape().setParameters(gpar);
    } catch (AipsError x) {
      cout << " This should never happen now:  " << x.getMesg() << endl;
      return false;
    }
  }
  return true;
}

void UVMod::printPar(const Int& iter) {

  //  cout << "UVM::printPar" << endl;
  
  // Ensure component is in Stokes:
  skycomp(0).flux().convertPol(ComponentType::STOKES);

  // Extract flux:
  Vector<Double> f;
  skycomp(0).flux().value(f);

  if (iter==0) {
    cout << "There are " 
	 << 2*nWt() << " - " 
	 << nVary() << " = " 
	 << 2*nWt()-nVary() << " degrees of freedom." 
	 << endl;
  }

  cout << " iter=" << iter << ":  ";

  //  cout << " lastchi2=" << lastChiSq() << " ";

  if (iter>0 && (chiSq()-lastChiSq())>DBL_EPSILON ) cout << "(";
  cout << " reduced chi2=" << chiSq()/Double(2*nWt()-nVary());
  if (iter>0 && (chiSq()-lastChiSq())>DBL_EPSILON ) cout << ")";

  //  cout << "(" << lamb() << ")";

  cout << ":  I=" <<  f(0)
       << ",  dir=" 
       << skycomp(0).shape().refDirection().getAngle(Unit("arcsec"));

  if (skycomp(0).shape().nParameters()>0) {
    Vector<Double> gpar(skycomp(0).shape().parameters());  //  a, b, pa
    cout << ",  shape=["
	 << gpar(0)*180.0*60.0*60.0/C::pi << ", "   // a (arcsec)
	 << gpar(1)/gpar(0) << ", "                 // r
	 << gpar(2)*180.0/C::pi << "]";             // pa (deg)
  }
  cout << endl;
    
}

} //# NAMESPACE CASA - END

