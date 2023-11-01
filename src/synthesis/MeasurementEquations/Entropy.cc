//# Entropy.cc:  this implements Entropy
//# Copyright (C) 1996,1997,1998,1999,2000
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
 
#include <synthesis/MeasurementEquations/CEMemModel.h>
#include <synthesis/MeasurementEquations/CEMemProgress.h>
#include <synthesis/MeasurementEquations/Entropy.h>
#include <casacore/lattices/LEL/LatticeExpr.h>
#include <casacore/lattices/LEL/LatticeExprNode.h>
#include <casacore/lattices/Lattices/LatticeStepper.h>
#include <casacore/lattices/Lattices/LatticeIterator.h>
#include <casacore/casa/BasicMath/Math.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

//----------------------------------------------------------------------
Entropy::Entropy()
{
  cemem_ptr = 0;
};

//----------------------------------------------------------------------
Entropy::~Entropy()
{
  // cemem_ptr->letEntropyDie();
};






//----------------------------------------------------------------------
EntropyI::EntropyI()
{
};

//----------------------------------------------------------------------
EntropyI::~EntropyI()
{
};

//----------------------------------------------------------------------

Float EntropyI::formEntropy()
{
  Lattice<Float> &model = *(cemem_ptr->itsModel_ptr);
  Float flux = 0.0;
  if (cemem_ptr->itsMask_ptr) {
    Lattice<Float> &mask = *(cemem_ptr->itsMask_ptr);
    flux = sum((mask * model)).getFloat();
  } else {    
    flux = sum(model).getFloat();
  }

  cemem_ptr->itsTotalFlux = flux + cemem_ptr->itsCycleFlux;

  cemem_ptr->itsFlux = flux;
  Float defLev = cemem_ptr->itsDefaultLevel;
  LatticeExprNode myEntropyLEN;
  Float myEntropy = 0;

  if (cemem_ptr->itsMask_ptr) {
    
    Lattice<Float> &model = *(cemem_ptr->itsModel_ptr);
    Lattice<Float> &mask = *(cemem_ptr->itsMask_ptr);

    RO_LatticeIterator<Float> 
      mod( model, LatticeStepper(model.shape(), model.niceCursorShape()));
    RO_LatticeIterator<Float> 
      mas( mask, LatticeStepper(model.shape(), model.niceCursorShape()));
    
    if (cemem_ptr->itsPrior_ptr) {
      Lattice<Float> &prior = *(cemem_ptr->itsPrior_ptr);
      RO_LatticeIterator<Float> 
	pri( prior, LatticeStepper(model.shape(), model.niceCursorShape()));
      uInt i;
      Bool modDeleteIt;
      Bool masDeleteIt;
      Bool priDeleteIt;
      
      for (mod.reset(), mas.reset(), pri.reset(); !mod.atEnd(); mod++, mas++, pri++) {
	const Float *modStore = mod.cursor().getStorage(modDeleteIt);
	const Float *masStore = mas.cursor().getStorage(masDeleteIt);
	const Float *priStore = pri.cursor().getStorage(priDeleteIt);
	const Float *modIter = modStore;
	const Float *masIter = masStore;
	const Float *priIter = priStore;
	for (i=0;i<mod.cursor().nelements();i++,modIter++, masIter++, priIter++) {
	  if (*masIter > 0.0) {
	    myEntropy -=  ( *modIter * log( *modIter / *priIter ) );
	  }
	}
	mod.cursor().freeStorage(modStore, modDeleteIt);
	mas.cursor().freeStorage(masStore, masDeleteIt);
	pri.cursor().freeStorage(priStore, priDeleteIt);
      }
    } else {
      
      uInt i;
      Bool modDeleteIt;
      Bool masDeleteIt;
      for (mod.reset(), mas.reset(); !mod.atEnd(); mod++, mas++) {
	const Float *modStore = mod.cursor().getStorage(modDeleteIt);
	const Float *masStore = mas.cursor().getStorage(masDeleteIt);
	const Float *modIter = modStore;
	const Float *masIter = masStore;
	for (i=0;i<mod.cursor().nelements();i++,modIter++, masIter++) {
	  if (*masIter > 0.0) {
	    myEntropy -=  ( *modIter * log( *modIter ) );
	  }
	}
	mod.cursor().freeStorage(modStore, modDeleteIt);
	mas.cursor().freeStorage(masStore, masDeleteIt);
      }
      myEntropy +=  flux * log(defLev);
    }    
    if (flux > 0.0) {
      myEntropy = myEntropy/flux + 
	log(cemem_ptr->itsNumberPixels);
    } else {
      myEntropy = 0.0;
    }
    
  } else { // No Mask;  Oh for the joy of LEL!
    
    if (cemem_ptr->itsPrior_ptr) {
      Lattice<Float> &prior = *(cemem_ptr->itsPrior_ptr);
      myEntropyLEN = - sum(model * log(model/prior)) ;
      myEntropy = myEntropyLEN.getFloat();
    } else {
      myEntropyLEN = - sum(model * log(model)) ;
      myEntropy = myEntropyLEN.getFloat() + flux * log(defLev);
    }
    if (flux > 0.0) {
      myEntropy = myEntropy/flux + 
	log(cemem_ptr->itsNumberPixels);
    } else {
      myEntropy = 0.0;
    }
  }

  return myEntropy;
};



