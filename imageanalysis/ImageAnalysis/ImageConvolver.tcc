//# ImageConvolver.cc:  convolution of an image by given Array
//# Copyright (C) 1995,1996,1997,1998,1999,2000,2001,2002
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
//# $Id: ImageConvolver.tcc 20615 2009-06-09 02:16:01Z Malte.Marquarding $
//   
#include <imageanalysis/ImageAnalysis/ImageConvolver.h>
//
#include <casa/aips.h>
#include <coordinates/Coordinates/CoordinateUtil.h>
#include <casa/Exceptions/Error.h>
#include <casa/Logging/LogIO.h>
#include <casa/BasicMath/Math.h>
#include <casa/BasicSL/String.h>
#include <images/Images/PagedImage.h>
#include <images/Images/TempImage.h>
#include <images/Regions/RegionHandler.h>
#include <images/Regions/ImageRegion.h>
#include <images/Images/ImageUtilities.h>
#include <lattices/Lattices/ArrayLattice.h>
#include <lattices/LatticeMath/LatticeConvolver.h>
#include <lattices/Lattices/LatticeUtilities.h>
#include <lattices/LEL/LatticeExpr.h>
#include <lattices/LEL/LatticeExprNode.h>
#include <casa/iostream.h>

#include <memory>


namespace casa { //# NAMESPACE CASA - BEGIN

template <class T> 
ImageConvolver<T>::ImageConvolver ()
{}

template <class T>
ImageConvolver<T>::ImageConvolver(const ImageConvolver<T> &other)
{
   operator=(other);
}

template <class T> 
ImageConvolver<T>::~ImageConvolver ()
{}


template <class T>
ImageConvolver<T> &ImageConvolver<T>::operator=(const ImageConvolver<T> &other)
{

// There is no state

   if (this != &other) {
   }
   return *this;
}


template <class T>
void ImageConvolver<T>::convolve(casacore::LogIO& os,  
                                 casacore::ImageInterface<T>& imageOut,
                                 const casacore::ImageInterface<T>& imageIn,
                                 const casacore::ImageInterface<T>& kernel,
                                 ScaleTypes scaleType, casacore::Double scale,
                                 casacore::Bool copyMiscellaneous, casacore::Bool warnOnly)
{
// Check Coordinates
    checkCoordinates (os, imageIn.coordinates(), kernel.coordinates(),
                      warnOnly);

// Convolve

    convolve (os, imageOut, imageIn, kernel,
              scaleType, scale, copyMiscellaneous);
}

template <class T>
void ImageConvolver<T>::convolve(casacore::LogIO& os,  
                                 casacore::ImageInterface<T>& imageOut,
                                 const casacore::ImageInterface<T>& imageIn,
                                 const casacore::Array<T>& kernel,
                                 ScaleTypes scaleType, casacore::Double scale,
                                 casacore::Bool copyMiscellaneous)
{
    casacore::ArrayLattice<T> kernelLattice(kernel);
    convolve (os, imageOut, imageIn, kernelLattice, 
              scaleType, scale, copyMiscellaneous);
}


template <class T>
void ImageConvolver<T>::convolve(casacore::LogIO& os,  
                                 casacore::ImageInterface<T>& imageOut,
                                 const casacore::ImageInterface<T>& imageIn,
                                 const casacore::Lattice<T>& kernel,
                                 ScaleTypes scaleType, casacore::Double scale,
                                 casacore::Bool copyMiscellaneous)
{

// Check
   const casacore::IPosition& inShape = imageIn.shape();
   const casacore::IPosition& outShape = imageOut.shape();
   if (!inShape.isEqual(outShape)) {
      os << "Input and output images must have same shape" << casacore::LogIO::EXCEPTION;
   }
    if (kernel.ndim() > imageIn.ndim()) {
        os << "Kernel lattice has more axes than the image!" << casacore::LogIO::EXCEPTION;
    }

// Add degenerate axes if needed

    casacore::Lattice<T>* pNewKernel = 0;
    casacore::LatticeUtilities::addDegenerateAxes (pNewKernel, kernel, inShape.nelements());
    std::unique_ptr<casacore::Lattice<T> > pnkMgr(pNewKernel);
// Normalize kernel.  
  
    casacore::LatticeExprNode node;
    if (scaleType==AUTOSCALE) {
       node = casacore::LatticeExprNode((*pNewKernel) / sum(*pNewKernel));
    } else if (scaleType==SCALE) {
       T t = static_cast<T>(scale);
       node = casacore::LatticeExprNode(t * (*pNewKernel));
    } else if (scaleType==NONE) {
       node = casacore::LatticeExprNode(*pNewKernel);
    }
    casacore::LatticeExpr<T> kernelExpr(node);
// Create convolver

    casacore::LatticeConvolver<T> lc(kernelExpr, imageIn.shape(),  casacore::ConvEnums::LINEAR);
//
    if (imageIn.isMasked()) {

// Generate output mask if needed

      makeMask(imageOut, os);

// Copy input mask to output.  Copy input pixels
// and set to zero where masked

      casacore::LatticeUtilities::copyDataAndMask(os, imageOut, imageIn, true);
// Convolve in situ

      lc.convolve(imageOut);
    } else {

// Convolve to output

      lc.convolve(imageOut, imageIn);
    }

// Overwrite output casacore::CoordinateSystem 
   imageOut.setCoordinateInfo (imageIn.coordinates());
// Copy miscellaneous things across as required

    if (copyMiscellaneous) casacore::ImageUtilities::copyMiscellaneous(imageOut, imageIn);
// Delete the restoring beam (should really check that the beam is in the
// plane of convolution)
    casacore::ImageInfo ii = imageOut.imageInfo();
    ii.removeRestoringBeam();
    imageOut.setImageInfo (ii);
}

template <class T>
void ImageConvolver<T>::makeMask(casacore::ImageInterface<T>& out, casacore::LogIO& os)  const
{
   if (out.canDefineRegion()) {   
    
// Generate mask name 
      
      casacore::String maskName = out.makeUniqueRegionName(casacore::String("mask"), 0);
    
// Make the mask if it does not exist
      
      if (!out.hasRegion (maskName, casacore::RegionHandler::Masks)) {
         out.makeMask(maskName, true, true, false, true);
         os << casacore::LogIO::NORMAL << "Created mask `" << maskName << "'" << casacore::LogIO::POST;
      }
   } else {
      os << casacore::LogIO::WARN << "Cannot make requested mask for this type of image" << endl;
   }
}


template <class T>
void ImageConvolver<T>::checkCoordinates (casacore::LogIO& os, const casacore::CoordinateSystem& cSysImage,
                                          const casacore::CoordinateSystem& cSysKernel,
                                          casacore::Bool warnOnly) const
{
   const casacore::uInt nPixelAxesK = cSysKernel.nPixelAxes();
   const casacore::uInt nPixelAxesI = cSysImage.nPixelAxes();
   if (nPixelAxesK > nPixelAxesI) {
        os << "Kernel has more pixel axes than the image" << casacore::LogIO::EXCEPTION;
    }
//
   const casacore::uInt nWorldAxesK = cSysKernel.nWorldAxes();
   const casacore::uInt nWorldAxesI = cSysImage.nWorldAxes();
   if (nWorldAxesK > nWorldAxesI) {
        os << "Kernel has more world axes than the image" << casacore::LogIO::EXCEPTION;
    }
//
   const casacore::Vector<casacore::Double>& incrI = cSysImage.increment();
   const casacore::Vector<casacore::Double>& incrK = cSysKernel.increment();
   const casacore::Vector<casacore::String>& unitI = cSysImage.worldAxisUnits();
   const casacore::Vector<casacore::String>& unitK = cSysKernel.worldAxisUnits();
//
   for (casacore::uInt i=0; i<nWorldAxesK; i++) {

// Compare casacore::Coordinate types and reference

      if (casacore::CoordinateUtil::findWorldAxis(cSysImage, i) != 
          casacore::CoordinateUtil::findWorldAxis(cSysKernel, i)) {
         if (warnOnly) {
            os << casacore::LogIO::WARN << "Coordinate types are not the same for axis " << i+1 << casacore::LogIO::POST;
         } else {
            os << "Coordinate types are not the same for axis " << i+1 << casacore::LogIO::EXCEPTION;
         }
      }

// Compare units 

      casacore::Unit u1(unitI[i]);
      casacore::Unit u2(unitK[i]);
      if (u1 != u2) {
         if (warnOnly) {
            os << casacore::LogIO::WARN << "Axis units are not consistent for axis " << i+1 << casacore::LogIO::POST;
         } else {
            os << "Axis units are not consistent for axis " << i+1 << casacore::LogIO::EXCEPTION;
         }
      }

// Compare increments ; this is not a very correct test as there may be
// values in the LinearTransform inside the coordinate.  Should really
// convert some values...  See how we go.

      casacore::Quantum<casacore::Double> q2(incrK[i], u2);
      casacore::Double val2 = q2.getValue(u1);
      if (!casacore::near(incrI[i], val2, 1.0e-6)) {
         if (warnOnly) {
            os << casacore::LogIO::WARN << "Axis increments are not consistent for axis " << i+1 << casacore::LogIO::POST;
         } else {
            os << "Axis increments are not consistent for axis " << i+1 << casacore::LogIO::EXCEPTION;
         } 
     }
   }
}

} //# NAMESPACE CASA - END

