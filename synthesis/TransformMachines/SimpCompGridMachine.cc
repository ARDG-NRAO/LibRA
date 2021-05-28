//# SimpleComponentGridMachine.cc: Implementation of SimpleComponentGridMachine class
//# Copyright (C) 1997,1998,1999,2000,2001
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

#include <synthesis/TransformMachines/SimpCompGridMachine.h>
#include <scimath/Mathematics/RigidVector.h>
#include <components/ComponentModels/ComponentList.h>
#include <components/ComponentModels/ComponentShape.h>
#include <components/ComponentModels/ComponentType.h>
#include <components/ComponentModels/Flux.h>
#include <components/ComponentModels/SkyComponent.h>
#include <components/ComponentModels/SpectralModel.h>
#include <msvis/MSVis/VisBuffer.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/Cube.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/Vector.h>
#include <casa/Arrays/IPosition.h>
#include <casa/BasicSL/Complex.h>
#include <casa/BasicSL/Constants.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

void SimpleComponentGridMachine::get(VisBuffer& vb,  SkyComponent& component,
				     Int row)
{
  // If row is -1 then we pass through all rows
  uInt startRow, endRow;
  if (row < 0) {
    startRow = 0;
    endRow = vb.nRow() - 1;
  } else {
    startRow = row;
    endRow = row;
  }
  const uInt nRow = endRow - startRow + 1;

  const uInt npol=vb.visCube().shape()(0);
  const uInt nChan=vb.visCube().shape()(1);
  const IPosition blc(3, 0, 0, startRow);
  const IPosition trc(3, npol-1, nChan-1, endRow);
  const IPosition inc(3, 1);
  Cube<Complex> modelData = vb.modelVisCube()(blc, trc, inc);
  Vector<Double> xyPos(2);
  Vector<Complex> visibility(4);
  const Vector<Double> & frequency = vb.frequency();

  // Find the offsets in polarization. 
  Int startPol, offPol;
  Vector<Int> corrType(vb.corrType());
  startPol=corrType(0);
  if((startPol>4)&&(startPol<9)) {
    offPol=5;
  }
  else if((startPol>8)&&(startPol<13)) {
    offPol=9;
  }
  else {
    offPol=startPol;
  }

  Bool constantSpectrum = false;
  if (component.spectrum().type() == ComponentType::CONSTANT_SPECTRUM) {
    constantSpectrum = true;
  }
  Bool pointShape = false;
  if (component.shape().type() == ComponentType::POINT) {
    pointShape = true;
  }

  // Loop over all rows
  Vector<Double> uvValue(3);
  uvValue=0.0;
  for (uInt r = 0; r < nRow; r++) {
    for (uInt chn = 0; chn < nChan; chn++) {
      if (chn == 0 || !(pointShape && constantSpectrum)) {
	Vector<DComplex> fluxValue = 
	  component.visibility(uvValue, frequency(chn)).value();
	convertArray(visibility, fluxValue);
      }
      for (uInt pol=0; pol < npol; pol++) {
	modelData(pol, chn, r) = visibility(corrType(pol)-offPol);
      }
    }
  }
}





void SimpleComponentGridMachine::get(VisBuffer& vb, const ComponentList& compList,
				     Int row)
{

  // Unlike the SimpleComponentFTMachine, we don't have overhead
  // to save via this method; but we need to provide it due to the
  // inheritance structure, so we just loop through
  // and call the get(SkyComponent).

 uInt ncomponents=compList.nelements();
  for (uInt icomp=0;icomp<ncomponents;icomp++) {
    SkyComponent component=compList.component(icomp).copy();
    get(vb,  component, row);
  }
}





} //# NAMESPACE CASA - END