//----------------------------------------------------------------------
void EntropyI::formGDG(Matrix<Double>& GDG)
{
  if (cemem_ptr->itsMask_ptr) {
    Lattice<Float> &model = *(cemem_ptr->itsModel_ptr);
    Lattice<Float> &resid = *(cemem_ptr->itsResidual_ptr);
    Lattice<Float> &mask = *(cemem_ptr->itsMask_ptr);
    
    GDG.resize(4,4);
    GDG.set(0.0);
    Float alpha = cemem_ptr->itsAlpha;
    Float beta  = cemem_ptr->itsBeta;
    Float defLev = cemem_ptr->itsDefaultLevel;
    
    // should not use Lattice Expression Language for efficiency
    // using it right now to get up and running
    Float ggc = 2 * alpha * cemem_ptr->itsQ;
    Float rHess;
    
    RO_LatticeIterator<Float> 
      mod( model, LatticeStepper(model.shape(), model.niceCursorShape()));
    RO_LatticeIterator<Float> 
      res( resid, LatticeStepper(model.shape(), model.niceCursorShape()));
    RO_LatticeIterator<Float> 
      mas( mask, LatticeStepper(model.shape(), model.niceCursorShape()));
    uInt i;
    Bool modDeleteIt;
    Bool resDeleteIt;
    Bool masDeleteIt; 
    Double GDGHH = 0.0;
    Double GDGHC = 0.0;
    Double GDGHF = 0.0;
    Double GDGCC = 0.0;
    Double GDGCF = 0.0;
    Double GDGFF = 0.0;
    Double gradH = 0.0;
    Double gradC = 0.0;
    if (cemem_ptr->itsPrior_ptr ==  0) {
      Float logDef = log(defLev);
      for (mod.reset(), res.reset(), mas.reset(); !mod.atEnd(); mod++, res++) {
	const Float *modStore = mod.cursor().getStorage(modDeleteIt);
	const Float *resStore = res.cursor().getStorage(resDeleteIt);
	const Float *masStore = mas.cursor().getStorage(masDeleteIt);
	const Float *modIter = modStore;
	const Float *resIter = resStore;
	const Float *masIter = masStore;
	for (i=0;i<mod.cursor().nelements();i++,modIter++, resIter++, masIter++) {
	  if (*masIter > 0.0) {
	    rHess = (*modIter)/(1 + ggc * (*modIter));
	    gradH = -log(*modIter) + logDef;
	    gradC = -2*(*resIter);
	    GDGHH = GDGHH + gradH * rHess * gradH;
	    GDGHC = GDGHC + gradH * rHess * gradC;
	    GDGHF = GDGHF + gradH * rHess;
	    GDGCC = GDGCC + gradC * rHess * gradC;
	    GDGCF = GDGCF + gradC * rHess;
	    GDGFF = GDGFF + rHess;
	  }
	}
	mod.cursor().freeStorage(modStore, modDeleteIt);
	res.cursor().freeStorage(resStore, resDeleteIt);
	mas.cursor().freeStorage(masStore, masDeleteIt);
      }
    } else {
      Lattice<Float> &prior = *(cemem_ptr->itsPrior_ptr);
      RO_LatticeIterator<Float> 
	pri( prior, LatticeStepper(prior.shape(), model.niceCursorShape()));    
      Bool priDeleteIt;
      for (mod.reset(), res.reset(), mas.reset(), pri.reset(); !mod.atEnd(); 
	   mod++, res++, mas++, pri++) {
	const Float *modStore = mod.cursor().getStorage(modDeleteIt);
	const Float *resStore = res.cursor().getStorage(resDeleteIt);
	const Float *masStore = mas.cursor().getStorage(masDeleteIt);
	const Float *priStore = pri.cursor().getStorage(priDeleteIt);
	const Float *modIter = modStore;
	const Float *resIter = resStore;
	const Float *masIter = masStore;
	const Float *priIter = priStore;
	for (i=0;i<mod.cursor().nelements();i++,modIter++, resIter++, masIter++, priIter++) {
	  if (*masIter > 0.0) {
	    rHess = (*modIter)/(1 + ggc* (*modIter));
	    gradH = -log( (*modIter) / (*priIter));
	    gradC = -2*(*resIter);
	    GDGHH = GDGHH + gradH * rHess * gradH;
	    GDGHC = GDGHC + gradH * rHess * gradC;
	    GDGHF = GDGHF + gradH * rHess;
	    GDGCC = GDGCC + gradC * rHess * gradC;
	    GDGCF = GDGCF + gradC * rHess;
	    GDGFF = GDGFF + rHess;
	  }
	}
	mod.cursor().freeStorage(modStore, modDeleteIt);
	res.cursor().freeStorage(resStore, resDeleteIt);
	mas.cursor().freeStorage(masStore, masDeleteIt);
 	pri.cursor().freeStorage(priStore, priDeleteIt);
      }
    }
    GDG(H,H) = GDGHH;
    GDG(H,C) = GDGHC;
    GDG(H,F) = GDGHF;
    GDG(C,C) = GDGCC;
    GDG(C,F) = GDGCF;
    GDG(F,F) = GDGFF;
    GDG(H,J) = GDGHH -  alpha * GDGHC - beta * GDGHF;
    GDG(C,J) = GDGHC -  alpha * GDGCC - beta * GDGCF;
    GDG(F,J) = GDGHF -  alpha * GDGCF - beta * GDGFF;
    GDG(J,J) = GDGHH +  square(alpha) * GDGCC 
      + square(beta)*GDGFF  + 2*alpha*beta*GDGCF  
      - 2*alpha*GDGHC - 2*beta*GDGHF;

  } else { // no mask

    Lattice<Float> &model = *(cemem_ptr->itsModel_ptr);
    Lattice<Float> &resid = *(cemem_ptr->itsResidual_ptr);    
    GDG.resize(4,4);
    GDG.set(0.0);
    Float alpha = cemem_ptr->itsAlpha;
    Float beta  = cemem_ptr->itsBeta;
    Float defLev = cemem_ptr->itsDefaultLevel;
    
    // should not use Lattice Expression Language for efficiency
    // using it right now to get up and running
    Float ggc = 2 * alpha * cemem_ptr->itsQ;
    Float rHess;
    
    RO_LatticeIterator<Float> 
      mod( model, LatticeStepper(model.shape(), model.niceCursorShape()));
    RO_LatticeIterator<Float> 
      res( resid, LatticeStepper(model.shape(), model.niceCursorShape()));
    uInt i;
    Bool modDeleteIt;
    Bool resDeleteIt;
    Double GDGHH = 0.0;
    Double GDGHC = 0.0;
    Double GDGHF = 0.0;
    Double GDGCC = 0.0;
    Double GDGCF = 0.0;
    Double GDGFF = 0.0;
    Double gradH = 0.0;
    Double gradC = 0.0;
    if (cemem_ptr->itsPrior_ptr ==  0) {
      Float logDef = log(defLev);
      for (mod.reset(), res.reset(); !mod.atEnd(); mod++, res++) {
	const Float *modStore = mod.cursor().getStorage(modDeleteIt);
	const Float *resStore = res.cursor().getStorage(resDeleteIt);
	const Float *modIter = modStore;
	const Float *resIter = resStore;
	for (i=0;i<mod.cursor().nelements();i++,modIter++, resIter++) {
	  rHess = (*modIter)/(1 + ggc * (*modIter));
	  gradH = -log(*modIter) + logDef;
	  gradC = -2*(*resIter);
	  GDGHH = GDGHH + gradH * rHess * gradH;
	  GDGHC = GDGHC + gradH * rHess * gradC;
	  GDGHF = GDGHF + gradH * rHess;
	  GDGCC = GDGCC + gradC * rHess * gradC;
	  GDGCF = GDGCF + gradC * rHess;
	  GDGFF = GDGFF + rHess;
	}
	mod.cursor().freeStorage(modStore, modDeleteIt);
	res.cursor().freeStorage(resStore, resDeleteIt);
      }
    } else {
      Lattice<Float> &prior = *(cemem_ptr->itsPrior_ptr);
      RO_LatticeIterator<Float> 
	pri( prior, LatticeStepper(prior.shape(), model.niceCursorShape()));    
      Bool priDeleteIt;
      for (mod.reset(), res.reset(), pri.reset(); !mod.atEnd(); mod++, res++, pri++) {
	const Float *modStore = mod.cursor().getStorage(modDeleteIt);
	const Float *resStore = res.cursor().getStorage(resDeleteIt);
	const Float *priStore = pri.cursor().getStorage(priDeleteIt);
	const Float *modIter = modStore;
	const Float *resIter = resStore;
	const Float *priIter = priStore;
	for (i=0;i<mod.cursor().nelements();i++,modIter++, resIter++, priIter++) {
	  rHess = (*modIter)/(1 + ggc* (*modIter));
	  gradH = -log( (*modIter) / (*priIter));
	  gradC = -2*(*resIter);
	  GDGHH = GDGHH + gradH * rHess * gradH;
	  GDGHC = GDGHC + gradH * rHess * gradC;
	  GDGHF = GDGHF + gradH * rHess;
	  GDGCC = GDGCC + gradC * rHess * gradC;
	  GDGCF = GDGCF + gradC * rHess;
	  GDGFF = GDGFF + rHess;
	}
	mod.cursor().freeStorage(modStore, modDeleteIt);
	res.cursor().freeStorage(resStore, resDeleteIt);
	pri.cursor().freeStorage(priStore, priDeleteIt);
      }
    }
    GDG(H,H) = GDGHH;
    GDG(H,C) = GDGHC;
    GDG(H,F) = GDGHF;
    GDG(C,C) = GDGCC;
    GDG(C,F) = GDGCF;
    GDG(F,F) = GDGFF;
    GDG(H,J) = GDGHH -  alpha * GDGHC - beta * GDGHF;
    GDG(C,J) = GDGHC -  alpha * GDGCC - beta * GDGCF;
    GDG(F,J) = GDGHF -  alpha * GDGCF - beta * GDGFF;
    GDG(J,J) = GDGHH +  square(alpha) * GDGCC 
      + square(beta)*GDGFF  + 2*alpha*beta*GDGCF  
      - 2*alpha*GDGHC - 2*beta*GDGHF;
  }
};

