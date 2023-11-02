//# VisEquation:  Vis Equation
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
//# $Id: VisEquation.cc,v 19.18 2006/01/13 01:06:55 gmoellen Exp $


#include <synthesis/MeasurementEquations/VisEquation.h>
#include <synthesis/MeasurementComponents/SolveDataBuffer.h>
#include <casacore/casa/Arrays/ArrayMath.h>
#include <casacore/casa/Arrays/ArrayPartMath.h>
#include <casacore/casa/Utilities/Assert.h>
#include <casacore/casa/BasicSL/String.h>

#include <casacore/casa/Exceptions/Error.h>
#include <msvis/MSVis/VisBuffer.h>
//#include <casa/Quanta/MVTime.h>
#include <iostream>

#include <casacore/casa/OS/Timer.h>

#define VISEQPRTLEV 0

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

// ***************************************************************************
// ********************  Start of public member functions ********************
// ***************************************************************************

//----------------------------------------------------------------------
VisEquation::VisEquation() :
  vcpb_(NULL), 
  napp_(0),
  lfd_(-1),
  rfd_(9999),
  freqAveOK_(false),
  svc_(NULL),
  pivot_(VisCal::ALL),  // at the sky
  //  spwOK_(),
  useInternalModel_(false),
  nVisTotal_(0),
  prtlev_(VISEQPRTLEV)
{
  if (prtlev()>0) cout << "VE::VE()" << endl;
};

//----------------------------------------------------------------------
VisEquation::~VisEquation() {};

//---------------------------------------------------------------------- 
VisEquation& VisEquation::operator=(const VisEquation& other)
{
  if(this!=&other) {
    vcpb_=other.vcpb_;
    napp_=other.napp_;
    svc_=other.svc_;
  };
  return *this;
};

//----------------------------------------------------------------------
VisEquation::VisEquation(const VisEquation& other)
{
  operator=(other);
}


//----------------------------------------------------------------------
void VisEquation::setapply(PtrBlock<VisCal*>& vcin) {

  if (prtlev()>0) cout << "VE::setapply()" << endl;

  // Be sure internal pointer points to same PB
  vcpb_ = &vcin;

  // How many VisCals in list?
  napp_=vc().nelements();

  // only if at least one VisCal in list
  if (napp_>0) {

    // only sort if a non-trivial list
    if (napp_>1) {
      
      // A temporary local copy for sorting:
      PtrBlock<VisCal*> lvc;
      lvc.resize(napp_,true,true);
      lvc=vc();
      
      // Sorted index will go here
      Vector<uInt> order(napp_,0);
      
      // Fill in the sort key
      Vector<Int> key(napp_,0);
      for (Int i=0;i<napp_;i++)
	key(i)=Int(vc()[i]->type());
      
      // Do the sort
      {
	Sort sort;
	sort.sortKey(&key(0),TpInt);
	sort.sort(order,uInt(napp_));
      }
      
      // Assign VisCals in sorted order
      if (prtlev()>2) cout << "Sorted VisCals:" << endl;
      vc().set(NULL);
      for (Int i=0;i<napp_;i++) {
	vc()[i]=lvc[order(i)];
	
	if (prtlev()>2) cout << vc()[i]->typeName() << " (" << vc()[i]->type() << ")" << endl;
	vc()[i]->initCalFlagCount();
      }
      
    }
  }

  // Set up freq-dependence of the Vis Equation
  //  (for currently specified solve/apply types)
  //  TBD: only needed in setsolve?
  //  setFreqDep();

  // Initialize visibility counter
  nVisTotal_=0;

}

//----------------------------------------------------------------------
void VisEquation::setsolve(SolvableVisCal& svc) {

  if (prtlev()>0) cout << "VE::setsolve()" << endl;
  
  // VE's own pointer to solvable component
  svc_=&svc;
  
  // Set up freq-dependence of the Vis Equation
  //  (for currently specified solve/apply components)
  setFreqDep();

}


//----------------------------------------------------------------------
void VisEquation::setPivot(VisCal::Type pivot) {

  if (prtlev()>0) cout << "VE::setPivot()" << endl;
  
  pivot_ = pivot;

}
  

//----------------------------------------------------------------------
void VisEquation::setModel(const Vector<Float>& stokes) {

  if (prtlev()>0) cout << "VE::setModel()" << endl;

  // Set the internal point source model
  useInternalModel_=true;
  stokesModel_.resize(4);
  stokesModel_.set(0.0);
  stokesModel_(Slice(0,stokes.nelements(),1))=stokes;

}

