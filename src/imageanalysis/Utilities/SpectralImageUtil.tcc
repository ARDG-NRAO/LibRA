//# SpectralImageUtil.cc: Spectral Image Utilities
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



template <typename T>  casacore::SubImage<T>* SpectralImageUtil::getChannel(casacore::ImageInterface<T>& theIm, casacore::Int beginchannel, casacore::Int endchannel, casacore::Bool writeAccess){
    casacore::CoordinateSystem csys=theIm.coordinates();
    if(endchannel<0 )
      endchannel=beginchannel;
    if(beginchannel > endchannel){ 
      casacore::Int temp=endchannel;
      endchannel=beginchannel;
      beginchannel=temp;
    }
    casacore::Int spectralIndex=casacore::CoordinateUtil::findSpectralAxis(csys);
    casacore::IPosition blc(theIm.shape());
    casacore::IPosition trc(theIm.shape());
    blc-=blc; //set all values to 0
    trc=theIm.shape();
    trc-=1; // set trc to image size -1
    if(beginchannel > trc[spectralIndex] || beginchannel <0 || endchannel >  trc[spectralIndex] )
      throw(casacore::AipsError("Channel requested does not exist"));
    blc[spectralIndex]=beginchannel;
    trc[spectralIndex]=endchannel;
    casacore::Slicer sl(blc, trc, casacore::Slicer::endIsLast);
    casacore::SubImage<T> * imageSub=new casacore::SubImage<T>(theIm, sl, writeAccess);
    return imageSub;
  };


 

