//# PBMath2D.cc: Implementation for PBMath2D
//# Copyright (C) 1996,1997,1998,1999,2000,2001,2002,2003
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
//# Correspondence concerning AIPS++ should be adressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//#
//# $Id$
 
#include <casacore/casa/aips.h>
#include <casacore/casa/BasicSL/Complex.h>
#include <casacore/casa/Arrays/Matrix.h>
#include <casacore/casa/Arrays/Vector.h>
#include <synthesis/TransformMachines/PBMath2D.h>

#include <casacore/images/Regions/ImageRegion.h>
#include <casacore/images/Images/ImageInterface.h>

#include <components/ComponentModels/SkyComponent.h>
#include <components/ComponentModels/Flux.h>
#include <components/ComponentModels/ComponentShape.h>

#include <casacore/lattices/Lattices/LatticeIterator.h>
#include <casacore/lattices/Lattices/LatticeStepper.h>
#include <casacore/lattices/LRegions/LCSlicer.h>
#include <casacore/casa/Arrays/IPosition.h>

#include <casacore/measures/Measures.h>
#include <casacore/measures/Measures/MeasConvert.h>

#include <casacore/coordinates/Coordinates/CoordinateSystem.h>
#include <casacore/coordinates/Coordinates/DirectionCoordinate.h>
#include <casacore/coordinates/Coordinates/SpectralCoordinate.h>
#include <casacore/coordinates/Coordinates/StokesCoordinate.h>
#include <casacore/coordinates/Coordinates/Projection.h>
#include <casacore/coordinates/Coordinates/CoordinateUtil.h>

#include <casacore/casa/BasicSL/String.h>
#include <casacore/casa/Utilities/Assert.h>
#include <casacore/casa/Exceptions/Error.h>



using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

PBMath2D::PBMath2D()
{
};


PBMath2D::~PBMath2D()
{
};


void PBMath2D::summary(Int nValues)
{
  String  name;
  namePBClass(name);
  LogIO os(LogOrigin("PBMath2D", "summary"));
  os << "Using " << name << " PB Class " <<  LogIO::POST;
  PBMathInterface::summary(nValues);

  os << "PBMath2D::summary not yet implemented" << LogIO::POST; 

};


Bool PBMath2D::ok()
{
  return true;
};



ImageRegion* 
PBMath2D::extent (const ImageInterface<Complex>& /*in*/, const MDirection& /*pointDir*/,
		  const Int row, const Float /*fPad*/,  const Int /*iChan*/,
		  const SkyJones::SizeType /*sizeType*/)
{
  if (row) {} // Not used yet

  return  ( new ImageRegion() );
};

ImageRegion* 
PBMath2D::extent (const ImageInterface<Float>& /*in*/, const MDirection& /*pointDir*/,
                  const Int row, const Float /*fPad*/, const Int /*iChan*/,
		  const SkyJones::SizeType /*sizeType*/)
{
  if (row) {} // unused
  return  ( new ImageRegion() );
};


Int PBMath2D::support(const CoordinateSystem& /*cs*/){

  return 0;

}


} //# NAMESPACE CASA - END

