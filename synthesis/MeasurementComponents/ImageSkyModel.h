//# ImageSkyModel.h: Definition for ImageSkyModel
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

#ifndef SYNTHESIS_IMAGESKYMODEL_H
#define SYNTHESIS_IMAGESKYMODEL_H

#include <ms/MeasurementSets/MeasurementSet.h>
#include <synthesis/MeasurementComponents/SkyModel.h>
#include <casa/Arrays/Matrix.h>
#include <images/Images/ImageInterface.h>
#include <images/Images/PagedImage.h>
#include <images/Images/TempImage.h>
#include <casa/Logging/LogMessage.h>
#include <casa/Logging/LogSink.h>
#include <casa/System/PGPlotter.h>

namespace casa { //# NAMESPACE CASA - BEGIN

// <summary> 
// Image Sky Model: Image-based Model for the Sky Brightness
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="" demos="">

// <prerequisite>
//   <li> <linkto class=SkyModel>SkyModel</linkto> class
//   <li> <linkto class=SkyEquation>SkyEquation</linkto> class
//   <li> <linkto class=casacore::ImageInterface>ImageInterface</linkto> class
//   <li> <linkto class=casacore::PagedImage>PagedImage</linkto> class
//   <li> <linkto module=MeasurementComponents>MeasurementComponents</linkto> module
//   <li> <linkto class=VisSet>VisSet</linkto> class
// </prerequisite>
//
// <etymology>
// ImageSkyModel describes an interface for Models to be used in
// the SkyEquation. It is derived from <linkto class=SkyModel>SkyModel</linkto>.
// </etymology>
//
// <synopsis> 
// A ImageSkyModel contains a number of separate models. The interface to
// SkyEquation is via an image per model. <linkto class=SkyEquation>SkyEquation</linkto> uses this image to
// calculate Fourier transforms, etc. Some (most) SkyModels are
// solvable: the SkyEquation can be used by the SkyModel to return
// gradients with respect to itself (via the image interface). Thus
// for a SkyModel to solve for itself, it calls the SkyEquation
// methods to get gradients of chi-squared with respect to the
// image pixel values (thus returning an image: basically a residual
// image). The SkyModel then uses these gradients as appropriate to
// update itself.
// </synopsis> 
//
// <example>
// See the example for <linkto class=SkyModel>SkyModel</linkto>.
// </example>
//
// <motivation>
// The properties of a model of the sky must be described
// for the <linkto class=SkyEquation>SkyEquation</linkto>.
// </motivation>
//
// <todo asof="97/10/01">
// <li> Multiple images in SkyModel
// <li> ComponentModel
// </todo>

class ImageSkyModel : public SkyModel {
public:

  // Empty constructor
  ImageSkyModel(const casacore::Int maxNumModels=1);

  void setMaxNumberModels(const casacore::Int maxNumModels);

  // Copy constructor
  ImageSkyModel(const ImageSkyModel& sm);

  // Add a componentlist
  virtual casacore::Bool add(ComponentList& compList);
  //update componentlist
  virtual casacore::Bool updatemodel(ComponentList& compList);


  // Add an image. maxNumXfr is the maximum Number of transfer functions
  // that we might want to associate with this image.
  virtual casacore::Int add(casacore::ImageInterface<casacore::Float>& image, const casacore::Int maxNumXfr=100);
  //update model image...you have to have added it before...nmodels_p held has to be bigger that image here
  //its left to the caller to make sure the image is conformant...otherwise you are in trouble.
  virtual casacore::Bool  updatemodel(const casacore::Int thismodel, casacore::ImageInterface<casacore::Float>& image);
  // Add a residual image
  virtual casacore::Bool addResidual(casacore::Int image, casacore::ImageInterface<casacore::Float>& residual);

  // Destructor
  virtual ~ImageSkyModel();

  // Assignment operator
  ImageSkyModel& operator=(const ImageSkyModel& other);

  // Number of models contained
  virtual casacore::Int numberOfModels() {return nmodels_p;};

  // MFS : Number of taylor terms per model
  virtual casacore::Int numberOfTaylorTerms() {return 1;};

  // MFS : In-place coefficient residual calculations
  virtual casacore::Bool calculateCoeffResiduals(){return false;};

  // MFS : Calculate restored alpha and beta.
  virtual casacore::Bool calculateAlphaBeta(const casacore::Vector<casacore::String>& /*restoredNames*/, const casacore::Vector<casacore::String>& /*residualNames*/){return false;};

  // MFS : Reference Frequency
  virtual casacore::Double getReferenceFrequency(){return 0.0;}

  // MFS : Index of Taylor term in array of nmodels x ntaylorterms
  //virtual casacore::Int getTaylorIndex(casacore::Int index){return 0;}
  virtual casacore::Int getTaylorIndex(casacore::Int index){return (casacore::Int)(index/nfields_p);}