//----------------------------------------------------------------------
// List the terms
Vector<VisCal::Type> VisEquation::listTypes() const {

  Vector<VisCal::Type> typelist(nTerms());
  for (Int i=0;i<napp_;++i)
    typelist(i)=(*vcpb_)[i]->type();

  return typelist;

}


//----------------------------------------------------------------------
// Get spwOK for a single spw by aggregating from the vc's
Bool VisEquation::spwOK(const Int& spw) {
  if (napp_<1) return true;  // if there are none, all is ok
  // otherwise, accumulate from the VisCals
  Bool spwok=vc()[0]->spwOK(spw);
  for (Int i=1;i<napp_;++i) 
    spwok = spwok && vc()[i]->spwOK(spw);
  return spwok;
}


//----------------------------------------------------------------------
// Report if calibration is collectively calibrateable by all VCs 
//   (including possible agnosticism by somein CalLibrary context; 
//    see SolvableVisCal::VBOKforCalApply)
Bool VisEquation::VBOKforCalApply(vi::VisBuffer2& vb) {

  Bool okForCal(True); // nominal
  for (Int i=0;i<napp_;++i) 
    okForCal = okForCal && vc()[i]->VBOKforCalApply(vb);
    
  return okForCal;

}



//----------------------------------------------------------------------
// Correct in place the OBSERVED visibilities in a VisBuffer
void VisEquation::correct(VisBuffer& vb, Bool trial) {

  if (prtlev()>0) cout << "VE::correct()" << endl;

  AlwaysAssert(ok(),AipsError);

  if (napp_==0) throw(AipsError("Nothing to Apply"));

  // Ensure correlations in canonical order
  // (this is a no-op if no sort necessary)
  vb.sortCorr();

  // Accumulate visibility count
  nVisTotal_+=(vb.nCorr()*vb.nChannel()*vb.nRow());

  // Apply each VisCal in left-to-right order 
  for (Int iapp=0;iapp<napp_;iapp++)
    vc()[iapp]->correct(vb,trial);

  // Ensure correlations restored to original order
  // (this is a no-op if no sort necessary)
  vb.unSortCorr();
  
}
//----------------------------------------------------------------------
// Correct in place the OBSERVED visibilities in a VisBuffer
void VisEquation::correct2(vi::VisBuffer2& vb, Bool trial, Bool doWtSp) {

  if (prtlev()>0) cout << "VE::correct2()" << endl;

  AlwaysAssert(ok(),AipsError);

  if (napp_==0) throw(AipsError("Nothing to Apply"));

  // Accumulate visibility count
  nVisTotal_+=(vb.nCorrelations()*vb.nChannels()*vb.nRows());

  // Apply each VisCal in left-to-right order 
  for (Int iapp=0;iapp<napp_;iapp++)
    vc()[iapp]->correct2(vb,trial,doWtSp);

}

//----------------------------------------------------------------------
// Report action record info (derived from consituent VisCals
Record VisEquation::actionRec() {
  Record cf;
  // Add in each VisCal's record
  for (Int iapp=0;iapp<napp_;iapp++)
    cf.defineRecord(iapp,vc()[iapp]->actionRec());

  // The TOTAL number of visibilities that passed through the VisEquationp
  cf.define("nVisTotal",nVisTotal_);

  return cf;
}

//----------------------------------------------------------------------
// Corrupt in place the MODEL visibilities in a VisBuffer
void VisEquation::corrupt(VisBuffer& vb) {

  if (prtlev()>0) cout << "VE::corrupt()" << endl;

  AlwaysAssert(ok(),AipsError);

  if (napp_==0) throw(AipsError("Nothing to Apply"));

  // Ensure correlations in canonical order
  // (this is a no-op if no sort necessary)
  vb.sortCorr();

  // Apply each VisCal in right-to-left order
  for (Int iapp=(napp_-1);iapp>-1;iapp--)
    vc()[iapp]->corrupt(vb);

  // Ensure correlations restored to original order
  // (this is a no-op if no sort necessary)
  vb.unSortCorr();

}

//----------------------------------------------------------------------
// Corrupt in place the MODEL visibilities in a VisBuffer
void VisEquation::corrupt2(vi::VisBuffer2& vb) {

  if (prtlev()>0) cout << "VE::corrupt2(VB2)" << endl;

  AlwaysAssert(ok(),AipsError);

  if (napp_==0) throw(AipsError("Nothing to Apply"));

  // Apply each VisCal in right-to-left order
  for (Int iapp=(napp_-1);iapp>-1;iapp--)
    vc()[iapp]->corrupt2(vb);

}

