//# ConvolutionEquation.h: this defines ConvolutionEquation
//# Copyright (C) 1996,1997,1999,2000,2003
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
//#
//# $Id$

#ifndef SYNTHESIS_CONVOLUTIONEQUATION_H
#define SYNTHESIS_CONVOLUTIONEQUATION_H


#include <casa/aips.h>
#include <synthesis/MeasurementEquations/LinearEquation.h>
#include <synthesis/MeasurementEquations/LinearModel.h>
#include <casa/Arrays/MaskedArray.h>
#include <scimath/Mathematics/Convolver.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/IPosition.h>

namespace casa { //# NAMESPACE CASA - BEGIN

// <summary> Implements the convolution equation </summary>

// <use visibility=local>

// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>

// <prerequisite> 
// <li> <linkto class="HogbomCleanModel">HogbomCleanModel</linkto> 
//       (or similar classes) 
// <li> LinearModel/LinearEquation Paradigm 
// </prerequisite>
//
// <etymology>
// This class implements convolution within the LinearEquation framework.
// </etymology>
//
// <synopsis>
// This class is used in conjunction with classes like HogbomCleanModel to
// implement deconvolution algorithms. This class contains the point spread
// function (psf) and the convolved data (dirty image), and is able to
// convolve a supplied model with the psf to produce a predicted output
// (using the evaluate() function), or to subtract the convolved data and
// produce a residual (using the residual() function).
//
// See the documentation for 
// <linkto class=HogbomCleanModel>HogbomCleanModel</linkto> 
// for an example of how this class can be used to perform deconvolution.
//
// This class also contains specialised functions (like the version of
// evaluate() for a point source model) that speed up the calculation of the
// convolution. This specialised version of evaluate() does not need to
// actually perform the convolution and instead returns a suitable part of
// the psf (zero padded if necessary). When this function is called this
// class will get the psf from the convolver and cache it, on the assumption
// that many evaluations of this function will be requested (as occurs in
// Clean algorithms). 
// 
// The size and shape of the psf and the supplied model may be different. The
// only restriction is that the dimension of the psf must be less than or
// equal to the dimension of the model. If the dimension of the
// model is larger than the dimension of the psf then the convolution
// will be repeated along the slowest moving (last) axis. The dirty image
// and the supplied model must be the same size and shape. 
//
// This class can also operate on MaskedArrays (and models representable by
// MaskedArrays). But the mask is currently discarded and the convolution
// performed on the entire supplied model. This may change in the future.
//
// </synopsis>
//
// <example>
// <srcblock>
// casacore::Matrix<casacore::Float> psf(4,4), dirty(20,20), model(20,20);
// .... put some meaningful values into these Arrays....
// // create a convolution equation, and an array model
// ConvolutionEquation convEqn(psf, dirty);
// ArrayModel<casacore::Float> myModel(model);
// // now calculate the convolution of the model and the psf
// casacore::Matrix<casacore::Float> prediction;
// convEqn.evaluate(myModel, prediction);
// // and calculate the difference between the predicted and actual convolution
// casacore::Matrix<casacore::Float> residual;
// convEqn.residual(mymodel, residual)
// </srcblock>
// </example>
//
// <motivation>
// This class was designed with deconvolution in mind. 
// </motivation>
//
// <todo asof="1990/05/03">
//   <li> Fix up the way this class works for other data types. 
//        It currently can handle Arrays of Floats only.
//        I do not intend to extend it to handle casacore::Double, casacore::Complex, & DComplex
//        until there is a demand.
//   <li> This class is not templated. If necessary I would use templating
//        to produce a casacore::Double Precision Version.
// </todo>

class ConvolutionEquation: 
  public LinearEquation< casacore::Array<casacore::Float>, casacore::Array<casacore::Float> >
{
public:
  // Construct the ConvolutionEquation. Until I write some functions for
  // setting the private data the default constructor is essentially useless
  ConvolutionEquation();

  // Construct the ConvolutionEquation setting the psf and measured data
  ConvolutionEquation(const casacore::Array<casacore::Float> & psf, 
		      const casacore::Array<casacore::Float> & dirtyImage);

  // Construct the ConvolutionEquation setting the psf and measured data
  // Even though a casacore::MaskedArray is used as an arguement the mask is discarded
  // internally and hence not used by residual().
  ConvolutionEquation(const casacore::Array<casacore::Float> & psf, 
		      const casacore::MaskedArray<casacore::Float> & dirtyImage);

  // Somewhere I read that a destructor should alway be defined even if it
  // does nothing (as this one does).
  ~ConvolutionEquation();
  
  // Do the convolution of the model supplied by the LinearModel class with
  // the internal psf. Return the answer in result .
  virtual casacore::Bool evaluate(casacore::Array<casacore::Float> & result, 
			const LinearModel< casacore::Array<casacore::Float> > & model);

  // Do the convolution of the model supplied by the LinearModel class with
  // the internal psf. Return the answer in result. This version 
  // uses Masked arrays. but the mask is currently discarded internally. 
  casacore::Bool evaluate(casacore::Array<casacore::Float> & result, 
		const LinearModel< casacore::MaskedArray<casacore::Float> > & model);

  // Do the convolution of the model supplied by the LinearModel class with
  // the internal psf. Return the answer in result. This version 
  // uses MaskedArrays,  but the mask is not currently used. However
  // the model mask is transfered to the result unchanged.
  casacore::Bool evaluate(casacore::MaskedArray<casacore::Float> & result, 
		const LinearModel< casacore::MaskedArray<casacore::Float> > & model);

  // Do the convolution of the a point source model at position 'position'
  // with amplitude 'amplitude' and the internal psf. Return the answer in
  // result. 
  casacore::Bool evaluate(casacore::Array<casacore::Float> & result, const casacore::IPosition & position, 
		const casacore::Float amplitude, 
		const casacore::IPosition & modelShape);

  // Calculate the convolution of the model (supplied by the LinearModel
  // class) and the psf and the difference between this and the supplied
  // (presumably measured) convolution.  
  virtual casacore::Bool residual(casacore::Array<casacore::Float> & result, 
			const LinearModel< casacore::Array<casacore::Float> > & model);


  // Calculate the convolution of the model (supplied by the LinearModel
  // class) and the psf and the difference between this and the supplied
  // (presumably measured) convolution.   Also return chisq.
  virtual casacore::Bool residual( casacore::Array<casacore::Float> & result, casacore::Float & chisq, 
                        const LinearModel< casacore::Array<casacore::Float> > & model);

  // Calculate the convolution of the model (supplied by the LinearModel
  // class) and the psf and the difference between this and the supplied
  // (presumably measured) convolution.   Also return chisq,
  // considering a mask image
  virtual casacore::Bool residual( casacore::Array<casacore::Float> & result, casacore::Float & chisq, 
			 casacore::Array<casacore::Float> & mask,
			 const LinearModel< casacore::Array<casacore::Float> > & model);

  // Calculate the convolution of the model (supplied by the LinearModel
  // class) and the psf and the difference between this and the supplied
  // (presumably measured) convolution. This version 
  // uses Masked arrays. but the mask is currently discarded internally.
  casacore::Bool residual(casacore::Array<casacore::Float> & result, 
		const LinearModel< casacore::MaskedArray<casacore::Float> > & model);

  // Calculate the convolution of the model (supplied by the LinearModel
  // class) and the psf and the difference between this and the supplied
  // (presumably measured) convolution. This version 
  // uses Masked arrays. but the mask is currently discarded in the
  // calculations and transfered unchanged from the model to the result.
  casacore::Bool residual(casacore::MaskedArray<casacore::Float> & result, 
		const LinearModel< casacore::MaskedArray<casacore::Float> > & model);

  // return the psf size used in the convolution. The returned size does not
  // include any zero padding  
  casacore::IPosition psfSize();
  // release the storage associated with the cached psf. The psf can 
  // however still be recovered from the casacore::Convolver object
  void flushPsf();

private:
  casacore::Array<casacore::Float> theMeas;
  casacore::Array<casacore::Float> thePsf;
  casacore::IPosition theRealPsfSize;
  casacore::IPosition thePsfOrigin;
  casacore::Convolver<casacore::Float> theConv;
};


} //# NAMESPACE CASA - END

#endif