  // Is this model solveable?
  casacore::Bool isSolveable(casacore::Int model=0);

  // Free and fix the model (returns previous status). Free means that
  // it will be solved for in any solution.
  casacore::Bool free(casacore::Int model=0);
  casacore::Bool  fix(casacore::Int model=0);

  // Initialize for gradient search
  virtual void initializeGradients();

  // Finalize for gradient search
  virtual void finalizeGradients() {};

  // Does this have a component list?
  casacore::Bool hasComponentList();

  casacore::Bool isEmpty(casacore::Int model=0);

  // Return the component list
  virtual ComponentList& componentList();

  // Return actual images to be used by SkyEquation. 
  // <group>
  casacore::ImageInterface<casacore::Float>& image(casacore::Int model=0);
  casacore::ImageInterface<casacore::Complex>& cImage(casacore::Int model=0);
  casacore::ImageInterface<casacore::Complex>& XFR(casacore::Int model=0, casacore::Int numXFR=0);
  casacore::ImageInterface<casacore::Float>& PSF(casacore::Int model=0);
  casacore::ImageInterface<casacore::Float>& gS(casacore::Int model=0);
  casacore::ImageInterface<casacore::Float>& residual(casacore::Int model=0);
  casacore::ImageInterface<casacore::Float>& ggS(casacore::Int model=0);
  // if (doFluxScale(mod))  image(mod) *  fluxScale(mod)  
  // gives actual brightness distribution
  casacore::ImageInterface<casacore::Float>& fluxScale(casacore::Int model=0);
  casacore::ImageInterface<casacore::Float>& work(casacore::Int model=0);
  casacore::ImageInterface<casacore::Float>& deltaImage(casacore::Int model=0);
  // </group>

  // tells if this model needs to be multiplied by a flux scale image
  casacore::Bool doFluxScale(casacore::Int model=0);
  // require use of flux scale image
  void mandateFluxScale(casacore::Int model=0);

  casacore::Bool hasXFR(casacore::Int model=0);

  // Add to Sum weights, Chi-Squared
  void addStatistics(casacore::Float sumwt, casacore::Float chisq) {sumwt_p+=sumwt;chisq_p+=chisq;}

  // Weight per model (channels, polarizations)
  casacore::Matrix<casacore::Float>& weight(casacore::Int model=0);

  // Solve for this SkyModel: This replaces the image with
  // the residual image
  virtual casacore::Bool solve (SkyEquation& me);

  // Solve explicitly for the residuals: same as solve for
  // this class
  // modelToMs determines if predicted vis is put in the MODEL_DATA column
  casacore::Bool solveResiduals (SkyEquation& me, casacore::Bool modelToMS=false);

  // Make the approximate PSFs needed for each model
  virtual void makeApproxPSFs(SkyEquation& se);

  // Get current residual image: this is either that image specified via
  // addResidual, or a scratch image.
  // For example in WFImageSkyModel it might return the whole main image
  //rather than facets 
  virtual casacore::ImageInterface<casacore::Float>& getResidual (casacore::Int model=0);

  // Return the fitted beam for each model
  casacore::ImageBeamSet& beam(casacore::Int model=0);

  // Set casacore::PGPlotter to be used
  void setPGPlotter(casacore::PGPlotter& pgp) { pgplotter_p = &pgp; }

  // This is the factor by which you multiply the worst outer
  // sidelobe by to get the threshold for the current cycle
  void setCycleFactor(float x) { cycleFactor_p = x; }

  // Cycle threshold will double in this number of iterations
  // (ie, use a large number if you don't want cycle threshold
  // to inch up)
  void setCycleSpeedup(float x) { cycleSpeedup_p = x; }

  // Yet another control for the minor cycle threshold.
  // This is in response to CAS-2673
  // This allows control similar to 'cyclefactor' - used in MFClarkCleanSkyModel
  void setCycleMaxPsfFraction(float x) { cycleMaxPsfFraction_p = x; }

  // Set the variable that switches on the progress display
  void setDisplayProgress (const casacore::Bool display ) {displayProgress_p = display; };

  // Set a variable to indicate the polarization frame in the data (circular or linear).
  // This is used along with the user's choice of output casacore::Stokes parameter
  // to decide the stokesCoordinate of the temporary images "cImage".
  void setDataPolFrame(StokesImageUtil::PolRep datapolrep) {dataPolRep_p = datapolrep;};

