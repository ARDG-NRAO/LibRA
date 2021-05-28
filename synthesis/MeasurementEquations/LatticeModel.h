//# LatticeModel.h: this defines LatticeModel
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

#ifndef SYNTHESIS_LATTICEMODEL_H
#define SYNTHESIS_LATTICEMODEL_H

#include <casa/aips.h>
#include <synthesis/MeasurementEquations/LinearModel.h>
#include <lattices/Lattices/Lattice.h>

namespace casa { //# NAMESPACE CASA - BEGIN

// <summary> models with an internal & external representation as an casacore::Lattice </summary>

// <use visibility=export>

// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>

// <prerequisite>
//   <li> <linkto module="Lattices">Lattices</linkto> module
//   <li> <linkto class="LinearModel">LinearModel</linkto> class
//   <li> LinearModel/LinearEquation paradigm
// </prerequisite>
//
// <synopsis>
// An LatticeModel is a base class for Models that can be
// represented by Lattices. It is expected that this class will be mainly 
// used as base classes for other classes which will then provide the solve()
// functions necessary to update the model given an equation.
// 
// However this class does not contain any pure virtual functions and hence
// can be used "as is". An example of this is given below. For an example of
// how this class can be used by derived classes see the 
// <linkto class=HogbomCleanModel>HogbomCleanModel</linkto> 
// class.
//
// </synopsis>
//
// <example>
// <srcblock>
// LatticeModel<casacore::Float> currentModel(); // Cannot use the model yet!
// {
//   casacore::PagedImage<casacore::Float> bestGuess(Iposition(2,32,32));
//    ... put your best guess into the casacore::Matrix ...
//   currentModel.setModel(bestGuess); // This does a real copy
// }
// ConvolutionEquation eqn(psf, dirty); // psf, and dirty are PagedImages defined
//                                      // elsewhere.
// eqn.evaluate(result, currentModel); // Here result is the convolution of
//                                     // of the model with the psf.
// </srcblock>
// </example>
//
// <motivation>
// All the different image plane based clean algorithms have a common
// implementation in that they can use an casacore::Lattice (ie, casacore::PagedImage)
// to store the current
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
// <todo asof="1998/11/06">
//   <li>  We don't have any "set" or "constructor" methods which
//   take constants.  We work in terms of pointers now because of
//   casacore::Lattice::operator= is protected, and Ger said it was the right thing.
// </todo>

class LatticeModel
  :public LinearModel<casacore::Lattice<casacore::Float> > {
public:

  LatticeModel(casacore::Lattice<casacore::Float>& mod);

  // The destructor does nothing.
  virtual ~LatticeModel();

  // returns a reference to the model
  virtual const casacore::Lattice<casacore::Float> & getModel() const { return *itsModelPtr;}

  // Change the underlying Model to to the one specified. Reference semantics
  // are used so that no data is copied.
  virtual void setModel(const casacore::Lattice<casacore::Float> & model) { itsModelPtr = model.clone(); }

private:

    casacore::Lattice<casacore::Float> * itsModelPtr;
};


} //# NAMESPACE CASA - END

#endif
