//# SkyModel.h: Definition for SkyModel
//# Copyright (C) 1996,1997,1998,1999,2000,2002
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

#ifndef SYNTHESIS_SKYMODEL_H
#define SYNTHESIS_SKYMODEL_H

#include <casa/aips.h>
#include <synthesis/MeasurementEquations/Iterate.h>
#include <synthesis/TransformMachines/StokesImageUtil.h>
#include <images/Images/ImageInterface.h>
#include <components/ComponentModels/ComponentList.h>
#include <casa/BasicSL/String.h>

namespace casa { //# NAMESPACE CASA - BEGIN

//forward declarations
class SkyEquation;

// <summary> 
// Sky Model: Model the Sky Brightness for the SkyEquation
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//   <li> casacore::Matrix module
//   <li> casacore::Vector module
//   <li> MeasurementComponents module
//   <li> VisSet module
// </prerequisite>
//
// <etymology>
// SkyModel describes an interface for Models to be used in
// the SkyEquation. It is an Abstract Base Class: most methods
// must be defined in derived classes.
// </etymology>
//
// <synopsis> 
// A SkyModel contains a number of separate models. The interface to
// SkyEquation is via an image per model. SkyEquation uses this image to
// calculate Fourier transforms, etc. Some (most) SkyModels are
// solvable: the SkyEquation can be used by the SkyModel to return
// gradients with respect to itself (via the image interface). Thus
// for a SkyModel to solve for itself, it calls the SkyEquation
// methods to get gradients of chi-squared with respect to the
// image pixel values (thus returning an image: basically a residual
// image). The SkyModel then uses these gradients as appropriate to
// update itself.
//
// The following examples illustrate how a SkyModel can  be
// used:
// <ul>
// <li> Simple cleaning: one model. The gradient gives the
// residual image. A special method gives a PSF.
// <li> Cleaning with visibility-based subtraction: one model. The
// gradient can be calculated as needed (using the SkyEquation)
// to produce the correct residual image.
// <li> Wide-field imaging: one model per patch of the sky
// that is to be imaged.
// <li> Non-coplanar baselines imaging: one model per facet of
// the polyhedron. At the end of processing all facets are combined
// into one overall image.
// <li> Mosaicing: one model per primary beam pointing. Each model
// is derived (as needed) by cutting out a patch from the full-field
// mosaic.
// </ul>
// </synopsis> 
//
// <example>
// <srcblock>
//      // Read the VisSet from disk
//      VisSet vs("3c84.MS");
//
//      // Create an ImageSkyModel from an image on disk
//      ImageSkyModel ism(casacore::PagedImage<casacore::Float>("3c84.modelImage"));
//
//      // Make an FTMachine: here we use a simple Grid and FT.
//      GridFT ft;
//
//      SkyEquation se(ism, vs, ft);
//
//      // Predict the visibility set
//      se.predict();
//
//      // Make a Clean Image and write it out
//      HogbomCleanImageSkyModel csm(ism);
//      if (csm.solve()) {
//        casacore::PagedImage<casacore::Float> cleanImage=csm.image(0);
//        cleanImage.setName("3c84.cleanImage");
//      }
//
// </srcblock>
// </example>
//
// <motivation>
// The properties of a model of the sky must be described
// for the SkyEquation.
// </motivation>
//
// <todo asof="97/10/01">
// <li> Multiple images in SkyModel
// <li> ComponentModel
// </todo>

class SkyModel : public Iterate {

public:
  
  enum PolRep {
    CIRCULAR=StokesImageUtil::CIRCULAR,
    LINEAR=StokesImageUtil::LINEAR
  };

  SkyModel() : itsAlgorithm(""), itsSubAlgorithm(""), imageRegion_p(0), isImageNormalized_p(false) { }  

  // Number of models contained
  virtual casacore::Int numberOfModels() = 0;

  // MFS : Number of taylor terms per model
  virtual casacore::Int numberOfTaylorTerms() = 0;

  // MFS : Reference Frequency
  virtual casacore::Double getReferenceFrequency() = 0;

  // MFS : Index of Taylor term in array of nmodels x ntaylorterms
  virtual casacore::Int getTaylorIndex(casacore::Int index=0) = 0;

