//# ConvolutionEquation.cc:  this defines ConvolutionEquation
//# Copyright (C) 1996,1997,1998,1999,2000,2003
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

#include <synthesis/MeasurementEquations/ConvolutionEquation.h>
#include <casacore/casa/Arrays/ArrayMath.h>
#include <casacore/casa/BasicMath/Math.h>
#include <casacore/casa/Arrays/Vector.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

ConvolutionEquation::ConvolutionEquation(){
};

ConvolutionEquation::ConvolutionEquation(const Array<Float> & psf, 
					 const Array<Float> & dirtyImage){
  theConv.setPsf(psf, dirtyImage.shape());
  theConv.setFastConvolve();
  theRealPsfSize = psf.shape();
  theMeas.resize(dirtyImage.shape());
  theMeas = dirtyImage;
};

ConvolutionEquation::ConvolutionEquation(const Array<Float>& psf, 
					 const MaskedArray<Float>& dirtyImage){
  theConv.setPsf(psf, dirtyImage.shape());
  theConv.setFastConvolve();
  theRealPsfSize = psf.shape();
  theMeas.resize(dirtyImage.shape());
  theMeas = dirtyImage.getArray();
};

ConvolutionEquation::~ConvolutionEquation(){
};

Bool ConvolutionEquation::evaluate(Array<Float> & result, 
				   const LinearModel<Array<Float> > & model){
  const Array<Float> modelArray = model.getModel();
  theConv.linearConv(result, modelArray, false); 
  return true;
};

Bool ConvolutionEquation::evaluate(Array<Float> & result, 
				   const LinearModel< MaskedArray<Float> > & model) {
  const MaskedArray<Float> modelArray = model.getModel();
  theConv.linearConv(result, modelArray.getArray(), false); 
  return true;
}

Bool ConvolutionEquation::evaluate(Array<Float> & result, 
				   const IPosition & position, 
				   const Float amplitude,
				   const IPosition & modelSize){
  if (thePsf.nelements() == 0){
    thePsf = theConv.getPsf(false);
    thePsfOrigin = thePsf.shape()/2;
  }
  IPosition psfSize = thePsf.shape();
  IPosition blc = thePsfOrigin - position;
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
  if ((min(blc.asVector()) < 0) || 
      (max((trc-psfSize).asVector()) >= 0))
    {
      IPosition newSize(thePsf.ndim(), 0);
      IPosition newtrc(thePsf.ndim(), 0);
      IPosition newblc(thePsf.ndim(), 0);
      for(uInt i = 0; i < thePsf.ndim(); i++){
	newblc(i) = -std::min(blc(i), (ssize_t)0);
	newSize(i) = std::max(trc(i)+1, psfSize(i)) + newblc(i);
	newtrc(i) = newblc(i) + psfSize(i) - 1;
      }
      {
	Array<Float> newPsf(newSize);
	newPsf = 0;
	newPsf(newblc,newtrc) = thePsf;
	thePsf.reference(newPsf);
      }
      thePsfOrigin = thePsfOrigin + newblc;
      result = thePsf(blc+newblc, trc+newblc);
      if (!nearAbs(Double(amplitude),Double(1.0)))
	result = thePsf(blc+newblc, trc+newblc)*amplitude;
      return true;
    }
  else {
    result = thePsf(blc, trc);
    if (!nearAbs(Double(amplitude),1.0))
      result = result * amplitude;
    return true;
  }
};

Bool ConvolutionEquation::
residual(Array<Float> & result, 
	 Float & chisq,
	 const LinearModel< Array<Float> > & model) {
  if (residual(result, model)) {
    chisq = sum(result*result);
    return true;
  }
  else
    return false;
}


Bool ConvolutionEquation::
residual(Array<Float> & result, 
	 Float & chisq,
	 Array<Float> & mask,
	 const LinearModel< Array<Float> > & model) {
  if (residual(result, model)) {
    result = result * mask;
    chisq = sum(result*result);
    return true;
  }
  else
    return false;
}


Bool ConvolutionEquation::
residual(Array<Float> & result, 
	 const LinearModel< Array<Float> > & model) {
  if (evaluate(result, model)) {
    result = theMeas - result;
    return true;
  }
  else
    return false;
}




Bool ConvolutionEquation::residual(Array<Float> & result, 
				   const LinearModel< MaskedArray<Float> > & model) {
  if (evaluate(result, model)) {
    result = theMeas - result;
    return true;
  }
  else
    return false;
}

Bool ConvolutionEquation::residual(MaskedArray<Float> & result, 
				   const LinearModel< MaskedArray<Float> > & model) {
  Array<Float> farray;
  if (residual(farray, model)) {
    result.setData(farray, model.getModel().getMask());
    return true;
  }
  else
    return false;
}

IPosition ConvolutionEquation::psfSize(){
  return theRealPsfSize;
}

void ConvolutionEquation::flushPsf(){
  thePsf.resize(IPosition(0));
}

} //# NAMESPACE CASA - END

