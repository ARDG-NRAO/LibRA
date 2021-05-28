//# LinearEquation.h: this defines LinearEquation
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
//# $Id$

#ifndef SYNTHESIS_LINEAREQUATION_H
#define SYNTHESIS_LINEAREQUATION_H


#include <casa/aips.h>
#include <synthesis/MeasurementEquations/LinearModel.h>
#include <synthesis/MeasurementEquations/ResidualEquation.h>


namespace casa { //# NAMESPACE CASA - BEGIN

// <summary> defines a relationship between Domain and Range objects </summary>

// <use visibility=export>

// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>

// <prerequisite> 
// This class should be read in conjunction with: 
// <li> <linkto class="LinearModel">LinearModel</linkto>
// <li> <linkto class="ResidualEquation">ResidualEquation</linkto>
// </prerequisite>
//
// <etymology>
//  LinearEquation was originally conceived for implementing linear
//  equations (like Ax=b) but was in hindsight found to be more general.
// </etymology>
//
// <synopsis>
// This abstract class defines the basic functions used for forward
// modelling of measured data of "Range" data type. It defines the
// functions used to transform a model, of "Domain" data type to predicted
// data. It can also compare the predicted data with the measured data and
// return residuals to classes derived from LinearModel which can then be
// used to update the model.
// </synopsis>
//
// <example>
// I'll pass this class into a subroutine as an actual instance of an
// abstract class cannot be constructed. 
// <srcblock>
// void foo(LinearModel< Image<casacore::Float> > mod, 
//          LinearEquation<Image<casacore::Float>, VisibilitySet>)
// VisibilitySet predictedVisibility;
// eqn.evaluate(predictedVisibility, mod); 
// </srcblock>
// </example>
//
// <motivation>
// This class was originally conceived to be used in implementing
// deconvolution algorithms. I would not be surprised if it found wider
// applicability. 
// </motivation>
//
// <templating arg=Domain>
// I do not see any restrictions on the Domain class. Its up to the derived
// class to handle the the appropriate Domain.
// </templating>
// <templating arg=Range>
// In order to calculate residuals it will probably be necessary for
// subtraction to be defined on the Range class, as well as some way for
// data in the Range data type to be converted back into the Domain data
// type. However it is left up to the derived classes to implement this. 
// </templating>
//
// <thrown>
// This is an interface class and does not contain any implementation (and
// hence does not throw exceptions)
// </thrown>
//
// <todo asof="1996/03/28">
// I expect that additional functions, other than evaluate() and those
// derived from ResidualEquation will be necessary. I have yet to decide if
// they should be defined in this class or derived classes.
// </todo>

template<class Domain, class Range> class LinearEquation: 
  public ResidualEquation<Domain>
{
public:
  // A virtual destructor may be necessary for use in derived classes.
  virtual ~LinearEquation();

  // This function evaluates the Equation for the specified model.
  // It returns false if the result could not be computed.
  virtual casacore::Bool evaluate(Range & result, 
			const LinearModel<Domain>& model) = 0;

  // evaluate residual
  virtual casacore::Bool residual(Domain & answer,
			const LinearModel<Domain> & model) = 0;
  
  // Same as above, but also calculates Chi^2
  virtual casacore::Bool residual(Domain & answer, casacore::Float & chisq,
			const LinearModel<Domain> & model) = 0;

  // Same as above, but also calculates Chi^2
  // considering a mask
  virtual casacore::Bool residual(Domain & answer, casacore::Float & chisq,
			Domain & mask,
			const LinearModel<Domain> & model) = 0;

};

} //# NAMESPACE CASA - END

#ifndef AIPS_NO_TEMPLATE_SRC
#include <synthesis/MeasurementEquations/LinearEquation.tcc>
#endif //# AIPS_NO_TEMPLATE_SRC
#endif


