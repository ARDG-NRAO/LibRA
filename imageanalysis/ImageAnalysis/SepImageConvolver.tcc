//# SepImageConvolver.cc:  separable convolution of an image
//# Copyright (C) 1995,1996,1997,1998,1999,2000,2001,2002,2003
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
//# $Id: SepImageConvolver.tcc 20615 2009-06-09 02:16:01Z Malte.Marquarding $
//   

#include <imageanalysis/ImageAnalysis/SepImageConvolver.h>

#include <casacore/casa/aips.h>

#include <casacore/casa/Arrays/Vector.h>
#include <casacore/casa/Arrays/ArrayMath.h>
#include <casacore/casa/Containers/Block.h>
#include <casacore/casa/Exceptions/Error.h>
#include <casacore/casa/Logging/LogIO.h>
#include <casacore/scimath/Mathematics/Convolver.h>
#include <casacore/casa/Quanta/UnitMap.h>
#include <casacore/casa/Quanta/Quantum.h>
#include <casacore/casa/BasicSL/String.h>

#include <casacore/coordinates/Coordinates/CoordinateSystem.h>
#include <casacore/images/Images/PagedImage.h>
#include <casacore/images/Regions/ImageRegion.h>
#include <casacore/images/Images/SubImage.h>
#include <casacore/images/Images/TempImage.h>
#include <casacore/lattices/Lattices/LatticeIterator.h>
#include <casacore/lattices/Lattices/LatticeUtilities.h>
#include <casacore/lattices/Lattices/TiledLineStepper.h>
#include <casacore/scimath/Mathematics/VectorKernel.h>
#include <casacore/casa/System/ProgressMeter.h>

#include <sstream>
#include <casa/iomanip.h>


