//# IPLatConvEquation.cc:  this defines IPLatConvEquation
//# Copyright (C) 1996,1997,1998,1999,2001
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

#include <synthesis/MeasurementEquations/IPLatConvEquation.h>
#include <synthesis/MeasurementEquations/LinearModel.h>
#include <synthesis/MeasurementEquations/LatticeModel.h>
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

IPLatConvEquation::IPLatConvEquation(Lattice<Float> & psf, 
				     Lattice<Float> & dirtyImage)
  : LatConvEquation(psf, dirtyImage)
{
  itsQ = sum(psf).getFloat();
}

IPLatConvEquation::~IPLatConvEquation() 
{
}

Bool IPLatConvEquation::residual(Lattice<Float> & result, 
			       const LinearModel< Lattice<Float> > & model) {

  if (LatConvEquation::residual(result, model)) {
    // Image Plane requires a second convolution
    TempLattice<Float> intermediate(result.shape());
    intermediate.copyData(result);
    LatticeModel intermediateModel (intermediate);
    if (LatConvEquation::evaluate(result, intermediateModel)) {
      result.copyData( LatticeExpr<Float>(result/itsQ) );
      return true;
    }
  }
  return false;
}


Bool IPLatConvEquation::residual(Lattice<Float> & result, 
			       Float & chisq,
			       const LinearModel<Lattice<Float> > & model) {
  if (LatConvEquation::residual(result, chisq, model)) {
    // Image Plane requires a second convolution
    TempLattice<Float> intermediate(result.shape());
    intermediate.copyData(result);
    LatticeModel intermediateModel (intermediate);
    if (LatConvEquation::evaluate(result, intermediateModel)) {
     result.copyData( LatticeExpr<Float>(result/itsQ) );
      return true;
    }
  }
  return false;
}

// Local Variables: 
// compile-command: "gmake OPTLIB=1 IPLatConvEquation"
// End: 

} //# NAMESPACE CASA - END