//----------------------------------------------------------------------
void EntropyI::formGDGStep(Matrix<Double>& GDG)
{

  if (cemem_ptr->itsMask_ptr) {

    Lattice<Float> &model = *(cemem_ptr->itsModel_ptr);
    Lattice<Float> &resid = *(cemem_ptr->itsResidual_ptr);
    Lattice<Float> &step  = *(cemem_ptr->itsStep_ptr);
    Lattice<Float> &mask = *(cemem_ptr->itsMask_ptr);

    GDG.resize(4,4);
    GDG.set(0.0);
    Float alpha = cemem_ptr->itsAlpha;
    Float beta  = cemem_ptr->itsBeta;
    Float defLev = cemem_ptr->itsDefaultLevel;
    
    // should not use Lattice Expression Language for efficiency
    // using it right now to get up and running
    Float ggc = 2 * alpha * cemem_ptr->itsQ;
    Float rHess;
    
    RO_LatticeIterator<Float> 
      mod( model, LatticeStepper(model.shape(), model.niceCursorShape()));
    RO_LatticeIterator<Float> 
      res( resid, LatticeStepper(resid.shape(), model.niceCursorShape()));
    RO_LatticeIterator<Float> 
      mas( mask, LatticeStepper(model.shape(), model.niceCursorShape()));
    LatticeIterator<Float> 
      stp( step, LatticeStepper(step.shape(), model.niceCursorShape()));
    uInt i;
    Bool modDeleteIt;
    Bool resDeleteIt;
    Bool stpDeleteIt;
    Bool masDeleteIt;
    Double GDGHH = 0.0;
    Double GDGHC = 0.0;
    Double GDGHF = 0.0;
    Double GDGCC = 0.0;
    Double GDGCF = 0.0;
    Double GDGFF = 0.0;
    Double gradH = 0.0;
    Double gradC = 0.0;
    Double gradJ = 0.0;
    Float stepValue;
    if ( cemem_ptr->itsPrior_ptr == 0) {
      Float logDef = log(defLev);
      for (mod.reset(), res.reset(), mas.reset(), stp.reset(); 
	   !mod.atEnd(); mod++, res++, stp++, mas++) {
	const Float *modStore = mod.cursor().getStorage(modDeleteIt);
	const Float *resStore = res.cursor().getStorage(resDeleteIt);
	const Float *masStore = mas.cursor().getStorage(masDeleteIt);
	Float *stpStore = stp.rwCursor().getStorage(stpDeleteIt);
	const Float *modIter = modStore;
	const Float *resIter = resStore;
	const Float *masIter = masStore;
	Float *stpIter = stpStore;
	for (i=0;i<mod.cursor().nelements();i++,modIter++, resIter++, stpIter++, masIter++) {
	  if (*masIter > 0.0) {
	    rHess = (*modIter)/(1 + ggc * (*modIter));
	    gradH = -log(*modIter) + logDef;
	    gradC = -2*(*resIter);
	    gradJ = gradH - alpha*gradC - beta;
	    stepValue = rHess * gradJ;
	    (*stpIter) = stepValue;
	    GDGHH = GDGHH + gradH * rHess * gradH;
	    GDGHC = GDGHC + gradH * rHess * gradC;
	    GDGHF = GDGHF + gradH * rHess;
	    GDGCC = GDGCC + gradC * rHess * gradC;
	    GDGCF = GDGCF + gradC * rHess;
	    GDGFF = GDGFF + rHess;
	  }
	}
	mod.cursor().freeStorage(modStore, modDeleteIt);
	res.cursor().freeStorage(resStore, resDeleteIt);
	mas.cursor().freeStorage(masStore, masDeleteIt);
	stp.rwCursor().putStorage(stpStore, stpDeleteIt);
       }
    } else {
      Lattice<Float> &prior = *(cemem_ptr->itsPrior_ptr);
      RO_LatticeIterator<Float> 
	pri( prior, LatticeStepper(prior.shape(), model.niceCursorShape()));    
      Bool priDeleteIt;
      for (mod.reset(), res.reset(), pri.reset(), mas.reset(), stp.reset(); 
	   !mod.atEnd(); mod++, res++, pri++, mas++, stp++) {
	const Float *modStore = mod.cursor().getStorage(modDeleteIt);
	const Float *resStore = res.cursor().getStorage(resDeleteIt);
	const Float *priStore = pri.cursor().getStorage(priDeleteIt);
	const Float *masStore = mas.cursor().getStorage(masDeleteIt);
	Float *stpStore = stp.rwCursor().getStorage(stpDeleteIt);
	const Float *modIter = modStore;
	const Float *resIter = resStore;
	const Float *priIter = priStore;
	const Float *masIter = masStore;
	Float *stpIter = stpStore;
	for (i=0;i<mod.cursor().nelements();
	     i++,modIter++, resIter++, priIter++, stpIter++, masIter++) {
	  if (*masIter > 0.0) {
	    rHess = (*modIter)/(1 + ggc* (*modIter));
	    gradH = -log( (*modIter) / (*priIter));
	    gradC = -2*(*resIter);
	    gradJ = gradH - alpha*gradC - beta;
	    stepValue = rHess * gradJ;
	    (*stpIter) = stepValue;
	    GDGHH = GDGHH + gradH * rHess * gradH;
	    GDGHC = GDGHC + gradH * rHess * gradC;
	    GDGHF = GDGHF + gradH * rHess;
	    GDGCC = GDGCC + gradC * rHess * gradC;
	    GDGCF = GDGCF + gradC * rHess;
	    GDGFF = GDGFF + rHess;
	  }
	}
	mod.cursor().freeStorage(modStore, modDeleteIt);
	res.cursor().freeStorage(resStore, resDeleteIt);
	pri.cursor().freeStorage(priStore, priDeleteIt);
	mas.cursor().freeStorage(masStore, masDeleteIt);
	stp.rwCursor().putStorage(stpStore, stpDeleteIt);
      }
    }
    GDG(H,H) = GDGHH;
    GDG(H,C) = GDGHC;
    GDG(H,F) = GDGHF;
    GDG(C,C) = GDGCC;
    GDG(C,F) = GDGCF;
    GDG(F,F) = GDGFF;
    GDG(H,J) = GDGHH -  alpha * GDGHC - beta * GDGHF;
    GDG(C,J) = GDGHC -  alpha * GDGCC - beta * GDGCF;
    GDG(F,J) = GDGHF -  alpha * GDGCF - beta * GDGFF;
    GDG(J,J) = GDGHH +  square(alpha) * GDGCC 
      + square(beta)*GDGFF  + 2*alpha*beta*GDGCF  
      - 2*alpha*GDGHC - 2*beta*GDGHF;


  } else { // no mask


    Lattice<Float> &model = *(cemem_ptr->itsModel_ptr);
    Lattice<Float> &resid = *(cemem_ptr->itsResidual_ptr);
    Lattice<Float> &step  = *(cemem_ptr->itsStep_ptr);
    
    GDG.resize(4,4);
    GDG.set(0.0);
    Float alpha = cemem_ptr->itsAlpha;
    Float beta  = cemem_ptr->itsBeta;
    Float defLev = cemem_ptr->itsDefaultLevel;
    
    Float ggc = 2 * alpha * cemem_ptr->itsQ;
    Float rHess;
    
    RO_LatticeIterator<Float> 
      mod( model, LatticeStepper(model.shape(), model.niceCursorShape()));
    RO_LatticeIterator<Float> 
      res( resid, LatticeStepper(resid.shape(), model.niceCursorShape()));
    LatticeIterator<Float> 
      stp( step, LatticeStepper(step.shape(), model.niceCursorShape()));
    uInt i;
    Bool modDeleteIt;
    Bool resDeleteIt;
    Bool stpDeleteIt;
    Double GDGHH = 0.0;
    Double GDGHC = 0.0;
    Double GDGHF = 0.0;
    Double GDGCC = 0.0;
    Double GDGCF = 0.0;
    Double GDGFF = 0.0;
    Double gradH = 0.0;
    Double gradC = 0.0;
    Double gradJ = 0.0;
    Float stepValue;
    if ( cemem_ptr->itsPrior_ptr == 0) {
      Float logDef = log(defLev);
      for (mod.reset(), res.reset(), stp.reset(); 
	   !mod.atEnd(); mod++, res++, stp++) {
	const Float *modStore = mod.cursor().getStorage(modDeleteIt);
	const Float *resStore = res.cursor().getStorage(resDeleteIt);
	Float *stpStore = stp.rwCursor().getStorage(stpDeleteIt);
	const Float *modIter = modStore;
	const Float *resIter = resStore;
	Float *stpIter = stpStore;
	for (i=0;i<mod.cursor().nelements();i++,modIter++, resIter++, stpIter++) {
	  rHess = (*modIter)/(1 + ggc * (*modIter));
	  gradH = -log(*modIter) + logDef;
	  gradC = -2*(*resIter);
	  gradJ = gradH - alpha*gradC - beta;
	  stepValue = rHess * gradJ;
	  (*stpIter) = stepValue;
	  GDGHH = GDGHH + gradH * rHess * gradH;
	  GDGHC = GDGHC + gradH * rHess * gradC;
	  GDGHF = GDGHF + gradH * rHess;
	  GDGCC = GDGCC + gradC * rHess * gradC;
	  GDGCF = GDGCF + gradC * rHess;
	  GDGFF = GDGFF + rHess;
	}
	mod.cursor().freeStorage(modStore, modDeleteIt);
	res.cursor().freeStorage(resStore, resDeleteIt);
	stp.rwCursor().putStorage(stpStore, stpDeleteIt);
      }
    } else {
      Lattice<Float> &prior = *(cemem_ptr->itsPrior_ptr);
      RO_LatticeIterator<Float> 
	pri( prior, LatticeStepper(prior.shape(), model.niceCursorShape()));    
      Bool priDeleteIt;
      for (mod.reset(), res.reset(), pri.reset(), stp.reset(); 
	   !mod.atEnd(); mod++, res++, pri++, stp++) {
	const Float *modStore = mod.cursor().getStorage(modDeleteIt);
	const Float *resStore = res.cursor().getStorage(resDeleteIt);
	const Float *priStore = pri.cursor().getStorage(priDeleteIt);
	Float *stpStore = stp.rwCursor().getStorage(stpDeleteIt);
	const Float *modIter = modStore;
	const Float *resIter = resStore;
	const Float *priIter = priStore;
	Float *stpIter = stpStore;
	for (i=0;i<mod.cursor().nelements();
	     i++,modIter++, resIter++, priIter++, stpIter++) {
	  rHess = (*modIter)/(1 + ggc* (*modIter));
	  gradH = -log( (*modIter) / (*priIter));
	  gradC = -2*(*resIter);
	  gradJ = gradH - alpha*gradC - beta;
	  stepValue = rHess * gradJ;
	  (*stpIter) = stepValue;
	  GDGHH = GDGHH + gradH * rHess * gradH;
	  GDGHC = GDGHC + gradH * rHess * gradC;
	  GDGHF = GDGHF + gradH * rHess;
	  GDGCC = GDGCC + gradC * rHess * gradC;
	  GDGCF = GDGCF + gradC * rHess;
	  GDGFF = GDGFF + rHess;
	}
	mod.cursor().freeStorage(modStore, modDeleteIt);
	res.cursor().freeStorage(resStore, resDeleteIt);
	pri.cursor().freeStorage(priStore, priDeleteIt);
	stp.rwCursor().putStorage(stpStore, stpDeleteIt);
      }
    }
    GDG(H,H) = GDGHH;
    GDG(H,C) = GDGHC;
    GDG(H,F) = GDGHF;
    GDG(C,C) = GDGCC;
    GDG(C,F) = GDGCF;
    GDG(F,F) = GDGFF;
    GDG(H,J) = GDGHH -  alpha * GDGHC - beta * GDGHF;
    GDG(C,J) = GDGHC -  alpha * GDGCC - beta * GDGCF;
    GDG(F,J) = GDGHF -  alpha * GDGCF - beta * GDGFF;
    GDG(J,J) = GDGHH +  square(alpha) * GDGCC 
      + square(beta)*GDGFF  + 2*alpha*beta*GDGCF  
      - 2*alpha*GDGHC - 2*beta*GDGHF;

  }

};