  // Tries to return a pointer to a casacore::TempImage (allocated with new, so remember
  // to use delete) with the given shape and CoordinateSystem.
  //
  // @param imgShp
  // @param imgCoords
  // @param nMouthsToFeed: If > 1 it is taken as a hint that it should leave
  //                       room for nMouthsToFeed - 1 more TempImages. 
  //
  // <throws>
  // casacore::AipsError on memory allocation error.
  // </throws>
  template<class M>
  static casacore::TempImage<M>* getTempImage(const casacore::TiledShape& imgShp,
                                    const casacore::CoordinateSystem& imgCoords,
                                    const casacore::uInt nMouthsToFeed=1);
  
  virtual casacore::Int getModelIndex(casacore::uInt field, casacore::uInt /*taylor*/){return field;};

  //try to make templattices use memory if possible
  //if set to false then always use disk
  virtual void setMemoryUse(casacore::Bool useMem=false);
  virtual casacore::Bool getMemoryUse(){return useMem_p;};
  //Set templattice tile vol  in pixels
  void setTileVol(const casacore::Int tileVol=1000000);
protected:

  // Make Newton Raphson step internally. This is really an implementation
  // detail: it is useful for derived classes.
  // The modelToMS parameter is for committing to MODEL_DATA column of the MS
  // the predicted visibilities.

  casacore::Bool makeNewtonRaphsonStep(SkyEquation& se, 
			     casacore::Bool incremental=false, casacore::Bool modelToMS=false);


  // Get casacore::PGPlotter to be used
  casacore::PGPlotter* getPGPlotter() { return pgplotter_p; }

  casacore::Int maxnmodels_p;
  casacore::Int nmodels_p;
  //MFS
  casacore::Int nfields_p;

  casacore::Int maxNumXFR_p;

  casacore::Float sumwt_p; 
  casacore::Float chisq_p; 

  // ComponentList
  ComponentList* componentList_p;

  // Images
  casacore::Vector<casacore::String> imageNames_p;
  // Everything here can be just interface
  casacore::PtrBlock<casacore::ImageInterface<casacore::Float> * > image_p;
  casacore::PtrBlock<casacore::ImageInterface<casacore::Float> * > residual_p;

  // We actually create these
  casacore::PtrBlock<casacore::ImageInterface<casacore::Complex> * > cimage_p;
  casacore::PtrBlock<casacore::ImageInterface<casacore::Complex> * > cxfr_p;
  casacore::PtrBlock<casacore::ImageInterface<casacore::Float> * > residualImage_p;
  casacore::PtrBlock<casacore::ImageInterface<casacore::Float> * > gS_p;
  casacore::PtrBlock<casacore::ImageInterface<casacore::Float> * > psf_p;
  casacore::PtrBlock<casacore::ImageInterface<casacore::Float> * > ggS_p;
  // if (doFluxScale_p), image_p * fluxScale_p gives the true brightness
  casacore::PtrBlock<casacore::ImageInterface<casacore::Float> * > fluxScale_p;
  casacore::PtrBlock<casacore::ImageInterface<casacore::Float> * > work_p;
  casacore::PtrBlock<casacore::ImageInterface<casacore::Float> * > deltaimage_p;
  casacore::Block<casacore::Bool> solve_p;
  casacore::Block<casacore::Bool> doFluxScale_p;

  casacore::PtrBlock<casacore::Matrix<casacore::Float> * > weight_p;

  casacore::PtrBlock<casacore::ImageBeamSet * > beam_p;

  casacore::LogSink logSink_p;
  casacore::LogSink& logSink() {return logSink_p;};
  
  casacore::Long cacheSize(casacore::Int model);
  casacore::IPosition tileShape(casacore::Int model);

  casacore::PGPlotter *pgplotter_p;
  casacore::Bool displayProgress_p;
  // This is the factor by which you multiply the worst outer
  // sidelobe by to get the threshold for the current cycle
  casacore::Float cycleFactor_p;
  // Cycle threshold will double in this number of iterations
  // (ie, use a large number if you don't want cycle threshold
  // to inch up)
  casacore::Float cycleSpeedup_p;
  // Cycle threshold = maxResidual x min(Max-Psf-Fraction , cyclefactor x maxpsfsidelobe)
  casacore::Float cycleMaxPsfFraction_p;
  // If PSF is done..should not redo it.
  casacore::Bool donePSF_p;
  // check if model has been modified especially for continuing
  // a deconvolution
  casacore::Bool modified_p;
  // Parameter to indicate the polaraization type of the data (circular or linear)
  // Required by cImage() to decide shapes.
  StokesImageUtil::PolRep dataPolRep_p;
  casacore::Bool workDirOnNFS_p;
  casacore::Bool useMem_p;
  casacore::Int tileVol_p;
};



} //# NAMESPACE CASA - END


#ifndef AIPS_NO_TEMPLATE_SRC
#include <synthesis/MeasurementComponents/ImageSkyModel.tcc>
#endif //# AIPS_NO_TEMPLATE_SRC

#endif