//----------------------------------------------------------------------
void VisEquation::collapse(VisBuffer& vb) {

  if (prtlev()>0) cout << "VE::collapse()" << endl;

  // Handle origin of model data here:
  if (useInternalModel_)
    vb.setModelVisCube(stokesModel_);
  else
    vb.modelVisCube();

  // Ensure required columns are present!
  vb.visCube();
  vb.weightMat();
  
  // Re-calculate weights from sigma column
  // TBD: somehow avoid if not necessary?
  vb.resetWeightMat();

  // Ensure correlations in canonical order
  // (this is a no-op if no sort necessary)
  // TBD: optimize in combo with model origination?
  vb.sortCorr();

  // If we are solving for the polarization:
  //  1. Normalize data and model by I model
  //  2. Set cross-hands to (1,0) so P factors multiplying them
  //     are propogated, and we can solve for pol factors
  if (svc().solvePol())
    svc().setUpForPolSolve(vb);

  // TBD: When warranted, do freqAve before setUpForPolSolve?
  
  // initialize LHS/RHS indices
  Int lidx=0;
  Int ridx=napp_-1;

  // If solve NOT freqDep, and data is, we want
  //  to freqAve as soon as possible before solve;
  //   apply any freqDep cal first
  if ( freqAveOK_ && !svc().freqDepMat() && vb.nChannel()>1 ) {
    
    // Correct OBSERVED data up to last freqDep term on LHS
    //  (type(lfd_) guaranteed < type(svc))
    while (lidx<napp_ && lidx <= lfd_) {
      vc()[lidx]->correct(vb);
      lidx++;
    }
    
    // Corrupt MODEL  data down to last freqDep term on RHS
    //  (type(rfd_) guaranteed >= type(svc))
    while (ridx>-1 && ridx >= rfd_) {
      vc()[ridx]->corrupt(vb);
      ridx--;
    }
    
    // All freq-dep cal has been applied, now freqAve
    vb.freqAveCubes();  // BOTH sides (if present)!
    
  }
  
  // Correct DATA up to solved-for term
  while (lidx<napp_ && vc()[lidx]->type() < svc().type()) {
    vc()[lidx]->correct(vb);
    lidx++;
  }
  
  // Corrupt MODEL down to solved-for term (incl. same type as solved-for term)
  while (ridx>-1    && vc()[ridx]->type() >= svc().type()) {
    vc()[ridx]->corrupt(vb);
    ridx--;
  }
  
}

//----------------------------------------------------------------------
void VisEquation::diffModelStokes(vi::VisBuffer2& vb, std::map<String,Cube<Complex> > dMdS) {

  Int nCorr(vb.nCorrelations());
  if (nCorr<4)
    throw(AipsError("Cannot differentiate w.r.t. Model Stokes unless data has four correlations"));

  Int nChan(vb.nChannels());
  Int nRow(vb.nRows());

  // Incoming map should be empty
  dMdS.clear();

  // Basis-dependent indexing
  Slice Isl(0,2,3);  // not basis-specific
  Slice Qsl0,Qsl1,Usl0,Usl1,Vsl0,Vsl1;
  Bool doCirc(true);
  if (vb.correlationTypes()(0)==5) {
    // Circular
    doCirc=true;  
    Qsl0=Slice(1);  // cross-hand
    Qsl1=Slice(2);
    Usl0=Slice(1);  // cross-hand
    Usl1=Slice(2);
    Vsl0=Slice(0);  // parallel-hand
    Vsl1=Slice(3);
  }
  else if (vb.correlationTypes()(0)==9) {
    // Linear
    doCirc=false;  
    Qsl0=Slice(0);  // parallel-hand
    Qsl1=Slice(3);
    Usl0=Slice(1);  // cross-hand
    Usl1=Slice(2);
    Vsl0=Slice(1);  // cross-hand
    Vsl1=Slice(2);
  }
  else
    throw(AipsError("Cannot differentiate w.r.t. Model Stokes for unrecognized polarization basis"));

  Complex cOne(1,0), cIm(0,1);

  String I("I");
  dMdS[I]=Cube<Complex>(nCorr,nChan,nRow,0.0);
  Cube<Complex>& dMdI(dMdS[I]);
  dMdI(Isl,Slice(),Slice()).set(cOne);  // not basis-specific

  String Q("Q");
  dMdS[Q]=Cube<Complex>(nCorr,nChan,nRow,0.0);
  Cube<Complex>& dMdQ(dMdS[Q]);
  dMdQ(Qsl0,Slice(),Slice()).set(cOne);
  dMdQ(Qsl1,Slice(),Slice()).set( (doCirc ? cOne : -cOne) );

  String U("U");
  dMdS[U]=Cube<Complex>(nCorr,nChan,nRow,0.0);
  Cube<Complex>& dMdU(dMdS[U]);
  dMdU(Usl0,Slice(),Slice()).set( (doCirc ? cIm : cOne) );
  dMdU(Usl1,Slice(),Slice()).set( (doCirc ? -cIm : cOne) );

  String V("V");
  dMdS[V]=Cube<Complex>(nCorr,nChan,nRow,0.0);
  Cube<Complex>& dMdV(dMdS[V]);
  dMdV(Vsl0,Slice(),Slice()).set( (doCirc ? cOne : cIm) );
  dMdV(Vsl1,Slice(),Slice()).set( (doCirc ? -cOne : -cIm) );

  Int ridx=napp_-1;
 
  // Corrupt MODEL down to solved-for term (incl. same type as solved-for term)
  while (ridx>-1    && vc()[ridx]->type() >= svc().type()) {
    vc()[ridx]->corrupt2(vb,dMdI);
    vc()[ridx]->corrupt2(vb,dMdQ);
    vc()[ridx]->corrupt2(vb,dMdU);
    vc()[ridx]->corrupt2(vb,dMdV);
    ridx--;
  }

}





