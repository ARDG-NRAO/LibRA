//# LatConvEquation.cc:  this defines LatConvEquation
//# Copyright (C) 1996,1997,1998,1999,2000,2001,2003
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

#include <synthesis/MeasurementEquations/LatConvEquation.h>
#include <synthesis/MeasurementEquations/LinearModel.h>
#include <lattices/LEL/LatticeExpr.h>
#include <lattices/LEL/LatticeExprNode.h>
#include <lattices/Lattices/SubLattice.h>
#include <lattices/LRegions/LCBox.h>
#include <casa/Utilities/Assert.h>
#include <casa/Arrays/Vector.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/IPosition.h>
#include <casa/Exceptions/Error.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

LatConvEquation::LatConvEquation(Lattice<Float> & psf, 
				 Lattice<Float> & dirtyImage)
  :itsMeas(&dirtyImage),
   itsPsf(&psf),
   itsConv(psf, dirtyImage.shape(), true)
{
  itsVirgin = true;
  itsRealPsfSize = psf.shape();
  itsPsfOrigin = itsPsf->shape()/2;  
}

LatConvEquation::~LatConvEquation() {
  if (!itsVirgin) {
    //    cout << "Deleting nonVirgin PSF" << endl;
    delete itsPsf;
  }
}

Bool LatConvEquation::evaluate(Lattice<Float> & result, 
			       const LinearModel<Lattice<Float> > & model){
  const Lattice<Float> & modelLattice = model.getModel();
  //  DebugAssert(modelLattice.shape().isEqual(result.shape()), AipsError);
  itsConv.linear(result, modelLattice); 
  return true;
}


Lattice<Float> * 
LatConvEquation::evaluate(const IPosition & position, 
			  const Float amplitude,
			  const IPosition & modelSize){
  if (itsPsf->nelements() == 0){
    itsConv.getPsf(*itsPsf);
    itsPsfOrigin = itsPsf->shape()/2;
    itsRealPsfSize = itsPsf->shape();
  }
  IPosition psfSize = itsPsf->shape();
  IPosition blc = itsPsfOrigin - position;
  IPosition trc = blc + modelSize - 1;
  // Check if the required bounds are outside the psf. If they are then
  // resize the psf (with zero padding) to encompass the requested
  // convolution.  Another way to do this is to just return the required
  // portion of the psf (of size modelSize) suitably padded. This will be
  // quicker and more memory efficient for just one evaluation, but if the
  // user is cleaning near the edges of their image, then it will have to be
  // done for each iteration that is near the edge. By resizing the whole
  // psf when necessary it will after a few resizes be at the required size
  // for all the iterations.
  //
  // If we resize the psf and itsVirgin == true, we need to create a new psf and
  // make sure itsVirgin = false
  //
  if ((min(blc.asVector()) < 0) || 
      (max((trc-psfSize).asVector()) >= 0))
    {
      

      // newtrc and newblc are the points in the newPsf that
      // refer to the old Psf (itsPsf).
      IPosition newSize(itsPsf->ndim(), 0);
      IPosition newtrc(itsPsf->ndim(), 0);
      IPosition newblc(itsPsf->ndim(), 0);
      for(uInt i = 0; i < itsPsf->ndim(); i++){
	newblc(i) = - min(blc(i), 0);
	newSize(i) = std::max(trc(i)+1, psfSize(i)) + newblc(i);
	newtrc(i) = newblc(i) + psfSize(i) - 1;
      }
      {
	// expand/pad itsPsf
	Lattice<Float> *newPsf;
	newPsf = new TempLattice<Float>(newSize);
	newPsf->set(0.0);
	LCBox box(newblc,newtrc,newSize);
	SubLattice<Float>  newPsfSub (*newPsf, box, true);
	newPsfSub.copyData(*itsPsf);  // fill the old Psf into the larger new one

	if (itsVirgin) {
	  // just drop old itsPsf; someone else is responcible
	  itsPsf = newPsf;
	  itsVirgin = false;
	} else {
	  // we must clean up old itsPsf
	  delete itsPsf;
	  itsPsf = newPsf;
	}
      }
      itsPsfOrigin = itsPsfOrigin + newblc;
      LCBox box(blc+newblc, trc+newtrc, newSize);
      Lattice<Float> *result = 0;
      if (!nearAbs(Double(amplitude),1.0)) {
	SubLattice<Float>  newPsfSub( *itsPsf, box, true);
	result = new TempLattice<Float>(newPsfSub.shape());
	result->copyData((LatticeExpr<Float>)((amplitude) * newPsfSub ));	
      } else {
	result = new  SubLattice<Float>( *itsPsf, box, true);
      }
      return result;
    }  else {
      LCBox box(blc,trc,itsPsf->shape());
      Lattice<Float> *result = 0;
      if (!nearAbs(Double(amplitude),1.0)) {
	SubLattice<Float>  newPsfSub( *itsPsf, box, true);
	result = new TempLattice<Float>(newPsfSub.shape());
	result->copyData((LatticeExpr<Float>)((amplitude) * newPsfSub ));	
      } else {
	result = new  SubLattice<Float>( *itsPsf, box, true);
      }
      return result;
    }
};


Bool LatConvEquation::evaluate(Array<Float> & result, 
			       const IPosition & position, 
			       const Float amplitude,
			       const IPosition & modelSize)
{
  Lattice<Float> *resultLattice = 0;
  resultLattice = evaluate(position, amplitude, modelSize);
  if (resultLattice != 0) {
    result = resultLattice->get();
    delete resultLattice; resultLattice=0;
    return true;
  } else {
    return false;
  }
};


Bool LatConvEquation::residual(Lattice<Float> & result, 
			       const LinearModel< Lattice<Float> > & model) {

  if (evaluate(result, model)) {
    LatticeExpr<Float> expr = *itsMeas - result;
    result.copyData(expr);
    return true;
  }
  return false;
}


Bool LatConvEquation::residual(Lattice<Float> & result, 
			       Float & chisq,
			       const LinearModel<Lattice<Float> > & model) {
  if (residual(result, model)) {
    LatticeExprNode myChisq = sum(result * result);
    chisq = myChisq.getFloat();
    return true;
  }
  return false;
}


Bool LatConvEquation::residual(Lattice<Float> & result, 
			       Float & chisq,
			       Lattice<Float> & mask,
			       const LinearModel<Lattice<Float> > & model) {
  if (residual(result, model)) {
    result.copyData( (LatticeExpr<Float>) (result * mask) );
    LatticeExprNode myChisq = sum(result * result);
    chisq = myChisq.getFloat();
    return true;
  }
  return false;
}

IPosition LatConvEquation::psfSize() {
  return itsConv.psfShape();
}

} //# NAMESPACE CASA - END