namespace casa { //# NAMESPACE CASA - BEGIN

template <class T> 
SepImageConvolver<T>::SepImageConvolver(
	const casacore::ImageInterface<T>& image, casacore::LogIO &os, casacore::Bool showProgress
)
: itsImagePtr(image.cloneII()),
  itsOs(os),
  itsAxes(0),
  itsVectorKernels(0),
  itsShowProgress(showProgress) {}

template <class T>
SepImageConvolver<T>::SepImageConvolver(const SepImageConvolver<T> &other)
: itsImagePtr(0) {
   operator=(other);
}


template <class T> 
SepImageConvolver<T>::~SepImageConvolver () {
   delete itsImagePtr;
   itsImagePtr = 0;
   const casacore::uInt n = itsVectorKernels.nelements();
   for (casacore::uInt i=0; i<n; i++) {
      delete itsVectorKernels[i];
      itsVectorKernels[i] = 0;
   }
}

template <class T>
SepImageConvolver<T> &SepImageConvolver<T>::operator=(
	const SepImageConvolver<T> &other
) {
   if (this != &other) {
      if (itsImagePtr!=0) delete itsImagePtr;
      itsImagePtr = other.itsImagePtr->cloneII();

      itsOs = other.itsOs;
      itsAxes = other.itsAxes.copy();
      itsShowProgress = other.itsShowProgress;

      const casacore::uInt n = itsVectorKernels.nelements();
      for (casacore::uInt i=0; i<n; i++) {
         delete itsVectorKernels[i];
         itsVectorKernels[i] = 0;
      }

      const casacore::uInt n2 = other.itsVectorKernels.nelements();
      itsVectorKernels.resize(n2);
      for (casacore::uInt i=0; i<n2; i++) {      
         itsVectorKernels[i] = new casacore::Vector<T>((other.itsVectorKernels[i])->copy());
      }
   }
   return *this;
}



template <class T>
void SepImageConvolver<T>::setKernel(
	casacore::uInt axis, const casacore::Vector<T>& kernel
) {
   _checkAxis(axis);

   casacore::uInt n = itsVectorKernels.nelements() + 1;
   itsVectorKernels.resize(n, true);
   itsVectorKernels[n-1] = new casacore::Vector<T>(kernel.copy());
   itsAxes.resize(n, true);
   itsAxes(n-1) = axis;
}

template <class T>
void SepImageConvolver<T>::setKernel(
	casacore::uInt axis, casacore::VectorKernel::KernelTypes kernelType,
    const casacore::Quantum<casacore::Double>& width, casacore::Bool autoScale,
    casacore::Bool useImageShapeExactly, casacore::Double scale
) {
	// Catch pixel units

   casacore::UnitMap::putUser("pix",casacore::UnitVal(1.0), "pixel units");
   casacore::String sunit = width.getFullUnit().getName();
   if (sunit==casacore::String("pix")) {
      setKernel (axis, kernelType, width.getValue(), autoScale, useImageShapeExactly, scale);
      itsOs.output() << "Axis " << axis+1<< " : setting width "  << width << endl;
      itsOs << casacore::LogIO::NORMAL;
      return;
   }
   _checkAxis(axis);

   // Convert width to pixels

   casacore::CoordinateSystem cSys = itsImagePtr->coordinates();
   casacore::Int worldAxis = cSys.pixelAxisToWorldAxis(axis);
   casacore::Double inc = cSys.increment()(worldAxis);

   casacore::Unit unit = casacore::Unit(cSys.worldAxisUnits()(worldAxis));
   if (width.getFullUnit()!=unit) {
      itsOs << "Specified width units (" << width.getUnit() 
            << ") are inconsistent with image axis unit (" 
            << unit.getName() << casacore::LogIO::EXCEPTION;
   }
   casacore::Double width2 = abs(width.getValue(unit)/inc);

   itsOs.output() << "Axis " << axis+1<< " : setting width "
         << width << " = " << width2 << " pixels" << endl;
   itsOs <<  casacore::LogIO::NORMAL;
   setKernel(axis, kernelType, width2, autoScale, useImageShapeExactly, scale);
}

template <class T>
void SepImageConvolver<T>::setKernel(
	casacore::uInt axis, casacore::VectorKernel::KernelTypes kernelType,
    casacore::Double width, casacore::Bool autoScale,
    casacore::Bool useImageShapeExactly, casacore::Double scale
) {
   _checkAxis(axis);

   // T can only be casacore::Float or Double

   casacore::Bool peakIsUnity = !autoScale;
   casacore::uInt shape = itsImagePtr->shape()(axis);
   casacore::Vector<T> x = casacore::VectorKernel::make(kernelType, T(width), shape, 
                                    useImageShapeExactly, peakIsUnity);
   if (!autoScale && !casacore::near(scale,1.0)) x *= casacore::Float(scale);
   casacore::uInt n = itsVectorKernels.nelements() + 1;
   itsVectorKernels.resize(n, true);
   itsVectorKernels[n-1] = new casacore::Vector<T>(x.copy());

   itsAxes.resize(n, true);
   itsAxes(n-1) = axis;
}


template <class T>
Vector<T> SepImageConvolver<T>::getKernel(casacore::uInt axis) {
   for (casacore::uInt i=0; i<itsAxes.nelements(); i++) {
      if (axis==itsAxes(i)) {
         return *(itsVectorKernels[i]);
      }
   }
   itsOs << "There is no kernel for the specified axis" << casacore::LogIO::EXCEPTION;
   return casacore::Vector<T>(0);
}

template <class T>
uInt SepImageConvolver<T>::getKernelShape(casacore::uInt axis) {
   for (casacore::uInt i=0; i<itsAxes.nelements(); i++) {
      if (axis==itsAxes(i)) {
         return itsVectorKernels[i]->nelements();
      }
   }
   itsOs << "There is no kernel for the specified axis" << casacore::LogIO::EXCEPTION;
   return 0;
}

template <class T>
void SepImageConvolver<T>::convolve(casacore::ImageInterface<T>& imageOut) {
   const casacore::uInt nAxes = itsAxes.nelements();
   if (nAxes==0) {
      itsOs << "You haven't specified any axes to convolve" << casacore::LogIO::EXCEPTION;
   }

   // Some checks

   casacore::IPosition shape = itsImagePtr->shape();
   if (!shape.isEqual(imageOut.shape())) {
      itsOs << "Image shapes are different" << casacore::LogIO::EXCEPTION;
   }
   casacore::CoordinateSystem cSys = itsImagePtr->coordinates();
   if (!cSys.near(imageOut.coordinates())) {
      itsOs << casacore::LogIO::WARN << "Image CoordinateSystems differ - this may be unwise"
            << casacore::LogIO::POST;
   }

   // Give the output image a mask if needed and make it the default

   if (itsImagePtr->isMasked() && !imageOut.isMasked()) {
      if (imageOut.canDefineRegion()) {
         casacore::String maskName = imageOut.makeUniqueRegionName (casacore::String("mask"), 0);
         imageOut.makeMask(maskName, true, true);
         itsOs << casacore::LogIO::NORMAL << "Created mask " << maskName 
               << " and make it the default" << casacore::LogIO::POST;
      } else {
         itsOs << casacore::LogIO::NORMAL << "Cannot create a mask for this output image" << casacore::LogIO::POST;
      }
   }

   // First copy input to output. We must replace masked pixels by zeros.  These reflect
   // both the pixel mask and the region mask.  We also set the output mask to the input mask
 
   casacore::LatticeUtilities::copyDataAndMask(itsOs, imageOut, *itsImagePtr, true);

   // casacore::Smooth in situ.
      
   casacore::IPosition niceShape = imageOut.niceCursorShape();
   casacore::uInt axis = 0;
   for (casacore::uInt i=0; i<nAxes; i++) {
      axis = itsAxes(i);
      itsOs << casacore::LogIO::NORMAL << "Convolving axis " << axis+1 << casacore::LogIO::POST;
      const casacore::Int n = shape(axis)/niceShape(axis);
      if (n*niceShape(axis)!=shape(axis)) {
         itsOs << casacore::LogIO::WARN 
               << "The tile shape is not integral along this axis, performance may degrade" 
               << casacore::LogIO::POST;
      }
      _smoothProfiles (imageOut, axis, *(itsVectorKernels[i]));
   }  
}
 
template <class T>
void SepImageConvolver<T>::_zero() {
   if (itsImagePtr->isMasked()) {
      itsOs << casacore::LogIO::NORMAL << "Zero masked pixels" << casacore::LogIO::POST;

      casacore::LatticeIterator<T> iter(*itsImagePtr);
      casacore::Bool deleteData, deleteMask;
      casacore::IPosition shape = iter.rwCursor().shape();
      casacore::Array<T> data(shape);
      casacore::Array<casacore::Bool> mask(shape);

      for (iter.reset(); !iter.atEnd(); iter++) {
         shape = iter.rwCursor().shape();
         if (!data.shape().isEqual(shape)) data.resize(shape);
         if (!mask.shape().isEqual(shape)) mask.resize(shape);

         itsImagePtr->getSlice(data, iter.position(), shape);
         itsImagePtr->getMaskSlice(mask, iter.position(), shape);

         T* pData = data.getStorage(deleteData);
         const casacore::Bool* pMask = mask.getStorage(deleteMask);

         for (casacore::Int i=0; i<shape.product(); i++) {
            if (!pMask[i]) pData[i] = 0.0;
         }

         data.putStorage(pData, deleteData);
         mask.freeStorage(pMask, deleteMask);
      }
   }     
}        
 
template <class T>
void SepImageConvolver<T>::_smoothProfiles (
	casacore::ImageInterface<T>& in, const casacore::Int& axis,
	const casacore::Vector<T>& psf
) {
  casacore::ProgressMeter* pProgressMeter = 0;
  if (itsShowProgress) {
     casacore::Double nMin = 0.0;
     casacore::Double nMax = 1.0;
     for (casacore::Int i=0; i<casacore::Int(in.shape().nelements()); i++) {
        if (i!=axis) {
           nMax *= in.shape()(i);
        }
     }
     ostringstream oss;
     oss << "Convolve Image Axis " << axis+1;
     pProgressMeter = new casacore::ProgressMeter(nMin, nMax, casacore::String(oss),
                                        casacore::String("Spectrum Convolutions"),
                                        casacore::String(""), casacore::String(""),
                                        true, max(1,casacore::Int(nMax/20)));   
  }

  casacore::TiledLineStepper navIn(in.shape(),
                         in.niceCursorShape(),
                         axis);
  casacore::LatticeIterator<T> inIt(in, navIn);
  casacore::Vector<T> result(in.shape()(axis));
  casacore::IPosition shape(1, in.shape()(axis));
  casacore::Convolver<T> conv(psf, shape);

  casacore::uInt i = 0;
  while (!inIt.atEnd()) {
    conv.linearConv(result, inIt.vectorCursor());
    inIt.woVectorCursor() = result;

    if (itsShowProgress) pProgressMeter->update(casacore::Double(i));
    inIt++;
    i++;
  }
  if (itsShowProgress) delete pProgressMeter;
}

template <class T>
void SepImageConvolver<T>::_checkAxis(casacore::uInt axis) {
   if (axis>itsImagePtr->ndim()-1) {
      itsOs << "Given pixel axis " << axis 
            << " is greater than the number of axes in the image" << casacore::LogIO::EXCEPTION;
   }
   const casacore::uInt n = itsAxes.nelements();
   for (casacore::uInt i=0; i<n; i++) {
      if (axis==itsAxes(i)) {
         itsOs << "You have already given this axis to be convolved" << casacore::LogIO::EXCEPTION;
      }
   }
}

} //# NAMESPACE CASA - END

