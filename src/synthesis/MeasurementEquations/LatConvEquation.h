//# LatConvEquation.h: this defines LatConvEquation
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
//#
//# $Id$

#ifndef SYNTHESIS_LATCONVEQUATION_H
#define SYNTHESIS_LATCONVEQUATION_H


#include <casacore/casa/aips.h>
#include <synthesis/MeasurementEquations/LinearEquation.h>
#include <casacore/lattices/LatticeMath/LatticeConvolver.h>
#include <casacore/lattices/Lattices/Lattice.h>
#include <casacore/casa/Arrays/IPosition.h>
#include <casacore/casa/Arrays/Array.h>

namespace casa { //# NAMESPACE CASA - BEGIN

template <class Domain> class LinearModel;


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
//
// </synopsis>
//
// <example>
// <srcblock>
// casacore::PagedArray<casacore::Float> psf(2,4,4), dirty(2,20,20), model(2,20,20);
// .... put some meaningful values into these Lattices....
// // create a convolution equation, and a casacore::PagedArray model
// LatConvEquation convEqn(psf, dirty);
// LinearModel< casacore::Lattice<casacore::Float> > myModel(model);
// // now calculate the convolution of the model and the psf
// casacore::PagedArray<casacore::Float> prediction;
// convEqn.evaluate(myModel, prediction);
// // and calculate the difference between the predicted and actual convolution
// casacore::PagedArray<casacore::Float> residual;
// convEqn.residual(mymodel, residual)
// </srcblock>
// </example>
//
// <motivation>
// This class was designed with deconvolution in mind. 
// </motivation>
//
// <todo asof="1990/05/03">
//   <li> Reinstate the  evaluate() method with position argument.
//   <li> Fix up copies and references with Ralph Marson's help
//   <li> This class is not templated. If necessary I would use templating
//        to produce a casacore::Double Precision Version.
// </todo>

class LatConvEquation: 
  public LinearEquation< casacore::Lattice<casacore::Float>, casacore::Lattice<casacore::Float> >
{
public:

  // Construct the LatConvEquation setting the psf and measured data
  LatConvEquation(casacore::Lattice<casacore::Float> & psf, 
		  casacore::Lattice<casacore::Float> & dirtyImage);

  // destroy
  virtual ~LatConvEquation();
  
  // Do the convolution of the model supplied by the LinearModel class with
  // the internal psf. Return the answer in result .
  virtual casacore::Bool evaluate(casacore::Lattice<casacore::Float> & result, 
			const LinearModel< casacore::Lattice<casacore::Float> > & model);

  // Do the convolution of the a point source model at position 'position'
  // with amplitude 'amplitude' and the internal psf. Return the answer in
  // result. 
  casacore::Lattice<casacore::Float> * evaluate(const casacore::IPosition & position, 
			    const casacore::Float amplitude, 
			    const casacore::IPosition & modelShape);

  // Do the convolution of the a point source model at position 'position'
  // with amplitude 'amplitude' and the internal psf. Return the answer in
  // result, which is an array
  // Calling this will result in throwing away creating a new PSF owned by
  // this object;
  casacore::Bool evaluate(casacore::Array<casacore::Float> & result, const casacore::IPosition & position, 
		const casacore::Float amplitude, 
		const casacore::IPosition & modelShape);

  // Calculate the convolution of the model (supplied by the LinearModel
  // class) and the psf and the difference between this and the supplied
  // (presumably measured) convolution.  
  virtual casacore::Bool residual(casacore::Lattice<casacore::Float> & result, 
			const LinearModel< casacore::Lattice<casacore::Float> > & model);

  // Calculate the convolution of the model (supplied by the LinearModel
  // class) and the psf and the difference between this and the supplied
  // (presumably measured) convolution.   Also return chisq.
  virtual casacore::Bool residual(casacore::Lattice<casacore::Float> & result, casacore::Float & chisq, 
			const LinearModel< casacore::Lattice<casacore::Float> > & model);

  // Calculate the convolution of the model (supplied by the LinearModel
  // class) and the psf and the difference between this and the supplied
  // (presumably measured) convolution.   Also return chisq, considering mask
  virtual casacore::Bool residual(casacore::Lattice<casacore::Float> & result, casacore::Float & chisq, 
			casacore::Lattice<casacore::Float> & mask,
			const LinearModel< casacore::Lattice<casacore::Float> > & model);

  // return the psf size used in the convolution. The returned size does not
  // include any zero padding  
  casacore::IPosition psfSize();

protected:

  // Don't use this one, due to the casacore::Lattice<casacore::Float> &
  LatConvEquation();
  casacore::Lattice<casacore::Float> * itsMeas;
  casacore::Lattice<casacore::Float> * itsPsf;
  casacore::LatticeConvolver<casacore::Float> itsConv;
  casacore::IPosition itsPsfOrigin;
  casacore::IPosition itsRealPsfSize;
  casacore::Bool itsVirgin;
};


} //# NAMESPACE CASA - END

#endif