  // Is this SkyModel solveable?
  virtual casacore::Bool isSolveable(casacore::Int model=0) = 0;

  // Is there a flux scale image associated with this model?
  virtual casacore::Bool doFluxScale(casacore::Int model=0) = 0;

  // Initialize for gradient search
  virtual void initializeGradients() = 0;

  // Finalize for gradient search
  virtual void finalizeGradients() = 0;

  // Return the component list
  virtual ComponentList& componentList() = 0;

  // Return the component list
  virtual casacore::Bool hasComponentList() = 0;

  // Image interface for this model (casacore::Stokes representation)
  virtual casacore::ImageInterface<casacore::Float>& image(casacore::Int model=0) = 0;

  // Increment in the image
  virtual casacore::ImageInterface<casacore::Float>& deltaImage(casacore::Int model=0) = 0;

  // casacore::Complex image (needed for e.g. RR,RL,LR,LL)
  virtual casacore::ImageInterface<casacore::Complex>& cImage(casacore::Int model=0) = 0;

  // casacore::Complex XFR
  virtual casacore::ImageInterface<casacore::Complex>& XFR(casacore::Int model=0, casacore::Int numXFR=0) = 0;
  virtual casacore::Bool hasXFR(casacore::Int model=0) = 0;

  // PSF 
  virtual casacore::ImageInterface<casacore::Float>& PSF(casacore::Int model=0) = 0;

  // Gradient of chi-squared wrt pixels
  virtual casacore::ImageInterface<casacore::Float>& gS(casacore::Int model=0) = 0;

  // Grad Grad chi-squared wrt pixels (diagonal elements only)
  virtual casacore::ImageInterface<casacore::Float>& ggS(casacore::Int model=0) = 0;

  // FluxScale image:   image * fluxScale => true brightness distribution
  virtual casacore::ImageInterface<casacore::Float>& fluxScale(casacore::Int model=0) = 0;
  
  // Work image
  virtual casacore::ImageInterface<casacore::Float>& work(casacore::Int model=0) = 0;

  // Add to Sum weights, Chi-Squared
  virtual void addStatistics(casacore::Float sumwt, casacore::Float chisq) = 0;

  // Weight per model (channels, polarizations)
  virtual casacore::Matrix<casacore::Float>& weight(casacore::Int model=0) = 0;

  // Solve for this SkyModel
  virtual casacore::Bool solve (SkyEquation& se) = 0;

  // Is this model empty
  virtual casacore::Bool isEmpty(casacore::Int model=0) = 0;

  virtual casacore::Int getModelIndex(casacore::uInt field=0, casacore::uInt taylor=0) = 0;

  //set Algorithm (e.g clean, mem, nnls)
  void setAlgorithm(const casacore::String& alg) {itsAlgorithm = alg;}
    
  // get  Algorithm
  const casacore::String getAlgorithm() { return itsAlgorithm; }

  // set Sub Algorithm
  void setSubAlgorithm(const casacore::String& alg) { itsSubAlgorithm = alg; }

  // get Sub Algorithm
  const casacore::String getSubAlgorithm() { return itsSubAlgorithm; }

  // Set the imageregion that will be used for the next XFR generation
  // <group>
  void setImageRegion( casacore::ImageRegion& ir ) { imageRegion_p = &ir; }
  // use the default shape
  void unsetImageRegion() { imageRegion_p = 0; }
  // </group>

  void setImageNormalization(casacore::Bool val) {isImageNormalized_p = val;};
  casacore::Bool isImageNormalized() {return isImageNormalized_p;};

  //set and get memory usage model
  virtual void setMemoryUse(casacore::Bool memuse)=0;
  virtual casacore::Bool getMemoryUse()=0;
  
protected:
  casacore::String itsAlgorithm;
  casacore::String itsSubAlgorithm;
  // this casacore::ImageRegion is used to suggest the shape for the
  // XFR.  If null, then just use the shape of image(model)
  casacore::ImageRegion *imageRegion_p;
  casacore::Bool isImageNormalized_p;

private:
};


} //# NAMESPACE CASA - END

#endif