//----------------------------------------------------------------------
void VisEquation::collapse2(vi::VisBuffer2& vb) {

  if (prtlev()>0) cout << "VE::collapse2(VB2)" << endl;

  // Trap case of unavailable calibration in any vc we intend to apply below
  //   In the solve context, if we can't pre-cal, we flag it
  //    NB: this assumes only one spw in the VB2!
  //if (!this->spwOK(vb.spectralWindows()(0))) {
  // Use new VBOKforCalApply, which is f(obs,fld,intent,spw) (not just f(spw))
  if (!this->VBOKforCalApply(vb)) {

    //cout << "UNCALIBRATEABLE VB2 in VE::collapse2" << endl;

    Cube<Bool> fl(vb.flagCube());          fl.set(true);
    Cube<Float> wtsp(vb.weightSpectrum()); wtsp.set(0.0f);
    Matrix<Float> wt(vb.weight());         wt.set(0.0f);
    return;
  }    

  // Handle origin of model data here:
  if (useInternalModel_)
    // Use specified (point-source) stokes model
    vb.setVisCubeModel(stokesModel_);
  else
    // from MS
    vb.visCubeModel();

  // If we are solving for the polarization:
  //  1. Normalize data and model by I model
  //  2. Set cross-hands to (1,0) so P factors multiplying them
  //     are propogated, and we can solve for pol factors
  if (svc().solvePol())
    svc().setUpForPolSolve(vb);

  // initialize LHS/RHS indices
  Int lidx=0;
  Int ridx=napp_-1;

  // Correct DATA up to solved-for term
  while (lidx<napp_ && vc()[lidx]->type() < svc().type()) {
    vc()[lidx]->correct2(vb,False,True);
    lidx++;
  }
  
  // Corrupt MODEL down to solved-for term (incl. same type as solved-for term)
  while (ridx>-1    && vc()[ridx]->type() >= svc().type()) {
    vc()[ridx]->corrupt2(vb);
    ridx--;
  }

  if (svc().normalizable())
    divideCorrByModel(vb);
  

  // Make fractional visibilities, if appropriate 
  // (e.g., for some polarization calibration solves on point-like calibrators)
  if (svc().divideByStokesIModelForSolve())
    divideByStokesIModel(vb);


}

