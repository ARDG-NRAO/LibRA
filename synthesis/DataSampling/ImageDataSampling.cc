//# ImageDataSampling.cc: Implementation of ImageDataSampling class
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

#include <synthesis/DataSampling/ImageDataSampling.h>
#include <casa/BasicSL/Constants.h>
#include <images/Images/ImageInterface.h>
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
#include <casa/sstream.h>

using namespace casacore;
namespace casa { //# NAMESPACE CASA - BEGIN

ImageDataSampling::ImageDataSampling(const ImageInterface<Float>& dirty,
				     const ImageInterface<Float>& psf,
				     const ImageInterface<Float>& sigma)
{
  ImageDataSampling::init(dirty, psf, &sigma, 0.0);
  DataSampling::IDLScript_p="@app_image";
}

ImageDataSampling::ImageDataSampling(const ImageInterface<Float>& dirty,
				     const ImageInterface<Float>& psf,
				     const Float sigma)
{
  ImageInterface<Float>* sigmaImagePtr=0;
  ImageDataSampling::init(dirty, psf, sigmaImagePtr, sigma);
  DataSampling::IDLScript_p="@app_image";
}

//---------------------------------------------------------------------- 
ImageDataSampling& ImageDataSampling::operator=(const ImageDataSampling& other)
{
  if(this!=&other) {
  };
  return *this;
};

//----------------------------------------------------------------------
ImageDataSampling::ImageDataSampling(const ImageDataSampling& other):
  DataSampling(other)
{
  operator=(other);
}

//----------------------------------------------------------------------
ImageDataSampling::~ImageDataSampling() {
}

void ImageDataSampling::init(const ImageInterface<Float>& dirty,
			     const ImageInterface<Float>& psf,
			     const ImageInterface<Float>* sigmaImagePtr,
			     const Float sigma)
{

  ok();

  Int nx = dirty.shape()(0);
  Int ny = dirty.shape()(1);

  dx_p.resize(IPosition(2, 2, nx*ny)); dx_p=0.0;
  data_p.resize(IPosition(2, nx, ny));  data_p=0.0;
  sigma_p.resize(IPosition(2, nx, ny)); sigma_p=sigma;
  prf_p.resize(IPosition(2, nx, ny)); prf_p=0.0;

  // Fill in data, dx, and prf
  IPosition zero(dirty.shape().nelements(), 0);
  IPosition onePlane(dirty.shape().nelements(), 1);
  onePlane(0)=nx;
  onePlane(1)=ny;
  data_p=dirty.getSlice(zero, onePlane, true);
  prf_p=psf.getSlice(zero, onePlane, true);
  if(sigmaImagePtr) {
    sigma_p=sigmaImagePtr->getSlice(zero, onePlane, true);
  }

  Int row=0;
  for (Int iy=0;iy<ny;iy++) {
    for (Int ix=0;ix<nx;ix++) {
      dx_p(IPosition(2, 0, row))=Float(ix);
      dx_p(IPosition(2, 1, row))=Float(iy);
      row++;
    }
  }
}
	
void ImageDataSampling::ok() {
}

} //# NAMESPACE CASA - END

