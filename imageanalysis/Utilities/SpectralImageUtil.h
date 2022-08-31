//# SpectralImageUtil.h: Definitions for casacore::Stokes Image utilities
//# Copyright (C) 2013
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
#ifndef SYNTHESIS_SPECTRALIMAGEUTIL_H
#define SYNTHESIS_SPECTRALIMAGEUTIL_H

#include <casacore/images/Images/ImageInterface.h>
#include <casacore/images/Images/SubImage.h>

namespace casa { //# NAMESPACE CASA - BEGIN

class SpectralImageUtil {

 public:
  //Returns a pointer to a subimage of the requested plane
  //Note caller is responsible for deleting the subimage object
  // If writeaccess is true ...modification on the subimage will get recorded on the original image if it is writeable
  template <typename T> static casacore::SubImage<T>* getChannel(casacore::ImageInterface<T>& theIm, casacore::Int begchannel=0,  casacore::Int endchannel=-1, casacore::Bool writeAccess=false);
  //Frequency at a given pixel along the spectral axis (value returned is in Hz)
  static casacore::Double worldFreq(const casacore::CoordinateSystem& cs, casacore::Double spectralpix=0.0);

};//end of class

#include <imageanalysis/Utilities/SpectralImageUtil.tcc>
} //# NAMESPACE CASA - END

#endif

