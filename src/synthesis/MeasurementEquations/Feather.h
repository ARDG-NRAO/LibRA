//# Feather.h: Helper for feathering
//# Copyright (C) 1996-2012
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
#ifndef SYNTHESIS_FEATHER_H
#define SYNTHESIS_FEATHER_H

#include <casacore/casa/BasicSL/String.h>
#include <casacore/casa/BasicSL.h>
#include <casacore/casa/Quanta/Quantum.h>
#include <casacore/scimath/Mathematics/GaussianBeam.h>
#include <casacore/coordinates/Coordinates/CoordinateSystem.h>
#include <casacore/casa/Arrays/ArrayFwd.h>

namespace casacore{

  template<class T> class ImageInterface;

}

namespace casa { //# NAMESPACE CASA - BEGIN
  //Forward declaration
  // <summary> Class that contains functions needed for feathering</summary>

  class Feather{

  public:

    //default constructor
    Feather();
    //Constructor 
    Feather(const casacore::ImageInterface<casacore::Float>& SDImage, const casacore::ImageInterface<casacore::Float>& INTImage, casacore::Float sdScale=1.0);
    //Destructor
    virtual ~Feather();

    //Clear the weight flags.  Used when changing between the
    //SDimage and the dirty image.
    void clearWeightFlags();

    //set the SDimage and casacore::Int images 
    void setSDScale(casacore::Float sdscale=1.0);
    void setSDImage(const casacore::ImageInterface<casacore::Float>& SDImage);
    void setINTImage(const casacore::ImageInterface<casacore::Float>& IntImage);
    //set and get effective dish diameter to be used in feathering function
    //setEffectiveDishDiam will return false if you are trying to assign a finer resolution than what 
    //the original data came with 
    casacore::Bool setEffectiveDishDiam(const casacore::Float xdiam, const casacore::Float ydiam=-1.0);
    void getEffectiveDishDiam(casacore::Float& xdiam, casacore::Float& ydiam);
    //Get the 1-D slices of amplitude  along the x and y axis of the FFT of images  
    //if radial is set to true then the 1D slice is the the circular average rather that X and Y cuts...only the x-values are valid then 
    //Note the SD image is already feathered by its beam..you cannot get unfeathered SD data as it implies deconvolution
    void getFTCutSDImage(casacore::Vector<casacore::Float>& ux, casacore::Vector<casacore::Float>& xamp, casacore::Vector<casacore::Float>& uy, casacore::Vector<casacore::Float>& yamp, const casacore::Bool radial=false );
    void getFTCutIntImage(casacore::Vector<casacore::Float>& ux, casacore::Vector<casacore::Float>& xamp, casacore::Vector<casacore::Float>& uy, casacore::Vector<casacore::Float>& yamp, casacore::Bool radial=false);
    //Get the 1-D slices of the feathering function that will be applied on SD and INTerf data
    //If normalize=false for the SD then the value for Jy/beam correction to final beam size is multiplied to 
    //the function
    void getFeatherSD(casacore::Vector<casacore::Float>& ux, casacore::Vector<casacore::Float>& xamp, casacore::Vector<casacore::Float>& uy, casacore::Vector<casacore::Float>& yamp, casacore::Bool radial=false, casacore::Bool normalize=true);
    void getFeatherINT(casacore::Vector<casacore::Float>& ux, casacore::Vector<casacore::Float>& xamp, casacore::Vector<casacore::Float>& uy, casacore::Vector<casacore::Float>& yamp, casacore::Bool radial=false);
    //Get 1-D slices of the feathered data ...
    //note for SD  this
    //should return the same values as getFTCutSDImage
    void getFeatheredCutSD(casacore::Vector<casacore::Float>& ux, casacore::Vector<casacore::Float>& xamp, casacore::Vector<casacore::Float>& uy, casacore::Vector<casacore::Float>& yamp, casacore::Bool radial=false);
    void getFeatheredCutINT(casacore::Vector<casacore::Float>& ux, casacore::Vector<casacore::Float>& xamp, casacore::Vector<casacore::Float>& uy, casacore::Vector<casacore::Float>& yamp, casacore::Bool radial=false);
    //This function convolves the INT image to the new GaussianBeam
    //So the INT image stored in this object is going to be replaced by the new convolved image
    void convolveINT(const casacore::GaussianBeam& newHighBeam);