void VisEquation::divideCorrByModel(vi::VisBuffer2& vb) {

  // This divides corrected data by model 
  //  ... and updates weightspec accordingly

  Cube<Complex> c(vb.visCubeCorrected());
  Cube<Complex> m(vb.visCubeModel());
  Cube<Bool> fl(vb.flagCube());
  Cube<Float> w(vb.weightSpectrum());

  Complex cOne(1.0);

  for (rownr_t irow=0;irow<vb.nRows();++irow) {
    if (vb.flagRow()(irow)) {
      // Row flagged, make sure cube also flagged, weight/data zeroed
      c(Slice(),Slice(),Slice(irow,1,1))=0.0f;
      w(Slice(),Slice(),Slice(irow,1,1))=0.0f;
      fl(Slice(),Slice(),Slice(irow,1,1))=True;
    }
    else {
      Bool *flp=&fl(0,0,irow);
      Float *wtp=&w(0,0,irow);
      Complex *cvp=&c(0,0,irow);
      Complex *mvp=&m(0,0,irow);

      for (Int ichan=0;ichan<vb.nChannels();++ichan) {
	for (Int icorr=0;icorr<vb.nCorrelations();++icorr) {
	  Bool& Fl(*flp);
	  Float& W(*wtp);
	  //Bool& Fl(fl(icorr,ichan,irow));
	  //Float& W(w(icorr,ichan,irow));
	  if (!Fl) {
	    // Not flagged...
	    Float A=abs(*mvp);
	    //Float A=abs(m(icorr,ichan,irow));
	    if (A >0.0f) {
	      // ...and model non-zero
	      Complex& C(*cvp);
	      Complex& M(*mvp);
	      //Complex& C(c(icorr,ichan,irow));
	      //Complex& M(m(icorr,ichan,irow));

	      // divide corr data by model
	      // NB: Use of DComplex here increased cost of this calculation by ~33%
	      C=Complex(DComplex(C)/DComplex(M));  
	      //C=C/M;  

	      W*=square(A);                        // multiply weight by model**2
	      M=cOne;                              // divide model by itself

	    }
	  }
	  else {
	    // be sure it is flagged and weightless
	    Fl=True;
	    W=0.0f;
	  }
	  ++cvp;
	  ++mvp;
	  ++flp;
	  ++wtp;
	} // icorr
      }	// ichan  
    } // !flagRow
  } // irow
  
  // Set unchan'd weight, in case someone wants it
  // NB: Use of median increases cost by ~100%
  // NB: use of mean increases cost by ~50%
  //  ...but both are inaccurate if some channels flagged,
  //  and it should not be necessary to do this here
  vb.setWeight(partialMedians(vb.weightSpectrum(),IPosition(1,1),True));
  //vb.setWeight(partialMeans(vb.weightSpectrum(),IPosition(1,1)));

}

void VisEquation::divideByStokesIModel(vi::VisBuffer2& vb) {

  Int nCorr(vb.nCorrelations());

  // This divides corrected data and model by the Stokes I model 
  //  ... and updates weightspec accordingly

  Cube<Complex> c(vb.visCubeCorrected());
  Cube<Complex> m(vb.visCubeModel());
  Cube<Bool> fl(vb.flagCube());
  Cube<Float> w(vb.weightSpectrum());

  //Complex cOne(1.0);

  for (rownr_t irow=0;irow<vb.nRows();++irow) {
    if (vb.flagRow()(irow)) {
      // Row flagged, make sure cube also flagged, weight/data zeroed
      c(Slice(),Slice(),Slice(irow,1,1))=0.0f;
      w(Slice(),Slice(),Slice(irow,1,1))=0.0f;
      fl(Slice(),Slice(),Slice(irow,1,1))=True;
    }
    else {
      // Bool *flp=&fl(0,0,irow);
      Float *wtp=&w(0,0,irow);
      Complex *cvp=&c(0,0,irow);
      Complex *mvp=&m(0,0,irow);

      for (Int ichan=0;ichan<vb.nChannels();++ichan) {
	Complex Imod(0.0);
	Float Iamp2(1.0);
	if (!fl(0,ichan,irow) && !fl(nCorr-1,ichan,irow)) {
	  Imod=(m(0,ichan,irow) + m(nCorr-1,ichan,irow))/2.0f;
	  Iamp2=real(Imod*conj(Imod));  // squared model amp (for weight adjust)
	  if (Iamp2>0.0f) {
	    for (Int icorr=0;icorr<nCorr;++icorr) {
	      Float& W(*wtp);
	      Complex& C(*cvp);
	      Complex& M(*mvp);
	      C/=Imod;
	      M/=Imod;
	      W*=Iamp2;
	      ++cvp;
	      ++mvp;
	      ++wtp;
	    } // icorr
	  } // non-zero Imod
	} // parallel hands not flagged
      }	// ichan  
    } // !flagRow
  } // irow
  
  // Set unchan'd weight, in case someone wants it
  // NB: Use of median increases cost by ~100%
  // NB: use of mean increases cost by ~50%
  //  ...but both are inaccurate if some channels flagged,
  //  and it should not be necessary to do this here
  vb.setWeight(partialMedians(vb.weightSpectrum(),IPosition(1,1),True));
  //vb.setWeight(partialMeans(vb.weightSpectrum(),IPosition(1,1)));

}




