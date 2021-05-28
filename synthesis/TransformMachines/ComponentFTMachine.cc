//# FTMachine.cc: Implementation of FTMachine class
//# Copyright (C) 1997,1998,2000,2001
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

#include <msvis/MSVis/VisibilityIterator.h>
#include <msvis/MSVis/VisBuffer.h>
#include <casa/Quanta/Euler.h>
#include <casa/Quanta/RotMatrix.h>
#include <measures/Measures/MDirection.h>
#include <ms/MeasurementSets/MSColumns.h>
#include <casa/BasicSL/Constants.h>
#include <synthesis/TransformMachines/ComponentFTMachine.h>
#include <msvis/MSVis/VisBuffer.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/MatrixMath.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/Vector.h>
#include <casa/Arrays/Matrix.h>
#include <casa/BasicSL/String.h>
#include <casa/Utilities/Assert.h>
#include <casa/Exceptions/Error.h>
#include <measures/Measures/UVWMachine.h>
#include <ms/MeasurementSets/MSColumns.h>
#include <casa/OS/Timer.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

//---------------------------------------------------------------------- 
  ComponentFTMachine::ComponentFTMachine() : numthreads_p(-1), phaseCenterTime_p(-1.0)
{
}

LogIO& ComponentFTMachine::logIO() {return logIO_p;};

//---------------------------------------------------------------------- 
ComponentFTMachine&
ComponentFTMachine::operator=(const ComponentFTMachine& other)
{
  if(this!=&other) {
  };
  return *this;
};

//----------------------------------------------------------------------
ComponentFTMachine::ComponentFTMachine(const ComponentFTMachine& other)
{
  operator=(other);
}

//----------------------------------------------------------------------
ComponentFTMachine::~ComponentFTMachine() {
}

void ComponentFTMachine::setnumthreads(const Int numthreads){
    numthreads_p=numthreads;
}

void ComponentFTMachine::rotateUVW(Matrix<Double>& uvw, Vector<Double>& dphase,
				   const VisBuffer& vb,
				   const MDirection& mDesired)
{

  // 1. We need to convert this type (e.g. MDirection::SUN to the
  // type observed (e.g. MDirection::J2000). So first we get
  // the type and combine it with the specified frame that includes
  // the observing epoch. So we end up with e.g. the position of
  // the Sun on the specified epoch, expressed in J2000 coordinates.
  // We also add an MPosition as well so that for compact arrays we
  // can specify images in e.g. AZEL or HADEC.
  

  MeasFrame mFrame((MEpoch(Quantity(vb.time()(0), 
				    vb.msColumns().timeQuant()(0).getUnit() ), 
			   vb.msColumns().timeMeas()(0).getRef())), 
		   vb.msColumns().antenna().positionMeas()(0));

  UVWMachine uvwMachine(mDesired, vb.phaseCenter(phaseCenterTime_p), mFrame,
			false, true);

  // Now we convert all rows and also calculate the
  // change in phase
  uInt nrows=dphase.nelements();
  Vector<Double> thisRow(3);
  thisRow=0.0;
  uInt i;
  for (uInt row=0;row<nrows;row++) {
    for (i=0;i<3;i++) thisRow(i)=uvw(i,row);
    uvwMachine.convertUVW(dphase(row), thisRow);
    for (i=0;i<3;i++) uvw(i,row)=thisRow(i);
  }

}

void ComponentFTMachine::rotateUVW(Double*& uvw, Double*& dphase, const Int nrows, 
				   const VisBuffer& vb,
				   const MDirection& mDesired)
{

  // 1. We need to convert this type (e.g. MDirection::SUN to the
  // type observed (e.g. MDirection::J2000). So first we get
  // the type and combine it with the specified frame that includes
  // the observing epoch. So we end up with e.g. the position of
  // the Sun on the specified epoch, expressed in J2000 coordinates.
  // We also add an MPosition as well so that for compact arrays we
  // can specify images in e.g. AZEL or HADEC.
  MeasFrame mFrame((MEpoch(Quantity(vb.time()(0), vb.msColumns().timeQuant()(0).getUnit()), vb.msColumns().timeMeas()(0).getRef())), vb.msColumns().antenna().positionMeas()(0));


  UVWMachine uvwMachine(mDesired, vb.phaseCenter(phaseCenterTime_p), mFrame,
			false, true);

  // Now we convert all rows and also calculate the
  // change in phase
  Vector<Double> thisRow(3);
  thisRow=0.0;
  uInt i;
  for (Int row=0;row<nrows;row++) {
    for (i=0;i<3;i++) thisRow(i)=uvw[row*3+i];
    uvwMachine.convertUVW(dphase[row], thisRow);
    for (i=0;i<3;i++) uvw[row*3+i]=thisRow(i);
  }

}

void ComponentFTMachine::ok() {
}






} //# NAMESPACE CASA - END