//----------------------------------------------------------------------
Double EntropyI::formGDS()
{
  Lattice<Float> &model = *(cemem_ptr->itsModel_ptr);
  Lattice<Float> &step  = *(cemem_ptr->itsStep_ptr);
  Lattice<Float> &resid = *(cemem_ptr->itsResidual_ptr);

  Float alpha = cemem_ptr->itsAlpha;
  Float beta = cemem_ptr->itsBeta;
  Float defLev = cemem_ptr->itsDefaultLevel;

  Double gds = 0;

  if (cemem_ptr->itsMask_ptr) { // Do Mask
    Lattice<Float> &mask = *(cemem_ptr->itsMask_ptr);

    RO_LatticeIterator<Float> 
      mod( model, LatticeStepper(model.shape(), model.niceCursorShape()));
    LatticeIterator<Float> 
      stp( step, LatticeStepper(step.shape(), model.niceCursorShape()));
    RO_LatticeIterator<Float> 
      res( resid, LatticeStepper(resid.shape(), model.niceCursorShape()));
    RO_LatticeIterator<Float> 
      mas( mask, LatticeStepper(model.shape(), model.niceCursorShape()));
    Bool modDeleteIt;
    Bool masDeleteIt;
    Bool stpDeleteIt;
    Bool resDeleteIt;
    Bool priDeleteIt;

    if (cemem_ptr->itsPrior_ptr) { // mask AND prior

      Lattice<Float> &prior = *(cemem_ptr->itsPrior_ptr);
      RO_LatticeIterator<Float> 
	pri( prior, LatticeStepper(model.shape(), model.niceCursorShape()));


      for (mod.reset(), mas.reset(), pri.reset(), res.reset(), stp.reset();
	   !mod.atEnd(); mod++, mas++, pri++, stp++, res++) {
	const Float *modStore = mod.cursor().getStorage(modDeleteIt);
	const Float *masStore = mas.cursor().getStorage(masDeleteIt);
	const Float *priStore = pri.cursor().getStorage(priDeleteIt);
	const Float *stpStore = stp.cursor().getStorage(stpDeleteIt);
	const Float *resStore = res.cursor().getStorage(resDeleteIt);
	const Float *modIter = modStore;
	const Float *masIter = masStore;
	const Float *priIter = priStore;
	const Float *stpIter = stpStore;
	const Float *resIter = resStore;
	for (uInt i=0;i<mod.cursor().nelements();
	     i++,modIter++, masIter++, priIter++, stpIter++,resIter++ ) {
	  if (*masIter > 0.0) {
	    gds += (*stpIter) * (-log( (*modIter)/(*priIter) ) + 2.0*alpha * (*resIter) - beta);
	  }
	}
	mod.cursor().freeStorage(modStore, modDeleteIt);
	mas.cursor().freeStorage(masStore, masDeleteIt);
	res.cursor().freeStorage(resStore, resDeleteIt);
	stp.cursor().freeStorage(stpStore, stpDeleteIt);
	pri.cursor().freeStorage(priStore, priDeleteIt);
      }

    }  else {  // Mask, but no prior
      
      Float logDefLev = log(defLev);
      for (mod.reset(), mas.reset(), res.reset(), stp.reset();
	   !mod.atEnd(); mod++, mas++, stp++, res++) {
	const Float *modStore = mod.cursor().getStorage(modDeleteIt);
	const Float *masStore = mas.cursor().getStorage(masDeleteIt);
	const Float *stpStore = stp.cursor().getStorage(stpDeleteIt);
	const Float *resStore = res.cursor().getStorage(resDeleteIt);
	const Float *modIter = modStore;
	const Float *masIter = masStore;
	const Float *stpIter = stpStore;
	const Float *resIter = resStore;
	for (uInt i=0;i<mod.cursor().nelements();i++,modIter++, masIter++, stpIter++, resIter++) {
	  if (*masIter > 0.0) {
	    gds += (*stpIter) * (-log( *modIter ) +logDefLev + 2.0*alpha * (*resIter) - beta);
	  }
	}
	mod.cursor().freeStorage(modStore, modDeleteIt);
	mas.cursor().freeStorage(masStore, masDeleteIt);
	res.cursor().freeStorage(resStore, resDeleteIt);
	stp.cursor().freeStorage(stpStore, stpDeleteIt);
      }
    }

  }  else {  // No Mask

    if (cemem_ptr->itsPrior_ptr == 0) {
      Float logDefLev = log(defLev);
      gds = sum( step *
		 ( -log(model)+logDefLev+2.0*alpha * resid -beta)).getDouble();
    } else {
      Lattice<Float> &prior = *(cemem_ptr->itsPrior_ptr);
      gds = sum( step *
		 ( -log(model/prior) + 2.0*alpha * resid - beta)).getDouble();
    }
  }
  
  return gds;

};


