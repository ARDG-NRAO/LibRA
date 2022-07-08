//# ImageHistograms.cc: generate histograms from an image
//# Copyright (C) 1996,1997,1998,1999,2000,2001,2003
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
//# $Id: ImageHistograms.tcc 19941 2007-02-27 05:36:58Z Malte.Marquarding $

#include <imageanalysis/ImageAnalysis/ImageHistograms.h>

#include <casacore/casa/aips.h>
#include <casacore/images/Images/ImageInterface.h>
#include <casacore/images/Images/ImageUtilities.h>
#include <casacore/lattices/LatticeMath/LatticeHistograms.h>
#include <casacore/casa/IO/ArrayIO.h>
#include <casacore/casa/Logging/LogIO.h>
#include <casacore/casa/BasicSL/String.h>

#include <sstream>

using namespace casacore;

namespace casa { //# NAMESPACE CASA - BEGIN

// Public functions

template <class T>
ImageHistograms<T>::ImageHistograms (const casacore::ImageInterface<T>& image, 
                                     casacore::LogIO &os,
                                     casacore::Bool showProgress,
                                     casacore::Bool forceDisk)
: casacore::LatticeHistograms<T>(image, os, showProgress, forceDisk),
  pInImage_p(0)
{
   if (!this->setNewImage(image)) {
      os_p << error_p << casacore::LogIO::EXCEPTION;
   }
}


template <class T>
ImageHistograms<T>::ImageHistograms (const casacore::ImageInterface<T>& image, 
                                     casacore::Bool showProgress,
                                     casacore::Bool forceDisk)
: casacore::LatticeHistograms<T>(image, showProgress, forceDisk),
  pInImage_p(0)
{
   if (!this->setNewImage(image)) {
      os_p << error_p << casacore::LogIO::EXCEPTION;
   }
}

 
template <class T>
ImageHistograms<T>::ImageHistograms(const ImageHistograms<T> &other)
: casacore::LatticeHistograms<T>(other),
  pInImage_p(0)
{
   if (pInImage_p!=0) delete pInImage_p;
   pInImage_p = other.pInImage_p->cloneII();
}      


template <class T>
ImageHistograms<T> &ImageHistograms<T>::operator=(const ImageHistograms<T> &other)
{
   if (this != &other) {
      casacore::LatticeHistograms<T>::operator=(other);
//
      if (pInImage_p!=0) delete pInImage_p;
      pInImage_p = other.pInImage_p->cloneII();
   }
   return *this;
}

 

template <class T>
ImageHistograms<T>::~ImageHistograms()
{
   delete pInImage_p;
   pInImage_p = 0;
}

template <class T>
Bool ImageHistograms<T>::setNewImage(const casacore::ImageInterface<T>& image)
{
   if (!goodParameterStatus_p) {
      return false;
   }
      
// Make a clone of the image
      
   if (pInImage_p!=0) delete pInImage_p;
   pInImage_p = image.cloneII();
      
      
// Pass it on to LatticeHistograms
   
   goodParameterStatus_p = this->setNewLattice(image);
//  
   return goodParameterStatus_p;
}

template <class T>
String ImageHistograms<T>::writeCoordinates(const casacore::IPosition& histPos) const
{
   ostringstream oss;
   const casacore::Int nDisplayAxes = displayAxes_p.nelements();
   if (nDisplayAxes > 0) {
      casacore::Vector<casacore::String> sWorld(1);
      casacore::Vector<casacore::Double> pixels(1);
      casacore::IPosition blc(pInImage_p->ndim(),0);
      casacore::IPosition trc(pInImage_p->shape()-1);
//
      casacore::CoordinateSystem cSys = pInImage_p->coordinates();
      for (casacore::Int j=0; j<nDisplayAxes; j++) {
         const casacore::Int worldAxis =cSys.pixelAxisToWorldAxis(displayAxes_p(j));
         const casacore::String name = cSys.worldAxisNames()(worldAxis);

// Get pixel coordinate relative to current lattice

         pixels(0) = casacore::Double(locHistInLattice(histPos,false)(j+1));
//
         if (casacore::ImageUtilities::pixToWorld (sWorld, cSys,
                                         displayAxes_p(j), cursorAxes_p,
                                         blc, trc, pixels, -1)) {
            oss <<  casacore::ImageUtilities::shortAxisName(name)
                << "=" << locHistInLattice(histPos,true)+1 
                << " (" << sWorld(0) << ")";
            if (j < nDisplayAxes-1) oss << ", ";
         } else {
            oss << "Axis " << displayAxes_p(j) + 1 
                << "=" << locHistInLattice(histPos,true)+1;
            if (j < nDisplayAxes-1) oss << ", ";
         }
      }
   }
//
   return casacore::String(oss);
}



} //# NAMESPACE CASA - END

