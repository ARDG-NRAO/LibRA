//# ImageTwoPtCorr.cc: Compute the two point correlation function of an image
//# Copyright (C) 1995,1997,1998,1999,2000,2001,2002,2003
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
//# $Id: ImageTwoPtCorr.tcc 19940 2007-02-27 05:35:22Z Malte.Marquarding $


#include <imageanalysis/ImageAnalysis/ImageTwoPtCorr.h>

#include <casacore/casa/Exceptions/Error.h>
#include <casacore/casa/Logging/LogIO.h>
#include <casacore/casa/Utilities/Assert.h>
#include <casacore/coordinates/Coordinates/CoordinateSystem.h>
#include <casacore/coordinates/Coordinates/CoordinateUtil.h>
#include <casacore/coordinates/Coordinates/DirectionCoordinate.h>
#include <casacore/coordinates/Coordinates/LinearCoordinate.h>
#include <casacore/images/Images/ImageInterface.h>
#include <casacore/images/Images/ImageInfo.h>
#include <casacore/lattices/LatticeMath/LatticeTwoPtCorr.h>

#include <iostream>

namespace casa { //# NAMESPACE CASA - BEGIN

template <class T> 
ImageTwoPtCorr<T>::ImageTwoPtCorr()
{}


template <class T> 
ImageTwoPtCorr<T>::~ImageTwoPtCorr()
{}

template <class T> 
ImageTwoPtCorr<T>& ImageTwoPtCorr<T>::operator=(const ImageTwoPtCorr& other)
{
  if (this != &other) {
  }
  return *this;
}


template <class T> 
void ImageTwoPtCorr<T>::autoCorrelation (casacore::ImageInterface<T>& imOut,
                                         const casacore::ImageInterface<T>& imIn,
                                         const casacore::IPosition& axes, 
                                         typename casacore::LatticeTwoPtCorr<T>::Method method,
                                         casacore::Bool progress) const
{
   AlwaysAssert(imIn.shape().nelements()==imOut.shape().nelements(), casacore::AipsError);
   casacore::LogIO os(casacore::LogOrigin("ImageTwoPtCorr", "structureFunction(...)", WHERE));

// Deal with axes

   casacore::CoordinateSystem cSysIn = imIn.coordinates();
   casacore::IPosition axes2 = setUpAxes (axes, cSysIn);

// Compute

   casacore::LatticeTwoPtCorr<T> twoPt;
   twoPt.autoCorrelation (imOut, imIn, axes2, method, progress);

// Overwrite output casacore::Coordinate System

   setCoordinateSystem (imOut, imIn, axes2);

// Copy Miscellaneous stuff

   copyMiscellaneous(imOut, imIn);

// Set Output Units

   setUnit(imOut);
}

template <class T> 
void ImageTwoPtCorr<T>::autoCorrelation (casacore::ImageInterface<T>& imOut,
                                         const casacore::ImageInterface<T>& imIn, 
                                         typename casacore::LatticeTwoPtCorr<T>::Method method,
                                         casacore::Bool progress) const
{
   casacore::IPosition axes;
   autoCorrelation (imOut, imIn, axes, method, progress);
}

// Private functions


template <class T> 
void ImageTwoPtCorr<T>::copyMiscellaneous (casacore::ImageInterface<T>& out,
                                        const casacore::ImageInterface<T>& in) const
{
   out.setMiscInfo(in.miscInfo());
   out.setImageInfo(in.imageInfo());
   out.appendLog(in.logger());
}

template <class T> 
void ImageTwoPtCorr<T>::setUnit (casacore::ImageInterface<T>& im) const
{

// Set image type to undefined

   casacore::ImageInfo info = im.imageInfo();
   info.setImageType (casacore::ImageInfo::Undefined);
   im.setImageInfo(info);

// Set unit to 'count'

   im.setUnits (casacore::Unit("count"));
}

template <class T> 
void ImageTwoPtCorr<T>::setCoordinateSystem (casacore::ImageInterface<T>& out,
                                          const casacore::ImageInterface<T>& in,
                                          const casacore::IPosition& axes) const
//
// Set the centre of the image to be refval=0 and the reference pixel
// 
{
   casacore::CoordinateSystem cSys = in.coordinates();
   casacore::IPosition shape = out.shape();
//
   casacore::Bool holdsOne;
   if (casacore::CoordinateUtil::holdsSky (holdsOne, cSys, axes.asVector())) {

// Replace DC by a LC

      casacore::String error;
      casacore::Int dC;
      casacore::Vector<casacore::Int> pixelAxes, worldAxes;
      casacore::CoordinateUtil::findSky (error, dC, pixelAxes, worldAxes, cSys);
      casacore::DirectionCoordinate dCoord = cSys.directionCoordinate(dC);
//
      casacore::Vector<casacore::String> names(2);
      names(0) = casacore::String("Lag-") + dCoord.worldAxisNames()(0);
      names(1) = casacore::String("Lag-") + dCoord.worldAxisNames()(1);
//
      casacore::Vector<casacore::String> units(dCoord.worldAxisUnits().copy());
//
      casacore::Vector<casacore::Double> refVal(2);
      refVal = 0.0;
//
      casacore::Vector<casacore::Double> inc(dCoord.increment().copy());
      casacore::Matrix<casacore::Double> pc(2,2); 
      pc= 0.0; 
      pc.diagonal() = 1.0;
//
      casacore::Vector<casacore::Double> refPix(2);
      refPix(0) = (shape(pixelAxes(0))-1) / 2;       // shape is odd (n = 2*lmax+1)
      refPix(1) = (shape(pixelAxes(1))-1) / 2;
//
      casacore::LinearCoordinate lCoord (names, units, refVal, inc, pc, refPix);
      cSys.replaceCoordinate (lCoord, dC);
   } else {
      casacore::Vector<casacore::Double> refVal(cSys.referenceValue());    // Refererence 
      refVal(axes(0)) = 0.0;
      refVal(axes(1)) = 0.0;
//
      casacore::Vector<casacore::Double> refPix(cSys.referencePixel());      
      refPix(axes(0)) = (shape(axes(0))-1) / 2;       // shape is odd (n = 2*lmax+1)
      refPix(axes(1)) = (shape(axes(1))-1) / 2;
  }

// Set in output image

   out.setCoordinateInfo(cSys);
}


template <class T> 
IPosition ImageTwoPtCorr<T>::setUpAxes (const casacore::IPosition& axes, 
                                  const casacore::CoordinateSystem& cSys) 
{
   casacore::LogIO os(casacore::LogOrigin("ImageTwoPtCorr", "setUpAxes(...)", WHERE));
   casacore::IPosition axes2(2);

// Find the Sky

   casacore::String error;
   casacore::Int dC;
   casacore::Vector<casacore::Int> pixelAxes, worldAxes;
   casacore::Bool foundSky = casacore::CoordinateUtil::findSky(error, dC, pixelAxes, worldAxes, cSys);
//
   if (axes.nelements()==0) {
      os << casacore::LogIO::NORMAL << "Selected Sky axes" << casacore::LogIO::POST;
      if (foundSky) {
         axes2(0) = pixelAxes(0);
         axes2(1) = pixelAxes(1);
      } else {
         axes2(0) = 0;
         axes2(1) = 1;
      }
   } else if (axes.nelements()==2) {
      os << casacore::LogIO::NORMAL << "Selected first two axes" << casacore::LogIO::POST;
      axes2(0) = axes(0);
      axes2(1) = axes(1);
   } else {
      os << "The axes argument must be of length 0 or 2" << casacore::LogIO::EXCEPTION;
   }

// Make sure we have all of the sky.  We do this because it makes it
// much easier to replace the DC by a LC in the CoordinateSystem.
// In principle we can code around this limitation.

   if (axes.nelements()==2) {
     if (foundSky) {
       casacore::Bool bad = (axes2(0)==pixelAxes(0) && axes2(1)!=pixelAxes(1)) ||
                  (axes2(0)==pixelAxes(1) && axes2(1)!=pixelAxes(0));
       if (bad) {
          os << "You cannot specify just one of the DirectionCoordinate (sky) axes" << casacore::LogIO::EXCEPTION;
       }
     }
   }
//
   return axes2;
}

template <class T> 
IPosition ImageTwoPtCorr<T>::setUpShape (const casacore::IPosition& inShape, const casacore::IPosition& axes)
{
   return casacore::LatticeTwoPtCorr<T>::setUpShape (inShape, axes);
}


} //# NAMESPACE CASA - END