//----------------------------------------------------------------------
void EntropyI::infoBanner(){
  cemem_ptr->itsLog << 
    " I  Entropy    Flux     Fit    Gradient "  << LogIO::POST;
};

//----------------------------------------------------------------------
void EntropyI::infoPerIteration(uInt iteration){
  cemem_ptr->itsLog << 
    iteration << "  " <<
    cemem_ptr->itsEntropy << "  " <<
    cemem_ptr->itsTotalFlux << "  " <<
    cemem_ptr->itsFit << "  " <<
    cemem_ptr->itsNormGrad << "  " <<
    LogIO::POST;

  if (cemem_ptr->itsProgressPtr) {
    Lattice<Float> &resid = *(cemem_ptr->itsResidual_ptr);
    Lattice<Float> &model = *(cemem_ptr->itsModel_ptr);
    Float maxResid = (max(abs(resid))).getFloat();
    // Note:  posMaximumResidual is not valid! (  IPosition(4,0,0,0,0)  )
    cemem_ptr->itsProgressPtr->
      info(false, iteration, cemem_ptr->itsNumberIterations, model, resid, 
	   maxResid, IPosition(4,0,0,0,0),
	   cemem_ptr->itsTotalFlux, cemem_ptr->itsFit, cemem_ptr->itsNormGrad,
	   cemem_ptr->itsEntropy);
  }
};


//----------------------------------------------------------------------
Float EntropyI::relaxMin()
{
  Float requiredModelMin = 0.1 * cemem_ptr->itsModelMin + 1.0e-8;
  return requiredModelMin;
};


//----------------------------------------------------------------------
Bool EntropyI::testConvergence()
{
  Bool converged;
  converged = ( (cemem_ptr->itsFit < (1.0+(cemem_ptr->itsTolerance))) &&
		(cemem_ptr->itsIteration != (cemem_ptr->itsFirstIteration)) &&
		(cemem_ptr->itsNormGrad < (cemem_ptr->itsTolerance)) );
  if (cemem_ptr->itsUseFluxConstraint) {
    converged = (converged && 
		 (abs(cemem_ptr->itsFlux - cemem_ptr->itsTargetFlux) 
		    < cemem_ptr->itsTolerance*cemem_ptr->itsTargetFlux));
  }

  return converged;

};





//----------------------------------------------------------------------
EntropyEmptiness::EntropyEmptiness()
{
};

//----------------------------------------------------------------------
EntropyEmptiness::~EntropyEmptiness()
{
};

//----------------------------------------------------------------------

Float EntropyEmptiness::formEntropy()
{
  Lattice<Float> &model = *(cemem_ptr->itsModel_ptr);
  Float flux = 0.0;
  flux = sum(model).getFloat();
  cemem_ptr->itsFlux = flux;
  cemem_ptr->itsTotalFlux = flux + cemem_ptr->itsCycleFlux;
  LatticeExprNode myEntropyLEN;
  Float aFit = cemem_ptr->itsAFit;
  Float myEntropy = 0;

  if (cemem_ptr->itsMask_ptr) {
    Lattice<Float> &mask  = *(cemem_ptr->itsMask_ptr);

    if (cemem_ptr->itsPrior_ptr == 0 ) {
      Float defLev = cemem_ptr->itsDefaultLevel;
      myEntropyLEN = - sum( mask * (log(cosh((model - defLev)/aFit))) ) ;
      myEntropy = -aFit * myEntropyLEN.getFloat();
    } else {
      Lattice<Float> &prior = *(cemem_ptr->itsPrior_ptr);
      myEntropyLEN = - sum( mask * (log(cosh((model - prior)/aFit))) ) ;
      myEntropy = -aFit * myEntropyLEN.getFloat();
    }

  } else {  // No Mask

    if (cemem_ptr->itsPrior_ptr == 0 ) {
      Float defLev = cemem_ptr->itsDefaultLevel;
      myEntropyLEN = - sum( log(cosh((model - defLev)/aFit)) ) ;
      myEntropy = -aFit * myEntropyLEN.getFloat();
    } else {
      Lattice<Float> &prior = *(cemem_ptr->itsPrior_ptr);
      myEntropyLEN = - sum( log(cosh((model - prior)/aFit)) ) ;
      myEntropy = -aFit * myEntropyLEN.getFloat();
    }
  }
  return myEntropy;
};