//----------------------------------------------------------------------
// void VisEquation::collapseForSim(VisBuffer& vb) {
void VisEquation::collapseForSim(VisBuffer& vb) {

  if (prtlev()>0) cout << "VE::collapseforSim()" << endl;

  // Handle origin of model data here (?):

  // Ensure correlations in canonical order
  // (this is a no-op if no sort necessary)
  // TBD: optimize in combo with model origination?
  vb.sortCorr();

  // initialize LHS/RHS indices
  Int lidx=0;
  Int ridx=napp_-1;

#ifdef RI_DEBUG
  cout << "vb.visCube    original: " << vb.visCube()(0,0,500) <<  vb.visCube()(0,0,1216) <<  vb.visCube()(0,0,1224) << endl;
  cout << "vb.model      original: " << vb.modelVisCube()(0,0,500) <<  vb.modelVisCube()(0,0,1216) <<  vb.modelVisCube()(0,0,1224) << endl;
#endif

  // copy data to model, to be corrupted in place there.
  // 20091030 RI changed skyequation to use Observed.  the below 
  // should not require scratch columns 
  vb.setModelVisCube(vb.visCube());

  // zero the data. correct will operate in place on data, so 
  // if we don't have an AMueller we don't get anything from this.  
  vb.setVisCube(0.0);
   
#ifdef RI_DEBUG
  cout << "vb.visCube before crct: " << vb.visCube()(0,0,500) <<  vb.visCube()(0,0,1216) <<  vb.visCube()(0,0,1224) << endl;
#endif

  // Correct DATA up to pivot 
  while (lidx<napp_ && vc()[lidx]->type() < pivot_) {
    if (prtlev()>2) cout << vc()[lidx]->typeName();
    if (vc()[ridx]->extraTag()!="NoiseScale" or vc()[lidx]->type()!=VisCal::T) {
      vc()[lidx]->correct(vb);
      if (prtlev()>2) cout << " -> correct";
    }
    if (prtlev()>2) cout << endl;
    lidx++;
  }

#ifdef RI_DEBUG
  cout << "vb.visCube  after crct: " << vb.visCube()(0,0,500) <<  vb.visCube()(0,0,1216) <<  vb.visCube()(0,0,1224) << endl;
  cout << "vb.model   before crpt: " << vb.modelVisCube()(0,0,500) <<  vb.modelVisCube()(0,0,1216) <<  vb.modelVisCube()(0,0,1224) << endl;
#endif

  // Corrupt Model down to (and including) the pivot
  while (ridx>-1    && vc()[ridx]->type() >= pivot_) {
    if (prtlev()>2) cout << vc()[lidx]->typeName();
    // manually pick off a T intended to be noise scaling T:
    if (pivot_ <= VisCal::T and vc()[ridx]->type()==VisCal::T) {
      if (vc()[ridx]->extraTag()=="NoiseScale") {
	vc()[ridx]->correct(vb);  // correct DATA
	if (prtlev()>2) cout << " -> correct";
      } else {
	vc()[ridx]->corrupt(vb);
	if (prtlev()>2) cout << " -> corrupt";
      }
    } else { 
      vc()[ridx]->corrupt(vb);
      if (prtlev()>2) cout << " -> corrupt";
    }
    if (prtlev()>2) cout << endl;
    ridx--;
  }
  
#ifdef RI_DEBUG
  cout << "vb.model    after crpt: " << vb.modelVisCube()(0,0,500) <<  vb.modelVisCube()(0,0,1216) <<  vb.modelVisCube()(0,0,1224) << endl;
  cout << "vb.visCube  after crpt: " << vb.visCube()(0,0,500) <<  vb.visCube()(0,0,1216) <<  vb.visCube()(0,0,1224) << endl << endl;
#endif

  // add corrected/scaled data (e.g. noise) to corrupted model
  // vb.modelVisCube()+=vb.visCube();

  // add corrupted Model to corrected/scaled data (i.e.. noise)
  vb.visCube()+=vb.modelVisCube();

  // Put correlations back in original order
  //  (~no-op if already canonical)
  vb.unSortCorr();

}

