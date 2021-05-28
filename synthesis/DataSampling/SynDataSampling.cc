//# SynDataSampling.cc: Implementation of SynDataSampling class
//# Copyright (C) 1997,1998,1999,2000,2001,2003
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

#include <synthesis/DataSampling/SynDataSampling.h>
#include <coordinates/Coordinates/CoordinateSystem.h>
#include <coordinates/Coordinates/DirectionCoordinate.h>
#include <ms/MeasurementSets/MSColumns.h>
#include <casa/BasicSL/Constants.h>
#include <synthesis/TransformMachines/SkyJones.h>
#include <msvis/MSVis/VisSet.h>
#include <msvis/MSVis/VisBuffer.h>
#include <msvis/MSVis/VisibilityIterator.h>
#include <images/Images/ImageInterface.h>
#include <images/Images/PagedImage.h>
#include <images/Images/TempImage.h>
#include <casa/Arrays/ArrayLogical.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/MaskedArray.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/Vector.h>
#include <casa/Arrays/Matrix.h>
#include <casa/BasicSL/String.h>
#include <casa/Utilities/Assert.h>
#include <casa/Exceptions/Error.h>
#include <casa/System/ProgressMeter.h>
#include <casa/sstream.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

SynDataSampling::SynDataSampling(MeasurementSet& ms,
				 const CoordinateSystem& coords,
				 const IPosition& shape,
				 const Quantity& sigma)
{

  LogIO os(LogOrigin("SynDataSampling", "SynDataSampling"));

  DataSampling::IDLScript_p="@app_syn";

  // Now create the VisSet
  Block<int> sort(4);
  sort[0] = MS::FIELD_ID;
  sort[1] = MS::ARRAY_ID;
  sort[2] = MS::DATA_DESC_ID;
  sort[3] = MS::TIME;
  
  Matrix<Int> noselection;
  VisSet vs(ms,sort,noselection);
  
  // First get the CoordinateSystem for the image and then find
  // the DirectionCoordinate
  Int directionIndex=coords.findCoordinate(Coordinate::DIRECTION);
  AlwaysAssert(directionIndex>=0, AipsError);
  DirectionCoordinate directionCoord=coords.directionCoordinate(directionIndex);

  dx_p.resize(IPosition(2, 2, 2*ms.nrow())); dx_p=-1.0;
  data_p.resize(IPosition(1, 2*ms.nrow()));  data_p=0.0;
  sigma_p.resize(IPosition(1, 2*ms.nrow())); sigma_p=-1.0;
  Int lastRow = 0;

  VisIter& vi=vs.iter();
  VisBuffer vb(vi);
  vi.originChunks();
  vi.origin();
 
  Int nx = shape(0);
  Int ny = shape(1);

  Vector<Float> uvScale, uvOffset;
  uvScale(0)=(Float(nx)*coords.increment()(0)); 
  uvScale(1)=(Float(ny)*coords.increment()(1)); 
  uvOffset(0)=nx/2;
  uvOffset(1)=ny/2;

  // Now fill in the data and position columns
  ProgressMeter pm(1.0, Double(ms.nrow()), "Sampling Data", "", "", "", true);

  // Loop over all visibilities
  Int cohDone = 0;
  Float sigmaVal=sigma.getValue();
  for (vi.originChunks();vi.moreChunks();vi.nextChunk()) {
    for (vi.origin(); vi.more(); vi++) {

      Matrix<Double> uvw(3, vb.uvw().nelements());
      uvw=0.0;
      Vector<Double> dphase(vb.uvw().nelements());
      dphase=0.0;
      for (Int i=0;i<=vb.nRow();i++) {
	for (Int idim=0;idim<3;idim++) uvw(idim,i)=vb.uvw()(i)(idim);
      }
      //      rotateUVW(uvw, dphase, vb);

      Vector<Double> worldPos(2);
      Vector<Double> xyPos(2);

      for (Int row=0;row<vb.nRow();row++) {

	IPosition irow(1, lastRow);
	IPosition irow1(1, lastRow+1);
	data_p(irow)  = real(vb.correctedVisCube()(0,0,row));
	data_p(irow1) = imag(vb.correctedVisCube()(0,0,row));

	worldPos(0)=uvw(IPosition(2, 0, row));
	worldPos(1)=uvw(IPosition(2, 1, row));

	for (Int i=0;i<2;i++) {
	  xyPos(i) = worldPos(i)*uvScale(i) + uvOffset(i);
	}

	dx_p(IPosition(2, 0, lastRow)) = Float(xyPos(0));
	dx_p(IPosition(2, 1, lastRow)) = Float(xyPos(1));
	dx_p(IPosition(2, 0, lastRow+1)) = Float(xyPos(0));
	dx_p(IPosition(2, 1, lastRow+1)) = Float(xyPos(1));
	if(sigmaVal>0.0) {
	  sigma_p(irow)  = sigmaVal;
	  sigma_p(irow1) = sigmaVal;
	}
	else {
	  sigma_p(irow)  = vb.sigma()(row);
	  sigma_p(irow1) = vb.sigma()(row);
	}
	lastRow+=2;
      }
      cohDone+=vb.nRow();
      pm.update(Double(cohDone));
    }
  }
}

//---------------------------------------------------------------------- 
SynDataSampling& SynDataSampling::operator=(const SynDataSampling& other)
{
  if(this!=&other) {
  };
  return *this;
};

//----------------------------------------------------------------------
SynDataSampling::SynDataSampling(const SynDataSampling& other):
    DataSampling(other)
{
  operator=(other);
}

//----------------------------------------------------------------------
SynDataSampling::~SynDataSampling() {
}

void SynDataSampling::ok() {
}

} //# NAMESPACE CASA - END