//----------------------------------------------------------------------
void EntropyEmptiness::formGDG(Matrix<Double>& GDG)
{
  Lattice<Float> &model = *(cemem_ptr->itsModel_ptr);
  Lattice<Float> &resid = *(cemem_ptr->itsResidual_ptr);

  GDG.resize(4,4);
  GDG.set(0.0);
  Float aFit = cemem_ptr->itsAFit;
  Float alpha = cemem_ptr->itsAlpha;
  Float beta  = cemem_ptr->itsBeta;

  Float ggc = 2 * alpha * cemem_ptr->itsQ;
  Float rHess;

  RO_LatticeIterator<Float> 
    mod( model, LatticeStepper(model.shape(), model.niceCursorShape()));
  RO_LatticeIterator<Float> 
    res( resid, LatticeStepper(model.shape(), model.niceCursorShape()));
  uInt i;
  Bool modDeleteIt;
  Bool resDeleteIt;
  Bool masDeleteIt;
  Double GDGHH = 0.0;
  Double GDGHC = 0.0;
  Double GDGHF = 0.0;
  Double GDGCC = 0.0;
  Double GDGCF = 0.0;
  Double GDGFF = 0.0;
  Double gradH = 0.0;
  Double gradC = 0.0;

  if (cemem_ptr->itsMask_ptr) { // Do MASK

    Lattice<Float> &mask = *(cemem_ptr->itsMask_ptr);
    RO_LatticeIterator<Float> 
      mas( mask, LatticeStepper(model.shape(), model.niceCursorShape()));

    if (cemem_ptr->itsPrior_ptr ==  0) {
      Float defLev = cemem_ptr->itsDefaultLevel;
      for (mod.reset(), res.reset(); !mod.atEnd(); mod++, res++) {
	const Float *modStore = mod.cursor().getStorage(modDeleteIt);
	const Float *resStore = res.cursor().getStorage(resDeleteIt);
	const Float *masStore = mas.cursor().getStorage(masDeleteIt);
	const Float *modIter = modStore;
	const Float *resIter = resStore;
	const Float *masIter = masStore;
	for (i=0;i<mod.cursor().nelements();i++,modIter++, resIter++, masIter++) {
	  if (*masIter > 0.0) {
	    gradH = -tanh( (*modIter - defLev)/aFit );
	    rHess = 1.0/( square(1.0-gradH) /aFit + ggc) ;
	    gradC = -2*(*resIter);
	    GDGHH = GDGHH + gradH * rHess * gradH;
	    GDGHC = GDGHC + gradH * rHess * gradC;
	    GDGHF = GDGHF + gradH * rHess;
	    GDGCC = GDGCC + gradC * rHess * gradC;
	    GDGCF = GDGCF + gradC * rHess;
	    GDGFF = GDGFF + rHess;
	  }
	}
	mod.cursor().freeStorage(modStore, modDeleteIt);
	res.cursor().freeStorage(resStore, resDeleteIt);
	mas.cursor().freeStorage(masStore, masDeleteIt);
      }
    } else {
      Lattice<Float> &prior = *(cemem_ptr->itsPrior_ptr);
      RO_LatticeIterator<Float> 
	pri( prior, LatticeStepper(prior.shape(), model.niceCursorShape()));    
      Bool priDeleteIt;
      for (mod.reset(), res.reset(), pri.reset(); !mod.atEnd(); mod++, res++, pri++) {
	const Float *modStore = mod.cursor().getStorage(modDeleteIt);
	const Float *resStore = res.cursor().getStorage(resDeleteIt);
	const Float *masStore = mas.cursor().getStorage(masDeleteIt);
	const Float *priStore = pri.cursor().getStorage(priDeleteIt);
	const Float *modIter = modStore;
	const Float *resIter = resStore;
	const Float *masIter = masStore;
	const Float *priIter = priStore;
	for (i=0;i<mod.cursor().nelements();i++,modIter++, resIter++, masIter++, priIter++) {
	  if (*masIter > 0.0) {
	    gradH = -tanh( (*modIter - *priIter)/aFit );
	    rHess = 1.0/( square(1.0-gradH)/aFit + ggc) ;
	    gradC = -2*(*resIter);
	    GDGHH = GDGHH + gradH * rHess * gradH;
	    GDGHC = GDGHC + gradH * rHess * gradC;
	    GDGHF = GDGHF + gradH * rHess;
	    GDGCC = GDGCC + gradC * rHess * gradC;
	    GDGCF = GDGCF + gradC * rHess;
	    GDGFF = GDGFF + rHess;
	  }
	}
	mod.cursor().freeStorage(modStore, modDeleteIt);
	res.cursor().freeStorage(resStore, resDeleteIt);
	mas.cursor().freeStorage(masStore, masDeleteIt);
	pri.cursor().freeStorage(priStore, priDeleteIt);
      }
    }

  }  else {  // No Mask

    if (cemem_ptr->itsPrior_ptr ==  0) {
      Float defLev = cemem_ptr->itsDefaultLevel;
      for (mod.reset(), res.reset(); !mod.atEnd(); mod++, res++) {
	const Float *modStore = mod.cursor().getStorage(modDeleteIt);
	const Float *resStore = res.cursor().getStorage(resDeleteIt);
	const Float *modIter = modStore;
	const Float *resIter = resStore;
	for (i=0;i<mod.cursor().nelements();i++,modIter++, resIter++) {
	  gradH = -tanh( (*modIter - defLev)/aFit );
	  rHess = 1.0/( square(1.0-gradH) /aFit + ggc) ;
	  gradC = -2*(*resIter);
	  GDGHH = GDGHH + gradH * rHess * gradH;
	  GDGHC = GDGHC + gradH * rHess * gradC;
	  GDGHF = GDGHF + gradH * rHess;
	  GDGCC = GDGCC + gradC * rHess * gradC;
	  GDGCF = GDGCF + gradC * rHess;
	  GDGFF = GDGFF + rHess;
	}
	mod.cursor().freeStorage(modStore, modDeleteIt);
	res.cursor().freeStorage(resStore, resDeleteIt);
      }
    } else {
      Lattice<Float> &prior = *(cemem_ptr->itsPrior_ptr);
      RO_LatticeIterator<Float> 
	pri( prior, LatticeStepper(prior.shape(), model.niceCursorShape()));    
      Bool priDeleteIt;
      for (mod.reset(), res.reset(), pri.reset(); !mod.atEnd(); mod++, res++, pri++) {
	const Float *modStore = mod.cursor().getStorage(modDeleteIt);
	const Float *resStore = res.cursor().getStorage(resDeleteIt);
	const Float *priStore = pri.cursor().getStorage(priDeleteIt);
	const Float *modIter = modStore;
	const Float *resIter = resStore;
	const Float *priIter = priStore;
	for (i=0;i<mod.cursor().nelements();i++,modIter++, resIter++, priIter++) {
	  gradH = -tanh( (*modIter - *priIter)/aFit );
	  rHess = 1.0/( square(1.0-gradH)/aFit + ggc) ;
	  gradC = -2*(*resIter);
	  GDGHH = GDGHH + gradH * rHess * gradH;
	  GDGHC = GDGHC + gradH * rHess * gradC;
	  GDGHF = GDGHF + gradH * rHess;
	  GDGCC = GDGCC + gradC * rHess * gradC;
	  GDGCF = GDGCF + gradC * rHess;
	  GDGFF = GDGFF + rHess;
	}
	mod.cursor().freeStorage(modStore, modDeleteIt);
	res.cursor().freeStorage(resStore, resDeleteIt);
	pri.cursor().freeStorage(priStore, priDeleteIt);
      }
    }

  }
  GDG(H,H) = GDGHH;
  GDG(H,C) = GDGHC;
  GDG(H,F) = GDGHF;
  GDG(C,C) = GDGCC;
  GDG(C,F) = GDGCF;
  GDG(F,F) = GDGFF;
  GDG(H,J) = GDGHH -  alpha * GDGHC - beta * GDGHF;
  GDG(C,J) = GDGHC -  alpha * GDGCC - beta * GDGCF;
  GDG(F,J) = GDGHF -  alpha * GDGCF - beta * GDGFF;
  GDG(J,J) = GDGHH +  square(alpha) * GDGCC 
    + square(beta)*GDGFF  + 2*alpha*beta*GDGCF  
    - 2*alpha*GDGHC - 2*beta*GDGHF;
};