void VisEquation::state() {

  if (prtlev()>0) cout << "VE::state()" << endl;

  cout << boolalpha;

  // Order in which DATA is corrected
  cout << "Correct order:" << endl;
  if (napp_>0) {
    for (Int iapp=0;iapp<napp_;iapp++)
      cout << "  " << iapp << " " 
	   << vc()[iapp]->typeName() << " (" 
	   << vc()[iapp]->type() << ")" << endl;
  }
  else
    cout << " <none>" << endl;
  
  cout << endl;
  
  cout << "Corrupt order:" << endl;
  if (napp_>0) {
    for (Int iapp=(napp_-1);iapp>-1;iapp--)
      cout << "  " << iapp << " " 
	   << vc()[iapp]->typeName() << " (" 
	   << vc()[iapp]->type() << ")" << endl;
  }
  else
    cout << " <none>" << endl;
  
  cout << endl;

  cout << "Source model:" << endl;
  cout << "  useInternalModel_ = " << useInternalModel_ << endl;
  if (useInternalModel_)
    cout << "  Stokes = " << stokesModel_ << endl;
  else
    cout << "  <using MODEL_DATA column>" << endl;
  cout << endl;

  cout << "Collapse order:" << endl;
  cout << "  freqAveOK_ = " << freqAveOK_ << endl;
  
  if (svc_) {
    Int lidx=0;
    Int ridx=napp_-1;

    if ( freqAveOK_ && !svc().freqDepMat() ) {
      // Correct DATA up to last freqDep term on LHS
      cout << " LHS (pre-freqAve):" << endl;
      if (lidx <= lfd_) 
	while (lidx <= lfd_) {  // up to and including the lfd_th term
	  cout << "  " << lidx << " " 
	       << vc()[lidx]->typeName() << " (" 
	       << vc()[lidx]->type() << ")" 
	       << " (freqDep correct)"
	       << endl;
	  lidx++;
	}
      else
	cout << "  <none>" << endl;
      
      // Corrupt MODEL down to last freqDep term on RHS
      cout << " RHS (pre-freqAve):" << endl;
      if (ridx >= rfd_) 
	while (ridx >= rfd_) {  // down to and including the rfd_th term
	  cout << "  " << (ridx) << " " 
	       << vc()[ridx]->typeName() << " (" 
	       << vc()[ridx]->type() << ")" 
	       << " (freqDep corrupt)"
	       << endl;
	  ridx--;
	}
      else
	cout << "  <none>" << endl;
      
      cout << " Frequency average both sides" << endl;
    }
    
    cout << " LHS:" << endl;
    if (lidx<napp_ && vc()[lidx]->type() < svc().type()) 
      while (lidx<napp_ && vc()[lidx]->type() < svc().type()) {
	cout << "  " << (lidx) << " " 
	     << vc()[lidx]->typeName() << " (" 
	     << vc()[lidx]->type() << ")" 
	     << endl;
	lidx++;
      }
    else
      cout << "  <none>" << endl;
    
    cout << " RHS:" << endl;
    if (ridx>-1 && vc()[ridx]->type() >= svc().type())
      while (ridx>-1 && vc()[ridx]->type() >= svc().type()) {
	cout << "  " << (ridx) << " " 
	     << vc()[ridx]->typeName() << " (" 
	     << vc()[ridx]->type() << ")" 
	     << endl;
	ridx--;
      }
    else
      cout << "  <none>" << endl;

    
    cout << "Ready to solve for " << svc().typeName() << " (" << svc().type() << ")" << endl;

  }
  else
    cout << " <Nothing to solve for>" << endl;

}



//----------------------------------------------------------------------
// Determine residuals of VisBuffer data w.r.t. svc_
void VisEquation::residuals(VisBuffer& vb,
			    Cube<Complex>& R,
			    const Int chan) {


  if (prtlev()>3) cout << "VE::residuals()" << endl;
  if (prtlev()>13)                              // Here only to shush a
    cout << "vb.nRow(): " << vb.nRow()          // compiler warning about
         << "\nR.shape(): " << R.shape()        // unused variables.
         << "\nchan: " << chan
         << endl;

  // Trap unspecified solvable term
  if (!svc_)
    throw(AipsError("No calibration term to differentiate."));

  // TBD: support for public non-in-place corruption and
  //        corrupt for specific channel

  throw(AipsError("Simple residual calculation NYI."));

}


// Calculate residuals and differentiated residuals
void VisEquation::diffResiduals(CalVisBuffer& cvb) {

  if (prtlev()>3) cout << "VE::diffResiduals(CVB)" << endl;

  // Trap unspecified solvable term
  if (!svc_)
    throw(AipsError("No calibration term to differentiate."));

  // Delegate to the SVC
  svc().differentiate(cvb);
  
}

// Calculate residuals and differentiated residuals
void VisEquation::differentiate(SolveDataBuffer& sdb) {

  if (prtlev()>3) cout << "VE::differentiate(SDB)" << endl;

  // Trap unspecified solvable term
  if (!svc_)
    throw(AipsError("No calibration term to differentiate."));

  // Delegate to the SVC
  svc().differentiate(sdb);
  
}