    //write the feathered image to disk
    casacore::Bool saveFeatheredImage(const casacore::String& imagename);
    ///////Helper function to get XY slices and radial cuts for any generic image
    static void getCutXY(casacore::Vector<casacore::Float>& ux, casacore::Vector<casacore::Float>& xamp, 
			 casacore::Vector<casacore::Float>& uy, casacore::Vector<casacore::Float>& yamp, 
			 const casacore::ImageInterface<casacore::Float>& image);
    static void getRadialCut(casacore::Vector<casacore::Float>& radius, casacore::Vector<casacore::Float>& radialAmp, 
			  const casacore::ImageInterface<casacore::Float>& image);


    ///////////////Old methods left for now till new version is implemented
    static void feather(const casacore::String& image, const casacore::ImageInterface<casacore::Float>& high, const casacore::ImageInterface<casacore::Float>& low, const casacore::Float& sdScale=1.0, const casacore::String& lowPSF="", const casacore::Bool useDefault=true, const casacore::String& vpTable="" , casacore::Float effSDDiam=-1.0, const casacore::Bool lowpassfiltersd=false);

    static casacore::Double worldFreq(const casacore::CoordinateSystem& cs, casacore::Int spectralpix=0);

    
  private:
    void fillXVectors( casacore::Vector<casacore::Float>& ux, casacore::Vector<casacore::Float>& uy ) const;
    static void applyDishDiam(casacore::ImageInterface<casacore::Complex>& image, casacore::GaussianBeam& beam, casacore::Float effDiam, casacore::ImageInterface<casacore::Float>& newbeam, casacore::Vector<casacore::Quantity>& extraconv);


    
  
    casacore::CountedPtr<casacore::ImageInterface<casacore::Float> > lowIm_p;
    casacore::CountedPtr<casacore::ImageInterface<casacore::Float> > lowImOrig_p;
    casacore::CountedPtr<casacore::ImageInterface<casacore::Float> > highIm_p;
    casacore::CountedPtr<casacore::ImageInterface<casacore::Complex> > cwImage_p;
    casacore::CountedPtr<casacore::ImageInterface<casacore::Complex> > cwHighIm_p;
    static void getCutXY(casacore::Vector<casacore::Float>& ux, casacore::Vector<casacore::Float>& xamp, 
		  casacore::Vector<casacore::Float>& uy, casacore::Vector<casacore::Float>& yamp, casacore::ImageInterface<casacore::Complex>& ftimage);
    static void getRadialCut(casacore::Vector<casacore::Float>& radialAmp, casacore::ImageInterface<casacore::Complex>& ftimage);
    //calculate the complex weight image to apply on the interf image
    static void getLowBeam(const casacore::ImageInterface<casacore::Float>& low0, const casacore::String& lowPSF, const casacore::Bool useDefaultPB, const casacore::String& vpTableStr, casacore::GaussianBeam& lBeam);
    void calcCWeightImage();
    void applyFeather();
    static void getRadialUVval(const casacore::Int npix, const casacore::IPosition& imshape, const casacore::CoordinateSystem& csys, casacore::Vector<casacore::Float>& radius);
    casacore::GaussianBeam hBeam_p;
    casacore::GaussianBeam lBeam_p;
    casacore::GaussianBeam lBeamOrig_p;
    casacore::Float dishDiam_p;
    casacore::Bool cweightCalced_p;
    casacore::Bool cweightApplied_p;
    casacore::Float sdScale_p;
    casacore::CoordinateSystem csysHigh_p;
  };



} //# NAMESPACE CASA - END

#endif