//----------------------------------------------------------------------
void EntropyEmptiness::formGDGStep(Matrix<Double>& GDG)
{
  Lattice<Float> &model = *(cemem_ptr->itsModel_ptr);
  Lattice<Float> &resid = *(cemem_ptr->itsResidual_ptr);
  Lattice<Float> &step  = *(cemem_ptr->itsStep_ptr);

  GDG.resize(4,4);
  GDG.set(0.0);
  Float alpha = cemem_ptr->itsAlpha;
  Float beta  = cemem_ptr->itsBeta;
  Float aFit = cemem_ptr->itsAFit;

  // should not use Lattice Expression Language for efficiency
  // using it right now to get up and running
  Float ggc = 2 * alpha * cemem_ptr->itsQ;
  Float rHess;

  RO_LatticeIterator<Float> 
    mod( model, LatticeStepper(model.shape(), model.niceCursorShape()));
  RO_LatticeIterator<Float> 
    res( resid, LatticeStepper(resid.shape(), model.niceCursorShape()));
  LatticeIterator<Float> 
    stp( step, LatticeStepper(step.shape(), model.niceCursorShape()));
  uInt i;
  Bool modDeleteIt;
  Bool resDeleteIt;
  Bool stpDeleteIt;
  Bool masDeleteIt;
  Double GDGHH = 0.0;
  Double GDGHC = 0.0;
  Double GDGHF = 0.0;
  Double GDGCC = 0.0;
  Double GDGCF = 0.0;
  Double GDGFF = 0.0;
  Double gradH = 0.0;
  Double gradC = 0.0;
  Double gradJ = 0.0;
  Float stepValue;

  if (cemem_ptr->itsMask_ptr) { // Mask
    Lattice<Float> &mask  = *(cemem_ptr->itsMask_ptr);
    RO_LatticeIterator<Float> 
      mas( mask, LatticeStepper(model.shape(), model.niceCursorShape()));

    if ( cemem_ptr->itsPrior_ptr == 0) {
      Float defLev = cemem_ptr->itsDefaultLevel;
      for (mod.reset(), res.reset(), stp.reset(), mas.reset(); 
	   !mod.atEnd(); mod++, res++, stp++, mas++) {
	const Float *modStore = mod.cursor().getStorage(modDeleteIt);
	const Float *resStore = res.cursor().getStorage(resDeleteIt);
	const Float *masStore = mas.cursor().getStorage(masDeleteIt);
	Float *stpStore = stp.rwCursor().getStorage(stpDeleteIt);
	const Float *modIter = modStore;
	const Float *resIter = resStore;
	const Float *masIter = masStore;
	Float *stpIter = stpStore;
	for (i=0;i<mod.cursor().nelements();i++,modIter++, resIter++, stpIter++, masIter++) {
	  if (*masIter > 0.0) {
	    gradH = -tanh( (*modIter - defLev)/aFit );
	    rHess = 1.0/( square(1.0-gradH)/aFit + ggc) ;
	    gradC = -2*(*resIter);
	    gradJ = gradH - alpha*gradC - beta;
	    stepValue = rHess * gradJ;
	    (*stpIter) = stepValue;
	    GDGHH = GDGHH + gradH * rHess * gradH;
	    GDGHC = GDGHC + gradH * rHess * gradC;
	    GDGHF = GDGHF + gradH * rHess;
	    GDGCC = GDGCC + gradC * rHess * gradC;
	    GDGCF = GDGCF + gradC * rHess;
	    GDGFF = GDGFF + rHess;
	  }
	}
	mod.cursor().freeStorage(modStore, modDeleteIt);
	res.cursor().freeStorage(resStore, resDeleteIt);
	mas.cursor().freeStorage(masStore, masDeleteIt);
	stp.rwCursor().putStorage(stpStore, stpDeleteIt);
      }
    } else {
      Lattice<Float> &prior = *(cemem_ptr->itsPrior_ptr);
      RO_LatticeIterator<Float> 
	pri( prior, LatticeStepper(prior.shape(), model.niceCursorShape()));    
      Bool priDeleteIt;
      for (mod.reset(), res.reset(), mas.reset(), pri.reset(), stp.reset(); 
	   !mod.atEnd(); mod++, res++, mas++, pri++, stp++) {
	const Float *modStore = mod.cursor().getStorage(modDeleteIt);
	const Float *resStore = res.cursor().getStorage(resDeleteIt);
	const Float *masStore = mas.cursor().getStorage(masDeleteIt);
	const Float *priStore = pri.cursor().getStorage(priDeleteIt);
	Float *stpStore = stp.rwCursor().getStorage(stpDeleteIt);
	const Float *modIter = modStore;
	const Float *resIter = resStore;
	const Float *masIter = masStore;
	const Float *priIter = priStore;
	Float *stpIter = stpStore;
	for (i=0;i<mod.cursor().nelements();
	     i++,modIter++, resIter++, masIter++, priIter++, stpIter++) {
	  if (*masIter > 0.0) {
	    gradH = -tanh( (*modIter - *priIter)/aFit );
	    rHess = 1.0/( square(1.0-gradH)/aFit + ggc) ;
	    gradC = -2*(*resIter);
	    gradJ = gradH - alpha*gradC - beta;
	    stepValue = rHess * gradJ;
	    (*stpIter) = stepValue;
	    GDGHH = GDGHH + gradH * rHess * gradH;
	    GDGHC = GDGHC + gradH * rHess * gradC;
	    GDGHF = GDGHF + gradH * rHess;
	    GDGCC = GDGCC + gradC * rHess * gradC;
	    GDGCF = GDGCF + gradC * rHess;
	    GDGFF = GDGFF + rHess;
	  }
	}
	mod.cursor().freeStorage(modStore, modDeleteIt);
	res.cursor().freeStorage(resStore, resDeleteIt);
	mas.cursor().freeStorage(masStore, masDeleteIt);
	pri.cursor().freeStorage(priStore, priDeleteIt);
	stp.rwCursor().putStorage(stpStore, stpDeleteIt);
      }
    }

  } else { // No Mask

    if ( cemem_ptr->itsPrior_ptr == 0) {
      Float defLev = cemem_ptr->itsDefaultLevel;
      for (mod.reset(), res.reset(), stp.reset(); 
	   !mod.atEnd(); mod++, res++, stp++) {
	const Float *modStore = mod.cursor().getStorage(modDeleteIt);
	const Float *resStore = res.cursor().getStorage(resDeleteIt);
	Float *stpStore = stp.rwCursor().getStorage(stpDeleteIt);
	const Float *modIter = modStore;
	const Float *resIter = resStore;
	Float *stpIter = stpStore;
	for (i=0;i<mod.cursor().nelements();i++,modIter++, resIter++, stpIter++) {
	  gradH = -tanh( (*modIter - defLev)/aFit );
	  rHess = 1.0/( square(1.0-gradH)/aFit + ggc) ;
	  gradC = -2*(*resIter);
	  gradJ = gradH - alpha*gradC - beta;
	  stepValue = rHess * gradJ;
	  (*stpIter) = stepValue;
	  GDGHH = GDGHH + gradH * rHess * gradH;
	  GDGHC = GDGHC + gradH * rHess * gradC;
	  GDGHF = GDGHF + gradH * rHess;
	  GDGCC = GDGCC + gradC * rHess * gradC;
	  GDGCF = GDGCF + gradC * rHess;
	  GDGFF = GDGFF + rHess;
	}
	mod.cursor().freeStorage(modStore, modDeleteIt);
	res.cursor().freeStorage(resStore, resDeleteIt);
	stp.rwCursor().putStorage(stpStore, stpDeleteIt);
      }
    } else {
      Lattice<Float> &prior = *(cemem_ptr->itsPrior_ptr);
      RO_LatticeIterator<Float> 
	pri( prior, LatticeStepper(prior.shape(), model.niceCursorShape()));    
      Bool priDeleteIt;
      for (mod.reset(), res.reset(), pri.reset(), stp.reset(); 
	   !mod.atEnd(); mod++, res++, pri++, stp++) {
	const Float *modStore = mod.cursor().getStorage(modDeleteIt);
	const Float *resStore = res.cursor().getStorage(resDeleteIt);
	const Float *priStore = pri.cursor().getStorage(priDeleteIt);
	Float *stpStore = stp.rwCursor().getStorage(stpDeleteIt);
	const Float *modIter = modStore;
	const Float *resIter = resStore;
	const Float *priIter = priStore;
	Float *stpIter = stpStore;
	for (i=0;i<mod.cursor().nelements();
	     i++,modIter++, resIter++, priIter++, stpIter++) {
	  gradH = -tanh( (*modIter - *priIter)/aFit );
	  rHess = 1.0/( square(1.0-gradH)/aFit + ggc) ;
	  gradC = -2*(*resIter);
	  gradJ = gradH - alpha*gradC - beta;
	  stepValue = rHess * gradJ;
	  (*stpIter) = stepValue;
	  GDGHH = GDGHH + gradH * rHess * gradH;
	  GDGHC = GDGHC + gradH * rHess * gradC;
	  GDGHF = GDGHF + gradH * rHess;
	  GDGCC = GDGCC + gradC * rHess * gradC;
	  GDGCF = GDGCF + gradC * rHess;
	  GDGFF = GDGFF + rHess;
	}
	mod.cursor().freeStorage(modStore, modDeleteIt);
	res.cursor().freeStorage(resStore, resDeleteIt);
	pri.cursor().freeStorage(priStore, priDeleteIt);
	stp.rwCursor().putStorage(stpStore, stpDeleteIt);
      }
    }
  }
  GDG(H,H) = GDGHH;
  GDG(H,C) = GDGHC;
  GDG(H,F) = GDGHF;
  GDG(C,C) = GDGCC;
  GDG(C,F) = GDGCF;
  GDG(F,F) = GDGFF;
  GDG(H,J) = GDGHH -  alpha * GDGHC - beta * GDGHF;
  GDG(C,J) = GDGHC -  alpha * GDGCC - beta * GDGCF;
  GDG(F,J) = GDGHF -  alpha * GDGCF - beta * GDGFF;
  GDG(J,J) = GDGHH +  square(alpha) * GDGCC 
    + square(beta)*GDGFF  + 2*alpha*beta*GDGCF  
    - 2*alpha*GDGHC - 2*beta*GDGHF;
};