// Calculate residuals and differentiated residuals
void VisEquation::diffResiduals(VisBuffer& vb, 
				Cube<Complex>& R, 
				Array<Complex>& dR,
				Matrix<Bool>& Rflg) {

  if (prtlev()>3) cout << "VE::diffResiduals()" << endl;

  // Trap unspecified solvable term
  if (!svc_)
    throw(AipsError("No calibration term to differentiate."));

  // Get trial model corrupt and differentiation from the SVC
  //   (R, dR, and Rflg will be sized by svc_)
  svc().differentiate(vb,R,dR,Rflg);

  // Now, subtract obs'd data from trial model corruption

  // Shape info
  Int nCorr(vb.corrType().nelements());
  Int& nChan(vb.nChannel());
  Int& nRow(vb.nRow());
  IPosition blc(3,0,0,0);
  IPosition trc(3,nCorr-1,nChan-1,nRow-1);

  // Slice if specific channel requested
  if (svc().freqDepPar()) 
    blc(1)=trc(1)=svc().focusChan();

  // If shapes match, subtract, else throw
  //  TBD: avoid subtraction in flagged channels?
  Cube<Complex> Vo(vb.visCube()(blc,trc));
  if (R.shape()==Vo.shape())
    R-=Vo;
  else
    throw(AipsError("Shape mismatch in residual calculation"));
  
}

void VisEquation::diffResiduals(VisBuffer& R,
                                VisBuffer& dR0,
                                VisBuffer& dR1,
                                Matrix<Bool>& Rflg)
{
  if (prtlev()>3) cout << "VE::diffResiduals()" << endl;

  // Trap unspecified solvable term
  if (!svc_)
    throw(AipsError("No calibration term to differentiate."));

  // Get trial model corrupt and differentiation from the SVC
  //   (R, dR, and Rflg will be sized by svc_)
  svc().differentiate(R,dR0,dR1, Rflg);

  // Now, subtract obs'd data from trial model corruption

  // Shape info
  Int nCorr(R.corrType().nelements());
  Int& nChan(R.nChannel());
  Int& nRow(R.nRow());
  IPosition blc(3,0,0,0);
  IPosition trc(3,nCorr-1,nChan-1,nRow-1);

  // Slice if specific channel requested
  if (svc().freqDepPar())
    blc(1)=trc(1)=svc().focusChan();
 // If shapes match, subtract, else throw
  //  TBD: avoid subtraction in flagged channels?
  //  Cube<Complex> Vo(R.visCube()(blc,trc));
//   cout << R.correctedVisCube().shape() << " "
//        << R.visCube().shape() << " "
//        << blc << " " << trc << endl;
//  Cube<Complex> Vo(R.correctedVisCube()(blc,trc));
  Cube<Complex> Vo(R.visCube()(blc,trc));
  Cube<Complex> Res;Res.reference(R.modelVisCube());
  /*
  for(Int i=0;i<Res.shape()(2);i++)
    {
      cout << i
           << " " << Res(0,0,i)
           << " " << Res(3,0,i)
           << " " << R.visCube()(0,0,i)
           << " " << R.visCube()(3,0,i)
           << " " << R.flag()(0,i)
           << " " << R.flag()(3,i)
           << " " << R.antenna1()(i)
           << " " << R.antenna2()(i)
           << " " << R.flagRow()(i)
           << endl;
    }
  exit(0);
  */
  if (Res.shape()==Vo.shape())
    Res-=Vo;
  else
    throw(AipsError("Shape mismatch in residual calculation"));
}


//----------------------------------------------------------------------
// ***************************************************************************
// ********************  Start of protected member functions *****************
// ***************************************************************************


void VisEquation::setFreqDep() {

  if (prtlev()>2) cout << "VE::setFreqDep()" << endl;

  // Nominal freq-dep is none on either side
  lfd_=-1;      // right-most freq-dep term on LHS
  rfd_=napp_;   // left-most freq-dep term on RHS

  // Nominally averaging in frequency before normalization is NOT OK
  //  (we will revise this when we can assert constant MODEL_DATA)
  freqAveOK_=false;

  // Only if there are both apply-able and solve-able terms
  if (svc_ && napp_>0) {

    // freqdep to LEFT of solvable type
    for (Int idx=0;         (idx<napp_ && vc()[idx]->type()<svc().type()); idx++)
      if (vc()[idx]->freqDepMat()) lfd_=idx;
    
    // freqdep to RIGHT of solvable type
    for (Int idx=(napp_-1); (idx>-1    && vc()[idx]->type()>=svc().type()); idx--)
      if (vc()[idx]->freqDepMat()) {
	rfd_=idx;
	// If we will corrupt the model with something freqdep, we can't
	//  frequency average in collapse
	freqAveOK_=false;
      }

  }
  

}

Bool VisEquation::ok() {

  if (napp_ != Int(vc().nelements()))
    return false;

  return(true);
}


} //# NAMESPACE CASA - END

