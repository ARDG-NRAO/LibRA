//# ResidualEquation.h: this defines ResidualEquation
//# Copyright (C) 1996,1997,1999,2000
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
//#
//# $Id$

#ifndef SYNTHESIS_RESIDUALEQUATION_H
#define SYNTHESIS_RESIDUALEQUATION_H

#include <casacore/casa/aips.h>
#include <synthesis/MeasurementEquations/LinearModel.h>

namespace casa { //# NAMESPACE CASA - BEGIN

template<class Domain> class LinearModel;

// <summary>Interface class containing functions returning "Domain" type</summary>

// <use visibility=local>

// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>

// <prerequisite>
//   This class should be read in conjunction with:
//   <li> <linkto class="LinearModel">LinearModel</linkto>
//   <li> <linkto class="LinearEquation">LinearEquation</linkto>
// </prerequisite>
//
// <etymology>
// ResidualEquation reflects the fact that the residual() function was the
// first, and perhaps most important member of the class.
// </etymology>
//
// <synopsis>
// ResidualEquation is an abstract interface class into the more extensive
// LinearEquation class. It is composed of the subset of functions from
// LinearEquation which return objects of the "Domain", type. These objects
// can be used by the solve() function in the LinearModel to determine a new
// model. The separation of the ResidualEquation functions isolates the
// model, of type "Domain" from knowing what the data "Range", used by the
// LinearEquation class is. 
// </synopsis>
//
// <example>
// Suppose we have a simple class that has a vector model and knows (using
// the solve() function) how to update this vector if it is told the error
// betweeen the measured data and a model of the data produced using the
// current vector. 
// <srcblock> 
// SimpleModel< casacore::Vector<casacore::Float> > simplemodel;
// </srcblock> 
// 
// Suppose we also have a big complicated equation that does
// the forward modelling from vectors to some arbitrary class;
//
// <srcblock> 
// FancyEquation<casacore::Vector<casacore::Float>, VisibilitySet> eqn;
// </srcblock> 
//
// Then Simplemodel class does not need to know anything about the
// VisibilitySet class as it knows that the FancyEquation class will have 
// (via inheritence) a ResidualEquation interface which returns vectors,
// so that it can use code like:
// <srcblock> 
// casacore::Vector<casacore::Float> error;
// if (eqn.residual(*this, error)){ 
//   use the error to determine a new model
// }
// </srcblock> 
// as part of the solve function.
// </example>
//
// <motivation>
// The main reason for the existence of this class is to isolate the type of
// the model ("Domain" type) from knowing what the "Range" of the equation is.
// </motivation>
//
// <templating arg=Domain>
// The template determines the type used to return the results for all the
// functions in this class. So even complicated template arguements should
// be OK.
// </templating>
//
// <thrown>
// This is an interface class and does not contain any implementation (and
// hence does not throw exceptions)
// </thrown>
//
// <todo asof="1996/03/29">
//   <li> decide whether to add a derivative function
//   <li> decide whether functions which return scalars should also be in
//   this class
// </todo>

template<class Domain> class ResidualEquation
{
public:
  // A virtual destructor may be necessary for use in derived classes.
  virtual ~ResidualEquation();

  // The canonical member of this class defines a function which 
  // gives the residual when the model is propagated through the
  // equation and compared with the data. It returns false if the answer
  // could not be computed.
  virtual casacore::Bool residual(Domain & answer,
			const LinearModel<Domain> & model) = 0;

  // Same as above, but also calculates Chi^2 (rms of residual image)
  virtual casacore::Bool residual(Domain & answer, casacore::Float & chisq,
                      const LinearModel<Domain> & model) = 0;

  // Same as above, but also calculates Chi^2 (rms of residual image)
  // considering a mask image
  virtual casacore::Bool residual(Domain & answer, casacore::Float & chisq,
			Domain & mask,
			const LinearModel<Domain> & model) = 0;

  // A proposal for another member of this class which returns the
  // derivative of the equation with respect to the current model.
  //   virtual casacore::Bool derivative(const LinearModel<Domain> & model, 
  // 			Domain & answer) = 0;


};


} //# NAMESPACE CASA - END

#ifndef AIPS_NO_TEMPLATE_SRC
#include <synthesis/MeasurementEquations/ResidualEquation.tcc>
#endif //# AIPS_NO_TEMPLATE_SRC
#endif
