//# WFCleanImageSkyModel.h: Definition for WFCleanImageSkyModel
//# Copyright (C) 1996,1997,1998,1999,2002
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

#ifndef SYNTHESIS_WFCLEANIMAGESKYMODEL_H
#define SYNTHESIS_WFCLEANIMAGESKYMODEL_H

#include <synthesis/MeasurementComponents/MFCleanImageSkyModel.h>
namespace casacore{

template <class T> class SubImage;
}

namespace casa { //# NAMESPACE CASA - BEGIN

//forward
class SkyEquation;

// <summary> 
// WF Image Sky Model: Image Sky Model implementing the WF Clean algorithm
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//   <li> <linkto class=ImageSkyModel>ImageSkyModel</linkto> module
//   <li> <linkto class=MFCleanImageSkyModel>MFCleanImageSkyModel</linkto> module
// </prerequisite>
//
// <etymology>
// WFCleanImageSkyModel implements the WF Clean algorithm.
// It is derived from <linkto class=MFCleanImageSkyModel>MFCleanImageSkyModel</linkto>.
// </etymology>
//
// <synopsis> 
// The WF Clean is an FFT-based clean algorithm. Cleaning is
// split into major and minor cycles. In a minor cycle, the
// brightest pixels are cleaned using only the strongest sidelobes
// (and main lobe) of the PSF. In the major cycle, a fully correct
// subtraction of the PSF is done for all points accumulated in the
// minor cycle using an FFT-based convolution for speed.
//
// The WF Clean is implemented using the 
// <linkto class=MFCleanImageSkyModel>MFCleanImageSkyModel</linkto>
// class.
//
// Masking is optionally performed using a mask image: only points
// where the mask is non-zero are cleaned. If no mask is specified
// all points in the inner quarter of the image are cleaned.
// </synopsis> 
//
// <example>
// See the example for <linkto class=SkyModel>SkyModel</linkto>.
// </example>
//
// <motivation>
// </motivation>
//
// <todo asof="97/10/01">
// <ul> Allow specification of more control parameters
// </todo>

class WFCleanImageSkyModel : public MFCleanImageSkyModel {
public:

  WFCleanImageSkyModel();

  virtual ~WFCleanImageSkyModel();

  WFCleanImageSkyModel(const casacore::Int nfacets, casacore::Bool largeMemory=false);

  virtual casacore::Int add(casacore::ImageInterface<casacore::Float>& image, const casacore::Int maxNumXfr=100);

  virtual casacore::Bool addResidual(casacore::Int image, casacore::ImageInterface<casacore::Float>& residual);

  virtual casacore::Bool addMask(casacore::Int image, casacore::ImageInterface<casacore::Float>& mask);

  // Solve for this SkyModel
  virtual casacore::Bool solve (SkyEquation& me);

  //This will return for model 0 the main residual image i.e nxn facets
  virtual casacore::ImageInterface<casacore::Float>& getResidual(casacore::Int physImageID);

  //void makeApproxPSFs(SkyEquation& se);
  
  /*
  casacore::PtrBlock<casacore::TempImage<casacore::Complex> *>& cimageBlkPtr() { return cimage_p;};
  casacore::PtrBlock<casacore::Matrix<casacore::Float> * >& weightBlkPtr(){return weight_p; } ;
  */
protected:

  // Make a facet
  casacore::SubImage<casacore::Float>* makeFacet(casacore::Int facet, casacore::ImageInterface<casacore::Float>& image);

  // Make the slicers
  casacore::Bool makeSlicers(const casacore::Int facet, const casacore::IPosition& imageShape, 
		   casacore::IPosition& facetShape,
		   casacore::Slicer& imageSlicer);
  // Number of facets
  casacore::Int nfacets_p;


private:

  casacore::CountedPtr<casacore::ImageInterface<casacore::Float> > imageImage_p;
  casacore::CountedPtr<casacore::ImageInterface<casacore::Float> > residualImage_p;
  casacore::CountedPtr<casacore::ImageInterface<casacore::Float> > maskImage_p;

  casacore::Int facets_p;
  //casacore::Bool largeMem_p;

  // Here we store the facets for the various images: Model, Mask
  // and Residual
  casacore::Block<casacore::CountedPtr<casacore::SubImage<casacore::Float> > > facetImages_p;
  casacore::Block<casacore::CountedPtr<casacore::SubImage<casacore::Float> > > facetMaskImages_p;
  casacore::Block<casacore::CountedPtr<casacore::SubImage<casacore::Float> > > facetResidualImages_p;
  
};


} //# NAMESPACE CASA - END

#endif


