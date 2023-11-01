//# ArrayModel.h: this defines ArrayModel
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

#ifndef SYNTHESIS_ARRAYMODEL_H
#define SYNTHESIS_ARRAYMODEL_H


#include <casacore/casa/aips.h>
#include <synthesis/MeasurementEquations/LinearModel.h>
#include <casacore/casa/Arrays/Array.h>

namespace casa { //# NAMESPACE CASA - BEGIN

// <summary> models with an internal & external representation 
// as an array </summary>

// <use visibility=export>

// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>

// <prerequisite>
//   <li> <linkto module="Arrays">Arrays</linkto>
//   <li> <linkto class="LinearModel">LinearModel</linkto>
//   <li> LinearModel/LinearEquation paradigm
// </prerequisite>
//
// <synopsis>
// An ArrayModel is a base class for Models that can be
// represented by arrays. It is expected that this class will be mainly used
// as base classes for other classes which will then provide the solve()
// functions necessary to update the model given an equation.
// 
// However this class does not contain any pure virtual functions and hence
// can be used "as is". An example of this is given below. For an example of
// how this class can be used by derived classes see the 
// <linkto class=HogbomCleanModel>HogbomCleanModel</linkto> 
// class.
//
// This class makes an internal copy of the casacore::Array supplied to it (either
// when constructed or when using the setModel function). If this is found
// to significantly affect performance (execution speed or memory
// requirements) this may be changed to a reference, perhaps using a smart
// pointer like the <linkto class=casacore::COWPtr>COWPtr</linkto>
// </synopsis>
//
// <example>
// <srcblock>
// ArrayModel<casacore::Float> currentModel(); // Cannot use the model yet!
// {
//   casacore::Matrix<casacore::Float> bestGuess(32,32);
//    ... put your best guess into the casacore::Matrix ...
//   currentModel.setModel(bestGuess); // This does a real copy
// }
// ConvolutionEquation eqn(psf, dirty); // psf, and dirty are arrays defined
//                                      // elsewhere.
// eqn.evaluate(result, currentModel); // Here result is the convolution of
//                                     // of the model with the psf.
// </srcblock>
// </example>
//
// <motivation>
// All the different image plane based clean algorithms have a common
// implementation in that they can use an array to store the current
// model. This class provides a way to abstract this functionality.
// </motivation>
//
// <templating arg=T>
// While the template arguement for this class can be just about anything,
// the use of this class with an equation class will significantly restrict
// the possible templates. I have used this class (or derivations of it)
// with the following data types.
//    <li> Float
//    <li> StokesVector
// </templating>
//
// <thrown>
// This class does not explicitly throw exceptions however the objects used
// by this class may
// </thrown>
//
// <todo asof="1996/05/14">
//   <li> Decide whether to copy the input array by reference or stay with
//   the current scheme.
// </todo>

template<class T> class ArrayModel: 
  public LinearModel<casacore::Array<T> > {
public:

  ArrayModel();
  ArrayModel(const casacore::Array<T> & model);
  ArrayModel(casacore::Array<T> & model);

  virtual const casacore::Array<T> & getModel() const;
  virtual void getModel(casacore::Array<T>& model) const;
  virtual void setModel(const casacore::Array<T>& model);
  virtual void setModel(casacore::Array<T> & model);

protected:
  casacore::Array<T> theModel;
};


} //# NAMESPACE CASA - END

#ifndef AIPS_NO_TEMPLATE_SRC
#include <synthesis/MeasurementEquations/ArrayModel.tcc>
#endif //# AIPS_NO_TEMPLATE_SRC
#endif