//----------------------------------------------------------------------
Double EntropyEmptiness::formGDS()
{
  Lattice<Float> &model = *(cemem_ptr->itsModel_ptr);
  Lattice<Float> &step  = *(cemem_ptr->itsStep_ptr);
  Lattice<Float> &resid = *(cemem_ptr->itsResidual_ptr);
  Float alpha = cemem_ptr->itsAlpha;
  Float beta = cemem_ptr->itsBeta;
  Float aFit = cemem_ptr->itsAFit;

  Double gds = 0;

  if (cemem_ptr->itsMask_ptr) {
    Lattice<Float> &mask = *(cemem_ptr->itsMask_ptr);
 
    if (cemem_ptr->itsPrior_ptr == 0) {
      Float defLev = cemem_ptr->itsDefaultLevel;
      gds = sum( mask * (step *
		 (-tanh((model - defLev)/aFit) + 2.0*alpha * resid -beta)))
	.getDouble();
    } else {
      Lattice<Float> &prior = *(cemem_ptr->itsPrior_ptr);
      gds = sum( mask * (step *
		 (-tanh((model - prior)/aFit) + 2.0*alpha * resid -beta)))
	.getDouble();
    }
  }  else {  // no mask
    if (cemem_ptr->itsPrior_ptr == 0) {
      Float defLev = cemem_ptr->itsDefaultLevel;
      gds = sum( step *
		 (-tanh((model - defLev)/aFit) + 2.0*alpha * resid -beta))
	.getDouble();
    } else {
      Lattice<Float> &prior = *(cemem_ptr->itsPrior_ptr);
      gds = sum( step *
		 (-tanh((model - prior)/aFit) + 2.0*alpha * resid -beta))
	.getDouble();
    }
  }
  return gds;
};


//----------------------------------------------------------------------
void EntropyEmptiness::infoBanner(){
  cemem_ptr->itsLog << 
    " I   Flux     Fit    Gradient "  << LogIO::POST;
};

//----------------------------------------------------------------------
void EntropyEmptiness::infoPerIteration(uInt iteration){
  cemem_ptr->itsLog << 
    iteration << "  " <<
    cemem_ptr->itsTotalFlux << "  " <<
    cemem_ptr->itsFit << "  " <<
    cemem_ptr->itsNormGrad << "  " <<
    LogIO::POST;

  if (cemem_ptr->itsProgressPtr) {
    Lattice<Float> &resid = *(cemem_ptr->itsResidual_ptr);
    Lattice<Float> &model = *(cemem_ptr->itsModel_ptr);
    Float maxResid = (max(abs(resid))).getFloat();
    // Note:  posMaximumResidual is not valid! (  IPosition(4,0,0,0,0)  )
    cemem_ptr->itsProgressPtr->
      info(false, iteration, cemem_ptr->itsNumberIterations, model, resid, 
	   maxResid, IPosition(4,0,0,0,0),
	   cemem_ptr->itsFlux, cemem_ptr->itsFit, cemem_ptr->itsNormGrad,
	   cemem_ptr->itsEntropy);
  }
};


//----------------------------------------------------------------------
Float EntropyEmptiness::relaxMin()
{
  // hey, Maximum Emptiness ain't GOT no minimum!
  Float requiredModelMin = -1e+20;
  return requiredModelMin;
};


//----------------------------------------------------------------------
Bool EntropyEmptiness::testConvergence()
{
  Bool converged;
  converged = ( (cemem_ptr->itsFit < cemem_ptr->itsTolerance) &&
		(cemem_ptr->itsIteration != (cemem_ptr->itsFirstIteration)) &&
		(cemem_ptr->itsNormGrad < (cemem_ptr->itsTolerance)) );
  if (cemem_ptr->itsUseFluxConstraint) {
    converged = (converged && 
		 (abs(cemem_ptr->itsFlux - cemem_ptr->itsTargetFlux) 
		    < cemem_ptr->itsTolerance*cemem_ptr->itsTargetFlux));
  }

  return converged;

};



} //# NAMESPACE CASA - END

