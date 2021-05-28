//# LinearModel.h: this defines LinearModel
//# Copyright (C) 1996,1997,1998,1999
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

#ifndef SYNTHESIS_LINEARMODEL_H
#define SYNTHESIS_LINEARMODEL_H

#include <casa/aips.h>

namespace casa { //# NAMESPACE CASA - BEGIN

//# Forward Declarations

// <summary>Provides a model for use in model fitting applications</summary>

// <use visibility=export>

// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>

// <prerequisite> 
// This class should be read in conjunction with: 
// <li> <linkto class="LinearEquation">LinearEquation</linkto>
// <li> <linkto class="ResidualEquation">ResidualEquation</linkto>
// </prerequisite>
//
// <etymology>
// LinearModel was originally conceived for providing the model, and optionally
// for derived classes the iterative solution methods for linear equations
// (like Ax=b) but may be more general in scope.
// </etymology>
//
// <synopsis>
// This abstract class defines the interface between a model of the sky (or
// any other quantity) and the equation used to predict the measured
// quantities. It provides the current best guess model, and given
// measures of the discrepency between the current model and the data (as
// provided by the ResidualEquation class) derived classes may be able to
// solve for a better model that corresponds more closely to the measured
// data.
// </synopsis>
//
// <example>
// I'll pass this class into a function as an actual instance of an
// abstract class cannot be constructed. 
// <srcblock>
// void foo(LinearModel< Image<casacore::Float> > mod, 
//          ResidualEquation< Image<casacore::Float> > eqn)
// {
//   Image<casacore::Float> currentModel, newModel;
//   currentModel = mod.getModel(); // get the current model;
// }
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
// The template determines the external representation of the model used.
// So complicated templates can be expected and should be OK.
// </templating>
//
// <thrown>
// This is an abstract class and does not contain any implementation (and
// hence does not throw exceptions)
// </thrown>
//
// <todo asof="1996/03/29">
// I cannot see what else I need to do with this class (famous last words)
// </todo>

template<class Domain> class LinearModel
{
public:
  // A virtual destructor is necessary
  virtual ~LinearModel();

  // Return the current model.
  virtual const Domain & getModel() const = 0;

  // Set the current model
  virtual void setModel(const Domain & model) = 0;
};


} //# NAMESPACE CASA - END

#ifndef AIPS_NO_TEMPLATE_SRC
#include <synthesis/MeasurementEquations/LinearModel.tcc>
#endif //# AIPS_NO_TEMPLATE_SRC
#endif
